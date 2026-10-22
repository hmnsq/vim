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

#అన్తర్భావ "vim.h"

#అన్తర్భావ <Xm/Form.h>
#అన్తర్భావ <Xm/PushBG.h>
#అన్తర్భావ <Xm/Text.h>
#అన్తర్భావ <Xm/TextF.h>
#అన్తర్భావ <Xm/Label.h>
#అన్తర్భావ <Xm/Frame.h>
#అన్తర్భావ <Xm/LabelG.h>
#అన్తర్భావ <Xm/ToggleBG.h>
#అన్తర్భావ <Xm/SeparatoG.h>
#అన్తర్భావ <Xm/DialogS.h>
#అన్తర్భావ <Xm/List.h>
#అన్తర్భావ <Xm/RowColumn.h>
#అన్తర్భావ <Xm/AtomMgr.h>
#అన్తర్భావ <Xm/Protocols.h>

#అన్తర్భావ <X11/keysym.h>
#అన్తర్భావ <X11/Xatom.h>
#అన్తర్భావ <X11/StringDefs.h>
#అన్తర్భావ <X11/Intrinsic.h>

బాహ్య Widget vimShell;

#నిర్వచితమైతే FEAT_MENU
# define apply_fontlist(w) gui_motif_menu_fontlist(w)
#లేకపోతే
# define apply_fontlist(w)
#అయితే_ముగింపు

/////////////////////////////////////////////////////////////////////////////
// Font selection dialogue implementation.

స్థిర అక్షరం wild[3] = "*";

/*
 * FIXME: This is a generic function, which should be used throughout the whole
 * application.
 *
 * Add close_callback, which will be called when the user selects close from
 * the window menu.  The close menu item usually activates f.kill which sends a
 * WM_DELETE_WINDOW protocol request for the window.
 */

    స్థిర శూన్య
కూడు_cancel_action(Widget shell, XtCallbackProc close_callback, శూన్య *arg)
{
    స్థిర Atom wmp_atom = 0;
    స్థిర Atom dw_atom = 0;
    Display *display = XtDisplay(shell);

    // deactivate the built-in delete response of killing the application
    Xt_Va_సమితి_విలువలు(shell, XmNdeleteResponse, XmDO_NOTHING, ఖాళీ);

    // add a delete window protocol callback instead
    ఒకవేళ (!dw_atom)
    {
	wmp_atom = XmInternAtom(display, "WM_PROTOCOLS", True);
	dw_atom = XmInternAtom(display, "WM_DELETE_WINDOW", True);
    }
    Xm_కూడు_Protocol_Callback(shell, wmp_atom, dw_atom, close_callback, arg);
}

#నిర్వచించు MAX_FONTS			65535
#నిర్వచించు MAX_FONT_NAME_LEN		256
#నిర్వచించు MAX_ENTRIES_లోపల_LIST		5000
#నిర్వచించు MAX_DISPLAY_కొలత		150
#నిర్వచించు తాత్కాలిక_BUF_కొలత			256

గణన ListSpecifier
{
    ENCODING,
    NAME,
    STYLE,
    కొలత_3,
    NONE
};

రకనిర్వచనం నిర్మాణం Shared_Font_Sel_దత్తాంశం
{
    Widget	dialog;
    Widget	ok;
    Widget	cancel;
    Widget	encoding_pulldown;
    Widget	encoding_menu;
    Widget	list[NONE];
    Widget	name;
    Widget	sample;
    అక్షరం	**పేర్లు;	// font name array of arrays
    పూర్ణాంకం		అంకె;		// number of font names
    String	sel[NONE];	// selection category
    Boolean	లోపల_pixels;	// toggle state - size in pixels
    అక్షరం	*font_name;	// current font name
    XFontStruct	*old;		// font data structure for sample display
    XmFontList	old_list;	// font data structure for sample display
    Boolean	నిష్క్రమించు;		// used for program exit control
} Shared_Font_Sel_దత్తాంశం_2;

/*
 * Checking access to the font name array for validity.
 */
    స్థిర అక్షరం *
fn(Shared_Font_Sel_దత్తాంశం_2 *దత్తాంశం, పూర్ణాంకం i)
{
    // Assertion checks:
    ఒకవేళ (దత్తాంశం->అంకె < 0)
	abort();
    ఒకవేళ (i >= దత్తాంశం->అంకె)
	i = దత్తాంశం->అంకె - 1;
    ఒకవేళ (i < 0)
	i = 0;

    తిరిగి దత్తాంశం->పేర్లు[i];
}

/*
 * Get a specific substring from a font name.
 */
    స్థిర శూన్య
get_part(అక్షరం *లోపల, పూర్ణాంకం pos, అక్షరం *బహిర్గతం)
{
    పూర్ణాంకం	i;
    పూర్ణాంకం j;

    *బహిర్గతం = '\0';

    కొరకు (i = 0; (pos > 0) && (లోపల[i] != '\0'); ++i)
	ఒకవేళ (లోపల[i] == '-')
	    pos--;

    ఒకవేళ (లోపల[i] == '\0')
	తిరిగి;

    కొరకు (j = 0; (లోపల[i] != '-') && (లోపల[i] != '\0'); ++i, ++j)
	బహిర్గతం[j] = లోపల[i];
    బహిర్గతం[j] = '\0';
}

/*
 * Given a font name this function returns the part used in the first
 * scroll list.
 */
    స్థిర శూన్య
name_part(అక్షరం *font, అక్షరం *buf)
{
    అక్షరం    buf2[తాత్కాలిక_BUF_కొలత];
    అక్షరం    buf3[తాత్కాలిక_BUF_కొలత];

    get_part(font, 2, buf2);
    get_part(font, 1, buf3);

    ఒకవేళ (*buf3 != NUL)
	vim_snprintf(buf, తాత్కాలిక_BUF_కొలత, "%s (%s)", buf2, buf3);
    లేకపోతే
	vim_snprintf(buf, తాత్కాలిక_BUF_కొలత, "%s", buf2);
}

/*
 * Given a font name this function returns the part used in the second scroll list.
 */
    స్థిర శూన్య
style_part(అక్షరం *font, అక్షరం *buf)
{
    అక్షరం    buf2[తాత్కాలిక_BUF_కొలత];
    అక్షరం    buf3[తాత్కాలిక_BUF_కొలత];

    get_part(font, 3, buf3);
    get_part(font, 5, buf2);

    ఒకవేళ (!strcmp(buf2, "normal") && !strcmp(buf2, "Normal")
						   && !strcmp(buf2, "NORMAL"))
	vim_snprintf(buf, తాత్కాలిక_BUF_కొలత, "%s %s", buf3, buf2);
    లేకపోతే
	strcpy(buf, buf3);

    get_part(font, 6, buf2);

    ఒకవేళ (buf2[0] != '\0')
	vim_snprintf(buf3, తాత్కాలిక_BUF_కొలత, "%s %s", buf, buf2);
    లేకపోతే
	strcpy(buf3, buf);

    get_part(font, 4, buf2);

    ఒకవేళ (!strcmp(buf2, "o") || !strcmp(buf2, "O"))
	vim_snprintf(buf, తాత్కాలిక_BUF_కొలత, "%s oblique", buf3);
    లేకపోతే ఒకవేళ (!strcmp(buf2, "i") || !strcmp(buf2, "I"))
	vim_snprintf(buf, తాత్కాలిక_BUF_కొలత, "%s italic", buf3);

    ఒకవేళ (!strcmp(buf, " "))
	strcpy(buf, "-");
}

/*
 * Given a font name this function returns the part used in the third
 * scroll list.
 */
    స్థిర శూన్య
కొలత_part(అక్షరం *font, అక్షరం *buf, పూర్ణాంకం లోపల_Pixels)
{
    పూర్ణాంకం	    కొలత;
    దశాంశం   తాత్కాలిక;

    *buf = '\0';

    ఒకవేళ (లోపల_Pixels)
    {
	get_part(font, 7, buf);
	ఒకవేళ (*buf != NUL)
	{
	    కొలత = atoi(buf);
	    sprintf(buf, "%3d", కొలత);
	}
    }
    లేకపోతే
    {
	get_part(font, 8, buf);
	ఒకవేళ (*buf != NUL)
	{
	    కొలత = atoi(buf);
	    తాత్కాలిక = (దశాంశం)కొలత / 10.0;
	    కొలత = తాత్కాలిక;
	    ఒకవేళ (buf[అక్షరశ్రేణి_పొడవు(buf) - 1] == '0')
		sprintf(buf, "%3d", కొలత);
	    లేకపోతే
		sprintf(buf, "%4.1f", తాత్కాలిక);
	}
    }
}

/*
 * Given a font name this function returns the part used in the choice menu.
 */
    స్థిర శూన్య
encoding_part(అక్షరం *font, అక్షరం *buf)
{
    అక్షరం    buf1[తాత్కాలిక_BUF_కొలత];
    అక్షరం    buf2[తాత్కాలిక_BUF_కొలత];

    *buf = '\0';

    get_part(font, 13, buf1);
    get_part(font, 14, buf2);

    ఒకవేళ (*buf1 != NUL && *buf2 != NUL)
	vim_snprintf(buf, తాత్కాలిక_BUF_కొలత, "%s-%s", buf1, buf2);
    ఒకవేళ (!strcmp(buf, " "))
	strcpy(buf, "-");
}

/*
 * Inserts a string into correct sorted position in a list.
 */
    స్థిర శూన్య
కూడు_గమ్యం_list(అక్షరం **buf, అక్షరం *అంశం, పూర్ణాంకం *లెక్క)
{
    పూర్ణాంకం	i;
    పూర్ణాంకం j;

    ఒకవేళ (*లెక్క == MAX_ENTRIES_లోపల_LIST)
	తిరిగి;

    // avoid duplication
    కొరకు (i = 0; i < *లెక్క; ++i)
    {
	ఒకవేళ (!strcmp(buf[i], అంశం))
	    తిరిగి;
    }

    // find order place, but make sure that wild card comes first
    ఒకవేళ (!strcmp(అంశం, wild))
	i = 0;
    లేకపోతే
	కొరకు (i = 0; i < *లెక్క; ++i)
	    ఒకవేళ (strcmp(buf[i], అంశం) > 0 && strcmp(buf[i], wild))
		ఆపు;

    // now insert the item
    కొరకు (j = *లెక్క; j > i; --j)
	buf[j] = buf[j-1];
    buf[i] = XtNewString(అంశం);

    ++(*లెక్క);
}

/*
 * True if the font matches some field.
 */
    స్థిర Boolean
సరిపోలిక(Shared_Font_Sel_దత్తాంశం_2 *దత్తాంశం, గణన ListSpecifier l, పూర్ణాంకం i)
{
    అక్షరం buf[తాత్కాలిక_BUF_కొలత];

    // An empty selection or a wild card matches anything.
    ఒకవేళ (!దత్తాంశం->sel[l] || !strcmp(దత్తాంశం->sel[l], wild))
	తిరిగి True;

    // chunk out the desired part...
    ఎంపిక (l)
    {
	స్థితి ENCODING:
	    encoding_part(fn(దత్తాంశం, i), buf);
	    ఆపు;

	స్థితి NAME:
	    name_part(fn(దత్తాంశం, i), buf);
	    ఆపు;

	స్థితి STYLE:
	    style_part(fn(దత్తాంశం, i), buf);
	    ఆపు;

	స్థితి కొలత_3:
	    కొలత_part(fn(దత్తాంశం, i), buf, దత్తాంశం->లోపల_pixels);
	    ఆపు;
	అప్రమేయ:
	    ;
    }

    // ...and chew it now

    తిరిగి !strcmp(buf, దత్తాంశం->sel[l]);
}

    స్థిర Boolean
proportional(అక్షరం *font)
{
    అక్షరం buf[తాత్కాలిక_BUF_కొలత];

    get_part(font, 11, buf);

    తిరిగి !strcmp(buf, "p") || !strcmp(buf, "P");
}


స్థిర శూన్య encoding_callback(Widget w, Shared_Font_Sel_దత్తాంశం_2 *దత్తాంశం,
							     Xt_సూచిక dummy);

/*
 * Parse through the fontlist data and set up the three scroll lists.  The fix
 * parameter can be used to exclude a list from any changes.  This is used for
 * updates after selections caused by the users actions.
 */
    స్థిర శూన్య
fill_lists(గణన ListSpecifier fix, Shared_Font_Sel_దత్తాంశం_2 *దత్తాంశం)
{
    అక్షరం	*list[NONE][MAX_ENTRIES_లోపల_LIST];
    పూర్ణాంకం		లెక్క[NONE];
    అక్షరం	buf[తాత్కాలిక_BUF_కొలత];
    XmString	వస్తువులు[MAX_ENTRIES_లోపల_LIST];
    పూర్ణాంకం		i;
    పూర్ణాంకం		idx;

    కొరకు (idx = (పూర్ణాంకం)ENCODING; idx < (పూర్ణాంకం)NONE; ++idx)
	లెక్క[idx] = 0;

    // First we insert the wild char into every single list.
    ఒకవేళ (fix != ENCODING)
	కూడు_గమ్యం_list(list[ENCODING], wild, &లెక్క[ENCODING]);
    ఒకవేళ (fix != NAME)
	కూడు_గమ్యం_list(list[NAME], wild, &లెక్క[NAME]);
    ఒకవేళ (fix != STYLE)
	కూడు_గమ్యం_list(list[STYLE], wild, &లెక్క[STYLE]);
    ఒకవేళ (fix != కొలత_3)
	కూడు_గమ్యం_list(list[కొలత_3], wild, &లెక్క[కొలత_3]);

    కొరకు (i = 0; i < దత్తాంశం->అంకె && i < MAX_ENTRIES_లోపల_LIST; i++)
    {
	ఒకవేళ (proportional(fn(దత్తాంశం, i)))
	    కొనసాగు;

	ఒకవేళ (fix != ENCODING
		&& సరిపోలిక(దత్తాంశం, NAME, i)
		&& సరిపోలిక(దత్తాంశం, STYLE, i)
		&& సరిపోలిక(దత్తాంశం, కొలత_3, i))
	{
	    encoding_part(fn(దత్తాంశం, i), buf);
	    కూడు_గమ్యం_list(list[ENCODING], buf, &లెక్క[ENCODING]);
	}

	ఒకవేళ (fix != NAME
		&& సరిపోలిక(దత్తాంశం, ENCODING, i)
		&& సరిపోలిక(దత్తాంశం, STYLE, i)
		&& సరిపోలిక(దత్తాంశం, కొలత_3, i))
	{
	    name_part(fn(దత్తాంశం, i), buf);
	    కూడు_గమ్యం_list(list[NAME], buf, &లెక్క[NAME]);
	}

	ఒకవేళ (fix != STYLE
		&& సరిపోలిక(దత్తాంశం, ENCODING, i)
		&& సరిపోలిక(దత్తాంశం, NAME, i)
		&& సరిపోలిక(దత్తాంశం, కొలత_3, i))
	{
	    style_part(fn(దత్తాంశం, i), buf);
	    కూడు_గమ్యం_list(list[STYLE], buf, &లెక్క[STYLE]);
	}

	ఒకవేళ (fix != కొలత_3
		&& సరిపోలిక(దత్తాంశం, ENCODING, i)
		&& సరిపోలిక(దత్తాంశం, NAME, i)
		&& సరిపోలిక(దత్తాంశం, STYLE, i))
	{
	    కొలత_part(fn(దత్తాంశం, i), buf, దత్తాంశం->లోపల_pixels);
	    కూడు_గమ్యం_list(list[కొలత_3], buf, &లెక్క[కొలత_3]);
	}
    }

    /*
     * And now do the preselection in all lists where there was one:
     */

    ఒకవేళ (fix != ENCODING)
    {
	Cardinal n_వస్తువులు;
	WidgetList children;
	Widget selected_button = 0;

	// Get and update the current button list.
	Xt_Va_Get_విలువలు(దత్తాంశం->encoding_pulldown,
		XmNchildren, &children,
		XmNnumChildren, &n_వస్తువులు,
		ఖాళీ);

	కొరకు (i = 0; i < లెక్క[ENCODING]; ++i)
	{
	    Widget button;

	    వస్తువులు[i] = XmStringCreateLocalized(list[ENCODING][i]);

	    ఒకవేళ (i < (పూర్ణాంకం)n_వస్తువులు)
	    {
		// recycle old button
		Xt_Va_సమితి_విలువలు(children[i],
			XmNlabelString, వస్తువులు[i],
			Xm_Nuser_దత్తాంశం, i,
			ఖాళీ);
		button = children[i];
	    }
	    లేకపోతే
	    {
		// create a new button
		button = XtVaCreateManagedWidget("button",
			xm_నెట్టు_Button_Gadget_Class,
			దత్తాంశం->encoding_pulldown,
			XmNlabelString, వస్తువులు[i],
			Xm_Nuser_దత్తాంశం, i,
			ఖాళీ);
		Xt_కూడు_Callback(button, XmNactivateCallback,
			(XtCallbackProc) encoding_callback, (Xt_సూచిక) దత్తాంశం);
		Xt_Manage_బిడ్డ(button);
	    }

	    ఒకవేళ (దత్తాంశం->sel[ENCODING])
	    {
		ఒకవేళ (!strcmp(దత్తాంశం->sel[ENCODING], list[ENCODING][i]))
		    selected_button = button;
	    }
	    XtFree(list[ENCODING][i]);
	}

	// Destroy all the outstanding menu items.
	కొరకు (i = లెక్క[ENCODING]; i < (పూర్ణాంకం)n_వస్తువులు; ++i)
	{
	    Xt_Unmanage_బిడ్డ(children[i]);
	    XtDestroyWidget(children[i]);
	}

	// Preserve the current selection visually.
	ఒకవేళ (selected_button)
	{
	    Xt_Va_సమితి_విలువలు(దత్తాంశం->encoding_menu,
		    XmNmenuHistory, selected_button,
		    ఖాళీ);
	}

	కొరకు (i = 0; i < లెక్క[ENCODING]; ++i)
	    XmStringFree(వస్తువులు[i]);
    }

    /*
     * Now loop through the remaining lists and set them up.
     */
    కొరకు (idx = (పూర్ణాంకం)NAME; idx < (పూర్ణాంకం)NONE; ++idx)
    {
	Widget w;

	ఒకవేళ (fix == (గణన ListSpecifier)idx)
	    కొనసాగు;

	ఎంపిక ((గణన ListSpecifier)idx)
	{
	    స్థితి NAME:
		w = దత్తాంశం->list[NAME];
		ఆపు;
	    స్థితి STYLE:
		w = దత్తాంశం->list[STYLE];
		ఆపు;
	    స్థితి కొలత_3:
		w = దత్తాంశం->list[కొలత_3];
		ఆపు;
	    అప్రమేయ:
		w = (Widget)0;	// for lint
	}

	కొరకు (i = 0; i < లెక్క[idx]; ++i)
	{
	    వస్తువులు[i] = XmStringCreateLocalized(list[idx][i]);
	    XtFree(list[idx][i]);
	}
	Xm_List_Delete_All_వస్తువులు(w);
	Xm_List_కూడు_వస్తువులు(w, వస్తువులు, లెక్క[idx], 1);
	ఒకవేళ (దత్తాంశం->sel[idx])
	{
	    XmStringFree(వస్తువులు[0]);
	    వస్తువులు[0] = XmStringCreateLocalized(దత్తాంశం->sel[idx]);
	    Xm_List_Select_అంశం(w, వస్తువులు[0], False);
	    Xm_List_సమితి_Bottom_అంశం(w, వస్తువులు[0]);
	}
	కొరకు (i = 0; i < లెక్క[idx]; ++i)
	    XmStringFree(వస్తువులు[i]);
    }
}

    స్థిర శూన్య
stoggle_callback(Widget w వాడని,
	Shared_Font_Sel_దత్తాంశం_2 *దత్తాంశం,
	XmToggleButtonCallbackStruct *call_దత్తాంశం)
{
    పూర్ణాంకం		i, do_sel;
    అక్షరం	new_కొలత_2[తాత్కాలిక_BUF_కొలత];
    XmString	str;

    ఒకవేళ (call_దత్తాంశం->reason != (పూర్ణాంకం)Xm_CR_విలువ_మారింది)
	తిరిగి;

    do_sel = (దత్తాంశం->sel[కొలత_3] != ఖాళీ) && strcmp(దత్తాంశం->sel[కొలత_3], wild);

    కొరకు (i = 0; do_sel && (i < దత్తాంశం->అంకె); i++)
	ఒకవేళ (సరిపోలిక(దత్తాంశం, ENCODING, i)
		&& సరిపోలిక(దత్తాంశం, NAME, i)
		&& సరిపోలిక(దత్తాంశం, STYLE, i)
		&& సరిపోలిక(దత్తాంశం, కొలత_3, i))
	{
	    కొలత_part(fn(దత్తాంశం, i), new_కొలత_2, !దత్తాంశం->లోపల_pixels);
	    ఆపు;
	}

    దత్తాంశం->లోపల_pixels = !దత్తాంశం->లోపల_pixels;

    ఒకవేళ (దత్తాంశం->sel[కొలత_3])
	XtFree(దత్తాంశం->sel[కొలత_3]);
    దత్తాంశం->sel[కొలత_3] = ఖాళీ;
    fill_lists(NONE, దత్తాంశం);

    ఒకవేళ (do_sel)
    {
	str = XmStringCreateLocalized(new_కొలత_2);
	Xm_List_Select_అంశం(దత్తాంశం->list[కొలత_3], str, True);
	Xm_List_సమితి_Bottom_అంశం(దత్తాంశం->list[కొలత_3], str);
	XmStringFree(str);
    }
}

/*
 * Show the currently selected font in the sample text label.
 */
    స్థిర శూన్య
display_sample(Shared_Font_Sel_దత్తాంశం_2 *దత్తాంశం)
{
    Arg		    వాదనలు[2];
    పూర్ణాంకం		    n;
    XFontStruct	*   font;
    XmFontList	    font_list;
    Display *	    display;
    XmString	    str;

    display = XtDisplay(దత్తాంశం->dialog);
    font = XLoadQueryFont(display, దత్తాంశం->font_name);
    font_list = gui_motif_create_fontlist(font);

    n = 0;
    str = XmStringCreateLocalized("AaBbZzYy 0123456789");
    Xt_సమితి_Arg(వాదనలు[n], XmNlabelString, str); n++;
    Xt_సమితి_Arg(వాదనలు[n], XmNfontList, font_list); n++;

    Xt_సమితి_విలువలు(దత్తాంశం->sample, వాదనలు, n);
    XmStringFree(str);

    ఒకవేళ (దత్తాంశం->old)
    {
	XFreeFont(display, దత్తాంశం->old);
	XmFontListFree(దత్తాంశం->old_list);
    }
    దత్తాంశం->old = font;
    దత్తాంశం->old_list = font_list;
}


    స్థిర Boolean
do_choice(Widget w,
	Shared_Font_Sel_దత్తాంశం_2 *దత్తాంశం,
	XmListCallbackStruct *call_దత్తాంశం,
	గణన ListSpecifier which)
{
    అక్షరం *sel;

    XmStringGetLtoR(call_దత్తాంశం->అంశం, XmSTRING_DEFAULT_CHARSET, &sel);

    ఒకవేళ (!దత్తాంశం->sel[which])
	దత్తాంశం->sel[which] = XtNewString(sel);
    లేకపోతే
    {
	ఒకవేళ (!strcmp(దత్తాంశం->sel[which], sel))
	{
	    // unselecting current selection
	    XtFree(దత్తాంశం->sel[which]);
	    దత్తాంశం->sel[which] = ఖాళీ;
	    ఒకవేళ (w)
		Xm_List_Deselect_అంశం(w, call_దత్తాంశం->అంశం);
	}
	లేకపోతే
	{
	    XtFree(దత్తాంశం->sel[which]);
	    దత్తాంశం->sel[which] = XtNewString(sel);
	}
    }
    XtFree(sel);

    fill_lists(which, దత్తాంశం);

    // If there is a font selection, we display it.
    ఒకవేళ (దత్తాంశం->sel[ENCODING]
	    && దత్తాంశం->sel[NAME]
	    && దత్తాంశం->sel[STYLE]
	    && దత్తాంశం->sel[కొలత_3]
	    && strcmp(దత్తాంశం->sel[ENCODING], wild)
	    && strcmp(దత్తాంశం->sel[NAME], wild)
	    && strcmp(దత్తాంశం->sel[STYLE], wild)
	    && strcmp(దత్తాంశం->sel[కొలత_3], wild))
    {
	పూర్ణాంకం i;

	ఒకవేళ (దత్తాంశం->font_name)
	    XtFree(దత్తాంశం->font_name);
	దత్తాంశం->font_name = ఖాళీ;

	కొరకు (i = 0; i < దత్తాంశం->అంకె; i++)
	{
	    ఒకవేళ (సరిపోలిక(దత్తాంశం, ENCODING, i)
		    && సరిపోలిక(దత్తాంశం, NAME, i)
		    && సరిపోలిక(దత్తాంశం, STYLE, i)
		    && సరిపోలిక(దత్తాంశం, కొలత_3, i))
	    {
		దత్తాంశం->font_name = XtNewString(fn(దత్తాంశం, i));
		ఆపు;
	    }
	}

	ఒకవేళ (దత్తాంశం->font_name)
	{
	    Xm_పాఠ్యం_సమితి_String(దత్తాంశం->name, దత్తాంశం->font_name);
	    display_sample(దత్తాంశం);
	}
	లేకపోతే
	    do_dialog(VIM_ERROR,
		    (char_u *)_("Error"),
		    (char_u *)_("Invalid font specification"),
		    (char_u *)_("&Dismiss"), 1, ఖాళీ, FALSE);

	తిరిగి True;
    }
    లేకపోతే
    {
	పూర్ణాంకం	    n;
	XmString    str;
	Arg	    వాదనలు[4];
	అక్షరం	    *nomatch_సందేశం = _("no specific match");

	n = 0;
	str = XmStringCreateLocalized(nomatch_సందేశం);
	Xt_సమితి_Arg(వాదనలు[n], XmNlabelString, str); ++n;
	Xt_సమితి_విలువలు(దత్తాంశం->sample, వాదనలు, n);
	apply_fontlist(దత్తాంశం->sample);
	Xm_పాఠ్యం_సమితి_String(దత్తాంశం->name, nomatch_సందేశం);
	XmStringFree(str);

	తిరిగి False;
    }
}

    స్థిర శూన్య
encoding_callback(Widget w,
	Shared_Font_Sel_దత్తాంశం_2 *దత్తాంశం,
	Xt_సూచిక dummy వాడని)
{
    XmString str;
    XmListCallbackStruct fake_దత్తాంశం;

    Xt_Va_Get_విలువలు(w, XmNlabelString, &str, ఖాళీ);

    ఒకవేళ (!str)
	తిరిగి;

    fake_దత్తాంశం.అంశం = str;

    do_choice(0, దత్తాంశం, &fake_దత్తాంశం, ENCODING);
}

    స్థిర శూన్య
name_callback(Widget w,
	Shared_Font_Sel_దత్తాంశం_2 *దత్తాంశం,
	XmListCallbackStruct *call_దత్తాంశం)
{
    do_choice(w, దత్తాంశం, call_దత్తాంశం, NAME);
}

    స్థిర శూన్య
style_callback(Widget w,
	Shared_Font_Sel_దత్తాంశం_2 *దత్తాంశం,
	XmListCallbackStruct *call_దత్తాంశం)
{
    do_choice(w, దత్తాంశం, call_దత్తాంశం, STYLE);
}

    స్థిర శూన్య
కొలత_callback(Widget w,
	Shared_Font_Sel_దత్తాంశం_2 *దత్తాంశం,
	XmListCallbackStruct *call_దత్తాంశం)
{
    do_choice(w, దత్తాంశం, call_దత్తాంశం, కొలత_3);
}

    స్థిర శూన్య
cancel_callback(Widget w వాడని,
	Shared_Font_Sel_దత్తాంశం_2 *దత్తాంశం,
	XmListCallbackStruct *call_దత్తాంశం వాడని)
{
    ఒకవేళ (దత్తాంశం->sel[ENCODING])
    {
	XtFree(దత్తాంశం->sel[ENCODING]);
	దత్తాంశం->sel[ENCODING] = ఖాళీ;
    }
    ఒకవేళ (దత్తాంశం->sel[NAME])
    {
	XtFree(దత్తాంశం->sel[NAME]);
	దత్తాంశం->sel[NAME] = ఖాళీ;
    }
    ఒకవేళ (దత్తాంశం->sel[STYLE])
    {
	XtFree(దత్తాంశం->sel[STYLE]);
	దత్తాంశం->sel[STYLE] = ఖాళీ;
    }
    ఒకవేళ (దత్తాంశం->sel[కొలత_3])
    {
	XtFree(దత్తాంశం->sel[కొలత_3]);
	దత్తాంశం->sel[కొలత_3] = ఖాళీ;
    }

    ఒకవేళ (దత్తాంశం->font_name)
	XtFree(దత్తాంశం->font_name);
    దత్తాంశం->font_name = ఖాళీ;

    దత్తాంశం->అంకె = 0;
    XFree_Font_పేర్లు(దత్తాంశం->పేర్లు);
    దత్తాంశం->పేర్లు = ఖాళీ;
    దత్తాంశం->నిష్క్రమించు = True;
}

    స్థిర శూన్య
ok_callback(Widget w వాడని,
	Shared_Font_Sel_దత్తాంశం_2 *దత్తాంశం,
	Xm_నెట్టు_Button_Callback_Struct *call_దత్తాంశం వాడని)
{
    అక్షరం    *నమూనా;
    అక్షరం    **name;
    పూర్ణాంకం	    i;

    నమూనా = Xm_పాఠ్యం_Get_String(దత్తాంశం->name);
    name    = XListFonts(XtDisplay(దత్తాంశం->dialog), నమూనా, 1, &i);
    XtFree(నమూనా);

    ఒకవేళ (i != 1)
    {
	do_dialog(VIM_ERROR,
		(char_u *)_("Error"),
		(char_u *)_("Invalid font specification"),
		(char_u *)_("&Dismiss"), 1, ఖాళీ, FALSE);
	XFree_Font_పేర్లు(name);
    }
    లేకపోతే
    {
	ఒకవేళ (దత్తాంశం->font_name)
	    XtFree(దత్తాంశం->font_name);
	దత్తాంశం->font_name = XtNewString(name[0]);

	ఒకవేళ (దత్తాంశం->sel[ENCODING])
	{
	    XtFree(దత్తాంశం->sel[ENCODING]);
	    దత్తాంశం->sel[ENCODING] = ఖాళీ;
	}
	ఒకవేళ (దత్తాంశం->sel[NAME])
	{
	    XtFree(దత్తాంశం->sel[NAME]);
	    దత్తాంశం->sel[NAME] = ఖాళీ;
	}
	ఒకవేళ (దత్తాంశం->sel[STYLE])
	{
	    XtFree(దత్తాంశం->sel[STYLE]);
	    దత్తాంశం->sel[STYLE] = ఖాళీ;
	}
	ఒకవేళ (దత్తాంశం->sel[కొలత_3])
	{
	    XtFree(దత్తాంశం->sel[కొలత_3]);
	    దత్తాంశం->sel[కొలత_3] = ఖాళీ;
	}

	XFree_Font_పేర్లు(name);

	దత్తాంశం->అంకె = 0;
	XFree_Font_పేర్లు(దత్తాంశం->పేర్లు);
	దత్తాంశం->పేర్లు = ఖాళీ;
	దత్తాంశం->నిష్క్రమించు = True;
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
    స్థిర Shared_Font_Sel_దత్తాంశం_2	దత్తాంశం_3;

    Widget		మాతృ;
    Widget		form;
    Widget		separator;
    Widget		sub_form;
    Widget		కొలత_toggle;
    Widget		name;
    Widget		disp_చట్రం;
    Widget		చట్రం;
    Arg			వాదనలు[64];
    పూర్ణాంకం			n;
    XmString		str;
    అక్షరం		big_font[MAX_FONT_NAME_LEN];
    Shared_Font_Sel_దత్తాంశం_2	*దత్తాంశం;

    దత్తాంశం = &దత్తాంశం_3;

    మాతృ = vimShell;
    దత్తాంశం->పేర్లు = XListFonts(XtDisplay(మాతృ), "-*-*-*-*-*-*-*-*-*-*-*-*-*-*",
						       MAX_FONTS, &దత్తాంశం->అంకె);

    /*
     * Find the name of the biggest font less than the given limit
     * MAX_DISPLAY_SIZE used to set up the initial height of the display
     * widget.
     */

    {
	పూర్ణాంకం	i;
	పూర్ణాంకం	max;
	పూర్ణాంకం	idx = 0;
	పూర్ణాంకం	కొలత;
	అక్షరం	buf[128];

	కొరకు (i = 0, max = 0; i < దత్తాంశం->అంకె; i++)
	{
	    get_part(fn(దత్తాంశం, i), 7, buf);
	    కొలత = atoi(buf);
	    ఒకవేళ ((కొలత > max) && (కొలత < MAX_DISPLAY_కొలత))
	    {
		idx = i;
		max = కొలత;
	    }
	}
	strcpy(big_font, fn(దత్తాంశం, idx));
    }
    దత్తాంశం->old = XLoadQueryFont(XtDisplay(మాతృ), big_font);
    దత్తాంశం->old_list = gui_motif_create_fontlist(దత్తాంశం->old);

    // Set the title of the Dialog window.
    దత్తాంశం->dialog = XmCreateDialogShell(మాతృ, "fontSelector", ఖాళీ, 0);
    str = XmStringCreateLocalized(_("Vim - Font Selector"));

    // Create form popup dialog widget.
    form = XtVaCreateWidget("form",
	    xmFormWidgetClass, దత్తాంశం->dialog,
	    XmNdialogTitle, str,
	    XmNautoUnmanage, False,
	    XmNdialogStyle, Xm_DIALOG_నిండిన_APPLICATION_MODAL,
	    ఖాళీ);
    XmStringFree(str);

    sub_form = XtVaCreateManagedWidget("subForm",
	    xmFormWidgetClass, form,
	    XmNbottomAttachment, XmATTACH_FORM,
	    Xm_Nbottom_స్థానభ్రంశం, 4,
	    XmNrightAttachment, XmATTACH_FORM,
	    Xm_Nright_స్థానభ్రంశం, 4,
	    XmNtopAttachment, XmATTACH_FORM,
	    Xm_Ntop_స్థానభ్రంశం, 4,
	    XmNorientation, XmVERTICAL,
	    ఖాళీ);

    దత్తాంశం->ok = XtVaCreateManagedWidget(_("OK"),
	    xm_నెట్టు_Button_Gadget_Class, sub_form,
	    XmNleftAttachment, XmATTACH_FORM,
	    XmNrightAttachment, XmATTACH_FORM,
	    XmNtopAttachment, XmATTACH_FORM,
	    Xm_Ntop_స్థానభ్రంశం, 4,
	    ఖాళీ);
    apply_fontlist(దత్తాంశం->ok);

    దత్తాంశం->cancel = XtVaCreateManagedWidget(_("Cancel"),
	    xm_నెట్టు_Button_Gadget_Class, sub_form,
	    XmNrightAttachment, XmATTACH_FORM,
	    XmNleftAttachment, XmATTACH_FORM,
	    XmNtopAttachment, XmATTACH_WIDGET,
	    XmNtopWidget, దత్తాంశం->ok,
	    Xm_Ntop_స్థానభ్రంశం, 4,
	    XmNshowAsDefault, True,
	    ఖాళీ);
    apply_fontlist(దత్తాంశం->cancel);

    // Create the separator for beauty.
    n = 0;
    Xt_సమితి_Arg(వాదనలు[n], XmNorientation, XmVERTICAL); n++;
    Xt_సమితి_Arg(వాదనలు[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    Xt_సమితి_Arg(వాదనలు[n], XmNtopAttachment, XmATTACH_FORM); n++;
    Xt_సమితి_Arg(వాదనలు[n], XmNrightAttachment, XmATTACH_WIDGET); n++;
    Xt_సమితి_Arg(వాదనలు[n], XmNrightWidget, sub_form); n++;
    Xt_సమితి_Arg(వాదనలు[n], Xm_Nright_స్థానభ్రంశం, 4); n++;
    separator = XmCreateSeparatorGadget(form, "separator", వాదనలు, n);
    Xt_Manage_బిడ్డ(separator);

    // Create font name text widget and the corresponding label.
    దత్తాంశం->name = XtVaCreateManagedWidget("fontName",
	    xm_పాఠ్యం_Widget_Class, form,
	    XmNbottomAttachment, XmATTACH_FORM,
	    Xm_Nbottom_స్థానభ్రంశం, 4,
	    XmNleftAttachment, XmATTACH_FORM,
	    Xm_Nleft_స్థానభ్రంశం, 4,
	    XmNrightAttachment, XmATTACH_WIDGET,
	    XmNrightWidget, separator,
	    Xm_Nright_స్థానభ్రంశం, 4,
	    XmNeditable, False,
	    Xm_Nedit_బహుళకం, Xm_SINGLE_పంక్తి_EDIT,
	    Xm_Nmax_నిడివి, MAX_FONT_NAME_LEN,
	    XmNcolumns, 60,
	    ఖాళీ);

    str = XmStringCreateLocalized(_("Name:"));
    name = XtVaCreateManagedWidget("fontNameLabel",
	    xmLabelGadgetClass, form,
	    XmNlabelString, str,
	    Xm_Nuser_దత్తాంశం, దత్తాంశం->name,
	    XmNleftAttachment, XmATTACH_OPPOSITE_WIDGET,
	    XmNleftWidget, దత్తాంశం->name,
	    XmNbottomAttachment, XmATTACH_WIDGET,
	    XmNbottomWidget, దత్తాంశం->name,
	    Xm_Ntop_స్థానభ్రంశం, 1,
	    ఖాళీ);
    XmStringFree(str);
    apply_fontlist(name);

    // create sample display label widget
    disp_చట్రం = XtVaCreateManagedWidget("sampleFrame",
	    xm_చట్రం_Widget_Class, form,
	    XmNshadowType, Xm_SHADOW_ETCHED_లోపల,
	    XmNleftAttachment, XmATTACH_FORM,
	    Xm_Nleft_స్థానభ్రంశం, 4,
	    XmNbottomAttachment, XmATTACH_WIDGET,
	    XmNbottomWidget, name,
	    XmNrightAttachment, XmATTACH_WIDGET,
	    XmNrightWidget, separator,
	    Xm_Nright_స్థానభ్రంశం, 4,
	    XmNalignment, XmALIGNMENT_BEGINNING,
	    ఖాళీ);

    దత్తాంశం->sample = XtVaCreateManagedWidget("sampleLabel",
	    xmLabelWidgetClass, disp_చట్రం,
	    XmNleftAttachment, XmATTACH_FORM,
	    XmNtopAttachment, XmATTACH_FORM,
	    XmNbottomAttachment, XmATTACH_FORM,
	    XmNrightAttachment, XmATTACH_FORM,
	    XmNalignment, XmALIGNMENT_BEGINNING,
	    Xm_Nrecompute_కొలత, False,
	    XmNfontList, దత్తాంశం->old_list,
	    ఖాళీ);

    // create toggle button
    str = XmStringCreateLocalized(_("Show size in Points"));
    కొలత_toggle = XtVaCreateManagedWidget("sizeToggle",
	    xmToggleButtonGadgetClass, form,
	    XmNlabelString, str,
	    XmNleftAttachment, XmATTACH_FORM,
	    Xm_Nleft_స్థానభ్రంశం, 4,
	    XmNbottomAttachment, XmATTACH_WIDGET,
	    XmNbottomWidget, disp_చట్రం,
	    Xm_Nbottom_స్థానభ్రంశం, 4,
	    ఖాళీ);
    XmStringFree(str);
    apply_fontlist(కొలత_toggle);
    Xt_Manage_బిడ్డ(కొలత_toggle);

    // Encoding pulldown menu.

    దత్తాంశం->encoding_pulldown = XmCreatePulldownMenu(form,
						 "encodingPulldown", ఖాళీ, 0);
    str = XmStringCreateLocalized(_("Encoding:"));
    n = 0;
    Xt_సమితి_Arg(వాదనలు[n], XmNsubMenuId, దత్తాంశం->encoding_pulldown); ++n;
    Xt_సమితి_Arg(వాదనలు[n], XmNlabelString, str); ++n;
    Xt_సమితి_Arg(వాదనలు[n], XmNleftAttachment, XmATTACH_FORM); ++n;
    Xt_సమితి_Arg(వాదనలు[n], Xm_Nleft_స్థానభ్రంశం, 4); ++n;
    Xt_సమితి_Arg(వాదనలు[n], XmNbottomAttachment, XmATTACH_WIDGET); ++n;
    Xt_సమితి_Arg(వాదనలు[n], XmNbottomWidget, కొలత_toggle); ++n;
    Xt_సమితి_Arg(వాదనలు[n], Xm_Nbottom_స్థానభ్రంశం, 4); ++n;
    Xt_సమితి_Arg(వాదనలు[n], XmNrightAttachment, XmATTACH_WIDGET); ++n;
    Xt_సమితి_Arg(వాదనలు[n], XmNrightWidget, separator); ++n;
    Xt_సమితి_Arg(వాదనలు[n], Xm_Nright_స్థానభ్రంశం, 4); ++n;
    దత్తాంశం->encoding_menu = XmCreateOptionMenu(form, "encodingMenu", వాదనలు, n);
    XmStringFree(str);
    Xm_కూడు_Tab_సమూహం(దత్తాంశం->encoding_menu);

    /*
     * Create scroll list widgets in a separate subform used to manage the
     * different sizes of the lists.
     */

    sub_form = XtVaCreateManagedWidget("subForm",
	    xmFormWidgetClass, form,
	    XmNbottomAttachment, XmATTACH_WIDGET,
	    XmNbottomWidget, దత్తాంశం->encoding_menu,
	    Xm_Nbottom_స్థానభ్రంశం, 4,
	    XmNleftAttachment, XmATTACH_FORM,
	    Xm_Nleft_స్థానభ్రంశం, 4,
	    XmNrightAttachment, XmATTACH_WIDGET,
	    XmNrightWidget, separator,
	    Xm_Nright_స్థానభ్రంశం, 4,
	    XmNtopAttachment, XmATTACH_FORM,
	    Xm_Ntop_స్థానభ్రంశం, 2,
	    XmNorientation, XmVERTICAL,
	    ఖాళీ);

    // font list
    చట్రం = XtVaCreateManagedWidget("frame", xm_చట్రం_Widget_Class, sub_form,
	    XmNshadowThickness, 0,
	    XmNtopAttachment, XmATTACH_FORM,
	    XmNbottomAttachment, XmATTACH_FORM,
	    XmNleftAttachment, XmATTACH_FORM,
	    XmNrightAttachment, XmATTACH_POSITION,
	    XmNrightPosition, 50,
	    ఖాళీ);

    str = XmStringCreateLocalized(_("Font:"));
    name = XtVaCreateManagedWidget("nameListLabel", xmLabelGadgetClass, చట్రం,
	    XmNchildType, Xm_చట్రం_TITLE_బిడ్డ,
	    XmNchildVerticalAlignment, XmALIGNMENT_CENTER,
	    XmNchildHorizontalAlignment, XmALIGNMENT_BEGINNING,
	    XmNlabelString, str,
	    ఖాళీ);
    XmStringFree(str);
    apply_fontlist(name);

    n = 0;
    Xt_సమితి_Arg(వాదనలు[n], Xm_Nvisible_అంశం_లెక్క, 8); ++n;
    Xt_సమితి_Arg(వాదనలు[n], XmNresizable, True); ++n;
    Xt_సమితి_Arg(వాదనలు[n], Xm_Nlist_కొలత_Policy, XmCONSTANT); ++n;
    Xt_సమితి_Arg(వాదనలు[n], XmNvisualPolicy, XmVARIABLE); ++n;
#నిర్వచితమైతే LESSTIF_సంస్కరణ
    Xt_సమితి_Arg(వాదనలు[n], XmNscrollBarDisplayPolicy, XmSTATIC); ++n;
#అయితే_ముగింపు
    దత్తాంశం->list[NAME] = XmCreateScrolledList(చట్రం, "fontList", వాదనలు, n);
    Xt_Va_సమితి_విలువలు(name, Xm_Nuser_దత్తాంశం, దత్తాంశం->list[NAME], ఖాళీ);

    // style list
    చట్రం = XtVaCreateManagedWidget("frame", xm_చట్రం_Widget_Class, sub_form,
	    XmNshadowThickness, 0,
	    XmNtopAttachment, XmATTACH_FORM,
	    XmNbottomAttachment, XmATTACH_FORM,
	    XmNleftAttachment, XmATTACH_POSITION,
	    XmNleftPosition, 50,
	    Xm_Nleft_స్థానభ్రంశం, 4,
	    XmNrightAttachment, XmATTACH_POSITION,
	    XmNrightPosition, 80,
	    ఖాళీ);

    str = XmStringCreateLocalized(_("Style:"));
    name = XtVaCreateManagedWidget("styleListLabel", xmLabelWidgetClass, చట్రం,
	    XmNchildType, Xm_చట్రం_TITLE_బిడ్డ,
	    XmNchildVerticalAlignment, XmALIGNMENT_CENTER,
	    XmNchildHorizontalAlignment, XmALIGNMENT_BEGINNING,
	    XmNlabelString, str,
	    ఖాళీ);
    XmStringFree(str);
    apply_fontlist(name);

    n = 0;
    Xt_సమితి_Arg(వాదనలు[n], Xm_Nvisible_అంశం_లెక్క, 8); ++n;
    Xt_సమితి_Arg(వాదనలు[n], XmNresizable, True); ++n;
    Xt_సమితి_Arg(వాదనలు[n], Xm_Nlist_కొలత_Policy, XmCONSTANT); ++n;
    Xt_సమితి_Arg(వాదనలు[n], XmNvisualPolicy, XmVARIABLE); ++n;
#నిర్వచితమైతే LESSTIF_సంస్కరణ
    Xt_సమితి_Arg(వాదనలు[n], XmNscrollBarDisplayPolicy, XmSTATIC); ++n;
#అయితే_ముగింపు
    దత్తాంశం->list[STYLE] = XmCreateScrolledList(చట్రం, "styleList", వాదనలు, n);
    Xt_Va_సమితి_విలువలు(name, Xm_Nuser_దత్తాంశం, దత్తాంశం->list[STYLE], ఖాళీ);

    // size list
    చట్రం = XtVaCreateManagedWidget("frame", xm_చట్రం_Widget_Class, sub_form,
	    XmNshadowThickness, 0,
	    XmNtopAttachment, XmATTACH_FORM,
	    XmNbottomAttachment, XmATTACH_FORM,
	    XmNleftAttachment, XmATTACH_POSITION,
	    XmNleftPosition, 80,
	    Xm_Nleft_స్థానభ్రంశం, 4,
	    XmNrightAttachment, XmATTACH_FORM,
	    ఖాళీ);

    str = XmStringCreateLocalized(_("Size:"));
    name = XtVaCreateManagedWidget("sizeListLabel", xmLabelGadgetClass, చట్రం,
	    XmNchildType, Xm_చట్రం_TITLE_బిడ్డ,
	    XmNchildVerticalAlignment, XmALIGNMENT_CENTER,
	    XmNchildHorizontalAlignment, XmALIGNMENT_BEGINNING,
	    XmNlabelString, str,
	    ఖాళీ);
    XmStringFree(str);
    apply_fontlist(name);

    n = 0;
    Xt_సమితి_Arg(వాదనలు[n], Xm_Nvisible_అంశం_లెక్క, 8); ++n;
    Xt_సమితి_Arg(వాదనలు[n], XmNresizable, True); ++n;
    Xt_సమితి_Arg(వాదనలు[n], Xm_Nlist_కొలత_Policy, XmCONSTANT); ++n;
    Xt_సమితి_Arg(వాదనలు[n], XmNvisualPolicy, XmVARIABLE); ++n;
#నిర్వచితమైతే LESSTIF_సంస్కరణ
    Xt_సమితి_Arg(వాదనలు[n], XmNscrollBarDisplayPolicy, XmSTATIC); ++n;
#అయితే_ముగింపు
    దత్తాంశం->list[కొలత_3] = XmCreateScrolledList(చట్రం, "sizeList", వాదనలు, n);
    Xt_Va_సమితి_విలువలు(name, Xm_Nuser_దత్తాంశం, దత్తాంశం->list[కొలత_3], ఖాళీ);

    // update form widgets cancel button
    Xt_Va_సమితి_విలువలు(form, XmNcancelButton, దత్తాంశం->cancel, ఖాళీ);

    Xt_కూడు_Callback(కొలత_toggle, Xm_Nvalue_మారింది_Callback,
	    (XtCallbackProc)stoggle_callback, (Xt_సూచిక)దత్తాంశం);
    Xt_కూడు_Callback(దత్తాంశం->list[NAME], XmNbrowseSelectionCallback,
	    (XtCallbackProc)name_callback, (Xt_సూచిక)దత్తాంశం);
    Xt_కూడు_Callback(దత్తాంశం->list[STYLE], XmNbrowseSelectionCallback,
	    (XtCallbackProc)style_callback, (Xt_సూచిక)దత్తాంశం);
    Xt_కూడు_Callback(దత్తాంశం->list[కొలత_3], XmNbrowseSelectionCallback,
	    (XtCallbackProc)కొలత_callback, (Xt_సూచిక)దత్తాంశం);
    Xt_కూడు_Callback(దత్తాంశం->ok, XmNactivateCallback,
	    (XtCallbackProc)ok_callback, (Xt_సూచిక)దత్తాంశం);
    Xt_కూడు_Callback(దత్తాంశం->cancel, XmNactivateCallback,
	    (XtCallbackProc)cancel_callback, (Xt_సూచిక)దత్తాంశం);

    XmProcessTraversal(దత్తాంశం->list[NAME], XmTRAVERSE_CURRENT);

    // setup tabgroups

    Xm_కూడు_Tab_సమూహం(దత్తాంశం->list[NAME]);
    Xm_కూడు_Tab_సమూహం(దత్తాంశం->list[STYLE]);
    Xm_కూడు_Tab_సమూహం(దత్తాంశం->list[కొలత_3]);
    Xm_కూడు_Tab_సమూహం(కొలత_toggle);
    Xm_కూడు_Tab_సమూహం(దత్తాంశం->name);
    Xm_కూడు_Tab_సమూహం(దత్తాంశం->ok);
    Xm_కూడు_Tab_సమూహం(దత్తాంశం->cancel);

    కూడు_cancel_action(దత్తాంశం->dialog, (XtCallbackProc)cancel_callback, దత్తాంశం);

    // Preset selection data.

    దత్తాంశం->నిష్క్రమించు = False;
    దత్తాంశం->లోపల_pixels= True;
    దత్తాంశం->sel[ENCODING] = ఖాళీ;
    దత్తాంశం->sel[NAME] = ఖాళీ;
    దత్తాంశం->sel[STYLE] = ఖాళీ;
    దత్తాంశం->sel[కొలత_3] = ఖాళీ;
    దత్తాంశం->font_name = ఖాళీ;

    // set up current font parameters
    ఒకవేళ (current && current[0] != '\0')
    {
	పూర్ణాంకం	    i;
	అక్షరం	    **పేర్లు;

	పేర్లు = XListFonts(XtDisplay(form), (అక్షరం *) current, 1, &i);

	ఒకవేళ (i != 0)
	{
	    అక్షరం namebuf[తాత్కాలిక_BUF_కొలత];
	    అక్షరం stylebuf[తాత్కాలిక_BUF_కొలత];
	    అక్షరం sizebuf[తాత్కాలిక_BUF_కొలత];
	    అక్షరం encodingbuf[తాత్కాలిక_BUF_కొలత];
	    అక్షరం *దొరికింది;

	    దొరికింది = పేర్లు[0];

	    name_part(దొరికింది, namebuf);
	    style_part(దొరికింది, stylebuf);
	    కొలత_part(దొరికింది, sizebuf, దత్తాంశం->లోపల_pixels);
	    encoding_part(దొరికింది, encodingbuf);

	    ఒకవేళ (*namebuf != NUL
		    && *stylebuf != NUL
		    && *sizebuf != NUL
		    && *encodingbuf != NUL)
	    {
		దత్తాంశం->sel[NAME] = XtNewString(namebuf);
		దత్తాంశం->sel[STYLE] = XtNewString(stylebuf);
		దత్తాంశం->sel[కొలత_3] = XtNewString(sizebuf);
		దత్తాంశం->sel[ENCODING] = XtNewString(encodingbuf);
		దత్తాంశం->font_name = XtNewString(పేర్లు[0]);
		display_sample(దత్తాంశం);
		Xm_పాఠ్యం_సమితి_String(దత్తాంశం->name, దత్తాంశం->font_name);
	    }
	    లేకపోతే
	    {
		// We can't preset a symbolic name, which isn't a full font
		// description. Therefore we just behave the same way as if the
		// user didn't have selected anything thus far.
		//
		// Unfortunately there is no known way to expand an abbreviated
		// font name.
		దత్తాంశం->font_name = ఖాళీ;
	    }
	}
	XFree_Font_పేర్లు(పేర్లు);
    }

    fill_lists(NONE, దత్తాంశం);

    // Unfortunately LessTif doesn't align the list widget's properly.  I don't
    // have currently any idea how to fix this problem.
    Xt_Manage_బిడ్డ(దత్తాంశం->list[NAME]);
    Xt_Manage_బిడ్డ(దత్తాంశం->list[STYLE]);
    Xt_Manage_బిడ్డ(దత్తాంశం->list[కొలత_3]);
    Xt_Manage_బిడ్డ(దత్తాంశం->encoding_menu);
    manage_centered(form);

    // modal event loop
    అయితే (!దత్తాంశం->నిష్క్రమించు)
	XtAppProcessEvent(Xt_Widget_గమ్యం_Application_Context(దత్తాంశం->dialog),
							(Xt_ఆదానం_ముసుగు)XtIMAll);

    ఒకవేళ (దత్తాంశం->old)
    {
	XFreeFont(XtDisplay(దత్తాంశం->dialog),  దత్తాంశం->old);
	XmFontListFree(దత్తాంశం->old_list);
    }
    XtDestroyWidget(దత్తాంశం->dialog);

    gui_motif_synch_fonts();

    తిరిగి (char_u *) దత్తాంశం->font_name;
}
