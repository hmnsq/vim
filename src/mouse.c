/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

/*
 * mouse.c: mouse handling functions
 */

#ಅನ್ತರ್ಭಾವ "vim.h"

/*
 * Horiziontal and vertical steps used when scrolling.
 * When negative scroll by a whole page.
 */
ಸ್ಥಿರ ದೀರ್ಘ mouse_hor_ಹೆಜ್ಜೆ = 6;
ಸ್ಥಿರ ದೀರ್ಘ mouse_vert_ಹೆಜ್ಜೆ = 3;
ಸ್ಥಿರ win_T *dragwin = ಖಾಲಿ;	// window being dragged
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ stl_click_handler(win_T *wp, ಪೂರ್ಣಾಂಕ mrow, ಪೂರ್ಣಾಂಕ mcol, ಪೂರ್ಣಾಂಕ which_button,
								    ಪೂರ್ಣಾಂಕ mods);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ stl_click_handler_regions(stl_click_region_T *regions,
				    ಪೂರ್ಣಾಂಕ region_ಎಣಿಕೆ, ಪೂರ್ಣಾಂಕ winid,
				    char_u *area_name, ಪೂರ್ಣಾಂಕ mrow, ಪೂರ್ಣಾಂಕ mcol,
				    ಪೂರ್ಣಾಂಕ which_button, ಪೂರ್ಣಾಂಕ mods);

    ಶೂನ್ಯ
mouse_ಗಣ_vert_scroll_ಹೆಜ್ಜೆ(ದೀರ್ಘ ಹೆಜ್ಜೆ)
{
    mouse_vert_ಹೆಜ್ಜೆ = ಹೆಜ್ಜೆ;
}

    ಶೂನ್ಯ
mouse_ಗಣ_hor_scroll_ಹೆಜ್ಜೆ(ದೀರ್ಘ ಹೆಜ್ಜೆ)
{
    mouse_hor_ಹೆಜ್ಜೆ = ಹೆಜ್ಜೆ;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CHECK_DOUBLE_CLICK
/*
 * Return the duration from t1 to t2 in milliseconds.
 */
    ಸ್ಥಿರ ದೀರ್ಘ
ಸಮಯ_diff_ms(ರಚನೆ timeval *t1, ರಚನೆ timeval *t2)
{
    // This handles wrapping of tv_usec correctly without any special case.
    // Example of 2 pairs (tv_sec, tv_usec) with a duration of 5 ms:
    //	   t1 = (1, 998000) t2 = (2, 3000) gives:
    //	   (2 - 1) * 1000 + (3000 - 998000) / 1000 -> 5 ms.
    ಹಿಂತಿರುಗಿ (t2->tv_sec - t1->tv_sec) * 1000
	 + (t2->tv_usec - t1->tv_usec) / 1000;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Get class of a character for selection: same class means same word.
 * 0: blank
 * 1: punctuation groups
 * 2: normal word character
 * >2: multi-byte word character.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
get_mouse_class(char_u *p)
{
    ಪೂರ್ಣಾಂಕ		c;

    ಆದರೆ (has_mbyte && MB_BYTE2LEN(p[0]) > 1)
	ಹಿಂತಿರುಗಿ mb_get_class(p);

    c = *p;
    ಆದರೆ (c == ' ' || c == '\t')
	ಹಿಂತಿರುಗಿ 0;

    ಆದರೆ (vim_iswordc(c))
	ಹಿಂತಿರುಗಿ 2;

    // There are a few special cases where we want certain combinations of
    // characters to be considered as a single word.  These are things like
    // "->", "/ *", "*=", "+=", "&=", "<=", ">=", "!=" etc.  Otherwise, each
    // character is in its own class.
    ಆದರೆ (c != NUL && vim_strchr((char_u *)"-+*/%<>&|^!=", c) != ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ 1;
    ಹಿಂತಿರುಗಿ c;
}

/*
 * Move "pos" back to the start of the word it's in.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ಕಂಡುಹಿಡಿ_ಆರಂಭ_ದ_ಶಬ್ದ(pos_T *pos)
{
    char_u	*ಸಾಲು;
    ಪೂರ್ಣಾಂಕ		cclass;
    ಪೂರ್ಣಾಂಕ		ಲಂಬಸಾಲು;

    ಸಾಲು = ml_get(pos->lnum);
    cclass = get_mouse_class(ಸಾಲು + pos->ಲಂಬಸಾಲು);

    ಆಗ (pos->ಲಂಬಸಾಲು > 0)
    {
	ಲಂಬಸಾಲು = pos->ಲಂಬಸಾಲು - 1;
	ಲಂಬಸಾಲು -= (*mb_ತಲೆ_off)(ಸಾಲು, ಸಾಲು + ಲಂಬಸಾಲು);
	ಆದರೆ (get_mouse_class(ಸಾಲು + ಲಂಬಸಾಲು) != cclass)
	    ನಿಲ್ಲಿಸು;
	pos->ಲಂಬಸಾಲು = ಲಂಬಸಾಲು;
    }
}

/*
 * Move "pos" forward to the end of the word it's in.
 * When 'selection' is "exclusive", the position is just after the word.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ಕಂಡುಹಿಡಿ_ಕೊನೆ_ದ_ಶಬ್ದ(pos_T *pos)
{
    char_u	*ಸಾಲು;
    ಪೂರ್ಣಾಂಕ		cclass;
    ಪೂರ್ಣಾಂಕ		ಲಂಬಸಾಲು;

    ಸಾಲು = ml_get(pos->lnum);
    ಆದರೆ (*p_sel == 'e' && pos->ಲಂಬಸಾಲು > 0)
    {
	--pos->ಲಂಬಸಾಲು;
	pos->ಲಂಬಸಾಲು -= (*mb_ತಲೆ_off)(ಸಾಲು, ಸಾಲು + pos->ಲಂಬಸಾಲು);
    }
    cclass = get_mouse_class(ಸಾಲು + pos->ಲಂಬಸಾಲು);
    ಆಗ (ಸಾಲು[pos->ಲಂಬಸಾಲು] != NUL)
    {
	ಲಂಬಸಾಲು = pos->ಲಂಬಸಾಲು + (*mb_ptr2len)(ಸಾಲು + pos->ಲಂಬಸಾಲು);
	ಆದರೆ (get_mouse_class(ಸಾಲು + ಲಂಬಸಾಲು) != cclass)
	{
	    ಆದರೆ (*p_sel == 'e')
		pos->ಲಂಬಸಾಲು = ಲಂಬಸಾಲು;
	    ನಿಲ್ಲಿಸು;
	}
	pos->ಲಂಬಸಾಲು = ಲಂಬಸಾಲು;
    }
}

#ಆದರೆ defined(FEAT_GUI_MOTIF) || defined(FEAT_GUI_GTK) \
	    || defined(FEAT_GUI_MSWIN) \
	    || defined(FEAT_GUI_PHOTON) \
	    || defined(FEAT_ಪದ_POPUP_MENU)
# define USE_POPUP_SETPOS
# define NEED_VCOL2_ಲಂಬಸಾಲು

/*
 * Translate window coordinates to buffer position without any side effects.
 * Returns IN_BUFFER and sets "mpos->col" to the column when in buffer text.
 * The column is one for the first column.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
get_fpos_ದ_mouse(pos_T *mpos)
{
    win_T	*wp;
    ಪೂರ್ಣಾಂಕ		ಅಡ್ಡಸಾಲು = mouse_ಅಡ್ಡಸಾಲು;
    ಪೂರ್ಣಾಂಕ		ಲಂಬಸಾಲು = mouse_ಲಂಬಸಾಲು;

    ಆದರೆ (ಅಡ್ಡಸಾಲು < 0 || ಲಂಬಸಾಲು < 0)		// check if it makes sense
	ಹಿಂತಿರುಗಿ ಒಳ_UNKNOWN;

    // find the window where the row is in
    wp = mouse_ಕಂಡುಹಿಡಿ_win(&ಅಡ್ಡಸಾಲು, &ಲಂಬಸಾಲು, FAIL_POPUP);
    ಆದರೆ (wp == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಒಳ_UNKNOWN;
    // winpos and height may change in win_enter()!
    ಆದರೆ (ಅಡ್ಡಸಾಲು >= wp->w_ಎತ್ತರ)	// In (or below) status line
	ಹಿಂತಿರುಗಿ ಒಳ_ಸ್ಥಿತಿ_ಸಾಲು;
    ಆದರೆ (ಲಂಬಸಾಲು >= wp->w_ಅಗಲ)	// In vertical separator line
	ಹಿಂತಿರುಗಿ ಒಳ_SEP_ಸಾಲು;

    ಆದರೆ (wp != curwin)
	ಹಿಂತಿರುಗಿ ಒಳ_UNKNOWN;

    // compute the position in the buffer line from the posn on the screen
    ಆದರೆ (mouse_comp_pos(curwin, &ಅಡ್ಡಸಾಲು, &ಲಂಬಸಾಲು, &mpos->lnum, ಖಾಲಿ))
	ಹಿಂತಿರುಗಿ ಒಳ_ಸ್ಥಿತಿ_ಸಾಲು; // past bottom

    mpos->ಲಂಬಸಾಲು = vcol2col(wp, mpos->lnum, ಲಂಬಸಾಲು, &mpos->coladd);
    ಹಿಂತಿರುಗಿ ಒಳ_ಬಫರ್;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Do the appropriate action for the current mouse click in the current mode.
 * Not used for Command-line mode.
 *
 * Normal and Visual Mode:
 * event	 modi-	position      visual	   change   action
 *		 fier	cursor			   window
 * left press	  -	yes	    end		    yes
 * left press	  C	yes	    end		    yes	    "^]" (2)
 * left press	  S	yes	end (popup: extend) yes	    "*" (2)
 * left drag	  -	yes	start if moved	    no
 * left relse	  -	yes	start if moved	    no
 * middle press	  -	yes	 if not active	    no	    put register
 * middle press	  -	yes	 if active	    no	    yank and put
 * right press	  -	yes	start or extend	    yes
 * right press	  S	yes	no change	    yes	    "#" (2)
 * right drag	  -	yes	extend		    no
 * right relse	  -	yes	extend		    no
 *
 * Insert or Replace Mode:
 * event	 modi-	position      visual	   change   action
 *		 fier	cursor			   window
 * left press	  -	yes	(cannot be active)  yes
 * left press	  C	yes	(cannot be active)  yes	    "CTRL-O^]" (2)
 * left press	  S	yes	(cannot be active)  yes	    "CTRL-O*" (2)
 * left drag	  -	yes	start or extend (1) no	    CTRL-O (1)
 * left relse	  -	yes	start or extend (1) no	    CTRL-O (1)
 * middle press	  -	no	(cannot be active)  no	    put register
 * right press	  -	yes	start or extend	    yes	    CTRL-O
 * right press	  S	yes	(cannot be active)  yes	    "CTRL-O#" (2)
 *
 * (1) only if mouse pointer moved since press
 * (2) only if click is in same buffer
 *
 * Return TRUE if start_arrow() should be called for edit mode.
 */
    ಪೂರ್ಣಾಂಕ
do_mouse(
    oparg_T	*oap,		// operator argument, can be NULL
    ಪೂರ್ಣಾಂಕ		c,		// K_LEFTMOUSE, etc
    ಪೂರ್ಣಾಂಕ		dir,		// Direction to 'put' if necessary
    ದೀರ್ಘ	ಎಣಿಕೆ_2,
    ಪೂರ್ಣಾಂಕ		fixindent)	// PUT_FIXINDENT if fixing indent necessary
{
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	do_always = FALSE;	// ignore 'mouse' setting next time
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	got_click = FALSE;	// got a click some time back

    ಪೂರ್ಣಾಂಕ		which_button;	// MOUSE_LEFT, _MIDDLE or _RIGHT
    ಪೂರ್ಣಾಂಕ		is_click = FALSE; // If FALSE it's a drag or release event
    ಪೂರ್ಣಾಂಕ		is_drag = FALSE;  // If TRUE it's a drag event
    ಪೂರ್ಣಾಂಕ		jump_flags = 0;	// flags for jump_to_mouse()
    pos_T	ಆರಂಭ_visual;
    ಪೂರ್ಣಾಂಕ		moved;		// Has cursor moved?
    ಪೂರ್ಣಾಂಕ		ಒಳ_ಸ್ಥಿತಿ_ಸಾಲು_2;	// mouse in status line
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	ಒಳ_tab_ಸಾಲು = FALSE; // mouse clicked in tab line
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	ಒಳ_tabpanel = FALSE; // mouse clicked in tabpanel
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TABPANEL
    ಸ್ಥಿರ bool	ಒಳ_tabpanel_scrollbar = false; // dragging tabpanel scrollbar
#ಆದರೆ_ಅಂತ್ಯ
    ಪೂರ್ಣಾಂಕ		ಒಳ_sep_ಸಾಲು;	// mouse in vertical separator line
    ಪೂರ್ಣಾಂಕ		c1, c2;
#ಆದರೆ defined(FEAT_FOLDING)
    pos_T	save_cursor;
#ಆದರೆ_ಅಂತ್ಯ
    win_T	*old_curwin = curwin;
    ಸ್ಥಿರ pos_T orig_cursor;
    colnr_T	leftcol, rightcol;
    pos_T	ಕೊನೆ_visual;
    ಪೂರ್ಣಾಂಕ		diff;
    ಪೂರ್ಣಾಂಕ		old_ಸಕ್ರಿಯ = VIsual_ಸಕ್ರಿಯ;
    ಪೂರ್ಣಾಂಕ		old_ಬಹುಲಕ = VIsual_ಬಹುಲಕ;
    ಪೂರ್ಣಾಂಕ		regname;

#ಆದರೆ defined(FEAT_FOLDING)
    save_cursor = curwin->w_cursor;
#ಆದರೆ_ಅಂತ್ಯ

    // When GUI is active, always recognize mouse events, otherwise:
    // - Ignore mouse event in normal mode if 'mouse' doesn't include 'n'.
    // - Ignore mouse event in visual mode if 'mouse' doesn't include 'v'.
    // - For command line and insert mode 'mouse' is checked before calling
    //	 do_mouse().
    ಆದರೆ (do_always)
	do_always = FALSE;
    ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
	ಆದರೆ (!gui.ಒಳ_use)
#ಆದರೆ_ಅಂತ್ಯ
	{
	    ಆದರೆ (VIsual_ಸಕ್ರಿಯ)
	    {
		ಆದರೆ (!mouse_has(MOUSE_VISUAL))
		    ಹಿಂತಿರುಗಿ FALSE;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (State == ಬಹುಲಕ_ಸಾಮಾನ್ಯ && !mouse_has(MOUSE_ಸಾಮಾನ್ಯ))
		ಹಿಂತಿರುಗಿ FALSE;
	}

    ಗಾಗಿ (;;)
    {
	which_button = get_mouse_button(KEY2TERMCAP1(c), &is_click, &is_drag);
	ಆದರೆ (is_drag)
	{
	    // If the next character is the same mouse event then use that
	    // one. Speeds up dragging the status line.
	    // Note: Since characters added to the stuff buffer in the code
	    // below need to come before the next character, do not do this
	    // when the current character was stuffed.
	    ಆದರೆ (!ಕೀಲಿ_Stuffed && vpeekc() != NUL)
	    {
		ಪೂರ್ಣಾಂಕ nc;
		ಪೂರ್ಣಾಂಕ save_mouse_ಅಡ್ಡಸಾಲು = mouse_ಅಡ್ಡಸಾಲು;
		ಪೂರ್ಣಾಂಕ save_mouse_ಲಂಬಸಾಲು = mouse_ಲಂಬಸಾಲು;

		// Need to get the character, peeking doesn't get the actual
		// one.
		nc = ಸುರಕ್ಷಿತ_vgetc();
		ಆದರೆ (c == nc)
		    ಮುಂದುವರಿ;
		vungetc(nc);
		mouse_ಅಡ್ಡಸಾಲು = save_mouse_ಅಡ್ಡಸಾಲು;
		mouse_ಲಂಬಸಾಲು = save_mouse_ಲಂಬಸಾಲು;
	    }
	}
	ನಿಲ್ಲಿಸು;
    }

    ಆದರೆ (c == K_MOUSEMOVE)
    {
	// Mouse moved without a button pressed.
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_BEVAL_ಪದ
	ui_may_remove_balloon();
	ಆದರೆ (p_bevalterm)
	{
	    profile_setlimit(p_bdlay, &bevalexpr_due);
	    bevalexpr_due_ಗಣ = TRUE;
	}
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
	popup_handle_mouse_moved();
#ಆದರೆ_ಅಂತ್ಯ
	ಹಿಂತಿರುಗಿ FALSE;
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MOUSESHAPE
    // May have stopped dragging the status or separator line.  The pointer is
    // most likely still on the status or separator line.
    ಆದರೆ (!is_drag && drag_ಸ್ಥಿತಿ_ಸಾಲು)
    {
	drag_ಸ್ಥಿತಿ_ಸಾಲು = FALSE;
	update_mouseshape(SHAPE_IDX_ಸ್ಥಿತಿ);
    }
    ಆದರೆ (!is_drag && drag_sep_ಸಾಲು)
    {
	drag_sep_ಸಾಲು = FALSE;
	update_mouseshape(SHAPE_IDX_VSEP);
    }
#ಆದರೆ_ಅಂತ್ಯ

    // Ignore drag and release events if we didn't get a click.
    ಆದರೆ (is_click)
    {
	got_click = TRUE;
	ಒಳ_tab_ಸಾಲು = FALSE;
	ಒಳ_tabpanel = FALSE;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TABPANEL
	ಒಳ_tabpanel_scrollbar = false;
#ಆದರೆ_ಅಂತ್ಯ
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಆದರೆ (!got_click)			// didn't get click, ignore
	    ಹಿಂತಿರುಗಿ FALSE;
	ಆದರೆ (!is_drag)			// release, reset got_click
	{
	    got_click = FALSE;
	    ಆದರೆ (ಒಳ_tab_ಸಾಲು || ಒಳ_tabpanel
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TABPANEL
		    || ಒಳ_tabpanel_scrollbar
#ಆದರೆ_ಅಂತ್ಯ
		    )
	    {
		ಒಳ_tab_ಸಾಲು = FALSE;
		ಒಳ_tabpanel = FALSE;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TABPANEL
		ಒಳ_tabpanel_scrollbar = false;
#ಆದರೆ_ಅಂತ್ಯ
		ಹಿಂತಿರುಗಿ FALSE;
	    }
	}
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TABPANEL
    // Continue a scrollbar drag before any tab-selection handling.
    ಆದರೆ (is_drag && ಒಳ_tabpanel_scrollbar)
    {
	tabpanel_drag_scrollbar(mouse_ಅಡ್ಡಸಾಲು);
	ಹಿಂತಿರುಗಿ FALSE;
    }
#ಆದರೆ_ಅಂತ್ಯ

    // CTRL right mouse button does CTRL-T
    ಆದರೆ (is_click && (mod_ಮುಖವಾಡ & MOD_ಮುಖವಾಡ_CTRL) && which_button == MOUSE_ಬಲ)
    {
	ಆದರೆ (State & ಬಹುಲಕ_ಸೇರಿಸು)
	    stuffcharReadbuff(Ctrl_O);
	ಆದರೆ (ಎಣಿಕೆ_2 > 1)
	    stuffnumReadbuff(ಎಣಿಕೆ_2);
	stuffcharReadbuff(Ctrl_T);
	got_click = FALSE;		// ignore drag&release now
	ಹಿಂತಿರುಗಿ FALSE;
    }

    // CTRL only works with left mouse button
    ಆದರೆ ((mod_ಮುಖವಾಡ & MOD_ಮುಖವಾಡ_CTRL) && which_button != MOUSE_ಎಡ)
	ಹಿಂತಿರುಗಿ FALSE;

    // When a modifier is down, ignore drag and release events, as well as
    // multiple clicks and the middle mouse button.
    // Accept shift-leftmouse drags when 'mousemodel' is "popup.*".
    ಆದರೆ ((mod_ಮುಖವಾಡ & (MOD_ಮುಖವಾಡ_ಸರಿಸು | MOD_ಮುಖವಾಡ_CTRL | MOD_ಮುಖವಾಡ_ALT
							     | MOD_ಮುಖವಾಡ_META))
	    && (!is_click
		|| (mod_ಮುಖವಾಡ & MOD_ಮುಖವಾಡ_MULTI_CLICK)
		|| which_button == MOUSE_ಮಧ್ಯ)
	    && !((mod_ಮುಖವಾಡ & (MOD_ಮುಖವಾಡ_ಸರಿಸು|MOD_ಮುಖವಾಡ_ALT))
		&& mouse_model_popup()
		&& which_button == MOUSE_ಎಡ)
	    && !((mod_ಮುಖವಾಡ & MOD_ಮುಖವಾಡ_ALT)
		&& !mouse_model_popup()
		&& which_button == MOUSE_ಬಲ)
	    )
	ಹಿಂತಿರುಗಿ FALSE;

    // If the button press was used as the movement command for an operator
    // (eg "d<MOUSE>"), or it is the middle button that is held down, ignore
    // drag/release events.
    ಆದರೆ (!is_click && which_button == MOUSE_ಮಧ್ಯ)
	ಹಿಂತಿರುಗಿ FALSE;

    ಆದರೆ (oap != ಖಾಲಿ)
	regname = oap->regname;
    ಇಲ್ಲದಿದ್ದರೆ
	regname = 0;

    // Middle mouse button does a 'put' of the selected text
    ಆದರೆ (which_button == MOUSE_ಮಧ್ಯ)
    {
	ಆದರೆ (State == ಬಹುಲಕ_ಸಾಮಾನ್ಯ)
	{
	    // If an operator was pending, we don't know what the user wanted
	    // to do. Go back to normal mode: Clear the operator and beep().
	    ಆದರೆ (oap != ಖಾಲಿ && oap->op_type != OP_NOP)
	    {
		clearopbeep(oap);
		ಹಿಂತಿರುಗಿ FALSE;
	    }

	    // If visual was active, yank the highlighted text and put it
	    // before the mouse pointer position.
	    // In Select mode replace the highlighted text with the clipboard.
	    ಆದರೆ (VIsual_ಸಕ್ರಿಯ)
	    {
		ಆದರೆ (VIsual_select)
		{
		    stuffcharReadbuff(Ctrl_G);
		    stuffReadbuff((char_u *)"\"+p");
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    stuffcharReadbuff('y');
		    stuffcharReadbuff(K_MIDDLEMOUSE);
		}
		do_always = TRUE;	// ignore 'mouse' setting next time
		ಹಿಂತಿರುಗಿ FALSE;
	    }
	    // The rest is below jump_to_mouse()
	}

	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((State & ಬಹುಲಕ_ಸೇರಿಸು) == 0)
	    ಹಿಂತಿರುಗಿ FALSE;

	// Middle click in insert mode doesn't move the mouse, just insert the
	// contents of a register.  '.' register is special, can't insert that
	// with do_put().
	// Also paste at the cursor if the current mode isn't in 'mouse' (only
	// happens for the GUI).
	ಆದರೆ ((State & ಬಹುಲಕ_ಸೇರಿಸು) || !mouse_has(MOUSE_ಸಾಮಾನ್ಯ))
	{
	    ಆದರೆ (regname == '.')
		ಸೇರಿಸು_reg(regname, TRUE);
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CLIPBOARD
		ಆದರೆ (clip_ನಕ್ಷತ್ರ.ಲಭ್ಯ && regname == 0)
		    regname = '*';
#ಆದರೆ_ಅಂತ್ಯ
		ಆದರೆ ((State & REPLACE_FLAG) && !yank_ರಿಜಿಸ್ಟರ್_mline(regname))
		    ಸೇರಿಸು_reg(regname, TRUE);
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    do_put(regname, ಖಾಲಿ, BACKWARD, 1L,
						      fixindent | PUT_CURSEND);

		    // Repeat it with CTRL-R CTRL-O r or CTRL-R CTRL-P r
		    Append_Char_ಗಮ್ಯ_Redobuff(Ctrl_R);
		    Append_Char_ಗಮ್ಯ_Redobuff(fixindent ? Ctrl_P : Ctrl_O);
		    Append_Char_ಗಮ್ಯ_Redobuff(regname == 0 ? '"' : regname);
		}
	    }
	    ಹಿಂತಿರುಗಿ FALSE;
	}
    }

    // When dragging or button-up stay in the same window.
    ಆದರೆ (!is_click)
	jump_flags |= MOUSE_FOCUS | MOUSE_DID_MOVE;

    ಆರಂಭ_visual.lnum = 0;

    ರಚನೆ tabpage_label_info {
	bool is_panel;	    // label type. true: tabpanel, false: tab line
	bool just_ಒಳ;	    // just in tabpage label area
	bool just_click;    // just click tabpage label area
	ಪೂರ್ಣಾಂಕ nr;		    // tabpage number
    } tp_label = { false, false, false, 0 };

    // Check for clicking in the tab page panel.
#ಆದರೆ defined(FEAT_TABPANEL)
    ಆದರೆ (mouse_ಲಂಬಸಾಲು < firstwin->w_wincol
		|| mouse_ಲಂಬಸಾಲು >= firstwin->w_wincol + topframe->fr_ಅಗಲ)
    {
	// A click on the scrollbar column starts a drag interaction and
	// preempts tab-selection.
	ಆದರೆ (is_click && !is_drag && mouse_on_tabpanel_scrollbar())
	{
	    ಒಳ_tabpanel_scrollbar = TRUE;
	    tabpanel_drag_scrollbar(mouse_ಅಡ್ಡಸಾಲು);
	    ಹಿಂತಿರುಗಿ FALSE;
	}

	// Dispatch 'tabpanel' %[FuncName] click regions before falling through
	// to tab-page selection.  On drag events fall through to the normal
	// tab-drag handling.
	ಆದರೆ (is_click && !is_drag
		&& stl_click_handler_regions(tabpanel_stl_click,
					    tabpanel_stl_click_ಎಣಿಕೆ,
					    0, (char_u *)"tabpanel",
					    mouse_ಅಡ್ಡಸಾಲು, mouse_ಲಂಬಸಾಲು,
					    which_button, mod_ಮುಖವಾಡ))
	    ಹಿಂತಿರುಗಿ FALSE;

	tp_label.is_panel = true;
	tp_label.just_ಒಳ = true;
	tp_label.nr = get_tabpagenr_on_tabpanel();

	// click in a tab selects that tab page
	ಆದರೆ (is_click && cmdwin_type == 0)
	    tp_label.just_click = true;
    }
    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
    // Check for clicking in the tab page line.
    ಆದರೆ (Tab_ಪುಟ_Idxs != ಖಾಲಿ && mouse_ಅಡ್ಡಸಾಲು == 0 && firstwin->w_winrow > 0)
    {
	// Dispatch 'tabline' %[FuncName] click regions before falling through
	// to tab-page selection.  On drag events fall through to the normal
	// tab-drag handling.
	ಆದರೆ (is_click && !is_drag
		&& stl_click_handler_regions(tabline_stl_click,
					    tabline_stl_click_ಎಣಿಕೆ,
					    0, (char_u *)"tabline",
					    mouse_ಅಡ್ಡಸಾಲು, mouse_ಲಂಬಸಾಲು,
					    which_button, mod_ಮುಖವಾಡ))
	    ಹಿಂತಿರುಗಿ FALSE;

	tp_label.just_ಒಳ = true;
	tp_label.nr = Tab_ಪುಟ_Idxs[mouse_ಲಂಬಸಾಲು];

	// click in a tab selects that tab page
	ಆದರೆ (is_click && cmdwin_type == 0
		&& mouse_ಲಂಬಸಾಲು < firstwin->w_wincol + topframe->fr_ಅಗಲ)
	    tp_label.just_click = true;
    }

    ಆದರೆ (tp_label.just_ಒಳ)
    {
	ಆದರೆ (is_drag)
	{
	    ಆದರೆ (ಒಳ_tabpanel || ಒಳ_tab_ಸಾಲು)
	    {
		c1 = tp_label.nr;
		tabpage_move(c1 <= 0 ? 9999 : c1 < tabpage_ಸೂಚ್ಯಂಕ(curtab)
							    ? c1 - 1 : c1);
	    }
	    ಹಿಂತಿರುಗಿ FALSE;
	}

	ಆದರೆ (tp_label.just_click)
	{
	    ಆದರೆ (tp_label.is_panel)
		ಒಳ_tabpanel = TRUE;
	    ಇಲ್ಲದಿದ್ದರೆ
		ಒಳ_tab_ಸಾಲು = TRUE;
	    c1 = tp_label.nr;
	    ಆದರೆ (c1 >= 0)
	    {
		ಆದರೆ ((mod_ಮುಖವಾಡ & MOD_ಮುಖವಾಡ_MULTI_CLICK) == MOD_ಮುಖವಾಡ_2_CLICK
						    && !tp_label.is_panel)
		{
		    // Double-click on the tabline opens a new, empty tab page.
		    // The tabpanel has no "empty area" (every row maps to a tab)
		    // and this behavior is not documented for tabpanel, so fall
		    // through to the regular tab-switch path there.
		    ಕೊನೆ_visual_ಬಹುಲಕ_keep_button();
		    tabpage_new();
		    tabpage_move(c1 == 0 ? 9999 : c1 - 1);
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    // Go to specified tab page, or next one if not clicking
		    // on a label.
		    goto_tabpage(c1);

		    // It's like clicking on the status line of a window.
		    ಆದರೆ (curwin != old_curwin)
			ಕೊನೆ_visual_ಬಹುಲಕ_keep_button();
		}
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		tabpage_T	*tp;

		// Close the current or specified tab page.
		ಆದರೆ (c1 == -999)
		    tp = curtab;
		ಇಲ್ಲದಿದ್ದರೆ
		    tp = ಕಂಡುಹಿಡಿ_tabpage(-c1);
		ಆದರೆ (tp == curtab)
		{
		    ಆದರೆ (ಮೊದಲ_tabpage->tp_ಮುಂದಿನ != ಖಾಲಿ)
			tabpage_close(FALSE);
		}
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (tp != ಖಾಲಿ)
		    tabpage_close_other(tp, FALSE);
	    }
	}
	ಹಿಂತಿರುಗಿ TRUE;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (is_drag && (ಒಳ_tabpanel || (ಒಳ_tab_ಸಾಲು && Tab_ಪುಟ_Idxs != ಖಾಲಿ)))
    {
#ಆದರೆ defined(FEAT_TABPANEL)
	ಆದರೆ (ಒಳ_tabpanel)
	    c1 = get_tabpagenr_on_tabpanel();
	ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	    c1 = Tab_ಪುಟ_Idxs[mouse_ಲಂಬಸಾಲು];
	tabpage_move(c1 <= 0 ? 9999 : c1 - 1);
	ಹಿಂತಿರುಗಿ FALSE;
    }

    // When 'mousemodel' is "popup" or "popup_setpos", translate mouse events:
    // right button up   -> pop-up menu
    // shift-left button -> right button
    // alt-left button   -> alt-right button
    ಆದರೆ (mouse_model_popup())
    {
	ಆದರೆ (which_button == MOUSE_ಬಲ
			    && !(mod_ಮುಖವಾಡ & (MOD_ಮುಖವಾಡ_ಸರಿಸು | MOD_ಮುಖವಾಡ_CTRL)))
	{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_POPUP_SETPOS
# ifdef FEAT_GUI
	    ಆದರೆ (gui.ಒಳ_use)
	    {
#  ಆದರೆ defined(FEAT_GUI_MOTIF) || defined(FEAT_GUI_GTK) \
			  || defined(FEAT_GUI_PHOTON)
		ಆದರೆ (!is_click)
		    // Ignore right button release events, only shows the popup
		    // menu on the button down event.
		    ಹಿಂತಿರುಗಿ FALSE;
#  endif
#  ಆದರೆ defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_HAIKU)
		ಆದರೆ (is_click || is_drag)
		    // Ignore right button down and drag mouse events.  Windows
		    // only shows the popup menu on the button up event.
		    ಹಿಂತಿರುಗಿ FALSE;
#  endif
	    }
# endif
# ಆದರೆ defined(FEAT_GUI) && defined(FEAT_ಪದ_POPUP_MENU)
	    ಇಲ್ಲದಿದ್ದರೆ
# endif
# ಆದರೆ defined(FEAT_ಪದ_POPUP_MENU)
	    ಆದರೆ (!is_click)
		// Ignore right button release events, only shows the popup
		// menu on the button down event.
		ಹಿಂತಿರುಗಿ FALSE;
# endif

	    jump_flags = 0;
	    ಆದರೆ (STRCMP(p_mousem, "popup_setpos") == 0)
	    {
		// First set the cursor position before showing the popup
		// menu.
		ಆದರೆ (VIsual_ಸಕ್ರಿಯ)
		{
		    pos_T    m_pos;

		    // set MOUSE_MAY_STOP_VIS if we are outside the
		    // selection or the current window (might have false
		    // negative here)
		    ಆದರೆ (mouse_ಅಡ್ಡಸಾಲು < curwin->w_winrow
			 || mouse_ಅಡ್ಡಸಾಲು
				  > (curwin->w_winrow + curwin->w_ಎತ್ತರ))
			jump_flags = MOUSE_MAY_STOP_VIS;
		    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (get_fpos_ದ_mouse(&m_pos) != ಒಳ_ಬಫರ್)
			jump_flags = MOUSE_MAY_STOP_VIS;
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			ಆದರೆ (VIsual_ಬಹುಲಕ == 'V')
			{
			    ಆದರೆ ((curwin->w_cursor.lnum <= VIsual.lnum
				    && (m_pos.lnum < curwin->w_cursor.lnum
					|| VIsual.lnum < m_pos.lnum))
				|| (VIsual.lnum < curwin->w_cursor.lnum
				    && (m_pos.lnum < VIsual.lnum
					|| curwin->w_cursor.lnum < m_pos.lnum)))
			    {
				jump_flags = MOUSE_MAY_STOP_VIS;
			    }
			}
			ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((LTOREQ_POS(curwin->w_cursor, VIsual)
				    && (LT_POS(m_pos, curwin->w_cursor)
					|| LT_POS(VIsual, m_pos)))
				|| (LT_POS(VIsual, curwin->w_cursor)
				    && (LT_POS(m_pos, VIsual)
				      || LT_POS(curwin->w_cursor, m_pos))))
			{
			    jump_flags = MOUSE_MAY_STOP_VIS;
			}
			ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (VIsual_ಬಹುಲಕ == Ctrl_V)
			{
			    getvcols(curwin, &curwin->w_cursor, &VIsual,
						     &leftcol, &rightcol, 0);
			    getvcol(curwin, &m_pos, ಖಾಲಿ, &m_pos.ಲಂಬಸಾಲು, ಖಾಲಿ, 0);
			    ಆದರೆ (m_pos.ಲಂಬಸಾಲು < leftcol || m_pos.ಲಂಬಸಾಲು > rightcol)
				jump_flags = MOUSE_MAY_STOP_VIS;
			}
		    }
		}
		ಇಲ್ಲದಿದ್ದರೆ
		    jump_flags = MOUSE_MAY_STOP_VIS;
	    }
	    ಆದರೆ (jump_flags)
	    {
		jump_flags = jump_ಗಮ್ಯ_mouse(jump_flags, ಖಾಲಿ, which_button);
		update_curbuf(VIsual_ಸಕ್ರಿಯ ? UPD_INVERTED : UPD_VALID);
		setcursor();
		ಹೊರಹೋಗು_flush();    // Update before showing popup menu
	    }
# ifdef FEAT_MENU
	    show_popupmenu();
	    got_click = FALSE;	// ignore release events
# endif
	    ಹಿಂತಿರುಗಿ (jump_flags & CURSOR_MOVED) != 0;
#ಇಲ್ಲದಿದ್ದರೆ
	    ಹಿಂತಿರುಗಿ FALSE;
#ಆದರೆ_ಅಂತ್ಯ
	}
	ಆದರೆ (which_button == MOUSE_ಎಡ
				&& (mod_ಮುಖವಾಡ & (MOD_ಮುಖವಾಡ_ಸರಿಸು|MOD_ಮುಖವಾಡ_ALT)))
	{
	    which_button = MOUSE_ಬಲ;
	    mod_ಮುಖವಾಡ &= ~MOD_ಮುಖವಾಡ_ಸರಿಸು;
	}
    }

    ಆದರೆ ((State & (ಬಹುಲಕ_ಸಾಮಾನ್ಯ | ಬಹುಲಕ_ಸೇರಿಸು))
			    && !(mod_ಮುಖವಾಡ & (MOD_ಮುಖವಾಡ_ಸರಿಸು | MOD_ಮುಖವಾಡ_CTRL)))
    {
	ಆದರೆ (which_button == MOUSE_ಎಡ)
	{
	    ಆದರೆ (is_click)
	    {
		// stop Visual mode for a left click in a window, but not when
		// on a status line
		ಆದರೆ (VIsual_ಸಕ್ರಿಯ)
		    jump_flags |= MOUSE_MAY_STOP_VIS;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (mouse_has(MOUSE_VISUAL))
		jump_flags |= MOUSE_MAY_VIS;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (which_button == MOUSE_ಬಲ)
	{
	    ಆದರೆ (is_click && VIsual_ಸಕ್ರಿಯ)
	    {
		// Remember the start and end of visual before moving the
		// cursor.
		ಆದರೆ (LT_POS(curwin->w_cursor, VIsual))
		{
		    ಆರಂಭ_visual = curwin->w_cursor;
		    ಕೊನೆ_visual = VIsual;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    ಆರಂಭ_visual = VIsual;
		    ಕೊನೆ_visual = curwin->w_cursor;
		}
	    }
	    jump_flags |= MOUSE_FOCUS;
	    ಆದರೆ (mouse_has(MOUSE_VISUAL))
		jump_flags |= MOUSE_MAY_VIS;
	}
    }

    // If an operator is pending, ignore all drags and releases until the
    // next mouse click.
    ಆದರೆ (!is_drag && oap != ಖಾಲಿ && oap->op_type != OP_NOP)
    {
	got_click = FALSE;
	oap->motion_type = MCHAR;
    }

    // When releasing the button let jump_to_mouse() know.
    ಆದರೆ (!is_click && !is_drag)
	jump_flags |= MOUSE_RELEASED;

    // JUMP!
    jump_flags = jump_ಗಮ್ಯ_mouse(jump_flags,
			oap == ಖಾಲಿ ? ಖಾಲಿ : &(oap->inclusive), which_button);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MENU
    // A click in the window toolbar has no side effects.
    ಆದರೆ (jump_flags & MOUSE_WINBAR)
	ಹಿಂತಿರುಗಿ FALSE;
#ಆದರೆ_ಅಂತ್ಯ
    moved = (jump_flags & CURSOR_MOVED);
    ಒಳ_ಸ್ಥಿತಿ_ಸಾಲು_2 = (jump_flags & ಒಳ_ಸ್ಥಿತಿ_ಸಾಲು);
    ಒಳ_sep_ಸಾಲು = (jump_flags & ಒಳ_SEP_ಸಾಲು);

    // Check for statusline click handler early, before visual mode or
    // other button-specific handling can interfere.
    ಆದರೆ (ಒಳ_ಸ್ಥಿತಿ_ಸಾಲು_2 && is_click && !is_drag
	    && stl_click_handler(dragwin, mouse_ಅಡ್ಡಸಾಲು, mouse_ಲಂಬಸಾಲು,
						which_button, mod_ಮುಖವಾಡ))
    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MOUSESHAPE
	ಆದರೆ (!drag_ಸ್ಥಿತಿ_ಸಾಲು)
	{
	    drag_ಸ್ಥಿತಿ_ಸಾಲು = TRUE;
	    update_mouseshape(-1);
	}
#ಆದರೆ_ಅಂತ್ಯ
	ಹಿಂತಿರುಗಿ FALSE;
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_NETBEANS_INTG
    ಆದರೆ (is_Netbeans_ಬಫರ್(curbuf)
			    && !(jump_flags & (ಒಳ_ಸ್ಥಿತಿ_ಸಾಲು | ಒಳ_SEP_ಸಾಲು)))
    {
	ಪೂರ್ಣಾಂಕ ಕೀಲಿ = KEY2TERMCAP1(c);

	ಆದರೆ (ಕೀಲಿ == (ಪೂರ್ಣಾಂಕ)KE_LEFTRELEASE || ಕೀಲಿ == (ಪೂರ್ಣಾಂಕ)KE_MIDDLERELEASE
					       || ಕೀಲಿ == (ಪೂರ್ಣಾಂಕ)KE_RIGHTRELEASE)
	    netbeans_button_release(which_button);
    }
#ಆದರೆ_ಅಂತ್ಯ

    // When jumping to another window, clear a pending operator.  That's a bit
    // friendlier than beeping and not jumping to that window.
    ಆದರೆ (curwin != old_curwin && oap != ಖಾಲಿ && oap->op_type != OP_NOP)
	clearop(oap);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
    ಆದರೆ (mod_ಮುಖವಾಡ == 0
	    && !is_drag
	    && (jump_flags & (MOUSE_FOLD_CLOSE | MOUSE_FOLD_OPEN))
	    && which_button == MOUSE_ಎಡ)
    {
	// open or close a fold at this line
	ಆದರೆ (jump_flags & MOUSE_FOLD_OPEN)
	    openFold(curwin->w_cursor.lnum, 1L);
	ಇಲ್ಲದಿದ್ದರೆ
	    closeFold(curwin->w_cursor.lnum, 1L);
	// don't move the cursor if still in the same window
	ಆದರೆ (curwin == old_curwin)
	    curwin->w_cursor = save_cursor;
    }
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_CLIPBOARD)
    ಆದರೆ ((jump_flags & ಒಳ_OTHER_WIN) && !VIsual_ಸಕ್ರಿಯ &&
	    (clip_ನಕ್ಷತ್ರ.ಲಭ್ಯ || clip_plus.ಲಭ್ಯ))
    {
	clip_modeless(which_button, is_click, is_drag);
	ಹಿಂತಿರುಗಿ FALSE;
    }
#ಆದರೆ_ಅಂತ್ಯ

    // Set global flag that we are extending the Visual area with mouse
    // dragging; temporarily minimize 'scrolloff'.
    ಆದರೆ (VIsual_ಸಕ್ರಿಯ && is_drag && get_scrolloff_ಮೌಲ್ಯ())
    {
	// In the very first line, allow scrolling one line
	ಆದರೆ (mouse_ಅಡ್ಡಸಾಲು == 0)
	    mouse_dragging = 2;
	ಇಲ್ಲದಿದ್ದರೆ
	    mouse_dragging = 1;
    }

    // When dragging the mouse above the window, scroll down.
    ಆದರೆ (is_drag && mouse_ಅಡ್ಡಸಾಲು < 0 && !ಒಳ_ಸ್ಥಿತಿ_ಸಾಲು_2)
    {
	scroll_redraw(FALSE, 1L);
	mouse_ಅಡ್ಡಸಾಲು = 0;
    }

    ಆದರೆ (ಆರಂಭ_visual.lnum)		// right click in visual mode
    {
       // When ALT is pressed make Visual mode blockwise.
       ಆದರೆ (mod_ಮುಖವಾಡ & MOD_ಮುಖವಾಡ_ALT)
	   VIsual_ಬಹುಲಕ = Ctrl_V;

	// In Visual-block mode, divide the area in four, pick up the corner
	// that is in the quarter that the cursor is in.
	ಆದರೆ (VIsual_ಬಹುಲಕ == Ctrl_V)
	{
	    getvcols(curwin, &ಆರಂಭ_visual, &ಕೊನೆ_visual,
						    &leftcol, &rightcol, 0);
	    ಆದರೆ (curwin->w_curswant > (leftcol + rightcol) / 2)
		ಕೊನೆ_visual.ಲಂಬಸಾಲು = leftcol;
	    ಇಲ್ಲದಿದ್ದರೆ
		ಕೊನೆ_visual.ಲಂಬಸಾಲು = rightcol;
	    ಆದರೆ (curwin->w_cursor.lnum >=
				    (ಆರಂಭ_visual.lnum + ಕೊನೆ_visual.lnum) / 2)
		ಕೊನೆ_visual.lnum = ಆರಂಭ_visual.lnum;

	    // move VIsual to the right column
	    ಆರಂಭ_visual = curwin->w_cursor;	    // save the cursor pos
	    curwin->w_cursor = ಕೊನೆ_visual;
	    coladvance(ಕೊನೆ_visual.ಲಂಬಸಾಲು);
	    VIsual = curwin->w_cursor;
	    curwin->w_cursor = ಆರಂಭ_visual;	    // restore the cursor
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    // If the click is before the start of visual, change the start.
	    // If the click is after the end of visual, change the end.  If
	    // the click is inside the visual, change the closest side.
	    ಆದರೆ (LT_POS(curwin->w_cursor, ಆರಂಭ_visual))
		VIsual = ಕೊನೆ_visual;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (LT_POS(ಕೊನೆ_visual, curwin->w_cursor))
		VIsual = ಆರಂಭ_visual;
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		// In the same line, compare column number
		ಆದರೆ (ಕೊನೆ_visual.lnum == ಆರಂಭ_visual.lnum)
		{
		    ಆದರೆ (curwin->w_cursor.ಲಂಬಸಾಲು - ಆರಂಭ_visual.ಲಂಬಸಾಲು >
				    ಕೊನೆ_visual.ಲಂಬಸಾಲು - curwin->w_cursor.ಲಂಬಸಾಲು)
			VIsual = ಆರಂಭ_visual;
		    ಇಲ್ಲದಿದ್ದರೆ
			VIsual = ಕೊನೆ_visual;
		}

		// In different lines, compare line number
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    diff = (curwin->w_cursor.lnum - ಆರಂಭ_visual.lnum) -
				(ಕೊನೆ_visual.lnum - curwin->w_cursor.lnum);

		    ಆದರೆ (diff > 0)		// closest to end
			VIsual = ಆರಂಭ_visual;
		    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (diff < 0)	// closest to start
			VIsual = ಕೊನೆ_visual;
		    ಇಲ್ಲದಿದ್ದರೆ			// in the middle line
		    {
			ಆದರೆ (curwin->w_cursor.ಲಂಬಸಾಲು <
					(ಆರಂಭ_visual.ಲಂಬಸಾಲು + ಕೊನೆ_visual.ಲಂಬಸಾಲು) / 2)
			    VIsual = ಕೊನೆ_visual;
			ಇಲ್ಲದಿದ್ದರೆ
			    VIsual = ಆರಂಭ_visual;
		    }
		}
	    }
	}
    }
    // If Visual mode started in insert mode, execute "CTRL-O"
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((State & ಬಹುಲಕ_ಸೇರಿಸು) && VIsual_ಸಕ್ರಿಯ)
	stuffcharReadbuff(Ctrl_O);

    // Middle mouse click: Put text before cursor.
    ಆದರೆ (which_button == MOUSE_ಮಧ್ಯ)
    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CLIPBOARD
	ಆದರೆ (clip_ನಕ್ಷತ್ರ.ಲಭ್ಯ && regname == 0)
	    regname = '*';
#ಆದರೆ_ಅಂತ್ಯ
	ಆದರೆ (yank_ರಿಜಿಸ್ಟರ್_mline(regname))
	{
	    ಆದರೆ (mouse_past_bottom)
		dir = FORWARD;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (mouse_past_eol)
	    dir = FORWARD;

	ಆದರೆ (fixindent)
	{
	    c1 = (dir == BACKWARD) ? '[' : ']';
	    c2 = 'p';
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    c1 = (dir == FORWARD) ? 'p' : 'P';
	    c2 = NUL;
	}
	prep_redo(regname, ಎಣಿಕೆ_2, NUL, c1, NUL, c2, NUL);

	// Remember where the paste started, so in edit() Insstart can be set
	// to this position
	ಆದರೆ (restart_edit != 0)
	    where_paste_started = curwin->w_cursor;
	do_put(regname, ಖಾಲಿ, dir, ಎಣಿಕೆ_2, fixindent | PUT_CURSEND);
    }

#ಆದರೆ defined(FEAT_QUICKFIX)
    // Ctrl-Mouse click or double click in a quickfix window jumps to the
    // error under the mouse pointer.
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (((mod_ಮುಖವಾಡ & MOD_ಮುಖವಾಡ_CTRL)
		|| (mod_ಮುಖವಾಡ & MOD_ಮುಖವಾಡ_MULTI_CLICK) == MOD_ಮುಖವಾಡ_2_CLICK)
	    && bt_quickfix(curbuf))
    {
	ಆದರೆ (curwin->w_llist_ref == ಖಾಲಿ)	// quickfix window
	    do_cmdline_cmd((char_u *)".cc");
	ಇಲ್ಲದಿದ್ದರೆ					// location list window
	    do_cmdline_cmd((char_u *)".ll");
	got_click = FALSE;		// ignore drag&release now
    }
#ಆದರೆ_ಅಂತ್ಯ

    // Ctrl-Mouse click (or double click in a help window) jumps to the tag
    // under the mouse pointer.
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((mod_ಮುಖವಾಡ & MOD_ಮುಖವಾಡ_CTRL) || (curbuf->b_help
		     && (mod_ಮುಖವಾಡ & MOD_ಮುಖವಾಡ_MULTI_CLICK) == MOD_ಮುಖವಾಡ_2_CLICK))
    {
	ಆದರೆ (State & ಬಹುಲಕ_ಸೇರಿಸು)
	    stuffcharReadbuff(Ctrl_O);
	stuffcharReadbuff(Ctrl_RSB);
	got_click = FALSE;		// ignore drag&release now
    }

    // Shift-Mouse click searches for the next occurrence of the word under
    // the mouse pointer
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((mod_ಮುಖವಾಡ & MOD_ಮುಖವಾಡ_ಸರಿಸು))
    {
	ಆದರೆ ((State & ಬಹುಲಕ_ಸೇರಿಸು) || (VIsual_ಸಕ್ರಿಯ && VIsual_select))
	    stuffcharReadbuff(Ctrl_O);
	ಆದರೆ (which_button == MOUSE_ಎಡ)
	    stuffcharReadbuff('*');
	ಇಲ್ಲದಿದ್ದರೆ	// MOUSE_RIGHT
	    stuffcharReadbuff('#');
    }

    // Handle double clicks, unless on status line
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಒಳ_ಸ್ಥಿತಿ_ಸಾಲು_2)
    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MOUSESHAPE
	ಆದರೆ ((is_drag || is_click) && !drag_ಸ್ಥಿತಿ_ಸಾಲು)
	{
	    drag_ಸ್ಥಿತಿ_ಸಾಲು = TRUE;
	    update_mouseshape(-1);
	}
#ಆದರೆ_ಅಂತ್ಯ
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಒಳ_sep_ಸಾಲು)
    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MOUSESHAPE
	ಆದರೆ ((is_drag || is_click) && !drag_sep_ಸಾಲು)
	{
	    drag_sep_ಸಾಲು = TRUE;
	    update_mouseshape(-1);
	}
#ಆದರೆ_ಅಂತ್ಯ
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((mod_ಮುಖವಾಡ & MOD_ಮುಖವಾಡ_MULTI_CLICK)
				       && (State & (ಬಹುಲಕ_ಸಾಮಾನ್ಯ | ಬಹುಲಕ_ಸೇರಿಸು))
	     && mouse_has(MOUSE_VISUAL))
    {
	ಆದರೆ (is_click || !VIsual_ಸಕ್ರಿಯ)
	{
	    ಆದರೆ (VIsual_ಸಕ್ರಿಯ)
		orig_cursor = VIsual;
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		check_visual_highlight();
		VIsual = curwin->w_cursor;
		orig_cursor = VIsual;
		VIsual_ಸಕ್ರಿಯ = TRUE;
		VIsual_reselect = TRUE;
		// start Select mode if 'selectmode' contains "mouse"
		may_ಆರಂಭ_select('o');
		setmouse();
	    }
	    ಆದರೆ ((mod_ಮುಖವಾಡ & MOD_ಮುಖವಾಡ_MULTI_CLICK) == MOD_ಮುಖವಾಡ_2_CLICK)
	    {
		// Double click with ALT pressed makes it blockwise.
		ಆದರೆ (mod_ಮುಖವಾಡ & MOD_ಮುಖವಾಡ_ALT)
		    VIsual_ಬಹುಲಕ = Ctrl_V;
		ಇಲ್ಲದಿದ್ದರೆ
		    VIsual_ಬಹುಲಕ = 'v';
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((mod_ಮುಖವಾಡ & MOD_ಮುಖವಾಡ_MULTI_CLICK) == MOD_ಮುಖವಾಡ_3_CLICK)
		VIsual_ಬಹುಲಕ = 'V';
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((mod_ಮುಖವಾಡ & MOD_ಮುಖವಾಡ_MULTI_CLICK) == MOD_ಮುಖವಾಡ_4_CLICK)
		VIsual_ಬಹುಲಕ = Ctrl_V;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CLIPBOARD
	    // Make sure the clipboard gets updated.  Needed because start and
	    // end may still be the same, and the selection needs to be owned
	    clip_ನಕ್ಷತ್ರ.vmode = NUL;
#ಆದರೆ_ಅಂತ್ಯ
	}
	// A double click selects a word or a block.
	ಆದರೆ ((mod_ಮುಖವಾಡ & MOD_ಮುಖವಾಡ_MULTI_CLICK) == MOD_ಮುಖವಾಡ_2_CLICK)
	{
	    pos_T	*pos = ಖಾಲಿ;
	    ಪೂರ್ಣಾಂಕ		gc;

	    ಆದರೆ (is_click)
	    {
		// If the character under the cursor (skipping white space) is
		// not a word character, try finding a match and select a (),
		// {}, [], #if/#endif, etc. block.
		ಕೊನೆ_visual = curwin->w_cursor;
		ಆಗ (gc = gchar_pos(&ಕೊನೆ_visual), VIM_ISWHITE(gc))
		    inc(&ಕೊನೆ_visual);
		ಆದರೆ (oap != ಖಾಲಿ)
		    oap->motion_type = MCHAR;
		ಆದರೆ (oap != ಖಾಲಿ
			&& VIsual_ಬಹುಲಕ == 'v'
			&& !vim_iswordc(gchar_pos(&ಕೊನೆ_visual))
			&& EQUAL_POS(curwin->w_cursor, VIsual)
			&& (pos = findmatch(oap, NUL)) != ಖಾಲಿ)
		{
		    curwin->w_cursor = *pos;
		    ಆದರೆ (oap->motion_type == MLINE)
			VIsual_ಬಹುಲಕ = 'V';
		    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*p_sel == 'e')
		    {
			ಆದರೆ (LT_POS(curwin->w_cursor, VIsual))
			    ++VIsual.ಲಂಬಸಾಲು;
			ಇಲ್ಲದಿದ್ದರೆ
			    ++curwin->w_cursor.ಲಂಬಸಾಲು;
		    }
		}
	    }

	    ಆದರೆ (pos == ಖಾಲಿ && (is_click || is_drag))
	    {
		// When not found a match or when dragging: extend to include
		// a word.
		ಆದರೆ (LT_POS(curwin->w_cursor, orig_cursor))
		{
		    ಕಂಡುಹಿಡಿ_ಆರಂಭ_ದ_ಶಬ್ದ(&curwin->w_cursor);
		    ಕಂಡುಹಿಡಿ_ಕೊನೆ_ದ_ಶಬ್ದ(&VIsual);
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    ಕಂಡುಹಿಡಿ_ಆರಂಭ_ದ_ಶಬ್ದ(&VIsual);
		    ಆದರೆ (*p_sel == 'e' && *ml_get_cursor() != NUL)
			curwin->w_cursor.ಲಂಬಸಾಲು +=
					 (*mb_ptr2len)(ml_get_cursor());
		    ಕಂಡುಹಿಡಿ_ಕೊನೆ_ದ_ಶಬ್ದ(&curwin->w_cursor);
		}
	    }
	    curwin->w_ಗಣ_curswant = true;
	}
	ಆದರೆ (is_click)
	    redraw_curbuf_later(UPD_INVERTED);	// update the inversion
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (VIsual_ಸಕ್ರಿಯ && !old_ಸಕ್ರಿಯ)
    {
	ಆದರೆ (mod_ಮುಖವಾಡ & MOD_ಮುಖವಾಡ_ALT)
	    VIsual_ಬಹುಲಕ = Ctrl_V;
	ಇಲ್ಲದಿದ್ದರೆ
	    VIsual_ಬಹುಲಕ = 'v';
    }

    // If Visual mode changed show it later.
    ಆದರೆ ((!VIsual_ಸಕ್ರಿಯ && old_ಸಕ್ರಿಯ && ಬಹುಲಕ_displayed)
	    || (VIsual_ಸಕ್ರಿಯ && p_smd && ಸಂದೇಶ_silent == 0
				 && (!old_ಸಕ್ರಿಯ || VIsual_ಬಹುಲಕ != old_ಬಹುಲಕ)))
	redraw_cmdline = TRUE;

    ಹಿಂತಿರುಗಿ moved;
}

    ಶೂನ್ಯ
ins_mouse(ಪೂರ್ಣಾಂಕ c)
{
    pos_T	tpos;
    win_T	*old_curwin = curwin;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
    // When GUI is active, also move/paste when 'mouse' is empty
    ಆದರೆ (!gui.ಒಳ_use)
#ಆದರೆ_ಅಂತ್ಯ
	ಆದರೆ (!mouse_has(MOUSE_ಸೇರಿಸು))
	    ಹಿಂತಿರುಗಿ;

    undisplay_dollar();
    tpos = curwin->w_cursor;
    ಆದರೆ (do_mouse(ಖಾಲಿ, c, BACKWARD, 1L, 0))
    {
	win_T	*new_curwin = curwin;

	ಆದರೆ (curwin != old_curwin && win_valid(old_curwin))
	{
	    // Mouse took us to another window.  We need to go back to the
	    // previous one to stop insert there properly.
	    curwin = old_curwin;
	    curbuf = curwin->w_ಬಫರ್;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಕೆಲಸ_CHANNEL
	    ಆದರೆ (bt_prompt(curbuf))
		// Restart Insert mode when re-entering the prompt buffer.
		curbuf->b_prompt_ಸೇರಿಸು = 'A';
#ಆದರೆ_ಅಂತ್ಯ
	}
	ಆರಂಭ_arrow(curwin == old_curwin ? &tpos : ಖಾಲಿ);
	ಆದರೆ (curwin != new_curwin && win_valid(new_curwin))
	{
	    curwin = new_curwin;
	    curbuf = curwin->w_ಬಫರ್;
	}
	ಗಣ_can_cindent(TRUE);
    }

    // redraw status lines (in case another window became active)
    redraw_statuslines();
}

/*
 * Common mouse wheel scrolling, shared between Insert mode and NV modes.
 * Default action is to scroll mouse_vert_step lines (or mouse_hor_step columns
 * depending on the scroll direction) or one page when Shift or Ctrl is used.
 * Direction is indicated by "cap->arg":
 *    K_MOUSEUP    - MSCR_UP
 *    K_MOUSEDOWN  - MSCR_DOWN
 *    K_MOUSELEFT  - MSCR_LEFT
 *    K_MOUSERIGHT - MSCR_RIGHT
 * "curwin" may have been changed to the window that should be scrolled and
 * differ from the window that actually has focus.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
do_mousescroll(cmdarg_T *cap)
{
    ಪೂರ್ಣಾಂಕ ಸರಿಸು_or_ctrl = mod_ಮುಖವಾಡ & (MOD_ಮುಖವಾಡ_ಸರಿಸು | MOD_ಮುಖವಾಡ_CTRL);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMINAL
    ಆದರೆ (ಪದ_use_loop())
	// This window is a terminal window, send the mouse event there.
	// Set "typed" to FALSE to avoid an endless loop.
	send_ಕೀಲಿಗಳು_ಗಮ್ಯ_ಪದ(curbuf->b_ಪದ, cap->cmdchar, mod_ಮುಖವಾಡ, FALSE);
    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ (cap->arg == MSCR_ಮೇಲೆ || cap->arg == MSCR_ಕೆಳಕ್ಕೆ)
    {
	// Vertical scrolling
	ಆದರೆ (!(State & ಬಹುಲಕ_ಸೇರಿಸು) && (mouse_vert_ಹೆಜ್ಜೆ < 0 || ಸರಿಸು_or_ctrl))
	{
	    // whole page up or down
	    pagescroll(cap->arg == MSCR_ಮೇಲೆ ? FORWARD : BACKWARD, 1L, FALSE);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಆದರೆ (mouse_vert_ಹೆಜ್ಜೆ < 0 || ಸರಿಸು_or_ctrl)
	    {
		// whole page up or down
		cap->count1 = (ದೀರ್ಘ)(curwin->w_botline - curwin->w_topline);
	    }
	    // Don't scroll more than half the window height.
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (curwin->w_ಎತ್ತರ < mouse_vert_ಹೆಜ್ಜೆ * 2)
	    {
		cap->count1 = curwin->w_ಎತ್ತರ / 2;
		ಆದರೆ (cap->count1 == 0)
		    cap->count1 = 1;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		cap->count1 = mouse_vert_ಹೆಜ್ಜೆ;
	    }
	    cap->count0 = cap->count1;
	    nv_scroll_ಸಾಲು(cap);
	}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
	ಆದರೆ (WIN_IS_POPUP(curwin))
	    popup_ಗಣ_firstline(curwin);
#ಆದರೆ_ಅಂತ್ಯ
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// Horizontal scrolling
	ದೀರ್ಘ ಹೆಜ್ಜೆ = (mouse_hor_ಹೆಜ್ಜೆ < 0 || ಸರಿಸು_or_ctrl)
					    ? curwin->w_ಅಗಲ : mouse_hor_ಹೆಜ್ಜೆ;
	ದೀರ್ಘ leftcol = curwin->w_leftcol
				     + (cap->arg == MSCR_ಬಲ ? -ಹೆಜ್ಜೆ : ಹೆಜ್ಜೆ);
	ಆದರೆ (leftcol < 0)
	    leftcol = 0;
	do_mousescroll_horiz((long_u)leftcol);
    }
    may_trigger_win_scrolled_resized();
}

/*
 * Insert mode implementation for scrolling in direction "dir", which is
 * one of the MSCR_ values.
 */
    ಶೂನ್ಯ
ins_mousescroll(ಪೂರ್ಣಾಂಕ dir)
{
    cmdarg_T	cap;
    oparg_T	oa;
    CLEAR_ಕ್ಷೇತ್ರ(cap);
    clear_oparg(&oa);
    cap.oap = &oa;
    cap.arg = dir;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TABPANEL
    ಆದರೆ (mouse_ಅಡ್ಡಸಾಲು >= 0 && mouse_ಲಂಬಸಾಲು >= 0
	    && (dir == MSCR_ಮೇಲೆ || dir == MSCR_ಕೆಳಕ್ಕೆ)
	    && mouse_on_tabpanel())
    {
	(ಶೂನ್ಯ)tabpanel_scroll(dir == MSCR_ಮೇಲೆ ? 1 : -1,
		mouse_vert_ಹೆಜ್ಜೆ > 0 ? mouse_vert_ಹೆಜ್ಜೆ : 3);
	ಹಿಂತಿರುಗಿ;
    }
#ಆದರೆ_ಅಂತ್ಯ

    ಆಯ್ಕೆ (dir)
    {
	ಸ್ಥಿತಿ MSCR_ಮೇಲೆ:
	    cap.cmdchar = K_MOUSEUP;
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ MSCR_ಕೆಳಕ್ಕೆ:
	    cap.cmdchar = K_MOUSEDOWN;
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ MSCR_ಎಡ:
	    cap.cmdchar = K_MOUSELEFT;
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ MSCR_ಬಲ:
	    cap.cmdchar = K_MOUSERIGHT;
	    ನಿಲ್ಲಿಸು;
	ಪೂರ್ವನಿಯೋಜಿತ:
	    siemsg("Invalid ins_mousescroll() argument: %d", dir);
    }

    win_T *old_curwin = curwin;
    ಆದರೆ (mouse_ಅಡ್ಡಸಾಲು >= 0 && mouse_ಲಂಬಸಾಲು >= 0)
    {
	// Find the window at the mouse pointer coordinates.
	// NOTE: Must restore "curwin" to "old_curwin" before returning!
	ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲು = mouse_ಅಡ್ಡಸಾಲು;
	ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು = mouse_ಲಂಬಸಾಲು;
	curwin = mouse_ಕಂಡುಹಿಡಿ_win(&ಅಡ್ಡಸಾಲು, &ಲಂಬಸಾಲು, ಕಂಡುಹಿಡಿ_POPUP);
	ಆದರೆ (curwin == ಖಾಲಿ)
	{
	    curwin = old_curwin;
	    ಹಿಂತಿರುಗಿ;
	}
	curbuf = curwin->w_ಬಫರ್;
    }

    ಆದರೆ (curwin == old_curwin)
    {
	// Don't scroll the current window if the popup menu is visible.
	ಆದರೆ (pum_visible())
	    ಹಿಂತಿರುಗಿ;

	undisplay_dollar();
    }

    linenr_T	orig_topline = curwin->w_topline;
    colnr_T	orig_leftcol = curwin->w_leftcol;
    pos_T	orig_cursor = curwin->w_cursor;

    // Call the common mouse scroll function shared with other modes.
    do_mousescroll(&cap);

    ಪೂರ್ಣಾಂಕ did_scroll = (orig_topline != curwin->w_topline
		   || orig_leftcol != curwin->w_leftcol);

    curwin->w_redr_ಸ್ಥಿತಿ = true;
    curwin = old_curwin;
    curbuf = curwin->w_ಬಫರ್;

    // If the window actually scrolled and the popup menu may overlay the
    // window, need to redraw it.
    ಆದರೆ (did_scroll && pum_visible())
    {
	// TODO: Would be more efficient to only redraw the windows that are
	// overlapped by the popup menu.
	redraw_all_later(UPD_NOT_VALID);
	ins_compl_show_pum();
    }

    ಆದರೆ (!EQUAL_POS(curwin->w_cursor, orig_cursor))
    {
	ಆರಂಭ_arrow(&orig_cursor);
	ಗಣ_can_cindent(TRUE);
    }
}

#ಆದರೆ defined(FEAT_PROP_POPUP)
/*
 * Command-line mode implementation for scrolling in direction "dir", which is
 * one of the MSCR_ values.  Scrolls the completion info popup when the mouse
 * pointer is on top of it.
 * Returns TRUE when the info popup was scrolled.
 */
    ಪೂರ್ಣಾಂಕ
cmdline_mousescroll(ಪೂರ್ಣಾಂಕ dir)
{
    cmdarg_T	cap;
    oparg_T	oa;

    CLEAR_ಕ್ಷೇತ್ರ(cap);
    clear_oparg(&oa);
    cap.oap = &oa;
    cap.arg = dir;

    ಆಯ್ಕೆ (dir)
    {
	ಸ್ಥಿತಿ MSCR_ಮೇಲೆ:	 cap.cmdchar = K_MOUSEUP; ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ MSCR_ಕೆಳಕ್ಕೆ:	 cap.cmdchar = K_MOUSEDOWN; ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ MSCR_ಎಡ:	 cap.cmdchar = K_MOUSELEFT; ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ MSCR_ಬಲ: cap.cmdchar = K_MOUSERIGHT; ನಿಲ್ಲಿಸು;
    }

    ಆದರೆ (mouse_ಅಡ್ಡಸಾಲು < 0 || mouse_ಲಂಬಸಾಲು < 0)
	ಹಿಂತಿರುಗಿ FALSE;

    ಪೂರ್ಣಾಂಕ	    ಅಡ್ಡಸಾಲು = mouse_ಅಡ್ಡಸಾಲು;
    ಪೂರ್ಣಾಂಕ	    ಲಂಬಸಾಲು = mouse_ಲಂಬಸಾಲು;
    win_T   *wp;

    // Only scroll when the mouse is on top of the info popup.
    wp = mouse_ಕಂಡುಹಿಡಿ_win(&ಅಡ್ಡಸಾಲು, &ಲಂಬಸಾಲು, ಕಂಡುಹಿಡಿ_POPUP);
    ಆದರೆ (wp == ಖಾಲಿ || !WIN_IS_POPUP(wp) || !(wp->w_popup_flags & POPF_INFO)
							|| !wp->w_has_scrollbar)
	ಹಿಂತಿರುಗಿ FALSE;

    win_T   *old_curwin = curwin;

    curwin = wp;
    curbuf = wp->w_ಬಫರ್;
    // Call the common mouse scroll function shared with other modes.
    do_mousescroll(&cap);
    curwin = old_curwin;
    curbuf = curwin->w_ಬಫರ್;

    // Cmdline mode doesn't normally call update_screen(), so redraw the
    // completion popup menu, which also repaints the info popup.
    ಆದರೆ (cmdline_pum_ಸಕ್ರಿಯ())
	cmdline_pum_display();
    ಹಿಂತಿರುಗಿ TRUE;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Return TRUE if "c" is a mouse key.
 */
    ಪೂರ್ಣಾಂಕ
is_mouse_ಕೀಲಿ(ಪೂರ್ಣಾಂಕ c)
{
    ಹಿಂತಿರುಗಿ c == K_LEFTMOUSE
	|| c == K_LEFTMOUSE_NM
	|| c == K_LEFTDRAG
	|| c == K_LEFTRELEASE
	|| c == K_LEFTRELEASE_NM
	|| c == K_MOUSEMOVE
	|| c == K_MIDDLEMOUSE
	|| c == K_MIDDLEDRAG
	|| c == K_MIDDLERELEASE
	|| c == K_RIGHTMOUSE
	|| c == K_RIGHTDRAG
	|| c == K_RIGHTRELEASE
	|| c == K_MOUSEDOWN
	|| c == K_MOUSEUP
	|| c == K_MOUSELEFT
	|| c == K_MOUSERIGHT
	|| c == K_X1MOUSE
	|| c == K_X1DRAG
	|| c == K_X1RELEASE
	|| c == K_X2MOUSE
	|| c == K_X2DRAG
	|| c == K_X2RELEASE;
}

ಸ್ಥಿರ ರಚನೆ mousetable
{
    ಪೂರ್ಣಾಂಕ	    pseudo_ಸಂಕೇತ;	// Code for pseudo mouse event
    ಪೂರ್ಣಾಂಕ	    button;		// Which mouse button is it?
    ಪೂರ್ಣಾಂಕ	    is_click;		// Is it a mouse button click event?
    ಪೂರ್ಣಾಂಕ	    is_drag;		// Is it a mouse drag event?
} mouse_ಕೋಷ್ಟಕ[] =
{
    {(ಪೂರ್ಣಾಂಕ)KE_LEFTMOUSE,		MOUSE_ಎಡ,	TRUE,	FALSE},
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
    {(ಪೂರ್ಣಾಂಕ)KE_LEFTMOUSE_NM,	MOUSE_ಎಡ,	TRUE,	FALSE},
#ಆದರೆ_ಅಂತ್ಯ
    {(ಪೂರ್ಣಾಂಕ)KE_LEFTDRAG,		MOUSE_ಎಡ,	FALSE,	TRUE},
    {(ಪೂರ್ಣಾಂಕ)KE_LEFTRELEASE,	MOUSE_ಎಡ,	FALSE,	FALSE},
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
    {(ಪೂರ್ಣಾಂಕ)KE_LEFTRELEASE_NM,	MOUSE_ಎಡ,	FALSE,	FALSE},
#ಆದರೆ_ಅಂತ್ಯ
    {(ಪೂರ್ಣಾಂಕ)KE_MIDDLEMOUSE,	MOUSE_ಮಧ್ಯ,	TRUE,	FALSE},
    {(ಪೂರ್ಣಾಂಕ)KE_MIDDLEDRAG,	MOUSE_ಮಧ್ಯ,	FALSE,	TRUE},
    {(ಪೂರ್ಣಾಂಕ)KE_MIDDLERELEASE,	MOUSE_ಮಧ್ಯ,	FALSE,	FALSE},
    {(ಪೂರ್ಣಾಂಕ)KE_RIGHTMOUSE,	MOUSE_ಬಲ,	TRUE,	FALSE},
    {(ಪೂರ್ಣಾಂಕ)KE_RIGHTDRAG,		MOUSE_ಬಲ,	FALSE,	TRUE},
    {(ಪೂರ್ಣಾಂಕ)KE_RIGHTRELEASE,	MOUSE_ಬಲ,	FALSE,	FALSE},
    {(ಪೂರ್ಣಾಂಕ)KE_X1MOUSE,		MOUSE_X1,	TRUE,	FALSE},
    {(ಪೂರ್ಣಾಂಕ)KE_X1DRAG,		MOUSE_X1,	FALSE,	TRUE},
    {(ಪೂರ್ಣಾಂಕ)KE_X1RELEASE,		MOUSE_X1,	FALSE,	FALSE},
    {(ಪೂರ್ಣಾಂಕ)KE_X2MOUSE,		MOUSE_X2,	TRUE,	FALSE},
    {(ಪೂರ್ಣಾಂಕ)KE_X2DRAG,		MOUSE_X2,	FALSE,	TRUE},
    {(ಪೂರ್ಣಾಂಕ)KE_X2RELEASE,		MOUSE_X2,	FALSE,	FALSE},
    // DRAG without CLICK
    {(ಪೂರ್ಣಾಂಕ)KE_MOUSEMOVE,		MOUSE_RELEASE,	FALSE,	TRUE},
    // RELEASE without CLICK
    {(ಪೂರ್ಣಾಂಕ)KE_IGNORE,		MOUSE_RELEASE,	FALSE,	FALSE},
    {0,				0,		0,	0},
};

/*
 * Look up the given mouse code to return the relevant information in the other
 * arguments.  Return which button is down or was released.
 */
    ಪೂರ್ಣಾಂಕ
get_mouse_button(ಪೂರ್ಣಾಂಕ ಸಂಕೇತ_2, ಪೂರ್ಣಾಂಕ *is_click, ಪೂರ್ಣಾಂಕ *is_drag)
{
    ಪೂರ್ಣಾಂಕ	    i;

    ಗಾಗಿ (i = 0; mouse_ಕೋಷ್ಟಕ[i].pseudo_ಸಂಕೇತ; i++)
	ಆದರೆ (ಸಂಕೇತ_2 == mouse_ಕೋಷ್ಟಕ[i].pseudo_ಸಂಕೇತ)
	{
	    *is_click = mouse_ಕೋಷ್ಟಕ[i].is_click;
	    *is_drag = mouse_ಕೋಷ್ಟಕ[i].is_drag;
	    ಹಿಂತಿರುಗಿ mouse_ಕೋಷ್ಟಕ[i].button;
	}
    ಹಿಂತಿರುಗಿ 0;	    // Shouldn't get here
}

/*
 * Return the appropriate pseudo mouse event token (KE_LEFTMOUSE etc) based on
 * the given information about which mouse button is down, and whether the
 * mouse was clicked, dragged or released.
 */
    ಪೂರ್ಣಾಂಕ
get_pseudo_mouse_ಸಂಕೇತ(
    ಪೂರ್ಣಾಂಕ	    button,	// eg MOUSE_LEFT
    ಪೂರ್ಣಾಂಕ	    is_click,
    ಪೂರ್ಣಾಂಕ	    is_drag)
{
    ಪೂರ್ಣಾಂಕ	    i;

    ಗಾಗಿ (i = 0; mouse_ಕೋಷ್ಟಕ[i].pseudo_ಸಂಕೇತ; i++)
	ಆದರೆ (button == mouse_ಕೋಷ್ಟಕ[i].button
	    && is_click == mouse_ಕೋಷ್ಟಕ[i].is_click
	    && is_drag == mouse_ಕೋಷ್ಟಕ[i].is_drag)
	{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
	    // Trick: a non mappable left click and release has mouse_col -1
	    // or added MOUSE_COLOFF.  Used for 'mousefocus' in
	    // gui_mouse_moved()
	    ಆದರೆ (mouse_ಲಂಬಸಾಲು < 0 || mouse_ಲಂಬಸಾಲು > MOUSE_COLOFF)
	    {
		ಆದರೆ (mouse_ಲಂಬಸಾಲು < 0)
		    mouse_ಲಂಬಸಾಲು = 0;
		ಇಲ್ಲದಿದ್ದರೆ
		    mouse_ಲಂಬಸಾಲು -= MOUSE_COLOFF;
		ಆದರೆ (mouse_ಕೋಷ್ಟಕ[i].pseudo_ಸಂಕೇತ == (ಪೂರ್ಣಾಂಕ)KE_LEFTMOUSE)
		    ಹಿಂತಿರುಗಿ (ಪೂರ್ಣಾಂಕ)KE_LEFTMOUSE_NM;
		ಆದರೆ (mouse_ಕೋಷ್ಟಕ[i].pseudo_ಸಂಕೇತ == (ಪೂರ್ಣಾಂಕ)KE_LEFTRELEASE)
		    ಹಿಂತಿರುಗಿ (ಪೂರ್ಣಾಂಕ)KE_LEFTRELEASE_NM;
	    }
#ಆದರೆ_ಅಂತ್ಯ
	    ಹಿಂತಿರುಗಿ mouse_ಕೋಷ್ಟಕ[i].pseudo_ಸಂಕೇತ;
	}
    ಹಿಂತಿರುಗಿ (ಪೂರ್ಣಾಂಕ)KE_IGNORE;	    // not recognized, ignore it
}

#ವ್ಯಾಖ್ಯಾನಿಸು HMT_ಸಾಮಾನ್ಯ	1
#ವ್ಯಾಖ್ಯಾನಿಸು HMT_NETTERM	2
#ವ್ಯಾಖ್ಯಾನಿಸು HMT_DEC	4
#ವ್ಯಾಖ್ಯಾನಿಸು HMT_JSBTERM	8
#ವ್ಯಾಖ್ಯಾನಿಸು HMT_PTERM	16
#ವ್ಯಾಖ್ಯಾನಿಸು HMT_URXVT	32
#ವ್ಯಾಖ್ಯಾನಿಸು HMT_GPM	64
#ವ್ಯಾಖ್ಯಾನಿಸು HMT_SGR	128
#ವ್ಯಾಖ್ಯಾನಿಸು HMT_SGR_REL	256
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ has_mouse_termcode = 0;

    ಶೂನ್ಯ
ಗಣ_mouse_termcode(
    ಪೂರ್ಣಾಂಕ		n,	// KS_MOUSE, KS_NETTERM_MOUSE or KS_DEC_MOUSE
    char_u	*s)
{
    char_u	name[2];

    name[0] = n;
    name[1] = KE_FILLER;
    ಕೂಡಿಸು_termcode(name, s, FALSE);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MOUSE_JSB
    ಆದರೆ (n == KS_JSBTERM_MOUSE)
	has_mouse_termcode |= HMT_JSBTERM;
    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MOUSE_NET
    ಆದರೆ (n == KS_NETTERM_MOUSE)
	has_mouse_termcode |= HMT_NETTERM;
    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MOUSE_DEC
    ಆದರೆ (n == KS_DEC_MOUSE)
	has_mouse_termcode |= HMT_DEC;
    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MOUSE_PTERM
    ಆದರೆ (n == KS_PTERM_MOUSE)
	has_mouse_termcode |= HMT_PTERM;
    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MOUSE_URXVT
    ಆದರೆ (n == KS_URXVT_MOUSE)
	has_mouse_termcode |= HMT_URXVT;
    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MOUSE_GPM
    ಆದರೆ (n == KS_GPM_MOUSE)
	has_mouse_termcode |= HMT_GPM;
    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ (n == KS_SGR_MOUSE)
	has_mouse_termcode |= HMT_SGR;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (n == KS_SGR_MOUSE_RELEASE)
	has_mouse_termcode |= HMT_SGR_REL;
    ಇಲ್ಲದಿದ್ದರೆ
	has_mouse_termcode |= HMT_ಸಾಮಾನ್ಯ;
}

#ಆದರೆ defined(UNIX) || defined(VMS)
    ಶೂನ್ಯ
del_mouse_termcode(
    ಪೂರ್ಣಾಂಕ		n)	// KS_MOUSE, KS_NETTERM_MOUSE or KS_DEC_MOUSE
{
    char_u	name[2];

    name[0] = n;
    name[1] = KE_FILLER;
    del_termcode(name);
# ifdef FEAT_MOUSE_JSB
    ಆದರೆ (n == KS_JSBTERM_MOUSE)
	has_mouse_termcode &= ~HMT_JSBTERM;
    ಇಲ್ಲದಿದ್ದರೆ
# endif
# ifdef FEAT_MOUSE_NET
    ಆದರೆ (n == KS_NETTERM_MOUSE)
	has_mouse_termcode &= ~HMT_NETTERM;
    ಇಲ್ಲದಿದ್ದರೆ
# endif
# ifdef FEAT_MOUSE_DEC
    ಆದರೆ (n == KS_DEC_MOUSE)
	has_mouse_termcode &= ~HMT_DEC;
    ಇಲ್ಲದಿದ್ದರೆ
# endif
# ifdef FEAT_MOUSE_PTERM
    ಆದರೆ (n == KS_PTERM_MOUSE)
	has_mouse_termcode &= ~HMT_PTERM;
    ಇಲ್ಲದಿದ್ದರೆ
# endif
# ifdef FEAT_MOUSE_URXVT
    ಆದರೆ (n == KS_URXVT_MOUSE)
	has_mouse_termcode &= ~HMT_URXVT;
    ಇಲ್ಲದಿದ್ದರೆ
# endif
# ifdef FEAT_MOUSE_GPM
    ಆದರೆ (n == KS_GPM_MOUSE)
	has_mouse_termcode &= ~HMT_GPM;
    ಇಲ್ಲದಿದ್ದರೆ
# endif
    ಆದರೆ (n == KS_SGR_MOUSE)
	has_mouse_termcode &= ~HMT_SGR;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (n == KS_SGR_MOUSE_RELEASE)
	has_mouse_termcode &= ~HMT_SGR_REL;
    ಇಲ್ಲದಿದ್ದರೆ
	has_mouse_termcode &= ~HMT_ಸಾಮಾನ್ಯ;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * setmouse() - switch mouse on/off depending on current mode and 'mouse'
 */
    ಶೂನ್ಯ
setmouse(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ	    checkfor;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MOUSESHAPE
    update_mouseshape(-1);
#ಆದರೆ_ಅಂತ್ಯ

    // Should be outside proc, but may break MOUSESHAPE
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
    // In the GUI the mouse is always enabled.
    ಆದರೆ (gui.ಒಳ_use)
	ಹಿಂತಿರುಗಿ;
#ಆದರೆ_ಅಂತ್ಯ
    // be quick when mouse is off
    ಆದರೆ (*p_mouse == NUL || has_mouse_termcode == 0)
	ಹಿಂತಿರುಗಿ;

    // don't switch mouse on when not in raw mode (Ex mode)
    ಆದರೆ (cur_tmode != TMODE_RAW)
    {
	mch_setmouse(FALSE);
	ಹಿಂತಿರುಗಿ;
    }

    ಆದರೆ (VIsual_ಸಕ್ರಿಯ)
	checkfor = MOUSE_VISUAL;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (State == ಬಹುಲಕ_HITRETURN || State == ಬಹುಲಕ_ASKMORE
						     || State == ಬಹುಲಕ_SETWSIZE)
	checkfor = MOUSE_RETURN;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (State & ಬಹುಲಕ_ಸೇರಿಸು)
	checkfor = MOUSE_ಸೇರಿಸು;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (State & ಬಹುಲಕ_CMDLINE)
	checkfor = MOUSE_COMMAND;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (State == ಬಹುಲಕ_CONFIRM || State == ಬಹುಲಕ_EXTERNCMD)
	checkfor = ' '; // don't use mouse for ":confirm" or ":!cmd"
    ಇಲ್ಲದಿದ್ದರೆ
	checkfor = MOUSE_ಸಾಮಾನ್ಯ;    // assume normal mode

    ಆದರೆ (mouse_has(checkfor))
	mch_setmouse(TRUE);
    ಇಲ್ಲದಿದ್ದರೆ
	mch_setmouse(FALSE);
}

/*
 * Return TRUE if
 * - "c" is in 'mouse', or
 * - 'a' is in 'mouse' and "c" is in MOUSE_A, or
 * - the current buffer is a help file and 'h' is in 'mouse' and we are in a
 *   normal editing mode (not at hit-return message).
 */
    ಪೂರ್ಣಾಂಕ
mouse_has(ಪೂರ್ಣಾಂಕ c)
{
    char_u	*p;

    ಗಾಗಿ (p = p_mouse; *p; ++p)
	ಆಯ್ಕೆ (*p)
	{
	    ಸ್ಥಿತಿ 'a': ಆದರೆ (vim_strchr((char_u *)MOUSE_A, c) != ಖಾಲಿ)
			  ಹಿಂತಿರುಗಿ TRUE;
		      ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ MOUSE_HELP: ಆದರೆ (c != MOUSE_RETURN && curbuf->b_help)
				 ಹಿಂತಿರುಗಿ TRUE;
			     ನಿಲ್ಲಿಸು;
	    ಪೂರ್ವನಿಯೋಜಿತ: ಆದರೆ (c == *p) ಹಿಂತಿರುಗಿ TRUE; ನಿಲ್ಲಿಸು;
	}
    ಹಿಂತಿರುಗಿ FALSE;
}

/*
 * Return TRUE when 'mousemodel' is set to "popup" or "popup_setpos".
 */
    ಪೂರ್ಣಾಂಕ
mouse_model_popup(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ (p_mousem[0] == 'p');
}

/*
 * Call a click-region callback function.
 * "regions"/"region_count" describe the resolved click regions,
 * "winid" is stored as the "winid" key in the info dict (0 for tabline).
 * Returns TRUE if the function was called and handled the click.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
stl_click_handler_regions(
	stl_click_region_T  *regions,
	ಪೂರ್ಣಾಂಕ		    region_ಎಣಿಕೆ,
	ಪೂರ್ಣಾಂಕ		    winid,
	char_u		    *area_name,
	ಪೂರ್ಣಾಂಕ		    mrow,
	ಪೂರ್ಣಾಂಕ		    mcol,
	ಪೂರ್ಣಾಂಕ		    which_button,
	ಪೂರ್ಣಾಂಕ		    mods)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    ಪೂರ್ಣಾಂಕ		n;
    ಪೂರ್ಣಾಂಕ		nclicks;
    char_u	button_str[2];
    char_u	mods_str[4];
    ಪೂರ್ಣಾಂಕ		mi = 0;
    dict_T	*info;
    typval_T	argvars[2];
    typval_T	rettv;
    funcexe_T	funcexe;
    ಪೂರ್ಣಾಂಕ		ಲಂಬಸಾಲು = mcol;

    ಆದರೆ (regions == ಖಾಲಿ || region_ಎಣಿಕೆ == 0)
	ಹಿಂತಿರುಗಿ FALSE;

    // Find the click region at the given row and column.
    ಗಾಗಿ (n = 0; n < region_ಎಣಿಕೆ; n++)
    {
	ಆದರೆ (regions[n].ಅಡ್ಡಸಾಲು == mrow
		&& ಲಂಬಸಾಲು >= regions[n].ಲಂಬಸಾಲು_ಆರಂಭ
		&& ಲಂಬಸಾಲು < regions[n].ಲಂಬಸಾಲು_ಕೊನೆ)
	    ನಿಲ್ಲಿಸು;
    }
    ಆದರೆ (n >= region_ಎಣಿಕೆ || regions[n].funcname == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FALSE;

    // Build the info dictionary.
    info = dict_alloc();
    ಆದರೆ (info == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FALSE;

    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(info, "minwid", regions[n].minwid);

    // Determine number of clicks.
    // MOD_MASK_2CLICK=0x20, MOD_MASK_3CLICK=0x40, MOD_MASK_4CLICK=0x60
    nclicks = ((mods & MOD_ಮುಖವಾಡ_MULTI_CLICK) >> 5) + 1;
    ಆದರೆ (nclicks > 3)
	nclicks = 3;
    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(info, "nclicks", nclicks);

    // Button.
    ಆದರೆ (which_button == MOUSE_ಎಡ)
	button_str[0] = 'l';
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (which_button == MOUSE_ಬಲ)
	button_str[0] = 'r';
    ಇಲ್ಲದಿದ್ದರೆ
	button_str[0] = 'm';
    button_str[1] = NUL;
    dict_ಕೂಡಿಸು_string_len(info, "button", button_str, 1);

    // Modifiers.
    ಆದರೆ (mods & MOD_ಮುಖವಾಡ_ಸರಿಸು)
	mods_str[mi++] = 's';
    ಆದರೆ (mods & MOD_ಮುಖವಾಡ_CTRL)
	mods_str[mi++] = 'c';
    ಆದರೆ (mods & MOD_ಮುಖವಾಡ_ALT)
	mods_str[mi++] = 'a';
    mods_str[mi] = NUL;
    dict_ಕೂಡಿಸು_string_len(info, "mods", mods_str, mi);

    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(info, "winid", winid);

    // "area": which option the clicked region belongs to.  Lets a shared
    // dispatcher distinguish 'statusline', 'tabline' and 'tabpanel' without
    // having to overload winid == 0.
    dict_ಕೂಡಿಸು_string(info, "area", area_name);

    // Expose tab page number for 'tabpanel' regions.
    ಆದರೆ (regions[n].tabnr > 0)
	dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(info, "tabnr", regions[n].tabnr);

    // Call the function with the info dict as argument.
    argvars[0].v_type = VAR_DICT;
    argvars[0].vval.v_dict = info;
    ++info->dv_refcount;
    argvars[1].v_type = VAR_UNKNOWN;

    rettv.v_type = VAR_ಸಂಖ್ಯೆ;
    rettv.vval.v_ಸಂಖ್ಯೆ = 0;

    CLEAR_ಕ್ಷೇತ್ರ(funcexe);
    funcexe.fe_evaluate = TRUE;
    (ಶೂನ್ಯ)call_func(regions[n].funcname, -1,
					    &rettv, 1, argvars, &funcexe);

    n = (ಪೂರ್ಣಾಂಕ)rettv.vval.v_ಸಂಖ್ಯೆ;
    clear_tv(&rettv);
    dict_unref(info);

    ಆದರೆ (n != 0)
    {
	// Make sure the tabline gets redrawn too when the callback asks for
	// a redraw (redraw_statuslines() only redraws the tabline when
	// redraw_tabline is set).  For tabpanel the whole screen needs to be
	// refreshed.
	ಆದರೆ (winid == 0)
	    redraw_tabline = TRUE;
# ifdef FEAT_TABPANEL
	ಆದರೆ (STRCMP(area_name, "tabpanel") == 0)
	    redraw_all_later(UPD_NOT_VALID);
# endif
	redraw_statuslines();
    }

    ಹಿಂತಿರುಗಿ TRUE;
#ಇಲ್ಲದಿದ್ದರೆ
    (ಶೂನ್ಯ)regions;
    (ಶೂನ್ಯ)region_ಎಣಿಕೆ;
    (ಶೂನ್ಯ)winid;
    (ಶೂನ್ಯ)area_name;
    (ಶೂನ್ಯ)mrow;
    (ಶೂನ್ಯ)mcol;
    (ಶೂನ್ಯ)which_button;
    (ಶೂನ್ಯ)mods;
    ಹಿಂತಿರುಗಿ FALSE;
#ಆದರೆ_ಅಂತ್ಯ
}

/*
 * Call a statusline click handler function for window "wp".
 * Returns TRUE if the function was called and handled the click.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
stl_click_handler(win_T *wp, ಪೂರ್ಣಾಂಕ mrow, ಪೂರ್ಣಾಂಕ mcol, ಪೂರ್ಣಾಂಕ which_button, ಪೂರ್ಣಾಂಕ mods)
{
    ಆದರೆ (wp == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FALSE;
    ಹಿಂತಿರುಗಿ stl_click_handler_regions(wp->w_stl_click, wp->w_stl_click_ಎಣಿಕೆ,
				wp->w_id, (char_u *)"statusline",
				mrow, mcol, which_button, mods);
}

// dragwin is declared near the top of the file

/*
 * Reset the window being dragged.  To be called when switching tab page.
 */
    ಶೂನ್ಯ
reset_dragwin(ಶೂನ್ಯ)
{
    dragwin = ಖಾಲಿ;
}

/*
 * Move the cursor to the specified row and column on the screen.
 * Change current window if necessary.	Returns an integer with the
 * CURSOR_MOVED bit set if the cursor has moved or unset otherwise.
 *
 * The MOUSE_FOLD_CLOSE bit is set when clicked on the '-' in a fold column.
 * The MOUSE_FOLD_OPEN bit is set when clicked on the '+' in a fold column.
 *
 * If flags has MOUSE_FOCUS, then the current window will not be changed, and
 * if the mouse is outside the window then the text will scroll, or if the
 * mouse was previously on a status line, then the status line may be dragged.
 *
 * If flags has MOUSE_MAY_VIS, then VIsual mode will be started before the
 * cursor is moved unless the cursor was on a status line.
 * This function returns one of IN_UNKNOWN, IN_BUFFER, IN_STATUS_LINE or
 * IN_SEP_LINE depending on where the cursor was clicked.
 *
 * If flags has MOUSE_MAY_STOP_VIS, then Visual mode will be stopped, unless
 * the mouse is on the status line of the same window.
 *
 * If flags has MOUSE_DID_MOVE, nothing is done if the mouse didn't move since
 * the last call.
 *
 * If flags has MOUSE_SETPOS, nothing is done, only the current position is
 * remembered.
 */
    ಪೂರ್ಣಾಂಕ
jump_ಗಮ್ಯ_mouse(
    ಪೂರ್ಣಾಂಕ		flags,
    ಪೂರ್ಣಾಂಕ		*inclusive,	// used for inclusive operator, can be NULL
    ಪೂರ್ಣಾಂಕ		which_button)	// MOUSE_LEFT, MOUSE_RIGHT, MOUSE_MIDDLE
{
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	on_ಸ್ಥಿತಿ_ಸಾಲು = 0;	// #lines below bottom of window
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	on_sep_ಸಾಲು = 0;	// on separator right of window
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MENU
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ  ಒಳ_winbar = FALSE;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ   ಒಳ_popup_win = FALSE;
    ಸ್ಥಿರ win_T *click_ಒಳ_popup_win = ಖಾಲಿ;
#ಆದರೆ_ಅಂತ್ಯ
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	prev_ಅಡ್ಡಸಾಲು = -1;
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	prev_ಲಂಬಸಾಲು = -1;
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	did_drag = FALSE;	// drag was noticed

    win_T	*wp, *old_curwin;
    pos_T	old_cursor;
    ಪೂರ್ಣಾಂಕ		ಎಣಿಕೆ_2;
    ಪೂರ್ಣಾಂಕ		ಮೊದಲ;
    ಪೂರ್ಣಾಂಕ		ಅಡ್ಡಸಾಲು = mouse_ಅಡ್ಡಸಾಲು;
    ಪೂರ್ಣಾಂಕ		ಲಂಬಸಾಲು = mouse_ಲಂಬಸಾಲು;
    colnr_T	ಲಂಬಸಾಲು_from_screen = -1;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
    ಪೂರ್ಣಾಂಕ		mouse_char = ' ';
#ಆದರೆ_ಅಂತ್ಯ

    mouse_past_bottom = FALSE;
    mouse_past_eol = FALSE;

    ಆದರೆ (flags & MOUSE_RELEASED)
    {
	// On button release we may change window focus if positioned on a
	// status line and no dragging happened.
	ಆದರೆ (dragwin != ಖಾಲಿ && !did_drag)
	    flags &= ~(MOUSE_FOCUS | MOUSE_DID_MOVE);
	dragwin = ಖಾಲಿ;
	did_drag = FALSE;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
	ಆದರೆ (click_ಒಳ_popup_win != ಖಾಲಿ && popup_dragwin == ಖಾಲಿ)
	    popup_close_for_mouse_click(click_ಒಳ_popup_win);

	popup_dragwin = ಖಾಲಿ;
	click_ಒಳ_popup_win = ಖಾಲಿ;
#ಆದರೆ_ಅಂತ್ಯ
    }

    ಆದರೆ ((flags & MOUSE_DID_MOVE)
	    && prev_ಅಡ್ಡಸಾಲು == mouse_ಅಡ್ಡಸಾಲು
	    && prev_ಲಂಬಸಾಲು == mouse_ಲಂಬಸಾಲು)
    {
retnomove:
	// before moving the cursor for a left click which is NOT in a status
	// line, stop Visual mode
	ಆದರೆ (on_ಸ್ಥಿತಿ_ಸಾಲು)
	    ಹಿಂತಿರುಗಿ ಒಳ_ಸ್ಥಿತಿ_ಸಾಲು;
	ಆದರೆ (on_sep_ಸಾಲು)
	    ಹಿಂತಿರುಗಿ ಒಳ_SEP_ಸಾಲು;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MENU
	ಆದರೆ (ಒಳ_winbar)
	{
	    // A quick second click may arrive as a double-click, but we use it
	    // as a second click in the WinBar.
	    ಆದರೆ ((mod_ಮುಖವಾಡ & MOD_ಮುಖವಾಡ_MULTI_CLICK) && !(flags & MOUSE_RELEASED))
	    {
		wp = mouse_ಕಂಡುಹಿಡಿ_win(&ಅಡ್ಡಸಾಲು, &ಲಂಬಸಾಲು, FAIL_POPUP);
		ಆದರೆ (wp == ಖಾಲಿ)
		    ಹಿಂತಿರುಗಿ ಒಳ_UNKNOWN;
		winbar_click(wp, ಲಂಬಸಾಲು);
	    }
	    ಹಿಂತಿರುಗಿ ಒಳ_OTHER_WIN | MOUSE_WINBAR;
	}
#ಆದರೆ_ಅಂತ್ಯ
	ಆದರೆ (flags & MOUSE_MAY_STOP_VIS)
	{
	    ಕೊನೆ_visual_ಬಹುಲಕ_keep_button();
	    redraw_curbuf_later(UPD_INVERTED);	// delete the inversion
	}
#ಆದರೆ defined(FEAT_CLIPBOARD)
	// Continue a modeless selection in another window.
	ಆದರೆ (cmdwin_type != 0 && ಅಡ್ಡಸಾಲು < cmdwin_win->w_winrow)
	    ಹಿಂತಿರುಗಿ ಒಳ_OTHER_WIN;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
	// Continue a modeless selection in a popup window or dragging it.
	ಆದರೆ (ಒಳ_popup_win)
	{
	    click_ಒಳ_popup_win = ಖಾಲಿ;  // don't close it on release
	    ಆದರೆ (popup_dragwin != ಖಾಲಿ)
	    {
		// dragging a popup window
		popup_drag(popup_dragwin);
		ಹಿಂತಿರುಗಿ ಒಳ_UNKNOWN;
	    }
	    ಹಿಂತಿರುಗಿ ಒಳ_OTHER_WIN;
	}
#ಆದರೆ_ಅಂತ್ಯ
	ಹಿಂತಿರುಗಿ ಒಳ_ಬಫರ್;
    }

    prev_ಅಡ್ಡಸಾಲು = mouse_ಅಡ್ಡಸಾಲು;
    prev_ಲಂಬಸಾಲು = mouse_ಲಂಬಸಾಲು;

    ಆದರೆ (flags & MOUSE_SETPOS)
	ಹೋಗು retnomove;				// ugly goto...

    old_curwin = curwin;
    old_cursor = curwin->w_cursor;

    ಆದರೆ (!(flags & MOUSE_FOCUS))
    {
	ಆದರೆ (ಅಡ್ಡಸಾಲು < 0 || ಲಂಬಸಾಲು < 0) // check if it makes sense
	    ಹಿಂತಿರುಗಿ ಒಳ_UNKNOWN;

	// find the window where the row is in and adjust "row" and "col" to be
	// relative to top-left of the window
	wp = mouse_ಕಂಡುಹಿಡಿ_win(&ಅಡ್ಡಸಾಲು, &ಲಂಬಸಾಲು, ಕಂಡುಹಿಡಿ_POPUP);
	ಆದರೆ (wp == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ ಒಳ_UNKNOWN;
	dragwin = ಖಾಲಿ;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
	// Click in a popup window may start dragging or modeless selection,
	// but not much else.
	ಆದರೆ (WIN_IS_POPUP(wp))
	{
	    on_sep_ಸಾಲು = 0;
	    on_ಸ್ಥಿತಿ_ಸಾಲು = 0;
	    ಒಳ_popup_win = TRUE;
	    ಆದರೆ (which_button == MOUSE_ಎಡ && popup_close_if_on_X(wp, ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು))
	    {
		ಹಿಂತಿರುಗಿ ಒಳ_UNKNOWN;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (((wp->w_popup_flags & (POPF_DRAG | POPF_RESIZE))
					      && popup_on_border(wp, ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು))
				       || (wp->w_popup_flags & POPF_DRAGALL))
	    {
		popup_dragwin = wp;
		popup_ಆರಂಭ_drag(wp, ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು);
		ಹಿಂತಿರುಗಿ ಒಳ_UNKNOWN;
	    }
	    // Only close on release, otherwise it's not possible to drag or do
	    // modeless selection.
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (wp->w_popup_close == POPCLOSE_CLICK
		    && which_button == MOUSE_ಎಡ)
	    {
		click_ಒಳ_popup_win = wp;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (which_button == MOUSE_ಎಡ)
		// If the click is in the scrollbar, may scroll up/down.
		popup_handle_scrollbar_click(wp, ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು);
# ifdef FEAT_CLIPBOARD
	    ಹಿಂತಿರುಗಿ ಒಳ_OTHER_WIN;
# ಇಲ್ಲದಿದ್ದರೆ
	    ಹಿಂತಿರುಗಿ ಒಳ_UNKNOWN;
# endif
	}
	ಒಳ_popup_win = FALSE;
	popup_dragwin = ಖಾಲಿ;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MENU
	ಆದರೆ (ಅಡ್ಡಸಾಲು == -1)
	{
	    // A click in the window toolbar does not enter another window or
	    // change Visual highlighting.
	    winbar_click(wp, ಲಂಬಸಾಲು);
	    ಒಳ_winbar = TRUE;
	    ಹಿಂತಿರುಗಿ ಒಳ_OTHER_WIN | MOUSE_WINBAR;
	}
	ಒಳ_winbar = FALSE;
#ಆದರೆ_ಅಂತ್ಯ

	// winpos and height may change in win_enter()!
	ಆದರೆ (ಅಡ್ಡಸಾಲು >= wp->w_ಎತ್ತರ)		// In (or below) status line
	{
	    on_ಸ್ಥಿತಿ_ಸಾಲು = ಅಡ್ಡಸಾಲು - wp->w_ಎತ್ತರ + 1;
	    dragwin = wp;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    on_ಸ್ಥಿತಿ_ಸಾಲು = 0;
	ಆದರೆ (ಲಂಬಸಾಲು >= wp->w_ಅಗಲ)		// In separator line
	{
	    on_sep_ಸಾಲು = ಲಂಬಸಾಲು - wp->w_ಅಗಲ + 1;
	    dragwin = wp;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    on_sep_ಸಾಲು = 0;

	// The rightmost character of the status line might be a vertical
	// separator character if there is no connecting window to the right.
	ಆದರೆ (on_ಸ್ಥಿತಿ_ಸಾಲು && on_sep_ಸಾಲು)
	{
	    ಆದರೆ (stl_connected(wp))
		on_sep_ಸಾಲು = 0;
	    ಇಲ್ಲದಿದ್ದರೆ
		on_ಸ್ಥಿತಿ_ಸಾಲು = 0;
	}

	// Before jumping to another buffer, or moving the cursor for a left
	// click, stop Visual mode.
	ಆದರೆ (VIsual_ಸಕ್ರಿಯ
		&& (wp->w_ಬಫರ್ != curwin->w_ಬಫರ್
		    || (!on_ಸ್ಥಿತಿ_ಸಾಲು && !on_sep_ಸಾಲು
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
			&& (
# ifdef FEAT_RIGHTLEFT
			    wp->w_p_rl ? ಲಂಬಸಾಲು < wp->w_ಅಗಲ - wp->w_p_fdc :
# endif
			    ಲಂಬಸಾಲು >= wp->w_p_fdc + (wp != cmdwin_win ? 0 : 1)
			    )
#ಆದರೆ_ಅಂತ್ಯ
			&& (flags & MOUSE_MAY_STOP_VIS))))
	{
	    ಕೊನೆ_visual_ಬಹುಲಕ_keep_button();
	    redraw_curbuf_later(UPD_INVERTED);	// delete the inversion
	}
	ಆದರೆ (cmdwin_type != 0 && wp != cmdwin_win)
	{
	    // A click outside the command-line window: Use modeless
	    // selection if possible.  Allow dragging the status lines.
	    on_sep_ಸಾಲು = 0;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CLIPBOARD
	    ಆದರೆ (on_ಸ್ಥಿತಿ_ಸಾಲು)
		ಹಿಂತಿರುಗಿ ಒಳ_ಸ್ಥಿತಿ_ಸಾಲು;
	    ಹಿಂತಿರುಗಿ ಒಳ_OTHER_WIN;
#ಇಲ್ಲದಿದ್ದರೆ
	    ಅಡ್ಡಸಾಲು = 0;
	    ಲಂಬಸಾಲು += wp->w_wincol;
	    wp = cmdwin_win;
#ಆದರೆ_ಅಂತ್ಯ
	}
#ಆದರೆ defined(FEAT_PROP_POPUP) && defined(FEAT_TERMINAL)
	ಆದರೆ (popup_is_popup(curwin) && curbuf->b_ಪದ != ಖಾಲಿ)
	    // terminal in popup window: don't jump to another window
	    ಹಿಂತಿರುಗಿ ಒಳ_OTHER_WIN;
#ಆದರೆ_ಅಂತ್ಯ
	// Only change window focus when not clicking on or dragging the
	// status line.  Do change focus when releasing the mouse button
	// (MOUSE_FOCUS was set above if we dragged first).
	ಆದರೆ (dragwin == ಖಾಲಿ || (flags & MOUSE_RELEASED))
	    win_enter(wp, TRUE);		// can make wp invalid!

	ಆದರೆ (curwin != old_curwin)
	{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CHECK_DOUBLE_CLICK
	    // set topline, to be able to check for double click ourselves
	    ಗಣ_mouse_topline(curwin);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMINAL
	    // when entering a terminal window may change state
	    ಪದ_win_entered();
#ಆದರೆ_ಅಂತ್ಯ
	}
	ಆದರೆ (on_ಸ್ಥಿತಿ_ಸಾಲು)			// In (or below) status line
	{
	    // Don't use start_arrow() if we're in the same window
	    ಆದರೆ (curwin == old_curwin)
		ಹಿಂತಿರುಗಿ ಒಳ_ಸ್ಥಿತಿ_ಸಾಲು;
	    ಇಲ್ಲದಿದ್ದರೆ
		ಹಿಂತಿರುಗಿ ಒಳ_ಸ್ಥಿತಿ_ಸಾಲು | CURSOR_MOVED;
	}
	ಆದರೆ (on_sep_ಸಾಲು)			// In (or below) status line
	{
	    // Don't use start_arrow() if we're in the same window
	    ಆದರೆ (curwin == old_curwin)
		ಹಿಂತಿರುಗಿ ಒಳ_SEP_ಸಾಲು;
	    ಇಲ್ಲದಿದ್ದರೆ
		ಹಿಂತಿರುಗಿ ಒಳ_SEP_ಸಾಲು | CURSOR_MOVED;
	}

	curwin->w_cursor.lnum = curwin->w_topline;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
	// remember topline, needed for double click
	gui_prev_topline = curwin->w_topline;
# ifdef FEAT_DIFF
	gui_prev_topfill = curwin->w_topfill;
# endif
#ಆದರೆ_ಅಂತ್ಯ
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (on_ಸ್ಥಿತಿ_ಸಾಲು && which_button == MOUSE_ಎಡ)
    {
	ಆದರೆ (dragwin != ಖಾಲಿ)
	{
	    // Drag the status line
	    ಎಣಿಕೆ_2 = ಅಡ್ಡಸಾಲು - W_WINROW(dragwin) - dragwin->w_ಎತ್ತರ + 1
							     - on_ಸ್ಥಿತಿ_ಸಾಲು;
	    win_drag_ಸ್ಥಿತಿ_ಸಾಲು(dragwin, ಎಣಿಕೆ_2);
	    did_drag |= ಎಣಿಕೆ_2;
	}
	ಹಿಂತಿರುಗಿ ಒಳ_ಸ್ಥಿತಿ_ಸಾಲು;			// Cursor didn't move
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (on_sep_ಸಾಲು && which_button == MOUSE_ಎಡ)
    {
	ಆದರೆ (dragwin != ಖಾಲಿ)
	{
	    // Drag the separator column
	    ಎಣಿಕೆ_2 = ಲಂಬಸಾಲು - dragwin->w_wincol - dragwin->w_ಅಗಲ + 1
								- on_sep_ಸಾಲು;
	    win_drag_vsep_ಸಾಲು(dragwin, ಎಣಿಕೆ_2);
	    did_drag |= ಎಣಿಕೆ_2;
	}
	ಹಿಂತಿರುಗಿ ಒಳ_SEP_ಸಾಲು;			// Cursor didn't move
    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MENU
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಒಳ_winbar)
    {
	// After a click on the window toolbar don't start Visual mode.
	ಹಿಂತಿರುಗಿ ಒಳ_OTHER_WIN | MOUSE_WINBAR;
    }
#ಆದರೆ_ಅಂತ್ಯ
    ಇಲ್ಲದಿದ್ದರೆ // keep_window_focus must be TRUE
    {
	// before moving the cursor for a left click, stop Visual mode
	ಆದರೆ (flags & MOUSE_MAY_STOP_VIS)
	{
	    ಕೊನೆ_visual_ಬಹುಲಕ_keep_button();
	    redraw_curbuf_later(UPD_INVERTED);	// delete the inversion
	}

#ಆದರೆ defined(FEAT_CLIPBOARD)
	// Continue a modeless selection in another window.
	ಆದರೆ (cmdwin_type != 0 && ಅಡ್ಡಸಾಲು < cmdwin_win->w_winrow)
	    ಹಿಂತಿರುಗಿ ಒಳ_OTHER_WIN;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
	ಆದರೆ (ಒಳ_popup_win)
	{
	    ಆದರೆ (popup_dragwin != ಖಾಲಿ)
	    {
		// dragging a popup window
		popup_drag(popup_dragwin);
		ಹಿಂತಿರುಗಿ ಒಳ_UNKNOWN;
	    }
	    // continue a modeless selection in a popup window
	    click_ಒಳ_popup_win = ಖಾಲಿ;
	    ಹಿಂತಿರುಗಿ ಒಳ_OTHER_WIN;
	}
#ಆದರೆ_ಅಂತ್ಯ

	ಅಡ್ಡಸಾಲು -= W_WINROW(curwin);
	ಲಂಬಸಾಲು -= curwin->w_wincol;

	// When clicking beyond the end of the window, scroll the screen.
	// Scroll by however many rows outside the window we are.
	ಆದರೆ (ಅಡ್ಡಸಾಲು < 0)
	{
	    ಎಣಿಕೆ_2 = 0;
	    ಗಾಗಿ (ಮೊದಲ = TRUE; curwin->w_topline > 1; )
	    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
		ಆದರೆ (curwin->w_topfill < diff_check_fill(curwin, curwin->w_topline))
		    ++ಎಣಿಕೆ_2;
		ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
		    ಎಣಿಕೆ_2 += plines(curwin->w_topline - 1);
		ಆದರೆ (!ಮೊದಲ && ಎಣಿಕೆ_2 > -ಅಡ್ಡಸಾಲು)
		    ನಿಲ್ಲಿಸು;
		ಮೊದಲ = FALSE;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
		(ಶೂನ್ಯ)hasFolding(curwin->w_topline, &curwin->w_topline, ಖಾಲಿ);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
		ಆದರೆ (curwin->w_topfill < diff_check_fill(curwin, curwin->w_topline))
		    ++curwin->w_topfill;
		ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
		{
		    --curwin->w_topline;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
		    curwin->w_topfill = 0;
#ಆದರೆ_ಅಂತ್ಯ
		}
	    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
	    check_topfill(curwin, FALSE);
#ಆದರೆ_ಅಂತ್ಯ
	    curwin->w_valid &=
		      ~(VALID_WROW|VALID_CROW|VALID_BOTLINE|VALID_BOTLINE_AP);
	    redraw_later(UPD_VALID);
	    ಅಡ್ಡಸಾಲು = 0;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಅಡ್ಡಸಾಲು >= curwin->w_ಎತ್ತರ)
	{
	    ಎಣಿಕೆ_2 = 0;
	    ಗಾಗಿ (ಮೊದಲ = TRUE; curwin->w_topline < curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ; )
	    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
		ಆದರೆ (curwin->w_topfill > 0)
		    ++ಎಣಿಕೆ_2;
		ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
		    ಎಣಿಕೆ_2 += plines(curwin->w_topline);
		ಆದರೆ (!ಮೊದಲ && ಎಣಿಕೆ_2 > ಅಡ್ಡಸಾಲು - curwin->w_ಎತ್ತರ + 1)
		    ನಿಲ್ಲಿಸು;
		ಮೊದಲ = FALSE;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
		ಆದರೆ (hasFolding(curwin->w_topline, ಖಾಲಿ, &curwin->w_topline)
			&& curwin->w_topline == curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
		    ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
		ಆದರೆ (curwin->w_topfill > 0)
		    --curwin->w_topfill;
		ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
		{
		    ++curwin->w_topline;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
		    curwin->w_topfill =
				   diff_check_fill(curwin, curwin->w_topline);
#ಆದರೆ_ಅಂತ್ಯ
		}
	    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
	    check_topfill(curwin, FALSE);
#ಆದರೆ_ಅಂತ್ಯ
	    redraw_later(UPD_VALID);
	    curwin->w_valid &=
		      ~(VALID_WROW|VALID_CROW|VALID_BOTLINE|VALID_BOTLINE_AP);
	    ಅಡ್ಡಸಾಲು = curwin->w_ಎತ್ತರ - 1;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಅಡ್ಡಸಾಲು == 0)
	{
	    // When dragging the mouse, while the text has been scrolled up as
	    // far as it goes, moving the mouse in the top line should scroll
	    // the text down (done later when recomputing w_topline).
	    ಆದರೆ (mouse_dragging > 0
		    && curwin->w_cursor.lnum
				       == curwin->w_ಬಫರ್->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ
		    && curwin->w_cursor.lnum == curwin->w_topline)
		curwin->w_valid &= ~(VALID_TOPLINE);
	}
    }

    ಆದರೆ (prev_ಅಡ್ಡಸಾಲು >= W_WINROW(curwin)
	&& prev_ಅಡ್ಡಸಾಲು < W_WINROW(curwin) + curwin->w_ಎತ್ತರ
	&& prev_ಲಂಬಸಾಲು >= curwin->w_wincol && prev_ಲಂಬಸಾಲು < W_ENDCOL(curwin)
						       && Screen_ಸಾಲುಗಳು != ಖಾಲಿ)
    {
	ಪೂರ್ಣಾಂಕ off = ಸಾಲು_ಸ್ಥಾನಾಂತರ[prev_ಅಡ್ಡಸಾಲು] + prev_ಲಂಬಸಾಲು;

	// Only use ScreenCols[] after the window was redrawn.  Mainly matters
	// for tests, a user would not click before redrawing.
	// Do not use when 'virtualedit' is active.
	ಆದರೆ (curwin->w_redr_type <= UPD_VALID_NO_UPDATE)
	    ಲಂಬಸಾಲು_from_screen = Screen_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ[off];
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
	// Remember the character under the mouse, it might be a '-' or '+' in
	// the fold column.
	mouse_char = enc_utf8 && Screen_ಸಾಲುಗಳು_UC[off] != 0
				       ? Screen_ಸಾಲುಗಳು_UC[off] : Screen_ಸಾಲುಗಳು[off];
#ಆದರೆ_ಅಂತ್ಯ
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
    // Check for position outside of the fold column.
    ಆದರೆ (
# ifdef FEAT_RIGHTLEFT
	    curwin->w_p_rl ? ಲಂಬಸಾಲು < curwin->w_ಅಗಲ - curwin->w_p_fdc :
# endif
	    ಲಂಬಸಾಲು >= curwin->w_p_fdc + (cmdwin_win != curwin ? 0 : 1)
       )
	mouse_char = ' ';
#ಆದರೆ_ಅಂತ್ಯ

    // compute the position in the buffer line from the posn on the screen
    ಆದರೆ (mouse_comp_pos(curwin, &ಅಡ್ಡಸಾಲು, &ಲಂಬಸಾಲು, &curwin->w_cursor.lnum, ಖಾಲಿ))
	mouse_past_bottom = TRUE;

    // Start Visual mode before coladvance(), for when 'sel' != "old"
    ಆದರೆ ((flags & MOUSE_MAY_VIS) && !VIsual_ಸಕ್ರಿಯ)
    {
	check_visual_highlight();
	VIsual = old_cursor;
	VIsual_ಸಕ್ರಿಯ = TRUE;
	VIsual_reselect = TRUE;
	// if 'selectmode' contains "mouse", start Select mode
	may_ಆರಂಭ_select('o');
	setmouse();
	ಆದರೆ (p_smd && ಸಂದೇಶ_silent == 0)
	    redraw_cmdline = TRUE;	// show visual mode later
    }

    ಆದರೆ (ಲಂಬಸಾಲು_from_screen >= 0)
    {
	// Use the virtual column from ScreenCols[], it is accurate also after
	// concealed characters.
	ಲಂಬಸಾಲು = ಲಂಬಸಾಲು_from_screen;
    }

    curwin->w_curswant = ಲಂಬಸಾಲು;
    curwin->w_ಗಣ_curswant = false;	// May still have been TRUE
    ಆದರೆ (coladvance(ಲಂಬಸಾಲು) == FAIL)	// Mouse click beyond end of line
    {
	ಆದರೆ (inclusive != ಖಾಲಿ)
	    *inclusive = TRUE;
	mouse_past_eol = TRUE;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (inclusive != ಖಾಲಿ)
	*inclusive = FALSE;

    ಎಣಿಕೆ_2 = ಒಳ_ಬಫರ್;
    ಆದರೆ (curwin != old_curwin || curwin->w_cursor.lnum != old_cursor.lnum
	    || curwin->w_cursor.ಲಂಬಸಾಲು != old_cursor.ಲಂಬಸಾಲು)
	ಎಣಿಕೆ_2 |= CURSOR_MOVED;		// Cursor has moved

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
    ಆದರೆ (mouse_char == curwin->w_fill_chars.foldclosed)
	ಎಣಿಕೆ_2 |= MOUSE_FOLD_OPEN;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (mouse_char != ' ')
	ಎಣಿಕೆ_2 |= MOUSE_FOLD_CLOSE;
#ಆದರೆ_ಅಂತ್ಯ

    ಹಿಂತಿರುಗಿ ಎಣಿಕೆ_2;
}

/*
 * Make a horizontal scroll to "leftcol".
 * Return TRUE if the cursor moved, FALSE otherwise.
 */
    ಪೂರ್ಣಾಂಕ
do_mousescroll_horiz(long_u leftcol)
{
    ಆದರೆ (curwin->w_p_wrap)
	ಹಿಂತಿರುಗಿ FALSE;  // no horizontal scrolling when wrapping

    ಆದರೆ (curwin->w_leftcol == (colnr_T)leftcol)
	ಹಿಂತಿರುಗಿ FALSE;  // already there

    // When the line of the cursor is too short, move the cursor to the
    // longest visible line.
    ಆದರೆ (
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
	    (!gui.ಒಳ_use || vim_strchr(p_go, GO_HORSCROLL) == ಖಾಲಿ) &&
#ಆದರೆ_ಅಂತ್ಯ
		    !virtual_ಸಕ್ರಿಯ()
	    && (ದೀರ್ಘ)leftcol > scroll_ಸಾಲು_len(curwin->w_cursor.lnum))
    {
	curwin->w_cursor.lnum = ui_ಕಂಡುಹಿಡಿ_longest_lnum();
	curwin->w_cursor.ಲಂಬಸಾಲು = 0;
    }

    ಹಿಂತಿರುಗಿ ಗಣ_leftcol((colnr_T)leftcol);
}

/*
 * Normal and Visual modes implementation for scrolling in direction
 * "cap->arg", which is one of the MSCR_ values.
 */
    ಶೂನ್ಯ
nv_mousescroll(cmdarg_T *cap)
{
    win_T   *old_curwin = curwin;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TABPANEL
    ಆದರೆ (mouse_ಅಡ್ಡಸಾಲು >= 0 && mouse_ಲಂಬಸಾಲು >= 0
	    && (cap->arg == MSCR_ಮೇಲೆ || cap->arg == MSCR_ಕೆಳಕ್ಕೆ)
	    && mouse_on_tabpanel())
    {
	(ಶೂನ್ಯ)tabpanel_scroll(cap->arg == MSCR_ಮೇಲೆ ? 1 : -1,
		mouse_vert_ಹೆಜ್ಜೆ > 0 ? mouse_vert_ಹೆಜ್ಜೆ : 3);
	ಹಿಂತಿರುಗಿ;
    }
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (mouse_ಅಡ್ಡಸಾಲು >= 0 && mouse_ಲಂಬಸಾಲು >= 0)
    {
	// Find the window at the mouse pointer coordinates.
	// NOTE: Must restore "curwin" to "old_curwin" before returning!
	ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲು = mouse_ಅಡ್ಡಸಾಲು;
	ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು = mouse_ಲಂಬಸಾಲು;
	curwin = mouse_ಕಂಡುಹಿಡಿ_win(&ಅಡ್ಡಸಾಲು, &ಲಂಬಸಾಲು, ಕಂಡುಹಿಡಿ_POPUP);
	ಆದರೆ (curwin == ಖಾಲಿ)
	{
	    curwin = old_curwin;
	    ಹಿಂತಿರುಗಿ;
	}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
	ಆದರೆ (WIN_IS_POPUP(curwin) && !curwin->w_has_scrollbar)
	{
	    // cannot scroll this popup window
	    curwin = old_curwin;
	    ಹಿಂತಿರುಗಿ;
	}
#ಆದರೆ_ಅಂತ್ಯ
	curbuf = curwin->w_ಬಫರ್;
    }

    // Call the common mouse scroll function shared with other modes.
    do_mousescroll(cap);

    curwin->w_redr_ಸ್ಥಿತಿ = true;
    curwin = old_curwin;
    curbuf = curwin->w_ಬಫರ್;
}

/*
 * Mouse clicks and drags.
 */
    ಶೂನ್ಯ
nv_mouse(cmdarg_T *cap)
{
    (ಶೂನ್ಯ)do_mouse(cap->oap, cap->cmdchar, BACKWARD, cap->count1, 0);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	held_button = MOUSE_RELEASE;

    ಶೂನ್ಯ
reset_held_button(ಶೂನ್ಯ)
{
    held_button = MOUSE_RELEASE;
}

/*
 * Check if typebuf 'tp' contains a terminal mouse code and returns the
 * modifiers found in typebuf in 'modifiers'.
 */
    ಪೂರ್ಣಾಂಕ
check_termcode_mouse(
    char_u	*tp,
    ಪೂರ್ಣಾಂಕ		*slen,
    char_u	*ಕೀಲಿ_name,
    char_u	*modifiers_ಆರಂಭ,
    ಪೂರ್ಣಾಂಕ		idx,
    ಪೂರ್ಣಾಂಕ		*modifiers)
{
    ಪೂರ್ಣಾಂಕ		j;
    char_u	*p;
#ಆದರೆ !defined(UNIX) || defined(FEAT_MOUSE_XTERM) || defined(FEAT_GUI) \
    || defined(FEAT_MOUSE_GPM) || defined(FEAT_SYSMOUSE)
    char_u	ಬೈಟುಗಳು[6];
    ಪೂರ್ಣಾಂಕ		ಅಂಕೆ_ಬೈಟುಗಳು;
#ಆದರೆ_ಅಂತ್ಯ
    ಪೂರ್ಣಾಂಕ		mouse_ಸಂಕೇತ = 0;	    // init for GCC
    ಪೂರ್ಣಾಂಕ		is_click, is_drag;
    ಪೂರ್ಣಾಂಕ		is_release, release_is_ambiguous;
    ಪೂರ್ಣಾಂಕ		wheel_ಸಂಕೇತ = 0;
    ಪೂರ್ಣಾಂಕ		current_button;
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	orig_ಅಂಕೆ_clicks = 1;
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	orig_mouse_ಸಂಕೇತ = 0x0;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CHECK_DOUBLE_CLICK
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	orig_mouse_ಲಂಬಸಾಲು = 0;
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	orig_mouse_ಅಡ್ಡಸಾಲು = 0;
    ಸ್ಥಿರ ರಚನೆ timeval  orig_mouse_ಸಮಯ = {0, 0};
    // time of previous mouse click
    ರಚನೆ timeval  mouse_ಸಮಯ;		// time of current mouse click
    ದೀರ್ಘ	timediff;		// elapsed time in msec
#ಆದರೆ_ಅಂತ್ಯ

    is_click = is_drag = is_release = release_is_ambiguous = FALSE;

#ಆದರೆ !defined(UNIX) || defined(FEAT_MOUSE_XTERM) || defined(FEAT_GUI) \
    || defined(FEAT_MOUSE_GPM) || defined(FEAT_SYSMOUSE)
    ಆದರೆ (ಕೀಲಿ_name[0] == KS_MOUSE
# ifdef FEAT_MOUSE_GPM
	    || ಕೀಲಿ_name[0] == KS_GPM_MOUSE
# endif
       )
    {
	/*
	 * For xterm we get "<t_mouse>scr", where s == encoded button state:
	 *	   0x20 = left button down
	 *	   0x21 = middle button down
	 *	   0x22 = right button down
	 *	   0x23 = any button release
	 *	   0x60 = button 4 down (scroll wheel down)
	 *	   0x61 = button 5 down (scroll wheel up)
	 *	   0x62 = button 6 down (scroll wheel left)
	 *	   0x63 = button 7 down (scroll wheel right)
	 *	   0xa0 = button 8 down (backward button)
	 *	   0xa1 = button 9 down (forward button)
	 *	add 0x04 for SHIFT
	 *	add 0x08 for ALT
	 *	add 0x10 for CTRL
	 *	add 0x20 for mouse drag (0x40 is drag with left button)
	 *	add 0x40 for mouse move (0x80 is move, 0x81 too)
	 *		 0x43 (drag + release) is also move
	 *  c == column + ' ' + 1 == column + 33
	 *  r == row + ' ' + 1 == row + 33
	 *
	 * The coordinates are passed on through global variables.  Ugly, but
	 * this avoids trouble with mouse clicks at an unexpected moment and
	 * allows for mapping them.
	 */
	ಗಾಗಿ (;;)
	{
	    // For the GUI and for MS-Windows two bytes each are used for row
	    // and column.  Allows for more than 223 columns.
# ಆದರೆ defined(FEAT_GUI) || defined(MSWIN)
	    ಆದರೆ (TRUE
#  ಆದರೆ defined(FEAT_GUI) && !defined(MSWIN)
		&& gui.ಒಳ_use
#  endif
		)
	    {
		ಅಂಕೆ_ಬೈಟುಗಳು = get_ಬೈಟುಗಳು_from_buf(tp + *slen, ಬೈಟುಗಳು, 5);
		ಆದರೆ (ಅಂಕೆ_ಬೈಟುಗಳು == -1)	// not enough coordinates
		    ಹಿಂತಿರುಗಿ -1;
		mouse_ಸಂಕೇತ = ಬೈಟುಗಳು[0];
		mouse_ಲಂಬಸಾಲು = 128 * (ಬೈಟುಗಳು[1] - ' ' - 1)
		    + ಬೈಟುಗಳು[2] - ' ' - 1;
		mouse_ಅಡ್ಡಸಾಲು = 128 * (ಬೈಟುಗಳು[3] - ' ' - 1)
		    + ಬೈಟುಗಳು[4] - ' ' - 1;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
# endif
	    {
		ಅಂಕೆ_ಬೈಟುಗಳು = get_ಬೈಟುಗಳು_from_buf(tp + *slen, ಬೈಟುಗಳು, 3);
		ಆದರೆ (ಅಂಕೆ_ಬೈಟುಗಳು == -1)	// not enough coordinates
		    ಹಿಂತಿರುಗಿ -1;
		mouse_ಸಂಕೇತ = ಬೈಟುಗಳು[0];
		mouse_ಲಂಬಸಾಲು = ಬೈಟುಗಳು[1] - ' ' - 1;
		mouse_ಅಡ್ಡಸಾಲು = ಬೈಟುಗಳು[2] - ' ' - 1;
	    }
	    *slen += ಅಂಕೆ_ಬೈಟುಗಳು;

	    // If the following bytes is also a mouse code and it has the same
	    // code, dump this one and get the next.  This makes dragging a
	    // whole lot faster.
# ifdef FEAT_GUI
	    ಆದರೆ (gui.ಒಳ_use)
		j = 3;
	    ಇಲ್ಲದಿದ್ದರೆ
# endif
		j = get_termcode_len(idx);
	    ಆದರೆ (STRNCMP(tp, tp + *slen, (size_t)j) == 0
		    && tp[*slen + j] == mouse_ಸಂಕೇತ
		    && tp[*slen + j + 1] != NUL
		    && tp[*slen + j + 2] != NUL
# ifdef FEAT_GUI
		    && (!gui.ಒಳ_use
			|| (tp[*slen + j + 3] != NUL
			    && tp[*slen + j + 4] != NUL))
# endif
	       )
		*slen += j;
	    ಇಲ್ಲದಿದ್ದರೆ
		ನಿಲ್ಲಿಸು;
	}
    }

    ಆದರೆ (ಕೀಲಿ_name[0] == KS_URXVT_MOUSE
	    || ಕೀಲಿ_name[0] == KS_SGR_MOUSE
	    || ಕೀಲಿ_name[0] == KS_SGR_MOUSE_RELEASE)
    {
	// URXVT 1015 mouse reporting mode:
	// Almost identical to xterm mouse mode, except the values are decimal
	// instead of bytes.
	//
	// \033[%d;%d;%dM
	//	       ^-- row
	//	    ^----- column
	//	 ^-------- code
	//
	// SGR 1006 mouse reporting mode:
	// Almost identical to xterm mouse mode, except the values are decimal
	// instead of bytes.
	//
	// \033[<%d;%d;%dM
	//	       ^-- row
	//	    ^----- column
	//	 ^-------- code
	//
	// \033[<%d;%d;%dm	  : mouse release event
	//	       ^-- row
	//	    ^----- column
	//	 ^-------- code
	p = modifiers_ಆರಂಭ;
	ಆದರೆ (p == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ -1;

	mouse_ಸಂಕೇತ = getdigits(&p);
	ಆದರೆ (*p++ != ';')
	    ಹಿಂತಿರುಗಿ -1;

	// when mouse reporting is SGR, add 32 to mouse code
	ಆದರೆ (ಕೀಲಿ_name[0] == KS_SGR_MOUSE
		|| ಕೀಲಿ_name[0] == KS_SGR_MOUSE_RELEASE)
	    mouse_ಸಂಕೇತ += 32;

	mouse_ಲಂಬಸಾಲು = getdigits(&p) - 1;
	ಆದರೆ (*p++ != ';')
	    ಹಿಂತಿರುಗಿ -1;

	mouse_ಅಡ್ಡಸಾಲು = getdigits(&p) - 1;

	// The modifiers were the mouse coordinates, not the modifier keys
	// (alt/shift/ctrl/meta) state.
	*modifiers = 0;
    }

    ಆದರೆ (ಕೀಲಿ_name[0] == KS_SGR_MOUSE
	    || ಕೀಲಿ_name[0] == KS_SGR_MOUSE_RELEASE)
    {
	ಆದರೆ (ಕೀಲಿ_name[0] == KS_SGR_MOUSE_RELEASE)
	{
	    is_release = TRUE;
	    // This is used below to set held_button.
	    mouse_ಸಂಕೇತ |= MOUSE_RELEASE;
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	release_is_ambiguous = TRUE;
	ಆದರೆ ((mouse_ಸಂಕೇತ & MOUSE_RELEASE) == MOUSE_RELEASE)
	    is_release = TRUE;
    }

    ಆದರೆ (ಕೀಲಿ_name[0] == KS_MOUSE
# ifdef FEAT_MOUSE_GPM
	    || ಕೀಲಿ_name[0] == KS_GPM_MOUSE
# endif
# ifdef FEAT_MOUSE_URXVT
	    || ಕೀಲಿ_name[0] == KS_URXVT_MOUSE
# endif
	    || ಕೀಲಿ_name[0] == KS_SGR_MOUSE
	    || ಕೀಲಿ_name[0] == KS_SGR_MOUSE_RELEASE)
    {
# ಆದರೆ !defined(MSWIN)
	/*
	 * Handle old style mouse events.
	 * Recognize the xterm mouse wheel, but not in the GUI, the
	 * Linux console with GPM and the MS-DOS or Win32 console
	 * (multi-clicks use >= 0x60).
	 */
	ಆದರೆ (mouse_ಸಂಕೇತ >= MOUSEWHEEL_ಕೆಳಗಿನ && mouse_ಸಂಕೇತ < MOUSESIDEBUTTONS_ಕೆಳಗಿನ
#  ifdef FEAT_GUI
		&& !gui.ಒಳ_use
#  endif
#  ifdef FEAT_MOUSE_GPM
		&& ಕೀಲಿ_name[0] != KS_GPM_MOUSE
#  endif
	   )
	{
#  ಆದರೆ defined(UNIX)
	    ಆದರೆ (use_xterm_mouse() > 1 && mouse_ಸಂಕೇತ >= 0x80)
		// mouse-move event, using MOUSE_DRAG works
		mouse_ಸಂಕೇತ = MOUSE_DRAG;
	    ಇಲ್ಲದಿದ್ದರೆ
#  endif
		// Keep the mouse_code before it's changed, so that we
		// remember that it was a mouse wheel click.
		wheel_ಸಂಕೇತ = mouse_ಸಂಕೇತ;
	}
#  ifdef FEAT_MOUSE_XTERM
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (held_button == MOUSE_RELEASE
#   ifdef FEAT_GUI
		&& !gui.ಒಳ_use
#   endif
		&& (mouse_ಸಂಕೇತ == 0x23 || mouse_ಸಂಕೇತ == 0x24
		    || mouse_ಸಂಕೇತ == 0x40 || mouse_ಸಂಕೇತ == 0x41))
	{
	    // Apparently 0x23 and 0x24 are used by rxvt scroll wheel.
	    // And 0x40 and 0x41 are used by some xterm emulator.
	    wheel_ಸಂಕೇತ = mouse_ಸಂಕೇತ - (mouse_ಸಂಕೇತ >= 0x40 ? 0x40 : 0x23)
							      + MOUSEWHEEL_ಕೆಳಗಿನ;
	}
#  endif

#  ಆದರೆ defined(UNIX)
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (use_xterm_mouse() > 1)
	{
	    ಆದರೆ (mouse_ಸಂಕೇತ & MOUSE_DRAG_XTERM)
		mouse_ಸಂಕೇತ |= MOUSE_DRAG;
	}
#  endif
#  ifdef FEAT_XCLIPBOARD
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!(mouse_ಸಂಕೇತ & MOUSE_DRAG & ~MOUSE_CLICK_ಮುಖವಾಡ))
	{
	    ಆದರೆ (is_release)
		stop_xterm_trace();
	    ಇಲ್ಲದಿದ್ದರೆ
		ಆರಂಭ_xterm_trace(mouse_ಸಂಕೇತ);
	}
#  endif
# endif
    }
#ಆದರೆ_ಅಂತ್ಯ // !UNIX || FEAT_MOUSE_XTERM
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MOUSE_NET
    ಆದರೆ (ಕೀಲಿ_name[0] == KS_NETTERM_MOUSE)
    {
	ಪೂರ್ಣಾಂಕ mc, mr;

	// expect a rather limited sequence like: balancing {
	// \033}6,45\r
	// '6' is the row, 45 is the column
	p = tp + *slen;
	mr = getdigits(&p);
	ಆದರೆ (*p++ != ',')
	    ಹಿಂತಿರುಗಿ -1;
	mc = getdigits(&p);
	ಆದರೆ (*p++ != '\r')
	    ಹಿಂತಿರುಗಿ -1;

	mouse_ಲಂಬಸಾಲು = mc - 1;
	mouse_ಅಡ್ಡಸಾಲು = mr - 1;
	mouse_ಸಂಕೇತ = MOUSE_ಎಡ;
	*slen += (ಪೂರ್ಣಾಂಕ)(p - (tp + *slen));
    }
#ಆದರೆ_ಅಂತ್ಯ	// FEAT_MOUSE_NET
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MOUSE_JSB
    ಆದರೆ (ಕೀಲಿ_name[0] == KS_JSBTERM_MOUSE)
    {
	ಪೂರ್ಣಾಂಕ mult, val, iter, button, ಸ್ಥಿತಿ_2;

	/*
	 * JSBTERM Input Model
	 * \033[0~zw uniq escape sequence
	 * (L-x)  Left button pressed - not pressed x not reporting
	 * (M-x)  Middle button pressed - not pressed x not reporting
	 * (R-x)  Right button pressed - not pressed x not reporting
	 * (SDmdu)  Single , Double click, m: mouse move, d: button down,
		      *						   u: button up
	 *  ###   X cursor position padded to 3 digits
	 *  ###   Y cursor position padded to 3 digits
	 * (s-x)  SHIFT key pressed - not pressed x not reporting
	 * (c-x)  CTRL key pressed - not pressed x not reporting
	 * \033\\ terminating sequence
	 */
	p = tp + *slen;
	button = mouse_ಸಂಕೇತ = 0;
	ಆಯ್ಕೆ (*p++)
	{
	    ಸ್ಥಿತಿ 'L': button = 1; ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ '-': ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ 'x': ನಿಲ್ಲಿಸು; // ignore sequence
	    ಪೂರ್ವನಿಯೋಜಿತ:  ಹಿಂತಿರುಗಿ -1; // Unknown Result
	}
	ಆಯ್ಕೆ (*p++)
	{
	    ಸ್ಥಿತಿ 'M': button |= 2; ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ '-': ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ 'x': ನಿಲ್ಲಿಸು; // ignore sequence
	    ಪೂರ್ವನಿಯೋಜಿತ:  ಹಿಂತಿರುಗಿ -1; // Unknown Result
	}
	ಆಯ್ಕೆ (*p++)
	{
	    ಸ್ಥಿತಿ 'R': button |= 4; ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ '-': ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ 'x': ನಿಲ್ಲಿಸು; // ignore sequence
	    ಪೂರ್ವನಿಯೋಜಿತ:  ಹಿಂತಿರುಗಿ -1; // Unknown Result
	}
	ಸ್ಥಿತಿ_2 = *p++;
	ಗಾಗಿ (val = 0, mult = 100, iter = 0; iter < 3; iter++,
		mult /= 10, p++)
	    ಆದರೆ (*p >= '0' && *p <= '9')
		val += (*p - '0') * mult;
	    ಇಲ್ಲದಿದ್ದರೆ
		ಹಿಂತಿರುಗಿ -1;
	mouse_ಲಂಬಸಾಲು = val;
	ಗಾಗಿ (val = 0, mult = 100, iter = 0; iter < 3; iter++,
		mult /= 10, p++)
	    ಆದರೆ (*p >= '0' && *p <= '9')
		val += (*p - '0') * mult;
	    ಇಲ್ಲದಿದ್ದರೆ
		ಹಿಂತಿರುಗಿ -1;
	mouse_ಅಡ್ಡಸಾಲು = val;
	ಆಯ್ಕೆ (*p++)
	{
	    ಸ್ಥಿತಿ 's': button |= 8; ನಿಲ್ಲಿಸು;  // SHIFT key Pressed
	    ಸ್ಥಿತಿ '-': ನಿಲ್ಲಿಸು;  // Not Pressed
	    ಸ್ಥಿತಿ 'x': ನಿಲ್ಲಿಸು;  // Not Reporting
	    ಪೂರ್ವನಿಯೋಜಿತ:  ಹಿಂತಿರುಗಿ -1; // Unknown Result
	}
	ಆಯ್ಕೆ (*p++)
	{
	    ಸ್ಥಿತಿ 'c': button |= 16; ನಿಲ್ಲಿಸು;  // CTRL key Pressed
	    ಸ್ಥಿತಿ '-': ನಿಲ್ಲಿಸು;  // Not Pressed
	    ಸ್ಥಿತಿ 'x': ನಿಲ್ಲಿಸು;  // Not Reporting
	    ಪೂರ್ವನಿಯೋಜಿತ:  ಹಿಂತಿರುಗಿ -1; // Unknown Result
	}
	ಆದರೆ (*p++ != '\033')
	    ಹಿಂತಿರುಗಿ -1;
	ಆದರೆ (*p++ != '\\')
	    ಹಿಂತಿರುಗಿ -1;
	ಆಯ್ಕೆ (ಸ್ಥಿತಿ_2)
	{
	    ಸ್ಥಿತಿ 'D': // Double Click
	    ಸ್ಥಿತಿ 'S': // Single Click
		ಆದರೆ (button & 1) mouse_ಸಂಕೇತ |= MOUSE_ಎಡ;
		ಆದರೆ (button & 2) mouse_ಸಂಕೇತ |= MOUSE_ಮಧ್ಯ;
		ಆದರೆ (button & 4) mouse_ಸಂಕೇತ |= MOUSE_ಬಲ;
		ಆದರೆ (button & 8) mouse_ಸಂಕೇತ |= MOUSE_ಸರಿಸು;
		ಆದರೆ (button & 16) mouse_ಸಂಕೇತ |= MOUSE_CTRL;
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ 'm': // Mouse move
		ಆದರೆ (button & 1) mouse_ಸಂಕೇತ |= MOUSE_ಎಡ;
		ಆದರೆ (button & 2) mouse_ಸಂಕೇತ |= MOUSE_ಮಧ್ಯ;
		ಆದರೆ (button & 4) mouse_ಸಂಕೇತ |= MOUSE_ಬಲ;
		ಆದರೆ (button & 8) mouse_ಸಂಕೇತ |= MOUSE_ಸರಿಸು;
		ಆದರೆ (button & 16) mouse_ಸಂಕೇತ |= MOUSE_CTRL;
		ಆದರೆ ((button & 7) != 0)
		{
		    held_button = mouse_ಸಂಕೇತ;
		    mouse_ಸಂಕೇತ |= MOUSE_DRAG;
		}
		is_drag = TRUE;
		showmode();
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ 'd': // Button Down
		ಆದರೆ (button & 1) mouse_ಸಂಕೇತ |= MOUSE_ಎಡ;
		ಆದರೆ (button & 2) mouse_ಸಂಕೇತ |= MOUSE_ಮಧ್ಯ;
		ಆದರೆ (button & 4) mouse_ಸಂಕೇತ |= MOUSE_ಬಲ;
		ಆದರೆ (button & 8) mouse_ಸಂಕೇತ |= MOUSE_ಸರಿಸು;
		ಆದರೆ (button & 16) mouse_ಸಂಕೇತ |= MOUSE_CTRL;
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ 'u': // Button Up
		is_release = TRUE;
		ಆದರೆ (button & 1)
		    mouse_ಸಂಕೇತ |= MOUSE_ಎಡ;
		ಆದರೆ (button & 2)
		    mouse_ಸಂಕೇತ |= MOUSE_ಮಧ್ಯ;
		ಆದರೆ (button & 4)
		    mouse_ಸಂಕೇತ |= MOUSE_ಬಲ;
		ಆದರೆ (button & 8)
		    mouse_ಸಂಕೇತ |= MOUSE_ಸರಿಸು;
		ಆದರೆ (button & 16)
		    mouse_ಸಂಕೇತ |= MOUSE_CTRL;
		ನಿಲ್ಲಿಸು;
	    ಪೂರ್ವನಿಯೋಜಿತ: ಹಿಂತಿರುಗಿ -1; // Unknown Result
	}

	*slen += (p - (tp + *slen));
    }
#ಆದರೆ_ಅಂತ್ಯ // FEAT_MOUSE_JSB
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MOUSE_DEC
    ಆದರೆ (ಕೀಲಿ_name[0] == KS_DEC_MOUSE)
    {
	/*
	 * The DEC Locator Input Model
	 * Netterm delivers the code sequence:
	 *  \033[2;4;24;80&w  (left button down)
	 *  \033[3;0;24;80&w  (left button up)
	 *  \033[6;1;24;80&w  (right button down)
	 *  \033[7;0;24;80&w  (right button up)
	 * CSI Pe ; Pb ; Pr ; Pc ; Pp & w
	 * Pe is the event code
	 * Pb is the button code
	 * Pr is the row coordinate
	 * Pc is the column coordinate
	 * Pp is the third coordinate (page number)
	 * Pe, the event code indicates what event caused this report
	 *    The following event codes are defined:
	 *    0 - request, the terminal received an explicit request for a
	 *	  locator report, but the locator is unavailable
	 *    1 - request, the terminal received an explicit request for a
	 *	  locator report
	 *    2 - left button down
	 *    3 - left button up
	 *    4 - middle button down
	 *    5 - middle button up
	 *    6 - right button down
	 *    7 - right button up
	 *    8 - fourth button down
	 *    9 - fourth button up
	 *    10 - locator outside filter rectangle
	 * Pb, the button code, ASCII decimal 0-15 indicating which buttons are
	 *   down if any. The state of the four buttons on the locator
	 *   correspond to the low four bits of the decimal value, "1" means
	 *   button depressed
	 *   0 - no buttons down,
	 *   1 - right,
	 *   2 - middle,
	 *   4 - left,
	 *   8 - fourth
	 * Pr is the row coordinate of the locator position in the page,
	 *   encoded as an ASCII decimal value.  If Pr is omitted, the locator
	 *   position is undefined (outside the terminal window for example).
	 * Pc is the column coordinate of the locator position in the page,
	 *   encoded as an ASCII decimal value.  If Pc is omitted, the locator
	 *   position is undefined (outside the terminal window for example).
	 * Pp is the page coordinate of the locator position encoded as an
	 *   ASCII decimal value.  The page coordinate may be omitted if the
	 *   locator is on page one (the default).  We ignore it anyway.
	 */
	ಪೂರ್ಣಾಂಕ Pe, Pb, Pr, Pc;

	p = tp + *slen;

	// get event status
	Pe = getdigits(&p);
	ಆದರೆ (*p++ != ';')
	    ಹಿಂತಿರುಗಿ -1;

	// get button status
	Pb = getdigits(&p);
	ಆದರೆ (*p++ != ';')
	    ಹಿಂತಿರುಗಿ -1;

	// get row status
	Pr = getdigits(&p);
	ಆದರೆ (*p++ != ';')
	    ಹಿಂತಿರುಗಿ -1;

	// get column status
	Pc = getdigits(&p);

	// the page parameter is optional
	ಆದರೆ (*p == ';')
	{
	    p++;
	    (ಶೂನ್ಯ)getdigits(&p);
	}
	ಆದರೆ (*p++ != '&')
	    ಹಿಂತಿರುಗಿ -1;
	ಆದರೆ (*p++ != 'w')
	    ಹಿಂತಿರುಗಿ -1;

	mouse_ಸಂಕೇತ = 0;
	ಆಯ್ಕೆ (Pe)
	{
	    ಸ್ಥಿತಿ  0: ಹಿಂತಿರುಗಿ -1; // position request while unavailable
	    ಸ್ಥಿತಿ  1: // a response to a locator position request includes
		     //	the status of all buttons
		     Pb &= 7;   // mask off and ignore fourth button
		     ಆದರೆ (Pb & 4)
			 mouse_ಸಂಕೇತ  = MOUSE_ಎಡ;
		     ಆದರೆ (Pb & 2)
			 mouse_ಸಂಕೇತ  = MOUSE_ಮಧ್ಯ;
		     ಆದರೆ (Pb & 1)
			 mouse_ಸಂಕೇತ  = MOUSE_ಬಲ;
		     ಆದರೆ (Pb)
		     {
			 held_button = mouse_ಸಂಕೇತ;
			 mouse_ಸಂಕೇತ |= MOUSE_DRAG;
			 WantQueryMouse = TRUE;
		     }
		     is_drag = TRUE;
		     showmode();
		     ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ  2: mouse_ಸಂಕೇತ = MOUSE_ಎಡ;
		     WantQueryMouse = TRUE;
		     ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ  3: mouse_ಸಂಕೇತ = MOUSE_ಎಡ;
		     is_release = TRUE;
		     ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ  4: mouse_ಸಂಕೇತ = MOUSE_ಮಧ್ಯ;
		     WantQueryMouse = TRUE;
		     ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ  5: mouse_ಸಂಕೇತ = MOUSE_ಮಧ್ಯ;
		     is_release = TRUE;
		     ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ  6: mouse_ಸಂಕೇತ = MOUSE_ಬಲ;
		     WantQueryMouse = TRUE;
		     ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ  7: mouse_ಸಂಕೇತ = MOUSE_ಬಲ;
		     is_release = TRUE;
		     ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ  8: ಹಿಂತಿರುಗಿ -1; // fourth button down
	    ಸ್ಥಿತಿ  9: ಹಿಂತಿರುಗಿ -1; // fourth button up
	    ಸ್ಥಿತಿ 10: ಹಿಂತಿರುಗಿ -1; // mouse outside of filter rectangle
	    ಪೂರ್ವನಿಯೋಜಿತ: ಹಿಂತಿರುಗಿ -1; // should never occur
	}

	mouse_ಲಂಬಸಾಲು = Pc - 1;
	mouse_ಅಡ್ಡಸಾಲು = Pr - 1;

	*slen += (ಪೂರ್ಣಾಂಕ)(p - (tp + *slen));
    }
#ಆದರೆ_ಅಂತ್ಯ // FEAT_MOUSE_DEC
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MOUSE_PTERM
    ಆದರೆ (ಕೀಲಿ_name[0] == KS_PTERM_MOUSE)
    {
	ಪೂರ್ಣಾಂಕ button, ಅಂಕೆ_clicks, action;

	p = tp + *slen;

	action = getdigits(&p);
	ಆದರೆ (*p++ != ';')
	    ಹಿಂತಿರುಗಿ -1;

	mouse_ಅಡ್ಡಸಾಲು = getdigits(&p);
	ಆದರೆ (*p++ != ';')
	    ಹಿಂತಿರುಗಿ -1;
	mouse_ಲಂಬಸಾಲು = getdigits(&p);
	ಆದರೆ (*p++ != ';')
	    ಹಿಂತಿರುಗಿ -1;

	button = getdigits(&p);
	mouse_ಸಂಕೇತ = 0;

	ಆಯ್ಕೆ (button)
	{
	    ಸ್ಥಿತಿ 4: mouse_ಸಂಕೇತ = MOUSE_ಎಡ; ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ 1: mouse_ಸಂಕೇತ = MOUSE_ಬಲ; ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ 2: mouse_ಸಂಕೇತ = MOUSE_ಮಧ್ಯ; ನಿಲ್ಲಿಸು;
	    ಪೂರ್ವನಿಯೋಜಿತ: ಹಿಂತಿರುಗಿ -1;
	}

	ಆಯ್ಕೆ (action)
	{
	    ಸ್ಥಿತಿ 31: // Initial press
		ಆದರೆ (*p++ != ';')
		    ಹಿಂತಿರುಗಿ -1;

		ಅಂಕೆ_clicks = getdigits(&p); // Not used
		ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ 32: // Release
		is_release = TRUE;
		ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ 33: // Drag
		held_button = mouse_ಸಂಕೇತ;
		mouse_ಸಂಕೇತ |= MOUSE_DRAG;
		ನಿಲ್ಲಿಸು;

	    ಪೂರ್ವನಿಯೋಜಿತ:
		ಹಿಂತಿರುಗಿ -1;
	}

	ಆದರೆ (*p++ != 't')
	    ಹಿಂತಿರುಗಿ -1;

	*slen += (p - (tp + *slen));
    }
#ಆದರೆ_ಅಂತ್ಯ // FEAT_MOUSE_PTERM

    // Interpret the mouse code
    current_button = (mouse_ಸಂಕೇತ & MOUSE_CLICK_ಮುಖವಾಡ);
    ಆದರೆ (is_release)
	current_button |= MOUSE_RELEASE;

    ಆದರೆ (current_button == MOUSE_RELEASE
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MOUSE_XTERM
	    && wheel_ಸಂಕೇತ == 0
#ಆದರೆ_ಅಂತ್ಯ
       )
    {
	/*
	 * If we get a mouse drag or release event when there is no mouse
	 * button held down (held_button == MOUSE_RELEASE), produce a K_IGNORE
	 * below.
	 * (can happen when you hold down two buttons and then let them go, or
	 * click in the menu bar, but not on a menu, and drag into the text).
	 */
	ಆದರೆ ((mouse_ಸಂಕೇತ & MOUSE_DRAG) == MOUSE_DRAG)
	    is_drag = TRUE;
	current_button = held_button;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
      ಆದರೆ (wheel_ಸಂಕೇತ == 0)
      {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CHECK_DOUBLE_CLICK
# ifdef FEAT_MOUSE_GPM
	/*
	 * Only for Unix, when GUI not active, we handle multi-clicks here, but
	 * not for GPM mouse events.
	 */
#  ifdef FEAT_GUI
	ಆದರೆ (ಕೀಲಿ_name[0] != KS_GPM_MOUSE && !gui.ಒಳ_use)
#  ಇಲ್ಲದಿದ್ದರೆ
	    ಆದರೆ (ಕೀಲಿ_name[0] != KS_GPM_MOUSE)
#  endif
# ಇಲ್ಲದಿದ್ದರೆ
#  ifdef FEAT_GUI
		ಆದರೆ (!gui.ಒಳ_use)
#  endif
# endif
		{
		    /*
		     * Compute the time elapsed since the previous mouse click.
		     */
		    gettimeofday(&mouse_ಸಮಯ, ಖಾಲಿ);
		    ಆದರೆ (orig_mouse_ಸಮಯ.tv_sec == 0)
		    {
			/*
			 * Avoid computing the difference between mouse_time
			 * and orig_mouse_time for the first click, as the
			 * difference would be huge and would cause
			 * multiplication overflow.
			 */
			timediff = p_mouset;
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
			timediff = ಸಮಯ_diff_ms(&orig_mouse_ಸಮಯ, &mouse_ಸಮಯ);
		    orig_mouse_ಸಮಯ = mouse_ಸಮಯ;
		    ಆದರೆ (mouse_ಸಂಕೇತ == orig_mouse_ಸಂಕೇತ
			    && timediff < p_mouset
			    && orig_ಅಂಕೆ_clicks != 4
			    && orig_mouse_ಲಂಬಸಾಲು == mouse_ಲಂಬಸಾಲು
			    && orig_mouse_ಅಡ್ಡಸಾಲು == mouse_ಅಡ್ಡಸಾಲು
			    && (is_mouse_topline(curwin)
				// Double click in tab pages line also works
				// when window contents changes.
				|| (mouse_ಅಡ್ಡಸಾಲು == 0 && firstwin->w_winrow > 0))
		       )
			++orig_ಅಂಕೆ_clicks;
		    ಇಲ್ಲದಿದ್ದರೆ
			orig_ಅಂಕೆ_clicks = 1;
		    orig_mouse_ಲಂಬಸಾಲು = mouse_ಲಂಬಸಾಲು;
		    orig_mouse_ಅಡ್ಡಸಾಲು = mouse_ಅಡ್ಡಸಾಲು;
		    ಗಣ_mouse_topline(curwin);
		}
# ಆದರೆ defined(FEAT_GUI) || defined(FEAT_MOUSE_GPM)
		ಇಲ್ಲದಿದ್ದರೆ
		    orig_ಅಂಕೆ_clicks = ಅಂಕೆ_MOUSE_CLICKS(mouse_ಸಂಕೇತ);
# endif
#ಇಲ್ಲದಿದ್ದರೆ
	orig_ಅಂಕೆ_clicks = ಅಂಕೆ_MOUSE_CLICKS(mouse_ಸಂಕೇತ);
#ಆದರೆ_ಅಂತ್ಯ
	is_click = TRUE;
      }
      orig_mouse_ಸಂಕೇತ = mouse_ಸಂಕೇತ;
    }
    ಆದರೆ (!is_drag)
	held_button = mouse_ಸಂಕೇತ & MOUSE_CLICK_ಮುಖವಾಡ;

    /*
     * Translate the actual mouse event into a pseudo mouse event.
     * First work out what modifiers are to be used.
     */
    ಆದರೆ (orig_mouse_ಸಂಕೇತ & MOUSE_ಸರಿಸು)
	*modifiers |= MOD_ಮುಖವಾಡ_ಸರಿಸು;
    ಆದರೆ (orig_mouse_ಸಂಕೇತ & MOUSE_CTRL)
	*modifiers |= MOD_ಮುಖವಾಡ_CTRL;
    ಆದರೆ (orig_mouse_ಸಂಕೇತ & MOUSE_ALT)
	*modifiers |= MOD_ಮುಖವಾಡ_ALT;
    ಆದರೆ (orig_ಅಂಕೆ_clicks == 2)
	*modifiers |= MOD_ಮುಖವಾಡ_2_CLICK;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (orig_ಅಂಕೆ_clicks == 3)
	*modifiers |= MOD_ಮುಖವಾಡ_3_CLICK;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (orig_ಅಂಕೆ_clicks == 4)
	*modifiers |= MOD_ಮುಖವಾಡ_4_CLICK;

    // Work out our pseudo mouse event. Note that MOUSE_RELEASE gets added,
    // then it's not mouse up/down.
    ಕೀಲಿ_name[0] = KS_EXTRA;
    ಆದರೆ (wheel_ಸಂಕೇತ != 0 && (!is_release || release_is_ambiguous))
    {
	ಆದರೆ (wheel_ಸಂಕೇತ & MOUSE_CTRL)
	    *modifiers |= MOD_ಮುಖವಾಡ_CTRL;
	ಆದರೆ (wheel_ಸಂಕೇತ & MOUSE_ALT)
	    *modifiers |= MOD_ಮುಖವಾಡ_ALT;

	ಆದರೆ (wheel_ಸಂಕೇತ & 1 && wheel_ಸಂಕೇತ & 2)
	    ಕೀಲಿ_name[1] = (ಪೂರ್ಣಾಂಕ)KE_MOUSELEFT;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (wheel_ಸಂಕೇತ & 2)
	    ಕೀಲಿ_name[1] = (ಪೂರ್ಣಾಂಕ)KE_MOUSERIGHT;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (wheel_ಸಂಕೇತ & 1)
	    ಕೀಲಿ_name[1] = (ಪೂರ್ಣಾಂಕ)KE_MOUSEUP;
	ಇಲ್ಲದಿದ್ದರೆ
	    ಕೀಲಿ_name[1] = (ಪೂರ್ಣಾಂಕ)KE_MOUSEDOWN;

	held_button = MOUSE_RELEASE;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
#ಆದರೆ defined(UNIX)
	ಆದರೆ (use_xterm_mouse() && orig_mouse_ಸಂಕೇತ >= MOUSESIDEBUTTONS_ಕೆಳಗಿನ)
	    current_button = (current_button) ? MOUSE_X2 : MOUSE_X1;
#ಆದರೆ_ಅಂತ್ಯ
	ಕೀಲಿ_name[1] = get_pseudo_mouse_ಸಂಕೇತ(current_button, is_click, is_drag);
    }


    // Make sure the mouse position is valid.  Some terminals may return weird
    // values.
    ಆದರೆ (mouse_ಲಂಬಸಾಲು >= Columns)
	mouse_ಲಂಬಸಾಲು = Columns - 1;
    ಆದರೆ (mouse_ಅಡ್ಡಸಾಲು >= ಅಡ್ಡಸಾಲುಗಳು)
	mouse_ಅಡ್ಡಸಾಲು = ಅಡ್ಡಸಾಲುಗಳು - 1;

    ಹಿಂತಿರುಗಿ 0;
}

// Functions also used for popup windows.

/*
 * Compute the buffer line position from the screen position "rowp" / "colp" in
 * window "win".
 * "plines_cache" can be NULL (no cache) or an array with "Rows" entries that
 * caches the plines_win() result from a previous call.  Entry is zero if not
 * computed yet.  There must be no text or setting changes since the entry is
 * put in the cache.
 * Returns TRUE if the position is below the last line.
 */
    ಪೂರ್ಣಾಂಕ
mouse_comp_pos(
    win_T	*win,
    ಪೂರ್ಣಾಂಕ		*rowp,
    ಪೂರ್ಣಾಂಕ		*colp,
    linenr_T	*lnump,
    ಪೂರ್ಣಾಂಕ		*plines_cache)
{
    ಪೂರ್ಣಾಂಕ		ಲಂಬಸಾಲು = *colp;
    ಪೂರ್ಣಾಂಕ		ಅಡ್ಡಸಾಲು = *rowp;
    linenr_T	lnum;
    ಪೂರ್ಣಾಂಕ		retval = FALSE;
    ಪೂರ್ಣಾಂಕ		off;
    ಪೂರ್ಣಾಂಕ		ಎಣಿಕೆ_2;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RIGHTLEFT
    ಆದರೆ (win->w_p_rl)
	ಲಂಬಸಾಲು = win->w_ಅಗಲ - 1 - ಲಂಬಸಾಲು;
#ಆದರೆ_ಅಂತ್ಯ

    lnum = win->w_topline;

    ಆಗ (ಅಡ್ಡಸಾಲು > 0)
    {
	ಪೂರ್ಣಾಂಕ cache_idx = lnum - win->w_topline;

	// Only "Rows" lines are cached, with folding we'll run out of entries
	// and use the slow way.
	ಆದರೆ (plines_cache != ಖಾಲಿ && cache_idx < ಅಡ್ಡಸಾಲುಗಳು
						&& plines_cache[cache_idx] > 0)
	    ಎಣಿಕೆ_2 = plines_cache[cache_idx];
	ಇಲ್ಲದಿದ್ದರೆ
	{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
	    // Don't include filler lines in "count"
	    ಆದರೆ (win->w_p_diff
# ifdef FEAT_FOLDING
		    && !hasFoldingWin(win, lnum, ಖಾಲಿ, ಖಾಲಿ, TRUE, ಖಾಲಿ)
# endif
		    )
	    {
		ಆದರೆ (lnum == win->w_topline)
		    ಅಡ್ಡಸಾಲು -= win->w_topfill;
		ಇಲ್ಲದಿದ್ದರೆ
		    ಅಡ್ಡಸಾಲು -= diff_check_fill(win, lnum);
		ಎಣಿಕೆ_2 = plines_win_nofill(win, lnum, FALSE);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
		ಎಣಿಕೆ_2 = plines_win(win, lnum, FALSE);
	    ಆದರೆ (plines_cache != ಖಾಲಿ && cache_idx < ಅಡ್ಡಸಾಲುಗಳು)
		plines_cache[cache_idx] = ಎಣಿಕೆ_2;
	}

	ಆದರೆ (win->w_skipcol > 0 && lnum == win->w_topline)
	{
	    ಪೂರ್ಣಾಂಕ width1 = win->w_ಅಗಲ - win_ಲಂಬಸಾಲು_off(win);

	    ಆದರೆ (width1 > 0)
	    {
		ಪೂರ್ಣಾಂಕ skip_ಸಾಲುಗಳು = 0;

		// Adjust for 'smoothscroll' clipping the top screen lines.
		// A similar formula is used in curs_columns().
		ಆದರೆ (win->w_skipcol > width1)
		    skip_ಸಾಲುಗಳು = (win->w_skipcol - width1)
					    / (width1 + win_ಲಂಬಸಾಲು_off2(win)) + 1;
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (win->w_skipcol > 0)
		    skip_ಸಾಲುಗಳು = 1;

		ಎಣಿಕೆ_2 -= skip_ಸಾಲುಗಳು;
	    }
	}

	ಆದರೆ (ಎಣಿಕೆ_2 > ಅಡ್ಡಸಾಲು)
	    ನಿಲ್ಲಿಸು;	// Position is in this buffer line.
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
	(ಶೂನ್ಯ)hasFoldingWin(win, lnum, ಖಾಲಿ, &lnum, TRUE, ಖಾಲಿ);
#ಆದರೆ_ಅಂತ್ಯ
	ಆದರೆ (lnum == win->w_ಬಫರ್->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	{
	    retval = TRUE;
	    ನಿಲ್ಲಿಸು;		// past end of file
	}
	ಅಡ್ಡಸಾಲು -= ಎಣಿಕೆ_2;
	++lnum;
    }

    ಆದರೆ (!retval)
    {
	// Compute the column without wrapping.
	off = win_ಲಂಬಸಾಲು_off(win) - win_ಲಂಬಸಾಲು_off2(win);
	ಆದರೆ (ಲಂಬಸಾಲು < off)
	    ಲಂಬಸಾಲು = off;
	ಲಂಬಸಾಲು += ಅಡ್ಡಸಾಲು * (win->w_ಅಗಲ - off);

	// Add skip column for the topline.
	ಆದರೆ (lnum == win->w_topline)
	    ಲಂಬಸಾಲು += win->w_skipcol;
    }

    ಆದರೆ (!win->w_p_wrap)
	ಲಂಬಸಾಲು += win->w_leftcol;

    // skip line number and fold column in front of the line
    ಲಂಬಸಾಲು -= win_ಲಂಬಸಾಲು_off(win);
    ಆದರೆ (ಲಂಬಸಾಲು <= 0)
    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_NETBEANS_INTG
	// if mouse is clicked on the gutter, then inform the netbeans server
	ಆದರೆ (*colp < win_ಲಂಬಸಾಲು_off(win))
	    netbeans_gutter_click(lnum);
#ಆದರೆ_ಅಂತ್ಯ
	ಲಂಬಸಾಲು = 0;
    }

    *colp = ಲಂಬಸಾಲು;
    *rowp = ಅಡ್ಡಸಾಲು;
    *lnump = lnum;
    ಹಿಂತಿರುಗಿ retval;
}

/*
 * Find the window at screen position "*rowp" and "*colp".  The positions are
 * updated to become relative to the top-left of the window.
 * When "popup" is FAIL_POPUP and the position is in a popup window then NULL
 * is returned.  When "popup" is IGNORE_POPUP then do not even check popup
 * windows.
 * Returns NULL when something is wrong.
 */
    win_T *
mouse_ಕಂಡುಹಿಡಿ_win(ಪೂರ್ಣಾಂಕ *rowp, ಪೂರ್ಣಾಂಕ *colp, mouse_ಕಂಡುಹಿಡಿ_T popup ಬಳಸದ)
{
    ಚೌಕಟ್ಟು_T	*fp;
    win_T	*wp;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
    win_T	*pwp = ಖಾಲಿ;

    ಆದರೆ (popup != IGNORE_POPUP)
    {
	popup_reset_handled(POPUP_HANDLED_1);
	ಆಗ ((wp = ಕಂಡುಹಿಡಿ_ಮುಂದಿನ_popup(TRUE, POPUP_HANDLED_1)) != ಖಾಲಿ)
	{
	    ಆದರೆ (*rowp >= wp->w_winrow && *rowp < wp->w_winrow + popup_ಎತ್ತರ(wp)
		    && *colp >= wp->w_wincol
				    && *colp < wp->w_wincol + popup_ಅಗಲ(wp))
		pwp = wp;
	}
	ಆದರೆ (pwp != ಖಾಲಿ)
	{
	    ಆದರೆ (popup == FAIL_POPUP)
		ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	    *rowp -= pwp->w_winrow;
	    *colp -= pwp->w_wincol;
	    ಹಿಂತಿರುಗಿ pwp;
	}
    }
#ಆದರೆ_ಅಂತ್ಯ

    fp = topframe;

    ಆದರೆ (*colp < firstwin->w_wincol
	    || *colp >= firstwin->w_wincol + fp->fr_ಅಗಲ
	    || *rowp < firstwin->w_winrow)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    *rowp -= firstwin->w_winrow;
    *colp -= firstwin->w_wincol;
    ಗಾಗಿ (;;)
    {
	ಆದರೆ (fp->fr_layout == FR_ಎಲೆ)
	    ನಿಲ್ಲಿಸು;
	ಆದರೆ (fp->fr_layout == FR_ಅಡ್ಡಸಾಲು)
	{
	    ಗಾಗಿ (fp = fp->fr_ಮಗು; fp->fr_ಮುಂದಿನ != ಖಾಲಿ; fp = fp->fr_ಮುಂದಿನ)
	    {
		ಆದರೆ (*colp < fp->fr_ಅಗಲ)
		    ನಿಲ್ಲಿಸು;
		*colp -= fp->fr_ಅಗಲ;
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ    // fr_layout == FR_COL
	{
	    ಗಾಗಿ (fp = fp->fr_ಮಗು; fp->fr_ಮುಂದಿನ != ಖಾಲಿ; fp = fp->fr_ಮುಂದಿನ)
	    {
		ಆದರೆ (*rowp < fp->fr_ಎತ್ತರ)
		    ನಿಲ್ಲಿಸು;
		*rowp -= fp->fr_ಎತ್ತರ;
	    }
	}
    }
    // When using a timer that closes a window the window might not actually
    // exist.
    FOR_ALL_WINDOWS(wp)
	ಆದರೆ (wp == fp->fr_win)
	{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MENU
	    *rowp -= wp->w_winbar_ಎತ್ತರ;
#ಆದರೆ_ಅಂತ್ಯ
	    ಹಿಂತಿರುಗಿ wp;
	}
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

#ಆದರೆ defined(NEED_VCOL2_ಲಂಬಸಾಲು) || defined(FEAT_BEVAL) || defined(FEAT_PROP_POPUP) \
	|| defined(FEAT_EVAL)
/*
 * Convert a virtual (screen) column to a character column.
 * The first column is zero.
 */
    ಪೂರ್ಣಾಂಕ
vcol2col(win_T *wp, linenr_T lnum, ಪೂರ್ಣಾಂಕ vcol, colnr_T *coladdp)
{
    char_u	    *ಸಾಲು;
    chartabsize_T   cts;

    // try to advance to the specified column
    ಸಾಲು = ml_get_buf(wp->w_ಬಫರ್, lnum, FALSE);
    init_chartabsize_arg(&cts, wp, lnum, 0, ಸಾಲು, ಸಾಲು);
    ಆಗ (cts.cts_vcol < vcol && *cts.cts_ptr != NUL)
    {
	ಪೂರ್ಣಾಂಕ ಗಾತ್ರ_2 = win_lbr_chartabsize(&cts, ಖಾಲಿ, ಖಾಲಿ);
	ಆದರೆ (cts.cts_vcol + ಗಾತ್ರ_2 > vcol)
	    ನಿಲ್ಲಿಸು;
	cts.cts_vcol += ಗಾತ್ರ_2;
	MB_PTR_ADV(cts.cts_ptr);
    }
    clear_chartabsize_arg(&cts);

    ಆದರೆ (coladdp != ಖಾಲಿ)
	*coladdp = vcol - cts.cts_vcol;
    ಹಿಂತಿರುಗಿ (ಪೂರ್ಣಾಂಕ)(cts.cts_ptr - ಸಾಲು);
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_EVAL)
/*
 * "getmousepos()" function.
 */
    ಶೂನ್ಯ
f_getmousepos(typval_T *argvars ಬಳಸದ, typval_T *rettv)
{
    dict_T	*d;
    win_T	*wp;
    ಪೂರ್ಣಾಂಕ		ಅಡ್ಡಸಾಲು = mouse_ಅಡ್ಡಸಾಲು;
    ಪೂರ್ಣಾಂಕ		ಲಂಬಸಾಲು = mouse_ಲಂಬಸಾಲು;
    varnumber_T winid = 0;
    varnumber_T winrow = 0;
    varnumber_T wincol = 0;
    linenr_T	lnum = 0;
    varnumber_T ಸ್ತಂಭ = 0;
    colnr_T	coladd = 0;

    ಆದರೆ (rettv_dict_alloc(rettv) == FAIL)
	ಹಿಂತಿರುಗಿ;
    d = rettv->vval.v_dict;

    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(d, "screenrow", (varnumber_T)mouse_ಅಡ್ಡಸಾಲು + 1);
    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(d, "screencol", (varnumber_T)mouse_ಲಂಬಸಾಲು + 1);

    wp = mouse_ಕಂಡುಹಿಡಿ_win(&ಅಡ್ಡಸಾಲು, &ಲಂಬಸಾಲು, ಕಂಡುಹಿಡಿ_POPUP);
    ಆದರೆ (wp != ಖಾಲಿ)
    {
	ಪೂರ್ಣಾಂಕ	ತುದಿ_off = 0;
	ಪೂರ್ಣಾಂಕ	ಎಡ_off = 0;
	ಪೂರ್ಣಾಂಕ	ಎತ್ತರ = wp->w_ಎತ್ತರ + wp->w_ಸ್ಥಿತಿ_ಎತ್ತರ;

# ifdef FEAT_PROP_POPUP
	ಆದರೆ (WIN_IS_POPUP(wp))
	{
	    ತುದಿ_off = popup_ತುದಿ_extra(wp);
	    ಎಡ_off = popup_ಎಡ_extra(wp);
	    ಎತ್ತರ = popup_ಎತ್ತರ(wp);
	}
# endif
	ಆದರೆ (ಅಡ್ಡಸಾಲು < ಎತ್ತರ)
	{
	    winid = wp->w_id;
	    winrow = ಅಡ್ಡಸಾಲು + 1;
	    wincol = ಲಂಬಸಾಲು + 1;
	    ಅಡ್ಡಸಾಲು -= ತುದಿ_off;
	    ಲಂಬಸಾಲು -= ಎಡ_off;
	    ಆದರೆ (ಅಡ್ಡಸಾಲು >= 0 && ಅಡ್ಡಸಾಲು < wp->w_ಎತ್ತರ && ಲಂಬಸಾಲು >= 0 && ಲಂಬಸಾಲು < wp->w_ಅಗಲ)
	    {
		(ಶೂನ್ಯ)mouse_comp_pos(wp, &ಅಡ್ಡಸಾಲು, &ಲಂಬಸಾಲು, &lnum, ಖಾಲಿ);
		ಲಂಬಸಾಲು = vcol2col(wp, lnum, ಲಂಬಸಾಲು, &coladd);
		ಸ್ತಂಭ = ಲಂಬಸಾಲು + 1;
	    }
	}
    }
    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(d, "winid", winid);
    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(d, "winrow", winrow);
    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(d, "wincol", wincol);
    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(d, "line", (varnumber_T)lnum);
    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(d, "column", ಸ್ತಂಭ);
    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(d, "coladd", coladd);
}
#ಆದರೆ_ಅಂತ್ಯ
