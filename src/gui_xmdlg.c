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

#جيڪڏهن_وصف_ٿيل FEAT_MENU
# define apply_fontlist(w) gui_motif_menu_fontlist(w)
#وإلا
# define apply_fontlist(w)
#جيڪڏهن_پورو

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
جوڙيو_cancel_action(Widget shell, XtCallbackProc close_callback, فراغ *arg)
{
    ساكن Atom wmp_atom = 0;
    ساكن Atom dw_atom = 0;
    Display *display = XtDisplay(shell);

    // deactivate the built-in delete response of killing the application
    Xt_Va_سيٽ_قدرون(shell, XmNdeleteResponse, XmDO_NOTHING, عدم);

    // add a delete window protocol callback instead
    إذا (!dw_atom)
    {
	wmp_atom = XmInternAtom(display, "WM_PROTOCOLS", True);
	dw_atom = XmInternAtom(display, "WM_DELETE_WINDOW", True);
    }
    Xm_جوڙيو_Protocol_Callback(shell, wmp_atom, dw_atom, close_callback, arg);
}

#وصف_ڪريو MAX_FONTS			65535
#وصف_ڪريو MAX_FONT_NAME_LEN		256
#وصف_ڪريو MAX_ENTRIES_اندريون_LIST		5000
#وصف_ڪريو MAX_DISPLAY_سائيز		150
#وصف_ڪريو عارضي_BUF_سائيز			256

تعداد ListSpecifier
{
    ENCODING,
    NAME,
    STYLE,
    سائيز_3,
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
    حرف	**نالا;	// font name array of arrays
    صحيح		انگ;		// number of font names
    String	sel[NONE];	// selection category
    Boolean	اندريون_pixels;	// toggle state - size in pixels
    حرف	*font_name;	// current font name
    XFontStruct	*old;		// font data structure for sample display
    XmFontList	old_list;	// font data structure for sample display
    Boolean	ٻاهر_نڪرو;		// used for program exit control
} Shared_Font_Sel_معلومات_2;

/*
 * Checking access to the font name array for validity.
 */
    ساكن حرف *
fn(Shared_Font_Sel_معلومات_2 *معلومات, صحيح i)
{
    // Assertion checks:
    إذا (معلومات->انگ < 0)
	abort();
    إذا (i >= معلومات->انگ)
	i = معلومات->انگ - 1;
    إذا (i < 0)
	i = 0;

    أرجع معلومات->نالا[i];
}

/*
 * Get a specific substring from a font name.
 */
    ساكن فراغ
get_part(حرف *اندريون, صحيح pos, حرف *ٻاهريون)
{
    صحيح	i;
    صحيح j;

    *ٻاهريون = '\0';

    لكل (i = 0; (pos > 0) && (اندريون[i] != '\0'); ++i)
	إذا (اندريون[i] == '-')
	    pos--;

    إذا (اندريون[i] == '\0')
	أرجع;

    لكل (j = 0; (اندريون[i] != '-') && (اندريون[i] != '\0'); ++i, ++j)
	ٻاهريون[j] = اندريون[i];
    ٻاهريون[j] = '\0';
}

/*
 * Given a font name this function returns the part used in the first
 * scroll list.
 */
    ساكن فراغ
name_part(حرف *font, حرف *buf)
{
    حرف    buf2[عارضي_BUF_سائيز];
    حرف    buf3[عارضي_BUF_سائيز];

    get_part(font, 2, buf2);
    get_part(font, 1, buf3);

    إذا (*buf3 != NUL)
	vim_snprintf(buf, عارضي_BUF_سائيز, "%s (%s)", buf2, buf3);
    وإلا
	vim_snprintf(buf, عارضي_BUF_سائيز, "%s", buf2);
}

/*
 * Given a font name this function returns the part used in the second scroll list.
 */
    ساكن فراغ
style_part(حرف *font, حرف *buf)
{
    حرف    buf2[عارضي_BUF_سائيز];
    حرف    buf3[عارضي_BUF_سائيز];

    get_part(font, 3, buf3);
    get_part(font, 5, buf2);

    إذا (!strcmp(buf2, "normal") && !strcmp(buf2, "Normal")
						   && !strcmp(buf2, "NORMAL"))
	vim_snprintf(buf, عارضي_BUF_سائيز, "%s %s", buf3, buf2);
    وإلا
	strcpy(buf, buf3);

    get_part(font, 6, buf2);

    إذا (buf2[0] != '\0')
	vim_snprintf(buf3, عارضي_BUF_سائيز, "%s %s", buf, buf2);
    وإلا
	strcpy(buf3, buf);

    get_part(font, 4, buf2);

    إذا (!strcmp(buf2, "o") || !strcmp(buf2, "O"))
	vim_snprintf(buf, عارضي_BUF_سائيز, "%s oblique", buf3);
    وإلا إذا (!strcmp(buf2, "i") || !strcmp(buf2, "I"))
	vim_snprintf(buf, عارضي_BUF_سائيز, "%s italic", buf3);

    إذا (!strcmp(buf, " "))
	strcpy(buf, "-");
}

/*
 * Given a font name this function returns the part used in the third
 * scroll list.
 */
    ساكن فراغ
سائيز_part(حرف *font, حرف *buf, صحيح اندريون_Pixels)
{
    صحيح	    سائيز;
    عائم   عارضي;

    *buf = '\0';

    إذا (اندريون_Pixels)
    {
	get_part(font, 7, buf);
	إذا (*buf != NUL)
	{
	    سائيز = atoi(buf);
	    sprintf(buf, "%3d", سائيز);
	}
    }
    وإلا
    {
	get_part(font, 8, buf);
	إذا (*buf != NUL)
	{
	    سائيز = atoi(buf);
	    عارضي = (عائم)سائيز / 10.0;
	    سائيز = عارضي;
	    إذا (buf[لڙي_ڊيگهه(buf) - 1] == '0')
		sprintf(buf, "%3d", سائيز);
	    وإلا
		sprintf(buf, "%4.1f", عارضي);
	}
    }
}

/*
 * Given a font name this function returns the part used in the choice menu.
 */
    ساكن فراغ
encoding_part(حرف *font, حرف *buf)
{
    حرف    buf1[عارضي_BUF_سائيز];
    حرف    buf2[عارضي_BUF_سائيز];

    *buf = '\0';

    get_part(font, 13, buf1);
    get_part(font, 14, buf2);

    إذا (*buf1 != NUL && *buf2 != NUL)
	vim_snprintf(buf, عارضي_BUF_سائيز, "%s-%s", buf1, buf2);
    إذا (!strcmp(buf, " "))
	strcpy(buf, "-");
}

/*
 * Inserts a string into correct sorted position in a list.
 */
    ساكن فراغ
جوڙيو_منزل_list(حرف **buf, حرف *شي, صحيح *ڳڻپ)
{
    صحيح	i;
    صحيح j;

    إذا (*ڳڻپ == MAX_ENTRIES_اندريون_LIST)
	أرجع;

    // avoid duplication
    لكل (i = 0; i < *ڳڻپ; ++i)
    {
	إذا (!strcmp(buf[i], شي))
	    أرجع;
    }

    // find order place, but make sure that wild card comes first
    إذا (!strcmp(شي, wild))
	i = 0;
    وإلا
	لكل (i = 0; i < *ڳڻپ; ++i)
	    إذا (strcmp(buf[i], شي) > 0 && strcmp(buf[i], wild))
		اكسر;

    // now insert the item
    لكل (j = *ڳڻپ; j > i; --j)
	buf[j] = buf[j-1];
    buf[i] = XtNewString(شي);

    ++(*ڳڻپ);
}

/*
 * True if the font matches some field.
 */
    ساكن Boolean
ميلاپ(Shared_Font_Sel_معلومات_2 *معلومات, تعداد ListSpecifier l, صحيح i)
{
    حرف buf[عارضي_BUF_سائيز];

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

	حالة سائيز_3:
	    سائيز_part(fn(معلومات, i), buf, معلومات->اندريون_pixels);
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
    حرف buf[عارضي_BUF_سائيز];

    get_part(font, 11, buf);

    أرجع !strcmp(buf, "p") || !strcmp(buf, "P");
}


ساكن فراغ encoding_callback(Widget w, Shared_Font_Sel_معلومات_2 *معلومات,
							     Xt_اشارو dummy);

/*
 * Parse through the fontlist data and set up the three scroll lists.  The fix
 * parameter can be used to exclude a list from any changes.  This is used for
 * updates after selections caused by the users actions.
 */
    ساكن فراغ
fill_lists(تعداد ListSpecifier fix, Shared_Font_Sel_معلومات_2 *معلومات)
{
    حرف	*list[NONE][MAX_ENTRIES_اندريون_LIST];
    صحيح		ڳڻپ[NONE];
    حرف	buf[عارضي_BUF_سائيز];
    XmString	شيون[MAX_ENTRIES_اندريون_LIST];
    صحيح		i;
    صحيح		idx;

    لكل (idx = (صحيح)ENCODING; idx < (صحيح)NONE; ++idx)
	ڳڻپ[idx] = 0;

    // First we insert the wild char into every single list.
    إذا (fix != ENCODING)
	جوڙيو_منزل_list(list[ENCODING], wild, &ڳڻپ[ENCODING]);
    إذا (fix != NAME)
	جوڙيو_منزل_list(list[NAME], wild, &ڳڻپ[NAME]);
    إذا (fix != STYLE)
	جوڙيو_منزل_list(list[STYLE], wild, &ڳڻپ[STYLE]);
    إذا (fix != سائيز_3)
	جوڙيو_منزل_list(list[سائيز_3], wild, &ڳڻپ[سائيز_3]);

    لكل (i = 0; i < معلومات->انگ && i < MAX_ENTRIES_اندريون_LIST; i++)
    {
	إذا (proportional(fn(معلومات, i)))
	    استمر;

	إذا (fix != ENCODING
		&& ميلاپ(معلومات, NAME, i)
		&& ميلاپ(معلومات, STYLE, i)
		&& ميلاپ(معلومات, سائيز_3, i))
	{
	    encoding_part(fn(معلومات, i), buf);
	    جوڙيو_منزل_list(list[ENCODING], buf, &ڳڻپ[ENCODING]);
	}

	إذا (fix != NAME
		&& ميلاپ(معلومات, ENCODING, i)
		&& ميلاپ(معلومات, STYLE, i)
		&& ميلاپ(معلومات, سائيز_3, i))
	{
	    name_part(fn(معلومات, i), buf);
	    جوڙيو_منزل_list(list[NAME], buf, &ڳڻپ[NAME]);
	}

	إذا (fix != STYLE
		&& ميلاپ(معلومات, ENCODING, i)
		&& ميلاپ(معلومات, NAME, i)
		&& ميلاپ(معلومات, سائيز_3, i))
	{
	    style_part(fn(معلومات, i), buf);
	    جوڙيو_منزل_list(list[STYLE], buf, &ڳڻپ[STYLE]);
	}

	إذا (fix != سائيز_3
		&& ميلاپ(معلومات, ENCODING, i)
		&& ميلاپ(معلومات, NAME, i)
		&& ميلاپ(معلومات, STYLE, i))
	{
	    سائيز_part(fn(معلومات, i), buf, معلومات->اندريون_pixels);
	    جوڙيو_منزل_list(list[سائيز_3], buf, &ڳڻپ[سائيز_3]);
	}
    }

    /*
     * And now do the preselection in all lists where there was one:
     */

    إذا (fix != ENCODING)
    {
	Cardinal n_شيون;
	WidgetList children;
	Widget selected_button = 0;

	// Get and update the current button list.
	Xt_Va_Get_قدرون(معلومات->encoding_pulldown,
		XmNchildren, &children,
		XmNnumChildren, &n_شيون,
		عدم);

	لكل (i = 0; i < ڳڻپ[ENCODING]; ++i)
	{
	    Widget button;

	    شيون[i] = XmStringCreateLocalized(list[ENCODING][i]);

	    إذا (i < (صحيح)n_شيون)
	    {
		// recycle old button
		Xt_Va_سيٽ_قدرون(children[i],
			XmNlabelString, شيون[i],
			Xm_Nuser_معلومات, i,
			عدم);
		button = children[i];
	    }
	    وإلا
	    {
		// create a new button
		button = XtVaCreateManagedWidget("button",
			xm_ڌڪو_Button_Gadget_Class,
			معلومات->encoding_pulldown,
			XmNlabelString, شيون[i],
			Xm_Nuser_معلومات, i,
			عدم);
		Xt_جوڙيو_Callback(button, XmNactivateCallback,
			(XtCallbackProc) encoding_callback, (Xt_اشارو) معلومات);
		Xt_Manage_ٻار(button);
	    }

	    إذا (معلومات->sel[ENCODING])
	    {
		إذا (!strcmp(معلومات->sel[ENCODING], list[ENCODING][i]))
		    selected_button = button;
	    }
	    XtFree(list[ENCODING][i]);
	}

	// Destroy all the outstanding menu items.
	لكل (i = ڳڻپ[ENCODING]; i < (صحيح)n_شيون; ++i)
	{
	    Xt_Unmanage_ٻار(children[i]);
	    XtDestroyWidget(children[i]);
	}

	// Preserve the current selection visually.
	إذا (selected_button)
	{
	    Xt_Va_سيٽ_قدرون(معلومات->encoding_menu,
		    XmNmenuHistory, selected_button,
		    عدم);
	}

	لكل (i = 0; i < ڳڻپ[ENCODING]; ++i)
	    XmStringFree(شيون[i]);
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
	    حالة سائيز_3:
		w = معلومات->list[سائيز_3];
		اكسر;
	    افتراضي:
		w = (Widget)0;	// for lint
	}

	لكل (i = 0; i < ڳڻپ[idx]; ++i)
	{
	    شيون[i] = XmStringCreateLocalized(list[idx][i]);
	    XtFree(list[idx][i]);
	}
	Xm_List_Delete_All_شيون(w);
	Xm_List_جوڙيو_شيون(w, شيون, ڳڻپ[idx], 1);
	إذا (معلومات->sel[idx])
	{
	    XmStringFree(شيون[0]);
	    شيون[0] = XmStringCreateLocalized(معلومات->sel[idx]);
	    Xm_List_Select_شي(w, شيون[0], False);
	    Xm_List_سيٽ_Bottom_شي(w, شيون[0]);
	}
	لكل (i = 0; i < ڳڻپ[idx]; ++i)
	    XmStringFree(شيون[i]);
    }
}

    ساكن فراغ
stoggle_callback(Widget w اڻ_استعمال_ٿيل,
	Shared_Font_Sel_معلومات_2 *معلومات,
	XmToggleButtonCallbackStruct *call_معلومات)
{
    صحيح		i, do_sel;
    حرف	new_سائيز_2[عارضي_BUF_سائيز];
    XmString	str;

    إذا (call_معلومات->reason != (صحيح)Xm_CR_قدر_بدليو)
	أرجع;

    do_sel = (معلومات->sel[سائيز_3] != عدم) && strcmp(معلومات->sel[سائيز_3], wild);

    لكل (i = 0; do_sel && (i < معلومات->انگ); i++)
	إذا (ميلاپ(معلومات, ENCODING, i)
		&& ميلاپ(معلومات, NAME, i)
		&& ميلاپ(معلومات, STYLE, i)
		&& ميلاپ(معلومات, سائيز_3, i))
	{
	    سائيز_part(fn(معلومات, i), new_سائيز_2, !معلومات->اندريون_pixels);
	    اكسر;
	}

    معلومات->اندريون_pixels = !معلومات->اندريون_pixels;

    إذا (معلومات->sel[سائيز_3])
	XtFree(معلومات->sel[سائيز_3]);
    معلومات->sel[سائيز_3] = عدم;
    fill_lists(NONE, معلومات);

    إذا (do_sel)
    {
	str = XmStringCreateLocalized(new_سائيز_2);
	Xm_List_Select_شي(معلومات->list[سائيز_3], str, True);
	Xm_List_سيٽ_Bottom_شي(معلومات->list[سائيز_3], str);
	XmStringFree(str);
    }
}

/*
 * Show the currently selected font in the sample text label.
 */
    ساكن فراغ
display_sample(Shared_Font_Sel_معلومات_2 *معلومات)
{
    Arg		    دليلون[2];
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
    Xt_سيٽ_Arg(دليلون[n], XmNlabelString, str); n++;
    Xt_سيٽ_Arg(دليلون[n], XmNfontList, font_list); n++;

    Xt_سيٽ_قدرون(معلومات->sample, دليلون, n);
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

    XmStringGetLtoR(call_معلومات->شي, XmSTRING_DEFAULT_CHARSET, &sel);

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
		Xm_List_Deselect_شي(w, call_معلومات->شي);
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
	    && معلومات->sel[سائيز_3]
	    && strcmp(معلومات->sel[ENCODING], wild)
	    && strcmp(معلومات->sel[NAME], wild)
	    && strcmp(معلومات->sel[STYLE], wild)
	    && strcmp(معلومات->sel[سائيز_3], wild))
    {
	صحيح i;

	إذا (معلومات->font_name)
	    XtFree(معلومات->font_name);
	معلومات->font_name = عدم;

	لكل (i = 0; i < معلومات->انگ; i++)
	{
	    إذا (ميلاپ(معلومات, ENCODING, i)
		    && ميلاپ(معلومات, NAME, i)
		    && ميلاپ(معلومات, STYLE, i)
		    && ميلاپ(معلومات, سائيز_3, i))
	    {
		معلومات->font_name = XtNewString(fn(معلومات, i));
		اكسر;
	    }
	}

	إذا (معلومات->font_name)
	{
	    Xm_متن_سيٽ_String(معلومات->name, معلومات->font_name);
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
	Arg	    دليلون[4];
	حرف	    *nomatch_نياپو = _("no specific match");

	n = 0;
	str = XmStringCreateLocalized(nomatch_نياپو);
	Xt_سيٽ_Arg(دليلون[n], XmNlabelString, str); ++n;
	Xt_سيٽ_قدرون(معلومات->sample, دليلون, n);
	apply_fontlist(معلومات->sample);
	Xm_متن_سيٽ_String(معلومات->name, nomatch_نياپو);
	XmStringFree(str);

	أرجع False;
    }
}

    ساكن فراغ
encoding_callback(Widget w,
	Shared_Font_Sel_معلومات_2 *معلومات,
	Xt_اشارو dummy اڻ_استعمال_ٿيل)
{
    XmString str;
    XmListCallbackStruct fake_معلومات;

    Xt_Va_Get_قدرون(w, XmNlabelString, &str, عدم);

    إذا (!str)
	أرجع;

    fake_معلومات.شي = str;

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
سائيز_callback(Widget w,
	Shared_Font_Sel_معلومات_2 *معلومات,
	XmListCallbackStruct *call_معلومات)
{
    do_choice(w, معلومات, call_معلومات, سائيز_3);
}

    ساكن فراغ
cancel_callback(Widget w اڻ_استعمال_ٿيل,
	Shared_Font_Sel_معلومات_2 *معلومات,
	XmListCallbackStruct *call_معلومات اڻ_استعمال_ٿيل)
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
    إذا (معلومات->sel[سائيز_3])
    {
	XtFree(معلومات->sel[سائيز_3]);
	معلومات->sel[سائيز_3] = عدم;
    }

    إذا (معلومات->font_name)
	XtFree(معلومات->font_name);
    معلومات->font_name = عدم;

    معلومات->انگ = 0;
    XFree_Font_نالا(معلومات->نالا);
    معلومات->نالا = عدم;
    معلومات->ٻاهر_نڪرو = True;
}

    ساكن فراغ
ok_callback(Widget w اڻ_استعمال_ٿيل,
	Shared_Font_Sel_معلومات_2 *معلومات,
	Xm_ڌڪو_Button_Callback_Struct *call_معلومات اڻ_استعمال_ٿيل)
{
    حرف    *نمونو;
    حرف    **name;
    صحيح	    i;

    نمونو = Xm_متن_Get_String(معلومات->name);
    name    = XListFonts(XtDisplay(معلومات->dialog), نمونو, 1, &i);
    XtFree(نمونو);

    إذا (i != 1)
    {
	do_dialog(VIM_ERROR,
		(char_u *)_("Error"),
		(char_u *)_("Invalid font specification"),
		(char_u *)_("&Dismiss"), 1, عدم, FALSE);
	XFree_Font_نالا(name);
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
	إذا (معلومات->sel[سائيز_3])
	{
	    XtFree(معلومات->sel[سائيز_3]);
	    معلومات->sel[سائيز_3] = عدم;
	}

	XFree_Font_نالا(name);

	معلومات->انگ = 0;
	XFree_Font_نالا(معلومات->نالا);
	معلومات->نالا = عدم;
	معلومات->ٻاهر_نڪرو = True;
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

    Widget		والد;
    Widget		form;
    Widget		separator;
    Widget		sub_form;
    Widget		سائيز_toggle;
    Widget		name;
    Widget		disp_چوکٺو;
    Widget		چوکٺو;
    Arg			دليلون[64];
    صحيح			n;
    XmString		str;
    حرف		big_font[MAX_FONT_NAME_LEN];
    Shared_Font_Sel_معلومات_2	*معلومات;

    معلومات = &معلومات_3;

    والد = vimShell;
    معلومات->نالا = XListFonts(XtDisplay(والد), "-*-*-*-*-*-*-*-*-*-*-*-*-*-*",
						       MAX_FONTS, &معلومات->انگ);

    /*
     * Find the name of the biggest font less than the given limit
     * MAX_DISPLAY_SIZE used to set up the initial height of the display
     * widget.
     */

    {
	صحيح	i;
	صحيح	max;
	صحيح	idx = 0;
	صحيح	سائيز;
	حرف	buf[128];

	لكل (i = 0, max = 0; i < معلومات->انگ; i++)
	{
	    get_part(fn(معلومات, i), 7, buf);
	    سائيز = atoi(buf);
	    إذا ((سائيز > max) && (سائيز < MAX_DISPLAY_سائيز))
	    {
		idx = i;
		max = سائيز;
	    }
	}
	strcpy(big_font, fn(معلومات, idx));
    }
    معلومات->old = XLoadQueryFont(XtDisplay(والد), big_font);
    معلومات->old_list = gui_motif_create_fontlist(معلومات->old);

    // Set the title of the Dialog window.
    معلومات->dialog = XmCreateDialogShell(والد, "fontSelector", عدم, 0);
    str = XmStringCreateLocalized(_("Vim - Font Selector"));

    // Create form popup dialog widget.
    form = XtVaCreateWidget("form",
	    xmFormWidgetClass, معلومات->dialog,
	    XmNdialogTitle, str,
	    XmNautoUnmanage, False,
	    XmNdialogStyle, Xm_DIALOG_ڀريل_APPLICATION_MODAL,
	    عدم);
    XmStringFree(str);

    sub_form = XtVaCreateManagedWidget("subForm",
	    xmFormWidgetClass, form,
	    XmNbottomAttachment, XmATTACH_FORM,
	    Xm_Nbottom_وٿي, 4,
	    XmNrightAttachment, XmATTACH_FORM,
	    Xm_Nright_وٿي, 4,
	    XmNtopAttachment, XmATTACH_FORM,
	    Xm_Ntop_وٿي, 4,
	    XmNorientation, XmVERTICAL,
	    عدم);

    معلومات->ok = XtVaCreateManagedWidget(_("OK"),
	    xm_ڌڪو_Button_Gadget_Class, sub_form,
	    XmNleftAttachment, XmATTACH_FORM,
	    XmNrightAttachment, XmATTACH_FORM,
	    XmNtopAttachment, XmATTACH_FORM,
	    Xm_Ntop_وٿي, 4,
	    عدم);
    apply_fontlist(معلومات->ok);

    معلومات->cancel = XtVaCreateManagedWidget(_("Cancel"),
	    xm_ڌڪو_Button_Gadget_Class, sub_form,
	    XmNrightAttachment, XmATTACH_FORM,
	    XmNleftAttachment, XmATTACH_FORM,
	    XmNtopAttachment, XmATTACH_WIDGET,
	    XmNtopWidget, معلومات->ok,
	    Xm_Ntop_وٿي, 4,
	    XmNshowAsDefault, True,
	    عدم);
    apply_fontlist(معلومات->cancel);

    // Create the separator for beauty.
    n = 0;
    Xt_سيٽ_Arg(دليلون[n], XmNorientation, XmVERTICAL); n++;
    Xt_سيٽ_Arg(دليلون[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    Xt_سيٽ_Arg(دليلون[n], XmNtopAttachment, XmATTACH_FORM); n++;
    Xt_سيٽ_Arg(دليلون[n], XmNrightAttachment, XmATTACH_WIDGET); n++;
    Xt_سيٽ_Arg(دليلون[n], XmNrightWidget, sub_form); n++;
    Xt_سيٽ_Arg(دليلون[n], Xm_Nright_وٿي, 4); n++;
    separator = XmCreateSeparatorGadget(form, "separator", دليلون, n);
    Xt_Manage_ٻار(separator);

    // Create font name text widget and the corresponding label.
    معلومات->name = XtVaCreateManagedWidget("fontName",
	    xm_متن_Widget_Class, form,
	    XmNbottomAttachment, XmATTACH_FORM,
	    Xm_Nbottom_وٿي, 4,
	    XmNleftAttachment, XmATTACH_FORM,
	    Xm_Nleft_وٿي, 4,
	    XmNrightAttachment, XmATTACH_WIDGET,
	    XmNrightWidget, separator,
	    Xm_Nright_وٿي, 4,
	    XmNeditable, False,
	    Xm_Nedit_گهڻائي, Xm_SINGLE_سٽ_EDIT,
	    Xm_Nmax_ڊيگهائي, MAX_FONT_NAME_LEN,
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
	    Xm_Ntop_وٿي, 1,
	    عدم);
    XmStringFree(str);
    apply_fontlist(name);

    // create sample display label widget
    disp_چوکٺو = XtVaCreateManagedWidget("sampleFrame",
	    xm_چوکٺو_Widget_Class, form,
	    XmNshadowType, Xm_SHADOW_ETCHED_اندريون,
	    XmNleftAttachment, XmATTACH_FORM,
	    Xm_Nleft_وٿي, 4,
	    XmNbottomAttachment, XmATTACH_WIDGET,
	    XmNbottomWidget, name,
	    XmNrightAttachment, XmATTACH_WIDGET,
	    XmNrightWidget, separator,
	    Xm_Nright_وٿي, 4,
	    XmNalignment, XmALIGNMENT_BEGINNING,
	    عدم);

    معلومات->sample = XtVaCreateManagedWidget("sampleLabel",
	    xmLabelWidgetClass, disp_چوکٺو,
	    XmNleftAttachment, XmATTACH_FORM,
	    XmNtopAttachment, XmATTACH_FORM,
	    XmNbottomAttachment, XmATTACH_FORM,
	    XmNrightAttachment, XmATTACH_FORM,
	    XmNalignment, XmALIGNMENT_BEGINNING,
	    Xm_Nrecompute_سائيز, False,
	    XmNfontList, معلومات->old_list,
	    عدم);

    // create toggle button
    str = XmStringCreateLocalized(_("Show size in Points"));
    سائيز_toggle = XtVaCreateManagedWidget("sizeToggle",
	    xmToggleButtonGadgetClass, form,
	    XmNlabelString, str,
	    XmNleftAttachment, XmATTACH_FORM,
	    Xm_Nleft_وٿي, 4,
	    XmNbottomAttachment, XmATTACH_WIDGET,
	    XmNbottomWidget, disp_چوکٺو,
	    Xm_Nbottom_وٿي, 4,
	    عدم);
    XmStringFree(str);
    apply_fontlist(سائيز_toggle);
    Xt_Manage_ٻار(سائيز_toggle);

    // Encoding pulldown menu.

    معلومات->encoding_pulldown = XmCreatePulldownMenu(form,
						 "encodingPulldown", عدم, 0);
    str = XmStringCreateLocalized(_("Encoding:"));
    n = 0;
    Xt_سيٽ_Arg(دليلون[n], XmNsubMenuId, معلومات->encoding_pulldown); ++n;
    Xt_سيٽ_Arg(دليلون[n], XmNlabelString, str); ++n;
    Xt_سيٽ_Arg(دليلون[n], XmNleftAttachment, XmATTACH_FORM); ++n;
    Xt_سيٽ_Arg(دليلون[n], Xm_Nleft_وٿي, 4); ++n;
    Xt_سيٽ_Arg(دليلون[n], XmNbottomAttachment, XmATTACH_WIDGET); ++n;
    Xt_سيٽ_Arg(دليلون[n], XmNbottomWidget, سائيز_toggle); ++n;
    Xt_سيٽ_Arg(دليلون[n], Xm_Nbottom_وٿي, 4); ++n;
    Xt_سيٽ_Arg(دليلون[n], XmNrightAttachment, XmATTACH_WIDGET); ++n;
    Xt_سيٽ_Arg(دليلون[n], XmNrightWidget, separator); ++n;
    Xt_سيٽ_Arg(دليلون[n], Xm_Nright_وٿي, 4); ++n;
    معلومات->encoding_menu = XmCreateOptionMenu(form, "encodingMenu", دليلون, n);
    XmStringFree(str);
    Xm_جوڙيو_Tab_گروهه(معلومات->encoding_menu);

    /*
     * Create scroll list widgets in a separate subform used to manage the
     * different sizes of the lists.
     */

    sub_form = XtVaCreateManagedWidget("subForm",
	    xmFormWidgetClass, form,
	    XmNbottomAttachment, XmATTACH_WIDGET,
	    XmNbottomWidget, معلومات->encoding_menu,
	    Xm_Nbottom_وٿي, 4,
	    XmNleftAttachment, XmATTACH_FORM,
	    Xm_Nleft_وٿي, 4,
	    XmNrightAttachment, XmATTACH_WIDGET,
	    XmNrightWidget, separator,
	    Xm_Nright_وٿي, 4,
	    XmNtopAttachment, XmATTACH_FORM,
	    Xm_Ntop_وٿي, 2,
	    XmNorientation, XmVERTICAL,
	    عدم);

    // font list
    چوکٺو = XtVaCreateManagedWidget("frame", xm_چوکٺو_Widget_Class, sub_form,
	    XmNshadowThickness, 0,
	    XmNtopAttachment, XmATTACH_FORM,
	    XmNbottomAttachment, XmATTACH_FORM,
	    XmNleftAttachment, XmATTACH_FORM,
	    XmNrightAttachment, XmATTACH_POSITION,
	    XmNrightPosition, 50,
	    عدم);

    str = XmStringCreateLocalized(_("Font:"));
    name = XtVaCreateManagedWidget("nameListLabel", xmLabelGadgetClass, چوکٺو,
	    XmNchildType, Xm_چوکٺو_TITLE_ٻار,
	    XmNchildVerticalAlignment, XmALIGNMENT_CENTER,
	    XmNchildHorizontalAlignment, XmALIGNMENT_BEGINNING,
	    XmNlabelString, str,
	    عدم);
    XmStringFree(str);
    apply_fontlist(name);

    n = 0;
    Xt_سيٽ_Arg(دليلون[n], Xm_Nvisible_شي_ڳڻپ, 8); ++n;
    Xt_سيٽ_Arg(دليلون[n], XmNresizable, True); ++n;
    Xt_سيٽ_Arg(دليلون[n], Xm_Nlist_سائيز_Policy, XmCONSTANT); ++n;
    Xt_سيٽ_Arg(دليلون[n], XmNvisualPolicy, XmVARIABLE); ++n;
#جيڪڏهن_وصف_ٿيل LESSTIF_نسخو
    Xt_سيٽ_Arg(دليلون[n], XmNscrollBarDisplayPolicy, XmSTATIC); ++n;
#جيڪڏهن_پورو
    معلومات->list[NAME] = XmCreateScrolledList(چوکٺو, "fontList", دليلون, n);
    Xt_Va_سيٽ_قدرون(name, Xm_Nuser_معلومات, معلومات->list[NAME], عدم);

    // style list
    چوکٺو = XtVaCreateManagedWidget("frame", xm_چوکٺو_Widget_Class, sub_form,
	    XmNshadowThickness, 0,
	    XmNtopAttachment, XmATTACH_FORM,
	    XmNbottomAttachment, XmATTACH_FORM,
	    XmNleftAttachment, XmATTACH_POSITION,
	    XmNleftPosition, 50,
	    Xm_Nleft_وٿي, 4,
	    XmNrightAttachment, XmATTACH_POSITION,
	    XmNrightPosition, 80,
	    عدم);

    str = XmStringCreateLocalized(_("Style:"));
    name = XtVaCreateManagedWidget("styleListLabel", xmLabelWidgetClass, چوکٺو,
	    XmNchildType, Xm_چوکٺو_TITLE_ٻار,
	    XmNchildVerticalAlignment, XmALIGNMENT_CENTER,
	    XmNchildHorizontalAlignment, XmALIGNMENT_BEGINNING,
	    XmNlabelString, str,
	    عدم);
    XmStringFree(str);
    apply_fontlist(name);

    n = 0;
    Xt_سيٽ_Arg(دليلون[n], Xm_Nvisible_شي_ڳڻپ, 8); ++n;
    Xt_سيٽ_Arg(دليلون[n], XmNresizable, True); ++n;
    Xt_سيٽ_Arg(دليلون[n], Xm_Nlist_سائيز_Policy, XmCONSTANT); ++n;
    Xt_سيٽ_Arg(دليلون[n], XmNvisualPolicy, XmVARIABLE); ++n;
#جيڪڏهن_وصف_ٿيل LESSTIF_نسخو
    Xt_سيٽ_Arg(دليلون[n], XmNscrollBarDisplayPolicy, XmSTATIC); ++n;
#جيڪڏهن_پورو
    معلومات->list[STYLE] = XmCreateScrolledList(چوکٺو, "styleList", دليلون, n);
    Xt_Va_سيٽ_قدرون(name, Xm_Nuser_معلومات, معلومات->list[STYLE], عدم);

    // size list
    چوکٺو = XtVaCreateManagedWidget("frame", xm_چوکٺو_Widget_Class, sub_form,
	    XmNshadowThickness, 0,
	    XmNtopAttachment, XmATTACH_FORM,
	    XmNbottomAttachment, XmATTACH_FORM,
	    XmNleftAttachment, XmATTACH_POSITION,
	    XmNleftPosition, 80,
	    Xm_Nleft_وٿي, 4,
	    XmNrightAttachment, XmATTACH_FORM,
	    عدم);

    str = XmStringCreateLocalized(_("Size:"));
    name = XtVaCreateManagedWidget("sizeListLabel", xmLabelGadgetClass, چوکٺو,
	    XmNchildType, Xm_چوکٺو_TITLE_ٻار,
	    XmNchildVerticalAlignment, XmALIGNMENT_CENTER,
	    XmNchildHorizontalAlignment, XmALIGNMENT_BEGINNING,
	    XmNlabelString, str,
	    عدم);
    XmStringFree(str);
    apply_fontlist(name);

    n = 0;
    Xt_سيٽ_Arg(دليلون[n], Xm_Nvisible_شي_ڳڻپ, 8); ++n;
    Xt_سيٽ_Arg(دليلون[n], XmNresizable, True); ++n;
    Xt_سيٽ_Arg(دليلون[n], Xm_Nlist_سائيز_Policy, XmCONSTANT); ++n;
    Xt_سيٽ_Arg(دليلون[n], XmNvisualPolicy, XmVARIABLE); ++n;
#جيڪڏهن_وصف_ٿيل LESSTIF_نسخو
    Xt_سيٽ_Arg(دليلون[n], XmNscrollBarDisplayPolicy, XmSTATIC); ++n;
#جيڪڏهن_پورو
    معلومات->list[سائيز_3] = XmCreateScrolledList(چوکٺو, "sizeList", دليلون, n);
    Xt_Va_سيٽ_قدرون(name, Xm_Nuser_معلومات, معلومات->list[سائيز_3], عدم);

    // update form widgets cancel button
    Xt_Va_سيٽ_قدرون(form, XmNcancelButton, معلومات->cancel, عدم);

    Xt_جوڙيو_Callback(سائيز_toggle, Xm_Nvalue_بدليو_Callback,
	    (XtCallbackProc)stoggle_callback, (Xt_اشارو)معلومات);
    Xt_جوڙيو_Callback(معلومات->list[NAME], XmNbrowseSelectionCallback,
	    (XtCallbackProc)name_callback, (Xt_اشارو)معلومات);
    Xt_جوڙيو_Callback(معلومات->list[STYLE], XmNbrowseSelectionCallback,
	    (XtCallbackProc)style_callback, (Xt_اشارو)معلومات);
    Xt_جوڙيو_Callback(معلومات->list[سائيز_3], XmNbrowseSelectionCallback,
	    (XtCallbackProc)سائيز_callback, (Xt_اشارو)معلومات);
    Xt_جوڙيو_Callback(معلومات->ok, XmNactivateCallback,
	    (XtCallbackProc)ok_callback, (Xt_اشارو)معلومات);
    Xt_جوڙيو_Callback(معلومات->cancel, XmNactivateCallback,
	    (XtCallbackProc)cancel_callback, (Xt_اشارو)معلومات);

    XmProcessTraversal(معلومات->list[NAME], XmTRAVERSE_CURRENT);

    // setup tabgroups

    Xm_جوڙيو_Tab_گروهه(معلومات->list[NAME]);
    Xm_جوڙيو_Tab_گروهه(معلومات->list[STYLE]);
    Xm_جوڙيو_Tab_گروهه(معلومات->list[سائيز_3]);
    Xm_جوڙيو_Tab_گروهه(سائيز_toggle);
    Xm_جوڙيو_Tab_گروهه(معلومات->name);
    Xm_جوڙيو_Tab_گروهه(معلومات->ok);
    Xm_جوڙيو_Tab_گروهه(معلومات->cancel);

    جوڙيو_cancel_action(معلومات->dialog, (XtCallbackProc)cancel_callback, معلومات);

    // Preset selection data.

    معلومات->ٻاهر_نڪرو = False;
    معلومات->اندريون_pixels= True;
    معلومات->sel[ENCODING] = عدم;
    معلومات->sel[NAME] = عدم;
    معلومات->sel[STYLE] = عدم;
    معلومات->sel[سائيز_3] = عدم;
    معلومات->font_name = عدم;

    // set up current font parameters
    إذا (current && current[0] != '\0')
    {
	صحيح	    i;
	حرف	    **نالا;

	نالا = XListFonts(XtDisplay(form), (حرف *) current, 1, &i);

	إذا (i != 0)
	{
	    حرف namebuf[عارضي_BUF_سائيز];
	    حرف stylebuf[عارضي_BUF_سائيز];
	    حرف sizebuf[عارضي_BUF_سائيز];
	    حرف encodingbuf[عارضي_BUF_سائيز];
	    حرف *مليو;

	    مليو = نالا[0];

	    name_part(مليو, namebuf);
	    style_part(مليو, stylebuf);
	    سائيز_part(مليو, sizebuf, معلومات->اندريون_pixels);
	    encoding_part(مليو, encodingbuf);

	    إذا (*namebuf != NUL
		    && *stylebuf != NUL
		    && *sizebuf != NUL
		    && *encodingbuf != NUL)
	    {
		معلومات->sel[NAME] = XtNewString(namebuf);
		معلومات->sel[STYLE] = XtNewString(stylebuf);
		معلومات->sel[سائيز_3] = XtNewString(sizebuf);
		معلومات->sel[ENCODING] = XtNewString(encodingbuf);
		معلومات->font_name = XtNewString(نالا[0]);
		display_sample(معلومات);
		Xm_متن_سيٽ_String(معلومات->name, معلومات->font_name);
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
	XFree_Font_نالا(نالا);
    }

    fill_lists(NONE, معلومات);

    // Unfortunately LessTif doesn't align the list widget's properly.  I don't
    // have currently any idea how to fix this problem.
    Xt_Manage_ٻار(معلومات->list[NAME]);
    Xt_Manage_ٻار(معلومات->list[STYLE]);
    Xt_Manage_ٻار(معلومات->list[سائيز_3]);
    Xt_Manage_ٻار(معلومات->encoding_menu);
    manage_centered(form);

    // modal event loop
    طالما (!معلومات->ٻاهر_نڪرو)
	XtAppProcessEvent(Xt_Widget_منزل_Application_Context(معلومات->dialog),
							(Xt_داخلا_نقاب)XtIMAll);

    إذا (معلومات->old)
    {
	XFreeFont(XtDisplay(معلومات->dialog),  معلومات->old);
	XmFontListFree(معلومات->old_list);
    }
    XtDestroyWidget(معلومات->dialog);

    gui_motif_synch_fonts();

    أرجع (char_u *) معلومات->font_name;
}
