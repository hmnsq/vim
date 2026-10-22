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
#否则
# define apply_fontlist(w)
#endif

/////////////////////////////////////////////////////////////////////////////
// Font selection dialogue implementation.

static char wild[3] = "*";

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
    もし (!dw_atom)
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

enum ListSpecifier
{
    ENCODING,
    NAME,
    STYLE,
    SIZE,
    NONE
};

typedef struct _SharedFontSelData
{
    Widget	dialog;
    Widget	ok;
    Widget	cancel;
    Widget	encoding_pulldown;
    Widget	encoding_menu;
    Widget	list[NONE];
    Widget	name;
    Widget	sample;
    char	**names;	// font name array of arrays
    整数		num;		// number of font names
    String	sel[NONE];	// selection category
    Boolean	in_pixels;	// toggle state - size in pixels
    char	*font_name;	// current font name
    XFontStruct	*old;		// font data structure for sample display
    XmFontList	old_list;	// font data structure for sample display
    Boolean	exit;		// used for program exit control
} SharedFontSelData;

/*
 * Checking access to the font name array for validity.
 */
    static char *
fn(SharedFontSelData *data, 整数 i)
{
    // Assertion checks:
    もし (data->num < 0)
	abort();
    もし (i >= data->num)
	i = data->num - 1;
    もし (i < 0)
	i = 0;

    戻る data->names[i];
}

/*
 * Get a specific substring from a font name.
 */
    static void
get_part(char *in, 整数 pos, char *out)
{
    整数	i;
    整数 j;

    *out = '\0';

    ために (i = 0; (pos > 0) && (in[i] != '\0'); ++i)
	もし (in[i] == '-')
	    pos--;

    もし (in[i] == '\0')
	戻る;

    ために (j = 0; (in[i] != '-') && (in[i] != '\0'); ++i, ++j)
	out[j] = in[i];
    out[j] = '\0';
}

/*
 * Given a font name this function returns the part used in the first
 * scroll list.
 */
    static void
name_part(char *font, char *buf)
{
    char    buf2[TEMP_BUF_SIZE];
    char    buf3[TEMP_BUF_SIZE];

    get_part(font, 2, buf2);
    get_part(font, 1, buf3);

    もし (*buf3 != NUL)
	vim_snprintf(buf, TEMP_BUF_SIZE, "%s (%s)", buf2, buf3);
    否则
	vim_snprintf(buf, TEMP_BUF_SIZE, "%s", buf2);
}

/*
 * Given a font name this function returns the part used in the second scroll list.
 */
    static void
style_part(char *font, char *buf)
{
    char    buf2[TEMP_BUF_SIZE];
    char    buf3[TEMP_BUF_SIZE];

    get_part(font, 3, buf3);
    get_part(font, 5, buf2);

    もし (!strcmp(buf2, "normal") && !strcmp(buf2, "Normal")
						   && !strcmp(buf2, "NORMAL"))
	vim_snprintf(buf, TEMP_BUF_SIZE, "%s %s", buf3, buf2);
    否则
	strcpy(buf, buf3);

    get_part(font, 6, buf2);

    もし (buf2[0] != '\0')
	vim_snprintf(buf3, TEMP_BUF_SIZE, "%s %s", buf, buf2);
    否则
	strcpy(buf3, buf);

    get_part(font, 4, buf2);

    もし (!strcmp(buf2, "o") || !strcmp(buf2, "O"))
	vim_snprintf(buf, TEMP_BUF_SIZE, "%s oblique", buf3);
    否则 もし (!strcmp(buf2, "i") || !strcmp(buf2, "I"))
	vim_snprintf(buf, TEMP_BUF_SIZE, "%s italic", buf3);

    もし (!strcmp(buf, " "))
	strcpy(buf, "-");
}

/*
 * Given a font name this function returns the part used in the third
 * scroll list.
 */
    static void
size_part(char *font, char *buf, 整数 inPixels)
{
    整数	    size;
    float   temp;

    *buf = '\0';

    もし (inPixels)
    {
	get_part(font, 7, buf);
	もし (*buf != NUL)
	{
	    size = atoi(buf);
	    sprintf(buf, "%3d", size);
	}
    }
    否则
    {
	get_part(font, 8, buf);
	もし (*buf != NUL)
	{
	    size = atoi(buf);
	    temp = (float)size / 10.0;
	    size = temp;
	    もし (buf[strlen(buf) - 1] == '0')
		sprintf(buf, "%3d", size);
	    否则
		sprintf(buf, "%4.1f", temp);
	}
    }
}

/*
 * Given a font name this function returns the part used in the choice menu.
 */
    static void
encoding_part(char *font, char *buf)
{
    char    buf1[TEMP_BUF_SIZE];
    char    buf2[TEMP_BUF_SIZE];

    *buf = '\0';

    get_part(font, 13, buf1);
    get_part(font, 14, buf2);

    もし (*buf1 != NUL && *buf2 != NUL)
	vim_snprintf(buf, TEMP_BUF_SIZE, "%s-%s", buf1, buf2);
    もし (!strcmp(buf, " "))
	strcpy(buf, "-");
}

/*
 * Inserts a string into correct sorted position in a list.
 */
    static void
add_to_list(char **buf, char *item, 整数 *count)
{
    整数	i;
    整数 j;

    もし (*count == MAX_ENTRIES_IN_LIST)
	戻る;

    // avoid duplication
    ために (i = 0; i < *count; ++i)
    {
	もし (!strcmp(buf[i], item))
	    戻る;
    }

    // find order place, but make sure that wild card comes first
    もし (!strcmp(item, wild))
	i = 0;
    否则
	ために (i = 0; i < *count; ++i)
	    もし (strcmp(buf[i], item) > 0 && strcmp(buf[i], wild))
		break;

    // now insert the item
    ために (j = *count; j > i; --j)
	buf[j] = buf[j-1];
    buf[i] = XtNewString(item);

    ++(*count);
}

/*
 * True if the font matches some field.
 */
    static Boolean
match(SharedFontSelData *data, enum ListSpecifier l, 整数 i)
{
    char buf[TEMP_BUF_SIZE];

    // An empty selection or a wild card matches anything.
    もし (!data->sel[l] || !strcmp(data->sel[l], wild))
	戻る True;

    // chunk out the desired part...
    switch (l)
    {
	case ENCODING:
	    encoding_part(fn(data, i), buf);
	    break;

	case NAME:
	    name_part(fn(data, i), buf);
	    break;

	case STYLE:
	    style_part(fn(data, i), buf);
	    break;

	case SIZE:
	    size_part(fn(data, i), buf, data->in_pixels);
	    break;
	default:
	    ;
    }

    // ...and chew it now

    戻る !strcmp(buf, data->sel[l]);
}

    static Boolean
proportional(char *font)
{
    char buf[TEMP_BUF_SIZE];

    get_part(font, 11, buf);

    戻る !strcmp(buf, "p") || !strcmp(buf, "P");
}


static void encoding_callback(Widget w, SharedFontSelData *data,
							     XtPointer dummy);

/*
 * Parse through the fontlist data and set up the three scroll lists.  The fix
 * parameter can be used to exclude a list from any changes.  This is used for
 * updates after selections caused by the users actions.
 */
    static void
fill_lists(enum ListSpecifier fix, SharedFontSelData *data)
{
    char	*list[NONE][MAX_ENTRIES_IN_LIST];
    整数		count[NONE];
    char	buf[TEMP_BUF_SIZE];
    XmString	items[MAX_ENTRIES_IN_LIST];
    整数		i;
    整数		idx;

    ために (idx = (整数)ENCODING; idx < (整数)NONE; ++idx)
	count[idx] = 0;

    // First we insert the wild char into every single list.
    もし (fix != ENCODING)
	add_to_list(list[ENCODING], wild, &count[ENCODING]);
    もし (fix != NAME)
	add_to_list(list[NAME], wild, &count[NAME]);
    もし (fix != STYLE)
	add_to_list(list[STYLE], wild, &count[STYLE]);
    もし (fix != SIZE)
	add_to_list(list[SIZE], wild, &count[SIZE]);

    ために (i = 0; i < data->num && i < MAX_ENTRIES_IN_LIST; i++)
    {
	もし (proportional(fn(data, i)))
	    continue;

	もし (fix != ENCODING
		&& match(data, NAME, i)
		&& match(data, STYLE, i)
		&& match(data, SIZE, i))
	{
	    encoding_part(fn(data, i), buf);
	    add_to_list(list[ENCODING], buf, &count[ENCODING]);
	}

	もし (fix != NAME
		&& match(data, ENCODING, i)
		&& match(data, STYLE, i)
		&& match(data, SIZE, i))
	{
	    name_part(fn(data, i), buf);
	    add_to_list(list[NAME], buf, &count[NAME]);
	}

	もし (fix != STYLE
		&& match(data, ENCODING, i)
		&& match(data, NAME, i)
		&& match(data, SIZE, i))
	{
	    style_part(fn(data, i), buf);
	    add_to_list(list[STYLE], buf, &count[STYLE]);
	}

	もし (fix != SIZE
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

    もし (fix != ENCODING)
    {
	Cardinal n_items;
	WidgetList children;
	Widget selected_button = 0;

	// Get and update the current button list.
	XtVaGetValues(data->encoding_pulldown,
		XmNchildren, &children,
		XmNnumChildren, &n_items,
		NULL);

	ために (i = 0; i < count[ENCODING]; ++i)
	{
	    Widget button;

	    items[i] = XmStringCreateLocalized(list[ENCODING][i]);

	    もし (i < (整数)n_items)
	    {
		// recycle old button
		XtVaSetValues(children[i],
			XmNlabelString, items[i],
			XmNuserData, i,
			NULL);
		button = children[i];
	    }
	    否则
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

	    もし (data->sel[ENCODING])
	    {
		もし (!strcmp(data->sel[ENCODING], list[ENCODING][i]))
		    selected_button = button;
	    }
	    XtFree(list[ENCODING][i]);
	}

	// Destroy all the outstanding menu items.
	ために (i = count[ENCODING]; i < (整数)n_items; ++i)
	{
	    XtUnmanageChild(children[i]);
	    XtDestroyWidget(children[i]);
	}

	// Preserve the current selection visually.
	もし (selected_button)
	{
	    XtVaSetValues(data->encoding_menu,
		    XmNmenuHistory, selected_button,
		    NULL);
	}

	ために (i = 0; i < count[ENCODING]; ++i)
	    XmStringFree(items[i]);
    }

    /*
     * Now loop through the remaining lists and set them up.
     */
    ために (idx = (整数)NAME; idx < (整数)NONE; ++idx)
    {
	Widget w;

	もし (fix == (enum ListSpecifier)idx)
	    continue;

	switch ((enum ListSpecifier)idx)
	{
	    case NAME:
		w = data->list[NAME];
		break;
	    case STYLE:
		w = data->list[STYLE];
		break;
	    case SIZE:
		w = data->list[SIZE];
		break;
	    default:
		w = (Widget)0;	// for lint
	}

	ために (i = 0; i < count[idx]; ++i)
	{
	    items[i] = XmStringCreateLocalized(list[idx][i]);
	    XtFree(list[idx][i]);
	}
	XmListDeleteAllItems(w);
	XmListAddItems(w, items, count[idx], 1);
	もし (data->sel[idx])
	{
	    XmStringFree(items[0]);
	    items[0] = XmStringCreateLocalized(data->sel[idx]);
	    XmListSelectItem(w, items[0], False);
	    XmListSetBottomItem(w, items[0]);
	}
	ために (i = 0; i < count[idx]; ++i)
	    XmStringFree(items[i]);
    }
}

    static void
stoggle_callback(Widget w UNUSED,
	SharedFontSelData *data,
	XmToggleButtonCallbackStruct *call_data)
{
    整数		i, do_sel;
    char	newSize[TEMP_BUF_SIZE];
    XmString	str;

    もし (call_data->reason != (整数)XmCR_VALUE_CHANGED)
	戻る;

    do_sel = (data->sel[SIZE] != NULL) && strcmp(data->sel[SIZE], wild);

    ために (i = 0; do_sel && (i < data->num); i++)
	もし (match(data, ENCODING, i)
		&& match(data, NAME, i)
		&& match(data, STYLE, i)
		&& match(data, SIZE, i))
	{
	    size_part(fn(data, i), newSize, !data->in_pixels);
	    break;
	}

    data->in_pixels = !data->in_pixels;

    もし (data->sel[SIZE])
	XtFree(data->sel[SIZE]);
    data->sel[SIZE] = NULL;
    fill_lists(NONE, data);

    もし (do_sel)
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
    整数		    n;
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

    もし (data->old)
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
	enum ListSpecifier which)
{
    char *sel;

    XmStringGetLtoR(call_data->item, XmSTRING_DEFAULT_CHARSET, &sel);

    もし (!data->sel[which])
	data->sel[which] = XtNewString(sel);
    否则
    {
	もし (!strcmp(data->sel[which], sel))
	{
	    // unselecting current selection
	    XtFree(data->sel[which]);
	    data->sel[which] = NULL;
	    もし (w)
		XmListDeselectItem(w, call_data->item);
	}
	否则
	{
	    XtFree(data->sel[which]);
	    data->sel[which] = XtNewString(sel);
	}
    }
    XtFree(sel);

    fill_lists(which, data);

    // If there is a font selection, we display it.
    もし (data->sel[ENCODING]
	    && data->sel[NAME]
	    && data->sel[STYLE]
	    && data->sel[SIZE]
	    && strcmp(data->sel[ENCODING], wild)
	    && strcmp(data->sel[NAME], wild)
	    && strcmp(data->sel[STYLE], wild)
	    && strcmp(data->sel[SIZE], wild))
    {
	整数 i;

	もし (data->font_name)
	    XtFree(data->font_name);
	data->font_name = NULL;

	ために (i = 0; i < data->num; i++)
	{
	    もし (match(data, ENCODING, i)
		    && match(data, NAME, i)
		    && match(data, STYLE, i)
		    && match(data, SIZE, i))
	    {
		data->font_name = XtNewString(fn(data, i));
		break;
	    }
	}

	もし (data->font_name)
	{
	    XmTextSetString(data->name, data->font_name);
	    display_sample(data);
	}
	否则
	    do_dialog(VIM_ERROR,
		    (char_u *)_("Error"),
		    (char_u *)_("Invalid font specification"),
		    (char_u *)_("&Dismiss"), 1, NULL, FALSE);

	戻る True;
    }
    否则
    {
	整数	    n;
	XmString    str;
	Arg	    args[4];
	char	    *nomatch_msg = _("no specific match");

	n = 0;
	str = XmStringCreateLocalized(nomatch_msg);
	XtSetArg(args[n], XmNlabelString, str); ++n;
	XtSetValues(data->sample, args, n);
	apply_fontlist(data->sample);
	XmTextSetString(data->name, nomatch_msg);
	XmStringFree(str);

	戻る False;
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

    もし (!str)
	戻る;

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
    もし (data->sel[ENCODING])
    {
	XtFree(data->sel[ENCODING]);
	data->sel[ENCODING] = NULL;
    }
    もし (data->sel[NAME])
    {
	XtFree(data->sel[NAME]);
	data->sel[NAME] = NULL;
    }
    もし (data->sel[STYLE])
    {
	XtFree(data->sel[STYLE]);
	data->sel[STYLE] = NULL;
    }
    もし (data->sel[SIZE])
    {
	XtFree(data->sel[SIZE]);
	data->sel[SIZE] = NULL;
    }

    もし (data->font_name)
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
    char    *pattern;
    char    **name;
    整数	    i;

    pattern = XmTextGetString(data->name);
    name    = XListFonts(XtDisplay(data->dialog), pattern, 1, &i);
    XtFree(pattern);

    もし (i != 1)
    {
	do_dialog(VIM_ERROR,
		(char_u *)_("Error"),
		(char_u *)_("Invalid font specification"),
		(char_u *)_("&Dismiss"), 1, NULL, FALSE);
	XFreeFontNames(name);
    }
    否则
    {
	もし (data->font_name)
	    XtFree(data->font_name);
	data->font_name = XtNewString(name[0]);

	もし (data->sel[ENCODING])
	{
	    XtFree(data->sel[ENCODING]);
	    data->sel[ENCODING] = NULL;
	}
	もし (data->sel[NAME])
	{
	    XtFree(data->sel[NAME]);
	    data->sel[NAME] = NULL;
	}
	もし (data->sel[STYLE])
	{
	    XtFree(data->sel[STYLE]);
	    data->sel[STYLE] = NULL;
	}
	もし (data->sel[SIZE])
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
    整数			n;
    XmString		str;
    char		big_font[MAX_FONT_NAME_LEN];
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
	整数	i;
	整数	max;
	整数	idx = 0;
	整数	size;
	char	buf[128];

	ために (i = 0, max = 0; i < data->num; i++)
	{
	    get_part(fn(data, i), 7, buf);
	    size = atoi(buf);
	    もし ((size > max) && (size < MAX_DISPLAY_SIZE))
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
    もし (current && current[0] != '\0')
    {
	整数	    i;
	char	    **names;

	names = XListFonts(XtDisplay(form), (char *) current, 1, &i);

	もし (i != 0)
	{
	    char namebuf[TEMP_BUF_SIZE];
	    char stylebuf[TEMP_BUF_SIZE];
	    char sizebuf[TEMP_BUF_SIZE];
	    char encodingbuf[TEMP_BUF_SIZE];
	    char *found;

	    found = names[0];

	    name_part(found, namebuf);
	    style_part(found, stylebuf);
	    size_part(found, sizebuf, data->in_pixels);
	    encoding_part(found, encodingbuf);

	    もし (*namebuf != NUL
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
	    否则
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
    ながら (!data->exit)
	XtAppProcessEvent(XtWidgetToApplicationContext(data->dialog),
							(XtInputMask)XtIMAll);

    もし (data->old)
    {
	XFreeFont(XtDisplay(data->dialog),  data->old);
	XmFontListFree(data->old_list);
    }
    XtDestroyWidget(data->dialog);

    gui_motif_synch_fonts();

    戻る (char_u *) data->font_name;
}
