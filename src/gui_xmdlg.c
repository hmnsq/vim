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

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MENU
# define apply_fontlist(w) gui_motif_menu_fontlist(w)
#ಇಲ್ಲದಿದ್ದರೆ
# define apply_fontlist(w)
#ಆದರೆ_ಅಂತ್ಯ

/////////////////////////////////////////////////////////////////////////////
// Font selection dialogue implementation.

ಸ್ಥಿರ ಅಕ್ಷರ wild[3] = "*";

/*
 * FIXME: This is a generic function, which should be used throughout the whole
 * application.
 *
 * Add close_callback, which will be called when the user selects close from
 * the window menu.  The close menu item usually activates f.kill which sends a
 * WM_DELETE_WINDOW protocol request for the window.
 */

    ಸ್ಥಿರ ಶೂನ್ಯ
ಕೂಡಿಸು_cancel_action(Widget shell, XtCallbackProc close_callback, ಶೂನ್ಯ *arg)
{
    ಸ್ಥಿರ Atom wmp_atom = 0;
    ಸ್ಥಿರ Atom dw_atom = 0;
    Display *display = XtDisplay(shell);

    // deactivate the built-in delete response of killing the application
    Xt_Va_ಗಣ_ಮೌಲ್ಯಗಳು(shell, XmNdeleteResponse, XmDO_NOTHING, ಖಾಲಿ);

    // add a delete window protocol callback instead
    ಆದರೆ (!dw_atom)
    {
	wmp_atom = XmInternAtom(display, "WM_PROTOCOLS", True);
	dw_atom = XmInternAtom(display, "WM_DELETE_WINDOW", True);
    }
    Xm_ಕೂಡಿಸು_Protocol_Callback(shell, wmp_atom, dw_atom, close_callback, arg);
}

#ವ್ಯಾಖ್ಯಾನಿಸು MAX_FONTS			65535
#ವ್ಯಾಖ್ಯಾನಿಸು MAX_FONT_NAME_LEN		256
#ವ್ಯಾಖ್ಯಾನಿಸು MAX_ENTRIES_ಒಳ_LIST		5000
#ವ್ಯಾಖ್ಯಾನಿಸು MAX_DISPLAY_ಗಾತ್ರ		150
#ವ್ಯಾಖ್ಯಾನಿಸು ತಾತ್ಕಾಲಿಕ_BUF_ಗಾತ್ರ			256

ಎಣಿಕೆ ListSpecifier
{
    ENCODING,
    NAME,
    STYLE,
    ಗಾತ್ರ_4,
    NONE
};

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ Shared_Font_Sel_ದತ್ತಾಂಶ
{
    Widget	dialog;
    Widget	ok;
    Widget	cancel;
    Widget	encoding_pulldown;
    Widget	encoding_menu;
    Widget	list[NONE];
    Widget	name;
    Widget	sample;
    ಅಕ್ಷರ	**ಹೆಸರುಗಳು;	// font name array of arrays
    ಪೂರ್ಣಾಂಕ		ಅಂಕೆ;		// number of font names
    String	sel[NONE];	// selection category
    Boolean	ಒಳ_pixels;	// toggle state - size in pixels
    ಅಕ್ಷರ	*font_name;	// current font name
    XFontStruct	*old;		// font data structure for sample display
    XmFontList	old_list;	// font data structure for sample display
    Boolean	ನಿರ್ಗಮಿಸು;		// used for program exit control
} Shared_Font_Sel_ದತ್ತಾಂಶ_2;

/*
 * Checking access to the font name array for validity.
 */
    ಸ್ಥಿರ ಅಕ್ಷರ *
fn(Shared_Font_Sel_ದತ್ತಾಂಶ_2 *ದತ್ತಾಂಶ, ಪೂರ್ಣಾಂಕ i)
{
    // Assertion checks:
    ಆದರೆ (ದತ್ತಾಂಶ->ಅಂಕೆ < 0)
	abort();
    ಆದರೆ (i >= ದತ್ತಾಂಶ->ಅಂಕೆ)
	i = ದತ್ತಾಂಶ->ಅಂಕೆ - 1;
    ಆದರೆ (i < 0)
	i = 0;

    ಹಿಂತಿರುಗಿ ದತ್ತಾಂಶ->ಹೆಸರುಗಳು[i];
}

/*
 * Get a specific substring from a font name.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
get_part(ಅಕ್ಷರ *ಒಳ, ಪೂರ್ಣಾಂಕ pos, ಅಕ್ಷರ *ಹೊರಹೋಗು)
{
    ಪೂರ್ಣಾಂಕ	i;
    ಪೂರ್ಣಾಂಕ j;

    *ಹೊರಹೋಗು = '\0';

    ಗಾಗಿ (i = 0; (pos > 0) && (ಒಳ[i] != '\0'); ++i)
	ಆದರೆ (ಒಳ[i] == '-')
	    pos--;

    ಆದರೆ (ಒಳ[i] == '\0')
	ಹಿಂತಿರುಗಿ;

    ಗಾಗಿ (j = 0; (ಒಳ[i] != '-') && (ಒಳ[i] != '\0'); ++i, ++j)
	ಹೊರಹೋಗು[j] = ಒಳ[i];
    ಹೊರಹೋಗು[j] = '\0';
}

/*
 * Given a font name this function returns the part used in the first
 * scroll list.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
name_part(ಅಕ್ಷರ *font, ಅಕ್ಷರ *buf)
{
    ಅಕ್ಷರ    buf2[ತಾತ್ಕಾಲಿಕ_BUF_ಗಾತ್ರ];
    ಅಕ್ಷರ    buf3[ತಾತ್ಕಾಲಿಕ_BUF_ಗಾತ್ರ];

    get_part(font, 2, buf2);
    get_part(font, 1, buf3);

    ಆದರೆ (*buf3 != NUL)
	vim_snprintf(buf, ತಾತ್ಕಾಲಿಕ_BUF_ಗಾತ್ರ, "%s (%s)", buf2, buf3);
    ಇಲ್ಲದಿದ್ದರೆ
	vim_snprintf(buf, ತಾತ್ಕಾಲಿಕ_BUF_ಗಾತ್ರ, "%s", buf2);
}

/*
 * Given a font name this function returns the part used in the second scroll list.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
style_part(ಅಕ್ಷರ *font, ಅಕ್ಷರ *buf)
{
    ಅಕ್ಷರ    buf2[ತಾತ್ಕಾಲಿಕ_BUF_ಗಾತ್ರ];
    ಅಕ್ಷರ    buf3[ತಾತ್ಕಾಲಿಕ_BUF_ಗಾತ್ರ];

    get_part(font, 3, buf3);
    get_part(font, 5, buf2);

    ಆದರೆ (!strcmp(buf2, "normal") && !strcmp(buf2, "Normal")
						   && !strcmp(buf2, "NORMAL"))
	vim_snprintf(buf, ತಾತ್ಕಾಲಿಕ_BUF_ಗಾತ್ರ, "%s %s", buf3, buf2);
    ಇಲ್ಲದಿದ್ದರೆ
	strcpy(buf, buf3);

    get_part(font, 6, buf2);

    ಆದರೆ (buf2[0] != '\0')
	vim_snprintf(buf3, ತಾತ್ಕಾಲಿಕ_BUF_ಗಾತ್ರ, "%s %s", buf, buf2);
    ಇಲ್ಲದಿದ್ದರೆ
	strcpy(buf3, buf);

    get_part(font, 4, buf2);

    ಆದರೆ (!strcmp(buf2, "o") || !strcmp(buf2, "O"))
	vim_snprintf(buf, ತಾತ್ಕಾಲಿಕ_BUF_ಗಾತ್ರ, "%s oblique", buf3);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!strcmp(buf2, "i") || !strcmp(buf2, "I"))
	vim_snprintf(buf, ತಾತ್ಕಾಲಿಕ_BUF_ಗಾತ್ರ, "%s italic", buf3);

    ಆದರೆ (!strcmp(buf, " "))
	strcpy(buf, "-");
}

/*
 * Given a font name this function returns the part used in the third
 * scroll list.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ಗಾತ್ರ_part(ಅಕ್ಷರ *font, ಅಕ್ಷರ *buf, ಪೂರ್ಣಾಂಕ ಒಳ_Pixels)
{
    ಪೂರ್ಣಾಂಕ	    ಗಾತ್ರ_2;
    ದಶಮಾಂಶ   ತಾತ್ಕಾಲಿಕ;

    *buf = '\0';

    ಆದರೆ (ಒಳ_Pixels)
    {
	get_part(font, 7, buf);
	ಆದರೆ (*buf != NUL)
	{
	    ಗಾತ್ರ_2 = atoi(buf);
	    sprintf(buf, "%3d", ಗಾತ್ರ_2);
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	get_part(font, 8, buf);
	ಆದರೆ (*buf != NUL)
	{
	    ಗಾತ್ರ_2 = atoi(buf);
	    ತಾತ್ಕಾಲಿಕ = (ದಶಮಾಂಶ)ಗಾತ್ರ_2 / 10.0;
	    ಗಾತ್ರ_2 = ತಾತ್ಕಾಲಿಕ;
	    ಆದರೆ (buf[ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(buf) - 1] == '0')
		sprintf(buf, "%3d", ಗಾತ್ರ_2);
	    ಇಲ್ಲದಿದ್ದರೆ
		sprintf(buf, "%4.1f", ತಾತ್ಕಾಲಿಕ);
	}
    }
}

/*
 * Given a font name this function returns the part used in the choice menu.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
encoding_part(ಅಕ್ಷರ *font, ಅಕ್ಷರ *buf)
{
    ಅಕ್ಷರ    buf1[ತಾತ್ಕಾಲಿಕ_BUF_ಗಾತ್ರ];
    ಅಕ್ಷರ    buf2[ತಾತ್ಕಾಲಿಕ_BUF_ಗಾತ್ರ];

    *buf = '\0';

    get_part(font, 13, buf1);
    get_part(font, 14, buf2);

    ಆದರೆ (*buf1 != NUL && *buf2 != NUL)
	vim_snprintf(buf, ತಾತ್ಕಾಲಿಕ_BUF_ಗಾತ್ರ, "%s-%s", buf1, buf2);
    ಆದರೆ (!strcmp(buf, " "))
	strcpy(buf, "-");
}

/*
 * Inserts a string into correct sorted position in a list.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ಕೂಡಿಸು_ಗಮ್ಯ_list(ಅಕ್ಷರ **buf, ಅಕ್ಷರ *ಅಂಶ, ಪೂರ್ಣಾಂಕ *ಎಣಿಕೆ_2)
{
    ಪೂರ್ಣಾಂಕ	i;
    ಪೂರ್ಣಾಂಕ j;

    ಆದರೆ (*ಎಣಿಕೆ_2 == MAX_ENTRIES_ಒಳ_LIST)
	ಹಿಂತಿರುಗಿ;

    // avoid duplication
    ಗಾಗಿ (i = 0; i < *ಎಣಿಕೆ_2; ++i)
    {
	ಆದರೆ (!strcmp(buf[i], ಅಂಶ))
	    ಹಿಂತಿರುಗಿ;
    }

    // find order place, but make sure that wild card comes first
    ಆದರೆ (!strcmp(ಅಂಶ, wild))
	i = 0;
    ಇಲ್ಲದಿದ್ದರೆ
	ಗಾಗಿ (i = 0; i < *ಎಣಿಕೆ_2; ++i)
	    ಆದರೆ (strcmp(buf[i], ಅಂಶ) > 0 && strcmp(buf[i], wild))
		ನಿಲ್ಲಿಸು;

    // now insert the item
    ಗಾಗಿ (j = *ಎಣಿಕೆ_2; j > i; --j)
	buf[j] = buf[j-1];
    buf[i] = XtNewString(ಅಂಶ);

    ++(*ಎಣಿಕೆ_2);
}

/*
 * True if the font matches some field.
 */
    ಸ್ಥಿರ Boolean
ಹೊಂದಿಕೆ(Shared_Font_Sel_ದತ್ತಾಂಶ_2 *ದತ್ತಾಂಶ, ಎಣಿಕೆ ListSpecifier l, ಪೂರ್ಣಾಂಕ i)
{
    ಅಕ್ಷರ buf[ತಾತ್ಕಾಲಿಕ_BUF_ಗಾತ್ರ];

    // An empty selection or a wild card matches anything.
    ಆದರೆ (!ದತ್ತಾಂಶ->sel[l] || !strcmp(ದತ್ತಾಂಶ->sel[l], wild))
	ಹಿಂತಿರುಗಿ True;

    // chunk out the desired part...
    ಆಯ್ಕೆ (l)
    {
	ಸ್ಥಿತಿ ENCODING:
	    encoding_part(fn(ದತ್ತಾಂಶ, i), buf);
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ NAME:
	    name_part(fn(ದತ್ತಾಂಶ, i), buf);
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ STYLE:
	    style_part(fn(ದತ್ತಾಂಶ, i), buf);
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ ಗಾತ್ರ_4:
	    ಗಾತ್ರ_part(fn(ದತ್ತಾಂಶ, i), buf, ದತ್ತಾಂಶ->ಒಳ_pixels);
	    ನಿಲ್ಲಿಸು;
	ಪೂರ್ವನಿಯೋಜಿತ:
	    ;
    }

    // ...and chew it now

    ಹಿಂತಿರುಗಿ !strcmp(buf, ದತ್ತಾಂಶ->sel[l]);
}

    ಸ್ಥಿರ Boolean
proportional(ಅಕ್ಷರ *font)
{
    ಅಕ್ಷರ buf[ತಾತ್ಕಾಲಿಕ_BUF_ಗಾತ್ರ];

    get_part(font, 11, buf);

    ಹಿಂತಿರುಗಿ !strcmp(buf, "p") || !strcmp(buf, "P");
}


ಸ್ಥಿರ ಶೂನ್ಯ encoding_callback(Widget w, Shared_Font_Sel_ದತ್ತಾಂಶ_2 *ದತ್ತಾಂಶ,
							     Xt_ಸೂಚಕ dummy);

/*
 * Parse through the fontlist data and set up the three scroll lists.  The fix
 * parameter can be used to exclude a list from any changes.  This is used for
 * updates after selections caused by the users actions.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
fill_lists(ಎಣಿಕೆ ListSpecifier fix, Shared_Font_Sel_ದತ್ತಾಂಶ_2 *ದತ್ತಾಂಶ)
{
    ಅಕ್ಷರ	*list[NONE][MAX_ENTRIES_ಒಳ_LIST];
    ಪೂರ್ಣಾಂಕ		ಎಣಿಕೆ_2[NONE];
    ಅಕ್ಷರ	buf[ತಾತ್ಕಾಲಿಕ_BUF_ಗಾತ್ರ];
    XmString	ವಸ್ತುಗಳು[MAX_ENTRIES_ಒಳ_LIST];
    ಪೂರ್ಣಾಂಕ		i;
    ಪೂರ್ಣಾಂಕ		idx;

    ಗಾಗಿ (idx = (ಪೂರ್ಣಾಂಕ)ENCODING; idx < (ಪೂರ್ಣಾಂಕ)NONE; ++idx)
	ಎಣಿಕೆ_2[idx] = 0;

    // First we insert the wild char into every single list.
    ಆದರೆ (fix != ENCODING)
	ಕೂಡಿಸು_ಗಮ್ಯ_list(list[ENCODING], wild, &ಎಣಿಕೆ_2[ENCODING]);
    ಆದರೆ (fix != NAME)
	ಕೂಡಿಸು_ಗಮ್ಯ_list(list[NAME], wild, &ಎಣಿಕೆ_2[NAME]);
    ಆದರೆ (fix != STYLE)
	ಕೂಡಿಸು_ಗಮ್ಯ_list(list[STYLE], wild, &ಎಣಿಕೆ_2[STYLE]);
    ಆದರೆ (fix != ಗಾತ್ರ_4)
	ಕೂಡಿಸು_ಗಮ್ಯ_list(list[ಗಾತ್ರ_4], wild, &ಎಣಿಕೆ_2[ಗಾತ್ರ_4]);

    ಗಾಗಿ (i = 0; i < ದತ್ತಾಂಶ->ಅಂಕೆ && i < MAX_ENTRIES_ಒಳ_LIST; i++)
    {
	ಆದರೆ (proportional(fn(ದತ್ತಾಂಶ, i)))
	    ಮುಂದುವರಿ;

	ಆದರೆ (fix != ENCODING
		&& ಹೊಂದಿಕೆ(ದತ್ತಾಂಶ, NAME, i)
		&& ಹೊಂದಿಕೆ(ದತ್ತಾಂಶ, STYLE, i)
		&& ಹೊಂದಿಕೆ(ದತ್ತಾಂಶ, ಗಾತ್ರ_4, i))
	{
	    encoding_part(fn(ದತ್ತಾಂಶ, i), buf);
	    ಕೂಡಿಸು_ಗಮ್ಯ_list(list[ENCODING], buf, &ಎಣಿಕೆ_2[ENCODING]);
	}

	ಆದರೆ (fix != NAME
		&& ಹೊಂದಿಕೆ(ದತ್ತಾಂಶ, ENCODING, i)
		&& ಹೊಂದಿಕೆ(ದತ್ತಾಂಶ, STYLE, i)
		&& ಹೊಂದಿಕೆ(ದತ್ತಾಂಶ, ಗಾತ್ರ_4, i))
	{
	    name_part(fn(ದತ್ತಾಂಶ, i), buf);
	    ಕೂಡಿಸು_ಗಮ್ಯ_list(list[NAME], buf, &ಎಣಿಕೆ_2[NAME]);
	}

	ಆದರೆ (fix != STYLE
		&& ಹೊಂದಿಕೆ(ದತ್ತಾಂಶ, ENCODING, i)
		&& ಹೊಂದಿಕೆ(ದತ್ತಾಂಶ, NAME, i)
		&& ಹೊಂದಿಕೆ(ದತ್ತಾಂಶ, ಗಾತ್ರ_4, i))
	{
	    style_part(fn(ದತ್ತಾಂಶ, i), buf);
	    ಕೂಡಿಸು_ಗಮ್ಯ_list(list[STYLE], buf, &ಎಣಿಕೆ_2[STYLE]);
	}

	ಆದರೆ (fix != ಗಾತ್ರ_4
		&& ಹೊಂದಿಕೆ(ದತ್ತಾಂಶ, ENCODING, i)
		&& ಹೊಂದಿಕೆ(ದತ್ತಾಂಶ, NAME, i)
		&& ಹೊಂದಿಕೆ(ದತ್ತಾಂಶ, STYLE, i))
	{
	    ಗಾತ್ರ_part(fn(ದತ್ತಾಂಶ, i), buf, ದತ್ತಾಂಶ->ಒಳ_pixels);
	    ಕೂಡಿಸು_ಗಮ್ಯ_list(list[ಗಾತ್ರ_4], buf, &ಎಣಿಕೆ_2[ಗಾತ್ರ_4]);
	}
    }

    /*
     * And now do the preselection in all lists where there was one:
     */

    ಆದರೆ (fix != ENCODING)
    {
	Cardinal n_ವಸ್ತುಗಳು;
	WidgetList children;
	Widget selected_button = 0;

	// Get and update the current button list.
	Xt_Va_Get_ಮೌಲ್ಯಗಳು(ದತ್ತಾಂಶ->encoding_pulldown,
		XmNchildren, &children,
		XmNnumChildren, &n_ವಸ್ತುಗಳು,
		ಖಾಲಿ);

	ಗಾಗಿ (i = 0; i < ಎಣಿಕೆ_2[ENCODING]; ++i)
	{
	    Widget button;

	    ವಸ್ತುಗಳು[i] = XmStringCreateLocalized(list[ENCODING][i]);

	    ಆದರೆ (i < (ಪೂರ್ಣಾಂಕ)n_ವಸ್ತುಗಳು)
	    {
		// recycle old button
		Xt_Va_ಗಣ_ಮೌಲ್ಯಗಳು(children[i],
			XmNlabelString, ವಸ್ತುಗಳು[i],
			Xm_Nuser_ದತ್ತಾಂಶ, i,
			ಖಾಲಿ);
		button = children[i];
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		// create a new button
		button = XtVaCreateManagedWidget("button",
			xm_ತಳ್ಳು_Button_Gadget_Class,
			ದತ್ತಾಂಶ->encoding_pulldown,
			XmNlabelString, ವಸ್ತುಗಳು[i],
			Xm_Nuser_ದತ್ತಾಂಶ, i,
			ಖಾಲಿ);
		Xt_ಕೂಡಿಸು_Callback(button, XmNactivateCallback,
			(XtCallbackProc) encoding_callback, (Xt_ಸೂಚಕ) ದತ್ತಾಂಶ);
		Xt_Manage_ಮಗು(button);
	    }

	    ಆದರೆ (ದತ್ತಾಂಶ->sel[ENCODING])
	    {
		ಆದರೆ (!strcmp(ದತ್ತಾಂಶ->sel[ENCODING], list[ENCODING][i]))
		    selected_button = button;
	    }
	    XtFree(list[ENCODING][i]);
	}

	// Destroy all the outstanding menu items.
	ಗಾಗಿ (i = ಎಣಿಕೆ_2[ENCODING]; i < (ಪೂರ್ಣಾಂಕ)n_ವಸ್ತುಗಳು; ++i)
	{
	    Xt_Unmanage_ಮಗು(children[i]);
	    XtDestroyWidget(children[i]);
	}

	// Preserve the current selection visually.
	ಆದರೆ (selected_button)
	{
	    Xt_Va_ಗಣ_ಮೌಲ್ಯಗಳು(ದತ್ತಾಂಶ->encoding_menu,
		    XmNmenuHistory, selected_button,
		    ಖಾಲಿ);
	}

	ಗಾಗಿ (i = 0; i < ಎಣಿಕೆ_2[ENCODING]; ++i)
	    XmStringFree(ವಸ್ತುಗಳು[i]);
    }

    /*
     * Now loop through the remaining lists and set them up.
     */
    ಗಾಗಿ (idx = (ಪೂರ್ಣಾಂಕ)NAME; idx < (ಪೂರ್ಣಾಂಕ)NONE; ++idx)
    {
	Widget w;

	ಆದರೆ (fix == (ಎಣಿಕೆ ListSpecifier)idx)
	    ಮುಂದುವರಿ;

	ಆಯ್ಕೆ ((ಎಣಿಕೆ ListSpecifier)idx)
	{
	    ಸ್ಥಿತಿ NAME:
		w = ದತ್ತಾಂಶ->list[NAME];
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ STYLE:
		w = ದತ್ತಾಂಶ->list[STYLE];
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ ಗಾತ್ರ_4:
		w = ದತ್ತಾಂಶ->list[ಗಾತ್ರ_4];
		ನಿಲ್ಲಿಸು;
	    ಪೂರ್ವನಿಯೋಜಿತ:
		w = (Widget)0;	// for lint
	}

	ಗಾಗಿ (i = 0; i < ಎಣಿಕೆ_2[idx]; ++i)
	{
	    ವಸ್ತುಗಳು[i] = XmStringCreateLocalized(list[idx][i]);
	    XtFree(list[idx][i]);
	}
	Xm_List_Delete_All_ವಸ್ತುಗಳು(w);
	Xm_List_ಕೂಡಿಸು_ವಸ್ತುಗಳು(w, ವಸ್ತುಗಳು, ಎಣಿಕೆ_2[idx], 1);
	ಆದರೆ (ದತ್ತಾಂಶ->sel[idx])
	{
	    XmStringFree(ವಸ್ತುಗಳು[0]);
	    ವಸ್ತುಗಳು[0] = XmStringCreateLocalized(ದತ್ತಾಂಶ->sel[idx]);
	    Xm_List_Select_ಅಂಶ(w, ವಸ್ತುಗಳು[0], False);
	    Xm_List_ಗಣ_Bottom_ಅಂಶ(w, ವಸ್ತುಗಳು[0]);
	}
	ಗಾಗಿ (i = 0; i < ಎಣಿಕೆ_2[idx]; ++i)
	    XmStringFree(ವಸ್ತುಗಳು[i]);
    }
}

    ಸ್ಥಿರ ಶೂನ್ಯ
stoggle_callback(Widget w ಬಳಸದ,
	Shared_Font_Sel_ದತ್ತಾಂಶ_2 *ದತ್ತಾಂಶ,
	XmToggleButtonCallbackStruct *call_ದತ್ತಾಂಶ)
{
    ಪೂರ್ಣಾಂಕ		i, do_sel;
    ಅಕ್ಷರ	new_ಗಾತ್ರ_2[ತಾತ್ಕಾಲಿಕ_BUF_ಗಾತ್ರ];
    XmString	str;

    ಆದರೆ (call_ದತ್ತಾಂಶ->reason != (ಪೂರ್ಣಾಂಕ)Xm_CR_ಮೌಲ್ಯ_ಬದಲಾಯಿತು)
	ಹಿಂತಿರುಗಿ;

    do_sel = (ದತ್ತಾಂಶ->sel[ಗಾತ್ರ_4] != ಖಾಲಿ) && strcmp(ದತ್ತಾಂಶ->sel[ಗಾತ್ರ_4], wild);

    ಗಾಗಿ (i = 0; do_sel && (i < ದತ್ತಾಂಶ->ಅಂಕೆ); i++)
	ಆದರೆ (ಹೊಂದಿಕೆ(ದತ್ತಾಂಶ, ENCODING, i)
		&& ಹೊಂದಿಕೆ(ದತ್ತಾಂಶ, NAME, i)
		&& ಹೊಂದಿಕೆ(ದತ್ತಾಂಶ, STYLE, i)
		&& ಹೊಂದಿಕೆ(ದತ್ತಾಂಶ, ಗಾತ್ರ_4, i))
	{
	    ಗಾತ್ರ_part(fn(ದತ್ತಾಂಶ, i), new_ಗಾತ್ರ_2, !ದತ್ತಾಂಶ->ಒಳ_pixels);
	    ನಿಲ್ಲಿಸು;
	}

    ದತ್ತಾಂಶ->ಒಳ_pixels = !ದತ್ತಾಂಶ->ಒಳ_pixels;

    ಆದರೆ (ದತ್ತಾಂಶ->sel[ಗಾತ್ರ_4])
	XtFree(ದತ್ತಾಂಶ->sel[ಗಾತ್ರ_4]);
    ದತ್ತಾಂಶ->sel[ಗಾತ್ರ_4] = ಖಾಲಿ;
    fill_lists(NONE, ದತ್ತಾಂಶ);

    ಆದರೆ (do_sel)
    {
	str = XmStringCreateLocalized(new_ಗಾತ್ರ_2);
	Xm_List_Select_ಅಂಶ(ದತ್ತಾಂಶ->list[ಗಾತ್ರ_4], str, True);
	Xm_List_ಗಣ_Bottom_ಅಂಶ(ದತ್ತಾಂಶ->list[ಗಾತ್ರ_4], str);
	XmStringFree(str);
    }
}

/*
 * Show the currently selected font in the sample text label.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
display_sample(Shared_Font_Sel_ದತ್ತಾಂಶ_2 *ದತ್ತಾಂಶ)
{
    Arg		    ವಾದಗಳು[2];
    ಪೂರ್ಣಾಂಕ		    n;
    XFontStruct	*   font;
    XmFontList	    font_list;
    Display *	    display;
    XmString	    str;

    display = XtDisplay(ದತ್ತಾಂಶ->dialog);
    font = XLoadQueryFont(display, ದತ್ತಾಂಶ->font_name);
    font_list = gui_motif_create_fontlist(font);

    n = 0;
    str = XmStringCreateLocalized("AaBbZzYy 0123456789");
    Xt_ಗಣ_Arg(ವಾದಗಳು[n], XmNlabelString, str); n++;
    Xt_ಗಣ_Arg(ವಾದಗಳು[n], XmNfontList, font_list); n++;

    Xt_ಗಣ_ಮೌಲ್ಯಗಳು(ದತ್ತಾಂಶ->sample, ವಾದಗಳು, n);
    XmStringFree(str);

    ಆದರೆ (ದತ್ತಾಂಶ->old)
    {
	XFreeFont(display, ದತ್ತಾಂಶ->old);
	XmFontListFree(ದತ್ತಾಂಶ->old_list);
    }
    ದತ್ತಾಂಶ->old = font;
    ದತ್ತಾಂಶ->old_list = font_list;
}


    ಸ್ಥಿರ Boolean
do_choice(Widget w,
	Shared_Font_Sel_ದತ್ತಾಂಶ_2 *ದತ್ತಾಂಶ,
	XmListCallbackStruct *call_ದತ್ತಾಂಶ,
	ಎಣಿಕೆ ListSpecifier which)
{
    ಅಕ್ಷರ *sel;

    XmStringGetLtoR(call_ದತ್ತಾಂಶ->ಅಂಶ, XmSTRING_DEFAULT_CHARSET, &sel);

    ಆದರೆ (!ದತ್ತಾಂಶ->sel[which])
	ದತ್ತಾಂಶ->sel[which] = XtNewString(sel);
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಆದರೆ (!strcmp(ದತ್ತಾಂಶ->sel[which], sel))
	{
	    // unselecting current selection
	    XtFree(ದತ್ತಾಂಶ->sel[which]);
	    ದತ್ತಾಂಶ->sel[which] = ಖಾಲಿ;
	    ಆದರೆ (w)
		Xm_List_Deselect_ಅಂಶ(w, call_ದತ್ತಾಂಶ->ಅಂಶ);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    XtFree(ದತ್ತಾಂಶ->sel[which]);
	    ದತ್ತಾಂಶ->sel[which] = XtNewString(sel);
	}
    }
    XtFree(sel);

    fill_lists(which, ದತ್ತಾಂಶ);

    // If there is a font selection, we display it.
    ಆದರೆ (ದತ್ತಾಂಶ->sel[ENCODING]
	    && ದತ್ತಾಂಶ->sel[NAME]
	    && ದತ್ತಾಂಶ->sel[STYLE]
	    && ದತ್ತಾಂಶ->sel[ಗಾತ್ರ_4]
	    && strcmp(ದತ್ತಾಂಶ->sel[ENCODING], wild)
	    && strcmp(ದತ್ತಾಂಶ->sel[NAME], wild)
	    && strcmp(ದತ್ತಾಂಶ->sel[STYLE], wild)
	    && strcmp(ದತ್ತಾಂಶ->sel[ಗಾತ್ರ_4], wild))
    {
	ಪೂರ್ಣಾಂಕ i;

	ಆದರೆ (ದತ್ತಾಂಶ->font_name)
	    XtFree(ದತ್ತಾಂಶ->font_name);
	ದತ್ತಾಂಶ->font_name = ಖಾಲಿ;

	ಗಾಗಿ (i = 0; i < ದತ್ತಾಂಶ->ಅಂಕೆ; i++)
	{
	    ಆದರೆ (ಹೊಂದಿಕೆ(ದತ್ತಾಂಶ, ENCODING, i)
		    && ಹೊಂದಿಕೆ(ದತ್ತಾಂಶ, NAME, i)
		    && ಹೊಂದಿಕೆ(ದತ್ತಾಂಶ, STYLE, i)
		    && ಹೊಂದಿಕೆ(ದತ್ತಾಂಶ, ಗಾತ್ರ_4, i))
	    {
		ದತ್ತಾಂಶ->font_name = XtNewString(fn(ದತ್ತಾಂಶ, i));
		ನಿಲ್ಲಿಸು;
	    }
	}

	ಆದರೆ (ದತ್ತಾಂಶ->font_name)
	{
	    Xm_ಪಠ್ಯ_ಗಣ_String(ದತ್ತಾಂಶ->name, ದತ್ತಾಂಶ->font_name);
	    display_sample(ದತ್ತಾಂಶ);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    do_dialog(VIM_ERROR,
		    (char_u *)_("Error"),
		    (char_u *)_("Invalid font specification"),
		    (char_u *)_("&Dismiss"), 1, ಖಾಲಿ, FALSE);

	ಹಿಂತಿರುಗಿ True;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಪೂರ್ಣಾಂಕ	    n;
	XmString    str;
	Arg	    ವಾದಗಳು[4];
	ಅಕ್ಷರ	    *nomatch_ಸಂದೇಶ = _("no specific match");

	n = 0;
	str = XmStringCreateLocalized(nomatch_ಸಂದೇಶ);
	Xt_ಗಣ_Arg(ವಾದಗಳು[n], XmNlabelString, str); ++n;
	Xt_ಗಣ_ಮೌಲ್ಯಗಳು(ದತ್ತಾಂಶ->sample, ವಾದಗಳು, n);
	apply_fontlist(ದತ್ತಾಂಶ->sample);
	Xm_ಪಠ್ಯ_ಗಣ_String(ದತ್ತಾಂಶ->name, nomatch_ಸಂದೇಶ);
	XmStringFree(str);

	ಹಿಂತಿರುಗಿ False;
    }
}

    ಸ್ಥಿರ ಶೂನ್ಯ
encoding_callback(Widget w,
	Shared_Font_Sel_ದತ್ತಾಂಶ_2 *ದತ್ತಾಂಶ,
	Xt_ಸೂಚಕ dummy ಬಳಸದ)
{
    XmString str;
    XmListCallbackStruct fake_ದತ್ತಾಂಶ;

    Xt_Va_Get_ಮೌಲ್ಯಗಳು(w, XmNlabelString, &str, ಖಾಲಿ);

    ಆದರೆ (!str)
	ಹಿಂತಿರುಗಿ;

    fake_ದತ್ತಾಂಶ.ಅಂಶ = str;

    do_choice(0, ದತ್ತಾಂಶ, &fake_ದತ್ತಾಂಶ, ENCODING);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
name_callback(Widget w,
	Shared_Font_Sel_ದತ್ತಾಂಶ_2 *ದತ್ತಾಂಶ,
	XmListCallbackStruct *call_ದತ್ತಾಂಶ)
{
    do_choice(w, ದತ್ತಾಂಶ, call_ದತ್ತಾಂಶ, NAME);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
style_callback(Widget w,
	Shared_Font_Sel_ದತ್ತಾಂಶ_2 *ದತ್ತಾಂಶ,
	XmListCallbackStruct *call_ದತ್ತಾಂಶ)
{
    do_choice(w, ದತ್ತಾಂಶ, call_ದತ್ತಾಂಶ, STYLE);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
ಗಾತ್ರ_callback(Widget w,
	Shared_Font_Sel_ದತ್ತಾಂಶ_2 *ದತ್ತಾಂಶ,
	XmListCallbackStruct *call_ದತ್ತಾಂಶ)
{
    do_choice(w, ದತ್ತಾಂಶ, call_ದತ್ತಾಂಶ, ಗಾತ್ರ_4);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
cancel_callback(Widget w ಬಳಸದ,
	Shared_Font_Sel_ದತ್ತಾಂಶ_2 *ದತ್ತಾಂಶ,
	XmListCallbackStruct *call_ದತ್ತಾಂಶ ಬಳಸದ)
{
    ಆದರೆ (ದತ್ತಾಂಶ->sel[ENCODING])
    {
	XtFree(ದತ್ತಾಂಶ->sel[ENCODING]);
	ದತ್ತಾಂಶ->sel[ENCODING] = ಖಾಲಿ;
    }
    ಆದರೆ (ದತ್ತಾಂಶ->sel[NAME])
    {
	XtFree(ದತ್ತಾಂಶ->sel[NAME]);
	ದತ್ತಾಂಶ->sel[NAME] = ಖಾಲಿ;
    }
    ಆದರೆ (ದತ್ತಾಂಶ->sel[STYLE])
    {
	XtFree(ದತ್ತಾಂಶ->sel[STYLE]);
	ದತ್ತಾಂಶ->sel[STYLE] = ಖಾಲಿ;
    }
    ಆದರೆ (ದತ್ತಾಂಶ->sel[ಗಾತ್ರ_4])
    {
	XtFree(ದತ್ತಾಂಶ->sel[ಗಾತ್ರ_4]);
	ದತ್ತಾಂಶ->sel[ಗಾತ್ರ_4] = ಖಾಲಿ;
    }

    ಆದರೆ (ದತ್ತಾಂಶ->font_name)
	XtFree(ದತ್ತಾಂಶ->font_name);
    ದತ್ತಾಂಶ->font_name = ಖಾಲಿ;

    ದತ್ತಾಂಶ->ಅಂಕೆ = 0;
    XFree_Font_ಹೆಸರುಗಳು(ದತ್ತಾಂಶ->ಹೆಸರುಗಳು);
    ದತ್ತಾಂಶ->ಹೆಸರುಗಳು = ಖಾಲಿ;
    ದತ್ತಾಂಶ->ನಿರ್ಗಮಿಸು = True;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
ok_callback(Widget w ಬಳಸದ,
	Shared_Font_Sel_ದತ್ತಾಂಶ_2 *ದತ್ತಾಂಶ,
	Xm_ತಳ್ಳು_Button_Callback_Struct *call_ದತ್ತಾಂಶ ಬಳಸದ)
{
    ಅಕ್ಷರ    *ನಮೂನೆ;
    ಅಕ್ಷರ    **name;
    ಪೂರ್ಣಾಂಕ	    i;

    ನಮೂನೆ = Xm_ಪಠ್ಯ_Get_String(ದತ್ತಾಂಶ->name);
    name    = XListFonts(XtDisplay(ದತ್ತಾಂಶ->dialog), ನಮೂನೆ, 1, &i);
    XtFree(ನಮೂನೆ);

    ಆದರೆ (i != 1)
    {
	do_dialog(VIM_ERROR,
		(char_u *)_("Error"),
		(char_u *)_("Invalid font specification"),
		(char_u *)_("&Dismiss"), 1, ಖಾಲಿ, FALSE);
	XFree_Font_ಹೆಸರುಗಳು(name);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಆದರೆ (ದತ್ತಾಂಶ->font_name)
	    XtFree(ದತ್ತಾಂಶ->font_name);
	ದತ್ತಾಂಶ->font_name = XtNewString(name[0]);

	ಆದರೆ (ದತ್ತಾಂಶ->sel[ENCODING])
	{
	    XtFree(ದತ್ತಾಂಶ->sel[ENCODING]);
	    ದತ್ತಾಂಶ->sel[ENCODING] = ಖಾಲಿ;
	}
	ಆದರೆ (ದತ್ತಾಂಶ->sel[NAME])
	{
	    XtFree(ದತ್ತಾಂಶ->sel[NAME]);
	    ದತ್ತಾಂಶ->sel[NAME] = ಖಾಲಿ;
	}
	ಆದರೆ (ದತ್ತಾಂಶ->sel[STYLE])
	{
	    XtFree(ದತ್ತಾಂಶ->sel[STYLE]);
	    ದತ್ತಾಂಶ->sel[STYLE] = ಖಾಲಿ;
	}
	ಆದರೆ (ದತ್ತಾಂಶ->sel[ಗಾತ್ರ_4])
	{
	    XtFree(ದತ್ತಾಂಶ->sel[ಗಾತ್ರ_4]);
	    ದತ್ತಾಂಶ->sel[ಗಾತ್ರ_4] = ಖಾಲಿ;
	}

	XFree_Font_ಹೆಸರುಗಳು(name);

	ದತ್ತಾಂಶ->ಅಂಕೆ = 0;
	XFree_Font_ಹೆಸರುಗಳು(ದತ್ತಾಂಶ->ಹೆಸರುಗಳು);
	ದತ್ತಾಂಶ->ಹೆಸರುಗಳು = ಖಾಲಿ;
	ದತ್ತಾಂಶ->ನಿರ್ಗಮಿಸು = True;
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
    ಸ್ಥಿರ Shared_Font_Sel_ದತ್ತಾಂಶ_2	ದತ್ತಾಂಶ_3;

    Widget		ಪೋಷಕ;
    Widget		form;
    Widget		separator;
    Widget		sub_form;
    Widget		ಗಾತ್ರ_toggle;
    Widget		name;
    Widget		disp_ಚೌಕಟ್ಟು;
    Widget		ಚೌಕಟ್ಟು;
    Arg			ವಾದಗಳು[64];
    ಪೂರ್ಣಾಂಕ			n;
    XmString		str;
    ಅಕ್ಷರ		big_font[MAX_FONT_NAME_LEN];
    Shared_Font_Sel_ದತ್ತಾಂಶ_2	*ದತ್ತಾಂಶ;

    ದತ್ತಾಂಶ = &ದತ್ತಾಂಶ_3;

    ಪೋಷಕ = vimShell;
    ದತ್ತಾಂಶ->ಹೆಸರುಗಳು = XListFonts(XtDisplay(ಪೋಷಕ), "-*-*-*-*-*-*-*-*-*-*-*-*-*-*",
						       MAX_FONTS, &ದತ್ತಾಂಶ->ಅಂಕೆ);

    /*
     * Find the name of the biggest font less than the given limit
     * MAX_DISPLAY_SIZE used to set up the initial height of the display
     * widget.
     */

    {
	ಪೂರ್ಣಾಂಕ	i;
	ಪೂರ್ಣಾಂಕ	max;
	ಪೂರ್ಣಾಂಕ	idx = 0;
	ಪೂರ್ಣಾಂಕ	ಗಾತ್ರ_2;
	ಅಕ್ಷರ	buf[128];

	ಗಾಗಿ (i = 0, max = 0; i < ದತ್ತಾಂಶ->ಅಂಕೆ; i++)
	{
	    get_part(fn(ದತ್ತಾಂಶ, i), 7, buf);
	    ಗಾತ್ರ_2 = atoi(buf);
	    ಆದರೆ ((ಗಾತ್ರ_2 > max) && (ಗಾತ್ರ_2 < MAX_DISPLAY_ಗಾತ್ರ))
	    {
		idx = i;
		max = ಗಾತ್ರ_2;
	    }
	}
	strcpy(big_font, fn(ದತ್ತಾಂಶ, idx));
    }
    ದತ್ತಾಂಶ->old = XLoadQueryFont(XtDisplay(ಪೋಷಕ), big_font);
    ದತ್ತಾಂಶ->old_list = gui_motif_create_fontlist(ದತ್ತಾಂಶ->old);

    // Set the title of the Dialog window.
    ದತ್ತಾಂಶ->dialog = XmCreateDialogShell(ಪೋಷಕ, "fontSelector", ಖಾಲಿ, 0);
    str = XmStringCreateLocalized(_("Vim - Font Selector"));

    // Create form popup dialog widget.
    form = XtVaCreateWidget("form",
	    xmFormWidgetClass, ದತ್ತಾಂಶ->dialog,
	    XmNdialogTitle, str,
	    XmNautoUnmanage, False,
	    XmNdialogStyle, Xm_DIALOG_ತುಂಬಿದ_APPLICATION_MODAL,
	    ಖಾಲಿ);
    XmStringFree(str);

    sub_form = XtVaCreateManagedWidget("subForm",
	    xmFormWidgetClass, form,
	    XmNbottomAttachment, XmATTACH_FORM,
	    Xm_Nbottom_ಸ್ಥಾನಾಂತರ, 4,
	    XmNrightAttachment, XmATTACH_FORM,
	    Xm_Nright_ಸ್ಥಾನಾಂತರ, 4,
	    XmNtopAttachment, XmATTACH_FORM,
	    Xm_Ntop_ಸ್ಥಾನಾಂತರ, 4,
	    XmNorientation, XmVERTICAL,
	    ಖಾಲಿ);

    ದತ್ತಾಂಶ->ok = XtVaCreateManagedWidget(_("OK"),
	    xm_ತಳ್ಳು_Button_Gadget_Class, sub_form,
	    XmNleftAttachment, XmATTACH_FORM,
	    XmNrightAttachment, XmATTACH_FORM,
	    XmNtopAttachment, XmATTACH_FORM,
	    Xm_Ntop_ಸ್ಥಾನಾಂತರ, 4,
	    ಖಾಲಿ);
    apply_fontlist(ದತ್ತಾಂಶ->ok);

    ದತ್ತಾಂಶ->cancel = XtVaCreateManagedWidget(_("Cancel"),
	    xm_ತಳ್ಳು_Button_Gadget_Class, sub_form,
	    XmNrightAttachment, XmATTACH_FORM,
	    XmNleftAttachment, XmATTACH_FORM,
	    XmNtopAttachment, XmATTACH_WIDGET,
	    XmNtopWidget, ದತ್ತಾಂಶ->ok,
	    Xm_Ntop_ಸ್ಥಾನಾಂತರ, 4,
	    XmNshowAsDefault, True,
	    ಖಾಲಿ);
    apply_fontlist(ದತ್ತಾಂಶ->cancel);

    // Create the separator for beauty.
    n = 0;
    Xt_ಗಣ_Arg(ವಾದಗಳು[n], XmNorientation, XmVERTICAL); n++;
    Xt_ಗಣ_Arg(ವಾದಗಳು[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    Xt_ಗಣ_Arg(ವಾದಗಳು[n], XmNtopAttachment, XmATTACH_FORM); n++;
    Xt_ಗಣ_Arg(ವಾದಗಳು[n], XmNrightAttachment, XmATTACH_WIDGET); n++;
    Xt_ಗಣ_Arg(ವಾದಗಳು[n], XmNrightWidget, sub_form); n++;
    Xt_ಗಣ_Arg(ವಾದಗಳು[n], Xm_Nright_ಸ್ಥಾನಾಂತರ, 4); n++;
    separator = XmCreateSeparatorGadget(form, "separator", ವಾದಗಳು, n);
    Xt_Manage_ಮಗು(separator);

    // Create font name text widget and the corresponding label.
    ದತ್ತಾಂಶ->name = XtVaCreateManagedWidget("fontName",
	    xm_ಪಠ್ಯ_Widget_Class, form,
	    XmNbottomAttachment, XmATTACH_FORM,
	    Xm_Nbottom_ಸ್ಥಾನಾಂತರ, 4,
	    XmNleftAttachment, XmATTACH_FORM,
	    Xm_Nleft_ಸ್ಥಾನಾಂತರ, 4,
	    XmNrightAttachment, XmATTACH_WIDGET,
	    XmNrightWidget, separator,
	    Xm_Nright_ಸ್ಥಾನಾಂತರ, 4,
	    XmNeditable, False,
	    Xm_Nedit_ಬಹುಲಕ, Xm_SINGLE_ಸಾಲು_EDIT,
	    Xm_Nmax_ಉದ್ದಳತೆ, MAX_FONT_NAME_LEN,
	    XmNcolumns, 60,
	    ಖಾಲಿ);

    str = XmStringCreateLocalized(_("Name:"));
    name = XtVaCreateManagedWidget("fontNameLabel",
	    xmLabelGadgetClass, form,
	    XmNlabelString, str,
	    Xm_Nuser_ದತ್ತಾಂಶ, ದತ್ತಾಂಶ->name,
	    XmNleftAttachment, XmATTACH_OPPOSITE_WIDGET,
	    XmNleftWidget, ದತ್ತಾಂಶ->name,
	    XmNbottomAttachment, XmATTACH_WIDGET,
	    XmNbottomWidget, ದತ್ತಾಂಶ->name,
	    Xm_Ntop_ಸ್ಥಾನಾಂತರ, 1,
	    ಖಾಲಿ);
    XmStringFree(str);
    apply_fontlist(name);

    // create sample display label widget
    disp_ಚೌಕಟ್ಟು = XtVaCreateManagedWidget("sampleFrame",
	    xm_ಚೌಕಟ್ಟು_Widget_Class, form,
	    XmNshadowType, Xm_SHADOW_ETCHED_ಒಳ,
	    XmNleftAttachment, XmATTACH_FORM,
	    Xm_Nleft_ಸ್ಥಾನಾಂತರ, 4,
	    XmNbottomAttachment, XmATTACH_WIDGET,
	    XmNbottomWidget, name,
	    XmNrightAttachment, XmATTACH_WIDGET,
	    XmNrightWidget, separator,
	    Xm_Nright_ಸ್ಥಾನಾಂತರ, 4,
	    XmNalignment, XmALIGNMENT_BEGINNING,
	    ಖಾಲಿ);

    ದತ್ತಾಂಶ->sample = XtVaCreateManagedWidget("sampleLabel",
	    xmLabelWidgetClass, disp_ಚೌಕಟ್ಟು,
	    XmNleftAttachment, XmATTACH_FORM,
	    XmNtopAttachment, XmATTACH_FORM,
	    XmNbottomAttachment, XmATTACH_FORM,
	    XmNrightAttachment, XmATTACH_FORM,
	    XmNalignment, XmALIGNMENT_BEGINNING,
	    Xm_Nrecompute_ಗಾತ್ರ, False,
	    XmNfontList, ದತ್ತಾಂಶ->old_list,
	    ಖಾಲಿ);

    // create toggle button
    str = XmStringCreateLocalized(_("Show size in Points"));
    ಗಾತ್ರ_toggle = XtVaCreateManagedWidget("sizeToggle",
	    xmToggleButtonGadgetClass, form,
	    XmNlabelString, str,
	    XmNleftAttachment, XmATTACH_FORM,
	    Xm_Nleft_ಸ್ಥಾನಾಂತರ, 4,
	    XmNbottomAttachment, XmATTACH_WIDGET,
	    XmNbottomWidget, disp_ಚೌಕಟ್ಟು,
	    Xm_Nbottom_ಸ್ಥಾನಾಂತರ, 4,
	    ಖಾಲಿ);
    XmStringFree(str);
    apply_fontlist(ಗಾತ್ರ_toggle);
    Xt_Manage_ಮಗು(ಗಾತ್ರ_toggle);

    // Encoding pulldown menu.

    ದತ್ತಾಂಶ->encoding_pulldown = XmCreatePulldownMenu(form,
						 "encodingPulldown", ಖಾಲಿ, 0);
    str = XmStringCreateLocalized(_("Encoding:"));
    n = 0;
    Xt_ಗಣ_Arg(ವಾದಗಳು[n], XmNsubMenuId, ದತ್ತಾಂಶ->encoding_pulldown); ++n;
    Xt_ಗಣ_Arg(ವಾದಗಳು[n], XmNlabelString, str); ++n;
    Xt_ಗಣ_Arg(ವಾದಗಳು[n], XmNleftAttachment, XmATTACH_FORM); ++n;
    Xt_ಗಣ_Arg(ವಾದಗಳು[n], Xm_Nleft_ಸ್ಥಾನಾಂತರ, 4); ++n;
    Xt_ಗಣ_Arg(ವಾದಗಳು[n], XmNbottomAttachment, XmATTACH_WIDGET); ++n;
    Xt_ಗಣ_Arg(ವಾದಗಳು[n], XmNbottomWidget, ಗಾತ್ರ_toggle); ++n;
    Xt_ಗಣ_Arg(ವಾದಗಳು[n], Xm_Nbottom_ಸ್ಥಾನಾಂತರ, 4); ++n;
    Xt_ಗಣ_Arg(ವಾದಗಳು[n], XmNrightAttachment, XmATTACH_WIDGET); ++n;
    Xt_ಗಣ_Arg(ವಾದಗಳು[n], XmNrightWidget, separator); ++n;
    Xt_ಗಣ_Arg(ವಾದಗಳು[n], Xm_Nright_ಸ್ಥಾನಾಂತರ, 4); ++n;
    ದತ್ತಾಂಶ->encoding_menu = XmCreateOptionMenu(form, "encodingMenu", ವಾದಗಳು, n);
    XmStringFree(str);
    Xm_ಕೂಡಿಸು_Tab_ಗುಂಪು(ದತ್ತಾಂಶ->encoding_menu);

    /*
     * Create scroll list widgets in a separate subform used to manage the
     * different sizes of the lists.
     */

    sub_form = XtVaCreateManagedWidget("subForm",
	    xmFormWidgetClass, form,
	    XmNbottomAttachment, XmATTACH_WIDGET,
	    XmNbottomWidget, ದತ್ತಾಂಶ->encoding_menu,
	    Xm_Nbottom_ಸ್ಥಾನಾಂತರ, 4,
	    XmNleftAttachment, XmATTACH_FORM,
	    Xm_Nleft_ಸ್ಥಾನಾಂತರ, 4,
	    XmNrightAttachment, XmATTACH_WIDGET,
	    XmNrightWidget, separator,
	    Xm_Nright_ಸ್ಥಾನಾಂತರ, 4,
	    XmNtopAttachment, XmATTACH_FORM,
	    Xm_Ntop_ಸ್ಥಾನಾಂತರ, 2,
	    XmNorientation, XmVERTICAL,
	    ಖಾಲಿ);

    // font list
    ಚೌಕಟ್ಟು = XtVaCreateManagedWidget("frame", xm_ಚೌಕಟ್ಟು_Widget_Class, sub_form,
	    XmNshadowThickness, 0,
	    XmNtopAttachment, XmATTACH_FORM,
	    XmNbottomAttachment, XmATTACH_FORM,
	    XmNleftAttachment, XmATTACH_FORM,
	    XmNrightAttachment, XmATTACH_POSITION,
	    XmNrightPosition, 50,
	    ಖಾಲಿ);

    str = XmStringCreateLocalized(_("Font:"));
    name = XtVaCreateManagedWidget("nameListLabel", xmLabelGadgetClass, ಚೌಕಟ್ಟು,
	    XmNchildType, Xm_ಚೌಕಟ್ಟು_TITLE_ಮಗು,
	    XmNchildVerticalAlignment, XmALIGNMENT_CENTER,
	    XmNchildHorizontalAlignment, XmALIGNMENT_BEGINNING,
	    XmNlabelString, str,
	    ಖಾಲಿ);
    XmStringFree(str);
    apply_fontlist(name);

    n = 0;
    Xt_ಗಣ_Arg(ವಾದಗಳು[n], Xm_Nvisible_ಅಂಶ_ಎಣಿಕೆ, 8); ++n;
    Xt_ಗಣ_Arg(ವಾದಗಳು[n], XmNresizable, True); ++n;
    Xt_ಗಣ_Arg(ವಾದಗಳು[n], Xm_Nlist_ಗಾತ್ರ_Policy, XmCONSTANT); ++n;
    Xt_ಗಣ_Arg(ವಾದಗಳು[n], XmNvisualPolicy, XmVARIABLE); ++n;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ LESSTIF_ಆವೃತ್ತಿ
    Xt_ಗಣ_Arg(ವಾದಗಳು[n], XmNscrollBarDisplayPolicy, XmSTATIC); ++n;
#ಆದರೆ_ಅಂತ್ಯ
    ದತ್ತಾಂಶ->list[NAME] = XmCreateScrolledList(ಚೌಕಟ್ಟು, "fontList", ವಾದಗಳು, n);
    Xt_Va_ಗಣ_ಮೌಲ್ಯಗಳು(name, Xm_Nuser_ದತ್ತಾಂಶ, ದತ್ತಾಂಶ->list[NAME], ಖಾಲಿ);

    // style list
    ಚೌಕಟ್ಟು = XtVaCreateManagedWidget("frame", xm_ಚೌಕಟ್ಟು_Widget_Class, sub_form,
	    XmNshadowThickness, 0,
	    XmNtopAttachment, XmATTACH_FORM,
	    XmNbottomAttachment, XmATTACH_FORM,
	    XmNleftAttachment, XmATTACH_POSITION,
	    XmNleftPosition, 50,
	    Xm_Nleft_ಸ್ಥಾನಾಂತರ, 4,
	    XmNrightAttachment, XmATTACH_POSITION,
	    XmNrightPosition, 80,
	    ಖಾಲಿ);

    str = XmStringCreateLocalized(_("Style:"));
    name = XtVaCreateManagedWidget("styleListLabel", xmLabelWidgetClass, ಚೌಕಟ್ಟು,
	    XmNchildType, Xm_ಚೌಕಟ್ಟು_TITLE_ಮಗು,
	    XmNchildVerticalAlignment, XmALIGNMENT_CENTER,
	    XmNchildHorizontalAlignment, XmALIGNMENT_BEGINNING,
	    XmNlabelString, str,
	    ಖಾಲಿ);
    XmStringFree(str);
    apply_fontlist(name);

    n = 0;
    Xt_ಗಣ_Arg(ವಾದಗಳು[n], Xm_Nvisible_ಅಂಶ_ಎಣಿಕೆ, 8); ++n;
    Xt_ಗಣ_Arg(ವಾದಗಳು[n], XmNresizable, True); ++n;
    Xt_ಗಣ_Arg(ವಾದಗಳು[n], Xm_Nlist_ಗಾತ್ರ_Policy, XmCONSTANT); ++n;
    Xt_ಗಣ_Arg(ವಾದಗಳು[n], XmNvisualPolicy, XmVARIABLE); ++n;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ LESSTIF_ಆವೃತ್ತಿ
    Xt_ಗಣ_Arg(ವಾದಗಳು[n], XmNscrollBarDisplayPolicy, XmSTATIC); ++n;
#ಆದರೆ_ಅಂತ್ಯ
    ದತ್ತಾಂಶ->list[STYLE] = XmCreateScrolledList(ಚೌಕಟ್ಟು, "styleList", ವಾದಗಳು, n);
    Xt_Va_ಗಣ_ಮೌಲ್ಯಗಳು(name, Xm_Nuser_ದತ್ತಾಂಶ, ದತ್ತಾಂಶ->list[STYLE], ಖಾಲಿ);

    // size list
    ಚೌಕಟ್ಟು = XtVaCreateManagedWidget("frame", xm_ಚೌಕಟ್ಟು_Widget_Class, sub_form,
	    XmNshadowThickness, 0,
	    XmNtopAttachment, XmATTACH_FORM,
	    XmNbottomAttachment, XmATTACH_FORM,
	    XmNleftAttachment, XmATTACH_POSITION,
	    XmNleftPosition, 80,
	    Xm_Nleft_ಸ್ಥಾನಾಂತರ, 4,
	    XmNrightAttachment, XmATTACH_FORM,
	    ಖಾಲಿ);

    str = XmStringCreateLocalized(_("Size:"));
    name = XtVaCreateManagedWidget("sizeListLabel", xmLabelGadgetClass, ಚೌಕಟ್ಟು,
	    XmNchildType, Xm_ಚೌಕಟ್ಟು_TITLE_ಮಗು,
	    XmNchildVerticalAlignment, XmALIGNMENT_CENTER,
	    XmNchildHorizontalAlignment, XmALIGNMENT_BEGINNING,
	    XmNlabelString, str,
	    ಖಾಲಿ);
    XmStringFree(str);
    apply_fontlist(name);

    n = 0;
    Xt_ಗಣ_Arg(ವಾದಗಳು[n], Xm_Nvisible_ಅಂಶ_ಎಣಿಕೆ, 8); ++n;
    Xt_ಗಣ_Arg(ವಾದಗಳು[n], XmNresizable, True); ++n;
    Xt_ಗಣ_Arg(ವಾದಗಳು[n], Xm_Nlist_ಗಾತ್ರ_Policy, XmCONSTANT); ++n;
    Xt_ಗಣ_Arg(ವಾದಗಳು[n], XmNvisualPolicy, XmVARIABLE); ++n;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ LESSTIF_ಆವೃತ್ತಿ
    Xt_ಗಣ_Arg(ವಾದಗಳು[n], XmNscrollBarDisplayPolicy, XmSTATIC); ++n;
#ಆದರೆ_ಅಂತ್ಯ
    ದತ್ತಾಂಶ->list[ಗಾತ್ರ_4] = XmCreateScrolledList(ಚೌಕಟ್ಟು, "sizeList", ವಾದಗಳು, n);
    Xt_Va_ಗಣ_ಮೌಲ್ಯಗಳು(name, Xm_Nuser_ದತ್ತಾಂಶ, ದತ್ತಾಂಶ->list[ಗಾತ್ರ_4], ಖಾಲಿ);

    // update form widgets cancel button
    Xt_Va_ಗಣ_ಮೌಲ್ಯಗಳು(form, XmNcancelButton, ದತ್ತಾಂಶ->cancel, ಖಾಲಿ);

    Xt_ಕೂಡಿಸು_Callback(ಗಾತ್ರ_toggle, Xm_Nvalue_ಬದಲಾಯಿತು_Callback,
	    (XtCallbackProc)stoggle_callback, (Xt_ಸೂಚಕ)ದತ್ತಾಂಶ);
    Xt_ಕೂಡಿಸು_Callback(ದತ್ತಾಂಶ->list[NAME], XmNbrowseSelectionCallback,
	    (XtCallbackProc)name_callback, (Xt_ಸೂಚಕ)ದತ್ತಾಂಶ);
    Xt_ಕೂಡಿಸು_Callback(ದತ್ತಾಂಶ->list[STYLE], XmNbrowseSelectionCallback,
	    (XtCallbackProc)style_callback, (Xt_ಸೂಚಕ)ದತ್ತಾಂಶ);
    Xt_ಕೂಡಿಸು_Callback(ದತ್ತಾಂಶ->list[ಗಾತ್ರ_4], XmNbrowseSelectionCallback,
	    (XtCallbackProc)ಗಾತ್ರ_callback, (Xt_ಸೂಚಕ)ದತ್ತಾಂಶ);
    Xt_ಕೂಡಿಸು_Callback(ದತ್ತಾಂಶ->ok, XmNactivateCallback,
	    (XtCallbackProc)ok_callback, (Xt_ಸೂಚಕ)ದತ್ತಾಂಶ);
    Xt_ಕೂಡಿಸು_Callback(ದತ್ತಾಂಶ->cancel, XmNactivateCallback,
	    (XtCallbackProc)cancel_callback, (Xt_ಸೂಚಕ)ದತ್ತಾಂಶ);

    XmProcessTraversal(ದತ್ತಾಂಶ->list[NAME], XmTRAVERSE_CURRENT);

    // setup tabgroups

    Xm_ಕೂಡಿಸು_Tab_ಗುಂಪು(ದತ್ತಾಂಶ->list[NAME]);
    Xm_ಕೂಡಿಸು_Tab_ಗುಂಪು(ದತ್ತಾಂಶ->list[STYLE]);
    Xm_ಕೂಡಿಸು_Tab_ಗುಂಪು(ದತ್ತಾಂಶ->list[ಗಾತ್ರ_4]);
    Xm_ಕೂಡಿಸು_Tab_ಗುಂಪು(ಗಾತ್ರ_toggle);
    Xm_ಕೂಡಿಸು_Tab_ಗುಂಪು(ದತ್ತಾಂಶ->name);
    Xm_ಕೂಡಿಸು_Tab_ಗುಂಪು(ದತ್ತಾಂಶ->ok);
    Xm_ಕೂಡಿಸು_Tab_ಗುಂಪು(ದತ್ತಾಂಶ->cancel);

    ಕೂಡಿಸು_cancel_action(ದತ್ತಾಂಶ->dialog, (XtCallbackProc)cancel_callback, ದತ್ತಾಂಶ);

    // Preset selection data.

    ದತ್ತಾಂಶ->ನಿರ್ಗಮಿಸು = False;
    ದತ್ತಾಂಶ->ಒಳ_pixels= True;
    ದತ್ತಾಂಶ->sel[ENCODING] = ಖಾಲಿ;
    ದತ್ತಾಂಶ->sel[NAME] = ಖಾಲಿ;
    ದತ್ತಾಂಶ->sel[STYLE] = ಖಾಲಿ;
    ದತ್ತಾಂಶ->sel[ಗಾತ್ರ_4] = ಖಾಲಿ;
    ದತ್ತಾಂಶ->font_name = ಖಾಲಿ;

    // set up current font parameters
    ಆದರೆ (current && current[0] != '\0')
    {
	ಪೂರ್ಣಾಂಕ	    i;
	ಅಕ್ಷರ	    **ಹೆಸರುಗಳು;

	ಹೆಸರುಗಳು = XListFonts(XtDisplay(form), (ಅಕ್ಷರ *) current, 1, &i);

	ಆದರೆ (i != 0)
	{
	    ಅಕ್ಷರ namebuf[ತಾತ್ಕಾಲಿಕ_BUF_ಗಾತ್ರ];
	    ಅಕ್ಷರ stylebuf[ತಾತ್ಕಾಲಿಕ_BUF_ಗಾತ್ರ];
	    ಅಕ್ಷರ sizebuf[ತಾತ್ಕಾಲಿಕ_BUF_ಗಾತ್ರ];
	    ಅಕ್ಷರ encodingbuf[ತಾತ್ಕಾಲಿಕ_BUF_ಗಾತ್ರ];
	    ಅಕ್ಷರ *ಸಿಕ್ಕಿತು;

	    ಸಿಕ್ಕಿತು = ಹೆಸರುಗಳು[0];

	    name_part(ಸಿಕ್ಕಿತು, namebuf);
	    style_part(ಸಿಕ್ಕಿತು, stylebuf);
	    ಗಾತ್ರ_part(ಸಿಕ್ಕಿತು, sizebuf, ದತ್ತಾಂಶ->ಒಳ_pixels);
	    encoding_part(ಸಿಕ್ಕಿತು, encodingbuf);

	    ಆದರೆ (*namebuf != NUL
		    && *stylebuf != NUL
		    && *sizebuf != NUL
		    && *encodingbuf != NUL)
	    {
		ದತ್ತಾಂಶ->sel[NAME] = XtNewString(namebuf);
		ದತ್ತಾಂಶ->sel[STYLE] = XtNewString(stylebuf);
		ದತ್ತಾಂಶ->sel[ಗಾತ್ರ_4] = XtNewString(sizebuf);
		ದತ್ತಾಂಶ->sel[ENCODING] = XtNewString(encodingbuf);
		ದತ್ತಾಂಶ->font_name = XtNewString(ಹೆಸರುಗಳು[0]);
		display_sample(ದತ್ತಾಂಶ);
		Xm_ಪಠ್ಯ_ಗಣ_String(ದತ್ತಾಂಶ->name, ದತ್ತಾಂಶ->font_name);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		// We can't preset a symbolic name, which isn't a full font
		// description. Therefore we just behave the same way as if the
		// user didn't have selected anything thus far.
		//
		// Unfortunately there is no known way to expand an abbreviated
		// font name.
		ದತ್ತಾಂಶ->font_name = ಖಾಲಿ;
	    }
	}
	XFree_Font_ಹೆಸರುಗಳು(ಹೆಸರುಗಳು);
    }

    fill_lists(NONE, ದತ್ತಾಂಶ);

    // Unfortunately LessTif doesn't align the list widget's properly.  I don't
    // have currently any idea how to fix this problem.
    Xt_Manage_ಮಗು(ದತ್ತಾಂಶ->list[NAME]);
    Xt_Manage_ಮಗು(ದತ್ತಾಂಶ->list[STYLE]);
    Xt_Manage_ಮಗು(ದತ್ತಾಂಶ->list[ಗಾತ್ರ_4]);
    Xt_Manage_ಮಗು(ದತ್ತಾಂಶ->encoding_menu);
    manage_centered(form);

    // modal event loop
    ಆಗ (!ದತ್ತಾಂಶ->ನಿರ್ಗಮಿಸು)
	XtAppProcessEvent(Xt_Widget_ಗಮ್ಯ_Application_Context(ದತ್ತಾಂಶ->dialog),
							(Xt_ಒಳಹರಿವು_ಮುಖವಾಡ)XtIMAll);

    ಆದರೆ (ದತ್ತಾಂಶ->old)
    {
	XFreeFont(XtDisplay(ದತ್ತಾಂಶ->dialog),  ದತ್ತಾಂಶ->old);
	XmFontListFree(ದತ್ತಾಂಶ->old_list);
    }
    XtDestroyWidget(ದತ್ತಾಂಶ->dialog);

    gui_motif_synch_fonts();

    ಹಿಂತಿರುಗಿ (char_u *) ದತ್ತಾಂಶ->font_name;
}
