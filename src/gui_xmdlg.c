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

#অন্তর্ভাব "vim.h"

#অন্তর্ভাব <Xm/Form.h>
#অন্তর্ভাব <Xm/PushBG.h>
#অন্তর্ভাব <Xm/Text.h>
#অন্তর্ভাব <Xm/TextF.h>
#অন্তর্ভাব <Xm/Label.h>
#অন্তর্ভাব <Xm/Frame.h>
#অন্তর্ভাব <Xm/LabelG.h>
#অন্তর্ভাব <Xm/ToggleBG.h>
#অন্তর্ভাব <Xm/SeparatoG.h>
#অন্তর্ভাব <Xm/DialogS.h>
#অন্তর্ভাব <Xm/List.h>
#অন্তর্ভাব <Xm/RowColumn.h>
#অন্তর্ভাব <Xm/AtomMgr.h>
#অন্তর্ভাব <Xm/Protocols.h>

#অন্তর্ভাব <X11/keysym.h>
#অন্তর্ভাব <X11/Xatom.h>
#অন্তর্ভাব <X11/StringDefs.h>
#অন্তর্ভাব <X11/Intrinsic.h>

বাহ্য Widget vimShell;

#ifdef FEAT_MENU
# define apply_fontlist(w) gui_motif_menu_fontlist(w)
#অন্যথা
# define apply_fontlist(w)
#endif

/////////////////////////////////////////////////////////////////////////////
// Font selection dialogue implementation.

স্থির বর্ণ wild[3] = "*";

/*
 * FIXME: This is a generic function, which should be used throughout the whole
 * application.
 *
 * Add close_callback, which will be called when the user selects close from
 * the window menu.  The close menu item usually activates f.kill which sends a
 * WM_DELETE_WINDOW protocol request for the window.
 */

    স্থির শূন্য
add_cancel_action(Widget shell, XtCallbackProc close_callback, শূন্য *arg)
{
    স্থির Atom wmp_atom = 0;
    স্থির Atom dw_atom = 0;
    Display *display = XtDisplay(shell);

    // deactivate the built-in delete response of killing the application
    Xt_Va_Set_মানসমূহ(shell, XmNdeleteResponse, XmDO_NOTHING, রিক্ত);

    // add a delete window protocol callback instead
    যদি (!dw_atom)
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

গণনা ListSpecifier
{
    ENCODING,
    NAME,
    STYLE,
    SIZE,
    NONE
};

প্রকারনির্দেশ সংরচনা _SharedFontSelData
{
    Widget	dialog;
    Widget	ok;
    Widget	cancel;
    Widget	encoding_pulldown;
    Widget	encoding_menu;
    Widget	list[NONE];
    Widget	name;
    Widget	sample;
    বর্ণ	**names;	// font name array of arrays
    পূর্ণাঙ্ক		অঙ্ক;		// number of font names
    String	sel[NONE];	// selection category
    Boolean	in_pixels;	// toggle state - size in pixels
    বর্ণ	*font_name;	// current font name
    XFontStruct	*old;		// font data structure for sample display
    XmFontList	old_list;	// font data structure for sample display
    Boolean	exit;		// used for program exit control
} SharedFontSelData;

/*
 * Checking access to the font name array for validity.
 */
    স্থির বর্ণ *
fn(SharedFontSelData *data, পূর্ণাঙ্ক i)
{
    // Assertion checks:
    যদি (data->অঙ্ক < 0)
	abort();
    যদি (i >= data->অঙ্ক)
	i = data->অঙ্ক - 1;
    যদি (i < 0)
	i = 0;

    প্রত্যাগম data->names[i];
}

/*
 * Get a specific substring from a font name.
 */
    স্থির শূন্য
get_part(বর্ণ *in, পূর্ণাঙ্ক pos, বর্ণ *out)
{
    পূর্ণাঙ্ক	i;
    পূর্ণাঙ্ক j;

    *out = '\0';

    প্রতি (i = 0; (pos > 0) && (in[i] != '\0'); ++i)
	যদি (in[i] == '-')
	    pos--;

    যদি (in[i] == '\0')
	প্রত্যাগম;

    প্রতি (j = 0; (in[i] != '-') && (in[i] != '\0'); ++i, ++j)
	out[j] = in[i];
    out[j] = '\0';
}

/*
 * Given a font name this function returns the part used in the first
 * scroll list.
 */
    স্থির শূন্য
name_part(বর্ণ *font, বর্ণ *buf)
{
    বর্ণ    buf2[TEMP_BUF_SIZE];
    বর্ণ    buf3[TEMP_BUF_SIZE];

    get_part(font, 2, buf2);
    get_part(font, 1, buf3);

    যদি (*buf3 != NUL)
	vim_snprintf(buf, TEMP_BUF_SIZE, "%s (%s)", buf2, buf3);
    অন্যথা
	vim_snprintf(buf, TEMP_BUF_SIZE, "%s", buf2);
}

/*
 * Given a font name this function returns the part used in the second scroll list.
 */
    স্থির শূন্য
style_part(বর্ণ *font, বর্ণ *buf)
{
    বর্ণ    buf2[TEMP_BUF_SIZE];
    বর্ণ    buf3[TEMP_BUF_SIZE];

    get_part(font, 3, buf3);
    get_part(font, 5, buf2);

    যদি (!strcmp(buf2, "normal") && !strcmp(buf2, "Normal")
						   && !strcmp(buf2, "NORMAL"))
	vim_snprintf(buf, TEMP_BUF_SIZE, "%s %s", buf3, buf2);
    অন্যথা
	strcpy(buf, buf3);

    get_part(font, 6, buf2);

    যদি (buf2[0] != '\0')
	vim_snprintf(buf3, TEMP_BUF_SIZE, "%s %s", buf, buf2);
    অন্যথা
	strcpy(buf3, buf);

    get_part(font, 4, buf2);

    যদি (!strcmp(buf2, "o") || !strcmp(buf2, "O"))
	vim_snprintf(buf, TEMP_BUF_SIZE, "%s oblique", buf3);
    অন্যথা যদি (!strcmp(buf2, "i") || !strcmp(buf2, "I"))
	vim_snprintf(buf, TEMP_BUF_SIZE, "%s italic", buf3);

    যদি (!strcmp(buf, " "))
	strcpy(buf, "-");
}

/*
 * Given a font name this function returns the part used in the third
 * scroll list.
 */
    স্থির শূন্য
size_part(বর্ণ *font, বর্ণ *buf, পূর্ণাঙ্ক inPixels)
{
    পূর্ণাঙ্ক	    size;
    ভিন্ন   temp;

    *buf = '\0';

    যদি (inPixels)
    {
	get_part(font, 7, buf);
	যদি (*buf != NUL)
	{
	    size = atoi(buf);
	    sprintf(buf, "%3d", size);
	}
    }
    অন্যথা
    {
	get_part(font, 8, buf);
	যদি (*buf != NUL)
	{
	    size = atoi(buf);
	    temp = (ভিন্ন)size / 10.0;
	    size = temp;
	    যদি (buf[strlen(buf) - 1] == '0')
		sprintf(buf, "%3d", size);
	    অন্যথা
		sprintf(buf, "%4.1f", temp);
	}
    }
}

/*
 * Given a font name this function returns the part used in the choice menu.
 */
    স্থির শূন্য
encoding_part(বর্ণ *font, বর্ণ *buf)
{
    বর্ণ    buf1[TEMP_BUF_SIZE];
    বর্ণ    buf2[TEMP_BUF_SIZE];

    *buf = '\0';

    get_part(font, 13, buf1);
    get_part(font, 14, buf2);

    যদি (*buf1 != NUL && *buf2 != NUL)
	vim_snprintf(buf, TEMP_BUF_SIZE, "%s-%s", buf1, buf2);
    যদি (!strcmp(buf, " "))
	strcpy(buf, "-");
}

/*
 * Inserts a string into correct sorted position in a list.
 */
    স্থির শূন্য
add_to_list(বর্ণ **buf, বর্ণ *item, পূর্ণাঙ্ক *গুনতি)
{
    পূর্ণাঙ্ক	i;
    পূর্ণাঙ্ক j;

    যদি (*গুনতি == MAX_ENTRIES_IN_LIST)
	প্রত্যাগম;

    // avoid duplication
    প্রতি (i = 0; i < *গুনতি; ++i)
    {
	যদি (!strcmp(buf[i], item))
	    প্রত্যাগম;
    }

    // find order place, but make sure that wild card comes first
    যদি (!strcmp(item, wild))
	i = 0;
    অন্যথা
	প্রতি (i = 0; i < *গুনতি; ++i)
	    যদি (strcmp(buf[i], item) > 0 && strcmp(buf[i], wild))
		ভঙ্গ;

    // now insert the item
    প্রতি (j = *গুনতি; j > i; --j)
	buf[j] = buf[j-1];
    buf[i] = XtNewString(item);

    ++(*গুনতি);
}

/*
 * True if the font matches some field.
 */
    স্থির Boolean
match(SharedFontSelData *data, গণনা ListSpecifier l, পূর্ণাঙ্ক i)
{
    বর্ণ buf[TEMP_BUF_SIZE];

    // An empty selection or a wild card matches anything.
    যদি (!data->sel[l] || !strcmp(data->sel[l], wild))
	প্রত্যাগম True;

    // chunk out the desired part...
    বিকল্প (l)
    {
	পক্ষ ENCODING:
	    encoding_part(fn(data, i), buf);
	    ভঙ্গ;

	পক্ষ NAME:
	    name_part(fn(data, i), buf);
	    ভঙ্গ;

	পক্ষ STYLE:
	    style_part(fn(data, i), buf);
	    ভঙ্গ;

	পক্ষ SIZE:
	    size_part(fn(data, i), buf, data->in_pixels);
	    ভঙ্গ;
	স্বভাব:
	    ;
    }

    // ...and chew it now

    প্রত্যাগম !strcmp(buf, data->sel[l]);
}

    স্থির Boolean
proportional(বর্ণ *font)
{
    বর্ণ buf[TEMP_BUF_SIZE];

    get_part(font, 11, buf);

    প্রত্যাগম !strcmp(buf, "p") || !strcmp(buf, "P");
}


স্থির শূন্য encoding_callback(Widget w, SharedFontSelData *data,
							     XtPointer dummy);

/*
 * Parse through the fontlist data and set up the three scroll lists.  The fix
 * parameter can be used to exclude a list from any changes.  This is used for
 * updates after selections caused by the users actions.
 */
    স্থির শূন্য
fill_lists(গণনা ListSpecifier fix, SharedFontSelData *data)
{
    বর্ণ	*list[NONE][MAX_ENTRIES_IN_LIST];
    পূর্ণাঙ্ক		গুনতি[NONE];
    বর্ণ	buf[TEMP_BUF_SIZE];
    XmString	বস্তুসমূহ[MAX_ENTRIES_IN_LIST];
    পূর্ণাঙ্ক		i;
    পূর্ণাঙ্ক		idx;

    প্রতি (idx = (পূর্ণাঙ্ক)ENCODING; idx < (পূর্ণাঙ্ক)NONE; ++idx)
	গুনতি[idx] = 0;

    // First we insert the wild char into every single list.
    যদি (fix != ENCODING)
	add_to_list(list[ENCODING], wild, &গুনতি[ENCODING]);
    যদি (fix != NAME)
	add_to_list(list[NAME], wild, &গুনতি[NAME]);
    যদি (fix != STYLE)
	add_to_list(list[STYLE], wild, &গুনতি[STYLE]);
    যদি (fix != SIZE)
	add_to_list(list[SIZE], wild, &গুনতি[SIZE]);

    প্রতি (i = 0; i < data->অঙ্ক && i < MAX_ENTRIES_IN_LIST; i++)
    {
	যদি (proportional(fn(data, i)))
	    অনুবর্ত;

	যদি (fix != ENCODING
		&& match(data, NAME, i)
		&& match(data, STYLE, i)
		&& match(data, SIZE, i))
	{
	    encoding_part(fn(data, i), buf);
	    add_to_list(list[ENCODING], buf, &গুনতি[ENCODING]);
	}

	যদি (fix != NAME
		&& match(data, ENCODING, i)
		&& match(data, STYLE, i)
		&& match(data, SIZE, i))
	{
	    name_part(fn(data, i), buf);
	    add_to_list(list[NAME], buf, &গুনতি[NAME]);
	}

	যদি (fix != STYLE
		&& match(data, ENCODING, i)
		&& match(data, NAME, i)
		&& match(data, SIZE, i))
	{
	    style_part(fn(data, i), buf);
	    add_to_list(list[STYLE], buf, &গুনতি[STYLE]);
	}

	যদি (fix != SIZE
		&& match(data, ENCODING, i)
		&& match(data, NAME, i)
		&& match(data, STYLE, i))
	{
	    size_part(fn(data, i), buf, data->in_pixels);
	    add_to_list(list[SIZE], buf, &গুনতি[SIZE]);
	}
    }

    /*
     * And now do the preselection in all lists where there was one:
     */

    যদি (fix != ENCODING)
    {
	Cardinal n_বস্তুসমূহ;
	WidgetList children;
	Widget selected_button = 0;

	// Get and update the current button list.
	Xt_Va_Get_মানসমূহ(data->encoding_pulldown,
		XmNchildren, &children,
		XmNnumChildren, &n_বস্তুসমূহ,
		রিক্ত);

	প্রতি (i = 0; i < গুনতি[ENCODING]; ++i)
	{
	    Widget button;

	    বস্তুসমূহ[i] = XmStringCreateLocalized(list[ENCODING][i]);

	    যদি (i < (পূর্ণাঙ্ক)n_বস্তুসমূহ)
	    {
		// recycle old button
		Xt_Va_Set_মানসমূহ(children[i],
			XmNlabelString, বস্তুসমূহ[i],
			XmNuserData, i,
			রিক্ত);
		button = children[i];
	    }
	    অন্যথা
	    {
		// create a new button
		button = XtVaCreateManagedWidget("button",
			xm_ঠেলো_Button_Gadget_Class,
			data->encoding_pulldown,
			XmNlabelString, বস্তুসমূহ[i],
			XmNuserData, i,
			রিক্ত);
		XtAddCallback(button, XmNactivateCallback,
			(XtCallbackProc) encoding_callback, (XtPointer) data);
		XtManageChild(button);
	    }

	    যদি (data->sel[ENCODING])
	    {
		যদি (!strcmp(data->sel[ENCODING], list[ENCODING][i]))
		    selected_button = button;
	    }
	    XtFree(list[ENCODING][i]);
	}

	// Destroy all the outstanding menu items.
	প্রতি (i = গুনতি[ENCODING]; i < (পূর্ণাঙ্ক)n_বস্তুসমূহ; ++i)
	{
	    XtUnmanageChild(children[i]);
	    XtDestroyWidget(children[i]);
	}

	// Preserve the current selection visually.
	যদি (selected_button)
	{
	    Xt_Va_Set_মানসমূহ(data->encoding_menu,
		    XmNmenuHistory, selected_button,
		    রিক্ত);
	}

	প্রতি (i = 0; i < গুনতি[ENCODING]; ++i)
	    XmStringFree(বস্তুসমূহ[i]);
    }

    /*
     * Now loop through the remaining lists and set them up.
     */
    প্রতি (idx = (পূর্ণাঙ্ক)NAME; idx < (পূর্ণাঙ্ক)NONE; ++idx)
    {
	Widget w;

	যদি (fix == (গণনা ListSpecifier)idx)
	    অনুবর্ত;

	বিকল্প ((গণনা ListSpecifier)idx)
	{
	    পক্ষ NAME:
		w = data->list[NAME];
		ভঙ্গ;
	    পক্ষ STYLE:
		w = data->list[STYLE];
		ভঙ্গ;
	    পক্ষ SIZE:
		w = data->list[SIZE];
		ভঙ্গ;
	    স্বভাব:
		w = (Widget)0;	// for lint
	}

	প্রতি (i = 0; i < গুনতি[idx]; ++i)
	{
	    বস্তুসমূহ[i] = XmStringCreateLocalized(list[idx][i]);
	    XtFree(list[idx][i]);
	}
	Xm_List_Delete_All_বস্তুসমূহ(w);
	Xm_List_Add_বস্তুসমূহ(w, বস্তুসমূহ, গুনতি[idx], 1);
	যদি (data->sel[idx])
	{
	    XmStringFree(বস্তুসমূহ[0]);
	    বস্তুসমূহ[0] = XmStringCreateLocalized(data->sel[idx]);
	    XmListSelectItem(w, বস্তুসমূহ[0], False);
	    XmListSetBottomItem(w, বস্তুসমূহ[0]);
	}
	প্রতি (i = 0; i < গুনতি[idx]; ++i)
	    XmStringFree(বস্তুসমূহ[i]);
    }
}

    স্থির শূন্য
stoggle_callback(Widget w UNUSED,
	SharedFontSelData *data,
	XmToggleButtonCallbackStruct *call_data)
{
    পূর্ণাঙ্ক		i, do_sel;
    বর্ণ	newSize[TEMP_BUF_SIZE];
    XmString	str;

    যদি (call_data->reason != (পূর্ণাঙ্ক)Xm_CR_মান_CHANGED)
	প্রত্যাগম;

    do_sel = (data->sel[SIZE] != রিক্ত) && strcmp(data->sel[SIZE], wild);

    প্রতি (i = 0; do_sel && (i < data->অঙ্ক); i++)
	যদি (match(data, ENCODING, i)
		&& match(data, NAME, i)
		&& match(data, STYLE, i)
		&& match(data, SIZE, i))
	{
	    size_part(fn(data, i), newSize, !data->in_pixels);
	    ভঙ্গ;
	}

    data->in_pixels = !data->in_pixels;

    যদি (data->sel[SIZE])
	XtFree(data->sel[SIZE]);
    data->sel[SIZE] = রিক্ত;
    fill_lists(NONE, data);

    যদি (do_sel)
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
    স্থির শূন্য
display_sample(SharedFontSelData *data)
{
    Arg		    args[2];
    পূর্ণাঙ্ক		    n;
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

    Xt_Set_মানসমূহ(data->sample, args, n);
    XmStringFree(str);

    যদি (data->old)
    {
	XFreeFont(display, data->old);
	XmFontListFree(data->old_list);
    }
    data->old = font;
    data->old_list = font_list;
}


    স্থির Boolean
do_choice(Widget w,
	SharedFontSelData *data,
	XmListCallbackStruct *call_data,
	গণনা ListSpecifier which)
{
    বর্ণ *sel;

    XmStringGetLtoR(call_data->item, XmSTRING_DEFAULT_CHARSET, &sel);

    যদি (!data->sel[which])
	data->sel[which] = XtNewString(sel);
    অন্যথা
    {
	যদি (!strcmp(data->sel[which], sel))
	{
	    // unselecting current selection
	    XtFree(data->sel[which]);
	    data->sel[which] = রিক্ত;
	    যদি (w)
		XmListDeselectItem(w, call_data->item);
	}
	অন্যথা
	{
	    XtFree(data->sel[which]);
	    data->sel[which] = XtNewString(sel);
	}
    }
    XtFree(sel);

    fill_lists(which, data);

    // If there is a font selection, we display it.
    যদি (data->sel[ENCODING]
	    && data->sel[NAME]
	    && data->sel[STYLE]
	    && data->sel[SIZE]
	    && strcmp(data->sel[ENCODING], wild)
	    && strcmp(data->sel[NAME], wild)
	    && strcmp(data->sel[STYLE], wild)
	    && strcmp(data->sel[SIZE], wild))
    {
	পূর্ণাঙ্ক i;

	যদি (data->font_name)
	    XtFree(data->font_name);
	data->font_name = রিক্ত;

	প্রতি (i = 0; i < data->অঙ্ক; i++)
	{
	    যদি (match(data, ENCODING, i)
		    && match(data, NAME, i)
		    && match(data, STYLE, i)
		    && match(data, SIZE, i))
	    {
		data->font_name = XtNewString(fn(data, i));
		ভঙ্গ;
	    }
	}

	যদি (data->font_name)
	{
	    XmTextSetString(data->name, data->font_name);
	    display_sample(data);
	}
	অন্যথা
	    do_dialog(VIM_ERROR,
		    (char_u *)_("Error"),
		    (char_u *)_("Invalid font specification"),
		    (char_u *)_("&Dismiss"), 1, রিক্ত, FALSE);

	প্রত্যাগম True;
    }
    অন্যথা
    {
	পূর্ণাঙ্ক	    n;
	XmString    str;
	Arg	    args[4];
	বর্ণ	    *nomatch_বার্তা = _("no specific match");

	n = 0;
	str = XmStringCreateLocalized(nomatch_বার্তা);
	XtSetArg(args[n], XmNlabelString, str); ++n;
	Xt_Set_মানসমূহ(data->sample, args, n);
	apply_fontlist(data->sample);
	XmTextSetString(data->name, nomatch_বার্তা);
	XmStringFree(str);

	প্রত্যাগম False;
    }
}

    স্থির শূন্য
encoding_callback(Widget w,
	SharedFontSelData *data,
	XtPointer dummy UNUSED)
{
    XmString str;
    XmListCallbackStruct fake_data;

    Xt_Va_Get_মানসমূহ(w, XmNlabelString, &str, রিক্ত);

    যদি (!str)
	প্রত্যাগম;

    fake_data.item = str;

    do_choice(0, data, &fake_data, ENCODING);
}

    স্থির শূন্য
name_callback(Widget w,
	SharedFontSelData *data,
	XmListCallbackStruct *call_data)
{
    do_choice(w, data, call_data, NAME);
}

    স্থির শূন্য
style_callback(Widget w,
	SharedFontSelData *data,
	XmListCallbackStruct *call_data)
{
    do_choice(w, data, call_data, STYLE);
}

    স্থির শূন্য
size_callback(Widget w,
	SharedFontSelData *data,
	XmListCallbackStruct *call_data)
{
    do_choice(w, data, call_data, SIZE);
}

    স্থির শূন্য
cancel_callback(Widget w UNUSED,
	SharedFontSelData *data,
	XmListCallbackStruct *call_data UNUSED)
{
    যদি (data->sel[ENCODING])
    {
	XtFree(data->sel[ENCODING]);
	data->sel[ENCODING] = রিক্ত;
    }
    যদি (data->sel[NAME])
    {
	XtFree(data->sel[NAME]);
	data->sel[NAME] = রিক্ত;
    }
    যদি (data->sel[STYLE])
    {
	XtFree(data->sel[STYLE]);
	data->sel[STYLE] = রিক্ত;
    }
    যদি (data->sel[SIZE])
    {
	XtFree(data->sel[SIZE]);
	data->sel[SIZE] = রিক্ত;
    }

    যদি (data->font_name)
	XtFree(data->font_name);
    data->font_name = রিক্ত;

    data->অঙ্ক = 0;
    XFreeFontNames(data->names);
    data->names = রিক্ত;
    data->exit = True;
}

    স্থির শূন্য
ok_callback(Widget w UNUSED,
	SharedFontSelData *data,
	Xm_ঠেলো_Button_Callback_Struct *call_data UNUSED)
{
    বর্ণ    *pattern;
    বর্ণ    **name;
    পূর্ণাঙ্ক	    i;

    pattern = XmTextGetString(data->name);
    name    = XListFonts(XtDisplay(data->dialog), pattern, 1, &i);
    XtFree(pattern);

    যদি (i != 1)
    {
	do_dialog(VIM_ERROR,
		(char_u *)_("Error"),
		(char_u *)_("Invalid font specification"),
		(char_u *)_("&Dismiss"), 1, রিক্ত, FALSE);
	XFreeFontNames(name);
    }
    অন্যথা
    {
	যদি (data->font_name)
	    XtFree(data->font_name);
	data->font_name = XtNewString(name[0]);

	যদি (data->sel[ENCODING])
	{
	    XtFree(data->sel[ENCODING]);
	    data->sel[ENCODING] = রিক্ত;
	}
	যদি (data->sel[NAME])
	{
	    XtFree(data->sel[NAME]);
	    data->sel[NAME] = রিক্ত;
	}
	যদি (data->sel[STYLE])
	{
	    XtFree(data->sel[STYLE]);
	    data->sel[STYLE] = রিক্ত;
	}
	যদি (data->sel[SIZE])
	{
	    XtFree(data->sel[SIZE]);
	    data->sel[SIZE] = রিক্ত;
	}

	XFreeFontNames(name);

	data->অঙ্ক = 0;
	XFreeFontNames(data->names);
	data->names = রিক্ত;
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
    স্থির SharedFontSelData	_data;

    Widget		parent;
    Widget		form;
    Widget		separator;
    Widget		sub_form;
    Widget		size_toggle;
    Widget		name;
    Widget		disp_frame;
    Widget		frame;
    Arg			args[64];
    পূর্ণাঙ্ক			n;
    XmString		str;
    বর্ণ		big_font[MAX_FONT_NAME_LEN];
    SharedFontSelData	*data;

    data = &_data;

    parent = vimShell;
    data->names = XListFonts(XtDisplay(parent), "-*-*-*-*-*-*-*-*-*-*-*-*-*-*",
						       MAX_FONTS, &data->অঙ্ক);

    /*
     * Find the name of the biggest font less than the given limit
     * MAX_DISPLAY_SIZE used to set up the initial height of the display
     * widget.
     */

    {
	পূর্ণাঙ্ক	i;
	পূর্ণাঙ্ক	max;
	পূর্ণাঙ্ক	idx = 0;
	পূর্ণাঙ্ক	size;
	বর্ণ	buf[128];

	প্রতি (i = 0, max = 0; i < data->অঙ্ক; i++)
	{
	    get_part(fn(data, i), 7, buf);
	    size = atoi(buf);
	    যদি ((size > max) && (size < MAX_DISPLAY_SIZE))
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
    data->dialog = XmCreateDialogShell(parent, "fontSelector", রিক্ত, 0);
    str = XmStringCreateLocalized(_("Vim - Font Selector"));

    // Create form popup dialog widget.
    form = XtVaCreateWidget("form",
	    xmFormWidgetClass, data->dialog,
	    XmNdialogTitle, str,
	    XmNautoUnmanage, False,
	    XmNdialogStyle, XmDIALOG_FULL_APPLICATION_MODAL,
	    রিক্ত);
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
	    রিক্ত);

    data->ok = XtVaCreateManagedWidget(_("OK"),
	    xm_ঠেলো_Button_Gadget_Class, sub_form,
	    XmNleftAttachment, XmATTACH_FORM,
	    XmNrightAttachment, XmATTACH_FORM,
	    XmNtopAttachment, XmATTACH_FORM,
	    XmNtopOffset, 4,
	    রিক্ত);
    apply_fontlist(data->ok);

    data->cancel = XtVaCreateManagedWidget(_("Cancel"),
	    xm_ঠেলো_Button_Gadget_Class, sub_form,
	    XmNrightAttachment, XmATTACH_FORM,
	    XmNleftAttachment, XmATTACH_FORM,
	    XmNtopAttachment, XmATTACH_WIDGET,
	    XmNtopWidget, data->ok,
	    XmNtopOffset, 4,
	    XmNshowAsDefault, True,
	    রিক্ত);
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
	    XmNeditMode, Xm_SINGLE_সারি_EDIT,
	    XmNmaxLength, MAX_FONT_NAME_LEN,
	    XmNcolumns, 60,
	    রিক্ত);

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
	    রিক্ত);
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
	    রিক্ত);

    data->sample = XtVaCreateManagedWidget("sampleLabel",
	    xmLabelWidgetClass, disp_frame,
	    XmNleftAttachment, XmATTACH_FORM,
	    XmNtopAttachment, XmATTACH_FORM,
	    XmNbottomAttachment, XmATTACH_FORM,
	    XmNrightAttachment, XmATTACH_FORM,
	    XmNalignment, XmALIGNMENT_BEGINNING,
	    XmNrecomputeSize, False,
	    XmNfontList, data->old_list,
	    রিক্ত);

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
	    রিক্ত);
    XmStringFree(str);
    apply_fontlist(size_toggle);
    XtManageChild(size_toggle);

    // Encoding pulldown menu.

    data->encoding_pulldown = XmCreatePulldownMenu(form,
						 "encodingPulldown", রিক্ত, 0);
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
	    রিক্ত);

    // font list
    frame = XtVaCreateManagedWidget("frame", xmFrameWidgetClass, sub_form,
	    XmNshadowThickness, 0,
	    XmNtopAttachment, XmATTACH_FORM,
	    XmNbottomAttachment, XmATTACH_FORM,
	    XmNleftAttachment, XmATTACH_FORM,
	    XmNrightAttachment, XmATTACH_POSITION,
	    XmNrightPosition, 50,
	    রিক্ত);

    str = XmStringCreateLocalized(_("Font:"));
    name = XtVaCreateManagedWidget("nameListLabel", xmLabelGadgetClass, frame,
	    XmNchildType, XmFRAME_TITLE_CHILD,
	    XmNchildVerticalAlignment, XmALIGNMENT_CENTER,
	    XmNchildHorizontalAlignment, XmALIGNMENT_BEGINNING,
	    XmNlabelString, str,
	    রিক্ত);
    XmStringFree(str);
    apply_fontlist(name);

    n = 0;
    XtSetArg(args[n], Xm_Nvisible_Item_গুনতি, 8); ++n;
    XtSetArg(args[n], XmNresizable, True); ++n;
    XtSetArg(args[n], XmNlistSizePolicy, XmCONSTANT); ++n;
    XtSetArg(args[n], XmNvisualPolicy, XmVARIABLE); ++n;
#ifdef LESSTIF_VERSION
    XtSetArg(args[n], XmNscrollBarDisplayPolicy, XmSTATIC); ++n;
#endif
    data->list[NAME] = XmCreateScrolledList(frame, "fontList", args, n);
    Xt_Va_Set_মানসমূহ(name, XmNuserData, data->list[NAME], রিক্ত);

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
	    রিক্ত);

    str = XmStringCreateLocalized(_("Style:"));
    name = XtVaCreateManagedWidget("styleListLabel", xmLabelWidgetClass, frame,
	    XmNchildType, XmFRAME_TITLE_CHILD,
	    XmNchildVerticalAlignment, XmALIGNMENT_CENTER,
	    XmNchildHorizontalAlignment, XmALIGNMENT_BEGINNING,
	    XmNlabelString, str,
	    রিক্ত);
    XmStringFree(str);
    apply_fontlist(name);

    n = 0;
    XtSetArg(args[n], Xm_Nvisible_Item_গুনতি, 8); ++n;
    XtSetArg(args[n], XmNresizable, True); ++n;
    XtSetArg(args[n], XmNlistSizePolicy, XmCONSTANT); ++n;
    XtSetArg(args[n], XmNvisualPolicy, XmVARIABLE); ++n;
#ifdef LESSTIF_VERSION
    XtSetArg(args[n], XmNscrollBarDisplayPolicy, XmSTATIC); ++n;
#endif
    data->list[STYLE] = XmCreateScrolledList(frame, "styleList", args, n);
    Xt_Va_Set_মানসমূহ(name, XmNuserData, data->list[STYLE], রিক্ত);

    // size list
    frame = XtVaCreateManagedWidget("frame", xmFrameWidgetClass, sub_form,
	    XmNshadowThickness, 0,
	    XmNtopAttachment, XmATTACH_FORM,
	    XmNbottomAttachment, XmATTACH_FORM,
	    XmNleftAttachment, XmATTACH_POSITION,
	    XmNleftPosition, 80,
	    XmNleftOffset, 4,
	    XmNrightAttachment, XmATTACH_FORM,
	    রিক্ত);

    str = XmStringCreateLocalized(_("Size:"));
    name = XtVaCreateManagedWidget("sizeListLabel", xmLabelGadgetClass, frame,
	    XmNchildType, XmFRAME_TITLE_CHILD,
	    XmNchildVerticalAlignment, XmALIGNMENT_CENTER,
	    XmNchildHorizontalAlignment, XmALIGNMENT_BEGINNING,
	    XmNlabelString, str,
	    রিক্ত);
    XmStringFree(str);
    apply_fontlist(name);

    n = 0;
    XtSetArg(args[n], Xm_Nvisible_Item_গুনতি, 8); ++n;
    XtSetArg(args[n], XmNresizable, True); ++n;
    XtSetArg(args[n], XmNlistSizePolicy, XmCONSTANT); ++n;
    XtSetArg(args[n], XmNvisualPolicy, XmVARIABLE); ++n;
#ifdef LESSTIF_VERSION
    XtSetArg(args[n], XmNscrollBarDisplayPolicy, XmSTATIC); ++n;
#endif
    data->list[SIZE] = XmCreateScrolledList(frame, "sizeList", args, n);
    Xt_Va_Set_মানসমূহ(name, XmNuserData, data->list[SIZE], রিক্ত);

    // update form widgets cancel button
    Xt_Va_Set_মানসমূহ(form, XmNcancelButton, data->cancel, রিক্ত);

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
    data->sel[ENCODING] = রিক্ত;
    data->sel[NAME] = রিক্ত;
    data->sel[STYLE] = রিক্ত;
    data->sel[SIZE] = রিক্ত;
    data->font_name = রিক্ত;

    // set up current font parameters
    যদি (current && current[0] != '\0')
    {
	পূর্ণাঙ্ক	    i;
	বর্ণ	    **names;

	names = XListFonts(XtDisplay(form), (বর্ণ *) current, 1, &i);

	যদি (i != 0)
	{
	    বর্ণ namebuf[TEMP_BUF_SIZE];
	    বর্ণ stylebuf[TEMP_BUF_SIZE];
	    বর্ণ sizebuf[TEMP_BUF_SIZE];
	    বর্ণ encodingbuf[TEMP_BUF_SIZE];
	    বর্ণ *found;

	    found = names[0];

	    name_part(found, namebuf);
	    style_part(found, stylebuf);
	    size_part(found, sizebuf, data->in_pixels);
	    encoding_part(found, encodingbuf);

	    যদি (*namebuf != NUL
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
	    অন্যথা
	    {
		// We can't preset a symbolic name, which isn't a full font
		// description. Therefore we just behave the same way as if the
		// user didn't have selected anything thus far.
		//
		// Unfortunately there is no known way to expand an abbreviated
		// font name.
		data->font_name = রিক্ত;
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
    যাবত্ (!data->exit)
	XtAppProcessEvent(XtWidgetToApplicationContext(data->dialog),
							(Xt_প্রবেশ_Mask)XtIMAll);

    যদি (data->old)
    {
	XFreeFont(XtDisplay(data->dialog),  data->old);
	XmFontListFree(data->old_list);
    }
    XtDestroyWidget(data->dialog);

    gui_motif_synch_fonts();

    প্রত্যাগম (char_u *) data->font_name;
}
