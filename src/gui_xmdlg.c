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

#അന്തര്ഭാവ "vim.h"

#അന്തര്ഭാവ <Xm/Form.h>
#അന്തര്ഭാവ <Xm/PushBG.h>
#അന്തര്ഭാവ <Xm/Text.h>
#അന്തര്ഭാവ <Xm/TextF.h>
#അന്തര്ഭാവ <Xm/Label.h>
#അന്തര്ഭാവ <Xm/Frame.h>
#അന്തര്ഭാവ <Xm/LabelG.h>
#അന്തര്ഭാവ <Xm/ToggleBG.h>
#അന്തര്ഭാവ <Xm/SeparatoG.h>
#അന്തര്ഭാവ <Xm/DialogS.h>
#അന്തര്ഭാവ <Xm/List.h>
#അന്തര്ഭാവ <Xm/RowColumn.h>
#അന്തര്ഭാവ <Xm/AtomMgr.h>
#അന്തര്ഭാവ <Xm/Protocols.h>

#അന്തര്ഭാവ <X11/keysym.h>
#അന്തര്ഭാവ <X11/Xatom.h>
#അന്തര്ഭാവ <X11/StringDefs.h>
#അന്തര്ഭാവ <X11/Intrinsic.h>

ബാഹ്യ Widget vimShell;

#ifdef FEAT_MENU
# define apply_fontlist(w) gui_motif_menu_fontlist(w)
#അല്ലെങ്കിൽ
# define apply_fontlist(w)
#endif

/////////////////////////////////////////////////////////////////////////////
// Font selection dialogue implementation.

സ്ഥിതം അക്ഷരം wild[3] = "*";

/*
 * FIXME: This is a generic function, which should be used throughout the whole
 * application.
 *
 * Add close_callback, which will be called when the user selects close from
 * the window menu.  The close menu item usually activates f.kill which sends a
 * WM_DELETE_WINDOW protocol request for the window.
 */

    സ്ഥിതം ശൂന്യം
add_cancel_action(Widget shell, XtCallbackProc close_callback, ശൂന്യം *arg)
{
    സ്ഥിതം Atom wmp_atom = 0;
    സ്ഥിതം Atom dw_atom = 0;
    Display *display = XtDisplay(shell);

    // deactivate the built-in delete response of killing the application
    Xt_Va_Set_മൂല്യങ്ങൾ(shell, XmNdeleteResponse, XmDO_NOTHING, ശൂന്യ);

    // add a delete window protocol callback instead
    എങ്കിൽ (!dw_atom)
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

എണ്ണൽ ListSpecifier
{
    ENCODING,
    NAME,
    STYLE,
    SIZE,
    NONE
};

തരനിർവചനം ഘടന _SharedFontSelData
{
    Widget	dialog;
    Widget	ok;
    Widget	cancel;
    Widget	encoding_pulldown;
    Widget	encoding_menu;
    Widget	list[NONE];
    Widget	name;
    Widget	sample;
    അക്ഷരം	**names;	// font name array of arrays
    പൂർണ്ണസംഖ്യ		അക്കം;		// number of font names
    String	sel[NONE];	// selection category
    Boolean	in_pixels;	// toggle state - size in pixels
    അക്ഷരം	*font_name;	// current font name
    XFontStruct	*old;		// font data structure for sample display
    XmFontList	old_list;	// font data structure for sample display
    Boolean	exit;		// used for program exit control
} SharedFontSelData;

/*
 * Checking access to the font name array for validity.
 */
    സ്ഥിതം അക്ഷരം *
fn(SharedFontSelData *data, പൂർണ്ണസംഖ്യ i)
{
    // Assertion checks:
    എങ്കിൽ (data->അക്കം < 0)
	abort();
    എങ്കിൽ (i >= data->അക്കം)
	i = data->അക്കം - 1;
    എങ്കിൽ (i < 0)
	i = 0;

    തിരികെ data->names[i];
}

/*
 * Get a specific substring from a font name.
 */
    സ്ഥിതം ശൂന്യം
get_part(അക്ഷരം *in, പൂർണ്ണസംഖ്യ pos, അക്ഷരം *out)
{
    പൂർണ്ണസംഖ്യ	i;
    പൂർണ്ണസംഖ്യ j;

    *out = '\0';

    വേണ്ടി (i = 0; (pos > 0) && (in[i] != '\0'); ++i)
	എങ്കിൽ (in[i] == '-')
	    pos--;

    എങ്കിൽ (in[i] == '\0')
	തിരികെ;

    വേണ്ടി (j = 0; (in[i] != '-') && (in[i] != '\0'); ++i, ++j)
	out[j] = in[i];
    out[j] = '\0';
}

/*
 * Given a font name this function returns the part used in the first
 * scroll list.
 */
    സ്ഥിതം ശൂന്യം
name_part(അക്ഷരം *font, അക്ഷരം *buf)
{
    അക്ഷരം    buf2[TEMP_BUF_SIZE];
    അക്ഷരം    buf3[TEMP_BUF_SIZE];

    get_part(font, 2, buf2);
    get_part(font, 1, buf3);

    എങ്കിൽ (*buf3 != NUL)
	vim_snprintf(buf, TEMP_BUF_SIZE, "%s (%s)", buf2, buf3);
    അല്ലെങ്കിൽ
	vim_snprintf(buf, TEMP_BUF_SIZE, "%s", buf2);
}

/*
 * Given a font name this function returns the part used in the second scroll list.
 */
    സ്ഥിതം ശൂന്യം
style_part(അക്ഷരം *font, അക്ഷരം *buf)
{
    അക്ഷരം    buf2[TEMP_BUF_SIZE];
    അക്ഷരം    buf3[TEMP_BUF_SIZE];

    get_part(font, 3, buf3);
    get_part(font, 5, buf2);

    എങ്കിൽ (!strcmp(buf2, "normal") && !strcmp(buf2, "Normal")
						   && !strcmp(buf2, "NORMAL"))
	vim_snprintf(buf, TEMP_BUF_SIZE, "%s %s", buf3, buf2);
    അല്ലെങ്കിൽ
	strcpy(buf, buf3);

    get_part(font, 6, buf2);

    എങ്കിൽ (buf2[0] != '\0')
	vim_snprintf(buf3, TEMP_BUF_SIZE, "%s %s", buf, buf2);
    അല്ലെങ്കിൽ
	strcpy(buf3, buf);

    get_part(font, 4, buf2);

    എങ്കിൽ (!strcmp(buf2, "o") || !strcmp(buf2, "O"))
	vim_snprintf(buf, TEMP_BUF_SIZE, "%s oblique", buf3);
    അല്ലെങ്കിൽ എങ്കിൽ (!strcmp(buf2, "i") || !strcmp(buf2, "I"))
	vim_snprintf(buf, TEMP_BUF_SIZE, "%s italic", buf3);

    എങ്കിൽ (!strcmp(buf, " "))
	strcpy(buf, "-");
}

/*
 * Given a font name this function returns the part used in the third
 * scroll list.
 */
    സ്ഥിതം ശൂന്യം
size_part(അക്ഷരം *font, അക്ഷരം *buf, പൂർണ്ണസംഖ്യ inPixels)
{
    പൂർണ്ണസംഖ്യ	    size;
    ദശാംശം   temp;

    *buf = '\0';

    എങ്കിൽ (inPixels)
    {
	get_part(font, 7, buf);
	എങ്കിൽ (*buf != NUL)
	{
	    size = atoi(buf);
	    sprintf(buf, "%3d", size);
	}
    }
    അല്ലെങ്കിൽ
    {
	get_part(font, 8, buf);
	എങ്കിൽ (*buf != NUL)
	{
	    size = atoi(buf);
	    temp = (ദശാംശം)size / 10.0;
	    size = temp;
	    എങ്കിൽ (buf[strlen(buf) - 1] == '0')
		sprintf(buf, "%3d", size);
	    അല്ലെങ്കിൽ
		sprintf(buf, "%4.1f", temp);
	}
    }
}

/*
 * Given a font name this function returns the part used in the choice menu.
 */
    സ്ഥിതം ശൂന്യം
encoding_part(അക്ഷരം *font, അക്ഷരം *buf)
{
    അക്ഷരം    buf1[TEMP_BUF_SIZE];
    അക്ഷരം    buf2[TEMP_BUF_SIZE];

    *buf = '\0';

    get_part(font, 13, buf1);
    get_part(font, 14, buf2);

    എങ്കിൽ (*buf1 != NUL && *buf2 != NUL)
	vim_snprintf(buf, TEMP_BUF_SIZE, "%s-%s", buf1, buf2);
    എങ്കിൽ (!strcmp(buf, " "))
	strcpy(buf, "-");
}

/*
 * Inserts a string into correct sorted position in a list.
 */
    സ്ഥിതം ശൂന്യം
add_to_list(അക്ഷരം **buf, അക്ഷരം *item, പൂർണ്ണസംഖ്യ *എണ്ണം)
{
    പൂർണ്ണസംഖ്യ	i;
    പൂർണ്ണസംഖ്യ j;

    എങ്കിൽ (*എണ്ണം == MAX_ENTRIES_IN_LIST)
	തിരികെ;

    // avoid duplication
    വേണ്ടി (i = 0; i < *എണ്ണം; ++i)
    {
	എങ്കിൽ (!strcmp(buf[i], item))
	    തിരികെ;
    }

    // find order place, but make sure that wild card comes first
    എങ്കിൽ (!strcmp(item, wild))
	i = 0;
    അല്ലെങ്കിൽ
	വേണ്ടി (i = 0; i < *എണ്ണം; ++i)
	    എങ്കിൽ (strcmp(buf[i], item) > 0 && strcmp(buf[i], wild))
		നിർത്തുക;

    // now insert the item
    വേണ്ടി (j = *എണ്ണം; j > i; --j)
	buf[j] = buf[j-1];
    buf[i] = XtNewString(item);

    ++(*എണ്ണം);
}

/*
 * True if the font matches some field.
 */
    സ്ഥിതം Boolean
match(SharedFontSelData *data, എണ്ണൽ ListSpecifier l, പൂർണ്ണസംഖ്യ i)
{
    അക്ഷരം buf[TEMP_BUF_SIZE];

    // An empty selection or a wild card matches anything.
    എങ്കിൽ (!data->sel[l] || !strcmp(data->sel[l], wild))
	തിരികെ True;

    // chunk out the desired part...
    തിരഞ്ഞെടുപ്പ് (l)
    {
	അവസ്ഥ ENCODING:
	    encoding_part(fn(data, i), buf);
	    നിർത്തുക;

	അവസ്ഥ NAME:
	    name_part(fn(data, i), buf);
	    നിർത്തുക;

	അവസ്ഥ STYLE:
	    style_part(fn(data, i), buf);
	    നിർത്തുക;

	അവസ്ഥ SIZE:
	    size_part(fn(data, i), buf, data->in_pixels);
	    നിർത്തുക;
	സ്ഥിരസ്ഥിതി:
	    ;
    }

    // ...and chew it now

    തിരികെ !strcmp(buf, data->sel[l]);
}

    സ്ഥിതം Boolean
proportional(അക്ഷരം *font)
{
    അക്ഷരം buf[TEMP_BUF_SIZE];

    get_part(font, 11, buf);

    തിരികെ !strcmp(buf, "p") || !strcmp(buf, "P");
}


സ്ഥിതം ശൂന്യം encoding_callback(Widget w, SharedFontSelData *data,
							     XtPointer dummy);

/*
 * Parse through the fontlist data and set up the three scroll lists.  The fix
 * parameter can be used to exclude a list from any changes.  This is used for
 * updates after selections caused by the users actions.
 */
    സ്ഥിതം ശൂന്യം
fill_lists(എണ്ണൽ ListSpecifier fix, SharedFontSelData *data)
{
    അക്ഷരം	*list[NONE][MAX_ENTRIES_IN_LIST];
    പൂർണ്ണസംഖ്യ		എണ്ണം[NONE];
    അക്ഷരം	buf[TEMP_BUF_SIZE];
    XmString	ഇനങ്ങൾ[MAX_ENTRIES_IN_LIST];
    പൂർണ്ണസംഖ്യ		i;
    പൂർണ്ണസംഖ്യ		idx;

    വേണ്ടി (idx = (പൂർണ്ണസംഖ്യ)ENCODING; idx < (പൂർണ്ണസംഖ്യ)NONE; ++idx)
	എണ്ണം[idx] = 0;

    // First we insert the wild char into every single list.
    എങ്കിൽ (fix != ENCODING)
	add_to_list(list[ENCODING], wild, &എണ്ണം[ENCODING]);
    എങ്കിൽ (fix != NAME)
	add_to_list(list[NAME], wild, &എണ്ണം[NAME]);
    എങ്കിൽ (fix != STYLE)
	add_to_list(list[STYLE], wild, &എണ്ണം[STYLE]);
    എങ്കിൽ (fix != SIZE)
	add_to_list(list[SIZE], wild, &എണ്ണം[SIZE]);

    വേണ്ടി (i = 0; i < data->അക്കം && i < MAX_ENTRIES_IN_LIST; i++)
    {
	എങ്കിൽ (proportional(fn(data, i)))
	    തുടരുക;

	എങ്കിൽ (fix != ENCODING
		&& match(data, NAME, i)
		&& match(data, STYLE, i)
		&& match(data, SIZE, i))
	{
	    encoding_part(fn(data, i), buf);
	    add_to_list(list[ENCODING], buf, &എണ്ണം[ENCODING]);
	}

	എങ്കിൽ (fix != NAME
		&& match(data, ENCODING, i)
		&& match(data, STYLE, i)
		&& match(data, SIZE, i))
	{
	    name_part(fn(data, i), buf);
	    add_to_list(list[NAME], buf, &എണ്ണം[NAME]);
	}

	എങ്കിൽ (fix != STYLE
		&& match(data, ENCODING, i)
		&& match(data, NAME, i)
		&& match(data, SIZE, i))
	{
	    style_part(fn(data, i), buf);
	    add_to_list(list[STYLE], buf, &എണ്ണം[STYLE]);
	}

	എങ്കിൽ (fix != SIZE
		&& match(data, ENCODING, i)
		&& match(data, NAME, i)
		&& match(data, STYLE, i))
	{
	    size_part(fn(data, i), buf, data->in_pixels);
	    add_to_list(list[SIZE], buf, &എണ്ണം[SIZE]);
	}
    }

    /*
     * And now do the preselection in all lists where there was one:
     */

    എങ്കിൽ (fix != ENCODING)
    {
	Cardinal n_ഇനങ്ങൾ;
	WidgetList children;
	Widget selected_button = 0;

	// Get and update the current button list.
	Xt_Va_Get_മൂല്യങ്ങൾ(data->encoding_pulldown,
		XmNchildren, &children,
		XmNnumChildren, &n_ഇനങ്ങൾ,
		ശൂന്യ);

	വേണ്ടി (i = 0; i < എണ്ണം[ENCODING]; ++i)
	{
	    Widget button;

	    ഇനങ്ങൾ[i] = XmStringCreateLocalized(list[ENCODING][i]);

	    എങ്കിൽ (i < (പൂർണ്ണസംഖ്യ)n_ഇനങ്ങൾ)
	    {
		// recycle old button
		Xt_Va_Set_മൂല്യങ്ങൾ(children[i],
			XmNlabelString, ഇനങ്ങൾ[i],
			XmNuserData, i,
			ശൂന്യ);
		button = children[i];
	    }
	    അല്ലെങ്കിൽ
	    {
		// create a new button
		button = XtVaCreateManagedWidget("button",
			xm_തള്ളുക_Button_Gadget_Class,
			data->encoding_pulldown,
			XmNlabelString, ഇനങ്ങൾ[i],
			XmNuserData, i,
			ശൂന്യ);
		XtAddCallback(button, XmNactivateCallback,
			(XtCallbackProc) encoding_callback, (XtPointer) data);
		XtManageChild(button);
	    }

	    എങ്കിൽ (data->sel[ENCODING])
	    {
		എങ്കിൽ (!strcmp(data->sel[ENCODING], list[ENCODING][i]))
		    selected_button = button;
	    }
	    XtFree(list[ENCODING][i]);
	}

	// Destroy all the outstanding menu items.
	വേണ്ടി (i = എണ്ണം[ENCODING]; i < (പൂർണ്ണസംഖ്യ)n_ഇനങ്ങൾ; ++i)
	{
	    XtUnmanageChild(children[i]);
	    XtDestroyWidget(children[i]);
	}

	// Preserve the current selection visually.
	എങ്കിൽ (selected_button)
	{
	    Xt_Va_Set_മൂല്യങ്ങൾ(data->encoding_menu,
		    XmNmenuHistory, selected_button,
		    ശൂന്യ);
	}

	വേണ്ടി (i = 0; i < എണ്ണം[ENCODING]; ++i)
	    XmStringFree(ഇനങ്ങൾ[i]);
    }

    /*
     * Now loop through the remaining lists and set them up.
     */
    വേണ്ടി (idx = (പൂർണ്ണസംഖ്യ)NAME; idx < (പൂർണ്ണസംഖ്യ)NONE; ++idx)
    {
	Widget w;

	എങ്കിൽ (fix == (എണ്ണൽ ListSpecifier)idx)
	    തുടരുക;

	തിരഞ്ഞെടുപ്പ് ((എണ്ണൽ ListSpecifier)idx)
	{
	    അവസ്ഥ NAME:
		w = data->list[NAME];
		നിർത്തുക;
	    അവസ്ഥ STYLE:
		w = data->list[STYLE];
		നിർത്തുക;
	    അവസ്ഥ SIZE:
		w = data->list[SIZE];
		നിർത്തുക;
	    സ്ഥിരസ്ഥിതി:
		w = (Widget)0;	// for lint
	}

	വേണ്ടി (i = 0; i < എണ്ണം[idx]; ++i)
	{
	    ഇനങ്ങൾ[i] = XmStringCreateLocalized(list[idx][i]);
	    XtFree(list[idx][i]);
	}
	Xm_List_Delete_All_ഇനങ്ങൾ(w);
	Xm_List_Add_ഇനങ്ങൾ(w, ഇനങ്ങൾ, എണ്ണം[idx], 1);
	എങ്കിൽ (data->sel[idx])
	{
	    XmStringFree(ഇനങ്ങൾ[0]);
	    ഇനങ്ങൾ[0] = XmStringCreateLocalized(data->sel[idx]);
	    XmListSelectItem(w, ഇനങ്ങൾ[0], False);
	    XmListSetBottomItem(w, ഇനങ്ങൾ[0]);
	}
	വേണ്ടി (i = 0; i < എണ്ണം[idx]; ++i)
	    XmStringFree(ഇനങ്ങൾ[i]);
    }
}

    സ്ഥിതം ശൂന്യം
stoggle_callback(Widget w UNUSED,
	SharedFontSelData *data,
	XmToggleButtonCallbackStruct *call_data)
{
    പൂർണ്ണസംഖ്യ		i, do_sel;
    അക്ഷരം	newSize[TEMP_BUF_SIZE];
    XmString	str;

    എങ്കിൽ (call_data->reason != (പൂർണ്ണസംഖ്യ)Xm_CR_മൂല്യം_CHANGED)
	തിരികെ;

    do_sel = (data->sel[SIZE] != ശൂന്യ) && strcmp(data->sel[SIZE], wild);

    വേണ്ടി (i = 0; do_sel && (i < data->അക്കം); i++)
	എങ്കിൽ (match(data, ENCODING, i)
		&& match(data, NAME, i)
		&& match(data, STYLE, i)
		&& match(data, SIZE, i))
	{
	    size_part(fn(data, i), newSize, !data->in_pixels);
	    നിർത്തുക;
	}

    data->in_pixels = !data->in_pixels;

    എങ്കിൽ (data->sel[SIZE])
	XtFree(data->sel[SIZE]);
    data->sel[SIZE] = ശൂന്യ;
    fill_lists(NONE, data);

    എങ്കിൽ (do_sel)
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
    സ്ഥിതം ശൂന്യം
display_sample(SharedFontSelData *data)
{
    Arg		    args[2];
    പൂർണ്ണസംഖ്യ		    n;
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

    Xt_Set_മൂല്യങ്ങൾ(data->sample, args, n);
    XmStringFree(str);

    എങ്കിൽ (data->old)
    {
	XFreeFont(display, data->old);
	XmFontListFree(data->old_list);
    }
    data->old = font;
    data->old_list = font_list;
}


    സ്ഥിതം Boolean
do_choice(Widget w,
	SharedFontSelData *data,
	XmListCallbackStruct *call_data,
	എണ്ണൽ ListSpecifier which)
{
    അക്ഷരം *sel;

    XmStringGetLtoR(call_data->item, XmSTRING_DEFAULT_CHARSET, &sel);

    എങ്കിൽ (!data->sel[which])
	data->sel[which] = XtNewString(sel);
    അല്ലെങ്കിൽ
    {
	എങ്കിൽ (!strcmp(data->sel[which], sel))
	{
	    // unselecting current selection
	    XtFree(data->sel[which]);
	    data->sel[which] = ശൂന്യ;
	    എങ്കിൽ (w)
		XmListDeselectItem(w, call_data->item);
	}
	അല്ലെങ്കിൽ
	{
	    XtFree(data->sel[which]);
	    data->sel[which] = XtNewString(sel);
	}
    }
    XtFree(sel);

    fill_lists(which, data);

    // If there is a font selection, we display it.
    എങ്കിൽ (data->sel[ENCODING]
	    && data->sel[NAME]
	    && data->sel[STYLE]
	    && data->sel[SIZE]
	    && strcmp(data->sel[ENCODING], wild)
	    && strcmp(data->sel[NAME], wild)
	    && strcmp(data->sel[STYLE], wild)
	    && strcmp(data->sel[SIZE], wild))
    {
	പൂർണ്ണസംഖ്യ i;

	എങ്കിൽ (data->font_name)
	    XtFree(data->font_name);
	data->font_name = ശൂന്യ;

	വേണ്ടി (i = 0; i < data->അക്കം; i++)
	{
	    എങ്കിൽ (match(data, ENCODING, i)
		    && match(data, NAME, i)
		    && match(data, STYLE, i)
		    && match(data, SIZE, i))
	    {
		data->font_name = XtNewString(fn(data, i));
		നിർത്തുക;
	    }
	}

	എങ്കിൽ (data->font_name)
	{
	    XmTextSetString(data->name, data->font_name);
	    display_sample(data);
	}
	അല്ലെങ്കിൽ
	    do_dialog(VIM_ERROR,
		    (char_u *)_("Error"),
		    (char_u *)_("Invalid font specification"),
		    (char_u *)_("&Dismiss"), 1, ശൂന്യ, FALSE);

	തിരികെ True;
    }
    അല്ലെങ്കിൽ
    {
	പൂർണ്ണസംഖ്യ	    n;
	XmString    str;
	Arg	    args[4];
	അക്ഷരം	    *nomatch_സന്ദേശം = _("no specific match");

	n = 0;
	str = XmStringCreateLocalized(nomatch_സന്ദേശം);
	XtSetArg(args[n], XmNlabelString, str); ++n;
	Xt_Set_മൂല്യങ്ങൾ(data->sample, args, n);
	apply_fontlist(data->sample);
	XmTextSetString(data->name, nomatch_സന്ദേശം);
	XmStringFree(str);

	തിരികെ False;
    }
}

    സ്ഥിതം ശൂന്യം
encoding_callback(Widget w,
	SharedFontSelData *data,
	XtPointer dummy UNUSED)
{
    XmString str;
    XmListCallbackStruct fake_data;

    Xt_Va_Get_മൂല്യങ്ങൾ(w, XmNlabelString, &str, ശൂന്യ);

    എങ്കിൽ (!str)
	തിരികെ;

    fake_data.item = str;

    do_choice(0, data, &fake_data, ENCODING);
}

    സ്ഥിതം ശൂന്യം
name_callback(Widget w,
	SharedFontSelData *data,
	XmListCallbackStruct *call_data)
{
    do_choice(w, data, call_data, NAME);
}

    സ്ഥിതം ശൂന്യം
style_callback(Widget w,
	SharedFontSelData *data,
	XmListCallbackStruct *call_data)
{
    do_choice(w, data, call_data, STYLE);
}

    സ്ഥിതം ശൂന്യം
size_callback(Widget w,
	SharedFontSelData *data,
	XmListCallbackStruct *call_data)
{
    do_choice(w, data, call_data, SIZE);
}

    സ്ഥിതം ശൂന്യം
cancel_callback(Widget w UNUSED,
	SharedFontSelData *data,
	XmListCallbackStruct *call_data UNUSED)
{
    എങ്കിൽ (data->sel[ENCODING])
    {
	XtFree(data->sel[ENCODING]);
	data->sel[ENCODING] = ശൂന്യ;
    }
    എങ്കിൽ (data->sel[NAME])
    {
	XtFree(data->sel[NAME]);
	data->sel[NAME] = ശൂന്യ;
    }
    എങ്കിൽ (data->sel[STYLE])
    {
	XtFree(data->sel[STYLE]);
	data->sel[STYLE] = ശൂന്യ;
    }
    എങ്കിൽ (data->sel[SIZE])
    {
	XtFree(data->sel[SIZE]);
	data->sel[SIZE] = ശൂന്യ;
    }

    എങ്കിൽ (data->font_name)
	XtFree(data->font_name);
    data->font_name = ശൂന്യ;

    data->അക്കം = 0;
    XFreeFontNames(data->names);
    data->names = ശൂന്യ;
    data->exit = True;
}

    സ്ഥിതം ശൂന്യം
ok_callback(Widget w UNUSED,
	SharedFontSelData *data,
	Xm_തള്ളുക_Button_Callback_Struct *call_data UNUSED)
{
    അക്ഷരം    *pattern;
    അക്ഷരം    **name;
    പൂർണ്ണസംഖ്യ	    i;

    pattern = XmTextGetString(data->name);
    name    = XListFonts(XtDisplay(data->dialog), pattern, 1, &i);
    XtFree(pattern);

    എങ്കിൽ (i != 1)
    {
	do_dialog(VIM_ERROR,
		(char_u *)_("Error"),
		(char_u *)_("Invalid font specification"),
		(char_u *)_("&Dismiss"), 1, ശൂന്യ, FALSE);
	XFreeFontNames(name);
    }
    അല്ലെങ്കിൽ
    {
	എങ്കിൽ (data->font_name)
	    XtFree(data->font_name);
	data->font_name = XtNewString(name[0]);

	എങ്കിൽ (data->sel[ENCODING])
	{
	    XtFree(data->sel[ENCODING]);
	    data->sel[ENCODING] = ശൂന്യ;
	}
	എങ്കിൽ (data->sel[NAME])
	{
	    XtFree(data->sel[NAME]);
	    data->sel[NAME] = ശൂന്യ;
	}
	എങ്കിൽ (data->sel[STYLE])
	{
	    XtFree(data->sel[STYLE]);
	    data->sel[STYLE] = ശൂന്യ;
	}
	എങ്കിൽ (data->sel[SIZE])
	{
	    XtFree(data->sel[SIZE]);
	    data->sel[SIZE] = ശൂന്യ;
	}

	XFreeFontNames(name);

	data->അക്കം = 0;
	XFreeFontNames(data->names);
	data->names = ശൂന്യ;
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
    സ്ഥിതം SharedFontSelData	_data;

    Widget		parent;
    Widget		form;
    Widget		separator;
    Widget		sub_form;
    Widget		size_toggle;
    Widget		name;
    Widget		disp_frame;
    Widget		frame;
    Arg			args[64];
    പൂർണ്ണസംഖ്യ			n;
    XmString		str;
    അക്ഷരം		big_font[MAX_FONT_NAME_LEN];
    SharedFontSelData	*data;

    data = &_data;

    parent = vimShell;
    data->names = XListFonts(XtDisplay(parent), "-*-*-*-*-*-*-*-*-*-*-*-*-*-*",
						       MAX_FONTS, &data->അക്കം);

    /*
     * Find the name of the biggest font less than the given limit
     * MAX_DISPLAY_SIZE used to set up the initial height of the display
     * widget.
     */

    {
	പൂർണ്ണസംഖ്യ	i;
	പൂർണ്ണസംഖ്യ	max;
	പൂർണ്ണസംഖ്യ	idx = 0;
	പൂർണ്ണസംഖ്യ	size;
	അക്ഷരം	buf[128];

	വേണ്ടി (i = 0, max = 0; i < data->അക്കം; i++)
	{
	    get_part(fn(data, i), 7, buf);
	    size = atoi(buf);
	    എങ്കിൽ ((size > max) && (size < MAX_DISPLAY_SIZE))
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
    data->dialog = XmCreateDialogShell(parent, "fontSelector", ശൂന്യ, 0);
    str = XmStringCreateLocalized(_("Vim - Font Selector"));

    // Create form popup dialog widget.
    form = XtVaCreateWidget("form",
	    xmFormWidgetClass, data->dialog,
	    XmNdialogTitle, str,
	    XmNautoUnmanage, False,
	    XmNdialogStyle, XmDIALOG_FULL_APPLICATION_MODAL,
	    ശൂന്യ);
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
	    ശൂന്യ);

    data->ok = XtVaCreateManagedWidget(_("OK"),
	    xm_തള്ളുക_Button_Gadget_Class, sub_form,
	    XmNleftAttachment, XmATTACH_FORM,
	    XmNrightAttachment, XmATTACH_FORM,
	    XmNtopAttachment, XmATTACH_FORM,
	    XmNtopOffset, 4,
	    ശൂന്യ);
    apply_fontlist(data->ok);

    data->cancel = XtVaCreateManagedWidget(_("Cancel"),
	    xm_തള്ളുക_Button_Gadget_Class, sub_form,
	    XmNrightAttachment, XmATTACH_FORM,
	    XmNleftAttachment, XmATTACH_FORM,
	    XmNtopAttachment, XmATTACH_WIDGET,
	    XmNtopWidget, data->ok,
	    XmNtopOffset, 4,
	    XmNshowAsDefault, True,
	    ശൂന്യ);
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
	    XmNeditMode, Xm_SINGLE_വരി_EDIT,
	    XmNmaxLength, MAX_FONT_NAME_LEN,
	    XmNcolumns, 60,
	    ശൂന്യ);

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
	    ശൂന്യ);
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
	    ശൂന്യ);

    data->sample = XtVaCreateManagedWidget("sampleLabel",
	    xmLabelWidgetClass, disp_frame,
	    XmNleftAttachment, XmATTACH_FORM,
	    XmNtopAttachment, XmATTACH_FORM,
	    XmNbottomAttachment, XmATTACH_FORM,
	    XmNrightAttachment, XmATTACH_FORM,
	    XmNalignment, XmALIGNMENT_BEGINNING,
	    XmNrecomputeSize, False,
	    XmNfontList, data->old_list,
	    ശൂന്യ);

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
	    ശൂന്യ);
    XmStringFree(str);
    apply_fontlist(size_toggle);
    XtManageChild(size_toggle);

    // Encoding pulldown menu.

    data->encoding_pulldown = XmCreatePulldownMenu(form,
						 "encodingPulldown", ശൂന്യ, 0);
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
	    ശൂന്യ);

    // font list
    frame = XtVaCreateManagedWidget("frame", xmFrameWidgetClass, sub_form,
	    XmNshadowThickness, 0,
	    XmNtopAttachment, XmATTACH_FORM,
	    XmNbottomAttachment, XmATTACH_FORM,
	    XmNleftAttachment, XmATTACH_FORM,
	    XmNrightAttachment, XmATTACH_POSITION,
	    XmNrightPosition, 50,
	    ശൂന്യ);

    str = XmStringCreateLocalized(_("Font:"));
    name = XtVaCreateManagedWidget("nameListLabel", xmLabelGadgetClass, frame,
	    XmNchildType, XmFRAME_TITLE_CHILD,
	    XmNchildVerticalAlignment, XmALIGNMENT_CENTER,
	    XmNchildHorizontalAlignment, XmALIGNMENT_BEGINNING,
	    XmNlabelString, str,
	    ശൂന്യ);
    XmStringFree(str);
    apply_fontlist(name);

    n = 0;
    XtSetArg(args[n], Xm_Nvisible_Item_എണ്ണം, 8); ++n;
    XtSetArg(args[n], XmNresizable, True); ++n;
    XtSetArg(args[n], XmNlistSizePolicy, XmCONSTANT); ++n;
    XtSetArg(args[n], XmNvisualPolicy, XmVARIABLE); ++n;
#ifdef LESSTIF_VERSION
    XtSetArg(args[n], XmNscrollBarDisplayPolicy, XmSTATIC); ++n;
#endif
    data->list[NAME] = XmCreateScrolledList(frame, "fontList", args, n);
    Xt_Va_Set_മൂല്യങ്ങൾ(name, XmNuserData, data->list[NAME], ശൂന്യ);

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
	    ശൂന്യ);

    str = XmStringCreateLocalized(_("Style:"));
    name = XtVaCreateManagedWidget("styleListLabel", xmLabelWidgetClass, frame,
	    XmNchildType, XmFRAME_TITLE_CHILD,
	    XmNchildVerticalAlignment, XmALIGNMENT_CENTER,
	    XmNchildHorizontalAlignment, XmALIGNMENT_BEGINNING,
	    XmNlabelString, str,
	    ശൂന്യ);
    XmStringFree(str);
    apply_fontlist(name);

    n = 0;
    XtSetArg(args[n], Xm_Nvisible_Item_എണ്ണം, 8); ++n;
    XtSetArg(args[n], XmNresizable, True); ++n;
    XtSetArg(args[n], XmNlistSizePolicy, XmCONSTANT); ++n;
    XtSetArg(args[n], XmNvisualPolicy, XmVARIABLE); ++n;
#ifdef LESSTIF_VERSION
    XtSetArg(args[n], XmNscrollBarDisplayPolicy, XmSTATIC); ++n;
#endif
    data->list[STYLE] = XmCreateScrolledList(frame, "styleList", args, n);
    Xt_Va_Set_മൂല്യങ്ങൾ(name, XmNuserData, data->list[STYLE], ശൂന്യ);

    // size list
    frame = XtVaCreateManagedWidget("frame", xmFrameWidgetClass, sub_form,
	    XmNshadowThickness, 0,
	    XmNtopAttachment, XmATTACH_FORM,
	    XmNbottomAttachment, XmATTACH_FORM,
	    XmNleftAttachment, XmATTACH_POSITION,
	    XmNleftPosition, 80,
	    XmNleftOffset, 4,
	    XmNrightAttachment, XmATTACH_FORM,
	    ശൂന്യ);

    str = XmStringCreateLocalized(_("Size:"));
    name = XtVaCreateManagedWidget("sizeListLabel", xmLabelGadgetClass, frame,
	    XmNchildType, XmFRAME_TITLE_CHILD,
	    XmNchildVerticalAlignment, XmALIGNMENT_CENTER,
	    XmNchildHorizontalAlignment, XmALIGNMENT_BEGINNING,
	    XmNlabelString, str,
	    ശൂന്യ);
    XmStringFree(str);
    apply_fontlist(name);

    n = 0;
    XtSetArg(args[n], Xm_Nvisible_Item_എണ്ണം, 8); ++n;
    XtSetArg(args[n], XmNresizable, True); ++n;
    XtSetArg(args[n], XmNlistSizePolicy, XmCONSTANT); ++n;
    XtSetArg(args[n], XmNvisualPolicy, XmVARIABLE); ++n;
#ifdef LESSTIF_VERSION
    XtSetArg(args[n], XmNscrollBarDisplayPolicy, XmSTATIC); ++n;
#endif
    data->list[SIZE] = XmCreateScrolledList(frame, "sizeList", args, n);
    Xt_Va_Set_മൂല്യങ്ങൾ(name, XmNuserData, data->list[SIZE], ശൂന്യ);

    // update form widgets cancel button
    Xt_Va_Set_മൂല്യങ്ങൾ(form, XmNcancelButton, data->cancel, ശൂന്യ);

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
    data->sel[ENCODING] = ശൂന്യ;
    data->sel[NAME] = ശൂന്യ;
    data->sel[STYLE] = ശൂന്യ;
    data->sel[SIZE] = ശൂന്യ;
    data->font_name = ശൂന്യ;

    // set up current font parameters
    എങ്കിൽ (current && current[0] != '\0')
    {
	പൂർണ്ണസംഖ്യ	    i;
	അക്ഷരം	    **names;

	names = XListFonts(XtDisplay(form), (അക്ഷരം *) current, 1, &i);

	എങ്കിൽ (i != 0)
	{
	    അക്ഷരം namebuf[TEMP_BUF_SIZE];
	    അക്ഷരം stylebuf[TEMP_BUF_SIZE];
	    അക്ഷരം sizebuf[TEMP_BUF_SIZE];
	    അക്ഷരം encodingbuf[TEMP_BUF_SIZE];
	    അക്ഷരം *found;

	    found = names[0];

	    name_part(found, namebuf);
	    style_part(found, stylebuf);
	    size_part(found, sizebuf, data->in_pixels);
	    encoding_part(found, encodingbuf);

	    എങ്കിൽ (*namebuf != NUL
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
	    അല്ലെങ്കിൽ
	    {
		// We can't preset a symbolic name, which isn't a full font
		// description. Therefore we just behave the same way as if the
		// user didn't have selected anything thus far.
		//
		// Unfortunately there is no known way to expand an abbreviated
		// font name.
		data->font_name = ശൂന്യ;
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
    ആകുമ്പോൾ (!data->exit)
	XtAppProcessEvent(XtWidgetToApplicationContext(data->dialog),
							(Xt_നിവേശം_Mask)XtIMAll);

    എങ്കിൽ (data->old)
    {
	XFreeFont(XtDisplay(data->dialog),  data->old);
	XmFontListFree(data->old_list);
    }
    XtDestroyWidget(data->dialog);

    gui_motif_synch_fonts();

    തിരികെ (char_u *) data->font_name;
}
