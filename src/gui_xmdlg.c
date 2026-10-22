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

#ಅನ್ತರ್ಭಾವ "vim.h"

#ಅನ್ತರ್ಭಾವ <Xm/Form.h>
#ಅನ್ತರ್ಭಾವ <Xm/PushBG.h>
#ಅನ್ತರ್ಭಾವ <Xm/Text.h>
#ಅನ್ತರ್ಭಾವ <Xm/TextF.h>
#ಅನ್ತರ್ಭಾವ <Xm/Label.h>
#ಅನ್ತರ್ಭಾವ <Xm/Frame.h>
#ಅನ್ತರ್ಭಾವ <Xm/LabelG.h>
#ಅನ್ತರ್ಭಾವ <Xm/ToggleBG.h>
#ಅನ್ತರ್ಭಾವ <Xm/SeparatoG.h>
#ಅನ್ತರ್ಭಾವ <Xm/DialogS.h>
#ಅನ್ತರ್ಭಾವ <Xm/List.h>
#ಅನ್ತರ್ಭಾವ <Xm/RowColumn.h>
#ಅನ್ತರ್ಭಾವ <Xm/AtomMgr.h>
#ಅನ್ತರ್ಭಾವ <Xm/Protocols.h>

#ಅನ್ತರ್ಭಾವ <X11/keysym.h>
#ಅನ್ತರ್ಭಾವ <X11/Xatom.h>
#ಅನ್ತರ್ಭಾವ <X11/StringDefs.h>
#ಅನ್ತರ್ಭಾವ <X11/Intrinsic.h>

ಬಾಹ್ಯ Widget vimShell;

#ifdef FEAT_MENU
# define apply_fontlist(w) gui_motif_menu_fontlist(w)
#ಅನ್ಯಥಾ
# define apply_fontlist(w)
#endif

/////////////////////////////////////////////////////////////////////////////
// Font selection dialogue implementation.

ಸ್ಥಿರ ವರ್ಣ wild[3] = "*";

/*
 * FIXME: This is a generic function, which should be used throughout the whole
 * application.
 *
 * Add close_callback, which will be called when the user selects close from
 * the window menu.  The close menu item usually activates f.kill which sends a
 * WM_DELETE_WINDOW protocol request for the window.
 */

    ಸ್ಥಿರ ಶೂನ್ಯ
add_cancel_action(Widget shell, XtCallbackProc close_callback, ಶೂನ್ಯ *arg)
{
    ಸ್ಥಿರ Atom wmp_atom = 0;
    ಸ್ಥಿರ Atom dw_atom = 0;
    Display *display = XtDisplay(shell);

    // deactivate the built-in delete response of killing the application
    Xt_Va_Set_ಮೌಲ್ಯಗಳು(shell, XmNdeleteResponse, XmDO_NOTHING, ರಿಕ್ತ);

    // add a delete window protocol callback instead
    ಯದಿ (!dw_atom)
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

ಗಣನಾ ListSpecifier
{
    ENCODING,
    NAME,
    STYLE,
    SIZE,
    NONE
};

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ _SharedFontSelData
{
    Widget	dialog;
    Widget	ok;
    Widget	cancel;
    Widget	encoding_pulldown;
    Widget	encoding_menu;
    Widget	list[NONE];
    Widget	name;
    Widget	sample;
    ವರ್ಣ	**names;	// font name array of arrays
    ಪೂರ್ಣಾಙ್ಕ		ಅಂಕೆ;		// number of font names
    String	sel[NONE];	// selection category
    Boolean	in_pixels;	// toggle state - size in pixels
    ವರ್ಣ	*font_name;	// current font name
    XFontStruct	*old;		// font data structure for sample display
    XmFontList	old_list;	// font data structure for sample display
    Boolean	exit;		// used for program exit control
} SharedFontSelData;

/*
 * Checking access to the font name array for validity.
 */
    ಸ್ಥಿರ ವರ್ಣ *
fn(SharedFontSelData *data, ಪೂರ್ಣಾಙ್ಕ i)
{
    // Assertion checks:
    ಯದಿ (data->ಅಂಕೆ < 0)
	abort();
    ಯದಿ (i >= data->ಅಂಕೆ)
	i = data->ಅಂಕೆ - 1;
    ಯದಿ (i < 0)
	i = 0;

    ಪ್ರತ್ಯಾಗಮ data->names[i];
}

/*
 * Get a specific substring from a font name.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
get_part(ವರ್ಣ *in, ಪೂರ್ಣಾಙ್ಕ pos, ವರ್ಣ *out)
{
    ಪೂರ್ಣಾಙ್ಕ	i;
    ಪೂರ್ಣಾಙ್ಕ j;

    *out = '\0';

    ಪ್ರತಿ (i = 0; (pos > 0) && (in[i] != '\0'); ++i)
	ಯದಿ (in[i] == '-')
	    pos--;

    ಯದಿ (in[i] == '\0')
	ಪ್ರತ್ಯಾಗಮ;

    ಪ್ರತಿ (j = 0; (in[i] != '-') && (in[i] != '\0'); ++i, ++j)
	out[j] = in[i];
    out[j] = '\0';
}

/*
 * Given a font name this function returns the part used in the first
 * scroll list.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
name_part(ವರ್ಣ *font, ವರ್ಣ *buf)
{
    ವರ್ಣ    buf2[TEMP_BUF_SIZE];
    ವರ್ಣ    buf3[TEMP_BUF_SIZE];

    get_part(font, 2, buf2);
    get_part(font, 1, buf3);

    ಯದಿ (*buf3 != NUL)
	vim_snprintf(buf, TEMP_BUF_SIZE, "%s (%s)", buf2, buf3);
    ಅನ್ಯಥಾ
	vim_snprintf(buf, TEMP_BUF_SIZE, "%s", buf2);
}

/*
 * Given a font name this function returns the part used in the second scroll list.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
style_part(ವರ್ಣ *font, ವರ್ಣ *buf)
{
    ವರ್ಣ    buf2[TEMP_BUF_SIZE];
    ವರ್ಣ    buf3[TEMP_BUF_SIZE];

    get_part(font, 3, buf3);
    get_part(font, 5, buf2);

    ಯದಿ (!strcmp(buf2, "normal") && !strcmp(buf2, "Normal")
						   && !strcmp(buf2, "NORMAL"))
	vim_snprintf(buf, TEMP_BUF_SIZE, "%s %s", buf3, buf2);
    ಅನ್ಯಥಾ
	strcpy(buf, buf3);

    get_part(font, 6, buf2);

    ಯದಿ (buf2[0] != '\0')
	vim_snprintf(buf3, TEMP_BUF_SIZE, "%s %s", buf, buf2);
    ಅನ್ಯಥಾ
	strcpy(buf3, buf);

    get_part(font, 4, buf2);

    ಯದಿ (!strcmp(buf2, "o") || !strcmp(buf2, "O"))
	vim_snprintf(buf, TEMP_BUF_SIZE, "%s oblique", buf3);
    ಅನ್ಯಥಾ ಯದಿ (!strcmp(buf2, "i") || !strcmp(buf2, "I"))
	vim_snprintf(buf, TEMP_BUF_SIZE, "%s italic", buf3);

    ಯದಿ (!strcmp(buf, " "))
	strcpy(buf, "-");
}

/*
 * Given a font name this function returns the part used in the third
 * scroll list.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
size_part(ವರ್ಣ *font, ವರ್ಣ *buf, ಪೂರ್ಣಾಙ್ಕ inPixels)
{
    ಪೂರ್ಣಾಙ್ಕ	    size;
    ಭಿನ್ನ   temp;

    *buf = '\0';

    ಯದಿ (inPixels)
    {
	get_part(font, 7, buf);
	ಯದಿ (*buf != NUL)
	{
	    size = atoi(buf);
	    sprintf(buf, "%3d", size);
	}
    }
    ಅನ್ಯಥಾ
    {
	get_part(font, 8, buf);
	ಯದಿ (*buf != NUL)
	{
	    size = atoi(buf);
	    temp = (ಭಿನ್ನ)size / 10.0;
	    size = temp;
	    ಯದಿ (buf[strlen(buf) - 1] == '0')
		sprintf(buf, "%3d", size);
	    ಅನ್ಯಥಾ
		sprintf(buf, "%4.1f", temp);
	}
    }
}

/*
 * Given a font name this function returns the part used in the choice menu.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
encoding_part(ವರ್ಣ *font, ವರ್ಣ *buf)
{
    ವರ್ಣ    buf1[TEMP_BUF_SIZE];
    ವರ್ಣ    buf2[TEMP_BUF_SIZE];

    *buf = '\0';

    get_part(font, 13, buf1);
    get_part(font, 14, buf2);

    ಯದಿ (*buf1 != NUL && *buf2 != NUL)
	vim_snprintf(buf, TEMP_BUF_SIZE, "%s-%s", buf1, buf2);
    ಯದಿ (!strcmp(buf, " "))
	strcpy(buf, "-");
}

/*
 * Inserts a string into correct sorted position in a list.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
add_to_list(ವರ್ಣ **buf, ವರ್ಣ *item, ಪೂರ್ಣಾಙ್ಕ *ಎಣಿಕೆ)
{
    ಪೂರ್ಣಾಙ್ಕ	i;
    ಪೂರ್ಣಾಙ್ಕ j;

    ಯದಿ (*ಎಣಿಕೆ == MAX_ENTRIES_IN_LIST)
	ಪ್ರತ್ಯಾಗಮ;

    // avoid duplication
    ಪ್ರತಿ (i = 0; i < *ಎಣಿಕೆ; ++i)
    {
	ಯದಿ (!strcmp(buf[i], item))
	    ಪ್ರತ್ಯಾಗಮ;
    }

    // find order place, but make sure that wild card comes first
    ಯದಿ (!strcmp(item, wild))
	i = 0;
    ಅನ್ಯಥಾ
	ಪ್ರತಿ (i = 0; i < *ಎಣಿಕೆ; ++i)
	    ಯದಿ (strcmp(buf[i], item) > 0 && strcmp(buf[i], wild))
		ಭಙ್ಗ;

    // now insert the item
    ಪ್ರತಿ (j = *ಎಣಿಕೆ; j > i; --j)
	buf[j] = buf[j-1];
    buf[i] = XtNewString(item);

    ++(*ಎಣಿಕೆ);
}

/*
 * True if the font matches some field.
 */
    ಸ್ಥಿರ Boolean
match(SharedFontSelData *data, ಗಣನಾ ListSpecifier l, ಪೂರ್ಣಾಙ್ಕ i)
{
    ವರ್ಣ buf[TEMP_BUF_SIZE];

    // An empty selection or a wild card matches anything.
    ಯದಿ (!data->sel[l] || !strcmp(data->sel[l], wild))
	ಪ್ರತ್ಯಾಗಮ True;

    // chunk out the desired part...
    ವಿಕಲ್ಪ (l)
    {
	ಪಕ್ಷ ENCODING:
	    encoding_part(fn(data, i), buf);
	    ಭಙ್ಗ;

	ಪಕ್ಷ NAME:
	    name_part(fn(data, i), buf);
	    ಭಙ್ಗ;

	ಪಕ್ಷ STYLE:
	    style_part(fn(data, i), buf);
	    ಭಙ್ಗ;

	ಪಕ್ಷ SIZE:
	    size_part(fn(data, i), buf, data->in_pixels);
	    ಭಙ್ಗ;
	ಸ್ವಭಾವ:
	    ;
    }

    // ...and chew it now

    ಪ್ರತ್ಯಾಗಮ !strcmp(buf, data->sel[l]);
}

    ಸ್ಥಿರ Boolean
proportional(ವರ್ಣ *font)
{
    ವರ್ಣ buf[TEMP_BUF_SIZE];

    get_part(font, 11, buf);

    ಪ್ರತ್ಯಾಗಮ !strcmp(buf, "p") || !strcmp(buf, "P");
}


ಸ್ಥಿರ ಶೂನ್ಯ encoding_callback(Widget w, SharedFontSelData *data,
							     XtPointer dummy);

/*
 * Parse through the fontlist data and set up the three scroll lists.  The fix
 * parameter can be used to exclude a list from any changes.  This is used for
 * updates after selections caused by the users actions.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
fill_lists(ಗಣನಾ ListSpecifier fix, SharedFontSelData *data)
{
    ವರ್ಣ	*list[NONE][MAX_ENTRIES_IN_LIST];
    ಪೂರ್ಣಾಙ್ಕ		ಎಣಿಕೆ[NONE];
    ವರ್ಣ	buf[TEMP_BUF_SIZE];
    XmString	ವಸ್ತುಗಳು[MAX_ENTRIES_IN_LIST];
    ಪೂರ್ಣಾಙ್ಕ		i;
    ಪೂರ್ಣಾಙ್ಕ		idx;

    ಪ್ರತಿ (idx = (ಪೂರ್ಣಾಙ್ಕ)ENCODING; idx < (ಪೂರ್ಣಾಙ್ಕ)NONE; ++idx)
	ಎಣಿಕೆ[idx] = 0;

    // First we insert the wild char into every single list.
    ಯದಿ (fix != ENCODING)
	add_to_list(list[ENCODING], wild, &ಎಣಿಕೆ[ENCODING]);
    ಯದಿ (fix != NAME)
	add_to_list(list[NAME], wild, &ಎಣಿಕೆ[NAME]);
    ಯದಿ (fix != STYLE)
	add_to_list(list[STYLE], wild, &ಎಣಿಕೆ[STYLE]);
    ಯದಿ (fix != SIZE)
	add_to_list(list[SIZE], wild, &ಎಣಿಕೆ[SIZE]);

    ಪ್ರತಿ (i = 0; i < data->ಅಂಕೆ && i < MAX_ENTRIES_IN_LIST; i++)
    {
	ಯದಿ (proportional(fn(data, i)))
	    ಅನುವರ್ತ;

	ಯದಿ (fix != ENCODING
		&& match(data, NAME, i)
		&& match(data, STYLE, i)
		&& match(data, SIZE, i))
	{
	    encoding_part(fn(data, i), buf);
	    add_to_list(list[ENCODING], buf, &ಎಣಿಕೆ[ENCODING]);
	}

	ಯದಿ (fix != NAME
		&& match(data, ENCODING, i)
		&& match(data, STYLE, i)
		&& match(data, SIZE, i))
	{
	    name_part(fn(data, i), buf);
	    add_to_list(list[NAME], buf, &ಎಣಿಕೆ[NAME]);
	}

	ಯದಿ (fix != STYLE
		&& match(data, ENCODING, i)
		&& match(data, NAME, i)
		&& match(data, SIZE, i))
	{
	    style_part(fn(data, i), buf);
	    add_to_list(list[STYLE], buf, &ಎಣಿಕೆ[STYLE]);
	}

	ಯದಿ (fix != SIZE
		&& match(data, ENCODING, i)
		&& match(data, NAME, i)
		&& match(data, STYLE, i))
	{
	    size_part(fn(data, i), buf, data->in_pixels);
	    add_to_list(list[SIZE], buf, &ಎಣಿಕೆ[SIZE]);
	}
    }

    /*
     * And now do the preselection in all lists where there was one:
     */

    ಯದಿ (fix != ENCODING)
    {
	Cardinal n_ವಸ್ತುಗಳು;
	WidgetList children;
	Widget selected_button = 0;

	// Get and update the current button list.
	Xt_Va_Get_ಮೌಲ್ಯಗಳು(data->encoding_pulldown,
		XmNchildren, &children,
		XmNnumChildren, &n_ವಸ್ತುಗಳು,
		ರಿಕ್ತ);

	ಪ್ರತಿ (i = 0; i < ಎಣಿಕೆ[ENCODING]; ++i)
	{
	    Widget button;

	    ವಸ್ತುಗಳು[i] = XmStringCreateLocalized(list[ENCODING][i]);

	    ಯದಿ (i < (ಪೂರ್ಣಾಙ್ಕ)n_ವಸ್ತುಗಳು)
	    {
		// recycle old button
		Xt_Va_Set_ಮೌಲ್ಯಗಳು(children[i],
			XmNlabelString, ವಸ್ತುಗಳು[i],
			XmNuserData, i,
			ರಿಕ್ತ);
		button = children[i];
	    }
	    ಅನ್ಯಥಾ
	    {
		// create a new button
		button = XtVaCreateManagedWidget("button",
			xm_ತಳ್ಳು_Button_Gadget_Class,
			data->encoding_pulldown,
			XmNlabelString, ವಸ್ತುಗಳು[i],
			XmNuserData, i,
			ರಿಕ್ತ);
		XtAddCallback(button, XmNactivateCallback,
			(XtCallbackProc) encoding_callback, (XtPointer) data);
		XtManageChild(button);
	    }

	    ಯದಿ (data->sel[ENCODING])
	    {
		ಯದಿ (!strcmp(data->sel[ENCODING], list[ENCODING][i]))
		    selected_button = button;
	    }
	    XtFree(list[ENCODING][i]);
	}

	// Destroy all the outstanding menu items.
	ಪ್ರತಿ (i = ಎಣಿಕೆ[ENCODING]; i < (ಪೂರ್ಣಾಙ್ಕ)n_ವಸ್ತುಗಳು; ++i)
	{
	    XtUnmanageChild(children[i]);
	    XtDestroyWidget(children[i]);
	}

	// Preserve the current selection visually.
	ಯದಿ (selected_button)
	{
	    Xt_Va_Set_ಮೌಲ್ಯಗಳು(data->encoding_menu,
		    XmNmenuHistory, selected_button,
		    ರಿಕ್ತ);
	}

	ಪ್ರತಿ (i = 0; i < ಎಣಿಕೆ[ENCODING]; ++i)
	    XmStringFree(ವಸ್ತುಗಳು[i]);
    }

    /*
     * Now loop through the remaining lists and set them up.
     */
    ಪ್ರತಿ (idx = (ಪೂರ್ಣಾಙ್ಕ)NAME; idx < (ಪೂರ್ಣಾಙ್ಕ)NONE; ++idx)
    {
	Widget w;

	ಯದಿ (fix == (ಗಣನಾ ListSpecifier)idx)
	    ಅನುವರ್ತ;

	ವಿಕಲ್ಪ ((ಗಣನಾ ListSpecifier)idx)
	{
	    ಪಕ್ಷ NAME:
		w = data->list[NAME];
		ಭಙ್ಗ;
	    ಪಕ್ಷ STYLE:
		w = data->list[STYLE];
		ಭಙ್ಗ;
	    ಪಕ್ಷ SIZE:
		w = data->list[SIZE];
		ಭಙ್ಗ;
	    ಸ್ವಭಾವ:
		w = (Widget)0;	// for lint
	}

	ಪ್ರತಿ (i = 0; i < ಎಣಿಕೆ[idx]; ++i)
	{
	    ವಸ್ತುಗಳು[i] = XmStringCreateLocalized(list[idx][i]);
	    XtFree(list[idx][i]);
	}
	Xm_List_Delete_All_ವಸ್ತುಗಳು(w);
	Xm_List_Add_ವಸ್ತುಗಳು(w, ವಸ್ತುಗಳು, ಎಣಿಕೆ[idx], 1);
	ಯದಿ (data->sel[idx])
	{
	    XmStringFree(ವಸ್ತುಗಳು[0]);
	    ವಸ್ತುಗಳು[0] = XmStringCreateLocalized(data->sel[idx]);
	    XmListSelectItem(w, ವಸ್ತುಗಳು[0], False);
	    XmListSetBottomItem(w, ವಸ್ತುಗಳು[0]);
	}
	ಪ್ರತಿ (i = 0; i < ಎಣಿಕೆ[idx]; ++i)
	    XmStringFree(ವಸ್ತುಗಳು[i]);
    }
}

    ಸ್ಥಿರ ಶೂನ್ಯ
stoggle_callback(Widget w UNUSED,
	SharedFontSelData *data,
	XmToggleButtonCallbackStruct *call_data)
{
    ಪೂರ್ಣಾಙ್ಕ		i, do_sel;
    ವರ್ಣ	newSize[TEMP_BUF_SIZE];
    XmString	str;

    ಯದಿ (call_data->reason != (ಪೂರ್ಣಾಙ್ಕ)Xm_CR_ಮೌಲ್ಯ_CHANGED)
	ಪ್ರತ್ಯಾಗಮ;

    do_sel = (data->sel[SIZE] != ರಿಕ್ತ) && strcmp(data->sel[SIZE], wild);

    ಪ್ರತಿ (i = 0; do_sel && (i < data->ಅಂಕೆ); i++)
	ಯದಿ (match(data, ENCODING, i)
		&& match(data, NAME, i)
		&& match(data, STYLE, i)
		&& match(data, SIZE, i))
	{
	    size_part(fn(data, i), newSize, !data->in_pixels);
	    ಭಙ್ಗ;
	}

    data->in_pixels = !data->in_pixels;

    ಯದಿ (data->sel[SIZE])
	XtFree(data->sel[SIZE]);
    data->sel[SIZE] = ರಿಕ್ತ;
    fill_lists(NONE, data);

    ಯದಿ (do_sel)
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
    ಸ್ಥಿರ ಶೂನ್ಯ
display_sample(SharedFontSelData *data)
{
    Arg		    args[2];
    ಪೂರ್ಣಾಙ್ಕ		    n;
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

    Xt_Set_ಮೌಲ್ಯಗಳು(data->sample, args, n);
    XmStringFree(str);

    ಯದಿ (data->old)
    {
	XFreeFont(display, data->old);
	XmFontListFree(data->old_list);
    }
    data->old = font;
    data->old_list = font_list;
}


    ಸ್ಥಿರ Boolean
do_choice(Widget w,
	SharedFontSelData *data,
	XmListCallbackStruct *call_data,
	ಗಣನಾ ListSpecifier which)
{
    ವರ್ಣ *sel;

    XmStringGetLtoR(call_data->item, XmSTRING_DEFAULT_CHARSET, &sel);

    ಯದಿ (!data->sel[which])
	data->sel[which] = XtNewString(sel);
    ಅನ್ಯಥಾ
    {
	ಯದಿ (!strcmp(data->sel[which], sel))
	{
	    // unselecting current selection
	    XtFree(data->sel[which]);
	    data->sel[which] = ರಿಕ್ತ;
	    ಯದಿ (w)
		XmListDeselectItem(w, call_data->item);
	}
	ಅನ್ಯಥಾ
	{
	    XtFree(data->sel[which]);
	    data->sel[which] = XtNewString(sel);
	}
    }
    XtFree(sel);

    fill_lists(which, data);

    // If there is a font selection, we display it.
    ಯದಿ (data->sel[ENCODING]
	    && data->sel[NAME]
	    && data->sel[STYLE]
	    && data->sel[SIZE]
	    && strcmp(data->sel[ENCODING], wild)
	    && strcmp(data->sel[NAME], wild)
	    && strcmp(data->sel[STYLE], wild)
	    && strcmp(data->sel[SIZE], wild))
    {
	ಪೂರ್ಣಾಙ್ಕ i;

	ಯದಿ (data->font_name)
	    XtFree(data->font_name);
	data->font_name = ರಿಕ್ತ;

	ಪ್ರತಿ (i = 0; i < data->ಅಂಕೆ; i++)
	{
	    ಯದಿ (match(data, ENCODING, i)
		    && match(data, NAME, i)
		    && match(data, STYLE, i)
		    && match(data, SIZE, i))
	    {
		data->font_name = XtNewString(fn(data, i));
		ಭಙ್ಗ;
	    }
	}

	ಯದಿ (data->font_name)
	{
	    XmTextSetString(data->name, data->font_name);
	    display_sample(data);
	}
	ಅನ್ಯಥಾ
	    do_dialog(VIM_ERROR,
		    (char_u *)_("Error"),
		    (char_u *)_("Invalid font specification"),
		    (char_u *)_("&Dismiss"), 1, ರಿಕ್ತ, FALSE);

	ಪ್ರತ್ಯಾಗಮ True;
    }
    ಅನ್ಯಥಾ
    {
	ಪೂರ್ಣಾಙ್ಕ	    n;
	XmString    str;
	Arg	    args[4];
	ವರ್ಣ	    *nomatch_ಸಂದೇಶ = _("no specific match");

	n = 0;
	str = XmStringCreateLocalized(nomatch_ಸಂದೇಶ);
	XtSetArg(args[n], XmNlabelString, str); ++n;
	Xt_Set_ಮೌಲ್ಯಗಳು(data->sample, args, n);
	apply_fontlist(data->sample);
	XmTextSetString(data->name, nomatch_ಸಂದೇಶ);
	XmStringFree(str);

	ಪ್ರತ್ಯಾಗಮ False;
    }
}

    ಸ್ಥಿರ ಶೂನ್ಯ
encoding_callback(Widget w,
	SharedFontSelData *data,
	XtPointer dummy UNUSED)
{
    XmString str;
    XmListCallbackStruct fake_data;

    Xt_Va_Get_ಮೌಲ್ಯಗಳು(w, XmNlabelString, &str, ರಿಕ್ತ);

    ಯದಿ (!str)
	ಪ್ರತ್ಯಾಗಮ;

    fake_data.item = str;

    do_choice(0, data, &fake_data, ENCODING);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
name_callback(Widget w,
	SharedFontSelData *data,
	XmListCallbackStruct *call_data)
{
    do_choice(w, data, call_data, NAME);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
style_callback(Widget w,
	SharedFontSelData *data,
	XmListCallbackStruct *call_data)
{
    do_choice(w, data, call_data, STYLE);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
size_callback(Widget w,
	SharedFontSelData *data,
	XmListCallbackStruct *call_data)
{
    do_choice(w, data, call_data, SIZE);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
cancel_callback(Widget w UNUSED,
	SharedFontSelData *data,
	XmListCallbackStruct *call_data UNUSED)
{
    ಯದಿ (data->sel[ENCODING])
    {
	XtFree(data->sel[ENCODING]);
	data->sel[ENCODING] = ರಿಕ್ತ;
    }
    ಯದಿ (data->sel[NAME])
    {
	XtFree(data->sel[NAME]);
	data->sel[NAME] = ರಿಕ್ತ;
    }
    ಯದಿ (data->sel[STYLE])
    {
	XtFree(data->sel[STYLE]);
	data->sel[STYLE] = ರಿಕ್ತ;
    }
    ಯದಿ (data->sel[SIZE])
    {
	XtFree(data->sel[SIZE]);
	data->sel[SIZE] = ರಿಕ್ತ;
    }

    ಯದಿ (data->font_name)
	XtFree(data->font_name);
    data->font_name = ರಿಕ್ತ;

    data->ಅಂಕೆ = 0;
    XFreeFontNames(data->names);
    data->names = ರಿಕ್ತ;
    data->exit = True;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
ok_callback(Widget w UNUSED,
	SharedFontSelData *data,
	Xm_ತಳ್ಳು_Button_Callback_Struct *call_data UNUSED)
{
    ವರ್ಣ    *pattern;
    ವರ್ಣ    **name;
    ಪೂರ್ಣಾಙ್ಕ	    i;

    pattern = XmTextGetString(data->name);
    name    = XListFonts(XtDisplay(data->dialog), pattern, 1, &i);
    XtFree(pattern);

    ಯದಿ (i != 1)
    {
	do_dialog(VIM_ERROR,
		(char_u *)_("Error"),
		(char_u *)_("Invalid font specification"),
		(char_u *)_("&Dismiss"), 1, ರಿಕ್ತ, FALSE);
	XFreeFontNames(name);
    }
    ಅನ್ಯಥಾ
    {
	ಯದಿ (data->font_name)
	    XtFree(data->font_name);
	data->font_name = XtNewString(name[0]);

	ಯದಿ (data->sel[ENCODING])
	{
	    XtFree(data->sel[ENCODING]);
	    data->sel[ENCODING] = ರಿಕ್ತ;
	}
	ಯದಿ (data->sel[NAME])
	{
	    XtFree(data->sel[NAME]);
	    data->sel[NAME] = ರಿಕ್ತ;
	}
	ಯದಿ (data->sel[STYLE])
	{
	    XtFree(data->sel[STYLE]);
	    data->sel[STYLE] = ರಿಕ್ತ;
	}
	ಯದಿ (data->sel[SIZE])
	{
	    XtFree(data->sel[SIZE]);
	    data->sel[SIZE] = ರಿಕ್ತ;
	}

	XFreeFontNames(name);

	data->ಅಂಕೆ = 0;
	XFreeFontNames(data->names);
	data->names = ರಿಕ್ತ;
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
    ಸ್ಥಿರ SharedFontSelData	_data;

    Widget		parent;
    Widget		form;
    Widget		separator;
    Widget		sub_form;
    Widget		size_toggle;
    Widget		name;
    Widget		disp_frame;
    Widget		frame;
    Arg			args[64];
    ಪೂರ್ಣಾಙ್ಕ			n;
    XmString		str;
    ವರ್ಣ		big_font[MAX_FONT_NAME_LEN];
    SharedFontSelData	*data;

    data = &_data;

    parent = vimShell;
    data->names = XListFonts(XtDisplay(parent), "-*-*-*-*-*-*-*-*-*-*-*-*-*-*",
						       MAX_FONTS, &data->ಅಂಕೆ);

    /*
     * Find the name of the biggest font less than the given limit
     * MAX_DISPLAY_SIZE used to set up the initial height of the display
     * widget.
     */

    {
	ಪೂರ್ಣಾಙ್ಕ	i;
	ಪೂರ್ಣಾಙ್ಕ	max;
	ಪೂರ್ಣಾಙ್ಕ	idx = 0;
	ಪೂರ್ಣಾಙ್ಕ	size;
	ವರ್ಣ	buf[128];

	ಪ್ರತಿ (i = 0, max = 0; i < data->ಅಂಕೆ; i++)
	{
	    get_part(fn(data, i), 7, buf);
	    size = atoi(buf);
	    ಯದಿ ((size > max) && (size < MAX_DISPLAY_SIZE))
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
    data->dialog = XmCreateDialogShell(parent, "fontSelector", ರಿಕ್ತ, 0);
    str = XmStringCreateLocalized(_("Vim - Font Selector"));

    // Create form popup dialog widget.
    form = XtVaCreateWidget("form",
	    xmFormWidgetClass, data->dialog,
	    XmNdialogTitle, str,
	    XmNautoUnmanage, False,
	    XmNdialogStyle, XmDIALOG_FULL_APPLICATION_MODAL,
	    ರಿಕ್ತ);
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
	    ರಿಕ್ತ);

    data->ok = XtVaCreateManagedWidget(_("OK"),
	    xm_ತಳ್ಳು_Button_Gadget_Class, sub_form,
	    XmNleftAttachment, XmATTACH_FORM,
	    XmNrightAttachment, XmATTACH_FORM,
	    XmNtopAttachment, XmATTACH_FORM,
	    XmNtopOffset, 4,
	    ರಿಕ್ತ);
    apply_fontlist(data->ok);

    data->cancel = XtVaCreateManagedWidget(_("Cancel"),
	    xm_ತಳ್ಳು_Button_Gadget_Class, sub_form,
	    XmNrightAttachment, XmATTACH_FORM,
	    XmNleftAttachment, XmATTACH_FORM,
	    XmNtopAttachment, XmATTACH_WIDGET,
	    XmNtopWidget, data->ok,
	    XmNtopOffset, 4,
	    XmNshowAsDefault, True,
	    ರಿಕ್ತ);
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
	    XmNeditMode, Xm_SINGLE_ಸಾಲು_EDIT,
	    XmNmaxLength, MAX_FONT_NAME_LEN,
	    XmNcolumns, 60,
	    ರಿಕ್ತ);

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
	    ರಿಕ್ತ);
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
	    ರಿಕ್ತ);

    data->sample = XtVaCreateManagedWidget("sampleLabel",
	    xmLabelWidgetClass, disp_frame,
	    XmNleftAttachment, XmATTACH_FORM,
	    XmNtopAttachment, XmATTACH_FORM,
	    XmNbottomAttachment, XmATTACH_FORM,
	    XmNrightAttachment, XmATTACH_FORM,
	    XmNalignment, XmALIGNMENT_BEGINNING,
	    XmNrecomputeSize, False,
	    XmNfontList, data->old_list,
	    ರಿಕ್ತ);

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
	    ರಿಕ್ತ);
    XmStringFree(str);
    apply_fontlist(size_toggle);
    XtManageChild(size_toggle);

    // Encoding pulldown menu.

    data->encoding_pulldown = XmCreatePulldownMenu(form,
						 "encodingPulldown", ರಿಕ್ತ, 0);
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
	    ರಿಕ್ತ);

    // font list
    frame = XtVaCreateManagedWidget("frame", xmFrameWidgetClass, sub_form,
	    XmNshadowThickness, 0,
	    XmNtopAttachment, XmATTACH_FORM,
	    XmNbottomAttachment, XmATTACH_FORM,
	    XmNleftAttachment, XmATTACH_FORM,
	    XmNrightAttachment, XmATTACH_POSITION,
	    XmNrightPosition, 50,
	    ರಿಕ್ತ);

    str = XmStringCreateLocalized(_("Font:"));
    name = XtVaCreateManagedWidget("nameListLabel", xmLabelGadgetClass, frame,
	    XmNchildType, XmFRAME_TITLE_CHILD,
	    XmNchildVerticalAlignment, XmALIGNMENT_CENTER,
	    XmNchildHorizontalAlignment, XmALIGNMENT_BEGINNING,
	    XmNlabelString, str,
	    ರಿಕ್ತ);
    XmStringFree(str);
    apply_fontlist(name);

    n = 0;
    XtSetArg(args[n], Xm_Nvisible_Item_ಎಣಿಕೆ, 8); ++n;
    XtSetArg(args[n], XmNresizable, True); ++n;
    XtSetArg(args[n], XmNlistSizePolicy, XmCONSTANT); ++n;
    XtSetArg(args[n], XmNvisualPolicy, XmVARIABLE); ++n;
#ifdef LESSTIF_VERSION
    XtSetArg(args[n], XmNscrollBarDisplayPolicy, XmSTATIC); ++n;
#endif
    data->list[NAME] = XmCreateScrolledList(frame, "fontList", args, n);
    Xt_Va_Set_ಮೌಲ್ಯಗಳು(name, XmNuserData, data->list[NAME], ರಿಕ್ತ);

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
	    ರಿಕ್ತ);

    str = XmStringCreateLocalized(_("Style:"));
    name = XtVaCreateManagedWidget("styleListLabel", xmLabelWidgetClass, frame,
	    XmNchildType, XmFRAME_TITLE_CHILD,
	    XmNchildVerticalAlignment, XmALIGNMENT_CENTER,
	    XmNchildHorizontalAlignment, XmALIGNMENT_BEGINNING,
	    XmNlabelString, str,
	    ರಿಕ್ತ);
    XmStringFree(str);
    apply_fontlist(name);

    n = 0;
    XtSetArg(args[n], Xm_Nvisible_Item_ಎಣಿಕೆ, 8); ++n;
    XtSetArg(args[n], XmNresizable, True); ++n;
    XtSetArg(args[n], XmNlistSizePolicy, XmCONSTANT); ++n;
    XtSetArg(args[n], XmNvisualPolicy, XmVARIABLE); ++n;
#ifdef LESSTIF_VERSION
    XtSetArg(args[n], XmNscrollBarDisplayPolicy, XmSTATIC); ++n;
#endif
    data->list[STYLE] = XmCreateScrolledList(frame, "styleList", args, n);
    Xt_Va_Set_ಮೌಲ್ಯಗಳು(name, XmNuserData, data->list[STYLE], ರಿಕ್ತ);

    // size list
    frame = XtVaCreateManagedWidget("frame", xmFrameWidgetClass, sub_form,
	    XmNshadowThickness, 0,
	    XmNtopAttachment, XmATTACH_FORM,
	    XmNbottomAttachment, XmATTACH_FORM,
	    XmNleftAttachment, XmATTACH_POSITION,
	    XmNleftPosition, 80,
	    XmNleftOffset, 4,
	    XmNrightAttachment, XmATTACH_FORM,
	    ರಿಕ್ತ);

    str = XmStringCreateLocalized(_("Size:"));
    name = XtVaCreateManagedWidget("sizeListLabel", xmLabelGadgetClass, frame,
	    XmNchildType, XmFRAME_TITLE_CHILD,
	    XmNchildVerticalAlignment, XmALIGNMENT_CENTER,
	    XmNchildHorizontalAlignment, XmALIGNMENT_BEGINNING,
	    XmNlabelString, str,
	    ರಿಕ್ತ);
    XmStringFree(str);
    apply_fontlist(name);

    n = 0;
    XtSetArg(args[n], Xm_Nvisible_Item_ಎಣಿಕೆ, 8); ++n;
    XtSetArg(args[n], XmNresizable, True); ++n;
    XtSetArg(args[n], XmNlistSizePolicy, XmCONSTANT); ++n;
    XtSetArg(args[n], XmNvisualPolicy, XmVARIABLE); ++n;
#ifdef LESSTIF_VERSION
    XtSetArg(args[n], XmNscrollBarDisplayPolicy, XmSTATIC); ++n;
#endif
    data->list[SIZE] = XmCreateScrolledList(frame, "sizeList", args, n);
    Xt_Va_Set_ಮೌಲ್ಯಗಳು(name, XmNuserData, data->list[SIZE], ರಿಕ್ತ);

    // update form widgets cancel button
    Xt_Va_Set_ಮೌಲ್ಯಗಳು(form, XmNcancelButton, data->cancel, ರಿಕ್ತ);

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
    data->sel[ENCODING] = ರಿಕ್ತ;
    data->sel[NAME] = ರಿಕ್ತ;
    data->sel[STYLE] = ರಿಕ್ತ;
    data->sel[SIZE] = ರಿಕ್ತ;
    data->font_name = ರಿಕ್ತ;

    // set up current font parameters
    ಯದಿ (current && current[0] != '\0')
    {
	ಪೂರ್ಣಾಙ್ಕ	    i;
	ವರ್ಣ	    **names;

	names = XListFonts(XtDisplay(form), (ವರ್ಣ *) current, 1, &i);

	ಯದಿ (i != 0)
	{
	    ವರ್ಣ namebuf[TEMP_BUF_SIZE];
	    ವರ್ಣ stylebuf[TEMP_BUF_SIZE];
	    ವರ್ಣ sizebuf[TEMP_BUF_SIZE];
	    ವರ್ಣ encodingbuf[TEMP_BUF_SIZE];
	    ವರ್ಣ *found;

	    found = names[0];

	    name_part(found, namebuf);
	    style_part(found, stylebuf);
	    size_part(found, sizebuf, data->in_pixels);
	    encoding_part(found, encodingbuf);

	    ಯದಿ (*namebuf != NUL
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
	    ಅನ್ಯಥಾ
	    {
		// We can't preset a symbolic name, which isn't a full font
		// description. Therefore we just behave the same way as if the
		// user didn't have selected anything thus far.
		//
		// Unfortunately there is no known way to expand an abbreviated
		// font name.
		data->font_name = ರಿಕ್ತ;
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
    ಯಾವತ್ (!data->exit)
	XtAppProcessEvent(XtWidgetToApplicationContext(data->dialog),
							(Xt_ಒಳಹರಿವು_Mask)XtIMAll);

    ಯದಿ (data->old)
    {
	XFreeFont(XtDisplay(data->dialog),  data->old);
	XmFontListFree(data->old_list);
    }
    XtDestroyWidget(data->dialog);

    gui_motif_synch_fonts();

    ಪ್ರತ್ಯಾಗಮ (char_u *) data->font_name;
}
