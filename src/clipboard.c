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

#ಅನ್ತರ್ಭಾವ "vim.h"

#ifdef FEAT_CYGWIN_WIN32_CLIPBOARD
# define WIN32_LEAN_AND_MEAN
# include <windows.h>
# include "winclip.pro"
#endif

// Functions for copying and pasting text between applications.
// This is always included in a GUI version, but may also be included when the
// clipboard and mouse is available to a terminal version such as xterm.
// Note: there are some more functions in ops.c that handle selection stuff.
//
// Also note that the majority of functions here deal with the X 'primary'
// (visible - for Visual mode use) selection, and only that. There are no
// versions of these for the 'clipboard' selection, as Visual mode has no use
// for them.


#ifdef FEAT_CLIPBOARD_PROVIDER
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ clip_provider_is_available(char_u *provider);
#endif

#ಯದಿ defined(FEAT_CLIPBOARD)

# ಯದಿ defined(FEAT_WAYLAND_CLIPBOARD)

#  include "wayland.h"

// Represents either the regular or primary selection
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ {
    char_u		*contents;	// Non-null if we own selection,
					// contains the data to send to other
					// clients.
    vwl_data_source_T	*source;	// Non-NULL if we own the selection,
					// else NULL if we don't.
    vwl_data_offer_T	*offer;		// Current offer for the selection

    bool		own_success;	// Used by clip_wl_own_selection()
    bool		available;	// If selection is ready to serve/use

    // These may point to the same proxy as the other selection
    vwl_data_device_manager_T	*manager;
    vwl_data_device_T		*device;
} clip_wl_selection_T;

// Represents the clipboard for the global Wayland connection, for the chosen
// seat (using the 'wl_seat' option)
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ {
    vwl_seat_T *seat;

    clip_wl_selection_T regular;
    clip_wl_selection_T primary;
} clip_wl_T;

// Mime types we support sending and receiving
// Mimes with a lower index in the array are prioritized first when we are
// receiving data.
ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ *supported_mimes[] = {
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

ಸ್ಥಿರ ಶೂನ್ಯ
clip_wl_receive_data(Clipboard_T *cbd, ಧ್ರುವ ವರ್ಣ *mime_type, ಪೂರ್ಣಾಙ್ಕ fd);
ಸ್ಥಿರ ಶೂನ್ಯ clip_wl_request_selection(Clipboard_T *cbd);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ clip_wl_own_selection(Clipboard_T *cbd);
ಸ್ಥಿರ ಶೂನ್ಯ clip_wl_lose_selection(Clipboard_T *cbd);
ಸ್ಥಿರ ಶೂನ್ಯ clip_wl_set_selection(Clipboard_T *cbd);

#  ಯದಿ defined(USE_SYSTEM)
ಸ್ಥಿರ bool clip_wl_owner_exists(Clipboard_T *cbd);
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
    ಶೂನ್ಯ
clip_init(ಪೂರ್ಣಾಙ್ಕ can_use)
{
    Clipboard_T *cb;

    cb = &clip_star;
    ಪ್ರತಿ (;;)
    {
	// No need to init again if cbd is already available
	ಯದಿ (can_use && cb->available)
	    ಗಚ್ಛ skip;

	cb->available  = can_use;
	cb->owned      = FALSE;
	cb->start.lnum = 0;
	cb->start.col  = 0;
	cb->end.lnum   = 0;
	cb->end.col    = 0;
	cb->state      = SELECT_CLEARED;

skip:
	ಯದಿ (cb == &clip_plus)
	    ಭಙ್ಗ;
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
    ಶೂನ್ಯ
clip_update_selection(Clipboard_T *clip)
{
    pos_T	    start, end;

    // If visual mode is only due to a redo command ("."), then ignore it
    ಯದಿ (!redo_VIsual_busy && VIsual_active && (State & MODE_NORMAL))
    {
	ಯದಿ (LT_POS(VIsual, curwin->w_cursor))
	{
	    start = VIsual;
	    end = curwin->w_cursor;
	    ಯದಿ (has_mbyte)
		end.col += (*mb_ptr2len)(ml_get_cursor()) - 1;
	}
	ಅನ್ಯಥಾ
	{
	    start = curwin->w_cursor;
	    end = VIsual;
	}
	ಯದಿ (!EQUAL_POS(clip->start, start)
		|| !EQUAL_POS(clip->end, end)
		|| clip->vmode != VIsual_mode)
	{
	    clip_clear_selection(clip);
	    clip->start = start;
	    clip->end = end;
	    clip->vmode = VIsual_mode;
	    clip_free_selection(clip);
	    clip_own_selection(clip);
	    clip_gen_set_selection(clip);
	}
    }
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
clip_gen_own_selection(Clipboard_T *cbd)
{
# ಯದಿ defined(FEAT_XCLIPBOARD) || defined(FEAT_WAYLAND_CLIPBOARD)
#  ifdef FEAT_GUI
    ಯದಿ (gui.in_use)
	ಪ್ರತ್ಯಾಗಮ clip_mch_own_selection(cbd);
    ಅನ್ಯಥಾ
#  endif
    {
	ಯದಿ (clipmethod == CLIPMETHOD_WAYLAND)
	{
#  ifdef FEAT_WAYLAND_CLIPBOARD
	    ಪ್ರತ್ಯಾಗಮ clip_wl_own_selection(cbd);
#  endif
	}
	ಅನ್ಯಥಾ ಯದಿ (clipmethod == CLIPMETHOD_X11)
	{
#  ifdef FEAT_XCLIPBOARD
	    ಪ್ರತ್ಯಾಗಮ clip_xterm_own_selection(cbd);
#  endif
	}
    }
    ಪ್ರತ್ಯಾಗಮ FAIL;
# ಅನ್ಯಥಾ
    ಪ್ರತ್ಯಾಗಮ clip_mch_own_selection(cbd);
# endif
}

    ಶೂನ್ಯ
clip_own_selection(Clipboard_T *cbd)
{
    /*
     * Also want to check somehow that we are reading from the keyboard rather
     * than a mapping etc.
     */
# ಯದಿ defined(FEAT_X11) || defined(FEAT_WAYLAND_CLIPBOARD)
    // Always own the selection, we might have lost it without being
    // notified, e.g. during a ":sh" command.
    ಯದಿ (cbd->available)
    {
	ಪೂರ್ಣಾಙ್ಕ was_owned = cbd->owned;

	cbd->owned = (clip_gen_own_selection(cbd) == OK);
	ಯದಿ (!was_owned && (cbd == &clip_star || cbd == &clip_plus))
	{
	    // May have to show a different kind of highlighting for the
	    // selected area.  There is no specific redraw command for this,
	    // just redraw all windows on the current buffer.
	    ಯದಿ (cbd->owned
		    && (get_real_state() == MODE_VISUAL
					    || get_real_state() == MODE_SELECT)
		    && (cbd == &clip_star ? clip_isautosel_star()
						      : clip_isautosel_plus())
		    && HL_ATTR(HLF_V) != HL_ATTR(HLF_VNC))
		redraw_curbuf_later(UPD_INVERTED_ALL);
	}
    }
# ಅನ್ಯಥಾ
    // Only own the clipboard when we didn't own it yet.
    ಯದಿ (!cbd->owned && cbd->available)
	cbd->owned = (clip_gen_own_selection(cbd) == OK);
# endif
}

    ಸ್ಥಿರ ಶೂನ್ಯ
clip_gen_lose_selection(Clipboard_T *cbd)
{
# ಯದಿ defined(FEAT_XCLIPBOARD) || defined(FEAT_WAYLAND_CLIPBOARD)
#  ifdef FEAT_GUI
    ಯದಿ (gui.in_use)
	clip_mch_lose_selection(cbd);
    ಅನ್ಯಥಾ
#  endif
    {
	ಯದಿ (clipmethod == CLIPMETHOD_WAYLAND)
	{
#  ifdef FEAT_WAYLAND_CLIPBOARD
	    clip_wl_lose_selection(cbd);
#  endif
	}
	ಅನ್ಯಥಾ ಯದಿ (clipmethod == CLIPMETHOD_X11)
	{
#  ifdef FEAT_XCLIPBOARD
	    clip_xterm_lose_selection(cbd);
#  endif
	}
    }
# ಅನ್ಯಥಾ
    clip_mch_lose_selection(cbd);
# endif
}

    ಶೂನ್ಯ
clip_lose_selection(Clipboard_T *cbd)
{
# ifdef FEAT_X11
    ಪೂರ್ಣಾಙ್ಕ	    was_owned = cbd->owned;
# endif
    ಪೂರ್ಣಾಙ್ಕ     visual_selection = FALSE;

    ಯದಿ (cbd == &clip_star || cbd == &clip_plus)
	visual_selection = TRUE;

    clip_free_selection(cbd);
    cbd->owned = FALSE;
    ಯದಿ (visual_selection)
	clip_clear_selection(cbd);
    clip_gen_lose_selection(cbd);
# ifdef FEAT_X11
    ಯದಿ (visual_selection)
    {
	// May have to show a different kind of highlighting for the selected
	// area.  There is no specific redraw command for this, just redraw all
	// windows on the current buffer.
	ಯದಿ (was_owned
		&& (get_real_state() == MODE_VISUAL
		    || get_real_state() == MODE_SELECT)
		&& (cbd == &clip_star ?
		    clip_isautosel_star() : clip_isautosel_plus())
		&& HL_ATTR(HLF_V) != HL_ATTR(HLF_VNC)
		&& !exiting)
	{
	    update_curbuf(UPD_INVERTED_ALL);
	    setcursor();
	    cursor_on();
	    out_flush_cursor(TRUE, FALSE);
	}
    }
# endif
}

    ಸ್ಥಿರ ಶೂನ್ಯ
clip_copy_selection(Clipboard_T *clip)
{
    ಯದಿ (VIsual_active && (State & MODE_NORMAL) && clip->available)
    {
	clip_update_selection(clip);
	clip_free_selection(clip);
	clip_own_selection(clip);
	ಯದಿ (clip->owned)
	    clip_get_selection(clip);
	clip_gen_set_selection(clip);
    }
}

/*
 * Save and restore clip_unnamed before doing possibly many changes. This
 * prevents accessing the clipboard very often which might slow down Vim
 * considerably.
 */
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ global_change_ಎಣಿಕೆ = 0; // if set, inside a start_global_changes
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ clipboard_needs_update = FALSE; // clipboard needs to be updated
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ clip_did_set_selection = TRUE;

/*
 * Save clip_unnamed and reset it.
 */
    ಶೂನ್ಯ
start_global_changes(ಶೂನ್ಯ)
{
    ಯದಿ (++global_change_ಎಣಿಕೆ > 1)
	ಪ್ರತ್ಯಾಗಮ;
    clip_unnamed_saved = clip_unnamed;
    clipboard_needs_update = FALSE;

    ಯದಿ (clip_did_set_selection)
    {
	clip_unnamed = 0;
	clip_did_set_selection = FALSE;
    }
}

/*
 * Return TRUE if setting the clipboard was postponed, it already contains the
 * right text.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
is_clipboard_needs_update(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ clipboard_needs_update;
}

/*
 * Restore clip_unnamed and set the selection when needed.
 */
    ಶೂನ್ಯ
end_global_changes(ಶೂನ್ಯ)
{
    ಯದಿ (--global_change_ಎಣಿಕೆ > 0)
	// recursive
	ಪ್ರತ್ಯಾಗಮ;
    ಯದಿ (!clip_did_set_selection)
    {
	clip_did_set_selection = TRUE;
	clip_unnamed = clip_unnamed_saved;
	clip_unnamed_saved = 0;
	ಯದಿ (clipboard_needs_update)
	{
	    // only store something in the clipboard,
	    // if we have yanked anything to it
	    ಯದಿ (clip_unnamed & CLIP_UNNAMED)
	    {
		clip_own_selection(&clip_star);
		clip_gen_set_selection(&clip_star);
	    }
	    ಯದಿ (clip_unnamed & CLIP_UNNAMED_PLUS)
	    {
		clip_own_selection(&clip_plus);
		clip_gen_set_selection(&clip_plus);
	    }
	}
    }
    clipboard_needs_update = FALSE;
}

/*
 * Called when Visual mode is ended: update the selection.
 */
    ಶೂನ್ಯ
clip_auto_select(ಶೂನ್ಯ)
{
    ಯದಿ (clip_isautosel_star())
	clip_copy_selection(&clip_star);
    ಯದಿ (clip_isautosel_plus())
	clip_copy_selection(&clip_plus);
}

/*
 * Return TRUE if automatic selection of Visual area is desired for the *
 * register.
 */
    ಪೂರ್ಣಾಙ್ಕ
clip_isautosel_star(ಶೂನ್ಯ)
{
# ifdef FEAT_CLIPBOARD_PROVIDER
    ಯದಿ (clipmethod == CLIPMETHOD_PROVIDER)
	ಪ್ರತ್ಯಾಗಮ false;
# endif
# ifdef FEAT_GUI
    ಯದಿ (gui.in_use)
	ಪ್ರತ್ಯಾಗಮ vim_strchr(p_go, GO_ASEL) != ರಿಕ್ತ
	    && vim_strchr(p_go, GO_ASELPLUS) == ರಿಕ್ತ;
# endif
    ಪ್ರತ್ಯಾಗಮ clip_autoselect_star;
}

/*
 * Return TRUE if automatic selection of Visual area is desired for the +
 * register.
 */
    ಪೂರ್ಣಾಙ್ಕ
clip_isautosel_plus(ಶೂನ್ಯ)
{
# ifdef FEAT_CLIPBOARD_PROVIDER
    ಯದಿ (clipmethod == CLIPMETHOD_PROVIDER)
	ಪ್ರತ್ಯಾಗಮ false;
# endif
# ifdef FEAT_GUI
    ಯದಿ (gui.in_use)
	ಪ್ರತ್ಯಾಗಮ vim_strchr(p_go, GO_ASELPLUS) != ರಿಕ್ತ;
# endif
    ಪ್ರತ್ಯಾಗಮ clip_autoselect_plus;
}


/*
 * Stuff for general mouse selection, without using Visual mode.
 */

/*
 * Compare two screen positions ala strcmp()
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
clip_compare_pos(
    ಪೂರ್ಣಾಙ್ಕ		row1,
    ಪೂರ್ಣಾಙ್ಕ		col1,
    ಪೂರ್ಣಾಙ್ಕ		row2,
    ಪೂರ್ಣಾಙ್ಕ		col2)
{
    ಯದಿ (row1 > row2) ಪ್ರತ್ಯಾಗಮ(1);
    ಯದಿ (row1 < row2) ಪ್ರತ್ಯಾಗಮ(-1);
    ಯದಿ (col1 > col2) ಪ್ರತ್ಯಾಗಮ(1);
    ಯದಿ (col1 < col2) ಪ್ರತ್ಯಾಗಮ(-1);
    ಪ್ರತ್ಯಾಗಮ(0);
}

// "how" flags for clip_invert_area()
# define CLIP_CLEAR	1
# define CLIP_SET	2
# define CLIP_TOGGLE	3

/*
 * Invert or un-invert a rectangle of the screen.
 * "invert" is true if the result is inverted.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
clip_invert_rectangle(
	Clipboard_T	*cbd UNUSED,
	ಪೂರ್ಣಾಙ್ಕ		row_arg,
	ಪೂರ್ಣಾಙ್ಕ		col_arg,
	ಪೂರ್ಣಾಙ್ಕ		height_arg,
	ಪೂರ್ಣಾಙ್ಕ		width_arg,
	ಪೂರ್ಣಾಙ್ಕ		invert)
{
    ಪೂರ್ಣಾಙ್ಕ		row = row_arg;
    ಪೂರ್ಣಾಙ್ಕ		col = col_arg;
    ಪೂರ್ಣಾಙ್ಕ		height = height_arg;
    ಪೂರ್ಣಾಙ್ಕ		width = width_arg;

# ifdef FEAT_PROP_POPUP
    // this goes on top of all popup windows
    screen_zindex = CLIP_ZINDEX;

    ಯದಿ (col < cbd->min_col)
    {
	width -= cbd->min_col - col;
	col = cbd->min_col;
    }
    ಯದಿ (width > cbd->max_col - col)
	width = cbd->max_col - col;
    ಯದಿ (row < cbd->min_row)
    {
	height -= cbd->min_row - row;
	row = cbd->min_row;
    }
    ಯದಿ (height > cbd->max_row - row + 1)
	height = cbd->max_row - row + 1;
# endif
# ifdef FEAT_GUI
    ಯದಿ (gui.in_use)
	gui_mch_invert_rectangle(row, col, height, width);
    ಅನ್ಯಥಾ
# endif
	screen_draw_rectangle(row, col, height, width, invert, FALSE);
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
    ಸ್ಥಿರ ಶೂನ್ಯ
clip_invert_area(
	Clipboard_T	*cbd,
	ಪೂರ್ಣಾಙ್ಕ		row1,
	ಪೂರ್ಣಾಙ್ಕ		col1,
	ಪೂರ್ಣಾಙ್ಕ		row2,
	ಪೂರ್ಣಾಙ್ಕ		col2,
	ಪೂರ್ಣಾಙ್ಕ		how)
{
    ಪೂರ್ಣಾಙ್ಕ		invert = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		max_col;

# ifdef FEAT_PROP_POPUP
    max_col = cbd->max_col - 1;
# ಅನ್ಯಥಾ
    max_col = Columns - 1;
# endif

    ಯದಿ (how == CLIP_SET)
	invert = TRUE;

    // Swap the from and to positions so the from is always before
    ಯದಿ (clip_compare_pos(row1, col1, row2, col2) > 0)
    {
	ಪೂರ್ಣಾಙ್ಕ tmp_row, tmp_col;

	tmp_row = row1;
	tmp_col = col1;
	row1	= row2;
	col1	= col2;
	row2	= tmp_row;
	col2	= tmp_col;
    }
    ಅನ್ಯಥಾ ಯದಿ (how == CLIP_TOGGLE)
	invert = TRUE;

    // If all on the same line, do it the easy way
    ಯದಿ (row1 == row2)
    {
	clip_invert_rectangle(cbd, row1, col1, 1, col2 - col1, invert);
    }
    ಅನ್ಯಥಾ
    {
	// Handle a piece of the first line
	ಯದಿ (col1 > 0)
	{
	    clip_invert_rectangle(cbd, row1, col1, 1,
						  (ಪೂರ್ಣಾಙ್ಕ)Columns - col1, invert);
	    row1++;
	}

	// Handle a piece of the last line
	ಯದಿ (col2 < max_col)
	{
	    clip_invert_rectangle(cbd, row2, 0, 1, col2, invert);
	    row2--;
	}

	// Handle the rectangle that's left
	ಯದಿ (row2 >= row1)
	    clip_invert_rectangle(cbd, row1, 0, row2 - row1 + 1,
							 (ಪೂರ್ಣಾಙ್ಕ)Columns, invert);
    }
}

/*
 * Start, continue or end a modeless selection.  Used when editing the
 * command-line, in the cmdline window and when the mouse is in a popup window.
 */
    ಶೂನ್ಯ
clip_modeless(ಪೂರ್ಣಾಙ್ಕ button, ಪೂರ್ಣಾಙ್ಕ is_click, ಪೂರ್ಣಾಙ್ಕ is_drag)
{
    ಪೂರ್ಣಾಙ್ಕ		repeat;

    repeat = ((clip_star.mode == SELECT_MODE_CHAR
		|| clip_star.mode == SELECT_MODE_ಸಾಲು)
					      && (mod_mask & MOD_MASK_2CLICK))
	    || (clip_star.mode == SELECT_MODE_WORD
					     && (mod_mask & MOD_MASK_3CLICK));
    ಯದಿ (is_click && button == MOUSE_ಬಲ)
    {
	// Right mouse button: If there was no selection, start one.
	// Otherwise extend the existing selection.
	ಯದಿ (clip_star.state == SELECT_CLEARED)
	    clip_start_selection(mouse_col, mouse_row, FALSE);
	clip_process_selection(button, mouse_col, mouse_row, repeat);
    }
    ಅನ್ಯಥಾ ಯದಿ (is_click)
	clip_start_selection(mouse_col, mouse_row, repeat);
    ಅನ್ಯಥಾ ಯದಿ (is_drag)
    {
	// Don't try extending a selection if there isn't one.  Happens when
	// button-down is in the cmdline and them moving mouse upwards.
	ಯದಿ (clip_star.state != SELECT_CLEARED)
	    clip_process_selection(button, mouse_col, mouse_row, repeat);
    }
    ಅನ್ಯಥಾ // release
	clip_process_selection(MOUSE_RELEASE, mouse_col, mouse_row, FALSE);
}

/*
 * Update the currently selected region by adding and/or subtracting from the
 * beginning or end and inverting the changed area(s).
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
clip_update_modeless_selection(
    Clipboard_T    *cb,
    ಪೂರ್ಣಾಙ್ಕ		    row1,
    ಪೂರ್ಣಾಙ್ಕ		    col1,
    ಪೂರ್ಣಾಙ್ಕ		    row2,
    ಪೂರ್ಣಾಙ್ಕ		    col2)
{
    // See if we changed at the beginning of the selection
    ಯದಿ (row1 != cb->start.lnum || col1 != (ಪೂರ್ಣಾಙ್ಕ)cb->start.col)
    {
	clip_invert_area(cb, row1, col1, (ಪೂರ್ಣಾಙ್ಕ)cb->start.lnum, cb->start.col,
								 CLIP_TOGGLE);
	cb->start.lnum = row1;
	cb->start.col  = col1;
    }

    // See if we changed at the end of the selection
    ಯದಿ (row2 != cb->end.lnum || col2 != (ಪೂರ್ಣಾಙ್ಕ)cb->end.col)
    {
	clip_invert_area(cb, (ಪೂರ್ಣಾಙ್ಕ)cb->end.lnum, cb->end.col, row2, col2,
								 CLIP_TOGGLE);
	cb->end.lnum = row2;
	cb->end.col  = col2;
    }
}

/*
 * Find the starting and ending positions of the word at the given row and
 * column.  Only white-separated words are recognized here.
 */
# define CHAR_CLASS(c)	(c <= ' ' ? ' ' : vim_iswordc(c))

    ಸ್ಥಿರ ಶೂನ್ಯ
clip_get_word_boundaries(Clipboard_T *cb, ಪೂರ್ಣಾಙ್ಕ row, ಪೂರ್ಣಾಙ್ಕ col)
{
    ಪೂರ್ಣಾಙ್ಕ		start_class;
    ಪೂರ್ಣಾಙ್ಕ		temp_col;
    char_u	*p;
    ಪೂರ್ಣಾಙ್ಕ		mboff;

    ಯದಿ (row >= screen_Rows || col >= screen_Columns || Screen_ಸಾಲುಗಳು == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    p = Screen_ಸಾಲುಗಳು + ಸಾಲು_Offset[row];
    // Correct for starting in the right half of a double-wide char
    ಯದಿ (enc_dbcs != 0)
	col -= dbcs_screen_head_off(p, p + col);
    ಅನ್ಯಥಾ ಯದಿ (enc_utf8 && p[col] == 0)
	--col;
    start_class = CHAR_CLASS(p[col]);

    temp_col = col;
    ಪ್ರತಿ ( ; temp_col > 0; temp_col--)
	ಯದಿ (enc_dbcs != 0
		   && (mboff = dbcs_screen_head_off(p, p + temp_col - 1)) > 0)
	    temp_col -= mboff;
	ಅನ್ಯಥಾ ಯದಿ (CHAR_CLASS(p[temp_col - 1]) != start_class
		&& !(enc_utf8 && p[temp_col - 1] == 0))
	    ಭಙ್ಗ;
    cb->word_start_col = temp_col;

    temp_col = col;
    ಪ್ರತಿ ( ; temp_col < screen_Columns; temp_col++)
	ಯದಿ (enc_dbcs != 0 && dbcs_ptr2cells(p + temp_col) == 2)
	    ++temp_col;
	ಅನ್ಯಥಾ ಯದಿ (CHAR_CLASS(p[temp_col]) != start_class
		&& !(enc_utf8 && p[temp_col] == 0))
	    ಭಙ್ಗ;
    cb->word_end_col = temp_col;
}

/*
 * Find the column position for the last non-whitespace character on the given
 * line at or before start_col.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
clip_get_ಸಾಲು_end(Clipboard_T *cbd UNUSED, ಪೂರ್ಣಾಙ್ಕ row)
{
    ಪೂರ್ಣಾಙ್ಕ	    i;

    ಯದಿ (row >= screen_Rows || Screen_ಸಾಲುಗಳು == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ 0;
    ಪ್ರತಿ (i =
# ifdef FEAT_PROP_POPUP
	    cbd->max_col;
# ಅನ್ಯಥಾ
	    screen_Columns;
# endif
			    i > 0; i--)
	ಯದಿ (Screen_ಸಾಲುಗಳು[ಸಾಲು_Offset[row] + i - 1] != ' ')
	    ಭಙ್ಗ;
    ಪ್ರತ್ಯಾಗಮ i;
}

/*
 * Start the selection
 */
    ಶೂನ್ಯ
clip_start_selection(ಪೂರ್ಣಾಙ್ಕ col, ಪೂರ್ಣಾಙ್ಕ row, ಪೂರ್ಣಾಙ್ಕ repeated_click)
{
    Clipboard_T	*cb = &clip_star;
# ifdef FEAT_PROP_POPUP
    win_T	*wp;
    ಪೂರ್ಣಾಙ್ಕ		row_cp = row;
    ಪೂರ್ಣಾಙ್ಕ		col_cp = col;

    wp = mouse_find_win(&row_cp, &col_cp, FIND_POPUP);
    ಯದಿ (wp != ರಿಕ್ತ && WIN_IS_POPUP(wp)
				  && popup_is_in_scrollbar(wp, row_cp, col_cp))
	// click or double click in scrollbar does not start a selection
	ಪ್ರತ್ಯಾಗಮ;
# endif

    ಯದಿ (cb->state == SELECT_DONE)
	clip_clear_selection(cb);

    row = check_row(row);
    col = check_col(col);
    col = mb_fix_col(col, row);

    cb->start.lnum  = row;
    cb->start.col   = col;
    cb->end	    = cb->start;
    cb->origin_row  = (short_u)cb->start.lnum;
    cb->state	    = SELECT_IN_PROGRESS;
# ifdef FEAT_PROP_POPUP
    ಯದಿ (wp != ರಿಕ್ತ && WIN_IS_POPUP(wp))
    {
	// Click in a popup window restricts selection to that window,
	// excluding the border.
	cb->min_col = wp->w_wincol + wp->w_popup_border[3];
	cb->max_col = wp->w_wincol + popup_width(wp)
				 - wp->w_popup_border[1] - wp->w_has_scrollbar;
	ಯದಿ (cb->max_col > screen_Columns)
	    cb->max_col = screen_Columns;
	cb->min_row = wp->w_winrow + wp->w_popup_border[0];
	cb->max_row = wp->w_winrow + popup_height(wp) - 1
						   - wp->w_popup_border[2];
    }
    ಅನ್ಯಥಾ
    {
	cb->min_col = 0;
	cb->max_col = screen_Columns;
	cb->min_row = 0;
	cb->max_row = screen_Rows;
    }
# endif

    ಯದಿ (repeated_click)
    {
	ಯದಿ (++cb->mode > SELECT_MODE_ಸಾಲು)
	    cb->mode = SELECT_MODE_CHAR;
    }
    ಅನ್ಯಥಾ
	cb->mode = SELECT_MODE_CHAR;

# ifdef FEAT_GUI
    // clear the cursor until the selection is made
    ಯದಿ (gui.in_use)
	gui_undraw_cursor();
# endif

    ವಿಕಲ್ಪ (cb->mode)
    {
	ಪಕ್ಷ SELECT_MODE_CHAR:
	    cb->origin_start_col = cb->start.col;
	    cb->word_end_col = clip_get_ಸಾಲು_end(cb, (ಪೂರ್ಣಾಙ್ಕ)cb->start.lnum);
	    ಭಙ್ಗ;

	ಪಕ್ಷ SELECT_MODE_WORD:
	    clip_get_word_boundaries(cb, (ಪೂರ್ಣಾಙ್ಕ)cb->start.lnum, cb->start.col);
	    cb->origin_start_col = cb->word_start_col;
	    cb->origin_end_col	 = cb->word_end_col;

	    clip_invert_area(cb, (ಪೂರ್ಣಾಙ್ಕ)cb->start.lnum, cb->word_start_col,
			    (ಪೂರ್ಣಾಙ್ಕ)cb->end.lnum, cb->word_end_col, CLIP_SET);
	    cb->start.col = cb->word_start_col;
	    cb->end.col   = cb->word_end_col;
	    ಭಙ್ಗ;

	ಪಕ್ಷ SELECT_MODE_ಸಾಲು:
	    clip_invert_area(cb, (ಪೂರ್ಣಾಙ್ಕ)cb->start.lnum, 0, (ಪೂರ್ಣಾಙ್ಕ)cb->start.lnum,
			    (ಪೂರ್ಣಾಙ್ಕ)Columns, CLIP_SET);
	    cb->start.col = 0;
	    cb->end.col   = Columns;
	    ಭಙ್ಗ;
    }

    cb->prev = cb->start;

# ifdef DEBUG_SELECTION
    printf("Selection started at (%ld,%d)\n", cb->start.lnum, cb->start.col);
# endif
}

/*
 * Continue processing the selection
 */
    ಶೂನ್ಯ
clip_process_selection(
    ಪೂರ್ಣಾಙ್ಕ		button,
    ಪೂರ್ಣಾಙ್ಕ		col,
    ಪೂರ್ಣಾಙ್ಕ		row,
    int_u	repeated_click)
{
    Clipboard_T	*cb = &clip_star;
    ಪೂರ್ಣಾಙ್ಕ		diff;
    ಪೂರ್ಣಾಙ್ಕ		slen = 1;	// cursor shape width

    ಯದಿ (button == MOUSE_RELEASE)
    {
	ಯದಿ (cb->state != SELECT_IN_PROGRESS)
	    ಪ್ರತ್ಯಾಗಮ;

	// Check to make sure we have something selected
	ಯದಿ (cb->start.lnum == cb->end.lnum && cb->start.col == cb->end.col)
	{
# ifdef FEAT_GUI
	    ಯದಿ (gui.in_use)
		gui_update_cursor(FALSE, FALSE);
# endif
	    cb->state = SELECT_CLEARED;
	    ಪ್ರತ್ಯಾಗಮ;
	}

# ifdef DEBUG_SELECTION
	printf("Selection ended: (%ld,%d) to (%ld,%d)\n", cb->start.lnum,
		cb->start.col, cb->end.lnum, cb->end.col);
# endif
	ಯದಿ (clip_isautosel_star() || clip_isautosel_plus()
		|| (
# ifdef FEAT_GUI
		    gui.in_use ? (vim_strchr(p_go, GO_ASELML) != ರಿಕ್ತ) :
# endif
		    clip_autoselectml))
	    clip_copy_modeless_selection(FALSE);
# ifdef FEAT_GUI
	ಯದಿ (gui.in_use)
	    gui_update_cursor(FALSE, FALSE);
# endif

	cb->state = SELECT_DONE;
	ಪ್ರತ್ಯಾಗಮ;
    }

    row = check_row(row);
    col = check_col(col);
    col = mb_fix_col(col, row);

    ಯದಿ (col == (ಪೂರ್ಣಾಙ್ಕ)cb->prev.col && row == cb->prev.lnum && !repeated_click)
	ಪ್ರತ್ಯಾಗಮ;

    /*
     * When extending the selection with the right mouse button, swap the
     * start and end if the position is before half the selection
     */
    ಯದಿ (cb->state == SELECT_DONE && button == MOUSE_ಬಲ)
    {
	/*
	 * If the click is before the start, or the click is inside the
	 * selection and the start is the closest side, set the origin to the
	 * end of the selection.
	 */
	ಯದಿ (clip_compare_pos(row, col, (ಪೂರ್ಣಾಙ್ಕ)cb->start.lnum, cb->start.col) < 0
		|| (clip_compare_pos(row, col,
					   (ಪೂರ್ಣಾಙ್ಕ)cb->end.lnum, cb->end.col) < 0
		    && (((cb->start.lnum == cb->end.lnum
			    && cb->end.col - col > col - cb->start.col))
			|| ((diff = (cb->end.lnum - row) -
						   (row - cb->start.lnum)) > 0
			    || (diff == 0 && col < (ಪೂರ್ಣಾಙ್ಕ)(cb->start.col +
							 cb->end.col) / 2)))))
	{
	    cb->origin_row = (short_u)cb->end.lnum;
	    cb->origin_start_col = cb->end.col - 1;
	    cb->origin_end_col = cb->end.col;
	}
	ಅನ್ಯಥಾ
	{
	    cb->origin_row = (short_u)cb->start.lnum;
	    cb->origin_start_col = cb->start.col;
	    cb->origin_end_col = cb->start.col;
	}
	ಯದಿ (cb->mode == SELECT_MODE_WORD && !repeated_click)
	    cb->mode = SELECT_MODE_CHAR;
    }

    // set state, for when using the right mouse button
    cb->state = SELECT_IN_PROGRESS;

# ifdef DEBUG_SELECTION
    printf("Selection extending to (%d,%d)\n", row, col);
# endif

    ಯದಿ (repeated_click && ++cb->mode > SELECT_MODE_ಸಾಲು)
	cb->mode = SELECT_MODE_CHAR;

    ವಿಕಲ್ಪ (cb->mode)
    {
	ಪಕ್ಷ SELECT_MODE_CHAR:
	    // If we're on a different line, find where the line ends
	    ಯದಿ (row != cb->prev.lnum)
		cb->word_end_col = clip_get_ಸಾಲು_end(cb, row);

	    // See if we are before or after the origin of the selection
	    ಯದಿ (clip_compare_pos(row, col, cb->origin_row,
						   cb->origin_start_col) >= 0)
	    {
		ಯದಿ (col >= (ಪೂರ್ಣಾಙ್ಕ)cb->word_end_col)
		    clip_update_modeless_selection(cb, cb->origin_row,
			    cb->origin_start_col, row, (ಪೂರ್ಣಾಙ್ಕ)Columns);
		ಅನ್ಯಥಾ
		{
		    ಯದಿ (has_mbyte && mb_lefthalve(row, col))
			slen = 2;
		    clip_update_modeless_selection(cb, cb->origin_row,
			    cb->origin_start_col, row, col + slen);
		}
	    }
	    ಅನ್ಯಥಾ
	    {
		ಯದಿ (has_mbyte
			&& mb_lefthalve(cb->origin_row, cb->origin_start_col))
		    slen = 2;
		ಯದಿ (col >= (ಪೂರ್ಣಾಙ್ಕ)cb->word_end_col)
		    clip_update_modeless_selection(cb, row, cb->word_end_col,
			    cb->origin_row, cb->origin_start_col + slen);
		ಅನ್ಯಥಾ
		    clip_update_modeless_selection(cb, row, col,
			    cb->origin_row, cb->origin_start_col + slen);
	    }
	    ಭಙ್ಗ;

	ಪಕ್ಷ SELECT_MODE_WORD:
	    // If we are still within the same word, do nothing
	    ಯದಿ (row == cb->prev.lnum && col >= (ಪೂರ್ಣಾಙ್ಕ)cb->word_start_col
		    && col < (ಪೂರ್ಣಾಙ್ಕ)cb->word_end_col && !repeated_click)
		ಪ್ರತ್ಯಾಗಮ;

	    // Get new word boundaries
	    clip_get_word_boundaries(cb, row, col);

	    // Handle being after the origin point of selection
	    ಯದಿ (clip_compare_pos(row, col, cb->origin_row,
		    cb->origin_start_col) >= 0)
		clip_update_modeless_selection(cb, cb->origin_row,
			cb->origin_start_col, row, cb->word_end_col);
	    ಅನ್ಯಥಾ
		clip_update_modeless_selection(cb, row, cb->word_start_col,
			cb->origin_row, cb->origin_end_col);
	    ಭಙ್ಗ;

	ಪಕ್ಷ SELECT_MODE_ಸಾಲು:
	    ಯದಿ (row == cb->prev.lnum && !repeated_click)
		ಪ್ರತ್ಯಾಗಮ;

	    ಯದಿ (clip_compare_pos(row, col, cb->origin_row,
		    cb->origin_start_col) >= 0)
		clip_update_modeless_selection(cb, cb->origin_row, 0, row,
			(ಪೂರ್ಣಾಙ್ಕ)Columns);
	    ಅನ್ಯಥಾ
		clip_update_modeless_selection(cb, row, 0, cb->origin_row,
			(ಪೂರ್ಣಾಙ್ಕ)Columns);
	    ಭಙ್ಗ;
    }

    cb->prev.lnum = row;
    cb->prev.col  = col;

# ifdef DEBUG_SELECTION
	printf("Selection is: (%ld,%d) to (%ld,%d)\n", cb->start.lnum,
		cb->start.col, cb->end.lnum, cb->end.col);
# endif
}

# ಯದಿ defined(FEAT_GUI)
/*
 * Redraw part of the selection if character at "row,col" is inside of it.
 * Only used for the GUI.
 */
    ಶೂನ್ಯ
clip_may_redraw_selection(ಪೂರ್ಣಾಙ್ಕ row, ಪೂರ್ಣಾಙ್ಕ col, ಪೂರ್ಣಾಙ್ಕ len)
{
    ಪೂರ್ಣಾಙ್ಕ		start = col;
    ಪೂರ್ಣಾಙ್ಕ		end = col + len;

    ಯದಿ (clip_star.state != SELECT_CLEARED
	    && row >= clip_star.start.lnum
	    && row <= clip_star.end.lnum)
    {
	ಯದಿ (row == clip_star.start.lnum && start < (ಪೂರ್ಣಾಙ್ಕ)clip_star.start.col)
	    start = clip_star.start.col;
	ಯದಿ (row == clip_star.end.lnum && end > (ಪೂರ್ಣಾಙ್ಕ)clip_star.end.col)
	    end = clip_star.end.col;
	ಯದಿ (end > start)
	    clip_invert_area(&clip_star, row, start, row, end, 0);
    }
}
# endif

/*
 * Called from outside to clear selected region from the display
 */
    ಶೂನ್ಯ
clip_clear_selection(Clipboard_T *cbd)
{

    ಯದಿ (cbd->state == SELECT_CLEARED)
	ಪ್ರತ್ಯಾಗಮ;

    clip_invert_area(cbd, (ಪೂರ್ಣಾಙ್ಕ)cbd->start.lnum, cbd->start.col,
				 (ಪೂರ್ಣಾಙ್ಕ)cbd->end.lnum, cbd->end.col, CLIP_CLEAR);
    cbd->state = SELECT_CLEARED;
}

/*
 * Clear the selection if any lines from "row1" to "row2" are inside of it.
 */
    ಶೂನ್ಯ
clip_may_clear_selection(ಪೂರ್ಣಾಙ್ಕ row1, ಪೂರ್ಣಾಙ್ಕ row2)
{
    ಯದಿ (clip_star.state == SELECT_DONE
	    && row2 >= clip_star.start.lnum
	    && row1 <= clip_star.end.lnum)
	clip_clear_selection(&clip_star);
}

/*
 * Called before the screen is scrolled up or down.  Adjusts the line numbers
 * of the selection.  Call with big number when clearing the screen.
 */
    ಶೂನ್ಯ
clip_scroll_selection(
    ಪೂರ್ಣಾಙ್ಕ	    rows)		// negative for scroll down
{
    ಪೂರ್ಣಾಙ್ಕ	    lnum;

    ಯದಿ (clip_star.state == SELECT_CLEARED)
	ಪ್ರತ್ಯಾಗಮ;

    lnum = clip_star.start.lnum - rows;
    ಯದಿ (lnum <= 0)
	clip_star.start.lnum = 0;
    ಅನ್ಯಥಾ ಯದಿ (lnum >= screen_Rows)	// scrolled off of the screen
	clip_star.state = SELECT_CLEARED;
    ಅನ್ಯಥಾ
	clip_star.start.lnum = lnum;

    lnum = clip_star.end.lnum - rows;
    ಯದಿ (lnum < 0)			// scrolled off of the screen
	clip_star.state = SELECT_CLEARED;
    ಅನ್ಯಥಾ ಯದಿ (lnum >= screen_Rows)
	clip_star.end.lnum = screen_Rows - 1;
    ಅನ್ಯಥಾ
	clip_star.end.lnum = lnum;
}

/*
 * Copy the currently selected area into the '*' or '+' register so it will be
 * available for pasting.
 * When "both" is TRUE also copy to the other register.
 */
    ಶೂನ್ಯ
clip_copy_modeless_selection(ಪೂರ್ಣಾಙ್ಕ both UNUSED)
{
    // The info for the modeless selection is stored in '*' register, however if
    // we are using the '+' register for modeless autoselect, we copy to
    // clip_plus instead while using the info in clip_star.
    Clipboard_T *cbd = clip_isautosel_plus() ? &clip_plus : &clip_star;
    char_u	*buffer;
    char_u	*bufp;
    ಪೂರ್ಣಾಙ್ಕ		row;
    ಪೂರ್ಣಾಙ್ಕ		start_col;
    ಪೂರ್ಣಾಙ್ಕ		end_col;
    ಪೂರ್ಣಾಙ್ಕ		ಸಾಲು_end_col;
    ಪೂರ್ಣಾಙ್ಕ		add_newline_flag = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		len;
    char_u	*p;
    ಪೂರ್ಣಾಙ್ಕ		row1 = clip_star.start.lnum;
    ಪೂರ್ಣಾಙ್ಕ		col1 = clip_star.start.col;
    ಪೂರ್ಣಾಙ್ಕ		row2 = clip_star.end.lnum;
    ಪೂರ್ಣಾಙ್ಕ		col2 = clip_star.end.col;

    // Can't use ScreenLines unless initialized
    ಯದಿ (Screen_ಸಾಲುಗಳು == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    /*
     * Make sure row1 <= row2, and if row1 == row2 that col1 <= col2.
     */
    ಯದಿ (row1 > row2)
    {
	row = row1; row1 = row2; row2 = row;
	row = col1; col1 = col2; col2 = row;
    }
    ಅನ್ಯಥಾ ಯದಿ (row1 == row2 && col1 > col2)
    {
	row = col1; col1 = col2; col2 = row;
    }
# ifdef FEAT_PROP_POPUP
    ಯದಿ (col1 < clip_star.min_col)
	col1 = clip_star.min_col;
    ಯದಿ (col2 > clip_star.max_col)
	col2 = clip_star.max_col;
    ಯದಿ (row1 > clip_star.max_row || row2 < clip_star.min_row)
	ಪ್ರತ್ಯಾಗಮ;
    ಯದಿ (row1 < clip_star.min_row)
	row1 = clip_star.min_row;
    ಯದಿ (row2 > clip_star.max_row)
	row2 = clip_star.max_row;
# endif
    // correct starting point for being on right half of double-wide char
    p = Screen_ಸಾಲುಗಳು + ಸಾಲು_Offset[row1];
    ಯದಿ (enc_dbcs != 0)
	col1 -= (*mb_head_off)(p, p + col1);
    ಅನ್ಯಥಾ ಯದಿ (enc_utf8 && p[col1] == 0)
	--col1;

    // Create a temporary buffer for storing the text
    len = (row2 - row1 + 1) * Columns + 1;
    ಯದಿ (enc_dbcs != 0)
	len *= 2;	// max. 2 bytes per display cell
    ಅನ್ಯಥಾ ಯದಿ (enc_utf8)
	len *= MB_MAXBYTES;
    // A newline is added after each row
    len += row2 - row1 + 2;
    buffer = alloc(len);
    ಯದಿ (buffer == ರಿಕ್ತ)	    // out of memory
	ಪ್ರತ್ಯಾಗಮ;

    // Process each row in the selection
    ಪ್ರತಿ (bufp = buffer, row = row1; row <= row2; row++)
    {
	ಯದಿ (row == row1)
	    start_col = col1;
	ಅನ್ಯಥಾ
# ifdef FEAT_PROP_POPUP
	    start_col = clip_star.min_col;
# ಅನ್ಯಥಾ
	    start_col = 0;
# endif

	ಯದಿ (row == row2)
	    end_col = col2;
	ಅನ್ಯಥಾ
# ifdef FEAT_PROP_POPUP
	    end_col = clip_star.max_col;
# ಅನ್ಯಥಾ
	    end_col = Columns;
# endif

	ಸಾಲು_end_col = clip_get_ಸಾಲು_end(&clip_star, row);

	// See if we need to nuke some trailing whitespace
	ಯದಿ (end_col >=
# ifdef FEAT_PROP_POPUP
		clip_star.max_col
# ಅನ್ಯಥಾ
		Columns
# endif
		    && (row < row2 || end_col > ಸಾಲು_end_col))
	{
	    // Get rid of trailing whitespace
	    end_col = ಸಾಲು_end_col;
	    ಯದಿ (end_col < start_col)
		end_col = start_col;

	    // If the last line extended to the end, add an extra newline
	    ಯದಿ (row == row2)
		add_newline_flag = TRUE;
	}

	// If after the first row, we need to always add a newline
	ಯದಿ (row > row1 && !ಸಾಲು_Wraps[row - 1])
	    *bufp++ = NL;

	// Safetey check for in case resizing went wrong
	ಯದಿ (row < screen_Rows && end_col <= screen_Columns)
	{
	    ಯದಿ (enc_dbcs != 0)
	    {
		ಪೂರ್ಣಾಙ್ಕ	i;

		p = Screen_ಸಾಲುಗಳು + ಸಾಲು_Offset[row];
		ಪ್ರತಿ (i = start_col; i < end_col; ++i)
		    ಯದಿ (enc_dbcs == DBCS_JPNU && p[i] == 0x8e)
		    {
			// single-width double-byte char
			*bufp++ = 0x8e;
			*bufp++ = ScreenLines2[ಸಾಲು_Offset[row] + i];
		    }
		    ಅನ್ಯಥಾ
		    {
			*bufp++ = p[i];
			ಯದಿ (MB_BYTE2LEN(p[i]) == 2)
			    *bufp++ = p[++i];
		    }
	    }
	    ಅನ್ಯಥಾ ಯದಿ (enc_utf8)
	    {
		ಪೂರ್ಣಾಙ್ಕ	off;
		ಪೂರ್ಣಾಙ್ಕ	i;

		off = ಸಾಲು_Offset[row];
		ಪ್ರತಿ (i = start_col; i < end_col; ++i)
		{
		    // The base character is either in ScreenLinesUC[] or
		    // ScreenLines[].
		    ಯದಿ (Screen_ಸಾಲುಗಳು_UC[off + i] == 0)
			*bufp++ = Screen_ಸಾಲುಗಳು[off + i];
		    ಅನ್ಯಥಾ
			// Includes the composing characters.
			bufp += utfc_char2bytes(off + i, bufp);
		    // Skip right half of double-wide character.
		    ಯದಿ (Screen_ಸಾಲುಗಳು[off + i + 1] == 0)
			++i;
		}
	    }
	    ಅನ್ಯಥಾ
	    {
		STRNCPY(bufp, Screen_ಸಾಲುಗಳು + ಸಾಲು_Offset[row] + start_col,
							 end_col - start_col);
		bufp += end_col - start_col;
	    }
	}
    }

    // Add a newline at the end if the selection ended there
    ಯದಿ (add_newline_flag)
	*bufp++ = NL;

    // First cleanup any old selection and become the owner.
    clip_free_selection(cbd);
    clip_own_selection(cbd);

    // Yank the text into the '*' register.
    clip_yank_selection(MCHAR, buffer, (long)(bufp - buffer), cbd);

    // Make the register contents available to the outside world.
    clip_gen_set_selection(cbd);

# ifdef FEAT_X11
    ಯದಿ (both)
    {
	Clipboard_T *other = cbd == &clip_star ? &clip_plus : &clip_star;
	// Do the same for the '+' register.
	clip_free_selection(other);
	clip_own_selection(other);
	clip_yank_selection(MCHAR, buffer, (long)(bufp - buffer), other);
	clip_gen_set_selection(other);
    }
# endif
    vim_free(buffer);
}

    ಶೂನ್ಯ
clip_gen_set_selection(Clipboard_T *cbd)
{
    ಯದಿ (!clip_did_set_selection)
    {
	// Updating postponed, so that accessing the system clipboard won't
	// hang Vim when accessing it many times (e.g. on a :g command).
	ಯದಿ ((cbd == &clip_plus && (clip_unnamed_saved & CLIP_UNNAMED_PLUS))
		|| (cbd == &clip_star && (clip_unnamed_saved & CLIP_UNNAMED)))
	{
	    clipboard_needs_update = TRUE;
	    ಪ್ರತ್ಯಾಗಮ;
	}
    }
# ಯದಿ defined(FEAT_XCLIPBOARD) || defined(FEAT_WAYLAND_CLIPBOARD)
#  ifdef FEAT_GUI
    ಯದಿ (gui.in_use)
	clip_mch_set_selection(cbd);
    ಅನ್ಯಥಾ
#  endif
    {
	ಯದಿ (clipmethod == CLIPMETHOD_WAYLAND)
	{
#  ifdef FEAT_WAYLAND_CLIPBOARD
	    clip_wl_set_selection(cbd);
#  endif
	}
	ಅನ್ಯಥಾ ಯದಿ (clipmethod == CLIPMETHOD_X11)
	{
#  ifdef FEAT_XCLIPBOARD
	    clip_xterm_set_selection(cbd);
#  endif
	}
    }
# ಅನ್ಯಥಾ
    clip_mch_set_selection(cbd);
# endif
}

    ಸ್ಥಿರ ಶೂನ್ಯ
clip_gen_request_selection(Clipboard_T *cbd)
{
# ಯದಿ defined(FEAT_XCLIPBOARD) || defined(FEAT_WAYLAND_CLIPBOARD)
#  ifdef FEAT_GUI
    ಯದಿ (gui.in_use)
	clip_mch_request_selection(cbd);
    ಅನ್ಯಥಾ
#  endif
    {
	ಯದಿ (clipmethod == CLIPMETHOD_WAYLAND)
	{
#  ifdef FEAT_WAYLAND_CLIPBOARD
	    clip_wl_request_selection(cbd);
#  endif
	}
	ಅನ್ಯಥಾ ಯದಿ (clipmethod == CLIPMETHOD_X11)
	{
#  ifdef FEAT_XCLIPBOARD
	    clip_xterm_request_selection(cbd);
#  endif
	}
    }
# ಅನ್ಯಥಾ
    clip_mch_request_selection(cbd);
# endif
}

# ಯದಿ (defined(FEAT_X11) && defined(FEAT_XCLIPBOARD) && defined(USE_SYSTEM))
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
clip_x11_owner_exists(Clipboard_T *cbd)
{
    ಪ್ರತ್ಯಾಗಮ XGetSelectionOwner(X_DISPLAY, cbd->sel_atom) != None;
}
# endif

# ಯದಿ (defined(FEAT_X11) || defined(FEAT_WAYLAND_CLIPBOARD)) \
	&& defined(USE_SYSTEM)
    ಪೂರ್ಣಾಙ್ಕ
clip_gen_owner_exists(Clipboard_T *cbd UNUSED)
{
#  ಯದಿ defined(FEAT_XCLIPBOARD) || defined(FEAT_WAYLAND_CLIPBOARD)
#   ifdef FEAT_GUI_GTK
    ಯದಿ (gui.in_use)
	ಪ್ರತ್ಯಾಗಮ clip_gtk_owner_exists(cbd);
    ಅನ್ಯಥಾ
#   endif
    {
	ಯದಿ (clipmethod == CLIPMETHOD_WAYLAND)
	{
#   ifdef FEAT_WAYLAND_CLIPBOARD
	    ಪ್ರತ್ಯಾಗಮ clip_wl_owner_exists(cbd);
#   endif
	}
	ಅನ್ಯಥಾ ಯದಿ (clipmethod == CLIPMETHOD_X11)
	{
#   ifdef FEAT_XCLIPBOARD
	    ಪ್ರತ್ಯಾಗಮ clip_x11_owner_exists(cbd);
#   endif
	}
	ಅನ್ಯಥಾ
	    ಪ್ರತ್ಯಾಗಮ FALSE;
    }
#  ಅನ್ಯಥಾ
    ಪ್ರತ್ಯಾಗಮ TRUE;
#  endif
}
# endif

/*
 * Stuff for the X clipboard.  Shared between VMS and Unix.
 */

# ಯದಿ defined(FEAT_XCLIPBOARD) || defined(FEAT_GUI_X11)
#  include <X11/Xatom.h>
#  include <X11/Intrinsic.h>

/*
 * Open the application context (if it hasn't been opened yet).
 * Used for Motif GUI and the xterm clipboard.
 */
    ಶೂನ್ಯ
open_app_context(ಶೂನ್ಯ)
{
    ಯದಿ (app_context == ರಿಕ್ತ)
    {
	XtToolkitInitialize();
	app_context = XtCreateApplicationContext();
    }
}

ಸ್ಥಿರ Atom	vim_atom;	// Vim's own special selection format
ಸ್ಥಿರ Atom	vimenc_atom;	// Vim's extended selection format
ಸ್ಥಿರ Atom	vim_mt_atom;	// Vim's own special selection format (in mime
				// type format)
ಸ್ಥಿರ Atom	vimenc_mt_atom;	// Vim's extended selection format (in mime type
				// format)
ಸ್ಥಿರ Atom	utf8_atom;
ಸ್ಥಿರ Atom	compound_text_atom;
ಸ್ಥಿರ Atom	text_atom;
ಸ್ಥಿರ Atom	targets_atom;
ಸ್ಥಿರ Atom	timestamp_atom;	// Used to get a timestamp

    ಶೂನ್ಯ
x11_setup_atoms(Display *dpy)
{
    vim_atom	       = XInternAtom(dpy, VIM_ATOM_NAME,   False);
    vimenc_atom	       = XInternAtom(dpy, VIMENC_ATOM_NAME,False);
    vim_mt_atom	       = XInternAtom(dpy, VIM_MIMETYPE_NAME,   False);
    vimenc_mt_atom     = XInternAtom(dpy, VIMENC_MIMETYPE_NAME,False);
    utf8_atom	       = XInternAtom(dpy, "UTF8_STRING",   False);
    compound_text_atom = XInternAtom(dpy, "COMPOUND_TEXT", False);
    text_atom	       = XInternAtom(dpy, "TEXT",	   False);
    targets_atom       = XInternAtom(dpy, "TARGETS",	   False);
    clip_star.sel_atom = XA_PRIMARY;
    clip_plus.sel_atom = XInternAtom(dpy, "CLIPBOARD",	   False);
    timestamp_atom     = XInternAtom(dpy, "TIMESTAMP",	   False);
}

/*
 * X Selection stuff, for cutting and pasting text to other windows.
 */

    ಸ್ಥಿರ Boolean
clip_x11_convert_selection_cb(
    Widget	w UNUSED,
    Atom	*sel_atom,
    Atom	*target,
    Atom	*type,
    XtPointer	*ಮೌಲ್ಯ,
    long_u	*length,
    ಪೂರ್ಣಾಙ್ಕ		*format)
{
    ಸ್ಥಿರ char_u   *save_result = ರಿಕ್ತ;
    ಸ್ಥಿರ long_u   save_length = 0;
    char_u	    *string;
    ಪೂರ್ಣಾಙ್ಕ		    motion_type;
    Clipboard_T    *cbd;
    ಪೂರ್ಣಾಙ್ಕ		    i;

    ಯದಿ (*sel_atom == clip_plus.sel_atom)
	cbd = &clip_plus;
    ಅನ್ಯಥಾ
	cbd = &clip_star;

    ಯದಿ (!cbd->owned)
	ಪ್ರತ್ಯಾಗಮ False;	    // Shouldn't ever happen

    // requestor wants to know what target types we support
    ಯದಿ (*target == targets_atom)
    {
	ಸ್ಥಿರ Atom array[9];

	*ಮೌಲ್ಯ = (XtPointer)array;
	i = 0;
	array[i++] = targets_atom;
	array[i++] = vimenc_atom;
	array[i++] = vim_atom;
	array[i++] = vimenc_mt_atom;
	array[i++] = vim_mt_atom;
	ಯದಿ (enc_utf8)
	    array[i++] = utf8_atom;
	array[i++] = XA_STRING;
	array[i++] = text_atom;
	array[i++] = compound_text_atom;

	*type = XA_ATOM;
	// This used to be: *format = sizeof(Atom) * 8; but that caused
	// crashes on 64 bit machines. (Peter Derr)
	*format = 32;
	*length = i;
	ಪ್ರತ್ಯಾಗಮ True;
    }

    ಯದಿ (       *target != XA_STRING
	    && *target != vimenc_atom
	    && *target != vimenc_mt_atom
	    && (*target != utf8_atom || !enc_utf8)
	    && *target != vim_atom
	    && *target != vim_mt_atom
	    && *target != text_atom
	    && *target != compound_text_atom)
	ಪ್ರತ್ಯಾಗಮ False;

    clip_get_selection(cbd);
    motion_type = clip_convert_selection(&string, length, cbd);
    ಯದಿ (motion_type < 0)
	ಪ್ರತ್ಯಾಗಮ False;

    // For our own format, the first byte contains the motion type
    ಯದಿ (*target == vim_atom || *target == vim_mt_atom)
	(*length)++;

    // Our own format with encoding: motion 'encoding' NUL text
    ಯದಿ (*target == vimenc_atom || *target == vimenc_mt_atom)
	*length += STRLEN(p_enc) + 2;

    ಯದಿ (save_length < *length || save_length / 2 >= *length)
	*ಮೌಲ್ಯ = XtRealloc((ವರ್ಣ *)save_result, (Cardinal)*length + 1);
    ಅನ್ಯಥಾ
	*ಮೌಲ್ಯ = save_result;
    ಯದಿ (*ಮೌಲ್ಯ == ರಿಕ್ತ)
    {
	vim_free(string);
	ಪ್ರತ್ಯಾಗಮ False;
    }
    save_result = (char_u *)*ಮೌಲ್ಯ;
    save_length = *length;

    ಯದಿ (*target == XA_STRING || (*target == utf8_atom && enc_utf8))
    {
	mch_memmove(save_result, string, (size_t)(*length));
	*type = *target;
    }
    ಅನ್ಯಥಾ ಯದಿ (*target == compound_text_atom || *target == text_atom)
    {
	XTextProperty	text_prop;
	ವರ್ಣ		*string_nt = (ವರ್ಣ *)save_result;
	ಪೂರ್ಣಾಙ್ಕ		conv_result;

	// create NUL terminated string which XmbTextListToTextProperty wants
	mch_memmove(string_nt, string, (size_t)*length);
	string_nt[*length] = NUL;
	conv_result = XmbTextListToTextProperty(X_DISPLAY, &string_nt,
					   1, XCompoundTextStyle, &text_prop);
	ಯದಿ (conv_result != Success)
	{
	    vim_free(string);
	    ಪ್ರತ್ಯಾಗಮ False;
	}
	*ಮೌಲ್ಯ = (XtPointer)(text_prop.ಮೌಲ್ಯ);	//    from plain text
	*length = text_prop.nitems;
	*type = compound_text_atom;
	XtFree((ವರ್ಣ *)save_result);
	save_result = (char_u *)*ಮೌಲ್ಯ;
	save_length = *length;
    }
    ಅನ್ಯಥಾ ಯದಿ (*target == vimenc_atom || *target == vimenc_mt_atom)
    {
	ಪೂರ್ಣಾಙ್ಕ l = STRLEN(p_enc);

	save_result[0] = motion_type;
	STRCPY(save_result + 1, p_enc);
	mch_memmove(save_result + l + 2, string, (size_t)(*length - l - 2));
	ಯದಿ (*target == vimenc_atom)
	    *type = vimenc_atom;
	ಅನ್ಯಥಾ
	    *type = vimenc_mt_atom;
    }
    ಅನ್ಯಥಾ
    {
	save_result[0] = motion_type;
	mch_memmove(save_result + 1, string, (size_t)(*length - 1));
	ಯದಿ (*target == vim_atom)
	    *type = vim_atom;
	ಅನ್ಯಥಾ
	    *type = vim_mt_atom;
    }
    *format = 8;	    // 8 bits per char
    vim_free(string);
    ಪ್ರತ್ಯಾಗಮ True;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
clip_x11_lose_ownership_cb(Widget w UNUSED, Atom *sel_atom)
{
    ಯದಿ (*sel_atom == clip_plus.sel_atom)
	clip_lose_selection(&clip_plus);
    ಅನ್ಯಥಾ
	clip_lose_selection(&clip_star);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
clip_x11_notify_cb(Widget w UNUSED, Atom *sel_atom UNUSED, Atom *target UNUSED)
{
    // To prevent automatically freeing the selection value.
}

/*
 * Property callback to get a timestamp for XtOwnSelection.
 */
#  ಯದಿ defined(FEAT_X11) && defined(FEAT_XCLIPBOARD)
    ಸ್ಥಿರ ಶೂನ್ಯ
clip_x11_timestamp_cb(
    Widget	w,
    XtPointer	n UNUSED,
    XEvent	*event,
    Boolean	*cont UNUSED)
{
    Atom	    actual_type;
    ಪೂರ್ಣಾಙ್ಕ		    format;
    unsigned  long  nitems, bytes_after;
    unsigned ವರ್ಣ   *prop=ರಿಕ್ತ;
    XPropertyEvent  *xproperty=&event->xproperty;

    // Must be a property notify, state can't be Delete (True), has to be
    // one of the supported selection types.
    ಯದಿ (event->type != PropertyNotify || xproperty->state
	    || (xproperty->atom != clip_star.sel_atom
				    && xproperty->atom != clip_plus.sel_atom))
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (XGetWindowProperty(xproperty->display, xproperty->window,
	  xproperty->atom, 0, 0, False, timestamp_atom, &actual_type, &format,
						&nitems, &bytes_after, &prop))
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (prop)
	XFree(prop);

    // Make sure the property type is "TIMESTAMP" and it's 32 bits.
    ಯದಿ (actual_type != timestamp_atom || format != 32)
	ಪ್ರತ್ಯಾಗಮ;

    // Get the selection, using the event timestamp.
    ಯದಿ (XtOwnSelection(w, xproperty->atom, xproperty->time,
	    clip_x11_convert_selection_cb, clip_x11_lose_ownership_cb,
	    clip_x11_notify_cb) == OK)
    {
	// Set the "owned" flag now, there may have been a call to
	// lose_ownership_cb in between.
	ಯದಿ (xproperty->atom == clip_plus.sel_atom)
	    clip_plus.owned = TRUE;
	ಅನ್ಯಥಾ
	    clip_star.owned = TRUE;
    }
}

    ಶೂನ್ಯ
x11_setup_selection(Widget w)
{
    XtAddEventHandler(w, PropertyChangeMask, False,
	    /*(XtEventHandler)*/clip_x11_timestamp_cb, (XtPointer)ರಿಕ್ತ);
}
#  endif

    ಸ್ಥಿರ ಶೂನ್ಯ
clip_x11_request_selection_cb(
    Widget	w UNUSED,
    XtPointer	success,
    Atom	*sel_atom,
    Atom	*type,
    XtPointer	ಮೌಲ್ಯ,
    long_u	*length,
    ಪೂರ್ಣಾಙ್ಕ		*format)
{
    ಪೂರ್ಣಾಙ್ಕ		motion_type = MAUTO;
    long_u	len;
    char_u	*p;
    ವರ್ಣ	**text_list = ರಿಕ್ತ;
    Clipboard_T	*cbd;
    char_u	*tmpbuf = ರಿಕ್ತ;

    ಯದಿ (*sel_atom == clip_plus.sel_atom)
	cbd = &clip_plus;
    ಅನ್ಯಥಾ
	cbd = &clip_star;

    ಯದಿ (ಮೌಲ್ಯ == ರಿಕ್ತ || *length == 0)
    {
	clip_free_selection(cbd);	// nothing received, clear register
	*(ಪೂರ್ಣಾಙ್ಕ *)success = FALSE;
	ಪ್ರತ್ಯಾಗಮ;
    }
    p = (char_u *)ಮೌಲ್ಯ;
    len = *length;
    ಯದಿ (*type == vim_atom || *type == vim_mt_atom)
    {
	motion_type = *p++;
	len--;
    }

    ಅನ್ಯಥಾ ಯದಿ (*type == vimenc_atom || *type == vimenc_mt_atom)
    {
	char_u		*enc;
	vimconv_T	conv;
	ಪೂರ್ಣಾಙ್ಕ		convlen;

	motion_type = *p++;
	--len;

	enc = p;
	p += STRLEN(p) + 1;
	len -= p - enc;

	// If the encoding of the text is different from 'encoding', attempt
	// converting it.
	conv.vc_type = CONV_NONE;
	convert_setup(&conv, enc, p_enc);
	ಯದಿ (conv.vc_type != CONV_NONE)
	{
	    convlen = len;	// Need to use an int here.
	    tmpbuf = string_convert(&conv, p, &convlen);
	    len = convlen;
	    ಯದಿ (tmpbuf != ರಿಕ್ತ)
		p = tmpbuf;
	    convert_setup(&conv, ರಿಕ್ತ, ರಿಕ್ತ);
	}
    }

    ಅನ್ಯಥಾ ಯದಿ (*type == compound_text_atom
	    || *type == utf8_atom
	    || (enc_dbcs != 0 && *type == text_atom))
    {
	XTextProperty	text_prop;
	ಪೂರ್ಣಾಙ್ಕ		n_text = 0;
	ಪೂರ್ಣಾಙ್ಕ		status;

	text_prop.ಮೌಲ್ಯ = (unsigned ವರ್ಣ *)ಮೌಲ್ಯ;
	text_prop.encoding = *type;
	text_prop.format = *format;
	text_prop.nitems = len;
#  ಯದಿ defined(X_HAVE_UTF8_STRING)
	ಯದಿ (*type == utf8_atom)
	    status = Xutf8TextPropertyToTextList(X_DISPLAY, &text_prop,
							 &text_list, &n_text);
	ಅನ್ಯಥಾ
#  endif
	    status = XmbTextPropertyToTextList(X_DISPLAY, &text_prop,
							 &text_list, &n_text);
	ಯದಿ (status != Success || n_text < 1)
	{
	    *(ಪೂರ್ಣಾಙ್ಕ *)success = FALSE;
	    ಪ್ರತ್ಯಾಗಮ;
	}
	p = (char_u *)text_list[0];
	len = STRLEN(p);
    }
    clip_yank_selection(motion_type, p, (long)len, cbd);

    ಯದಿ (text_list != ರಿಕ್ತ)
	XFreeStringList(text_list);
    vim_free(tmpbuf);
    XtFree((ವರ್ಣ *)ಮೌಲ್ಯ);
    *(ಪೂರ್ಣಾಙ್ಕ *)success = TRUE;
}

    ಶೂನ್ಯ
clip_x11_request_selection(
    Widget	myShell,
    Display	*dpy,
    Clipboard_T	*cbd)
{
    XEvent	event;
    Atom	type;
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	success;
    ಪೂರ್ಣಾಙ್ಕ		i;
    time_t	start_time;
    ಪೂರ್ಣಾಙ್ಕ		timed_out = FALSE;

    ಪ್ರತಿ (i = 0; i < 8; i++)
    {
	ವಿಕಲ್ಪ (i)
	{
	    ಪಕ್ಷ 0:  type = vimenc_mt_atom;	ಭಙ್ಗ;
	    ಪಕ್ಷ 1:  type = vimenc_atom;	ಭಙ್ಗ;
	    ಪಕ್ಷ 2:  type = vim_mt_atom;	ಭಙ್ಗ;
	    ಪಕ್ಷ 3:  type = vim_atom;		ಭಙ್ಗ;
	    ಪಕ್ಷ 4:  type = utf8_atom;		ಭಙ್ಗ;
	    ಪಕ್ಷ 5:  type = compound_text_atom; ಭಙ್ಗ;
	    ಪಕ್ಷ 6:  type = text_atom;		ಭಙ್ಗ;
	    ಸ್ವಭಾವ: type = XA_STRING;
	}
	ಯದಿ (type == utf8_atom
#  ಯದಿ defined(X_HAVE_UTF8_STRING)
		&& !enc_utf8
#  endif
		)
	    // Only request utf-8 when 'encoding' is utf8 and
	    // Xutf8TextPropertyToTextList is available.
	    ಅನುವರ್ತ;
	success = MAYBE;
	Xt_Get_Selection_ಮೌಲ್ಯ(myShell, cbd->sel_atom, type,
	    clip_x11_request_selection_cb, (XtPointer)&success, CurrentTime);

	// Make sure the request for the selection goes out before waiting for
	// a response.
	XFlush(dpy);

	/*
	 * Wait for result of selection request, otherwise if we type more
	 * characters, then they will appear before the one that requested the
	 * paste!  Don't worry, we will catch up with any other events later.
	 */
	start_time = time(ರಿಕ್ತ);
	ಯಾವತ್ (success == MAYBE)
	{
	    ಯದಿ (XCheckTypedEvent(dpy, PropertyNotify, &event)
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
		ಅನುವರ್ತ;
	    }

	    // Time out after 2 to 3 seconds to avoid that we hang when the
	    // other process doesn't respond.  Note that the SelectionNotify
	    // event may still come later when the selection owner comes back
	    // to life and the text gets inserted unexpectedly.  Don't know
	    // why that happens or how to avoid that :-(.
	    ಯದಿ (time(ರಿಕ್ತ) > start_time + 2)
	    {
		timed_out = TRUE;
		ಭಙ್ಗ;
	    }

	    // Do we need this?  Probably not.
	    XSync(dpy, False);

	    // Wait for 1 msec to avoid that we eat up all CPU time.
	    ui_delay(1L, TRUE);
	}

	ಯದಿ (success == TRUE)
	    ಪ್ರತ್ಯಾಗಮ;

	// don't do a retry with another type after timing out, otherwise we
	// hang for 15 seconds.
	ಯದಿ (timed_out)
	    ಭಙ್ಗ;
    }

    // Final fallback position - use the X CUT_BUFFER0 store
    yank_cut_buffer0(dpy, cbd);
}

    ಶೂನ್ಯ
clip_x11_lose_selection(Widget myShell, Clipboard_T *cbd)
{
    XtDisownSelection(myShell, cbd->sel_atom,
				XtLastTimestampProcessed(XtDisplay(myShell)));
}

    ಪೂರ್ಣಾಙ್ಕ
clip_x11_own_selection(Widget myShell, Clipboard_T *cbd)
{
    // When using the GUI we have proper timestamps, use the one of the last
    // event.  When in the console we don't get events (the terminal gets
    // them), Get the time by a zero-length append, clip_x11_timestamp_cb will
    // be called with the current timestamp.
#  ifdef FEAT_GUI
    ಯದಿ (gui.in_use)
    {
	ಯದಿ (XtOwnSelection(myShell, cbd->sel_atom,
	       XtLastTimestampProcessed(XtDisplay(myShell)),
	       clip_x11_convert_selection_cb, clip_x11_lose_ownership_cb,
	       clip_x11_notify_cb) == False)
	    ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ಅನ್ಯಥಾ
#  endif
    {
	ಯದಿ (!XChangeProperty(XtDisplay(myShell), XtWindow(myShell),
		  cbd->sel_atom, timestamp_atom, 32, PropModeAppend, ರಿಕ್ತ, 0))
	    ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    // Flush is required in a terminal as nothing else is doing it.
    XFlush(XtDisplay(myShell));
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Send the current selection to the clipboard.  Do nothing for X because we
 * will fill in the selection only when requested by another app.
 */
    ಶೂನ್ಯ
clip_x11_set_selection(Clipboard_T *cbd UNUSED)
{
}

# endif

# ಯದಿ (defined(FEAT_XCLIPBOARD) || defined(FEAT_GUI_X11) || defined(FEAT_GUI_GTK)) \
	&& !defined(USE_GTK4)
/*
 * Get the contents of the X CUT_BUFFER0 and put it in "cbd".
 */
    ಶೂನ್ಯ
yank_cut_buffer0(Display *dpy, Clipboard_T *cbd)
{
    ಪೂರ್ಣಾಙ್ಕ		nbytes = 0;
    char_u	*buffer = (char_u *)XFetchBuffer(dpy, &nbytes, 0);

    ಯದಿ (nbytes > 0)
    {
	ಪೂರ್ಣಾಙ್ಕ  done = FALSE;

	// CUT_BUFFER0 is supposed to be always latin1.  Convert to 'enc' when
	// using a multi-byte encoding.  Conversion between two 8-bit
	// character sets usually fails and the text might actually be in
	// 'enc' anyway.
	ಯದಿ (has_mbyte)
	{
	    char_u	*conv_buf;
	    vimconv_T	vc;

	    vc.vc_type = CONV_NONE;
	    ಯದಿ (convert_setup(&vc, (char_u *)"latin1", p_enc) == OK)
	    {
		conv_buf = string_convert(&vc, buffer, &nbytes);
		ಯದಿ (conv_buf != ರಿಕ್ತ)
		{
		    clip_yank_selection(MCHAR, conv_buf, (long)nbytes, cbd);
		    vim_free(conv_buf);
		    done = TRUE;
		}
		convert_setup(&vc, ರಿಕ್ತ, ರಿಕ್ತ);
	    }
	}
	ಯದಿ (!done)  // use the text without conversion
	    clip_yank_selection(MCHAR, buffer, (long)nbytes, cbd);
	XFree((ಶೂನ್ಯ *)buffer);
	ಯದಿ (p_verbose > 0)
	{
	    verbose_enter();
	    verb_ಸಂದೇಶ(_("Used CUT_BUFFER0 instead of empty selection"));
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
# ಯದಿ defined(FEAT_X11) && defined(FEAT_CLIPBOARD)
    ಶೂನ್ಯ
x11_export_final_selection(ಶೂನ್ಯ)
{
    Display	*dpy;
    char_u	*str = ರಿಕ್ತ;
    long_u	len = 0;
    ಪೂರ್ಣಾಙ್ಕ		motion_type = -1;

#  ifdef FEAT_GUI
    ಯದಿ (gui.in_use)
	dpy = X_DISPLAY;
    ಅನ್ಯಥಾ
#  endif
#  ifdef FEAT_XCLIPBOARD
	dpy = xterm_dpy;
#  ಅನ್ಯಥಾ
	ಪ್ರತ್ಯಾಗಮ;
#  endif

    // Get selection to export
    ಯದಿ (clip_plus.owned)
	motion_type = clip_convert_selection(&str, &len, &clip_plus);
    ಅನ್ಯಥಾ ಯದಿ (clip_star.owned)
	motion_type = clip_convert_selection(&str, &len, &clip_star);

    // Check it's OK
    ಯದಿ (dpy != ರಿಕ್ತ && str != ರಿಕ್ತ && motion_type >= 0
					       && len < 1024*1024 && len > 0)
    {
	ಪೂರ್ಣಾಙ್ಕ ok = TRUE;

	// The CUT_BUFFER0 is supposed to always contain latin1.  Convert from
	// 'enc' when it is a multi-byte encoding.  When 'enc' is an 8-bit
	// encoding conversion usually doesn't work, so keep the text as-is.
	ಯದಿ (has_mbyte)
	{
	    vimconv_T	vc;

	    vc.vc_type = CONV_NONE;
	    ಯದಿ (convert_setup(&vc, p_enc, (char_u *)"latin1") == OK)
	    {
		ಪೂರ್ಣಾಙ್ಕ	intlen = len;
		char_u	*conv_str;

		vc.vc_fail = TRUE;
		conv_str = string_convert(&vc, str, &intlen);
		len = intlen;
		ಯದಿ (conv_str != ರಿಕ್ತ)
		{
		    vim_free(str);
		    str = conv_str;
		}
		ಅನ್ಯಥಾ
		{
		    ok = FALSE;
		}
		convert_setup(&vc, ರಿಕ್ತ, ರಿಕ್ತ);
	    }
	    ಅನ್ಯಥಾ
	    {
		ok = FALSE;
	    }
	}

	// Do not store the string if conversion failed.  Better to use any
	// other selection than garbled text.
	ಯದಿ (ok)
	{
	    XStoreBuffer(dpy, (ವರ್ಣ *)str, (ಪೂರ್ಣಾಙ್ಕ)len, 0);
	    XFlush(dpy);
	}
    }

    vim_free(str);
}
# endif

    ಶೂನ್ಯ
clip_free_selection(Clipboard_T *cbd)
{
    yankreg_T *y_ptr = get_y_current();

    ಯದಿ (cbd == &clip_plus)
	set_y_current(get_y_register(PLUS_REGISTER));
    ಅನ್ಯಥಾ
	set_y_current(get_y_register(STAR_REGISTER));
    free_yank_all();
    get_y_current()->y_size = 0;
    set_y_current(y_ptr);
}

/*
 * Get the selected text and put it in register '*' or '+'.
 */
    ಶೂನ್ಯ
clip_get_selection(Clipboard_T *cbd)
{
    yankreg_T	*old_y_previous, *old_y_current;
    pos_T	old_cursor;
    pos_T	old_visual;
    ಪೂರ್ಣಾಙ್ಕ		old_visual_mode;
    colnr_T	old_virtcol;
    colnr_T	old_curswant;
    ಪೂರ್ಣಾಙ್ಕ		old_set_curswant;
    pos_T	old_op_start, old_op_end;
    oparg_T	oa;
    cmdarg_T	ca;

    ಯದಿ (cbd->owned)
    {
	ಯದಿ ((cbd == &clip_plus
		&& get_y_register(PLUS_REGISTER)->y_array != ರಿಕ್ತ)
		|| (cbd == &clip_star
		    && get_y_register(STAR_REGISTER)->y_array != ರಿಕ್ತ))
	    ಪ್ರತ್ಯಾಗಮ;

	// Avoid triggering autocmds such as TextYankPost.
	block_autocmds();

	// Get the text between clip_star.start & clip_star.end
	old_y_previous = get_y_previous();
	old_y_current = get_y_current();
	old_cursor = curwin->w_cursor;
	old_virtcol = curwin->w_virtcol;
	old_curswant = curwin->w_curswant;
	old_set_curswant = curwin->w_set_curswant;
	old_op_start = curbuf->b_op_start;
	old_op_end = curbuf->b_op_end;
	old_visual = VIsual;
	old_visual_mode = VIsual_mode;
	clear_oparg(&oa);
	oa.regname = (cbd == &clip_plus ? '+' : '*');
	oa.op_type = OP_YANK;
	CLEAR_FIELD(ca);
	ca.oap = &oa;
	ca.cmdchar = 'y';
	ca.count1 = 1;
	ca.retval = CA_NO_ADJ_OP_END;
	do_pending_operator(&ca, 0, TRUE);

	// restore things
	set_y_previous(old_y_previous);
	set_y_current(old_y_current);
	curwin->w_cursor = old_cursor;
	curwin->w_virtcol = old_virtcol;
	changed_cline_bef_curs();   // old w_virtcol et al. may be invalid
	curwin->w_curswant = old_curswant;
	curwin->w_set_curswant = old_set_curswant;
	curbuf->b_op_start = old_op_start;
	curbuf->b_op_end = old_op_end;
	VIsual = old_visual;
	VIsual_mode = old_visual_mode;

	unblock_autocmds();
    }
    ಅನ್ಯಥಾ ಯದಿ (!is_clipboard_needs_update())
    {
	clip_free_selection(cbd);

	// Try to get selected text from another window
	clip_gen_request_selection(cbd);
    }
}

/*
 * Convert from the GUI selection string into the '*'/'+' register.
 */
    ಶೂನ್ಯ
clip_yank_selection(
    ಪೂರ್ಣಾಙ್ಕ		type,
    char_u	*str,
    long	len,
    Clipboard_T *cbd)
{
    yankreg_T *y_ptr;

    ಯದಿ (cbd == &clip_plus)
	y_ptr = get_y_register(PLUS_REGISTER);
    ಅನ್ಯಥಾ
	y_ptr = get_y_register(STAR_REGISTER);

    clip_free_selection(cbd);

    str_to_reg(y_ptr, type, str, len, -1, FALSE);
}

    ಪೂರ್ಣಾಙ್ಕ
clip_convert_selection_offset(
	char_u	    **str,
	long_u	    *len,
	ಪೂರ್ಣಾಙ್ಕ	    offset, // Extra space to add in *str and the offset to
			    // place the actual string in *str.
	Clipboard_T *cbd)
{
    char_u	*p;
    ಪೂರ್ಣಾಙ್ಕ		lnum;
    ಪೂರ್ಣಾಙ್ಕ		i, j;
    int_u	eolsize;
    yankreg_T	*y_ptr;

    ಯದಿ (cbd == &clip_plus)
	y_ptr = get_y_register(PLUS_REGISTER);
    ಅನ್ಯಥಾ
	y_ptr = get_y_register(STAR_REGISTER);

# ifdef USE_CRNL
    eolsize = 2;
# ಅನ್ಯಥಾ
    eolsize = 1;
# endif

    *str = ರಿಕ್ತ;
    *len = 0;
    ಯದಿ (y_ptr->y_array == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ -1;

    ಪ್ರತಿ (i = 0; i < y_ptr->y_size; i++)
	*len += (long_u)y_ptr->y_array[i].length + eolsize;

    // Don't want newline character at end of last line if we're in MCHAR mode.
    ಯದಿ (y_ptr->y_type == MCHAR && *len >= eolsize)
	*len -= eolsize;

    *len += offset;
    p = *str = alloc(*len + 1);	// add one to avoid zero
    ಯದಿ (p == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ -1;
    p += offset;
    lnum = 0;
    ಪ್ರತಿ (i = 0, j = 0; i < (ಪೂರ್ಣಾಙ್ಕ)*len - offset; i++, j++)
    {
	ಯದಿ (y_ptr->y_array[lnum].string[j] == '\n')
	    p[i] = NUL;
	ಅನ್ಯಥಾ ಯದಿ (y_ptr->y_array[lnum].string[j] == NUL)
	{
# ifdef USE_CRNL
	    p[i++] = '\r';
# endif
	    p[i] = '\n';
	    lnum++;
	    j = -1;
	}
	ಅನ್ಯಥಾ
	    p[i] = y_ptr->y_array[lnum].string[j];
    }
    ಪ್ರತ್ಯಾಗಮ y_ptr->y_type;
}

/*
 * Convert the '*'/'+' register into a GUI selection string returned in *str
 * with length *len.
 * Returns the motion type, or -1 for failure.
 */
    ಪೂರ್ಣಾಙ್ಕ
clip_convert_selection(char_u **str, long_u *len, Clipboard_T *cbd)
{
    ಪ್ರತ್ಯಾಗಮ clip_convert_selection_offset(str, len, 0, cbd);
}

/*
 * When "regname" is a clipboard register, obtain the selection.  If it's not
 * available return zero, otherwise return "regname".
 */
    ಪೂರ್ಣಾಙ್ಕ
may_get_selection(ಪೂರ್ಣಾಙ್ಕ regname)
{
    ಯದಿ (regname == '*')
    {
	ಯದಿ (!clip_star.available)
	    regname = 0;
	ಅನ್ಯಥಾ
	    clip_get_selection(&clip_star);
    }
    ಅನ್ಯಥಾ ಯದಿ (regname == '+')
    {
	ಯದಿ (!clip_plus.available)
	    regname = 0;
	ಅನ್ಯಥಾ
	    clip_get_selection(&clip_plus);
    }
    ಪ್ರತ್ಯಾಗಮ regname;
}

/*
 * If we have written to a clipboard register, send the text to the clipboard.
 */
    ಶೂನ್ಯ
may_set_selection(ಶೂನ್ಯ)
{
    ಯದಿ ((get_y_current() == get_y_register(STAR_REGISTER))
	    && clip_star.available)
    {
	clip_own_selection(&clip_star);
	clip_gen_set_selection(&clip_star);
    }
    ಅನ್ಯಥಾ ಯದಿ ((get_y_current() == get_y_register(PLUS_REGISTER))
	    && clip_plus.available)
    {
	clip_own_selection(&clip_plus);
	clip_gen_set_selection(&clip_plus);
    }
}

# ಯದಿ defined(FEAT_WAYLAND_CLIPBOARD)

    ಸ್ಥಿರ clip_wl_selection_T *
clip_wl_get_selection(wayland_selection_T sel)
{
    ವಿಕಲ್ಪ (sel)
    {
	ಪಕ್ಷ WAYLAND_SELECTION_REGULAR:
	    ಪ್ರತ್ಯಾಗಮ &clip_wl.regular;
	ಪಕ್ಷ WAYLAND_SELECTION_PRIMARY:
	    ಪ್ರತ್ಯಾಗಮ &clip_wl.primary;
	ಸ್ವಭಾವ:
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
}

    ಸ್ಥಿರ clip_wl_selection_T *
clip_wl_get_selection_from_cbd(Clipboard_T *cbd)
{
    ಯದಿ (cbd == &clip_plus)
	ಪ್ರತ್ಯಾಗಮ &clip_wl.regular;
    ಅನ್ಯಥಾ ಯದಿ (cbd == &clip_star)
	ಪ್ರತ್ಯಾಗಮ &clip_wl.primary;
    ಅನ್ಯಥಾ
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

    ಸ್ಥಿರ Clipboard_T *
clip_wl_get_cbd_from_selection(clip_wl_selection_T *sel)
{
    ಯದಿ (sel == &clip_wl.regular)
	ಪ್ರತ್ಯಾಗಮ &clip_plus;
    ಅನ್ಯಥಾ ಯದಿ (sel == &clip_wl.primary)
	ಪ್ರತ್ಯಾಗಮ &clip_star;
    ಅನ್ಯಥಾ
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

    ಸ್ಥಿರ wayland_selection_T
clip_wl_get_selection_type(clip_wl_selection_T *sel)
{
    ಯದಿ (sel == &clip_wl.regular)
	ಪ್ರತ್ಯಾಗಮ WAYLAND_SELECTION_REGULAR;
    ಅನ್ಯಥಾ ಯದಿ (sel == &clip_wl.primary)
	ಪ್ರತ್ಯಾಗಮ WAYLAND_SELECTION_PRIMARY;
    ಅನ್ಯಥಾ
	ಪ್ರತ್ಯಾಗಮ WAYLAND_SELECTION_NONE;
}

    ಸ್ಥಿರ bool
wl_data_offer_listener_event_offer(
    ಶೂನ್ಯ *data UNUSED,
    vwl_data_offer_T *offer UNUSED,
    ಧ್ರುವ ವರ್ಣ *mime_type
)
{
    // Only accept mime type if we support it
    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < (ಪೂರ್ಣಾಙ್ಕ)ARRAY_LENGTH(supported_mimes); i++)
	ಯದಿ (STRCMP(mime_type, supported_mimes[i]) == 0)
	    ಪ್ರತ್ಯಾಗಮ true;
    ಪ್ರತ್ಯಾಗಮ FALSE;
}

ಸ್ಥಿರ ಧ್ರುವ vwl_data_offer_listener_T vwl_data_offer_listener = {
    .offer = wl_data_offer_listener_event_offer
};

    ಸ್ಥಿರ ಶೂನ್ಯ
vwl_data_device_listener_event_data_offer(
	ಶೂನ್ಯ *data UNUSED,
	vwl_data_device_T *device UNUSED,
	vwl_data_offer_T *offer)
{
    // Immediately start listening for offer events from the data offer
    vwl_data_offer_add_listener(offer, &vwl_data_offer_listener, ರಿಕ್ತ);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
vwl_data_device_listener_event_selection(
	ಶೂನ್ಯ *data UNUSED,
	vwl_data_device_T *device UNUSED,
	vwl_data_offer_T *offer,
	wayland_selection_T selection)
{
    clip_wl_selection_T *sel = clip_wl_get_selection(selection);

    // Destroy previous offer if any, it is now invalid
    vwl_data_offer_destroy(sel->offer);

    // There are two cases when sel->offer is NULL
    // 1. No one owns the selection
    // 2. We own the selection (we'll just access the register directly)
    ಯದಿ (offer == ರಿಕ್ತ || sel->source != ರಿಕ್ತ)
    {
	// Selection event is from us, so we are the source client. Therefore
	// ignore it. Or the selection is cleared, so set sel->offer to NULL
	vwl_data_offer_destroy(offer);
	sel->offer = ರಿಕ್ತ;
	ಪ್ರತ್ಯಾಗಮ;
    }

    // Save offer. When we want to request data, then we'll actually call the
    // receive method.
    sel->offer = offer;

}

    ಸ್ಥಿರ ಶೂನ್ಯ
vwl_data_device_listener_event_finished(
	ಶೂನ್ಯ *data UNUSED,
	vwl_data_device_T *device)
{
    clip_wl_selection_T *sel;
    // Device finished, guessing this can happen is when the seat becomes
    // invalid? If so, let the user call :wlrestore! to reset. There wouldn't be
    // any point in trying to create another data device for the same seat,
    // since the seat is in an invalid state.
    ಯದಿ (device == clip_wl.regular.device)
    {
	sel = &clip_wl.regular;
	clip_wl.regular.device = ರಿಕ್ತ;
    }
    ಅನ್ಯಥಾ ಯದಿ (device == clip_wl.primary.device)
    {
	sel = &clip_wl.primary;
	clip_wl.primary.device = ರಿಕ್ತ;
    }
    ಅನ್ಯಥಾ
	// Shouldn't happen
	ಪ್ರತ್ಯಾಗಮ;

    vim_free(sel->contents);
    vwl_data_source_destroy(sel->source);
    vwl_data_offer_destroy(sel->offer);
    sel->available = FALSE;

    vwl_data_device_destroy(device);
}

ಸ್ಥಿರ ಧ್ರುವ vwl_data_device_listener_T vwl_data_device_listener = {
    .data_offer = vwl_data_device_listener_event_data_offer,
    .selection = vwl_data_device_listener_event_selection,
    .finished = vwl_data_device_listener_event_finished
};

/*
 * Initialize the clipboard for Wayland using the global Wayland connection.
 * Returns OK on success and FAIL on failure.
 */
    ಪೂರ್ಣಾಙ್ಕ
clip_init_wayland(ಶೂನ್ಯ)
{
    int_u supported = WAYLAND_SELECTION_NONE;

    ಯದಿ (wayland_ct == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    memset(&clip_wl, 0, ಪರಿಮಾಣ(clip_wl));

    clip_wl.seat = vwl_connection_get_seat(wayland_ct, (ವರ್ಣ *)p_wse);

    ಯದಿ (clip_wl.seat == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    clip_wl.regular.manager = vwl_connection_get_data_device_manager(
	    wayland_ct, WAYLAND_SELECTION_REGULAR, &supported);

    ಯದಿ (clip_wl.regular.manager != ರಿಕ್ತ)
    {
	clip_wl.regular.device = vwl_data_device_manager_get_data_device(
		clip_wl.regular.manager, clip_wl.seat);

	ಯದಿ (clip_wl.regular.device != ರಿಕ್ತ)
	    clip_wl.regular.available = true;
	ಅನ್ಯಥಾ
	{
	    // Shouldn't happen
	    vwl_data_device_manager_discard(clip_wl.regular.manager);
	    clip_wl.regular.manager = ರಿಕ್ತ;
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}
    }

    // If we still don't support the primary selection, try finding one for it
    // specifically.
    ಯದಿ (!(supported & WAYLAND_SELECTION_PRIMARY))
    {
	clip_wl.primary.manager = vwl_connection_get_data_device_manager(
		wayland_ct, WAYLAND_SELECTION_PRIMARY, &supported);

	ಯದಿ (clip_wl.primary.manager != ರಿಕ್ತ)
	{
	    clip_wl.primary.device = vwl_data_device_manager_get_data_device(
		    clip_wl.primary.manager, clip_wl.seat);

	    ಯದಿ (clip_wl.primary.device != ರಿಕ್ತ)
		clip_wl.primary.available = true;
	    ಅನ್ಯಥಾ
	    {
		vwl_data_device_manager_discard(clip_wl.primary.manager);
		clip_wl.primary.manager = ರಿಕ್ತ;
	    }
	}
    }

    ಯದಿ (clip_wl.regular.available && !clip_wl.primary.available)
    {
	// The protocol supports both regular and primary selections, just use
	// one data device manager and one data device. Or the primary selection
	// is not supported, make it point to the regular selection instead.
	clip_wl.primary.available = true;
	clip_wl.primary.manager = clip_wl.regular.manager;
	clip_wl.primary.device = clip_wl.regular.device;
    }

    ಯದಿ (!clip_wl.regular.available && !clip_wl.primary.available)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    // Start listening for selection updates
    ಯದಿ (clip_wl.regular.device != ರಿಕ್ತ)
	vwl_data_device_add_listener(clip_wl.regular.device,
		&vwl_data_device_listener, ರಿಕ್ತ);
    // Don't want to listen to the same data device twice
    ಯದಿ (clip_wl.primary.device != ರಿಕ್ತ
	    && clip_wl.primary.device != clip_wl.regular.device)
	vwl_data_device_add_listener(clip_wl.primary.device,
		&vwl_data_device_listener, ರಿಕ್ತ);

    ಪ್ರತ್ಯಾಗಮ OK;
}

    ಶೂನ್ಯ
clip_uninit_wayland(ಶೂನ್ಯ)
{
    clip_wl_selection_T *sel;

    ಯದಿ (clipmethod == CLIPMETHOD_WAYLAND)
    {
	ಯದಿ (clip_star.owned)
	    clip_lose_selection(&clip_star);
	ಯದಿ (clip_plus.owned)
	    clip_lose_selection(&clip_plus);
    }

    // Don't want to double free
    ಯದಿ (clip_wl.regular.manager != clip_wl.primary.manager)
	vwl_data_device_manager_discard(clip_wl.primary.manager);
    vwl_data_device_manager_discard(clip_wl.regular.manager);

    ಯದಿ (clip_wl.regular.device != clip_wl.primary.device)
	vwl_data_device_destroy(clip_wl.primary.device);
    vwl_data_device_destroy(clip_wl.regular.device);

    sel = &clip_wl.regular;
    ಯಾವತ್ (true)
    {
	vim_free(sel->contents);
	vwl_data_source_destroy(sel->source);
	vwl_data_offer_destroy(sel->offer);
	sel->available = false;

	ಯದಿ (sel == &clip_wl.primary)
	    ಭಙ್ಗ;
	sel = &clip_wl.primary;
    }

    vim_memset(&clip_wl, 0, ಪರಿಮಾಣ(clip_wl));
}

    ಪೂರ್ಣಾಙ್ಕ
clip_reset_wayland(ಶೂನ್ಯ)
{
    wayland_uninit_connection();

    ಯದಿ (wayland_init_connection(wayland_display_name) == FAIL
	    || clip_init_wayland() == FAIL)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    choose_clipmethod();
    ಪ್ರತ್ಯಾಗಮ OK;
}


/*
 * Read data from a file descriptor and write it to the given clipboard.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
clip_wl_receive_data(Clipboard_T *cbd, ಧ್ರುವ ವರ್ಣ *mime_type, ಪೂರ್ಣಾಙ್ಕ fd)
{
    char_u	*start, *final;
    long	len;
    garray_T	buf;
    ಪೂರ್ಣಾಙ್ಕ		motion_type = MAUTO;
    ssize_t	r = 0;
    char_u	*tofree = ರಿಕ್ತ;
#  ifndef HAVE_SELECT
    ಸಂರಚನಾ pollfd   pfd;

    pfd.fd = fd;
    pfd.events = POLLIN;
#  ಅನ್ಯಥಾ
    fd_set rfds;
    ಸಂರಚನಾ timeval  tv;

    FD_ZERO(&rfds);
    FD_SET(fd, &rfds);
#  endif

    // Make pipe (read end) non-blocking
    ಯದಿ (fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK) == -1)
	ಪ್ರತ್ಯಾಗಮ;

    ga_init2(&buf, 1, 4096);

    // 4096 bytes seems reasonable for initial buffer size, memory is cheap
    // anyways.
    ಯದಿ (ga_grow(&buf, 4096) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    start = buf.ga_data;

#  ifndef HAVE_SELECT
    ಯಾವತ್ (poll(&pfd, 1, p_wtm) > 0)
#  ಅನ್ಯಥಾ
    ಯಾವತ್ (tv.tv_sec = p_wtm / 1000, tv.tv_usec = (p_wtm % 1000) * 1000,
	    select(fd + 1, &rfds, ರಿಕ್ತ, ರಿಕ್ತ, &tv) > 0)
#  endif
    {
	r = ಪಠ(fd, start, buf.ga_maxlen - 1 - buf.ga_len);

	ಯದಿ (r == 0)
	    ಭಙ್ಗ;
	ಅನ್ಯಥಾ ಯದಿ (r < 0)
	{
	    ಯದಿ (errno == EAGAIN || errno == EINTR)
		ಅನುವರ್ತ;
	    ಭಙ್ಗ;
	}

	start += r;
	buf.ga_len += r;

	// Realloc if we are at the end of the buffer
	ಯದಿ (buf.ga_len >= buf.ga_maxlen - 1)
	{
	    ಯದಿ (ga_grow(&buf, 8192) == FAIL)
		ಭಙ್ಗ;
	    start = (char_u *)buf.ga_data + buf.ga_len;
	}
    }

    ಯದಿ (buf.ga_len == 0)
    {
	clip_free_selection(cbd); // Nothing received, clear register
	ga_clear(&buf);
	ಪ್ರತ್ಯಾಗಮ;
    }

    final = buf.ga_data;
    len = buf.ga_len;

    ಯದಿ (clip_convert_data(&final, &len, &motion_type,
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
    ಸ್ಥಿರ ಶೂನ್ಯ
clip_wl_request_selection(Clipboard_T *cbd)
{
    clip_wl_selection_T *sel = clip_wl_get_selection_from_cbd(cbd);
    ಪೂರ್ಣಾಙ್ಕ			fds[2];
    ಪೂರ್ಣಾಙ್ಕ			mime_types_len;
    ಧ್ರುವ ವರ್ಣ		**mime_types;
    ಧ್ರುವ ವರ್ಣ		*chosen_mime = ರಿಕ್ತ;

    ಯದಿ (!sel->available)
	ಗಚ್ಛ clear;

    // Dispatch any events that still queued up before checking for a data
    // offer.
    ಯದಿ (vwl_connection_roundtrip(wayland_ct) == FAIL)
	ಗಚ್ಛ clear;

    ಯದಿ (sel->offer == ರಿಕ್ತ)
	ಗಚ್ಛ clear;

    mime_types_len = sel->offer->mime_types.ga_len;
    mime_types = sel->offer->mime_types.ga_data;

    // Choose mime type to receive from. Mime types with a lower index in the
    // "supported_mimes" array are prioritized over ones after it.
    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < (ಪೂರ್ಣಾಙ್ಕ)ARRAY_LENGTH(supported_mimes)
	    && chosen_mime == ರಿಕ್ತ; i++)
    {
	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ k = 0; k < mime_types_len && chosen_mime == ರಿಕ್ತ; k++)
	    ಯದಿ (STRCMP(mime_types[k], supported_mimes[i]) == 0)
		chosen_mime = supported_mimes[i];
    }

    ಯದಿ (chosen_mime == ರಿಕ್ತ || pipe(fds) == -1)
	ಗಚ್ಛ clear;

    vwl_data_offer_receive(sel->offer, chosen_mime, fds[1]);

    close(fds[1]); // Close before we read data so that when the source client
		   // closes their end we receive an EOF.

    ಯದಿ (vwl_connection_flush(wayland_ct) >= 0)
	clip_wl_receive_data(cbd, chosen_mime, fds[0]);

    close(fds[0]);

    ಪ್ರತ್ಯಾಗಮ;
clear:
    clip_free_selection(cbd);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
vwl_data_source_listener_event_send(
    ಶೂನ್ಯ *data,
    vwl_data_source_T *source UNUSED,
    ಧ್ರುವ ವರ್ಣ *mime_type,
    int32_t fd
)
{
    clip_wl_selection_T *sel = data;
    Clipboard_T		*cbd = clip_wl_get_cbd_from_selection(sel);
    bool		have_mime = false;
    ಪೂರ್ಣಾಙ್ಕ			motion_type;
    long_u		length;
    char_u		*string; // Will be reallocated to a bigger size if
				 // needed.
    ಪೂರ್ಣಾಙ್ಕ			offset = 0;
    bool		is_vim, is_vimenc;
    size_t		total = 0;
#  ifndef HAVE_SELECT
    ಸಂರಚನಾ pollfd   pfd;

    pfd.fd = fd;
    pfd.events = POLLOUT;
#  ಅನ್ಯಥಾ
    fd_set	    wfds;
    ಸಂರಚನಾ timeval  tv;

    FD_ZERO(&wfds);
    FD_SET(fd, &wfds);
#  endif

    // Check if we actually have mime type
    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < (ಪೂರ್ಣಾಙ್ಕ)ARRAY_LENGTH(supported_mimes); i++)
	ಯದಿ (STRCMP(supported_mimes[i], mime_type) == 0)
	{
	    have_mime = true;
	    ಭಙ್ಗ;
	}

    ಯದಿ (!have_mime)
	ಗಚ್ಛ exit;

    // First byte sent is motion type for vim specific formats. For the vimenc
    // format, after the first byte is the encoding type, which is null
    // terminated.

    is_vimenc = STRCMP(mime_type, VIMENC_ATOM_NAME) == 0
	|| STRCMP(mime_type, VIMENC_MIMETYPE_NAME) == 0;
    is_vim = STRCMP(mime_type, VIM_ATOM_NAME) == 0
	|| STRCMP(mime_type, VIM_MIMETYPE_NAME) == 0;

    ಯದಿ (is_vimenc)
	offset += 2 + STRLEN(p_enc);
    ಅನ್ಯಥಾ ಯದಿ (is_vim)
	offset += 1;

    clip_get_selection(cbd);
    motion_type = clip_convert_selection_offset(&string, &length, offset, cbd);

    ಯದಿ (motion_type < 0)
	ಗಚ್ಛ exit;

    ಯದಿ (is_vimenc)
    {
	string[0] = (char_u)motion_type;
	// Use vim_strncpy for safer copying
	vim_strncpy(string + 1, p_enc, STRLEN(p_enc));
    }
    ಅನ್ಯಥಾ ಯದಿ (is_vim)
	string[0] = (char_u)motion_type;


    ಯಾವತ್ (total < (size_t)length &&
#  ifndef HAVE_SELECT
	    poll(&pfd, 1, p_wtm) > 0)
#  ಅನ್ಯಥಾ
	    ((tv.tv_sec = p_wtm / 1000, tv.tv_usec = (p_wtm % 1000) * 1000),
	    select(fd + 1, ರಿಕ್ತ, &wfds, ರಿಕ್ತ, &tv) > 0))
#  endif
    {
	ssize_t w = ಲಿಖ(fd, string + total, length - total);

	ಯದಿ (w == -1)
	    ಭಙ್ಗ;
	total += w;
    }

    vim_free(string);
exit:
    close(fd);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
vwl_data_source_listener_event_cancelled(
	ಶೂನ್ಯ *data,
	vwl_data_source_T *source UNUSED)
{
    clip_wl_selection_T *sel = data;
    Clipboard_T		*cbd = clip_wl_get_cbd_from_selection(sel);

    clip_lose_selection(cbd);
}

ಸ್ಥಿರ ಧ್ರುವ vwl_data_source_listener_T vwl_data_source_listener = {
    .send = vwl_data_source_listener_event_send,
    .cancelled = vwl_data_source_listener_event_cancelled
};

    ಸ್ಥಿರ ಶೂನ್ಯ
clip_wl_do_set_selection(ಶೂನ್ಯ *data, uint32_t serial)
{
    clip_wl_selection_T *sel = data;
    wayland_selection_T sel_type = clip_wl_get_selection_type(sel);

    vwl_data_device_set_selection(sel->device, sel->source, serial, sel_type);

    sel->own_success = (vwl_connection_roundtrip(wayland_ct) == OK);
}

/*
 * Own the selection that cbd corresponds to. Start listening for requests from
 * other Wayland clients so they can receive data from us. Returns OK on success
 * and FAIL on failure.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
clip_wl_own_selection(Clipboard_T *cbd)
{
    clip_wl_selection_T *sel = clip_wl_get_selection_from_cbd(cbd);
    wayland_selection_T sel_type = clip_wl_get_selection_type(sel);

    ಯದಿ (!sel->available || vwl_connection_roundtrip(wayland_ct) == FAIL)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    ಯದಿ (sel->source != ರಿಕ್ತ)
    {
	ಯದಿ (sel_type == WAYLAND_SELECTION_PRIMARY)
	    // We already own the selection, ignore (only do this for primary
	    // selection). We don't re set the selection because then we would
	    // be setting the selection every time the user moves the visual
	    // selection cursor, which is messy and inefficient. Some
	    // applications like Google Chrome do it this way however.
	    ಪ್ರತ್ಯಾಗಮ OK;
	ಅನ್ಯಥಾ ಯದಿ (sel_type == WAYLAND_SELECTION_REGULAR)
	{
	    // Technically we don't need to do this as we already own the
	    // selection, however if a user yanks text a second time, the
	    // text yanked won't appear in their clipboard manager if they are
	    // using one.
	    //
	    // This can be unexpected behaviour for the user so its probably
	    // better to do it this way. Additionally other Wayland applications
	    // seem to set the selection every time.
	    vwl_data_source_destroy(sel->source);
	}
	ಅನ್ಯಥಾ
	    // Shouldn't happen
	    ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    sel->source = vwl_data_device_manager_create_data_source(sel->manager);
    vwl_data_source_add_listener(sel->source, &vwl_data_source_listener, sel);

    // Advertise mime types
    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < (ಪೂರ್ಣಾಙ್ಕ)ARRAY_LENGTH(supported_mimes); i++)
	vwl_data_source_offer(sel->source, supported_mimes[i]);

    sel->own_success = false;
    clip_wl_do_set_selection(sel, 0);

    ಯದಿ (!sel->own_success)
	ಗಚ್ಛ fail;

    ಪ್ರತ್ಯಾಗಮ OK;
fail:
    vwl_data_source_destroy(sel->source);
    sel->source = ರಿಕ್ತ;
    ಪ್ರತ್ಯಾಗಮ FAIL;
}

/*
 * Disown the selection that cbd corresponds to.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
clip_wl_lose_selection(Clipboard_T *cbd)
{
    clip_wl_selection_T *sel = clip_wl_get_selection_from_cbd(cbd);

    ಯದಿ (!sel->available)
	ಪ್ರತ್ಯಾಗಮ;

    vwl_data_source_destroy(sel->source);
    sel->source = ರಿಕ್ತ;
}

/*
 * Send the current selection to the clipboard. Do nothing for Wayland because
 * we will fill in the selection only when requested by another client.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
clip_wl_set_selection(Clipboard_T *cbd UNUSED)
{
}

#  ಯದಿ defined(USE_SYSTEM)
/*
 * Return true if we own the selection corresponding to cbd or another client
 * does.
 */
    ಸ್ಥಿರ bool
clip_wl_owner_exists(Clipboard_T *cbd)
{
    clip_wl_selection_T *sel = clip_wl_get_selection_from_cbd(cbd);

    ಯದಿ (vwl_connection_roundtrip(wayland_ct) == FAIL)
	ಪ್ರತ್ಯಾಗಮ false;

    ಪ್ರತ್ಯಾಗಮ sel->available && (sel->source != ರಿಕ್ತ || sel->offer != ರಿಕ್ತ);
}
#  endif

# endif // FEAT_WAYLAND_CLIPBOARD

#endif // FEAT_CLIPBOARD

#ifdef HAVE_CLIPMETHOD

/*
 * Returns the first method for accessing the clipboard that is available/works,
 * depending on the order of values in str.
 */
    ಸ್ಥಿರ clipmethod_T
get_clipmethod(char_u *str)
{
    ಪೂರ್ಣಾಙ್ಕ		len	= (ಪೂರ್ಣಾಙ್ಕ)STRLEN(str) + 1;
    char_u	*buf	= alloc(len);

    ಯದಿ (buf == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ CLIPMETHOD_FAIL;

    clipmethod_T ret = CLIPMETHOD_FAIL;
    char_u	*p = str;

    ಯಾವತ್ (*p != NUL)
    {
	clipmethod_T method = CLIPMETHOD_NONE;

	(ಶೂನ್ಯ)copy_option_part(&p, buf, len, ",");

	ಯದಿ (STRCMP(buf, "wayland") == 0)
	{
# ifdef FEAT_GUI
	    ಯದಿ (!gui.in_use)
# endif
	    {
# ifdef FEAT_WAYLAND_CLIPBOARD
		ಯದಿ (clip_wl.regular.available || clip_wl.primary.available)
		    method = CLIPMETHOD_WAYLAND;
# endif
	    }
	}
	ಅನ್ಯಥಾ ಯದಿ (STRCMP(buf, "x11") == 0)
	{
# ifdef FEAT_GUI
	    ಯದಿ (!gui.in_use)
# endif
	    {
# ifdef FEAT_XCLIPBOARD
		// x_IOerror_handler() in os_unix.c should set xterm_dpy to NULL if
		// we lost connection to the X server.
		ಯದಿ (xterm_dpy != ರಿಕ್ತ)
		{
		    // If the X connection is lost then that handler will longjmp
		    // somewhere else, in that case we will call choose_clipmethod()
		    // again from there, and this if block won't be executed since
		    // xterm_dpy will be set to NULL.
		    xterm_update();
		    method = CLIPMETHOD_X11;
		}
# endif
	    }
	}
	ಅನ್ಯಥಾ
	{
# ifdef FEAT_CLIPBOARD_PROVIDER
	    // Check if name matches a clipboard provider
	    ಪೂರ್ಣಾಙ್ಕ r = clip_provider_is_available(buf);

	    ಯದಿ (r == 1)
	    {
		method = CLIPMETHOD_PROVIDER;
		ಯದಿ (ret == CLIPMETHOD_FAIL)
		{
		    vim_free(clip_provider);
		    clip_provider = vim_strsave(buf);
		    ಯದಿ (clip_provider == ರಿಕ್ತ)
			ಗಚ್ಛ fail;
		}
	    }
	    ಅನ್ಯಥಾ ಯದಿ (r == -1)
# endif
	    {
# ifdef FEAT_CLIPBOARD_PROVIDER
fail:
# endif
		ret = CLIPMETHOD_FAIL;
		ಗಚ್ಛ exit;
	    }
	}

	// Keep on going in order to catch errors
	ಯದಿ (method != CLIPMETHOD_NONE && ret == CLIPMETHOD_FAIL)
	    ret = method;
    }

    // No match found, use "none".
    ret = (ret == CLIPMETHOD_FAIL) ? CLIPMETHOD_NONE : ret;

exit:
    vim_free(buf);
    ಪ್ರತ್ಯಾಗಮ ret;
}


/*
 * Returns name of clipmethod in a statically allocated string.
 */
    ಸ್ಥಿರ char_u *
clipmethod_to_str(clipmethod_T method)
{
    ವಿಕಲ್ಪ(method)
    {
	ಪಕ್ಷ CLIPMETHOD_WAYLAND:
	    ಪ್ರತ್ಯಾಗಮ (char_u *)"wayland";
	ಪಕ್ಷ CLIPMETHOD_X11:
	    ಪ್ರತ್ಯಾಗಮ (char_u *)"x11";
	ಪಕ್ಷ CLIPMETHOD_PROVIDER:
# ifdef FEAT_EVAL
	    ಪ್ರತ್ಯಾಗಮ clip_provider;
# endif
	ಸ್ವಭಾವ:
	    ಪ್ರತ್ಯಾಗಮ (char_u *)"none";
    }
}

/*
 * Sets the current clipmethod to use given by `get_clipmethod()`. Returns an
 * error message on failure else NULL.
 */
    ವರ್ಣ *
choose_clipmethod(ಶೂನ್ಯ)
{
    clipmethod_T method = get_clipmethod(p_cpm);

    ಯದಿ (method == CLIPMETHOD_FAIL)
	ಪ್ರತ್ಯಾಗಮ e_invalid_argument;

// If GUI is running or we are not on a system with Wayland or X11, then always
// return CLIPMETHOD_NONE. System or GUI clipboard handling always overrides.
// This is unless a provider is being used.
# ಯದಿ defined(FEAT_XCLIPBOARD) || defined(FEAT_WAYLAND_CLIPBOARD)
#  ಯದಿ defined(FEAT_GUI)
    ಯದಿ (method != CLIPMETHOD_PROVIDER && gui.in_use)
    {
#   ifdef FEAT_WAYLAND
	// We only interact with Wayland for the clipboard, we can just deinit
	// everything.
	wayland_uninit_connection();
#   endif

	method = CLIPMETHOD_NONE;
	ಗಚ್ಛ lose_sel_exit;
    }
#  endif
# ಅನ್ಯಥಾ
    // If on a system like windows or macos, then clipmethod is irrelevant, we
    // use their way of accessing the clipboard. This is unless we are using the
    // clipboard provider
#  ifdef FEAT_CLIPBOARD_PROVIDER
    ಯದಿ (method != CLIPMETHOD_PROVIDER)
#  endif
    {
	method = CLIPMETHOD_NONE;
	ಗಚ್ಛ exit;
    }
# endif

# ifdef FEAT_CLIPBOARD
    // Deinitialize clipboard if there is no way to access clipboard
    ಯದಿ (method == CLIPMETHOD_NONE)
	clip_init(FALSE);
    // If we have a clipmethod that works now, then initialize clipboard
    ಅನ್ಯಥಾ ಯದಿ (clipmethod == CLIPMETHOD_NONE && method != CLIPMETHOD_NONE)
    {
	clip_init(TRUE);
	did_warn_clipboard = false;
    }
    // Disown clipboard if we are switching to a new method
    ಅನ್ಯಥಾ ಯದಿ (clipmethod != CLIPMETHOD_NONE && method != clipmethod)
    {
#  ಯದಿ (defined(FEAT_XCLIPBOARD) || defined(FEAT_WAYLAND_CLIPBOARD)) \
	&& defined(FEAT_GUI)
lose_sel_exit:
#  endif
	ಯದಿ (clip_star.owned)
	    clip_lose_selection(&clip_star);
	ಯದಿ (clip_plus.owned)
	    clip_lose_selection(&clip_plus);

#  ಯದಿ defined(FEAT_GUI)
	ಯದಿ (!gui.in_use)
#  endif
	{
	    clip_init(TRUE);
	    did_warn_clipboard = false;
	}
    }
# endif // FEAT_CLIPBOARD

# ಯದಿ !defined(FEAT_XCLIPBOARD) && !defined(FEAT_WAYLAND_CLIPBOARD)
exit:
# endif

    clipmethod = method;

# ifdef FEAT_EVAL
    set_vim_var_string(VV_CLIPMETHOD, (char_u*)clipmethod_to_str(method), -1);
# endif

    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

/*
 * Call choose_clipmethod().
 */
    ಶೂನ್ಯ
ex_clipreset(exarg_T *eap UNUSED)
{
    clipmethod_T prev = clipmethod;

    choose_clipmethod();

    ಯದಿ (clipmethod == CLIPMETHOD_NONE)
	smsg(_("Could not find a way to access the clipboard."));
    ಅನ್ಯಥಾ ಯದಿ (clipmethod != prev)
	smsg(_("Switched to clipboard method '%s'."),
		clipmethod_to_str(clipmethod));
}

/*
 * Adjust the register name pointed to with "rp" for the clipboard being
 * used always and the clipboard being available.
 */
    ಶೂನ್ಯ
adjust_clip_reg(ಪೂರ್ಣಾಙ್ಕ *rp)
{
# ifdef FEAT_CLIPBOARD_PROVIDER
    ಯದಿ (clipmethod == CLIPMETHOD_PROVIDER)
    {
	ಯದಿ (*rp == 0 && clip_unnamed != 0)
	    *rp = ((clip_unnamed & CLIP_UNNAMED_PLUS)) ? '+' : '*';
	ಪ್ರತ್ಯಾಗಮ;
    }
# endif
# ifdef FEAT_CLIPBOARD
    // If no reg. specified, and "unnamed" or "unnamedplus" is in 'clipboard',
    // use '*' or '+' reg, respectively. "unnamedplus" prevails.
    ಯದಿ (*rp == 0 && (clip_unnamed != 0 || clip_unnamed_saved != 0))
    {
	ಯದಿ (clip_unnamed != 0)
	    *rp = ((clip_unnamed & CLIP_UNNAMED_PLUS) && clip_plus.available)
								  ? '+' : '*';
	ಅನ್ಯಥಾ
	    *rp = ((clip_unnamed_saved & CLIP_UNNAMED_PLUS)
					   && clip_plus.available) ? '+' : '*';
    }
    ಯದಿ ((!clip_star.available && *rp == '*') ||
	   (!clip_plus.available && *rp == '+'))
    {
	ಸಂದೇಶ_warn_missing_clipboard();
	*rp = 0;
    }
# endif
}

/*
 * Extract the items in the 'clipboard' option and set global values.
 * Return an error message or NULL for success.
 */
    ವರ್ಣ *
did_set_clipboard(optset_T *args UNUSED)
{
    ಪೂರ್ಣಾಙ್ಕ		new_unnamed = 0;
# ifdef FEAT_CLIPBOARD
    ಪೂರ್ಣಾಙ್ಕ		new_autoselect_star = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		new_autoselect_plus = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		new_autoselectml = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		new_html = FALSE;
# endif
    regprog_T	*new_exclude_prog = ರಿಕ್ತ;
    ವರ್ಣ	*errmsg = ರಿಕ್ತ;
    char_u	*p;

    ಪ್ರತಿ (p = p_cb; *p != NUL; )
    {
	// Note: Keep this in sync with p_cb_values.
	ಯದಿ (STRNCMP(p, "unnamed", 7) == 0 && (p[7] == ',' || p[7] == NUL))
	{
	    new_unnamed |= CLIP_UNNAMED;
	    p += 7;
	}
	ಅನ್ಯಥಾ ಯದಿ (STRNCMP(p, "unnamedplus", 11) == 0
					    && (p[11] == ',' || p[11] == NUL))
	{
	    new_unnamed |= CLIP_UNNAMED_PLUS;
	    p += 11;
	}
# ifdef FEAT_CLIPBOARD
	ಅನ್ಯಥಾ ಯದಿ (STRNCMP(p, "autoselect", 10) == 0
					    && (p[10] == ',' || p[10] == NUL))
	{
	    new_autoselect_star = TRUE;
	    p += 10;
	}
	ಅನ್ಯಥಾ ಯದಿ (STRNCMP(p, "autoselectplus", 14) == 0
					    && (p[14] == ',' || p[14] == NUL))
	{
	    new_autoselect_plus = TRUE;
	    p += 14;
	}
	ಅನ್ಯಥಾ ಯದಿ (STRNCMP(p, "autoselectml", 12) == 0
					    && (p[12] == ',' || p[12] == NUL))
	{
	    new_autoselectml = TRUE;
	    p += 12;
	}
	ಅನ್ಯಥಾ ಯದಿ (STRNCMP(p, "html", 4) == 0 && (p[4] == ',' || p[4] == NUL))
	{
	    new_html = TRUE;
	    p += 4;
	}
	ಅನ್ಯಥಾ ಯದಿ (STRNCMP(p, "exclude:", 8) == 0 && new_exclude_prog == ರಿಕ್ತ)
	{
	    p += 8;
	    new_exclude_prog = vim_regcomp(p, RE_MAGIC);
	    ಯದಿ (new_exclude_prog == ರಿಕ್ತ)
		errmsg = e_invalid_argument;
	    ಭಙ್ಗ;
	}
# endif
	ಅನ್ಯಥಾ
	{
	    errmsg = e_invalid_argument;
	    ಭಙ್ಗ;
	}
	ಯದಿ (*p == ',')
	    ++p;
    }
    ಯದಿ (errmsg == ರಿಕ್ತ)
    {
# ifdef FEAT_CLIPBOARD
	ಯದಿ (global_busy)
	    // clip_unnamed will be reset to clip_unnamed_saved
	    // at end_global_changes
	    clip_unnamed_saved = new_unnamed;
	ಅನ್ಯಥಾ
# endif
	    clip_unnamed = new_unnamed;
# ifdef FEAT_CLIPBOARD
	clip_autoselect_star = new_autoselect_star;
	clip_autoselect_plus = new_autoselect_plus;
	clip_autoselectml = new_autoselectml;
	clip_html = new_html;
	vim_regfree(clip_exclude_prog);
	clip_exclude_prog = new_exclude_prog;
# endif
# ಯದಿ defined(FEAT_GUI_GTK)
	ಯದಿ (gui.in_use)
	{
#  ifdef USE_GTK4
	    gui_gtk_update_selection_formats(&clip_plus);
	    gui_gtk_update_selection_formats(&clip_star);
#  ಅನ್ಯಥಾ
	    gui_gtk_set_selection_targets((GdkAtom)GDK_SELECTION_PRIMARY);
	    gui_gtk_set_selection_targets((GdkAtom)clip_plus.gtk_sel_atom);
#  endif
#  ifdef FEAT_DND
	    gui_gtk_set_dnd_targets();
#  endif
	}
# endif
    }
    ಅನ್ಯಥಾ
	vim_regfree(new_exclude_prog);

    ಪ್ರತ್ಯಾಗಮ errmsg;
}

#endif // HAVE_CLIPMETHOD

#ifdef FEAT_CLIPBOARD_PROVIDER

/*
 * Check if a clipboard provider with given name is available. Returns 1 if available,
 * 0 if not available, and -1 on error
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
clip_provider_is_available(char_u *provider)
{
    dict_T	*providers = get_vim_var_dict(VV_CLIPPROVIDERS);
    typval_T	provider_tv = {0};
    callback_T	callback = {0};
    typval_T	rettv = {0};
    typval_T	func_tv = {0};
    ಪೂರ್ಣಾಙ್ಕ		res = 0;

    ಯದಿ (dict_get_tv(providers, (ವರ್ಣ *)provider, &provider_tv) == FAIL
	    || provider_tv.v_type != VAR_DICT)
	// clipboard provider not defined
	ಪ್ರತ್ಯಾಗಮ -1;

    ಯದಿ (dict_get_tv(provider_tv.vval.v_dict, "available", &func_tv) == FAIL)
    {
	clear_tv(&provider_tv);
	// If "available" function not specified assume always TRUE
	ಪ್ರತ್ಯಾಗಮ 1;
    }

    ಯದಿ ((callback = get_callback(&func_tv)).cb_name == ರಿಕ್ತ)
	ಗಚ್ಛ fail;

    ಯದಿ (call_callback(&callback, -1, &rettv, 0, ರಿಕ್ತ) == FAIL ||
	    (rettv.v_type != VAR_BOOL && rettv.v_type != VAR_ಸಂಖ್ಯೆ))
	ಗಚ್ಛ fail;

    ಯದಿ (rettv.vval.v_ಸಂಖ್ಯೆ)
	res = 1;

    ಯದಿ (FALSE)
fail:
	res = -1;

    free_callback(&callback);
    clear_tv(&func_tv);
    clear_tv(&rettv);
    clear_tv(&provider_tv);

    ಪ್ರತ್ಯಾಗಮ res;
}

/*
 * Get the specified callback "function" from the provider dictionary for
 * register "reg".
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
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

    ಯದಿ (dict_get_tv(providers, (ವರ್ಣ *)provider, &provider_tv) == FAIL)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    ಅನ್ಯಥಾ ಯದಿ (provider_tv.v_type != VAR_DICT)
    {
	clear_tv(&provider_tv);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ಅನ್ಯಥಾ ಯದಿ (dict_get_tv(
		provider_tv.vval.v_dict,
		(ವರ್ಣ *)function,
		&action_tv) == FAIL)
    {
	clear_tv(&provider_tv);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ಅನ್ಯಥಾ ಯದಿ (action_tv.v_type != VAR_DICT)
    {
	clear_tv(&provider_tv);
	clear_tv(&action_tv);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ಅನ್ಯಥಾ ಯದಿ (dict_get_tv(action_tv.vval.v_dict, (ವರ್ಣ *)reg, &func_tv) == FAIL)
    {
	clear_tv(&provider_tv);
	clear_tv(&action_tv);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ಅನ್ಯಥಾ ಯದಿ ((cb = get_callback(&func_tv)).cb_name == ರಿಕ್ತ)
    {
	clear_tv(&provider_tv);
	clear_tv(&action_tv);
	clear_tv(&func_tv);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    clear_tv(&provider_tv);
    clear_tv(&action_tv);

    // func_tv owns the function name, so we must make a copy for the callback
    set_callback(callback, &cb);
    clear_tv(&func_tv);
    ಪ್ರತ್ಯಾಗಮ OK;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
clip_provider_copy(char_u *reg, char_u *provider)
{
    ಸ್ಥಿರ bool	recursive = false;
    callback_T	callback;
    typval_T	rettv;
    typval_T	argvars[4];
    yankreg_T	*y_ptr;
    char_u	type[2 + NUMBUFLEN] = {0};
    list_T	*list = ರಿಕ್ತ;

    ಯದಿ (recursive)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (clip_provider_get_callback(
		reg,
		provider,
		(char_u *)"copy",
		&callback) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    // Convert register type into a string
    ಯದಿ (*reg == '+')
	y_ptr = get_y_register(REAL_PLUS_REGISTER);
    ಅನ್ಯಥಾ
	y_ptr = get_y_register(STAR_REGISTER);

    ವಿಕಲ್ಪ (y_ptr->y_type)
    {
	ಪಕ್ಷ MCHAR:
	    type[0] = 'v';
	    ಭಙ್ಗ;
	ಪಕ್ಷ MLINE:
	    type[0] = 'V';
	    ಭಙ್ಗ;
	ಪಕ್ಷ MBLOCK:
	    sprintf((ವರ್ಣ *)type, "%c%d", Ctrl_V, y_ptr->y_width + 1);
	    ಭಙ್ಗ;
	ಸ್ವಭಾವ:
	    type[0] = 0;
	    ಭಙ್ಗ;
    }

    argvars[0].v_type = VAR_STRING;
    argvars[0].vval.v_string = reg;

    argvars[1].v_type = VAR_STRING;
    argvars[1].vval.v_string = type;

    // Get register contents by creating a list of lines
    list = list_alloc();

    ಯದಿ (list == ರಿಕ್ತ)
    {
	free_callback(&callback);
	ಪ್ರತ್ಯಾಗಮ;
    }

    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < y_ptr->y_size; i++)
	ಯದಿ (list_append_string(list, y_ptr->y_array[i].string,
	    (ಪೂರ್ಣಾಙ್ಕ)y_ptr->y_array[i].length) == FAIL)
	{
	    free_callback(&callback);
	    list_unref(list);
	    ಪ್ರತ್ಯಾಗಮ;
	}

    list->lv_refcount++;

    argvars[2].v_type = VAR_LIST;
    argvars[2].v_lock = VAR_FIXED;
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

    ಸ್ಥಿರ ಶೂನ್ಯ
clip_provider_paste(char_u *reg, char_u *provider)
{
    ಸ್ಥಿರ bool	recursive = false;
    callback_T	callback;
    typval_T	argvars[2];
    typval_T	rettv;
    ಪೂರ್ಣಾಙ್ಕ		ret;
    char_u	*reg_type;
    list_T	*ಸಾಲುಗಳು;

    ಯದಿ (recursive)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (clip_provider_get_callback(
		reg,
		provider,
		(char_u *)"paste",
		&callback) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    argvars[0].v_type = VAR_STRING;
    argvars[0].vval.v_string = reg;

    argvars[1].v_type = VAR_UNKNOWN;

    textlock++;
    recursive = true;
    ret = call_callback(&callback, -1, &rettv, 1, argvars);
    recursive = false;
    textlock--;

    ಯದಿ (ret == FAIL)
	ಗಚ್ಛ exit;
    ಅನ್ಯಥಾ ಯದಿ (rettv.v_type == VAR_TUPLE
	    && TUPLE_LEN(rettv.vval.v_tuple) == 2
	    && TUPLE_ITEM(rettv.vval.v_tuple, 0)->v_type == VAR_STRING
	    && TUPLE_ITEM(rettv.vval.v_tuple, 1)->v_type == VAR_LIST)
    {
	reg_type = TUPLE_ITEM(rettv.vval.v_tuple, 0)->vval.v_string;
	ಸಾಲುಗಳು = TUPLE_ITEM(rettv.vval.v_tuple, 1)->vval.v_list;
    }
    ಅನ್ಯಥಾ ಯದಿ (rettv.v_type == VAR_LIST
	    && rettv.vval.v_list->lv_len == 2
	    && rettv.vval.v_list->lv_first->li_tv.v_type == VAR_STRING
	    && rettv.vval.v_list->lv_first->li_next->li_tv.v_type == VAR_LIST)
    {
	reg_type = rettv.vval.v_list->lv_first->li_tv.vval.v_string;
	ಸಾಲುಗಳು = rettv.vval.v_list->lv_first->li_next->li_tv.vval.v_list;
    }
    ಅನ್ಯಥಾ
	ಗಚ್ಛ exit;

    {
	char_u		yank_type = MAUTO;
	long		block_len = -1;
	yankreg_T	*y_ptr, *cur_y_ptr;
	char_u		**lstval;
	char_u		**allocval;
	char_u		buf[NUMBUFLEN];
	char_u		**curval;
	char_u		**curallocval;
	char_u		*strval;
	listitem_T	*li;
	ಪೂರ್ಣಾಙ್ಕ		len;

	// If the list is NULL handle like an empty list.
	len = ಸಾಲುಗಳು == ರಿಕ್ತ ? 0 : ಸಾಲುಗಳು->lv_len;

	// First half: use for pointers to result lines; second half: use for
	// pointers to allocated copies.
	lstval = ALLOC_MULT(char_u *, (len + 1) * 2);
	ಯದಿ (lstval == ರಿಕ್ತ)
	    ಗಚ್ಛ exit;
	curval = lstval;
	allocval = lstval + len + 2;
	curallocval = allocval;

	ಯದಿ (ಸಾಲುಗಳು != ರಿಕ್ತ)
	{
	    CHECK_LIST_MATERIALIZE(ಸಾಲುಗಳು);
	    FOR_ALL_LIST_ವಸ್ತುಗಳು(ಸಾಲುಗಳು, li)
	    {
		strval = tv_get_string_buf_chk(&li->li_tv, buf);
		ಯದಿ (strval == ರಿಕ್ತ)
		    ಗಚ್ಛ free_lstval;
		ಯದಿ (strval == buf)
		{
		    // Need to make a copy, next tv_get_string_buf_chk() will
		    // overwrite the string.
		    strval = vim_strsave(buf);
		    ಯದಿ (strval == ರಿಕ್ತ)
			ಗಚ್ಛ free_lstval;
		    *curallocval++ = strval;
		}
		*curval++ = strval;
	    }
	}
	*curval++ = ರಿಕ್ತ;

	ಯದಿ (*reg_type != NUL && (STRLEN(reg_type) == 0
		|| get_yank_type(&reg_type, &yank_type, &block_len) == FAIL))
	{
	    emsg(e_invalid_argument);
	    ಗಚ್ಛ free_lstval;
	}

	ಯದಿ (*reg == '+')
	    y_ptr = get_y_register(REAL_PLUS_REGISTER);
	ಅನ್ಯಥಾ
	    y_ptr = get_y_register(STAR_REGISTER);

	// Free previous register contents
	cur_y_ptr = get_y_current();
	set_y_current(y_ptr);

	free_yank_all();
	get_y_current()->y_size = 0;

	set_y_current(cur_y_ptr);

	str_to_reg(y_ptr,
		yank_type,
		(char_u *)lstval,
		-1,
		block_len,
		TRUE);

free_lstval:
	ಯಾವತ್ (curallocval > allocval)
	    vim_free(*--curallocval);
	vim_free(lstval);
    }

exit:
    free_callback(&callback);
    clear_tv(&rettv);
}

// Used to stop calling the provider callback every time there is an update.
// This prevents unnecessary calls when accessing the provider often in an
// interval.
//
// If -1 then allow provider callback to be called then set to one. Default
// value (is allowed) is -2.
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ star_pause_ಎಣಿಕೆ = -2, plus_pause_ಎಣಿಕೆ = -2;

    ಶೂನ್ಯ
call_clip_provider_request(ಪೂರ್ಣಾಙ್ಕ reg)
{
    ಯದಿ (clipmethod != CLIPMETHOD_PROVIDER)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (reg == '+' && plus_pause_ಎಣಿಕೆ < 0)
    {
	ಯದಿ (plus_pause_ಎಣಿಕೆ == -1)
	    plus_pause_ಎಣಿಕೆ = 1;
	clip_provider_paste((char_u *)"+", clip_provider);
    }
    ಅನ್ಯಥಾ ಯದಿ (reg == '*' && star_pause_ಎಣಿಕೆ < 0)
    {
	ಯದಿ (star_pause_ಎಣಿಕೆ == -1)
	    star_pause_ಎಣಿಕೆ = 1;
	clip_provider_paste((char_u *)"*", clip_provider);
    }
    ಅನ್ಯಥಾ
	ಪ್ರತ್ಯಾಗಮ;
}

    ಶೂನ್ಯ
call_clip_provider_set(ಪೂರ್ಣಾಙ್ಕ reg)
{
    ಯದಿ (clipmethod != CLIPMETHOD_PROVIDER)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (reg == '+' && plus_pause_ಎಣಿಕೆ < 0)
    {
	ಯದಿ (plus_pause_ಎಣಿಕೆ == -1)
	    plus_pause_ಎಣಿಕೆ = 1;
	clip_provider_copy((char_u *)"+", clip_provider);
    }
    ಅನ್ಯಥಾ ಯದಿ (reg == '*' && star_pause_ಎಣಿಕೆ < 0)
    {
	ಯದಿ (star_pause_ಎಣಿಕೆ == -1)
	    star_pause_ಎಣಿಕೆ = 1;
	clip_provider_copy((char_u *)"*", clip_provider);
    }
}

/*
 * Makes it so that the next provider call is only done once any calls after are
 * ignored, until dec_clip_provider is called the same number of times after
 * again. Note that this is per clipboard register ("+", "*")
 */
    ಶೂನ್ಯ
inc_clip_provider(ಶೂನ್ಯ)
{
    plus_pause_ಎಣಿಕೆ = (plus_pause_ಎಣಿಕೆ == -2
	|| plus_pause_ಎಣಿಕೆ == -1) ? -1 : plus_pause_ಎಣಿಕೆ + 1;
    star_pause_ಎಣಿಕೆ = (star_pause_ಎಣಿಕೆ == -2
	|| star_pause_ಎಣಿಕೆ == -1) ? -1 : star_pause_ಎಣಿಕೆ + 1;
}

    ಶೂನ್ಯ
dec_clip_provider(ಶೂನ್ಯ)
{
    ಯದಿ (plus_pause_ಎಣಿಕೆ != -2)
	plus_pause_ಎಣಿಕೆ = plus_pause_ಎಣಿಕೆ == -1 ? -1 : plus_pause_ಎಣಿಕೆ - 1;
    ಯದಿ (star_pause_ಎಣಿಕೆ != -2)
	star_pause_ಎಣಿಕೆ = star_pause_ಎಣಿಕೆ == -1 ? -1 : star_pause_ಎಣಿಕೆ - 1;

    ಯದಿ (plus_pause_ಎಣಿಕೆ == 0 || plus_pause_ಎಣಿಕೆ == -1)
	plus_pause_ಎಣಿಕೆ = -2;
    ಯದಿ (star_pause_ಎಣಿಕೆ == 0 || star_pause_ಎಣಿಕೆ == -1)
	star_pause_ಎಣಿಕೆ = -2;
}

#endif // FEAT_CLIPBOARD_PROVIDER

#ಯದಿ defined(FEAT_WAYLAND_CLIPBOARD) || (defined(FEAT_GUI_GTK) && defined(USE_GTK4))
/*
 * If "vim" is TRUE, then get the motion type. If "vimenc" is TRUE, then get the
 * motion type and also convert "*buf". "buf" and "len_store" will be updated to
 * reflect the actual contents, but should be set beforehand with the initial
 * contents. Note that if "*tofree" is not NULL, use vim_free() on "*buf",
 * otherwise use the free func for which "*buf" was allocated with. In both
 * cases use vim_free() on "*tofree". Returns OK on success and FAIL on failure.
 */
    ಪೂರ್ಣಾಙ್ಕ
clip_convert_data(
	char_u	**buf,
	long	*len_store,
	ಪೂರ್ಣಾಙ್ಕ	*motion,
	bool	vim,
	bool	vimenc,
	char_u	**tofree)
{
    char_u  *final = *buf;
    char_u  *enc;
    long    len = *len_store;

    ಯದಿ (vim && len >= 2)
    {
	*motion = *final++;
	len--;
    }
    ಅನ್ಯಥಾ ಯದಿ (vimenc && len >= 3)
    {
	vimconv_T   conv;
	ಪೂರ್ಣಾಙ್ಕ	    convlen;

	// First byte is motion type
	*motion = *final++;
	len--;

	// Get encoding of selection
	enc = final;

	// Skip the encoding type including null terminator in final text
	final = memchr(final, NUL, len);
	ಯದಿ (final == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	final++; // Skip NUL

	// Subtract pointers to get length of encoding;
	len -= final - enc;

	conv.vc_type = CONV_NONE;
	convert_setup(&conv, enc, p_enc);
	ಯದಿ (conv.vc_type != CONV_NONE)
	{
	   char_u *tmp;

	   convlen = len;
	   tmp = string_convert(&conv, final, &convlen);
	   len = convlen;
	   ಯದಿ (tmp != ರಿಕ್ತ)
	   {
		final = tmp;
		*tofree = final;
	   }
	   convert_setup(&conv, ರಿಕ್ತ, ರಿಕ್ತ);
	}
    }
    *buf = final;
    *len_store = len;
    ಪ್ರತ್ಯಾಗಮ OK;
}
#endif
