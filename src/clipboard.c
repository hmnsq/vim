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

#अन्तर्भाव "vim.h"

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
स्थिर पूर्णाङ्क clip_provider_is_available(char_u *provider);
#endif

#जइ defined(FEAT_CLIPBOARD)

# जइ defined(FEAT_WAYLAND_CLIPBOARD)

#  include "wayland.h"

// Represents either the regular or primary selection
प्रकारनिर्देश संरचना {
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
प्रकारनिर्देश संरचना {
    vwl_seat_T *seat;

    clip_wl_selection_T regular;
    clip_wl_selection_T primary;
} clip_wl_T;

// Mime types we support sending and receiving
// Mimes with a lower index in the array are prioritized first when we are
// receiving data.
स्थिर ध्रुव वर्ण *supported_mimes[] = {
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

स्थिर शून्य
clip_wl_receive_data(Clipboard_T *cbd, ध्रुव वर्ण *mime_type, पूर्णाङ्क fd);
स्थिर शून्य clip_wl_request_selection(Clipboard_T *cbd);
स्थिर पूर्णाङ्क clip_wl_own_selection(Clipboard_T *cbd);
स्थिर शून्य clip_wl_lose_selection(Clipboard_T *cbd);
स्थिर शून्य clip_wl_set_selection(Clipboard_T *cbd);

#  जइ defined(USE_SYSTEM)
स्थिर bool clip_wl_owner_exists(Clipboard_T *cbd);
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
    शून्य
clip_init(पूर्णाङ्क can_use)
{
    Clipboard_T *cb;

    cb = &clip_star;
    प्रति (;;)
    {
	// No need to init again if cbd is already available
	जइ (can_use && cb->available)
	    गच्छ skip;

	cb->available  = can_use;
	cb->owned      = FALSE;
	cb->start.lnum = 0;
	cb->start.col  = 0;
	cb->end.lnum   = 0;
	cb->end.col    = 0;
	cb->state      = SELECT_CLEARED;

skip:
	जइ (cb == &clip_plus)
	    भङ्ग;
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
    शून्य
clip_update_selection(Clipboard_T *clip)
{
    pos_T	    start, end;

    // If visual mode is only due to a redo command ("."), then ignore it
    जइ (!redo_VIsual_busy && VIsual_active && (State & MODE_NORMAL))
    {
	जइ (LT_POS(VIsual, curwin->w_cursor))
	{
	    start = VIsual;
	    end = curwin->w_cursor;
	    जइ (has_mbyte)
		end.col += (*mb_ptr2len)(ml_get_cursor()) - 1;
	}
	अण्णहा
	{
	    start = curwin->w_cursor;
	    end = VIsual;
	}
	जइ (!EQUAL_POS(clip->start, start)
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

    स्थिर पूर्णाङ्क
clip_gen_own_selection(Clipboard_T *cbd)
{
# जइ defined(FEAT_XCLIPBOARD) || defined(FEAT_WAYLAND_CLIPBOARD)
#  ifdef FEAT_GUI
    जइ (gui.in_use)
	प्रत्यागम clip_mch_own_selection(cbd);
    अण्णहा
#  endif
    {
	जइ (clipmethod == CLIPMETHOD_WAYLAND)
	{
#  ifdef FEAT_WAYLAND_CLIPBOARD
	    प्रत्यागम clip_wl_own_selection(cbd);
#  endif
	}
	अण्णहा जइ (clipmethod == CLIPMETHOD_X11)
	{
#  ifdef FEAT_XCLIPBOARD
	    प्रत्यागम clip_xterm_own_selection(cbd);
#  endif
	}
    }
    प्रत्यागम FAIL;
# अण्णहा
    प्रत्यागम clip_mch_own_selection(cbd);
# endif
}

    शून्य
clip_own_selection(Clipboard_T *cbd)
{
    /*
     * Also want to check somehow that we are reading from the keyboard rather
     * than a mapping etc.
     */
# जइ defined(FEAT_X11) || defined(FEAT_WAYLAND_CLIPBOARD)
    // Always own the selection, we might have lost it without being
    // notified, e.g. during a ":sh" command.
    जइ (cbd->available)
    {
	पूर्णाङ्क was_owned = cbd->owned;

	cbd->owned = (clip_gen_own_selection(cbd) == OK);
	जइ (!was_owned && (cbd == &clip_star || cbd == &clip_plus))
	{
	    // May have to show a different kind of highlighting for the
	    // selected area.  There is no specific redraw command for this,
	    // just redraw all windows on the current buffer.
	    जइ (cbd->owned
		    && (get_real_state() == MODE_VISUAL
					    || get_real_state() == MODE_SELECT)
		    && (cbd == &clip_star ? clip_isautosel_star()
						      : clip_isautosel_plus())
		    && HL_ATTR(HLF_V) != HL_ATTR(HLF_VNC))
		redraw_curbuf_later(UPD_INVERTED_ALL);
	}
    }
# अण्णहा
    // Only own the clipboard when we didn't own it yet.
    जइ (!cbd->owned && cbd->available)
	cbd->owned = (clip_gen_own_selection(cbd) == OK);
# endif
}

    स्थिर शून्य
clip_gen_lose_selection(Clipboard_T *cbd)
{
# जइ defined(FEAT_XCLIPBOARD) || defined(FEAT_WAYLAND_CLIPBOARD)
#  ifdef FEAT_GUI
    जइ (gui.in_use)
	clip_mch_lose_selection(cbd);
    अण्णहा
#  endif
    {
	जइ (clipmethod == CLIPMETHOD_WAYLAND)
	{
#  ifdef FEAT_WAYLAND_CLIPBOARD
	    clip_wl_lose_selection(cbd);
#  endif
	}
	अण्णहा जइ (clipmethod == CLIPMETHOD_X11)
	{
#  ifdef FEAT_XCLIPBOARD
	    clip_xterm_lose_selection(cbd);
#  endif
	}
    }
# अण्णहा
    clip_mch_lose_selection(cbd);
# endif
}

    शून्य
clip_lose_selection(Clipboard_T *cbd)
{
# ifdef FEAT_X11
    पूर्णाङ्क	    was_owned = cbd->owned;
# endif
    पूर्णाङ्क     visual_selection = FALSE;

    जइ (cbd == &clip_star || cbd == &clip_plus)
	visual_selection = TRUE;

    clip_free_selection(cbd);
    cbd->owned = FALSE;
    जइ (visual_selection)
	clip_clear_selection(cbd);
    clip_gen_lose_selection(cbd);
# ifdef FEAT_X11
    जइ (visual_selection)
    {
	// May have to show a different kind of highlighting for the selected
	// area.  There is no specific redraw command for this, just redraw all
	// windows on the current buffer.
	जइ (was_owned
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

    स्थिर शून्य
clip_copy_selection(Clipboard_T *clip)
{
    जइ (VIsual_active && (State & MODE_NORMAL) && clip->available)
    {
	clip_update_selection(clip);
	clip_free_selection(clip);
	clip_own_selection(clip);
	जइ (clip->owned)
	    clip_get_selection(clip);
	clip_gen_set_selection(clip);
    }
}

/*
 * Save and restore clip_unnamed before doing possibly many changes. This
 * prevents accessing the clipboard very often which might slow down Vim
 * considerably.
 */
स्थिर पूर्णाङ्क global_change_गणनम् = 0; // if set, inside a start_global_changes
स्थिर पूर्णाङ्क clipboard_needs_update = FALSE; // clipboard needs to be updated
स्थिर पूर्णाङ्क clip_did_set_selection = TRUE;

/*
 * Save clip_unnamed and reset it.
 */
    शून्य
start_global_changes(शून्य)
{
    जइ (++global_change_गणनम् > 1)
	प्रत्यागम;
    clip_unnamed_saved = clip_unnamed;
    clipboard_needs_update = FALSE;

    जइ (clip_did_set_selection)
    {
	clip_unnamed = 0;
	clip_did_set_selection = FALSE;
    }
}

/*
 * Return TRUE if setting the clipboard was postponed, it already contains the
 * right text.
 */
    स्थिर पूर्णाङ्क
is_clipboard_needs_update(शून्य)
{
    प्रत्यागम clipboard_needs_update;
}

/*
 * Restore clip_unnamed and set the selection when needed.
 */
    शून्य
end_global_changes(शून्य)
{
    जइ (--global_change_गणनम् > 0)
	// recursive
	प्रत्यागम;
    जइ (!clip_did_set_selection)
    {
	clip_did_set_selection = TRUE;
	clip_unnamed = clip_unnamed_saved;
	clip_unnamed_saved = 0;
	जइ (clipboard_needs_update)
	{
	    // only store something in the clipboard,
	    // if we have yanked anything to it
	    जइ (clip_unnamed & CLIP_UNNAMED)
	    {
		clip_own_selection(&clip_star);
		clip_gen_set_selection(&clip_star);
	    }
	    जइ (clip_unnamed & CLIP_UNNAMED_PLUS)
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
    शून्य
clip_auto_select(शून्य)
{
    जइ (clip_isautosel_star())
	clip_copy_selection(&clip_star);
    जइ (clip_isautosel_plus())
	clip_copy_selection(&clip_plus);
}

/*
 * Return TRUE if automatic selection of Visual area is desired for the *
 * register.
 */
    पूर्णाङ्क
clip_isautosel_star(शून्य)
{
# ifdef FEAT_CLIPBOARD_PROVIDER
    जइ (clipmethod == CLIPMETHOD_PROVIDER)
	प्रत्यागम false;
# endif
# ifdef FEAT_GUI
    जइ (gui.in_use)
	प्रत्यागम vim_strchr(p_go, GO_ASEL) != रिक्त
	    && vim_strchr(p_go, GO_ASELPLUS) == रिक्त;
# endif
    प्रत्यागम clip_autoselect_star;
}

/*
 * Return TRUE if automatic selection of Visual area is desired for the +
 * register.
 */
    पूर्णाङ्क
clip_isautosel_plus(शून्य)
{
# ifdef FEAT_CLIPBOARD_PROVIDER
    जइ (clipmethod == CLIPMETHOD_PROVIDER)
	प्रत्यागम false;
# endif
# ifdef FEAT_GUI
    जइ (gui.in_use)
	प्रत्यागम vim_strchr(p_go, GO_ASELPLUS) != रिक्त;
# endif
    प्रत्यागम clip_autoselect_plus;
}


/*
 * Stuff for general mouse selection, without using Visual mode.
 */

/*
 * Compare two screen positions ala strcmp()
 */
    स्थिर पूर्णाङ्क
clip_compare_pos(
    पूर्णाङ्क		row1,
    पूर्णाङ्क		col1,
    पूर्णाङ्क		row2,
    पूर्णाङ्क		col2)
{
    जइ (row1 > row2) प्रत्यागम(1);
    जइ (row1 < row2) प्रत्यागम(-1);
    जइ (col1 > col2) प्रत्यागम(1);
    जइ (col1 < col2) प्रत्यागम(-1);
    प्रत्यागम(0);
}

// "how" flags for clip_invert_area()
# define CLIP_CLEAR	1
# define CLIP_SET	2
# define CLIP_TOGGLE	3

/*
 * Invert or un-invert a rectangle of the screen.
 * "invert" is true if the result is inverted.
 */
    स्थिर शून्य
clip_invert_rectangle(
	Clipboard_T	*cbd UNUSED,
	पूर्णाङ्क		row_arg,
	पूर्णाङ्क		col_arg,
	पूर्णाङ्क		height_arg,
	पूर्णाङ्क		width_arg,
	पूर्णाङ्क		invert)
{
    पूर्णाङ्क		row = row_arg;
    पूर्णाङ्क		col = col_arg;
    पूर्णाङ्क		height = height_arg;
    पूर्णाङ्क		width = width_arg;

# ifdef FEAT_PROP_POPUP
    // this goes on top of all popup windows
    screen_zindex = CLIP_ZINDEX;

    जइ (col < cbd->min_col)
    {
	width -= cbd->min_col - col;
	col = cbd->min_col;
    }
    जइ (width > cbd->max_col - col)
	width = cbd->max_col - col;
    जइ (row < cbd->min_row)
    {
	height -= cbd->min_row - row;
	row = cbd->min_row;
    }
    जइ (height > cbd->max_row - row + 1)
	height = cbd->max_row - row + 1;
# endif
# ifdef FEAT_GUI
    जइ (gui.in_use)
	gui_mch_invert_rectangle(row, col, height, width);
    अण्णहा
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
    स्थिर शून्य
clip_invert_area(
	Clipboard_T	*cbd,
	पूर्णाङ्क		row1,
	पूर्णाङ्क		col1,
	पूर्णाङ्क		row2,
	पूर्णाङ्क		col2,
	पूर्णाङ्क		how)
{
    पूर्णाङ्क		invert = FALSE;
    पूर्णाङ्क		max_col;

# ifdef FEAT_PROP_POPUP
    max_col = cbd->max_col - 1;
# अण्णहा
    max_col = Columns - 1;
# endif

    जइ (how == CLIP_SET)
	invert = TRUE;

    // Swap the from and to positions so the from is always before
    जइ (clip_compare_pos(row1, col1, row2, col2) > 0)
    {
	पूर्णाङ्क tmp_row, tmp_col;

	tmp_row = row1;
	tmp_col = col1;
	row1	= row2;
	col1	= col2;
	row2	= tmp_row;
	col2	= tmp_col;
    }
    अण्णहा जइ (how == CLIP_TOGGLE)
	invert = TRUE;

    // If all on the same line, do it the easy way
    जइ (row1 == row2)
    {
	clip_invert_rectangle(cbd, row1, col1, 1, col2 - col1, invert);
    }
    अण्णहा
    {
	// Handle a piece of the first line
	जइ (col1 > 0)
	{
	    clip_invert_rectangle(cbd, row1, col1, 1,
						  (पूर्णाङ्क)Columns - col1, invert);
	    row1++;
	}

	// Handle a piece of the last line
	जइ (col2 < max_col)
	{
	    clip_invert_rectangle(cbd, row2, 0, 1, col2, invert);
	    row2--;
	}

	// Handle the rectangle that's left
	जइ (row2 >= row1)
	    clip_invert_rectangle(cbd, row1, 0, row2 - row1 + 1,
							 (पूर्णाङ्क)Columns, invert);
    }
}

/*
 * Start, continue or end a modeless selection.  Used when editing the
 * command-line, in the cmdline window and when the mouse is in a popup window.
 */
    शून्य
clip_modeless(पूर्णाङ्क button, पूर्णाङ्क is_click, पूर्णाङ्क is_drag)
{
    पूर्णाङ्क		repeat;

    repeat = ((clip_star.mode == SELECT_MODE_CHAR
		|| clip_star.mode == SELECT_MODE_पङ्क्तिः)
					      && (mod_mask & MOD_MASK_2CLICK))
	    || (clip_star.mode == SELECT_MODE_WORD
					     && (mod_mask & MOD_MASK_3CLICK));
    जइ (is_click && button == MOUSE_दक्षिणः)
    {
	// Right mouse button: If there was no selection, start one.
	// Otherwise extend the existing selection.
	जइ (clip_star.state == SELECT_CLEARED)
	    clip_start_selection(mouse_col, mouse_row, FALSE);
	clip_process_selection(button, mouse_col, mouse_row, repeat);
    }
    अण्णहा जइ (is_click)
	clip_start_selection(mouse_col, mouse_row, repeat);
    अण्णहा जइ (is_drag)
    {
	// Don't try extending a selection if there isn't one.  Happens when
	// button-down is in the cmdline and them moving mouse upwards.
	जइ (clip_star.state != SELECT_CLEARED)
	    clip_process_selection(button, mouse_col, mouse_row, repeat);
    }
    अण्णहा // release
	clip_process_selection(MOUSE_RELEASE, mouse_col, mouse_row, FALSE);
}

/*
 * Update the currently selected region by adding and/or subtracting from the
 * beginning or end and inverting the changed area(s).
 */
    स्थिर शून्य
clip_update_modeless_selection(
    Clipboard_T    *cb,
    पूर्णाङ्क		    row1,
    पूर्णाङ्क		    col1,
    पूर्णाङ्क		    row2,
    पूर्णाङ्क		    col2)
{
    // See if we changed at the beginning of the selection
    जइ (row1 != cb->start.lnum || col1 != (पूर्णाङ्क)cb->start.col)
    {
	clip_invert_area(cb, row1, col1, (पूर्णाङ्क)cb->start.lnum, cb->start.col,
								 CLIP_TOGGLE);
	cb->start.lnum = row1;
	cb->start.col  = col1;
    }

    // See if we changed at the end of the selection
    जइ (row2 != cb->end.lnum || col2 != (पूर्णाङ्क)cb->end.col)
    {
	clip_invert_area(cb, (पूर्णाङ्क)cb->end.lnum, cb->end.col, row2, col2,
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

    स्थिर शून्य
clip_get_word_boundaries(Clipboard_T *cb, पूर्णाङ्क row, पूर्णाङ्क col)
{
    पूर्णाङ्क		start_class;
    पूर्णाङ्क		temp_col;
    char_u	*p;
    पूर्णाङ्क		mboff;

    जइ (row >= screen_Rows || col >= screen_Columns || Screen_पङ्क्तयः == रिक्त)
	प्रत्यागम;

    p = Screen_पङ्क्तयः + पङ्क्तिः_Offset[row];
    // Correct for starting in the right half of a double-wide char
    जइ (enc_dbcs != 0)
	col -= dbcs_screen_head_off(p, p + col);
    अण्णहा जइ (enc_utf8 && p[col] == 0)
	--col;
    start_class = CHAR_CLASS(p[col]);

    temp_col = col;
    प्रति ( ; temp_col > 0; temp_col--)
	जइ (enc_dbcs != 0
		   && (mboff = dbcs_screen_head_off(p, p + temp_col - 1)) > 0)
	    temp_col -= mboff;
	अण्णहा जइ (CHAR_CLASS(p[temp_col - 1]) != start_class
		&& !(enc_utf8 && p[temp_col - 1] == 0))
	    भङ्ग;
    cb->word_start_col = temp_col;

    temp_col = col;
    प्रति ( ; temp_col < screen_Columns; temp_col++)
	जइ (enc_dbcs != 0 && dbcs_ptr2cells(p + temp_col) == 2)
	    ++temp_col;
	अण्णहा जइ (CHAR_CLASS(p[temp_col]) != start_class
		&& !(enc_utf8 && p[temp_col] == 0))
	    भङ्ग;
    cb->word_end_col = temp_col;
}

/*
 * Find the column position for the last non-whitespace character on the given
 * line at or before start_col.
 */
    स्थिर पूर्णाङ्क
clip_get_पङ्क्तिः_end(Clipboard_T *cbd UNUSED, पूर्णाङ्क row)
{
    पूर्णाङ्क	    i;

    जइ (row >= screen_Rows || Screen_पङ्क्तयः == रिक्त)
	प्रत्यागम 0;
    प्रति (i =
# ifdef FEAT_PROP_POPUP
	    cbd->max_col;
# अण्णहा
	    screen_Columns;
# endif
			    i > 0; i--)
	जइ (Screen_पङ्क्तयः[पङ्क्तिः_Offset[row] + i - 1] != ' ')
	    भङ्ग;
    प्रत्यागम i;
}

/*
 * Start the selection
 */
    शून्य
clip_start_selection(पूर्णाङ्क col, पूर्णाङ्क row, पूर्णाङ्क repeated_click)
{
    Clipboard_T	*cb = &clip_star;
# ifdef FEAT_PROP_POPUP
    win_T	*wp;
    पूर्णाङ्क		row_cp = row;
    पूर्णाङ्क		col_cp = col;

    wp = mouse_find_win(&row_cp, &col_cp, FIND_POPUP);
    जइ (wp != रिक्त && WIN_IS_POPUP(wp)
				  && popup_is_in_scrollbar(wp, row_cp, col_cp))
	// click or double click in scrollbar does not start a selection
	प्रत्यागम;
# endif

    जइ (cb->state == SELECT_DONE)
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
    जइ (wp != रिक्त && WIN_IS_POPUP(wp))
    {
	// Click in a popup window restricts selection to that window,
	// excluding the border.
	cb->min_col = wp->w_wincol + wp->w_popup_border[3];
	cb->max_col = wp->w_wincol + popup_width(wp)
				 - wp->w_popup_border[1] - wp->w_has_scrollbar;
	जइ (cb->max_col > screen_Columns)
	    cb->max_col = screen_Columns;
	cb->min_row = wp->w_winrow + wp->w_popup_border[0];
	cb->max_row = wp->w_winrow + popup_height(wp) - 1
						   - wp->w_popup_border[2];
    }
    अण्णहा
    {
	cb->min_col = 0;
	cb->max_col = screen_Columns;
	cb->min_row = 0;
	cb->max_row = screen_Rows;
    }
# endif

    जइ (repeated_click)
    {
	जइ (++cb->mode > SELECT_MODE_पङ्क्तिः)
	    cb->mode = SELECT_MODE_CHAR;
    }
    अण्णहा
	cb->mode = SELECT_MODE_CHAR;

# ifdef FEAT_GUI
    // clear the cursor until the selection is made
    जइ (gui.in_use)
	gui_undraw_cursor();
# endif

    विकल्प (cb->mode)
    {
	पक्ष SELECT_MODE_CHAR:
	    cb->origin_start_col = cb->start.col;
	    cb->word_end_col = clip_get_पङ्क्तिः_end(cb, (पूर्णाङ्क)cb->start.lnum);
	    भङ्ग;

	पक्ष SELECT_MODE_WORD:
	    clip_get_word_boundaries(cb, (पूर्णाङ्क)cb->start.lnum, cb->start.col);
	    cb->origin_start_col = cb->word_start_col;
	    cb->origin_end_col	 = cb->word_end_col;

	    clip_invert_area(cb, (पूर्णाङ्क)cb->start.lnum, cb->word_start_col,
			    (पूर्णाङ्क)cb->end.lnum, cb->word_end_col, CLIP_SET);
	    cb->start.col = cb->word_start_col;
	    cb->end.col   = cb->word_end_col;
	    भङ्ग;

	पक्ष SELECT_MODE_पङ्क्तिः:
	    clip_invert_area(cb, (पूर्णाङ्क)cb->start.lnum, 0, (पूर्णाङ्क)cb->start.lnum,
			    (पूर्णाङ्क)Columns, CLIP_SET);
	    cb->start.col = 0;
	    cb->end.col   = Columns;
	    भङ्ग;
    }

    cb->prev = cb->start;

# ifdef DEBUG_SELECTION
    printf("Selection started at (%ld,%d)\n", cb->start.lnum, cb->start.col);
# endif
}

/*
 * Continue processing the selection
 */
    शून्य
clip_process_selection(
    पूर्णाङ्क		button,
    पूर्णाङ्क		col,
    पूर्णाङ्क		row,
    int_u	repeated_click)
{
    Clipboard_T	*cb = &clip_star;
    पूर्णाङ्क		diff;
    पूर्णाङ्क		slen = 1;	// cursor shape width

    जइ (button == MOUSE_RELEASE)
    {
	जइ (cb->state != SELECT_IN_PROGRESS)
	    प्रत्यागम;

	// Check to make sure we have something selected
	जइ (cb->start.lnum == cb->end.lnum && cb->start.col == cb->end.col)
	{
# ifdef FEAT_GUI
	    जइ (gui.in_use)
		gui_update_cursor(FALSE, FALSE);
# endif
	    cb->state = SELECT_CLEARED;
	    प्रत्यागम;
	}

# ifdef DEBUG_SELECTION
	printf("Selection ended: (%ld,%d) to (%ld,%d)\n", cb->start.lnum,
		cb->start.col, cb->end.lnum, cb->end.col);
# endif
	जइ (clip_isautosel_star() || clip_isautosel_plus()
		|| (
# ifdef FEAT_GUI
		    gui.in_use ? (vim_strchr(p_go, GO_ASELML) != रिक्त) :
# endif
		    clip_autoselectml))
	    clip_copy_modeless_selection(FALSE);
# ifdef FEAT_GUI
	जइ (gui.in_use)
	    gui_update_cursor(FALSE, FALSE);
# endif

	cb->state = SELECT_DONE;
	प्रत्यागम;
    }

    row = check_row(row);
    col = check_col(col);
    col = mb_fix_col(col, row);

    जइ (col == (पूर्णाङ्क)cb->prev.col && row == cb->prev.lnum && !repeated_click)
	प्रत्यागम;

    /*
     * When extending the selection with the right mouse button, swap the
     * start and end if the position is before half the selection
     */
    जइ (cb->state == SELECT_DONE && button == MOUSE_दक्षिणः)
    {
	/*
	 * If the click is before the start, or the click is inside the
	 * selection and the start is the closest side, set the origin to the
	 * end of the selection.
	 */
	जइ (clip_compare_pos(row, col, (पूर्णाङ्क)cb->start.lnum, cb->start.col) < 0
		|| (clip_compare_pos(row, col,
					   (पूर्णाङ्क)cb->end.lnum, cb->end.col) < 0
		    && (((cb->start.lnum == cb->end.lnum
			    && cb->end.col - col > col - cb->start.col))
			|| ((diff = (cb->end.lnum - row) -
						   (row - cb->start.lnum)) > 0
			    || (diff == 0 && col < (पूर्णाङ्क)(cb->start.col +
							 cb->end.col) / 2)))))
	{
	    cb->origin_row = (short_u)cb->end.lnum;
	    cb->origin_start_col = cb->end.col - 1;
	    cb->origin_end_col = cb->end.col;
	}
	अण्णहा
	{
	    cb->origin_row = (short_u)cb->start.lnum;
	    cb->origin_start_col = cb->start.col;
	    cb->origin_end_col = cb->start.col;
	}
	जइ (cb->mode == SELECT_MODE_WORD && !repeated_click)
	    cb->mode = SELECT_MODE_CHAR;
    }

    // set state, for when using the right mouse button
    cb->state = SELECT_IN_PROGRESS;

# ifdef DEBUG_SELECTION
    printf("Selection extending to (%d,%d)\n", row, col);
# endif

    जइ (repeated_click && ++cb->mode > SELECT_MODE_पङ्क्तिः)
	cb->mode = SELECT_MODE_CHAR;

    विकल्प (cb->mode)
    {
	पक्ष SELECT_MODE_CHAR:
	    // If we're on a different line, find where the line ends
	    जइ (row != cb->prev.lnum)
		cb->word_end_col = clip_get_पङ्क्तिः_end(cb, row);

	    // See if we are before or after the origin of the selection
	    जइ (clip_compare_pos(row, col, cb->origin_row,
						   cb->origin_start_col) >= 0)
	    {
		जइ (col >= (पूर्णाङ्क)cb->word_end_col)
		    clip_update_modeless_selection(cb, cb->origin_row,
			    cb->origin_start_col, row, (पूर्णाङ्क)Columns);
		अण्णहा
		{
		    जइ (has_mbyte && mb_lefthalve(row, col))
			slen = 2;
		    clip_update_modeless_selection(cb, cb->origin_row,
			    cb->origin_start_col, row, col + slen);
		}
	    }
	    अण्णहा
	    {
		जइ (has_mbyte
			&& mb_lefthalve(cb->origin_row, cb->origin_start_col))
		    slen = 2;
		जइ (col >= (पूर्णाङ्क)cb->word_end_col)
		    clip_update_modeless_selection(cb, row, cb->word_end_col,
			    cb->origin_row, cb->origin_start_col + slen);
		अण्णहा
		    clip_update_modeless_selection(cb, row, col,
			    cb->origin_row, cb->origin_start_col + slen);
	    }
	    भङ्ग;

	पक्ष SELECT_MODE_WORD:
	    // If we are still within the same word, do nothing
	    जइ (row == cb->prev.lnum && col >= (पूर्णाङ्क)cb->word_start_col
		    && col < (पूर्णाङ्क)cb->word_end_col && !repeated_click)
		प्रत्यागम;

	    // Get new word boundaries
	    clip_get_word_boundaries(cb, row, col);

	    // Handle being after the origin point of selection
	    जइ (clip_compare_pos(row, col, cb->origin_row,
		    cb->origin_start_col) >= 0)
		clip_update_modeless_selection(cb, cb->origin_row,
			cb->origin_start_col, row, cb->word_end_col);
	    अण्णहा
		clip_update_modeless_selection(cb, row, cb->word_start_col,
			cb->origin_row, cb->origin_end_col);
	    भङ्ग;

	पक्ष SELECT_MODE_पङ्क्तिः:
	    जइ (row == cb->prev.lnum && !repeated_click)
		प्रत्यागम;

	    जइ (clip_compare_pos(row, col, cb->origin_row,
		    cb->origin_start_col) >= 0)
		clip_update_modeless_selection(cb, cb->origin_row, 0, row,
			(पूर्णाङ्क)Columns);
	    अण्णहा
		clip_update_modeless_selection(cb, row, 0, cb->origin_row,
			(पूर्णाङ्क)Columns);
	    भङ्ग;
    }

    cb->prev.lnum = row;
    cb->prev.col  = col;

# ifdef DEBUG_SELECTION
	printf("Selection is: (%ld,%d) to (%ld,%d)\n", cb->start.lnum,
		cb->start.col, cb->end.lnum, cb->end.col);
# endif
}

# जइ defined(FEAT_GUI)
/*
 * Redraw part of the selection if character at "row,col" is inside of it.
 * Only used for the GUI.
 */
    शून्य
clip_may_redraw_selection(पूर्णाङ्क row, पूर्णाङ्क col, पूर्णाङ्क len)
{
    पूर्णाङ्क		start = col;
    पूर्णाङ्क		end = col + len;

    जइ (clip_star.state != SELECT_CLEARED
	    && row >= clip_star.start.lnum
	    && row <= clip_star.end.lnum)
    {
	जइ (row == clip_star.start.lnum && start < (पूर्णाङ्क)clip_star.start.col)
	    start = clip_star.start.col;
	जइ (row == clip_star.end.lnum && end > (पूर्णाङ्क)clip_star.end.col)
	    end = clip_star.end.col;
	जइ (end > start)
	    clip_invert_area(&clip_star, row, start, row, end, 0);
    }
}
# endif

/*
 * Called from outside to clear selected region from the display
 */
    शून्य
clip_clear_selection(Clipboard_T *cbd)
{

    जइ (cbd->state == SELECT_CLEARED)
	प्रत्यागम;

    clip_invert_area(cbd, (पूर्णाङ्क)cbd->start.lnum, cbd->start.col,
				 (पूर्णाङ्क)cbd->end.lnum, cbd->end.col, CLIP_CLEAR);
    cbd->state = SELECT_CLEARED;
}

/*
 * Clear the selection if any lines from "row1" to "row2" are inside of it.
 */
    शून्य
clip_may_clear_selection(पूर्णाङ्क row1, पूर्णाङ्क row2)
{
    जइ (clip_star.state == SELECT_DONE
	    && row2 >= clip_star.start.lnum
	    && row1 <= clip_star.end.lnum)
	clip_clear_selection(&clip_star);
}

/*
 * Called before the screen is scrolled up or down.  Adjusts the line numbers
 * of the selection.  Call with big number when clearing the screen.
 */
    शून्य
clip_scroll_selection(
    पूर्णाङ्क	    rows)		// negative for scroll down
{
    पूर्णाङ्क	    lnum;

    जइ (clip_star.state == SELECT_CLEARED)
	प्रत्यागम;

    lnum = clip_star.start.lnum - rows;
    जइ (lnum <= 0)
	clip_star.start.lnum = 0;
    अण्णहा जइ (lnum >= screen_Rows)	// scrolled off of the screen
	clip_star.state = SELECT_CLEARED;
    अण्णहा
	clip_star.start.lnum = lnum;

    lnum = clip_star.end.lnum - rows;
    जइ (lnum < 0)			// scrolled off of the screen
	clip_star.state = SELECT_CLEARED;
    अण्णहा जइ (lnum >= screen_Rows)
	clip_star.end.lnum = screen_Rows - 1;
    अण्णहा
	clip_star.end.lnum = lnum;
}

/*
 * Copy the currently selected area into the '*' or '+' register so it will be
 * available for pasting.
 * When "both" is TRUE also copy to the other register.
 */
    शून्य
clip_copy_modeless_selection(पूर्णाङ्क both UNUSED)
{
    // The info for the modeless selection is stored in '*' register, however if
    // we are using the '+' register for modeless autoselect, we copy to
    // clip_plus instead while using the info in clip_star.
    Clipboard_T *cbd = clip_isautosel_plus() ? &clip_plus : &clip_star;
    char_u	*buffer;
    char_u	*bufp;
    पूर्णाङ्क		row;
    पूर्णाङ्क		start_col;
    पूर्णाङ्क		end_col;
    पूर्णाङ्क		पङ्क्तिः_end_col;
    पूर्णाङ्क		add_newline_flag = FALSE;
    पूर्णाङ्क		len;
    char_u	*p;
    पूर्णाङ्क		row1 = clip_star.start.lnum;
    पूर्णाङ्क		col1 = clip_star.start.col;
    पूर्णाङ्क		row2 = clip_star.end.lnum;
    पूर्णाङ्क		col2 = clip_star.end.col;

    // Can't use ScreenLines unless initialized
    जइ (Screen_पङ्क्तयः == रिक्त)
	प्रत्यागम;

    /*
     * Make sure row1 <= row2, and if row1 == row2 that col1 <= col2.
     */
    जइ (row1 > row2)
    {
	row = row1; row1 = row2; row2 = row;
	row = col1; col1 = col2; col2 = row;
    }
    अण्णहा जइ (row1 == row2 && col1 > col2)
    {
	row = col1; col1 = col2; col2 = row;
    }
# ifdef FEAT_PROP_POPUP
    जइ (col1 < clip_star.min_col)
	col1 = clip_star.min_col;
    जइ (col2 > clip_star.max_col)
	col2 = clip_star.max_col;
    जइ (row1 > clip_star.max_row || row2 < clip_star.min_row)
	प्रत्यागम;
    जइ (row1 < clip_star.min_row)
	row1 = clip_star.min_row;
    जइ (row2 > clip_star.max_row)
	row2 = clip_star.max_row;
# endif
    // correct starting point for being on right half of double-wide char
    p = Screen_पङ्क्तयः + पङ्क्तिः_Offset[row1];
    जइ (enc_dbcs != 0)
	col1 -= (*mb_head_off)(p, p + col1);
    अण्णहा जइ (enc_utf8 && p[col1] == 0)
	--col1;

    // Create a temporary buffer for storing the text
    len = (row2 - row1 + 1) * Columns + 1;
    जइ (enc_dbcs != 0)
	len *= 2;	// max. 2 bytes per display cell
    अण्णहा जइ (enc_utf8)
	len *= MB_MAXBYTES;
    // A newline is added after each row
    len += row2 - row1 + 2;
    buffer = alloc(len);
    जइ (buffer == रिक्त)	    // out of memory
	प्रत्यागम;

    // Process each row in the selection
    प्रति (bufp = buffer, row = row1; row <= row2; row++)
    {
	जइ (row == row1)
	    start_col = col1;
	अण्णहा
# ifdef FEAT_PROP_POPUP
	    start_col = clip_star.min_col;
# अण्णहा
	    start_col = 0;
# endif

	जइ (row == row2)
	    end_col = col2;
	अण्णहा
# ifdef FEAT_PROP_POPUP
	    end_col = clip_star.max_col;
# अण्णहा
	    end_col = Columns;
# endif

	पङ्क्तिः_end_col = clip_get_पङ्क्तिः_end(&clip_star, row);

	// See if we need to nuke some trailing whitespace
	जइ (end_col >=
# ifdef FEAT_PROP_POPUP
		clip_star.max_col
# अण्णहा
		Columns
# endif
		    && (row < row2 || end_col > पङ्क्तिः_end_col))
	{
	    // Get rid of trailing whitespace
	    end_col = पङ्क्तिः_end_col;
	    जइ (end_col < start_col)
		end_col = start_col;

	    // If the last line extended to the end, add an extra newline
	    जइ (row == row2)
		add_newline_flag = TRUE;
	}

	// If after the first row, we need to always add a newline
	जइ (row > row1 && !पङ्क्तिः_Wraps[row - 1])
	    *bufp++ = NL;

	// Safetey check for in case resizing went wrong
	जइ (row < screen_Rows && end_col <= screen_Columns)
	{
	    जइ (enc_dbcs != 0)
	    {
		पूर्णाङ्क	i;

		p = Screen_पङ्क्तयः + पङ्क्तिः_Offset[row];
		प्रति (i = start_col; i < end_col; ++i)
		    जइ (enc_dbcs == DBCS_JPNU && p[i] == 0x8e)
		    {
			// single-width double-byte char
			*bufp++ = 0x8e;
			*bufp++ = ScreenLines2[पङ्क्तिः_Offset[row] + i];
		    }
		    अण्णहा
		    {
			*bufp++ = p[i];
			जइ (MB_BYTE2LEN(p[i]) == 2)
			    *bufp++ = p[++i];
		    }
	    }
	    अण्णहा जइ (enc_utf8)
	    {
		पूर्णाङ्क	off;
		पूर्णाङ्क	i;

		off = पङ्क्तिः_Offset[row];
		प्रति (i = start_col; i < end_col; ++i)
		{
		    // The base character is either in ScreenLinesUC[] or
		    // ScreenLines[].
		    जइ (Screen_पङ्क्तयः_UC[off + i] == 0)
			*bufp++ = Screen_पङ्क्तयः[off + i];
		    अण्णहा
			// Includes the composing characters.
			bufp += utfc_char2bytes(off + i, bufp);
		    // Skip right half of double-wide character.
		    जइ (Screen_पङ्क्तयः[off + i + 1] == 0)
			++i;
		}
	    }
	    अण्णहा
	    {
		STRNCPY(bufp, Screen_पङ्क्तयः + पङ्क्तिः_Offset[row] + start_col,
							 end_col - start_col);
		bufp += end_col - start_col;
	    }
	}
    }

    // Add a newline at the end if the selection ended there
    जइ (add_newline_flag)
	*bufp++ = NL;

    // First cleanup any old selection and become the owner.
    clip_free_selection(cbd);
    clip_own_selection(cbd);

    // Yank the text into the '*' register.
    clip_yank_selection(MCHAR, buffer, (long)(bufp - buffer), cbd);

    // Make the register contents available to the outside world.
    clip_gen_set_selection(cbd);

# ifdef FEAT_X11
    जइ (both)
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

    शून्य
clip_gen_set_selection(Clipboard_T *cbd)
{
    जइ (!clip_did_set_selection)
    {
	// Updating postponed, so that accessing the system clipboard won't
	// hang Vim when accessing it many times (e.g. on a :g command).
	जइ ((cbd == &clip_plus && (clip_unnamed_saved & CLIP_UNNAMED_PLUS))
		|| (cbd == &clip_star && (clip_unnamed_saved & CLIP_UNNAMED)))
	{
	    clipboard_needs_update = TRUE;
	    प्रत्यागम;
	}
    }
# जइ defined(FEAT_XCLIPBOARD) || defined(FEAT_WAYLAND_CLIPBOARD)
#  ifdef FEAT_GUI
    जइ (gui.in_use)
	clip_mch_set_selection(cbd);
    अण्णहा
#  endif
    {
	जइ (clipmethod == CLIPMETHOD_WAYLAND)
	{
#  ifdef FEAT_WAYLAND_CLIPBOARD
	    clip_wl_set_selection(cbd);
#  endif
	}
	अण्णहा जइ (clipmethod == CLIPMETHOD_X11)
	{
#  ifdef FEAT_XCLIPBOARD
	    clip_xterm_set_selection(cbd);
#  endif
	}
    }
# अण्णहा
    clip_mch_set_selection(cbd);
# endif
}

    स्थिर शून्य
clip_gen_request_selection(Clipboard_T *cbd)
{
# जइ defined(FEAT_XCLIPBOARD) || defined(FEAT_WAYLAND_CLIPBOARD)
#  ifdef FEAT_GUI
    जइ (gui.in_use)
	clip_mch_request_selection(cbd);
    अण्णहा
#  endif
    {
	जइ (clipmethod == CLIPMETHOD_WAYLAND)
	{
#  ifdef FEAT_WAYLAND_CLIPBOARD
	    clip_wl_request_selection(cbd);
#  endif
	}
	अण्णहा जइ (clipmethod == CLIPMETHOD_X11)
	{
#  ifdef FEAT_XCLIPBOARD
	    clip_xterm_request_selection(cbd);
#  endif
	}
    }
# अण्णहा
    clip_mch_request_selection(cbd);
# endif
}

# जइ (defined(FEAT_X11) && defined(FEAT_XCLIPBOARD) && defined(USE_SYSTEM))
    स्थिर पूर्णाङ्क
clip_x11_owner_exists(Clipboard_T *cbd)
{
    प्रत्यागम XGetSelectionOwner(X_DISPLAY, cbd->sel_atom) != None;
}
# endif

# जइ (defined(FEAT_X11) || defined(FEAT_WAYLAND_CLIPBOARD)) \
	&& defined(USE_SYSTEM)
    पूर्णाङ्क
clip_gen_owner_exists(Clipboard_T *cbd UNUSED)
{
#  जइ defined(FEAT_XCLIPBOARD) || defined(FEAT_WAYLAND_CLIPBOARD)
#   ifdef FEAT_GUI_GTK
    जइ (gui.in_use)
	प्रत्यागम clip_gtk_owner_exists(cbd);
    अण्णहा
#   endif
    {
	जइ (clipmethod == CLIPMETHOD_WAYLAND)
	{
#   ifdef FEAT_WAYLAND_CLIPBOARD
	    प्रत्यागम clip_wl_owner_exists(cbd);
#   endif
	}
	अण्णहा जइ (clipmethod == CLIPMETHOD_X11)
	{
#   ifdef FEAT_XCLIPBOARD
	    प्रत्यागम clip_x11_owner_exists(cbd);
#   endif
	}
	अण्णहा
	    प्रत्यागम FALSE;
    }
#  अण्णहा
    प्रत्यागम TRUE;
#  endif
}
# endif

/*
 * Stuff for the X clipboard.  Shared between VMS and Unix.
 */

# जइ defined(FEAT_XCLIPBOARD) || defined(FEAT_GUI_X11)
#  include <X11/Xatom.h>
#  include <X11/Intrinsic.h>

/*
 * Open the application context (if it hasn't been opened yet).
 * Used for Motif GUI and the xterm clipboard.
 */
    शून्य
open_app_context(शून्य)
{
    जइ (app_context == रिक्त)
    {
	XtToolkitInitialize();
	app_context = XtCreateApplicationContext();
    }
}

स्थिर Atom	vim_atom;	// Vim's own special selection format
स्थिर Atom	vimenc_atom;	// Vim's extended selection format
स्थिर Atom	vim_mt_atom;	// Vim's own special selection format (in mime
				// type format)
स्थिर Atom	vimenc_mt_atom;	// Vim's extended selection format (in mime type
				// format)
स्थिर Atom	utf8_atom;
स्थिर Atom	compound_text_atom;
स्थिर Atom	text_atom;
स्थिर Atom	targets_atom;
स्थिर Atom	timestamp_atom;	// Used to get a timestamp

    शून्य
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

    स्थिर Boolean
clip_x11_convert_selection_cb(
    Widget	w UNUSED,
    Atom	*sel_atom,
    Atom	*target,
    Atom	*type,
    XtPointer	*मानम्,
    long_u	*length,
    पूर्णाङ्क		*format)
{
    स्थिर char_u   *save_result = रिक्त;
    स्थिर long_u   save_length = 0;
    char_u	    *string;
    पूर्णाङ्क		    motion_type;
    Clipboard_T    *cbd;
    पूर्णाङ्क		    i;

    जइ (*sel_atom == clip_plus.sel_atom)
	cbd = &clip_plus;
    अण्णहा
	cbd = &clip_star;

    जइ (!cbd->owned)
	प्रत्यागम False;	    // Shouldn't ever happen

    // requestor wants to know what target types we support
    जइ (*target == targets_atom)
    {
	स्थिर Atom array[9];

	*मानम् = (XtPointer)array;
	i = 0;
	array[i++] = targets_atom;
	array[i++] = vimenc_atom;
	array[i++] = vim_atom;
	array[i++] = vimenc_mt_atom;
	array[i++] = vim_mt_atom;
	जइ (enc_utf8)
	    array[i++] = utf8_atom;
	array[i++] = XA_STRING;
	array[i++] = text_atom;
	array[i++] = compound_text_atom;

	*type = XA_ATOM;
	// This used to be: *format = sizeof(Atom) * 8; but that caused
	// crashes on 64 bit machines. (Peter Derr)
	*format = 32;
	*length = i;
	प्रत्यागम True;
    }

    जइ (       *target != XA_STRING
	    && *target != vimenc_atom
	    && *target != vimenc_mt_atom
	    && (*target != utf8_atom || !enc_utf8)
	    && *target != vim_atom
	    && *target != vim_mt_atom
	    && *target != text_atom
	    && *target != compound_text_atom)
	प्रत्यागम False;

    clip_get_selection(cbd);
    motion_type = clip_convert_selection(&string, length, cbd);
    जइ (motion_type < 0)
	प्रत्यागम False;

    // For our own format, the first byte contains the motion type
    जइ (*target == vim_atom || *target == vim_mt_atom)
	(*length)++;

    // Our own format with encoding: motion 'encoding' NUL text
    जइ (*target == vimenc_atom || *target == vimenc_mt_atom)
	*length += STRLEN(p_enc) + 2;

    जइ (save_length < *length || save_length / 2 >= *length)
	*मानम् = XtRealloc((वर्ण *)save_result, (Cardinal)*length + 1);
    अण्णहा
	*मानम् = save_result;
    जइ (*मानम् == रिक्त)
    {
	vim_free(string);
	प्रत्यागम False;
    }
    save_result = (char_u *)*मानम्;
    save_length = *length;

    जइ (*target == XA_STRING || (*target == utf8_atom && enc_utf8))
    {
	mch_memmove(save_result, string, (size_t)(*length));
	*type = *target;
    }
    अण्णहा जइ (*target == compound_text_atom || *target == text_atom)
    {
	XTextProperty	text_prop;
	वर्ण		*string_nt = (वर्ण *)save_result;
	पूर्णाङ्क		conv_result;

	// create NUL terminated string which XmbTextListToTextProperty wants
	mch_memmove(string_nt, string, (size_t)*length);
	string_nt[*length] = NUL;
	conv_result = XmbTextListToTextProperty(X_DISPLAY, &string_nt,
					   1, XCompoundTextStyle, &text_prop);
	जइ (conv_result != Success)
	{
	    vim_free(string);
	    प्रत्यागम False;
	}
	*मानम् = (XtPointer)(text_prop.मानम्);	//    from plain text
	*length = text_prop.nitems;
	*type = compound_text_atom;
	XtFree((वर्ण *)save_result);
	save_result = (char_u *)*मानम्;
	save_length = *length;
    }
    अण्णहा जइ (*target == vimenc_atom || *target == vimenc_mt_atom)
    {
	पूर्णाङ्क l = STRLEN(p_enc);

	save_result[0] = motion_type;
	STRCPY(save_result + 1, p_enc);
	mch_memmove(save_result + l + 2, string, (size_t)(*length - l - 2));
	जइ (*target == vimenc_atom)
	    *type = vimenc_atom;
	अण्णहा
	    *type = vimenc_mt_atom;
    }
    अण्णहा
    {
	save_result[0] = motion_type;
	mch_memmove(save_result + 1, string, (size_t)(*length - 1));
	जइ (*target == vim_atom)
	    *type = vim_atom;
	अण्णहा
	    *type = vim_mt_atom;
    }
    *format = 8;	    // 8 bits per char
    vim_free(string);
    प्रत्यागम True;
}

    स्थिर शून्य
clip_x11_lose_ownership_cb(Widget w UNUSED, Atom *sel_atom)
{
    जइ (*sel_atom == clip_plus.sel_atom)
	clip_lose_selection(&clip_plus);
    अण्णहा
	clip_lose_selection(&clip_star);
}

    स्थिर शून्य
clip_x11_notify_cb(Widget w UNUSED, Atom *sel_atom UNUSED, Atom *target UNUSED)
{
    // To prevent automatically freeing the selection value.
}

/*
 * Property callback to get a timestamp for XtOwnSelection.
 */
#  जइ defined(FEAT_X11) && defined(FEAT_XCLIPBOARD)
    स्थिर शून्य
clip_x11_timestamp_cb(
    Widget	w,
    XtPointer	n UNUSED,
    XEvent	*event,
    Boolean	*cont UNUSED)
{
    Atom	    actual_type;
    पूर्णाङ्क		    format;
    unsigned  long  nitems, bytes_after;
    unsigned वर्ण   *prop=रिक्त;
    XPropertyEvent  *xproperty=&event->xproperty;

    // Must be a property notify, state can't be Delete (True), has to be
    // one of the supported selection types.
    जइ (event->type != PropertyNotify || xproperty->state
	    || (xproperty->atom != clip_star.sel_atom
				    && xproperty->atom != clip_plus.sel_atom))
	प्रत्यागम;

    जइ (XGetWindowProperty(xproperty->display, xproperty->window,
	  xproperty->atom, 0, 0, False, timestamp_atom, &actual_type, &format,
						&nitems, &bytes_after, &prop))
	प्रत्यागम;

    जइ (prop)
	XFree(prop);

    // Make sure the property type is "TIMESTAMP" and it's 32 bits.
    जइ (actual_type != timestamp_atom || format != 32)
	प्रत्यागम;

    // Get the selection, using the event timestamp.
    जइ (XtOwnSelection(w, xproperty->atom, xproperty->time,
	    clip_x11_convert_selection_cb, clip_x11_lose_ownership_cb,
	    clip_x11_notify_cb) == OK)
    {
	// Set the "owned" flag now, there may have been a call to
	// lose_ownership_cb in between.
	जइ (xproperty->atom == clip_plus.sel_atom)
	    clip_plus.owned = TRUE;
	अण्णहा
	    clip_star.owned = TRUE;
    }
}

    शून्य
x11_setup_selection(Widget w)
{
    XtAddEventHandler(w, PropertyChangeMask, False,
	    /*(XtEventHandler)*/clip_x11_timestamp_cb, (XtPointer)रिक्त);
}
#  endif

    स्थिर शून्य
clip_x11_request_selection_cb(
    Widget	w UNUSED,
    XtPointer	success,
    Atom	*sel_atom,
    Atom	*type,
    XtPointer	मानम्,
    long_u	*length,
    पूर्णाङ्क		*format)
{
    पूर्णाङ्क		motion_type = MAUTO;
    long_u	len;
    char_u	*p;
    वर्ण	**text_list = रिक्त;
    Clipboard_T	*cbd;
    char_u	*tmpbuf = रिक्त;

    जइ (*sel_atom == clip_plus.sel_atom)
	cbd = &clip_plus;
    अण्णहा
	cbd = &clip_star;

    जइ (मानम् == रिक्त || *length == 0)
    {
	clip_free_selection(cbd);	// nothing received, clear register
	*(पूर्णाङ्क *)success = FALSE;
	प्रत्यागम;
    }
    p = (char_u *)मानम्;
    len = *length;
    जइ (*type == vim_atom || *type == vim_mt_atom)
    {
	motion_type = *p++;
	len--;
    }

    अण्णहा जइ (*type == vimenc_atom || *type == vimenc_mt_atom)
    {
	char_u		*enc;
	vimconv_T	conv;
	पूर्णाङ्क		convlen;

	motion_type = *p++;
	--len;

	enc = p;
	p += STRLEN(p) + 1;
	len -= p - enc;

	// If the encoding of the text is different from 'encoding', attempt
	// converting it.
	conv.vc_type = CONV_NONE;
	convert_setup(&conv, enc, p_enc);
	जइ (conv.vc_type != CONV_NONE)
	{
	    convlen = len;	// Need to use an int here.
	    tmpbuf = string_convert(&conv, p, &convlen);
	    len = convlen;
	    जइ (tmpbuf != रिक्त)
		p = tmpbuf;
	    convert_setup(&conv, रिक्त, रिक्त);
	}
    }

    अण्णहा जइ (*type == compound_text_atom
	    || *type == utf8_atom
	    || (enc_dbcs != 0 && *type == text_atom))
    {
	XTextProperty	text_prop;
	पूर्णाङ्क		n_text = 0;
	पूर्णाङ्क		status;

	text_prop.मानम् = (unsigned वर्ण *)मानम्;
	text_prop.encoding = *type;
	text_prop.format = *format;
	text_prop.nitems = len;
#  जइ defined(X_HAVE_UTF8_STRING)
	जइ (*type == utf8_atom)
	    status = Xutf8TextPropertyToTextList(X_DISPLAY, &text_prop,
							 &text_list, &n_text);
	अण्णहा
#  endif
	    status = XmbTextPropertyToTextList(X_DISPLAY, &text_prop,
							 &text_list, &n_text);
	जइ (status != Success || n_text < 1)
	{
	    *(पूर्णाङ्क *)success = FALSE;
	    प्रत्यागम;
	}
	p = (char_u *)text_list[0];
	len = STRLEN(p);
    }
    clip_yank_selection(motion_type, p, (long)len, cbd);

    जइ (text_list != रिक्त)
	XFreeStringList(text_list);
    vim_free(tmpbuf);
    XtFree((वर्ण *)मानम्);
    *(पूर्णाङ्क *)success = TRUE;
}

    शून्य
clip_x11_request_selection(
    Widget	myShell,
    Display	*dpy,
    Clipboard_T	*cbd)
{
    XEvent	event;
    Atom	type;
    स्थिर पूर्णाङ्क	success;
    पूर्णाङ्क		i;
    time_t	start_time;
    पूर्णाङ्क		timed_out = FALSE;

    प्रति (i = 0; i < 8; i++)
    {
	विकल्प (i)
	{
	    पक्ष 0:  type = vimenc_mt_atom;	भङ्ग;
	    पक्ष 1:  type = vimenc_atom;	भङ्ग;
	    पक्ष 2:  type = vim_mt_atom;	भङ्ग;
	    पक्ष 3:  type = vim_atom;		भङ्ग;
	    पक्ष 4:  type = utf8_atom;		भङ्ग;
	    पक्ष 5:  type = compound_text_atom; भङ्ग;
	    पक्ष 6:  type = text_atom;		भङ्ग;
	    स्वभाव: type = XA_STRING;
	}
	जइ (type == utf8_atom
#  जइ defined(X_HAVE_UTF8_STRING)
		&& !enc_utf8
#  endif
		)
	    // Only request utf-8 when 'encoding' is utf8 and
	    // Xutf8TextPropertyToTextList is available.
	    अनुवर्त;
	success = MAYBE;
	Xt_Get_Selection_मानम्(myShell, cbd->sel_atom, type,
	    clip_x11_request_selection_cb, (XtPointer)&success, CurrentTime);

	// Make sure the request for the selection goes out before waiting for
	// a response.
	XFlush(dpy);

	/*
	 * Wait for result of selection request, otherwise if we type more
	 * characters, then they will appear before the one that requested the
	 * paste!  Don't worry, we will catch up with any other events later.
	 */
	start_time = time(रिक्त);
	जाव (success == MAYBE)
	{
	    जइ (XCheckTypedEvent(dpy, PropertyNotify, &event)
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
		अनुवर्त;
	    }

	    // Time out after 2 to 3 seconds to avoid that we hang when the
	    // other process doesn't respond.  Note that the SelectionNotify
	    // event may still come later when the selection owner comes back
	    // to life and the text gets inserted unexpectedly.  Don't know
	    // why that happens or how to avoid that :-(.
	    जइ (time(रिक्त) > start_time + 2)
	    {
		timed_out = TRUE;
		भङ्ग;
	    }

	    // Do we need this?  Probably not.
	    XSync(dpy, False);

	    // Wait for 1 msec to avoid that we eat up all CPU time.
	    ui_delay(1L, TRUE);
	}

	जइ (success == TRUE)
	    प्रत्यागम;

	// don't do a retry with another type after timing out, otherwise we
	// hang for 15 seconds.
	जइ (timed_out)
	    भङ्ग;
    }

    // Final fallback position - use the X CUT_BUFFER0 store
    yank_cut_buffer0(dpy, cbd);
}

    शून्य
clip_x11_lose_selection(Widget myShell, Clipboard_T *cbd)
{
    XtDisownSelection(myShell, cbd->sel_atom,
				XtLastTimestampProcessed(XtDisplay(myShell)));
}

    पूर्णाङ्क
clip_x11_own_selection(Widget myShell, Clipboard_T *cbd)
{
    // When using the GUI we have proper timestamps, use the one of the last
    // event.  When in the console we don't get events (the terminal gets
    // them), Get the time by a zero-length append, clip_x11_timestamp_cb will
    // be called with the current timestamp.
#  ifdef FEAT_GUI
    जइ (gui.in_use)
    {
	जइ (XtOwnSelection(myShell, cbd->sel_atom,
	       XtLastTimestampProcessed(XtDisplay(myShell)),
	       clip_x11_convert_selection_cb, clip_x11_lose_ownership_cb,
	       clip_x11_notify_cb) == False)
	    प्रत्यागम FAIL;
    }
    अण्णहा
#  endif
    {
	जइ (!XChangeProperty(XtDisplay(myShell), XtWindow(myShell),
		  cbd->sel_atom, timestamp_atom, 32, PropModeAppend, रिक्त, 0))
	    प्रत्यागम FAIL;
    }
    // Flush is required in a terminal as nothing else is doing it.
    XFlush(XtDisplay(myShell));
    प्रत्यागम OK;
}

/*
 * Send the current selection to the clipboard.  Do nothing for X because we
 * will fill in the selection only when requested by another app.
 */
    शून्य
clip_x11_set_selection(Clipboard_T *cbd UNUSED)
{
}

# endif

# जइ (defined(FEAT_XCLIPBOARD) || defined(FEAT_GUI_X11) || defined(FEAT_GUI_GTK)) \
	&& !defined(USE_GTK4)
/*
 * Get the contents of the X CUT_BUFFER0 and put it in "cbd".
 */
    शून्य
yank_cut_buffer0(Display *dpy, Clipboard_T *cbd)
{
    पूर्णाङ्क		nbytes = 0;
    char_u	*buffer = (char_u *)XFetchBuffer(dpy, &nbytes, 0);

    जइ (nbytes > 0)
    {
	पूर्णाङ्क  done = FALSE;

	// CUT_BUFFER0 is supposed to be always latin1.  Convert to 'enc' when
	// using a multi-byte encoding.  Conversion between two 8-bit
	// character sets usually fails and the text might actually be in
	// 'enc' anyway.
	जइ (has_mbyte)
	{
	    char_u	*conv_buf;
	    vimconv_T	vc;

	    vc.vc_type = CONV_NONE;
	    जइ (convert_setup(&vc, (char_u *)"latin1", p_enc) == OK)
	    {
		conv_buf = string_convert(&vc, buffer, &nbytes);
		जइ (conv_buf != रिक्त)
		{
		    clip_yank_selection(MCHAR, conv_buf, (long)nbytes, cbd);
		    vim_free(conv_buf);
		    done = TRUE;
		}
		convert_setup(&vc, रिक्त, रिक्त);
	    }
	}
	जइ (!done)  // use the text without conversion
	    clip_yank_selection(MCHAR, buffer, (long)nbytes, cbd);
	XFree((शून्य *)buffer);
	जइ (p_verbose > 0)
	{
	    verbose_enter();
	    verb_सन्देशः(_("Used CUT_BUFFER0 instead of empty selection"));
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
# जइ defined(FEAT_X11) && defined(FEAT_CLIPBOARD)
    शून्य
x11_export_final_selection(शून्य)
{
    Display	*dpy;
    char_u	*str = रिक्त;
    long_u	len = 0;
    पूर्णाङ्क		motion_type = -1;

#  ifdef FEAT_GUI
    जइ (gui.in_use)
	dpy = X_DISPLAY;
    अण्णहा
#  endif
#  ifdef FEAT_XCLIPBOARD
	dpy = xterm_dpy;
#  अण्णहा
	प्रत्यागम;
#  endif

    // Get selection to export
    जइ (clip_plus.owned)
	motion_type = clip_convert_selection(&str, &len, &clip_plus);
    अण्णहा जइ (clip_star.owned)
	motion_type = clip_convert_selection(&str, &len, &clip_star);

    // Check it's OK
    जइ (dpy != रिक्त && str != रिक्त && motion_type >= 0
					       && len < 1024*1024 && len > 0)
    {
	पूर्णाङ्क ok = TRUE;

	// The CUT_BUFFER0 is supposed to always contain latin1.  Convert from
	// 'enc' when it is a multi-byte encoding.  When 'enc' is an 8-bit
	// encoding conversion usually doesn't work, so keep the text as-is.
	जइ (has_mbyte)
	{
	    vimconv_T	vc;

	    vc.vc_type = CONV_NONE;
	    जइ (convert_setup(&vc, p_enc, (char_u *)"latin1") == OK)
	    {
		पूर्णाङ्क	intlen = len;
		char_u	*conv_str;

		vc.vc_fail = TRUE;
		conv_str = string_convert(&vc, str, &intlen);
		len = intlen;
		जइ (conv_str != रिक्त)
		{
		    vim_free(str);
		    str = conv_str;
		}
		अण्णहा
		{
		    ok = FALSE;
		}
		convert_setup(&vc, रिक्त, रिक्त);
	    }
	    अण्णहा
	    {
		ok = FALSE;
	    }
	}

	// Do not store the string if conversion failed.  Better to use any
	// other selection than garbled text.
	जइ (ok)
	{
	    XStoreBuffer(dpy, (वर्ण *)str, (पूर्णाङ्क)len, 0);
	    XFlush(dpy);
	}
    }

    vim_free(str);
}
# endif

    शून्य
clip_free_selection(Clipboard_T *cbd)
{
    yankreg_T *y_ptr = get_y_current();

    जइ (cbd == &clip_plus)
	set_y_current(get_y_register(PLUS_REGISTER));
    अण्णहा
	set_y_current(get_y_register(STAR_REGISTER));
    free_yank_all();
    get_y_current()->y_size = 0;
    set_y_current(y_ptr);
}

/*
 * Get the selected text and put it in register '*' or '+'.
 */
    शून्य
clip_get_selection(Clipboard_T *cbd)
{
    yankreg_T	*old_y_previous, *old_y_current;
    pos_T	old_cursor;
    pos_T	old_visual;
    पूर्णाङ्क		old_visual_mode;
    colnr_T	old_virtcol;
    colnr_T	old_curswant;
    पूर्णाङ्क		old_set_curswant;
    pos_T	old_op_start, old_op_end;
    oparg_T	oa;
    cmdarg_T	ca;

    जइ (cbd->owned)
    {
	जइ ((cbd == &clip_plus
		&& get_y_register(PLUS_REGISTER)->y_array != रिक्त)
		|| (cbd == &clip_star
		    && get_y_register(STAR_REGISTER)->y_array != रिक्त))
	    प्रत्यागम;

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
    अण्णहा जइ (!is_clipboard_needs_update())
    {
	clip_free_selection(cbd);

	// Try to get selected text from another window
	clip_gen_request_selection(cbd);
    }
}

/*
 * Convert from the GUI selection string into the '*'/'+' register.
 */
    शून्य
clip_yank_selection(
    पूर्णाङ्क		type,
    char_u	*str,
    long	len,
    Clipboard_T *cbd)
{
    yankreg_T *y_ptr;

    जइ (cbd == &clip_plus)
	y_ptr = get_y_register(PLUS_REGISTER);
    अण्णहा
	y_ptr = get_y_register(STAR_REGISTER);

    clip_free_selection(cbd);

    str_to_reg(y_ptr, type, str, len, -1, FALSE);
}

    पूर्णाङ्क
clip_convert_selection_offset(
	char_u	    **str,
	long_u	    *len,
	पूर्णाङ्क	    offset, // Extra space to add in *str and the offset to
			    // place the actual string in *str.
	Clipboard_T *cbd)
{
    char_u	*p;
    पूर्णाङ्क		lnum;
    पूर्णाङ्क		i, j;
    int_u	eolsize;
    yankreg_T	*y_ptr;

    जइ (cbd == &clip_plus)
	y_ptr = get_y_register(PLUS_REGISTER);
    अण्णहा
	y_ptr = get_y_register(STAR_REGISTER);

# ifdef USE_CRNL
    eolsize = 2;
# अण्णहा
    eolsize = 1;
# endif

    *str = रिक्त;
    *len = 0;
    जइ (y_ptr->y_array == रिक्त)
	प्रत्यागम -1;

    प्रति (i = 0; i < y_ptr->y_size; i++)
	*len += (long_u)y_ptr->y_array[i].length + eolsize;

    // Don't want newline character at end of last line if we're in MCHAR mode.
    जइ (y_ptr->y_type == MCHAR && *len >= eolsize)
	*len -= eolsize;

    *len += offset;
    p = *str = alloc(*len + 1);	// add one to avoid zero
    जइ (p == रिक्त)
	प्रत्यागम -1;
    p += offset;
    lnum = 0;
    प्रति (i = 0, j = 0; i < (पूर्णाङ्क)*len - offset; i++, j++)
    {
	जइ (y_ptr->y_array[lnum].string[j] == '\n')
	    p[i] = NUL;
	अण्णहा जइ (y_ptr->y_array[lnum].string[j] == NUL)
	{
# ifdef USE_CRNL
	    p[i++] = '\r';
# endif
	    p[i] = '\n';
	    lnum++;
	    j = -1;
	}
	अण्णहा
	    p[i] = y_ptr->y_array[lnum].string[j];
    }
    प्रत्यागम y_ptr->y_type;
}

/*
 * Convert the '*'/'+' register into a GUI selection string returned in *str
 * with length *len.
 * Returns the motion type, or -1 for failure.
 */
    पूर्णाङ्क
clip_convert_selection(char_u **str, long_u *len, Clipboard_T *cbd)
{
    प्रत्यागम clip_convert_selection_offset(str, len, 0, cbd);
}

/*
 * When "regname" is a clipboard register, obtain the selection.  If it's not
 * available return zero, otherwise return "regname".
 */
    पूर्णाङ्क
may_get_selection(पूर्णाङ्क regname)
{
    जइ (regname == '*')
    {
	जइ (!clip_star.available)
	    regname = 0;
	अण्णहा
	    clip_get_selection(&clip_star);
    }
    अण्णहा जइ (regname == '+')
    {
	जइ (!clip_plus.available)
	    regname = 0;
	अण्णहा
	    clip_get_selection(&clip_plus);
    }
    प्रत्यागम regname;
}

/*
 * If we have written to a clipboard register, send the text to the clipboard.
 */
    शून्य
may_set_selection(शून्य)
{
    जइ ((get_y_current() == get_y_register(STAR_REGISTER))
	    && clip_star.available)
    {
	clip_own_selection(&clip_star);
	clip_gen_set_selection(&clip_star);
    }
    अण्णहा जइ ((get_y_current() == get_y_register(PLUS_REGISTER))
	    && clip_plus.available)
    {
	clip_own_selection(&clip_plus);
	clip_gen_set_selection(&clip_plus);
    }
}

# जइ defined(FEAT_WAYLAND_CLIPBOARD)

    स्थिर clip_wl_selection_T *
clip_wl_get_selection(wayland_selection_T sel)
{
    विकल्प (sel)
    {
	पक्ष WAYLAND_SELECTION_REGULAR:
	    प्रत्यागम &clip_wl.regular;
	पक्ष WAYLAND_SELECTION_PRIMARY:
	    प्रत्यागम &clip_wl.primary;
	स्वभाव:
	    प्रत्यागम रिक्त;
    }
}

    स्थिर clip_wl_selection_T *
clip_wl_get_selection_from_cbd(Clipboard_T *cbd)
{
    जइ (cbd == &clip_plus)
	प्रत्यागम &clip_wl.regular;
    अण्णहा जइ (cbd == &clip_star)
	प्रत्यागम &clip_wl.primary;
    अण्णहा
	प्रत्यागम रिक्त;
}

    स्थिर Clipboard_T *
clip_wl_get_cbd_from_selection(clip_wl_selection_T *sel)
{
    जइ (sel == &clip_wl.regular)
	प्रत्यागम &clip_plus;
    अण्णहा जइ (sel == &clip_wl.primary)
	प्रत्यागम &clip_star;
    अण्णहा
	प्रत्यागम रिक्त;
}

    स्थिर wayland_selection_T
clip_wl_get_selection_type(clip_wl_selection_T *sel)
{
    जइ (sel == &clip_wl.regular)
	प्रत्यागम WAYLAND_SELECTION_REGULAR;
    अण्णहा जइ (sel == &clip_wl.primary)
	प्रत्यागम WAYLAND_SELECTION_PRIMARY;
    अण्णहा
	प्रत्यागम WAYLAND_SELECTION_NONE;
}

    स्थिर bool
wl_data_offer_listener_event_offer(
    शून्य *data UNUSED,
    vwl_data_offer_T *offer UNUSED,
    ध्रुव वर्ण *mime_type
)
{
    // Only accept mime type if we support it
    प्रति (पूर्णाङ्क i = 0; i < (पूर्णाङ्क)ARRAY_LENGTH(supported_mimes); i++)
	जइ (STRCMP(mime_type, supported_mimes[i]) == 0)
	    प्रत्यागम true;
    प्रत्यागम FALSE;
}

स्थिर ध्रुव vwl_data_offer_listener_T vwl_data_offer_listener = {
    .offer = wl_data_offer_listener_event_offer
};

    स्थिर शून्य
vwl_data_device_listener_event_data_offer(
	शून्य *data UNUSED,
	vwl_data_device_T *device UNUSED,
	vwl_data_offer_T *offer)
{
    // Immediately start listening for offer events from the data offer
    vwl_data_offer_add_listener(offer, &vwl_data_offer_listener, रिक्त);
}

    स्थिर शून्य
vwl_data_device_listener_event_selection(
	शून्य *data UNUSED,
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
    जइ (offer == रिक्त || sel->source != रिक्त)
    {
	// Selection event is from us, so we are the source client. Therefore
	// ignore it. Or the selection is cleared, so set sel->offer to NULL
	vwl_data_offer_destroy(offer);
	sel->offer = रिक्त;
	प्रत्यागम;
    }

    // Save offer. When we want to request data, then we'll actually call the
    // receive method.
    sel->offer = offer;

}

    स्थिर शून्य
vwl_data_device_listener_event_finished(
	शून्य *data UNUSED,
	vwl_data_device_T *device)
{
    clip_wl_selection_T *sel;
    // Device finished, guessing this can happen is when the seat becomes
    // invalid? If so, let the user call :wlrestore! to reset. There wouldn't be
    // any point in trying to create another data device for the same seat,
    // since the seat is in an invalid state.
    जइ (device == clip_wl.regular.device)
    {
	sel = &clip_wl.regular;
	clip_wl.regular.device = रिक्त;
    }
    अण्णहा जइ (device == clip_wl.primary.device)
    {
	sel = &clip_wl.primary;
	clip_wl.primary.device = रिक्त;
    }
    अण्णहा
	// Shouldn't happen
	प्रत्यागम;

    vim_free(sel->contents);
    vwl_data_source_destroy(sel->source);
    vwl_data_offer_destroy(sel->offer);
    sel->available = FALSE;

    vwl_data_device_destroy(device);
}

स्थिर ध्रुव vwl_data_device_listener_T vwl_data_device_listener = {
    .data_offer = vwl_data_device_listener_event_data_offer,
    .selection = vwl_data_device_listener_event_selection,
    .finished = vwl_data_device_listener_event_finished
};

/*
 * Initialize the clipboard for Wayland using the global Wayland connection.
 * Returns OK on success and FAIL on failure.
 */
    पूर्णाङ्क
clip_init_wayland(शून्य)
{
    int_u supported = WAYLAND_SELECTION_NONE;

    जइ (wayland_ct == रिक्त)
	प्रत्यागम FAIL;

    memset(&clip_wl, 0, परिमाण(clip_wl));

    clip_wl.seat = vwl_connection_get_seat(wayland_ct, (वर्ण *)p_wse);

    जइ (clip_wl.seat == रिक्त)
	प्रत्यागम FAIL;

    clip_wl.regular.manager = vwl_connection_get_data_device_manager(
	    wayland_ct, WAYLAND_SELECTION_REGULAR, &supported);

    जइ (clip_wl.regular.manager != रिक्त)
    {
	clip_wl.regular.device = vwl_data_device_manager_get_data_device(
		clip_wl.regular.manager, clip_wl.seat);

	जइ (clip_wl.regular.device != रिक्त)
	    clip_wl.regular.available = true;
	अण्णहा
	{
	    // Shouldn't happen
	    vwl_data_device_manager_discard(clip_wl.regular.manager);
	    clip_wl.regular.manager = रिक्त;
	    प्रत्यागम FAIL;
	}
    }

    // If we still don't support the primary selection, try finding one for it
    // specifically.
    जइ (!(supported & WAYLAND_SELECTION_PRIMARY))
    {
	clip_wl.primary.manager = vwl_connection_get_data_device_manager(
		wayland_ct, WAYLAND_SELECTION_PRIMARY, &supported);

	जइ (clip_wl.primary.manager != रिक्त)
	{
	    clip_wl.primary.device = vwl_data_device_manager_get_data_device(
		    clip_wl.primary.manager, clip_wl.seat);

	    जइ (clip_wl.primary.device != रिक्त)
		clip_wl.primary.available = true;
	    अण्णहा
	    {
		vwl_data_device_manager_discard(clip_wl.primary.manager);
		clip_wl.primary.manager = रिक्त;
	    }
	}
    }

    जइ (clip_wl.regular.available && !clip_wl.primary.available)
    {
	// The protocol supports both regular and primary selections, just use
	// one data device manager and one data device. Or the primary selection
	// is not supported, make it point to the regular selection instead.
	clip_wl.primary.available = true;
	clip_wl.primary.manager = clip_wl.regular.manager;
	clip_wl.primary.device = clip_wl.regular.device;
    }

    जइ (!clip_wl.regular.available && !clip_wl.primary.available)
	प्रत्यागम FAIL;

    // Start listening for selection updates
    जइ (clip_wl.regular.device != रिक्त)
	vwl_data_device_add_listener(clip_wl.regular.device,
		&vwl_data_device_listener, रिक्त);
    // Don't want to listen to the same data device twice
    जइ (clip_wl.primary.device != रिक्त
	    && clip_wl.primary.device != clip_wl.regular.device)
	vwl_data_device_add_listener(clip_wl.primary.device,
		&vwl_data_device_listener, रिक्त);

    प्रत्यागम OK;
}

    शून्य
clip_uninit_wayland(शून्य)
{
    clip_wl_selection_T *sel;

    जइ (clipmethod == CLIPMETHOD_WAYLAND)
    {
	जइ (clip_star.owned)
	    clip_lose_selection(&clip_star);
	जइ (clip_plus.owned)
	    clip_lose_selection(&clip_plus);
    }

    // Don't want to double free
    जइ (clip_wl.regular.manager != clip_wl.primary.manager)
	vwl_data_device_manager_discard(clip_wl.primary.manager);
    vwl_data_device_manager_discard(clip_wl.regular.manager);

    जइ (clip_wl.regular.device != clip_wl.primary.device)
	vwl_data_device_destroy(clip_wl.primary.device);
    vwl_data_device_destroy(clip_wl.regular.device);

    sel = &clip_wl.regular;
    जाव (true)
    {
	vim_free(sel->contents);
	vwl_data_source_destroy(sel->source);
	vwl_data_offer_destroy(sel->offer);
	sel->available = false;

	जइ (sel == &clip_wl.primary)
	    भङ्ग;
	sel = &clip_wl.primary;
    }

    vim_memset(&clip_wl, 0, परिमाण(clip_wl));
}

    पूर्णाङ्क
clip_reset_wayland(शून्य)
{
    wayland_uninit_connection();

    जइ (wayland_init_connection(wayland_display_name) == FAIL
	    || clip_init_wayland() == FAIL)
	प्रत्यागम FAIL;

    choose_clipmethod();
    प्रत्यागम OK;
}


/*
 * Read data from a file descriptor and write it to the given clipboard.
 */
    स्थिर शून्य
clip_wl_receive_data(Clipboard_T *cbd, ध्रुव वर्ण *mime_type, पूर्णाङ्क fd)
{
    char_u	*start, *final;
    long	len;
    garray_T	buf;
    पूर्णाङ्क		motion_type = MAUTO;
    ssize_t	r = 0;
    char_u	*tofree = रिक्त;
#  ifndef HAVE_SELECT
    संरचना pollfd   pfd;

    pfd.fd = fd;
    pfd.events = POLLIN;
#  अण्णहा
    fd_set rfds;
    संरचना timeval  tv;

    FD_ZERO(&rfds);
    FD_SET(fd, &rfds);
#  endif

    // Make pipe (read end) non-blocking
    जइ (fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK) == -1)
	प्रत्यागम;

    ga_init2(&buf, 1, 4096);

    // 4096 bytes seems reasonable for initial buffer size, memory is cheap
    // anyways.
    जइ (ga_grow(&buf, 4096) == FAIL)
	प्रत्यागम;

    start = buf.ga_data;

#  ifndef HAVE_SELECT
    जाव (poll(&pfd, 1, p_wtm) > 0)
#  अण्णहा
    जाव (tv.tv_sec = p_wtm / 1000, tv.tv_usec = (p_wtm % 1000) * 1000,
	    select(fd + 1, &rfds, रिक्त, रिक्त, &tv) > 0)
#  endif
    {
	r = पठ(fd, start, buf.ga_maxlen - 1 - buf.ga_len);

	जइ (r == 0)
	    भङ्ग;
	अण्णहा जइ (r < 0)
	{
	    जइ (errno == EAGAIN || errno == EINTR)
		अनुवर्त;
	    भङ्ग;
	}

	start += r;
	buf.ga_len += r;

	// Realloc if we are at the end of the buffer
	जइ (buf.ga_len >= buf.ga_maxlen - 1)
	{
	    जइ (ga_grow(&buf, 8192) == FAIL)
		भङ्ग;
	    start = (char_u *)buf.ga_data + buf.ga_len;
	}
    }

    जइ (buf.ga_len == 0)
    {
	clip_free_selection(cbd); // Nothing received, clear register
	ga_clear(&buf);
	प्रत्यागम;
    }

    final = buf.ga_data;
    len = buf.ga_len;

    जइ (clip_convert_data(&final, &len, &motion_type,
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
    स्थिर शून्य
clip_wl_request_selection(Clipboard_T *cbd)
{
    clip_wl_selection_T *sel = clip_wl_get_selection_from_cbd(cbd);
    पूर्णाङ्क			fds[2];
    पूर्णाङ्क			mime_types_len;
    ध्रुव वर्ण		**mime_types;
    ध्रुव वर्ण		*chosen_mime = रिक्त;

    जइ (!sel->available)
	गच्छ clear;

    // Dispatch any events that still queued up before checking for a data
    // offer.
    जइ (vwl_connection_roundtrip(wayland_ct) == FAIL)
	गच्छ clear;

    जइ (sel->offer == रिक्त)
	गच्छ clear;

    mime_types_len = sel->offer->mime_types.ga_len;
    mime_types = sel->offer->mime_types.ga_data;

    // Choose mime type to receive from. Mime types with a lower index in the
    // "supported_mimes" array are prioritized over ones after it.
    प्रति (पूर्णाङ्क i = 0; i < (पूर्णाङ्क)ARRAY_LENGTH(supported_mimes)
	    && chosen_mime == रिक्त; i++)
    {
	प्रति (पूर्णाङ्क k = 0; k < mime_types_len && chosen_mime == रिक्त; k++)
	    जइ (STRCMP(mime_types[k], supported_mimes[i]) == 0)
		chosen_mime = supported_mimes[i];
    }

    जइ (chosen_mime == रिक्त || pipe(fds) == -1)
	गच्छ clear;

    vwl_data_offer_receive(sel->offer, chosen_mime, fds[1]);

    close(fds[1]); // Close before we read data so that when the source client
		   // closes their end we receive an EOF.

    जइ (vwl_connection_flush(wayland_ct) >= 0)
	clip_wl_receive_data(cbd, chosen_mime, fds[0]);

    close(fds[0]);

    प्रत्यागम;
clear:
    clip_free_selection(cbd);
}

    स्थिर शून्य
vwl_data_source_listener_event_send(
    शून्य *data,
    vwl_data_source_T *source UNUSED,
    ध्रुव वर्ण *mime_type,
    int32_t fd
)
{
    clip_wl_selection_T *sel = data;
    Clipboard_T		*cbd = clip_wl_get_cbd_from_selection(sel);
    bool		have_mime = false;
    पूर्णाङ्क			motion_type;
    long_u		length;
    char_u		*string; // Will be reallocated to a bigger size if
				 // needed.
    पूर्णाङ्क			offset = 0;
    bool		is_vim, is_vimenc;
    size_t		total = 0;
#  ifndef HAVE_SELECT
    संरचना pollfd   pfd;

    pfd.fd = fd;
    pfd.events = POLLOUT;
#  अण्णहा
    fd_set	    wfds;
    संरचना timeval  tv;

    FD_ZERO(&wfds);
    FD_SET(fd, &wfds);
#  endif

    // Check if we actually have mime type
    प्रति (पूर्णाङ्क i = 0; i < (पूर्णाङ्क)ARRAY_LENGTH(supported_mimes); i++)
	जइ (STRCMP(supported_mimes[i], mime_type) == 0)
	{
	    have_mime = true;
	    भङ्ग;
	}

    जइ (!have_mime)
	गच्छ exit;

    // First byte sent is motion type for vim specific formats. For the vimenc
    // format, after the first byte is the encoding type, which is null
    // terminated.

    is_vimenc = STRCMP(mime_type, VIMENC_ATOM_NAME) == 0
	|| STRCMP(mime_type, VIMENC_MIMETYPE_NAME) == 0;
    is_vim = STRCMP(mime_type, VIM_ATOM_NAME) == 0
	|| STRCMP(mime_type, VIM_MIMETYPE_NAME) == 0;

    जइ (is_vimenc)
	offset += 2 + STRLEN(p_enc);
    अण्णहा जइ (is_vim)
	offset += 1;

    clip_get_selection(cbd);
    motion_type = clip_convert_selection_offset(&string, &length, offset, cbd);

    जइ (motion_type < 0)
	गच्छ exit;

    जइ (is_vimenc)
    {
	string[0] = (char_u)motion_type;
	// Use vim_strncpy for safer copying
	vim_strncpy(string + 1, p_enc, STRLEN(p_enc));
    }
    अण्णहा जइ (is_vim)
	string[0] = (char_u)motion_type;


    जाव (total < (size_t)length &&
#  ifndef HAVE_SELECT
	    poll(&pfd, 1, p_wtm) > 0)
#  अण्णहा
	    ((tv.tv_sec = p_wtm / 1000, tv.tv_usec = (p_wtm % 1000) * 1000),
	    select(fd + 1, रिक्त, &wfds, रिक्त, &tv) > 0))
#  endif
    {
	ssize_t w = लिख(fd, string + total, length - total);

	जइ (w == -1)
	    भङ्ग;
	total += w;
    }

    vim_free(string);
exit:
    close(fd);
}

    स्थिर शून्य
vwl_data_source_listener_event_cancelled(
	शून्य *data,
	vwl_data_source_T *source UNUSED)
{
    clip_wl_selection_T *sel = data;
    Clipboard_T		*cbd = clip_wl_get_cbd_from_selection(sel);

    clip_lose_selection(cbd);
}

स्थिर ध्रुव vwl_data_source_listener_T vwl_data_source_listener = {
    .send = vwl_data_source_listener_event_send,
    .cancelled = vwl_data_source_listener_event_cancelled
};

    स्थिर शून्य
clip_wl_do_set_selection(शून्य *data, uint32_t serial)
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
    स्थिर पूर्णाङ्क
clip_wl_own_selection(Clipboard_T *cbd)
{
    clip_wl_selection_T *sel = clip_wl_get_selection_from_cbd(cbd);
    wayland_selection_T sel_type = clip_wl_get_selection_type(sel);

    जइ (!sel->available || vwl_connection_roundtrip(wayland_ct) == FAIL)
	प्रत्यागम FAIL;

    जइ (sel->source != रिक्त)
    {
	जइ (sel_type == WAYLAND_SELECTION_PRIMARY)
	    // We already own the selection, ignore (only do this for primary
	    // selection). We don't re set the selection because then we would
	    // be setting the selection every time the user moves the visual
	    // selection cursor, which is messy and inefficient. Some
	    // applications like Google Chrome do it this way however.
	    प्रत्यागम OK;
	अण्णहा जइ (sel_type == WAYLAND_SELECTION_REGULAR)
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
	अण्णहा
	    // Shouldn't happen
	    प्रत्यागम FAIL;
    }

    sel->source = vwl_data_device_manager_create_data_source(sel->manager);
    vwl_data_source_add_listener(sel->source, &vwl_data_source_listener, sel);

    // Advertise mime types
    प्रति (पूर्णाङ्क i = 0; i < (पूर्णाङ्क)ARRAY_LENGTH(supported_mimes); i++)
	vwl_data_source_offer(sel->source, supported_mimes[i]);

    sel->own_success = false;
    clip_wl_do_set_selection(sel, 0);

    जइ (!sel->own_success)
	गच्छ fail;

    प्रत्यागम OK;
fail:
    vwl_data_source_destroy(sel->source);
    sel->source = रिक्त;
    प्रत्यागम FAIL;
}

/*
 * Disown the selection that cbd corresponds to.
 */
    स्थिर शून्य
clip_wl_lose_selection(Clipboard_T *cbd)
{
    clip_wl_selection_T *sel = clip_wl_get_selection_from_cbd(cbd);

    जइ (!sel->available)
	प्रत्यागम;

    vwl_data_source_destroy(sel->source);
    sel->source = रिक्त;
}

/*
 * Send the current selection to the clipboard. Do nothing for Wayland because
 * we will fill in the selection only when requested by another client.
 */
    स्थिर शून्य
clip_wl_set_selection(Clipboard_T *cbd UNUSED)
{
}

#  जइ defined(USE_SYSTEM)
/*
 * Return true if we own the selection corresponding to cbd or another client
 * does.
 */
    स्थिर bool
clip_wl_owner_exists(Clipboard_T *cbd)
{
    clip_wl_selection_T *sel = clip_wl_get_selection_from_cbd(cbd);

    जइ (vwl_connection_roundtrip(wayland_ct) == FAIL)
	प्रत्यागम false;

    प्रत्यागम sel->available && (sel->source != रिक्त || sel->offer != रिक्त);
}
#  endif

# endif // FEAT_WAYLAND_CLIPBOARD

#endif // FEAT_CLIPBOARD

#ifdef HAVE_CLIPMETHOD

/*
 * Returns the first method for accessing the clipboard that is available/works,
 * depending on the order of values in str.
 */
    स्थिर clipmethod_T
get_clipmethod(char_u *str)
{
    पूर्णाङ्क		len	= (पूर्णाङ्क)STRLEN(str) + 1;
    char_u	*buf	= alloc(len);

    जइ (buf == रिक्त)
	प्रत्यागम CLIPMETHOD_FAIL;

    clipmethod_T ret = CLIPMETHOD_FAIL;
    char_u	*p = str;

    जाव (*p != NUL)
    {
	clipmethod_T method = CLIPMETHOD_NONE;

	(शून्य)copy_option_part(&p, buf, len, ",");

	जइ (STRCMP(buf, "wayland") == 0)
	{
# ifdef FEAT_GUI
	    जइ (!gui.in_use)
# endif
	    {
# ifdef FEAT_WAYLAND_CLIPBOARD
		जइ (clip_wl.regular.available || clip_wl.primary.available)
		    method = CLIPMETHOD_WAYLAND;
# endif
	    }
	}
	अण्णहा जइ (STRCMP(buf, "x11") == 0)
	{
# ifdef FEAT_GUI
	    जइ (!gui.in_use)
# endif
	    {
# ifdef FEAT_XCLIPBOARD
		// x_IOerror_handler() in os_unix.c should set xterm_dpy to NULL if
		// we lost connection to the X server.
		जइ (xterm_dpy != रिक्त)
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
	अण्णहा
	{
# ifdef FEAT_CLIPBOARD_PROVIDER
	    // Check if name matches a clipboard provider
	    पूर्णाङ्क r = clip_provider_is_available(buf);

	    जइ (r == 1)
	    {
		method = CLIPMETHOD_PROVIDER;
		जइ (ret == CLIPMETHOD_FAIL)
		{
		    vim_free(clip_provider);
		    clip_provider = vim_strsave(buf);
		    जइ (clip_provider == रिक्त)
			गच्छ fail;
		}
	    }
	    अण्णहा जइ (r == -1)
# endif
	    {
# ifdef FEAT_CLIPBOARD_PROVIDER
fail:
# endif
		ret = CLIPMETHOD_FAIL;
		गच्छ exit;
	    }
	}

	// Keep on going in order to catch errors
	जइ (method != CLIPMETHOD_NONE && ret == CLIPMETHOD_FAIL)
	    ret = method;
    }

    // No match found, use "none".
    ret = (ret == CLIPMETHOD_FAIL) ? CLIPMETHOD_NONE : ret;

exit:
    vim_free(buf);
    प्रत्यागम ret;
}


/*
 * Returns name of clipmethod in a statically allocated string.
 */
    स्थिर char_u *
clipmethod_to_str(clipmethod_T method)
{
    विकल्प(method)
    {
	पक्ष CLIPMETHOD_WAYLAND:
	    प्रत्यागम (char_u *)"wayland";
	पक्ष CLIPMETHOD_X11:
	    प्रत्यागम (char_u *)"x11";
	पक्ष CLIPMETHOD_PROVIDER:
# ifdef FEAT_EVAL
	    प्रत्यागम clip_provider;
# endif
	स्वभाव:
	    प्रत्यागम (char_u *)"none";
    }
}

/*
 * Sets the current clipmethod to use given by `get_clipmethod()`. Returns an
 * error message on failure else NULL.
 */
    वर्ण *
choose_clipmethod(शून्य)
{
    clipmethod_T method = get_clipmethod(p_cpm);

    जइ (method == CLIPMETHOD_FAIL)
	प्रत्यागम e_invalid_argument;

// If GUI is running or we are not on a system with Wayland or X11, then always
// return CLIPMETHOD_NONE. System or GUI clipboard handling always overrides.
// This is unless a provider is being used.
# जइ defined(FEAT_XCLIPBOARD) || defined(FEAT_WAYLAND_CLIPBOARD)
#  जइ defined(FEAT_GUI)
    जइ (method != CLIPMETHOD_PROVIDER && gui.in_use)
    {
#   ifdef FEAT_WAYLAND
	// We only interact with Wayland for the clipboard, we can just deinit
	// everything.
	wayland_uninit_connection();
#   endif

	method = CLIPMETHOD_NONE;
	गच्छ lose_sel_exit;
    }
#  endif
# अण्णहा
    // If on a system like windows or macos, then clipmethod is irrelevant, we
    // use their way of accessing the clipboard. This is unless we are using the
    // clipboard provider
#  ifdef FEAT_CLIPBOARD_PROVIDER
    जइ (method != CLIPMETHOD_PROVIDER)
#  endif
    {
	method = CLIPMETHOD_NONE;
	गच्छ exit;
    }
# endif

# ifdef FEAT_CLIPBOARD
    // Deinitialize clipboard if there is no way to access clipboard
    जइ (method == CLIPMETHOD_NONE)
	clip_init(FALSE);
    // If we have a clipmethod that works now, then initialize clipboard
    अण्णहा जइ (clipmethod == CLIPMETHOD_NONE && method != CLIPMETHOD_NONE)
    {
	clip_init(TRUE);
	did_warn_clipboard = false;
    }
    // Disown clipboard if we are switching to a new method
    अण्णहा जइ (clipmethod != CLIPMETHOD_NONE && method != clipmethod)
    {
#  जइ (defined(FEAT_XCLIPBOARD) || defined(FEAT_WAYLAND_CLIPBOARD)) \
	&& defined(FEAT_GUI)
lose_sel_exit:
#  endif
	जइ (clip_star.owned)
	    clip_lose_selection(&clip_star);
	जइ (clip_plus.owned)
	    clip_lose_selection(&clip_plus);

#  जइ defined(FEAT_GUI)
	जइ (!gui.in_use)
#  endif
	{
	    clip_init(TRUE);
	    did_warn_clipboard = false;
	}
    }
# endif // FEAT_CLIPBOARD

# जइ !defined(FEAT_XCLIPBOARD) && !defined(FEAT_WAYLAND_CLIPBOARD)
exit:
# endif

    clipmethod = method;

# ifdef FEAT_EVAL
    set_vim_var_string(VV_CLIPMETHOD, (char_u*)clipmethod_to_str(method), -1);
# endif

    प्रत्यागम रिक्त;
}

/*
 * Call choose_clipmethod().
 */
    शून्य
ex_clipreset(exarg_T *eap UNUSED)
{
    clipmethod_T prev = clipmethod;

    choose_clipmethod();

    जइ (clipmethod == CLIPMETHOD_NONE)
	smsg(_("Could not find a way to access the clipboard."));
    अण्णहा जइ (clipmethod != prev)
	smsg(_("Switched to clipboard method '%s'."),
		clipmethod_to_str(clipmethod));
}

/*
 * Adjust the register name pointed to with "rp" for the clipboard being
 * used always and the clipboard being available.
 */
    शून्य
adjust_clip_reg(पूर्णाङ्क *rp)
{
# ifdef FEAT_CLIPBOARD_PROVIDER
    जइ (clipmethod == CLIPMETHOD_PROVIDER)
    {
	जइ (*rp == 0 && clip_unnamed != 0)
	    *rp = ((clip_unnamed & CLIP_UNNAMED_PLUS)) ? '+' : '*';
	प्रत्यागम;
    }
# endif
# ifdef FEAT_CLIPBOARD
    // If no reg. specified, and "unnamed" or "unnamedplus" is in 'clipboard',
    // use '*' or '+' reg, respectively. "unnamedplus" prevails.
    जइ (*rp == 0 && (clip_unnamed != 0 || clip_unnamed_saved != 0))
    {
	जइ (clip_unnamed != 0)
	    *rp = ((clip_unnamed & CLIP_UNNAMED_PLUS) && clip_plus.available)
								  ? '+' : '*';
	अण्णहा
	    *rp = ((clip_unnamed_saved & CLIP_UNNAMED_PLUS)
					   && clip_plus.available) ? '+' : '*';
    }
    जइ ((!clip_star.available && *rp == '*') ||
	   (!clip_plus.available && *rp == '+'))
    {
	सन्देशः_warn_missing_clipboard();
	*rp = 0;
    }
# endif
}

/*
 * Extract the items in the 'clipboard' option and set global values.
 * Return an error message or NULL for success.
 */
    वर्ण *
did_set_clipboard(optset_T *args UNUSED)
{
    पूर्णाङ्क		new_unnamed = 0;
# ifdef FEAT_CLIPBOARD
    पूर्णाङ्क		new_autoselect_star = FALSE;
    पूर्णाङ्क		new_autoselect_plus = FALSE;
    पूर्णाङ्क		new_autoselectml = FALSE;
    पूर्णाङ्क		new_html = FALSE;
# endif
    regprog_T	*new_exclude_prog = रिक्त;
    वर्ण	*errmsg = रिक्त;
    char_u	*p;

    प्रति (p = p_cb; *p != NUL; )
    {
	// Note: Keep this in sync with p_cb_values.
	जइ (STRNCMP(p, "unnamed", 7) == 0 && (p[7] == ',' || p[7] == NUL))
	{
	    new_unnamed |= CLIP_UNNAMED;
	    p += 7;
	}
	अण्णहा जइ (STRNCMP(p, "unnamedplus", 11) == 0
					    && (p[11] == ',' || p[11] == NUL))
	{
	    new_unnamed |= CLIP_UNNAMED_PLUS;
	    p += 11;
	}
# ifdef FEAT_CLIPBOARD
	अण्णहा जइ (STRNCMP(p, "autoselect", 10) == 0
					    && (p[10] == ',' || p[10] == NUL))
	{
	    new_autoselect_star = TRUE;
	    p += 10;
	}
	अण्णहा जइ (STRNCMP(p, "autoselectplus", 14) == 0
					    && (p[14] == ',' || p[14] == NUL))
	{
	    new_autoselect_plus = TRUE;
	    p += 14;
	}
	अण्णहा जइ (STRNCMP(p, "autoselectml", 12) == 0
					    && (p[12] == ',' || p[12] == NUL))
	{
	    new_autoselectml = TRUE;
	    p += 12;
	}
	अण्णहा जइ (STRNCMP(p, "html", 4) == 0 && (p[4] == ',' || p[4] == NUL))
	{
	    new_html = TRUE;
	    p += 4;
	}
	अण्णहा जइ (STRNCMP(p, "exclude:", 8) == 0 && new_exclude_prog == रिक्त)
	{
	    p += 8;
	    new_exclude_prog = vim_regcomp(p, RE_MAGIC);
	    जइ (new_exclude_prog == रिक्त)
		errmsg = e_invalid_argument;
	    भङ्ग;
	}
# endif
	अण्णहा
	{
	    errmsg = e_invalid_argument;
	    भङ्ग;
	}
	जइ (*p == ',')
	    ++p;
    }
    जइ (errmsg == रिक्त)
    {
# ifdef FEAT_CLIPBOARD
	जइ (global_busy)
	    // clip_unnamed will be reset to clip_unnamed_saved
	    // at end_global_changes
	    clip_unnamed_saved = new_unnamed;
	अण्णहा
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
# जइ defined(FEAT_GUI_GTK)
	जइ (gui.in_use)
	{
#  ifdef USE_GTK4
	    gui_gtk_update_selection_formats(&clip_plus);
	    gui_gtk_update_selection_formats(&clip_star);
#  अण्णहा
	    gui_gtk_set_selection_targets((GdkAtom)GDK_SELECTION_PRIMARY);
	    gui_gtk_set_selection_targets((GdkAtom)clip_plus.gtk_sel_atom);
#  endif
#  ifdef FEAT_DND
	    gui_gtk_set_dnd_targets();
#  endif
	}
# endif
    }
    अण्णहा
	vim_regfree(new_exclude_prog);

    प्रत्यागम errmsg;
}

#endif // HAVE_CLIPMETHOD

#ifdef FEAT_CLIPBOARD_PROVIDER

/*
 * Check if a clipboard provider with given name is available. Returns 1 if available,
 * 0 if not available, and -1 on error
 */
    स्थिर पूर्णाङ्क
clip_provider_is_available(char_u *provider)
{
    dict_T	*providers = get_vim_var_dict(VV_CLIPPROVIDERS);
    typval_T	provider_tv = {0};
    callback_T	callback = {0};
    typval_T	rettv = {0};
    typval_T	func_tv = {0};
    पूर्णाङ्क		res = 0;

    जइ (dict_get_tv(providers, (वर्ण *)provider, &provider_tv) == FAIL
	    || provider_tv.v_type != VAR_DICT)
	// clipboard provider not defined
	प्रत्यागम -1;

    जइ (dict_get_tv(provider_tv.vval.v_dict, "available", &func_tv) == FAIL)
    {
	clear_tv(&provider_tv);
	// If "available" function not specified assume always TRUE
	प्रत्यागम 1;
    }

    जइ ((callback = get_callback(&func_tv)).cb_name == रिक्त)
	गच्छ fail;

    जइ (call_callback(&callback, -1, &rettv, 0, रिक्त) == FAIL ||
	    (rettv.v_type != VAR_BOOL && rettv.v_type != VAR_सङ्ख्या))
	गच्छ fail;

    जइ (rettv.vval.v_सङ्ख्या)
	res = 1;

    जइ (FALSE)
fail:
	res = -1;

    free_callback(&callback);
    clear_tv(&func_tv);
    clear_tv(&rettv);
    clear_tv(&provider_tv);

    प्रत्यागम res;
}

/*
 * Get the specified callback "function" from the provider dictionary for
 * register "reg".
 */
    स्थिर पूर्णाङ्क
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

    जइ (dict_get_tv(providers, (वर्ण *)provider, &provider_tv) == FAIL)
	प्रत्यागम FAIL;
    अण्णहा जइ (provider_tv.v_type != VAR_DICT)
    {
	clear_tv(&provider_tv);
	प्रत्यागम FAIL;
    }
    अण्णहा जइ (dict_get_tv(
		provider_tv.vval.v_dict,
		(वर्ण *)function,
		&action_tv) == FAIL)
    {
	clear_tv(&provider_tv);
	प्रत्यागम FAIL;
    }
    अण्णहा जइ (action_tv.v_type != VAR_DICT)
    {
	clear_tv(&provider_tv);
	clear_tv(&action_tv);
	प्रत्यागम FAIL;
    }
    अण्णहा जइ (dict_get_tv(action_tv.vval.v_dict, (वर्ण *)reg, &func_tv) == FAIL)
    {
	clear_tv(&provider_tv);
	clear_tv(&action_tv);
	प्रत्यागम FAIL;
    }
    अण्णहा जइ ((cb = get_callback(&func_tv)).cb_name == रिक्त)
    {
	clear_tv(&provider_tv);
	clear_tv(&action_tv);
	clear_tv(&func_tv);
	प्रत्यागम FAIL;
    }
    clear_tv(&provider_tv);
    clear_tv(&action_tv);

    // func_tv owns the function name, so we must make a copy for the callback
    set_callback(callback, &cb);
    clear_tv(&func_tv);
    प्रत्यागम OK;
}

    स्थिर शून्य
clip_provider_copy(char_u *reg, char_u *provider)
{
    स्थिर bool	recursive = false;
    callback_T	callback;
    typval_T	rettv;
    typval_T	argvars[4];
    yankreg_T	*y_ptr;
    char_u	type[2 + NUMBUFLEN] = {0};
    list_T	*list = रिक्त;

    जइ (recursive)
	प्रत्यागम;

    जइ (clip_provider_get_callback(
		reg,
		provider,
		(char_u *)"copy",
		&callback) == FAIL)
	प्रत्यागम;

    // Convert register type into a string
    जइ (*reg == '+')
	y_ptr = get_y_register(REAL_PLUS_REGISTER);
    अण्णहा
	y_ptr = get_y_register(STAR_REGISTER);

    विकल्प (y_ptr->y_type)
    {
	पक्ष MCHAR:
	    type[0] = 'v';
	    भङ्ग;
	पक्ष MLINE:
	    type[0] = 'V';
	    भङ्ग;
	पक्ष MBLOCK:
	    sprintf((वर्ण *)type, "%c%d", Ctrl_V, y_ptr->y_width + 1);
	    भङ्ग;
	स्वभाव:
	    type[0] = 0;
	    भङ्ग;
    }

    argvars[0].v_type = VAR_STRING;
    argvars[0].vval.v_string = reg;

    argvars[1].v_type = VAR_STRING;
    argvars[1].vval.v_string = type;

    // Get register contents by creating a list of lines
    list = list_alloc();

    जइ (list == रिक्त)
    {
	free_callback(&callback);
	प्रत्यागम;
    }

    प्रति (पूर्णाङ्क i = 0; i < y_ptr->y_size; i++)
	जइ (list_append_string(list, y_ptr->y_array[i].string,
	    (पूर्णाङ्क)y_ptr->y_array[i].length) == FAIL)
	{
	    free_callback(&callback);
	    list_unref(list);
	    प्रत्यागम;
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

    स्थिर शून्य
clip_provider_paste(char_u *reg, char_u *provider)
{
    स्थिर bool	recursive = false;
    callback_T	callback;
    typval_T	argvars[2];
    typval_T	rettv;
    पूर्णाङ्क		ret;
    char_u	*reg_type;
    list_T	*पङ्क्तयः;

    जइ (recursive)
	प्रत्यागम;

    जइ (clip_provider_get_callback(
		reg,
		provider,
		(char_u *)"paste",
		&callback) == FAIL)
	प्रत्यागम;

    argvars[0].v_type = VAR_STRING;
    argvars[0].vval.v_string = reg;

    argvars[1].v_type = VAR_UNKNOWN;

    textlock++;
    recursive = true;
    ret = call_callback(&callback, -1, &rettv, 1, argvars);
    recursive = false;
    textlock--;

    जइ (ret == FAIL)
	गच्छ exit;
    अण्णहा जइ (rettv.v_type == VAR_TUPLE
	    && TUPLE_LEN(rettv.vval.v_tuple) == 2
	    && TUPLE_ITEM(rettv.vval.v_tuple, 0)->v_type == VAR_STRING
	    && TUPLE_ITEM(rettv.vval.v_tuple, 1)->v_type == VAR_LIST)
    {
	reg_type = TUPLE_ITEM(rettv.vval.v_tuple, 0)->vval.v_string;
	पङ्क्तयः = TUPLE_ITEM(rettv.vval.v_tuple, 1)->vval.v_list;
    }
    अण्णहा जइ (rettv.v_type == VAR_LIST
	    && rettv.vval.v_list->lv_len == 2
	    && rettv.vval.v_list->lv_first->li_tv.v_type == VAR_STRING
	    && rettv.vval.v_list->lv_first->li_next->li_tv.v_type == VAR_LIST)
    {
	reg_type = rettv.vval.v_list->lv_first->li_tv.vval.v_string;
	पङ्क्तयः = rettv.vval.v_list->lv_first->li_next->li_tv.vval.v_list;
    }
    अण्णहा
	गच्छ exit;

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
	पूर्णाङ्क		len;

	// If the list is NULL handle like an empty list.
	len = पङ्क्तयः == रिक्त ? 0 : पङ्क्तयः->lv_len;

	// First half: use for pointers to result lines; second half: use for
	// pointers to allocated copies.
	lstval = ALLOC_MULT(char_u *, (len + 1) * 2);
	जइ (lstval == रिक्त)
	    गच्छ exit;
	curval = lstval;
	allocval = lstval + len + 2;
	curallocval = allocval;

	जइ (पङ्क्तयः != रिक्त)
	{
	    CHECK_LIST_MATERIALIZE(पङ्क्तयः);
	    FOR_ALL_LIST_वस्तूनि(पङ्क्तयः, li)
	    {
		strval = tv_get_string_buf_chk(&li->li_tv, buf);
		जइ (strval == रिक्त)
		    गच्छ free_lstval;
		जइ (strval == buf)
		{
		    // Need to make a copy, next tv_get_string_buf_chk() will
		    // overwrite the string.
		    strval = vim_strsave(buf);
		    जइ (strval == रिक्त)
			गच्छ free_lstval;
		    *curallocval++ = strval;
		}
		*curval++ = strval;
	    }
	}
	*curval++ = रिक्त;

	जइ (*reg_type != NUL && (STRLEN(reg_type) == 0
		|| get_yank_type(&reg_type, &yank_type, &block_len) == FAIL))
	{
	    emsg(e_invalid_argument);
	    गच्छ free_lstval;
	}

	जइ (*reg == '+')
	    y_ptr = get_y_register(REAL_PLUS_REGISTER);
	अण्णहा
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
	जाव (curallocval > allocval)
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
स्थिर पूर्णाङ्क star_pause_गणनम् = -2, plus_pause_गणनम् = -2;

    शून्य
call_clip_provider_request(पूर्णाङ्क reg)
{
    जइ (clipmethod != CLIPMETHOD_PROVIDER)
	प्रत्यागम;

    जइ (reg == '+' && plus_pause_गणनम् < 0)
    {
	जइ (plus_pause_गणनम् == -1)
	    plus_pause_गणनम् = 1;
	clip_provider_paste((char_u *)"+", clip_provider);
    }
    अण्णहा जइ (reg == '*' && star_pause_गणनम् < 0)
    {
	जइ (star_pause_गणनम् == -1)
	    star_pause_गणनम् = 1;
	clip_provider_paste((char_u *)"*", clip_provider);
    }
    अण्णहा
	प्रत्यागम;
}

    शून्य
call_clip_provider_set(पूर्णाङ्क reg)
{
    जइ (clipmethod != CLIPMETHOD_PROVIDER)
	प्रत्यागम;

    जइ (reg == '+' && plus_pause_गणनम् < 0)
    {
	जइ (plus_pause_गणनम् == -1)
	    plus_pause_गणनम् = 1;
	clip_provider_copy((char_u *)"+", clip_provider);
    }
    अण्णहा जइ (reg == '*' && star_pause_गणनम् < 0)
    {
	जइ (star_pause_गणनम् == -1)
	    star_pause_गणनम् = 1;
	clip_provider_copy((char_u *)"*", clip_provider);
    }
}

/*
 * Makes it so that the next provider call is only done once any calls after are
 * ignored, until dec_clip_provider is called the same number of times after
 * again. Note that this is per clipboard register ("+", "*")
 */
    शून्य
inc_clip_provider(शून्य)
{
    plus_pause_गणनम् = (plus_pause_गणनम् == -2
	|| plus_pause_गणनम् == -1) ? -1 : plus_pause_गणनम् + 1;
    star_pause_गणनम् = (star_pause_गणनम् == -2
	|| star_pause_गणनम् == -1) ? -1 : star_pause_गणनम् + 1;
}

    शून्य
dec_clip_provider(शून्य)
{
    जइ (plus_pause_गणनम् != -2)
	plus_pause_गणनम् = plus_pause_गणनम् == -1 ? -1 : plus_pause_गणनम् - 1;
    जइ (star_pause_गणनम् != -2)
	star_pause_गणनम् = star_pause_गणनम् == -1 ? -1 : star_pause_गणनम् - 1;

    जइ (plus_pause_गणनम् == 0 || plus_pause_गणनम् == -1)
	plus_pause_गणनम् = -2;
    जइ (star_pause_गणनम् == 0 || star_pause_गणनम् == -1)
	star_pause_गणनम् = -2;
}

#endif // FEAT_CLIPBOARD_PROVIDER

#जइ defined(FEAT_WAYLAND_CLIPBOARD) || (defined(FEAT_GUI_GTK) && defined(USE_GTK4))
/*
 * If "vim" is TRUE, then get the motion type. If "vimenc" is TRUE, then get the
 * motion type and also convert "*buf". "buf" and "len_store" will be updated to
 * reflect the actual contents, but should be set beforehand with the initial
 * contents. Note that if "*tofree" is not NULL, use vim_free() on "*buf",
 * otherwise use the free func for which "*buf" was allocated with. In both
 * cases use vim_free() on "*tofree". Returns OK on success and FAIL on failure.
 */
    पूर्णाङ्क
clip_convert_data(
	char_u	**buf,
	long	*len_store,
	पूर्णाङ्क	*motion,
	bool	vim,
	bool	vimenc,
	char_u	**tofree)
{
    char_u  *final = *buf;
    char_u  *enc;
    long    len = *len_store;

    जइ (vim && len >= 2)
    {
	*motion = *final++;
	len--;
    }
    अण्णहा जइ (vimenc && len >= 3)
    {
	vimconv_T   conv;
	पूर्णाङ्क	    convlen;

	// First byte is motion type
	*motion = *final++;
	len--;

	// Get encoding of selection
	enc = final;

	// Skip the encoding type including null terminator in final text
	final = memchr(final, NUL, len);
	जइ (final == रिक्त)
	    प्रत्यागम FAIL;
	final++; // Skip NUL

	// Subtract pointers to get length of encoding;
	len -= final - enc;

	conv.vc_type = CONV_NONE;
	convert_setup(&conv, enc, p_enc);
	जइ (conv.vc_type != CONV_NONE)
	{
	   char_u *tmp;

	   convlen = len;
	   tmp = string_convert(&conv, final, &convlen);
	   len = convlen;
	   जइ (tmp != रिक्त)
	   {
		final = tmp;
		*tofree = final;
	   }
	   convert_setup(&conv, रिक्त, रिक्त);
	}
    }
    *buf = final;
    *len_store = len;
    प्रत्यागम OK;
}
#endif
