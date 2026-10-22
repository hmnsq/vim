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

#ifdef FEAT_MENU
# define apply_fontlist(w) gui_motif_menu_fontlist(w)
#وإلا
# define apply_fontlist(w)
#endif

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
add_cancel_action(Widget shell, XtCallbackProc close_callback, فراغ *arg)
{
    ساكن Atom wmp_atom = 0;
    ساكن Atom dw_atom = 0;
    Display *display = XtDisplay(shell);

    // deactivate the built-in delete response of killing the application
    Xt_Va_Set_درجوجي(shell, XmNdeleteResponse, XmDO_NOTHING, عدم);

    // add a delete window protocol callback instead
    إذا (!dw_atom)
    {
	wmp_atom = XmInternAtom(display, "WM_PROTOCOLS", True);
	dw_atom = XmInternAtom(display, "WM_DELETE_WINDOW", True);
    }
    XmAddProtocolCallback(shell, wmp_atom, dw_atom, close_callback, arg);
}

#define MAX_FONTS			65535
#define MAX_FONT_NAME_LEN		256
#define MAX_ENTRIES_IN_LIST		5000
#define MAX_DISPLAY_SIZE		150
#define TEMP_BUF_SIZE			256

تعداد ListSpecifier
{
    ENCODING,
    NAME,
    STYLE,
    SIZE,
    NONE
};

تعريفنوع بنية _SharedFontSelData
{
    Widget	dialog;
    Widget	ok;
    Widget	cancel;
    Widget	encoding_pulldown;
    Widget	encoding_menu;
    Widget	list[NONE];
    Widget	name;
    Widget	sample;
    حرف	**names;	// font name array of arrays
    صحيح		لمب;		// number of font names
    String	sel[NONE];	// selection category
    Boolean	in_pixels;	// toggle state - size in pixels
    حرف	*font_name;	// current font name
    XFontStruct	*old;		// font data structure for sample display
    XmFontList	old_list;	// font data structure for sample display
    Boolean	exit;		// used for program exit control
} SharedFontSelData;

/*
 * Checking access to the font name array for validity.
 */
    ساكن حرف *
fn(SharedFontSelData *data, صحيح i)
{
    // Assertion checks:
    إذا (data->لمب < 0)
	abort();
    إذا (i >= data->لمب)
	i = data->لمب - 1;
    إذا (i < 0)
	i = 0;

    أرجع data->names[i];
}

/*
 * Get a specific substring from a font name.
 */
    ساكن فراغ
get_part(حرف *in, صحيح pos, حرف *out)
{
    صحيح	i;
    صحيح j;

    *out = '\0';

    لكل (i = 0; (pos > 0) && (in[i] != '\0'); ++i)
	إذا (in[i] == '-')
	    pos--;

    إذا (in[i] == '\0')
	أرجع;

    لكل (j = 0; (in[i] != '-') && (in[i] != '\0'); ++i, ++j)
	out[j] = in[i];
    out[j] = '\0';
}

/*
 * Given a font name this function returns the part used in the first
 * scroll list.
 */
    ساكن فراغ
name_part(حرف *font, حرف *buf)
{
    حرف    buf2[TEMP_BUF_SIZE];
    حرف    buf3[TEMP_BUF_SIZE];

    get_part(font, 2, buf2);
    get_part(font, 1, buf3);

    إذا (*buf3 != NUL)
	vim_snprintf(buf, TEMP_BUF_SIZE, "%s (%s)", buf2, buf3);
    وإلا
	vim_snprintf(buf, TEMP_BUF_SIZE, "%s", buf2);
}

/*
 * Given a font name this function returns the part used in the second scroll list.
 */
    ساكن فراغ
style_part(حرف *font, حرف *buf)
{
    حرف    buf2[TEMP_BUF_SIZE];
    حرف    buf3[TEMP_BUF_SIZE];

    get_part(font, 3, buf3);
    get_part(font, 5, buf2);

    إذا (!strcmp(buf2, "normal") && !strcmp(buf2, "Normal")
						   && !strcmp(buf2, "NORMAL"))
	vim_snprintf(buf, TEMP_BUF_SIZE, "%s %s", buf3, buf2);
    وإلا
	strcpy(buf, buf3);

    get_part(font, 6, buf2);

    إذا (buf2[0] != '\0')
	vim_snprintf(buf3, TEMP_BUF_SIZE, "%s %s", buf, buf2);
    وإلا
	strcpy(buf3, buf);

    get_part(font, 4, buf2);

    إذا (!strcmp(buf2, "o") || !strcmp(buf2, "O"))
	vim_snprintf(buf, TEMP_BUF_SIZE, "%s oblique", buf3);
    وإلا إذا (!strcmp(buf2, "i") || !strcmp(buf2, "I"))
	vim_snprintf(buf, TEMP_BUF_SIZE, "%s italic", buf3);

    إذا (!strcmp(buf, " "))
	strcpy(buf, "-");
}

/*
 * Given a font name this function returns the part used in the third
 * scroll list.
 */
    ساكن فراغ
size_part(حرف *font, حرف *buf, صحيح inPixels)
{
    صحيح	    size;
    عائم   temp;

    *buf = '\0';

    إذا (inPixels)
    {
	get_part(font, 7, buf);
	إذا (*buf != NUL)
	{
	    size = atoi(buf);
	    sprintf(buf, "%3d", size);
	}
    }
    وإلا
    {
	get_part(font, 8, buf);
	إذا (*buf != NUL)
	{
	    size = atoi(buf);
	    temp = (عائم)size / 10.0;
	    size = temp;
	    إذا (buf[strlen(buf) - 1] == '0')
		sprintf(buf, "%3d", size);
	    وإلا
		sprintf(buf, "%4.1f", temp);
	}
    }
}

/*
 * Given a font name this function returns the part used in the choice menu.
 */
    ساكن فراغ
encoding_part(حرف *font, حرف *buf)
{
    حرف    buf1[TEMP_BUF_SIZE];
    حرف    buf2[TEMP_BUF_SIZE];

    *buf = '\0';

    get_part(font, 13, buf1);
    get_part(font, 14, buf2);

    إذا (*buf1 != NUL && *buf2 != NUL)
	vim_snprintf(buf, TEMP_BUF_SIZE, "%s-%s", buf1, buf2);
    إذا (!strcmp(buf, " "))
	strcpy(buf, "-");
}

/*
 * Inserts a string into correct sorted position in a list.
 */
    ساكن فراغ
add_to_list(حرف **buf, حرف *item, صحيح *كدي)
{
    صحيح	i;
    صحيح j;

    إذا (*كدي == MAX_ENTRIES_IN_LIST)
	أرجع;

    // avoid duplication
    لكل (i = 0; i < *كدي; ++i)
    {
	إذا (!strcmp(buf[i], item))
	    أرجع;
    }

    // find order place, but make sure that wild card comes first
    إذا (!strcmp(item, wild))
	i = 0;
    وإلا
	لكل (i = 0; i < *كدي; ++i)
	    إذا (strcmp(buf[i], item) > 0 && strcmp(buf[i], wild))
		اكسر;

    // now insert the item
    لكل (j = *كدي; j > i; --j)
	buf[j] = buf[j-1];
    buf[i] = XtNewString(item);

    ++(*كدي);
}

/*
 * True if the font matches some field.
 */
    ساكن Boolean
match(SharedFontSelData *data, تعداد ListSpecifier l, صحيح i)
{
    حرف buf[TEMP_BUF_SIZE];

    // An empty selection or a wild card matches anything.
    إذا (!data->sel[l] || !strcmp(data->sel[l], wild))
	أرجع True;

    // chunk out the desired part...
    بدل (l)
    {
	حالة ENCODING:
	    encoding_part(fn(data, i), buf);
	    اكسر;

	حالة NAME:
	    name_part(fn(data, i), buf);
	    اكسر;

	حالة STYLE:
	    style_part(fn(data, i), buf);
	    اكسر;

	حالة SIZE:
	    size_part(fn(data, i), buf, data->in_pixels);
	    اكسر;
	افتراضي:
	    ;
    }

    // ...and chew it now

    أرجع !strcmp(buf, data->sel[l]);
}

    ساكن Boolean
proportional(حرف *font)
{
    حرف buf[TEMP_BUF_SIZE];

    get_part(font, 11, buf);

    أرجع !strcmp(buf, "p") || !strcmp(buf, "P");
}


ساكن فراغ encoding_callback(Widget w, SharedFontSelData *data,
							     XtPointer dummy);

/*
 * Parse through the fontlist data and set up the three scroll lists.  The fix
 * parameter can be used to exclude a list from any changes.  This is used for
 * updates after selections caused by the users actions.
 */
    ساكن فراغ
fill_lists(تعداد ListSpecifier fix, SharedFontSelData *data)
{
    حرف	*list[NONE][MAX_ENTRIES_IN_LIST];
    صحيح		كدي[NONE];
    حرف	buf[TEMP_BUF_SIZE];
    XmString	ابوبو[MAX_ENTRIES_IN_LIST];
    صحيح		i;
    صحيح		idx;

    لكل (idx = (صحيح)ENCODING; idx < (صحيح)NONE; ++idx)
	كدي[idx] = 0;

    // First we insert the wild char into every single list.
    إذا (fix != ENCODING)
	add_to_list(list[ENCODING], wild, &كدي[ENCODING]);
    إذا (fix != NAME)
	add_to_list(list[NAME], wild, &كدي[NAME]);
    إذا (fix != STYLE)
	add_to_list(list[STYLE], wild, &كدي[STYLE]);
    إذا (fix != SIZE)
	add_to_list(list[SIZE], wild, &كدي[SIZE]);

    لكل (i = 0; i < data->لمب && i < MAX_ENTRIES_IN_LIST; i++)
    {
	إذا (proportional(fn(data, i)))
	    استمر;

	إذا (fix != ENCODING
		&& match(data, NAME, i)
		&& match(data, STYLE, i)
		&& match(data, SIZE, i))
	{
	    encoding_part(fn(data, i), buf);
	    add_to_list(list[ENCODING], buf, &كدي[ENCODING]);
	}

	إذا (fix != NAME
		&& match(data, ENCODING, i)
		&& match(data, STYLE, i)
		&& match(data, SIZE, i))
	{
	    name_part(fn(data, i), buf);
	    add_to_list(list[NAME], buf, &كدي[NAME]);
	}

	إذا (fix != STYLE
		&& match(data, ENCODING, i)
		&& match(data, NAME, i)
		&& match(data, SIZE, i))
	{
	    style_part(fn(data, i), buf);
	    add_to_list(list[STYLE], buf, &كدي[STYLE]);
	}

	إذا (fix != SIZE
		&& match(data, ENCODING, i)
		&& match(data, NAME, i)
		&& match(data, STYLE, i))
	{
	    size_part(fn(data, i), buf, data->in_pixels);
	    add_to_list(list[SIZE], buf, &كدي[SIZE]);
	}
    }

    /*
     * And now do the preselection in all lists where there was one:
     */

    إذا (fix != ENCODING)
    {
	Cardinal n_ابوبو;
	WidgetList children;
	Widget selected_button = 0;

	// Get and update the current button list.
	Xt_Va_Get_درجوجي(data->encoding_pulldown,
		XmNchildren, &children,
		XmNnumChildren, &n_ابوبو,
		عدم);

	لكل (i = 0; i < كدي[ENCODING]; ++i)
	{
	    Widget button;

	    ابوبو[i] = XmStringCreateLocalized(list[ENCODING][i]);

	    إذا (i < (صحيح)n_ابوبو)
	    {
		// recycle old button
		Xt_Va_Set_درجوجي(children[i],
			XmNlabelString, ابوبو[i],
			XmNuserData, i,
			عدم);
		button = children[i];
	    }
	    وإلا
	    {
		// create a new button
		button = XtVaCreateManagedWidget("button",
			xm_تور_Button_Gadget_Class,
			data->encoding_pulldown,
			XmNlabelString, ابوبو[i],
			XmNuserData, i,
			عدم);
		XtAddCallback(button, XmNactivateCallback,
			(XtCallbackProc) encoding_callback, (XtPointer) data);
		XtManageChild(button);
	    }

	    إذا (data->sel[ENCODING])
	    {
		إذا (!strcmp(data->sel[ENCODING], list[ENCODING][i]))
		    selected_button = button;
	    }
	    XtFree(list[ENCODING][i]);
	}

	// Destroy all the outstanding menu items.
	لكل (i = كدي[ENCODING]; i < (صحيح)n_ابوبو; ++i)
	{
	    XtUnmanageChild(children[i]);
	    XtDestroyWidget(children[i]);
	}

	// Preserve the current selection visually.
	إذا (selected_button)
	{
	    Xt_Va_Set_درجوجي(data->encoding_menu,
		    XmNmenuHistory, selected_button,
		    عدم);
	}

	لكل (i = 0; i < كدي[ENCODING]; ++i)
	    XmStringFree(ابوبو[i]);
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
		w = data->list[NAME];
		اكسر;
	    حالة STYLE:
		w = data->list[STYLE];
		اكسر;
	    حالة SIZE:
		w = data->list[SIZE];
		اكسر;
	    افتراضي:
		w = (Widget)0;	// for lint
	}

	لكل (i = 0; i < كدي[idx]; ++i)
	{
	    ابوبو[i] = XmStringCreateLocalized(list[idx][i]);
	    XtFree(list[idx][i]);
	}
	Xm_List_Delete_All_ابوبو(w);
	Xm_List_Add_ابوبو(w, ابوبو, كدي[idx], 1);
	إذا (data->sel[idx])
	{
	    XmStringFree(ابوبو[0]);
	    ابوبو[0] = XmStringCreateLocalized(data->sel[idx]);
	    XmListSelectItem(w, ابوبو[0], False);
	    XmListSetBottomItem(w, ابوبو[0]);
	}
	لكل (i = 0; i < كدي[idx]; ++i)
	    XmStringFree(ابوبو[i]);
    }
}

    ساكن فراغ
stoggle_callback(Widget w UNUSED,
	SharedFontSelData *data,
	XmToggleButtonCallbackStruct *call_data)
{
    صحيح		i, do_sel;
    حرف	newSize[TEMP_BUF_SIZE];
    XmString	str;

    إذا (call_data->reason != (صحيح)Xm_CR_درج_CHANGED)
	أرجع;

    do_sel = (data->sel[SIZE] != عدم) && strcmp(data->sel[SIZE], wild);

    لكل (i = 0; do_sel && (i < data->لمب); i++)
	إذا (match(data, ENCODING, i)
		&& match(data, NAME, i)
		&& match(data, STYLE, i)
		&& match(data, SIZE, i))
	{
	    size_part(fn(data, i), newSize, !data->in_pixels);
	    اكسر;
	}

    data->in_pixels = !data->in_pixels;

    إذا (data->sel[SIZE])
	XtFree(data->sel[SIZE]);
    data->sel[SIZE] = عدم;
    fill_lists(NONE, data);

    إذا (do_sel)
    {
	str = XmStringCreateLocalized(newSize);
	XmListSelectItem(data->list[SIZE], str, True);
	XmListSetBottomItem(data->list[SIZE], str);
	XmStringFree(str);
    }
}

/*
 * Show the currently selected font in the sample text label.
 */
    ساكن فراغ
display_sample(SharedFontSelData *data)
{
    Arg		    حجوجي[2];
    صحيح		    n;
    XFontStruct	*   font;
    XmFontList	    font_list;
    Display *	    display;
    XmString	    str;

    display = XtDisplay(data->dialog);
    font = XLoadQueryFont(display, data->font_name);
    font_list = gui_motif_create_fontlist(font);

    n = 0;
    str = XmStringCreateLocalized("AaBbZzYy 0123456789");
    XtSetArg(حجوجي[n], XmNlabelString, str); n++;
    XtSetArg(حجوجي[n], XmNfontList, font_list); n++;

    Xt_Set_درجوجي(data->sample, حجوجي, n);
    XmStringFree(str);

    إذا (data->old)
    {
	XFreeFont(display, data->old);
	XmFontListFree(data->old_list);
    }
    data->old = font;
    data->old_list = font_list;
}


    ساكن Boolean
do_choice(Widget w,
	SharedFontSelData *data,
	XmListCallbackStruct *call_data,
	تعداد ListSpecifier which)
{
    حرف *sel;

    XmStringGetLtoR(call_data->item, XmSTRING_DEFAULT_CHARSET, &sel);

    إذا (!data->sel[which])
	data->sel[which] = XtNewString(sel);
    وإلا
    {
	إذا (!strcmp(data->sel[which], sel))
	{
	    // unselecting current selection
	    XtFree(data->sel[which]);
	    data->sel[which] = عدم;
	    إذا (w)
		XmListDeselectItem(w, call_data->item);
	}
	وإلا
	{
	    XtFree(data->sel[which]);
	    data->sel[which] = XtNewString(sel);
	}
    }
    XtFree(sel);

    fill_lists(which, data);

    // If there is a font selection, we display it.
    إذا (data->sel[ENCODING]
	    && data->sel[NAME]
	    && data->sel[STYLE]
	    && data->sel[SIZE]
	    && strcmp(data->sel[ENCODING], wild)
	    && strcmp(data->sel[NAME], wild)
	    && strcmp(data->sel[STYLE], wild)
	    && strcmp(data->sel[SIZE], wild))
    {
	صحيح i;

	إذا (data->font_name)
	    XtFree(data->font_name);
	data->font_name = عدم;

	لكل (i = 0; i < data->لمب; i++)
	{
	    إذا (match(data, ENCODING, i)
		    && match(data, NAME, i)
		    && match(data, STYLE, i)
		    && match(data, SIZE, i))
	    {
		data->font_name = XtNewString(fn(data, i));
		اكسر;
	    }
	}

	إذا (data->font_name)
	{
	    XmTextSetString(data->name, data->font_name);
	    display_sample(data);
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
	Arg	    حجوجي[4];
	حرف	    *nomatch_سكو = _("no specific match");

	n = 0;
	str = XmStringCreateLocalized(nomatch_سكو);
	XtSetArg(حجوجي[n], XmNlabelString, str); ++n;
	Xt_Set_درجوجي(data->sample, حجوجي, n);
	apply_fontlist(data->sample);
	XmTextSetString(data->name, nomatch_سكو);
	XmStringFree(str);

	أرجع False;
    }
}

    ساكن فراغ
encoding_callback(Widget w,
	SharedFontSelData *data,
	XtPointer dummy UNUSED)
{
    XmString str;
    XmListCallbackStruct fake_data;

    Xt_Va_Get_درجوجي(w, XmNlabelString, &str, عدم);

    إذا (!str)
	أرجع;

    fake_data.item = str;

    do_choice(0, data, &fake_data, ENCODING);
}

    ساكن فراغ
name_callback(Widget w,
	SharedFontSelData *data,
	XmListCallbackStruct *call_data)
{
    do_choice(w, data, call_data, NAME);
}

    ساكن فراغ
style_callback(Widget w,
	SharedFontSelData *data,
	XmListCallbackStruct *call_data)
{
    do_choice(w, data, call_data, STYLE);
}

    ساكن فراغ
size_callback(Widget w,
	SharedFontSelData *data,
	XmListCallbackStruct *call_data)
{
    do_choice(w, data, call_data, SIZE);
}

    ساكن فراغ
cancel_callback(Widget w UNUSED,
	SharedFontSelData *data,
	XmListCallbackStruct *call_data UNUSED)
{
    إذا (data->sel[ENCODING])
    {
	XtFree(data->sel[ENCODING]);
	data->sel[ENCODING] = عدم;
    }
    إذا (data->sel[NAME])
    {
	XtFree(data->sel[NAME]);
	data->sel[NAME] = عدم;
    }
    إذا (data->sel[STYLE])
    {
	XtFree(data->sel[STYLE]);
	data->sel[STYLE] = عدم;
    }
    إذا (data->sel[SIZE])
    {
	XtFree(data->sel[SIZE]);
	data->sel[SIZE] = عدم;
    }

    إذا (data->font_name)
	XtFree(data->font_name);
    data->font_name = عدم;

    data->لمب = 0;
    XFreeFontNames(data->names);
    data->names = عدم;
    data->exit = True;
}

    ساكن فراغ
ok_callback(Widget w UNUSED,
	SharedFontSelData *data,
	Xm_تور_Button_Callback_Struct *call_data UNUSED)
{
    حرف    *pattern;
    حرف    **name;
    صحيح	    i;

    pattern = XmTextGetString(data->name);
    name    = XListFonts(XtDisplay(data->dialog), pattern, 1, &i);
    XtFree(pattern);

    إذا (i != 1)
    {
	do_dialog(VIM_ERROR,
		(char_u *)_("Error"),
		(char_u *)_("Invalid font specification"),
		(char_u *)_("&Dismiss"), 1, عدم, FALSE);
	XFreeFontNames(name);
    }
    وإلا
    {
	إذا (data->font_name)
	    XtFree(data->font_name);
	data->font_name = XtNewString(name[0]);

	إذا (data->sel[ENCODING])
	{
	    XtFree(data->sel[ENCODING]);
	    data->sel[ENCODING] = عدم;
	}
	إذا (data->sel[NAME])
	{
	    XtFree(data->sel[NAME]);
	    data->sel[NAME] = عدم;
	}
	إذا (data->sel[STYLE])
	{
	    XtFree(data->sel[STYLE]);
	    data->sel[STYLE] = عدم;
	}
	إذا (data->sel[SIZE])
	{
	    XtFree(data->sel[SIZE]);
	    data->sel[SIZE] = عدم;
	}

	XFreeFontNames(name);

	data->لمب = 0;
	XFreeFontNames(data->names);
	data->names = عدم;
	data->exit = True;
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
    ساكن SharedFontSelData	_data;

    Widget		parent;
    Widget		form;
    Widget		separator;
    Widget		sub_form;
    Widget		size_toggle;
    Widget		name;
    Widget		disp_frame;
    Widget		frame;
    Arg			حجوجي[64];
    صحيح			n;
    XmString		str;
    حرف		big_font[MAX_FONT_NAME_LEN];
    SharedFontSelData	*data;

    data = &_data;

    parent = vimShell;
    data->names = XListFonts(XtDisplay(parent), "-*-*-*-*-*-*-*-*-*-*-*-*-*-*",
						       MAX_FONTS, &data->لمب);

    /*
     * Find the name of the biggest font less than the given limit
     * MAX_DISPLAY_SIZE used to set up the initial height of the display
     * widget.
     */

    {
	صحيح	i;
	صحيح	max;
	صحيح	idx = 0;
	صحيح	size;
	حرف	buf[128];

	لكل (i = 0, max = 0; i < data->لمب; i++)
	{
	    get_part(fn(data, i), 7, buf);
	    size = atoi(buf);
	    إذا ((size > max) && (size < MAX_DISPLAY_SIZE))
	    {
		idx = i;
		max = size;
	    }
	}
	strcpy(big_font, fn(data, idx));
    }
    data->old = XLoadQueryFont(XtDisplay(parent), big_font);
    data->old_list = gui_motif_create_fontlist(data->old);

    // Set the title of the Dialog window.
    data->dialog = XmCreateDialogShell(parent, "fontSelector", عدم, 0);
    str = XmStringCreateLocalized(_("Vim - Font Selector"));

    // Create form popup dialog widget.
    form = XtVaCreateWidget("form",
	    xmFormWidgetClass, data->dialog,
	    XmNdialogTitle, str,
	    XmNautoUnmanage, False,
	    XmNdialogStyle, XmDIALOG_FULL_APPLICATION_MODAL,
	    عدم);
    XmStringFree(str);

    sub_form = XtVaCreateManagedWidget("subForm",
	    xmFormWidgetClass, form,
	    XmNbottomAttachment, XmATTACH_FORM,
	    XmNbottomOffset, 4,
	    XmNrightAttachment, XmATTACH_FORM,
	    XmNrightOffset, 4,
	    XmNtopAttachment, XmATTACH_FORM,
	    XmNtopOffset, 4,
	    XmNorientation, XmVERTICAL,
	    عدم);

    data->ok = XtVaCreateManagedWidget(_("OK"),
	    xm_تور_Button_Gadget_Class, sub_form,
	    XmNleftAttachment, XmATTACH_FORM,
	    XmNrightAttachment, XmATTACH_FORM,
	    XmNtopAttachment, XmATTACH_FORM,
	    XmNtopOffset, 4,
	    عدم);
    apply_fontlist(data->ok);

    data->cancel = XtVaCreateManagedWidget(_("Cancel"),
	    xm_تور_Button_Gadget_Class, sub_form,
	    XmNrightAttachment, XmATTACH_FORM,
	    XmNleftAttachment, XmATTACH_FORM,
	    XmNtopAttachment, XmATTACH_WIDGET,
	    XmNtopWidget, data->ok,
	    XmNtopOffset, 4,
	    XmNshowAsDefault, True,
	    عدم);
    apply_fontlist(data->cancel);

    // Create the separator for beauty.
    n = 0;
    XtSetArg(حجوجي[n], XmNorientation, XmVERTICAL); n++;
    XtSetArg(حجوجي[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    XtSetArg(حجوجي[n], XmNtopAttachment, XmATTACH_FORM); n++;
    XtSetArg(حجوجي[n], XmNrightAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(حجوجي[n], XmNrightWidget, sub_form); n++;
    XtSetArg(حجوجي[n], XmNrightOffset, 4); n++;
    separator = XmCreateSeparatorGadget(form, "separator", حجوجي, n);
    XtManageChild(separator);

    // Create font name text widget and the corresponding label.
    data->name = XtVaCreateManagedWidget("fontName",
	    xmTextWidgetClass, form,
	    XmNbottomAttachment, XmATTACH_FORM,
	    XmNbottomOffset, 4,
	    XmNleftAttachment, XmATTACH_FORM,
	    XmNleftOffset, 4,
	    XmNrightAttachment, XmATTACH_WIDGET,
	    XmNrightWidget, separator,
	    XmNrightOffset, 4,
	    XmNeditable, False,
	    XmNeditMode, Xm_SINGLE_لي_EDIT,
	    XmNmaxLength, MAX_FONT_NAME_LEN,
	    XmNcolumns, 60,
	    عدم);

    str = XmStringCreateLocalized(_("Name:"));
    name = XtVaCreateManagedWidget("fontNameLabel",
	    xmLabelGadgetClass, form,
	    XmNlabelString, str,
	    XmNuserData, data->name,
	    XmNleftAttachment, XmATTACH_OPPOSITE_WIDGET,
	    XmNleftWidget, data->name,
	    XmNbottomAttachment, XmATTACH_WIDGET,
	    XmNbottomWidget, data->name,
	    XmNtopOffset, 1,
	    عدم);
    XmStringFree(str);
    apply_fontlist(name);

    // create sample display label widget
    disp_frame = XtVaCreateManagedWidget("sampleFrame",
	    xmFrameWidgetClass, form,
	    XmNshadowType, XmSHADOW_ETCHED_IN,
	    XmNleftAttachment, XmATTACH_FORM,
	    XmNleftOffset, 4,
	    XmNbottomAttachment, XmATTACH_WIDGET,
	    XmNbottomWidget, name,
	    XmNrightAttachment, XmATTACH_WIDGET,
	    XmNrightWidget, separator,
	    XmNrightOffset, 4,
	    XmNalignment, XmALIGNMENT_BEGINNING,
	    عدم);

    data->sample = XtVaCreateManagedWidget("sampleLabel",
	    xmLabelWidgetClass, disp_frame,
	    XmNleftAttachment, XmATTACH_FORM,
	    XmNtopAttachment, XmATTACH_FORM,
	    XmNbottomAttachment, XmATTACH_FORM,
	    XmNrightAttachment, XmATTACH_FORM,
	    XmNalignment, XmALIGNMENT_BEGINNING,
	    XmNrecomputeSize, False,
	    XmNfontList, data->old_list,
	    عدم);

    // create toggle button
    str = XmStringCreateLocalized(_("Show size in Points"));
    size_toggle = XtVaCreateManagedWidget("sizeToggle",
	    xmToggleButtonGadgetClass, form,
	    XmNlabelString, str,
	    XmNleftAttachment, XmATTACH_FORM,
	    XmNleftOffset, 4,
	    XmNbottomAttachment, XmATTACH_WIDGET,
	    XmNbottomWidget, disp_frame,
	    XmNbottomOffset, 4,
	    عدم);
    XmStringFree(str);
    apply_fontlist(size_toggle);
    XtManageChild(size_toggle);

    // Encoding pulldown menu.

    data->encoding_pulldown = XmCreatePulldownMenu(form,
						 "encodingPulldown", عدم, 0);
    str = XmStringCreateLocalized(_("Encoding:"));
    n = 0;
    XtSetArg(حجوجي[n], XmNsubMenuId, data->encoding_pulldown); ++n;
    XtSetArg(حجوجي[n], XmNlabelString, str); ++n;
    XtSetArg(حجوجي[n], XmNleftAttachment, XmATTACH_FORM); ++n;
    XtSetArg(حجوجي[n], XmNleftOffset, 4); ++n;
    XtSetArg(حجوجي[n], XmNbottomAttachment, XmATTACH_WIDGET); ++n;
    XtSetArg(حجوجي[n], XmNbottomWidget, size_toggle); ++n;
    XtSetArg(حجوجي[n], XmNbottomOffset, 4); ++n;
    XtSetArg(حجوجي[n], XmNrightAttachment, XmATTACH_WIDGET); ++n;
    XtSetArg(حجوجي[n], XmNrightWidget, separator); ++n;
    XtSetArg(حجوجي[n], XmNrightOffset, 4); ++n;
    data->encoding_menu = XmCreateOptionMenu(form, "encodingMenu", حجوجي, n);
    XmStringFree(str);
    XmAddTabGroup(data->encoding_menu);

    /*
     * Create scroll list widgets in a separate subform used to manage the
     * different sizes of the lists.
     */

    sub_form = XtVaCreateManagedWidget("subForm",
	    xmFormWidgetClass, form,
	    XmNbottomAttachment, XmATTACH_WIDGET,
	    XmNbottomWidget, data->encoding_menu,
	    XmNbottomOffset, 4,
	    XmNleftAttachment, XmATTACH_FORM,
	    XmNleftOffset, 4,
	    XmNrightAttachment, XmATTACH_WIDGET,
	    XmNrightWidget, separator,
	    XmNrightOffset, 4,
	    XmNtopAttachment, XmATTACH_FORM,
	    XmNtopOffset, 2,
	    XmNorientation, XmVERTICAL,
	    عدم);

    // font list
    frame = XtVaCreateManagedWidget("frame", xmFrameWidgetClass, sub_form,
	    XmNshadowThickness, 0,
	    XmNtopAttachment, XmATTACH_FORM,
	    XmNbottomAttachment, XmATTACH_FORM,
	    XmNleftAttachment, XmATTACH_FORM,
	    XmNrightAttachment, XmATTACH_POSITION,
	    XmNrightPosition, 50,
	    عدم);

    str = XmStringCreateLocalized(_("Font:"));
    name = XtVaCreateManagedWidget("nameListLabel", xmLabelGadgetClass, frame,
	    XmNchildType, XmFRAME_TITLE_CHILD,
	    XmNchildVerticalAlignment, XmALIGNMENT_CENTER,
	    XmNchildHorizontalAlignment, XmALIGNMENT_BEGINNING,
	    XmNlabelString, str,
	    عدم);
    XmStringFree(str);
    apply_fontlist(name);

    n = 0;
    XtSetArg(حجوجي[n], Xm_Nvisible_Item_كدي, 8); ++n;
    XtSetArg(حجوجي[n], XmNresizable, True); ++n;
    XtSetArg(حجوجي[n], XmNlistSizePolicy, XmCONSTANT); ++n;
    XtSetArg(حجوجي[n], XmNvisualPolicy, XmVARIABLE); ++n;
#ifdef LESSTIF_VERSION
    XtSetArg(حجوجي[n], XmNscrollBarDisplayPolicy, XmSTATIC); ++n;
#endif
    data->list[NAME] = XmCreateScrolledList(frame, "fontList", حجوجي, n);
    Xt_Va_Set_درجوجي(name, XmNuserData, data->list[NAME], عدم);

    // style list
    frame = XtVaCreateManagedWidget("frame", xmFrameWidgetClass, sub_form,
	    XmNshadowThickness, 0,
	    XmNtopAttachment, XmATTACH_FORM,
	    XmNbottomAttachment, XmATTACH_FORM,
	    XmNleftAttachment, XmATTACH_POSITION,
	    XmNleftPosition, 50,
	    XmNleftOffset, 4,
	    XmNrightAttachment, XmATTACH_POSITION,
	    XmNrightPosition, 80,
	    عدم);

    str = XmStringCreateLocalized(_("Style:"));
    name = XtVaCreateManagedWidget("styleListLabel", xmLabelWidgetClass, frame,
	    XmNchildType, XmFRAME_TITLE_CHILD,
	    XmNchildVerticalAlignment, XmALIGNMENT_CENTER,
	    XmNchildHorizontalAlignment, XmALIGNMENT_BEGINNING,
	    XmNlabelString, str,
	    عدم);
    XmStringFree(str);
    apply_fontlist(name);

    n = 0;
    XtSetArg(حجوجي[n], Xm_Nvisible_Item_كدي, 8); ++n;
    XtSetArg(حجوجي[n], XmNresizable, True); ++n;
    XtSetArg(حجوجي[n], XmNlistSizePolicy, XmCONSTANT); ++n;
    XtSetArg(حجوجي[n], XmNvisualPolicy, XmVARIABLE); ++n;
#ifdef LESSTIF_VERSION
    XtSetArg(حجوجي[n], XmNscrollBarDisplayPolicy, XmSTATIC); ++n;
#endif
    data->list[STYLE] = XmCreateScrolledList(frame, "styleList", حجوجي, n);
    Xt_Va_Set_درجوجي(name, XmNuserData, data->list[STYLE], عدم);

    // size list
    frame = XtVaCreateManagedWidget("frame", xmFrameWidgetClass, sub_form,
	    XmNshadowThickness, 0,
	    XmNtopAttachment, XmATTACH_FORM,
	    XmNbottomAttachment, XmATTACH_FORM,
	    XmNleftAttachment, XmATTACH_POSITION,
	    XmNleftPosition, 80,
	    XmNleftOffset, 4,
	    XmNrightAttachment, XmATTACH_FORM,
	    عدم);

    str = XmStringCreateLocalized(_("Size:"));
    name = XtVaCreateManagedWidget("sizeListLabel", xmLabelGadgetClass, frame,
	    XmNchildType, XmFRAME_TITLE_CHILD,
	    XmNchildVerticalAlignment, XmALIGNMENT_CENTER,
	    XmNchildHorizontalAlignment, XmALIGNMENT_BEGINNING,
	    XmNlabelString, str,
	    عدم);
    XmStringFree(str);
    apply_fontlist(name);

    n = 0;
    XtSetArg(حجوجي[n], Xm_Nvisible_Item_كدي, 8); ++n;
    XtSetArg(حجوجي[n], XmNresizable, True); ++n;
    XtSetArg(حجوجي[n], XmNlistSizePolicy, XmCONSTANT); ++n;
    XtSetArg(حجوجي[n], XmNvisualPolicy, XmVARIABLE); ++n;
#ifdef LESSTIF_VERSION
    XtSetArg(حجوجي[n], XmNscrollBarDisplayPolicy, XmSTATIC); ++n;
#endif
    data->list[SIZE] = XmCreateScrolledList(frame, "sizeList", حجوجي, n);
    Xt_Va_Set_درجوجي(name, XmNuserData, data->list[SIZE], عدم);

    // update form widgets cancel button
    Xt_Va_Set_درجوجي(form, XmNcancelButton, data->cancel, عدم);

    XtAddCallback(size_toggle, XmNvalueChangedCallback,
	    (XtCallbackProc)stoggle_callback, (XtPointer)data);
    XtAddCallback(data->list[NAME], XmNbrowseSelectionCallback,
	    (XtCallbackProc)name_callback, (XtPointer)data);
    XtAddCallback(data->list[STYLE], XmNbrowseSelectionCallback,
	    (XtCallbackProc)style_callback, (XtPointer)data);
    XtAddCallback(data->list[SIZE], XmNbrowseSelectionCallback,
	    (XtCallbackProc)size_callback, (XtPointer)data);
    XtAddCallback(data->ok, XmNactivateCallback,
	    (XtCallbackProc)ok_callback, (XtPointer)data);
    XtAddCallback(data->cancel, XmNactivateCallback,
	    (XtCallbackProc)cancel_callback, (XtPointer)data);

    XmProcessTraversal(data->list[NAME], XmTRAVERSE_CURRENT);

    // setup tabgroups

    XmAddTabGroup(data->list[NAME]);
    XmAddTabGroup(data->list[STYLE]);
    XmAddTabGroup(data->list[SIZE]);
    XmAddTabGroup(size_toggle);
    XmAddTabGroup(data->name);
    XmAddTabGroup(data->ok);
    XmAddTabGroup(data->cancel);

    add_cancel_action(data->dialog, (XtCallbackProc)cancel_callback, data);

    // Preset selection data.

    data->exit = False;
    data->in_pixels= True;
    data->sel[ENCODING] = عدم;
    data->sel[NAME] = عدم;
    data->sel[STYLE] = عدم;
    data->sel[SIZE] = عدم;
    data->font_name = عدم;

    // set up current font parameters
    إذا (current && current[0] != '\0')
    {
	صحيح	    i;
	حرف	    **names;

	names = XListFonts(XtDisplay(form), (حرف *) current, 1, &i);

	إذا (i != 0)
	{
	    حرف namebuf[TEMP_BUF_SIZE];
	    حرف stylebuf[TEMP_BUF_SIZE];
	    حرف sizebuf[TEMP_BUF_SIZE];
	    حرف encodingbuf[TEMP_BUF_SIZE];
	    حرف *found;

	    found = names[0];

	    name_part(found, namebuf);
	    style_part(found, stylebuf);
	    size_part(found, sizebuf, data->in_pixels);
	    encoding_part(found, encodingbuf);

	    إذا (*namebuf != NUL
		    && *stylebuf != NUL
		    && *sizebuf != NUL
		    && *encodingbuf != NUL)
	    {
		data->sel[NAME] = XtNewString(namebuf);
		data->sel[STYLE] = XtNewString(stylebuf);
		data->sel[SIZE] = XtNewString(sizebuf);
		data->sel[ENCODING] = XtNewString(encodingbuf);
		data->font_name = XtNewString(names[0]);
		display_sample(data);
		XmTextSetString(data->name, data->font_name);
	    }
	    وإلا
	    {
		// We can't preset a symbolic name, which isn't a full font
		// description. Therefore we just behave the same way as if the
		// user didn't have selected anything thus far.
		//
		// Unfortunately there is no known way to expand an abbreviated
		// font name.
		data->font_name = عدم;
	    }
	}
	XFreeFontNames(names);
    }

    fill_lists(NONE, data);

    // Unfortunately LessTif doesn't align the list widget's properly.  I don't
    // have currently any idea how to fix this problem.
    XtManageChild(data->list[NAME]);
    XtManageChild(data->list[STYLE]);
    XtManageChild(data->list[SIZE]);
    XtManageChild(data->encoding_menu);
    manage_centered(form);

    // modal event loop
    طالما (!data->exit)
	XtAppProcessEvent(XtWidgetToApplicationContext(data->dialog),
							(Xt_شغرو_Mask)XtIMAll);

    إذا (data->old)
    {
	XFreeFont(XtDisplay(data->dialog),  data->old);
	XmFontListFree(data->old_list);
    }
    XtDestroyWidget(data->dialog);

    gui_motif_synch_fonts();

    أرجع (char_u *) data->font_name;
}
