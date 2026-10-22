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

#ਅਨ੍ਤਰ੍ਭਾਵ "vim.h"

#ਅਨ੍ਤਰ੍ਭਾਵ <Xm/Form.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <Xm/PushBG.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <Xm/Text.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <Xm/TextF.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <Xm/Label.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <Xm/Frame.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <Xm/LabelG.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <Xm/ToggleBG.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <Xm/SeparatoG.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <Xm/DialogS.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <Xm/List.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <Xm/RowColumn.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <Xm/AtomMgr.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <Xm/Protocols.h>

#ਅਨ੍ਤਰ੍ਭਾਵ <X11/keysym.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <X11/Xatom.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <X11/StringDefs.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <X11/Intrinsic.h>

ਬਾਹਰੀ Widget vimShell;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MENU
# define apply_fontlist(w) gui_motif_menu_fontlist(w)
#ਨਹੀਂਤਾਂ
# define apply_fontlist(w)
#ਜੇ_ਅੰਤ

/////////////////////////////////////////////////////////////////////////////
// Font selection dialogue implementation.

ਸਥਿਰ ਅੱਖਰ wild[3] = "*";

/*
 * FIXME: This is a generic function, which should be used throughout the whole
 * application.
 *
 * Add close_callback, which will be called when the user selects close from
 * the window menu.  The close menu item usually activates f.kill which sends a
 * WM_DELETE_WINDOW protocol request for the window.
 */

    ਸਥਿਰ ਖਾਲੀ
ਜੋੜੋ_cancel_action(Widget shell, XtCallbackProc close_callback, ਖਾਲੀ *arg)
{
    ਸਥਿਰ Atom wmp_atom = 0;
    ਸਥਿਰ Atom dw_atom = 0;
    Display *display = XtDisplay(shell);

    // deactivate the built-in delete response of killing the application
    Xt_Va_ਸੈੱਟ_ਮੁੱਲਾਂ(shell, XmNdeleteResponse, XmDO_NOTHING, ਨਿਰਾ);

    // add a delete window protocol callback instead
    ਜੇ (!dw_atom)
    {
	wmp_atom = XmInternAtom(display, "WM_PROTOCOLS", True);
	dw_atom = XmInternAtom(display, "WM_DELETE_WINDOW", True);
    }
    Xm_ਜੋੜੋ_Protocol_Callback(shell, wmp_atom, dw_atom, close_callback, arg);
}

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MAX_FONTS			65535
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MAX_FONT_NAME_LEN		256
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MAX_ENTRIES_ਵਿੱਚ_LIST		5000
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MAX_DISPLAY_ਮਾਪ		150
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਆਰਜ਼ੀ_BUF_ਮਾਪ			256

ਗਿਣਤੀ ListSpecifier
{
    ENCODING,
    NAME,
    STYLE,
    ਮਾਪ_3,
    NONE
};

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ Shared_Font_Sel_ਅੰਕੜੇ
{
    Widget	dialog;
    Widget	ok;
    Widget	cancel;
    Widget	encoding_pulldown;
    Widget	encoding_menu;
    Widget	list[NONE];
    Widget	name;
    Widget	sample;
    ਅੱਖਰ	**ਨਾਂ;	// font name array of arrays
    ਪੂਰਨ		ਅੰਕ;		// number of font names
    String	sel[NONE];	// selection category
    Boolean	ਵਿੱਚ_pixels;	// toggle state - size in pixels
    ਅੱਖਰ	*font_name;	// current font name
    XFontStruct	*old;		// font data structure for sample display
    XmFontList	old_list;	// font data structure for sample display
    Boolean	ਬਾਹਰ_ਨਿਕਲੋ;		// used for program exit control
} Shared_Font_Sel_ਅੰਕੜੇ_2;

/*
 * Checking access to the font name array for validity.
 */
    ਸਥਿਰ ਅੱਖਰ *
fn(Shared_Font_Sel_ਅੰਕੜੇ_2 *ਅੰਕੜੇ, ਪੂਰਨ i)
{
    // Assertion checks:
    ਜੇ (ਅੰਕੜੇ->ਅੰਕ < 0)
	abort();
    ਜੇ (i >= ਅੰਕੜੇ->ਅੰਕ)
	i = ਅੰਕੜੇ->ਅੰਕ - 1;
    ਜੇ (i < 0)
	i = 0;

    ਵਾਪਸ ਅੰਕੜੇ->ਨਾਂ[i];
}

/*
 * Get a specific substring from a font name.
 */
    ਸਥਿਰ ਖਾਲੀ
get_part(ਅੱਖਰ *ਵਿੱਚ, ਪੂਰਨ pos, ਅੱਖਰ *ਬਾਹਰਲਾ)
{
    ਪੂਰਨ	i;
    ਪੂਰਨ j;

    *ਬਾਹਰਲਾ = '\0';

    ਲਈ (i = 0; (pos > 0) && (ਵਿੱਚ[i] != '\0'); ++i)
	ਜੇ (ਵਿੱਚ[i] == '-')
	    pos--;

    ਜੇ (ਵਿੱਚ[i] == '\0')
	ਵਾਪਸ;

    ਲਈ (j = 0; (ਵਿੱਚ[i] != '-') && (ਵਿੱਚ[i] != '\0'); ++i, ++j)
	ਬਾਹਰਲਾ[j] = ਵਿੱਚ[i];
    ਬਾਹਰਲਾ[j] = '\0';
}

/*
 * Given a font name this function returns the part used in the first
 * scroll list.
 */
    ਸਥਿਰ ਖਾਲੀ
name_part(ਅੱਖਰ *font, ਅੱਖਰ *buf)
{
    ਅੱਖਰ    buf2[ਆਰਜ਼ੀ_BUF_ਮਾਪ];
    ਅੱਖਰ    buf3[ਆਰਜ਼ੀ_BUF_ਮਾਪ];

    get_part(font, 2, buf2);
    get_part(font, 1, buf3);

    ਜੇ (*buf3 != NUL)
	vim_snprintf(buf, ਆਰਜ਼ੀ_BUF_ਮਾਪ, "%s (%s)", buf2, buf3);
    ਨਹੀਂਤਾਂ
	vim_snprintf(buf, ਆਰਜ਼ੀ_BUF_ਮਾਪ, "%s", buf2);
}

/*
 * Given a font name this function returns the part used in the second scroll list.
 */
    ਸਥਿਰ ਖਾਲੀ
style_part(ਅੱਖਰ *font, ਅੱਖਰ *buf)
{
    ਅੱਖਰ    buf2[ਆਰਜ਼ੀ_BUF_ਮਾਪ];
    ਅੱਖਰ    buf3[ਆਰਜ਼ੀ_BUF_ਮਾਪ];

    get_part(font, 3, buf3);
    get_part(font, 5, buf2);

    ਜੇ (!strcmp(buf2, "normal") && !strcmp(buf2, "Normal")
						   && !strcmp(buf2, "NORMAL"))
	vim_snprintf(buf, ਆਰਜ਼ੀ_BUF_ਮਾਪ, "%s %s", buf3, buf2);
    ਨਹੀਂਤਾਂ
	strcpy(buf, buf3);

    get_part(font, 6, buf2);

    ਜੇ (buf2[0] != '\0')
	vim_snprintf(buf3, ਆਰਜ਼ੀ_BUF_ਮਾਪ, "%s %s", buf, buf2);
    ਨਹੀਂਤਾਂ
	strcpy(buf3, buf);

    get_part(font, 4, buf2);

    ਜੇ (!strcmp(buf2, "o") || !strcmp(buf2, "O"))
	vim_snprintf(buf, ਆਰਜ਼ੀ_BUF_ਮਾਪ, "%s oblique", buf3);
    ਨਹੀਂਤਾਂ ਜੇ (!strcmp(buf2, "i") || !strcmp(buf2, "I"))
	vim_snprintf(buf, ਆਰਜ਼ੀ_BUF_ਮਾਪ, "%s italic", buf3);

    ਜੇ (!strcmp(buf, " "))
	strcpy(buf, "-");
}

/*
 * Given a font name this function returns the part used in the third
 * scroll list.
 */
    ਸਥਿਰ ਖਾਲੀ
ਮਾਪ_part(ਅੱਖਰ *font, ਅੱਖਰ *buf, ਪੂਰਨ ਵਿੱਚ_Pixels)
{
    ਪੂਰਨ	    ਮਾਪ;
    ਦਸ਼ਮਲਵ   ਆਰਜ਼ੀ;

    *buf = '\0';

    ਜੇ (ਵਿੱਚ_Pixels)
    {
	get_part(font, 7, buf);
	ਜੇ (*buf != NUL)
	{
	    ਮਾਪ = atoi(buf);
	    sprintf(buf, "%3d", ਮਾਪ);
	}
    }
    ਨਹੀਂਤਾਂ
    {
	get_part(font, 8, buf);
	ਜੇ (*buf != NUL)
	{
	    ਮਾਪ = atoi(buf);
	    ਆਰਜ਼ੀ = (ਦਸ਼ਮਲਵ)ਮਾਪ / 10.0;
	    ਮਾਪ = ਆਰਜ਼ੀ;
	    ਜੇ (buf[ਲੜੀ_ਲੰਬਾਈ(buf) - 1] == '0')
		sprintf(buf, "%3d", ਮਾਪ);
	    ਨਹੀਂਤਾਂ
		sprintf(buf, "%4.1f", ਆਰਜ਼ੀ);
	}
    }
}

/*
 * Given a font name this function returns the part used in the choice menu.
 */
    ਸਥਿਰ ਖਾਲੀ
encoding_part(ਅੱਖਰ *font, ਅੱਖਰ *buf)
{
    ਅੱਖਰ    buf1[ਆਰਜ਼ੀ_BUF_ਮਾਪ];
    ਅੱਖਰ    buf2[ਆਰਜ਼ੀ_BUF_ਮਾਪ];

    *buf = '\0';

    get_part(font, 13, buf1);
    get_part(font, 14, buf2);

    ਜੇ (*buf1 != NUL && *buf2 != NUL)
	vim_snprintf(buf, ਆਰਜ਼ੀ_BUF_ਮਾਪ, "%s-%s", buf1, buf2);
    ਜੇ (!strcmp(buf, " "))
	strcpy(buf, "-");
}

/*
 * Inserts a string into correct sorted position in a list.
 */
    ਸਥਿਰ ਖਾਲੀ
ਜੋੜੋ_ਮੰਜ਼ਿਲ_list(ਅੱਖਰ **buf, ਅੱਖਰ *ਵਸਤੂ, ਪੂਰਨ *ਗਿਣਤੀ_2)
{
    ਪੂਰਨ	i;
    ਪੂਰਨ j;

    ਜੇ (*ਗਿਣਤੀ_2 == MAX_ENTRIES_ਵਿੱਚ_LIST)
	ਵਾਪਸ;

    // avoid duplication
    ਲਈ (i = 0; i < *ਗਿਣਤੀ_2; ++i)
    {
	ਜੇ (!strcmp(buf[i], ਵਸਤੂ))
	    ਵਾਪਸ;
    }

    // find order place, but make sure that wild card comes first
    ਜੇ (!strcmp(ਵਸਤੂ, wild))
	i = 0;
    ਨਹੀਂਤਾਂ
	ਲਈ (i = 0; i < *ਗਿਣਤੀ_2; ++i)
	    ਜੇ (strcmp(buf[i], ਵਸਤੂ) > 0 && strcmp(buf[i], wild))
		ਰੋਕੋ;

    // now insert the item
    ਲਈ (j = *ਗਿਣਤੀ_2; j > i; --j)
	buf[j] = buf[j-1];
    buf[i] = XtNewString(ਵਸਤੂ);

    ++(*ਗਿਣਤੀ_2);
}

/*
 * True if the font matches some field.
 */
    ਸਥਿਰ Boolean
ਮੇਲ(Shared_Font_Sel_ਅੰਕੜੇ_2 *ਅੰਕੜੇ, ਗਿਣਤੀ ListSpecifier l, ਪੂਰਨ i)
{
    ਅੱਖਰ buf[ਆਰਜ਼ੀ_BUF_ਮਾਪ];

    // An empty selection or a wild card matches anything.
    ਜੇ (!ਅੰਕੜੇ->sel[l] || !strcmp(ਅੰਕੜੇ->sel[l], wild))
	ਵਾਪਸ True;

    // chunk out the desired part...
    ਚੋਣ (l)
    {
	ਹਾਲਤ ENCODING:
	    encoding_part(fn(ਅੰਕੜੇ, i), buf);
	    ਰੋਕੋ;

	ਹਾਲਤ NAME:
	    name_part(fn(ਅੰਕੜੇ, i), buf);
	    ਰੋਕੋ;

	ਹਾਲਤ STYLE:
	    style_part(fn(ਅੰਕੜੇ, i), buf);
	    ਰੋਕੋ;

	ਹਾਲਤ ਮਾਪ_3:
	    ਮਾਪ_part(fn(ਅੰਕੜੇ, i), buf, ਅੰਕੜੇ->ਵਿੱਚ_pixels);
	    ਰੋਕੋ;
	ਮੂਲ:
	    ;
    }

    // ...and chew it now

    ਵਾਪਸ !strcmp(buf, ਅੰਕੜੇ->sel[l]);
}

    ਸਥਿਰ Boolean
proportional(ਅੱਖਰ *font)
{
    ਅੱਖਰ buf[ਆਰਜ਼ੀ_BUF_ਮਾਪ];

    get_part(font, 11, buf);

    ਵਾਪਸ !strcmp(buf, "p") || !strcmp(buf, "P");
}


ਸਥਿਰ ਖਾਲੀ encoding_callback(Widget w, Shared_Font_Sel_ਅੰਕੜੇ_2 *ਅੰਕੜੇ,
							     Xt_ਸੂਚਕ dummy);

/*
 * Parse through the fontlist data and set up the three scroll lists.  The fix
 * parameter can be used to exclude a list from any changes.  This is used for
 * updates after selections caused by the users actions.
 */
    ਸਥਿਰ ਖਾਲੀ
fill_lists(ਗਿਣਤੀ ListSpecifier fix, Shared_Font_Sel_ਅੰਕੜੇ_2 *ਅੰਕੜੇ)
{
    ਅੱਖਰ	*list[NONE][MAX_ENTRIES_ਵਿੱਚ_LIST];
    ਪੂਰਨ		ਗਿਣਤੀ_2[NONE];
    ਅੱਖਰ	buf[ਆਰਜ਼ੀ_BUF_ਮਾਪ];
    XmString	ਚੀਜ਼ਾਂ[MAX_ENTRIES_ਵਿੱਚ_LIST];
    ਪੂਰਨ		i;
    ਪੂਰਨ		idx;

    ਲਈ (idx = (ਪੂਰਨ)ENCODING; idx < (ਪੂਰਨ)NONE; ++idx)
	ਗਿਣਤੀ_2[idx] = 0;

    // First we insert the wild char into every single list.
    ਜੇ (fix != ENCODING)
	ਜੋੜੋ_ਮੰਜ਼ਿਲ_list(list[ENCODING], wild, &ਗਿਣਤੀ_2[ENCODING]);
    ਜੇ (fix != NAME)
	ਜੋੜੋ_ਮੰਜ਼ਿਲ_list(list[NAME], wild, &ਗਿਣਤੀ_2[NAME]);
    ਜੇ (fix != STYLE)
	ਜੋੜੋ_ਮੰਜ਼ਿਲ_list(list[STYLE], wild, &ਗਿਣਤੀ_2[STYLE]);
    ਜੇ (fix != ਮਾਪ_3)
	ਜੋੜੋ_ਮੰਜ਼ਿਲ_list(list[ਮਾਪ_3], wild, &ਗਿਣਤੀ_2[ਮਾਪ_3]);

    ਲਈ (i = 0; i < ਅੰਕੜੇ->ਅੰਕ && i < MAX_ENTRIES_ਵਿੱਚ_LIST; i++)
    {
	ਜੇ (proportional(fn(ਅੰਕੜੇ, i)))
	    ਜਾਰੀ;

	ਜੇ (fix != ENCODING
		&& ਮੇਲ(ਅੰਕੜੇ, NAME, i)
		&& ਮੇਲ(ਅੰਕੜੇ, STYLE, i)
		&& ਮੇਲ(ਅੰਕੜੇ, ਮਾਪ_3, i))
	{
	    encoding_part(fn(ਅੰਕੜੇ, i), buf);
	    ਜੋੜੋ_ਮੰਜ਼ਿਲ_list(list[ENCODING], buf, &ਗਿਣਤੀ_2[ENCODING]);
	}

	ਜੇ (fix != NAME
		&& ਮੇਲ(ਅੰਕੜੇ, ENCODING, i)
		&& ਮੇਲ(ਅੰਕੜੇ, STYLE, i)
		&& ਮੇਲ(ਅੰਕੜੇ, ਮਾਪ_3, i))
	{
	    name_part(fn(ਅੰਕੜੇ, i), buf);
	    ਜੋੜੋ_ਮੰਜ਼ਿਲ_list(list[NAME], buf, &ਗਿਣਤੀ_2[NAME]);
	}

	ਜੇ (fix != STYLE
		&& ਮੇਲ(ਅੰਕੜੇ, ENCODING, i)
		&& ਮੇਲ(ਅੰਕੜੇ, NAME, i)
		&& ਮੇਲ(ਅੰਕੜੇ, ਮਾਪ_3, i))
	{
	    style_part(fn(ਅੰਕੜੇ, i), buf);
	    ਜੋੜੋ_ਮੰਜ਼ਿਲ_list(list[STYLE], buf, &ਗਿਣਤੀ_2[STYLE]);
	}

	ਜੇ (fix != ਮਾਪ_3
		&& ਮੇਲ(ਅੰਕੜੇ, ENCODING, i)
		&& ਮੇਲ(ਅੰਕੜੇ, NAME, i)
		&& ਮੇਲ(ਅੰਕੜੇ, STYLE, i))
	{
	    ਮਾਪ_part(fn(ਅੰਕੜੇ, i), buf, ਅੰਕੜੇ->ਵਿੱਚ_pixels);
	    ਜੋੜੋ_ਮੰਜ਼ਿਲ_list(list[ਮਾਪ_3], buf, &ਗਿਣਤੀ_2[ਮਾਪ_3]);
	}
    }

    /*
     * And now do the preselection in all lists where there was one:
     */

    ਜੇ (fix != ENCODING)
    {
	Cardinal n_ਚੀਜ਼ਾਂ;
	WidgetList children;
	Widget selected_button = 0;

	// Get and update the current button list.
	Xt_Va_Get_ਮੁੱਲਾਂ(ਅੰਕੜੇ->encoding_pulldown,
		XmNchildren, &children,
		XmNnumChildren, &n_ਚੀਜ਼ਾਂ,
		ਨਿਰਾ);

	ਲਈ (i = 0; i < ਗਿਣਤੀ_2[ENCODING]; ++i)
	{
	    Widget button;

	    ਚੀਜ਼ਾਂ[i] = XmStringCreateLocalized(list[ENCODING][i]);

	    ਜੇ (i < (ਪੂਰਨ)n_ਚੀਜ਼ਾਂ)
	    {
		// recycle old button
		Xt_Va_ਸੈੱਟ_ਮੁੱਲਾਂ(children[i],
			XmNlabelString, ਚੀਜ਼ਾਂ[i],
			Xm_Nuser_ਅੰਕੜੇ, i,
			ਨਿਰਾ);
		button = children[i];
	    }
	    ਨਹੀਂਤਾਂ
	    {
		// create a new button
		button = XtVaCreateManagedWidget("button",
			xm_ਧੱਕੋ_Button_Gadget_Class,
			ਅੰਕੜੇ->encoding_pulldown,
			XmNlabelString, ਚੀਜ਼ਾਂ[i],
			Xm_Nuser_ਅੰਕੜੇ, i,
			ਨਿਰਾ);
		Xt_ਜੋੜੋ_Callback(button, XmNactivateCallback,
			(XtCallbackProc) encoding_callback, (Xt_ਸੂਚਕ) ਅੰਕੜੇ);
		Xt_Manage_ਬੱਚਾ(button);
	    }

	    ਜੇ (ਅੰਕੜੇ->sel[ENCODING])
	    {
		ਜੇ (!strcmp(ਅੰਕੜੇ->sel[ENCODING], list[ENCODING][i]))
		    selected_button = button;
	    }
	    XtFree(list[ENCODING][i]);
	}

	// Destroy all the outstanding menu items.
	ਲਈ (i = ਗਿਣਤੀ_2[ENCODING]; i < (ਪੂਰਨ)n_ਚੀਜ਼ਾਂ; ++i)
	{
	    Xt_Unmanage_ਬੱਚਾ(children[i]);
	    XtDestroyWidget(children[i]);
	}

	// Preserve the current selection visually.
	ਜੇ (selected_button)
	{
	    Xt_Va_ਸੈੱਟ_ਮੁੱਲਾਂ(ਅੰਕੜੇ->encoding_menu,
		    XmNmenuHistory, selected_button,
		    ਨਿਰਾ);
	}

	ਲਈ (i = 0; i < ਗਿਣਤੀ_2[ENCODING]; ++i)
	    XmStringFree(ਚੀਜ਼ਾਂ[i]);
    }

    /*
     * Now loop through the remaining lists and set them up.
     */
    ਲਈ (idx = (ਪੂਰਨ)NAME; idx < (ਪੂਰਨ)NONE; ++idx)
    {
	Widget w;

	ਜੇ (fix == (ਗਿਣਤੀ ListSpecifier)idx)
	    ਜਾਰੀ;

	ਚੋਣ ((ਗਿਣਤੀ ListSpecifier)idx)
	{
	    ਹਾਲਤ NAME:
		w = ਅੰਕੜੇ->list[NAME];
		ਰੋਕੋ;
	    ਹਾਲਤ STYLE:
		w = ਅੰਕੜੇ->list[STYLE];
		ਰੋਕੋ;
	    ਹਾਲਤ ਮਾਪ_3:
		w = ਅੰਕੜੇ->list[ਮਾਪ_3];
		ਰੋਕੋ;
	    ਮੂਲ:
		w = (Widget)0;	// for lint
	}

	ਲਈ (i = 0; i < ਗਿਣਤੀ_2[idx]; ++i)
	{
	    ਚੀਜ਼ਾਂ[i] = XmStringCreateLocalized(list[idx][i]);
	    XtFree(list[idx][i]);
	}
	Xm_List_Delete_All_ਚੀਜ਼ਾਂ(w);
	Xm_List_ਜੋੜੋ_ਚੀਜ਼ਾਂ(w, ਚੀਜ਼ਾਂ, ਗਿਣਤੀ_2[idx], 1);
	ਜੇ (ਅੰਕੜੇ->sel[idx])
	{
	    XmStringFree(ਚੀਜ਼ਾਂ[0]);
	    ਚੀਜ਼ਾਂ[0] = XmStringCreateLocalized(ਅੰਕੜੇ->sel[idx]);
	    Xm_List_Select_ਵਸਤੂ(w, ਚੀਜ਼ਾਂ[0], False);
	    Xm_List_ਸੈੱਟ_Bottom_ਵਸਤੂ(w, ਚੀਜ਼ਾਂ[0]);
	}
	ਲਈ (i = 0; i < ਗਿਣਤੀ_2[idx]; ++i)
	    XmStringFree(ਚੀਜ਼ਾਂ[i]);
    }
}

    ਸਥਿਰ ਖਾਲੀ
stoggle_callback(Widget w ਅਣਵਰਤਿਆ,
	Shared_Font_Sel_ਅੰਕੜੇ_2 *ਅੰਕੜੇ,
	XmToggleButtonCallbackStruct *call_ਅੰਕੜੇ)
{
    ਪੂਰਨ		i, do_sel;
    ਅੱਖਰ	new_ਮਾਪ_2[ਆਰਜ਼ੀ_BUF_ਮਾਪ];
    XmString	str;

    ਜੇ (call_ਅੰਕੜੇ->reason != (ਪੂਰਨ)Xm_CR_ਮੁੱਲ_ਬਦਲਿਆ)
	ਵਾਪਸ;

    do_sel = (ਅੰਕੜੇ->sel[ਮਾਪ_3] != ਨਿਰਾ) && strcmp(ਅੰਕੜੇ->sel[ਮਾਪ_3], wild);

    ਲਈ (i = 0; do_sel && (i < ਅੰਕੜੇ->ਅੰਕ); i++)
	ਜੇ (ਮੇਲ(ਅੰਕੜੇ, ENCODING, i)
		&& ਮੇਲ(ਅੰਕੜੇ, NAME, i)
		&& ਮੇਲ(ਅੰਕੜੇ, STYLE, i)
		&& ਮੇਲ(ਅੰਕੜੇ, ਮਾਪ_3, i))
	{
	    ਮਾਪ_part(fn(ਅੰਕੜੇ, i), new_ਮਾਪ_2, !ਅੰਕੜੇ->ਵਿੱਚ_pixels);
	    ਰੋਕੋ;
	}

    ਅੰਕੜੇ->ਵਿੱਚ_pixels = !ਅੰਕੜੇ->ਵਿੱਚ_pixels;

    ਜੇ (ਅੰਕੜੇ->sel[ਮਾਪ_3])
	XtFree(ਅੰਕੜੇ->sel[ਮਾਪ_3]);
    ਅੰਕੜੇ->sel[ਮਾਪ_3] = ਨਿਰਾ;
    fill_lists(NONE, ਅੰਕੜੇ);

    ਜੇ (do_sel)
    {
	str = XmStringCreateLocalized(new_ਮਾਪ_2);
	Xm_List_Select_ਵਸਤੂ(ਅੰਕੜੇ->list[ਮਾਪ_3], str, True);
	Xm_List_ਸੈੱਟ_Bottom_ਵਸਤੂ(ਅੰਕੜੇ->list[ਮਾਪ_3], str);
	XmStringFree(str);
    }
}

/*
 * Show the currently selected font in the sample text label.
 */
    ਸਥਿਰ ਖਾਲੀ
display_sample(Shared_Font_Sel_ਅੰਕੜੇ_2 *ਅੰਕੜੇ)
{
    Arg		    ਦਲੀਲਾਂ[2];
    ਪੂਰਨ		    n;
    XFontStruct	*   font;
    XmFontList	    font_list;
    Display *	    display;
    XmString	    str;

    display = XtDisplay(ਅੰਕੜੇ->dialog);
    font = XLoadQueryFont(display, ਅੰਕੜੇ->font_name);
    font_list = gui_motif_create_fontlist(font);

    n = 0;
    str = XmStringCreateLocalized("AaBbZzYy 0123456789");
    Xt_ਸੈੱਟ_Arg(ਦਲੀਲਾਂ[n], XmNlabelString, str); n++;
    Xt_ਸੈੱਟ_Arg(ਦਲੀਲਾਂ[n], XmNfontList, font_list); n++;

    Xt_ਸੈੱਟ_ਮੁੱਲਾਂ(ਅੰਕੜੇ->sample, ਦਲੀਲਾਂ, n);
    XmStringFree(str);

    ਜੇ (ਅੰਕੜੇ->old)
    {
	XFreeFont(display, ਅੰਕੜੇ->old);
	XmFontListFree(ਅੰਕੜੇ->old_list);
    }
    ਅੰਕੜੇ->old = font;
    ਅੰਕੜੇ->old_list = font_list;
}


    ਸਥਿਰ Boolean
do_choice(Widget w,
	Shared_Font_Sel_ਅੰਕੜੇ_2 *ਅੰਕੜੇ,
	XmListCallbackStruct *call_ਅੰਕੜੇ,
	ਗਿਣਤੀ ListSpecifier which)
{
    ਅੱਖਰ *sel;

    XmStringGetLtoR(call_ਅੰਕੜੇ->ਵਸਤੂ, XmSTRING_DEFAULT_CHARSET, &sel);

    ਜੇ (!ਅੰਕੜੇ->sel[which])
	ਅੰਕੜੇ->sel[which] = XtNewString(sel);
    ਨਹੀਂਤਾਂ
    {
	ਜੇ (!strcmp(ਅੰਕੜੇ->sel[which], sel))
	{
	    // unselecting current selection
	    XtFree(ਅੰਕੜੇ->sel[which]);
	    ਅੰਕੜੇ->sel[which] = ਨਿਰਾ;
	    ਜੇ (w)
		Xm_List_Deselect_ਵਸਤੂ(w, call_ਅੰਕੜੇ->ਵਸਤੂ);
	}
	ਨਹੀਂਤਾਂ
	{
	    XtFree(ਅੰਕੜੇ->sel[which]);
	    ਅੰਕੜੇ->sel[which] = XtNewString(sel);
	}
    }
    XtFree(sel);

    fill_lists(which, ਅੰਕੜੇ);

    // If there is a font selection, we display it.
    ਜੇ (ਅੰਕੜੇ->sel[ENCODING]
	    && ਅੰਕੜੇ->sel[NAME]
	    && ਅੰਕੜੇ->sel[STYLE]
	    && ਅੰਕੜੇ->sel[ਮਾਪ_3]
	    && strcmp(ਅੰਕੜੇ->sel[ENCODING], wild)
	    && strcmp(ਅੰਕੜੇ->sel[NAME], wild)
	    && strcmp(ਅੰਕੜੇ->sel[STYLE], wild)
	    && strcmp(ਅੰਕੜੇ->sel[ਮਾਪ_3], wild))
    {
	ਪੂਰਨ i;

	ਜੇ (ਅੰਕੜੇ->font_name)
	    XtFree(ਅੰਕੜੇ->font_name);
	ਅੰਕੜੇ->font_name = ਨਿਰਾ;

	ਲਈ (i = 0; i < ਅੰਕੜੇ->ਅੰਕ; i++)
	{
	    ਜੇ (ਮੇਲ(ਅੰਕੜੇ, ENCODING, i)
		    && ਮੇਲ(ਅੰਕੜੇ, NAME, i)
		    && ਮੇਲ(ਅੰਕੜੇ, STYLE, i)
		    && ਮੇਲ(ਅੰਕੜੇ, ਮਾਪ_3, i))
	    {
		ਅੰਕੜੇ->font_name = XtNewString(fn(ਅੰਕੜੇ, i));
		ਰੋਕੋ;
	    }
	}

	ਜੇ (ਅੰਕੜੇ->font_name)
	{
	    Xm_ਪਾਠ_ਸੈੱਟ_String(ਅੰਕੜੇ->name, ਅੰਕੜੇ->font_name);
	    display_sample(ਅੰਕੜੇ);
	}
	ਨਹੀਂਤਾਂ
	    do_dialog(VIM_ERROR,
		    (char_u *)_("Error"),
		    (char_u *)_("Invalid font specification"),
		    (char_u *)_("&Dismiss"), 1, ਨਿਰਾ, FALSE);

	ਵਾਪਸ True;
    }
    ਨਹੀਂਤਾਂ
    {
	ਪੂਰਨ	    n;
	XmString    str;
	Arg	    ਦਲੀਲਾਂ[4];
	ਅੱਖਰ	    *nomatch_ਸੁਨੇਹਾ = _("no specific match");

	n = 0;
	str = XmStringCreateLocalized(nomatch_ਸੁਨੇਹਾ);
	Xt_ਸੈੱਟ_Arg(ਦਲੀਲਾਂ[n], XmNlabelString, str); ++n;
	Xt_ਸੈੱਟ_ਮੁੱਲਾਂ(ਅੰਕੜੇ->sample, ਦਲੀਲਾਂ, n);
	apply_fontlist(ਅੰਕੜੇ->sample);
	Xm_ਪਾਠ_ਸੈੱਟ_String(ਅੰਕੜੇ->name, nomatch_ਸੁਨੇਹਾ);
	XmStringFree(str);

	ਵਾਪਸ False;
    }
}

    ਸਥਿਰ ਖਾਲੀ
encoding_callback(Widget w,
	Shared_Font_Sel_ਅੰਕੜੇ_2 *ਅੰਕੜੇ,
	Xt_ਸੂਚਕ dummy ਅਣਵਰਤਿਆ)
{
    XmString str;
    XmListCallbackStruct fake_ਅੰਕੜੇ;

    Xt_Va_Get_ਮੁੱਲਾਂ(w, XmNlabelString, &str, ਨਿਰਾ);

    ਜੇ (!str)
	ਵਾਪਸ;

    fake_ਅੰਕੜੇ.ਵਸਤੂ = str;

    do_choice(0, ਅੰਕੜੇ, &fake_ਅੰਕੜੇ, ENCODING);
}

    ਸਥਿਰ ਖਾਲੀ
name_callback(Widget w,
	Shared_Font_Sel_ਅੰਕੜੇ_2 *ਅੰਕੜੇ,
	XmListCallbackStruct *call_ਅੰਕੜੇ)
{
    do_choice(w, ਅੰਕੜੇ, call_ਅੰਕੜੇ, NAME);
}

    ਸਥਿਰ ਖਾਲੀ
style_callback(Widget w,
	Shared_Font_Sel_ਅੰਕੜੇ_2 *ਅੰਕੜੇ,
	XmListCallbackStruct *call_ਅੰਕੜੇ)
{
    do_choice(w, ਅੰਕੜੇ, call_ਅੰਕੜੇ, STYLE);
}

    ਸਥਿਰ ਖਾਲੀ
ਮਾਪ_callback(Widget w,
	Shared_Font_Sel_ਅੰਕੜੇ_2 *ਅੰਕੜੇ,
	XmListCallbackStruct *call_ਅੰਕੜੇ)
{
    do_choice(w, ਅੰਕੜੇ, call_ਅੰਕੜੇ, ਮਾਪ_3);
}

    ਸਥਿਰ ਖਾਲੀ
cancel_callback(Widget w ਅਣਵਰਤਿਆ,
	Shared_Font_Sel_ਅੰਕੜੇ_2 *ਅੰਕੜੇ,
	XmListCallbackStruct *call_ਅੰਕੜੇ ਅਣਵਰਤਿਆ)
{
    ਜੇ (ਅੰਕੜੇ->sel[ENCODING])
    {
	XtFree(ਅੰਕੜੇ->sel[ENCODING]);
	ਅੰਕੜੇ->sel[ENCODING] = ਨਿਰਾ;
    }
    ਜੇ (ਅੰਕੜੇ->sel[NAME])
    {
	XtFree(ਅੰਕੜੇ->sel[NAME]);
	ਅੰਕੜੇ->sel[NAME] = ਨਿਰਾ;
    }
    ਜੇ (ਅੰਕੜੇ->sel[STYLE])
    {
	XtFree(ਅੰਕੜੇ->sel[STYLE]);
	ਅੰਕੜੇ->sel[STYLE] = ਨਿਰਾ;
    }
    ਜੇ (ਅੰਕੜੇ->sel[ਮਾਪ_3])
    {
	XtFree(ਅੰਕੜੇ->sel[ਮਾਪ_3]);
	ਅੰਕੜੇ->sel[ਮਾਪ_3] = ਨਿਰਾ;
    }

    ਜੇ (ਅੰਕੜੇ->font_name)
	XtFree(ਅੰਕੜੇ->font_name);
    ਅੰਕੜੇ->font_name = ਨਿਰਾ;

    ਅੰਕੜੇ->ਅੰਕ = 0;
    XFree_Font_ਨਾਂ(ਅੰਕੜੇ->ਨਾਂ);
    ਅੰਕੜੇ->ਨਾਂ = ਨਿਰਾ;
    ਅੰਕੜੇ->ਬਾਹਰ_ਨਿਕਲੋ = True;
}

    ਸਥਿਰ ਖਾਲੀ
ok_callback(Widget w ਅਣਵਰਤਿਆ,
	Shared_Font_Sel_ਅੰਕੜੇ_2 *ਅੰਕੜੇ,
	Xm_ਧੱਕੋ_Button_Callback_Struct *call_ਅੰਕੜੇ ਅਣਵਰਤਿਆ)
{
    ਅੱਖਰ    *ਨਮੂਨਾ;
    ਅੱਖਰ    **name;
    ਪੂਰਨ	    i;

    ਨਮੂਨਾ = Xm_ਪਾਠ_Get_String(ਅੰਕੜੇ->name);
    name    = XListFonts(XtDisplay(ਅੰਕੜੇ->dialog), ਨਮੂਨਾ, 1, &i);
    XtFree(ਨਮੂਨਾ);

    ਜੇ (i != 1)
    {
	do_dialog(VIM_ERROR,
		(char_u *)_("Error"),
		(char_u *)_("Invalid font specification"),
		(char_u *)_("&Dismiss"), 1, ਨਿਰਾ, FALSE);
	XFree_Font_ਨਾਂ(name);
    }
    ਨਹੀਂਤਾਂ
    {
	ਜੇ (ਅੰਕੜੇ->font_name)
	    XtFree(ਅੰਕੜੇ->font_name);
	ਅੰਕੜੇ->font_name = XtNewString(name[0]);

	ਜੇ (ਅੰਕੜੇ->sel[ENCODING])
	{
	    XtFree(ਅੰਕੜੇ->sel[ENCODING]);
	    ਅੰਕੜੇ->sel[ENCODING] = ਨਿਰਾ;
	}
	ਜੇ (ਅੰਕੜੇ->sel[NAME])
	{
	    XtFree(ਅੰਕੜੇ->sel[NAME]);
	    ਅੰਕੜੇ->sel[NAME] = ਨਿਰਾ;
	}
	ਜੇ (ਅੰਕੜੇ->sel[STYLE])
	{
	    XtFree(ਅੰਕੜੇ->sel[STYLE]);
	    ਅੰਕੜੇ->sel[STYLE] = ਨਿਰਾ;
	}
	ਜੇ (ਅੰਕੜੇ->sel[ਮਾਪ_3])
	{
	    XtFree(ਅੰਕੜੇ->sel[ਮਾਪ_3]);
	    ਅੰਕੜੇ->sel[ਮਾਪ_3] = ਨਿਰਾ;
	}

	XFree_Font_ਨਾਂ(name);

	ਅੰਕੜੇ->ਅੰਕ = 0;
	XFree_Font_ਨਾਂ(ਅੰਕੜੇ->ਨਾਂ);
	ਅੰਕੜੇ->ਨਾਂ = ਨਿਰਾ;
	ਅੰਕੜੇ->ਬਾਹਰ_ਨਿਕਲੋ = True;
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
    ਸਥਿਰ Shared_Font_Sel_ਅੰਕੜੇ_2	ਅੰਕੜੇ_3;

    Widget		ਮਾਪਾ;
    Widget		form;
    Widget		separator;
    Widget		sub_form;
    Widget		ਮਾਪ_toggle;
    Widget		name;
    Widget		disp_ਚੌਖਟਾ;
    Widget		ਚੌਖਟਾ;
    Arg			ਦਲੀਲਾਂ[64];
    ਪੂਰਨ			n;
    XmString		str;
    ਅੱਖਰ		big_font[MAX_FONT_NAME_LEN];
    Shared_Font_Sel_ਅੰਕੜੇ_2	*ਅੰਕੜੇ;

    ਅੰਕੜੇ = &ਅੰਕੜੇ_3;

    ਮਾਪਾ = vimShell;
    ਅੰਕੜੇ->ਨਾਂ = XListFonts(XtDisplay(ਮਾਪਾ), "-*-*-*-*-*-*-*-*-*-*-*-*-*-*",
						       MAX_FONTS, &ਅੰਕੜੇ->ਅੰਕ);

    /*
     * Find the name of the biggest font less than the given limit
     * MAX_DISPLAY_SIZE used to set up the initial height of the display
     * widget.
     */

    {
	ਪੂਰਨ	i;
	ਪੂਰਨ	max;
	ਪੂਰਨ	idx = 0;
	ਪੂਰਨ	ਮਾਪ;
	ਅੱਖਰ	buf[128];

	ਲਈ (i = 0, max = 0; i < ਅੰਕੜੇ->ਅੰਕ; i++)
	{
	    get_part(fn(ਅੰਕੜੇ, i), 7, buf);
	    ਮਾਪ = atoi(buf);
	    ਜੇ ((ਮਾਪ > max) && (ਮਾਪ < MAX_DISPLAY_ਮਾਪ))
	    {
		idx = i;
		max = ਮਾਪ;
	    }
	}
	strcpy(big_font, fn(ਅੰਕੜੇ, idx));
    }
    ਅੰਕੜੇ->old = XLoadQueryFont(XtDisplay(ਮਾਪਾ), big_font);
    ਅੰਕੜੇ->old_list = gui_motif_create_fontlist(ਅੰਕੜੇ->old);

    // Set the title of the Dialog window.
    ਅੰਕੜੇ->dialog = XmCreateDialogShell(ਮਾਪਾ, "fontSelector", ਨਿਰਾ, 0);
    str = XmStringCreateLocalized(_("Vim - Font Selector"));

    // Create form popup dialog widget.
    form = XtVaCreateWidget("form",
	    xmFormWidgetClass, ਅੰਕੜੇ->dialog,
	    XmNdialogTitle, str,
	    XmNautoUnmanage, False,
	    XmNdialogStyle, Xm_DIALOG_ਭਰਿਆ_APPLICATION_MODAL,
	    ਨਿਰਾ);
    XmStringFree(str);

    sub_form = XtVaCreateManagedWidget("subForm",
	    xmFormWidgetClass, form,
	    XmNbottomAttachment, XmATTACH_FORM,
	    Xm_Nbottom_ਵਿੱਥ, 4,
	    XmNrightAttachment, XmATTACH_FORM,
	    Xm_Nright_ਵਿੱਥ, 4,
	    XmNtopAttachment, XmATTACH_FORM,
	    Xm_Ntop_ਵਿੱਥ, 4,
	    XmNorientation, XmVERTICAL,
	    ਨਿਰਾ);

    ਅੰਕੜੇ->ok = XtVaCreateManagedWidget(_("OK"),
	    xm_ਧੱਕੋ_Button_Gadget_Class, sub_form,
	    XmNleftAttachment, XmATTACH_FORM,
	    XmNrightAttachment, XmATTACH_FORM,
	    XmNtopAttachment, XmATTACH_FORM,
	    Xm_Ntop_ਵਿੱਥ, 4,
	    ਨਿਰਾ);
    apply_fontlist(ਅੰਕੜੇ->ok);

    ਅੰਕੜੇ->cancel = XtVaCreateManagedWidget(_("Cancel"),
	    xm_ਧੱਕੋ_Button_Gadget_Class, sub_form,
	    XmNrightAttachment, XmATTACH_FORM,
	    XmNleftAttachment, XmATTACH_FORM,
	    XmNtopAttachment, XmATTACH_WIDGET,
	    XmNtopWidget, ਅੰਕੜੇ->ok,
	    Xm_Ntop_ਵਿੱਥ, 4,
	    XmNshowAsDefault, True,
	    ਨਿਰਾ);
    apply_fontlist(ਅੰਕੜੇ->cancel);

    // Create the separator for beauty.
    n = 0;
    Xt_ਸੈੱਟ_Arg(ਦਲੀਲਾਂ[n], XmNorientation, XmVERTICAL); n++;
    Xt_ਸੈੱਟ_Arg(ਦਲੀਲਾਂ[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    Xt_ਸੈੱਟ_Arg(ਦਲੀਲਾਂ[n], XmNtopAttachment, XmATTACH_FORM); n++;
    Xt_ਸੈੱਟ_Arg(ਦਲੀਲਾਂ[n], XmNrightAttachment, XmATTACH_WIDGET); n++;
    Xt_ਸੈੱਟ_Arg(ਦਲੀਲਾਂ[n], XmNrightWidget, sub_form); n++;
    Xt_ਸੈੱਟ_Arg(ਦਲੀਲਾਂ[n], Xm_Nright_ਵਿੱਥ, 4); n++;
    separator = XmCreateSeparatorGadget(form, "separator", ਦਲੀਲਾਂ, n);
    Xt_Manage_ਬੱਚਾ(separator);

    // Create font name text widget and the corresponding label.
    ਅੰਕੜੇ->name = XtVaCreateManagedWidget("fontName",
	    xm_ਪਾਠ_Widget_Class, form,
	    XmNbottomAttachment, XmATTACH_FORM,
	    Xm_Nbottom_ਵਿੱਥ, 4,
	    XmNleftAttachment, XmATTACH_FORM,
	    Xm_Nleft_ਵਿੱਥ, 4,
	    XmNrightAttachment, XmATTACH_WIDGET,
	    XmNrightWidget, separator,
	    Xm_Nright_ਵਿੱਥ, 4,
	    XmNeditable, False,
	    Xm_Nedit_ਬਹੁਲਕ, Xm_SINGLE_ਸਤਰ_EDIT,
	    Xm_Nmax_ਲੰਮਾਈ, MAX_FONT_NAME_LEN,
	    XmNcolumns, 60,
	    ਨਿਰਾ);

    str = XmStringCreateLocalized(_("Name:"));
    name = XtVaCreateManagedWidget("fontNameLabel",
	    xmLabelGadgetClass, form,
	    XmNlabelString, str,
	    Xm_Nuser_ਅੰਕੜੇ, ਅੰਕੜੇ->name,
	    XmNleftAttachment, XmATTACH_OPPOSITE_WIDGET,
	    XmNleftWidget, ਅੰਕੜੇ->name,
	    XmNbottomAttachment, XmATTACH_WIDGET,
	    XmNbottomWidget, ਅੰਕੜੇ->name,
	    Xm_Ntop_ਵਿੱਥ, 1,
	    ਨਿਰਾ);
    XmStringFree(str);
    apply_fontlist(name);

    // create sample display label widget
    disp_ਚੌਖਟਾ = XtVaCreateManagedWidget("sampleFrame",
	    xm_ਚੌਖਟਾ_Widget_Class, form,
	    XmNshadowType, Xm_SHADOW_ETCHED_ਵਿੱਚ,
	    XmNleftAttachment, XmATTACH_FORM,
	    Xm_Nleft_ਵਿੱਥ, 4,
	    XmNbottomAttachment, XmATTACH_WIDGET,
	    XmNbottomWidget, name,
	    XmNrightAttachment, XmATTACH_WIDGET,
	    XmNrightWidget, separator,
	    Xm_Nright_ਵਿੱਥ, 4,
	    XmNalignment, XmALIGNMENT_BEGINNING,
	    ਨਿਰਾ);

    ਅੰਕੜੇ->sample = XtVaCreateManagedWidget("sampleLabel",
	    xmLabelWidgetClass, disp_ਚੌਖਟਾ,
	    XmNleftAttachment, XmATTACH_FORM,
	    XmNtopAttachment, XmATTACH_FORM,
	    XmNbottomAttachment, XmATTACH_FORM,
	    XmNrightAttachment, XmATTACH_FORM,
	    XmNalignment, XmALIGNMENT_BEGINNING,
	    Xm_Nrecompute_ਮਾਪ, False,
	    XmNfontList, ਅੰਕੜੇ->old_list,
	    ਨਿਰਾ);

    // create toggle button
    str = XmStringCreateLocalized(_("Show size in Points"));
    ਮਾਪ_toggle = XtVaCreateManagedWidget("sizeToggle",
	    xmToggleButtonGadgetClass, form,
	    XmNlabelString, str,
	    XmNleftAttachment, XmATTACH_FORM,
	    Xm_Nleft_ਵਿੱਥ, 4,
	    XmNbottomAttachment, XmATTACH_WIDGET,
	    XmNbottomWidget, disp_ਚੌਖਟਾ,
	    Xm_Nbottom_ਵਿੱਥ, 4,
	    ਨਿਰਾ);
    XmStringFree(str);
    apply_fontlist(ਮਾਪ_toggle);
    Xt_Manage_ਬੱਚਾ(ਮਾਪ_toggle);

    // Encoding pulldown menu.

    ਅੰਕੜੇ->encoding_pulldown = XmCreatePulldownMenu(form,
						 "encodingPulldown", ਨਿਰਾ, 0);
    str = XmStringCreateLocalized(_("Encoding:"));
    n = 0;
    Xt_ਸੈੱਟ_Arg(ਦਲੀਲਾਂ[n], XmNsubMenuId, ਅੰਕੜੇ->encoding_pulldown); ++n;
    Xt_ਸੈੱਟ_Arg(ਦਲੀਲਾਂ[n], XmNlabelString, str); ++n;
    Xt_ਸੈੱਟ_Arg(ਦਲੀਲਾਂ[n], XmNleftAttachment, XmATTACH_FORM); ++n;
    Xt_ਸੈੱਟ_Arg(ਦਲੀਲਾਂ[n], Xm_Nleft_ਵਿੱਥ, 4); ++n;
    Xt_ਸੈੱਟ_Arg(ਦਲੀਲਾਂ[n], XmNbottomAttachment, XmATTACH_WIDGET); ++n;
    Xt_ਸੈੱਟ_Arg(ਦਲੀਲਾਂ[n], XmNbottomWidget, ਮਾਪ_toggle); ++n;
    Xt_ਸੈੱਟ_Arg(ਦਲੀਲਾਂ[n], Xm_Nbottom_ਵਿੱਥ, 4); ++n;
    Xt_ਸੈੱਟ_Arg(ਦਲੀਲਾਂ[n], XmNrightAttachment, XmATTACH_WIDGET); ++n;
    Xt_ਸੈੱਟ_Arg(ਦਲੀਲਾਂ[n], XmNrightWidget, separator); ++n;
    Xt_ਸੈੱਟ_Arg(ਦਲੀਲਾਂ[n], Xm_Nright_ਵਿੱਥ, 4); ++n;
    ਅੰਕੜੇ->encoding_menu = XmCreateOptionMenu(form, "encodingMenu", ਦਲੀਲਾਂ, n);
    XmStringFree(str);
    Xm_ਜੋੜੋ_Tab_ਸਮੂਹ(ਅੰਕੜੇ->encoding_menu);

    /*
     * Create scroll list widgets in a separate subform used to manage the
     * different sizes of the lists.
     */

    sub_form = XtVaCreateManagedWidget("subForm",
	    xmFormWidgetClass, form,
	    XmNbottomAttachment, XmATTACH_WIDGET,
	    XmNbottomWidget, ਅੰਕੜੇ->encoding_menu,
	    Xm_Nbottom_ਵਿੱਥ, 4,
	    XmNleftAttachment, XmATTACH_FORM,
	    Xm_Nleft_ਵਿੱਥ, 4,
	    XmNrightAttachment, XmATTACH_WIDGET,
	    XmNrightWidget, separator,
	    Xm_Nright_ਵਿੱਥ, 4,
	    XmNtopAttachment, XmATTACH_FORM,
	    Xm_Ntop_ਵਿੱਥ, 2,
	    XmNorientation, XmVERTICAL,
	    ਨਿਰਾ);

    // font list
    ਚੌਖਟਾ = XtVaCreateManagedWidget("frame", xm_ਚੌਖਟਾ_Widget_Class, sub_form,
	    XmNshadowThickness, 0,
	    XmNtopAttachment, XmATTACH_FORM,
	    XmNbottomAttachment, XmATTACH_FORM,
	    XmNleftAttachment, XmATTACH_FORM,
	    XmNrightAttachment, XmATTACH_POSITION,
	    XmNrightPosition, 50,
	    ਨਿਰਾ);

    str = XmStringCreateLocalized(_("Font:"));
    name = XtVaCreateManagedWidget("nameListLabel", xmLabelGadgetClass, ਚੌਖਟਾ,
	    XmNchildType, Xm_ਚੌਖਟਾ_TITLE_ਬੱਚਾ,
	    XmNchildVerticalAlignment, XmALIGNMENT_CENTER,
	    XmNchildHorizontalAlignment, XmALIGNMENT_BEGINNING,
	    XmNlabelString, str,
	    ਨਿਰਾ);
    XmStringFree(str);
    apply_fontlist(name);

    n = 0;
    Xt_ਸੈੱਟ_Arg(ਦਲੀਲਾਂ[n], Xm_Nvisible_ਵਸਤੂ_ਗਿਣਤੀ, 8); ++n;
    Xt_ਸੈੱਟ_Arg(ਦਲੀਲਾਂ[n], XmNresizable, True); ++n;
    Xt_ਸੈੱਟ_Arg(ਦਲੀਲਾਂ[n], Xm_Nlist_ਮਾਪ_Policy, XmCONSTANT); ++n;
    Xt_ਸੈੱਟ_Arg(ਦਲੀਲਾਂ[n], XmNvisualPolicy, XmVARIABLE); ++n;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ LESSTIF_ਸੰਸਕਰਣ
    Xt_ਸੈੱਟ_Arg(ਦਲੀਲਾਂ[n], XmNscrollBarDisplayPolicy, XmSTATIC); ++n;
#ਜੇ_ਅੰਤ
    ਅੰਕੜੇ->list[NAME] = XmCreateScrolledList(ਚੌਖਟਾ, "fontList", ਦਲੀਲਾਂ, n);
    Xt_Va_ਸੈੱਟ_ਮੁੱਲਾਂ(name, Xm_Nuser_ਅੰਕੜੇ, ਅੰਕੜੇ->list[NAME], ਨਿਰਾ);

    // style list
    ਚੌਖਟਾ = XtVaCreateManagedWidget("frame", xm_ਚੌਖਟਾ_Widget_Class, sub_form,
	    XmNshadowThickness, 0,
	    XmNtopAttachment, XmATTACH_FORM,
	    XmNbottomAttachment, XmATTACH_FORM,
	    XmNleftAttachment, XmATTACH_POSITION,
	    XmNleftPosition, 50,
	    Xm_Nleft_ਵਿੱਥ, 4,
	    XmNrightAttachment, XmATTACH_POSITION,
	    XmNrightPosition, 80,
	    ਨਿਰਾ);

    str = XmStringCreateLocalized(_("Style:"));
    name = XtVaCreateManagedWidget("styleListLabel", xmLabelWidgetClass, ਚੌਖਟਾ,
	    XmNchildType, Xm_ਚੌਖਟਾ_TITLE_ਬੱਚਾ,
	    XmNchildVerticalAlignment, XmALIGNMENT_CENTER,
	    XmNchildHorizontalAlignment, XmALIGNMENT_BEGINNING,
	    XmNlabelString, str,
	    ਨਿਰਾ);
    XmStringFree(str);
    apply_fontlist(name);

    n = 0;
    Xt_ਸੈੱਟ_Arg(ਦਲੀਲਾਂ[n], Xm_Nvisible_ਵਸਤੂ_ਗਿਣਤੀ, 8); ++n;
    Xt_ਸੈੱਟ_Arg(ਦਲੀਲਾਂ[n], XmNresizable, True); ++n;
    Xt_ਸੈੱਟ_Arg(ਦਲੀਲਾਂ[n], Xm_Nlist_ਮਾਪ_Policy, XmCONSTANT); ++n;
    Xt_ਸੈੱਟ_Arg(ਦਲੀਲਾਂ[n], XmNvisualPolicy, XmVARIABLE); ++n;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ LESSTIF_ਸੰਸਕਰਣ
    Xt_ਸੈੱਟ_Arg(ਦਲੀਲਾਂ[n], XmNscrollBarDisplayPolicy, XmSTATIC); ++n;
#ਜੇ_ਅੰਤ
    ਅੰਕੜੇ->list[STYLE] = XmCreateScrolledList(ਚੌਖਟਾ, "styleList", ਦਲੀਲਾਂ, n);
    Xt_Va_ਸੈੱਟ_ਮੁੱਲਾਂ(name, Xm_Nuser_ਅੰਕੜੇ, ਅੰਕੜੇ->list[STYLE], ਨਿਰਾ);

    // size list
    ਚੌਖਟਾ = XtVaCreateManagedWidget("frame", xm_ਚੌਖਟਾ_Widget_Class, sub_form,
	    XmNshadowThickness, 0,
	    XmNtopAttachment, XmATTACH_FORM,
	    XmNbottomAttachment, XmATTACH_FORM,
	    XmNleftAttachment, XmATTACH_POSITION,
	    XmNleftPosition, 80,
	    Xm_Nleft_ਵਿੱਥ, 4,
	    XmNrightAttachment, XmATTACH_FORM,
	    ਨਿਰਾ);

    str = XmStringCreateLocalized(_("Size:"));
    name = XtVaCreateManagedWidget("sizeListLabel", xmLabelGadgetClass, ਚੌਖਟਾ,
	    XmNchildType, Xm_ਚੌਖਟਾ_TITLE_ਬੱਚਾ,
	    XmNchildVerticalAlignment, XmALIGNMENT_CENTER,
	    XmNchildHorizontalAlignment, XmALIGNMENT_BEGINNING,
	    XmNlabelString, str,
	    ਨਿਰਾ);
    XmStringFree(str);
    apply_fontlist(name);

    n = 0;
    Xt_ਸੈੱਟ_Arg(ਦਲੀਲਾਂ[n], Xm_Nvisible_ਵਸਤੂ_ਗਿਣਤੀ, 8); ++n;
    Xt_ਸੈੱਟ_Arg(ਦਲੀਲਾਂ[n], XmNresizable, True); ++n;
    Xt_ਸੈੱਟ_Arg(ਦਲੀਲਾਂ[n], Xm_Nlist_ਮਾਪ_Policy, XmCONSTANT); ++n;
    Xt_ਸੈੱਟ_Arg(ਦਲੀਲਾਂ[n], XmNvisualPolicy, XmVARIABLE); ++n;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ LESSTIF_ਸੰਸਕਰਣ
    Xt_ਸੈੱਟ_Arg(ਦਲੀਲਾਂ[n], XmNscrollBarDisplayPolicy, XmSTATIC); ++n;
#ਜੇ_ਅੰਤ
    ਅੰਕੜੇ->list[ਮਾਪ_3] = XmCreateScrolledList(ਚੌਖਟਾ, "sizeList", ਦਲੀਲਾਂ, n);
    Xt_Va_ਸੈੱਟ_ਮੁੱਲਾਂ(name, Xm_Nuser_ਅੰਕੜੇ, ਅੰਕੜੇ->list[ਮਾਪ_3], ਨਿਰਾ);

    // update form widgets cancel button
    Xt_Va_ਸੈੱਟ_ਮੁੱਲਾਂ(form, XmNcancelButton, ਅੰਕੜੇ->cancel, ਨਿਰਾ);

    Xt_ਜੋੜੋ_Callback(ਮਾਪ_toggle, Xm_Nvalue_ਬਦਲਿਆ_Callback,
	    (XtCallbackProc)stoggle_callback, (Xt_ਸੂਚਕ)ਅੰਕੜੇ);
    Xt_ਜੋੜੋ_Callback(ਅੰਕੜੇ->list[NAME], XmNbrowseSelectionCallback,
	    (XtCallbackProc)name_callback, (Xt_ਸੂਚਕ)ਅੰਕੜੇ);
    Xt_ਜੋੜੋ_Callback(ਅੰਕੜੇ->list[STYLE], XmNbrowseSelectionCallback,
	    (XtCallbackProc)style_callback, (Xt_ਸੂਚਕ)ਅੰਕੜੇ);
    Xt_ਜੋੜੋ_Callback(ਅੰਕੜੇ->list[ਮਾਪ_3], XmNbrowseSelectionCallback,
	    (XtCallbackProc)ਮਾਪ_callback, (Xt_ਸੂਚਕ)ਅੰਕੜੇ);
    Xt_ਜੋੜੋ_Callback(ਅੰਕੜੇ->ok, XmNactivateCallback,
	    (XtCallbackProc)ok_callback, (Xt_ਸੂਚਕ)ਅੰਕੜੇ);
    Xt_ਜੋੜੋ_Callback(ਅੰਕੜੇ->cancel, XmNactivateCallback,
	    (XtCallbackProc)cancel_callback, (Xt_ਸੂਚਕ)ਅੰਕੜੇ);

    XmProcessTraversal(ਅੰਕੜੇ->list[NAME], XmTRAVERSE_CURRENT);

    // setup tabgroups

    Xm_ਜੋੜੋ_Tab_ਸਮੂਹ(ਅੰਕੜੇ->list[NAME]);
    Xm_ਜੋੜੋ_Tab_ਸਮੂਹ(ਅੰਕੜੇ->list[STYLE]);
    Xm_ਜੋੜੋ_Tab_ਸਮੂਹ(ਅੰਕੜੇ->list[ਮਾਪ_3]);
    Xm_ਜੋੜੋ_Tab_ਸਮੂਹ(ਮਾਪ_toggle);
    Xm_ਜੋੜੋ_Tab_ਸਮੂਹ(ਅੰਕੜੇ->name);
    Xm_ਜੋੜੋ_Tab_ਸਮੂਹ(ਅੰਕੜੇ->ok);
    Xm_ਜੋੜੋ_Tab_ਸਮੂਹ(ਅੰਕੜੇ->cancel);

    ਜੋੜੋ_cancel_action(ਅੰਕੜੇ->dialog, (XtCallbackProc)cancel_callback, ਅੰਕੜੇ);

    // Preset selection data.

    ਅੰਕੜੇ->ਬਾਹਰ_ਨਿਕਲੋ = False;
    ਅੰਕੜੇ->ਵਿੱਚ_pixels= True;
    ਅੰਕੜੇ->sel[ENCODING] = ਨਿਰਾ;
    ਅੰਕੜੇ->sel[NAME] = ਨਿਰਾ;
    ਅੰਕੜੇ->sel[STYLE] = ਨਿਰਾ;
    ਅੰਕੜੇ->sel[ਮਾਪ_3] = ਨਿਰਾ;
    ਅੰਕੜੇ->font_name = ਨਿਰਾ;

    // set up current font parameters
    ਜੇ (current && current[0] != '\0')
    {
	ਪੂਰਨ	    i;
	ਅੱਖਰ	    **ਨਾਂ;

	ਨਾਂ = XListFonts(XtDisplay(form), (ਅੱਖਰ *) current, 1, &i);

	ਜੇ (i != 0)
	{
	    ਅੱਖਰ namebuf[ਆਰਜ਼ੀ_BUF_ਮਾਪ];
	    ਅੱਖਰ stylebuf[ਆਰਜ਼ੀ_BUF_ਮਾਪ];
	    ਅੱਖਰ sizebuf[ਆਰਜ਼ੀ_BUF_ਮਾਪ];
	    ਅੱਖਰ encodingbuf[ਆਰਜ਼ੀ_BUF_ਮਾਪ];
	    ਅੱਖਰ *ਮਿਲਿਆ;

	    ਮਿਲਿਆ = ਨਾਂ[0];

	    name_part(ਮਿਲਿਆ, namebuf);
	    style_part(ਮਿਲਿਆ, stylebuf);
	    ਮਾਪ_part(ਮਿਲਿਆ, sizebuf, ਅੰਕੜੇ->ਵਿੱਚ_pixels);
	    encoding_part(ਮਿਲਿਆ, encodingbuf);

	    ਜੇ (*namebuf != NUL
		    && *stylebuf != NUL
		    && *sizebuf != NUL
		    && *encodingbuf != NUL)
	    {
		ਅੰਕੜੇ->sel[NAME] = XtNewString(namebuf);
		ਅੰਕੜੇ->sel[STYLE] = XtNewString(stylebuf);
		ਅੰਕੜੇ->sel[ਮਾਪ_3] = XtNewString(sizebuf);
		ਅੰਕੜੇ->sel[ENCODING] = XtNewString(encodingbuf);
		ਅੰਕੜੇ->font_name = XtNewString(ਨਾਂ[0]);
		display_sample(ਅੰਕੜੇ);
		Xm_ਪਾਠ_ਸੈੱਟ_String(ਅੰਕੜੇ->name, ਅੰਕੜੇ->font_name);
	    }
	    ਨਹੀਂਤਾਂ
	    {
		// We can't preset a symbolic name, which isn't a full font
		// description. Therefore we just behave the same way as if the
		// user didn't have selected anything thus far.
		//
		// Unfortunately there is no known way to expand an abbreviated
		// font name.
		ਅੰਕੜੇ->font_name = ਨਿਰਾ;
	    }
	}
	XFree_Font_ਨਾਂ(ਨਾਂ);
    }

    fill_lists(NONE, ਅੰਕੜੇ);

    // Unfortunately LessTif doesn't align the list widget's properly.  I don't
    // have currently any idea how to fix this problem.
    Xt_Manage_ਬੱਚਾ(ਅੰਕੜੇ->list[NAME]);
    Xt_Manage_ਬੱਚਾ(ਅੰਕੜੇ->list[STYLE]);
    Xt_Manage_ਬੱਚਾ(ਅੰਕੜੇ->list[ਮਾਪ_3]);
    Xt_Manage_ਬੱਚਾ(ਅੰਕੜੇ->encoding_menu);
    manage_centered(form);

    // modal event loop
    ਜਦੋਂ (!ਅੰਕੜੇ->ਬਾਹਰ_ਨਿਕਲੋ)
	XtAppProcessEvent(Xt_Widget_ਮੰਜ਼ਿਲ_Application_Context(ਅੰਕੜੇ->dialog),
							(Xt_ਨਿਵੇਸ਼_ਨਕਾਬ)XtIMAll);

    ਜੇ (ਅੰਕੜੇ->old)
    {
	XFreeFont(XtDisplay(ਅੰਕੜੇ->dialog),  ਅੰਕੜੇ->old);
	XmFontListFree(ਅੰਕੜੇ->old_list);
    }
    XtDestroyWidget(ਅੰਕੜੇ->dialog);

    gui_motif_synch_fonts();

    ਵਾਪਸ (char_u *) ਅੰਕੜੇ->font_name;
}
