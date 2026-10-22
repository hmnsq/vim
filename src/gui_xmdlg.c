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

#ଅନ୍ତର୍ଭାଵ "vim.h"

#ଅନ୍ତର୍ଭାଵ <Xm/Form.h>
#ଅନ୍ତର୍ଭାଵ <Xm/PushBG.h>
#ଅନ୍ତର୍ଭାଵ <Xm/Text.h>
#ଅନ୍ତର୍ଭାଵ <Xm/TextF.h>
#ଅନ୍ତର୍ଭାଵ <Xm/Label.h>
#ଅନ୍ତର୍ଭାଵ <Xm/Frame.h>
#ଅନ୍ତର୍ଭାଵ <Xm/LabelG.h>
#ଅନ୍ତର୍ଭାଵ <Xm/ToggleBG.h>
#ଅନ୍ତର୍ଭାଵ <Xm/SeparatoG.h>
#ଅନ୍ତର୍ଭାଵ <Xm/DialogS.h>
#ଅନ୍ତର୍ଭାଵ <Xm/List.h>
#ଅନ୍ତର୍ଭାଵ <Xm/RowColumn.h>
#ଅନ୍ତର୍ଭାଵ <Xm/AtomMgr.h>
#ଅନ୍ତର୍ଭାଵ <Xm/Protocols.h>

#ଅନ୍ତର୍ଭାଵ <X11/keysym.h>
#ଅନ୍ତର୍ଭାଵ <X11/Xatom.h>
#ଅନ୍ତର୍ଭାଵ <X11/StringDefs.h>
#ଅନ୍ତର୍ଭାଵ <X11/Intrinsic.h>

ବାହ୍ୟ Widget vimShell;

#ifdef FEAT_MENU
# define apply_fontlist(w) gui_motif_menu_fontlist(w)
#ଅନ୍ୟଥା
# define apply_fontlist(w)
#endif

/////////////////////////////////////////////////////////////////////////////
// Font selection dialogue implementation.

ସ୍ଥିର ଅକ୍ଷର wild[3] = "*";

/*
 * FIXME: This is a generic function, which should be used throughout the whole
 * application.
 *
 * Add close_callback, which will be called when the user selects close from
 * the window menu.  The close menu item usually activates f.kill which sends a
 * WM_DELETE_WINDOW protocol request for the window.
 */

    ସ୍ଥିର ଶୂନ୍ୟ
add_cancel_action(Widget shell, XtCallbackProc close_callback, ଶୂନ୍ୟ *arg)
{
    ସ୍ଥିର Atom wmp_atom = 0;
    ସ୍ଥିର Atom dw_atom = 0;
    Display *display = XtDisplay(shell);

    // deactivate the built-in delete response of killing the application
    Xt_Va_Set_ମୂଲ୍ୟଗୁଡ଼ିକ(shell, XmNdeleteResponse, XmDO_NOTHING, ଖାଲି);

    // add a delete window protocol callback instead
    ଯଦି (!dw_atom)
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

ଗଣନା ListSpecifier
{
    ENCODING,
    NAME,
    STYLE,
    SIZE,
    NONE
};

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା _SharedFontSelData
{
    Widget	dialog;
    Widget	ok;
    Widget	cancel;
    Widget	encoding_pulldown;
    Widget	encoding_menu;
    Widget	list[NONE];
    Widget	name;
    Widget	sample;
    ଅକ୍ଷର	**names;	// font name array of arrays
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ଅଙ୍କ;		// number of font names
    String	sel[NONE];	// selection category
    Boolean	in_pixels;	// toggle state - size in pixels
    ଅକ୍ଷର	*font_name;	// current font name
    XFontStruct	*old;		// font data structure for sample display
    XmFontList	old_list;	// font data structure for sample display
    Boolean	exit;		// used for program exit control
} SharedFontSelData;

/*
 * Checking access to the font name array for validity.
 */
    ସ୍ଥିର ଅକ୍ଷର *
fn(SharedFontSelData *data, ପୂର୍ଣ୍ଣସଂଖ୍ୟା i)
{
    // Assertion checks:
    ଯଦି (data->ଅଙ୍କ < 0)
	abort();
    ଯଦି (i >= data->ଅଙ୍କ)
	i = data->ଅଙ୍କ - 1;
    ଯଦି (i < 0)
	i = 0;

    ଫେରନ୍ତୁ data->names[i];
}

/*
 * Get a specific substring from a font name.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
get_part(ଅକ୍ଷର *in, ପୂର୍ଣ୍ଣସଂଖ୍ୟା pos, ଅକ୍ଷର *out)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା j;

    *out = '\0';

    ପାଇଁ (i = 0; (pos > 0) && (in[i] != '\0'); ++i)
	ଯଦି (in[i] == '-')
	    pos--;

    ଯଦି (in[i] == '\0')
	ଫେରନ୍ତୁ;

    ପାଇଁ (j = 0; (in[i] != '-') && (in[i] != '\0'); ++i, ++j)
	out[j] = in[i];
    out[j] = '\0';
}

/*
 * Given a font name this function returns the part used in the first
 * scroll list.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
name_part(ଅକ୍ଷର *font, ଅକ୍ଷର *buf)
{
    ଅକ୍ଷର    buf2[TEMP_BUF_SIZE];
    ଅକ୍ଷର    buf3[TEMP_BUF_SIZE];

    get_part(font, 2, buf2);
    get_part(font, 1, buf3);

    ଯଦି (*buf3 != NUL)
	vim_snprintf(buf, TEMP_BUF_SIZE, "%s (%s)", buf2, buf3);
    ଅନ୍ୟଥା
	vim_snprintf(buf, TEMP_BUF_SIZE, "%s", buf2);
}

/*
 * Given a font name this function returns the part used in the second scroll list.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
style_part(ଅକ୍ଷର *font, ଅକ୍ଷର *buf)
{
    ଅକ୍ଷର    buf2[TEMP_BUF_SIZE];
    ଅକ୍ଷର    buf3[TEMP_BUF_SIZE];

    get_part(font, 3, buf3);
    get_part(font, 5, buf2);

    ଯଦି (!strcmp(buf2, "normal") && !strcmp(buf2, "Normal")
						   && !strcmp(buf2, "NORMAL"))
	vim_snprintf(buf, TEMP_BUF_SIZE, "%s %s", buf3, buf2);
    ଅନ୍ୟଥା
	strcpy(buf, buf3);

    get_part(font, 6, buf2);

    ଯଦି (buf2[0] != '\0')
	vim_snprintf(buf3, TEMP_BUF_SIZE, "%s %s", buf, buf2);
    ଅନ୍ୟଥା
	strcpy(buf3, buf);

    get_part(font, 4, buf2);

    ଯଦି (!strcmp(buf2, "o") || !strcmp(buf2, "O"))
	vim_snprintf(buf, TEMP_BUF_SIZE, "%s oblique", buf3);
    ଅନ୍ୟଥା ଯଦି (!strcmp(buf2, "i") || !strcmp(buf2, "I"))
	vim_snprintf(buf, TEMP_BUF_SIZE, "%s italic", buf3);

    ଯଦି (!strcmp(buf, " "))
	strcpy(buf, "-");
}

/*
 * Given a font name this function returns the part used in the third
 * scroll list.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
size_part(ଅକ୍ଷର *font, ଅକ୍ଷର *buf, ପୂର୍ଣ୍ଣସଂଖ୍ୟା inPixels)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    size;
    ଦଶମିକ   temp;

    *buf = '\0';

    ଯଦି (inPixels)
    {
	get_part(font, 7, buf);
	ଯଦି (*buf != NUL)
	{
	    size = atoi(buf);
	    sprintf(buf, "%3d", size);
	}
    }
    ଅନ୍ୟଥା
    {
	get_part(font, 8, buf);
	ଯଦି (*buf != NUL)
	{
	    size = atoi(buf);
	    temp = (ଦଶମିକ)size / 10.0;
	    size = temp;
	    ଯଦି (buf[strlen(buf) - 1] == '0')
		sprintf(buf, "%3d", size);
	    ଅନ୍ୟଥା
		sprintf(buf, "%4.1f", temp);
	}
    }
}

/*
 * Given a font name this function returns the part used in the choice menu.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
encoding_part(ଅକ୍ଷର *font, ଅକ୍ଷର *buf)
{
    ଅକ୍ଷର    buf1[TEMP_BUF_SIZE];
    ଅକ୍ଷର    buf2[TEMP_BUF_SIZE];

    *buf = '\0';

    get_part(font, 13, buf1);
    get_part(font, 14, buf2);

    ଯଦି (*buf1 != NUL && *buf2 != NUL)
	vim_snprintf(buf, TEMP_BUF_SIZE, "%s-%s", buf1, buf2);
    ଯଦି (!strcmp(buf, " "))
	strcpy(buf, "-");
}

/*
 * Inserts a string into correct sorted position in a list.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
add_to_list(ଅକ୍ଷର **buf, ଅକ୍ଷର *item, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ଗଣନା_2)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା j;

    ଯଦି (*ଗଣନା_2 == MAX_ENTRIES_IN_LIST)
	ଫେରନ୍ତୁ;

    // avoid duplication
    ପାଇଁ (i = 0; i < *ଗଣନା_2; ++i)
    {
	ଯଦି (!strcmp(buf[i], item))
	    ଫେରନ୍ତୁ;
    }

    // find order place, but make sure that wild card comes first
    ଯଦି (!strcmp(item, wild))
	i = 0;
    ଅନ୍ୟଥା
	ପାଇଁ (i = 0; i < *ଗଣନା_2; ++i)
	    ଯଦି (strcmp(buf[i], item) > 0 && strcmp(buf[i], wild))
		ଭାଙ୍ଗ;

    // now insert the item
    ପାଇଁ (j = *ଗଣନା_2; j > i; --j)
	buf[j] = buf[j-1];
    buf[i] = XtNewString(item);

    ++(*ଗଣନା_2);
}

/*
 * True if the font matches some field.
 */
    ସ୍ଥିର Boolean
match(SharedFontSelData *data, ଗଣନା ListSpecifier l, ପୂର୍ଣ୍ଣସଂଖ୍ୟା i)
{
    ଅକ୍ଷର buf[TEMP_BUF_SIZE];

    // An empty selection or a wild card matches anything.
    ଯଦି (!data->sel[l] || !strcmp(data->sel[l], wild))
	ଫେରନ୍ତୁ True;

    // chunk out the desired part...
    ବାଛ (l)
    {
	ସ୍ଥିତି ENCODING:
	    encoding_part(fn(data, i), buf);
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି NAME:
	    name_part(fn(data, i), buf);
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି STYLE:
	    style_part(fn(data, i), buf);
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି SIZE:
	    size_part(fn(data, i), buf, data->in_pixels);
	    ଭାଙ୍ଗ;
	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    ;
    }

    // ...and chew it now

    ଫେରନ୍ତୁ !strcmp(buf, data->sel[l]);
}

    ସ୍ଥିର Boolean
proportional(ଅକ୍ଷର *font)
{
    ଅକ୍ଷର buf[TEMP_BUF_SIZE];

    get_part(font, 11, buf);

    ଫେରନ୍ତୁ !strcmp(buf, "p") || !strcmp(buf, "P");
}


ସ୍ଥିର ଶୂନ୍ୟ encoding_callback(Widget w, SharedFontSelData *data,
							     XtPointer dummy);

/*
 * Parse through the fontlist data and set up the three scroll lists.  The fix
 * parameter can be used to exclude a list from any changes.  This is used for
 * updates after selections caused by the users actions.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
fill_lists(ଗଣନା ListSpecifier fix, SharedFontSelData *data)
{
    ଅକ୍ଷର	*list[NONE][MAX_ENTRIES_IN_LIST];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ଗଣନା_2[NONE];
    ଅକ୍ଷର	buf[TEMP_BUF_SIZE];
    XmString	ବସ୍ତୁଗୁଡ଼ିକ[MAX_ENTRIES_IN_LIST];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		idx;

    ପାଇଁ (idx = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ENCODING; idx < (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)NONE; ++idx)
	ଗଣନା_2[idx] = 0;

    // First we insert the wild char into every single list.
    ଯଦି (fix != ENCODING)
	add_to_list(list[ENCODING], wild, &ଗଣନା_2[ENCODING]);
    ଯଦି (fix != NAME)
	add_to_list(list[NAME], wild, &ଗଣନା_2[NAME]);
    ଯଦି (fix != STYLE)
	add_to_list(list[STYLE], wild, &ଗଣନା_2[STYLE]);
    ଯଦି (fix != SIZE)
	add_to_list(list[SIZE], wild, &ଗଣନା_2[SIZE]);

    ପାଇଁ (i = 0; i < data->ଅଙ୍କ && i < MAX_ENTRIES_IN_LIST; i++)
    {
	ଯଦି (proportional(fn(data, i)))
	    ଜାରି;

	ଯଦି (fix != ENCODING
		&& match(data, NAME, i)
		&& match(data, STYLE, i)
		&& match(data, SIZE, i))
	{
	    encoding_part(fn(data, i), buf);
	    add_to_list(list[ENCODING], buf, &ଗଣନା_2[ENCODING]);
	}

	ଯଦି (fix != NAME
		&& match(data, ENCODING, i)
		&& match(data, STYLE, i)
		&& match(data, SIZE, i))
	{
	    name_part(fn(data, i), buf);
	    add_to_list(list[NAME], buf, &ଗଣନା_2[NAME]);
	}

	ଯଦି (fix != STYLE
		&& match(data, ENCODING, i)
		&& match(data, NAME, i)
		&& match(data, SIZE, i))
	{
	    style_part(fn(data, i), buf);
	    add_to_list(list[STYLE], buf, &ଗଣନା_2[STYLE]);
	}

	ଯଦି (fix != SIZE
		&& match(data, ENCODING, i)
		&& match(data, NAME, i)
		&& match(data, STYLE, i))
	{
	    size_part(fn(data, i), buf, data->in_pixels);
	    add_to_list(list[SIZE], buf, &ଗଣନା_2[SIZE]);
	}
    }

    /*
     * And now do the preselection in all lists where there was one:
     */

    ଯଦି (fix != ENCODING)
    {
	Cardinal n_ବସ୍ତୁଗୁଡ଼ିକ;
	WidgetList children;
	Widget selected_button = 0;

	// Get and update the current button list.
	Xt_Va_Get_ମୂଲ୍ୟଗୁଡ଼ିକ(data->encoding_pulldown,
		XmNchildren, &children,
		XmNnumChildren, &n_ବସ୍ତୁଗୁଡ଼ିକ,
		ଖାଲି);

	ପାଇଁ (i = 0; i < ଗଣନା_2[ENCODING]; ++i)
	{
	    Widget button;

	    ବସ୍ତୁଗୁଡ଼ିକ[i] = XmStringCreateLocalized(list[ENCODING][i]);

	    ଯଦି (i < (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)n_ବସ୍ତୁଗୁଡ଼ିକ)
	    {
		// recycle old button
		Xt_Va_Set_ମୂଲ୍ୟଗୁଡ଼ିକ(children[i],
			XmNlabelString, ବସ୍ତୁଗୁଡ଼ିକ[i],
			XmNuserData, i,
			ଖାଲି);
		button = children[i];
	    }
	    ଅନ୍ୟଥା
	    {
		// create a new button
		button = XtVaCreateManagedWidget("button",
			xm_ଠେଲ_Button_Gadget_Class,
			data->encoding_pulldown,
			XmNlabelString, ବସ୍ତୁଗୁଡ଼ିକ[i],
			XmNuserData, i,
			ଖାଲି);
		XtAddCallback(button, XmNactivateCallback,
			(XtCallbackProc) encoding_callback, (XtPointer) data);
		XtManageChild(button);
	    }

	    ଯଦି (data->sel[ENCODING])
	    {
		ଯଦି (!strcmp(data->sel[ENCODING], list[ENCODING][i]))
		    selected_button = button;
	    }
	    XtFree(list[ENCODING][i]);
	}

	// Destroy all the outstanding menu items.
	ପାଇଁ (i = ଗଣନା_2[ENCODING]; i < (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)n_ବସ୍ତୁଗୁଡ଼ିକ; ++i)
	{
	    XtUnmanageChild(children[i]);
	    XtDestroyWidget(children[i]);
	}

	// Preserve the current selection visually.
	ଯଦି (selected_button)
	{
	    Xt_Va_Set_ମୂଲ୍ୟଗୁଡ଼ିକ(data->encoding_menu,
		    XmNmenuHistory, selected_button,
		    ଖାଲି);
	}

	ପାଇଁ (i = 0; i < ଗଣନା_2[ENCODING]; ++i)
	    XmStringFree(ବସ୍ତୁଗୁଡ଼ିକ[i]);
    }

    /*
     * Now loop through the remaining lists and set them up.
     */
    ପାଇଁ (idx = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)NAME; idx < (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)NONE; ++idx)
    {
	Widget w;

	ଯଦି (fix == (ଗଣନା ListSpecifier)idx)
	    ଜାରି;

	ବାଛ ((ଗଣନା ListSpecifier)idx)
	{
	    ସ୍ଥିତି NAME:
		w = data->list[NAME];
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି STYLE:
		w = data->list[STYLE];
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି SIZE:
		w = data->list[SIZE];
		ଭାଙ୍ଗ;
	    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
		w = (Widget)0;	// for lint
	}

	ପାଇଁ (i = 0; i < ଗଣନା_2[idx]; ++i)
	{
	    ବସ୍ତୁଗୁଡ଼ିକ[i] = XmStringCreateLocalized(list[idx][i]);
	    XtFree(list[idx][i]);
	}
	Xm_List_Delete_All_ବସ୍ତୁଗୁଡ଼ିକ(w);
	Xm_List_Add_ବସ୍ତୁଗୁଡ଼ିକ(w, ବସ୍ତୁଗୁଡ଼ିକ, ଗଣନା_2[idx], 1);
	ଯଦି (data->sel[idx])
	{
	    XmStringFree(ବସ୍ତୁଗୁଡ଼ିକ[0]);
	    ବସ୍ତୁଗୁଡ଼ିକ[0] = XmStringCreateLocalized(data->sel[idx]);
	    XmListSelectItem(w, ବସ୍ତୁଗୁଡ଼ିକ[0], False);
	    XmListSetBottomItem(w, ବସ୍ତୁଗୁଡ଼ିକ[0]);
	}
	ପାଇଁ (i = 0; i < ଗଣନା_2[idx]; ++i)
	    XmStringFree(ବସ୍ତୁଗୁଡ଼ିକ[i]);
    }
}

    ସ୍ଥିର ଶୂନ୍ୟ
stoggle_callback(Widget w UNUSED,
	SharedFontSelData *data,
	XmToggleButtonCallbackStruct *call_data)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i, do_sel;
    ଅକ୍ଷର	newSize[TEMP_BUF_SIZE];
    XmString	str;

    ଯଦି (call_data->reason != (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)Xm_CR_ମୂଲ୍ୟ_CHANGED)
	ଫେରନ୍ତୁ;

    do_sel = (data->sel[SIZE] != ଖାଲି) && strcmp(data->sel[SIZE], wild);

    ପାଇଁ (i = 0; do_sel && (i < data->ଅଙ୍କ); i++)
	ଯଦି (match(data, ENCODING, i)
		&& match(data, NAME, i)
		&& match(data, STYLE, i)
		&& match(data, SIZE, i))
	{
	    size_part(fn(data, i), newSize, !data->in_pixels);
	    ଭାଙ୍ଗ;
	}

    data->in_pixels = !data->in_pixels;

    ଯଦି (data->sel[SIZE])
	XtFree(data->sel[SIZE]);
    data->sel[SIZE] = ଖାଲି;
    fill_lists(NONE, data);

    ଯଦି (do_sel)
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
    ସ୍ଥିର ଶୂନ୍ୟ
display_sample(SharedFontSelData *data)
{
    Arg		    args[2];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    n;
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

    Xt_Set_ମୂଲ୍ୟଗୁଡ଼ିକ(data->sample, args, n);
    XmStringFree(str);

    ଯଦି (data->old)
    {
	XFreeFont(display, data->old);
	XmFontListFree(data->old_list);
    }
    data->old = font;
    data->old_list = font_list;
}


    ସ୍ଥିର Boolean
do_choice(Widget w,
	SharedFontSelData *data,
	XmListCallbackStruct *call_data,
	ଗଣନା ListSpecifier which)
{
    ଅକ୍ଷର *sel;

    XmStringGetLtoR(call_data->item, XmSTRING_DEFAULT_CHARSET, &sel);

    ଯଦି (!data->sel[which])
	data->sel[which] = XtNewString(sel);
    ଅନ୍ୟଥା
    {
	ଯଦି (!strcmp(data->sel[which], sel))
	{
	    // unselecting current selection
	    XtFree(data->sel[which]);
	    data->sel[which] = ଖାଲି;
	    ଯଦି (w)
		XmListDeselectItem(w, call_data->item);
	}
	ଅନ୍ୟଥା
	{
	    XtFree(data->sel[which]);
	    data->sel[which] = XtNewString(sel);
	}
    }
    XtFree(sel);

    fill_lists(which, data);

    // If there is a font selection, we display it.
    ଯଦି (data->sel[ENCODING]
	    && data->sel[NAME]
	    && data->sel[STYLE]
	    && data->sel[SIZE]
	    && strcmp(data->sel[ENCODING], wild)
	    && strcmp(data->sel[NAME], wild)
	    && strcmp(data->sel[STYLE], wild)
	    && strcmp(data->sel[SIZE], wild))
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;

	ଯଦି (data->font_name)
	    XtFree(data->font_name);
	data->font_name = ଖାଲି;

	ପାଇଁ (i = 0; i < data->ଅଙ୍କ; i++)
	{
	    ଯଦି (match(data, ENCODING, i)
		    && match(data, NAME, i)
		    && match(data, STYLE, i)
		    && match(data, SIZE, i))
	    {
		data->font_name = XtNewString(fn(data, i));
		ଭାଙ୍ଗ;
	    }
	}

	ଯଦି (data->font_name)
	{
	    XmTextSetString(data->name, data->font_name);
	    display_sample(data);
	}
	ଅନ୍ୟଥା
	    do_dialog(VIM_ERROR,
		    (char_u *)_("Error"),
		    (char_u *)_("Invalid font specification"),
		    (char_u *)_("&Dismiss"), 1, ଖାଲି, FALSE);

	ଫେରନ୍ତୁ True;
    }
    ଅନ୍ୟଥା
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    n;
	XmString    str;
	Arg	    args[4];
	ଅକ୍ଷର	    *nomatch_ସନ୍ଦେଶ = _("no specific match");

	n = 0;
	str = XmStringCreateLocalized(nomatch_ସନ୍ଦେଶ);
	XtSetArg(args[n], XmNlabelString, str); ++n;
	Xt_Set_ମୂଲ୍ୟଗୁଡ଼ିକ(data->sample, args, n);
	apply_fontlist(data->sample);
	XmTextSetString(data->name, nomatch_ସନ୍ଦେଶ);
	XmStringFree(str);

	ଫେରନ୍ତୁ False;
    }
}

    ସ୍ଥିର ଶୂନ୍ୟ
encoding_callback(Widget w,
	SharedFontSelData *data,
	XtPointer dummy UNUSED)
{
    XmString str;
    XmListCallbackStruct fake_data;

    Xt_Va_Get_ମୂଲ୍ୟଗୁଡ଼ିକ(w, XmNlabelString, &str, ଖାଲି);

    ଯଦି (!str)
	ଫେରନ୍ତୁ;

    fake_data.item = str;

    do_choice(0, data, &fake_data, ENCODING);
}

    ସ୍ଥିର ଶୂନ୍ୟ
name_callback(Widget w,
	SharedFontSelData *data,
	XmListCallbackStruct *call_data)
{
    do_choice(w, data, call_data, NAME);
}

    ସ୍ଥିର ଶୂନ୍ୟ
style_callback(Widget w,
	SharedFontSelData *data,
	XmListCallbackStruct *call_data)
{
    do_choice(w, data, call_data, STYLE);
}

    ସ୍ଥିର ଶୂନ୍ୟ
size_callback(Widget w,
	SharedFontSelData *data,
	XmListCallbackStruct *call_data)
{
    do_choice(w, data, call_data, SIZE);
}

    ସ୍ଥିର ଶୂନ୍ୟ
cancel_callback(Widget w UNUSED,
	SharedFontSelData *data,
	XmListCallbackStruct *call_data UNUSED)
{
    ଯଦି (data->sel[ENCODING])
    {
	XtFree(data->sel[ENCODING]);
	data->sel[ENCODING] = ଖାଲି;
    }
    ଯଦି (data->sel[NAME])
    {
	XtFree(data->sel[NAME]);
	data->sel[NAME] = ଖାଲି;
    }
    ଯଦି (data->sel[STYLE])
    {
	XtFree(data->sel[STYLE]);
	data->sel[STYLE] = ଖାଲି;
    }
    ଯଦି (data->sel[SIZE])
    {
	XtFree(data->sel[SIZE]);
	data->sel[SIZE] = ଖାଲି;
    }

    ଯଦି (data->font_name)
	XtFree(data->font_name);
    data->font_name = ଖାଲି;

    data->ଅଙ୍କ = 0;
    XFreeFontNames(data->names);
    data->names = ଖାଲି;
    data->exit = True;
}

    ସ୍ଥିର ଶୂନ୍ୟ
ok_callback(Widget w UNUSED,
	SharedFontSelData *data,
	Xm_ଠେଲ_Button_Callback_Struct *call_data UNUSED)
{
    ଅକ୍ଷର    *pattern;
    ଅକ୍ଷର    **name;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    i;

    pattern = XmTextGetString(data->name);
    name    = XListFonts(XtDisplay(data->dialog), pattern, 1, &i);
    XtFree(pattern);

    ଯଦି (i != 1)
    {
	do_dialog(VIM_ERROR,
		(char_u *)_("Error"),
		(char_u *)_("Invalid font specification"),
		(char_u *)_("&Dismiss"), 1, ଖାଲି, FALSE);
	XFreeFontNames(name);
    }
    ଅନ୍ୟଥା
    {
	ଯଦି (data->font_name)
	    XtFree(data->font_name);
	data->font_name = XtNewString(name[0]);

	ଯଦି (data->sel[ENCODING])
	{
	    XtFree(data->sel[ENCODING]);
	    data->sel[ENCODING] = ଖାଲି;
	}
	ଯଦି (data->sel[NAME])
	{
	    XtFree(data->sel[NAME]);
	    data->sel[NAME] = ଖାଲି;
	}
	ଯଦି (data->sel[STYLE])
	{
	    XtFree(data->sel[STYLE]);
	    data->sel[STYLE] = ଖାଲି;
	}
	ଯଦି (data->sel[SIZE])
	{
	    XtFree(data->sel[SIZE]);
	    data->sel[SIZE] = ଖାଲି;
	}

	XFreeFontNames(name);

	data->ଅଙ୍କ = 0;
	XFreeFontNames(data->names);
	data->names = ଖାଲି;
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
    ସ୍ଥିର SharedFontSelData	_data;

    Widget		parent;
    Widget		form;
    Widget		separator;
    Widget		sub_form;
    Widget		size_toggle;
    Widget		name;
    Widget		disp_frame;
    Widget		frame;
    Arg			args[64];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			n;
    XmString		str;
    ଅକ୍ଷର		big_font[MAX_FONT_NAME_LEN];
    SharedFontSelData	*data;

    data = &_data;

    parent = vimShell;
    data->names = XListFonts(XtDisplay(parent), "-*-*-*-*-*-*-*-*-*-*-*-*-*-*",
						       MAX_FONTS, &data->ଅଙ୍କ);

    /*
     * Find the name of the biggest font less than the given limit
     * MAX_DISPLAY_SIZE used to set up the initial height of the display
     * widget.
     */

    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	i;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	max;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	idx = 0;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	size;
	ଅକ୍ଷର	buf[128];

	ପାଇଁ (i = 0, max = 0; i < data->ଅଙ୍କ; i++)
	{
	    get_part(fn(data, i), 7, buf);
	    size = atoi(buf);
	    ଯଦି ((size > max) && (size < MAX_DISPLAY_SIZE))
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
    data->dialog = XmCreateDialogShell(parent, "fontSelector", ଖାଲି, 0);
    str = XmStringCreateLocalized(_("Vim - Font Selector"));

    // Create form popup dialog widget.
    form = XtVaCreateWidget("form",
	    xmFormWidgetClass, data->dialog,
	    XmNdialogTitle, str,
	    XmNautoUnmanage, False,
	    XmNdialogStyle, XmDIALOG_FULL_APPLICATION_MODAL,
	    ଖାଲି);
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
	    ଖାଲି);

    data->ok = XtVaCreateManagedWidget(_("OK"),
	    xm_ଠେଲ_Button_Gadget_Class, sub_form,
	    XmNleftAttachment, XmATTACH_FORM,
	    XmNrightAttachment, XmATTACH_FORM,
	    XmNtopAttachment, XmATTACH_FORM,
	    XmNtopOffset, 4,
	    ଖାଲି);
    apply_fontlist(data->ok);

    data->cancel = XtVaCreateManagedWidget(_("Cancel"),
	    xm_ଠେଲ_Button_Gadget_Class, sub_form,
	    XmNrightAttachment, XmATTACH_FORM,
	    XmNleftAttachment, XmATTACH_FORM,
	    XmNtopAttachment, XmATTACH_WIDGET,
	    XmNtopWidget, data->ok,
	    XmNtopOffset, 4,
	    XmNshowAsDefault, True,
	    ଖାଲି);
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
	    XmNeditMode, Xm_SINGLE_ଧାଡ଼ି_EDIT,
	    XmNmaxLength, MAX_FONT_NAME_LEN,
	    XmNcolumns, 60,
	    ଖାଲି);

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
	    ଖାଲି);
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
	    ଖାଲି);

    data->sample = XtVaCreateManagedWidget("sampleLabel",
	    xmLabelWidgetClass, disp_frame,
	    XmNleftAttachment, XmATTACH_FORM,
	    XmNtopAttachment, XmATTACH_FORM,
	    XmNbottomAttachment, XmATTACH_FORM,
	    XmNrightAttachment, XmATTACH_FORM,
	    XmNalignment, XmALIGNMENT_BEGINNING,
	    XmNrecomputeSize, False,
	    XmNfontList, data->old_list,
	    ଖାଲି);

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
	    ଖାଲି);
    XmStringFree(str);
    apply_fontlist(size_toggle);
    XtManageChild(size_toggle);

    // Encoding pulldown menu.

    data->encoding_pulldown = XmCreatePulldownMenu(form,
						 "encodingPulldown", ଖାଲି, 0);
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
	    ଖାଲି);

    // font list
    frame = XtVaCreateManagedWidget("frame", xmFrameWidgetClass, sub_form,
	    XmNshadowThickness, 0,
	    XmNtopAttachment, XmATTACH_FORM,
	    XmNbottomAttachment, XmATTACH_FORM,
	    XmNleftAttachment, XmATTACH_FORM,
	    XmNrightAttachment, XmATTACH_POSITION,
	    XmNrightPosition, 50,
	    ଖାଲି);

    str = XmStringCreateLocalized(_("Font:"));
    name = XtVaCreateManagedWidget("nameListLabel", xmLabelGadgetClass, frame,
	    XmNchildType, XmFRAME_TITLE_CHILD,
	    XmNchildVerticalAlignment, XmALIGNMENT_CENTER,
	    XmNchildHorizontalAlignment, XmALIGNMENT_BEGINNING,
	    XmNlabelString, str,
	    ଖାଲି);
    XmStringFree(str);
    apply_fontlist(name);

    n = 0;
    XtSetArg(args[n], Xm_Nvisible_Item_ଗଣନା, 8); ++n;
    XtSetArg(args[n], XmNresizable, True); ++n;
    XtSetArg(args[n], XmNlistSizePolicy, XmCONSTANT); ++n;
    XtSetArg(args[n], XmNvisualPolicy, XmVARIABLE); ++n;
#ifdef LESSTIF_VERSION
    XtSetArg(args[n], XmNscrollBarDisplayPolicy, XmSTATIC); ++n;
#endif
    data->list[NAME] = XmCreateScrolledList(frame, "fontList", args, n);
    Xt_Va_Set_ମୂଲ୍ୟଗୁଡ଼ିକ(name, XmNuserData, data->list[NAME], ଖାଲି);

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
	    ଖାଲି);

    str = XmStringCreateLocalized(_("Style:"));
    name = XtVaCreateManagedWidget("styleListLabel", xmLabelWidgetClass, frame,
	    XmNchildType, XmFRAME_TITLE_CHILD,
	    XmNchildVerticalAlignment, XmALIGNMENT_CENTER,
	    XmNchildHorizontalAlignment, XmALIGNMENT_BEGINNING,
	    XmNlabelString, str,
	    ଖାଲି);
    XmStringFree(str);
    apply_fontlist(name);

    n = 0;
    XtSetArg(args[n], Xm_Nvisible_Item_ଗଣନା, 8); ++n;
    XtSetArg(args[n], XmNresizable, True); ++n;
    XtSetArg(args[n], XmNlistSizePolicy, XmCONSTANT); ++n;
    XtSetArg(args[n], XmNvisualPolicy, XmVARIABLE); ++n;
#ifdef LESSTIF_VERSION
    XtSetArg(args[n], XmNscrollBarDisplayPolicy, XmSTATIC); ++n;
#endif
    data->list[STYLE] = XmCreateScrolledList(frame, "styleList", args, n);
    Xt_Va_Set_ମୂଲ୍ୟଗୁଡ଼ିକ(name, XmNuserData, data->list[STYLE], ଖାଲି);

    // size list
    frame = XtVaCreateManagedWidget("frame", xmFrameWidgetClass, sub_form,
	    XmNshadowThickness, 0,
	    XmNtopAttachment, XmATTACH_FORM,
	    XmNbottomAttachment, XmATTACH_FORM,
	    XmNleftAttachment, XmATTACH_POSITION,
	    XmNleftPosition, 80,
	    XmNleftOffset, 4,
	    XmNrightAttachment, XmATTACH_FORM,
	    ଖାଲି);

    str = XmStringCreateLocalized(_("Size:"));
    name = XtVaCreateManagedWidget("sizeListLabel", xmLabelGadgetClass, frame,
	    XmNchildType, XmFRAME_TITLE_CHILD,
	    XmNchildVerticalAlignment, XmALIGNMENT_CENTER,
	    XmNchildHorizontalAlignment, XmALIGNMENT_BEGINNING,
	    XmNlabelString, str,
	    ଖାଲି);
    XmStringFree(str);
    apply_fontlist(name);

    n = 0;
    XtSetArg(args[n], Xm_Nvisible_Item_ଗଣନା, 8); ++n;
    XtSetArg(args[n], XmNresizable, True); ++n;
    XtSetArg(args[n], XmNlistSizePolicy, XmCONSTANT); ++n;
    XtSetArg(args[n], XmNvisualPolicy, XmVARIABLE); ++n;
#ifdef LESSTIF_VERSION
    XtSetArg(args[n], XmNscrollBarDisplayPolicy, XmSTATIC); ++n;
#endif
    data->list[SIZE] = XmCreateScrolledList(frame, "sizeList", args, n);
    Xt_Va_Set_ମୂଲ୍ୟଗୁଡ଼ିକ(name, XmNuserData, data->list[SIZE], ଖାଲି);

    // update form widgets cancel button
    Xt_Va_Set_ମୂଲ୍ୟଗୁଡ଼ିକ(form, XmNcancelButton, data->cancel, ଖାଲି);

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
    data->sel[ENCODING] = ଖାଲି;
    data->sel[NAME] = ଖାଲି;
    data->sel[STYLE] = ଖାଲି;
    data->sel[SIZE] = ଖାଲି;
    data->font_name = ଖାଲି;

    // set up current font parameters
    ଯଦି (current && current[0] != '\0')
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    i;
	ଅକ୍ଷର	    **names;

	names = XListFonts(XtDisplay(form), (ଅକ୍ଷର *) current, 1, &i);

	ଯଦି (i != 0)
	{
	    ଅକ୍ଷର namebuf[TEMP_BUF_SIZE];
	    ଅକ୍ଷର stylebuf[TEMP_BUF_SIZE];
	    ଅକ୍ଷର sizebuf[TEMP_BUF_SIZE];
	    ଅକ୍ଷର encodingbuf[TEMP_BUF_SIZE];
	    ଅକ୍ଷର *found;

	    found = names[0];

	    name_part(found, namebuf);
	    style_part(found, stylebuf);
	    size_part(found, sizebuf, data->in_pixels);
	    encoding_part(found, encodingbuf);

	    ଯଦି (*namebuf != NUL
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
	    ଅନ୍ୟଥା
	    {
		// We can't preset a symbolic name, which isn't a full font
		// description. Therefore we just behave the same way as if the
		// user didn't have selected anything thus far.
		//
		// Unfortunately there is no known way to expand an abbreviated
		// font name.
		data->font_name = ଖାଲି;
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
    ଯେତେବେଳେ (!data->exit)
	XtAppProcessEvent(XtWidgetToApplicationContext(data->dialog),
							(Xt_ନିବେଶ_Mask)XtIMAll);

    ଯଦି (data->old)
    {
	XFreeFont(XtDisplay(data->dialog),  data->old);
	XmFontListFree(data->old_list);
    }
    XtDestroyWidget(data->dialog);

    gui_motif_synch_fonts();

    ଫେରନ୍ତୁ (char_u *) data->font_name;
}
