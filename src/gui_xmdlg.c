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

#अन्तर्भाव "vim.h"

#अन्तर्भाव <Xm/Form.h>
#अन्तर्भाव <Xm/PushBG.h>
#अन्तर्भाव <Xm/Text.h>
#अन्तर्भाव <Xm/TextF.h>
#अन्तर्भाव <Xm/Label.h>
#अन्तर्भाव <Xm/Frame.h>
#अन्तर्भाव <Xm/LabelG.h>
#अन्तर्भाव <Xm/ToggleBG.h>
#अन्तर्भाव <Xm/SeparatoG.h>
#अन्तर्भाव <Xm/DialogS.h>
#अन्तर्भाव <Xm/List.h>
#अन्तर्भाव <Xm/RowColumn.h>
#अन्तर्भाव <Xm/AtomMgr.h>
#अन्तर्भाव <Xm/Protocols.h>

#अन्तर्भाव <X11/keysym.h>
#अन्तर्भाव <X11/Xatom.h>
#अन्तर्भाव <X11/StringDefs.h>
#अन्तर्भाव <X11/Intrinsic.h>

बाह्य Widget vimShell;

#ifdef FEAT_MENU
# define apply_fontlist(w) gui_motif_menu_fontlist(w)
#अन्यथा
# define apply_fontlist(w)
#endif

/////////////////////////////////////////////////////////////////////////////
// Font selection dialogue implementation.

स्थिर वर्ण wild[3] = "*";

/*
 * FIXME: This is a generic function, which should be used throughout the whole
 * application.
 *
 * Add close_callback, which will be called when the user selects close from
 * the window menu.  The close menu item usually activates f.kill which sends a
 * WM_DELETE_WINDOW protocol request for the window.
 */

    स्थिर शून्य
add_cancel_action(Widget shell, XtCallbackProc close_callback, शून्य *arg)
{
    स्थिर Atom wmp_atom = 0;
    स्थिर Atom dw_atom = 0;
    Display *display = XtDisplay(shell);

    // deactivate the built-in delete response of killing the application
    Xt_Va_Set_मूल्ये(shell, XmNdeleteResponse, XmDO_NOTHING, रिक्त);

    // add a delete window protocol callback instead
    यदि (!dw_atom)
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

गणना ListSpecifier
{
    ENCODING,
    NAME,
    STYLE,
    SIZE,
    NONE
};

प्रकारनिर्देश संरचना _SharedFontSelData
{
    Widget	dialog;
    Widget	ok;
    Widget	cancel;
    Widget	encoding_pulldown;
    Widget	encoding_menu;
    Widget	list[NONE];
    Widget	name;
    Widget	sample;
    वर्ण	**names;	// font name array of arrays
    पूर्णाङ्क		अंक;		// number of font names
    String	sel[NONE];	// selection category
    Boolean	in_pixels;	// toggle state - size in pixels
    वर्ण	*font_name;	// current font name
    XFontStruct	*old;		// font data structure for sample display
    XmFontList	old_list;	// font data structure for sample display
    Boolean	exit;		// used for program exit control
} SharedFontSelData;

/*
 * Checking access to the font name array for validity.
 */
    स्थिर वर्ण *
fn(SharedFontSelData *data, पूर्णाङ्क i)
{
    // Assertion checks:
    यदि (data->अंक < 0)
	abort();
    यदि (i >= data->अंक)
	i = data->अंक - 1;
    यदि (i < 0)
	i = 0;

    प्रत्यागम data->names[i];
}

/*
 * Get a specific substring from a font name.
 */
    स्थिर शून्य
get_part(वर्ण *in, पूर्णाङ्क pos, वर्ण *out)
{
    पूर्णाङ्क	i;
    पूर्णाङ्क j;

    *out = '\0';

    प्रति (i = 0; (pos > 0) && (in[i] != '\0'); ++i)
	यदि (in[i] == '-')
	    pos--;

    यदि (in[i] == '\0')
	प्रत्यागम;

    प्रति (j = 0; (in[i] != '-') && (in[i] != '\0'); ++i, ++j)
	out[j] = in[i];
    out[j] = '\0';
}

/*
 * Given a font name this function returns the part used in the first
 * scroll list.
 */
    स्थिर शून्य
name_part(वर्ण *font, वर्ण *buf)
{
    वर्ण    buf2[TEMP_BUF_SIZE];
    वर्ण    buf3[TEMP_BUF_SIZE];

    get_part(font, 2, buf2);
    get_part(font, 1, buf3);

    यदि (*buf3 != NUL)
	vim_snprintf(buf, TEMP_BUF_SIZE, "%s (%s)", buf2, buf3);
    अन्यथा
	vim_snprintf(buf, TEMP_BUF_SIZE, "%s", buf2);
}

/*
 * Given a font name this function returns the part used in the second scroll list.
 */
    स्थिर शून्य
style_part(वर्ण *font, वर्ण *buf)
{
    वर्ण    buf2[TEMP_BUF_SIZE];
    वर्ण    buf3[TEMP_BUF_SIZE];

    get_part(font, 3, buf3);
    get_part(font, 5, buf2);

    यदि (!strcmp(buf2, "normal") && !strcmp(buf2, "Normal")
						   && !strcmp(buf2, "NORMAL"))
	vim_snprintf(buf, TEMP_BUF_SIZE, "%s %s", buf3, buf2);
    अन्यथा
	strcpy(buf, buf3);

    get_part(font, 6, buf2);

    यदि (buf2[0] != '\0')
	vim_snprintf(buf3, TEMP_BUF_SIZE, "%s %s", buf, buf2);
    अन्यथा
	strcpy(buf3, buf);

    get_part(font, 4, buf2);

    यदि (!strcmp(buf2, "o") || !strcmp(buf2, "O"))
	vim_snprintf(buf, TEMP_BUF_SIZE, "%s oblique", buf3);
    अन्यथा यदि (!strcmp(buf2, "i") || !strcmp(buf2, "I"))
	vim_snprintf(buf, TEMP_BUF_SIZE, "%s italic", buf3);

    यदि (!strcmp(buf, " "))
	strcpy(buf, "-");
}

/*
 * Given a font name this function returns the part used in the third
 * scroll list.
 */
    स्थिर शून्य
size_part(वर्ण *font, वर्ण *buf, पूर्णाङ्क inPixels)
{
    पूर्णाङ्क	    size;
    भिन्न   temp;

    *buf = '\0';

    यदि (inPixels)
    {
	get_part(font, 7, buf);
	यदि (*buf != NUL)
	{
	    size = atoi(buf);
	    sprintf(buf, "%3d", size);
	}
    }
    अन्यथा
    {
	get_part(font, 8, buf);
	यदि (*buf != NUL)
	{
	    size = atoi(buf);
	    temp = (भिन्न)size / 10.0;
	    size = temp;
	    यदि (buf[strlen(buf) - 1] == '0')
		sprintf(buf, "%3d", size);
	    अन्यथा
		sprintf(buf, "%4.1f", temp);
	}
    }
}

/*
 * Given a font name this function returns the part used in the choice menu.
 */
    स्थिर शून्य
encoding_part(वर्ण *font, वर्ण *buf)
{
    वर्ण    buf1[TEMP_BUF_SIZE];
    वर्ण    buf2[TEMP_BUF_SIZE];

    *buf = '\0';

    get_part(font, 13, buf1);
    get_part(font, 14, buf2);

    यदि (*buf1 != NUL && *buf2 != NUL)
	vim_snprintf(buf, TEMP_BUF_SIZE, "%s-%s", buf1, buf2);
    यदि (!strcmp(buf, " "))
	strcpy(buf, "-");
}

/*
 * Inserts a string into correct sorted position in a list.
 */
    स्थिर शून्य
add_to_list(वर्ण **buf, वर्ण *item, पूर्णाङ्क *मोजणी)
{
    पूर्णाङ्क	i;
    पूर्णाङ्क j;

    यदि (*मोजणी == MAX_ENTRIES_IN_LIST)
	प्रत्यागम;

    // avoid duplication
    प्रति (i = 0; i < *मोजणी; ++i)
    {
	यदि (!strcmp(buf[i], item))
	    प्रत्यागम;
    }

    // find order place, but make sure that wild card comes first
    यदि (!strcmp(item, wild))
	i = 0;
    अन्यथा
	प्रति (i = 0; i < *मोजणी; ++i)
	    यदि (strcmp(buf[i], item) > 0 && strcmp(buf[i], wild))
		भङ्ग;

    // now insert the item
    प्रति (j = *मोजणी; j > i; --j)
	buf[j] = buf[j-1];
    buf[i] = XtNewString(item);

    ++(*मोजणी);
}

/*
 * True if the font matches some field.
 */
    स्थिर Boolean
match(SharedFontSelData *data, गणना ListSpecifier l, पूर्णाङ्क i)
{
    वर्ण buf[TEMP_BUF_SIZE];

    // An empty selection or a wild card matches anything.
    यदि (!data->sel[l] || !strcmp(data->sel[l], wild))
	प्रत्यागम True;

    // chunk out the desired part...
    विकल्प (l)
    {
	पक्ष ENCODING:
	    encoding_part(fn(data, i), buf);
	    भङ्ग;

	पक्ष NAME:
	    name_part(fn(data, i), buf);
	    भङ्ग;

	पक्ष STYLE:
	    style_part(fn(data, i), buf);
	    भङ्ग;

	पक्ष SIZE:
	    size_part(fn(data, i), buf, data->in_pixels);
	    भङ्ग;
	स्वभाव:
	    ;
    }

    // ...and chew it now

    प्रत्यागम !strcmp(buf, data->sel[l]);
}

    स्थिर Boolean
proportional(वर्ण *font)
{
    वर्ण buf[TEMP_BUF_SIZE];

    get_part(font, 11, buf);

    प्रत्यागम !strcmp(buf, "p") || !strcmp(buf, "P");
}


स्थिर शून्य encoding_callback(Widget w, SharedFontSelData *data,
							     XtPointer dummy);

/*
 * Parse through the fontlist data and set up the three scroll lists.  The fix
 * parameter can be used to exclude a list from any changes.  This is used for
 * updates after selections caused by the users actions.
 */
    स्थिर शून्य
fill_lists(गणना ListSpecifier fix, SharedFontSelData *data)
{
    वर्ण	*list[NONE][MAX_ENTRIES_IN_LIST];
    पूर्णाङ्क		मोजणी[NONE];
    वर्ण	buf[TEMP_BUF_SIZE];
    XmString	वस्तुसंच[MAX_ENTRIES_IN_LIST];
    पूर्णाङ्क		i;
    पूर्णाङ्क		idx;

    प्रति (idx = (पूर्णाङ्क)ENCODING; idx < (पूर्णाङ्क)NONE; ++idx)
	मोजणी[idx] = 0;

    // First we insert the wild char into every single list.
    यदि (fix != ENCODING)
	add_to_list(list[ENCODING], wild, &मोजणी[ENCODING]);
    यदि (fix != NAME)
	add_to_list(list[NAME], wild, &मोजणी[NAME]);
    यदि (fix != STYLE)
	add_to_list(list[STYLE], wild, &मोजणी[STYLE]);
    यदि (fix != SIZE)
	add_to_list(list[SIZE], wild, &मोजणी[SIZE]);

    प्रति (i = 0; i < data->अंक && i < MAX_ENTRIES_IN_LIST; i++)
    {
	यदि (proportional(fn(data, i)))
	    अनुवर्त;

	यदि (fix != ENCODING
		&& match(data, NAME, i)
		&& match(data, STYLE, i)
		&& match(data, SIZE, i))
	{
	    encoding_part(fn(data, i), buf);
	    add_to_list(list[ENCODING], buf, &मोजणी[ENCODING]);
	}

	यदि (fix != NAME
		&& match(data, ENCODING, i)
		&& match(data, STYLE, i)
		&& match(data, SIZE, i))
	{
	    name_part(fn(data, i), buf);
	    add_to_list(list[NAME], buf, &मोजणी[NAME]);
	}

	यदि (fix != STYLE
		&& match(data, ENCODING, i)
		&& match(data, NAME, i)
		&& match(data, SIZE, i))
	{
	    style_part(fn(data, i), buf);
	    add_to_list(list[STYLE], buf, &मोजणी[STYLE]);
	}

	यदि (fix != SIZE
		&& match(data, ENCODING, i)
		&& match(data, NAME, i)
		&& match(data, STYLE, i))
	{
	    size_part(fn(data, i), buf, data->in_pixels);
	    add_to_list(list[SIZE], buf, &मोजणी[SIZE]);
	}
    }

    /*
     * And now do the preselection in all lists where there was one:
     */

    यदि (fix != ENCODING)
    {
	Cardinal n_वस्तुसंच;
	WidgetList children;
	Widget selected_button = 0;

	// Get and update the current button list.
	Xt_Va_Get_मूल्ये(data->encoding_pulldown,
		XmNchildren, &children,
		XmNnumChildren, &n_वस्तुसंच,
		रिक्त);

	प्रति (i = 0; i < मोजणी[ENCODING]; ++i)
	{
	    Widget button;

	    वस्तुसंच[i] = XmStringCreateLocalized(list[ENCODING][i]);

	    यदि (i < (पूर्णाङ्क)n_वस्तुसंच)
	    {
		// recycle old button
		Xt_Va_Set_मूल्ये(children[i],
			XmNlabelString, वस्तुसंच[i],
			XmNuserData, i,
			रिक्त);
		button = children[i];
	    }
	    अन्यथा
	    {
		// create a new button
		button = XtVaCreateManagedWidget("button",
			xm_ढकला_Button_Gadget_Class,
			data->encoding_pulldown,
			XmNlabelString, वस्तुसंच[i],
			XmNuserData, i,
			रिक्त);
		XtAddCallback(button, XmNactivateCallback,
			(XtCallbackProc) encoding_callback, (XtPointer) data);
		XtManageChild(button);
	    }

	    यदि (data->sel[ENCODING])
	    {
		यदि (!strcmp(data->sel[ENCODING], list[ENCODING][i]))
		    selected_button = button;
	    }
	    XtFree(list[ENCODING][i]);
	}

	// Destroy all the outstanding menu items.
	प्रति (i = मोजणी[ENCODING]; i < (पूर्णाङ्क)n_वस्तुसंच; ++i)
	{
	    XtUnmanageChild(children[i]);
	    XtDestroyWidget(children[i]);
	}

	// Preserve the current selection visually.
	यदि (selected_button)
	{
	    Xt_Va_Set_मूल्ये(data->encoding_menu,
		    XmNmenuHistory, selected_button,
		    रिक्त);
	}

	प्रति (i = 0; i < मोजणी[ENCODING]; ++i)
	    XmStringFree(वस्तुसंच[i]);
    }

    /*
     * Now loop through the remaining lists and set them up.
     */
    प्रति (idx = (पूर्णाङ्क)NAME; idx < (पूर्णाङ्क)NONE; ++idx)
    {
	Widget w;

	यदि (fix == (गणना ListSpecifier)idx)
	    अनुवर्त;

	विकल्प ((गणना ListSpecifier)idx)
	{
	    पक्ष NAME:
		w = data->list[NAME];
		भङ्ग;
	    पक्ष STYLE:
		w = data->list[STYLE];
		भङ्ग;
	    पक्ष SIZE:
		w = data->list[SIZE];
		भङ्ग;
	    स्वभाव:
		w = (Widget)0;	// for lint
	}

	प्रति (i = 0; i < मोजणी[idx]; ++i)
	{
	    वस्तुसंच[i] = XmStringCreateLocalized(list[idx][i]);
	    XtFree(list[idx][i]);
	}
	Xm_List_Delete_All_वस्तुसंच(w);
	Xm_List_Add_वस्तुसंच(w, वस्तुसंच, मोजणी[idx], 1);
	यदि (data->sel[idx])
	{
	    XmStringFree(वस्तुसंच[0]);
	    वस्तुसंच[0] = XmStringCreateLocalized(data->sel[idx]);
	    XmListSelectItem(w, वस्तुसंच[0], False);
	    XmListSetBottomItem(w, वस्तुसंच[0]);
	}
	प्रति (i = 0; i < मोजणी[idx]; ++i)
	    XmStringFree(वस्तुसंच[i]);
    }
}

    स्थिर शून्य
stoggle_callback(Widget w UNUSED,
	SharedFontSelData *data,
	XmToggleButtonCallbackStruct *call_data)
{
    पूर्णाङ्क		i, do_sel;
    वर्ण	newSize[TEMP_BUF_SIZE];
    XmString	str;

    यदि (call_data->reason != (पूर्णाङ्क)Xm_CR_मूल्य_CHANGED)
	प्रत्यागम;

    do_sel = (data->sel[SIZE] != रिक्त) && strcmp(data->sel[SIZE], wild);

    प्रति (i = 0; do_sel && (i < data->अंक); i++)
	यदि (match(data, ENCODING, i)
		&& match(data, NAME, i)
		&& match(data, STYLE, i)
		&& match(data, SIZE, i))
	{
	    size_part(fn(data, i), newSize, !data->in_pixels);
	    भङ्ग;
	}

    data->in_pixels = !data->in_pixels;

    यदि (data->sel[SIZE])
	XtFree(data->sel[SIZE]);
    data->sel[SIZE] = रिक्त;
    fill_lists(NONE, data);

    यदि (do_sel)
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
    स्थिर शून्य
display_sample(SharedFontSelData *data)
{
    Arg		    args[2];
    पूर्णाङ्क		    n;
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

    Xt_Set_मूल्ये(data->sample, args, n);
    XmStringFree(str);

    यदि (data->old)
    {
	XFreeFont(display, data->old);
	XmFontListFree(data->old_list);
    }
    data->old = font;
    data->old_list = font_list;
}


    स्थिर Boolean
do_choice(Widget w,
	SharedFontSelData *data,
	XmListCallbackStruct *call_data,
	गणना ListSpecifier which)
{
    वर्ण *sel;

    XmStringGetLtoR(call_data->item, XmSTRING_DEFAULT_CHARSET, &sel);

    यदि (!data->sel[which])
	data->sel[which] = XtNewString(sel);
    अन्यथा
    {
	यदि (!strcmp(data->sel[which], sel))
	{
	    // unselecting current selection
	    XtFree(data->sel[which]);
	    data->sel[which] = रिक्त;
	    यदि (w)
		XmListDeselectItem(w, call_data->item);
	}
	अन्यथा
	{
	    XtFree(data->sel[which]);
	    data->sel[which] = XtNewString(sel);
	}
    }
    XtFree(sel);

    fill_lists(which, data);

    // If there is a font selection, we display it.
    यदि (data->sel[ENCODING]
	    && data->sel[NAME]
	    && data->sel[STYLE]
	    && data->sel[SIZE]
	    && strcmp(data->sel[ENCODING], wild)
	    && strcmp(data->sel[NAME], wild)
	    && strcmp(data->sel[STYLE], wild)
	    && strcmp(data->sel[SIZE], wild))
    {
	पूर्णाङ्क i;

	यदि (data->font_name)
	    XtFree(data->font_name);
	data->font_name = रिक्त;

	प्रति (i = 0; i < data->अंक; i++)
	{
	    यदि (match(data, ENCODING, i)
		    && match(data, NAME, i)
		    && match(data, STYLE, i)
		    && match(data, SIZE, i))
	    {
		data->font_name = XtNewString(fn(data, i));
		भङ्ग;
	    }
	}

	यदि (data->font_name)
	{
	    XmTextSetString(data->name, data->font_name);
	    display_sample(data);
	}
	अन्यथा
	    do_dialog(VIM_ERROR,
		    (char_u *)_("Error"),
		    (char_u *)_("Invalid font specification"),
		    (char_u *)_("&Dismiss"), 1, रिक्त, FALSE);

	प्रत्यागम True;
    }
    अन्यथा
    {
	पूर्णाङ्क	    n;
	XmString    str;
	Arg	    args[4];
	वर्ण	    *nomatch_संदेश = _("no specific match");

	n = 0;
	str = XmStringCreateLocalized(nomatch_संदेश);
	XtSetArg(args[n], XmNlabelString, str); ++n;
	Xt_Set_मूल्ये(data->sample, args, n);
	apply_fontlist(data->sample);
	XmTextSetString(data->name, nomatch_संदेश);
	XmStringFree(str);

	प्रत्यागम False;
    }
}

    स्थिर शून्य
encoding_callback(Widget w,
	SharedFontSelData *data,
	XtPointer dummy UNUSED)
{
    XmString str;
    XmListCallbackStruct fake_data;

    Xt_Va_Get_मूल्ये(w, XmNlabelString, &str, रिक्त);

    यदि (!str)
	प्रत्यागम;

    fake_data.item = str;

    do_choice(0, data, &fake_data, ENCODING);
}

    स्थिर शून्य
name_callback(Widget w,
	SharedFontSelData *data,
	XmListCallbackStruct *call_data)
{
    do_choice(w, data, call_data, NAME);
}

    स्थिर शून्य
style_callback(Widget w,
	SharedFontSelData *data,
	XmListCallbackStruct *call_data)
{
    do_choice(w, data, call_data, STYLE);
}

    स्थिर शून्य
size_callback(Widget w,
	SharedFontSelData *data,
	XmListCallbackStruct *call_data)
{
    do_choice(w, data, call_data, SIZE);
}

    स्थिर शून्य
cancel_callback(Widget w UNUSED,
	SharedFontSelData *data,
	XmListCallbackStruct *call_data UNUSED)
{
    यदि (data->sel[ENCODING])
    {
	XtFree(data->sel[ENCODING]);
	data->sel[ENCODING] = रिक्त;
    }
    यदि (data->sel[NAME])
    {
	XtFree(data->sel[NAME]);
	data->sel[NAME] = रिक्त;
    }
    यदि (data->sel[STYLE])
    {
	XtFree(data->sel[STYLE]);
	data->sel[STYLE] = रिक्त;
    }
    यदि (data->sel[SIZE])
    {
	XtFree(data->sel[SIZE]);
	data->sel[SIZE] = रिक्त;
    }

    यदि (data->font_name)
	XtFree(data->font_name);
    data->font_name = रिक्त;

    data->अंक = 0;
    XFreeFontNames(data->names);
    data->names = रिक्त;
    data->exit = True;
}

    स्थिर शून्य
ok_callback(Widget w UNUSED,
	SharedFontSelData *data,
	Xm_ढकला_Button_Callback_Struct *call_data UNUSED)
{
    वर्ण    *pattern;
    वर्ण    **name;
    पूर्णाङ्क	    i;

    pattern = XmTextGetString(data->name);
    name    = XListFonts(XtDisplay(data->dialog), pattern, 1, &i);
    XtFree(pattern);

    यदि (i != 1)
    {
	do_dialog(VIM_ERROR,
		(char_u *)_("Error"),
		(char_u *)_("Invalid font specification"),
		(char_u *)_("&Dismiss"), 1, रिक्त, FALSE);
	XFreeFontNames(name);
    }
    अन्यथा
    {
	यदि (data->font_name)
	    XtFree(data->font_name);
	data->font_name = XtNewString(name[0]);

	यदि (data->sel[ENCODING])
	{
	    XtFree(data->sel[ENCODING]);
	    data->sel[ENCODING] = रिक्त;
	}
	यदि (data->sel[NAME])
	{
	    XtFree(data->sel[NAME]);
	    data->sel[NAME] = रिक्त;
	}
	यदि (data->sel[STYLE])
	{
	    XtFree(data->sel[STYLE]);
	    data->sel[STYLE] = रिक्त;
	}
	यदि (data->sel[SIZE])
	{
	    XtFree(data->sel[SIZE]);
	    data->sel[SIZE] = रिक्त;
	}

	XFreeFontNames(name);

	data->अंक = 0;
	XFreeFontNames(data->names);
	data->names = रिक्त;
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
    स्थिर SharedFontSelData	_data;

    Widget		parent;
    Widget		form;
    Widget		separator;
    Widget		sub_form;
    Widget		size_toggle;
    Widget		name;
    Widget		disp_frame;
    Widget		frame;
    Arg			args[64];
    पूर्णाङ्क			n;
    XmString		str;
    वर्ण		big_font[MAX_FONT_NAME_LEN];
    SharedFontSelData	*data;

    data = &_data;

    parent = vimShell;
    data->names = XListFonts(XtDisplay(parent), "-*-*-*-*-*-*-*-*-*-*-*-*-*-*",
						       MAX_FONTS, &data->अंक);

    /*
     * Find the name of the biggest font less than the given limit
     * MAX_DISPLAY_SIZE used to set up the initial height of the display
     * widget.
     */

    {
	पूर्णाङ्क	i;
	पूर्णाङ्क	max;
	पूर्णाङ्क	idx = 0;
	पूर्णाङ्क	size;
	वर्ण	buf[128];

	प्रति (i = 0, max = 0; i < data->अंक; i++)
	{
	    get_part(fn(data, i), 7, buf);
	    size = atoi(buf);
	    यदि ((size > max) && (size < MAX_DISPLAY_SIZE))
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
    data->dialog = XmCreateDialogShell(parent, "fontSelector", रिक्त, 0);
    str = XmStringCreateLocalized(_("Vim - Font Selector"));

    // Create form popup dialog widget.
    form = XtVaCreateWidget("form",
	    xmFormWidgetClass, data->dialog,
	    XmNdialogTitle, str,
	    XmNautoUnmanage, False,
	    XmNdialogStyle, XmDIALOG_FULL_APPLICATION_MODAL,
	    रिक्त);
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
	    रिक्त);

    data->ok = XtVaCreateManagedWidget(_("OK"),
	    xm_ढकला_Button_Gadget_Class, sub_form,
	    XmNleftAttachment, XmATTACH_FORM,
	    XmNrightAttachment, XmATTACH_FORM,
	    XmNtopAttachment, XmATTACH_FORM,
	    XmNtopOffset, 4,
	    रिक्त);
    apply_fontlist(data->ok);

    data->cancel = XtVaCreateManagedWidget(_("Cancel"),
	    xm_ढकला_Button_Gadget_Class, sub_form,
	    XmNrightAttachment, XmATTACH_FORM,
	    XmNleftAttachment, XmATTACH_FORM,
	    XmNtopAttachment, XmATTACH_WIDGET,
	    XmNtopWidget, data->ok,
	    XmNtopOffset, 4,
	    XmNshowAsDefault, True,
	    रिक्त);
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
	    XmNeditMode, Xm_SINGLE_ओळ_EDIT,
	    XmNmaxLength, MAX_FONT_NAME_LEN,
	    XmNcolumns, 60,
	    रिक्त);

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
	    रिक्त);
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
	    रिक्त);

    data->sample = XtVaCreateManagedWidget("sampleLabel",
	    xmLabelWidgetClass, disp_frame,
	    XmNleftAttachment, XmATTACH_FORM,
	    XmNtopAttachment, XmATTACH_FORM,
	    XmNbottomAttachment, XmATTACH_FORM,
	    XmNrightAttachment, XmATTACH_FORM,
	    XmNalignment, XmALIGNMENT_BEGINNING,
	    XmNrecomputeSize, False,
	    XmNfontList, data->old_list,
	    रिक्त);

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
	    रिक्त);
    XmStringFree(str);
    apply_fontlist(size_toggle);
    XtManageChild(size_toggle);

    // Encoding pulldown menu.

    data->encoding_pulldown = XmCreatePulldownMenu(form,
						 "encodingPulldown", रिक्त, 0);
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
	    रिक्त);

    // font list
    frame = XtVaCreateManagedWidget("frame", xmFrameWidgetClass, sub_form,
	    XmNshadowThickness, 0,
	    XmNtopAttachment, XmATTACH_FORM,
	    XmNbottomAttachment, XmATTACH_FORM,
	    XmNleftAttachment, XmATTACH_FORM,
	    XmNrightAttachment, XmATTACH_POSITION,
	    XmNrightPosition, 50,
	    रिक्त);

    str = XmStringCreateLocalized(_("Font:"));
    name = XtVaCreateManagedWidget("nameListLabel", xmLabelGadgetClass, frame,
	    XmNchildType, XmFRAME_TITLE_CHILD,
	    XmNchildVerticalAlignment, XmALIGNMENT_CENTER,
	    XmNchildHorizontalAlignment, XmALIGNMENT_BEGINNING,
	    XmNlabelString, str,
	    रिक्त);
    XmStringFree(str);
    apply_fontlist(name);

    n = 0;
    XtSetArg(args[n], Xm_Nvisible_Item_मोजणी, 8); ++n;
    XtSetArg(args[n], XmNresizable, True); ++n;
    XtSetArg(args[n], XmNlistSizePolicy, XmCONSTANT); ++n;
    XtSetArg(args[n], XmNvisualPolicy, XmVARIABLE); ++n;
#ifdef LESSTIF_VERSION
    XtSetArg(args[n], XmNscrollBarDisplayPolicy, XmSTATIC); ++n;
#endif
    data->list[NAME] = XmCreateScrolledList(frame, "fontList", args, n);
    Xt_Va_Set_मूल्ये(name, XmNuserData, data->list[NAME], रिक्त);

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
	    रिक्त);

    str = XmStringCreateLocalized(_("Style:"));
    name = XtVaCreateManagedWidget("styleListLabel", xmLabelWidgetClass, frame,
	    XmNchildType, XmFRAME_TITLE_CHILD,
	    XmNchildVerticalAlignment, XmALIGNMENT_CENTER,
	    XmNchildHorizontalAlignment, XmALIGNMENT_BEGINNING,
	    XmNlabelString, str,
	    रिक्त);
    XmStringFree(str);
    apply_fontlist(name);

    n = 0;
    XtSetArg(args[n], Xm_Nvisible_Item_मोजणी, 8); ++n;
    XtSetArg(args[n], XmNresizable, True); ++n;
    XtSetArg(args[n], XmNlistSizePolicy, XmCONSTANT); ++n;
    XtSetArg(args[n], XmNvisualPolicy, XmVARIABLE); ++n;
#ifdef LESSTIF_VERSION
    XtSetArg(args[n], XmNscrollBarDisplayPolicy, XmSTATIC); ++n;
#endif
    data->list[STYLE] = XmCreateScrolledList(frame, "styleList", args, n);
    Xt_Va_Set_मूल्ये(name, XmNuserData, data->list[STYLE], रिक्त);

    // size list
    frame = XtVaCreateManagedWidget("frame", xmFrameWidgetClass, sub_form,
	    XmNshadowThickness, 0,
	    XmNtopAttachment, XmATTACH_FORM,
	    XmNbottomAttachment, XmATTACH_FORM,
	    XmNleftAttachment, XmATTACH_POSITION,
	    XmNleftPosition, 80,
	    XmNleftOffset, 4,
	    XmNrightAttachment, XmATTACH_FORM,
	    रिक्त);

    str = XmStringCreateLocalized(_("Size:"));
    name = XtVaCreateManagedWidget("sizeListLabel", xmLabelGadgetClass, frame,
	    XmNchildType, XmFRAME_TITLE_CHILD,
	    XmNchildVerticalAlignment, XmALIGNMENT_CENTER,
	    XmNchildHorizontalAlignment, XmALIGNMENT_BEGINNING,
	    XmNlabelString, str,
	    रिक्त);
    XmStringFree(str);
    apply_fontlist(name);

    n = 0;
    XtSetArg(args[n], Xm_Nvisible_Item_मोजणी, 8); ++n;
    XtSetArg(args[n], XmNresizable, True); ++n;
    XtSetArg(args[n], XmNlistSizePolicy, XmCONSTANT); ++n;
    XtSetArg(args[n], XmNvisualPolicy, XmVARIABLE); ++n;
#ifdef LESSTIF_VERSION
    XtSetArg(args[n], XmNscrollBarDisplayPolicy, XmSTATIC); ++n;
#endif
    data->list[SIZE] = XmCreateScrolledList(frame, "sizeList", args, n);
    Xt_Va_Set_मूल्ये(name, XmNuserData, data->list[SIZE], रिक्त);

    // update form widgets cancel button
    Xt_Va_Set_मूल्ये(form, XmNcancelButton, data->cancel, रिक्त);

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
    data->sel[ENCODING] = रिक्त;
    data->sel[NAME] = रिक्त;
    data->sel[STYLE] = रिक्त;
    data->sel[SIZE] = रिक्त;
    data->font_name = रिक्त;

    // set up current font parameters
    यदि (current && current[0] != '\0')
    {
	पूर्णाङ्क	    i;
	वर्ण	    **names;

	names = XListFonts(XtDisplay(form), (वर्ण *) current, 1, &i);

	यदि (i != 0)
	{
	    वर्ण namebuf[TEMP_BUF_SIZE];
	    वर्ण stylebuf[TEMP_BUF_SIZE];
	    वर्ण sizebuf[TEMP_BUF_SIZE];
	    वर्ण encodingbuf[TEMP_BUF_SIZE];
	    वर्ण *found;

	    found = names[0];

	    name_part(found, namebuf);
	    style_part(found, stylebuf);
	    size_part(found, sizebuf, data->in_pixels);
	    encoding_part(found, encodingbuf);

	    यदि (*namebuf != NUL
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
	    अन्यथा
	    {
		// We can't preset a symbolic name, which isn't a full font
		// description. Therefore we just behave the same way as if the
		// user didn't have selected anything thus far.
		//
		// Unfortunately there is no known way to expand an abbreviated
		// font name.
		data->font_name = रिक्त;
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
    यावत् (!data->exit)
	XtAppProcessEvent(XtWidgetToApplicationContext(data->dialog),
							(Xt_आदान_Mask)XtIMAll);

    यदि (data->old)
    {
	XFreeFont(XtDisplay(data->dialog),  data->old);
	XmFontListFree(data->old_list);
    }
    XtDestroyWidget(data->dialog);

    gui_motif_synch_fonts();

    प्रत्यागम (char_u *) data->font_name;
}
