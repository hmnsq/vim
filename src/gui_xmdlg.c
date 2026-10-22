/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

/*
 * (C) 2001,2005 by Marcin Dalecki <martin@dalecki.de>
 *
 * Implementation of dialog functions for the Motif GUI variant.
 *
 * Note about Lesstif: Apparently lesstif doesn't get the widget layout right,
 * when using a dynamic scrollbar policy.
 */

#ضمن "vim.h"

#ضمن <Xm/Form.h>
#ضمن <Xm/PushBG.h>
#ضمن <Xm/Text.h>
#ضمن <Xm/TextF.h>
#ضمن <Xm/Label.h>
#ضمن <Xm/Frame.h>
#ضمن <Xm/LabelG.h>
#ضمن <Xm/ToggleBG.h>
#ضمن <Xm/SeparatoG.h>
#ضمن <Xm/DialogS.h>
#ضمن <Xm/List.h>
#ضمن <Xm/RowColumn.h>
#ضمن <Xm/AtomMgr.h>
#ضمن <Xm/Protocols.h>

#ضمن <X11/keysym.h>
#ضمن <X11/Xatom.h>
#ضمن <X11/StringDefs.h>
#ضمن <X11/Intrinsic.h>

خارجي Widget vimShell;

#اگر_تعریف_شده FEAT_MENU
# define apply_fontlist(w) gui_motif_menu_fontlist(w)
#وإلا
# define apply_fontlist(w)
#پایان_اگر

/////////////////////////////////////////////////////////////////////////////
// Font selection dialogue implementation.

ساكن حرف wild[3] = "*";

/*
 * FIXME: This is a generic function, which should be used throughout the whole
 * application.
 *
 * Add close_callback, which will be called when the user selects close from
 * the window menu.  The close menu item usually activates f.kill which sends a
 * WM_DELETE_WINDOW protocol request for the window.
 */

    ساكن فراغ
جمع_کن_cancel_action(Widget shell, XtCallbackProc close_callback, فراغ *arg)
{
    ساكن Atom wmp_atom = 0;
    ساكن Atom dw_atom = 0;
    Display *display = XtDisplay(shell);

    // deactivate the built-in delete response of killing the application
    Xt_Va_مجموعه_مقادیر(shell, XmNdeleteResponse, XmDO_NOTHING, عدم);

    // add a delete window protocol callback instead
    إذا (!dw_atom)
    {
	wmp_atom = XmInternAtom(display, "WM_PROTOCOLS", True);
	dw_atom = XmInternAtom(display, "WM_DELETE_WINDOW", True);
    }
    Xm_جمع_کن_Protocol_Callback(shell, wmp_atom, dw_atom, close_callback, arg);
}

#تعریف_کن MAX_FONTS			65535
#تعریف_کن MAX_FONT_NAME_LEN		256
#تعریف_کن MAX_ENTRIES_درون_LIST		5000
#تعریف_کن MAX_DISPLAY_اندازه		150
#تعریف_کن موقت_BUF_اندازه			256

تعداد ListSpecifier
{
    ENCODING,
    NAME,
    STYLE,
    اندازه_3,
    NONE
};

تعريفنوع بنية Shared_Font_Sel_داده
{
    Widget	dialog;
    Widget	ok;
    Widget	cancel;
    Widget	encoding_pulldown;
    Widget	encoding_menu;
    Widget	list[NONE];
    Widget	name;
    Widget	sample;
    حرف	**نامها;	// font name array of arrays
    صحيح		رقم;		// number of font names
    String	sel[NONE];	// selection category
    Boolean	درون_pixels;	// toggle state - size in pixels
    حرف	*font_name;	// current font name
    XFontStruct	*old;		// font data structure for sample display
    XmFontList	old_list;	// font data structure for sample display
    Boolean	خروج;		// used for program exit control
} Shared_Font_Sel_داده_2;

/*
 * Checking access to the font name array for validity.
 */
    ساكن حرف *
fn(Shared_Font_Sel_داده_2 *داده, صحيح i)
{
    // Assertion checks:
    إذا (داده->رقم < 0)
	abort();
    إذا (i >= داده->رقم)
	i = داده->رقم - 1;
    إذا (i < 0)
	i = 0;

    أرجع داده->نامها[i];
}

/*
 * Get a specific substring from a font name.
 */
    ساكن فراغ
get_part(حرف *درون, صحيح pos, حرف *بیرون)
{
    صحيح	i;
    صحيح j;

    *بیرون = '\0';

    لكل (i = 0; (pos > 0) && (درون[i] != '\0'); ++i)
	إذا (درون[i] == '-')
	    pos--;

    إذا (درون[i] == '\0')
	أرجع;

    لكل (j = 0; (درون[i] != '-') && (درون[i] != '\0'); ++i, ++j)
	بیرون[j] = درون[i];
    بیرون[j] = '\0';
}

/*
 * Given a font name this function returns the part used in the first
 * scroll list.
 */
    ساكن فراغ
name_part(حرف *font, حرف *buf)
{
    حرف    buf2[موقت_BUF_اندازه];
    حرف    buf3[موقت_BUF_اندازه];

    get_part(font, 2, buf2);
    get_part(font, 1, buf3);

    إذا (*buf3 != NUL)
	vim_snprintf(buf, موقت_BUF_اندازه, "%s (%s)", buf2, buf3);
    وإلا
	vim_snprintf(buf, موقت_BUF_اندازه, "%s", buf2);
}

/*
 * Given a font name this function returns the part used in the second scroll list.
 */
    ساكن فراغ
style_part(حرف *font, حرف *buf)
{
    حرف    buf2[موقت_BUF_اندازه];
    حرف    buf3[موقت_BUF_اندازه];

    get_part(font, 3, buf3);
    get_part(font, 5, buf2);

    إذا (!strcmp(buf2, "normal") && !strcmp(buf2, "Normal")
						   && !strcmp(buf2, "NORMAL"))
	vim_snprintf(buf, موقت_BUF_اندازه, "%s %s", buf3, buf2);
    وإلا
	strcpy(buf, buf3);

    get_part(font, 6, buf2);

    إذا (buf2[0] != '\0')
	vim_snprintf(buf3, موقت_BUF_اندازه, "%s %s", buf, buf2);
    وإلا
	strcpy(buf3, buf);

    get_part(font, 4, buf2);

    إذا (!strcmp(buf2, "o") || !strcmp(buf2, "O"))
	vim_snprintf(buf, موقت_BUF_اندازه, "%s oblique", buf3);
    وإلا إذا (!strcmp(buf2, "i") || !strcmp(buf2, "I"))
	vim_snprintf(buf, موقت_BUF_اندازه, "%s italic", buf3);

    إذا (!strcmp(buf, " "))
	strcpy(buf, "-");
}

/*
 * Given a font name this function returns the part used in the third
 * scroll list.
 */
    ساكن فراغ
اندازه_part(حرف *font, حرف *buf, صحيح درون_Pixels)
{
    صحيح	    اندازه;
    عائم   موقت;

    *buf = '\0';

    إذا (درون_Pixels)
    {
	get_part(font, 7, buf);
	إذا (*buf != NUL)
	{
	    اندازه = atoi(buf);
	    sprintf(buf, "%3d", اندازه);
	}
    }
    وإلا
    {
	get_part(font, 8, buf);
	إذا (*buf != NUL)
	{
	    اندازه = atoi(buf);
	    موقت = (عائم)اندازه / 10.0;
	    اندازه = موقت;
	    إذا (buf[طول_رشته(buf) - 1] == '0')
		sprintf(buf, "%3d", اندازه);
	    وإلا
		sprintf(buf, "%4.1f", موقت);
	}
    }
}

/*
 * Given a font name this function returns the part used in the choice menu.
 */
    ساكن فراغ
encoding_part(حرف *font, حرف *buf)
{
    حرف    buf1[موقت_BUF_اندازه];
    حرف    buf2[موقت_BUF_اندازه];

    *buf = '\0';

    get_part(font, 13, buf1);
    get_part(font, 14, buf2);

    إذا (*buf1 != NUL && *buf2 != NUL)
	vim_snprintf(buf, موقت_BUF_اندازه, "%s-%s", buf1, buf2);
    إذا (!strcmp(buf, " "))
	strcpy(buf, "-");
}

/*
 * Inserts a string into correct sorted position in a list.
 */
    ساكن فراغ
جمع_کن_مقصد_list(حرف **buf, حرف *قلم, صحيح *شمار)
{
    صحيح	i;
    صحيح j;

    إذا (*شمار == MAX_ENTRIES_درون_LIST)
	أرجع;

    // avoid duplication
    لكل (i = 0; i < *شمار; ++i)
    {
	إذا (!strcmp(buf[i], قلم))
	    أرجع;
    }

    // find order place, but make sure that wild card comes first
    إذا (!strcmp(قلم, wild))
	i = 0;
    وإلا
	لكل (i = 0; i < *شمار; ++i)
	    إذا (strcmp(buf[i], قلم) > 0 && strcmp(buf[i], wild))
		اكسر;

    // now insert the item
    لكل (j = *شمار; j > i; --j)
	buf[j] = buf[j-1];
    buf[i] = XtNewString(قلم);

    ++(*شمار);
}

/*
 * True if the font matches some field.
 */
    ساكن Boolean
تطابق(Shared_Font_Sel_داده_2 *داده, تعداد ListSpecifier l, صحيح i)
{
    حرف buf[موقت_BUF_اندازه];

    // An empty selection or a wild card matches anything.
    إذا (!داده->sel[l] || !strcmp(داده->sel[l], wild))
	أرجع True;

    // chunk out the desired part...
    بدل (l)
    {
	حالة ENCODING:
	    encoding_part(fn(داده, i), buf);
	    اكسر;

	حالة NAME:
	    name_part(fn(داده, i), buf);
	    اكسر;

	حالة STYLE:
	    style_part(fn(داده, i), buf);
	    اكسر;

	حالة اندازه_3:
	    اندازه_part(fn(داده, i), buf, داده->درون_pixels);
	    اكسر;
	افتراضي:
	    ;
    }

    // ...and chew it now

    أرجع !strcmp(buf, داده->sel[l]);
}

    ساكن Boolean
proportional(حرف *font)
{
    حرف buf[موقت_BUF_اندازه];

    get_part(font, 11, buf);

    أرجع !strcmp(buf, "p") || !strcmp(buf, "P");
}


ساكن فراغ encoding_callback(Widget w, Shared_Font_Sel_داده_2 *داده,
							     Xt_اشاره_گر dummy);

/*
 * Parse through the fontlist data and set up the three scroll lists.  The fix
 * parameter can be used to exclude a list from any changes.  This is used for
 * updates after selections caused by the users actions.
 */
    ساكن فراغ
fill_lists(تعداد ListSpecifier fix, Shared_Font_Sel_داده_2 *داده)
{
    حرف	*list[NONE][MAX_ENTRIES_درون_LIST];
    صحيح		شمار[NONE];
    حرف	buf[موقت_BUF_اندازه];
    XmString	اقلام[MAX_ENTRIES_درون_LIST];
    صحيح		i;
    صحيح		idx;

    لكل (idx = (صحيح)ENCODING; idx < (صحيح)NONE; ++idx)
	شمار[idx] = 0;

    // First we insert the wild char into every single list.
    إذا (fix != ENCODING)
	جمع_کن_مقصد_list(list[ENCODING], wild, &شمار[ENCODING]);
    إذا (fix != NAME)
	جمع_کن_مقصد_list(list[NAME], wild, &شمار[NAME]);
    إذا (fix != STYLE)
	جمع_کن_مقصد_list(list[STYLE], wild, &شمار[STYLE]);
    إذا (fix != اندازه_3)
	جمع_کن_مقصد_list(list[اندازه_3], wild, &شمار[اندازه_3]);

    لكل (i = 0; i < داده->رقم && i < MAX_ENTRIES_درون_LIST; i++)
    {
	إذا (proportional(fn(داده, i)))
	    استمر;

	إذا (fix != ENCODING
		&& تطابق(داده, NAME, i)
		&& تطابق(داده, STYLE, i)
		&& تطابق(داده, اندازه_3, i))
	{
	    encoding_part(fn(داده, i), buf);
	    جمع_کن_مقصد_list(list[ENCODING], buf, &شمار[ENCODING]);
	}

	إذا (fix != NAME
		&& تطابق(داده, ENCODING, i)
		&& تطابق(داده, STYLE, i)
		&& تطابق(داده, اندازه_3, i))
	{
	    name_part(fn(داده, i), buf);
	    جمع_کن_مقصد_list(list[NAME], buf, &شمار[NAME]);
	}

	إذا (fix != STYLE
		&& تطابق(داده, ENCODING, i)
		&& تطابق(داده, NAME, i)
		&& تطابق(داده, اندازه_3, i))
	{
	    style_part(fn(داده, i), buf);
	    جمع_کن_مقصد_list(list[STYLE], buf, &شمار[STYLE]);
	}

	إذا (fix != اندازه_3
		&& تطابق(داده, ENCODING, i)
		&& تطابق(داده, NAME, i)
		&& تطابق(داده, STYLE, i))
	{
	    اندازه_part(fn(داده, i), buf, داده->درون_pixels);
	    جمع_کن_مقصد_list(list[اندازه_3], buf, &شمار[اندازه_3]);
	}
    }

    /*
     * And now do the preselection in all lists where there was one:
     */

    إذا (fix != ENCODING)
    {
	Cardinal n_اقلام;
	WidgetList children;
	Widget selected_button = 0;

	// Get and update the current button list.
	Xt_Va_Get_مقادیر(داده->encoding_pulldown,
		XmNchildren, &children,
		XmNnumChildren, &n_اقلام,
		عدم);

	لكل (i = 0; i < شمار[ENCODING]; ++i)
	{
	    Widget button;

	    اقلام[i] = XmStringCreateLocalized(list[ENCODING][i]);

	    إذا (i < (صحيح)n_اقلام)
	    {
		// recycle old button
		Xt_Va_مجموعه_مقادیر(children[i],
			XmNlabelString, اقلام[i],
			Xm_Nuser_داده, i,
			عدم);
		button = children[i];
	    }
	    وإلا
	    {
		// create a new button
		button = XtVaCreateManagedWidget("button",
			xm_افزودن_Button_Gadget_Class,
			داده->encoding_pulldown,
			XmNlabelString, اقلام[i],
			Xm_Nuser_داده, i,
			عدم);
		Xt_جمع_کن_Callback(button, XmNactivateCallback,
			(XtCallbackProc) encoding_callback, (Xt_اشاره_گر) داده);
		Xt_Manage_فرزند(button);
	    }

	    إذا (داده->sel[ENCODING])
	    {
		إذا (!strcmp(داده->sel[ENCODING], list[ENCODING][i]))
		    selected_button = button;
	    }
	    XtFree(list[ENCODING][i]);
	}

	// Destroy all the outstanding menu items.
	لكل (i = شمار[ENCODING]; i < (صحيح)n_اقلام; ++i)
	{
	    Xt_Unmanage_فرزند(children[i]);
	    XtDestroyWidget(children[i]);
	}

	// Preserve the current selection visually.
	إذا (selected_button)
	{
	    Xt_Va_مجموعه_مقادیر(داده->encoding_menu,
		    XmNmenuHistory, selected_button,
		    عدم);
	}

	لكل (i = 0; i < شمار[ENCODING]; ++i)
	    XmStringFree(اقلام[i]);
    }

    /*
     * Now loop through the remaining lists and set them up.
     */
    لكل (idx = (صحيح)NAME; idx < (صحيح)NONE; ++idx)
    {
	Widget w;

	إذا (fix == (تعداد ListSpecifier)idx)
	    استمر;

	بدل ((تعداد ListSpecifier)idx)
	{
	    حالة NAME:
		w = داده->list[NAME];
		اكسر;
	    حالة STYLE:
		w = داده->list[STYLE];
		اكسر;
	    حالة اندازه_3:
		w = داده->list[اندازه_3];
		اكسر;
	    افتراضي:
		w = (Widget)0;	// for lint
	}

	لكل (i = 0; i < شمار[idx]; ++i)
	{
	    اقلام[i] = XmStringCreateLocalized(list[idx][i]);
	    XtFree(list[idx][i]);
	}
	Xm_List_Delete_All_اقلام(w);
	Xm_List_جمع_کن_اقلام(w, اقلام, شمار[idx], 1);
	إذا (داده->sel[idx])
	{
	    XmStringFree(اقلام[0]);
	    اقلام[0] = XmStringCreateLocalized(داده->sel[idx]);
	    Xm_List_Select_قلم(w, اقلام[0], False);
	    Xm_List_مجموعه_Bottom_قلم(w, اقلام[0]);
	}
	لكل (i = 0; i < شمار[idx]; ++i)
	    XmStringFree(اقلام[i]);
    }
}

    ساكن فراغ
stoggle_callback(Widget w استفادهنشده,
	Shared_Font_Sel_داده_2 *داده,
	XmToggleButtonCallbackStruct *call_داده)
{
    صحيح		i, do_sel;
    حرف	new_اندازه_2[موقت_BUF_اندازه];
    XmString	str;

    إذا (call_داده->reason != (صحيح)Xm_CR_مقدار_تغییریافته)
	أرجع;

    do_sel = (داده->sel[اندازه_3] != عدم) && strcmp(داده->sel[اندازه_3], wild);

    لكل (i = 0; do_sel && (i < داده->رقم); i++)
	إذا (تطابق(داده, ENCODING, i)
		&& تطابق(داده, NAME, i)
		&& تطابق(داده, STYLE, i)
		&& تطابق(داده, اندازه_3, i))
	{
	    اندازه_part(fn(داده, i), new_اندازه_2, !داده->درون_pixels);
	    اكسر;
	}

    داده->درون_pixels = !داده->درون_pixels;

    إذا (داده->sel[اندازه_3])
	XtFree(داده->sel[اندازه_3]);
    داده->sel[اندازه_3] = عدم;
    fill_lists(NONE, داده);

    إذا (do_sel)
    {
	str = XmStringCreateLocalized(new_اندازه_2);
	Xm_List_Select_قلم(داده->list[اندازه_3], str, True);
	Xm_List_مجموعه_Bottom_قلم(داده->list[اندازه_3], str);
	XmStringFree(str);
    }
}

/*
 * Show the currently selected font in the sample text label.
 */
    ساكن فراغ
display_sample(Shared_Font_Sel_داده_2 *داده)
{
    Arg		    آرگومانها[2];
    صحيح		    n;
    XFontStruct	*   font;
    XmFontList	    font_list;
    Display *	    display;
    XmString	    str;

    display = XtDisplay(داده->dialog);
    font = XLoadQueryFont(display, داده->font_name);
    font_list = gui_motif_create_fontlist(font);

    n = 0;
    str = XmStringCreateLocalized("AaBbZzYy 0123456789");
    Xt_مجموعه_Arg(آرگومانها[n], XmNlabelString, str); n++;
    Xt_مجموعه_Arg(آرگومانها[n], XmNfontList, font_list); n++;

    Xt_مجموعه_مقادیر(داده->sample, آرگومانها, n);
    XmStringFree(str);

    إذا (داده->old)
    {
	XFreeFont(display, داده->old);
	XmFontListFree(داده->old_list);
    }
    داده->old = font;
    داده->old_list = font_list;
}


    ساكن Boolean
do_choice(Widget w,
	Shared_Font_Sel_داده_2 *داده,
	XmListCallbackStruct *call_داده,
	تعداد ListSpecifier which)
{
    حرف *sel;

    XmStringGetLtoR(call_داده->قلم, XmSTRING_DEFAULT_CHARSET, &sel);

    إذا (!داده->sel[which])
	داده->sel[which] = XtNewString(sel);
    وإلا
    {
	إذا (!strcmp(داده->sel[which], sel))
	{
	    // unselecting current selection
	    XtFree(داده->sel[which]);
	    داده->sel[which] = عدم;
	    إذا (w)
		Xm_List_Deselect_قلم(w, call_داده->قلم);
	}
	وإلا
	{
	    XtFree(داده->sel[which]);
	    داده->sel[which] = XtNewString(sel);
	}
    }
    XtFree(sel);

    fill_lists(which, داده);

    // If there is a font selection, we display it.
    إذا (داده->sel[ENCODING]
	    && داده->sel[NAME]
	    && داده->sel[STYLE]
	    && داده->sel[اندازه_3]
	    && strcmp(داده->sel[ENCODING], wild)
	    && strcmp(داده->sel[NAME], wild)
	    && strcmp(داده->sel[STYLE], wild)
	    && strcmp(داده->sel[اندازه_3], wild))
    {
	صحيح i;

	إذا (داده->font_name)
	    XtFree(داده->font_name);
	داده->font_name = عدم;

	لكل (i = 0; i < داده->رقم; i++)
	{
	    إذا (تطابق(داده, ENCODING, i)
		    && تطابق(داده, NAME, i)
		    && تطابق(داده, STYLE, i)
		    && تطابق(داده, اندازه_3, i))
	    {
		داده->font_name = XtNewString(fn(داده, i));
		اكسر;
	    }
	}

	إذا (داده->font_name)
	{
	    Xm_متن_مجموعه_String(داده->name, داده->font_name);
	    display_sample(داده);
	}
	وإلا
	    do_dialog(VIM_ERROR,
		    (char_u *)_("Error"),
		    (char_u *)_("Invalid font specification"),
		    (char_u *)_("&Dismiss"), 1, عدم, FALSE);

	أرجع True;
    }
    وإلا
    {
	صحيح	    n;
	XmString    str;
	Arg	    آرگومانها[4];
	حرف	    *nomatch_پیام = _("no specific match");

	n = 0;
	str = XmStringCreateLocalized(nomatch_پیام);
	Xt_مجموعه_Arg(آرگومانها[n], XmNlabelString, str); ++n;
	Xt_مجموعه_مقادیر(داده->sample, آرگومانها, n);
	apply_fontlist(داده->sample);
	Xm_متن_مجموعه_String(داده->name, nomatch_پیام);
	XmStringFree(str);

	أرجع False;
    }
}

    ساكن فراغ
encoding_callback(Widget w,
	Shared_Font_Sel_داده_2 *داده,
	Xt_اشاره_گر dummy استفادهنشده)
{
    XmString str;
    XmListCallbackStruct fake_داده;

    Xt_Va_Get_مقادیر(w, XmNlabelString, &str, عدم);

    إذا (!str)
	أرجع;

    fake_داده.قلم = str;

    do_choice(0, داده, &fake_داده, ENCODING);
}

    ساكن فراغ
name_callback(Widget w,
	Shared_Font_Sel_داده_2 *داده,
	XmListCallbackStruct *call_داده)
{
    do_choice(w, داده, call_داده, NAME);
}

    ساكن فراغ
style_callback(Widget w,
	Shared_Font_Sel_داده_2 *داده,
	XmListCallbackStruct *call_داده)
{
    do_choice(w, داده, call_داده, STYLE);
}

    ساكن فراغ
اندازه_callback(Widget w,
	Shared_Font_Sel_داده_2 *داده,
	XmListCallbackStruct *call_داده)
{
    do_choice(w, داده, call_داده, اندازه_3);
}

    ساكن فراغ
cancel_callback(Widget w استفادهنشده,
	Shared_Font_Sel_داده_2 *داده,
	XmListCallbackStruct *call_داده استفادهنشده)
{
    إذا (داده->sel[ENCODING])
    {
	XtFree(داده->sel[ENCODING]);
	داده->sel[ENCODING] = عدم;
    }
    إذا (داده->sel[NAME])
    {
	XtFree(داده->sel[NAME]);
	داده->sel[NAME] = عدم;
    }
    إذا (داده->sel[STYLE])
    {
	XtFree(داده->sel[STYLE]);
	داده->sel[STYLE] = عدم;
    }
    إذا (داده->sel[اندازه_3])
    {
	XtFree(داده->sel[اندازه_3]);
	داده->sel[اندازه_3] = عدم;
    }

    إذا (داده->font_name)
	XtFree(داده->font_name);
    داده->font_name = عدم;

    داده->رقم = 0;
    XFree_Font_نامها(داده->نامها);
    داده->نامها = عدم;
    داده->خروج = True;
}

    ساكن فراغ
ok_callback(Widget w استفادهنشده,
	Shared_Font_Sel_داده_2 *داده,
	Xm_افزودن_Button_Callback_Struct *call_داده استفادهنشده)
{
    حرف    *الگو;
    حرف    **name;
    صحيح	    i;

    الگو = Xm_متن_Get_String(داده->name);
    name    = XListFonts(XtDisplay(داده->dialog), الگو, 1, &i);
    XtFree(الگو);

    إذا (i != 1)
    {
	do_dialog(VIM_ERROR,
		(char_u *)_("Error"),
		(char_u *)_("Invalid font specification"),
		(char_u *)_("&Dismiss"), 1, عدم, FALSE);
	XFree_Font_نامها(name);
    }
    وإلا
    {
	إذا (داده->font_name)
	    XtFree(داده->font_name);
	داده->font_name = XtNewString(name[0]);

	إذا (داده->sel[ENCODING])
	{
	    XtFree(داده->sel[ENCODING]);
	    داده->sel[ENCODING] = عدم;
	}
	إذا (داده->sel[NAME])
	{
	    XtFree(داده->sel[NAME]);
	    داده->sel[NAME] = عدم;
	}
	إذا (داده->sel[STYLE])
	{
	    XtFree(داده->sel[STYLE]);
	    داده->sel[STYLE] = عدم;
	}
	إذا (داده->sel[اندازه_3])
	{
	    XtFree(داده->sel[اندازه_3]);
	    داده->sel[اندازه_3] = عدم;
	}

	XFree_Font_نامها(name);

	داده->رقم = 0;
	XFree_Font_نامها(داده->نامها);
	داده->نامها = عدم;
	داده->خروج = True;
    }
}

/*
 * Returns pointer to an ASCII character string that contains the name of the
 * selected font (in X format for naming fonts); it is the users responsibility
 * to free the space allocated to this string.
 */
    char_u *
gui_xm_select_font(char_u *current)
{
    ساكن Shared_Font_Sel_داده_2	داده_3;

    Widget		والد;
    Widget		form;
    Widget		separator;
    Widget		sub_form;
    Widget		اندازه_toggle;
    Widget		name;
    Widget		disp_قاب;
    Widget		قاب;
    Arg			آرگومانها[64];
    صحيح			n;
    XmString		str;
    حرف		big_font[MAX_FONT_NAME_LEN];
    Shared_Font_Sel_داده_2	*داده;

    داده = &داده_3;

    والد = vimShell;
    داده->نامها = XListFonts(XtDisplay(والد), "-*-*-*-*-*-*-*-*-*-*-*-*-*-*",
						       MAX_FONTS, &داده->رقم);

    /*
     * Find the name of the biggest font less than the given limit
     * MAX_DISPLAY_SIZE used to set up the initial height of the display
     * widget.
     */

    {
	صحيح	i;
	صحيح	max;
	صحيح	idx = 0;
	صحيح	اندازه;
	حرف	buf[128];

	لكل (i = 0, max = 0; i < داده->رقم; i++)
	{
	    get_part(fn(داده, i), 7, buf);
	    اندازه = atoi(buf);
	    إذا ((اندازه > max) && (اندازه < MAX_DISPLAY_اندازه))
	    {
		idx = i;
		max = اندازه;
	    }
	}
	strcpy(big_font, fn(داده, idx));
    }
    داده->old = XLoadQueryFont(XtDisplay(والد), big_font);
    داده->old_list = gui_motif_create_fontlist(داده->old);

    // Set the title of the Dialog window.
    داده->dialog = XmCreateDialogShell(والد, "fontSelector", عدم, 0);
    str = XmStringCreateLocalized(_("Vim - Font Selector"));

    // Create form popup dialog widget.
    form = XtVaCreateWidget("form",
	    xmFormWidgetClass, داده->dialog,
	    XmNdialogTitle, str,
	    XmNautoUnmanage, False,
	    XmNdialogStyle, Xm_DIALOG_پر_APPLICATION_MODAL,
	    عدم);
    XmStringFree(str);

    sub_form = XtVaCreateManagedWidget("subForm",
	    xmFormWidgetClass, form,
	    XmNbottomAttachment, XmATTACH_FORM,
	    Xm_Nbottom_جابه_جایی, 4,
	    XmNrightAttachment, XmATTACH_FORM,
	    Xm_Nright_جابه_جایی, 4,
	    XmNtopAttachment, XmATTACH_FORM,
	    Xm_Ntop_جابه_جایی, 4,
	    XmNorientation, XmVERTICAL,
	    عدم);

    داده->ok = XtVaCreateManagedWidget(_("OK"),
	    xm_افزودن_Button_Gadget_Class, sub_form,
	    XmNleftAttachment, XmATTACH_FORM,
	    XmNrightAttachment, XmATTACH_FORM,
	    XmNtopAttachment, XmATTACH_FORM,
	    Xm_Ntop_جابه_جایی, 4,
	    عدم);
    apply_fontlist(داده->ok);

    داده->cancel = XtVaCreateManagedWidget(_("Cancel"),
	    xm_افزودن_Button_Gadget_Class, sub_form,
	    XmNrightAttachment, XmATTACH_FORM,
	    XmNleftAttachment, XmATTACH_FORM,
	    XmNtopAttachment, XmATTACH_WIDGET,
	    XmNtopWidget, داده->ok,
	    Xm_Ntop_جابه_جایی, 4,
	    XmNshowAsDefault, True,
	    عدم);
    apply_fontlist(داده->cancel);

    // Create the separator for beauty.
    n = 0;
    Xt_مجموعه_Arg(آرگومانها[n], XmNorientation, XmVERTICAL); n++;
    Xt_مجموعه_Arg(آرگومانها[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    Xt_مجموعه_Arg(آرگومانها[n], XmNtopAttachment, XmATTACH_FORM); n++;
    Xt_مجموعه_Arg(آرگومانها[n], XmNrightAttachment, XmATTACH_WIDGET); n++;
    Xt_مجموعه_Arg(آرگومانها[n], XmNrightWidget, sub_form); n++;
    Xt_مجموعه_Arg(آرگومانها[n], Xm_Nright_جابه_جایی, 4); n++;
    separator = XmCreateSeparatorGadget(form, "separator", آرگومانها, n);
    Xt_Manage_فرزند(separator);

    // Create font name text widget and the corresponding label.
    داده->name = XtVaCreateManagedWidget("fontName",
	    xm_متن_Widget_Class, form,
	    XmNbottomAttachment, XmATTACH_FORM,
	    Xm_Nbottom_جابه_جایی, 4,
	    XmNleftAttachment, XmATTACH_FORM,
	    Xm_Nleft_جابه_جایی, 4,
	    XmNrightAttachment, XmATTACH_WIDGET,
	    XmNrightWidget, separator,
	    Xm_Nright_جابه_جایی, 4,
	    XmNeditable, False,
	    Xm_Nedit_نما, Xm_SINGLE_سطر_EDIT,
	    Xm_Nmax_درازی, MAX_FONT_NAME_LEN,
	    XmNcolumns, 60,
	    عدم);

    str = XmStringCreateLocalized(_("Name:"));
    name = XtVaCreateManagedWidget("fontNameLabel",
	    xmLabelGadgetClass, form,
	    XmNlabelString, str,
	    Xm_Nuser_داده, داده->name,
	    XmNleftAttachment, XmATTACH_OPPOSITE_WIDGET,
	    XmNleftWidget, داده->name,
	    XmNbottomAttachment, XmATTACH_WIDGET,
	    XmNbottomWidget, داده->name,
	    Xm_Ntop_جابه_جایی, 1,
	    عدم);
    XmStringFree(str);
    apply_fontlist(name);

    // create sample display label widget
    disp_قاب = XtVaCreateManagedWidget("sampleFrame",
	    xm_قاب_Widget_Class, form,
	    XmNshadowType, Xm_SHADOW_ETCHED_درون,
	    XmNleftAttachment, XmATTACH_FORM,
	    Xm_Nleft_جابه_جایی, 4,
	    XmNbottomAttachment, XmATTACH_WIDGET,
	    XmNbottomWidget, name,
	    XmNrightAttachment, XmATTACH_WIDGET,
	    XmNrightWidget, separator,
	    Xm_Nright_جابه_جایی, 4,
	    XmNalignment, XmALIGNMENT_BEGINNING,
	    عدم);

    داده->sample = XtVaCreateManagedWidget("sampleLabel",
	    xmLabelWidgetClass, disp_قاب,
	    XmNleftAttachment, XmATTACH_FORM,
	    XmNtopAttachment, XmATTACH_FORM,
	    XmNbottomAttachment, XmATTACH_FORM,
	    XmNrightAttachment, XmATTACH_FORM,
	    XmNalignment, XmALIGNMENT_BEGINNING,
	    Xm_Nrecompute_اندازه, False,
	    XmNfontList, داده->old_list,
	    عدم);

    // create toggle button
    str = XmStringCreateLocalized(_("Show size in Points"));
    اندازه_toggle = XtVaCreateManagedWidget("sizeToggle",
	    xmToggleButtonGadgetClass, form,
	    XmNlabelString, str,
	    XmNleftAttachment, XmATTACH_FORM,
	    Xm_Nleft_جابه_جایی, 4,
	    XmNbottomAttachment, XmATTACH_WIDGET,
	    XmNbottomWidget, disp_قاب,
	    Xm_Nbottom_جابه_جایی, 4,
	    عدم);
    XmStringFree(str);
    apply_fontlist(اندازه_toggle);
    Xt_Manage_فرزند(اندازه_toggle);

    // Encoding pulldown menu.

    داده->encoding_pulldown = XmCreatePulldownMenu(form,
						 "encodingPulldown", عدم, 0);
    str = XmStringCreateLocalized(_("Encoding:"));
    n = 0;
    Xt_مجموعه_Arg(آرگومانها[n], XmNsubMenuId, داده->encoding_pulldown); ++n;
    Xt_مجموعه_Arg(آرگومانها[n], XmNlabelString, str); ++n;
    Xt_مجموعه_Arg(آرگومانها[n], XmNleftAttachment, XmATTACH_FORM); ++n;
    Xt_مجموعه_Arg(آرگومانها[n], Xm_Nleft_جابه_جایی, 4); ++n;
    Xt_مجموعه_Arg(آرگومانها[n], XmNbottomAttachment, XmATTACH_WIDGET); ++n;
    Xt_مجموعه_Arg(آرگومانها[n], XmNbottomWidget, اندازه_toggle); ++n;
    Xt_مجموعه_Arg(آرگومانها[n], Xm_Nbottom_جابه_جایی, 4); ++n;
    Xt_مجموعه_Arg(آرگومانها[n], XmNrightAttachment, XmATTACH_WIDGET); ++n;
    Xt_مجموعه_Arg(آرگومانها[n], XmNrightWidget, separator); ++n;
    Xt_مجموعه_Arg(آرگومانها[n], Xm_Nright_جابه_جایی, 4); ++n;
    داده->encoding_menu = XmCreateOptionMenu(form, "encodingMenu", آرگومانها, n);
    XmStringFree(str);
    Xm_جمع_کن_Tab_گروه(داده->encoding_menu);

    /*
     * Create scroll list widgets in a separate subform used to manage the
     * different sizes of the lists.
     */

    sub_form = XtVaCreateManagedWidget("subForm",
	    xmFormWidgetClass, form,
	    XmNbottomAttachment, XmATTACH_WIDGET,
	    XmNbottomWidget, داده->encoding_menu,
	    Xm_Nbottom_جابه_جایی, 4,
	    XmNleftAttachment, XmATTACH_FORM,
	    Xm_Nleft_جابه_جایی, 4,
	    XmNrightAttachment, XmATTACH_WIDGET,
	    XmNrightWidget, separator,
	    Xm_Nright_جابه_جایی, 4,
	    XmNtopAttachment, XmATTACH_FORM,
	    Xm_Ntop_جابه_جایی, 2,
	    XmNorientation, XmVERTICAL,
	    عدم);

    // font list
    قاب = XtVaCreateManagedWidget("frame", xm_قاب_Widget_Class, sub_form,
	    XmNshadowThickness, 0,
	    XmNtopAttachment, XmATTACH_FORM,
	    XmNbottomAttachment, XmATTACH_FORM,
	    XmNleftAttachment, XmATTACH_FORM,
	    XmNrightAttachment, XmATTACH_POSITION,
	    XmNrightPosition, 50,
	    عدم);

    str = XmStringCreateLocalized(_("Font:"));
    name = XtVaCreateManagedWidget("nameListLabel", xmLabelGadgetClass, قاب,
	    XmNchildType, Xm_قاب_TITLE_فرزند,
	    XmNchildVerticalAlignment, XmALIGNMENT_CENTER,
	    XmNchildHorizontalAlignment, XmALIGNMENT_BEGINNING,
	    XmNlabelString, str,
	    عدم);
    XmStringFree(str);
    apply_fontlist(name);

    n = 0;
    Xt_مجموعه_Arg(آرگومانها[n], Xm_Nvisible_قلم_شمار, 8); ++n;
    Xt_مجموعه_Arg(آرگومانها[n], XmNresizable, True); ++n;
    Xt_مجموعه_Arg(آرگومانها[n], Xm_Nlist_اندازه_Policy, XmCONSTANT); ++n;
    Xt_مجموعه_Arg(آرگومانها[n], XmNvisualPolicy, XmVARIABLE); ++n;
#اگر_تعریف_شده LESSTIF_نسخه
    Xt_مجموعه_Arg(آرگومانها[n], XmNscrollBarDisplayPolicy, XmSTATIC); ++n;
#پایان_اگر
    داده->list[NAME] = XmCreateScrolledList(قاب, "fontList", آرگومانها, n);
    Xt_Va_مجموعه_مقادیر(name, Xm_Nuser_داده, داده->list[NAME], عدم);

    // style list
    قاب = XtVaCreateManagedWidget("frame", xm_قاب_Widget_Class, sub_form,
	    XmNshadowThickness, 0,
	    XmNtopAttachment, XmATTACH_FORM,
	    XmNbottomAttachment, XmATTACH_FORM,
	    XmNleftAttachment, XmATTACH_POSITION,
	    XmNleftPosition, 50,
	    Xm_Nleft_جابه_جایی, 4,
	    XmNrightAttachment, XmATTACH_POSITION,
	    XmNrightPosition, 80,
	    عدم);

    str = XmStringCreateLocalized(_("Style:"));
    name = XtVaCreateManagedWidget("styleListLabel", xmLabelWidgetClass, قاب,
	    XmNchildType, Xm_قاب_TITLE_فرزند,
	    XmNchildVerticalAlignment, XmALIGNMENT_CENTER,
	    XmNchildHorizontalAlignment, XmALIGNMENT_BEGINNING,
	    XmNlabelString, str,
	    عدم);
    XmStringFree(str);
    apply_fontlist(name);

    n = 0;
    Xt_مجموعه_Arg(آرگومانها[n], Xm_Nvisible_قلم_شمار, 8); ++n;
    Xt_مجموعه_Arg(آرگومانها[n], XmNresizable, True); ++n;
    Xt_مجموعه_Arg(آرگومانها[n], Xm_Nlist_اندازه_Policy, XmCONSTANT); ++n;
    Xt_مجموعه_Arg(آرگومانها[n], XmNvisualPolicy, XmVARIABLE); ++n;
#اگر_تعریف_شده LESSTIF_نسخه
    Xt_مجموعه_Arg(آرگومانها[n], XmNscrollBarDisplayPolicy, XmSTATIC); ++n;
#پایان_اگر
    داده->list[STYLE] = XmCreateScrolledList(قاب, "styleList", آرگومانها, n);
    Xt_Va_مجموعه_مقادیر(name, Xm_Nuser_داده, داده->list[STYLE], عدم);

    // size list
    قاب = XtVaCreateManagedWidget("frame", xm_قاب_Widget_Class, sub_form,
	    XmNshadowThickness, 0,
	    XmNtopAttachment, XmATTACH_FORM,
	    XmNbottomAttachment, XmATTACH_FORM,
	    XmNleftAttachment, XmATTACH_POSITION,
	    XmNleftPosition, 80,
	    Xm_Nleft_جابه_جایی, 4,
	    XmNrightAttachment, XmATTACH_FORM,
	    عدم);

    str = XmStringCreateLocalized(_("Size:"));
    name = XtVaCreateManagedWidget("sizeListLabel", xmLabelGadgetClass, قاب,
	    XmNchildType, Xm_قاب_TITLE_فرزند,
	    XmNchildVerticalAlignment, XmALIGNMENT_CENTER,
	    XmNchildHorizontalAlignment, XmALIGNMENT_BEGINNING,
	    XmNlabelString, str,
	    عدم);
    XmStringFree(str);
    apply_fontlist(name);

    n = 0;
    Xt_مجموعه_Arg(آرگومانها[n], Xm_Nvisible_قلم_شمار, 8); ++n;
    Xt_مجموعه_Arg(آرگومانها[n], XmNresizable, True); ++n;
    Xt_مجموعه_Arg(آرگومانها[n], Xm_Nlist_اندازه_Policy, XmCONSTANT); ++n;
    Xt_مجموعه_Arg(آرگومانها[n], XmNvisualPolicy, XmVARIABLE); ++n;
#اگر_تعریف_شده LESSTIF_نسخه
    Xt_مجموعه_Arg(آرگومانها[n], XmNscrollBarDisplayPolicy, XmSTATIC); ++n;
#پایان_اگر
    داده->list[اندازه_3] = XmCreateScrolledList(قاب, "sizeList", آرگومانها, n);
    Xt_Va_مجموعه_مقادیر(name, Xm_Nuser_داده, داده->list[اندازه_3], عدم);

    // update form widgets cancel button
    Xt_Va_مجموعه_مقادیر(form, XmNcancelButton, داده->cancel, عدم);

    Xt_جمع_کن_Callback(اندازه_toggle, Xm_Nvalue_تغییریافته_Callback,
	    (XtCallbackProc)stoggle_callback, (Xt_اشاره_گر)داده);
    Xt_جمع_کن_Callback(داده->list[NAME], XmNbrowseSelectionCallback,
	    (XtCallbackProc)name_callback, (Xt_اشاره_گر)داده);
    Xt_جمع_کن_Callback(داده->list[STYLE], XmNbrowseSelectionCallback,
	    (XtCallbackProc)style_callback, (Xt_اشاره_گر)داده);
    Xt_جمع_کن_Callback(داده->list[اندازه_3], XmNbrowseSelectionCallback,
	    (XtCallbackProc)اندازه_callback, (Xt_اشاره_گر)داده);
    Xt_جمع_کن_Callback(داده->ok, XmNactivateCallback,
	    (XtCallbackProc)ok_callback, (Xt_اشاره_گر)داده);
    Xt_جمع_کن_Callback(داده->cancel, XmNactivateCallback,
	    (XtCallbackProc)cancel_callback, (Xt_اشاره_گر)داده);

    XmProcessTraversal(داده->list[NAME], XmTRAVERSE_CURRENT);

    // setup tabgroups

    Xm_جمع_کن_Tab_گروه(داده->list[NAME]);
    Xm_جمع_کن_Tab_گروه(داده->list[STYLE]);
    Xm_جمع_کن_Tab_گروه(داده->list[اندازه_3]);
    Xm_جمع_کن_Tab_گروه(اندازه_toggle);
    Xm_جمع_کن_Tab_گروه(داده->name);
    Xm_جمع_کن_Tab_گروه(داده->ok);
    Xm_جمع_کن_Tab_گروه(داده->cancel);

    جمع_کن_cancel_action(داده->dialog, (XtCallbackProc)cancel_callback, داده);

    // Preset selection data.

    داده->خروج = False;
    داده->درون_pixels= True;
    داده->sel[ENCODING] = عدم;
    داده->sel[NAME] = عدم;
    داده->sel[STYLE] = عدم;
    داده->sel[اندازه_3] = عدم;
    داده->font_name = عدم;

    // set up current font parameters
    إذا (current && current[0] != '\0')
    {
	صحيح	    i;
	حرف	    **نامها;

	نامها = XListFonts(XtDisplay(form), (حرف *) current, 1, &i);

	إذا (i != 0)
	{
	    حرف namebuf[موقت_BUF_اندازه];
	    حرف stylebuf[موقت_BUF_اندازه];
	    حرف sizebuf[موقت_BUF_اندازه];
	    حرف encodingbuf[موقت_BUF_اندازه];
	    حرف *یافت;

	    یافت = نامها[0];

	    name_part(یافت, namebuf);
	    style_part(یافت, stylebuf);
	    اندازه_part(یافت, sizebuf, داده->درون_pixels);
	    encoding_part(یافت, encodingbuf);

	    إذا (*namebuf != NUL
		    && *stylebuf != NUL
		    && *sizebuf != NUL
		    && *encodingbuf != NUL)
	    {
		داده->sel[NAME] = XtNewString(namebuf);
		داده->sel[STYLE] = XtNewString(stylebuf);
		داده->sel[اندازه_3] = XtNewString(sizebuf);
		داده->sel[ENCODING] = XtNewString(encodingbuf);
		داده->font_name = XtNewString(نامها[0]);
		display_sample(داده);
		Xm_متن_مجموعه_String(داده->name, داده->font_name);
	    }
	    وإلا
	    {
		// We can't preset a symbolic name, which isn't a full font
		// description. Therefore we just behave the same way as if the
		// user didn't have selected anything thus far.
		//
		// Unfortunately there is no known way to expand an abbreviated
		// font name.
		داده->font_name = عدم;
	    }
	}
	XFree_Font_نامها(نامها);
    }

    fill_lists(NONE, داده);

    // Unfortunately LessTif doesn't align the list widget's properly.  I don't
    // have currently any idea how to fix this problem.
    Xt_Manage_فرزند(داده->list[NAME]);
    Xt_Manage_فرزند(داده->list[STYLE]);
    Xt_Manage_فرزند(داده->list[اندازه_3]);
    Xt_Manage_فرزند(داده->encoding_menu);
    manage_centered(form);

    // modal event loop
    طالما (!داده->خروج)
	XtAppProcessEvent(Xt_Widget_مقصد_Application_Context(داده->dialog),
							(Xt_درون_داد_ماسک)XtIMAll);

    إذا (داده->old)
    {
	XFreeFont(XtDisplay(داده->dialog),  داده->old);
	XmFontListFree(داده->old_list);
    }
    XtDestroyWidget(داده->dialog);

    gui_motif_synch_fonts();

    أرجع (char_u *) داده->font_name;
}
