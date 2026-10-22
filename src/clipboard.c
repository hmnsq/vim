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

#include "vim.h"

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
static පූර්ණ_සංඛ්‍යා clip_provider_is_available(char_u *provider);
#endif

#නම් defined(FEAT_CLIPBOARD)

# නම් defined(FEAT_WAYLAND_CLIPBOARD)

#  include "wayland.h"

// Represents either the regular or primary selection
typedef sa_nracanaa {
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
typedef sa_nracanaa {
    vwl_seat_T *seat;

    clip_wl_selection_T regular;
    clip_wl_selection_T primary;
} clip_wl_T;

// Mime types we support sending and receiving
// Mimes with a lower index in the array are prioritized first when we are
// receiving data.
static const var_na *supported_mimes[] = {
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

static void
clip_wl_receive_data(Clipboard_T *cbd, const var_na *mime_type, පූර්ණ_සංඛ්‍යා fd);
static void clip_wl_request_selection(Clipboard_T *cbd);
static පූර්ණ_සංඛ්‍යා clip_wl_own_selection(Clipboard_T *cbd);
static void clip_wl_lose_selection(Clipboard_T *cbd);
static void clip_wl_set_selection(Clipboard_T *cbd);

#  නම් defined(USE_SYSTEM)
static bool clip_wl_owner_exists(Clipboard_T *cbd);
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
    void
clip_init(පූර්ණ_සංඛ්‍යා can_use)
{
    Clipboard_T *cb;

    cb = &clip_star;
    සඳහා (;;)
    {
	// No need to init again if cbd is already available
	නම් (can_use && cb->available)
	    goto skip;

	cb->available  = can_use;
	cb->owned      = FALSE;
	cb->start.lnum = 0;
	cb->start.col  = 0;
	cb->end.lnum   = 0;
	cb->end.col    = 0;
	cb->state      = SELECT_CLEARED;

skip:
	නම් (cb == &clip_plus)
	    bha_nga;
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
    void
clip_update_selection(Clipboard_T *clip)
{
    pos_T	    start, end;

    // If visual mode is only due to a redo command ("."), then ignore it
    නම් (!redo_VIsual_busy && VIsual_active && (State & MODE_NORMAL))
    {
	නම් (LT_POS(VIsual, curwin->w_cursor))
	{
	    start = VIsual;
	    end = curwin->w_cursor;
	    නම් (has_mbyte)
		end.col += (*mb_ptr2len)(ml_get_cursor()) - 1;
	}
	නැතහොත්
	{
	    start = curwin->w_cursor;
	    end = VIsual;
	}
	නම් (!EQUAL_POS(clip->start, start)
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

    static පූර්ණ_සංඛ්‍යා
clip_gen_own_selection(Clipboard_T *cbd)
{
# නම් defined(FEAT_XCLIPBOARD) || defined(FEAT_WAYLAND_CLIPBOARD)
#  ifdef FEAT_GUI
    නම් (gui.in_use)
	ආපසු clip_mch_own_selection(cbd);
    නැතහොත්
#  endif
    {
	නම් (clipmethod == CLIPMETHOD_WAYLAND)
	{
#  ifdef FEAT_WAYLAND_CLIPBOARD
	    ආපසු clip_wl_own_selection(cbd);
#  endif
	}
	නැතහොත් නම් (clipmethod == CLIPMETHOD_X11)
	{
#  ifdef FEAT_XCLIPBOARD
	    ආපසු clip_xterm_own_selection(cbd);
#  endif
	}
    }
    ආපසු FAIL;
# නැතහොත්
    ආපසු clip_mch_own_selection(cbd);
# endif
}

    void
clip_own_selection(Clipboard_T *cbd)
{
    /*
     * Also want to check somehow that we are reading from the keyboard rather
     * than a mapping etc.
     */
# නම් defined(FEAT_X11) || defined(FEAT_WAYLAND_CLIPBOARD)
    // Always own the selection, we might have lost it without being
    // notified, e.g. during a ":sh" command.
    නම් (cbd->available)
    {
	පූර්ණ_සංඛ්‍යා was_owned = cbd->owned;

	cbd->owned = (clip_gen_own_selection(cbd) == OK);
	නම් (!was_owned && (cbd == &clip_star || cbd == &clip_plus))
	{
	    // May have to show a different kind of highlighting for the
	    // selected area.  There is no specific redraw command for this,
	    // just redraw all windows on the current buffer.
	    නම් (cbd->owned
		    && (get_real_state() == MODE_VISUAL
					    || get_real_state() == MODE_SELECT)
		    && (cbd == &clip_star ? clip_isautosel_star()
						      : clip_isautosel_plus())
		    && HL_ATTR(HLF_V) != HL_ATTR(HLF_VNC))
		redraw_curbuf_later(UPD_INVERTED_ALL);
	}
    }
# නැතහොත්
    // Only own the clipboard when we didn't own it yet.
    නම් (!cbd->owned && cbd->available)
	cbd->owned = (clip_gen_own_selection(cbd) == OK);
# endif
}

    static void
clip_gen_lose_selection(Clipboard_T *cbd)
{
# නම් defined(FEAT_XCLIPBOARD) || defined(FEAT_WAYLAND_CLIPBOARD)
#  ifdef FEAT_GUI
    නම් (gui.in_use)
	clip_mch_lose_selection(cbd);
    නැතහොත්
#  endif
    {
	නම් (clipmethod == CLIPMETHOD_WAYLAND)
	{
#  ifdef FEAT_WAYLAND_CLIPBOARD
	    clip_wl_lose_selection(cbd);
#  endif
	}
	නැතහොත් නම් (clipmethod == CLIPMETHOD_X11)
	{
#  ifdef FEAT_XCLIPBOARD
	    clip_xterm_lose_selection(cbd);
#  endif
	}
    }
# නැතහොත්
    clip_mch_lose_selection(cbd);
# endif
}

    void
clip_lose_selection(Clipboard_T *cbd)
{
# ifdef FEAT_X11
    පූර්ණ_සංඛ්‍යා	    was_owned = cbd->owned;
# endif
    පූර්ණ_සංඛ්‍යා     visual_selection = FALSE;

    නම් (cbd == &clip_star || cbd == &clip_plus)
	visual_selection = TRUE;

    clip_free_selection(cbd);
    cbd->owned = FALSE;
    නම් (visual_selection)
	clip_clear_selection(cbd);
    clip_gen_lose_selection(cbd);
# ifdef FEAT_X11
    නම් (visual_selection)
    {
	// May have to show a different kind of highlighting for the selected
	// area.  There is no specific redraw command for this, just redraw all
	// windows on the current buffer.
	නම් (was_owned
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

    static void
clip_copy_selection(Clipboard_T *clip)
{
    නම් (VIsual_active && (State & MODE_NORMAL) && clip->available)
    {
	clip_update_selection(clip);
	clip_free_selection(clip);
	clip_own_selection(clip);
	නම් (clip->owned)
	    clip_get_selection(clip);
	clip_gen_set_selection(clip);
    }
}

/*
 * Save and restore clip_unnamed before doing possibly many changes. This
 * prevents accessing the clipboard very often which might slow down Vim
 * considerably.
 */
static පූර්ණ_සංඛ්‍යා global_change_count = 0; // if set, inside a start_global_changes
static පූර්ණ_සංඛ්‍යා clipboard_needs_update = FALSE; // clipboard needs to be updated
static පූර්ණ_සංඛ්‍යා clip_did_set_selection = TRUE;

/*
 * Save clip_unnamed and reset it.
 */
    void
start_global_changes(void)
{
    නම් (++global_change_count > 1)
	ආපසු;
    clip_unnamed_saved = clip_unnamed;
    clipboard_needs_update = FALSE;

    නම් (clip_did_set_selection)
    {
	clip_unnamed = 0;
	clip_did_set_selection = FALSE;
    }
}

/*
 * Return TRUE if setting the clipboard was postponed, it already contains the
 * right text.
 */
    static පූර්ණ_සංඛ්‍යා
is_clipboard_needs_update(void)
{
    ආපසු clipboard_needs_update;
}

/*
 * Restore clip_unnamed and set the selection when needed.
 */
    void
end_global_changes(void)
{
    නම් (--global_change_count > 0)
	// recursive
	ආපසු;
    නම් (!clip_did_set_selection)
    {
	clip_did_set_selection = TRUE;
	clip_unnamed = clip_unnamed_saved;
	clip_unnamed_saved = 0;
	නම් (clipboard_needs_update)
	{
	    // only store something in the clipboard,
	    // if we have yanked anything to it
	    නම් (clip_unnamed & CLIP_UNNAMED)
	    {
		clip_own_selection(&clip_star);
		clip_gen_set_selection(&clip_star);
	    }
	    නම් (clip_unnamed & CLIP_UNNAMED_PLUS)
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
    void
clip_auto_select(void)
{
    නම් (clip_isautosel_star())
	clip_copy_selection(&clip_star);
    නම් (clip_isautosel_plus())
	clip_copy_selection(&clip_plus);
}

/*
 * Return TRUE if automatic selection of Visual area is desired for the *
 * register.
 */
    පූර්ණ_සංඛ්‍යා
clip_isautosel_star(void)
{
# ifdef FEAT_CLIPBOARD_PROVIDER
    නම් (clipmethod == CLIPMETHOD_PROVIDER)
	ආපසු false;
# endif
# ifdef FEAT_GUI
    නම් (gui.in_use)
	ආපසු vim_strchr(p_go, GO_ASEL) != NULL
	    && vim_strchr(p_go, GO_ASELPLUS) == NULL;
# endif
    ආපසු clip_autoselect_star;
}

/*
 * Return TRUE if automatic selection of Visual area is desired for the +
 * register.
 */
    පූර්ණ_සංඛ්‍යා
clip_isautosel_plus(void)
{
# ifdef FEAT_CLIPBOARD_PROVIDER
    නම් (clipmethod == CLIPMETHOD_PROVIDER)
	ආපසු false;
# endif
# ifdef FEAT_GUI
    නම් (gui.in_use)
	ආපසු vim_strchr(p_go, GO_ASELPLUS) != NULL;
# endif
    ආපසු clip_autoselect_plus;
}


/*
 * Stuff for general mouse selection, without using Visual mode.
 */

/*
 * Compare two screen positions ala strcmp()
 */
    static පූර්ණ_සංඛ්‍යා
clip_compare_pos(
    පූර්ණ_සංඛ්‍යා		row1,
    පූර්ණ_සංඛ්‍යා		col1,
    පූර්ණ_සංඛ්‍යා		row2,
    පූර්ණ_සංඛ්‍යා		col2)
{
    නම් (row1 > row2) ආපසු(1);
    නම් (row1 < row2) ආපසු(-1);
    නම් (col1 > col2) ආපසු(1);
    නම් (col1 < col2) ආපසු(-1);
    ආපසු(0);
}

// "how" flags for clip_invert_area()
# define CLIP_CLEAR	1
# define CLIP_SET	2
# define CLIP_TOGGLE	3

/*
 * Invert or un-invert a rectangle of the screen.
 * "invert" is true if the result is inverted.
 */
    static void
clip_invert_rectangle(
	Clipboard_T	*cbd UNUSED,
	පූර්ණ_සංඛ්‍යා		row_arg,
	පූර්ණ_සංඛ්‍යා		col_arg,
	පූර්ණ_සංඛ්‍යා		height_arg,
	පූර්ණ_සංඛ්‍යා		width_arg,
	පූර්ණ_සංඛ්‍යා		invert)
{
    පූර්ණ_සංඛ්‍යා		row = row_arg;
    පූර්ණ_සංඛ්‍යා		col = col_arg;
    පූර්ණ_සංඛ්‍යා		height = height_arg;
    පූර්ණ_සංඛ්‍යා		width = width_arg;

# ifdef FEAT_PROP_POPUP
    // this goes on top of all popup windows
    screen_zindex = CLIP_ZINDEX;

    නම් (col < cbd->min_col)
    {
	width -= cbd->min_col - col;
	col = cbd->min_col;
    }
    නම් (width > cbd->max_col - col)
	width = cbd->max_col - col;
    නම් (row < cbd->min_row)
    {
	height -= cbd->min_row - row;
	row = cbd->min_row;
    }
    නම් (height > cbd->max_row - row + 1)
	height = cbd->max_row - row + 1;
# endif
# ifdef FEAT_GUI
    නම් (gui.in_use)
	gui_mch_invert_rectangle(row, col, height, width);
    නැතහොත්
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
    static void
clip_invert_area(
	Clipboard_T	*cbd,
	පූර්ණ_සංඛ්‍යා		row1,
	පූර්ණ_සංඛ්‍යා		col1,
	පූර්ණ_සංඛ්‍යා		row2,
	පූර්ණ_සංඛ්‍යා		col2,
	පූර්ණ_සංඛ්‍යා		how)
{
    පූර්ණ_සංඛ්‍යා		invert = FALSE;
    පූර්ණ_සංඛ්‍යා		max_col;

# ifdef FEAT_PROP_POPUP
    max_col = cbd->max_col - 1;
# නැතහොත්
    max_col = Columns - 1;
# endif

    නම් (how == CLIP_SET)
	invert = TRUE;

    // Swap the from and to positions so the from is always before
    නම් (clip_compare_pos(row1, col1, row2, col2) > 0)
    {
	පූර්ණ_සංඛ්‍යා tmp_row, tmp_col;

	tmp_row = row1;
	tmp_col = col1;
	row1	= row2;
	col1	= col2;
	row2	= tmp_row;
	col2	= tmp_col;
    }
    නැතහොත් නම් (how == CLIP_TOGGLE)
	invert = TRUE;

    // If all on the same line, do it the easy way
    නම් (row1 == row2)
    {
	clip_invert_rectangle(cbd, row1, col1, 1, col2 - col1, invert);
    }
    නැතහොත්
    {
	// Handle a piece of the first line
	නම් (col1 > 0)
	{
	    clip_invert_rectangle(cbd, row1, col1, 1,
						  (පූර්ණ_සංඛ්‍යා)Columns - col1, invert);
	    row1++;
	}

	// Handle a piece of the last line
	නම් (col2 < max_col)
	{
	    clip_invert_rectangle(cbd, row2, 0, 1, col2, invert);
	    row2--;
	}

	// Handle the rectangle that's left
	නම් (row2 >= row1)
	    clip_invert_rectangle(cbd, row1, 0, row2 - row1 + 1,
							 (පූර්ණ_සංඛ්‍යා)Columns, invert);
    }
}

/*
 * Start, continue or end a modeless selection.  Used when editing the
 * command-line, in the cmdline window and when the mouse is in a popup window.
 */
    void
clip_modeless(පූර්ණ_සංඛ්‍යා button, පූර්ණ_සංඛ්‍යා is_click, පූර්ණ_සංඛ්‍යා is_drag)
{
    පූර්ණ_සංඛ්‍යා		repeat;

    repeat = ((clip_star.mode == SELECT_MODE_CHAR
		|| clip_star.mode == SELECT_MODE_LINE)
					      && (mod_mask & MOD_MASK_2CLICK))
	    || (clip_star.mode == SELECT_MODE_WORD
					     && (mod_mask & MOD_MASK_3CLICK));
    නම් (is_click && button == MOUSE_RIGHT)
    {
	// Right mouse button: If there was no selection, start one.
	// Otherwise extend the existing selection.
	නම් (clip_star.state == SELECT_CLEARED)
	    clip_start_selection(mouse_col, mouse_row, FALSE);
	clip_process_selection(button, mouse_col, mouse_row, repeat);
    }
    නැතහොත් නම් (is_click)
	clip_start_selection(mouse_col, mouse_row, repeat);
    නැතහොත් නම් (is_drag)
    {
	// Don't try extending a selection if there isn't one.  Happens when
	// button-down is in the cmdline and them moving mouse upwards.
	නම් (clip_star.state != SELECT_CLEARED)
	    clip_process_selection(button, mouse_col, mouse_row, repeat);
    }
    නැතහොත් // release
	clip_process_selection(MOUSE_RELEASE, mouse_col, mouse_row, FALSE);
}

/*
 * Update the currently selected region by adding and/or subtracting from the
 * beginning or end and inverting the changed area(s).
 */
    static void
clip_update_modeless_selection(
    Clipboard_T    *cb,
    පූර්ණ_සංඛ්‍යා		    row1,
    පූර්ණ_සංඛ්‍යා		    col1,
    පූර්ණ_සංඛ්‍යා		    row2,
    පූර්ණ_සංඛ්‍යා		    col2)
{
    // See if we changed at the beginning of the selection
    නම් (row1 != cb->start.lnum || col1 != (පූර්ණ_සංඛ්‍යා)cb->start.col)
    {
	clip_invert_area(cb, row1, col1, (පූර්ණ_සංඛ්‍යා)cb->start.lnum, cb->start.col,
								 CLIP_TOGGLE);
	cb->start.lnum = row1;
	cb->start.col  = col1;
    }

    // See if we changed at the end of the selection
    නම් (row2 != cb->end.lnum || col2 != (පූර්ණ_සංඛ්‍යා)cb->end.col)
    {
	clip_invert_area(cb, (පූර්ණ_සංඛ්‍යා)cb->end.lnum, cb->end.col, row2, col2,
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

    static void
clip_get_word_boundaries(Clipboard_T *cb, පූර්ණ_සංඛ්‍යා row, පූර්ණ_සංඛ්‍යා col)
{
    පූර්ණ_සංඛ්‍යා		start_class;
    පූර්ණ_සංඛ්‍යා		temp_col;
    char_u	*p;
    පූර්ණ_සංඛ්‍යා		mboff;

    නම් (row >= screen_Rows || col >= screen_Columns || ScreenLines == NULL)
	ආපසු;

    p = ScreenLines + LineOffset[row];
    // Correct for starting in the right half of a double-wide char
    නම් (enc_dbcs != 0)
	col -= dbcs_screen_head_off(p, p + col);
    නැතහොත් නම් (enc_utf8 && p[col] == 0)
	--col;
    start_class = CHAR_CLASS(p[col]);

    temp_col = col;
    සඳහා ( ; temp_col > 0; temp_col--)
	නම් (enc_dbcs != 0
		   && (mboff = dbcs_screen_head_off(p, p + temp_col - 1)) > 0)
	    temp_col -= mboff;
	නැතහොත් නම් (CHAR_CLASS(p[temp_col - 1]) != start_class
		&& !(enc_utf8 && p[temp_col - 1] == 0))
	    bha_nga;
    cb->word_start_col = temp_col;

    temp_col = col;
    සඳහා ( ; temp_col < screen_Columns; temp_col++)
	නම් (enc_dbcs != 0 && dbcs_ptr2cells(p + temp_col) == 2)
	    ++temp_col;
	නැතහොත් නම් (CHAR_CLASS(p[temp_col]) != start_class
		&& !(enc_utf8 && p[temp_col] == 0))
	    bha_nga;
    cb->word_end_col = temp_col;
}

/*
 * Find the column position for the last non-whitespace character on the given
 * line at or before start_col.
 */
    static පූර්ණ_සංඛ්‍යා
clip_get_line_end(Clipboard_T *cbd UNUSED, පූර්ණ_සංඛ්‍යා row)
{
    පූර්ණ_සංඛ්‍යා	    i;

    නම් (row >= screen_Rows || ScreenLines == NULL)
	ආපසු 0;
    සඳහා (i =
# ifdef FEAT_PROP_POPUP
	    cbd->max_col;
# නැතහොත්
	    screen_Columns;
# endif
			    i > 0; i--)
	නම් (ScreenLines[LineOffset[row] + i - 1] != ' ')
	    bha_nga;
    ආපසු i;
}

/*
 * Start the selection
 */
    void
clip_start_selection(පූර්ණ_සංඛ්‍යා col, පූර්ණ_සංඛ්‍යා row, පූර්ණ_සංඛ්‍යා repeated_click)
{
    Clipboard_T	*cb = &clip_star;
# ifdef FEAT_PROP_POPUP
    win_T	*wp;
    පූර්ණ_සංඛ්‍යා		row_cp = row;
    පූර්ණ_සංඛ්‍යා		col_cp = col;

    wp = mouse_find_win(&row_cp, &col_cp, FIND_POPUP);
    නම් (wp != NULL && WIN_IS_POPUP(wp)
				  && popup_is_in_scrollbar(wp, row_cp, col_cp))
	// click or double click in scrollbar does not start a selection
	ආපසු;
# endif

    නම් (cb->state == SELECT_DONE)
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
    නම් (wp != NULL && WIN_IS_POPUP(wp))
    {
	// Click in a popup window restricts selection to that window,
	// excluding the border.
	cb->min_col = wp->w_wincol + wp->w_popup_border[3];
	cb->max_col = wp->w_wincol + popup_width(wp)
				 - wp->w_popup_border[1] - wp->w_has_scrollbar;
	නම් (cb->max_col > screen_Columns)
	    cb->max_col = screen_Columns;
	cb->min_row = wp->w_winrow + wp->w_popup_border[0];
	cb->max_row = wp->w_winrow + popup_height(wp) - 1
						   - wp->w_popup_border[2];
    }
    නැතහොත්
    {
	cb->min_col = 0;
	cb->max_col = screen_Columns;
	cb->min_row = 0;
	cb->max_row = screen_Rows;
    }
# endif

    නම් (repeated_click)
    {
	නම් (++cb->mode > SELECT_MODE_LINE)
	    cb->mode = SELECT_MODE_CHAR;
    }
    නැතහොත්
	cb->mode = SELECT_MODE_CHAR;

# ifdef FEAT_GUI
    // clear the cursor until the selection is made
    නම් (gui.in_use)
	gui_undraw_cursor();
# endif

    switch (cb->mode)
    {
	pak_sa SELECT_MODE_CHAR:
	    cb->origin_start_col = cb->start.col;
	    cb->word_end_col = clip_get_line_end(cb, (පූර්ණ_සංඛ්‍යා)cb->start.lnum);
	    bha_nga;

	pak_sa SELECT_MODE_WORD:
	    clip_get_word_boundaries(cb, (පූර්ණ_සංඛ්‍යා)cb->start.lnum, cb->start.col);
	    cb->origin_start_col = cb->word_start_col;
	    cb->origin_end_col	 = cb->word_end_col;

	    clip_invert_area(cb, (පූර්ණ_සංඛ්‍යා)cb->start.lnum, cb->word_start_col,
			    (පූර්ණ_සංඛ්‍යා)cb->end.lnum, cb->word_end_col, CLIP_SET);
	    cb->start.col = cb->word_start_col;
	    cb->end.col   = cb->word_end_col;
	    bha_nga;

	pak_sa SELECT_MODE_LINE:
	    clip_invert_area(cb, (පූර්ණ_සංඛ්‍යා)cb->start.lnum, 0, (පූර්ණ_සංඛ්‍යා)cb->start.lnum,
			    (පූර්ණ_සංඛ්‍යා)Columns, CLIP_SET);
	    cb->start.col = 0;
	    cb->end.col   = Columns;
	    bha_nga;
    }

    cb->prev = cb->start;

# ifdef DEBUG_SELECTION
    printf("Selection started at (%ld,%d)\n", cb->start.lnum, cb->start.col);
# endif
}

/*
 * Continue processing the selection
 */
    void
clip_process_selection(
    පූර්ණ_සංඛ්‍යා		button,
    පූර්ණ_සංඛ්‍යා		col,
    පූර්ණ_සංඛ්‍යා		row,
    int_u	repeated_click)
{
    Clipboard_T	*cb = &clip_star;
    පූර්ණ_සංඛ්‍යා		diff;
    පූර්ණ_සංඛ්‍යා		slen = 1;	// cursor shape width

    නම් (button == MOUSE_RELEASE)
    {
	නම් (cb->state != SELECT_IN_PROGRESS)
	    ආපසු;

	// Check to make sure we have something selected
	නම් (cb->start.lnum == cb->end.lnum && cb->start.col == cb->end.col)
	{
# ifdef FEAT_GUI
	    නම් (gui.in_use)
		gui_update_cursor(FALSE, FALSE);
# endif
	    cb->state = SELECT_CLEARED;
	    ආපසු;
	}

# ifdef DEBUG_SELECTION
	printf("Selection ended: (%ld,%d) to (%ld,%d)\n", cb->start.lnum,
		cb->start.col, cb->end.lnum, cb->end.col);
# endif
	නම් (clip_isautosel_star() || clip_isautosel_plus()
		|| (
# ifdef FEAT_GUI
		    gui.in_use ? (vim_strchr(p_go, GO_ASELML) != NULL) :
# endif
		    clip_autoselectml))
	    clip_copy_modeless_selection(FALSE);
# ifdef FEAT_GUI
	නම් (gui.in_use)
	    gui_update_cursor(FALSE, FALSE);
# endif

	cb->state = SELECT_DONE;
	ආපසු;
    }

    row = check_row(row);
    col = check_col(col);
    col = mb_fix_col(col, row);

    නම් (col == (පූර්ණ_සංඛ්‍යා)cb->prev.col && row == cb->prev.lnum && !repeated_click)
	ආපසු;

    /*
     * When extending the selection with the right mouse button, swap the
     * start and end if the position is before half the selection
     */
    නම් (cb->state == SELECT_DONE && button == MOUSE_RIGHT)
    {
	/*
	 * If the click is before the start, or the click is inside the
	 * selection and the start is the closest side, set the origin to the
	 * end of the selection.
	 */
	නම් (clip_compare_pos(row, col, (පූර්ණ_සංඛ්‍යා)cb->start.lnum, cb->start.col) < 0
		|| (clip_compare_pos(row, col,
					   (පූර්ණ_සංඛ්‍යා)cb->end.lnum, cb->end.col) < 0
		    && (((cb->start.lnum == cb->end.lnum
			    && cb->end.col - col > col - cb->start.col))
			|| ((diff = (cb->end.lnum - row) -
						   (row - cb->start.lnum)) > 0
			    || (diff == 0 && col < (පූර්ණ_සංඛ්‍යා)(cb->start.col +
							 cb->end.col) / 2)))))
	{
	    cb->origin_row = (short_u)cb->end.lnum;
	    cb->origin_start_col = cb->end.col - 1;
	    cb->origin_end_col = cb->end.col;
	}
	නැතහොත්
	{
	    cb->origin_row = (short_u)cb->start.lnum;
	    cb->origin_start_col = cb->start.col;
	    cb->origin_end_col = cb->start.col;
	}
	නම් (cb->mode == SELECT_MODE_WORD && !repeated_click)
	    cb->mode = SELECT_MODE_CHAR;
    }

    // set state, for when using the right mouse button
    cb->state = SELECT_IN_PROGRESS;

# ifdef DEBUG_SELECTION
    printf("Selection extending to (%d,%d)\n", row, col);
# endif

    නම් (repeated_click && ++cb->mode > SELECT_MODE_LINE)
	cb->mode = SELECT_MODE_CHAR;

    switch (cb->mode)
    {
	pak_sa SELECT_MODE_CHAR:
	    // If we're on a different line, find where the line ends
	    නම් (row != cb->prev.lnum)
		cb->word_end_col = clip_get_line_end(cb, row);

	    // See if we are before or after the origin of the selection
	    නම් (clip_compare_pos(row, col, cb->origin_row,
						   cb->origin_start_col) >= 0)
	    {
		නම් (col >= (පූර්ණ_සංඛ්‍යා)cb->word_end_col)
		    clip_update_modeless_selection(cb, cb->origin_row,
			    cb->origin_start_col, row, (පූර්ණ_සංඛ්‍යා)Columns);
		නැතහොත්
		{
		    නම් (has_mbyte && mb_lefthalve(row, col))
			slen = 2;
		    clip_update_modeless_selection(cb, cb->origin_row,
			    cb->origin_start_col, row, col + slen);
		}
	    }
	    නැතහොත්
	    {
		නම් (has_mbyte
			&& mb_lefthalve(cb->origin_row, cb->origin_start_col))
		    slen = 2;
		නම් (col >= (පූර්ණ_සංඛ්‍යා)cb->word_end_col)
		    clip_update_modeless_selection(cb, row, cb->word_end_col,
			    cb->origin_row, cb->origin_start_col + slen);
		නැතහොත්
		    clip_update_modeless_selection(cb, row, col,
			    cb->origin_row, cb->origin_start_col + slen);
	    }
	    bha_nga;

	pak_sa SELECT_MODE_WORD:
	    // If we are still within the same word, do nothing
	    නම් (row == cb->prev.lnum && col >= (පූර්ණ_සංඛ්‍යා)cb->word_start_col
		    && col < (පූර්ණ_සංඛ්‍යා)cb->word_end_col && !repeated_click)
		ආපසු;

	    // Get new word boundaries
	    clip_get_word_boundaries(cb, row, col);

	    // Handle being after the origin point of selection
	    නම් (clip_compare_pos(row, col, cb->origin_row,
		    cb->origin_start_col) >= 0)
		clip_update_modeless_selection(cb, cb->origin_row,
			cb->origin_start_col, row, cb->word_end_col);
	    නැතහොත්
		clip_update_modeless_selection(cb, row, cb->word_start_col,
			cb->origin_row, cb->origin_end_col);
	    bha_nga;

	pak_sa SELECT_MODE_LINE:
	    නම් (row == cb->prev.lnum && !repeated_click)
		ආපසු;

	    නම් (clip_compare_pos(row, col, cb->origin_row,
		    cb->origin_start_col) >= 0)
		clip_update_modeless_selection(cb, cb->origin_row, 0, row,
			(පූර්ණ_සංඛ්‍යා)Columns);
	    නැතහොත්
		clip_update_modeless_selection(cb, row, 0, cb->origin_row,
			(පූර්ණ_සංඛ්‍යා)Columns);
	    bha_nga;
    }

    cb->prev.lnum = row;
    cb->prev.col  = col;

# ifdef DEBUG_SELECTION
	printf("Selection is: (%ld,%d) to (%ld,%d)\n", cb->start.lnum,
		cb->start.col, cb->end.lnum, cb->end.col);
# endif
}

# නම් defined(FEAT_GUI)
/*
 * Redraw part of the selection if character at "row,col" is inside of it.
 * Only used for the GUI.
 */
    void
clip_may_redraw_selection(පූර්ණ_සංඛ්‍යා row, පූර්ණ_සංඛ්‍යා col, පූර්ණ_සංඛ්‍යා len)
{
    පූර්ණ_සංඛ්‍යා		start = col;
    පූර්ණ_සංඛ්‍යා		end = col + len;

    නම් (clip_star.state != SELECT_CLEARED
	    && row >= clip_star.start.lnum
	    && row <= clip_star.end.lnum)
    {
	නම් (row == clip_star.start.lnum && start < (පූර්ණ_සංඛ්‍යා)clip_star.start.col)
	    start = clip_star.start.col;
	නම් (row == clip_star.end.lnum && end > (පූර්ණ_සංඛ්‍යා)clip_star.end.col)
	    end = clip_star.end.col;
	නම් (end > start)
	    clip_invert_area(&clip_star, row, start, row, end, 0);
    }
}
# endif

/*
 * Called from outside to clear selected region from the display
 */
    void
clip_clear_selection(Clipboard_T *cbd)
{

    නම් (cbd->state == SELECT_CLEARED)
	ආපසු;

    clip_invert_area(cbd, (පූර්ණ_සංඛ්‍යා)cbd->start.lnum, cbd->start.col,
				 (පූර්ණ_සංඛ්‍යා)cbd->end.lnum, cbd->end.col, CLIP_CLEAR);
    cbd->state = SELECT_CLEARED;
}

/*
 * Clear the selection if any lines from "row1" to "row2" are inside of it.
 */
    void
clip_may_clear_selection(පූර්ණ_සංඛ්‍යා row1, පූර්ණ_සංඛ්‍යා row2)
{
    නම් (clip_star.state == SELECT_DONE
	    && row2 >= clip_star.start.lnum
	    && row1 <= clip_star.end.lnum)
	clip_clear_selection(&clip_star);
}

/*
 * Called before the screen is scrolled up or down.  Adjusts the line numbers
 * of the selection.  Call with big number when clearing the screen.
 */
    void
clip_scroll_selection(
    පූර්ණ_සංඛ්‍යා	    rows)		// negative for scroll down
{
    පූර්ණ_සංඛ්‍යා	    lnum;

    නම් (clip_star.state == SELECT_CLEARED)
	ආපසු;

    lnum = clip_star.start.lnum - rows;
    නම් (lnum <= 0)
	clip_star.start.lnum = 0;
    නැතහොත් නම් (lnum >= screen_Rows)	// scrolled off of the screen
	clip_star.state = SELECT_CLEARED;
    නැතහොත්
	clip_star.start.lnum = lnum;

    lnum = clip_star.end.lnum - rows;
    නම් (lnum < 0)			// scrolled off of the screen
	clip_star.state = SELECT_CLEARED;
    නැතහොත් නම් (lnum >= screen_Rows)
	clip_star.end.lnum = screen_Rows - 1;
    නැතහොත්
	clip_star.end.lnum = lnum;
}

/*
 * Copy the currently selected area into the '*' or '+' register so it will be
 * available for pasting.
 * When "both" is TRUE also copy to the other register.
 */
    void
clip_copy_modeless_selection(පූර්ණ_සංඛ්‍යා both UNUSED)
{
    // The info for the modeless selection is stored in '*' register, however if
    // we are using the '+' register for modeless autoselect, we copy to
    // clip_plus instead while using the info in clip_star.
    Clipboard_T *cbd = clip_isautosel_plus() ? &clip_plus : &clip_star;
    char_u	*buffer;
    char_u	*bufp;
    පූර්ණ_සංඛ්‍යා		row;
    පූර්ණ_සංඛ්‍යා		start_col;
    පූර්ණ_සංඛ්‍යා		end_col;
    පූර්ණ_සංඛ්‍යා		line_end_col;
    පූර්ණ_සංඛ්‍යා		add_newline_flag = FALSE;
    පූර්ණ_සංඛ්‍යා		len;
    char_u	*p;
    පූර්ණ_සංඛ්‍යා		row1 = clip_star.start.lnum;
    පූර්ණ_සංඛ්‍යා		col1 = clip_star.start.col;
    පූර්ණ_සංඛ්‍යා		row2 = clip_star.end.lnum;
    පූර්ණ_සංඛ්‍යා		col2 = clip_star.end.col;

    // Can't use ScreenLines unless initialized
    නම් (ScreenLines == NULL)
	ආපසු;

    /*
     * Make sure row1 <= row2, and if row1 == row2 that col1 <= col2.
     */
    නම් (row1 > row2)
    {
	row = row1; row1 = row2; row2 = row;
	row = col1; col1 = col2; col2 = row;
    }
    නැතහොත් නම් (row1 == row2 && col1 > col2)
    {
	row = col1; col1 = col2; col2 = row;
    }
# ifdef FEAT_PROP_POPUP
    නම් (col1 < clip_star.min_col)
	col1 = clip_star.min_col;
    නම් (col2 > clip_star.max_col)
	col2 = clip_star.max_col;
    නම් (row1 > clip_star.max_row || row2 < clip_star.min_row)
	ආපසු;
    නම් (row1 < clip_star.min_row)
	row1 = clip_star.min_row;
    නම් (row2 > clip_star.max_row)
	row2 = clip_star.max_row;
# endif
    // correct starting point for being on right half of double-wide char
    p = ScreenLines + LineOffset[row1];
    නම් (enc_dbcs != 0)
	col1 -= (*mb_head_off)(p, p + col1);
    නැතහොත් නම් (enc_utf8 && p[col1] == 0)
	--col1;

    // Create a temporary buffer for storing the text
    len = (row2 - row1 + 1) * Columns + 1;
    නම් (enc_dbcs != 0)
	len *= 2;	// max. 2 bytes per display cell
    නැතහොත් නම් (enc_utf8)
	len *= MB_MAXBYTES;
    // A newline is added after each row
    len += row2 - row1 + 2;
    buffer = alloc(len);
    නම් (buffer == NULL)	    // out of memory
	ආපසු;

    // Process each row in the selection
    සඳහා (bufp = buffer, row = row1; row <= row2; row++)
    {
	නම් (row == row1)
	    start_col = col1;
	නැතහොත්
# ifdef FEAT_PROP_POPUP
	    start_col = clip_star.min_col;
# නැතහොත්
	    start_col = 0;
# endif

	නම් (row == row2)
	    end_col = col2;
	නැතහොත්
# ifdef FEAT_PROP_POPUP
	    end_col = clip_star.max_col;
# නැතහොත්
	    end_col = Columns;
# endif

	line_end_col = clip_get_line_end(&clip_star, row);

	// See if we need to nuke some trailing whitespace
	නම් (end_col >=
# ifdef FEAT_PROP_POPUP
		clip_star.max_col
# නැතහොත්
		Columns
# endif
		    && (row < row2 || end_col > line_end_col))
	{
	    // Get rid of trailing whitespace
	    end_col = line_end_col;
	    නම් (end_col < start_col)
		end_col = start_col;

	    // If the last line extended to the end, add an extra newline
	    නම් (row == row2)
		add_newline_flag = TRUE;
	}

	// If after the first row, we need to always add a newline
	නම් (row > row1 && !LineWraps[row - 1])
	    *bufp++ = NL;

	// Safetey check for in case resizing went wrong
	නම් (row < screen_Rows && end_col <= screen_Columns)
	{
	    නම් (enc_dbcs != 0)
	    {
		පූර්ණ_සංඛ්‍යා	i;

		p = ScreenLines + LineOffset[row];
		සඳහා (i = start_col; i < end_col; ++i)
		    නම් (enc_dbcs == DBCS_JPNU && p[i] == 0x8e)
		    {
			// single-width double-byte char
			*bufp++ = 0x8e;
			*bufp++ = ScreenLines2[LineOffset[row] + i];
		    }
		    නැතහොත්
		    {
			*bufp++ = p[i];
			නම් (MB_BYTE2LEN(p[i]) == 2)
			    *bufp++ = p[++i];
		    }
	    }
	    නැතහොත් නම් (enc_utf8)
	    {
		පූර්ණ_සංඛ්‍යා	off;
		පූර්ණ_සංඛ්‍යා	i;

		off = LineOffset[row];
		සඳහා (i = start_col; i < end_col; ++i)
		{
		    // The base character is either in ScreenLinesUC[] or
		    // ScreenLines[].
		    නම් (ScreenLinesUC[off + i] == 0)
			*bufp++ = ScreenLines[off + i];
		    නැතහොත්
			// Includes the composing characters.
			bufp += utfc_char2bytes(off + i, bufp);
		    // Skip right half of double-wide character.
		    නම් (ScreenLines[off + i + 1] == 0)
			++i;
		}
	    }
	    නැතහොත්
	    {
		STRNCPY(bufp, ScreenLines + LineOffset[row] + start_col,
							 end_col - start_col);
		bufp += end_col - start_col;
	    }
	}
    }

    // Add a newline at the end if the selection ended there
    නම් (add_newline_flag)
	*bufp++ = NL;

    // First cleanup any old selection and become the owner.
    clip_free_selection(cbd);
    clip_own_selection(cbd);

    // Yank the text into the '*' register.
    clip_yank_selection(MCHAR, buffer, (long)(bufp - buffer), cbd);

    // Make the register contents available to the outside world.
    clip_gen_set_selection(cbd);

# ifdef FEAT_X11
    නම් (both)
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

    void
clip_gen_set_selection(Clipboard_T *cbd)
{
    නම් (!clip_did_set_selection)
    {
	// Updating postponed, so that accessing the system clipboard won't
	// hang Vim when accessing it many times (e.g. on a :g command).
	නම් ((cbd == &clip_plus && (clip_unnamed_saved & CLIP_UNNAMED_PLUS))
		|| (cbd == &clip_star && (clip_unnamed_saved & CLIP_UNNAMED)))
	{
	    clipboard_needs_update = TRUE;
	    ආපසු;
	}
    }
# නම් defined(FEAT_XCLIPBOARD) || defined(FEAT_WAYLAND_CLIPBOARD)
#  ifdef FEAT_GUI
    නම් (gui.in_use)
	clip_mch_set_selection(cbd);
    නැතහොත්
#  endif
    {
	නම් (clipmethod == CLIPMETHOD_WAYLAND)
	{
#  ifdef FEAT_WAYLAND_CLIPBOARD
	    clip_wl_set_selection(cbd);
#  endif
	}
	නැතහොත් නම් (clipmethod == CLIPMETHOD_X11)
	{
#  ifdef FEAT_XCLIPBOARD
	    clip_xterm_set_selection(cbd);
#  endif
	}
    }
# නැතහොත්
    clip_mch_set_selection(cbd);
# endif
}

    static void
clip_gen_request_selection(Clipboard_T *cbd)
{
# නම් defined(FEAT_XCLIPBOARD) || defined(FEAT_WAYLAND_CLIPBOARD)
#  ifdef FEAT_GUI
    නම් (gui.in_use)
	clip_mch_request_selection(cbd);
    නැතහොත්
#  endif
    {
	නම් (clipmethod == CLIPMETHOD_WAYLAND)
	{
#  ifdef FEAT_WAYLAND_CLIPBOARD
	    clip_wl_request_selection(cbd);
#  endif
	}
	නැතහොත් නම් (clipmethod == CLIPMETHOD_X11)
	{
#  ifdef FEAT_XCLIPBOARD
	    clip_xterm_request_selection(cbd);
#  endif
	}
    }
# නැතහොත්
    clip_mch_request_selection(cbd);
# endif
}

# නම් (defined(FEAT_X11) && defined(FEAT_XCLIPBOARD) && defined(USE_SYSTEM))
    static පූර්ණ_සංඛ්‍යා
clip_x11_owner_exists(Clipboard_T *cbd)
{
    ආපසු XGetSelectionOwner(X_DISPLAY, cbd->sel_atom) != None;
}
# endif

# නම් (defined(FEAT_X11) || defined(FEAT_WAYLAND_CLIPBOARD)) \
	&& defined(USE_SYSTEM)
    පූර්ණ_සංඛ්‍යා
clip_gen_owner_exists(Clipboard_T *cbd UNUSED)
{
#  නම් defined(FEAT_XCLIPBOARD) || defined(FEAT_WAYLAND_CLIPBOARD)
#   ifdef FEAT_GUI_GTK
    නම් (gui.in_use)
	ආපසු clip_gtk_owner_exists(cbd);
    නැතහොත්
#   endif
    {
	නම් (clipmethod == CLIPMETHOD_WAYLAND)
	{
#   ifdef FEAT_WAYLAND_CLIPBOARD
	    ආපසු clip_wl_owner_exists(cbd);
#   endif
	}
	නැතහොත් නම් (clipmethod == CLIPMETHOD_X11)
	{
#   ifdef FEAT_XCLIPBOARD
	    ආපසු clip_x11_owner_exists(cbd);
#   endif
	}
	නැතහොත්
	    ආපසු FALSE;
    }
#  නැතහොත්
    ආපසු TRUE;
#  endif
}
# endif

/*
 * Stuff for the X clipboard.  Shared between VMS and Unix.
 */

# නම් defined(FEAT_XCLIPBOARD) || defined(FEAT_GUI_X11)
#  include <X11/Xatom.h>
#  include <X11/Intrinsic.h>

/*
 * Open the application context (if it hasn't been opened yet).
 * Used for Motif GUI and the xterm clipboard.
 */
    void
open_app_context(void)
{
    නම් (app_context == NULL)
    {
	XtToolkitInitialize();
	app_context = XtCreateApplicationContext();
    }
}

static Atom	vim_atom;	// Vim's own special selection format
static Atom	vimenc_atom;	// Vim's extended selection format
static Atom	vim_mt_atom;	// Vim's own special selection format (in mime
				// type format)
static Atom	vimenc_mt_atom;	// Vim's extended selection format (in mime type
				// format)
static Atom	utf8_atom;
static Atom	compound_text_atom;
static Atom	text_atom;
static Atom	targets_atom;
static Atom	timestamp_atom;	// Used to get a timestamp

    void
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

    static Boolean
clip_x11_convert_selection_cb(
    Widget	w UNUSED,
    Atom	*sel_atom,
    Atom	*target,
    Atom	*type,
    XtPointer	*value,
    long_u	*length,
    පූර්ණ_සංඛ්‍යා		*format)
{
    static char_u   *save_result = NULL;
    static long_u   save_length = 0;
    char_u	    *string;
    පූර්ණ_සංඛ්‍යා		    motion_type;
    Clipboard_T    *cbd;
    පූර්ණ_සංඛ්‍යා		    i;

    නම් (*sel_atom == clip_plus.sel_atom)
	cbd = &clip_plus;
    නැතහොත්
	cbd = &clip_star;

    නම් (!cbd->owned)
	ආපසු False;	    // Shouldn't ever happen

    // requestor wants to know what target types we support
    නම් (*target == targets_atom)
    {
	static Atom array[9];

	*value = (XtPointer)array;
	i = 0;
	array[i++] = targets_atom;
	array[i++] = vimenc_atom;
	array[i++] = vim_atom;
	array[i++] = vimenc_mt_atom;
	array[i++] = vim_mt_atom;
	නම් (enc_utf8)
	    array[i++] = utf8_atom;
	array[i++] = XA_STRING;
	array[i++] = text_atom;
	array[i++] = compound_text_atom;

	*type = XA_ATOM;
	// This used to be: *format = sizeof(Atom) * 8; but that caused
	// crashes on 64 bit machines. (Peter Derr)
	*format = 32;
	*length = i;
	ආපසු True;
    }

    නම් (       *target != XA_STRING
	    && *target != vimenc_atom
	    && *target != vimenc_mt_atom
	    && (*target != utf8_atom || !enc_utf8)
	    && *target != vim_atom
	    && *target != vim_mt_atom
	    && *target != text_atom
	    && *target != compound_text_atom)
	ආපසු False;

    clip_get_selection(cbd);
    motion_type = clip_convert_selection(&string, length, cbd);
    නම් (motion_type < 0)
	ආපසු False;

    // For our own format, the first byte contains the motion type
    නම් (*target == vim_atom || *target == vim_mt_atom)
	(*length)++;

    // Our own format with encoding: motion 'encoding' NUL text
    නම් (*target == vimenc_atom || *target == vimenc_mt_atom)
	*length += STRLEN(p_enc) + 2;

    නම් (save_length < *length || save_length / 2 >= *length)
	*value = XtRealloc((var_na *)save_result, (Cardinal)*length + 1);
    නැතහොත්
	*value = save_result;
    නම් (*value == NULL)
    {
	vim_free(string);
	ආපසු False;
    }
    save_result = (char_u *)*value;
    save_length = *length;

    නම් (*target == XA_STRING || (*target == utf8_atom && enc_utf8))
    {
	mch_memmove(save_result, string, (size_t)(*length));
	*type = *target;
    }
    නැතහොත් නම් (*target == compound_text_atom || *target == text_atom)
    {
	XTextProperty	text_prop;
	var_na		*string_nt = (var_na *)save_result;
	පූර්ණ_සංඛ්‍යා		conv_result;

	// create NUL terminated string which XmbTextListToTextProperty wants
	mch_memmove(string_nt, string, (size_t)*length);
	string_nt[*length] = NUL;
	conv_result = XmbTextListToTextProperty(X_DISPLAY, &string_nt,
					   1, XCompoundTextStyle, &text_prop);
	නම් (conv_result != Success)
	{
	    vim_free(string);
	    ආපසු False;
	}
	*value = (XtPointer)(text_prop.value);	//    from plain text
	*length = text_prop.nitems;
	*type = compound_text_atom;
	XtFree((var_na *)save_result);
	save_result = (char_u *)*value;
	save_length = *length;
    }
    නැතහොත් නම් (*target == vimenc_atom || *target == vimenc_mt_atom)
    {
	පූර්ණ_සංඛ්‍යා l = STRLEN(p_enc);

	save_result[0] = motion_type;
	STRCPY(save_result + 1, p_enc);
	mch_memmove(save_result + l + 2, string, (size_t)(*length - l - 2));
	නම් (*target == vimenc_atom)
	    *type = vimenc_atom;
	නැතහොත්
	    *type = vimenc_mt_atom;
    }
    නැතහොත්
    {
	save_result[0] = motion_type;
	mch_memmove(save_result + 1, string, (size_t)(*length - 1));
	නම් (*target == vim_atom)
	    *type = vim_atom;
	නැතහොත්
	    *type = vim_mt_atom;
    }
    *format = 8;	    // 8 bits per char
    vim_free(string);
    ආපසු True;
}

    static void
clip_x11_lose_ownership_cb(Widget w UNUSED, Atom *sel_atom)
{
    නම් (*sel_atom == clip_plus.sel_atom)
	clip_lose_selection(&clip_plus);
    නැතහොත්
	clip_lose_selection(&clip_star);
}

    static void
clip_x11_notify_cb(Widget w UNUSED, Atom *sel_atom UNUSED, Atom *target UNUSED)
{
    // To prevent automatically freeing the selection value.
}

/*
 * Property callback to get a timestamp for XtOwnSelection.
 */
#  නම් defined(FEAT_X11) && defined(FEAT_XCLIPBOARD)
    static void
clip_x11_timestamp_cb(
    Widget	w,
    XtPointer	n UNUSED,
    XEvent	*event,
    Boolean	*cont UNUSED)
{
    Atom	    actual_type;
    පූර්ණ_සංඛ්‍යා		    format;
    unsigned  long  nitems, bytes_after;
    unsigned var_na   *prop=NULL;
    XPropertyEvent  *xproperty=&event->xproperty;

    // Must be a property notify, state can't be Delete (True), has to be
    // one of the supported selection types.
    නම් (event->type != PropertyNotify || xproperty->state
	    || (xproperty->atom != clip_star.sel_atom
				    && xproperty->atom != clip_plus.sel_atom))
	ආපසු;

    නම් (XGetWindowProperty(xproperty->display, xproperty->window,
	  xproperty->atom, 0, 0, False, timestamp_atom, &actual_type, &format,
						&nitems, &bytes_after, &prop))
	ආපසු;

    නම් (prop)
	XFree(prop);

    // Make sure the property type is "TIMESTAMP" and it's 32 bits.
    නම් (actual_type != timestamp_atom || format != 32)
	ආපසු;

    // Get the selection, using the event timestamp.
    නම් (XtOwnSelection(w, xproperty->atom, xproperty->time,
	    clip_x11_convert_selection_cb, clip_x11_lose_ownership_cb,
	    clip_x11_notify_cb) == OK)
    {
	// Set the "owned" flag now, there may have been a call to
	// lose_ownership_cb in between.
	නම් (xproperty->atom == clip_plus.sel_atom)
	    clip_plus.owned = TRUE;
	නැතහොත්
	    clip_star.owned = TRUE;
    }
}

    void
x11_setup_selection(Widget w)
{
    XtAddEventHandler(w, PropertyChangeMask, False,
	    /*(XtEventHandler)*/clip_x11_timestamp_cb, (XtPointer)NULL);
}
#  endif

    static void
clip_x11_request_selection_cb(
    Widget	w UNUSED,
    XtPointer	success,
    Atom	*sel_atom,
    Atom	*type,
    XtPointer	value,
    long_u	*length,
    පූර්ණ_සංඛ්‍යා		*format)
{
    පූර්ණ_සංඛ්‍යා		motion_type = MAUTO;
    long_u	len;
    char_u	*p;
    var_na	**text_list = NULL;
    Clipboard_T	*cbd;
    char_u	*tmpbuf = NULL;

    නම් (*sel_atom == clip_plus.sel_atom)
	cbd = &clip_plus;
    නැතහොත්
	cbd = &clip_star;

    නම් (value == NULL || *length == 0)
    {
	clip_free_selection(cbd);	// nothing received, clear register
	*(පූර්ණ_සංඛ්‍යා *)success = FALSE;
	ආපසු;
    }
    p = (char_u *)value;
    len = *length;
    නම් (*type == vim_atom || *type == vim_mt_atom)
    {
	motion_type = *p++;
	len--;
    }

    නැතහොත් නම් (*type == vimenc_atom || *type == vimenc_mt_atom)
    {
	char_u		*enc;
	vimconv_T	conv;
	පූර්ණ_සංඛ්‍යා		convlen;

	motion_type = *p++;
	--len;

	enc = p;
	p += STRLEN(p) + 1;
	len -= p - enc;

	// If the encoding of the text is different from 'encoding', attempt
	// converting it.
	conv.vc_type = CONV_NONE;
	convert_setup(&conv, enc, p_enc);
	නම් (conv.vc_type != CONV_NONE)
	{
	    convlen = len;	// Need to use an int here.
	    tmpbuf = string_convert(&conv, p, &convlen);
	    len = convlen;
	    නම් (tmpbuf != NULL)
		p = tmpbuf;
	    convert_setup(&conv, NULL, NULL);
	}
    }

    නැතහොත් නම් (*type == compound_text_atom
	    || *type == utf8_atom
	    || (enc_dbcs != 0 && *type == text_atom))
    {
	XTextProperty	text_prop;
	පූර්ණ_සංඛ්‍යා		n_text = 0;
	පූර්ණ_සංඛ්‍යා		status;

	text_prop.value = (unsigned var_na *)value;
	text_prop.encoding = *type;
	text_prop.format = *format;
	text_prop.nitems = len;
#  නම් defined(X_HAVE_UTF8_STRING)
	නම් (*type == utf8_atom)
	    status = Xutf8TextPropertyToTextList(X_DISPLAY, &text_prop,
							 &text_list, &n_text);
	නැතහොත්
#  endif
	    status = XmbTextPropertyToTextList(X_DISPLAY, &text_prop,
							 &text_list, &n_text);
	නම් (status != Success || n_text < 1)
	{
	    *(පූර්ණ_සංඛ්‍යා *)success = FALSE;
	    ආපසු;
	}
	p = (char_u *)text_list[0];
	len = STRLEN(p);
    }
    clip_yank_selection(motion_type, p, (long)len, cbd);

    නම් (text_list != NULL)
	XFreeStringList(text_list);
    vim_free(tmpbuf);
    XtFree((var_na *)value);
    *(පූර්ණ_සංඛ්‍යා *)success = TRUE;
}

    void
clip_x11_request_selection(
    Widget	myShell,
    Display	*dpy,
    Clipboard_T	*cbd)
{
    XEvent	event;
    Atom	type;
    static පූර්ණ_සංඛ්‍යා	success;
    පූර්ණ_සංඛ්‍යා		i;
    time_t	start_time;
    පූර්ණ_සංඛ්‍යා		timed_out = FALSE;

    සඳහා (i = 0; i < 8; i++)
    {
	switch (i)
	{
	    pak_sa 0:  type = vimenc_mt_atom;	bha_nga;
	    pak_sa 1:  type = vimenc_atom;	bha_nga;
	    pak_sa 2:  type = vim_mt_atom;	bha_nga;
	    pak_sa 3:  type = vim_atom;		bha_nga;
	    pak_sa 4:  type = utf8_atom;		bha_nga;
	    pak_sa 5:  type = compound_text_atom; bha_nga;
	    pak_sa 6:  type = text_atom;		bha_nga;
	    default: type = XA_STRING;
	}
	නම් (type == utf8_atom
#  නම් defined(X_HAVE_UTF8_STRING)
		&& !enc_utf8
#  endif
		)
	    // Only request utf-8 when 'encoding' is utf8 and
	    // Xutf8TextPropertyToTextList is available.
	    continue;
	success = MAYBE;
	XtGetSelectionValue(myShell, cbd->sel_atom, type,
	    clip_x11_request_selection_cb, (XtPointer)&success, CurrentTime);

	// Make sure the request for the selection goes out before waiting for
	// a response.
	XFlush(dpy);

	/*
	 * Wait for result of selection request, otherwise if we type more
	 * characters, then they will appear before the one that requested the
	 * paste!  Don't worry, we will catch up with any other events later.
	 */
	start_time = time(NULL);
	අතරතුර (success == MAYBE)
	{
	    නම් (XCheckTypedEvent(dpy, PropertyNotify, &event)
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
		continue;
	    }

	    // Time out after 2 to 3 seconds to avoid that we hang when the
	    // other process doesn't respond.  Note that the SelectionNotify
	    // event may still come later when the selection owner comes back
	    // to life and the text gets inserted unexpectedly.  Don't know
	    // why that happens or how to avoid that :-(.
	    නම් (time(NULL) > start_time + 2)
	    {
		timed_out = TRUE;
		bha_nga;
	    }

	    // Do we need this?  Probably not.
	    XSync(dpy, False);

	    // Wait for 1 msec to avoid that we eat up all CPU time.
	    ui_delay(1L, TRUE);
	}

	නම් (success == TRUE)
	    ආපසු;

	// don't do a retry with another type after timing out, otherwise we
	// hang for 15 seconds.
	නම් (timed_out)
	    bha_nga;
    }

    // Final fallback position - use the X CUT_BUFFER0 store
    yank_cut_buffer0(dpy, cbd);
}

    void
clip_x11_lose_selection(Widget myShell, Clipboard_T *cbd)
{
    XtDisownSelection(myShell, cbd->sel_atom,
				XtLastTimestampProcessed(XtDisplay(myShell)));
}

    පූර්ණ_සංඛ්‍යා
clip_x11_own_selection(Widget myShell, Clipboard_T *cbd)
{
    // When using the GUI we have proper timestamps, use the one of the last
    // event.  When in the console we don't get events (the terminal gets
    // them), Get the time by a zero-length append, clip_x11_timestamp_cb will
    // be called with the current timestamp.
#  ifdef FEAT_GUI
    නම් (gui.in_use)
    {
	නම් (XtOwnSelection(myShell, cbd->sel_atom,
	       XtLastTimestampProcessed(XtDisplay(myShell)),
	       clip_x11_convert_selection_cb, clip_x11_lose_ownership_cb,
	       clip_x11_notify_cb) == False)
	    ආපසු FAIL;
    }
    නැතහොත්
#  endif
    {
	නම් (!XChangeProperty(XtDisplay(myShell), XtWindow(myShell),
		  cbd->sel_atom, timestamp_atom, 32, PropModeAppend, NULL, 0))
	    ආපසු FAIL;
    }
    // Flush is required in a terminal as nothing else is doing it.
    XFlush(XtDisplay(myShell));
    ආපසු OK;
}

/*
 * Send the current selection to the clipboard.  Do nothing for X because we
 * will fill in the selection only when requested by another app.
 */
    void
clip_x11_set_selection(Clipboard_T *cbd UNUSED)
{
}

# endif

# නම් (defined(FEAT_XCLIPBOARD) || defined(FEAT_GUI_X11) || defined(FEAT_GUI_GTK)) \
	&& !defined(USE_GTK4)
/*
 * Get the contents of the X CUT_BUFFER0 and put it in "cbd".
 */
    void
yank_cut_buffer0(Display *dpy, Clipboard_T *cbd)
{
    පූර්ණ_සංඛ්‍යා		nbytes = 0;
    char_u	*buffer = (char_u *)XFetchBuffer(dpy, &nbytes, 0);

    නම් (nbytes > 0)
    {
	පූර්ණ_සංඛ්‍යා  done = FALSE;

	// CUT_BUFFER0 is supposed to be always latin1.  Convert to 'enc' when
	// using a multi-byte encoding.  Conversion between two 8-bit
	// character sets usually fails and the text might actually be in
	// 'enc' anyway.
	නම් (has_mbyte)
	{
	    char_u	*conv_buf;
	    vimconv_T	vc;

	    vc.vc_type = CONV_NONE;
	    නම් (convert_setup(&vc, (char_u *)"latin1", p_enc) == OK)
	    {
		conv_buf = string_convert(&vc, buffer, &nbytes);
		නම් (conv_buf != NULL)
		{
		    clip_yank_selection(MCHAR, conv_buf, (long)nbytes, cbd);
		    vim_free(conv_buf);
		    done = TRUE;
		}
		convert_setup(&vc, NULL, NULL);
	    }
	}
	නම් (!done)  // use the text without conversion
	    clip_yank_selection(MCHAR, buffer, (long)nbytes, cbd);
	XFree((void *)buffer);
	නම් (p_verbose > 0)
	{
	    verbose_enter();
	    verb_msg(_("Used CUT_BUFFER0 instead of empty selection"));
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
# නම් defined(FEAT_X11) && defined(FEAT_CLIPBOARD)
    void
x11_export_final_selection(void)
{
    Display	*dpy;
    char_u	*str = NULL;
    long_u	len = 0;
    පූර්ණ_සංඛ්‍යා		motion_type = -1;

#  ifdef FEAT_GUI
    නම් (gui.in_use)
	dpy = X_DISPLAY;
    නැතහොත්
#  endif
#  ifdef FEAT_XCLIPBOARD
	dpy = xterm_dpy;
#  නැතහොත්
	ආපසු;
#  endif

    // Get selection to export
    නම් (clip_plus.owned)
	motion_type = clip_convert_selection(&str, &len, &clip_plus);
    නැතහොත් නම් (clip_star.owned)
	motion_type = clip_convert_selection(&str, &len, &clip_star);

    // Check it's OK
    නම් (dpy != NULL && str != NULL && motion_type >= 0
					       && len < 1024*1024 && len > 0)
    {
	පූර්ණ_සංඛ්‍යා ok = TRUE;

	// The CUT_BUFFER0 is supposed to always contain latin1.  Convert from
	// 'enc' when it is a multi-byte encoding.  When 'enc' is an 8-bit
	// encoding conversion usually doesn't work, so keep the text as-is.
	නම් (has_mbyte)
	{
	    vimconv_T	vc;

	    vc.vc_type = CONV_NONE;
	    නම් (convert_setup(&vc, p_enc, (char_u *)"latin1") == OK)
	    {
		පූර්ණ_සංඛ්‍යා	intlen = len;
		char_u	*conv_str;

		vc.vc_fail = TRUE;
		conv_str = string_convert(&vc, str, &intlen);
		len = intlen;
		නම් (conv_str != NULL)
		{
		    vim_free(str);
		    str = conv_str;
		}
		නැතහොත්
		{
		    ok = FALSE;
		}
		convert_setup(&vc, NULL, NULL);
	    }
	    නැතහොත්
	    {
		ok = FALSE;
	    }
	}

	// Do not store the string if conversion failed.  Better to use any
	// other selection than garbled text.
	නම් (ok)
	{
	    XStoreBuffer(dpy, (var_na *)str, (පූර්ණ_සංඛ්‍යා)len, 0);
	    XFlush(dpy);
	}
    }

    vim_free(str);
}
# endif

    void
clip_free_selection(Clipboard_T *cbd)
{
    yankreg_T *y_ptr = get_y_current();

    නම් (cbd == &clip_plus)
	set_y_current(get_y_register(PLUS_REGISTER));
    නැතහොත්
	set_y_current(get_y_register(STAR_REGISTER));
    free_yank_all();
    get_y_current()->y_size = 0;
    set_y_current(y_ptr);
}

/*
 * Get the selected text and put it in register '*' or '+'.
 */
    void
clip_get_selection(Clipboard_T *cbd)
{
    yankreg_T	*old_y_previous, *old_y_current;
    pos_T	old_cursor;
    pos_T	old_visual;
    පූර්ණ_සංඛ්‍යා		old_visual_mode;
    colnr_T	old_virtcol;
    colnr_T	old_curswant;
    පූර්ණ_සංඛ්‍යා		old_set_curswant;
    pos_T	old_op_start, old_op_end;
    oparg_T	oa;
    cmdarg_T	ca;

    නම් (cbd->owned)
    {
	නම් ((cbd == &clip_plus
		&& get_y_register(PLUS_REGISTER)->y_array != NULL)
		|| (cbd == &clip_star
		    && get_y_register(STAR_REGISTER)->y_array != NULL))
	    ආපසු;

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
    නැතහොත් නම් (!is_clipboard_needs_update())
    {
	clip_free_selection(cbd);

	// Try to get selected text from another window
	clip_gen_request_selection(cbd);
    }
}

/*
 * Convert from the GUI selection string into the '*'/'+' register.
 */
    void
clip_yank_selection(
    පූර්ණ_සංඛ්‍යා		type,
    char_u	*str,
    long	len,
    Clipboard_T *cbd)
{
    yankreg_T *y_ptr;

    නම් (cbd == &clip_plus)
	y_ptr = get_y_register(PLUS_REGISTER);
    නැතහොත්
	y_ptr = get_y_register(STAR_REGISTER);

    clip_free_selection(cbd);

    str_to_reg(y_ptr, type, str, len, -1, FALSE);
}

    පූර්ණ_සංඛ්‍යා
clip_convert_selection_offset(
	char_u	    **str,
	long_u	    *len,
	පූර්ණ_සංඛ්‍යා	    offset, // Extra space to add in *str and the offset to
			    // place the actual string in *str.
	Clipboard_T *cbd)
{
    char_u	*p;
    පූර්ණ_සංඛ්‍යා		lnum;
    පූර්ණ_සංඛ්‍යා		i, j;
    int_u	eolsize;
    yankreg_T	*y_ptr;

    නම් (cbd == &clip_plus)
	y_ptr = get_y_register(PLUS_REGISTER);
    නැතහොත්
	y_ptr = get_y_register(STAR_REGISTER);

# ifdef USE_CRNL
    eolsize = 2;
# නැතහොත්
    eolsize = 1;
# endif

    *str = NULL;
    *len = 0;
    නම් (y_ptr->y_array == NULL)
	ආපසු -1;

    සඳහා (i = 0; i < y_ptr->y_size; i++)
	*len += (long_u)y_ptr->y_array[i].length + eolsize;

    // Don't want newline character at end of last line if we're in MCHAR mode.
    නම් (y_ptr->y_type == MCHAR && *len >= eolsize)
	*len -= eolsize;

    *len += offset;
    p = *str = alloc(*len + 1);	// add one to avoid zero
    නම් (p == NULL)
	ආපසු -1;
    p += offset;
    lnum = 0;
    සඳහා (i = 0, j = 0; i < (පූර්ණ_සංඛ්‍යා)*len - offset; i++, j++)
    {
	නම් (y_ptr->y_array[lnum].string[j] == '\n')
	    p[i] = NUL;
	නැතහොත් නම් (y_ptr->y_array[lnum].string[j] == NUL)
	{
# ifdef USE_CRNL
	    p[i++] = '\r';
# endif
	    p[i] = '\n';
	    lnum++;
	    j = -1;
	}
	නැතහොත්
	    p[i] = y_ptr->y_array[lnum].string[j];
    }
    ආපසු y_ptr->y_type;
}

/*
 * Convert the '*'/'+' register into a GUI selection string returned in *str
 * with length *len.
 * Returns the motion type, or -1 for failure.
 */
    පූර්ණ_සංඛ්‍යා
clip_convert_selection(char_u **str, long_u *len, Clipboard_T *cbd)
{
    ආපසු clip_convert_selection_offset(str, len, 0, cbd);
}

/*
 * When "regname" is a clipboard register, obtain the selection.  If it's not
 * available return zero, otherwise return "regname".
 */
    පූර්ණ_සංඛ්‍යා
may_get_selection(පූර්ණ_සංඛ්‍යා regname)
{
    නම් (regname == '*')
    {
	නම් (!clip_star.available)
	    regname = 0;
	නැතහොත්
	    clip_get_selection(&clip_star);
    }
    නැතහොත් නම් (regname == '+')
    {
	නම් (!clip_plus.available)
	    regname = 0;
	නැතහොත්
	    clip_get_selection(&clip_plus);
    }
    ආපසු regname;
}

/*
 * If we have written to a clipboard register, send the text to the clipboard.
 */
    void
may_set_selection(void)
{
    නම් ((get_y_current() == get_y_register(STAR_REGISTER))
	    && clip_star.available)
    {
	clip_own_selection(&clip_star);
	clip_gen_set_selection(&clip_star);
    }
    නැතහොත් නම් ((get_y_current() == get_y_register(PLUS_REGISTER))
	    && clip_plus.available)
    {
	clip_own_selection(&clip_plus);
	clip_gen_set_selection(&clip_plus);
    }
}

# නම් defined(FEAT_WAYLAND_CLIPBOARD)

    static clip_wl_selection_T *
clip_wl_get_selection(wayland_selection_T sel)
{
    switch (sel)
    {
	pak_sa WAYLAND_SELECTION_REGULAR:
	    ආපසු &clip_wl.regular;
	pak_sa WAYLAND_SELECTION_PRIMARY:
	    ආපසු &clip_wl.primary;
	default:
	    ආපසු NULL;
    }
}

    static clip_wl_selection_T *
clip_wl_get_selection_from_cbd(Clipboard_T *cbd)
{
    නම් (cbd == &clip_plus)
	ආපසු &clip_wl.regular;
    නැතහොත් නම් (cbd == &clip_star)
	ආපසු &clip_wl.primary;
    නැතහොත්
	ආපසු NULL;
}

    static Clipboard_T *
clip_wl_get_cbd_from_selection(clip_wl_selection_T *sel)
{
    නම් (sel == &clip_wl.regular)
	ආපසු &clip_plus;
    නැතහොත් නම් (sel == &clip_wl.primary)
	ආපසු &clip_star;
    නැතහොත්
	ආපසු NULL;
}

    static wayland_selection_T
clip_wl_get_selection_type(clip_wl_selection_T *sel)
{
    නම් (sel == &clip_wl.regular)
	ආපසු WAYLAND_SELECTION_REGULAR;
    නැතහොත් නම් (sel == &clip_wl.primary)
	ආපසු WAYLAND_SELECTION_PRIMARY;
    නැතහොත්
	ආපසු WAYLAND_SELECTION_NONE;
}

    static bool
wl_data_offer_listener_event_offer(
    void *data UNUSED,
    vwl_data_offer_T *offer UNUSED,
    const var_na *mime_type
)
{
    // Only accept mime type if we support it
    සඳහා (පූර්ණ_සංඛ්‍යා i = 0; i < (පූර්ණ_සංඛ්‍යා)ARRAY_LENGTH(supported_mimes); i++)
	නම් (STRCMP(mime_type, supported_mimes[i]) == 0)
	    ආපසු true;
    ආපසු FALSE;
}

static const vwl_data_offer_listener_T vwl_data_offer_listener = {
    .offer = wl_data_offer_listener_event_offer
};

    static void
vwl_data_device_listener_event_data_offer(
	void *data UNUSED,
	vwl_data_device_T *device UNUSED,
	vwl_data_offer_T *offer)
{
    // Immediately start listening for offer events from the data offer
    vwl_data_offer_add_listener(offer, &vwl_data_offer_listener, NULL);
}

    static void
vwl_data_device_listener_event_selection(
	void *data UNUSED,
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
    නම් (offer == NULL || sel->source != NULL)
    {
	// Selection event is from us, so we are the source client. Therefore
	// ignore it. Or the selection is cleared, so set sel->offer to NULL
	vwl_data_offer_destroy(offer);
	sel->offer = NULL;
	ආපසු;
    }

    // Save offer. When we want to request data, then we'll actually call the
    // receive method.
    sel->offer = offer;

}

    static void
vwl_data_device_listener_event_finished(
	void *data UNUSED,
	vwl_data_device_T *device)
{
    clip_wl_selection_T *sel;
    // Device finished, guessing this can happen is when the seat becomes
    // invalid? If so, let the user call :wlrestore! to reset. There wouldn't be
    // any point in trying to create another data device for the same seat,
    // since the seat is in an invalid state.
    නම් (device == clip_wl.regular.device)
    {
	sel = &clip_wl.regular;
	clip_wl.regular.device = NULL;
    }
    නැතහොත් නම් (device == clip_wl.primary.device)
    {
	sel = &clip_wl.primary;
	clip_wl.primary.device = NULL;
    }
    නැතහොත්
	// Shouldn't happen
	ආපසු;

    vim_free(sel->contents);
    vwl_data_source_destroy(sel->source);
    vwl_data_offer_destroy(sel->offer);
    sel->available = FALSE;

    vwl_data_device_destroy(device);
}

static const vwl_data_device_listener_T vwl_data_device_listener = {
    .data_offer = vwl_data_device_listener_event_data_offer,
    .selection = vwl_data_device_listener_event_selection,
    .finished = vwl_data_device_listener_event_finished
};

/*
 * Initialize the clipboard for Wayland using the global Wayland connection.
 * Returns OK on success and FAIL on failure.
 */
    පූර්ණ_සංඛ්‍යා
clip_init_wayland(void)
{
    int_u supported = WAYLAND_SELECTION_NONE;

    නම් (wayland_ct == NULL)
	ආපසු FAIL;

    memset(&clip_wl, 0, parimaa_na(clip_wl));

    clip_wl.seat = vwl_connection_get_seat(wayland_ct, (var_na *)p_wse);

    නම් (clip_wl.seat == NULL)
	ආපසු FAIL;

    clip_wl.regular.manager = vwl_connection_get_data_device_manager(
	    wayland_ct, WAYLAND_SELECTION_REGULAR, &supported);

    නම් (clip_wl.regular.manager != NULL)
    {
	clip_wl.regular.device = vwl_data_device_manager_get_data_device(
		clip_wl.regular.manager, clip_wl.seat);

	නම් (clip_wl.regular.device != NULL)
	    clip_wl.regular.available = true;
	නැතහොත්
	{
	    // Shouldn't happen
	    vwl_data_device_manager_discard(clip_wl.regular.manager);
	    clip_wl.regular.manager = NULL;
	    ආපසු FAIL;
	}
    }

    // If we still don't support the primary selection, try finding one for it
    // specifically.
    නම් (!(supported & WAYLAND_SELECTION_PRIMARY))
    {
	clip_wl.primary.manager = vwl_connection_get_data_device_manager(
		wayland_ct, WAYLAND_SELECTION_PRIMARY, &supported);

	නම් (clip_wl.primary.manager != NULL)
	{
	    clip_wl.primary.device = vwl_data_device_manager_get_data_device(
		    clip_wl.primary.manager, clip_wl.seat);

	    නම් (clip_wl.primary.device != NULL)
		clip_wl.primary.available = true;
	    නැතහොත්
	    {
		vwl_data_device_manager_discard(clip_wl.primary.manager);
		clip_wl.primary.manager = NULL;
	    }
	}
    }

    නම් (clip_wl.regular.available && !clip_wl.primary.available)
    {
	// The protocol supports both regular and primary selections, just use
	// one data device manager and one data device. Or the primary selection
	// is not supported, make it point to the regular selection instead.
	clip_wl.primary.available = true;
	clip_wl.primary.manager = clip_wl.regular.manager;
	clip_wl.primary.device = clip_wl.regular.device;
    }

    නම් (!clip_wl.regular.available && !clip_wl.primary.available)
	ආපසු FAIL;

    // Start listening for selection updates
    නම් (clip_wl.regular.device != NULL)
	vwl_data_device_add_listener(clip_wl.regular.device,
		&vwl_data_device_listener, NULL);
    // Don't want to listen to the same data device twice
    නම් (clip_wl.primary.device != NULL
	    && clip_wl.primary.device != clip_wl.regular.device)
	vwl_data_device_add_listener(clip_wl.primary.device,
		&vwl_data_device_listener, NULL);

    ආපසු OK;
}

    void
clip_uninit_wayland(void)
{
    clip_wl_selection_T *sel;

    නම් (clipmethod == CLIPMETHOD_WAYLAND)
    {
	නම් (clip_star.owned)
	    clip_lose_selection(&clip_star);
	නම් (clip_plus.owned)
	    clip_lose_selection(&clip_plus);
    }

    // Don't want to double free
    නම් (clip_wl.regular.manager != clip_wl.primary.manager)
	vwl_data_device_manager_discard(clip_wl.primary.manager);
    vwl_data_device_manager_discard(clip_wl.regular.manager);

    නම් (clip_wl.regular.device != clip_wl.primary.device)
	vwl_data_device_destroy(clip_wl.primary.device);
    vwl_data_device_destroy(clip_wl.regular.device);

    sel = &clip_wl.regular;
    අතරතුර (true)
    {
	vim_free(sel->contents);
	vwl_data_source_destroy(sel->source);
	vwl_data_offer_destroy(sel->offer);
	sel->available = false;

	නම් (sel == &clip_wl.primary)
	    bha_nga;
	sel = &clip_wl.primary;
    }

    vim_memset(&clip_wl, 0, parimaa_na(clip_wl));
}

    පූර්ණ_සංඛ්‍යා
clip_reset_wayland(void)
{
    wayland_uninit_connection();

    නම් (wayland_init_connection(wayland_display_name) == FAIL
	    || clip_init_wayland() == FAIL)
	ආපසු FAIL;

    choose_clipmethod();
    ආපසු OK;
}


/*
 * Read data from a file descriptor and write it to the given clipboard.
 */
    static void
clip_wl_receive_data(Clipboard_T *cbd, const var_na *mime_type, පූර්ණ_සංඛ්‍යා fd)
{
    char_u	*start, *final;
    long	len;
    garray_T	buf;
    පූර්ණ_සංඛ්‍යා		motion_type = MAUTO;
    ssize_t	r = 0;
    char_u	*tofree = NULL;
#  ifndef HAVE_SELECT
    sa_nracanaa pollfd   pfd;

    pfd.fd = fd;
    pfd.events = POLLIN;
#  නැතහොත්
    fd_set rfds;
    sa_nracanaa timeval  tv;

    FD_ZERO(&rfds);
    FD_SET(fd, &rfds);
#  endif

    // Make pipe (read end) non-blocking
    නම් (fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK) == -1)
	ආපසු;

    ga_init2(&buf, 1, 4096);

    // 4096 bytes seems reasonable for initial buffer size, memory is cheap
    // anyways.
    නම් (ga_grow(&buf, 4096) == FAIL)
	ආපසු;

    start = buf.ga_data;

#  ifndef HAVE_SELECT
    අතරතුර (poll(&pfd, 1, p_wtm) > 0)
#  නැතහොත්
    අතරතුර (tv.tv_sec = p_wtm / 1000, tv.tv_usec = (p_wtm % 1000) * 1000,
	    select(fd + 1, &rfds, NULL, NULL, &tv) > 0)
#  endif
    {
	r = read(fd, start, buf.ga_maxlen - 1 - buf.ga_len);

	නම් (r == 0)
	    bha_nga;
	නැතහොත් නම් (r < 0)
	{
	    නම් (errno == EAGAIN || errno == EINTR)
		continue;
	    bha_nga;
	}

	start += r;
	buf.ga_len += r;

	// Realloc if we are at the end of the buffer
	නම් (buf.ga_len >= buf.ga_maxlen - 1)
	{
	    නම් (ga_grow(&buf, 8192) == FAIL)
		bha_nga;
	    start = (char_u *)buf.ga_data + buf.ga_len;
	}
    }

    නම් (buf.ga_len == 0)
    {
	clip_free_selection(cbd); // Nothing received, clear register
	ga_clear(&buf);
	ආපසු;
    }

    final = buf.ga_data;
    len = buf.ga_len;

    නම් (clip_convert_data(&final, &len, &motion_type,
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
    static void
clip_wl_request_selection(Clipboard_T *cbd)
{
    clip_wl_selection_T *sel = clip_wl_get_selection_from_cbd(cbd);
    පූර්ණ_සංඛ්‍යා			fds[2];
    පූර්ණ_සංඛ්‍යා			mime_types_len;
    const var_na		**mime_types;
    const var_na		*chosen_mime = NULL;

    නම් (!sel->available)
	goto clear;

    // Dispatch any events that still queued up before checking for a data
    // offer.
    නම් (vwl_connection_roundtrip(wayland_ct) == FAIL)
	goto clear;

    නම් (sel->offer == NULL)
	goto clear;

    mime_types_len = sel->offer->mime_types.ga_len;
    mime_types = sel->offer->mime_types.ga_data;

    // Choose mime type to receive from. Mime types with a lower index in the
    // "supported_mimes" array are prioritized over ones after it.
    සඳහා (පූර්ණ_සංඛ්‍යා i = 0; i < (පූර්ණ_සංඛ්‍යා)ARRAY_LENGTH(supported_mimes)
	    && chosen_mime == NULL; i++)
    {
	සඳහා (පූර්ණ_සංඛ්‍යා k = 0; k < mime_types_len && chosen_mime == NULL; k++)
	    නම් (STRCMP(mime_types[k], supported_mimes[i]) == 0)
		chosen_mime = supported_mimes[i];
    }

    නම් (chosen_mime == NULL || pipe(fds) == -1)
	goto clear;

    vwl_data_offer_receive(sel->offer, chosen_mime, fds[1]);

    close(fds[1]); // Close before we read data so that when the source client
		   // closes their end we receive an EOF.

    නම් (vwl_connection_flush(wayland_ct) >= 0)
	clip_wl_receive_data(cbd, chosen_mime, fds[0]);

    close(fds[0]);

    ආපසු;
clear:
    clip_free_selection(cbd);
}

    static void
vwl_data_source_listener_event_send(
    void *data,
    vwl_data_source_T *source UNUSED,
    const var_na *mime_type,
    int32_t fd
)
{
    clip_wl_selection_T *sel = data;
    Clipboard_T		*cbd = clip_wl_get_cbd_from_selection(sel);
    bool		have_mime = false;
    පූර්ණ_සංඛ්‍යා			motion_type;
    long_u		length;
    char_u		*string; // Will be reallocated to a bigger size if
				 // needed.
    පූර්ණ_සංඛ්‍යා			offset = 0;
    bool		is_vim, is_vimenc;
    size_t		total = 0;
#  ifndef HAVE_SELECT
    sa_nracanaa pollfd   pfd;

    pfd.fd = fd;
    pfd.events = POLLOUT;
#  නැතහොත්
    fd_set	    wfds;
    sa_nracanaa timeval  tv;

    FD_ZERO(&wfds);
    FD_SET(fd, &wfds);
#  endif

    // Check if we actually have mime type
    සඳහා (පූර්ණ_සංඛ්‍යා i = 0; i < (පූර්ණ_සංඛ්‍යා)ARRAY_LENGTH(supported_mimes); i++)
	නම් (STRCMP(supported_mimes[i], mime_type) == 0)
	{
	    have_mime = true;
	    bha_nga;
	}

    නම් (!have_mime)
	goto exit;

    // First byte sent is motion type for vim specific formats. For the vimenc
    // format, after the first byte is the encoding type, which is null
    // terminated.

    is_vimenc = STRCMP(mime_type, VIMENC_ATOM_NAME) == 0
	|| STRCMP(mime_type, VIMENC_MIMETYPE_NAME) == 0;
    is_vim = STRCMP(mime_type, VIM_ATOM_NAME) == 0
	|| STRCMP(mime_type, VIM_MIMETYPE_NAME) == 0;

    නම් (is_vimenc)
	offset += 2 + STRLEN(p_enc);
    නැතහොත් නම් (is_vim)
	offset += 1;

    clip_get_selection(cbd);
    motion_type = clip_convert_selection_offset(&string, &length, offset, cbd);

    නම් (motion_type < 0)
	goto exit;

    නම් (is_vimenc)
    {
	string[0] = (char_u)motion_type;
	// Use vim_strncpy for safer copying
	vim_strncpy(string + 1, p_enc, STRLEN(p_enc));
    }
    නැතහොත් නම් (is_vim)
	string[0] = (char_u)motion_type;


    අතරතුර (total < (size_t)length &&
#  ifndef HAVE_SELECT
	    poll(&pfd, 1, p_wtm) > 0)
#  නැතහොත්
	    ((tv.tv_sec = p_wtm / 1000, tv.tv_usec = (p_wtm % 1000) * 1000),
	    select(fd + 1, NULL, &wfds, NULL, &tv) > 0))
#  endif
    {
	ssize_t w = write(fd, string + total, length - total);

	නම් (w == -1)
	    bha_nga;
	total += w;
    }

    vim_free(string);
exit:
    close(fd);
}

    static void
vwl_data_source_listener_event_cancelled(
	void *data,
	vwl_data_source_T *source UNUSED)
{
    clip_wl_selection_T *sel = data;
    Clipboard_T		*cbd = clip_wl_get_cbd_from_selection(sel);

    clip_lose_selection(cbd);
}

static const vwl_data_source_listener_T vwl_data_source_listener = {
    .send = vwl_data_source_listener_event_send,
    .cancelled = vwl_data_source_listener_event_cancelled
};

    static void
clip_wl_do_set_selection(void *data, uint32_t serial)
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
    static පූර්ණ_සංඛ්‍යා
clip_wl_own_selection(Clipboard_T *cbd)
{
    clip_wl_selection_T *sel = clip_wl_get_selection_from_cbd(cbd);
    wayland_selection_T sel_type = clip_wl_get_selection_type(sel);

    නම් (!sel->available || vwl_connection_roundtrip(wayland_ct) == FAIL)
	ආපසු FAIL;

    නම් (sel->source != NULL)
    {
	නම් (sel_type == WAYLAND_SELECTION_PRIMARY)
	    // We already own the selection, ignore (only do this for primary
	    // selection). We don't re set the selection because then we would
	    // be setting the selection every time the user moves the visual
	    // selection cursor, which is messy and inefficient. Some
	    // applications like Google Chrome do it this way however.
	    ආපසු OK;
	නැතහොත් නම් (sel_type == WAYLAND_SELECTION_REGULAR)
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
	නැතහොත්
	    // Shouldn't happen
	    ආපසු FAIL;
    }

    sel->source = vwl_data_device_manager_create_data_source(sel->manager);
    vwl_data_source_add_listener(sel->source, &vwl_data_source_listener, sel);

    // Advertise mime types
    සඳහා (පූර්ණ_සංඛ්‍යා i = 0; i < (පූර්ණ_සංඛ්‍යා)ARRAY_LENGTH(supported_mimes); i++)
	vwl_data_source_offer(sel->source, supported_mimes[i]);

    sel->own_success = false;
    clip_wl_do_set_selection(sel, 0);

    නම් (!sel->own_success)
	goto fail;

    ආපසු OK;
fail:
    vwl_data_source_destroy(sel->source);
    sel->source = NULL;
    ආපසු FAIL;
}

/*
 * Disown the selection that cbd corresponds to.
 */
    static void
clip_wl_lose_selection(Clipboard_T *cbd)
{
    clip_wl_selection_T *sel = clip_wl_get_selection_from_cbd(cbd);

    නම් (!sel->available)
	ආපසු;

    vwl_data_source_destroy(sel->source);
    sel->source = NULL;
}

/*
 * Send the current selection to the clipboard. Do nothing for Wayland because
 * we will fill in the selection only when requested by another client.
 */
    static void
clip_wl_set_selection(Clipboard_T *cbd UNUSED)
{
}

#  නම් defined(USE_SYSTEM)
/*
 * Return true if we own the selection corresponding to cbd or another client
 * does.
 */
    static bool
clip_wl_owner_exists(Clipboard_T *cbd)
{
    clip_wl_selection_T *sel = clip_wl_get_selection_from_cbd(cbd);

    නම් (vwl_connection_roundtrip(wayland_ct) == FAIL)
	ආපසු false;

    ආපසු sel->available && (sel->source != NULL || sel->offer != NULL);
}
#  endif

# endif // FEAT_WAYLAND_CLIPBOARD

#endif // FEAT_CLIPBOARD

#ifdef HAVE_CLIPMETHOD

/*
 * Returns the first method for accessing the clipboard that is available/works,
 * depending on the order of values in str.
 */
    static clipmethod_T
get_clipmethod(char_u *str)
{
    පූර්ණ_සංඛ්‍යා		len	= (පූර්ණ_සංඛ්‍යා)STRLEN(str) + 1;
    char_u	*buf	= alloc(len);

    නම් (buf == NULL)
	ආපසු CLIPMETHOD_FAIL;

    clipmethod_T ret = CLIPMETHOD_FAIL;
    char_u	*p = str;

    අතරතුර (*p != NUL)
    {
	clipmethod_T method = CLIPMETHOD_NONE;

	(void)copy_option_part(&p, buf, len, ",");

	නම් (STRCMP(buf, "wayland") == 0)
	{
# ifdef FEAT_GUI
	    නම් (!gui.in_use)
# endif
	    {
# ifdef FEAT_WAYLAND_CLIPBOARD
		නම් (clip_wl.regular.available || clip_wl.primary.available)
		    method = CLIPMETHOD_WAYLAND;
# endif
	    }
	}
	නැතහොත් නම් (STRCMP(buf, "x11") == 0)
	{
# ifdef FEAT_GUI
	    නම් (!gui.in_use)
# endif
	    {
# ifdef FEAT_XCLIPBOARD
		// x_IOerror_handler() in os_unix.c should set xterm_dpy to NULL if
		// we lost connection to the X server.
		නම් (xterm_dpy != NULL)
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
	නැතහොත්
	{
# ifdef FEAT_CLIPBOARD_PROVIDER
	    // Check if name matches a clipboard provider
	    පූර්ණ_සංඛ්‍යා r = clip_provider_is_available(buf);

	    නම් (r == 1)
	    {
		method = CLIPMETHOD_PROVIDER;
		නම් (ret == CLIPMETHOD_FAIL)
		{
		    vim_free(clip_provider);
		    clip_provider = vim_strsave(buf);
		    නම් (clip_provider == NULL)
			goto fail;
		}
	    }
	    නැතහොත් නම් (r == -1)
# endif
	    {
# ifdef FEAT_CLIPBOARD_PROVIDER
fail:
# endif
		ret = CLIPMETHOD_FAIL;
		goto exit;
	    }
	}

	// Keep on going in order to catch errors
	නම් (method != CLIPMETHOD_NONE && ret == CLIPMETHOD_FAIL)
	    ret = method;
    }

    // No match found, use "none".
    ret = (ret == CLIPMETHOD_FAIL) ? CLIPMETHOD_NONE : ret;

exit:
    vim_free(buf);
    ආපසු ret;
}


/*
 * Returns name of clipmethod in a statically allocated string.
 */
    static char_u *
clipmethod_to_str(clipmethod_T method)
{
    switch(method)
    {
	pak_sa CLIPMETHOD_WAYLAND:
	    ආපසු (char_u *)"wayland";
	pak_sa CLIPMETHOD_X11:
	    ආපසු (char_u *)"x11";
	pak_sa CLIPMETHOD_PROVIDER:
# ifdef FEAT_EVAL
	    ආපසු clip_provider;
# endif
	default:
	    ආපසු (char_u *)"none";
    }
}

/*
 * Sets the current clipmethod to use given by `get_clipmethod()`. Returns an
 * error message on failure else NULL.
 */
    var_na *
choose_clipmethod(void)
{
    clipmethod_T method = get_clipmethod(p_cpm);

    නම් (method == CLIPMETHOD_FAIL)
	ආපසු e_invalid_argument;

// If GUI is running or we are not on a system with Wayland or X11, then always
// return CLIPMETHOD_NONE. System or GUI clipboard handling always overrides.
// This is unless a provider is being used.
# නම් defined(FEAT_XCLIPBOARD) || defined(FEAT_WAYLAND_CLIPBOARD)
#  නම් defined(FEAT_GUI)
    නම් (method != CLIPMETHOD_PROVIDER && gui.in_use)
    {
#   ifdef FEAT_WAYLAND
	// We only interact with Wayland for the clipboard, we can just deinit
	// everything.
	wayland_uninit_connection();
#   endif

	method = CLIPMETHOD_NONE;
	goto lose_sel_exit;
    }
#  endif
# නැතහොත්
    // If on a system like windows or macos, then clipmethod is irrelevant, we
    // use their way of accessing the clipboard. This is unless we are using the
    // clipboard provider
#  ifdef FEAT_CLIPBOARD_PROVIDER
    නම් (method != CLIPMETHOD_PROVIDER)
#  endif
    {
	method = CLIPMETHOD_NONE;
	goto exit;
    }
# endif

# ifdef FEAT_CLIPBOARD
    // Deinitialize clipboard if there is no way to access clipboard
    නම් (method == CLIPMETHOD_NONE)
	clip_init(FALSE);
    // If we have a clipmethod that works now, then initialize clipboard
    නැතහොත් නම් (clipmethod == CLIPMETHOD_NONE && method != CLIPMETHOD_NONE)
    {
	clip_init(TRUE);
	did_warn_clipboard = false;
    }
    // Disown clipboard if we are switching to a new method
    නැතහොත් නම් (clipmethod != CLIPMETHOD_NONE && method != clipmethod)
    {
#  නම් (defined(FEAT_XCLIPBOARD) || defined(FEAT_WAYLAND_CLIPBOARD)) \
	&& defined(FEAT_GUI)
lose_sel_exit:
#  endif
	නම් (clip_star.owned)
	    clip_lose_selection(&clip_star);
	නම් (clip_plus.owned)
	    clip_lose_selection(&clip_plus);

#  නම් defined(FEAT_GUI)
	නම් (!gui.in_use)
#  endif
	{
	    clip_init(TRUE);
	    did_warn_clipboard = false;
	}
    }
# endif // FEAT_CLIPBOARD

# නම් !defined(FEAT_XCLIPBOARD) && !defined(FEAT_WAYLAND_CLIPBOARD)
exit:
# endif

    clipmethod = method;

# ifdef FEAT_EVAL
    set_vim_var_string(VV_CLIPMETHOD, (char_u*)clipmethod_to_str(method), -1);
# endif

    ආපසු NULL;
}

/*
 * Call choose_clipmethod().
 */
    void
ex_clipreset(exarg_T *eap UNUSED)
{
    clipmethod_T prev = clipmethod;

    choose_clipmethod();

    නම් (clipmethod == CLIPMETHOD_NONE)
	smsg(_("Could not find a way to access the clipboard."));
    නැතහොත් නම් (clipmethod != prev)
	smsg(_("Switched to clipboard method '%s'."),
		clipmethod_to_str(clipmethod));
}

/*
 * Adjust the register name pointed to with "rp" for the clipboard being
 * used always and the clipboard being available.
 */
    void
adjust_clip_reg(පූර්ණ_සංඛ්‍යා *rp)
{
# ifdef FEAT_CLIPBOARD_PROVIDER
    නම් (clipmethod == CLIPMETHOD_PROVIDER)
    {
	නම් (*rp == 0 && clip_unnamed != 0)
	    *rp = ((clip_unnamed & CLIP_UNNAMED_PLUS)) ? '+' : '*';
	ආපසු;
    }
# endif
# ifdef FEAT_CLIPBOARD
    // If no reg. specified, and "unnamed" or "unnamedplus" is in 'clipboard',
    // use '*' or '+' reg, respectively. "unnamedplus" prevails.
    නම් (*rp == 0 && (clip_unnamed != 0 || clip_unnamed_saved != 0))
    {
	නම් (clip_unnamed != 0)
	    *rp = ((clip_unnamed & CLIP_UNNAMED_PLUS) && clip_plus.available)
								  ? '+' : '*';
	නැතහොත්
	    *rp = ((clip_unnamed_saved & CLIP_UNNAMED_PLUS)
					   && clip_plus.available) ? '+' : '*';
    }
    නම් ((!clip_star.available && *rp == '*') ||
	   (!clip_plus.available && *rp == '+'))
    {
	msg_warn_missing_clipboard();
	*rp = 0;
    }
# endif
}

/*
 * Extract the items in the 'clipboard' option and set global values.
 * Return an error message or NULL for success.
 */
    var_na *
did_set_clipboard(optset_T *args UNUSED)
{
    පූර්ණ_සංඛ්‍යා		new_unnamed = 0;
# ifdef FEAT_CLIPBOARD
    පූර්ණ_සංඛ්‍යා		new_autoselect_star = FALSE;
    පූර්ණ_සංඛ්‍යා		new_autoselect_plus = FALSE;
    පූර්ණ_සංඛ්‍යා		new_autoselectml = FALSE;
    පූර්ණ_සංඛ්‍යා		new_html = FALSE;
# endif
    regprog_T	*new_exclude_prog = NULL;
    var_na	*errmsg = NULL;
    char_u	*p;

    සඳහා (p = p_cb; *p != NUL; )
    {
	// Note: Keep this in sync with p_cb_values.
	නම් (STRNCMP(p, "unnamed", 7) == 0 && (p[7] == ',' || p[7] == NUL))
	{
	    new_unnamed |= CLIP_UNNAMED;
	    p += 7;
	}
	නැතහොත් නම් (STRNCMP(p, "unnamedplus", 11) == 0
					    && (p[11] == ',' || p[11] == NUL))
	{
	    new_unnamed |= CLIP_UNNAMED_PLUS;
	    p += 11;
	}
# ifdef FEAT_CLIPBOARD
	නැතහොත් නම් (STRNCMP(p, "autoselect", 10) == 0
					    && (p[10] == ',' || p[10] == NUL))
	{
	    new_autoselect_star = TRUE;
	    p += 10;
	}
	නැතහොත් නම් (STRNCMP(p, "autoselectplus", 14) == 0
					    && (p[14] == ',' || p[14] == NUL))
	{
	    new_autoselect_plus = TRUE;
	    p += 14;
	}
	නැතහොත් නම් (STRNCMP(p, "autoselectml", 12) == 0
					    && (p[12] == ',' || p[12] == NUL))
	{
	    new_autoselectml = TRUE;
	    p += 12;
	}
	නැතහොත් නම් (STRNCMP(p, "html", 4) == 0 && (p[4] == ',' || p[4] == NUL))
	{
	    new_html = TRUE;
	    p += 4;
	}
	නැතහොත් නම් (STRNCMP(p, "exclude:", 8) == 0 && new_exclude_prog == NULL)
	{
	    p += 8;
	    new_exclude_prog = vim_regcomp(p, RE_MAGIC);
	    නම් (new_exclude_prog == NULL)
		errmsg = e_invalid_argument;
	    bha_nga;
	}
# endif
	නැතහොත්
	{
	    errmsg = e_invalid_argument;
	    bha_nga;
	}
	නම් (*p == ',')
	    ++p;
    }
    නම් (errmsg == NULL)
    {
# ifdef FEAT_CLIPBOARD
	නම් (global_busy)
	    // clip_unnamed will be reset to clip_unnamed_saved
	    // at end_global_changes
	    clip_unnamed_saved = new_unnamed;
	නැතහොත්
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
# නම් defined(FEAT_GUI_GTK)
	නම් (gui.in_use)
	{
#  ifdef USE_GTK4
	    gui_gtk_update_selection_formats(&clip_plus);
	    gui_gtk_update_selection_formats(&clip_star);
#  නැතහොත්
	    gui_gtk_set_selection_targets((GdkAtom)GDK_SELECTION_PRIMARY);
	    gui_gtk_set_selection_targets((GdkAtom)clip_plus.gtk_sel_atom);
#  endif
#  ifdef FEAT_DND
	    gui_gtk_set_dnd_targets();
#  endif
	}
# endif
    }
    නැතහොත්
	vim_regfree(new_exclude_prog);

    ආපසු errmsg;
}

#endif // HAVE_CLIPMETHOD

#ifdef FEAT_CLIPBOARD_PROVIDER

/*
 * Check if a clipboard provider with given name is available. Returns 1 if available,
 * 0 if not available, and -1 on error
 */
    static පූර්ණ_සංඛ්‍යා
clip_provider_is_available(char_u *provider)
{
    dict_T	*providers = get_vim_var_dict(VV_CLIPPROVIDERS);
    typval_T	provider_tv = {0};
    callback_T	callback = {0};
    typval_T	rettv = {0};
    typval_T	func_tv = {0};
    පූර්ණ_සංඛ්‍යා		res = 0;

    නම් (dict_get_tv(providers, (var_na *)provider, &provider_tv) == FAIL
	    || provider_tv.v_type != VAR_DICT)
	// clipboard provider not defined
	ආපසු -1;

    නම් (dict_get_tv(provider_tv.vval.v_dict, "available", &func_tv) == FAIL)
    {
	clear_tv(&provider_tv);
	// If "available" function not specified assume always TRUE
	ආපසු 1;
    }

    නම් ((callback = get_callback(&func_tv)).cb_name == NULL)
	goto fail;

    නම් (call_callback(&callback, -1, &rettv, 0, NULL) == FAIL ||
	    (rettv.v_type != VAR_BOOL && rettv.v_type != VAR_NUMBER))
	goto fail;

    නම් (rettv.vval.v_number)
	res = 1;

    නම් (FALSE)
fail:
	res = -1;

    free_callback(&callback);
    clear_tv(&func_tv);
    clear_tv(&rettv);
    clear_tv(&provider_tv);

    ආපසු res;
}

/*
 * Get the specified callback "function" from the provider dictionary for
 * register "reg".
 */
    static පූර්ණ_සංඛ්‍යා
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

    නම් (dict_get_tv(providers, (var_na *)provider, &provider_tv) == FAIL)
	ආපසු FAIL;
    නැතහොත් නම් (provider_tv.v_type != VAR_DICT)
    {
	clear_tv(&provider_tv);
	ආපසු FAIL;
    }
    නැතහොත් නම් (dict_get_tv(
		provider_tv.vval.v_dict,
		(var_na *)function,
		&action_tv) == FAIL)
    {
	clear_tv(&provider_tv);
	ආපසු FAIL;
    }
    නැතහොත් නම් (action_tv.v_type != VAR_DICT)
    {
	clear_tv(&provider_tv);
	clear_tv(&action_tv);
	ආපසු FAIL;
    }
    නැතහොත් නම් (dict_get_tv(action_tv.vval.v_dict, (var_na *)reg, &func_tv) == FAIL)
    {
	clear_tv(&provider_tv);
	clear_tv(&action_tv);
	ආපසු FAIL;
    }
    නැතහොත් නම් ((cb = get_callback(&func_tv)).cb_name == NULL)
    {
	clear_tv(&provider_tv);
	clear_tv(&action_tv);
	clear_tv(&func_tv);
	ආපසු FAIL;
    }
    clear_tv(&provider_tv);
    clear_tv(&action_tv);

    // func_tv owns the function name, so we must make a copy for the callback
    set_callback(callback, &cb);
    clear_tv(&func_tv);
    ආපසු OK;
}

    static void
clip_provider_copy(char_u *reg, char_u *provider)
{
    static bool	recursive = false;
    callback_T	callback;
    typval_T	rettv;
    typval_T	argvars[4];
    yankreg_T	*y_ptr;
    char_u	type[2 + NUMBUFLEN] = {0};
    list_T	*list = NULL;

    නම් (recursive)
	ආපසු;

    නම් (clip_provider_get_callback(
		reg,
		provider,
		(char_u *)"copy",
		&callback) == FAIL)
	ආපසු;

    // Convert register type into a string
    නම් (*reg == '+')
	y_ptr = get_y_register(REAL_PLUS_REGISTER);
    නැතහොත්
	y_ptr = get_y_register(STAR_REGISTER);

    switch (y_ptr->y_type)
    {
	pak_sa MCHAR:
	    type[0] = 'v';
	    bha_nga;
	pak_sa MLINE:
	    type[0] = 'V';
	    bha_nga;
	pak_sa MBLOCK:
	    sprintf((var_na *)type, "%c%d", Ctrl_V, y_ptr->y_width + 1);
	    bha_nga;
	default:
	    type[0] = 0;
	    bha_nga;
    }

    argvars[0].v_type = VAR_STRING;
    argvars[0].vval.v_string = reg;

    argvars[1].v_type = VAR_STRING;
    argvars[1].vval.v_string = type;

    // Get register contents by creating a list of lines
    list = list_alloc();

    නම් (list == NULL)
    {
	free_callback(&callback);
	ආපසු;
    }

    සඳහා (පූර්ණ_සංඛ්‍යා i = 0; i < y_ptr->y_size; i++)
	නම් (list_append_string(list, y_ptr->y_array[i].string,
	    (පූර්ණ_සංඛ්‍යා)y_ptr->y_array[i].length) == FAIL)
	{
	    free_callback(&callback);
	    list_unref(list);
	    ආපසු;
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

    static void
clip_provider_paste(char_u *reg, char_u *provider)
{
    static bool	recursive = false;
    callback_T	callback;
    typval_T	argvars[2];
    typval_T	rettv;
    පූර්ණ_සංඛ්‍යා		ret;
    char_u	*reg_type;
    list_T	*lines;

    නම් (recursive)
	ආපසු;

    නම් (clip_provider_get_callback(
		reg,
		provider,
		(char_u *)"paste",
		&callback) == FAIL)
	ආපසු;

    argvars[0].v_type = VAR_STRING;
    argvars[0].vval.v_string = reg;

    argvars[1].v_type = VAR_UNKNOWN;

    textlock++;
    recursive = true;
    ret = call_callback(&callback, -1, &rettv, 1, argvars);
    recursive = false;
    textlock--;

    නම් (ret == FAIL)
	goto exit;
    නැතහොත් නම් (rettv.v_type == VAR_TUPLE
	    && TUPLE_LEN(rettv.vval.v_tuple) == 2
	    && TUPLE_ITEM(rettv.vval.v_tuple, 0)->v_type == VAR_STRING
	    && TUPLE_ITEM(rettv.vval.v_tuple, 1)->v_type == VAR_LIST)
    {
	reg_type = TUPLE_ITEM(rettv.vval.v_tuple, 0)->vval.v_string;
	lines = TUPLE_ITEM(rettv.vval.v_tuple, 1)->vval.v_list;
    }
    නැතහොත් නම් (rettv.v_type == VAR_LIST
	    && rettv.vval.v_list->lv_len == 2
	    && rettv.vval.v_list->lv_first->li_tv.v_type == VAR_STRING
	    && rettv.vval.v_list->lv_first->li_next->li_tv.v_type == VAR_LIST)
    {
	reg_type = rettv.vval.v_list->lv_first->li_tv.vval.v_string;
	lines = rettv.vval.v_list->lv_first->li_next->li_tv.vval.v_list;
    }
    නැතහොත්
	goto exit;

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
	පූර්ණ_සංඛ්‍යා		len;

	// If the list is NULL handle like an empty list.
	len = lines == NULL ? 0 : lines->lv_len;

	// First half: use for pointers to result lines; second half: use for
	// pointers to allocated copies.
	lstval = ALLOC_MULT(char_u *, (len + 1) * 2);
	නම් (lstval == NULL)
	    goto exit;
	curval = lstval;
	allocval = lstval + len + 2;
	curallocval = allocval;

	නම් (lines != NULL)
	{
	    CHECK_LIST_MATERIALIZE(lines);
	    FOR_ALL_LIST_ITEMS(lines, li)
	    {
		strval = tv_get_string_buf_chk(&li->li_tv, buf);
		නම් (strval == NULL)
		    goto free_lstval;
		නම් (strval == buf)
		{
		    // Need to make a copy, next tv_get_string_buf_chk() will
		    // overwrite the string.
		    strval = vim_strsave(buf);
		    නම් (strval == NULL)
			goto free_lstval;
		    *curallocval++ = strval;
		}
		*curval++ = strval;
	    }
	}
	*curval++ = NULL;

	නම් (*reg_type != NUL && (STRLEN(reg_type) == 0
		|| get_yank_type(&reg_type, &yank_type, &block_len) == FAIL))
	{
	    emsg(e_invalid_argument);
	    goto free_lstval;
	}

	නම් (*reg == '+')
	    y_ptr = get_y_register(REAL_PLUS_REGISTER);
	නැතහොත්
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
	අතරතුර (curallocval > allocval)
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
static පූර්ණ_සංඛ්‍යා star_pause_count = -2, plus_pause_count = -2;

    void
call_clip_provider_request(පූර්ණ_සංඛ්‍යා reg)
{
    නම් (clipmethod != CLIPMETHOD_PROVIDER)
	ආපසු;

    නම් (reg == '+' && plus_pause_count < 0)
    {
	නම් (plus_pause_count == -1)
	    plus_pause_count = 1;
	clip_provider_paste((char_u *)"+", clip_provider);
    }
    නැතහොත් නම් (reg == '*' && star_pause_count < 0)
    {
	නම් (star_pause_count == -1)
	    star_pause_count = 1;
	clip_provider_paste((char_u *)"*", clip_provider);
    }
    නැතහොත්
	ආපසු;
}

    void
call_clip_provider_set(පූර්ණ_සංඛ්‍යා reg)
{
    නම් (clipmethod != CLIPMETHOD_PROVIDER)
	ආපසු;

    නම් (reg == '+' && plus_pause_count < 0)
    {
	නම් (plus_pause_count == -1)
	    plus_pause_count = 1;
	clip_provider_copy((char_u *)"+", clip_provider);
    }
    නැතහොත් නම් (reg == '*' && star_pause_count < 0)
    {
	නම් (star_pause_count == -1)
	    star_pause_count = 1;
	clip_provider_copy((char_u *)"*", clip_provider);
    }
}

/*
 * Makes it so that the next provider call is only done once any calls after are
 * ignored, until dec_clip_provider is called the same number of times after
 * again. Note that this is per clipboard register ("+", "*")
 */
    void
inc_clip_provider(void)
{
    plus_pause_count = (plus_pause_count == -2
	|| plus_pause_count == -1) ? -1 : plus_pause_count + 1;
    star_pause_count = (star_pause_count == -2
	|| star_pause_count == -1) ? -1 : star_pause_count + 1;
}

    void
dec_clip_provider(void)
{
    නම් (plus_pause_count != -2)
	plus_pause_count = plus_pause_count == -1 ? -1 : plus_pause_count - 1;
    නම් (star_pause_count != -2)
	star_pause_count = star_pause_count == -1 ? -1 : star_pause_count - 1;

    නම් (plus_pause_count == 0 || plus_pause_count == -1)
	plus_pause_count = -2;
    නම් (star_pause_count == 0 || star_pause_count == -1)
	star_pause_count = -2;
}

#endif // FEAT_CLIPBOARD_PROVIDER

#නම් defined(FEAT_WAYLAND_CLIPBOARD) || (defined(FEAT_GUI_GTK) && defined(USE_GTK4))
/*
 * If "vim" is TRUE, then get the motion type. If "vimenc" is TRUE, then get the
 * motion type and also convert "*buf". "buf" and "len_store" will be updated to
 * reflect the actual contents, but should be set beforehand with the initial
 * contents. Note that if "*tofree" is not NULL, use vim_free() on "*buf",
 * otherwise use the free func for which "*buf" was allocated with. In both
 * cases use vim_free() on "*tofree". Returns OK on success and FAIL on failure.
 */
    පූර්ණ_සංඛ්‍යා
clip_convert_data(
	char_u	**buf,
	long	*len_store,
	පූර්ණ_සංඛ්‍යා	*motion,
	bool	vim,
	bool	vimenc,
	char_u	**tofree)
{
    char_u  *final = *buf;
    char_u  *enc;
    long    len = *len_store;

    නම් (vim && len >= 2)
    {
	*motion = *final++;
	len--;
    }
    නැතහොත් නම් (vimenc && len >= 3)
    {
	vimconv_T   conv;
	පූර්ණ_සංඛ්‍යා	    convlen;

	// First byte is motion type
	*motion = *final++;
	len--;

	// Get encoding of selection
	enc = final;

	// Skip the encoding type including null terminator in final text
	final = memchr(final, NUL, len);
	නම් (final == NULL)
	    ආපසු FAIL;
	final++; // Skip NUL

	// Subtract pointers to get length of encoding;
	len -= final - enc;

	conv.vc_type = CONV_NONE;
	convert_setup(&conv, enc, p_enc);
	නම් (conv.vc_type != CONV_NONE)
	{
	   char_u *tmp;

	   convlen = len;
	   tmp = string_convert(&conv, final, &convlen);
	   len = convlen;
	   නම් (tmp != NULL)
	   {
		final = tmp;
		*tofree = final;
	   }
	   convert_setup(&conv, NULL, NULL);
	}
    }
    *buf = final;
    *len_store = len;
    ආපසු OK;
}
#endif
