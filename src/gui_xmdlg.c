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

#include "vim.h"

#include <Xm/Form.h>
#include <Xm/PushBG.h>
#include <Xm/Text.h>
#include <Xm/TextF.h>
#include <Xm/Label.h>
#include <Xm/Frame.h>
#include <Xm/LabelG.h>
#include <Xm/ToggleBG.h>
#include <Xm/SeparatoG.h>
#include <Xm/DialogS.h>
#include <Xm/List.h>
#include <Xm/RowColumn.h>
#include <Xm/AtomMgr.h>
#include <Xm/Protocols.h>

#include <X11/keysym.h>
#include <X11/Xatom.h>
#include <X11/StringDefs.h>
#include <X11/Intrinsic.h>

extern Widget vimShell;

#ifdef FEAT_MENU
# define apply_fontlist(w) gui_motif_menu_fontlist(w)
#ꦭꦶꦪ
# define apply_fontlist(w)
#endif

/////////////////////////////////////////////////////////////////////////////
// Font selection dialogue implementation.

static var_na wild[3] = "*";

/*
 * FIXME: This is a generic function, which should be used throughout the whole
 * application.
 *
 * Add close_callback, which will be called when the user selects close from
 * the window menu.  The close menu item usually activates f.kill which sends a
 * WM_DELETE_WINDOW protocol request for the window.
 */

    static void
add_cancel_action(Widget shell, XtCallbackProc close_callback, void *arg)
{
    static Atom wmp_atom = 0;
    static Atom dw_atom = 0;
    Display *display = XtDisplay(shell);

    // deactivate the built-in delete response of killing the application
    XtVaSetValues(shell, XmNdeleteResponse, XmDO_NOTHING, NULL);

    // add a delete window protocol callback instead
    ꦪꦼꦤ꧀ (!dw_atom)
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

ga_nanaa ListSpecifier
{
    ENCODING,
    NAME,
    STYLE,
    SIZE,
    NONE
};

typedef sa_nracanaa _SharedFontSelData
{
    Widget	dialog;
    Widget	ok;
    Widget	cancel;
    Widget	encoding_pulldown;
    Widget	encoding_menu;
    Widget	list[NONE];
    Widget	name;
    Widget	sample;
    var_na	**names;	// font name array of arrays
    puur_naa_nka		num;		// number of font names
    String	sel[NONE];	// selection category
    Boolean	in_pixels;	// toggle state - size in pixels
    var_na	*font_name;	// current font name
    XFontStruct	*old;		// font data structure for sample display
    XmFontList	old_list;	// font data structure for sample display
    Boolean	exit;		// used for program exit control
} SharedFontSelData;

/*
 * Checking access to the font name array for validity.
 */
    static var_na *
fn(SharedFontSelData *data, puur_naa_nka i)
{
    // Assertion checks:
    ꦪꦼꦤ꧀ (data->num < 0)
	abort();
    ꦪꦼꦤ꧀ (i >= data->num)
	i = data->num - 1;
    ꦪꦼꦤ꧀ (i < 0)
	i = 0;

    ꦮꦭꦶ data->names[i];
}

/*
 * Get a specific substring from a font name.
 */
    static void
get_part(var_na *in, puur_naa_nka pos, var_na *out)
{
    puur_naa_nka	i;
    puur_naa_nka j;

    *out = '\0';

    ꦏꦁꦒꦺꦴ (i = 0; (pos > 0) && (in[i] != '\0'); ++i)
	ꦪꦼꦤ꧀ (in[i] == '-')
	    pos--;

    ꦪꦼꦤ꧀ (in[i] == '\0')
	ꦮꦭꦶ;

    ꦏꦁꦒꦺꦴ (j = 0; (in[i] != '-') && (in[i] != '\0'); ++i, ++j)
	out[j] = in[i];
    out[j] = '\0';
}

/*
 * Given a font name this function returns the part used in the first
 * scroll list.
 */
    static void
name_part(var_na *font, var_na *buf)
{
    var_na    buf2[TEMP_BUF_SIZE];
    var_na    buf3[TEMP_BUF_SIZE];

    get_part(font, 2, buf2);
    get_part(font, 1, buf3);

    ꦪꦼꦤ꧀ (*buf3 != NUL)
	vim_snprintf(buf, TEMP_BUF_SIZE, "%s (%s)", buf2, buf3);
    ꦭꦶꦪ
	vim_snprintf(buf, TEMP_BUF_SIZE, "%s", buf2);
}

/*
 * Given a font name this function returns the part used in the second scroll list.
 */
    static void
style_part(var_na *font, var_na *buf)
{
    var_na    buf2[TEMP_BUF_SIZE];
    var_na    buf3[TEMP_BUF_SIZE];

    get_part(font, 3, buf3);
    get_part(font, 5, buf2);

    ꦪꦼꦤ꧀ (!strcmp(buf2, "normal") && !strcmp(buf2, "Normal")
						   && !strcmp(buf2, "NORMAL"))
	vim_snprintf(buf, TEMP_BUF_SIZE, "%s %s", buf3, buf2);
    ꦭꦶꦪ
	strcpy(buf, buf3);

    get_part(font, 6, buf2);

    ꦪꦼꦤ꧀ (buf2[0] != '\0')
	vim_snprintf(buf3, TEMP_BUF_SIZE, "%s %s", buf, buf2);
    ꦭꦶꦪ
	strcpy(buf3, buf);

    get_part(font, 4, buf2);

    ꦪꦼꦤ꧀ (!strcmp(buf2, "o") || !strcmp(buf2, "O"))
	vim_snprintf(buf, TEMP_BUF_SIZE, "%s oblique", buf3);
    ꦭꦶꦪ ꦪꦼꦤ꧀ (!strcmp(buf2, "i") || !strcmp(buf2, "I"))
	vim_snprintf(buf, TEMP_BUF_SIZE, "%s italic", buf3);

    ꦪꦼꦤ꧀ (!strcmp(buf, " "))
	strcpy(buf, "-");
}

/*
 * Given a font name this function returns the part used in the third
 * scroll list.
 */
    static void
size_part(var_na *font, var_na *buf, puur_naa_nka inPixels)
{
    puur_naa_nka	    size;
    float   temp;

    *buf = '\0';

    ꦪꦼꦤ꧀ (inPixels)
    {
	get_part(font, 7, buf);
	ꦪꦼꦤ꧀ (*buf != NUL)
	{
	    size = atoi(buf);
	    sprintf(buf, "%3d", size);
	}
    }
    ꦭꦶꦪ
    {
	get_part(font, 8, buf);
	ꦪꦼꦤ꧀ (*buf != NUL)
	{
	    size = atoi(buf);
	    temp = (float)size / 10.0;
	    size = temp;
	    ꦪꦼꦤ꧀ (buf[strlen(buf) - 1] == '0')
		sprintf(buf, "%3d", size);
	    ꦭꦶꦪ
		sprintf(buf, "%4.1f", temp);
	}
    }
}

/*
 * Given a font name this function returns the part used in the choice menu.
 */
    static void
encoding_part(var_na *font, var_na *buf)
{
    var_na    buf1[TEMP_BUF_SIZE];
    var_na    buf2[TEMP_BUF_SIZE];

    *buf = '\0';

    get_part(font, 13, buf1);
    get_part(font, 14, buf2);

    ꦪꦼꦤ꧀ (*buf1 != NUL && *buf2 != NUL)
	vim_snprintf(buf, TEMP_BUF_SIZE, "%s-%s", buf1, buf2);
    ꦪꦼꦤ꧀ (!strcmp(buf, " "))
	strcpy(buf, "-");
}

/*
 * Inserts a string into correct sorted position in a list.
 */
    static void
add_to_list(var_na **buf, var_na *item, puur_naa_nka *count)
{
    puur_naa_nka	i;
    puur_naa_nka j;

    ꦪꦼꦤ꧀ (*count == MAX_ENTRIES_IN_LIST)
	ꦮꦭꦶ;

    // avoid duplication
    ꦏꦁꦒꦺꦴ (i = 0; i < *count; ++i)
    {
	ꦪꦼꦤ꧀ (!strcmp(buf[i], item))
	    ꦮꦭꦶ;
    }

    // find order place, but make sure that wild card comes first
    ꦪꦼꦤ꧀ (!strcmp(item, wild))
	i = 0;
    ꦭꦶꦪ
	ꦏꦁꦒꦺꦴ (i = 0; i < *count; ++i)
	    ꦪꦼꦤ꧀ (strcmp(buf[i], item) > 0 && strcmp(buf[i], wild))
		bha_nga;

    // now insert the item
    ꦏꦁꦒꦺꦴ (j = *count; j > i; --j)
	buf[j] = buf[j-1];
    buf[i] = XtNewString(item);

    ++(*count);
}

/*
 * True if the font matches some field.
 */
    static Boolean
match(SharedFontSelData *data, ga_nanaa ListSpecifier l, puur_naa_nka i)
{
    var_na buf[TEMP_BUF_SIZE];

    // An empty selection or a wild card matches anything.
    ꦪꦼꦤ꧀ (!data->sel[l] || !strcmp(data->sel[l], wild))
	ꦮꦭꦶ True;

    // chunk out the desired part...
    switch (l)
    {
	pak_sa ENCODING:
	    encoding_part(fn(data, i), buf);
	    bha_nga;

	pak_sa NAME:
	    name_part(fn(data, i), buf);
	    bha_nga;

	pak_sa STYLE:
	    style_part(fn(data, i), buf);
	    bha_nga;

	pak_sa SIZE:
	    size_part(fn(data, i), buf, data->in_pixels);
	    bha_nga;
	default:
	    ;
    }

    // ...and chew it now

    ꦮꦭꦶ !strcmp(buf, data->sel[l]);
}

    static Boolean
proportional(var_na *font)
{
    var_na buf[TEMP_BUF_SIZE];

    get_part(font, 11, buf);

    ꦮꦭꦶ !strcmp(buf, "p") || !strcmp(buf, "P");
}


static void encoding_callback(Widget w, SharedFontSelData *data,
							     XtPointer dummy);

/*
 * Parse through the fontlist data and set up the three scroll lists.  The fix
 * parameter can be used to exclude a list from any changes.  This is used for
 * updates after selections caused by the users actions.
 */
    static void
fill_lists(ga_nanaa ListSpecifier fix, SharedFontSelData *data)
{
    var_na	*list[NONE][MAX_ENTRIES_IN_LIST];
    puur_naa_nka		count[NONE];
    var_na	buf[TEMP_BUF_SIZE];
    XmString	items[MAX_ENTRIES_IN_LIST];
    puur_naa_nka		i;
    puur_naa_nka		idx;

    ꦏꦁꦒꦺꦴ (idx = (puur_naa_nka)ENCODING; idx < (puur_naa_nka)NONE; ++idx)
	count[idx] = 0;

    // First we insert the wild char into every single list.
    ꦪꦼꦤ꧀ (fix != ENCODING)
	add_to_list(list[ENCODING], wild, &count[ENCODING]);
    ꦪꦼꦤ꧀ (fix != NAME)
	add_to_list(list[NAME], wild, &count[NAME]);
    ꦪꦼꦤ꧀ (fix != STYLE)
	add_to_list(list[STYLE], wild, &count[STYLE]);
    ꦪꦼꦤ꧀ (fix != SIZE)
	add_to_list(list[SIZE], wild, &count[SIZE]);

    ꦏꦁꦒꦺꦴ (i = 0; i < data->num && i < MAX_ENTRIES_IN_LIST; i++)
    {
	ꦪꦼꦤ꧀ (proportional(fn(data, i)))
	    continue;

	ꦪꦼꦤ꧀ (fix != ENCODING
		&& match(data, NAME, i)
		&& match(data, STYLE, i)
		&& match(data, SIZE, i))
	{
	    encoding_part(fn(data, i), buf);
	    add_to_list(list[ENCODING], buf, &count[ENCODING]);
	}

	ꦪꦼꦤ꧀ (fix != NAME
		&& match(data, ENCODING, i)
		&& match(data, STYLE, i)
		&& match(data, SIZE, i))
	{
	    name_part(fn(data, i), buf);
	    add_to_list(list[NAME], buf, &count[NAME]);
	}

	ꦪꦼꦤ꧀ (fix != STYLE
		&& match(data, ENCODING, i)
		&& match(data, NAME, i)
		&& match(data, SIZE, i))
	{
	    style_part(fn(data, i), buf);
	    add_to_list(list[STYLE], buf, &count[STYLE]);
	}

	ꦪꦼꦤ꧀ (fix != SIZE
		&& match(data, ENCODING, i)
		&& match(data, NAME, i)
		&& match(data, STYLE, i))
	{
	    size_part(fn(data, i), buf, data->in_pixels);
	    add_to_list(list[SIZE], buf, &count[SIZE]);
	}
    }

    /*
     * And now do the preselection in all lists where there was one:
     */

    ꦪꦼꦤ꧀ (fix != ENCODING)
    {
	Cardinal n_items;
	WidgetList children;
	Widget selected_button = 0;

	// Get and update the current button list.
	XtVaGetValues(data->encoding_pulldown,
		XmNchildren, &children,
		XmNnumChildren, &n_items,
		NULL);

	ꦏꦁꦒꦺꦴ (i = 0; i < count[ENCODING]; ++i)
	{
	    Widget button;

	    items[i] = XmStringCreateLocalized(list[ENCODING][i]);

	    ꦪꦼꦤ꧀ (i < (puur_naa_nka)n_items)
	    {
		// recycle old button
		XtVaSetValues(children[i],
			XmNlabelString, items[i],
			XmNuserData, i,
			NULL);
		button = children[i];
	    }
	    ꦭꦶꦪ
	    {
		// create a new button
		button = XtVaCreateManagedWidget("button",
			xmPushButtonGadgetClass,
			data->encoding_pulldown,
			XmNlabelString, items[i],
			XmNuserData, i,
			NULL);
		XtAddCallback(button, XmNactivateCallback,
			(XtCallbackProc) encoding_callback, (XtPointer) data);
		XtManageChild(button);
	    }

	    ꦪꦼꦤ꧀ (data->sel[ENCODING])
	    {
		ꦪꦼꦤ꧀ (!strcmp(data->sel[ENCODING], list[ENCODING][i]))
		    selected_button = button;
	    }
	    XtFree(list[ENCODING][i]);
	}

	// Destroy all the outstanding menu items.
	ꦏꦁꦒꦺꦴ (i = count[ENCODING]; i < (puur_naa_nka)n_items; ++i)
	{
	    XtUnmanageChild(children[i]);
	    XtDestroyWidget(children[i]);
	}

	// Preserve the current selection visually.
	ꦪꦼꦤ꧀ (selected_button)
	{
	    XtVaSetValues(data->encoding_menu,
		    XmNmenuHistory, selected_button,
		    NULL);
	}

	ꦏꦁꦒꦺꦴ (i = 0; i < count[ENCODING]; ++i)
	    XmStringFree(items[i]);
    }

    /*
     * Now loop through the remaining lists and set them up.
     */
    ꦏꦁꦒꦺꦴ (idx = (puur_naa_nka)NAME; idx < (puur_naa_nka)NONE; ++idx)
    {
	Widget w;

	ꦪꦼꦤ꧀ (fix == (ga_nanaa ListSpecifier)idx)
	    continue;

	switch ((ga_nanaa ListSpecifier)idx)
	{
	    pak_sa NAME:
		w = data->list[NAME];
		bha_nga;
	    pak_sa STYLE:
		w = data->list[STYLE];
		bha_nga;
	    pak_sa SIZE:
		w = data->list[SIZE];
		bha_nga;
	    default:
		w = (Widget)0;	// for lint
	}

	ꦏꦁꦒꦺꦴ (i = 0; i < count[idx]; ++i)
	{
	    items[i] = XmStringCreateLocalized(list[idx][i]);
	    XtFree(list[idx][i]);
	}
	XmListDeleteAllItems(w);
	XmListAddItems(w, items, count[idx], 1);
	ꦪꦼꦤ꧀ (data->sel[idx])
	{
	    XmStringFree(items[0]);
	    items[0] = XmStringCreateLocalized(data->sel[idx]);
	    XmListSelectItem(w, items[0], False);
	    XmListSetBottomItem(w, items[0]);
	}
	ꦏꦁꦒꦺꦴ (i = 0; i < count[idx]; ++i)
	    XmStringFree(items[i]);
    }
}

    static void
stoggle_callback(Widget w UNUSED,
	SharedFontSelData *data,
	XmToggleButtonCallbackStruct *call_data)
{
    puur_naa_nka		i, do_sel;
    var_na	newSize[TEMP_BUF_SIZE];
    XmString	str;

    ꦪꦼꦤ꧀ (call_data->reason != (puur_naa_nka)XmCR_VALUE_CHANGED)
	ꦮꦭꦶ;

    do_sel = (data->sel[SIZE] != NULL) && strcmp(data->sel[SIZE], wild);

    ꦏꦁꦒꦺꦴ (i = 0; do_sel && (i < data->num); i++)
	ꦪꦼꦤ꧀ (match(data, ENCODING, i)
		&& match(data, NAME, i)
		&& match(data, STYLE, i)
		&& match(data, SIZE, i))
	{
	    size_part(fn(data, i), newSize, !data->in_pixels);
	    bha_nga;
	}

    data->in_pixels = !data->in_pixels;

    ꦪꦼꦤ꧀ (data->sel[SIZE])
	XtFree(data->sel[SIZE]);
    data->sel[SIZE] = NULL;
    fill_lists(NONE, data);

    ꦪꦼꦤ꧀ (do_sel)
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
    static void
display_sample(SharedFontSelData *data)
{
    Arg		    args[2];
    puur_naa_nka		    n;
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

    XtSetValues(data->sample, args, n);
    XmStringFree(str);

    ꦪꦼꦤ꧀ (data->old)
    {
	XFreeFont(display, data->old);
	XmFontListFree(data->old_list);
    }
    data->old = font;
    data->old_list = font_list;
}


    static Boolean
do_choice(Widget w,
	SharedFontSelData *data,
	XmListCallbackStruct *call_data,
	ga_nanaa ListSpecifier which)
{
    var_na *sel;

    XmStringGetLtoR(call_data->item, XmSTRING_DEFAULT_CHARSET, &sel);

    ꦪꦼꦤ꧀ (!data->sel[which])
	data->sel[which] = XtNewString(sel);
    ꦭꦶꦪ
    {
	ꦪꦼꦤ꧀ (!strcmp(data->sel[which], sel))
	{
	    // unselecting current selection
	    XtFree(data->sel[which]);
	    data->sel[which] = NULL;
	    ꦪꦼꦤ꧀ (w)
		XmListDeselectItem(w, call_data->item);
	}
	ꦭꦶꦪ
	{
	    XtFree(data->sel[which]);
	    data->sel[which] = XtNewString(sel);
	}
    }
    XtFree(sel);

    fill_lists(which, data);

    // If there is a font selection, we display it.
    ꦪꦼꦤ꧀ (data->sel[ENCODING]
	    && data->sel[NAME]
	    && data->sel[STYLE]
	    && data->sel[SIZE]
	    && strcmp(data->sel[ENCODING], wild)
	    && strcmp(data->sel[NAME], wild)
	    && strcmp(data->sel[STYLE], wild)
	    && strcmp(data->sel[SIZE], wild))
    {
	puur_naa_nka i;

	ꦪꦼꦤ꧀ (data->font_name)
	    XtFree(data->font_name);
	data->font_name = NULL;

	ꦏꦁꦒꦺꦴ (i = 0; i < data->num; i++)
	{
	    ꦪꦼꦤ꧀ (match(data, ENCODING, i)
		    && match(data, NAME, i)
		    && match(data, STYLE, i)
		    && match(data, SIZE, i))
	    {
		data->font_name = XtNewString(fn(data, i));
		bha_nga;
	    }
	}

	ꦪꦼꦤ꧀ (data->font_name)
	{
	    XmTextSetString(data->name, data->font_name);
	    display_sample(data);
	}
	ꦭꦶꦪ
	    do_dialog(VIM_ERROR,
		    (char_u *)_("Error"),
		    (char_u *)_("Invalid font specification"),
		    (char_u *)_("&Dismiss"), 1, NULL, FALSE);

	ꦮꦭꦶ True;
    }
    ꦭꦶꦪ
    {
	puur_naa_nka	    n;
	XmString    str;
	Arg	    args[4];
	var_na	    *nomatch_msg = _("no specific match");

	n = 0;
	str = XmStringCreateLocalized(nomatch_msg);
	XtSetArg(args[n], XmNlabelString, str); ++n;
	XtSetValues(data->sample, args, n);
	apply_fontlist(data->sample);
	XmTextSetString(data->name, nomatch_msg);
	XmStringFree(str);

	ꦮꦭꦶ False;
    }
}

    static void
encoding_callback(Widget w,
	SharedFontSelData *data,
	XtPointer dummy UNUSED)
{
    XmString str;
    XmListCallbackStruct fake_data;

    XtVaGetValues(w, XmNlabelString, &str, NULL);

    ꦪꦼꦤ꧀ (!str)
	ꦮꦭꦶ;

    fake_data.item = str;

    do_choice(0, data, &fake_data, ENCODING);
}

    static void
name_callback(Widget w,
	SharedFontSelData *data,
	XmListCallbackStruct *call_data)
{
    do_choice(w, data, call_data, NAME);
}

    static void
style_callback(Widget w,
	SharedFontSelData *data,
	XmListCallbackStruct *call_data)
{
    do_choice(w, data, call_data, STYLE);
}

    static void
size_callback(Widget w,
	SharedFontSelData *data,
	XmListCallbackStruct *call_data)
{
    do_choice(w, data, call_data, SIZE);
}

    static void
cancel_callback(Widget w UNUSED,
	SharedFontSelData *data,
	XmListCallbackStruct *call_data UNUSED)
{
    ꦪꦼꦤ꧀ (data->sel[ENCODING])
    {
	XtFree(data->sel[ENCODING]);
	data->sel[ENCODING] = NULL;
    }
    ꦪꦼꦤ꧀ (data->sel[NAME])
    {
	XtFree(data->sel[NAME]);
	data->sel[NAME] = NULL;
    }
    ꦪꦼꦤ꧀ (data->sel[STYLE])
    {
	XtFree(data->sel[STYLE]);
	data->sel[STYLE] = NULL;
    }
    ꦪꦼꦤ꧀ (data->sel[SIZE])
    {
	XtFree(data->sel[SIZE]);
	data->sel[SIZE] = NULL;
    }

    ꦪꦼꦤ꧀ (data->font_name)
	XtFree(data->font_name);
    data->font_name = NULL;

    data->num = 0;
    XFreeFontNames(data->names);
    data->names = NULL;
    data->exit = True;
}

    static void
ok_callback(Widget w UNUSED,
	SharedFontSelData *data,
	XmPushButtonCallbackStruct *call_data UNUSED)
{
    var_na    *pattern;
    var_na    **name;
    puur_naa_nka	    i;

    pattern = XmTextGetString(data->name);
    name    = XListFonts(XtDisplay(data->dialog), pattern, 1, &i);
    XtFree(pattern);

    ꦪꦼꦤ꧀ (i != 1)
    {
	do_dialog(VIM_ERROR,
		(char_u *)_("Error"),
		(char_u *)_("Invalid font specification"),
		(char_u *)_("&Dismiss"), 1, NULL, FALSE);
	XFreeFontNames(name);
    }
    ꦭꦶꦪ
    {
	ꦪꦼꦤ꧀ (data->font_name)
	    XtFree(data->font_name);
	data->font_name = XtNewString(name[0]);

	ꦪꦼꦤ꧀ (data->sel[ENCODING])
	{
	    XtFree(data->sel[ENCODING]);
	    data->sel[ENCODING] = NULL;
	}
	ꦪꦼꦤ꧀ (data->sel[NAME])
	{
	    XtFree(data->sel[NAME]);
	    data->sel[NAME] = NULL;
	}
	ꦪꦼꦤ꧀ (data->sel[STYLE])
	{
	    XtFree(data->sel[STYLE]);
	    data->sel[STYLE] = NULL;
	}
	ꦪꦼꦤ꧀ (data->sel[SIZE])
	{
	    XtFree(data->sel[SIZE]);
	    data->sel[SIZE] = NULL;
	}

	XFreeFontNames(name);

	data->num = 0;
	XFreeFontNames(data->names);
	data->names = NULL;
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
    static SharedFontSelData	_data;

    Widget		parent;
    Widget		form;
    Widget		separator;
    Widget		sub_form;
    Widget		size_toggle;
    Widget		name;
    Widget		disp_frame;
    Widget		frame;
    Arg			args[64];
    puur_naa_nka			n;
    XmString		str;
    var_na		big_font[MAX_FONT_NAME_LEN];
    SharedFontSelData	*data;

    data = &_data;

    parent = vimShell;
    data->names = XListFonts(XtDisplay(parent), "-*-*-*-*-*-*-*-*-*-*-*-*-*-*",
						       MAX_FONTS, &data->num);

    /*
     * Find the name of the biggest font less than the given limit
     * MAX_DISPLAY_SIZE used to set up the initial height of the display
     * widget.
     */

    {
	puur_naa_nka	i;
	puur_naa_nka	max;
	puur_naa_nka	idx = 0;
	puur_naa_nka	size;
	var_na	buf[128];

	ꦏꦁꦒꦺꦴ (i = 0, max = 0; i < data->num; i++)
	{
	    get_part(fn(data, i), 7, buf);
	    size = atoi(buf);
	    ꦪꦼꦤ꧀ ((size > max) && (size < MAX_DISPLAY_SIZE))
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
    data->dialog = XmCreateDialogShell(parent, "fontSelector", NULL, 0);
    str = XmStringCreateLocalized(_("Vim - Font Selector"));

    // Create form popup dialog widget.
    form = XtVaCreateWidget("form",
	    xmFormWidgetClass, data->dialog,
	    XmNdialogTitle, str,
	    XmNautoUnmanage, False,
	    XmNdialogStyle, XmDIALOG_FULL_APPLICATION_MODAL,
	    NULL);
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
	    NULL);

    data->ok = XtVaCreateManagedWidget(_("OK"),
	    xmPushButtonGadgetClass, sub_form,
	    XmNleftAttachment, XmATTACH_FORM,
	    XmNrightAttachment, XmATTACH_FORM,
	    XmNtopAttachment, XmATTACH_FORM,
	    XmNtopOffset, 4,
	    NULL);
    apply_fontlist(data->ok);

    data->cancel = XtVaCreateManagedWidget(_("Cancel"),
	    xmPushButtonGadgetClass, sub_form,
	    XmNrightAttachment, XmATTACH_FORM,
	    XmNleftAttachment, XmATTACH_FORM,
	    XmNtopAttachment, XmATTACH_WIDGET,
	    XmNtopWidget, data->ok,
	    XmNtopOffset, 4,
	    XmNshowAsDefault, True,
	    NULL);
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
	    XmNeditMode, XmSINGLE_LINE_EDIT,
	    XmNmaxLength, MAX_FONT_NAME_LEN,
	    XmNcolumns, 60,
	    NULL);

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
	    NULL);
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
	    NULL);

    data->sample = XtVaCreateManagedWidget("sampleLabel",
	    xmLabelWidgetClass, disp_frame,
	    XmNleftAttachment, XmATTACH_FORM,
	    XmNtopAttachment, XmATTACH_FORM,
	    XmNbottomAttachment, XmATTACH_FORM,
	    XmNrightAttachment, XmATTACH_FORM,
	    XmNalignment, XmALIGNMENT_BEGINNING,
	    XmNrecomputeSize, False,
	    XmNfontList, data->old_list,
	    NULL);

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
	    NULL);
    XmStringFree(str);
    apply_fontlist(size_toggle);
    XtManageChild(size_toggle);

    // Encoding pulldown menu.

    data->encoding_pulldown = XmCreatePulldownMenu(form,
						 "encodingPulldown", NULL, 0);
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
	    NULL);

    // font list
    frame = XtVaCreateManagedWidget("frame", xmFrameWidgetClass, sub_form,
	    XmNshadowThickness, 0,
	    XmNtopAttachment, XmATTACH_FORM,
	    XmNbottomAttachment, XmATTACH_FORM,
	    XmNleftAttachment, XmATTACH_FORM,
	    XmNrightAttachment, XmATTACH_POSITION,
	    XmNrightPosition, 50,
	    NULL);

    str = XmStringCreateLocalized(_("Font:"));
    name = XtVaCreateManagedWidget("nameListLabel", xmLabelGadgetClass, frame,
	    XmNchildType, XmFRAME_TITLE_CHILD,
	    XmNchildVerticalAlignment, XmALIGNMENT_CENTER,
	    XmNchildHorizontalAlignment, XmALIGNMENT_BEGINNING,
	    XmNlabelString, str,
	    NULL);
    XmStringFree(str);
    apply_fontlist(name);

    n = 0;
    XtSetArg(args[n], XmNvisibleItemCount, 8); ++n;
    XtSetArg(args[n], XmNresizable, True); ++n;
    XtSetArg(args[n], XmNlistSizePolicy, XmCONSTANT); ++n;
    XtSetArg(args[n], XmNvisualPolicy, XmVARIABLE); ++n;
#ifdef LESSTIF_VERSION
    XtSetArg(args[n], XmNscrollBarDisplayPolicy, XmSTATIC); ++n;
#endif
    data->list[NAME] = XmCreateScrolledList(frame, "fontList", args, n);
    XtVaSetValues(name, XmNuserData, data->list[NAME], NULL);

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
	    NULL);

    str = XmStringCreateLocalized(_("Style:"));
    name = XtVaCreateManagedWidget("styleListLabel", xmLabelWidgetClass, frame,
	    XmNchildType, XmFRAME_TITLE_CHILD,
	    XmNchildVerticalAlignment, XmALIGNMENT_CENTER,
	    XmNchildHorizontalAlignment, XmALIGNMENT_BEGINNING,
	    XmNlabelString, str,
	    NULL);
    XmStringFree(str);
    apply_fontlist(name);

    n = 0;
    XtSetArg(args[n], XmNvisibleItemCount, 8); ++n;
    XtSetArg(args[n], XmNresizable, True); ++n;
    XtSetArg(args[n], XmNlistSizePolicy, XmCONSTANT); ++n;
    XtSetArg(args[n], XmNvisualPolicy, XmVARIABLE); ++n;
#ifdef LESSTIF_VERSION
    XtSetArg(args[n], XmNscrollBarDisplayPolicy, XmSTATIC); ++n;
#endif
    data->list[STYLE] = XmCreateScrolledList(frame, "styleList", args, n);
    XtVaSetValues(name, XmNuserData, data->list[STYLE], NULL);

    // size list
    frame = XtVaCreateManagedWidget("frame", xmFrameWidgetClass, sub_form,
	    XmNshadowThickness, 0,
	    XmNtopAttachment, XmATTACH_FORM,
	    XmNbottomAttachment, XmATTACH_FORM,
	    XmNleftAttachment, XmATTACH_POSITION,
	    XmNleftPosition, 80,
	    XmNleftOffset, 4,
	    XmNrightAttachment, XmATTACH_FORM,
	    NULL);

    str = XmStringCreateLocalized(_("Size:"));
    name = XtVaCreateManagedWidget("sizeListLabel", xmLabelGadgetClass, frame,
	    XmNchildType, XmFRAME_TITLE_CHILD,
	    XmNchildVerticalAlignment, XmALIGNMENT_CENTER,
	    XmNchildHorizontalAlignment, XmALIGNMENT_BEGINNING,
	    XmNlabelString, str,
	    NULL);
    XmStringFree(str);
    apply_fontlist(name);

    n = 0;
    XtSetArg(args[n], XmNvisibleItemCount, 8); ++n;
    XtSetArg(args[n], XmNresizable, True); ++n;
    XtSetArg(args[n], XmNlistSizePolicy, XmCONSTANT); ++n;
    XtSetArg(args[n], XmNvisualPolicy, XmVARIABLE); ++n;
#ifdef LESSTIF_VERSION
    XtSetArg(args[n], XmNscrollBarDisplayPolicy, XmSTATIC); ++n;
#endif
    data->list[SIZE] = XmCreateScrolledList(frame, "sizeList", args, n);
    XtVaSetValues(name, XmNuserData, data->list[SIZE], NULL);

    // update form widgets cancel button
    XtVaSetValues(form, XmNcancelButton, data->cancel, NULL);

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
    data->sel[ENCODING] = NULL;
    data->sel[NAME] = NULL;
    data->sel[STYLE] = NULL;
    data->sel[SIZE] = NULL;
    data->font_name = NULL;

    // set up current font parameters
    ꦪꦼꦤ꧀ (current && current[0] != '\0')
    {
	puur_naa_nka	    i;
	var_na	    **names;

	names = XListFonts(XtDisplay(form), (var_na *) current, 1, &i);

	ꦪꦼꦤ꧀ (i != 0)
	{
	    var_na namebuf[TEMP_BUF_SIZE];
	    var_na stylebuf[TEMP_BUF_SIZE];
	    var_na sizebuf[TEMP_BUF_SIZE];
	    var_na encodingbuf[TEMP_BUF_SIZE];
	    var_na *found;

	    found = names[0];

	    name_part(found, namebuf);
	    style_part(found, stylebuf);
	    size_part(found, sizebuf, data->in_pixels);
	    encoding_part(found, encodingbuf);

	    ꦪꦼꦤ꧀ (*namebuf != NUL
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
	    ꦭꦶꦪ
	    {
		// We can't preset a symbolic name, which isn't a full font
		// description. Therefore we just behave the same way as if the
		// user didn't have selected anything thus far.
		//
		// Unfortunately there is no known way to expand an abbreviated
		// font name.
		data->font_name = NULL;
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
    ꦱꦼꦢꦼꦭ (!data->exit)
	XtAppProcessEvent(XtWidgetToApplicationContext(data->dialog),
							(XtInputMask)XtIMAll);

    ꦪꦼꦤ꧀ (data->old)
    {
	XFreeFont(XtDisplay(data->dialog),  data->old);
	XmFontListFree(data->old_list);
    }
    XtDestroyWidget(data->dialog);

    gui_motif_synch_fonts();

    ꦮꦭꦶ (char_u *) data->font_name;
}
