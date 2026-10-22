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

بیرونی Widget vimShell;

#ifdef FEAT_MENU
# define apply_fontlist(w) gui_motif_menu_fontlist(w)
#وگرنه
# define apply_fontlist(w)
#endif

/////////////////////////////////////////////////////////////////////////////
// Font selection dialogue implementation.

ایستا نویسه wild[3] = "*";

/*
 * FIXME: This is a generic function, which should be used throughout the whole
 * application.
 *
 * Add close_callback, which will be called when the user selects close from
 * the window menu.  The close menu item usually activates f.kill which sends a
 * WM_DELETE_WINDOW protocol request for the window.
 */

    ایستا تهی
add_cancel_action(Widget shell, XtCallbackProc close_callback, تهی *arg)
{
    ایستا Atom wmp_atom = 0;
    ایستا Atom dw_atom = 0;
    Display *display = XtDisplay(shell);

    // deactivate the built-in delete response of killing the application
    Xt_Va_Set_مقادیر(shell, XmNdeleteResponse, XmDO_NOTHING, پوچ);

    // add a delete window protocol callback instead
    اگر (!dw_atom)
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

شمارش ListSpecifier
{
    ENCODING,
    NAME,
    STYLE,
    SIZE,
    NONE
};

تعریفنوع ساختار _SharedFontSelData
{
    Widget	dialog;
    Widget	ok;
    Widget	cancel;
    Widget	encoding_pulldown;
    Widget	encoding_menu;
    Widget	list[NONE];
    Widget	name;
    Widget	sample;
    نویسه	**names;	// font name array of arrays
    صحیح		رقم;		// number of font names
    String	sel[NONE];	// selection category
    Boolean	in_pixels;	// toggle state - size in pixels
    نویسه	*font_name;	// current font name
    XFontStruct	*old;		// font data structure for sample display
    XmFontList	old_list;	// font data structure for sample display
    Boolean	exit;		// used for program exit control
} SharedFontSelData;

/*
 * Checking access to the font name array for validity.
 */
    ایستا نویسه *
fn(SharedFontSelData *data, صحیح i)
{
    // Assertion checks:
    اگر (data->رقم < 0)
	abort();
    اگر (i >= data->رقم)
	i = data->رقم - 1;
    اگر (i < 0)
	i = 0;

    بازگشت data->names[i];
}

/*
 * Get a specific substring from a font name.
 */
    ایستا تهی
get_part(نویسه *in, صحیح pos, نویسه *out)
{
    صحیح	i;
    صحیح j;

    *out = '\0';

    برای (i = 0; (pos > 0) && (in[i] != '\0'); ++i)
	اگر (in[i] == '-')
	    pos--;

    اگر (in[i] == '\0')
	بازگشت;

    برای (j = 0; (in[i] != '-') && (in[i] != '\0'); ++i, ++j)
	out[j] = in[i];
    out[j] = '\0';
}

/*
 * Given a font name this function returns the part used in the first
 * scroll list.
 */
    ایستا تهی
name_part(نویسه *font, نویسه *buf)
{
    نویسه    buf2[TEMP_BUF_SIZE];
    نویسه    buf3[TEMP_BUF_SIZE];

    get_part(font, 2, buf2);
    get_part(font, 1, buf3);

    اگر (*buf3 != NUL)
	vim_snprintf(buf, TEMP_BUF_SIZE, "%s (%s)", buf2, buf3);
    وگرنه
	vim_snprintf(buf, TEMP_BUF_SIZE, "%s", buf2);
}

/*
 * Given a font name this function returns the part used in the second scroll list.
 */
    ایستا تهی
style_part(نویسه *font, نویسه *buf)
{
    نویسه    buf2[TEMP_BUF_SIZE];
    نویسه    buf3[TEMP_BUF_SIZE];

    get_part(font, 3, buf3);
    get_part(font, 5, buf2);

    اگر (!strcmp(buf2, "normal") && !strcmp(buf2, "Normal")
						   && !strcmp(buf2, "NORMAL"))
	vim_snprintf(buf, TEMP_BUF_SIZE, "%s %s", buf3, buf2);
    وگرنه
	strcpy(buf, buf3);

    get_part(font, 6, buf2);

    اگر (buf2[0] != '\0')
	vim_snprintf(buf3, TEMP_BUF_SIZE, "%s %s", buf, buf2);
    وگرنه
	strcpy(buf3, buf);

    get_part(font, 4, buf2);

    اگر (!strcmp(buf2, "o") || !strcmp(buf2, "O"))
	vim_snprintf(buf, TEMP_BUF_SIZE, "%s oblique", buf3);
    وگرنه اگر (!strcmp(buf2, "i") || !strcmp(buf2, "I"))
	vim_snprintf(buf, TEMP_BUF_SIZE, "%s italic", buf3);

    اگر (!strcmp(buf, " "))
	strcpy(buf, "-");
}

/*
 * Given a font name this function returns the part used in the third
 * scroll list.
 */
    ایستا تهی
size_part(نویسه *font, نویسه *buf, صحیح inPixels)
{
    صحیح	    size;
    اعشاری   temp;

    *buf = '\0';

    اگر (inPixels)
    {
	get_part(font, 7, buf);
	اگر (*buf != NUL)
	{
	    size = atoi(buf);
	    sprintf(buf, "%3d", size);
	}
    }
    وگرنه
    {
	get_part(font, 8, buf);
	اگر (*buf != NUL)
	{
	    size = atoi(buf);
	    temp = (اعشاری)size / 10.0;
	    size = temp;
	    اگر (buf[strlen(buf) - 1] == '0')
		sprintf(buf, "%3d", size);
	    وگرنه
		sprintf(buf, "%4.1f", temp);
	}
    }
}

/*
 * Given a font name this function returns the part used in the choice menu.
 */
    ایستا تهی
encoding_part(نویسه *font, نویسه *buf)
{
    نویسه    buf1[TEMP_BUF_SIZE];
    نویسه    buf2[TEMP_BUF_SIZE];

    *buf = '\0';

    get_part(font, 13, buf1);
    get_part(font, 14, buf2);

    اگر (*buf1 != NUL && *buf2 != NUL)
	vim_snprintf(buf, TEMP_BUF_SIZE, "%s-%s", buf1, buf2);
    اگر (!strcmp(buf, " "))
	strcpy(buf, "-");
}

/*
 * Inserts a string into correct sorted position in a list.
 */
    ایستا تهی
add_to_list(نویسه **buf, نویسه *item, صحیح *شمار)
{
    صحیح	i;
    صحیح j;

    اگر (*شمار == MAX_ENTRIES_IN_LIST)
	بازگشت;

    // avoid duplication
    برای (i = 0; i < *شمار; ++i)
    {
	اگر (!strcmp(buf[i], item))
	    بازگشت;
    }

    // find order place, but make sure that wild card comes first
    اگر (!strcmp(item, wild))
	i = 0;
    وگرنه
	برای (i = 0; i < *شمار; ++i)
	    اگر (strcmp(buf[i], item) > 0 && strcmp(buf[i], wild))
		بشکن;

    // now insert the item
    برای (j = *شمار; j > i; --j)
	buf[j] = buf[j-1];
    buf[i] = XtNewString(item);

    ++(*شمار);
}

/*
 * True if the font matches some field.
 */
    ایستا Boolean
match(SharedFontSelData *data, شمارش ListSpecifier l, صحیح i)
{
    نویسه buf[TEMP_BUF_SIZE];

    // An empty selection or a wild card matches anything.
    اگر (!data->sel[l] || !strcmp(data->sel[l], wild))
	بازگشت True;

    // chunk out the desired part...
    انتخاب (l)
    {
	حالت ENCODING:
	    encoding_part(fn(data, i), buf);
	    بشکن;

	حالت NAME:
	    name_part(fn(data, i), buf);
	    بشکن;

	حالت STYLE:
	    style_part(fn(data, i), buf);
	    بشکن;

	حالت SIZE:
	    size_part(fn(data, i), buf, data->in_pixels);
	    بشکن;
	پیشفرض:
	    ;
    }

    // ...and chew it now

    بازگشت !strcmp(buf, data->sel[l]);
}

    ایستا Boolean
proportional(نویسه *font)
{
    نویسه buf[TEMP_BUF_SIZE];

    get_part(font, 11, buf);

    بازگشت !strcmp(buf, "p") || !strcmp(buf, "P");
}


ایستا تهی encoding_callback(Widget w, SharedFontSelData *data,
							     XtPointer dummy);

/*
 * Parse through the fontlist data and set up the three scroll lists.  The fix
 * parameter can be used to exclude a list from any changes.  This is used for
 * updates after selections caused by the users actions.
 */
    ایستا تهی
fill_lists(شمارش ListSpecifier fix, SharedFontSelData *data)
{
    نویسه	*list[NONE][MAX_ENTRIES_IN_LIST];
    صحیح		شمار[NONE];
    نویسه	buf[TEMP_BUF_SIZE];
    XmString	اقلام[MAX_ENTRIES_IN_LIST];
    صحیح		i;
    صحیح		idx;

    برای (idx = (صحیح)ENCODING; idx < (صحیح)NONE; ++idx)
	شمار[idx] = 0;

    // First we insert the wild char into every single list.
    اگر (fix != ENCODING)
	add_to_list(list[ENCODING], wild, &شمار[ENCODING]);
    اگر (fix != NAME)
	add_to_list(list[NAME], wild, &شمار[NAME]);
    اگر (fix != STYLE)
	add_to_list(list[STYLE], wild, &شمار[STYLE]);
    اگر (fix != SIZE)
	add_to_list(list[SIZE], wild, &شمار[SIZE]);

    برای (i = 0; i < data->رقم && i < MAX_ENTRIES_IN_LIST; i++)
    {
	اگر (proportional(fn(data, i)))
	    ادامه;

	اگر (fix != ENCODING
		&& match(data, NAME, i)
		&& match(data, STYLE, i)
		&& match(data, SIZE, i))
	{
	    encoding_part(fn(data, i), buf);
	    add_to_list(list[ENCODING], buf, &شمار[ENCODING]);
	}

	اگر (fix != NAME
		&& match(data, ENCODING, i)
		&& match(data, STYLE, i)
		&& match(data, SIZE, i))
	{
	    name_part(fn(data, i), buf);
	    add_to_list(list[NAME], buf, &شمار[NAME]);
	}

	اگر (fix != STYLE
		&& match(data, ENCODING, i)
		&& match(data, NAME, i)
		&& match(data, SIZE, i))
	{
	    style_part(fn(data, i), buf);
	    add_to_list(list[STYLE], buf, &شمار[STYLE]);
	}

	اگر (fix != SIZE
		&& match(data, ENCODING, i)
		&& match(data, NAME, i)
		&& match(data, STYLE, i))
	{
	    size_part(fn(data, i), buf, data->in_pixels);
	    add_to_list(list[SIZE], buf, &شمار[SIZE]);
	}
    }

    /*
     * And now do the preselection in all lists where there was one:
     */

    اگر (fix != ENCODING)
    {
	Cardinal n_اقلام;
	WidgetList children;
	Widget selected_button = 0;

	// Get and update the current button list.
	Xt_Va_Get_مقادیر(data->encoding_pulldown,
		XmNchildren, &children,
		XmNnumChildren, &n_اقلام,
		پوچ);

	برای (i = 0; i < شمار[ENCODING]; ++i)
	{
	    Widget button;

	    اقلام[i] = XmStringCreateLocalized(list[ENCODING][i]);

	    اگر (i < (صحیح)n_اقلام)
	    {
		// recycle old button
		Xt_Va_Set_مقادیر(children[i],
			XmNlabelString, اقلام[i],
			XmNuserData, i,
			پوچ);
		button = children[i];
	    }
	    وگرنه
	    {
		// create a new button
		button = XtVaCreateManagedWidget("button",
			xm_افزودن_Button_Gadget_Class,
			data->encoding_pulldown,
			XmNlabelString, اقلام[i],
			XmNuserData, i,
			پوچ);
		XtAddCallback(button, XmNactivateCallback,
			(XtCallbackProc) encoding_callback, (XtPointer) data);
		XtManageChild(button);
	    }

	    اگر (data->sel[ENCODING])
	    {
		اگر (!strcmp(data->sel[ENCODING], list[ENCODING][i]))
		    selected_button = button;
	    }
	    XtFree(list[ENCODING][i]);
	}

	// Destroy all the outstanding menu items.
	برای (i = شمار[ENCODING]; i < (صحیح)n_اقلام; ++i)
	{
	    XtUnmanageChild(children[i]);
	    XtDestroyWidget(children[i]);
	}

	// Preserve the current selection visually.
	اگر (selected_button)
	{
	    Xt_Va_Set_مقادیر(data->encoding_menu,
		    XmNmenuHistory, selected_button,
		    پوچ);
	}

	برای (i = 0; i < شمار[ENCODING]; ++i)
	    XmStringFree(اقلام[i]);
    }

    /*
     * Now loop through the remaining lists and set them up.
     */
    برای (idx = (صحیح)NAME; idx < (صحیح)NONE; ++idx)
    {
	Widget w;

	اگر (fix == (شمارش ListSpecifier)idx)
	    ادامه;

	انتخاب ((شمارش ListSpecifier)idx)
	{
	    حالت NAME:
		w = data->list[NAME];
		بشکن;
	    حالت STYLE:
		w = data->list[STYLE];
		بشکن;
	    حالت SIZE:
		w = data->list[SIZE];
		بشکن;
	    پیشفرض:
		w = (Widget)0;	// for lint
	}

	برای (i = 0; i < شمار[idx]; ++i)
	{
	    اقلام[i] = XmStringCreateLocalized(list[idx][i]);
	    XtFree(list[idx][i]);
	}
	Xm_List_Delete_All_اقلام(w);
	Xm_List_Add_اقلام(w, اقلام, شمار[idx], 1);
	اگر (data->sel[idx])
	{
	    XmStringFree(اقلام[0]);
	    اقلام[0] = XmStringCreateLocalized(data->sel[idx]);
	    XmListSelectItem(w, اقلام[0], False);
	    XmListSetBottomItem(w, اقلام[0]);
	}
	برای (i = 0; i < شمار[idx]; ++i)
	    XmStringFree(اقلام[i]);
    }
}

    ایستا تهی
stoggle_callback(Widget w UNUSED,
	SharedFontSelData *data,
	XmToggleButtonCallbackStruct *call_data)
{
    صحیح		i, do_sel;
    نویسه	newSize[TEMP_BUF_SIZE];
    XmString	str;

    اگر (call_data->reason != (صحیح)Xm_CR_مقدار_CHANGED)
	بازگشت;

    do_sel = (data->sel[SIZE] != پوچ) && strcmp(data->sel[SIZE], wild);

    برای (i = 0; do_sel && (i < data->رقم); i++)
	اگر (match(data, ENCODING, i)
		&& match(data, NAME, i)
		&& match(data, STYLE, i)
		&& match(data, SIZE, i))
	{
	    size_part(fn(data, i), newSize, !data->in_pixels);
	    بشکن;
	}

    data->in_pixels = !data->in_pixels;

    اگر (data->sel[SIZE])
	XtFree(data->sel[SIZE]);
    data->sel[SIZE] = پوچ;
    fill_lists(NONE, data);

    اگر (do_sel)
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
    ایستا تهی
display_sample(SharedFontSelData *data)
{
    Arg		    args[2];
    صحیح		    n;
    XFontStruct	*   font;
    XmFontList	    font_list;
    Display *	    display;
    XmString	    str;

    display = XtDisplay(data->dialog);
    font = XLoadQueryFont(display, data->font_name);
    font_list = gui_motif_create_fontlist(font);

    n = 0;
    str = XmStringCreateLocalized("AaBbZzYy 0123456789");
    XtSetArg(args[n], XmNlabelString, str); n++;
    XtSetArg(args[n], XmNfontList, font_list); n++;

    Xt_Set_مقادیر(data->sample, args, n);
    XmStringFree(str);

    اگر (data->old)
    {
	XFreeFont(display, data->old);
	XmFontListFree(data->old_list);
    }
    data->old = font;
    data->old_list = font_list;
}


    ایستا Boolean
do_choice(Widget w,
	SharedFontSelData *data,
	XmListCallbackStruct *call_data,
	شمارش ListSpecifier which)
{
    نویسه *sel;

    XmStringGetLtoR(call_data->item, XmSTRING_DEFAULT_CHARSET, &sel);

    اگر (!data->sel[which])
	data->sel[which] = XtNewString(sel);
    وگرنه
    {
	اگر (!strcmp(data->sel[which], sel))
	{
	    // unselecting current selection
	    XtFree(data->sel[which]);
	    data->sel[which] = پوچ;
	    اگر (w)
		XmListDeselectItem(w, call_data->item);
	}
	وگرنه
	{
	    XtFree(data->sel[which]);
	    data->sel[which] = XtNewString(sel);
	}
    }
    XtFree(sel);

    fill_lists(which, data);

    // If there is a font selection, we display it.
    اگر (data->sel[ENCODING]
	    && data->sel[NAME]
	    && data->sel[STYLE]
	    && data->sel[SIZE]
	    && strcmp(data->sel[ENCODING], wild)
	    && strcmp(data->sel[NAME], wild)
	    && strcmp(data->sel[STYLE], wild)
	    && strcmp(data->sel[SIZE], wild))
    {
	صحیح i;

	اگر (data->font_name)
	    XtFree(data->font_name);
	data->font_name = پوچ;

	برای (i = 0; i < data->رقم; i++)
	{
	    اگر (match(data, ENCODING, i)
		    && match(data, NAME, i)
		    && match(data, STYLE, i)
		    && match(data, SIZE, i))
	    {
		data->font_name = XtNewString(fn(data, i));
		بشکن;
	    }
	}

	اگر (data->font_name)
	{
	    XmTextSetString(data->name, data->font_name);
	    display_sample(data);
	}
	وگرنه
	    do_dialog(VIM_ERROR,
		    (char_u *)_("Error"),
		    (char_u *)_("Invalid font specification"),
		    (char_u *)_("&Dismiss"), 1, پوچ, FALSE);

	بازگشت True;
    }
    وگرنه
    {
	صحیح	    n;
	XmString    str;
	Arg	    args[4];
	نویسه	    *nomatch_پیام = _("no specific match");

	n = 0;
	str = XmStringCreateLocalized(nomatch_پیام);
	XtSetArg(args[n], XmNlabelString, str); ++n;
	Xt_Set_مقادیر(data->sample, args, n);
	apply_fontlist(data->sample);
	XmTextSetString(data->name, nomatch_پیام);
	XmStringFree(str);

	بازگشت False;
    }
}

    ایستا تهی
encoding_callback(Widget w,
	SharedFontSelData *data,
	XtPointer dummy UNUSED)
{
    XmString str;
    XmListCallbackStruct fake_data;

    Xt_Va_Get_مقادیر(w, XmNlabelString, &str, پوچ);

    اگر (!str)
	بازگشت;

    fake_data.item = str;

    do_choice(0, data, &fake_data, ENCODING);
}

    ایستا تهی
name_callback(Widget w,
	SharedFontSelData *data,
	XmListCallbackStruct *call_data)
{
    do_choice(w, data, call_data, NAME);
}

    ایستا تهی
style_callback(Widget w,
	SharedFontSelData *data,
	XmListCallbackStruct *call_data)
{
    do_choice(w, data, call_data, STYLE);
}

    ایستا تهی
size_callback(Widget w,
	SharedFontSelData *data,
	XmListCallbackStruct *call_data)
{
    do_choice(w, data, call_data, SIZE);
}

    ایستا تهی
cancel_callback(Widget w UNUSED,
	SharedFontSelData *data,
	XmListCallbackStruct *call_data UNUSED)
{
    اگر (data->sel[ENCODING])
    {
	XtFree(data->sel[ENCODING]);
	data->sel[ENCODING] = پوچ;
    }
    اگر (data->sel[NAME])
    {
	XtFree(data->sel[NAME]);
	data->sel[NAME] = پوچ;
    }
    اگر (data->sel[STYLE])
    {
	XtFree(data->sel[STYLE]);
	data->sel[STYLE] = پوچ;
    }
    اگر (data->sel[SIZE])
    {
	XtFree(data->sel[SIZE]);
	data->sel[SIZE] = پوچ;
    }

    اگر (data->font_name)
	XtFree(data->font_name);
    data->font_name = پوچ;

    data->رقم = 0;
    XFreeFontNames(data->names);
    data->names = پوچ;
    data->exit = True;
}

    ایستا تهی
ok_callback(Widget w UNUSED,
	SharedFontSelData *data,
	Xm_افزودن_Button_Callback_Struct *call_data UNUSED)
{
    نویسه    *pattern;
    نویسه    **name;
    صحیح	    i;

    pattern = XmTextGetString(data->name);
    name    = XListFonts(XtDisplay(data->dialog), pattern, 1, &i);
    XtFree(pattern);

    اگر (i != 1)
    {
	do_dialog(VIM_ERROR,
		(char_u *)_("Error"),
		(char_u *)_("Invalid font specification"),
		(char_u *)_("&Dismiss"), 1, پوچ, FALSE);
	XFreeFontNames(name);
    }
    وگرنه
    {
	اگر (data->font_name)
	    XtFree(data->font_name);
	data->font_name = XtNewString(name[0]);

	اگر (data->sel[ENCODING])
	{
	    XtFree(data->sel[ENCODING]);
	    data->sel[ENCODING] = پوچ;
	}
	اگر (data->sel[NAME])
	{
	    XtFree(data->sel[NAME]);
	    data->sel[NAME] = پوچ;
	}
	اگر (data->sel[STYLE])
	{
	    XtFree(data->sel[STYLE]);
	    data->sel[STYLE] = پوچ;
	}
	اگر (data->sel[SIZE])
	{
	    XtFree(data->sel[SIZE]);
	    data->sel[SIZE] = پوچ;
	}

	XFreeFontNames(name);

	data->رقم = 0;
	XFreeFontNames(data->names);
	data->names = پوچ;
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
    ایستا SharedFontSelData	_data;

    Widget		parent;
    Widget		form;
    Widget		separator;
    Widget		sub_form;
    Widget		size_toggle;
    Widget		name;
    Widget		disp_frame;
    Widget		frame;
    Arg			args[64];
    صحیح			n;
    XmString		str;
    نویسه		big_font[MAX_FONT_NAME_LEN];
    SharedFontSelData	*data;

    data = &_data;

    parent = vimShell;
    data->names = XListFonts(XtDisplay(parent), "-*-*-*-*-*-*-*-*-*-*-*-*-*-*",
						       MAX_FONTS, &data->رقم);

    /*
     * Find the name of the biggest font less than the given limit
     * MAX_DISPLAY_SIZE used to set up the initial height of the display
     * widget.
     */

    {
	صحیح	i;
	صحیح	max;
	صحیح	idx = 0;
	صحیح	size;
	نویسه	buf[128];

	برای (i = 0, max = 0; i < data->رقم; i++)
	{
	    get_part(fn(data, i), 7, buf);
	    size = atoi(buf);
	    اگر ((size > max) && (size < MAX_DISPLAY_SIZE))
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
    data->dialog = XmCreateDialogShell(parent, "fontSelector", پوچ, 0);
    str = XmStringCreateLocalized(_("Vim - Font Selector"));

    // Create form popup dialog widget.
    form = XtVaCreateWidget("form",
	    xmFormWidgetClass, data->dialog,
	    XmNdialogTitle, str,
	    XmNautoUnmanage, False,
	    XmNdialogStyle, XmDIALOG_FULL_APPLICATION_MODAL,
	    پوچ);
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
	    پوچ);

    data->ok = XtVaCreateManagedWidget(_("OK"),
	    xm_افزودن_Button_Gadget_Class, sub_form,
	    XmNleftAttachment, XmATTACH_FORM,
	    XmNrightAttachment, XmATTACH_FORM,
	    XmNtopAttachment, XmATTACH_FORM,
	    XmNtopOffset, 4,
	    پوچ);
    apply_fontlist(data->ok);

    data->cancel = XtVaCreateManagedWidget(_("Cancel"),
	    xm_افزودن_Button_Gadget_Class, sub_form,
	    XmNrightAttachment, XmATTACH_FORM,
	    XmNleftAttachment, XmATTACH_FORM,
	    XmNtopAttachment, XmATTACH_WIDGET,
	    XmNtopWidget, data->ok,
	    XmNtopOffset, 4,
	    XmNshowAsDefault, True,
	    پوچ);
    apply_fontlist(data->cancel);

    // Create the separator for beauty.
    n = 0;
    XtSetArg(args[n], XmNorientation, XmVERTICAL); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNrightWidget, sub_form); n++;
    XtSetArg(args[n], XmNrightOffset, 4); n++;
    separator = XmCreateSeparatorGadget(form, "separator", args, n);
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
	    XmNeditMode, Xm_SINGLE_سطر_EDIT,
	    XmNmaxLength, MAX_FONT_NAME_LEN,
	    XmNcolumns, 60,
	    پوچ);

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
	    پوچ);
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
	    پوچ);

    data->sample = XtVaCreateManagedWidget("sampleLabel",
	    xmLabelWidgetClass, disp_frame,
	    XmNleftAttachment, XmATTACH_FORM,
	    XmNtopAttachment, XmATTACH_FORM,
	    XmNbottomAttachment, XmATTACH_FORM,
	    XmNrightAttachment, XmATTACH_FORM,
	    XmNalignment, XmALIGNMENT_BEGINNING,
	    XmNrecomputeSize, False,
	    XmNfontList, data->old_list,
	    پوچ);

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
	    پوچ);
    XmStringFree(str);
    apply_fontlist(size_toggle);
    XtManageChild(size_toggle);

    // Encoding pulldown menu.

    data->encoding_pulldown = XmCreatePulldownMenu(form,
						 "encodingPulldown", پوچ, 0);
    str = XmStringCreateLocalized(_("Encoding:"));
    n = 0;
    XtSetArg(args[n], XmNsubMenuId, data->encoding_pulldown); ++n;
    XtSetArg(args[n], XmNlabelString, str); ++n;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); ++n;
    XtSetArg(args[n], XmNleftOffset, 4); ++n;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_WIDGET); ++n;
    XtSetArg(args[n], XmNbottomWidget, size_toggle); ++n;
    XtSetArg(args[n], XmNbottomOffset, 4); ++n;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_WIDGET); ++n;
    XtSetArg(args[n], XmNrightWidget, separator); ++n;
    XtSetArg(args[n], XmNrightOffset, 4); ++n;
    data->encoding_menu = XmCreateOptionMenu(form, "encodingMenu", args, n);
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
	    پوچ);

    // font list
    frame = XtVaCreateManagedWidget("frame", xmFrameWidgetClass, sub_form,
	    XmNshadowThickness, 0,
	    XmNtopAttachment, XmATTACH_FORM,
	    XmNbottomAttachment, XmATTACH_FORM,
	    XmNleftAttachment, XmATTACH_FORM,
	    XmNrightAttachment, XmATTACH_POSITION,
	    XmNrightPosition, 50,
	    پوچ);

    str = XmStringCreateLocalized(_("Font:"));
    name = XtVaCreateManagedWidget("nameListLabel", xmLabelGadgetClass, frame,
	    XmNchildType, XmFRAME_TITLE_CHILD,
	    XmNchildVerticalAlignment, XmALIGNMENT_CENTER,
	    XmNchildHorizontalAlignment, XmALIGNMENT_BEGINNING,
	    XmNlabelString, str,
	    پوچ);
    XmStringFree(str);
    apply_fontlist(name);

    n = 0;
    XtSetArg(args[n], Xm_Nvisible_Item_شمار, 8); ++n;
    XtSetArg(args[n], XmNresizable, True); ++n;
    XtSetArg(args[n], XmNlistSizePolicy, XmCONSTANT); ++n;
    XtSetArg(args[n], XmNvisualPolicy, XmVARIABLE); ++n;
#ifdef LESSTIF_VERSION
    XtSetArg(args[n], XmNscrollBarDisplayPolicy, XmSTATIC); ++n;
#endif
    data->list[NAME] = XmCreateScrolledList(frame, "fontList", args, n);
    Xt_Va_Set_مقادیر(name, XmNuserData, data->list[NAME], پوچ);

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
	    پوچ);

    str = XmStringCreateLocalized(_("Style:"));
    name = XtVaCreateManagedWidget("styleListLabel", xmLabelWidgetClass, frame,
	    XmNchildType, XmFRAME_TITLE_CHILD,
	    XmNchildVerticalAlignment, XmALIGNMENT_CENTER,
	    XmNchildHorizontalAlignment, XmALIGNMENT_BEGINNING,
	    XmNlabelString, str,
	    پوچ);
    XmStringFree(str);
    apply_fontlist(name);

    n = 0;
    XtSetArg(args[n], Xm_Nvisible_Item_شمار, 8); ++n;
    XtSetArg(args[n], XmNresizable, True); ++n;
    XtSetArg(args[n], XmNlistSizePolicy, XmCONSTANT); ++n;
    XtSetArg(args[n], XmNvisualPolicy, XmVARIABLE); ++n;
#ifdef LESSTIF_VERSION
    XtSetArg(args[n], XmNscrollBarDisplayPolicy, XmSTATIC); ++n;
#endif
    data->list[STYLE] = XmCreateScrolledList(frame, "styleList", args, n);
    Xt_Va_Set_مقادیر(name, XmNuserData, data->list[STYLE], پوچ);

    // size list
    frame = XtVaCreateManagedWidget("frame", xmFrameWidgetClass, sub_form,
	    XmNshadowThickness, 0,
	    XmNtopAttachment, XmATTACH_FORM,
	    XmNbottomAttachment, XmATTACH_FORM,
	    XmNleftAttachment, XmATTACH_POSITION,
	    XmNleftPosition, 80,
	    XmNleftOffset, 4,
	    XmNrightAttachment, XmATTACH_FORM,
	    پوچ);

    str = XmStringCreateLocalized(_("Size:"));
    name = XtVaCreateManagedWidget("sizeListLabel", xmLabelGadgetClass, frame,
	    XmNchildType, XmFRAME_TITLE_CHILD,
	    XmNchildVerticalAlignment, XmALIGNMENT_CENTER,
	    XmNchildHorizontalAlignment, XmALIGNMENT_BEGINNING,
	    XmNlabelString, str,
	    پوچ);
    XmStringFree(str);
    apply_fontlist(name);

    n = 0;
    XtSetArg(args[n], Xm_Nvisible_Item_شمار, 8); ++n;
    XtSetArg(args[n], XmNresizable, True); ++n;
    XtSetArg(args[n], XmNlistSizePolicy, XmCONSTANT); ++n;
    XtSetArg(args[n], XmNvisualPolicy, XmVARIABLE); ++n;
#ifdef LESSTIF_VERSION
    XtSetArg(args[n], XmNscrollBarDisplayPolicy, XmSTATIC); ++n;
#endif
    data->list[SIZE] = XmCreateScrolledList(frame, "sizeList", args, n);
    Xt_Va_Set_مقادیر(name, XmNuserData, data->list[SIZE], پوچ);

    // update form widgets cancel button
    Xt_Va_Set_مقادیر(form, XmNcancelButton, data->cancel, پوچ);

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
    data->sel[ENCODING] = پوچ;
    data->sel[NAME] = پوچ;
    data->sel[STYLE] = پوچ;
    data->sel[SIZE] = پوچ;
    data->font_name = پوچ;

    // set up current font parameters
    اگر (current && current[0] != '\0')
    {
	صحیح	    i;
	نویسه	    **names;

	names = XListFonts(XtDisplay(form), (نویسه *) current, 1, &i);

	اگر (i != 0)
	{
	    نویسه namebuf[TEMP_BUF_SIZE];
	    نویسه stylebuf[TEMP_BUF_SIZE];
	    نویسه sizebuf[TEMP_BUF_SIZE];
	    نویسه encodingbuf[TEMP_BUF_SIZE];
	    نویسه *found;

	    found = names[0];

	    name_part(found, namebuf);
	    style_part(found, stylebuf);
	    size_part(found, sizebuf, data->in_pixels);
	    encoding_part(found, encodingbuf);

	    اگر (*namebuf != NUL
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
	    وگرنه
	    {
		// We can't preset a symbolic name, which isn't a full font
		// description. Therefore we just behave the same way as if the
		// user didn't have selected anything thus far.
		//
		// Unfortunately there is no known way to expand an abbreviated
		// font name.
		data->font_name = پوچ;
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
    وقتی (!data->exit)
	XtAppProcessEvent(XtWidgetToApplicationContext(data->dialog),
							(Xt_درون_داد_Mask)XtIMAll);

    اگر (data->old)
    {
	XFreeFont(XtDisplay(data->dialog),  data->old);
	XmFontListFree(data->old_list);
    }
    XtDestroyWidget(data->dialog);

    gui_motif_synch_fonts();

    بازگشت (char_u *) data->font_name;
}
