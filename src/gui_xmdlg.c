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

#அந்தர்பாவ "vim.h"

#அந்தர்பாவ <Xm/Form.h>
#அந்தர்பாவ <Xm/PushBG.h>
#அந்தர்பாவ <Xm/Text.h>
#அந்தர்பாவ <Xm/TextF.h>
#அந்தர்பாவ <Xm/Label.h>
#அந்தர்பாவ <Xm/Frame.h>
#அந்தர்பாவ <Xm/LabelG.h>
#அந்தர்பாவ <Xm/ToggleBG.h>
#அந்தர்பாவ <Xm/SeparatoG.h>
#அந்தர்பாவ <Xm/DialogS.h>
#அந்தர்பாவ <Xm/List.h>
#அந்தர்பாவ <Xm/RowColumn.h>
#அந்தர்பாவ <Xm/AtomMgr.h>
#அந்தர்பாவ <Xm/Protocols.h>

#அந்தர்பாவ <X11/keysym.h>
#அந்தர்பாவ <X11/Xatom.h>
#அந்தர்பாவ <X11/StringDefs.h>
#அந்தர்பாவ <X11/Intrinsic.h>

வெளி Widget vimShell;

#ifdef FEAT_MENU
# define apply_fontlist(w) gui_motif_menu_fontlist(w)
#இல்லையெனில்
# define apply_fontlist(w)
#endif

/////////////////////////////////////////////////////////////////////////////
// Font selection dialogue implementation.

நிலையான எழுத்து wild[3] = "*";

/*
 * FIXME: This is a generic function, which should be used throughout the whole
 * application.
 *
 * Add close_callback, which will be called when the user selects close from
 * the window menu.  The close menu item usually activates f.kill which sends a
 * WM_DELETE_WINDOW protocol request for the window.
 */

    நிலையான வெறுமை
add_cancel_action(Widget shell, XtCallbackProc close_callback, வெறுமை *arg)
{
    நிலையான Atom wmp_atom = 0;
    நிலையான Atom dw_atom = 0;
    Display *display = XtDisplay(shell);

    // deactivate the built-in delete response of killing the application
    Xt_Va_Set_மதிப்புகள்(shell, XmNdeleteResponse, XmDO_NOTHING, வெற்று);

    // add a delete window protocol callback instead
    எனில் (!dw_atom)
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

எண்ணிக்கை ListSpecifier
{
    ENCODING,
    NAME,
    STYLE,
    SIZE,
    NONE
};

வகைவரை கட்டமைப்பு _SharedFontSelData
{
    Widget	dialog;
    Widget	ok;
    Widget	cancel;
    Widget	encoding_pulldown;
    Widget	encoding_menu;
    Widget	list[NONE];
    Widget	name;
    Widget	sample;
    எழுத்து	**names;	// font name array of arrays
    முழு		இலக்கம்;		// number of font names
    String	sel[NONE];	// selection category
    Boolean	in_pixels;	// toggle state - size in pixels
    எழுத்து	*font_name;	// current font name
    XFontStruct	*old;		// font data structure for sample display
    XmFontList	old_list;	// font data structure for sample display
    Boolean	exit;		// used for program exit control
} SharedFontSelData;

/*
 * Checking access to the font name array for validity.
 */
    நிலையான எழுத்து *
fn(SharedFontSelData *data, முழு i)
{
    // Assertion checks:
    எனில் (data->இலக்கம் < 0)
	abort();
    எனில் (i >= data->இலக்கம்)
	i = data->இலக்கம் - 1;
    எனில் (i < 0)
	i = 0;

    திரும்பு data->names[i];
}

/*
 * Get a specific substring from a font name.
 */
    நிலையான வெறுமை
get_part(எழுத்து *in, முழு pos, எழுத்து *out)
{
    முழு	i;
    முழு j;

    *out = '\0';

    க்கு (i = 0; (pos > 0) && (in[i] != '\0'); ++i)
	எனில் (in[i] == '-')
	    pos--;

    எனில் (in[i] == '\0')
	திரும்பு;

    க்கு (j = 0; (in[i] != '-') && (in[i] != '\0'); ++i, ++j)
	out[j] = in[i];
    out[j] = '\0';
}

/*
 * Given a font name this function returns the part used in the first
 * scroll list.
 */
    நிலையான வெறுமை
name_part(எழுத்து *font, எழுத்து *buf)
{
    எழுத்து    buf2[TEMP_BUF_SIZE];
    எழுத்து    buf3[TEMP_BUF_SIZE];

    get_part(font, 2, buf2);
    get_part(font, 1, buf3);

    எனில் (*buf3 != NUL)
	vim_snprintf(buf, TEMP_BUF_SIZE, "%s (%s)", buf2, buf3);
    இல்லையெனில்
	vim_snprintf(buf, TEMP_BUF_SIZE, "%s", buf2);
}

/*
 * Given a font name this function returns the part used in the second scroll list.
 */
    நிலையான வெறுமை
style_part(எழுத்து *font, எழுத்து *buf)
{
    எழுத்து    buf2[TEMP_BUF_SIZE];
    எழுத்து    buf3[TEMP_BUF_SIZE];

    get_part(font, 3, buf3);
    get_part(font, 5, buf2);

    எனில் (!strcmp(buf2, "normal") && !strcmp(buf2, "Normal")
						   && !strcmp(buf2, "NORMAL"))
	vim_snprintf(buf, TEMP_BUF_SIZE, "%s %s", buf3, buf2);
    இல்லையெனில்
	strcpy(buf, buf3);

    get_part(font, 6, buf2);

    எனில் (buf2[0] != '\0')
	vim_snprintf(buf3, TEMP_BUF_SIZE, "%s %s", buf, buf2);
    இல்லையெனில்
	strcpy(buf3, buf);

    get_part(font, 4, buf2);

    எனில் (!strcmp(buf2, "o") || !strcmp(buf2, "O"))
	vim_snprintf(buf, TEMP_BUF_SIZE, "%s oblique", buf3);
    இல்லையெனில் எனில் (!strcmp(buf2, "i") || !strcmp(buf2, "I"))
	vim_snprintf(buf, TEMP_BUF_SIZE, "%s italic", buf3);

    எனில் (!strcmp(buf, " "))
	strcpy(buf, "-");
}

/*
 * Given a font name this function returns the part used in the third
 * scroll list.
 */
    நிலையான வெறுமை
size_part(எழுத்து *font, எழுத்து *buf, முழு inPixels)
{
    முழு	    size;
    தசம   temp;

    *buf = '\0';

    எனில் (inPixels)
    {
	get_part(font, 7, buf);
	எனில் (*buf != NUL)
	{
	    size = atoi(buf);
	    sprintf(buf, "%3d", size);
	}
    }
    இல்லையெனில்
    {
	get_part(font, 8, buf);
	எனில் (*buf != NUL)
	{
	    size = atoi(buf);
	    temp = (தசம)size / 10.0;
	    size = temp;
	    எனில் (buf[strlen(buf) - 1] == '0')
		sprintf(buf, "%3d", size);
	    இல்லையெனில்
		sprintf(buf, "%4.1f", temp);
	}
    }
}

/*
 * Given a font name this function returns the part used in the choice menu.
 */
    நிலையான வெறுமை
encoding_part(எழுத்து *font, எழுத்து *buf)
{
    எழுத்து    buf1[TEMP_BUF_SIZE];
    எழுத்து    buf2[TEMP_BUF_SIZE];

    *buf = '\0';

    get_part(font, 13, buf1);
    get_part(font, 14, buf2);

    எனில் (*buf1 != NUL && *buf2 != NUL)
	vim_snprintf(buf, TEMP_BUF_SIZE, "%s-%s", buf1, buf2);
    எனில் (!strcmp(buf, " "))
	strcpy(buf, "-");
}

/*
 * Inserts a string into correct sorted position in a list.
 */
    நிலையான வெறுமை
add_to_list(எழுத்து **buf, எழுத்து *item, முழு *மொத்தம்)
{
    முழு	i;
    முழு j;

    எனில் (*மொத்தம் == MAX_ENTRIES_IN_LIST)
	திரும்பு;

    // avoid duplication
    க்கு (i = 0; i < *மொத்தம்; ++i)
    {
	எனில் (!strcmp(buf[i], item))
	    திரும்பு;
    }

    // find order place, but make sure that wild card comes first
    எனில் (!strcmp(item, wild))
	i = 0;
    இல்லையெனில்
	க்கு (i = 0; i < *மொத்தம்; ++i)
	    எனில் (strcmp(buf[i], item) > 0 && strcmp(buf[i], wild))
		நிறுத்து;

    // now insert the item
    க்கு (j = *மொத்தம்; j > i; --j)
	buf[j] = buf[j-1];
    buf[i] = XtNewString(item);

    ++(*மொத்தம்);
}

/*
 * True if the font matches some field.
 */
    நிலையான Boolean
match(SharedFontSelData *data, எண்ணிக்கை ListSpecifier l, முழு i)
{
    எழுத்து buf[TEMP_BUF_SIZE];

    // An empty selection or a wild card matches anything.
    எனில் (!data->sel[l] || !strcmp(data->sel[l], wild))
	திரும்பு True;

    // chunk out the desired part...
    தேர்வு (l)
    {
	நிலை ENCODING:
	    encoding_part(fn(data, i), buf);
	    நிறுத்து;

	நிலை NAME:
	    name_part(fn(data, i), buf);
	    நிறுத்து;

	நிலை STYLE:
	    style_part(fn(data, i), buf);
	    நிறுத்து;

	நிலை SIZE:
	    size_part(fn(data, i), buf, data->in_pixels);
	    நிறுத்து;
	இயல்பு:
	    ;
    }

    // ...and chew it now

    திரும்பு !strcmp(buf, data->sel[l]);
}

    நிலையான Boolean
proportional(எழுத்து *font)
{
    எழுத்து buf[TEMP_BUF_SIZE];

    get_part(font, 11, buf);

    திரும்பு !strcmp(buf, "p") || !strcmp(buf, "P");
}


நிலையான வெறுமை encoding_callback(Widget w, SharedFontSelData *data,
							     XtPointer dummy);

/*
 * Parse through the fontlist data and set up the three scroll lists.  The fix
 * parameter can be used to exclude a list from any changes.  This is used for
 * updates after selections caused by the users actions.
 */
    நிலையான வெறுமை
fill_lists(எண்ணிக்கை ListSpecifier fix, SharedFontSelData *data)
{
    எழுத்து	*list[NONE][MAX_ENTRIES_IN_LIST];
    முழு		மொத்தம்[NONE];
    எழுத்து	buf[TEMP_BUF_SIZE];
    XmString	பொருட்கள்[MAX_ENTRIES_IN_LIST];
    முழு		i;
    முழு		idx;

    க்கு (idx = (முழு)ENCODING; idx < (முழு)NONE; ++idx)
	மொத்தம்[idx] = 0;

    // First we insert the wild char into every single list.
    எனில் (fix != ENCODING)
	add_to_list(list[ENCODING], wild, &மொத்தம்[ENCODING]);
    எனில் (fix != NAME)
	add_to_list(list[NAME], wild, &மொத்தம்[NAME]);
    எனில் (fix != STYLE)
	add_to_list(list[STYLE], wild, &மொத்தம்[STYLE]);
    எனில் (fix != SIZE)
	add_to_list(list[SIZE], wild, &மொத்தம்[SIZE]);

    க்கு (i = 0; i < data->இலக்கம் && i < MAX_ENTRIES_IN_LIST; i++)
    {
	எனில் (proportional(fn(data, i)))
	    தொடர்;

	எனில் (fix != ENCODING
		&& match(data, NAME, i)
		&& match(data, STYLE, i)
		&& match(data, SIZE, i))
	{
	    encoding_part(fn(data, i), buf);
	    add_to_list(list[ENCODING], buf, &மொத்தம்[ENCODING]);
	}

	எனில் (fix != NAME
		&& match(data, ENCODING, i)
		&& match(data, STYLE, i)
		&& match(data, SIZE, i))
	{
	    name_part(fn(data, i), buf);
	    add_to_list(list[NAME], buf, &மொத்தம்[NAME]);
	}

	எனில் (fix != STYLE
		&& match(data, ENCODING, i)
		&& match(data, NAME, i)
		&& match(data, SIZE, i))
	{
	    style_part(fn(data, i), buf);
	    add_to_list(list[STYLE], buf, &மொத்தம்[STYLE]);
	}

	எனில் (fix != SIZE
		&& match(data, ENCODING, i)
		&& match(data, NAME, i)
		&& match(data, STYLE, i))
	{
	    size_part(fn(data, i), buf, data->in_pixels);
	    add_to_list(list[SIZE], buf, &மொத்தம்[SIZE]);
	}
    }

    /*
     * And now do the preselection in all lists where there was one:
     */

    எனில் (fix != ENCODING)
    {
	Cardinal n_பொருட்கள்;
	WidgetList children;
	Widget selected_button = 0;

	// Get and update the current button list.
	Xt_Va_Get_மதிப்புகள்(data->encoding_pulldown,
		XmNchildren, &children,
		XmNnumChildren, &n_பொருட்கள்,
		வெற்று);

	க்கு (i = 0; i < மொத்தம்[ENCODING]; ++i)
	{
	    Widget button;

	    பொருட்கள்[i] = XmStringCreateLocalized(list[ENCODING][i]);

	    எனில் (i < (முழு)n_பொருட்கள்)
	    {
		// recycle old button
		Xt_Va_Set_மதிப்புகள்(children[i],
			XmNlabelString, பொருட்கள்[i],
			XmNuserData, i,
			வெற்று);
		button = children[i];
	    }
	    இல்லையெனில்
	    {
		// create a new button
		button = XtVaCreateManagedWidget("button",
			xm_தள்ளு_Button_Gadget_Class,
			data->encoding_pulldown,
			XmNlabelString, பொருட்கள்[i],
			XmNuserData, i,
			வெற்று);
		XtAddCallback(button, XmNactivateCallback,
			(XtCallbackProc) encoding_callback, (XtPointer) data);
		XtManageChild(button);
	    }

	    எனில் (data->sel[ENCODING])
	    {
		எனில் (!strcmp(data->sel[ENCODING], list[ENCODING][i]))
		    selected_button = button;
	    }
	    XtFree(list[ENCODING][i]);
	}

	// Destroy all the outstanding menu items.
	க்கு (i = மொத்தம்[ENCODING]; i < (முழு)n_பொருட்கள்; ++i)
	{
	    XtUnmanageChild(children[i]);
	    XtDestroyWidget(children[i]);
	}

	// Preserve the current selection visually.
	எனில் (selected_button)
	{
	    Xt_Va_Set_மதிப்புகள்(data->encoding_menu,
		    XmNmenuHistory, selected_button,
		    வெற்று);
	}

	க்கு (i = 0; i < மொத்தம்[ENCODING]; ++i)
	    XmStringFree(பொருட்கள்[i]);
    }

    /*
     * Now loop through the remaining lists and set them up.
     */
    க்கு (idx = (முழு)NAME; idx < (முழு)NONE; ++idx)
    {
	Widget w;

	எனில் (fix == (எண்ணிக்கை ListSpecifier)idx)
	    தொடர்;

	தேர்வு ((எண்ணிக்கை ListSpecifier)idx)
	{
	    நிலை NAME:
		w = data->list[NAME];
		நிறுத்து;
	    நிலை STYLE:
		w = data->list[STYLE];
		நிறுத்து;
	    நிலை SIZE:
		w = data->list[SIZE];
		நிறுத்து;
	    இயல்பு:
		w = (Widget)0;	// for lint
	}

	க்கு (i = 0; i < மொத்தம்[idx]; ++i)
	{
	    பொருட்கள்[i] = XmStringCreateLocalized(list[idx][i]);
	    XtFree(list[idx][i]);
	}
	Xm_List_Delete_All_பொருட்கள்(w);
	Xm_List_Add_பொருட்கள்(w, பொருட்கள், மொத்தம்[idx], 1);
	எனில் (data->sel[idx])
	{
	    XmStringFree(பொருட்கள்[0]);
	    பொருட்கள்[0] = XmStringCreateLocalized(data->sel[idx]);
	    XmListSelectItem(w, பொருட்கள்[0], False);
	    XmListSetBottomItem(w, பொருட்கள்[0]);
	}
	க்கு (i = 0; i < மொத்தம்[idx]; ++i)
	    XmStringFree(பொருட்கள்[i]);
    }
}

    நிலையான வெறுமை
stoggle_callback(Widget w UNUSED,
	SharedFontSelData *data,
	XmToggleButtonCallbackStruct *call_data)
{
    முழு		i, do_sel;
    எழுத்து	newSize[TEMP_BUF_SIZE];
    XmString	str;

    எனில் (call_data->reason != (முழு)Xm_CR_மதிப்பு_CHANGED)
	திரும்பு;

    do_sel = (data->sel[SIZE] != வெற்று) && strcmp(data->sel[SIZE], wild);

    க்கு (i = 0; do_sel && (i < data->இலக்கம்); i++)
	எனில் (match(data, ENCODING, i)
		&& match(data, NAME, i)
		&& match(data, STYLE, i)
		&& match(data, SIZE, i))
	{
	    size_part(fn(data, i), newSize, !data->in_pixels);
	    நிறுத்து;
	}

    data->in_pixels = !data->in_pixels;

    எனில் (data->sel[SIZE])
	XtFree(data->sel[SIZE]);
    data->sel[SIZE] = வெற்று;
    fill_lists(NONE, data);

    எனில் (do_sel)
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
    நிலையான வெறுமை
display_sample(SharedFontSelData *data)
{
    Arg		    args[2];
    முழு		    n;
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

    Xt_Set_மதிப்புகள்(data->sample, args, n);
    XmStringFree(str);

    எனில் (data->old)
    {
	XFreeFont(display, data->old);
	XmFontListFree(data->old_list);
    }
    data->old = font;
    data->old_list = font_list;
}


    நிலையான Boolean
do_choice(Widget w,
	SharedFontSelData *data,
	XmListCallbackStruct *call_data,
	எண்ணிக்கை ListSpecifier which)
{
    எழுத்து *sel;

    XmStringGetLtoR(call_data->item, XmSTRING_DEFAULT_CHARSET, &sel);

    எனில் (!data->sel[which])
	data->sel[which] = XtNewString(sel);
    இல்லையெனில்
    {
	எனில் (!strcmp(data->sel[which], sel))
	{
	    // unselecting current selection
	    XtFree(data->sel[which]);
	    data->sel[which] = வெற்று;
	    எனில் (w)
		XmListDeselectItem(w, call_data->item);
	}
	இல்லையெனில்
	{
	    XtFree(data->sel[which]);
	    data->sel[which] = XtNewString(sel);
	}
    }
    XtFree(sel);

    fill_lists(which, data);

    // If there is a font selection, we display it.
    எனில் (data->sel[ENCODING]
	    && data->sel[NAME]
	    && data->sel[STYLE]
	    && data->sel[SIZE]
	    && strcmp(data->sel[ENCODING], wild)
	    && strcmp(data->sel[NAME], wild)
	    && strcmp(data->sel[STYLE], wild)
	    && strcmp(data->sel[SIZE], wild))
    {
	முழு i;

	எனில் (data->font_name)
	    XtFree(data->font_name);
	data->font_name = வெற்று;

	க்கு (i = 0; i < data->இலக்கம்; i++)
	{
	    எனில் (match(data, ENCODING, i)
		    && match(data, NAME, i)
		    && match(data, STYLE, i)
		    && match(data, SIZE, i))
	    {
		data->font_name = XtNewString(fn(data, i));
		நிறுத்து;
	    }
	}

	எனில் (data->font_name)
	{
	    XmTextSetString(data->name, data->font_name);
	    display_sample(data);
	}
	இல்லையெனில்
	    do_dialog(VIM_ERROR,
		    (char_u *)_("Error"),
		    (char_u *)_("Invalid font specification"),
		    (char_u *)_("&Dismiss"), 1, வெற்று, FALSE);

	திரும்பு True;
    }
    இல்லையெனில்
    {
	முழு	    n;
	XmString    str;
	Arg	    args[4];
	எழுத்து	    *nomatch_செய்தி = _("no specific match");

	n = 0;
	str = XmStringCreateLocalized(nomatch_செய்தி);
	XtSetArg(args[n], XmNlabelString, str); ++n;
	Xt_Set_மதிப்புகள்(data->sample, args, n);
	apply_fontlist(data->sample);
	XmTextSetString(data->name, nomatch_செய்தி);
	XmStringFree(str);

	திரும்பு False;
    }
}

    நிலையான வெறுமை
encoding_callback(Widget w,
	SharedFontSelData *data,
	XtPointer dummy UNUSED)
{
    XmString str;
    XmListCallbackStruct fake_data;

    Xt_Va_Get_மதிப்புகள்(w, XmNlabelString, &str, வெற்று);

    எனில் (!str)
	திரும்பு;

    fake_data.item = str;

    do_choice(0, data, &fake_data, ENCODING);
}

    நிலையான வெறுமை
name_callback(Widget w,
	SharedFontSelData *data,
	XmListCallbackStruct *call_data)
{
    do_choice(w, data, call_data, NAME);
}

    நிலையான வெறுமை
style_callback(Widget w,
	SharedFontSelData *data,
	XmListCallbackStruct *call_data)
{
    do_choice(w, data, call_data, STYLE);
}

    நிலையான வெறுமை
size_callback(Widget w,
	SharedFontSelData *data,
	XmListCallbackStruct *call_data)
{
    do_choice(w, data, call_data, SIZE);
}

    நிலையான வெறுமை
cancel_callback(Widget w UNUSED,
	SharedFontSelData *data,
	XmListCallbackStruct *call_data UNUSED)
{
    எனில் (data->sel[ENCODING])
    {
	XtFree(data->sel[ENCODING]);
	data->sel[ENCODING] = வெற்று;
    }
    எனில் (data->sel[NAME])
    {
	XtFree(data->sel[NAME]);
	data->sel[NAME] = வெற்று;
    }
    எனில் (data->sel[STYLE])
    {
	XtFree(data->sel[STYLE]);
	data->sel[STYLE] = வெற்று;
    }
    எனில் (data->sel[SIZE])
    {
	XtFree(data->sel[SIZE]);
	data->sel[SIZE] = வெற்று;
    }

    எனில் (data->font_name)
	XtFree(data->font_name);
    data->font_name = வெற்று;

    data->இலக்கம் = 0;
    XFreeFontNames(data->names);
    data->names = வெற்று;
    data->exit = True;
}

    நிலையான வெறுமை
ok_callback(Widget w UNUSED,
	SharedFontSelData *data,
	Xm_தள்ளு_Button_Callback_Struct *call_data UNUSED)
{
    எழுத்து    *pattern;
    எழுத்து    **name;
    முழு	    i;

    pattern = XmTextGetString(data->name);
    name    = XListFonts(XtDisplay(data->dialog), pattern, 1, &i);
    XtFree(pattern);

    எனில் (i != 1)
    {
	do_dialog(VIM_ERROR,
		(char_u *)_("Error"),
		(char_u *)_("Invalid font specification"),
		(char_u *)_("&Dismiss"), 1, வெற்று, FALSE);
	XFreeFontNames(name);
    }
    இல்லையெனில்
    {
	எனில் (data->font_name)
	    XtFree(data->font_name);
	data->font_name = XtNewString(name[0]);

	எனில் (data->sel[ENCODING])
	{
	    XtFree(data->sel[ENCODING]);
	    data->sel[ENCODING] = வெற்று;
	}
	எனில் (data->sel[NAME])
	{
	    XtFree(data->sel[NAME]);
	    data->sel[NAME] = வெற்று;
	}
	எனில் (data->sel[STYLE])
	{
	    XtFree(data->sel[STYLE]);
	    data->sel[STYLE] = வெற்று;
	}
	எனில் (data->sel[SIZE])
	{
	    XtFree(data->sel[SIZE]);
	    data->sel[SIZE] = வெற்று;
	}

	XFreeFontNames(name);

	data->இலக்கம் = 0;
	XFreeFontNames(data->names);
	data->names = வெற்று;
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
    நிலையான SharedFontSelData	_data;

    Widget		parent;
    Widget		form;
    Widget		separator;
    Widget		sub_form;
    Widget		size_toggle;
    Widget		name;
    Widget		disp_frame;
    Widget		frame;
    Arg			args[64];
    முழு			n;
    XmString		str;
    எழுத்து		big_font[MAX_FONT_NAME_LEN];
    SharedFontSelData	*data;

    data = &_data;

    parent = vimShell;
    data->names = XListFonts(XtDisplay(parent), "-*-*-*-*-*-*-*-*-*-*-*-*-*-*",
						       MAX_FONTS, &data->இலக்கம்);

    /*
     * Find the name of the biggest font less than the given limit
     * MAX_DISPLAY_SIZE used to set up the initial height of the display
     * widget.
     */

    {
	முழு	i;
	முழு	max;
	முழு	idx = 0;
	முழு	size;
	எழுத்து	buf[128];

	க்கு (i = 0, max = 0; i < data->இலக்கம்; i++)
	{
	    get_part(fn(data, i), 7, buf);
	    size = atoi(buf);
	    எனில் ((size > max) && (size < MAX_DISPLAY_SIZE))
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
    data->dialog = XmCreateDialogShell(parent, "fontSelector", வெற்று, 0);
    str = XmStringCreateLocalized(_("Vim - Font Selector"));

    // Create form popup dialog widget.
    form = XtVaCreateWidget("form",
	    xmFormWidgetClass, data->dialog,
	    XmNdialogTitle, str,
	    XmNautoUnmanage, False,
	    XmNdialogStyle, XmDIALOG_FULL_APPLICATION_MODAL,
	    வெற்று);
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
	    வெற்று);

    data->ok = XtVaCreateManagedWidget(_("OK"),
	    xm_தள்ளு_Button_Gadget_Class, sub_form,
	    XmNleftAttachment, XmATTACH_FORM,
	    XmNrightAttachment, XmATTACH_FORM,
	    XmNtopAttachment, XmATTACH_FORM,
	    XmNtopOffset, 4,
	    வெற்று);
    apply_fontlist(data->ok);

    data->cancel = XtVaCreateManagedWidget(_("Cancel"),
	    xm_தள்ளு_Button_Gadget_Class, sub_form,
	    XmNrightAttachment, XmATTACH_FORM,
	    XmNleftAttachment, XmATTACH_FORM,
	    XmNtopAttachment, XmATTACH_WIDGET,
	    XmNtopWidget, data->ok,
	    XmNtopOffset, 4,
	    XmNshowAsDefault, True,
	    வெற்று);
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
	    XmNeditMode, Xm_SINGLE_வரி_EDIT,
	    XmNmaxLength, MAX_FONT_NAME_LEN,
	    XmNcolumns, 60,
	    வெற்று);

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
	    வெற்று);
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
	    வெற்று);

    data->sample = XtVaCreateManagedWidget("sampleLabel",
	    xmLabelWidgetClass, disp_frame,
	    XmNleftAttachment, XmATTACH_FORM,
	    XmNtopAttachment, XmATTACH_FORM,
	    XmNbottomAttachment, XmATTACH_FORM,
	    XmNrightAttachment, XmATTACH_FORM,
	    XmNalignment, XmALIGNMENT_BEGINNING,
	    XmNrecomputeSize, False,
	    XmNfontList, data->old_list,
	    வெற்று);

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
	    வெற்று);
    XmStringFree(str);
    apply_fontlist(size_toggle);
    XtManageChild(size_toggle);

    // Encoding pulldown menu.

    data->encoding_pulldown = XmCreatePulldownMenu(form,
						 "encodingPulldown", வெற்று, 0);
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
	    வெற்று);

    // font list
    frame = XtVaCreateManagedWidget("frame", xmFrameWidgetClass, sub_form,
	    XmNshadowThickness, 0,
	    XmNtopAttachment, XmATTACH_FORM,
	    XmNbottomAttachment, XmATTACH_FORM,
	    XmNleftAttachment, XmATTACH_FORM,
	    XmNrightAttachment, XmATTACH_POSITION,
	    XmNrightPosition, 50,
	    வெற்று);

    str = XmStringCreateLocalized(_("Font:"));
    name = XtVaCreateManagedWidget("nameListLabel", xmLabelGadgetClass, frame,
	    XmNchildType, XmFRAME_TITLE_CHILD,
	    XmNchildVerticalAlignment, XmALIGNMENT_CENTER,
	    XmNchildHorizontalAlignment, XmALIGNMENT_BEGINNING,
	    XmNlabelString, str,
	    வெற்று);
    XmStringFree(str);
    apply_fontlist(name);

    n = 0;
    XtSetArg(args[n], Xm_Nvisible_Item_மொத்தம், 8); ++n;
    XtSetArg(args[n], XmNresizable, True); ++n;
    XtSetArg(args[n], XmNlistSizePolicy, XmCONSTANT); ++n;
    XtSetArg(args[n], XmNvisualPolicy, XmVARIABLE); ++n;
#ifdef LESSTIF_VERSION
    XtSetArg(args[n], XmNscrollBarDisplayPolicy, XmSTATIC); ++n;
#endif
    data->list[NAME] = XmCreateScrolledList(frame, "fontList", args, n);
    Xt_Va_Set_மதிப்புகள்(name, XmNuserData, data->list[NAME], வெற்று);

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
	    வெற்று);

    str = XmStringCreateLocalized(_("Style:"));
    name = XtVaCreateManagedWidget("styleListLabel", xmLabelWidgetClass, frame,
	    XmNchildType, XmFRAME_TITLE_CHILD,
	    XmNchildVerticalAlignment, XmALIGNMENT_CENTER,
	    XmNchildHorizontalAlignment, XmALIGNMENT_BEGINNING,
	    XmNlabelString, str,
	    வெற்று);
    XmStringFree(str);
    apply_fontlist(name);

    n = 0;
    XtSetArg(args[n], Xm_Nvisible_Item_மொத்தம், 8); ++n;
    XtSetArg(args[n], XmNresizable, True); ++n;
    XtSetArg(args[n], XmNlistSizePolicy, XmCONSTANT); ++n;
    XtSetArg(args[n], XmNvisualPolicy, XmVARIABLE); ++n;
#ifdef LESSTIF_VERSION
    XtSetArg(args[n], XmNscrollBarDisplayPolicy, XmSTATIC); ++n;
#endif
    data->list[STYLE] = XmCreateScrolledList(frame, "styleList", args, n);
    Xt_Va_Set_மதிப்புகள்(name, XmNuserData, data->list[STYLE], வெற்று);

    // size list
    frame = XtVaCreateManagedWidget("frame", xmFrameWidgetClass, sub_form,
	    XmNshadowThickness, 0,
	    XmNtopAttachment, XmATTACH_FORM,
	    XmNbottomAttachment, XmATTACH_FORM,
	    XmNleftAttachment, XmATTACH_POSITION,
	    XmNleftPosition, 80,
	    XmNleftOffset, 4,
	    XmNrightAttachment, XmATTACH_FORM,
	    வெற்று);

    str = XmStringCreateLocalized(_("Size:"));
    name = XtVaCreateManagedWidget("sizeListLabel", xmLabelGadgetClass, frame,
	    XmNchildType, XmFRAME_TITLE_CHILD,
	    XmNchildVerticalAlignment, XmALIGNMENT_CENTER,
	    XmNchildHorizontalAlignment, XmALIGNMENT_BEGINNING,
	    XmNlabelString, str,
	    வெற்று);
    XmStringFree(str);
    apply_fontlist(name);

    n = 0;
    XtSetArg(args[n], Xm_Nvisible_Item_மொத்தம், 8); ++n;
    XtSetArg(args[n], XmNresizable, True); ++n;
    XtSetArg(args[n], XmNlistSizePolicy, XmCONSTANT); ++n;
    XtSetArg(args[n], XmNvisualPolicy, XmVARIABLE); ++n;
#ifdef LESSTIF_VERSION
    XtSetArg(args[n], XmNscrollBarDisplayPolicy, XmSTATIC); ++n;
#endif
    data->list[SIZE] = XmCreateScrolledList(frame, "sizeList", args, n);
    Xt_Va_Set_மதிப்புகள்(name, XmNuserData, data->list[SIZE], வெற்று);

    // update form widgets cancel button
    Xt_Va_Set_மதிப்புகள்(form, XmNcancelButton, data->cancel, வெற்று);

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
    data->sel[ENCODING] = வெற்று;
    data->sel[NAME] = வெற்று;
    data->sel[STYLE] = வெற்று;
    data->sel[SIZE] = வெற்று;
    data->font_name = வெற்று;

    // set up current font parameters
    எனில் (current && current[0] != '\0')
    {
	முழு	    i;
	எழுத்து	    **names;

	names = XListFonts(XtDisplay(form), (எழுத்து *) current, 1, &i);

	எனில் (i != 0)
	{
	    எழுத்து namebuf[TEMP_BUF_SIZE];
	    எழுத்து stylebuf[TEMP_BUF_SIZE];
	    எழுத்து sizebuf[TEMP_BUF_SIZE];
	    எழுத்து encodingbuf[TEMP_BUF_SIZE];
	    எழுத்து *found;

	    found = names[0];

	    name_part(found, namebuf);
	    style_part(found, stylebuf);
	    size_part(found, sizebuf, data->in_pixels);
	    encoding_part(found, encodingbuf);

	    எனில் (*namebuf != NUL
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
	    இல்லையெனில்
	    {
		// We can't preset a symbolic name, which isn't a full font
		// description. Therefore we just behave the same way as if the
		// user didn't have selected anything thus far.
		//
		// Unfortunately there is no known way to expand an abbreviated
		// font name.
		data->font_name = வெற்று;
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
    போது (!data->exit)
	XtAppProcessEvent(XtWidgetToApplicationContext(data->dialog),
							(Xt_உள்ளீடு_Mask)XtIMAll);

    எனில் (data->old)
    {
	XFreeFont(XtDisplay(data->dialog),  data->old);
	XmFontListFree(data->old_list);
    }
    XtDestroyWidget(data->dialog);

    gui_motif_synch_fonts();

    திரும்பு (char_u *) data->font_name;
}
