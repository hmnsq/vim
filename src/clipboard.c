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

#অন্তর্ভাব "vim.h"

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
স্থির পূর্ণসংখ্যা clip_provider_is_available(char_u *provider);
#endif

#যদি defined(FEAT_CLIPBOARD)

# যদি defined(FEAT_WAYLAND_CLIPBOARD)

#  include "wayland.h"

// Represents either the regular or primary selection
প্রকারলেখ কাঠামো {
    char_u		*contents;	// Non-null if we own selection,
					// contains the data to send to other
					// clients.
    vwl_তথ্য_source_T	*source;	// Non-NULL if we own the selection,
					// else NULL if we don't.
    vwl_তথ্য_offer_T	*offer;		// Current offer for the selection

    bool		own_success;	// Used by clip_wl_own_selection()
    bool		available;	// If selection is ready to serve/use

    // These may point to the same proxy as the other selection
    vwl_তথ্য_device_manager_T	*manager;
    vwl_তথ্য_device_T		*device;
} clip_wl_selection_T;

// Represents the clipboard for the global Wayland connection, for the chosen
// seat (using the 'wl_seat' option)
প্রকারলেখ কাঠামো {
    vwl_seat_T *seat;

    clip_wl_selection_T regular;
    clip_wl_selection_T primary;
} clip_wl_T;

// Mime types we support sending and receiving
// Mimes with a lower index in the array are prioritized first when we are
// receiving data.
স্থির ধ্রুবক অক্ষর *supported_mimes[] = {
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

স্থির শূন্য
clip_wl_receive_তথ্য(Clipboard_T *cbd, ধ্রুবক অক্ষর *mime_type, পূর্ণসংখ্যা fd);
স্থির শূন্য clip_wl_request_selection(Clipboard_T *cbd);
স্থির পূর্ণসংখ্যা clip_wl_own_selection(Clipboard_T *cbd);
স্থির শূন্য clip_wl_lose_selection(Clipboard_T *cbd);
স্থির শূন্য clip_wl_set_selection(Clipboard_T *cbd);

#  যদি defined(USE_SYSTEM)
স্থির bool clip_wl_owner_exists(Clipboard_T *cbd);
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
    শূন্য
clip_init(পূর্ণসংখ্যা can_use)
{
    Clipboard_T *cb;

    cb = &clip_star;
    জন্য (;;)
    {
	// No need to init again if cbd is already available
	যদি (can_use && cb->available)
	    যাও skip;

	cb->available  = can_use;
	cb->owned      = FALSE;
	cb->start.lnum = 0;
	cb->start.col  = 0;
	cb->end.lnum   = 0;
	cb->end.col    = 0;
	cb->state      = SELECT_CLEARED;

skip:
	যদি (cb == &clip_plus)
	    বিরতি;
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
    শূন্য
clip_update_selection(Clipboard_T *clip)
{
    pos_T	    start, end;

    // If visual mode is only due to a redo command ("."), then ignore it
    যদি (!redo_VIsual_busy && VIsual_active && (State & MODE_NORMAL))
    {
	যদি (LT_POS(VIsual, curwin->w_cursor))
	{
	    start = VIsual;
	    end = curwin->w_cursor;
	    যদি (has_mbyte)
		end.col += (*mb_ptr2len)(ml_get_cursor()) - 1;
	}
	অন্যথা
	{
	    start = curwin->w_cursor;
	    end = VIsual;
	}
	যদি (!EQUAL_POS(clip->start, start)
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

    স্থির পূর্ণসংখ্যা
clip_gen_own_selection(Clipboard_T *cbd)
{
# যদি defined(FEAT_XCLIPBOARD) || defined(FEAT_WAYLAND_CLIPBOARD)
#  ifdef FEAT_GUI
    যদি (gui.in_use)
	ফেরত clip_mch_own_selection(cbd);
    অন্যথা
#  endif
    {
	যদি (clipmethod == CLIPMETHOD_WAYLAND)
	{
#  ifdef FEAT_WAYLAND_CLIPBOARD
	    ফেরত clip_wl_own_selection(cbd);
#  endif
	}
	অন্যথা যদি (clipmethod == CLIPMETHOD_X11)
	{
#  ifdef FEAT_XCLIPBOARD
	    ফেরত clip_xterm_own_selection(cbd);
#  endif
	}
    }
    ফেরত FAIL;
# অন্যথা
    ফেরত clip_mch_own_selection(cbd);
# endif
}

    শূন্য
clip_own_selection(Clipboard_T *cbd)
{
    /*
     * Also want to check somehow that we are reading from the keyboard rather
     * than a mapping etc.
     */
# যদি defined(FEAT_X11) || defined(FEAT_WAYLAND_CLIPBOARD)
    // Always own the selection, we might have lost it without being
    // notified, e.g. during a ":sh" command.
    যদি (cbd->available)
    {
	পূর্ণসংখ্যা was_owned = cbd->owned;

	cbd->owned = (clip_gen_own_selection(cbd) == OK);
	যদি (!was_owned && (cbd == &clip_star || cbd == &clip_plus))
	{
	    // May have to show a different kind of highlighting for the
	    // selected area.  There is no specific redraw command for this,
	    // just redraw all windows on the current buffer.
	    যদি (cbd->owned
		    && (get_real_state() == MODE_VISUAL
					    || get_real_state() == MODE_SELECT)
		    && (cbd == &clip_star ? clip_isautosel_star()
						      : clip_isautosel_plus())
		    && HL_ATTR(HLF_V) != HL_ATTR(HLF_VNC))
		redraw_curbuf_later(UPD_INVERTED_ALL);
	}
    }
# অন্যথা
    // Only own the clipboard when we didn't own it yet.
    যদি (!cbd->owned && cbd->available)
	cbd->owned = (clip_gen_own_selection(cbd) == OK);
# endif
}

    স্থির শূন্য
clip_gen_lose_selection(Clipboard_T *cbd)
{
# যদি defined(FEAT_XCLIPBOARD) || defined(FEAT_WAYLAND_CLIPBOARD)
#  ifdef FEAT_GUI
    যদি (gui.in_use)
	clip_mch_lose_selection(cbd);
    অন্যথা
#  endif
    {
	যদি (clipmethod == CLIPMETHOD_WAYLAND)
	{
#  ifdef FEAT_WAYLAND_CLIPBOARD
	    clip_wl_lose_selection(cbd);
#  endif
	}
	অন্যথা যদি (clipmethod == CLIPMETHOD_X11)
	{
#  ifdef FEAT_XCLIPBOARD
	    clip_xterm_lose_selection(cbd);
#  endif
	}
    }
# অন্যথা
    clip_mch_lose_selection(cbd);
# endif
}

    শূন্য
clip_lose_selection(Clipboard_T *cbd)
{
# ifdef FEAT_X11
    পূর্ণসংখ্যা	    was_owned = cbd->owned;
# endif
    পূর্ণসংখ্যা     visual_selection = FALSE;

    যদি (cbd == &clip_star || cbd == &clip_plus)
	visual_selection = TRUE;

    clip_free_selection(cbd);
    cbd->owned = FALSE;
    যদি (visual_selection)
	clip_clear_selection(cbd);
    clip_gen_lose_selection(cbd);
# ifdef FEAT_X11
    যদি (visual_selection)
    {
	// May have to show a different kind of highlighting for the selected
	// area.  There is no specific redraw command for this, just redraw all
	// windows on the current buffer.
	যদি (was_owned
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

    স্থির শূন্য
clip_copy_selection(Clipboard_T *clip)
{
    যদি (VIsual_active && (State & MODE_NORMAL) && clip->available)
    {
	clip_update_selection(clip);
	clip_free_selection(clip);
	clip_own_selection(clip);
	যদি (clip->owned)
	    clip_get_selection(clip);
	clip_gen_set_selection(clip);
    }
}

/*
 * Save and restore clip_unnamed before doing possibly many changes. This
 * prevents accessing the clipboard very often which might slow down Vim
 * considerably.
 */
স্থির পূর্ণসংখ্যা global_change_গুনতি = 0; // if set, inside a start_global_changes
স্থির পূর্ণসংখ্যা clipboard_needs_update = FALSE; // clipboard needs to be updated
স্থির পূর্ণসংখ্যা clip_did_set_selection = TRUE;

/*
 * Save clip_unnamed and reset it.
 */
    শূন্য
start_global_changes(শূন্য)
{
    যদি (++global_change_গুনতি > 1)
	ফেরত;
    clip_unnamed_saved = clip_unnamed;
    clipboard_needs_update = FALSE;

    যদি (clip_did_set_selection)
    {
	clip_unnamed = 0;
	clip_did_set_selection = FALSE;
    }
}

/*
 * Return TRUE if setting the clipboard was postponed, it already contains the
 * right text.
 */
    স্থির পূর্ণসংখ্যা
is_clipboard_needs_update(শূন্য)
{
    ফেরত clipboard_needs_update;
}

/*
 * Restore clip_unnamed and set the selection when needed.
 */
    শূন্য
end_global_changes(শূন্য)
{
    যদি (--global_change_গুনতি > 0)
	// recursive
	ফেরত;
    যদি (!clip_did_set_selection)
    {
	clip_did_set_selection = TRUE;
	clip_unnamed = clip_unnamed_saved;
	clip_unnamed_saved = 0;
	যদি (clipboard_needs_update)
	{
	    // only store something in the clipboard,
	    // if we have yanked anything to it
	    যদি (clip_unnamed & CLIP_UNNAMED)
	    {
		clip_own_selection(&clip_star);
		clip_gen_set_selection(&clip_star);
	    }
	    যদি (clip_unnamed & CLIP_UNNAMED_PLUS)
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
    শূন্য
clip_auto_select(শূন্য)
{
    যদি (clip_isautosel_star())
	clip_copy_selection(&clip_star);
    যদি (clip_isautosel_plus())
	clip_copy_selection(&clip_plus);
}

/*
 * Return TRUE if automatic selection of Visual area is desired for the *
 * register.
 */
    পূর্ণসংখ্যা
clip_isautosel_star(শূন্য)
{
# ifdef FEAT_CLIPBOARD_PROVIDER
    যদি (clipmethod == CLIPMETHOD_PROVIDER)
	ফেরত false;
# endif
# ifdef FEAT_GUI
    যদি (gui.in_use)
	ফেরত vim_strchr(p_go, GO_ASEL) != ফাঁকা
	    && vim_strchr(p_go, GO_ASELPLUS) == ফাঁকা;
# endif
    ফেরত clip_autoselect_star;
}

/*
 * Return TRUE if automatic selection of Visual area is desired for the +
 * register.
 */
    পূর্ণসংখ্যা
clip_isautosel_plus(শূন্য)
{
# ifdef FEAT_CLIPBOARD_PROVIDER
    যদি (clipmethod == CLIPMETHOD_PROVIDER)
	ফেরত false;
# endif
# ifdef FEAT_GUI
    যদি (gui.in_use)
	ফেরত vim_strchr(p_go, GO_ASELPLUS) != ফাঁকা;
# endif
    ফেরত clip_autoselect_plus;
}


/*
 * Stuff for general mouse selection, without using Visual mode.
 */

/*
 * Compare two screen positions ala strcmp()
 */
    স্থির পূর্ণসংখ্যা
clip_compare_pos(
    পূর্ণসংখ্যা		row1,
    পূর্ণসংখ্যা		col1,
    পূর্ণসংখ্যা		row2,
    পূর্ণসংখ্যা		col2)
{
    যদি (row1 > row2) ফেরত(1);
    যদি (row1 < row2) ফেরত(-1);
    যদি (col1 > col2) ফেরত(1);
    যদি (col1 < col2) ফেরত(-1);
    ফেরত(0);
}

// "how" flags for clip_invert_area()
# define CLIP_CLEAR	1
# define CLIP_SET	2
# define CLIP_TOGGLE	3

/*
 * Invert or un-invert a rectangle of the screen.
 * "invert" is true if the result is inverted.
 */
    স্থির শূন্য
clip_invert_rectangle(
	Clipboard_T	*cbd UNUSED,
	পূর্ণসংখ্যা		row_arg,
	পূর্ণসংখ্যা		col_arg,
	পূর্ণসংখ্যা		height_arg,
	পূর্ণসংখ্যা		width_arg,
	পূর্ণসংখ্যা		invert)
{
    পূর্ণসংখ্যা		row = row_arg;
    পূর্ণসংখ্যা		col = col_arg;
    পূর্ণসংখ্যা		height = height_arg;
    পূর্ণসংখ্যা		width = width_arg;

# ifdef FEAT_PROP_POPUP
    // this goes on top of all popup windows
    screen_zindex = CLIP_ZINDEX;

    যদি (col < cbd->min_col)
    {
	width -= cbd->min_col - col;
	col = cbd->min_col;
    }
    যদি (width > cbd->max_col - col)
	width = cbd->max_col - col;
    যদি (row < cbd->min_row)
    {
	height -= cbd->min_row - row;
	row = cbd->min_row;
    }
    যদি (height > cbd->max_row - row + 1)
	height = cbd->max_row - row + 1;
# endif
# ifdef FEAT_GUI
    যদি (gui.in_use)
	gui_mch_invert_rectangle(row, col, height, width);
    অন্যথা
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
    স্থির শূন্য
clip_invert_area(
	Clipboard_T	*cbd,
	পূর্ণসংখ্যা		row1,
	পূর্ণসংখ্যা		col1,
	পূর্ণসংখ্যা		row2,
	পূর্ণসংখ্যা		col2,
	পূর্ণসংখ্যা		how)
{
    পূর্ণসংখ্যা		invert = FALSE;
    পূর্ণসংখ্যা		max_col;

# ifdef FEAT_PROP_POPUP
    max_col = cbd->max_col - 1;
# অন্যথা
    max_col = Columns - 1;
# endif

    যদি (how == CLIP_SET)
	invert = TRUE;

    // Swap the from and to positions so the from is always before
    যদি (clip_compare_pos(row1, col1, row2, col2) > 0)
    {
	পূর্ণসংখ্যা tmp_row, tmp_col;

	tmp_row = row1;
	tmp_col = col1;
	row1	= row2;
	col1	= col2;
	row2	= tmp_row;
	col2	= tmp_col;
    }
    অন্যথা যদি (how == CLIP_TOGGLE)
	invert = TRUE;

    // If all on the same line, do it the easy way
    যদি (row1 == row2)
    {
	clip_invert_rectangle(cbd, row1, col1, 1, col2 - col1, invert);
    }
    অন্যথা
    {
	// Handle a piece of the first line
	যদি (col1 > 0)
	{
	    clip_invert_rectangle(cbd, row1, col1, 1,
						  (পূর্ণসংখ্যা)Columns - col1, invert);
	    row1++;
	}

	// Handle a piece of the last line
	যদি (col2 < max_col)
	{
	    clip_invert_rectangle(cbd, row2, 0, 1, col2, invert);
	    row2--;
	}

	// Handle the rectangle that's left
	যদি (row2 >= row1)
	    clip_invert_rectangle(cbd, row1, 0, row2 - row1 + 1,
							 (পূর্ণসংখ্যা)Columns, invert);
    }
}

/*
 * Start, continue or end a modeless selection.  Used when editing the
 * command-line, in the cmdline window and when the mouse is in a popup window.
 */
    শূন্য
clip_modeless(পূর্ণসংখ্যা button, পূর্ণসংখ্যা is_click, পূর্ণসংখ্যা is_drag)
{
    পূর্ণসংখ্যা		repeat;

    repeat = ((clip_star.mode == SELECT_MODE_CHAR
		|| clip_star.mode == SELECT_MODE_সারি)
					      && (mod_mask & MOD_MASK_2CLICK))
	    || (clip_star.mode == SELECT_MODE_WORD
					     && (mod_mask & MOD_MASK_3CLICK));
    যদি (is_click && button == MOUSE_ডান)
    {
	// Right mouse button: If there was no selection, start one.
	// Otherwise extend the existing selection.
	যদি (clip_star.state == SELECT_CLEARED)
	    clip_start_selection(mouse_col, mouse_row, FALSE);
	clip_process_selection(button, mouse_col, mouse_row, repeat);
    }
    অন্যথা যদি (is_click)
	clip_start_selection(mouse_col, mouse_row, repeat);
    অন্যথা যদি (is_drag)
    {
	// Don't try extending a selection if there isn't one.  Happens when
	// button-down is in the cmdline and them moving mouse upwards.
	যদি (clip_star.state != SELECT_CLEARED)
	    clip_process_selection(button, mouse_col, mouse_row, repeat);
    }
    অন্যথা // release
	clip_process_selection(MOUSE_RELEASE, mouse_col, mouse_row, FALSE);
}

/*
 * Update the currently selected region by adding and/or subtracting from the
 * beginning or end and inverting the changed area(s).
 */
    স্থির শূন্য
clip_update_modeless_selection(
    Clipboard_T    *cb,
    পূর্ণসংখ্যা		    row1,
    পূর্ণসংখ্যা		    col1,
    পূর্ণসংখ্যা		    row2,
    পূর্ণসংখ্যা		    col2)
{
    // See if we changed at the beginning of the selection
    যদি (row1 != cb->start.lnum || col1 != (পূর্ণসংখ্যা)cb->start.col)
    {
	clip_invert_area(cb, row1, col1, (পূর্ণসংখ্যা)cb->start.lnum, cb->start.col,
								 CLIP_TOGGLE);
	cb->start.lnum = row1;
	cb->start.col  = col1;
    }

    // See if we changed at the end of the selection
    যদি (row2 != cb->end.lnum || col2 != (পূর্ণসংখ্যা)cb->end.col)
    {
	clip_invert_area(cb, (পূর্ণসংখ্যা)cb->end.lnum, cb->end.col, row2, col2,
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

    স্থির শূন্য
clip_get_word_boundaries(Clipboard_T *cb, পূর্ণসংখ্যা row, পূর্ণসংখ্যা col)
{
    পূর্ণসংখ্যা		start_class;
    পূর্ণসংখ্যা		অস্থায়ী_col;
    char_u	*p;
    পূর্ণসংখ্যা		mboff;

    যদি (row >= screen_Rows || col >= screen_Columns || Screen_সারিগুলি == ফাঁকা)
	ফেরত;

    p = Screen_সারিগুলি + সারি_Offset[row];
    // Correct for starting in the right half of a double-wide char
    যদি (enc_dbcs != 0)
	col -= dbcs_screen_শীর্ষ_off(p, p + col);
    অন্যথা যদি (enc_utf8 && p[col] == 0)
	--col;
    start_class = CHAR_CLASS(p[col]);

    অস্থায়ী_col = col;
    জন্য ( ; অস্থায়ী_col > 0; অস্থায়ী_col--)
	যদি (enc_dbcs != 0
		   && (mboff = dbcs_screen_শীর্ষ_off(p, p + অস্থায়ী_col - 1)) > 0)
	    অস্থায়ী_col -= mboff;
	অন্যথা যদি (CHAR_CLASS(p[অস্থায়ী_col - 1]) != start_class
		&& !(enc_utf8 && p[অস্থায়ী_col - 1] == 0))
	    বিরতি;
    cb->word_start_col = অস্থায়ী_col;

    অস্থায়ী_col = col;
    জন্য ( ; অস্থায়ী_col < screen_Columns; অস্থায়ী_col++)
	যদি (enc_dbcs != 0 && dbcs_ptr2cells(p + অস্থায়ী_col) == 2)
	    ++অস্থায়ী_col;
	অন্যথা যদি (CHAR_CLASS(p[অস্থায়ী_col]) != start_class
		&& !(enc_utf8 && p[অস্থায়ী_col] == 0))
	    বিরতি;
    cb->word_end_col = অস্থায়ী_col;
}

/*
 * Find the column position for the last non-whitespace character on the given
 * line at or before start_col.
 */
    স্থির পূর্ণসংখ্যা
clip_get_সারি_end(Clipboard_T *cbd UNUSED, পূর্ণসংখ্যা row)
{
    পূর্ণসংখ্যা	    i;

    যদি (row >= screen_Rows || Screen_সারিগুলি == ফাঁকা)
	ফেরত 0;
    জন্য (i =
# ifdef FEAT_PROP_POPUP
	    cbd->max_col;
# অন্যথা
	    screen_Columns;
# endif
			    i > 0; i--)
	যদি (Screen_সারিগুলি[সারি_Offset[row] + i - 1] != ' ')
	    বিরতি;
    ফেরত i;
}

/*
 * Start the selection
 */
    শূন্য
clip_start_selection(পূর্ণসংখ্যা col, পূর্ণসংখ্যা row, পূর্ণসংখ্যা repeated_click)
{
    Clipboard_T	*cb = &clip_star;
# ifdef FEAT_PROP_POPUP
    win_T	*wp;
    পূর্ণসংখ্যা		row_cp = row;
    পূর্ণসংখ্যা		col_cp = col;

    wp = mouse_find_win(&row_cp, &col_cp, FIND_POPUP);
    যদি (wp != ফাঁকা && WIN_IS_POPUP(wp)
				  && popup_is_in_scrollbar(wp, row_cp, col_cp))
	// click or double click in scrollbar does not start a selection
	ফেরত;
# endif

    যদি (cb->state == SELECT_DONE)
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
    যদি (wp != ফাঁকা && WIN_IS_POPUP(wp))
    {
	// Click in a popup window restricts selection to that window,
	// excluding the border.
	cb->min_col = wp->w_wincol + wp->w_popup_border[3];
	cb->max_col = wp->w_wincol + popup_width(wp)
				 - wp->w_popup_border[1] - wp->w_has_scrollbar;
	যদি (cb->max_col > screen_Columns)
	    cb->max_col = screen_Columns;
	cb->min_row = wp->w_winrow + wp->w_popup_border[0];
	cb->max_row = wp->w_winrow + popup_height(wp) - 1
						   - wp->w_popup_border[2];
    }
    অন্যথা
    {
	cb->min_col = 0;
	cb->max_col = screen_Columns;
	cb->min_row = 0;
	cb->max_row = screen_Rows;
    }
# endif

    যদি (repeated_click)
    {
	যদি (++cb->mode > SELECT_MODE_সারি)
	    cb->mode = SELECT_MODE_CHAR;
    }
    অন্যথা
	cb->mode = SELECT_MODE_CHAR;

# ifdef FEAT_GUI
    // clear the cursor until the selection is made
    যদি (gui.in_use)
	gui_undraw_cursor();
# endif

    নির্বাচন (cb->mode)
    {
	ক্ষেত্র SELECT_MODE_CHAR:
	    cb->origin_start_col = cb->start.col;
	    cb->word_end_col = clip_get_সারি_end(cb, (পূর্ণসংখ্যা)cb->start.lnum);
	    বিরতি;

	ক্ষেত্র SELECT_MODE_WORD:
	    clip_get_word_boundaries(cb, (পূর্ণসংখ্যা)cb->start.lnum, cb->start.col);
	    cb->origin_start_col = cb->word_start_col;
	    cb->origin_end_col	 = cb->word_end_col;

	    clip_invert_area(cb, (পূর্ণসংখ্যা)cb->start.lnum, cb->word_start_col,
			    (পূর্ণসংখ্যা)cb->end.lnum, cb->word_end_col, CLIP_SET);
	    cb->start.col = cb->word_start_col;
	    cb->end.col   = cb->word_end_col;
	    বিরতি;

	ক্ষেত্র SELECT_MODE_সারি:
	    clip_invert_area(cb, (পূর্ণসংখ্যা)cb->start.lnum, 0, (পূর্ণসংখ্যা)cb->start.lnum,
			    (পূর্ণসংখ্যা)Columns, CLIP_SET);
	    cb->start.col = 0;
	    cb->end.col   = Columns;
	    বিরতি;
    }

    cb->পূর্ব = cb->start;

# ifdef DEBUG_SELECTION
    printf("Selection started at (%ld,%d)\n", cb->start.lnum, cb->start.col);
# endif
}

/*
 * Continue processing the selection
 */
    শূন্য
clip_process_selection(
    পূর্ণসংখ্যা		button,
    পূর্ণসংখ্যা		col,
    পূর্ণসংখ্যা		row,
    int_u	repeated_click)
{
    Clipboard_T	*cb = &clip_star;
    পূর্ণসংখ্যা		diff;
    পূর্ণসংখ্যা		slen = 1;	// cursor shape width

    যদি (button == MOUSE_RELEASE)
    {
	যদি (cb->state != SELECT_IN_PROGRESS)
	    ফেরত;

	// Check to make sure we have something selected
	যদি (cb->start.lnum == cb->end.lnum && cb->start.col == cb->end.col)
	{
# ifdef FEAT_GUI
	    যদি (gui.in_use)
		gui_update_cursor(FALSE, FALSE);
# endif
	    cb->state = SELECT_CLEARED;
	    ফেরত;
	}

# ifdef DEBUG_SELECTION
	printf("Selection ended: (%ld,%d) to (%ld,%d)\n", cb->start.lnum,
		cb->start.col, cb->end.lnum, cb->end.col);
# endif
	যদি (clip_isautosel_star() || clip_isautosel_plus()
		|| (
# ifdef FEAT_GUI
		    gui.in_use ? (vim_strchr(p_go, GO_ASELML) != ফাঁকা) :
# endif
		    clip_autoselectml))
	    clip_copy_modeless_selection(FALSE);
# ifdef FEAT_GUI
	যদি (gui.in_use)
	    gui_update_cursor(FALSE, FALSE);
# endif

	cb->state = SELECT_DONE;
	ফেরত;
    }

    row = check_row(row);
    col = check_col(col);
    col = mb_fix_col(col, row);

    যদি (col == (পূর্ণসংখ্যা)cb->পূর্ব.col && row == cb->পূর্ব.lnum && !repeated_click)
	ফেরত;

    /*
     * When extending the selection with the right mouse button, swap the
     * start and end if the position is before half the selection
     */
    যদি (cb->state == SELECT_DONE && button == MOUSE_ডান)
    {
	/*
	 * If the click is before the start, or the click is inside the
	 * selection and the start is the closest side, set the origin to the
	 * end of the selection.
	 */
	যদি (clip_compare_pos(row, col, (পূর্ণসংখ্যা)cb->start.lnum, cb->start.col) < 0
		|| (clip_compare_pos(row, col,
					   (পূর্ণসংখ্যা)cb->end.lnum, cb->end.col) < 0
		    && (((cb->start.lnum == cb->end.lnum
			    && cb->end.col - col > col - cb->start.col))
			|| ((diff = (cb->end.lnum - row) -
						   (row - cb->start.lnum)) > 0
			    || (diff == 0 && col < (পূর্ণসংখ্যা)(cb->start.col +
							 cb->end.col) / 2)))))
	{
	    cb->origin_row = (short_u)cb->end.lnum;
	    cb->origin_start_col = cb->end.col - 1;
	    cb->origin_end_col = cb->end.col;
	}
	অন্যথা
	{
	    cb->origin_row = (short_u)cb->start.lnum;
	    cb->origin_start_col = cb->start.col;
	    cb->origin_end_col = cb->start.col;
	}
	যদি (cb->mode == SELECT_MODE_WORD && !repeated_click)
	    cb->mode = SELECT_MODE_CHAR;
    }

    // set state, for when using the right mouse button
    cb->state = SELECT_IN_PROGRESS;

# ifdef DEBUG_SELECTION
    printf("Selection extending to (%d,%d)\n", row, col);
# endif

    যদি (repeated_click && ++cb->mode > SELECT_MODE_সারি)
	cb->mode = SELECT_MODE_CHAR;

    নির্বাচন (cb->mode)
    {
	ক্ষেত্র SELECT_MODE_CHAR:
	    // If we're on a different line, find where the line ends
	    যদি (row != cb->পূর্ব.lnum)
		cb->word_end_col = clip_get_সারি_end(cb, row);

	    // See if we are before or after the origin of the selection
	    যদি (clip_compare_pos(row, col, cb->origin_row,
						   cb->origin_start_col) >= 0)
	    {
		যদি (col >= (পূর্ণসংখ্যা)cb->word_end_col)
		    clip_update_modeless_selection(cb, cb->origin_row,
			    cb->origin_start_col, row, (পূর্ণসংখ্যা)Columns);
		অন্যথা
		{
		    যদি (has_mbyte && mb_lefthalve(row, col))
			slen = 2;
		    clip_update_modeless_selection(cb, cb->origin_row,
			    cb->origin_start_col, row, col + slen);
		}
	    }
	    অন্যথা
	    {
		যদি (has_mbyte
			&& mb_lefthalve(cb->origin_row, cb->origin_start_col))
		    slen = 2;
		যদি (col >= (পূর্ণসংখ্যা)cb->word_end_col)
		    clip_update_modeless_selection(cb, row, cb->word_end_col,
			    cb->origin_row, cb->origin_start_col + slen);
		অন্যথা
		    clip_update_modeless_selection(cb, row, col,
			    cb->origin_row, cb->origin_start_col + slen);
	    }
	    বিরতি;

	ক্ষেত্র SELECT_MODE_WORD:
	    // If we are still within the same word, do nothing
	    যদি (row == cb->পূর্ব.lnum && col >= (পূর্ণসংখ্যা)cb->word_start_col
		    && col < (পূর্ণসংখ্যা)cb->word_end_col && !repeated_click)
		ফেরত;

	    // Get new word boundaries
	    clip_get_word_boundaries(cb, row, col);

	    // Handle being after the origin point of selection
	    যদি (clip_compare_pos(row, col, cb->origin_row,
		    cb->origin_start_col) >= 0)
		clip_update_modeless_selection(cb, cb->origin_row,
			cb->origin_start_col, row, cb->word_end_col);
	    অন্যথা
		clip_update_modeless_selection(cb, row, cb->word_start_col,
			cb->origin_row, cb->origin_end_col);
	    বিরতি;

	ক্ষেত্র SELECT_MODE_সারি:
	    যদি (row == cb->পূর্ব.lnum && !repeated_click)
		ফেরত;

	    যদি (clip_compare_pos(row, col, cb->origin_row,
		    cb->origin_start_col) >= 0)
		clip_update_modeless_selection(cb, cb->origin_row, 0, row,
			(পূর্ণসংখ্যা)Columns);
	    অন্যথা
		clip_update_modeless_selection(cb, row, 0, cb->origin_row,
			(পূর্ণসংখ্যা)Columns);
	    বিরতি;
    }

    cb->পূর্ব.lnum = row;
    cb->পূর্ব.col  = col;

# ifdef DEBUG_SELECTION
	printf("Selection is: (%ld,%d) to (%ld,%d)\n", cb->start.lnum,
		cb->start.col, cb->end.lnum, cb->end.col);
# endif
}

# যদি defined(FEAT_GUI)
/*
 * Redraw part of the selection if character at "row,col" is inside of it.
 * Only used for the GUI.
 */
    শূন্য
clip_may_redraw_selection(পূর্ণসংখ্যা row, পূর্ণসংখ্যা col, পূর্ণসংখ্যা len)
{
    পূর্ণসংখ্যা		start = col;
    পূর্ণসংখ্যা		end = col + len;

    যদি (clip_star.state != SELECT_CLEARED
	    && row >= clip_star.start.lnum
	    && row <= clip_star.end.lnum)
    {
	যদি (row == clip_star.start.lnum && start < (পূর্ণসংখ্যা)clip_star.start.col)
	    start = clip_star.start.col;
	যদি (row == clip_star.end.lnum && end > (পূর্ণসংখ্যা)clip_star.end.col)
	    end = clip_star.end.col;
	যদি (end > start)
	    clip_invert_area(&clip_star, row, start, row, end, 0);
    }
}
# endif

/*
 * Called from outside to clear selected region from the display
 */
    শূন্য
clip_clear_selection(Clipboard_T *cbd)
{

    যদি (cbd->state == SELECT_CLEARED)
	ফেরত;

    clip_invert_area(cbd, (পূর্ণসংখ্যা)cbd->start.lnum, cbd->start.col,
				 (পূর্ণসংখ্যা)cbd->end.lnum, cbd->end.col, CLIP_CLEAR);
    cbd->state = SELECT_CLEARED;
}

/*
 * Clear the selection if any lines from "row1" to "row2" are inside of it.
 */
    শূন্য
clip_may_clear_selection(পূর্ণসংখ্যা row1, পূর্ণসংখ্যা row2)
{
    যদি (clip_star.state == SELECT_DONE
	    && row2 >= clip_star.start.lnum
	    && row1 <= clip_star.end.lnum)
	clip_clear_selection(&clip_star);
}

/*
 * Called before the screen is scrolled up or down.  Adjusts the line numbers
 * of the selection.  Call with big number when clearing the screen.
 */
    শূন্য
clip_scroll_selection(
    পূর্ণসংখ্যা	    rows)		// negative for scroll down
{
    পূর্ণসংখ্যা	    lnum;

    যদি (clip_star.state == SELECT_CLEARED)
	ফেরত;

    lnum = clip_star.start.lnum - rows;
    যদি (lnum <= 0)
	clip_star.start.lnum = 0;
    অন্যথা যদি (lnum >= screen_Rows)	// scrolled off of the screen
	clip_star.state = SELECT_CLEARED;
    অন্যথা
	clip_star.start.lnum = lnum;

    lnum = clip_star.end.lnum - rows;
    যদি (lnum < 0)			// scrolled off of the screen
	clip_star.state = SELECT_CLEARED;
    অন্যথা যদি (lnum >= screen_Rows)
	clip_star.end.lnum = screen_Rows - 1;
    অন্যথা
	clip_star.end.lnum = lnum;
}

/*
 * Copy the currently selected area into the '*' or '+' register so it will be
 * available for pasting.
 * When "both" is TRUE also copy to the other register.
 */
    শূন্য
clip_copy_modeless_selection(পূর্ণসংখ্যা both UNUSED)
{
    // The info for the modeless selection is stored in '*' register, however if
    // we are using the '+' register for modeless autoselect, we copy to
    // clip_plus instead while using the info in clip_star.
    Clipboard_T *cbd = clip_isautosel_plus() ? &clip_plus : &clip_star;
    char_u	*বাফার;
    char_u	*bufp;
    পূর্ণসংখ্যা		row;
    পূর্ণসংখ্যা		start_col;
    পূর্ণসংখ্যা		end_col;
    পূর্ণসংখ্যা		সারি_end_col;
    পূর্ণসংখ্যা		add_newline_flag = FALSE;
    পূর্ণসংখ্যা		len;
    char_u	*p;
    পূর্ণসংখ্যা		row1 = clip_star.start.lnum;
    পূর্ণসংখ্যা		col1 = clip_star.start.col;
    পূর্ণসংখ্যা		row2 = clip_star.end.lnum;
    পূর্ণসংখ্যা		col2 = clip_star.end.col;

    // Can't use ScreenLines unless initialized
    যদি (Screen_সারিগুলি == ফাঁকা)
	ফেরত;

    /*
     * Make sure row1 <= row2, and if row1 == row2 that col1 <= col2.
     */
    যদি (row1 > row2)
    {
	row = row1; row1 = row2; row2 = row;
	row = col1; col1 = col2; col2 = row;
    }
    অন্যথা যদি (row1 == row2 && col1 > col2)
    {
	row = col1; col1 = col2; col2 = row;
    }
# ifdef FEAT_PROP_POPUP
    যদি (col1 < clip_star.min_col)
	col1 = clip_star.min_col;
    যদি (col2 > clip_star.max_col)
	col2 = clip_star.max_col;
    যদি (row1 > clip_star.max_row || row2 < clip_star.min_row)
	ফেরত;
    যদি (row1 < clip_star.min_row)
	row1 = clip_star.min_row;
    যদি (row2 > clip_star.max_row)
	row2 = clip_star.max_row;
# endif
    // correct starting point for being on right half of double-wide char
    p = Screen_সারিগুলি + সারি_Offset[row1];
    যদি (enc_dbcs != 0)
	col1 -= (*mb_শীর্ষ_off)(p, p + col1);
    অন্যথা যদি (enc_utf8 && p[col1] == 0)
	--col1;

    // Create a temporary buffer for storing the text
    len = (row2 - row1 + 1) * Columns + 1;
    যদি (enc_dbcs != 0)
	len *= 2;	// max. 2 bytes per display cell
    অন্যথা যদি (enc_utf8)
	len *= MB_MAXBYTES;
    // A newline is added after each row
    len += row2 - row1 + 2;
    বাফার = alloc(len);
    যদি (বাফার == ফাঁকা)	    // out of memory
	ফেরত;

    // Process each row in the selection
    জন্য (bufp = বাফার, row = row1; row <= row2; row++)
    {
	যদি (row == row1)
	    start_col = col1;
	অন্যথা
# ifdef FEAT_PROP_POPUP
	    start_col = clip_star.min_col;
# অন্যথা
	    start_col = 0;
# endif

	যদি (row == row2)
	    end_col = col2;
	অন্যথা
# ifdef FEAT_PROP_POPUP
	    end_col = clip_star.max_col;
# অন্যথা
	    end_col = Columns;
# endif

	সারি_end_col = clip_get_সারি_end(&clip_star, row);

	// See if we need to nuke some trailing whitespace
	যদি (end_col >=
# ifdef FEAT_PROP_POPUP
		clip_star.max_col
# অন্যথা
		Columns
# endif
		    && (row < row2 || end_col > সারি_end_col))
	{
	    // Get rid of trailing whitespace
	    end_col = সারি_end_col;
	    যদি (end_col < start_col)
		end_col = start_col;

	    // If the last line extended to the end, add an extra newline
	    যদি (row == row2)
		add_newline_flag = TRUE;
	}

	// If after the first row, we need to always add a newline
	যদি (row > row1 && !সারি_Wraps[row - 1])
	    *bufp++ = NL;

	// Safetey check for in case resizing went wrong
	যদি (row < screen_Rows && end_col <= screen_Columns)
	{
	    যদি (enc_dbcs != 0)
	    {
		পূর্ণসংখ্যা	i;

		p = Screen_সারিগুলি + সারি_Offset[row];
		জন্য (i = start_col; i < end_col; ++i)
		    যদি (enc_dbcs == DBCS_JPNU && p[i] == 0x8e)
		    {
			// single-width double-byte char
			*bufp++ = 0x8e;
			*bufp++ = ScreenLines2[সারি_Offset[row] + i];
		    }
		    অন্যথা
		    {
			*bufp++ = p[i];
			যদি (MB_BYTE2LEN(p[i]) == 2)
			    *bufp++ = p[++i];
		    }
	    }
	    অন্যথা যদি (enc_utf8)
	    {
		পূর্ণসংখ্যা	off;
		পূর্ণসংখ্যা	i;

		off = সারি_Offset[row];
		জন্য (i = start_col; i < end_col; ++i)
		{
		    // The base character is either in ScreenLinesUC[] or
		    // ScreenLines[].
		    যদি (Screen_সারিগুলি_UC[off + i] == 0)
			*bufp++ = Screen_সারিগুলি[off + i];
		    অন্যথা
			// Includes the composing characters.
			bufp += utfc_char2bytes(off + i, bufp);
		    // Skip right half of double-wide character.
		    যদি (Screen_সারিগুলি[off + i + 1] == 0)
			++i;
		}
	    }
	    অন্যথা
	    {
		STRNCPY(bufp, Screen_সারিগুলি + সারি_Offset[row] + start_col,
							 end_col - start_col);
		bufp += end_col - start_col;
	    }
	}
    }

    // Add a newline at the end if the selection ended there
    যদি (add_newline_flag)
	*bufp++ = NL;

    // First cleanup any old selection and become the owner.
    clip_free_selection(cbd);
    clip_own_selection(cbd);

    // Yank the text into the '*' register.
    clip_yank_selection(MCHAR, বাফার, (long)(bufp - বাফার), cbd);

    // Make the register contents available to the outside world.
    clip_gen_set_selection(cbd);

# ifdef FEAT_X11
    যদি (both)
    {
	Clipboard_T *other = cbd == &clip_star ? &clip_plus : &clip_star;
	// Do the same for the '+' register.
	clip_free_selection(other);
	clip_own_selection(other);
	clip_yank_selection(MCHAR, বাফার, (long)(bufp - বাফার), other);
	clip_gen_set_selection(other);
    }
# endif
    vim_free(বাফার);
}

    শূন্য
clip_gen_set_selection(Clipboard_T *cbd)
{
    যদি (!clip_did_set_selection)
    {
	// Updating postponed, so that accessing the system clipboard won't
	// hang Vim when accessing it many times (e.g. on a :g command).
	যদি ((cbd == &clip_plus && (clip_unnamed_saved & CLIP_UNNAMED_PLUS))
		|| (cbd == &clip_star && (clip_unnamed_saved & CLIP_UNNAMED)))
	{
	    clipboard_needs_update = TRUE;
	    ফেরত;
	}
    }
# যদি defined(FEAT_XCLIPBOARD) || defined(FEAT_WAYLAND_CLIPBOARD)
#  ifdef FEAT_GUI
    যদি (gui.in_use)
	clip_mch_set_selection(cbd);
    অন্যথা
#  endif
    {
	যদি (clipmethod == CLIPMETHOD_WAYLAND)
	{
#  ifdef FEAT_WAYLAND_CLIPBOARD
	    clip_wl_set_selection(cbd);
#  endif
	}
	অন্যথা যদি (clipmethod == CLIPMETHOD_X11)
	{
#  ifdef FEAT_XCLIPBOARD
	    clip_xterm_set_selection(cbd);
#  endif
	}
    }
# অন্যথা
    clip_mch_set_selection(cbd);
# endif
}

    স্থির শূন্য
clip_gen_request_selection(Clipboard_T *cbd)
{
# যদি defined(FEAT_XCLIPBOARD) || defined(FEAT_WAYLAND_CLIPBOARD)
#  ifdef FEAT_GUI
    যদি (gui.in_use)
	clip_mch_request_selection(cbd);
    অন্যথা
#  endif
    {
	যদি (clipmethod == CLIPMETHOD_WAYLAND)
	{
#  ifdef FEAT_WAYLAND_CLIPBOARD
	    clip_wl_request_selection(cbd);
#  endif
	}
	অন্যথা যদি (clipmethod == CLIPMETHOD_X11)
	{
#  ifdef FEAT_XCLIPBOARD
	    clip_xterm_request_selection(cbd);
#  endif
	}
    }
# অন্যথা
    clip_mch_request_selection(cbd);
# endif
}

# যদি (defined(FEAT_X11) && defined(FEAT_XCLIPBOARD) && defined(USE_SYSTEM))
    স্থির পূর্ণসংখ্যা
clip_x11_owner_exists(Clipboard_T *cbd)
{
    ফেরত XGetSelectionOwner(X_DISPLAY, cbd->sel_atom) != None;
}
# endif

# যদি (defined(FEAT_X11) || defined(FEAT_WAYLAND_CLIPBOARD)) \
	&& defined(USE_SYSTEM)
    পূর্ণসংখ্যা
clip_gen_owner_exists(Clipboard_T *cbd UNUSED)
{
#  যদি defined(FEAT_XCLIPBOARD) || defined(FEAT_WAYLAND_CLIPBOARD)
#   ifdef FEAT_GUI_GTK
    যদি (gui.in_use)
	ফেরত clip_gtk_owner_exists(cbd);
    অন্যথা
#   endif
    {
	যদি (clipmethod == CLIPMETHOD_WAYLAND)
	{
#   ifdef FEAT_WAYLAND_CLIPBOARD
	    ফেরত clip_wl_owner_exists(cbd);
#   endif
	}
	অন্যথা যদি (clipmethod == CLIPMETHOD_X11)
	{
#   ifdef FEAT_XCLIPBOARD
	    ফেরত clip_x11_owner_exists(cbd);
#   endif
	}
	অন্যথা
	    ফেরত FALSE;
    }
#  অন্যথা
    ফেরত TRUE;
#  endif
}
# endif

/*
 * Stuff for the X clipboard.  Shared between VMS and Unix.
 */

# যদি defined(FEAT_XCLIPBOARD) || defined(FEAT_GUI_X11)
#  include <X11/Xatom.h>
#  include <X11/Intrinsic.h>

/*
 * Open the application context (if it hasn't been opened yet).
 * Used for Motif GUI and the xterm clipboard.
 */
    শূন্য
open_app_context(শূন্য)
{
    যদি (app_context == ফাঁকা)
    {
	XtToolkitInitialize();
	app_context = XtCreateApplicationContext();
    }
}

স্থির Atom	vim_atom;	// Vim's own special selection format
স্থির Atom	vimenc_atom;	// Vim's extended selection format
স্থির Atom	vim_mt_atom;	// Vim's own special selection format (in mime
				// type format)
স্থির Atom	vimenc_mt_atom;	// Vim's extended selection format (in mime type
				// format)
স্থির Atom	utf8_atom;
স্থির Atom	compound_text_atom;
স্থির Atom	text_atom;
স্থির Atom	targets_atom;
স্থির Atom	timestamp_atom;	// Used to get a timestamp

    শূন্য
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

    স্থির Boolean
clip_x11_convert_selection_cb(
    Widget	w UNUSED,
    Atom	*sel_atom,
    Atom	*target,
    Atom	*type,
    XtPointer	*মান,
    long_u	*দৈর্ঘ্য,
    পূর্ণসংখ্যা		*format)
{
    স্থির char_u   *save_ফলাফল = ফাঁকা;
    স্থির long_u   save_দৈর্ঘ্য = 0;
    char_u	    *string;
    পূর্ণসংখ্যা		    motion_type;
    Clipboard_T    *cbd;
    পূর্ণসংখ্যা		    i;

    যদি (*sel_atom == clip_plus.sel_atom)
	cbd = &clip_plus;
    অন্যথা
	cbd = &clip_star;

    যদি (!cbd->owned)
	ফেরত False;	    // Shouldn't ever happen

    // requestor wants to know what target types we support
    যদি (*target == targets_atom)
    {
	স্থির Atom array[9];

	*মান = (XtPointer)array;
	i = 0;
	array[i++] = targets_atom;
	array[i++] = vimenc_atom;
	array[i++] = vim_atom;
	array[i++] = vimenc_mt_atom;
	array[i++] = vim_mt_atom;
	যদি (enc_utf8)
	    array[i++] = utf8_atom;
	array[i++] = XA_STRING;
	array[i++] = text_atom;
	array[i++] = compound_text_atom;

	*type = XA_ATOM;
	// This used to be: *format = sizeof(Atom) * 8; but that caused
	// crashes on 64 bit machines. (Peter Derr)
	*format = 32;
	*দৈর্ঘ্য = i;
	ফেরত True;
    }

    যদি (       *target != XA_STRING
	    && *target != vimenc_atom
	    && *target != vimenc_mt_atom
	    && (*target != utf8_atom || !enc_utf8)
	    && *target != vim_atom
	    && *target != vim_mt_atom
	    && *target != text_atom
	    && *target != compound_text_atom)
	ফেরত False;

    clip_get_selection(cbd);
    motion_type = clip_convert_selection(&string, দৈর্ঘ্য, cbd);
    যদি (motion_type < 0)
	ফেরত False;

    // For our own format, the first byte contains the motion type
    যদি (*target == vim_atom || *target == vim_mt_atom)
	(*দৈর্ঘ্য)++;

    // Our own format with encoding: motion 'encoding' NUL text
    যদি (*target == vimenc_atom || *target == vimenc_mt_atom)
	*দৈর্ঘ্য += STRLEN(p_enc) + 2;

    যদি (save_দৈর্ঘ্য < *দৈর্ঘ্য || save_দৈর্ঘ্য / 2 >= *দৈর্ঘ্য)
	*মান = XtRealloc((অক্ষর *)save_ফলাফল, (Cardinal)*দৈর্ঘ্য + 1);
    অন্যথা
	*মান = save_ফলাফল;
    যদি (*মান == ফাঁকা)
    {
	vim_free(string);
	ফেরত False;
    }
    save_ফলাফল = (char_u *)*মান;
    save_দৈর্ঘ্য = *দৈর্ঘ্য;

    যদি (*target == XA_STRING || (*target == utf8_atom && enc_utf8))
    {
	mch_memmove(save_ফলাফল, string, (size_t)(*দৈর্ঘ্য));
	*type = *target;
    }
    অন্যথা যদি (*target == compound_text_atom || *target == text_atom)
    {
	XTextProperty	text_prop;
	অক্ষর		*string_nt = (অক্ষর *)save_ফলাফল;
	পূর্ণসংখ্যা		conv_ফলাফল;

	// create NUL terminated string which XmbTextListToTextProperty wants
	mch_memmove(string_nt, string, (size_t)*দৈর্ঘ্য);
	string_nt[*দৈর্ঘ্য] = NUL;
	conv_ফলাফল = XmbTextListToTextProperty(X_DISPLAY, &string_nt,
					   1, XCompoundTextStyle, &text_prop);
	যদি (conv_ফলাফল != Success)
	{
	    vim_free(string);
	    ফেরত False;
	}
	*মান = (XtPointer)(text_prop.মান);	//    from plain text
	*দৈর্ঘ্য = text_prop.nitems;
	*type = compound_text_atom;
	XtFree((অক্ষর *)save_ফলাফল);
	save_ফলাফল = (char_u *)*মান;
	save_দৈর্ঘ্য = *দৈর্ঘ্য;
    }
    অন্যথা যদি (*target == vimenc_atom || *target == vimenc_mt_atom)
    {
	পূর্ণসংখ্যা l = STRLEN(p_enc);

	save_ফলাফল[0] = motion_type;
	STRCPY(save_ফলাফল + 1, p_enc);
	mch_memmove(save_ফলাফল + l + 2, string, (size_t)(*দৈর্ঘ্য - l - 2));
	যদি (*target == vimenc_atom)
	    *type = vimenc_atom;
	অন্যথা
	    *type = vimenc_mt_atom;
    }
    অন্যথা
    {
	save_ফলাফল[0] = motion_type;
	mch_memmove(save_ফলাফল + 1, string, (size_t)(*দৈর্ঘ্য - 1));
	যদি (*target == vim_atom)
	    *type = vim_atom;
	অন্যথা
	    *type = vim_mt_atom;
    }
    *format = 8;	    // 8 bits per char
    vim_free(string);
    ফেরত True;
}

    স্থির শূন্য
clip_x11_lose_ownership_cb(Widget w UNUSED, Atom *sel_atom)
{
    যদি (*sel_atom == clip_plus.sel_atom)
	clip_lose_selection(&clip_plus);
    অন্যথা
	clip_lose_selection(&clip_star);
}

    স্থির শূন্য
clip_x11_notify_cb(Widget w UNUSED, Atom *sel_atom UNUSED, Atom *target UNUSED)
{
    // To prevent automatically freeing the selection value.
}

/*
 * Property callback to get a timestamp for XtOwnSelection.
 */
#  যদি defined(FEAT_X11) && defined(FEAT_XCLIPBOARD)
    স্থির শূন্য
clip_x11_timestamp_cb(
    Widget	w,
    XtPointer	n UNUSED,
    XEvent	*event,
    Boolean	*cont UNUSED)
{
    Atom	    actual_type;
    পূর্ণসংখ্যা		    format;
    unsigned  long  nitems, bytes_after;
    unsigned অক্ষর   *prop=ফাঁকা;
    XPropertyEvent  *xproperty=&event->xproperty;

    // Must be a property notify, state can't be Delete (True), has to be
    // one of the supported selection types.
    যদি (event->type != PropertyNotify || xproperty->state
	    || (xproperty->atom != clip_star.sel_atom
				    && xproperty->atom != clip_plus.sel_atom))
	ফেরত;

    যদি (XGetWindowProperty(xproperty->display, xproperty->window,
	  xproperty->atom, 0, 0, False, timestamp_atom, &actual_type, &format,
						&nitems, &bytes_after, &prop))
	ফেরত;

    যদি (prop)
	XFree(prop);

    // Make sure the property type is "TIMESTAMP" and it's 32 bits.
    যদি (actual_type != timestamp_atom || format != 32)
	ফেরত;

    // Get the selection, using the event timestamp.
    যদি (XtOwnSelection(w, xproperty->atom, xproperty->time,
	    clip_x11_convert_selection_cb, clip_x11_lose_ownership_cb,
	    clip_x11_notify_cb) == OK)
    {
	// Set the "owned" flag now, there may have been a call to
	// lose_ownership_cb in between.
	যদি (xproperty->atom == clip_plus.sel_atom)
	    clip_plus.owned = TRUE;
	অন্যথা
	    clip_star.owned = TRUE;
    }
}

    শূন্য
x11_setup_selection(Widget w)
{
    XtAddEventHandler(w, PropertyChangeMask, False,
	    /*(XtEventHandler)*/clip_x11_timestamp_cb, (XtPointer)ফাঁকা);
}
#  endif

    স্থির শূন্য
clip_x11_request_selection_cb(
    Widget	w UNUSED,
    XtPointer	success,
    Atom	*sel_atom,
    Atom	*type,
    XtPointer	মান,
    long_u	*দৈর্ঘ্য,
    পূর্ণসংখ্যা		*format)
{
    পূর্ণসংখ্যা		motion_type = MAUTO;
    long_u	len;
    char_u	*p;
    অক্ষর	**text_list = ফাঁকা;
    Clipboard_T	*cbd;
    char_u	*tmpbuf = ফাঁকা;

    যদি (*sel_atom == clip_plus.sel_atom)
	cbd = &clip_plus;
    অন্যথা
	cbd = &clip_star;

    যদি (মান == ফাঁকা || *দৈর্ঘ্য == 0)
    {
	clip_free_selection(cbd);	// nothing received, clear register
	*(পূর্ণসংখ্যা *)success = FALSE;
	ফেরত;
    }
    p = (char_u *)মান;
    len = *দৈর্ঘ্য;
    যদি (*type == vim_atom || *type == vim_mt_atom)
    {
	motion_type = *p++;
	len--;
    }

    অন্যথা যদি (*type == vimenc_atom || *type == vimenc_mt_atom)
    {
	char_u		*enc;
	vimconv_T	conv;
	পূর্ণসংখ্যা		convlen;

	motion_type = *p++;
	--len;

	enc = p;
	p += STRLEN(p) + 1;
	len -= p - enc;

	// If the encoding of the text is different from 'encoding', attempt
	// converting it.
	conv.vc_type = CONV_NONE;
	convert_setup(&conv, enc, p_enc);
	যদি (conv.vc_type != CONV_NONE)
	{
	    convlen = len;	// Need to use an int here.
	    tmpbuf = string_convert(&conv, p, &convlen);
	    len = convlen;
	    যদি (tmpbuf != ফাঁকা)
		p = tmpbuf;
	    convert_setup(&conv, ফাঁকা, ফাঁকা);
	}
    }

    অন্যথা যদি (*type == compound_text_atom
	    || *type == utf8_atom
	    || (enc_dbcs != 0 && *type == text_atom))
    {
	XTextProperty	text_prop;
	পূর্ণসংখ্যা		n_text = 0;
	পূর্ণসংখ্যা		status;

	text_prop.মান = (unsigned অক্ষর *)মান;
	text_prop.encoding = *type;
	text_prop.format = *format;
	text_prop.nitems = len;
#  যদি defined(X_HAVE_UTF8_STRING)
	যদি (*type == utf8_atom)
	    status = Xutf8TextPropertyToTextList(X_DISPLAY, &text_prop,
							 &text_list, &n_text);
	অন্যথা
#  endif
	    status = XmbTextPropertyToTextList(X_DISPLAY, &text_prop,
							 &text_list, &n_text);
	যদি (status != Success || n_text < 1)
	{
	    *(পূর্ণসংখ্যা *)success = FALSE;
	    ফেরত;
	}
	p = (char_u *)text_list[0];
	len = STRLEN(p);
    }
    clip_yank_selection(motion_type, p, (long)len, cbd);

    যদি (text_list != ফাঁকা)
	XFreeStringList(text_list);
    vim_free(tmpbuf);
    XtFree((অক্ষর *)মান);
    *(পূর্ণসংখ্যা *)success = TRUE;
}

    শূন্য
clip_x11_request_selection(
    Widget	myShell,
    Display	*dpy,
    Clipboard_T	*cbd)
{
    XEvent	event;
    Atom	type;
    স্থির পূর্ণসংখ্যা	success;
    পূর্ণসংখ্যা		i;
    time_t	start_time;
    পূর্ণসংখ্যা		timed_out = FALSE;

    জন্য (i = 0; i < 8; i++)
    {
	নির্বাচন (i)
	{
	    ক্ষেত্র 0:  type = vimenc_mt_atom;	বিরতি;
	    ক্ষেত্র 1:  type = vimenc_atom;	বিরতি;
	    ক্ষেত্র 2:  type = vim_mt_atom;	বিরতি;
	    ক্ষেত্র 3:  type = vim_atom;		বিরতি;
	    ক্ষেত্র 4:  type = utf8_atom;		বিরতি;
	    ক্ষেত্র 5:  type = compound_text_atom; বিরতি;
	    ক্ষেত্র 6:  type = text_atom;		বিরতি;
	    পূর্বনির্ধারিত: type = XA_STRING;
	}
	যদি (type == utf8_atom
#  যদি defined(X_HAVE_UTF8_STRING)
		&& !enc_utf8
#  endif
		)
	    // Only request utf-8 when 'encoding' is utf8 and
	    // Xutf8TextPropertyToTextList is available.
	    চালু;
	success = MAYBE;
	Xt_Get_Selection_মান(myShell, cbd->sel_atom, type,
	    clip_x11_request_selection_cb, (XtPointer)&success, CurrentTime);

	// Make sure the request for the selection goes out before waiting for
	// a response.
	XFlush(dpy);

	/*
	 * Wait for result of selection request, otherwise if we type more
	 * characters, then they will appear before the one that requested the
	 * paste!  Don't worry, we will catch up with any other events later.
	 */
	start_time = time(ফাঁকা);
	যখন (success == MAYBE)
	{
	    যদি (XCheckTypedEvent(dpy, PropertyNotify, &event)
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
		চালু;
	    }

	    // Time out after 2 to 3 seconds to avoid that we hang when the
	    // other process doesn't respond.  Note that the SelectionNotify
	    // event may still come later when the selection owner comes back
	    // to life and the text gets inserted unexpectedly.  Don't know
	    // why that happens or how to avoid that :-(.
	    যদি (time(ফাঁকা) > start_time + 2)
	    {
		timed_out = TRUE;
		বিরতি;
	    }

	    // Do we need this?  Probably not.
	    XSync(dpy, False);

	    // Wait for 1 msec to avoid that we eat up all CPU time.
	    ui_delay(1L, TRUE);
	}

	যদি (success == TRUE)
	    ফেরত;

	// don't do a retry with another type after timing out, otherwise we
	// hang for 15 seconds.
	যদি (timed_out)
	    বিরতি;
    }

    // Final fallback position - use the X CUT_BUFFER0 store
    yank_cut_buffer0(dpy, cbd);
}

    শূন্য
clip_x11_lose_selection(Widget myShell, Clipboard_T *cbd)
{
    XtDisownSelection(myShell, cbd->sel_atom,
				XtLastTimestampProcessed(XtDisplay(myShell)));
}

    পূর্ণসংখ্যা
clip_x11_own_selection(Widget myShell, Clipboard_T *cbd)
{
    // When using the GUI we have proper timestamps, use the one of the last
    // event.  When in the console we don't get events (the terminal gets
    // them), Get the time by a zero-length append, clip_x11_timestamp_cb will
    // be called with the current timestamp.
#  ifdef FEAT_GUI
    যদি (gui.in_use)
    {
	যদি (XtOwnSelection(myShell, cbd->sel_atom,
	       XtLastTimestampProcessed(XtDisplay(myShell)),
	       clip_x11_convert_selection_cb, clip_x11_lose_ownership_cb,
	       clip_x11_notify_cb) == False)
	    ফেরত FAIL;
    }
    অন্যথা
#  endif
    {
	যদি (!XChangeProperty(XtDisplay(myShell), XtWindow(myShell),
		  cbd->sel_atom, timestamp_atom, 32, PropModeAppend, ফাঁকা, 0))
	    ফেরত FAIL;
    }
    // Flush is required in a terminal as nothing else is doing it.
    XFlush(XtDisplay(myShell));
    ফেরত OK;
}

/*
 * Send the current selection to the clipboard.  Do nothing for X because we
 * will fill in the selection only when requested by another app.
 */
    শূন্য
clip_x11_set_selection(Clipboard_T *cbd UNUSED)
{
}

# endif

# যদি (defined(FEAT_XCLIPBOARD) || defined(FEAT_GUI_X11) || defined(FEAT_GUI_GTK)) \
	&& !defined(USE_GTK4)
/*
 * Get the contents of the X CUT_BUFFER0 and put it in "cbd".
 */
    শূন্য
yank_cut_buffer0(Display *dpy, Clipboard_T *cbd)
{
    পূর্ণসংখ্যা		nbytes = 0;
    char_u	*বাফার = (char_u *)XFetch_বাফার(dpy, &nbytes, 0);

    যদি (nbytes > 0)
    {
	পূর্ণসংখ্যা  done = FALSE;

	// CUT_BUFFER0 is supposed to be always latin1.  Convert to 'enc' when
	// using a multi-byte encoding.  Conversion between two 8-bit
	// character sets usually fails and the text might actually be in
	// 'enc' anyway.
	যদি (has_mbyte)
	{
	    char_u	*conv_buf;
	    vimconv_T	vc;

	    vc.vc_type = CONV_NONE;
	    যদি (convert_setup(&vc, (char_u *)"latin1", p_enc) == OK)
	    {
		conv_buf = string_convert(&vc, বাফার, &nbytes);
		যদি (conv_buf != ফাঁকা)
		{
		    clip_yank_selection(MCHAR, conv_buf, (long)nbytes, cbd);
		    vim_free(conv_buf);
		    done = TRUE;
		}
		convert_setup(&vc, ফাঁকা, ফাঁকা);
	    }
	}
	যদি (!done)  // use the text without conversion
	    clip_yank_selection(MCHAR, বাফার, (long)nbytes, cbd);
	XFree((শূন্য *)বাফার);
	যদি (p_verbose > 0)
	{
	    verbose_enter();
	    verb_বার্তা(_("Used CUT_BUFFER0 instead of empty selection"));
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
# যদি defined(FEAT_X11) && defined(FEAT_CLIPBOARD)
    শূন্য
x11_export_final_selection(শূন্য)
{
    Display	*dpy;
    char_u	*str = ফাঁকা;
    long_u	len = 0;
    পূর্ণসংখ্যা		motion_type = -1;

#  ifdef FEAT_GUI
    যদি (gui.in_use)
	dpy = X_DISPLAY;
    অন্যথা
#  endif
#  ifdef FEAT_XCLIPBOARD
	dpy = xterm_dpy;
#  অন্যথা
	ফেরত;
#  endif

    // Get selection to export
    যদি (clip_plus.owned)
	motion_type = clip_convert_selection(&str, &len, &clip_plus);
    অন্যথা যদি (clip_star.owned)
	motion_type = clip_convert_selection(&str, &len, &clip_star);

    // Check it's OK
    যদি (dpy != ফাঁকা && str != ফাঁকা && motion_type >= 0
					       && len < 1024*1024 && len > 0)
    {
	পূর্ণসংখ্যা ok = TRUE;

	// The CUT_BUFFER0 is supposed to always contain latin1.  Convert from
	// 'enc' when it is a multi-byte encoding.  When 'enc' is an 8-bit
	// encoding conversion usually doesn't work, so keep the text as-is.
	যদি (has_mbyte)
	{
	    vimconv_T	vc;

	    vc.vc_type = CONV_NONE;
	    যদি (convert_setup(&vc, p_enc, (char_u *)"latin1") == OK)
	    {
		পূর্ণসংখ্যা	intlen = len;
		char_u	*conv_str;

		vc.vc_fail = TRUE;
		conv_str = string_convert(&vc, str, &intlen);
		len = intlen;
		যদি (conv_str != ফাঁকা)
		{
		    vim_free(str);
		    str = conv_str;
		}
		অন্যথা
		{
		    ok = FALSE;
		}
		convert_setup(&vc, ফাঁকা, ফাঁকা);
	    }
	    অন্যথা
	    {
		ok = FALSE;
	    }
	}

	// Do not store the string if conversion failed.  Better to use any
	// other selection than garbled text.
	যদি (ok)
	{
	    XStore_বাফার(dpy, (অক্ষর *)str, (পূর্ণসংখ্যা)len, 0);
	    XFlush(dpy);
	}
    }

    vim_free(str);
}
# endif

    শূন্য
clip_free_selection(Clipboard_T *cbd)
{
    yankreg_T *y_ptr = get_y_current();

    যদি (cbd == &clip_plus)
	set_y_current(get_y_register(PLUS_REGISTER));
    অন্যথা
	set_y_current(get_y_register(STAR_REGISTER));
    free_yank_all();
    get_y_current()->y_আকার = 0;
    set_y_current(y_ptr);
}

/*
 * Get the selected text and put it in register '*' or '+'.
 */
    শূন্য
clip_get_selection(Clipboard_T *cbd)
{
    yankreg_T	*old_y_previous, *old_y_current;
    pos_T	old_cursor;
    pos_T	old_visual;
    পূর্ণসংখ্যা		old_visual_mode;
    colnr_T	old_virtcol;
    colnr_T	old_curswant;
    পূর্ণসংখ্যা		old_set_curswant;
    pos_T	old_op_start, old_op_end;
    oparg_T	oa;
    cmdarg_T	ca;

    যদি (cbd->owned)
    {
	যদি ((cbd == &clip_plus
		&& get_y_register(PLUS_REGISTER)->y_array != ফাঁকা)
		|| (cbd == &clip_star
		    && get_y_register(STAR_REGISTER)->y_array != ফাঁকা))
	    ফেরত;

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
    অন্যথা যদি (!is_clipboard_needs_update())
    {
	clip_free_selection(cbd);

	// Try to get selected text from another window
	clip_gen_request_selection(cbd);
    }
}

/*
 * Convert from the GUI selection string into the '*'/'+' register.
 */
    শূন্য
clip_yank_selection(
    পূর্ণসংখ্যা		type,
    char_u	*str,
    long	len,
    Clipboard_T *cbd)
{
    yankreg_T *y_ptr;

    যদি (cbd == &clip_plus)
	y_ptr = get_y_register(PLUS_REGISTER);
    অন্যথা
	y_ptr = get_y_register(STAR_REGISTER);

    clip_free_selection(cbd);

    str_to_reg(y_ptr, type, str, len, -1, FALSE);
}

    পূর্ণসংখ্যা
clip_convert_selection_offset(
	char_u	    **str,
	long_u	    *len,
	পূর্ণসংখ্যা	    offset, // Extra space to add in *str and the offset to
			    // place the actual string in *str.
	Clipboard_T *cbd)
{
    char_u	*p;
    পূর্ণসংখ্যা		lnum;
    পূর্ণসংখ্যা		i, j;
    int_u	eolsize;
    yankreg_T	*y_ptr;

    যদি (cbd == &clip_plus)
	y_ptr = get_y_register(PLUS_REGISTER);
    অন্যথা
	y_ptr = get_y_register(STAR_REGISTER);

# ifdef USE_CRNL
    eolsize = 2;
# অন্যথা
    eolsize = 1;
# endif

    *str = ফাঁকা;
    *len = 0;
    যদি (y_ptr->y_array == ফাঁকা)
	ফেরত -1;

    জন্য (i = 0; i < y_ptr->y_আকার; i++)
	*len += (long_u)y_ptr->y_array[i].দৈর্ঘ্য + eolsize;

    // Don't want newline character at end of last line if we're in MCHAR mode.
    যদি (y_ptr->y_type == MCHAR && *len >= eolsize)
	*len -= eolsize;

    *len += offset;
    p = *str = alloc(*len + 1);	// add one to avoid zero
    যদি (p == ফাঁকা)
	ফেরত -1;
    p += offset;
    lnum = 0;
    জন্য (i = 0, j = 0; i < (পূর্ণসংখ্যা)*len - offset; i++, j++)
    {
	যদি (y_ptr->y_array[lnum].string[j] == '\n')
	    p[i] = NUL;
	অন্যথা যদি (y_ptr->y_array[lnum].string[j] == NUL)
	{
# ifdef USE_CRNL
	    p[i++] = '\r';
# endif
	    p[i] = '\n';
	    lnum++;
	    j = -1;
	}
	অন্যথা
	    p[i] = y_ptr->y_array[lnum].string[j];
    }
    ফেরত y_ptr->y_type;
}

/*
 * Convert the '*'/'+' register into a GUI selection string returned in *str
 * with length *len.
 * Returns the motion type, or -1 for failure.
 */
    পূর্ণসংখ্যা
clip_convert_selection(char_u **str, long_u *len, Clipboard_T *cbd)
{
    ফেরত clip_convert_selection_offset(str, len, 0, cbd);
}

/*
 * When "regname" is a clipboard register, obtain the selection.  If it's not
 * available return zero, otherwise return "regname".
 */
    পূর্ণসংখ্যা
may_get_selection(পূর্ণসংখ্যা regname)
{
    যদি (regname == '*')
    {
	যদি (!clip_star.available)
	    regname = 0;
	অন্যথা
	    clip_get_selection(&clip_star);
    }
    অন্যথা যদি (regname == '+')
    {
	যদি (!clip_plus.available)
	    regname = 0;
	অন্যথা
	    clip_get_selection(&clip_plus);
    }
    ফেরত regname;
}

/*
 * If we have written to a clipboard register, send the text to the clipboard.
 */
    শূন্য
may_set_selection(শূন্য)
{
    যদি ((get_y_current() == get_y_register(STAR_REGISTER))
	    && clip_star.available)
    {
	clip_own_selection(&clip_star);
	clip_gen_set_selection(&clip_star);
    }
    অন্যথা যদি ((get_y_current() == get_y_register(PLUS_REGISTER))
	    && clip_plus.available)
    {
	clip_own_selection(&clip_plus);
	clip_gen_set_selection(&clip_plus);
    }
}

# যদি defined(FEAT_WAYLAND_CLIPBOARD)

    স্থির clip_wl_selection_T *
clip_wl_get_selection(wayland_selection_T sel)
{
    নির্বাচন (sel)
    {
	ক্ষেত্র WAYLAND_SELECTION_REGULAR:
	    ফেরত &clip_wl.regular;
	ক্ষেত্র WAYLAND_SELECTION_PRIMARY:
	    ফেরত &clip_wl.primary;
	পূর্বনির্ধারিত:
	    ফেরত ফাঁকা;
    }
}

    স্থির clip_wl_selection_T *
clip_wl_get_selection_from_cbd(Clipboard_T *cbd)
{
    যদি (cbd == &clip_plus)
	ফেরত &clip_wl.regular;
    অন্যথা যদি (cbd == &clip_star)
	ফেরত &clip_wl.primary;
    অন্যথা
	ফেরত ফাঁকা;
}

    স্থির Clipboard_T *
clip_wl_get_cbd_from_selection(clip_wl_selection_T *sel)
{
    যদি (sel == &clip_wl.regular)
	ফেরত &clip_plus;
    অন্যথা যদি (sel == &clip_wl.primary)
	ফেরত &clip_star;
    অন্যথা
	ফেরত ফাঁকা;
}

    স্থির wayland_selection_T
clip_wl_get_selection_type(clip_wl_selection_T *sel)
{
    যদি (sel == &clip_wl.regular)
	ফেরত WAYLAND_SELECTION_REGULAR;
    অন্যথা যদি (sel == &clip_wl.primary)
	ফেরত WAYLAND_SELECTION_PRIMARY;
    অন্যথা
	ফেরত WAYLAND_SELECTION_NONE;
}

    স্থির bool
wl_তথ্য_offer_listener_event_offer(
    শূন্য *তথ্য UNUSED,
    vwl_তথ্য_offer_T *offer UNUSED,
    ধ্রুবক অক্ষর *mime_type
)
{
    // Only accept mime type if we support it
    জন্য (পূর্ণসংখ্যা i = 0; i < (পূর্ণসংখ্যা)ARRAY_দৈর্ঘ্য(supported_mimes); i++)
	যদি (STRCMP(mime_type, supported_mimes[i]) == 0)
	    ফেরত true;
    ফেরত FALSE;
}

স্থির ধ্রুবক vwl_তথ্য_offer_listener_T vwl_তথ্য_offer_listener = {
    .offer = wl_তথ্য_offer_listener_event_offer
};

    স্থির শূন্য
vwl_তথ্য_device_listener_event_তথ্য_offer(
	শূন্য *তথ্য UNUSED,
	vwl_তথ্য_device_T *device UNUSED,
	vwl_তথ্য_offer_T *offer)
{
    // Immediately start listening for offer events from the data offer
    vwl_তথ্য_offer_add_listener(offer, &vwl_তথ্য_offer_listener, ফাঁকা);
}

    স্থির শূন্য
vwl_তথ্য_device_listener_event_selection(
	শূন্য *তথ্য UNUSED,
	vwl_তথ্য_device_T *device UNUSED,
	vwl_তথ্য_offer_T *offer,
	wayland_selection_T selection)
{
    clip_wl_selection_T *sel = clip_wl_get_selection(selection);

    // Destroy previous offer if any, it is now invalid
    vwl_তথ্য_offer_destroy(sel->offer);

    // There are two cases when sel->offer is NULL
    // 1. No one owns the selection
    // 2. We own the selection (we'll just access the register directly)
    যদি (offer == ফাঁকা || sel->source != ফাঁকা)
    {
	// Selection event is from us, so we are the source client. Therefore
	// ignore it. Or the selection is cleared, so set sel->offer to NULL
	vwl_তথ্য_offer_destroy(offer);
	sel->offer = ফাঁকা;
	ফেরত;
    }

    // Save offer. When we want to request data, then we'll actually call the
    // receive method.
    sel->offer = offer;

}

    স্থির শূন্য
vwl_তথ্য_device_listener_event_finished(
	শূন্য *তথ্য UNUSED,
	vwl_তথ্য_device_T *device)
{
    clip_wl_selection_T *sel;
    // Device finished, guessing this can happen is when the seat becomes
    // invalid? If so, let the user call :wlrestore! to reset. There wouldn't be
    // any point in trying to create another data device for the same seat,
    // since the seat is in an invalid state.
    যদি (device == clip_wl.regular.device)
    {
	sel = &clip_wl.regular;
	clip_wl.regular.device = ফাঁকা;
    }
    অন্যথা যদি (device == clip_wl.primary.device)
    {
	sel = &clip_wl.primary;
	clip_wl.primary.device = ফাঁকা;
    }
    অন্যথা
	// Shouldn't happen
	ফেরত;

    vim_free(sel->contents);
    vwl_তথ্য_source_destroy(sel->source);
    vwl_তথ্য_offer_destroy(sel->offer);
    sel->available = FALSE;

    vwl_তথ্য_device_destroy(device);
}

স্থির ধ্রুবক vwl_তথ্য_device_listener_T vwl_তথ্য_device_listener = {
    .তথ্য_offer = vwl_তথ্য_device_listener_event_তথ্য_offer,
    .selection = vwl_তথ্য_device_listener_event_selection,
    .finished = vwl_তথ্য_device_listener_event_finished
};

/*
 * Initialize the clipboard for Wayland using the global Wayland connection.
 * Returns OK on success and FAIL on failure.
 */
    পূর্ণসংখ্যা
clip_init_wayland(শূন্য)
{
    int_u supported = WAYLAND_SELECTION_NONE;

    যদি (wayland_ct == ফাঁকা)
	ফেরত FAIL;

    memset(&clip_wl, 0, আকার(clip_wl));

    clip_wl.seat = vwl_connection_get_seat(wayland_ct, (অক্ষর *)p_wse);

    যদি (clip_wl.seat == ফাঁকা)
	ফেরত FAIL;

    clip_wl.regular.manager = vwl_connection_get_তথ্য_device_manager(
	    wayland_ct, WAYLAND_SELECTION_REGULAR, &supported);

    যদি (clip_wl.regular.manager != ফাঁকা)
    {
	clip_wl.regular.device = vwl_তথ্য_device_manager_get_তথ্য_device(
		clip_wl.regular.manager, clip_wl.seat);

	যদি (clip_wl.regular.device != ফাঁকা)
	    clip_wl.regular.available = true;
	অন্যথা
	{
	    // Shouldn't happen
	    vwl_তথ্য_device_manager_discard(clip_wl.regular.manager);
	    clip_wl.regular.manager = ফাঁকা;
	    ফেরত FAIL;
	}
    }

    // If we still don't support the primary selection, try finding one for it
    // specifically.
    যদি (!(supported & WAYLAND_SELECTION_PRIMARY))
    {
	clip_wl.primary.manager = vwl_connection_get_তথ্য_device_manager(
		wayland_ct, WAYLAND_SELECTION_PRIMARY, &supported);

	যদি (clip_wl.primary.manager != ফাঁকা)
	{
	    clip_wl.primary.device = vwl_তথ্য_device_manager_get_তথ্য_device(
		    clip_wl.primary.manager, clip_wl.seat);

	    যদি (clip_wl.primary.device != ফাঁকা)
		clip_wl.primary.available = true;
	    অন্যথা
	    {
		vwl_তথ্য_device_manager_discard(clip_wl.primary.manager);
		clip_wl.primary.manager = ফাঁকা;
	    }
	}
    }

    যদি (clip_wl.regular.available && !clip_wl.primary.available)
    {
	// The protocol supports both regular and primary selections, just use
	// one data device manager and one data device. Or the primary selection
	// is not supported, make it point to the regular selection instead.
	clip_wl.primary.available = true;
	clip_wl.primary.manager = clip_wl.regular.manager;
	clip_wl.primary.device = clip_wl.regular.device;
    }

    যদি (!clip_wl.regular.available && !clip_wl.primary.available)
	ফেরত FAIL;

    // Start listening for selection updates
    যদি (clip_wl.regular.device != ফাঁকা)
	vwl_তথ্য_device_add_listener(clip_wl.regular.device,
		&vwl_তথ্য_device_listener, ফাঁকা);
    // Don't want to listen to the same data device twice
    যদি (clip_wl.primary.device != ফাঁকা
	    && clip_wl.primary.device != clip_wl.regular.device)
	vwl_তথ্য_device_add_listener(clip_wl.primary.device,
		&vwl_তথ্য_device_listener, ফাঁকা);

    ফেরত OK;
}

    শূন্য
clip_uninit_wayland(শূন্য)
{
    clip_wl_selection_T *sel;

    যদি (clipmethod == CLIPMETHOD_WAYLAND)
    {
	যদি (clip_star.owned)
	    clip_lose_selection(&clip_star);
	যদি (clip_plus.owned)
	    clip_lose_selection(&clip_plus);
    }

    // Don't want to double free
    যদি (clip_wl.regular.manager != clip_wl.primary.manager)
	vwl_তথ্য_device_manager_discard(clip_wl.primary.manager);
    vwl_তথ্য_device_manager_discard(clip_wl.regular.manager);

    যদি (clip_wl.regular.device != clip_wl.primary.device)
	vwl_তথ্য_device_destroy(clip_wl.primary.device);
    vwl_তথ্য_device_destroy(clip_wl.regular.device);

    sel = &clip_wl.regular;
    যখন (true)
    {
	vim_free(sel->contents);
	vwl_তথ্য_source_destroy(sel->source);
	vwl_তথ্য_offer_destroy(sel->offer);
	sel->available = false;

	যদি (sel == &clip_wl.primary)
	    বিরতি;
	sel = &clip_wl.primary;
    }

    vim_memset(&clip_wl, 0, আকার(clip_wl));
}

    পূর্ণসংখ্যা
clip_reset_wayland(শূন্য)
{
    wayland_uninit_connection();

    যদি (wayland_init_connection(wayland_display_name) == FAIL
	    || clip_init_wayland() == FAIL)
	ফেরত FAIL;

    choose_clipmethod();
    ফেরত OK;
}


/*
 * Read data from a file descriptor and write it to the given clipboard.
 */
    স্থির শূন্য
clip_wl_receive_তথ্য(Clipboard_T *cbd, ধ্রুবক অক্ষর *mime_type, পূর্ণসংখ্যা fd)
{
    char_u	*start, *final;
    long	len;
    garray_T	buf;
    পূর্ণসংখ্যা		motion_type = MAUTO;
    ssize_t	r = 0;
    char_u	*tofree = ফাঁকা;
#  ifndef HAVE_SELECT
    কাঠামো pollfd   pfd;

    pfd.fd = fd;
    pfd.events = POLLIN;
#  অন্যথা
    fd_set rfds;
    কাঠামো timeval  tv;

    FD_ZERO(&rfds);
    FD_SET(fd, &rfds);
#  endif

    // Make pipe (read end) non-blocking
    যদি (fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK) == -1)
	ফেরত;

    ga_init2(&buf, 1, 4096);

    // 4096 bytes seems reasonable for initial buffer size, memory is cheap
    // anyways.
    যদি (ga_grow(&buf, 4096) == FAIL)
	ফেরত;

    start = buf.ga_তথ্য;

#  ifndef HAVE_SELECT
    যখন (poll(&pfd, 1, p_wtm) > 0)
#  অন্যথা
    যখন (tv.tv_sec = p_wtm / 1000, tv.tv_usec = (p_wtm % 1000) * 1000,
	    select(fd + 1, &rfds, ফাঁকা, ফাঁকা, &tv) > 0)
#  endif
    {
	r = পঠ(fd, start, buf.ga_maxlen - 1 - buf.ga_len);

	যদি (r == 0)
	    বিরতি;
	অন্যথা যদি (r < 0)
	{
	    যদি (errno == EAGAIN || errno == EINTR)
		চালু;
	    বিরতি;
	}

	start += r;
	buf.ga_len += r;

	// Realloc if we are at the end of the buffer
	যদি (buf.ga_len >= buf.ga_maxlen - 1)
	{
	    যদি (ga_grow(&buf, 8192) == FAIL)
		বিরতি;
	    start = (char_u *)buf.ga_তথ্য + buf.ga_len;
	}
    }

    যদি (buf.ga_len == 0)
    {
	clip_free_selection(cbd); // Nothing received, clear register
	ga_clear(&buf);
	ফেরত;
    }

    final = buf.ga_তথ্য;
    len = buf.ga_len;

    যদি (clip_convert_তথ্য(&final, &len, &motion_type,
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
    স্থির শূন্য
clip_wl_request_selection(Clipboard_T *cbd)
{
    clip_wl_selection_T *sel = clip_wl_get_selection_from_cbd(cbd);
    পূর্ণসংখ্যা			fds[2];
    পূর্ণসংখ্যা			mime_types_len;
    ধ্রুবক অক্ষর		**mime_types;
    ধ্রুবক অক্ষর		*chosen_mime = ফাঁকা;

    যদি (!sel->available)
	যাও clear;

    // Dispatch any events that still queued up before checking for a data
    // offer.
    যদি (vwl_connection_roundtrip(wayland_ct) == FAIL)
	যাও clear;

    যদি (sel->offer == ফাঁকা)
	যাও clear;

    mime_types_len = sel->offer->mime_types.ga_len;
    mime_types = sel->offer->mime_types.ga_তথ্য;

    // Choose mime type to receive from. Mime types with a lower index in the
    // "supported_mimes" array are prioritized over ones after it.
    জন্য (পূর্ণসংখ্যা i = 0; i < (পূর্ণসংখ্যা)ARRAY_দৈর্ঘ্য(supported_mimes)
	    && chosen_mime == ফাঁকা; i++)
    {
	জন্য (পূর্ণসংখ্যা k = 0; k < mime_types_len && chosen_mime == ফাঁকা; k++)
	    যদি (STRCMP(mime_types[k], supported_mimes[i]) == 0)
		chosen_mime = supported_mimes[i];
    }

    যদি (chosen_mime == ফাঁকা || pipe(fds) == -1)
	যাও clear;

    vwl_তথ্য_offer_receive(sel->offer, chosen_mime, fds[1]);

    close(fds[1]); // Close before we read data so that when the source client
		   // closes their end we receive an EOF.

    যদি (vwl_connection_flush(wayland_ct) >= 0)
	clip_wl_receive_তথ্য(cbd, chosen_mime, fds[0]);

    close(fds[0]);

    ফেরত;
clear:
    clip_free_selection(cbd);
}

    স্থির শূন্য
vwl_তথ্য_source_listener_event_send(
    শূন্য *তথ্য,
    vwl_তথ্য_source_T *source UNUSED,
    ধ্রুবক অক্ষর *mime_type,
    int32_t fd
)
{
    clip_wl_selection_T *sel = তথ্য;
    Clipboard_T		*cbd = clip_wl_get_cbd_from_selection(sel);
    bool		have_mime = false;
    পূর্ণসংখ্যা			motion_type;
    long_u		দৈর্ঘ্য;
    char_u		*string; // Will be reallocated to a bigger size if
				 // needed.
    পূর্ণসংখ্যা			offset = 0;
    bool		is_vim, is_vimenc;
    size_t		যোগ = 0;
#  ifndef HAVE_SELECT
    কাঠামো pollfd   pfd;

    pfd.fd = fd;
    pfd.events = POLLOUT;
#  অন্যথা
    fd_set	    wfds;
    কাঠামো timeval  tv;

    FD_ZERO(&wfds);
    FD_SET(fd, &wfds);
#  endif

    // Check if we actually have mime type
    জন্য (পূর্ণসংখ্যা i = 0; i < (পূর্ণসংখ্যা)ARRAY_দৈর্ঘ্য(supported_mimes); i++)
	যদি (STRCMP(supported_mimes[i], mime_type) == 0)
	{
	    have_mime = true;
	    বিরতি;
	}

    যদি (!have_mime)
	যাও exit;

    // First byte sent is motion type for vim specific formats. For the vimenc
    // format, after the first byte is the encoding type, which is null
    // terminated.

    is_vimenc = STRCMP(mime_type, VIMENC_ATOM_NAME) == 0
	|| STRCMP(mime_type, VIMENC_MIMETYPE_NAME) == 0;
    is_vim = STRCMP(mime_type, VIM_ATOM_NAME) == 0
	|| STRCMP(mime_type, VIM_MIMETYPE_NAME) == 0;

    যদি (is_vimenc)
	offset += 2 + STRLEN(p_enc);
    অন্যথা যদি (is_vim)
	offset += 1;

    clip_get_selection(cbd);
    motion_type = clip_convert_selection_offset(&string, &দৈর্ঘ্য, offset, cbd);

    যদি (motion_type < 0)
	যাও exit;

    যদি (is_vimenc)
    {
	string[0] = (char_u)motion_type;
	// Use vim_strncpy for safer copying
	vim_strncpy(string + 1, p_enc, STRLEN(p_enc));
    }
    অন্যথা যদি (is_vim)
	string[0] = (char_u)motion_type;


    যখন (যোগ < (size_t)দৈর্ঘ্য &&
#  ifndef HAVE_SELECT
	    poll(&pfd, 1, p_wtm) > 0)
#  অন্যথা
	    ((tv.tv_sec = p_wtm / 1000, tv.tv_usec = (p_wtm % 1000) * 1000),
	    select(fd + 1, ফাঁকা, &wfds, ফাঁকা, &tv) > 0))
#  endif
    {
	ssize_t w = লিখ(fd, string + যোগ, দৈর্ঘ্য - যোগ);

	যদি (w == -1)
	    বিরতি;
	যোগ += w;
    }

    vim_free(string);
exit:
    close(fd);
}

    স্থির শূন্য
vwl_তথ্য_source_listener_event_cancelled(
	শূন্য *তথ্য,
	vwl_তথ্য_source_T *source UNUSED)
{
    clip_wl_selection_T *sel = তথ্য;
    Clipboard_T		*cbd = clip_wl_get_cbd_from_selection(sel);

    clip_lose_selection(cbd);
}

স্থির ধ্রুবক vwl_তথ্য_source_listener_T vwl_তথ্য_source_listener = {
    .send = vwl_তথ্য_source_listener_event_send,
    .cancelled = vwl_তথ্য_source_listener_event_cancelled
};

    স্থির শূন্য
clip_wl_do_set_selection(শূন্য *তথ্য, uint32_t serial)
{
    clip_wl_selection_T *sel = তথ্য;
    wayland_selection_T sel_type = clip_wl_get_selection_type(sel);

    vwl_তথ্য_device_set_selection(sel->device, sel->source, serial, sel_type);

    sel->own_success = (vwl_connection_roundtrip(wayland_ct) == OK);
}

/*
 * Own the selection that cbd corresponds to. Start listening for requests from
 * other Wayland clients so they can receive data from us. Returns OK on success
 * and FAIL on failure.
 */
    স্থির পূর্ণসংখ্যা
clip_wl_own_selection(Clipboard_T *cbd)
{
    clip_wl_selection_T *sel = clip_wl_get_selection_from_cbd(cbd);
    wayland_selection_T sel_type = clip_wl_get_selection_type(sel);

    যদি (!sel->available || vwl_connection_roundtrip(wayland_ct) == FAIL)
	ফেরত FAIL;

    যদি (sel->source != ফাঁকা)
    {
	যদি (sel_type == WAYLAND_SELECTION_PRIMARY)
	    // We already own the selection, ignore (only do this for primary
	    // selection). We don't re set the selection because then we would
	    // be setting the selection every time the user moves the visual
	    // selection cursor, which is messy and inefficient. Some
	    // applications like Google Chrome do it this way however.
	    ফেরত OK;
	অন্যথা যদি (sel_type == WAYLAND_SELECTION_REGULAR)
	{
	    // Technically we don't need to do this as we already own the
	    // selection, however if a user yanks text a second time, the
	    // text yanked won't appear in their clipboard manager if they are
	    // using one.
	    //
	    // This can be unexpected behaviour for the user so its probably
	    // better to do it this way. Additionally other Wayland applications
	    // seem to set the selection every time.
	    vwl_তথ্য_source_destroy(sel->source);
	}
	অন্যথা
	    // Shouldn't happen
	    ফেরত FAIL;
    }

    sel->source = vwl_তথ্য_device_manager_create_তথ্য_source(sel->manager);
    vwl_তথ্য_source_add_listener(sel->source, &vwl_তথ্য_source_listener, sel);

    // Advertise mime types
    জন্য (পূর্ণসংখ্যা i = 0; i < (পূর্ণসংখ্যা)ARRAY_দৈর্ঘ্য(supported_mimes); i++)
	vwl_তথ্য_source_offer(sel->source, supported_mimes[i]);

    sel->own_success = false;
    clip_wl_do_set_selection(sel, 0);

    যদি (!sel->own_success)
	যাও fail;

    ফেরত OK;
fail:
    vwl_তথ্য_source_destroy(sel->source);
    sel->source = ফাঁকা;
    ফেরত FAIL;
}

/*
 * Disown the selection that cbd corresponds to.
 */
    স্থির শূন্য
clip_wl_lose_selection(Clipboard_T *cbd)
{
    clip_wl_selection_T *sel = clip_wl_get_selection_from_cbd(cbd);

    যদি (!sel->available)
	ফেরত;

    vwl_তথ্য_source_destroy(sel->source);
    sel->source = ফাঁকা;
}

/*
 * Send the current selection to the clipboard. Do nothing for Wayland because
 * we will fill in the selection only when requested by another client.
 */
    স্থির শূন্য
clip_wl_set_selection(Clipboard_T *cbd UNUSED)
{
}

#  যদি defined(USE_SYSTEM)
/*
 * Return true if we own the selection corresponding to cbd or another client
 * does.
 */
    স্থির bool
clip_wl_owner_exists(Clipboard_T *cbd)
{
    clip_wl_selection_T *sel = clip_wl_get_selection_from_cbd(cbd);

    যদি (vwl_connection_roundtrip(wayland_ct) == FAIL)
	ফেরত false;

    ফেরত sel->available && (sel->source != ফাঁকা || sel->offer != ফাঁকা);
}
#  endif

# endif // FEAT_WAYLAND_CLIPBOARD

#endif // FEAT_CLIPBOARD

#ifdef HAVE_CLIPMETHOD

/*
 * Returns the first method for accessing the clipboard that is available/works,
 * depending on the order of values in str.
 */
    স্থির clipmethod_T
get_clipmethod(char_u *str)
{
    পূর্ণসংখ্যা		len	= (পূর্ণসংখ্যা)STRLEN(str) + 1;
    char_u	*buf	= alloc(len);

    যদি (buf == ফাঁকা)
	ফেরত CLIPMETHOD_FAIL;

    clipmethod_T ret = CLIPMETHOD_FAIL;
    char_u	*p = str;

    যখন (*p != NUL)
    {
	clipmethod_T method = CLIPMETHOD_NONE;

	(শূন্য)copy_option_part(&p, buf, len, ",");

	যদি (STRCMP(buf, "wayland") == 0)
	{
# ifdef FEAT_GUI
	    যদি (!gui.in_use)
# endif
	    {
# ifdef FEAT_WAYLAND_CLIPBOARD
		যদি (clip_wl.regular.available || clip_wl.primary.available)
		    method = CLIPMETHOD_WAYLAND;
# endif
	    }
	}
	অন্যথা যদি (STRCMP(buf, "x11") == 0)
	{
# ifdef FEAT_GUI
	    যদি (!gui.in_use)
# endif
	    {
# ifdef FEAT_XCLIPBOARD
		// x_IOerror_handler() in os_unix.c should set xterm_dpy to NULL if
		// we lost connection to the X server.
		যদি (xterm_dpy != ফাঁকা)
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
	অন্যথা
	{
# ifdef FEAT_CLIPBOARD_PROVIDER
	    // Check if name matches a clipboard provider
	    পূর্ণসংখ্যা r = clip_provider_is_available(buf);

	    যদি (r == 1)
	    {
		method = CLIPMETHOD_PROVIDER;
		যদি (ret == CLIPMETHOD_FAIL)
		{
		    vim_free(clip_provider);
		    clip_provider = vim_strsave(buf);
		    যদি (clip_provider == ফাঁকা)
			যাও fail;
		}
	    }
	    অন্যথা যদি (r == -1)
# endif
	    {
# ifdef FEAT_CLIPBOARD_PROVIDER
fail:
# endif
		ret = CLIPMETHOD_FAIL;
		যাও exit;
	    }
	}

	// Keep on going in order to catch errors
	যদি (method != CLIPMETHOD_NONE && ret == CLIPMETHOD_FAIL)
	    ret = method;
    }

    // No match found, use "none".
    ret = (ret == CLIPMETHOD_FAIL) ? CLIPMETHOD_NONE : ret;

exit:
    vim_free(buf);
    ফেরত ret;
}


/*
 * Returns name of clipmethod in a statically allocated string.
 */
    স্থির char_u *
clipmethod_to_str(clipmethod_T method)
{
    নির্বাচন(method)
    {
	ক্ষেত্র CLIPMETHOD_WAYLAND:
	    ফেরত (char_u *)"wayland";
	ক্ষেত্র CLIPMETHOD_X11:
	    ফেরত (char_u *)"x11";
	ক্ষেত্র CLIPMETHOD_PROVIDER:
# ifdef FEAT_EVAL
	    ফেরত clip_provider;
# endif
	পূর্বনির্ধারিত:
	    ফেরত (char_u *)"none";
    }
}

/*
 * Sets the current clipmethod to use given by `get_clipmethod()`. Returns an
 * error message on failure else NULL.
 */
    অক্ষর *
choose_clipmethod(শূন্য)
{
    clipmethod_T method = get_clipmethod(p_cpm);

    যদি (method == CLIPMETHOD_FAIL)
	ফেরত e_invalid_argument;

// If GUI is running or we are not on a system with Wayland or X11, then always
// return CLIPMETHOD_NONE. System or GUI clipboard handling always overrides.
// This is unless a provider is being used.
# যদি defined(FEAT_XCLIPBOARD) || defined(FEAT_WAYLAND_CLIPBOARD)
#  যদি defined(FEAT_GUI)
    যদি (method != CLIPMETHOD_PROVIDER && gui.in_use)
    {
#   ifdef FEAT_WAYLAND
	// We only interact with Wayland for the clipboard, we can just deinit
	// everything.
	wayland_uninit_connection();
#   endif

	method = CLIPMETHOD_NONE;
	যাও lose_sel_exit;
    }
#  endif
# অন্যথা
    // If on a system like windows or macos, then clipmethod is irrelevant, we
    // use their way of accessing the clipboard. This is unless we are using the
    // clipboard provider
#  ifdef FEAT_CLIPBOARD_PROVIDER
    যদি (method != CLIPMETHOD_PROVIDER)
#  endif
    {
	method = CLIPMETHOD_NONE;
	যাও exit;
    }
# endif

# ifdef FEAT_CLIPBOARD
    // Deinitialize clipboard if there is no way to access clipboard
    যদি (method == CLIPMETHOD_NONE)
	clip_init(FALSE);
    // If we have a clipmethod that works now, then initialize clipboard
    অন্যথা যদি (clipmethod == CLIPMETHOD_NONE && method != CLIPMETHOD_NONE)
    {
	clip_init(TRUE);
	did_warn_clipboard = false;
    }
    // Disown clipboard if we are switching to a new method
    অন্যথা যদি (clipmethod != CLIPMETHOD_NONE && method != clipmethod)
    {
#  যদি (defined(FEAT_XCLIPBOARD) || defined(FEAT_WAYLAND_CLIPBOARD)) \
	&& defined(FEAT_GUI)
lose_sel_exit:
#  endif
	যদি (clip_star.owned)
	    clip_lose_selection(&clip_star);
	যদি (clip_plus.owned)
	    clip_lose_selection(&clip_plus);

#  যদি defined(FEAT_GUI)
	যদি (!gui.in_use)
#  endif
	{
	    clip_init(TRUE);
	    did_warn_clipboard = false;
	}
    }
# endif // FEAT_CLIPBOARD

# যদি !defined(FEAT_XCLIPBOARD) && !defined(FEAT_WAYLAND_CLIPBOARD)
exit:
# endif

    clipmethod = method;

# ifdef FEAT_EVAL
    set_vim_var_string(VV_CLIPMETHOD, (char_u*)clipmethod_to_str(method), -1);
# endif

    ফেরত ফাঁকা;
}

/*
 * Call choose_clipmethod().
 */
    শূন্য
ex_clipreset(exarg_T *eap UNUSED)
{
    clipmethod_T পূর্ব = clipmethod;

    choose_clipmethod();

    যদি (clipmethod == CLIPMETHOD_NONE)
	smsg(_("Could not find a way to access the clipboard."));
    অন্যথা যদি (clipmethod != পূর্ব)
	smsg(_("Switched to clipboard method '%s'."),
		clipmethod_to_str(clipmethod));
}

/*
 * Adjust the register name pointed to with "rp" for the clipboard being
 * used always and the clipboard being available.
 */
    শূন্য
adjust_clip_reg(পূর্ণসংখ্যা *rp)
{
# ifdef FEAT_CLIPBOARD_PROVIDER
    যদি (clipmethod == CLIPMETHOD_PROVIDER)
    {
	যদি (*rp == 0 && clip_unnamed != 0)
	    *rp = ((clip_unnamed & CLIP_UNNAMED_PLUS)) ? '+' : '*';
	ফেরত;
    }
# endif
# ifdef FEAT_CLIPBOARD
    // If no reg. specified, and "unnamed" or "unnamedplus" is in 'clipboard',
    // use '*' or '+' reg, respectively. "unnamedplus" prevails.
    যদি (*rp == 0 && (clip_unnamed != 0 || clip_unnamed_saved != 0))
    {
	যদি (clip_unnamed != 0)
	    *rp = ((clip_unnamed & CLIP_UNNAMED_PLUS) && clip_plus.available)
								  ? '+' : '*';
	অন্যথা
	    *rp = ((clip_unnamed_saved & CLIP_UNNAMED_PLUS)
					   && clip_plus.available) ? '+' : '*';
    }
    যদি ((!clip_star.available && *rp == '*') ||
	   (!clip_plus.available && *rp == '+'))
    {
	বার্তা_warn_missing_clipboard();
	*rp = 0;
    }
# endif
}

/*
 * Extract the items in the 'clipboard' option and set global values.
 * Return an error message or NULL for success.
 */
    অক্ষর *
did_set_clipboard(optset_T *args UNUSED)
{
    পূর্ণসংখ্যা		new_unnamed = 0;
# ifdef FEAT_CLIPBOARD
    পূর্ণসংখ্যা		new_autoselect_star = FALSE;
    পূর্ণসংখ্যা		new_autoselect_plus = FALSE;
    পূর্ণসংখ্যা		new_autoselectml = FALSE;
    পূর্ণসংখ্যা		new_html = FALSE;
# endif
    regprog_T	*new_exclude_prog = ফাঁকা;
    অক্ষর	*errmsg = ফাঁকা;
    char_u	*p;

    জন্য (p = p_cb; *p != NUL; )
    {
	// Note: Keep this in sync with p_cb_values.
	যদি (STRNCMP(p, "unnamed", 7) == 0 && (p[7] == ',' || p[7] == NUL))
	{
	    new_unnamed |= CLIP_UNNAMED;
	    p += 7;
	}
	অন্যথা যদি (STRNCMP(p, "unnamedplus", 11) == 0
					    && (p[11] == ',' || p[11] == NUL))
	{
	    new_unnamed |= CLIP_UNNAMED_PLUS;
	    p += 11;
	}
# ifdef FEAT_CLIPBOARD
	অন্যথা যদি (STRNCMP(p, "autoselect", 10) == 0
					    && (p[10] == ',' || p[10] == NUL))
	{
	    new_autoselect_star = TRUE;
	    p += 10;
	}
	অন্যথা যদি (STRNCMP(p, "autoselectplus", 14) == 0
					    && (p[14] == ',' || p[14] == NUL))
	{
	    new_autoselect_plus = TRUE;
	    p += 14;
	}
	অন্যথা যদি (STRNCMP(p, "autoselectml", 12) == 0
					    && (p[12] == ',' || p[12] == NUL))
	{
	    new_autoselectml = TRUE;
	    p += 12;
	}
	অন্যথা যদি (STRNCMP(p, "html", 4) == 0 && (p[4] == ',' || p[4] == NUL))
	{
	    new_html = TRUE;
	    p += 4;
	}
	অন্যথা যদি (STRNCMP(p, "exclude:", 8) == 0 && new_exclude_prog == ফাঁকা)
	{
	    p += 8;
	    new_exclude_prog = vim_regcomp(p, RE_MAGIC);
	    যদি (new_exclude_prog == ফাঁকা)
		errmsg = e_invalid_argument;
	    বিরতি;
	}
# endif
	অন্যথা
	{
	    errmsg = e_invalid_argument;
	    বিরতি;
	}
	যদি (*p == ',')
	    ++p;
    }
    যদি (errmsg == ফাঁকা)
    {
# ifdef FEAT_CLIPBOARD
	যদি (global_busy)
	    // clip_unnamed will be reset to clip_unnamed_saved
	    // at end_global_changes
	    clip_unnamed_saved = new_unnamed;
	অন্যথা
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
# যদি defined(FEAT_GUI_GTK)
	যদি (gui.in_use)
	{
#  ifdef USE_GTK4
	    gui_gtk_update_selection_formats(&clip_plus);
	    gui_gtk_update_selection_formats(&clip_star);
#  অন্যথা
	    gui_gtk_set_selection_targets((GdkAtom)GDK_SELECTION_PRIMARY);
	    gui_gtk_set_selection_targets((GdkAtom)clip_plus.gtk_sel_atom);
#  endif
#  ifdef FEAT_DND
	    gui_gtk_set_dnd_targets();
#  endif
	}
# endif
    }
    অন্যথা
	vim_regfree(new_exclude_prog);

    ফেরত errmsg;
}

#endif // HAVE_CLIPMETHOD

#ifdef FEAT_CLIPBOARD_PROVIDER

/*
 * Check if a clipboard provider with given name is available. Returns 1 if available,
 * 0 if not available, and -1 on error
 */
    স্থির পূর্ণসংখ্যা
clip_provider_is_available(char_u *provider)
{
    dict_T	*providers = get_vim_var_dict(VV_CLIPPROVIDERS);
    typval_T	provider_tv = {0};
    callback_T	callback = {0};
    typval_T	rettv = {0};
    typval_T	func_tv = {0};
    পূর্ণসংখ্যা		res = 0;

    যদি (dict_get_tv(providers, (অক্ষর *)provider, &provider_tv) == FAIL
	    || provider_tv.v_type != VAR_DICT)
	// clipboard provider not defined
	ফেরত -1;

    যদি (dict_get_tv(provider_tv.vval.v_dict, "available", &func_tv) == FAIL)
    {
	clear_tv(&provider_tv);
	// If "available" function not specified assume always TRUE
	ফেরত 1;
    }

    যদি ((callback = get_callback(&func_tv)).cb_name == ফাঁকা)
	যাও fail;

    যদি (call_callback(&callback, -1, &rettv, 0, ফাঁকা) == FAIL ||
	    (rettv.v_type != VAR_BOOL && rettv.v_type != VAR_সংখ্যা))
	যাও fail;

    যদি (rettv.vval.v_সংখ্যা)
	res = 1;

    যদি (FALSE)
fail:
	res = -1;

    free_callback(&callback);
    clear_tv(&func_tv);
    clear_tv(&rettv);
    clear_tv(&provider_tv);

    ফেরত res;
}

/*
 * Get the specified callback "function" from the provider dictionary for
 * register "reg".
 */
    স্থির পূর্ণসংখ্যা
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

    যদি (dict_get_tv(providers, (অক্ষর *)provider, &provider_tv) == FAIL)
	ফেরত FAIL;
    অন্যথা যদি (provider_tv.v_type != VAR_DICT)
    {
	clear_tv(&provider_tv);
	ফেরত FAIL;
    }
    অন্যথা যদি (dict_get_tv(
		provider_tv.vval.v_dict,
		(অক্ষর *)function,
		&action_tv) == FAIL)
    {
	clear_tv(&provider_tv);
	ফেরত FAIL;
    }
    অন্যথা যদি (action_tv.v_type != VAR_DICT)
    {
	clear_tv(&provider_tv);
	clear_tv(&action_tv);
	ফেরত FAIL;
    }
    অন্যথা যদি (dict_get_tv(action_tv.vval.v_dict, (অক্ষর *)reg, &func_tv) == FAIL)
    {
	clear_tv(&provider_tv);
	clear_tv(&action_tv);
	ফেরত FAIL;
    }
    অন্যথা যদি ((cb = get_callback(&func_tv)).cb_name == ফাঁকা)
    {
	clear_tv(&provider_tv);
	clear_tv(&action_tv);
	clear_tv(&func_tv);
	ফেরত FAIL;
    }
    clear_tv(&provider_tv);
    clear_tv(&action_tv);

    // func_tv owns the function name, so we must make a copy for the callback
    set_callback(callback, &cb);
    clear_tv(&func_tv);
    ফেরত OK;
}

    স্থির শূন্য
clip_provider_copy(char_u *reg, char_u *provider)
{
    স্থির bool	recursive = false;
    callback_T	callback;
    typval_T	rettv;
    typval_T	argvars[4];
    yankreg_T	*y_ptr;
    char_u	type[2 + NUMBUFLEN] = {0};
    list_T	*list = ফাঁকা;

    যদি (recursive)
	ফেরত;

    যদি (clip_provider_get_callback(
		reg,
		provider,
		(char_u *)"copy",
		&callback) == FAIL)
	ফেরত;

    // Convert register type into a string
    যদি (*reg == '+')
	y_ptr = get_y_register(REAL_PLUS_REGISTER);
    অন্যথা
	y_ptr = get_y_register(STAR_REGISTER);

    নির্বাচন (y_ptr->y_type)
    {
	ক্ষেত্র MCHAR:
	    type[0] = 'v';
	    বিরতি;
	ক্ষেত্র MLINE:
	    type[0] = 'V';
	    বিরতি;
	ক্ষেত্র MBLOCK:
	    sprintf((অক্ষর *)type, "%c%d", Ctrl_V, y_ptr->y_width + 1);
	    বিরতি;
	পূর্বনির্ধারিত:
	    type[0] = 0;
	    বিরতি;
    }

    argvars[0].v_type = VAR_STRING;
    argvars[0].vval.v_string = reg;

    argvars[1].v_type = VAR_STRING;
    argvars[1].vval.v_string = type;

    // Get register contents by creating a list of lines
    list = list_alloc();

    যদি (list == ফাঁকা)
    {
	free_callback(&callback);
	ফেরত;
    }

    জন্য (পূর্ণসংখ্যা i = 0; i < y_ptr->y_আকার; i++)
	যদি (list_append_string(list, y_ptr->y_array[i].string,
	    (পূর্ণসংখ্যা)y_ptr->y_array[i].দৈর্ঘ্য) == FAIL)
	{
	    free_callback(&callback);
	    list_unref(list);
	    ফেরত;
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

    স্থির শূন্য
clip_provider_paste(char_u *reg, char_u *provider)
{
    স্থির bool	recursive = false;
    callback_T	callback;
    typval_T	argvars[2];
    typval_T	rettv;
    পূর্ণসংখ্যা		ret;
    char_u	*reg_type;
    list_T	*সারিগুলি;

    যদি (recursive)
	ফেরত;

    যদি (clip_provider_get_callback(
		reg,
		provider,
		(char_u *)"paste",
		&callback) == FAIL)
	ফেরত;

    argvars[0].v_type = VAR_STRING;
    argvars[0].vval.v_string = reg;

    argvars[1].v_type = VAR_UNKNOWN;

    textlock++;
    recursive = true;
    ret = call_callback(&callback, -1, &rettv, 1, argvars);
    recursive = false;
    textlock--;

    যদি (ret == FAIL)
	যাও exit;
    অন্যথা যদি (rettv.v_type == VAR_TUPLE
	    && TUPLE_LEN(rettv.vval.v_tuple) == 2
	    && TUPLE_ITEM(rettv.vval.v_tuple, 0)->v_type == VAR_STRING
	    && TUPLE_ITEM(rettv.vval.v_tuple, 1)->v_type == VAR_LIST)
    {
	reg_type = TUPLE_ITEM(rettv.vval.v_tuple, 0)->vval.v_string;
	সারিগুলি = TUPLE_ITEM(rettv.vval.v_tuple, 1)->vval.v_list;
    }
    অন্যথা যদি (rettv.v_type == VAR_LIST
	    && rettv.vval.v_list->lv_len == 2
	    && rettv.vval.v_list->lv_first->li_tv.v_type == VAR_STRING
	    && rettv.vval.v_list->lv_first->li_পরবর্তী->li_tv.v_type == VAR_LIST)
    {
	reg_type = rettv.vval.v_list->lv_first->li_tv.vval.v_string;
	সারিগুলি = rettv.vval.v_list->lv_first->li_পরবর্তী->li_tv.vval.v_list;
    }
    অন্যথা
	যাও exit;

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
	পূর্ণসংখ্যা		len;

	// If the list is NULL handle like an empty list.
	len = সারিগুলি == ফাঁকা ? 0 : সারিগুলি->lv_len;

	// First half: use for pointers to result lines; second half: use for
	// pointers to allocated copies.
	lstval = ALLOC_MULT(char_u *, (len + 1) * 2);
	যদি (lstval == ফাঁকা)
	    যাও exit;
	curval = lstval;
	allocval = lstval + len + 2;
	curallocval = allocval;

	যদি (সারিগুলি != ফাঁকা)
	{
	    CHECK_LIST_MATERIALIZE(সারিগুলি);
	    FOR_ALL_LIST_বস্তুসমূহ(সারিগুলি, li)
	    {
		strval = tv_get_string_buf_chk(&li->li_tv, buf);
		যদি (strval == ফাঁকা)
		    যাও free_lstval;
		যদি (strval == buf)
		{
		    // Need to make a copy, next tv_get_string_buf_chk() will
		    // overwrite the string.
		    strval = vim_strsave(buf);
		    যদি (strval == ফাঁকা)
			যাও free_lstval;
		    *curallocval++ = strval;
		}
		*curval++ = strval;
	    }
	}
	*curval++ = ফাঁকা;

	যদি (*reg_type != NUL && (STRLEN(reg_type) == 0
		|| get_yank_type(&reg_type, &yank_type, &block_len) == FAIL))
	{
	    emsg(e_invalid_argument);
	    যাও free_lstval;
	}

	যদি (*reg == '+')
	    y_ptr = get_y_register(REAL_PLUS_REGISTER);
	অন্যথা
	    y_ptr = get_y_register(STAR_REGISTER);

	// Free previous register contents
	cur_y_ptr = get_y_current();
	set_y_current(y_ptr);

	free_yank_all();
	get_y_current()->y_আকার = 0;

	set_y_current(cur_y_ptr);

	str_to_reg(y_ptr,
		yank_type,
		(char_u *)lstval,
		-1,
		block_len,
		TRUE);

free_lstval:
	যখন (curallocval > allocval)
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
স্থির পূর্ণসংখ্যা star_pause_গুনতি = -2, plus_pause_গুনতি = -2;

    শূন্য
call_clip_provider_request(পূর্ণসংখ্যা reg)
{
    যদি (clipmethod != CLIPMETHOD_PROVIDER)
	ফেরত;

    যদি (reg == '+' && plus_pause_গুনতি < 0)
    {
	যদি (plus_pause_গুনতি == -1)
	    plus_pause_গুনতি = 1;
	clip_provider_paste((char_u *)"+", clip_provider);
    }
    অন্যথা যদি (reg == '*' && star_pause_গুনতি < 0)
    {
	যদি (star_pause_গুনতি == -1)
	    star_pause_গুনতি = 1;
	clip_provider_paste((char_u *)"*", clip_provider);
    }
    অন্যথা
	ফেরত;
}

    শূন্য
call_clip_provider_set(পূর্ণসংখ্যা reg)
{
    যদি (clipmethod != CLIPMETHOD_PROVIDER)
	ফেরত;

    যদি (reg == '+' && plus_pause_গুনতি < 0)
    {
	যদি (plus_pause_গুনতি == -1)
	    plus_pause_গুনতি = 1;
	clip_provider_copy((char_u *)"+", clip_provider);
    }
    অন্যথা যদি (reg == '*' && star_pause_গুনতি < 0)
    {
	যদি (star_pause_গুনতি == -1)
	    star_pause_গুনতি = 1;
	clip_provider_copy((char_u *)"*", clip_provider);
    }
}

/*
 * Makes it so that the next provider call is only done once any calls after are
 * ignored, until dec_clip_provider is called the same number of times after
 * again. Note that this is per clipboard register ("+", "*")
 */
    শূন্য
inc_clip_provider(শূন্য)
{
    plus_pause_গুনতি = (plus_pause_গুনতি == -2
	|| plus_pause_গুনতি == -1) ? -1 : plus_pause_গুনতি + 1;
    star_pause_গুনতি = (star_pause_গুনতি == -2
	|| star_pause_গুনতি == -1) ? -1 : star_pause_গুনতি + 1;
}

    শূন্য
dec_clip_provider(শূন্য)
{
    যদি (plus_pause_গুনতি != -2)
	plus_pause_গুনতি = plus_pause_গুনতি == -1 ? -1 : plus_pause_গুনতি - 1;
    যদি (star_pause_গুনতি != -2)
	star_pause_গুনতি = star_pause_গুনতি == -1 ? -1 : star_pause_গুনতি - 1;

    যদি (plus_pause_গুনতি == 0 || plus_pause_গুনতি == -1)
	plus_pause_গুনতি = -2;
    যদি (star_pause_গুনতি == 0 || star_pause_গুনতি == -1)
	star_pause_গুনতি = -2;
}

#endif // FEAT_CLIPBOARD_PROVIDER

#যদি defined(FEAT_WAYLAND_CLIPBOARD) || (defined(FEAT_GUI_GTK) && defined(USE_GTK4))
/*
 * If "vim" is TRUE, then get the motion type. If "vimenc" is TRUE, then get the
 * motion type and also convert "*buf". "buf" and "len_store" will be updated to
 * reflect the actual contents, but should be set beforehand with the initial
 * contents. Note that if "*tofree" is not NULL, use vim_free() on "*buf",
 * otherwise use the free func for which "*buf" was allocated with. In both
 * cases use vim_free() on "*tofree". Returns OK on success and FAIL on failure.
 */
    পূর্ণসংখ্যা
clip_convert_তথ্য(
	char_u	**buf,
	long	*len_store,
	পূর্ণসংখ্যা	*motion,
	bool	vim,
	bool	vimenc,
	char_u	**tofree)
{
    char_u  *final = *buf;
    char_u  *enc;
    long    len = *len_store;

    যদি (vim && len >= 2)
    {
	*motion = *final++;
	len--;
    }
    অন্যথা যদি (vimenc && len >= 3)
    {
	vimconv_T   conv;
	পূর্ণসংখ্যা	    convlen;

	// First byte is motion type
	*motion = *final++;
	len--;

	// Get encoding of selection
	enc = final;

	// Skip the encoding type including null terminator in final text
	final = memchr(final, NUL, len);
	যদি (final == ফাঁকা)
	    ফেরত FAIL;
	final++; // Skip NUL

	// Subtract pointers to get length of encoding;
	len -= final - enc;

	conv.vc_type = CONV_NONE;
	convert_setup(&conv, enc, p_enc);
	যদি (conv.vc_type != CONV_NONE)
	{
	   char_u *tmp;

	   convlen = len;
	   tmp = string_convert(&conv, final, &convlen);
	   len = convlen;
	   যদি (tmp != ফাঁকা)
	   {
		final = tmp;
		*tofree = final;
	   }
	   convert_setup(&conv, ফাঁকা, ফাঁকা);
	}
    }
    *buf = final;
    *len_store = len;
    ফেরত OK;
}
#endif
