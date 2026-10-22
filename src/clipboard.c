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

#അന്തര്ഭാവ "vim.h"

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
സ്ഥിതം പൂർണ്ണസംഖ്യ clip_provider_is_available(char_u *provider);
#endif

#എങ്കിൽ defined(FEAT_CLIPBOARD)

# എങ്കിൽ defined(FEAT_WAYLAND_CLIPBOARD)

#  include "wayland.h"

// Represents either the regular or primary selection
തരനിർവചനം ഘടന {
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
തരനിർവചനം ഘടന {
    vwl_seat_T *seat;

    clip_wl_selection_T regular;
    clip_wl_selection_T primary;
} clip_wl_T;

// Mime types we support sending and receiving
// Mimes with a lower index in the array are prioritized first when we are
// receiving data.
സ്ഥിതം സ്ഥിരം അക്ഷരം *supported_mimes[] = {
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

സ്ഥിതം ശൂന്യം
clip_wl_receive_data(Clipboard_T *cbd, സ്ഥിരം അക്ഷരം *mime_type, പൂർണ്ണസംഖ്യ fd);
സ്ഥിതം ശൂന്യം clip_wl_request_selection(Clipboard_T *cbd);
സ്ഥിതം പൂർണ്ണസംഖ്യ clip_wl_own_selection(Clipboard_T *cbd);
സ്ഥിതം ശൂന്യം clip_wl_lose_selection(Clipboard_T *cbd);
സ്ഥിതം ശൂന്യം clip_wl_set_selection(Clipboard_T *cbd);

#  എങ്കിൽ defined(USE_SYSTEM)
സ്ഥിതം bool clip_wl_owner_exists(Clipboard_T *cbd);
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
    ശൂന്യം
clip_init(പൂർണ്ണസംഖ്യ can_use)
{
    Clipboard_T *cb;

    cb = &clip_star;
    വേണ്ടി (;;)
    {
	// No need to init again if cbd is already available
	എങ്കിൽ (can_use && cb->available)
	    പോകുക skip;

	cb->available  = can_use;
	cb->owned      = FALSE;
	cb->start.lnum = 0;
	cb->start.col  = 0;
	cb->end.lnum   = 0;
	cb->end.col    = 0;
	cb->state      = SELECT_CLEARED;

skip:
	എങ്കിൽ (cb == &clip_plus)
	    നിർത്തുക;
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
    ശൂന്യം
clip_update_selection(Clipboard_T *clip)
{
    pos_T	    start, end;

    // If visual mode is only due to a redo command ("."), then ignore it
    എങ്കിൽ (!redo_VIsual_busy && VIsual_active && (State & MODE_NORMAL))
    {
	എങ്കിൽ (LT_POS(VIsual, curwin->w_cursor))
	{
	    start = VIsual;
	    end = curwin->w_cursor;
	    എങ്കിൽ (has_mbyte)
		end.col += (*mb_ptr2len)(ml_get_cursor()) - 1;
	}
	അല്ലെങ്കിൽ
	{
	    start = curwin->w_cursor;
	    end = VIsual;
	}
	എങ്കിൽ (!EQUAL_POS(clip->start, start)
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

    സ്ഥിതം പൂർണ്ണസംഖ്യ
clip_gen_own_selection(Clipboard_T *cbd)
{
# എങ്കിൽ defined(FEAT_XCLIPBOARD) || defined(FEAT_WAYLAND_CLIPBOARD)
#  ifdef FEAT_GUI
    എങ്കിൽ (gui.in_use)
	തിരികെ clip_mch_own_selection(cbd);
    അല്ലെങ്കിൽ
#  endif
    {
	എങ്കിൽ (clipmethod == CLIPMETHOD_WAYLAND)
	{
#  ifdef FEAT_WAYLAND_CLIPBOARD
	    തിരികെ clip_wl_own_selection(cbd);
#  endif
	}
	അല്ലെങ്കിൽ എങ്കിൽ (clipmethod == CLIPMETHOD_X11)
	{
#  ifdef FEAT_XCLIPBOARD
	    തിരികെ clip_xterm_own_selection(cbd);
#  endif
	}
    }
    തിരികെ FAIL;
# അല്ലെങ്കിൽ
    തിരികെ clip_mch_own_selection(cbd);
# endif
}

    ശൂന്യം
clip_own_selection(Clipboard_T *cbd)
{
    /*
     * Also want to check somehow that we are reading from the keyboard rather
     * than a mapping etc.
     */
# എങ്കിൽ defined(FEAT_X11) || defined(FEAT_WAYLAND_CLIPBOARD)
    // Always own the selection, we might have lost it without being
    // notified, e.g. during a ":sh" command.
    എങ്കിൽ (cbd->available)
    {
	പൂർണ്ണസംഖ്യ was_owned = cbd->owned;

	cbd->owned = (clip_gen_own_selection(cbd) == OK);
	എങ്കിൽ (!was_owned && (cbd == &clip_star || cbd == &clip_plus))
	{
	    // May have to show a different kind of highlighting for the
	    // selected area.  There is no specific redraw command for this,
	    // just redraw all windows on the current buffer.
	    എങ്കിൽ (cbd->owned
		    && (get_real_state() == MODE_VISUAL
					    || get_real_state() == MODE_SELECT)
		    && (cbd == &clip_star ? clip_isautosel_star()
						      : clip_isautosel_plus())
		    && HL_ATTR(HLF_V) != HL_ATTR(HLF_VNC))
		redraw_curbuf_later(UPD_INVERTED_ALL);
	}
    }
# അല്ലെങ്കിൽ
    // Only own the clipboard when we didn't own it yet.
    എങ്കിൽ (!cbd->owned && cbd->available)
	cbd->owned = (clip_gen_own_selection(cbd) == OK);
# endif
}

    സ്ഥിതം ശൂന്യം
clip_gen_lose_selection(Clipboard_T *cbd)
{
# എങ്കിൽ defined(FEAT_XCLIPBOARD) || defined(FEAT_WAYLAND_CLIPBOARD)
#  ifdef FEAT_GUI
    എങ്കിൽ (gui.in_use)
	clip_mch_lose_selection(cbd);
    അല്ലെങ്കിൽ
#  endif
    {
	എങ്കിൽ (clipmethod == CLIPMETHOD_WAYLAND)
	{
#  ifdef FEAT_WAYLAND_CLIPBOARD
	    clip_wl_lose_selection(cbd);
#  endif
	}
	അല്ലെങ്കിൽ എങ്കിൽ (clipmethod == CLIPMETHOD_X11)
	{
#  ifdef FEAT_XCLIPBOARD
	    clip_xterm_lose_selection(cbd);
#  endif
	}
    }
# അല്ലെങ്കിൽ
    clip_mch_lose_selection(cbd);
# endif
}

    ശൂന്യം
clip_lose_selection(Clipboard_T *cbd)
{
# ifdef FEAT_X11
    പൂർണ്ണസംഖ്യ	    was_owned = cbd->owned;
# endif
    പൂർണ്ണസംഖ്യ     visual_selection = FALSE;

    എങ്കിൽ (cbd == &clip_star || cbd == &clip_plus)
	visual_selection = TRUE;

    clip_free_selection(cbd);
    cbd->owned = FALSE;
    എങ്കിൽ (visual_selection)
	clip_clear_selection(cbd);
    clip_gen_lose_selection(cbd);
# ifdef FEAT_X11
    എങ്കിൽ (visual_selection)
    {
	// May have to show a different kind of highlighting for the selected
	// area.  There is no specific redraw command for this, just redraw all
	// windows on the current buffer.
	എങ്കിൽ (was_owned
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

    സ്ഥിതം ശൂന്യം
clip_copy_selection(Clipboard_T *clip)
{
    എങ്കിൽ (VIsual_active && (State & MODE_NORMAL) && clip->available)
    {
	clip_update_selection(clip);
	clip_free_selection(clip);
	clip_own_selection(clip);
	എങ്കിൽ (clip->owned)
	    clip_get_selection(clip);
	clip_gen_set_selection(clip);
    }
}

/*
 * Save and restore clip_unnamed before doing possibly many changes. This
 * prevents accessing the clipboard very often which might slow down Vim
 * considerably.
 */
സ്ഥിതം പൂർണ്ണസംഖ്യ global_change_എണ്ണം = 0; // if set, inside a start_global_changes
സ്ഥിതം പൂർണ്ണസംഖ്യ clipboard_needs_update = FALSE; // clipboard needs to be updated
സ്ഥിതം പൂർണ്ണസംഖ്യ clip_did_set_selection = TRUE;

/*
 * Save clip_unnamed and reset it.
 */
    ശൂന്യം
start_global_changes(ശൂന്യം)
{
    എങ്കിൽ (++global_change_എണ്ണം > 1)
	തിരികെ;
    clip_unnamed_saved = clip_unnamed;
    clipboard_needs_update = FALSE;

    എങ്കിൽ (clip_did_set_selection)
    {
	clip_unnamed = 0;
	clip_did_set_selection = FALSE;
    }
}

/*
 * Return TRUE if setting the clipboard was postponed, it already contains the
 * right text.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
is_clipboard_needs_update(ശൂന്യം)
{
    തിരികെ clipboard_needs_update;
}

/*
 * Restore clip_unnamed and set the selection when needed.
 */
    ശൂന്യം
end_global_changes(ശൂന്യം)
{
    എങ്കിൽ (--global_change_എണ്ണം > 0)
	// recursive
	തിരികെ;
    എങ്കിൽ (!clip_did_set_selection)
    {
	clip_did_set_selection = TRUE;
	clip_unnamed = clip_unnamed_saved;
	clip_unnamed_saved = 0;
	എങ്കിൽ (clipboard_needs_update)
	{
	    // only store something in the clipboard,
	    // if we have yanked anything to it
	    എങ്കിൽ (clip_unnamed & CLIP_UNNAMED)
	    {
		clip_own_selection(&clip_star);
		clip_gen_set_selection(&clip_star);
	    }
	    എങ്കിൽ (clip_unnamed & CLIP_UNNAMED_PLUS)
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
    ശൂന്യം
clip_auto_select(ശൂന്യം)
{
    എങ്കിൽ (clip_isautosel_star())
	clip_copy_selection(&clip_star);
    എങ്കിൽ (clip_isautosel_plus())
	clip_copy_selection(&clip_plus);
}

/*
 * Return TRUE if automatic selection of Visual area is desired for the *
 * register.
 */
    പൂർണ്ണസംഖ്യ
clip_isautosel_star(ശൂന്യം)
{
# ifdef FEAT_CLIPBOARD_PROVIDER
    എങ്കിൽ (clipmethod == CLIPMETHOD_PROVIDER)
	തിരികെ false;
# endif
# ifdef FEAT_GUI
    എങ്കിൽ (gui.in_use)
	തിരികെ vim_strchr(p_go, GO_ASEL) != ശൂന്യ
	    && vim_strchr(p_go, GO_ASELPLUS) == ശൂന്യ;
# endif
    തിരികെ clip_autoselect_star;
}

/*
 * Return TRUE if automatic selection of Visual area is desired for the +
 * register.
 */
    പൂർണ്ണസംഖ്യ
clip_isautosel_plus(ശൂന്യം)
{
# ifdef FEAT_CLIPBOARD_PROVIDER
    എങ്കിൽ (clipmethod == CLIPMETHOD_PROVIDER)
	തിരികെ false;
# endif
# ifdef FEAT_GUI
    എങ്കിൽ (gui.in_use)
	തിരികെ vim_strchr(p_go, GO_ASELPLUS) != ശൂന്യ;
# endif
    തിരികെ clip_autoselect_plus;
}


/*
 * Stuff for general mouse selection, without using Visual mode.
 */

/*
 * Compare two screen positions ala strcmp()
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
clip_compare_pos(
    പൂർണ്ണസംഖ്യ		row1,
    പൂർണ്ണസംഖ്യ		col1,
    പൂർണ്ണസംഖ്യ		row2,
    പൂർണ്ണസംഖ്യ		col2)
{
    എങ്കിൽ (row1 > row2) തിരികെ(1);
    എങ്കിൽ (row1 < row2) തിരികെ(-1);
    എങ്കിൽ (col1 > col2) തിരികെ(1);
    എങ്കിൽ (col1 < col2) തിരികെ(-1);
    തിരികെ(0);
}

// "how" flags for clip_invert_area()
# define CLIP_CLEAR	1
# define CLIP_SET	2
# define CLIP_TOGGLE	3

/*
 * Invert or un-invert a rectangle of the screen.
 * "invert" is true if the result is inverted.
 */
    സ്ഥിതം ശൂന്യം
clip_invert_rectangle(
	Clipboard_T	*cbd UNUSED,
	പൂർണ്ണസംഖ്യ		row_arg,
	പൂർണ്ണസംഖ്യ		col_arg,
	പൂർണ്ണസംഖ്യ		height_arg,
	പൂർണ്ണസംഖ്യ		width_arg,
	പൂർണ്ണസംഖ്യ		invert)
{
    പൂർണ്ണസംഖ്യ		row = row_arg;
    പൂർണ്ണസംഖ്യ		col = col_arg;
    പൂർണ്ണസംഖ്യ		height = height_arg;
    പൂർണ്ണസംഖ്യ		width = width_arg;

# ifdef FEAT_PROP_POPUP
    // this goes on top of all popup windows
    screen_zindex = CLIP_ZINDEX;

    എങ്കിൽ (col < cbd->min_col)
    {
	width -= cbd->min_col - col;
	col = cbd->min_col;
    }
    എങ്കിൽ (width > cbd->max_col - col)
	width = cbd->max_col - col;
    എങ്കിൽ (row < cbd->min_row)
    {
	height -= cbd->min_row - row;
	row = cbd->min_row;
    }
    എങ്കിൽ (height > cbd->max_row - row + 1)
	height = cbd->max_row - row + 1;
# endif
# ifdef FEAT_GUI
    എങ്കിൽ (gui.in_use)
	gui_mch_invert_rectangle(row, col, height, width);
    അല്ലെങ്കിൽ
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
    സ്ഥിതം ശൂന്യം
clip_invert_area(
	Clipboard_T	*cbd,
	പൂർണ്ണസംഖ്യ		row1,
	പൂർണ്ണസംഖ്യ		col1,
	പൂർണ്ണസംഖ്യ		row2,
	പൂർണ്ണസംഖ്യ		col2,
	പൂർണ്ണസംഖ്യ		how)
{
    പൂർണ്ണസംഖ്യ		invert = FALSE;
    പൂർണ്ണസംഖ്യ		max_col;

# ifdef FEAT_PROP_POPUP
    max_col = cbd->max_col - 1;
# അല്ലെങ്കിൽ
    max_col = Columns - 1;
# endif

    എങ്കിൽ (how == CLIP_SET)
	invert = TRUE;

    // Swap the from and to positions so the from is always before
    എങ്കിൽ (clip_compare_pos(row1, col1, row2, col2) > 0)
    {
	പൂർണ്ണസംഖ്യ tmp_row, tmp_col;

	tmp_row = row1;
	tmp_col = col1;
	row1	= row2;
	col1	= col2;
	row2	= tmp_row;
	col2	= tmp_col;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (how == CLIP_TOGGLE)
	invert = TRUE;

    // If all on the same line, do it the easy way
    എങ്കിൽ (row1 == row2)
    {
	clip_invert_rectangle(cbd, row1, col1, 1, col2 - col1, invert);
    }
    അല്ലെങ്കിൽ
    {
	// Handle a piece of the first line
	എങ്കിൽ (col1 > 0)
	{
	    clip_invert_rectangle(cbd, row1, col1, 1,
						  (പൂർണ്ണസംഖ്യ)Columns - col1, invert);
	    row1++;
	}

	// Handle a piece of the last line
	എങ്കിൽ (col2 < max_col)
	{
	    clip_invert_rectangle(cbd, row2, 0, 1, col2, invert);
	    row2--;
	}

	// Handle the rectangle that's left
	എങ്കിൽ (row2 >= row1)
	    clip_invert_rectangle(cbd, row1, 0, row2 - row1 + 1,
							 (പൂർണ്ണസംഖ്യ)Columns, invert);
    }
}

/*
 * Start, continue or end a modeless selection.  Used when editing the
 * command-line, in the cmdline window and when the mouse is in a popup window.
 */
    ശൂന്യം
clip_modeless(പൂർണ്ണസംഖ്യ button, പൂർണ്ണസംഖ്യ is_click, പൂർണ്ണസംഖ്യ is_drag)
{
    പൂർണ്ണസംഖ്യ		repeat;

    repeat = ((clip_star.mode == SELECT_MODE_CHAR
		|| clip_star.mode == SELECT_MODE_വരി)
					      && (mod_mask & MOD_MASK_2CLICK))
	    || (clip_star.mode == SELECT_MODE_WORD
					     && (mod_mask & MOD_MASK_3CLICK));
    എങ്കിൽ (is_click && button == MOUSE_വലത്)
    {
	// Right mouse button: If there was no selection, start one.
	// Otherwise extend the existing selection.
	എങ്കിൽ (clip_star.state == SELECT_CLEARED)
	    clip_start_selection(mouse_col, mouse_row, FALSE);
	clip_process_selection(button, mouse_col, mouse_row, repeat);
    }
    അല്ലെങ്കിൽ എങ്കിൽ (is_click)
	clip_start_selection(mouse_col, mouse_row, repeat);
    അല്ലെങ്കിൽ എങ്കിൽ (is_drag)
    {
	// Don't try extending a selection if there isn't one.  Happens when
	// button-down is in the cmdline and them moving mouse upwards.
	എങ്കിൽ (clip_star.state != SELECT_CLEARED)
	    clip_process_selection(button, mouse_col, mouse_row, repeat);
    }
    അല്ലെങ്കിൽ // release
	clip_process_selection(MOUSE_RELEASE, mouse_col, mouse_row, FALSE);
}

/*
 * Update the currently selected region by adding and/or subtracting from the
 * beginning or end and inverting the changed area(s).
 */
    സ്ഥിതം ശൂന്യം
clip_update_modeless_selection(
    Clipboard_T    *cb,
    പൂർണ്ണസംഖ്യ		    row1,
    പൂർണ്ണസംഖ്യ		    col1,
    പൂർണ്ണസംഖ്യ		    row2,
    പൂർണ്ണസംഖ്യ		    col2)
{
    // See if we changed at the beginning of the selection
    എങ്കിൽ (row1 != cb->start.lnum || col1 != (പൂർണ്ണസംഖ്യ)cb->start.col)
    {
	clip_invert_area(cb, row1, col1, (പൂർണ്ണസംഖ്യ)cb->start.lnum, cb->start.col,
								 CLIP_TOGGLE);
	cb->start.lnum = row1;
	cb->start.col  = col1;
    }

    // See if we changed at the end of the selection
    എങ്കിൽ (row2 != cb->end.lnum || col2 != (പൂർണ്ണസംഖ്യ)cb->end.col)
    {
	clip_invert_area(cb, (പൂർണ്ണസംഖ്യ)cb->end.lnum, cb->end.col, row2, col2,
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

    സ്ഥിതം ശൂന്യം
clip_get_word_boundaries(Clipboard_T *cb, പൂർണ്ണസംഖ്യ row, പൂർണ്ണസംഖ്യ col)
{
    പൂർണ്ണസംഖ്യ		start_class;
    പൂർണ്ണസംഖ്യ		temp_col;
    char_u	*p;
    പൂർണ്ണസംഖ്യ		mboff;

    എങ്കിൽ (row >= screen_Rows || col >= screen_Columns || Screen_വരികൾ == ശൂന്യ)
	തിരികെ;

    p = Screen_വരികൾ + വരി_Offset[row];
    // Correct for starting in the right half of a double-wide char
    എങ്കിൽ (enc_dbcs != 0)
	col -= dbcs_screen_head_off(p, p + col);
    അല്ലെങ്കിൽ എങ്കിൽ (enc_utf8 && p[col] == 0)
	--col;
    start_class = CHAR_CLASS(p[col]);

    temp_col = col;
    വേണ്ടി ( ; temp_col > 0; temp_col--)
	എങ്കിൽ (enc_dbcs != 0
		   && (mboff = dbcs_screen_head_off(p, p + temp_col - 1)) > 0)
	    temp_col -= mboff;
	അല്ലെങ്കിൽ എങ്കിൽ (CHAR_CLASS(p[temp_col - 1]) != start_class
		&& !(enc_utf8 && p[temp_col - 1] == 0))
	    നിർത്തുക;
    cb->word_start_col = temp_col;

    temp_col = col;
    വേണ്ടി ( ; temp_col < screen_Columns; temp_col++)
	എങ്കിൽ (enc_dbcs != 0 && dbcs_ptr2cells(p + temp_col) == 2)
	    ++temp_col;
	അല്ലെങ്കിൽ എങ്കിൽ (CHAR_CLASS(p[temp_col]) != start_class
		&& !(enc_utf8 && p[temp_col] == 0))
	    നിർത്തുക;
    cb->word_end_col = temp_col;
}

/*
 * Find the column position for the last non-whitespace character on the given
 * line at or before start_col.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
clip_get_വരി_end(Clipboard_T *cbd UNUSED, പൂർണ്ണസംഖ്യ row)
{
    പൂർണ്ണസംഖ്യ	    i;

    എങ്കിൽ (row >= screen_Rows || Screen_വരികൾ == ശൂന്യ)
	തിരികെ 0;
    വേണ്ടി (i =
# ifdef FEAT_PROP_POPUP
	    cbd->max_col;
# അല്ലെങ്കിൽ
	    screen_Columns;
# endif
			    i > 0; i--)
	എങ്കിൽ (Screen_വരികൾ[വരി_Offset[row] + i - 1] != ' ')
	    നിർത്തുക;
    തിരികെ i;
}

/*
 * Start the selection
 */
    ശൂന്യം
clip_start_selection(പൂർണ്ണസംഖ്യ col, പൂർണ്ണസംഖ്യ row, പൂർണ്ണസംഖ്യ repeated_click)
{
    Clipboard_T	*cb = &clip_star;
# ifdef FEAT_PROP_POPUP
    win_T	*wp;
    പൂർണ്ണസംഖ്യ		row_cp = row;
    പൂർണ്ണസംഖ്യ		col_cp = col;

    wp = mouse_find_win(&row_cp, &col_cp, FIND_POPUP);
    എങ്കിൽ (wp != ശൂന്യ && WIN_IS_POPUP(wp)
				  && popup_is_in_scrollbar(wp, row_cp, col_cp))
	// click or double click in scrollbar does not start a selection
	തിരികെ;
# endif

    എങ്കിൽ (cb->state == SELECT_DONE)
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
    എങ്കിൽ (wp != ശൂന്യ && WIN_IS_POPUP(wp))
    {
	// Click in a popup window restricts selection to that window,
	// excluding the border.
	cb->min_col = wp->w_wincol + wp->w_popup_border[3];
	cb->max_col = wp->w_wincol + popup_width(wp)
				 - wp->w_popup_border[1] - wp->w_has_scrollbar;
	എങ്കിൽ (cb->max_col > screen_Columns)
	    cb->max_col = screen_Columns;
	cb->min_row = wp->w_winrow + wp->w_popup_border[0];
	cb->max_row = wp->w_winrow + popup_height(wp) - 1
						   - wp->w_popup_border[2];
    }
    അല്ലെങ്കിൽ
    {
	cb->min_col = 0;
	cb->max_col = screen_Columns;
	cb->min_row = 0;
	cb->max_row = screen_Rows;
    }
# endif

    എങ്കിൽ (repeated_click)
    {
	എങ്കിൽ (++cb->mode > SELECT_MODE_വരി)
	    cb->mode = SELECT_MODE_CHAR;
    }
    അല്ലെങ്കിൽ
	cb->mode = SELECT_MODE_CHAR;

# ifdef FEAT_GUI
    // clear the cursor until the selection is made
    എങ്കിൽ (gui.in_use)
	gui_undraw_cursor();
# endif

    തിരഞ്ഞെടുപ്പ് (cb->mode)
    {
	അവസ്ഥ SELECT_MODE_CHAR:
	    cb->origin_start_col = cb->start.col;
	    cb->word_end_col = clip_get_വരി_end(cb, (പൂർണ്ണസംഖ്യ)cb->start.lnum);
	    നിർത്തുക;

	അവസ്ഥ SELECT_MODE_WORD:
	    clip_get_word_boundaries(cb, (പൂർണ്ണസംഖ്യ)cb->start.lnum, cb->start.col);
	    cb->origin_start_col = cb->word_start_col;
	    cb->origin_end_col	 = cb->word_end_col;

	    clip_invert_area(cb, (പൂർണ്ണസംഖ്യ)cb->start.lnum, cb->word_start_col,
			    (പൂർണ്ണസംഖ്യ)cb->end.lnum, cb->word_end_col, CLIP_SET);
	    cb->start.col = cb->word_start_col;
	    cb->end.col   = cb->word_end_col;
	    നിർത്തുക;

	അവസ്ഥ SELECT_MODE_വരി:
	    clip_invert_area(cb, (പൂർണ്ണസംഖ്യ)cb->start.lnum, 0, (പൂർണ്ണസംഖ്യ)cb->start.lnum,
			    (പൂർണ്ണസംഖ്യ)Columns, CLIP_SET);
	    cb->start.col = 0;
	    cb->end.col   = Columns;
	    നിർത്തുക;
    }

    cb->prev = cb->start;

# ifdef DEBUG_SELECTION
    printf("Selection started at (%ld,%d)\n", cb->start.lnum, cb->start.col);
# endif
}

/*
 * Continue processing the selection
 */
    ശൂന്യം
clip_process_selection(
    പൂർണ്ണസംഖ്യ		button,
    പൂർണ്ണസംഖ്യ		col,
    പൂർണ്ണസംഖ്യ		row,
    int_u	repeated_click)
{
    Clipboard_T	*cb = &clip_star;
    പൂർണ്ണസംഖ്യ		diff;
    പൂർണ്ണസംഖ്യ		slen = 1;	// cursor shape width

    എങ്കിൽ (button == MOUSE_RELEASE)
    {
	എങ്കിൽ (cb->state != SELECT_IN_PROGRESS)
	    തിരികെ;

	// Check to make sure we have something selected
	എങ്കിൽ (cb->start.lnum == cb->end.lnum && cb->start.col == cb->end.col)
	{
# ifdef FEAT_GUI
	    എങ്കിൽ (gui.in_use)
		gui_update_cursor(FALSE, FALSE);
# endif
	    cb->state = SELECT_CLEARED;
	    തിരികെ;
	}

# ifdef DEBUG_SELECTION
	printf("Selection ended: (%ld,%d) to (%ld,%d)\n", cb->start.lnum,
		cb->start.col, cb->end.lnum, cb->end.col);
# endif
	എങ്കിൽ (clip_isautosel_star() || clip_isautosel_plus()
		|| (
# ifdef FEAT_GUI
		    gui.in_use ? (vim_strchr(p_go, GO_ASELML) != ശൂന്യ) :
# endif
		    clip_autoselectml))
	    clip_copy_modeless_selection(FALSE);
# ifdef FEAT_GUI
	എങ്കിൽ (gui.in_use)
	    gui_update_cursor(FALSE, FALSE);
# endif

	cb->state = SELECT_DONE;
	തിരികെ;
    }

    row = check_row(row);
    col = check_col(col);
    col = mb_fix_col(col, row);

    എങ്കിൽ (col == (പൂർണ്ണസംഖ്യ)cb->prev.col && row == cb->prev.lnum && !repeated_click)
	തിരികെ;

    /*
     * When extending the selection with the right mouse button, swap the
     * start and end if the position is before half the selection
     */
    എങ്കിൽ (cb->state == SELECT_DONE && button == MOUSE_വലത്)
    {
	/*
	 * If the click is before the start, or the click is inside the
	 * selection and the start is the closest side, set the origin to the
	 * end of the selection.
	 */
	എങ്കിൽ (clip_compare_pos(row, col, (പൂർണ്ണസംഖ്യ)cb->start.lnum, cb->start.col) < 0
		|| (clip_compare_pos(row, col,
					   (പൂർണ്ണസംഖ്യ)cb->end.lnum, cb->end.col) < 0
		    && (((cb->start.lnum == cb->end.lnum
			    && cb->end.col - col > col - cb->start.col))
			|| ((diff = (cb->end.lnum - row) -
						   (row - cb->start.lnum)) > 0
			    || (diff == 0 && col < (പൂർണ്ണസംഖ്യ)(cb->start.col +
							 cb->end.col) / 2)))))
	{
	    cb->origin_row = (short_u)cb->end.lnum;
	    cb->origin_start_col = cb->end.col - 1;
	    cb->origin_end_col = cb->end.col;
	}
	അല്ലെങ്കിൽ
	{
	    cb->origin_row = (short_u)cb->start.lnum;
	    cb->origin_start_col = cb->start.col;
	    cb->origin_end_col = cb->start.col;
	}
	എങ്കിൽ (cb->mode == SELECT_MODE_WORD && !repeated_click)
	    cb->mode = SELECT_MODE_CHAR;
    }

    // set state, for when using the right mouse button
    cb->state = SELECT_IN_PROGRESS;

# ifdef DEBUG_SELECTION
    printf("Selection extending to (%d,%d)\n", row, col);
# endif

    എങ്കിൽ (repeated_click && ++cb->mode > SELECT_MODE_വരി)
	cb->mode = SELECT_MODE_CHAR;

    തിരഞ്ഞെടുപ്പ് (cb->mode)
    {
	അവസ്ഥ SELECT_MODE_CHAR:
	    // If we're on a different line, find where the line ends
	    എങ്കിൽ (row != cb->prev.lnum)
		cb->word_end_col = clip_get_വരി_end(cb, row);

	    // See if we are before or after the origin of the selection
	    എങ്കിൽ (clip_compare_pos(row, col, cb->origin_row,
						   cb->origin_start_col) >= 0)
	    {
		എങ്കിൽ (col >= (പൂർണ്ണസംഖ്യ)cb->word_end_col)
		    clip_update_modeless_selection(cb, cb->origin_row,
			    cb->origin_start_col, row, (പൂർണ്ണസംഖ്യ)Columns);
		അല്ലെങ്കിൽ
		{
		    എങ്കിൽ (has_mbyte && mb_lefthalve(row, col))
			slen = 2;
		    clip_update_modeless_selection(cb, cb->origin_row,
			    cb->origin_start_col, row, col + slen);
		}
	    }
	    അല്ലെങ്കിൽ
	    {
		എങ്കിൽ (has_mbyte
			&& mb_lefthalve(cb->origin_row, cb->origin_start_col))
		    slen = 2;
		എങ്കിൽ (col >= (പൂർണ്ണസംഖ്യ)cb->word_end_col)
		    clip_update_modeless_selection(cb, row, cb->word_end_col,
			    cb->origin_row, cb->origin_start_col + slen);
		അല്ലെങ്കിൽ
		    clip_update_modeless_selection(cb, row, col,
			    cb->origin_row, cb->origin_start_col + slen);
	    }
	    നിർത്തുക;

	അവസ്ഥ SELECT_MODE_WORD:
	    // If we are still within the same word, do nothing
	    എങ്കിൽ (row == cb->prev.lnum && col >= (പൂർണ്ണസംഖ്യ)cb->word_start_col
		    && col < (പൂർണ്ണസംഖ്യ)cb->word_end_col && !repeated_click)
		തിരികെ;

	    // Get new word boundaries
	    clip_get_word_boundaries(cb, row, col);

	    // Handle being after the origin point of selection
	    എങ്കിൽ (clip_compare_pos(row, col, cb->origin_row,
		    cb->origin_start_col) >= 0)
		clip_update_modeless_selection(cb, cb->origin_row,
			cb->origin_start_col, row, cb->word_end_col);
	    അല്ലെങ്കിൽ
		clip_update_modeless_selection(cb, row, cb->word_start_col,
			cb->origin_row, cb->origin_end_col);
	    നിർത്തുക;

	അവസ്ഥ SELECT_MODE_വരി:
	    എങ്കിൽ (row == cb->prev.lnum && !repeated_click)
		തിരികെ;

	    എങ്കിൽ (clip_compare_pos(row, col, cb->origin_row,
		    cb->origin_start_col) >= 0)
		clip_update_modeless_selection(cb, cb->origin_row, 0, row,
			(പൂർണ്ണസംഖ്യ)Columns);
	    അല്ലെങ്കിൽ
		clip_update_modeless_selection(cb, row, 0, cb->origin_row,
			(പൂർണ്ണസംഖ്യ)Columns);
	    നിർത്തുക;
    }

    cb->prev.lnum = row;
    cb->prev.col  = col;

# ifdef DEBUG_SELECTION
	printf("Selection is: (%ld,%d) to (%ld,%d)\n", cb->start.lnum,
		cb->start.col, cb->end.lnum, cb->end.col);
# endif
}

# എങ്കിൽ defined(FEAT_GUI)
/*
 * Redraw part of the selection if character at "row,col" is inside of it.
 * Only used for the GUI.
 */
    ശൂന്യം
clip_may_redraw_selection(പൂർണ്ണസംഖ്യ row, പൂർണ്ണസംഖ്യ col, പൂർണ്ണസംഖ്യ len)
{
    പൂർണ്ണസംഖ്യ		start = col;
    പൂർണ്ണസംഖ്യ		end = col + len;

    എങ്കിൽ (clip_star.state != SELECT_CLEARED
	    && row >= clip_star.start.lnum
	    && row <= clip_star.end.lnum)
    {
	എങ്കിൽ (row == clip_star.start.lnum && start < (പൂർണ്ണസംഖ്യ)clip_star.start.col)
	    start = clip_star.start.col;
	എങ്കിൽ (row == clip_star.end.lnum && end > (പൂർണ്ണസംഖ്യ)clip_star.end.col)
	    end = clip_star.end.col;
	എങ്കിൽ (end > start)
	    clip_invert_area(&clip_star, row, start, row, end, 0);
    }
}
# endif

/*
 * Called from outside to clear selected region from the display
 */
    ശൂന്യം
clip_clear_selection(Clipboard_T *cbd)
{

    എങ്കിൽ (cbd->state == SELECT_CLEARED)
	തിരികെ;

    clip_invert_area(cbd, (പൂർണ്ണസംഖ്യ)cbd->start.lnum, cbd->start.col,
				 (പൂർണ്ണസംഖ്യ)cbd->end.lnum, cbd->end.col, CLIP_CLEAR);
    cbd->state = SELECT_CLEARED;
}

/*
 * Clear the selection if any lines from "row1" to "row2" are inside of it.
 */
    ശൂന്യം
clip_may_clear_selection(പൂർണ്ണസംഖ്യ row1, പൂർണ്ണസംഖ്യ row2)
{
    എങ്കിൽ (clip_star.state == SELECT_DONE
	    && row2 >= clip_star.start.lnum
	    && row1 <= clip_star.end.lnum)
	clip_clear_selection(&clip_star);
}

/*
 * Called before the screen is scrolled up or down.  Adjusts the line numbers
 * of the selection.  Call with big number when clearing the screen.
 */
    ശൂന്യം
clip_scroll_selection(
    പൂർണ്ണസംഖ്യ	    rows)		// negative for scroll down
{
    പൂർണ്ണസംഖ്യ	    lnum;

    എങ്കിൽ (clip_star.state == SELECT_CLEARED)
	തിരികെ;

    lnum = clip_star.start.lnum - rows;
    എങ്കിൽ (lnum <= 0)
	clip_star.start.lnum = 0;
    അല്ലെങ്കിൽ എങ്കിൽ (lnum >= screen_Rows)	// scrolled off of the screen
	clip_star.state = SELECT_CLEARED;
    അല്ലെങ്കിൽ
	clip_star.start.lnum = lnum;

    lnum = clip_star.end.lnum - rows;
    എങ്കിൽ (lnum < 0)			// scrolled off of the screen
	clip_star.state = SELECT_CLEARED;
    അല്ലെങ്കിൽ എങ്കിൽ (lnum >= screen_Rows)
	clip_star.end.lnum = screen_Rows - 1;
    അല്ലെങ്കിൽ
	clip_star.end.lnum = lnum;
}

/*
 * Copy the currently selected area into the '*' or '+' register so it will be
 * available for pasting.
 * When "both" is TRUE also copy to the other register.
 */
    ശൂന്യം
clip_copy_modeless_selection(പൂർണ്ണസംഖ്യ both UNUSED)
{
    // The info for the modeless selection is stored in '*' register, however if
    // we are using the '+' register for modeless autoselect, we copy to
    // clip_plus instead while using the info in clip_star.
    Clipboard_T *cbd = clip_isautosel_plus() ? &clip_plus : &clip_star;
    char_u	*buffer;
    char_u	*bufp;
    പൂർണ്ണസംഖ്യ		row;
    പൂർണ്ണസംഖ്യ		start_col;
    പൂർണ്ണസംഖ്യ		end_col;
    പൂർണ്ണസംഖ്യ		വരി_end_col;
    പൂർണ്ണസംഖ്യ		add_newline_flag = FALSE;
    പൂർണ്ണസംഖ്യ		len;
    char_u	*p;
    പൂർണ്ണസംഖ്യ		row1 = clip_star.start.lnum;
    പൂർണ്ണസംഖ്യ		col1 = clip_star.start.col;
    പൂർണ്ണസംഖ്യ		row2 = clip_star.end.lnum;
    പൂർണ്ണസംഖ്യ		col2 = clip_star.end.col;

    // Can't use ScreenLines unless initialized
    എങ്കിൽ (Screen_വരികൾ == ശൂന്യ)
	തിരികെ;

    /*
     * Make sure row1 <= row2, and if row1 == row2 that col1 <= col2.
     */
    എങ്കിൽ (row1 > row2)
    {
	row = row1; row1 = row2; row2 = row;
	row = col1; col1 = col2; col2 = row;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (row1 == row2 && col1 > col2)
    {
	row = col1; col1 = col2; col2 = row;
    }
# ifdef FEAT_PROP_POPUP
    എങ്കിൽ (col1 < clip_star.min_col)
	col1 = clip_star.min_col;
    എങ്കിൽ (col2 > clip_star.max_col)
	col2 = clip_star.max_col;
    എങ്കിൽ (row1 > clip_star.max_row || row2 < clip_star.min_row)
	തിരികെ;
    എങ്കിൽ (row1 < clip_star.min_row)
	row1 = clip_star.min_row;
    എങ്കിൽ (row2 > clip_star.max_row)
	row2 = clip_star.max_row;
# endif
    // correct starting point for being on right half of double-wide char
    p = Screen_വരികൾ + വരി_Offset[row1];
    എങ്കിൽ (enc_dbcs != 0)
	col1 -= (*mb_head_off)(p, p + col1);
    അല്ലെങ്കിൽ എങ്കിൽ (enc_utf8 && p[col1] == 0)
	--col1;

    // Create a temporary buffer for storing the text
    len = (row2 - row1 + 1) * Columns + 1;
    എങ്കിൽ (enc_dbcs != 0)
	len *= 2;	// max. 2 bytes per display cell
    അല്ലെങ്കിൽ എങ്കിൽ (enc_utf8)
	len *= MB_MAXBYTES;
    // A newline is added after each row
    len += row2 - row1 + 2;
    buffer = alloc(len);
    എങ്കിൽ (buffer == ശൂന്യ)	    // out of memory
	തിരികെ;

    // Process each row in the selection
    വേണ്ടി (bufp = buffer, row = row1; row <= row2; row++)
    {
	എങ്കിൽ (row == row1)
	    start_col = col1;
	അല്ലെങ്കിൽ
# ifdef FEAT_PROP_POPUP
	    start_col = clip_star.min_col;
# അല്ലെങ്കിൽ
	    start_col = 0;
# endif

	എങ്കിൽ (row == row2)
	    end_col = col2;
	അല്ലെങ്കിൽ
# ifdef FEAT_PROP_POPUP
	    end_col = clip_star.max_col;
# അല്ലെങ്കിൽ
	    end_col = Columns;
# endif

	വരി_end_col = clip_get_വരി_end(&clip_star, row);

	// See if we need to nuke some trailing whitespace
	എങ്കിൽ (end_col >=
# ifdef FEAT_PROP_POPUP
		clip_star.max_col
# അല്ലെങ്കിൽ
		Columns
# endif
		    && (row < row2 || end_col > വരി_end_col))
	{
	    // Get rid of trailing whitespace
	    end_col = വരി_end_col;
	    എങ്കിൽ (end_col < start_col)
		end_col = start_col;

	    // If the last line extended to the end, add an extra newline
	    എങ്കിൽ (row == row2)
		add_newline_flag = TRUE;
	}

	// If after the first row, we need to always add a newline
	എങ്കിൽ (row > row1 && !വരി_Wraps[row - 1])
	    *bufp++ = NL;

	// Safetey check for in case resizing went wrong
	എങ്കിൽ (row < screen_Rows && end_col <= screen_Columns)
	{
	    എങ്കിൽ (enc_dbcs != 0)
	    {
		പൂർണ്ണസംഖ്യ	i;

		p = Screen_വരികൾ + വരി_Offset[row];
		വേണ്ടി (i = start_col; i < end_col; ++i)
		    എങ്കിൽ (enc_dbcs == DBCS_JPNU && p[i] == 0x8e)
		    {
			// single-width double-byte char
			*bufp++ = 0x8e;
			*bufp++ = ScreenLines2[വരി_Offset[row] + i];
		    }
		    അല്ലെങ്കിൽ
		    {
			*bufp++ = p[i];
			എങ്കിൽ (MB_BYTE2LEN(p[i]) == 2)
			    *bufp++ = p[++i];
		    }
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (enc_utf8)
	    {
		പൂർണ്ണസംഖ്യ	off;
		പൂർണ്ണസംഖ്യ	i;

		off = വരി_Offset[row];
		വേണ്ടി (i = start_col; i < end_col; ++i)
		{
		    // The base character is either in ScreenLinesUC[] or
		    // ScreenLines[].
		    എങ്കിൽ (Screen_വരികൾ_UC[off + i] == 0)
			*bufp++ = Screen_വരികൾ[off + i];
		    അല്ലെങ്കിൽ
			// Includes the composing characters.
			bufp += utfc_char2bytes(off + i, bufp);
		    // Skip right half of double-wide character.
		    എങ്കിൽ (Screen_വരികൾ[off + i + 1] == 0)
			++i;
		}
	    }
	    അല്ലെങ്കിൽ
	    {
		STRNCPY(bufp, Screen_വരികൾ + വരി_Offset[row] + start_col,
							 end_col - start_col);
		bufp += end_col - start_col;
	    }
	}
    }

    // Add a newline at the end if the selection ended there
    എങ്കിൽ (add_newline_flag)
	*bufp++ = NL;

    // First cleanup any old selection and become the owner.
    clip_free_selection(cbd);
    clip_own_selection(cbd);

    // Yank the text into the '*' register.
    clip_yank_selection(MCHAR, buffer, (long)(bufp - buffer), cbd);

    // Make the register contents available to the outside world.
    clip_gen_set_selection(cbd);

# ifdef FEAT_X11
    എങ്കിൽ (both)
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

    ശൂന്യം
clip_gen_set_selection(Clipboard_T *cbd)
{
    എങ്കിൽ (!clip_did_set_selection)
    {
	// Updating postponed, so that accessing the system clipboard won't
	// hang Vim when accessing it many times (e.g. on a :g command).
	എങ്കിൽ ((cbd == &clip_plus && (clip_unnamed_saved & CLIP_UNNAMED_PLUS))
		|| (cbd == &clip_star && (clip_unnamed_saved & CLIP_UNNAMED)))
	{
	    clipboard_needs_update = TRUE;
	    തിരികെ;
	}
    }
# എങ്കിൽ defined(FEAT_XCLIPBOARD) || defined(FEAT_WAYLAND_CLIPBOARD)
#  ifdef FEAT_GUI
    എങ്കിൽ (gui.in_use)
	clip_mch_set_selection(cbd);
    അല്ലെങ്കിൽ
#  endif
    {
	എങ്കിൽ (clipmethod == CLIPMETHOD_WAYLAND)
	{
#  ifdef FEAT_WAYLAND_CLIPBOARD
	    clip_wl_set_selection(cbd);
#  endif
	}
	അല്ലെങ്കിൽ എങ്കിൽ (clipmethod == CLIPMETHOD_X11)
	{
#  ifdef FEAT_XCLIPBOARD
	    clip_xterm_set_selection(cbd);
#  endif
	}
    }
# അല്ലെങ്കിൽ
    clip_mch_set_selection(cbd);
# endif
}

    സ്ഥിതം ശൂന്യം
clip_gen_request_selection(Clipboard_T *cbd)
{
# എങ്കിൽ defined(FEAT_XCLIPBOARD) || defined(FEAT_WAYLAND_CLIPBOARD)
#  ifdef FEAT_GUI
    എങ്കിൽ (gui.in_use)
	clip_mch_request_selection(cbd);
    അല്ലെങ്കിൽ
#  endif
    {
	എങ്കിൽ (clipmethod == CLIPMETHOD_WAYLAND)
	{
#  ifdef FEAT_WAYLAND_CLIPBOARD
	    clip_wl_request_selection(cbd);
#  endif
	}
	അല്ലെങ്കിൽ എങ്കിൽ (clipmethod == CLIPMETHOD_X11)
	{
#  ifdef FEAT_XCLIPBOARD
	    clip_xterm_request_selection(cbd);
#  endif
	}
    }
# അല്ലെങ്കിൽ
    clip_mch_request_selection(cbd);
# endif
}

# എങ്കിൽ (defined(FEAT_X11) && defined(FEAT_XCLIPBOARD) && defined(USE_SYSTEM))
    സ്ഥിതം പൂർണ്ണസംഖ്യ
clip_x11_owner_exists(Clipboard_T *cbd)
{
    തിരികെ XGetSelectionOwner(X_DISPLAY, cbd->sel_atom) != None;
}
# endif

# എങ്കിൽ (defined(FEAT_X11) || defined(FEAT_WAYLAND_CLIPBOARD)) \
	&& defined(USE_SYSTEM)
    പൂർണ്ണസംഖ്യ
clip_gen_owner_exists(Clipboard_T *cbd UNUSED)
{
#  എങ്കിൽ defined(FEAT_XCLIPBOARD) || defined(FEAT_WAYLAND_CLIPBOARD)
#   ifdef FEAT_GUI_GTK
    എങ്കിൽ (gui.in_use)
	തിരികെ clip_gtk_owner_exists(cbd);
    അല്ലെങ്കിൽ
#   endif
    {
	എങ്കിൽ (clipmethod == CLIPMETHOD_WAYLAND)
	{
#   ifdef FEAT_WAYLAND_CLIPBOARD
	    തിരികെ clip_wl_owner_exists(cbd);
#   endif
	}
	അല്ലെങ്കിൽ എങ്കിൽ (clipmethod == CLIPMETHOD_X11)
	{
#   ifdef FEAT_XCLIPBOARD
	    തിരികെ clip_x11_owner_exists(cbd);
#   endif
	}
	അല്ലെങ്കിൽ
	    തിരികെ FALSE;
    }
#  അല്ലെങ്കിൽ
    തിരികെ TRUE;
#  endif
}
# endif

/*
 * Stuff for the X clipboard.  Shared between VMS and Unix.
 */

# എങ്കിൽ defined(FEAT_XCLIPBOARD) || defined(FEAT_GUI_X11)
#  include <X11/Xatom.h>
#  include <X11/Intrinsic.h>

/*
 * Open the application context (if it hasn't been opened yet).
 * Used for Motif GUI and the xterm clipboard.
 */
    ശൂന്യം
open_app_context(ശൂന്യം)
{
    എങ്കിൽ (app_context == ശൂന്യ)
    {
	XtToolkitInitialize();
	app_context = XtCreateApplicationContext();
    }
}

സ്ഥിതം Atom	vim_atom;	// Vim's own special selection format
സ്ഥിതം Atom	vimenc_atom;	// Vim's extended selection format
സ്ഥിതം Atom	vim_mt_atom;	// Vim's own special selection format (in mime
				// type format)
സ്ഥിതം Atom	vimenc_mt_atom;	// Vim's extended selection format (in mime type
				// format)
സ്ഥിതം Atom	utf8_atom;
സ്ഥിതം Atom	compound_text_atom;
സ്ഥിതം Atom	text_atom;
സ്ഥിതം Atom	targets_atom;
സ്ഥിതം Atom	timestamp_atom;	// Used to get a timestamp

    ശൂന്യം
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

    സ്ഥിതം Boolean
clip_x11_convert_selection_cb(
    Widget	w UNUSED,
    Atom	*sel_atom,
    Atom	*target,
    Atom	*type,
    XtPointer	*മൂല്യം,
    long_u	*length,
    പൂർണ്ണസംഖ്യ		*format)
{
    സ്ഥിതം char_u   *save_result = ശൂന്യ;
    സ്ഥിതം long_u   save_length = 0;
    char_u	    *string;
    പൂർണ്ണസംഖ്യ		    motion_type;
    Clipboard_T    *cbd;
    പൂർണ്ണസംഖ്യ		    i;

    എങ്കിൽ (*sel_atom == clip_plus.sel_atom)
	cbd = &clip_plus;
    അല്ലെങ്കിൽ
	cbd = &clip_star;

    എങ്കിൽ (!cbd->owned)
	തിരികെ False;	    // Shouldn't ever happen

    // requestor wants to know what target types we support
    എങ്കിൽ (*target == targets_atom)
    {
	സ്ഥിതം Atom array[9];

	*മൂല്യം = (XtPointer)array;
	i = 0;
	array[i++] = targets_atom;
	array[i++] = vimenc_atom;
	array[i++] = vim_atom;
	array[i++] = vimenc_mt_atom;
	array[i++] = vim_mt_atom;
	എങ്കിൽ (enc_utf8)
	    array[i++] = utf8_atom;
	array[i++] = XA_STRING;
	array[i++] = text_atom;
	array[i++] = compound_text_atom;

	*type = XA_ATOM;
	// This used to be: *format = sizeof(Atom) * 8; but that caused
	// crashes on 64 bit machines. (Peter Derr)
	*format = 32;
	*length = i;
	തിരികെ True;
    }

    എങ്കിൽ (       *target != XA_STRING
	    && *target != vimenc_atom
	    && *target != vimenc_mt_atom
	    && (*target != utf8_atom || !enc_utf8)
	    && *target != vim_atom
	    && *target != vim_mt_atom
	    && *target != text_atom
	    && *target != compound_text_atom)
	തിരികെ False;

    clip_get_selection(cbd);
    motion_type = clip_convert_selection(&string, length, cbd);
    എങ്കിൽ (motion_type < 0)
	തിരികെ False;

    // For our own format, the first byte contains the motion type
    എങ്കിൽ (*target == vim_atom || *target == vim_mt_atom)
	(*length)++;

    // Our own format with encoding: motion 'encoding' NUL text
    എങ്കിൽ (*target == vimenc_atom || *target == vimenc_mt_atom)
	*length += STRLEN(p_enc) + 2;

    എങ്കിൽ (save_length < *length || save_length / 2 >= *length)
	*മൂല്യം = XtRealloc((അക്ഷരം *)save_result, (Cardinal)*length + 1);
    അല്ലെങ്കിൽ
	*മൂല്യം = save_result;
    എങ്കിൽ (*മൂല്യം == ശൂന്യ)
    {
	vim_free(string);
	തിരികെ False;
    }
    save_result = (char_u *)*മൂല്യം;
    save_length = *length;

    എങ്കിൽ (*target == XA_STRING || (*target == utf8_atom && enc_utf8))
    {
	mch_memmove(save_result, string, (size_t)(*length));
	*type = *target;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (*target == compound_text_atom || *target == text_atom)
    {
	XTextProperty	text_prop;
	അക്ഷരം		*string_nt = (അക്ഷരം *)save_result;
	പൂർണ്ണസംഖ്യ		conv_result;

	// create NUL terminated string which XmbTextListToTextProperty wants
	mch_memmove(string_nt, string, (size_t)*length);
	string_nt[*length] = NUL;
	conv_result = XmbTextListToTextProperty(X_DISPLAY, &string_nt,
					   1, XCompoundTextStyle, &text_prop);
	എങ്കിൽ (conv_result != Success)
	{
	    vim_free(string);
	    തിരികെ False;
	}
	*മൂല്യം = (XtPointer)(text_prop.മൂല്യം);	//    from plain text
	*length = text_prop.nitems;
	*type = compound_text_atom;
	XtFree((അക്ഷരം *)save_result);
	save_result = (char_u *)*മൂല്യം;
	save_length = *length;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (*target == vimenc_atom || *target == vimenc_mt_atom)
    {
	പൂർണ്ണസംഖ്യ l = STRLEN(p_enc);

	save_result[0] = motion_type;
	STRCPY(save_result + 1, p_enc);
	mch_memmove(save_result + l + 2, string, (size_t)(*length - l - 2));
	എങ്കിൽ (*target == vimenc_atom)
	    *type = vimenc_atom;
	അല്ലെങ്കിൽ
	    *type = vimenc_mt_atom;
    }
    അല്ലെങ്കിൽ
    {
	save_result[0] = motion_type;
	mch_memmove(save_result + 1, string, (size_t)(*length - 1));
	എങ്കിൽ (*target == vim_atom)
	    *type = vim_atom;
	അല്ലെങ്കിൽ
	    *type = vim_mt_atom;
    }
    *format = 8;	    // 8 bits per char
    vim_free(string);
    തിരികെ True;
}

    സ്ഥിതം ശൂന്യം
clip_x11_lose_ownership_cb(Widget w UNUSED, Atom *sel_atom)
{
    എങ്കിൽ (*sel_atom == clip_plus.sel_atom)
	clip_lose_selection(&clip_plus);
    അല്ലെങ്കിൽ
	clip_lose_selection(&clip_star);
}

    സ്ഥിതം ശൂന്യം
clip_x11_notify_cb(Widget w UNUSED, Atom *sel_atom UNUSED, Atom *target UNUSED)
{
    // To prevent automatically freeing the selection value.
}

/*
 * Property callback to get a timestamp for XtOwnSelection.
 */
#  എങ്കിൽ defined(FEAT_X11) && defined(FEAT_XCLIPBOARD)
    സ്ഥിതം ശൂന്യം
clip_x11_timestamp_cb(
    Widget	w,
    XtPointer	n UNUSED,
    XEvent	*event,
    Boolean	*cont UNUSED)
{
    Atom	    actual_type;
    പൂർണ്ണസംഖ്യ		    format;
    unsigned  long  nitems, bytes_after;
    unsigned അക്ഷരം   *prop=ശൂന്യ;
    XPropertyEvent  *xproperty=&event->xproperty;

    // Must be a property notify, state can't be Delete (True), has to be
    // one of the supported selection types.
    എങ്കിൽ (event->type != PropertyNotify || xproperty->state
	    || (xproperty->atom != clip_star.sel_atom
				    && xproperty->atom != clip_plus.sel_atom))
	തിരികെ;

    എങ്കിൽ (XGetWindowProperty(xproperty->display, xproperty->window,
	  xproperty->atom, 0, 0, False, timestamp_atom, &actual_type, &format,
						&nitems, &bytes_after, &prop))
	തിരികെ;

    എങ്കിൽ (prop)
	XFree(prop);

    // Make sure the property type is "TIMESTAMP" and it's 32 bits.
    എങ്കിൽ (actual_type != timestamp_atom || format != 32)
	തിരികെ;

    // Get the selection, using the event timestamp.
    എങ്കിൽ (XtOwnSelection(w, xproperty->atom, xproperty->time,
	    clip_x11_convert_selection_cb, clip_x11_lose_ownership_cb,
	    clip_x11_notify_cb) == OK)
    {
	// Set the "owned" flag now, there may have been a call to
	// lose_ownership_cb in between.
	എങ്കിൽ (xproperty->atom == clip_plus.sel_atom)
	    clip_plus.owned = TRUE;
	അല്ലെങ്കിൽ
	    clip_star.owned = TRUE;
    }
}

    ശൂന്യം
x11_setup_selection(Widget w)
{
    XtAddEventHandler(w, PropertyChangeMask, False,
	    /*(XtEventHandler)*/clip_x11_timestamp_cb, (XtPointer)ശൂന്യ);
}
#  endif

    സ്ഥിതം ശൂന്യം
clip_x11_request_selection_cb(
    Widget	w UNUSED,
    XtPointer	success,
    Atom	*sel_atom,
    Atom	*type,
    XtPointer	മൂല്യം,
    long_u	*length,
    പൂർണ്ണസംഖ്യ		*format)
{
    പൂർണ്ണസംഖ്യ		motion_type = MAUTO;
    long_u	len;
    char_u	*p;
    അക്ഷരം	**text_list = ശൂന്യ;
    Clipboard_T	*cbd;
    char_u	*tmpbuf = ശൂന്യ;

    എങ്കിൽ (*sel_atom == clip_plus.sel_atom)
	cbd = &clip_plus;
    അല്ലെങ്കിൽ
	cbd = &clip_star;

    എങ്കിൽ (മൂല്യം == ശൂന്യ || *length == 0)
    {
	clip_free_selection(cbd);	// nothing received, clear register
	*(പൂർണ്ണസംഖ്യ *)success = FALSE;
	തിരികെ;
    }
    p = (char_u *)മൂല്യം;
    len = *length;
    എങ്കിൽ (*type == vim_atom || *type == vim_mt_atom)
    {
	motion_type = *p++;
	len--;
    }

    അല്ലെങ്കിൽ എങ്കിൽ (*type == vimenc_atom || *type == vimenc_mt_atom)
    {
	char_u		*enc;
	vimconv_T	conv;
	പൂർണ്ണസംഖ്യ		convlen;

	motion_type = *p++;
	--len;

	enc = p;
	p += STRLEN(p) + 1;
	len -= p - enc;

	// If the encoding of the text is different from 'encoding', attempt
	// converting it.
	conv.vc_type = CONV_NONE;
	convert_setup(&conv, enc, p_enc);
	എങ്കിൽ (conv.vc_type != CONV_NONE)
	{
	    convlen = len;	// Need to use an int here.
	    tmpbuf = string_convert(&conv, p, &convlen);
	    len = convlen;
	    എങ്കിൽ (tmpbuf != ശൂന്യ)
		p = tmpbuf;
	    convert_setup(&conv, ശൂന്യ, ശൂന്യ);
	}
    }

    അല്ലെങ്കിൽ എങ്കിൽ (*type == compound_text_atom
	    || *type == utf8_atom
	    || (enc_dbcs != 0 && *type == text_atom))
    {
	XTextProperty	text_prop;
	പൂർണ്ണസംഖ്യ		n_text = 0;
	പൂർണ്ണസംഖ്യ		status;

	text_prop.മൂല്യം = (unsigned അക്ഷരം *)മൂല്യം;
	text_prop.encoding = *type;
	text_prop.format = *format;
	text_prop.nitems = len;
#  എങ്കിൽ defined(X_HAVE_UTF8_STRING)
	എങ്കിൽ (*type == utf8_atom)
	    status = Xutf8TextPropertyToTextList(X_DISPLAY, &text_prop,
							 &text_list, &n_text);
	അല്ലെങ്കിൽ
#  endif
	    status = XmbTextPropertyToTextList(X_DISPLAY, &text_prop,
							 &text_list, &n_text);
	എങ്കിൽ (status != Success || n_text < 1)
	{
	    *(പൂർണ്ണസംഖ്യ *)success = FALSE;
	    തിരികെ;
	}
	p = (char_u *)text_list[0];
	len = STRLEN(p);
    }
    clip_yank_selection(motion_type, p, (long)len, cbd);

    എങ്കിൽ (text_list != ശൂന്യ)
	XFreeStringList(text_list);
    vim_free(tmpbuf);
    XtFree((അക്ഷരം *)മൂല്യം);
    *(പൂർണ്ണസംഖ്യ *)success = TRUE;
}

    ശൂന്യം
clip_x11_request_selection(
    Widget	myShell,
    Display	*dpy,
    Clipboard_T	*cbd)
{
    XEvent	event;
    Atom	type;
    സ്ഥിതം പൂർണ്ണസംഖ്യ	success;
    പൂർണ്ണസംഖ്യ		i;
    time_t	start_time;
    പൂർണ്ണസംഖ്യ		timed_out = FALSE;

    വേണ്ടി (i = 0; i < 8; i++)
    {
	തിരഞ്ഞെടുപ്പ് (i)
	{
	    അവസ്ഥ 0:  type = vimenc_mt_atom;	നിർത്തുക;
	    അവസ്ഥ 1:  type = vimenc_atom;	നിർത്തുക;
	    അവസ്ഥ 2:  type = vim_mt_atom;	നിർത്തുക;
	    അവസ്ഥ 3:  type = vim_atom;		നിർത്തുക;
	    അവസ്ഥ 4:  type = utf8_atom;		നിർത്തുക;
	    അവസ്ഥ 5:  type = compound_text_atom; നിർത്തുക;
	    അവസ്ഥ 6:  type = text_atom;		നിർത്തുക;
	    സ്ഥിരസ്ഥിതി: type = XA_STRING;
	}
	എങ്കിൽ (type == utf8_atom
#  എങ്കിൽ defined(X_HAVE_UTF8_STRING)
		&& !enc_utf8
#  endif
		)
	    // Only request utf-8 when 'encoding' is utf8 and
	    // Xutf8TextPropertyToTextList is available.
	    തുടരുക;
	success = MAYBE;
	Xt_Get_Selection_മൂല്യം(myShell, cbd->sel_atom, type,
	    clip_x11_request_selection_cb, (XtPointer)&success, CurrentTime);

	// Make sure the request for the selection goes out before waiting for
	// a response.
	XFlush(dpy);

	/*
	 * Wait for result of selection request, otherwise if we type more
	 * characters, then they will appear before the one that requested the
	 * paste!  Don't worry, we will catch up with any other events later.
	 */
	start_time = time(ശൂന്യ);
	ആകുമ്പോൾ (success == MAYBE)
	{
	    എങ്കിൽ (XCheckTypedEvent(dpy, PropertyNotify, &event)
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
		തുടരുക;
	    }

	    // Time out after 2 to 3 seconds to avoid that we hang when the
	    // other process doesn't respond.  Note that the SelectionNotify
	    // event may still come later when the selection owner comes back
	    // to life and the text gets inserted unexpectedly.  Don't know
	    // why that happens or how to avoid that :-(.
	    എങ്കിൽ (time(ശൂന്യ) > start_time + 2)
	    {
		timed_out = TRUE;
		നിർത്തുക;
	    }

	    // Do we need this?  Probably not.
	    XSync(dpy, False);

	    // Wait for 1 msec to avoid that we eat up all CPU time.
	    ui_delay(1L, TRUE);
	}

	എങ്കിൽ (success == TRUE)
	    തിരികെ;

	// don't do a retry with another type after timing out, otherwise we
	// hang for 15 seconds.
	എങ്കിൽ (timed_out)
	    നിർത്തുക;
    }

    // Final fallback position - use the X CUT_BUFFER0 store
    yank_cut_buffer0(dpy, cbd);
}

    ശൂന്യം
clip_x11_lose_selection(Widget myShell, Clipboard_T *cbd)
{
    XtDisownSelection(myShell, cbd->sel_atom,
				XtLastTimestampProcessed(XtDisplay(myShell)));
}

    പൂർണ്ണസംഖ്യ
clip_x11_own_selection(Widget myShell, Clipboard_T *cbd)
{
    // When using the GUI we have proper timestamps, use the one of the last
    // event.  When in the console we don't get events (the terminal gets
    // them), Get the time by a zero-length append, clip_x11_timestamp_cb will
    // be called with the current timestamp.
#  ifdef FEAT_GUI
    എങ്കിൽ (gui.in_use)
    {
	എങ്കിൽ (XtOwnSelection(myShell, cbd->sel_atom,
	       XtLastTimestampProcessed(XtDisplay(myShell)),
	       clip_x11_convert_selection_cb, clip_x11_lose_ownership_cb,
	       clip_x11_notify_cb) == False)
	    തിരികെ FAIL;
    }
    അല്ലെങ്കിൽ
#  endif
    {
	എങ്കിൽ (!XChangeProperty(XtDisplay(myShell), XtWindow(myShell),
		  cbd->sel_atom, timestamp_atom, 32, PropModeAppend, ശൂന്യ, 0))
	    തിരികെ FAIL;
    }
    // Flush is required in a terminal as nothing else is doing it.
    XFlush(XtDisplay(myShell));
    തിരികെ OK;
}

/*
 * Send the current selection to the clipboard.  Do nothing for X because we
 * will fill in the selection only when requested by another app.
 */
    ശൂന്യം
clip_x11_set_selection(Clipboard_T *cbd UNUSED)
{
}

# endif

# എങ്കിൽ (defined(FEAT_XCLIPBOARD) || defined(FEAT_GUI_X11) || defined(FEAT_GUI_GTK)) \
	&& !defined(USE_GTK4)
/*
 * Get the contents of the X CUT_BUFFER0 and put it in "cbd".
 */
    ശൂന്യം
yank_cut_buffer0(Display *dpy, Clipboard_T *cbd)
{
    പൂർണ്ണസംഖ്യ		nbytes = 0;
    char_u	*buffer = (char_u *)XFetchBuffer(dpy, &nbytes, 0);

    എങ്കിൽ (nbytes > 0)
    {
	പൂർണ്ണസംഖ്യ  done = FALSE;

	// CUT_BUFFER0 is supposed to be always latin1.  Convert to 'enc' when
	// using a multi-byte encoding.  Conversion between two 8-bit
	// character sets usually fails and the text might actually be in
	// 'enc' anyway.
	എങ്കിൽ (has_mbyte)
	{
	    char_u	*conv_buf;
	    vimconv_T	vc;

	    vc.vc_type = CONV_NONE;
	    എങ്കിൽ (convert_setup(&vc, (char_u *)"latin1", p_enc) == OK)
	    {
		conv_buf = string_convert(&vc, buffer, &nbytes);
		എങ്കിൽ (conv_buf != ശൂന്യ)
		{
		    clip_yank_selection(MCHAR, conv_buf, (long)nbytes, cbd);
		    vim_free(conv_buf);
		    done = TRUE;
		}
		convert_setup(&vc, ശൂന്യ, ശൂന്യ);
	    }
	}
	എങ്കിൽ (!done)  // use the text without conversion
	    clip_yank_selection(MCHAR, buffer, (long)nbytes, cbd);
	XFree((ശൂന്യം *)buffer);
	എങ്കിൽ (p_verbose > 0)
	{
	    verbose_enter();
	    verb_സന്ദേശം(_("Used CUT_BUFFER0 instead of empty selection"));
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
# എങ്കിൽ defined(FEAT_X11) && defined(FEAT_CLIPBOARD)
    ശൂന്യം
x11_export_final_selection(ശൂന്യം)
{
    Display	*dpy;
    char_u	*str = ശൂന്യ;
    long_u	len = 0;
    പൂർണ്ണസംഖ്യ		motion_type = -1;

#  ifdef FEAT_GUI
    എങ്കിൽ (gui.in_use)
	dpy = X_DISPLAY;
    അല്ലെങ്കിൽ
#  endif
#  ifdef FEAT_XCLIPBOARD
	dpy = xterm_dpy;
#  അല്ലെങ്കിൽ
	തിരികെ;
#  endif

    // Get selection to export
    എങ്കിൽ (clip_plus.owned)
	motion_type = clip_convert_selection(&str, &len, &clip_plus);
    അല്ലെങ്കിൽ എങ്കിൽ (clip_star.owned)
	motion_type = clip_convert_selection(&str, &len, &clip_star);

    // Check it's OK
    എങ്കിൽ (dpy != ശൂന്യ && str != ശൂന്യ && motion_type >= 0
					       && len < 1024*1024 && len > 0)
    {
	പൂർണ്ണസംഖ്യ ok = TRUE;

	// The CUT_BUFFER0 is supposed to always contain latin1.  Convert from
	// 'enc' when it is a multi-byte encoding.  When 'enc' is an 8-bit
	// encoding conversion usually doesn't work, so keep the text as-is.
	എങ്കിൽ (has_mbyte)
	{
	    vimconv_T	vc;

	    vc.vc_type = CONV_NONE;
	    എങ്കിൽ (convert_setup(&vc, p_enc, (char_u *)"latin1") == OK)
	    {
		പൂർണ്ണസംഖ്യ	intlen = len;
		char_u	*conv_str;

		vc.vc_fail = TRUE;
		conv_str = string_convert(&vc, str, &intlen);
		len = intlen;
		എങ്കിൽ (conv_str != ശൂന്യ)
		{
		    vim_free(str);
		    str = conv_str;
		}
		അല്ലെങ്കിൽ
		{
		    ok = FALSE;
		}
		convert_setup(&vc, ശൂന്യ, ശൂന്യ);
	    }
	    അല്ലെങ്കിൽ
	    {
		ok = FALSE;
	    }
	}

	// Do not store the string if conversion failed.  Better to use any
	// other selection than garbled text.
	എങ്കിൽ (ok)
	{
	    XStoreBuffer(dpy, (അക്ഷരം *)str, (പൂർണ്ണസംഖ്യ)len, 0);
	    XFlush(dpy);
	}
    }

    vim_free(str);
}
# endif

    ശൂന്യം
clip_free_selection(Clipboard_T *cbd)
{
    yankreg_T *y_ptr = get_y_current();

    എങ്കിൽ (cbd == &clip_plus)
	set_y_current(get_y_register(PLUS_REGISTER));
    അല്ലെങ്കിൽ
	set_y_current(get_y_register(STAR_REGISTER));
    free_yank_all();
    get_y_current()->y_size = 0;
    set_y_current(y_ptr);
}

/*
 * Get the selected text and put it in register '*' or '+'.
 */
    ശൂന്യം
clip_get_selection(Clipboard_T *cbd)
{
    yankreg_T	*old_y_previous, *old_y_current;
    pos_T	old_cursor;
    pos_T	old_visual;
    പൂർണ്ണസംഖ്യ		old_visual_mode;
    colnr_T	old_virtcol;
    colnr_T	old_curswant;
    പൂർണ്ണസംഖ്യ		old_set_curswant;
    pos_T	old_op_start, old_op_end;
    oparg_T	oa;
    cmdarg_T	ca;

    എങ്കിൽ (cbd->owned)
    {
	എങ്കിൽ ((cbd == &clip_plus
		&& get_y_register(PLUS_REGISTER)->y_array != ശൂന്യ)
		|| (cbd == &clip_star
		    && get_y_register(STAR_REGISTER)->y_array != ശൂന്യ))
	    തിരികെ;

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
    അല്ലെങ്കിൽ എങ്കിൽ (!is_clipboard_needs_update())
    {
	clip_free_selection(cbd);

	// Try to get selected text from another window
	clip_gen_request_selection(cbd);
    }
}

/*
 * Convert from the GUI selection string into the '*'/'+' register.
 */
    ശൂന്യം
clip_yank_selection(
    പൂർണ്ണസംഖ്യ		type,
    char_u	*str,
    long	len,
    Clipboard_T *cbd)
{
    yankreg_T *y_ptr;

    എങ്കിൽ (cbd == &clip_plus)
	y_ptr = get_y_register(PLUS_REGISTER);
    അല്ലെങ്കിൽ
	y_ptr = get_y_register(STAR_REGISTER);

    clip_free_selection(cbd);

    str_to_reg(y_ptr, type, str, len, -1, FALSE);
}

    പൂർണ്ണസംഖ്യ
clip_convert_selection_offset(
	char_u	    **str,
	long_u	    *len,
	പൂർണ്ണസംഖ്യ	    offset, // Extra space to add in *str and the offset to
			    // place the actual string in *str.
	Clipboard_T *cbd)
{
    char_u	*p;
    പൂർണ്ണസംഖ്യ		lnum;
    പൂർണ്ണസംഖ്യ		i, j;
    int_u	eolsize;
    yankreg_T	*y_ptr;

    എങ്കിൽ (cbd == &clip_plus)
	y_ptr = get_y_register(PLUS_REGISTER);
    അല്ലെങ്കിൽ
	y_ptr = get_y_register(STAR_REGISTER);

# ifdef USE_CRNL
    eolsize = 2;
# അല്ലെങ്കിൽ
    eolsize = 1;
# endif

    *str = ശൂന്യ;
    *len = 0;
    എങ്കിൽ (y_ptr->y_array == ശൂന്യ)
	തിരികെ -1;

    വേണ്ടി (i = 0; i < y_ptr->y_size; i++)
	*len += (long_u)y_ptr->y_array[i].length + eolsize;

    // Don't want newline character at end of last line if we're in MCHAR mode.
    എങ്കിൽ (y_ptr->y_type == MCHAR && *len >= eolsize)
	*len -= eolsize;

    *len += offset;
    p = *str = alloc(*len + 1);	// add one to avoid zero
    എങ്കിൽ (p == ശൂന്യ)
	തിരികെ -1;
    p += offset;
    lnum = 0;
    വേണ്ടി (i = 0, j = 0; i < (പൂർണ്ണസംഖ്യ)*len - offset; i++, j++)
    {
	എങ്കിൽ (y_ptr->y_array[lnum].string[j] == '\n')
	    p[i] = NUL;
	അല്ലെങ്കിൽ എങ്കിൽ (y_ptr->y_array[lnum].string[j] == NUL)
	{
# ifdef USE_CRNL
	    p[i++] = '\r';
# endif
	    p[i] = '\n';
	    lnum++;
	    j = -1;
	}
	അല്ലെങ്കിൽ
	    p[i] = y_ptr->y_array[lnum].string[j];
    }
    തിരികെ y_ptr->y_type;
}

/*
 * Convert the '*'/'+' register into a GUI selection string returned in *str
 * with length *len.
 * Returns the motion type, or -1 for failure.
 */
    പൂർണ്ണസംഖ്യ
clip_convert_selection(char_u **str, long_u *len, Clipboard_T *cbd)
{
    തിരികെ clip_convert_selection_offset(str, len, 0, cbd);
}

/*
 * When "regname" is a clipboard register, obtain the selection.  If it's not
 * available return zero, otherwise return "regname".
 */
    പൂർണ്ണസംഖ്യ
may_get_selection(പൂർണ്ണസംഖ്യ regname)
{
    എങ്കിൽ (regname == '*')
    {
	എങ്കിൽ (!clip_star.available)
	    regname = 0;
	അല്ലെങ്കിൽ
	    clip_get_selection(&clip_star);
    }
    അല്ലെങ്കിൽ എങ്കിൽ (regname == '+')
    {
	എങ്കിൽ (!clip_plus.available)
	    regname = 0;
	അല്ലെങ്കിൽ
	    clip_get_selection(&clip_plus);
    }
    തിരികെ regname;
}

/*
 * If we have written to a clipboard register, send the text to the clipboard.
 */
    ശൂന്യം
may_set_selection(ശൂന്യം)
{
    എങ്കിൽ ((get_y_current() == get_y_register(STAR_REGISTER))
	    && clip_star.available)
    {
	clip_own_selection(&clip_star);
	clip_gen_set_selection(&clip_star);
    }
    അല്ലെങ്കിൽ എങ്കിൽ ((get_y_current() == get_y_register(PLUS_REGISTER))
	    && clip_plus.available)
    {
	clip_own_selection(&clip_plus);
	clip_gen_set_selection(&clip_plus);
    }
}

# എങ്കിൽ defined(FEAT_WAYLAND_CLIPBOARD)

    സ്ഥിതം clip_wl_selection_T *
clip_wl_get_selection(wayland_selection_T sel)
{
    തിരഞ്ഞെടുപ്പ് (sel)
    {
	അവസ്ഥ WAYLAND_SELECTION_REGULAR:
	    തിരികെ &clip_wl.regular;
	അവസ്ഥ WAYLAND_SELECTION_PRIMARY:
	    തിരികെ &clip_wl.primary;
	സ്ഥിരസ്ഥിതി:
	    തിരികെ ശൂന്യ;
    }
}

    സ്ഥിതം clip_wl_selection_T *
clip_wl_get_selection_from_cbd(Clipboard_T *cbd)
{
    എങ്കിൽ (cbd == &clip_plus)
	തിരികെ &clip_wl.regular;
    അല്ലെങ്കിൽ എങ്കിൽ (cbd == &clip_star)
	തിരികെ &clip_wl.primary;
    അല്ലെങ്കിൽ
	തിരികെ ശൂന്യ;
}

    സ്ഥിതം Clipboard_T *
clip_wl_get_cbd_from_selection(clip_wl_selection_T *sel)
{
    എങ്കിൽ (sel == &clip_wl.regular)
	തിരികെ &clip_plus;
    അല്ലെങ്കിൽ എങ്കിൽ (sel == &clip_wl.primary)
	തിരികെ &clip_star;
    അല്ലെങ്കിൽ
	തിരികെ ശൂന്യ;
}

    സ്ഥിതം wayland_selection_T
clip_wl_get_selection_type(clip_wl_selection_T *sel)
{
    എങ്കിൽ (sel == &clip_wl.regular)
	തിരികെ WAYLAND_SELECTION_REGULAR;
    അല്ലെങ്കിൽ എങ്കിൽ (sel == &clip_wl.primary)
	തിരികെ WAYLAND_SELECTION_PRIMARY;
    അല്ലെങ്കിൽ
	തിരികെ WAYLAND_SELECTION_NONE;
}

    സ്ഥിതം bool
wl_data_offer_listener_event_offer(
    ശൂന്യം *data UNUSED,
    vwl_data_offer_T *offer UNUSED,
    സ്ഥിരം അക്ഷരം *mime_type
)
{
    // Only accept mime type if we support it
    വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < (പൂർണ്ണസംഖ്യ)ARRAY_LENGTH(supported_mimes); i++)
	എങ്കിൽ (STRCMP(mime_type, supported_mimes[i]) == 0)
	    തിരികെ true;
    തിരികെ FALSE;
}

സ്ഥിതം സ്ഥിരം vwl_data_offer_listener_T vwl_data_offer_listener = {
    .offer = wl_data_offer_listener_event_offer
};

    സ്ഥിതം ശൂന്യം
vwl_data_device_listener_event_data_offer(
	ശൂന്യം *data UNUSED,
	vwl_data_device_T *device UNUSED,
	vwl_data_offer_T *offer)
{
    // Immediately start listening for offer events from the data offer
    vwl_data_offer_add_listener(offer, &vwl_data_offer_listener, ശൂന്യ);
}

    സ്ഥിതം ശൂന്യം
vwl_data_device_listener_event_selection(
	ശൂന്യം *data UNUSED,
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
    എങ്കിൽ (offer == ശൂന്യ || sel->source != ശൂന്യ)
    {
	// Selection event is from us, so we are the source client. Therefore
	// ignore it. Or the selection is cleared, so set sel->offer to NULL
	vwl_data_offer_destroy(offer);
	sel->offer = ശൂന്യ;
	തിരികെ;
    }

    // Save offer. When we want to request data, then we'll actually call the
    // receive method.
    sel->offer = offer;

}

    സ്ഥിതം ശൂന്യം
vwl_data_device_listener_event_finished(
	ശൂന്യം *data UNUSED,
	vwl_data_device_T *device)
{
    clip_wl_selection_T *sel;
    // Device finished, guessing this can happen is when the seat becomes
    // invalid? If so, let the user call :wlrestore! to reset. There wouldn't be
    // any point in trying to create another data device for the same seat,
    // since the seat is in an invalid state.
    എങ്കിൽ (device == clip_wl.regular.device)
    {
	sel = &clip_wl.regular;
	clip_wl.regular.device = ശൂന്യ;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (device == clip_wl.primary.device)
    {
	sel = &clip_wl.primary;
	clip_wl.primary.device = ശൂന്യ;
    }
    അല്ലെങ്കിൽ
	// Shouldn't happen
	തിരികെ;

    vim_free(sel->contents);
    vwl_data_source_destroy(sel->source);
    vwl_data_offer_destroy(sel->offer);
    sel->available = FALSE;

    vwl_data_device_destroy(device);
}

സ്ഥിതം സ്ഥിരം vwl_data_device_listener_T vwl_data_device_listener = {
    .data_offer = vwl_data_device_listener_event_data_offer,
    .selection = vwl_data_device_listener_event_selection,
    .finished = vwl_data_device_listener_event_finished
};

/*
 * Initialize the clipboard for Wayland using the global Wayland connection.
 * Returns OK on success and FAIL on failure.
 */
    പൂർണ്ണസംഖ്യ
clip_init_wayland(ശൂന്യം)
{
    int_u supported = WAYLAND_SELECTION_NONE;

    എങ്കിൽ (wayland_ct == ശൂന്യ)
	തിരികെ FAIL;

    memset(&clip_wl, 0, വലുപ്പം(clip_wl));

    clip_wl.seat = vwl_connection_get_seat(wayland_ct, (അക്ഷരം *)p_wse);

    എങ്കിൽ (clip_wl.seat == ശൂന്യ)
	തിരികെ FAIL;

    clip_wl.regular.manager = vwl_connection_get_data_device_manager(
	    wayland_ct, WAYLAND_SELECTION_REGULAR, &supported);

    എങ്കിൽ (clip_wl.regular.manager != ശൂന്യ)
    {
	clip_wl.regular.device = vwl_data_device_manager_get_data_device(
		clip_wl.regular.manager, clip_wl.seat);

	എങ്കിൽ (clip_wl.regular.device != ശൂന്യ)
	    clip_wl.regular.available = true;
	അല്ലെങ്കിൽ
	{
	    // Shouldn't happen
	    vwl_data_device_manager_discard(clip_wl.regular.manager);
	    clip_wl.regular.manager = ശൂന്യ;
	    തിരികെ FAIL;
	}
    }

    // If we still don't support the primary selection, try finding one for it
    // specifically.
    എങ്കിൽ (!(supported & WAYLAND_SELECTION_PRIMARY))
    {
	clip_wl.primary.manager = vwl_connection_get_data_device_manager(
		wayland_ct, WAYLAND_SELECTION_PRIMARY, &supported);

	എങ്കിൽ (clip_wl.primary.manager != ശൂന്യ)
	{
	    clip_wl.primary.device = vwl_data_device_manager_get_data_device(
		    clip_wl.primary.manager, clip_wl.seat);

	    എങ്കിൽ (clip_wl.primary.device != ശൂന്യ)
		clip_wl.primary.available = true;
	    അല്ലെങ്കിൽ
	    {
		vwl_data_device_manager_discard(clip_wl.primary.manager);
		clip_wl.primary.manager = ശൂന്യ;
	    }
	}
    }

    എങ്കിൽ (clip_wl.regular.available && !clip_wl.primary.available)
    {
	// The protocol supports both regular and primary selections, just use
	// one data device manager and one data device. Or the primary selection
	// is not supported, make it point to the regular selection instead.
	clip_wl.primary.available = true;
	clip_wl.primary.manager = clip_wl.regular.manager;
	clip_wl.primary.device = clip_wl.regular.device;
    }

    എങ്കിൽ (!clip_wl.regular.available && !clip_wl.primary.available)
	തിരികെ FAIL;

    // Start listening for selection updates
    എങ്കിൽ (clip_wl.regular.device != ശൂന്യ)
	vwl_data_device_add_listener(clip_wl.regular.device,
		&vwl_data_device_listener, ശൂന്യ);
    // Don't want to listen to the same data device twice
    എങ്കിൽ (clip_wl.primary.device != ശൂന്യ
	    && clip_wl.primary.device != clip_wl.regular.device)
	vwl_data_device_add_listener(clip_wl.primary.device,
		&vwl_data_device_listener, ശൂന്യ);

    തിരികെ OK;
}

    ശൂന്യം
clip_uninit_wayland(ശൂന്യം)
{
    clip_wl_selection_T *sel;

    എങ്കിൽ (clipmethod == CLIPMETHOD_WAYLAND)
    {
	എങ്കിൽ (clip_star.owned)
	    clip_lose_selection(&clip_star);
	എങ്കിൽ (clip_plus.owned)
	    clip_lose_selection(&clip_plus);
    }

    // Don't want to double free
    എങ്കിൽ (clip_wl.regular.manager != clip_wl.primary.manager)
	vwl_data_device_manager_discard(clip_wl.primary.manager);
    vwl_data_device_manager_discard(clip_wl.regular.manager);

    എങ്കിൽ (clip_wl.regular.device != clip_wl.primary.device)
	vwl_data_device_destroy(clip_wl.primary.device);
    vwl_data_device_destroy(clip_wl.regular.device);

    sel = &clip_wl.regular;
    ആകുമ്പോൾ (true)
    {
	vim_free(sel->contents);
	vwl_data_source_destroy(sel->source);
	vwl_data_offer_destroy(sel->offer);
	sel->available = false;

	എങ്കിൽ (sel == &clip_wl.primary)
	    നിർത്തുക;
	sel = &clip_wl.primary;
    }

    vim_memset(&clip_wl, 0, വലുപ്പം(clip_wl));
}

    പൂർണ്ണസംഖ്യ
clip_reset_wayland(ശൂന്യം)
{
    wayland_uninit_connection();

    എങ്കിൽ (wayland_init_connection(wayland_display_name) == FAIL
	    || clip_init_wayland() == FAIL)
	തിരികെ FAIL;

    choose_clipmethod();
    തിരികെ OK;
}


/*
 * Read data from a file descriptor and write it to the given clipboard.
 */
    സ്ഥിതം ശൂന്യം
clip_wl_receive_data(Clipboard_T *cbd, സ്ഥിരം അക്ഷരം *mime_type, പൂർണ്ണസംഖ്യ fd)
{
    char_u	*start, *final;
    long	len;
    garray_T	buf;
    പൂർണ്ണസംഖ്യ		motion_type = MAUTO;
    ssize_t	r = 0;
    char_u	*tofree = ശൂന്യ;
#  ifndef HAVE_SELECT
    ഘടന pollfd   pfd;

    pfd.fd = fd;
    pfd.events = POLLIN;
#  അല്ലെങ്കിൽ
    fd_set rfds;
    ഘടന timeval  tv;

    FD_ZERO(&rfds);
    FD_SET(fd, &rfds);
#  endif

    // Make pipe (read end) non-blocking
    എങ്കിൽ (fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK) == -1)
	തിരികെ;

    ga_init2(&buf, 1, 4096);

    // 4096 bytes seems reasonable for initial buffer size, memory is cheap
    // anyways.
    എങ്കിൽ (ga_grow(&buf, 4096) == FAIL)
	തിരികെ;

    start = buf.ga_data;

#  ifndef HAVE_SELECT
    ആകുമ്പോൾ (poll(&pfd, 1, p_wtm) > 0)
#  അല്ലെങ്കിൽ
    ആകുമ്പോൾ (tv.tv_sec = p_wtm / 1000, tv.tv_usec = (p_wtm % 1000) * 1000,
	    select(fd + 1, &rfds, ശൂന്യ, ശൂന്യ, &tv) > 0)
#  endif
    {
	r = പഠ(fd, start, buf.ga_maxlen - 1 - buf.ga_len);

	എങ്കിൽ (r == 0)
	    നിർത്തുക;
	അല്ലെങ്കിൽ എങ്കിൽ (r < 0)
	{
	    എങ്കിൽ (errno == EAGAIN || errno == EINTR)
		തുടരുക;
	    നിർത്തുക;
	}

	start += r;
	buf.ga_len += r;

	// Realloc if we are at the end of the buffer
	എങ്കിൽ (buf.ga_len >= buf.ga_maxlen - 1)
	{
	    എങ്കിൽ (ga_grow(&buf, 8192) == FAIL)
		നിർത്തുക;
	    start = (char_u *)buf.ga_data + buf.ga_len;
	}
    }

    എങ്കിൽ (buf.ga_len == 0)
    {
	clip_free_selection(cbd); // Nothing received, clear register
	ga_clear(&buf);
	തിരികെ;
    }

    final = buf.ga_data;
    len = buf.ga_len;

    എങ്കിൽ (clip_convert_data(&final, &len, &motion_type,
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
    സ്ഥിതം ശൂന്യം
clip_wl_request_selection(Clipboard_T *cbd)
{
    clip_wl_selection_T *sel = clip_wl_get_selection_from_cbd(cbd);
    പൂർണ്ണസംഖ്യ			fds[2];
    പൂർണ്ണസംഖ്യ			mime_types_len;
    സ്ഥിരം അക്ഷരം		**mime_types;
    സ്ഥിരം അക്ഷരം		*chosen_mime = ശൂന്യ;

    എങ്കിൽ (!sel->available)
	പോകുക clear;

    // Dispatch any events that still queued up before checking for a data
    // offer.
    എങ്കിൽ (vwl_connection_roundtrip(wayland_ct) == FAIL)
	പോകുക clear;

    എങ്കിൽ (sel->offer == ശൂന്യ)
	പോകുക clear;

    mime_types_len = sel->offer->mime_types.ga_len;
    mime_types = sel->offer->mime_types.ga_data;

    // Choose mime type to receive from. Mime types with a lower index in the
    // "supported_mimes" array are prioritized over ones after it.
    വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < (പൂർണ്ണസംഖ്യ)ARRAY_LENGTH(supported_mimes)
	    && chosen_mime == ശൂന്യ; i++)
    {
	വേണ്ടി (പൂർണ്ണസംഖ്യ k = 0; k < mime_types_len && chosen_mime == ശൂന്യ; k++)
	    എങ്കിൽ (STRCMP(mime_types[k], supported_mimes[i]) == 0)
		chosen_mime = supported_mimes[i];
    }

    എങ്കിൽ (chosen_mime == ശൂന്യ || pipe(fds) == -1)
	പോകുക clear;

    vwl_data_offer_receive(sel->offer, chosen_mime, fds[1]);

    close(fds[1]); // Close before we read data so that when the source client
		   // closes their end we receive an EOF.

    എങ്കിൽ (vwl_connection_flush(wayland_ct) >= 0)
	clip_wl_receive_data(cbd, chosen_mime, fds[0]);

    close(fds[0]);

    തിരികെ;
clear:
    clip_free_selection(cbd);
}

    സ്ഥിതം ശൂന്യം
vwl_data_source_listener_event_send(
    ശൂന്യം *data,
    vwl_data_source_T *source UNUSED,
    സ്ഥിരം അക്ഷരം *mime_type,
    int32_t fd
)
{
    clip_wl_selection_T *sel = data;
    Clipboard_T		*cbd = clip_wl_get_cbd_from_selection(sel);
    bool		have_mime = false;
    പൂർണ്ണസംഖ്യ			motion_type;
    long_u		length;
    char_u		*string; // Will be reallocated to a bigger size if
				 // needed.
    പൂർണ്ണസംഖ്യ			offset = 0;
    bool		is_vim, is_vimenc;
    size_t		total = 0;
#  ifndef HAVE_SELECT
    ഘടന pollfd   pfd;

    pfd.fd = fd;
    pfd.events = POLLOUT;
#  അല്ലെങ്കിൽ
    fd_set	    wfds;
    ഘടന timeval  tv;

    FD_ZERO(&wfds);
    FD_SET(fd, &wfds);
#  endif

    // Check if we actually have mime type
    വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < (പൂർണ്ണസംഖ്യ)ARRAY_LENGTH(supported_mimes); i++)
	എങ്കിൽ (STRCMP(supported_mimes[i], mime_type) == 0)
	{
	    have_mime = true;
	    നിർത്തുക;
	}

    എങ്കിൽ (!have_mime)
	പോകുക exit;

    // First byte sent is motion type for vim specific formats. For the vimenc
    // format, after the first byte is the encoding type, which is null
    // terminated.

    is_vimenc = STRCMP(mime_type, VIMENC_ATOM_NAME) == 0
	|| STRCMP(mime_type, VIMENC_MIMETYPE_NAME) == 0;
    is_vim = STRCMP(mime_type, VIM_ATOM_NAME) == 0
	|| STRCMP(mime_type, VIM_MIMETYPE_NAME) == 0;

    എങ്കിൽ (is_vimenc)
	offset += 2 + STRLEN(p_enc);
    അല്ലെങ്കിൽ എങ്കിൽ (is_vim)
	offset += 1;

    clip_get_selection(cbd);
    motion_type = clip_convert_selection_offset(&string, &length, offset, cbd);

    എങ്കിൽ (motion_type < 0)
	പോകുക exit;

    എങ്കിൽ (is_vimenc)
    {
	string[0] = (char_u)motion_type;
	// Use vim_strncpy for safer copying
	vim_strncpy(string + 1, p_enc, STRLEN(p_enc));
    }
    അല്ലെങ്കിൽ എങ്കിൽ (is_vim)
	string[0] = (char_u)motion_type;


    ആകുമ്പോൾ (total < (size_t)length &&
#  ifndef HAVE_SELECT
	    poll(&pfd, 1, p_wtm) > 0)
#  അല്ലെങ്കിൽ
	    ((tv.tv_sec = p_wtm / 1000, tv.tv_usec = (p_wtm % 1000) * 1000),
	    select(fd + 1, ശൂന്യ, &wfds, ശൂന്യ, &tv) > 0))
#  endif
    {
	ssize_t w = ലിഖ(fd, string + total, length - total);

	എങ്കിൽ (w == -1)
	    നിർത്തുക;
	total += w;
    }

    vim_free(string);
exit:
    close(fd);
}

    സ്ഥിതം ശൂന്യം
vwl_data_source_listener_event_cancelled(
	ശൂന്യം *data,
	vwl_data_source_T *source UNUSED)
{
    clip_wl_selection_T *sel = data;
    Clipboard_T		*cbd = clip_wl_get_cbd_from_selection(sel);

    clip_lose_selection(cbd);
}

സ്ഥിതം സ്ഥിരം vwl_data_source_listener_T vwl_data_source_listener = {
    .send = vwl_data_source_listener_event_send,
    .cancelled = vwl_data_source_listener_event_cancelled
};

    സ്ഥിതം ശൂന്യം
clip_wl_do_set_selection(ശൂന്യം *data, uint32_t serial)
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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
clip_wl_own_selection(Clipboard_T *cbd)
{
    clip_wl_selection_T *sel = clip_wl_get_selection_from_cbd(cbd);
    wayland_selection_T sel_type = clip_wl_get_selection_type(sel);

    എങ്കിൽ (!sel->available || vwl_connection_roundtrip(wayland_ct) == FAIL)
	തിരികെ FAIL;

    എങ്കിൽ (sel->source != ശൂന്യ)
    {
	എങ്കിൽ (sel_type == WAYLAND_SELECTION_PRIMARY)
	    // We already own the selection, ignore (only do this for primary
	    // selection). We don't re set the selection because then we would
	    // be setting the selection every time the user moves the visual
	    // selection cursor, which is messy and inefficient. Some
	    // applications like Google Chrome do it this way however.
	    തിരികെ OK;
	അല്ലെങ്കിൽ എങ്കിൽ (sel_type == WAYLAND_SELECTION_REGULAR)
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
	അല്ലെങ്കിൽ
	    // Shouldn't happen
	    തിരികെ FAIL;
    }

    sel->source = vwl_data_device_manager_create_data_source(sel->manager);
    vwl_data_source_add_listener(sel->source, &vwl_data_source_listener, sel);

    // Advertise mime types
    വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < (പൂർണ്ണസംഖ്യ)ARRAY_LENGTH(supported_mimes); i++)
	vwl_data_source_offer(sel->source, supported_mimes[i]);

    sel->own_success = false;
    clip_wl_do_set_selection(sel, 0);

    എങ്കിൽ (!sel->own_success)
	പോകുക fail;

    തിരികെ OK;
fail:
    vwl_data_source_destroy(sel->source);
    sel->source = ശൂന്യ;
    തിരികെ FAIL;
}

/*
 * Disown the selection that cbd corresponds to.
 */
    സ്ഥിതം ശൂന്യം
clip_wl_lose_selection(Clipboard_T *cbd)
{
    clip_wl_selection_T *sel = clip_wl_get_selection_from_cbd(cbd);

    എങ്കിൽ (!sel->available)
	തിരികെ;

    vwl_data_source_destroy(sel->source);
    sel->source = ശൂന്യ;
}

/*
 * Send the current selection to the clipboard. Do nothing for Wayland because
 * we will fill in the selection only when requested by another client.
 */
    സ്ഥിതം ശൂന്യം
clip_wl_set_selection(Clipboard_T *cbd UNUSED)
{
}

#  എങ്കിൽ defined(USE_SYSTEM)
/*
 * Return true if we own the selection corresponding to cbd or another client
 * does.
 */
    സ്ഥിതം bool
clip_wl_owner_exists(Clipboard_T *cbd)
{
    clip_wl_selection_T *sel = clip_wl_get_selection_from_cbd(cbd);

    എങ്കിൽ (vwl_connection_roundtrip(wayland_ct) == FAIL)
	തിരികെ false;

    തിരികെ sel->available && (sel->source != ശൂന്യ || sel->offer != ശൂന്യ);
}
#  endif

# endif // FEAT_WAYLAND_CLIPBOARD

#endif // FEAT_CLIPBOARD

#ifdef HAVE_CLIPMETHOD

/*
 * Returns the first method for accessing the clipboard that is available/works,
 * depending on the order of values in str.
 */
    സ്ഥിതം clipmethod_T
get_clipmethod(char_u *str)
{
    പൂർണ്ണസംഖ്യ		len	= (പൂർണ്ണസംഖ്യ)STRLEN(str) + 1;
    char_u	*buf	= alloc(len);

    എങ്കിൽ (buf == ശൂന്യ)
	തിരികെ CLIPMETHOD_FAIL;

    clipmethod_T ret = CLIPMETHOD_FAIL;
    char_u	*p = str;

    ആകുമ്പോൾ (*p != NUL)
    {
	clipmethod_T method = CLIPMETHOD_NONE;

	(ശൂന്യം)copy_option_part(&p, buf, len, ",");

	എങ്കിൽ (STRCMP(buf, "wayland") == 0)
	{
# ifdef FEAT_GUI
	    എങ്കിൽ (!gui.in_use)
# endif
	    {
# ifdef FEAT_WAYLAND_CLIPBOARD
		എങ്കിൽ (clip_wl.regular.available || clip_wl.primary.available)
		    method = CLIPMETHOD_WAYLAND;
# endif
	    }
	}
	അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(buf, "x11") == 0)
	{
# ifdef FEAT_GUI
	    എങ്കിൽ (!gui.in_use)
# endif
	    {
# ifdef FEAT_XCLIPBOARD
		// x_IOerror_handler() in os_unix.c should set xterm_dpy to NULL if
		// we lost connection to the X server.
		എങ്കിൽ (xterm_dpy != ശൂന്യ)
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
	അല്ലെങ്കിൽ
	{
# ifdef FEAT_CLIPBOARD_PROVIDER
	    // Check if name matches a clipboard provider
	    പൂർണ്ണസംഖ്യ r = clip_provider_is_available(buf);

	    എങ്കിൽ (r == 1)
	    {
		method = CLIPMETHOD_PROVIDER;
		എങ്കിൽ (ret == CLIPMETHOD_FAIL)
		{
		    vim_free(clip_provider);
		    clip_provider = vim_strsave(buf);
		    എങ്കിൽ (clip_provider == ശൂന്യ)
			പോകുക fail;
		}
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (r == -1)
# endif
	    {
# ifdef FEAT_CLIPBOARD_PROVIDER
fail:
# endif
		ret = CLIPMETHOD_FAIL;
		പോകുക exit;
	    }
	}

	// Keep on going in order to catch errors
	എങ്കിൽ (method != CLIPMETHOD_NONE && ret == CLIPMETHOD_FAIL)
	    ret = method;
    }

    // No match found, use "none".
    ret = (ret == CLIPMETHOD_FAIL) ? CLIPMETHOD_NONE : ret;

exit:
    vim_free(buf);
    തിരികെ ret;
}


/*
 * Returns name of clipmethod in a statically allocated string.
 */
    സ്ഥിതം char_u *
clipmethod_to_str(clipmethod_T method)
{
    തിരഞ്ഞെടുപ്പ്(method)
    {
	അവസ്ഥ CLIPMETHOD_WAYLAND:
	    തിരികെ (char_u *)"wayland";
	അവസ്ഥ CLIPMETHOD_X11:
	    തിരികെ (char_u *)"x11";
	അവസ്ഥ CLIPMETHOD_PROVIDER:
# ifdef FEAT_EVAL
	    തിരികെ clip_provider;
# endif
	സ്ഥിരസ്ഥിതി:
	    തിരികെ (char_u *)"none";
    }
}

/*
 * Sets the current clipmethod to use given by `get_clipmethod()`. Returns an
 * error message on failure else NULL.
 */
    അക്ഷരം *
choose_clipmethod(ശൂന്യം)
{
    clipmethod_T method = get_clipmethod(p_cpm);

    എങ്കിൽ (method == CLIPMETHOD_FAIL)
	തിരികെ e_invalid_argument;

// If GUI is running or we are not on a system with Wayland or X11, then always
// return CLIPMETHOD_NONE. System or GUI clipboard handling always overrides.
// This is unless a provider is being used.
# എങ്കിൽ defined(FEAT_XCLIPBOARD) || defined(FEAT_WAYLAND_CLIPBOARD)
#  എങ്കിൽ defined(FEAT_GUI)
    എങ്കിൽ (method != CLIPMETHOD_PROVIDER && gui.in_use)
    {
#   ifdef FEAT_WAYLAND
	// We only interact with Wayland for the clipboard, we can just deinit
	// everything.
	wayland_uninit_connection();
#   endif

	method = CLIPMETHOD_NONE;
	പോകുക lose_sel_exit;
    }
#  endif
# അല്ലെങ്കിൽ
    // If on a system like windows or macos, then clipmethod is irrelevant, we
    // use their way of accessing the clipboard. This is unless we are using the
    // clipboard provider
#  ifdef FEAT_CLIPBOARD_PROVIDER
    എങ്കിൽ (method != CLIPMETHOD_PROVIDER)
#  endif
    {
	method = CLIPMETHOD_NONE;
	പോകുക exit;
    }
# endif

# ifdef FEAT_CLIPBOARD
    // Deinitialize clipboard if there is no way to access clipboard
    എങ്കിൽ (method == CLIPMETHOD_NONE)
	clip_init(FALSE);
    // If we have a clipmethod that works now, then initialize clipboard
    അല്ലെങ്കിൽ എങ്കിൽ (clipmethod == CLIPMETHOD_NONE && method != CLIPMETHOD_NONE)
    {
	clip_init(TRUE);
	did_warn_clipboard = false;
    }
    // Disown clipboard if we are switching to a new method
    അല്ലെങ്കിൽ എങ്കിൽ (clipmethod != CLIPMETHOD_NONE && method != clipmethod)
    {
#  എങ്കിൽ (defined(FEAT_XCLIPBOARD) || defined(FEAT_WAYLAND_CLIPBOARD)) \
	&& defined(FEAT_GUI)
lose_sel_exit:
#  endif
	എങ്കിൽ (clip_star.owned)
	    clip_lose_selection(&clip_star);
	എങ്കിൽ (clip_plus.owned)
	    clip_lose_selection(&clip_plus);

#  എങ്കിൽ defined(FEAT_GUI)
	എങ്കിൽ (!gui.in_use)
#  endif
	{
	    clip_init(TRUE);
	    did_warn_clipboard = false;
	}
    }
# endif // FEAT_CLIPBOARD

# എങ്കിൽ !defined(FEAT_XCLIPBOARD) && !defined(FEAT_WAYLAND_CLIPBOARD)
exit:
# endif

    clipmethod = method;

# ifdef FEAT_EVAL
    set_vim_var_string(VV_CLIPMETHOD, (char_u*)clipmethod_to_str(method), -1);
# endif

    തിരികെ ശൂന്യ;
}

/*
 * Call choose_clipmethod().
 */
    ശൂന്യം
ex_clipreset(exarg_T *eap UNUSED)
{
    clipmethod_T prev = clipmethod;

    choose_clipmethod();

    എങ്കിൽ (clipmethod == CLIPMETHOD_NONE)
	smsg(_("Could not find a way to access the clipboard."));
    അല്ലെങ്കിൽ എങ്കിൽ (clipmethod != prev)
	smsg(_("Switched to clipboard method '%s'."),
		clipmethod_to_str(clipmethod));
}

/*
 * Adjust the register name pointed to with "rp" for the clipboard being
 * used always and the clipboard being available.
 */
    ശൂന്യം
adjust_clip_reg(പൂർണ്ണസംഖ്യ *rp)
{
# ifdef FEAT_CLIPBOARD_PROVIDER
    എങ്കിൽ (clipmethod == CLIPMETHOD_PROVIDER)
    {
	എങ്കിൽ (*rp == 0 && clip_unnamed != 0)
	    *rp = ((clip_unnamed & CLIP_UNNAMED_PLUS)) ? '+' : '*';
	തിരികെ;
    }
# endif
# ifdef FEAT_CLIPBOARD
    // If no reg. specified, and "unnamed" or "unnamedplus" is in 'clipboard',
    // use '*' or '+' reg, respectively. "unnamedplus" prevails.
    എങ്കിൽ (*rp == 0 && (clip_unnamed != 0 || clip_unnamed_saved != 0))
    {
	എങ്കിൽ (clip_unnamed != 0)
	    *rp = ((clip_unnamed & CLIP_UNNAMED_PLUS) && clip_plus.available)
								  ? '+' : '*';
	അല്ലെങ്കിൽ
	    *rp = ((clip_unnamed_saved & CLIP_UNNAMED_PLUS)
					   && clip_plus.available) ? '+' : '*';
    }
    എങ്കിൽ ((!clip_star.available && *rp == '*') ||
	   (!clip_plus.available && *rp == '+'))
    {
	സന്ദേശം_warn_missing_clipboard();
	*rp = 0;
    }
# endif
}

/*
 * Extract the items in the 'clipboard' option and set global values.
 * Return an error message or NULL for success.
 */
    അക്ഷരം *
did_set_clipboard(optset_T *args UNUSED)
{
    പൂർണ്ണസംഖ്യ		new_unnamed = 0;
# ifdef FEAT_CLIPBOARD
    പൂർണ്ണസംഖ്യ		new_autoselect_star = FALSE;
    പൂർണ്ണസംഖ്യ		new_autoselect_plus = FALSE;
    പൂർണ്ണസംഖ്യ		new_autoselectml = FALSE;
    പൂർണ്ണസംഖ്യ		new_html = FALSE;
# endif
    regprog_T	*new_exclude_prog = ശൂന്യ;
    അക്ഷരം	*errmsg = ശൂന്യ;
    char_u	*p;

    വേണ്ടി (p = p_cb; *p != NUL; )
    {
	// Note: Keep this in sync with p_cb_values.
	എങ്കിൽ (STRNCMP(p, "unnamed", 7) == 0 && (p[7] == ',' || p[7] == NUL))
	{
	    new_unnamed |= CLIP_UNNAMED;
	    p += 7;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (STRNCMP(p, "unnamedplus", 11) == 0
					    && (p[11] == ',' || p[11] == NUL))
	{
	    new_unnamed |= CLIP_UNNAMED_PLUS;
	    p += 11;
	}
# ifdef FEAT_CLIPBOARD
	അല്ലെങ്കിൽ എങ്കിൽ (STRNCMP(p, "autoselect", 10) == 0
					    && (p[10] == ',' || p[10] == NUL))
	{
	    new_autoselect_star = TRUE;
	    p += 10;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (STRNCMP(p, "autoselectplus", 14) == 0
					    && (p[14] == ',' || p[14] == NUL))
	{
	    new_autoselect_plus = TRUE;
	    p += 14;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (STRNCMP(p, "autoselectml", 12) == 0
					    && (p[12] == ',' || p[12] == NUL))
	{
	    new_autoselectml = TRUE;
	    p += 12;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (STRNCMP(p, "html", 4) == 0 && (p[4] == ',' || p[4] == NUL))
	{
	    new_html = TRUE;
	    p += 4;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (STRNCMP(p, "exclude:", 8) == 0 && new_exclude_prog == ശൂന്യ)
	{
	    p += 8;
	    new_exclude_prog = vim_regcomp(p, RE_MAGIC);
	    എങ്കിൽ (new_exclude_prog == ശൂന്യ)
		errmsg = e_invalid_argument;
	    നിർത്തുക;
	}
# endif
	അല്ലെങ്കിൽ
	{
	    errmsg = e_invalid_argument;
	    നിർത്തുക;
	}
	എങ്കിൽ (*p == ',')
	    ++p;
    }
    എങ്കിൽ (errmsg == ശൂന്യ)
    {
# ifdef FEAT_CLIPBOARD
	എങ്കിൽ (global_busy)
	    // clip_unnamed will be reset to clip_unnamed_saved
	    // at end_global_changes
	    clip_unnamed_saved = new_unnamed;
	അല്ലെങ്കിൽ
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
# എങ്കിൽ defined(FEAT_GUI_GTK)
	എങ്കിൽ (gui.in_use)
	{
#  ifdef USE_GTK4
	    gui_gtk_update_selection_formats(&clip_plus);
	    gui_gtk_update_selection_formats(&clip_star);
#  അല്ലെങ്കിൽ
	    gui_gtk_set_selection_targets((GdkAtom)GDK_SELECTION_PRIMARY);
	    gui_gtk_set_selection_targets((GdkAtom)clip_plus.gtk_sel_atom);
#  endif
#  ifdef FEAT_DND
	    gui_gtk_set_dnd_targets();
#  endif
	}
# endif
    }
    അല്ലെങ്കിൽ
	vim_regfree(new_exclude_prog);

    തിരികെ errmsg;
}

#endif // HAVE_CLIPMETHOD

#ifdef FEAT_CLIPBOARD_PROVIDER

/*
 * Check if a clipboard provider with given name is available. Returns 1 if available,
 * 0 if not available, and -1 on error
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
clip_provider_is_available(char_u *provider)
{
    dict_T	*providers = get_vim_var_dict(VV_CLIPPROVIDERS);
    typval_T	provider_tv = {0};
    callback_T	callback = {0};
    typval_T	rettv = {0};
    typval_T	func_tv = {0};
    പൂർണ്ണസംഖ്യ		res = 0;

    എങ്കിൽ (dict_get_tv(providers, (അക്ഷരം *)provider, &provider_tv) == FAIL
	    || provider_tv.v_type != VAR_DICT)
	// clipboard provider not defined
	തിരികെ -1;

    എങ്കിൽ (dict_get_tv(provider_tv.vval.v_dict, "available", &func_tv) == FAIL)
    {
	clear_tv(&provider_tv);
	// If "available" function not specified assume always TRUE
	തിരികെ 1;
    }

    എങ്കിൽ ((callback = get_callback(&func_tv)).cb_name == ശൂന്യ)
	പോകുക fail;

    എങ്കിൽ (call_callback(&callback, -1, &rettv, 0, ശൂന്യ) == FAIL ||
	    (rettv.v_type != VAR_BOOL && rettv.v_type != VAR_സംഖ്യ))
	പോകുക fail;

    എങ്കിൽ (rettv.vval.v_സംഖ്യ)
	res = 1;

    എങ്കിൽ (FALSE)
fail:
	res = -1;

    free_callback(&callback);
    clear_tv(&func_tv);
    clear_tv(&rettv);
    clear_tv(&provider_tv);

    തിരികെ res;
}

/*
 * Get the specified callback "function" from the provider dictionary for
 * register "reg".
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
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

    എങ്കിൽ (dict_get_tv(providers, (അക്ഷരം *)provider, &provider_tv) == FAIL)
	തിരികെ FAIL;
    അല്ലെങ്കിൽ എങ്കിൽ (provider_tv.v_type != VAR_DICT)
    {
	clear_tv(&provider_tv);
	തിരികെ FAIL;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (dict_get_tv(
		provider_tv.vval.v_dict,
		(അക്ഷരം *)function,
		&action_tv) == FAIL)
    {
	clear_tv(&provider_tv);
	തിരികെ FAIL;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (action_tv.v_type != VAR_DICT)
    {
	clear_tv(&provider_tv);
	clear_tv(&action_tv);
	തിരികെ FAIL;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (dict_get_tv(action_tv.vval.v_dict, (അക്ഷരം *)reg, &func_tv) == FAIL)
    {
	clear_tv(&provider_tv);
	clear_tv(&action_tv);
	തിരികെ FAIL;
    }
    അല്ലെങ്കിൽ എങ്കിൽ ((cb = get_callback(&func_tv)).cb_name == ശൂന്യ)
    {
	clear_tv(&provider_tv);
	clear_tv(&action_tv);
	clear_tv(&func_tv);
	തിരികെ FAIL;
    }
    clear_tv(&provider_tv);
    clear_tv(&action_tv);

    // func_tv owns the function name, so we must make a copy for the callback
    set_callback(callback, &cb);
    clear_tv(&func_tv);
    തിരികെ OK;
}

    സ്ഥിതം ശൂന്യം
clip_provider_copy(char_u *reg, char_u *provider)
{
    സ്ഥിതം bool	recursive = false;
    callback_T	callback;
    typval_T	rettv;
    typval_T	argvars[4];
    yankreg_T	*y_ptr;
    char_u	type[2 + NUMBUFLEN] = {0};
    list_T	*list = ശൂന്യ;

    എങ്കിൽ (recursive)
	തിരികെ;

    എങ്കിൽ (clip_provider_get_callback(
		reg,
		provider,
		(char_u *)"copy",
		&callback) == FAIL)
	തിരികെ;

    // Convert register type into a string
    എങ്കിൽ (*reg == '+')
	y_ptr = get_y_register(REAL_PLUS_REGISTER);
    അല്ലെങ്കിൽ
	y_ptr = get_y_register(STAR_REGISTER);

    തിരഞ്ഞെടുപ്പ് (y_ptr->y_type)
    {
	അവസ്ഥ MCHAR:
	    type[0] = 'v';
	    നിർത്തുക;
	അവസ്ഥ MLINE:
	    type[0] = 'V';
	    നിർത്തുക;
	അവസ്ഥ MBLOCK:
	    sprintf((അക്ഷരം *)type, "%c%d", Ctrl_V, y_ptr->y_width + 1);
	    നിർത്തുക;
	സ്ഥിരസ്ഥിതി:
	    type[0] = 0;
	    നിർത്തുക;
    }

    argvars[0].v_type = VAR_STRING;
    argvars[0].vval.v_string = reg;

    argvars[1].v_type = VAR_STRING;
    argvars[1].vval.v_string = type;

    // Get register contents by creating a list of lines
    list = list_alloc();

    എങ്കിൽ (list == ശൂന്യ)
    {
	free_callback(&callback);
	തിരികെ;
    }

    വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < y_ptr->y_size; i++)
	എങ്കിൽ (list_append_string(list, y_ptr->y_array[i].string,
	    (പൂർണ്ണസംഖ്യ)y_ptr->y_array[i].length) == FAIL)
	{
	    free_callback(&callback);
	    list_unref(list);
	    തിരികെ;
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

    സ്ഥിതം ശൂന്യം
clip_provider_paste(char_u *reg, char_u *provider)
{
    സ്ഥിതം bool	recursive = false;
    callback_T	callback;
    typval_T	argvars[2];
    typval_T	rettv;
    പൂർണ്ണസംഖ്യ		ret;
    char_u	*reg_type;
    list_T	*വരികൾ;

    എങ്കിൽ (recursive)
	തിരികെ;

    എങ്കിൽ (clip_provider_get_callback(
		reg,
		provider,
		(char_u *)"paste",
		&callback) == FAIL)
	തിരികെ;

    argvars[0].v_type = VAR_STRING;
    argvars[0].vval.v_string = reg;

    argvars[1].v_type = VAR_UNKNOWN;

    textlock++;
    recursive = true;
    ret = call_callback(&callback, -1, &rettv, 1, argvars);
    recursive = false;
    textlock--;

    എങ്കിൽ (ret == FAIL)
	പോകുക exit;
    അല്ലെങ്കിൽ എങ്കിൽ (rettv.v_type == VAR_TUPLE
	    && TUPLE_LEN(rettv.vval.v_tuple) == 2
	    && TUPLE_ITEM(rettv.vval.v_tuple, 0)->v_type == VAR_STRING
	    && TUPLE_ITEM(rettv.vval.v_tuple, 1)->v_type == VAR_LIST)
    {
	reg_type = TUPLE_ITEM(rettv.vval.v_tuple, 0)->vval.v_string;
	വരികൾ = TUPLE_ITEM(rettv.vval.v_tuple, 1)->vval.v_list;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (rettv.v_type == VAR_LIST
	    && rettv.vval.v_list->lv_len == 2
	    && rettv.vval.v_list->lv_first->li_tv.v_type == VAR_STRING
	    && rettv.vval.v_list->lv_first->li_next->li_tv.v_type == VAR_LIST)
    {
	reg_type = rettv.vval.v_list->lv_first->li_tv.vval.v_string;
	വരികൾ = rettv.vval.v_list->lv_first->li_next->li_tv.vval.v_list;
    }
    അല്ലെങ്കിൽ
	പോകുക exit;

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
	പൂർണ്ണസംഖ്യ		len;

	// If the list is NULL handle like an empty list.
	len = വരികൾ == ശൂന്യ ? 0 : വരികൾ->lv_len;

	// First half: use for pointers to result lines; second half: use for
	// pointers to allocated copies.
	lstval = ALLOC_MULT(char_u *, (len + 1) * 2);
	എങ്കിൽ (lstval == ശൂന്യ)
	    പോകുക exit;
	curval = lstval;
	allocval = lstval + len + 2;
	curallocval = allocval;

	എങ്കിൽ (വരികൾ != ശൂന്യ)
	{
	    CHECK_LIST_MATERIALIZE(വരികൾ);
	    FOR_ALL_LIST_ഇനങ്ങൾ(വരികൾ, li)
	    {
		strval = tv_get_string_buf_chk(&li->li_tv, buf);
		എങ്കിൽ (strval == ശൂന്യ)
		    പോകുക free_lstval;
		എങ്കിൽ (strval == buf)
		{
		    // Need to make a copy, next tv_get_string_buf_chk() will
		    // overwrite the string.
		    strval = vim_strsave(buf);
		    എങ്കിൽ (strval == ശൂന്യ)
			പോകുക free_lstval;
		    *curallocval++ = strval;
		}
		*curval++ = strval;
	    }
	}
	*curval++ = ശൂന്യ;

	എങ്കിൽ (*reg_type != NUL && (STRLEN(reg_type) == 0
		|| get_yank_type(&reg_type, &yank_type, &block_len) == FAIL))
	{
	    emsg(e_invalid_argument);
	    പോകുക free_lstval;
	}

	എങ്കിൽ (*reg == '+')
	    y_ptr = get_y_register(REAL_PLUS_REGISTER);
	അല്ലെങ്കിൽ
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
	ആകുമ്പോൾ (curallocval > allocval)
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
സ്ഥിതം പൂർണ്ണസംഖ്യ star_pause_എണ്ണം = -2, plus_pause_എണ്ണം = -2;

    ശൂന്യം
call_clip_provider_request(പൂർണ്ണസംഖ്യ reg)
{
    എങ്കിൽ (clipmethod != CLIPMETHOD_PROVIDER)
	തിരികെ;

    എങ്കിൽ (reg == '+' && plus_pause_എണ്ണം < 0)
    {
	എങ്കിൽ (plus_pause_എണ്ണം == -1)
	    plus_pause_എണ്ണം = 1;
	clip_provider_paste((char_u *)"+", clip_provider);
    }
    അല്ലെങ്കിൽ എങ്കിൽ (reg == '*' && star_pause_എണ്ണം < 0)
    {
	എങ്കിൽ (star_pause_എണ്ണം == -1)
	    star_pause_എണ്ണം = 1;
	clip_provider_paste((char_u *)"*", clip_provider);
    }
    അല്ലെങ്കിൽ
	തിരികെ;
}

    ശൂന്യം
call_clip_provider_set(പൂർണ്ണസംഖ്യ reg)
{
    എങ്കിൽ (clipmethod != CLIPMETHOD_PROVIDER)
	തിരികെ;

    എങ്കിൽ (reg == '+' && plus_pause_എണ്ണം < 0)
    {
	എങ്കിൽ (plus_pause_എണ്ണം == -1)
	    plus_pause_എണ്ണം = 1;
	clip_provider_copy((char_u *)"+", clip_provider);
    }
    അല്ലെങ്കിൽ എങ്കിൽ (reg == '*' && star_pause_എണ്ണം < 0)
    {
	എങ്കിൽ (star_pause_എണ്ണം == -1)
	    star_pause_എണ്ണം = 1;
	clip_provider_copy((char_u *)"*", clip_provider);
    }
}

/*
 * Makes it so that the next provider call is only done once any calls after are
 * ignored, until dec_clip_provider is called the same number of times after
 * again. Note that this is per clipboard register ("+", "*")
 */
    ശൂന്യം
inc_clip_provider(ശൂന്യം)
{
    plus_pause_എണ്ണം = (plus_pause_എണ്ണം == -2
	|| plus_pause_എണ്ണം == -1) ? -1 : plus_pause_എണ്ണം + 1;
    star_pause_എണ്ണം = (star_pause_എണ്ണം == -2
	|| star_pause_എണ്ണം == -1) ? -1 : star_pause_എണ്ണം + 1;
}

    ശൂന്യം
dec_clip_provider(ശൂന്യം)
{
    എങ്കിൽ (plus_pause_എണ്ണം != -2)
	plus_pause_എണ്ണം = plus_pause_എണ്ണം == -1 ? -1 : plus_pause_എണ്ണം - 1;
    എങ്കിൽ (star_pause_എണ്ണം != -2)
	star_pause_എണ്ണം = star_pause_എണ്ണം == -1 ? -1 : star_pause_എണ്ണം - 1;

    എങ്കിൽ (plus_pause_എണ്ണം == 0 || plus_pause_എണ്ണം == -1)
	plus_pause_എണ്ണം = -2;
    എങ്കിൽ (star_pause_എണ്ണം == 0 || star_pause_എണ്ണം == -1)
	star_pause_എണ്ണം = -2;
}

#endif // FEAT_CLIPBOARD_PROVIDER

#എങ്കിൽ defined(FEAT_WAYLAND_CLIPBOARD) || (defined(FEAT_GUI_GTK) && defined(USE_GTK4))
/*
 * If "vim" is TRUE, then get the motion type. If "vimenc" is TRUE, then get the
 * motion type and also convert "*buf". "buf" and "len_store" will be updated to
 * reflect the actual contents, but should be set beforehand with the initial
 * contents. Note that if "*tofree" is not NULL, use vim_free() on "*buf",
 * otherwise use the free func for which "*buf" was allocated with. In both
 * cases use vim_free() on "*tofree". Returns OK on success and FAIL on failure.
 */
    പൂർണ്ണസംഖ്യ
clip_convert_data(
	char_u	**buf,
	long	*len_store,
	പൂർണ്ണസംഖ്യ	*motion,
	bool	vim,
	bool	vimenc,
	char_u	**tofree)
{
    char_u  *final = *buf;
    char_u  *enc;
    long    len = *len_store;

    എങ്കിൽ (vim && len >= 2)
    {
	*motion = *final++;
	len--;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (vimenc && len >= 3)
    {
	vimconv_T   conv;
	പൂർണ്ണസംഖ്യ	    convlen;

	// First byte is motion type
	*motion = *final++;
	len--;

	// Get encoding of selection
	enc = final;

	// Skip the encoding type including null terminator in final text
	final = memchr(final, NUL, len);
	എങ്കിൽ (final == ശൂന്യ)
	    തിരികെ FAIL;
	final++; // Skip NUL

	// Subtract pointers to get length of encoding;
	len -= final - enc;

	conv.vc_type = CONV_NONE;
	convert_setup(&conv, enc, p_enc);
	എങ്കിൽ (conv.vc_type != CONV_NONE)
	{
	   char_u *tmp;

	   convlen = len;
	   tmp = string_convert(&conv, final, &convlen);
	   len = convlen;
	   എങ്കിൽ (tmp != ശൂന്യ)
	   {
		final = tmp;
		*tofree = final;
	   }
	   convert_setup(&conv, ശൂന്യ, ശൂന്യ);
	}
    }
    *buf = final;
    *len_store = len;
    തിരികെ OK;
}
#endif
