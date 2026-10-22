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

#شامل "vim.h"

#اگر_تعریف_شدہ FEAT_CYGWIN_WIN32_CLIPBOARD
# define WIN32_LEAN_AND_اوسط
# include <windows.h>
# include "winclip.pro"
#اگر_ختم

// Functions for copying and pasting text between applications.
// This is always included in a GUI version, but may also be included when the
// clipboard and mouse is available to a terminal version such as xterm.
// Note: there are some more functions in ops.c that handle selection stuff.
//
// Also note that the majority of functions here deal with the X 'primary'
// (visible - for Visual mode use) selection, and only that. There are no
// versions of these for the 'clipboard' selection, as Visual mode has no use
// for them.


#اگر_تعریف_شدہ FEAT_CLIPBOARD_PROVIDER
ساکن صحیح clip_provider_is_دستیاب(char_u *provider);
#اگر_ختم

#اگر defined(FEAT_CLIPBOARD)

# اگر defined(FEAT_WAYLAND_CLIPBOARD)

#  include "wayland.h"

// Represents either the regular or primary selection
قسمتعریف ساخت {
    char_u		*contents;	// Non-null if we own selection,
					// contains the data to send to other
					// clients.
    vwl_ڈیٹا_ماخذ_T	*ماخذ;	// Non-NULL if we own the selection,
					// else NULL if we don't.
    vwl_ڈیٹا_offer_T	*offer;		// Current offer for the selection

    bool		own_success;	// Used by clip_wl_own_selection()
    bool		دستیاب;	// If selection is ready to serve/use

    // These may point to the same proxy as the other selection
    vwl_ڈیٹا_device_manager_T	*manager;
    vwl_ڈیٹا_device_T		*device;
} clip_wl_selection_T;

// Represents the clipboard for the global Wayland connection, for the chosen
// seat (using the 'wl_seat' option)
قسمتعریف ساخت {
    vwl_seat_T *seat;

    clip_wl_selection_T regular;
    clip_wl_selection_T primary;
} clip_wl_T;

// Mime types we support sending and receiving
// Mimes with a lower index in the array are prioritized first when we are
// receiving data.
ساکن ثابت حرف *supported_mimes[] = {
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

ساکن خالی
clip_wl_receive_ڈیٹا(Clipboard_T *cbd, ثابت حرف *mime_type, صحیح fd);
ساکن خالی clip_wl_request_selection(Clipboard_T *cbd);
ساکن صحیح clip_wl_own_selection(Clipboard_T *cbd);
ساکن خالی clip_wl_lose_selection(Clipboard_T *cbd);
ساکن خالی clip_wl_سیٹ_selection(Clipboard_T *cbd);

#  اگر defined(USE_SYSTEM)
ساکن bool clip_wl_owner_exists(Clipboard_T *cbd);
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
    خالی
clip_init(صحیح can_use)
{
    Clipboard_T *cb;

    cb = &clip_ستارہ;
    برائے (;;)
    {
	// No need to init again if cbd is already available
	اگر (can_use && cb->دستیاب)
	    جاؤ skip;

	cb->دستیاب  = can_use;
	cb->owned      = FALSE;
	cb->آغاز.lnum = 0;
	cb->آغاز.ستون  = 0;
	cb->انتہا.lnum   = 0;
	cb->انتہا.ستون    = 0;
	cb->state      = SELECT_CLEARED;

skip:
	اگر (cb == &clip_plus)
	    توڑو;
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
    خالی
clip_update_selection(Clipboard_T *clip)
{
    pos_T	    آغاز, انتہا;

    // If visual mode is only due to a redo command ("."), then ignore it
    اگر (!redo_VIsual_busy && VIsual_فعال && (State & منوال_معمول))
    {
	اگر (LT_POS(VIsual, curwin->w_cursor))
	{
	    آغاز = VIsual;
	    انتہا = curwin->w_cursor;
	    اگر (has_mbyte)
		انتہا.ستون += (*mb_ptr2len)(ml_get_cursor()) - 1;
	}
	ورنہ
	{
	    آغاز = curwin->w_cursor;
	    انتہا = VIsual;
	}
	اگر (!EQUAL_POS(clip->آغاز, آغاز)
		|| !EQUAL_POS(clip->انتہا, انتہا)
		|| clip->vmode != VIsual_منوال)
	{
	    clip_clear_selection(clip);
	    clip->آغاز = آغاز;
	    clip->انتہا = انتہا;
	    clip->vmode = VIsual_منوال;
	    clip_free_selection(clip);
	    clip_own_selection(clip);
	    clip_gen_سیٹ_selection(clip);
	}
    }
}

    ساکن صحیح
clip_gen_own_selection(Clipboard_T *cbd)
{
# اگر defined(FEAT_XCLIPBOARD) || defined(FEAT_WAYLAND_CLIPBOARD)
#  ifdef FEAT_GUI
    اگر (gui.اندر_use)
	واپس clip_mch_own_selection(cbd);
    ورنہ
#  endif
    {
	اگر (clipmethod == CLIPMETHOD_WAYLAND)
	{
#  ifdef FEAT_WAYLAND_CLIPBOARD
	    واپس clip_wl_own_selection(cbd);
#  endif
	}
	ورنہ اگر (clipmethod == CLIPMETHOD_X11)
	{
#  ifdef FEAT_XCLIPBOARD
	    واپس clip_xterm_own_selection(cbd);
#  endif
	}
    }
    واپس FAIL;
# ورنہ
    واپس clip_mch_own_selection(cbd);
# endif
}

    خالی
clip_own_selection(Clipboard_T *cbd)
{
    /*
     * Also want to check somehow that we are reading from the keyboard rather
     * than a mapping etc.
     */
# اگر defined(FEAT_X11) || defined(FEAT_WAYLAND_CLIPBOARD)
    // Always own the selection, we might have lost it without being
    // notified, e.g. during a ":sh" command.
    اگر (cbd->دستیاب)
    {
	صحیح was_owned = cbd->owned;

	cbd->owned = (clip_gen_own_selection(cbd) == OK);
	اگر (!was_owned && (cbd == &clip_ستارہ || cbd == &clip_plus))
	{
	    // May have to show a different kind of highlighting for the
	    // selected area.  There is no specific redraw command for this,
	    // just redraw all windows on the current buffer.
	    اگر (cbd->owned
		    && (get_real_state() == منوال_VISUAL
					    || get_real_state() == منوال_SELECT)
		    && (cbd == &clip_ستارہ ? clip_isautosel_ستارہ()
						      : clip_isautosel_plus())
		    && HL_ATTR(HLF_V) != HL_ATTR(HLF_VNC))
		redraw_curbuf_later(UPD_INVERTED_ALL);
	}
    }
# ورنہ
    // Only own the clipboard when we didn't own it yet.
    اگر (!cbd->owned && cbd->دستیاب)
	cbd->owned = (clip_gen_own_selection(cbd) == OK);
# endif
}

    ساکن خالی
clip_gen_lose_selection(Clipboard_T *cbd)
{
# اگر defined(FEAT_XCLIPBOARD) || defined(FEAT_WAYLAND_CLIPBOARD)
#  ifdef FEAT_GUI
    اگر (gui.اندر_use)
	clip_mch_lose_selection(cbd);
    ورنہ
#  endif
    {
	اگر (clipmethod == CLIPMETHOD_WAYLAND)
	{
#  ifdef FEAT_WAYLAND_CLIPBOARD
	    clip_wl_lose_selection(cbd);
#  endif
	}
	ورنہ اگر (clipmethod == CLIPMETHOD_X11)
	{
#  ifdef FEAT_XCLIPBOARD
	    clip_xterm_lose_selection(cbd);
#  endif
	}
    }
# ورنہ
    clip_mch_lose_selection(cbd);
# endif
}

    خالی
clip_lose_selection(Clipboard_T *cbd)
{
# ifdef FEAT_X11
    صحیح	    was_owned = cbd->owned;
# endif
    صحیح     visual_selection = FALSE;

    اگر (cbd == &clip_ستارہ || cbd == &clip_plus)
	visual_selection = TRUE;

    clip_free_selection(cbd);
    cbd->owned = FALSE;
    اگر (visual_selection)
	clip_clear_selection(cbd);
    clip_gen_lose_selection(cbd);
# ifdef FEAT_X11
    اگر (visual_selection)
    {
	// May have to show a different kind of highlighting for the selected
	// area.  There is no specific redraw command for this, just redraw all
	// windows on the current buffer.
	اگر (was_owned
		&& (get_real_state() == منوال_VISUAL
		    || get_real_state() == منوال_SELECT)
		&& (cbd == &clip_ستارہ ?
		    clip_isautosel_ستارہ() : clip_isautosel_plus())
		&& HL_ATTR(HLF_V) != HL_ATTR(HLF_VNC)
		&& !exiting)
	{
	    update_curbuf(UPD_INVERTED_ALL);
	    setcursor();
	    cursor_on();
	    برآمد_flush_cursor(TRUE, FALSE);
	}
    }
# endif
}

    ساکن خالی
clip_copy_selection(Clipboard_T *clip)
{
    اگر (VIsual_فعال && (State & منوال_معمول) && clip->دستیاب)
    {
	clip_update_selection(clip);
	clip_free_selection(clip);
	clip_own_selection(clip);
	اگر (clip->owned)
	    clip_get_selection(clip);
	clip_gen_سیٹ_selection(clip);
    }
}

/*
 * Save and restore clip_unnamed before doing possibly many changes. This
 * prevents accessing the clipboard very often which might slow down Vim
 * considerably.
 */
ساکن صحیح global_change_گنتی = 0; // if set, inside a start_global_changes
ساکن صحیح clipboard_needs_update = FALSE; // clipboard needs to be updated
ساکن صحیح clip_did_سیٹ_selection = TRUE;

/*
 * Save clip_unnamed and reset it.
 */
    خالی
آغاز_global_changes(خالی)
{
    اگر (++global_change_گنتی > 1)
	واپس;
    clip_unnamed_saved = clip_unnamed;
    clipboard_needs_update = FALSE;

    اگر (clip_did_سیٹ_selection)
    {
	clip_unnamed = 0;
	clip_did_سیٹ_selection = FALSE;
    }
}

/*
 * Return TRUE if setting the clipboard was postponed, it already contains the
 * right text.
 */
    ساکن صحیح
is_clipboard_needs_update(خالی)
{
    واپس clipboard_needs_update;
}

/*
 * Restore clip_unnamed and set the selection when needed.
 */
    خالی
انتہا_global_changes(خالی)
{
    اگر (--global_change_گنتی > 0)
	// recursive
	واپس;
    اگر (!clip_did_سیٹ_selection)
    {
	clip_did_سیٹ_selection = TRUE;
	clip_unnamed = clip_unnamed_saved;
	clip_unnamed_saved = 0;
	اگر (clipboard_needs_update)
	{
	    // only store something in the clipboard,
	    // if we have yanked anything to it
	    اگر (clip_unnamed & CLIP_UNNAMED)
	    {
		clip_own_selection(&clip_ستارہ);
		clip_gen_سیٹ_selection(&clip_ستارہ);
	    }
	    اگر (clip_unnamed & CLIP_UNNAMED_PLUS)
	    {
		clip_own_selection(&clip_plus);
		clip_gen_سیٹ_selection(&clip_plus);
	    }
	}
    }
    clipboard_needs_update = FALSE;
}

/*
 * Called when Visual mode is ended: update the selection.
 */
    خالی
clip_auto_select(خالی)
{
    اگر (clip_isautosel_ستارہ())
	clip_copy_selection(&clip_ستارہ);
    اگر (clip_isautosel_plus())
	clip_copy_selection(&clip_plus);
}

/*
 * Return TRUE if automatic selection of Visual area is desired for the *
 * register.
 */
    صحیح
clip_isautosel_ستارہ(خالی)
{
# ifdef FEAT_CLIPBOARD_PROVIDER
    اگر (clipmethod == CLIPMETHOD_PROVIDER)
	واپس false;
# endif
# ifdef FEAT_GUI
    اگر (gui.اندر_use)
	واپس vim_strchr(p_go, GO_ASEL) != تہی
	    && vim_strchr(p_go, GO_ASELPLUS) == تہی;
# endif
    واپس clip_autoselect_ستارہ;
}

/*
 * Return TRUE if automatic selection of Visual area is desired for the +
 * register.
 */
    صحیح
clip_isautosel_plus(خالی)
{
# ifdef FEAT_CLIPBOARD_PROVIDER
    اگر (clipmethod == CLIPMETHOD_PROVIDER)
	واپس false;
# endif
# ifdef FEAT_GUI
    اگر (gui.اندر_use)
	واپس vim_strchr(p_go, GO_ASELPLUS) != تہی;
# endif
    واپس clip_autoselect_plus;
}


/*
 * Stuff for general mouse selection, without using Visual mode.
 */

/*
 * Compare two screen positions ala strcmp()
 */
    ساکن صحیح
clip_compare_pos(
    صحیح		row1,
    صحیح		col1,
    صحیح		row2,
    صحیح		col2)
{
    اگر (row1 > row2) واپس(1);
    اگر (row1 < row2) واپس(-1);
    اگر (col1 > col2) واپس(1);
    اگر (col1 < col2) واپس(-1);
    واپس(0);
}

// "how" flags for clip_invert_area()
# define CLIP_CLEAR	1
# define CLIP_سیٹ	2
# define CLIP_TOGGLE	3

/*
 * Invert or un-invert a rectangle of the screen.
 * "invert" is true if the result is inverted.
 */
    ساکن خالی
clip_invert_rectangle(
	Clipboard_T	*cbd غیر_مستعمل,
	صحیح		صف_arg,
	صحیح		ستون_arg,
	صحیح		اونچائی_arg,
	صحیح		چوڑائی_arg,
	صحیح		invert)
{
    صحیح		صف = صف_arg;
    صحیح		ستون = ستون_arg;
    صحیح		اونچائی = اونچائی_arg;
    صحیح		چوڑائی = چوڑائی_arg;

# ifdef FEAT_PROP_POPUP
    // this goes on top of all popup windows
    screen_zindex = CLIP_ZINDEX;

    اگر (ستون < cbd->min_ستون)
    {
	چوڑائی -= cbd->min_ستون - ستون;
	ستون = cbd->min_ستون;
    }
    اگر (چوڑائی > cbd->max_ستون - ستون)
	چوڑائی = cbd->max_ستون - ستون;
    اگر (صف < cbd->min_صف)
    {
	اونچائی -= cbd->min_صف - صف;
	صف = cbd->min_صف;
    }
    اگر (اونچائی > cbd->max_صف - صف + 1)
	اونچائی = cbd->max_صف - صف + 1;
# endif
# ifdef FEAT_GUI
    اگر (gui.اندر_use)
	gui_mch_invert_rectangle(صف, ستون, اونچائی, چوڑائی);
    ورنہ
# endif
	screen_draw_rectangle(صف, ستون, اونچائی, چوڑائی, invert, FALSE);
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
    ساکن خالی
clip_invert_area(
	Clipboard_T	*cbd,
	صحیح		row1,
	صحیح		col1,
	صحیح		row2,
	صحیح		col2,
	صحیح		how)
{
    صحیح		invert = FALSE;
    صحیح		max_ستون;

# ifdef FEAT_PROP_POPUP
    max_ستون = cbd->max_ستون - 1;
# ورنہ
    max_ستون = Columns - 1;
# endif

    اگر (how == CLIP_سیٹ)
	invert = TRUE;

    // Swap the from and to positions so the from is always before
    اگر (clip_compare_pos(row1, col1, row2, col2) > 0)
    {
	صحیح tmp_صف, tmp_ستون;

	tmp_صف = row1;
	tmp_ستون = col1;
	row1	= row2;
	col1	= col2;
	row2	= tmp_صف;
	col2	= tmp_ستون;
    }
    ورنہ اگر (how == CLIP_TOGGLE)
	invert = TRUE;

    // If all on the same line, do it the easy way
    اگر (row1 == row2)
    {
	clip_invert_rectangle(cbd, row1, col1, 1, col2 - col1, invert);
    }
    ورنہ
    {
	// Handle a piece of the first line
	اگر (col1 > 0)
	{
	    clip_invert_rectangle(cbd, row1, col1, 1,
						  (صحیح)Columns - col1, invert);
	    row1++;
	}

	// Handle a piece of the last line
	اگر (col2 < max_ستون)
	{
	    clip_invert_rectangle(cbd, row2, 0, 1, col2, invert);
	    row2--;
	}

	// Handle the rectangle that's left
	اگر (row2 >= row1)
	    clip_invert_rectangle(cbd, row1, 0, row2 - row1 + 1,
							 (صحیح)Columns, invert);
    }
}

/*
 * Start, continue or end a modeless selection.  Used when editing the
 * command-line, in the cmdline window and when the mouse is in a popup window.
 */
    خالی
clip_modeless(صحیح button, صحیح is_click, صحیح is_drag)
{
    صحیح		repeat;

    repeat = ((clip_ستارہ.منوال == SELECT_منوال_CHAR
		|| clip_ستارہ.منوال == SELECT_منوال_سطر)
					      && (mod_نقاب & MOD_نقاب_2_CLICK))
	    || (clip_ستارہ.منوال == SELECT_منوال_لفظ
					     && (mod_نقاب & MOD_نقاب_3_CLICK));
    اگر (is_click && button == MOUSE_دایاں)
    {
	// Right mouse button: If there was no selection, start one.
	// Otherwise extend the existing selection.
	اگر (clip_ستارہ.state == SELECT_CLEARED)
	    clip_آغاز_selection(mouse_ستون, mouse_صف, FALSE);
	clip_process_selection(button, mouse_ستون, mouse_صف, repeat);
    }
    ورنہ اگر (is_click)
	clip_آغاز_selection(mouse_ستون, mouse_صف, repeat);
    ورنہ اگر (is_drag)
    {
	// Don't try extending a selection if there isn't one.  Happens when
	// button-down is in the cmdline and them moving mouse upwards.
	اگر (clip_ستارہ.state != SELECT_CLEARED)
	    clip_process_selection(button, mouse_ستون, mouse_صف, repeat);
    }
    ورنہ // release
	clip_process_selection(MOUSE_RELEASE, mouse_ستون, mouse_صف, FALSE);
}

/*
 * Update the currently selected region by adding and/or subtracting from the
 * beginning or end and inverting the changed area(s).
 */
    ساکن خالی
clip_update_modeless_selection(
    Clipboard_T    *cb,
    صحیح		    row1,
    صحیح		    col1,
    صحیح		    row2,
    صحیح		    col2)
{
    // See if we changed at the beginning of the selection
    اگر (row1 != cb->آغاز.lnum || col1 != (صحیح)cb->آغاز.ستون)
    {
	clip_invert_area(cb, row1, col1, (صحیح)cb->آغاز.lnum, cb->آغاز.ستون,
								 CLIP_TOGGLE);
	cb->آغاز.lnum = row1;
	cb->آغاز.ستون  = col1;
    }

    // See if we changed at the end of the selection
    اگر (row2 != cb->انتہا.lnum || col2 != (صحیح)cb->انتہا.ستون)
    {
	clip_invert_area(cb, (صحیح)cb->انتہا.lnum, cb->انتہا.ستون, row2, col2,
								 CLIP_TOGGLE);
	cb->انتہا.lnum = row2;
	cb->انتہا.ستون  = col2;
    }
}

/*
 * Find the starting and ending positions of the word at the given row and
 * column.  Only white-separated words are recognized here.
 */
# define CHAR_CLASS(c)	(c <= ' ' ? ' ' : vim_iswordc(c))

    ساکن خالی
clip_get_لفظ_boundaries(Clipboard_T *cb, صحیح صف, صحیح ستون)
{
    صحیح		آغاز_class;
    صحیح		عارضی_ستون;
    char_u	*p;
    صحیح		mboff;

    اگر (صف >= screen_صفیں || ستون >= screen_Columns || Screen_سطریں == تہی)
	واپس;

    p = Screen_سطریں + سطر_انحراف[صف];
    // Correct for starting in the right half of a double-wide char
    اگر (enc_dbcs != 0)
	ستون -= dbcs_screen_سر_off(p, p + ستون);
    ورنہ اگر (enc_utf8 && p[ستون] == 0)
	--ستون;
    آغاز_class = CHAR_CLASS(p[ستون]);

    عارضی_ستون = ستون;
    برائے ( ; عارضی_ستون > 0; عارضی_ستون--)
	اگر (enc_dbcs != 0
		   && (mboff = dbcs_screen_سر_off(p, p + عارضی_ستون - 1)) > 0)
	    عارضی_ستون -= mboff;
	ورنہ اگر (CHAR_CLASS(p[عارضی_ستون - 1]) != آغاز_class
		&& !(enc_utf8 && p[عارضی_ستون - 1] == 0))
	    توڑو;
    cb->لفظ_آغاز_ستون = عارضی_ستون;

    عارضی_ستون = ستون;
    برائے ( ; عارضی_ستون < screen_Columns; عارضی_ستون++)
	اگر (enc_dbcs != 0 && dbcs_ptr2cells(p + عارضی_ستون) == 2)
	    ++عارضی_ستون;
	ورنہ اگر (CHAR_CLASS(p[عارضی_ستون]) != آغاز_class
		&& !(enc_utf8 && p[عارضی_ستون] == 0))
	    توڑو;
    cb->لفظ_انتہا_ستون = عارضی_ستون;
}

/*
 * Find the column position for the last non-whitespace character on the given
 * line at or before start_col.
 */
    ساکن صحیح
clip_get_سطر_انتہا(Clipboard_T *cbd غیر_مستعمل, صحیح صف)
{
    صحیح	    i;

    اگر (صف >= screen_صفیں || Screen_سطریں == تہی)
	واپس 0;
    برائے (i =
# ifdef FEAT_PROP_POPUP
	    cbd->max_ستون;
# ورنہ
	    screen_Columns;
# endif
			    i > 0; i--)
	اگر (Screen_سطریں[سطر_انحراف[صف] + i - 1] != ' ')
	    توڑو;
    واپس i;
}

/*
 * Start the selection
 */
    خالی
clip_آغاز_selection(صحیح ستون, صحیح صف, صحیح repeated_click)
{
    Clipboard_T	*cb = &clip_ستارہ;
# ifdef FEAT_PROP_POPUP
    win_T	*wp;
    صحیح		صف_cp = صف;
    صحیح		ستون_cp = ستون;

    wp = mouse_ڈھونڈو_win(&صف_cp, &ستون_cp, ڈھونڈو_POPUP);
    اگر (wp != تہی && WIN_IS_POPUP(wp)
				  && popup_is_اندر_scrollbar(wp, صف_cp, ستون_cp))
	// click or double click in scrollbar does not start a selection
	واپس;
# endif

    اگر (cb->state == SELECT_مکمل)
	clip_clear_selection(cb);

    صف = check_صف(صف);
    ستون = check_ستون(ستون);
    ستون = mb_fix_ستون(ستون, صف);

    cb->آغاز.lnum  = صف;
    cb->آغاز.ستون   = ستون;
    cb->انتہا	    = cb->آغاز;
    cb->origin_صف  = (short_u)cb->آغاز.lnum;
    cb->state	    = SELECT_اندر_پیش_رفت;
# ifdef FEAT_PROP_POPUP
    اگر (wp != تہی && WIN_IS_POPUP(wp))
    {
	// Click in a popup window restricts selection to that window,
	// excluding the border.
	cb->min_ستون = wp->w_wincol + wp->w_popup_border[3];
	cb->max_ستون = wp->w_wincol + popup_چوڑائی(wp)
				 - wp->w_popup_border[1] - wp->w_has_scrollbar;
	اگر (cb->max_ستون > screen_Columns)
	    cb->max_ستون = screen_Columns;
	cb->min_صف = wp->w_winrow + wp->w_popup_border[0];
	cb->max_صف = wp->w_winrow + popup_اونچائی(wp) - 1
						   - wp->w_popup_border[2];
    }
    ورنہ
    {
	cb->min_ستون = 0;
	cb->max_ستون = screen_Columns;
	cb->min_صف = 0;
	cb->max_صف = screen_صفیں;
    }
# endif

    اگر (repeated_click)
    {
	اگر (++cb->منوال > SELECT_منوال_سطر)
	    cb->منوال = SELECT_منوال_CHAR;
    }
    ورنہ
	cb->منوال = SELECT_منوال_CHAR;

# ifdef FEAT_GUI
    // clear the cursor until the selection is made
    اگر (gui.اندر_use)
	gui_undraw_cursor();
# endif

    انتخاب (cb->منوال)
    {
	حالت SELECT_منوال_CHAR:
	    cb->origin_آغاز_ستون = cb->آغاز.ستون;
	    cb->لفظ_انتہا_ستون = clip_get_سطر_انتہا(cb, (صحیح)cb->آغاز.lnum);
	    توڑو;

	حالت SELECT_منوال_لفظ:
	    clip_get_لفظ_boundaries(cb, (صحیح)cb->آغاز.lnum, cb->آغاز.ستون);
	    cb->origin_آغاز_ستون = cb->لفظ_آغاز_ستون;
	    cb->origin_انتہا_ستون	 = cb->لفظ_انتہا_ستون;

	    clip_invert_area(cb, (صحیح)cb->آغاز.lnum, cb->لفظ_آغاز_ستون,
			    (صحیح)cb->انتہا.lnum, cb->لفظ_انتہا_ستون, CLIP_سیٹ);
	    cb->آغاز.ستون = cb->لفظ_آغاز_ستون;
	    cb->انتہا.ستون   = cb->لفظ_انتہا_ستون;
	    توڑو;

	حالت SELECT_منوال_سطر:
	    clip_invert_area(cb, (صحیح)cb->آغاز.lnum, 0, (صحیح)cb->آغاز.lnum,
			    (صحیح)Columns, CLIP_سیٹ);
	    cb->آغاز.ستون = 0;
	    cb->انتہا.ستون   = Columns;
	    توڑو;
    }

    cb->پچھلا = cb->آغاز;

# ifdef DEBUG_SELECTION
    لکھو("Selection started at (%ld,%d)\n", cb->آغاز.lnum, cb->آغاز.ستون);
# endif
}

/*
 * Continue processing the selection
 */
    خالی
clip_process_selection(
    صحیح		button,
    صحیح		ستون,
    صحیح		صف,
    int_u	repeated_click)
{
    Clipboard_T	*cb = &clip_ستارہ;
    صحیح		diff;
    صحیح		slen = 1;	// cursor shape width

    اگر (button == MOUSE_RELEASE)
    {
	اگر (cb->state != SELECT_اندر_پیش_رفت)
	    واپس;

	// Check to make sure we have something selected
	اگر (cb->آغاز.lnum == cb->انتہا.lnum && cb->آغاز.ستون == cb->انتہا.ستون)
	{
# ifdef FEAT_GUI
	    اگر (gui.اندر_use)
		gui_update_cursor(FALSE, FALSE);
# endif
	    cb->state = SELECT_CLEARED;
	    واپس;
	}

# ifdef DEBUG_SELECTION
	لکھو("Selection ended: (%ld,%d) to (%ld,%d)\n", cb->آغاز.lnum,
		cb->آغاز.ستون, cb->انتہا.lnum, cb->انتہا.ستون);
# endif
	اگر (clip_isautosel_ستارہ() || clip_isautosel_plus()
		|| (
# ifdef FEAT_GUI
		    gui.اندر_use ? (vim_strchr(p_go, GO_ASELML) != تہی) :
# endif
		    clip_autoselectml))
	    clip_copy_modeless_selection(FALSE);
# ifdef FEAT_GUI
	اگر (gui.اندر_use)
	    gui_update_cursor(FALSE, FALSE);
# endif

	cb->state = SELECT_مکمل;
	واپس;
    }

    صف = check_صف(صف);
    ستون = check_ستون(ستون);
    ستون = mb_fix_ستون(ستون, صف);

    اگر (ستون == (صحیح)cb->پچھلا.ستون && صف == cb->پچھلا.lnum && !repeated_click)
	واپس;

    /*
     * When extending the selection with the right mouse button, swap the
     * start and end if the position is before half the selection
     */
    اگر (cb->state == SELECT_مکمل && button == MOUSE_دایاں)
    {
	/*
	 * If the click is before the start, or the click is inside the
	 * selection and the start is the closest side, set the origin to the
	 * end of the selection.
	 */
	اگر (clip_compare_pos(صف, ستون, (صحیح)cb->آغاز.lnum, cb->آغاز.ستون) < 0
		|| (clip_compare_pos(صف, ستون,
					   (صحیح)cb->انتہا.lnum, cb->انتہا.ستون) < 0
		    && (((cb->آغاز.lnum == cb->انتہا.lnum
			    && cb->انتہا.ستون - ستون > ستون - cb->آغاز.ستون))
			|| ((diff = (cb->انتہا.lnum - صف) -
						   (صف - cb->آغاز.lnum)) > 0
			    || (diff == 0 && ستون < (صحیح)(cb->آغاز.ستون +
							 cb->انتہا.ستون) / 2)))))
	{
	    cb->origin_صف = (short_u)cb->انتہا.lnum;
	    cb->origin_آغاز_ستون = cb->انتہا.ستون - 1;
	    cb->origin_انتہا_ستون = cb->انتہا.ستون;
	}
	ورنہ
	{
	    cb->origin_صف = (short_u)cb->آغاز.lnum;
	    cb->origin_آغاز_ستون = cb->آغاز.ستون;
	    cb->origin_انتہا_ستون = cb->آغاز.ستون;
	}
	اگر (cb->منوال == SELECT_منوال_لفظ && !repeated_click)
	    cb->منوال = SELECT_منوال_CHAR;
    }

    // set state, for when using the right mouse button
    cb->state = SELECT_اندر_پیش_رفت;

# ifdef DEBUG_SELECTION
    لکھو("Selection extending to (%d,%d)\n", صف, ستون);
# endif

    اگر (repeated_click && ++cb->منوال > SELECT_منوال_سطر)
	cb->منوال = SELECT_منوال_CHAR;

    انتخاب (cb->منوال)
    {
	حالت SELECT_منوال_CHAR:
	    // If we're on a different line, find where the line ends
	    اگر (صف != cb->پچھلا.lnum)
		cb->لفظ_انتہا_ستون = clip_get_سطر_انتہا(cb, صف);

	    // See if we are before or after the origin of the selection
	    اگر (clip_compare_pos(صف, ستون, cb->origin_صف,
						   cb->origin_آغاز_ستون) >= 0)
	    {
		اگر (ستون >= (صحیح)cb->لفظ_انتہا_ستون)
		    clip_update_modeless_selection(cb, cb->origin_صف,
			    cb->origin_آغاز_ستون, صف, (صحیح)Columns);
		ورنہ
		{
		    اگر (has_mbyte && mb_lefthalve(صف, ستون))
			slen = 2;
		    clip_update_modeless_selection(cb, cb->origin_صف,
			    cb->origin_آغاز_ستون, صف, ستون + slen);
		}
	    }
	    ورنہ
	    {
		اگر (has_mbyte
			&& mb_lefthalve(cb->origin_صف, cb->origin_آغاز_ستون))
		    slen = 2;
		اگر (ستون >= (صحیح)cb->لفظ_انتہا_ستون)
		    clip_update_modeless_selection(cb, صف, cb->لفظ_انتہا_ستون,
			    cb->origin_صف, cb->origin_آغاز_ستون + slen);
		ورنہ
		    clip_update_modeless_selection(cb, صف, ستون,
			    cb->origin_صف, cb->origin_آغاز_ستون + slen);
	    }
	    توڑو;

	حالت SELECT_منوال_لفظ:
	    // If we are still within the same word, do nothing
	    اگر (صف == cb->پچھلا.lnum && ستون >= (صحیح)cb->لفظ_آغاز_ستون
		    && ستون < (صحیح)cb->لفظ_انتہا_ستون && !repeated_click)
		واپس;

	    // Get new word boundaries
	    clip_get_لفظ_boundaries(cb, صف, ستون);

	    // Handle being after the origin point of selection
	    اگر (clip_compare_pos(صف, ستون, cb->origin_صف,
		    cb->origin_آغاز_ستون) >= 0)
		clip_update_modeless_selection(cb, cb->origin_صف,
			cb->origin_آغاز_ستون, صف, cb->لفظ_انتہا_ستون);
	    ورنہ
		clip_update_modeless_selection(cb, صف, cb->لفظ_آغاز_ستون,
			cb->origin_صف, cb->origin_انتہا_ستون);
	    توڑو;

	حالت SELECT_منوال_سطر:
	    اگر (صف == cb->پچھلا.lnum && !repeated_click)
		واپس;

	    اگر (clip_compare_pos(صف, ستون, cb->origin_صف,
		    cb->origin_آغاز_ستون) >= 0)
		clip_update_modeless_selection(cb, cb->origin_صف, 0, صف,
			(صحیح)Columns);
	    ورنہ
		clip_update_modeless_selection(cb, صف, 0, cb->origin_صف,
			(صحیح)Columns);
	    توڑو;
    }

    cb->پچھلا.lnum = صف;
    cb->پچھلا.ستون  = ستون;

# ifdef DEBUG_SELECTION
	لکھو("Selection is: (%ld,%d) to (%ld,%d)\n", cb->آغاز.lnum,
		cb->آغاز.ستون, cb->انتہا.lnum, cb->انتہا.ستون);
# endif
}

# اگر defined(FEAT_GUI)
/*
 * Redraw part of the selection if character at "row,col" is inside of it.
 * Only used for the GUI.
 */
    خالی
clip_may_redraw_selection(صحیح صف, صحیح ستون, صحیح len)
{
    صحیح		آغاز = ستون;
    صحیح		انتہا = ستون + len;

    اگر (clip_ستارہ.state != SELECT_CLEARED
	    && صف >= clip_ستارہ.آغاز.lnum
	    && صف <= clip_ستارہ.انتہا.lnum)
    {
	اگر (صف == clip_ستارہ.آغاز.lnum && آغاز < (صحیح)clip_ستارہ.آغاز.ستون)
	    آغاز = clip_ستارہ.آغاز.ستون;
	اگر (صف == clip_ستارہ.انتہا.lnum && انتہا > (صحیح)clip_ستارہ.انتہا.ستون)
	    انتہا = clip_ستارہ.انتہا.ستون;
	اگر (انتہا > آغاز)
	    clip_invert_area(&clip_ستارہ, صف, آغاز, صف, انتہا, 0);
    }
}
# endif

/*
 * Called from outside to clear selected region from the display
 */
    خالی
clip_clear_selection(Clipboard_T *cbd)
{

    اگر (cbd->state == SELECT_CLEARED)
	واپس;

    clip_invert_area(cbd, (صحیح)cbd->آغاز.lnum, cbd->آغاز.ستون,
				 (صحیح)cbd->انتہا.lnum, cbd->انتہا.ستون, CLIP_CLEAR);
    cbd->state = SELECT_CLEARED;
}

/*
 * Clear the selection if any lines from "row1" to "row2" are inside of it.
 */
    خالی
clip_may_clear_selection(صحیح row1, صحیح row2)
{
    اگر (clip_ستارہ.state == SELECT_مکمل
	    && row2 >= clip_ستارہ.آغاز.lnum
	    && row1 <= clip_ستارہ.انتہا.lnum)
	clip_clear_selection(&clip_ستارہ);
}

/*
 * Called before the screen is scrolled up or down.  Adjusts the line numbers
 * of the selection.  Call with big number when clearing the screen.
 */
    خالی
clip_scroll_selection(
    صحیح	    صفیں_2)		// negative for scroll down
{
    صحیح	    lnum;

    اگر (clip_ستارہ.state == SELECT_CLEARED)
	واپس;

    lnum = clip_ستارہ.آغاز.lnum - صفیں_2;
    اگر (lnum <= 0)
	clip_ستارہ.آغاز.lnum = 0;
    ورنہ اگر (lnum >= screen_صفیں)	// scrolled off of the screen
	clip_ستارہ.state = SELECT_CLEARED;
    ورنہ
	clip_ستارہ.آغاز.lnum = lnum;

    lnum = clip_ستارہ.انتہا.lnum - صفیں_2;
    اگر (lnum < 0)			// scrolled off of the screen
	clip_ستارہ.state = SELECT_CLEARED;
    ورنہ اگر (lnum >= screen_صفیں)
	clip_ستارہ.انتہا.lnum = screen_صفیں - 1;
    ورنہ
	clip_ستارہ.انتہا.lnum = lnum;
}

/*
 * Copy the currently selected area into the '*' or '+' register so it will be
 * available for pasting.
 * When "both" is TRUE also copy to the other register.
 */
    خالی
clip_copy_modeless_selection(صحیح both غیر_مستعمل)
{
    // The info for the modeless selection is stored in '*' register, however if
    // we are using the '+' register for modeless autoselect, we copy to
    // clip_plus instead while using the info in clip_star.
    Clipboard_T *cbd = clip_isautosel_plus() ? &clip_plus : &clip_ستارہ;
    char_u	*بفر;
    char_u	*bufp;
    صحیح		صف;
    صحیح		آغاز_ستون;
    صحیح		انتہا_ستون;
    صحیح		سطر_انتہا_ستون;
    صحیح		جوڑو_newline_flag = FALSE;
    صحیح		len;
    char_u	*p;
    صحیح		row1 = clip_ستارہ.آغاز.lnum;
    صحیح		col1 = clip_ستارہ.آغاز.ستون;
    صحیح		row2 = clip_ستارہ.انتہا.lnum;
    صحیح		col2 = clip_ستارہ.انتہا.ستون;

    // Can't use ScreenLines unless initialized
    اگر (Screen_سطریں == تہی)
	واپس;

    /*
     * Make sure row1 <= row2, and if row1 == row2 that col1 <= col2.
     */
    اگر (row1 > row2)
    {
	صف = row1; row1 = row2; row2 = صف;
	صف = col1; col1 = col2; col2 = صف;
    }
    ورنہ اگر (row1 == row2 && col1 > col2)
    {
	صف = col1; col1 = col2; col2 = صف;
    }
# ifdef FEAT_PROP_POPUP
    اگر (col1 < clip_ستارہ.min_ستون)
	col1 = clip_ستارہ.min_ستون;
    اگر (col2 > clip_ستارہ.max_ستون)
	col2 = clip_ستارہ.max_ستون;
    اگر (row1 > clip_ستارہ.max_صف || row2 < clip_ستارہ.min_صف)
	واپس;
    اگر (row1 < clip_ستارہ.min_صف)
	row1 = clip_ستارہ.min_صف;
    اگر (row2 > clip_ستارہ.max_صف)
	row2 = clip_ستارہ.max_صف;
# endif
    // correct starting point for being on right half of double-wide char
    p = Screen_سطریں + سطر_انحراف[row1];
    اگر (enc_dbcs != 0)
	col1 -= (*mb_سر_off)(p, p + col1);
    ورنہ اگر (enc_utf8 && p[col1] == 0)
	--col1;

    // Create a temporary buffer for storing the text
    len = (row2 - row1 + 1) * Columns + 1;
    اگر (enc_dbcs != 0)
	len *= 2;	// max. 2 bytes per display cell
    ورنہ اگر (enc_utf8)
	len *= MB_MAXBYTES;
    // A newline is added after each row
    len += row2 - row1 + 2;
    بفر = alloc(len);
    اگر (بفر == تہی)	    // out of memory
	واپس;

    // Process each row in the selection
    برائے (bufp = بفر, صف = row1; صف <= row2; صف++)
    {
	اگر (صف == row1)
	    آغاز_ستون = col1;
	ورنہ
# ifdef FEAT_PROP_POPUP
	    آغاز_ستون = clip_ستارہ.min_ستون;
# ورنہ
	    آغاز_ستون = 0;
# endif

	اگر (صف == row2)
	    انتہا_ستون = col2;
	ورنہ
# ifdef FEAT_PROP_POPUP
	    انتہا_ستون = clip_ستارہ.max_ستون;
# ورنہ
	    انتہا_ستون = Columns;
# endif

	سطر_انتہا_ستون = clip_get_سطر_انتہا(&clip_ستارہ, صف);

	// See if we need to nuke some trailing whitespace
	اگر (انتہا_ستون >=
# ifdef FEAT_PROP_POPUP
		clip_ستارہ.max_ستون
# ورنہ
		Columns
# endif
		    && (صف < row2 || انتہا_ستون > سطر_انتہا_ستون))
	{
	    // Get rid of trailing whitespace
	    انتہا_ستون = سطر_انتہا_ستون;
	    اگر (انتہا_ستون < آغاز_ستون)
		انتہا_ستون = آغاز_ستون;

	    // If the last line extended to the end, add an extra newline
	    اگر (صف == row2)
		جوڑو_newline_flag = TRUE;
	}

	// If after the first row, we need to always add a newline
	اگر (صف > row1 && !سطر_Wraps[صف - 1])
	    *bufp++ = NL;

	// Safetey check for in case resizing went wrong
	اگر (صف < screen_صفیں && انتہا_ستون <= screen_Columns)
	{
	    اگر (enc_dbcs != 0)
	    {
		صحیح	i;

		p = Screen_سطریں + سطر_انحراف[صف];
		برائے (i = آغاز_ستون; i < انتہا_ستون; ++i)
		    اگر (enc_dbcs == DBCS_JPNU && p[i] == 0x8e)
		    {
			// single-width double-byte char
			*bufp++ = 0x8e;
			*bufp++ = ScreenLines2[سطر_انحراف[صف] + i];
		    }
		    ورنہ
		    {
			*bufp++ = p[i];
			اگر (MB_BYTE2LEN(p[i]) == 2)
			    *bufp++ = p[++i];
		    }
	    }
	    ورنہ اگر (enc_utf8)
	    {
		صحیح	off;
		صحیح	i;

		off = سطر_انحراف[صف];
		برائے (i = آغاز_ستون; i < انتہا_ستون; ++i)
		{
		    // The base character is either in ScreenLinesUC[] or
		    // ScreenLines[].
		    اگر (Screen_سطریں_UC[off + i] == 0)
			*bufp++ = Screen_سطریں[off + i];
		    ورنہ
			// Includes the composing characters.
			bufp += utfc_char2bytes(off + i, bufp);
		    // Skip right half of double-wide character.
		    اگر (Screen_سطریں[off + i + 1] == 0)
			++i;
		}
	    }
	    ورنہ
	    {
		STRNCPY(bufp, Screen_سطریں + سطر_انحراف[صف] + آغاز_ستون,
							 انتہا_ستون - آغاز_ستون);
		bufp += انتہا_ستون - آغاز_ستون;
	    }
	}
    }

    // Add a newline at the end if the selection ended there
    اگر (جوڑو_newline_flag)
	*bufp++ = NL;

    // First cleanup any old selection and become the owner.
    clip_free_selection(cbd);
    clip_own_selection(cbd);

    // Yank the text into the '*' register.
    clip_yank_selection(MCHAR, بفر, (طویل)(bufp - بفر), cbd);

    // Make the register contents available to the outside world.
    clip_gen_سیٹ_selection(cbd);

# ifdef FEAT_X11
    اگر (both)
    {
	Clipboard_T *other = cbd == &clip_ستارہ ? &clip_plus : &clip_ستارہ;
	// Do the same for the '+' register.
	clip_free_selection(other);
	clip_own_selection(other);
	clip_yank_selection(MCHAR, بفر, (طویل)(bufp - بفر), other);
	clip_gen_سیٹ_selection(other);
    }
# endif
    vim_free(بفر);
}

    خالی
clip_gen_سیٹ_selection(Clipboard_T *cbd)
{
    اگر (!clip_did_سیٹ_selection)
    {
	// Updating postponed, so that accessing the system clipboard won't
	// hang Vim when accessing it many times (e.g. on a :g command).
	اگر ((cbd == &clip_plus && (clip_unnamed_saved & CLIP_UNNAMED_PLUS))
		|| (cbd == &clip_ستارہ && (clip_unnamed_saved & CLIP_UNNAMED)))
	{
	    clipboard_needs_update = TRUE;
	    واپس;
	}
    }
# اگر defined(FEAT_XCLIPBOARD) || defined(FEAT_WAYLAND_CLIPBOARD)
#  ifdef FEAT_GUI
    اگر (gui.اندر_use)
	clip_mch_سیٹ_selection(cbd);
    ورنہ
#  endif
    {
	اگر (clipmethod == CLIPMETHOD_WAYLAND)
	{
#  ifdef FEAT_WAYLAND_CLIPBOARD
	    clip_wl_سیٹ_selection(cbd);
#  endif
	}
	ورنہ اگر (clipmethod == CLIPMETHOD_X11)
	{
#  ifdef FEAT_XCLIPBOARD
	    clip_xterm_سیٹ_selection(cbd);
#  endif
	}
    }
# ورنہ
    clip_mch_سیٹ_selection(cbd);
# endif
}

    ساکن خالی
clip_gen_request_selection(Clipboard_T *cbd)
{
# اگر defined(FEAT_XCLIPBOARD) || defined(FEAT_WAYLAND_CLIPBOARD)
#  ifdef FEAT_GUI
    اگر (gui.اندر_use)
	clip_mch_request_selection(cbd);
    ورنہ
#  endif
    {
	اگر (clipmethod == CLIPMETHOD_WAYLAND)
	{
#  ifdef FEAT_WAYLAND_CLIPBOARD
	    clip_wl_request_selection(cbd);
#  endif
	}
	ورنہ اگر (clipmethod == CLIPMETHOD_X11)
	{
#  ifdef FEAT_XCLIPBOARD
	    clip_xterm_request_selection(cbd);
#  endif
	}
    }
# ورنہ
    clip_mch_request_selection(cbd);
# endif
}

# اگر (defined(FEAT_X11) && defined(FEAT_XCLIPBOARD) && defined(USE_SYSTEM))
    ساکن صحیح
clip_x11_owner_exists(Clipboard_T *cbd)
{
    واپس XGetSelectionOwner(X_DISPLAY, cbd->sel_atom) != None;
}
# endif

# اگر (defined(FEAT_X11) || defined(FEAT_WAYLAND_CLIPBOARD)) \
	&& defined(USE_SYSTEM)
    صحیح
clip_gen_owner_exists(Clipboard_T *cbd غیر_مستعمل)
{
#  اگر defined(FEAT_XCLIPBOARD) || defined(FEAT_WAYLAND_CLIPBOARD)
#   ifdef FEAT_GUI_GTK
    اگر (gui.اندر_use)
	واپس clip_gtk_owner_exists(cbd);
    ورنہ
#   endif
    {
	اگر (clipmethod == CLIPMETHOD_WAYLAND)
	{
#   ifdef FEAT_WAYLAND_CLIPBOARD
	    واپس clip_wl_owner_exists(cbd);
#   endif
	}
	ورنہ اگر (clipmethod == CLIPMETHOD_X11)
	{
#   ifdef FEAT_XCLIPBOARD
	    واپس clip_x11_owner_exists(cbd);
#   endif
	}
	ورنہ
	    واپس FALSE;
    }
#  ورنہ
    واپس TRUE;
#  endif
}
# endif

/*
 * Stuff for the X clipboard.  Shared between VMS and Unix.
 */

# اگر defined(FEAT_XCLIPBOARD) || defined(FEAT_GUI_X11)
#  include <X11/Xatom.h>
#  include <X11/Intrinsic.h>

/*
 * Open the application context (if it hasn't been opened yet).
 * Used for Motif GUI and the xterm clipboard.
 */
    خالی
open_app_context(خالی)
{
    اگر (app_context == تہی)
    {
	XtToolkitInitialize();
	app_context = XtCreateApplicationContext();
    }
}

ساکن Atom	vim_atom;	// Vim's own special selection format
ساکن Atom	vimenc_atom;	// Vim's extended selection format
ساکن Atom	vim_mt_atom;	// Vim's own special selection format (in mime
				// type format)
ساکن Atom	vimenc_mt_atom;	// Vim's extended selection format (in mime type
				// format)
ساکن Atom	utf8_atom;
ساکن Atom	compound_متن_atom;
ساکن Atom	متن_atom;
ساکن Atom	targets_atom;
ساکن Atom	timestamp_atom;	// Used to get a timestamp

    خالی
x11_تیاری_atoms(Display *dpy)
{
    vim_atom	       = XInternAtom(dpy, VIM_ATOM_NAME,   False);
    vimenc_atom	       = XInternAtom(dpy, VIMENC_ATOM_NAME,False);
    vim_mt_atom	       = XInternAtom(dpy, VIM_MIMETYPE_NAME,   False);
    vimenc_mt_atom     = XInternAtom(dpy, VIMENC_MIMETYPE_NAME,False);
    utf8_atom	       = XInternAtom(dpy, "UTF8_STRING",   False);
    compound_متن_atom = XInternAtom(dpy, "COMPOUND_TEXT", False);
    متن_atom	       = XInternAtom(dpy, "TEXT",	   False);
    targets_atom       = XInternAtom(dpy, "TARGETS",	   False);
    clip_ستارہ.sel_atom = XA_PRIMARY;
    clip_plus.sel_atom = XInternAtom(dpy, "CLIPBOARD",	   False);
    timestamp_atom     = XInternAtom(dpy, "TIMESTAMP",	   False);
}

/*
 * X Selection stuff, for cutting and pasting text to other windows.
 */

    ساکن Boolean
clip_x11_convert_selection_cb(
    Widget	w غیر_مستعمل,
    Atom	*sel_atom,
    Atom	*ہدف,
    Atom	*type,
    Xt_اشارہ_گر	*قدر,
    long_u	*لمبائی,
    صحیح		*format)
{
    ساکن char_u   *save_نتیجہ = تہی;
    ساکن long_u   save_لمبائی = 0;
    char_u	    *string;
    صحیح		    motion_type;
    Clipboard_T    *cbd;
    صحیح		    i;

    اگر (*sel_atom == clip_plus.sel_atom)
	cbd = &clip_plus;
    ورنہ
	cbd = &clip_ستارہ;

    اگر (!cbd->owned)
	واپس False;	    // Shouldn't ever happen

    // requestor wants to know what target types we support
    اگر (*ہدف == targets_atom)
    {
	ساکن Atom array[9];

	*قدر = (Xt_اشارہ_گر)array;
	i = 0;
	array[i++] = targets_atom;
	array[i++] = vimenc_atom;
	array[i++] = vim_atom;
	array[i++] = vimenc_mt_atom;
	array[i++] = vim_mt_atom;
	اگر (enc_utf8)
	    array[i++] = utf8_atom;
	array[i++] = XA_STRING;
	array[i++] = متن_atom;
	array[i++] = compound_متن_atom;

	*type = XA_ATOM;
	// This used to be: *format = sizeof(Atom) * 8; but that caused
	// crashes on 64 bit machines. (Peter Derr)
	*format = 32;
	*لمبائی = i;
	واپس True;
    }

    اگر (       *ہدف != XA_STRING
	    && *ہدف != vimenc_atom
	    && *ہدف != vimenc_mt_atom
	    && (*ہدف != utf8_atom || !enc_utf8)
	    && *ہدف != vim_atom
	    && *ہدف != vim_mt_atom
	    && *ہدف != متن_atom
	    && *ہدف != compound_متن_atom)
	واپس False;

    clip_get_selection(cbd);
    motion_type = clip_convert_selection(&string, لمبائی, cbd);
    اگر (motion_type < 0)
	واپس False;

    // For our own format, the first byte contains the motion type
    اگر (*ہدف == vim_atom || *ہدف == vim_mt_atom)
	(*لمبائی)++;

    // Our own format with encoding: motion 'encoding' NUL text
    اگر (*ہدف == vimenc_atom || *ہدف == vimenc_mt_atom)
	*لمبائی += STRLEN(p_enc) + 2;

    اگر (save_لمبائی < *لمبائی || save_لمبائی / 2 >= *لمبائی)
	*قدر = XtRealloc((حرف *)save_نتیجہ, (Cardinal)*لمبائی + 1);
    ورنہ
	*قدر = save_نتیجہ;
    اگر (*قدر == تہی)
    {
	vim_free(string);
	واپس False;
    }
    save_نتیجہ = (char_u *)*قدر;
    save_لمبائی = *لمبائی;

    اگر (*ہدف == XA_STRING || (*ہدف == utf8_atom && enc_utf8))
    {
	mch_memmove(save_نتیجہ, string, (size_t)(*لمبائی));
	*type = *ہدف;
    }
    ورنہ اگر (*ہدف == compound_متن_atom || *ہدف == متن_atom)
    {
	XTextProperty	متن_prop;
	حرف		*string_nt = (حرف *)save_نتیجہ;
	صحیح		conv_نتیجہ;

	// create NUL terminated string which XmbTextListToTextProperty wants
	mch_memmove(string_nt, string, (size_t)*لمبائی);
	string_nt[*لمبائی] = NUL;
	conv_نتیجہ = Xmb_متن_List_مقصد_متن_Property(X_DISPLAY, &string_nt,
					   1, XCompound_متن_Style, &متن_prop);
	اگر (conv_نتیجہ != Success)
	{
	    vim_free(string);
	    واپس False;
	}
	*قدر = (Xt_اشارہ_گر)(متن_prop.قدر);	//    from plain text
	*لمبائی = متن_prop.nitems;
	*type = compound_متن_atom;
	XtFree((حرف *)save_نتیجہ);
	save_نتیجہ = (char_u *)*قدر;
	save_لمبائی = *لمبائی;
    }
    ورنہ اگر (*ہدف == vimenc_atom || *ہدف == vimenc_mt_atom)
    {
	صحیح l = STRLEN(p_enc);

	save_نتیجہ[0] = motion_type;
	STRCPY(save_نتیجہ + 1, p_enc);
	mch_memmove(save_نتیجہ + l + 2, string, (size_t)(*لمبائی - l - 2));
	اگر (*ہدف == vimenc_atom)
	    *type = vimenc_atom;
	ورنہ
	    *type = vimenc_mt_atom;
    }
    ورنہ
    {
	save_نتیجہ[0] = motion_type;
	mch_memmove(save_نتیجہ + 1, string, (size_t)(*لمبائی - 1));
	اگر (*ہدف == vim_atom)
	    *type = vim_atom;
	ورنہ
	    *type = vim_mt_atom;
    }
    *format = 8;	    // 8 bits per char
    vim_free(string);
    واپس True;
}

    ساکن خالی
clip_x11_lose_ownership_cb(Widget w غیر_مستعمل, Atom *sel_atom)
{
    اگر (*sel_atom == clip_plus.sel_atom)
	clip_lose_selection(&clip_plus);
    ورنہ
	clip_lose_selection(&clip_ستارہ);
}

    ساکن خالی
clip_x11_notify_cb(Widget w غیر_مستعمل, Atom *sel_atom غیر_مستعمل, Atom *ہدف غیر_مستعمل)
{
    // To prevent automatically freeing the selection value.
}

/*
 * Property callback to get a timestamp for XtOwnSelection.
 */
#  اگر defined(FEAT_X11) && defined(FEAT_XCLIPBOARD)
    ساکن خالی
clip_x11_timestamp_cb(
    Widget	w,
    Xt_اشارہ_گر	n غیر_مستعمل,
    XEvent	*event,
    Boolean	*cont غیر_مستعمل)
{
    Atom	    actual_type;
    صحیح		    format;
    غیرعلامتی  طویل  nitems, بائٹیں_after;
    غیرعلامتی حرف   *prop=تہی;
    XPropertyEvent  *xproperty=&event->xproperty;

    // Must be a property notify, state can't be Delete (True), has to be
    // one of the supported selection types.
    اگر (event->type != PropertyNotify || xproperty->state
	    || (xproperty->atom != clip_ستارہ.sel_atom
				    && xproperty->atom != clip_plus.sel_atom))
	واپس;

    اگر (XGet_کھڑکی_Property(xproperty->display, xproperty->کھڑکی_2,
	  xproperty->atom, 0, 0, False, timestamp_atom, &actual_type, &format,
						&nitems, &بائٹیں_after, &prop))
	واپس;

    اگر (prop)
	XFree(prop);

    // Make sure the property type is "TIMESTAMP" and it's 32 bits.
    اگر (actual_type != timestamp_atom || format != 32)
	واپس;

    // Get the selection, using the event timestamp.
    اگر (XtOwnSelection(w, xproperty->atom, xproperty->وقت,
	    clip_x11_convert_selection_cb, clip_x11_lose_ownership_cb,
	    clip_x11_notify_cb) == OK)
    {
	// Set the "owned" flag now, there may have been a call to
	// lose_ownership_cb in between.
	اگر (xproperty->atom == clip_plus.sel_atom)
	    clip_plus.owned = TRUE;
	ورنہ
	    clip_ستارہ.owned = TRUE;
    }
}

    خالی
x11_تیاری_selection(Widget w)
{
    Xt_جوڑو_Event_Handler(w, Property_Change_نقاب, False,
	    /*(XtEventHandler)*/clip_x11_timestamp_cb, (Xt_اشارہ_گر)تہی);
}
#  endif

    ساکن خالی
clip_x11_request_selection_cb(
    Widget	w غیر_مستعمل,
    Xt_اشارہ_گر	success,
    Atom	*sel_atom,
    Atom	*type,
    Xt_اشارہ_گر	قدر,
    long_u	*لمبائی,
    صحیح		*format)
{
    صحیح		motion_type = MAUTO;
    long_u	len;
    char_u	*p;
    حرف	**متن_list = تہی;
    Clipboard_T	*cbd;
    char_u	*tmpbuf = تہی;

    اگر (*sel_atom == clip_plus.sel_atom)
	cbd = &clip_plus;
    ورنہ
	cbd = &clip_ستارہ;

    اگر (قدر == تہی || *لمبائی == 0)
    {
	clip_free_selection(cbd);	// nothing received, clear register
	*(صحیح *)success = FALSE;
	واپس;
    }
    p = (char_u *)قدر;
    len = *لمبائی;
    اگر (*type == vim_atom || *type == vim_mt_atom)
    {
	motion_type = *p++;
	len--;
    }

    ورنہ اگر (*type == vimenc_atom || *type == vimenc_mt_atom)
    {
	char_u		*enc;
	vimconv_T	conv;
	صحیح		convlen;

	motion_type = *p++;
	--len;

	enc = p;
	p += STRLEN(p) + 1;
	len -= p - enc;

	// If the encoding of the text is different from 'encoding', attempt
	// converting it.
	conv.vc_type = CONV_NONE;
	convert_تیاری(&conv, enc, p_enc);
	اگر (conv.vc_type != CONV_NONE)
	{
	    convlen = len;	// Need to use an int here.
	    tmpbuf = string_convert(&conv, p, &convlen);
	    len = convlen;
	    اگر (tmpbuf != تہی)
		p = tmpbuf;
	    convert_تیاری(&conv, تہی, تہی);
	}
    }

    ورنہ اگر (*type == compound_متن_atom
	    || *type == utf8_atom
	    || (enc_dbcs != 0 && *type == متن_atom))
    {
	XTextProperty	متن_prop;
	صحیح		n_متن = 0;
	صحیح		حالت_2;

	متن_prop.قدر = (غیرعلامتی حرف *)قدر;
	متن_prop.encoding = *type;
	متن_prop.format = *format;
	متن_prop.nitems = len;
#  اگر defined(X_HAVE_UTF8_STRING)
	اگر (*type == utf8_atom)
	    حالت_2 = Xutf8_متن_Property_مقصد_متن_List(X_DISPLAY, &متن_prop,
							 &متن_list, &n_متن);
	ورنہ
#  endif
	    حالت_2 = Xmb_متن_Property_مقصد_متن_List(X_DISPLAY, &متن_prop,
							 &متن_list, &n_متن);
	اگر (حالت_2 != Success || n_متن < 1)
	{
	    *(صحیح *)success = FALSE;
	    واپس;
	}
	p = (char_u *)متن_list[0];
	len = STRLEN(p);
    }
    clip_yank_selection(motion_type, p, (طویل)len, cbd);

    اگر (متن_list != تہی)
	XFreeStringList(متن_list);
    vim_free(tmpbuf);
    XtFree((حرف *)قدر);
    *(صحیح *)success = TRUE;
}

    خالی
clip_x11_request_selection(
    Widget	myShell,
    Display	*dpy,
    Clipboard_T	*cbd)
{
    XEvent	event;
    Atom	type;
    ساکن صحیح	success;
    صحیح		i;
    time_t	آغاز_وقت;
    صحیح		timed_برآمد = FALSE;

    برائے (i = 0; i < 8; i++)
    {
	انتخاب (i)
	{
	    حالت 0:  type = vimenc_mt_atom;	توڑو;
	    حالت 1:  type = vimenc_atom;	توڑو;
	    حالت 2:  type = vim_mt_atom;	توڑو;
	    حالت 3:  type = vim_atom;		توڑو;
	    حالت 4:  type = utf8_atom;		توڑو;
	    حالت 5:  type = compound_متن_atom; توڑو;
	    حالت 6:  type = متن_atom;		توڑو;
	    طےشدہ: type = XA_STRING;
	}
	اگر (type == utf8_atom
#  اگر defined(X_HAVE_UTF8_STRING)
		&& !enc_utf8
#  endif
		)
	    // Only request utf-8 when 'encoding' is utf8 and
	    // Xutf8TextPropertyToTextList is available.
	    جاری;
	success = MAYBE;
	Xt_Get_Selection_قدر(myShell, cbd->sel_atom, type,
	    clip_x11_request_selection_cb, (Xt_اشارہ_گر)&success, Current_وقت);

	// Make sure the request for the selection goes out before waiting for
	// a response.
	XFlush(dpy);

	/*
	 * Wait for result of selection request, otherwise if we type more
	 * characters, then they will appear before the one that requested the
	 * paste!  Don't worry, we will catch up with any other events later.
	 */
	آغاز_وقت = وقت(تہی);
	جب (success == MAYBE)
	{
	    اگر (XCheckTypedEvent(dpy, PropertyNotify, &event)
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
		جاری;
	    }

	    // Time out after 2 to 3 seconds to avoid that we hang when the
	    // other process doesn't respond.  Note that the SelectionNotify
	    // event may still come later when the selection owner comes back
	    // to life and the text gets inserted unexpectedly.  Don't know
	    // why that happens or how to avoid that :-(.
	    اگر (وقت(تہی) > آغاز_وقت + 2)
	    {
		timed_برآمد = TRUE;
		توڑو;
	    }

	    // Do we need this?  Probably not.
	    XSync(dpy, False);

	    // Wait for 1 msec to avoid that we eat up all CPU time.
	    ui_delay(1L, TRUE);
	}

	اگر (success == TRUE)
	    واپس;

	// don't do a retry with another type after timing out, otherwise we
	// hang for 15 seconds.
	اگر (timed_برآمد)
	    توڑو;
    }

    // Final fallback position - use the X CUT_BUFFER0 store
    yank_cut_buffer0(dpy, cbd);
}

    خالی
clip_x11_lose_selection(Widget myShell, Clipboard_T *cbd)
{
    XtDisownSelection(myShell, cbd->sel_atom,
				Xt_آخری_Timestamp_Processed(XtDisplay(myShell)));
}

    صحیح
clip_x11_own_selection(Widget myShell, Clipboard_T *cbd)
{
    // When using the GUI we have proper timestamps, use the one of the last
    // event.  When in the console we don't get events (the terminal gets
    // them), Get the time by a zero-length append, clip_x11_timestamp_cb will
    // be called with the current timestamp.
#  ifdef FEAT_GUI
    اگر (gui.اندر_use)
    {
	اگر (XtOwnSelection(myShell, cbd->sel_atom,
	       Xt_آخری_Timestamp_Processed(XtDisplay(myShell)),
	       clip_x11_convert_selection_cb, clip_x11_lose_ownership_cb,
	       clip_x11_notify_cb) == False)
	    واپس FAIL;
    }
    ورنہ
#  endif
    {
	اگر (!XChangeProperty(XtDisplay(myShell), Xt_کھڑکی(myShell),
		  cbd->sel_atom, timestamp_atom, 32, Prop_منوال_Append, تہی, 0))
	    واپس FAIL;
    }
    // Flush is required in a terminal as nothing else is doing it.
    XFlush(XtDisplay(myShell));
    واپس OK;
}

/*
 * Send the current selection to the clipboard.  Do nothing for X because we
 * will fill in the selection only when requested by another app.
 */
    خالی
clip_x11_سیٹ_selection(Clipboard_T *cbd غیر_مستعمل)
{
}

# endif

# اگر (defined(FEAT_XCLIPBOARD) || defined(FEAT_GUI_X11) || defined(FEAT_GUI_GTK)) \
	&& !defined(USE_GTK4)
/*
 * Get the contents of the X CUT_BUFFER0 and put it in "cbd".
 */
    خالی
yank_cut_buffer0(Display *dpy, Clipboard_T *cbd)
{
    صحیح		nbytes = 0;
    char_u	*بفر = (char_u *)XFetch_بفر(dpy, &nbytes, 0);

    اگر (nbytes > 0)
    {
	صحیح  مکمل = FALSE;

	// CUT_BUFFER0 is supposed to be always latin1.  Convert to 'enc' when
	// using a multi-byte encoding.  Conversion between two 8-bit
	// character sets usually fails and the text might actually be in
	// 'enc' anyway.
	اگر (has_mbyte)
	{
	    char_u	*conv_buf;
	    vimconv_T	vc;

	    vc.vc_type = CONV_NONE;
	    اگر (convert_تیاری(&vc, (char_u *)"latin1", p_enc) == OK)
	    {
		conv_buf = string_convert(&vc, بفر, &nbytes);
		اگر (conv_buf != تہی)
		{
		    clip_yank_selection(MCHAR, conv_buf, (طویل)nbytes, cbd);
		    vim_free(conv_buf);
		    مکمل = TRUE;
		}
		convert_تیاری(&vc, تہی, تہی);
	    }
	}
	اگر (!مکمل)  // use the text without conversion
	    clip_yank_selection(MCHAR, بفر, (طویل)nbytes, cbd);
	XFree((خالی *)بفر);
	اگر (p_verbose > 0)
	{
	    verbose_enter();
	    verb_پیغام(_("Used CUT_BUFFER0 instead of empty selection"));
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
# اگر defined(FEAT_X11) && defined(FEAT_CLIPBOARD)
    خالی
x11_export_final_selection(خالی)
{
    Display	*dpy;
    char_u	*str = تہی;
    long_u	len = 0;
    صحیح		motion_type = -1;

#  ifdef FEAT_GUI
    اگر (gui.اندر_use)
	dpy = X_DISPLAY;
    ورنہ
#  endif
#  ifdef FEAT_XCLIPBOARD
	dpy = xterm_dpy;
#  ورنہ
	واپس;
#  endif

    // Get selection to export
    اگر (clip_plus.owned)
	motion_type = clip_convert_selection(&str, &len, &clip_plus);
    ورنہ اگر (clip_ستارہ.owned)
	motion_type = clip_convert_selection(&str, &len, &clip_ستارہ);

    // Check it's OK
    اگر (dpy != تہی && str != تہی && motion_type >= 0
					       && len < 1024*1024 && len > 0)
    {
	صحیح ok = TRUE;

	// The CUT_BUFFER0 is supposed to always contain latin1.  Convert from
	// 'enc' when it is a multi-byte encoding.  When 'enc' is an 8-bit
	// encoding conversion usually doesn't work, so keep the text as-is.
	اگر (has_mbyte)
	{
	    vimconv_T	vc;

	    vc.vc_type = CONV_NONE;
	    اگر (convert_تیاری(&vc, p_enc, (char_u *)"latin1") == OK)
	    {
		صحیح	intlen = len;
		char_u	*conv_str;

		vc.vc_fail = TRUE;
		conv_str = string_convert(&vc, str, &intlen);
		len = intlen;
		اگر (conv_str != تہی)
		{
		    vim_free(str);
		    str = conv_str;
		}
		ورنہ
		{
		    ok = FALSE;
		}
		convert_تیاری(&vc, تہی, تہی);
	    }
	    ورنہ
	    {
		ok = FALSE;
	    }
	}

	// Do not store the string if conversion failed.  Better to use any
	// other selection than garbled text.
	اگر (ok)
	{
	    XStore_بفر(dpy, (حرف *)str, (صحیح)len, 0);
	    XFlush(dpy);
	}
    }

    vim_free(str);
}
# endif

    خالی
clip_free_selection(Clipboard_T *cbd)
{
    yankreg_T *y_ptr = get_y_current();

    اگر (cbd == &clip_plus)
	سیٹ_y_current(get_y_رجسٹر(PLUS_رجسٹر));
    ورنہ
	سیٹ_y_current(get_y_رجسٹر(ستارہ_رجسٹر));
    free_yank_all();
    get_y_current()->y_سائز = 0;
    سیٹ_y_current(y_ptr);
}

/*
 * Get the selected text and put it in register '*' or '+'.
 */
    خالی
clip_get_selection(Clipboard_T *cbd)
{
    yankreg_T	*old_y_پچھلا, *old_y_current;
    pos_T	old_cursor;
    pos_T	old_visual;
    صحیح		old_visual_منوال;
    colnr_T	old_virtcol;
    colnr_T	old_curswant;
    صحیح		old_سیٹ_curswant;
    pos_T	old_op_آغاز, old_op_انتہا;
    oparg_T	oa;
    cmdarg_T	ca;

    اگر (cbd->owned)
    {
	اگر ((cbd == &clip_plus
		&& get_y_رجسٹر(PLUS_رجسٹر)->y_array != تہی)
		|| (cbd == &clip_ستارہ
		    && get_y_رجسٹر(ستارہ_رجسٹر)->y_array != تہی))
	    واپس;

	// Avoid triggering autocmds such as TextYankPost.
	بلاک_autocmds();

	// Get the text between clip_star.start & clip_star.end
	old_y_پچھلا = get_y_پچھلا();
	old_y_current = get_y_current();
	old_cursor = curwin->w_cursor;
	old_virtcol = curwin->w_virtcol;
	old_curswant = curwin->w_curswant;
	old_سیٹ_curswant = curwin->w_سیٹ_curswant;
	old_op_آغاز = curbuf->b_op_آغاز;
	old_op_انتہا = curbuf->b_op_انتہا;
	old_visual = VIsual;
	old_visual_منوال = VIsual_منوال;
	clear_oparg(&oa);
	oa.regname = (cbd == &clip_plus ? '+' : '*');
	oa.op_type = OP_YANK;
	CLEAR_میدان(ca);
	ca.oap = &oa;
	ca.cmdchar = 'y';
	ca.count1 = 1;
	ca.retval = CA_NO_ADJ_OP_انتہا;
	do_pending_operator(&ca, 0, TRUE);

	// restore things
	سیٹ_y_پچھلا(old_y_پچھلا);
	سیٹ_y_current(old_y_current);
	curwin->w_cursor = old_cursor;
	curwin->w_virtcol = old_virtcol;
	بدلا_cline_bef_curs();   // old w_virtcol et al. may be invalid
	curwin->w_curswant = old_curswant;
	curwin->w_سیٹ_curswant = old_سیٹ_curswant;
	curbuf->b_op_آغاز = old_op_آغاز;
	curbuf->b_op_انتہا = old_op_انتہا;
	VIsual = old_visual;
	VIsual_منوال = old_visual_منوال;

	unblock_autocmds();
    }
    ورنہ اگر (!is_clipboard_needs_update())
    {
	clip_free_selection(cbd);

	// Try to get selected text from another window
	clip_gen_request_selection(cbd);
    }
}

/*
 * Convert from the GUI selection string into the '*'/'+' register.
 */
    خالی
clip_yank_selection(
    صحیح		type,
    char_u	*str,
    طویل	len,
    Clipboard_T *cbd)
{
    yankreg_T *y_ptr;

    اگر (cbd == &clip_plus)
	y_ptr = get_y_رجسٹر(PLUS_رجسٹر);
    ورنہ
	y_ptr = get_y_رجسٹر(ستارہ_رجسٹر);

    clip_free_selection(cbd);

    str_مقصد_reg(y_ptr, type, str, len, -1, FALSE);
}

    صحیح
clip_convert_selection_انحراف(
	char_u	    **str,
	long_u	    *len,
	صحیح	    انحراف, // Extra space to add in *str and the offset to
			    // place the actual string in *str.
	Clipboard_T *cbd)
{
    char_u	*p;
    صحیح		lnum;
    صحیح		i, j;
    int_u	eolsize;
    yankreg_T	*y_ptr;

    اگر (cbd == &clip_plus)
	y_ptr = get_y_رجسٹر(PLUS_رجسٹر);
    ورنہ
	y_ptr = get_y_رجسٹر(ستارہ_رجسٹر);

# ifdef USE_CRNL
    eolsize = 2;
# ورنہ
    eolsize = 1;
# endif

    *str = تہی;
    *len = 0;
    اگر (y_ptr->y_array == تہی)
	واپس -1;

    برائے (i = 0; i < y_ptr->y_سائز; i++)
	*len += (long_u)y_ptr->y_array[i].لمبائی + eolsize;

    // Don't want newline character at end of last line if we're in MCHAR mode.
    اگر (y_ptr->y_type == MCHAR && *len >= eolsize)
	*len -= eolsize;

    *len += انحراف;
    p = *str = alloc(*len + 1);	// add one to avoid zero
    اگر (p == تہی)
	واپس -1;
    p += انحراف;
    lnum = 0;
    برائے (i = 0, j = 0; i < (صحیح)*len - انحراف; i++, j++)
    {
	اگر (y_ptr->y_array[lnum].string[j] == '\n')
	    p[i] = NUL;
	ورنہ اگر (y_ptr->y_array[lnum].string[j] == NUL)
	{
# ifdef USE_CRNL
	    p[i++] = '\r';
# endif
	    p[i] = '\n';
	    lnum++;
	    j = -1;
	}
	ورنہ
	    p[i] = y_ptr->y_array[lnum].string[j];
    }
    واپس y_ptr->y_type;
}

/*
 * Convert the '*'/'+' register into a GUI selection string returned in *str
 * with length *len.
 * Returns the motion type, or -1 for failure.
 */
    صحیح
clip_convert_selection(char_u **str, long_u *len, Clipboard_T *cbd)
{
    واپس clip_convert_selection_انحراف(str, len, 0, cbd);
}

/*
 * When "regname" is a clipboard register, obtain the selection.  If it's not
 * available return zero, otherwise return "regname".
 */
    صحیح
may_get_selection(صحیح regname)
{
    اگر (regname == '*')
    {
	اگر (!clip_ستارہ.دستیاب)
	    regname = 0;
	ورنہ
	    clip_get_selection(&clip_ستارہ);
    }
    ورنہ اگر (regname == '+')
    {
	اگر (!clip_plus.دستیاب)
	    regname = 0;
	ورنہ
	    clip_get_selection(&clip_plus);
    }
    واپس regname;
}

/*
 * If we have written to a clipboard register, send the text to the clipboard.
 */
    خالی
may_سیٹ_selection(خالی)
{
    اگر ((get_y_current() == get_y_رجسٹر(ستارہ_رجسٹر))
	    && clip_ستارہ.دستیاب)
    {
	clip_own_selection(&clip_ستارہ);
	clip_gen_سیٹ_selection(&clip_ستارہ);
    }
    ورنہ اگر ((get_y_current() == get_y_رجسٹر(PLUS_رجسٹر))
	    && clip_plus.دستیاب)
    {
	clip_own_selection(&clip_plus);
	clip_gen_سیٹ_selection(&clip_plus);
    }
}

# اگر defined(FEAT_WAYLAND_CLIPBOARD)

    ساکن clip_wl_selection_T *
clip_wl_get_selection(wayland_selection_T sel)
{
    انتخاب (sel)
    {
	حالت WAYLAND_SELECTION_REGULAR:
	    واپس &clip_wl.regular;
	حالت WAYLAND_SELECTION_PRIMARY:
	    واپس &clip_wl.primary;
	طےشدہ:
	    واپس تہی;
    }
}

    ساکن clip_wl_selection_T *
clip_wl_get_selection_from_cbd(Clipboard_T *cbd)
{
    اگر (cbd == &clip_plus)
	واپس &clip_wl.regular;
    ورنہ اگر (cbd == &clip_ستارہ)
	واپس &clip_wl.primary;
    ورنہ
	واپس تہی;
}

    ساکن Clipboard_T *
clip_wl_get_cbd_from_selection(clip_wl_selection_T *sel)
{
    اگر (sel == &clip_wl.regular)
	واپس &clip_plus;
    ورنہ اگر (sel == &clip_wl.primary)
	واپس &clip_ستارہ;
    ورنہ
	واپس تہی;
}

    ساکن wayland_selection_T
clip_wl_get_selection_type(clip_wl_selection_T *sel)
{
    اگر (sel == &clip_wl.regular)
	واپس WAYLAND_SELECTION_REGULAR;
    ورنہ اگر (sel == &clip_wl.primary)
	واپس WAYLAND_SELECTION_PRIMARY;
    ورنہ
	واپس WAYLAND_SELECTION_NONE;
}

    ساکن bool
wl_ڈیٹا_offer_listener_event_offer(
    خالی *ڈیٹا غیر_مستعمل,
    vwl_ڈیٹا_offer_T *offer غیر_مستعمل,
    ثابت حرف *mime_type
)
{
    // Only accept mime type if we support it
    برائے (صحیح i = 0; i < (صحیح)ARRAY_لمبائی(supported_mimes); i++)
	اگر (STRCMP(mime_type, supported_mimes[i]) == 0)
	    واپس true;
    واپس FALSE;
}

ساکن ثابت vwl_ڈیٹا_offer_listener_T vwl_ڈیٹا_offer_listener = {
    .offer = wl_ڈیٹا_offer_listener_event_offer
};

    ساکن خالی
vwl_ڈیٹا_device_listener_event_ڈیٹا_offer(
	خالی *ڈیٹا غیر_مستعمل,
	vwl_ڈیٹا_device_T *device غیر_مستعمل,
	vwl_ڈیٹا_offer_T *offer)
{
    // Immediately start listening for offer events from the data offer
    vwl_ڈیٹا_offer_جوڑو_listener(offer, &vwl_ڈیٹا_offer_listener, تہی);
}

    ساکن خالی
vwl_ڈیٹا_device_listener_event_selection(
	خالی *ڈیٹا غیر_مستعمل,
	vwl_ڈیٹا_device_T *device غیر_مستعمل,
	vwl_ڈیٹا_offer_T *offer,
	wayland_selection_T selection)
{
    clip_wl_selection_T *sel = clip_wl_get_selection(selection);

    // Destroy previous offer if any, it is now invalid
    vwl_ڈیٹا_offer_destroy(sel->offer);

    // There are two cases when sel->offer is NULL
    // 1. No one owns the selection
    // 2. We own the selection (we'll just access the register directly)
    اگر (offer == تہی || sel->ماخذ != تہی)
    {
	// Selection event is from us, so we are the source client. Therefore
	// ignore it. Or the selection is cleared, so set sel->offer to NULL
	vwl_ڈیٹا_offer_destroy(offer);
	sel->offer = تہی;
	واپس;
    }

    // Save offer. When we want to request data, then we'll actually call the
    // receive method.
    sel->offer = offer;

}

    ساکن خالی
vwl_ڈیٹا_device_listener_event_ختم_شدہ(
	خالی *ڈیٹا غیر_مستعمل,
	vwl_ڈیٹا_device_T *device)
{
    clip_wl_selection_T *sel;
    // Device finished, guessing this can happen is when the seat becomes
    // invalid? If so, let the user call :wlrestore! to reset. There wouldn't be
    // any point in trying to create another data device for the same seat,
    // since the seat is in an invalid state.
    اگر (device == clip_wl.regular.device)
    {
	sel = &clip_wl.regular;
	clip_wl.regular.device = تہی;
    }
    ورنہ اگر (device == clip_wl.primary.device)
    {
	sel = &clip_wl.primary;
	clip_wl.primary.device = تہی;
    }
    ورنہ
	// Shouldn't happen
	واپس;

    vim_free(sel->contents);
    vwl_ڈیٹا_ماخذ_destroy(sel->ماخذ);
    vwl_ڈیٹا_offer_destroy(sel->offer);
    sel->دستیاب = FALSE;

    vwl_ڈیٹا_device_destroy(device);
}

ساکن ثابت vwl_ڈیٹا_device_listener_T vwl_ڈیٹا_device_listener = {
    .ڈیٹا_offer = vwl_ڈیٹا_device_listener_event_ڈیٹا_offer,
    .selection = vwl_ڈیٹا_device_listener_event_selection,
    .ختم_شدہ = vwl_ڈیٹا_device_listener_event_ختم_شدہ
};

/*
 * Initialize the clipboard for Wayland using the global Wayland connection.
 * Returns OK on success and FAIL on failure.
 */
    صحیح
clip_init_wayland(خالی)
{
    int_u supported = WAYLAND_SELECTION_NONE;

    اگر (wayland_ct == تہی)
	واپس FAIL;

    memset(&clip_wl, 0, حجم(clip_wl));

    clip_wl.seat = vwl_connection_get_seat(wayland_ct, (حرف *)p_wse);

    اگر (clip_wl.seat == تہی)
	واپس FAIL;

    clip_wl.regular.manager = vwl_connection_get_ڈیٹا_device_manager(
	    wayland_ct, WAYLAND_SELECTION_REGULAR, &supported);

    اگر (clip_wl.regular.manager != تہی)
    {
	clip_wl.regular.device = vwl_ڈیٹا_device_manager_get_ڈیٹا_device(
		clip_wl.regular.manager, clip_wl.seat);

	اگر (clip_wl.regular.device != تہی)
	    clip_wl.regular.دستیاب = true;
	ورنہ
	{
	    // Shouldn't happen
	    vwl_ڈیٹا_device_manager_discard(clip_wl.regular.manager);
	    clip_wl.regular.manager = تہی;
	    واپس FAIL;
	}
    }

    // If we still don't support the primary selection, try finding one for it
    // specifically.
    اگر (!(supported & WAYLAND_SELECTION_PRIMARY))
    {
	clip_wl.primary.manager = vwl_connection_get_ڈیٹا_device_manager(
		wayland_ct, WAYLAND_SELECTION_PRIMARY, &supported);

	اگر (clip_wl.primary.manager != تہی)
	{
	    clip_wl.primary.device = vwl_ڈیٹا_device_manager_get_ڈیٹا_device(
		    clip_wl.primary.manager, clip_wl.seat);

	    اگر (clip_wl.primary.device != تہی)
		clip_wl.primary.دستیاب = true;
	    ورنہ
	    {
		vwl_ڈیٹا_device_manager_discard(clip_wl.primary.manager);
		clip_wl.primary.manager = تہی;
	    }
	}
    }

    اگر (clip_wl.regular.دستیاب && !clip_wl.primary.دستیاب)
    {
	// The protocol supports both regular and primary selections, just use
	// one data device manager and one data device. Or the primary selection
	// is not supported, make it point to the regular selection instead.
	clip_wl.primary.دستیاب = true;
	clip_wl.primary.manager = clip_wl.regular.manager;
	clip_wl.primary.device = clip_wl.regular.device;
    }

    اگر (!clip_wl.regular.دستیاب && !clip_wl.primary.دستیاب)
	واپس FAIL;

    // Start listening for selection updates
    اگر (clip_wl.regular.device != تہی)
	vwl_ڈیٹا_device_جوڑو_listener(clip_wl.regular.device,
		&vwl_ڈیٹا_device_listener, تہی);
    // Don't want to listen to the same data device twice
    اگر (clip_wl.primary.device != تہی
	    && clip_wl.primary.device != clip_wl.regular.device)
	vwl_ڈیٹا_device_جوڑو_listener(clip_wl.primary.device,
		&vwl_ڈیٹا_device_listener, تہی);

    واپس OK;
}

    خالی
clip_uninit_wayland(خالی)
{
    clip_wl_selection_T *sel;

    اگر (clipmethod == CLIPMETHOD_WAYLAND)
    {
	اگر (clip_ستارہ.owned)
	    clip_lose_selection(&clip_ستارہ);
	اگر (clip_plus.owned)
	    clip_lose_selection(&clip_plus);
    }

    // Don't want to double free
    اگر (clip_wl.regular.manager != clip_wl.primary.manager)
	vwl_ڈیٹا_device_manager_discard(clip_wl.primary.manager);
    vwl_ڈیٹا_device_manager_discard(clip_wl.regular.manager);

    اگر (clip_wl.regular.device != clip_wl.primary.device)
	vwl_ڈیٹا_device_destroy(clip_wl.primary.device);
    vwl_ڈیٹا_device_destroy(clip_wl.regular.device);

    sel = &clip_wl.regular;
    جب (true)
    {
	vim_free(sel->contents);
	vwl_ڈیٹا_ماخذ_destroy(sel->ماخذ);
	vwl_ڈیٹا_offer_destroy(sel->offer);
	sel->دستیاب = false;

	اگر (sel == &clip_wl.primary)
	    توڑو;
	sel = &clip_wl.primary;
    }

    vim_memset(&clip_wl, 0, حجم(clip_wl));
}

    صحیح
clip_reset_wayland(خالی)
{
    wayland_uninit_connection();

    اگر (wayland_init_connection(wayland_display_name) == FAIL
	    || clip_init_wayland() == FAIL)
	واپس FAIL;

    انتخاب_clipmethod();
    واپس OK;
}


/*
 * Read data from a file descriptor and write it to the given clipboard.
 */
    ساکن خالی
clip_wl_receive_ڈیٹا(Clipboard_T *cbd, ثابت حرف *mime_type, صحیح fd)
{
    char_u	*آغاز, *final;
    طویل	len;
    garray_T	buf;
    صحیح		motion_type = MAUTO;
    ssize_t	r = 0;
    char_u	*tofree = تہی;
#  ifndef HAVE_SELECT
    ساخت pollfd   pfd;

    pfd.fd = fd;
    pfd.events = POLLIN;
#  ورنہ
    fd_set rfds;
    ساخت timeval  tv;

    FD_ZERO(&rfds);
    FD_SET(fd, &rfds);
#  endif

    // Make pipe (read end) non-blocking
    اگر (fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK) == -1)
	واپس;

    ga_init2(&buf, 1, 4096);

    // 4096 bytes seems reasonable for initial buffer size, memory is cheap
    // anyways.
    اگر (ga_grow(&buf, 4096) == FAIL)
	واپس;

    آغاز = buf.ga_ڈیٹا;

#  ifndef HAVE_SELECT
    جب (poll(&pfd, 1, p_wtm) > 0)
#  ورنہ
    جب (tv.tv_sec = p_wtm / 1000, tv.tv_usec = (p_wtm % 1000) * 1000,
	    select(fd + 1, &rfds, تہی, تہی, &tv) > 0)
#  endif
    {
	r = اقرأ(fd, آغاز, buf.ga_maxlen - 1 - buf.ga_len);

	اگر (r == 0)
	    توڑو;
	ورنہ اگر (r < 0)
	{
	    اگر (errno == EAGAIN || errno == EINTR)
		جاری;
	    توڑو;
	}

	آغاز += r;
	buf.ga_len += r;

	// Realloc if we are at the end of the buffer
	اگر (buf.ga_len >= buf.ga_maxlen - 1)
	{
	    اگر (ga_grow(&buf, 8192) == FAIL)
		توڑو;
	    آغاز = (char_u *)buf.ga_ڈیٹا + buf.ga_len;
	}
    }

    اگر (buf.ga_len == 0)
    {
	clip_free_selection(cbd); // Nothing received, clear register
	ga_clear(&buf);
	واپس;
    }

    final = buf.ga_ڈیٹا;
    len = buf.ga_len;

    اگر (clip_convert_ڈیٹا(&final, &len, &motion_type,
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
    ساکن خالی
clip_wl_request_selection(Clipboard_T *cbd)
{
    clip_wl_selection_T *sel = clip_wl_get_selection_from_cbd(cbd);
    صحیح			fds[2];
    صحیح			mime_types_len;
    ثابت حرف		**mime_types;
    ثابت حرف		*chosen_mime = تہی;

    اگر (!sel->دستیاب)
	جاؤ clear;

    // Dispatch any events that still queued up before checking for a data
    // offer.
    اگر (vwl_connection_roundtrip(wayland_ct) == FAIL)
	جاؤ clear;

    اگر (sel->offer == تہی)
	جاؤ clear;

    mime_types_len = sel->offer->mime_types.ga_len;
    mime_types = sel->offer->mime_types.ga_ڈیٹا;

    // Choose mime type to receive from. Mime types with a lower index in the
    // "supported_mimes" array are prioritized over ones after it.
    برائے (صحیح i = 0; i < (صحیح)ARRAY_لمبائی(supported_mimes)
	    && chosen_mime == تہی; i++)
    {
	برائے (صحیح k = 0; k < mime_types_len && chosen_mime == تہی; k++)
	    اگر (STRCMP(mime_types[k], supported_mimes[i]) == 0)
		chosen_mime = supported_mimes[i];
    }

    اگر (chosen_mime == تہی || pipe(fds) == -1)
	جاؤ clear;

    vwl_ڈیٹا_offer_receive(sel->offer, chosen_mime, fds[1]);

    close(fds[1]); // Close before we read data so that when the source client
		   // closes their end we receive an EOF.

    اگر (vwl_connection_flush(wayland_ct) >= 0)
	clip_wl_receive_ڈیٹا(cbd, chosen_mime, fds[0]);

    close(fds[0]);

    واپس;
clear:
    clip_free_selection(cbd);
}

    ساکن خالی
vwl_ڈیٹا_ماخذ_listener_event_send(
    خالی *ڈیٹا,
    vwl_ڈیٹا_ماخذ_T *ماخذ غیر_مستعمل,
    ثابت حرف *mime_type,
    int32_t fd
)
{
    clip_wl_selection_T *sel = ڈیٹا;
    Clipboard_T		*cbd = clip_wl_get_cbd_from_selection(sel);
    bool		have_mime = false;
    صحیح			motion_type;
    long_u		لمبائی;
    char_u		*string; // Will be reallocated to a bigger size if
				 // needed.
    صحیح			انحراف = 0;
    bool		is_vim, is_vimenc;
    size_t		کل = 0;
#  ifndef HAVE_SELECT
    ساخت pollfd   pfd;

    pfd.fd = fd;
    pfd.events = POLLOUT;
#  ورنہ
    fd_set	    wfds;
    ساخت timeval  tv;

    FD_ZERO(&wfds);
    FD_SET(fd, &wfds);
#  endif

    // Check if we actually have mime type
    برائے (صحیح i = 0; i < (صحیح)ARRAY_لمبائی(supported_mimes); i++)
	اگر (STRCMP(supported_mimes[i], mime_type) == 0)
	{
	    have_mime = true;
	    توڑو;
	}

    اگر (!have_mime)
	جاؤ باہر_نکلو;

    // First byte sent is motion type for vim specific formats. For the vimenc
    // format, after the first byte is the encoding type, which is null
    // terminated.

    is_vimenc = STRCMP(mime_type, VIMENC_ATOM_NAME) == 0
	|| STRCMP(mime_type, VIMENC_MIMETYPE_NAME) == 0;
    is_vim = STRCMP(mime_type, VIM_ATOM_NAME) == 0
	|| STRCMP(mime_type, VIM_MIMETYPE_NAME) == 0;

    اگر (is_vimenc)
	انحراف += 2 + STRLEN(p_enc);
    ورنہ اگر (is_vim)
	انحراف += 1;

    clip_get_selection(cbd);
    motion_type = clip_convert_selection_انحراف(&string, &لمبائی, انحراف, cbd);

    اگر (motion_type < 0)
	جاؤ باہر_نکلو;

    اگر (is_vimenc)
    {
	string[0] = (char_u)motion_type;
	// Use vim_strncpy for safer copying
	vim_strncpy(string + 1, p_enc, STRLEN(p_enc));
    }
    ورنہ اگر (is_vim)
	string[0] = (char_u)motion_type;


    جب (کل < (size_t)لمبائی &&
#  ifndef HAVE_SELECT
	    poll(&pfd, 1, p_wtm) > 0)
#  ورنہ
	    ((tv.tv_sec = p_wtm / 1000, tv.tv_usec = (p_wtm % 1000) * 1000),
	    select(fd + 1, تہی, &wfds, تہی, &tv) > 0))
#  endif
    {
	ssize_t w = اكتب(fd, string + کل, لمبائی - کل);

	اگر (w == -1)
	    توڑو;
	کل += w;
    }

    vim_free(string);
باہر_نکلو:
    close(fd);
}

    ساکن خالی
vwl_ڈیٹا_ماخذ_listener_event_cancelled(
	خالی *ڈیٹا,
	vwl_ڈیٹا_ماخذ_T *ماخذ غیر_مستعمل)
{
    clip_wl_selection_T *sel = ڈیٹا;
    Clipboard_T		*cbd = clip_wl_get_cbd_from_selection(sel);

    clip_lose_selection(cbd);
}

ساکن ثابت vwl_ڈیٹا_ماخذ_listener_T vwl_ڈیٹا_ماخذ_listener = {
    .send = vwl_ڈیٹا_ماخذ_listener_event_send,
    .cancelled = vwl_ڈیٹا_ماخذ_listener_event_cancelled
};

    ساکن خالی
clip_wl_do_سیٹ_selection(خالی *ڈیٹا, uint32_t serial)
{
    clip_wl_selection_T *sel = ڈیٹا;
    wayland_selection_T sel_type = clip_wl_get_selection_type(sel);

    vwl_ڈیٹا_device_سیٹ_selection(sel->device, sel->ماخذ, serial, sel_type);

    sel->own_success = (vwl_connection_roundtrip(wayland_ct) == OK);
}

/*
 * Own the selection that cbd corresponds to. Start listening for requests from
 * other Wayland clients so they can receive data from us. Returns OK on success
 * and FAIL on failure.
 */
    ساکن صحیح
clip_wl_own_selection(Clipboard_T *cbd)
{
    clip_wl_selection_T *sel = clip_wl_get_selection_from_cbd(cbd);
    wayland_selection_T sel_type = clip_wl_get_selection_type(sel);

    اگر (!sel->دستیاب || vwl_connection_roundtrip(wayland_ct) == FAIL)
	واپس FAIL;

    اگر (sel->ماخذ != تہی)
    {
	اگر (sel_type == WAYLAND_SELECTION_PRIMARY)
	    // We already own the selection, ignore (only do this for primary
	    // selection). We don't re set the selection because then we would
	    // be setting the selection every time the user moves the visual
	    // selection cursor, which is messy and inefficient. Some
	    // applications like Google Chrome do it this way however.
	    واپس OK;
	ورنہ اگر (sel_type == WAYLAND_SELECTION_REGULAR)
	{
	    // Technically we don't need to do this as we already own the
	    // selection, however if a user yanks text a second time, the
	    // text yanked won't appear in their clipboard manager if they are
	    // using one.
	    //
	    // This can be unexpected behaviour for the user so its probably
	    // better to do it this way. Additionally other Wayland applications
	    // seem to set the selection every time.
	    vwl_ڈیٹا_ماخذ_destroy(sel->ماخذ);
	}
	ورنہ
	    // Shouldn't happen
	    واپس FAIL;
    }

    sel->ماخذ = vwl_ڈیٹا_device_manager_create_ڈیٹا_ماخذ(sel->manager);
    vwl_ڈیٹا_ماخذ_جوڑو_listener(sel->ماخذ, &vwl_ڈیٹا_ماخذ_listener, sel);

    // Advertise mime types
    برائے (صحیح i = 0; i < (صحیح)ARRAY_لمبائی(supported_mimes); i++)
	vwl_ڈیٹا_ماخذ_offer(sel->ماخذ, supported_mimes[i]);

    sel->own_success = false;
    clip_wl_do_سیٹ_selection(sel, 0);

    اگر (!sel->own_success)
	جاؤ fail;

    واپس OK;
fail:
    vwl_ڈیٹا_ماخذ_destroy(sel->ماخذ);
    sel->ماخذ = تہی;
    واپس FAIL;
}

/*
 * Disown the selection that cbd corresponds to.
 */
    ساکن خالی
clip_wl_lose_selection(Clipboard_T *cbd)
{
    clip_wl_selection_T *sel = clip_wl_get_selection_from_cbd(cbd);

    اگر (!sel->دستیاب)
	واپس;

    vwl_ڈیٹا_ماخذ_destroy(sel->ماخذ);
    sel->ماخذ = تہی;
}

/*
 * Send the current selection to the clipboard. Do nothing for Wayland because
 * we will fill in the selection only when requested by another client.
 */
    ساکن خالی
clip_wl_سیٹ_selection(Clipboard_T *cbd غیر_مستعمل)
{
}

#  اگر defined(USE_SYSTEM)
/*
 * Return true if we own the selection corresponding to cbd or another client
 * does.
 */
    ساکن bool
clip_wl_owner_exists(Clipboard_T *cbd)
{
    clip_wl_selection_T *sel = clip_wl_get_selection_from_cbd(cbd);

    اگر (vwl_connection_roundtrip(wayland_ct) == FAIL)
	واپس false;

    واپس sel->دستیاب && (sel->ماخذ != تہی || sel->offer != تہی);
}
#  endif

# endif // FEAT_WAYLAND_CLIPBOARD

#اگر_ختم // FEAT_CLIPBOARD

#اگر_تعریف_شدہ HAVE_CLIPMETHOD

/*
 * Returns the first method for accessing the clipboard that is available/works,
 * depending on the order of values in str.
 */
    ساکن clipmethod_T
get_clipmethod(char_u *str)
{
    صحیح		len	= (صحیح)STRLEN(str) + 1;
    char_u	*buf	= alloc(len);

    اگر (buf == تہی)
	واپس CLIPMETHOD_FAIL;

    clipmethod_T ret = CLIPMETHOD_FAIL;
    char_u	*p = str;

    جب (*p != NUL)
    {
	clipmethod_T طریقہ = CLIPMETHOD_NONE;

	(خالی)copy_option_part(&p, buf, len, ",");

	اگر (STRCMP(buf, "wayland") == 0)
	{
# ifdef FEAT_GUI
	    اگر (!gui.اندر_use)
# endif
	    {
# ifdef FEAT_WAYLAND_CLIPBOARD
		اگر (clip_wl.regular.دستیاب || clip_wl.primary.دستیاب)
		    طریقہ = CLIPMETHOD_WAYLAND;
# endif
	    }
	}
	ورنہ اگر (STRCMP(buf, "x11") == 0)
	{
# ifdef FEAT_GUI
	    اگر (!gui.اندر_use)
# endif
	    {
# ifdef FEAT_XCLIPBOARD
		// x_IOerror_handler() in os_unix.c should set xterm_dpy to NULL if
		// we lost connection to the X server.
		اگر (xterm_dpy != تہی)
		{
		    // If the X connection is lost then that handler will longjmp
		    // somewhere else, in that case we will call choose_clipmethod()
		    // again from there, and this if block won't be executed since
		    // xterm_dpy will be set to NULL.
		    xterm_update();
		    طریقہ = CLIPMETHOD_X11;
		}
# endif
	    }
	}
	ورنہ
	{
# ifdef FEAT_CLIPBOARD_PROVIDER
	    // Check if name matches a clipboard provider
	    صحیح r = clip_provider_is_دستیاب(buf);

	    اگر (r == 1)
	    {
		طریقہ = CLIPMETHOD_PROVIDER;
		اگر (ret == CLIPMETHOD_FAIL)
		{
		    vim_free(clip_provider);
		    clip_provider = vim_strsave(buf);
		    اگر (clip_provider == تہی)
			جاؤ fail;
		}
	    }
	    ورنہ اگر (r == -1)
# endif
	    {
# ifdef FEAT_CLIPBOARD_PROVIDER
fail:
# endif
		ret = CLIPMETHOD_FAIL;
		جاؤ باہر_نکلو;
	    }
	}

	// Keep on going in order to catch errors
	اگر (طریقہ != CLIPMETHOD_NONE && ret == CLIPMETHOD_FAIL)
	    ret = طریقہ;
    }

    // No match found, use "none".
    ret = (ret == CLIPMETHOD_FAIL) ? CLIPMETHOD_NONE : ret;

باہر_نکلو:
    vim_free(buf);
    واپس ret;
}


/*
 * Returns name of clipmethod in a statically allocated string.
 */
    ساکن char_u *
clipmethod_مقصد_str(clipmethod_T طریقہ)
{
    انتخاب(طریقہ)
    {
	حالت CLIPMETHOD_WAYLAND:
	    واپس (char_u *)"wayland";
	حالت CLIPMETHOD_X11:
	    واپس (char_u *)"x11";
	حالت CLIPMETHOD_PROVIDER:
# ifdef FEAT_EVAL
	    واپس clip_provider;
# endif
	طےشدہ:
	    واپس (char_u *)"none";
    }
}

/*
 * Sets the current clipmethod to use given by `get_clipmethod()`. Returns an
 * error message on failure else NULL.
 */
    حرف *
انتخاب_clipmethod(خالی)
{
    clipmethod_T طریقہ = get_clipmethod(p_cpm);

    اگر (طریقہ == CLIPMETHOD_FAIL)
	واپس e_invalid_دلیل;

// If GUI is running or we are not on a system with Wayland or X11, then always
// return CLIPMETHOD_NONE. System or GUI clipboard handling always overrides.
// This is unless a provider is being used.
# اگر defined(FEAT_XCLIPBOARD) || defined(FEAT_WAYLAND_CLIPBOARD)
#  اگر defined(FEAT_GUI)
    اگر (طریقہ != CLIPMETHOD_PROVIDER && gui.اندر_use)
    {
#   ifdef FEAT_WAYLAND
	// We only interact with Wayland for the clipboard, we can just deinit
	// everything.
	wayland_uninit_connection();
#   endif

	طریقہ = CLIPMETHOD_NONE;
	جاؤ lose_sel_exit;
    }
#  endif
# ورنہ
    // If on a system like windows or macos, then clipmethod is irrelevant, we
    // use their way of accessing the clipboard. This is unless we are using the
    // clipboard provider
#  ifdef FEAT_CLIPBOARD_PROVIDER
    اگر (طریقہ != CLIPMETHOD_PROVIDER)
#  endif
    {
	طریقہ = CLIPMETHOD_NONE;
	جاؤ باہر_نکلو;
    }
# endif

# ifdef FEAT_CLIPBOARD
    // Deinitialize clipboard if there is no way to access clipboard
    اگر (طریقہ == CLIPMETHOD_NONE)
	clip_init(FALSE);
    // If we have a clipmethod that works now, then initialize clipboard
    ورنہ اگر (clipmethod == CLIPMETHOD_NONE && طریقہ != CLIPMETHOD_NONE)
    {
	clip_init(TRUE);
	did_warn_clipboard = false;
    }
    // Disown clipboard if we are switching to a new method
    ورنہ اگر (clipmethod != CLIPMETHOD_NONE && طریقہ != clipmethod)
    {
#  اگر (defined(FEAT_XCLIPBOARD) || defined(FEAT_WAYLAND_CLIPBOARD)) \
	&& defined(FEAT_GUI)
lose_sel_exit:
#  endif
	اگر (clip_ستارہ.owned)
	    clip_lose_selection(&clip_ستارہ);
	اگر (clip_plus.owned)
	    clip_lose_selection(&clip_plus);

#  اگر defined(FEAT_GUI)
	اگر (!gui.اندر_use)
#  endif
	{
	    clip_init(TRUE);
	    did_warn_clipboard = false;
	}
    }
# endif // FEAT_CLIPBOARD

# اگر !defined(FEAT_XCLIPBOARD) && !defined(FEAT_WAYLAND_CLIPBOARD)
باہر_نکلو:
# endif

    clipmethod = طریقہ;

# ifdef FEAT_EVAL
    سیٹ_vim_var_string(VV_CLIPMETHOD, (char_u*)clipmethod_مقصد_str(طریقہ), -1);
# endif

    واپس تہی;
}

/*
 * Call choose_clipmethod().
 */
    خالی
ex_clipreset(exarg_T *eap غیر_مستعمل)
{
    clipmethod_T پچھلا = clipmethod;

    انتخاب_clipmethod();

    اگر (clipmethod == CLIPMETHOD_NONE)
	smsg(_("Could not find a way to access the clipboard."));
    ورنہ اگر (clipmethod != پچھلا)
	smsg(_("Switched to clipboard method '%s'."),
		clipmethod_مقصد_str(clipmethod));
}

/*
 * Adjust the register name pointed to with "rp" for the clipboard being
 * used always and the clipboard being available.
 */
    خالی
adjust_clip_reg(صحیح *rp)
{
# ifdef FEAT_CLIPBOARD_PROVIDER
    اگر (clipmethod == CLIPMETHOD_PROVIDER)
    {
	اگر (*rp == 0 && clip_unnamed != 0)
	    *rp = ((clip_unnamed & CLIP_UNNAMED_PLUS)) ? '+' : '*';
	واپس;
    }
# endif
# ifdef FEAT_CLIPBOARD
    // If no reg. specified, and "unnamed" or "unnamedplus" is in 'clipboard',
    // use '*' or '+' reg, respectively. "unnamedplus" prevails.
    اگر (*rp == 0 && (clip_unnamed != 0 || clip_unnamed_saved != 0))
    {
	اگر (clip_unnamed != 0)
	    *rp = ((clip_unnamed & CLIP_UNNAMED_PLUS) && clip_plus.دستیاب)
								  ? '+' : '*';
	ورنہ
	    *rp = ((clip_unnamed_saved & CLIP_UNNAMED_PLUS)
					   && clip_plus.دستیاب) ? '+' : '*';
    }
    اگر ((!clip_ستارہ.دستیاب && *rp == '*') ||
	   (!clip_plus.دستیاب && *rp == '+'))
    {
	پیغام_warn_missing_clipboard();
	*rp = 0;
    }
# endif
}

/*
 * Extract the items in the 'clipboard' option and set global values.
 * Return an error message or NULL for success.
 */
    حرف *
did_سیٹ_clipboard(optset_T *دلائل غیر_مستعمل)
{
    صحیح		new_unnamed = 0;
# ifdef FEAT_CLIPBOARD
    صحیح		new_autoselect_ستارہ = FALSE;
    صحیح		new_autoselect_plus = FALSE;
    صحیح		new_autoselectml = FALSE;
    صحیح		new_html = FALSE;
# endif
    regprog_T	*new_exclude_prog = تہی;
    حرف	*errmsg = تہی;
    char_u	*p;

    برائے (p = p_cb; *p != NUL; )
    {
	// Note: Keep this in sync with p_cb_values.
	اگر (STRNCMP(p, "unnamed", 7) == 0 && (p[7] == ',' || p[7] == NUL))
	{
	    new_unnamed |= CLIP_UNNAMED;
	    p += 7;
	}
	ورنہ اگر (STRNCMP(p, "unnamedplus", 11) == 0
					    && (p[11] == ',' || p[11] == NUL))
	{
	    new_unnamed |= CLIP_UNNAMED_PLUS;
	    p += 11;
	}
# ifdef FEAT_CLIPBOARD
	ورنہ اگر (STRNCMP(p, "autoselect", 10) == 0
					    && (p[10] == ',' || p[10] == NUL))
	{
	    new_autoselect_ستارہ = TRUE;
	    p += 10;
	}
	ورنہ اگر (STRNCMP(p, "autoselectplus", 14) == 0
					    && (p[14] == ',' || p[14] == NUL))
	{
	    new_autoselect_plus = TRUE;
	    p += 14;
	}
	ورنہ اگر (STRNCMP(p, "autoselectml", 12) == 0
					    && (p[12] == ',' || p[12] == NUL))
	{
	    new_autoselectml = TRUE;
	    p += 12;
	}
	ورنہ اگر (STRNCMP(p, "html", 4) == 0 && (p[4] == ',' || p[4] == NUL))
	{
	    new_html = TRUE;
	    p += 4;
	}
	ورنہ اگر (STRNCMP(p, "exclude:", 8) == 0 && new_exclude_prog == تہی)
	{
	    p += 8;
	    new_exclude_prog = vim_regcomp(p, RE_MAGIC);
	    اگر (new_exclude_prog == تہی)
		errmsg = e_invalid_دلیل;
	    توڑو;
	}
# endif
	ورنہ
	{
	    errmsg = e_invalid_دلیل;
	    توڑو;
	}
	اگر (*p == ',')
	    ++p;
    }
    اگر (errmsg == تہی)
    {
# ifdef FEAT_CLIPBOARD
	اگر (global_busy)
	    // clip_unnamed will be reset to clip_unnamed_saved
	    // at end_global_changes
	    clip_unnamed_saved = new_unnamed;
	ورنہ
# endif
	    clip_unnamed = new_unnamed;
# ifdef FEAT_CLIPBOARD
	clip_autoselect_ستارہ = new_autoselect_ستارہ;
	clip_autoselect_plus = new_autoselect_plus;
	clip_autoselectml = new_autoselectml;
	clip_html = new_html;
	vim_regfree(clip_exclude_prog);
	clip_exclude_prog = new_exclude_prog;
# endif
# اگر defined(FEAT_GUI_GTK)
	اگر (gui.اندر_use)
	{
#  ifdef USE_GTK4
	    gui_gtk_update_selection_formats(&clip_plus);
	    gui_gtk_update_selection_formats(&clip_ستارہ);
#  ورنہ
	    gui_gtk_سیٹ_selection_targets((GdkAtom)GDK_SELECTION_PRIMARY);
	    gui_gtk_سیٹ_selection_targets((GdkAtom)clip_plus.gtk_sel_atom);
#  endif
#  ifdef FEAT_DND
	    gui_gtk_سیٹ_dnd_targets();
#  endif
	}
# endif
    }
    ورنہ
	vim_regfree(new_exclude_prog);

    واپس errmsg;
}

#اگر_ختم // HAVE_CLIPMETHOD

#اگر_تعریف_شدہ FEAT_CLIPBOARD_PROVIDER

/*
 * Check if a clipboard provider with given name is available. Returns 1 if available,
 * 0 if not available, and -1 on error
 */
    ساکن صحیح
clip_provider_is_دستیاب(char_u *provider)
{
    dict_T	*providers = get_vim_var_dict(VV_CLIPPROVIDERS);
    typval_T	provider_tv = {0};
    callback_T	callback = {0};
    typval_T	rettv = {0};
    typval_T	func_tv = {0};
    صحیح		res = 0;

    اگر (dict_get_tv(providers, (حرف *)provider, &provider_tv) == FAIL
	    || provider_tv.v_type != VAR_DICT)
	// clipboard provider not defined
	واپس -1;

    اگر (dict_get_tv(provider_tv.vval.v_dict, "available", &func_tv) == FAIL)
    {
	clear_tv(&provider_tv);
	// If "available" function not specified assume always TRUE
	واپس 1;
    }

    اگر ((callback = get_callback(&func_tv)).cb_name == تہی)
	جاؤ fail;

    اگر (call_callback(&callback, -1, &rettv, 0, تہی) == FAIL ||
	    (rettv.v_type != VAR_BOOL && rettv.v_type != VAR_نمبر))
	جاؤ fail;

    اگر (rettv.vval.v_نمبر)
	res = 1;

    اگر (FALSE)
fail:
	res = -1;

    free_callback(&callback);
    clear_tv(&func_tv);
    clear_tv(&rettv);
    clear_tv(&provider_tv);

    واپس res;
}

/*
 * Get the specified callback "function" from the provider dictionary for
 * register "reg".
 */
    ساکن صحیح
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

    اگر (dict_get_tv(providers, (حرف *)provider, &provider_tv) == FAIL)
	واپس FAIL;
    ورنہ اگر (provider_tv.v_type != VAR_DICT)
    {
	clear_tv(&provider_tv);
	واپس FAIL;
    }
    ورنہ اگر (dict_get_tv(
		provider_tv.vval.v_dict,
		(حرف *)function,
		&action_tv) == FAIL)
    {
	clear_tv(&provider_tv);
	واپس FAIL;
    }
    ورنہ اگر (action_tv.v_type != VAR_DICT)
    {
	clear_tv(&provider_tv);
	clear_tv(&action_tv);
	واپس FAIL;
    }
    ورنہ اگر (dict_get_tv(action_tv.vval.v_dict, (حرف *)reg, &func_tv) == FAIL)
    {
	clear_tv(&provider_tv);
	clear_tv(&action_tv);
	واپس FAIL;
    }
    ورنہ اگر ((cb = get_callback(&func_tv)).cb_name == تہی)
    {
	clear_tv(&provider_tv);
	clear_tv(&action_tv);
	clear_tv(&func_tv);
	واپس FAIL;
    }
    clear_tv(&provider_tv);
    clear_tv(&action_tv);

    // func_tv owns the function name, so we must make a copy for the callback
    سیٹ_callback(callback, &cb);
    clear_tv(&func_tv);
    واپس OK;
}

    ساکن خالی
clip_provider_copy(char_u *reg, char_u *provider)
{
    ساکن bool	recursive = false;
    callback_T	callback;
    typval_T	rettv;
    typval_T	argvars[4];
    yankreg_T	*y_ptr;
    char_u	type[2 + NUMBUFLEN] = {0};
    list_T	*list = تہی;

    اگر (recursive)
	واپس;

    اگر (clip_provider_get_callback(
		reg,
		provider,
		(char_u *)"copy",
		&callback) == FAIL)
	واپس;

    // Convert register type into a string
    اگر (*reg == '+')
	y_ptr = get_y_رجسٹر(REAL_PLUS_رجسٹر);
    ورنہ
	y_ptr = get_y_رجسٹر(ستارہ_رجسٹر);

    انتخاب (y_ptr->y_type)
    {
	حالت MCHAR:
	    type[0] = 'v';
	    توڑو;
	حالت MLINE:
	    type[0] = 'V';
	    توڑو;
	حالت MBLOCK:
	    sprintf((حرف *)type, "%c%d", Ctrl_V, y_ptr->y_چوڑائی + 1);
	    توڑو;
	طےشدہ:
	    type[0] = 0;
	    توڑو;
    }

    argvars[0].v_type = VAR_STRING;
    argvars[0].vval.v_string = reg;

    argvars[1].v_type = VAR_STRING;
    argvars[1].vval.v_string = type;

    // Get register contents by creating a list of lines
    list = list_alloc();

    اگر (list == تہی)
    {
	free_callback(&callback);
	واپس;
    }

    برائے (صحیح i = 0; i < y_ptr->y_سائز; i++)
	اگر (list_append_string(list, y_ptr->y_array[i].string,
	    (صحیح)y_ptr->y_array[i].لمبائی) == FAIL)
	{
	    free_callback(&callback);
	    list_unref(list);
	    واپس;
	}

    list->lv_refcount++;

    argvars[2].v_type = VAR_LIST;
    argvars[2].v_تالا = VAR_FIXED;
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

    ساکن خالی
clip_provider_paste(char_u *reg, char_u *provider)
{
    ساکن bool	recursive = false;
    callback_T	callback;
    typval_T	argvars[2];
    typval_T	rettv;
    صحیح		ret;
    char_u	*reg_type;
    list_T	*سطریں;

    اگر (recursive)
	واپس;

    اگر (clip_provider_get_callback(
		reg,
		provider,
		(char_u *)"paste",
		&callback) == FAIL)
	واپس;

    argvars[0].v_type = VAR_STRING;
    argvars[0].vval.v_string = reg;

    argvars[1].v_type = VAR_UNKNOWN;

    textlock++;
    recursive = true;
    ret = call_callback(&callback, -1, &rettv, 1, argvars);
    recursive = false;
    textlock--;

    اگر (ret == FAIL)
	جاؤ باہر_نکلو;
    ورنہ اگر (rettv.v_type == VAR_TUPLE
	    && TUPLE_LEN(rettv.vval.v_tuple) == 2
	    && TUPLE_شے(rettv.vval.v_tuple, 0)->v_type == VAR_STRING
	    && TUPLE_شے(rettv.vval.v_tuple, 1)->v_type == VAR_LIST)
    {
	reg_type = TUPLE_شے(rettv.vval.v_tuple, 0)->vval.v_string;
	سطریں = TUPLE_شے(rettv.vval.v_tuple, 1)->vval.v_list;
    }
    ورنہ اگر (rettv.v_type == VAR_LIST
	    && rettv.vval.v_list->lv_len == 2
	    && rettv.vval.v_list->lv_پہلا->li_tv.v_type == VAR_STRING
	    && rettv.vval.v_list->lv_پہلا->li_اگلا->li_tv.v_type == VAR_LIST)
    {
	reg_type = rettv.vval.v_list->lv_پہلا->li_tv.vval.v_string;
	سطریں = rettv.vval.v_list->lv_پہلا->li_اگلا->li_tv.vval.v_list;
    }
    ورنہ
	جاؤ باہر_نکلو;

    {
	char_u		yank_type = MAUTO;
	طویل		بلاک_len = -1;
	yankreg_T	*y_ptr, *cur_y_ptr;
	char_u		**lstval;
	char_u		**allocval;
	char_u		buf[NUMBUFLEN];
	char_u		**curval;
	char_u		**curallocval;
	char_u		*strval;
	listitem_T	*li;
	صحیح		len;

	// If the list is NULL handle like an empty list.
	len = سطریں == تہی ? 0 : سطریں->lv_len;

	// First half: use for pointers to result lines; second half: use for
	// pointers to allocated copies.
	lstval = ALLOC_MULT(char_u *, (len + 1) * 2);
	اگر (lstval == تہی)
	    جاؤ باہر_نکلو;
	curval = lstval;
	allocval = lstval + len + 2;
	curallocval = allocval;

	اگر (سطریں != تہی)
	{
	    CHECK_LIST_MATERIALIZE(سطریں);
	    FOR_ALL_LIST_اشیاء(سطریں, li)
	    {
		strval = tv_get_string_buf_chk(&li->li_tv, buf);
		اگر (strval == تہی)
		    جاؤ free_lstval;
		اگر (strval == buf)
		{
		    // Need to make a copy, next tv_get_string_buf_chk() will
		    // overwrite the string.
		    strval = vim_strsave(buf);
		    اگر (strval == تہی)
			جاؤ free_lstval;
		    *curallocval++ = strval;
		}
		*curval++ = strval;
	    }
	}
	*curval++ = تہی;

	اگر (*reg_type != NUL && (STRLEN(reg_type) == 0
		|| get_yank_type(&reg_type, &yank_type, &بلاک_len) == FAIL))
	{
	    emsg(e_invalid_دلیل);
	    جاؤ free_lstval;
	}

	اگر (*reg == '+')
	    y_ptr = get_y_رجسٹر(REAL_PLUS_رجسٹر);
	ورنہ
	    y_ptr = get_y_رجسٹر(ستارہ_رجسٹر);

	// Free previous register contents
	cur_y_ptr = get_y_current();
	سیٹ_y_current(y_ptr);

	free_yank_all();
	get_y_current()->y_سائز = 0;

	سیٹ_y_current(cur_y_ptr);

	str_مقصد_reg(y_ptr,
		yank_type,
		(char_u *)lstval,
		-1,
		بلاک_len,
		TRUE);

free_lstval:
	جب (curallocval > allocval)
	    vim_free(*--curallocval);
	vim_free(lstval);
    }

باہر_نکلو:
    free_callback(&callback);
    clear_tv(&rettv);
}

// Used to stop calling the provider callback every time there is an update.
// This prevents unnecessary calls when accessing the provider often in an
// interval.
//
// If -1 then allow provider callback to be called then set to one. Default
// value (is allowed) is -2.
ساکن صحیح ستارہ_pause_گنتی = -2, plus_pause_گنتی = -2;

    خالی
call_clip_provider_request(صحیح reg)
{
    اگر (clipmethod != CLIPMETHOD_PROVIDER)
	واپس;

    اگر (reg == '+' && plus_pause_گنتی < 0)
    {
	اگر (plus_pause_گنتی == -1)
	    plus_pause_گنتی = 1;
	clip_provider_paste((char_u *)"+", clip_provider);
    }
    ورنہ اگر (reg == '*' && ستارہ_pause_گنتی < 0)
    {
	اگر (ستارہ_pause_گنتی == -1)
	    ستارہ_pause_گنتی = 1;
	clip_provider_paste((char_u *)"*", clip_provider);
    }
    ورنہ
	واپس;
}

    خالی
call_clip_provider_سیٹ(صحیح reg)
{
    اگر (clipmethod != CLIPMETHOD_PROVIDER)
	واپس;

    اگر (reg == '+' && plus_pause_گنتی < 0)
    {
	اگر (plus_pause_گنتی == -1)
	    plus_pause_گنتی = 1;
	clip_provider_copy((char_u *)"+", clip_provider);
    }
    ورنہ اگر (reg == '*' && ستارہ_pause_گنتی < 0)
    {
	اگر (ستارہ_pause_گنتی == -1)
	    ستارہ_pause_گنتی = 1;
	clip_provider_copy((char_u *)"*", clip_provider);
    }
}

/*
 * Makes it so that the next provider call is only done once any calls after are
 * ignored, until dec_clip_provider is called the same number of times after
 * again. Note that this is per clipboard register ("+", "*")
 */
    خالی
inc_clip_provider(خالی)
{
    plus_pause_گنتی = (plus_pause_گنتی == -2
	|| plus_pause_گنتی == -1) ? -1 : plus_pause_گنتی + 1;
    ستارہ_pause_گنتی = (ستارہ_pause_گنتی == -2
	|| ستارہ_pause_گنتی == -1) ? -1 : ستارہ_pause_گنتی + 1;
}

    خالی
dec_clip_provider(خالی)
{
    اگر (plus_pause_گنتی != -2)
	plus_pause_گنتی = plus_pause_گنتی == -1 ? -1 : plus_pause_گنتی - 1;
    اگر (ستارہ_pause_گنتی != -2)
	ستارہ_pause_گنتی = ستارہ_pause_گنتی == -1 ? -1 : ستارہ_pause_گنتی - 1;

    اگر (plus_pause_گنتی == 0 || plus_pause_گنتی == -1)
	plus_pause_گنتی = -2;
    اگر (ستارہ_pause_گنتی == 0 || ستارہ_pause_گنتی == -1)
	ستارہ_pause_گنتی = -2;
}

#اگر_ختم // FEAT_CLIPBOARD_PROVIDER

#اگر defined(FEAT_WAYLAND_CLIPBOARD) || (defined(FEAT_GUI_GTK) && defined(USE_GTK4))
/*
 * If "vim" is TRUE, then get the motion type. If "vimenc" is TRUE, then get the
 * motion type and also convert "*buf". "buf" and "len_store" will be updated to
 * reflect the actual contents, but should be set beforehand with the initial
 * contents. Note that if "*tofree" is not NULL, use vim_free() on "*buf",
 * otherwise use the free func for which "*buf" was allocated with. In both
 * cases use vim_free() on "*tofree". Returns OK on success and FAIL on failure.
 */
    صحیح
clip_convert_ڈیٹا(
	char_u	**buf,
	طویل	*len_store,
	صحیح	*motion,
	bool	vim,
	bool	vimenc,
	char_u	**tofree)
{
    char_u  *final = *buf;
    char_u  *enc;
    طویل    len = *len_store;

    اگر (vim && len >= 2)
    {
	*motion = *final++;
	len--;
    }
    ورنہ اگر (vimenc && len >= 3)
    {
	vimconv_T   conv;
	صحیح	    convlen;

	// First byte is motion type
	*motion = *final++;
	len--;

	// Get encoding of selection
	enc = final;

	// Skip the encoding type including null terminator in final text
	final = memchr(final, NUL, len);
	اگر (final == تہی)
	    واپس FAIL;
	final++; // Skip NUL

	// Subtract pointers to get length of encoding;
	len -= final - enc;

	conv.vc_type = CONV_NONE;
	convert_تیاری(&conv, enc, p_enc);
	اگر (conv.vc_type != CONV_NONE)
	{
	   char_u *tmp;

	   convlen = len;
	   tmp = string_convert(&conv, final, &convlen);
	   len = convlen;
	   اگر (tmp != تہی)
	   {
		final = tmp;
		*tofree = final;
	   }
	   convert_تیاری(&conv, تہی, تہی);
	}
    }
    *buf = final;
    *len_store = len;
    واپس OK;
}
#اگر_ختم
