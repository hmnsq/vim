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

#ਅਨ੍ਤਰ੍ਭਾਵ "vim.h"

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CYGWIN_WIN32_CLIPBOARD
# define WIN32_LEAN_AND_ਔਸਤ
# include <windows.h>
# include "winclip.pro"
#ਜੇ_ਅੰਤ

// Functions for copying and pasting text between applications.
// This is always included in a GUI version, but may also be included when the
// clipboard and mouse is available to a terminal version such as xterm.
// Note: there are some more functions in ops.c that handle selection stuff.
//
// Also note that the majority of functions here deal with the X 'primary'
// (visible - for Visual mode use) selection, and only that. There are no
// versions of these for the 'clipboard' selection, as Visual mode has no use
// for them.


#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CLIPBOARD_PROVIDER
ਸਥਿਰ ਪੂਰਨ clip_provider_is_ਉਪਲਬਧ(char_u *provider);
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_CLIPBOARD)

# ਜੇ defined(FEAT_WAYLAND_CLIPBOARD)

#  include "wayland.h"

// Represents either the regular or primary selection
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
    char_u		*contents;	// Non-null if we own selection,
					// contains the data to send to other
					// clients.
    vwl_ਅੰਕੜੇ_ਸਰੋਤ_T	*ਸਰੋਤ;	// Non-NULL if we own the selection,
					// else NULL if we don't.
    vwl_ਅੰਕੜੇ_offer_T	*offer;		// Current offer for the selection

    bool		own_success;	// Used by clip_wl_own_selection()
    bool		ਉਪਲਬਧ;	// If selection is ready to serve/use

    // These may point to the same proxy as the other selection
    vwl_ਅੰਕੜੇ_device_manager_T	*manager;
    vwl_ਅੰਕੜੇ_device_T		*device;
} clip_wl_selection_T;

// Represents the clipboard for the global Wayland connection, for the chosen
// seat (using the 'wl_seat' option)
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
    vwl_seat_T *seat;

    clip_wl_selection_T regular;
    clip_wl_selection_T primary;
} clip_wl_T;

// Mime types we support sending and receiving
// Mimes with a lower index in the array are prioritized first when we are
// receiving data.
ਸਥਿਰ ਅਚਲ ਅੱਖਰ *supported_mimes[] = {
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

ਸਥਿਰ ਖਾਲੀ
clip_wl_receive_ਅੰਕੜੇ(Clipboard_T *cbd, ਅਚਲ ਅੱਖਰ *mime_type, ਪੂਰਨ fd);
ਸਥਿਰ ਖਾਲੀ clip_wl_request_selection(Clipboard_T *cbd);
ਸਥਿਰ ਪੂਰਨ clip_wl_own_selection(Clipboard_T *cbd);
ਸਥਿਰ ਖਾਲੀ clip_wl_lose_selection(Clipboard_T *cbd);
ਸਥਿਰ ਖਾਲੀ clip_wl_ਸੈੱਟ_selection(Clipboard_T *cbd);

#  ਜੇ defined(USE_SYSTEM)
ਸਥਿਰ bool clip_wl_owner_exists(Clipboard_T *cbd);
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
    ਖਾਲੀ
clip_init(ਪੂਰਨ can_use)
{
    Clipboard_T *cb;

    cb = &clip_ਤਾਰਾ;
    ਲਈ (;;)
    {
	// No need to init again if cbd is already available
	ਜੇ (can_use && cb->ਉਪਲਬਧ)
	    ਜਾਓ skip;

	cb->ਉਪਲਬਧ  = can_use;
	cb->owned      = FALSE;
	cb->ਸ਼ੁਰੂ.lnum = 0;
	cb->ਸ਼ੁਰੂ.ਖਾਨਾ  = 0;
	cb->ਸਿਰਾ.lnum   = 0;
	cb->ਸਿਰਾ.ਖਾਨਾ    = 0;
	cb->state      = SELECT_CLEARED;

skip:
	ਜੇ (cb == &clip_plus)
	    ਰੋਕੋ;
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
    ਖਾਲੀ
clip_update_selection(Clipboard_T *clip)
{
    pos_T	    ਸ਼ੁਰੂ, ਸਿਰਾ;

    // If visual mode is only due to a redo command ("."), then ignore it
    ਜੇ (!redo_VIsual_busy && VIsual_ਸਰਗਰਮ && (State & ਬਹੁਲਕ_ਸਾਧਾਰਨ))
    {
	ਜੇ (LT_POS(VIsual, curwin->w_cursor))
	{
	    ਸ਼ੁਰੂ = VIsual;
	    ਸਿਰਾ = curwin->w_cursor;
	    ਜੇ (has_mbyte)
		ਸਿਰਾ.ਖਾਨਾ += (*mb_ptr2len)(ml_get_cursor()) - 1;
	}
	ਨਹੀਂਤਾਂ
	{
	    ਸ਼ੁਰੂ = curwin->w_cursor;
	    ਸਿਰਾ = VIsual;
	}
	ਜੇ (!EQUAL_POS(clip->ਸ਼ੁਰੂ, ਸ਼ੁਰੂ)
		|| !EQUAL_POS(clip->ਸਿਰਾ, ਸਿਰਾ)
		|| clip->vmode != VIsual_ਬਹੁਲਕ)
	{
	    clip_clear_selection(clip);
	    clip->ਸ਼ੁਰੂ = ਸ਼ੁਰੂ;
	    clip->ਸਿਰਾ = ਸਿਰਾ;
	    clip->vmode = VIsual_ਬਹੁਲਕ;
	    clip_free_selection(clip);
	    clip_own_selection(clip);
	    clip_gen_ਸੈੱਟ_selection(clip);
	}
    }
}

    ਸਥਿਰ ਪੂਰਨ
clip_gen_own_selection(Clipboard_T *cbd)
{
# ਜੇ defined(FEAT_XCLIPBOARD) || defined(FEAT_WAYLAND_CLIPBOARD)
#  ifdef FEAT_GUI
    ਜੇ (gui.ਵਿੱਚ_use)
	ਵਾਪਸ clip_mch_own_selection(cbd);
    ਨਹੀਂਤਾਂ
#  endif
    {
	ਜੇ (clipmethod == CLIPMETHOD_WAYLAND)
	{
#  ifdef FEAT_WAYLAND_CLIPBOARD
	    ਵਾਪਸ clip_wl_own_selection(cbd);
#  endif
	}
	ਨਹੀਂਤਾਂ ਜੇ (clipmethod == CLIPMETHOD_X11)
	{
#  ifdef FEAT_XCLIPBOARD
	    ਵਾਪਸ clip_xterm_own_selection(cbd);
#  endif
	}
    }
    ਵਾਪਸ FAIL;
# ਨਹੀਂਤਾਂ
    ਵਾਪਸ clip_mch_own_selection(cbd);
# endif
}

    ਖਾਲੀ
clip_own_selection(Clipboard_T *cbd)
{
    /*
     * Also want to check somehow that we are reading from the keyboard rather
     * than a mapping etc.
     */
# ਜੇ defined(FEAT_X11) || defined(FEAT_WAYLAND_CLIPBOARD)
    // Always own the selection, we might have lost it without being
    // notified, e.g. during a ":sh" command.
    ਜੇ (cbd->ਉਪਲਬਧ)
    {
	ਪੂਰਨ was_owned = cbd->owned;

	cbd->owned = (clip_gen_own_selection(cbd) == OK);
	ਜੇ (!was_owned && (cbd == &clip_ਤਾਰਾ || cbd == &clip_plus))
	{
	    // May have to show a different kind of highlighting for the
	    // selected area.  There is no specific redraw command for this,
	    // just redraw all windows on the current buffer.
	    ਜੇ (cbd->owned
		    && (get_real_state() == ਬਹੁਲਕ_VISUAL
					    || get_real_state() == ਬਹੁਲਕ_SELECT)
		    && (cbd == &clip_ਤਾਰਾ ? clip_isautosel_ਤਾਰਾ()
						      : clip_isautosel_plus())
		    && HL_ATTR(HLF_V) != HL_ATTR(HLF_VNC))
		redraw_curbuf_later(UPD_INVERTED_ALL);
	}
    }
# ਨਹੀਂਤਾਂ
    // Only own the clipboard when we didn't own it yet.
    ਜੇ (!cbd->owned && cbd->ਉਪਲਬਧ)
	cbd->owned = (clip_gen_own_selection(cbd) == OK);
# endif
}

    ਸਥਿਰ ਖਾਲੀ
clip_gen_lose_selection(Clipboard_T *cbd)
{
# ਜੇ defined(FEAT_XCLIPBOARD) || defined(FEAT_WAYLAND_CLIPBOARD)
#  ifdef FEAT_GUI
    ਜੇ (gui.ਵਿੱਚ_use)
	clip_mch_lose_selection(cbd);
    ਨਹੀਂਤਾਂ
#  endif
    {
	ਜੇ (clipmethod == CLIPMETHOD_WAYLAND)
	{
#  ifdef FEAT_WAYLAND_CLIPBOARD
	    clip_wl_lose_selection(cbd);
#  endif
	}
	ਨਹੀਂਤਾਂ ਜੇ (clipmethod == CLIPMETHOD_X11)
	{
#  ifdef FEAT_XCLIPBOARD
	    clip_xterm_lose_selection(cbd);
#  endif
	}
    }
# ਨਹੀਂਤਾਂ
    clip_mch_lose_selection(cbd);
# endif
}

    ਖਾਲੀ
clip_lose_selection(Clipboard_T *cbd)
{
# ifdef FEAT_X11
    ਪੂਰਨ	    was_owned = cbd->owned;
# endif
    ਪੂਰਨ     visual_selection = FALSE;

    ਜੇ (cbd == &clip_ਤਾਰਾ || cbd == &clip_plus)
	visual_selection = TRUE;

    clip_free_selection(cbd);
    cbd->owned = FALSE;
    ਜੇ (visual_selection)
	clip_clear_selection(cbd);
    clip_gen_lose_selection(cbd);
# ifdef FEAT_X11
    ਜੇ (visual_selection)
    {
	// May have to show a different kind of highlighting for the selected
	// area.  There is no specific redraw command for this, just redraw all
	// windows on the current buffer.
	ਜੇ (was_owned
		&& (get_real_state() == ਬਹੁਲਕ_VISUAL
		    || get_real_state() == ਬਹੁਲਕ_SELECT)
		&& (cbd == &clip_ਤਾਰਾ ?
		    clip_isautosel_ਤਾਰਾ() : clip_isautosel_plus())
		&& HL_ATTR(HLF_V) != HL_ATTR(HLF_VNC)
		&& !exiting)
	{
	    update_curbuf(UPD_INVERTED_ALL);
	    setcursor();
	    cursor_on();
	    ਬਾਹਰਲਾ_flush_cursor(TRUE, FALSE);
	}
    }
# endif
}

    ਸਥਿਰ ਖਾਲੀ
clip_copy_selection(Clipboard_T *clip)
{
    ਜੇ (VIsual_ਸਰਗਰਮ && (State & ਬਹੁਲਕ_ਸਾਧਾਰਨ) && clip->ਉਪਲਬਧ)
    {
	clip_update_selection(clip);
	clip_free_selection(clip);
	clip_own_selection(clip);
	ਜੇ (clip->owned)
	    clip_get_selection(clip);
	clip_gen_ਸੈੱਟ_selection(clip);
    }
}

/*
 * Save and restore clip_unnamed before doing possibly many changes. This
 * prevents accessing the clipboard very often which might slow down Vim
 * considerably.
 */
ਸਥਿਰ ਪੂਰਨ global_change_ਗਿਣਤੀ = 0; // if set, inside a start_global_changes
ਸਥਿਰ ਪੂਰਨ clipboard_needs_update = FALSE; // clipboard needs to be updated
ਸਥਿਰ ਪੂਰਨ clip_did_ਸੈੱਟ_selection = TRUE;

/*
 * Save clip_unnamed and reset it.
 */
    ਖਾਲੀ
ਸ਼ੁਰੂ_global_changes(ਖਾਲੀ)
{
    ਜੇ (++global_change_ਗਿਣਤੀ > 1)
	ਵਾਪਸ;
    clip_unnamed_saved = clip_unnamed;
    clipboard_needs_update = FALSE;

    ਜੇ (clip_did_ਸੈੱਟ_selection)
    {
	clip_unnamed = 0;
	clip_did_ਸੈੱਟ_selection = FALSE;
    }
}

/*
 * Return TRUE if setting the clipboard was postponed, it already contains the
 * right text.
 */
    ਸਥਿਰ ਪੂਰਨ
is_clipboard_needs_update(ਖਾਲੀ)
{
    ਵਾਪਸ clipboard_needs_update;
}

/*
 * Restore clip_unnamed and set the selection when needed.
 */
    ਖਾਲੀ
ਸਿਰਾ_global_changes(ਖਾਲੀ)
{
    ਜੇ (--global_change_ਗਿਣਤੀ > 0)
	// recursive
	ਵਾਪਸ;
    ਜੇ (!clip_did_ਸੈੱਟ_selection)
    {
	clip_did_ਸੈੱਟ_selection = TRUE;
	clip_unnamed = clip_unnamed_saved;
	clip_unnamed_saved = 0;
	ਜੇ (clipboard_needs_update)
	{
	    // only store something in the clipboard,
	    // if we have yanked anything to it
	    ਜੇ (clip_unnamed & CLIP_UNNAMED)
	    {
		clip_own_selection(&clip_ਤਾਰਾ);
		clip_gen_ਸੈੱਟ_selection(&clip_ਤਾਰਾ);
	    }
	    ਜੇ (clip_unnamed & CLIP_UNNAMED_PLUS)
	    {
		clip_own_selection(&clip_plus);
		clip_gen_ਸੈੱਟ_selection(&clip_plus);
	    }
	}
    }
    clipboard_needs_update = FALSE;
}

/*
 * Called when Visual mode is ended: update the selection.
 */
    ਖਾਲੀ
clip_auto_select(ਖਾਲੀ)
{
    ਜੇ (clip_isautosel_ਤਾਰਾ())
	clip_copy_selection(&clip_ਤਾਰਾ);
    ਜੇ (clip_isautosel_plus())
	clip_copy_selection(&clip_plus);
}

/*
 * Return TRUE if automatic selection of Visual area is desired for the *
 * register.
 */
    ਪੂਰਨ
clip_isautosel_ਤਾਰਾ(ਖਾਲੀ)
{
# ifdef FEAT_CLIPBOARD_PROVIDER
    ਜੇ (clipmethod == CLIPMETHOD_PROVIDER)
	ਵਾਪਸ false;
# endif
# ifdef FEAT_GUI
    ਜੇ (gui.ਵਿੱਚ_use)
	ਵਾਪਸ vim_strchr(p_go, GO_ASEL) != ਨਿਰਾ
	    && vim_strchr(p_go, GO_ASELPLUS) == ਨਿਰਾ;
# endif
    ਵਾਪਸ clip_autoselect_ਤਾਰਾ;
}

/*
 * Return TRUE if automatic selection of Visual area is desired for the +
 * register.
 */
    ਪੂਰਨ
clip_isautosel_plus(ਖਾਲੀ)
{
# ifdef FEAT_CLIPBOARD_PROVIDER
    ਜੇ (clipmethod == CLIPMETHOD_PROVIDER)
	ਵਾਪਸ false;
# endif
# ifdef FEAT_GUI
    ਜੇ (gui.ਵਿੱਚ_use)
	ਵਾਪਸ vim_strchr(p_go, GO_ASELPLUS) != ਨਿਰਾ;
# endif
    ਵਾਪਸ clip_autoselect_plus;
}


/*
 * Stuff for general mouse selection, without using Visual mode.
 */

/*
 * Compare two screen positions ala strcmp()
 */
    ਸਥਿਰ ਪੂਰਨ
clip_compare_pos(
    ਪੂਰਨ		row1,
    ਪੂਰਨ		col1,
    ਪੂਰਨ		row2,
    ਪੂਰਨ		col2)
{
    ਜੇ (row1 > row2) ਵਾਪਸ(1);
    ਜੇ (row1 < row2) ਵਾਪਸ(-1);
    ਜੇ (col1 > col2) ਵਾਪਸ(1);
    ਜੇ (col1 < col2) ਵਾਪਸ(-1);
    ਵਾਪਸ(0);
}

// "how" flags for clip_invert_area()
# define CLIP_CLEAR	1
# define CLIP_ਸੈੱਟ	2
# define CLIP_TOGGLE	3

/*
 * Invert or un-invert a rectangle of the screen.
 * "invert" is true if the result is inverted.
 */
    ਸਥਿਰ ਖਾਲੀ
clip_invert_rectangle(
	Clipboard_T	*cbd ਅਣਵਰਤਿਆ,
	ਪੂਰਨ		ਪੰਕਤੀ_arg,
	ਪੂਰਨ		ਖਾਨਾ_arg,
	ਪੂਰਨ		ਉਚਾਈ_arg,
	ਪੂਰਨ		ਚੌੜਾਈ_arg,
	ਪੂਰਨ		invert)
{
    ਪੂਰਨ		ਪੰਕਤੀ = ਪੰਕਤੀ_arg;
    ਪੂਰਨ		ਖਾਨਾ = ਖਾਨਾ_arg;
    ਪੂਰਨ		ਉਚਾਈ = ਉਚਾਈ_arg;
    ਪੂਰਨ		ਚੌੜਾਈ = ਚੌੜਾਈ_arg;

# ifdef FEAT_PROP_POPUP
    // this goes on top of all popup windows
    screen_zindex = CLIP_ZINDEX;

    ਜੇ (ਖਾਨਾ < cbd->min_ਖਾਨਾ)
    {
	ਚੌੜਾਈ -= cbd->min_ਖਾਨਾ - ਖਾਨਾ;
	ਖਾਨਾ = cbd->min_ਖਾਨਾ;
    }
    ਜੇ (ਚੌੜਾਈ > cbd->max_ਖਾਨਾ - ਖਾਨਾ)
	ਚੌੜਾਈ = cbd->max_ਖਾਨਾ - ਖਾਨਾ;
    ਜੇ (ਪੰਕਤੀ < cbd->min_ਪੰਕਤੀ)
    {
	ਉਚਾਈ -= cbd->min_ਪੰਕਤੀ - ਪੰਕਤੀ;
	ਪੰਕਤੀ = cbd->min_ਪੰਕਤੀ;
    }
    ਜੇ (ਉਚਾਈ > cbd->max_ਪੰਕਤੀ - ਪੰਕਤੀ + 1)
	ਉਚਾਈ = cbd->max_ਪੰਕਤੀ - ਪੰਕਤੀ + 1;
# endif
# ifdef FEAT_GUI
    ਜੇ (gui.ਵਿੱਚ_use)
	gui_mch_invert_rectangle(ਪੰਕਤੀ, ਖਾਨਾ, ਉਚਾਈ, ਚੌੜਾਈ);
    ਨਹੀਂਤਾਂ
# endif
	screen_draw_rectangle(ਪੰਕਤੀ, ਖਾਨਾ, ਉਚਾਈ, ਚੌੜਾਈ, invert, FALSE);
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
    ਸਥਿਰ ਖਾਲੀ
clip_invert_area(
	Clipboard_T	*cbd,
	ਪੂਰਨ		row1,
	ਪੂਰਨ		col1,
	ਪੂਰਨ		row2,
	ਪੂਰਨ		col2,
	ਪੂਰਨ		how)
{
    ਪੂਰਨ		invert = FALSE;
    ਪੂਰਨ		max_ਖਾਨਾ;

# ifdef FEAT_PROP_POPUP
    max_ਖਾਨਾ = cbd->max_ਖਾਨਾ - 1;
# ਨਹੀਂਤਾਂ
    max_ਖਾਨਾ = Columns - 1;
# endif

    ਜੇ (how == CLIP_ਸੈੱਟ)
	invert = TRUE;

    // Swap the from and to positions so the from is always before
    ਜੇ (clip_compare_pos(row1, col1, row2, col2) > 0)
    {
	ਪੂਰਨ tmp_ਪੰਕਤੀ, tmp_ਖਾਨਾ;

	tmp_ਪੰਕਤੀ = row1;
	tmp_ਖਾਨਾ = col1;
	row1	= row2;
	col1	= col2;
	row2	= tmp_ਪੰਕਤੀ;
	col2	= tmp_ਖਾਨਾ;
    }
    ਨਹੀਂਤਾਂ ਜੇ (how == CLIP_TOGGLE)
	invert = TRUE;

    // If all on the same line, do it the easy way
    ਜੇ (row1 == row2)
    {
	clip_invert_rectangle(cbd, row1, col1, 1, col2 - col1, invert);
    }
    ਨਹੀਂਤਾਂ
    {
	// Handle a piece of the first line
	ਜੇ (col1 > 0)
	{
	    clip_invert_rectangle(cbd, row1, col1, 1,
						  (ਪੂਰਨ)Columns - col1, invert);
	    row1++;
	}

	// Handle a piece of the last line
	ਜੇ (col2 < max_ਖਾਨਾ)
	{
	    clip_invert_rectangle(cbd, row2, 0, 1, col2, invert);
	    row2--;
	}

	// Handle the rectangle that's left
	ਜੇ (row2 >= row1)
	    clip_invert_rectangle(cbd, row1, 0, row2 - row1 + 1,
							 (ਪੂਰਨ)Columns, invert);
    }
}

/*
 * Start, continue or end a modeless selection.  Used when editing the
 * command-line, in the cmdline window and when the mouse is in a popup window.
 */
    ਖਾਲੀ
clip_modeless(ਪੂਰਨ button, ਪੂਰਨ is_click, ਪੂਰਨ is_drag)
{
    ਪੂਰਨ		repeat;

    repeat = ((clip_ਤਾਰਾ.ਬਹੁਲਕ == SELECT_ਬਹੁਲਕ_CHAR
		|| clip_ਤਾਰਾ.ਬਹੁਲਕ == SELECT_ਬਹੁਲਕ_ਸਤਰ)
					      && (mod_ਨਕਾਬ & MOD_ਨਕਾਬ_2_CLICK))
	    || (clip_ਤਾਰਾ.ਬਹੁਲਕ == SELECT_ਬਹੁਲਕ_ਸ਼ਬਦ
					     && (mod_ਨਕਾਬ & MOD_ਨਕਾਬ_3_CLICK));
    ਜੇ (is_click && button == MOUSE_ਸੱਜਾ)
    {
	// Right mouse button: If there was no selection, start one.
	// Otherwise extend the existing selection.
	ਜੇ (clip_ਤਾਰਾ.state == SELECT_CLEARED)
	    clip_ਸ਼ੁਰੂ_selection(mouse_ਖਾਨਾ, mouse_ਪੰਕਤੀ, FALSE);
	clip_process_selection(button, mouse_ਖਾਨਾ, mouse_ਪੰਕਤੀ, repeat);
    }
    ਨਹੀਂਤਾਂ ਜੇ (is_click)
	clip_ਸ਼ੁਰੂ_selection(mouse_ਖਾਨਾ, mouse_ਪੰਕਤੀ, repeat);
    ਨਹੀਂਤਾਂ ਜੇ (is_drag)
    {
	// Don't try extending a selection if there isn't one.  Happens when
	// button-down is in the cmdline and them moving mouse upwards.
	ਜੇ (clip_ਤਾਰਾ.state != SELECT_CLEARED)
	    clip_process_selection(button, mouse_ਖਾਨਾ, mouse_ਪੰਕਤੀ, repeat);
    }
    ਨਹੀਂਤਾਂ // release
	clip_process_selection(MOUSE_RELEASE, mouse_ਖਾਨਾ, mouse_ਪੰਕਤੀ, FALSE);
}

/*
 * Update the currently selected region by adding and/or subtracting from the
 * beginning or end and inverting the changed area(s).
 */
    ਸਥਿਰ ਖਾਲੀ
clip_update_modeless_selection(
    Clipboard_T    *cb,
    ਪੂਰਨ		    row1,
    ਪੂਰਨ		    col1,
    ਪੂਰਨ		    row2,
    ਪੂਰਨ		    col2)
{
    // See if we changed at the beginning of the selection
    ਜੇ (row1 != cb->ਸ਼ੁਰੂ.lnum || col1 != (ਪੂਰਨ)cb->ਸ਼ੁਰੂ.ਖਾਨਾ)
    {
	clip_invert_area(cb, row1, col1, (ਪੂਰਨ)cb->ਸ਼ੁਰੂ.lnum, cb->ਸ਼ੁਰੂ.ਖਾਨਾ,
								 CLIP_TOGGLE);
	cb->ਸ਼ੁਰੂ.lnum = row1;
	cb->ਸ਼ੁਰੂ.ਖਾਨਾ  = col1;
    }

    // See if we changed at the end of the selection
    ਜੇ (row2 != cb->ਸਿਰਾ.lnum || col2 != (ਪੂਰਨ)cb->ਸਿਰਾ.ਖਾਨਾ)
    {
	clip_invert_area(cb, (ਪੂਰਨ)cb->ਸਿਰਾ.lnum, cb->ਸਿਰਾ.ਖਾਨਾ, row2, col2,
								 CLIP_TOGGLE);
	cb->ਸਿਰਾ.lnum = row2;
	cb->ਸਿਰਾ.ਖਾਨਾ  = col2;
    }
}

/*
 * Find the starting and ending positions of the word at the given row and
 * column.  Only white-separated words are recognized here.
 */
# define CHAR_CLASS(c)	(c <= ' ' ? ' ' : vim_iswordc(c))

    ਸਥਿਰ ਖਾਲੀ
clip_get_ਸ਼ਬਦ_boundaries(Clipboard_T *cb, ਪੂਰਨ ਪੰਕਤੀ, ਪੂਰਨ ਖਾਨਾ)
{
    ਪੂਰਨ		ਸ਼ੁਰੂ_class;
    ਪੂਰਨ		ਆਰਜ਼ੀ_ਖਾਨਾ;
    char_u	*p;
    ਪੂਰਨ		mboff;

    ਜੇ (ਪੰਕਤੀ >= screen_ਪੰਕਤੀਆਂ || ਖਾਨਾ >= screen_Columns || Screen_ਸਤਰਾਂ == ਨਿਰਾ)
	ਵਾਪਸ;

    p = Screen_ਸਤਰਾਂ + ਸਤਰ_ਵਿੱਥ[ਪੰਕਤੀ];
    // Correct for starting in the right half of a double-wide char
    ਜੇ (enc_dbcs != 0)
	ਖਾਨਾ -= dbcs_screen_ਸਿਰ_off(p, p + ਖਾਨਾ);
    ਨਹੀਂਤਾਂ ਜੇ (enc_utf8 && p[ਖਾਨਾ] == 0)
	--ਖਾਨਾ;
    ਸ਼ੁਰੂ_class = CHAR_CLASS(p[ਖਾਨਾ]);

    ਆਰਜ਼ੀ_ਖਾਨਾ = ਖਾਨਾ;
    ਲਈ ( ; ਆਰਜ਼ੀ_ਖਾਨਾ > 0; ਆਰਜ਼ੀ_ਖਾਨਾ--)
	ਜੇ (enc_dbcs != 0
		   && (mboff = dbcs_screen_ਸਿਰ_off(p, p + ਆਰਜ਼ੀ_ਖਾਨਾ - 1)) > 0)
	    ਆਰਜ਼ੀ_ਖਾਨਾ -= mboff;
	ਨਹੀਂਤਾਂ ਜੇ (CHAR_CLASS(p[ਆਰਜ਼ੀ_ਖਾਨਾ - 1]) != ਸ਼ੁਰੂ_class
		&& !(enc_utf8 && p[ਆਰਜ਼ੀ_ਖਾਨਾ - 1] == 0))
	    ਰੋਕੋ;
    cb->ਸ਼ਬਦ_ਸ਼ੁਰੂ_ਖਾਨਾ = ਆਰਜ਼ੀ_ਖਾਨਾ;

    ਆਰਜ਼ੀ_ਖਾਨਾ = ਖਾਨਾ;
    ਲਈ ( ; ਆਰਜ਼ੀ_ਖਾਨਾ < screen_Columns; ਆਰਜ਼ੀ_ਖਾਨਾ++)
	ਜੇ (enc_dbcs != 0 && dbcs_ptr2cells(p + ਆਰਜ਼ੀ_ਖਾਨਾ) == 2)
	    ++ਆਰਜ਼ੀ_ਖਾਨਾ;
	ਨਹੀਂਤਾਂ ਜੇ (CHAR_CLASS(p[ਆਰਜ਼ੀ_ਖਾਨਾ]) != ਸ਼ੁਰੂ_class
		&& !(enc_utf8 && p[ਆਰਜ਼ੀ_ਖਾਨਾ] == 0))
	    ਰੋਕੋ;
    cb->ਸ਼ਬਦ_ਸਿਰਾ_ਖਾਨਾ = ਆਰਜ਼ੀ_ਖਾਨਾ;
}

/*
 * Find the column position for the last non-whitespace character on the given
 * line at or before start_col.
 */
    ਸਥਿਰ ਪੂਰਨ
clip_get_ਸਤਰ_ਸਿਰਾ(Clipboard_T *cbd ਅਣਵਰਤਿਆ, ਪੂਰਨ ਪੰਕਤੀ)
{
    ਪੂਰਨ	    i;

    ਜੇ (ਪੰਕਤੀ >= screen_ਪੰਕਤੀਆਂ || Screen_ਸਤਰਾਂ == ਨਿਰਾ)
	ਵਾਪਸ 0;
    ਲਈ (i =
# ifdef FEAT_PROP_POPUP
	    cbd->max_ਖਾਨਾ;
# ਨਹੀਂਤਾਂ
	    screen_Columns;
# endif
			    i > 0; i--)
	ਜੇ (Screen_ਸਤਰਾਂ[ਸਤਰ_ਵਿੱਥ[ਪੰਕਤੀ] + i - 1] != ' ')
	    ਰੋਕੋ;
    ਵਾਪਸ i;
}

/*
 * Start the selection
 */
    ਖਾਲੀ
clip_ਸ਼ੁਰੂ_selection(ਪੂਰਨ ਖਾਨਾ, ਪੂਰਨ ਪੰਕਤੀ, ਪੂਰਨ repeated_click)
{
    Clipboard_T	*cb = &clip_ਤਾਰਾ;
# ifdef FEAT_PROP_POPUP
    win_T	*wp;
    ਪੂਰਨ		ਪੰਕਤੀ_cp = ਪੰਕਤੀ;
    ਪੂਰਨ		ਖਾਨਾ_cp = ਖਾਨਾ;

    wp = mouse_ਲੱਭੋ_win(&ਪੰਕਤੀ_cp, &ਖਾਨਾ_cp, ਲੱਭੋ_POPUP);
    ਜੇ (wp != ਨਿਰਾ && WIN_IS_POPUP(wp)
				  && popup_is_ਵਿੱਚ_scrollbar(wp, ਪੰਕਤੀ_cp, ਖਾਨਾ_cp))
	// click or double click in scrollbar does not start a selection
	ਵਾਪਸ;
# endif

    ਜੇ (cb->state == SELECT_ਪੂਰਾ)
	clip_clear_selection(cb);

    ਪੰਕਤੀ = check_ਪੰਕਤੀ(ਪੰਕਤੀ);
    ਖਾਨਾ = check_ਖਾਨਾ(ਖਾਨਾ);
    ਖਾਨਾ = mb_fix_ਖਾਨਾ(ਖਾਨਾ, ਪੰਕਤੀ);

    cb->ਸ਼ੁਰੂ.lnum  = ਪੰਕਤੀ;
    cb->ਸ਼ੁਰੂ.ਖਾਨਾ   = ਖਾਨਾ;
    cb->ਸਿਰਾ	    = cb->ਸ਼ੁਰੂ;
    cb->origin_ਪੰਕਤੀ  = (short_u)cb->ਸ਼ੁਰੂ.lnum;
    cb->state	    = SELECT_ਵਿੱਚ_ਤਰੱਕੀ;
# ifdef FEAT_PROP_POPUP
    ਜੇ (wp != ਨਿਰਾ && WIN_IS_POPUP(wp))
    {
	// Click in a popup window restricts selection to that window,
	// excluding the border.
	cb->min_ਖਾਨਾ = wp->w_wincol + wp->w_popup_border[3];
	cb->max_ਖਾਨਾ = wp->w_wincol + popup_ਚੌੜਾਈ(wp)
				 - wp->w_popup_border[1] - wp->w_has_scrollbar;
	ਜੇ (cb->max_ਖਾਨਾ > screen_Columns)
	    cb->max_ਖਾਨਾ = screen_Columns;
	cb->min_ਪੰਕਤੀ = wp->w_winrow + wp->w_popup_border[0];
	cb->max_ਪੰਕਤੀ = wp->w_winrow + popup_ਉਚਾਈ(wp) - 1
						   - wp->w_popup_border[2];
    }
    ਨਹੀਂਤਾਂ
    {
	cb->min_ਖਾਨਾ = 0;
	cb->max_ਖਾਨਾ = screen_Columns;
	cb->min_ਪੰਕਤੀ = 0;
	cb->max_ਪੰਕਤੀ = screen_ਪੰਕਤੀਆਂ;
    }
# endif

    ਜੇ (repeated_click)
    {
	ਜੇ (++cb->ਬਹੁਲਕ > SELECT_ਬਹੁਲਕ_ਸਤਰ)
	    cb->ਬਹੁਲਕ = SELECT_ਬਹੁਲਕ_CHAR;
    }
    ਨਹੀਂਤਾਂ
	cb->ਬਹੁਲਕ = SELECT_ਬਹੁਲਕ_CHAR;

# ifdef FEAT_GUI
    // clear the cursor until the selection is made
    ਜੇ (gui.ਵਿੱਚ_use)
	gui_undraw_cursor();
# endif

    ਚੋਣ (cb->ਬਹੁਲਕ)
    {
	ਹਾਲਤ SELECT_ਬਹੁਲਕ_CHAR:
	    cb->origin_ਸ਼ੁਰੂ_ਖਾਨਾ = cb->ਸ਼ੁਰੂ.ਖਾਨਾ;
	    cb->ਸ਼ਬਦ_ਸਿਰਾ_ਖਾਨਾ = clip_get_ਸਤਰ_ਸਿਰਾ(cb, (ਪੂਰਨ)cb->ਸ਼ੁਰੂ.lnum);
	    ਰੋਕੋ;

	ਹਾਲਤ SELECT_ਬਹੁਲਕ_ਸ਼ਬਦ:
	    clip_get_ਸ਼ਬਦ_boundaries(cb, (ਪੂਰਨ)cb->ਸ਼ੁਰੂ.lnum, cb->ਸ਼ੁਰੂ.ਖਾਨਾ);
	    cb->origin_ਸ਼ੁਰੂ_ਖਾਨਾ = cb->ਸ਼ਬਦ_ਸ਼ੁਰੂ_ਖਾਨਾ;
	    cb->origin_ਸਿਰਾ_ਖਾਨਾ	 = cb->ਸ਼ਬਦ_ਸਿਰਾ_ਖਾਨਾ;

	    clip_invert_area(cb, (ਪੂਰਨ)cb->ਸ਼ੁਰੂ.lnum, cb->ਸ਼ਬਦ_ਸ਼ੁਰੂ_ਖਾਨਾ,
			    (ਪੂਰਨ)cb->ਸਿਰਾ.lnum, cb->ਸ਼ਬਦ_ਸਿਰਾ_ਖਾਨਾ, CLIP_ਸੈੱਟ);
	    cb->ਸ਼ੁਰੂ.ਖਾਨਾ = cb->ਸ਼ਬਦ_ਸ਼ੁਰੂ_ਖਾਨਾ;
	    cb->ਸਿਰਾ.ਖਾਨਾ   = cb->ਸ਼ਬਦ_ਸਿਰਾ_ਖਾਨਾ;
	    ਰੋਕੋ;

	ਹਾਲਤ SELECT_ਬਹੁਲਕ_ਸਤਰ:
	    clip_invert_area(cb, (ਪੂਰਨ)cb->ਸ਼ੁਰੂ.lnum, 0, (ਪੂਰਨ)cb->ਸ਼ੁਰੂ.lnum,
			    (ਪੂਰਨ)Columns, CLIP_ਸੈੱਟ);
	    cb->ਸ਼ੁਰੂ.ਖਾਨਾ = 0;
	    cb->ਸਿਰਾ.ਖਾਨਾ   = Columns;
	    ਰੋਕੋ;
    }

    cb->prev = cb->ਸ਼ੁਰੂ;

# ifdef DEBUG_SELECTION
    ਲਿਖੋ("Selection started at (%ld,%d)\n", cb->ਸ਼ੁਰੂ.lnum, cb->ਸ਼ੁਰੂ.ਖਾਨਾ);
# endif
}

/*
 * Continue processing the selection
 */
    ਖਾਲੀ
clip_process_selection(
    ਪੂਰਨ		button,
    ਪੂਰਨ		ਖਾਨਾ,
    ਪੂਰਨ		ਪੰਕਤੀ,
    int_u	repeated_click)
{
    Clipboard_T	*cb = &clip_ਤਾਰਾ;
    ਪੂਰਨ		diff;
    ਪੂਰਨ		slen = 1;	// cursor shape width

    ਜੇ (button == MOUSE_RELEASE)
    {
	ਜੇ (cb->state != SELECT_ਵਿੱਚ_ਤਰੱਕੀ)
	    ਵਾਪਸ;

	// Check to make sure we have something selected
	ਜੇ (cb->ਸ਼ੁਰੂ.lnum == cb->ਸਿਰਾ.lnum && cb->ਸ਼ੁਰੂ.ਖਾਨਾ == cb->ਸਿਰਾ.ਖਾਨਾ)
	{
# ifdef FEAT_GUI
	    ਜੇ (gui.ਵਿੱਚ_use)
		gui_update_cursor(FALSE, FALSE);
# endif
	    cb->state = SELECT_CLEARED;
	    ਵਾਪਸ;
	}

# ifdef DEBUG_SELECTION
	ਲਿਖੋ("Selection ended: (%ld,%d) to (%ld,%d)\n", cb->ਸ਼ੁਰੂ.lnum,
		cb->ਸ਼ੁਰੂ.ਖਾਨਾ, cb->ਸਿਰਾ.lnum, cb->ਸਿਰਾ.ਖਾਨਾ);
# endif
	ਜੇ (clip_isautosel_ਤਾਰਾ() || clip_isautosel_plus()
		|| (
# ifdef FEAT_GUI
		    gui.ਵਿੱਚ_use ? (vim_strchr(p_go, GO_ASELML) != ਨਿਰਾ) :
# endif
		    clip_autoselectml))
	    clip_copy_modeless_selection(FALSE);
# ifdef FEAT_GUI
	ਜੇ (gui.ਵਿੱਚ_use)
	    gui_update_cursor(FALSE, FALSE);
# endif

	cb->state = SELECT_ਪੂਰਾ;
	ਵਾਪਸ;
    }

    ਪੰਕਤੀ = check_ਪੰਕਤੀ(ਪੰਕਤੀ);
    ਖਾਨਾ = check_ਖਾਨਾ(ਖਾਨਾ);
    ਖਾਨਾ = mb_fix_ਖਾਨਾ(ਖਾਨਾ, ਪੰਕਤੀ);

    ਜੇ (ਖਾਨਾ == (ਪੂਰਨ)cb->prev.ਖਾਨਾ && ਪੰਕਤੀ == cb->prev.lnum && !repeated_click)
	ਵਾਪਸ;

    /*
     * When extending the selection with the right mouse button, swap the
     * start and end if the position is before half the selection
     */
    ਜੇ (cb->state == SELECT_ਪੂਰਾ && button == MOUSE_ਸੱਜਾ)
    {
	/*
	 * If the click is before the start, or the click is inside the
	 * selection and the start is the closest side, set the origin to the
	 * end of the selection.
	 */
	ਜੇ (clip_compare_pos(ਪੰਕਤੀ, ਖਾਨਾ, (ਪੂਰਨ)cb->ਸ਼ੁਰੂ.lnum, cb->ਸ਼ੁਰੂ.ਖਾਨਾ) < 0
		|| (clip_compare_pos(ਪੰਕਤੀ, ਖਾਨਾ,
					   (ਪੂਰਨ)cb->ਸਿਰਾ.lnum, cb->ਸਿਰਾ.ਖਾਨਾ) < 0
		    && (((cb->ਸ਼ੁਰੂ.lnum == cb->ਸਿਰਾ.lnum
			    && cb->ਸਿਰਾ.ਖਾਨਾ - ਖਾਨਾ > ਖਾਨਾ - cb->ਸ਼ੁਰੂ.ਖਾਨਾ))
			|| ((diff = (cb->ਸਿਰਾ.lnum - ਪੰਕਤੀ) -
						   (ਪੰਕਤੀ - cb->ਸ਼ੁਰੂ.lnum)) > 0
			    || (diff == 0 && ਖਾਨਾ < (ਪੂਰਨ)(cb->ਸ਼ੁਰੂ.ਖਾਨਾ +
							 cb->ਸਿਰਾ.ਖਾਨਾ) / 2)))))
	{
	    cb->origin_ਪੰਕਤੀ = (short_u)cb->ਸਿਰਾ.lnum;
	    cb->origin_ਸ਼ੁਰੂ_ਖਾਨਾ = cb->ਸਿਰਾ.ਖਾਨਾ - 1;
	    cb->origin_ਸਿਰਾ_ਖਾਨਾ = cb->ਸਿਰਾ.ਖਾਨਾ;
	}
	ਨਹੀਂਤਾਂ
	{
	    cb->origin_ਪੰਕਤੀ = (short_u)cb->ਸ਼ੁਰੂ.lnum;
	    cb->origin_ਸ਼ੁਰੂ_ਖਾਨਾ = cb->ਸ਼ੁਰੂ.ਖਾਨਾ;
	    cb->origin_ਸਿਰਾ_ਖਾਨਾ = cb->ਸ਼ੁਰੂ.ਖਾਨਾ;
	}
	ਜੇ (cb->ਬਹੁਲਕ == SELECT_ਬਹੁਲਕ_ਸ਼ਬਦ && !repeated_click)
	    cb->ਬਹੁਲਕ = SELECT_ਬਹੁਲਕ_CHAR;
    }

    // set state, for when using the right mouse button
    cb->state = SELECT_ਵਿੱਚ_ਤਰੱਕੀ;

# ifdef DEBUG_SELECTION
    ਲਿਖੋ("Selection extending to (%d,%d)\n", ਪੰਕਤੀ, ਖਾਨਾ);
# endif

    ਜੇ (repeated_click && ++cb->ਬਹੁਲਕ > SELECT_ਬਹੁਲਕ_ਸਤਰ)
	cb->ਬਹੁਲਕ = SELECT_ਬਹੁਲਕ_CHAR;

    ਚੋਣ (cb->ਬਹੁਲਕ)
    {
	ਹਾਲਤ SELECT_ਬਹੁਲਕ_CHAR:
	    // If we're on a different line, find where the line ends
	    ਜੇ (ਪੰਕਤੀ != cb->prev.lnum)
		cb->ਸ਼ਬਦ_ਸਿਰਾ_ਖਾਨਾ = clip_get_ਸਤਰ_ਸਿਰਾ(cb, ਪੰਕਤੀ);

	    // See if we are before or after the origin of the selection
	    ਜੇ (clip_compare_pos(ਪੰਕਤੀ, ਖਾਨਾ, cb->origin_ਪੰਕਤੀ,
						   cb->origin_ਸ਼ੁਰੂ_ਖਾਨਾ) >= 0)
	    {
		ਜੇ (ਖਾਨਾ >= (ਪੂਰਨ)cb->ਸ਼ਬਦ_ਸਿਰਾ_ਖਾਨਾ)
		    clip_update_modeless_selection(cb, cb->origin_ਪੰਕਤੀ,
			    cb->origin_ਸ਼ੁਰੂ_ਖਾਨਾ, ਪੰਕਤੀ, (ਪੂਰਨ)Columns);
		ਨਹੀਂਤਾਂ
		{
		    ਜੇ (has_mbyte && mb_lefthalve(ਪੰਕਤੀ, ਖਾਨਾ))
			slen = 2;
		    clip_update_modeless_selection(cb, cb->origin_ਪੰਕਤੀ,
			    cb->origin_ਸ਼ੁਰੂ_ਖਾਨਾ, ਪੰਕਤੀ, ਖਾਨਾ + slen);
		}
	    }
	    ਨਹੀਂਤਾਂ
	    {
		ਜੇ (has_mbyte
			&& mb_lefthalve(cb->origin_ਪੰਕਤੀ, cb->origin_ਸ਼ੁਰੂ_ਖਾਨਾ))
		    slen = 2;
		ਜੇ (ਖਾਨਾ >= (ਪੂਰਨ)cb->ਸ਼ਬਦ_ਸਿਰਾ_ਖਾਨਾ)
		    clip_update_modeless_selection(cb, ਪੰਕਤੀ, cb->ਸ਼ਬਦ_ਸਿਰਾ_ਖਾਨਾ,
			    cb->origin_ਪੰਕਤੀ, cb->origin_ਸ਼ੁਰੂ_ਖਾਨਾ + slen);
		ਨਹੀਂਤਾਂ
		    clip_update_modeless_selection(cb, ਪੰਕਤੀ, ਖਾਨਾ,
			    cb->origin_ਪੰਕਤੀ, cb->origin_ਸ਼ੁਰੂ_ਖਾਨਾ + slen);
	    }
	    ਰੋਕੋ;

	ਹਾਲਤ SELECT_ਬਹੁਲਕ_ਸ਼ਬਦ:
	    // If we are still within the same word, do nothing
	    ਜੇ (ਪੰਕਤੀ == cb->prev.lnum && ਖਾਨਾ >= (ਪੂਰਨ)cb->ਸ਼ਬਦ_ਸ਼ੁਰੂ_ਖਾਨਾ
		    && ਖਾਨਾ < (ਪੂਰਨ)cb->ਸ਼ਬਦ_ਸਿਰਾ_ਖਾਨਾ && !repeated_click)
		ਵਾਪਸ;

	    // Get new word boundaries
	    clip_get_ਸ਼ਬਦ_boundaries(cb, ਪੰਕਤੀ, ਖਾਨਾ);

	    // Handle being after the origin point of selection
	    ਜੇ (clip_compare_pos(ਪੰਕਤੀ, ਖਾਨਾ, cb->origin_ਪੰਕਤੀ,
		    cb->origin_ਸ਼ੁਰੂ_ਖਾਨਾ) >= 0)
		clip_update_modeless_selection(cb, cb->origin_ਪੰਕਤੀ,
			cb->origin_ਸ਼ੁਰੂ_ਖਾਨਾ, ਪੰਕਤੀ, cb->ਸ਼ਬਦ_ਸਿਰਾ_ਖਾਨਾ);
	    ਨਹੀਂਤਾਂ
		clip_update_modeless_selection(cb, ਪੰਕਤੀ, cb->ਸ਼ਬਦ_ਸ਼ੁਰੂ_ਖਾਨਾ,
			cb->origin_ਪੰਕਤੀ, cb->origin_ਸਿਰਾ_ਖਾਨਾ);
	    ਰੋਕੋ;

	ਹਾਲਤ SELECT_ਬਹੁਲਕ_ਸਤਰ:
	    ਜੇ (ਪੰਕਤੀ == cb->prev.lnum && !repeated_click)
		ਵਾਪਸ;

	    ਜੇ (clip_compare_pos(ਪੰਕਤੀ, ਖਾਨਾ, cb->origin_ਪੰਕਤੀ,
		    cb->origin_ਸ਼ੁਰੂ_ਖਾਨਾ) >= 0)
		clip_update_modeless_selection(cb, cb->origin_ਪੰਕਤੀ, 0, ਪੰਕਤੀ,
			(ਪੂਰਨ)Columns);
	    ਨਹੀਂਤਾਂ
		clip_update_modeless_selection(cb, ਪੰਕਤੀ, 0, cb->origin_ਪੰਕਤੀ,
			(ਪੂਰਨ)Columns);
	    ਰੋਕੋ;
    }

    cb->prev.lnum = ਪੰਕਤੀ;
    cb->prev.ਖਾਨਾ  = ਖਾਨਾ;

# ifdef DEBUG_SELECTION
	ਲਿਖੋ("Selection is: (%ld,%d) to (%ld,%d)\n", cb->ਸ਼ੁਰੂ.lnum,
		cb->ਸ਼ੁਰੂ.ਖਾਨਾ, cb->ਸਿਰਾ.lnum, cb->ਸਿਰਾ.ਖਾਨਾ);
# endif
}

# ਜੇ defined(FEAT_GUI)
/*
 * Redraw part of the selection if character at "row,col" is inside of it.
 * Only used for the GUI.
 */
    ਖਾਲੀ
clip_may_redraw_selection(ਪੂਰਨ ਪੰਕਤੀ, ਪੂਰਨ ਖਾਨਾ, ਪੂਰਨ len)
{
    ਪੂਰਨ		ਸ਼ੁਰੂ = ਖਾਨਾ;
    ਪੂਰਨ		ਸਿਰਾ = ਖਾਨਾ + len;

    ਜੇ (clip_ਤਾਰਾ.state != SELECT_CLEARED
	    && ਪੰਕਤੀ >= clip_ਤਾਰਾ.ਸ਼ੁਰੂ.lnum
	    && ਪੰਕਤੀ <= clip_ਤਾਰਾ.ਸਿਰਾ.lnum)
    {
	ਜੇ (ਪੰਕਤੀ == clip_ਤਾਰਾ.ਸ਼ੁਰੂ.lnum && ਸ਼ੁਰੂ < (ਪੂਰਨ)clip_ਤਾਰਾ.ਸ਼ੁਰੂ.ਖਾਨਾ)
	    ਸ਼ੁਰੂ = clip_ਤਾਰਾ.ਸ਼ੁਰੂ.ਖਾਨਾ;
	ਜੇ (ਪੰਕਤੀ == clip_ਤਾਰਾ.ਸਿਰਾ.lnum && ਸਿਰਾ > (ਪੂਰਨ)clip_ਤਾਰਾ.ਸਿਰਾ.ਖਾਨਾ)
	    ਸਿਰਾ = clip_ਤਾਰਾ.ਸਿਰਾ.ਖਾਨਾ;
	ਜੇ (ਸਿਰਾ > ਸ਼ੁਰੂ)
	    clip_invert_area(&clip_ਤਾਰਾ, ਪੰਕਤੀ, ਸ਼ੁਰੂ, ਪੰਕਤੀ, ਸਿਰਾ, 0);
    }
}
# endif

/*
 * Called from outside to clear selected region from the display
 */
    ਖਾਲੀ
clip_clear_selection(Clipboard_T *cbd)
{

    ਜੇ (cbd->state == SELECT_CLEARED)
	ਵਾਪਸ;

    clip_invert_area(cbd, (ਪੂਰਨ)cbd->ਸ਼ੁਰੂ.lnum, cbd->ਸ਼ੁਰੂ.ਖਾਨਾ,
				 (ਪੂਰਨ)cbd->ਸਿਰਾ.lnum, cbd->ਸਿਰਾ.ਖਾਨਾ, CLIP_CLEAR);
    cbd->state = SELECT_CLEARED;
}

/*
 * Clear the selection if any lines from "row1" to "row2" are inside of it.
 */
    ਖਾਲੀ
clip_may_clear_selection(ਪੂਰਨ row1, ਪੂਰਨ row2)
{
    ਜੇ (clip_ਤਾਰਾ.state == SELECT_ਪੂਰਾ
	    && row2 >= clip_ਤਾਰਾ.ਸ਼ੁਰੂ.lnum
	    && row1 <= clip_ਤਾਰਾ.ਸਿਰਾ.lnum)
	clip_clear_selection(&clip_ਤਾਰਾ);
}

/*
 * Called before the screen is scrolled up or down.  Adjusts the line numbers
 * of the selection.  Call with big number when clearing the screen.
 */
    ਖਾਲੀ
clip_scroll_selection(
    ਪੂਰਨ	    ਪੰਕਤੀਆਂ_2)		// negative for scroll down
{
    ਪੂਰਨ	    lnum;

    ਜੇ (clip_ਤਾਰਾ.state == SELECT_CLEARED)
	ਵਾਪਸ;

    lnum = clip_ਤਾਰਾ.ਸ਼ੁਰੂ.lnum - ਪੰਕਤੀਆਂ_2;
    ਜੇ (lnum <= 0)
	clip_ਤਾਰਾ.ਸ਼ੁਰੂ.lnum = 0;
    ਨਹੀਂਤਾਂ ਜੇ (lnum >= screen_ਪੰਕਤੀਆਂ)	// scrolled off of the screen
	clip_ਤਾਰਾ.state = SELECT_CLEARED;
    ਨਹੀਂਤਾਂ
	clip_ਤਾਰਾ.ਸ਼ੁਰੂ.lnum = lnum;

    lnum = clip_ਤਾਰਾ.ਸਿਰਾ.lnum - ਪੰਕਤੀਆਂ_2;
    ਜੇ (lnum < 0)			// scrolled off of the screen
	clip_ਤਾਰਾ.state = SELECT_CLEARED;
    ਨਹੀਂਤਾਂ ਜੇ (lnum >= screen_ਪੰਕਤੀਆਂ)
	clip_ਤਾਰਾ.ਸਿਰਾ.lnum = screen_ਪੰਕਤੀਆਂ - 1;
    ਨਹੀਂਤਾਂ
	clip_ਤਾਰਾ.ਸਿਰਾ.lnum = lnum;
}

/*
 * Copy the currently selected area into the '*' or '+' register so it will be
 * available for pasting.
 * When "both" is TRUE also copy to the other register.
 */
    ਖਾਲੀ
clip_copy_modeless_selection(ਪੂਰਨ both ਅਣਵਰਤਿਆ)
{
    // The info for the modeless selection is stored in '*' register, however if
    // we are using the '+' register for modeless autoselect, we copy to
    // clip_plus instead while using the info in clip_star.
    Clipboard_T *cbd = clip_isautosel_plus() ? &clip_plus : &clip_ਤਾਰਾ;
    char_u	*ਬਫਰ;
    char_u	*bufp;
    ਪੂਰਨ		ਪੰਕਤੀ;
    ਪੂਰਨ		ਸ਼ੁਰੂ_ਖਾਨਾ;
    ਪੂਰਨ		ਸਿਰਾ_ਖਾਨਾ;
    ਪੂਰਨ		ਸਤਰ_ਸਿਰਾ_ਖਾਨਾ;
    ਪੂਰਨ		ਜੋੜੋ_newline_flag = FALSE;
    ਪੂਰਨ		len;
    char_u	*p;
    ਪੂਰਨ		row1 = clip_ਤਾਰਾ.ਸ਼ੁਰੂ.lnum;
    ਪੂਰਨ		col1 = clip_ਤਾਰਾ.ਸ਼ੁਰੂ.ਖਾਨਾ;
    ਪੂਰਨ		row2 = clip_ਤਾਰਾ.ਸਿਰਾ.lnum;
    ਪੂਰਨ		col2 = clip_ਤਾਰਾ.ਸਿਰਾ.ਖਾਨਾ;

    // Can't use ScreenLines unless initialized
    ਜੇ (Screen_ਸਤਰਾਂ == ਨਿਰਾ)
	ਵਾਪਸ;

    /*
     * Make sure row1 <= row2, and if row1 == row2 that col1 <= col2.
     */
    ਜੇ (row1 > row2)
    {
	ਪੰਕਤੀ = row1; row1 = row2; row2 = ਪੰਕਤੀ;
	ਪੰਕਤੀ = col1; col1 = col2; col2 = ਪੰਕਤੀ;
    }
    ਨਹੀਂਤਾਂ ਜੇ (row1 == row2 && col1 > col2)
    {
	ਪੰਕਤੀ = col1; col1 = col2; col2 = ਪੰਕਤੀ;
    }
# ifdef FEAT_PROP_POPUP
    ਜੇ (col1 < clip_ਤਾਰਾ.min_ਖਾਨਾ)
	col1 = clip_ਤਾਰਾ.min_ਖਾਨਾ;
    ਜੇ (col2 > clip_ਤਾਰਾ.max_ਖਾਨਾ)
	col2 = clip_ਤਾਰਾ.max_ਖਾਨਾ;
    ਜੇ (row1 > clip_ਤਾਰਾ.max_ਪੰਕਤੀ || row2 < clip_ਤਾਰਾ.min_ਪੰਕਤੀ)
	ਵਾਪਸ;
    ਜੇ (row1 < clip_ਤਾਰਾ.min_ਪੰਕਤੀ)
	row1 = clip_ਤਾਰਾ.min_ਪੰਕਤੀ;
    ਜੇ (row2 > clip_ਤਾਰਾ.max_ਪੰਕਤੀ)
	row2 = clip_ਤਾਰਾ.max_ਪੰਕਤੀ;
# endif
    // correct starting point for being on right half of double-wide char
    p = Screen_ਸਤਰਾਂ + ਸਤਰ_ਵਿੱਥ[row1];
    ਜੇ (enc_dbcs != 0)
	col1 -= (*mb_ਸਿਰ_off)(p, p + col1);
    ਨਹੀਂਤਾਂ ਜੇ (enc_utf8 && p[col1] == 0)
	--col1;

    // Create a temporary buffer for storing the text
    len = (row2 - row1 + 1) * Columns + 1;
    ਜੇ (enc_dbcs != 0)
	len *= 2;	// max. 2 bytes per display cell
    ਨਹੀਂਤਾਂ ਜੇ (enc_utf8)
	len *= MB_MAXBYTES;
    // A newline is added after each row
    len += row2 - row1 + 2;
    ਬਫਰ = alloc(len);
    ਜੇ (ਬਫਰ == ਨਿਰਾ)	    // out of memory
	ਵਾਪਸ;

    // Process each row in the selection
    ਲਈ (bufp = ਬਫਰ, ਪੰਕਤੀ = row1; ਪੰਕਤੀ <= row2; ਪੰਕਤੀ++)
    {
	ਜੇ (ਪੰਕਤੀ == row1)
	    ਸ਼ੁਰੂ_ਖਾਨਾ = col1;
	ਨਹੀਂਤਾਂ
# ifdef FEAT_PROP_POPUP
	    ਸ਼ੁਰੂ_ਖਾਨਾ = clip_ਤਾਰਾ.min_ਖਾਨਾ;
# ਨਹੀਂਤਾਂ
	    ਸ਼ੁਰੂ_ਖਾਨਾ = 0;
# endif

	ਜੇ (ਪੰਕਤੀ == row2)
	    ਸਿਰਾ_ਖਾਨਾ = col2;
	ਨਹੀਂਤਾਂ
# ifdef FEAT_PROP_POPUP
	    ਸਿਰਾ_ਖਾਨਾ = clip_ਤਾਰਾ.max_ਖਾਨਾ;
# ਨਹੀਂਤਾਂ
	    ਸਿਰਾ_ਖਾਨਾ = Columns;
# endif

	ਸਤਰ_ਸਿਰਾ_ਖਾਨਾ = clip_get_ਸਤਰ_ਸਿਰਾ(&clip_ਤਾਰਾ, ਪੰਕਤੀ);

	// See if we need to nuke some trailing whitespace
	ਜੇ (ਸਿਰਾ_ਖਾਨਾ >=
# ifdef FEAT_PROP_POPUP
		clip_ਤਾਰਾ.max_ਖਾਨਾ
# ਨਹੀਂਤਾਂ
		Columns
# endif
		    && (ਪੰਕਤੀ < row2 || ਸਿਰਾ_ਖਾਨਾ > ਸਤਰ_ਸਿਰਾ_ਖਾਨਾ))
	{
	    // Get rid of trailing whitespace
	    ਸਿਰਾ_ਖਾਨਾ = ਸਤਰ_ਸਿਰਾ_ਖਾਨਾ;
	    ਜੇ (ਸਿਰਾ_ਖਾਨਾ < ਸ਼ੁਰੂ_ਖਾਨਾ)
		ਸਿਰਾ_ਖਾਨਾ = ਸ਼ੁਰੂ_ਖਾਨਾ;

	    // If the last line extended to the end, add an extra newline
	    ਜੇ (ਪੰਕਤੀ == row2)
		ਜੋੜੋ_newline_flag = TRUE;
	}

	// If after the first row, we need to always add a newline
	ਜੇ (ਪੰਕਤੀ > row1 && !ਸਤਰ_Wraps[ਪੰਕਤੀ - 1])
	    *bufp++ = NL;

	// Safetey check for in case resizing went wrong
	ਜੇ (ਪੰਕਤੀ < screen_ਪੰਕਤੀਆਂ && ਸਿਰਾ_ਖਾਨਾ <= screen_Columns)
	{
	    ਜੇ (enc_dbcs != 0)
	    {
		ਪੂਰਨ	i;

		p = Screen_ਸਤਰਾਂ + ਸਤਰ_ਵਿੱਥ[ਪੰਕਤੀ];
		ਲਈ (i = ਸ਼ੁਰੂ_ਖਾਨਾ; i < ਸਿਰਾ_ਖਾਨਾ; ++i)
		    ਜੇ (enc_dbcs == DBCS_JPNU && p[i] == 0x8e)
		    {
			// single-width double-byte char
			*bufp++ = 0x8e;
			*bufp++ = ScreenLines2[ਸਤਰ_ਵਿੱਥ[ਪੰਕਤੀ] + i];
		    }
		    ਨਹੀਂਤਾਂ
		    {
			*bufp++ = p[i];
			ਜੇ (MB_BYTE2LEN(p[i]) == 2)
			    *bufp++ = p[++i];
		    }
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (enc_utf8)
	    {
		ਪੂਰਨ	off;
		ਪੂਰਨ	i;

		off = ਸਤਰ_ਵਿੱਥ[ਪੰਕਤੀ];
		ਲਈ (i = ਸ਼ੁਰੂ_ਖਾਨਾ; i < ਸਿਰਾ_ਖਾਨਾ; ++i)
		{
		    // The base character is either in ScreenLinesUC[] or
		    // ScreenLines[].
		    ਜੇ (Screen_ਸਤਰਾਂ_UC[off + i] == 0)
			*bufp++ = Screen_ਸਤਰਾਂ[off + i];
		    ਨਹੀਂਤਾਂ
			// Includes the composing characters.
			bufp += utfc_char2bytes(off + i, bufp);
		    // Skip right half of double-wide character.
		    ਜੇ (Screen_ਸਤਰਾਂ[off + i + 1] == 0)
			++i;
		}
	    }
	    ਨਹੀਂਤਾਂ
	    {
		STRNCPY(bufp, Screen_ਸਤਰਾਂ + ਸਤਰ_ਵਿੱਥ[ਪੰਕਤੀ] + ਸ਼ੁਰੂ_ਖਾਨਾ,
							 ਸਿਰਾ_ਖਾਨਾ - ਸ਼ੁਰੂ_ਖਾਨਾ);
		bufp += ਸਿਰਾ_ਖਾਨਾ - ਸ਼ੁਰੂ_ਖਾਨਾ;
	    }
	}
    }

    // Add a newline at the end if the selection ended there
    ਜੇ (ਜੋੜੋ_newline_flag)
	*bufp++ = NL;

    // First cleanup any old selection and become the owner.
    clip_free_selection(cbd);
    clip_own_selection(cbd);

    // Yank the text into the '*' register.
    clip_yank_selection(MCHAR, ਬਫਰ, (ਲੰਮਾ)(bufp - ਬਫਰ), cbd);

    // Make the register contents available to the outside world.
    clip_gen_ਸੈੱਟ_selection(cbd);

# ifdef FEAT_X11
    ਜੇ (both)
    {
	Clipboard_T *other = cbd == &clip_ਤਾਰਾ ? &clip_plus : &clip_ਤਾਰਾ;
	// Do the same for the '+' register.
	clip_free_selection(other);
	clip_own_selection(other);
	clip_yank_selection(MCHAR, ਬਫਰ, (ਲੰਮਾ)(bufp - ਬਫਰ), other);
	clip_gen_ਸੈੱਟ_selection(other);
    }
# endif
    vim_free(ਬਫਰ);
}

    ਖਾਲੀ
clip_gen_ਸੈੱਟ_selection(Clipboard_T *cbd)
{
    ਜੇ (!clip_did_ਸੈੱਟ_selection)
    {
	// Updating postponed, so that accessing the system clipboard won't
	// hang Vim when accessing it many times (e.g. on a :g command).
	ਜੇ ((cbd == &clip_plus && (clip_unnamed_saved & CLIP_UNNAMED_PLUS))
		|| (cbd == &clip_ਤਾਰਾ && (clip_unnamed_saved & CLIP_UNNAMED)))
	{
	    clipboard_needs_update = TRUE;
	    ਵਾਪਸ;
	}
    }
# ਜੇ defined(FEAT_XCLIPBOARD) || defined(FEAT_WAYLAND_CLIPBOARD)
#  ifdef FEAT_GUI
    ਜੇ (gui.ਵਿੱਚ_use)
	clip_mch_ਸੈੱਟ_selection(cbd);
    ਨਹੀਂਤਾਂ
#  endif
    {
	ਜੇ (clipmethod == CLIPMETHOD_WAYLAND)
	{
#  ifdef FEAT_WAYLAND_CLIPBOARD
	    clip_wl_ਸੈੱਟ_selection(cbd);
#  endif
	}
	ਨਹੀਂਤਾਂ ਜੇ (clipmethod == CLIPMETHOD_X11)
	{
#  ifdef FEAT_XCLIPBOARD
	    clip_xterm_ਸੈੱਟ_selection(cbd);
#  endif
	}
    }
# ਨਹੀਂਤਾਂ
    clip_mch_ਸੈੱਟ_selection(cbd);
# endif
}

    ਸਥਿਰ ਖਾਲੀ
clip_gen_request_selection(Clipboard_T *cbd)
{
# ਜੇ defined(FEAT_XCLIPBOARD) || defined(FEAT_WAYLAND_CLIPBOARD)
#  ifdef FEAT_GUI
    ਜੇ (gui.ਵਿੱਚ_use)
	clip_mch_request_selection(cbd);
    ਨਹੀਂਤਾਂ
#  endif
    {
	ਜੇ (clipmethod == CLIPMETHOD_WAYLAND)
	{
#  ifdef FEAT_WAYLAND_CLIPBOARD
	    clip_wl_request_selection(cbd);
#  endif
	}
	ਨਹੀਂਤਾਂ ਜੇ (clipmethod == CLIPMETHOD_X11)
	{
#  ifdef FEAT_XCLIPBOARD
	    clip_xterm_request_selection(cbd);
#  endif
	}
    }
# ਨਹੀਂਤਾਂ
    clip_mch_request_selection(cbd);
# endif
}

# ਜੇ (defined(FEAT_X11) && defined(FEAT_XCLIPBOARD) && defined(USE_SYSTEM))
    ਸਥਿਰ ਪੂਰਨ
clip_x11_owner_exists(Clipboard_T *cbd)
{
    ਵਾਪਸ XGetSelectionOwner(X_DISPLAY, cbd->sel_atom) != None;
}
# endif

# ਜੇ (defined(FEAT_X11) || defined(FEAT_WAYLAND_CLIPBOARD)) \
	&& defined(USE_SYSTEM)
    ਪੂਰਨ
clip_gen_owner_exists(Clipboard_T *cbd ਅਣਵਰਤਿਆ)
{
#  ਜੇ defined(FEAT_XCLIPBOARD) || defined(FEAT_WAYLAND_CLIPBOARD)
#   ifdef FEAT_GUI_GTK
    ਜੇ (gui.ਵਿੱਚ_use)
	ਵਾਪਸ clip_gtk_owner_exists(cbd);
    ਨਹੀਂਤਾਂ
#   endif
    {
	ਜੇ (clipmethod == CLIPMETHOD_WAYLAND)
	{
#   ifdef FEAT_WAYLAND_CLIPBOARD
	    ਵਾਪਸ clip_wl_owner_exists(cbd);
#   endif
	}
	ਨਹੀਂਤਾਂ ਜੇ (clipmethod == CLIPMETHOD_X11)
	{
#   ifdef FEAT_XCLIPBOARD
	    ਵਾਪਸ clip_x11_owner_exists(cbd);
#   endif
	}
	ਨਹੀਂਤਾਂ
	    ਵਾਪਸ FALSE;
    }
#  ਨਹੀਂਤਾਂ
    ਵਾਪਸ TRUE;
#  endif
}
# endif

/*
 * Stuff for the X clipboard.  Shared between VMS and Unix.
 */

# ਜੇ defined(FEAT_XCLIPBOARD) || defined(FEAT_GUI_X11)
#  include <X11/Xatom.h>
#  include <X11/Intrinsic.h>

/*
 * Open the application context (if it hasn't been opened yet).
 * Used for Motif GUI and the xterm clipboard.
 */
    ਖਾਲੀ
open_app_context(ਖਾਲੀ)
{
    ਜੇ (app_context == ਨਿਰਾ)
    {
	XtToolkitInitialize();
	app_context = XtCreateApplicationContext();
    }
}

ਸਥਿਰ Atom	vim_atom;	// Vim's own special selection format
ਸਥਿਰ Atom	vimenc_atom;	// Vim's extended selection format
ਸਥਿਰ Atom	vim_mt_atom;	// Vim's own special selection format (in mime
				// type format)
ਸਥਿਰ Atom	vimenc_mt_atom;	// Vim's extended selection format (in mime type
				// format)
ਸਥਿਰ Atom	utf8_atom;
ਸਥਿਰ Atom	compound_ਪਾਠ_atom;
ਸਥਿਰ Atom	ਪਾਠ_atom;
ਸਥਿਰ Atom	targets_atom;
ਸਥਿਰ Atom	timestamp_atom;	// Used to get a timestamp

    ਖਾਲੀ
x11_ਤਿਆਰੀ_atoms(Display *dpy)
{
    vim_atom	       = XInternAtom(dpy, VIM_ATOM_NAME,   False);
    vimenc_atom	       = XInternAtom(dpy, VIMENC_ATOM_NAME,False);
    vim_mt_atom	       = XInternAtom(dpy, VIM_MIMETYPE_NAME,   False);
    vimenc_mt_atom     = XInternAtom(dpy, VIMENC_MIMETYPE_NAME,False);
    utf8_atom	       = XInternAtom(dpy, "UTF8_STRING",   False);
    compound_ਪਾਠ_atom = XInternAtom(dpy, "COMPOUND_TEXT", False);
    ਪਾਠ_atom	       = XInternAtom(dpy, "TEXT",	   False);
    targets_atom       = XInternAtom(dpy, "TARGETS",	   False);
    clip_ਤਾਰਾ.sel_atom = XA_PRIMARY;
    clip_plus.sel_atom = XInternAtom(dpy, "CLIPBOARD",	   False);
    timestamp_atom     = XInternAtom(dpy, "TIMESTAMP",	   False);
}

/*
 * X Selection stuff, for cutting and pasting text to other windows.
 */

    ਸਥਿਰ Boolean
clip_x11_convert_selection_cb(
    Widget	w ਅਣਵਰਤਿਆ,
    Atom	*sel_atom,
    Atom	*ਨਿਸ਼ਾਨਾ,
    Atom	*type,
    Xt_ਸੂਚਕ	*ਮੁੱਲ,
    long_u	*ਲੰਮਾਈ,
    ਪੂਰਨ		*format)
{
    ਸਥਿਰ char_u   *save_ਨਤੀਜਾ = ਨਿਰਾ;
    ਸਥਿਰ long_u   save_ਲੰਮਾਈ = 0;
    char_u	    *string;
    ਪੂਰਨ		    motion_type;
    Clipboard_T    *cbd;
    ਪੂਰਨ		    i;

    ਜੇ (*sel_atom == clip_plus.sel_atom)
	cbd = &clip_plus;
    ਨਹੀਂਤਾਂ
	cbd = &clip_ਤਾਰਾ;

    ਜੇ (!cbd->owned)
	ਵਾਪਸ False;	    // Shouldn't ever happen

    // requestor wants to know what target types we support
    ਜੇ (*ਨਿਸ਼ਾਨਾ == targets_atom)
    {
	ਸਥਿਰ Atom array[9];

	*ਮੁੱਲ = (Xt_ਸੂਚਕ)array;
	i = 0;
	array[i++] = targets_atom;
	array[i++] = vimenc_atom;
	array[i++] = vim_atom;
	array[i++] = vimenc_mt_atom;
	array[i++] = vim_mt_atom;
	ਜੇ (enc_utf8)
	    array[i++] = utf8_atom;
	array[i++] = XA_STRING;
	array[i++] = ਪਾਠ_atom;
	array[i++] = compound_ਪਾਠ_atom;

	*type = XA_ATOM;
	// This used to be: *format = sizeof(Atom) * 8; but that caused
	// crashes on 64 bit machines. (Peter Derr)
	*format = 32;
	*ਲੰਮਾਈ = i;
	ਵਾਪਸ True;
    }

    ਜੇ (       *ਨਿਸ਼ਾਨਾ != XA_STRING
	    && *ਨਿਸ਼ਾਨਾ != vimenc_atom
	    && *ਨਿਸ਼ਾਨਾ != vimenc_mt_atom
	    && (*ਨਿਸ਼ਾਨਾ != utf8_atom || !enc_utf8)
	    && *ਨਿਸ਼ਾਨਾ != vim_atom
	    && *ਨਿਸ਼ਾਨਾ != vim_mt_atom
	    && *ਨਿਸ਼ਾਨਾ != ਪਾਠ_atom
	    && *ਨਿਸ਼ਾਨਾ != compound_ਪਾਠ_atom)
	ਵਾਪਸ False;

    clip_get_selection(cbd);
    motion_type = clip_convert_selection(&string, ਲੰਮਾਈ, cbd);
    ਜੇ (motion_type < 0)
	ਵਾਪਸ False;

    // For our own format, the first byte contains the motion type
    ਜੇ (*ਨਿਸ਼ਾਨਾ == vim_atom || *ਨਿਸ਼ਾਨਾ == vim_mt_atom)
	(*ਲੰਮਾਈ)++;

    // Our own format with encoding: motion 'encoding' NUL text
    ਜੇ (*ਨਿਸ਼ਾਨਾ == vimenc_atom || *ਨਿਸ਼ਾਨਾ == vimenc_mt_atom)
	*ਲੰਮਾਈ += STRLEN(p_enc) + 2;

    ਜੇ (save_ਲੰਮਾਈ < *ਲੰਮਾਈ || save_ਲੰਮਾਈ / 2 >= *ਲੰਮਾਈ)
	*ਮੁੱਲ = XtRealloc((ਅੱਖਰ *)save_ਨਤੀਜਾ, (Cardinal)*ਲੰਮਾਈ + 1);
    ਨਹੀਂਤਾਂ
	*ਮੁੱਲ = save_ਨਤੀਜਾ;
    ਜੇ (*ਮੁੱਲ == ਨਿਰਾ)
    {
	vim_free(string);
	ਵਾਪਸ False;
    }
    save_ਨਤੀਜਾ = (char_u *)*ਮੁੱਲ;
    save_ਲੰਮਾਈ = *ਲੰਮਾਈ;

    ਜੇ (*ਨਿਸ਼ਾਨਾ == XA_STRING || (*ਨਿਸ਼ਾਨਾ == utf8_atom && enc_utf8))
    {
	mch_memmove(save_ਨਤੀਜਾ, string, (size_t)(*ਲੰਮਾਈ));
	*type = *ਨਿਸ਼ਾਨਾ;
    }
    ਨਹੀਂਤਾਂ ਜੇ (*ਨਿਸ਼ਾਨਾ == compound_ਪਾਠ_atom || *ਨਿਸ਼ਾਨਾ == ਪਾਠ_atom)
    {
	XTextProperty	ਪਾਠ_prop;
	ਅੱਖਰ		*string_nt = (ਅੱਖਰ *)save_ਨਤੀਜਾ;
	ਪੂਰਨ		conv_ਨਤੀਜਾ;

	// create NUL terminated string which XmbTextListToTextProperty wants
	mch_memmove(string_nt, string, (size_t)*ਲੰਮਾਈ);
	string_nt[*ਲੰਮਾਈ] = NUL;
	conv_ਨਤੀਜਾ = Xmb_ਪਾਠ_List_ਮੰਜ਼ਿਲ_ਪਾਠ_Property(X_DISPLAY, &string_nt,
					   1, XCompound_ਪਾਠ_Style, &ਪਾਠ_prop);
	ਜੇ (conv_ਨਤੀਜਾ != Success)
	{
	    vim_free(string);
	    ਵਾਪਸ False;
	}
	*ਮੁੱਲ = (Xt_ਸੂਚਕ)(ਪਾਠ_prop.ਮੁੱਲ);	//    from plain text
	*ਲੰਮਾਈ = ਪਾਠ_prop.nitems;
	*type = compound_ਪਾਠ_atom;
	XtFree((ਅੱਖਰ *)save_ਨਤੀਜਾ);
	save_ਨਤੀਜਾ = (char_u *)*ਮੁੱਲ;
	save_ਲੰਮਾਈ = *ਲੰਮਾਈ;
    }
    ਨਹੀਂਤਾਂ ਜੇ (*ਨਿਸ਼ਾਨਾ == vimenc_atom || *ਨਿਸ਼ਾਨਾ == vimenc_mt_atom)
    {
	ਪੂਰਨ l = STRLEN(p_enc);

	save_ਨਤੀਜਾ[0] = motion_type;
	STRCPY(save_ਨਤੀਜਾ + 1, p_enc);
	mch_memmove(save_ਨਤੀਜਾ + l + 2, string, (size_t)(*ਲੰਮਾਈ - l - 2));
	ਜੇ (*ਨਿਸ਼ਾਨਾ == vimenc_atom)
	    *type = vimenc_atom;
	ਨਹੀਂਤਾਂ
	    *type = vimenc_mt_atom;
    }
    ਨਹੀਂਤਾਂ
    {
	save_ਨਤੀਜਾ[0] = motion_type;
	mch_memmove(save_ਨਤੀਜਾ + 1, string, (size_t)(*ਲੰਮਾਈ - 1));
	ਜੇ (*ਨਿਸ਼ਾਨਾ == vim_atom)
	    *type = vim_atom;
	ਨਹੀਂਤਾਂ
	    *type = vim_mt_atom;
    }
    *format = 8;	    // 8 bits per char
    vim_free(string);
    ਵਾਪਸ True;
}

    ਸਥਿਰ ਖਾਲੀ
clip_x11_lose_ownership_cb(Widget w ਅਣਵਰਤਿਆ, Atom *sel_atom)
{
    ਜੇ (*sel_atom == clip_plus.sel_atom)
	clip_lose_selection(&clip_plus);
    ਨਹੀਂਤਾਂ
	clip_lose_selection(&clip_ਤਾਰਾ);
}

    ਸਥਿਰ ਖਾਲੀ
clip_x11_notify_cb(Widget w ਅਣਵਰਤਿਆ, Atom *sel_atom ਅਣਵਰਤਿਆ, Atom *ਨਿਸ਼ਾਨਾ ਅਣਵਰਤਿਆ)
{
    // To prevent automatically freeing the selection value.
}

/*
 * Property callback to get a timestamp for XtOwnSelection.
 */
#  ਜੇ defined(FEAT_X11) && defined(FEAT_XCLIPBOARD)
    ਸਥਿਰ ਖਾਲੀ
clip_x11_timestamp_cb(
    Widget	w,
    Xt_ਸੂਚਕ	n ਅਣਵਰਤਿਆ,
    XEvent	*event,
    Boolean	*cont ਅਣਵਰਤਿਆ)
{
    Atom	    actual_type;
    ਪੂਰਨ		    format;
    ਬਿਨਾਨਿਸ਼ਾਨੀ  ਲੰਮਾ  nitems, ਬਾਈਟਾਂ_after;
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ   *prop=ਨਿਰਾ;
    XPropertyEvent  *xproperty=&event->xproperty;

    // Must be a property notify, state can't be Delete (True), has to be
    // one of the supported selection types.
    ਜੇ (event->type != PropertyNotify || xproperty->state
	    || (xproperty->atom != clip_ਤਾਰਾ.sel_atom
				    && xproperty->atom != clip_plus.sel_atom))
	ਵਾਪਸ;

    ਜੇ (XGet_ਖਿੜਕੀ_Property(xproperty->display, xproperty->ਖਿੜਕੀ_2,
	  xproperty->atom, 0, 0, False, timestamp_atom, &actual_type, &format,
						&nitems, &ਬਾਈਟਾਂ_after, &prop))
	ਵਾਪਸ;

    ਜੇ (prop)
	XFree(prop);

    // Make sure the property type is "TIMESTAMP" and it's 32 bits.
    ਜੇ (actual_type != timestamp_atom || format != 32)
	ਵਾਪਸ;

    // Get the selection, using the event timestamp.
    ਜੇ (XtOwnSelection(w, xproperty->atom, xproperty->ਸਮਾਂ,
	    clip_x11_convert_selection_cb, clip_x11_lose_ownership_cb,
	    clip_x11_notify_cb) == OK)
    {
	// Set the "owned" flag now, there may have been a call to
	// lose_ownership_cb in between.
	ਜੇ (xproperty->atom == clip_plus.sel_atom)
	    clip_plus.owned = TRUE;
	ਨਹੀਂਤਾਂ
	    clip_ਤਾਰਾ.owned = TRUE;
    }
}

    ਖਾਲੀ
x11_ਤਿਆਰੀ_selection(Widget w)
{
    Xt_ਜੋੜੋ_Event_Handler(w, Property_Change_ਨਕਾਬ, False,
	    /*(XtEventHandler)*/clip_x11_timestamp_cb, (Xt_ਸੂਚਕ)ਨਿਰਾ);
}
#  endif

    ਸਥਿਰ ਖਾਲੀ
clip_x11_request_selection_cb(
    Widget	w ਅਣਵਰਤਿਆ,
    Xt_ਸੂਚਕ	success,
    Atom	*sel_atom,
    Atom	*type,
    Xt_ਸੂਚਕ	ਮੁੱਲ,
    long_u	*ਲੰਮਾਈ,
    ਪੂਰਨ		*format)
{
    ਪੂਰਨ		motion_type = MAUTO;
    long_u	len;
    char_u	*p;
    ਅੱਖਰ	**ਪਾਠ_list = ਨਿਰਾ;
    Clipboard_T	*cbd;
    char_u	*tmpbuf = ਨਿਰਾ;

    ਜੇ (*sel_atom == clip_plus.sel_atom)
	cbd = &clip_plus;
    ਨਹੀਂਤਾਂ
	cbd = &clip_ਤਾਰਾ;

    ਜੇ (ਮੁੱਲ == ਨਿਰਾ || *ਲੰਮਾਈ == 0)
    {
	clip_free_selection(cbd);	// nothing received, clear register
	*(ਪੂਰਨ *)success = FALSE;
	ਵਾਪਸ;
    }
    p = (char_u *)ਮੁੱਲ;
    len = *ਲੰਮਾਈ;
    ਜੇ (*type == vim_atom || *type == vim_mt_atom)
    {
	motion_type = *p++;
	len--;
    }

    ਨਹੀਂਤਾਂ ਜੇ (*type == vimenc_atom || *type == vimenc_mt_atom)
    {
	char_u		*enc;
	vimconv_T	conv;
	ਪੂਰਨ		convlen;

	motion_type = *p++;
	--len;

	enc = p;
	p += STRLEN(p) + 1;
	len -= p - enc;

	// If the encoding of the text is different from 'encoding', attempt
	// converting it.
	conv.vc_type = CONV_NONE;
	convert_ਤਿਆਰੀ(&conv, enc, p_enc);
	ਜੇ (conv.vc_type != CONV_NONE)
	{
	    convlen = len;	// Need to use an int here.
	    tmpbuf = string_convert(&conv, p, &convlen);
	    len = convlen;
	    ਜੇ (tmpbuf != ਨਿਰਾ)
		p = tmpbuf;
	    convert_ਤਿਆਰੀ(&conv, ਨਿਰਾ, ਨਿਰਾ);
	}
    }

    ਨਹੀਂਤਾਂ ਜੇ (*type == compound_ਪਾਠ_atom
	    || *type == utf8_atom
	    || (enc_dbcs != 0 && *type == ਪਾਠ_atom))
    {
	XTextProperty	ਪਾਠ_prop;
	ਪੂਰਨ		n_ਪਾਠ = 0;
	ਪੂਰਨ		ਸਥਿਤੀ;

	ਪਾਠ_prop.ਮੁੱਲ = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *)ਮੁੱਲ;
	ਪਾਠ_prop.encoding = *type;
	ਪਾਠ_prop.format = *format;
	ਪਾਠ_prop.nitems = len;
#  ਜੇ defined(X_HAVE_UTF8_STRING)
	ਜੇ (*type == utf8_atom)
	    ਸਥਿਤੀ = Xutf8_ਪਾਠ_Property_ਮੰਜ਼ਿਲ_ਪਾਠ_List(X_DISPLAY, &ਪਾਠ_prop,
							 &ਪਾਠ_list, &n_ਪਾਠ);
	ਨਹੀਂਤਾਂ
#  endif
	    ਸਥਿਤੀ = Xmb_ਪਾਠ_Property_ਮੰਜ਼ਿਲ_ਪਾਠ_List(X_DISPLAY, &ਪਾਠ_prop,
							 &ਪਾਠ_list, &n_ਪਾਠ);
	ਜੇ (ਸਥਿਤੀ != Success || n_ਪਾਠ < 1)
	{
	    *(ਪੂਰਨ *)success = FALSE;
	    ਵਾਪਸ;
	}
	p = (char_u *)ਪਾਠ_list[0];
	len = STRLEN(p);
    }
    clip_yank_selection(motion_type, p, (ਲੰਮਾ)len, cbd);

    ਜੇ (ਪਾਠ_list != ਨਿਰਾ)
	XFreeStringList(ਪਾਠ_list);
    vim_free(tmpbuf);
    XtFree((ਅੱਖਰ *)ਮੁੱਲ);
    *(ਪੂਰਨ *)success = TRUE;
}

    ਖਾਲੀ
clip_x11_request_selection(
    Widget	myShell,
    Display	*dpy,
    Clipboard_T	*cbd)
{
    XEvent	event;
    Atom	type;
    ਸਥਿਰ ਪੂਰਨ	success;
    ਪੂਰਨ		i;
    time_t	ਸ਼ੁਰੂ_ਸਮਾਂ;
    ਪੂਰਨ		timed_ਬਾਹਰਲਾ = FALSE;

    ਲਈ (i = 0; i < 8; i++)
    {
	ਚੋਣ (i)
	{
	    ਹਾਲਤ 0:  type = vimenc_mt_atom;	ਰੋਕੋ;
	    ਹਾਲਤ 1:  type = vimenc_atom;	ਰੋਕੋ;
	    ਹਾਲਤ 2:  type = vim_mt_atom;	ਰੋਕੋ;
	    ਹਾਲਤ 3:  type = vim_atom;		ਰੋਕੋ;
	    ਹਾਲਤ 4:  type = utf8_atom;		ਰੋਕੋ;
	    ਹਾਲਤ 5:  type = compound_ਪਾਠ_atom; ਰੋਕੋ;
	    ਹਾਲਤ 6:  type = ਪਾਠ_atom;		ਰੋਕੋ;
	    ਮੂਲ: type = XA_STRING;
	}
	ਜੇ (type == utf8_atom
#  ਜੇ defined(X_HAVE_UTF8_STRING)
		&& !enc_utf8
#  endif
		)
	    // Only request utf-8 when 'encoding' is utf8 and
	    // Xutf8TextPropertyToTextList is available.
	    ਜਾਰੀ;
	success = MAYBE;
	Xt_Get_Selection_ਮੁੱਲ(myShell, cbd->sel_atom, type,
	    clip_x11_request_selection_cb, (Xt_ਸੂਚਕ)&success, Current_ਸਮਾਂ);

	// Make sure the request for the selection goes out before waiting for
	// a response.
	XFlush(dpy);

	/*
	 * Wait for result of selection request, otherwise if we type more
	 * characters, then they will appear before the one that requested the
	 * paste!  Don't worry, we will catch up with any other events later.
	 */
	ਸ਼ੁਰੂ_ਸਮਾਂ = ਸਮਾਂ(ਨਿਰਾ);
	ਜਦੋਂ (success == MAYBE)
	{
	    ਜੇ (XCheckTypedEvent(dpy, PropertyNotify, &event)
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
		ਜਾਰੀ;
	    }

	    // Time out after 2 to 3 seconds to avoid that we hang when the
	    // other process doesn't respond.  Note that the SelectionNotify
	    // event may still come later when the selection owner comes back
	    // to life and the text gets inserted unexpectedly.  Don't know
	    // why that happens or how to avoid that :-(.
	    ਜੇ (ਸਮਾਂ(ਨਿਰਾ) > ਸ਼ੁਰੂ_ਸਮਾਂ + 2)
	    {
		timed_ਬਾਹਰਲਾ = TRUE;
		ਰੋਕੋ;
	    }

	    // Do we need this?  Probably not.
	    XSync(dpy, False);

	    // Wait for 1 msec to avoid that we eat up all CPU time.
	    ui_delay(1L, TRUE);
	}

	ਜੇ (success == TRUE)
	    ਵਾਪਸ;

	// don't do a retry with another type after timing out, otherwise we
	// hang for 15 seconds.
	ਜੇ (timed_ਬਾਹਰਲਾ)
	    ਰੋਕੋ;
    }

    // Final fallback position - use the X CUT_BUFFER0 store
    yank_cut_buffer0(dpy, cbd);
}

    ਖਾਲੀ
clip_x11_lose_selection(Widget myShell, Clipboard_T *cbd)
{
    XtDisownSelection(myShell, cbd->sel_atom,
				Xt_ਆਖਰੀ_Timestamp_Processed(XtDisplay(myShell)));
}

    ਪੂਰਨ
clip_x11_own_selection(Widget myShell, Clipboard_T *cbd)
{
    // When using the GUI we have proper timestamps, use the one of the last
    // event.  When in the console we don't get events (the terminal gets
    // them), Get the time by a zero-length append, clip_x11_timestamp_cb will
    // be called with the current timestamp.
#  ifdef FEAT_GUI
    ਜੇ (gui.ਵਿੱਚ_use)
    {
	ਜੇ (XtOwnSelection(myShell, cbd->sel_atom,
	       Xt_ਆਖਰੀ_Timestamp_Processed(XtDisplay(myShell)),
	       clip_x11_convert_selection_cb, clip_x11_lose_ownership_cb,
	       clip_x11_notify_cb) == False)
	    ਵਾਪਸ FAIL;
    }
    ਨਹੀਂਤਾਂ
#  endif
    {
	ਜੇ (!XChangeProperty(XtDisplay(myShell), Xt_ਖਿੜਕੀ(myShell),
		  cbd->sel_atom, timestamp_atom, 32, Prop_ਬਹੁਲਕ_Append, ਨਿਰਾ, 0))
	    ਵਾਪਸ FAIL;
    }
    // Flush is required in a terminal as nothing else is doing it.
    XFlush(XtDisplay(myShell));
    ਵਾਪਸ OK;
}

/*
 * Send the current selection to the clipboard.  Do nothing for X because we
 * will fill in the selection only when requested by another app.
 */
    ਖਾਲੀ
clip_x11_ਸੈੱਟ_selection(Clipboard_T *cbd ਅਣਵਰਤਿਆ)
{
}

# endif

# ਜੇ (defined(FEAT_XCLIPBOARD) || defined(FEAT_GUI_X11) || defined(FEAT_GUI_GTK)) \
	&& !defined(USE_GTK4)
/*
 * Get the contents of the X CUT_BUFFER0 and put it in "cbd".
 */
    ਖਾਲੀ
yank_cut_buffer0(Display *dpy, Clipboard_T *cbd)
{
    ਪੂਰਨ		nbytes = 0;
    char_u	*ਬਫਰ = (char_u *)XFetch_ਬਫਰ(dpy, &nbytes, 0);

    ਜੇ (nbytes > 0)
    {
	ਪੂਰਨ  ਪੂਰਾ = FALSE;

	// CUT_BUFFER0 is supposed to be always latin1.  Convert to 'enc' when
	// using a multi-byte encoding.  Conversion between two 8-bit
	// character sets usually fails and the text might actually be in
	// 'enc' anyway.
	ਜੇ (has_mbyte)
	{
	    char_u	*conv_buf;
	    vimconv_T	vc;

	    vc.vc_type = CONV_NONE;
	    ਜੇ (convert_ਤਿਆਰੀ(&vc, (char_u *)"latin1", p_enc) == OK)
	    {
		conv_buf = string_convert(&vc, ਬਫਰ, &nbytes);
		ਜੇ (conv_buf != ਨਿਰਾ)
		{
		    clip_yank_selection(MCHAR, conv_buf, (ਲੰਮਾ)nbytes, cbd);
		    vim_free(conv_buf);
		    ਪੂਰਾ = TRUE;
		}
		convert_ਤਿਆਰੀ(&vc, ਨਿਰਾ, ਨਿਰਾ);
	    }
	}
	ਜੇ (!ਪੂਰਾ)  // use the text without conversion
	    clip_yank_selection(MCHAR, ਬਫਰ, (ਲੰਮਾ)nbytes, cbd);
	XFree((ਖਾਲੀ *)ਬਫਰ);
	ਜੇ (p_verbose > 0)
	{
	    verbose_enter();
	    verb_ਸੁਨੇਹਾ(_("Used CUT_BUFFER0 instead of empty selection"));
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
# ਜੇ defined(FEAT_X11) && defined(FEAT_CLIPBOARD)
    ਖਾਲੀ
x11_export_final_selection(ਖਾਲੀ)
{
    Display	*dpy;
    char_u	*str = ਨਿਰਾ;
    long_u	len = 0;
    ਪੂਰਨ		motion_type = -1;

#  ifdef FEAT_GUI
    ਜੇ (gui.ਵਿੱਚ_use)
	dpy = X_DISPLAY;
    ਨਹੀਂਤਾਂ
#  endif
#  ifdef FEAT_XCLIPBOARD
	dpy = xterm_dpy;
#  ਨਹੀਂਤਾਂ
	ਵਾਪਸ;
#  endif

    // Get selection to export
    ਜੇ (clip_plus.owned)
	motion_type = clip_convert_selection(&str, &len, &clip_plus);
    ਨਹੀਂਤਾਂ ਜੇ (clip_ਤਾਰਾ.owned)
	motion_type = clip_convert_selection(&str, &len, &clip_ਤਾਰਾ);

    // Check it's OK
    ਜੇ (dpy != ਨਿਰਾ && str != ਨਿਰਾ && motion_type >= 0
					       && len < 1024*1024 && len > 0)
    {
	ਪੂਰਨ ok = TRUE;

	// The CUT_BUFFER0 is supposed to always contain latin1.  Convert from
	// 'enc' when it is a multi-byte encoding.  When 'enc' is an 8-bit
	// encoding conversion usually doesn't work, so keep the text as-is.
	ਜੇ (has_mbyte)
	{
	    vimconv_T	vc;

	    vc.vc_type = CONV_NONE;
	    ਜੇ (convert_ਤਿਆਰੀ(&vc, p_enc, (char_u *)"latin1") == OK)
	    {
		ਪੂਰਨ	intlen = len;
		char_u	*conv_str;

		vc.vc_fail = TRUE;
		conv_str = string_convert(&vc, str, &intlen);
		len = intlen;
		ਜੇ (conv_str != ਨਿਰਾ)
		{
		    vim_free(str);
		    str = conv_str;
		}
		ਨਹੀਂਤਾਂ
		{
		    ok = FALSE;
		}
		convert_ਤਿਆਰੀ(&vc, ਨਿਰਾ, ਨਿਰਾ);
	    }
	    ਨਹੀਂਤਾਂ
	    {
		ok = FALSE;
	    }
	}

	// Do not store the string if conversion failed.  Better to use any
	// other selection than garbled text.
	ਜੇ (ok)
	{
	    XStore_ਬਫਰ(dpy, (ਅੱਖਰ *)str, (ਪੂਰਨ)len, 0);
	    XFlush(dpy);
	}
    }

    vim_free(str);
}
# endif

    ਖਾਲੀ
clip_free_selection(Clipboard_T *cbd)
{
    yankreg_T *y_ptr = get_y_current();

    ਜੇ (cbd == &clip_plus)
	ਸੈੱਟ_y_current(get_y_ਰਜਿਸਟਰ(PLUS_ਰਜਿਸਟਰ));
    ਨਹੀਂਤਾਂ
	ਸੈੱਟ_y_current(get_y_ਰਜਿਸਟਰ(ਤਾਰਾ_ਰਜਿਸਟਰ));
    free_yank_all();
    get_y_current()->y_ਮਾਪ = 0;
    ਸੈੱਟ_y_current(y_ptr);
}

/*
 * Get the selected text and put it in register '*' or '+'.
 */
    ਖਾਲੀ
clip_get_selection(Clipboard_T *cbd)
{
    yankreg_T	*old_y_ਪਿਛਲਾ, *old_y_current;
    pos_T	old_cursor;
    pos_T	old_visual;
    ਪੂਰਨ		old_visual_ਬਹੁਲਕ;
    colnr_T	old_virtcol;
    colnr_T	old_curswant;
    ਪੂਰਨ		old_ਸੈੱਟ_curswant;
    pos_T	old_op_ਸ਼ੁਰੂ, old_op_ਸਿਰਾ;
    oparg_T	oa;
    cmdarg_T	ca;

    ਜੇ (cbd->owned)
    {
	ਜੇ ((cbd == &clip_plus
		&& get_y_ਰਜਿਸਟਰ(PLUS_ਰਜਿਸਟਰ)->y_array != ਨਿਰਾ)
		|| (cbd == &clip_ਤਾਰਾ
		    && get_y_ਰਜਿਸਟਰ(ਤਾਰਾ_ਰਜਿਸਟਰ)->y_array != ਨਿਰਾ))
	    ਵਾਪਸ;

	// Avoid triggering autocmds such as TextYankPost.
	ਖੰਡ_autocmds();

	// Get the text between clip_star.start & clip_star.end
	old_y_ਪਿਛਲਾ = get_y_ਪਿਛਲਾ();
	old_y_current = get_y_current();
	old_cursor = curwin->w_cursor;
	old_virtcol = curwin->w_virtcol;
	old_curswant = curwin->w_curswant;
	old_ਸੈੱਟ_curswant = curwin->w_ਸੈੱਟ_curswant;
	old_op_ਸ਼ੁਰੂ = curbuf->b_op_ਸ਼ੁਰੂ;
	old_op_ਸਿਰਾ = curbuf->b_op_ਸਿਰਾ;
	old_visual = VIsual;
	old_visual_ਬਹੁਲਕ = VIsual_ਬਹੁਲਕ;
	clear_oparg(&oa);
	oa.regname = (cbd == &clip_plus ? '+' : '*');
	oa.op_type = OP_YANK;
	CLEAR_ਖੇਤਰ(ca);
	ca.oap = &oa;
	ca.cmdchar = 'y';
	ca.count1 = 1;
	ca.retval = CA_NO_ADJ_OP_ਸਿਰਾ;
	do_pending_operator(&ca, 0, TRUE);

	// restore things
	ਸੈੱਟ_y_ਪਿਛਲਾ(old_y_ਪਿਛਲਾ);
	ਸੈੱਟ_y_current(old_y_current);
	curwin->w_cursor = old_cursor;
	curwin->w_virtcol = old_virtcol;
	ਬਦਲਿਆ_cline_bef_curs();   // old w_virtcol et al. may be invalid
	curwin->w_curswant = old_curswant;
	curwin->w_ਸੈੱਟ_curswant = old_ਸੈੱਟ_curswant;
	curbuf->b_op_ਸ਼ੁਰੂ = old_op_ਸ਼ੁਰੂ;
	curbuf->b_op_ਸਿਰਾ = old_op_ਸਿਰਾ;
	VIsual = old_visual;
	VIsual_ਬਹੁਲਕ = old_visual_ਬਹੁਲਕ;

	unblock_autocmds();
    }
    ਨਹੀਂਤਾਂ ਜੇ (!is_clipboard_needs_update())
    {
	clip_free_selection(cbd);

	// Try to get selected text from another window
	clip_gen_request_selection(cbd);
    }
}

/*
 * Convert from the GUI selection string into the '*'/'+' register.
 */
    ਖਾਲੀ
clip_yank_selection(
    ਪੂਰਨ		type,
    char_u	*str,
    ਲੰਮਾ	len,
    Clipboard_T *cbd)
{
    yankreg_T *y_ptr;

    ਜੇ (cbd == &clip_plus)
	y_ptr = get_y_ਰਜਿਸਟਰ(PLUS_ਰਜਿਸਟਰ);
    ਨਹੀਂਤਾਂ
	y_ptr = get_y_ਰਜਿਸਟਰ(ਤਾਰਾ_ਰਜਿਸਟਰ);

    clip_free_selection(cbd);

    str_ਮੰਜ਼ਿਲ_reg(y_ptr, type, str, len, -1, FALSE);
}

    ਪੂਰਨ
clip_convert_selection_ਵਿੱਥ(
	char_u	    **str,
	long_u	    *len,
	ਪੂਰਨ	    ਵਿੱਥ, // Extra space to add in *str and the offset to
			    // place the actual string in *str.
	Clipboard_T *cbd)
{
    char_u	*p;
    ਪੂਰਨ		lnum;
    ਪੂਰਨ		i, j;
    int_u	eolsize;
    yankreg_T	*y_ptr;

    ਜੇ (cbd == &clip_plus)
	y_ptr = get_y_ਰਜਿਸਟਰ(PLUS_ਰਜਿਸਟਰ);
    ਨਹੀਂਤਾਂ
	y_ptr = get_y_ਰਜਿਸਟਰ(ਤਾਰਾ_ਰਜਿਸਟਰ);

# ifdef USE_CRNL
    eolsize = 2;
# ਨਹੀਂਤਾਂ
    eolsize = 1;
# endif

    *str = ਨਿਰਾ;
    *len = 0;
    ਜੇ (y_ptr->y_array == ਨਿਰਾ)
	ਵਾਪਸ -1;

    ਲਈ (i = 0; i < y_ptr->y_ਮਾਪ; i++)
	*len += (long_u)y_ptr->y_array[i].ਲੰਮਾਈ + eolsize;

    // Don't want newline character at end of last line if we're in MCHAR mode.
    ਜੇ (y_ptr->y_type == MCHAR && *len >= eolsize)
	*len -= eolsize;

    *len += ਵਿੱਥ;
    p = *str = alloc(*len + 1);	// add one to avoid zero
    ਜੇ (p == ਨਿਰਾ)
	ਵਾਪਸ -1;
    p += ਵਿੱਥ;
    lnum = 0;
    ਲਈ (i = 0, j = 0; i < (ਪੂਰਨ)*len - ਵਿੱਥ; i++, j++)
    {
	ਜੇ (y_ptr->y_array[lnum].string[j] == '\n')
	    p[i] = NUL;
	ਨਹੀਂਤਾਂ ਜੇ (y_ptr->y_array[lnum].string[j] == NUL)
	{
# ifdef USE_CRNL
	    p[i++] = '\r';
# endif
	    p[i] = '\n';
	    lnum++;
	    j = -1;
	}
	ਨਹੀਂਤਾਂ
	    p[i] = y_ptr->y_array[lnum].string[j];
    }
    ਵਾਪਸ y_ptr->y_type;
}

/*
 * Convert the '*'/'+' register into a GUI selection string returned in *str
 * with length *len.
 * Returns the motion type, or -1 for failure.
 */
    ਪੂਰਨ
clip_convert_selection(char_u **str, long_u *len, Clipboard_T *cbd)
{
    ਵਾਪਸ clip_convert_selection_ਵਿੱਥ(str, len, 0, cbd);
}

/*
 * When "regname" is a clipboard register, obtain the selection.  If it's not
 * available return zero, otherwise return "regname".
 */
    ਪੂਰਨ
may_get_selection(ਪੂਰਨ regname)
{
    ਜੇ (regname == '*')
    {
	ਜੇ (!clip_ਤਾਰਾ.ਉਪਲਬਧ)
	    regname = 0;
	ਨਹੀਂਤਾਂ
	    clip_get_selection(&clip_ਤਾਰਾ);
    }
    ਨਹੀਂਤਾਂ ਜੇ (regname == '+')
    {
	ਜੇ (!clip_plus.ਉਪਲਬਧ)
	    regname = 0;
	ਨਹੀਂਤਾਂ
	    clip_get_selection(&clip_plus);
    }
    ਵਾਪਸ regname;
}

/*
 * If we have written to a clipboard register, send the text to the clipboard.
 */
    ਖਾਲੀ
may_ਸੈੱਟ_selection(ਖਾਲੀ)
{
    ਜੇ ((get_y_current() == get_y_ਰਜਿਸਟਰ(ਤਾਰਾ_ਰਜਿਸਟਰ))
	    && clip_ਤਾਰਾ.ਉਪਲਬਧ)
    {
	clip_own_selection(&clip_ਤਾਰਾ);
	clip_gen_ਸੈੱਟ_selection(&clip_ਤਾਰਾ);
    }
    ਨਹੀਂਤਾਂ ਜੇ ((get_y_current() == get_y_ਰਜਿਸਟਰ(PLUS_ਰਜਿਸਟਰ))
	    && clip_plus.ਉਪਲਬਧ)
    {
	clip_own_selection(&clip_plus);
	clip_gen_ਸੈੱਟ_selection(&clip_plus);
    }
}

# ਜੇ defined(FEAT_WAYLAND_CLIPBOARD)

    ਸਥਿਰ clip_wl_selection_T *
clip_wl_get_selection(wayland_selection_T sel)
{
    ਚੋਣ (sel)
    {
	ਹਾਲਤ WAYLAND_SELECTION_REGULAR:
	    ਵਾਪਸ &clip_wl.regular;
	ਹਾਲਤ WAYLAND_SELECTION_PRIMARY:
	    ਵਾਪਸ &clip_wl.primary;
	ਮੂਲ:
	    ਵਾਪਸ ਨਿਰਾ;
    }
}

    ਸਥਿਰ clip_wl_selection_T *
clip_wl_get_selection_from_cbd(Clipboard_T *cbd)
{
    ਜੇ (cbd == &clip_plus)
	ਵਾਪਸ &clip_wl.regular;
    ਨਹੀਂਤਾਂ ਜੇ (cbd == &clip_ਤਾਰਾ)
	ਵਾਪਸ &clip_wl.primary;
    ਨਹੀਂਤਾਂ
	ਵਾਪਸ ਨਿਰਾ;
}

    ਸਥਿਰ Clipboard_T *
clip_wl_get_cbd_from_selection(clip_wl_selection_T *sel)
{
    ਜੇ (sel == &clip_wl.regular)
	ਵਾਪਸ &clip_plus;
    ਨਹੀਂਤਾਂ ਜੇ (sel == &clip_wl.primary)
	ਵਾਪਸ &clip_ਤਾਰਾ;
    ਨਹੀਂਤਾਂ
	ਵਾਪਸ ਨਿਰਾ;
}

    ਸਥਿਰ wayland_selection_T
clip_wl_get_selection_type(clip_wl_selection_T *sel)
{
    ਜੇ (sel == &clip_wl.regular)
	ਵਾਪਸ WAYLAND_SELECTION_REGULAR;
    ਨਹੀਂਤਾਂ ਜੇ (sel == &clip_wl.primary)
	ਵਾਪਸ WAYLAND_SELECTION_PRIMARY;
    ਨਹੀਂਤਾਂ
	ਵਾਪਸ WAYLAND_SELECTION_NONE;
}

    ਸਥਿਰ bool
wl_ਅੰਕੜੇ_offer_listener_event_offer(
    ਖਾਲੀ *ਅੰਕੜੇ ਅਣਵਰਤਿਆ,
    vwl_ਅੰਕੜੇ_offer_T *offer ਅਣਵਰਤਿਆ,
    ਅਚਲ ਅੱਖਰ *mime_type
)
{
    // Only accept mime type if we support it
    ਲਈ (ਪੂਰਨ i = 0; i < (ਪੂਰਨ)ARRAY_ਲੰਮਾਈ(supported_mimes); i++)
	ਜੇ (STRCMP(mime_type, supported_mimes[i]) == 0)
	    ਵਾਪਸ true;
    ਵਾਪਸ FALSE;
}

ਸਥਿਰ ਅਚਲ vwl_ਅੰਕੜੇ_offer_listener_T vwl_ਅੰਕੜੇ_offer_listener = {
    .offer = wl_ਅੰਕੜੇ_offer_listener_event_offer
};

    ਸਥਿਰ ਖਾਲੀ
vwl_ਅੰਕੜੇ_device_listener_event_ਅੰਕੜੇ_offer(
	ਖਾਲੀ *ਅੰਕੜੇ ਅਣਵਰਤਿਆ,
	vwl_ਅੰਕੜੇ_device_T *device ਅਣਵਰਤਿਆ,
	vwl_ਅੰਕੜੇ_offer_T *offer)
{
    // Immediately start listening for offer events from the data offer
    vwl_ਅੰਕੜੇ_offer_ਜੋੜੋ_listener(offer, &vwl_ਅੰਕੜੇ_offer_listener, ਨਿਰਾ);
}

    ਸਥਿਰ ਖਾਲੀ
vwl_ਅੰਕੜੇ_device_listener_event_selection(
	ਖਾਲੀ *ਅੰਕੜੇ ਅਣਵਰਤਿਆ,
	vwl_ਅੰਕੜੇ_device_T *device ਅਣਵਰਤਿਆ,
	vwl_ਅੰਕੜੇ_offer_T *offer,
	wayland_selection_T selection)
{
    clip_wl_selection_T *sel = clip_wl_get_selection(selection);

    // Destroy previous offer if any, it is now invalid
    vwl_ਅੰਕੜੇ_offer_destroy(sel->offer);

    // There are two cases when sel->offer is NULL
    // 1. No one owns the selection
    // 2. We own the selection (we'll just access the register directly)
    ਜੇ (offer == ਨਿਰਾ || sel->ਸਰੋਤ != ਨਿਰਾ)
    {
	// Selection event is from us, so we are the source client. Therefore
	// ignore it. Or the selection is cleared, so set sel->offer to NULL
	vwl_ਅੰਕੜੇ_offer_destroy(offer);
	sel->offer = ਨਿਰਾ;
	ਵਾਪਸ;
    }

    // Save offer. When we want to request data, then we'll actually call the
    // receive method.
    sel->offer = offer;

}

    ਸਥਿਰ ਖਾਲੀ
vwl_ਅੰਕੜੇ_device_listener_event_ਮੁਕੰਮਲ(
	ਖਾਲੀ *ਅੰਕੜੇ ਅਣਵਰਤਿਆ,
	vwl_ਅੰਕੜੇ_device_T *device)
{
    clip_wl_selection_T *sel;
    // Device finished, guessing this can happen is when the seat becomes
    // invalid? If so, let the user call :wlrestore! to reset. There wouldn't be
    // any point in trying to create another data device for the same seat,
    // since the seat is in an invalid state.
    ਜੇ (device == clip_wl.regular.device)
    {
	sel = &clip_wl.regular;
	clip_wl.regular.device = ਨਿਰਾ;
    }
    ਨਹੀਂਤਾਂ ਜੇ (device == clip_wl.primary.device)
    {
	sel = &clip_wl.primary;
	clip_wl.primary.device = ਨਿਰਾ;
    }
    ਨਹੀਂਤਾਂ
	// Shouldn't happen
	ਵਾਪਸ;

    vim_free(sel->contents);
    vwl_ਅੰਕੜੇ_ਸਰੋਤ_destroy(sel->ਸਰੋਤ);
    vwl_ਅੰਕੜੇ_offer_destroy(sel->offer);
    sel->ਉਪਲਬਧ = FALSE;

    vwl_ਅੰਕੜੇ_device_destroy(device);
}

ਸਥਿਰ ਅਚਲ vwl_ਅੰਕੜੇ_device_listener_T vwl_ਅੰਕੜੇ_device_listener = {
    .ਅੰਕੜੇ_offer = vwl_ਅੰਕੜੇ_device_listener_event_ਅੰਕੜੇ_offer,
    .selection = vwl_ਅੰਕੜੇ_device_listener_event_selection,
    .ਮੁਕੰਮਲ = vwl_ਅੰਕੜੇ_device_listener_event_ਮੁਕੰਮਲ
};

/*
 * Initialize the clipboard for Wayland using the global Wayland connection.
 * Returns OK on success and FAIL on failure.
 */
    ਪੂਰਨ
clip_init_wayland(ਖਾਲੀ)
{
    int_u supported = WAYLAND_SELECTION_NONE;

    ਜੇ (wayland_ct == ਨਿਰਾ)
	ਵਾਪਸ FAIL;

    memset(&clip_wl, 0, ਆਕਾਰ(clip_wl));

    clip_wl.seat = vwl_connection_get_seat(wayland_ct, (ਅੱਖਰ *)p_wse);

    ਜੇ (clip_wl.seat == ਨਿਰਾ)
	ਵਾਪਸ FAIL;

    clip_wl.regular.manager = vwl_connection_get_ਅੰਕੜੇ_device_manager(
	    wayland_ct, WAYLAND_SELECTION_REGULAR, &supported);

    ਜੇ (clip_wl.regular.manager != ਨਿਰਾ)
    {
	clip_wl.regular.device = vwl_ਅੰਕੜੇ_device_manager_get_ਅੰਕੜੇ_device(
		clip_wl.regular.manager, clip_wl.seat);

	ਜੇ (clip_wl.regular.device != ਨਿਰਾ)
	    clip_wl.regular.ਉਪਲਬਧ = true;
	ਨਹੀਂਤਾਂ
	{
	    // Shouldn't happen
	    vwl_ਅੰਕੜੇ_device_manager_discard(clip_wl.regular.manager);
	    clip_wl.regular.manager = ਨਿਰਾ;
	    ਵਾਪਸ FAIL;
	}
    }

    // If we still don't support the primary selection, try finding one for it
    // specifically.
    ਜੇ (!(supported & WAYLAND_SELECTION_PRIMARY))
    {
	clip_wl.primary.manager = vwl_connection_get_ਅੰਕੜੇ_device_manager(
		wayland_ct, WAYLAND_SELECTION_PRIMARY, &supported);

	ਜੇ (clip_wl.primary.manager != ਨਿਰਾ)
	{
	    clip_wl.primary.device = vwl_ਅੰਕੜੇ_device_manager_get_ਅੰਕੜੇ_device(
		    clip_wl.primary.manager, clip_wl.seat);

	    ਜੇ (clip_wl.primary.device != ਨਿਰਾ)
		clip_wl.primary.ਉਪਲਬਧ = true;
	    ਨਹੀਂਤਾਂ
	    {
		vwl_ਅੰਕੜੇ_device_manager_discard(clip_wl.primary.manager);
		clip_wl.primary.manager = ਨਿਰਾ;
	    }
	}
    }

    ਜੇ (clip_wl.regular.ਉਪਲਬਧ && !clip_wl.primary.ਉਪਲਬਧ)
    {
	// The protocol supports both regular and primary selections, just use
	// one data device manager and one data device. Or the primary selection
	// is not supported, make it point to the regular selection instead.
	clip_wl.primary.ਉਪਲਬਧ = true;
	clip_wl.primary.manager = clip_wl.regular.manager;
	clip_wl.primary.device = clip_wl.regular.device;
    }

    ਜੇ (!clip_wl.regular.ਉਪਲਬਧ && !clip_wl.primary.ਉਪਲਬਧ)
	ਵਾਪਸ FAIL;

    // Start listening for selection updates
    ਜੇ (clip_wl.regular.device != ਨਿਰਾ)
	vwl_ਅੰਕੜੇ_device_ਜੋੜੋ_listener(clip_wl.regular.device,
		&vwl_ਅੰਕੜੇ_device_listener, ਨਿਰਾ);
    // Don't want to listen to the same data device twice
    ਜੇ (clip_wl.primary.device != ਨਿਰਾ
	    && clip_wl.primary.device != clip_wl.regular.device)
	vwl_ਅੰਕੜੇ_device_ਜੋੜੋ_listener(clip_wl.primary.device,
		&vwl_ਅੰਕੜੇ_device_listener, ਨਿਰਾ);

    ਵਾਪਸ OK;
}

    ਖਾਲੀ
clip_uninit_wayland(ਖਾਲੀ)
{
    clip_wl_selection_T *sel;

    ਜੇ (clipmethod == CLIPMETHOD_WAYLAND)
    {
	ਜੇ (clip_ਤਾਰਾ.owned)
	    clip_lose_selection(&clip_ਤਾਰਾ);
	ਜੇ (clip_plus.owned)
	    clip_lose_selection(&clip_plus);
    }

    // Don't want to double free
    ਜੇ (clip_wl.regular.manager != clip_wl.primary.manager)
	vwl_ਅੰਕੜੇ_device_manager_discard(clip_wl.primary.manager);
    vwl_ਅੰਕੜੇ_device_manager_discard(clip_wl.regular.manager);

    ਜੇ (clip_wl.regular.device != clip_wl.primary.device)
	vwl_ਅੰਕੜੇ_device_destroy(clip_wl.primary.device);
    vwl_ਅੰਕੜੇ_device_destroy(clip_wl.regular.device);

    sel = &clip_wl.regular;
    ਜਦੋਂ (true)
    {
	vim_free(sel->contents);
	vwl_ਅੰਕੜੇ_ਸਰੋਤ_destroy(sel->ਸਰੋਤ);
	vwl_ਅੰਕੜੇ_offer_destroy(sel->offer);
	sel->ਉਪਲਬਧ = false;

	ਜੇ (sel == &clip_wl.primary)
	    ਰੋਕੋ;
	sel = &clip_wl.primary;
    }

    vim_memset(&clip_wl, 0, ਆਕਾਰ(clip_wl));
}

    ਪੂਰਨ
clip_reset_wayland(ਖਾਲੀ)
{
    wayland_uninit_connection();

    ਜੇ (wayland_init_connection(wayland_display_name) == FAIL
	    || clip_init_wayland() == FAIL)
	ਵਾਪਸ FAIL;

    ਚੁਣੋ_clipmethod();
    ਵਾਪਸ OK;
}


/*
 * Read data from a file descriptor and write it to the given clipboard.
 */
    ਸਥਿਰ ਖਾਲੀ
clip_wl_receive_ਅੰਕੜੇ(Clipboard_T *cbd, ਅਚਲ ਅੱਖਰ *mime_type, ਪੂਰਨ fd)
{
    char_u	*ਸ਼ੁਰੂ, *final;
    ਲੰਮਾ	len;
    garray_T	buf;
    ਪੂਰਨ		motion_type = MAUTO;
    ssize_t	r = 0;
    char_u	*tofree = ਨਿਰਾ;
#  ifndef HAVE_SELECT
    ਬਣਤਰ pollfd   pfd;

    pfd.fd = fd;
    pfd.events = POLLIN;
#  ਨਹੀਂਤਾਂ
    fd_set rfds;
    ਬਣਤਰ timeval  tv;

    FD_ZERO(&rfds);
    FD_SET(fd, &rfds);
#  endif

    // Make pipe (read end) non-blocking
    ਜੇ (fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK) == -1)
	ਵਾਪਸ;

    ga_init2(&buf, 1, 4096);

    // 4096 bytes seems reasonable for initial buffer size, memory is cheap
    // anyways.
    ਜੇ (ga_grow(&buf, 4096) == FAIL)
	ਵਾਪਸ;

    ਸ਼ੁਰੂ = buf.ga_ਅੰਕੜੇ;

#  ifndef HAVE_SELECT
    ਜਦੋਂ (poll(&pfd, 1, p_wtm) > 0)
#  ਨਹੀਂਤਾਂ
    ਜਦੋਂ (tv.tv_sec = p_wtm / 1000, tv.tv_usec = (p_wtm % 1000) * 1000,
	    select(fd + 1, &rfds, ਨਿਰਾ, ਨਿਰਾ, &tv) > 0)
#  endif
    {
	r = ਪਠ(fd, ਸ਼ੁਰੂ, buf.ga_maxlen - 1 - buf.ga_len);

	ਜੇ (r == 0)
	    ਰੋਕੋ;
	ਨਹੀਂਤਾਂ ਜੇ (r < 0)
	{
	    ਜੇ (errno == EAGAIN || errno == EINTR)
		ਜਾਰੀ;
	    ਰੋਕੋ;
	}

	ਸ਼ੁਰੂ += r;
	buf.ga_len += r;

	// Realloc if we are at the end of the buffer
	ਜੇ (buf.ga_len >= buf.ga_maxlen - 1)
	{
	    ਜੇ (ga_grow(&buf, 8192) == FAIL)
		ਰੋਕੋ;
	    ਸ਼ੁਰੂ = (char_u *)buf.ga_ਅੰਕੜੇ + buf.ga_len;
	}
    }

    ਜੇ (buf.ga_len == 0)
    {
	clip_free_selection(cbd); // Nothing received, clear register
	ga_clear(&buf);
	ਵਾਪਸ;
    }

    final = buf.ga_ਅੰਕੜੇ;
    len = buf.ga_len;

    ਜੇ (clip_convert_ਅੰਕੜੇ(&final, &len, &motion_type,
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
    ਸਥਿਰ ਖਾਲੀ
clip_wl_request_selection(Clipboard_T *cbd)
{
    clip_wl_selection_T *sel = clip_wl_get_selection_from_cbd(cbd);
    ਪੂਰਨ			fds[2];
    ਪੂਰਨ			mime_types_len;
    ਅਚਲ ਅੱਖਰ		**mime_types;
    ਅਚਲ ਅੱਖਰ		*chosen_mime = ਨਿਰਾ;

    ਜੇ (!sel->ਉਪਲਬਧ)
	ਜਾਓ clear;

    // Dispatch any events that still queued up before checking for a data
    // offer.
    ਜੇ (vwl_connection_roundtrip(wayland_ct) == FAIL)
	ਜਾਓ clear;

    ਜੇ (sel->offer == ਨਿਰਾ)
	ਜਾਓ clear;

    mime_types_len = sel->offer->mime_types.ga_len;
    mime_types = sel->offer->mime_types.ga_ਅੰਕੜੇ;

    // Choose mime type to receive from. Mime types with a lower index in the
    // "supported_mimes" array are prioritized over ones after it.
    ਲਈ (ਪੂਰਨ i = 0; i < (ਪੂਰਨ)ARRAY_ਲੰਮਾਈ(supported_mimes)
	    && chosen_mime == ਨਿਰਾ; i++)
    {
	ਲਈ (ਪੂਰਨ k = 0; k < mime_types_len && chosen_mime == ਨਿਰਾ; k++)
	    ਜੇ (STRCMP(mime_types[k], supported_mimes[i]) == 0)
		chosen_mime = supported_mimes[i];
    }

    ਜੇ (chosen_mime == ਨਿਰਾ || pipe(fds) == -1)
	ਜਾਓ clear;

    vwl_ਅੰਕੜੇ_offer_receive(sel->offer, chosen_mime, fds[1]);

    close(fds[1]); // Close before we read data so that when the source client
		   // closes their end we receive an EOF.

    ਜੇ (vwl_connection_flush(wayland_ct) >= 0)
	clip_wl_receive_ਅੰਕੜੇ(cbd, chosen_mime, fds[0]);

    close(fds[0]);

    ਵਾਪਸ;
clear:
    clip_free_selection(cbd);
}

    ਸਥਿਰ ਖਾਲੀ
vwl_ਅੰਕੜੇ_ਸਰੋਤ_listener_event_send(
    ਖਾਲੀ *ਅੰਕੜੇ,
    vwl_ਅੰਕੜੇ_ਸਰੋਤ_T *ਸਰੋਤ ਅਣਵਰਤਿਆ,
    ਅਚਲ ਅੱਖਰ *mime_type,
    int32_t fd
)
{
    clip_wl_selection_T *sel = ਅੰਕੜੇ;
    Clipboard_T		*cbd = clip_wl_get_cbd_from_selection(sel);
    bool		have_mime = false;
    ਪੂਰਨ			motion_type;
    long_u		ਲੰਮਾਈ;
    char_u		*string; // Will be reallocated to a bigger size if
				 // needed.
    ਪੂਰਨ			ਵਿੱਥ = 0;
    bool		is_vim, is_vimenc;
    size_t		ਕੁੱਲ = 0;
#  ifndef HAVE_SELECT
    ਬਣਤਰ pollfd   pfd;

    pfd.fd = fd;
    pfd.events = POLLOUT;
#  ਨਹੀਂਤਾਂ
    fd_set	    wfds;
    ਬਣਤਰ timeval  tv;

    FD_ZERO(&wfds);
    FD_SET(fd, &wfds);
#  endif

    // Check if we actually have mime type
    ਲਈ (ਪੂਰਨ i = 0; i < (ਪੂਰਨ)ARRAY_ਲੰਮਾਈ(supported_mimes); i++)
	ਜੇ (STRCMP(supported_mimes[i], mime_type) == 0)
	{
	    have_mime = true;
	    ਰੋਕੋ;
	}

    ਜੇ (!have_mime)
	ਜਾਓ ਬਾਹਰ_ਨਿਕਲੋ;

    // First byte sent is motion type for vim specific formats. For the vimenc
    // format, after the first byte is the encoding type, which is null
    // terminated.

    is_vimenc = STRCMP(mime_type, VIMENC_ATOM_NAME) == 0
	|| STRCMP(mime_type, VIMENC_MIMETYPE_NAME) == 0;
    is_vim = STRCMP(mime_type, VIM_ATOM_NAME) == 0
	|| STRCMP(mime_type, VIM_MIMETYPE_NAME) == 0;

    ਜੇ (is_vimenc)
	ਵਿੱਥ += 2 + STRLEN(p_enc);
    ਨਹੀਂਤਾਂ ਜੇ (is_vim)
	ਵਿੱਥ += 1;

    clip_get_selection(cbd);
    motion_type = clip_convert_selection_ਵਿੱਥ(&string, &ਲੰਮਾਈ, ਵਿੱਥ, cbd);

    ਜੇ (motion_type < 0)
	ਜਾਓ ਬਾਹਰ_ਨਿਕਲੋ;

    ਜੇ (is_vimenc)
    {
	string[0] = (char_u)motion_type;
	// Use vim_strncpy for safer copying
	vim_strncpy(string + 1, p_enc, STRLEN(p_enc));
    }
    ਨਹੀਂਤਾਂ ਜੇ (is_vim)
	string[0] = (char_u)motion_type;


    ਜਦੋਂ (ਕੁੱਲ < (size_t)ਲੰਮਾਈ &&
#  ifndef HAVE_SELECT
	    poll(&pfd, 1, p_wtm) > 0)
#  ਨਹੀਂਤਾਂ
	    ((tv.tv_sec = p_wtm / 1000, tv.tv_usec = (p_wtm % 1000) * 1000),
	    select(fd + 1, ਨਿਰਾ, &wfds, ਨਿਰਾ, &tv) > 0))
#  endif
    {
	ssize_t w = ਲਿਖ(fd, string + ਕੁੱਲ, ਲੰਮਾਈ - ਕੁੱਲ);

	ਜੇ (w == -1)
	    ਰੋਕੋ;
	ਕੁੱਲ += w;
    }

    vim_free(string);
ਬਾਹਰ_ਨਿਕਲੋ:
    close(fd);
}

    ਸਥਿਰ ਖਾਲੀ
vwl_ਅੰਕੜੇ_ਸਰੋਤ_listener_event_cancelled(
	ਖਾਲੀ *ਅੰਕੜੇ,
	vwl_ਅੰਕੜੇ_ਸਰੋਤ_T *ਸਰੋਤ ਅਣਵਰਤਿਆ)
{
    clip_wl_selection_T *sel = ਅੰਕੜੇ;
    Clipboard_T		*cbd = clip_wl_get_cbd_from_selection(sel);

    clip_lose_selection(cbd);
}

ਸਥਿਰ ਅਚਲ vwl_ਅੰਕੜੇ_ਸਰੋਤ_listener_T vwl_ਅੰਕੜੇ_ਸਰੋਤ_listener = {
    .send = vwl_ਅੰਕੜੇ_ਸਰੋਤ_listener_event_send,
    .cancelled = vwl_ਅੰਕੜੇ_ਸਰੋਤ_listener_event_cancelled
};

    ਸਥਿਰ ਖਾਲੀ
clip_wl_do_ਸੈੱਟ_selection(ਖਾਲੀ *ਅੰਕੜੇ, uint32_t serial)
{
    clip_wl_selection_T *sel = ਅੰਕੜੇ;
    wayland_selection_T sel_type = clip_wl_get_selection_type(sel);

    vwl_ਅੰਕੜੇ_device_ਸੈੱਟ_selection(sel->device, sel->ਸਰੋਤ, serial, sel_type);

    sel->own_success = (vwl_connection_roundtrip(wayland_ct) == OK);
}

/*
 * Own the selection that cbd corresponds to. Start listening for requests from
 * other Wayland clients so they can receive data from us. Returns OK on success
 * and FAIL on failure.
 */
    ਸਥਿਰ ਪੂਰਨ
clip_wl_own_selection(Clipboard_T *cbd)
{
    clip_wl_selection_T *sel = clip_wl_get_selection_from_cbd(cbd);
    wayland_selection_T sel_type = clip_wl_get_selection_type(sel);

    ਜੇ (!sel->ਉਪਲਬਧ || vwl_connection_roundtrip(wayland_ct) == FAIL)
	ਵਾਪਸ FAIL;

    ਜੇ (sel->ਸਰੋਤ != ਨਿਰਾ)
    {
	ਜੇ (sel_type == WAYLAND_SELECTION_PRIMARY)
	    // We already own the selection, ignore (only do this for primary
	    // selection). We don't re set the selection because then we would
	    // be setting the selection every time the user moves the visual
	    // selection cursor, which is messy and inefficient. Some
	    // applications like Google Chrome do it this way however.
	    ਵਾਪਸ OK;
	ਨਹੀਂਤਾਂ ਜੇ (sel_type == WAYLAND_SELECTION_REGULAR)
	{
	    // Technically we don't need to do this as we already own the
	    // selection, however if a user yanks text a second time, the
	    // text yanked won't appear in their clipboard manager if they are
	    // using one.
	    //
	    // This can be unexpected behaviour for the user so its probably
	    // better to do it this way. Additionally other Wayland applications
	    // seem to set the selection every time.
	    vwl_ਅੰਕੜੇ_ਸਰੋਤ_destroy(sel->ਸਰੋਤ);
	}
	ਨਹੀਂਤਾਂ
	    // Shouldn't happen
	    ਵਾਪਸ FAIL;
    }

    sel->ਸਰੋਤ = vwl_ਅੰਕੜੇ_device_manager_create_ਅੰਕੜੇ_ਸਰੋਤ(sel->manager);
    vwl_ਅੰਕੜੇ_ਸਰੋਤ_ਜੋੜੋ_listener(sel->ਸਰੋਤ, &vwl_ਅੰਕੜੇ_ਸਰੋਤ_listener, sel);

    // Advertise mime types
    ਲਈ (ਪੂਰਨ i = 0; i < (ਪੂਰਨ)ARRAY_ਲੰਮਾਈ(supported_mimes); i++)
	vwl_ਅੰਕੜੇ_ਸਰੋਤ_offer(sel->ਸਰੋਤ, supported_mimes[i]);

    sel->own_success = false;
    clip_wl_do_ਸੈੱਟ_selection(sel, 0);

    ਜੇ (!sel->own_success)
	ਜਾਓ fail;

    ਵਾਪਸ OK;
fail:
    vwl_ਅੰਕੜੇ_ਸਰੋਤ_destroy(sel->ਸਰੋਤ);
    sel->ਸਰੋਤ = ਨਿਰਾ;
    ਵਾਪਸ FAIL;
}

/*
 * Disown the selection that cbd corresponds to.
 */
    ਸਥਿਰ ਖਾਲੀ
clip_wl_lose_selection(Clipboard_T *cbd)
{
    clip_wl_selection_T *sel = clip_wl_get_selection_from_cbd(cbd);

    ਜੇ (!sel->ਉਪਲਬਧ)
	ਵਾਪਸ;

    vwl_ਅੰਕੜੇ_ਸਰੋਤ_destroy(sel->ਸਰੋਤ);
    sel->ਸਰੋਤ = ਨਿਰਾ;
}

/*
 * Send the current selection to the clipboard. Do nothing for Wayland because
 * we will fill in the selection only when requested by another client.
 */
    ਸਥਿਰ ਖਾਲੀ
clip_wl_ਸੈੱਟ_selection(Clipboard_T *cbd ਅਣਵਰਤਿਆ)
{
}

#  ਜੇ defined(USE_SYSTEM)
/*
 * Return true if we own the selection corresponding to cbd or another client
 * does.
 */
    ਸਥਿਰ bool
clip_wl_owner_exists(Clipboard_T *cbd)
{
    clip_wl_selection_T *sel = clip_wl_get_selection_from_cbd(cbd);

    ਜੇ (vwl_connection_roundtrip(wayland_ct) == FAIL)
	ਵਾਪਸ false;

    ਵਾਪਸ sel->ਉਪਲਬਧ && (sel->ਸਰੋਤ != ਨਿਰਾ || sel->offer != ਨਿਰਾ);
}
#  endif

# endif // FEAT_WAYLAND_CLIPBOARD

#ਜੇ_ਅੰਤ // FEAT_CLIPBOARD

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_CLIPMETHOD

/*
 * Returns the first method for accessing the clipboard that is available/works,
 * depending on the order of values in str.
 */
    ਸਥਿਰ clipmethod_T
get_clipmethod(char_u *str)
{
    ਪੂਰਨ		len	= (ਪੂਰਨ)STRLEN(str) + 1;
    char_u	*buf	= alloc(len);

    ਜੇ (buf == ਨਿਰਾ)
	ਵਾਪਸ CLIPMETHOD_FAIL;

    clipmethod_T ret = CLIPMETHOD_FAIL;
    char_u	*p = str;

    ਜਦੋਂ (*p != NUL)
    {
	clipmethod_T ਵਿਧੀ = CLIPMETHOD_NONE;

	(ਖਾਲੀ)copy_option_part(&p, buf, len, ",");

	ਜੇ (STRCMP(buf, "wayland") == 0)
	{
# ifdef FEAT_GUI
	    ਜੇ (!gui.ਵਿੱਚ_use)
# endif
	    {
# ifdef FEAT_WAYLAND_CLIPBOARD
		ਜੇ (clip_wl.regular.ਉਪਲਬਧ || clip_wl.primary.ਉਪਲਬਧ)
		    ਵਿਧੀ = CLIPMETHOD_WAYLAND;
# endif
	    }
	}
	ਨਹੀਂਤਾਂ ਜੇ (STRCMP(buf, "x11") == 0)
	{
# ifdef FEAT_GUI
	    ਜੇ (!gui.ਵਿੱਚ_use)
# endif
	    {
# ifdef FEAT_XCLIPBOARD
		// x_IOerror_handler() in os_unix.c should set xterm_dpy to NULL if
		// we lost connection to the X server.
		ਜੇ (xterm_dpy != ਨਿਰਾ)
		{
		    // If the X connection is lost then that handler will longjmp
		    // somewhere else, in that case we will call choose_clipmethod()
		    // again from there, and this if block won't be executed since
		    // xterm_dpy will be set to NULL.
		    xterm_update();
		    ਵਿਧੀ = CLIPMETHOD_X11;
		}
# endif
	    }
	}
	ਨਹੀਂਤਾਂ
	{
# ifdef FEAT_CLIPBOARD_PROVIDER
	    // Check if name matches a clipboard provider
	    ਪੂਰਨ r = clip_provider_is_ਉਪਲਬਧ(buf);

	    ਜੇ (r == 1)
	    {
		ਵਿਧੀ = CLIPMETHOD_PROVIDER;
		ਜੇ (ret == CLIPMETHOD_FAIL)
		{
		    vim_free(clip_provider);
		    clip_provider = vim_strsave(buf);
		    ਜੇ (clip_provider == ਨਿਰਾ)
			ਜਾਓ fail;
		}
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (r == -1)
# endif
	    {
# ifdef FEAT_CLIPBOARD_PROVIDER
fail:
# endif
		ret = CLIPMETHOD_FAIL;
		ਜਾਓ ਬਾਹਰ_ਨਿਕਲੋ;
	    }
	}

	// Keep on going in order to catch errors
	ਜੇ (ਵਿਧੀ != CLIPMETHOD_NONE && ret == CLIPMETHOD_FAIL)
	    ret = ਵਿਧੀ;
    }

    // No match found, use "none".
    ret = (ret == CLIPMETHOD_FAIL) ? CLIPMETHOD_NONE : ret;

ਬਾਹਰ_ਨਿਕਲੋ:
    vim_free(buf);
    ਵਾਪਸ ret;
}


/*
 * Returns name of clipmethod in a statically allocated string.
 */
    ਸਥਿਰ char_u *
clipmethod_ਮੰਜ਼ਿਲ_str(clipmethod_T ਵਿਧੀ)
{
    ਚੋਣ(ਵਿਧੀ)
    {
	ਹਾਲਤ CLIPMETHOD_WAYLAND:
	    ਵਾਪਸ (char_u *)"wayland";
	ਹਾਲਤ CLIPMETHOD_X11:
	    ਵਾਪਸ (char_u *)"x11";
	ਹਾਲਤ CLIPMETHOD_PROVIDER:
# ifdef FEAT_EVAL
	    ਵਾਪਸ clip_provider;
# endif
	ਮੂਲ:
	    ਵਾਪਸ (char_u *)"none";
    }
}

/*
 * Sets the current clipmethod to use given by `get_clipmethod()`. Returns an
 * error message on failure else NULL.
 */
    ਅੱਖਰ *
ਚੁਣੋ_clipmethod(ਖਾਲੀ)
{
    clipmethod_T ਵਿਧੀ = get_clipmethod(p_cpm);

    ਜੇ (ਵਿਧੀ == CLIPMETHOD_FAIL)
	ਵਾਪਸ e_invalid_ਦਲੀਲ;

// If GUI is running or we are not on a system with Wayland or X11, then always
// return CLIPMETHOD_NONE. System or GUI clipboard handling always overrides.
// This is unless a provider is being used.
# ਜੇ defined(FEAT_XCLIPBOARD) || defined(FEAT_WAYLAND_CLIPBOARD)
#  ਜੇ defined(FEAT_GUI)
    ਜੇ (ਵਿਧੀ != CLIPMETHOD_PROVIDER && gui.ਵਿੱਚ_use)
    {
#   ifdef FEAT_WAYLAND
	// We only interact with Wayland for the clipboard, we can just deinit
	// everything.
	wayland_uninit_connection();
#   endif

	ਵਿਧੀ = CLIPMETHOD_NONE;
	ਜਾਓ lose_sel_exit;
    }
#  endif
# ਨਹੀਂਤਾਂ
    // If on a system like windows or macos, then clipmethod is irrelevant, we
    // use their way of accessing the clipboard. This is unless we are using the
    // clipboard provider
#  ifdef FEAT_CLIPBOARD_PROVIDER
    ਜੇ (ਵਿਧੀ != CLIPMETHOD_PROVIDER)
#  endif
    {
	ਵਿਧੀ = CLIPMETHOD_NONE;
	ਜਾਓ ਬਾਹਰ_ਨਿਕਲੋ;
    }
# endif

# ifdef FEAT_CLIPBOARD
    // Deinitialize clipboard if there is no way to access clipboard
    ਜੇ (ਵਿਧੀ == CLIPMETHOD_NONE)
	clip_init(FALSE);
    // If we have a clipmethod that works now, then initialize clipboard
    ਨਹੀਂਤਾਂ ਜੇ (clipmethod == CLIPMETHOD_NONE && ਵਿਧੀ != CLIPMETHOD_NONE)
    {
	clip_init(TRUE);
	did_warn_clipboard = false;
    }
    // Disown clipboard if we are switching to a new method
    ਨਹੀਂਤਾਂ ਜੇ (clipmethod != CLIPMETHOD_NONE && ਵਿਧੀ != clipmethod)
    {
#  ਜੇ (defined(FEAT_XCLIPBOARD) || defined(FEAT_WAYLAND_CLIPBOARD)) \
	&& defined(FEAT_GUI)
lose_sel_exit:
#  endif
	ਜੇ (clip_ਤਾਰਾ.owned)
	    clip_lose_selection(&clip_ਤਾਰਾ);
	ਜੇ (clip_plus.owned)
	    clip_lose_selection(&clip_plus);

#  ਜੇ defined(FEAT_GUI)
	ਜੇ (!gui.ਵਿੱਚ_use)
#  endif
	{
	    clip_init(TRUE);
	    did_warn_clipboard = false;
	}
    }
# endif // FEAT_CLIPBOARD

# ਜੇ !defined(FEAT_XCLIPBOARD) && !defined(FEAT_WAYLAND_CLIPBOARD)
ਬਾਹਰ_ਨਿਕਲੋ:
# endif

    clipmethod = ਵਿਧੀ;

# ifdef FEAT_EVAL
    ਸੈੱਟ_vim_var_string(VV_CLIPMETHOD, (char_u*)clipmethod_ਮੰਜ਼ਿਲ_str(ਵਿਧੀ), -1);
# endif

    ਵਾਪਸ ਨਿਰਾ;
}

/*
 * Call choose_clipmethod().
 */
    ਖਾਲੀ
ex_clipreset(exarg_T *eap ਅਣਵਰਤਿਆ)
{
    clipmethod_T prev = clipmethod;

    ਚੁਣੋ_clipmethod();

    ਜੇ (clipmethod == CLIPMETHOD_NONE)
	smsg(_("Could not find a way to access the clipboard."));
    ਨਹੀਂਤਾਂ ਜੇ (clipmethod != prev)
	smsg(_("Switched to clipboard method '%s'."),
		clipmethod_ਮੰਜ਼ਿਲ_str(clipmethod));
}

/*
 * Adjust the register name pointed to with "rp" for the clipboard being
 * used always and the clipboard being available.
 */
    ਖਾਲੀ
adjust_clip_reg(ਪੂਰਨ *rp)
{
# ifdef FEAT_CLIPBOARD_PROVIDER
    ਜੇ (clipmethod == CLIPMETHOD_PROVIDER)
    {
	ਜੇ (*rp == 0 && clip_unnamed != 0)
	    *rp = ((clip_unnamed & CLIP_UNNAMED_PLUS)) ? '+' : '*';
	ਵਾਪਸ;
    }
# endif
# ifdef FEAT_CLIPBOARD
    // If no reg. specified, and "unnamed" or "unnamedplus" is in 'clipboard',
    // use '*' or '+' reg, respectively. "unnamedplus" prevails.
    ਜੇ (*rp == 0 && (clip_unnamed != 0 || clip_unnamed_saved != 0))
    {
	ਜੇ (clip_unnamed != 0)
	    *rp = ((clip_unnamed & CLIP_UNNAMED_PLUS) && clip_plus.ਉਪਲਬਧ)
								  ? '+' : '*';
	ਨਹੀਂਤਾਂ
	    *rp = ((clip_unnamed_saved & CLIP_UNNAMED_PLUS)
					   && clip_plus.ਉਪਲਬਧ) ? '+' : '*';
    }
    ਜੇ ((!clip_ਤਾਰਾ.ਉਪਲਬਧ && *rp == '*') ||
	   (!clip_plus.ਉਪਲਬਧ && *rp == '+'))
    {
	ਸੁਨੇਹਾ_warn_missing_clipboard();
	*rp = 0;
    }
# endif
}

/*
 * Extract the items in the 'clipboard' option and set global values.
 * Return an error message or NULL for success.
 */
    ਅੱਖਰ *
did_ਸੈੱਟ_clipboard(optset_T *ਦਲੀਲਾਂ ਅਣਵਰਤਿਆ)
{
    ਪੂਰਨ		new_unnamed = 0;
# ifdef FEAT_CLIPBOARD
    ਪੂਰਨ		new_autoselect_ਤਾਰਾ = FALSE;
    ਪੂਰਨ		new_autoselect_plus = FALSE;
    ਪੂਰਨ		new_autoselectml = FALSE;
    ਪੂਰਨ		new_html = FALSE;
# endif
    regprog_T	*new_exclude_prog = ਨਿਰਾ;
    ਅੱਖਰ	*errmsg = ਨਿਰਾ;
    char_u	*p;

    ਲਈ (p = p_cb; *p != NUL; )
    {
	// Note: Keep this in sync with p_cb_values.
	ਜੇ (STRNCMP(p, "unnamed", 7) == 0 && (p[7] == ',' || p[7] == NUL))
	{
	    new_unnamed |= CLIP_UNNAMED;
	    p += 7;
	}
	ਨਹੀਂਤਾਂ ਜੇ (STRNCMP(p, "unnamedplus", 11) == 0
					    && (p[11] == ',' || p[11] == NUL))
	{
	    new_unnamed |= CLIP_UNNAMED_PLUS;
	    p += 11;
	}
# ifdef FEAT_CLIPBOARD
	ਨਹੀਂਤਾਂ ਜੇ (STRNCMP(p, "autoselect", 10) == 0
					    && (p[10] == ',' || p[10] == NUL))
	{
	    new_autoselect_ਤਾਰਾ = TRUE;
	    p += 10;
	}
	ਨਹੀਂਤਾਂ ਜੇ (STRNCMP(p, "autoselectplus", 14) == 0
					    && (p[14] == ',' || p[14] == NUL))
	{
	    new_autoselect_plus = TRUE;
	    p += 14;
	}
	ਨਹੀਂਤਾਂ ਜੇ (STRNCMP(p, "autoselectml", 12) == 0
					    && (p[12] == ',' || p[12] == NUL))
	{
	    new_autoselectml = TRUE;
	    p += 12;
	}
	ਨਹੀਂਤਾਂ ਜੇ (STRNCMP(p, "html", 4) == 0 && (p[4] == ',' || p[4] == NUL))
	{
	    new_html = TRUE;
	    p += 4;
	}
	ਨਹੀਂਤਾਂ ਜੇ (STRNCMP(p, "exclude:", 8) == 0 && new_exclude_prog == ਨਿਰਾ)
	{
	    p += 8;
	    new_exclude_prog = vim_regcomp(p, RE_MAGIC);
	    ਜੇ (new_exclude_prog == ਨਿਰਾ)
		errmsg = e_invalid_ਦਲੀਲ;
	    ਰੋਕੋ;
	}
# endif
	ਨਹੀਂਤਾਂ
	{
	    errmsg = e_invalid_ਦਲੀਲ;
	    ਰੋਕੋ;
	}
	ਜੇ (*p == ',')
	    ++p;
    }
    ਜੇ (errmsg == ਨਿਰਾ)
    {
# ifdef FEAT_CLIPBOARD
	ਜੇ (global_busy)
	    // clip_unnamed will be reset to clip_unnamed_saved
	    // at end_global_changes
	    clip_unnamed_saved = new_unnamed;
	ਨਹੀਂਤਾਂ
# endif
	    clip_unnamed = new_unnamed;
# ifdef FEAT_CLIPBOARD
	clip_autoselect_ਤਾਰਾ = new_autoselect_ਤਾਰਾ;
	clip_autoselect_plus = new_autoselect_plus;
	clip_autoselectml = new_autoselectml;
	clip_html = new_html;
	vim_regfree(clip_exclude_prog);
	clip_exclude_prog = new_exclude_prog;
# endif
# ਜੇ defined(FEAT_GUI_GTK)
	ਜੇ (gui.ਵਿੱਚ_use)
	{
#  ifdef USE_GTK4
	    gui_gtk_update_selection_formats(&clip_plus);
	    gui_gtk_update_selection_formats(&clip_ਤਾਰਾ);
#  ਨਹੀਂਤਾਂ
	    gui_gtk_ਸੈੱਟ_selection_targets((GdkAtom)GDK_SELECTION_PRIMARY);
	    gui_gtk_ਸੈੱਟ_selection_targets((GdkAtom)clip_plus.gtk_sel_atom);
#  endif
#  ifdef FEAT_DND
	    gui_gtk_ਸੈੱਟ_dnd_targets();
#  endif
	}
# endif
    }
    ਨਹੀਂਤਾਂ
	vim_regfree(new_exclude_prog);

    ਵਾਪਸ errmsg;
}

#ਜੇ_ਅੰਤ // HAVE_CLIPMETHOD

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CLIPBOARD_PROVIDER

/*
 * Check if a clipboard provider with given name is available. Returns 1 if available,
 * 0 if not available, and -1 on error
 */
    ਸਥਿਰ ਪੂਰਨ
clip_provider_is_ਉਪਲਬਧ(char_u *provider)
{
    dict_T	*providers = get_vim_var_dict(VV_CLIPPROVIDERS);
    typval_T	provider_tv = {0};
    callback_T	callback = {0};
    typval_T	rettv = {0};
    typval_T	func_tv = {0};
    ਪੂਰਨ		res = 0;

    ਜੇ (dict_get_tv(providers, (ਅੱਖਰ *)provider, &provider_tv) == FAIL
	    || provider_tv.v_type != VAR_DICT)
	// clipboard provider not defined
	ਵਾਪਸ -1;

    ਜੇ (dict_get_tv(provider_tv.vval.v_dict, "available", &func_tv) == FAIL)
    {
	clear_tv(&provider_tv);
	// If "available" function not specified assume always TRUE
	ਵਾਪਸ 1;
    }

    ਜੇ ((callback = get_callback(&func_tv)).cb_name == ਨਿਰਾ)
	ਜਾਓ fail;

    ਜੇ (call_callback(&callback, -1, &rettv, 0, ਨਿਰਾ) == FAIL ||
	    (rettv.v_type != VAR_BOOL && rettv.v_type != VAR_ਸੰਖਿਆ))
	ਜਾਓ fail;

    ਜੇ (rettv.vval.v_ਸੰਖਿਆ)
	res = 1;

    ਜੇ (FALSE)
fail:
	res = -1;

    free_callback(&callback);
    clear_tv(&func_tv);
    clear_tv(&rettv);
    clear_tv(&provider_tv);

    ਵਾਪਸ res;
}

/*
 * Get the specified callback "function" from the provider dictionary for
 * register "reg".
 */
    ਸਥਿਰ ਪੂਰਨ
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

    ਜੇ (dict_get_tv(providers, (ਅੱਖਰ *)provider, &provider_tv) == FAIL)
	ਵਾਪਸ FAIL;
    ਨਹੀਂਤਾਂ ਜੇ (provider_tv.v_type != VAR_DICT)
    {
	clear_tv(&provider_tv);
	ਵਾਪਸ FAIL;
    }
    ਨਹੀਂਤਾਂ ਜੇ (dict_get_tv(
		provider_tv.vval.v_dict,
		(ਅੱਖਰ *)function,
		&action_tv) == FAIL)
    {
	clear_tv(&provider_tv);
	ਵਾਪਸ FAIL;
    }
    ਨਹੀਂਤਾਂ ਜੇ (action_tv.v_type != VAR_DICT)
    {
	clear_tv(&provider_tv);
	clear_tv(&action_tv);
	ਵਾਪਸ FAIL;
    }
    ਨਹੀਂਤਾਂ ਜੇ (dict_get_tv(action_tv.vval.v_dict, (ਅੱਖਰ *)reg, &func_tv) == FAIL)
    {
	clear_tv(&provider_tv);
	clear_tv(&action_tv);
	ਵਾਪਸ FAIL;
    }
    ਨਹੀਂਤਾਂ ਜੇ ((cb = get_callback(&func_tv)).cb_name == ਨਿਰਾ)
    {
	clear_tv(&provider_tv);
	clear_tv(&action_tv);
	clear_tv(&func_tv);
	ਵਾਪਸ FAIL;
    }
    clear_tv(&provider_tv);
    clear_tv(&action_tv);

    // func_tv owns the function name, so we must make a copy for the callback
    ਸੈੱਟ_callback(callback, &cb);
    clear_tv(&func_tv);
    ਵਾਪਸ OK;
}

    ਸਥਿਰ ਖਾਲੀ
clip_provider_copy(char_u *reg, char_u *provider)
{
    ਸਥਿਰ bool	recursive = false;
    callback_T	callback;
    typval_T	rettv;
    typval_T	argvars[4];
    yankreg_T	*y_ptr;
    char_u	type[2 + NUMBUFLEN] = {0};
    list_T	*list = ਨਿਰਾ;

    ਜੇ (recursive)
	ਵਾਪਸ;

    ਜੇ (clip_provider_get_callback(
		reg,
		provider,
		(char_u *)"copy",
		&callback) == FAIL)
	ਵਾਪਸ;

    // Convert register type into a string
    ਜੇ (*reg == '+')
	y_ptr = get_y_ਰਜਿਸਟਰ(REAL_PLUS_ਰਜਿਸਟਰ);
    ਨਹੀਂਤਾਂ
	y_ptr = get_y_ਰਜਿਸਟਰ(ਤਾਰਾ_ਰਜਿਸਟਰ);

    ਚੋਣ (y_ptr->y_type)
    {
	ਹਾਲਤ MCHAR:
	    type[0] = 'v';
	    ਰੋਕੋ;
	ਹਾਲਤ MLINE:
	    type[0] = 'V';
	    ਰੋਕੋ;
	ਹਾਲਤ MBLOCK:
	    sprintf((ਅੱਖਰ *)type, "%c%d", Ctrl_V, y_ptr->y_ਚੌੜਾਈ + 1);
	    ਰੋਕੋ;
	ਮੂਲ:
	    type[0] = 0;
	    ਰੋਕੋ;
    }

    argvars[0].v_type = VAR_STRING;
    argvars[0].vval.v_string = reg;

    argvars[1].v_type = VAR_STRING;
    argvars[1].vval.v_string = type;

    // Get register contents by creating a list of lines
    list = list_alloc();

    ਜੇ (list == ਨਿਰਾ)
    {
	free_callback(&callback);
	ਵਾਪਸ;
    }

    ਲਈ (ਪੂਰਨ i = 0; i < y_ptr->y_ਮਾਪ; i++)
	ਜੇ (list_append_string(list, y_ptr->y_array[i].string,
	    (ਪੂਰਨ)y_ptr->y_array[i].ਲੰਮਾਈ) == FAIL)
	{
	    free_callback(&callback);
	    list_unref(list);
	    ਵਾਪਸ;
	}

    list->lv_refcount++;

    argvars[2].v_type = VAR_LIST;
    argvars[2].v_ਤਾਲਾ = VAR_FIXED;
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

    ਸਥਿਰ ਖਾਲੀ
clip_provider_paste(char_u *reg, char_u *provider)
{
    ਸਥਿਰ bool	recursive = false;
    callback_T	callback;
    typval_T	argvars[2];
    typval_T	rettv;
    ਪੂਰਨ		ret;
    char_u	*reg_type;
    list_T	*ਸਤਰਾਂ;

    ਜੇ (recursive)
	ਵਾਪਸ;

    ਜੇ (clip_provider_get_callback(
		reg,
		provider,
		(char_u *)"paste",
		&callback) == FAIL)
	ਵਾਪਸ;

    argvars[0].v_type = VAR_STRING;
    argvars[0].vval.v_string = reg;

    argvars[1].v_type = VAR_UNKNOWN;

    textlock++;
    recursive = true;
    ret = call_callback(&callback, -1, &rettv, 1, argvars);
    recursive = false;
    textlock--;

    ਜੇ (ret == FAIL)
	ਜਾਓ ਬਾਹਰ_ਨਿਕਲੋ;
    ਨਹੀਂਤਾਂ ਜੇ (rettv.v_type == VAR_TUPLE
	    && TUPLE_LEN(rettv.vval.v_tuple) == 2
	    && TUPLE_ਵਸਤੂ(rettv.vval.v_tuple, 0)->v_type == VAR_STRING
	    && TUPLE_ਵਸਤੂ(rettv.vval.v_tuple, 1)->v_type == VAR_LIST)
    {
	reg_type = TUPLE_ਵਸਤੂ(rettv.vval.v_tuple, 0)->vval.v_string;
	ਸਤਰਾਂ = TUPLE_ਵਸਤੂ(rettv.vval.v_tuple, 1)->vval.v_list;
    }
    ਨਹੀਂਤਾਂ ਜੇ (rettv.v_type == VAR_LIST
	    && rettv.vval.v_list->lv_len == 2
	    && rettv.vval.v_list->lv_ਪਹਿਲਾ->li_tv.v_type == VAR_STRING
	    && rettv.vval.v_list->lv_ਪਹਿਲਾ->li_ਅਗਲਾ->li_tv.v_type == VAR_LIST)
    {
	reg_type = rettv.vval.v_list->lv_ਪਹਿਲਾ->li_tv.vval.v_string;
	ਸਤਰਾਂ = rettv.vval.v_list->lv_ਪਹਿਲਾ->li_ਅਗਲਾ->li_tv.vval.v_list;
    }
    ਨਹੀਂਤਾਂ
	ਜਾਓ ਬਾਹਰ_ਨਿਕਲੋ;

    {
	char_u		yank_type = MAUTO;
	ਲੰਮਾ		ਖੰਡ_len = -1;
	yankreg_T	*y_ptr, *cur_y_ptr;
	char_u		**lstval;
	char_u		**allocval;
	char_u		buf[NUMBUFLEN];
	char_u		**curval;
	char_u		**curallocval;
	char_u		*strval;
	listitem_T	*li;
	ਪੂਰਨ		len;

	// If the list is NULL handle like an empty list.
	len = ਸਤਰਾਂ == ਨਿਰਾ ? 0 : ਸਤਰਾਂ->lv_len;

	// First half: use for pointers to result lines; second half: use for
	// pointers to allocated copies.
	lstval = ALLOC_MULT(char_u *, (len + 1) * 2);
	ਜੇ (lstval == ਨਿਰਾ)
	    ਜਾਓ ਬਾਹਰ_ਨਿਕਲੋ;
	curval = lstval;
	allocval = lstval + len + 2;
	curallocval = allocval;

	ਜੇ (ਸਤਰਾਂ != ਨਿਰਾ)
	{
	    CHECK_LIST_MATERIALIZE(ਸਤਰਾਂ);
	    FOR_ALL_LIST_ਚੀਜ਼ਾਂ(ਸਤਰਾਂ, li)
	    {
		strval = tv_get_string_buf_chk(&li->li_tv, buf);
		ਜੇ (strval == ਨਿਰਾ)
		    ਜਾਓ free_lstval;
		ਜੇ (strval == buf)
		{
		    // Need to make a copy, next tv_get_string_buf_chk() will
		    // overwrite the string.
		    strval = vim_strsave(buf);
		    ਜੇ (strval == ਨਿਰਾ)
			ਜਾਓ free_lstval;
		    *curallocval++ = strval;
		}
		*curval++ = strval;
	    }
	}
	*curval++ = ਨਿਰਾ;

	ਜੇ (*reg_type != NUL && (STRLEN(reg_type) == 0
		|| get_yank_type(&reg_type, &yank_type, &ਖੰਡ_len) == FAIL))
	{
	    emsg(e_invalid_ਦਲੀਲ);
	    ਜਾਓ free_lstval;
	}

	ਜੇ (*reg == '+')
	    y_ptr = get_y_ਰਜਿਸਟਰ(REAL_PLUS_ਰਜਿਸਟਰ);
	ਨਹੀਂਤਾਂ
	    y_ptr = get_y_ਰਜਿਸਟਰ(ਤਾਰਾ_ਰਜਿਸਟਰ);

	// Free previous register contents
	cur_y_ptr = get_y_current();
	ਸੈੱਟ_y_current(y_ptr);

	free_yank_all();
	get_y_current()->y_ਮਾਪ = 0;

	ਸੈੱਟ_y_current(cur_y_ptr);

	str_ਮੰਜ਼ਿਲ_reg(y_ptr,
		yank_type,
		(char_u *)lstval,
		-1,
		ਖੰਡ_len,
		TRUE);

free_lstval:
	ਜਦੋਂ (curallocval > allocval)
	    vim_free(*--curallocval);
	vim_free(lstval);
    }

ਬਾਹਰ_ਨਿਕਲੋ:
    free_callback(&callback);
    clear_tv(&rettv);
}

// Used to stop calling the provider callback every time there is an update.
// This prevents unnecessary calls when accessing the provider often in an
// interval.
//
// If -1 then allow provider callback to be called then set to one. Default
// value (is allowed) is -2.
ਸਥਿਰ ਪੂਰਨ ਤਾਰਾ_pause_ਗਿਣਤੀ = -2, plus_pause_ਗਿਣਤੀ = -2;

    ਖਾਲੀ
call_clip_provider_request(ਪੂਰਨ reg)
{
    ਜੇ (clipmethod != CLIPMETHOD_PROVIDER)
	ਵਾਪਸ;

    ਜੇ (reg == '+' && plus_pause_ਗਿਣਤੀ < 0)
    {
	ਜੇ (plus_pause_ਗਿਣਤੀ == -1)
	    plus_pause_ਗਿਣਤੀ = 1;
	clip_provider_paste((char_u *)"+", clip_provider);
    }
    ਨਹੀਂਤਾਂ ਜੇ (reg == '*' && ਤਾਰਾ_pause_ਗਿਣਤੀ < 0)
    {
	ਜੇ (ਤਾਰਾ_pause_ਗਿਣਤੀ == -1)
	    ਤਾਰਾ_pause_ਗਿਣਤੀ = 1;
	clip_provider_paste((char_u *)"*", clip_provider);
    }
    ਨਹੀਂਤਾਂ
	ਵਾਪਸ;
}

    ਖਾਲੀ
call_clip_provider_ਸੈੱਟ(ਪੂਰਨ reg)
{
    ਜੇ (clipmethod != CLIPMETHOD_PROVIDER)
	ਵਾਪਸ;

    ਜੇ (reg == '+' && plus_pause_ਗਿਣਤੀ < 0)
    {
	ਜੇ (plus_pause_ਗਿਣਤੀ == -1)
	    plus_pause_ਗਿਣਤੀ = 1;
	clip_provider_copy((char_u *)"+", clip_provider);
    }
    ਨਹੀਂਤਾਂ ਜੇ (reg == '*' && ਤਾਰਾ_pause_ਗਿਣਤੀ < 0)
    {
	ਜੇ (ਤਾਰਾ_pause_ਗਿਣਤੀ == -1)
	    ਤਾਰਾ_pause_ਗਿਣਤੀ = 1;
	clip_provider_copy((char_u *)"*", clip_provider);
    }
}

/*
 * Makes it so that the next provider call is only done once any calls after are
 * ignored, until dec_clip_provider is called the same number of times after
 * again. Note that this is per clipboard register ("+", "*")
 */
    ਖਾਲੀ
inc_clip_provider(ਖਾਲੀ)
{
    plus_pause_ਗਿਣਤੀ = (plus_pause_ਗਿਣਤੀ == -2
	|| plus_pause_ਗਿਣਤੀ == -1) ? -1 : plus_pause_ਗਿਣਤੀ + 1;
    ਤਾਰਾ_pause_ਗਿਣਤੀ = (ਤਾਰਾ_pause_ਗਿਣਤੀ == -2
	|| ਤਾਰਾ_pause_ਗਿਣਤੀ == -1) ? -1 : ਤਾਰਾ_pause_ਗਿਣਤੀ + 1;
}

    ਖਾਲੀ
dec_clip_provider(ਖਾਲੀ)
{
    ਜੇ (plus_pause_ਗਿਣਤੀ != -2)
	plus_pause_ਗਿਣਤੀ = plus_pause_ਗਿਣਤੀ == -1 ? -1 : plus_pause_ਗਿਣਤੀ - 1;
    ਜੇ (ਤਾਰਾ_pause_ਗਿਣਤੀ != -2)
	ਤਾਰਾ_pause_ਗਿਣਤੀ = ਤਾਰਾ_pause_ਗਿਣਤੀ == -1 ? -1 : ਤਾਰਾ_pause_ਗਿਣਤੀ - 1;

    ਜੇ (plus_pause_ਗਿਣਤੀ == 0 || plus_pause_ਗਿਣਤੀ == -1)
	plus_pause_ਗਿਣਤੀ = -2;
    ਜੇ (ਤਾਰਾ_pause_ਗਿਣਤੀ == 0 || ਤਾਰਾ_pause_ਗਿਣਤੀ == -1)
	ਤਾਰਾ_pause_ਗਿਣਤੀ = -2;
}

#ਜੇ_ਅੰਤ // FEAT_CLIPBOARD_PROVIDER

#ਜੇ defined(FEAT_WAYLAND_CLIPBOARD) || (defined(FEAT_GUI_GTK) && defined(USE_GTK4))
/*
 * If "vim" is TRUE, then get the motion type. If "vimenc" is TRUE, then get the
 * motion type and also convert "*buf". "buf" and "len_store" will be updated to
 * reflect the actual contents, but should be set beforehand with the initial
 * contents. Note that if "*tofree" is not NULL, use vim_free() on "*buf",
 * otherwise use the free func for which "*buf" was allocated with. In both
 * cases use vim_free() on "*tofree". Returns OK on success and FAIL on failure.
 */
    ਪੂਰਨ
clip_convert_ਅੰਕੜੇ(
	char_u	**buf,
	ਲੰਮਾ	*len_store,
	ਪੂਰਨ	*motion,
	bool	vim,
	bool	vimenc,
	char_u	**tofree)
{
    char_u  *final = *buf;
    char_u  *enc;
    ਲੰਮਾ    len = *len_store;

    ਜੇ (vim && len >= 2)
    {
	*motion = *final++;
	len--;
    }
    ਨਹੀਂਤਾਂ ਜੇ (vimenc && len >= 3)
    {
	vimconv_T   conv;
	ਪੂਰਨ	    convlen;

	// First byte is motion type
	*motion = *final++;
	len--;

	// Get encoding of selection
	enc = final;

	// Skip the encoding type including null terminator in final text
	final = memchr(final, NUL, len);
	ਜੇ (final == ਨਿਰਾ)
	    ਵਾਪਸ FAIL;
	final++; // Skip NUL

	// Subtract pointers to get length of encoding;
	len -= final - enc;

	conv.vc_type = CONV_NONE;
	convert_ਤਿਆਰੀ(&conv, enc, p_enc);
	ਜੇ (conv.vc_type != CONV_NONE)
	{
	   char_u *tmp;

	   convlen = len;
	   tmp = string_convert(&conv, final, &convlen);
	   len = convlen;
	   ਜੇ (tmp != ਨਿਰਾ)
	   {
		final = tmp;
		*tofree = final;
	   }
	   convert_ਤਿਆਰੀ(&conv, ਨਿਰਾ, ਨਿਰਾ);
	}
    }
    *buf = final;
    *len_store = len;
    ਵਾਪਸ OK;
}
#ਜੇ_ਅੰਤ
