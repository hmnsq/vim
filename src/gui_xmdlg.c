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

#که_تعریف_شوی FEAT_MENU
# define apply_fontlist(w) gui_motif_menu_fontlist(w)
#وإلا
# define apply_fontlist(w)
#د_که_پای

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
جمع_کړه_cancel_action(Widget shell, XtCallbackProc close_callback, فراغ *arg)
{
    ساكن Atom wmp_atom = 0;
    ساكن Atom dw_atom = 0;
    Display *display = XtDisplay(shell);

    // deactivate the built-in delete response of killing the application
    Xt_Va_سېټ_ارزښتونه(shell, XmNdeleteResponse, XmDO_NOTHING, عدم);

    // add a delete window protocol callback instead
    إذا (!dw_atom)
    {
	wmp_atom = XmInternAtom(display, "WM_PROTOCOLS", True);
	dw_atom = XmInternAtom(display, "WM_DELETE_WINDOW", True);
    }
    Xm_جمع_کړه_Protocol_Callback(shell, wmp_atom, dw_atom, close_callback, arg);
}

#تعریف_کړه MAX_FONTS			65535
#تعریف_کړه MAX_FONT_NAME_LEN		256
#تعریف_کړه MAX_ENTRIES_دننه_LIST		5000
#تعریف_کړه MAX_DISPLAY_اندازه		150
#تعریف_کړه لنډمهاله_BUF_اندازه			256

تعداد ListSpecifier
{
    ENCODING,
    NAME,
    STYLE,
    اندازه_3,
    NONE
};

تعريفنوع بنية Shared_Font_Sel_معلومات
{
    Widget	dialog;
    Widget	ok;
    Widget	cancel;
    Widget	encoding_pulldown;
    Widget	encoding_menu;
    Widget	list[NONE];
    Widget	name;
    Widget	sample;
    حرف	**نومونه;	// font name array of arrays
    صحيح		عدد;		// number of font names
    String	sel[NONE];	// selection category
    Boolean	دننه_pixels;	// toggle state - size in pixels
    حرف	*font_name;	// current font name
    XFontStruct	*old;		// font data structure for sample display
    XmFontList	old_list;	// font data structure for sample display
    Boolean	وتل;		// used for program exit control
} Shared_Font_Sel_معلومات_2;

/*
 * Checking access to the font name array for validity.
 */
    ساكن حرف *
fn(Shared_Font_Sel_معلومات_2 *معلومات, صحيح i)
{
    // Assertion checks:
    إذا (معلومات->عدد < 0)
	abort();
    إذا (i >= معلومات->عدد)
	i = معلومات->عدد - 1;
    إذا (i < 0)
	i = 0;

    أرجع معلومات->نومونه[i];
}

/*
 * Get a specific substring from a font name.
 */
    ساكن فراغ
get_part(حرف *دننه, صحيح pos, حرف *بهر)
{
    صحيح	i;
    صحيح j;

    *بهر = '\0';

    لكل (i = 0; (pos > 0) && (دننه[i] != '\0'); ++i)
	إذا (دننه[i] == '-')
	    pos--;

    إذا (دننه[i] == '\0')
	أرجع;

    لكل (j = 0; (دننه[i] != '-') && (دننه[i] != '\0'); ++i, ++j)
	بهر[j] = دننه[i];
    بهر[j] = '\0';
}

/*
 * Given a font name this function returns the part used in the first
 * scroll list.
 */
    ساكن فراغ
name_part(حرف *font, حرف *buf)
{
    حرف    buf2[لنډمهاله_BUF_اندازه];
    حرف    buf3[لنډمهاله_BUF_اندازه];

    get_part(font, 2, buf2);
    get_part(font, 1, buf3);

    إذا (*buf3 != NUL)
	vim_snprintf(buf, لنډمهاله_BUF_اندازه, "%s (%s)", buf2, buf3);
    وإلا
	vim_snprintf(buf, لنډمهاله_BUF_اندازه, "%s", buf2);
}

/*
 * Given a font name this function returns the part used in the second scroll list.
 */
    ساكن فراغ
style_part(حرف *font, حرف *buf)
{
    حرف    buf2[لنډمهاله_BUF_اندازه];
    حرف    buf3[لنډمهاله_BUF_اندازه];

    get_part(font, 3, buf3);
    get_part(font, 5, buf2);

    إذا (!strcmp(buf2, "normal") && !strcmp(buf2, "Normal")
						   && !strcmp(buf2, "NORMAL"))
	vim_snprintf(buf, لنډمهاله_BUF_اندازه, "%s %s", buf3, buf2);
    وإلا
	strcpy(buf, buf3);

    get_part(font, 6, buf2);

    إذا (buf2[0] != '\0')
	vim_snprintf(buf3, لنډمهاله_BUF_اندازه, "%s %s", buf, buf2);
    وإلا
	strcpy(buf3, buf);

    get_part(font, 4, buf2);

    إذا (!strcmp(buf2, "o") || !strcmp(buf2, "O"))
	vim_snprintf(buf, لنډمهاله_BUF_اندازه, "%s oblique", buf3);
    وإلا إذا (!strcmp(buf2, "i") || !strcmp(buf2, "I"))
	vim_snprintf(buf, لنډمهاله_BUF_اندازه, "%s italic", buf3);

    إذا (!strcmp(buf, " "))
	strcpy(buf, "-");
}

/*
 * Given a font name this function returns the part used in the third
 * scroll list.
 */
    ساكن فراغ
اندازه_part(حرف *font, حرف *buf, صحيح دننه_Pixels)
{
    صحيح	    اندازه;
    عائم   لنډمهاله;

    *buf = '\0';

    إذا (دننه_Pixels)
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
	    لنډمهاله = (عائم)اندازه / 10.0;
	    اندازه = لنډمهاله;
	    إذا (buf[مزي_اوږدوالی(buf) - 1] == '0')
		sprintf(buf, "%3d", اندازه);
	    وإلا
		sprintf(buf, "%4.1f", لنډمهاله);
	}
    }
}

/*
 * Given a font name this function returns the part used in the choice menu.
 */
    ساكن فراغ
encoding_part(حرف *font, حرف *buf)
{
    حرف    buf1[لنډمهاله_BUF_اندازه];
    حرف    buf2[لنډمهاله_BUF_اندازه];

    *buf = '\0';

    get_part(font, 13, buf1);
    get_part(font, 14, buf2);

    إذا (*buf1 != NUL && *buf2 != NUL)
	vim_snprintf(buf, لنډمهاله_BUF_اندازه, "%s-%s", buf1, buf2);
    إذا (!strcmp(buf, " "))
	strcpy(buf, "-");
}

/*
 * Inserts a string into correct sorted position in a list.
 */
    ساكن فراغ
جمع_کړه_منزل_list(حرف **buf, حرف *توکی, صحيح *شمېر)
{
    صحيح	i;
    صحيح j;

    إذا (*شمېر == MAX_ENTRIES_دننه_LIST)
	أرجع;

    // avoid duplication
    لكل (i = 0; i < *شمېر; ++i)
    {
	إذا (!strcmp(buf[i], توکی))
	    أرجع;
    }

    // find order place, but make sure that wild card comes first
    إذا (!strcmp(توکی, wild))
	i = 0;
    وإلا
	لكل (i = 0; i < *شمېر; ++i)
	    إذا (strcmp(buf[i], توکی) > 0 && strcmp(buf[i], wild))
		اكسر;

    // now insert the item
    لكل (j = *شمېر; j > i; --j)
	buf[j] = buf[j-1];
    buf[i] = XtNewString(توکی);

    ++(*شمېر);
}

/*
 * True if the font matches some field.
 */
    ساكن Boolean
سمون(Shared_Font_Sel_معلومات_2 *معلومات, تعداد ListSpecifier l, صحيح i)
{
    حرف buf[لنډمهاله_BUF_اندازه];

    // An empty selection or a wild card matches anything.
    إذا (!معلومات->sel[l] || !strcmp(معلومات->sel[l], wild))
	أرجع True;

    // chunk out the desired part...
    بدل (l)
    {
	حالة ENCODING:
	    encoding_part(fn(معلومات, i), buf);
	    اكسر;

	حالة NAME:
	    name_part(fn(معلومات, i), buf);
	    اكسر;

	حالة STYLE:
	    style_part(fn(معلومات, i), buf);
	    اكسر;

	حالة اندازه_3:
	    اندازه_part(fn(معلومات, i), buf, معلومات->دننه_pixels);
	    اكسر;
	افتراضي:
	    ;
    }

    // ...and chew it now

    أرجع !strcmp(buf, معلومات->sel[l]);
}

    ساكن Boolean
proportional(حرف *font)
{
    حرف buf[لنډمهاله_BUF_اندازه];

    get_part(font, 11, buf);

    أرجع !strcmp(buf, "p") || !strcmp(buf, "P");
}


ساكن فراغ encoding_callback(Widget w, Shared_Font_Sel_معلومات_2 *معلومات,
							     Xt_اشاره_ګر dummy);

/*
 * Parse through the fontlist data and set up the three scroll lists.  The fix
 * parameter can be used to exclude a list from any changes.  This is used for
 * updates after selections caused by the users actions.
 */
    ساكن فراغ
fill_lists(تعداد ListSpecifier fix, Shared_Font_Sel_معلومات_2 *معلومات)
{
    حرف	*list[NONE][MAX_ENTRIES_دننه_LIST];
    صحيح		شمېر[NONE];
    حرف	buf[لنډمهاله_BUF_اندازه];
    XmString	توکي[MAX_ENTRIES_دننه_LIST];
    صحيح		i;
    صحيح		idx;

    لكل (idx = (صحيح)ENCODING; idx < (صحيح)NONE; ++idx)
	شمېر[idx] = 0;

    // First we insert the wild char into every single list.
    إذا (fix != ENCODING)
	جمع_کړه_منزل_list(list[ENCODING], wild, &شمېر[ENCODING]);
    إذا (fix != NAME)
	جمع_کړه_منزل_list(list[NAME], wild, &شمېر[NAME]);
    إذا (fix != STYLE)
	جمع_کړه_منزل_list(list[STYLE], wild, &شمېر[STYLE]);
    إذا (fix != اندازه_3)
	جمع_کړه_منزل_list(list[اندازه_3], wild, &شمېر[اندازه_3]);

    لكل (i = 0; i < معلومات->عدد && i < MAX_ENTRIES_دننه_LIST; i++)
    {
	إذا (proportional(fn(معلومات, i)))
	    استمر;

	إذا (fix != ENCODING
		&& سمون(معلومات, NAME, i)
		&& سمون(معلومات, STYLE, i)
		&& سمون(معلومات, اندازه_3, i))
	{
	    encoding_part(fn(معلومات, i), buf);
	    جمع_کړه_منزل_list(list[ENCODING], buf, &شمېر[ENCODING]);
	}

	إذا (fix != NAME
		&& سمون(معلومات, ENCODING, i)
		&& سمون(معلومات, STYLE, i)
		&& سمون(معلومات, اندازه_3, i))
	{
	    name_part(fn(معلومات, i), buf);
	    جمع_کړه_منزل_list(list[NAME], buf, &شمېر[NAME]);
	}

	إذا (fix != STYLE
		&& سمون(معلومات, ENCODING, i)
		&& سمون(معلومات, NAME, i)
		&& سمون(معلومات, اندازه_3, i))
	{
	    style_part(fn(معلومات, i), buf);
	    جمع_کړه_منزل_list(list[STYLE], buf, &شمېر[STYLE]);
	}

	إذا (fix != اندازه_3
		&& سمون(معلومات, ENCODING, i)
		&& سمون(معلومات, NAME, i)
		&& سمون(معلومات, STYLE, i))
	{
	    اندازه_part(fn(معلومات, i), buf, معلومات->دننه_pixels);
	    جمع_کړه_منزل_list(list[اندازه_3], buf, &شمېر[اندازه_3]);
	}
    }

    /*
     * And now do the preselection in all lists where there was one:
     */

    إذا (fix != ENCODING)
    {
	Cardinal n_توکي;
	WidgetList children;
	Widget selected_button = 0;

	// Get and update the current button list.
	Xt_Va_Get_ارزښتونه(معلومات->encoding_pulldown,
		XmNchildren, &children,
		XmNnumChildren, &n_توکي,
		عدم);

	لكل (i = 0; i < شمېر[ENCODING]; ++i)
	{
	    Widget button;

	    توکي[i] = XmStringCreateLocalized(list[ENCODING][i]);

	    إذا (i < (صحيح)n_توکي)
	    {
		// recycle old button
		Xt_Va_سېټ_ارزښتونه(children[i],
			XmNlabelString, توکي[i],
			Xm_Nuser_معلومات, i,
			عدم);
		button = children[i];
	    }
	    وإلا
	    {
		// create a new button
		button = XtVaCreateManagedWidget("button",
			xm_ټېل_Button_Gadget_Class,
			معلومات->encoding_pulldown,
			XmNlabelString, توکي[i],
			Xm_Nuser_معلومات, i,
			عدم);
		Xt_جمع_کړه_Callback(button, XmNactivateCallback,
			(XtCallbackProc) encoding_callback, (Xt_اشاره_ګر) معلومات);
		Xt_Manage_ماشوم(button);
	    }

	    إذا (معلومات->sel[ENCODING])
	    {
		إذا (!strcmp(معلومات->sel[ENCODING], list[ENCODING][i]))
		    selected_button = button;
	    }
	    XtFree(list[ENCODING][i]);
	}

	// Destroy all the outstanding menu items.
	لكل (i = شمېر[ENCODING]; i < (صحيح)n_توکي; ++i)
	{
	    Xt_Unmanage_ماشوم(children[i]);
	    XtDestroyWidget(children[i]);
	}

	// Preserve the current selection visually.
	إذا (selected_button)
	{
	    Xt_Va_سېټ_ارزښتونه(معلومات->encoding_menu,
		    XmNmenuHistory, selected_button,
		    عدم);
	}

	لكل (i = 0; i < شمېر[ENCODING]; ++i)
	    XmStringFree(توکي[i]);
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
		w = معلومات->list[NAME];
		اكسر;
	    حالة STYLE:
		w = معلومات->list[STYLE];
		اكسر;
	    حالة اندازه_3:
		w = معلومات->list[اندازه_3];
		اكسر;
	    افتراضي:
		w = (Widget)0;	// for lint
	}

	لكل (i = 0; i < شمېر[idx]; ++i)
	{
	    توکي[i] = XmStringCreateLocalized(list[idx][i]);
	    XtFree(list[idx][i]);
	}
	Xm_List_Delete_All_توکي(w);
	Xm_List_جمع_کړه_توکي(w, توکي, شمېر[idx], 1);
	إذا (معلومات->sel[idx])
	{
	    XmStringFree(توکي[0]);
	    توکي[0] = XmStringCreateLocalized(معلومات->sel[idx]);
	    Xm_List_Select_توکی(w, توکي[0], False);
	    Xm_List_سېټ_Bottom_توکی(w, توکي[0]);
	}
	لكل (i = 0; i < شمېر[idx]; ++i)
	    XmStringFree(توکي[i]);
    }
}

    ساكن فراغ
stoggle_callback(Widget w ناکارول_شوی,
	Shared_Font_Sel_معلومات_2 *معلومات,
	XmToggleButtonCallbackStruct *call_معلومات)
{
    صحيح		i, do_sel;
    حرف	new_اندازه_2[لنډمهاله_BUF_اندازه];
    XmString	str;

    إذا (call_معلومات->reason != (صحيح)Xm_CR_ارزښت_بدل_شوی)
	أرجع;

    do_sel = (معلومات->sel[اندازه_3] != عدم) && strcmp(معلومات->sel[اندازه_3], wild);

    لكل (i = 0; do_sel && (i < معلومات->عدد); i++)
	إذا (سمون(معلومات, ENCODING, i)
		&& سمون(معلومات, NAME, i)
		&& سمون(معلومات, STYLE, i)
		&& سمون(معلومات, اندازه_3, i))
	{
	    اندازه_part(fn(معلومات, i), new_اندازه_2, !معلومات->دننه_pixels);
	    اكسر;
	}

    معلومات->دننه_pixels = !معلومات->دننه_pixels;

    إذا (معلومات->sel[اندازه_3])
	XtFree(معلومات->sel[اندازه_3]);
    معلومات->sel[اندازه_3] = عدم;
    fill_lists(NONE, معلومات);

    إذا (do_sel)
    {
	str = XmStringCreateLocalized(new_اندازه_2);
	Xm_List_Select_توکی(معلومات->list[اندازه_3], str, True);
	Xm_List_سېټ_Bottom_توکی(معلومات->list[اندازه_3], str);
	XmStringFree(str);
    }
}

/*
 * Show the currently selected font in the sample text label.
 */
    ساكن فراغ
display_sample(Shared_Font_Sel_معلومات_2 *معلومات)
{
    Arg		    دلیلونه[2];
    صحيح		    n;
    XFontStruct	*   font;
    XmFontList	    font_list;
    Display *	    display;
    XmString	    str;

    display = XtDisplay(معلومات->dialog);
    font = XLoadQueryFont(display, معلومات->font_name);
    font_list = gui_motif_create_fontlist(font);

    n = 0;
    str = XmStringCreateLocalized("AaBbZzYy 0123456789");
    Xt_سېټ_Arg(دلیلونه[n], XmNlabelString, str); n++;
    Xt_سېټ_Arg(دلیلونه[n], XmNfontList, font_list); n++;

    Xt_سېټ_ارزښتونه(معلومات->sample, دلیلونه, n);
    XmStringFree(str);

    إذا (معلومات->old)
    {
	XFreeFont(display, معلومات->old);
	XmFontListFree(معلومات->old_list);
    }
    معلومات->old = font;
    معلومات->old_list = font_list;
}


    ساكن Boolean
do_choice(Widget w,
	Shared_Font_Sel_معلومات_2 *معلومات,
	XmListCallbackStruct *call_معلومات,
	تعداد ListSpecifier which)
{
    حرف *sel;

    XmStringGetLtoR(call_معلومات->توکی, XmSTRING_DEFAULT_CHARSET, &sel);

    إذا (!معلومات->sel[which])
	معلومات->sel[which] = XtNewString(sel);
    وإلا
    {
	إذا (!strcmp(معلومات->sel[which], sel))
	{
	    // unselecting current selection
	    XtFree(معلومات->sel[which]);
	    معلومات->sel[which] = عدم;
	    إذا (w)
		Xm_List_Deselect_توکی(w, call_معلومات->توکی);
	}
	وإلا
	{
	    XtFree(معلومات->sel[which]);
	    معلومات->sel[which] = XtNewString(sel);
	}
    }
    XtFree(sel);

    fill_lists(which, معلومات);

    // If there is a font selection, we display it.
    إذا (معلومات->sel[ENCODING]
	    && معلومات->sel[NAME]
	    && معلومات->sel[STYLE]
	    && معلومات->sel[اندازه_3]
	    && strcmp(معلومات->sel[ENCODING], wild)
	    && strcmp(معلومات->sel[NAME], wild)
	    && strcmp(معلومات->sel[STYLE], wild)
	    && strcmp(معلومات->sel[اندازه_3], wild))
    {
	صحيح i;

	إذا (معلومات->font_name)
	    XtFree(معلومات->font_name);
	معلومات->font_name = عدم;

	لكل (i = 0; i < معلومات->عدد; i++)
	{
	    إذا (سمون(معلومات, ENCODING, i)
		    && سمون(معلومات, NAME, i)
		    && سمون(معلومات, STYLE, i)
		    && سمون(معلومات, اندازه_3, i))
	    {
		معلومات->font_name = XtNewString(fn(معلومات, i));
		اكسر;
	    }
	}

	إذا (معلومات->font_name)
	{
	    Xm_متن_سېټ_String(معلومات->name, معلومات->font_name);
	    display_sample(معلومات);
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
	Arg	    دلیلونه[4];
	حرف	    *nomatch_پیغام = _("no specific match");

	n = 0;
	str = XmStringCreateLocalized(nomatch_پیغام);
	Xt_سېټ_Arg(دلیلونه[n], XmNlabelString, str); ++n;
	Xt_سېټ_ارزښتونه(معلومات->sample, دلیلونه, n);
	apply_fontlist(معلومات->sample);
	Xm_متن_سېټ_String(معلومات->name, nomatch_پیغام);
	XmStringFree(str);

	أرجع False;
    }
}

    ساكن فراغ
encoding_callback(Widget w,
	Shared_Font_Sel_معلومات_2 *معلومات,
	Xt_اشاره_ګر dummy ناکارول_شوی)
{
    XmString str;
    XmListCallbackStruct fake_معلومات;

    Xt_Va_Get_ارزښتونه(w, XmNlabelString, &str, عدم);

    إذا (!str)
	أرجع;

    fake_معلومات.توکی = str;

    do_choice(0, معلومات, &fake_معلومات, ENCODING);
}

    ساكن فراغ
name_callback(Widget w,
	Shared_Font_Sel_معلومات_2 *معلومات,
	XmListCallbackStruct *call_معلومات)
{
    do_choice(w, معلومات, call_معلومات, NAME);
}

    ساكن فراغ
style_callback(Widget w,
	Shared_Font_Sel_معلومات_2 *معلومات,
	XmListCallbackStruct *call_معلومات)
{
    do_choice(w, معلومات, call_معلومات, STYLE);
}

    ساكن فراغ
اندازه_callback(Widget w,
	Shared_Font_Sel_معلومات_2 *معلومات,
	XmListCallbackStruct *call_معلومات)
{
    do_choice(w, معلومات, call_معلومات, اندازه_3);
}

    ساكن فراغ
cancel_callback(Widget w ناکارول_شوی,
	Shared_Font_Sel_معلومات_2 *معلومات,
	XmListCallbackStruct *call_معلومات ناکارول_شوی)
{
    إذا (معلومات->sel[ENCODING])
    {
	XtFree(معلومات->sel[ENCODING]);
	معلومات->sel[ENCODING] = عدم;
    }
    إذا (معلومات->sel[NAME])
    {
	XtFree(معلومات->sel[NAME]);
	معلومات->sel[NAME] = عدم;
    }
    إذا (معلومات->sel[STYLE])
    {
	XtFree(معلومات->sel[STYLE]);
	معلومات->sel[STYLE] = عدم;
    }
    إذا (معلومات->sel[اندازه_3])
    {
	XtFree(معلومات->sel[اندازه_3]);
	معلومات->sel[اندازه_3] = عدم;
    }

    إذا (معلومات->font_name)
	XtFree(معلومات->font_name);
    معلومات->font_name = عدم;

    معلومات->عدد = 0;
    XFree_Font_نومونه(معلومات->نومونه);
    معلومات->نومونه = عدم;
    معلومات->وتل = True;
}

    ساكن فراغ
ok_callback(Widget w ناکارول_شوی,
	Shared_Font_Sel_معلومات_2 *معلومات,
	Xm_ټېل_Button_Callback_Struct *call_معلومات ناکارول_شوی)
{
    حرف    *نمونه;
    حرف    **name;
    صحيح	    i;

    نمونه = Xm_متن_Get_String(معلومات->name);
    name    = XListFonts(XtDisplay(معلومات->dialog), نمونه, 1, &i);
    XtFree(نمونه);

    إذا (i != 1)
    {
	do_dialog(VIM_ERROR,
		(char_u *)_("Error"),
		(char_u *)_("Invalid font specification"),
		(char_u *)_("&Dismiss"), 1, عدم, FALSE);
	XFree_Font_نومونه(name);
    }
    وإلا
    {
	إذا (معلومات->font_name)
	    XtFree(معلومات->font_name);
	معلومات->font_name = XtNewString(name[0]);

	إذا (معلومات->sel[ENCODING])
	{
	    XtFree(معلومات->sel[ENCODING]);
	    معلومات->sel[ENCODING] = عدم;
	}
	إذا (معلومات->sel[NAME])
	{
	    XtFree(معلومات->sel[NAME]);
	    معلومات->sel[NAME] = عدم;
	}
	إذا (معلومات->sel[STYLE])
	{
	    XtFree(معلومات->sel[STYLE]);
	    معلومات->sel[STYLE] = عدم;
	}
	إذا (معلومات->sel[اندازه_3])
	{
	    XtFree(معلومات->sel[اندازه_3]);
	    معلومات->sel[اندازه_3] = عدم;
	}

	XFree_Font_نومونه(name);

	معلومات->عدد = 0;
	XFree_Font_نومونه(معلومات->نومونه);
	معلومات->نومونه = عدم;
	معلومات->وتل = True;
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
    ساكن Shared_Font_Sel_معلومات_2	معلومات_3;

    Widget		پلار;
    Widget		form;
    Widget		separator;
    Widget		sub_form;
    Widget		اندازه_toggle;
    Widget		name;
    Widget		disp_چوکاټ;
    Widget		چوکاټ;
    Arg			دلیلونه[64];
    صحيح			n;
    XmString		str;
    حرف		big_font[MAX_FONT_NAME_LEN];
    Shared_Font_Sel_معلومات_2	*معلومات;

    معلومات = &معلومات_3;

    پلار = vimShell;
    معلومات->نومونه = XListFonts(XtDisplay(پلار), "-*-*-*-*-*-*-*-*-*-*-*-*-*-*",
						       MAX_FONTS, &معلومات->عدد);

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

	لكل (i = 0, max = 0; i < معلومات->عدد; i++)
	{
	    get_part(fn(معلومات, i), 7, buf);
	    اندازه = atoi(buf);
	    إذا ((اندازه > max) && (اندازه < MAX_DISPLAY_اندازه))
	    {
		idx = i;
		max = اندازه;
	    }
	}
	strcpy(big_font, fn(معلومات, idx));
    }
    معلومات->old = XLoadQueryFont(XtDisplay(پلار), big_font);
    معلومات->old_list = gui_motif_create_fontlist(معلومات->old);

    // Set the title of the Dialog window.
    معلومات->dialog = XmCreateDialogShell(پلار, "fontSelector", عدم, 0);
    str = XmStringCreateLocalized(_("Vim - Font Selector"));

    // Create form popup dialog widget.
    form = XtVaCreateWidget("form",
	    xmFormWidgetClass, معلومات->dialog,
	    XmNdialogTitle, str,
	    XmNautoUnmanage, False,
	    XmNdialogStyle, Xm_DIALOG_ډک_APPLICATION_MODAL,
	    عدم);
    XmStringFree(str);

    sub_form = XtVaCreateManagedWidget("subForm",
	    xmFormWidgetClass, form,
	    XmNbottomAttachment, XmATTACH_FORM,
	    Xm_Nbottom_دننه_واټن, 4,
	    XmNrightAttachment, XmATTACH_FORM,
	    Xm_Nright_دننه_واټن, 4,
	    XmNtopAttachment, XmATTACH_FORM,
	    Xm_Ntop_دننه_واټن, 4,
	    XmNorientation, XmVERTICAL,
	    عدم);

    معلومات->ok = XtVaCreateManagedWidget(_("OK"),
	    xm_ټېل_Button_Gadget_Class, sub_form,
	    XmNleftAttachment, XmATTACH_FORM,
	    XmNrightAttachment, XmATTACH_FORM,
	    XmNtopAttachment, XmATTACH_FORM,
	    Xm_Ntop_دننه_واټن, 4,
	    عدم);
    apply_fontlist(معلومات->ok);

    معلومات->cancel = XtVaCreateManagedWidget(_("Cancel"),
	    xm_ټېل_Button_Gadget_Class, sub_form,
	    XmNrightAttachment, XmATTACH_FORM,
	    XmNleftAttachment, XmATTACH_FORM,
	    XmNtopAttachment, XmATTACH_WIDGET,
	    XmNtopWidget, معلومات->ok,
	    Xm_Ntop_دننه_واټن, 4,
	    XmNshowAsDefault, True,
	    عدم);
    apply_fontlist(معلومات->cancel);

    // Create the separator for beauty.
    n = 0;
    Xt_سېټ_Arg(دلیلونه[n], XmNorientation, XmVERTICAL); n++;
    Xt_سېټ_Arg(دلیلونه[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    Xt_سېټ_Arg(دلیلونه[n], XmNtopAttachment, XmATTACH_FORM); n++;
    Xt_سېټ_Arg(دلیلونه[n], XmNrightAttachment, XmATTACH_WIDGET); n++;
    Xt_سېټ_Arg(دلیلونه[n], XmNrightWidget, sub_form); n++;
    Xt_سېټ_Arg(دلیلونه[n], Xm_Nright_دننه_واټن, 4); n++;
    separator = XmCreateSeparatorGadget(form, "separator", دلیلونه, n);
    Xt_Manage_ماشوم(separator);

    // Create font name text widget and the corresponding label.
    معلومات->name = XtVaCreateManagedWidget("fontName",
	    xm_متن_Widget_Class, form,
	    XmNbottomAttachment, XmATTACH_FORM,
	    Xm_Nbottom_دننه_واټن, 4,
	    XmNleftAttachment, XmATTACH_FORM,
	    Xm_Nleft_دننه_واټن, 4,
	    XmNrightAttachment, XmATTACH_WIDGET,
	    XmNrightWidget, separator,
	    Xm_Nright_دننه_واټن, 4,
	    XmNeditable, False,
	    Xm_Nedit_ډېرپېښ, Xm_SINGLE_کرښه_EDIT,
	    Xm_Nmax_اوږدتیا, MAX_FONT_NAME_LEN,
	    XmNcolumns, 60,
	    عدم);

    str = XmStringCreateLocalized(_("Name:"));
    name = XtVaCreateManagedWidget("fontNameLabel",
	    xmLabelGadgetClass, form,
	    XmNlabelString, str,
	    Xm_Nuser_معلومات, معلومات->name,
	    XmNleftAttachment, XmATTACH_OPPOSITE_WIDGET,
	    XmNleftWidget, معلومات->name,
	    XmNbottomAttachment, XmATTACH_WIDGET,
	    XmNbottomWidget, معلومات->name,
	    Xm_Ntop_دننه_واټن, 1,
	    عدم);
    XmStringFree(str);
    apply_fontlist(name);

    // create sample display label widget
    disp_چوکاټ = XtVaCreateManagedWidget("sampleFrame",
	    xm_چوکاټ_Widget_Class, form,
	    XmNshadowType, Xm_SHADOW_ETCHED_دننه,
	    XmNleftAttachment, XmATTACH_FORM,
	    Xm_Nleft_دننه_واټن, 4,
	    XmNbottomAttachment, XmATTACH_WIDGET,
	    XmNbottomWidget, name,
	    XmNrightAttachment, XmATTACH_WIDGET,
	    XmNrightWidget, separator,
	    Xm_Nright_دننه_واټن, 4,
	    XmNalignment, XmALIGNMENT_BEGINNING,
	    عدم);

    معلومات->sample = XtVaCreateManagedWidget("sampleLabel",
	    xmLabelWidgetClass, disp_چوکاټ,
	    XmNleftAttachment, XmATTACH_FORM,
	    XmNtopAttachment, XmATTACH_FORM,
	    XmNbottomAttachment, XmATTACH_FORM,
	    XmNrightAttachment, XmATTACH_FORM,
	    XmNalignment, XmALIGNMENT_BEGINNING,
	    Xm_Nrecompute_اندازه, False,
	    XmNfontList, معلومات->old_list,
	    عدم);

    // create toggle button
    str = XmStringCreateLocalized(_("Show size in Points"));
    اندازه_toggle = XtVaCreateManagedWidget("sizeToggle",
	    xmToggleButtonGadgetClass, form,
	    XmNlabelString, str,
	    XmNleftAttachment, XmATTACH_FORM,
	    Xm_Nleft_دننه_واټن, 4,
	    XmNbottomAttachment, XmATTACH_WIDGET,
	    XmNbottomWidget, disp_چوکاټ,
	    Xm_Nbottom_دننه_واټن, 4,
	    عدم);
    XmStringFree(str);
    apply_fontlist(اندازه_toggle);
    Xt_Manage_ماشوم(اندازه_toggle);

    // Encoding pulldown menu.

    معلومات->encoding_pulldown = XmCreatePulldownMenu(form,
						 "encodingPulldown", عدم, 0);
    str = XmStringCreateLocalized(_("Encoding:"));
    n = 0;
    Xt_سېټ_Arg(دلیلونه[n], XmNsubMenuId, معلومات->encoding_pulldown); ++n;
    Xt_سېټ_Arg(دلیلونه[n], XmNlabelString, str); ++n;
    Xt_سېټ_Arg(دلیلونه[n], XmNleftAttachment, XmATTACH_FORM); ++n;
    Xt_سېټ_Arg(دلیلونه[n], Xm_Nleft_دننه_واټن, 4); ++n;
    Xt_سېټ_Arg(دلیلونه[n], XmNbottomAttachment, XmATTACH_WIDGET); ++n;
    Xt_سېټ_Arg(دلیلونه[n], XmNbottomWidget, اندازه_toggle); ++n;
    Xt_سېټ_Arg(دلیلونه[n], Xm_Nbottom_دننه_واټن, 4); ++n;
    Xt_سېټ_Arg(دلیلونه[n], XmNrightAttachment, XmATTACH_WIDGET); ++n;
    Xt_سېټ_Arg(دلیلونه[n], XmNrightWidget, separator); ++n;
    Xt_سېټ_Arg(دلیلونه[n], Xm_Nright_دننه_واټن, 4); ++n;
    معلومات->encoding_menu = XmCreateOptionMenu(form, "encodingMenu", دلیلونه, n);
    XmStringFree(str);
    Xm_جمع_کړه_Tab_ډله(معلومات->encoding_menu);

    /*
     * Create scroll list widgets in a separate subform used to manage the
     * different sizes of the lists.
     */

    sub_form = XtVaCreateManagedWidget("subForm",
	    xmFormWidgetClass, form,
	    XmNbottomAttachment, XmATTACH_WIDGET,
	    XmNbottomWidget, معلومات->encoding_menu,
	    Xm_Nbottom_دننه_واټن, 4,
	    XmNleftAttachment, XmATTACH_FORM,
	    Xm_Nleft_دننه_واټن, 4,
	    XmNrightAttachment, XmATTACH_WIDGET,
	    XmNrightWidget, separator,
	    Xm_Nright_دننه_واټن, 4,
	    XmNtopAttachment, XmATTACH_FORM,
	    Xm_Ntop_دننه_واټن, 2,
	    XmNorientation, XmVERTICAL,
	    عدم);

    // font list
    چوکاټ = XtVaCreateManagedWidget("frame", xm_چوکاټ_Widget_Class, sub_form,
	    XmNshadowThickness, 0,
	    XmNtopAttachment, XmATTACH_FORM,
	    XmNbottomAttachment, XmATTACH_FORM,
	    XmNleftAttachment, XmATTACH_FORM,
	    XmNrightAttachment, XmATTACH_POSITION,
	    XmNrightPosition, 50,
	    عدم);

    str = XmStringCreateLocalized(_("Font:"));
    name = XtVaCreateManagedWidget("nameListLabel", xmLabelGadgetClass, چوکاټ,
	    XmNchildType, Xm_چوکاټ_TITLE_ماشوم,
	    XmNchildVerticalAlignment, XmALIGNMENT_CENTER,
	    XmNchildHorizontalAlignment, XmALIGNMENT_BEGINNING,
	    XmNlabelString, str,
	    عدم);
    XmStringFree(str);
    apply_fontlist(name);

    n = 0;
    Xt_سېټ_Arg(دلیلونه[n], Xm_Nvisible_توکی_شمېر, 8); ++n;
    Xt_سېټ_Arg(دلیلونه[n], XmNresizable, True); ++n;
    Xt_سېټ_Arg(دلیلونه[n], Xm_Nlist_اندازه_Policy, XmCONSTANT); ++n;
    Xt_سېټ_Arg(دلیلونه[n], XmNvisualPolicy, XmVARIABLE); ++n;
#که_تعریف_شوی LESSTIF_نسخه
    Xt_سېټ_Arg(دلیلونه[n], XmNscrollBarDisplayPolicy, XmSTATIC); ++n;
#د_که_پای
    معلومات->list[NAME] = XmCreateScrolledList(چوکاټ, "fontList", دلیلونه, n);
    Xt_Va_سېټ_ارزښتونه(name, Xm_Nuser_معلومات, معلومات->list[NAME], عدم);

    // style list
    چوکاټ = XtVaCreateManagedWidget("frame", xm_چوکاټ_Widget_Class, sub_form,
	    XmNshadowThickness, 0,
	    XmNtopAttachment, XmATTACH_FORM,
	    XmNbottomAttachment, XmATTACH_FORM,
	    XmNleftAttachment, XmATTACH_POSITION,
	    XmNleftPosition, 50,
	    Xm_Nleft_دننه_واټن, 4,
	    XmNrightAttachment, XmATTACH_POSITION,
	    XmNrightPosition, 80,
	    عدم);

    str = XmStringCreateLocalized(_("Style:"));
    name = XtVaCreateManagedWidget("styleListLabel", xmLabelWidgetClass, چوکاټ,
	    XmNchildType, Xm_چوکاټ_TITLE_ماشوم,
	    XmNchildVerticalAlignment, XmALIGNMENT_CENTER,
	    XmNchildHorizontalAlignment, XmALIGNMENT_BEGINNING,
	    XmNlabelString, str,
	    عدم);
    XmStringFree(str);
    apply_fontlist(name);

    n = 0;
    Xt_سېټ_Arg(دلیلونه[n], Xm_Nvisible_توکی_شمېر, 8); ++n;
    Xt_سېټ_Arg(دلیلونه[n], XmNresizable, True); ++n;
    Xt_سېټ_Arg(دلیلونه[n], Xm_Nlist_اندازه_Policy, XmCONSTANT); ++n;
    Xt_سېټ_Arg(دلیلونه[n], XmNvisualPolicy, XmVARIABLE); ++n;
#که_تعریف_شوی LESSTIF_نسخه
    Xt_سېټ_Arg(دلیلونه[n], XmNscrollBarDisplayPolicy, XmSTATIC); ++n;
#د_که_پای
    معلومات->list[STYLE] = XmCreateScrolledList(چوکاټ, "styleList", دلیلونه, n);
    Xt_Va_سېټ_ارزښتونه(name, Xm_Nuser_معلومات, معلومات->list[STYLE], عدم);

    // size list
    چوکاټ = XtVaCreateManagedWidget("frame", xm_چوکاټ_Widget_Class, sub_form,
	    XmNshadowThickness, 0,
	    XmNtopAttachment, XmATTACH_FORM,
	    XmNbottomAttachment, XmATTACH_FORM,
	    XmNleftAttachment, XmATTACH_POSITION,
	    XmNleftPosition, 80,
	    Xm_Nleft_دننه_واټن, 4,
	    XmNrightAttachment, XmATTACH_FORM,
	    عدم);

    str = XmStringCreateLocalized(_("Size:"));
    name = XtVaCreateManagedWidget("sizeListLabel", xmLabelGadgetClass, چوکاټ,
	    XmNchildType, Xm_چوکاټ_TITLE_ماشوم,
	    XmNchildVerticalAlignment, XmALIGNMENT_CENTER,
	    XmNchildHorizontalAlignment, XmALIGNMENT_BEGINNING,
	    XmNlabelString, str,
	    عدم);
    XmStringFree(str);
    apply_fontlist(name);

    n = 0;
    Xt_سېټ_Arg(دلیلونه[n], Xm_Nvisible_توکی_شمېر, 8); ++n;
    Xt_سېټ_Arg(دلیلونه[n], XmNresizable, True); ++n;
    Xt_سېټ_Arg(دلیلونه[n], Xm_Nlist_اندازه_Policy, XmCONSTANT); ++n;
    Xt_سېټ_Arg(دلیلونه[n], XmNvisualPolicy, XmVARIABLE); ++n;
#که_تعریف_شوی LESSTIF_نسخه
    Xt_سېټ_Arg(دلیلونه[n], XmNscrollBarDisplayPolicy, XmSTATIC); ++n;
#د_که_پای
    معلومات->list[اندازه_3] = XmCreateScrolledList(چوکاټ, "sizeList", دلیلونه, n);
    Xt_Va_سېټ_ارزښتونه(name, Xm_Nuser_معلومات, معلومات->list[اندازه_3], عدم);

    // update form widgets cancel button
    Xt_Va_سېټ_ارزښتونه(form, XmNcancelButton, معلومات->cancel, عدم);

    Xt_جمع_کړه_Callback(اندازه_toggle, Xm_Nvalue_بدل_شوی_Callback,
	    (XtCallbackProc)stoggle_callback, (Xt_اشاره_ګر)معلومات);
    Xt_جمع_کړه_Callback(معلومات->list[NAME], XmNbrowseSelectionCallback,
	    (XtCallbackProc)name_callback, (Xt_اشاره_ګر)معلومات);
    Xt_جمع_کړه_Callback(معلومات->list[STYLE], XmNbrowseSelectionCallback,
	    (XtCallbackProc)style_callback, (Xt_اشاره_ګر)معلومات);
    Xt_جمع_کړه_Callback(معلومات->list[اندازه_3], XmNbrowseSelectionCallback,
	    (XtCallbackProc)اندازه_callback, (Xt_اشاره_ګر)معلومات);
    Xt_جمع_کړه_Callback(معلومات->ok, XmNactivateCallback,
	    (XtCallbackProc)ok_callback, (Xt_اشاره_ګر)معلومات);
    Xt_جمع_کړه_Callback(معلومات->cancel, XmNactivateCallback,
	    (XtCallbackProc)cancel_callback, (Xt_اشاره_ګر)معلومات);

    XmProcessTraversal(معلومات->list[NAME], XmTRAVERSE_CURRENT);

    // setup tabgroups

    Xm_جمع_کړه_Tab_ډله(معلومات->list[NAME]);
    Xm_جمع_کړه_Tab_ډله(معلومات->list[STYLE]);
    Xm_جمع_کړه_Tab_ډله(معلومات->list[اندازه_3]);
    Xm_جمع_کړه_Tab_ډله(اندازه_toggle);
    Xm_جمع_کړه_Tab_ډله(معلومات->name);
    Xm_جمع_کړه_Tab_ډله(معلومات->ok);
    Xm_جمع_کړه_Tab_ډله(معلومات->cancel);

    جمع_کړه_cancel_action(معلومات->dialog, (XtCallbackProc)cancel_callback, معلومات);

    // Preset selection data.

    معلومات->وتل = False;
    معلومات->دننه_pixels= True;
    معلومات->sel[ENCODING] = عدم;
    معلومات->sel[NAME] = عدم;
    معلومات->sel[STYLE] = عدم;
    معلومات->sel[اندازه_3] = عدم;
    معلومات->font_name = عدم;

    // set up current font parameters
    إذا (current && current[0] != '\0')
    {
	صحيح	    i;
	حرف	    **نومونه;

	نومونه = XListFonts(XtDisplay(form), (حرف *) current, 1, &i);

	إذا (i != 0)
	{
	    حرف namebuf[لنډمهاله_BUF_اندازه];
	    حرف stylebuf[لنډمهاله_BUF_اندازه];
	    حرف sizebuf[لنډمهاله_BUF_اندازه];
	    حرف encodingbuf[لنډمهاله_BUF_اندازه];
	    حرف *وموندل;

	    وموندل = نومونه[0];

	    name_part(وموندل, namebuf);
	    style_part(وموندل, stylebuf);
	    اندازه_part(وموندل, sizebuf, معلومات->دننه_pixels);
	    encoding_part(وموندل, encodingbuf);

	    إذا (*namebuf != NUL
		    && *stylebuf != NUL
		    && *sizebuf != NUL
		    && *encodingbuf != NUL)
	    {
		معلومات->sel[NAME] = XtNewString(namebuf);
		معلومات->sel[STYLE] = XtNewString(stylebuf);
		معلومات->sel[اندازه_3] = XtNewString(sizebuf);
		معلومات->sel[ENCODING] = XtNewString(encodingbuf);
		معلومات->font_name = XtNewString(نومونه[0]);
		display_sample(معلومات);
		Xm_متن_سېټ_String(معلومات->name, معلومات->font_name);
	    }
	    وإلا
	    {
		// We can't preset a symbolic name, which isn't a full font
		// description. Therefore we just behave the same way as if the
		// user didn't have selected anything thus far.
		//
		// Unfortunately there is no known way to expand an abbreviated
		// font name.
		معلومات->font_name = عدم;
	    }
	}
	XFree_Font_نومونه(نومونه);
    }

    fill_lists(NONE, معلومات);

    // Unfortunately LessTif doesn't align the list widget's properly.  I don't
    // have currently any idea how to fix this problem.
    Xt_Manage_ماشوم(معلومات->list[NAME]);
    Xt_Manage_ماشوم(معلومات->list[STYLE]);
    Xt_Manage_ماشوم(معلومات->list[اندازه_3]);
    Xt_Manage_ماشوم(معلومات->encoding_menu);
    manage_centered(form);

    // modal event loop
    طالما (!معلومات->وتل)
	XtAppProcessEvent(Xt_Widget_منزل_Application_Context(معلومات->dialog),
							(Xt_ننوت_ماسک)XtIMAll);

    إذا (معلومات->old)
    {
	XFreeFont(XtDisplay(معلومات->dialog),  معلومات->old);
	XmFontListFree(معلومات->old_list);
    }
    XtDestroyWidget(معلومات->dialog);

    gui_motif_synch_fonts();

    أرجع (char_u *) معلومات->font_name;
}
