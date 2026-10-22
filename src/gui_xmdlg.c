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

বাহ্যিক Widget vimShell;

#ifdef FEAT_MENU
# define apply_fontlist(w) gui_motif_menu_fontlist(w)
#অন্যথা
# define apply_fontlist(w)
#endif

/////////////////////////////////////////////////////////////////////////////
// Font selection dialogue implementation.

স্থির অক্ষর wild[3] = "*";

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
    Xt_Va_Set_মানসমূহ(shell, XmNdeleteResponse, XmDO_NOTHING, ফাঁকা);

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
#define MAX_DISPLAY_আকার		150
#define অস্থায়ী_BUF_আকার			256

গণনা ListSpecifier
{
    ENCODING,
    NAME,
    STYLE,
    আকার_4,
    NONE
};

প্রকারলেখ কাঠামো Shared_Font_Sel_তথ্য
{
    Widget	dialog;
    Widget	ok;
    Widget	cancel;
    Widget	encoding_pulldown;
    Widget	encoding_menu;
    Widget	list[NONE];
    Widget	name;
    Widget	sample;
    অক্ষর	**names;	// font name array of arrays
    পূর্ণসংখ্যা		অঙ্ক;		// number of font names
    String	sel[NONE];	// selection category
    Boolean	in_pixels;	// toggle state - size in pixels
    অক্ষর	*font_name;	// current font name
    XFontStruct	*old;		// font data structure for sample display
    XmFontList	old_list;	// font data structure for sample display
    Boolean	exit;		// used for program exit control
} Shared_Font_Sel_তথ্য_2;

/*
 * Checking access to the font name array for validity.
 */
    স্থির অক্ষর *
fn(Shared_Font_Sel_তথ্য_2 *তথ্য, পূর্ণসংখ্যা i)
{
    // Assertion checks:
    যদি (তথ্য->অঙ্ক < 0)
	abort();
    যদি (i >= তথ্য->অঙ্ক)
	i = তথ্য->অঙ্ক - 1;
    যদি (i < 0)
	i = 0;

    ফেরত তথ্য->names[i];
}

/*
 * Get a specific substring from a font name.
 */
    স্থির শূন্য
get_part(অক্ষর *in, পূর্ণসংখ্যা pos, অক্ষর *out)
{
    পূর্ণসংখ্যা	i;
    পূর্ণসংখ্যা j;

    *out = '\0';

    জন্য (i = 0; (pos > 0) && (in[i] != '\0'); ++i)
	যদি (in[i] == '-')
	    pos--;

    যদি (in[i] == '\0')
	ফেরত;

    জন্য (j = 0; (in[i] != '-') && (in[i] != '\0'); ++i, ++j)
	out[j] = in[i];
    out[j] = '\0';
}

/*
 * Given a font name this function returns the part used in the first
 * scroll list.
 */
    স্থির শূন্য
name_part(অক্ষর *font, অক্ষর *buf)
{
    অক্ষর    buf2[অস্থায়ী_BUF_আকার];
    অক্ষর    buf3[অস্থায়ী_BUF_আকার];

    get_part(font, 2, buf2);
    get_part(font, 1, buf3);

    যদি (*buf3 != NUL)
	vim_snprintf(buf, অস্থায়ী_BUF_আকার, "%s (%s)", buf2, buf3);
    অন্যথা
	vim_snprintf(buf, অস্থায়ী_BUF_আকার, "%s", buf2);
}

/*
 * Given a font name this function returns the part used in the second scroll list.
 */
    স্থির শূন্য
style_part(অক্ষর *font, অক্ষর *buf)
{
    অক্ষর    buf2[অস্থায়ী_BUF_আকার];
    অক্ষর    buf3[অস্থায়ী_BUF_আকার];

    get_part(font, 3, buf3);
    get_part(font, 5, buf2);

    যদি (!strcmp(buf2, "normal") && !strcmp(buf2, "Normal")
						   && !strcmp(buf2, "NORMAL"))
	vim_snprintf(buf, অস্থায়ী_BUF_আকার, "%s %s", buf3, buf2);
    অন্যথা
	strcpy(buf, buf3);

    get_part(font, 6, buf2);

    যদি (buf2[0] != '\0')
	vim_snprintf(buf3, অস্থায়ী_BUF_আকার, "%s %s", buf, buf2);
    অন্যথা
	strcpy(buf3, buf);

    get_part(font, 4, buf2);

    যদি (!strcmp(buf2, "o") || !strcmp(buf2, "O"))
	vim_snprintf(buf, অস্থায়ী_BUF_আকার, "%s oblique", buf3);
    অন্যথা যদি (!strcmp(buf2, "i") || !strcmp(buf2, "I"))
	vim_snprintf(buf, অস্থায়ী_BUF_আকার, "%s italic", buf3);

    যদি (!strcmp(buf, " "))
	strcpy(buf, "-");
}

/*
 * Given a font name this function returns the part used in the third
 * scroll list.
 */
    স্থির শূন্য
আকার_part(অক্ষর *font, অক্ষর *buf, পূর্ণসংখ্যা inPixels)
{
    পূর্ণসংখ্যা	    আকার_2;
    দশমিক   অস্থায়ী;

    *buf = '\0';

    যদি (inPixels)
    {
	get_part(font, 7, buf);
	যদি (*buf != NUL)
	{
	    আকার_2 = atoi(buf);
	    sprintf(buf, "%3d", আকার_2);
	}
    }
    অন্যথা
    {
	get_part(font, 8, buf);
	যদি (*buf != NUL)
	{
	    আকার_2 = atoi(buf);
	    অস্থায়ী = (দশমিক)আকার_2 / 10.0;
	    আকার_2 = অস্থায়ী;
	    যদি (buf[strlen(buf) - 1] == '0')
		sprintf(buf, "%3d", আকার_2);
	    অন্যথা
		sprintf(buf, "%4.1f", অস্থায়ী);
	}
    }
}

/*
 * Given a font name this function returns the part used in the choice menu.
 */
    স্থির শূন্য
encoding_part(অক্ষর *font, অক্ষর *buf)
{
    অক্ষর    buf1[অস্থায়ী_BUF_আকার];
    অক্ষর    buf2[অস্থায়ী_BUF_আকার];

    *buf = '\0';

    get_part(font, 13, buf1);
    get_part(font, 14, buf2);

    যদি (*buf1 != NUL && *buf2 != NUL)
	vim_snprintf(buf, অস্থায়ী_BUF_আকার, "%s-%s", buf1, buf2);
    যদি (!strcmp(buf, " "))
	strcpy(buf, "-");
}

/*
 * Inserts a string into correct sorted position in a list.
 */
    স্থির শূন্য
add_to_list(অক্ষর **buf, অক্ষর *item, পূর্ণসংখ্যা *গুনতি)
{
    পূর্ণসংখ্যা	i;
    পূর্ণসংখ্যা j;

    যদি (*গুনতি == MAX_ENTRIES_IN_LIST)
	ফেরত;

    // avoid duplication
    জন্য (i = 0; i < *গুনতি; ++i)
    {
	যদি (!strcmp(buf[i], item))
	    ফেরত;
    }

    // find order place, but make sure that wild card comes first
    যদি (!strcmp(item, wild))
	i = 0;
    অন্যথা
	জন্য (i = 0; i < *গুনতি; ++i)
	    যদি (strcmp(buf[i], item) > 0 && strcmp(buf[i], wild))
		বিরতি;

    // now insert the item
    জন্য (j = *গুনতি; j > i; --j)
	buf[j] = buf[j-1];
    buf[i] = XtNewString(item);

    ++(*গুনতি);
}

/*
 * True if the font matches some field.
 */
    স্থির Boolean
match(Shared_Font_Sel_তথ্য_2 *তথ্য, গণনা ListSpecifier l, পূর্ণসংখ্যা i)
{
    অক্ষর buf[অস্থায়ী_BUF_আকার];

    // An empty selection or a wild card matches anything.
    যদি (!তথ্য->sel[l] || !strcmp(তথ্য->sel[l], wild))
	ফেরত True;

    // chunk out the desired part...
    নির্বাচন (l)
    {
	ক্ষেত্র ENCODING:
	    encoding_part(fn(তথ্য, i), buf);
	    বিরতি;

	ক্ষেত্র NAME:
	    name_part(fn(তথ্য, i), buf);
	    বিরতি;

	ক্ষেত্র STYLE:
	    style_part(fn(তথ্য, i), buf);
	    বিরতি;

	ক্ষেত্র আকার_4:
	    আকার_part(fn(তথ্য, i), buf, তথ্য->in_pixels);
	    বিরতি;
	পূর্বনির্ধারিত:
	    ;
    }

    // ...and chew it now

    ফেরত !strcmp(buf, তথ্য->sel[l]);
}

    স্থির Boolean
proportional(অক্ষর *font)
{
    অক্ষর buf[অস্থায়ী_BUF_আকার];

    get_part(font, 11, buf);

    ফেরত !strcmp(buf, "p") || !strcmp(buf, "P");
}


স্থির শূন্য encoding_callback(Widget w, Shared_Font_Sel_তথ্য_2 *তথ্য,
							     XtPointer dummy);

/*
 * Parse through the fontlist data and set up the three scroll lists.  The fix
 * parameter can be used to exclude a list from any changes.  This is used for
 * updates after selections caused by the users actions.
 */
    স্থির শূন্য
fill_lists(গণনা ListSpecifier fix, Shared_Font_Sel_তথ্য_2 *তথ্য)
{
    অক্ষর	*list[NONE][MAX_ENTRIES_IN_LIST];
    পূর্ণসংখ্যা		গুনতি[NONE];
    অক্ষর	buf[অস্থায়ী_BUF_আকার];
    XmString	বস্তুসমূহ[MAX_ENTRIES_IN_LIST];
    পূর্ণসংখ্যা		i;
    পূর্ণসংখ্যা		idx;

    জন্য (idx = (পূর্ণসংখ্যা)ENCODING; idx < (পূর্ণসংখ্যা)NONE; ++idx)
	গুনতি[idx] = 0;

    // First we insert the wild char into every single list.
    যদি (fix != ENCODING)
	add_to_list(list[ENCODING], wild, &গুনতি[ENCODING]);
    যদি (fix != NAME)
	add_to_list(list[NAME], wild, &গুনতি[NAME]);
    যদি (fix != STYLE)
	add_to_list(list[STYLE], wild, &গুনতি[STYLE]);
    যদি (fix != আকার_4)
	add_to_list(list[আকার_4], wild, &গুনতি[আকার_4]);

    জন্য (i = 0; i < তথ্য->অঙ্ক && i < MAX_ENTRIES_IN_LIST; i++)
    {
	যদি (proportional(fn(তথ্য, i)))
	    চালু;

	যদি (fix != ENCODING
		&& match(তথ্য, NAME, i)
		&& match(তথ্য, STYLE, i)
		&& match(তথ্য, আকার_4, i))
	{
	    encoding_part(fn(তথ্য, i), buf);
	    add_to_list(list[ENCODING], buf, &গুনতি[ENCODING]);
	}

	যদি (fix != NAME
		&& match(তথ্য, ENCODING, i)
		&& match(তথ্য, STYLE, i)
		&& match(তথ্য, আকার_4, i))
	{
	    name_part(fn(তথ্য, i), buf);
	    add_to_list(list[NAME], buf, &গুনতি[NAME]);
	}

	যদি (fix != STYLE
		&& match(তথ্য, ENCODING, i)
		&& match(তথ্য, NAME, i)
		&& match(তথ্য, আকার_4, i))
	{
	    style_part(fn(তথ্য, i), buf);
	    add_to_list(list[STYLE], buf, &গুনতি[STYLE]);
	}

	যদি (fix != আকার_4
		&& match(তথ্য, ENCODING, i)
		&& match(তথ্য, NAME, i)
		&& match(তথ্য, STYLE, i))
	{
	    আকার_part(fn(তথ্য, i), buf, তথ্য->in_pixels);
	    add_to_list(list[আকার_4], buf, &গুনতি[আকার_4]);
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
	Xt_Va_Get_মানসমূহ(তথ্য->encoding_pulldown,
		XmNchildren, &children,
		XmNnumChildren, &n_বস্তুসমূহ,
		ফাঁকা);

	জন্য (i = 0; i < গুনতি[ENCODING]; ++i)
	{
	    Widget button;

	    বস্তুসমূহ[i] = XmStringCreateLocalized(list[ENCODING][i]);

	    যদি (i < (পূর্ণসংখ্যা)n_বস্তুসমূহ)
	    {
		// recycle old button
		Xt_Va_Set_মানসমূহ(children[i],
			XmNlabelString, বস্তুসমূহ[i],
			Xm_Nuser_তথ্য, i,
			ফাঁকা);
		button = children[i];
	    }
	    অন্যথা
	    {
		// create a new button
		button = XtVaCreateManagedWidget("button",
			xm_ঠেলো_Button_Gadget_Class,
			তথ্য->encoding_pulldown,
			XmNlabelString, বস্তুসমূহ[i],
			Xm_Nuser_তথ্য, i,
			ফাঁকা);
		XtAddCallback(button, XmNactivateCallback,
			(XtCallbackProc) encoding_callback, (XtPointer) তথ্য);
		XtManageChild(button);
	    }

	    যদি (তথ্য->sel[ENCODING])
	    {
		যদি (!strcmp(তথ্য->sel[ENCODING], list[ENCODING][i]))
		    selected_button = button;
	    }
	    XtFree(list[ENCODING][i]);
	}

	// Destroy all the outstanding menu items.
	জন্য (i = গুনতি[ENCODING]; i < (পূর্ণসংখ্যা)n_বস্তুসমূহ; ++i)
	{
	    XtUnmanageChild(children[i]);
	    XtDestroyWidget(children[i]);
	}

	// Preserve the current selection visually.
	যদি (selected_button)
	{
	    Xt_Va_Set_মানসমূহ(তথ্য->encoding_menu,
		    XmNmenuHistory, selected_button,
		    ফাঁকা);
	}

	জন্য (i = 0; i < গুনতি[ENCODING]; ++i)
	    XmStringFree(বস্তুসমূহ[i]);
    }

    /*
     * Now loop through the remaining lists and set them up.
     */
    জন্য (idx = (পূর্ণসংখ্যা)NAME; idx < (পূর্ণসংখ্যা)NONE; ++idx)
    {
	Widget w;

	যদি (fix == (গণনা ListSpecifier)idx)
	    চালু;

	নির্বাচন ((গণনা ListSpecifier)idx)
	{
	    ক্ষেত্র NAME:
		w = তথ্য->list[NAME];
		বিরতি;
	    ক্ষেত্র STYLE:
		w = তথ্য->list[STYLE];
		বিরতি;
	    ক্ষেত্র আকার_4:
		w = তথ্য->list[আকার_4];
		বিরতি;
	    পূর্বনির্ধারিত:
		w = (Widget)0;	// for lint
	}

	জন্য (i = 0; i < গুনতি[idx]; ++i)
	{
	    বস্তুসমূহ[i] = XmStringCreateLocalized(list[idx][i]);
	    XtFree(list[idx][i]);
	}
	Xm_List_Delete_All_বস্তুসমূহ(w);
	Xm_List_Add_বস্তুসমূহ(w, বস্তুসমূহ, গুনতি[idx], 1);
	যদি (তথ্য->sel[idx])
	{
	    XmStringFree(বস্তুসমূহ[0]);
	    বস্তুসমূহ[0] = XmStringCreateLocalized(তথ্য->sel[idx]);
	    XmListSelectItem(w, বস্তুসমূহ[0], False);
	    XmListSetBottomItem(w, বস্তুসমূহ[0]);
	}
	জন্য (i = 0; i < গুনতি[idx]; ++i)
	    XmStringFree(বস্তুসমূহ[i]);
    }
}

    স্থির শূন্য
stoggle_callback(Widget w UNUSED,
	Shared_Font_Sel_তথ্য_2 *তথ্য,
	XmToggleButtonCallbackStruct *call_তথ্য)
{
    পূর্ণসংখ্যা		i, do_sel;
    অক্ষর	new_আকার_2[অস্থায়ী_BUF_আকার];
    XmString	str;

    যদি (call_তথ্য->reason != (পূর্ণসংখ্যা)Xm_CR_মান_CHANGED)
	ফেরত;

    do_sel = (তথ্য->sel[আকার_4] != ফাঁকা) && strcmp(তথ্য->sel[আকার_4], wild);

    জন্য (i = 0; do_sel && (i < তথ্য->অঙ্ক); i++)
	যদি (match(তথ্য, ENCODING, i)
		&& match(তথ্য, NAME, i)
		&& match(তথ্য, STYLE, i)
		&& match(তথ্য, আকার_4, i))
	{
	    আকার_part(fn(তথ্য, i), new_আকার_2, !তথ্য->in_pixels);
	    বিরতি;
	}

    তথ্য->in_pixels = !তথ্য->in_pixels;

    যদি (তথ্য->sel[আকার_4])
	XtFree(তথ্য->sel[আকার_4]);
    তথ্য->sel[আকার_4] = ফাঁকা;
    fill_lists(NONE, তথ্য);

    যদি (do_sel)
    {
	str = XmStringCreateLocalized(new_আকার_2);
	XmListSelectItem(তথ্য->list[আকার_4], str, True);
	XmListSetBottomItem(তথ্য->list[আকার_4], str);
	XmStringFree(str);
    }
}

/*
 * Show the currently selected font in the sample text label.
 */
    স্থির শূন্য
display_sample(Shared_Font_Sel_তথ্য_2 *তথ্য)
{
    Arg		    args[2];
    পূর্ণসংখ্যা		    n;
    XFontStruct	*   font;
    XmFontList	    font_list;
    Display *	    display;
    XmString	    str;

    display = XtDisplay(তথ্য->dialog);
    font = XLoadQueryFont(display, তথ্য->font_name);
    font_list = gui_motif_create_fontlist(font);

    n = 0;
    str = XmStringCreateLocalized("AaBbZzYy 0123456789");
    XtSetArg(args[n], XmNlabelString, str); n++;
    XtSetArg(args[n], XmNfontList, font_list); n++;

    Xt_Set_মানসমূহ(তথ্য->sample, args, n);
    XmStringFree(str);

    যদি (তথ্য->old)
    {
	XFreeFont(display, তথ্য->old);
	XmFontListFree(তথ্য->old_list);
    }
    তথ্য->old = font;
    তথ্য->old_list = font_list;
}


    স্থির Boolean
do_choice(Widget w,
	Shared_Font_Sel_তথ্য_2 *তথ্য,
	XmListCallbackStruct *call_তথ্য,
	গণনা ListSpecifier which)
{
    অক্ষর *sel;

    XmStringGetLtoR(call_তথ্য->item, XmSTRING_DEFAULT_CHARSET, &sel);

    যদি (!তথ্য->sel[which])
	তথ্য->sel[which] = XtNewString(sel);
    অন্যথা
    {
	যদি (!strcmp(তথ্য->sel[which], sel))
	{
	    // unselecting current selection
	    XtFree(তথ্য->sel[which]);
	    তথ্য->sel[which] = ফাঁকা;
	    যদি (w)
		XmListDeselectItem(w, call_তথ্য->item);
	}
	অন্যথা
	{
	    XtFree(তথ্য->sel[which]);
	    তথ্য->sel[which] = XtNewString(sel);
	}
    }
    XtFree(sel);

    fill_lists(which, তথ্য);

    // If there is a font selection, we display it.
    যদি (তথ্য->sel[ENCODING]
	    && তথ্য->sel[NAME]
	    && তথ্য->sel[STYLE]
	    && তথ্য->sel[আকার_4]
	    && strcmp(তথ্য->sel[ENCODING], wild)
	    && strcmp(তথ্য->sel[NAME], wild)
	    && strcmp(তথ্য->sel[STYLE], wild)
	    && strcmp(তথ্য->sel[আকার_4], wild))
    {
	পূর্ণসংখ্যা i;

	যদি (তথ্য->font_name)
	    XtFree(তথ্য->font_name);
	তথ্য->font_name = ফাঁকা;

	জন্য (i = 0; i < তথ্য->অঙ্ক; i++)
	{
	    যদি (match(তথ্য, ENCODING, i)
		    && match(তথ্য, NAME, i)
		    && match(তথ্য, STYLE, i)
		    && match(তথ্য, আকার_4, i))
	    {
		তথ্য->font_name = XtNewString(fn(তথ্য, i));
		বিরতি;
	    }
	}

	যদি (তথ্য->font_name)
	{
	    XmTextSetString(তথ্য->name, তথ্য->font_name);
	    display_sample(তথ্য);
	}
	অন্যথা
	    do_dialog(VIM_ERROR,
		    (char_u *)_("Error"),
		    (char_u *)_("Invalid font specification"),
		    (char_u *)_("&Dismiss"), 1, ফাঁকা, FALSE);

	ফেরত True;
    }
    অন্যথা
    {
	পূর্ণসংখ্যা	    n;
	XmString    str;
	Arg	    args[4];
	অক্ষর	    *nomatch_বার্তা = _("no specific match");

	n = 0;
	str = XmStringCreateLocalized(nomatch_বার্তা);
	XtSetArg(args[n], XmNlabelString, str); ++n;
	Xt_Set_মানসমূহ(তথ্য->sample, args, n);
	apply_fontlist(তথ্য->sample);
	XmTextSetString(তথ্য->name, nomatch_বার্তা);
	XmStringFree(str);

	ফেরত False;
    }
}

    স্থির শূন্য
encoding_callback(Widget w,
	Shared_Font_Sel_তথ্য_2 *তথ্য,
	XtPointer dummy UNUSED)
{
    XmString str;
    XmListCallbackStruct fake_তথ্য;

    Xt_Va_Get_মানসমূহ(w, XmNlabelString, &str, ফাঁকা);

    যদি (!str)
	ফেরত;

    fake_তথ্য.item = str;

    do_choice(0, তথ্য, &fake_তথ্য, ENCODING);
}

    স্থির শূন্য
name_callback(Widget w,
	Shared_Font_Sel_তথ্য_2 *তথ্য,
	XmListCallbackStruct *call_তথ্য)
{
    do_choice(w, তথ্য, call_তথ্য, NAME);
}

    স্থির শূন্য
style_callback(Widget w,
	Shared_Font_Sel_তথ্য_2 *তথ্য,
	XmListCallbackStruct *call_তথ্য)
{
    do_choice(w, তথ্য, call_তথ্য, STYLE);
}

    স্থির শূন্য
আকার_callback(Widget w,
	Shared_Font_Sel_তথ্য_2 *তথ্য,
	XmListCallbackStruct *call_তথ্য)
{
    do_choice(w, তথ্য, call_তথ্য, আকার_4);
}

    স্থির শূন্য
cancel_callback(Widget w UNUSED,
	Shared_Font_Sel_তথ্য_2 *তথ্য,
	XmListCallbackStruct *call_তথ্য UNUSED)
{
    যদি (তথ্য->sel[ENCODING])
    {
	XtFree(তথ্য->sel[ENCODING]);
	তথ্য->sel[ENCODING] = ফাঁকা;
    }
    যদি (তথ্য->sel[NAME])
    {
	XtFree(তথ্য->sel[NAME]);
	তথ্য->sel[NAME] = ফাঁকা;
    }
    যদি (তথ্য->sel[STYLE])
    {
	XtFree(তথ্য->sel[STYLE]);
	তথ্য->sel[STYLE] = ফাঁকা;
    }
    যদি (তথ্য->sel[আকার_4])
    {
	XtFree(তথ্য->sel[আকার_4]);
	তথ্য->sel[আকার_4] = ফাঁকা;
    }

    যদি (তথ্য->font_name)
	XtFree(তথ্য->font_name);
    তথ্য->font_name = ফাঁকা;

    তথ্য->অঙ্ক = 0;
    XFreeFontNames(তথ্য->names);
    তথ্য->names = ফাঁকা;
    তথ্য->exit = True;
}

    স্থির শূন্য
ok_callback(Widget w UNUSED,
	Shared_Font_Sel_তথ্য_2 *তথ্য,
	Xm_ঠেলো_Button_Callback_Struct *call_তথ্য UNUSED)
{
    অক্ষর    *pattern;
    অক্ষর    **name;
    পূর্ণসংখ্যা	    i;

    pattern = XmTextGetString(তথ্য->name);
    name    = XListFonts(XtDisplay(তথ্য->dialog), pattern, 1, &i);
    XtFree(pattern);

    যদি (i != 1)
    {
	do_dialog(VIM_ERROR,
		(char_u *)_("Error"),
		(char_u *)_("Invalid font specification"),
		(char_u *)_("&Dismiss"), 1, ফাঁকা, FALSE);
	XFreeFontNames(name);
    }
    অন্যথা
    {
	যদি (তথ্য->font_name)
	    XtFree(তথ্য->font_name);
	তথ্য->font_name = XtNewString(name[0]);

	যদি (তথ্য->sel[ENCODING])
	{
	    XtFree(তথ্য->sel[ENCODING]);
	    তথ্য->sel[ENCODING] = ফাঁকা;
	}
	যদি (তথ্য->sel[NAME])
	{
	    XtFree(তথ্য->sel[NAME]);
	    তথ্য->sel[NAME] = ফাঁকা;
	}
	যদি (তথ্য->sel[STYLE])
	{
	    XtFree(তথ্য->sel[STYLE]);
	    তথ্য->sel[STYLE] = ফাঁকা;
	}
	যদি (তথ্য->sel[আকার_4])
	{
	    XtFree(তথ্য->sel[আকার_4]);
	    তথ্য->sel[আকার_4] = ফাঁকা;
	}

	XFreeFontNames(name);

	তথ্য->অঙ্ক = 0;
	XFreeFontNames(তথ্য->names);
	তথ্য->names = ফাঁকা;
	তথ্য->exit = True;
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
    স্থির Shared_Font_Sel_তথ্য_2	তথ্য_3;

    Widget		parent;
    Widget		form;
    Widget		separator;
    Widget		sub_form;
    Widget		আকার_toggle;
    Widget		name;
    Widget		disp_frame;
    Widget		frame;
    Arg			args[64];
    পূর্ণসংখ্যা			n;
    XmString		str;
    অক্ষর		big_font[MAX_FONT_NAME_LEN];
    Shared_Font_Sel_তথ্য_2	*তথ্য;

    তথ্য = &তথ্য_3;

    parent = vimShell;
    তথ্য->names = XListFonts(XtDisplay(parent), "-*-*-*-*-*-*-*-*-*-*-*-*-*-*",
						       MAX_FONTS, &তথ্য->অঙ্ক);

    /*
     * Find the name of the biggest font less than the given limit
     * MAX_DISPLAY_SIZE used to set up the initial height of the display
     * widget.
     */

    {
	পূর্ণসংখ্যা	i;
	পূর্ণসংখ্যা	max;
	পূর্ণসংখ্যা	idx = 0;
	পূর্ণসংখ্যা	আকার_2;
	অক্ষর	buf[128];

	জন্য (i = 0, max = 0; i < তথ্য->অঙ্ক; i++)
	{
	    get_part(fn(তথ্য, i), 7, buf);
	    আকার_2 = atoi(buf);
	    যদি ((আকার_2 > max) && (আকার_2 < MAX_DISPLAY_আকার))
	    {
		idx = i;
		max = আকার_2;
	    }
	}
	strcpy(big_font, fn(তথ্য, idx));
    }
    তথ্য->old = XLoadQueryFont(XtDisplay(parent), big_font);
    তথ্য->old_list = gui_motif_create_fontlist(তথ্য->old);

    // Set the title of the Dialog window.
    তথ্য->dialog = XmCreateDialogShell(parent, "fontSelector", ফাঁকা, 0);
    str = XmStringCreateLocalized(_("Vim - Font Selector"));

    // Create form popup dialog widget.
    form = XtVaCreateWidget("form",
	    xmFormWidgetClass, তথ্য->dialog,
	    XmNdialogTitle, str,
	    XmNautoUnmanage, False,
	    XmNdialogStyle, XmDIALOG_FULL_APPLICATION_MODAL,
	    ফাঁকা);
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
	    ফাঁকা);

    তথ্য->ok = XtVaCreateManagedWidget(_("OK"),
	    xm_ঠেলো_Button_Gadget_Class, sub_form,
	    XmNleftAttachment, XmATTACH_FORM,
	    XmNrightAttachment, XmATTACH_FORM,
	    XmNtopAttachment, XmATTACH_FORM,
	    XmNtopOffset, 4,
	    ফাঁকা);
    apply_fontlist(তথ্য->ok);

    তথ্য->cancel = XtVaCreateManagedWidget(_("Cancel"),
	    xm_ঠেলো_Button_Gadget_Class, sub_form,
	    XmNrightAttachment, XmATTACH_FORM,
	    XmNleftAttachment, XmATTACH_FORM,
	    XmNtopAttachment, XmATTACH_WIDGET,
	    XmNtopWidget, তথ্য->ok,
	    XmNtopOffset, 4,
	    XmNshowAsDefault, True,
	    ফাঁকা);
    apply_fontlist(তথ্য->cancel);

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
    তথ্য->name = XtVaCreateManagedWidget("fontName",
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
	    Xm_Nmax_দৈর্ঘ্য, MAX_FONT_NAME_LEN,
	    XmNcolumns, 60,
	    ফাঁকা);

    str = XmStringCreateLocalized(_("Name:"));
    name = XtVaCreateManagedWidget("fontNameLabel",
	    xmLabelGadgetClass, form,
	    XmNlabelString, str,
	    Xm_Nuser_তথ্য, তথ্য->name,
	    XmNleftAttachment, XmATTACH_OPPOSITE_WIDGET,
	    XmNleftWidget, তথ্য->name,
	    XmNbottomAttachment, XmATTACH_WIDGET,
	    XmNbottomWidget, তথ্য->name,
	    XmNtopOffset, 1,
	    ফাঁকা);
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
	    ফাঁকা);

    তথ্য->sample = XtVaCreateManagedWidget("sampleLabel",
	    xmLabelWidgetClass, disp_frame,
	    XmNleftAttachment, XmATTACH_FORM,
	    XmNtopAttachment, XmATTACH_FORM,
	    XmNbottomAttachment, XmATTACH_FORM,
	    XmNrightAttachment, XmATTACH_FORM,
	    XmNalignment, XmALIGNMENT_BEGINNING,
	    Xm_Nrecompute_আকার, False,
	    XmNfontList, তথ্য->old_list,
	    ফাঁকা);

    // create toggle button
    str = XmStringCreateLocalized(_("Show size in Points"));
    আকার_toggle = XtVaCreateManagedWidget("sizeToggle",
	    xmToggleButtonGadgetClass, form,
	    XmNlabelString, str,
	    XmNleftAttachment, XmATTACH_FORM,
	    XmNleftOffset, 4,
	    XmNbottomAttachment, XmATTACH_WIDGET,
	    XmNbottomWidget, disp_frame,
	    XmNbottomOffset, 4,
	    ফাঁকা);
    XmStringFree(str);
    apply_fontlist(আকার_toggle);
    XtManageChild(আকার_toggle);

    // Encoding pulldown menu.

    তথ্য->encoding_pulldown = XmCreatePulldownMenu(form,
						 "encodingPulldown", ফাঁকা, 0);
    str = XmStringCreateLocalized(_("Encoding:"));
    n = 0;
    XtSetArg(args[n], XmNsubMenuId, তথ্য->encoding_pulldown); ++n;
    XtSetArg(args[n], XmNlabelString, str); ++n;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); ++n;
    XtSetArg(args[n], XmNleftOffset, 4); ++n;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_WIDGET); ++n;
    XtSetArg(args[n], XmNbottomWidget, আকার_toggle); ++n;
    XtSetArg(args[n], XmNbottomOffset, 4); ++n;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_WIDGET); ++n;
    XtSetArg(args[n], XmNrightWidget, separator); ++n;
    XtSetArg(args[n], XmNrightOffset, 4); ++n;
    তথ্য->encoding_menu = XmCreateOptionMenu(form, "encodingMenu", args, n);
    XmStringFree(str);
    XmAddTabGroup(তথ্য->encoding_menu);

    /*
     * Create scroll list widgets in a separate subform used to manage the
     * different sizes of the lists.
     */

    sub_form = XtVaCreateManagedWidget("subForm",
	    xmFormWidgetClass, form,
	    XmNbottomAttachment, XmATTACH_WIDGET,
	    XmNbottomWidget, তথ্য->encoding_menu,
	    XmNbottomOffset, 4,
	    XmNleftAttachment, XmATTACH_FORM,
	    XmNleftOffset, 4,
	    XmNrightAttachment, XmATTACH_WIDGET,
	    XmNrightWidget, separator,
	    XmNrightOffset, 4,
	    XmNtopAttachment, XmATTACH_FORM,
	    XmNtopOffset, 2,
	    XmNorientation, XmVERTICAL,
	    ফাঁকা);

    // font list
    frame = XtVaCreateManagedWidget("frame", xmFrameWidgetClass, sub_form,
	    XmNshadowThickness, 0,
	    XmNtopAttachment, XmATTACH_FORM,
	    XmNbottomAttachment, XmATTACH_FORM,
	    XmNleftAttachment, XmATTACH_FORM,
	    XmNrightAttachment, XmATTACH_POSITION,
	    XmNrightPosition, 50,
	    ফাঁকা);

    str = XmStringCreateLocalized(_("Font:"));
    name = XtVaCreateManagedWidget("nameListLabel", xmLabelGadgetClass, frame,
	    XmNchildType, XmFRAME_TITLE_CHILD,
	    XmNchildVerticalAlignment, XmALIGNMENT_CENTER,
	    XmNchildHorizontalAlignment, XmALIGNMENT_BEGINNING,
	    XmNlabelString, str,
	    ফাঁকা);
    XmStringFree(str);
    apply_fontlist(name);

    n = 0;
    XtSetArg(args[n], Xm_Nvisible_Item_গুনতি, 8); ++n;
    XtSetArg(args[n], XmNresizable, True); ++n;
    XtSetArg(args[n], Xm_Nlist_আকার_Policy, XmCONSTANT); ++n;
    XtSetArg(args[n], XmNvisualPolicy, XmVARIABLE); ++n;
#ifdef LESSTIF_VERSION
    XtSetArg(args[n], XmNscrollBarDisplayPolicy, XmSTATIC); ++n;
#endif
    তথ্য->list[NAME] = XmCreateScrolledList(frame, "fontList", args, n);
    Xt_Va_Set_মানসমূহ(name, Xm_Nuser_তথ্য, তথ্য->list[NAME], ফাঁকা);

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
	    ফাঁকা);

    str = XmStringCreateLocalized(_("Style:"));
    name = XtVaCreateManagedWidget("styleListLabel", xmLabelWidgetClass, frame,
	    XmNchildType, XmFRAME_TITLE_CHILD,
	    XmNchildVerticalAlignment, XmALIGNMENT_CENTER,
	    XmNchildHorizontalAlignment, XmALIGNMENT_BEGINNING,
	    XmNlabelString, str,
	    ফাঁকা);
    XmStringFree(str);
    apply_fontlist(name);

    n = 0;
    XtSetArg(args[n], Xm_Nvisible_Item_গুনতি, 8); ++n;
    XtSetArg(args[n], XmNresizable, True); ++n;
    XtSetArg(args[n], Xm_Nlist_আকার_Policy, XmCONSTANT); ++n;
    XtSetArg(args[n], XmNvisualPolicy, XmVARIABLE); ++n;
#ifdef LESSTIF_VERSION
    XtSetArg(args[n], XmNscrollBarDisplayPolicy, XmSTATIC); ++n;
#endif
    তথ্য->list[STYLE] = XmCreateScrolledList(frame, "styleList", args, n);
    Xt_Va_Set_মানসমূহ(name, Xm_Nuser_তথ্য, তথ্য->list[STYLE], ফাঁকা);

    // size list
    frame = XtVaCreateManagedWidget("frame", xmFrameWidgetClass, sub_form,
	    XmNshadowThickness, 0,
	    XmNtopAttachment, XmATTACH_FORM,
	    XmNbottomAttachment, XmATTACH_FORM,
	    XmNleftAttachment, XmATTACH_POSITION,
	    XmNleftPosition, 80,
	    XmNleftOffset, 4,
	    XmNrightAttachment, XmATTACH_FORM,
	    ফাঁকা);

    str = XmStringCreateLocalized(_("Size:"));
    name = XtVaCreateManagedWidget("sizeListLabel", xmLabelGadgetClass, frame,
	    XmNchildType, XmFRAME_TITLE_CHILD,
	    XmNchildVerticalAlignment, XmALIGNMENT_CENTER,
	    XmNchildHorizontalAlignment, XmALIGNMENT_BEGINNING,
	    XmNlabelString, str,
	    ফাঁকা);
    XmStringFree(str);
    apply_fontlist(name);

    n = 0;
    XtSetArg(args[n], Xm_Nvisible_Item_গুনতি, 8); ++n;
    XtSetArg(args[n], XmNresizable, True); ++n;
    XtSetArg(args[n], Xm_Nlist_আকার_Policy, XmCONSTANT); ++n;
    XtSetArg(args[n], XmNvisualPolicy, XmVARIABLE); ++n;
#ifdef LESSTIF_VERSION
    XtSetArg(args[n], XmNscrollBarDisplayPolicy, XmSTATIC); ++n;
#endif
    তথ্য->list[আকার_4] = XmCreateScrolledList(frame, "sizeList", args, n);
    Xt_Va_Set_মানসমূহ(name, Xm_Nuser_তথ্য, তথ্য->list[আকার_4], ফাঁকা);

    // update form widgets cancel button
    Xt_Va_Set_মানসমূহ(form, XmNcancelButton, তথ্য->cancel, ফাঁকা);

    XtAddCallback(আকার_toggle, XmNvalueChangedCallback,
	    (XtCallbackProc)stoggle_callback, (XtPointer)তথ্য);
    XtAddCallback(তথ্য->list[NAME], XmNbrowseSelectionCallback,
	    (XtCallbackProc)name_callback, (XtPointer)তথ্য);
    XtAddCallback(তথ্য->list[STYLE], XmNbrowseSelectionCallback,
	    (XtCallbackProc)style_callback, (XtPointer)তথ্য);
    XtAddCallback(তথ্য->list[আকার_4], XmNbrowseSelectionCallback,
	    (XtCallbackProc)আকার_callback, (XtPointer)তথ্য);
    XtAddCallback(তথ্য->ok, XmNactivateCallback,
	    (XtCallbackProc)ok_callback, (XtPointer)তথ্য);
    XtAddCallback(তথ্য->cancel, XmNactivateCallback,
	    (XtCallbackProc)cancel_callback, (XtPointer)তথ্য);

    XmProcessTraversal(তথ্য->list[NAME], XmTRAVERSE_CURRENT);

    // setup tabgroups

    XmAddTabGroup(তথ্য->list[NAME]);
    XmAddTabGroup(তথ্য->list[STYLE]);
    XmAddTabGroup(তথ্য->list[আকার_4]);
    XmAddTabGroup(আকার_toggle);
    XmAddTabGroup(তথ্য->name);
    XmAddTabGroup(তথ্য->ok);
    XmAddTabGroup(তথ্য->cancel);

    add_cancel_action(তথ্য->dialog, (XtCallbackProc)cancel_callback, তথ্য);

    // Preset selection data.

    তথ্য->exit = False;
    তথ্য->in_pixels= True;
    তথ্য->sel[ENCODING] = ফাঁকা;
    তথ্য->sel[NAME] = ফাঁকা;
    তথ্য->sel[STYLE] = ফাঁকা;
    তথ্য->sel[আকার_4] = ফাঁকা;
    তথ্য->font_name = ফাঁকা;

    // set up current font parameters
    যদি (current && current[0] != '\0')
    {
	পূর্ণসংখ্যা	    i;
	অক্ষর	    **names;

	names = XListFonts(XtDisplay(form), (অক্ষর *) current, 1, &i);

	যদি (i != 0)
	{
	    অক্ষর namebuf[অস্থায়ী_BUF_আকার];
	    অক্ষর stylebuf[অস্থায়ী_BUF_আকার];
	    অক্ষর sizebuf[অস্থায়ী_BUF_আকার];
	    অক্ষর encodingbuf[অস্থায়ী_BUF_আকার];
	    অক্ষর *found;

	    found = names[0];

	    name_part(found, namebuf);
	    style_part(found, stylebuf);
	    আকার_part(found, sizebuf, তথ্য->in_pixels);
	    encoding_part(found, encodingbuf);

	    যদি (*namebuf != NUL
		    && *stylebuf != NUL
		    && *sizebuf != NUL
		    && *encodingbuf != NUL)
	    {
		তথ্য->sel[NAME] = XtNewString(namebuf);
		তথ্য->sel[STYLE] = XtNewString(stylebuf);
		তথ্য->sel[আকার_4] = XtNewString(sizebuf);
		তথ্য->sel[ENCODING] = XtNewString(encodingbuf);
		তথ্য->font_name = XtNewString(names[0]);
		display_sample(তথ্য);
		XmTextSetString(তথ্য->name, তথ্য->font_name);
	    }
	    অন্যথা
	    {
		// We can't preset a symbolic name, which isn't a full font
		// description. Therefore we just behave the same way as if the
		// user didn't have selected anything thus far.
		//
		// Unfortunately there is no known way to expand an abbreviated
		// font name.
		তথ্য->font_name = ফাঁকা;
	    }
	}
	XFreeFontNames(names);
    }

    fill_lists(NONE, তথ্য);

    // Unfortunately LessTif doesn't align the list widget's properly.  I don't
    // have currently any idea how to fix this problem.
    XtManageChild(তথ্য->list[NAME]);
    XtManageChild(তথ্য->list[STYLE]);
    XtManageChild(তথ্য->list[আকার_4]);
    XtManageChild(তথ্য->encoding_menu);
    manage_centered(form);

    // modal event loop
    যখন (!তথ্য->exit)
	XtAppProcessEvent(XtWidgetToApplicationContext(তথ্য->dialog),
							(Xt_প্রবেশ_Mask)XtIMAll);

    যদি (তথ্য->old)
    {
	XFreeFont(XtDisplay(তথ্য->dialog),  তথ্য->old);
	XmFontListFree(তথ্য->old_list);
    }
    XtDestroyWidget(তথ্য->dialog);

    gui_motif_synch_fonts();

    ফেরত (char_u *) তথ্য->font_name;
}
