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

#ضمن "vim.h"

#اگر_تعریف_شده FEAT_CYGWIN_WIN32_CLIPBOARD
# define WIN32_LEAN_AND_میانگین
# include <windows.h>
# include "winclip.pro"
#پایان_اگر

// Functions for copying and pasting text between applications.
// This is always included in a GUI version, but may also be included when the
// clipboard and mouse is available to a terminal version such as xterm.
// Note: there are some more functions in ops.c that handle selection stuff.
//
// Also note that the majority of functions here deal with the X 'primary'
// (visible - for Visual mode use) selection, and only that. There are no
// versions of these for the 'clipboard' selection, as Visual mode has no use
// for them.


#اگر_تعریف_شده FEAT_CLIPBOARD_PROVIDER
ساكن صحيح clip_provider_is_موجود(char_u *provider);
#پایان_اگر

#إذا defined(FEAT_CLIPBOARD)

# إذا defined(FEAT_WAYLAND_CLIPBOARD)

#  include "wayland.h"

// Represents either the regular or primary selection
تعريفنوع بنية {
    char_u		*contents;	// Non-null if we own selection,
					// contains the data to send to other
					// clients.
    vwl_داده_منبع_T	*منبع;	// Non-NULL if we own the selection,
					// else NULL if we don't.
    vwl_داده_offer_T	*offer;		// Current offer for the selection

    bool		own_success;	// Used by clip_wl_own_selection()
    bool		موجود;	// If selection is ready to serve/use

    // These may point to the same proxy as the other selection
    vwl_داده_device_manager_T	*manager;
    vwl_داده_device_T		*device;
} clip_wl_selection_T;

// Represents the clipboard for the global Wayland connection, for the chosen
// seat (using the 'wl_seat' option)
تعريفنوع بنية {
    vwl_seat_T *seat;

    clip_wl_selection_T regular;
    clip_wl_selection_T primary;
} clip_wl_T;

// Mime types we support sending and receiving
// Mimes with a lower index in the array are prioritized first when we are
// receiving data.
ساكن ثابت حرف *supported_mimes[] = {
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

ساكن فراغ
clip_wl_receive_داده(Clipboard_T *cbd, ثابت حرف *mime_type, صحيح fd);
ساكن فراغ clip_wl_request_selection(Clipboard_T *cbd);
ساكن صحيح clip_wl_own_selection(Clipboard_T *cbd);
ساكن فراغ clip_wl_lose_selection(Clipboard_T *cbd);
ساكن فراغ clip_wl_مجموعه_selection(Clipboard_T *cbd);

#  إذا defined(USE_SYSTEM)
ساكن bool clip_wl_owner_exists(Clipboard_T *cbd);
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
    فراغ
clip_init(صحيح can_use)
{
    Clipboard_T *cb;

    cb = &clip_ستاره;
    لكل (;;)
    {
	// No need to init again if cbd is already available
	إذا (can_use && cb->موجود)
	    اذهب skip;

	cb->موجود  = can_use;
	cb->owned      = FALSE;
	cb->شروع.lnum = 0;
	cb->شروع.ستون  = 0;
	cb->انتها.lnum   = 0;
	cb->انتها.ستون    = 0;
	cb->state      = SELECT_CLEARED;

skip:
	إذا (cb == &clip_plus)
	    اكسر;
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
    فراغ
clip_update_selection(Clipboard_T *clip)
{
    pos_T	    شروع, انتها;

    // If visual mode is only due to a redo command ("."), then ignore it
    إذا (!redo_VIsual_busy && VIsual_فعال && (State & نما_نرمال))
    {
	إذا (LT_POS(VIsual, curwin->w_cursor))
	{
	    شروع = VIsual;
	    انتها = curwin->w_cursor;
	    إذا (has_mbyte)
		انتها.ستون += (*mb_ptr2len)(ml_get_cursor()) - 1;
	}
	وإلا
	{
	    شروع = curwin->w_cursor;
	    انتها = VIsual;
	}
	إذا (!EQUAL_POS(clip->شروع, شروع)
		|| !EQUAL_POS(clip->انتها, انتها)
		|| clip->vmode != VIsual_نما)
	{
	    clip_clear_selection(clip);
	    clip->شروع = شروع;
	    clip->انتها = انتها;
	    clip->vmode = VIsual_نما;
	    clip_free_selection(clip);
	    clip_own_selection(clip);
	    clip_gen_مجموعه_selection(clip);
	}
    }
}

    ساكن صحيح
clip_gen_own_selection(Clipboard_T *cbd)
{
# إذا defined(FEAT_XCLIPBOARD) || defined(FEAT_WAYLAND_CLIPBOARD)
#  ifdef FEAT_GUI
    إذا (gui.درون_use)
	أرجع clip_mch_own_selection(cbd);
    وإلا
#  endif
    {
	إذا (clipmethod == CLIPMETHOD_WAYLAND)
	{
#  ifdef FEAT_WAYLAND_CLIPBOARD
	    أرجع clip_wl_own_selection(cbd);
#  endif
	}
	وإلا إذا (clipmethod == CLIPMETHOD_X11)
	{
#  ifdef FEAT_XCLIPBOARD
	    أرجع clip_xterm_own_selection(cbd);
#  endif
	}
    }
    أرجع FAIL;
# وإلا
    أرجع clip_mch_own_selection(cbd);
# endif
}

    فراغ
clip_own_selection(Clipboard_T *cbd)
{
    /*
     * Also want to check somehow that we are reading from the keyboard rather
     * than a mapping etc.
     */
# إذا defined(FEAT_X11) || defined(FEAT_WAYLAND_CLIPBOARD)
    // Always own the selection, we might have lost it without being
    // notified, e.g. during a ":sh" command.
    إذا (cbd->موجود)
    {
	صحيح was_owned = cbd->owned;

	cbd->owned = (clip_gen_own_selection(cbd) == OK);
	إذا (!was_owned && (cbd == &clip_ستاره || cbd == &clip_plus))
	{
	    // May have to show a different kind of highlighting for the
	    // selected area.  There is no specific redraw command for this,
	    // just redraw all windows on the current buffer.
	    إذا (cbd->owned
		    && (get_real_state() == نما_VISUAL
					    || get_real_state() == نما_SELECT)
		    && (cbd == &clip_ستاره ? clip_isautosel_ستاره()
						      : clip_isautosel_plus())
		    && HL_ATTR(HLF_V) != HL_ATTR(HLF_VNC))
		redraw_curbuf_later(UPD_INVERTED_ALL);
	}
    }
# وإلا
    // Only own the clipboard when we didn't own it yet.
    إذا (!cbd->owned && cbd->موجود)
	cbd->owned = (clip_gen_own_selection(cbd) == OK);
# endif
}

    ساكن فراغ
clip_gen_lose_selection(Clipboard_T *cbd)
{
# إذا defined(FEAT_XCLIPBOARD) || defined(FEAT_WAYLAND_CLIPBOARD)
#  ifdef FEAT_GUI
    إذا (gui.درون_use)
	clip_mch_lose_selection(cbd);
    وإلا
#  endif
    {
	إذا (clipmethod == CLIPMETHOD_WAYLAND)
	{
#  ifdef FEAT_WAYLAND_CLIPBOARD
	    clip_wl_lose_selection(cbd);
#  endif
	}
	وإلا إذا (clipmethod == CLIPMETHOD_X11)
	{
#  ifdef FEAT_XCLIPBOARD
	    clip_xterm_lose_selection(cbd);
#  endif
	}
    }
# وإلا
    clip_mch_lose_selection(cbd);
# endif
}

    فراغ
clip_lose_selection(Clipboard_T *cbd)
{
# ifdef FEAT_X11
    صحيح	    was_owned = cbd->owned;
# endif
    صحيح     visual_selection = FALSE;

    إذا (cbd == &clip_ستاره || cbd == &clip_plus)
	visual_selection = TRUE;

    clip_free_selection(cbd);
    cbd->owned = FALSE;
    إذا (visual_selection)
	clip_clear_selection(cbd);
    clip_gen_lose_selection(cbd);
# ifdef FEAT_X11
    إذا (visual_selection)
    {
	// May have to show a different kind of highlighting for the selected
	// area.  There is no specific redraw command for this, just redraw all
	// windows on the current buffer.
	إذا (was_owned
		&& (get_real_state() == نما_VISUAL
		    || get_real_state() == نما_SELECT)
		&& (cbd == &clip_ستاره ?
		    clip_isautosel_ستاره() : clip_isautosel_plus())
		&& HL_ATTR(HLF_V) != HL_ATTR(HLF_VNC)
		&& !exiting)
	{
	    update_curbuf(UPD_INVERTED_ALL);
	    setcursor();
	    cursor_on();
	    بیرون_flush_cursor(TRUE, FALSE);
	}
    }
# endif
}

    ساكن فراغ
clip_copy_selection(Clipboard_T *clip)
{
    إذا (VIsual_فعال && (State & نما_نرمال) && clip->موجود)
    {
	clip_update_selection(clip);
	clip_free_selection(clip);
	clip_own_selection(clip);
	إذا (clip->owned)
	    clip_get_selection(clip);
	clip_gen_مجموعه_selection(clip);
    }
}

/*
 * Save and restore clip_unnamed before doing possibly many changes. This
 * prevents accessing the clipboard very often which might slow down Vim
 * considerably.
 */
ساكن صحيح global_change_شمار = 0; // if set, inside a start_global_changes
ساكن صحيح clipboard_needs_update = FALSE; // clipboard needs to be updated
ساكن صحيح clip_did_مجموعه_selection = TRUE;

/*
 * Save clip_unnamed and reset it.
 */
    فراغ
شروع_global_changes(فراغ)
{
    إذا (++global_change_شمار > 1)
	أرجع;
    clip_unnamed_saved = clip_unnamed;
    clipboard_needs_update = FALSE;

    إذا (clip_did_مجموعه_selection)
    {
	clip_unnamed = 0;
	clip_did_مجموعه_selection = FALSE;
    }
}

/*
 * Return TRUE if setting the clipboard was postponed, it already contains the
 * right text.
 */
    ساكن صحيح
is_clipboard_needs_update(فراغ)
{
    أرجع clipboard_needs_update;
}

/*
 * Restore clip_unnamed and set the selection when needed.
 */
    فراغ
انتها_global_changes(فراغ)
{
    إذا (--global_change_شمار > 0)
	// recursive
	أرجع;
    إذا (!clip_did_مجموعه_selection)
    {
	clip_did_مجموعه_selection = TRUE;
	clip_unnamed = clip_unnamed_saved;
	clip_unnamed_saved = 0;
	إذا (clipboard_needs_update)
	{
	    // only store something in the clipboard,
	    // if we have yanked anything to it
	    إذا (clip_unnamed & CLIP_UNNAMED)
	    {
		clip_own_selection(&clip_ستاره);
		clip_gen_مجموعه_selection(&clip_ستاره);
	    }
	    إذا (clip_unnamed & CLIP_UNNAMED_PLUS)
	    {
		clip_own_selection(&clip_plus);
		clip_gen_مجموعه_selection(&clip_plus);
	    }
	}
    }
    clipboard_needs_update = FALSE;
}

/*
 * Called when Visual mode is ended: update the selection.
 */
    فراغ
clip_auto_select(فراغ)
{
    إذا (clip_isautosel_ستاره())
	clip_copy_selection(&clip_ستاره);
    إذا (clip_isautosel_plus())
	clip_copy_selection(&clip_plus);
}

/*
 * Return TRUE if automatic selection of Visual area is desired for the *
 * register.
 */
    صحيح
clip_isautosel_ستاره(فراغ)
{
# ifdef FEAT_CLIPBOARD_PROVIDER
    إذا (clipmethod == CLIPMETHOD_PROVIDER)
	أرجع false;
# endif
# ifdef FEAT_GUI
    إذا (gui.درون_use)
	أرجع vim_strchr(p_go, GO_ASEL) != عدم
	    && vim_strchr(p_go, GO_ASELPLUS) == عدم;
# endif
    أرجع clip_autoselect_ستاره;
}

/*
 * Return TRUE if automatic selection of Visual area is desired for the +
 * register.
 */
    صحيح
clip_isautosel_plus(فراغ)
{
# ifdef FEAT_CLIPBOARD_PROVIDER
    إذا (clipmethod == CLIPMETHOD_PROVIDER)
	أرجع false;
# endif
# ifdef FEAT_GUI
    إذا (gui.درون_use)
	أرجع vim_strchr(p_go, GO_ASELPLUS) != عدم;
# endif
    أرجع clip_autoselect_plus;
}


/*
 * Stuff for general mouse selection, without using Visual mode.
 */

/*
 * Compare two screen positions ala strcmp()
 */
    ساكن صحيح
clip_compare_pos(
    صحيح		row1,
    صحيح		col1,
    صحيح		row2,
    صحيح		col2)
{
    إذا (row1 > row2) أرجع(1);
    إذا (row1 < row2) أرجع(-1);
    إذا (col1 > col2) أرجع(1);
    إذا (col1 < col2) أرجع(-1);
    أرجع(0);
}

// "how" flags for clip_invert_area()
# define CLIP_CLEAR	1
# define CLIP_مجموعه	2
# define CLIP_TOGGLE	3

/*
 * Invert or un-invert a rectangle of the screen.
 * "invert" is true if the result is inverted.
 */
    ساكن فراغ
clip_invert_rectangle(
	Clipboard_T	*cbd استفادهنشده,
	صحيح		ردیف_arg,
	صحيح		ستون_arg,
	صحيح		ارتفاع_arg,
	صحيح		عرض_arg,
	صحيح		invert)
{
    صحيح		ردیف = ردیف_arg;
    صحيح		ستون = ستون_arg;
    صحيح		ارتفاع = ارتفاع_arg;
    صحيح		عرض = عرض_arg;

# ifdef FEAT_PROP_POPUP
    // this goes on top of all popup windows
    screen_zindex = CLIP_ZINDEX;

    إذا (ستون < cbd->min_ستون)
    {
	عرض -= cbd->min_ستون - ستون;
	ستون = cbd->min_ستون;
    }
    إذا (عرض > cbd->max_ستون - ستون)
	عرض = cbd->max_ستون - ستون;
    إذا (ردیف < cbd->min_ردیف)
    {
	ارتفاع -= cbd->min_ردیف - ردیف;
	ردیف = cbd->min_ردیف;
    }
    إذا (ارتفاع > cbd->max_ردیف - ردیف + 1)
	ارتفاع = cbd->max_ردیف - ردیف + 1;
# endif
# ifdef FEAT_GUI
    إذا (gui.درون_use)
	gui_mch_invert_rectangle(ردیف, ستون, ارتفاع, عرض);
    وإلا
# endif
	screen_draw_rectangle(ردیف, ستون, ارتفاع, عرض, invert, FALSE);
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
    ساكن فراغ
clip_invert_area(
	Clipboard_T	*cbd,
	صحيح		row1,
	صحيح		col1,
	صحيح		row2,
	صحيح		col2,
	صحيح		how)
{
    صحيح		invert = FALSE;
    صحيح		max_ستون;

# ifdef FEAT_PROP_POPUP
    max_ستون = cbd->max_ستون - 1;
# وإلا
    max_ستون = Columns - 1;
# endif

    إذا (how == CLIP_مجموعه)
	invert = TRUE;

    // Swap the from and to positions so the from is always before
    إذا (clip_compare_pos(row1, col1, row2, col2) > 0)
    {
	صحيح tmp_ردیف, tmp_ستون;

	tmp_ردیف = row1;
	tmp_ستون = col1;
	row1	= row2;
	col1	= col2;
	row2	= tmp_ردیف;
	col2	= tmp_ستون;
    }
    وإلا إذا (how == CLIP_TOGGLE)
	invert = TRUE;

    // If all on the same line, do it the easy way
    إذا (row1 == row2)
    {
	clip_invert_rectangle(cbd, row1, col1, 1, col2 - col1, invert);
    }
    وإلا
    {
	// Handle a piece of the first line
	إذا (col1 > 0)
	{
	    clip_invert_rectangle(cbd, row1, col1, 1,
						  (صحيح)Columns - col1, invert);
	    row1++;
	}

	// Handle a piece of the last line
	إذا (col2 < max_ستون)
	{
	    clip_invert_rectangle(cbd, row2, 0, 1, col2, invert);
	    row2--;
	}

	// Handle the rectangle that's left
	إذا (row2 >= row1)
	    clip_invert_rectangle(cbd, row1, 0, row2 - row1 + 1,
							 (صحيح)Columns, invert);
    }
}

/*
 * Start, continue or end a modeless selection.  Used when editing the
 * command-line, in the cmdline window and when the mouse is in a popup window.
 */
    فراغ
clip_modeless(صحيح button, صحيح is_click, صحيح is_drag)
{
    صحيح		repeat;

    repeat = ((clip_ستاره.نما == SELECT_نما_CHAR
		|| clip_ستاره.نما == SELECT_نما_سطر)
					      && (mod_ماسک & MOD_ماسک_2_CLICK))
	    || (clip_ستاره.نما == SELECT_نما_کلمه
					     && (mod_ماسک & MOD_ماسک_3_CLICK));
    إذا (is_click && button == MOUSE_راست)
    {
	// Right mouse button: If there was no selection, start one.
	// Otherwise extend the existing selection.
	إذا (clip_ستاره.state == SELECT_CLEARED)
	    clip_شروع_selection(mouse_ستون, mouse_ردیف, FALSE);
	clip_process_selection(button, mouse_ستون, mouse_ردیف, repeat);
    }
    وإلا إذا (is_click)
	clip_شروع_selection(mouse_ستون, mouse_ردیف, repeat);
    وإلا إذا (is_drag)
    {
	// Don't try extending a selection if there isn't one.  Happens when
	// button-down is in the cmdline and them moving mouse upwards.
	إذا (clip_ستاره.state != SELECT_CLEARED)
	    clip_process_selection(button, mouse_ستون, mouse_ردیف, repeat);
    }
    وإلا // release
	clip_process_selection(MOUSE_RELEASE, mouse_ستون, mouse_ردیف, FALSE);
}

/*
 * Update the currently selected region by adding and/or subtracting from the
 * beginning or end and inverting the changed area(s).
 */
    ساكن فراغ
clip_update_modeless_selection(
    Clipboard_T    *cb,
    صحيح		    row1,
    صحيح		    col1,
    صحيح		    row2,
    صحيح		    col2)
{
    // See if we changed at the beginning of the selection
    إذا (row1 != cb->شروع.lnum || col1 != (صحيح)cb->شروع.ستون)
    {
	clip_invert_area(cb, row1, col1, (صحيح)cb->شروع.lnum, cb->شروع.ستون,
								 CLIP_TOGGLE);
	cb->شروع.lnum = row1;
	cb->شروع.ستون  = col1;
    }

    // See if we changed at the end of the selection
    إذا (row2 != cb->انتها.lnum || col2 != (صحيح)cb->انتها.ستون)
    {
	clip_invert_area(cb, (صحيح)cb->انتها.lnum, cb->انتها.ستون, row2, col2,
								 CLIP_TOGGLE);
	cb->انتها.lnum = row2;
	cb->انتها.ستون  = col2;
    }
}

/*
 * Find the starting and ending positions of the word at the given row and
 * column.  Only white-separated words are recognized here.
 */
# define CHAR_CLASS(c)	(c <= ' ' ? ' ' : vim_iswordc(c))

    ساكن فراغ
clip_get_کلمه_boundaries(Clipboard_T *cb, صحيح ردیف, صحيح ستون)
{
    صحيح		شروع_class;
    صحيح		موقت_ستون;
    char_u	*p;
    صحيح		mboff;

    إذا (ردیف >= screen_ردیفها || ستون >= screen_Columns || Screen_سطرها == عدم)
	أرجع;

    p = Screen_سطرها + سطر_جابه_جایی[ردیف];
    // Correct for starting in the right half of a double-wide char
    إذا (enc_dbcs != 0)
	ستون -= dbcs_screen_سر_off(p, p + ستون);
    وإلا إذا (enc_utf8 && p[ستون] == 0)
	--ستون;
    شروع_class = CHAR_CLASS(p[ستون]);

    موقت_ستون = ستون;
    لكل ( ; موقت_ستون > 0; موقت_ستون--)
	إذا (enc_dbcs != 0
		   && (mboff = dbcs_screen_سر_off(p, p + موقت_ستون - 1)) > 0)
	    موقت_ستون -= mboff;
	وإلا إذا (CHAR_CLASS(p[موقت_ستون - 1]) != شروع_class
		&& !(enc_utf8 && p[موقت_ستون - 1] == 0))
	    اكسر;
    cb->کلمه_شروع_ستون = موقت_ستون;

    موقت_ستون = ستون;
    لكل ( ; موقت_ستون < screen_Columns; موقت_ستون++)
	إذا (enc_dbcs != 0 && dbcs_ptr2cells(p + موقت_ستون) == 2)
	    ++موقت_ستون;
	وإلا إذا (CHAR_CLASS(p[موقت_ستون]) != شروع_class
		&& !(enc_utf8 && p[موقت_ستون] == 0))
	    اكسر;
    cb->کلمه_انتها_ستون = موقت_ستون;
}

/*
 * Find the column position for the last non-whitespace character on the given
 * line at or before start_col.
 */
    ساكن صحيح
clip_get_سطر_انتها(Clipboard_T *cbd استفادهنشده, صحيح ردیف)
{
    صحيح	    i;

    إذا (ردیف >= screen_ردیفها || Screen_سطرها == عدم)
	أرجع 0;
    لكل (i =
# ifdef FEAT_PROP_POPUP
	    cbd->max_ستون;
# وإلا
	    screen_Columns;
# endif
			    i > 0; i--)
	إذا (Screen_سطرها[سطر_جابه_جایی[ردیف] + i - 1] != ' ')
	    اكسر;
    أرجع i;
}

/*
 * Start the selection
 */
    فراغ
clip_شروع_selection(صحيح ستون, صحيح ردیف, صحيح repeated_click)
{
    Clipboard_T	*cb = &clip_ستاره;
# ifdef FEAT_PROP_POPUP
    win_T	*wp;
    صحيح		ردیف_cp = ردیف;
    صحيح		ستون_cp = ستون;

    wp = mouse_پیدا_کن_win(&ردیف_cp, &ستون_cp, پیدا_کن_POPUP);
    إذا (wp != عدم && WIN_IS_POPUP(wp)
				  && popup_is_درون_scrollbar(wp, ردیف_cp, ستون_cp))
	// click or double click in scrollbar does not start a selection
	أرجع;
# endif

    إذا (cb->state == SELECT_انجامشده)
	clip_clear_selection(cb);

    ردیف = check_ردیف(ردیف);
    ستون = check_ستون(ستون);
    ستون = mb_fix_ستون(ستون, ردیف);

    cb->شروع.lnum  = ردیف;
    cb->شروع.ستون   = ستون;
    cb->انتها	    = cb->شروع;
    cb->origin_ردیف  = (short_u)cb->شروع.lnum;
    cb->state	    = SELECT_درون_پیشرفت;
# ifdef FEAT_PROP_POPUP
    إذا (wp != عدم && WIN_IS_POPUP(wp))
    {
	// Click in a popup window restricts selection to that window,
	// excluding the border.
	cb->min_ستون = wp->w_wincol + wp->w_popup_border[3];
	cb->max_ستون = wp->w_wincol + popup_عرض(wp)
				 - wp->w_popup_border[1] - wp->w_has_scrollbar;
	إذا (cb->max_ستون > screen_Columns)
	    cb->max_ستون = screen_Columns;
	cb->min_ردیف = wp->w_winrow + wp->w_popup_border[0];
	cb->max_ردیف = wp->w_winrow + popup_ارتفاع(wp) - 1
						   - wp->w_popup_border[2];
    }
    وإلا
    {
	cb->min_ستون = 0;
	cb->max_ستون = screen_Columns;
	cb->min_ردیف = 0;
	cb->max_ردیف = screen_ردیفها;
    }
# endif

    إذا (repeated_click)
    {
	إذا (++cb->نما > SELECT_نما_سطر)
	    cb->نما = SELECT_نما_CHAR;
    }
    وإلا
	cb->نما = SELECT_نما_CHAR;

# ifdef FEAT_GUI
    // clear the cursor until the selection is made
    إذا (gui.درون_use)
	gui_undraw_cursor();
# endif

    بدل (cb->نما)
    {
	حالة SELECT_نما_CHAR:
	    cb->origin_شروع_ستون = cb->شروع.ستون;
	    cb->کلمه_انتها_ستون = clip_get_سطر_انتها(cb, (صحيح)cb->شروع.lnum);
	    اكسر;

	حالة SELECT_نما_کلمه:
	    clip_get_کلمه_boundaries(cb, (صحيح)cb->شروع.lnum, cb->شروع.ستون);
	    cb->origin_شروع_ستون = cb->کلمه_شروع_ستون;
	    cb->origin_انتها_ستون	 = cb->کلمه_انتها_ستون;

	    clip_invert_area(cb, (صحيح)cb->شروع.lnum, cb->کلمه_شروع_ستون,
			    (صحيح)cb->انتها.lnum, cb->کلمه_انتها_ستون, CLIP_مجموعه);
	    cb->شروع.ستون = cb->کلمه_شروع_ستون;
	    cb->انتها.ستون   = cb->کلمه_انتها_ستون;
	    اكسر;

	حالة SELECT_نما_سطر:
	    clip_invert_area(cb, (صحيح)cb->شروع.lnum, 0, (صحيح)cb->شروع.lnum,
			    (صحيح)Columns, CLIP_مجموعه);
	    cb->شروع.ستون = 0;
	    cb->انتها.ستون   = Columns;
	    اكسر;
    }

    cb->prev = cb->شروع;

# ifdef DEBUG_SELECTION
    چاپ_کن("Selection started at (%ld,%d)\n", cb->شروع.lnum, cb->شروع.ستون);
# endif
}

/*
 * Continue processing the selection
 */
    فراغ
clip_process_selection(
    صحيح		button,
    صحيح		ستون,
    صحيح		ردیف,
    int_u	repeated_click)
{
    Clipboard_T	*cb = &clip_ستاره;
    صحيح		diff;
    صحيح		slen = 1;	// cursor shape width

    إذا (button == MOUSE_RELEASE)
    {
	إذا (cb->state != SELECT_درون_پیشرفت)
	    أرجع;

	// Check to make sure we have something selected
	إذا (cb->شروع.lnum == cb->انتها.lnum && cb->شروع.ستون == cb->انتها.ستون)
	{
# ifdef FEAT_GUI
	    إذا (gui.درون_use)
		gui_update_cursor(FALSE, FALSE);
# endif
	    cb->state = SELECT_CLEARED;
	    أرجع;
	}

# ifdef DEBUG_SELECTION
	چاپ_کن("Selection ended: (%ld,%d) to (%ld,%d)\n", cb->شروع.lnum,
		cb->شروع.ستون, cb->انتها.lnum, cb->انتها.ستون);
# endif
	إذا (clip_isautosel_ستاره() || clip_isautosel_plus()
		|| (
# ifdef FEAT_GUI
		    gui.درون_use ? (vim_strchr(p_go, GO_ASELML) != عدم) :
# endif
		    clip_autoselectml))
	    clip_copy_modeless_selection(FALSE);
# ifdef FEAT_GUI
	إذا (gui.درون_use)
	    gui_update_cursor(FALSE, FALSE);
# endif

	cb->state = SELECT_انجامشده;
	أرجع;
    }

    ردیف = check_ردیف(ردیف);
    ستون = check_ستون(ستون);
    ستون = mb_fix_ستون(ستون, ردیف);

    إذا (ستون == (صحيح)cb->prev.ستون && ردیف == cb->prev.lnum && !repeated_click)
	أرجع;

    /*
     * When extending the selection with the right mouse button, swap the
     * start and end if the position is before half the selection
     */
    إذا (cb->state == SELECT_انجامشده && button == MOUSE_راست)
    {
	/*
	 * If the click is before the start, or the click is inside the
	 * selection and the start is the closest side, set the origin to the
	 * end of the selection.
	 */
	إذا (clip_compare_pos(ردیف, ستون, (صحيح)cb->شروع.lnum, cb->شروع.ستون) < 0
		|| (clip_compare_pos(ردیف, ستون,
					   (صحيح)cb->انتها.lnum, cb->انتها.ستون) < 0
		    && (((cb->شروع.lnum == cb->انتها.lnum
			    && cb->انتها.ستون - ستون > ستون - cb->شروع.ستون))
			|| ((diff = (cb->انتها.lnum - ردیف) -
						   (ردیف - cb->شروع.lnum)) > 0
			    || (diff == 0 && ستون < (صحيح)(cb->شروع.ستون +
							 cb->انتها.ستون) / 2)))))
	{
	    cb->origin_ردیف = (short_u)cb->انتها.lnum;
	    cb->origin_شروع_ستون = cb->انتها.ستون - 1;
	    cb->origin_انتها_ستون = cb->انتها.ستون;
	}
	وإلا
	{
	    cb->origin_ردیف = (short_u)cb->شروع.lnum;
	    cb->origin_شروع_ستون = cb->شروع.ستون;
	    cb->origin_انتها_ستون = cb->شروع.ستون;
	}
	إذا (cb->نما == SELECT_نما_کلمه && !repeated_click)
	    cb->نما = SELECT_نما_CHAR;
    }

    // set state, for when using the right mouse button
    cb->state = SELECT_درون_پیشرفت;

# ifdef DEBUG_SELECTION
    چاپ_کن("Selection extending to (%d,%d)\n", ردیف, ستون);
# endif

    إذا (repeated_click && ++cb->نما > SELECT_نما_سطر)
	cb->نما = SELECT_نما_CHAR;

    بدل (cb->نما)
    {
	حالة SELECT_نما_CHAR:
	    // If we're on a different line, find where the line ends
	    إذا (ردیف != cb->prev.lnum)
		cb->کلمه_انتها_ستون = clip_get_سطر_انتها(cb, ردیف);

	    // See if we are before or after the origin of the selection
	    إذا (clip_compare_pos(ردیف, ستون, cb->origin_ردیف,
						   cb->origin_شروع_ستون) >= 0)
	    {
		إذا (ستون >= (صحيح)cb->کلمه_انتها_ستون)
		    clip_update_modeless_selection(cb, cb->origin_ردیف,
			    cb->origin_شروع_ستون, ردیف, (صحيح)Columns);
		وإلا
		{
		    إذا (has_mbyte && mb_lefthalve(ردیف, ستون))
			slen = 2;
		    clip_update_modeless_selection(cb, cb->origin_ردیف,
			    cb->origin_شروع_ستون, ردیف, ستون + slen);
		}
	    }
	    وإلا
	    {
		إذا (has_mbyte
			&& mb_lefthalve(cb->origin_ردیف, cb->origin_شروع_ستون))
		    slen = 2;
		إذا (ستون >= (صحيح)cb->کلمه_انتها_ستون)
		    clip_update_modeless_selection(cb, ردیف, cb->کلمه_انتها_ستون,
			    cb->origin_ردیف, cb->origin_شروع_ستون + slen);
		وإلا
		    clip_update_modeless_selection(cb, ردیف, ستون,
			    cb->origin_ردیف, cb->origin_شروع_ستون + slen);
	    }
	    اكسر;

	حالة SELECT_نما_کلمه:
	    // If we are still within the same word, do nothing
	    إذا (ردیف == cb->prev.lnum && ستون >= (صحيح)cb->کلمه_شروع_ستون
		    && ستون < (صحيح)cb->کلمه_انتها_ستون && !repeated_click)
		أرجع;

	    // Get new word boundaries
	    clip_get_کلمه_boundaries(cb, ردیف, ستون);

	    // Handle being after the origin point of selection
	    إذا (clip_compare_pos(ردیف, ستون, cb->origin_ردیف,
		    cb->origin_شروع_ستون) >= 0)
		clip_update_modeless_selection(cb, cb->origin_ردیف,
			cb->origin_شروع_ستون, ردیف, cb->کلمه_انتها_ستون);
	    وإلا
		clip_update_modeless_selection(cb, ردیف, cb->کلمه_شروع_ستون,
			cb->origin_ردیف, cb->origin_انتها_ستون);
	    اكسر;

	حالة SELECT_نما_سطر:
	    إذا (ردیف == cb->prev.lnum && !repeated_click)
		أرجع;

	    إذا (clip_compare_pos(ردیف, ستون, cb->origin_ردیف,
		    cb->origin_شروع_ستون) >= 0)
		clip_update_modeless_selection(cb, cb->origin_ردیف, 0, ردیف,
			(صحيح)Columns);
	    وإلا
		clip_update_modeless_selection(cb, ردیف, 0, cb->origin_ردیف,
			(صحيح)Columns);
	    اكسر;
    }

    cb->prev.lnum = ردیف;
    cb->prev.ستون  = ستون;

# ifdef DEBUG_SELECTION
	چاپ_کن("Selection is: (%ld,%d) to (%ld,%d)\n", cb->شروع.lnum,
		cb->شروع.ستون, cb->انتها.lnum, cb->انتها.ستون);
# endif
}

# إذا defined(FEAT_GUI)
/*
 * Redraw part of the selection if character at "row,col" is inside of it.
 * Only used for the GUI.
 */
    فراغ
clip_may_redraw_selection(صحيح ردیف, صحيح ستون, صحيح len)
{
    صحيح		شروع = ستون;
    صحيح		انتها = ستون + len;

    إذا (clip_ستاره.state != SELECT_CLEARED
	    && ردیف >= clip_ستاره.شروع.lnum
	    && ردیف <= clip_ستاره.انتها.lnum)
    {
	إذا (ردیف == clip_ستاره.شروع.lnum && شروع < (صحيح)clip_ستاره.شروع.ستون)
	    شروع = clip_ستاره.شروع.ستون;
	إذا (ردیف == clip_ستاره.انتها.lnum && انتها > (صحيح)clip_ستاره.انتها.ستون)
	    انتها = clip_ستاره.انتها.ستون;
	إذا (انتها > شروع)
	    clip_invert_area(&clip_ستاره, ردیف, شروع, ردیف, انتها, 0);
    }
}
# endif

/*
 * Called from outside to clear selected region from the display
 */
    فراغ
clip_clear_selection(Clipboard_T *cbd)
{

    إذا (cbd->state == SELECT_CLEARED)
	أرجع;

    clip_invert_area(cbd, (صحيح)cbd->شروع.lnum, cbd->شروع.ستون,
				 (صحيح)cbd->انتها.lnum, cbd->انتها.ستون, CLIP_CLEAR);
    cbd->state = SELECT_CLEARED;
}

/*
 * Clear the selection if any lines from "row1" to "row2" are inside of it.
 */
    فراغ
clip_may_clear_selection(صحيح row1, صحيح row2)
{
    إذا (clip_ستاره.state == SELECT_انجامشده
	    && row2 >= clip_ستاره.شروع.lnum
	    && row1 <= clip_ستاره.انتها.lnum)
	clip_clear_selection(&clip_ستاره);
}

/*
 * Called before the screen is scrolled up or down.  Adjusts the line numbers
 * of the selection.  Call with big number when clearing the screen.
 */
    فراغ
clip_scroll_selection(
    صحيح	    ردیفها_2)		// negative for scroll down
{
    صحيح	    lnum;

    إذا (clip_ستاره.state == SELECT_CLEARED)
	أرجع;

    lnum = clip_ستاره.شروع.lnum - ردیفها_2;
    إذا (lnum <= 0)
	clip_ستاره.شروع.lnum = 0;
    وإلا إذا (lnum >= screen_ردیفها)	// scrolled off of the screen
	clip_ستاره.state = SELECT_CLEARED;
    وإلا
	clip_ستاره.شروع.lnum = lnum;

    lnum = clip_ستاره.انتها.lnum - ردیفها_2;
    إذا (lnum < 0)			// scrolled off of the screen
	clip_ستاره.state = SELECT_CLEARED;
    وإلا إذا (lnum >= screen_ردیفها)
	clip_ستاره.انتها.lnum = screen_ردیفها - 1;
    وإلا
	clip_ستاره.انتها.lnum = lnum;
}

/*
 * Copy the currently selected area into the '*' or '+' register so it will be
 * available for pasting.
 * When "both" is TRUE also copy to the other register.
 */
    فراغ
clip_copy_modeless_selection(صحيح both استفادهنشده)
{
    // The info for the modeless selection is stored in '*' register, however if
    // we are using the '+' register for modeless autoselect, we copy to
    // clip_plus instead while using the info in clip_star.
    Clipboard_T *cbd = clip_isautosel_plus() ? &clip_plus : &clip_ستاره;
    char_u	*میانگیر;
    char_u	*bufp;
    صحيح		ردیف;
    صحيح		شروع_ستون;
    صحيح		انتها_ستون;
    صحيح		سطر_انتها_ستون;
    صحيح		جمع_کن_newline_flag = FALSE;
    صحيح		len;
    char_u	*p;
    صحيح		row1 = clip_ستاره.شروع.lnum;
    صحيح		col1 = clip_ستاره.شروع.ستون;
    صحيح		row2 = clip_ستاره.انتها.lnum;
    صحيح		col2 = clip_ستاره.انتها.ستون;

    // Can't use ScreenLines unless initialized
    إذا (Screen_سطرها == عدم)
	أرجع;

    /*
     * Make sure row1 <= row2, and if row1 == row2 that col1 <= col2.
     */
    إذا (row1 > row2)
    {
	ردیف = row1; row1 = row2; row2 = ردیف;
	ردیف = col1; col1 = col2; col2 = ردیف;
    }
    وإلا إذا (row1 == row2 && col1 > col2)
    {
	ردیف = col1; col1 = col2; col2 = ردیف;
    }
# ifdef FEAT_PROP_POPUP
    إذا (col1 < clip_ستاره.min_ستون)
	col1 = clip_ستاره.min_ستون;
    إذا (col2 > clip_ستاره.max_ستون)
	col2 = clip_ستاره.max_ستون;
    إذا (row1 > clip_ستاره.max_ردیف || row2 < clip_ستاره.min_ردیف)
	أرجع;
    إذا (row1 < clip_ستاره.min_ردیف)
	row1 = clip_ستاره.min_ردیف;
    إذا (row2 > clip_ستاره.max_ردیف)
	row2 = clip_ستاره.max_ردیف;
# endif
    // correct starting point for being on right half of double-wide char
    p = Screen_سطرها + سطر_جابه_جایی[row1];
    إذا (enc_dbcs != 0)
	col1 -= (*mb_سر_off)(p, p + col1);
    وإلا إذا (enc_utf8 && p[col1] == 0)
	--col1;

    // Create a temporary buffer for storing the text
    len = (row2 - row1 + 1) * Columns + 1;
    إذا (enc_dbcs != 0)
	len *= 2;	// max. 2 bytes per display cell
    وإلا إذا (enc_utf8)
	len *= MB_MAXBYTES;
    // A newline is added after each row
    len += row2 - row1 + 2;
    میانگیر = alloc(len);
    إذا (میانگیر == عدم)	    // out of memory
	أرجع;

    // Process each row in the selection
    لكل (bufp = میانگیر, ردیف = row1; ردیف <= row2; ردیف++)
    {
	إذا (ردیف == row1)
	    شروع_ستون = col1;
	وإلا
# ifdef FEAT_PROP_POPUP
	    شروع_ستون = clip_ستاره.min_ستون;
# وإلا
	    شروع_ستون = 0;
# endif

	إذا (ردیف == row2)
	    انتها_ستون = col2;
	وإلا
# ifdef FEAT_PROP_POPUP
	    انتها_ستون = clip_ستاره.max_ستون;
# وإلا
	    انتها_ستون = Columns;
# endif

	سطر_انتها_ستون = clip_get_سطر_انتها(&clip_ستاره, ردیف);

	// See if we need to nuke some trailing whitespace
	إذا (انتها_ستون >=
# ifdef FEAT_PROP_POPUP
		clip_ستاره.max_ستون
# وإلا
		Columns
# endif
		    && (ردیف < row2 || انتها_ستون > سطر_انتها_ستون))
	{
	    // Get rid of trailing whitespace
	    انتها_ستون = سطر_انتها_ستون;
	    إذا (انتها_ستون < شروع_ستون)
		انتها_ستون = شروع_ستون;

	    // If the last line extended to the end, add an extra newline
	    إذا (ردیف == row2)
		جمع_کن_newline_flag = TRUE;
	}

	// If after the first row, we need to always add a newline
	إذا (ردیف > row1 && !سطر_Wraps[ردیف - 1])
	    *bufp++ = NL;

	// Safetey check for in case resizing went wrong
	إذا (ردیف < screen_ردیفها && انتها_ستون <= screen_Columns)
	{
	    إذا (enc_dbcs != 0)
	    {
		صحيح	i;

		p = Screen_سطرها + سطر_جابه_جایی[ردیف];
		لكل (i = شروع_ستون; i < انتها_ستون; ++i)
		    إذا (enc_dbcs == DBCS_JPNU && p[i] == 0x8e)
		    {
			// single-width double-byte char
			*bufp++ = 0x8e;
			*bufp++ = ScreenLines2[سطر_جابه_جایی[ردیف] + i];
		    }
		    وإلا
		    {
			*bufp++ = p[i];
			إذا (MB_BYTE2LEN(p[i]) == 2)
			    *bufp++ = p[++i];
		    }
	    }
	    وإلا إذا (enc_utf8)
	    {
		صحيح	off;
		صحيح	i;

		off = سطر_جابه_جایی[ردیف];
		لكل (i = شروع_ستون; i < انتها_ستون; ++i)
		{
		    // The base character is either in ScreenLinesUC[] or
		    // ScreenLines[].
		    إذا (Screen_سطرها_UC[off + i] == 0)
			*bufp++ = Screen_سطرها[off + i];
		    وإلا
			// Includes the composing characters.
			bufp += utfc_char2bytes(off + i, bufp);
		    // Skip right half of double-wide character.
		    إذا (Screen_سطرها[off + i + 1] == 0)
			++i;
		}
	    }
	    وإلا
	    {
		STRNCPY(bufp, Screen_سطرها + سطر_جابه_جایی[ردیف] + شروع_ستون,
							 انتها_ستون - شروع_ستون);
		bufp += انتها_ستون - شروع_ستون;
	    }
	}
    }

    // Add a newline at the end if the selection ended there
    إذا (جمع_کن_newline_flag)
	*bufp++ = NL;

    // First cleanup any old selection and become the owner.
    clip_free_selection(cbd);
    clip_own_selection(cbd);

    // Yank the text into the '*' register.
    clip_yank_selection(MCHAR, میانگیر, (دراز)(bufp - میانگیر), cbd);

    // Make the register contents available to the outside world.
    clip_gen_مجموعه_selection(cbd);

# ifdef FEAT_X11
    إذا (both)
    {
	Clipboard_T *other = cbd == &clip_ستاره ? &clip_plus : &clip_ستاره;
	// Do the same for the '+' register.
	clip_free_selection(other);
	clip_own_selection(other);
	clip_yank_selection(MCHAR, میانگیر, (دراز)(bufp - میانگیر), other);
	clip_gen_مجموعه_selection(other);
    }
# endif
    vim_free(میانگیر);
}

    فراغ
clip_gen_مجموعه_selection(Clipboard_T *cbd)
{
    إذا (!clip_did_مجموعه_selection)
    {
	// Updating postponed, so that accessing the system clipboard won't
	// hang Vim when accessing it many times (e.g. on a :g command).
	إذا ((cbd == &clip_plus && (clip_unnamed_saved & CLIP_UNNAMED_PLUS))
		|| (cbd == &clip_ستاره && (clip_unnamed_saved & CLIP_UNNAMED)))
	{
	    clipboard_needs_update = TRUE;
	    أرجع;
	}
    }
# إذا defined(FEAT_XCLIPBOARD) || defined(FEAT_WAYLAND_CLIPBOARD)
#  ifdef FEAT_GUI
    إذا (gui.درون_use)
	clip_mch_مجموعه_selection(cbd);
    وإلا
#  endif
    {
	إذا (clipmethod == CLIPMETHOD_WAYLAND)
	{
#  ifdef FEAT_WAYLAND_CLIPBOARD
	    clip_wl_مجموعه_selection(cbd);
#  endif
	}
	وإلا إذا (clipmethod == CLIPMETHOD_X11)
	{
#  ifdef FEAT_XCLIPBOARD
	    clip_xterm_مجموعه_selection(cbd);
#  endif
	}
    }
# وإلا
    clip_mch_مجموعه_selection(cbd);
# endif
}

    ساكن فراغ
clip_gen_request_selection(Clipboard_T *cbd)
{
# إذا defined(FEAT_XCLIPBOARD) || defined(FEAT_WAYLAND_CLIPBOARD)
#  ifdef FEAT_GUI
    إذا (gui.درون_use)
	clip_mch_request_selection(cbd);
    وإلا
#  endif
    {
	إذا (clipmethod == CLIPMETHOD_WAYLAND)
	{
#  ifdef FEAT_WAYLAND_CLIPBOARD
	    clip_wl_request_selection(cbd);
#  endif
	}
	وإلا إذا (clipmethod == CLIPMETHOD_X11)
	{
#  ifdef FEAT_XCLIPBOARD
	    clip_xterm_request_selection(cbd);
#  endif
	}
    }
# وإلا
    clip_mch_request_selection(cbd);
# endif
}

# إذا (defined(FEAT_X11) && defined(FEAT_XCLIPBOARD) && defined(USE_SYSTEM))
    ساكن صحيح
clip_x11_owner_exists(Clipboard_T *cbd)
{
    أرجع XGetSelectionOwner(X_DISPLAY, cbd->sel_atom) != None;
}
# endif

# إذا (defined(FEAT_X11) || defined(FEAT_WAYLAND_CLIPBOARD)) \
	&& defined(USE_SYSTEM)
    صحيح
clip_gen_owner_exists(Clipboard_T *cbd استفادهنشده)
{
#  إذا defined(FEAT_XCLIPBOARD) || defined(FEAT_WAYLAND_CLIPBOARD)
#   ifdef FEAT_GUI_GTK
    إذا (gui.درون_use)
	أرجع clip_gtk_owner_exists(cbd);
    وإلا
#   endif
    {
	إذا (clipmethod == CLIPMETHOD_WAYLAND)
	{
#   ifdef FEAT_WAYLAND_CLIPBOARD
	    أرجع clip_wl_owner_exists(cbd);
#   endif
	}
	وإلا إذا (clipmethod == CLIPMETHOD_X11)
	{
#   ifdef FEAT_XCLIPBOARD
	    أرجع clip_x11_owner_exists(cbd);
#   endif
	}
	وإلا
	    أرجع FALSE;
    }
#  وإلا
    أرجع TRUE;
#  endif
}
# endif

/*
 * Stuff for the X clipboard.  Shared between VMS and Unix.
 */

# إذا defined(FEAT_XCLIPBOARD) || defined(FEAT_GUI_X11)
#  include <X11/Xatom.h>
#  include <X11/Intrinsic.h>

/*
 * Open the application context (if it hasn't been opened yet).
 * Used for Motif GUI and the xterm clipboard.
 */
    فراغ
open_app_context(فراغ)
{
    إذا (app_context == عدم)
    {
	XtToolkitInitialize();
	app_context = XtCreateApplicationContext();
    }
}

ساكن Atom	vim_atom;	// Vim's own special selection format
ساكن Atom	vimenc_atom;	// Vim's extended selection format
ساكن Atom	vim_mt_atom;	// Vim's own special selection format (in mime
				// type format)
ساكن Atom	vimenc_mt_atom;	// Vim's extended selection format (in mime type
				// format)
ساكن Atom	utf8_atom;
ساكن Atom	compound_متن_atom;
ساكن Atom	متن_atom;
ساكن Atom	targets_atom;
ساكن Atom	timestamp_atom;	// Used to get a timestamp

    فراغ
x11_راهاندازی_atoms(Display *dpy)
{
    vim_atom	       = XInternAtom(dpy, VIM_ATOM_NAME,   False);
    vimenc_atom	       = XInternAtom(dpy, VIMENC_ATOM_NAME,False);
    vim_mt_atom	       = XInternAtom(dpy, VIM_MIMETYPE_NAME,   False);
    vimenc_mt_atom     = XInternAtom(dpy, VIMENC_MIMETYPE_NAME,False);
    utf8_atom	       = XInternAtom(dpy, "UTF8_STRING",   False);
    compound_متن_atom = XInternAtom(dpy, "COMPOUND_TEXT", False);
    متن_atom	       = XInternAtom(dpy, "TEXT",	   False);
    targets_atom       = XInternAtom(dpy, "TARGETS",	   False);
    clip_ستاره.sel_atom = XA_PRIMARY;
    clip_plus.sel_atom = XInternAtom(dpy, "CLIPBOARD",	   False);
    timestamp_atom     = XInternAtom(dpy, "TIMESTAMP",	   False);
}

/*
 * X Selection stuff, for cutting and pasting text to other windows.
 */

    ساكن Boolean
clip_x11_convert_selection_cb(
    Widget	w استفادهنشده,
    Atom	*sel_atom,
    Atom	*هدف,
    Atom	*type,
    Xt_اشاره_گر	*مقدار,
    long_u	*درازی,
    صحيح		*format)
{
    ساكن char_u   *save_نتیجه = عدم;
    ساكن long_u   save_درازی = 0;
    char_u	    *string;
    صحيح		    motion_type;
    Clipboard_T    *cbd;
    صحيح		    i;

    إذا (*sel_atom == clip_plus.sel_atom)
	cbd = &clip_plus;
    وإلا
	cbd = &clip_ستاره;

    إذا (!cbd->owned)
	أرجع False;	    // Shouldn't ever happen

    // requestor wants to know what target types we support
    إذا (*هدف == targets_atom)
    {
	ساكن Atom array[9];

	*مقدار = (Xt_اشاره_گر)array;
	i = 0;
	array[i++] = targets_atom;
	array[i++] = vimenc_atom;
	array[i++] = vim_atom;
	array[i++] = vimenc_mt_atom;
	array[i++] = vim_mt_atom;
	إذا (enc_utf8)
	    array[i++] = utf8_atom;
	array[i++] = XA_STRING;
	array[i++] = متن_atom;
	array[i++] = compound_متن_atom;

	*type = XA_ATOM;
	// This used to be: *format = sizeof(Atom) * 8; but that caused
	// crashes on 64 bit machines. (Peter Derr)
	*format = 32;
	*درازی = i;
	أرجع True;
    }

    إذا (       *هدف != XA_STRING
	    && *هدف != vimenc_atom
	    && *هدف != vimenc_mt_atom
	    && (*هدف != utf8_atom || !enc_utf8)
	    && *هدف != vim_atom
	    && *هدف != vim_mt_atom
	    && *هدف != متن_atom
	    && *هدف != compound_متن_atom)
	أرجع False;

    clip_get_selection(cbd);
    motion_type = clip_convert_selection(&string, درازی, cbd);
    إذا (motion_type < 0)
	أرجع False;

    // For our own format, the first byte contains the motion type
    إذا (*هدف == vim_atom || *هدف == vim_mt_atom)
	(*درازی)++;

    // Our own format with encoding: motion 'encoding' NUL text
    إذا (*هدف == vimenc_atom || *هدف == vimenc_mt_atom)
	*درازی += STRLEN(p_enc) + 2;

    إذا (save_درازی < *درازی || save_درازی / 2 >= *درازی)
	*مقدار = XtRealloc((حرف *)save_نتیجه, (Cardinal)*درازی + 1);
    وإلا
	*مقدار = save_نتیجه;
    إذا (*مقدار == عدم)
    {
	vim_free(string);
	أرجع False;
    }
    save_نتیجه = (char_u *)*مقدار;
    save_درازی = *درازی;

    إذا (*هدف == XA_STRING || (*هدف == utf8_atom && enc_utf8))
    {
	mch_memmove(save_نتیجه, string, (size_t)(*درازی));
	*type = *هدف;
    }
    وإلا إذا (*هدف == compound_متن_atom || *هدف == متن_atom)
    {
	XTextProperty	متن_prop;
	حرف		*string_nt = (حرف *)save_نتیجه;
	صحيح		conv_نتیجه;

	// create NUL terminated string which XmbTextListToTextProperty wants
	mch_memmove(string_nt, string, (size_t)*درازی);
	string_nt[*درازی] = NUL;
	conv_نتیجه = Xmb_متن_List_مقصد_متن_Property(X_DISPLAY, &string_nt,
					   1, XCompound_متن_Style, &متن_prop);
	إذا (conv_نتیجه != Success)
	{
	    vim_free(string);
	    أرجع False;
	}
	*مقدار = (Xt_اشاره_گر)(متن_prop.مقدار);	//    from plain text
	*درازی = متن_prop.nitems;
	*type = compound_متن_atom;
	XtFree((حرف *)save_نتیجه);
	save_نتیجه = (char_u *)*مقدار;
	save_درازی = *درازی;
    }
    وإلا إذا (*هدف == vimenc_atom || *هدف == vimenc_mt_atom)
    {
	صحيح l = STRLEN(p_enc);

	save_نتیجه[0] = motion_type;
	STRCPY(save_نتیجه + 1, p_enc);
	mch_memmove(save_نتیجه + l + 2, string, (size_t)(*درازی - l - 2));
	إذا (*هدف == vimenc_atom)
	    *type = vimenc_atom;
	وإلا
	    *type = vimenc_mt_atom;
    }
    وإلا
    {
	save_نتیجه[0] = motion_type;
	mch_memmove(save_نتیجه + 1, string, (size_t)(*درازی - 1));
	إذا (*هدف == vim_atom)
	    *type = vim_atom;
	وإلا
	    *type = vim_mt_atom;
    }
    *format = 8;	    // 8 bits per char
    vim_free(string);
    أرجع True;
}

    ساكن فراغ
clip_x11_lose_ownership_cb(Widget w استفادهنشده, Atom *sel_atom)
{
    إذا (*sel_atom == clip_plus.sel_atom)
	clip_lose_selection(&clip_plus);
    وإلا
	clip_lose_selection(&clip_ستاره);
}

    ساكن فراغ
clip_x11_notify_cb(Widget w استفادهنشده, Atom *sel_atom استفادهنشده, Atom *هدف استفادهنشده)
{
    // To prevent automatically freeing the selection value.
}

/*
 * Property callback to get a timestamp for XtOwnSelection.
 */
#  إذا defined(FEAT_X11) && defined(FEAT_XCLIPBOARD)
    ساكن فراغ
clip_x11_timestamp_cb(
    Widget	w,
    Xt_اشاره_گر	n استفادهنشده,
    XEvent	*event,
    Boolean	*cont استفادهنشده)
{
    Atom	    actual_type;
    صحيح		    format;
    بیعلامت  دراز  nitems, بایتها_after;
    بیعلامت حرف   *prop=عدم;
    XPropertyEvent  *xproperty=&event->xproperty;

    // Must be a property notify, state can't be Delete (True), has to be
    // one of the supported selection types.
    إذا (event->type != PropertyNotify || xproperty->state
	    || (xproperty->atom != clip_ستاره.sel_atom
				    && xproperty->atom != clip_plus.sel_atom))
	أرجع;

    إذا (XGet_پنجره_Property(xproperty->display, xproperty->پنجره_2,
	  xproperty->atom, 0, 0, False, timestamp_atom, &actual_type, &format,
						&nitems, &بایتها_after, &prop))
	أرجع;

    إذا (prop)
	XFree(prop);

    // Make sure the property type is "TIMESTAMP" and it's 32 bits.
    إذا (actual_type != timestamp_atom || format != 32)
	أرجع;

    // Get the selection, using the event timestamp.
    إذا (XtOwnSelection(w, xproperty->atom, xproperty->زمان,
	    clip_x11_convert_selection_cb, clip_x11_lose_ownership_cb,
	    clip_x11_notify_cb) == OK)
    {
	// Set the "owned" flag now, there may have been a call to
	// lose_ownership_cb in between.
	إذا (xproperty->atom == clip_plus.sel_atom)
	    clip_plus.owned = TRUE;
	وإلا
	    clip_ستاره.owned = TRUE;
    }
}

    فراغ
x11_راهاندازی_selection(Widget w)
{
    Xt_جمع_کن_Event_Handler(w, Property_Change_ماسک, False,
	    /*(XtEventHandler)*/clip_x11_timestamp_cb, (Xt_اشاره_گر)عدم);
}
#  endif

    ساكن فراغ
clip_x11_request_selection_cb(
    Widget	w استفادهنشده,
    Xt_اشاره_گر	success,
    Atom	*sel_atom,
    Atom	*type,
    Xt_اشاره_گر	مقدار,
    long_u	*درازی,
    صحيح		*format)
{
    صحيح		motion_type = MAUTO;
    long_u	len;
    char_u	*p;
    حرف	**متن_list = عدم;
    Clipboard_T	*cbd;
    char_u	*tmpbuf = عدم;

    إذا (*sel_atom == clip_plus.sel_atom)
	cbd = &clip_plus;
    وإلا
	cbd = &clip_ستاره;

    إذا (مقدار == عدم || *درازی == 0)
    {
	clip_free_selection(cbd);	// nothing received, clear register
	*(صحيح *)success = FALSE;
	أرجع;
    }
    p = (char_u *)مقدار;
    len = *درازی;
    إذا (*type == vim_atom || *type == vim_mt_atom)
    {
	motion_type = *p++;
	len--;
    }

    وإلا إذا (*type == vimenc_atom || *type == vimenc_mt_atom)
    {
	char_u		*enc;
	vimconv_T	conv;
	صحيح		convlen;

	motion_type = *p++;
	--len;

	enc = p;
	p += STRLEN(p) + 1;
	len -= p - enc;

	// If the encoding of the text is different from 'encoding', attempt
	// converting it.
	conv.vc_type = CONV_NONE;
	convert_راهاندازی(&conv, enc, p_enc);
	إذا (conv.vc_type != CONV_NONE)
	{
	    convlen = len;	// Need to use an int here.
	    tmpbuf = string_convert(&conv, p, &convlen);
	    len = convlen;
	    إذا (tmpbuf != عدم)
		p = tmpbuf;
	    convert_راهاندازی(&conv, عدم, عدم);
	}
    }

    وإلا إذا (*type == compound_متن_atom
	    || *type == utf8_atom
	    || (enc_dbcs != 0 && *type == متن_atom))
    {
	XTextProperty	متن_prop;
	صحيح		n_متن = 0;
	صحيح		وضعیت;

	متن_prop.مقدار = (بیعلامت حرف *)مقدار;
	متن_prop.encoding = *type;
	متن_prop.format = *format;
	متن_prop.nitems = len;
#  إذا defined(X_HAVE_UTF8_STRING)
	إذا (*type == utf8_atom)
	    وضعیت = Xutf8_متن_Property_مقصد_متن_List(X_DISPLAY, &متن_prop,
							 &متن_list, &n_متن);
	وإلا
#  endif
	    وضعیت = Xmb_متن_Property_مقصد_متن_List(X_DISPLAY, &متن_prop,
							 &متن_list, &n_متن);
	إذا (وضعیت != Success || n_متن < 1)
	{
	    *(صحيح *)success = FALSE;
	    أرجع;
	}
	p = (char_u *)متن_list[0];
	len = STRLEN(p);
    }
    clip_yank_selection(motion_type, p, (دراز)len, cbd);

    إذا (متن_list != عدم)
	XFreeStringList(متن_list);
    vim_free(tmpbuf);
    XtFree((حرف *)مقدار);
    *(صحيح *)success = TRUE;
}

    فراغ
clip_x11_request_selection(
    Widget	myShell,
    Display	*dpy,
    Clipboard_T	*cbd)
{
    XEvent	event;
    Atom	type;
    ساكن صحيح	success;
    صحيح		i;
    time_t	شروع_زمان;
    صحيح		timed_بیرون = FALSE;

    لكل (i = 0; i < 8; i++)
    {
	بدل (i)
	{
	    حالة 0:  type = vimenc_mt_atom;	اكسر;
	    حالة 1:  type = vimenc_atom;	اكسر;
	    حالة 2:  type = vim_mt_atom;	اكسر;
	    حالة 3:  type = vim_atom;		اكسر;
	    حالة 4:  type = utf8_atom;		اكسر;
	    حالة 5:  type = compound_متن_atom; اكسر;
	    حالة 6:  type = متن_atom;		اكسر;
	    افتراضي: type = XA_STRING;
	}
	إذا (type == utf8_atom
#  إذا defined(X_HAVE_UTF8_STRING)
		&& !enc_utf8
#  endif
		)
	    // Only request utf-8 when 'encoding' is utf8 and
	    // Xutf8TextPropertyToTextList is available.
	    استمر;
	success = MAYBE;
	Xt_Get_Selection_مقدار(myShell, cbd->sel_atom, type,
	    clip_x11_request_selection_cb, (Xt_اشاره_گر)&success, Current_زمان);

	// Make sure the request for the selection goes out before waiting for
	// a response.
	XFlush(dpy);

	/*
	 * Wait for result of selection request, otherwise if we type more
	 * characters, then they will appear before the one that requested the
	 * paste!  Don't worry, we will catch up with any other events later.
	 */
	شروع_زمان = زمان(عدم);
	طالما (success == MAYBE)
	{
	    إذا (XCheckTypedEvent(dpy, PropertyNotify, &event)
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
		استمر;
	    }

	    // Time out after 2 to 3 seconds to avoid that we hang when the
	    // other process doesn't respond.  Note that the SelectionNotify
	    // event may still come later when the selection owner comes back
	    // to life and the text gets inserted unexpectedly.  Don't know
	    // why that happens or how to avoid that :-(.
	    إذا (زمان(عدم) > شروع_زمان + 2)
	    {
		timed_بیرون = TRUE;
		اكسر;
	    }

	    // Do we need this?  Probably not.
	    XSync(dpy, False);

	    // Wait for 1 msec to avoid that we eat up all CPU time.
	    ui_delay(1L, TRUE);
	}

	إذا (success == TRUE)
	    أرجع;

	// don't do a retry with another type after timing out, otherwise we
	// hang for 15 seconds.
	إذا (timed_بیرون)
	    اكسر;
    }

    // Final fallback position - use the X CUT_BUFFER0 store
    yank_cut_buffer0(dpy, cbd);
}

    فراغ
clip_x11_lose_selection(Widget myShell, Clipboard_T *cbd)
{
    XtDisownSelection(myShell, cbd->sel_atom,
				Xt_آخرین_Timestamp_Processed(XtDisplay(myShell)));
}

    صحيح
clip_x11_own_selection(Widget myShell, Clipboard_T *cbd)
{
    // When using the GUI we have proper timestamps, use the one of the last
    // event.  When in the console we don't get events (the terminal gets
    // them), Get the time by a zero-length append, clip_x11_timestamp_cb will
    // be called with the current timestamp.
#  ifdef FEAT_GUI
    إذا (gui.درون_use)
    {
	إذا (XtOwnSelection(myShell, cbd->sel_atom,
	       Xt_آخرین_Timestamp_Processed(XtDisplay(myShell)),
	       clip_x11_convert_selection_cb, clip_x11_lose_ownership_cb,
	       clip_x11_notify_cb) == False)
	    أرجع FAIL;
    }
    وإلا
#  endif
    {
	إذا (!XChangeProperty(XtDisplay(myShell), Xt_پنجره(myShell),
		  cbd->sel_atom, timestamp_atom, 32, Prop_نما_Append, عدم, 0))
	    أرجع FAIL;
    }
    // Flush is required in a terminal as nothing else is doing it.
    XFlush(XtDisplay(myShell));
    أرجع OK;
}

/*
 * Send the current selection to the clipboard.  Do nothing for X because we
 * will fill in the selection only when requested by another app.
 */
    فراغ
clip_x11_مجموعه_selection(Clipboard_T *cbd استفادهنشده)
{
}

# endif

# إذا (defined(FEAT_XCLIPBOARD) || defined(FEAT_GUI_X11) || defined(FEAT_GUI_GTK)) \
	&& !defined(USE_GTK4)
/*
 * Get the contents of the X CUT_BUFFER0 and put it in "cbd".
 */
    فراغ
yank_cut_buffer0(Display *dpy, Clipboard_T *cbd)
{
    صحيح		nbytes = 0;
    char_u	*میانگیر = (char_u *)XFetch_میانگیر(dpy, &nbytes, 0);

    إذا (nbytes > 0)
    {
	صحيح  انجامشده = FALSE;

	// CUT_BUFFER0 is supposed to be always latin1.  Convert to 'enc' when
	// using a multi-byte encoding.  Conversion between two 8-bit
	// character sets usually fails and the text might actually be in
	// 'enc' anyway.
	إذا (has_mbyte)
	{
	    char_u	*conv_buf;
	    vimconv_T	vc;

	    vc.vc_type = CONV_NONE;
	    إذا (convert_راهاندازی(&vc, (char_u *)"latin1", p_enc) == OK)
	    {
		conv_buf = string_convert(&vc, میانگیر, &nbytes);
		إذا (conv_buf != عدم)
		{
		    clip_yank_selection(MCHAR, conv_buf, (دراز)nbytes, cbd);
		    vim_free(conv_buf);
		    انجامشده = TRUE;
		}
		convert_راهاندازی(&vc, عدم, عدم);
	    }
	}
	إذا (!انجامشده)  // use the text without conversion
	    clip_yank_selection(MCHAR, میانگیر, (دراز)nbytes, cbd);
	XFree((فراغ *)میانگیر);
	إذا (p_verbose > 0)
	{
	    verbose_enter();
	    verb_پیام(_("Used CUT_BUFFER0 instead of empty selection"));
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
# إذا defined(FEAT_X11) && defined(FEAT_CLIPBOARD)
    فراغ
x11_export_final_selection(فراغ)
{
    Display	*dpy;
    char_u	*str = عدم;
    long_u	len = 0;
    صحيح		motion_type = -1;

#  ifdef FEAT_GUI
    إذا (gui.درون_use)
	dpy = X_DISPLAY;
    وإلا
#  endif
#  ifdef FEAT_XCLIPBOARD
	dpy = xterm_dpy;
#  وإلا
	أرجع;
#  endif

    // Get selection to export
    إذا (clip_plus.owned)
	motion_type = clip_convert_selection(&str, &len, &clip_plus);
    وإلا إذا (clip_ستاره.owned)
	motion_type = clip_convert_selection(&str, &len, &clip_ستاره);

    // Check it's OK
    إذا (dpy != عدم && str != عدم && motion_type >= 0
					       && len < 1024*1024 && len > 0)
    {
	صحيح ok = TRUE;

	// The CUT_BUFFER0 is supposed to always contain latin1.  Convert from
	// 'enc' when it is a multi-byte encoding.  When 'enc' is an 8-bit
	// encoding conversion usually doesn't work, so keep the text as-is.
	إذا (has_mbyte)
	{
	    vimconv_T	vc;

	    vc.vc_type = CONV_NONE;
	    إذا (convert_راهاندازی(&vc, p_enc, (char_u *)"latin1") == OK)
	    {
		صحيح	intlen = len;
		char_u	*conv_str;

		vc.vc_fail = TRUE;
		conv_str = string_convert(&vc, str, &intlen);
		len = intlen;
		إذا (conv_str != عدم)
		{
		    vim_free(str);
		    str = conv_str;
		}
		وإلا
		{
		    ok = FALSE;
		}
		convert_راهاندازی(&vc, عدم, عدم);
	    }
	    وإلا
	    {
		ok = FALSE;
	    }
	}

	// Do not store the string if conversion failed.  Better to use any
	// other selection than garbled text.
	إذا (ok)
	{
	    XStore_میانگیر(dpy, (حرف *)str, (صحيح)len, 0);
	    XFlush(dpy);
	}
    }

    vim_free(str);
}
# endif

    فراغ
clip_free_selection(Clipboard_T *cbd)
{
    yankreg_T *y_ptr = get_y_current();

    إذا (cbd == &clip_plus)
	مجموعه_y_current(get_y_ثبات(PLUS_ثبات));
    وإلا
	مجموعه_y_current(get_y_ثبات(ستاره_ثبات));
    free_yank_all();
    get_y_current()->y_اندازه = 0;
    مجموعه_y_current(y_ptr);
}

/*
 * Get the selected text and put it in register '*' or '+'.
 */
    فراغ
clip_get_selection(Clipboard_T *cbd)
{
    yankreg_T	*old_y_قبلی, *old_y_current;
    pos_T	old_cursor;
    pos_T	old_visual;
    صحيح		old_visual_نما;
    colnr_T	old_virtcol;
    colnr_T	old_curswant;
    صحيح		old_مجموعه_curswant;
    pos_T	old_op_شروع, old_op_انتها;
    oparg_T	oa;
    cmdarg_T	ca;

    إذا (cbd->owned)
    {
	إذا ((cbd == &clip_plus
		&& get_y_ثبات(PLUS_ثبات)->y_array != عدم)
		|| (cbd == &clip_ستاره
		    && get_y_ثبات(ستاره_ثبات)->y_array != عدم))
	    أرجع;

	// Avoid triggering autocmds such as TextYankPost.
	بلوک_autocmds();

	// Get the text between clip_star.start & clip_star.end
	old_y_قبلی = get_y_قبلی();
	old_y_current = get_y_current();
	old_cursor = curwin->w_cursor;
	old_virtcol = curwin->w_virtcol;
	old_curswant = curwin->w_curswant;
	old_مجموعه_curswant = curwin->w_مجموعه_curswant;
	old_op_شروع = curbuf->b_op_شروع;
	old_op_انتها = curbuf->b_op_انتها;
	old_visual = VIsual;
	old_visual_نما = VIsual_نما;
	clear_oparg(&oa);
	oa.regname = (cbd == &clip_plus ? '+' : '*');
	oa.op_type = OP_YANK;
	CLEAR_فیلد(ca);
	ca.oap = &oa;
	ca.cmdchar = 'y';
	ca.count1 = 1;
	ca.retval = CA_NO_ADJ_OP_انتها;
	do_pending_operator(&ca, 0, TRUE);

	// restore things
	مجموعه_y_قبلی(old_y_قبلی);
	مجموعه_y_current(old_y_current);
	curwin->w_cursor = old_cursor;
	curwin->w_virtcol = old_virtcol;
	تغییریافته_cline_bef_curs();   // old w_virtcol et al. may be invalid
	curwin->w_curswant = old_curswant;
	curwin->w_مجموعه_curswant = old_مجموعه_curswant;
	curbuf->b_op_شروع = old_op_شروع;
	curbuf->b_op_انتها = old_op_انتها;
	VIsual = old_visual;
	VIsual_نما = old_visual_نما;

	unblock_autocmds();
    }
    وإلا إذا (!is_clipboard_needs_update())
    {
	clip_free_selection(cbd);

	// Try to get selected text from another window
	clip_gen_request_selection(cbd);
    }
}

/*
 * Convert from the GUI selection string into the '*'/'+' register.
 */
    فراغ
clip_yank_selection(
    صحيح		type,
    char_u	*str,
    دراز	len,
    Clipboard_T *cbd)
{
    yankreg_T *y_ptr;

    إذا (cbd == &clip_plus)
	y_ptr = get_y_ثبات(PLUS_ثبات);
    وإلا
	y_ptr = get_y_ثبات(ستاره_ثبات);

    clip_free_selection(cbd);

    str_مقصد_reg(y_ptr, type, str, len, -1, FALSE);
}

    صحيح
clip_convert_selection_جابه_جایی(
	char_u	    **str,
	long_u	    *len,
	صحيح	    جابه_جایی, // Extra space to add in *str and the offset to
			    // place the actual string in *str.
	Clipboard_T *cbd)
{
    char_u	*p;
    صحيح		lnum;
    صحيح		i, j;
    int_u	eolsize;
    yankreg_T	*y_ptr;

    إذا (cbd == &clip_plus)
	y_ptr = get_y_ثبات(PLUS_ثبات);
    وإلا
	y_ptr = get_y_ثبات(ستاره_ثبات);

# ifdef USE_CRNL
    eolsize = 2;
# وإلا
    eolsize = 1;
# endif

    *str = عدم;
    *len = 0;
    إذا (y_ptr->y_array == عدم)
	أرجع -1;

    لكل (i = 0; i < y_ptr->y_اندازه; i++)
	*len += (long_u)y_ptr->y_array[i].درازی + eolsize;

    // Don't want newline character at end of last line if we're in MCHAR mode.
    إذا (y_ptr->y_type == MCHAR && *len >= eolsize)
	*len -= eolsize;

    *len += جابه_جایی;
    p = *str = alloc(*len + 1);	// add one to avoid zero
    إذا (p == عدم)
	أرجع -1;
    p += جابه_جایی;
    lnum = 0;
    لكل (i = 0, j = 0; i < (صحيح)*len - جابه_جایی; i++, j++)
    {
	إذا (y_ptr->y_array[lnum].string[j] == '\n')
	    p[i] = NUL;
	وإلا إذا (y_ptr->y_array[lnum].string[j] == NUL)
	{
# ifdef USE_CRNL
	    p[i++] = '\r';
# endif
	    p[i] = '\n';
	    lnum++;
	    j = -1;
	}
	وإلا
	    p[i] = y_ptr->y_array[lnum].string[j];
    }
    أرجع y_ptr->y_type;
}

/*
 * Convert the '*'/'+' register into a GUI selection string returned in *str
 * with length *len.
 * Returns the motion type, or -1 for failure.
 */
    صحيح
clip_convert_selection(char_u **str, long_u *len, Clipboard_T *cbd)
{
    أرجع clip_convert_selection_جابه_جایی(str, len, 0, cbd);
}

/*
 * When "regname" is a clipboard register, obtain the selection.  If it's not
 * available return zero, otherwise return "regname".
 */
    صحيح
may_get_selection(صحيح regname)
{
    إذا (regname == '*')
    {
	إذا (!clip_ستاره.موجود)
	    regname = 0;
	وإلا
	    clip_get_selection(&clip_ستاره);
    }
    وإلا إذا (regname == '+')
    {
	إذا (!clip_plus.موجود)
	    regname = 0;
	وإلا
	    clip_get_selection(&clip_plus);
    }
    أرجع regname;
}

/*
 * If we have written to a clipboard register, send the text to the clipboard.
 */
    فراغ
may_مجموعه_selection(فراغ)
{
    إذا ((get_y_current() == get_y_ثبات(ستاره_ثبات))
	    && clip_ستاره.موجود)
    {
	clip_own_selection(&clip_ستاره);
	clip_gen_مجموعه_selection(&clip_ستاره);
    }
    وإلا إذا ((get_y_current() == get_y_ثبات(PLUS_ثبات))
	    && clip_plus.موجود)
    {
	clip_own_selection(&clip_plus);
	clip_gen_مجموعه_selection(&clip_plus);
    }
}

# إذا defined(FEAT_WAYLAND_CLIPBOARD)

    ساكن clip_wl_selection_T *
clip_wl_get_selection(wayland_selection_T sel)
{
    بدل (sel)
    {
	حالة WAYLAND_SELECTION_REGULAR:
	    أرجع &clip_wl.regular;
	حالة WAYLAND_SELECTION_PRIMARY:
	    أرجع &clip_wl.primary;
	افتراضي:
	    أرجع عدم;
    }
}

    ساكن clip_wl_selection_T *
clip_wl_get_selection_from_cbd(Clipboard_T *cbd)
{
    إذا (cbd == &clip_plus)
	أرجع &clip_wl.regular;
    وإلا إذا (cbd == &clip_ستاره)
	أرجع &clip_wl.primary;
    وإلا
	أرجع عدم;
}

    ساكن Clipboard_T *
clip_wl_get_cbd_from_selection(clip_wl_selection_T *sel)
{
    إذا (sel == &clip_wl.regular)
	أرجع &clip_plus;
    وإلا إذا (sel == &clip_wl.primary)
	أرجع &clip_ستاره;
    وإلا
	أرجع عدم;
}

    ساكن wayland_selection_T
clip_wl_get_selection_type(clip_wl_selection_T *sel)
{
    إذا (sel == &clip_wl.regular)
	أرجع WAYLAND_SELECTION_REGULAR;
    وإلا إذا (sel == &clip_wl.primary)
	أرجع WAYLAND_SELECTION_PRIMARY;
    وإلا
	أرجع WAYLAND_SELECTION_NONE;
}

    ساكن bool
wl_داده_offer_listener_event_offer(
    فراغ *داده استفادهنشده,
    vwl_داده_offer_T *offer استفادهنشده,
    ثابت حرف *mime_type
)
{
    // Only accept mime type if we support it
    لكل (صحيح i = 0; i < (صحيح)ARRAY_درازی(supported_mimes); i++)
	إذا (STRCMP(mime_type, supported_mimes[i]) == 0)
	    أرجع true;
    أرجع FALSE;
}

ساكن ثابت vwl_داده_offer_listener_T vwl_داده_offer_listener = {
    .offer = wl_داده_offer_listener_event_offer
};

    ساكن فراغ
vwl_داده_device_listener_event_داده_offer(
	فراغ *داده استفادهنشده,
	vwl_داده_device_T *device استفادهنشده,
	vwl_داده_offer_T *offer)
{
    // Immediately start listening for offer events from the data offer
    vwl_داده_offer_جمع_کن_listener(offer, &vwl_داده_offer_listener, عدم);
}

    ساكن فراغ
vwl_داده_device_listener_event_selection(
	فراغ *داده استفادهنشده,
	vwl_داده_device_T *device استفادهنشده,
	vwl_داده_offer_T *offer,
	wayland_selection_T selection)
{
    clip_wl_selection_T *sel = clip_wl_get_selection(selection);

    // Destroy previous offer if any, it is now invalid
    vwl_داده_offer_destroy(sel->offer);

    // There are two cases when sel->offer is NULL
    // 1. No one owns the selection
    // 2. We own the selection (we'll just access the register directly)
    إذا (offer == عدم || sel->منبع != عدم)
    {
	// Selection event is from us, so we are the source client. Therefore
	// ignore it. Or the selection is cleared, so set sel->offer to NULL
	vwl_داده_offer_destroy(offer);
	sel->offer = عدم;
	أرجع;
    }

    // Save offer. When we want to request data, then we'll actually call the
    // receive method.
    sel->offer = offer;

}

    ساكن فراغ
vwl_داده_device_listener_event_پایانیافته(
	فراغ *داده استفادهنشده,
	vwl_داده_device_T *device)
{
    clip_wl_selection_T *sel;
    // Device finished, guessing this can happen is when the seat becomes
    // invalid? If so, let the user call :wlrestore! to reset. There wouldn't be
    // any point in trying to create another data device for the same seat,
    // since the seat is in an invalid state.
    إذا (device == clip_wl.regular.device)
    {
	sel = &clip_wl.regular;
	clip_wl.regular.device = عدم;
    }
    وإلا إذا (device == clip_wl.primary.device)
    {
	sel = &clip_wl.primary;
	clip_wl.primary.device = عدم;
    }
    وإلا
	// Shouldn't happen
	أرجع;

    vim_free(sel->contents);
    vwl_داده_منبع_destroy(sel->منبع);
    vwl_داده_offer_destroy(sel->offer);
    sel->موجود = FALSE;

    vwl_داده_device_destroy(device);
}

ساكن ثابت vwl_داده_device_listener_T vwl_داده_device_listener = {
    .داده_offer = vwl_داده_device_listener_event_داده_offer,
    .selection = vwl_داده_device_listener_event_selection,
    .پایانیافته = vwl_داده_device_listener_event_پایانیافته
};

/*
 * Initialize the clipboard for Wayland using the global Wayland connection.
 * Returns OK on success and FAIL on failure.
 */
    صحيح
clip_init_wayland(فراغ)
{
    int_u supported = WAYLAND_SELECTION_NONE;

    إذا (wayland_ct == عدم)
	أرجع FAIL;

    memset(&clip_wl, 0, حجم(clip_wl));

    clip_wl.seat = vwl_connection_get_seat(wayland_ct, (حرف *)p_wse);

    إذا (clip_wl.seat == عدم)
	أرجع FAIL;

    clip_wl.regular.manager = vwl_connection_get_داده_device_manager(
	    wayland_ct, WAYLAND_SELECTION_REGULAR, &supported);

    إذا (clip_wl.regular.manager != عدم)
    {
	clip_wl.regular.device = vwl_داده_device_manager_get_داده_device(
		clip_wl.regular.manager, clip_wl.seat);

	إذا (clip_wl.regular.device != عدم)
	    clip_wl.regular.موجود = true;
	وإلا
	{
	    // Shouldn't happen
	    vwl_داده_device_manager_discard(clip_wl.regular.manager);
	    clip_wl.regular.manager = عدم;
	    أرجع FAIL;
	}
    }

    // If we still don't support the primary selection, try finding one for it
    // specifically.
    إذا (!(supported & WAYLAND_SELECTION_PRIMARY))
    {
	clip_wl.primary.manager = vwl_connection_get_داده_device_manager(
		wayland_ct, WAYLAND_SELECTION_PRIMARY, &supported);

	إذا (clip_wl.primary.manager != عدم)
	{
	    clip_wl.primary.device = vwl_داده_device_manager_get_داده_device(
		    clip_wl.primary.manager, clip_wl.seat);

	    إذا (clip_wl.primary.device != عدم)
		clip_wl.primary.موجود = true;
	    وإلا
	    {
		vwl_داده_device_manager_discard(clip_wl.primary.manager);
		clip_wl.primary.manager = عدم;
	    }
	}
    }

    إذا (clip_wl.regular.موجود && !clip_wl.primary.موجود)
    {
	// The protocol supports both regular and primary selections, just use
	// one data device manager and one data device. Or the primary selection
	// is not supported, make it point to the regular selection instead.
	clip_wl.primary.موجود = true;
	clip_wl.primary.manager = clip_wl.regular.manager;
	clip_wl.primary.device = clip_wl.regular.device;
    }

    إذا (!clip_wl.regular.موجود && !clip_wl.primary.موجود)
	أرجع FAIL;

    // Start listening for selection updates
    إذا (clip_wl.regular.device != عدم)
	vwl_داده_device_جمع_کن_listener(clip_wl.regular.device,
		&vwl_داده_device_listener, عدم);
    // Don't want to listen to the same data device twice
    إذا (clip_wl.primary.device != عدم
	    && clip_wl.primary.device != clip_wl.regular.device)
	vwl_داده_device_جمع_کن_listener(clip_wl.primary.device,
		&vwl_داده_device_listener, عدم);

    أرجع OK;
}

    فراغ
clip_uninit_wayland(فراغ)
{
    clip_wl_selection_T *sel;

    إذا (clipmethod == CLIPMETHOD_WAYLAND)
    {
	إذا (clip_ستاره.owned)
	    clip_lose_selection(&clip_ستاره);
	إذا (clip_plus.owned)
	    clip_lose_selection(&clip_plus);
    }

    // Don't want to double free
    إذا (clip_wl.regular.manager != clip_wl.primary.manager)
	vwl_داده_device_manager_discard(clip_wl.primary.manager);
    vwl_داده_device_manager_discard(clip_wl.regular.manager);

    إذا (clip_wl.regular.device != clip_wl.primary.device)
	vwl_داده_device_destroy(clip_wl.primary.device);
    vwl_داده_device_destroy(clip_wl.regular.device);

    sel = &clip_wl.regular;
    طالما (true)
    {
	vim_free(sel->contents);
	vwl_داده_منبع_destroy(sel->منبع);
	vwl_داده_offer_destroy(sel->offer);
	sel->موجود = false;

	إذا (sel == &clip_wl.primary)
	    اكسر;
	sel = &clip_wl.primary;
    }

    vim_memset(&clip_wl, 0, حجم(clip_wl));
}

    صحيح
clip_reset_wayland(فراغ)
{
    wayland_uninit_connection();

    إذا (wayland_init_connection(wayland_display_name) == FAIL
	    || clip_init_wayland() == FAIL)
	أرجع FAIL;

    انتخاب_clipmethod();
    أرجع OK;
}


/*
 * Read data from a file descriptor and write it to the given clipboard.
 */
    ساكن فراغ
clip_wl_receive_داده(Clipboard_T *cbd, ثابت حرف *mime_type, صحيح fd)
{
    char_u	*شروع, *final;
    دراز	len;
    garray_T	buf;
    صحيح		motion_type = MAUTO;
    ssize_t	r = 0;
    char_u	*tofree = عدم;
#  ifndef HAVE_SELECT
    بنية pollfd   pfd;

    pfd.fd = fd;
    pfd.events = POLLIN;
#  وإلا
    fd_set rfds;
    بنية timeval  tv;

    FD_ZERO(&rfds);
    FD_SET(fd, &rfds);
#  endif

    // Make pipe (read end) non-blocking
    إذا (fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK) == -1)
	أرجع;

    ga_init2(&buf, 1, 4096);

    // 4096 bytes seems reasonable for initial buffer size, memory is cheap
    // anyways.
    إذا (ga_grow(&buf, 4096) == FAIL)
	أرجع;

    شروع = buf.ga_داده;

#  ifndef HAVE_SELECT
    طالما (poll(&pfd, 1, p_wtm) > 0)
#  وإلا
    طالما (tv.tv_sec = p_wtm / 1000, tv.tv_usec = (p_wtm % 1000) * 1000,
	    select(fd + 1, &rfds, عدم, عدم, &tv) > 0)
#  endif
    {
	r = اقرأ(fd, شروع, buf.ga_maxlen - 1 - buf.ga_len);

	إذا (r == 0)
	    اكسر;
	وإلا إذا (r < 0)
	{
	    إذا (errno == EAGAIN || errno == EINTR)
		استمر;
	    اكسر;
	}

	شروع += r;
	buf.ga_len += r;

	// Realloc if we are at the end of the buffer
	إذا (buf.ga_len >= buf.ga_maxlen - 1)
	{
	    إذا (ga_grow(&buf, 8192) == FAIL)
		اكسر;
	    شروع = (char_u *)buf.ga_داده + buf.ga_len;
	}
    }

    إذا (buf.ga_len == 0)
    {
	clip_free_selection(cbd); // Nothing received, clear register
	ga_clear(&buf);
	أرجع;
    }

    final = buf.ga_داده;
    len = buf.ga_len;

    إذا (clip_convert_داده(&final, &len, &motion_type,
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
    ساكن فراغ
clip_wl_request_selection(Clipboard_T *cbd)
{
    clip_wl_selection_T *sel = clip_wl_get_selection_from_cbd(cbd);
    صحيح			fds[2];
    صحيح			mime_types_len;
    ثابت حرف		**mime_types;
    ثابت حرف		*chosen_mime = عدم;

    إذا (!sel->موجود)
	اذهب clear;

    // Dispatch any events that still queued up before checking for a data
    // offer.
    إذا (vwl_connection_roundtrip(wayland_ct) == FAIL)
	اذهب clear;

    إذا (sel->offer == عدم)
	اذهب clear;

    mime_types_len = sel->offer->mime_types.ga_len;
    mime_types = sel->offer->mime_types.ga_داده;

    // Choose mime type to receive from. Mime types with a lower index in the
    // "supported_mimes" array are prioritized over ones after it.
    لكل (صحيح i = 0; i < (صحيح)ARRAY_درازی(supported_mimes)
	    && chosen_mime == عدم; i++)
    {
	لكل (صحيح k = 0; k < mime_types_len && chosen_mime == عدم; k++)
	    إذا (STRCMP(mime_types[k], supported_mimes[i]) == 0)
		chosen_mime = supported_mimes[i];
    }

    إذا (chosen_mime == عدم || pipe(fds) == -1)
	اذهب clear;

    vwl_داده_offer_receive(sel->offer, chosen_mime, fds[1]);

    close(fds[1]); // Close before we read data so that when the source client
		   // closes their end we receive an EOF.

    إذا (vwl_connection_flush(wayland_ct) >= 0)
	clip_wl_receive_داده(cbd, chosen_mime, fds[0]);

    close(fds[0]);

    أرجع;
clear:
    clip_free_selection(cbd);
}

    ساكن فراغ
vwl_داده_منبع_listener_event_send(
    فراغ *داده,
    vwl_داده_منبع_T *منبع استفادهنشده,
    ثابت حرف *mime_type,
    int32_t fd
)
{
    clip_wl_selection_T *sel = داده;
    Clipboard_T		*cbd = clip_wl_get_cbd_from_selection(sel);
    bool		have_mime = false;
    صحيح			motion_type;
    long_u		درازی;
    char_u		*string; // Will be reallocated to a bigger size if
				 // needed.
    صحيح			جابه_جایی = 0;
    bool		is_vim, is_vimenc;
    size_t		مجموع = 0;
#  ifndef HAVE_SELECT
    بنية pollfd   pfd;

    pfd.fd = fd;
    pfd.events = POLLOUT;
#  وإلا
    fd_set	    wfds;
    بنية timeval  tv;

    FD_ZERO(&wfds);
    FD_SET(fd, &wfds);
#  endif

    // Check if we actually have mime type
    لكل (صحيح i = 0; i < (صحيح)ARRAY_درازی(supported_mimes); i++)
	إذا (STRCMP(supported_mimes[i], mime_type) == 0)
	{
	    have_mime = true;
	    اكسر;
	}

    إذا (!have_mime)
	اذهب خروج;

    // First byte sent is motion type for vim specific formats. For the vimenc
    // format, after the first byte is the encoding type, which is null
    // terminated.

    is_vimenc = STRCMP(mime_type, VIMENC_ATOM_NAME) == 0
	|| STRCMP(mime_type, VIMENC_MIMETYPE_NAME) == 0;
    is_vim = STRCMP(mime_type, VIM_ATOM_NAME) == 0
	|| STRCMP(mime_type, VIM_MIMETYPE_NAME) == 0;

    إذا (is_vimenc)
	جابه_جایی += 2 + STRLEN(p_enc);
    وإلا إذا (is_vim)
	جابه_جایی += 1;

    clip_get_selection(cbd);
    motion_type = clip_convert_selection_جابه_جایی(&string, &درازی, جابه_جایی, cbd);

    إذا (motion_type < 0)
	اذهب خروج;

    إذا (is_vimenc)
    {
	string[0] = (char_u)motion_type;
	// Use vim_strncpy for safer copying
	vim_strncpy(string + 1, p_enc, STRLEN(p_enc));
    }
    وإلا إذا (is_vim)
	string[0] = (char_u)motion_type;


    طالما (مجموع < (size_t)درازی &&
#  ifndef HAVE_SELECT
	    poll(&pfd, 1, p_wtm) > 0)
#  وإلا
	    ((tv.tv_sec = p_wtm / 1000, tv.tv_usec = (p_wtm % 1000) * 1000),
	    select(fd + 1, عدم, &wfds, عدم, &tv) > 0))
#  endif
    {
	ssize_t w = اكتب(fd, string + مجموع, درازی - مجموع);

	إذا (w == -1)
	    اكسر;
	مجموع += w;
    }

    vim_free(string);
خروج:
    close(fd);
}

    ساكن فراغ
vwl_داده_منبع_listener_event_cancelled(
	فراغ *داده,
	vwl_داده_منبع_T *منبع استفادهنشده)
{
    clip_wl_selection_T *sel = داده;
    Clipboard_T		*cbd = clip_wl_get_cbd_from_selection(sel);

    clip_lose_selection(cbd);
}

ساكن ثابت vwl_داده_منبع_listener_T vwl_داده_منبع_listener = {
    .send = vwl_داده_منبع_listener_event_send,
    .cancelled = vwl_داده_منبع_listener_event_cancelled
};

    ساكن فراغ
clip_wl_do_مجموعه_selection(فراغ *داده, uint32_t serial)
{
    clip_wl_selection_T *sel = داده;
    wayland_selection_T sel_type = clip_wl_get_selection_type(sel);

    vwl_داده_device_مجموعه_selection(sel->device, sel->منبع, serial, sel_type);

    sel->own_success = (vwl_connection_roundtrip(wayland_ct) == OK);
}

/*
 * Own the selection that cbd corresponds to. Start listening for requests from
 * other Wayland clients so they can receive data from us. Returns OK on success
 * and FAIL on failure.
 */
    ساكن صحيح
clip_wl_own_selection(Clipboard_T *cbd)
{
    clip_wl_selection_T *sel = clip_wl_get_selection_from_cbd(cbd);
    wayland_selection_T sel_type = clip_wl_get_selection_type(sel);

    إذا (!sel->موجود || vwl_connection_roundtrip(wayland_ct) == FAIL)
	أرجع FAIL;

    إذا (sel->منبع != عدم)
    {
	إذا (sel_type == WAYLAND_SELECTION_PRIMARY)
	    // We already own the selection, ignore (only do this for primary
	    // selection). We don't re set the selection because then we would
	    // be setting the selection every time the user moves the visual
	    // selection cursor, which is messy and inefficient. Some
	    // applications like Google Chrome do it this way however.
	    أرجع OK;
	وإلا إذا (sel_type == WAYLAND_SELECTION_REGULAR)
	{
	    // Technically we don't need to do this as we already own the
	    // selection, however if a user yanks text a second time, the
	    // text yanked won't appear in their clipboard manager if they are
	    // using one.
	    //
	    // This can be unexpected behaviour for the user so its probably
	    // better to do it this way. Additionally other Wayland applications
	    // seem to set the selection every time.
	    vwl_داده_منبع_destroy(sel->منبع);
	}
	وإلا
	    // Shouldn't happen
	    أرجع FAIL;
    }

    sel->منبع = vwl_داده_device_manager_create_داده_منبع(sel->manager);
    vwl_داده_منبع_جمع_کن_listener(sel->منبع, &vwl_داده_منبع_listener, sel);

    // Advertise mime types
    لكل (صحيح i = 0; i < (صحيح)ARRAY_درازی(supported_mimes); i++)
	vwl_داده_منبع_offer(sel->منبع, supported_mimes[i]);

    sel->own_success = false;
    clip_wl_do_مجموعه_selection(sel, 0);

    إذا (!sel->own_success)
	اذهب fail;

    أرجع OK;
fail:
    vwl_داده_منبع_destroy(sel->منبع);
    sel->منبع = عدم;
    أرجع FAIL;
}

/*
 * Disown the selection that cbd corresponds to.
 */
    ساكن فراغ
clip_wl_lose_selection(Clipboard_T *cbd)
{
    clip_wl_selection_T *sel = clip_wl_get_selection_from_cbd(cbd);

    إذا (!sel->موجود)
	أرجع;

    vwl_داده_منبع_destroy(sel->منبع);
    sel->منبع = عدم;
}

/*
 * Send the current selection to the clipboard. Do nothing for Wayland because
 * we will fill in the selection only when requested by another client.
 */
    ساكن فراغ
clip_wl_مجموعه_selection(Clipboard_T *cbd استفادهنشده)
{
}

#  إذا defined(USE_SYSTEM)
/*
 * Return true if we own the selection corresponding to cbd or another client
 * does.
 */
    ساكن bool
clip_wl_owner_exists(Clipboard_T *cbd)
{
    clip_wl_selection_T *sel = clip_wl_get_selection_from_cbd(cbd);

    إذا (vwl_connection_roundtrip(wayland_ct) == FAIL)
	أرجع false;

    أرجع sel->موجود && (sel->منبع != عدم || sel->offer != عدم);
}
#  endif

# endif // FEAT_WAYLAND_CLIPBOARD

#پایان_اگر // FEAT_CLIPBOARD

#اگر_تعریف_شده HAVE_CLIPMETHOD

/*
 * Returns the first method for accessing the clipboard that is available/works,
 * depending on the order of values in str.
 */
    ساكن clipmethod_T
get_clipmethod(char_u *str)
{
    صحيح		len	= (صحيح)STRLEN(str) + 1;
    char_u	*buf	= alloc(len);

    إذا (buf == عدم)
	أرجع CLIPMETHOD_FAIL;

    clipmethod_T ret = CLIPMETHOD_FAIL;
    char_u	*p = str;

    طالما (*p != NUL)
    {
	clipmethod_T روش = CLIPMETHOD_NONE;

	(فراغ)copy_option_part(&p, buf, len, ",");

	إذا (STRCMP(buf, "wayland") == 0)
	{
# ifdef FEAT_GUI
	    إذا (!gui.درون_use)
# endif
	    {
# ifdef FEAT_WAYLAND_CLIPBOARD
		إذا (clip_wl.regular.موجود || clip_wl.primary.موجود)
		    روش = CLIPMETHOD_WAYLAND;
# endif
	    }
	}
	وإلا إذا (STRCMP(buf, "x11") == 0)
	{
# ifdef FEAT_GUI
	    إذا (!gui.درون_use)
# endif
	    {
# ifdef FEAT_XCLIPBOARD
		// x_IOerror_handler() in os_unix.c should set xterm_dpy to NULL if
		// we lost connection to the X server.
		إذا (xterm_dpy != عدم)
		{
		    // If the X connection is lost then that handler will longjmp
		    // somewhere else, in that case we will call choose_clipmethod()
		    // again from there, and this if block won't be executed since
		    // xterm_dpy will be set to NULL.
		    xterm_update();
		    روش = CLIPMETHOD_X11;
		}
# endif
	    }
	}
	وإلا
	{
# ifdef FEAT_CLIPBOARD_PROVIDER
	    // Check if name matches a clipboard provider
	    صحيح r = clip_provider_is_موجود(buf);

	    إذا (r == 1)
	    {
		روش = CLIPMETHOD_PROVIDER;
		إذا (ret == CLIPMETHOD_FAIL)
		{
		    vim_free(clip_provider);
		    clip_provider = vim_strsave(buf);
		    إذا (clip_provider == عدم)
			اذهب fail;
		}
	    }
	    وإلا إذا (r == -1)
# endif
	    {
# ifdef FEAT_CLIPBOARD_PROVIDER
fail:
# endif
		ret = CLIPMETHOD_FAIL;
		اذهب خروج;
	    }
	}

	// Keep on going in order to catch errors
	إذا (روش != CLIPMETHOD_NONE && ret == CLIPMETHOD_FAIL)
	    ret = روش;
    }

    // No match found, use "none".
    ret = (ret == CLIPMETHOD_FAIL) ? CLIPMETHOD_NONE : ret;

خروج:
    vim_free(buf);
    أرجع ret;
}


/*
 * Returns name of clipmethod in a statically allocated string.
 */
    ساكن char_u *
clipmethod_مقصد_str(clipmethod_T روش)
{
    بدل(روش)
    {
	حالة CLIPMETHOD_WAYLAND:
	    أرجع (char_u *)"wayland";
	حالة CLIPMETHOD_X11:
	    أرجع (char_u *)"x11";
	حالة CLIPMETHOD_PROVIDER:
# ifdef FEAT_EVAL
	    أرجع clip_provider;
# endif
	افتراضي:
	    أرجع (char_u *)"none";
    }
}

/*
 * Sets the current clipmethod to use given by `get_clipmethod()`. Returns an
 * error message on failure else NULL.
 */
    حرف *
انتخاب_clipmethod(فراغ)
{
    clipmethod_T روش = get_clipmethod(p_cpm);

    إذا (روش == CLIPMETHOD_FAIL)
	أرجع e_invalid_آرگومان;

// If GUI is running or we are not on a system with Wayland or X11, then always
// return CLIPMETHOD_NONE. System or GUI clipboard handling always overrides.
// This is unless a provider is being used.
# إذا defined(FEAT_XCLIPBOARD) || defined(FEAT_WAYLAND_CLIPBOARD)
#  إذا defined(FEAT_GUI)
    إذا (روش != CLIPMETHOD_PROVIDER && gui.درون_use)
    {
#   ifdef FEAT_WAYLAND
	// We only interact with Wayland for the clipboard, we can just deinit
	// everything.
	wayland_uninit_connection();
#   endif

	روش = CLIPMETHOD_NONE;
	اذهب lose_sel_exit;
    }
#  endif
# وإلا
    // If on a system like windows or macos, then clipmethod is irrelevant, we
    // use their way of accessing the clipboard. This is unless we are using the
    // clipboard provider
#  ifdef FEAT_CLIPBOARD_PROVIDER
    إذا (روش != CLIPMETHOD_PROVIDER)
#  endif
    {
	روش = CLIPMETHOD_NONE;
	اذهب خروج;
    }
# endif

# ifdef FEAT_CLIPBOARD
    // Deinitialize clipboard if there is no way to access clipboard
    إذا (روش == CLIPMETHOD_NONE)
	clip_init(FALSE);
    // If we have a clipmethod that works now, then initialize clipboard
    وإلا إذا (clipmethod == CLIPMETHOD_NONE && روش != CLIPMETHOD_NONE)
    {
	clip_init(TRUE);
	did_warn_clipboard = false;
    }
    // Disown clipboard if we are switching to a new method
    وإلا إذا (clipmethod != CLIPMETHOD_NONE && روش != clipmethod)
    {
#  إذا (defined(FEAT_XCLIPBOARD) || defined(FEAT_WAYLAND_CLIPBOARD)) \
	&& defined(FEAT_GUI)
lose_sel_exit:
#  endif
	إذا (clip_ستاره.owned)
	    clip_lose_selection(&clip_ستاره);
	إذا (clip_plus.owned)
	    clip_lose_selection(&clip_plus);

#  إذا defined(FEAT_GUI)
	إذا (!gui.درون_use)
#  endif
	{
	    clip_init(TRUE);
	    did_warn_clipboard = false;
	}
    }
# endif // FEAT_CLIPBOARD

# إذا !defined(FEAT_XCLIPBOARD) && !defined(FEAT_WAYLAND_CLIPBOARD)
خروج:
# endif

    clipmethod = روش;

# ifdef FEAT_EVAL
    مجموعه_vim_var_string(VV_CLIPMETHOD, (char_u*)clipmethod_مقصد_str(روش), -1);
# endif

    أرجع عدم;
}

/*
 * Call choose_clipmethod().
 */
    فراغ
ex_clipreset(exarg_T *eap استفادهنشده)
{
    clipmethod_T prev = clipmethod;

    انتخاب_clipmethod();

    إذا (clipmethod == CLIPMETHOD_NONE)
	smsg(_("Could not find a way to access the clipboard."));
    وإلا إذا (clipmethod != prev)
	smsg(_("Switched to clipboard method '%s'."),
		clipmethod_مقصد_str(clipmethod));
}

/*
 * Adjust the register name pointed to with "rp" for the clipboard being
 * used always and the clipboard being available.
 */
    فراغ
adjust_clip_reg(صحيح *rp)
{
# ifdef FEAT_CLIPBOARD_PROVIDER
    إذا (clipmethod == CLIPMETHOD_PROVIDER)
    {
	إذا (*rp == 0 && clip_unnamed != 0)
	    *rp = ((clip_unnamed & CLIP_UNNAMED_PLUS)) ? '+' : '*';
	أرجع;
    }
# endif
# ifdef FEAT_CLIPBOARD
    // If no reg. specified, and "unnamed" or "unnamedplus" is in 'clipboard',
    // use '*' or '+' reg, respectively. "unnamedplus" prevails.
    إذا (*rp == 0 && (clip_unnamed != 0 || clip_unnamed_saved != 0))
    {
	إذا (clip_unnamed != 0)
	    *rp = ((clip_unnamed & CLIP_UNNAMED_PLUS) && clip_plus.موجود)
								  ? '+' : '*';
	وإلا
	    *rp = ((clip_unnamed_saved & CLIP_UNNAMED_PLUS)
					   && clip_plus.موجود) ? '+' : '*';
    }
    إذا ((!clip_ستاره.موجود && *rp == '*') ||
	   (!clip_plus.موجود && *rp == '+'))
    {
	پیام_warn_missing_clipboard();
	*rp = 0;
    }
# endif
}

/*
 * Extract the items in the 'clipboard' option and set global values.
 * Return an error message or NULL for success.
 */
    حرف *
did_مجموعه_clipboard(optset_T *آرگومانها استفادهنشده)
{
    صحيح		new_unnamed = 0;
# ifdef FEAT_CLIPBOARD
    صحيح		new_autoselect_ستاره = FALSE;
    صحيح		new_autoselect_plus = FALSE;
    صحيح		new_autoselectml = FALSE;
    صحيح		new_html = FALSE;
# endif
    regprog_T	*new_exclude_prog = عدم;
    حرف	*errmsg = عدم;
    char_u	*p;

    لكل (p = p_cb; *p != NUL; )
    {
	// Note: Keep this in sync with p_cb_values.
	إذا (STRNCMP(p, "unnamed", 7) == 0 && (p[7] == ',' || p[7] == NUL))
	{
	    new_unnamed |= CLIP_UNNAMED;
	    p += 7;
	}
	وإلا إذا (STRNCMP(p, "unnamedplus", 11) == 0
					    && (p[11] == ',' || p[11] == NUL))
	{
	    new_unnamed |= CLIP_UNNAMED_PLUS;
	    p += 11;
	}
# ifdef FEAT_CLIPBOARD
	وإلا إذا (STRNCMP(p, "autoselect", 10) == 0
					    && (p[10] == ',' || p[10] == NUL))
	{
	    new_autoselect_ستاره = TRUE;
	    p += 10;
	}
	وإلا إذا (STRNCMP(p, "autoselectplus", 14) == 0
					    && (p[14] == ',' || p[14] == NUL))
	{
	    new_autoselect_plus = TRUE;
	    p += 14;
	}
	وإلا إذا (STRNCMP(p, "autoselectml", 12) == 0
					    && (p[12] == ',' || p[12] == NUL))
	{
	    new_autoselectml = TRUE;
	    p += 12;
	}
	وإلا إذا (STRNCMP(p, "html", 4) == 0 && (p[4] == ',' || p[4] == NUL))
	{
	    new_html = TRUE;
	    p += 4;
	}
	وإلا إذا (STRNCMP(p, "exclude:", 8) == 0 && new_exclude_prog == عدم)
	{
	    p += 8;
	    new_exclude_prog = vim_regcomp(p, RE_MAGIC);
	    إذا (new_exclude_prog == عدم)
		errmsg = e_invalid_آرگومان;
	    اكسر;
	}
# endif
	وإلا
	{
	    errmsg = e_invalid_آرگومان;
	    اكسر;
	}
	إذا (*p == ',')
	    ++p;
    }
    إذا (errmsg == عدم)
    {
# ifdef FEAT_CLIPBOARD
	إذا (global_busy)
	    // clip_unnamed will be reset to clip_unnamed_saved
	    // at end_global_changes
	    clip_unnamed_saved = new_unnamed;
	وإلا
# endif
	    clip_unnamed = new_unnamed;
# ifdef FEAT_CLIPBOARD
	clip_autoselect_ستاره = new_autoselect_ستاره;
	clip_autoselect_plus = new_autoselect_plus;
	clip_autoselectml = new_autoselectml;
	clip_html = new_html;
	vim_regfree(clip_exclude_prog);
	clip_exclude_prog = new_exclude_prog;
# endif
# إذا defined(FEAT_GUI_GTK)
	إذا (gui.درون_use)
	{
#  ifdef USE_GTK4
	    gui_gtk_update_selection_formats(&clip_plus);
	    gui_gtk_update_selection_formats(&clip_ستاره);
#  وإلا
	    gui_gtk_مجموعه_selection_targets((GdkAtom)GDK_SELECTION_PRIMARY);
	    gui_gtk_مجموعه_selection_targets((GdkAtom)clip_plus.gtk_sel_atom);
#  endif
#  ifdef FEAT_DND
	    gui_gtk_مجموعه_dnd_targets();
#  endif
	}
# endif
    }
    وإلا
	vim_regfree(new_exclude_prog);

    أرجع errmsg;
}

#پایان_اگر // HAVE_CLIPMETHOD

#اگر_تعریف_شده FEAT_CLIPBOARD_PROVIDER

/*
 * Check if a clipboard provider with given name is available. Returns 1 if available,
 * 0 if not available, and -1 on error
 */
    ساكن صحيح
clip_provider_is_موجود(char_u *provider)
{
    dict_T	*providers = get_vim_var_dict(VV_CLIPPROVIDERS);
    typval_T	provider_tv = {0};
    callback_T	callback = {0};
    typval_T	rettv = {0};
    typval_T	func_tv = {0};
    صحيح		res = 0;

    إذا (dict_get_tv(providers, (حرف *)provider, &provider_tv) == FAIL
	    || provider_tv.v_type != VAR_DICT)
	// clipboard provider not defined
	أرجع -1;

    إذا (dict_get_tv(provider_tv.vval.v_dict, "available", &func_tv) == FAIL)
    {
	clear_tv(&provider_tv);
	// If "available" function not specified assume always TRUE
	أرجع 1;
    }

    إذا ((callback = get_callback(&func_tv)).cb_name == عدم)
	اذهب fail;

    إذا (call_callback(&callback, -1, &rettv, 0, عدم) == FAIL ||
	    (rettv.v_type != VAR_BOOL && rettv.v_type != VAR_عدد))
	اذهب fail;

    إذا (rettv.vval.v_عدد)
	res = 1;

    إذا (FALSE)
fail:
	res = -1;

    free_callback(&callback);
    clear_tv(&func_tv);
    clear_tv(&rettv);
    clear_tv(&provider_tv);

    أرجع res;
}

/*
 * Get the specified callback "function" from the provider dictionary for
 * register "reg".
 */
    ساكن صحيح
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

    إذا (dict_get_tv(providers, (حرف *)provider, &provider_tv) == FAIL)
	أرجع FAIL;
    وإلا إذا (provider_tv.v_type != VAR_DICT)
    {
	clear_tv(&provider_tv);
	أرجع FAIL;
    }
    وإلا إذا (dict_get_tv(
		provider_tv.vval.v_dict,
		(حرف *)function,
		&action_tv) == FAIL)
    {
	clear_tv(&provider_tv);
	أرجع FAIL;
    }
    وإلا إذا (action_tv.v_type != VAR_DICT)
    {
	clear_tv(&provider_tv);
	clear_tv(&action_tv);
	أرجع FAIL;
    }
    وإلا إذا (dict_get_tv(action_tv.vval.v_dict, (حرف *)reg, &func_tv) == FAIL)
    {
	clear_tv(&provider_tv);
	clear_tv(&action_tv);
	أرجع FAIL;
    }
    وإلا إذا ((cb = get_callback(&func_tv)).cb_name == عدم)
    {
	clear_tv(&provider_tv);
	clear_tv(&action_tv);
	clear_tv(&func_tv);
	أرجع FAIL;
    }
    clear_tv(&provider_tv);
    clear_tv(&action_tv);

    // func_tv owns the function name, so we must make a copy for the callback
    مجموعه_callback(callback, &cb);
    clear_tv(&func_tv);
    أرجع OK;
}

    ساكن فراغ
clip_provider_copy(char_u *reg, char_u *provider)
{
    ساكن bool	recursive = false;
    callback_T	callback;
    typval_T	rettv;
    typval_T	argvars[4];
    yankreg_T	*y_ptr;
    char_u	type[2 + NUMBUFLEN] = {0};
    list_T	*list = عدم;

    إذا (recursive)
	أرجع;

    إذا (clip_provider_get_callback(
		reg,
		provider,
		(char_u *)"copy",
		&callback) == FAIL)
	أرجع;

    // Convert register type into a string
    إذا (*reg == '+')
	y_ptr = get_y_ثبات(REAL_PLUS_ثبات);
    وإلا
	y_ptr = get_y_ثبات(ستاره_ثبات);

    بدل (y_ptr->y_type)
    {
	حالة MCHAR:
	    type[0] = 'v';
	    اكسر;
	حالة MLINE:
	    type[0] = 'V';
	    اكسر;
	حالة MBLOCK:
	    sprintf((حرف *)type, "%c%d", Ctrl_V, y_ptr->y_عرض + 1);
	    اكسر;
	افتراضي:
	    type[0] = 0;
	    اكسر;
    }

    argvars[0].v_type = VAR_STRING;
    argvars[0].vval.v_string = reg;

    argvars[1].v_type = VAR_STRING;
    argvars[1].vval.v_string = type;

    // Get register contents by creating a list of lines
    list = list_alloc();

    إذا (list == عدم)
    {
	free_callback(&callback);
	أرجع;
    }

    لكل (صحيح i = 0; i < y_ptr->y_اندازه; i++)
	إذا (list_append_string(list, y_ptr->y_array[i].string,
	    (صحيح)y_ptr->y_array[i].درازی) == FAIL)
	{
	    free_callback(&callback);
	    list_unref(list);
	    أرجع;
	}

    list->lv_refcount++;

    argvars[2].v_type = VAR_LIST;
    argvars[2].v_قفل = VAR_FIXED;
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

    ساكن فراغ
clip_provider_paste(char_u *reg, char_u *provider)
{
    ساكن bool	recursive = false;
    callback_T	callback;
    typval_T	argvars[2];
    typval_T	rettv;
    صحيح		ret;
    char_u	*reg_type;
    list_T	*سطرها;

    إذا (recursive)
	أرجع;

    إذا (clip_provider_get_callback(
		reg,
		provider,
		(char_u *)"paste",
		&callback) == FAIL)
	أرجع;

    argvars[0].v_type = VAR_STRING;
    argvars[0].vval.v_string = reg;

    argvars[1].v_type = VAR_UNKNOWN;

    textlock++;
    recursive = true;
    ret = call_callback(&callback, -1, &rettv, 1, argvars);
    recursive = false;
    textlock--;

    إذا (ret == FAIL)
	اذهب خروج;
    وإلا إذا (rettv.v_type == VAR_TUPLE
	    && TUPLE_LEN(rettv.vval.v_tuple) == 2
	    && TUPLE_قلم(rettv.vval.v_tuple, 0)->v_type == VAR_STRING
	    && TUPLE_قلم(rettv.vval.v_tuple, 1)->v_type == VAR_LIST)
    {
	reg_type = TUPLE_قلم(rettv.vval.v_tuple, 0)->vval.v_string;
	سطرها = TUPLE_قلم(rettv.vval.v_tuple, 1)->vval.v_list;
    }
    وإلا إذا (rettv.v_type == VAR_LIST
	    && rettv.vval.v_list->lv_len == 2
	    && rettv.vval.v_list->lv_اولی->li_tv.v_type == VAR_STRING
	    && rettv.vval.v_list->lv_اولی->li_بعدی->li_tv.v_type == VAR_LIST)
    {
	reg_type = rettv.vval.v_list->lv_اولی->li_tv.vval.v_string;
	سطرها = rettv.vval.v_list->lv_اولی->li_بعدی->li_tv.vval.v_list;
    }
    وإلا
	اذهب خروج;

    {
	char_u		yank_type = MAUTO;
	دراز		بلوک_len = -1;
	yankreg_T	*y_ptr, *cur_y_ptr;
	char_u		**lstval;
	char_u		**allocval;
	char_u		buf[NUMBUFLEN];
	char_u		**curval;
	char_u		**curallocval;
	char_u		*strval;
	listitem_T	*li;
	صحيح		len;

	// If the list is NULL handle like an empty list.
	len = سطرها == عدم ? 0 : سطرها->lv_len;

	// First half: use for pointers to result lines; second half: use for
	// pointers to allocated copies.
	lstval = ALLOC_MULT(char_u *, (len + 1) * 2);
	إذا (lstval == عدم)
	    اذهب خروج;
	curval = lstval;
	allocval = lstval + len + 2;
	curallocval = allocval;

	إذا (سطرها != عدم)
	{
	    CHECK_LIST_MATERIALIZE(سطرها);
	    FOR_ALL_LIST_اقلام(سطرها, li)
	    {
		strval = tv_get_string_buf_chk(&li->li_tv, buf);
		إذا (strval == عدم)
		    اذهب free_lstval;
		إذا (strval == buf)
		{
		    // Need to make a copy, next tv_get_string_buf_chk() will
		    // overwrite the string.
		    strval = vim_strsave(buf);
		    إذا (strval == عدم)
			اذهب free_lstval;
		    *curallocval++ = strval;
		}
		*curval++ = strval;
	    }
	}
	*curval++ = عدم;

	إذا (*reg_type != NUL && (STRLEN(reg_type) == 0
		|| get_yank_type(&reg_type, &yank_type, &بلوک_len) == FAIL))
	{
	    emsg(e_invalid_آرگومان);
	    اذهب free_lstval;
	}

	إذا (*reg == '+')
	    y_ptr = get_y_ثبات(REAL_PLUS_ثبات);
	وإلا
	    y_ptr = get_y_ثبات(ستاره_ثبات);

	// Free previous register contents
	cur_y_ptr = get_y_current();
	مجموعه_y_current(y_ptr);

	free_yank_all();
	get_y_current()->y_اندازه = 0;

	مجموعه_y_current(cur_y_ptr);

	str_مقصد_reg(y_ptr,
		yank_type,
		(char_u *)lstval,
		-1,
		بلوک_len,
		TRUE);

free_lstval:
	طالما (curallocval > allocval)
	    vim_free(*--curallocval);
	vim_free(lstval);
    }

خروج:
    free_callback(&callback);
    clear_tv(&rettv);
}

// Used to stop calling the provider callback every time there is an update.
// This prevents unnecessary calls when accessing the provider often in an
// interval.
//
// If -1 then allow provider callback to be called then set to one. Default
// value (is allowed) is -2.
ساكن صحيح ستاره_pause_شمار = -2, plus_pause_شمار = -2;

    فراغ
call_clip_provider_request(صحيح reg)
{
    إذا (clipmethod != CLIPMETHOD_PROVIDER)
	أرجع;

    إذا (reg == '+' && plus_pause_شمار < 0)
    {
	إذا (plus_pause_شمار == -1)
	    plus_pause_شمار = 1;
	clip_provider_paste((char_u *)"+", clip_provider);
    }
    وإلا إذا (reg == '*' && ستاره_pause_شمار < 0)
    {
	إذا (ستاره_pause_شمار == -1)
	    ستاره_pause_شمار = 1;
	clip_provider_paste((char_u *)"*", clip_provider);
    }
    وإلا
	أرجع;
}

    فراغ
call_clip_provider_مجموعه(صحيح reg)
{
    إذا (clipmethod != CLIPMETHOD_PROVIDER)
	أرجع;

    إذا (reg == '+' && plus_pause_شمار < 0)
    {
	إذا (plus_pause_شمار == -1)
	    plus_pause_شمار = 1;
	clip_provider_copy((char_u *)"+", clip_provider);
    }
    وإلا إذا (reg == '*' && ستاره_pause_شمار < 0)
    {
	إذا (ستاره_pause_شمار == -1)
	    ستاره_pause_شمار = 1;
	clip_provider_copy((char_u *)"*", clip_provider);
    }
}

/*
 * Makes it so that the next provider call is only done once any calls after are
 * ignored, until dec_clip_provider is called the same number of times after
 * again. Note that this is per clipboard register ("+", "*")
 */
    فراغ
inc_clip_provider(فراغ)
{
    plus_pause_شمار = (plus_pause_شمار == -2
	|| plus_pause_شمار == -1) ? -1 : plus_pause_شمار + 1;
    ستاره_pause_شمار = (ستاره_pause_شمار == -2
	|| ستاره_pause_شمار == -1) ? -1 : ستاره_pause_شمار + 1;
}

    فراغ
dec_clip_provider(فراغ)
{
    إذا (plus_pause_شمار != -2)
	plus_pause_شمار = plus_pause_شمار == -1 ? -1 : plus_pause_شمار - 1;
    إذا (ستاره_pause_شمار != -2)
	ستاره_pause_شمار = ستاره_pause_شمار == -1 ? -1 : ستاره_pause_شمار - 1;

    إذا (plus_pause_شمار == 0 || plus_pause_شمار == -1)
	plus_pause_شمار = -2;
    إذا (ستاره_pause_شمار == 0 || ستاره_pause_شمار == -1)
	ستاره_pause_شمار = -2;
}

#پایان_اگر // FEAT_CLIPBOARD_PROVIDER

#إذا defined(FEAT_WAYLAND_CLIPBOARD) || (defined(FEAT_GUI_GTK) && defined(USE_GTK4))
/*
 * If "vim" is TRUE, then get the motion type. If "vimenc" is TRUE, then get the
 * motion type and also convert "*buf". "buf" and "len_store" will be updated to
 * reflect the actual contents, but should be set beforehand with the initial
 * contents. Note that if "*tofree" is not NULL, use vim_free() on "*buf",
 * otherwise use the free func for which "*buf" was allocated with. In both
 * cases use vim_free() on "*tofree". Returns OK on success and FAIL on failure.
 */
    صحيح
clip_convert_داده(
	char_u	**buf,
	دراز	*len_store,
	صحيح	*motion,
	bool	vim,
	bool	vimenc,
	char_u	**tofree)
{
    char_u  *final = *buf;
    char_u  *enc;
    دراز    len = *len_store;

    إذا (vim && len >= 2)
    {
	*motion = *final++;
	len--;
    }
    وإلا إذا (vimenc && len >= 3)
    {
	vimconv_T   conv;
	صحيح	    convlen;

	// First byte is motion type
	*motion = *final++;
	len--;

	// Get encoding of selection
	enc = final;

	// Skip the encoding type including null terminator in final text
	final = memchr(final, NUL, len);
	إذا (final == عدم)
	    أرجع FAIL;
	final++; // Skip NUL

	// Subtract pointers to get length of encoding;
	len -= final - enc;

	conv.vc_type = CONV_NONE;
	convert_راهاندازی(&conv, enc, p_enc);
	إذا (conv.vc_type != CONV_NONE)
	{
	   char_u *tmp;

	   convlen = len;
	   tmp = string_convert(&conv, final, &convlen);
	   len = convlen;
	   إذا (tmp != عدم)
	   {
		final = tmp;
		*tofree = final;
	   }
	   convert_راهاندازی(&conv, عدم, عدم);
	}
    }
    *buf = final;
    *len_store = len;
    أرجع OK;
}
#پایان_اگر
