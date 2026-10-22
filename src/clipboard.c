/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

/*
 * clipboard.c: Functions to handle the clipboard. Additionally contains the
 *		clipboard provider code, which is separate from the main
 *		clipboard code.
 */

#అన్తర్భావ "vim.h"

#నిర్వచితమైతే FEAT_CYGWIN_WIN32_CLIPBOARD
# define WIN32_LEAN_AND_సగటు
# include <windows.h>
# include "winclip.pro"
#అయితే_ముగింపు

// Functions for copying and pasting text between applications.
// This is always included in a GUI version, but may also be included when the
// clipboard and mouse is available to a terminal version such as xterm.
// Note: there are some more functions in ops.c that handle selection stuff.
//
// Also note that the majority of functions here deal with the X 'primary'
// (visible - for Visual mode use) selection, and only that. There are no
// versions of these for the 'clipboard' selection, as Visual mode has no use
// for them.


#నిర్వచితమైతే FEAT_CLIPBOARD_PROVIDER
స్థిర పూర్ణాంకం clip_provider_is_అందుబాటు(char_u *provider);
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_CLIPBOARD)

# ఒకవేళ defined(FEAT_WAYLAND_CLIPBOARD)

#  include "wayland.h"

// Represents either the regular or primary selection
రకనిర్వచనం నిర్మాణం {
    char_u		*contents;	// Non-null if we own selection,
					// contains the data to send to other
					// clients.
    vwl_దత్తాంశం_ఆధారం_T	*ఆధారం;	// Non-NULL if we own the selection,
					// else NULL if we don't.
    vwl_దత్తాంశం_offer_T	*offer;		// Current offer for the selection

    bool		own_success;	// Used by clip_wl_own_selection()
    bool		అందుబాటు;	// If selection is ready to serve/use

    // These may point to the same proxy as the other selection
    vwl_దత్తాంశం_device_manager_T	*manager;
    vwl_దత్తాంశం_device_T		*device;
} clip_wl_selection_T;

// Represents the clipboard for the global Wayland connection, for the chosen
// seat (using the 'wl_seat' option)
రకనిర్వచనం నిర్మాణం {
    vwl_seat_T *seat;

    clip_wl_selection_T regular;
    clip_wl_selection_T primary;
} clip_wl_T;

// Mime types we support sending and receiving
// Mimes with a lower index in the array are prioritized first when we are
// receiving data.
స్థిర స్థిరరాశి అక్షరం *supported_mimes[] = {
    VIMENC_MIMETYPE_NAME,
    VIMENC_ATOM_NAME,
    VIM_MIMETYPE_NAME,
    VIM_ATOM_NAME,
    "text/plain;charset=utf-8",
    "text/plain",
    "UTF8_STRING",
    "STRING",
    "TEXT"
};

clip_wl_T clip_wl;

స్థిర శూన్య
clip_wl_receive_దత్తాంశం(Clipboard_T *cbd, స్థిరరాశి అక్షరం *mime_type, పూర్ణాంకం fd);
స్థిర శూన్య clip_wl_request_selection(Clipboard_T *cbd);
స్థిర పూర్ణాంకం clip_wl_own_selection(Clipboard_T *cbd);
స్థిర శూన్య clip_wl_lose_selection(Clipboard_T *cbd);
స్థిర శూన్య clip_wl_సమితి_selection(Clipboard_T *cbd);

#  ఒకవేళ defined(USE_SYSTEM)
స్థిర bool clip_wl_owner_exists(Clipboard_T *cbd);
#  endif

# endif // FEAT_WAYLAND_CLIPBOARD

/*
 * Selection stuff using Visual mode, for cutting and pasting text to other
 * windows.
 */

/*
 * Call this to initialise the clipboard.  Pass it FALSE if the clipboard code
 * is included, but the clipboard can not be used, or TRUE if the clipboard can
 * be used.  Eg unix may call this with FALSE, then call it again with TRUE if
 * the GUI starts.
 */
    శూన్య
clip_init(పూర్ణాంకం can_use)
{
    Clipboard_T *cb;

    cb = &clip_నక్షత్రం;
    కొరకు (;;)
    {
	// No need to init again if cbd is already available
	ఒకవేళ (can_use && cb->అందుబాటు)
	    వెళ్ళు skip;

	cb->అందుబాటు  = can_use;
	cb->owned      = FALSE;
	cb->ప్రారంభం.lnum = 0;
	cb->ప్రారంభం.నిలువు  = 0;
	cb->చివర.lnum   = 0;
	cb->చివర.నిలువు    = 0;
	cb->state      = SELECT_CLEARED;

skip:
	ఒకవేళ (cb == &clip_plus)
	    ఆపు;
	cb = &clip_plus;
    }
}

/*
 * Check whether the VIsual area has changed, and if so try to become the owner
 * of the selection, and free any old converted selection we may still have
 * lying around.  If the VIsual mode has ended, make a copy of what was
 * selected so we can still give it to others.	Will probably have to make sure
 * this is called whenever VIsual mode is ended.
 */
    శూన్య
clip_update_selection(Clipboard_T *clip)
{
    pos_T	    ప్రారంభం, చివర;

    // If visual mode is only due to a redo command ("."), then ignore it
    ఒకవేళ (!redo_VIsual_busy && VIsual_చురుకైన && (State & బహుళకం_సాధారణ))
    {
	ఒకవేళ (LT_POS(VIsual, curwin->w_cursor))
	{
	    ప్రారంభం = VIsual;
	    చివర = curwin->w_cursor;
	    ఒకవేళ (has_mbyte)
		చివర.నిలువు += (*mb_ptr2len)(ml_get_cursor()) - 1;
	}
	లేకపోతే
	{
	    ప్రారంభం = curwin->w_cursor;
	    చివర = VIsual;
	}
	ఒకవేళ (!EQUAL_POS(clip->ప్రారంభం, ప్రారంభం)
		|| !EQUAL_POS(clip->చివర, చివర)
		|| clip->vmode != VIsual_బహుళకం)
	{
	    clip_clear_selection(clip);
	    clip->ప్రారంభం = ప్రారంభం;
	    clip->చివర = చివర;
	    clip->vmode = VIsual_బహుళకం;
	    clip_free_selection(clip);
	    clip_own_selection(clip);
	    clip_gen_సమితి_selection(clip);
	}
    }
}

    స్థిర పూర్ణాంకం
clip_gen_own_selection(Clipboard_T *cbd)
{
# ఒకవేళ defined(FEAT_XCLIPBOARD) || defined(FEAT_WAYLAND_CLIPBOARD)
#  ifdef FEAT_GUI
    ఒకవేళ (gui.లోపల_use)
	తిరిగి clip_mch_own_selection(cbd);
    లేకపోతే
#  endif
    {
	ఒకవేళ (clipmethod == CLIPMETHOD_WAYLAND)
	{
#  ifdef FEAT_WAYLAND_CLIPBOARD
	    తిరిగి clip_wl_own_selection(cbd);
#  endif
	}
	లేకపోతే ఒకవేళ (clipmethod == CLIPMETHOD_X11)
	{
#  ifdef FEAT_XCLIPBOARD
	    తిరిగి clip_xterm_own_selection(cbd);
#  endif
	}
    }
    తిరిగి FAIL;
# లేకపోతే
    తిరిగి clip_mch_own_selection(cbd);
# endif
}

    శూన్య
clip_own_selection(Clipboard_T *cbd)
{
    /*
     * Also want to check somehow that we are reading from the keyboard rather
     * than a mapping etc.
     */
# ఒకవేళ defined(FEAT_X11) || defined(FEAT_WAYLAND_CLIPBOARD)
    // Always own the selection, we might have lost it without being
    // notified, e.g. during a ":sh" command.
    ఒకవేళ (cbd->అందుబాటు)
    {
	పూర్ణాంకం was_owned = cbd->owned;

	cbd->owned = (clip_gen_own_selection(cbd) == OK);
	ఒకవేళ (!was_owned && (cbd == &clip_నక్షత్రం || cbd == &clip_plus))
	{
	    // May have to show a different kind of highlighting for the
	    // selected area.  There is no specific redraw command for this,
	    // just redraw all windows on the current buffer.
	    ఒకవేళ (cbd->owned
		    && (get_real_state() == బహుళకం_VISUAL
					    || get_real_state() == బహుళకం_SELECT)
		    && (cbd == &clip_నక్షత్రం ? clip_isautosel_నక్షత్రం()
						      : clip_isautosel_plus())
		    && HL_ATTR(HLF_V) != HL_ATTR(HLF_VNC))
		redraw_curbuf_later(UPD_INVERTED_ALL);
	}
    }
# లేకపోతే
    // Only own the clipboard when we didn't own it yet.
    ఒకవేళ (!cbd->owned && cbd->అందుబాటు)
	cbd->owned = (clip_gen_own_selection(cbd) == OK);
# endif
}

    స్థిర శూన్య
clip_gen_lose_selection(Clipboard_T *cbd)
{
# ఒకవేళ defined(FEAT_XCLIPBOARD) || defined(FEAT_WAYLAND_CLIPBOARD)
#  ifdef FEAT_GUI
    ఒకవేళ (gui.లోపల_use)
	clip_mch_lose_selection(cbd);
    లేకపోతే
#  endif
    {
	ఒకవేళ (clipmethod == CLIPMETHOD_WAYLAND)
	{
#  ifdef FEAT_WAYLAND_CLIPBOARD
	    clip_wl_lose_selection(cbd);
#  endif
	}
	లేకపోతే ఒకవేళ (clipmethod == CLIPMETHOD_X11)
	{
#  ifdef FEAT_XCLIPBOARD
	    clip_xterm_lose_selection(cbd);
#  endif
	}
    }
# లేకపోతే
    clip_mch_lose_selection(cbd);
# endif
}

    శూన్య
clip_lose_selection(Clipboard_T *cbd)
{
# ifdef FEAT_X11
    పూర్ణాంకం	    was_owned = cbd->owned;
# endif
    పూర్ణాంకం     visual_selection = FALSE;

    ఒకవేళ (cbd == &clip_నక్షత్రం || cbd == &clip_plus)
	visual_selection = TRUE;

    clip_free_selection(cbd);
    cbd->owned = FALSE;
    ఒకవేళ (visual_selection)
	clip_clear_selection(cbd);
    clip_gen_lose_selection(cbd);
# ifdef FEAT_X11
    ఒకవేళ (visual_selection)
    {
	// May have to show a different kind of highlighting for the selected
	// area.  There is no specific redraw command for this, just redraw all
	// windows on the current buffer.
	ఒకవేళ (was_owned
		&& (get_real_state() == బహుళకం_VISUAL
		    || get_real_state() == బహుళకం_SELECT)
		&& (cbd == &clip_నక్షత్రం ?
		    clip_isautosel_నక్షత్రం() : clip_isautosel_plus())
		&& HL_ATTR(HLF_V) != HL_ATTR(HLF_VNC)
		&& !exiting)
	{
	    update_curbuf(UPD_INVERTED_ALL);
	    setcursor();
	    cursor_on();
	    బహిర్గతం_flush_cursor(TRUE, FALSE);
	}
    }
# endif
}

    స్థిర శూన్య
clip_copy_selection(Clipboard_T *clip)
{
    ఒకవేళ (VIsual_చురుకైన && (State & బహుళకం_సాధారణ) && clip->అందుబాటు)
    {
	clip_update_selection(clip);
	clip_free_selection(clip);
	clip_own_selection(clip);
	ఒకవేళ (clip->owned)
	    clip_get_selection(clip);
	clip_gen_సమితి_selection(clip);
    }
}

/*
 * Save and restore clip_unnamed before doing possibly many changes. This
 * prevents accessing the clipboard very often which might slow down Vim
 * considerably.
 */
స్థిర పూర్ణాంకం global_change_లెక్క = 0; // if set, inside a start_global_changes
స్థిర పూర్ణాంకం clipboard_needs_update = FALSE; // clipboard needs to be updated
స్థిర పూర్ణాంకం clip_did_సమితి_selection = TRUE;

/*
 * Save clip_unnamed and reset it.
 */
    శూన్య
ప్రారంభం_global_changes(శూన్య)
{
    ఒకవేళ (++global_change_లెక్క > 1)
	తిరిగి;
    clip_unnamed_saved = clip_unnamed;
    clipboard_needs_update = FALSE;

    ఒకవేళ (clip_did_సమితి_selection)
    {
	clip_unnamed = 0;
	clip_did_సమితి_selection = FALSE;
    }
}

/*
 * Return TRUE if setting the clipboard was postponed, it already contains the
 * right text.
 */
    స్థిర పూర్ణాంకం
is_clipboard_needs_update(శూన్య)
{
    తిరిగి clipboard_needs_update;
}

/*
 * Restore clip_unnamed and set the selection when needed.
 */
    శూన్య
చివర_global_changes(శూన్య)
{
    ఒకవేళ (--global_change_లెక్క > 0)
	// recursive
	తిరిగి;
    ఒకవేళ (!clip_did_సమితి_selection)
    {
	clip_did_సమితి_selection = TRUE;
	clip_unnamed = clip_unnamed_saved;
	clip_unnamed_saved = 0;
	ఒకవేళ (clipboard_needs_update)
	{
	    // only store something in the clipboard,
	    // if we have yanked anything to it
	    ఒకవేళ (clip_unnamed & CLIP_UNNAMED)
	    {
		clip_own_selection(&clip_నక్షత్రం);
		clip_gen_సమితి_selection(&clip_నక్షత్రం);
	    }
	    ఒకవేళ (clip_unnamed & CLIP_UNNAMED_PLUS)
	    {
		clip_own_selection(&clip_plus);
		clip_gen_సమితి_selection(&clip_plus);
	    }
	}
    }
    clipboard_needs_update = FALSE;
}

/*
 * Called when Visual mode is ended: update the selection.
 */
    శూన్య
clip_auto_select(శూన్య)
{
    ఒకవేళ (clip_isautosel_నక్షత్రం())
	clip_copy_selection(&clip_నక్షత్రం);
    ఒకవేళ (clip_isautosel_plus())
	clip_copy_selection(&clip_plus);
}

/*
 * Return TRUE if automatic selection of Visual area is desired for the *
 * register.
 */
    పూర్ణాంకం
clip_isautosel_నక్షత్రం(శూన్య)
{
# ifdef FEAT_CLIPBOARD_PROVIDER
    ఒకవేళ (clipmethod == CLIPMETHOD_PROVIDER)
	తిరిగి false;
# endif
# ifdef FEAT_GUI
    ఒకవేళ (gui.లోపల_use)
	తిరిగి vim_strchr(p_go, GO_ASEL) != ఖాళీ
	    && vim_strchr(p_go, GO_ASELPLUS) == ఖాళీ;
# endif
    తిరిగి clip_autoselect_నక్షత్రం;
}

/*
 * Return TRUE if automatic selection of Visual area is desired for the +
 * register.
 */
    పూర్ణాంకం
clip_isautosel_plus(శూన్య)
{
# ifdef FEAT_CLIPBOARD_PROVIDER
    ఒకవేళ (clipmethod == CLIPMETHOD_PROVIDER)
	తిరిగి false;
# endif
# ifdef FEAT_GUI
    ఒకవేళ (gui.లోపల_use)
	తిరిగి vim_strchr(p_go, GO_ASELPLUS) != ఖాళీ;
# endif
    తిరిగి clip_autoselect_plus;
}


/*
 * Stuff for general mouse selection, without using Visual mode.
 */

/*
 * Compare two screen positions ala strcmp()
 */
    స్థిర పూర్ణాంకం
clip_compare_pos(
    పూర్ణాంకం		row1,
    పూర్ణాంకం		col1,
    పూర్ణాంకం		row2,
    పూర్ణాంకం		col2)
{
    ఒకవేళ (row1 > row2) తిరిగి(1);
    ఒకవేళ (row1 < row2) తిరిగి(-1);
    ఒకవేళ (col1 > col2) తిరిగి(1);
    ఒకవేళ (col1 < col2) తిరిగి(-1);
    తిరిగి(0);
}

// "how" flags for clip_invert_area()
# define CLIP_CLEAR	1
# define CLIP_సమితి	2
# define CLIP_TOGGLE	3

/*
 * Invert or un-invert a rectangle of the screen.
 * "invert" is true if the result is inverted.
 */
    స్థిర శూన్య
clip_invert_rectangle(
	Clipboard_T	*cbd వాడని,
	పూర్ణాంకం		అడ్డువరుస_arg,
	పూర్ణాంకం		నిలువు_arg,
	పూర్ణాంకం		ఎత్తు_arg,
	పూర్ణాంకం		వెడల్పు_arg,
	పూర్ణాంకం		invert)
{
    పూర్ణాంకం		అడ్డువరుస = అడ్డువరుస_arg;
    పూర్ణాంకం		నిలువు = నిలువు_arg;
    పూర్ణాంకం		ఎత్తు = ఎత్తు_arg;
    పూర్ణాంకం		వెడల్పు = వెడల్పు_arg;

# ifdef FEAT_PROP_POPUP
    // this goes on top of all popup windows
    screen_zindex = CLIP_ZINDEX;

    ఒకవేళ (నిలువు < cbd->min_నిలువు)
    {
	వెడల్పు -= cbd->min_నిలువు - నిలువు;
	నిలువు = cbd->min_నిలువు;
    }
    ఒకవేళ (వెడల్పు > cbd->max_నిలువు - నిలువు)
	వెడల్పు = cbd->max_నిలువు - నిలువు;
    ఒకవేళ (అడ్డువరుస < cbd->min_అడ్డువరుస)
    {
	ఎత్తు -= cbd->min_అడ్డువరుస - అడ్డువరుస;
	అడ్డువరుస = cbd->min_అడ్డువరుస;
    }
    ఒకవేళ (ఎత్తు > cbd->max_అడ్డువరుస - అడ్డువరుస + 1)
	ఎత్తు = cbd->max_అడ్డువరుస - అడ్డువరుస + 1;
# endif
# ifdef FEAT_GUI
    ఒకవేళ (gui.లోపల_use)
	gui_mch_invert_rectangle(అడ్డువరుస, నిలువు, ఎత్తు, వెడల్పు);
    లేకపోతే
# endif
	screen_draw_rectangle(అడ్డువరుస, నిలువు, ఎత్తు, వెడల్పు, invert, FALSE);
# ifdef FEAT_PROP_POPUP
    screen_zindex = 0;
# endif
}

/*
 * Invert a region of the display between a starting and ending row and column
 * Values for "how":
 * CLIP_CLEAR:  undo inversion
 * CLIP_SET:    set inversion
 * CLIP_TOGGLE: set inversion if pos1 < pos2, undo inversion otherwise.
 * 0: invert (GUI only).
 */
    స్థిర శూన్య
clip_invert_area(
	Clipboard_T	*cbd,
	పూర్ణాంకం		row1,
	పూర్ణాంకం		col1,
	పూర్ణాంకం		row2,
	పూర్ణాంకం		col2,
	పూర్ణాంకం		how)
{
    పూర్ణాంకం		invert = FALSE;
    పూర్ణాంకం		max_నిలువు;

# ifdef FEAT_PROP_POPUP
    max_నిలువు = cbd->max_నిలువు - 1;
# లేకపోతే
    max_నిలువు = Columns - 1;
# endif

    ఒకవేళ (how == CLIP_సమితి)
	invert = TRUE;

    // Swap the from and to positions so the from is always before
    ఒకవేళ (clip_compare_pos(row1, col1, row2, col2) > 0)
    {
	పూర్ణాంకం tmp_అడ్డువరుస, tmp_నిలువు;

	tmp_అడ్డువరుస = row1;
	tmp_నిలువు = col1;
	row1	= row2;
	col1	= col2;
	row2	= tmp_అడ్డువరుస;
	col2	= tmp_నిలువు;
    }
    లేకపోతే ఒకవేళ (how == CLIP_TOGGLE)
	invert = TRUE;

    // If all on the same line, do it the easy way
    ఒకవేళ (row1 == row2)
    {
	clip_invert_rectangle(cbd, row1, col1, 1, col2 - col1, invert);
    }
    లేకపోతే
    {
	// Handle a piece of the first line
	ఒకవేళ (col1 > 0)
	{
	    clip_invert_rectangle(cbd, row1, col1, 1,
						  (పూర్ణాంకం)Columns - col1, invert);
	    row1++;
	}

	// Handle a piece of the last line
	ఒకవేళ (col2 < max_నిలువు)
	{
	    clip_invert_rectangle(cbd, row2, 0, 1, col2, invert);
	    row2--;
	}

	// Handle the rectangle that's left
	ఒకవేళ (row2 >= row1)
	    clip_invert_rectangle(cbd, row1, 0, row2 - row1 + 1,
							 (పూర్ణాంకం)Columns, invert);
    }
}

/*
 * Start, continue or end a modeless selection.  Used when editing the
 * command-line, in the cmdline window and when the mouse is in a popup window.
 */
    శూన్య
clip_modeless(పూర్ణాంకం button, పూర్ణాంకం is_click, పూర్ణాంకం is_drag)
{
    పూర్ణాంకం		repeat;

    repeat = ((clip_నక్షత్రం.బహుళకం == SELECT_బహుళకం_CHAR
		|| clip_నక్షత్రం.బహుళకం == SELECT_బహుళకం_పంక్తి)
					      && (mod_ముసుగు & MOD_ముసుగు_2_CLICK))
	    || (clip_నక్షత్రం.బహుళకం == SELECT_బహుళకం_మాట
					     && (mod_ముసుగు & MOD_ముసుగు_3_CLICK));
    ఒకవేళ (is_click && button == MOUSE_కుడి)
    {
	// Right mouse button: If there was no selection, start one.
	// Otherwise extend the existing selection.
	ఒకవేళ (clip_నక్షత్రం.state == SELECT_CLEARED)
	    clip_ప్రారంభం_selection(mouse_నిలువు, mouse_అడ్డువరుస, FALSE);
	clip_process_selection(button, mouse_నిలువు, mouse_అడ్డువరుస, repeat);
    }
    లేకపోతే ఒకవేళ (is_click)
	clip_ప్రారంభం_selection(mouse_నిలువు, mouse_అడ్డువరుస, repeat);
    లేకపోతే ఒకవేళ (is_drag)
    {
	// Don't try extending a selection if there isn't one.  Happens when
	// button-down is in the cmdline and them moving mouse upwards.
	ఒకవేళ (clip_నక్షత్రం.state != SELECT_CLEARED)
	    clip_process_selection(button, mouse_నిలువు, mouse_అడ్డువరుస, repeat);
    }
    లేకపోతే // release
	clip_process_selection(MOUSE_RELEASE, mouse_నిలువు, mouse_అడ్డువరుస, FALSE);
}

/*
 * Update the currently selected region by adding and/or subtracting from the
 * beginning or end and inverting the changed area(s).
 */
    స్థిర శూన్య
clip_update_modeless_selection(
    Clipboard_T    *cb,
    పూర్ణాంకం		    row1,
    పూర్ణాంకం		    col1,
    పూర్ణాంకం		    row2,
    పూర్ణాంకం		    col2)
{
    // See if we changed at the beginning of the selection
    ఒకవేళ (row1 != cb->ప్రారంభం.lnum || col1 != (పూర్ణాంకం)cb->ప్రారంభం.నిలువు)
    {
	clip_invert_area(cb, row1, col1, (పూర్ణాంకం)cb->ప్రారంభం.lnum, cb->ప్రారంభం.నిలువు,
								 CLIP_TOGGLE);
	cb->ప్రారంభం.lnum = row1;
	cb->ప్రారంభం.నిలువు  = col1;
    }

    // See if we changed at the end of the selection
    ఒకవేళ (row2 != cb->చివర.lnum || col2 != (పూర్ణాంకం)cb->చివర.నిలువు)
    {
	clip_invert_area(cb, (పూర్ణాంకం)cb->చివర.lnum, cb->చివర.నిలువు, row2, col2,
								 CLIP_TOGGLE);
	cb->చివర.lnum = row2;
	cb->చివర.నిలువు  = col2;
    }
}

/*
 * Find the starting and ending positions of the word at the given row and
 * column.  Only white-separated words are recognized here.
 */
# define CHAR_CLASS(c)	(c <= ' ' ? ' ' : vim_iswordc(c))

    స్థిర శూన్య
clip_get_మాట_boundaries(Clipboard_T *cb, పూర్ణాంకం అడ్డువరుస, పూర్ణాంకం నిలువు)
{
    పూర్ణాంకం		ప్రారంభం_class;
    పూర్ణాంకం		తాత్కాలిక_నిలువు;
    char_u	*p;
    పూర్ణాంకం		mboff;

    ఒకవేళ (అడ్డువరుస >= screen_అడ్డువరుసలు || నిలువు >= screen_Columns || Screen_పంక్తులు == ఖాళీ)
	తిరిగి;

    p = Screen_పంక్తులు + పంక్తి_స్థానభ్రంశం[అడ్డువరుస];
    // Correct for starting in the right half of a double-wide char
    ఒకవేళ (enc_dbcs != 0)
	నిలువు -= dbcs_screen_తల_off(p, p + నిలువు);
    లేకపోతే ఒకవేళ (enc_utf8 && p[నిలువు] == 0)
	--నిలువు;
    ప్రారంభం_class = CHAR_CLASS(p[నిలువు]);

    తాత్కాలిక_నిలువు = నిలువు;
    కొరకు ( ; తాత్కాలిక_నిలువు > 0; తాత్కాలిక_నిలువు--)
	ఒకవేళ (enc_dbcs != 0
		   && (mboff = dbcs_screen_తల_off(p, p + తాత్కాలిక_నిలువు - 1)) > 0)
	    తాత్కాలిక_నిలువు -= mboff;
	లేకపోతే ఒకవేళ (CHAR_CLASS(p[తాత్కాలిక_నిలువు - 1]) != ప్రారంభం_class
		&& !(enc_utf8 && p[తాత్కాలిక_నిలువు - 1] == 0))
	    ఆపు;
    cb->మాట_ప్రారంభం_నిలువు = తాత్కాలిక_నిలువు;

    తాత్కాలిక_నిలువు = నిలువు;
    కొరకు ( ; తాత్కాలిక_నిలువు < screen_Columns; తాత్కాలిక_నిలువు++)
	ఒకవేళ (enc_dbcs != 0 && dbcs_ptr2cells(p + తాత్కాలిక_నిలువు) == 2)
	    ++తాత్కాలిక_నిలువు;
	లేకపోతే ఒకవేళ (CHAR_CLASS(p[తాత్కాలిక_నిలువు]) != ప్రారంభం_class
		&& !(enc_utf8 && p[తాత్కాలిక_నిలువు] == 0))
	    ఆపు;
    cb->మాట_చివర_నిలువు = తాత్కాలిక_నిలువు;
}

/*
 * Find the column position for the last non-whitespace character on the given
 * line at or before start_col.
 */
    స్థిర పూర్ణాంకం
clip_get_పంక్తి_చివర(Clipboard_T *cbd వాడని, పూర్ణాంకం అడ్డువరుస)
{
    పూర్ణాంకం	    i;

    ఒకవేళ (అడ్డువరుస >= screen_అడ్డువరుసలు || Screen_పంక్తులు == ఖాళీ)
	తిరిగి 0;
    కొరకు (i =
# ifdef FEAT_PROP_POPUP
	    cbd->max_నిలువు;
# లేకపోతే
	    screen_Columns;
# endif
			    i > 0; i--)
	ఒకవేళ (Screen_పంక్తులు[పంక్తి_స్థానభ్రంశం[అడ్డువరుస] + i - 1] != ' ')
	    ఆపు;
    తిరిగి i;
}

/*
 * Start the selection
 */
    శూన్య
clip_ప్రారంభం_selection(పూర్ణాంకం నిలువు, పూర్ణాంకం అడ్డువరుస, పూర్ణాంకం repeated_click)
{
    Clipboard_T	*cb = &clip_నక్షత్రం;
# ifdef FEAT_PROP_POPUP
    win_T	*wp;
    పూర్ణాంకం		అడ్డువరుస_cp = అడ్డువరుస;
    పూర్ణాంకం		నిలువు_cp = నిలువు;

    wp = mouse_కనుగొను_win(&అడ్డువరుస_cp, &నిలువు_cp, కనుగొను_POPUP);
    ఒకవేళ (wp != ఖాళీ && WIN_IS_POPUP(wp)
				  && popup_is_లోపల_scrollbar(wp, అడ్డువరుస_cp, నిలువు_cp))
	// click or double click in scrollbar does not start a selection
	తిరిగి;
# endif

    ఒకవేళ (cb->state == SELECT_పూర్తి)
	clip_clear_selection(cb);

    అడ్డువరుస = check_అడ్డువరుస(అడ్డువరుస);
    నిలువు = check_నిలువు(నిలువు);
    నిలువు = mb_fix_నిలువు(నిలువు, అడ్డువరుస);

    cb->ప్రారంభం.lnum  = అడ్డువరుస;
    cb->ప్రారంభం.నిలువు   = నిలువు;
    cb->చివర	    = cb->ప్రారంభం;
    cb->origin_అడ్డువరుస  = (short_u)cb->ప్రారంభం.lnum;
    cb->state	    = SELECT_లోపల_పురోగతి;
# ifdef FEAT_PROP_POPUP
    ఒకవేళ (wp != ఖాళీ && WIN_IS_POPUP(wp))
    {
	// Click in a popup window restricts selection to that window,
	// excluding the border.
	cb->min_నిలువు = wp->w_wincol + wp->w_popup_border[3];
	cb->max_నిలువు = wp->w_wincol + popup_వెడల్పు(wp)
				 - wp->w_popup_border[1] - wp->w_has_scrollbar;
	ఒకవేళ (cb->max_నిలువు > screen_Columns)
	    cb->max_నిలువు = screen_Columns;
	cb->min_అడ్డువరుస = wp->w_winrow + wp->w_popup_border[0];
	cb->max_అడ్డువరుస = wp->w_winrow + popup_ఎత్తు(wp) - 1
						   - wp->w_popup_border[2];
    }
    లేకపోతే
    {
	cb->min_నిలువు = 0;
	cb->max_నిలువు = screen_Columns;
	cb->min_అడ్డువరుస = 0;
	cb->max_అడ్డువరుస = screen_అడ్డువరుసలు;
    }
# endif

    ఒకవేళ (repeated_click)
    {
	ఒకవేళ (++cb->బహుళకం > SELECT_బహుళకం_పంక్తి)
	    cb->బహుళకం = SELECT_బహుళకం_CHAR;
    }
    లేకపోతే
	cb->బహుళకం = SELECT_బహుళకం_CHAR;

# ifdef FEAT_GUI
    // clear the cursor until the selection is made
    ఒకవేళ (gui.లోపల_use)
	gui_undraw_cursor();
# endif

    ఎంపిక (cb->బహుళకం)
    {
	స్థితి SELECT_బహుళకం_CHAR:
	    cb->origin_ప్రారంభం_నిలువు = cb->ప్రారంభం.నిలువు;
	    cb->మాట_చివర_నిలువు = clip_get_పంక్తి_చివర(cb, (పూర్ణాంకం)cb->ప్రారంభం.lnum);
	    ఆపు;

	స్థితి SELECT_బహుళకం_మాట:
	    clip_get_మాట_boundaries(cb, (పూర్ణాంకం)cb->ప్రారంభం.lnum, cb->ప్రారంభం.నిలువు);
	    cb->origin_ప్రారంభం_నిలువు = cb->మాట_ప్రారంభం_నిలువు;
	    cb->origin_చివర_నిలువు	 = cb->మాట_చివర_నిలువు;

	    clip_invert_area(cb, (పూర్ణాంకం)cb->ప్రారంభం.lnum, cb->మాట_ప్రారంభం_నిలువు,
			    (పూర్ణాంకం)cb->చివర.lnum, cb->మాట_చివర_నిలువు, CLIP_సమితి);
	    cb->ప్రారంభం.నిలువు = cb->మాట_ప్రారంభం_నిలువు;
	    cb->చివర.నిలువు   = cb->మాట_చివర_నిలువు;
	    ఆపు;

	స్థితి SELECT_బహుళకం_పంక్తి:
	    clip_invert_area(cb, (పూర్ణాంకం)cb->ప్రారంభం.lnum, 0, (పూర్ణాంకం)cb->ప్రారంభం.lnum,
			    (పూర్ణాంకం)Columns, CLIP_సమితి);
	    cb->ప్రారంభం.నిలువు = 0;
	    cb->చివర.నిలువు   = Columns;
	    ఆపు;
    }

    cb->prev = cb->ప్రారంభం;

# ifdef DEBUG_SELECTION
    రాయి("Selection started at (%ld,%d)\n", cb->ప్రారంభం.lnum, cb->ప్రారంభం.నిలువు);
# endif
}

/*
 * Continue processing the selection
 */
    శూన్య
clip_process_selection(
    పూర్ణాంకం		button,
    పూర్ణాంకం		నిలువు,
    పూర్ణాంకం		అడ్డువరుస,
    int_u	repeated_click)
{
    Clipboard_T	*cb = &clip_నక్షత్రం;
    పూర్ణాంకం		diff;
    పూర్ణాంకం		slen = 1;	// cursor shape width

    ఒకవేళ (button == MOUSE_RELEASE)
    {
	ఒకవేళ (cb->state != SELECT_లోపల_పురోగతి)
	    తిరిగి;

	// Check to make sure we have something selected
	ఒకవేళ (cb->ప్రారంభం.lnum == cb->చివర.lnum && cb->ప్రారంభం.నిలువు == cb->చివర.నిలువు)
	{
# ifdef FEAT_GUI
	    ఒకవేళ (gui.లోపల_use)
		gui_update_cursor(FALSE, FALSE);
# endif
	    cb->state = SELECT_CLEARED;
	    తిరిగి;
	}

# ifdef DEBUG_SELECTION
	రాయి("Selection ended: (%ld,%d) to (%ld,%d)\n", cb->ప్రారంభం.lnum,
		cb->ప్రారంభం.నిలువు, cb->చివర.lnum, cb->చివర.నిలువు);
# endif
	ఒకవేళ (clip_isautosel_నక్షత్రం() || clip_isautosel_plus()
		|| (
# ifdef FEAT_GUI
		    gui.లోపల_use ? (vim_strchr(p_go, GO_ASELML) != ఖాళీ) :
# endif
		    clip_autoselectml))
	    clip_copy_modeless_selection(FALSE);
# ifdef FEAT_GUI
	ఒకవేళ (gui.లోపల_use)
	    gui_update_cursor(FALSE, FALSE);
# endif

	cb->state = SELECT_పూర్తి;
	తిరిగి;
    }

    అడ్డువరుస = check_అడ్డువరుస(అడ్డువరుస);
    నిలువు = check_నిలువు(నిలువు);
    నిలువు = mb_fix_నిలువు(నిలువు, అడ్డువరుస);

    ఒకవేళ (నిలువు == (పూర్ణాంకం)cb->prev.నిలువు && అడ్డువరుస == cb->prev.lnum && !repeated_click)
	తిరిగి;

    /*
     * When extending the selection with the right mouse button, swap the
     * start and end if the position is before half the selection
     */
    ఒకవేళ (cb->state == SELECT_పూర్తి && button == MOUSE_కుడి)
    {
	/*
	 * If the click is before the start, or the click is inside the
	 * selection and the start is the closest side, set the origin to the
	 * end of the selection.
	 */
	ఒకవేళ (clip_compare_pos(అడ్డువరుస, నిలువు, (పూర్ణాంకం)cb->ప్రారంభం.lnum, cb->ప్రారంభం.నిలువు) < 0
		|| (clip_compare_pos(అడ్డువరుస, నిలువు,
					   (పూర్ణాంకం)cb->చివర.lnum, cb->చివర.నిలువు) < 0
		    && (((cb->ప్రారంభం.lnum == cb->చివర.lnum
			    && cb->చివర.నిలువు - నిలువు > నిలువు - cb->ప్రారంభం.నిలువు))
			|| ((diff = (cb->చివర.lnum - అడ్డువరుస) -
						   (అడ్డువరుస - cb->ప్రారంభం.lnum)) > 0
			    || (diff == 0 && నిలువు < (పూర్ణాంకం)(cb->ప్రారంభం.నిలువు +
							 cb->చివర.నిలువు) / 2)))))
	{
	    cb->origin_అడ్డువరుస = (short_u)cb->చివర.lnum;
	    cb->origin_ప్రారంభం_నిలువు = cb->చివర.నిలువు - 1;
	    cb->origin_చివర_నిలువు = cb->చివర.నిలువు;
	}
	లేకపోతే
	{
	    cb->origin_అడ్డువరుస = (short_u)cb->ప్రారంభం.lnum;
	    cb->origin_ప్రారంభం_నిలువు = cb->ప్రారంభం.నిలువు;
	    cb->origin_చివర_నిలువు = cb->ప్రారంభం.నిలువు;
	}
	ఒకవేళ (cb->బహుళకం == SELECT_బహుళకం_మాట && !repeated_click)
	    cb->బహుళకం = SELECT_బహుళకం_CHAR;
    }

    // set state, for when using the right mouse button
    cb->state = SELECT_లోపల_పురోగతి;

# ifdef DEBUG_SELECTION
    రాయి("Selection extending to (%d,%d)\n", అడ్డువరుస, నిలువు);
# endif

    ఒకవేళ (repeated_click && ++cb->బహుళకం > SELECT_బహుళకం_పంక్తి)
	cb->బహుళకం = SELECT_బహుళకం_CHAR;

    ఎంపిక (cb->బహుళకం)
    {
	స్థితి SELECT_బహుళకం_CHAR:
	    // If we're on a different line, find where the line ends
	    ఒకవేళ (అడ్డువరుస != cb->prev.lnum)
		cb->మాట_చివర_నిలువు = clip_get_పంక్తి_చివర(cb, అడ్డువరుస);

	    // See if we are before or after the origin of the selection
	    ఒకవేళ (clip_compare_pos(అడ్డువరుస, నిలువు, cb->origin_అడ్డువరుస,
						   cb->origin_ప్రారంభం_నిలువు) >= 0)
	    {
		ఒకవేళ (నిలువు >= (పూర్ణాంకం)cb->మాట_చివర_నిలువు)
		    clip_update_modeless_selection(cb, cb->origin_అడ్డువరుస,
			    cb->origin_ప్రారంభం_నిలువు, అడ్డువరుస, (పూర్ణాంకం)Columns);
		లేకపోతే
		{
		    ఒకవేళ (has_mbyte && mb_lefthalve(అడ్డువరుస, నిలువు))
			slen = 2;
		    clip_update_modeless_selection(cb, cb->origin_అడ్డువరుస,
			    cb->origin_ప్రారంభం_నిలువు, అడ్డువరుస, నిలువు + slen);
		}
	    }
	    లేకపోతే
	    {
		ఒకవేళ (has_mbyte
			&& mb_lefthalve(cb->origin_అడ్డువరుస, cb->origin_ప్రారంభం_నిలువు))
		    slen = 2;
		ఒకవేళ (నిలువు >= (పూర్ణాంకం)cb->మాట_చివర_నిలువు)
		    clip_update_modeless_selection(cb, అడ్డువరుస, cb->మాట_చివర_నిలువు,
			    cb->origin_అడ్డువరుస, cb->origin_ప్రారంభం_నిలువు + slen);
		లేకపోతే
		    clip_update_modeless_selection(cb, అడ్డువరుస, నిలువు,
			    cb->origin_అడ్డువరుస, cb->origin_ప్రారంభం_నిలువు + slen);
	    }
	    ఆపు;

	స్థితి SELECT_బహుళకం_మాట:
	    // If we are still within the same word, do nothing
	    ఒకవేళ (అడ్డువరుస == cb->prev.lnum && నిలువు >= (పూర్ణాంకం)cb->మాట_ప్రారంభం_నిలువు
		    && నిలువు < (పూర్ణాంకం)cb->మాట_చివర_నిలువు && !repeated_click)
		తిరిగి;

	    // Get new word boundaries
	    clip_get_మాట_boundaries(cb, అడ్డువరుస, నిలువు);

	    // Handle being after the origin point of selection
	    ఒకవేళ (clip_compare_pos(అడ్డువరుస, నిలువు, cb->origin_అడ్డువరుస,
		    cb->origin_ప్రారంభం_నిలువు) >= 0)
		clip_update_modeless_selection(cb, cb->origin_అడ్డువరుస,
			cb->origin_ప్రారంభం_నిలువు, అడ్డువరుస, cb->మాట_చివర_నిలువు);
	    లేకపోతే
		clip_update_modeless_selection(cb, అడ్డువరుస, cb->మాట_ప్రారంభం_నిలువు,
			cb->origin_అడ్డువరుస, cb->origin_చివర_నిలువు);
	    ఆపు;

	స్థితి SELECT_బహుళకం_పంక్తి:
	    ఒకవేళ (అడ్డువరుస == cb->prev.lnum && !repeated_click)
		తిరిగి;

	    ఒకవేళ (clip_compare_pos(అడ్డువరుస, నిలువు, cb->origin_అడ్డువరుస,
		    cb->origin_ప్రారంభం_నిలువు) >= 0)
		clip_update_modeless_selection(cb, cb->origin_అడ్డువరుస, 0, అడ్డువరుస,
			(పూర్ణాంకం)Columns);
	    లేకపోతే
		clip_update_modeless_selection(cb, అడ్డువరుస, 0, cb->origin_అడ్డువరుస,
			(పూర్ణాంకం)Columns);
	    ఆపు;
    }

    cb->prev.lnum = అడ్డువరుస;
    cb->prev.నిలువు  = నిలువు;

# ifdef DEBUG_SELECTION
	రాయి("Selection is: (%ld,%d) to (%ld,%d)\n", cb->ప్రారంభం.lnum,
		cb->ప్రారంభం.నిలువు, cb->చివర.lnum, cb->చివర.నిలువు);
# endif
}

# ఒకవేళ defined(FEAT_GUI)
/*
 * Redraw part of the selection if character at "row,col" is inside of it.
 * Only used for the GUI.
 */
    శూన్య
clip_may_redraw_selection(పూర్ణాంకం అడ్డువరుస, పూర్ణాంకం నిలువు, పూర్ణాంకం len)
{
    పూర్ణాంకం		ప్రారంభం = నిలువు;
    పూర్ణాంకం		చివర = నిలువు + len;

    ఒకవేళ (clip_నక్షత్రం.state != SELECT_CLEARED
	    && అడ్డువరుస >= clip_నక్షత్రం.ప్రారంభం.lnum
	    && అడ్డువరుస <= clip_నక్షత్రం.చివర.lnum)
    {
	ఒకవేళ (అడ్డువరుస == clip_నక్షత్రం.ప్రారంభం.lnum && ప్రారంభం < (పూర్ణాంకం)clip_నక్షత్రం.ప్రారంభం.నిలువు)
	    ప్రారంభం = clip_నక్షత్రం.ప్రారంభం.నిలువు;
	ఒకవేళ (అడ్డువరుస == clip_నక్షత్రం.చివర.lnum && చివర > (పూర్ణాంకం)clip_నక్షత్రం.చివర.నిలువు)
	    చివర = clip_నక్షత్రం.చివర.నిలువు;
	ఒకవేళ (చివర > ప్రారంభం)
	    clip_invert_area(&clip_నక్షత్రం, అడ్డువరుస, ప్రారంభం, అడ్డువరుస, చివర, 0);
    }
}
# endif

/*
 * Called from outside to clear selected region from the display
 */
    శూన్య
clip_clear_selection(Clipboard_T *cbd)
{

    ఒకవేళ (cbd->state == SELECT_CLEARED)
	తిరిగి;

    clip_invert_area(cbd, (పూర్ణాంకం)cbd->ప్రారంభం.lnum, cbd->ప్రారంభం.నిలువు,
				 (పూర్ణాంకం)cbd->చివర.lnum, cbd->చివర.నిలువు, CLIP_CLEAR);
    cbd->state = SELECT_CLEARED;
}

/*
 * Clear the selection if any lines from "row1" to "row2" are inside of it.
 */
    శూన్య
clip_may_clear_selection(పూర్ణాంకం row1, పూర్ణాంకం row2)
{
    ఒకవేళ (clip_నక్షత్రం.state == SELECT_పూర్తి
	    && row2 >= clip_నక్షత్రం.ప్రారంభం.lnum
	    && row1 <= clip_నక్షత్రం.చివర.lnum)
	clip_clear_selection(&clip_నక్షత్రం);
}

/*
 * Called before the screen is scrolled up or down.  Adjusts the line numbers
 * of the selection.  Call with big number when clearing the screen.
 */
    శూన్య
clip_scroll_selection(
    పూర్ణాంకం	    అడ్డువరుసలు_2)		// negative for scroll down
{
    పూర్ణాంకం	    lnum;

    ఒకవేళ (clip_నక్షత్రం.state == SELECT_CLEARED)
	తిరిగి;

    lnum = clip_నక్షత్రం.ప్రారంభం.lnum - అడ్డువరుసలు_2;
    ఒకవేళ (lnum <= 0)
	clip_నక్షత్రం.ప్రారంభం.lnum = 0;
    లేకపోతే ఒకవేళ (lnum >= screen_అడ్డువరుసలు)	// scrolled off of the screen
	clip_నక్షత్రం.state = SELECT_CLEARED;
    లేకపోతే
	clip_నక్షత్రం.ప్రారంభం.lnum = lnum;

    lnum = clip_నక్షత్రం.చివర.lnum - అడ్డువరుసలు_2;
    ఒకవేళ (lnum < 0)			// scrolled off of the screen
	clip_నక్షత్రం.state = SELECT_CLEARED;
    లేకపోతే ఒకవేళ (lnum >= screen_అడ్డువరుసలు)
	clip_నక్షత్రం.చివర.lnum = screen_అడ్డువరుసలు - 1;
    లేకపోతే
	clip_నక్షత్రం.చివర.lnum = lnum;
}

/*
 * Copy the currently selected area into the '*' or '+' register so it will be
 * available for pasting.
 * When "both" is TRUE also copy to the other register.
 */
    శూన్య
clip_copy_modeless_selection(పూర్ణాంకం both వాడని)
{
    // The info for the modeless selection is stored in '*' register, however if
    // we are using the '+' register for modeless autoselect, we copy to
    // clip_plus instead while using the info in clip_star.
    Clipboard_T *cbd = clip_isautosel_plus() ? &clip_plus : &clip_నక్షత్రం;
    char_u	*బఫరు;
    char_u	*bufp;
    పూర్ణాంకం		అడ్డువరుస;
    పూర్ణాంకం		ప్రారంభం_నిలువు;
    పూర్ణాంకం		చివర_నిలువు;
    పూర్ణాంకం		పంక్తి_చివర_నిలువు;
    పూర్ణాంకం		కూడు_newline_flag = FALSE;
    పూర్ణాంకం		len;
    char_u	*p;
    పూర్ణాంకం		row1 = clip_నక్షత్రం.ప్రారంభం.lnum;
    పూర్ణాంకం		col1 = clip_నక్షత్రం.ప్రారంభం.నిలువు;
    పూర్ణాంకం		row2 = clip_నక్షత్రం.చివర.lnum;
    పూర్ణాంకం		col2 = clip_నక్షత్రం.చివర.నిలువు;

    // Can't use ScreenLines unless initialized
    ఒకవేళ (Screen_పంక్తులు == ఖాళీ)
	తిరిగి;

    /*
     * Make sure row1 <= row2, and if row1 == row2 that col1 <= col2.
     */
    ఒకవేళ (row1 > row2)
    {
	అడ్డువరుస = row1; row1 = row2; row2 = అడ్డువరుస;
	అడ్డువరుస = col1; col1 = col2; col2 = అడ్డువరుస;
    }
    లేకపోతే ఒకవేళ (row1 == row2 && col1 > col2)
    {
	అడ్డువరుస = col1; col1 = col2; col2 = అడ్డువరుస;
    }
# ifdef FEAT_PROP_POPUP
    ఒకవేళ (col1 < clip_నక్షత్రం.min_నిలువు)
	col1 = clip_నక్షత్రం.min_నిలువు;
    ఒకవేళ (col2 > clip_నక్షత్రం.max_నిలువు)
	col2 = clip_నక్షత్రం.max_నిలువు;
    ఒకవేళ (row1 > clip_నక్షత్రం.max_అడ్డువరుస || row2 < clip_నక్షత్రం.min_అడ్డువరుస)
	తిరిగి;
    ఒకవేళ (row1 < clip_నక్షత్రం.min_అడ్డువరుస)
	row1 = clip_నక్షత్రం.min_అడ్డువరుస;
    ఒకవేళ (row2 > clip_నక్షత్రం.max_అడ్డువరుస)
	row2 = clip_నక్షత్రం.max_అడ్డువరుస;
# endif
    // correct starting point for being on right half of double-wide char
    p = Screen_పంక్తులు + పంక్తి_స్థానభ్రంశం[row1];
    ఒకవేళ (enc_dbcs != 0)
	col1 -= (*mb_తల_off)(p, p + col1);
    లేకపోతే ఒకవేళ (enc_utf8 && p[col1] == 0)
	--col1;

    // Create a temporary buffer for storing the text
    len = (row2 - row1 + 1) * Columns + 1;
    ఒకవేళ (enc_dbcs != 0)
	len *= 2;	// max. 2 bytes per display cell
    లేకపోతే ఒకవేళ (enc_utf8)
	len *= MB_MAXBYTES;
    // A newline is added after each row
    len += row2 - row1 + 2;
    బఫరు = alloc(len);
    ఒకవేళ (బఫరు == ఖాళీ)	    // out of memory
	తిరిగి;

    // Process each row in the selection
    కొరకు (bufp = బఫరు, అడ్డువరుస = row1; అడ్డువరుస <= row2; అడ్డువరుస++)
    {
	ఒకవేళ (అడ్డువరుస == row1)
	    ప్రారంభం_నిలువు = col1;
	లేకపోతే
# ifdef FEAT_PROP_POPUP
	    ప్రారంభం_నిలువు = clip_నక్షత్రం.min_నిలువు;
# లేకపోతే
	    ప్రారంభం_నిలువు = 0;
# endif

	ఒకవేళ (అడ్డువరుస == row2)
	    చివర_నిలువు = col2;
	లేకపోతే
# ifdef FEAT_PROP_POPUP
	    చివర_నిలువు = clip_నక్షత్రం.max_నిలువు;
# లేకపోతే
	    చివర_నిలువు = Columns;
# endif

	పంక్తి_చివర_నిలువు = clip_get_పంక్తి_చివర(&clip_నక్షత్రం, అడ్డువరుస);

	// See if we need to nuke some trailing whitespace
	ఒకవేళ (చివర_నిలువు >=
# ifdef FEAT_PROP_POPUP
		clip_నక్షత్రం.max_నిలువు
# లేకపోతే
		Columns
# endif
		    && (అడ్డువరుస < row2 || చివర_నిలువు > పంక్తి_చివర_నిలువు))
	{
	    // Get rid of trailing whitespace
	    చివర_నిలువు = పంక్తి_చివర_నిలువు;
	    ఒకవేళ (చివర_నిలువు < ప్రారంభం_నిలువు)
		చివర_నిలువు = ప్రారంభం_నిలువు;

	    // If the last line extended to the end, add an extra newline
	    ఒకవేళ (అడ్డువరుస == row2)
		కూడు_newline_flag = TRUE;
	}

	// If after the first row, we need to always add a newline
	ఒకవేళ (అడ్డువరుస > row1 && !పంక్తి_Wraps[అడ్డువరుస - 1])
	    *bufp++ = NL;

	// Safetey check for in case resizing went wrong
	ఒకవేళ (అడ్డువరుస < screen_అడ్డువరుసలు && చివర_నిలువు <= screen_Columns)
	{
	    ఒకవేళ (enc_dbcs != 0)
	    {
		పూర్ణాంకం	i;

		p = Screen_పంక్తులు + పంక్తి_స్థానభ్రంశం[అడ్డువరుస];
		కొరకు (i = ప్రారంభం_నిలువు; i < చివర_నిలువు; ++i)
		    ఒకవేళ (enc_dbcs == DBCS_JPNU && p[i] == 0x8e)
		    {
			// single-width double-byte char
			*bufp++ = 0x8e;
			*bufp++ = ScreenLines2[పంక్తి_స్థానభ్రంశం[అడ్డువరుస] + i];
		    }
		    లేకపోతే
		    {
			*bufp++ = p[i];
			ఒకవేళ (MB_BYTE2LEN(p[i]) == 2)
			    *bufp++ = p[++i];
		    }
	    }
	    లేకపోతే ఒకవేళ (enc_utf8)
	    {
		పూర్ణాంకం	off;
		పూర్ణాంకం	i;

		off = పంక్తి_స్థానభ్రంశం[అడ్డువరుస];
		కొరకు (i = ప్రారంభం_నిలువు; i < చివర_నిలువు; ++i)
		{
		    // The base character is either in ScreenLinesUC[] or
		    // ScreenLines[].
		    ఒకవేళ (Screen_పంక్తులు_UC[off + i] == 0)
			*bufp++ = Screen_పంక్తులు[off + i];
		    లేకపోతే
			// Includes the composing characters.
			bufp += utfc_char2bytes(off + i, bufp);
		    // Skip right half of double-wide character.
		    ఒకవేళ (Screen_పంక్తులు[off + i + 1] == 0)
			++i;
		}
	    }
	    లేకపోతే
	    {
		STRNCPY(bufp, Screen_పంక్తులు + పంక్తి_స్థానభ్రంశం[అడ్డువరుస] + ప్రారంభం_నిలువు,
							 చివర_నిలువు - ప్రారంభం_నిలువు);
		bufp += చివర_నిలువు - ప్రారంభం_నిలువు;
	    }
	}
    }

    // Add a newline at the end if the selection ended there
    ఒకవేళ (కూడు_newline_flag)
	*bufp++ = NL;

    // First cleanup any old selection and become the owner.
    clip_free_selection(cbd);
    clip_own_selection(cbd);

    // Yank the text into the '*' register.
    clip_yank_selection(MCHAR, బఫరు, (దీర్ఘ)(bufp - బఫరు), cbd);

    // Make the register contents available to the outside world.
    clip_gen_సమితి_selection(cbd);

# ifdef FEAT_X11
    ఒకవేళ (both)
    {
	Clipboard_T *other = cbd == &clip_నక్షత్రం ? &clip_plus : &clip_నక్షత్రం;
	// Do the same for the '+' register.
	clip_free_selection(other);
	clip_own_selection(other);
	clip_yank_selection(MCHAR, బఫరు, (దీర్ఘ)(bufp - బఫరు), other);
	clip_gen_సమితి_selection(other);
    }
# endif
    vim_free(బఫరు);
}

    శూన్య
clip_gen_సమితి_selection(Clipboard_T *cbd)
{
    ఒకవేళ (!clip_did_సమితి_selection)
    {
	// Updating postponed, so that accessing the system clipboard won't
	// hang Vim when accessing it many times (e.g. on a :g command).
	ఒకవేళ ((cbd == &clip_plus && (clip_unnamed_saved & CLIP_UNNAMED_PLUS))
		|| (cbd == &clip_నక్షత్రం && (clip_unnamed_saved & CLIP_UNNAMED)))
	{
	    clipboard_needs_update = TRUE;
	    తిరిగి;
	}
    }
# ఒకవేళ defined(FEAT_XCLIPBOARD) || defined(FEAT_WAYLAND_CLIPBOARD)
#  ifdef FEAT_GUI
    ఒకవేళ (gui.లోపల_use)
	clip_mch_సమితి_selection(cbd);
    లేకపోతే
#  endif
    {
	ఒకవేళ (clipmethod == CLIPMETHOD_WAYLAND)
	{
#  ifdef FEAT_WAYLAND_CLIPBOARD
	    clip_wl_సమితి_selection(cbd);
#  endif
	}
	లేకపోతే ఒకవేళ (clipmethod == CLIPMETHOD_X11)
	{
#  ifdef FEAT_XCLIPBOARD
	    clip_xterm_సమితి_selection(cbd);
#  endif
	}
    }
# లేకపోతే
    clip_mch_సమితి_selection(cbd);
# endif
}

    స్థిర శూన్య
clip_gen_request_selection(Clipboard_T *cbd)
{
# ఒకవేళ defined(FEAT_XCLIPBOARD) || defined(FEAT_WAYLAND_CLIPBOARD)
#  ifdef FEAT_GUI
    ఒకవేళ (gui.లోపల_use)
	clip_mch_request_selection(cbd);
    లేకపోతే
#  endif
    {
	ఒకవేళ (clipmethod == CLIPMETHOD_WAYLAND)
	{
#  ifdef FEAT_WAYLAND_CLIPBOARD
	    clip_wl_request_selection(cbd);
#  endif
	}
	లేకపోతే ఒకవేళ (clipmethod == CLIPMETHOD_X11)
	{
#  ifdef FEAT_XCLIPBOARD
	    clip_xterm_request_selection(cbd);
#  endif
	}
    }
# లేకపోతే
    clip_mch_request_selection(cbd);
# endif
}

# ఒకవేళ (defined(FEAT_X11) && defined(FEAT_XCLIPBOARD) && defined(USE_SYSTEM))
    స్థిర పూర్ణాంకం
clip_x11_owner_exists(Clipboard_T *cbd)
{
    తిరిగి XGetSelectionOwner(X_DISPLAY, cbd->sel_atom) != None;
}
# endif

# ఒకవేళ (defined(FEAT_X11) || defined(FEAT_WAYLAND_CLIPBOARD)) \
	&& defined(USE_SYSTEM)
    పూర్ణాంకం
clip_gen_owner_exists(Clipboard_T *cbd వాడని)
{
#  ఒకవేళ defined(FEAT_XCLIPBOARD) || defined(FEAT_WAYLAND_CLIPBOARD)
#   ifdef FEAT_GUI_GTK
    ఒకవేళ (gui.లోపల_use)
	తిరిగి clip_gtk_owner_exists(cbd);
    లేకపోతే
#   endif
    {
	ఒకవేళ (clipmethod == CLIPMETHOD_WAYLAND)
	{
#   ifdef FEAT_WAYLAND_CLIPBOARD
	    తిరిగి clip_wl_owner_exists(cbd);
#   endif
	}
	లేకపోతే ఒకవేళ (clipmethod == CLIPMETHOD_X11)
	{
#   ifdef FEAT_XCLIPBOARD
	    తిరిగి clip_x11_owner_exists(cbd);
#   endif
	}
	లేకపోతే
	    తిరిగి FALSE;
    }
#  లేకపోతే
    తిరిగి TRUE;
#  endif
}
# endif

/*
 * Stuff for the X clipboard.  Shared between VMS and Unix.
 */

# ఒకవేళ defined(FEAT_XCLIPBOARD) || defined(FEAT_GUI_X11)
#  include <X11/Xatom.h>
#  include <X11/Intrinsic.h>

/*
 * Open the application context (if it hasn't been opened yet).
 * Used for Motif GUI and the xterm clipboard.
 */
    శూన్య
open_app_context(శూన్య)
{
    ఒకవేళ (app_context == ఖాళీ)
    {
	XtToolkitInitialize();
	app_context = XtCreateApplicationContext();
    }
}

స్థిర Atom	vim_atom;	// Vim's own special selection format
స్థిర Atom	vimenc_atom;	// Vim's extended selection format
స్థిర Atom	vim_mt_atom;	// Vim's own special selection format (in mime
				// type format)
స్థిర Atom	vimenc_mt_atom;	// Vim's extended selection format (in mime type
				// format)
స్థిర Atom	utf8_atom;
స్థిర Atom	compound_పాఠ్యం_atom;
స్థిర Atom	పాఠ్యం_atom;
స్థిర Atom	targets_atom;
స్థిర Atom	timestamp_atom;	// Used to get a timestamp

    శూన్య
x11_అమరిక_atoms(Display *dpy)
{
    vim_atom	       = XInternAtom(dpy, VIM_ATOM_NAME,   False);
    vimenc_atom	       = XInternAtom(dpy, VIMENC_ATOM_NAME,False);
    vim_mt_atom	       = XInternAtom(dpy, VIM_MIMETYPE_NAME,   False);
    vimenc_mt_atom     = XInternAtom(dpy, VIMENC_MIMETYPE_NAME,False);
    utf8_atom	       = XInternAtom(dpy, "UTF8_STRING",   False);
    compound_పాఠ్యం_atom = XInternAtom(dpy, "COMPOUND_TEXT", False);
    పాఠ్యం_atom	       = XInternAtom(dpy, "TEXT",	   False);
    targets_atom       = XInternAtom(dpy, "TARGETS",	   False);
    clip_నక్షత్రం.sel_atom = XA_PRIMARY;
    clip_plus.sel_atom = XInternAtom(dpy, "CLIPBOARD",	   False);
    timestamp_atom     = XInternAtom(dpy, "TIMESTAMP",	   False);
}

/*
 * X Selection stuff, for cutting and pasting text to other windows.
 */

    స్థిర Boolean
clip_x11_convert_selection_cb(
    Widget	w వాడని,
    Atom	*sel_atom,
    Atom	*లక్ష్యం,
    Atom	*type,
    Xt_సూచిక	*విలువ,
    long_u	*నిడివి,
    పూర్ణాంకం		*format)
{
    స్థిర char_u   *save_ఫలితం = ఖాళీ;
    స్థిర long_u   save_నిడివి = 0;
    char_u	    *string;
    పూర్ణాంకం		    motion_type;
    Clipboard_T    *cbd;
    పూర్ణాంకం		    i;

    ఒకవేళ (*sel_atom == clip_plus.sel_atom)
	cbd = &clip_plus;
    లేకపోతే
	cbd = &clip_నక్షత్రం;

    ఒకవేళ (!cbd->owned)
	తిరిగి False;	    // Shouldn't ever happen

    // requestor wants to know what target types we support
    ఒకవేళ (*లక్ష్యం == targets_atom)
    {
	స్థిర Atom array[9];

	*విలువ = (Xt_సూచిక)array;
	i = 0;
	array[i++] = targets_atom;
	array[i++] = vimenc_atom;
	array[i++] = vim_atom;
	array[i++] = vimenc_mt_atom;
	array[i++] = vim_mt_atom;
	ఒకవేళ (enc_utf8)
	    array[i++] = utf8_atom;
	array[i++] = XA_STRING;
	array[i++] = పాఠ్యం_atom;
	array[i++] = compound_పాఠ్యం_atom;

	*type = XA_ATOM;
	// This used to be: *format = sizeof(Atom) * 8; but that caused
	// crashes on 64 bit machines. (Peter Derr)
	*format = 32;
	*నిడివి = i;
	తిరిగి True;
    }

    ఒకవేళ (       *లక్ష్యం != XA_STRING
	    && *లక్ష్యం != vimenc_atom
	    && *లక్ష్యం != vimenc_mt_atom
	    && (*లక్ష్యం != utf8_atom || !enc_utf8)
	    && *లక్ష్యం != vim_atom
	    && *లక్ష్యం != vim_mt_atom
	    && *లక్ష్యం != పాఠ్యం_atom
	    && *లక్ష్యం != compound_పాఠ్యం_atom)
	తిరిగి False;

    clip_get_selection(cbd);
    motion_type = clip_convert_selection(&string, నిడివి, cbd);
    ఒకవేళ (motion_type < 0)
	తిరిగి False;

    // For our own format, the first byte contains the motion type
    ఒకవేళ (*లక్ష్యం == vim_atom || *లక్ష్యం == vim_mt_atom)
	(*నిడివి)++;

    // Our own format with encoding: motion 'encoding' NUL text
    ఒకవేళ (*లక్ష్యం == vimenc_atom || *లక్ష్యం == vimenc_mt_atom)
	*నిడివి += STRLEN(p_enc) + 2;

    ఒకవేళ (save_నిడివి < *నిడివి || save_నిడివి / 2 >= *నిడివి)
	*విలువ = XtRealloc((అక్షరం *)save_ఫలితం, (Cardinal)*నిడివి + 1);
    లేకపోతే
	*విలువ = save_ఫలితం;
    ఒకవేళ (*విలువ == ఖాళీ)
    {
	vim_free(string);
	తిరిగి False;
    }
    save_ఫలితం = (char_u *)*విలువ;
    save_నిడివి = *నిడివి;

    ఒకవేళ (*లక్ష్యం == XA_STRING || (*లక్ష్యం == utf8_atom && enc_utf8))
    {
	mch_memmove(save_ఫలితం, string, (size_t)(*నిడివి));
	*type = *లక్ష్యం;
    }
    లేకపోతే ఒకవేళ (*లక్ష్యం == compound_పాఠ్యం_atom || *లక్ష్యం == పాఠ్యం_atom)
    {
	XTextProperty	పాఠ్యం_prop;
	అక్షరం		*string_nt = (అక్షరం *)save_ఫలితం;
	పూర్ణాంకం		conv_ఫలితం;

	// create NUL terminated string which XmbTextListToTextProperty wants
	mch_memmove(string_nt, string, (size_t)*నిడివి);
	string_nt[*నిడివి] = NUL;
	conv_ఫలితం = Xmb_పాఠ్యం_List_గమ్యం_పాఠ్యం_Property(X_DISPLAY, &string_nt,
					   1, XCompound_పాఠ్యం_Style, &పాఠ్యం_prop);
	ఒకవేళ (conv_ఫలితం != Success)
	{
	    vim_free(string);
	    తిరిగి False;
	}
	*విలువ = (Xt_సూచిక)(పాఠ్యం_prop.విలువ);	//    from plain text
	*నిడివి = పాఠ్యం_prop.nitems;
	*type = compound_పాఠ్యం_atom;
	XtFree((అక్షరం *)save_ఫలితం);
	save_ఫలితం = (char_u *)*విలువ;
	save_నిడివి = *నిడివి;
    }
    లేకపోతే ఒకవేళ (*లక్ష్యం == vimenc_atom || *లక్ష్యం == vimenc_mt_atom)
    {
	పూర్ణాంకం l = STRLEN(p_enc);

	save_ఫలితం[0] = motion_type;
	STRCPY(save_ఫలితం + 1, p_enc);
	mch_memmove(save_ఫలితం + l + 2, string, (size_t)(*నిడివి - l - 2));
	ఒకవేళ (*లక్ష్యం == vimenc_atom)
	    *type = vimenc_atom;
	లేకపోతే
	    *type = vimenc_mt_atom;
    }
    లేకపోతే
    {
	save_ఫలితం[0] = motion_type;
	mch_memmove(save_ఫలితం + 1, string, (size_t)(*నిడివి - 1));
	ఒకవేళ (*లక్ష్యం == vim_atom)
	    *type = vim_atom;
	లేకపోతే
	    *type = vim_mt_atom;
    }
    *format = 8;	    // 8 bits per char
    vim_free(string);
    తిరిగి True;
}

    స్థిర శూన్య
clip_x11_lose_ownership_cb(Widget w వాడని, Atom *sel_atom)
{
    ఒకవేళ (*sel_atom == clip_plus.sel_atom)
	clip_lose_selection(&clip_plus);
    లేకపోతే
	clip_lose_selection(&clip_నక్షత్రం);
}

    స్థిర శూన్య
clip_x11_notify_cb(Widget w వాడని, Atom *sel_atom వాడని, Atom *లక్ష్యం వాడని)
{
    // To prevent automatically freeing the selection value.
}

/*
 * Property callback to get a timestamp for XtOwnSelection.
 */
#  ఒకవేళ defined(FEAT_X11) && defined(FEAT_XCLIPBOARD)
    స్థిర శూన్య
clip_x11_timestamp_cb(
    Widget	w,
    Xt_సూచిక	n వాడని,
    XEvent	*event,
    Boolean	*cont వాడని)
{
    Atom	    actual_type;
    పూర్ణాంకం		    format;
    గుర్తులేని  దీర్ఘ  nitems, బైట్లు_after;
    గుర్తులేని అక్షరం   *prop=ఖాళీ;
    XPropertyEvent  *xproperty=&event->xproperty;

    // Must be a property notify, state can't be Delete (True), has to be
    // one of the supported selection types.
    ఒకవేళ (event->type != PropertyNotify || xproperty->state
	    || (xproperty->atom != clip_నక్షత్రం.sel_atom
				    && xproperty->atom != clip_plus.sel_atom))
	తిరిగి;

    ఒకవేళ (XGet_కిటికీ_Property(xproperty->display, xproperty->కిటికీ_2,
	  xproperty->atom, 0, 0, False, timestamp_atom, &actual_type, &format,
						&nitems, &బైట్లు_after, &prop))
	తిరిగి;

    ఒకవేళ (prop)
	XFree(prop);

    // Make sure the property type is "TIMESTAMP" and it's 32 bits.
    ఒకవేళ (actual_type != timestamp_atom || format != 32)
	తిరిగి;

    // Get the selection, using the event timestamp.
    ఒకవేళ (XtOwnSelection(w, xproperty->atom, xproperty->సమయం,
	    clip_x11_convert_selection_cb, clip_x11_lose_ownership_cb,
	    clip_x11_notify_cb) == OK)
    {
	// Set the "owned" flag now, there may have been a call to
	// lose_ownership_cb in between.
	ఒకవేళ (xproperty->atom == clip_plus.sel_atom)
	    clip_plus.owned = TRUE;
	లేకపోతే
	    clip_నక్షత్రం.owned = TRUE;
    }
}

    శూన్య
x11_అమరిక_selection(Widget w)
{
    Xt_కూడు_Event_Handler(w, Property_Change_ముసుగు, False,
	    /*(XtEventHandler)*/clip_x11_timestamp_cb, (Xt_సూచిక)ఖాళీ);
}
#  endif

    స్థిర శూన్య
clip_x11_request_selection_cb(
    Widget	w వాడని,
    Xt_సూచిక	success,
    Atom	*sel_atom,
    Atom	*type,
    Xt_సూచిక	విలువ,
    long_u	*నిడివి,
    పూర్ణాంకం		*format)
{
    పూర్ణాంకం		motion_type = MAUTO;
    long_u	len;
    char_u	*p;
    అక్షరం	**పాఠ్యం_list = ఖాళీ;
    Clipboard_T	*cbd;
    char_u	*tmpbuf = ఖాళీ;

    ఒకవేళ (*sel_atom == clip_plus.sel_atom)
	cbd = &clip_plus;
    లేకపోతే
	cbd = &clip_నక్షత్రం;

    ఒకవేళ (విలువ == ఖాళీ || *నిడివి == 0)
    {
	clip_free_selection(cbd);	// nothing received, clear register
	*(పూర్ణాంకం *)success = FALSE;
	తిరిగి;
    }
    p = (char_u *)విలువ;
    len = *నిడివి;
    ఒకవేళ (*type == vim_atom || *type == vim_mt_atom)
    {
	motion_type = *p++;
	len--;
    }

    లేకపోతే ఒకవేళ (*type == vimenc_atom || *type == vimenc_mt_atom)
    {
	char_u		*enc;
	vimconv_T	conv;
	పూర్ణాంకం		convlen;

	motion_type = *p++;
	--len;

	enc = p;
	p += STRLEN(p) + 1;
	len -= p - enc;

	// If the encoding of the text is different from 'encoding', attempt
	// converting it.
	conv.vc_type = CONV_NONE;
	convert_అమరిక(&conv, enc, p_enc);
	ఒకవేళ (conv.vc_type != CONV_NONE)
	{
	    convlen = len;	// Need to use an int here.
	    tmpbuf = string_convert(&conv, p, &convlen);
	    len = convlen;
	    ఒకవేళ (tmpbuf != ఖాళీ)
		p = tmpbuf;
	    convert_అమరిక(&conv, ఖాళీ, ఖాళీ);
	}
    }

    లేకపోతే ఒకవేళ (*type == compound_పాఠ్యం_atom
	    || *type == utf8_atom
	    || (enc_dbcs != 0 && *type == పాఠ్యం_atom))
    {
	XTextProperty	పాఠ్యం_prop;
	పూర్ణాంకం		n_పాఠ్యం = 0;
	పూర్ణాంకం		స్థితి_2;

	పాఠ్యం_prop.విలువ = (గుర్తులేని అక్షరం *)విలువ;
	పాఠ్యం_prop.encoding = *type;
	పాఠ్యం_prop.format = *format;
	పాఠ్యం_prop.nitems = len;
#  ఒకవేళ defined(X_HAVE_UTF8_STRING)
	ఒకవేళ (*type == utf8_atom)
	    స్థితి_2 = Xutf8_పాఠ్యం_Property_గమ్యం_పాఠ్యం_List(X_DISPLAY, &పాఠ్యం_prop,
							 &పాఠ్యం_list, &n_పాఠ్యం);
	లేకపోతే
#  endif
	    స్థితి_2 = Xmb_పాఠ్యం_Property_గమ్యం_పాఠ్యం_List(X_DISPLAY, &పాఠ్యం_prop,
							 &పాఠ్యం_list, &n_పాఠ్యం);
	ఒకవేళ (స్థితి_2 != Success || n_పాఠ్యం < 1)
	{
	    *(పూర్ణాంకం *)success = FALSE;
	    తిరిగి;
	}
	p = (char_u *)పాఠ్యం_list[0];
	len = STRLEN(p);
    }
    clip_yank_selection(motion_type, p, (దీర్ఘ)len, cbd);

    ఒకవేళ (పాఠ్యం_list != ఖాళీ)
	XFreeStringList(పాఠ్యం_list);
    vim_free(tmpbuf);
    XtFree((అక్షరం *)విలువ);
    *(పూర్ణాంకం *)success = TRUE;
}

    శూన్య
clip_x11_request_selection(
    Widget	myShell,
    Display	*dpy,
    Clipboard_T	*cbd)
{
    XEvent	event;
    Atom	type;
    స్థిర పూర్ణాంకం	success;
    పూర్ణాంకం		i;
    time_t	ప్రారంభం_సమయం;
    పూర్ణాంకం		timed_బహిర్గతం = FALSE;

    కొరకు (i = 0; i < 8; i++)
    {
	ఎంపిక (i)
	{
	    స్థితి 0:  type = vimenc_mt_atom;	ఆపు;
	    స్థితి 1:  type = vimenc_atom;	ఆపు;
	    స్థితి 2:  type = vim_mt_atom;	ఆపు;
	    స్థితి 3:  type = vim_atom;		ఆపు;
	    స్థితి 4:  type = utf8_atom;		ఆపు;
	    స్థితి 5:  type = compound_పాఠ్యం_atom; ఆపు;
	    స్థితి 6:  type = పాఠ్యం_atom;		ఆపు;
	    అప్రమేయ: type = XA_STRING;
	}
	ఒకవేళ (type == utf8_atom
#  ఒకవేళ defined(X_HAVE_UTF8_STRING)
		&& !enc_utf8
#  endif
		)
	    // Only request utf-8 when 'encoding' is utf8 and
	    // Xutf8TextPropertyToTextList is available.
	    కొనసాగు;
	success = MAYBE;
	Xt_Get_Selection_విలువ(myShell, cbd->sel_atom, type,
	    clip_x11_request_selection_cb, (Xt_సూచిక)&success, Current_సమయం);

	// Make sure the request for the selection goes out before waiting for
	// a response.
	XFlush(dpy);

	/*
	 * Wait for result of selection request, otherwise if we type more
	 * characters, then they will appear before the one that requested the
	 * paste!  Don't worry, we will catch up with any other events later.
	 */
	ప్రారంభం_సమయం = సమయం(ఖాళీ);
	అయితే (success == MAYBE)
	{
	    ఒకవేళ (XCheckTypedEvent(dpy, PropertyNotify, &event)
		    || XCheckTypedEvent(dpy, SelectionNotify, &event)
		    || XCheckTypedEvent(dpy, SelectionRequest, &event))
	    {
		// This is where clip_x11_request_selection_cb() should be
		// called.  It may actually happen a bit later, so we loop
		// until "success" changes.
		// We may get a SelectionRequest here and if we don't handle
		// it we hang.  KDE klipper does this, for example.
		// We need to handle a PropertyNotify for large selections.
		XtDispatchEvent(&event);
		కొనసాగు;
	    }

	    // Time out after 2 to 3 seconds to avoid that we hang when the
	    // other process doesn't respond.  Note that the SelectionNotify
	    // event may still come later when the selection owner comes back
	    // to life and the text gets inserted unexpectedly.  Don't know
	    // why that happens or how to avoid that :-(.
	    ఒకవేళ (సమయం(ఖాళీ) > ప్రారంభం_సమయం + 2)
	    {
		timed_బహిర్గతం = TRUE;
		ఆపు;
	    }

	    // Do we need this?  Probably not.
	    XSync(dpy, False);

	    // Wait for 1 msec to avoid that we eat up all CPU time.
	    ui_delay(1L, TRUE);
	}

	ఒకవేళ (success == TRUE)
	    తిరిగి;

	// don't do a retry with another type after timing out, otherwise we
	// hang for 15 seconds.
	ఒకవేళ (timed_బహిర్గతం)
	    ఆపు;
    }

    // Final fallback position - use the X CUT_BUFFER0 store
    yank_cut_buffer0(dpy, cbd);
}

    శూన్య
clip_x11_lose_selection(Widget myShell, Clipboard_T *cbd)
{
    XtDisownSelection(myShell, cbd->sel_atom,
				Xt_చివరి_Timestamp_Processed(XtDisplay(myShell)));
}

    పూర్ణాంకం
clip_x11_own_selection(Widget myShell, Clipboard_T *cbd)
{
    // When using the GUI we have proper timestamps, use the one of the last
    // event.  When in the console we don't get events (the terminal gets
    // them), Get the time by a zero-length append, clip_x11_timestamp_cb will
    // be called with the current timestamp.
#  ifdef FEAT_GUI
    ఒకవేళ (gui.లోపల_use)
    {
	ఒకవేళ (XtOwnSelection(myShell, cbd->sel_atom,
	       Xt_చివరి_Timestamp_Processed(XtDisplay(myShell)),
	       clip_x11_convert_selection_cb, clip_x11_lose_ownership_cb,
	       clip_x11_notify_cb) == False)
	    తిరిగి FAIL;
    }
    లేకపోతే
#  endif
    {
	ఒకవేళ (!XChangeProperty(XtDisplay(myShell), Xt_కిటికీ(myShell),
		  cbd->sel_atom, timestamp_atom, 32, Prop_బహుళకం_Append, ఖాళీ, 0))
	    తిరిగి FAIL;
    }
    // Flush is required in a terminal as nothing else is doing it.
    XFlush(XtDisplay(myShell));
    తిరిగి OK;
}

/*
 * Send the current selection to the clipboard.  Do nothing for X because we
 * will fill in the selection only when requested by another app.
 */
    శూన్య
clip_x11_సమితి_selection(Clipboard_T *cbd వాడని)
{
}

# endif

# ఒకవేళ (defined(FEAT_XCLIPBOARD) || defined(FEAT_GUI_X11) || defined(FEAT_GUI_GTK)) \
	&& !defined(USE_GTK4)
/*
 * Get the contents of the X CUT_BUFFER0 and put it in "cbd".
 */
    శూన్య
yank_cut_buffer0(Display *dpy, Clipboard_T *cbd)
{
    పూర్ణాంకం		nbytes = 0;
    char_u	*బఫరు = (char_u *)XFetch_బఫరు(dpy, &nbytes, 0);

    ఒకవేళ (nbytes > 0)
    {
	పూర్ణాంకం  పూర్తి = FALSE;

	// CUT_BUFFER0 is supposed to be always latin1.  Convert to 'enc' when
	// using a multi-byte encoding.  Conversion between two 8-bit
	// character sets usually fails and the text might actually be in
	// 'enc' anyway.
	ఒకవేళ (has_mbyte)
	{
	    char_u	*conv_buf;
	    vimconv_T	vc;

	    vc.vc_type = CONV_NONE;
	    ఒకవేళ (convert_అమరిక(&vc, (char_u *)"latin1", p_enc) == OK)
	    {
		conv_buf = string_convert(&vc, బఫరు, &nbytes);
		ఒకవేళ (conv_buf != ఖాళీ)
		{
		    clip_yank_selection(MCHAR, conv_buf, (దీర్ఘ)nbytes, cbd);
		    vim_free(conv_buf);
		    పూర్తి = TRUE;
		}
		convert_అమరిక(&vc, ఖాళీ, ఖాళీ);
	    }
	}
	ఒకవేళ (!పూర్తి)  // use the text without conversion
	    clip_yank_selection(MCHAR, బఫరు, (దీర్ఘ)nbytes, cbd);
	XFree((శూన్య *)బఫరు);
	ఒకవేళ (p_verbose > 0)
	{
	    verbose_enter();
	    verb_సందేశం(_("Used CUT_BUFFER0 instead of empty selection"));
	    verbose_leave();
	}
    }
}
# endif

/*
 * SELECTION / PRIMARY ('*')
 *
 * Text selection stuff that uses the GUI selection register '*'.  When using a
 * GUI this may be text from another window, otherwise it is the last text we
 * had highlighted with VIsual mode.  With mouse support, clicking the middle
 * button performs the paste, otherwise you will need to do <"*p>. "
 * If not under X, it is synonymous with the clipboard register '+'.
 *
 * X CLIPBOARD ('+')
 *
 * Text selection stuff that uses the GUI clipboard register '+'.
 * Under X, this matches the standard cut/paste buffer CLIPBOARD selection.
 * It will be used for unnamed cut/pasting is 'clipboard' contains "unnamed",
 * otherwise you will need to do <"+p>. "
 * If not under X, it is synonymous with the selection register '*'.
 */

/*
 * Routine to export any final X selection we had to the environment
 * so that the text is still available after Vim has exited. X selections
 * only exist while the owning application exists, so we write to the
 * permanent (while X runs) store CUT_BUFFER0.
 * Dump the CLIPBOARD selection if we own it (it's logically the more
 * 'permanent' of the two), otherwise the PRIMARY one.
 * For now, use a hard-coded sanity limit of 1Mb of data.
 */
# ఒకవేళ defined(FEAT_X11) && defined(FEAT_CLIPBOARD)
    శూన్య
x11_export_final_selection(శూన్య)
{
    Display	*dpy;
    char_u	*str = ఖాళీ;
    long_u	len = 0;
    పూర్ణాంకం		motion_type = -1;

#  ifdef FEAT_GUI
    ఒకవేళ (gui.లోపల_use)
	dpy = X_DISPLAY;
    లేకపోతే
#  endif
#  ifdef FEAT_XCLIPBOARD
	dpy = xterm_dpy;
#  లేకపోతే
	తిరిగి;
#  endif

    // Get selection to export
    ఒకవేళ (clip_plus.owned)
	motion_type = clip_convert_selection(&str, &len, &clip_plus);
    లేకపోతే ఒకవేళ (clip_నక్షత్రం.owned)
	motion_type = clip_convert_selection(&str, &len, &clip_నక్షత్రం);

    // Check it's OK
    ఒకవేళ (dpy != ఖాళీ && str != ఖాళీ && motion_type >= 0
					       && len < 1024*1024 && len > 0)
    {
	పూర్ణాంకం ok = TRUE;

	// The CUT_BUFFER0 is supposed to always contain latin1.  Convert from
	// 'enc' when it is a multi-byte encoding.  When 'enc' is an 8-bit
	// encoding conversion usually doesn't work, so keep the text as-is.
	ఒకవేళ (has_mbyte)
	{
	    vimconv_T	vc;

	    vc.vc_type = CONV_NONE;
	    ఒకవేళ (convert_అమరిక(&vc, p_enc, (char_u *)"latin1") == OK)
	    {
		పూర్ణాంకం	intlen = len;
		char_u	*conv_str;

		vc.vc_fail = TRUE;
		conv_str = string_convert(&vc, str, &intlen);
		len = intlen;
		ఒకవేళ (conv_str != ఖాళీ)
		{
		    vim_free(str);
		    str = conv_str;
		}
		లేకపోతే
		{
		    ok = FALSE;
		}
		convert_అమరిక(&vc, ఖాళీ, ఖాళీ);
	    }
	    లేకపోతే
	    {
		ok = FALSE;
	    }
	}

	// Do not store the string if conversion failed.  Better to use any
	// other selection than garbled text.
	ఒకవేళ (ok)
	{
	    XStore_బఫరు(dpy, (అక్షరం *)str, (పూర్ణాంకం)len, 0);
	    XFlush(dpy);
	}
    }

    vim_free(str);
}
# endif

    శూన్య
clip_free_selection(Clipboard_T *cbd)
{
    yankreg_T *y_ptr = get_y_current();

    ఒకవేళ (cbd == &clip_plus)
	సమితి_y_current(get_y_రిజిస్టరు(PLUS_రిజిస్టరు));
    లేకపోతే
	సమితి_y_current(get_y_రిజిస్టరు(నక్షత్రం_రిజిస్టరు));
    free_yank_all();
    get_y_current()->y_కొలత = 0;
    సమితి_y_current(y_ptr);
}

/*
 * Get the selected text and put it in register '*' or '+'.
 */
    శూన్య
clip_get_selection(Clipboard_T *cbd)
{
    yankreg_T	*old_y_మునుపటి, *old_y_current;
    pos_T	old_cursor;
    pos_T	old_visual;
    పూర్ణాంకం		old_visual_బహుళకం;
    colnr_T	old_virtcol;
    colnr_T	old_curswant;
    పూర్ణాంకం		old_సమితి_curswant;
    pos_T	old_op_ప్రారంభం, old_op_చివర;
    oparg_T	oa;
    cmdarg_T	ca;

    ఒకవేళ (cbd->owned)
    {
	ఒకవేళ ((cbd == &clip_plus
		&& get_y_రిజిస్టరు(PLUS_రిజిస్టరు)->y_array != ఖాళీ)
		|| (cbd == &clip_నక్షత్రం
		    && get_y_రిజిస్టరు(నక్షత్రం_రిజిస్టరు)->y_array != ఖాళీ))
	    తిరిగి;

	// Avoid triggering autocmds such as TextYankPost.
	ఖండం_autocmds();

	// Get the text between clip_star.start & clip_star.end
	old_y_మునుపటి = get_y_మునుపటి();
	old_y_current = get_y_current();
	old_cursor = curwin->w_cursor;
	old_virtcol = curwin->w_virtcol;
	old_curswant = curwin->w_curswant;
	old_సమితి_curswant = curwin->w_సమితి_curswant;
	old_op_ప్రారంభం = curbuf->b_op_ప్రారంభం;
	old_op_చివర = curbuf->b_op_చివర;
	old_visual = VIsual;
	old_visual_బహుళకం = VIsual_బహుళకం;
	clear_oparg(&oa);
	oa.regname = (cbd == &clip_plus ? '+' : '*');
	oa.op_type = OP_YANK;
	CLEAR_క్షేత్రం(ca);
	ca.oap = &oa;
	ca.cmdchar = 'y';
	ca.count1 = 1;
	ca.retval = CA_NO_ADJ_OP_చివర;
	do_pending_operator(&ca, 0, TRUE);

	// restore things
	సమితి_y_మునుపటి(old_y_మునుపటి);
	సమితి_y_current(old_y_current);
	curwin->w_cursor = old_cursor;
	curwin->w_virtcol = old_virtcol;
	మారింది_cline_bef_curs();   // old w_virtcol et al. may be invalid
	curwin->w_curswant = old_curswant;
	curwin->w_సమితి_curswant = old_సమితి_curswant;
	curbuf->b_op_ప్రారంభం = old_op_ప్రారంభం;
	curbuf->b_op_చివర = old_op_చివర;
	VIsual = old_visual;
	VIsual_బహుళకం = old_visual_బహుళకం;

	unblock_autocmds();
    }
    లేకపోతే ఒకవేళ (!is_clipboard_needs_update())
    {
	clip_free_selection(cbd);

	// Try to get selected text from another window
	clip_gen_request_selection(cbd);
    }
}

/*
 * Convert from the GUI selection string into the '*'/'+' register.
 */
    శూన్య
clip_yank_selection(
    పూర్ణాంకం		type,
    char_u	*str,
    దీర్ఘ	len,
    Clipboard_T *cbd)
{
    yankreg_T *y_ptr;

    ఒకవేళ (cbd == &clip_plus)
	y_ptr = get_y_రిజిస్టరు(PLUS_రిజిస్టరు);
    లేకపోతే
	y_ptr = get_y_రిజిస్టరు(నక్షత్రం_రిజిస్టరు);

    clip_free_selection(cbd);

    str_గమ్యం_reg(y_ptr, type, str, len, -1, FALSE);
}

    పూర్ణాంకం
clip_convert_selection_స్థానభ్రంశం(
	char_u	    **str,
	long_u	    *len,
	పూర్ణాంకం	    స్థానభ్రంశం, // Extra space to add in *str and the offset to
			    // place the actual string in *str.
	Clipboard_T *cbd)
{
    char_u	*p;
    పూర్ణాంకం		lnum;
    పూర్ణాంకం		i, j;
    int_u	eolsize;
    yankreg_T	*y_ptr;

    ఒకవేళ (cbd == &clip_plus)
	y_ptr = get_y_రిజిస్టరు(PLUS_రిజిస్టరు);
    లేకపోతే
	y_ptr = get_y_రిజిస్టరు(నక్షత్రం_రిజిస్టరు);

# ifdef USE_CRNL
    eolsize = 2;
# లేకపోతే
    eolsize = 1;
# endif

    *str = ఖాళీ;
    *len = 0;
    ఒకవేళ (y_ptr->y_array == ఖాళీ)
	తిరిగి -1;

    కొరకు (i = 0; i < y_ptr->y_కొలత; i++)
	*len += (long_u)y_ptr->y_array[i].నిడివి + eolsize;

    // Don't want newline character at end of last line if we're in MCHAR mode.
    ఒకవేళ (y_ptr->y_type == MCHAR && *len >= eolsize)
	*len -= eolsize;

    *len += స్థానభ్రంశం;
    p = *str = alloc(*len + 1);	// add one to avoid zero
    ఒకవేళ (p == ఖాళీ)
	తిరిగి -1;
    p += స్థానభ్రంశం;
    lnum = 0;
    కొరకు (i = 0, j = 0; i < (పూర్ణాంకం)*len - స్థానభ్రంశం; i++, j++)
    {
	ఒకవేళ (y_ptr->y_array[lnum].string[j] == '\n')
	    p[i] = NUL;
	లేకపోతే ఒకవేళ (y_ptr->y_array[lnum].string[j] == NUL)
	{
# ifdef USE_CRNL
	    p[i++] = '\r';
# endif
	    p[i] = '\n';
	    lnum++;
	    j = -1;
	}
	లేకపోతే
	    p[i] = y_ptr->y_array[lnum].string[j];
    }
    తిరిగి y_ptr->y_type;
}

/*
 * Convert the '*'/'+' register into a GUI selection string returned in *str
 * with length *len.
 * Returns the motion type, or -1 for failure.
 */
    పూర్ణాంకం
clip_convert_selection(char_u **str, long_u *len, Clipboard_T *cbd)
{
    తిరిగి clip_convert_selection_స్థానభ్రంశం(str, len, 0, cbd);
}

/*
 * When "regname" is a clipboard register, obtain the selection.  If it's not
 * available return zero, otherwise return "regname".
 */
    పూర్ణాంకం
may_get_selection(పూర్ణాంకం regname)
{
    ఒకవేళ (regname == '*')
    {
	ఒకవేళ (!clip_నక్షత్రం.అందుబాటు)
	    regname = 0;
	లేకపోతే
	    clip_get_selection(&clip_నక్షత్రం);
    }
    లేకపోతే ఒకవేళ (regname == '+')
    {
	ఒకవేళ (!clip_plus.అందుబాటు)
	    regname = 0;
	లేకపోతే
	    clip_get_selection(&clip_plus);
    }
    తిరిగి regname;
}

/*
 * If we have written to a clipboard register, send the text to the clipboard.
 */
    శూన్య
may_సమితి_selection(శూన్య)
{
    ఒకవేళ ((get_y_current() == get_y_రిజిస్టరు(నక్షత్రం_రిజిస్టరు))
	    && clip_నక్షత్రం.అందుబాటు)
    {
	clip_own_selection(&clip_నక్షత్రం);
	clip_gen_సమితి_selection(&clip_నక్షత్రం);
    }
    లేకపోతే ఒకవేళ ((get_y_current() == get_y_రిజిస్టరు(PLUS_రిజిస్టరు))
	    && clip_plus.అందుబాటు)
    {
	clip_own_selection(&clip_plus);
	clip_gen_సమితి_selection(&clip_plus);
    }
}

# ఒకవేళ defined(FEAT_WAYLAND_CLIPBOARD)

    స్థిర clip_wl_selection_T *
clip_wl_get_selection(wayland_selection_T sel)
{
    ఎంపిక (sel)
    {
	స్థితి WAYLAND_SELECTION_REGULAR:
	    తిరిగి &clip_wl.regular;
	స్థితి WAYLAND_SELECTION_PRIMARY:
	    తిరిగి &clip_wl.primary;
	అప్రమేయ:
	    తిరిగి ఖాళీ;
    }
}

    స్థిర clip_wl_selection_T *
clip_wl_get_selection_from_cbd(Clipboard_T *cbd)
{
    ఒకవేళ (cbd == &clip_plus)
	తిరిగి &clip_wl.regular;
    లేకపోతే ఒకవేళ (cbd == &clip_నక్షత్రం)
	తిరిగి &clip_wl.primary;
    లేకపోతే
	తిరిగి ఖాళీ;
}

    స్థిర Clipboard_T *
clip_wl_get_cbd_from_selection(clip_wl_selection_T *sel)
{
    ఒకవేళ (sel == &clip_wl.regular)
	తిరిగి &clip_plus;
    లేకపోతే ఒకవేళ (sel == &clip_wl.primary)
	తిరిగి &clip_నక్షత్రం;
    లేకపోతే
	తిరిగి ఖాళీ;
}

    స్థిర wayland_selection_T
clip_wl_get_selection_type(clip_wl_selection_T *sel)
{
    ఒకవేళ (sel == &clip_wl.regular)
	తిరిగి WAYLAND_SELECTION_REGULAR;
    లేకపోతే ఒకవేళ (sel == &clip_wl.primary)
	తిరిగి WAYLAND_SELECTION_PRIMARY;
    లేకపోతే
	తిరిగి WAYLAND_SELECTION_NONE;
}

    స్థిర bool
wl_దత్తాంశం_offer_listener_event_offer(
    శూన్య *దత్తాంశం వాడని,
    vwl_దత్తాంశం_offer_T *offer వాడని,
    స్థిరరాశి అక్షరం *mime_type
)
{
    // Only accept mime type if we support it
    కొరకు (పూర్ణాంకం i = 0; i < (పూర్ణాంకం)ARRAY_నిడివి(supported_mimes); i++)
	ఒకవేళ (STRCMP(mime_type, supported_mimes[i]) == 0)
	    తిరిగి true;
    తిరిగి FALSE;
}

స్థిర స్థిరరాశి vwl_దత్తాంశం_offer_listener_T vwl_దత్తాంశం_offer_listener = {
    .offer = wl_దత్తాంశం_offer_listener_event_offer
};

    స్థిర శూన్య
vwl_దత్తాంశం_device_listener_event_దత్తాంశం_offer(
	శూన్య *దత్తాంశం వాడని,
	vwl_దత్తాంశం_device_T *device వాడని,
	vwl_దత్తాంశం_offer_T *offer)
{
    // Immediately start listening for offer events from the data offer
    vwl_దత్తాంశం_offer_కూడు_listener(offer, &vwl_దత్తాంశం_offer_listener, ఖాళీ);
}

    స్థిర శూన్య
vwl_దత్తాంశం_device_listener_event_selection(
	శూన్య *దత్తాంశం వాడని,
	vwl_దత్తాంశం_device_T *device వాడని,
	vwl_దత్తాంశం_offer_T *offer,
	wayland_selection_T selection)
{
    clip_wl_selection_T *sel = clip_wl_get_selection(selection);

    // Destroy previous offer if any, it is now invalid
    vwl_దత్తాంశం_offer_destroy(sel->offer);

    // There are two cases when sel->offer is NULL
    // 1. No one owns the selection
    // 2. We own the selection (we'll just access the register directly)
    ఒకవేళ (offer == ఖాళీ || sel->ఆధారం != ఖాళీ)
    {
	// Selection event is from us, so we are the source client. Therefore
	// ignore it. Or the selection is cleared, so set sel->offer to NULL
	vwl_దత్తాంశం_offer_destroy(offer);
	sel->offer = ఖాళీ;
	తిరిగి;
    }

    // Save offer. When we want to request data, then we'll actually call the
    // receive method.
    sel->offer = offer;

}

    స్థిర శూన్య
vwl_దత్తాంశం_device_listener_event_ముగిసిన(
	శూన్య *దత్తాంశం వాడని,
	vwl_దత్తాంశం_device_T *device)
{
    clip_wl_selection_T *sel;
    // Device finished, guessing this can happen is when the seat becomes
    // invalid? If so, let the user call :wlrestore! to reset. There wouldn't be
    // any point in trying to create another data device for the same seat,
    // since the seat is in an invalid state.
    ఒకవేళ (device == clip_wl.regular.device)
    {
	sel = &clip_wl.regular;
	clip_wl.regular.device = ఖాళీ;
    }
    లేకపోతే ఒకవేళ (device == clip_wl.primary.device)
    {
	sel = &clip_wl.primary;
	clip_wl.primary.device = ఖాళీ;
    }
    లేకపోతే
	// Shouldn't happen
	తిరిగి;

    vim_free(sel->contents);
    vwl_దత్తాంశం_ఆధారం_destroy(sel->ఆధారం);
    vwl_దత్తాంశం_offer_destroy(sel->offer);
    sel->అందుబాటు = FALSE;

    vwl_దత్తాంశం_device_destroy(device);
}

స్థిర స్థిరరాశి vwl_దత్తాంశం_device_listener_T vwl_దత్తాంశం_device_listener = {
    .దత్తాంశం_offer = vwl_దత్తాంశం_device_listener_event_దత్తాంశం_offer,
    .selection = vwl_దత్తాంశం_device_listener_event_selection,
    .ముగిసిన = vwl_దత్తాంశం_device_listener_event_ముగిసిన
};

/*
 * Initialize the clipboard for Wayland using the global Wayland connection.
 * Returns OK on success and FAIL on failure.
 */
    పూర్ణాంకం
clip_init_wayland(శూన్య)
{
    int_u supported = WAYLAND_SELECTION_NONE;

    ఒకవేళ (wayland_ct == ఖాళీ)
	తిరిగి FAIL;

    memset(&clip_wl, 0, పరిమాణం(clip_wl));

    clip_wl.seat = vwl_connection_get_seat(wayland_ct, (అక్షరం *)p_wse);

    ఒకవేళ (clip_wl.seat == ఖాళీ)
	తిరిగి FAIL;

    clip_wl.regular.manager = vwl_connection_get_దత్తాంశం_device_manager(
	    wayland_ct, WAYLAND_SELECTION_REGULAR, &supported);

    ఒకవేళ (clip_wl.regular.manager != ఖాళీ)
    {
	clip_wl.regular.device = vwl_దత్తాంశం_device_manager_get_దత్తాంశం_device(
		clip_wl.regular.manager, clip_wl.seat);

	ఒకవేళ (clip_wl.regular.device != ఖాళీ)
	    clip_wl.regular.అందుబాటు = true;
	లేకపోతే
	{
	    // Shouldn't happen
	    vwl_దత్తాంశం_device_manager_discard(clip_wl.regular.manager);
	    clip_wl.regular.manager = ఖాళీ;
	    తిరిగి FAIL;
	}
    }

    // If we still don't support the primary selection, try finding one for it
    // specifically.
    ఒకవేళ (!(supported & WAYLAND_SELECTION_PRIMARY))
    {
	clip_wl.primary.manager = vwl_connection_get_దత్తాంశం_device_manager(
		wayland_ct, WAYLAND_SELECTION_PRIMARY, &supported);

	ఒకవేళ (clip_wl.primary.manager != ఖాళీ)
	{
	    clip_wl.primary.device = vwl_దత్తాంశం_device_manager_get_దత్తాంశం_device(
		    clip_wl.primary.manager, clip_wl.seat);

	    ఒకవేళ (clip_wl.primary.device != ఖాళీ)
		clip_wl.primary.అందుబాటు = true;
	    లేకపోతే
	    {
		vwl_దత్తాంశం_device_manager_discard(clip_wl.primary.manager);
		clip_wl.primary.manager = ఖాళీ;
	    }
	}
    }

    ఒకవేళ (clip_wl.regular.అందుబాటు && !clip_wl.primary.అందుబాటు)
    {
	// The protocol supports both regular and primary selections, just use
	// one data device manager and one data device. Or the primary selection
	// is not supported, make it point to the regular selection instead.
	clip_wl.primary.అందుబాటు = true;
	clip_wl.primary.manager = clip_wl.regular.manager;
	clip_wl.primary.device = clip_wl.regular.device;
    }

    ఒకవేళ (!clip_wl.regular.అందుబాటు && !clip_wl.primary.అందుబాటు)
	తిరిగి FAIL;

    // Start listening for selection updates
    ఒకవేళ (clip_wl.regular.device != ఖాళీ)
	vwl_దత్తాంశం_device_కూడు_listener(clip_wl.regular.device,
		&vwl_దత్తాంశం_device_listener, ఖాళీ);
    // Don't want to listen to the same data device twice
    ఒకవేళ (clip_wl.primary.device != ఖాళీ
	    && clip_wl.primary.device != clip_wl.regular.device)
	vwl_దత్తాంశం_device_కూడు_listener(clip_wl.primary.device,
		&vwl_దత్తాంశం_device_listener, ఖాళీ);

    తిరిగి OK;
}

    శూన్య
clip_uninit_wayland(శూన్య)
{
    clip_wl_selection_T *sel;

    ఒకవేళ (clipmethod == CLIPMETHOD_WAYLAND)
    {
	ఒకవేళ (clip_నక్షత్రం.owned)
	    clip_lose_selection(&clip_నక్షత్రం);
	ఒకవేళ (clip_plus.owned)
	    clip_lose_selection(&clip_plus);
    }

    // Don't want to double free
    ఒకవేళ (clip_wl.regular.manager != clip_wl.primary.manager)
	vwl_దత్తాంశం_device_manager_discard(clip_wl.primary.manager);
    vwl_దత్తాంశం_device_manager_discard(clip_wl.regular.manager);

    ఒకవేళ (clip_wl.regular.device != clip_wl.primary.device)
	vwl_దత్తాంశం_device_destroy(clip_wl.primary.device);
    vwl_దత్తాంశం_device_destroy(clip_wl.regular.device);

    sel = &clip_wl.regular;
    అయితే (true)
    {
	vim_free(sel->contents);
	vwl_దత్తాంశం_ఆధారం_destroy(sel->ఆధారం);
	vwl_దత్తాంశం_offer_destroy(sel->offer);
	sel->అందుబాటు = false;

	ఒకవేళ (sel == &clip_wl.primary)
	    ఆపు;
	sel = &clip_wl.primary;
    }

    vim_memset(&clip_wl, 0, పరిమాణం(clip_wl));
}

    పూర్ణాంకం
clip_reset_wayland(శూన్య)
{
    wayland_uninit_connection();

    ఒకవేళ (wayland_init_connection(wayland_display_name) == FAIL
	    || clip_init_wayland() == FAIL)
	తిరిగి FAIL;

    ఎంచుకో_clipmethod();
    తిరిగి OK;
}


/*
 * Read data from a file descriptor and write it to the given clipboard.
 */
    స్థిర శూన్య
clip_wl_receive_దత్తాంశం(Clipboard_T *cbd, స్థిరరాశి అక్షరం *mime_type, పూర్ణాంకం fd)
{
    char_u	*ప్రారంభం, *final;
    దీర్ఘ	len;
    garray_T	buf;
    పూర్ణాంకం		motion_type = MAUTO;
    ssize_t	r = 0;
    char_u	*tofree = ఖాళీ;
#  ifndef HAVE_SELECT
    నిర్మాణం pollfd   pfd;

    pfd.fd = fd;
    pfd.events = POLLIN;
#  లేకపోతే
    fd_set rfds;
    నిర్మాణం timeval  tv;

    FD_ZERO(&rfds);
    FD_SET(fd, &rfds);
#  endif

    // Make pipe (read end) non-blocking
    ఒకవేళ (fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK) == -1)
	తిరిగి;

    ga_init2(&buf, 1, 4096);

    // 4096 bytes seems reasonable for initial buffer size, memory is cheap
    // anyways.
    ఒకవేళ (ga_grow(&buf, 4096) == FAIL)
	తిరిగి;

    ప్రారంభం = buf.ga_దత్తాంశం;

#  ifndef HAVE_SELECT
    అయితే (poll(&pfd, 1, p_wtm) > 0)
#  లేకపోతే
    అయితే (tv.tv_sec = p_wtm / 1000, tv.tv_usec = (p_wtm % 1000) * 1000,
	    select(fd + 1, &rfds, ఖాళీ, ఖాళీ, &tv) > 0)
#  endif
    {
	r = పఠ(fd, ప్రారంభం, buf.ga_maxlen - 1 - buf.ga_len);

	ఒకవేళ (r == 0)
	    ఆపు;
	లేకపోతే ఒకవేళ (r < 0)
	{
	    ఒకవేళ (errno == EAGAIN || errno == EINTR)
		కొనసాగు;
	    ఆపు;
	}

	ప్రారంభం += r;
	buf.ga_len += r;

	// Realloc if we are at the end of the buffer
	ఒకవేళ (buf.ga_len >= buf.ga_maxlen - 1)
	{
	    ఒకవేళ (ga_grow(&buf, 8192) == FAIL)
		ఆపు;
	    ప్రారంభం = (char_u *)buf.ga_దత్తాంశం + buf.ga_len;
	}
    }

    ఒకవేళ (buf.ga_len == 0)
    {
	clip_free_selection(cbd); // Nothing received, clear register
	ga_clear(&buf);
	తిరిగి;
    }

    final = buf.ga_దత్తాంశం;
    len = buf.ga_len;

    ఒకవేళ (clip_convert_దత్తాంశం(&final, &len, &motion_type,
	    STRCMP(mime_type, VIM_ATOM_NAME) == 0
	    || STRCMP(mime_type, VIM_MIMETYPE_NAME) == 0,
	    STRCMP(mime_type, VIMENC_ATOM_NAME) == 0
	    || STRCMP(mime_type, VIMENC_MIMETYPE_NAME) == 0, &tofree) == OK)
	clip_yank_selection(motion_type, final, len, cbd);
    ga_clear(&buf);
    vim_free(tofree);
}

/*
 * Get the current selection and fill the respective register for cbd with the
 * data.
 */
    స్థిర శూన్య
clip_wl_request_selection(Clipboard_T *cbd)
{
    clip_wl_selection_T *sel = clip_wl_get_selection_from_cbd(cbd);
    పూర్ణాంకం			fds[2];
    పూర్ణాంకం			mime_types_len;
    స్థిరరాశి అక్షరం		**mime_types;
    స్థిరరాశి అక్షరం		*chosen_mime = ఖాళీ;

    ఒకవేళ (!sel->అందుబాటు)
	వెళ్ళు clear;

    // Dispatch any events that still queued up before checking for a data
    // offer.
    ఒకవేళ (vwl_connection_roundtrip(wayland_ct) == FAIL)
	వెళ్ళు clear;

    ఒకవేళ (sel->offer == ఖాళీ)
	వెళ్ళు clear;

    mime_types_len = sel->offer->mime_types.ga_len;
    mime_types = sel->offer->mime_types.ga_దత్తాంశం;

    // Choose mime type to receive from. Mime types with a lower index in the
    // "supported_mimes" array are prioritized over ones after it.
    కొరకు (పూర్ణాంకం i = 0; i < (పూర్ణాంకం)ARRAY_నిడివి(supported_mimes)
	    && chosen_mime == ఖాళీ; i++)
    {
	కొరకు (పూర్ణాంకం k = 0; k < mime_types_len && chosen_mime == ఖాళీ; k++)
	    ఒకవేళ (STRCMP(mime_types[k], supported_mimes[i]) == 0)
		chosen_mime = supported_mimes[i];
    }

    ఒకవేళ (chosen_mime == ఖాళీ || pipe(fds) == -1)
	వెళ్ళు clear;

    vwl_దత్తాంశం_offer_receive(sel->offer, chosen_mime, fds[1]);

    close(fds[1]); // Close before we read data so that when the source client
		   // closes their end we receive an EOF.

    ఒకవేళ (vwl_connection_flush(wayland_ct) >= 0)
	clip_wl_receive_దత్తాంశం(cbd, chosen_mime, fds[0]);

    close(fds[0]);

    తిరిగి;
clear:
    clip_free_selection(cbd);
}

    స్థిర శూన్య
vwl_దత్తాంశం_ఆధారం_listener_event_send(
    శూన్య *దత్తాంశం,
    vwl_దత్తాంశం_ఆధారం_T *ఆధారం వాడని,
    స్థిరరాశి అక్షరం *mime_type,
    int32_t fd
)
{
    clip_wl_selection_T *sel = దత్తాంశం;
    Clipboard_T		*cbd = clip_wl_get_cbd_from_selection(sel);
    bool		have_mime = false;
    పూర్ణాంకం			motion_type;
    long_u		నిడివి;
    char_u		*string; // Will be reallocated to a bigger size if
				 // needed.
    పూర్ణాంకం			స్థానభ్రంశం = 0;
    bool		is_vim, is_vimenc;
    size_t		మొత్తం = 0;
#  ifndef HAVE_SELECT
    నిర్మాణం pollfd   pfd;

    pfd.fd = fd;
    pfd.events = POLLOUT;
#  లేకపోతే
    fd_set	    wfds;
    నిర్మాణం timeval  tv;

    FD_ZERO(&wfds);
    FD_SET(fd, &wfds);
#  endif

    // Check if we actually have mime type
    కొరకు (పూర్ణాంకం i = 0; i < (పూర్ణాంకం)ARRAY_నిడివి(supported_mimes); i++)
	ఒకవేళ (STRCMP(supported_mimes[i], mime_type) == 0)
	{
	    have_mime = true;
	    ఆపు;
	}

    ఒకవేళ (!have_mime)
	వెళ్ళు నిష్క్రమించు;

    // First byte sent is motion type for vim specific formats. For the vimenc
    // format, after the first byte is the encoding type, which is null
    // terminated.

    is_vimenc = STRCMP(mime_type, VIMENC_ATOM_NAME) == 0
	|| STRCMP(mime_type, VIMENC_MIMETYPE_NAME) == 0;
    is_vim = STRCMP(mime_type, VIM_ATOM_NAME) == 0
	|| STRCMP(mime_type, VIM_MIMETYPE_NAME) == 0;

    ఒకవేళ (is_vimenc)
	స్థానభ్రంశం += 2 + STRLEN(p_enc);
    లేకపోతే ఒకవేళ (is_vim)
	స్థానభ్రంశం += 1;

    clip_get_selection(cbd);
    motion_type = clip_convert_selection_స్థానభ్రంశం(&string, &నిడివి, స్థానభ్రంశం, cbd);

    ఒకవేళ (motion_type < 0)
	వెళ్ళు నిష్క్రమించు;

    ఒకవేళ (is_vimenc)
    {
	string[0] = (char_u)motion_type;
	// Use vim_strncpy for safer copying
	vim_strncpy(string + 1, p_enc, STRLEN(p_enc));
    }
    లేకపోతే ఒకవేళ (is_vim)
	string[0] = (char_u)motion_type;


    అయితే (మొత్తం < (size_t)నిడివి &&
#  ifndef HAVE_SELECT
	    poll(&pfd, 1, p_wtm) > 0)
#  లేకపోతే
	    ((tv.tv_sec = p_wtm / 1000, tv.tv_usec = (p_wtm % 1000) * 1000),
	    select(fd + 1, ఖాళీ, &wfds, ఖాళీ, &tv) > 0))
#  endif
    {
	ssize_t w = లిఖ(fd, string + మొత్తం, నిడివి - మొత్తం);

	ఒకవేళ (w == -1)
	    ఆపు;
	మొత్తం += w;
    }

    vim_free(string);
నిష్క్రమించు:
    close(fd);
}

    స్థిర శూన్య
vwl_దత్తాంశం_ఆధారం_listener_event_cancelled(
	శూన్య *దత్తాంశం,
	vwl_దత్తాంశం_ఆధారం_T *ఆధారం వాడని)
{
    clip_wl_selection_T *sel = దత్తాంశం;
    Clipboard_T		*cbd = clip_wl_get_cbd_from_selection(sel);

    clip_lose_selection(cbd);
}

స్థిర స్థిరరాశి vwl_దత్తాంశం_ఆధారం_listener_T vwl_దత్తాంశం_ఆధారం_listener = {
    .send = vwl_దత్తాంశం_ఆధారం_listener_event_send,
    .cancelled = vwl_దత్తాంశం_ఆధారం_listener_event_cancelled
};

    స్థిర శూన్య
clip_wl_do_సమితి_selection(శూన్య *దత్తాంశం, uint32_t serial)
{
    clip_wl_selection_T *sel = దత్తాంశం;
    wayland_selection_T sel_type = clip_wl_get_selection_type(sel);

    vwl_దత్తాంశం_device_సమితి_selection(sel->device, sel->ఆధారం, serial, sel_type);

    sel->own_success = (vwl_connection_roundtrip(wayland_ct) == OK);
}

/*
 * Own the selection that cbd corresponds to. Start listening for requests from
 * other Wayland clients so they can receive data from us. Returns OK on success
 * and FAIL on failure.
 */
    స్థిర పూర్ణాంకం
clip_wl_own_selection(Clipboard_T *cbd)
{
    clip_wl_selection_T *sel = clip_wl_get_selection_from_cbd(cbd);
    wayland_selection_T sel_type = clip_wl_get_selection_type(sel);

    ఒకవేళ (!sel->అందుబాటు || vwl_connection_roundtrip(wayland_ct) == FAIL)
	తిరిగి FAIL;

    ఒకవేళ (sel->ఆధారం != ఖాళీ)
    {
	ఒకవేళ (sel_type == WAYLAND_SELECTION_PRIMARY)
	    // We already own the selection, ignore (only do this for primary
	    // selection). We don't re set the selection because then we would
	    // be setting the selection every time the user moves the visual
	    // selection cursor, which is messy and inefficient. Some
	    // applications like Google Chrome do it this way however.
	    తిరిగి OK;
	లేకపోతే ఒకవేళ (sel_type == WAYLAND_SELECTION_REGULAR)
	{
	    // Technically we don't need to do this as we already own the
	    // selection, however if a user yanks text a second time, the
	    // text yanked won't appear in their clipboard manager if they are
	    // using one.
	    //
	    // This can be unexpected behaviour for the user so its probably
	    // better to do it this way. Additionally other Wayland applications
	    // seem to set the selection every time.
	    vwl_దత్తాంశం_ఆధారం_destroy(sel->ఆధారం);
	}
	లేకపోతే
	    // Shouldn't happen
	    తిరిగి FAIL;
    }

    sel->ఆధారం = vwl_దత్తాంశం_device_manager_create_దత్తాంశం_ఆధారం(sel->manager);
    vwl_దత్తాంశం_ఆధారం_కూడు_listener(sel->ఆధారం, &vwl_దత్తాంశం_ఆధారం_listener, sel);

    // Advertise mime types
    కొరకు (పూర్ణాంకం i = 0; i < (పూర్ణాంకం)ARRAY_నిడివి(supported_mimes); i++)
	vwl_దత్తాంశం_ఆధారం_offer(sel->ఆధారం, supported_mimes[i]);

    sel->own_success = false;
    clip_wl_do_సమితి_selection(sel, 0);

    ఒకవేళ (!sel->own_success)
	వెళ్ళు fail;

    తిరిగి OK;
fail:
    vwl_దత్తాంశం_ఆధారం_destroy(sel->ఆధారం);
    sel->ఆధారం = ఖాళీ;
    తిరిగి FAIL;
}

/*
 * Disown the selection that cbd corresponds to.
 */
    స్థిర శూన్య
clip_wl_lose_selection(Clipboard_T *cbd)
{
    clip_wl_selection_T *sel = clip_wl_get_selection_from_cbd(cbd);

    ఒకవేళ (!sel->అందుబాటు)
	తిరిగి;

    vwl_దత్తాంశం_ఆధారం_destroy(sel->ఆధారం);
    sel->ఆధారం = ఖాళీ;
}

/*
 * Send the current selection to the clipboard. Do nothing for Wayland because
 * we will fill in the selection only when requested by another client.
 */
    స్థిర శూన్య
clip_wl_సమితి_selection(Clipboard_T *cbd వాడని)
{
}

#  ఒకవేళ defined(USE_SYSTEM)
/*
 * Return true if we own the selection corresponding to cbd or another client
 * does.
 */
    స్థిర bool
clip_wl_owner_exists(Clipboard_T *cbd)
{
    clip_wl_selection_T *sel = clip_wl_get_selection_from_cbd(cbd);

    ఒకవేళ (vwl_connection_roundtrip(wayland_ct) == FAIL)
	తిరిగి false;

    తిరిగి sel->అందుబాటు && (sel->ఆధారం != ఖాళీ || sel->offer != ఖాళీ);
}
#  endif

# endif // FEAT_WAYLAND_CLIPBOARD

#అయితే_ముగింపు // FEAT_CLIPBOARD

#నిర్వచితమైతే HAVE_CLIPMETHOD

/*
 * Returns the first method for accessing the clipboard that is available/works,
 * depending on the order of values in str.
 */
    స్థిర clipmethod_T
get_clipmethod(char_u *str)
{
    పూర్ణాంకం		len	= (పూర్ణాంకం)STRLEN(str) + 1;
    char_u	*buf	= alloc(len);

    ఒకవేళ (buf == ఖాళీ)
	తిరిగి CLIPMETHOD_FAIL;

    clipmethod_T ret = CLIPMETHOD_FAIL;
    char_u	*p = str;

    అయితే (*p != NUL)
    {
	clipmethod_T పద్ధతి = CLIPMETHOD_NONE;

	(శూన్య)copy_option_part(&p, buf, len, ",");

	ఒకవేళ (STRCMP(buf, "wayland") == 0)
	{
# ifdef FEAT_GUI
	    ఒకవేళ (!gui.లోపల_use)
# endif
	    {
# ifdef FEAT_WAYLAND_CLIPBOARD
		ఒకవేళ (clip_wl.regular.అందుబాటు || clip_wl.primary.అందుబాటు)
		    పద్ధతి = CLIPMETHOD_WAYLAND;
# endif
	    }
	}
	లేకపోతే ఒకవేళ (STRCMP(buf, "x11") == 0)
	{
# ifdef FEAT_GUI
	    ఒకవేళ (!gui.లోపల_use)
# endif
	    {
# ifdef FEAT_XCLIPBOARD
		// x_IOerror_handler() in os_unix.c should set xterm_dpy to NULL if
		// we lost connection to the X server.
		ఒకవేళ (xterm_dpy != ఖాళీ)
		{
		    // If the X connection is lost then that handler will longjmp
		    // somewhere else, in that case we will call choose_clipmethod()
		    // again from there, and this if block won't be executed since
		    // xterm_dpy will be set to NULL.
		    xterm_update();
		    పద్ధతి = CLIPMETHOD_X11;
		}
# endif
	    }
	}
	లేకపోతే
	{
# ifdef FEAT_CLIPBOARD_PROVIDER
	    // Check if name matches a clipboard provider
	    పూర్ణాంకం r = clip_provider_is_అందుబాటు(buf);

	    ఒకవేళ (r == 1)
	    {
		పద్ధతి = CLIPMETHOD_PROVIDER;
		ఒకవేళ (ret == CLIPMETHOD_FAIL)
		{
		    vim_free(clip_provider);
		    clip_provider = vim_strsave(buf);
		    ఒకవేళ (clip_provider == ఖాళీ)
			వెళ్ళు fail;
		}
	    }
	    లేకపోతే ఒకవేళ (r == -1)
# endif
	    {
# ifdef FEAT_CLIPBOARD_PROVIDER
fail:
# endif
		ret = CLIPMETHOD_FAIL;
		వెళ్ళు నిష్క్రమించు;
	    }
	}

	// Keep on going in order to catch errors
	ఒకవేళ (పద్ధతి != CLIPMETHOD_NONE && ret == CLIPMETHOD_FAIL)
	    ret = పద్ధతి;
    }

    // No match found, use "none".
    ret = (ret == CLIPMETHOD_FAIL) ? CLIPMETHOD_NONE : ret;

నిష్క్రమించు:
    vim_free(buf);
    తిరిగి ret;
}


/*
 * Returns name of clipmethod in a statically allocated string.
 */
    స్థిర char_u *
clipmethod_గమ్యం_str(clipmethod_T పద్ధతి)
{
    ఎంపిక(పద్ధతి)
    {
	స్థితి CLIPMETHOD_WAYLAND:
	    తిరిగి (char_u *)"wayland";
	స్థితి CLIPMETHOD_X11:
	    తిరిగి (char_u *)"x11";
	స్థితి CLIPMETHOD_PROVIDER:
# ifdef FEAT_EVAL
	    తిరిగి clip_provider;
# endif
	అప్రమేయ:
	    తిరిగి (char_u *)"none";
    }
}

/*
 * Sets the current clipmethod to use given by `get_clipmethod()`. Returns an
 * error message on failure else NULL.
 */
    అక్షరం *
ఎంచుకో_clipmethod(శూన్య)
{
    clipmethod_T పద్ధతి = get_clipmethod(p_cpm);

    ఒకవేళ (పద్ధతి == CLIPMETHOD_FAIL)
	తిరిగి e_invalid_వాదం;

// If GUI is running or we are not on a system with Wayland or X11, then always
// return CLIPMETHOD_NONE. System or GUI clipboard handling always overrides.
// This is unless a provider is being used.
# ఒకవేళ defined(FEAT_XCLIPBOARD) || defined(FEAT_WAYLAND_CLIPBOARD)
#  ఒకవేళ defined(FEAT_GUI)
    ఒకవేళ (పద్ధతి != CLIPMETHOD_PROVIDER && gui.లోపల_use)
    {
#   ifdef FEAT_WAYLAND
	// We only interact with Wayland for the clipboard, we can just deinit
	// everything.
	wayland_uninit_connection();
#   endif

	పద్ధతి = CLIPMETHOD_NONE;
	వెళ్ళు lose_sel_exit;
    }
#  endif
# లేకపోతే
    // If on a system like windows or macos, then clipmethod is irrelevant, we
    // use their way of accessing the clipboard. This is unless we are using the
    // clipboard provider
#  ifdef FEAT_CLIPBOARD_PROVIDER
    ఒకవేళ (పద్ధతి != CLIPMETHOD_PROVIDER)
#  endif
    {
	పద్ధతి = CLIPMETHOD_NONE;
	వెళ్ళు నిష్క్రమించు;
    }
# endif

# ifdef FEAT_CLIPBOARD
    // Deinitialize clipboard if there is no way to access clipboard
    ఒకవేళ (పద్ధతి == CLIPMETHOD_NONE)
	clip_init(FALSE);
    // If we have a clipmethod that works now, then initialize clipboard
    లేకపోతే ఒకవేళ (clipmethod == CLIPMETHOD_NONE && పద్ధతి != CLIPMETHOD_NONE)
    {
	clip_init(TRUE);
	did_warn_clipboard = false;
    }
    // Disown clipboard if we are switching to a new method
    లేకపోతే ఒకవేళ (clipmethod != CLIPMETHOD_NONE && పద్ధతి != clipmethod)
    {
#  ఒకవేళ (defined(FEAT_XCLIPBOARD) || defined(FEAT_WAYLAND_CLIPBOARD)) \
	&& defined(FEAT_GUI)
lose_sel_exit:
#  endif
	ఒకవేళ (clip_నక్షత్రం.owned)
	    clip_lose_selection(&clip_నక్షత్రం);
	ఒకవేళ (clip_plus.owned)
	    clip_lose_selection(&clip_plus);

#  ఒకవేళ defined(FEAT_GUI)
	ఒకవేళ (!gui.లోపల_use)
#  endif
	{
	    clip_init(TRUE);
	    did_warn_clipboard = false;
	}
    }
# endif // FEAT_CLIPBOARD

# ఒకవేళ !defined(FEAT_XCLIPBOARD) && !defined(FEAT_WAYLAND_CLIPBOARD)
నిష్క్రమించు:
# endif

    clipmethod = పద్ధతి;

# ifdef FEAT_EVAL
    సమితి_vim_var_string(VV_CLIPMETHOD, (char_u*)clipmethod_గమ్యం_str(పద్ధతి), -1);
# endif

    తిరిగి ఖాళీ;
}

/*
 * Call choose_clipmethod().
 */
    శూన్య
ex_clipreset(exarg_T *eap వాడని)
{
    clipmethod_T prev = clipmethod;

    ఎంచుకో_clipmethod();

    ఒకవేళ (clipmethod == CLIPMETHOD_NONE)
	smsg(_("Could not find a way to access the clipboard."));
    లేకపోతే ఒకవేళ (clipmethod != prev)
	smsg(_("Switched to clipboard method '%s'."),
		clipmethod_గమ్యం_str(clipmethod));
}

/*
 * Adjust the register name pointed to with "rp" for the clipboard being
 * used always and the clipboard being available.
 */
    శూన్య
adjust_clip_reg(పూర్ణాంకం *rp)
{
# ifdef FEAT_CLIPBOARD_PROVIDER
    ఒకవేళ (clipmethod == CLIPMETHOD_PROVIDER)
    {
	ఒకవేళ (*rp == 0 && clip_unnamed != 0)
	    *rp = ((clip_unnamed & CLIP_UNNAMED_PLUS)) ? '+' : '*';
	తిరిగి;
    }
# endif
# ifdef FEAT_CLIPBOARD
    // If no reg. specified, and "unnamed" or "unnamedplus" is in 'clipboard',
    // use '*' or '+' reg, respectively. "unnamedplus" prevails.
    ఒకవేళ (*rp == 0 && (clip_unnamed != 0 || clip_unnamed_saved != 0))
    {
	ఒకవేళ (clip_unnamed != 0)
	    *rp = ((clip_unnamed & CLIP_UNNAMED_PLUS) && clip_plus.అందుబాటు)
								  ? '+' : '*';
	లేకపోతే
	    *rp = ((clip_unnamed_saved & CLIP_UNNAMED_PLUS)
					   && clip_plus.అందుబాటు) ? '+' : '*';
    }
    ఒకవేళ ((!clip_నక్షత్రం.అందుబాటు && *rp == '*') ||
	   (!clip_plus.అందుబాటు && *rp == '+'))
    {
	సందేశం_warn_missing_clipboard();
	*rp = 0;
    }
# endif
}

/*
 * Extract the items in the 'clipboard' option and set global values.
 * Return an error message or NULL for success.
 */
    అక్షరం *
did_సమితి_clipboard(optset_T *వాదనలు వాడని)
{
    పూర్ణాంకం		new_unnamed = 0;
# ifdef FEAT_CLIPBOARD
    పూర్ణాంకం		new_autoselect_నక్షత్రం = FALSE;
    పూర్ణాంకం		new_autoselect_plus = FALSE;
    పూర్ణాంకం		new_autoselectml = FALSE;
    పూర్ణాంకం		new_html = FALSE;
# endif
    regprog_T	*new_exclude_prog = ఖాళీ;
    అక్షరం	*errmsg = ఖాళీ;
    char_u	*p;

    కొరకు (p = p_cb; *p != NUL; )
    {
	// Note: Keep this in sync with p_cb_values.
	ఒకవేళ (STRNCMP(p, "unnamed", 7) == 0 && (p[7] == ',' || p[7] == NUL))
	{
	    new_unnamed |= CLIP_UNNAMED;
	    p += 7;
	}
	లేకపోతే ఒకవేళ (STRNCMP(p, "unnamedplus", 11) == 0
					    && (p[11] == ',' || p[11] == NUL))
	{
	    new_unnamed |= CLIP_UNNAMED_PLUS;
	    p += 11;
	}
# ifdef FEAT_CLIPBOARD
	లేకపోతే ఒకవేళ (STRNCMP(p, "autoselect", 10) == 0
					    && (p[10] == ',' || p[10] == NUL))
	{
	    new_autoselect_నక్షత్రం = TRUE;
	    p += 10;
	}
	లేకపోతే ఒకవేళ (STRNCMP(p, "autoselectplus", 14) == 0
					    && (p[14] == ',' || p[14] == NUL))
	{
	    new_autoselect_plus = TRUE;
	    p += 14;
	}
	లేకపోతే ఒకవేళ (STRNCMP(p, "autoselectml", 12) == 0
					    && (p[12] == ',' || p[12] == NUL))
	{
	    new_autoselectml = TRUE;
	    p += 12;
	}
	లేకపోతే ఒకవేళ (STRNCMP(p, "html", 4) == 0 && (p[4] == ',' || p[4] == NUL))
	{
	    new_html = TRUE;
	    p += 4;
	}
	లేకపోతే ఒకవేళ (STRNCMP(p, "exclude:", 8) == 0 && new_exclude_prog == ఖాళీ)
	{
	    p += 8;
	    new_exclude_prog = vim_regcomp(p, RE_MAGIC);
	    ఒకవేళ (new_exclude_prog == ఖాళీ)
		errmsg = e_invalid_వాదం;
	    ఆపు;
	}
# endif
	లేకపోతే
	{
	    errmsg = e_invalid_వాదం;
	    ఆపు;
	}
	ఒకవేళ (*p == ',')
	    ++p;
    }
    ఒకవేళ (errmsg == ఖాళీ)
    {
# ifdef FEAT_CLIPBOARD
	ఒకవేళ (global_busy)
	    // clip_unnamed will be reset to clip_unnamed_saved
	    // at end_global_changes
	    clip_unnamed_saved = new_unnamed;
	లేకపోతే
# endif
	    clip_unnamed = new_unnamed;
# ifdef FEAT_CLIPBOARD
	clip_autoselect_నక్షత్రం = new_autoselect_నక్షత్రం;
	clip_autoselect_plus = new_autoselect_plus;
	clip_autoselectml = new_autoselectml;
	clip_html = new_html;
	vim_regfree(clip_exclude_prog);
	clip_exclude_prog = new_exclude_prog;
# endif
# ఒకవేళ defined(FEAT_GUI_GTK)
	ఒకవేళ (gui.లోపల_use)
	{
#  ifdef USE_GTK4
	    gui_gtk_update_selection_formats(&clip_plus);
	    gui_gtk_update_selection_formats(&clip_నక్షత్రం);
#  లేకపోతే
	    gui_gtk_సమితి_selection_targets((GdkAtom)GDK_SELECTION_PRIMARY);
	    gui_gtk_సమితి_selection_targets((GdkAtom)clip_plus.gtk_sel_atom);
#  endif
#  ifdef FEAT_DND
	    gui_gtk_సమితి_dnd_targets();
#  endif
	}
# endif
    }
    లేకపోతే
	vim_regfree(new_exclude_prog);

    తిరిగి errmsg;
}

#అయితే_ముగింపు // HAVE_CLIPMETHOD

#నిర్వచితమైతే FEAT_CLIPBOARD_PROVIDER

/*
 * Check if a clipboard provider with given name is available. Returns 1 if available,
 * 0 if not available, and -1 on error
 */
    స్థిర పూర్ణాంకం
clip_provider_is_అందుబాటు(char_u *provider)
{
    dict_T	*providers = get_vim_var_dict(VV_CLIPPROVIDERS);
    typval_T	provider_tv = {0};
    callback_T	callback = {0};
    typval_T	rettv = {0};
    typval_T	func_tv = {0};
    పూర్ణాంకం		res = 0;

    ఒకవేళ (dict_get_tv(providers, (అక్షరం *)provider, &provider_tv) == FAIL
	    || provider_tv.v_type != VAR_DICT)
	// clipboard provider not defined
	తిరిగి -1;

    ఒకవేళ (dict_get_tv(provider_tv.vval.v_dict, "available", &func_tv) == FAIL)
    {
	clear_tv(&provider_tv);
	// If "available" function not specified assume always TRUE
	తిరిగి 1;
    }

    ఒకవేళ ((callback = get_callback(&func_tv)).cb_name == ఖాళీ)
	వెళ్ళు fail;

    ఒకవేళ (call_callback(&callback, -1, &rettv, 0, ఖాళీ) == FAIL ||
	    (rettv.v_type != VAR_BOOL && rettv.v_type != VAR_సంఖ్య))
	వెళ్ళు fail;

    ఒకవేళ (rettv.vval.v_సంఖ్య)
	res = 1;

    ఒకవేళ (FALSE)
fail:
	res = -1;

    free_callback(&callback);
    clear_tv(&func_tv);
    clear_tv(&rettv);
    clear_tv(&provider_tv);

    తిరిగి res;
}

/*
 * Get the specified callback "function" from the provider dictionary for
 * register "reg".
 */
    స్థిర పూర్ణాంకం
clip_provider_get_callback(
	char_u *reg,
	char_u *provider,
	char_u *function,
	callback_T *callback)
{
    dict_T	*providers = get_vim_var_dict(VV_CLIPPROVIDERS);
    typval_T	provider_tv;
    typval_T	action_tv;
    typval_T	func_tv;
    callback_T	cb;

    ఒకవేళ (dict_get_tv(providers, (అక్షరం *)provider, &provider_tv) == FAIL)
	తిరిగి FAIL;
    లేకపోతే ఒకవేళ (provider_tv.v_type != VAR_DICT)
    {
	clear_tv(&provider_tv);
	తిరిగి FAIL;
    }
    లేకపోతే ఒకవేళ (dict_get_tv(
		provider_tv.vval.v_dict,
		(అక్షరం *)function,
		&action_tv) == FAIL)
    {
	clear_tv(&provider_tv);
	తిరిగి FAIL;
    }
    లేకపోతే ఒకవేళ (action_tv.v_type != VAR_DICT)
    {
	clear_tv(&provider_tv);
	clear_tv(&action_tv);
	తిరిగి FAIL;
    }
    లేకపోతే ఒకవేళ (dict_get_tv(action_tv.vval.v_dict, (అక్షరం *)reg, &func_tv) == FAIL)
    {
	clear_tv(&provider_tv);
	clear_tv(&action_tv);
	తిరిగి FAIL;
    }
    లేకపోతే ఒకవేళ ((cb = get_callback(&func_tv)).cb_name == ఖాళీ)
    {
	clear_tv(&provider_tv);
	clear_tv(&action_tv);
	clear_tv(&func_tv);
	తిరిగి FAIL;
    }
    clear_tv(&provider_tv);
    clear_tv(&action_tv);

    // func_tv owns the function name, so we must make a copy for the callback
    సమితి_callback(callback, &cb);
    clear_tv(&func_tv);
    తిరిగి OK;
}

    స్థిర శూన్య
clip_provider_copy(char_u *reg, char_u *provider)
{
    స్థిర bool	recursive = false;
    callback_T	callback;
    typval_T	rettv;
    typval_T	argvars[4];
    yankreg_T	*y_ptr;
    char_u	type[2 + NUMBUFLEN] = {0};
    list_T	*list = ఖాళీ;

    ఒకవేళ (recursive)
	తిరిగి;

    ఒకవేళ (clip_provider_get_callback(
		reg,
		provider,
		(char_u *)"copy",
		&callback) == FAIL)
	తిరిగి;

    // Convert register type into a string
    ఒకవేళ (*reg == '+')
	y_ptr = get_y_రిజిస్టరు(REAL_PLUS_రిజిస్టరు);
    లేకపోతే
	y_ptr = get_y_రిజిస్టరు(నక్షత్రం_రిజిస్టరు);

    ఎంపిక (y_ptr->y_type)
    {
	స్థితి MCHAR:
	    type[0] = 'v';
	    ఆపు;
	స్థితి MLINE:
	    type[0] = 'V';
	    ఆపు;
	స్థితి MBLOCK:
	    sprintf((అక్షరం *)type, "%c%d", Ctrl_V, y_ptr->y_వెడల్పు + 1);
	    ఆపు;
	అప్రమేయ:
	    type[0] = 0;
	    ఆపు;
    }

    argvars[0].v_type = VAR_STRING;
    argvars[0].vval.v_string = reg;

    argvars[1].v_type = VAR_STRING;
    argvars[1].vval.v_string = type;

    // Get register contents by creating a list of lines
    list = list_alloc();

    ఒకవేళ (list == ఖాళీ)
    {
	free_callback(&callback);
	తిరిగి;
    }

    కొరకు (పూర్ణాంకం i = 0; i < y_ptr->y_కొలత; i++)
	ఒకవేళ (list_append_string(list, y_ptr->y_array[i].string,
	    (పూర్ణాంకం)y_ptr->y_array[i].నిడివి) == FAIL)
	{
	    free_callback(&callback);
	    list_unref(list);
	    తిరిగి;
	}

    list->lv_refcount++;

    argvars[2].v_type = VAR_LIST;
    argvars[2].v_తాళం = VAR_FIXED;
    argvars[2].vval.v_list = list;

    argvars[3].v_type = VAR_UNKNOWN;

    textlock++;
    recursive = true;
    call_callback(&callback, -1, &rettv, 3, argvars);
    recursive = false;
    clear_tv(&rettv);
    textlock--;

    free_callback(&callback);
    list_unref(list);
}

    స్థిర శూన్య
clip_provider_paste(char_u *reg, char_u *provider)
{
    స్థిర bool	recursive = false;
    callback_T	callback;
    typval_T	argvars[2];
    typval_T	rettv;
    పూర్ణాంకం		ret;
    char_u	*reg_type;
    list_T	*పంక్తులు;

    ఒకవేళ (recursive)
	తిరిగి;

    ఒకవేళ (clip_provider_get_callback(
		reg,
		provider,
		(char_u *)"paste",
		&callback) == FAIL)
	తిరిగి;

    argvars[0].v_type = VAR_STRING;
    argvars[0].vval.v_string = reg;

    argvars[1].v_type = VAR_UNKNOWN;

    textlock++;
    recursive = true;
    ret = call_callback(&callback, -1, &rettv, 1, argvars);
    recursive = false;
    textlock--;

    ఒకవేళ (ret == FAIL)
	వెళ్ళు నిష్క్రమించు;
    లేకపోతే ఒకవేళ (rettv.v_type == VAR_TUPLE
	    && TUPLE_LEN(rettv.vval.v_tuple) == 2
	    && TUPLE_అంశం(rettv.vval.v_tuple, 0)->v_type == VAR_STRING
	    && TUPLE_అంశం(rettv.vval.v_tuple, 1)->v_type == VAR_LIST)
    {
	reg_type = TUPLE_అంశం(rettv.vval.v_tuple, 0)->vval.v_string;
	పంక్తులు = TUPLE_అంశం(rettv.vval.v_tuple, 1)->vval.v_list;
    }
    లేకపోతే ఒకవేళ (rettv.v_type == VAR_LIST
	    && rettv.vval.v_list->lv_len == 2
	    && rettv.vval.v_list->lv_మొదటి->li_tv.v_type == VAR_STRING
	    && rettv.vval.v_list->lv_మొదటి->li_తదుపరి->li_tv.v_type == VAR_LIST)
    {
	reg_type = rettv.vval.v_list->lv_మొదటి->li_tv.vval.v_string;
	పంక్తులు = rettv.vval.v_list->lv_మొదటి->li_తదుపరి->li_tv.vval.v_list;
    }
    లేకపోతే
	వెళ్ళు నిష్క్రమించు;

    {
	char_u		yank_type = MAUTO;
	దీర్ఘ		ఖండం_len = -1;
	yankreg_T	*y_ptr, *cur_y_ptr;
	char_u		**lstval;
	char_u		**allocval;
	char_u		buf[NUMBUFLEN];
	char_u		**curval;
	char_u		**curallocval;
	char_u		*strval;
	listitem_T	*li;
	పూర్ణాంకం		len;

	// If the list is NULL handle like an empty list.
	len = పంక్తులు == ఖాళీ ? 0 : పంక్తులు->lv_len;

	// First half: use for pointers to result lines; second half: use for
	// pointers to allocated copies.
	lstval = ALLOC_MULT(char_u *, (len + 1) * 2);
	ఒకవేళ (lstval == ఖాళీ)
	    వెళ్ళు నిష్క్రమించు;
	curval = lstval;
	allocval = lstval + len + 2;
	curallocval = allocval;

	ఒకవేళ (పంక్తులు != ఖాళీ)
	{
	    CHECK_LIST_MATERIALIZE(పంక్తులు);
	    FOR_ALL_LIST_వస్తువులు(పంక్తులు, li)
	    {
		strval = tv_get_string_buf_chk(&li->li_tv, buf);
		ఒకవేళ (strval == ఖాళీ)
		    వెళ్ళు free_lstval;
		ఒకవేళ (strval == buf)
		{
		    // Need to make a copy, next tv_get_string_buf_chk() will
		    // overwrite the string.
		    strval = vim_strsave(buf);
		    ఒకవేళ (strval == ఖాళీ)
			వెళ్ళు free_lstval;
		    *curallocval++ = strval;
		}
		*curval++ = strval;
	    }
	}
	*curval++ = ఖాళీ;

	ఒకవేళ (*reg_type != NUL && (STRLEN(reg_type) == 0
		|| get_yank_type(&reg_type, &yank_type, &ఖండం_len) == FAIL))
	{
	    emsg(e_invalid_వాదం);
	    వెళ్ళు free_lstval;
	}

	ఒకవేళ (*reg == '+')
	    y_ptr = get_y_రిజిస్టరు(REAL_PLUS_రిజిస్టరు);
	లేకపోతే
	    y_ptr = get_y_రిజిస్టరు(నక్షత్రం_రిజిస్టరు);

	// Free previous register contents
	cur_y_ptr = get_y_current();
	సమితి_y_current(y_ptr);

	free_yank_all();
	get_y_current()->y_కొలత = 0;

	సమితి_y_current(cur_y_ptr);

	str_గమ్యం_reg(y_ptr,
		yank_type,
		(char_u *)lstval,
		-1,
		ఖండం_len,
		TRUE);

free_lstval:
	అయితే (curallocval > allocval)
	    vim_free(*--curallocval);
	vim_free(lstval);
    }

నిష్క్రమించు:
    free_callback(&callback);
    clear_tv(&rettv);
}

// Used to stop calling the provider callback every time there is an update.
// This prevents unnecessary calls when accessing the provider often in an
// interval.
//
// If -1 then allow provider callback to be called then set to one. Default
// value (is allowed) is -2.
స్థిర పూర్ణాంకం నక్షత్రం_pause_లెక్క = -2, plus_pause_లెక్క = -2;

    శూన్య
call_clip_provider_request(పూర్ణాంకం reg)
{
    ఒకవేళ (clipmethod != CLIPMETHOD_PROVIDER)
	తిరిగి;

    ఒకవేళ (reg == '+' && plus_pause_లెక్క < 0)
    {
	ఒకవేళ (plus_pause_లెక్క == -1)
	    plus_pause_లెక్క = 1;
	clip_provider_paste((char_u *)"+", clip_provider);
    }
    లేకపోతే ఒకవేళ (reg == '*' && నక్షత్రం_pause_లెక్క < 0)
    {
	ఒకవేళ (నక్షత్రం_pause_లెక్క == -1)
	    నక్షత్రం_pause_లెక్క = 1;
	clip_provider_paste((char_u *)"*", clip_provider);
    }
    లేకపోతే
	తిరిగి;
}

    శూన్య
call_clip_provider_సమితి(పూర్ణాంకం reg)
{
    ఒకవేళ (clipmethod != CLIPMETHOD_PROVIDER)
	తిరిగి;

    ఒకవేళ (reg == '+' && plus_pause_లెక్క < 0)
    {
	ఒకవేళ (plus_pause_లెక్క == -1)
	    plus_pause_లెక్క = 1;
	clip_provider_copy((char_u *)"+", clip_provider);
    }
    లేకపోతే ఒకవేళ (reg == '*' && నక్షత్రం_pause_లెక్క < 0)
    {
	ఒకవేళ (నక్షత్రం_pause_లెక్క == -1)
	    నక్షత్రం_pause_లెక్క = 1;
	clip_provider_copy((char_u *)"*", clip_provider);
    }
}

/*
 * Makes it so that the next provider call is only done once any calls after are
 * ignored, until dec_clip_provider is called the same number of times after
 * again. Note that this is per clipboard register ("+", "*")
 */
    శూన్య
inc_clip_provider(శూన్య)
{
    plus_pause_లెక్క = (plus_pause_లెక్క == -2
	|| plus_pause_లెక్క == -1) ? -1 : plus_pause_లెక్క + 1;
    నక్షత్రం_pause_లెక్క = (నక్షత్రం_pause_లెక్క == -2
	|| నక్షత్రం_pause_లెక్క == -1) ? -1 : నక్షత్రం_pause_లెక్క + 1;
}

    శూన్య
dec_clip_provider(శూన్య)
{
    ఒకవేళ (plus_pause_లెక్క != -2)
	plus_pause_లెక్క = plus_pause_లెక్క == -1 ? -1 : plus_pause_లెక్క - 1;
    ఒకవేళ (నక్షత్రం_pause_లెక్క != -2)
	నక్షత్రం_pause_లెక్క = నక్షత్రం_pause_లెక్క == -1 ? -1 : నక్షత్రం_pause_లెక్క - 1;

    ఒకవేళ (plus_pause_లెక్క == 0 || plus_pause_లెక్క == -1)
	plus_pause_లెక్క = -2;
    ఒకవేళ (నక్షత్రం_pause_లెక్క == 0 || నక్షత్రం_pause_లెక్క == -1)
	నక్షత్రం_pause_లెక్క = -2;
}

#అయితే_ముగింపు // FEAT_CLIPBOARD_PROVIDER

#ఒకవేళ defined(FEAT_WAYLAND_CLIPBOARD) || (defined(FEAT_GUI_GTK) && defined(USE_GTK4))
/*
 * If "vim" is TRUE, then get the motion type. If "vimenc" is TRUE, then get the
 * motion type and also convert "*buf". "buf" and "len_store" will be updated to
 * reflect the actual contents, but should be set beforehand with the initial
 * contents. Note that if "*tofree" is not NULL, use vim_free() on "*buf",
 * otherwise use the free func for which "*buf" was allocated with. In both
 * cases use vim_free() on "*tofree". Returns OK on success and FAIL on failure.
 */
    పూర్ణాంకం
clip_convert_దత్తాంశం(
	char_u	**buf,
	దీర్ఘ	*len_store,
	పూర్ణాంకం	*motion,
	bool	vim,
	bool	vimenc,
	char_u	**tofree)
{
    char_u  *final = *buf;
    char_u  *enc;
    దీర్ఘ    len = *len_store;

    ఒకవేళ (vim && len >= 2)
    {
	*motion = *final++;
	len--;
    }
    లేకపోతే ఒకవేళ (vimenc && len >= 3)
    {
	vimconv_T   conv;
	పూర్ణాంకం	    convlen;

	// First byte is motion type
	*motion = *final++;
	len--;

	// Get encoding of selection
	enc = final;

	// Skip the encoding type including null terminator in final text
	final = memchr(final, NUL, len);
	ఒకవేళ (final == ఖాళీ)
	    తిరిగి FAIL;
	final++; // Skip NUL

	// Subtract pointers to get length of encoding;
	len -= final - enc;

	conv.vc_type = CONV_NONE;
	convert_అమరిక(&conv, enc, p_enc);
	ఒకవేళ (conv.vc_type != CONV_NONE)
	{
	   char_u *tmp;

	   convlen = len;
	   tmp = string_convert(&conv, final, &convlen);
	   len = convlen;
	   ఒకవేళ (tmp != ఖాళీ)
	   {
		final = tmp;
		*tofree = final;
	   }
	   convert_అమరిక(&conv, ఖాళీ, ఖాళీ);
	}
    }
    *buf = final;
    *len_store = len;
    తిరిగి OK;
}
#అయితే_ముగింపు
