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

#ଅନ୍ତର୍ଭାଵ "vim.h"

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
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା clip_provider_is_available(char_u *provider);
#endif

#ଯଦି defined(FEAT_CLIPBOARD)

# ଯଦି defined(FEAT_WAYLAND_CLIPBOARD)

#  include "wayland.h"

// Represents either the regular or primary selection
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା {
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
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା {
    vwl_seat_T *seat;

    clip_wl_selection_T regular;
    clip_wl_selection_T primary;
} clip_wl_T;

// Mime types we support sending and receiving
// Mimes with a lower index in the array are prioritized first when we are
// receiving data.
ସ୍ଥିର ଅଚଳ ଅକ୍ଷର *supported_mimes[] = {
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

ସ୍ଥିର ଶୂନ୍ୟ
clip_wl_receive_data(Clipboard_T *cbd, ଅଚଳ ଅକ୍ଷର *mime_type, ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd);
ସ୍ଥିର ଶୂନ୍ୟ clip_wl_request_selection(Clipboard_T *cbd);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା clip_wl_own_selection(Clipboard_T *cbd);
ସ୍ଥିର ଶୂନ୍ୟ clip_wl_lose_selection(Clipboard_T *cbd);
ସ୍ଥିର ଶୂନ୍ୟ clip_wl_set_selection(Clipboard_T *cbd);

#  ଯଦି defined(USE_SYSTEM)
ସ୍ଥିର bool clip_wl_owner_exists(Clipboard_T *cbd);
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
    ଶୂନ୍ୟ
clip_init(ପୂର୍ଣ୍ଣସଂଖ୍ୟା can_use)
{
    Clipboard_T *cb;

    cb = &clip_star;
    ପାଇଁ (;;)
    {
	// No need to init again if cbd is already available
	ଯଦି (can_use && cb->available)
	    ଯାଅ skip;

	cb->available  = can_use;
	cb->owned      = FALSE;
	cb->start.lnum = 0;
	cb->start.col  = 0;
	cb->end.lnum   = 0;
	cb->end.col    = 0;
	cb->state      = SELECT_CLEARED;

skip:
	ଯଦି (cb == &clip_plus)
	    ଭାଙ୍ଗ;
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
    ଶୂନ୍ୟ
clip_update_selection(Clipboard_T *clip)
{
    pos_T	    start, end;

    // If visual mode is only due to a redo command ("."), then ignore it
    ଯଦି (!redo_VIsual_busy && VIsual_active && (State & MODE_NORMAL))
    {
	ଯଦି (LT_POS(VIsual, curwin->w_cursor))
	{
	    start = VIsual;
	    end = curwin->w_cursor;
	    ଯଦି (has_mbyte)
		end.col += (*mb_ptr2len)(ml_get_cursor()) - 1;
	}
	ଅନ୍ୟଥା
	{
	    start = curwin->w_cursor;
	    end = VIsual;
	}
	ଯଦି (!EQUAL_POS(clip->start, start)
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

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
clip_gen_own_selection(Clipboard_T *cbd)
{
# ଯଦି defined(FEAT_XCLIPBOARD) || defined(FEAT_WAYLAND_CLIPBOARD)
#  ifdef FEAT_GUI
    ଯଦି (gui.in_use)
	ଫେରନ୍ତୁ clip_mch_own_selection(cbd);
    ଅନ୍ୟଥା
#  endif
    {
	ଯଦି (clipmethod == CLIPMETHOD_WAYLAND)
	{
#  ifdef FEAT_WAYLAND_CLIPBOARD
	    ଫେରନ୍ତୁ clip_wl_own_selection(cbd);
#  endif
	}
	ଅନ୍ୟଥା ଯଦି (clipmethod == CLIPMETHOD_X11)
	{
#  ifdef FEAT_XCLIPBOARD
	    ଫେରନ୍ତୁ clip_xterm_own_selection(cbd);
#  endif
	}
    }
    ଫେରନ୍ତୁ FAIL;
# ଅନ୍ୟଥା
    ଫେରନ୍ତୁ clip_mch_own_selection(cbd);
# endif
}

    ଶୂନ୍ୟ
clip_own_selection(Clipboard_T *cbd)
{
    /*
     * Also want to check somehow that we are reading from the keyboard rather
     * than a mapping etc.
     */
# ଯଦି defined(FEAT_X11) || defined(FEAT_WAYLAND_CLIPBOARD)
    // Always own the selection, we might have lost it without being
    // notified, e.g. during a ":sh" command.
    ଯଦି (cbd->available)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା was_owned = cbd->owned;

	cbd->owned = (clip_gen_own_selection(cbd) == OK);
	ଯଦି (!was_owned && (cbd == &clip_star || cbd == &clip_plus))
	{
	    // May have to show a different kind of highlighting for the
	    // selected area.  There is no specific redraw command for this,
	    // just redraw all windows on the current buffer.
	    ଯଦି (cbd->owned
		    && (get_real_state() == MODE_VISUAL
					    || get_real_state() == MODE_SELECT)
		    && (cbd == &clip_star ? clip_isautosel_star()
						      : clip_isautosel_plus())
		    && HL_ATTR(HLF_V) != HL_ATTR(HLF_VNC))
		redraw_curbuf_later(UPD_INVERTED_ALL);
	}
    }
# ଅନ୍ୟଥା
    // Only own the clipboard when we didn't own it yet.
    ଯଦି (!cbd->owned && cbd->available)
	cbd->owned = (clip_gen_own_selection(cbd) == OK);
# endif
}

    ସ୍ଥିର ଶୂନ୍ୟ
clip_gen_lose_selection(Clipboard_T *cbd)
{
# ଯଦି defined(FEAT_XCLIPBOARD) || defined(FEAT_WAYLAND_CLIPBOARD)
#  ifdef FEAT_GUI
    ଯଦି (gui.in_use)
	clip_mch_lose_selection(cbd);
    ଅନ୍ୟଥା
#  endif
    {
	ଯଦି (clipmethod == CLIPMETHOD_WAYLAND)
	{
#  ifdef FEAT_WAYLAND_CLIPBOARD
	    clip_wl_lose_selection(cbd);
#  endif
	}
	ଅନ୍ୟଥା ଯଦି (clipmethod == CLIPMETHOD_X11)
	{
#  ifdef FEAT_XCLIPBOARD
	    clip_xterm_lose_selection(cbd);
#  endif
	}
    }
# ଅନ୍ୟଥା
    clip_mch_lose_selection(cbd);
# endif
}

    ଶୂନ୍ୟ
clip_lose_selection(Clipboard_T *cbd)
{
# ifdef FEAT_X11
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    was_owned = cbd->owned;
# endif
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା     visual_selection = FALSE;

    ଯଦି (cbd == &clip_star || cbd == &clip_plus)
	visual_selection = TRUE;

    clip_free_selection(cbd);
    cbd->owned = FALSE;
    ଯଦି (visual_selection)
	clip_clear_selection(cbd);
    clip_gen_lose_selection(cbd);
# ifdef FEAT_X11
    ଯଦି (visual_selection)
    {
	// May have to show a different kind of highlighting for the selected
	// area.  There is no specific redraw command for this, just redraw all
	// windows on the current buffer.
	ଯଦି (was_owned
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

    ସ୍ଥିର ଶୂନ୍ୟ
clip_copy_selection(Clipboard_T *clip)
{
    ଯଦି (VIsual_active && (State & MODE_NORMAL) && clip->available)
    {
	clip_update_selection(clip);
	clip_free_selection(clip);
	clip_own_selection(clip);
	ଯଦି (clip->owned)
	    clip_get_selection(clip);
	clip_gen_set_selection(clip);
    }
}

/*
 * Save and restore clip_unnamed before doing possibly many changes. This
 * prevents accessing the clipboard very often which might slow down Vim
 * considerably.
 */
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା global_change_ଗଣନା = 0; // if set, inside a start_global_changes
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା clipboard_needs_update = FALSE; // clipboard needs to be updated
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା clip_did_set_selection = TRUE;

/*
 * Save clip_unnamed and reset it.
 */
    ଶୂନ୍ୟ
start_global_changes(ଶୂନ୍ୟ)
{
    ଯଦି (++global_change_ଗଣନା > 1)
	ଫେରନ୍ତୁ;
    clip_unnamed_saved = clip_unnamed;
    clipboard_needs_update = FALSE;

    ଯଦି (clip_did_set_selection)
    {
	clip_unnamed = 0;
	clip_did_set_selection = FALSE;
    }
}

/*
 * Return TRUE if setting the clipboard was postponed, it already contains the
 * right text.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
is_clipboard_needs_update(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ clipboard_needs_update;
}

/*
 * Restore clip_unnamed and set the selection when needed.
 */
    ଶୂନ୍ୟ
end_global_changes(ଶୂନ୍ୟ)
{
    ଯଦି (--global_change_ଗଣନା > 0)
	// recursive
	ଫେରନ୍ତୁ;
    ଯଦି (!clip_did_set_selection)
    {
	clip_did_set_selection = TRUE;
	clip_unnamed = clip_unnamed_saved;
	clip_unnamed_saved = 0;
	ଯଦି (clipboard_needs_update)
	{
	    // only store something in the clipboard,
	    // if we have yanked anything to it
	    ଯଦି (clip_unnamed & CLIP_UNNAMED)
	    {
		clip_own_selection(&clip_star);
		clip_gen_set_selection(&clip_star);
	    }
	    ଯଦି (clip_unnamed & CLIP_UNNAMED_PLUS)
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
    ଶୂନ୍ୟ
clip_auto_select(ଶୂନ୍ୟ)
{
    ଯଦି (clip_isautosel_star())
	clip_copy_selection(&clip_star);
    ଯଦି (clip_isautosel_plus())
	clip_copy_selection(&clip_plus);
}

/*
 * Return TRUE if automatic selection of Visual area is desired for the *
 * register.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
clip_isautosel_star(ଶୂନ୍ୟ)
{
# ifdef FEAT_CLIPBOARD_PROVIDER
    ଯଦି (clipmethod == CLIPMETHOD_PROVIDER)
	ଫେରନ୍ତୁ false;
# endif
# ifdef FEAT_GUI
    ଯଦି (gui.in_use)
	ଫେରନ୍ତୁ vim_strchr(p_go, GO_ASEL) != ଖାଲି
	    && vim_strchr(p_go, GO_ASELPLUS) == ଖାଲି;
# endif
    ଫେରନ୍ତୁ clip_autoselect_star;
}

/*
 * Return TRUE if automatic selection of Visual area is desired for the +
 * register.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
clip_isautosel_plus(ଶୂନ୍ୟ)
{
# ifdef FEAT_CLIPBOARD_PROVIDER
    ଯଦି (clipmethod == CLIPMETHOD_PROVIDER)
	ଫେରନ୍ତୁ false;
# endif
# ifdef FEAT_GUI
    ଯଦି (gui.in_use)
	ଫେରନ୍ତୁ vim_strchr(p_go, GO_ASELPLUS) != ଖାଲି;
# endif
    ଫେରନ୍ତୁ clip_autoselect_plus;
}


/*
 * Stuff for general mouse selection, without using Visual mode.
 */

/*
 * Compare two screen positions ala strcmp()
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
clip_compare_pos(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		row1,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		col1,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		row2,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		col2)
{
    ଯଦି (row1 > row2) ଫେରନ୍ତୁ(1);
    ଯଦି (row1 < row2) ଫେରନ୍ତୁ(-1);
    ଯଦି (col1 > col2) ଫେରନ୍ତୁ(1);
    ଯଦି (col1 < col2) ଫେରନ୍ତୁ(-1);
    ଫେରନ୍ତୁ(0);
}

// "how" flags for clip_invert_area()
# define CLIP_CLEAR	1
# define CLIP_SET	2
# define CLIP_TOGGLE	3

/*
 * Invert or un-invert a rectangle of the screen.
 * "invert" is true if the result is inverted.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
clip_invert_rectangle(
	Clipboard_T	*cbd UNUSED,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		row_arg,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		col_arg,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		height_arg,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		width_arg,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		invert)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		row = row_arg;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		col = col_arg;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		height = height_arg;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		width = width_arg;

# ifdef FEAT_PROP_POPUP
    // this goes on top of all popup windows
    screen_zindex = CLIP_ZINDEX;

    ଯଦି (col < cbd->min_col)
    {
	width -= cbd->min_col - col;
	col = cbd->min_col;
    }
    ଯଦି (width > cbd->max_col - col)
	width = cbd->max_col - col;
    ଯଦି (row < cbd->min_row)
    {
	height -= cbd->min_row - row;
	row = cbd->min_row;
    }
    ଯଦି (height > cbd->max_row - row + 1)
	height = cbd->max_row - row + 1;
# endif
# ifdef FEAT_GUI
    ଯଦି (gui.in_use)
	gui_mch_invert_rectangle(row, col, height, width);
    ଅନ୍ୟଥା
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
    ସ୍ଥିର ଶୂନ୍ୟ
clip_invert_area(
	Clipboard_T	*cbd,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		row1,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		col1,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		row2,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		col2,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		how)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		invert = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		max_col;

# ifdef FEAT_PROP_POPUP
    max_col = cbd->max_col - 1;
# ଅନ୍ୟଥା
    max_col = Columns - 1;
# endif

    ଯଦି (how == CLIP_SET)
	invert = TRUE;

    // Swap the from and to positions so the from is always before
    ଯଦି (clip_compare_pos(row1, col1, row2, col2) > 0)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା tmp_row, tmp_col;

	tmp_row = row1;
	tmp_col = col1;
	row1	= row2;
	col1	= col2;
	row2	= tmp_row;
	col2	= tmp_col;
    }
    ଅନ୍ୟଥା ଯଦି (how == CLIP_TOGGLE)
	invert = TRUE;

    // If all on the same line, do it the easy way
    ଯଦି (row1 == row2)
    {
	clip_invert_rectangle(cbd, row1, col1, 1, col2 - col1, invert);
    }
    ଅନ୍ୟଥା
    {
	// Handle a piece of the first line
	ଯଦି (col1 > 0)
	{
	    clip_invert_rectangle(cbd, row1, col1, 1,
						  (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)Columns - col1, invert);
	    row1++;
	}

	// Handle a piece of the last line
	ଯଦି (col2 < max_col)
	{
	    clip_invert_rectangle(cbd, row2, 0, 1, col2, invert);
	    row2--;
	}

	// Handle the rectangle that's left
	ଯଦି (row2 >= row1)
	    clip_invert_rectangle(cbd, row1, 0, row2 - row1 + 1,
							 (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)Columns, invert);
    }
}

/*
 * Start, continue or end a modeless selection.  Used when editing the
 * command-line, in the cmdline window and when the mouse is in a popup window.
 */
    ଶୂନ୍ୟ
clip_modeless(ପୂର୍ଣ୍ଣସଂଖ୍ୟା button, ପୂର୍ଣ୍ଣସଂଖ୍ୟା is_click, ପୂର୍ଣ୍ଣସଂଖ୍ୟା is_drag)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		repeat;

    repeat = ((clip_star.mode == SELECT_MODE_CHAR
		|| clip_star.mode == SELECT_MODE_ଧାଡ଼ି)
					      && (mod_mask & MOD_MASK_2CLICK))
	    || (clip_star.mode == SELECT_MODE_WORD
					     && (mod_mask & MOD_MASK_3CLICK));
    ଯଦି (is_click && button == MOUSE_ଡାହାଣ)
    {
	// Right mouse button: If there was no selection, start one.
	// Otherwise extend the existing selection.
	ଯଦି (clip_star.state == SELECT_CLEARED)
	    clip_start_selection(mouse_col, mouse_row, FALSE);
	clip_process_selection(button, mouse_col, mouse_row, repeat);
    }
    ଅନ୍ୟଥା ଯଦି (is_click)
	clip_start_selection(mouse_col, mouse_row, repeat);
    ଅନ୍ୟଥା ଯଦି (is_drag)
    {
	// Don't try extending a selection if there isn't one.  Happens when
	// button-down is in the cmdline and them moving mouse upwards.
	ଯଦି (clip_star.state != SELECT_CLEARED)
	    clip_process_selection(button, mouse_col, mouse_row, repeat);
    }
    ଅନ୍ୟଥା // release
	clip_process_selection(MOUSE_RELEASE, mouse_col, mouse_row, FALSE);
}

/*
 * Update the currently selected region by adding and/or subtracting from the
 * beginning or end and inverting the changed area(s).
 */
    ସ୍ଥିର ଶୂନ୍ୟ
clip_update_modeless_selection(
    Clipboard_T    *cb,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    row1,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    col1,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    row2,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    col2)
{
    // See if we changed at the beginning of the selection
    ଯଦି (row1 != cb->start.lnum || col1 != (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)cb->start.col)
    {
	clip_invert_area(cb, row1, col1, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)cb->start.lnum, cb->start.col,
								 CLIP_TOGGLE);
	cb->start.lnum = row1;
	cb->start.col  = col1;
    }

    // See if we changed at the end of the selection
    ଯଦି (row2 != cb->end.lnum || col2 != (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)cb->end.col)
    {
	clip_invert_area(cb, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)cb->end.lnum, cb->end.col, row2, col2,
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

    ସ୍ଥିର ଶୂନ୍ୟ
clip_get_word_boundaries(Clipboard_T *cb, ପୂର୍ଣ୍ଣସଂଖ୍ୟା row, ପୂର୍ଣ୍ଣସଂଖ୍ୟା col)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		start_class;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		temp_col;
    char_u	*p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		mboff;

    ଯଦି (row >= screen_Rows || col >= screen_Columns || Screen_ଧାଡ଼ିଗୁଡ଼ିକ == ଖାଲି)
	ଫେରନ୍ତୁ;

    p = Screen_ଧାଡ଼ିଗୁଡ଼ିକ + ଧାଡ଼ି_Offset[row];
    // Correct for starting in the right half of a double-wide char
    ଯଦି (enc_dbcs != 0)
	col -= dbcs_screen_head_off(p, p + col);
    ଅନ୍ୟଥା ଯଦି (enc_utf8 && p[col] == 0)
	--col;
    start_class = CHAR_CLASS(p[col]);

    temp_col = col;
    ପାଇଁ ( ; temp_col > 0; temp_col--)
	ଯଦି (enc_dbcs != 0
		   && (mboff = dbcs_screen_head_off(p, p + temp_col - 1)) > 0)
	    temp_col -= mboff;
	ଅନ୍ୟଥା ଯଦି (CHAR_CLASS(p[temp_col - 1]) != start_class
		&& !(enc_utf8 && p[temp_col - 1] == 0))
	    ଭାଙ୍ଗ;
    cb->word_start_col = temp_col;

    temp_col = col;
    ପାଇଁ ( ; temp_col < screen_Columns; temp_col++)
	ଯଦି (enc_dbcs != 0 && dbcs_ptr2cells(p + temp_col) == 2)
	    ++temp_col;
	ଅନ୍ୟଥା ଯଦି (CHAR_CLASS(p[temp_col]) != start_class
		&& !(enc_utf8 && p[temp_col] == 0))
	    ଭାଙ୍ଗ;
    cb->word_end_col = temp_col;
}

/*
 * Find the column position for the last non-whitespace character on the given
 * line at or before start_col.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
clip_get_ଧାଡ଼ି_end(Clipboard_T *cbd UNUSED, ପୂର୍ଣ୍ଣସଂଖ୍ୟା row)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    i;

    ଯଦି (row >= screen_Rows || Screen_ଧାଡ଼ିଗୁଡ଼ିକ == ଖାଲି)
	ଫେରନ୍ତୁ 0;
    ପାଇଁ (i =
# ifdef FEAT_PROP_POPUP
	    cbd->max_col;
# ଅନ୍ୟଥା
	    screen_Columns;
# endif
			    i > 0; i--)
	ଯଦି (Screen_ଧାଡ଼ିଗୁଡ଼ିକ[ଧାଡ଼ି_Offset[row] + i - 1] != ' ')
	    ଭାଙ୍ଗ;
    ଫେରନ୍ତୁ i;
}

/*
 * Start the selection
 */
    ଶୂନ୍ୟ
clip_start_selection(ପୂର୍ଣ୍ଣସଂଖ୍ୟା col, ପୂର୍ଣ୍ଣସଂଖ୍ୟା row, ପୂର୍ଣ୍ଣସଂଖ୍ୟା repeated_click)
{
    Clipboard_T	*cb = &clip_star;
# ifdef FEAT_PROP_POPUP
    win_T	*wp;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		row_cp = row;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		col_cp = col;

    wp = mouse_find_win(&row_cp, &col_cp, FIND_POPUP);
    ଯଦି (wp != ଖାଲି && WIN_IS_POPUP(wp)
				  && popup_is_in_scrollbar(wp, row_cp, col_cp))
	// click or double click in scrollbar does not start a selection
	ଫେରନ୍ତୁ;
# endif

    ଯଦି (cb->state == SELECT_DONE)
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
    ଯଦି (wp != ଖାଲି && WIN_IS_POPUP(wp))
    {
	// Click in a popup window restricts selection to that window,
	// excluding the border.
	cb->min_col = wp->w_wincol + wp->w_popup_border[3];
	cb->max_col = wp->w_wincol + popup_width(wp)
				 - wp->w_popup_border[1] - wp->w_has_scrollbar;
	ଯଦି (cb->max_col > screen_Columns)
	    cb->max_col = screen_Columns;
	cb->min_row = wp->w_winrow + wp->w_popup_border[0];
	cb->max_row = wp->w_winrow + popup_height(wp) - 1
						   - wp->w_popup_border[2];
    }
    ଅନ୍ୟଥା
    {
	cb->min_col = 0;
	cb->max_col = screen_Columns;
	cb->min_row = 0;
	cb->max_row = screen_Rows;
    }
# endif

    ଯଦି (repeated_click)
    {
	ଯଦି (++cb->mode > SELECT_MODE_ଧାଡ଼ି)
	    cb->mode = SELECT_MODE_CHAR;
    }
    ଅନ୍ୟଥା
	cb->mode = SELECT_MODE_CHAR;

# ifdef FEAT_GUI
    // clear the cursor until the selection is made
    ଯଦି (gui.in_use)
	gui_undraw_cursor();
# endif

    ବାଛ (cb->mode)
    {
	ସ୍ଥିତି SELECT_MODE_CHAR:
	    cb->origin_start_col = cb->start.col;
	    cb->word_end_col = clip_get_ଧାଡ଼ି_end(cb, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)cb->start.lnum);
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି SELECT_MODE_WORD:
	    clip_get_word_boundaries(cb, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)cb->start.lnum, cb->start.col);
	    cb->origin_start_col = cb->word_start_col;
	    cb->origin_end_col	 = cb->word_end_col;

	    clip_invert_area(cb, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)cb->start.lnum, cb->word_start_col,
			    (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)cb->end.lnum, cb->word_end_col, CLIP_SET);
	    cb->start.col = cb->word_start_col;
	    cb->end.col   = cb->word_end_col;
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି SELECT_MODE_ଧାଡ଼ି:
	    clip_invert_area(cb, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)cb->start.lnum, 0, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)cb->start.lnum,
			    (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)Columns, CLIP_SET);
	    cb->start.col = 0;
	    cb->end.col   = Columns;
	    ଭାଙ୍ଗ;
    }

    cb->prev = cb->start;

# ifdef DEBUG_SELECTION
    printf("Selection started at (%ld,%d)\n", cb->start.lnum, cb->start.col);
# endif
}

/*
 * Continue processing the selection
 */
    ଶୂନ୍ୟ
clip_process_selection(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		button,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		col,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		row,
    int_u	repeated_click)
{
    Clipboard_T	*cb = &clip_star;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		diff;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		slen = 1;	// cursor shape width

    ଯଦି (button == MOUSE_RELEASE)
    {
	ଯଦି (cb->state != SELECT_IN_PROGRESS)
	    ଫେରନ୍ତୁ;

	// Check to make sure we have something selected
	ଯଦି (cb->start.lnum == cb->end.lnum && cb->start.col == cb->end.col)
	{
# ifdef FEAT_GUI
	    ଯଦି (gui.in_use)
		gui_update_cursor(FALSE, FALSE);
# endif
	    cb->state = SELECT_CLEARED;
	    ଫେରନ୍ତୁ;
	}

# ifdef DEBUG_SELECTION
	printf("Selection ended: (%ld,%d) to (%ld,%d)\n", cb->start.lnum,
		cb->start.col, cb->end.lnum, cb->end.col);
# endif
	ଯଦି (clip_isautosel_star() || clip_isautosel_plus()
		|| (
# ifdef FEAT_GUI
		    gui.in_use ? (vim_strchr(p_go, GO_ASELML) != ଖାଲି) :
# endif
		    clip_autoselectml))
	    clip_copy_modeless_selection(FALSE);
# ifdef FEAT_GUI
	ଯଦି (gui.in_use)
	    gui_update_cursor(FALSE, FALSE);
# endif

	cb->state = SELECT_DONE;
	ଫେରନ୍ତୁ;
    }

    row = check_row(row);
    col = check_col(col);
    col = mb_fix_col(col, row);

    ଯଦି (col == (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)cb->prev.col && row == cb->prev.lnum && !repeated_click)
	ଫେରନ୍ତୁ;

    /*
     * When extending the selection with the right mouse button, swap the
     * start and end if the position is before half the selection
     */
    ଯଦି (cb->state == SELECT_DONE && button == MOUSE_ଡାହାଣ)
    {
	/*
	 * If the click is before the start, or the click is inside the
	 * selection and the start is the closest side, set the origin to the
	 * end of the selection.
	 */
	ଯଦି (clip_compare_pos(row, col, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)cb->start.lnum, cb->start.col) < 0
		|| (clip_compare_pos(row, col,
					   (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)cb->end.lnum, cb->end.col) < 0
		    && (((cb->start.lnum == cb->end.lnum
			    && cb->end.col - col > col - cb->start.col))
			|| ((diff = (cb->end.lnum - row) -
						   (row - cb->start.lnum)) > 0
			    || (diff == 0 && col < (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(cb->start.col +
							 cb->end.col) / 2)))))
	{
	    cb->origin_row = (short_u)cb->end.lnum;
	    cb->origin_start_col = cb->end.col - 1;
	    cb->origin_end_col = cb->end.col;
	}
	ଅନ୍ୟଥା
	{
	    cb->origin_row = (short_u)cb->start.lnum;
	    cb->origin_start_col = cb->start.col;
	    cb->origin_end_col = cb->start.col;
	}
	ଯଦି (cb->mode == SELECT_MODE_WORD && !repeated_click)
	    cb->mode = SELECT_MODE_CHAR;
    }

    // set state, for when using the right mouse button
    cb->state = SELECT_IN_PROGRESS;

# ifdef DEBUG_SELECTION
    printf("Selection extending to (%d,%d)\n", row, col);
# endif

    ଯଦି (repeated_click && ++cb->mode > SELECT_MODE_ଧାଡ଼ି)
	cb->mode = SELECT_MODE_CHAR;

    ବାଛ (cb->mode)
    {
	ସ୍ଥିତି SELECT_MODE_CHAR:
	    // If we're on a different line, find where the line ends
	    ଯଦି (row != cb->prev.lnum)
		cb->word_end_col = clip_get_ଧାଡ଼ି_end(cb, row);

	    // See if we are before or after the origin of the selection
	    ଯଦି (clip_compare_pos(row, col, cb->origin_row,
						   cb->origin_start_col) >= 0)
	    {
		ଯଦି (col >= (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)cb->word_end_col)
		    clip_update_modeless_selection(cb, cb->origin_row,
			    cb->origin_start_col, row, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)Columns);
		ଅନ୍ୟଥା
		{
		    ଯଦି (has_mbyte && mb_lefthalve(row, col))
			slen = 2;
		    clip_update_modeless_selection(cb, cb->origin_row,
			    cb->origin_start_col, row, col + slen);
		}
	    }
	    ଅନ୍ୟଥା
	    {
		ଯଦି (has_mbyte
			&& mb_lefthalve(cb->origin_row, cb->origin_start_col))
		    slen = 2;
		ଯଦି (col >= (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)cb->word_end_col)
		    clip_update_modeless_selection(cb, row, cb->word_end_col,
			    cb->origin_row, cb->origin_start_col + slen);
		ଅନ୍ୟଥା
		    clip_update_modeless_selection(cb, row, col,
			    cb->origin_row, cb->origin_start_col + slen);
	    }
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି SELECT_MODE_WORD:
	    // If we are still within the same word, do nothing
	    ଯଦି (row == cb->prev.lnum && col >= (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)cb->word_start_col
		    && col < (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)cb->word_end_col && !repeated_click)
		ଫେରନ୍ତୁ;

	    // Get new word boundaries
	    clip_get_word_boundaries(cb, row, col);

	    // Handle being after the origin point of selection
	    ଯଦି (clip_compare_pos(row, col, cb->origin_row,
		    cb->origin_start_col) >= 0)
		clip_update_modeless_selection(cb, cb->origin_row,
			cb->origin_start_col, row, cb->word_end_col);
	    ଅନ୍ୟଥା
		clip_update_modeless_selection(cb, row, cb->word_start_col,
			cb->origin_row, cb->origin_end_col);
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି SELECT_MODE_ଧାଡ଼ି:
	    ଯଦି (row == cb->prev.lnum && !repeated_click)
		ଫେରନ୍ତୁ;

	    ଯଦି (clip_compare_pos(row, col, cb->origin_row,
		    cb->origin_start_col) >= 0)
		clip_update_modeless_selection(cb, cb->origin_row, 0, row,
			(ପୂର୍ଣ୍ଣସଂଖ୍ୟା)Columns);
	    ଅନ୍ୟଥା
		clip_update_modeless_selection(cb, row, 0, cb->origin_row,
			(ପୂର୍ଣ୍ଣସଂଖ୍ୟା)Columns);
	    ଭାଙ୍ଗ;
    }

    cb->prev.lnum = row;
    cb->prev.col  = col;

# ifdef DEBUG_SELECTION
	printf("Selection is: (%ld,%d) to (%ld,%d)\n", cb->start.lnum,
		cb->start.col, cb->end.lnum, cb->end.col);
# endif
}

# ଯଦି defined(FEAT_GUI)
/*
 * Redraw part of the selection if character at "row,col" is inside of it.
 * Only used for the GUI.
 */
    ଶୂନ୍ୟ
clip_may_redraw_selection(ପୂର୍ଣ୍ଣସଂଖ୍ୟା row, ପୂର୍ଣ୍ଣସଂଖ୍ୟା col, ପୂର୍ଣ୍ଣସଂଖ୍ୟା len)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		start = col;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		end = col + len;

    ଯଦି (clip_star.state != SELECT_CLEARED
	    && row >= clip_star.start.lnum
	    && row <= clip_star.end.lnum)
    {
	ଯଦି (row == clip_star.start.lnum && start < (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)clip_star.start.col)
	    start = clip_star.start.col;
	ଯଦି (row == clip_star.end.lnum && end > (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)clip_star.end.col)
	    end = clip_star.end.col;
	ଯଦି (end > start)
	    clip_invert_area(&clip_star, row, start, row, end, 0);
    }
}
# endif

/*
 * Called from outside to clear selected region from the display
 */
    ଶୂନ୍ୟ
clip_clear_selection(Clipboard_T *cbd)
{

    ଯଦି (cbd->state == SELECT_CLEARED)
	ଫେରନ୍ତୁ;

    clip_invert_area(cbd, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)cbd->start.lnum, cbd->start.col,
				 (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)cbd->end.lnum, cbd->end.col, CLIP_CLEAR);
    cbd->state = SELECT_CLEARED;
}

/*
 * Clear the selection if any lines from "row1" to "row2" are inside of it.
 */
    ଶୂନ୍ୟ
clip_may_clear_selection(ପୂର୍ଣ୍ଣସଂଖ୍ୟା row1, ପୂର୍ଣ୍ଣସଂଖ୍ୟା row2)
{
    ଯଦି (clip_star.state == SELECT_DONE
	    && row2 >= clip_star.start.lnum
	    && row1 <= clip_star.end.lnum)
	clip_clear_selection(&clip_star);
}

/*
 * Called before the screen is scrolled up or down.  Adjusts the line numbers
 * of the selection.  Call with big number when clearing the screen.
 */
    ଶୂନ୍ୟ
clip_scroll_selection(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    rows)		// negative for scroll down
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    lnum;

    ଯଦି (clip_star.state == SELECT_CLEARED)
	ଫେରନ୍ତୁ;

    lnum = clip_star.start.lnum - rows;
    ଯଦି (lnum <= 0)
	clip_star.start.lnum = 0;
    ଅନ୍ୟଥା ଯଦି (lnum >= screen_Rows)	// scrolled off of the screen
	clip_star.state = SELECT_CLEARED;
    ଅନ୍ୟଥା
	clip_star.start.lnum = lnum;

    lnum = clip_star.end.lnum - rows;
    ଯଦି (lnum < 0)			// scrolled off of the screen
	clip_star.state = SELECT_CLEARED;
    ଅନ୍ୟଥା ଯଦି (lnum >= screen_Rows)
	clip_star.end.lnum = screen_Rows - 1;
    ଅନ୍ୟଥା
	clip_star.end.lnum = lnum;
}

/*
 * Copy the currently selected area into the '*' or '+' register so it will be
 * available for pasting.
 * When "both" is TRUE also copy to the other register.
 */
    ଶୂନ୍ୟ
clip_copy_modeless_selection(ପୂର୍ଣ୍ଣସଂଖ୍ୟା both UNUSED)
{
    // The info for the modeless selection is stored in '*' register, however if
    // we are using the '+' register for modeless autoselect, we copy to
    // clip_plus instead while using the info in clip_star.
    Clipboard_T *cbd = clip_isautosel_plus() ? &clip_plus : &clip_star;
    char_u	*buffer;
    char_u	*bufp;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		row;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		start_col;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		end_col;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ଧାଡ଼ି_end_col;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		add_newline_flag = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len;
    char_u	*p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		row1 = clip_star.start.lnum;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		col1 = clip_star.start.col;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		row2 = clip_star.end.lnum;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		col2 = clip_star.end.col;

    // Can't use ScreenLines unless initialized
    ଯଦି (Screen_ଧାଡ଼ିଗୁଡ଼ିକ == ଖାଲି)
	ଫେରନ୍ତୁ;

    /*
     * Make sure row1 <= row2, and if row1 == row2 that col1 <= col2.
     */
    ଯଦି (row1 > row2)
    {
	row = row1; row1 = row2; row2 = row;
	row = col1; col1 = col2; col2 = row;
    }
    ଅନ୍ୟଥା ଯଦି (row1 == row2 && col1 > col2)
    {
	row = col1; col1 = col2; col2 = row;
    }
# ifdef FEAT_PROP_POPUP
    ଯଦି (col1 < clip_star.min_col)
	col1 = clip_star.min_col;
    ଯଦି (col2 > clip_star.max_col)
	col2 = clip_star.max_col;
    ଯଦି (row1 > clip_star.max_row || row2 < clip_star.min_row)
	ଫେରନ୍ତୁ;
    ଯଦି (row1 < clip_star.min_row)
	row1 = clip_star.min_row;
    ଯଦି (row2 > clip_star.max_row)
	row2 = clip_star.max_row;
# endif
    // correct starting point for being on right half of double-wide char
    p = Screen_ଧାଡ଼ିଗୁଡ଼ିକ + ଧାଡ଼ି_Offset[row1];
    ଯଦି (enc_dbcs != 0)
	col1 -= (*mb_head_off)(p, p + col1);
    ଅନ୍ୟଥା ଯଦି (enc_utf8 && p[col1] == 0)
	--col1;

    // Create a temporary buffer for storing the text
    len = (row2 - row1 + 1) * Columns + 1;
    ଯଦି (enc_dbcs != 0)
	len *= 2;	// max. 2 bytes per display cell
    ଅନ୍ୟଥା ଯଦି (enc_utf8)
	len *= MB_MAXBYTES;
    // A newline is added after each row
    len += row2 - row1 + 2;
    buffer = alloc(len);
    ଯଦି (buffer == ଖାଲି)	    // out of memory
	ଫେରନ୍ତୁ;

    // Process each row in the selection
    ପାଇଁ (bufp = buffer, row = row1; row <= row2; row++)
    {
	ଯଦି (row == row1)
	    start_col = col1;
	ଅନ୍ୟଥା
# ifdef FEAT_PROP_POPUP
	    start_col = clip_star.min_col;
# ଅନ୍ୟଥା
	    start_col = 0;
# endif

	ଯଦି (row == row2)
	    end_col = col2;
	ଅନ୍ୟଥା
# ifdef FEAT_PROP_POPUP
	    end_col = clip_star.max_col;
# ଅନ୍ୟଥା
	    end_col = Columns;
# endif

	ଧାଡ଼ି_end_col = clip_get_ଧାଡ଼ି_end(&clip_star, row);

	// See if we need to nuke some trailing whitespace
	ଯଦି (end_col >=
# ifdef FEAT_PROP_POPUP
		clip_star.max_col
# ଅନ୍ୟଥା
		Columns
# endif
		    && (row < row2 || end_col > ଧାଡ଼ି_end_col))
	{
	    // Get rid of trailing whitespace
	    end_col = ଧାଡ଼ି_end_col;
	    ଯଦି (end_col < start_col)
		end_col = start_col;

	    // If the last line extended to the end, add an extra newline
	    ଯଦି (row == row2)
		add_newline_flag = TRUE;
	}

	// If after the first row, we need to always add a newline
	ଯଦି (row > row1 && !ଧାଡ଼ି_Wraps[row - 1])
	    *bufp++ = NL;

	// Safetey check for in case resizing went wrong
	ଯଦି (row < screen_Rows && end_col <= screen_Columns)
	{
	    ଯଦି (enc_dbcs != 0)
	    {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା	i;

		p = Screen_ଧାଡ଼ିଗୁଡ଼ିକ + ଧାଡ଼ି_Offset[row];
		ପାଇଁ (i = start_col; i < end_col; ++i)
		    ଯଦି (enc_dbcs == DBCS_JPNU && p[i] == 0x8e)
		    {
			// single-width double-byte char
			*bufp++ = 0x8e;
			*bufp++ = ScreenLines2[ଧାଡ଼ି_Offset[row] + i];
		    }
		    ଅନ୍ୟଥା
		    {
			*bufp++ = p[i];
			ଯଦି (MB_BYTE2LEN(p[i]) == 2)
			    *bufp++ = p[++i];
		    }
	    }
	    ଅନ୍ୟଥା ଯଦି (enc_utf8)
	    {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା	off;
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା	i;

		off = ଧାଡ଼ି_Offset[row];
		ପାଇଁ (i = start_col; i < end_col; ++i)
		{
		    // The base character is either in ScreenLinesUC[] or
		    // ScreenLines[].
		    ଯଦି (Screen_ଧାଡ଼ିଗୁଡ଼ିକ_UC[off + i] == 0)
			*bufp++ = Screen_ଧାଡ଼ିଗୁଡ଼ିକ[off + i];
		    ଅନ୍ୟଥା
			// Includes the composing characters.
			bufp += utfc_char2bytes(off + i, bufp);
		    // Skip right half of double-wide character.
		    ଯଦି (Screen_ଧାଡ଼ିଗୁଡ଼ିକ[off + i + 1] == 0)
			++i;
		}
	    }
	    ଅନ୍ୟଥା
	    {
		STRNCPY(bufp, Screen_ଧାଡ଼ିଗୁଡ଼ିକ + ଧାଡ଼ି_Offset[row] + start_col,
							 end_col - start_col);
		bufp += end_col - start_col;
	    }
	}
    }

    // Add a newline at the end if the selection ended there
    ଯଦି (add_newline_flag)
	*bufp++ = NL;

    // First cleanup any old selection and become the owner.
    clip_free_selection(cbd);
    clip_own_selection(cbd);

    // Yank the text into the '*' register.
    clip_yank_selection(MCHAR, buffer, (long)(bufp - buffer), cbd);

    // Make the register contents available to the outside world.
    clip_gen_set_selection(cbd);

# ifdef FEAT_X11
    ଯଦି (both)
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

    ଶୂନ୍ୟ
clip_gen_set_selection(Clipboard_T *cbd)
{
    ଯଦି (!clip_did_set_selection)
    {
	// Updating postponed, so that accessing the system clipboard won't
	// hang Vim when accessing it many times (e.g. on a :g command).
	ଯଦି ((cbd == &clip_plus && (clip_unnamed_saved & CLIP_UNNAMED_PLUS))
		|| (cbd == &clip_star && (clip_unnamed_saved & CLIP_UNNAMED)))
	{
	    clipboard_needs_update = TRUE;
	    ଫେରନ୍ତୁ;
	}
    }
# ଯଦି defined(FEAT_XCLIPBOARD) || defined(FEAT_WAYLAND_CLIPBOARD)
#  ifdef FEAT_GUI
    ଯଦି (gui.in_use)
	clip_mch_set_selection(cbd);
    ଅନ୍ୟଥା
#  endif
    {
	ଯଦି (clipmethod == CLIPMETHOD_WAYLAND)
	{
#  ifdef FEAT_WAYLAND_CLIPBOARD
	    clip_wl_set_selection(cbd);
#  endif
	}
	ଅନ୍ୟଥା ଯଦି (clipmethod == CLIPMETHOD_X11)
	{
#  ifdef FEAT_XCLIPBOARD
	    clip_xterm_set_selection(cbd);
#  endif
	}
    }
# ଅନ୍ୟଥା
    clip_mch_set_selection(cbd);
# endif
}

    ସ୍ଥିର ଶୂନ୍ୟ
clip_gen_request_selection(Clipboard_T *cbd)
{
# ଯଦି defined(FEAT_XCLIPBOARD) || defined(FEAT_WAYLAND_CLIPBOARD)
#  ifdef FEAT_GUI
    ଯଦି (gui.in_use)
	clip_mch_request_selection(cbd);
    ଅନ୍ୟଥା
#  endif
    {
	ଯଦି (clipmethod == CLIPMETHOD_WAYLAND)
	{
#  ifdef FEAT_WAYLAND_CLIPBOARD
	    clip_wl_request_selection(cbd);
#  endif
	}
	ଅନ୍ୟଥା ଯଦି (clipmethod == CLIPMETHOD_X11)
	{
#  ifdef FEAT_XCLIPBOARD
	    clip_xterm_request_selection(cbd);
#  endif
	}
    }
# ଅନ୍ୟଥା
    clip_mch_request_selection(cbd);
# endif
}

# ଯଦି (defined(FEAT_X11) && defined(FEAT_XCLIPBOARD) && defined(USE_SYSTEM))
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
clip_x11_owner_exists(Clipboard_T *cbd)
{
    ଫେରନ୍ତୁ XGetSelectionOwner(X_DISPLAY, cbd->sel_atom) != None;
}
# endif

# ଯଦି (defined(FEAT_X11) || defined(FEAT_WAYLAND_CLIPBOARD)) \
	&& defined(USE_SYSTEM)
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
clip_gen_owner_exists(Clipboard_T *cbd UNUSED)
{
#  ଯଦି defined(FEAT_XCLIPBOARD) || defined(FEAT_WAYLAND_CLIPBOARD)
#   ifdef FEAT_GUI_GTK
    ଯଦି (gui.in_use)
	ଫେରନ୍ତୁ clip_gtk_owner_exists(cbd);
    ଅନ୍ୟଥା
#   endif
    {
	ଯଦି (clipmethod == CLIPMETHOD_WAYLAND)
	{
#   ifdef FEAT_WAYLAND_CLIPBOARD
	    ଫେରନ୍ତୁ clip_wl_owner_exists(cbd);
#   endif
	}
	ଅନ୍ୟଥା ଯଦି (clipmethod == CLIPMETHOD_X11)
	{
#   ifdef FEAT_XCLIPBOARD
	    ଫେରନ୍ତୁ clip_x11_owner_exists(cbd);
#   endif
	}
	ଅନ୍ୟଥା
	    ଫେରନ୍ତୁ FALSE;
    }
#  ଅନ୍ୟଥା
    ଫେରନ୍ତୁ TRUE;
#  endif
}
# endif

/*
 * Stuff for the X clipboard.  Shared between VMS and Unix.
 */

# ଯଦି defined(FEAT_XCLIPBOARD) || defined(FEAT_GUI_X11)
#  include <X11/Xatom.h>
#  include <X11/Intrinsic.h>

/*
 * Open the application context (if it hasn't been opened yet).
 * Used for Motif GUI and the xterm clipboard.
 */
    ଶୂନ୍ୟ
open_app_context(ଶୂନ୍ୟ)
{
    ଯଦି (app_context == ଖାଲି)
    {
	XtToolkitInitialize();
	app_context = XtCreateApplicationContext();
    }
}

ସ୍ଥିର Atom	vim_atom;	// Vim's own special selection format
ସ୍ଥିର Atom	vimenc_atom;	// Vim's extended selection format
ସ୍ଥିର Atom	vim_mt_atom;	// Vim's own special selection format (in mime
				// type format)
ସ୍ଥିର Atom	vimenc_mt_atom;	// Vim's extended selection format (in mime type
				// format)
ସ୍ଥିର Atom	utf8_atom;
ସ୍ଥିର Atom	compound_text_atom;
ସ୍ଥିର Atom	text_atom;
ସ୍ଥିର Atom	targets_atom;
ସ୍ଥିର Atom	timestamp_atom;	// Used to get a timestamp

    ଶୂନ୍ୟ
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

    ସ୍ଥିର Boolean
clip_x11_convert_selection_cb(
    Widget	w UNUSED,
    Atom	*sel_atom,
    Atom	*target,
    Atom	*type,
    XtPointer	*ମୂଲ୍ୟ,
    long_u	*length,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*format)
{
    ସ୍ଥିର char_u   *save_result = ଖାଲି;
    ସ୍ଥିର long_u   save_length = 0;
    char_u	    *string;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    motion_type;
    Clipboard_T    *cbd;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    i;

    ଯଦି (*sel_atom == clip_plus.sel_atom)
	cbd = &clip_plus;
    ଅନ୍ୟଥା
	cbd = &clip_star;

    ଯଦି (!cbd->owned)
	ଫେରନ୍ତୁ False;	    // Shouldn't ever happen

    // requestor wants to know what target types we support
    ଯଦି (*target == targets_atom)
    {
	ସ୍ଥିର Atom array[9];

	*ମୂଲ୍ୟ = (XtPointer)array;
	i = 0;
	array[i++] = targets_atom;
	array[i++] = vimenc_atom;
	array[i++] = vim_atom;
	array[i++] = vimenc_mt_atom;
	array[i++] = vim_mt_atom;
	ଯଦି (enc_utf8)
	    array[i++] = utf8_atom;
	array[i++] = XA_STRING;
	array[i++] = text_atom;
	array[i++] = compound_text_atom;

	*type = XA_ATOM;
	// This used to be: *format = sizeof(Atom) * 8; but that caused
	// crashes on 64 bit machines. (Peter Derr)
	*format = 32;
	*length = i;
	ଫେରନ୍ତୁ True;
    }

    ଯଦି (       *target != XA_STRING
	    && *target != vimenc_atom
	    && *target != vimenc_mt_atom
	    && (*target != utf8_atom || !enc_utf8)
	    && *target != vim_atom
	    && *target != vim_mt_atom
	    && *target != text_atom
	    && *target != compound_text_atom)
	ଫେରନ୍ତୁ False;

    clip_get_selection(cbd);
    motion_type = clip_convert_selection(&string, length, cbd);
    ଯଦି (motion_type < 0)
	ଫେରନ୍ତୁ False;

    // For our own format, the first byte contains the motion type
    ଯଦି (*target == vim_atom || *target == vim_mt_atom)
	(*length)++;

    // Our own format with encoding: motion 'encoding' NUL text
    ଯଦି (*target == vimenc_atom || *target == vimenc_mt_atom)
	*length += STRLEN(p_enc) + 2;

    ଯଦି (save_length < *length || save_length / 2 >= *length)
	*ମୂଲ୍ୟ = XtRealloc((ଅକ୍ଷର *)save_result, (Cardinal)*length + 1);
    ଅନ୍ୟଥା
	*ମୂଲ୍ୟ = save_result;
    ଯଦି (*ମୂଲ୍ୟ == ଖାଲି)
    {
	vim_free(string);
	ଫେରନ୍ତୁ False;
    }
    save_result = (char_u *)*ମୂଲ୍ୟ;
    save_length = *length;

    ଯଦି (*target == XA_STRING || (*target == utf8_atom && enc_utf8))
    {
	mch_memmove(save_result, string, (size_t)(*length));
	*type = *target;
    }
    ଅନ୍ୟଥା ଯଦି (*target == compound_text_atom || *target == text_atom)
    {
	XTextProperty	text_prop;
	ଅକ୍ଷର		*string_nt = (ଅକ୍ଷର *)save_result;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		conv_result;

	// create NUL terminated string which XmbTextListToTextProperty wants
	mch_memmove(string_nt, string, (size_t)*length);
	string_nt[*length] = NUL;
	conv_result = XmbTextListToTextProperty(X_DISPLAY, &string_nt,
					   1, XCompoundTextStyle, &text_prop);
	ଯଦି (conv_result != Success)
	{
	    vim_free(string);
	    ଫେରନ୍ତୁ False;
	}
	*ମୂଲ୍ୟ = (XtPointer)(text_prop.ମୂଲ୍ୟ);	//    from plain text
	*length = text_prop.nitems;
	*type = compound_text_atom;
	XtFree((ଅକ୍ଷର *)save_result);
	save_result = (char_u *)*ମୂଲ୍ୟ;
	save_length = *length;
    }
    ଅନ୍ୟଥା ଯଦି (*target == vimenc_atom || *target == vimenc_mt_atom)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା l = STRLEN(p_enc);

	save_result[0] = motion_type;
	STRCPY(save_result + 1, p_enc);
	mch_memmove(save_result + l + 2, string, (size_t)(*length - l - 2));
	ଯଦି (*target == vimenc_atom)
	    *type = vimenc_atom;
	ଅନ୍ୟଥା
	    *type = vimenc_mt_atom;
    }
    ଅନ୍ୟଥା
    {
	save_result[0] = motion_type;
	mch_memmove(save_result + 1, string, (size_t)(*length - 1));
	ଯଦି (*target == vim_atom)
	    *type = vim_atom;
	ଅନ୍ୟଥା
	    *type = vim_mt_atom;
    }
    *format = 8;	    // 8 bits per char
    vim_free(string);
    ଫେରନ୍ତୁ True;
}

    ସ୍ଥିର ଶୂନ୍ୟ
clip_x11_lose_ownership_cb(Widget w UNUSED, Atom *sel_atom)
{
    ଯଦି (*sel_atom == clip_plus.sel_atom)
	clip_lose_selection(&clip_plus);
    ଅନ୍ୟଥା
	clip_lose_selection(&clip_star);
}

    ସ୍ଥିର ଶୂନ୍ୟ
clip_x11_notify_cb(Widget w UNUSED, Atom *sel_atom UNUSED, Atom *target UNUSED)
{
    // To prevent automatically freeing the selection value.
}

/*
 * Property callback to get a timestamp for XtOwnSelection.
 */
#  ଯଦି defined(FEAT_X11) && defined(FEAT_XCLIPBOARD)
    ସ୍ଥିର ଶୂନ୍ୟ
clip_x11_timestamp_cb(
    Widget	w,
    XtPointer	n UNUSED,
    XEvent	*event,
    Boolean	*cont UNUSED)
{
    Atom	    actual_type;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    format;
    unsigned  long  nitems, bytes_after;
    unsigned ଅକ୍ଷର   *prop=ଖାଲି;
    XPropertyEvent  *xproperty=&event->xproperty;

    // Must be a property notify, state can't be Delete (True), has to be
    // one of the supported selection types.
    ଯଦି (event->type != PropertyNotify || xproperty->state
	    || (xproperty->atom != clip_star.sel_atom
				    && xproperty->atom != clip_plus.sel_atom))
	ଫେରନ୍ତୁ;

    ଯଦି (XGetWindowProperty(xproperty->display, xproperty->window,
	  xproperty->atom, 0, 0, False, timestamp_atom, &actual_type, &format,
						&nitems, &bytes_after, &prop))
	ଫେରନ୍ତୁ;

    ଯଦି (prop)
	XFree(prop);

    // Make sure the property type is "TIMESTAMP" and it's 32 bits.
    ଯଦି (actual_type != timestamp_atom || format != 32)
	ଫେରନ୍ତୁ;

    // Get the selection, using the event timestamp.
    ଯଦି (XtOwnSelection(w, xproperty->atom, xproperty->time,
	    clip_x11_convert_selection_cb, clip_x11_lose_ownership_cb,
	    clip_x11_notify_cb) == OK)
    {
	// Set the "owned" flag now, there may have been a call to
	// lose_ownership_cb in between.
	ଯଦି (xproperty->atom == clip_plus.sel_atom)
	    clip_plus.owned = TRUE;
	ଅନ୍ୟଥା
	    clip_star.owned = TRUE;
    }
}

    ଶୂନ୍ୟ
x11_setup_selection(Widget w)
{
    XtAddEventHandler(w, PropertyChangeMask, False,
	    /*(XtEventHandler)*/clip_x11_timestamp_cb, (XtPointer)ଖାଲି);
}
#  endif

    ସ୍ଥିର ଶୂନ୍ୟ
clip_x11_request_selection_cb(
    Widget	w UNUSED,
    XtPointer	success,
    Atom	*sel_atom,
    Atom	*type,
    XtPointer	ମୂଲ୍ୟ,
    long_u	*length,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*format)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		motion_type = MAUTO;
    long_u	len;
    char_u	*p;
    ଅକ୍ଷର	**text_list = ଖାଲି;
    Clipboard_T	*cbd;
    char_u	*tmpbuf = ଖାଲି;

    ଯଦି (*sel_atom == clip_plus.sel_atom)
	cbd = &clip_plus;
    ଅନ୍ୟଥା
	cbd = &clip_star;

    ଯଦି (ମୂଲ୍ୟ == ଖାଲି || *length == 0)
    {
	clip_free_selection(cbd);	// nothing received, clear register
	*(ପୂର୍ଣ୍ଣସଂଖ୍ୟା *)success = FALSE;
	ଫେରନ୍ତୁ;
    }
    p = (char_u *)ମୂଲ୍ୟ;
    len = *length;
    ଯଦି (*type == vim_atom || *type == vim_mt_atom)
    {
	motion_type = *p++;
	len--;
    }

    ଅନ୍ୟଥା ଯଦି (*type == vimenc_atom || *type == vimenc_mt_atom)
    {
	char_u		*enc;
	vimconv_T	conv;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		convlen;

	motion_type = *p++;
	--len;

	enc = p;
	p += STRLEN(p) + 1;
	len -= p - enc;

	// If the encoding of the text is different from 'encoding', attempt
	// converting it.
	conv.vc_type = CONV_NONE;
	convert_setup(&conv, enc, p_enc);
	ଯଦି (conv.vc_type != CONV_NONE)
	{
	    convlen = len;	// Need to use an int here.
	    tmpbuf = string_convert(&conv, p, &convlen);
	    len = convlen;
	    ଯଦି (tmpbuf != ଖାଲି)
		p = tmpbuf;
	    convert_setup(&conv, ଖାଲି, ଖାଲି);
	}
    }

    ଅନ୍ୟଥା ଯଦି (*type == compound_text_atom
	    || *type == utf8_atom
	    || (enc_dbcs != 0 && *type == text_atom))
    {
	XTextProperty	text_prop;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		n_text = 0;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		status;

	text_prop.ମୂଲ୍ୟ = (unsigned ଅକ୍ଷର *)ମୂଲ୍ୟ;
	text_prop.encoding = *type;
	text_prop.format = *format;
	text_prop.nitems = len;
#  ଯଦି defined(X_HAVE_UTF8_STRING)
	ଯଦି (*type == utf8_atom)
	    status = Xutf8TextPropertyToTextList(X_DISPLAY, &text_prop,
							 &text_list, &n_text);
	ଅନ୍ୟଥା
#  endif
	    status = XmbTextPropertyToTextList(X_DISPLAY, &text_prop,
							 &text_list, &n_text);
	ଯଦି (status != Success || n_text < 1)
	{
	    *(ପୂର୍ଣ୍ଣସଂଖ୍ୟା *)success = FALSE;
	    ଫେରନ୍ତୁ;
	}
	p = (char_u *)text_list[0];
	len = STRLEN(p);
    }
    clip_yank_selection(motion_type, p, (long)len, cbd);

    ଯଦି (text_list != ଖାଲି)
	XFreeStringList(text_list);
    vim_free(tmpbuf);
    XtFree((ଅକ୍ଷର *)ମୂଲ୍ୟ);
    *(ପୂର୍ଣ୍ଣସଂଖ୍ୟା *)success = TRUE;
}

    ଶୂନ୍ୟ
clip_x11_request_selection(
    Widget	myShell,
    Display	*dpy,
    Clipboard_T	*cbd)
{
    XEvent	event;
    Atom	type;
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	success;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    time_t	start_time;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		timed_out = FALSE;

    ପାଇଁ (i = 0; i < 8; i++)
    {
	ବାଛ (i)
	{
	    ସ୍ଥିତି 0:  type = vimenc_mt_atom;	ଭାଙ୍ଗ;
	    ସ୍ଥିତି 1:  type = vimenc_atom;	ଭାଙ୍ଗ;
	    ସ୍ଥିତି 2:  type = vim_mt_atom;	ଭାଙ୍ଗ;
	    ସ୍ଥିତି 3:  type = vim_atom;		ଭାଙ୍ଗ;
	    ସ୍ଥିତି 4:  type = utf8_atom;		ଭାଙ୍ଗ;
	    ସ୍ଥିତି 5:  type = compound_text_atom; ଭାଙ୍ଗ;
	    ସ୍ଥିତି 6:  type = text_atom;		ଭାଙ୍ଗ;
	    ପୂର୍ବନିର୍ଦ୍ଧାରିତ: type = XA_STRING;
	}
	ଯଦି (type == utf8_atom
#  ଯଦି defined(X_HAVE_UTF8_STRING)
		&& !enc_utf8
#  endif
		)
	    // Only request utf-8 when 'encoding' is utf8 and
	    // Xutf8TextPropertyToTextList is available.
	    ଜାରି;
	success = MAYBE;
	Xt_Get_Selection_ମୂଲ୍ୟ(myShell, cbd->sel_atom, type,
	    clip_x11_request_selection_cb, (XtPointer)&success, CurrentTime);

	// Make sure the request for the selection goes out before waiting for
	// a response.
	XFlush(dpy);

	/*
	 * Wait for result of selection request, otherwise if we type more
	 * characters, then they will appear before the one that requested the
	 * paste!  Don't worry, we will catch up with any other events later.
	 */
	start_time = time(ଖାଲି);
	ଯେତେବେଳେ (success == MAYBE)
	{
	    ଯଦି (XCheckTypedEvent(dpy, PropertyNotify, &event)
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
		ଜାରି;
	    }

	    // Time out after 2 to 3 seconds to avoid that we hang when the
	    // other process doesn't respond.  Note that the SelectionNotify
	    // event may still come later when the selection owner comes back
	    // to life and the text gets inserted unexpectedly.  Don't know
	    // why that happens or how to avoid that :-(.
	    ଯଦି (time(ଖାଲି) > start_time + 2)
	    {
		timed_out = TRUE;
		ଭାଙ୍ଗ;
	    }

	    // Do we need this?  Probably not.
	    XSync(dpy, False);

	    // Wait for 1 msec to avoid that we eat up all CPU time.
	    ui_delay(1L, TRUE);
	}

	ଯଦି (success == TRUE)
	    ଫେରନ୍ତୁ;

	// don't do a retry with another type after timing out, otherwise we
	// hang for 15 seconds.
	ଯଦି (timed_out)
	    ଭାଙ୍ଗ;
    }

    // Final fallback position - use the X CUT_BUFFER0 store
    yank_cut_buffer0(dpy, cbd);
}

    ଶୂନ୍ୟ
clip_x11_lose_selection(Widget myShell, Clipboard_T *cbd)
{
    XtDisownSelection(myShell, cbd->sel_atom,
				XtLastTimestampProcessed(XtDisplay(myShell)));
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
clip_x11_own_selection(Widget myShell, Clipboard_T *cbd)
{
    // When using the GUI we have proper timestamps, use the one of the last
    // event.  When in the console we don't get events (the terminal gets
    // them), Get the time by a zero-length append, clip_x11_timestamp_cb will
    // be called with the current timestamp.
#  ifdef FEAT_GUI
    ଯଦି (gui.in_use)
    {
	ଯଦି (XtOwnSelection(myShell, cbd->sel_atom,
	       XtLastTimestampProcessed(XtDisplay(myShell)),
	       clip_x11_convert_selection_cb, clip_x11_lose_ownership_cb,
	       clip_x11_notify_cb) == False)
	    ଫେରନ୍ତୁ FAIL;
    }
    ଅନ୍ୟଥା
#  endif
    {
	ଯଦି (!XChangeProperty(XtDisplay(myShell), XtWindow(myShell),
		  cbd->sel_atom, timestamp_atom, 32, PropModeAppend, ଖାଲି, 0))
	    ଫେରନ୍ତୁ FAIL;
    }
    // Flush is required in a terminal as nothing else is doing it.
    XFlush(XtDisplay(myShell));
    ଫେରନ୍ତୁ OK;
}

/*
 * Send the current selection to the clipboard.  Do nothing for X because we
 * will fill in the selection only when requested by another app.
 */
    ଶୂନ୍ୟ
clip_x11_set_selection(Clipboard_T *cbd UNUSED)
{
}

# endif

# ଯଦି (defined(FEAT_XCLIPBOARD) || defined(FEAT_GUI_X11) || defined(FEAT_GUI_GTK)) \
	&& !defined(USE_GTK4)
/*
 * Get the contents of the X CUT_BUFFER0 and put it in "cbd".
 */
    ଶୂନ୍ୟ
yank_cut_buffer0(Display *dpy, Clipboard_T *cbd)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		nbytes = 0;
    char_u	*buffer = (char_u *)XFetchBuffer(dpy, &nbytes, 0);

    ଯଦି (nbytes > 0)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା  done = FALSE;

	// CUT_BUFFER0 is supposed to be always latin1.  Convert to 'enc' when
	// using a multi-byte encoding.  Conversion between two 8-bit
	// character sets usually fails and the text might actually be in
	// 'enc' anyway.
	ଯଦି (has_mbyte)
	{
	    char_u	*conv_buf;
	    vimconv_T	vc;

	    vc.vc_type = CONV_NONE;
	    ଯଦି (convert_setup(&vc, (char_u *)"latin1", p_enc) == OK)
	    {
		conv_buf = string_convert(&vc, buffer, &nbytes);
		ଯଦି (conv_buf != ଖାଲି)
		{
		    clip_yank_selection(MCHAR, conv_buf, (long)nbytes, cbd);
		    vim_free(conv_buf);
		    done = TRUE;
		}
		convert_setup(&vc, ଖାଲି, ଖାଲି);
	    }
	}
	ଯଦି (!done)  // use the text without conversion
	    clip_yank_selection(MCHAR, buffer, (long)nbytes, cbd);
	XFree((ଶୂନ୍ୟ *)buffer);
	ଯଦି (p_verbose > 0)
	{
	    verbose_enter();
	    verb_ସନ୍ଦେଶ(_("Used CUT_BUFFER0 instead of empty selection"));
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
# ଯଦି defined(FEAT_X11) && defined(FEAT_CLIPBOARD)
    ଶୂନ୍ୟ
x11_export_final_selection(ଶୂନ୍ୟ)
{
    Display	*dpy;
    char_u	*str = ଖାଲି;
    long_u	len = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		motion_type = -1;

#  ifdef FEAT_GUI
    ଯଦି (gui.in_use)
	dpy = X_DISPLAY;
    ଅନ୍ୟଥା
#  endif
#  ifdef FEAT_XCLIPBOARD
	dpy = xterm_dpy;
#  ଅନ୍ୟଥା
	ଫେରନ୍ତୁ;
#  endif

    // Get selection to export
    ଯଦି (clip_plus.owned)
	motion_type = clip_convert_selection(&str, &len, &clip_plus);
    ଅନ୍ୟଥା ଯଦି (clip_star.owned)
	motion_type = clip_convert_selection(&str, &len, &clip_star);

    // Check it's OK
    ଯଦି (dpy != ଖାଲି && str != ଖାଲି && motion_type >= 0
					       && len < 1024*1024 && len > 0)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ok = TRUE;

	// The CUT_BUFFER0 is supposed to always contain latin1.  Convert from
	// 'enc' when it is a multi-byte encoding.  When 'enc' is an 8-bit
	// encoding conversion usually doesn't work, so keep the text as-is.
	ଯଦି (has_mbyte)
	{
	    vimconv_T	vc;

	    vc.vc_type = CONV_NONE;
	    ଯଦି (convert_setup(&vc, p_enc, (char_u *)"latin1") == OK)
	    {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା	intlen = len;
		char_u	*conv_str;

		vc.vc_fail = TRUE;
		conv_str = string_convert(&vc, str, &intlen);
		len = intlen;
		ଯଦି (conv_str != ଖାଲି)
		{
		    vim_free(str);
		    str = conv_str;
		}
		ଅନ୍ୟଥା
		{
		    ok = FALSE;
		}
		convert_setup(&vc, ଖାଲି, ଖାଲି);
	    }
	    ଅନ୍ୟଥା
	    {
		ok = FALSE;
	    }
	}

	// Do not store the string if conversion failed.  Better to use any
	// other selection than garbled text.
	ଯଦି (ok)
	{
	    XStoreBuffer(dpy, (ଅକ୍ଷର *)str, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)len, 0);
	    XFlush(dpy);
	}
    }

    vim_free(str);
}
# endif

    ଶୂନ୍ୟ
clip_free_selection(Clipboard_T *cbd)
{
    yankreg_T *y_ptr = get_y_current();

    ଯଦି (cbd == &clip_plus)
	set_y_current(get_y_register(PLUS_REGISTER));
    ଅନ୍ୟଥା
	set_y_current(get_y_register(STAR_REGISTER));
    free_yank_all();
    get_y_current()->y_size = 0;
    set_y_current(y_ptr);
}

/*
 * Get the selected text and put it in register '*' or '+'.
 */
    ଶୂନ୍ୟ
clip_get_selection(Clipboard_T *cbd)
{
    yankreg_T	*old_y_previous, *old_y_current;
    pos_T	old_cursor;
    pos_T	old_visual;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		old_visual_mode;
    colnr_T	old_virtcol;
    colnr_T	old_curswant;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		old_set_curswant;
    pos_T	old_op_start, old_op_end;
    oparg_T	oa;
    cmdarg_T	ca;

    ଯଦି (cbd->owned)
    {
	ଯଦି ((cbd == &clip_plus
		&& get_y_register(PLUS_REGISTER)->y_array != ଖାଲି)
		|| (cbd == &clip_star
		    && get_y_register(STAR_REGISTER)->y_array != ଖାଲି))
	    ଫେରନ୍ତୁ;

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
    ଅନ୍ୟଥା ଯଦି (!is_clipboard_needs_update())
    {
	clip_free_selection(cbd);

	// Try to get selected text from another window
	clip_gen_request_selection(cbd);
    }
}

/*
 * Convert from the GUI selection string into the '*'/'+' register.
 */
    ଶୂନ୍ୟ
clip_yank_selection(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		type,
    char_u	*str,
    long	len,
    Clipboard_T *cbd)
{
    yankreg_T *y_ptr;

    ଯଦି (cbd == &clip_plus)
	y_ptr = get_y_register(PLUS_REGISTER);
    ଅନ୍ୟଥା
	y_ptr = get_y_register(STAR_REGISTER);

    clip_free_selection(cbd);

    str_to_reg(y_ptr, type, str, len, -1, FALSE);
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
clip_convert_selection_offset(
	char_u	    **str,
	long_u	    *len,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    offset, // Extra space to add in *str and the offset to
			    // place the actual string in *str.
	Clipboard_T *cbd)
{
    char_u	*p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		lnum;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i, j;
    int_u	eolsize;
    yankreg_T	*y_ptr;

    ଯଦି (cbd == &clip_plus)
	y_ptr = get_y_register(PLUS_REGISTER);
    ଅନ୍ୟଥା
	y_ptr = get_y_register(STAR_REGISTER);

# ifdef USE_CRNL
    eolsize = 2;
# ଅନ୍ୟଥା
    eolsize = 1;
# endif

    *str = ଖାଲି;
    *len = 0;
    ଯଦି (y_ptr->y_array == ଖାଲି)
	ଫେରନ୍ତୁ -1;

    ପାଇଁ (i = 0; i < y_ptr->y_size; i++)
	*len += (long_u)y_ptr->y_array[i].length + eolsize;

    // Don't want newline character at end of last line if we're in MCHAR mode.
    ଯଦି (y_ptr->y_type == MCHAR && *len >= eolsize)
	*len -= eolsize;

    *len += offset;
    p = *str = alloc(*len + 1);	// add one to avoid zero
    ଯଦି (p == ଖାଲି)
	ଫେରନ୍ତୁ -1;
    p += offset;
    lnum = 0;
    ପାଇଁ (i = 0, j = 0; i < (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)*len - offset; i++, j++)
    {
	ଯଦି (y_ptr->y_array[lnum].string[j] == '\n')
	    p[i] = NUL;
	ଅନ୍ୟଥା ଯଦି (y_ptr->y_array[lnum].string[j] == NUL)
	{
# ifdef USE_CRNL
	    p[i++] = '\r';
# endif
	    p[i] = '\n';
	    lnum++;
	    j = -1;
	}
	ଅନ୍ୟଥା
	    p[i] = y_ptr->y_array[lnum].string[j];
    }
    ଫେରନ୍ତୁ y_ptr->y_type;
}

/*
 * Convert the '*'/'+' register into a GUI selection string returned in *str
 * with length *len.
 * Returns the motion type, or -1 for failure.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
clip_convert_selection(char_u **str, long_u *len, Clipboard_T *cbd)
{
    ଫେରନ୍ତୁ clip_convert_selection_offset(str, len, 0, cbd);
}

/*
 * When "regname" is a clipboard register, obtain the selection.  If it's not
 * available return zero, otherwise return "regname".
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
may_get_selection(ପୂର୍ଣ୍ଣସଂଖ୍ୟା regname)
{
    ଯଦି (regname == '*')
    {
	ଯଦି (!clip_star.available)
	    regname = 0;
	ଅନ୍ୟଥା
	    clip_get_selection(&clip_star);
    }
    ଅନ୍ୟଥା ଯଦି (regname == '+')
    {
	ଯଦି (!clip_plus.available)
	    regname = 0;
	ଅନ୍ୟଥା
	    clip_get_selection(&clip_plus);
    }
    ଫେରନ୍ତୁ regname;
}

/*
 * If we have written to a clipboard register, send the text to the clipboard.
 */
    ଶୂନ୍ୟ
may_set_selection(ଶୂନ୍ୟ)
{
    ଯଦି ((get_y_current() == get_y_register(STAR_REGISTER))
	    && clip_star.available)
    {
	clip_own_selection(&clip_star);
	clip_gen_set_selection(&clip_star);
    }
    ଅନ୍ୟଥା ଯଦି ((get_y_current() == get_y_register(PLUS_REGISTER))
	    && clip_plus.available)
    {
	clip_own_selection(&clip_plus);
	clip_gen_set_selection(&clip_plus);
    }
}

# ଯଦି defined(FEAT_WAYLAND_CLIPBOARD)

    ସ୍ଥିର clip_wl_selection_T *
clip_wl_get_selection(wayland_selection_T sel)
{
    ବାଛ (sel)
    {
	ସ୍ଥିତି WAYLAND_SELECTION_REGULAR:
	    ଫେରନ୍ତୁ &clip_wl.regular;
	ସ୍ଥିତି WAYLAND_SELECTION_PRIMARY:
	    ଫେରନ୍ତୁ &clip_wl.primary;
	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    ଫେରନ୍ତୁ ଖାଲି;
    }
}

    ସ୍ଥିର clip_wl_selection_T *
clip_wl_get_selection_from_cbd(Clipboard_T *cbd)
{
    ଯଦି (cbd == &clip_plus)
	ଫେରନ୍ତୁ &clip_wl.regular;
    ଅନ୍ୟଥା ଯଦି (cbd == &clip_star)
	ଫେରନ୍ତୁ &clip_wl.primary;
    ଅନ୍ୟଥା
	ଫେରନ୍ତୁ ଖାଲି;
}

    ସ୍ଥିର Clipboard_T *
clip_wl_get_cbd_from_selection(clip_wl_selection_T *sel)
{
    ଯଦି (sel == &clip_wl.regular)
	ଫେରନ୍ତୁ &clip_plus;
    ଅନ୍ୟଥା ଯଦି (sel == &clip_wl.primary)
	ଫେରନ୍ତୁ &clip_star;
    ଅନ୍ୟଥା
	ଫେରନ୍ତୁ ଖାଲି;
}

    ସ୍ଥିର wayland_selection_T
clip_wl_get_selection_type(clip_wl_selection_T *sel)
{
    ଯଦି (sel == &clip_wl.regular)
	ଫେରନ୍ତୁ WAYLAND_SELECTION_REGULAR;
    ଅନ୍ୟଥା ଯଦି (sel == &clip_wl.primary)
	ଫେରନ୍ତୁ WAYLAND_SELECTION_PRIMARY;
    ଅନ୍ୟଥା
	ଫେରନ୍ତୁ WAYLAND_SELECTION_NONE;
}

    ସ୍ଥିର bool
wl_data_offer_listener_event_offer(
    ଶୂନ୍ୟ *data UNUSED,
    vwl_data_offer_T *offer UNUSED,
    ଅଚଳ ଅକ୍ଷର *mime_type
)
{
    // Only accept mime type if we support it
    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ARRAY_LENGTH(supported_mimes); i++)
	ଯଦି (STRCMP(mime_type, supported_mimes[i]) == 0)
	    ଫେରନ୍ତୁ true;
    ଫେରନ୍ତୁ FALSE;
}

ସ୍ଥିର ଅଚଳ vwl_data_offer_listener_T vwl_data_offer_listener = {
    .offer = wl_data_offer_listener_event_offer
};

    ସ୍ଥିର ଶୂନ୍ୟ
vwl_data_device_listener_event_data_offer(
	ଶୂନ୍ୟ *data UNUSED,
	vwl_data_device_T *device UNUSED,
	vwl_data_offer_T *offer)
{
    // Immediately start listening for offer events from the data offer
    vwl_data_offer_add_listener(offer, &vwl_data_offer_listener, ଖାଲି);
}

    ସ୍ଥିର ଶୂନ୍ୟ
vwl_data_device_listener_event_selection(
	ଶୂନ୍ୟ *data UNUSED,
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
    ଯଦି (offer == ଖାଲି || sel->source != ଖାଲି)
    {
	// Selection event is from us, so we are the source client. Therefore
	// ignore it. Or the selection is cleared, so set sel->offer to NULL
	vwl_data_offer_destroy(offer);
	sel->offer = ଖାଲି;
	ଫେରନ୍ତୁ;
    }

    // Save offer. When we want to request data, then we'll actually call the
    // receive method.
    sel->offer = offer;

}

    ସ୍ଥିର ଶୂନ୍ୟ
vwl_data_device_listener_event_finished(
	ଶୂନ୍ୟ *data UNUSED,
	vwl_data_device_T *device)
{
    clip_wl_selection_T *sel;
    // Device finished, guessing this can happen is when the seat becomes
    // invalid? If so, let the user call :wlrestore! to reset. There wouldn't be
    // any point in trying to create another data device for the same seat,
    // since the seat is in an invalid state.
    ଯଦି (device == clip_wl.regular.device)
    {
	sel = &clip_wl.regular;
	clip_wl.regular.device = ଖାଲି;
    }
    ଅନ୍ୟଥା ଯଦି (device == clip_wl.primary.device)
    {
	sel = &clip_wl.primary;
	clip_wl.primary.device = ଖାଲି;
    }
    ଅନ୍ୟଥା
	// Shouldn't happen
	ଫେରନ୍ତୁ;

    vim_free(sel->contents);
    vwl_data_source_destroy(sel->source);
    vwl_data_offer_destroy(sel->offer);
    sel->available = FALSE;

    vwl_data_device_destroy(device);
}

ସ୍ଥିର ଅଚଳ vwl_data_device_listener_T vwl_data_device_listener = {
    .data_offer = vwl_data_device_listener_event_data_offer,
    .selection = vwl_data_device_listener_event_selection,
    .finished = vwl_data_device_listener_event_finished
};

/*
 * Initialize the clipboard for Wayland using the global Wayland connection.
 * Returns OK on success and FAIL on failure.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
clip_init_wayland(ଶୂନ୍ୟ)
{
    int_u supported = WAYLAND_SELECTION_NONE;

    ଯଦି (wayland_ct == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;

    memset(&clip_wl, 0, ଆକାର(clip_wl));

    clip_wl.seat = vwl_connection_get_seat(wayland_ct, (ଅକ୍ଷର *)p_wse);

    ଯଦି (clip_wl.seat == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;

    clip_wl.regular.manager = vwl_connection_get_data_device_manager(
	    wayland_ct, WAYLAND_SELECTION_REGULAR, &supported);

    ଯଦି (clip_wl.regular.manager != ଖାଲି)
    {
	clip_wl.regular.device = vwl_data_device_manager_get_data_device(
		clip_wl.regular.manager, clip_wl.seat);

	ଯଦି (clip_wl.regular.device != ଖାଲି)
	    clip_wl.regular.available = true;
	ଅନ୍ୟଥା
	{
	    // Shouldn't happen
	    vwl_data_device_manager_discard(clip_wl.regular.manager);
	    clip_wl.regular.manager = ଖାଲି;
	    ଫେରନ୍ତୁ FAIL;
	}
    }

    // If we still don't support the primary selection, try finding one for it
    // specifically.
    ଯଦି (!(supported & WAYLAND_SELECTION_PRIMARY))
    {
	clip_wl.primary.manager = vwl_connection_get_data_device_manager(
		wayland_ct, WAYLAND_SELECTION_PRIMARY, &supported);

	ଯଦି (clip_wl.primary.manager != ଖାଲି)
	{
	    clip_wl.primary.device = vwl_data_device_manager_get_data_device(
		    clip_wl.primary.manager, clip_wl.seat);

	    ଯଦି (clip_wl.primary.device != ଖାଲି)
		clip_wl.primary.available = true;
	    ଅନ୍ୟଥା
	    {
		vwl_data_device_manager_discard(clip_wl.primary.manager);
		clip_wl.primary.manager = ଖାଲି;
	    }
	}
    }

    ଯଦି (clip_wl.regular.available && !clip_wl.primary.available)
    {
	// The protocol supports both regular and primary selections, just use
	// one data device manager and one data device. Or the primary selection
	// is not supported, make it point to the regular selection instead.
	clip_wl.primary.available = true;
	clip_wl.primary.manager = clip_wl.regular.manager;
	clip_wl.primary.device = clip_wl.regular.device;
    }

    ଯଦି (!clip_wl.regular.available && !clip_wl.primary.available)
	ଫେରନ୍ତୁ FAIL;

    // Start listening for selection updates
    ଯଦି (clip_wl.regular.device != ଖାଲି)
	vwl_data_device_add_listener(clip_wl.regular.device,
		&vwl_data_device_listener, ଖାଲି);
    // Don't want to listen to the same data device twice
    ଯଦି (clip_wl.primary.device != ଖାଲି
	    && clip_wl.primary.device != clip_wl.regular.device)
	vwl_data_device_add_listener(clip_wl.primary.device,
		&vwl_data_device_listener, ଖାଲି);

    ଫେରନ୍ତୁ OK;
}

    ଶୂନ୍ୟ
clip_uninit_wayland(ଶୂନ୍ୟ)
{
    clip_wl_selection_T *sel;

    ଯଦି (clipmethod == CLIPMETHOD_WAYLAND)
    {
	ଯଦି (clip_star.owned)
	    clip_lose_selection(&clip_star);
	ଯଦି (clip_plus.owned)
	    clip_lose_selection(&clip_plus);
    }

    // Don't want to double free
    ଯଦି (clip_wl.regular.manager != clip_wl.primary.manager)
	vwl_data_device_manager_discard(clip_wl.primary.manager);
    vwl_data_device_manager_discard(clip_wl.regular.manager);

    ଯଦି (clip_wl.regular.device != clip_wl.primary.device)
	vwl_data_device_destroy(clip_wl.primary.device);
    vwl_data_device_destroy(clip_wl.regular.device);

    sel = &clip_wl.regular;
    ଯେତେବେଳେ (true)
    {
	vim_free(sel->contents);
	vwl_data_source_destroy(sel->source);
	vwl_data_offer_destroy(sel->offer);
	sel->available = false;

	ଯଦି (sel == &clip_wl.primary)
	    ଭାଙ୍ଗ;
	sel = &clip_wl.primary;
    }

    vim_memset(&clip_wl, 0, ଆକାର(clip_wl));
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
clip_reset_wayland(ଶୂନ୍ୟ)
{
    wayland_uninit_connection();

    ଯଦି (wayland_init_connection(wayland_display_name) == FAIL
	    || clip_init_wayland() == FAIL)
	ଫେରନ୍ତୁ FAIL;

    choose_clipmethod();
    ଫେରନ୍ତୁ OK;
}


/*
 * Read data from a file descriptor and write it to the given clipboard.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
clip_wl_receive_data(Clipboard_T *cbd, ଅଚଳ ଅକ୍ଷର *mime_type, ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd)
{
    char_u	*start, *final;
    long	len;
    garray_T	buf;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		motion_type = MAUTO;
    ssize_t	r = 0;
    char_u	*tofree = ଖାଲି;
#  ifndef HAVE_SELECT
    ସଂରଚନା pollfd   pfd;

    pfd.fd = fd;
    pfd.events = POLLIN;
#  ଅନ୍ୟଥା
    fd_set rfds;
    ସଂରଚନା timeval  tv;

    FD_ZERO(&rfds);
    FD_SET(fd, &rfds);
#  endif

    // Make pipe (read end) non-blocking
    ଯଦି (fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK) == -1)
	ଫେରନ୍ତୁ;

    ga_init2(&buf, 1, 4096);

    // 4096 bytes seems reasonable for initial buffer size, memory is cheap
    // anyways.
    ଯଦି (ga_grow(&buf, 4096) == FAIL)
	ଫେରନ୍ତୁ;

    start = buf.ga_data;

#  ifndef HAVE_SELECT
    ଯେତେବେଳେ (poll(&pfd, 1, p_wtm) > 0)
#  ଅନ୍ୟଥା
    ଯେତେବେଳେ (tv.tv_sec = p_wtm / 1000, tv.tv_usec = (p_wtm % 1000) * 1000,
	    select(fd + 1, &rfds, ଖାଲି, ଖାଲି, &tv) > 0)
#  endif
    {
	r = ପଠ(fd, start, buf.ga_maxlen - 1 - buf.ga_len);

	ଯଦି (r == 0)
	    ଭାଙ୍ଗ;
	ଅନ୍ୟଥା ଯଦି (r < 0)
	{
	    ଯଦି (errno == EAGAIN || errno == EINTR)
		ଜାରି;
	    ଭାଙ୍ଗ;
	}

	start += r;
	buf.ga_len += r;

	// Realloc if we are at the end of the buffer
	ଯଦି (buf.ga_len >= buf.ga_maxlen - 1)
	{
	    ଯଦି (ga_grow(&buf, 8192) == FAIL)
		ଭାଙ୍ଗ;
	    start = (char_u *)buf.ga_data + buf.ga_len;
	}
    }

    ଯଦି (buf.ga_len == 0)
    {
	clip_free_selection(cbd); // Nothing received, clear register
	ga_clear(&buf);
	ଫେରନ୍ତୁ;
    }

    final = buf.ga_data;
    len = buf.ga_len;

    ଯଦି (clip_convert_data(&final, &len, &motion_type,
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
    ସ୍ଥିର ଶୂନ୍ୟ
clip_wl_request_selection(Clipboard_T *cbd)
{
    clip_wl_selection_T *sel = clip_wl_get_selection_from_cbd(cbd);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			fds[2];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			mime_types_len;
    ଅଚଳ ଅକ୍ଷର		**mime_types;
    ଅଚଳ ଅକ୍ଷର		*chosen_mime = ଖାଲି;

    ଯଦି (!sel->available)
	ଯାଅ clear;

    // Dispatch any events that still queued up before checking for a data
    // offer.
    ଯଦି (vwl_connection_roundtrip(wayland_ct) == FAIL)
	ଯାଅ clear;

    ଯଦି (sel->offer == ଖାଲି)
	ଯାଅ clear;

    mime_types_len = sel->offer->mime_types.ga_len;
    mime_types = sel->offer->mime_types.ga_data;

    // Choose mime type to receive from. Mime types with a lower index in the
    // "supported_mimes" array are prioritized over ones after it.
    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ARRAY_LENGTH(supported_mimes)
	    && chosen_mime == ଖାଲି; i++)
    {
	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା k = 0; k < mime_types_len && chosen_mime == ଖାଲି; k++)
	    ଯଦି (STRCMP(mime_types[k], supported_mimes[i]) == 0)
		chosen_mime = supported_mimes[i];
    }

    ଯଦି (chosen_mime == ଖାଲି || pipe(fds) == -1)
	ଯାଅ clear;

    vwl_data_offer_receive(sel->offer, chosen_mime, fds[1]);

    close(fds[1]); // Close before we read data so that when the source client
		   // closes their end we receive an EOF.

    ଯଦି (vwl_connection_flush(wayland_ct) >= 0)
	clip_wl_receive_data(cbd, chosen_mime, fds[0]);

    close(fds[0]);

    ଫେରନ୍ତୁ;
clear:
    clip_free_selection(cbd);
}

    ସ୍ଥିର ଶୂନ୍ୟ
vwl_data_source_listener_event_send(
    ଶୂନ୍ୟ *data,
    vwl_data_source_T *source UNUSED,
    ଅଚଳ ଅକ୍ଷର *mime_type,
    int32_t fd
)
{
    clip_wl_selection_T *sel = data;
    Clipboard_T		*cbd = clip_wl_get_cbd_from_selection(sel);
    bool		have_mime = false;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			motion_type;
    long_u		length;
    char_u		*string; // Will be reallocated to a bigger size if
				 // needed.
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			offset = 0;
    bool		is_vim, is_vimenc;
    size_t		total = 0;
#  ifndef HAVE_SELECT
    ସଂରଚନା pollfd   pfd;

    pfd.fd = fd;
    pfd.events = POLLOUT;
#  ଅନ୍ୟଥା
    fd_set	    wfds;
    ସଂରଚନା timeval  tv;

    FD_ZERO(&wfds);
    FD_SET(fd, &wfds);
#  endif

    // Check if we actually have mime type
    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ARRAY_LENGTH(supported_mimes); i++)
	ଯଦି (STRCMP(supported_mimes[i], mime_type) == 0)
	{
	    have_mime = true;
	    ଭାଙ୍ଗ;
	}

    ଯଦି (!have_mime)
	ଯାଅ exit;

    // First byte sent is motion type for vim specific formats. For the vimenc
    // format, after the first byte is the encoding type, which is null
    // terminated.

    is_vimenc = STRCMP(mime_type, VIMENC_ATOM_NAME) == 0
	|| STRCMP(mime_type, VIMENC_MIMETYPE_NAME) == 0;
    is_vim = STRCMP(mime_type, VIM_ATOM_NAME) == 0
	|| STRCMP(mime_type, VIM_MIMETYPE_NAME) == 0;

    ଯଦି (is_vimenc)
	offset += 2 + STRLEN(p_enc);
    ଅନ୍ୟଥା ଯଦି (is_vim)
	offset += 1;

    clip_get_selection(cbd);
    motion_type = clip_convert_selection_offset(&string, &length, offset, cbd);

    ଯଦି (motion_type < 0)
	ଯାଅ exit;

    ଯଦି (is_vimenc)
    {
	string[0] = (char_u)motion_type;
	// Use vim_strncpy for safer copying
	vim_strncpy(string + 1, p_enc, STRLEN(p_enc));
    }
    ଅନ୍ୟଥା ଯଦି (is_vim)
	string[0] = (char_u)motion_type;


    ଯେତେବେଳେ (total < (size_t)length &&
#  ifndef HAVE_SELECT
	    poll(&pfd, 1, p_wtm) > 0)
#  ଅନ୍ୟଥା
	    ((tv.tv_sec = p_wtm / 1000, tv.tv_usec = (p_wtm % 1000) * 1000),
	    select(fd + 1, ଖାଲି, &wfds, ଖାଲି, &tv) > 0))
#  endif
    {
	ssize_t w = ଲିଖ(fd, string + total, length - total);

	ଯଦି (w == -1)
	    ଭାଙ୍ଗ;
	total += w;
    }

    vim_free(string);
exit:
    close(fd);
}

    ସ୍ଥିର ଶୂନ୍ୟ
vwl_data_source_listener_event_cancelled(
	ଶୂନ୍ୟ *data,
	vwl_data_source_T *source UNUSED)
{
    clip_wl_selection_T *sel = data;
    Clipboard_T		*cbd = clip_wl_get_cbd_from_selection(sel);

    clip_lose_selection(cbd);
}

ସ୍ଥିର ଅଚଳ vwl_data_source_listener_T vwl_data_source_listener = {
    .send = vwl_data_source_listener_event_send,
    .cancelled = vwl_data_source_listener_event_cancelled
};

    ସ୍ଥିର ଶୂନ୍ୟ
clip_wl_do_set_selection(ଶୂନ୍ୟ *data, uint32_t serial)
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
clip_wl_own_selection(Clipboard_T *cbd)
{
    clip_wl_selection_T *sel = clip_wl_get_selection_from_cbd(cbd);
    wayland_selection_T sel_type = clip_wl_get_selection_type(sel);

    ଯଦି (!sel->available || vwl_connection_roundtrip(wayland_ct) == FAIL)
	ଫେରନ୍ତୁ FAIL;

    ଯଦି (sel->source != ଖାଲି)
    {
	ଯଦି (sel_type == WAYLAND_SELECTION_PRIMARY)
	    // We already own the selection, ignore (only do this for primary
	    // selection). We don't re set the selection because then we would
	    // be setting the selection every time the user moves the visual
	    // selection cursor, which is messy and inefficient. Some
	    // applications like Google Chrome do it this way however.
	    ଫେରନ୍ତୁ OK;
	ଅନ୍ୟଥା ଯଦି (sel_type == WAYLAND_SELECTION_REGULAR)
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
	ଅନ୍ୟଥା
	    // Shouldn't happen
	    ଫେରନ୍ତୁ FAIL;
    }

    sel->source = vwl_data_device_manager_create_data_source(sel->manager);
    vwl_data_source_add_listener(sel->source, &vwl_data_source_listener, sel);

    // Advertise mime types
    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ARRAY_LENGTH(supported_mimes); i++)
	vwl_data_source_offer(sel->source, supported_mimes[i]);

    sel->own_success = false;
    clip_wl_do_set_selection(sel, 0);

    ଯଦି (!sel->own_success)
	ଯାଅ fail;

    ଫେରନ୍ତୁ OK;
fail:
    vwl_data_source_destroy(sel->source);
    sel->source = ଖାଲି;
    ଫେରନ୍ତୁ FAIL;
}

/*
 * Disown the selection that cbd corresponds to.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
clip_wl_lose_selection(Clipboard_T *cbd)
{
    clip_wl_selection_T *sel = clip_wl_get_selection_from_cbd(cbd);

    ଯଦି (!sel->available)
	ଫେରନ୍ତୁ;

    vwl_data_source_destroy(sel->source);
    sel->source = ଖାଲି;
}

/*
 * Send the current selection to the clipboard. Do nothing for Wayland because
 * we will fill in the selection only when requested by another client.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
clip_wl_set_selection(Clipboard_T *cbd UNUSED)
{
}

#  ଯଦି defined(USE_SYSTEM)
/*
 * Return true if we own the selection corresponding to cbd or another client
 * does.
 */
    ସ୍ଥିର bool
clip_wl_owner_exists(Clipboard_T *cbd)
{
    clip_wl_selection_T *sel = clip_wl_get_selection_from_cbd(cbd);

    ଯଦି (vwl_connection_roundtrip(wayland_ct) == FAIL)
	ଫେରନ୍ତୁ false;

    ଫେରନ୍ତୁ sel->available && (sel->source != ଖାଲି || sel->offer != ଖାଲି);
}
#  endif

# endif // FEAT_WAYLAND_CLIPBOARD

#endif // FEAT_CLIPBOARD

#ifdef HAVE_CLIPMETHOD

/*
 * Returns the first method for accessing the clipboard that is available/works,
 * depending on the order of values in str.
 */
    ସ୍ଥିର clipmethod_T
get_clipmethod(char_u *str)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len	= (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(str) + 1;
    char_u	*buf	= alloc(len);

    ଯଦି (buf == ଖାଲି)
	ଫେରନ୍ତୁ CLIPMETHOD_FAIL;

    clipmethod_T ret = CLIPMETHOD_FAIL;
    char_u	*p = str;

    ଯେତେବେଳେ (*p != NUL)
    {
	clipmethod_T method = CLIPMETHOD_NONE;

	(ଶୂନ୍ୟ)copy_option_part(&p, buf, len, ",");

	ଯଦି (STRCMP(buf, "wayland") == 0)
	{
# ifdef FEAT_GUI
	    ଯଦି (!gui.in_use)
# endif
	    {
# ifdef FEAT_WAYLAND_CLIPBOARD
		ଯଦି (clip_wl.regular.available || clip_wl.primary.available)
		    method = CLIPMETHOD_WAYLAND;
# endif
	    }
	}
	ଅନ୍ୟଥା ଯଦି (STRCMP(buf, "x11") == 0)
	{
# ifdef FEAT_GUI
	    ଯଦି (!gui.in_use)
# endif
	    {
# ifdef FEAT_XCLIPBOARD
		// x_IOerror_handler() in os_unix.c should set xterm_dpy to NULL if
		// we lost connection to the X server.
		ଯଦି (xterm_dpy != ଖାଲି)
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
	ଅନ୍ୟଥା
	{
# ifdef FEAT_CLIPBOARD_PROVIDER
	    // Check if name matches a clipboard provider
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା r = clip_provider_is_available(buf);

	    ଯଦି (r == 1)
	    {
		method = CLIPMETHOD_PROVIDER;
		ଯଦି (ret == CLIPMETHOD_FAIL)
		{
		    vim_free(clip_provider);
		    clip_provider = vim_strsave(buf);
		    ଯଦି (clip_provider == ଖାଲି)
			ଯାଅ fail;
		}
	    }
	    ଅନ୍ୟଥା ଯଦି (r == -1)
# endif
	    {
# ifdef FEAT_CLIPBOARD_PROVIDER
fail:
# endif
		ret = CLIPMETHOD_FAIL;
		ଯାଅ exit;
	    }
	}

	// Keep on going in order to catch errors
	ଯଦି (method != CLIPMETHOD_NONE && ret == CLIPMETHOD_FAIL)
	    ret = method;
    }

    // No match found, use "none".
    ret = (ret == CLIPMETHOD_FAIL) ? CLIPMETHOD_NONE : ret;

exit:
    vim_free(buf);
    ଫେରନ୍ତୁ ret;
}


/*
 * Returns name of clipmethod in a statically allocated string.
 */
    ସ୍ଥିର char_u *
clipmethod_to_str(clipmethod_T method)
{
    ବାଛ(method)
    {
	ସ୍ଥିତି CLIPMETHOD_WAYLAND:
	    ଫେରନ୍ତୁ (char_u *)"wayland";
	ସ୍ଥିତି CLIPMETHOD_X11:
	    ଫେରନ୍ତୁ (char_u *)"x11";
	ସ୍ଥିତି CLIPMETHOD_PROVIDER:
# ifdef FEAT_EVAL
	    ଫେରନ୍ତୁ clip_provider;
# endif
	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    ଫେରନ୍ତୁ (char_u *)"none";
    }
}

/*
 * Sets the current clipmethod to use given by `get_clipmethod()`. Returns an
 * error message on failure else NULL.
 */
    ଅକ୍ଷର *
choose_clipmethod(ଶୂନ୍ୟ)
{
    clipmethod_T method = get_clipmethod(p_cpm);

    ଯଦି (method == CLIPMETHOD_FAIL)
	ଫେରନ୍ତୁ e_invalid_argument;

// If GUI is running or we are not on a system with Wayland or X11, then always
// return CLIPMETHOD_NONE. System or GUI clipboard handling always overrides.
// This is unless a provider is being used.
# ଯଦି defined(FEAT_XCLIPBOARD) || defined(FEAT_WAYLAND_CLIPBOARD)
#  ଯଦି defined(FEAT_GUI)
    ଯଦି (method != CLIPMETHOD_PROVIDER && gui.in_use)
    {
#   ifdef FEAT_WAYLAND
	// We only interact with Wayland for the clipboard, we can just deinit
	// everything.
	wayland_uninit_connection();
#   endif

	method = CLIPMETHOD_NONE;
	ଯାଅ lose_sel_exit;
    }
#  endif
# ଅନ୍ୟଥା
    // If on a system like windows or macos, then clipmethod is irrelevant, we
    // use their way of accessing the clipboard. This is unless we are using the
    // clipboard provider
#  ifdef FEAT_CLIPBOARD_PROVIDER
    ଯଦି (method != CLIPMETHOD_PROVIDER)
#  endif
    {
	method = CLIPMETHOD_NONE;
	ଯାଅ exit;
    }
# endif

# ifdef FEAT_CLIPBOARD
    // Deinitialize clipboard if there is no way to access clipboard
    ଯଦି (method == CLIPMETHOD_NONE)
	clip_init(FALSE);
    // If we have a clipmethod that works now, then initialize clipboard
    ଅନ୍ୟଥା ଯଦି (clipmethod == CLIPMETHOD_NONE && method != CLIPMETHOD_NONE)
    {
	clip_init(TRUE);
	did_warn_clipboard = false;
    }
    // Disown clipboard if we are switching to a new method
    ଅନ୍ୟଥା ଯଦି (clipmethod != CLIPMETHOD_NONE && method != clipmethod)
    {
#  ଯଦି (defined(FEAT_XCLIPBOARD) || defined(FEAT_WAYLAND_CLIPBOARD)) \
	&& defined(FEAT_GUI)
lose_sel_exit:
#  endif
	ଯଦି (clip_star.owned)
	    clip_lose_selection(&clip_star);
	ଯଦି (clip_plus.owned)
	    clip_lose_selection(&clip_plus);

#  ଯଦି defined(FEAT_GUI)
	ଯଦି (!gui.in_use)
#  endif
	{
	    clip_init(TRUE);
	    did_warn_clipboard = false;
	}
    }
# endif // FEAT_CLIPBOARD

# ଯଦି !defined(FEAT_XCLIPBOARD) && !defined(FEAT_WAYLAND_CLIPBOARD)
exit:
# endif

    clipmethod = method;

# ifdef FEAT_EVAL
    set_vim_var_string(VV_CLIPMETHOD, (char_u*)clipmethod_to_str(method), -1);
# endif

    ଫେରନ୍ତୁ ଖାଲି;
}

/*
 * Call choose_clipmethod().
 */
    ଶୂନ୍ୟ
ex_clipreset(exarg_T *eap UNUSED)
{
    clipmethod_T prev = clipmethod;

    choose_clipmethod();

    ଯଦି (clipmethod == CLIPMETHOD_NONE)
	smsg(_("Could not find a way to access the clipboard."));
    ଅନ୍ୟଥା ଯଦି (clipmethod != prev)
	smsg(_("Switched to clipboard method '%s'."),
		clipmethod_to_str(clipmethod));
}

/*
 * Adjust the register name pointed to with "rp" for the clipboard being
 * used always and the clipboard being available.
 */
    ଶୂନ୍ୟ
adjust_clip_reg(ପୂର୍ଣ୍ଣସଂଖ୍ୟା *rp)
{
# ifdef FEAT_CLIPBOARD_PROVIDER
    ଯଦି (clipmethod == CLIPMETHOD_PROVIDER)
    {
	ଯଦି (*rp == 0 && clip_unnamed != 0)
	    *rp = ((clip_unnamed & CLIP_UNNAMED_PLUS)) ? '+' : '*';
	ଫେରନ୍ତୁ;
    }
# endif
# ifdef FEAT_CLIPBOARD
    // If no reg. specified, and "unnamed" or "unnamedplus" is in 'clipboard',
    // use '*' or '+' reg, respectively. "unnamedplus" prevails.
    ଯଦି (*rp == 0 && (clip_unnamed != 0 || clip_unnamed_saved != 0))
    {
	ଯଦି (clip_unnamed != 0)
	    *rp = ((clip_unnamed & CLIP_UNNAMED_PLUS) && clip_plus.available)
								  ? '+' : '*';
	ଅନ୍ୟଥା
	    *rp = ((clip_unnamed_saved & CLIP_UNNAMED_PLUS)
					   && clip_plus.available) ? '+' : '*';
    }
    ଯଦି ((!clip_star.available && *rp == '*') ||
	   (!clip_plus.available && *rp == '+'))
    {
	ସନ୍ଦେଶ_warn_missing_clipboard();
	*rp = 0;
    }
# endif
}

/*
 * Extract the items in the 'clipboard' option and set global values.
 * Return an error message or NULL for success.
 */
    ଅକ୍ଷର *
did_set_clipboard(optset_T *args UNUSED)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		new_unnamed = 0;
# ifdef FEAT_CLIPBOARD
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		new_autoselect_star = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		new_autoselect_plus = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		new_autoselectml = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		new_html = FALSE;
# endif
    regprog_T	*new_exclude_prog = ଖାଲି;
    ଅକ୍ଷର	*errmsg = ଖାଲି;
    char_u	*p;

    ପାଇଁ (p = p_cb; *p != NUL; )
    {
	// Note: Keep this in sync with p_cb_values.
	ଯଦି (STRNCMP(p, "unnamed", 7) == 0 && (p[7] == ',' || p[7] == NUL))
	{
	    new_unnamed |= CLIP_UNNAMED;
	    p += 7;
	}
	ଅନ୍ୟଥା ଯଦି (STRNCMP(p, "unnamedplus", 11) == 0
					    && (p[11] == ',' || p[11] == NUL))
	{
	    new_unnamed |= CLIP_UNNAMED_PLUS;
	    p += 11;
	}
# ifdef FEAT_CLIPBOARD
	ଅନ୍ୟଥା ଯଦି (STRNCMP(p, "autoselect", 10) == 0
					    && (p[10] == ',' || p[10] == NUL))
	{
	    new_autoselect_star = TRUE;
	    p += 10;
	}
	ଅନ୍ୟଥା ଯଦି (STRNCMP(p, "autoselectplus", 14) == 0
					    && (p[14] == ',' || p[14] == NUL))
	{
	    new_autoselect_plus = TRUE;
	    p += 14;
	}
	ଅନ୍ୟଥା ଯଦି (STRNCMP(p, "autoselectml", 12) == 0
					    && (p[12] == ',' || p[12] == NUL))
	{
	    new_autoselectml = TRUE;
	    p += 12;
	}
	ଅନ୍ୟଥା ଯଦି (STRNCMP(p, "html", 4) == 0 && (p[4] == ',' || p[4] == NUL))
	{
	    new_html = TRUE;
	    p += 4;
	}
	ଅନ୍ୟଥା ଯଦି (STRNCMP(p, "exclude:", 8) == 0 && new_exclude_prog == ଖାଲି)
	{
	    p += 8;
	    new_exclude_prog = vim_regcomp(p, RE_MAGIC);
	    ଯଦି (new_exclude_prog == ଖାଲି)
		errmsg = e_invalid_argument;
	    ଭାଙ୍ଗ;
	}
# endif
	ଅନ୍ୟଥା
	{
	    errmsg = e_invalid_argument;
	    ଭାଙ୍ଗ;
	}
	ଯଦି (*p == ',')
	    ++p;
    }
    ଯଦି (errmsg == ଖାଲି)
    {
# ifdef FEAT_CLIPBOARD
	ଯଦି (global_busy)
	    // clip_unnamed will be reset to clip_unnamed_saved
	    // at end_global_changes
	    clip_unnamed_saved = new_unnamed;
	ଅନ୍ୟଥା
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
# ଯଦି defined(FEAT_GUI_GTK)
	ଯଦି (gui.in_use)
	{
#  ifdef USE_GTK4
	    gui_gtk_update_selection_formats(&clip_plus);
	    gui_gtk_update_selection_formats(&clip_star);
#  ଅନ୍ୟଥା
	    gui_gtk_set_selection_targets((GdkAtom)GDK_SELECTION_PRIMARY);
	    gui_gtk_set_selection_targets((GdkAtom)clip_plus.gtk_sel_atom);
#  endif
#  ifdef FEAT_DND
	    gui_gtk_set_dnd_targets();
#  endif
	}
# endif
    }
    ଅନ୍ୟଥା
	vim_regfree(new_exclude_prog);

    ଫେରନ୍ତୁ errmsg;
}

#endif // HAVE_CLIPMETHOD

#ifdef FEAT_CLIPBOARD_PROVIDER

/*
 * Check if a clipboard provider with given name is available. Returns 1 if available,
 * 0 if not available, and -1 on error
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
clip_provider_is_available(char_u *provider)
{
    dict_T	*providers = get_vim_var_dict(VV_CLIPPROVIDERS);
    typval_T	provider_tv = {0};
    callback_T	callback = {0};
    typval_T	rettv = {0};
    typval_T	func_tv = {0};
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		res = 0;

    ଯଦି (dict_get_tv(providers, (ଅକ୍ଷର *)provider, &provider_tv) == FAIL
	    || provider_tv.v_type != VAR_DICT)
	// clipboard provider not defined
	ଫେରନ୍ତୁ -1;

    ଯଦି (dict_get_tv(provider_tv.vval.v_dict, "available", &func_tv) == FAIL)
    {
	clear_tv(&provider_tv);
	// If "available" function not specified assume always TRUE
	ଫେରନ୍ତୁ 1;
    }

    ଯଦି ((callback = get_callback(&func_tv)).cb_name == ଖାଲି)
	ଯାଅ fail;

    ଯଦି (call_callback(&callback, -1, &rettv, 0, ଖାଲି) == FAIL ||
	    (rettv.v_type != VAR_BOOL && rettv.v_type != VAR_ସଂଖ୍ୟା))
	ଯାଅ fail;

    ଯଦି (rettv.vval.v_ସଂଖ୍ୟା)
	res = 1;

    ଯଦି (FALSE)
fail:
	res = -1;

    free_callback(&callback);
    clear_tv(&func_tv);
    clear_tv(&rettv);
    clear_tv(&provider_tv);

    ଫେରନ୍ତୁ res;
}

/*
 * Get the specified callback "function" from the provider dictionary for
 * register "reg".
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
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

    ଯଦି (dict_get_tv(providers, (ଅକ୍ଷର *)provider, &provider_tv) == FAIL)
	ଫେରନ୍ତୁ FAIL;
    ଅନ୍ୟଥା ଯଦି (provider_tv.v_type != VAR_DICT)
    {
	clear_tv(&provider_tv);
	ଫେରନ୍ତୁ FAIL;
    }
    ଅନ୍ୟଥା ଯଦି (dict_get_tv(
		provider_tv.vval.v_dict,
		(ଅକ୍ଷର *)function,
		&action_tv) == FAIL)
    {
	clear_tv(&provider_tv);
	ଫେରନ୍ତୁ FAIL;
    }
    ଅନ୍ୟଥା ଯଦି (action_tv.v_type != VAR_DICT)
    {
	clear_tv(&provider_tv);
	clear_tv(&action_tv);
	ଫେରନ୍ତୁ FAIL;
    }
    ଅନ୍ୟଥା ଯଦି (dict_get_tv(action_tv.vval.v_dict, (ଅକ୍ଷର *)reg, &func_tv) == FAIL)
    {
	clear_tv(&provider_tv);
	clear_tv(&action_tv);
	ଫେରନ୍ତୁ FAIL;
    }
    ଅନ୍ୟଥା ଯଦି ((cb = get_callback(&func_tv)).cb_name == ଖାଲି)
    {
	clear_tv(&provider_tv);
	clear_tv(&action_tv);
	clear_tv(&func_tv);
	ଫେରନ୍ତୁ FAIL;
    }
    clear_tv(&provider_tv);
    clear_tv(&action_tv);

    // func_tv owns the function name, so we must make a copy for the callback
    set_callback(callback, &cb);
    clear_tv(&func_tv);
    ଫେରନ୍ତୁ OK;
}

    ସ୍ଥିର ଶୂନ୍ୟ
clip_provider_copy(char_u *reg, char_u *provider)
{
    ସ୍ଥିର bool	recursive = false;
    callback_T	callback;
    typval_T	rettv;
    typval_T	argvars[4];
    yankreg_T	*y_ptr;
    char_u	type[2 + NUMBUFLEN] = {0};
    list_T	*list = ଖାଲି;

    ଯଦି (recursive)
	ଫେରନ୍ତୁ;

    ଯଦି (clip_provider_get_callback(
		reg,
		provider,
		(char_u *)"copy",
		&callback) == FAIL)
	ଫେରନ୍ତୁ;

    // Convert register type into a string
    ଯଦି (*reg == '+')
	y_ptr = get_y_register(REAL_PLUS_REGISTER);
    ଅନ୍ୟଥା
	y_ptr = get_y_register(STAR_REGISTER);

    ବାଛ (y_ptr->y_type)
    {
	ସ୍ଥିତି MCHAR:
	    type[0] = 'v';
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି MLINE:
	    type[0] = 'V';
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି MBLOCK:
	    sprintf((ଅକ୍ଷର *)type, "%c%d", Ctrl_V, y_ptr->y_width + 1);
	    ଭାଙ୍ଗ;
	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    type[0] = 0;
	    ଭାଙ୍ଗ;
    }

    argvars[0].v_type = VAR_STRING;
    argvars[0].vval.v_string = reg;

    argvars[1].v_type = VAR_STRING;
    argvars[1].vval.v_string = type;

    // Get register contents by creating a list of lines
    list = list_alloc();

    ଯଦି (list == ଖାଲି)
    {
	free_callback(&callback);
	ଫେରନ୍ତୁ;
    }

    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < y_ptr->y_size; i++)
	ଯଦି (list_append_string(list, y_ptr->y_array[i].string,
	    (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)y_ptr->y_array[i].length) == FAIL)
	{
	    free_callback(&callback);
	    list_unref(list);
	    ଫେରନ୍ତୁ;
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

    ସ୍ଥିର ଶୂନ୍ୟ
clip_provider_paste(char_u *reg, char_u *provider)
{
    ସ୍ଥିର bool	recursive = false;
    callback_T	callback;
    typval_T	argvars[2];
    typval_T	rettv;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ret;
    char_u	*reg_type;
    list_T	*ଧାଡ଼ିଗୁଡ଼ିକ;

    ଯଦି (recursive)
	ଫେରନ୍ତୁ;

    ଯଦି (clip_provider_get_callback(
		reg,
		provider,
		(char_u *)"paste",
		&callback) == FAIL)
	ଫେରନ୍ତୁ;

    argvars[0].v_type = VAR_STRING;
    argvars[0].vval.v_string = reg;

    argvars[1].v_type = VAR_UNKNOWN;

    textlock++;
    recursive = true;
    ret = call_callback(&callback, -1, &rettv, 1, argvars);
    recursive = false;
    textlock--;

    ଯଦି (ret == FAIL)
	ଯାଅ exit;
    ଅନ୍ୟଥା ଯଦି (rettv.v_type == VAR_TUPLE
	    && TUPLE_LEN(rettv.vval.v_tuple) == 2
	    && TUPLE_ITEM(rettv.vval.v_tuple, 0)->v_type == VAR_STRING
	    && TUPLE_ITEM(rettv.vval.v_tuple, 1)->v_type == VAR_LIST)
    {
	reg_type = TUPLE_ITEM(rettv.vval.v_tuple, 0)->vval.v_string;
	ଧାଡ଼ିଗୁଡ଼ିକ = TUPLE_ITEM(rettv.vval.v_tuple, 1)->vval.v_list;
    }
    ଅନ୍ୟଥା ଯଦି (rettv.v_type == VAR_LIST
	    && rettv.vval.v_list->lv_len == 2
	    && rettv.vval.v_list->lv_first->li_tv.v_type == VAR_STRING
	    && rettv.vval.v_list->lv_first->li_next->li_tv.v_type == VAR_LIST)
    {
	reg_type = rettv.vval.v_list->lv_first->li_tv.vval.v_string;
	ଧାଡ଼ିଗୁଡ଼ିକ = rettv.vval.v_list->lv_first->li_next->li_tv.vval.v_list;
    }
    ଅନ୍ୟଥା
	ଯାଅ exit;

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
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len;

	// If the list is NULL handle like an empty list.
	len = ଧାଡ଼ିଗୁଡ଼ିକ == ଖାଲି ? 0 : ଧାଡ଼ିଗୁଡ଼ିକ->lv_len;

	// First half: use for pointers to result lines; second half: use for
	// pointers to allocated copies.
	lstval = ALLOC_MULT(char_u *, (len + 1) * 2);
	ଯଦି (lstval == ଖାଲି)
	    ଯାଅ exit;
	curval = lstval;
	allocval = lstval + len + 2;
	curallocval = allocval;

	ଯଦି (ଧାଡ଼ିଗୁଡ଼ିକ != ଖାଲି)
	{
	    CHECK_LIST_MATERIALIZE(ଧାଡ଼ିଗୁଡ଼ିକ);
	    FOR_ALL_LIST_ବସ୍ତୁଗୁଡ଼ିକ(ଧାଡ଼ିଗୁଡ଼ିକ, li)
	    {
		strval = tv_get_string_buf_chk(&li->li_tv, buf);
		ଯଦି (strval == ଖାଲି)
		    ଯାଅ free_lstval;
		ଯଦି (strval == buf)
		{
		    // Need to make a copy, next tv_get_string_buf_chk() will
		    // overwrite the string.
		    strval = vim_strsave(buf);
		    ଯଦି (strval == ଖାଲି)
			ଯାଅ free_lstval;
		    *curallocval++ = strval;
		}
		*curval++ = strval;
	    }
	}
	*curval++ = ଖାଲି;

	ଯଦି (*reg_type != NUL && (STRLEN(reg_type) == 0
		|| get_yank_type(&reg_type, &yank_type, &block_len) == FAIL))
	{
	    emsg(e_invalid_argument);
	    ଯାଅ free_lstval;
	}

	ଯଦି (*reg == '+')
	    y_ptr = get_y_register(REAL_PLUS_REGISTER);
	ଅନ୍ୟଥା
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
	ଯେତେବେଳେ (curallocval > allocval)
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
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା star_pause_ଗଣନା = -2, plus_pause_ଗଣନା = -2;

    ଶୂନ୍ୟ
call_clip_provider_request(ପୂର୍ଣ୍ଣସଂଖ୍ୟା reg)
{
    ଯଦି (clipmethod != CLIPMETHOD_PROVIDER)
	ଫେରନ୍ତୁ;

    ଯଦି (reg == '+' && plus_pause_ଗଣନା < 0)
    {
	ଯଦି (plus_pause_ଗଣନା == -1)
	    plus_pause_ଗଣନା = 1;
	clip_provider_paste((char_u *)"+", clip_provider);
    }
    ଅନ୍ୟଥା ଯଦି (reg == '*' && star_pause_ଗଣନା < 0)
    {
	ଯଦି (star_pause_ଗଣନା == -1)
	    star_pause_ଗଣନା = 1;
	clip_provider_paste((char_u *)"*", clip_provider);
    }
    ଅନ୍ୟଥା
	ଫେରନ୍ତୁ;
}

    ଶୂନ୍ୟ
call_clip_provider_set(ପୂର୍ଣ୍ଣସଂଖ୍ୟା reg)
{
    ଯଦି (clipmethod != CLIPMETHOD_PROVIDER)
	ଫେରନ୍ତୁ;

    ଯଦି (reg == '+' && plus_pause_ଗଣନା < 0)
    {
	ଯଦି (plus_pause_ଗଣନା == -1)
	    plus_pause_ଗଣନା = 1;
	clip_provider_copy((char_u *)"+", clip_provider);
    }
    ଅନ୍ୟଥା ଯଦି (reg == '*' && star_pause_ଗଣନା < 0)
    {
	ଯଦି (star_pause_ଗଣନା == -1)
	    star_pause_ଗଣନା = 1;
	clip_provider_copy((char_u *)"*", clip_provider);
    }
}

/*
 * Makes it so that the next provider call is only done once any calls after are
 * ignored, until dec_clip_provider is called the same number of times after
 * again. Note that this is per clipboard register ("+", "*")
 */
    ଶୂନ୍ୟ
inc_clip_provider(ଶୂନ୍ୟ)
{
    plus_pause_ଗଣନା = (plus_pause_ଗଣନା == -2
	|| plus_pause_ଗଣନା == -1) ? -1 : plus_pause_ଗଣନା + 1;
    star_pause_ଗଣନା = (star_pause_ଗଣନା == -2
	|| star_pause_ଗଣନା == -1) ? -1 : star_pause_ଗଣନା + 1;
}

    ଶୂନ୍ୟ
dec_clip_provider(ଶୂନ୍ୟ)
{
    ଯଦି (plus_pause_ଗଣନା != -2)
	plus_pause_ଗଣନା = plus_pause_ଗଣନା == -1 ? -1 : plus_pause_ଗଣନା - 1;
    ଯଦି (star_pause_ଗଣନା != -2)
	star_pause_ଗଣନା = star_pause_ଗଣନା == -1 ? -1 : star_pause_ଗଣନା - 1;

    ଯଦି (plus_pause_ଗଣନା == 0 || plus_pause_ଗଣନା == -1)
	plus_pause_ଗଣନା = -2;
    ଯଦି (star_pause_ଗଣନା == 0 || star_pause_ଗଣନା == -1)
	star_pause_ଗଣନା = -2;
}

#endif // FEAT_CLIPBOARD_PROVIDER

#ଯଦି defined(FEAT_WAYLAND_CLIPBOARD) || (defined(FEAT_GUI_GTK) && defined(USE_GTK4))
/*
 * If "vim" is TRUE, then get the motion type. If "vimenc" is TRUE, then get the
 * motion type and also convert "*buf". "buf" and "len_store" will be updated to
 * reflect the actual contents, but should be set beforehand with the initial
 * contents. Note that if "*tofree" is not NULL, use vim_free() on "*buf",
 * otherwise use the free func for which "*buf" was allocated with. In both
 * cases use vim_free() on "*tofree". Returns OK on success and FAIL on failure.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
clip_convert_data(
	char_u	**buf,
	long	*len_store,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	*motion,
	bool	vim,
	bool	vimenc,
	char_u	**tofree)
{
    char_u  *final = *buf;
    char_u  *enc;
    long    len = *len_store;

    ଯଦି (vim && len >= 2)
    {
	*motion = *final++;
	len--;
    }
    ଅନ୍ୟଥା ଯଦି (vimenc && len >= 3)
    {
	vimconv_T   conv;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    convlen;

	// First byte is motion type
	*motion = *final++;
	len--;

	// Get encoding of selection
	enc = final;

	// Skip the encoding type including null terminator in final text
	final = memchr(final, NUL, len);
	ଯଦି (final == ଖାଲି)
	    ଫେରନ୍ତୁ FAIL;
	final++; // Skip NUL

	// Subtract pointers to get length of encoding;
	len -= final - enc;

	conv.vc_type = CONV_NONE;
	convert_setup(&conv, enc, p_enc);
	ଯଦି (conv.vc_type != CONV_NONE)
	{
	   char_u *tmp;

	   convlen = len;
	   tmp = string_convert(&conv, final, &convlen);
	   len = convlen;
	   ଯଦି (tmp != ଖାଲି)
	   {
		final = tmp;
		*tofree = final;
	   }
	   convert_setup(&conv, ଖାଲି, ଖାଲି);
	}
    }
    *buf = final;
    *len_store = len;
    ଫେରନ୍ତୁ OK;
}
#endif
