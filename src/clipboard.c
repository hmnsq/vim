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

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CYGWIN_WIN32_CLIPBOARD
# define WIN32_LEAN_AND_ಸರಾಸರಿ
# include <windows.h>
# include "winclip.pro"
#ಆದರೆ_ಅಂತ್ಯ

// Functions for copying and pasting text between applications.
// This is always included in a GUI version, but may also be included when the
// clipboard and mouse is available to a terminal version such as xterm.
// Note: there are some more functions in ops.c that handle selection stuff.
//
// Also note that the majority of functions here deal with the X 'primary'
// (visible - for Visual mode use) selection, and only that. There are no
// versions of these for the 'clipboard' selection, as Visual mode has no use
// for them.


#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CLIPBOARD_PROVIDER
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ clip_provider_is_ಲಭ್ಯ(char_u *provider);
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_CLIPBOARD)

# ಆದರೆ defined(FEAT_WAYLAND_CLIPBOARD)

#  include "wayland.h"

// Represents either the regular or primary selection
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    char_u		*contents;	// Non-null if we own selection,
					// contains the data to send to other
					// clients.
    vwl_ದತ್ತಾಂಶ_ಮೂಲ_T	*ಮೂಲ;	// Non-NULL if we own the selection,
					// else NULL if we don't.
    vwl_ದತ್ತಾಂಶ_offer_T	*offer;		// Current offer for the selection

    bool		own_success;	// Used by clip_wl_own_selection()
    bool		ಲಭ್ಯ;	// If selection is ready to serve/use

    // These may point to the same proxy as the other selection
    vwl_ದತ್ತಾಂಶ_device_manager_T	*manager;
    vwl_ದತ್ತಾಂಶ_device_T		*device;
} clip_wl_selection_T;

// Represents the clipboard for the global Wayland connection, for the chosen
// seat (using the 'wl_seat' option)
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    vwl_seat_T *seat;

    clip_wl_selection_T regular;
    clip_wl_selection_T primary;
} clip_wl_T;

// Mime types we support sending and receiving
// Mimes with a lower index in the array are prioritized first when we are
// receiving data.
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *supported_mimes[] = {
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
clip_wl_receive_ದತ್ತಾಂಶ(Clipboard_T *cbd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *mime_type, ಪೂರ್ಣಾಂಕ fd);
ಸ್ಥಿರ ಶೂನ್ಯ clip_wl_request_selection(Clipboard_T *cbd);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ clip_wl_own_selection(Clipboard_T *cbd);
ಸ್ಥಿರ ಶೂನ್ಯ clip_wl_lose_selection(Clipboard_T *cbd);
ಸ್ಥಿರ ಶೂನ್ಯ clip_wl_ಗಣ_selection(Clipboard_T *cbd);

#  ಆದರೆ defined(USE_SYSTEM)
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
clip_init(ಪೂರ್ಣಾಂಕ can_use)
{
    Clipboard_T *cb;

    cb = &clip_ನಕ್ಷತ್ರ;
    ಗಾಗಿ (;;)
    {
	// No need to init again if cbd is already available
	ಆದರೆ (can_use && cb->ಲಭ್ಯ)
	    ಹೋಗು skip;

	cb->ಲಭ್ಯ  = can_use;
	cb->owned      = FALSE;
	cb->ಆರಂಭ.lnum = 0;
	cb->ಆರಂಭ.ಲಂಬಸಾಲು  = 0;
	cb->ಕೊನೆ.lnum   = 0;
	cb->ಕೊನೆ.ಲಂಬಸಾಲು    = 0;
	cb->state      = SELECT_CLEARED;

skip:
	ಆದರೆ (cb == &clip_plus)
	    ನಿಲ್ಲಿಸು;
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
    pos_T	    ಆರಂಭ, ಕೊನೆ;

    // If visual mode is only due to a redo command ("."), then ignore it
    ಆದರೆ (!redo_VIsual_busy && VIsual_ಸಕ್ರಿಯ && (State & ಬಹುಲಕ_ಸಾಮಾನ್ಯ))
    {
	ಆದರೆ (LT_POS(VIsual, curwin->w_cursor))
	{
	    ಆರಂಭ = VIsual;
	    ಕೊನೆ = curwin->w_cursor;
	    ಆದರೆ (has_mbyte)
		ಕೊನೆ.ಲಂಬಸಾಲು += (*mb_ptr2len)(ml_get_cursor()) - 1;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಆರಂಭ = curwin->w_cursor;
	    ಕೊನೆ = VIsual;
	}
	ಆದರೆ (!EQUAL_POS(clip->ಆರಂಭ, ಆರಂಭ)
		|| !EQUAL_POS(clip->ಕೊನೆ, ಕೊನೆ)
		|| clip->vmode != VIsual_ಬಹುಲಕ)
	{
	    clip_clear_selection(clip);
	    clip->ಆರಂಭ = ಆರಂಭ;
	    clip->ಕೊನೆ = ಕೊನೆ;
	    clip->vmode = VIsual_ಬಹುಲಕ;
	    clip_free_selection(clip);
	    clip_own_selection(clip);
	    clip_gen_ಗಣ_selection(clip);
	}
    }
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
clip_gen_own_selection(Clipboard_T *cbd)
{
# ಆದರೆ defined(FEAT_XCLIPBOARD) || defined(FEAT_WAYLAND_CLIPBOARD)
#  ifdef FEAT_GUI
    ಆದರೆ (gui.ಒಳ_use)
	ಹಿಂತಿರುಗಿ clip_mch_own_selection(cbd);
    ಇಲ್ಲದಿದ್ದರೆ
#  endif
    {
	ಆದರೆ (clipmethod == CLIPMETHOD_WAYLAND)
	{
#  ifdef FEAT_WAYLAND_CLIPBOARD
	    ಹಿಂತಿರುಗಿ clip_wl_own_selection(cbd);
#  endif
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (clipmethod == CLIPMETHOD_X11)
	{
#  ifdef FEAT_XCLIPBOARD
	    ಹಿಂತಿರುಗಿ clip_xterm_own_selection(cbd);
#  endif
	}
    }
    ಹಿಂತಿರುಗಿ FAIL;
# ಇಲ್ಲದಿದ್ದರೆ
    ಹಿಂತಿರುಗಿ clip_mch_own_selection(cbd);
# endif
}

    ಶೂನ್ಯ
clip_own_selection(Clipboard_T *cbd)
{
    /*
     * Also want to check somehow that we are reading from the keyboard rather
     * than a mapping etc.
     */
# ಆದರೆ defined(FEAT_X11) || defined(FEAT_WAYLAND_CLIPBOARD)
    // Always own the selection, we might have lost it without being
    // notified, e.g. during a ":sh" command.
    ಆದರೆ (cbd->ಲಭ್ಯ)
    {
	ಪೂರ್ಣಾಂಕ was_owned = cbd->owned;

	cbd->owned = (clip_gen_own_selection(cbd) == OK);
	ಆದರೆ (!was_owned && (cbd == &clip_ನಕ್ಷತ್ರ || cbd == &clip_plus))
	{
	    // May have to show a different kind of highlighting for the
	    // selected area.  There is no specific redraw command for this,
	    // just redraw all windows on the current buffer.
	    ಆದರೆ (cbd->owned
		    && (get_real_state() == ಬಹುಲಕ_VISUAL
					    || get_real_state() == ಬಹುಲಕ_SELECT)
		    && (cbd == &clip_ನಕ್ಷತ್ರ ? clip_isautosel_ನಕ್ಷತ್ರ()
						      : clip_isautosel_plus())
		    && HL_ATTR(HLF_V) != HL_ATTR(HLF_VNC))
		redraw_curbuf_later(UPD_INVERTED_ALL);
	}
    }
# ಇಲ್ಲದಿದ್ದರೆ
    // Only own the clipboard when we didn't own it yet.
    ಆದರೆ (!cbd->owned && cbd->ಲಭ್ಯ)
	cbd->owned = (clip_gen_own_selection(cbd) == OK);
# endif
}

    ಸ್ಥಿರ ಶೂನ್ಯ
clip_gen_lose_selection(Clipboard_T *cbd)
{
# ಆದರೆ defined(FEAT_XCLIPBOARD) || defined(FEAT_WAYLAND_CLIPBOARD)
#  ifdef FEAT_GUI
    ಆದರೆ (gui.ಒಳ_use)
	clip_mch_lose_selection(cbd);
    ಇಲ್ಲದಿದ್ದರೆ
#  endif
    {
	ಆದರೆ (clipmethod == CLIPMETHOD_WAYLAND)
	{
#  ifdef FEAT_WAYLAND_CLIPBOARD
	    clip_wl_lose_selection(cbd);
#  endif
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (clipmethod == CLIPMETHOD_X11)
	{
#  ifdef FEAT_XCLIPBOARD
	    clip_xterm_lose_selection(cbd);
#  endif
	}
    }
# ಇಲ್ಲದಿದ್ದರೆ
    clip_mch_lose_selection(cbd);
# endif
}

    ಶೂನ್ಯ
clip_lose_selection(Clipboard_T *cbd)
{
# ifdef FEAT_X11
    ಪೂರ್ಣಾಂಕ	    was_owned = cbd->owned;
# endif
    ಪೂರ್ಣಾಂಕ     visual_selection = FALSE;

    ಆದರೆ (cbd == &clip_ನಕ್ಷತ್ರ || cbd == &clip_plus)
	visual_selection = TRUE;

    clip_free_selection(cbd);
    cbd->owned = FALSE;
    ಆದರೆ (visual_selection)
	clip_clear_selection(cbd);
    clip_gen_lose_selection(cbd);
# ifdef FEAT_X11
    ಆದರೆ (visual_selection)
    {
	// May have to show a different kind of highlighting for the selected
	// area.  There is no specific redraw command for this, just redraw all
	// windows on the current buffer.
	ಆದರೆ (was_owned
		&& (get_real_state() == ಬಹುಲಕ_VISUAL
		    || get_real_state() == ಬಹುಲಕ_SELECT)
		&& (cbd == &clip_ನಕ್ಷತ್ರ ?
		    clip_isautosel_ನಕ್ಷತ್ರ() : clip_isautosel_plus())
		&& HL_ATTR(HLF_V) != HL_ATTR(HLF_VNC)
		&& !exiting)
	{
	    update_curbuf(UPD_INVERTED_ALL);
	    setcursor();
	    cursor_on();
	    ಹೊರಹೋಗು_flush_cursor(TRUE, FALSE);
	}
    }
# endif
}

    ಸ್ಥಿರ ಶೂನ್ಯ
clip_copy_selection(Clipboard_T *clip)
{
    ಆದರೆ (VIsual_ಸಕ್ರಿಯ && (State & ಬಹುಲಕ_ಸಾಮಾನ್ಯ) && clip->ಲಭ್ಯ)
    {
	clip_update_selection(clip);
	clip_free_selection(clip);
	clip_own_selection(clip);
	ಆದರೆ (clip->owned)
	    clip_get_selection(clip);
	clip_gen_ಗಣ_selection(clip);
    }
}

/*
 * Save and restore clip_unnamed before doing possibly many changes. This
 * prevents accessing the clipboard very often which might slow down Vim
 * considerably.
 */
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ global_change_ಎಣಿಕೆ = 0; // if set, inside a start_global_changes
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ clipboard_needs_update = FALSE; // clipboard needs to be updated
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ clip_did_ಗಣ_selection = TRUE;

/*
 * Save clip_unnamed and reset it.
 */
    ಶೂನ್ಯ
ಆರಂಭ_global_changes(ಶೂನ್ಯ)
{
    ಆದರೆ (++global_change_ಎಣಿಕೆ > 1)
	ಹಿಂತಿರುಗಿ;
    clip_unnamed_saved = clip_unnamed;
    clipboard_needs_update = FALSE;

    ಆದರೆ (clip_did_ಗಣ_selection)
    {
	clip_unnamed = 0;
	clip_did_ಗಣ_selection = FALSE;
    }
}

/*
 * Return TRUE if setting the clipboard was postponed, it already contains the
 * right text.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
is_clipboard_needs_update(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ clipboard_needs_update;
}

/*
 * Restore clip_unnamed and set the selection when needed.
 */
    ಶೂನ್ಯ
ಕೊನೆ_global_changes(ಶೂನ್ಯ)
{
    ಆದರೆ (--global_change_ಎಣಿಕೆ > 0)
	// recursive
	ಹಿಂತಿರುಗಿ;
    ಆದರೆ (!clip_did_ಗಣ_selection)
    {
	clip_did_ಗಣ_selection = TRUE;
	clip_unnamed = clip_unnamed_saved;
	clip_unnamed_saved = 0;
	ಆದರೆ (clipboard_needs_update)
	{
	    // only store something in the clipboard,
	    // if we have yanked anything to it
	    ಆದರೆ (clip_unnamed & CLIP_UNNAMED)
	    {
		clip_own_selection(&clip_ನಕ್ಷತ್ರ);
		clip_gen_ಗಣ_selection(&clip_ನಕ್ಷತ್ರ);
	    }
	    ಆದರೆ (clip_unnamed & CLIP_UNNAMED_PLUS)
	    {
		clip_own_selection(&clip_plus);
		clip_gen_ಗಣ_selection(&clip_plus);
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
    ಆದರೆ (clip_isautosel_ನಕ್ಷತ್ರ())
	clip_copy_selection(&clip_ನಕ್ಷತ್ರ);
    ಆದರೆ (clip_isautosel_plus())
	clip_copy_selection(&clip_plus);
}

/*
 * Return TRUE if automatic selection of Visual area is desired for the *
 * register.
 */
    ಪೂರ್ಣಾಂಕ
clip_isautosel_ನಕ್ಷತ್ರ(ಶೂನ್ಯ)
{
# ifdef FEAT_CLIPBOARD_PROVIDER
    ಆದರೆ (clipmethod == CLIPMETHOD_PROVIDER)
	ಹಿಂತಿರುಗಿ false;
# endif
# ifdef FEAT_GUI
    ಆದರೆ (gui.ಒಳ_use)
	ಹಿಂತಿರುಗಿ vim_strchr(p_go, GO_ASEL) != ಖಾಲಿ
	    && vim_strchr(p_go, GO_ASELPLUS) == ಖಾಲಿ;
# endif
    ಹಿಂತಿರುಗಿ clip_autoselect_ನಕ್ಷತ್ರ;
}

/*
 * Return TRUE if automatic selection of Visual area is desired for the +
 * register.
 */
    ಪೂರ್ಣಾಂಕ
clip_isautosel_plus(ಶೂನ್ಯ)
{
# ifdef FEAT_CLIPBOARD_PROVIDER
    ಆದರೆ (clipmethod == CLIPMETHOD_PROVIDER)
	ಹಿಂತಿರುಗಿ false;
# endif
# ifdef FEAT_GUI
    ಆದರೆ (gui.ಒಳ_use)
	ಹಿಂತಿರುಗಿ vim_strchr(p_go, GO_ASELPLUS) != ಖಾಲಿ;
# endif
    ಹಿಂತಿರುಗಿ clip_autoselect_plus;
}


/*
 * Stuff for general mouse selection, without using Visual mode.
 */

/*
 * Compare two screen positions ala strcmp()
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
clip_compare_pos(
    ಪೂರ್ಣಾಂಕ		row1,
    ಪೂರ್ಣಾಂಕ		col1,
    ಪೂರ್ಣಾಂಕ		row2,
    ಪೂರ್ಣಾಂಕ		col2)
{
    ಆದರೆ (row1 > row2) ಹಿಂತಿರುಗಿ(1);
    ಆದರೆ (row1 < row2) ಹಿಂತಿರುಗಿ(-1);
    ಆದರೆ (col1 > col2) ಹಿಂತಿರುಗಿ(1);
    ಆದರೆ (col1 < col2) ಹಿಂತಿರುಗಿ(-1);
    ಹಿಂತಿರುಗಿ(0);
}

// "how" flags for clip_invert_area()
# define CLIP_CLEAR	1
# define CLIP_ಗಣ	2
# define CLIP_TOGGLE	3

/*
 * Invert or un-invert a rectangle of the screen.
 * "invert" is true if the result is inverted.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
clip_invert_rectangle(
	Clipboard_T	*cbd ಬಳಸದ,
	ಪೂರ್ಣಾಂಕ		ಅಡ್ಡಸಾಲು_arg,
	ಪೂರ್ಣಾಂಕ		ಲಂಬಸಾಲು_arg,
	ಪೂರ್ಣಾಂಕ		ಎತ್ತರ_arg,
	ಪೂರ್ಣಾಂಕ		ಅಗಲ_arg,
	ಪೂರ್ಣಾಂಕ		invert)
{
    ಪೂರ್ಣಾಂಕ		ಅಡ್ಡಸಾಲು = ಅಡ್ಡಸಾಲು_arg;
    ಪೂರ್ಣಾಂಕ		ಲಂಬಸಾಲು = ಲಂಬಸಾಲು_arg;
    ಪೂರ್ಣಾಂಕ		ಎತ್ತರ = ಎತ್ತರ_arg;
    ಪೂರ್ಣಾಂಕ		ಅಗಲ = ಅಗಲ_arg;

# ifdef FEAT_PROP_POPUP
    // this goes on top of all popup windows
    screen_zindex = CLIP_ZINDEX;

    ಆದರೆ (ಲಂಬಸಾಲು < cbd->min_ಲಂಬಸಾಲು)
    {
	ಅಗಲ -= cbd->min_ಲಂಬಸಾಲು - ಲಂಬಸಾಲು;
	ಲಂಬಸಾಲು = cbd->min_ಲಂಬಸಾಲು;
    }
    ಆದರೆ (ಅಗಲ > cbd->max_ಲಂಬಸಾಲು - ಲಂಬಸಾಲು)
	ಅಗಲ = cbd->max_ಲಂಬಸಾಲು - ಲಂಬಸಾಲು;
    ಆದರೆ (ಅಡ್ಡಸಾಲು < cbd->min_ಅಡ್ಡಸಾಲು)
    {
	ಎತ್ತರ -= cbd->min_ಅಡ್ಡಸಾಲು - ಅಡ್ಡಸಾಲು;
	ಅಡ್ಡಸಾಲು = cbd->min_ಅಡ್ಡಸಾಲು;
    }
    ಆದರೆ (ಎತ್ತರ > cbd->max_ಅಡ್ಡಸಾಲು - ಅಡ್ಡಸಾಲು + 1)
	ಎತ್ತರ = cbd->max_ಅಡ್ಡಸಾಲು - ಅಡ್ಡಸಾಲು + 1;
# endif
# ifdef FEAT_GUI
    ಆದರೆ (gui.ಒಳ_use)
	gui_mch_invert_rectangle(ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು, ಎತ್ತರ, ಅಗಲ);
    ಇಲ್ಲದಿದ್ದರೆ
# endif
	screen_draw_rectangle(ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು, ಎತ್ತರ, ಅಗಲ, invert, FALSE);
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
	ಪೂರ್ಣಾಂಕ		row1,
	ಪೂರ್ಣಾಂಕ		col1,
	ಪೂರ್ಣಾಂಕ		row2,
	ಪೂರ್ಣಾಂಕ		col2,
	ಪೂರ್ಣಾಂಕ		how)
{
    ಪೂರ್ಣಾಂಕ		invert = FALSE;
    ಪೂರ್ಣಾಂಕ		max_ಲಂಬಸಾಲು;

# ifdef FEAT_PROP_POPUP
    max_ಲಂಬಸಾಲು = cbd->max_ಲಂಬಸಾಲು - 1;
# ಇಲ್ಲದಿದ್ದರೆ
    max_ಲಂಬಸಾಲು = Columns - 1;
# endif

    ಆದರೆ (how == CLIP_ಗಣ)
	invert = TRUE;

    // Swap the from and to positions so the from is always before
    ಆದರೆ (clip_compare_pos(row1, col1, row2, col2) > 0)
    {
	ಪೂರ್ಣಾಂಕ tmp_ಅಡ್ಡಸಾಲು, tmp_ಲಂಬಸಾಲು;

	tmp_ಅಡ್ಡಸಾಲು = row1;
	tmp_ಲಂಬಸಾಲು = col1;
	row1	= row2;
	col1	= col2;
	row2	= tmp_ಅಡ್ಡಸಾಲು;
	col2	= tmp_ಲಂಬಸಾಲು;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (how == CLIP_TOGGLE)
	invert = TRUE;

    // If all on the same line, do it the easy way
    ಆದರೆ (row1 == row2)
    {
	clip_invert_rectangle(cbd, row1, col1, 1, col2 - col1, invert);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// Handle a piece of the first line
	ಆದರೆ (col1 > 0)
	{
	    clip_invert_rectangle(cbd, row1, col1, 1,
						  (ಪೂರ್ಣಾಂಕ)Columns - col1, invert);
	    row1++;
	}

	// Handle a piece of the last line
	ಆದರೆ (col2 < max_ಲಂಬಸಾಲು)
	{
	    clip_invert_rectangle(cbd, row2, 0, 1, col2, invert);
	    row2--;
	}

	// Handle the rectangle that's left
	ಆದರೆ (row2 >= row1)
	    clip_invert_rectangle(cbd, row1, 0, row2 - row1 + 1,
							 (ಪೂರ್ಣಾಂಕ)Columns, invert);
    }
}

/*
 * Start, continue or end a modeless selection.  Used when editing the
 * command-line, in the cmdline window and when the mouse is in a popup window.
 */
    ಶೂನ್ಯ
clip_modeless(ಪೂರ್ಣಾಂಕ button, ಪೂರ್ಣಾಂಕ is_click, ಪೂರ್ಣಾಂಕ is_drag)
{
    ಪೂರ್ಣಾಂಕ		repeat;

    repeat = ((clip_ನಕ್ಷತ್ರ.ಬಹುಲಕ == SELECT_ಬಹುಲಕ_CHAR
		|| clip_ನಕ್ಷತ್ರ.ಬಹುಲಕ == SELECT_ಬಹುಲಕ_ಸಾಲು)
					      && (mod_ಮುಖವಾಡ & MOD_ಮುಖವಾಡ_2_CLICK))
	    || (clip_ನಕ್ಷತ್ರ.ಬಹುಲಕ == SELECT_ಬಹುಲಕ_ಶಬ್ದ
					     && (mod_ಮುಖವಾಡ & MOD_ಮುಖವಾಡ_3_CLICK));
    ಆದರೆ (is_click && button == MOUSE_ಬಲ)
    {
	// Right mouse button: If there was no selection, start one.
	// Otherwise extend the existing selection.
	ಆದರೆ (clip_ನಕ್ಷತ್ರ.state == SELECT_CLEARED)
	    clip_ಆರಂಭ_selection(mouse_ಲಂಬಸಾಲು, mouse_ಅಡ್ಡಸಾಲು, FALSE);
	clip_process_selection(button, mouse_ಲಂಬಸಾಲು, mouse_ಅಡ್ಡಸಾಲು, repeat);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (is_click)
	clip_ಆರಂಭ_selection(mouse_ಲಂಬಸಾಲು, mouse_ಅಡ್ಡಸಾಲು, repeat);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (is_drag)
    {
	// Don't try extending a selection if there isn't one.  Happens when
	// button-down is in the cmdline and them moving mouse upwards.
	ಆದರೆ (clip_ನಕ್ಷತ್ರ.state != SELECT_CLEARED)
	    clip_process_selection(button, mouse_ಲಂಬಸಾಲು, mouse_ಅಡ್ಡಸಾಲು, repeat);
    }
    ಇಲ್ಲದಿದ್ದರೆ // release
	clip_process_selection(MOUSE_RELEASE, mouse_ಲಂಬಸಾಲು, mouse_ಅಡ್ಡಸಾಲು, FALSE);
}

/*
 * Update the currently selected region by adding and/or subtracting from the
 * beginning or end and inverting the changed area(s).
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
clip_update_modeless_selection(
    Clipboard_T    *cb,
    ಪೂರ್ಣಾಂಕ		    row1,
    ಪೂರ್ಣಾಂಕ		    col1,
    ಪೂರ್ಣಾಂಕ		    row2,
    ಪೂರ್ಣಾಂಕ		    col2)
{
    // See if we changed at the beginning of the selection
    ಆದರೆ (row1 != cb->ಆರಂಭ.lnum || col1 != (ಪೂರ್ಣಾಂಕ)cb->ಆರಂಭ.ಲಂಬಸಾಲು)
    {
	clip_invert_area(cb, row1, col1, (ಪೂರ್ಣಾಂಕ)cb->ಆರಂಭ.lnum, cb->ಆರಂಭ.ಲಂಬಸಾಲು,
								 CLIP_TOGGLE);
	cb->ಆರಂಭ.lnum = row1;
	cb->ಆರಂಭ.ಲಂಬಸಾಲು  = col1;
    }

    // See if we changed at the end of the selection
    ಆದರೆ (row2 != cb->ಕೊನೆ.lnum || col2 != (ಪೂರ್ಣಾಂಕ)cb->ಕೊನೆ.ಲಂಬಸಾಲು)
    {
	clip_invert_area(cb, (ಪೂರ್ಣಾಂಕ)cb->ಕೊನೆ.lnum, cb->ಕೊನೆ.ಲಂಬಸಾಲು, row2, col2,
								 CLIP_TOGGLE);
	cb->ಕೊನೆ.lnum = row2;
	cb->ಕೊನೆ.ಲಂಬಸಾಲು  = col2;
    }
}

/*
 * Find the starting and ending positions of the word at the given row and
 * column.  Only white-separated words are recognized here.
 */
# define CHAR_CLASS(c)	(c <= ' ' ? ' ' : vim_iswordc(c))

    ಸ್ಥಿರ ಶೂನ್ಯ
clip_get_ಶಬ್ದ_boundaries(Clipboard_T *cb, ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲು, ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು)
{
    ಪೂರ್ಣಾಂಕ		ಆರಂಭ_class;
    ಪೂರ್ಣಾಂಕ		ತಾತ್ಕಾಲಿಕ_ಲಂಬಸಾಲು;
    char_u	*p;
    ಪೂರ್ಣಾಂಕ		mboff;

    ಆದರೆ (ಅಡ್ಡಸಾಲು >= screen_ಅಡ್ಡಸಾಲುಗಳು || ಲಂಬಸಾಲು >= screen_Columns || Screen_ಸಾಲುಗಳು == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    p = Screen_ಸಾಲುಗಳು + ಸಾಲು_ಸ್ಥಾನಾಂತರ[ಅಡ್ಡಸಾಲು];
    // Correct for starting in the right half of a double-wide char
    ಆದರೆ (enc_dbcs != 0)
	ಲಂಬಸಾಲು -= dbcs_screen_ತಲೆ_off(p, p + ಲಂಬಸಾಲು);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (enc_utf8 && p[ಲಂಬಸಾಲು] == 0)
	--ಲಂಬಸಾಲು;
    ಆರಂಭ_class = CHAR_CLASS(p[ಲಂಬಸಾಲು]);

    ತಾತ್ಕಾಲಿಕ_ಲಂಬಸಾಲು = ಲಂಬಸಾಲು;
    ಗಾಗಿ ( ; ತಾತ್ಕಾಲಿಕ_ಲಂಬಸಾಲು > 0; ತಾತ್ಕಾಲಿಕ_ಲಂಬಸಾಲು--)
	ಆದರೆ (enc_dbcs != 0
		   && (mboff = dbcs_screen_ತಲೆ_off(p, p + ತಾತ್ಕಾಲಿಕ_ಲಂಬಸಾಲು - 1)) > 0)
	    ತಾತ್ಕಾಲಿಕ_ಲಂಬಸಾಲು -= mboff;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (CHAR_CLASS(p[ತಾತ್ಕಾಲಿಕ_ಲಂಬಸಾಲು - 1]) != ಆರಂಭ_class
		&& !(enc_utf8 && p[ತಾತ್ಕಾಲಿಕ_ಲಂಬಸಾಲು - 1] == 0))
	    ನಿಲ್ಲಿಸು;
    cb->ಶಬ್ದ_ಆರಂಭ_ಲಂಬಸಾಲು = ತಾತ್ಕಾಲಿಕ_ಲಂಬಸಾಲು;

    ತಾತ್ಕಾಲಿಕ_ಲಂಬಸಾಲು = ಲಂಬಸಾಲು;
    ಗಾಗಿ ( ; ತಾತ್ಕಾಲಿಕ_ಲಂಬಸಾಲು < screen_Columns; ತಾತ್ಕಾಲಿಕ_ಲಂಬಸಾಲು++)
	ಆದರೆ (enc_dbcs != 0 && dbcs_ptr2cells(p + ತಾತ್ಕಾಲಿಕ_ಲಂಬಸಾಲು) == 2)
	    ++ತಾತ್ಕಾಲಿಕ_ಲಂಬಸಾಲು;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (CHAR_CLASS(p[ತಾತ್ಕಾಲಿಕ_ಲಂಬಸಾಲು]) != ಆರಂಭ_class
		&& !(enc_utf8 && p[ತಾತ್ಕಾಲಿಕ_ಲಂಬಸಾಲು] == 0))
	    ನಿಲ್ಲಿಸು;
    cb->ಶಬ್ದ_ಕೊನೆ_ಲಂಬಸಾಲು = ತಾತ್ಕಾಲಿಕ_ಲಂಬಸಾಲು;
}

/*
 * Find the column position for the last non-whitespace character on the given
 * line at or before start_col.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
clip_get_ಸಾಲು_ಕೊನೆ(Clipboard_T *cbd ಬಳಸದ, ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲು)
{
    ಪೂರ್ಣಾಂಕ	    i;

    ಆದರೆ (ಅಡ್ಡಸಾಲು >= screen_ಅಡ್ಡಸಾಲುಗಳು || Screen_ಸಾಲುಗಳು == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ 0;
    ಗಾಗಿ (i =
# ifdef FEAT_PROP_POPUP
	    cbd->max_ಲಂಬಸಾಲು;
# ಇಲ್ಲದಿದ್ದರೆ
	    screen_Columns;
# endif
			    i > 0; i--)
	ಆದರೆ (Screen_ಸಾಲುಗಳು[ಸಾಲು_ಸ್ಥಾನಾಂತರ[ಅಡ್ಡಸಾಲು] + i - 1] != ' ')
	    ನಿಲ್ಲಿಸು;
    ಹಿಂತಿರುಗಿ i;
}

/*
 * Start the selection
 */
    ಶೂನ್ಯ
clip_ಆರಂಭ_selection(ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು, ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲು, ಪೂರ್ಣಾಂಕ repeated_click)
{
    Clipboard_T	*cb = &clip_ನಕ್ಷತ್ರ;
# ifdef FEAT_PROP_POPUP
    win_T	*wp;
    ಪೂರ್ಣಾಂಕ		ಅಡ್ಡಸಾಲು_cp = ಅಡ್ಡಸಾಲು;
    ಪೂರ್ಣಾಂಕ		ಲಂಬಸಾಲು_cp = ಲಂಬಸಾಲು;

    wp = mouse_ಕಂಡುಹಿಡಿ_win(&ಅಡ್ಡಸಾಲು_cp, &ಲಂಬಸಾಲು_cp, ಕಂಡುಹಿಡಿ_POPUP);
    ಆದರೆ (wp != ಖಾಲಿ && WIN_IS_POPUP(wp)
				  && popup_is_ಒಳ_scrollbar(wp, ಅಡ್ಡಸಾಲು_cp, ಲಂಬಸಾಲು_cp))
	// click or double click in scrollbar does not start a selection
	ಹಿಂತಿರುಗಿ;
# endif

    ಆದರೆ (cb->state == SELECT_ಮುಗಿದ)
	clip_clear_selection(cb);

    ಅಡ್ಡಸಾಲು = check_ಅಡ್ಡಸಾಲು(ಅಡ್ಡಸಾಲು);
    ಲಂಬಸಾಲು = check_ಲಂಬಸಾಲು(ಲಂಬಸಾಲು);
    ಲಂಬಸಾಲು = mb_fix_ಲಂಬಸಾಲು(ಲಂಬಸಾಲು, ಅಡ್ಡಸಾಲು);

    cb->ಆರಂಭ.lnum  = ಅಡ್ಡಸಾಲು;
    cb->ಆರಂಭ.ಲಂಬಸಾಲು   = ಲಂಬಸಾಲು;
    cb->ಕೊನೆ	    = cb->ಆರಂಭ;
    cb->origin_ಅಡ್ಡಸಾಲು  = (short_u)cb->ಆರಂಭ.lnum;
    cb->state	    = SELECT_ಒಳ_ಪ್ರಗತಿ;
# ifdef FEAT_PROP_POPUP
    ಆದರೆ (wp != ಖಾಲಿ && WIN_IS_POPUP(wp))
    {
	// Click in a popup window restricts selection to that window,
	// excluding the border.
	cb->min_ಲಂಬಸಾಲು = wp->w_wincol + wp->w_popup_border[3];
	cb->max_ಲಂಬಸಾಲು = wp->w_wincol + popup_ಅಗಲ(wp)
				 - wp->w_popup_border[1] - wp->w_has_scrollbar;
	ಆದರೆ (cb->max_ಲಂಬಸಾಲು > screen_Columns)
	    cb->max_ಲಂಬಸಾಲು = screen_Columns;
	cb->min_ಅಡ್ಡಸಾಲು = wp->w_winrow + wp->w_popup_border[0];
	cb->max_ಅಡ್ಡಸಾಲು = wp->w_winrow + popup_ಎತ್ತರ(wp) - 1
						   - wp->w_popup_border[2];
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	cb->min_ಲಂಬಸಾಲು = 0;
	cb->max_ಲಂಬಸಾಲು = screen_Columns;
	cb->min_ಅಡ್ಡಸಾಲು = 0;
	cb->max_ಅಡ್ಡಸಾಲು = screen_ಅಡ್ಡಸಾಲುಗಳು;
    }
# endif

    ಆದರೆ (repeated_click)
    {
	ಆದರೆ (++cb->ಬಹುಲಕ > SELECT_ಬಹುಲಕ_ಸಾಲು)
	    cb->ಬಹುಲಕ = SELECT_ಬಹುಲಕ_CHAR;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	cb->ಬಹುಲಕ = SELECT_ಬಹುಲಕ_CHAR;

# ifdef FEAT_GUI
    // clear the cursor until the selection is made
    ಆದರೆ (gui.ಒಳ_use)
	gui_undraw_cursor();
# endif

    ಆಯ್ಕೆ (cb->ಬಹುಲಕ)
    {
	ಸ್ಥಿತಿ SELECT_ಬಹುಲಕ_CHAR:
	    cb->origin_ಆರಂಭ_ಲಂಬಸಾಲು = cb->ಆರಂಭ.ಲಂಬಸಾಲು;
	    cb->ಶಬ್ದ_ಕೊನೆ_ಲಂಬಸಾಲು = clip_get_ಸಾಲು_ಕೊನೆ(cb, (ಪೂರ್ಣಾಂಕ)cb->ಆರಂಭ.lnum);
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ SELECT_ಬಹುಲಕ_ಶಬ್ದ:
	    clip_get_ಶಬ್ದ_boundaries(cb, (ಪೂರ್ಣಾಂಕ)cb->ಆರಂಭ.lnum, cb->ಆರಂಭ.ಲಂಬಸಾಲು);
	    cb->origin_ಆರಂಭ_ಲಂಬಸಾಲು = cb->ಶಬ್ದ_ಆರಂಭ_ಲಂಬಸಾಲು;
	    cb->origin_ಕೊನೆ_ಲಂಬಸಾಲು	 = cb->ಶಬ್ದ_ಕೊನೆ_ಲಂಬಸಾಲು;

	    clip_invert_area(cb, (ಪೂರ್ಣಾಂಕ)cb->ಆರಂಭ.lnum, cb->ಶಬ್ದ_ಆರಂಭ_ಲಂಬಸಾಲು,
			    (ಪೂರ್ಣಾಂಕ)cb->ಕೊನೆ.lnum, cb->ಶಬ್ದ_ಕೊನೆ_ಲಂಬಸಾಲು, CLIP_ಗಣ);
	    cb->ಆರಂಭ.ಲಂಬಸಾಲು = cb->ಶಬ್ದ_ಆರಂಭ_ಲಂಬಸಾಲು;
	    cb->ಕೊನೆ.ಲಂಬಸಾಲು   = cb->ಶಬ್ದ_ಕೊನೆ_ಲಂಬಸಾಲು;
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ SELECT_ಬಹುಲಕ_ಸಾಲು:
	    clip_invert_area(cb, (ಪೂರ್ಣಾಂಕ)cb->ಆರಂಭ.lnum, 0, (ಪೂರ್ಣಾಂಕ)cb->ಆರಂಭ.lnum,
			    (ಪೂರ್ಣಾಂಕ)Columns, CLIP_ಗಣ);
	    cb->ಆರಂಭ.ಲಂಬಸಾಲು = 0;
	    cb->ಕೊನೆ.ಲಂಬಸಾಲು   = Columns;
	    ನಿಲ್ಲಿಸು;
    }

    cb->prev = cb->ಆರಂಭ;

# ifdef DEBUG_SELECTION
    ಬರೆ("Selection started at (%ld,%d)\n", cb->ಆರಂಭ.lnum, cb->ಆರಂಭ.ಲಂಬಸಾಲು);
# endif
}

/*
 * Continue processing the selection
 */
    ಶೂನ್ಯ
clip_process_selection(
    ಪೂರ್ಣಾಂಕ		button,
    ಪೂರ್ಣಾಂಕ		ಲಂಬಸಾಲು,
    ಪೂರ್ಣಾಂಕ		ಅಡ್ಡಸಾಲು,
    int_u	repeated_click)
{
    Clipboard_T	*cb = &clip_ನಕ್ಷತ್ರ;
    ಪೂರ್ಣಾಂಕ		diff;
    ಪೂರ್ಣಾಂಕ		slen = 1;	// cursor shape width

    ಆದರೆ (button == MOUSE_RELEASE)
    {
	ಆದರೆ (cb->state != SELECT_ಒಳ_ಪ್ರಗತಿ)
	    ಹಿಂತಿರುಗಿ;

	// Check to make sure we have something selected
	ಆದರೆ (cb->ಆರಂಭ.lnum == cb->ಕೊನೆ.lnum && cb->ಆರಂಭ.ಲಂಬಸಾಲು == cb->ಕೊನೆ.ಲಂಬಸಾಲು)
	{
# ifdef FEAT_GUI
	    ಆದರೆ (gui.ಒಳ_use)
		gui_update_cursor(FALSE, FALSE);
# endif
	    cb->state = SELECT_CLEARED;
	    ಹಿಂತಿರುಗಿ;
	}

# ifdef DEBUG_SELECTION
	ಬರೆ("Selection ended: (%ld,%d) to (%ld,%d)\n", cb->ಆರಂಭ.lnum,
		cb->ಆರಂಭ.ಲಂಬಸಾಲು, cb->ಕೊನೆ.lnum, cb->ಕೊನೆ.ಲಂಬಸಾಲು);
# endif
	ಆದರೆ (clip_isautosel_ನಕ್ಷತ್ರ() || clip_isautosel_plus()
		|| (
# ifdef FEAT_GUI
		    gui.ಒಳ_use ? (vim_strchr(p_go, GO_ASELML) != ಖಾಲಿ) :
# endif
		    clip_autoselectml))
	    clip_copy_modeless_selection(FALSE);
# ifdef FEAT_GUI
	ಆದರೆ (gui.ಒಳ_use)
	    gui_update_cursor(FALSE, FALSE);
# endif

	cb->state = SELECT_ಮುಗಿದ;
	ಹಿಂತಿರುಗಿ;
    }

    ಅಡ್ಡಸಾಲು = check_ಅಡ್ಡಸಾಲು(ಅಡ್ಡಸಾಲು);
    ಲಂಬಸಾಲು = check_ಲಂಬಸಾಲು(ಲಂಬಸಾಲು);
    ಲಂಬಸಾಲು = mb_fix_ಲಂಬಸಾಲು(ಲಂಬಸಾಲು, ಅಡ್ಡಸಾಲು);

    ಆದರೆ (ಲಂಬಸಾಲು == (ಪೂರ್ಣಾಂಕ)cb->prev.ಲಂಬಸಾಲು && ಅಡ್ಡಸಾಲು == cb->prev.lnum && !repeated_click)
	ಹಿಂತಿರುಗಿ;

    /*
     * When extending the selection with the right mouse button, swap the
     * start and end if the position is before half the selection
     */
    ಆದರೆ (cb->state == SELECT_ಮುಗಿದ && button == MOUSE_ಬಲ)
    {
	/*
	 * If the click is before the start, or the click is inside the
	 * selection and the start is the closest side, set the origin to the
	 * end of the selection.
	 */
	ಆದರೆ (clip_compare_pos(ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು, (ಪೂರ್ಣಾಂಕ)cb->ಆರಂಭ.lnum, cb->ಆರಂಭ.ಲಂಬಸಾಲು) < 0
		|| (clip_compare_pos(ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು,
					   (ಪೂರ್ಣಾಂಕ)cb->ಕೊನೆ.lnum, cb->ಕೊನೆ.ಲಂಬಸಾಲು) < 0
		    && (((cb->ಆರಂಭ.lnum == cb->ಕೊನೆ.lnum
			    && cb->ಕೊನೆ.ಲಂಬಸಾಲು - ಲಂಬಸಾಲು > ಲಂಬಸಾಲು - cb->ಆರಂಭ.ಲಂಬಸಾಲು))
			|| ((diff = (cb->ಕೊನೆ.lnum - ಅಡ್ಡಸಾಲು) -
						   (ಅಡ್ಡಸಾಲು - cb->ಆರಂಭ.lnum)) > 0
			    || (diff == 0 && ಲಂಬಸಾಲು < (ಪೂರ್ಣಾಂಕ)(cb->ಆರಂಭ.ಲಂಬಸಾಲು +
							 cb->ಕೊನೆ.ಲಂಬಸಾಲು) / 2)))))
	{
	    cb->origin_ಅಡ್ಡಸಾಲು = (short_u)cb->ಕೊನೆ.lnum;
	    cb->origin_ಆರಂಭ_ಲಂಬಸಾಲು = cb->ಕೊನೆ.ಲಂಬಸಾಲು - 1;
	    cb->origin_ಕೊನೆ_ಲಂಬಸಾಲು = cb->ಕೊನೆ.ಲಂಬಸಾಲು;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    cb->origin_ಅಡ್ಡಸಾಲು = (short_u)cb->ಆರಂಭ.lnum;
	    cb->origin_ಆರಂಭ_ಲಂಬಸಾಲು = cb->ಆರಂಭ.ಲಂಬಸಾಲು;
	    cb->origin_ಕೊನೆ_ಲಂಬಸಾಲು = cb->ಆರಂಭ.ಲಂಬಸಾಲು;
	}
	ಆದರೆ (cb->ಬಹುಲಕ == SELECT_ಬಹುಲಕ_ಶಬ್ದ && !repeated_click)
	    cb->ಬಹುಲಕ = SELECT_ಬಹುಲಕ_CHAR;
    }

    // set state, for when using the right mouse button
    cb->state = SELECT_ಒಳ_ಪ್ರಗತಿ;

# ifdef DEBUG_SELECTION
    ಬರೆ("Selection extending to (%d,%d)\n", ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು);
# endif

    ಆದರೆ (repeated_click && ++cb->ಬಹುಲಕ > SELECT_ಬಹುಲಕ_ಸಾಲು)
	cb->ಬಹುಲಕ = SELECT_ಬಹುಲಕ_CHAR;

    ಆಯ್ಕೆ (cb->ಬಹುಲಕ)
    {
	ಸ್ಥಿತಿ SELECT_ಬಹುಲಕ_CHAR:
	    // If we're on a different line, find where the line ends
	    ಆದರೆ (ಅಡ್ಡಸಾಲು != cb->prev.lnum)
		cb->ಶಬ್ದ_ಕೊನೆ_ಲಂಬಸಾಲು = clip_get_ಸಾಲು_ಕೊನೆ(cb, ಅಡ್ಡಸಾಲು);

	    // See if we are before or after the origin of the selection
	    ಆದರೆ (clip_compare_pos(ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು, cb->origin_ಅಡ್ಡಸಾಲು,
						   cb->origin_ಆರಂಭ_ಲಂಬಸಾಲು) >= 0)
	    {
		ಆದರೆ (ಲಂಬಸಾಲು >= (ಪೂರ್ಣಾಂಕ)cb->ಶಬ್ದ_ಕೊನೆ_ಲಂಬಸಾಲು)
		    clip_update_modeless_selection(cb, cb->origin_ಅಡ್ಡಸಾಲು,
			    cb->origin_ಆರಂಭ_ಲಂಬಸಾಲು, ಅಡ್ಡಸಾಲು, (ಪೂರ್ಣಾಂಕ)Columns);
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    ಆದರೆ (has_mbyte && mb_lefthalve(ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು))
			slen = 2;
		    clip_update_modeless_selection(cb, cb->origin_ಅಡ್ಡಸಾಲು,
			    cb->origin_ಆರಂಭ_ಲಂಬಸಾಲು, ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು + slen);
		}
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		ಆದರೆ (has_mbyte
			&& mb_lefthalve(cb->origin_ಅಡ್ಡಸಾಲು, cb->origin_ಆರಂಭ_ಲಂಬಸಾಲು))
		    slen = 2;
		ಆದರೆ (ಲಂಬಸಾಲು >= (ಪೂರ್ಣಾಂಕ)cb->ಶಬ್ದ_ಕೊನೆ_ಲಂಬಸಾಲು)
		    clip_update_modeless_selection(cb, ಅಡ್ಡಸಾಲು, cb->ಶಬ್ದ_ಕೊನೆ_ಲಂಬಸಾಲು,
			    cb->origin_ಅಡ್ಡಸಾಲು, cb->origin_ಆರಂಭ_ಲಂಬಸಾಲು + slen);
		ಇಲ್ಲದಿದ್ದರೆ
		    clip_update_modeless_selection(cb, ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು,
			    cb->origin_ಅಡ್ಡಸಾಲು, cb->origin_ಆರಂಭ_ಲಂಬಸಾಲು + slen);
	    }
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ SELECT_ಬಹುಲಕ_ಶಬ್ದ:
	    // If we are still within the same word, do nothing
	    ಆದರೆ (ಅಡ್ಡಸಾಲು == cb->prev.lnum && ಲಂಬಸಾಲು >= (ಪೂರ್ಣಾಂಕ)cb->ಶಬ್ದ_ಆರಂಭ_ಲಂಬಸಾಲು
		    && ಲಂಬಸಾಲು < (ಪೂರ್ಣಾಂಕ)cb->ಶಬ್ದ_ಕೊನೆ_ಲಂಬಸಾಲು && !repeated_click)
		ಹಿಂತಿರುಗಿ;

	    // Get new word boundaries
	    clip_get_ಶಬ್ದ_boundaries(cb, ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು);

	    // Handle being after the origin point of selection
	    ಆದರೆ (clip_compare_pos(ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು, cb->origin_ಅಡ್ಡಸಾಲು,
		    cb->origin_ಆರಂಭ_ಲಂಬಸಾಲು) >= 0)
		clip_update_modeless_selection(cb, cb->origin_ಅಡ್ಡಸಾಲು,
			cb->origin_ಆರಂಭ_ಲಂಬಸಾಲು, ಅಡ್ಡಸಾಲು, cb->ಶಬ್ದ_ಕೊನೆ_ಲಂಬಸಾಲು);
	    ಇಲ್ಲದಿದ್ದರೆ
		clip_update_modeless_selection(cb, ಅಡ್ಡಸಾಲು, cb->ಶಬ್ದ_ಆರಂಭ_ಲಂಬಸಾಲು,
			cb->origin_ಅಡ್ಡಸಾಲು, cb->origin_ಕೊನೆ_ಲಂಬಸಾಲು);
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ SELECT_ಬಹುಲಕ_ಸಾಲು:
	    ಆದರೆ (ಅಡ್ಡಸಾಲು == cb->prev.lnum && !repeated_click)
		ಹಿಂತಿರುಗಿ;

	    ಆದರೆ (clip_compare_pos(ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು, cb->origin_ಅಡ್ಡಸಾಲು,
		    cb->origin_ಆರಂಭ_ಲಂಬಸಾಲು) >= 0)
		clip_update_modeless_selection(cb, cb->origin_ಅಡ್ಡಸಾಲು, 0, ಅಡ್ಡಸಾಲು,
			(ಪೂರ್ಣಾಂಕ)Columns);
	    ಇಲ್ಲದಿದ್ದರೆ
		clip_update_modeless_selection(cb, ಅಡ್ಡಸಾಲು, 0, cb->origin_ಅಡ್ಡಸಾಲು,
			(ಪೂರ್ಣಾಂಕ)Columns);
	    ನಿಲ್ಲಿಸು;
    }

    cb->prev.lnum = ಅಡ್ಡಸಾಲು;
    cb->prev.ಲಂಬಸಾಲು  = ಲಂಬಸಾಲು;

# ifdef DEBUG_SELECTION
	ಬರೆ("Selection is: (%ld,%d) to (%ld,%d)\n", cb->ಆರಂಭ.lnum,
		cb->ಆರಂಭ.ಲಂಬಸಾಲು, cb->ಕೊನೆ.lnum, cb->ಕೊನೆ.ಲಂಬಸಾಲು);
# endif
}

# ಆದರೆ defined(FEAT_GUI)
/*
 * Redraw part of the selection if character at "row,col" is inside of it.
 * Only used for the GUI.
 */
    ಶೂನ್ಯ
clip_may_redraw_selection(ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲು, ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು, ಪೂರ್ಣಾಂಕ len)
{
    ಪೂರ್ಣಾಂಕ		ಆರಂಭ = ಲಂಬಸಾಲು;
    ಪೂರ್ಣಾಂಕ		ಕೊನೆ = ಲಂಬಸಾಲು + len;

    ಆದರೆ (clip_ನಕ್ಷತ್ರ.state != SELECT_CLEARED
	    && ಅಡ್ಡಸಾಲು >= clip_ನಕ್ಷತ್ರ.ಆರಂಭ.lnum
	    && ಅಡ್ಡಸಾಲು <= clip_ನಕ್ಷತ್ರ.ಕೊನೆ.lnum)
    {
	ಆದರೆ (ಅಡ್ಡಸಾಲು == clip_ನಕ್ಷತ್ರ.ಆರಂಭ.lnum && ಆರಂಭ < (ಪೂರ್ಣಾಂಕ)clip_ನಕ್ಷತ್ರ.ಆರಂಭ.ಲಂಬಸಾಲು)
	    ಆರಂಭ = clip_ನಕ್ಷತ್ರ.ಆರಂಭ.ಲಂಬಸಾಲು;
	ಆದರೆ (ಅಡ್ಡಸಾಲು == clip_ನಕ್ಷತ್ರ.ಕೊನೆ.lnum && ಕೊನೆ > (ಪೂರ್ಣಾಂಕ)clip_ನಕ್ಷತ್ರ.ಕೊನೆ.ಲಂಬಸಾಲು)
	    ಕೊನೆ = clip_ನಕ್ಷತ್ರ.ಕೊನೆ.ಲಂಬಸಾಲು;
	ಆದರೆ (ಕೊನೆ > ಆರಂಭ)
	    clip_invert_area(&clip_ನಕ್ಷತ್ರ, ಅಡ್ಡಸಾಲು, ಆರಂಭ, ಅಡ್ಡಸಾಲು, ಕೊನೆ, 0);
    }
}
# endif

/*
 * Called from outside to clear selected region from the display
 */
    ಶೂನ್ಯ
clip_clear_selection(Clipboard_T *cbd)
{

    ಆದರೆ (cbd->state == SELECT_CLEARED)
	ಹಿಂತಿರುಗಿ;

    clip_invert_area(cbd, (ಪೂರ್ಣಾಂಕ)cbd->ಆರಂಭ.lnum, cbd->ಆರಂಭ.ಲಂಬಸಾಲು,
				 (ಪೂರ್ಣಾಂಕ)cbd->ಕೊನೆ.lnum, cbd->ಕೊನೆ.ಲಂಬಸಾಲು, CLIP_CLEAR);
    cbd->state = SELECT_CLEARED;
}

/*
 * Clear the selection if any lines from "row1" to "row2" are inside of it.
 */
    ಶೂನ್ಯ
clip_may_clear_selection(ಪೂರ್ಣಾಂಕ row1, ಪೂರ್ಣಾಂಕ row2)
{
    ಆದರೆ (clip_ನಕ್ಷತ್ರ.state == SELECT_ಮುಗಿದ
	    && row2 >= clip_ನಕ್ಷತ್ರ.ಆರಂಭ.lnum
	    && row1 <= clip_ನಕ್ಷತ್ರ.ಕೊನೆ.lnum)
	clip_clear_selection(&clip_ನಕ್ಷತ್ರ);
}

/*
 * Called before the screen is scrolled up or down.  Adjusts the line numbers
 * of the selection.  Call with big number when clearing the screen.
 */
    ಶೂನ್ಯ
clip_scroll_selection(
    ಪೂರ್ಣಾಂಕ	    ಅಡ್ಡಸಾಲುಗಳು_2)		// negative for scroll down
{
    ಪೂರ್ಣಾಂಕ	    lnum;

    ಆದರೆ (clip_ನಕ್ಷತ್ರ.state == SELECT_CLEARED)
	ಹಿಂತಿರುಗಿ;

    lnum = clip_ನಕ್ಷತ್ರ.ಆರಂಭ.lnum - ಅಡ್ಡಸಾಲುಗಳು_2;
    ಆದರೆ (lnum <= 0)
	clip_ನಕ್ಷತ್ರ.ಆರಂಭ.lnum = 0;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (lnum >= screen_ಅಡ್ಡಸಾಲುಗಳು)	// scrolled off of the screen
	clip_ನಕ್ಷತ್ರ.state = SELECT_CLEARED;
    ಇಲ್ಲದಿದ್ದರೆ
	clip_ನಕ್ಷತ್ರ.ಆರಂಭ.lnum = lnum;

    lnum = clip_ನಕ್ಷತ್ರ.ಕೊನೆ.lnum - ಅಡ್ಡಸಾಲುಗಳು_2;
    ಆದರೆ (lnum < 0)			// scrolled off of the screen
	clip_ನಕ್ಷತ್ರ.state = SELECT_CLEARED;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (lnum >= screen_ಅಡ್ಡಸಾಲುಗಳು)
	clip_ನಕ್ಷತ್ರ.ಕೊನೆ.lnum = screen_ಅಡ್ಡಸಾಲುಗಳು - 1;
    ಇಲ್ಲದಿದ್ದರೆ
	clip_ನಕ್ಷತ್ರ.ಕೊನೆ.lnum = lnum;
}

/*
 * Copy the currently selected area into the '*' or '+' register so it will be
 * available for pasting.
 * When "both" is TRUE also copy to the other register.
 */
    ಶೂನ್ಯ
clip_copy_modeless_selection(ಪೂರ್ಣಾಂಕ both ಬಳಸದ)
{
    // The info for the modeless selection is stored in '*' register, however if
    // we are using the '+' register for modeless autoselect, we copy to
    // clip_plus instead while using the info in clip_star.
    Clipboard_T *cbd = clip_isautosel_plus() ? &clip_plus : &clip_ನಕ್ಷತ್ರ;
    char_u	*ಬಫರ್;
    char_u	*bufp;
    ಪೂರ್ಣಾಂಕ		ಅಡ್ಡಸಾಲು;
    ಪೂರ್ಣಾಂಕ		ಆರಂಭ_ಲಂಬಸಾಲು;
    ಪೂರ್ಣಾಂಕ		ಕೊನೆ_ಲಂಬಸಾಲು;
    ಪೂರ್ಣಾಂಕ		ಸಾಲು_ಕೊನೆ_ಲಂಬಸಾಲು;
    ಪೂರ್ಣಾಂಕ		ಕೂಡಿಸು_newline_flag = FALSE;
    ಪೂರ್ಣಾಂಕ		len;
    char_u	*p;
    ಪೂರ್ಣಾಂಕ		row1 = clip_ನಕ್ಷತ್ರ.ಆರಂಭ.lnum;
    ಪೂರ್ಣಾಂಕ		col1 = clip_ನಕ್ಷತ್ರ.ಆರಂಭ.ಲಂಬಸಾಲು;
    ಪೂರ್ಣಾಂಕ		row2 = clip_ನಕ್ಷತ್ರ.ಕೊನೆ.lnum;
    ಪೂರ್ಣಾಂಕ		col2 = clip_ನಕ್ಷತ್ರ.ಕೊನೆ.ಲಂಬಸಾಲು;

    // Can't use ScreenLines unless initialized
    ಆದರೆ (Screen_ಸಾಲುಗಳು == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    /*
     * Make sure row1 <= row2, and if row1 == row2 that col1 <= col2.
     */
    ಆದರೆ (row1 > row2)
    {
	ಅಡ್ಡಸಾಲು = row1; row1 = row2; row2 = ಅಡ್ಡಸಾಲು;
	ಅಡ್ಡಸಾಲು = col1; col1 = col2; col2 = ಅಡ್ಡಸಾಲು;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (row1 == row2 && col1 > col2)
    {
	ಅಡ್ಡಸಾಲು = col1; col1 = col2; col2 = ಅಡ್ಡಸಾಲು;
    }
# ifdef FEAT_PROP_POPUP
    ಆದರೆ (col1 < clip_ನಕ್ಷತ್ರ.min_ಲಂಬಸಾಲು)
	col1 = clip_ನಕ್ಷತ್ರ.min_ಲಂಬಸಾಲು;
    ಆದರೆ (col2 > clip_ನಕ್ಷತ್ರ.max_ಲಂಬಸಾಲು)
	col2 = clip_ನಕ್ಷತ್ರ.max_ಲಂಬಸಾಲು;
    ಆದರೆ (row1 > clip_ನಕ್ಷತ್ರ.max_ಅಡ್ಡಸಾಲು || row2 < clip_ನಕ್ಷತ್ರ.min_ಅಡ್ಡಸಾಲು)
	ಹಿಂತಿರುಗಿ;
    ಆದರೆ (row1 < clip_ನಕ್ಷತ್ರ.min_ಅಡ್ಡಸಾಲು)
	row1 = clip_ನಕ್ಷತ್ರ.min_ಅಡ್ಡಸಾಲು;
    ಆದರೆ (row2 > clip_ನಕ್ಷತ್ರ.max_ಅಡ್ಡಸಾಲು)
	row2 = clip_ನಕ್ಷತ್ರ.max_ಅಡ್ಡಸಾಲು;
# endif
    // correct starting point for being on right half of double-wide char
    p = Screen_ಸಾಲುಗಳು + ಸಾಲು_ಸ್ಥಾನಾಂತರ[row1];
    ಆದರೆ (enc_dbcs != 0)
	col1 -= (*mb_ತಲೆ_off)(p, p + col1);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (enc_utf8 && p[col1] == 0)
	--col1;

    // Create a temporary buffer for storing the text
    len = (row2 - row1 + 1) * Columns + 1;
    ಆದರೆ (enc_dbcs != 0)
	len *= 2;	// max. 2 bytes per display cell
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (enc_utf8)
	len *= MB_MAXBYTES;
    // A newline is added after each row
    len += row2 - row1 + 2;
    ಬಫರ್ = alloc(len);
    ಆದರೆ (ಬಫರ್ == ಖಾಲಿ)	    // out of memory
	ಹಿಂತಿರುಗಿ;

    // Process each row in the selection
    ಗಾಗಿ (bufp = ಬಫರ್, ಅಡ್ಡಸಾಲು = row1; ಅಡ್ಡಸಾಲು <= row2; ಅಡ್ಡಸಾಲು++)
    {
	ಆದರೆ (ಅಡ್ಡಸಾಲು == row1)
	    ಆರಂಭ_ಲಂಬಸಾಲು = col1;
	ಇಲ್ಲದಿದ್ದರೆ
# ifdef FEAT_PROP_POPUP
	    ಆರಂಭ_ಲಂಬಸಾಲು = clip_ನಕ್ಷತ್ರ.min_ಲಂಬಸಾಲು;
# ಇಲ್ಲದಿದ್ದರೆ
	    ಆರಂಭ_ಲಂಬಸಾಲು = 0;
# endif

	ಆದರೆ (ಅಡ್ಡಸಾಲು == row2)
	    ಕೊನೆ_ಲಂಬಸಾಲು = col2;
	ಇಲ್ಲದಿದ್ದರೆ
# ifdef FEAT_PROP_POPUP
	    ಕೊನೆ_ಲಂಬಸಾಲು = clip_ನಕ್ಷತ್ರ.max_ಲಂಬಸಾಲು;
# ಇಲ್ಲದಿದ್ದರೆ
	    ಕೊನೆ_ಲಂಬಸಾಲು = Columns;
# endif

	ಸಾಲು_ಕೊನೆ_ಲಂಬಸಾಲು = clip_get_ಸಾಲು_ಕೊನೆ(&clip_ನಕ್ಷತ್ರ, ಅಡ್ಡಸಾಲು);

	// See if we need to nuke some trailing whitespace
	ಆದರೆ (ಕೊನೆ_ಲಂಬಸಾಲು >=
# ifdef FEAT_PROP_POPUP
		clip_ನಕ್ಷತ್ರ.max_ಲಂಬಸಾಲು
# ಇಲ್ಲದಿದ್ದರೆ
		Columns
# endif
		    && (ಅಡ್ಡಸಾಲು < row2 || ಕೊನೆ_ಲಂಬಸಾಲು > ಸಾಲು_ಕೊನೆ_ಲಂಬಸಾಲು))
	{
	    // Get rid of trailing whitespace
	    ಕೊನೆ_ಲಂಬಸಾಲು = ಸಾಲು_ಕೊನೆ_ಲಂಬಸಾಲು;
	    ಆದರೆ (ಕೊನೆ_ಲಂಬಸಾಲು < ಆರಂಭ_ಲಂಬಸಾಲು)
		ಕೊನೆ_ಲಂಬಸಾಲು = ಆರಂಭ_ಲಂಬಸಾಲು;

	    // If the last line extended to the end, add an extra newline
	    ಆದರೆ (ಅಡ್ಡಸಾಲು == row2)
		ಕೂಡಿಸು_newline_flag = TRUE;
	}

	// If after the first row, we need to always add a newline
	ಆದರೆ (ಅಡ್ಡಸಾಲು > row1 && !ಸಾಲು_Wraps[ಅಡ್ಡಸಾಲು - 1])
	    *bufp++ = NL;

	// Safetey check for in case resizing went wrong
	ಆದರೆ (ಅಡ್ಡಸಾಲು < screen_ಅಡ್ಡಸಾಲುಗಳು && ಕೊನೆ_ಲಂಬಸಾಲು <= screen_Columns)
	{
	    ಆದರೆ (enc_dbcs != 0)
	    {
		ಪೂರ್ಣಾಂಕ	i;

		p = Screen_ಸಾಲುಗಳು + ಸಾಲು_ಸ್ಥಾನಾಂತರ[ಅಡ್ಡಸಾಲು];
		ಗಾಗಿ (i = ಆರಂಭ_ಲಂಬಸಾಲು; i < ಕೊನೆ_ಲಂಬಸಾಲು; ++i)
		    ಆದರೆ (enc_dbcs == DBCS_JPNU && p[i] == 0x8e)
		    {
			// single-width double-byte char
			*bufp++ = 0x8e;
			*bufp++ = ScreenLines2[ಸಾಲು_ಸ್ಥಾನಾಂತರ[ಅಡ್ಡಸಾಲು] + i];
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			*bufp++ = p[i];
			ಆದರೆ (MB_BYTE2LEN(p[i]) == 2)
			    *bufp++ = p[++i];
		    }
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (enc_utf8)
	    {
		ಪೂರ್ಣಾಂಕ	off;
		ಪೂರ್ಣಾಂಕ	i;

		off = ಸಾಲು_ಸ್ಥಾನಾಂತರ[ಅಡ್ಡಸಾಲು];
		ಗಾಗಿ (i = ಆರಂಭ_ಲಂಬಸಾಲು; i < ಕೊನೆ_ಲಂಬಸಾಲು; ++i)
		{
		    // The base character is either in ScreenLinesUC[] or
		    // ScreenLines[].
		    ಆದರೆ (Screen_ಸಾಲುಗಳು_UC[off + i] == 0)
			*bufp++ = Screen_ಸಾಲುಗಳು[off + i];
		    ಇಲ್ಲದಿದ್ದರೆ
			// Includes the composing characters.
			bufp += utfc_char2bytes(off + i, bufp);
		    // Skip right half of double-wide character.
		    ಆದರೆ (Screen_ಸಾಲುಗಳು[off + i + 1] == 0)
			++i;
		}
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		STRNCPY(bufp, Screen_ಸಾಲುಗಳು + ಸಾಲು_ಸ್ಥಾನಾಂತರ[ಅಡ್ಡಸಾಲು] + ಆರಂಭ_ಲಂಬಸಾಲು,
							 ಕೊನೆ_ಲಂಬಸಾಲು - ಆರಂಭ_ಲಂಬಸಾಲು);
		bufp += ಕೊನೆ_ಲಂಬಸಾಲು - ಆರಂಭ_ಲಂಬಸಾಲು;
	    }
	}
    }

    // Add a newline at the end if the selection ended there
    ಆದರೆ (ಕೂಡಿಸು_newline_flag)
	*bufp++ = NL;

    // First cleanup any old selection and become the owner.
    clip_free_selection(cbd);
    clip_own_selection(cbd);

    // Yank the text into the '*' register.
    clip_yank_selection(MCHAR, ಬಫರ್, (ದೀರ್ಘ)(bufp - ಬಫರ್), cbd);

    // Make the register contents available to the outside world.
    clip_gen_ಗಣ_selection(cbd);

# ifdef FEAT_X11
    ಆದರೆ (both)
    {
	Clipboard_T *other = cbd == &clip_ನಕ್ಷತ್ರ ? &clip_plus : &clip_ನಕ್ಷತ್ರ;
	// Do the same for the '+' register.
	clip_free_selection(other);
	clip_own_selection(other);
	clip_yank_selection(MCHAR, ಬಫರ್, (ದೀರ್ಘ)(bufp - ಬಫರ್), other);
	clip_gen_ಗಣ_selection(other);
    }
# endif
    vim_free(ಬಫರ್);
}

    ಶೂನ್ಯ
clip_gen_ಗಣ_selection(Clipboard_T *cbd)
{
    ಆದರೆ (!clip_did_ಗಣ_selection)
    {
	// Updating postponed, so that accessing the system clipboard won't
	// hang Vim when accessing it many times (e.g. on a :g command).
	ಆದರೆ ((cbd == &clip_plus && (clip_unnamed_saved & CLIP_UNNAMED_PLUS))
		|| (cbd == &clip_ನಕ್ಷತ್ರ && (clip_unnamed_saved & CLIP_UNNAMED)))
	{
	    clipboard_needs_update = TRUE;
	    ಹಿಂತಿರುಗಿ;
	}
    }
# ಆದರೆ defined(FEAT_XCLIPBOARD) || defined(FEAT_WAYLAND_CLIPBOARD)
#  ifdef FEAT_GUI
    ಆದರೆ (gui.ಒಳ_use)
	clip_mch_ಗಣ_selection(cbd);
    ಇಲ್ಲದಿದ್ದರೆ
#  endif
    {
	ಆದರೆ (clipmethod == CLIPMETHOD_WAYLAND)
	{
#  ifdef FEAT_WAYLAND_CLIPBOARD
	    clip_wl_ಗಣ_selection(cbd);
#  endif
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (clipmethod == CLIPMETHOD_X11)
	{
#  ifdef FEAT_XCLIPBOARD
	    clip_xterm_ಗಣ_selection(cbd);
#  endif
	}
    }
# ಇಲ್ಲದಿದ್ದರೆ
    clip_mch_ಗಣ_selection(cbd);
# endif
}

    ಸ್ಥಿರ ಶೂನ್ಯ
clip_gen_request_selection(Clipboard_T *cbd)
{
# ಆದರೆ defined(FEAT_XCLIPBOARD) || defined(FEAT_WAYLAND_CLIPBOARD)
#  ifdef FEAT_GUI
    ಆದರೆ (gui.ಒಳ_use)
	clip_mch_request_selection(cbd);
    ಇಲ್ಲದಿದ್ದರೆ
#  endif
    {
	ಆದರೆ (clipmethod == CLIPMETHOD_WAYLAND)
	{
#  ifdef FEAT_WAYLAND_CLIPBOARD
	    clip_wl_request_selection(cbd);
#  endif
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (clipmethod == CLIPMETHOD_X11)
	{
#  ifdef FEAT_XCLIPBOARD
	    clip_xterm_request_selection(cbd);
#  endif
	}
    }
# ಇಲ್ಲದಿದ್ದರೆ
    clip_mch_request_selection(cbd);
# endif
}

# ಆದರೆ (defined(FEAT_X11) && defined(FEAT_XCLIPBOARD) && defined(USE_SYSTEM))
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
clip_x11_owner_exists(Clipboard_T *cbd)
{
    ಹಿಂತಿರುಗಿ XGetSelectionOwner(X_DISPLAY, cbd->sel_atom) != None;
}
# endif

# ಆದರೆ (defined(FEAT_X11) || defined(FEAT_WAYLAND_CLIPBOARD)) \
	&& defined(USE_SYSTEM)
    ಪೂರ್ಣಾಂಕ
clip_gen_owner_exists(Clipboard_T *cbd ಬಳಸದ)
{
#  ಆದರೆ defined(FEAT_XCLIPBOARD) || defined(FEAT_WAYLAND_CLIPBOARD)
#   ifdef FEAT_GUI_GTK
    ಆದರೆ (gui.ಒಳ_use)
	ಹಿಂತಿರುಗಿ clip_gtk_owner_exists(cbd);
    ಇಲ್ಲದಿದ್ದರೆ
#   endif
    {
	ಆದರೆ (clipmethod == CLIPMETHOD_WAYLAND)
	{
#   ifdef FEAT_WAYLAND_CLIPBOARD
	    ಹಿಂತಿರುಗಿ clip_wl_owner_exists(cbd);
#   endif
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (clipmethod == CLIPMETHOD_X11)
	{
#   ifdef FEAT_XCLIPBOARD
	    ಹಿಂತಿರುಗಿ clip_x11_owner_exists(cbd);
#   endif
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    ಹಿಂತಿರುಗಿ FALSE;
    }
#  ಇಲ್ಲದಿದ್ದರೆ
    ಹಿಂತಿರುಗಿ TRUE;
#  endif
}
# endif

/*
 * Stuff for the X clipboard.  Shared between VMS and Unix.
 */

# ಆದರೆ defined(FEAT_XCLIPBOARD) || defined(FEAT_GUI_X11)
#  include <X11/Xatom.h>
#  include <X11/Intrinsic.h>

/*
 * Open the application context (if it hasn't been opened yet).
 * Used for Motif GUI and the xterm clipboard.
 */
    ಶೂನ್ಯ
open_app_context(ಶೂನ್ಯ)
{
    ಆದರೆ (app_context == ಖಾಲಿ)
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
ಸ್ಥಿರ Atom	compound_ಪಠ್ಯ_atom;
ಸ್ಥಿರ Atom	ಪಠ್ಯ_atom;
ಸ್ಥಿರ Atom	targets_atom;
ಸ್ಥಿರ Atom	timestamp_atom;	// Used to get a timestamp

    ಶೂನ್ಯ
x11_ಸಿದ್ಧತೆ_atoms(Display *dpy)
{
    vim_atom	       = XInternAtom(dpy, VIM_ATOM_NAME,   False);
    vimenc_atom	       = XInternAtom(dpy, VIMENC_ATOM_NAME,False);
    vim_mt_atom	       = XInternAtom(dpy, VIM_MIMETYPE_NAME,   False);
    vimenc_mt_atom     = XInternAtom(dpy, VIMENC_MIMETYPE_NAME,False);
    utf8_atom	       = XInternAtom(dpy, "UTF8_STRING",   False);
    compound_ಪಠ್ಯ_atom = XInternAtom(dpy, "COMPOUND_TEXT", False);
    ಪಠ್ಯ_atom	       = XInternAtom(dpy, "TEXT",	   False);
    targets_atom       = XInternAtom(dpy, "TARGETS",	   False);
    clip_ನಕ್ಷತ್ರ.sel_atom = XA_PRIMARY;
    clip_plus.sel_atom = XInternAtom(dpy, "CLIPBOARD",	   False);
    timestamp_atom     = XInternAtom(dpy, "TIMESTAMP",	   False);
}

/*
 * X Selection stuff, for cutting and pasting text to other windows.
 */

    ಸ್ಥಿರ Boolean
clip_x11_convert_selection_cb(
    Widget	w ಬಳಸದ,
    Atom	*sel_atom,
    Atom	*ಗುರಿ,
    Atom	*type,
    Xt_ಸೂಚಕ	*ಮೌಲ್ಯ,
    long_u	*ಉದ್ದಳತೆ,
    ಪೂರ್ಣಾಂಕ		*format)
{
    ಸ್ಥಿರ char_u   *save_ಫಲಿತಾಂಶ = ಖಾಲಿ;
    ಸ್ಥಿರ long_u   save_ಉದ್ದಳತೆ = 0;
    char_u	    *string;
    ಪೂರ್ಣಾಂಕ		    motion_type;
    Clipboard_T    *cbd;
    ಪೂರ್ಣಾಂಕ		    i;

    ಆದರೆ (*sel_atom == clip_plus.sel_atom)
	cbd = &clip_plus;
    ಇಲ್ಲದಿದ್ದರೆ
	cbd = &clip_ನಕ್ಷತ್ರ;

    ಆದರೆ (!cbd->owned)
	ಹಿಂತಿರುಗಿ False;	    // Shouldn't ever happen

    // requestor wants to know what target types we support
    ಆದರೆ (*ಗುರಿ == targets_atom)
    {
	ಸ್ಥಿರ Atom array[9];

	*ಮೌಲ್ಯ = (Xt_ಸೂಚಕ)array;
	i = 0;
	array[i++] = targets_atom;
	array[i++] = vimenc_atom;
	array[i++] = vim_atom;
	array[i++] = vimenc_mt_atom;
	array[i++] = vim_mt_atom;
	ಆದರೆ (enc_utf8)
	    array[i++] = utf8_atom;
	array[i++] = XA_STRING;
	array[i++] = ಪಠ್ಯ_atom;
	array[i++] = compound_ಪಠ್ಯ_atom;

	*type = XA_ATOM;
	// This used to be: *format = sizeof(Atom) * 8; but that caused
	// crashes on 64 bit machines. (Peter Derr)
	*format = 32;
	*ಉದ್ದಳತೆ = i;
	ಹಿಂತಿರುಗಿ True;
    }

    ಆದರೆ (       *ಗುರಿ != XA_STRING
	    && *ಗುರಿ != vimenc_atom
	    && *ಗುರಿ != vimenc_mt_atom
	    && (*ಗುರಿ != utf8_atom || !enc_utf8)
	    && *ಗುರಿ != vim_atom
	    && *ಗುರಿ != vim_mt_atom
	    && *ಗುರಿ != ಪಠ್ಯ_atom
	    && *ಗುರಿ != compound_ಪಠ್ಯ_atom)
	ಹಿಂತಿರುಗಿ False;

    clip_get_selection(cbd);
    motion_type = clip_convert_selection(&string, ಉದ್ದಳತೆ, cbd);
    ಆದರೆ (motion_type < 0)
	ಹಿಂತಿರುಗಿ False;

    // For our own format, the first byte contains the motion type
    ಆದರೆ (*ಗುರಿ == vim_atom || *ಗುರಿ == vim_mt_atom)
	(*ಉದ್ದಳತೆ)++;

    // Our own format with encoding: motion 'encoding' NUL text
    ಆದರೆ (*ಗುರಿ == vimenc_atom || *ಗುರಿ == vimenc_mt_atom)
	*ಉದ್ದಳತೆ += STRLEN(p_enc) + 2;

    ಆದರೆ (save_ಉದ್ದಳತೆ < *ಉದ್ದಳತೆ || save_ಉದ್ದಳತೆ / 2 >= *ಉದ್ದಳತೆ)
	*ಮೌಲ್ಯ = XtRealloc((ಅಕ್ಷರ *)save_ಫಲಿತಾಂಶ, (Cardinal)*ಉದ್ದಳತೆ + 1);
    ಇಲ್ಲದಿದ್ದರೆ
	*ಮೌಲ್ಯ = save_ಫಲಿತಾಂಶ;
    ಆದರೆ (*ಮೌಲ್ಯ == ಖಾಲಿ)
    {
	vim_free(string);
	ಹಿಂತಿರುಗಿ False;
    }
    save_ಫಲಿತಾಂಶ = (char_u *)*ಮೌಲ್ಯ;
    save_ಉದ್ದಳತೆ = *ಉದ್ದಳತೆ;

    ಆದರೆ (*ಗುರಿ == XA_STRING || (*ಗುರಿ == utf8_atom && enc_utf8))
    {
	mch_memmove(save_ಫಲಿತಾಂಶ, string, (size_t)(*ಉದ್ದಳತೆ));
	*type = *ಗುರಿ;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*ಗುರಿ == compound_ಪಠ್ಯ_atom || *ಗುರಿ == ಪಠ್ಯ_atom)
    {
	XTextProperty	ಪಠ್ಯ_prop;
	ಅಕ್ಷರ		*string_nt = (ಅಕ್ಷರ *)save_ಫಲಿತಾಂಶ;
	ಪೂರ್ಣಾಂಕ		conv_ಫಲಿತಾಂಶ;

	// create NUL terminated string which XmbTextListToTextProperty wants
	mch_memmove(string_nt, string, (size_t)*ಉದ್ದಳತೆ);
	string_nt[*ಉದ್ದಳತೆ] = NUL;
	conv_ಫಲಿತಾಂಶ = Xmb_ಪಠ್ಯ_List_ಗಮ್ಯ_ಪಠ್ಯ_Property(X_DISPLAY, &string_nt,
					   1, XCompound_ಪಠ್ಯ_Style, &ಪಠ್ಯ_prop);
	ಆದರೆ (conv_ಫಲಿತಾಂಶ != Success)
	{
	    vim_free(string);
	    ಹಿಂತಿರುಗಿ False;
	}
	*ಮೌಲ್ಯ = (Xt_ಸೂಚಕ)(ಪಠ್ಯ_prop.ಮೌಲ್ಯ);	//    from plain text
	*ಉದ್ದಳತೆ = ಪಠ್ಯ_prop.nitems;
	*type = compound_ಪಠ್ಯ_atom;
	XtFree((ಅಕ್ಷರ *)save_ಫಲಿತಾಂಶ);
	save_ಫಲಿತಾಂಶ = (char_u *)*ಮೌಲ್ಯ;
	save_ಉದ್ದಳತೆ = *ಉದ್ದಳತೆ;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*ಗುರಿ == vimenc_atom || *ಗುರಿ == vimenc_mt_atom)
    {
	ಪೂರ್ಣಾಂಕ l = STRLEN(p_enc);

	save_ಫಲಿತಾಂಶ[0] = motion_type;
	STRCPY(save_ಫಲಿತಾಂಶ + 1, p_enc);
	mch_memmove(save_ಫಲಿತಾಂಶ + l + 2, string, (size_t)(*ಉದ್ದಳತೆ - l - 2));
	ಆದರೆ (*ಗುರಿ == vimenc_atom)
	    *type = vimenc_atom;
	ಇಲ್ಲದಿದ್ದರೆ
	    *type = vimenc_mt_atom;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	save_ಫಲಿತಾಂಶ[0] = motion_type;
	mch_memmove(save_ಫಲಿತಾಂಶ + 1, string, (size_t)(*ಉದ್ದಳತೆ - 1));
	ಆದರೆ (*ಗುರಿ == vim_atom)
	    *type = vim_atom;
	ಇಲ್ಲದಿದ್ದರೆ
	    *type = vim_mt_atom;
    }
    *format = 8;	    // 8 bits per char
    vim_free(string);
    ಹಿಂತಿರುಗಿ True;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
clip_x11_lose_ownership_cb(Widget w ಬಳಸದ, Atom *sel_atom)
{
    ಆದರೆ (*sel_atom == clip_plus.sel_atom)
	clip_lose_selection(&clip_plus);
    ಇಲ್ಲದಿದ್ದರೆ
	clip_lose_selection(&clip_ನಕ್ಷತ್ರ);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
clip_x11_notify_cb(Widget w ಬಳಸದ, Atom *sel_atom ಬಳಸದ, Atom *ಗುರಿ ಬಳಸದ)
{
    // To prevent automatically freeing the selection value.
}

/*
 * Property callback to get a timestamp for XtOwnSelection.
 */
#  ಆದರೆ defined(FEAT_X11) && defined(FEAT_XCLIPBOARD)
    ಸ್ಥಿರ ಶೂನ್ಯ
clip_x11_timestamp_cb(
    Widget	w,
    Xt_ಸೂಚಕ	n ಬಳಸದ,
    XEvent	*event,
    Boolean	*cont ಬಳಸದ)
{
    Atom	    actual_type;
    ಪೂರ್ಣಾಂಕ		    format;
    ಚಿಹ್ನೆರಹಿತ  ದೀರ್ಘ  nitems, ಬೈಟುಗಳು_after;
    ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ   *prop=ಖಾಲಿ;
    XPropertyEvent  *xproperty=&event->xproperty;

    // Must be a property notify, state can't be Delete (True), has to be
    // one of the supported selection types.
    ಆದರೆ (event->type != PropertyNotify || xproperty->state
	    || (xproperty->atom != clip_ನಕ್ಷತ್ರ.sel_atom
				    && xproperty->atom != clip_plus.sel_atom))
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (XGet_ಕಿಟಕಿ_Property(xproperty->display, xproperty->ಕಿಟಕಿ_2,
	  xproperty->atom, 0, 0, False, timestamp_atom, &actual_type, &format,
						&nitems, &ಬೈಟುಗಳು_after, &prop))
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (prop)
	XFree(prop);

    // Make sure the property type is "TIMESTAMP" and it's 32 bits.
    ಆದರೆ (actual_type != timestamp_atom || format != 32)
	ಹಿಂತಿರುಗಿ;

    // Get the selection, using the event timestamp.
    ಆದರೆ (XtOwnSelection(w, xproperty->atom, xproperty->ಸಮಯ,
	    clip_x11_convert_selection_cb, clip_x11_lose_ownership_cb,
	    clip_x11_notify_cb) == OK)
    {
	// Set the "owned" flag now, there may have been a call to
	// lose_ownership_cb in between.
	ಆದರೆ (xproperty->atom == clip_plus.sel_atom)
	    clip_plus.owned = TRUE;
	ಇಲ್ಲದಿದ್ದರೆ
	    clip_ನಕ್ಷತ್ರ.owned = TRUE;
    }
}

    ಶೂನ್ಯ
x11_ಸಿದ್ಧತೆ_selection(Widget w)
{
    Xt_ಕೂಡಿಸು_Event_Handler(w, Property_Change_ಮುಖವಾಡ, False,
	    /*(XtEventHandler)*/clip_x11_timestamp_cb, (Xt_ಸೂಚಕ)ಖಾಲಿ);
}
#  endif

    ಸ್ಥಿರ ಶೂನ್ಯ
clip_x11_request_selection_cb(
    Widget	w ಬಳಸದ,
    Xt_ಸೂಚಕ	success,
    Atom	*sel_atom,
    Atom	*type,
    Xt_ಸೂಚಕ	ಮೌಲ್ಯ,
    long_u	*ಉದ್ದಳತೆ,
    ಪೂರ್ಣಾಂಕ		*format)
{
    ಪೂರ್ಣಾಂಕ		motion_type = MAUTO;
    long_u	len;
    char_u	*p;
    ಅಕ್ಷರ	**ಪಠ್ಯ_list = ಖಾಲಿ;
    Clipboard_T	*cbd;
    char_u	*tmpbuf = ಖಾಲಿ;

    ಆದರೆ (*sel_atom == clip_plus.sel_atom)
	cbd = &clip_plus;
    ಇಲ್ಲದಿದ್ದರೆ
	cbd = &clip_ನಕ್ಷತ್ರ;

    ಆದರೆ (ಮೌಲ್ಯ == ಖಾಲಿ || *ಉದ್ದಳತೆ == 0)
    {
	clip_free_selection(cbd);	// nothing received, clear register
	*(ಪೂರ್ಣಾಂಕ *)success = FALSE;
	ಹಿಂತಿರುಗಿ;
    }
    p = (char_u *)ಮೌಲ್ಯ;
    len = *ಉದ್ದಳತೆ;
    ಆದರೆ (*type == vim_atom || *type == vim_mt_atom)
    {
	motion_type = *p++;
	len--;
    }

    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*type == vimenc_atom || *type == vimenc_mt_atom)
    {
	char_u		*enc;
	vimconv_T	conv;
	ಪೂರ್ಣಾಂಕ		convlen;

	motion_type = *p++;
	--len;

	enc = p;
	p += STRLEN(p) + 1;
	len -= p - enc;

	// If the encoding of the text is different from 'encoding', attempt
	// converting it.
	conv.vc_type = CONV_NONE;
	convert_ಸಿದ್ಧತೆ(&conv, enc, p_enc);
	ಆದರೆ (conv.vc_type != CONV_NONE)
	{
	    convlen = len;	// Need to use an int here.
	    tmpbuf = string_convert(&conv, p, &convlen);
	    len = convlen;
	    ಆದರೆ (tmpbuf != ಖಾಲಿ)
		p = tmpbuf;
	    convert_ಸಿದ್ಧತೆ(&conv, ಖಾಲಿ, ಖಾಲಿ);
	}
    }

    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*type == compound_ಪಠ್ಯ_atom
	    || *type == utf8_atom
	    || (enc_dbcs != 0 && *type == ಪಠ್ಯ_atom))
    {
	XTextProperty	ಪಠ್ಯ_prop;
	ಪೂರ್ಣಾಂಕ		n_ಪಠ್ಯ = 0;
	ಪೂರ್ಣಾಂಕ		ಸ್ಥಿತಿ_2;

	ಪಠ್ಯ_prop.ಮೌಲ್ಯ = (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *)ಮೌಲ್ಯ;
	ಪಠ್ಯ_prop.encoding = *type;
	ಪಠ್ಯ_prop.format = *format;
	ಪಠ್ಯ_prop.nitems = len;
#  ಆದರೆ defined(X_HAVE_UTF8_STRING)
	ಆದರೆ (*type == utf8_atom)
	    ಸ್ಥಿತಿ_2 = Xutf8_ಪಠ್ಯ_Property_ಗಮ್ಯ_ಪಠ್ಯ_List(X_DISPLAY, &ಪಠ್ಯ_prop,
							 &ಪಠ್ಯ_list, &n_ಪಠ್ಯ);
	ಇಲ್ಲದಿದ್ದರೆ
#  endif
	    ಸ್ಥಿತಿ_2 = Xmb_ಪಠ್ಯ_Property_ಗಮ್ಯ_ಪಠ್ಯ_List(X_DISPLAY, &ಪಠ್ಯ_prop,
							 &ಪಠ್ಯ_list, &n_ಪಠ್ಯ);
	ಆದರೆ (ಸ್ಥಿತಿ_2 != Success || n_ಪಠ್ಯ < 1)
	{
	    *(ಪೂರ್ಣಾಂಕ *)success = FALSE;
	    ಹಿಂತಿರುಗಿ;
	}
	p = (char_u *)ಪಠ್ಯ_list[0];
	len = STRLEN(p);
    }
    clip_yank_selection(motion_type, p, (ದೀರ್ಘ)len, cbd);

    ಆದರೆ (ಪಠ್ಯ_list != ಖಾಲಿ)
	XFreeStringList(ಪಠ್ಯ_list);
    vim_free(tmpbuf);
    XtFree((ಅಕ್ಷರ *)ಮೌಲ್ಯ);
    *(ಪೂರ್ಣಾಂಕ *)success = TRUE;
}

    ಶೂನ್ಯ
clip_x11_request_selection(
    Widget	myShell,
    Display	*dpy,
    Clipboard_T	*cbd)
{
    XEvent	event;
    Atom	type;
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	success;
    ಪೂರ್ಣಾಂಕ		i;
    time_t	ಆರಂಭ_ಸಮಯ;
    ಪೂರ್ಣಾಂಕ		timed_ಹೊರಹೋಗು = FALSE;

    ಗಾಗಿ (i = 0; i < 8; i++)
    {
	ಆಯ್ಕೆ (i)
	{
	    ಸ್ಥಿತಿ 0:  type = vimenc_mt_atom;	ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ 1:  type = vimenc_atom;	ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ 2:  type = vim_mt_atom;	ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ 3:  type = vim_atom;		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ 4:  type = utf8_atom;		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ 5:  type = compound_ಪಠ್ಯ_atom; ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ 6:  type = ಪಠ್ಯ_atom;		ನಿಲ್ಲಿಸು;
	    ಪೂರ್ವನಿಯೋಜಿತ: type = XA_STRING;
	}
	ಆದರೆ (type == utf8_atom
#  ಆದರೆ defined(X_HAVE_UTF8_STRING)
		&& !enc_utf8
#  endif
		)
	    // Only request utf-8 when 'encoding' is utf8 and
	    // Xutf8TextPropertyToTextList is available.
	    ಮುಂದುವರಿ;
	success = MAYBE;
	Xt_Get_Selection_ಮೌಲ್ಯ(myShell, cbd->sel_atom, type,
	    clip_x11_request_selection_cb, (Xt_ಸೂಚಕ)&success, Current_ಸಮಯ);

	// Make sure the request for the selection goes out before waiting for
	// a response.
	XFlush(dpy);

	/*
	 * Wait for result of selection request, otherwise if we type more
	 * characters, then they will appear before the one that requested the
	 * paste!  Don't worry, we will catch up with any other events later.
	 */
	ಆರಂಭ_ಸಮಯ = ಸಮಯ(ಖಾಲಿ);
	ಆಗ (success == MAYBE)
	{
	    ಆದರೆ (XCheckTypedEvent(dpy, PropertyNotify, &event)
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
		ಮುಂದುವರಿ;
	    }

	    // Time out after 2 to 3 seconds to avoid that we hang when the
	    // other process doesn't respond.  Note that the SelectionNotify
	    // event may still come later when the selection owner comes back
	    // to life and the text gets inserted unexpectedly.  Don't know
	    // why that happens or how to avoid that :-(.
	    ಆದರೆ (ಸಮಯ(ಖಾಲಿ) > ಆರಂಭ_ಸಮಯ + 2)
	    {
		timed_ಹೊರಹೋಗು = TRUE;
		ನಿಲ್ಲಿಸು;
	    }

	    // Do we need this?  Probably not.
	    XSync(dpy, False);

	    // Wait for 1 msec to avoid that we eat up all CPU time.
	    ui_delay(1L, TRUE);
	}

	ಆದರೆ (success == TRUE)
	    ಹಿಂತಿರುಗಿ;

	// don't do a retry with another type after timing out, otherwise we
	// hang for 15 seconds.
	ಆದರೆ (timed_ಹೊರಹೋಗು)
	    ನಿಲ್ಲಿಸು;
    }

    // Final fallback position - use the X CUT_BUFFER0 store
    yank_cut_buffer0(dpy, cbd);
}

    ಶೂನ್ಯ
clip_x11_lose_selection(Widget myShell, Clipboard_T *cbd)
{
    XtDisownSelection(myShell, cbd->sel_atom,
				Xt_ಕೊನೆಯ_Timestamp_Processed(XtDisplay(myShell)));
}

    ಪೂರ್ಣಾಂಕ
clip_x11_own_selection(Widget myShell, Clipboard_T *cbd)
{
    // When using the GUI we have proper timestamps, use the one of the last
    // event.  When in the console we don't get events (the terminal gets
    // them), Get the time by a zero-length append, clip_x11_timestamp_cb will
    // be called with the current timestamp.
#  ifdef FEAT_GUI
    ಆದರೆ (gui.ಒಳ_use)
    {
	ಆದರೆ (XtOwnSelection(myShell, cbd->sel_atom,
	       Xt_ಕೊನೆಯ_Timestamp_Processed(XtDisplay(myShell)),
	       clip_x11_convert_selection_cb, clip_x11_lose_ownership_cb,
	       clip_x11_notify_cb) == False)
	    ಹಿಂತಿರುಗಿ FAIL;
    }
    ಇಲ್ಲದಿದ್ದರೆ
#  endif
    {
	ಆದರೆ (!XChangeProperty(XtDisplay(myShell), Xt_ಕಿಟಕಿ(myShell),
		  cbd->sel_atom, timestamp_atom, 32, Prop_ಬಹುಲಕ_Append, ಖಾಲಿ, 0))
	    ಹಿಂತಿರುಗಿ FAIL;
    }
    // Flush is required in a terminal as nothing else is doing it.
    XFlush(XtDisplay(myShell));
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Send the current selection to the clipboard.  Do nothing for X because we
 * will fill in the selection only when requested by another app.
 */
    ಶೂನ್ಯ
clip_x11_ಗಣ_selection(Clipboard_T *cbd ಬಳಸದ)
{
}

# endif

# ಆದರೆ (defined(FEAT_XCLIPBOARD) || defined(FEAT_GUI_X11) || defined(FEAT_GUI_GTK)) \
	&& !defined(USE_GTK4)
/*
 * Get the contents of the X CUT_BUFFER0 and put it in "cbd".
 */
    ಶೂನ್ಯ
yank_cut_buffer0(Display *dpy, Clipboard_T *cbd)
{
    ಪೂರ್ಣಾಂಕ		nbytes = 0;
    char_u	*ಬಫರ್ = (char_u *)XFetch_ಬಫರ್(dpy, &nbytes, 0);

    ಆದರೆ (nbytes > 0)
    {
	ಪೂರ್ಣಾಂಕ  ಮುಗಿದ = FALSE;

	// CUT_BUFFER0 is supposed to be always latin1.  Convert to 'enc' when
	// using a multi-byte encoding.  Conversion between two 8-bit
	// character sets usually fails and the text might actually be in
	// 'enc' anyway.
	ಆದರೆ (has_mbyte)
	{
	    char_u	*conv_buf;
	    vimconv_T	vc;

	    vc.vc_type = CONV_NONE;
	    ಆದರೆ (convert_ಸಿದ್ಧತೆ(&vc, (char_u *)"latin1", p_enc) == OK)
	    {
		conv_buf = string_convert(&vc, ಬಫರ್, &nbytes);
		ಆದರೆ (conv_buf != ಖಾಲಿ)
		{
		    clip_yank_selection(MCHAR, conv_buf, (ದೀರ್ಘ)nbytes, cbd);
		    vim_free(conv_buf);
		    ಮುಗಿದ = TRUE;
		}
		convert_ಸಿದ್ಧತೆ(&vc, ಖಾಲಿ, ಖಾಲಿ);
	    }
	}
	ಆದರೆ (!ಮುಗಿದ)  // use the text without conversion
	    clip_yank_selection(MCHAR, ಬಫರ್, (ದೀರ್ಘ)nbytes, cbd);
	XFree((ಶೂನ್ಯ *)ಬಫರ್);
	ಆದರೆ (p_verbose > 0)
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
# ಆದರೆ defined(FEAT_X11) && defined(FEAT_CLIPBOARD)
    ಶೂನ್ಯ
x11_export_final_selection(ಶೂನ್ಯ)
{
    Display	*dpy;
    char_u	*str = ಖಾಲಿ;
    long_u	len = 0;
    ಪೂರ್ಣಾಂಕ		motion_type = -1;

#  ifdef FEAT_GUI
    ಆದರೆ (gui.ಒಳ_use)
	dpy = X_DISPLAY;
    ಇಲ್ಲದಿದ್ದರೆ
#  endif
#  ifdef FEAT_XCLIPBOARD
	dpy = xterm_dpy;
#  ಇಲ್ಲದಿದ್ದರೆ
	ಹಿಂತಿರುಗಿ;
#  endif

    // Get selection to export
    ಆದರೆ (clip_plus.owned)
	motion_type = clip_convert_selection(&str, &len, &clip_plus);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (clip_ನಕ್ಷತ್ರ.owned)
	motion_type = clip_convert_selection(&str, &len, &clip_ನಕ್ಷತ್ರ);

    // Check it's OK
    ಆದರೆ (dpy != ಖಾಲಿ && str != ಖಾಲಿ && motion_type >= 0
					       && len < 1024*1024 && len > 0)
    {
	ಪೂರ್ಣಾಂಕ ok = TRUE;

	// The CUT_BUFFER0 is supposed to always contain latin1.  Convert from
	// 'enc' when it is a multi-byte encoding.  When 'enc' is an 8-bit
	// encoding conversion usually doesn't work, so keep the text as-is.
	ಆದರೆ (has_mbyte)
	{
	    vimconv_T	vc;

	    vc.vc_type = CONV_NONE;
	    ಆದರೆ (convert_ಸಿದ್ಧತೆ(&vc, p_enc, (char_u *)"latin1") == OK)
	    {
		ಪೂರ್ಣಾಂಕ	intlen = len;
		char_u	*conv_str;

		vc.vc_fail = TRUE;
		conv_str = string_convert(&vc, str, &intlen);
		len = intlen;
		ಆದರೆ (conv_str != ಖಾಲಿ)
		{
		    vim_free(str);
		    str = conv_str;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    ok = FALSE;
		}
		convert_ಸಿದ್ಧತೆ(&vc, ಖಾಲಿ, ಖಾಲಿ);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		ok = FALSE;
	    }
	}

	// Do not store the string if conversion failed.  Better to use any
	// other selection than garbled text.
	ಆದರೆ (ok)
	{
	    XStore_ಬಫರ್(dpy, (ಅಕ್ಷರ *)str, (ಪೂರ್ಣಾಂಕ)len, 0);
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

    ಆದರೆ (cbd == &clip_plus)
	ಗಣ_y_current(get_y_ರಿಜಿಸ್ಟರ್(PLUS_ರಿಜಿಸ್ಟರ್));
    ಇಲ್ಲದಿದ್ದರೆ
	ಗಣ_y_current(get_y_ರಿಜಿಸ್ಟರ್(ನಕ್ಷತ್ರ_ರಿಜಿಸ್ಟರ್));
    free_yank_all();
    get_y_current()->y_ಗಾತ್ರ = 0;
    ಗಣ_y_current(y_ptr);
}

/*
 * Get the selected text and put it in register '*' or '+'.
 */
    ಶೂನ್ಯ
clip_get_selection(Clipboard_T *cbd)
{
    yankreg_T	*old_y_ಹಿಂದಿನ, *old_y_current;
    pos_T	old_cursor;
    pos_T	old_visual;
    ಪೂರ್ಣಾಂಕ		old_visual_ಬಹುಲಕ;
    colnr_T	old_virtcol;
    colnr_T	old_curswant;
    ಪೂರ್ಣಾಂಕ		old_ಗಣ_curswant;
    pos_T	old_op_ಆರಂಭ, old_op_ಕೊನೆ;
    oparg_T	oa;
    cmdarg_T	ca;

    ಆದರೆ (cbd->owned)
    {
	ಆದರೆ ((cbd == &clip_plus
		&& get_y_ರಿಜಿಸ್ಟರ್(PLUS_ರಿಜಿಸ್ಟರ್)->y_array != ಖಾಲಿ)
		|| (cbd == &clip_ನಕ್ಷತ್ರ
		    && get_y_ರಿಜಿಸ್ಟರ್(ನಕ್ಷತ್ರ_ರಿಜಿಸ್ಟರ್)->y_array != ಖಾಲಿ))
	    ಹಿಂತಿರುಗಿ;

	// Avoid triggering autocmds such as TextYankPost.
	ಖಂಡ_autocmds();

	// Get the text between clip_star.start & clip_star.end
	old_y_ಹಿಂದಿನ = get_y_ಹಿಂದಿನ();
	old_y_current = get_y_current();
	old_cursor = curwin->w_cursor;
	old_virtcol = curwin->w_virtcol;
	old_curswant = curwin->w_curswant;
	old_ಗಣ_curswant = curwin->w_ಗಣ_curswant;
	old_op_ಆರಂಭ = curbuf->b_op_ಆರಂಭ;
	old_op_ಕೊನೆ = curbuf->b_op_ಕೊನೆ;
	old_visual = VIsual;
	old_visual_ಬಹುಲಕ = VIsual_ಬಹುಲಕ;
	clear_oparg(&oa);
	oa.regname = (cbd == &clip_plus ? '+' : '*');
	oa.op_type = OP_YANK;
	CLEAR_ಕ್ಷೇತ್ರ(ca);
	ca.oap = &oa;
	ca.cmdchar = 'y';
	ca.count1 = 1;
	ca.retval = CA_NO_ADJ_OP_ಕೊನೆ;
	do_pending_operator(&ca, 0, TRUE);

	// restore things
	ಗಣ_y_ಹಿಂದಿನ(old_y_ಹಿಂದಿನ);
	ಗಣ_y_current(old_y_current);
	curwin->w_cursor = old_cursor;
	curwin->w_virtcol = old_virtcol;
	ಬದಲಾಯಿತು_cline_bef_curs();   // old w_virtcol et al. may be invalid
	curwin->w_curswant = old_curswant;
	curwin->w_ಗಣ_curswant = old_ಗಣ_curswant;
	curbuf->b_op_ಆರಂಭ = old_op_ಆರಂಭ;
	curbuf->b_op_ಕೊನೆ = old_op_ಕೊನೆ;
	VIsual = old_visual;
	VIsual_ಬಹುಲಕ = old_visual_ಬಹುಲಕ;

	unblock_autocmds();
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!is_clipboard_needs_update())
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
    ಪೂರ್ಣಾಂಕ		type,
    char_u	*str,
    ದೀರ್ಘ	len,
    Clipboard_T *cbd)
{
    yankreg_T *y_ptr;

    ಆದರೆ (cbd == &clip_plus)
	y_ptr = get_y_ರಿಜಿಸ್ಟರ್(PLUS_ರಿಜಿಸ್ಟರ್);
    ಇಲ್ಲದಿದ್ದರೆ
	y_ptr = get_y_ರಿಜಿಸ್ಟರ್(ನಕ್ಷತ್ರ_ರಿಜಿಸ್ಟರ್);

    clip_free_selection(cbd);

    str_ಗಮ್ಯ_reg(y_ptr, type, str, len, -1, FALSE);
}

    ಪೂರ್ಣಾಂಕ
clip_convert_selection_ಸ್ಥಾನಾಂತರ(
	char_u	    **str,
	long_u	    *len,
	ಪೂರ್ಣಾಂಕ	    ಸ್ಥಾನಾಂತರ, // Extra space to add in *str and the offset to
			    // place the actual string in *str.
	Clipboard_T *cbd)
{
    char_u	*p;
    ಪೂರ್ಣಾಂಕ		lnum;
    ಪೂರ್ಣಾಂಕ		i, j;
    int_u	eolsize;
    yankreg_T	*y_ptr;

    ಆದರೆ (cbd == &clip_plus)
	y_ptr = get_y_ರಿಜಿಸ್ಟರ್(PLUS_ರಿಜಿಸ್ಟರ್);
    ಇಲ್ಲದಿದ್ದರೆ
	y_ptr = get_y_ರಿಜಿಸ್ಟರ್(ನಕ್ಷತ್ರ_ರಿಜಿಸ್ಟರ್);

# ifdef USE_CRNL
    eolsize = 2;
# ಇಲ್ಲದಿದ್ದರೆ
    eolsize = 1;
# endif

    *str = ಖಾಲಿ;
    *len = 0;
    ಆದರೆ (y_ptr->y_array == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ -1;

    ಗಾಗಿ (i = 0; i < y_ptr->y_ಗಾತ್ರ; i++)
	*len += (long_u)y_ptr->y_array[i].ಉದ್ದಳತೆ + eolsize;

    // Don't want newline character at end of last line if we're in MCHAR mode.
    ಆದರೆ (y_ptr->y_type == MCHAR && *len >= eolsize)
	*len -= eolsize;

    *len += ಸ್ಥಾನಾಂತರ;
    p = *str = alloc(*len + 1);	// add one to avoid zero
    ಆದರೆ (p == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ -1;
    p += ಸ್ಥಾನಾಂತರ;
    lnum = 0;
    ಗಾಗಿ (i = 0, j = 0; i < (ಪೂರ್ಣಾಂಕ)*len - ಸ್ಥಾನಾಂತರ; i++, j++)
    {
	ಆದರೆ (y_ptr->y_array[lnum].string[j] == '\n')
	    p[i] = NUL;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (y_ptr->y_array[lnum].string[j] == NUL)
	{
# ifdef USE_CRNL
	    p[i++] = '\r';
# endif
	    p[i] = '\n';
	    lnum++;
	    j = -1;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    p[i] = y_ptr->y_array[lnum].string[j];
    }
    ಹಿಂತಿರುಗಿ y_ptr->y_type;
}

/*
 * Convert the '*'/'+' register into a GUI selection string returned in *str
 * with length *len.
 * Returns the motion type, or -1 for failure.
 */
    ಪೂರ್ಣಾಂಕ
clip_convert_selection(char_u **str, long_u *len, Clipboard_T *cbd)
{
    ಹಿಂತಿರುಗಿ clip_convert_selection_ಸ್ಥಾನಾಂತರ(str, len, 0, cbd);
}

/*
 * When "regname" is a clipboard register, obtain the selection.  If it's not
 * available return zero, otherwise return "regname".
 */
    ಪೂರ್ಣಾಂಕ
may_get_selection(ಪೂರ್ಣಾಂಕ regname)
{
    ಆದರೆ (regname == '*')
    {
	ಆದರೆ (!clip_ನಕ್ಷತ್ರ.ಲಭ್ಯ)
	    regname = 0;
	ಇಲ್ಲದಿದ್ದರೆ
	    clip_get_selection(&clip_ನಕ್ಷತ್ರ);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (regname == '+')
    {
	ಆದರೆ (!clip_plus.ಲಭ್ಯ)
	    regname = 0;
	ಇಲ್ಲದಿದ್ದರೆ
	    clip_get_selection(&clip_plus);
    }
    ಹಿಂತಿರುಗಿ regname;
}

/*
 * If we have written to a clipboard register, send the text to the clipboard.
 */
    ಶೂನ್ಯ
may_ಗಣ_selection(ಶೂನ್ಯ)
{
    ಆದರೆ ((get_y_current() == get_y_ರಿಜಿಸ್ಟರ್(ನಕ್ಷತ್ರ_ರಿಜಿಸ್ಟರ್))
	    && clip_ನಕ್ಷತ್ರ.ಲಭ್ಯ)
    {
	clip_own_selection(&clip_ನಕ್ಷತ್ರ);
	clip_gen_ಗಣ_selection(&clip_ನಕ್ಷತ್ರ);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((get_y_current() == get_y_ರಿಜಿಸ್ಟರ್(PLUS_ರಿಜಿಸ್ಟರ್))
	    && clip_plus.ಲಭ್ಯ)
    {
	clip_own_selection(&clip_plus);
	clip_gen_ಗಣ_selection(&clip_plus);
    }
}

# ಆದರೆ defined(FEAT_WAYLAND_CLIPBOARD)

    ಸ್ಥಿರ clip_wl_selection_T *
clip_wl_get_selection(wayland_selection_T sel)
{
    ಆಯ್ಕೆ (sel)
    {
	ಸ್ಥಿತಿ WAYLAND_SELECTION_REGULAR:
	    ಹಿಂತಿರುಗಿ &clip_wl.regular;
	ಸ್ಥಿತಿ WAYLAND_SELECTION_PRIMARY:
	    ಹಿಂತಿರುಗಿ &clip_wl.primary;
	ಪೂರ್ವನಿಯೋಜಿತ:
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
}

    ಸ್ಥಿರ clip_wl_selection_T *
clip_wl_get_selection_from_cbd(Clipboard_T *cbd)
{
    ಆದರೆ (cbd == &clip_plus)
	ಹಿಂತಿರುಗಿ &clip_wl.regular;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (cbd == &clip_ನಕ್ಷತ್ರ)
	ಹಿಂತಿರುಗಿ &clip_wl.primary;
    ಇಲ್ಲದಿದ್ದರೆ
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

    ಸ್ಥಿರ Clipboard_T *
clip_wl_get_cbd_from_selection(clip_wl_selection_T *sel)
{
    ಆದರೆ (sel == &clip_wl.regular)
	ಹಿಂತಿರುಗಿ &clip_plus;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (sel == &clip_wl.primary)
	ಹಿಂತಿರುಗಿ &clip_ನಕ್ಷತ್ರ;
    ಇಲ್ಲದಿದ್ದರೆ
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

    ಸ್ಥಿರ wayland_selection_T
clip_wl_get_selection_type(clip_wl_selection_T *sel)
{
    ಆದರೆ (sel == &clip_wl.regular)
	ಹಿಂತಿರುಗಿ WAYLAND_SELECTION_REGULAR;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (sel == &clip_wl.primary)
	ಹಿಂತಿರುಗಿ WAYLAND_SELECTION_PRIMARY;
    ಇಲ್ಲದಿದ್ದರೆ
	ಹಿಂತಿರುಗಿ WAYLAND_SELECTION_NONE;
}

    ಸ್ಥಿರ bool
wl_ದತ್ತಾಂಶ_offer_listener_event_offer(
    ಶೂನ್ಯ *ದತ್ತಾಂಶ ಬಳಸದ,
    vwl_ದತ್ತಾಂಶ_offer_T *offer ಬಳಸದ,
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *mime_type
)
{
    // Only accept mime type if we support it
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < (ಪೂರ್ಣಾಂಕ)ARRAY_ಉದ್ದಳತೆ(supported_mimes); i++)
	ಆದರೆ (STRCMP(mime_type, supported_mimes[i]) == 0)
	    ಹಿಂತಿರುಗಿ true;
    ಹಿಂತಿರುಗಿ FALSE;
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ vwl_ದತ್ತಾಂಶ_offer_listener_T vwl_ದತ್ತಾಂಶ_offer_listener = {
    .offer = wl_ದತ್ತಾಂಶ_offer_listener_event_offer
};

    ಸ್ಥಿರ ಶೂನ್ಯ
vwl_ದತ್ತಾಂಶ_device_listener_event_ದತ್ತಾಂಶ_offer(
	ಶೂನ್ಯ *ದತ್ತಾಂಶ ಬಳಸದ,
	vwl_ದತ್ತಾಂಶ_device_T *device ಬಳಸದ,
	vwl_ದತ್ತಾಂಶ_offer_T *offer)
{
    // Immediately start listening for offer events from the data offer
    vwl_ದತ್ತಾಂಶ_offer_ಕೂಡಿಸು_listener(offer, &vwl_ದತ್ತಾಂಶ_offer_listener, ಖಾಲಿ);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
vwl_ದತ್ತಾಂಶ_device_listener_event_selection(
	ಶೂನ್ಯ *ದತ್ತಾಂಶ ಬಳಸದ,
	vwl_ದತ್ತಾಂಶ_device_T *device ಬಳಸದ,
	vwl_ದತ್ತಾಂಶ_offer_T *offer,
	wayland_selection_T selection)
{
    clip_wl_selection_T *sel = clip_wl_get_selection(selection);

    // Destroy previous offer if any, it is now invalid
    vwl_ದತ್ತಾಂಶ_offer_destroy(sel->offer);

    // There are two cases when sel->offer is NULL
    // 1. No one owns the selection
    // 2. We own the selection (we'll just access the register directly)
    ಆದರೆ (offer == ಖಾಲಿ || sel->ಮೂಲ != ಖಾಲಿ)
    {
	// Selection event is from us, so we are the source client. Therefore
	// ignore it. Or the selection is cleared, so set sel->offer to NULL
	vwl_ದತ್ತಾಂಶ_offer_destroy(offer);
	sel->offer = ಖಾಲಿ;
	ಹಿಂತಿರುಗಿ;
    }

    // Save offer. When we want to request data, then we'll actually call the
    // receive method.
    sel->offer = offer;

}

    ಸ್ಥಿರ ಶೂನ್ಯ
vwl_ದತ್ತಾಂಶ_device_listener_event_ಪೂರ್ಣಗೊಂಡ(
	ಶೂನ್ಯ *ದತ್ತಾಂಶ ಬಳಸದ,
	vwl_ದತ್ತಾಂಶ_device_T *device)
{
    clip_wl_selection_T *sel;
    // Device finished, guessing this can happen is when the seat becomes
    // invalid? If so, let the user call :wlrestore! to reset. There wouldn't be
    // any point in trying to create another data device for the same seat,
    // since the seat is in an invalid state.
    ಆದರೆ (device == clip_wl.regular.device)
    {
	sel = &clip_wl.regular;
	clip_wl.regular.device = ಖಾಲಿ;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (device == clip_wl.primary.device)
    {
	sel = &clip_wl.primary;
	clip_wl.primary.device = ಖಾಲಿ;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	// Shouldn't happen
	ಹಿಂತಿರುಗಿ;

    vim_free(sel->contents);
    vwl_ದತ್ತಾಂಶ_ಮೂಲ_destroy(sel->ಮೂಲ);
    vwl_ದತ್ತಾಂಶ_offer_destroy(sel->offer);
    sel->ಲಭ್ಯ = FALSE;

    vwl_ದತ್ತಾಂಶ_device_destroy(device);
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ vwl_ದತ್ತಾಂಶ_device_listener_T vwl_ದತ್ತಾಂಶ_device_listener = {
    .ದತ್ತಾಂಶ_offer = vwl_ದತ್ತಾಂಶ_device_listener_event_ದತ್ತಾಂಶ_offer,
    .selection = vwl_ದತ್ತಾಂಶ_device_listener_event_selection,
    .ಪೂರ್ಣಗೊಂಡ = vwl_ದತ್ತಾಂಶ_device_listener_event_ಪೂರ್ಣಗೊಂಡ
};

/*
 * Initialize the clipboard for Wayland using the global Wayland connection.
 * Returns OK on success and FAIL on failure.
 */
    ಪೂರ್ಣಾಂಕ
clip_init_wayland(ಶೂನ್ಯ)
{
    int_u supported = WAYLAND_SELECTION_NONE;

    ಆದರೆ (wayland_ct == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;

    memset(&clip_wl, 0, ಗಾತ್ರ(clip_wl));

    clip_wl.seat = vwl_connection_get_seat(wayland_ct, (ಅಕ್ಷರ *)p_wse);

    ಆದರೆ (clip_wl.seat == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;

    clip_wl.regular.manager = vwl_connection_get_ದತ್ತಾಂಶ_device_manager(
	    wayland_ct, WAYLAND_SELECTION_REGULAR, &supported);

    ಆದರೆ (clip_wl.regular.manager != ಖಾಲಿ)
    {
	clip_wl.regular.device = vwl_ದತ್ತಾಂಶ_device_manager_get_ದತ್ತಾಂಶ_device(
		clip_wl.regular.manager, clip_wl.seat);

	ಆದರೆ (clip_wl.regular.device != ಖಾಲಿ)
	    clip_wl.regular.ಲಭ್ಯ = true;
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    // Shouldn't happen
	    vwl_ದತ್ತಾಂಶ_device_manager_discard(clip_wl.regular.manager);
	    clip_wl.regular.manager = ಖಾಲಿ;
	    ಹಿಂತಿರುಗಿ FAIL;
	}
    }

    // If we still don't support the primary selection, try finding one for it
    // specifically.
    ಆದರೆ (!(supported & WAYLAND_SELECTION_PRIMARY))
    {
	clip_wl.primary.manager = vwl_connection_get_ದತ್ತಾಂಶ_device_manager(
		wayland_ct, WAYLAND_SELECTION_PRIMARY, &supported);

	ಆದರೆ (clip_wl.primary.manager != ಖಾಲಿ)
	{
	    clip_wl.primary.device = vwl_ದತ್ತಾಂಶ_device_manager_get_ದತ್ತಾಂಶ_device(
		    clip_wl.primary.manager, clip_wl.seat);

	    ಆದರೆ (clip_wl.primary.device != ಖಾಲಿ)
		clip_wl.primary.ಲಭ್ಯ = true;
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		vwl_ದತ್ತಾಂಶ_device_manager_discard(clip_wl.primary.manager);
		clip_wl.primary.manager = ಖಾಲಿ;
	    }
	}
    }

    ಆದರೆ (clip_wl.regular.ಲಭ್ಯ && !clip_wl.primary.ಲಭ್ಯ)
    {
	// The protocol supports both regular and primary selections, just use
	// one data device manager and one data device. Or the primary selection
	// is not supported, make it point to the regular selection instead.
	clip_wl.primary.ಲಭ್ಯ = true;
	clip_wl.primary.manager = clip_wl.regular.manager;
	clip_wl.primary.device = clip_wl.regular.device;
    }

    ಆದರೆ (!clip_wl.regular.ಲಭ್ಯ && !clip_wl.primary.ಲಭ್ಯ)
	ಹಿಂತಿರುಗಿ FAIL;

    // Start listening for selection updates
    ಆದರೆ (clip_wl.regular.device != ಖಾಲಿ)
	vwl_ದತ್ತಾಂಶ_device_ಕೂಡಿಸು_listener(clip_wl.regular.device,
		&vwl_ದತ್ತಾಂಶ_device_listener, ಖಾಲಿ);
    // Don't want to listen to the same data device twice
    ಆದರೆ (clip_wl.primary.device != ಖಾಲಿ
	    && clip_wl.primary.device != clip_wl.regular.device)
	vwl_ದತ್ತಾಂಶ_device_ಕೂಡಿಸು_listener(clip_wl.primary.device,
		&vwl_ದತ್ತಾಂಶ_device_listener, ಖಾಲಿ);

    ಹಿಂತಿರುಗಿ OK;
}

    ಶೂನ್ಯ
clip_uninit_wayland(ಶೂನ್ಯ)
{
    clip_wl_selection_T *sel;

    ಆದರೆ (clipmethod == CLIPMETHOD_WAYLAND)
    {
	ಆದರೆ (clip_ನಕ್ಷತ್ರ.owned)
	    clip_lose_selection(&clip_ನಕ್ಷತ್ರ);
	ಆದರೆ (clip_plus.owned)
	    clip_lose_selection(&clip_plus);
    }

    // Don't want to double free
    ಆದರೆ (clip_wl.regular.manager != clip_wl.primary.manager)
	vwl_ದತ್ತಾಂಶ_device_manager_discard(clip_wl.primary.manager);
    vwl_ದತ್ತಾಂಶ_device_manager_discard(clip_wl.regular.manager);

    ಆದರೆ (clip_wl.regular.device != clip_wl.primary.device)
	vwl_ದತ್ತಾಂಶ_device_destroy(clip_wl.primary.device);
    vwl_ದತ್ತಾಂಶ_device_destroy(clip_wl.regular.device);

    sel = &clip_wl.regular;
    ಆಗ (true)
    {
	vim_free(sel->contents);
	vwl_ದತ್ತಾಂಶ_ಮೂಲ_destroy(sel->ಮೂಲ);
	vwl_ದತ್ತಾಂಶ_offer_destroy(sel->offer);
	sel->ಲಭ್ಯ = false;

	ಆದರೆ (sel == &clip_wl.primary)
	    ನಿಲ್ಲಿಸು;
	sel = &clip_wl.primary;
    }

    vim_memset(&clip_wl, 0, ಗಾತ್ರ(clip_wl));
}

    ಪೂರ್ಣಾಂಕ
clip_reset_wayland(ಶೂನ್ಯ)
{
    wayland_uninit_connection();

    ಆದರೆ (wayland_init_connection(wayland_display_name) == FAIL
	    || clip_init_wayland() == FAIL)
	ಹಿಂತಿರುಗಿ FAIL;

    ಆರಿಸು_clipmethod();
    ಹಿಂತಿರುಗಿ OK;
}


/*
 * Read data from a file descriptor and write it to the given clipboard.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
clip_wl_receive_ದತ್ತಾಂಶ(Clipboard_T *cbd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *mime_type, ಪೂರ್ಣಾಂಕ fd)
{
    char_u	*ಆರಂಭ, *final;
    ದೀರ್ಘ	len;
    garray_T	buf;
    ಪೂರ್ಣಾಂಕ		motion_type = MAUTO;
    ssize_t	r = 0;
    char_u	*tofree = ಖಾಲಿ;
#  ifndef HAVE_SELECT
    ರಚನೆ pollfd   pfd;

    pfd.fd = fd;
    pfd.events = POLLIN;
#  ಇಲ್ಲದಿದ್ದರೆ
    fd_set rfds;
    ರಚನೆ timeval  tv;

    FD_ZERO(&rfds);
    FD_SET(fd, &rfds);
#  endif

    // Make pipe (read end) non-blocking
    ಆದರೆ (fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK) == -1)
	ಹಿಂತಿರುಗಿ;

    ga_init2(&buf, 1, 4096);

    // 4096 bytes seems reasonable for initial buffer size, memory is cheap
    // anyways.
    ಆದರೆ (ga_grow(&buf, 4096) == FAIL)
	ಹಿಂತಿರುಗಿ;

    ಆರಂಭ = buf.ga_ದತ್ತಾಂಶ;

#  ifndef HAVE_SELECT
    ಆಗ (poll(&pfd, 1, p_wtm) > 0)
#  ಇಲ್ಲದಿದ್ದರೆ
    ಆಗ (tv.tv_sec = p_wtm / 1000, tv.tv_usec = (p_wtm % 1000) * 1000,
	    select(fd + 1, &rfds, ಖಾಲಿ, ಖಾಲಿ, &tv) > 0)
#  endif
    {
	r = ಪಠ(fd, ಆರಂಭ, buf.ga_maxlen - 1 - buf.ga_len);

	ಆದರೆ (r == 0)
	    ನಿಲ್ಲಿಸು;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (r < 0)
	{
	    ಆದರೆ (errno == EAGAIN || errno == EINTR)
		ಮುಂದುವರಿ;
	    ನಿಲ್ಲಿಸು;
	}

	ಆರಂಭ += r;
	buf.ga_len += r;

	// Realloc if we are at the end of the buffer
	ಆದರೆ (buf.ga_len >= buf.ga_maxlen - 1)
	{
	    ಆದರೆ (ga_grow(&buf, 8192) == FAIL)
		ನಿಲ್ಲಿಸು;
	    ಆರಂಭ = (char_u *)buf.ga_ದತ್ತಾಂಶ + buf.ga_len;
	}
    }

    ಆದರೆ (buf.ga_len == 0)
    {
	clip_free_selection(cbd); // Nothing received, clear register
	ga_clear(&buf);
	ಹಿಂತಿರುಗಿ;
    }

    final = buf.ga_ದತ್ತಾಂಶ;
    len = buf.ga_len;

    ಆದರೆ (clip_convert_ದತ್ತಾಂಶ(&final, &len, &motion_type,
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
    ಪೂರ್ಣಾಂಕ			fds[2];
    ಪೂರ್ಣಾಂಕ			mime_types_len;
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ		**mime_types;
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ		*chosen_mime = ಖಾಲಿ;

    ಆದರೆ (!sel->ಲಭ್ಯ)
	ಹೋಗು clear;

    // Dispatch any events that still queued up before checking for a data
    // offer.
    ಆದರೆ (vwl_connection_roundtrip(wayland_ct) == FAIL)
	ಹೋಗು clear;

    ಆದರೆ (sel->offer == ಖಾಲಿ)
	ಹೋಗು clear;

    mime_types_len = sel->offer->mime_types.ga_len;
    mime_types = sel->offer->mime_types.ga_ದತ್ತಾಂಶ;

    // Choose mime type to receive from. Mime types with a lower index in the
    // "supported_mimes" array are prioritized over ones after it.
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < (ಪೂರ್ಣಾಂಕ)ARRAY_ಉದ್ದಳತೆ(supported_mimes)
	    && chosen_mime == ಖಾಲಿ; i++)
    {
	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ k = 0; k < mime_types_len && chosen_mime == ಖಾಲಿ; k++)
	    ಆದರೆ (STRCMP(mime_types[k], supported_mimes[i]) == 0)
		chosen_mime = supported_mimes[i];
    }

    ಆದರೆ (chosen_mime == ಖಾಲಿ || pipe(fds) == -1)
	ಹೋಗು clear;

    vwl_ದತ್ತಾಂಶ_offer_receive(sel->offer, chosen_mime, fds[1]);

    close(fds[1]); // Close before we read data so that when the source client
		   // closes their end we receive an EOF.

    ಆದರೆ (vwl_connection_flush(wayland_ct) >= 0)
	clip_wl_receive_ದತ್ತಾಂಶ(cbd, chosen_mime, fds[0]);

    close(fds[0]);

    ಹಿಂತಿರುಗಿ;
clear:
    clip_free_selection(cbd);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
vwl_ದತ್ತಾಂಶ_ಮೂಲ_listener_event_send(
    ಶೂನ್ಯ *ದತ್ತಾಂಶ,
    vwl_ದತ್ತಾಂಶ_ಮೂಲ_T *ಮೂಲ ಬಳಸದ,
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *mime_type,
    int32_t fd
)
{
    clip_wl_selection_T *sel = ದತ್ತಾಂಶ;
    Clipboard_T		*cbd = clip_wl_get_cbd_from_selection(sel);
    bool		have_mime = false;
    ಪೂರ್ಣಾಂಕ			motion_type;
    long_u		ಉದ್ದಳತೆ;
    char_u		*string; // Will be reallocated to a bigger size if
				 // needed.
    ಪೂರ್ಣಾಂಕ			ಸ್ಥಾನಾಂತರ = 0;
    bool		is_vim, is_vimenc;
    size_t		ಒಟ್ಟು = 0;
#  ifndef HAVE_SELECT
    ರಚನೆ pollfd   pfd;

    pfd.fd = fd;
    pfd.events = POLLOUT;
#  ಇಲ್ಲದಿದ್ದರೆ
    fd_set	    wfds;
    ರಚನೆ timeval  tv;

    FD_ZERO(&wfds);
    FD_SET(fd, &wfds);
#  endif

    // Check if we actually have mime type
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < (ಪೂರ್ಣಾಂಕ)ARRAY_ಉದ್ದಳತೆ(supported_mimes); i++)
	ಆದರೆ (STRCMP(supported_mimes[i], mime_type) == 0)
	{
	    have_mime = true;
	    ನಿಲ್ಲಿಸು;
	}

    ಆದರೆ (!have_mime)
	ಹೋಗು ನಿರ್ಗಮಿಸು;

    // First byte sent is motion type for vim specific formats. For the vimenc
    // format, after the first byte is the encoding type, which is null
    // terminated.

    is_vimenc = STRCMP(mime_type, VIMENC_ATOM_NAME) == 0
	|| STRCMP(mime_type, VIMENC_MIMETYPE_NAME) == 0;
    is_vim = STRCMP(mime_type, VIM_ATOM_NAME) == 0
	|| STRCMP(mime_type, VIM_MIMETYPE_NAME) == 0;

    ಆದರೆ (is_vimenc)
	ಸ್ಥಾನಾಂತರ += 2 + STRLEN(p_enc);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (is_vim)
	ಸ್ಥಾನಾಂತರ += 1;

    clip_get_selection(cbd);
    motion_type = clip_convert_selection_ಸ್ಥಾನಾಂತರ(&string, &ಉದ್ದಳತೆ, ಸ್ಥಾನಾಂತರ, cbd);

    ಆದರೆ (motion_type < 0)
	ಹೋಗು ನಿರ್ಗಮಿಸು;

    ಆದರೆ (is_vimenc)
    {
	string[0] = (char_u)motion_type;
	// Use vim_strncpy for safer copying
	vim_strncpy(string + 1, p_enc, STRLEN(p_enc));
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (is_vim)
	string[0] = (char_u)motion_type;


    ಆಗ (ಒಟ್ಟು < (size_t)ಉದ್ದಳತೆ &&
#  ifndef HAVE_SELECT
	    poll(&pfd, 1, p_wtm) > 0)
#  ಇಲ್ಲದಿದ್ದರೆ
	    ((tv.tv_sec = p_wtm / 1000, tv.tv_usec = (p_wtm % 1000) * 1000),
	    select(fd + 1, ಖಾಲಿ, &wfds, ಖಾಲಿ, &tv) > 0))
#  endif
    {
	ssize_t w = ಲಿಖ(fd, string + ಒಟ್ಟು, ಉದ್ದಳತೆ - ಒಟ್ಟು);

	ಆದರೆ (w == -1)
	    ನಿಲ್ಲಿಸು;
	ಒಟ್ಟು += w;
    }

    vim_free(string);
ನಿರ್ಗಮಿಸು:
    close(fd);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
vwl_ದತ್ತಾಂಶ_ಮೂಲ_listener_event_cancelled(
	ಶೂನ್ಯ *ದತ್ತಾಂಶ,
	vwl_ದತ್ತಾಂಶ_ಮೂಲ_T *ಮೂಲ ಬಳಸದ)
{
    clip_wl_selection_T *sel = ದತ್ತಾಂಶ;
    Clipboard_T		*cbd = clip_wl_get_cbd_from_selection(sel);

    clip_lose_selection(cbd);
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ vwl_ದತ್ತಾಂಶ_ಮೂಲ_listener_T vwl_ದತ್ತಾಂಶ_ಮೂಲ_listener = {
    .send = vwl_ದತ್ತಾಂಶ_ಮೂಲ_listener_event_send,
    .cancelled = vwl_ದತ್ತಾಂಶ_ಮೂಲ_listener_event_cancelled
};

    ಸ್ಥಿರ ಶೂನ್ಯ
clip_wl_do_ಗಣ_selection(ಶೂನ್ಯ *ದತ್ತಾಂಶ, uint32_t serial)
{
    clip_wl_selection_T *sel = ದತ್ತಾಂಶ;
    wayland_selection_T sel_type = clip_wl_get_selection_type(sel);

    vwl_ದತ್ತಾಂಶ_device_ಗಣ_selection(sel->device, sel->ಮೂಲ, serial, sel_type);

    sel->own_success = (vwl_connection_roundtrip(wayland_ct) == OK);
}

/*
 * Own the selection that cbd corresponds to. Start listening for requests from
 * other Wayland clients so they can receive data from us. Returns OK on success
 * and FAIL on failure.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
clip_wl_own_selection(Clipboard_T *cbd)
{
    clip_wl_selection_T *sel = clip_wl_get_selection_from_cbd(cbd);
    wayland_selection_T sel_type = clip_wl_get_selection_type(sel);

    ಆದರೆ (!sel->ಲಭ್ಯ || vwl_connection_roundtrip(wayland_ct) == FAIL)
	ಹಿಂತಿರುಗಿ FAIL;

    ಆದರೆ (sel->ಮೂಲ != ಖಾಲಿ)
    {
	ಆದರೆ (sel_type == WAYLAND_SELECTION_PRIMARY)
	    // We already own the selection, ignore (only do this for primary
	    // selection). We don't re set the selection because then we would
	    // be setting the selection every time the user moves the visual
	    // selection cursor, which is messy and inefficient. Some
	    // applications like Google Chrome do it this way however.
	    ಹಿಂತಿರುಗಿ OK;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (sel_type == WAYLAND_SELECTION_REGULAR)
	{
	    // Technically we don't need to do this as we already own the
	    // selection, however if a user yanks text a second time, the
	    // text yanked won't appear in their clipboard manager if they are
	    // using one.
	    //
	    // This can be unexpected behaviour for the user so its probably
	    // better to do it this way. Additionally other Wayland applications
	    // seem to set the selection every time.
	    vwl_ದತ್ತಾಂಶ_ಮೂಲ_destroy(sel->ಮೂಲ);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    // Shouldn't happen
	    ಹಿಂತಿರುಗಿ FAIL;
    }

    sel->ಮೂಲ = vwl_ದತ್ತಾಂಶ_device_manager_create_ದತ್ತಾಂಶ_ಮೂಲ(sel->manager);
    vwl_ದತ್ತಾಂಶ_ಮೂಲ_ಕೂಡಿಸು_listener(sel->ಮೂಲ, &vwl_ದತ್ತಾಂಶ_ಮೂಲ_listener, sel);

    // Advertise mime types
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < (ಪೂರ್ಣಾಂಕ)ARRAY_ಉದ್ದಳತೆ(supported_mimes); i++)
	vwl_ದತ್ತಾಂಶ_ಮೂಲ_offer(sel->ಮೂಲ, supported_mimes[i]);

    sel->own_success = false;
    clip_wl_do_ಗಣ_selection(sel, 0);

    ಆದರೆ (!sel->own_success)
	ಹೋಗು fail;

    ಹಿಂತಿರುಗಿ OK;
fail:
    vwl_ದತ್ತಾಂಶ_ಮೂಲ_destroy(sel->ಮೂಲ);
    sel->ಮೂಲ = ಖಾಲಿ;
    ಹಿಂತಿರುಗಿ FAIL;
}

/*
 * Disown the selection that cbd corresponds to.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
clip_wl_lose_selection(Clipboard_T *cbd)
{
    clip_wl_selection_T *sel = clip_wl_get_selection_from_cbd(cbd);

    ಆದರೆ (!sel->ಲಭ್ಯ)
	ಹಿಂತಿರುಗಿ;

    vwl_ದತ್ತಾಂಶ_ಮೂಲ_destroy(sel->ಮೂಲ);
    sel->ಮೂಲ = ಖಾಲಿ;
}

/*
 * Send the current selection to the clipboard. Do nothing for Wayland because
 * we will fill in the selection only when requested by another client.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
clip_wl_ಗಣ_selection(Clipboard_T *cbd ಬಳಸದ)
{
}

#  ಆದರೆ defined(USE_SYSTEM)
/*
 * Return true if we own the selection corresponding to cbd or another client
 * does.
 */
    ಸ್ಥಿರ bool
clip_wl_owner_exists(Clipboard_T *cbd)
{
    clip_wl_selection_T *sel = clip_wl_get_selection_from_cbd(cbd);

    ಆದರೆ (vwl_connection_roundtrip(wayland_ct) == FAIL)
	ಹಿಂತಿರುಗಿ false;

    ಹಿಂತಿರುಗಿ sel->ಲಭ್ಯ && (sel->ಮೂಲ != ಖಾಲಿ || sel->offer != ಖಾಲಿ);
}
#  endif

# endif // FEAT_WAYLAND_CLIPBOARD

#ಆದರೆ_ಅಂತ್ಯ // FEAT_CLIPBOARD

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_CLIPMETHOD

/*
 * Returns the first method for accessing the clipboard that is available/works,
 * depending on the order of values in str.
 */
    ಸ್ಥಿರ clipmethod_T
get_clipmethod(char_u *str)
{
    ಪೂರ್ಣಾಂಕ		len	= (ಪೂರ್ಣಾಂಕ)STRLEN(str) + 1;
    char_u	*buf	= alloc(len);

    ಆದರೆ (buf == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ CLIPMETHOD_FAIL;

    clipmethod_T ret = CLIPMETHOD_FAIL;
    char_u	*p = str;

    ಆಗ (*p != NUL)
    {
	clipmethod_T ವಿಧಾನ = CLIPMETHOD_NONE;

	(ಶೂನ್ಯ)copy_option_part(&p, buf, len, ",");

	ಆದರೆ (STRCMP(buf, "wayland") == 0)
	{
# ifdef FEAT_GUI
	    ಆದರೆ (!gui.ಒಳ_use)
# endif
	    {
# ifdef FEAT_WAYLAND_CLIPBOARD
		ಆದರೆ (clip_wl.regular.ಲಭ್ಯ || clip_wl.primary.ಲಭ್ಯ)
		    ವಿಧಾನ = CLIPMETHOD_WAYLAND;
# endif
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(buf, "x11") == 0)
	{
# ifdef FEAT_GUI
	    ಆದರೆ (!gui.ಒಳ_use)
# endif
	    {
# ifdef FEAT_XCLIPBOARD
		// x_IOerror_handler() in os_unix.c should set xterm_dpy to NULL if
		// we lost connection to the X server.
		ಆದರೆ (xterm_dpy != ಖಾಲಿ)
		{
		    // If the X connection is lost then that handler will longjmp
		    // somewhere else, in that case we will call choose_clipmethod()
		    // again from there, and this if block won't be executed since
		    // xterm_dpy will be set to NULL.
		    xterm_update();
		    ವಿಧಾನ = CLIPMETHOD_X11;
		}
# endif
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
# ifdef FEAT_CLIPBOARD_PROVIDER
	    // Check if name matches a clipboard provider
	    ಪೂರ್ಣಾಂಕ r = clip_provider_is_ಲಭ್ಯ(buf);

	    ಆದರೆ (r == 1)
	    {
		ವಿಧಾನ = CLIPMETHOD_PROVIDER;
		ಆದರೆ (ret == CLIPMETHOD_FAIL)
		{
		    vim_free(clip_provider);
		    clip_provider = vim_strsave(buf);
		    ಆದರೆ (clip_provider == ಖಾಲಿ)
			ಹೋಗು fail;
		}
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (r == -1)
# endif
	    {
# ifdef FEAT_CLIPBOARD_PROVIDER
fail:
# endif
		ret = CLIPMETHOD_FAIL;
		ಹೋಗು ನಿರ್ಗಮಿಸು;
	    }
	}

	// Keep on going in order to catch errors
	ಆದರೆ (ವಿಧಾನ != CLIPMETHOD_NONE && ret == CLIPMETHOD_FAIL)
	    ret = ವಿಧಾನ;
    }

    // No match found, use "none".
    ret = (ret == CLIPMETHOD_FAIL) ? CLIPMETHOD_NONE : ret;

ನಿರ್ಗಮಿಸು:
    vim_free(buf);
    ಹಿಂತಿರುಗಿ ret;
}


/*
 * Returns name of clipmethod in a statically allocated string.
 */
    ಸ್ಥಿರ char_u *
clipmethod_ಗಮ್ಯ_str(clipmethod_T ವಿಧಾನ)
{
    ಆಯ್ಕೆ(ವಿಧಾನ)
    {
	ಸ್ಥಿತಿ CLIPMETHOD_WAYLAND:
	    ಹಿಂತಿರುಗಿ (char_u *)"wayland";
	ಸ್ಥಿತಿ CLIPMETHOD_X11:
	    ಹಿಂತಿರುಗಿ (char_u *)"x11";
	ಸ್ಥಿತಿ CLIPMETHOD_PROVIDER:
# ifdef FEAT_EVAL
	    ಹಿಂತಿರುಗಿ clip_provider;
# endif
	ಪೂರ್ವನಿಯೋಜಿತ:
	    ಹಿಂತಿರುಗಿ (char_u *)"none";
    }
}

/*
 * Sets the current clipmethod to use given by `get_clipmethod()`. Returns an
 * error message on failure else NULL.
 */
    ಅಕ್ಷರ *
ಆರಿಸು_clipmethod(ಶೂನ್ಯ)
{
    clipmethod_T ವಿಧಾನ = get_clipmethod(p_cpm);

    ಆದರೆ (ವಿಧಾನ == CLIPMETHOD_FAIL)
	ಹಿಂತಿರುಗಿ e_invalid_ವಾದ;

// If GUI is running or we are not on a system with Wayland or X11, then always
// return CLIPMETHOD_NONE. System or GUI clipboard handling always overrides.
// This is unless a provider is being used.
# ಆದರೆ defined(FEAT_XCLIPBOARD) || defined(FEAT_WAYLAND_CLIPBOARD)
#  ಆದರೆ defined(FEAT_GUI)
    ಆದರೆ (ವಿಧಾನ != CLIPMETHOD_PROVIDER && gui.ಒಳ_use)
    {
#   ifdef FEAT_WAYLAND
	// We only interact with Wayland for the clipboard, we can just deinit
	// everything.
	wayland_uninit_connection();
#   endif

	ವಿಧಾನ = CLIPMETHOD_NONE;
	ಹೋಗು lose_sel_exit;
    }
#  endif
# ಇಲ್ಲದಿದ್ದರೆ
    // If on a system like windows or macos, then clipmethod is irrelevant, we
    // use their way of accessing the clipboard. This is unless we are using the
    // clipboard provider
#  ifdef FEAT_CLIPBOARD_PROVIDER
    ಆದರೆ (ವಿಧಾನ != CLIPMETHOD_PROVIDER)
#  endif
    {
	ವಿಧಾನ = CLIPMETHOD_NONE;
	ಹೋಗು ನಿರ್ಗಮಿಸು;
    }
# endif

# ifdef FEAT_CLIPBOARD
    // Deinitialize clipboard if there is no way to access clipboard
    ಆದರೆ (ವಿಧಾನ == CLIPMETHOD_NONE)
	clip_init(FALSE);
    // If we have a clipmethod that works now, then initialize clipboard
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (clipmethod == CLIPMETHOD_NONE && ವಿಧಾನ != CLIPMETHOD_NONE)
    {
	clip_init(TRUE);
	did_warn_clipboard = false;
    }
    // Disown clipboard if we are switching to a new method
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (clipmethod != CLIPMETHOD_NONE && ವಿಧಾನ != clipmethod)
    {
#  ಆದರೆ (defined(FEAT_XCLIPBOARD) || defined(FEAT_WAYLAND_CLIPBOARD)) \
	&& defined(FEAT_GUI)
lose_sel_exit:
#  endif
	ಆದರೆ (clip_ನಕ್ಷತ್ರ.owned)
	    clip_lose_selection(&clip_ನಕ್ಷತ್ರ);
	ಆದರೆ (clip_plus.owned)
	    clip_lose_selection(&clip_plus);

#  ಆದರೆ defined(FEAT_GUI)
	ಆದರೆ (!gui.ಒಳ_use)
#  endif
	{
	    clip_init(TRUE);
	    did_warn_clipboard = false;
	}
    }
# endif // FEAT_CLIPBOARD

# ಆದರೆ !defined(FEAT_XCLIPBOARD) && !defined(FEAT_WAYLAND_CLIPBOARD)
ನಿರ್ಗಮಿಸು:
# endif

    clipmethod = ವಿಧಾನ;

# ifdef FEAT_EVAL
    ಗಣ_vim_var_string(VV_CLIPMETHOD, (char_u*)clipmethod_ಗಮ್ಯ_str(ವಿಧಾನ), -1);
# endif

    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

/*
 * Call choose_clipmethod().
 */
    ಶೂನ್ಯ
ex_clipreset(exarg_T *eap ಬಳಸದ)
{
    clipmethod_T prev = clipmethod;

    ಆರಿಸು_clipmethod();

    ಆದರೆ (clipmethod == CLIPMETHOD_NONE)
	smsg(_("Could not find a way to access the clipboard."));
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (clipmethod != prev)
	smsg(_("Switched to clipboard method '%s'."),
		clipmethod_ಗಮ್ಯ_str(clipmethod));
}

/*
 * Adjust the register name pointed to with "rp" for the clipboard being
 * used always and the clipboard being available.
 */
    ಶೂನ್ಯ
adjust_clip_reg(ಪೂರ್ಣಾಂಕ *rp)
{
# ifdef FEAT_CLIPBOARD_PROVIDER
    ಆದರೆ (clipmethod == CLIPMETHOD_PROVIDER)
    {
	ಆದರೆ (*rp == 0 && clip_unnamed != 0)
	    *rp = ((clip_unnamed & CLIP_UNNAMED_PLUS)) ? '+' : '*';
	ಹಿಂತಿರುಗಿ;
    }
# endif
# ifdef FEAT_CLIPBOARD
    // If no reg. specified, and "unnamed" or "unnamedplus" is in 'clipboard',
    // use '*' or '+' reg, respectively. "unnamedplus" prevails.
    ಆದರೆ (*rp == 0 && (clip_unnamed != 0 || clip_unnamed_saved != 0))
    {
	ಆದರೆ (clip_unnamed != 0)
	    *rp = ((clip_unnamed & CLIP_UNNAMED_PLUS) && clip_plus.ಲಭ್ಯ)
								  ? '+' : '*';
	ಇಲ್ಲದಿದ್ದರೆ
	    *rp = ((clip_unnamed_saved & CLIP_UNNAMED_PLUS)
					   && clip_plus.ಲಭ್ಯ) ? '+' : '*';
    }
    ಆದರೆ ((!clip_ನಕ್ಷತ್ರ.ಲಭ್ಯ && *rp == '*') ||
	   (!clip_plus.ಲಭ್ಯ && *rp == '+'))
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
    ಅಕ್ಷರ *
did_ಗಣ_clipboard(optset_T *ವಾದಗಳು ಬಳಸದ)
{
    ಪೂರ್ಣಾಂಕ		new_unnamed = 0;
# ifdef FEAT_CLIPBOARD
    ಪೂರ್ಣಾಂಕ		new_autoselect_ನಕ್ಷತ್ರ = FALSE;
    ಪೂರ್ಣಾಂಕ		new_autoselect_plus = FALSE;
    ಪೂರ್ಣಾಂಕ		new_autoselectml = FALSE;
    ಪೂರ್ಣಾಂಕ		new_html = FALSE;
# endif
    regprog_T	*new_exclude_prog = ಖಾಲಿ;
    ಅಕ್ಷರ	*errmsg = ಖಾಲಿ;
    char_u	*p;

    ಗಾಗಿ (p = p_cb; *p != NUL; )
    {
	// Note: Keep this in sync with p_cb_values.
	ಆದರೆ (STRNCMP(p, "unnamed", 7) == 0 && (p[7] == ',' || p[7] == NUL))
	{
	    new_unnamed |= CLIP_UNNAMED;
	    p += 7;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNCMP(p, "unnamedplus", 11) == 0
					    && (p[11] == ',' || p[11] == NUL))
	{
	    new_unnamed |= CLIP_UNNAMED_PLUS;
	    p += 11;
	}
# ifdef FEAT_CLIPBOARD
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNCMP(p, "autoselect", 10) == 0
					    && (p[10] == ',' || p[10] == NUL))
	{
	    new_autoselect_ನಕ್ಷತ್ರ = TRUE;
	    p += 10;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNCMP(p, "autoselectplus", 14) == 0
					    && (p[14] == ',' || p[14] == NUL))
	{
	    new_autoselect_plus = TRUE;
	    p += 14;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNCMP(p, "autoselectml", 12) == 0
					    && (p[12] == ',' || p[12] == NUL))
	{
	    new_autoselectml = TRUE;
	    p += 12;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNCMP(p, "html", 4) == 0 && (p[4] == ',' || p[4] == NUL))
	{
	    new_html = TRUE;
	    p += 4;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNCMP(p, "exclude:", 8) == 0 && new_exclude_prog == ಖಾಲಿ)
	{
	    p += 8;
	    new_exclude_prog = vim_regcomp(p, RE_MAGIC);
	    ಆದರೆ (new_exclude_prog == ಖಾಲಿ)
		errmsg = e_invalid_ವಾದ;
	    ನಿಲ್ಲಿಸು;
	}
# endif
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    errmsg = e_invalid_ವಾದ;
	    ನಿಲ್ಲಿಸು;
	}
	ಆದರೆ (*p == ',')
	    ++p;
    }
    ಆದರೆ (errmsg == ಖಾಲಿ)
    {
# ifdef FEAT_CLIPBOARD
	ಆದರೆ (global_busy)
	    // clip_unnamed will be reset to clip_unnamed_saved
	    // at end_global_changes
	    clip_unnamed_saved = new_unnamed;
	ಇಲ್ಲದಿದ್ದರೆ
# endif
	    clip_unnamed = new_unnamed;
# ifdef FEAT_CLIPBOARD
	clip_autoselect_ನಕ್ಷತ್ರ = new_autoselect_ನಕ್ಷತ್ರ;
	clip_autoselect_plus = new_autoselect_plus;
	clip_autoselectml = new_autoselectml;
	clip_html = new_html;
	vim_regfree(clip_exclude_prog);
	clip_exclude_prog = new_exclude_prog;
# endif
# ಆದರೆ defined(FEAT_GUI_GTK)
	ಆದರೆ (gui.ಒಳ_use)
	{
#  ifdef USE_GTK4
	    gui_gtk_update_selection_formats(&clip_plus);
	    gui_gtk_update_selection_formats(&clip_ನಕ್ಷತ್ರ);
#  ಇಲ್ಲದಿದ್ದರೆ
	    gui_gtk_ಗಣ_selection_targets((GdkAtom)GDK_SELECTION_PRIMARY);
	    gui_gtk_ಗಣ_selection_targets((GdkAtom)clip_plus.gtk_sel_atom);
#  endif
#  ifdef FEAT_DND
	    gui_gtk_ಗಣ_dnd_targets();
#  endif
	}
# endif
    }
    ಇಲ್ಲದಿದ್ದರೆ
	vim_regfree(new_exclude_prog);

    ಹಿಂತಿರುಗಿ errmsg;
}

#ಆದರೆ_ಅಂತ್ಯ // HAVE_CLIPMETHOD

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CLIPBOARD_PROVIDER

/*
 * Check if a clipboard provider with given name is available. Returns 1 if available,
 * 0 if not available, and -1 on error
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
clip_provider_is_ಲಭ್ಯ(char_u *provider)
{
    dict_T	*providers = get_vim_var_dict(VV_CLIPPROVIDERS);
    typval_T	provider_tv = {0};
    callback_T	callback = {0};
    typval_T	rettv = {0};
    typval_T	func_tv = {0};
    ಪೂರ್ಣಾಂಕ		res = 0;

    ಆದರೆ (dict_get_tv(providers, (ಅಕ್ಷರ *)provider, &provider_tv) == FAIL
	    || provider_tv.v_type != VAR_DICT)
	// clipboard provider not defined
	ಹಿಂತಿರುಗಿ -1;

    ಆದರೆ (dict_get_tv(provider_tv.vval.v_dict, "available", &func_tv) == FAIL)
    {
	clear_tv(&provider_tv);
	// If "available" function not specified assume always TRUE
	ಹಿಂತಿರುಗಿ 1;
    }

    ಆದರೆ ((callback = get_callback(&func_tv)).cb_name == ಖಾಲಿ)
	ಹೋಗು fail;

    ಆದರೆ (call_callback(&callback, -1, &rettv, 0, ಖಾಲಿ) == FAIL ||
	    (rettv.v_type != VAR_BOOL && rettv.v_type != VAR_ಸಂಖ್ಯೆ))
	ಹೋಗು fail;

    ಆದರೆ (rettv.vval.v_ಸಂಖ್ಯೆ)
	res = 1;

    ಆದರೆ (FALSE)
fail:
	res = -1;

    free_callback(&callback);
    clear_tv(&func_tv);
    clear_tv(&rettv);
    clear_tv(&provider_tv);

    ಹಿಂತಿರುಗಿ res;
}

/*
 * Get the specified callback "function" from the provider dictionary for
 * register "reg".
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
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

    ಆದರೆ (dict_get_tv(providers, (ಅಕ್ಷರ *)provider, &provider_tv) == FAIL)
	ಹಿಂತಿರುಗಿ FAIL;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (provider_tv.v_type != VAR_DICT)
    {
	clear_tv(&provider_tv);
	ಹಿಂತಿರುಗಿ FAIL;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (dict_get_tv(
		provider_tv.vval.v_dict,
		(ಅಕ್ಷರ *)function,
		&action_tv) == FAIL)
    {
	clear_tv(&provider_tv);
	ಹಿಂತಿರುಗಿ FAIL;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (action_tv.v_type != VAR_DICT)
    {
	clear_tv(&provider_tv);
	clear_tv(&action_tv);
	ಹಿಂತಿರುಗಿ FAIL;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (dict_get_tv(action_tv.vval.v_dict, (ಅಕ್ಷರ *)reg, &func_tv) == FAIL)
    {
	clear_tv(&provider_tv);
	clear_tv(&action_tv);
	ಹಿಂತಿರುಗಿ FAIL;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((cb = get_callback(&func_tv)).cb_name == ಖಾಲಿ)
    {
	clear_tv(&provider_tv);
	clear_tv(&action_tv);
	clear_tv(&func_tv);
	ಹಿಂತಿರುಗಿ FAIL;
    }
    clear_tv(&provider_tv);
    clear_tv(&action_tv);

    // func_tv owns the function name, so we must make a copy for the callback
    ಗಣ_callback(callback, &cb);
    clear_tv(&func_tv);
    ಹಿಂತಿರುಗಿ OK;
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
    list_T	*list = ಖಾಲಿ;

    ಆದರೆ (recursive)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (clip_provider_get_callback(
		reg,
		provider,
		(char_u *)"copy",
		&callback) == FAIL)
	ಹಿಂತಿರುಗಿ;

    // Convert register type into a string
    ಆದರೆ (*reg == '+')
	y_ptr = get_y_ರಿಜಿಸ್ಟರ್(REAL_PLUS_ರಿಜಿಸ್ಟರ್);
    ಇಲ್ಲದಿದ್ದರೆ
	y_ptr = get_y_ರಿಜಿಸ್ಟರ್(ನಕ್ಷತ್ರ_ರಿಜಿಸ್ಟರ್);

    ಆಯ್ಕೆ (y_ptr->y_type)
    {
	ಸ್ಥಿತಿ MCHAR:
	    type[0] = 'v';
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ MLINE:
	    type[0] = 'V';
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ MBLOCK:
	    sprintf((ಅಕ್ಷರ *)type, "%c%d", Ctrl_V, y_ptr->y_ಅಗಲ + 1);
	    ನಿಲ್ಲಿಸು;
	ಪೂರ್ವನಿಯೋಜಿತ:
	    type[0] = 0;
	    ನಿಲ್ಲಿಸು;
    }

    argvars[0].v_type = VAR_STRING;
    argvars[0].vval.v_string = reg;

    argvars[1].v_type = VAR_STRING;
    argvars[1].vval.v_string = type;

    // Get register contents by creating a list of lines
    list = list_alloc();

    ಆದರೆ (list == ಖಾಲಿ)
    {
	free_callback(&callback);
	ಹಿಂತಿರುಗಿ;
    }

    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < y_ptr->y_ಗಾತ್ರ; i++)
	ಆದರೆ (list_append_string(list, y_ptr->y_array[i].string,
	    (ಪೂರ್ಣಾಂಕ)y_ptr->y_array[i].ಉದ್ದಳತೆ) == FAIL)
	{
	    free_callback(&callback);
	    list_unref(list);
	    ಹಿಂತಿರುಗಿ;
	}

    list->lv_refcount++;

    argvars[2].v_type = VAR_LIST;
    argvars[2].v_ಬೀಗ = VAR_FIXED;
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
    ಪೂರ್ಣಾಂಕ		ret;
    char_u	*reg_type;
    list_T	*ಸಾಲುಗಳು;

    ಆದರೆ (recursive)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (clip_provider_get_callback(
		reg,
		provider,
		(char_u *)"paste",
		&callback) == FAIL)
	ಹಿಂತಿರುಗಿ;

    argvars[0].v_type = VAR_STRING;
    argvars[0].vval.v_string = reg;

    argvars[1].v_type = VAR_UNKNOWN;

    textlock++;
    recursive = true;
    ret = call_callback(&callback, -1, &rettv, 1, argvars);
    recursive = false;
    textlock--;

    ಆದರೆ (ret == FAIL)
	ಹೋಗು ನಿರ್ಗಮಿಸು;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (rettv.v_type == VAR_TUPLE
	    && TUPLE_LEN(rettv.vval.v_tuple) == 2
	    && TUPLE_ಅಂಶ(rettv.vval.v_tuple, 0)->v_type == VAR_STRING
	    && TUPLE_ಅಂಶ(rettv.vval.v_tuple, 1)->v_type == VAR_LIST)
    {
	reg_type = TUPLE_ಅಂಶ(rettv.vval.v_tuple, 0)->vval.v_string;
	ಸಾಲುಗಳು = TUPLE_ಅಂಶ(rettv.vval.v_tuple, 1)->vval.v_list;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (rettv.v_type == VAR_LIST
	    && rettv.vval.v_list->lv_len == 2
	    && rettv.vval.v_list->lv_ಮೊದಲ->li_tv.v_type == VAR_STRING
	    && rettv.vval.v_list->lv_ಮೊದಲ->li_ಮುಂದಿನ->li_tv.v_type == VAR_LIST)
    {
	reg_type = rettv.vval.v_list->lv_ಮೊದಲ->li_tv.vval.v_string;
	ಸಾಲುಗಳು = rettv.vval.v_list->lv_ಮೊದಲ->li_ಮುಂದಿನ->li_tv.vval.v_list;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	ಹೋಗು ನಿರ್ಗಮಿಸು;

    {
	char_u		yank_type = MAUTO;
	ದೀರ್ಘ		ಖಂಡ_len = -1;
	yankreg_T	*y_ptr, *cur_y_ptr;
	char_u		**lstval;
	char_u		**allocval;
	char_u		buf[NUMBUFLEN];
	char_u		**curval;
	char_u		**curallocval;
	char_u		*strval;
	listitem_T	*li;
	ಪೂರ್ಣಾಂಕ		len;

	// If the list is NULL handle like an empty list.
	len = ಸಾಲುಗಳು == ಖಾಲಿ ? 0 : ಸಾಲುಗಳು->lv_len;

	// First half: use for pointers to result lines; second half: use for
	// pointers to allocated copies.
	lstval = ALLOC_MULT(char_u *, (len + 1) * 2);
	ಆದರೆ (lstval == ಖಾಲಿ)
	    ಹೋಗು ನಿರ್ಗಮಿಸು;
	curval = lstval;
	allocval = lstval + len + 2;
	curallocval = allocval;

	ಆದರೆ (ಸಾಲುಗಳು != ಖಾಲಿ)
	{
	    CHECK_LIST_MATERIALIZE(ಸಾಲುಗಳು);
	    FOR_ALL_LIST_ವಸ್ತುಗಳು(ಸಾಲುಗಳು, li)
	    {
		strval = tv_get_string_buf_chk(&li->li_tv, buf);
		ಆದರೆ (strval == ಖಾಲಿ)
		    ಹೋಗು free_lstval;
		ಆದರೆ (strval == buf)
		{
		    // Need to make a copy, next tv_get_string_buf_chk() will
		    // overwrite the string.
		    strval = vim_strsave(buf);
		    ಆದರೆ (strval == ಖಾಲಿ)
			ಹೋಗು free_lstval;
		    *curallocval++ = strval;
		}
		*curval++ = strval;
	    }
	}
	*curval++ = ಖಾಲಿ;

	ಆದರೆ (*reg_type != NUL && (STRLEN(reg_type) == 0
		|| get_yank_type(&reg_type, &yank_type, &ಖಂಡ_len) == FAIL))
	{
	    emsg(e_invalid_ವಾದ);
	    ಹೋಗು free_lstval;
	}

	ಆದರೆ (*reg == '+')
	    y_ptr = get_y_ರಿಜಿಸ್ಟರ್(REAL_PLUS_ರಿಜಿಸ್ಟರ್);
	ಇಲ್ಲದಿದ್ದರೆ
	    y_ptr = get_y_ರಿಜಿಸ್ಟರ್(ನಕ್ಷತ್ರ_ರಿಜಿಸ್ಟರ್);

	// Free previous register contents
	cur_y_ptr = get_y_current();
	ಗಣ_y_current(y_ptr);

	free_yank_all();
	get_y_current()->y_ಗಾತ್ರ = 0;

	ಗಣ_y_current(cur_y_ptr);

	str_ಗಮ್ಯ_reg(y_ptr,
		yank_type,
		(char_u *)lstval,
		-1,
		ಖಂಡ_len,
		TRUE);

free_lstval:
	ಆಗ (curallocval > allocval)
	    vim_free(*--curallocval);
	vim_free(lstval);
    }

ನಿರ್ಗಮಿಸು:
    free_callback(&callback);
    clear_tv(&rettv);
}

// Used to stop calling the provider callback every time there is an update.
// This prevents unnecessary calls when accessing the provider often in an
// interval.
//
// If -1 then allow provider callback to be called then set to one. Default
// value (is allowed) is -2.
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ನಕ್ಷತ್ರ_pause_ಎಣಿಕೆ = -2, plus_pause_ಎಣಿಕೆ = -2;

    ಶೂನ್ಯ
call_clip_provider_request(ಪೂರ್ಣಾಂಕ reg)
{
    ಆದರೆ (clipmethod != CLIPMETHOD_PROVIDER)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (reg == '+' && plus_pause_ಎಣಿಕೆ < 0)
    {
	ಆದರೆ (plus_pause_ಎಣಿಕೆ == -1)
	    plus_pause_ಎಣಿಕೆ = 1;
	clip_provider_paste((char_u *)"+", clip_provider);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (reg == '*' && ನಕ್ಷತ್ರ_pause_ಎಣಿಕೆ < 0)
    {
	ಆದರೆ (ನಕ್ಷತ್ರ_pause_ಎಣಿಕೆ == -1)
	    ನಕ್ಷತ್ರ_pause_ಎಣಿಕೆ = 1;
	clip_provider_paste((char_u *)"*", clip_provider);
    }
    ಇಲ್ಲದಿದ್ದರೆ
	ಹಿಂತಿರುಗಿ;
}

    ಶೂನ್ಯ
call_clip_provider_ಗಣ(ಪೂರ್ಣಾಂಕ reg)
{
    ಆದರೆ (clipmethod != CLIPMETHOD_PROVIDER)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (reg == '+' && plus_pause_ಎಣಿಕೆ < 0)
    {
	ಆದರೆ (plus_pause_ಎಣಿಕೆ == -1)
	    plus_pause_ಎಣಿಕೆ = 1;
	clip_provider_copy((char_u *)"+", clip_provider);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (reg == '*' && ನಕ್ಷತ್ರ_pause_ಎಣಿಕೆ < 0)
    {
	ಆದರೆ (ನಕ್ಷತ್ರ_pause_ಎಣಿಕೆ == -1)
	    ನಕ್ಷತ್ರ_pause_ಎಣಿಕೆ = 1;
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
    ನಕ್ಷತ್ರ_pause_ಎಣಿಕೆ = (ನಕ್ಷತ್ರ_pause_ಎಣಿಕೆ == -2
	|| ನಕ್ಷತ್ರ_pause_ಎಣಿಕೆ == -1) ? -1 : ನಕ್ಷತ್ರ_pause_ಎಣಿಕೆ + 1;
}

    ಶೂನ್ಯ
dec_clip_provider(ಶೂನ್ಯ)
{
    ಆದರೆ (plus_pause_ಎಣಿಕೆ != -2)
	plus_pause_ಎಣಿಕೆ = plus_pause_ಎಣಿಕೆ == -1 ? -1 : plus_pause_ಎಣಿಕೆ - 1;
    ಆದರೆ (ನಕ್ಷತ್ರ_pause_ಎಣಿಕೆ != -2)
	ನಕ್ಷತ್ರ_pause_ಎಣಿಕೆ = ನಕ್ಷತ್ರ_pause_ಎಣಿಕೆ == -1 ? -1 : ನಕ್ಷತ್ರ_pause_ಎಣಿಕೆ - 1;

    ಆದರೆ (plus_pause_ಎಣಿಕೆ == 0 || plus_pause_ಎಣಿಕೆ == -1)
	plus_pause_ಎಣಿಕೆ = -2;
    ಆದರೆ (ನಕ್ಷತ್ರ_pause_ಎಣಿಕೆ == 0 || ನಕ್ಷತ್ರ_pause_ಎಣಿಕೆ == -1)
	ನಕ್ಷತ್ರ_pause_ಎಣಿಕೆ = -2;
}

#ಆದರೆ_ಅಂತ್ಯ // FEAT_CLIPBOARD_PROVIDER

#ಆದರೆ defined(FEAT_WAYLAND_CLIPBOARD) || (defined(FEAT_GUI_GTK) && defined(USE_GTK4))
/*
 * If "vim" is TRUE, then get the motion type. If "vimenc" is TRUE, then get the
 * motion type and also convert "*buf". "buf" and "len_store" will be updated to
 * reflect the actual contents, but should be set beforehand with the initial
 * contents. Note that if "*tofree" is not NULL, use vim_free() on "*buf",
 * otherwise use the free func for which "*buf" was allocated with. In both
 * cases use vim_free() on "*tofree". Returns OK on success and FAIL on failure.
 */
    ಪೂರ್ಣಾಂಕ
clip_convert_ದತ್ತಾಂಶ(
	char_u	**buf,
	ದೀರ್ಘ	*len_store,
	ಪೂರ್ಣಾಂಕ	*motion,
	bool	vim,
	bool	vimenc,
	char_u	**tofree)
{
    char_u  *final = *buf;
    char_u  *enc;
    ದೀರ್ಘ    len = *len_store;

    ಆದರೆ (vim && len >= 2)
    {
	*motion = *final++;
	len--;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (vimenc && len >= 3)
    {
	vimconv_T   conv;
	ಪೂರ್ಣಾಂಕ	    convlen;

	// First byte is motion type
	*motion = *final++;
	len--;

	// Get encoding of selection
	enc = final;

	// Skip the encoding type including null terminator in final text
	final = memchr(final, NUL, len);
	ಆದರೆ (final == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ FAIL;
	final++; // Skip NUL

	// Subtract pointers to get length of encoding;
	len -= final - enc;

	conv.vc_type = CONV_NONE;
	convert_ಸಿದ್ಧತೆ(&conv, enc, p_enc);
	ಆದರೆ (conv.vc_type != CONV_NONE)
	{
	   char_u *tmp;

	   convlen = len;
	   tmp = string_convert(&conv, final, &convlen);
	   len = convlen;
	   ಆದರೆ (tmp != ಖಾಲಿ)
	   {
		final = tmp;
		*tofree = final;
	   }
	   convert_ಸಿದ್ಧತೆ(&conv, ಖಾಲಿ, ಖಾಲಿ);
	}
    }
    *buf = final;
    *len_store = len;
    ಹಿಂತಿರುಗಿ OK;
}
#ಆದರೆ_ಅಂತ್ಯ
