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

#شامل "vim.h"

#شامل <Xm/Form.h>
#شامل <Xm/PushBG.h>
#شامل <Xm/Text.h>
#شامل <Xm/TextF.h>
#شامل <Xm/Label.h>
#شامل <Xm/Frame.h>
#شامل <Xm/LabelG.h>
#شامل <Xm/ToggleBG.h>
#شامل <Xm/SeparatoG.h>
#شامل <Xm/DialogS.h>
#شامل <Xm/List.h>
#شامل <Xm/RowColumn.h>
#شامل <Xm/AtomMgr.h>
#شامل <Xm/Protocols.h>

#شامل <X11/keysym.h>
#شامل <X11/Xatom.h>
#شامل <X11/StringDefs.h>
#شامل <X11/Intrinsic.h>

خارجی Widget vimShell;

#اگر_تعریف_شدہ FEAT_MENU
# define apply_fontlist(w) gui_motif_menu_fontlist(w)
#ورنہ
# define apply_fontlist(w)
#اگر_ختم

/////////////////////////////////////////////////////////////////////////////
// Font selection dialogue implementation.

ساکن حرف wild[3] = "*";

/*
 * FIXME: This is a generic function, which should be used throughout the whole
 * application.
 *
 * Add close_callback, which will be called when the user selects close from
 * the window menu.  The close menu item usually activates f.kill which sends a
 * WM_DELETE_WINDOW protocol request for the window.
 */

    ساکن خالی
جوڑو_cancel_action(Widget shell, XtCallbackProc close_callback, خالی *arg)
{
    ساکن Atom wmp_atom = 0;
    ساکن Atom dw_atom = 0;
    Display *display = XtDisplay(shell);

    // deactivate the built-in delete response of killing the application
    Xt_Va_سیٹ_اقدار(shell, XmNdeleteResponse, XmDO_NOTHING, تہی);

    // add a delete window protocol callback instead
    اگر (!dw_atom)
    {
	wmp_atom = XmInternAtom(display, "WM_PROTOCOLS", True);
	dw_atom = XmInternAtom(display, "WM_DELETE_WINDOW", True);
    }
    Xm_جوڑو_Protocol_Callback(shell, wmp_atom, dw_atom, close_callback, arg);
}

#تعریف_کرو MAX_FONTS			65535
#تعریف_کرو MAX_FONT_NAME_LEN		256
#تعریف_کرو MAX_ENTRIES_اندر_LIST		5000
#تعریف_کرو MAX_DISPLAY_سائز		150
#تعریف_کرو عارضی_BUF_سائز			256

شمار ListSpecifier
{
    ENCODING,
    NAME,
    STYLE,
    سائز_3,
    NONE
};

قسمتعریف ساخت Shared_Font_Sel_ڈیٹا
{
    Widget	dialog;
    Widget	ok;
    Widget	cancel;
    Widget	encoding_pulldown;
    Widget	encoding_menu;
    Widget	list[NONE];
    Widget	name;
    Widget	sample;
    حرف	**نام;	// font name array of arrays
    صحیح		عدد;		// number of font names
    String	sel[NONE];	// selection category
    Boolean	اندر_pixels;	// toggle state - size in pixels
    حرف	*font_name;	// current font name
    XFontStruct	*old;		// font data structure for sample display
    XmFontList	old_list;	// font data structure for sample display
    Boolean	باہر_نکلو;		// used for program exit control
} Shared_Font_Sel_ڈیٹا_2;

/*
 * Checking access to the font name array for validity.
 */
    ساکن حرف *
fn(Shared_Font_Sel_ڈیٹا_2 *ڈیٹا, صحیح i)
{
    // Assertion checks:
    اگر (ڈیٹا->عدد < 0)
	abort();
    اگر (i >= ڈیٹا->عدد)
	i = ڈیٹا->عدد - 1;
    اگر (i < 0)
	i = 0;

    واپس ڈیٹا->نام[i];
}

/*
 * Get a specific substring from a font name.
 */
    ساکن خالی
get_part(حرف *اندر, صحیح pos, حرف *برآمد)
{
    صحیح	i;
    صحیح j;

    *برآمد = '\0';

    برائے (i = 0; (pos > 0) && (اندر[i] != '\0'); ++i)
	اگر (اندر[i] == '-')
	    pos--;

    اگر (اندر[i] == '\0')
	واپس;

    برائے (j = 0; (اندر[i] != '-') && (اندر[i] != '\0'); ++i, ++j)
	برآمد[j] = اندر[i];
    برآمد[j] = '\0';
}

/*
 * Given a font name this function returns the part used in the first
 * scroll list.
 */
    ساکن خالی
name_part(حرف *font, حرف *buf)
{
    حرف    buf2[عارضی_BUF_سائز];
    حرف    buf3[عارضی_BUF_سائز];

    get_part(font, 2, buf2);
    get_part(font, 1, buf3);

    اگر (*buf3 != NUL)
	vim_snprintf(buf, عارضی_BUF_سائز, "%s (%s)", buf2, buf3);
    ورنہ
	vim_snprintf(buf, عارضی_BUF_سائز, "%s", buf2);
}

/*
 * Given a font name this function returns the part used in the second scroll list.
 */
    ساکن خالی
style_part(حرف *font, حرف *buf)
{
    حرف    buf2[عارضی_BUF_سائز];
    حرف    buf3[عارضی_BUF_سائز];

    get_part(font, 3, buf3);
    get_part(font, 5, buf2);

    اگر (!strcmp(buf2, "normal") && !strcmp(buf2, "Normal")
						   && !strcmp(buf2, "NORMAL"))
	vim_snprintf(buf, عارضی_BUF_سائز, "%s %s", buf3, buf2);
    ورنہ
	strcpy(buf, buf3);

    get_part(font, 6, buf2);

    اگر (buf2[0] != '\0')
	vim_snprintf(buf3, عارضی_BUF_سائز, "%s %s", buf, buf2);
    ورنہ
	strcpy(buf3, buf);

    get_part(font, 4, buf2);

    اگر (!strcmp(buf2, "o") || !strcmp(buf2, "O"))
	vim_snprintf(buf, عارضی_BUF_سائز, "%s oblique", buf3);
    ورنہ اگر (!strcmp(buf2, "i") || !strcmp(buf2, "I"))
	vim_snprintf(buf, عارضی_BUF_سائز, "%s italic", buf3);

    اگر (!strcmp(buf, " "))
	strcpy(buf, "-");
}

/*
 * Given a font name this function returns the part used in the third
 * scroll list.
 */
    ساکن خالی
سائز_part(حرف *font, حرف *buf, صحیح اندر_Pixels)
{
    صحیح	    سائز;
    اعشاری   عارضی;

    *buf = '\0';

    اگر (اندر_Pixels)
    {
	get_part(font, 7, buf);
	اگر (*buf != NUL)
	{
	    سائز = atoi(buf);
	    sprintf(buf, "%3d", سائز);
	}
    }
    ورنہ
    {
	get_part(font, 8, buf);
	اگر (*buf != NUL)
	{
	    سائز = atoi(buf);
	    عارضی = (اعشاری)سائز / 10.0;
	    سائز = عارضی;
	    اگر (buf[لڑی_لمبائی(buf) - 1] == '0')
		sprintf(buf, "%3d", سائز);
	    ورنہ
		sprintf(buf, "%4.1f", عارضی);
	}
    }
}

/*
 * Given a font name this function returns the part used in the choice menu.
 */
    ساکن خالی
encoding_part(حرف *font, حرف *buf)
{
    حرف    buf1[عارضی_BUF_سائز];
    حرف    buf2[عارضی_BUF_سائز];

    *buf = '\0';

    get_part(font, 13, buf1);
    get_part(font, 14, buf2);

    اگر (*buf1 != NUL && *buf2 != NUL)
	vim_snprintf(buf, عارضی_BUF_سائز, "%s-%s", buf1, buf2);
    اگر (!strcmp(buf, " "))
	strcpy(buf, "-");
}

/*
 * Inserts a string into correct sorted position in a list.
 */
    ساکن خالی
جوڑو_مقصد_list(حرف **buf, حرف *شے, صحیح *گنتی)
{
    صحیح	i;
    صحیح j;

    اگر (*گنتی == MAX_ENTRIES_اندر_LIST)
	واپس;

    // avoid duplication
    برائے (i = 0; i < *گنتی; ++i)
    {
	اگر (!strcmp(buf[i], شے))
	    واپس;
    }

    // find order place, but make sure that wild card comes first
    اگر (!strcmp(شے, wild))
	i = 0;
    ورنہ
	برائے (i = 0; i < *گنتی; ++i)
	    اگر (strcmp(buf[i], شے) > 0 && strcmp(buf[i], wild))
		توڑو;

    // now insert the item
    برائے (j = *گنتی; j > i; --j)
	buf[j] = buf[j-1];
    buf[i] = XtNewString(شے);

    ++(*گنتی);
}

/*
 * True if the font matches some field.
 */
    ساکن Boolean
مطابقت(Shared_Font_Sel_ڈیٹا_2 *ڈیٹا, شمار ListSpecifier l, صحیح i)
{
    حرف buf[عارضی_BUF_سائز];

    // An empty selection or a wild card matches anything.
    اگر (!ڈیٹا->sel[l] || !strcmp(ڈیٹا->sel[l], wild))
	واپس True;

    // chunk out the desired part...
    انتخاب (l)
    {
	حالت ENCODING:
	    encoding_part(fn(ڈیٹا, i), buf);
	    توڑو;

	حالت NAME:
	    name_part(fn(ڈیٹا, i), buf);
	    توڑو;

	حالت STYLE:
	    style_part(fn(ڈیٹا, i), buf);
	    توڑو;

	حالت سائز_3:
	    سائز_part(fn(ڈیٹا, i), buf, ڈیٹا->اندر_pixels);
	    توڑو;
	طےشدہ:
	    ;
    }

    // ...and chew it now

    واپس !strcmp(buf, ڈیٹا->sel[l]);
}

    ساکن Boolean
proportional(حرف *font)
{
    حرف buf[عارضی_BUF_سائز];

    get_part(font, 11, buf);

    واپس !strcmp(buf, "p") || !strcmp(buf, "P");
}


ساکن خالی encoding_callback(Widget w, Shared_Font_Sel_ڈیٹا_2 *ڈیٹا,
							     Xt_اشارہ_گر dummy);

/*
 * Parse through the fontlist data and set up the three scroll lists.  The fix
 * parameter can be used to exclude a list from any changes.  This is used for
 * updates after selections caused by the users actions.
 */
    ساکن خالی
fill_lists(شمار ListSpecifier fix, Shared_Font_Sel_ڈیٹا_2 *ڈیٹا)
{
    حرف	*list[NONE][MAX_ENTRIES_اندر_LIST];
    صحیح		گنتی[NONE];
    حرف	buf[عارضی_BUF_سائز];
    XmString	اشیاء[MAX_ENTRIES_اندر_LIST];
    صحیح		i;
    صحیح		idx;

    برائے (idx = (صحیح)ENCODING; idx < (صحیح)NONE; ++idx)
	گنتی[idx] = 0;

    // First we insert the wild char into every single list.
    اگر (fix != ENCODING)
	جوڑو_مقصد_list(list[ENCODING], wild, &گنتی[ENCODING]);
    اگر (fix != NAME)
	جوڑو_مقصد_list(list[NAME], wild, &گنتی[NAME]);
    اگر (fix != STYLE)
	جوڑو_مقصد_list(list[STYLE], wild, &گنتی[STYLE]);
    اگر (fix != سائز_3)
	جوڑو_مقصد_list(list[سائز_3], wild, &گنتی[سائز_3]);

    برائے (i = 0; i < ڈیٹا->عدد && i < MAX_ENTRIES_اندر_LIST; i++)
    {
	اگر (proportional(fn(ڈیٹا, i)))
	    جاری;

	اگر (fix != ENCODING
		&& مطابقت(ڈیٹا, NAME, i)
		&& مطابقت(ڈیٹا, STYLE, i)
		&& مطابقت(ڈیٹا, سائز_3, i))
	{
	    encoding_part(fn(ڈیٹا, i), buf);
	    جوڑو_مقصد_list(list[ENCODING], buf, &گنتی[ENCODING]);
	}

	اگر (fix != NAME
		&& مطابقت(ڈیٹا, ENCODING, i)
		&& مطابقت(ڈیٹا, STYLE, i)
		&& مطابقت(ڈیٹا, سائز_3, i))
	{
	    name_part(fn(ڈیٹا, i), buf);
	    جوڑو_مقصد_list(list[NAME], buf, &گنتی[NAME]);
	}

	اگر (fix != STYLE
		&& مطابقت(ڈیٹا, ENCODING, i)
		&& مطابقت(ڈیٹا, NAME, i)
		&& مطابقت(ڈیٹا, سائز_3, i))
	{
	    style_part(fn(ڈیٹا, i), buf);
	    جوڑو_مقصد_list(list[STYLE], buf, &گنتی[STYLE]);
	}

	اگر (fix != سائز_3
		&& مطابقت(ڈیٹا, ENCODING, i)
		&& مطابقت(ڈیٹا, NAME, i)
		&& مطابقت(ڈیٹا, STYLE, i))
	{
	    سائز_part(fn(ڈیٹا, i), buf, ڈیٹا->اندر_pixels);
	    جوڑو_مقصد_list(list[سائز_3], buf, &گنتی[سائز_3]);
	}
    }

    /*
     * And now do the preselection in all lists where there was one:
     */

    اگر (fix != ENCODING)
    {
	Cardinal n_اشیاء;
	WidgetList children;
	Widget selected_button = 0;

	// Get and update the current button list.
	Xt_Va_Get_اقدار(ڈیٹا->encoding_pulldown,
		XmNchildren, &children,
		XmNnumChildren, &n_اشیاء,
		تہی);

	برائے (i = 0; i < گنتی[ENCODING]; ++i)
	{
	    Widget button;

	    اشیاء[i] = XmStringCreateLocalized(list[ENCODING][i]);

	    اگر (i < (صحیح)n_اشیاء)
	    {
		// recycle old button
		Xt_Va_سیٹ_اقدار(children[i],
			XmNlabelString, اشیاء[i],
			Xm_Nuser_ڈیٹا, i,
			تہی);
		button = children[i];
	    }
	    ورنہ
	    {
		// create a new button
		button = XtVaCreateManagedWidget("button",
			xm_ڈالو_Button_Gadget_Class,
			ڈیٹا->encoding_pulldown,
			XmNlabelString, اشیاء[i],
			Xm_Nuser_ڈیٹا, i,
			تہی);
		Xt_جوڑو_Callback(button, XmNactivateCallback,
			(XtCallbackProc) encoding_callback, (Xt_اشارہ_گر) ڈیٹا);
		Xt_Manage_بچہ(button);
	    }

	    اگر (ڈیٹا->sel[ENCODING])
	    {
		اگر (!strcmp(ڈیٹا->sel[ENCODING], list[ENCODING][i]))
		    selected_button = button;
	    }
	    XtFree(list[ENCODING][i]);
	}

	// Destroy all the outstanding menu items.
	برائے (i = گنتی[ENCODING]; i < (صحیح)n_اشیاء; ++i)
	{
	    Xt_Unmanage_بچہ(children[i]);
	    XtDestroyWidget(children[i]);
	}

	// Preserve the current selection visually.
	اگر (selected_button)
	{
	    Xt_Va_سیٹ_اقدار(ڈیٹا->encoding_menu,
		    XmNmenuHistory, selected_button,
		    تہی);
	}

	برائے (i = 0; i < گنتی[ENCODING]; ++i)
	    XmStringFree(اشیاء[i]);
    }

    /*
     * Now loop through the remaining lists and set them up.
     */
    برائے (idx = (صحیح)NAME; idx < (صحیح)NONE; ++idx)
    {
	Widget w;

	اگر (fix == (شمار ListSpecifier)idx)
	    جاری;

	انتخاب ((شمار ListSpecifier)idx)
	{
	    حالت NAME:
		w = ڈیٹا->list[NAME];
		توڑو;
	    حالت STYLE:
		w = ڈیٹا->list[STYLE];
		توڑو;
	    حالت سائز_3:
		w = ڈیٹا->list[سائز_3];
		توڑو;
	    طےشدہ:
		w = (Widget)0;	// for lint
	}

	برائے (i = 0; i < گنتی[idx]; ++i)
	{
	    اشیاء[i] = XmStringCreateLocalized(list[idx][i]);
	    XtFree(list[idx][i]);
	}
	Xm_List_Delete_All_اشیاء(w);
	Xm_List_جوڑو_اشیاء(w, اشیاء, گنتی[idx], 1);
	اگر (ڈیٹا->sel[idx])
	{
	    XmStringFree(اشیاء[0]);
	    اشیاء[0] = XmStringCreateLocalized(ڈیٹا->sel[idx]);
	    Xm_List_Select_شے(w, اشیاء[0], False);
	    Xm_List_سیٹ_Bottom_شے(w, اشیاء[0]);
	}
	برائے (i = 0; i < گنتی[idx]; ++i)
	    XmStringFree(اشیاء[i]);
    }
}

    ساکن خالی
stoggle_callback(Widget w غیر_مستعمل,
	Shared_Font_Sel_ڈیٹا_2 *ڈیٹا,
	XmToggleButtonCallbackStruct *call_ڈیٹا)
{
    صحیح		i, do_sel;
    حرف	new_سائز_2[عارضی_BUF_سائز];
    XmString	str;

    اگر (call_ڈیٹا->reason != (صحیح)Xm_CR_قدر_بدلا)
	واپس;

    do_sel = (ڈیٹا->sel[سائز_3] != تہی) && strcmp(ڈیٹا->sel[سائز_3], wild);

    برائے (i = 0; do_sel && (i < ڈیٹا->عدد); i++)
	اگر (مطابقت(ڈیٹا, ENCODING, i)
		&& مطابقت(ڈیٹا, NAME, i)
		&& مطابقت(ڈیٹا, STYLE, i)
		&& مطابقت(ڈیٹا, سائز_3, i))
	{
	    سائز_part(fn(ڈیٹا, i), new_سائز_2, !ڈیٹا->اندر_pixels);
	    توڑو;
	}

    ڈیٹا->اندر_pixels = !ڈیٹا->اندر_pixels;

    اگر (ڈیٹا->sel[سائز_3])
	XtFree(ڈیٹا->sel[سائز_3]);
    ڈیٹا->sel[سائز_3] = تہی;
    fill_lists(NONE, ڈیٹا);

    اگر (do_sel)
    {
	str = XmStringCreateLocalized(new_سائز_2);
	Xm_List_Select_شے(ڈیٹا->list[سائز_3], str, True);
	Xm_List_سیٹ_Bottom_شے(ڈیٹا->list[سائز_3], str);
	XmStringFree(str);
    }
}

/*
 * Show the currently selected font in the sample text label.
 */
    ساکن خالی
display_sample(Shared_Font_Sel_ڈیٹا_2 *ڈیٹا)
{
    Arg		    دلائل[2];
    صحیح		    n;
    XFontStruct	*   font;
    XmFontList	    font_list;
    Display *	    display;
    XmString	    str;

    display = XtDisplay(ڈیٹا->dialog);
    font = XLoadQueryFont(display, ڈیٹا->font_name);
    font_list = gui_motif_create_fontlist(font);

    n = 0;
    str = XmStringCreateLocalized("AaBbZzYy 0123456789");
    Xt_سیٹ_Arg(دلائل[n], XmNlabelString, str); n++;
    Xt_سیٹ_Arg(دلائل[n], XmNfontList, font_list); n++;

    Xt_سیٹ_اقدار(ڈیٹا->sample, دلائل, n);
    XmStringFree(str);

    اگر (ڈیٹا->old)
    {
	XFreeFont(display, ڈیٹا->old);
	XmFontListFree(ڈیٹا->old_list);
    }
    ڈیٹا->old = font;
    ڈیٹا->old_list = font_list;
}


    ساکن Boolean
do_choice(Widget w,
	Shared_Font_Sel_ڈیٹا_2 *ڈیٹا,
	XmListCallbackStruct *call_ڈیٹا,
	شمار ListSpecifier which)
{
    حرف *sel;

    XmStringGetLtoR(call_ڈیٹا->شے, XmSTRING_DEFAULT_CHARSET, &sel);

    اگر (!ڈیٹا->sel[which])
	ڈیٹا->sel[which] = XtNewString(sel);
    ورنہ
    {
	اگر (!strcmp(ڈیٹا->sel[which], sel))
	{
	    // unselecting current selection
	    XtFree(ڈیٹا->sel[which]);
	    ڈیٹا->sel[which] = تہی;
	    اگر (w)
		Xm_List_Deselect_شے(w, call_ڈیٹا->شے);
	}
	ورنہ
	{
	    XtFree(ڈیٹا->sel[which]);
	    ڈیٹا->sel[which] = XtNewString(sel);
	}
    }
    XtFree(sel);

    fill_lists(which, ڈیٹا);

    // If there is a font selection, we display it.
    اگر (ڈیٹا->sel[ENCODING]
	    && ڈیٹا->sel[NAME]
	    && ڈیٹا->sel[STYLE]
	    && ڈیٹا->sel[سائز_3]
	    && strcmp(ڈیٹا->sel[ENCODING], wild)
	    && strcmp(ڈیٹا->sel[NAME], wild)
	    && strcmp(ڈیٹا->sel[STYLE], wild)
	    && strcmp(ڈیٹا->sel[سائز_3], wild))
    {
	صحیح i;

	اگر (ڈیٹا->font_name)
	    XtFree(ڈیٹا->font_name);
	ڈیٹا->font_name = تہی;

	برائے (i = 0; i < ڈیٹا->عدد; i++)
	{
	    اگر (مطابقت(ڈیٹا, ENCODING, i)
		    && مطابقت(ڈیٹا, NAME, i)
		    && مطابقت(ڈیٹا, STYLE, i)
		    && مطابقت(ڈیٹا, سائز_3, i))
	    {
		ڈیٹا->font_name = XtNewString(fn(ڈیٹا, i));
		توڑو;
	    }
	}

	اگر (ڈیٹا->font_name)
	{
	    Xm_متن_سیٹ_String(ڈیٹا->name, ڈیٹا->font_name);
	    display_sample(ڈیٹا);
	}
	ورنہ
	    do_dialog(VIM_ERROR,
		    (char_u *)_("Error"),
		    (char_u *)_("Invalid font specification"),
		    (char_u *)_("&Dismiss"), 1, تہی, FALSE);

	واپس True;
    }
    ورنہ
    {
	صحیح	    n;
	XmString    str;
	Arg	    دلائل[4];
	حرف	    *nomatch_پیغام = _("no specific match");

	n = 0;
	str = XmStringCreateLocalized(nomatch_پیغام);
	Xt_سیٹ_Arg(دلائل[n], XmNlabelString, str); ++n;
	Xt_سیٹ_اقدار(ڈیٹا->sample, دلائل, n);
	apply_fontlist(ڈیٹا->sample);
	Xm_متن_سیٹ_String(ڈیٹا->name, nomatch_پیغام);
	XmStringFree(str);

	واپس False;
    }
}

    ساکن خالی
encoding_callback(Widget w,
	Shared_Font_Sel_ڈیٹا_2 *ڈیٹا,
	Xt_اشارہ_گر dummy غیر_مستعمل)
{
    XmString str;
    XmListCallbackStruct fake_ڈیٹا;

    Xt_Va_Get_اقدار(w, XmNlabelString, &str, تہی);

    اگر (!str)
	واپس;

    fake_ڈیٹا.شے = str;

    do_choice(0, ڈیٹا, &fake_ڈیٹا, ENCODING);
}

    ساکن خالی
name_callback(Widget w,
	Shared_Font_Sel_ڈیٹا_2 *ڈیٹا,
	XmListCallbackStruct *call_ڈیٹا)
{
    do_choice(w, ڈیٹا, call_ڈیٹا, NAME);
}

    ساکن خالی
style_callback(Widget w,
	Shared_Font_Sel_ڈیٹا_2 *ڈیٹا,
	XmListCallbackStruct *call_ڈیٹا)
{
    do_choice(w, ڈیٹا, call_ڈیٹا, STYLE);
}

    ساکن خالی
سائز_callback(Widget w,
	Shared_Font_Sel_ڈیٹا_2 *ڈیٹا,
	XmListCallbackStruct *call_ڈیٹا)
{
    do_choice(w, ڈیٹا, call_ڈیٹا, سائز_3);
}

    ساکن خالی
cancel_callback(Widget w غیر_مستعمل,
	Shared_Font_Sel_ڈیٹا_2 *ڈیٹا,
	XmListCallbackStruct *call_ڈیٹا غیر_مستعمل)
{
    اگر (ڈیٹا->sel[ENCODING])
    {
	XtFree(ڈیٹا->sel[ENCODING]);
	ڈیٹا->sel[ENCODING] = تہی;
    }
    اگر (ڈیٹا->sel[NAME])
    {
	XtFree(ڈیٹا->sel[NAME]);
	ڈیٹا->sel[NAME] = تہی;
    }
    اگر (ڈیٹا->sel[STYLE])
    {
	XtFree(ڈیٹا->sel[STYLE]);
	ڈیٹا->sel[STYLE] = تہی;
    }
    اگر (ڈیٹا->sel[سائز_3])
    {
	XtFree(ڈیٹا->sel[سائز_3]);
	ڈیٹا->sel[سائز_3] = تہی;
    }

    اگر (ڈیٹا->font_name)
	XtFree(ڈیٹا->font_name);
    ڈیٹا->font_name = تہی;

    ڈیٹا->عدد = 0;
    XFree_Font_نام(ڈیٹا->نام);
    ڈیٹا->نام = تہی;
    ڈیٹا->باہر_نکلو = True;
}

    ساکن خالی
ok_callback(Widget w غیر_مستعمل,
	Shared_Font_Sel_ڈیٹا_2 *ڈیٹا,
	Xm_ڈالو_Button_Callback_Struct *call_ڈیٹا غیر_مستعمل)
{
    حرف    *نمونہ;
    حرف    **name;
    صحیح	    i;

    نمونہ = Xm_متن_Get_String(ڈیٹا->name);
    name    = XListFonts(XtDisplay(ڈیٹا->dialog), نمونہ, 1, &i);
    XtFree(نمونہ);

    اگر (i != 1)
    {
	do_dialog(VIM_ERROR,
		(char_u *)_("Error"),
		(char_u *)_("Invalid font specification"),
		(char_u *)_("&Dismiss"), 1, تہی, FALSE);
	XFree_Font_نام(name);
    }
    ورنہ
    {
	اگر (ڈیٹا->font_name)
	    XtFree(ڈیٹا->font_name);
	ڈیٹا->font_name = XtNewString(name[0]);

	اگر (ڈیٹا->sel[ENCODING])
	{
	    XtFree(ڈیٹا->sel[ENCODING]);
	    ڈیٹا->sel[ENCODING] = تہی;
	}
	اگر (ڈیٹا->sel[NAME])
	{
	    XtFree(ڈیٹا->sel[NAME]);
	    ڈیٹا->sel[NAME] = تہی;
	}
	اگر (ڈیٹا->sel[STYLE])
	{
	    XtFree(ڈیٹا->sel[STYLE]);
	    ڈیٹا->sel[STYLE] = تہی;
	}
	اگر (ڈیٹا->sel[سائز_3])
	{
	    XtFree(ڈیٹا->sel[سائز_3]);
	    ڈیٹا->sel[سائز_3] = تہی;
	}

	XFree_Font_نام(name);

	ڈیٹا->عدد = 0;
	XFree_Font_نام(ڈیٹا->نام);
	ڈیٹا->نام = تہی;
	ڈیٹا->باہر_نکلو = True;
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
    ساکن Shared_Font_Sel_ڈیٹا_2	ڈیٹا_3;

    Widget		والد;
    Widget		form;
    Widget		separator;
    Widget		sub_form;
    Widget		سائز_toggle;
    Widget		name;
    Widget		disp_چوکھٹا;
    Widget		چوکھٹا;
    Arg			دلائل[64];
    صحیح			n;
    XmString		str;
    حرف		big_font[MAX_FONT_NAME_LEN];
    Shared_Font_Sel_ڈیٹا_2	*ڈیٹا;

    ڈیٹا = &ڈیٹا_3;

    والد = vimShell;
    ڈیٹا->نام = XListFonts(XtDisplay(والد), "-*-*-*-*-*-*-*-*-*-*-*-*-*-*",
						       MAX_FONTS, &ڈیٹا->عدد);

    /*
     * Find the name of the biggest font less than the given limit
     * MAX_DISPLAY_SIZE used to set up the initial height of the display
     * widget.
     */

    {
	صحیح	i;
	صحیح	max;
	صحیح	idx = 0;
	صحیح	سائز;
	حرف	buf[128];

	برائے (i = 0, max = 0; i < ڈیٹا->عدد; i++)
	{
	    get_part(fn(ڈیٹا, i), 7, buf);
	    سائز = atoi(buf);
	    اگر ((سائز > max) && (سائز < MAX_DISPLAY_سائز))
	    {
		idx = i;
		max = سائز;
	    }
	}
	strcpy(big_font, fn(ڈیٹا, idx));
    }
    ڈیٹا->old = XLoadQueryFont(XtDisplay(والد), big_font);
    ڈیٹا->old_list = gui_motif_create_fontlist(ڈیٹا->old);

    // Set the title of the Dialog window.
    ڈیٹا->dialog = XmCreateDialogShell(والد, "fontSelector", تہی, 0);
    str = XmStringCreateLocalized(_("Vim - Font Selector"));

    // Create form popup dialog widget.
    form = XtVaCreateWidget("form",
	    xmFormWidgetClass, ڈیٹا->dialog,
	    XmNdialogTitle, str,
	    XmNautoUnmanage, False,
	    XmNdialogStyle, Xm_DIALOG_بھرا_APPLICATION_MODAL,
	    تہی);
    XmStringFree(str);

    sub_form = XtVaCreateManagedWidget("subForm",
	    xmFormWidgetClass, form,
	    XmNbottomAttachment, XmATTACH_FORM,
	    Xm_Nbottom_انحراف, 4,
	    XmNrightAttachment, XmATTACH_FORM,
	    Xm_Nright_انحراف, 4,
	    XmNtopAttachment, XmATTACH_FORM,
	    Xm_Ntop_انحراف, 4,
	    XmNorientation, XmVERTICAL,
	    تہی);

    ڈیٹا->ok = XtVaCreateManagedWidget(_("OK"),
	    xm_ڈالو_Button_Gadget_Class, sub_form,
	    XmNleftAttachment, XmATTACH_FORM,
	    XmNrightAttachment, XmATTACH_FORM,
	    XmNtopAttachment, XmATTACH_FORM,
	    Xm_Ntop_انحراف, 4,
	    تہی);
    apply_fontlist(ڈیٹا->ok);

    ڈیٹا->cancel = XtVaCreateManagedWidget(_("Cancel"),
	    xm_ڈالو_Button_Gadget_Class, sub_form,
	    XmNrightAttachment, XmATTACH_FORM,
	    XmNleftAttachment, XmATTACH_FORM,
	    XmNtopAttachment, XmATTACH_WIDGET,
	    XmNtopWidget, ڈیٹا->ok,
	    Xm_Ntop_انحراف, 4,
	    XmNshowAsDefault, True,
	    تہی);
    apply_fontlist(ڈیٹا->cancel);

    // Create the separator for beauty.
    n = 0;
    Xt_سیٹ_Arg(دلائل[n], XmNorientation, XmVERTICAL); n++;
    Xt_سیٹ_Arg(دلائل[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    Xt_سیٹ_Arg(دلائل[n], XmNtopAttachment, XmATTACH_FORM); n++;
    Xt_سیٹ_Arg(دلائل[n], XmNrightAttachment, XmATTACH_WIDGET); n++;
    Xt_سیٹ_Arg(دلائل[n], XmNrightWidget, sub_form); n++;
    Xt_سیٹ_Arg(دلائل[n], Xm_Nright_انحراف, 4); n++;
    separator = XmCreateSeparatorGadget(form, "separator", دلائل, n);
    Xt_Manage_بچہ(separator);

    // Create font name text widget and the corresponding label.
    ڈیٹا->name = XtVaCreateManagedWidget("fontName",
	    xm_متن_Widget_Class, form,
	    XmNbottomAttachment, XmATTACH_FORM,
	    Xm_Nbottom_انحراف, 4,
	    XmNleftAttachment, XmATTACH_FORM,
	    Xm_Nleft_انحراف, 4,
	    XmNrightAttachment, XmATTACH_WIDGET,
	    XmNrightWidget, separator,
	    Xm_Nright_انحراف, 4,
	    XmNeditable, False,
	    Xm_Nedit_منوال, Xm_SINGLE_سطر_EDIT,
	    Xm_Nmax_لمبائی, MAX_FONT_NAME_LEN,
	    XmNcolumns, 60,
	    تہی);

    str = XmStringCreateLocalized(_("Name:"));
    name = XtVaCreateManagedWidget("fontNameLabel",
	    xmLabelGadgetClass, form,
	    XmNlabelString, str,
	    Xm_Nuser_ڈیٹا, ڈیٹا->name,
	    XmNleftAttachment, XmATTACH_OPPOSITE_WIDGET,
	    XmNleftWidget, ڈیٹا->name,
	    XmNbottomAttachment, XmATTACH_WIDGET,
	    XmNbottomWidget, ڈیٹا->name,
	    Xm_Ntop_انحراف, 1,
	    تہی);
    XmStringFree(str);
    apply_fontlist(name);

    // create sample display label widget
    disp_چوکھٹا = XtVaCreateManagedWidget("sampleFrame",
	    xm_چوکھٹا_Widget_Class, form,
	    XmNshadowType, Xm_SHADOW_ETCHED_اندر,
	    XmNleftAttachment, XmATTACH_FORM,
	    Xm_Nleft_انحراف, 4,
	    XmNbottomAttachment, XmATTACH_WIDGET,
	    XmNbottomWidget, name,
	    XmNrightAttachment, XmATTACH_WIDGET,
	    XmNrightWidget, separator,
	    Xm_Nright_انحراف, 4,
	    XmNalignment, XmALIGNMENT_BEGINNING,
	    تہی);

    ڈیٹا->sample = XtVaCreateManagedWidget("sampleLabel",
	    xmLabelWidgetClass, disp_چوکھٹا,
	    XmNleftAttachment, XmATTACH_FORM,
	    XmNtopAttachment, XmATTACH_FORM,
	    XmNbottomAttachment, XmATTACH_FORM,
	    XmNrightAttachment, XmATTACH_FORM,
	    XmNalignment, XmALIGNMENT_BEGINNING,
	    Xm_Nrecompute_سائز, False,
	    XmNfontList, ڈیٹا->old_list,
	    تہی);

    // create toggle button
    str = XmStringCreateLocalized(_("Show size in Points"));
    سائز_toggle = XtVaCreateManagedWidget("sizeToggle",
	    xmToggleButtonGadgetClass, form,
	    XmNlabelString, str,
	    XmNleftAttachment, XmATTACH_FORM,
	    Xm_Nleft_انحراف, 4,
	    XmNbottomAttachment, XmATTACH_WIDGET,
	    XmNbottomWidget, disp_چوکھٹا,
	    Xm_Nbottom_انحراف, 4,
	    تہی);
    XmStringFree(str);
    apply_fontlist(سائز_toggle);
    Xt_Manage_بچہ(سائز_toggle);

    // Encoding pulldown menu.

    ڈیٹا->encoding_pulldown = XmCreatePulldownMenu(form,
						 "encodingPulldown", تہی, 0);
    str = XmStringCreateLocalized(_("Encoding:"));
    n = 0;
    Xt_سیٹ_Arg(دلائل[n], XmNsubMenuId, ڈیٹا->encoding_pulldown); ++n;
    Xt_سیٹ_Arg(دلائل[n], XmNlabelString, str); ++n;
    Xt_سیٹ_Arg(دلائل[n], XmNleftAttachment, XmATTACH_FORM); ++n;
    Xt_سیٹ_Arg(دلائل[n], Xm_Nleft_انحراف, 4); ++n;
    Xt_سیٹ_Arg(دلائل[n], XmNbottomAttachment, XmATTACH_WIDGET); ++n;
    Xt_سیٹ_Arg(دلائل[n], XmNbottomWidget, سائز_toggle); ++n;
    Xt_سیٹ_Arg(دلائل[n], Xm_Nbottom_انحراف, 4); ++n;
    Xt_سیٹ_Arg(دلائل[n], XmNrightAttachment, XmATTACH_WIDGET); ++n;
    Xt_سیٹ_Arg(دلائل[n], XmNrightWidget, separator); ++n;
    Xt_سیٹ_Arg(دلائل[n], Xm_Nright_انحراف, 4); ++n;
    ڈیٹا->encoding_menu = XmCreateOptionMenu(form, "encodingMenu", دلائل, n);
    XmStringFree(str);
    Xm_جوڑو_Tab_گروہ(ڈیٹا->encoding_menu);

    /*
     * Create scroll list widgets in a separate subform used to manage the
     * different sizes of the lists.
     */

    sub_form = XtVaCreateManagedWidget("subForm",
	    xmFormWidgetClass, form,
	    XmNbottomAttachment, XmATTACH_WIDGET,
	    XmNbottomWidget, ڈیٹا->encoding_menu,
	    Xm_Nbottom_انحراف, 4,
	    XmNleftAttachment, XmATTACH_FORM,
	    Xm_Nleft_انحراف, 4,
	    XmNrightAttachment, XmATTACH_WIDGET,
	    XmNrightWidget, separator,
	    Xm_Nright_انحراف, 4,
	    XmNtopAttachment, XmATTACH_FORM,
	    Xm_Ntop_انحراف, 2,
	    XmNorientation, XmVERTICAL,
	    تہی);

    // font list
    چوکھٹا = XtVaCreateManagedWidget("frame", xm_چوکھٹا_Widget_Class, sub_form,
	    XmNshadowThickness, 0,
	    XmNtopAttachment, XmATTACH_FORM,
	    XmNbottomAttachment, XmATTACH_FORM,
	    XmNleftAttachment, XmATTACH_FORM,
	    XmNrightAttachment, XmATTACH_POSITION,
	    XmNrightPosition, 50,
	    تہی);

    str = XmStringCreateLocalized(_("Font:"));
    name = XtVaCreateManagedWidget("nameListLabel", xmLabelGadgetClass, چوکھٹا,
	    XmNchildType, Xm_چوکھٹا_TITLE_بچہ,
	    XmNchildVerticalAlignment, XmALIGNMENT_CENTER,
	    XmNchildHorizontalAlignment, XmALIGNMENT_BEGINNING,
	    XmNlabelString, str,
	    تہی);
    XmStringFree(str);
    apply_fontlist(name);

    n = 0;
    Xt_سیٹ_Arg(دلائل[n], Xm_Nvisible_شے_گنتی, 8); ++n;
    Xt_سیٹ_Arg(دلائل[n], XmNresizable, True); ++n;
    Xt_سیٹ_Arg(دلائل[n], Xm_Nlist_سائز_Policy, XmCONSTANT); ++n;
    Xt_سیٹ_Arg(دلائل[n], XmNvisualPolicy, XmVARIABLE); ++n;
#اگر_تعریف_شدہ LESSTIF_نسخہ
    Xt_سیٹ_Arg(دلائل[n], XmNscrollBarDisplayPolicy, XmSTATIC); ++n;
#اگر_ختم
    ڈیٹا->list[NAME] = XmCreateScrolledList(چوکھٹا, "fontList", دلائل, n);
    Xt_Va_سیٹ_اقدار(name, Xm_Nuser_ڈیٹا, ڈیٹا->list[NAME], تہی);

    // style list
    چوکھٹا = XtVaCreateManagedWidget("frame", xm_چوکھٹا_Widget_Class, sub_form,
	    XmNshadowThickness, 0,
	    XmNtopAttachment, XmATTACH_FORM,
	    XmNbottomAttachment, XmATTACH_FORM,
	    XmNleftAttachment, XmATTACH_POSITION,
	    XmNleftPosition, 50,
	    Xm_Nleft_انحراف, 4,
	    XmNrightAttachment, XmATTACH_POSITION,
	    XmNrightPosition, 80,
	    تہی);

    str = XmStringCreateLocalized(_("Style:"));
    name = XtVaCreateManagedWidget("styleListLabel", xmLabelWidgetClass, چوکھٹا,
	    XmNchildType, Xm_چوکھٹا_TITLE_بچہ,
	    XmNchildVerticalAlignment, XmALIGNMENT_CENTER,
	    XmNchildHorizontalAlignment, XmALIGNMENT_BEGINNING,
	    XmNlabelString, str,
	    تہی);
    XmStringFree(str);
    apply_fontlist(name);

    n = 0;
    Xt_سیٹ_Arg(دلائل[n], Xm_Nvisible_شے_گنتی, 8); ++n;
    Xt_سیٹ_Arg(دلائل[n], XmNresizable, True); ++n;
    Xt_سیٹ_Arg(دلائل[n], Xm_Nlist_سائز_Policy, XmCONSTANT); ++n;
    Xt_سیٹ_Arg(دلائل[n], XmNvisualPolicy, XmVARIABLE); ++n;
#اگر_تعریف_شدہ LESSTIF_نسخہ
    Xt_سیٹ_Arg(دلائل[n], XmNscrollBarDisplayPolicy, XmSTATIC); ++n;
#اگر_ختم
    ڈیٹا->list[STYLE] = XmCreateScrolledList(چوکھٹا, "styleList", دلائل, n);
    Xt_Va_سیٹ_اقدار(name, Xm_Nuser_ڈیٹا, ڈیٹا->list[STYLE], تہی);

    // size list
    چوکھٹا = XtVaCreateManagedWidget("frame", xm_چوکھٹا_Widget_Class, sub_form,
	    XmNshadowThickness, 0,
	    XmNtopAttachment, XmATTACH_FORM,
	    XmNbottomAttachment, XmATTACH_FORM,
	    XmNleftAttachment, XmATTACH_POSITION,
	    XmNleftPosition, 80,
	    Xm_Nleft_انحراف, 4,
	    XmNrightAttachment, XmATTACH_FORM,
	    تہی);

    str = XmStringCreateLocalized(_("Size:"));
    name = XtVaCreateManagedWidget("sizeListLabel", xmLabelGadgetClass, چوکھٹا,
	    XmNchildType, Xm_چوکھٹا_TITLE_بچہ,
	    XmNchildVerticalAlignment, XmALIGNMENT_CENTER,
	    XmNchildHorizontalAlignment, XmALIGNMENT_BEGINNING,
	    XmNlabelString, str,
	    تہی);
    XmStringFree(str);
    apply_fontlist(name);

    n = 0;
    Xt_سیٹ_Arg(دلائل[n], Xm_Nvisible_شے_گنتی, 8); ++n;
    Xt_سیٹ_Arg(دلائل[n], XmNresizable, True); ++n;
    Xt_سیٹ_Arg(دلائل[n], Xm_Nlist_سائز_Policy, XmCONSTANT); ++n;
    Xt_سیٹ_Arg(دلائل[n], XmNvisualPolicy, XmVARIABLE); ++n;
#اگر_تعریف_شدہ LESSTIF_نسخہ
    Xt_سیٹ_Arg(دلائل[n], XmNscrollBarDisplayPolicy, XmSTATIC); ++n;
#اگر_ختم
    ڈیٹا->list[سائز_3] = XmCreateScrolledList(چوکھٹا, "sizeList", دلائل, n);
    Xt_Va_سیٹ_اقدار(name, Xm_Nuser_ڈیٹا, ڈیٹا->list[سائز_3], تہی);

    // update form widgets cancel button
    Xt_Va_سیٹ_اقدار(form, XmNcancelButton, ڈیٹا->cancel, تہی);

    Xt_جوڑو_Callback(سائز_toggle, Xm_Nvalue_بدلا_Callback,
	    (XtCallbackProc)stoggle_callback, (Xt_اشارہ_گر)ڈیٹا);
    Xt_جوڑو_Callback(ڈیٹا->list[NAME], XmNbrowseSelectionCallback,
	    (XtCallbackProc)name_callback, (Xt_اشارہ_گر)ڈیٹا);
    Xt_جوڑو_Callback(ڈیٹا->list[STYLE], XmNbrowseSelectionCallback,
	    (XtCallbackProc)style_callback, (Xt_اشارہ_گر)ڈیٹا);
    Xt_جوڑو_Callback(ڈیٹا->list[سائز_3], XmNbrowseSelectionCallback,
	    (XtCallbackProc)سائز_callback, (Xt_اشارہ_گر)ڈیٹا);
    Xt_جوڑو_Callback(ڈیٹا->ok, XmNactivateCallback,
	    (XtCallbackProc)ok_callback, (Xt_اشارہ_گر)ڈیٹا);
    Xt_جوڑو_Callback(ڈیٹا->cancel, XmNactivateCallback,
	    (XtCallbackProc)cancel_callback, (Xt_اشارہ_گر)ڈیٹا);

    XmProcessTraversal(ڈیٹا->list[NAME], XmTRAVERSE_CURRENT);

    // setup tabgroups

    Xm_جوڑو_Tab_گروہ(ڈیٹا->list[NAME]);
    Xm_جوڑو_Tab_گروہ(ڈیٹا->list[STYLE]);
    Xm_جوڑو_Tab_گروہ(ڈیٹا->list[سائز_3]);
    Xm_جوڑو_Tab_گروہ(سائز_toggle);
    Xm_جوڑو_Tab_گروہ(ڈیٹا->name);
    Xm_جوڑو_Tab_گروہ(ڈیٹا->ok);
    Xm_جوڑو_Tab_گروہ(ڈیٹا->cancel);

    جوڑو_cancel_action(ڈیٹا->dialog, (XtCallbackProc)cancel_callback, ڈیٹا);

    // Preset selection data.

    ڈیٹا->باہر_نکلو = False;
    ڈیٹا->اندر_pixels= True;
    ڈیٹا->sel[ENCODING] = تہی;
    ڈیٹا->sel[NAME] = تہی;
    ڈیٹا->sel[STYLE] = تہی;
    ڈیٹا->sel[سائز_3] = تہی;
    ڈیٹا->font_name = تہی;

    // set up current font parameters
    اگر (current && current[0] != '\0')
    {
	صحیح	    i;
	حرف	    **نام;

	نام = XListFonts(XtDisplay(form), (حرف *) current, 1, &i);

	اگر (i != 0)
	{
	    حرف namebuf[عارضی_BUF_سائز];
	    حرف stylebuf[عارضی_BUF_سائز];
	    حرف sizebuf[عارضی_BUF_سائز];
	    حرف encodingbuf[عارضی_BUF_سائز];
	    حرف *ملا;

	    ملا = نام[0];

	    name_part(ملا, namebuf);
	    style_part(ملا, stylebuf);
	    سائز_part(ملا, sizebuf, ڈیٹا->اندر_pixels);
	    encoding_part(ملا, encodingbuf);

	    اگر (*namebuf != NUL
		    && *stylebuf != NUL
		    && *sizebuf != NUL
		    && *encodingbuf != NUL)
	    {
		ڈیٹا->sel[NAME] = XtNewString(namebuf);
		ڈیٹا->sel[STYLE] = XtNewString(stylebuf);
		ڈیٹا->sel[سائز_3] = XtNewString(sizebuf);
		ڈیٹا->sel[ENCODING] = XtNewString(encodingbuf);
		ڈیٹا->font_name = XtNewString(نام[0]);
		display_sample(ڈیٹا);
		Xm_متن_سیٹ_String(ڈیٹا->name, ڈیٹا->font_name);
	    }
	    ورنہ
	    {
		// We can't preset a symbolic name, which isn't a full font
		// description. Therefore we just behave the same way as if the
		// user didn't have selected anything thus far.
		//
		// Unfortunately there is no known way to expand an abbreviated
		// font name.
		ڈیٹا->font_name = تہی;
	    }
	}
	XFree_Font_نام(نام);
    }

    fill_lists(NONE, ڈیٹا);

    // Unfortunately LessTif doesn't align the list widget's properly.  I don't
    // have currently any idea how to fix this problem.
    Xt_Manage_بچہ(ڈیٹا->list[NAME]);
    Xt_Manage_بچہ(ڈیٹا->list[STYLE]);
    Xt_Manage_بچہ(ڈیٹا->list[سائز_3]);
    Xt_Manage_بچہ(ڈیٹا->encoding_menu);
    manage_centered(form);

    // modal event loop
    جب (!ڈیٹا->باہر_نکلو)
	XtAppProcessEvent(Xt_Widget_مقصد_Application_Context(ڈیٹا->dialog),
							(Xt_داخلہ_نقاب)XtIMAll);

    اگر (ڈیٹا->old)
    {
	XFreeFont(XtDisplay(ڈیٹا->dialog),  ڈیٹا->old);
	XmFontListFree(ڈیٹا->old_list);
    }
    XtDestroyWidget(ڈیٹا->dialog);

    gui_motif_synch_fonts();

    واپس (char_u *) ڈیٹا->font_name;
}
