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

#અન્તર્ભાવ "vim.h"

#અન્તર્ભાવ <Xm/Form.h>
#અન્તર્ભાવ <Xm/PushBG.h>
#અન્તર્ભાવ <Xm/Text.h>
#અન્તર્ભાવ <Xm/TextF.h>
#અન્તર્ભાવ <Xm/Label.h>
#અન્તર્ભાવ <Xm/Frame.h>
#અન્તર્ભાવ <Xm/LabelG.h>
#અન્તર્ભાવ <Xm/ToggleBG.h>
#અન્તર્ભાવ <Xm/SeparatoG.h>
#અન્તર્ભાવ <Xm/DialogS.h>
#અન્તર્ભાવ <Xm/List.h>
#અન્તર્ભાવ <Xm/RowColumn.h>
#અન્તર્ભાવ <Xm/AtomMgr.h>
#અન્તર્ભાવ <Xm/Protocols.h>

#અન્તર્ભાવ <X11/keysym.h>
#અન્તર્ભાવ <X11/Xatom.h>
#અન્તર્ભાવ <X11/StringDefs.h>
#અન્તર્ભાવ <X11/Intrinsic.h>

બાહ્ય Widget vimShell;

#ifdef FEAT_MENU
# define apply_fontlist(w) gui_motif_menu_fontlist(w)
#નહીંતર
# define apply_fontlist(w)
#endif

/////////////////////////////////////////////////////////////////////////////
// Font selection dialogue implementation.

સ્થિર અક્ષર wild[3] = "*";

/*
 * FIXME: This is a generic function, which should be used throughout the whole
 * application.
 *
 * Add close_callback, which will be called when the user selects close from
 * the window menu.  The close menu item usually activates f.kill which sends a
 * WM_DELETE_WINDOW protocol request for the window.
 */

    સ્થિર શૂન્ય
add_cancel_action(Widget shell, XtCallbackProc close_callback, શૂન્ય *arg)
{
    સ્થિર Atom wmp_atom = 0;
    સ્થિર Atom dw_atom = 0;
    Display *display = XtDisplay(shell);

    // deactivate the built-in delete response of killing the application
    Xt_Va_Set_કિંમતો(shell, XmNdeleteResponse, XmDO_NOTHING, ખાલી);

    // add a delete window protocol callback instead
    જો (!dw_atom)
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

ગણના ListSpecifier
{
    ENCODING,
    NAME,
    STYLE,
    SIZE,
    NONE
};

પ્રકારવ્યાખ્યા સંરચના _SharedFontSelData
{
    Widget	dialog;
    Widget	ok;
    Widget	cancel;
    Widget	encoding_pulldown;
    Widget	encoding_menu;
    Widget	list[NONE];
    Widget	name;
    Widget	sample;
    અક્ષર	**names;	// font name array of arrays
    પૂર્ણાંક		અંક;		// number of font names
    String	sel[NONE];	// selection category
    Boolean	in_pixels;	// toggle state - size in pixels
    અક્ષર	*font_name;	// current font name
    XFontStruct	*old;		// font data structure for sample display
    XmFontList	old_list;	// font data structure for sample display
    Boolean	exit;		// used for program exit control
} SharedFontSelData;

/*
 * Checking access to the font name array for validity.
 */
    સ્થિર અક્ષર *
fn(SharedFontSelData *data, પૂર્ણાંક i)
{
    // Assertion checks:
    જો (data->અંક < 0)
	abort();
    જો (i >= data->અંક)
	i = data->અંક - 1;
    જો (i < 0)
	i = 0;

    પાછા data->names[i];
}

/*
 * Get a specific substring from a font name.
 */
    સ્થિર શૂન્ય
get_part(અક્ષર *in, પૂર્ણાંક pos, અક્ષર *out)
{
    પૂર્ણાંક	i;
    પૂર્ણાંક j;

    *out = '\0';

    માટે (i = 0; (pos > 0) && (in[i] != '\0'); ++i)
	જો (in[i] == '-')
	    pos--;

    જો (in[i] == '\0')
	પાછા;

    માટે (j = 0; (in[i] != '-') && (in[i] != '\0'); ++i, ++j)
	out[j] = in[i];
    out[j] = '\0';
}

/*
 * Given a font name this function returns the part used in the first
 * scroll list.
 */
    સ્થિર શૂન્ય
name_part(અક્ષર *font, અક્ષર *buf)
{
    અક્ષર    buf2[TEMP_BUF_SIZE];
    અક્ષર    buf3[TEMP_BUF_SIZE];

    get_part(font, 2, buf2);
    get_part(font, 1, buf3);

    જો (*buf3 != NUL)
	vim_snprintf(buf, TEMP_BUF_SIZE, "%s (%s)", buf2, buf3);
    નહીંતર
	vim_snprintf(buf, TEMP_BUF_SIZE, "%s", buf2);
}

/*
 * Given a font name this function returns the part used in the second scroll list.
 */
    સ્થિર શૂન્ય
style_part(અક્ષર *font, અક્ષર *buf)
{
    અક્ષર    buf2[TEMP_BUF_SIZE];
    અક્ષર    buf3[TEMP_BUF_SIZE];

    get_part(font, 3, buf3);
    get_part(font, 5, buf2);

    જો (!strcmp(buf2, "normal") && !strcmp(buf2, "Normal")
						   && !strcmp(buf2, "NORMAL"))
	vim_snprintf(buf, TEMP_BUF_SIZE, "%s %s", buf3, buf2);
    નહીંતર
	strcpy(buf, buf3);

    get_part(font, 6, buf2);

    જો (buf2[0] != '\0')
	vim_snprintf(buf3, TEMP_BUF_SIZE, "%s %s", buf, buf2);
    નહીંતર
	strcpy(buf3, buf);

    get_part(font, 4, buf2);

    જો (!strcmp(buf2, "o") || !strcmp(buf2, "O"))
	vim_snprintf(buf, TEMP_BUF_SIZE, "%s oblique", buf3);
    નહીંતર જો (!strcmp(buf2, "i") || !strcmp(buf2, "I"))
	vim_snprintf(buf, TEMP_BUF_SIZE, "%s italic", buf3);

    જો (!strcmp(buf, " "))
	strcpy(buf, "-");
}

/*
 * Given a font name this function returns the part used in the third
 * scroll list.
 */
    સ્થિર શૂન્ય
size_part(અક્ષર *font, અક્ષર *buf, પૂર્ણાંક inPixels)
{
    પૂર્ણાંક	    size;
    દશાંશ   temp;

    *buf = '\0';

    જો (inPixels)
    {
	get_part(font, 7, buf);
	જો (*buf != NUL)
	{
	    size = atoi(buf);
	    sprintf(buf, "%3d", size);
	}
    }
    નહીંતર
    {
	get_part(font, 8, buf);
	જો (*buf != NUL)
	{
	    size = atoi(buf);
	    temp = (દશાંશ)size / 10.0;
	    size = temp;
	    જો (buf[strlen(buf) - 1] == '0')
		sprintf(buf, "%3d", size);
	    નહીંતર
		sprintf(buf, "%4.1f", temp);
	}
    }
}

/*
 * Given a font name this function returns the part used in the choice menu.
 */
    સ્થિર શૂન્ય
encoding_part(અક્ષર *font, અક્ષર *buf)
{
    અક્ષર    buf1[TEMP_BUF_SIZE];
    અક્ષર    buf2[TEMP_BUF_SIZE];

    *buf = '\0';

    get_part(font, 13, buf1);
    get_part(font, 14, buf2);

    જો (*buf1 != NUL && *buf2 != NUL)
	vim_snprintf(buf, TEMP_BUF_SIZE, "%s-%s", buf1, buf2);
    જો (!strcmp(buf, " "))
	strcpy(buf, "-");
}

/*
 * Inserts a string into correct sorted position in a list.
 */
    સ્થિર શૂન્ય
add_to_list(અક્ષર **buf, અક્ષર *item, પૂર્ણાંક *ગણતરી)
{
    પૂર્ણાંક	i;
    પૂર્ણાંક j;

    જો (*ગણતરી == MAX_ENTRIES_IN_LIST)
	પાછા;

    // avoid duplication
    માટે (i = 0; i < *ગણતરી; ++i)
    {
	જો (!strcmp(buf[i], item))
	    પાછા;
    }

    // find order place, but make sure that wild card comes first
    જો (!strcmp(item, wild))
	i = 0;
    નહીંતર
	માટે (i = 0; i < *ગણતરી; ++i)
	    જો (strcmp(buf[i], item) > 0 && strcmp(buf[i], wild))
		તોડો;

    // now insert the item
    માટે (j = *ગણતરી; j > i; --j)
	buf[j] = buf[j-1];
    buf[i] = XtNewString(item);

    ++(*ગણતરી);
}

/*
 * True if the font matches some field.
 */
    સ્થિર Boolean
match(SharedFontSelData *data, ગણના ListSpecifier l, પૂર્ણાંક i)
{
    અક્ષર buf[TEMP_BUF_SIZE];

    // An empty selection or a wild card matches anything.
    જો (!data->sel[l] || !strcmp(data->sel[l], wild))
	પાછા True;

    // chunk out the desired part...
    પસંદગી (l)
    {
	સ્થિતિ ENCODING:
	    encoding_part(fn(data, i), buf);
	    તોડો;

	સ્થિતિ NAME:
	    name_part(fn(data, i), buf);
	    તોડો;

	સ્થિતિ STYLE:
	    style_part(fn(data, i), buf);
	    તોડો;

	સ્થિતિ SIZE:
	    size_part(fn(data, i), buf, data->in_pixels);
	    તોડો;
	મૂળભૂત:
	    ;
    }

    // ...and chew it now

    પાછા !strcmp(buf, data->sel[l]);
}

    સ્થિર Boolean
proportional(અક્ષર *font)
{
    અક્ષર buf[TEMP_BUF_SIZE];

    get_part(font, 11, buf);

    પાછા !strcmp(buf, "p") || !strcmp(buf, "P");
}


સ્થિર શૂન્ય encoding_callback(Widget w, SharedFontSelData *data,
							     XtPointer dummy);

/*
 * Parse through the fontlist data and set up the three scroll lists.  The fix
 * parameter can be used to exclude a list from any changes.  This is used for
 * updates after selections caused by the users actions.
 */
    સ્થિર શૂન્ય
fill_lists(ગણના ListSpecifier fix, SharedFontSelData *data)
{
    અક્ષર	*list[NONE][MAX_ENTRIES_IN_LIST];
    પૂર્ણાંક		ગણતરી[NONE];
    અક્ષર	buf[TEMP_BUF_SIZE];
    XmString	વસ્તુઓ[MAX_ENTRIES_IN_LIST];
    પૂર્ણાંક		i;
    પૂર્ણાંક		idx;

    માટે (idx = (પૂર્ણાંક)ENCODING; idx < (પૂર્ણાંક)NONE; ++idx)
	ગણતરી[idx] = 0;

    // First we insert the wild char into every single list.
    જો (fix != ENCODING)
	add_to_list(list[ENCODING], wild, &ગણતરી[ENCODING]);
    જો (fix != NAME)
	add_to_list(list[NAME], wild, &ગણતરી[NAME]);
    જો (fix != STYLE)
	add_to_list(list[STYLE], wild, &ગણતરી[STYLE]);
    જો (fix != SIZE)
	add_to_list(list[SIZE], wild, &ગણતરી[SIZE]);

    માટે (i = 0; i < data->અંક && i < MAX_ENTRIES_IN_LIST; i++)
    {
	જો (proportional(fn(data, i)))
	    ચાલુ;

	જો (fix != ENCODING
		&& match(data, NAME, i)
		&& match(data, STYLE, i)
		&& match(data, SIZE, i))
	{
	    encoding_part(fn(data, i), buf);
	    add_to_list(list[ENCODING], buf, &ગણતરી[ENCODING]);
	}

	જો (fix != NAME
		&& match(data, ENCODING, i)
		&& match(data, STYLE, i)
		&& match(data, SIZE, i))
	{
	    name_part(fn(data, i), buf);
	    add_to_list(list[NAME], buf, &ગણતરી[NAME]);
	}

	જો (fix != STYLE
		&& match(data, ENCODING, i)
		&& match(data, NAME, i)
		&& match(data, SIZE, i))
	{
	    style_part(fn(data, i), buf);
	    add_to_list(list[STYLE], buf, &ગણતરી[STYLE]);
	}

	જો (fix != SIZE
		&& match(data, ENCODING, i)
		&& match(data, NAME, i)
		&& match(data, STYLE, i))
	{
	    size_part(fn(data, i), buf, data->in_pixels);
	    add_to_list(list[SIZE], buf, &ગણતરી[SIZE]);
	}
    }

    /*
     * And now do the preselection in all lists where there was one:
     */

    જો (fix != ENCODING)
    {
	Cardinal n_વસ્તુઓ;
	WidgetList children;
	Widget selected_button = 0;

	// Get and update the current button list.
	Xt_Va_Get_કિંમતો(data->encoding_pulldown,
		XmNchildren, &children,
		XmNnumChildren, &n_વસ્તુઓ,
		ખાલી);

	માટે (i = 0; i < ગણતરી[ENCODING]; ++i)
	{
	    Widget button;

	    વસ્તુઓ[i] = XmStringCreateLocalized(list[ENCODING][i]);

	    જો (i < (પૂર્ણાંક)n_વસ્તુઓ)
	    {
		// recycle old button
		Xt_Va_Set_કિંમતો(children[i],
			XmNlabelString, વસ્તુઓ[i],
			XmNuserData, i,
			ખાલી);
		button = children[i];
	    }
	    નહીંતર
	    {
		// create a new button
		button = XtVaCreateManagedWidget("button",
			xm_ધકેલો_Button_Gadget_Class,
			data->encoding_pulldown,
			XmNlabelString, વસ્તુઓ[i],
			XmNuserData, i,
			ખાલી);
		XtAddCallback(button, XmNactivateCallback,
			(XtCallbackProc) encoding_callback, (XtPointer) data);
		XtManageChild(button);
	    }

	    જો (data->sel[ENCODING])
	    {
		જો (!strcmp(data->sel[ENCODING], list[ENCODING][i]))
		    selected_button = button;
	    }
	    XtFree(list[ENCODING][i]);
	}

	// Destroy all the outstanding menu items.
	માટે (i = ગણતરી[ENCODING]; i < (પૂર્ણાંક)n_વસ્તુઓ; ++i)
	{
	    XtUnmanageChild(children[i]);
	    XtDestroyWidget(children[i]);
	}

	// Preserve the current selection visually.
	જો (selected_button)
	{
	    Xt_Va_Set_કિંમતો(data->encoding_menu,
		    XmNmenuHistory, selected_button,
		    ખાલી);
	}

	માટે (i = 0; i < ગણતરી[ENCODING]; ++i)
	    XmStringFree(વસ્તુઓ[i]);
    }

    /*
     * Now loop through the remaining lists and set them up.
     */
    માટે (idx = (પૂર્ણાંક)NAME; idx < (પૂર્ણાંક)NONE; ++idx)
    {
	Widget w;

	જો (fix == (ગણના ListSpecifier)idx)
	    ચાલુ;

	પસંદગી ((ગણના ListSpecifier)idx)
	{
	    સ્થિતિ NAME:
		w = data->list[NAME];
		તોડો;
	    સ્થિતિ STYLE:
		w = data->list[STYLE];
		તોડો;
	    સ્થિતિ SIZE:
		w = data->list[SIZE];
		તોડો;
	    મૂળભૂત:
		w = (Widget)0;	// for lint
	}

	માટે (i = 0; i < ગણતરી[idx]; ++i)
	{
	    વસ્તુઓ[i] = XmStringCreateLocalized(list[idx][i]);
	    XtFree(list[idx][i]);
	}
	Xm_List_Delete_All_વસ્તુઓ(w);
	Xm_List_Add_વસ્તુઓ(w, વસ્તુઓ, ગણતરી[idx], 1);
	જો (data->sel[idx])
	{
	    XmStringFree(વસ્તુઓ[0]);
	    વસ્તુઓ[0] = XmStringCreateLocalized(data->sel[idx]);
	    XmListSelectItem(w, વસ્તુઓ[0], False);
	    XmListSetBottomItem(w, વસ્તુઓ[0]);
	}
	માટે (i = 0; i < ગણતરી[idx]; ++i)
	    XmStringFree(વસ્તુઓ[i]);
    }
}

    સ્થિર શૂન્ય
stoggle_callback(Widget w UNUSED,
	SharedFontSelData *data,
	XmToggleButtonCallbackStruct *call_data)
{
    પૂર્ણાંક		i, do_sel;
    અક્ષર	newSize[TEMP_BUF_SIZE];
    XmString	str;

    જો (call_data->reason != (પૂર્ણાંક)Xm_CR_કિંમત_CHANGED)
	પાછા;

    do_sel = (data->sel[SIZE] != ખાલી) && strcmp(data->sel[SIZE], wild);

    માટે (i = 0; do_sel && (i < data->અંક); i++)
	જો (match(data, ENCODING, i)
		&& match(data, NAME, i)
		&& match(data, STYLE, i)
		&& match(data, SIZE, i))
	{
	    size_part(fn(data, i), newSize, !data->in_pixels);
	    તોડો;
	}

    data->in_pixels = !data->in_pixels;

    જો (data->sel[SIZE])
	XtFree(data->sel[SIZE]);
    data->sel[SIZE] = ખાલી;
    fill_lists(NONE, data);

    જો (do_sel)
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
    સ્થિર શૂન્ય
display_sample(SharedFontSelData *data)
{
    Arg		    args[2];
    પૂર્ણાંક		    n;
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

    Xt_Set_કિંમતો(data->sample, args, n);
    XmStringFree(str);

    જો (data->old)
    {
	XFreeFont(display, data->old);
	XmFontListFree(data->old_list);
    }
    data->old = font;
    data->old_list = font_list;
}


    સ્થિર Boolean
do_choice(Widget w,
	SharedFontSelData *data,
	XmListCallbackStruct *call_data,
	ગણના ListSpecifier which)
{
    અક્ષર *sel;

    XmStringGetLtoR(call_data->item, XmSTRING_DEFAULT_CHARSET, &sel);

    જો (!data->sel[which])
	data->sel[which] = XtNewString(sel);
    નહીંતર
    {
	જો (!strcmp(data->sel[which], sel))
	{
	    // unselecting current selection
	    XtFree(data->sel[which]);
	    data->sel[which] = ખાલી;
	    જો (w)
		XmListDeselectItem(w, call_data->item);
	}
	નહીંતર
	{
	    XtFree(data->sel[which]);
	    data->sel[which] = XtNewString(sel);
	}
    }
    XtFree(sel);

    fill_lists(which, data);

    // If there is a font selection, we display it.
    જો (data->sel[ENCODING]
	    && data->sel[NAME]
	    && data->sel[STYLE]
	    && data->sel[SIZE]
	    && strcmp(data->sel[ENCODING], wild)
	    && strcmp(data->sel[NAME], wild)
	    && strcmp(data->sel[STYLE], wild)
	    && strcmp(data->sel[SIZE], wild))
    {
	પૂર્ણાંક i;

	જો (data->font_name)
	    XtFree(data->font_name);
	data->font_name = ખાલી;

	માટે (i = 0; i < data->અંક; i++)
	{
	    જો (match(data, ENCODING, i)
		    && match(data, NAME, i)
		    && match(data, STYLE, i)
		    && match(data, SIZE, i))
	    {
		data->font_name = XtNewString(fn(data, i));
		તોડો;
	    }
	}

	જો (data->font_name)
	{
	    XmTextSetString(data->name, data->font_name);
	    display_sample(data);
	}
	નહીંતર
	    do_dialog(VIM_ERROR,
		    (char_u *)_("Error"),
		    (char_u *)_("Invalid font specification"),
		    (char_u *)_("&Dismiss"), 1, ખાલી, FALSE);

	પાછા True;
    }
    નહીંતર
    {
	પૂર્ણાંક	    n;
	XmString    str;
	Arg	    args[4];
	અક્ષર	    *nomatch_સંદેશ = _("no specific match");

	n = 0;
	str = XmStringCreateLocalized(nomatch_સંદેશ);
	XtSetArg(args[n], XmNlabelString, str); ++n;
	Xt_Set_કિંમતો(data->sample, args, n);
	apply_fontlist(data->sample);
	XmTextSetString(data->name, nomatch_સંદેશ);
	XmStringFree(str);

	પાછા False;
    }
}

    સ્થિર શૂન્ય
encoding_callback(Widget w,
	SharedFontSelData *data,
	XtPointer dummy UNUSED)
{
    XmString str;
    XmListCallbackStruct fake_data;

    Xt_Va_Get_કિંમતો(w, XmNlabelString, &str, ખાલી);

    જો (!str)
	પાછા;

    fake_data.item = str;

    do_choice(0, data, &fake_data, ENCODING);
}

    સ્થિર શૂન્ય
name_callback(Widget w,
	SharedFontSelData *data,
	XmListCallbackStruct *call_data)
{
    do_choice(w, data, call_data, NAME);
}

    સ્થિર શૂન્ય
style_callback(Widget w,
	SharedFontSelData *data,
	XmListCallbackStruct *call_data)
{
    do_choice(w, data, call_data, STYLE);
}

    સ્થિર શૂન્ય
size_callback(Widget w,
	SharedFontSelData *data,
	XmListCallbackStruct *call_data)
{
    do_choice(w, data, call_data, SIZE);
}

    સ્થિર શૂન્ય
cancel_callback(Widget w UNUSED,
	SharedFontSelData *data,
	XmListCallbackStruct *call_data UNUSED)
{
    જો (data->sel[ENCODING])
    {
	XtFree(data->sel[ENCODING]);
	data->sel[ENCODING] = ખાલી;
    }
    જો (data->sel[NAME])
    {
	XtFree(data->sel[NAME]);
	data->sel[NAME] = ખાલી;
    }
    જો (data->sel[STYLE])
    {
	XtFree(data->sel[STYLE]);
	data->sel[STYLE] = ખાલી;
    }
    જો (data->sel[SIZE])
    {
	XtFree(data->sel[SIZE]);
	data->sel[SIZE] = ખાલી;
    }

    જો (data->font_name)
	XtFree(data->font_name);
    data->font_name = ખાલી;

    data->અંક = 0;
    XFreeFontNames(data->names);
    data->names = ખાલી;
    data->exit = True;
}

    સ્થિર શૂન્ય
ok_callback(Widget w UNUSED,
	SharedFontSelData *data,
	Xm_ધકેલો_Button_Callback_Struct *call_data UNUSED)
{
    અક્ષર    *pattern;
    અક્ષર    **name;
    પૂર્ણાંક	    i;

    pattern = XmTextGetString(data->name);
    name    = XListFonts(XtDisplay(data->dialog), pattern, 1, &i);
    XtFree(pattern);

    જો (i != 1)
    {
	do_dialog(VIM_ERROR,
		(char_u *)_("Error"),
		(char_u *)_("Invalid font specification"),
		(char_u *)_("&Dismiss"), 1, ખાલી, FALSE);
	XFreeFontNames(name);
    }
    નહીંતર
    {
	જો (data->font_name)
	    XtFree(data->font_name);
	data->font_name = XtNewString(name[0]);

	જો (data->sel[ENCODING])
	{
	    XtFree(data->sel[ENCODING]);
	    data->sel[ENCODING] = ખાલી;
	}
	જો (data->sel[NAME])
	{
	    XtFree(data->sel[NAME]);
	    data->sel[NAME] = ખાલી;
	}
	જો (data->sel[STYLE])
	{
	    XtFree(data->sel[STYLE]);
	    data->sel[STYLE] = ખાલી;
	}
	જો (data->sel[SIZE])
	{
	    XtFree(data->sel[SIZE]);
	    data->sel[SIZE] = ખાલી;
	}

	XFreeFontNames(name);

	data->અંક = 0;
	XFreeFontNames(data->names);
	data->names = ખાલી;
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
    સ્થિર SharedFontSelData	_data;

    Widget		parent;
    Widget		form;
    Widget		separator;
    Widget		sub_form;
    Widget		size_toggle;
    Widget		name;
    Widget		disp_frame;
    Widget		frame;
    Arg			args[64];
    પૂર્ણાંક			n;
    XmString		str;
    અક્ષર		big_font[MAX_FONT_NAME_LEN];
    SharedFontSelData	*data;

    data = &_data;

    parent = vimShell;
    data->names = XListFonts(XtDisplay(parent), "-*-*-*-*-*-*-*-*-*-*-*-*-*-*",
						       MAX_FONTS, &data->અંક);

    /*
     * Find the name of the biggest font less than the given limit
     * MAX_DISPLAY_SIZE used to set up the initial height of the display
     * widget.
     */

    {
	પૂર્ણાંક	i;
	પૂર્ણાંક	max;
	પૂર્ણાંક	idx = 0;
	પૂર્ણાંક	size;
	અક્ષર	buf[128];

	માટે (i = 0, max = 0; i < data->અંક; i++)
	{
	    get_part(fn(data, i), 7, buf);
	    size = atoi(buf);
	    જો ((size > max) && (size < MAX_DISPLAY_SIZE))
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
    data->dialog = XmCreateDialogShell(parent, "fontSelector", ખાલી, 0);
    str = XmStringCreateLocalized(_("Vim - Font Selector"));

    // Create form popup dialog widget.
    form = XtVaCreateWidget("form",
	    xmFormWidgetClass, data->dialog,
	    XmNdialogTitle, str,
	    XmNautoUnmanage, False,
	    XmNdialogStyle, XmDIALOG_FULL_APPLICATION_MODAL,
	    ખાલી);
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
	    ખાલી);

    data->ok = XtVaCreateManagedWidget(_("OK"),
	    xm_ધકેલો_Button_Gadget_Class, sub_form,
	    XmNleftAttachment, XmATTACH_FORM,
	    XmNrightAttachment, XmATTACH_FORM,
	    XmNtopAttachment, XmATTACH_FORM,
	    XmNtopOffset, 4,
	    ખાલી);
    apply_fontlist(data->ok);

    data->cancel = XtVaCreateManagedWidget(_("Cancel"),
	    xm_ધકેલો_Button_Gadget_Class, sub_form,
	    XmNrightAttachment, XmATTACH_FORM,
	    XmNleftAttachment, XmATTACH_FORM,
	    XmNtopAttachment, XmATTACH_WIDGET,
	    XmNtopWidget, data->ok,
	    XmNtopOffset, 4,
	    XmNshowAsDefault, True,
	    ખાલી);
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
	    XmNeditMode, Xm_SINGLE_લીટી_EDIT,
	    XmNmaxLength, MAX_FONT_NAME_LEN,
	    XmNcolumns, 60,
	    ખાલી);

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
	    ખાલી);
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
	    ખાલી);

    data->sample = XtVaCreateManagedWidget("sampleLabel",
	    xmLabelWidgetClass, disp_frame,
	    XmNleftAttachment, XmATTACH_FORM,
	    XmNtopAttachment, XmATTACH_FORM,
	    XmNbottomAttachment, XmATTACH_FORM,
	    XmNrightAttachment, XmATTACH_FORM,
	    XmNalignment, XmALIGNMENT_BEGINNING,
	    XmNrecomputeSize, False,
	    XmNfontList, data->old_list,
	    ખાલી);

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
	    ખાલી);
    XmStringFree(str);
    apply_fontlist(size_toggle);
    XtManageChild(size_toggle);

    // Encoding pulldown menu.

    data->encoding_pulldown = XmCreatePulldownMenu(form,
						 "encodingPulldown", ખાલી, 0);
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
	    ખાલી);

    // font list
    frame = XtVaCreateManagedWidget("frame", xmFrameWidgetClass, sub_form,
	    XmNshadowThickness, 0,
	    XmNtopAttachment, XmATTACH_FORM,
	    XmNbottomAttachment, XmATTACH_FORM,
	    XmNleftAttachment, XmATTACH_FORM,
	    XmNrightAttachment, XmATTACH_POSITION,
	    XmNrightPosition, 50,
	    ખાલી);

    str = XmStringCreateLocalized(_("Font:"));
    name = XtVaCreateManagedWidget("nameListLabel", xmLabelGadgetClass, frame,
	    XmNchildType, XmFRAME_TITLE_CHILD,
	    XmNchildVerticalAlignment, XmALIGNMENT_CENTER,
	    XmNchildHorizontalAlignment, XmALIGNMENT_BEGINNING,
	    XmNlabelString, str,
	    ખાલી);
    XmStringFree(str);
    apply_fontlist(name);

    n = 0;
    XtSetArg(args[n], Xm_Nvisible_Item_ગણતરી, 8); ++n;
    XtSetArg(args[n], XmNresizable, True); ++n;
    XtSetArg(args[n], XmNlistSizePolicy, XmCONSTANT); ++n;
    XtSetArg(args[n], XmNvisualPolicy, XmVARIABLE); ++n;
#ifdef LESSTIF_VERSION
    XtSetArg(args[n], XmNscrollBarDisplayPolicy, XmSTATIC); ++n;
#endif
    data->list[NAME] = XmCreateScrolledList(frame, "fontList", args, n);
    Xt_Va_Set_કિંમતો(name, XmNuserData, data->list[NAME], ખાલી);

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
	    ખાલી);

    str = XmStringCreateLocalized(_("Style:"));
    name = XtVaCreateManagedWidget("styleListLabel", xmLabelWidgetClass, frame,
	    XmNchildType, XmFRAME_TITLE_CHILD,
	    XmNchildVerticalAlignment, XmALIGNMENT_CENTER,
	    XmNchildHorizontalAlignment, XmALIGNMENT_BEGINNING,
	    XmNlabelString, str,
	    ખાલી);
    XmStringFree(str);
    apply_fontlist(name);

    n = 0;
    XtSetArg(args[n], Xm_Nvisible_Item_ગણતરી, 8); ++n;
    XtSetArg(args[n], XmNresizable, True); ++n;
    XtSetArg(args[n], XmNlistSizePolicy, XmCONSTANT); ++n;
    XtSetArg(args[n], XmNvisualPolicy, XmVARIABLE); ++n;
#ifdef LESSTIF_VERSION
    XtSetArg(args[n], XmNscrollBarDisplayPolicy, XmSTATIC); ++n;
#endif
    data->list[STYLE] = XmCreateScrolledList(frame, "styleList", args, n);
    Xt_Va_Set_કિંમતો(name, XmNuserData, data->list[STYLE], ખાલી);

    // size list
    frame = XtVaCreateManagedWidget("frame", xmFrameWidgetClass, sub_form,
	    XmNshadowThickness, 0,
	    XmNtopAttachment, XmATTACH_FORM,
	    XmNbottomAttachment, XmATTACH_FORM,
	    XmNleftAttachment, XmATTACH_POSITION,
	    XmNleftPosition, 80,
	    XmNleftOffset, 4,
	    XmNrightAttachment, XmATTACH_FORM,
	    ખાલી);

    str = XmStringCreateLocalized(_("Size:"));
    name = XtVaCreateManagedWidget("sizeListLabel", xmLabelGadgetClass, frame,
	    XmNchildType, XmFRAME_TITLE_CHILD,
	    XmNchildVerticalAlignment, XmALIGNMENT_CENTER,
	    XmNchildHorizontalAlignment, XmALIGNMENT_BEGINNING,
	    XmNlabelString, str,
	    ખાલી);
    XmStringFree(str);
    apply_fontlist(name);

    n = 0;
    XtSetArg(args[n], Xm_Nvisible_Item_ગણતરી, 8); ++n;
    XtSetArg(args[n], XmNresizable, True); ++n;
    XtSetArg(args[n], XmNlistSizePolicy, XmCONSTANT); ++n;
    XtSetArg(args[n], XmNvisualPolicy, XmVARIABLE); ++n;
#ifdef LESSTIF_VERSION
    XtSetArg(args[n], XmNscrollBarDisplayPolicy, XmSTATIC); ++n;
#endif
    data->list[SIZE] = XmCreateScrolledList(frame, "sizeList", args, n);
    Xt_Va_Set_કિંમતો(name, XmNuserData, data->list[SIZE], ખાલી);

    // update form widgets cancel button
    Xt_Va_Set_કિંમતો(form, XmNcancelButton, data->cancel, ખાલી);

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
    data->sel[ENCODING] = ખાલી;
    data->sel[NAME] = ખાલી;
    data->sel[STYLE] = ખાલી;
    data->sel[SIZE] = ખાલી;
    data->font_name = ખાલી;

    // set up current font parameters
    જો (current && current[0] != '\0')
    {
	પૂર્ણાંક	    i;
	અક્ષર	    **names;

	names = XListFonts(XtDisplay(form), (અક્ષર *) current, 1, &i);

	જો (i != 0)
	{
	    અક્ષર namebuf[TEMP_BUF_SIZE];
	    અક્ષર stylebuf[TEMP_BUF_SIZE];
	    અક્ષર sizebuf[TEMP_BUF_SIZE];
	    અક્ષર encodingbuf[TEMP_BUF_SIZE];
	    અક્ષર *found;

	    found = names[0];

	    name_part(found, namebuf);
	    style_part(found, stylebuf);
	    size_part(found, sizebuf, data->in_pixels);
	    encoding_part(found, encodingbuf);

	    જો (*namebuf != NUL
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
	    નહીંતર
	    {
		// We can't preset a symbolic name, which isn't a full font
		// description. Therefore we just behave the same way as if the
		// user didn't have selected anything thus far.
		//
		// Unfortunately there is no known way to expand an abbreviated
		// font name.
		data->font_name = ખાલી;
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
    જ્યારે (!data->exit)
	XtAppProcessEvent(XtWidgetToApplicationContext(data->dialog),
							(Xt_આગત_Mask)XtIMAll);

    જો (data->old)
    {
	XFreeFont(XtDisplay(data->dialog),  data->old);
	XmFontListFree(data->old_list);
    }
    XtDestroyWidget(data->dialog);

    gui_motif_synch_fonts();

    પાછા (char_u *) data->font_name;
}
