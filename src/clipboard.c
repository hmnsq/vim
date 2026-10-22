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

#અન્તર્ભાવ "vim.h"

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
સ્થિર પૂર્ણાંક clip_provider_is_available(char_u *provider);
#endif

#જો defined(FEAT_CLIPBOARD)

# જો defined(FEAT_WAYLAND_CLIPBOARD)

#  include "wayland.h"

// Represents either the regular or primary selection
પ્રકારવ્યાખ્યા સંરચના {
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
પ્રકારવ્યાખ્યા સંરચના {
    vwl_seat_T *seat;

    clip_wl_selection_T regular;
    clip_wl_selection_T primary;
} clip_wl_T;

// Mime types we support sending and receiving
// Mimes with a lower index in the array are prioritized first when we are
// receiving data.
સ્થિર અચલ અક્ષર *supported_mimes[] = {
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

સ્થિર શૂન્ય
clip_wl_receive_data(Clipboard_T *cbd, અચલ અક્ષર *mime_type, પૂર્ણાંક fd);
સ્થિર શૂન્ય clip_wl_request_selection(Clipboard_T *cbd);
સ્થિર પૂર્ણાંક clip_wl_own_selection(Clipboard_T *cbd);
સ્થિર શૂન્ય clip_wl_lose_selection(Clipboard_T *cbd);
સ્થિર શૂન્ય clip_wl_set_selection(Clipboard_T *cbd);

#  જો defined(USE_SYSTEM)
સ્થિર bool clip_wl_owner_exists(Clipboard_T *cbd);
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
    શૂન્ય
clip_init(પૂર્ણાંક can_use)
{
    Clipboard_T *cb;

    cb = &clip_star;
    માટે (;;)
    {
	// No need to init again if cbd is already available
	જો (can_use && cb->available)
	    જાઓ skip;

	cb->available  = can_use;
	cb->owned      = FALSE;
	cb->start.lnum = 0;
	cb->start.col  = 0;
	cb->end.lnum   = 0;
	cb->end.col    = 0;
	cb->state      = SELECT_CLEARED;

skip:
	જો (cb == &clip_plus)
	    તોડો;
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
    શૂન્ય
clip_update_selection(Clipboard_T *clip)
{
    pos_T	    start, end;

    // If visual mode is only due to a redo command ("."), then ignore it
    જો (!redo_VIsual_busy && VIsual_active && (State & MODE_NORMAL))
    {
	જો (LT_POS(VIsual, curwin->w_cursor))
	{
	    start = VIsual;
	    end = curwin->w_cursor;
	    જો (has_mbyte)
		end.col += (*mb_ptr2len)(ml_get_cursor()) - 1;
	}
	નહીંતર
	{
	    start = curwin->w_cursor;
	    end = VIsual;
	}
	જો (!EQUAL_POS(clip->start, start)
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

    સ્થિર પૂર્ણાંક
clip_gen_own_selection(Clipboard_T *cbd)
{
# જો defined(FEAT_XCLIPBOARD) || defined(FEAT_WAYLAND_CLIPBOARD)
#  ifdef FEAT_GUI
    જો (gui.in_use)
	પાછા clip_mch_own_selection(cbd);
    નહીંતર
#  endif
    {
	જો (clipmethod == CLIPMETHOD_WAYLAND)
	{
#  ifdef FEAT_WAYLAND_CLIPBOARD
	    પાછા clip_wl_own_selection(cbd);
#  endif
	}
	નહીંતર જો (clipmethod == CLIPMETHOD_X11)
	{
#  ifdef FEAT_XCLIPBOARD
	    પાછા clip_xterm_own_selection(cbd);
#  endif
	}
    }
    પાછા FAIL;
# નહીંતર
    પાછા clip_mch_own_selection(cbd);
# endif
}

    શૂન્ય
clip_own_selection(Clipboard_T *cbd)
{
    /*
     * Also want to check somehow that we are reading from the keyboard rather
     * than a mapping etc.
     */
# જો defined(FEAT_X11) || defined(FEAT_WAYLAND_CLIPBOARD)
    // Always own the selection, we might have lost it without being
    // notified, e.g. during a ":sh" command.
    જો (cbd->available)
    {
	પૂર્ણાંક was_owned = cbd->owned;

	cbd->owned = (clip_gen_own_selection(cbd) == OK);
	જો (!was_owned && (cbd == &clip_star || cbd == &clip_plus))
	{
	    // May have to show a different kind of highlighting for the
	    // selected area.  There is no specific redraw command for this,
	    // just redraw all windows on the current buffer.
	    જો (cbd->owned
		    && (get_real_state() == MODE_VISUAL
					    || get_real_state() == MODE_SELECT)
		    && (cbd == &clip_star ? clip_isautosel_star()
						      : clip_isautosel_plus())
		    && HL_ATTR(HLF_V) != HL_ATTR(HLF_VNC))
		redraw_curbuf_later(UPD_INVERTED_ALL);
	}
    }
# નહીંતર
    // Only own the clipboard when we didn't own it yet.
    જો (!cbd->owned && cbd->available)
	cbd->owned = (clip_gen_own_selection(cbd) == OK);
# endif
}

    સ્થિર શૂન્ય
clip_gen_lose_selection(Clipboard_T *cbd)
{
# જો defined(FEAT_XCLIPBOARD) || defined(FEAT_WAYLAND_CLIPBOARD)
#  ifdef FEAT_GUI
    જો (gui.in_use)
	clip_mch_lose_selection(cbd);
    નહીંતર
#  endif
    {
	જો (clipmethod == CLIPMETHOD_WAYLAND)
	{
#  ifdef FEAT_WAYLAND_CLIPBOARD
	    clip_wl_lose_selection(cbd);
#  endif
	}
	નહીંતર જો (clipmethod == CLIPMETHOD_X11)
	{
#  ifdef FEAT_XCLIPBOARD
	    clip_xterm_lose_selection(cbd);
#  endif
	}
    }
# નહીંતર
    clip_mch_lose_selection(cbd);
# endif
}

    શૂન્ય
clip_lose_selection(Clipboard_T *cbd)
{
# ifdef FEAT_X11
    પૂર્ણાંક	    was_owned = cbd->owned;
# endif
    પૂર્ણાંક     visual_selection = FALSE;

    જો (cbd == &clip_star || cbd == &clip_plus)
	visual_selection = TRUE;

    clip_free_selection(cbd);
    cbd->owned = FALSE;
    જો (visual_selection)
	clip_clear_selection(cbd);
    clip_gen_lose_selection(cbd);
# ifdef FEAT_X11
    જો (visual_selection)
    {
	// May have to show a different kind of highlighting for the selected
	// area.  There is no specific redraw command for this, just redraw all
	// windows on the current buffer.
	જો (was_owned
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

    સ્થિર શૂન્ય
clip_copy_selection(Clipboard_T *clip)
{
    જો (VIsual_active && (State & MODE_NORMAL) && clip->available)
    {
	clip_update_selection(clip);
	clip_free_selection(clip);
	clip_own_selection(clip);
	જો (clip->owned)
	    clip_get_selection(clip);
	clip_gen_set_selection(clip);
    }
}

/*
 * Save and restore clip_unnamed before doing possibly many changes. This
 * prevents accessing the clipboard very often which might slow down Vim
 * considerably.
 */
સ્થિર પૂર્ણાંક global_change_ગણતરી = 0; // if set, inside a start_global_changes
સ્થિર પૂર્ણાંક clipboard_needs_update = FALSE; // clipboard needs to be updated
સ્થિર પૂર્ણાંક clip_did_set_selection = TRUE;

/*
 * Save clip_unnamed and reset it.
 */
    શૂન્ય
start_global_changes(શૂન્ય)
{
    જો (++global_change_ગણતરી > 1)
	પાછા;
    clip_unnamed_saved = clip_unnamed;
    clipboard_needs_update = FALSE;

    જો (clip_did_set_selection)
    {
	clip_unnamed = 0;
	clip_did_set_selection = FALSE;
    }
}

/*
 * Return TRUE if setting the clipboard was postponed, it already contains the
 * right text.
 */
    સ્થિર પૂર્ણાંક
is_clipboard_needs_update(શૂન્ય)
{
    પાછા clipboard_needs_update;
}

/*
 * Restore clip_unnamed and set the selection when needed.
 */
    શૂન્ય
end_global_changes(શૂન્ય)
{
    જો (--global_change_ગણતરી > 0)
	// recursive
	પાછા;
    જો (!clip_did_set_selection)
    {
	clip_did_set_selection = TRUE;
	clip_unnamed = clip_unnamed_saved;
	clip_unnamed_saved = 0;
	જો (clipboard_needs_update)
	{
	    // only store something in the clipboard,
	    // if we have yanked anything to it
	    જો (clip_unnamed & CLIP_UNNAMED)
	    {
		clip_own_selection(&clip_star);
		clip_gen_set_selection(&clip_star);
	    }
	    જો (clip_unnamed & CLIP_UNNAMED_PLUS)
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
    શૂન્ય
clip_auto_select(શૂન્ય)
{
    જો (clip_isautosel_star())
	clip_copy_selection(&clip_star);
    જો (clip_isautosel_plus())
	clip_copy_selection(&clip_plus);
}

/*
 * Return TRUE if automatic selection of Visual area is desired for the *
 * register.
 */
    પૂર્ણાંક
clip_isautosel_star(શૂન્ય)
{
# ifdef FEAT_CLIPBOARD_PROVIDER
    જો (clipmethod == CLIPMETHOD_PROVIDER)
	પાછા false;
# endif
# ifdef FEAT_GUI
    જો (gui.in_use)
	પાછા vim_strchr(p_go, GO_ASEL) != ખાલી
	    && vim_strchr(p_go, GO_ASELPLUS) == ખાલી;
# endif
    પાછા clip_autoselect_star;
}

/*
 * Return TRUE if automatic selection of Visual area is desired for the +
 * register.
 */
    પૂર્ણાંક
clip_isautosel_plus(શૂન્ય)
{
# ifdef FEAT_CLIPBOARD_PROVIDER
    જો (clipmethod == CLIPMETHOD_PROVIDER)
	પાછા false;
# endif
# ifdef FEAT_GUI
    જો (gui.in_use)
	પાછા vim_strchr(p_go, GO_ASELPLUS) != ખાલી;
# endif
    પાછા clip_autoselect_plus;
}


/*
 * Stuff for general mouse selection, without using Visual mode.
 */

/*
 * Compare two screen positions ala strcmp()
 */
    સ્થિર પૂર્ણાંક
clip_compare_pos(
    પૂર્ણાંક		row1,
    પૂર્ણાંક		col1,
    પૂર્ણાંક		row2,
    પૂર્ણાંક		col2)
{
    જો (row1 > row2) પાછા(1);
    જો (row1 < row2) પાછા(-1);
    જો (col1 > col2) પાછા(1);
    જો (col1 < col2) પાછા(-1);
    પાછા(0);
}

// "how" flags for clip_invert_area()
# define CLIP_CLEAR	1
# define CLIP_SET	2
# define CLIP_TOGGLE	3

/*
 * Invert or un-invert a rectangle of the screen.
 * "invert" is true if the result is inverted.
 */
    સ્થિર શૂન્ય
clip_invert_rectangle(
	Clipboard_T	*cbd UNUSED,
	પૂર્ણાંક		row_arg,
	પૂર્ણાંક		col_arg,
	પૂર્ણાંક		height_arg,
	પૂર્ણાંક		width_arg,
	પૂર્ણાંક		invert)
{
    પૂર્ણાંક		row = row_arg;
    પૂર્ણાંક		col = col_arg;
    પૂર્ણાંક		height = height_arg;
    પૂર્ણાંક		width = width_arg;

# ifdef FEAT_PROP_POPUP
    // this goes on top of all popup windows
    screen_zindex = CLIP_ZINDEX;

    જો (col < cbd->min_col)
    {
	width -= cbd->min_col - col;
	col = cbd->min_col;
    }
    જો (width > cbd->max_col - col)
	width = cbd->max_col - col;
    જો (row < cbd->min_row)
    {
	height -= cbd->min_row - row;
	row = cbd->min_row;
    }
    જો (height > cbd->max_row - row + 1)
	height = cbd->max_row - row + 1;
# endif
# ifdef FEAT_GUI
    જો (gui.in_use)
	gui_mch_invert_rectangle(row, col, height, width);
    નહીંતર
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
    સ્થિર શૂન્ય
clip_invert_area(
	Clipboard_T	*cbd,
	પૂર્ણાંક		row1,
	પૂર્ણાંક		col1,
	પૂર્ણાંક		row2,
	પૂર્ણાંક		col2,
	પૂર્ણાંક		how)
{
    પૂર્ણાંક		invert = FALSE;
    પૂર્ણાંક		max_col;

# ifdef FEAT_PROP_POPUP
    max_col = cbd->max_col - 1;
# નહીંતર
    max_col = Columns - 1;
# endif

    જો (how == CLIP_SET)
	invert = TRUE;

    // Swap the from and to positions so the from is always before
    જો (clip_compare_pos(row1, col1, row2, col2) > 0)
    {
	પૂર્ણાંક tmp_row, tmp_col;

	tmp_row = row1;
	tmp_col = col1;
	row1	= row2;
	col1	= col2;
	row2	= tmp_row;
	col2	= tmp_col;
    }
    નહીંતર જો (how == CLIP_TOGGLE)
	invert = TRUE;

    // If all on the same line, do it the easy way
    જો (row1 == row2)
    {
	clip_invert_rectangle(cbd, row1, col1, 1, col2 - col1, invert);
    }
    નહીંતર
    {
	// Handle a piece of the first line
	જો (col1 > 0)
	{
	    clip_invert_rectangle(cbd, row1, col1, 1,
						  (પૂર્ણાંક)Columns - col1, invert);
	    row1++;
	}

	// Handle a piece of the last line
	જો (col2 < max_col)
	{
	    clip_invert_rectangle(cbd, row2, 0, 1, col2, invert);
	    row2--;
	}

	// Handle the rectangle that's left
	જો (row2 >= row1)
	    clip_invert_rectangle(cbd, row1, 0, row2 - row1 + 1,
							 (પૂર્ણાંક)Columns, invert);
    }
}

/*
 * Start, continue or end a modeless selection.  Used when editing the
 * command-line, in the cmdline window and when the mouse is in a popup window.
 */
    શૂન્ય
clip_modeless(પૂર્ણાંક button, પૂર્ણાંક is_click, પૂર્ણાંક is_drag)
{
    પૂર્ણાંક		repeat;

    repeat = ((clip_star.mode == SELECT_MODE_CHAR
		|| clip_star.mode == SELECT_MODE_લીટી)
					      && (mod_mask & MOD_MASK_2CLICK))
	    || (clip_star.mode == SELECT_MODE_WORD
					     && (mod_mask & MOD_MASK_3CLICK));
    જો (is_click && button == MOUSE_જમણું)
    {
	// Right mouse button: If there was no selection, start one.
	// Otherwise extend the existing selection.
	જો (clip_star.state == SELECT_CLEARED)
	    clip_start_selection(mouse_col, mouse_row, FALSE);
	clip_process_selection(button, mouse_col, mouse_row, repeat);
    }
    નહીંતર જો (is_click)
	clip_start_selection(mouse_col, mouse_row, repeat);
    નહીંતર જો (is_drag)
    {
	// Don't try extending a selection if there isn't one.  Happens when
	// button-down is in the cmdline and them moving mouse upwards.
	જો (clip_star.state != SELECT_CLEARED)
	    clip_process_selection(button, mouse_col, mouse_row, repeat);
    }
    નહીંતર // release
	clip_process_selection(MOUSE_RELEASE, mouse_col, mouse_row, FALSE);
}

/*
 * Update the currently selected region by adding and/or subtracting from the
 * beginning or end and inverting the changed area(s).
 */
    સ્થિર શૂન્ય
clip_update_modeless_selection(
    Clipboard_T    *cb,
    પૂર્ણાંક		    row1,
    પૂર્ણાંક		    col1,
    પૂર્ણાંક		    row2,
    પૂર્ણાંક		    col2)
{
    // See if we changed at the beginning of the selection
    જો (row1 != cb->start.lnum || col1 != (પૂર્ણાંક)cb->start.col)
    {
	clip_invert_area(cb, row1, col1, (પૂર્ણાંક)cb->start.lnum, cb->start.col,
								 CLIP_TOGGLE);
	cb->start.lnum = row1;
	cb->start.col  = col1;
    }

    // See if we changed at the end of the selection
    જો (row2 != cb->end.lnum || col2 != (પૂર્ણાંક)cb->end.col)
    {
	clip_invert_area(cb, (પૂર્ણાંક)cb->end.lnum, cb->end.col, row2, col2,
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

    સ્થિર શૂન્ય
clip_get_word_boundaries(Clipboard_T *cb, પૂર્ણાંક row, પૂર્ણાંક col)
{
    પૂર્ણાંક		start_class;
    પૂર્ણાંક		temp_col;
    char_u	*p;
    પૂર્ણાંક		mboff;

    જો (row >= screen_Rows || col >= screen_Columns || Screen_લીટીઓ == ખાલી)
	પાછા;

    p = Screen_લીટીઓ + લીટી_Offset[row];
    // Correct for starting in the right half of a double-wide char
    જો (enc_dbcs != 0)
	col -= dbcs_screen_head_off(p, p + col);
    નહીંતર જો (enc_utf8 && p[col] == 0)
	--col;
    start_class = CHAR_CLASS(p[col]);

    temp_col = col;
    માટે ( ; temp_col > 0; temp_col--)
	જો (enc_dbcs != 0
		   && (mboff = dbcs_screen_head_off(p, p + temp_col - 1)) > 0)
	    temp_col -= mboff;
	નહીંતર જો (CHAR_CLASS(p[temp_col - 1]) != start_class
		&& !(enc_utf8 && p[temp_col - 1] == 0))
	    તોડો;
    cb->word_start_col = temp_col;

    temp_col = col;
    માટે ( ; temp_col < screen_Columns; temp_col++)
	જો (enc_dbcs != 0 && dbcs_ptr2cells(p + temp_col) == 2)
	    ++temp_col;
	નહીંતર જો (CHAR_CLASS(p[temp_col]) != start_class
		&& !(enc_utf8 && p[temp_col] == 0))
	    તોડો;
    cb->word_end_col = temp_col;
}

/*
 * Find the column position for the last non-whitespace character on the given
 * line at or before start_col.
 */
    સ્થિર પૂર્ણાંક
clip_get_લીટી_end(Clipboard_T *cbd UNUSED, પૂર્ણાંક row)
{
    પૂર્ણાંક	    i;

    જો (row >= screen_Rows || Screen_લીટીઓ == ખાલી)
	પાછા 0;
    માટે (i =
# ifdef FEAT_PROP_POPUP
	    cbd->max_col;
# નહીંતર
	    screen_Columns;
# endif
			    i > 0; i--)
	જો (Screen_લીટીઓ[લીટી_Offset[row] + i - 1] != ' ')
	    તોડો;
    પાછા i;
}

/*
 * Start the selection
 */
    શૂન્ય
clip_start_selection(પૂર્ણાંક col, પૂર્ણાંક row, પૂર્ણાંક repeated_click)
{
    Clipboard_T	*cb = &clip_star;
# ifdef FEAT_PROP_POPUP
    win_T	*wp;
    પૂર્ણાંક		row_cp = row;
    પૂર્ણાંક		col_cp = col;

    wp = mouse_find_win(&row_cp, &col_cp, FIND_POPUP);
    જો (wp != ખાલી && WIN_IS_POPUP(wp)
				  && popup_is_in_scrollbar(wp, row_cp, col_cp))
	// click or double click in scrollbar does not start a selection
	પાછા;
# endif

    જો (cb->state == SELECT_DONE)
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
    જો (wp != ખાલી && WIN_IS_POPUP(wp))
    {
	// Click in a popup window restricts selection to that window,
	// excluding the border.
	cb->min_col = wp->w_wincol + wp->w_popup_border[3];
	cb->max_col = wp->w_wincol + popup_width(wp)
				 - wp->w_popup_border[1] - wp->w_has_scrollbar;
	જો (cb->max_col > screen_Columns)
	    cb->max_col = screen_Columns;
	cb->min_row = wp->w_winrow + wp->w_popup_border[0];
	cb->max_row = wp->w_winrow + popup_height(wp) - 1
						   - wp->w_popup_border[2];
    }
    નહીંતર
    {
	cb->min_col = 0;
	cb->max_col = screen_Columns;
	cb->min_row = 0;
	cb->max_row = screen_Rows;
    }
# endif

    જો (repeated_click)
    {
	જો (++cb->mode > SELECT_MODE_લીટી)
	    cb->mode = SELECT_MODE_CHAR;
    }
    નહીંતર
	cb->mode = SELECT_MODE_CHAR;

# ifdef FEAT_GUI
    // clear the cursor until the selection is made
    જો (gui.in_use)
	gui_undraw_cursor();
# endif

    પસંદગી (cb->mode)
    {
	સ્થિતિ SELECT_MODE_CHAR:
	    cb->origin_start_col = cb->start.col;
	    cb->word_end_col = clip_get_લીટી_end(cb, (પૂર્ણાંક)cb->start.lnum);
	    તોડો;

	સ્થિતિ SELECT_MODE_WORD:
	    clip_get_word_boundaries(cb, (પૂર્ણાંક)cb->start.lnum, cb->start.col);
	    cb->origin_start_col = cb->word_start_col;
	    cb->origin_end_col	 = cb->word_end_col;

	    clip_invert_area(cb, (પૂર્ણાંક)cb->start.lnum, cb->word_start_col,
			    (પૂર્ણાંક)cb->end.lnum, cb->word_end_col, CLIP_SET);
	    cb->start.col = cb->word_start_col;
	    cb->end.col   = cb->word_end_col;
	    તોડો;

	સ્થિતિ SELECT_MODE_લીટી:
	    clip_invert_area(cb, (પૂર્ણાંક)cb->start.lnum, 0, (પૂર્ણાંક)cb->start.lnum,
			    (પૂર્ણાંક)Columns, CLIP_SET);
	    cb->start.col = 0;
	    cb->end.col   = Columns;
	    તોડો;
    }

    cb->prev = cb->start;

# ifdef DEBUG_SELECTION
    printf("Selection started at (%ld,%d)\n", cb->start.lnum, cb->start.col);
# endif
}

/*
 * Continue processing the selection
 */
    શૂન્ય
clip_process_selection(
    પૂર્ણાંક		button,
    પૂર્ણાંક		col,
    પૂર્ણાંક		row,
    int_u	repeated_click)
{
    Clipboard_T	*cb = &clip_star;
    પૂર્ણાંક		diff;
    પૂર્ણાંક		slen = 1;	// cursor shape width

    જો (button == MOUSE_RELEASE)
    {
	જો (cb->state != SELECT_IN_PROGRESS)
	    પાછા;

	// Check to make sure we have something selected
	જો (cb->start.lnum == cb->end.lnum && cb->start.col == cb->end.col)
	{
# ifdef FEAT_GUI
	    જો (gui.in_use)
		gui_update_cursor(FALSE, FALSE);
# endif
	    cb->state = SELECT_CLEARED;
	    પાછા;
	}

# ifdef DEBUG_SELECTION
	printf("Selection ended: (%ld,%d) to (%ld,%d)\n", cb->start.lnum,
		cb->start.col, cb->end.lnum, cb->end.col);
# endif
	જો (clip_isautosel_star() || clip_isautosel_plus()
		|| (
# ifdef FEAT_GUI
		    gui.in_use ? (vim_strchr(p_go, GO_ASELML) != ખાલી) :
# endif
		    clip_autoselectml))
	    clip_copy_modeless_selection(FALSE);
# ifdef FEAT_GUI
	જો (gui.in_use)
	    gui_update_cursor(FALSE, FALSE);
# endif

	cb->state = SELECT_DONE;
	પાછા;
    }

    row = check_row(row);
    col = check_col(col);
    col = mb_fix_col(col, row);

    જો (col == (પૂર્ણાંક)cb->prev.col && row == cb->prev.lnum && !repeated_click)
	પાછા;

    /*
     * When extending the selection with the right mouse button, swap the
     * start and end if the position is before half the selection
     */
    જો (cb->state == SELECT_DONE && button == MOUSE_જમણું)
    {
	/*
	 * If the click is before the start, or the click is inside the
	 * selection and the start is the closest side, set the origin to the
	 * end of the selection.
	 */
	જો (clip_compare_pos(row, col, (પૂર્ણાંક)cb->start.lnum, cb->start.col) < 0
		|| (clip_compare_pos(row, col,
					   (પૂર્ણાંક)cb->end.lnum, cb->end.col) < 0
		    && (((cb->start.lnum == cb->end.lnum
			    && cb->end.col - col > col - cb->start.col))
			|| ((diff = (cb->end.lnum - row) -
						   (row - cb->start.lnum)) > 0
			    || (diff == 0 && col < (પૂર્ણાંક)(cb->start.col +
							 cb->end.col) / 2)))))
	{
	    cb->origin_row = (short_u)cb->end.lnum;
	    cb->origin_start_col = cb->end.col - 1;
	    cb->origin_end_col = cb->end.col;
	}
	નહીંતર
	{
	    cb->origin_row = (short_u)cb->start.lnum;
	    cb->origin_start_col = cb->start.col;
	    cb->origin_end_col = cb->start.col;
	}
	જો (cb->mode == SELECT_MODE_WORD && !repeated_click)
	    cb->mode = SELECT_MODE_CHAR;
    }

    // set state, for when using the right mouse button
    cb->state = SELECT_IN_PROGRESS;

# ifdef DEBUG_SELECTION
    printf("Selection extending to (%d,%d)\n", row, col);
# endif

    જો (repeated_click && ++cb->mode > SELECT_MODE_લીટી)
	cb->mode = SELECT_MODE_CHAR;

    પસંદગી (cb->mode)
    {
	સ્થિતિ SELECT_MODE_CHAR:
	    // If we're on a different line, find where the line ends
	    જો (row != cb->prev.lnum)
		cb->word_end_col = clip_get_લીટી_end(cb, row);

	    // See if we are before or after the origin of the selection
	    જો (clip_compare_pos(row, col, cb->origin_row,
						   cb->origin_start_col) >= 0)
	    {
		જો (col >= (પૂર્ણાંક)cb->word_end_col)
		    clip_update_modeless_selection(cb, cb->origin_row,
			    cb->origin_start_col, row, (પૂર્ણાંક)Columns);
		નહીંતર
		{
		    જો (has_mbyte && mb_lefthalve(row, col))
			slen = 2;
		    clip_update_modeless_selection(cb, cb->origin_row,
			    cb->origin_start_col, row, col + slen);
		}
	    }
	    નહીંતર
	    {
		જો (has_mbyte
			&& mb_lefthalve(cb->origin_row, cb->origin_start_col))
		    slen = 2;
		જો (col >= (પૂર્ણાંક)cb->word_end_col)
		    clip_update_modeless_selection(cb, row, cb->word_end_col,
			    cb->origin_row, cb->origin_start_col + slen);
		નહીંતર
		    clip_update_modeless_selection(cb, row, col,
			    cb->origin_row, cb->origin_start_col + slen);
	    }
	    તોડો;

	સ્થિતિ SELECT_MODE_WORD:
	    // If we are still within the same word, do nothing
	    જો (row == cb->prev.lnum && col >= (પૂર્ણાંક)cb->word_start_col
		    && col < (પૂર્ણાંક)cb->word_end_col && !repeated_click)
		પાછા;

	    // Get new word boundaries
	    clip_get_word_boundaries(cb, row, col);

	    // Handle being after the origin point of selection
	    જો (clip_compare_pos(row, col, cb->origin_row,
		    cb->origin_start_col) >= 0)
		clip_update_modeless_selection(cb, cb->origin_row,
			cb->origin_start_col, row, cb->word_end_col);
	    નહીંતર
		clip_update_modeless_selection(cb, row, cb->word_start_col,
			cb->origin_row, cb->origin_end_col);
	    તોડો;

	સ્થિતિ SELECT_MODE_લીટી:
	    જો (row == cb->prev.lnum && !repeated_click)
		પાછા;

	    જો (clip_compare_pos(row, col, cb->origin_row,
		    cb->origin_start_col) >= 0)
		clip_update_modeless_selection(cb, cb->origin_row, 0, row,
			(પૂર્ણાંક)Columns);
	    નહીંતર
		clip_update_modeless_selection(cb, row, 0, cb->origin_row,
			(પૂર્ણાંક)Columns);
	    તોડો;
    }

    cb->prev.lnum = row;
    cb->prev.col  = col;

# ifdef DEBUG_SELECTION
	printf("Selection is: (%ld,%d) to (%ld,%d)\n", cb->start.lnum,
		cb->start.col, cb->end.lnum, cb->end.col);
# endif
}

# જો defined(FEAT_GUI)
/*
 * Redraw part of the selection if character at "row,col" is inside of it.
 * Only used for the GUI.
 */
    શૂન્ય
clip_may_redraw_selection(પૂર્ણાંક row, પૂર્ણાંક col, પૂર્ણાંક len)
{
    પૂર્ણાંક		start = col;
    પૂર્ણાંક		end = col + len;

    જો (clip_star.state != SELECT_CLEARED
	    && row >= clip_star.start.lnum
	    && row <= clip_star.end.lnum)
    {
	જો (row == clip_star.start.lnum && start < (પૂર્ણાંક)clip_star.start.col)
	    start = clip_star.start.col;
	જો (row == clip_star.end.lnum && end > (પૂર્ણાંક)clip_star.end.col)
	    end = clip_star.end.col;
	જો (end > start)
	    clip_invert_area(&clip_star, row, start, row, end, 0);
    }
}
# endif

/*
 * Called from outside to clear selected region from the display
 */
    શૂન્ય
clip_clear_selection(Clipboard_T *cbd)
{

    જો (cbd->state == SELECT_CLEARED)
	પાછા;

    clip_invert_area(cbd, (પૂર્ણાંક)cbd->start.lnum, cbd->start.col,
				 (પૂર્ણાંક)cbd->end.lnum, cbd->end.col, CLIP_CLEAR);
    cbd->state = SELECT_CLEARED;
}

/*
 * Clear the selection if any lines from "row1" to "row2" are inside of it.
 */
    શૂન્ય
clip_may_clear_selection(પૂર્ણાંક row1, પૂર્ણાંક row2)
{
    જો (clip_star.state == SELECT_DONE
	    && row2 >= clip_star.start.lnum
	    && row1 <= clip_star.end.lnum)
	clip_clear_selection(&clip_star);
}

/*
 * Called before the screen is scrolled up or down.  Adjusts the line numbers
 * of the selection.  Call with big number when clearing the screen.
 */
    શૂન્ય
clip_scroll_selection(
    પૂર્ણાંક	    rows)		// negative for scroll down
{
    પૂર્ણાંક	    lnum;

    જો (clip_star.state == SELECT_CLEARED)
	પાછા;

    lnum = clip_star.start.lnum - rows;
    જો (lnum <= 0)
	clip_star.start.lnum = 0;
    નહીંતર જો (lnum >= screen_Rows)	// scrolled off of the screen
	clip_star.state = SELECT_CLEARED;
    નહીંતર
	clip_star.start.lnum = lnum;

    lnum = clip_star.end.lnum - rows;
    જો (lnum < 0)			// scrolled off of the screen
	clip_star.state = SELECT_CLEARED;
    નહીંતર જો (lnum >= screen_Rows)
	clip_star.end.lnum = screen_Rows - 1;
    નહીંતર
	clip_star.end.lnum = lnum;
}

/*
 * Copy the currently selected area into the '*' or '+' register so it will be
 * available for pasting.
 * When "both" is TRUE also copy to the other register.
 */
    શૂન્ય
clip_copy_modeless_selection(પૂર્ણાંક both UNUSED)
{
    // The info for the modeless selection is stored in '*' register, however if
    // we are using the '+' register for modeless autoselect, we copy to
    // clip_plus instead while using the info in clip_star.
    Clipboard_T *cbd = clip_isautosel_plus() ? &clip_plus : &clip_star;
    char_u	*buffer;
    char_u	*bufp;
    પૂર્ણાંક		row;
    પૂર્ણાંક		start_col;
    પૂર્ણાંક		end_col;
    પૂર્ણાંક		લીટી_end_col;
    પૂર્ણાંક		add_newline_flag = FALSE;
    પૂર્ણાંક		len;
    char_u	*p;
    પૂર્ણાંક		row1 = clip_star.start.lnum;
    પૂર્ણાંક		col1 = clip_star.start.col;
    પૂર્ણાંક		row2 = clip_star.end.lnum;
    પૂર્ણાંક		col2 = clip_star.end.col;

    // Can't use ScreenLines unless initialized
    જો (Screen_લીટીઓ == ખાલી)
	પાછા;

    /*
     * Make sure row1 <= row2, and if row1 == row2 that col1 <= col2.
     */
    જો (row1 > row2)
    {
	row = row1; row1 = row2; row2 = row;
	row = col1; col1 = col2; col2 = row;
    }
    નહીંતર જો (row1 == row2 && col1 > col2)
    {
	row = col1; col1 = col2; col2 = row;
    }
# ifdef FEAT_PROP_POPUP
    જો (col1 < clip_star.min_col)
	col1 = clip_star.min_col;
    જો (col2 > clip_star.max_col)
	col2 = clip_star.max_col;
    જો (row1 > clip_star.max_row || row2 < clip_star.min_row)
	પાછા;
    જો (row1 < clip_star.min_row)
	row1 = clip_star.min_row;
    જો (row2 > clip_star.max_row)
	row2 = clip_star.max_row;
# endif
    // correct starting point for being on right half of double-wide char
    p = Screen_લીટીઓ + લીટી_Offset[row1];
    જો (enc_dbcs != 0)
	col1 -= (*mb_head_off)(p, p + col1);
    નહીંતર જો (enc_utf8 && p[col1] == 0)
	--col1;

    // Create a temporary buffer for storing the text
    len = (row2 - row1 + 1) * Columns + 1;
    જો (enc_dbcs != 0)
	len *= 2;	// max. 2 bytes per display cell
    નહીંતર જો (enc_utf8)
	len *= MB_MAXBYTES;
    // A newline is added after each row
    len += row2 - row1 + 2;
    buffer = alloc(len);
    જો (buffer == ખાલી)	    // out of memory
	પાછા;

    // Process each row in the selection
    માટે (bufp = buffer, row = row1; row <= row2; row++)
    {
	જો (row == row1)
	    start_col = col1;
	નહીંતર
# ifdef FEAT_PROP_POPUP
	    start_col = clip_star.min_col;
# નહીંતર
	    start_col = 0;
# endif

	જો (row == row2)
	    end_col = col2;
	નહીંતર
# ifdef FEAT_PROP_POPUP
	    end_col = clip_star.max_col;
# નહીંતર
	    end_col = Columns;
# endif

	લીટી_end_col = clip_get_લીટી_end(&clip_star, row);

	// See if we need to nuke some trailing whitespace
	જો (end_col >=
# ifdef FEAT_PROP_POPUP
		clip_star.max_col
# નહીંતર
		Columns
# endif
		    && (row < row2 || end_col > લીટી_end_col))
	{
	    // Get rid of trailing whitespace
	    end_col = લીટી_end_col;
	    જો (end_col < start_col)
		end_col = start_col;

	    // If the last line extended to the end, add an extra newline
	    જો (row == row2)
		add_newline_flag = TRUE;
	}

	// If after the first row, we need to always add a newline
	જો (row > row1 && !લીટી_Wraps[row - 1])
	    *bufp++ = NL;

	// Safetey check for in case resizing went wrong
	જો (row < screen_Rows && end_col <= screen_Columns)
	{
	    જો (enc_dbcs != 0)
	    {
		પૂર્ણાંક	i;

		p = Screen_લીટીઓ + લીટી_Offset[row];
		માટે (i = start_col; i < end_col; ++i)
		    જો (enc_dbcs == DBCS_JPNU && p[i] == 0x8e)
		    {
			// single-width double-byte char
			*bufp++ = 0x8e;
			*bufp++ = ScreenLines2[લીટી_Offset[row] + i];
		    }
		    નહીંતર
		    {
			*bufp++ = p[i];
			જો (MB_BYTE2LEN(p[i]) == 2)
			    *bufp++ = p[++i];
		    }
	    }
	    નહીંતર જો (enc_utf8)
	    {
		પૂર્ણાંક	off;
		પૂર્ણાંક	i;

		off = લીટી_Offset[row];
		માટે (i = start_col; i < end_col; ++i)
		{
		    // The base character is either in ScreenLinesUC[] or
		    // ScreenLines[].
		    જો (Screen_લીટીઓ_UC[off + i] == 0)
			*bufp++ = Screen_લીટીઓ[off + i];
		    નહીંતર
			// Includes the composing characters.
			bufp += utfc_char2bytes(off + i, bufp);
		    // Skip right half of double-wide character.
		    જો (Screen_લીટીઓ[off + i + 1] == 0)
			++i;
		}
	    }
	    નહીંતર
	    {
		STRNCPY(bufp, Screen_લીટીઓ + લીટી_Offset[row] + start_col,
							 end_col - start_col);
		bufp += end_col - start_col;
	    }
	}
    }

    // Add a newline at the end if the selection ended there
    જો (add_newline_flag)
	*bufp++ = NL;

    // First cleanup any old selection and become the owner.
    clip_free_selection(cbd);
    clip_own_selection(cbd);

    // Yank the text into the '*' register.
    clip_yank_selection(MCHAR, buffer, (long)(bufp - buffer), cbd);

    // Make the register contents available to the outside world.
    clip_gen_set_selection(cbd);

# ifdef FEAT_X11
    જો (both)
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

    શૂન્ય
clip_gen_set_selection(Clipboard_T *cbd)
{
    જો (!clip_did_set_selection)
    {
	// Updating postponed, so that accessing the system clipboard won't
	// hang Vim when accessing it many times (e.g. on a :g command).
	જો ((cbd == &clip_plus && (clip_unnamed_saved & CLIP_UNNAMED_PLUS))
		|| (cbd == &clip_star && (clip_unnamed_saved & CLIP_UNNAMED)))
	{
	    clipboard_needs_update = TRUE;
	    પાછા;
	}
    }
# જો defined(FEAT_XCLIPBOARD) || defined(FEAT_WAYLAND_CLIPBOARD)
#  ifdef FEAT_GUI
    જો (gui.in_use)
	clip_mch_set_selection(cbd);
    નહીંતર
#  endif
    {
	જો (clipmethod == CLIPMETHOD_WAYLAND)
	{
#  ifdef FEAT_WAYLAND_CLIPBOARD
	    clip_wl_set_selection(cbd);
#  endif
	}
	નહીંતર જો (clipmethod == CLIPMETHOD_X11)
	{
#  ifdef FEAT_XCLIPBOARD
	    clip_xterm_set_selection(cbd);
#  endif
	}
    }
# નહીંતર
    clip_mch_set_selection(cbd);
# endif
}

    સ્થિર શૂન્ય
clip_gen_request_selection(Clipboard_T *cbd)
{
# જો defined(FEAT_XCLIPBOARD) || defined(FEAT_WAYLAND_CLIPBOARD)
#  ifdef FEAT_GUI
    જો (gui.in_use)
	clip_mch_request_selection(cbd);
    નહીંતર
#  endif
    {
	જો (clipmethod == CLIPMETHOD_WAYLAND)
	{
#  ifdef FEAT_WAYLAND_CLIPBOARD
	    clip_wl_request_selection(cbd);
#  endif
	}
	નહીંતર જો (clipmethod == CLIPMETHOD_X11)
	{
#  ifdef FEAT_XCLIPBOARD
	    clip_xterm_request_selection(cbd);
#  endif
	}
    }
# નહીંતર
    clip_mch_request_selection(cbd);
# endif
}

# જો (defined(FEAT_X11) && defined(FEAT_XCLIPBOARD) && defined(USE_SYSTEM))
    સ્થિર પૂર્ણાંક
clip_x11_owner_exists(Clipboard_T *cbd)
{
    પાછા XGetSelectionOwner(X_DISPLAY, cbd->sel_atom) != None;
}
# endif

# જો (defined(FEAT_X11) || defined(FEAT_WAYLAND_CLIPBOARD)) \
	&& defined(USE_SYSTEM)
    પૂર્ણાંક
clip_gen_owner_exists(Clipboard_T *cbd UNUSED)
{
#  જો defined(FEAT_XCLIPBOARD) || defined(FEAT_WAYLAND_CLIPBOARD)
#   ifdef FEAT_GUI_GTK
    જો (gui.in_use)
	પાછા clip_gtk_owner_exists(cbd);
    નહીંતર
#   endif
    {
	જો (clipmethod == CLIPMETHOD_WAYLAND)
	{
#   ifdef FEAT_WAYLAND_CLIPBOARD
	    પાછા clip_wl_owner_exists(cbd);
#   endif
	}
	નહીંતર જો (clipmethod == CLIPMETHOD_X11)
	{
#   ifdef FEAT_XCLIPBOARD
	    પાછા clip_x11_owner_exists(cbd);
#   endif
	}
	નહીંતર
	    પાછા FALSE;
    }
#  નહીંતર
    પાછા TRUE;
#  endif
}
# endif

/*
 * Stuff for the X clipboard.  Shared between VMS and Unix.
 */

# જો defined(FEAT_XCLIPBOARD) || defined(FEAT_GUI_X11)
#  include <X11/Xatom.h>
#  include <X11/Intrinsic.h>

/*
 * Open the application context (if it hasn't been opened yet).
 * Used for Motif GUI and the xterm clipboard.
 */
    શૂન્ય
open_app_context(શૂન્ય)
{
    જો (app_context == ખાલી)
    {
	XtToolkitInitialize();
	app_context = XtCreateApplicationContext();
    }
}

સ્થિર Atom	vim_atom;	// Vim's own special selection format
સ્થિર Atom	vimenc_atom;	// Vim's extended selection format
સ્થિર Atom	vim_mt_atom;	// Vim's own special selection format (in mime
				// type format)
સ્થિર Atom	vimenc_mt_atom;	// Vim's extended selection format (in mime type
				// format)
સ્થિર Atom	utf8_atom;
સ્થિર Atom	compound_text_atom;
સ્થિર Atom	text_atom;
સ્થિર Atom	targets_atom;
સ્થિર Atom	timestamp_atom;	// Used to get a timestamp

    શૂન્ય
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

    સ્થિર Boolean
clip_x11_convert_selection_cb(
    Widget	w UNUSED,
    Atom	*sel_atom,
    Atom	*target,
    Atom	*type,
    XtPointer	*કિંમત,
    long_u	*length,
    પૂર્ણાંક		*format)
{
    સ્થિર char_u   *save_result = ખાલી;
    સ્થિર long_u   save_length = 0;
    char_u	    *string;
    પૂર્ણાંક		    motion_type;
    Clipboard_T    *cbd;
    પૂર્ણાંક		    i;

    જો (*sel_atom == clip_plus.sel_atom)
	cbd = &clip_plus;
    નહીંતર
	cbd = &clip_star;

    જો (!cbd->owned)
	પાછા False;	    // Shouldn't ever happen

    // requestor wants to know what target types we support
    જો (*target == targets_atom)
    {
	સ્થિર Atom array[9];

	*કિંમત = (XtPointer)array;
	i = 0;
	array[i++] = targets_atom;
	array[i++] = vimenc_atom;
	array[i++] = vim_atom;
	array[i++] = vimenc_mt_atom;
	array[i++] = vim_mt_atom;
	જો (enc_utf8)
	    array[i++] = utf8_atom;
	array[i++] = XA_STRING;
	array[i++] = text_atom;
	array[i++] = compound_text_atom;

	*type = XA_ATOM;
	// This used to be: *format = sizeof(Atom) * 8; but that caused
	// crashes on 64 bit machines. (Peter Derr)
	*format = 32;
	*length = i;
	પાછા True;
    }

    જો (       *target != XA_STRING
	    && *target != vimenc_atom
	    && *target != vimenc_mt_atom
	    && (*target != utf8_atom || !enc_utf8)
	    && *target != vim_atom
	    && *target != vim_mt_atom
	    && *target != text_atom
	    && *target != compound_text_atom)
	પાછા False;

    clip_get_selection(cbd);
    motion_type = clip_convert_selection(&string, length, cbd);
    જો (motion_type < 0)
	પાછા False;

    // For our own format, the first byte contains the motion type
    જો (*target == vim_atom || *target == vim_mt_atom)
	(*length)++;

    // Our own format with encoding: motion 'encoding' NUL text
    જો (*target == vimenc_atom || *target == vimenc_mt_atom)
	*length += STRLEN(p_enc) + 2;

    જો (save_length < *length || save_length / 2 >= *length)
	*કિંમત = XtRealloc((અક્ષર *)save_result, (Cardinal)*length + 1);
    નહીંતર
	*કિંમત = save_result;
    જો (*કિંમત == ખાલી)
    {
	vim_free(string);
	પાછા False;
    }
    save_result = (char_u *)*કિંમત;
    save_length = *length;

    જો (*target == XA_STRING || (*target == utf8_atom && enc_utf8))
    {
	mch_memmove(save_result, string, (size_t)(*length));
	*type = *target;
    }
    નહીંતર જો (*target == compound_text_atom || *target == text_atom)
    {
	XTextProperty	text_prop;
	અક્ષર		*string_nt = (અક્ષર *)save_result;
	પૂર્ણાંક		conv_result;

	// create NUL terminated string which XmbTextListToTextProperty wants
	mch_memmove(string_nt, string, (size_t)*length);
	string_nt[*length] = NUL;
	conv_result = XmbTextListToTextProperty(X_DISPLAY, &string_nt,
					   1, XCompoundTextStyle, &text_prop);
	જો (conv_result != Success)
	{
	    vim_free(string);
	    પાછા False;
	}
	*કિંમત = (XtPointer)(text_prop.કિંમત);	//    from plain text
	*length = text_prop.nitems;
	*type = compound_text_atom;
	XtFree((અક્ષર *)save_result);
	save_result = (char_u *)*કિંમત;
	save_length = *length;
    }
    નહીંતર જો (*target == vimenc_atom || *target == vimenc_mt_atom)
    {
	પૂર્ણાંક l = STRLEN(p_enc);

	save_result[0] = motion_type;
	STRCPY(save_result + 1, p_enc);
	mch_memmove(save_result + l + 2, string, (size_t)(*length - l - 2));
	જો (*target == vimenc_atom)
	    *type = vimenc_atom;
	નહીંતર
	    *type = vimenc_mt_atom;
    }
    નહીંતર
    {
	save_result[0] = motion_type;
	mch_memmove(save_result + 1, string, (size_t)(*length - 1));
	જો (*target == vim_atom)
	    *type = vim_atom;
	નહીંતર
	    *type = vim_mt_atom;
    }
    *format = 8;	    // 8 bits per char
    vim_free(string);
    પાછા True;
}

    સ્થિર શૂન્ય
clip_x11_lose_ownership_cb(Widget w UNUSED, Atom *sel_atom)
{
    જો (*sel_atom == clip_plus.sel_atom)
	clip_lose_selection(&clip_plus);
    નહીંતર
	clip_lose_selection(&clip_star);
}

    સ્થિર શૂન્ય
clip_x11_notify_cb(Widget w UNUSED, Atom *sel_atom UNUSED, Atom *target UNUSED)
{
    // To prevent automatically freeing the selection value.
}

/*
 * Property callback to get a timestamp for XtOwnSelection.
 */
#  જો defined(FEAT_X11) && defined(FEAT_XCLIPBOARD)
    સ્થિર શૂન્ય
clip_x11_timestamp_cb(
    Widget	w,
    XtPointer	n UNUSED,
    XEvent	*event,
    Boolean	*cont UNUSED)
{
    Atom	    actual_type;
    પૂર્ણાંક		    format;
    unsigned  long  nitems, bytes_after;
    unsigned અક્ષર   *prop=ખાલી;
    XPropertyEvent  *xproperty=&event->xproperty;

    // Must be a property notify, state can't be Delete (True), has to be
    // one of the supported selection types.
    જો (event->type != PropertyNotify || xproperty->state
	    || (xproperty->atom != clip_star.sel_atom
				    && xproperty->atom != clip_plus.sel_atom))
	પાછા;

    જો (XGetWindowProperty(xproperty->display, xproperty->window,
	  xproperty->atom, 0, 0, False, timestamp_atom, &actual_type, &format,
						&nitems, &bytes_after, &prop))
	પાછા;

    જો (prop)
	XFree(prop);

    // Make sure the property type is "TIMESTAMP" and it's 32 bits.
    જો (actual_type != timestamp_atom || format != 32)
	પાછા;

    // Get the selection, using the event timestamp.
    જો (XtOwnSelection(w, xproperty->atom, xproperty->time,
	    clip_x11_convert_selection_cb, clip_x11_lose_ownership_cb,
	    clip_x11_notify_cb) == OK)
    {
	// Set the "owned" flag now, there may have been a call to
	// lose_ownership_cb in between.
	જો (xproperty->atom == clip_plus.sel_atom)
	    clip_plus.owned = TRUE;
	નહીંતર
	    clip_star.owned = TRUE;
    }
}

    શૂન્ય
x11_setup_selection(Widget w)
{
    XtAddEventHandler(w, PropertyChangeMask, False,
	    /*(XtEventHandler)*/clip_x11_timestamp_cb, (XtPointer)ખાલી);
}
#  endif

    સ્થિર શૂન્ય
clip_x11_request_selection_cb(
    Widget	w UNUSED,
    XtPointer	success,
    Atom	*sel_atom,
    Atom	*type,
    XtPointer	કિંમત,
    long_u	*length,
    પૂર્ણાંક		*format)
{
    પૂર્ણાંક		motion_type = MAUTO;
    long_u	len;
    char_u	*p;
    અક્ષર	**text_list = ખાલી;
    Clipboard_T	*cbd;
    char_u	*tmpbuf = ખાલી;

    જો (*sel_atom == clip_plus.sel_atom)
	cbd = &clip_plus;
    નહીંતર
	cbd = &clip_star;

    જો (કિંમત == ખાલી || *length == 0)
    {
	clip_free_selection(cbd);	// nothing received, clear register
	*(પૂર્ણાંક *)success = FALSE;
	પાછા;
    }
    p = (char_u *)કિંમત;
    len = *length;
    જો (*type == vim_atom || *type == vim_mt_atom)
    {
	motion_type = *p++;
	len--;
    }

    નહીંતર જો (*type == vimenc_atom || *type == vimenc_mt_atom)
    {
	char_u		*enc;
	vimconv_T	conv;
	પૂર્ણાંક		convlen;

	motion_type = *p++;
	--len;

	enc = p;
	p += STRLEN(p) + 1;
	len -= p - enc;

	// If the encoding of the text is different from 'encoding', attempt
	// converting it.
	conv.vc_type = CONV_NONE;
	convert_setup(&conv, enc, p_enc);
	જો (conv.vc_type != CONV_NONE)
	{
	    convlen = len;	// Need to use an int here.
	    tmpbuf = string_convert(&conv, p, &convlen);
	    len = convlen;
	    જો (tmpbuf != ખાલી)
		p = tmpbuf;
	    convert_setup(&conv, ખાલી, ખાલી);
	}
    }

    નહીંતર જો (*type == compound_text_atom
	    || *type == utf8_atom
	    || (enc_dbcs != 0 && *type == text_atom))
    {
	XTextProperty	text_prop;
	પૂર્ણાંક		n_text = 0;
	પૂર્ણાંક		status;

	text_prop.કિંમત = (unsigned અક્ષર *)કિંમત;
	text_prop.encoding = *type;
	text_prop.format = *format;
	text_prop.nitems = len;
#  જો defined(X_HAVE_UTF8_STRING)
	જો (*type == utf8_atom)
	    status = Xutf8TextPropertyToTextList(X_DISPLAY, &text_prop,
							 &text_list, &n_text);
	નહીંતર
#  endif
	    status = XmbTextPropertyToTextList(X_DISPLAY, &text_prop,
							 &text_list, &n_text);
	જો (status != Success || n_text < 1)
	{
	    *(પૂર્ણાંક *)success = FALSE;
	    પાછા;
	}
	p = (char_u *)text_list[0];
	len = STRLEN(p);
    }
    clip_yank_selection(motion_type, p, (long)len, cbd);

    જો (text_list != ખાલી)
	XFreeStringList(text_list);
    vim_free(tmpbuf);
    XtFree((અક્ષર *)કિંમત);
    *(પૂર્ણાંક *)success = TRUE;
}

    શૂન્ય
clip_x11_request_selection(
    Widget	myShell,
    Display	*dpy,
    Clipboard_T	*cbd)
{
    XEvent	event;
    Atom	type;
    સ્થિર પૂર્ણાંક	success;
    પૂર્ણાંક		i;
    time_t	start_time;
    પૂર્ણાંક		timed_out = FALSE;

    માટે (i = 0; i < 8; i++)
    {
	પસંદગી (i)
	{
	    સ્થિતિ 0:  type = vimenc_mt_atom;	તોડો;
	    સ્થિતિ 1:  type = vimenc_atom;	તોડો;
	    સ્થિતિ 2:  type = vim_mt_atom;	તોડો;
	    સ્થિતિ 3:  type = vim_atom;		તોડો;
	    સ્થિતિ 4:  type = utf8_atom;		તોડો;
	    સ્થિતિ 5:  type = compound_text_atom; તોડો;
	    સ્થિતિ 6:  type = text_atom;		તોડો;
	    મૂળભૂત: type = XA_STRING;
	}
	જો (type == utf8_atom
#  જો defined(X_HAVE_UTF8_STRING)
		&& !enc_utf8
#  endif
		)
	    // Only request utf-8 when 'encoding' is utf8 and
	    // Xutf8TextPropertyToTextList is available.
	    ચાલુ;
	success = MAYBE;
	Xt_Get_Selection_કિંમત(myShell, cbd->sel_atom, type,
	    clip_x11_request_selection_cb, (XtPointer)&success, CurrentTime);

	// Make sure the request for the selection goes out before waiting for
	// a response.
	XFlush(dpy);

	/*
	 * Wait for result of selection request, otherwise if we type more
	 * characters, then they will appear before the one that requested the
	 * paste!  Don't worry, we will catch up with any other events later.
	 */
	start_time = time(ખાલી);
	જ્યારે (success == MAYBE)
	{
	    જો (XCheckTypedEvent(dpy, PropertyNotify, &event)
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
		ચાલુ;
	    }

	    // Time out after 2 to 3 seconds to avoid that we hang when the
	    // other process doesn't respond.  Note that the SelectionNotify
	    // event may still come later when the selection owner comes back
	    // to life and the text gets inserted unexpectedly.  Don't know
	    // why that happens or how to avoid that :-(.
	    જો (time(ખાલી) > start_time + 2)
	    {
		timed_out = TRUE;
		તોડો;
	    }

	    // Do we need this?  Probably not.
	    XSync(dpy, False);

	    // Wait for 1 msec to avoid that we eat up all CPU time.
	    ui_delay(1L, TRUE);
	}

	જો (success == TRUE)
	    પાછા;

	// don't do a retry with another type after timing out, otherwise we
	// hang for 15 seconds.
	જો (timed_out)
	    તોડો;
    }

    // Final fallback position - use the X CUT_BUFFER0 store
    yank_cut_buffer0(dpy, cbd);
}

    શૂન્ય
clip_x11_lose_selection(Widget myShell, Clipboard_T *cbd)
{
    XtDisownSelection(myShell, cbd->sel_atom,
				XtLastTimestampProcessed(XtDisplay(myShell)));
}

    પૂર્ણાંક
clip_x11_own_selection(Widget myShell, Clipboard_T *cbd)
{
    // When using the GUI we have proper timestamps, use the one of the last
    // event.  When in the console we don't get events (the terminal gets
    // them), Get the time by a zero-length append, clip_x11_timestamp_cb will
    // be called with the current timestamp.
#  ifdef FEAT_GUI
    જો (gui.in_use)
    {
	જો (XtOwnSelection(myShell, cbd->sel_atom,
	       XtLastTimestampProcessed(XtDisplay(myShell)),
	       clip_x11_convert_selection_cb, clip_x11_lose_ownership_cb,
	       clip_x11_notify_cb) == False)
	    પાછા FAIL;
    }
    નહીંતર
#  endif
    {
	જો (!XChangeProperty(XtDisplay(myShell), XtWindow(myShell),
		  cbd->sel_atom, timestamp_atom, 32, PropModeAppend, ખાલી, 0))
	    પાછા FAIL;
    }
    // Flush is required in a terminal as nothing else is doing it.
    XFlush(XtDisplay(myShell));
    પાછા OK;
}

/*
 * Send the current selection to the clipboard.  Do nothing for X because we
 * will fill in the selection only when requested by another app.
 */
    શૂન્ય
clip_x11_set_selection(Clipboard_T *cbd UNUSED)
{
}

# endif

# જો (defined(FEAT_XCLIPBOARD) || defined(FEAT_GUI_X11) || defined(FEAT_GUI_GTK)) \
	&& !defined(USE_GTK4)
/*
 * Get the contents of the X CUT_BUFFER0 and put it in "cbd".
 */
    શૂન્ય
yank_cut_buffer0(Display *dpy, Clipboard_T *cbd)
{
    પૂર્ણાંક		nbytes = 0;
    char_u	*buffer = (char_u *)XFetchBuffer(dpy, &nbytes, 0);

    જો (nbytes > 0)
    {
	પૂર્ણાંક  done = FALSE;

	// CUT_BUFFER0 is supposed to be always latin1.  Convert to 'enc' when
	// using a multi-byte encoding.  Conversion between two 8-bit
	// character sets usually fails and the text might actually be in
	// 'enc' anyway.
	જો (has_mbyte)
	{
	    char_u	*conv_buf;
	    vimconv_T	vc;

	    vc.vc_type = CONV_NONE;
	    જો (convert_setup(&vc, (char_u *)"latin1", p_enc) == OK)
	    {
		conv_buf = string_convert(&vc, buffer, &nbytes);
		જો (conv_buf != ખાલી)
		{
		    clip_yank_selection(MCHAR, conv_buf, (long)nbytes, cbd);
		    vim_free(conv_buf);
		    done = TRUE;
		}
		convert_setup(&vc, ખાલી, ખાલી);
	    }
	}
	જો (!done)  // use the text without conversion
	    clip_yank_selection(MCHAR, buffer, (long)nbytes, cbd);
	XFree((શૂન્ય *)buffer);
	જો (p_verbose > 0)
	{
	    verbose_enter();
	    verb_સંદેશ(_("Used CUT_BUFFER0 instead of empty selection"));
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
# જો defined(FEAT_X11) && defined(FEAT_CLIPBOARD)
    શૂન્ય
x11_export_final_selection(શૂન્ય)
{
    Display	*dpy;
    char_u	*str = ખાલી;
    long_u	len = 0;
    પૂર્ણાંક		motion_type = -1;

#  ifdef FEAT_GUI
    જો (gui.in_use)
	dpy = X_DISPLAY;
    નહીંતર
#  endif
#  ifdef FEAT_XCLIPBOARD
	dpy = xterm_dpy;
#  નહીંતર
	પાછા;
#  endif

    // Get selection to export
    જો (clip_plus.owned)
	motion_type = clip_convert_selection(&str, &len, &clip_plus);
    નહીંતર જો (clip_star.owned)
	motion_type = clip_convert_selection(&str, &len, &clip_star);

    // Check it's OK
    જો (dpy != ખાલી && str != ખાલી && motion_type >= 0
					       && len < 1024*1024 && len > 0)
    {
	પૂર્ણાંક ok = TRUE;

	// The CUT_BUFFER0 is supposed to always contain latin1.  Convert from
	// 'enc' when it is a multi-byte encoding.  When 'enc' is an 8-bit
	// encoding conversion usually doesn't work, so keep the text as-is.
	જો (has_mbyte)
	{
	    vimconv_T	vc;

	    vc.vc_type = CONV_NONE;
	    જો (convert_setup(&vc, p_enc, (char_u *)"latin1") == OK)
	    {
		પૂર્ણાંક	intlen = len;
		char_u	*conv_str;

		vc.vc_fail = TRUE;
		conv_str = string_convert(&vc, str, &intlen);
		len = intlen;
		જો (conv_str != ખાલી)
		{
		    vim_free(str);
		    str = conv_str;
		}
		નહીંતર
		{
		    ok = FALSE;
		}
		convert_setup(&vc, ખાલી, ખાલી);
	    }
	    નહીંતર
	    {
		ok = FALSE;
	    }
	}

	// Do not store the string if conversion failed.  Better to use any
	// other selection than garbled text.
	જો (ok)
	{
	    XStoreBuffer(dpy, (અક્ષર *)str, (પૂર્ણાંક)len, 0);
	    XFlush(dpy);
	}
    }

    vim_free(str);
}
# endif

    શૂન્ય
clip_free_selection(Clipboard_T *cbd)
{
    yankreg_T *y_ptr = get_y_current();

    જો (cbd == &clip_plus)
	set_y_current(get_y_register(PLUS_REGISTER));
    નહીંતર
	set_y_current(get_y_register(STAR_REGISTER));
    free_yank_all();
    get_y_current()->y_size = 0;
    set_y_current(y_ptr);
}

/*
 * Get the selected text and put it in register '*' or '+'.
 */
    શૂન્ય
clip_get_selection(Clipboard_T *cbd)
{
    yankreg_T	*old_y_previous, *old_y_current;
    pos_T	old_cursor;
    pos_T	old_visual;
    પૂર્ણાંક		old_visual_mode;
    colnr_T	old_virtcol;
    colnr_T	old_curswant;
    પૂર્ણાંક		old_set_curswant;
    pos_T	old_op_start, old_op_end;
    oparg_T	oa;
    cmdarg_T	ca;

    જો (cbd->owned)
    {
	જો ((cbd == &clip_plus
		&& get_y_register(PLUS_REGISTER)->y_array != ખાલી)
		|| (cbd == &clip_star
		    && get_y_register(STAR_REGISTER)->y_array != ખાલી))
	    પાછા;

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
    નહીંતર જો (!is_clipboard_needs_update())
    {
	clip_free_selection(cbd);

	// Try to get selected text from another window
	clip_gen_request_selection(cbd);
    }
}

/*
 * Convert from the GUI selection string into the '*'/'+' register.
 */
    શૂન્ય
clip_yank_selection(
    પૂર્ણાંક		type,
    char_u	*str,
    long	len,
    Clipboard_T *cbd)
{
    yankreg_T *y_ptr;

    જો (cbd == &clip_plus)
	y_ptr = get_y_register(PLUS_REGISTER);
    નહીંતર
	y_ptr = get_y_register(STAR_REGISTER);

    clip_free_selection(cbd);

    str_to_reg(y_ptr, type, str, len, -1, FALSE);
}

    પૂર્ણાંક
clip_convert_selection_offset(
	char_u	    **str,
	long_u	    *len,
	પૂર્ણાંક	    offset, // Extra space to add in *str and the offset to
			    // place the actual string in *str.
	Clipboard_T *cbd)
{
    char_u	*p;
    પૂર્ણાંક		lnum;
    પૂર્ણાંક		i, j;
    int_u	eolsize;
    yankreg_T	*y_ptr;

    જો (cbd == &clip_plus)
	y_ptr = get_y_register(PLUS_REGISTER);
    નહીંતર
	y_ptr = get_y_register(STAR_REGISTER);

# ifdef USE_CRNL
    eolsize = 2;
# નહીંતર
    eolsize = 1;
# endif

    *str = ખાલી;
    *len = 0;
    જો (y_ptr->y_array == ખાલી)
	પાછા -1;

    માટે (i = 0; i < y_ptr->y_size; i++)
	*len += (long_u)y_ptr->y_array[i].length + eolsize;

    // Don't want newline character at end of last line if we're in MCHAR mode.
    જો (y_ptr->y_type == MCHAR && *len >= eolsize)
	*len -= eolsize;

    *len += offset;
    p = *str = alloc(*len + 1);	// add one to avoid zero
    જો (p == ખાલી)
	પાછા -1;
    p += offset;
    lnum = 0;
    માટે (i = 0, j = 0; i < (પૂર્ણાંક)*len - offset; i++, j++)
    {
	જો (y_ptr->y_array[lnum].string[j] == '\n')
	    p[i] = NUL;
	નહીંતર જો (y_ptr->y_array[lnum].string[j] == NUL)
	{
# ifdef USE_CRNL
	    p[i++] = '\r';
# endif
	    p[i] = '\n';
	    lnum++;
	    j = -1;
	}
	નહીંતર
	    p[i] = y_ptr->y_array[lnum].string[j];
    }
    પાછા y_ptr->y_type;
}

/*
 * Convert the '*'/'+' register into a GUI selection string returned in *str
 * with length *len.
 * Returns the motion type, or -1 for failure.
 */
    પૂર્ણાંક
clip_convert_selection(char_u **str, long_u *len, Clipboard_T *cbd)
{
    પાછા clip_convert_selection_offset(str, len, 0, cbd);
}

/*
 * When "regname" is a clipboard register, obtain the selection.  If it's not
 * available return zero, otherwise return "regname".
 */
    પૂર્ણાંક
may_get_selection(પૂર્ણાંક regname)
{
    જો (regname == '*')
    {
	જો (!clip_star.available)
	    regname = 0;
	નહીંતર
	    clip_get_selection(&clip_star);
    }
    નહીંતર જો (regname == '+')
    {
	જો (!clip_plus.available)
	    regname = 0;
	નહીંતર
	    clip_get_selection(&clip_plus);
    }
    પાછા regname;
}

/*
 * If we have written to a clipboard register, send the text to the clipboard.
 */
    શૂન્ય
may_set_selection(શૂન્ય)
{
    જો ((get_y_current() == get_y_register(STAR_REGISTER))
	    && clip_star.available)
    {
	clip_own_selection(&clip_star);
	clip_gen_set_selection(&clip_star);
    }
    નહીંતર જો ((get_y_current() == get_y_register(PLUS_REGISTER))
	    && clip_plus.available)
    {
	clip_own_selection(&clip_plus);
	clip_gen_set_selection(&clip_plus);
    }
}

# જો defined(FEAT_WAYLAND_CLIPBOARD)

    સ્થિર clip_wl_selection_T *
clip_wl_get_selection(wayland_selection_T sel)
{
    પસંદગી (sel)
    {
	સ્થિતિ WAYLAND_SELECTION_REGULAR:
	    પાછા &clip_wl.regular;
	સ્થિતિ WAYLAND_SELECTION_PRIMARY:
	    પાછા &clip_wl.primary;
	મૂળભૂત:
	    પાછા ખાલી;
    }
}

    સ્થિર clip_wl_selection_T *
clip_wl_get_selection_from_cbd(Clipboard_T *cbd)
{
    જો (cbd == &clip_plus)
	પાછા &clip_wl.regular;
    નહીંતર જો (cbd == &clip_star)
	પાછા &clip_wl.primary;
    નહીંતર
	પાછા ખાલી;
}

    સ્થિર Clipboard_T *
clip_wl_get_cbd_from_selection(clip_wl_selection_T *sel)
{
    જો (sel == &clip_wl.regular)
	પાછા &clip_plus;
    નહીંતર જો (sel == &clip_wl.primary)
	પાછા &clip_star;
    નહીંતર
	પાછા ખાલી;
}

    સ્થિર wayland_selection_T
clip_wl_get_selection_type(clip_wl_selection_T *sel)
{
    જો (sel == &clip_wl.regular)
	પાછા WAYLAND_SELECTION_REGULAR;
    નહીંતર જો (sel == &clip_wl.primary)
	પાછા WAYLAND_SELECTION_PRIMARY;
    નહીંતર
	પાછા WAYLAND_SELECTION_NONE;
}

    સ્થિર bool
wl_data_offer_listener_event_offer(
    શૂન્ય *data UNUSED,
    vwl_data_offer_T *offer UNUSED,
    અચલ અક્ષર *mime_type
)
{
    // Only accept mime type if we support it
    માટે (પૂર્ણાંક i = 0; i < (પૂર્ણાંક)ARRAY_LENGTH(supported_mimes); i++)
	જો (STRCMP(mime_type, supported_mimes[i]) == 0)
	    પાછા true;
    પાછા FALSE;
}

સ્થિર અચલ vwl_data_offer_listener_T vwl_data_offer_listener = {
    .offer = wl_data_offer_listener_event_offer
};

    સ્થિર શૂન્ય
vwl_data_device_listener_event_data_offer(
	શૂન્ય *data UNUSED,
	vwl_data_device_T *device UNUSED,
	vwl_data_offer_T *offer)
{
    // Immediately start listening for offer events from the data offer
    vwl_data_offer_add_listener(offer, &vwl_data_offer_listener, ખાલી);
}

    સ્થિર શૂન્ય
vwl_data_device_listener_event_selection(
	શૂન્ય *data UNUSED,
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
    જો (offer == ખાલી || sel->source != ખાલી)
    {
	// Selection event is from us, so we are the source client. Therefore
	// ignore it. Or the selection is cleared, so set sel->offer to NULL
	vwl_data_offer_destroy(offer);
	sel->offer = ખાલી;
	પાછા;
    }

    // Save offer. When we want to request data, then we'll actually call the
    // receive method.
    sel->offer = offer;

}

    સ્થિર શૂન્ય
vwl_data_device_listener_event_finished(
	શૂન્ય *data UNUSED,
	vwl_data_device_T *device)
{
    clip_wl_selection_T *sel;
    // Device finished, guessing this can happen is when the seat becomes
    // invalid? If so, let the user call :wlrestore! to reset. There wouldn't be
    // any point in trying to create another data device for the same seat,
    // since the seat is in an invalid state.
    જો (device == clip_wl.regular.device)
    {
	sel = &clip_wl.regular;
	clip_wl.regular.device = ખાલી;
    }
    નહીંતર જો (device == clip_wl.primary.device)
    {
	sel = &clip_wl.primary;
	clip_wl.primary.device = ખાલી;
    }
    નહીંતર
	// Shouldn't happen
	પાછા;

    vim_free(sel->contents);
    vwl_data_source_destroy(sel->source);
    vwl_data_offer_destroy(sel->offer);
    sel->available = FALSE;

    vwl_data_device_destroy(device);
}

સ્થિર અચલ vwl_data_device_listener_T vwl_data_device_listener = {
    .data_offer = vwl_data_device_listener_event_data_offer,
    .selection = vwl_data_device_listener_event_selection,
    .finished = vwl_data_device_listener_event_finished
};

/*
 * Initialize the clipboard for Wayland using the global Wayland connection.
 * Returns OK on success and FAIL on failure.
 */
    પૂર્ણાંક
clip_init_wayland(શૂન્ય)
{
    int_u supported = WAYLAND_SELECTION_NONE;

    જો (wayland_ct == ખાલી)
	પાછા FAIL;

    memset(&clip_wl, 0, કદ(clip_wl));

    clip_wl.seat = vwl_connection_get_seat(wayland_ct, (અક્ષર *)p_wse);

    જો (clip_wl.seat == ખાલી)
	પાછા FAIL;

    clip_wl.regular.manager = vwl_connection_get_data_device_manager(
	    wayland_ct, WAYLAND_SELECTION_REGULAR, &supported);

    જો (clip_wl.regular.manager != ખાલી)
    {
	clip_wl.regular.device = vwl_data_device_manager_get_data_device(
		clip_wl.regular.manager, clip_wl.seat);

	જો (clip_wl.regular.device != ખાલી)
	    clip_wl.regular.available = true;
	નહીંતર
	{
	    // Shouldn't happen
	    vwl_data_device_manager_discard(clip_wl.regular.manager);
	    clip_wl.regular.manager = ખાલી;
	    પાછા FAIL;
	}
    }

    // If we still don't support the primary selection, try finding one for it
    // specifically.
    જો (!(supported & WAYLAND_SELECTION_PRIMARY))
    {
	clip_wl.primary.manager = vwl_connection_get_data_device_manager(
		wayland_ct, WAYLAND_SELECTION_PRIMARY, &supported);

	જો (clip_wl.primary.manager != ખાલી)
	{
	    clip_wl.primary.device = vwl_data_device_manager_get_data_device(
		    clip_wl.primary.manager, clip_wl.seat);

	    જો (clip_wl.primary.device != ખાલી)
		clip_wl.primary.available = true;
	    નહીંતર
	    {
		vwl_data_device_manager_discard(clip_wl.primary.manager);
		clip_wl.primary.manager = ખાલી;
	    }
	}
    }

    જો (clip_wl.regular.available && !clip_wl.primary.available)
    {
	// The protocol supports both regular and primary selections, just use
	// one data device manager and one data device. Or the primary selection
	// is not supported, make it point to the regular selection instead.
	clip_wl.primary.available = true;
	clip_wl.primary.manager = clip_wl.regular.manager;
	clip_wl.primary.device = clip_wl.regular.device;
    }

    જો (!clip_wl.regular.available && !clip_wl.primary.available)
	પાછા FAIL;

    // Start listening for selection updates
    જો (clip_wl.regular.device != ખાલી)
	vwl_data_device_add_listener(clip_wl.regular.device,
		&vwl_data_device_listener, ખાલી);
    // Don't want to listen to the same data device twice
    જો (clip_wl.primary.device != ખાલી
	    && clip_wl.primary.device != clip_wl.regular.device)
	vwl_data_device_add_listener(clip_wl.primary.device,
		&vwl_data_device_listener, ખાલી);

    પાછા OK;
}

    શૂન્ય
clip_uninit_wayland(શૂન્ય)
{
    clip_wl_selection_T *sel;

    જો (clipmethod == CLIPMETHOD_WAYLAND)
    {
	જો (clip_star.owned)
	    clip_lose_selection(&clip_star);
	જો (clip_plus.owned)
	    clip_lose_selection(&clip_plus);
    }

    // Don't want to double free
    જો (clip_wl.regular.manager != clip_wl.primary.manager)
	vwl_data_device_manager_discard(clip_wl.primary.manager);
    vwl_data_device_manager_discard(clip_wl.regular.manager);

    જો (clip_wl.regular.device != clip_wl.primary.device)
	vwl_data_device_destroy(clip_wl.primary.device);
    vwl_data_device_destroy(clip_wl.regular.device);

    sel = &clip_wl.regular;
    જ્યારે (true)
    {
	vim_free(sel->contents);
	vwl_data_source_destroy(sel->source);
	vwl_data_offer_destroy(sel->offer);
	sel->available = false;

	જો (sel == &clip_wl.primary)
	    તોડો;
	sel = &clip_wl.primary;
    }

    vim_memset(&clip_wl, 0, કદ(clip_wl));
}

    પૂર્ણાંક
clip_reset_wayland(શૂન્ય)
{
    wayland_uninit_connection();

    જો (wayland_init_connection(wayland_display_name) == FAIL
	    || clip_init_wayland() == FAIL)
	પાછા FAIL;

    choose_clipmethod();
    પાછા OK;
}


/*
 * Read data from a file descriptor and write it to the given clipboard.
 */
    સ્થિર શૂન્ય
clip_wl_receive_data(Clipboard_T *cbd, અચલ અક્ષર *mime_type, પૂર્ણાંક fd)
{
    char_u	*start, *final;
    long	len;
    garray_T	buf;
    પૂર્ણાંક		motion_type = MAUTO;
    ssize_t	r = 0;
    char_u	*tofree = ખાલી;
#  ifndef HAVE_SELECT
    સંરચના pollfd   pfd;

    pfd.fd = fd;
    pfd.events = POLLIN;
#  નહીંતર
    fd_set rfds;
    સંરચના timeval  tv;

    FD_ZERO(&rfds);
    FD_SET(fd, &rfds);
#  endif

    // Make pipe (read end) non-blocking
    જો (fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK) == -1)
	પાછા;

    ga_init2(&buf, 1, 4096);

    // 4096 bytes seems reasonable for initial buffer size, memory is cheap
    // anyways.
    જો (ga_grow(&buf, 4096) == FAIL)
	પાછા;

    start = buf.ga_data;

#  ifndef HAVE_SELECT
    જ્યારે (poll(&pfd, 1, p_wtm) > 0)
#  નહીંતર
    જ્યારે (tv.tv_sec = p_wtm / 1000, tv.tv_usec = (p_wtm % 1000) * 1000,
	    select(fd + 1, &rfds, ખાલી, ખાલી, &tv) > 0)
#  endif
    {
	r = પઠ(fd, start, buf.ga_maxlen - 1 - buf.ga_len);

	જો (r == 0)
	    તોડો;
	નહીંતર જો (r < 0)
	{
	    જો (errno == EAGAIN || errno == EINTR)
		ચાલુ;
	    તોડો;
	}

	start += r;
	buf.ga_len += r;

	// Realloc if we are at the end of the buffer
	જો (buf.ga_len >= buf.ga_maxlen - 1)
	{
	    જો (ga_grow(&buf, 8192) == FAIL)
		તોડો;
	    start = (char_u *)buf.ga_data + buf.ga_len;
	}
    }

    જો (buf.ga_len == 0)
    {
	clip_free_selection(cbd); // Nothing received, clear register
	ga_clear(&buf);
	પાછા;
    }

    final = buf.ga_data;
    len = buf.ga_len;

    જો (clip_convert_data(&final, &len, &motion_type,
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
    સ્થિર શૂન્ય
clip_wl_request_selection(Clipboard_T *cbd)
{
    clip_wl_selection_T *sel = clip_wl_get_selection_from_cbd(cbd);
    પૂર્ણાંક			fds[2];
    પૂર્ણાંક			mime_types_len;
    અચલ અક્ષર		**mime_types;
    અચલ અક્ષર		*chosen_mime = ખાલી;

    જો (!sel->available)
	જાઓ clear;

    // Dispatch any events that still queued up before checking for a data
    // offer.
    જો (vwl_connection_roundtrip(wayland_ct) == FAIL)
	જાઓ clear;

    જો (sel->offer == ખાલી)
	જાઓ clear;

    mime_types_len = sel->offer->mime_types.ga_len;
    mime_types = sel->offer->mime_types.ga_data;

    // Choose mime type to receive from. Mime types with a lower index in the
    // "supported_mimes" array are prioritized over ones after it.
    માટે (પૂર્ણાંક i = 0; i < (પૂર્ણાંક)ARRAY_LENGTH(supported_mimes)
	    && chosen_mime == ખાલી; i++)
    {
	માટે (પૂર્ણાંક k = 0; k < mime_types_len && chosen_mime == ખાલી; k++)
	    જો (STRCMP(mime_types[k], supported_mimes[i]) == 0)
		chosen_mime = supported_mimes[i];
    }

    જો (chosen_mime == ખાલી || pipe(fds) == -1)
	જાઓ clear;

    vwl_data_offer_receive(sel->offer, chosen_mime, fds[1]);

    close(fds[1]); // Close before we read data so that when the source client
		   // closes their end we receive an EOF.

    જો (vwl_connection_flush(wayland_ct) >= 0)
	clip_wl_receive_data(cbd, chosen_mime, fds[0]);

    close(fds[0]);

    પાછા;
clear:
    clip_free_selection(cbd);
}

    સ્થિર શૂન્ય
vwl_data_source_listener_event_send(
    શૂન્ય *data,
    vwl_data_source_T *source UNUSED,
    અચલ અક્ષર *mime_type,
    int32_t fd
)
{
    clip_wl_selection_T *sel = data;
    Clipboard_T		*cbd = clip_wl_get_cbd_from_selection(sel);
    bool		have_mime = false;
    પૂર્ણાંક			motion_type;
    long_u		length;
    char_u		*string; // Will be reallocated to a bigger size if
				 // needed.
    પૂર્ણાંક			offset = 0;
    bool		is_vim, is_vimenc;
    size_t		total = 0;
#  ifndef HAVE_SELECT
    સંરચના pollfd   pfd;

    pfd.fd = fd;
    pfd.events = POLLOUT;
#  નહીંતર
    fd_set	    wfds;
    સંરચના timeval  tv;

    FD_ZERO(&wfds);
    FD_SET(fd, &wfds);
#  endif

    // Check if we actually have mime type
    માટે (પૂર્ણાંક i = 0; i < (પૂર્ણાંક)ARRAY_LENGTH(supported_mimes); i++)
	જો (STRCMP(supported_mimes[i], mime_type) == 0)
	{
	    have_mime = true;
	    તોડો;
	}

    જો (!have_mime)
	જાઓ exit;

    // First byte sent is motion type for vim specific formats. For the vimenc
    // format, after the first byte is the encoding type, which is null
    // terminated.

    is_vimenc = STRCMP(mime_type, VIMENC_ATOM_NAME) == 0
	|| STRCMP(mime_type, VIMENC_MIMETYPE_NAME) == 0;
    is_vim = STRCMP(mime_type, VIM_ATOM_NAME) == 0
	|| STRCMP(mime_type, VIM_MIMETYPE_NAME) == 0;

    જો (is_vimenc)
	offset += 2 + STRLEN(p_enc);
    નહીંતર જો (is_vim)
	offset += 1;

    clip_get_selection(cbd);
    motion_type = clip_convert_selection_offset(&string, &length, offset, cbd);

    જો (motion_type < 0)
	જાઓ exit;

    જો (is_vimenc)
    {
	string[0] = (char_u)motion_type;
	// Use vim_strncpy for safer copying
	vim_strncpy(string + 1, p_enc, STRLEN(p_enc));
    }
    નહીંતર જો (is_vim)
	string[0] = (char_u)motion_type;


    જ્યારે (total < (size_t)length &&
#  ifndef HAVE_SELECT
	    poll(&pfd, 1, p_wtm) > 0)
#  નહીંતર
	    ((tv.tv_sec = p_wtm / 1000, tv.tv_usec = (p_wtm % 1000) * 1000),
	    select(fd + 1, ખાલી, &wfds, ખાલી, &tv) > 0))
#  endif
    {
	ssize_t w = લિખ(fd, string + total, length - total);

	જો (w == -1)
	    તોડો;
	total += w;
    }

    vim_free(string);
exit:
    close(fd);
}

    સ્થિર શૂન્ય
vwl_data_source_listener_event_cancelled(
	શૂન્ય *data,
	vwl_data_source_T *source UNUSED)
{
    clip_wl_selection_T *sel = data;
    Clipboard_T		*cbd = clip_wl_get_cbd_from_selection(sel);

    clip_lose_selection(cbd);
}

સ્થિર અચલ vwl_data_source_listener_T vwl_data_source_listener = {
    .send = vwl_data_source_listener_event_send,
    .cancelled = vwl_data_source_listener_event_cancelled
};

    સ્થિર શૂન્ય
clip_wl_do_set_selection(શૂન્ય *data, uint32_t serial)
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
    સ્થિર પૂર્ણાંક
clip_wl_own_selection(Clipboard_T *cbd)
{
    clip_wl_selection_T *sel = clip_wl_get_selection_from_cbd(cbd);
    wayland_selection_T sel_type = clip_wl_get_selection_type(sel);

    જો (!sel->available || vwl_connection_roundtrip(wayland_ct) == FAIL)
	પાછા FAIL;

    જો (sel->source != ખાલી)
    {
	જો (sel_type == WAYLAND_SELECTION_PRIMARY)
	    // We already own the selection, ignore (only do this for primary
	    // selection). We don't re set the selection because then we would
	    // be setting the selection every time the user moves the visual
	    // selection cursor, which is messy and inefficient. Some
	    // applications like Google Chrome do it this way however.
	    પાછા OK;
	નહીંતર જો (sel_type == WAYLAND_SELECTION_REGULAR)
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
	નહીંતર
	    // Shouldn't happen
	    પાછા FAIL;
    }

    sel->source = vwl_data_device_manager_create_data_source(sel->manager);
    vwl_data_source_add_listener(sel->source, &vwl_data_source_listener, sel);

    // Advertise mime types
    માટે (પૂર્ણાંક i = 0; i < (પૂર્ણાંક)ARRAY_LENGTH(supported_mimes); i++)
	vwl_data_source_offer(sel->source, supported_mimes[i]);

    sel->own_success = false;
    clip_wl_do_set_selection(sel, 0);

    જો (!sel->own_success)
	જાઓ fail;

    પાછા OK;
fail:
    vwl_data_source_destroy(sel->source);
    sel->source = ખાલી;
    પાછા FAIL;
}

/*
 * Disown the selection that cbd corresponds to.
 */
    સ્થિર શૂન્ય
clip_wl_lose_selection(Clipboard_T *cbd)
{
    clip_wl_selection_T *sel = clip_wl_get_selection_from_cbd(cbd);

    જો (!sel->available)
	પાછા;

    vwl_data_source_destroy(sel->source);
    sel->source = ખાલી;
}

/*
 * Send the current selection to the clipboard. Do nothing for Wayland because
 * we will fill in the selection only when requested by another client.
 */
    સ્થિર શૂન્ય
clip_wl_set_selection(Clipboard_T *cbd UNUSED)
{
}

#  જો defined(USE_SYSTEM)
/*
 * Return true if we own the selection corresponding to cbd or another client
 * does.
 */
    સ્થિર bool
clip_wl_owner_exists(Clipboard_T *cbd)
{
    clip_wl_selection_T *sel = clip_wl_get_selection_from_cbd(cbd);

    જો (vwl_connection_roundtrip(wayland_ct) == FAIL)
	પાછા false;

    પાછા sel->available && (sel->source != ખાલી || sel->offer != ખાલી);
}
#  endif

# endif // FEAT_WAYLAND_CLIPBOARD

#endif // FEAT_CLIPBOARD

#ifdef HAVE_CLIPMETHOD

/*
 * Returns the first method for accessing the clipboard that is available/works,
 * depending on the order of values in str.
 */
    સ્થિર clipmethod_T
get_clipmethod(char_u *str)
{
    પૂર્ણાંક		len	= (પૂર્ણાંક)STRLEN(str) + 1;
    char_u	*buf	= alloc(len);

    જો (buf == ખાલી)
	પાછા CLIPMETHOD_FAIL;

    clipmethod_T ret = CLIPMETHOD_FAIL;
    char_u	*p = str;

    જ્યારે (*p != NUL)
    {
	clipmethod_T method = CLIPMETHOD_NONE;

	(શૂન્ય)copy_option_part(&p, buf, len, ",");

	જો (STRCMP(buf, "wayland") == 0)
	{
# ifdef FEAT_GUI
	    જો (!gui.in_use)
# endif
	    {
# ifdef FEAT_WAYLAND_CLIPBOARD
		જો (clip_wl.regular.available || clip_wl.primary.available)
		    method = CLIPMETHOD_WAYLAND;
# endif
	    }
	}
	નહીંતર જો (STRCMP(buf, "x11") == 0)
	{
# ifdef FEAT_GUI
	    જો (!gui.in_use)
# endif
	    {
# ifdef FEAT_XCLIPBOARD
		// x_IOerror_handler() in os_unix.c should set xterm_dpy to NULL if
		// we lost connection to the X server.
		જો (xterm_dpy != ખાલી)
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
	નહીંતર
	{
# ifdef FEAT_CLIPBOARD_PROVIDER
	    // Check if name matches a clipboard provider
	    પૂર્ણાંક r = clip_provider_is_available(buf);

	    જો (r == 1)
	    {
		method = CLIPMETHOD_PROVIDER;
		જો (ret == CLIPMETHOD_FAIL)
		{
		    vim_free(clip_provider);
		    clip_provider = vim_strsave(buf);
		    જો (clip_provider == ખાલી)
			જાઓ fail;
		}
	    }
	    નહીંતર જો (r == -1)
# endif
	    {
# ifdef FEAT_CLIPBOARD_PROVIDER
fail:
# endif
		ret = CLIPMETHOD_FAIL;
		જાઓ exit;
	    }
	}

	// Keep on going in order to catch errors
	જો (method != CLIPMETHOD_NONE && ret == CLIPMETHOD_FAIL)
	    ret = method;
    }

    // No match found, use "none".
    ret = (ret == CLIPMETHOD_FAIL) ? CLIPMETHOD_NONE : ret;

exit:
    vim_free(buf);
    પાછા ret;
}


/*
 * Returns name of clipmethod in a statically allocated string.
 */
    સ્થિર char_u *
clipmethod_to_str(clipmethod_T method)
{
    પસંદગી(method)
    {
	સ્થિતિ CLIPMETHOD_WAYLAND:
	    પાછા (char_u *)"wayland";
	સ્થિતિ CLIPMETHOD_X11:
	    પાછા (char_u *)"x11";
	સ્થિતિ CLIPMETHOD_PROVIDER:
# ifdef FEAT_EVAL
	    પાછા clip_provider;
# endif
	મૂળભૂત:
	    પાછા (char_u *)"none";
    }
}

/*
 * Sets the current clipmethod to use given by `get_clipmethod()`. Returns an
 * error message on failure else NULL.
 */
    અક્ષર *
choose_clipmethod(શૂન્ય)
{
    clipmethod_T method = get_clipmethod(p_cpm);

    જો (method == CLIPMETHOD_FAIL)
	પાછા e_invalid_argument;

// If GUI is running or we are not on a system with Wayland or X11, then always
// return CLIPMETHOD_NONE. System or GUI clipboard handling always overrides.
// This is unless a provider is being used.
# જો defined(FEAT_XCLIPBOARD) || defined(FEAT_WAYLAND_CLIPBOARD)
#  જો defined(FEAT_GUI)
    જો (method != CLIPMETHOD_PROVIDER && gui.in_use)
    {
#   ifdef FEAT_WAYLAND
	// We only interact with Wayland for the clipboard, we can just deinit
	// everything.
	wayland_uninit_connection();
#   endif

	method = CLIPMETHOD_NONE;
	જાઓ lose_sel_exit;
    }
#  endif
# નહીંતર
    // If on a system like windows or macos, then clipmethod is irrelevant, we
    // use their way of accessing the clipboard. This is unless we are using the
    // clipboard provider
#  ifdef FEAT_CLIPBOARD_PROVIDER
    જો (method != CLIPMETHOD_PROVIDER)
#  endif
    {
	method = CLIPMETHOD_NONE;
	જાઓ exit;
    }
# endif

# ifdef FEAT_CLIPBOARD
    // Deinitialize clipboard if there is no way to access clipboard
    જો (method == CLIPMETHOD_NONE)
	clip_init(FALSE);
    // If we have a clipmethod that works now, then initialize clipboard
    નહીંતર જો (clipmethod == CLIPMETHOD_NONE && method != CLIPMETHOD_NONE)
    {
	clip_init(TRUE);
	did_warn_clipboard = false;
    }
    // Disown clipboard if we are switching to a new method
    નહીંતર જો (clipmethod != CLIPMETHOD_NONE && method != clipmethod)
    {
#  જો (defined(FEAT_XCLIPBOARD) || defined(FEAT_WAYLAND_CLIPBOARD)) \
	&& defined(FEAT_GUI)
lose_sel_exit:
#  endif
	જો (clip_star.owned)
	    clip_lose_selection(&clip_star);
	જો (clip_plus.owned)
	    clip_lose_selection(&clip_plus);

#  જો defined(FEAT_GUI)
	જો (!gui.in_use)
#  endif
	{
	    clip_init(TRUE);
	    did_warn_clipboard = false;
	}
    }
# endif // FEAT_CLIPBOARD

# જો !defined(FEAT_XCLIPBOARD) && !defined(FEAT_WAYLAND_CLIPBOARD)
exit:
# endif

    clipmethod = method;

# ifdef FEAT_EVAL
    set_vim_var_string(VV_CLIPMETHOD, (char_u*)clipmethod_to_str(method), -1);
# endif

    પાછા ખાલી;
}

/*
 * Call choose_clipmethod().
 */
    શૂન્ય
ex_clipreset(exarg_T *eap UNUSED)
{
    clipmethod_T prev = clipmethod;

    choose_clipmethod();

    જો (clipmethod == CLIPMETHOD_NONE)
	smsg(_("Could not find a way to access the clipboard."));
    નહીંતર જો (clipmethod != prev)
	smsg(_("Switched to clipboard method '%s'."),
		clipmethod_to_str(clipmethod));
}

/*
 * Adjust the register name pointed to with "rp" for the clipboard being
 * used always and the clipboard being available.
 */
    શૂન્ય
adjust_clip_reg(પૂર્ણાંક *rp)
{
# ifdef FEAT_CLIPBOARD_PROVIDER
    જો (clipmethod == CLIPMETHOD_PROVIDER)
    {
	જો (*rp == 0 && clip_unnamed != 0)
	    *rp = ((clip_unnamed & CLIP_UNNAMED_PLUS)) ? '+' : '*';
	પાછા;
    }
# endif
# ifdef FEAT_CLIPBOARD
    // If no reg. specified, and "unnamed" or "unnamedplus" is in 'clipboard',
    // use '*' or '+' reg, respectively. "unnamedplus" prevails.
    જો (*rp == 0 && (clip_unnamed != 0 || clip_unnamed_saved != 0))
    {
	જો (clip_unnamed != 0)
	    *rp = ((clip_unnamed & CLIP_UNNAMED_PLUS) && clip_plus.available)
								  ? '+' : '*';
	નહીંતર
	    *rp = ((clip_unnamed_saved & CLIP_UNNAMED_PLUS)
					   && clip_plus.available) ? '+' : '*';
    }
    જો ((!clip_star.available && *rp == '*') ||
	   (!clip_plus.available && *rp == '+'))
    {
	સંદેશ_warn_missing_clipboard();
	*rp = 0;
    }
# endif
}

/*
 * Extract the items in the 'clipboard' option and set global values.
 * Return an error message or NULL for success.
 */
    અક્ષર *
did_set_clipboard(optset_T *args UNUSED)
{
    પૂર્ણાંક		new_unnamed = 0;
# ifdef FEAT_CLIPBOARD
    પૂર્ણાંક		new_autoselect_star = FALSE;
    પૂર્ણાંક		new_autoselect_plus = FALSE;
    પૂર્ણાંક		new_autoselectml = FALSE;
    પૂર્ણાંક		new_html = FALSE;
# endif
    regprog_T	*new_exclude_prog = ખાલી;
    અક્ષર	*errmsg = ખાલી;
    char_u	*p;

    માટે (p = p_cb; *p != NUL; )
    {
	// Note: Keep this in sync with p_cb_values.
	જો (STRNCMP(p, "unnamed", 7) == 0 && (p[7] == ',' || p[7] == NUL))
	{
	    new_unnamed |= CLIP_UNNAMED;
	    p += 7;
	}
	નહીંતર જો (STRNCMP(p, "unnamedplus", 11) == 0
					    && (p[11] == ',' || p[11] == NUL))
	{
	    new_unnamed |= CLIP_UNNAMED_PLUS;
	    p += 11;
	}
# ifdef FEAT_CLIPBOARD
	નહીંતર જો (STRNCMP(p, "autoselect", 10) == 0
					    && (p[10] == ',' || p[10] == NUL))
	{
	    new_autoselect_star = TRUE;
	    p += 10;
	}
	નહીંતર જો (STRNCMP(p, "autoselectplus", 14) == 0
					    && (p[14] == ',' || p[14] == NUL))
	{
	    new_autoselect_plus = TRUE;
	    p += 14;
	}
	નહીંતર જો (STRNCMP(p, "autoselectml", 12) == 0
					    && (p[12] == ',' || p[12] == NUL))
	{
	    new_autoselectml = TRUE;
	    p += 12;
	}
	નહીંતર જો (STRNCMP(p, "html", 4) == 0 && (p[4] == ',' || p[4] == NUL))
	{
	    new_html = TRUE;
	    p += 4;
	}
	નહીંતર જો (STRNCMP(p, "exclude:", 8) == 0 && new_exclude_prog == ખાલી)
	{
	    p += 8;
	    new_exclude_prog = vim_regcomp(p, RE_MAGIC);
	    જો (new_exclude_prog == ખાલી)
		errmsg = e_invalid_argument;
	    તોડો;
	}
# endif
	નહીંતર
	{
	    errmsg = e_invalid_argument;
	    તોડો;
	}
	જો (*p == ',')
	    ++p;
    }
    જો (errmsg == ખાલી)
    {
# ifdef FEAT_CLIPBOARD
	જો (global_busy)
	    // clip_unnamed will be reset to clip_unnamed_saved
	    // at end_global_changes
	    clip_unnamed_saved = new_unnamed;
	નહીંતર
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
# જો defined(FEAT_GUI_GTK)
	જો (gui.in_use)
	{
#  ifdef USE_GTK4
	    gui_gtk_update_selection_formats(&clip_plus);
	    gui_gtk_update_selection_formats(&clip_star);
#  નહીંતર
	    gui_gtk_set_selection_targets((GdkAtom)GDK_SELECTION_PRIMARY);
	    gui_gtk_set_selection_targets((GdkAtom)clip_plus.gtk_sel_atom);
#  endif
#  ifdef FEAT_DND
	    gui_gtk_set_dnd_targets();
#  endif
	}
# endif
    }
    નહીંતર
	vim_regfree(new_exclude_prog);

    પાછા errmsg;
}

#endif // HAVE_CLIPMETHOD

#ifdef FEAT_CLIPBOARD_PROVIDER

/*
 * Check if a clipboard provider with given name is available. Returns 1 if available,
 * 0 if not available, and -1 on error
 */
    સ્થિર પૂર્ણાંક
clip_provider_is_available(char_u *provider)
{
    dict_T	*providers = get_vim_var_dict(VV_CLIPPROVIDERS);
    typval_T	provider_tv = {0};
    callback_T	callback = {0};
    typval_T	rettv = {0};
    typval_T	func_tv = {0};
    પૂર્ણાંક		res = 0;

    જો (dict_get_tv(providers, (અક્ષર *)provider, &provider_tv) == FAIL
	    || provider_tv.v_type != VAR_DICT)
	// clipboard provider not defined
	પાછા -1;

    જો (dict_get_tv(provider_tv.vval.v_dict, "available", &func_tv) == FAIL)
    {
	clear_tv(&provider_tv);
	// If "available" function not specified assume always TRUE
	પાછા 1;
    }

    જો ((callback = get_callback(&func_tv)).cb_name == ખાલી)
	જાઓ fail;

    જો (call_callback(&callback, -1, &rettv, 0, ખાલી) == FAIL ||
	    (rettv.v_type != VAR_BOOL && rettv.v_type != VAR_સંખ્યા))
	જાઓ fail;

    જો (rettv.vval.v_સંખ્યા)
	res = 1;

    જો (FALSE)
fail:
	res = -1;

    free_callback(&callback);
    clear_tv(&func_tv);
    clear_tv(&rettv);
    clear_tv(&provider_tv);

    પાછા res;
}

/*
 * Get the specified callback "function" from the provider dictionary for
 * register "reg".
 */
    સ્થિર પૂર્ણાંક
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

    જો (dict_get_tv(providers, (અક્ષર *)provider, &provider_tv) == FAIL)
	પાછા FAIL;
    નહીંતર જો (provider_tv.v_type != VAR_DICT)
    {
	clear_tv(&provider_tv);
	પાછા FAIL;
    }
    નહીંતર જો (dict_get_tv(
		provider_tv.vval.v_dict,
		(અક્ષર *)function,
		&action_tv) == FAIL)
    {
	clear_tv(&provider_tv);
	પાછા FAIL;
    }
    નહીંતર જો (action_tv.v_type != VAR_DICT)
    {
	clear_tv(&provider_tv);
	clear_tv(&action_tv);
	પાછા FAIL;
    }
    નહીંતર જો (dict_get_tv(action_tv.vval.v_dict, (અક્ષર *)reg, &func_tv) == FAIL)
    {
	clear_tv(&provider_tv);
	clear_tv(&action_tv);
	પાછા FAIL;
    }
    નહીંતર જો ((cb = get_callback(&func_tv)).cb_name == ખાલી)
    {
	clear_tv(&provider_tv);
	clear_tv(&action_tv);
	clear_tv(&func_tv);
	પાછા FAIL;
    }
    clear_tv(&provider_tv);
    clear_tv(&action_tv);

    // func_tv owns the function name, so we must make a copy for the callback
    set_callback(callback, &cb);
    clear_tv(&func_tv);
    પાછા OK;
}

    સ્થિર શૂન્ય
clip_provider_copy(char_u *reg, char_u *provider)
{
    સ્થિર bool	recursive = false;
    callback_T	callback;
    typval_T	rettv;
    typval_T	argvars[4];
    yankreg_T	*y_ptr;
    char_u	type[2 + NUMBUFLEN] = {0};
    list_T	*list = ખાલી;

    જો (recursive)
	પાછા;

    જો (clip_provider_get_callback(
		reg,
		provider,
		(char_u *)"copy",
		&callback) == FAIL)
	પાછા;

    // Convert register type into a string
    જો (*reg == '+')
	y_ptr = get_y_register(REAL_PLUS_REGISTER);
    નહીંતર
	y_ptr = get_y_register(STAR_REGISTER);

    પસંદગી (y_ptr->y_type)
    {
	સ્થિતિ MCHAR:
	    type[0] = 'v';
	    તોડો;
	સ્થિતિ MLINE:
	    type[0] = 'V';
	    તોડો;
	સ્થિતિ MBLOCK:
	    sprintf((અક્ષર *)type, "%c%d", Ctrl_V, y_ptr->y_width + 1);
	    તોડો;
	મૂળભૂત:
	    type[0] = 0;
	    તોડો;
    }

    argvars[0].v_type = VAR_STRING;
    argvars[0].vval.v_string = reg;

    argvars[1].v_type = VAR_STRING;
    argvars[1].vval.v_string = type;

    // Get register contents by creating a list of lines
    list = list_alloc();

    જો (list == ખાલી)
    {
	free_callback(&callback);
	પાછા;
    }

    માટે (પૂર્ણાંક i = 0; i < y_ptr->y_size; i++)
	જો (list_append_string(list, y_ptr->y_array[i].string,
	    (પૂર્ણાંક)y_ptr->y_array[i].length) == FAIL)
	{
	    free_callback(&callback);
	    list_unref(list);
	    પાછા;
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

    સ્થિર શૂન્ય
clip_provider_paste(char_u *reg, char_u *provider)
{
    સ્થિર bool	recursive = false;
    callback_T	callback;
    typval_T	argvars[2];
    typval_T	rettv;
    પૂર્ણાંક		ret;
    char_u	*reg_type;
    list_T	*લીટીઓ;

    જો (recursive)
	પાછા;

    જો (clip_provider_get_callback(
		reg,
		provider,
		(char_u *)"paste",
		&callback) == FAIL)
	પાછા;

    argvars[0].v_type = VAR_STRING;
    argvars[0].vval.v_string = reg;

    argvars[1].v_type = VAR_UNKNOWN;

    textlock++;
    recursive = true;
    ret = call_callback(&callback, -1, &rettv, 1, argvars);
    recursive = false;
    textlock--;

    જો (ret == FAIL)
	જાઓ exit;
    નહીંતર જો (rettv.v_type == VAR_TUPLE
	    && TUPLE_LEN(rettv.vval.v_tuple) == 2
	    && TUPLE_ITEM(rettv.vval.v_tuple, 0)->v_type == VAR_STRING
	    && TUPLE_ITEM(rettv.vval.v_tuple, 1)->v_type == VAR_LIST)
    {
	reg_type = TUPLE_ITEM(rettv.vval.v_tuple, 0)->vval.v_string;
	લીટીઓ = TUPLE_ITEM(rettv.vval.v_tuple, 1)->vval.v_list;
    }
    નહીંતર જો (rettv.v_type == VAR_LIST
	    && rettv.vval.v_list->lv_len == 2
	    && rettv.vval.v_list->lv_first->li_tv.v_type == VAR_STRING
	    && rettv.vval.v_list->lv_first->li_next->li_tv.v_type == VAR_LIST)
    {
	reg_type = rettv.vval.v_list->lv_first->li_tv.vval.v_string;
	લીટીઓ = rettv.vval.v_list->lv_first->li_next->li_tv.vval.v_list;
    }
    નહીંતર
	જાઓ exit;

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
	પૂર્ણાંક		len;

	// If the list is NULL handle like an empty list.
	len = લીટીઓ == ખાલી ? 0 : લીટીઓ->lv_len;

	// First half: use for pointers to result lines; second half: use for
	// pointers to allocated copies.
	lstval = ALLOC_MULT(char_u *, (len + 1) * 2);
	જો (lstval == ખાલી)
	    જાઓ exit;
	curval = lstval;
	allocval = lstval + len + 2;
	curallocval = allocval;

	જો (લીટીઓ != ખાલી)
	{
	    CHECK_LIST_MATERIALIZE(લીટીઓ);
	    FOR_ALL_LIST_વસ્તુઓ(લીટીઓ, li)
	    {
		strval = tv_get_string_buf_chk(&li->li_tv, buf);
		જો (strval == ખાલી)
		    જાઓ free_lstval;
		જો (strval == buf)
		{
		    // Need to make a copy, next tv_get_string_buf_chk() will
		    // overwrite the string.
		    strval = vim_strsave(buf);
		    જો (strval == ખાલી)
			જાઓ free_lstval;
		    *curallocval++ = strval;
		}
		*curval++ = strval;
	    }
	}
	*curval++ = ખાલી;

	જો (*reg_type != NUL && (STRLEN(reg_type) == 0
		|| get_yank_type(&reg_type, &yank_type, &block_len) == FAIL))
	{
	    emsg(e_invalid_argument);
	    જાઓ free_lstval;
	}

	જો (*reg == '+')
	    y_ptr = get_y_register(REAL_PLUS_REGISTER);
	નહીંતર
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
	જ્યારે (curallocval > allocval)
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
સ્થિર પૂર્ણાંક star_pause_ગણતરી = -2, plus_pause_ગણતરી = -2;

    શૂન્ય
call_clip_provider_request(પૂર્ણાંક reg)
{
    જો (clipmethod != CLIPMETHOD_PROVIDER)
	પાછા;

    જો (reg == '+' && plus_pause_ગણતરી < 0)
    {
	જો (plus_pause_ગણતરી == -1)
	    plus_pause_ગણતરી = 1;
	clip_provider_paste((char_u *)"+", clip_provider);
    }
    નહીંતર જો (reg == '*' && star_pause_ગણતરી < 0)
    {
	જો (star_pause_ગણતરી == -1)
	    star_pause_ગણતરી = 1;
	clip_provider_paste((char_u *)"*", clip_provider);
    }
    નહીંતર
	પાછા;
}

    શૂન્ય
call_clip_provider_set(પૂર્ણાંક reg)
{
    જો (clipmethod != CLIPMETHOD_PROVIDER)
	પાછા;

    જો (reg == '+' && plus_pause_ગણતરી < 0)
    {
	જો (plus_pause_ગણતરી == -1)
	    plus_pause_ગણતરી = 1;
	clip_provider_copy((char_u *)"+", clip_provider);
    }
    નહીંતર જો (reg == '*' && star_pause_ગણતરી < 0)
    {
	જો (star_pause_ગણતરી == -1)
	    star_pause_ગણતરી = 1;
	clip_provider_copy((char_u *)"*", clip_provider);
    }
}

/*
 * Makes it so that the next provider call is only done once any calls after are
 * ignored, until dec_clip_provider is called the same number of times after
 * again. Note that this is per clipboard register ("+", "*")
 */
    શૂન્ય
inc_clip_provider(શૂન્ય)
{
    plus_pause_ગણતરી = (plus_pause_ગણતરી == -2
	|| plus_pause_ગણતરી == -1) ? -1 : plus_pause_ગણતરી + 1;
    star_pause_ગણતરી = (star_pause_ગણતરી == -2
	|| star_pause_ગણતરી == -1) ? -1 : star_pause_ગણતરી + 1;
}

    શૂન્ય
dec_clip_provider(શૂન્ય)
{
    જો (plus_pause_ગણતરી != -2)
	plus_pause_ગણતરી = plus_pause_ગણતરી == -1 ? -1 : plus_pause_ગણતરી - 1;
    જો (star_pause_ગણતરી != -2)
	star_pause_ગણતરી = star_pause_ગણતરી == -1 ? -1 : star_pause_ગણતરી - 1;

    જો (plus_pause_ગણતરી == 0 || plus_pause_ગણતરી == -1)
	plus_pause_ગણતરી = -2;
    જો (star_pause_ગણતરી == 0 || star_pause_ગણતરી == -1)
	star_pause_ગણતરી = -2;
}

#endif // FEAT_CLIPBOARD_PROVIDER

#જો defined(FEAT_WAYLAND_CLIPBOARD) || (defined(FEAT_GUI_GTK) && defined(USE_GTK4))
/*
 * If "vim" is TRUE, then get the motion type. If "vimenc" is TRUE, then get the
 * motion type and also convert "*buf". "buf" and "len_store" will be updated to
 * reflect the actual contents, but should be set beforehand with the initial
 * contents. Note that if "*tofree" is not NULL, use vim_free() on "*buf",
 * otherwise use the free func for which "*buf" was allocated with. In both
 * cases use vim_free() on "*tofree". Returns OK on success and FAIL on failure.
 */
    પૂર્ણાંક
clip_convert_data(
	char_u	**buf,
	long	*len_store,
	પૂર્ણાંક	*motion,
	bool	vim,
	bool	vimenc,
	char_u	**tofree)
{
    char_u  *final = *buf;
    char_u  *enc;
    long    len = *len_store;

    જો (vim && len >= 2)
    {
	*motion = *final++;
	len--;
    }
    નહીંતર જો (vimenc && len >= 3)
    {
	vimconv_T   conv;
	પૂર્ણાંક	    convlen;

	// First byte is motion type
	*motion = *final++;
	len--;

	// Get encoding of selection
	enc = final;

	// Skip the encoding type including null terminator in final text
	final = memchr(final, NUL, len);
	જો (final == ખાલી)
	    પાછા FAIL;
	final++; // Skip NUL

	// Subtract pointers to get length of encoding;
	len -= final - enc;

	conv.vc_type = CONV_NONE;
	convert_setup(&conv, enc, p_enc);
	જો (conv.vc_type != CONV_NONE)
	{
	   char_u *tmp;

	   convlen = len;
	   tmp = string_convert(&conv, final, &convlen);
	   len = convlen;
	   જો (tmp != ખાલી)
	   {
		final = tmp;
		*tofree = final;
	   }
	   convert_setup(&conv, ખાલી, ખાલી);
	}
    }
    *buf = final;
    *len_store = len;
    પાછા OK;
}
#endif
