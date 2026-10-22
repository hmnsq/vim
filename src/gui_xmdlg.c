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

#جے_تعریف_شدہ FEAT_MENU
# define apply_fontlist(w) gui_motif_menu_fontlist(w)
#وإلا
# define apply_fontlist(w)
#جے_انت

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
جوڑو_cancel_action(Widget shell, XtCallbackProc close_callback, فراغ *arg)
{
    ساكن Atom wmp_atom = 0;
    ساكن Atom dw_atom = 0;
    Display *display = XtDisplay(shell);

    // deactivate the built-in delete response of killing the application
    Xt_Va_سیٹ_اقدار(shell, XmNdeleteResponse, XmDO_NOTHING, عدم);

    // add a delete window protocol callback instead
    إذا (!dw_atom)
    {
	wmp_atom = XmInternAtom(display, "WM_PROTOCOLS", True);
	dw_atom = XmInternAtom(display, "WM_DELETE_WINDOW", True);
    }
    Xm_جوڑو_Protocol_Callback(shell, wmp_atom, dw_atom, close_callback, arg);
}

#تعریف_کرو MAX_FONTS			65535
#تعریف_کرو MAX_FONT_NAME_LEN		256
#تعریف_کرو MAX_ENTRIES_وچ_LIST		5000
#تعریف_کرو MAX_DISPLAY_ناپ		150
#تعریف_کرو عارضی_BUF_ناپ			256

تعداد ListSpecifier
{
    ENCODING,
    NAME,
    STYLE,
    ناپ_3,
    NONE
};

تعريفنوع بنية Shared_Font_Sel_انکڑے
{
    Widget	dialog;
    Widget	ok;
    Widget	cancel;
    Widget	encoding_pulldown;
    Widget	encoding_menu;
    Widget	list[NONE];
    Widget	name;
    Widget	sample;
    حرف	**ناں;	// font name array of arrays
    صحيح		عدد;		// number of font names
    String	sel[NONE];	// selection category
    Boolean	وچ_pixels;	// toggle state - size in pixels
    حرف	*font_name;	// current font name
    XFontStruct	*old;		// font data structure for sample display
    XmFontList	old_list;	// font data structure for sample display
    Boolean	باہر_نکلو;		// used for program exit control
} Shared_Font_Sel_انکڑے_2;

/*
 * Checking access to the font name array for validity.
 */
    ساكن حرف *
fn(Shared_Font_Sel_انکڑے_2 *انکڑے, صحيح i)
{
    // Assertion checks:
    إذا (انکڑے->عدد < 0)
	abort();
    إذا (i >= انکڑے->عدد)
	i = انکڑے->عدد - 1;
    إذا (i < 0)
	i = 0;

    أرجع انکڑے->ناں[i];
}

/*
 * Get a specific substring from a font name.
 */
    ساكن فراغ
get_part(حرف *وچ, صحيح pos, حرف *باہرلا)
{
    صحيح	i;
    صحيح j;

    *باہرلا = '\0';

    لكل (i = 0; (pos > 0) && (وچ[i] != '\0'); ++i)
	إذا (وچ[i] == '-')
	    pos--;

    إذا (وچ[i] == '\0')
	أرجع;

    لكل (j = 0; (وچ[i] != '-') && (وچ[i] != '\0'); ++i, ++j)
	باہرلا[j] = وچ[i];
    باہرلا[j] = '\0';
}

/*
 * Given a font name this function returns the part used in the first
 * scroll list.
 */
    ساكن فراغ
name_part(حرف *font, حرف *buf)
{
    حرف    buf2[عارضی_BUF_ناپ];
    حرف    buf3[عارضی_BUF_ناپ];

    get_part(font, 2, buf2);
    get_part(font, 1, buf3);

    إذا (*buf3 != NUL)
	vim_snprintf(buf, عارضی_BUF_ناپ, "%s (%s)", buf2, buf3);
    وإلا
	vim_snprintf(buf, عارضی_BUF_ناپ, "%s", buf2);
}

/*
 * Given a font name this function returns the part used in the second scroll list.
 */
    ساكن فراغ
style_part(حرف *font, حرف *buf)
{
    حرف    buf2[عارضی_BUF_ناپ];
    حرف    buf3[عارضی_BUF_ناپ];

    get_part(font, 3, buf3);
    get_part(font, 5, buf2);

    إذا (!strcmp(buf2, "normal") && !strcmp(buf2, "Normal")
						   && !strcmp(buf2, "NORMAL"))
	vim_snprintf(buf, عارضی_BUF_ناپ, "%s %s", buf3, buf2);
    وإلا
	strcpy(buf, buf3);

    get_part(font, 6, buf2);

    إذا (buf2[0] != '\0')
	vim_snprintf(buf3, عارضی_BUF_ناپ, "%s %s", buf, buf2);
    وإلا
	strcpy(buf3, buf);

    get_part(font, 4, buf2);

    إذا (!strcmp(buf2, "o") || !strcmp(buf2, "O"))
	vim_snprintf(buf, عارضی_BUF_ناپ, "%s oblique", buf3);
    وإلا إذا (!strcmp(buf2, "i") || !strcmp(buf2, "I"))
	vim_snprintf(buf, عارضی_BUF_ناپ, "%s italic", buf3);

    إذا (!strcmp(buf, " "))
	strcpy(buf, "-");
}

/*
 * Given a font name this function returns the part used in the third
 * scroll list.
 */
    ساكن فراغ
ناپ_part(حرف *font, حرف *buf, صحيح وچ_Pixels)
{
    صحيح	    ناپ;
    عائم   عارضی;

    *buf = '\0';

    إذا (وچ_Pixels)
    {
	get_part(font, 7, buf);
	إذا (*buf != NUL)
	{
	    ناپ = atoi(buf);
	    sprintf(buf, "%3d", ناپ);
	}
    }
    وإلا
    {
	get_part(font, 8, buf);
	إذا (*buf != NUL)
	{
	    ناپ = atoi(buf);
	    عارضی = (عائم)ناپ / 10.0;
	    ناپ = عارضی;
	    إذا (buf[لڑی_لمبائی(buf) - 1] == '0')
		sprintf(buf, "%3d", ناپ);
	    وإلا
		sprintf(buf, "%4.1f", عارضی);
	}
    }
}

/*
 * Given a font name this function returns the part used in the choice menu.
 */
    ساكن فراغ
encoding_part(حرف *font, حرف *buf)
{
    حرف    buf1[عارضی_BUF_ناپ];
    حرف    buf2[عارضی_BUF_ناپ];

    *buf = '\0';

    get_part(font, 13, buf1);
    get_part(font, 14, buf2);

    إذا (*buf1 != NUL && *buf2 != NUL)
	vim_snprintf(buf, عارضی_BUF_ناپ, "%s-%s", buf1, buf2);
    إذا (!strcmp(buf, " "))
	strcpy(buf, "-");
}

/*
 * Inserts a string into correct sorted position in a list.
 */
    ساكن فراغ
جوڑو_منزل_list(حرف **buf, حرف *شے, صحيح *گنتی)
{
    صحيح	i;
    صحيح j;

    إذا (*گنتی == MAX_ENTRIES_وچ_LIST)
	أرجع;

    // avoid duplication
    لكل (i = 0; i < *گنتی; ++i)
    {
	إذا (!strcmp(buf[i], شے))
	    أرجع;
    }

    // find order place, but make sure that wild card comes first
    إذا (!strcmp(شے, wild))
	i = 0;
    وإلا
	لكل (i = 0; i < *گنتی; ++i)
	    إذا (strcmp(buf[i], شے) > 0 && strcmp(buf[i], wild))
		اكسر;

    // now insert the item
    لكل (j = *گنتی; j > i; --j)
	buf[j] = buf[j-1];
    buf[i] = XtNewString(شے);

    ++(*گنتی);
}

/*
 * True if the font matches some field.
 */
    ساكن Boolean
میل(Shared_Font_Sel_انکڑے_2 *انکڑے, تعداد ListSpecifier l, صحيح i)
{
    حرف buf[عارضی_BUF_ناپ];

    // An empty selection or a wild card matches anything.
    إذا (!انکڑے->sel[l] || !strcmp(انکڑے->sel[l], wild))
	أرجع True;

    // chunk out the desired part...
    بدل (l)
    {
	حالة ENCODING:
	    encoding_part(fn(انکڑے, i), buf);
	    اكسر;

	حالة NAME:
	    name_part(fn(انکڑے, i), buf);
	    اكسر;

	حالة STYLE:
	    style_part(fn(انکڑے, i), buf);
	    اكسر;

	حالة ناپ_3:
	    ناپ_part(fn(انکڑے, i), buf, انکڑے->وچ_pixels);
	    اكسر;
	افتراضي:
	    ;
    }

    // ...and chew it now

    أرجع !strcmp(buf, انکڑے->sel[l]);
}

    ساكن Boolean
proportional(حرف *font)
{
    حرف buf[عارضی_BUF_ناپ];

    get_part(font, 11, buf);

    أرجع !strcmp(buf, "p") || !strcmp(buf, "P");
}


ساكن فراغ encoding_callback(Widget w, Shared_Font_Sel_انکڑے_2 *انکڑے,
							     Xt_اشارہ_گر dummy);

/*
 * Parse through the fontlist data and set up the three scroll lists.  The fix
 * parameter can be used to exclude a list from any changes.  This is used for
 * updates after selections caused by the users actions.
 */
    ساكن فراغ
fill_lists(تعداد ListSpecifier fix, Shared_Font_Sel_انکڑے_2 *انکڑے)
{
    حرف	*list[NONE][MAX_ENTRIES_وچ_LIST];
    صحيح		گنتی[NONE];
    حرف	buf[عارضی_BUF_ناپ];
    XmString	اشیاء[MAX_ENTRIES_وچ_LIST];
    صحيح		i;
    صحيح		idx;

    لكل (idx = (صحيح)ENCODING; idx < (صحيح)NONE; ++idx)
	گنتی[idx] = 0;

    // First we insert the wild char into every single list.
    إذا (fix != ENCODING)
	جوڑو_منزل_list(list[ENCODING], wild, &گنتی[ENCODING]);
    إذا (fix != NAME)
	جوڑو_منزل_list(list[NAME], wild, &گنتی[NAME]);
    إذا (fix != STYLE)
	جوڑو_منزل_list(list[STYLE], wild, &گنتی[STYLE]);
    إذا (fix != ناپ_3)
	جوڑو_منزل_list(list[ناپ_3], wild, &گنتی[ناپ_3]);

    لكل (i = 0; i < انکڑے->عدد && i < MAX_ENTRIES_وچ_LIST; i++)
    {
	إذا (proportional(fn(انکڑے, i)))
	    استمر;

	إذا (fix != ENCODING
		&& میل(انکڑے, NAME, i)
		&& میل(انکڑے, STYLE, i)
		&& میل(انکڑے, ناپ_3, i))
	{
	    encoding_part(fn(انکڑے, i), buf);
	    جوڑو_منزل_list(list[ENCODING], buf, &گنتی[ENCODING]);
	}

	إذا (fix != NAME
		&& میل(انکڑے, ENCODING, i)
		&& میل(انکڑے, STYLE, i)
		&& میل(انکڑے, ناپ_3, i))
	{
	    name_part(fn(انکڑے, i), buf);
	    جوڑو_منزل_list(list[NAME], buf, &گنتی[NAME]);
	}

	إذا (fix != STYLE
		&& میل(انکڑے, ENCODING, i)
		&& میل(انکڑے, NAME, i)
		&& میل(انکڑے, ناپ_3, i))
	{
	    style_part(fn(انکڑے, i), buf);
	    جوڑو_منزل_list(list[STYLE], buf, &گنتی[STYLE]);
	}

	إذا (fix != ناپ_3
		&& میل(انکڑے, ENCODING, i)
		&& میل(انکڑے, NAME, i)
		&& میل(انکڑے, STYLE, i))
	{
	    ناپ_part(fn(انکڑے, i), buf, انکڑے->وچ_pixels);
	    جوڑو_منزل_list(list[ناپ_3], buf, &گنتی[ناپ_3]);
	}
    }

    /*
     * And now do the preselection in all lists where there was one:
     */

    إذا (fix != ENCODING)
    {
	Cardinal n_اشیاء;
	WidgetList children;
	Widget selected_button = 0;

	// Get and update the current button list.
	Xt_Va_Get_اقدار(انکڑے->encoding_pulldown,
		XmNchildren, &children,
		XmNnumChildren, &n_اشیاء,
		عدم);

	لكل (i = 0; i < گنتی[ENCODING]; ++i)
	{
	    Widget button;

	    اشیاء[i] = XmStringCreateLocalized(list[ENCODING][i]);

	    إذا (i < (صحيح)n_اشیاء)
	    {
		// recycle old button
		Xt_Va_سیٹ_اقدار(children[i],
			XmNlabelString, اشیاء[i],
			Xm_Nuser_انکڑے, i,
			عدم);
		button = children[i];
	    }
	    وإلا
	    {
		// create a new button
		button = XtVaCreateManagedWidget("button",
			xm_ڈالو_Button_Gadget_Class,
			انکڑے->encoding_pulldown,
			XmNlabelString, اشیاء[i],
			Xm_Nuser_انکڑے, i,
			عدم);
		Xt_جوڑو_Callback(button, XmNactivateCallback,
			(XtCallbackProc) encoding_callback, (Xt_اشارہ_گر) انکڑے);
		Xt_Manage_بچہ(button);
	    }

	    إذا (انکڑے->sel[ENCODING])
	    {
		إذا (!strcmp(انکڑے->sel[ENCODING], list[ENCODING][i]))
		    selected_button = button;
	    }
	    XtFree(list[ENCODING][i]);
	}

	// Destroy all the outstanding menu items.
	لكل (i = گنتی[ENCODING]; i < (صحيح)n_اشیاء; ++i)
	{
	    Xt_Unmanage_بچہ(children[i]);
	    XtDestroyWidget(children[i]);
	}

	// Preserve the current selection visually.
	إذا (selected_button)
	{
	    Xt_Va_سیٹ_اقدار(انکڑے->encoding_menu,
		    XmNmenuHistory, selected_button,
		    عدم);
	}

	لكل (i = 0; i < گنتی[ENCODING]; ++i)
	    XmStringFree(اشیاء[i]);
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
		w = انکڑے->list[NAME];
		اكسر;
	    حالة STYLE:
		w = انکڑے->list[STYLE];
		اكسر;
	    حالة ناپ_3:
		w = انکڑے->list[ناپ_3];
		اكسر;
	    افتراضي:
		w = (Widget)0;	// for lint
	}

	لكل (i = 0; i < گنتی[idx]; ++i)
	{
	    اشیاء[i] = XmStringCreateLocalized(list[idx][i]);
	    XtFree(list[idx][i]);
	}
	Xm_List_Delete_All_اشیاء(w);
	Xm_List_جوڑو_اشیاء(w, اشیاء, گنتی[idx], 1);
	إذا (انکڑے->sel[idx])
	{
	    XmStringFree(اشیاء[0]);
	    اشیاء[0] = XmStringCreateLocalized(انکڑے->sel[idx]);
	    Xm_List_Select_شے(w, اشیاء[0], False);
	    Xm_List_سیٹ_Bottom_شے(w, اشیاء[0]);
	}
	لكل (i = 0; i < گنتی[idx]; ++i)
	    XmStringFree(اشیاء[i]);
    }
}

    ساكن فراغ
stoggle_callback(Widget w بن_ورتیا,
	Shared_Font_Sel_انکڑے_2 *انکڑے,
	XmToggleButtonCallbackStruct *call_انکڑے)
{
    صحيح		i, do_sel;
    حرف	new_ناپ_2[عارضی_BUF_ناپ];
    XmString	str;

    إذا (call_انکڑے->reason != (صحيح)Xm_CR_قدر_بدلیا)
	أرجع;

    do_sel = (انکڑے->sel[ناپ_3] != عدم) && strcmp(انکڑے->sel[ناپ_3], wild);

    لكل (i = 0; do_sel && (i < انکڑے->عدد); i++)
	إذا (میل(انکڑے, ENCODING, i)
		&& میل(انکڑے, NAME, i)
		&& میل(انکڑے, STYLE, i)
		&& میل(انکڑے, ناپ_3, i))
	{
	    ناپ_part(fn(انکڑے, i), new_ناپ_2, !انکڑے->وچ_pixels);
	    اكسر;
	}

    انکڑے->وچ_pixels = !انکڑے->وچ_pixels;

    إذا (انکڑے->sel[ناپ_3])
	XtFree(انکڑے->sel[ناپ_3]);
    انکڑے->sel[ناپ_3] = عدم;
    fill_lists(NONE, انکڑے);

    إذا (do_sel)
    {
	str = XmStringCreateLocalized(new_ناپ_2);
	Xm_List_Select_شے(انکڑے->list[ناپ_3], str, True);
	Xm_List_سیٹ_Bottom_شے(انکڑے->list[ناپ_3], str);
	XmStringFree(str);
    }
}

/*
 * Show the currently selected font in the sample text label.
 */
    ساكن فراغ
display_sample(Shared_Font_Sel_انکڑے_2 *انکڑے)
{
    Arg		    دلیلاں[2];
    صحيح		    n;
    XFontStruct	*   font;
    XmFontList	    font_list;
    Display *	    display;
    XmString	    str;

    display = XtDisplay(انکڑے->dialog);
    font = XLoadQueryFont(display, انکڑے->font_name);
    font_list = gui_motif_create_fontlist(font);

    n = 0;
    str = XmStringCreateLocalized("AaBbZzYy 0123456789");
    Xt_سیٹ_Arg(دلیلاں[n], XmNlabelString, str); n++;
    Xt_سیٹ_Arg(دلیلاں[n], XmNfontList, font_list); n++;

    Xt_سیٹ_اقدار(انکڑے->sample, دلیلاں, n);
    XmStringFree(str);

    إذا (انکڑے->old)
    {
	XFreeFont(display, انکڑے->old);
	XmFontListFree(انکڑے->old_list);
    }
    انکڑے->old = font;
    انکڑے->old_list = font_list;
}


    ساكن Boolean
do_choice(Widget w,
	Shared_Font_Sel_انکڑے_2 *انکڑے,
	XmListCallbackStruct *call_انکڑے,
	تعداد ListSpecifier which)
{
    حرف *sel;

    XmStringGetLtoR(call_انکڑے->شے, XmSTRING_DEFAULT_CHARSET, &sel);

    إذا (!انکڑے->sel[which])
	انکڑے->sel[which] = XtNewString(sel);
    وإلا
    {
	إذا (!strcmp(انکڑے->sel[which], sel))
	{
	    // unselecting current selection
	    XtFree(انکڑے->sel[which]);
	    انکڑے->sel[which] = عدم;
	    إذا (w)
		Xm_List_Deselect_شے(w, call_انکڑے->شے);
	}
	وإلا
	{
	    XtFree(انکڑے->sel[which]);
	    انکڑے->sel[which] = XtNewString(sel);
	}
    }
    XtFree(sel);

    fill_lists(which, انکڑے);

    // If there is a font selection, we display it.
    إذا (انکڑے->sel[ENCODING]
	    && انکڑے->sel[NAME]
	    && انکڑے->sel[STYLE]
	    && انکڑے->sel[ناپ_3]
	    && strcmp(انکڑے->sel[ENCODING], wild)
	    && strcmp(انکڑے->sel[NAME], wild)
	    && strcmp(انکڑے->sel[STYLE], wild)
	    && strcmp(انکڑے->sel[ناپ_3], wild))
    {
	صحيح i;

	إذا (انکڑے->font_name)
	    XtFree(انکڑے->font_name);
	انکڑے->font_name = عدم;

	لكل (i = 0; i < انکڑے->عدد; i++)
	{
	    إذا (میل(انکڑے, ENCODING, i)
		    && میل(انکڑے, NAME, i)
		    && میل(انکڑے, STYLE, i)
		    && میل(انکڑے, ناپ_3, i))
	    {
		انکڑے->font_name = XtNewString(fn(انکڑے, i));
		اكسر;
	    }
	}

	إذا (انکڑے->font_name)
	{
	    Xm_متن_سیٹ_String(انکڑے->name, انکڑے->font_name);
	    display_sample(انکڑے);
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
	Arg	    دلیلاں[4];
	حرف	    *nomatch_پیغام = _("no specific match");

	n = 0;
	str = XmStringCreateLocalized(nomatch_پیغام);
	Xt_سیٹ_Arg(دلیلاں[n], XmNlabelString, str); ++n;
	Xt_سیٹ_اقدار(انکڑے->sample, دلیلاں, n);
	apply_fontlist(انکڑے->sample);
	Xm_متن_سیٹ_String(انکڑے->name, nomatch_پیغام);
	XmStringFree(str);

	أرجع False;
    }
}

    ساكن فراغ
encoding_callback(Widget w,
	Shared_Font_Sel_انکڑے_2 *انکڑے,
	Xt_اشارہ_گر dummy بن_ورتیا)
{
    XmString str;
    XmListCallbackStruct fake_انکڑے;

    Xt_Va_Get_اقدار(w, XmNlabelString, &str, عدم);

    إذا (!str)
	أرجع;

    fake_انکڑے.شے = str;

    do_choice(0, انکڑے, &fake_انکڑے, ENCODING);
}

    ساكن فراغ
name_callback(Widget w,
	Shared_Font_Sel_انکڑے_2 *انکڑے,
	XmListCallbackStruct *call_انکڑے)
{
    do_choice(w, انکڑے, call_انکڑے, NAME);
}

    ساكن فراغ
style_callback(Widget w,
	Shared_Font_Sel_انکڑے_2 *انکڑے,
	XmListCallbackStruct *call_انکڑے)
{
    do_choice(w, انکڑے, call_انکڑے, STYLE);
}

    ساكن فراغ
ناپ_callback(Widget w,
	Shared_Font_Sel_انکڑے_2 *انکڑے,
	XmListCallbackStruct *call_انکڑے)
{
    do_choice(w, انکڑے, call_انکڑے, ناپ_3);
}

    ساكن فراغ
cancel_callback(Widget w بن_ورتیا,
	Shared_Font_Sel_انکڑے_2 *انکڑے,
	XmListCallbackStruct *call_انکڑے بن_ورتیا)
{
    إذا (انکڑے->sel[ENCODING])
    {
	XtFree(انکڑے->sel[ENCODING]);
	انکڑے->sel[ENCODING] = عدم;
    }
    إذا (انکڑے->sel[NAME])
    {
	XtFree(انکڑے->sel[NAME]);
	انکڑے->sel[NAME] = عدم;
    }
    إذا (انکڑے->sel[STYLE])
    {
	XtFree(انکڑے->sel[STYLE]);
	انکڑے->sel[STYLE] = عدم;
    }
    إذا (انکڑے->sel[ناپ_3])
    {
	XtFree(انکڑے->sel[ناپ_3]);
	انکڑے->sel[ناپ_3] = عدم;
    }

    إذا (انکڑے->font_name)
	XtFree(انکڑے->font_name);
    انکڑے->font_name = عدم;

    انکڑے->عدد = 0;
    XFree_Font_ناں(انکڑے->ناں);
    انکڑے->ناں = عدم;
    انکڑے->باہر_نکلو = True;
}

    ساكن فراغ
ok_callback(Widget w بن_ورتیا,
	Shared_Font_Sel_انکڑے_2 *انکڑے,
	Xm_ڈالو_Button_Callback_Struct *call_انکڑے بن_ورتیا)
{
    حرف    *نمونہ;
    حرف    **name;
    صحيح	    i;

    نمونہ = Xm_متن_Get_String(انکڑے->name);
    name    = XListFonts(XtDisplay(انکڑے->dialog), نمونہ, 1, &i);
    XtFree(نمونہ);

    إذا (i != 1)
    {
	do_dialog(VIM_ERROR,
		(char_u *)_("Error"),
		(char_u *)_("Invalid font specification"),
		(char_u *)_("&Dismiss"), 1, عدم, FALSE);
	XFree_Font_ناں(name);
    }
    وإلا
    {
	إذا (انکڑے->font_name)
	    XtFree(انکڑے->font_name);
	انکڑے->font_name = XtNewString(name[0]);

	إذا (انکڑے->sel[ENCODING])
	{
	    XtFree(انکڑے->sel[ENCODING]);
	    انکڑے->sel[ENCODING] = عدم;
	}
	إذا (انکڑے->sel[NAME])
	{
	    XtFree(انکڑے->sel[NAME]);
	    انکڑے->sel[NAME] = عدم;
	}
	إذا (انکڑے->sel[STYLE])
	{
	    XtFree(انکڑے->sel[STYLE]);
	    انکڑے->sel[STYLE] = عدم;
	}
	إذا (انکڑے->sel[ناپ_3])
	{
	    XtFree(انکڑے->sel[ناپ_3]);
	    انکڑے->sel[ناپ_3] = عدم;
	}

	XFree_Font_ناں(name);

	انکڑے->عدد = 0;
	XFree_Font_ناں(انکڑے->ناں);
	انکڑے->ناں = عدم;
	انکڑے->باہر_نکلو = True;
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
    ساكن Shared_Font_Sel_انکڑے_2	انکڑے_3;

    Widget		ماپا;
    Widget		form;
    Widget		separator;
    Widget		sub_form;
    Widget		ناپ_toggle;
    Widget		name;
    Widget		disp_چوکھٹا;
    Widget		چوکھٹا;
    Arg			دلیلاں[64];
    صحيح			n;
    XmString		str;
    حرف		big_font[MAX_FONT_NAME_LEN];
    Shared_Font_Sel_انکڑے_2	*انکڑے;

    انکڑے = &انکڑے_3;

    ماپا = vimShell;
    انکڑے->ناں = XListFonts(XtDisplay(ماپا), "-*-*-*-*-*-*-*-*-*-*-*-*-*-*",
						       MAX_FONTS, &انکڑے->عدد);

    /*
     * Find the name of the biggest font less than the given limit
     * MAX_DISPLAY_SIZE used to set up the initial height of the display
     * widget.
     */

    {
	صحيح	i;
	صحيح	max;
	صحيح	idx = 0;
	صحيح	ناپ;
	حرف	buf[128];

	لكل (i = 0, max = 0; i < انکڑے->عدد; i++)
	{
	    get_part(fn(انکڑے, i), 7, buf);
	    ناپ = atoi(buf);
	    إذا ((ناپ > max) && (ناپ < MAX_DISPLAY_ناپ))
	    {
		idx = i;
		max = ناپ;
	    }
	}
	strcpy(big_font, fn(انکڑے, idx));
    }
    انکڑے->old = XLoadQueryFont(XtDisplay(ماپا), big_font);
    انکڑے->old_list = gui_motif_create_fontlist(انکڑے->old);

    // Set the title of the Dialog window.
    انکڑے->dialog = XmCreateDialogShell(ماپا, "fontSelector", عدم, 0);
    str = XmStringCreateLocalized(_("Vim - Font Selector"));

    // Create form popup dialog widget.
    form = XtVaCreateWidget("form",
	    xmFormWidgetClass, انکڑے->dialog,
	    XmNdialogTitle, str,
	    XmNautoUnmanage, False,
	    XmNdialogStyle, Xm_DIALOG_بھریا_APPLICATION_MODAL,
	    عدم);
    XmStringFree(str);

    sub_form = XtVaCreateManagedWidget("subForm",
	    xmFormWidgetClass, form,
	    XmNbottomAttachment, XmATTACH_FORM,
	    Xm_Nbottom_وتھ, 4,
	    XmNrightAttachment, XmATTACH_FORM,
	    Xm_Nright_وتھ, 4,
	    XmNtopAttachment, XmATTACH_FORM,
	    Xm_Ntop_وتھ, 4,
	    XmNorientation, XmVERTICAL,
	    عدم);

    انکڑے->ok = XtVaCreateManagedWidget(_("OK"),
	    xm_ڈالو_Button_Gadget_Class, sub_form,
	    XmNleftAttachment, XmATTACH_FORM,
	    XmNrightAttachment, XmATTACH_FORM,
	    XmNtopAttachment, XmATTACH_FORM,
	    Xm_Ntop_وتھ, 4,
	    عدم);
    apply_fontlist(انکڑے->ok);

    انکڑے->cancel = XtVaCreateManagedWidget(_("Cancel"),
	    xm_ڈالو_Button_Gadget_Class, sub_form,
	    XmNrightAttachment, XmATTACH_FORM,
	    XmNleftAttachment, XmATTACH_FORM,
	    XmNtopAttachment, XmATTACH_WIDGET,
	    XmNtopWidget, انکڑے->ok,
	    Xm_Ntop_وتھ, 4,
	    XmNshowAsDefault, True,
	    عدم);
    apply_fontlist(انکڑے->cancel);

    // Create the separator for beauty.
    n = 0;
    Xt_سیٹ_Arg(دلیلاں[n], XmNorientation, XmVERTICAL); n++;
    Xt_سیٹ_Arg(دلیلاں[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    Xt_سیٹ_Arg(دلیلاں[n], XmNtopAttachment, XmATTACH_FORM); n++;
    Xt_سیٹ_Arg(دلیلاں[n], XmNrightAttachment, XmATTACH_WIDGET); n++;
    Xt_سیٹ_Arg(دلیلاں[n], XmNrightWidget, sub_form); n++;
    Xt_سیٹ_Arg(دلیلاں[n], Xm_Nright_وتھ, 4); n++;
    separator = XmCreateSeparatorGadget(form, "separator", دلیلاں, n);
    Xt_Manage_بچہ(separator);

    // Create font name text widget and the corresponding label.
    انکڑے->name = XtVaCreateManagedWidget("fontName",
	    xm_متن_Widget_Class, form,
	    XmNbottomAttachment, XmATTACH_FORM,
	    Xm_Nbottom_وتھ, 4,
	    XmNleftAttachment, XmATTACH_FORM,
	    Xm_Nleft_وتھ, 4,
	    XmNrightAttachment, XmATTACH_WIDGET,
	    XmNrightWidget, separator,
	    Xm_Nright_وتھ, 4,
	    XmNeditable, False,
	    Xm_Nedit_منوال, Xm_SINGLE_سطر_EDIT,
	    Xm_Nmax_لمائی, MAX_FONT_NAME_LEN,
	    XmNcolumns, 60,
	    عدم);

    str = XmStringCreateLocalized(_("Name:"));
    name = XtVaCreateManagedWidget("fontNameLabel",
	    xmLabelGadgetClass, form,
	    XmNlabelString, str,
	    Xm_Nuser_انکڑے, انکڑے->name,
	    XmNleftAttachment, XmATTACH_OPPOSITE_WIDGET,
	    XmNleftWidget, انکڑے->name,
	    XmNbottomAttachment, XmATTACH_WIDGET,
	    XmNbottomWidget, انکڑے->name,
	    Xm_Ntop_وتھ, 1,
	    عدم);
    XmStringFree(str);
    apply_fontlist(name);

    // create sample display label widget
    disp_چوکھٹا = XtVaCreateManagedWidget("sampleFrame",
	    xm_چوکھٹا_Widget_Class, form,
	    XmNshadowType, Xm_SHADOW_ETCHED_وچ,
	    XmNleftAttachment, XmATTACH_FORM,
	    Xm_Nleft_وتھ, 4,
	    XmNbottomAttachment, XmATTACH_WIDGET,
	    XmNbottomWidget, name,
	    XmNrightAttachment, XmATTACH_WIDGET,
	    XmNrightWidget, separator,
	    Xm_Nright_وتھ, 4,
	    XmNalignment, XmALIGNMENT_BEGINNING,
	    عدم);

    انکڑے->sample = XtVaCreateManagedWidget("sampleLabel",
	    xmLabelWidgetClass, disp_چوکھٹا,
	    XmNleftAttachment, XmATTACH_FORM,
	    XmNtopAttachment, XmATTACH_FORM,
	    XmNbottomAttachment, XmATTACH_FORM,
	    XmNrightAttachment, XmATTACH_FORM,
	    XmNalignment, XmALIGNMENT_BEGINNING,
	    Xm_Nrecompute_ناپ, False,
	    XmNfontList, انکڑے->old_list,
	    عدم);

    // create toggle button
    str = XmStringCreateLocalized(_("Show size in Points"));
    ناپ_toggle = XtVaCreateManagedWidget("sizeToggle",
	    xmToggleButtonGadgetClass, form,
	    XmNlabelString, str,
	    XmNleftAttachment, XmATTACH_FORM,
	    Xm_Nleft_وتھ, 4,
	    XmNbottomAttachment, XmATTACH_WIDGET,
	    XmNbottomWidget, disp_چوکھٹا,
	    Xm_Nbottom_وتھ, 4,
	    عدم);
    XmStringFree(str);
    apply_fontlist(ناپ_toggle);
    Xt_Manage_بچہ(ناپ_toggle);

    // Encoding pulldown menu.

    انکڑے->encoding_pulldown = XmCreatePulldownMenu(form,
						 "encodingPulldown", عدم, 0);
    str = XmStringCreateLocalized(_("Encoding:"));
    n = 0;
    Xt_سیٹ_Arg(دلیلاں[n], XmNsubMenuId, انکڑے->encoding_pulldown); ++n;
    Xt_سیٹ_Arg(دلیلاں[n], XmNlabelString, str); ++n;
    Xt_سیٹ_Arg(دلیلاں[n], XmNleftAttachment, XmATTACH_FORM); ++n;
    Xt_سیٹ_Arg(دلیلاں[n], Xm_Nleft_وتھ, 4); ++n;
    Xt_سیٹ_Arg(دلیلاں[n], XmNbottomAttachment, XmATTACH_WIDGET); ++n;
    Xt_سیٹ_Arg(دلیلاں[n], XmNbottomWidget, ناپ_toggle); ++n;
    Xt_سیٹ_Arg(دلیلاں[n], Xm_Nbottom_وتھ, 4); ++n;
    Xt_سیٹ_Arg(دلیلاں[n], XmNrightAttachment, XmATTACH_WIDGET); ++n;
    Xt_سیٹ_Arg(دلیلاں[n], XmNrightWidget, separator); ++n;
    Xt_سیٹ_Arg(دلیلاں[n], Xm_Nright_وتھ, 4); ++n;
    انکڑے->encoding_menu = XmCreateOptionMenu(form, "encodingMenu", دلیلاں, n);
    XmStringFree(str);
    Xm_جوڑو_Tab_گروہ(انکڑے->encoding_menu);

    /*
     * Create scroll list widgets in a separate subform used to manage the
     * different sizes of the lists.
     */

    sub_form = XtVaCreateManagedWidget("subForm",
	    xmFormWidgetClass, form,
	    XmNbottomAttachment, XmATTACH_WIDGET,
	    XmNbottomWidget, انکڑے->encoding_menu,
	    Xm_Nbottom_وتھ, 4,
	    XmNleftAttachment, XmATTACH_FORM,
	    Xm_Nleft_وتھ, 4,
	    XmNrightAttachment, XmATTACH_WIDGET,
	    XmNrightWidget, separator,
	    Xm_Nright_وتھ, 4,
	    XmNtopAttachment, XmATTACH_FORM,
	    Xm_Ntop_وتھ, 2,
	    XmNorientation, XmVERTICAL,
	    عدم);

    // font list
    چوکھٹا = XtVaCreateManagedWidget("frame", xm_چوکھٹا_Widget_Class, sub_form,
	    XmNshadowThickness, 0,
	    XmNtopAttachment, XmATTACH_FORM,
	    XmNbottomAttachment, XmATTACH_FORM,
	    XmNleftAttachment, XmATTACH_FORM,
	    XmNrightAttachment, XmATTACH_POSITION,
	    XmNrightPosition, 50,
	    عدم);

    str = XmStringCreateLocalized(_("Font:"));
    name = XtVaCreateManagedWidget("nameListLabel", xmLabelGadgetClass, چوکھٹا,
	    XmNchildType, Xm_چوکھٹا_TITLE_بچہ,
	    XmNchildVerticalAlignment, XmALIGNMENT_CENTER,
	    XmNchildHorizontalAlignment, XmALIGNMENT_BEGINNING,
	    XmNlabelString, str,
	    عدم);
    XmStringFree(str);
    apply_fontlist(name);

    n = 0;
    Xt_سیٹ_Arg(دلیلاں[n], Xm_Nvisible_شے_گنتی, 8); ++n;
    Xt_سیٹ_Arg(دلیلاں[n], XmNresizable, True); ++n;
    Xt_سیٹ_Arg(دلیلاں[n], Xm_Nlist_ناپ_Policy, XmCONSTANT); ++n;
    Xt_سیٹ_Arg(دلیلاں[n], XmNvisualPolicy, XmVARIABLE); ++n;
#جے_تعریف_شدہ LESSTIF_نسخہ
    Xt_سیٹ_Arg(دلیلاں[n], XmNscrollBarDisplayPolicy, XmSTATIC); ++n;
#جے_انت
    انکڑے->list[NAME] = XmCreateScrolledList(چوکھٹا, "fontList", دلیلاں, n);
    Xt_Va_سیٹ_اقدار(name, Xm_Nuser_انکڑے, انکڑے->list[NAME], عدم);

    // style list
    چوکھٹا = XtVaCreateManagedWidget("frame", xm_چوکھٹا_Widget_Class, sub_form,
	    XmNshadowThickness, 0,
	    XmNtopAttachment, XmATTACH_FORM,
	    XmNbottomAttachment, XmATTACH_FORM,
	    XmNleftAttachment, XmATTACH_POSITION,
	    XmNleftPosition, 50,
	    Xm_Nleft_وتھ, 4,
	    XmNrightAttachment, XmATTACH_POSITION,
	    XmNrightPosition, 80,
	    عدم);

    str = XmStringCreateLocalized(_("Style:"));
    name = XtVaCreateManagedWidget("styleListLabel", xmLabelWidgetClass, چوکھٹا,
	    XmNchildType, Xm_چوکھٹا_TITLE_بچہ,
	    XmNchildVerticalAlignment, XmALIGNMENT_CENTER,
	    XmNchildHorizontalAlignment, XmALIGNMENT_BEGINNING,
	    XmNlabelString, str,
	    عدم);
    XmStringFree(str);
    apply_fontlist(name);

    n = 0;
    Xt_سیٹ_Arg(دلیلاں[n], Xm_Nvisible_شے_گنتی, 8); ++n;
    Xt_سیٹ_Arg(دلیلاں[n], XmNresizable, True); ++n;
    Xt_سیٹ_Arg(دلیلاں[n], Xm_Nlist_ناپ_Policy, XmCONSTANT); ++n;
    Xt_سیٹ_Arg(دلیلاں[n], XmNvisualPolicy, XmVARIABLE); ++n;
#جے_تعریف_شدہ LESSTIF_نسخہ
    Xt_سیٹ_Arg(دلیلاں[n], XmNscrollBarDisplayPolicy, XmSTATIC); ++n;
#جے_انت
    انکڑے->list[STYLE] = XmCreateScrolledList(چوکھٹا, "styleList", دلیلاں, n);
    Xt_Va_سیٹ_اقدار(name, Xm_Nuser_انکڑے, انکڑے->list[STYLE], عدم);

    // size list
    چوکھٹا = XtVaCreateManagedWidget("frame", xm_چوکھٹا_Widget_Class, sub_form,
	    XmNshadowThickness, 0,
	    XmNtopAttachment, XmATTACH_FORM,
	    XmNbottomAttachment, XmATTACH_FORM,
	    XmNleftAttachment, XmATTACH_POSITION,
	    XmNleftPosition, 80,
	    Xm_Nleft_وتھ, 4,
	    XmNrightAttachment, XmATTACH_FORM,
	    عدم);

    str = XmStringCreateLocalized(_("Size:"));
    name = XtVaCreateManagedWidget("sizeListLabel", xmLabelGadgetClass, چوکھٹا,
	    XmNchildType, Xm_چوکھٹا_TITLE_بچہ,
	    XmNchildVerticalAlignment, XmALIGNMENT_CENTER,
	    XmNchildHorizontalAlignment, XmALIGNMENT_BEGINNING,
	    XmNlabelString, str,
	    عدم);
    XmStringFree(str);
    apply_fontlist(name);

    n = 0;
    Xt_سیٹ_Arg(دلیلاں[n], Xm_Nvisible_شے_گنتی, 8); ++n;
    Xt_سیٹ_Arg(دلیلاں[n], XmNresizable, True); ++n;
    Xt_سیٹ_Arg(دلیلاں[n], Xm_Nlist_ناپ_Policy, XmCONSTANT); ++n;
    Xt_سیٹ_Arg(دلیلاں[n], XmNvisualPolicy, XmVARIABLE); ++n;
#جے_تعریف_شدہ LESSTIF_نسخہ
    Xt_سیٹ_Arg(دلیلاں[n], XmNscrollBarDisplayPolicy, XmSTATIC); ++n;
#جے_انت
    انکڑے->list[ناپ_3] = XmCreateScrolledList(چوکھٹا, "sizeList", دلیلاں, n);
    Xt_Va_سیٹ_اقدار(name, Xm_Nuser_انکڑے, انکڑے->list[ناپ_3], عدم);

    // update form widgets cancel button
    Xt_Va_سیٹ_اقدار(form, XmNcancelButton, انکڑے->cancel, عدم);

    Xt_جوڑو_Callback(ناپ_toggle, Xm_Nvalue_بدلیا_Callback,
	    (XtCallbackProc)stoggle_callback, (Xt_اشارہ_گر)انکڑے);
    Xt_جوڑو_Callback(انکڑے->list[NAME], XmNbrowseSelectionCallback,
	    (XtCallbackProc)name_callback, (Xt_اشارہ_گر)انکڑے);
    Xt_جوڑو_Callback(انکڑے->list[STYLE], XmNbrowseSelectionCallback,
	    (XtCallbackProc)style_callback, (Xt_اشارہ_گر)انکڑے);
    Xt_جوڑو_Callback(انکڑے->list[ناپ_3], XmNbrowseSelectionCallback,
	    (XtCallbackProc)ناپ_callback, (Xt_اشارہ_گر)انکڑے);
    Xt_جوڑو_Callback(انکڑے->ok, XmNactivateCallback,
	    (XtCallbackProc)ok_callback, (Xt_اشارہ_گر)انکڑے);
    Xt_جوڑو_Callback(انکڑے->cancel, XmNactivateCallback,
	    (XtCallbackProc)cancel_callback, (Xt_اشارہ_گر)انکڑے);

    XmProcessTraversal(انکڑے->list[NAME], XmTRAVERSE_CURRENT);

    // setup tabgroups

    Xm_جوڑو_Tab_گروہ(انکڑے->list[NAME]);
    Xm_جوڑو_Tab_گروہ(انکڑے->list[STYLE]);
    Xm_جوڑو_Tab_گروہ(انکڑے->list[ناپ_3]);
    Xm_جوڑو_Tab_گروہ(ناپ_toggle);
    Xm_جوڑو_Tab_گروہ(انکڑے->name);
    Xm_جوڑو_Tab_گروہ(انکڑے->ok);
    Xm_جوڑو_Tab_گروہ(انکڑے->cancel);

    جوڑو_cancel_action(انکڑے->dialog, (XtCallbackProc)cancel_callback, انکڑے);

    // Preset selection data.

    انکڑے->باہر_نکلو = False;
    انکڑے->وچ_pixels= True;
    انکڑے->sel[ENCODING] = عدم;
    انکڑے->sel[NAME] = عدم;
    انکڑے->sel[STYLE] = عدم;
    انکڑے->sel[ناپ_3] = عدم;
    انکڑے->font_name = عدم;

    // set up current font parameters
    إذا (current && current[0] != '\0')
    {
	صحيح	    i;
	حرف	    **ناں;

	ناں = XListFonts(XtDisplay(form), (حرف *) current, 1, &i);

	إذا (i != 0)
	{
	    حرف namebuf[عارضی_BUF_ناپ];
	    حرف stylebuf[عارضی_BUF_ناپ];
	    حرف sizebuf[عارضی_BUF_ناپ];
	    حرف encodingbuf[عارضی_BUF_ناپ];
	    حرف *لبھیا;

	    لبھیا = ناں[0];

	    name_part(لبھیا, namebuf);
	    style_part(لبھیا, stylebuf);
	    ناپ_part(لبھیا, sizebuf, انکڑے->وچ_pixels);
	    encoding_part(لبھیا, encodingbuf);

	    إذا (*namebuf != NUL
		    && *stylebuf != NUL
		    && *sizebuf != NUL
		    && *encodingbuf != NUL)
	    {
		انکڑے->sel[NAME] = XtNewString(namebuf);
		انکڑے->sel[STYLE] = XtNewString(stylebuf);
		انکڑے->sel[ناپ_3] = XtNewString(sizebuf);
		انکڑے->sel[ENCODING] = XtNewString(encodingbuf);
		انکڑے->font_name = XtNewString(ناں[0]);
		display_sample(انکڑے);
		Xm_متن_سیٹ_String(انکڑے->name, انکڑے->font_name);
	    }
	    وإلا
	    {
		// We can't preset a symbolic name, which isn't a full font
		// description. Therefore we just behave the same way as if the
		// user didn't have selected anything thus far.
		//
		// Unfortunately there is no known way to expand an abbreviated
		// font name.
		انکڑے->font_name = عدم;
	    }
	}
	XFree_Font_ناں(ناں);
    }

    fill_lists(NONE, انکڑے);

    // Unfortunately LessTif doesn't align the list widget's properly.  I don't
    // have currently any idea how to fix this problem.
    Xt_Manage_بچہ(انکڑے->list[NAME]);
    Xt_Manage_بچہ(انکڑے->list[STYLE]);
    Xt_Manage_بچہ(انکڑے->list[ناپ_3]);
    Xt_Manage_بچہ(انکڑے->encoding_menu);
    manage_centered(form);

    // modal event loop
    طالما (!انکڑے->باہر_نکلو)
	XtAppProcessEvent(Xt_Widget_منزل_Application_Context(انکڑے->dialog),
							(Xt_داخلہ_نقاب)XtIMAll);

    إذا (انکڑے->old)
    {
	XFreeFont(XtDisplay(انکڑے->dialog),  انکڑے->old);
	XmFontListFree(انکڑے->old_list);
    }
    XtDestroyWidget(انکڑے->dialog);

    gui_motif_synch_fonts();

    أرجع (char_u *) انکڑے->font_name;
}
