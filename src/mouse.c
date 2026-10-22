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

#ਅਨ੍ਤਰ੍ਭਾਵ "vim.h"

/*
 * Horiziontal and vertical steps used when scrolling.
 * When negative scroll by a whole page.
 */
ਸਥਿਰ ਲੰਮਾ mouse_hor_ਕਦਮ = 6;
ਸਥਿਰ ਲੰਮਾ mouse_vert_ਕਦਮ = 3;
ਸਥਿਰ win_T *dragwin = ਨਿਰਾ;	// window being dragged
ਸਥਿਰ ਪੂਰਨ stl_click_handler(win_T *wp, ਪੂਰਨ mrow, ਪੂਰਨ mcol, ਪੂਰਨ which_button,
								    ਪੂਰਨ mods);
ਸਥਿਰ ਪੂਰਨ stl_click_handler_regions(stl_click_region_T *regions,
				    ਪੂਰਨ region_ਗਿਣਤੀ, ਪੂਰਨ winid,
				    char_u *area_name, ਪੂਰਨ mrow, ਪੂਰਨ mcol,
				    ਪੂਰਨ which_button, ਪੂਰਨ mods);

    ਖਾਲੀ
mouse_ਸੈੱਟ_vert_scroll_ਕਦਮ(ਲੰਮਾ ਕਦਮ)
{
    mouse_vert_ਕਦਮ = ਕਦਮ;
}

    ਖਾਲੀ
mouse_ਸੈੱਟ_hor_scroll_ਕਦਮ(ਲੰਮਾ ਕਦਮ)
{
    mouse_hor_ਕਦਮ = ਕਦਮ;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CHECK_DOUBLE_CLICK
/*
 * Return the duration from t1 to t2 in milliseconds.
 */
    ਸਥਿਰ ਲੰਮਾ
ਸਮਾਂ_diff_ms(ਬਣਤਰ timeval *t1, ਬਣਤਰ timeval *t2)
{
    // This handles wrapping of tv_usec correctly without any special case.
    // Example of 2 pairs (tv_sec, tv_usec) with a duration of 5 ms:
    //	   t1 = (1, 998000) t2 = (2, 3000) gives:
    //	   (2 - 1) * 1000 + (3000 - 998000) / 1000 -> 5 ms.
    ਵਾਪਸ (t2->tv_sec - t1->tv_sec) * 1000
	 + (t2->tv_usec - t1->tv_usec) / 1000;
}
#ਜੇ_ਅੰਤ

/*
 * Get class of a character for selection: same class means same word.
 * 0: blank
 * 1: punctuation groups
 * 2: normal word character
 * >2: multi-byte word character.
 */
    ਸਥਿਰ ਪੂਰਨ
get_mouse_class(char_u *p)
{
    ਪੂਰਨ		c;

    ਜੇ (has_mbyte && MB_BYTE2LEN(p[0]) > 1)
	ਵਾਪਸ mb_get_class(p);

    c = *p;
    ਜੇ (c == ' ' || c == '\t')
	ਵਾਪਸ 0;

    ਜੇ (vim_iswordc(c))
	ਵਾਪਸ 2;

    // There are a few special cases where we want certain combinations of
    // characters to be considered as a single word.  These are things like
    // "->", "/ *", "*=", "+=", "&=", "<=", ">=", "!=" etc.  Otherwise, each
    // character is in its own class.
    ਜੇ (c != NUL && vim_strchr((char_u *)"-+*/%<>&|^!=", c) != ਨਿਰਾ)
	ਵਾਪਸ 1;
    ਵਾਪਸ c;
}

/*
 * Move "pos" back to the start of the word it's in.
 */
    ਸਥਿਰ ਖਾਲੀ
ਲੱਭੋ_ਸ਼ੁਰੂ_ਦਾ_ਸ਼ਬਦ(pos_T *pos)
{
    char_u	*ਸਤਰ;
    ਪੂਰਨ		cclass;
    ਪੂਰਨ		ਖਾਨਾ;

    ਸਤਰ = ml_get(pos->lnum);
    cclass = get_mouse_class(ਸਤਰ + pos->ਖਾਨਾ);

    ਜਦੋਂ (pos->ਖਾਨਾ > 0)
    {
	ਖਾਨਾ = pos->ਖਾਨਾ - 1;
	ਖਾਨਾ -= (*mb_ਸਿਰ_off)(ਸਤਰ, ਸਤਰ + ਖਾਨਾ);
	ਜੇ (get_mouse_class(ਸਤਰ + ਖਾਨਾ) != cclass)
	    ਰੋਕੋ;
	pos->ਖਾਨਾ = ਖਾਨਾ;
    }
}

/*
 * Move "pos" forward to the end of the word it's in.
 * When 'selection' is "exclusive", the position is just after the word.
 */
    ਸਥਿਰ ਖਾਲੀ
ਲੱਭੋ_ਸਿਰਾ_ਦਾ_ਸ਼ਬਦ(pos_T *pos)
{
    char_u	*ਸਤਰ;
    ਪੂਰਨ		cclass;
    ਪੂਰਨ		ਖਾਨਾ;

    ਸਤਰ = ml_get(pos->lnum);
    ਜੇ (*p_sel == 'e' && pos->ਖਾਨਾ > 0)
    {
	--pos->ਖਾਨਾ;
	pos->ਖਾਨਾ -= (*mb_ਸਿਰ_off)(ਸਤਰ, ਸਤਰ + pos->ਖਾਨਾ);
    }
    cclass = get_mouse_class(ਸਤਰ + pos->ਖਾਨਾ);
    ਜਦੋਂ (ਸਤਰ[pos->ਖਾਨਾ] != NUL)
    {
	ਖਾਨਾ = pos->ਖਾਨਾ + (*mb_ptr2len)(ਸਤਰ + pos->ਖਾਨਾ);
	ਜੇ (get_mouse_class(ਸਤਰ + ਖਾਨਾ) != cclass)
	{
	    ਜੇ (*p_sel == 'e')
		pos->ਖਾਨਾ = ਖਾਨਾ;
	    ਰੋਕੋ;
	}
	pos->ਖਾਨਾ = ਖਾਨਾ;
    }
}

#ਜੇ defined(FEAT_GUI_MOTIF) || defined(FEAT_GUI_GTK) \
	    || defined(FEAT_GUI_MSWIN) \
	    || defined(FEAT_GUI_PHOTON) \
	    || defined(FEAT_ਪਦ_POPUP_MENU)
# define USE_POPUP_SETPOS
# define NEED_VCOL2_ਖਾਨਾ

/*
 * Translate window coordinates to buffer position without any side effects.
 * Returns IN_BUFFER and sets "mpos->col" to the column when in buffer text.
 * The column is one for the first column.
 */
    ਸਥਿਰ ਪੂਰਨ
get_fpos_ਦਾ_mouse(pos_T *mpos)
{
    win_T	*wp;
    ਪੂਰਨ		ਪੰਕਤੀ = mouse_ਪੰਕਤੀ;
    ਪੂਰਨ		ਖਾਨਾ = mouse_ਖਾਨਾ;

    ਜੇ (ਪੰਕਤੀ < 0 || ਖਾਨਾ < 0)		// check if it makes sense
	ਵਾਪਸ ਵਿੱਚ_UNKNOWN;

    // find the window where the row is in
    wp = mouse_ਲੱਭੋ_win(&ਪੰਕਤੀ, &ਖਾਨਾ, FAIL_POPUP);
    ਜੇ (wp == ਨਿਰਾ)
	ਵਾਪਸ ਵਿੱਚ_UNKNOWN;
    // winpos and height may change in win_enter()!
    ਜੇ (ਪੰਕਤੀ >= wp->w_ਉਚਾਈ)	// In (or below) status line
	ਵਾਪਸ ਵਿੱਚ_ਸਥਿਤੀ_ਸਤਰ;
    ਜੇ (ਖਾਨਾ >= wp->w_ਚੌੜਾਈ)	// In vertical separator line
	ਵਾਪਸ ਵਿੱਚ_SEP_ਸਤਰ;

    ਜੇ (wp != curwin)
	ਵਾਪਸ ਵਿੱਚ_UNKNOWN;

    // compute the position in the buffer line from the posn on the screen
    ਜੇ (mouse_comp_pos(curwin, &ਪੰਕਤੀ, &ਖਾਨਾ, &mpos->lnum, ਨਿਰਾ))
	ਵਾਪਸ ਵਿੱਚ_ਸਥਿਤੀ_ਸਤਰ; // past bottom

    mpos->ਖਾਨਾ = vcol2col(wp, mpos->lnum, ਖਾਨਾ, &mpos->coladd);
    ਵਾਪਸ ਵਿੱਚ_ਬਫਰ;
}
#ਜੇ_ਅੰਤ

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
    ਪੂਰਨ
do_mouse(
    oparg_T	*oap,		// operator argument, can be NULL
    ਪੂਰਨ		c,		// K_LEFTMOUSE, etc
    ਪੂਰਨ		dir,		// Direction to 'put' if necessary
    ਲੰਮਾ	ਗਿਣਤੀ_2,
    ਪੂਰਨ		fixindent)	// PUT_FIXINDENT if fixing indent necessary
{
    ਸਥਿਰ ਪੂਰਨ	do_always = FALSE;	// ignore 'mouse' setting next time
    ਸਥਿਰ ਪੂਰਨ	got_click = FALSE;	// got a click some time back

    ਪੂਰਨ		which_button;	// MOUSE_LEFT, _MIDDLE or _RIGHT
    ਪੂਰਨ		is_click = FALSE; // If FALSE it's a drag or release event
    ਪੂਰਨ		is_drag = FALSE;  // If TRUE it's a drag event
    ਪੂਰਨ		jump_flags = 0;	// flags for jump_to_mouse()
    pos_T	ਸ਼ੁਰੂ_visual;
    ਪੂਰਨ		moved;		// Has cursor moved?
    ਪੂਰਨ		ਵਿੱਚ_ਸਥਿਤੀ_ਸਤਰ_2;	// mouse in status line
    ਸਥਿਰ ਪੂਰਨ	ਵਿੱਚ_tab_ਸਤਰ = FALSE; // mouse clicked in tab line
    ਸਥਿਰ ਪੂਰਨ	ਵਿੱਚ_tabpanel = FALSE; // mouse clicked in tabpanel
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TABPANEL
    ਸਥਿਰ bool	ਵਿੱਚ_tabpanel_scrollbar = false; // dragging tabpanel scrollbar
#ਜੇ_ਅੰਤ
    ਪੂਰਨ		ਵਿੱਚ_sep_ਸਤਰ;	// mouse in vertical separator line
    ਪੂਰਨ		c1, c2;
#ਜੇ defined(FEAT_FOLDING)
    pos_T	save_cursor;
#ਜੇ_ਅੰਤ
    win_T	*old_curwin = curwin;
    ਸਥਿਰ pos_T orig_cursor;
    colnr_T	leftcol, rightcol;
    pos_T	ਸਿਰਾ_visual;
    ਪੂਰਨ		diff;
    ਪੂਰਨ		old_ਸਰਗਰਮ = VIsual_ਸਰਗਰਮ;
    ਪੂਰਨ		old_ਬਹੁਲਕ = VIsual_ਬਹੁਲਕ;
    ਪੂਰਨ		regname;

#ਜੇ defined(FEAT_FOLDING)
    save_cursor = curwin->w_cursor;
#ਜੇ_ਅੰਤ

    // When GUI is active, always recognize mouse events, otherwise:
    // - Ignore mouse event in normal mode if 'mouse' doesn't include 'n'.
    // - Ignore mouse event in visual mode if 'mouse' doesn't include 'v'.
    // - For command line and insert mode 'mouse' is checked before calling
    //	 do_mouse().
    ਜੇ (do_always)
	do_always = FALSE;
    ਨਹੀਂਤਾਂ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
	ਜੇ (!gui.ਵਿੱਚ_use)
#ਜੇ_ਅੰਤ
	{
	    ਜੇ (VIsual_ਸਰਗਰਮ)
	    {
		ਜੇ (!mouse_has(MOUSE_VISUAL))
		    ਵਾਪਸ FALSE;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (State == ਬਹੁਲਕ_ਸਾਧਾਰਨ && !mouse_has(MOUSE_ਸਾਧਾਰਨ))
		ਵਾਪਸ FALSE;
	}

    ਲਈ (;;)
    {
	which_button = get_mouse_button(KEY2TERMCAP1(c), &is_click, &is_drag);
	ਜੇ (is_drag)
	{
	    // If the next character is the same mouse event then use that
	    // one. Speeds up dragging the status line.
	    // Note: Since characters added to the stuff buffer in the code
	    // below need to come before the next character, do not do this
	    // when the current character was stuffed.
	    ਜੇ (!ਕੁੰਜੀ_Stuffed && vpeekc() != NUL)
	    {
		ਪੂਰਨ nc;
		ਪੂਰਨ save_mouse_ਪੰਕਤੀ = mouse_ਪੰਕਤੀ;
		ਪੂਰਨ save_mouse_ਖਾਨਾ = mouse_ਖਾਨਾ;

		// Need to get the character, peeking doesn't get the actual
		// one.
		nc = ਸੁਰੱਖਿਅਤ_vgetc();
		ਜੇ (c == nc)
		    ਜਾਰੀ;
		vungetc(nc);
		mouse_ਪੰਕਤੀ = save_mouse_ਪੰਕਤੀ;
		mouse_ਖਾਨਾ = save_mouse_ਖਾਨਾ;
	    }
	}
	ਰੋਕੋ;
    }

    ਜੇ (c == K_MOUSEMOVE)
    {
	// Mouse moved without a button pressed.
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_BEVAL_ਪਦ
	ui_may_remove_balloon();
	ਜੇ (p_bevalterm)
	{
	    profile_setlimit(p_bdlay, &bevalexpr_due);
	    bevalexpr_due_ਸੈੱਟ = TRUE;
	}
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
	popup_handle_mouse_moved();
#ਜੇ_ਅੰਤ
	ਵਾਪਸ FALSE;
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MOUSESHAPE
    // May have stopped dragging the status or separator line.  The pointer is
    // most likely still on the status or separator line.
    ਜੇ (!is_drag && drag_ਸਥਿਤੀ_ਸਤਰ)
    {
	drag_ਸਥਿਤੀ_ਸਤਰ = FALSE;
	update_mouseshape(SHAPE_IDX_ਸਥਿਤੀ);
    }
    ਜੇ (!is_drag && drag_sep_ਸਤਰ)
    {
	drag_sep_ਸਤਰ = FALSE;
	update_mouseshape(SHAPE_IDX_VSEP);
    }
#ਜੇ_ਅੰਤ

    // Ignore drag and release events if we didn't get a click.
    ਜੇ (is_click)
    {
	got_click = TRUE;
	ਵਿੱਚ_tab_ਸਤਰ = FALSE;
	ਵਿੱਚ_tabpanel = FALSE;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TABPANEL
	ਵਿੱਚ_tabpanel_scrollbar = false;
#ਜੇ_ਅੰਤ
    }
    ਨਹੀਂਤਾਂ
    {
	ਜੇ (!got_click)			// didn't get click, ignore
	    ਵਾਪਸ FALSE;
	ਜੇ (!is_drag)			// release, reset got_click
	{
	    got_click = FALSE;
	    ਜੇ (ਵਿੱਚ_tab_ਸਤਰ || ਵਿੱਚ_tabpanel
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TABPANEL
		    || ਵਿੱਚ_tabpanel_scrollbar
#ਜੇ_ਅੰਤ
		    )
	    {
		ਵਿੱਚ_tab_ਸਤਰ = FALSE;
		ਵਿੱਚ_tabpanel = FALSE;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TABPANEL
		ਵਿੱਚ_tabpanel_scrollbar = false;
#ਜੇ_ਅੰਤ
		ਵਾਪਸ FALSE;
	    }
	}
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TABPANEL
    // Continue a scrollbar drag before any tab-selection handling.
    ਜੇ (is_drag && ਵਿੱਚ_tabpanel_scrollbar)
    {
	tabpanel_drag_scrollbar(mouse_ਪੰਕਤੀ);
	ਵਾਪਸ FALSE;
    }
#ਜੇ_ਅੰਤ

    // CTRL right mouse button does CTRL-T
    ਜੇ (is_click && (mod_ਨਕਾਬ & MOD_ਨਕਾਬ_CTRL) && which_button == MOUSE_ਸੱਜਾ)
    {
	ਜੇ (State & ਬਹੁਲਕ_ਪਾਓ)
	    stuffcharReadbuff(Ctrl_O);
	ਜੇ (ਗਿਣਤੀ_2 > 1)
	    stuffnumReadbuff(ਗਿਣਤੀ_2);
	stuffcharReadbuff(Ctrl_T);
	got_click = FALSE;		// ignore drag&release now
	ਵਾਪਸ FALSE;
    }

    // CTRL only works with left mouse button
    ਜੇ ((mod_ਨਕਾਬ & MOD_ਨਕਾਬ_CTRL) && which_button != MOUSE_ਖੱਬਾ)
	ਵਾਪਸ FALSE;

    // When a modifier is down, ignore drag and release events, as well as
    // multiple clicks and the middle mouse button.
    // Accept shift-leftmouse drags when 'mousemodel' is "popup.*".
    ਜੇ ((mod_ਨਕਾਬ & (MOD_ਨਕਾਬ_ਖਿਸਕਾਓ | MOD_ਨਕਾਬ_CTRL | MOD_ਨਕਾਬ_ALT
							     | MOD_ਨਕਾਬ_META))
	    && (!is_click
		|| (mod_ਨਕਾਬ & MOD_ਨਕਾਬ_MULTI_CLICK)
		|| which_button == MOUSE_ਵਿਚਕਾਰ)
	    && !((mod_ਨਕਾਬ & (MOD_ਨਕਾਬ_ਖਿਸਕਾਓ|MOD_ਨਕਾਬ_ALT))
		&& mouse_model_popup()
		&& which_button == MOUSE_ਖੱਬਾ)
	    && !((mod_ਨਕਾਬ & MOD_ਨਕਾਬ_ALT)
		&& !mouse_model_popup()
		&& which_button == MOUSE_ਸੱਜਾ)
	    )
	ਵਾਪਸ FALSE;

    // If the button press was used as the movement command for an operator
    // (eg "d<MOUSE>"), or it is the middle button that is held down, ignore
    // drag/release events.
    ਜੇ (!is_click && which_button == MOUSE_ਵਿਚਕਾਰ)
	ਵਾਪਸ FALSE;

    ਜੇ (oap != ਨਿਰਾ)
	regname = oap->regname;
    ਨਹੀਂਤਾਂ
	regname = 0;

    // Middle mouse button does a 'put' of the selected text
    ਜੇ (which_button == MOUSE_ਵਿਚਕਾਰ)
    {
	ਜੇ (State == ਬਹੁਲਕ_ਸਾਧਾਰਨ)
	{
	    // If an operator was pending, we don't know what the user wanted
	    // to do. Go back to normal mode: Clear the operator and beep().
	    ਜੇ (oap != ਨਿਰਾ && oap->op_type != OP_NOP)
	    {
		clearopbeep(oap);
		ਵਾਪਸ FALSE;
	    }

	    // If visual was active, yank the highlighted text and put it
	    // before the mouse pointer position.
	    // In Select mode replace the highlighted text with the clipboard.
	    ਜੇ (VIsual_ਸਰਗਰਮ)
	    {
		ਜੇ (VIsual_select)
		{
		    stuffcharReadbuff(Ctrl_G);
		    stuffReadbuff((char_u *)"\"+p");
		}
		ਨਹੀਂਤਾਂ
		{
		    stuffcharReadbuff('y');
		    stuffcharReadbuff(K_MIDDLEMOUSE);
		}
		do_always = TRUE;	// ignore 'mouse' setting next time
		ਵਾਪਸ FALSE;
	    }
	    // The rest is below jump_to_mouse()
	}

	ਨਹੀਂਤਾਂ ਜੇ ((State & ਬਹੁਲਕ_ਪਾਓ) == 0)
	    ਵਾਪਸ FALSE;

	// Middle click in insert mode doesn't move the mouse, just insert the
	// contents of a register.  '.' register is special, can't insert that
	// with do_put().
	// Also paste at the cursor if the current mode isn't in 'mouse' (only
	// happens for the GUI).
	ਜੇ ((State & ਬਹੁਲਕ_ਪਾਓ) || !mouse_has(MOUSE_ਸਾਧਾਰਨ))
	{
	    ਜੇ (regname == '.')
		ਪਾਓ_reg(regname, TRUE);
	    ਨਹੀਂਤਾਂ
	    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CLIPBOARD
		ਜੇ (clip_ਤਾਰਾ.ਉਪਲਬਧ && regname == 0)
		    regname = '*';
#ਜੇ_ਅੰਤ
		ਜੇ ((State & REPLACE_FLAG) && !yank_ਰਜਿਸਟਰ_mline(regname))
		    ਪਾਓ_reg(regname, TRUE);
		ਨਹੀਂਤਾਂ
		{
		    do_put(regname, ਨਿਰਾ, BACKWARD, 1L,
						      fixindent | PUT_CURSEND);

		    // Repeat it with CTRL-R CTRL-O r or CTRL-R CTRL-P r
		    Append_Char_ਮੰਜ਼ਿਲ_Redobuff(Ctrl_R);
		    Append_Char_ਮੰਜ਼ਿਲ_Redobuff(fixindent ? Ctrl_P : Ctrl_O);
		    Append_Char_ਮੰਜ਼ਿਲ_Redobuff(regname == 0 ? '"' : regname);
		}
	    }
	    ਵਾਪਸ FALSE;
	}
    }

    // When dragging or button-up stay in the same window.
    ਜੇ (!is_click)
	jump_flags |= MOUSE_FOCUS | MOUSE_DID_MOVE;

    ਸ਼ੁਰੂ_visual.lnum = 0;

    ਬਣਤਰ tabpage_label_info {
	bool is_panel;	    // label type. true: tabpanel, false: tab line
	bool just_ਵਿੱਚ;	    // just in tabpage label area
	bool just_click;    // just click tabpage label area
	ਪੂਰਨ nr;		    // tabpage number
    } tp_label = { false, false, false, 0 };

    // Check for clicking in the tab page panel.
#ਜੇ defined(FEAT_TABPANEL)
    ਜੇ (mouse_ਖਾਨਾ < firstwin->w_wincol
		|| mouse_ਖਾਨਾ >= firstwin->w_wincol + topframe->fr_ਚੌੜਾਈ)
    {
	// A click on the scrollbar column starts a drag interaction and
	// preempts tab-selection.
	ਜੇ (is_click && !is_drag && mouse_on_tabpanel_scrollbar())
	{
	    ਵਿੱਚ_tabpanel_scrollbar = TRUE;
	    tabpanel_drag_scrollbar(mouse_ਪੰਕਤੀ);
	    ਵਾਪਸ FALSE;
	}

	// Dispatch 'tabpanel' %[FuncName] click regions before falling through
	// to tab-page selection.  On drag events fall through to the normal
	// tab-drag handling.
	ਜੇ (is_click && !is_drag
		&& stl_click_handler_regions(tabpanel_stl_click,
					    tabpanel_stl_click_ਗਿਣਤੀ,
					    0, (char_u *)"tabpanel",
					    mouse_ਪੰਕਤੀ, mouse_ਖਾਨਾ,
					    which_button, mod_ਨਕਾਬ))
	    ਵਾਪਸ FALSE;

	tp_label.is_panel = true;
	tp_label.just_ਵਿੱਚ = true;
	tp_label.nr = get_tabpagenr_on_tabpanel();

	// click in a tab selects that tab page
	ਜੇ (is_click && cmdwin_type == 0)
	    tp_label.just_click = true;
    }
    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
    // Check for clicking in the tab page line.
    ਜੇ (Tab_ਪੰਨਾ_Idxs != ਨਿਰਾ && mouse_ਪੰਕਤੀ == 0 && firstwin->w_winrow > 0)
    {
	// Dispatch 'tabline' %[FuncName] click regions before falling through
	// to tab-page selection.  On drag events fall through to the normal
	// tab-drag handling.
	ਜੇ (is_click && !is_drag
		&& stl_click_handler_regions(tabline_stl_click,
					    tabline_stl_click_ਗਿਣਤੀ,
					    0, (char_u *)"tabline",
					    mouse_ਪੰਕਤੀ, mouse_ਖਾਨਾ,
					    which_button, mod_ਨਕਾਬ))
	    ਵਾਪਸ FALSE;

	tp_label.just_ਵਿੱਚ = true;
	tp_label.nr = Tab_ਪੰਨਾ_Idxs[mouse_ਖਾਨਾ];

	// click in a tab selects that tab page
	ਜੇ (is_click && cmdwin_type == 0
		&& mouse_ਖਾਨਾ < firstwin->w_wincol + topframe->fr_ਚੌੜਾਈ)
	    tp_label.just_click = true;
    }

    ਜੇ (tp_label.just_ਵਿੱਚ)
    {
	ਜੇ (is_drag)
	{
	    ਜੇ (ਵਿੱਚ_tabpanel || ਵਿੱਚ_tab_ਸਤਰ)
	    {
		c1 = tp_label.nr;
		tabpage_move(c1 <= 0 ? 9999 : c1 < tabpage_ਸੂਚਕਾਂਕ(curtab)
							    ? c1 - 1 : c1);
	    }
	    ਵਾਪਸ FALSE;
	}

	ਜੇ (tp_label.just_click)
	{
	    ਜੇ (tp_label.is_panel)
		ਵਿੱਚ_tabpanel = TRUE;
	    ਨਹੀਂਤਾਂ
		ਵਿੱਚ_tab_ਸਤਰ = TRUE;
	    c1 = tp_label.nr;
	    ਜੇ (c1 >= 0)
	    {
		ਜੇ ((mod_ਨਕਾਬ & MOD_ਨਕਾਬ_MULTI_CLICK) == MOD_ਨਕਾਬ_2_CLICK
						    && !tp_label.is_panel)
		{
		    // Double-click on the tabline opens a new, empty tab page.
		    // The tabpanel has no "empty area" (every row maps to a tab)
		    // and this behavior is not documented for tabpanel, so fall
		    // through to the regular tab-switch path there.
		    ਸਿਰਾ_visual_ਬਹੁਲਕ_keep_button();
		    tabpage_new();
		    tabpage_move(c1 == 0 ? 9999 : c1 - 1);
		}
		ਨਹੀਂਤਾਂ
		{
		    // Go to specified tab page, or next one if not clicking
		    // on a label.
		    goto_tabpage(c1);

		    // It's like clicking on the status line of a window.
		    ਜੇ (curwin != old_curwin)
			ਸਿਰਾ_visual_ਬਹੁਲਕ_keep_button();
		}
	    }
	    ਨਹੀਂਤਾਂ
	    {
		tabpage_T	*tp;

		// Close the current or specified tab page.
		ਜੇ (c1 == -999)
		    tp = curtab;
		ਨਹੀਂਤਾਂ
		    tp = ਲੱਭੋ_tabpage(-c1);
		ਜੇ (tp == curtab)
		{
		    ਜੇ (ਪਹਿਲਾ_tabpage->tp_ਅਗਲਾ != ਨਿਰਾ)
			tabpage_close(FALSE);
		}
		ਨਹੀਂਤਾਂ ਜੇ (tp != ਨਿਰਾ)
		    tabpage_close_other(tp, FALSE);
	    }
	}
	ਵਾਪਸ TRUE;
    }
    ਨਹੀਂਤਾਂ ਜੇ (is_drag && (ਵਿੱਚ_tabpanel || (ਵਿੱਚ_tab_ਸਤਰ && Tab_ਪੰਨਾ_Idxs != ਨਿਰਾ)))
    {
#ਜੇ defined(FEAT_TABPANEL)
	ਜੇ (ਵਿੱਚ_tabpanel)
	    c1 = get_tabpagenr_on_tabpanel();
	ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	    c1 = Tab_ਪੰਨਾ_Idxs[mouse_ਖਾਨਾ];
	tabpage_move(c1 <= 0 ? 9999 : c1 - 1);
	ਵਾਪਸ FALSE;
    }

    // When 'mousemodel' is "popup" or "popup_setpos", translate mouse events:
    // right button up   -> pop-up menu
    // shift-left button -> right button
    // alt-left button   -> alt-right button
    ਜੇ (mouse_model_popup())
    {
	ਜੇ (which_button == MOUSE_ਸੱਜਾ
			    && !(mod_ਨਕਾਬ & (MOD_ਨਕਾਬ_ਖਿਸਕਾਓ | MOD_ਨਕਾਬ_CTRL)))
	{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_POPUP_SETPOS
# ifdef FEAT_GUI
	    ਜੇ (gui.ਵਿੱਚ_use)
	    {
#  ਜੇ defined(FEAT_GUI_MOTIF) || defined(FEAT_GUI_GTK) \
			  || defined(FEAT_GUI_PHOTON)
		ਜੇ (!is_click)
		    // Ignore right button release events, only shows the popup
		    // menu on the button down event.
		    ਵਾਪਸ FALSE;
#  endif
#  ਜੇ defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_HAIKU)
		ਜੇ (is_click || is_drag)
		    // Ignore right button down and drag mouse events.  Windows
		    // only shows the popup menu on the button up event.
		    ਵਾਪਸ FALSE;
#  endif
	    }
# endif
# ਜੇ defined(FEAT_GUI) && defined(FEAT_ਪਦ_POPUP_MENU)
	    ਨਹੀਂਤਾਂ
# endif
# ਜੇ defined(FEAT_ਪਦ_POPUP_MENU)
	    ਜੇ (!is_click)
		// Ignore right button release events, only shows the popup
		// menu on the button down event.
		ਵਾਪਸ FALSE;
# endif

	    jump_flags = 0;
	    ਜੇ (STRCMP(p_mousem, "popup_setpos") == 0)
	    {
		// First set the cursor position before showing the popup
		// menu.
		ਜੇ (VIsual_ਸਰਗਰਮ)
		{
		    pos_T    m_pos;

		    // set MOUSE_MAY_STOP_VIS if we are outside the
		    // selection or the current window (might have false
		    // negative here)
		    ਜੇ (mouse_ਪੰਕਤੀ < curwin->w_winrow
			 || mouse_ਪੰਕਤੀ
				  > (curwin->w_winrow + curwin->w_ਉਚਾਈ))
			jump_flags = MOUSE_MAY_STOP_VIS;
		    ਨਹੀਂਤਾਂ ਜੇ (get_fpos_ਦਾ_mouse(&m_pos) != ਵਿੱਚ_ਬਫਰ)
			jump_flags = MOUSE_MAY_STOP_VIS;
		    ਨਹੀਂਤਾਂ
		    {
			ਜੇ (VIsual_ਬਹੁਲਕ == 'V')
			{
			    ਜੇ ((curwin->w_cursor.lnum <= VIsual.lnum
				    && (m_pos.lnum < curwin->w_cursor.lnum
					|| VIsual.lnum < m_pos.lnum))
				|| (VIsual.lnum < curwin->w_cursor.lnum
				    && (m_pos.lnum < VIsual.lnum
					|| curwin->w_cursor.lnum < m_pos.lnum)))
			    {
				jump_flags = MOUSE_MAY_STOP_VIS;
			    }
			}
			ਨਹੀਂਤਾਂ ਜੇ ((LTOREQ_POS(curwin->w_cursor, VIsual)
				    && (LT_POS(m_pos, curwin->w_cursor)
					|| LT_POS(VIsual, m_pos)))
				|| (LT_POS(VIsual, curwin->w_cursor)
				    && (LT_POS(m_pos, VIsual)
				      || LT_POS(curwin->w_cursor, m_pos))))
			{
			    jump_flags = MOUSE_MAY_STOP_VIS;
			}
			ਨਹੀਂਤਾਂ ਜੇ (VIsual_ਬਹੁਲਕ == Ctrl_V)
			{
			    getvcols(curwin, &curwin->w_cursor, &VIsual,
						     &leftcol, &rightcol, 0);
			    getvcol(curwin, &m_pos, ਨਿਰਾ, &m_pos.ਖਾਨਾ, ਨਿਰਾ, 0);
			    ਜੇ (m_pos.ਖਾਨਾ < leftcol || m_pos.ਖਾਨਾ > rightcol)
				jump_flags = MOUSE_MAY_STOP_VIS;
			}
		    }
		}
		ਨਹੀਂਤਾਂ
		    jump_flags = MOUSE_MAY_STOP_VIS;
	    }
	    ਜੇ (jump_flags)
	    {
		jump_flags = jump_ਮੰਜ਼ਿਲ_mouse(jump_flags, ਨਿਰਾ, which_button);
		update_curbuf(VIsual_ਸਰਗਰਮ ? UPD_INVERTED : UPD_VALID);
		setcursor();
		ਬਾਹਰਲਾ_flush();    // Update before showing popup menu
	    }
# ifdef FEAT_MENU
	    show_popupmenu();
	    got_click = FALSE;	// ignore release events
# endif
	    ਵਾਪਸ (jump_flags & CURSOR_MOVED) != 0;
#ਨਹੀਂਤਾਂ
	    ਵਾਪਸ FALSE;
#ਜੇ_ਅੰਤ
	}
	ਜੇ (which_button == MOUSE_ਖੱਬਾ
				&& (mod_ਨਕਾਬ & (MOD_ਨਕਾਬ_ਖਿਸਕਾਓ|MOD_ਨਕਾਬ_ALT)))
	{
	    which_button = MOUSE_ਸੱਜਾ;
	    mod_ਨਕਾਬ &= ~MOD_ਨਕਾਬ_ਖਿਸਕਾਓ;
	}
    }

    ਜੇ ((State & (ਬਹੁਲਕ_ਸਾਧਾਰਨ | ਬਹੁਲਕ_ਪਾਓ))
			    && !(mod_ਨਕਾਬ & (MOD_ਨਕਾਬ_ਖਿਸਕਾਓ | MOD_ਨਕਾਬ_CTRL)))
    {
	ਜੇ (which_button == MOUSE_ਖੱਬਾ)
	{
	    ਜੇ (is_click)
	    {
		// stop Visual mode for a left click in a window, but not when
		// on a status line
		ਜੇ (VIsual_ਸਰਗਰਮ)
		    jump_flags |= MOUSE_MAY_STOP_VIS;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (mouse_has(MOUSE_VISUAL))
		jump_flags |= MOUSE_MAY_VIS;
	}
	ਨਹੀਂਤਾਂ ਜੇ (which_button == MOUSE_ਸੱਜਾ)
	{
	    ਜੇ (is_click && VIsual_ਸਰਗਰਮ)
	    {
		// Remember the start and end of visual before moving the
		// cursor.
		ਜੇ (LT_POS(curwin->w_cursor, VIsual))
		{
		    ਸ਼ੁਰੂ_visual = curwin->w_cursor;
		    ਸਿਰਾ_visual = VIsual;
		}
		ਨਹੀਂਤਾਂ
		{
		    ਸ਼ੁਰੂ_visual = VIsual;
		    ਸਿਰਾ_visual = curwin->w_cursor;
		}
	    }
	    jump_flags |= MOUSE_FOCUS;
	    ਜੇ (mouse_has(MOUSE_VISUAL))
		jump_flags |= MOUSE_MAY_VIS;
	}
    }

    // If an operator is pending, ignore all drags and releases until the
    // next mouse click.
    ਜੇ (!is_drag && oap != ਨਿਰਾ && oap->op_type != OP_NOP)
    {
	got_click = FALSE;
	oap->motion_type = MCHAR;
    }

    // When releasing the button let jump_to_mouse() know.
    ਜੇ (!is_click && !is_drag)
	jump_flags |= MOUSE_RELEASED;

    // JUMP!
    jump_flags = jump_ਮੰਜ਼ਿਲ_mouse(jump_flags,
			oap == ਨਿਰਾ ? ਨਿਰਾ : &(oap->inclusive), which_button);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MENU
    // A click in the window toolbar has no side effects.
    ਜੇ (jump_flags & MOUSE_WINBAR)
	ਵਾਪਸ FALSE;
#ਜੇ_ਅੰਤ
    moved = (jump_flags & CURSOR_MOVED);
    ਵਿੱਚ_ਸਥਿਤੀ_ਸਤਰ_2 = (jump_flags & ਵਿੱਚ_ਸਥਿਤੀ_ਸਤਰ);
    ਵਿੱਚ_sep_ਸਤਰ = (jump_flags & ਵਿੱਚ_SEP_ਸਤਰ);

    // Check for statusline click handler early, before visual mode or
    // other button-specific handling can interfere.
    ਜੇ (ਵਿੱਚ_ਸਥਿਤੀ_ਸਤਰ_2 && is_click && !is_drag
	    && stl_click_handler(dragwin, mouse_ਪੰਕਤੀ, mouse_ਖਾਨਾ,
						which_button, mod_ਨਕਾਬ))
    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MOUSESHAPE
	ਜੇ (!drag_ਸਥਿਤੀ_ਸਤਰ)
	{
	    drag_ਸਥਿਤੀ_ਸਤਰ = TRUE;
	    update_mouseshape(-1);
	}
#ਜੇ_ਅੰਤ
	ਵਾਪਸ FALSE;
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_NETBEANS_INTG
    ਜੇ (is_Netbeans_ਬਫਰ(curbuf)
			    && !(jump_flags & (ਵਿੱਚ_ਸਥਿਤੀ_ਸਤਰ | ਵਿੱਚ_SEP_ਸਤਰ)))
    {
	ਪੂਰਨ ਕੁੰਜੀ = KEY2TERMCAP1(c);

	ਜੇ (ਕੁੰਜੀ == (ਪੂਰਨ)KE_LEFTRELEASE || ਕੁੰਜੀ == (ਪੂਰਨ)KE_MIDDLERELEASE
					       || ਕੁੰਜੀ == (ਪੂਰਨ)KE_RIGHTRELEASE)
	    netbeans_button_release(which_button);
    }
#ਜੇ_ਅੰਤ

    // When jumping to another window, clear a pending operator.  That's a bit
    // friendlier than beeping and not jumping to that window.
    ਜੇ (curwin != old_curwin && oap != ਨਿਰਾ && oap->op_type != OP_NOP)
	clearop(oap);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
    ਜੇ (mod_ਨਕਾਬ == 0
	    && !is_drag
	    && (jump_flags & (MOUSE_FOLD_CLOSE | MOUSE_FOLD_OPEN))
	    && which_button == MOUSE_ਖੱਬਾ)
    {
	// open or close a fold at this line
	ਜੇ (jump_flags & MOUSE_FOLD_OPEN)
	    openFold(curwin->w_cursor.lnum, 1L);
	ਨਹੀਂਤਾਂ
	    closeFold(curwin->w_cursor.lnum, 1L);
	// don't move the cursor if still in the same window
	ਜੇ (curwin == old_curwin)
	    curwin->w_cursor = save_cursor;
    }
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_CLIPBOARD)
    ਜੇ ((jump_flags & ਵਿੱਚ_OTHER_WIN) && !VIsual_ਸਰਗਰਮ &&
	    (clip_ਤਾਰਾ.ਉਪਲਬਧ || clip_plus.ਉਪਲਬਧ))
    {
	clip_modeless(which_button, is_click, is_drag);
	ਵਾਪਸ FALSE;
    }
#ਜੇ_ਅੰਤ

    // Set global flag that we are extending the Visual area with mouse
    // dragging; temporarily minimize 'scrolloff'.
    ਜੇ (VIsual_ਸਰਗਰਮ && is_drag && get_scrolloff_ਮੁੱਲ())
    {
	// In the very first line, allow scrolling one line
	ਜੇ (mouse_ਪੰਕਤੀ == 0)
	    mouse_dragging = 2;
	ਨਹੀਂਤਾਂ
	    mouse_dragging = 1;
    }

    // When dragging the mouse above the window, scroll down.
    ਜੇ (is_drag && mouse_ਪੰਕਤੀ < 0 && !ਵਿੱਚ_ਸਥਿਤੀ_ਸਤਰ_2)
    {
	scroll_redraw(FALSE, 1L);
	mouse_ਪੰਕਤੀ = 0;
    }

    ਜੇ (ਸ਼ੁਰੂ_visual.lnum)		// right click in visual mode
    {
       // When ALT is pressed make Visual mode blockwise.
       ਜੇ (mod_ਨਕਾਬ & MOD_ਨਕਾਬ_ALT)
	   VIsual_ਬਹੁਲਕ = Ctrl_V;

	// In Visual-block mode, divide the area in four, pick up the corner
	// that is in the quarter that the cursor is in.
	ਜੇ (VIsual_ਬਹੁਲਕ == Ctrl_V)
	{
	    getvcols(curwin, &ਸ਼ੁਰੂ_visual, &ਸਿਰਾ_visual,
						    &leftcol, &rightcol, 0);
	    ਜੇ (curwin->w_curswant > (leftcol + rightcol) / 2)
		ਸਿਰਾ_visual.ਖਾਨਾ = leftcol;
	    ਨਹੀਂਤਾਂ
		ਸਿਰਾ_visual.ਖਾਨਾ = rightcol;
	    ਜੇ (curwin->w_cursor.lnum >=
				    (ਸ਼ੁਰੂ_visual.lnum + ਸਿਰਾ_visual.lnum) / 2)
		ਸਿਰਾ_visual.lnum = ਸ਼ੁਰੂ_visual.lnum;

	    // move VIsual to the right column
	    ਸ਼ੁਰੂ_visual = curwin->w_cursor;	    // save the cursor pos
	    curwin->w_cursor = ਸਿਰਾ_visual;
	    coladvance(ਸਿਰਾ_visual.ਖਾਨਾ);
	    VIsual = curwin->w_cursor;
	    curwin->w_cursor = ਸ਼ੁਰੂ_visual;	    // restore the cursor
	}
	ਨਹੀਂਤਾਂ
	{
	    // If the click is before the start of visual, change the start.
	    // If the click is after the end of visual, change the end.  If
	    // the click is inside the visual, change the closest side.
	    ਜੇ (LT_POS(curwin->w_cursor, ਸ਼ੁਰੂ_visual))
		VIsual = ਸਿਰਾ_visual;
	    ਨਹੀਂਤਾਂ ਜੇ (LT_POS(ਸਿਰਾ_visual, curwin->w_cursor))
		VIsual = ਸ਼ੁਰੂ_visual;
	    ਨਹੀਂਤਾਂ
	    {
		// In the same line, compare column number
		ਜੇ (ਸਿਰਾ_visual.lnum == ਸ਼ੁਰੂ_visual.lnum)
		{
		    ਜੇ (curwin->w_cursor.ਖਾਨਾ - ਸ਼ੁਰੂ_visual.ਖਾਨਾ >
				    ਸਿਰਾ_visual.ਖਾਨਾ - curwin->w_cursor.ਖਾਨਾ)
			VIsual = ਸ਼ੁਰੂ_visual;
		    ਨਹੀਂਤਾਂ
			VIsual = ਸਿਰਾ_visual;
		}

		// In different lines, compare line number
		ਨਹੀਂਤਾਂ
		{
		    diff = (curwin->w_cursor.lnum - ਸ਼ੁਰੂ_visual.lnum) -
				(ਸਿਰਾ_visual.lnum - curwin->w_cursor.lnum);

		    ਜੇ (diff > 0)		// closest to end
			VIsual = ਸ਼ੁਰੂ_visual;
		    ਨਹੀਂਤਾਂ ਜੇ (diff < 0)	// closest to start
			VIsual = ਸਿਰਾ_visual;
		    ਨਹੀਂਤਾਂ			// in the middle line
		    {
			ਜੇ (curwin->w_cursor.ਖਾਨਾ <
					(ਸ਼ੁਰੂ_visual.ਖਾਨਾ + ਸਿਰਾ_visual.ਖਾਨਾ) / 2)
			    VIsual = ਸਿਰਾ_visual;
			ਨਹੀਂਤਾਂ
			    VIsual = ਸ਼ੁਰੂ_visual;
		    }
		}
	    }
	}
    }
    // If Visual mode started in insert mode, execute "CTRL-O"
    ਨਹੀਂਤਾਂ ਜੇ ((State & ਬਹੁਲਕ_ਪਾਓ) && VIsual_ਸਰਗਰਮ)
	stuffcharReadbuff(Ctrl_O);

    // Middle mouse click: Put text before cursor.
    ਜੇ (which_button == MOUSE_ਵਿਚਕਾਰ)
    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CLIPBOARD
	ਜੇ (clip_ਤਾਰਾ.ਉਪਲਬਧ && regname == 0)
	    regname = '*';
#ਜੇ_ਅੰਤ
	ਜੇ (yank_ਰਜਿਸਟਰ_mline(regname))
	{
	    ਜੇ (mouse_past_bottom)
		dir = FORWARD;
	}
	ਨਹੀਂਤਾਂ ਜੇ (mouse_past_eol)
	    dir = FORWARD;

	ਜੇ (fixindent)
	{
	    c1 = (dir == BACKWARD) ? '[' : ']';
	    c2 = 'p';
	}
	ਨਹੀਂਤਾਂ
	{
	    c1 = (dir == FORWARD) ? 'p' : 'P';
	    c2 = NUL;
	}
	prep_redo(regname, ਗਿਣਤੀ_2, NUL, c1, NUL, c2, NUL);

	// Remember where the paste started, so in edit() Insstart can be set
	// to this position
	ਜੇ (restart_edit != 0)
	    where_paste_started = curwin->w_cursor;
	do_put(regname, ਨਿਰਾ, dir, ਗਿਣਤੀ_2, fixindent | PUT_CURSEND);
    }

#ਜੇ defined(FEAT_QUICKFIX)
    // Ctrl-Mouse click or double click in a quickfix window jumps to the
    // error under the mouse pointer.
    ਨਹੀਂਤਾਂ ਜੇ (((mod_ਨਕਾਬ & MOD_ਨਕਾਬ_CTRL)
		|| (mod_ਨਕਾਬ & MOD_ਨਕਾਬ_MULTI_CLICK) == MOD_ਨਕਾਬ_2_CLICK)
	    && bt_quickfix(curbuf))
    {
	ਜੇ (curwin->w_llist_ref == ਨਿਰਾ)	// quickfix window
	    do_cmdline_cmd((char_u *)".cc");
	ਨਹੀਂਤਾਂ					// location list window
	    do_cmdline_cmd((char_u *)".ll");
	got_click = FALSE;		// ignore drag&release now
    }
#ਜੇ_ਅੰਤ

    // Ctrl-Mouse click (or double click in a help window) jumps to the tag
    // under the mouse pointer.
    ਨਹੀਂਤਾਂ ਜੇ ((mod_ਨਕਾਬ & MOD_ਨਕਾਬ_CTRL) || (curbuf->b_help
		     && (mod_ਨਕਾਬ & MOD_ਨਕਾਬ_MULTI_CLICK) == MOD_ਨਕਾਬ_2_CLICK))
    {
	ਜੇ (State & ਬਹੁਲਕ_ਪਾਓ)
	    stuffcharReadbuff(Ctrl_O);
	stuffcharReadbuff(Ctrl_RSB);
	got_click = FALSE;		// ignore drag&release now
    }

    // Shift-Mouse click searches for the next occurrence of the word under
    // the mouse pointer
    ਨਹੀਂਤਾਂ ਜੇ ((mod_ਨਕਾਬ & MOD_ਨਕਾਬ_ਖਿਸਕਾਓ))
    {
	ਜੇ ((State & ਬਹੁਲਕ_ਪਾਓ) || (VIsual_ਸਰਗਰਮ && VIsual_select))
	    stuffcharReadbuff(Ctrl_O);
	ਜੇ (which_button == MOUSE_ਖੱਬਾ)
	    stuffcharReadbuff('*');
	ਨਹੀਂਤਾਂ	// MOUSE_RIGHT
	    stuffcharReadbuff('#');
    }

    // Handle double clicks, unless on status line
    ਨਹੀਂਤਾਂ ਜੇ (ਵਿੱਚ_ਸਥਿਤੀ_ਸਤਰ_2)
    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MOUSESHAPE
	ਜੇ ((is_drag || is_click) && !drag_ਸਥਿਤੀ_ਸਤਰ)
	{
	    drag_ਸਥਿਤੀ_ਸਤਰ = TRUE;
	    update_mouseshape(-1);
	}
#ਜੇ_ਅੰਤ
    }
    ਨਹੀਂਤਾਂ ਜੇ (ਵਿੱਚ_sep_ਸਤਰ)
    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MOUSESHAPE
	ਜੇ ((is_drag || is_click) && !drag_sep_ਸਤਰ)
	{
	    drag_sep_ਸਤਰ = TRUE;
	    update_mouseshape(-1);
	}
#ਜੇ_ਅੰਤ
    }
    ਨਹੀਂਤਾਂ ਜੇ ((mod_ਨਕਾਬ & MOD_ਨਕਾਬ_MULTI_CLICK)
				       && (State & (ਬਹੁਲਕ_ਸਾਧਾਰਨ | ਬਹੁਲਕ_ਪਾਓ))
	     && mouse_has(MOUSE_VISUAL))
    {
	ਜੇ (is_click || !VIsual_ਸਰਗਰਮ)
	{
	    ਜੇ (VIsual_ਸਰਗਰਮ)
		orig_cursor = VIsual;
	    ਨਹੀਂਤਾਂ
	    {
		check_visual_highlight();
		VIsual = curwin->w_cursor;
		orig_cursor = VIsual;
		VIsual_ਸਰਗਰਮ = TRUE;
		VIsual_reselect = TRUE;
		// start Select mode if 'selectmode' contains "mouse"
		may_ਸ਼ੁਰੂ_select('o');
		setmouse();
	    }
	    ਜੇ ((mod_ਨਕਾਬ & MOD_ਨਕਾਬ_MULTI_CLICK) == MOD_ਨਕਾਬ_2_CLICK)
	    {
		// Double click with ALT pressed makes it blockwise.
		ਜੇ (mod_ਨਕਾਬ & MOD_ਨਕਾਬ_ALT)
		    VIsual_ਬਹੁਲਕ = Ctrl_V;
		ਨਹੀਂਤਾਂ
		    VIsual_ਬਹੁਲਕ = 'v';
	    }
	    ਨਹੀਂਤਾਂ ਜੇ ((mod_ਨਕਾਬ & MOD_ਨਕਾਬ_MULTI_CLICK) == MOD_ਨਕਾਬ_3_CLICK)
		VIsual_ਬਹੁਲਕ = 'V';
	    ਨਹੀਂਤਾਂ ਜੇ ((mod_ਨਕਾਬ & MOD_ਨਕਾਬ_MULTI_CLICK) == MOD_ਨਕਾਬ_4_CLICK)
		VIsual_ਬਹੁਲਕ = Ctrl_V;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CLIPBOARD
	    // Make sure the clipboard gets updated.  Needed because start and
	    // end may still be the same, and the selection needs to be owned
	    clip_ਤਾਰਾ.vmode = NUL;
#ਜੇ_ਅੰਤ
	}
	// A double click selects a word or a block.
	ਜੇ ((mod_ਨਕਾਬ & MOD_ਨਕਾਬ_MULTI_CLICK) == MOD_ਨਕਾਬ_2_CLICK)
	{
	    pos_T	*pos = ਨਿਰਾ;
	    ਪੂਰਨ		gc;

	    ਜੇ (is_click)
	    {
		// If the character under the cursor (skipping white space) is
		// not a word character, try finding a match and select a (),
		// {}, [], #if/#endif, etc. block.
		ਸਿਰਾ_visual = curwin->w_cursor;
		ਜਦੋਂ (gc = gchar_pos(&ਸਿਰਾ_visual), VIM_ISWHITE(gc))
		    inc(&ਸਿਰਾ_visual);
		ਜੇ (oap != ਨਿਰਾ)
		    oap->motion_type = MCHAR;
		ਜੇ (oap != ਨਿਰਾ
			&& VIsual_ਬਹੁਲਕ == 'v'
			&& !vim_iswordc(gchar_pos(&ਸਿਰਾ_visual))
			&& EQUAL_POS(curwin->w_cursor, VIsual)
			&& (pos = findmatch(oap, NUL)) != ਨਿਰਾ)
		{
		    curwin->w_cursor = *pos;
		    ਜੇ (oap->motion_type == MLINE)
			VIsual_ਬਹੁਲਕ = 'V';
		    ਨਹੀਂਤਾਂ ਜੇ (*p_sel == 'e')
		    {
			ਜੇ (LT_POS(curwin->w_cursor, VIsual))
			    ++VIsual.ਖਾਨਾ;
			ਨਹੀਂਤਾਂ
			    ++curwin->w_cursor.ਖਾਨਾ;
		    }
		}
	    }

	    ਜੇ (pos == ਨਿਰਾ && (is_click || is_drag))
	    {
		// When not found a match or when dragging: extend to include
		// a word.
		ਜੇ (LT_POS(curwin->w_cursor, orig_cursor))
		{
		    ਲੱਭੋ_ਸ਼ੁਰੂ_ਦਾ_ਸ਼ਬਦ(&curwin->w_cursor);
		    ਲੱਭੋ_ਸਿਰਾ_ਦਾ_ਸ਼ਬਦ(&VIsual);
		}
		ਨਹੀਂਤਾਂ
		{
		    ਲੱਭੋ_ਸ਼ੁਰੂ_ਦਾ_ਸ਼ਬਦ(&VIsual);
		    ਜੇ (*p_sel == 'e' && *ml_get_cursor() != NUL)
			curwin->w_cursor.ਖਾਨਾ +=
					 (*mb_ptr2len)(ml_get_cursor());
		    ਲੱਭੋ_ਸਿਰਾ_ਦਾ_ਸ਼ਬਦ(&curwin->w_cursor);
		}
	    }
	    curwin->w_ਸੈੱਟ_curswant = true;
	}
	ਜੇ (is_click)
	    redraw_curbuf_later(UPD_INVERTED);	// update the inversion
    }
    ਨਹੀਂਤਾਂ ਜੇ (VIsual_ਸਰਗਰਮ && !old_ਸਰਗਰਮ)
    {
	ਜੇ (mod_ਨਕਾਬ & MOD_ਨਕਾਬ_ALT)
	    VIsual_ਬਹੁਲਕ = Ctrl_V;
	ਨਹੀਂਤਾਂ
	    VIsual_ਬਹੁਲਕ = 'v';
    }

    // If Visual mode changed show it later.
    ਜੇ ((!VIsual_ਸਰਗਰਮ && old_ਸਰਗਰਮ && ਬਹੁਲਕ_displayed)
	    || (VIsual_ਸਰਗਰਮ && p_smd && ਸੁਨੇਹਾ_silent == 0
				 && (!old_ਸਰਗਰਮ || VIsual_ਬਹੁਲਕ != old_ਬਹੁਲਕ)))
	redraw_cmdline = TRUE;

    ਵਾਪਸ moved;
}

    ਖਾਲੀ
ins_mouse(ਪੂਰਨ c)
{
    pos_T	tpos;
    win_T	*old_curwin = curwin;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
    // When GUI is active, also move/paste when 'mouse' is empty
    ਜੇ (!gui.ਵਿੱਚ_use)
#ਜੇ_ਅੰਤ
	ਜੇ (!mouse_has(MOUSE_ਪਾਓ))
	    ਵਾਪਸ;

    undisplay_dollar();
    tpos = curwin->w_cursor;
    ਜੇ (do_mouse(ਨਿਰਾ, c, BACKWARD, 1L, 0))
    {
	win_T	*new_curwin = curwin;

	ਜੇ (curwin != old_curwin && win_valid(old_curwin))
	{
	    // Mouse took us to another window.  We need to go back to the
	    // previous one to stop insert there properly.
	    curwin = old_curwin;
	    curbuf = curwin->w_ਬਫਰ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਕੰਮ_CHANNEL
	    ਜੇ (bt_prompt(curbuf))
		// Restart Insert mode when re-entering the prompt buffer.
		curbuf->b_prompt_ਪਾਓ = 'A';
#ਜੇ_ਅੰਤ
	}
	ਸ਼ੁਰੂ_arrow(curwin == old_curwin ? &tpos : ਨਿਰਾ);
	ਜੇ (curwin != new_curwin && win_valid(new_curwin))
	{
	    curwin = new_curwin;
	    curbuf = curwin->w_ਬਫਰ;
	}
	ਸੈੱਟ_can_cindent(TRUE);
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
    ਸਥਿਰ ਖਾਲੀ
do_mousescroll(cmdarg_T *cap)
{
    ਪੂਰਨ ਖਿਸਕਾਓ_or_ctrl = mod_ਨਕਾਬ & (MOD_ਨਕਾਬ_ਖਿਸਕਾਓ | MOD_ਨਕਾਬ_CTRL);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMINAL
    ਜੇ (ਪਦ_use_loop())
	// This window is a terminal window, send the mouse event there.
	// Set "typed" to FALSE to avoid an endless loop.
	send_ਕੁੰਜੀਆਂ_ਮੰਜ਼ਿਲ_ਪਦ(curbuf->b_ਪਦ, cap->cmdchar, mod_ਨਕਾਬ, FALSE);
    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
    ਜੇ (cap->arg == MSCR_ਉੱਪਰ || cap->arg == MSCR_ਥੱਲੇ)
    {
	// Vertical scrolling
	ਜੇ (!(State & ਬਹੁਲਕ_ਪਾਓ) && (mouse_vert_ਕਦਮ < 0 || ਖਿਸਕਾਓ_or_ctrl))
	{
	    // whole page up or down
	    pagescroll(cap->arg == MSCR_ਉੱਪਰ ? FORWARD : BACKWARD, 1L, FALSE);
	}
	ਨਹੀਂਤਾਂ
	{
	    ਜੇ (mouse_vert_ਕਦਮ < 0 || ਖਿਸਕਾਓ_or_ctrl)
	    {
		// whole page up or down
		cap->count1 = (ਲੰਮਾ)(curwin->w_botline - curwin->w_topline);
	    }
	    // Don't scroll more than half the window height.
	    ਨਹੀਂਤਾਂ ਜੇ (curwin->w_ਉਚਾਈ < mouse_vert_ਕਦਮ * 2)
	    {
		cap->count1 = curwin->w_ਉਚਾਈ / 2;
		ਜੇ (cap->count1 == 0)
		    cap->count1 = 1;
	    }
	    ਨਹੀਂਤਾਂ
	    {
		cap->count1 = mouse_vert_ਕਦਮ;
	    }
	    cap->count0 = cap->count1;
	    nv_scroll_ਸਤਰ(cap);
	}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
	ਜੇ (WIN_IS_POPUP(curwin))
	    popup_ਸੈੱਟ_firstline(curwin);
#ਜੇ_ਅੰਤ
    }
    ਨਹੀਂਤਾਂ
    {
	// Horizontal scrolling
	ਲੰਮਾ ਕਦਮ = (mouse_hor_ਕਦਮ < 0 || ਖਿਸਕਾਓ_or_ctrl)
					    ? curwin->w_ਚੌੜਾਈ : mouse_hor_ਕਦਮ;
	ਲੰਮਾ leftcol = curwin->w_leftcol
				     + (cap->arg == MSCR_ਸੱਜਾ ? -ਕਦਮ : ਕਦਮ);
	ਜੇ (leftcol < 0)
	    leftcol = 0;
	do_mousescroll_horiz((long_u)leftcol);
    }
    may_trigger_win_scrolled_resized();
}

/*
 * Insert mode implementation for scrolling in direction "dir", which is
 * one of the MSCR_ values.
 */
    ਖਾਲੀ
ins_mousescroll(ਪੂਰਨ dir)
{
    cmdarg_T	cap;
    oparg_T	oa;
    CLEAR_ਖੇਤਰ(cap);
    clear_oparg(&oa);
    cap.oap = &oa;
    cap.arg = dir;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TABPANEL
    ਜੇ (mouse_ਪੰਕਤੀ >= 0 && mouse_ਖਾਨਾ >= 0
	    && (dir == MSCR_ਉੱਪਰ || dir == MSCR_ਥੱਲੇ)
	    && mouse_on_tabpanel())
    {
	(ਖਾਲੀ)tabpanel_scroll(dir == MSCR_ਉੱਪਰ ? 1 : -1,
		mouse_vert_ਕਦਮ > 0 ? mouse_vert_ਕਦਮ : 3);
	ਵਾਪਸ;
    }
#ਜੇ_ਅੰਤ

    ਚੋਣ (dir)
    {
	ਹਾਲਤ MSCR_ਉੱਪਰ:
	    cap.cmdchar = K_MOUSEUP;
	    ਰੋਕੋ;
	ਹਾਲਤ MSCR_ਥੱਲੇ:
	    cap.cmdchar = K_MOUSEDOWN;
	    ਰੋਕੋ;
	ਹਾਲਤ MSCR_ਖੱਬਾ:
	    cap.cmdchar = K_MOUSELEFT;
	    ਰੋਕੋ;
	ਹਾਲਤ MSCR_ਸੱਜਾ:
	    cap.cmdchar = K_MOUSERIGHT;
	    ਰੋਕੋ;
	ਮੂਲ:
	    siemsg("Invalid ins_mousescroll() argument: %d", dir);
    }

    win_T *old_curwin = curwin;
    ਜੇ (mouse_ਪੰਕਤੀ >= 0 && mouse_ਖਾਨਾ >= 0)
    {
	// Find the window at the mouse pointer coordinates.
	// NOTE: Must restore "curwin" to "old_curwin" before returning!
	ਪੂਰਨ ਪੰਕਤੀ = mouse_ਪੰਕਤੀ;
	ਪੂਰਨ ਖਾਨਾ = mouse_ਖਾਨਾ;
	curwin = mouse_ਲੱਭੋ_win(&ਪੰਕਤੀ, &ਖਾਨਾ, ਲੱਭੋ_POPUP);
	ਜੇ (curwin == ਨਿਰਾ)
	{
	    curwin = old_curwin;
	    ਵਾਪਸ;
	}
	curbuf = curwin->w_ਬਫਰ;
    }

    ਜੇ (curwin == old_curwin)
    {
	// Don't scroll the current window if the popup menu is visible.
	ਜੇ (pum_visible())
	    ਵਾਪਸ;

	undisplay_dollar();
    }

    linenr_T	orig_topline = curwin->w_topline;
    colnr_T	orig_leftcol = curwin->w_leftcol;
    pos_T	orig_cursor = curwin->w_cursor;

    // Call the common mouse scroll function shared with other modes.
    do_mousescroll(&cap);

    ਪੂਰਨ did_scroll = (orig_topline != curwin->w_topline
		   || orig_leftcol != curwin->w_leftcol);

    curwin->w_redr_ਸਥਿਤੀ = true;
    curwin = old_curwin;
    curbuf = curwin->w_ਬਫਰ;

    // If the window actually scrolled and the popup menu may overlay the
    // window, need to redraw it.
    ਜੇ (did_scroll && pum_visible())
    {
	// TODO: Would be more efficient to only redraw the windows that are
	// overlapped by the popup menu.
	redraw_all_later(UPD_NOT_VALID);
	ins_compl_show_pum();
    }

    ਜੇ (!EQUAL_POS(curwin->w_cursor, orig_cursor))
    {
	ਸ਼ੁਰੂ_arrow(&orig_cursor);
	ਸੈੱਟ_can_cindent(TRUE);
    }
}

#ਜੇ defined(FEAT_PROP_POPUP)
/*
 * Command-line mode implementation for scrolling in direction "dir", which is
 * one of the MSCR_ values.  Scrolls the completion info popup when the mouse
 * pointer is on top of it.
 * Returns TRUE when the info popup was scrolled.
 */
    ਪੂਰਨ
cmdline_mousescroll(ਪੂਰਨ dir)
{
    cmdarg_T	cap;
    oparg_T	oa;

    CLEAR_ਖੇਤਰ(cap);
    clear_oparg(&oa);
    cap.oap = &oa;
    cap.arg = dir;

    ਚੋਣ (dir)
    {
	ਹਾਲਤ MSCR_ਉੱਪਰ:	 cap.cmdchar = K_MOUSEUP; ਰੋਕੋ;
	ਹਾਲਤ MSCR_ਥੱਲੇ:	 cap.cmdchar = K_MOUSEDOWN; ਰੋਕੋ;
	ਹਾਲਤ MSCR_ਖੱਬਾ:	 cap.cmdchar = K_MOUSELEFT; ਰੋਕੋ;
	ਹਾਲਤ MSCR_ਸੱਜਾ: cap.cmdchar = K_MOUSERIGHT; ਰੋਕੋ;
    }

    ਜੇ (mouse_ਪੰਕਤੀ < 0 || mouse_ਖਾਨਾ < 0)
	ਵਾਪਸ FALSE;

    ਪੂਰਨ	    ਪੰਕਤੀ = mouse_ਪੰਕਤੀ;
    ਪੂਰਨ	    ਖਾਨਾ = mouse_ਖਾਨਾ;
    win_T   *wp;

    // Only scroll when the mouse is on top of the info popup.
    wp = mouse_ਲੱਭੋ_win(&ਪੰਕਤੀ, &ਖਾਨਾ, ਲੱਭੋ_POPUP);
    ਜੇ (wp == ਨਿਰਾ || !WIN_IS_POPUP(wp) || !(wp->w_popup_flags & POPF_INFO)
							|| !wp->w_has_scrollbar)
	ਵਾਪਸ FALSE;

    win_T   *old_curwin = curwin;

    curwin = wp;
    curbuf = wp->w_ਬਫਰ;
    // Call the common mouse scroll function shared with other modes.
    do_mousescroll(&cap);
    curwin = old_curwin;
    curbuf = curwin->w_ਬਫਰ;

    // Cmdline mode doesn't normally call update_screen(), so redraw the
    // completion popup menu, which also repaints the info popup.
    ਜੇ (cmdline_pum_ਸਰਗਰਮ())
	cmdline_pum_display();
    ਵਾਪਸ TRUE;
}
#ਜੇ_ਅੰਤ

/*
 * Return TRUE if "c" is a mouse key.
 */
    ਪੂਰਨ
is_mouse_ਕੁੰਜੀ(ਪੂਰਨ c)
{
    ਵਾਪਸ c == K_LEFTMOUSE
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

ਸਥਿਰ ਬਣਤਰ mousetable
{
    ਪੂਰਨ	    pseudo_ਕੋਡ;	// Code for pseudo mouse event
    ਪੂਰਨ	    button;		// Which mouse button is it?
    ਪੂਰਨ	    is_click;		// Is it a mouse button click event?
    ਪੂਰਨ	    is_drag;		// Is it a mouse drag event?
} mouse_ਸਾਰਣੀ[] =
{
    {(ਪੂਰਨ)KE_LEFTMOUSE,		MOUSE_ਖੱਬਾ,	TRUE,	FALSE},
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
    {(ਪੂਰਨ)KE_LEFTMOUSE_NM,	MOUSE_ਖੱਬਾ,	TRUE,	FALSE},
#ਜੇ_ਅੰਤ
    {(ਪੂਰਨ)KE_LEFTDRAG,		MOUSE_ਖੱਬਾ,	FALSE,	TRUE},
    {(ਪੂਰਨ)KE_LEFTRELEASE,	MOUSE_ਖੱਬਾ,	FALSE,	FALSE},
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
    {(ਪੂਰਨ)KE_LEFTRELEASE_NM,	MOUSE_ਖੱਬਾ,	FALSE,	FALSE},
#ਜੇ_ਅੰਤ
    {(ਪੂਰਨ)KE_MIDDLEMOUSE,	MOUSE_ਵਿਚਕਾਰ,	TRUE,	FALSE},
    {(ਪੂਰਨ)KE_MIDDLEDRAG,	MOUSE_ਵਿਚਕਾਰ,	FALSE,	TRUE},
    {(ਪੂਰਨ)KE_MIDDLERELEASE,	MOUSE_ਵਿਚਕਾਰ,	FALSE,	FALSE},
    {(ਪੂਰਨ)KE_RIGHTMOUSE,	MOUSE_ਸੱਜਾ,	TRUE,	FALSE},
    {(ਪੂਰਨ)KE_RIGHTDRAG,		MOUSE_ਸੱਜਾ,	FALSE,	TRUE},
    {(ਪੂਰਨ)KE_RIGHTRELEASE,	MOUSE_ਸੱਜਾ,	FALSE,	FALSE},
    {(ਪੂਰਨ)KE_X1MOUSE,		MOUSE_X1,	TRUE,	FALSE},
    {(ਪੂਰਨ)KE_X1DRAG,		MOUSE_X1,	FALSE,	TRUE},
    {(ਪੂਰਨ)KE_X1RELEASE,		MOUSE_X1,	FALSE,	FALSE},
    {(ਪੂਰਨ)KE_X2MOUSE,		MOUSE_X2,	TRUE,	FALSE},
    {(ਪੂਰਨ)KE_X2DRAG,		MOUSE_X2,	FALSE,	TRUE},
    {(ਪੂਰਨ)KE_X2RELEASE,		MOUSE_X2,	FALSE,	FALSE},
    // DRAG without CLICK
    {(ਪੂਰਨ)KE_MOUSEMOVE,		MOUSE_RELEASE,	FALSE,	TRUE},
    // RELEASE without CLICK
    {(ਪੂਰਨ)KE_IGNORE,		MOUSE_RELEASE,	FALSE,	FALSE},
    {0,				0,		0,	0},
};

/*
 * Look up the given mouse code to return the relevant information in the other
 * arguments.  Return which button is down or was released.
 */
    ਪੂਰਨ
get_mouse_button(ਪੂਰਨ ਕੋਡ, ਪੂਰਨ *is_click, ਪੂਰਨ *is_drag)
{
    ਪੂਰਨ	    i;

    ਲਈ (i = 0; mouse_ਸਾਰਣੀ[i].pseudo_ਕੋਡ; i++)
	ਜੇ (ਕੋਡ == mouse_ਸਾਰਣੀ[i].pseudo_ਕੋਡ)
	{
	    *is_click = mouse_ਸਾਰਣੀ[i].is_click;
	    *is_drag = mouse_ਸਾਰਣੀ[i].is_drag;
	    ਵਾਪਸ mouse_ਸਾਰਣੀ[i].button;
	}
    ਵਾਪਸ 0;	    // Shouldn't get here
}

/*
 * Return the appropriate pseudo mouse event token (KE_LEFTMOUSE etc) based on
 * the given information about which mouse button is down, and whether the
 * mouse was clicked, dragged or released.
 */
    ਪੂਰਨ
get_pseudo_mouse_ਕੋਡ(
    ਪੂਰਨ	    button,	// eg MOUSE_LEFT
    ਪੂਰਨ	    is_click,
    ਪੂਰਨ	    is_drag)
{
    ਪੂਰਨ	    i;

    ਲਈ (i = 0; mouse_ਸਾਰਣੀ[i].pseudo_ਕੋਡ; i++)
	ਜੇ (button == mouse_ਸਾਰਣੀ[i].button
	    && is_click == mouse_ਸਾਰਣੀ[i].is_click
	    && is_drag == mouse_ਸਾਰਣੀ[i].is_drag)
	{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
	    // Trick: a non mappable left click and release has mouse_col -1
	    // or added MOUSE_COLOFF.  Used for 'mousefocus' in
	    // gui_mouse_moved()
	    ਜੇ (mouse_ਖਾਨਾ < 0 || mouse_ਖਾਨਾ > MOUSE_COLOFF)
	    {
		ਜੇ (mouse_ਖਾਨਾ < 0)
		    mouse_ਖਾਨਾ = 0;
		ਨਹੀਂਤਾਂ
		    mouse_ਖਾਨਾ -= MOUSE_COLOFF;
		ਜੇ (mouse_ਸਾਰਣੀ[i].pseudo_ਕੋਡ == (ਪੂਰਨ)KE_LEFTMOUSE)
		    ਵਾਪਸ (ਪੂਰਨ)KE_LEFTMOUSE_NM;
		ਜੇ (mouse_ਸਾਰਣੀ[i].pseudo_ਕੋਡ == (ਪੂਰਨ)KE_LEFTRELEASE)
		    ਵਾਪਸ (ਪੂਰਨ)KE_LEFTRELEASE_NM;
	    }
#ਜੇ_ਅੰਤ
	    ਵਾਪਸ mouse_ਸਾਰਣੀ[i].pseudo_ਕੋਡ;
	}
    ਵਾਪਸ (ਪੂਰਨ)KE_IGNORE;	    // not recognized, ignore it
}

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HMT_ਸਾਧਾਰਨ	1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HMT_NETTERM	2
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HMT_DEC	4
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HMT_JSBTERM	8
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HMT_PTERM	16
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HMT_URXVT	32
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HMT_GPM	64
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HMT_SGR	128
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HMT_SGR_REL	256
ਸਥਿਰ ਪੂਰਨ has_mouse_termcode = 0;

    ਖਾਲੀ
ਸੈੱਟ_mouse_termcode(
    ਪੂਰਨ		n,	// KS_MOUSE, KS_NETTERM_MOUSE or KS_DEC_MOUSE
    char_u	*s)
{
    char_u	name[2];

    name[0] = n;
    name[1] = KE_FILLER;
    ਜੋੜੋ_termcode(name, s, FALSE);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MOUSE_JSB
    ਜੇ (n == KS_JSBTERM_MOUSE)
	has_mouse_termcode |= HMT_JSBTERM;
    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MOUSE_NET
    ਜੇ (n == KS_NETTERM_MOUSE)
	has_mouse_termcode |= HMT_NETTERM;
    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MOUSE_DEC
    ਜੇ (n == KS_DEC_MOUSE)
	has_mouse_termcode |= HMT_DEC;
    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MOUSE_PTERM
    ਜੇ (n == KS_PTERM_MOUSE)
	has_mouse_termcode |= HMT_PTERM;
    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MOUSE_URXVT
    ਜੇ (n == KS_URXVT_MOUSE)
	has_mouse_termcode |= HMT_URXVT;
    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MOUSE_GPM
    ਜੇ (n == KS_GPM_MOUSE)
	has_mouse_termcode |= HMT_GPM;
    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
    ਜੇ (n == KS_SGR_MOUSE)
	has_mouse_termcode |= HMT_SGR;
    ਨਹੀਂਤਾਂ ਜੇ (n == KS_SGR_MOUSE_RELEASE)
	has_mouse_termcode |= HMT_SGR_REL;
    ਨਹੀਂਤਾਂ
	has_mouse_termcode |= HMT_ਸਾਧਾਰਨ;
}

#ਜੇ defined(UNIX) || defined(VMS)
    ਖਾਲੀ
del_mouse_termcode(
    ਪੂਰਨ		n)	// KS_MOUSE, KS_NETTERM_MOUSE or KS_DEC_MOUSE
{
    char_u	name[2];

    name[0] = n;
    name[1] = KE_FILLER;
    del_termcode(name);
# ifdef FEAT_MOUSE_JSB
    ਜੇ (n == KS_JSBTERM_MOUSE)
	has_mouse_termcode &= ~HMT_JSBTERM;
    ਨਹੀਂਤਾਂ
# endif
# ifdef FEAT_MOUSE_NET
    ਜੇ (n == KS_NETTERM_MOUSE)
	has_mouse_termcode &= ~HMT_NETTERM;
    ਨਹੀਂਤਾਂ
# endif
# ifdef FEAT_MOUSE_DEC
    ਜੇ (n == KS_DEC_MOUSE)
	has_mouse_termcode &= ~HMT_DEC;
    ਨਹੀਂਤਾਂ
# endif
# ifdef FEAT_MOUSE_PTERM
    ਜੇ (n == KS_PTERM_MOUSE)
	has_mouse_termcode &= ~HMT_PTERM;
    ਨਹੀਂਤਾਂ
# endif
# ifdef FEAT_MOUSE_URXVT
    ਜੇ (n == KS_URXVT_MOUSE)
	has_mouse_termcode &= ~HMT_URXVT;
    ਨਹੀਂਤਾਂ
# endif
# ifdef FEAT_MOUSE_GPM
    ਜੇ (n == KS_GPM_MOUSE)
	has_mouse_termcode &= ~HMT_GPM;
    ਨਹੀਂਤਾਂ
# endif
    ਜੇ (n == KS_SGR_MOUSE)
	has_mouse_termcode &= ~HMT_SGR;
    ਨਹੀਂਤਾਂ ਜੇ (n == KS_SGR_MOUSE_RELEASE)
	has_mouse_termcode &= ~HMT_SGR_REL;
    ਨਹੀਂਤਾਂ
	has_mouse_termcode &= ~HMT_ਸਾਧਾਰਨ;
}
#ਜੇ_ਅੰਤ

/*
 * setmouse() - switch mouse on/off depending on current mode and 'mouse'
 */
    ਖਾਲੀ
setmouse(ਖਾਲੀ)
{
    ਪੂਰਨ	    checkfor;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MOUSESHAPE
    update_mouseshape(-1);
#ਜੇ_ਅੰਤ

    // Should be outside proc, but may break MOUSESHAPE
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
    // In the GUI the mouse is always enabled.
    ਜੇ (gui.ਵਿੱਚ_use)
	ਵਾਪਸ;
#ਜੇ_ਅੰਤ
    // be quick when mouse is off
    ਜੇ (*p_mouse == NUL || has_mouse_termcode == 0)
	ਵਾਪਸ;

    // don't switch mouse on when not in raw mode (Ex mode)
    ਜੇ (cur_tmode != TMODE_RAW)
    {
	mch_setmouse(FALSE);
	ਵਾਪਸ;
    }

    ਜੇ (VIsual_ਸਰਗਰਮ)
	checkfor = MOUSE_VISUAL;
    ਨਹੀਂਤਾਂ ਜੇ (State == ਬਹੁਲਕ_HITRETURN || State == ਬਹੁਲਕ_ASKMORE
						     || State == ਬਹੁਲਕ_SETWSIZE)
	checkfor = MOUSE_RETURN;
    ਨਹੀਂਤਾਂ ਜੇ (State & ਬਹੁਲਕ_ਪਾਓ)
	checkfor = MOUSE_ਪਾਓ;
    ਨਹੀਂਤਾਂ ਜੇ (State & ਬਹੁਲਕ_CMDLINE)
	checkfor = MOUSE_COMMAND;
    ਨਹੀਂਤਾਂ ਜੇ (State == ਬਹੁਲਕ_CONFIRM || State == ਬਹੁਲਕ_EXTERNCMD)
	checkfor = ' '; // don't use mouse for ":confirm" or ":!cmd"
    ਨਹੀਂਤਾਂ
	checkfor = MOUSE_ਸਾਧਾਰਨ;    // assume normal mode

    ਜੇ (mouse_has(checkfor))
	mch_setmouse(TRUE);
    ਨਹੀਂਤਾਂ
	mch_setmouse(FALSE);
}

/*
 * Return TRUE if
 * - "c" is in 'mouse', or
 * - 'a' is in 'mouse' and "c" is in MOUSE_A, or
 * - the current buffer is a help file and 'h' is in 'mouse' and we are in a
 *   normal editing mode (not at hit-return message).
 */
    ਪੂਰਨ
mouse_has(ਪੂਰਨ c)
{
    char_u	*p;

    ਲਈ (p = p_mouse; *p; ++p)
	ਚੋਣ (*p)
	{
	    ਹਾਲਤ 'a': ਜੇ (vim_strchr((char_u *)MOUSE_A, c) != ਨਿਰਾ)
			  ਵਾਪਸ TRUE;
		      ਰੋਕੋ;
	    ਹਾਲਤ MOUSE_HELP: ਜੇ (c != MOUSE_RETURN && curbuf->b_help)
				 ਵਾਪਸ TRUE;
			     ਰੋਕੋ;
	    ਮੂਲ: ਜੇ (c == *p) ਵਾਪਸ TRUE; ਰੋਕੋ;
	}
    ਵਾਪਸ FALSE;
}

/*
 * Return TRUE when 'mousemodel' is set to "popup" or "popup_setpos".
 */
    ਪੂਰਨ
mouse_model_popup(ਖਾਲੀ)
{
    ਵਾਪਸ (p_mousem[0] == 'p');
}

/*
 * Call a click-region callback function.
 * "regions"/"region_count" describe the resolved click regions,
 * "winid" is stored as the "winid" key in the info dict (0 for tabline).
 * Returns TRUE if the function was called and handled the click.
 */
    ਸਥਿਰ ਪੂਰਨ
stl_click_handler_regions(
	stl_click_region_T  *regions,
	ਪੂਰਨ		    region_ਗਿਣਤੀ,
	ਪੂਰਨ		    winid,
	char_u		    *area_name,
	ਪੂਰਨ		    mrow,
	ਪੂਰਨ		    mcol,
	ਪੂਰਨ		    which_button,
	ਪੂਰਨ		    mods)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    ਪੂਰਨ		n;
    ਪੂਰਨ		nclicks;
    char_u	button_str[2];
    char_u	mods_str[4];
    ਪੂਰਨ		mi = 0;
    dict_T	*info;
    typval_T	argvars[2];
    typval_T	rettv;
    funcexe_T	funcexe;
    ਪੂਰਨ		ਖਾਨਾ = mcol;

    ਜੇ (regions == ਨਿਰਾ || region_ਗਿਣਤੀ == 0)
	ਵਾਪਸ FALSE;

    // Find the click region at the given row and column.
    ਲਈ (n = 0; n < region_ਗਿਣਤੀ; n++)
    {
	ਜੇ (regions[n].ਪੰਕਤੀ == mrow
		&& ਖਾਨਾ >= regions[n].ਖਾਨਾ_ਸ਼ੁਰੂ
		&& ਖਾਨਾ < regions[n].ਖਾਨਾ_ਸਿਰਾ)
	    ਰੋਕੋ;
    }
    ਜੇ (n >= region_ਗਿਣਤੀ || regions[n].funcname == ਨਿਰਾ)
	ਵਾਪਸ FALSE;

    // Build the info dictionary.
    info = dict_alloc();
    ਜੇ (info == ਨਿਰਾ)
	ਵਾਪਸ FALSE;

    dict_ਜੋੜੋ_ਸੰਖਿਆ(info, "minwid", regions[n].minwid);

    // Determine number of clicks.
    // MOD_MASK_2CLICK=0x20, MOD_MASK_3CLICK=0x40, MOD_MASK_4CLICK=0x60
    nclicks = ((mods & MOD_ਨਕਾਬ_MULTI_CLICK) >> 5) + 1;
    ਜੇ (nclicks > 3)
	nclicks = 3;
    dict_ਜੋੜੋ_ਸੰਖਿਆ(info, "nclicks", nclicks);

    // Button.
    ਜੇ (which_button == MOUSE_ਖੱਬਾ)
	button_str[0] = 'l';
    ਨਹੀਂਤਾਂ ਜੇ (which_button == MOUSE_ਸੱਜਾ)
	button_str[0] = 'r';
    ਨਹੀਂਤਾਂ
	button_str[0] = 'm';
    button_str[1] = NUL;
    dict_ਜੋੜੋ_string_len(info, "button", button_str, 1);

    // Modifiers.
    ਜੇ (mods & MOD_ਨਕਾਬ_ਖਿਸਕਾਓ)
	mods_str[mi++] = 's';
    ਜੇ (mods & MOD_ਨਕਾਬ_CTRL)
	mods_str[mi++] = 'c';
    ਜੇ (mods & MOD_ਨਕਾਬ_ALT)
	mods_str[mi++] = 'a';
    mods_str[mi] = NUL;
    dict_ਜੋੜੋ_string_len(info, "mods", mods_str, mi);

    dict_ਜੋੜੋ_ਸੰਖਿਆ(info, "winid", winid);

    // "area": which option the clicked region belongs to.  Lets a shared
    // dispatcher distinguish 'statusline', 'tabline' and 'tabpanel' without
    // having to overload winid == 0.
    dict_ਜੋੜੋ_string(info, "area", area_name);

    // Expose tab page number for 'tabpanel' regions.
    ਜੇ (regions[n].tabnr > 0)
	dict_ਜੋੜੋ_ਸੰਖਿਆ(info, "tabnr", regions[n].tabnr);

    // Call the function with the info dict as argument.
    argvars[0].v_type = VAR_DICT;
    argvars[0].vval.v_dict = info;
    ++info->dv_refcount;
    argvars[1].v_type = VAR_UNKNOWN;

    rettv.v_type = VAR_ਸੰਖਿਆ;
    rettv.vval.v_ਸੰਖਿਆ = 0;

    CLEAR_ਖੇਤਰ(funcexe);
    funcexe.fe_evaluate = TRUE;
    (ਖਾਲੀ)call_func(regions[n].funcname, -1,
					    &rettv, 1, argvars, &funcexe);

    n = (ਪੂਰਨ)rettv.vval.v_ਸੰਖਿਆ;
    clear_tv(&rettv);
    dict_unref(info);

    ਜੇ (n != 0)
    {
	// Make sure the tabline gets redrawn too when the callback asks for
	// a redraw (redraw_statuslines() only redraws the tabline when
	// redraw_tabline is set).  For tabpanel the whole screen needs to be
	// refreshed.
	ਜੇ (winid == 0)
	    redraw_tabline = TRUE;
# ifdef FEAT_TABPANEL
	ਜੇ (STRCMP(area_name, "tabpanel") == 0)
	    redraw_all_later(UPD_NOT_VALID);
# endif
	redraw_statuslines();
    }

    ਵਾਪਸ TRUE;
#ਨਹੀਂਤਾਂ
    (ਖਾਲੀ)regions;
    (ਖਾਲੀ)region_ਗਿਣਤੀ;
    (ਖਾਲੀ)winid;
    (ਖਾਲੀ)area_name;
    (ਖਾਲੀ)mrow;
    (ਖਾਲੀ)mcol;
    (ਖਾਲੀ)which_button;
    (ਖਾਲੀ)mods;
    ਵਾਪਸ FALSE;
#ਜੇ_ਅੰਤ
}

/*
 * Call a statusline click handler function for window "wp".
 * Returns TRUE if the function was called and handled the click.
 */
    ਸਥਿਰ ਪੂਰਨ
stl_click_handler(win_T *wp, ਪੂਰਨ mrow, ਪੂਰਨ mcol, ਪੂਰਨ which_button, ਪੂਰਨ mods)
{
    ਜੇ (wp == ਨਿਰਾ)
	ਵਾਪਸ FALSE;
    ਵਾਪਸ stl_click_handler_regions(wp->w_stl_click, wp->w_stl_click_ਗਿਣਤੀ,
				wp->w_id, (char_u *)"statusline",
				mrow, mcol, which_button, mods);
}

// dragwin is declared near the top of the file

/*
 * Reset the window being dragged.  To be called when switching tab page.
 */
    ਖਾਲੀ
reset_dragwin(ਖਾਲੀ)
{
    dragwin = ਨਿਰਾ;
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
    ਪੂਰਨ
jump_ਮੰਜ਼ਿਲ_mouse(
    ਪੂਰਨ		flags,
    ਪੂਰਨ		*inclusive,	// used for inclusive operator, can be NULL
    ਪੂਰਨ		which_button)	// MOUSE_LEFT, MOUSE_RIGHT, MOUSE_MIDDLE
{
    ਸਥਿਰ ਪੂਰਨ	on_ਸਥਿਤੀ_ਸਤਰ = 0;	// #lines below bottom of window
    ਸਥਿਰ ਪੂਰਨ	on_sep_ਸਤਰ = 0;	// on separator right of window
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MENU
    ਸਥਿਰ ਪੂਰਨ  ਵਿੱਚ_winbar = FALSE;
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
    ਸਥਿਰ ਪੂਰਨ   ਵਿੱਚ_popup_win = FALSE;
    ਸਥਿਰ win_T *click_ਵਿੱਚ_popup_win = ਨਿਰਾ;
#ਜੇ_ਅੰਤ
    ਸਥਿਰ ਪੂਰਨ	prev_ਪੰਕਤੀ = -1;
    ਸਥਿਰ ਪੂਰਨ	prev_ਖਾਨਾ = -1;
    ਸਥਿਰ ਪੂਰਨ	did_drag = FALSE;	// drag was noticed

    win_T	*wp, *old_curwin;
    pos_T	old_cursor;
    ਪੂਰਨ		ਗਿਣਤੀ_2;
    ਪੂਰਨ		ਪਹਿਲਾ;
    ਪੂਰਨ		ਪੰਕਤੀ = mouse_ਪੰਕਤੀ;
    ਪੂਰਨ		ਖਾਨਾ = mouse_ਖਾਨਾ;
    colnr_T	ਖਾਨਾ_from_screen = -1;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
    ਪੂਰਨ		mouse_char = ' ';
#ਜੇ_ਅੰਤ

    mouse_past_bottom = FALSE;
    mouse_past_eol = FALSE;

    ਜੇ (flags & MOUSE_RELEASED)
    {
	// On button release we may change window focus if positioned on a
	// status line and no dragging happened.
	ਜੇ (dragwin != ਨਿਰਾ && !did_drag)
	    flags &= ~(MOUSE_FOCUS | MOUSE_DID_MOVE);
	dragwin = ਨਿਰਾ;
	did_drag = FALSE;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
	ਜੇ (click_ਵਿੱਚ_popup_win != ਨਿਰਾ && popup_dragwin == ਨਿਰਾ)
	    popup_close_for_mouse_click(click_ਵਿੱਚ_popup_win);

	popup_dragwin = ਨਿਰਾ;
	click_ਵਿੱਚ_popup_win = ਨਿਰਾ;
#ਜੇ_ਅੰਤ
    }

    ਜੇ ((flags & MOUSE_DID_MOVE)
	    && prev_ਪੰਕਤੀ == mouse_ਪੰਕਤੀ
	    && prev_ਖਾਨਾ == mouse_ਖਾਨਾ)
    {
retnomove:
	// before moving the cursor for a left click which is NOT in a status
	// line, stop Visual mode
	ਜੇ (on_ਸਥਿਤੀ_ਸਤਰ)
	    ਵਾਪਸ ਵਿੱਚ_ਸਥਿਤੀ_ਸਤਰ;
	ਜੇ (on_sep_ਸਤਰ)
	    ਵਾਪਸ ਵਿੱਚ_SEP_ਸਤਰ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MENU
	ਜੇ (ਵਿੱਚ_winbar)
	{
	    // A quick second click may arrive as a double-click, but we use it
	    // as a second click in the WinBar.
	    ਜੇ ((mod_ਨਕਾਬ & MOD_ਨਕਾਬ_MULTI_CLICK) && !(flags & MOUSE_RELEASED))
	    {
		wp = mouse_ਲੱਭੋ_win(&ਪੰਕਤੀ, &ਖਾਨਾ, FAIL_POPUP);
		ਜੇ (wp == ਨਿਰਾ)
		    ਵਾਪਸ ਵਿੱਚ_UNKNOWN;
		winbar_click(wp, ਖਾਨਾ);
	    }
	    ਵਾਪਸ ਵਿੱਚ_OTHER_WIN | MOUSE_WINBAR;
	}
#ਜੇ_ਅੰਤ
	ਜੇ (flags & MOUSE_MAY_STOP_VIS)
	{
	    ਸਿਰਾ_visual_ਬਹੁਲਕ_keep_button();
	    redraw_curbuf_later(UPD_INVERTED);	// delete the inversion
	}
#ਜੇ defined(FEAT_CLIPBOARD)
	// Continue a modeless selection in another window.
	ਜੇ (cmdwin_type != 0 && ਪੰਕਤੀ < cmdwin_win->w_winrow)
	    ਵਾਪਸ ਵਿੱਚ_OTHER_WIN;
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
	// Continue a modeless selection in a popup window or dragging it.
	ਜੇ (ਵਿੱਚ_popup_win)
	{
	    click_ਵਿੱਚ_popup_win = ਨਿਰਾ;  // don't close it on release
	    ਜੇ (popup_dragwin != ਨਿਰਾ)
	    {
		// dragging a popup window
		popup_drag(popup_dragwin);
		ਵਾਪਸ ਵਿੱਚ_UNKNOWN;
	    }
	    ਵਾਪਸ ਵਿੱਚ_OTHER_WIN;
	}
#ਜੇ_ਅੰਤ
	ਵਾਪਸ ਵਿੱਚ_ਬਫਰ;
    }

    prev_ਪੰਕਤੀ = mouse_ਪੰਕਤੀ;
    prev_ਖਾਨਾ = mouse_ਖਾਨਾ;

    ਜੇ (flags & MOUSE_SETPOS)
	ਜਾਓ retnomove;				// ugly goto...

    old_curwin = curwin;
    old_cursor = curwin->w_cursor;

    ਜੇ (!(flags & MOUSE_FOCUS))
    {
	ਜੇ (ਪੰਕਤੀ < 0 || ਖਾਨਾ < 0) // check if it makes sense
	    ਵਾਪਸ ਵਿੱਚ_UNKNOWN;

	// find the window where the row is in and adjust "row" and "col" to be
	// relative to top-left of the window
	wp = mouse_ਲੱਭੋ_win(&ਪੰਕਤੀ, &ਖਾਨਾ, ਲੱਭੋ_POPUP);
	ਜੇ (wp == ਨਿਰਾ)
	    ਵਾਪਸ ਵਿੱਚ_UNKNOWN;
	dragwin = ਨਿਰਾ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
	// Click in a popup window may start dragging or modeless selection,
	// but not much else.
	ਜੇ (WIN_IS_POPUP(wp))
	{
	    on_sep_ਸਤਰ = 0;
	    on_ਸਥਿਤੀ_ਸਤਰ = 0;
	    ਵਿੱਚ_popup_win = TRUE;
	    ਜੇ (which_button == MOUSE_ਖੱਬਾ && popup_close_if_on_X(wp, ਪੰਕਤੀ, ਖਾਨਾ))
	    {
		ਵਾਪਸ ਵਿੱਚ_UNKNOWN;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (((wp->w_popup_flags & (POPF_DRAG | POPF_RESIZE))
					      && popup_on_border(wp, ਪੰਕਤੀ, ਖਾਨਾ))
				       || (wp->w_popup_flags & POPF_DRAGALL))
	    {
		popup_dragwin = wp;
		popup_ਸ਼ੁਰੂ_drag(wp, ਪੰਕਤੀ, ਖਾਨਾ);
		ਵਾਪਸ ਵਿੱਚ_UNKNOWN;
	    }
	    // Only close on release, otherwise it's not possible to drag or do
	    // modeless selection.
	    ਨਹੀਂਤਾਂ ਜੇ (wp->w_popup_close == POPCLOSE_CLICK
		    && which_button == MOUSE_ਖੱਬਾ)
	    {
		click_ਵਿੱਚ_popup_win = wp;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (which_button == MOUSE_ਖੱਬਾ)
		// If the click is in the scrollbar, may scroll up/down.
		popup_handle_scrollbar_click(wp, ਪੰਕਤੀ, ਖਾਨਾ);
# ifdef FEAT_CLIPBOARD
	    ਵਾਪਸ ਵਿੱਚ_OTHER_WIN;
# ਨਹੀਂਤਾਂ
	    ਵਾਪਸ ਵਿੱਚ_UNKNOWN;
# endif
	}
	ਵਿੱਚ_popup_win = FALSE;
	popup_dragwin = ਨਿਰਾ;
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MENU
	ਜੇ (ਪੰਕਤੀ == -1)
	{
	    // A click in the window toolbar does not enter another window or
	    // change Visual highlighting.
	    winbar_click(wp, ਖਾਨਾ);
	    ਵਿੱਚ_winbar = TRUE;
	    ਵਾਪਸ ਵਿੱਚ_OTHER_WIN | MOUSE_WINBAR;
	}
	ਵਿੱਚ_winbar = FALSE;
#ਜੇ_ਅੰਤ

	// winpos and height may change in win_enter()!
	ਜੇ (ਪੰਕਤੀ >= wp->w_ਉਚਾਈ)		// In (or below) status line
	{
	    on_ਸਥਿਤੀ_ਸਤਰ = ਪੰਕਤੀ - wp->w_ਉਚਾਈ + 1;
	    dragwin = wp;
	}
	ਨਹੀਂਤਾਂ
	    on_ਸਥਿਤੀ_ਸਤਰ = 0;
	ਜੇ (ਖਾਨਾ >= wp->w_ਚੌੜਾਈ)		// In separator line
	{
	    on_sep_ਸਤਰ = ਖਾਨਾ - wp->w_ਚੌੜਾਈ + 1;
	    dragwin = wp;
	}
	ਨਹੀਂਤਾਂ
	    on_sep_ਸਤਰ = 0;

	// The rightmost character of the status line might be a vertical
	// separator character if there is no connecting window to the right.
	ਜੇ (on_ਸਥਿਤੀ_ਸਤਰ && on_sep_ਸਤਰ)
	{
	    ਜੇ (stl_connected(wp))
		on_sep_ਸਤਰ = 0;
	    ਨਹੀਂਤਾਂ
		on_ਸਥਿਤੀ_ਸਤਰ = 0;
	}

	// Before jumping to another buffer, or moving the cursor for a left
	// click, stop Visual mode.
	ਜੇ (VIsual_ਸਰਗਰਮ
		&& (wp->w_ਬਫਰ != curwin->w_ਬਫਰ
		    || (!on_ਸਥਿਤੀ_ਸਤਰ && !on_sep_ਸਤਰ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
			&& (
# ifdef FEAT_RIGHTLEFT
			    wp->w_p_rl ? ਖਾਨਾ < wp->w_ਚੌੜਾਈ - wp->w_p_fdc :
# endif
			    ਖਾਨਾ >= wp->w_p_fdc + (wp != cmdwin_win ? 0 : 1)
			    )
#ਜੇ_ਅੰਤ
			&& (flags & MOUSE_MAY_STOP_VIS))))
	{
	    ਸਿਰਾ_visual_ਬਹੁਲਕ_keep_button();
	    redraw_curbuf_later(UPD_INVERTED);	// delete the inversion
	}
	ਜੇ (cmdwin_type != 0 && wp != cmdwin_win)
	{
	    // A click outside the command-line window: Use modeless
	    // selection if possible.  Allow dragging the status lines.
	    on_sep_ਸਤਰ = 0;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CLIPBOARD
	    ਜੇ (on_ਸਥਿਤੀ_ਸਤਰ)
		ਵਾਪਸ ਵਿੱਚ_ਸਥਿਤੀ_ਸਤਰ;
	    ਵਾਪਸ ਵਿੱਚ_OTHER_WIN;
#ਨਹੀਂਤਾਂ
	    ਪੰਕਤੀ = 0;
	    ਖਾਨਾ += wp->w_wincol;
	    wp = cmdwin_win;
#ਜੇ_ਅੰਤ
	}
#ਜੇ defined(FEAT_PROP_POPUP) && defined(FEAT_TERMINAL)
	ਜੇ (popup_is_popup(curwin) && curbuf->b_ਪਦ != ਨਿਰਾ)
	    // terminal in popup window: don't jump to another window
	    ਵਾਪਸ ਵਿੱਚ_OTHER_WIN;
#ਜੇ_ਅੰਤ
	// Only change window focus when not clicking on or dragging the
	// status line.  Do change focus when releasing the mouse button
	// (MOUSE_FOCUS was set above if we dragged first).
	ਜੇ (dragwin == ਨਿਰਾ || (flags & MOUSE_RELEASED))
	    win_enter(wp, TRUE);		// can make wp invalid!

	ਜੇ (curwin != old_curwin)
	{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CHECK_DOUBLE_CLICK
	    // set topline, to be able to check for double click ourselves
	    ਸੈੱਟ_mouse_topline(curwin);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMINAL
	    // when entering a terminal window may change state
	    ਪਦ_win_entered();
#ਜੇ_ਅੰਤ
	}
	ਜੇ (on_ਸਥਿਤੀ_ਸਤਰ)			// In (or below) status line
	{
	    // Don't use start_arrow() if we're in the same window
	    ਜੇ (curwin == old_curwin)
		ਵਾਪਸ ਵਿੱਚ_ਸਥਿਤੀ_ਸਤਰ;
	    ਨਹੀਂਤਾਂ
		ਵਾਪਸ ਵਿੱਚ_ਸਥਿਤੀ_ਸਤਰ | CURSOR_MOVED;
	}
	ਜੇ (on_sep_ਸਤਰ)			// In (or below) status line
	{
	    // Don't use start_arrow() if we're in the same window
	    ਜੇ (curwin == old_curwin)
		ਵਾਪਸ ਵਿੱਚ_SEP_ਸਤਰ;
	    ਨਹੀਂਤਾਂ
		ਵਾਪਸ ਵਿੱਚ_SEP_ਸਤਰ | CURSOR_MOVED;
	}

	curwin->w_cursor.lnum = curwin->w_topline;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
	// remember topline, needed for double click
	gui_prev_topline = curwin->w_topline;
# ifdef FEAT_DIFF
	gui_prev_topfill = curwin->w_topfill;
# endif
#ਜੇ_ਅੰਤ
    }
    ਨਹੀਂਤਾਂ ਜੇ (on_ਸਥਿਤੀ_ਸਤਰ && which_button == MOUSE_ਖੱਬਾ)
    {
	ਜੇ (dragwin != ਨਿਰਾ)
	{
	    // Drag the status line
	    ਗਿਣਤੀ_2 = ਪੰਕਤੀ - W_WINROW(dragwin) - dragwin->w_ਉਚਾਈ + 1
							     - on_ਸਥਿਤੀ_ਸਤਰ;
	    win_drag_ਸਥਿਤੀ_ਸਤਰ(dragwin, ਗਿਣਤੀ_2);
	    did_drag |= ਗਿਣਤੀ_2;
	}
	ਵਾਪਸ ਵਿੱਚ_ਸਥਿਤੀ_ਸਤਰ;			// Cursor didn't move
    }
    ਨਹੀਂਤਾਂ ਜੇ (on_sep_ਸਤਰ && which_button == MOUSE_ਖੱਬਾ)
    {
	ਜੇ (dragwin != ਨਿਰਾ)
	{
	    // Drag the separator column
	    ਗਿਣਤੀ_2 = ਖਾਨਾ - dragwin->w_wincol - dragwin->w_ਚੌੜਾਈ + 1
								- on_sep_ਸਤਰ;
	    win_drag_vsep_ਸਤਰ(dragwin, ਗਿਣਤੀ_2);
	    did_drag |= ਗਿਣਤੀ_2;
	}
	ਵਾਪਸ ਵਿੱਚ_SEP_ਸਤਰ;			// Cursor didn't move
    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MENU
    ਨਹੀਂਤਾਂ ਜੇ (ਵਿੱਚ_winbar)
    {
	// After a click on the window toolbar don't start Visual mode.
	ਵਾਪਸ ਵਿੱਚ_OTHER_WIN | MOUSE_WINBAR;
    }
#ਜੇ_ਅੰਤ
    ਨਹੀਂਤਾਂ // keep_window_focus must be TRUE
    {
	// before moving the cursor for a left click, stop Visual mode
	ਜੇ (flags & MOUSE_MAY_STOP_VIS)
	{
	    ਸਿਰਾ_visual_ਬਹੁਲਕ_keep_button();
	    redraw_curbuf_later(UPD_INVERTED);	// delete the inversion
	}

#ਜੇ defined(FEAT_CLIPBOARD)
	// Continue a modeless selection in another window.
	ਜੇ (cmdwin_type != 0 && ਪੰਕਤੀ < cmdwin_win->w_winrow)
	    ਵਾਪਸ ਵਿੱਚ_OTHER_WIN;
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
	ਜੇ (ਵਿੱਚ_popup_win)
	{
	    ਜੇ (popup_dragwin != ਨਿਰਾ)
	    {
		// dragging a popup window
		popup_drag(popup_dragwin);
		ਵਾਪਸ ਵਿੱਚ_UNKNOWN;
	    }
	    // continue a modeless selection in a popup window
	    click_ਵਿੱਚ_popup_win = ਨਿਰਾ;
	    ਵਾਪਸ ਵਿੱਚ_OTHER_WIN;
	}
#ਜੇ_ਅੰਤ

	ਪੰਕਤੀ -= W_WINROW(curwin);
	ਖਾਨਾ -= curwin->w_wincol;

	// When clicking beyond the end of the window, scroll the screen.
	// Scroll by however many rows outside the window we are.
	ਜੇ (ਪੰਕਤੀ < 0)
	{
	    ਗਿਣਤੀ_2 = 0;
	    ਲਈ (ਪਹਿਲਾ = TRUE; curwin->w_topline > 1; )
	    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
		ਜੇ (curwin->w_topfill < diff_check_fill(curwin, curwin->w_topline))
		    ++ਗਿਣਤੀ_2;
		ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
		    ਗਿਣਤੀ_2 += plines(curwin->w_topline - 1);
		ਜੇ (!ਪਹਿਲਾ && ਗਿਣਤੀ_2 > -ਪੰਕਤੀ)
		    ਰੋਕੋ;
		ਪਹਿਲਾ = FALSE;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
		(ਖਾਲੀ)hasFolding(curwin->w_topline, &curwin->w_topline, ਨਿਰਾ);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
		ਜੇ (curwin->w_topfill < diff_check_fill(curwin, curwin->w_topline))
		    ++curwin->w_topfill;
		ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
		{
		    --curwin->w_topline;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
		    curwin->w_topfill = 0;
#ਜੇ_ਅੰਤ
		}
	    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
	    check_topfill(curwin, FALSE);
#ਜੇ_ਅੰਤ
	    curwin->w_valid &=
		      ~(VALID_WROW|VALID_CROW|VALID_BOTLINE|VALID_BOTLINE_AP);
	    redraw_later(UPD_VALID);
	    ਪੰਕਤੀ = 0;
	}
	ਨਹੀਂਤਾਂ ਜੇ (ਪੰਕਤੀ >= curwin->w_ਉਚਾਈ)
	{
	    ਗਿਣਤੀ_2 = 0;
	    ਲਈ (ਪਹਿਲਾ = TRUE; curwin->w_topline < curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ; )
	    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
		ਜੇ (curwin->w_topfill > 0)
		    ++ਗਿਣਤੀ_2;
		ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
		    ਗਿਣਤੀ_2 += plines(curwin->w_topline);
		ਜੇ (!ਪਹਿਲਾ && ਗਿਣਤੀ_2 > ਪੰਕਤੀ - curwin->w_ਉਚਾਈ + 1)
		    ਰੋਕੋ;
		ਪਹਿਲਾ = FALSE;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
		ਜੇ (hasFolding(curwin->w_topline, ਨਿਰਾ, &curwin->w_topline)
			&& curwin->w_topline == curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
		    ਰੋਕੋ;
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
		ਜੇ (curwin->w_topfill > 0)
		    --curwin->w_topfill;
		ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
		{
		    ++curwin->w_topline;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
		    curwin->w_topfill =
				   diff_check_fill(curwin, curwin->w_topline);
#ਜੇ_ਅੰਤ
		}
	    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
	    check_topfill(curwin, FALSE);
#ਜੇ_ਅੰਤ
	    redraw_later(UPD_VALID);
	    curwin->w_valid &=
		      ~(VALID_WROW|VALID_CROW|VALID_BOTLINE|VALID_BOTLINE_AP);
	    ਪੰਕਤੀ = curwin->w_ਉਚਾਈ - 1;
	}
	ਨਹੀਂਤਾਂ ਜੇ (ਪੰਕਤੀ == 0)
	{
	    // When dragging the mouse, while the text has been scrolled up as
	    // far as it goes, moving the mouse in the top line should scroll
	    // the text down (done later when recomputing w_topline).
	    ਜੇ (mouse_dragging > 0
		    && curwin->w_cursor.lnum
				       == curwin->w_ਬਫਰ->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ
		    && curwin->w_cursor.lnum == curwin->w_topline)
		curwin->w_valid &= ~(VALID_TOPLINE);
	}
    }

    ਜੇ (prev_ਪੰਕਤੀ >= W_WINROW(curwin)
	&& prev_ਪੰਕਤੀ < W_WINROW(curwin) + curwin->w_ਉਚਾਈ
	&& prev_ਖਾਨਾ >= curwin->w_wincol && prev_ਖਾਨਾ < W_ENDCOL(curwin)
						       && Screen_ਸਤਰਾਂ != ਨਿਰਾ)
    {
	ਪੂਰਨ off = ਸਤਰ_ਵਿੱਥ[prev_ਪੰਕਤੀ] + prev_ਖਾਨਾ;

	// Only use ScreenCols[] after the window was redrawn.  Mainly matters
	// for tests, a user would not click before redrawing.
	// Do not use when 'virtualedit' is active.
	ਜੇ (curwin->w_redr_type <= UPD_VALID_NO_UPDATE)
	    ਖਾਨਾ_from_screen = Screen_ਖਾਨੇ_ਗਿਣਤੀ[off];
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
	// Remember the character under the mouse, it might be a '-' or '+' in
	// the fold column.
	mouse_char = enc_utf8 && Screen_ਸਤਰਾਂ_UC[off] != 0
				       ? Screen_ਸਤਰਾਂ_UC[off] : Screen_ਸਤਰਾਂ[off];
#ਜੇ_ਅੰਤ
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
    // Check for position outside of the fold column.
    ਜੇ (
# ifdef FEAT_RIGHTLEFT
	    curwin->w_p_rl ? ਖਾਨਾ < curwin->w_ਚੌੜਾਈ - curwin->w_p_fdc :
# endif
	    ਖਾਨਾ >= curwin->w_p_fdc + (cmdwin_win != curwin ? 0 : 1)
       )
	mouse_char = ' ';
#ਜੇ_ਅੰਤ

    // compute the position in the buffer line from the posn on the screen
    ਜੇ (mouse_comp_pos(curwin, &ਪੰਕਤੀ, &ਖਾਨਾ, &curwin->w_cursor.lnum, ਨਿਰਾ))
	mouse_past_bottom = TRUE;

    // Start Visual mode before coladvance(), for when 'sel' != "old"
    ਜੇ ((flags & MOUSE_MAY_VIS) && !VIsual_ਸਰਗਰਮ)
    {
	check_visual_highlight();
	VIsual = old_cursor;
	VIsual_ਸਰਗਰਮ = TRUE;
	VIsual_reselect = TRUE;
	// if 'selectmode' contains "mouse", start Select mode
	may_ਸ਼ੁਰੂ_select('o');
	setmouse();
	ਜੇ (p_smd && ਸੁਨੇਹਾ_silent == 0)
	    redraw_cmdline = TRUE;	// show visual mode later
    }

    ਜੇ (ਖਾਨਾ_from_screen >= 0)
    {
	// Use the virtual column from ScreenCols[], it is accurate also after
	// concealed characters.
	ਖਾਨਾ = ਖਾਨਾ_from_screen;
    }

    curwin->w_curswant = ਖਾਨਾ;
    curwin->w_ਸੈੱਟ_curswant = false;	// May still have been TRUE
    ਜੇ (coladvance(ਖਾਨਾ) == FAIL)	// Mouse click beyond end of line
    {
	ਜੇ (inclusive != ਨਿਰਾ)
	    *inclusive = TRUE;
	mouse_past_eol = TRUE;
    }
    ਨਹੀਂਤਾਂ ਜੇ (inclusive != ਨਿਰਾ)
	*inclusive = FALSE;

    ਗਿਣਤੀ_2 = ਵਿੱਚ_ਬਫਰ;
    ਜੇ (curwin != old_curwin || curwin->w_cursor.lnum != old_cursor.lnum
	    || curwin->w_cursor.ਖਾਨਾ != old_cursor.ਖਾਨਾ)
	ਗਿਣਤੀ_2 |= CURSOR_MOVED;		// Cursor has moved

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
    ਜੇ (mouse_char == curwin->w_fill_chars.foldclosed)
	ਗਿਣਤੀ_2 |= MOUSE_FOLD_OPEN;
    ਨਹੀਂਤਾਂ ਜੇ (mouse_char != ' ')
	ਗਿਣਤੀ_2 |= MOUSE_FOLD_CLOSE;
#ਜੇ_ਅੰਤ

    ਵਾਪਸ ਗਿਣਤੀ_2;
}

/*
 * Make a horizontal scroll to "leftcol".
 * Return TRUE if the cursor moved, FALSE otherwise.
 */
    ਪੂਰਨ
do_mousescroll_horiz(long_u leftcol)
{
    ਜੇ (curwin->w_p_wrap)
	ਵਾਪਸ FALSE;  // no horizontal scrolling when wrapping

    ਜੇ (curwin->w_leftcol == (colnr_T)leftcol)
	ਵਾਪਸ FALSE;  // already there

    // When the line of the cursor is too short, move the cursor to the
    // longest visible line.
    ਜੇ (
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
	    (!gui.ਵਿੱਚ_use || vim_strchr(p_go, GO_HORSCROLL) == ਨਿਰਾ) &&
#ਜੇ_ਅੰਤ
		    !virtual_ਸਰਗਰਮ()
	    && (ਲੰਮਾ)leftcol > scroll_ਸਤਰ_len(curwin->w_cursor.lnum))
    {
	curwin->w_cursor.lnum = ui_ਲੱਭੋ_longest_lnum();
	curwin->w_cursor.ਖਾਨਾ = 0;
    }

    ਵਾਪਸ ਸੈੱਟ_leftcol((colnr_T)leftcol);
}

/*
 * Normal and Visual modes implementation for scrolling in direction
 * "cap->arg", which is one of the MSCR_ values.
 */
    ਖਾਲੀ
nv_mousescroll(cmdarg_T *cap)
{
    win_T   *old_curwin = curwin;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TABPANEL
    ਜੇ (mouse_ਪੰਕਤੀ >= 0 && mouse_ਖਾਨਾ >= 0
	    && (cap->arg == MSCR_ਉੱਪਰ || cap->arg == MSCR_ਥੱਲੇ)
	    && mouse_on_tabpanel())
    {
	(ਖਾਲੀ)tabpanel_scroll(cap->arg == MSCR_ਉੱਪਰ ? 1 : -1,
		mouse_vert_ਕਦਮ > 0 ? mouse_vert_ਕਦਮ : 3);
	ਵਾਪਸ;
    }
#ਜੇ_ਅੰਤ

    ਜੇ (mouse_ਪੰਕਤੀ >= 0 && mouse_ਖਾਨਾ >= 0)
    {
	// Find the window at the mouse pointer coordinates.
	// NOTE: Must restore "curwin" to "old_curwin" before returning!
	ਪੂਰਨ ਪੰਕਤੀ = mouse_ਪੰਕਤੀ;
	ਪੂਰਨ ਖਾਨਾ = mouse_ਖਾਨਾ;
	curwin = mouse_ਲੱਭੋ_win(&ਪੰਕਤੀ, &ਖਾਨਾ, ਲੱਭੋ_POPUP);
	ਜੇ (curwin == ਨਿਰਾ)
	{
	    curwin = old_curwin;
	    ਵਾਪਸ;
	}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
	ਜੇ (WIN_IS_POPUP(curwin) && !curwin->w_has_scrollbar)
	{
	    // cannot scroll this popup window
	    curwin = old_curwin;
	    ਵਾਪਸ;
	}
#ਜੇ_ਅੰਤ
	curbuf = curwin->w_ਬਫਰ;
    }

    // Call the common mouse scroll function shared with other modes.
    do_mousescroll(cap);

    curwin->w_redr_ਸਥਿਤੀ = true;
    curwin = old_curwin;
    curbuf = curwin->w_ਬਫਰ;
}

/*
 * Mouse clicks and drags.
 */
    ਖਾਲੀ
nv_mouse(cmdarg_T *cap)
{
    (ਖਾਲੀ)do_mouse(cap->oap, cap->cmdchar, BACKWARD, cap->count1, 0);
}

ਸਥਿਰ ਪੂਰਨ	held_button = MOUSE_RELEASE;

    ਖਾਲੀ
reset_held_button(ਖਾਲੀ)
{
    held_button = MOUSE_RELEASE;
}

/*
 * Check if typebuf 'tp' contains a terminal mouse code and returns the
 * modifiers found in typebuf in 'modifiers'.
 */
    ਪੂਰਨ
check_termcode_mouse(
    char_u	*tp,
    ਪੂਰਨ		*slen,
    char_u	*ਕੁੰਜੀ_name,
    char_u	*modifiers_ਸ਼ੁਰੂ,
    ਪੂਰਨ		idx,
    ਪੂਰਨ		*modifiers)
{
    ਪੂਰਨ		j;
    char_u	*p;
#ਜੇ !defined(UNIX) || defined(FEAT_MOUSE_XTERM) || defined(FEAT_GUI) \
    || defined(FEAT_MOUSE_GPM) || defined(FEAT_SYSMOUSE)
    char_u	ਬਾਈਟਾਂ[6];
    ਪੂਰਨ		ਅੰਕ_ਬਾਈਟਾਂ;
#ਜੇ_ਅੰਤ
    ਪੂਰਨ		mouse_ਕੋਡ = 0;	    // init for GCC
    ਪੂਰਨ		is_click, is_drag;
    ਪੂਰਨ		is_release, release_is_ambiguous;
    ਪੂਰਨ		wheel_ਕੋਡ = 0;
    ਪੂਰਨ		current_button;
    ਸਥਿਰ ਪੂਰਨ	orig_ਅੰਕ_clicks = 1;
    ਸਥਿਰ ਪੂਰਨ	orig_mouse_ਕੋਡ = 0x0;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CHECK_DOUBLE_CLICK
    ਸਥਿਰ ਪੂਰਨ	orig_mouse_ਖਾਨਾ = 0;
    ਸਥਿਰ ਪੂਰਨ	orig_mouse_ਪੰਕਤੀ = 0;
    ਸਥਿਰ ਬਣਤਰ timeval  orig_mouse_ਸਮਾਂ = {0, 0};
    // time of previous mouse click
    ਬਣਤਰ timeval  mouse_ਸਮਾਂ;		// time of current mouse click
    ਲੰਮਾ	timediff;		// elapsed time in msec
#ਜੇ_ਅੰਤ

    is_click = is_drag = is_release = release_is_ambiguous = FALSE;

#ਜੇ !defined(UNIX) || defined(FEAT_MOUSE_XTERM) || defined(FEAT_GUI) \
    || defined(FEAT_MOUSE_GPM) || defined(FEAT_SYSMOUSE)
    ਜੇ (ਕੁੰਜੀ_name[0] == KS_MOUSE
# ifdef FEAT_MOUSE_GPM
	    || ਕੁੰਜੀ_name[0] == KS_GPM_MOUSE
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
	ਲਈ (;;)
	{
	    // For the GUI and for MS-Windows two bytes each are used for row
	    // and column.  Allows for more than 223 columns.
# ਜੇ defined(FEAT_GUI) || defined(MSWIN)
	    ਜੇ (TRUE
#  ਜੇ defined(FEAT_GUI) && !defined(MSWIN)
		&& gui.ਵਿੱਚ_use
#  endif
		)
	    {
		ਅੰਕ_ਬਾਈਟਾਂ = get_ਬਾਈਟਾਂ_from_buf(tp + *slen, ਬਾਈਟਾਂ, 5);
		ਜੇ (ਅੰਕ_ਬਾਈਟਾਂ == -1)	// not enough coordinates
		    ਵਾਪਸ -1;
		mouse_ਕੋਡ = ਬਾਈਟਾਂ[0];
		mouse_ਖਾਨਾ = 128 * (ਬਾਈਟਾਂ[1] - ' ' - 1)
		    + ਬਾਈਟਾਂ[2] - ' ' - 1;
		mouse_ਪੰਕਤੀ = 128 * (ਬਾਈਟਾਂ[3] - ' ' - 1)
		    + ਬਾਈਟਾਂ[4] - ' ' - 1;
	    }
	    ਨਹੀਂਤਾਂ
# endif
	    {
		ਅੰਕ_ਬਾਈਟਾਂ = get_ਬਾਈਟਾਂ_from_buf(tp + *slen, ਬਾਈਟਾਂ, 3);
		ਜੇ (ਅੰਕ_ਬਾਈਟਾਂ == -1)	// not enough coordinates
		    ਵਾਪਸ -1;
		mouse_ਕੋਡ = ਬਾਈਟਾਂ[0];
		mouse_ਖਾਨਾ = ਬਾਈਟਾਂ[1] - ' ' - 1;
		mouse_ਪੰਕਤੀ = ਬਾਈਟਾਂ[2] - ' ' - 1;
	    }
	    *slen += ਅੰਕ_ਬਾਈਟਾਂ;

	    // If the following bytes is also a mouse code and it has the same
	    // code, dump this one and get the next.  This makes dragging a
	    // whole lot faster.
# ifdef FEAT_GUI
	    ਜੇ (gui.ਵਿੱਚ_use)
		j = 3;
	    ਨਹੀਂਤਾਂ
# endif
		j = get_termcode_len(idx);
	    ਜੇ (STRNCMP(tp, tp + *slen, (size_t)j) == 0
		    && tp[*slen + j] == mouse_ਕੋਡ
		    && tp[*slen + j + 1] != NUL
		    && tp[*slen + j + 2] != NUL
# ifdef FEAT_GUI
		    && (!gui.ਵਿੱਚ_use
			|| (tp[*slen + j + 3] != NUL
			    && tp[*slen + j + 4] != NUL))
# endif
	       )
		*slen += j;
	    ਨਹੀਂਤਾਂ
		ਰੋਕੋ;
	}
    }

    ਜੇ (ਕੁੰਜੀ_name[0] == KS_URXVT_MOUSE
	    || ਕੁੰਜੀ_name[0] == KS_SGR_MOUSE
	    || ਕੁੰਜੀ_name[0] == KS_SGR_MOUSE_RELEASE)
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
	p = modifiers_ਸ਼ੁਰੂ;
	ਜੇ (p == ਨਿਰਾ)
	    ਵਾਪਸ -1;

	mouse_ਕੋਡ = getdigits(&p);
	ਜੇ (*p++ != ';')
	    ਵਾਪਸ -1;

	// when mouse reporting is SGR, add 32 to mouse code
	ਜੇ (ਕੁੰਜੀ_name[0] == KS_SGR_MOUSE
		|| ਕੁੰਜੀ_name[0] == KS_SGR_MOUSE_RELEASE)
	    mouse_ਕੋਡ += 32;

	mouse_ਖਾਨਾ = getdigits(&p) - 1;
	ਜੇ (*p++ != ';')
	    ਵਾਪਸ -1;

	mouse_ਪੰਕਤੀ = getdigits(&p) - 1;

	// The modifiers were the mouse coordinates, not the modifier keys
	// (alt/shift/ctrl/meta) state.
	*modifiers = 0;
    }

    ਜੇ (ਕੁੰਜੀ_name[0] == KS_SGR_MOUSE
	    || ਕੁੰਜੀ_name[0] == KS_SGR_MOUSE_RELEASE)
    {
	ਜੇ (ਕੁੰਜੀ_name[0] == KS_SGR_MOUSE_RELEASE)
	{
	    is_release = TRUE;
	    // This is used below to set held_button.
	    mouse_ਕੋਡ |= MOUSE_RELEASE;
	}
    }
    ਨਹੀਂਤਾਂ
    {
	release_is_ambiguous = TRUE;
	ਜੇ ((mouse_ਕੋਡ & MOUSE_RELEASE) == MOUSE_RELEASE)
	    is_release = TRUE;
    }

    ਜੇ (ਕੁੰਜੀ_name[0] == KS_MOUSE
# ifdef FEAT_MOUSE_GPM
	    || ਕੁੰਜੀ_name[0] == KS_GPM_MOUSE
# endif
# ifdef FEAT_MOUSE_URXVT
	    || ਕੁੰਜੀ_name[0] == KS_URXVT_MOUSE
# endif
	    || ਕੁੰਜੀ_name[0] == KS_SGR_MOUSE
	    || ਕੁੰਜੀ_name[0] == KS_SGR_MOUSE_RELEASE)
    {
# ਜੇ !defined(MSWIN)
	/*
	 * Handle old style mouse events.
	 * Recognize the xterm mouse wheel, but not in the GUI, the
	 * Linux console with GPM and the MS-DOS or Win32 console
	 * (multi-clicks use >= 0x60).
	 */
	ਜੇ (mouse_ਕੋਡ >= MOUSEWHEEL_ਨੀਵਾਂ && mouse_ਕੋਡ < MOUSESIDEBUTTONS_ਨੀਵਾਂ
#  ifdef FEAT_GUI
		&& !gui.ਵਿੱਚ_use
#  endif
#  ifdef FEAT_MOUSE_GPM
		&& ਕੁੰਜੀ_name[0] != KS_GPM_MOUSE
#  endif
	   )
	{
#  ਜੇ defined(UNIX)
	    ਜੇ (use_xterm_mouse() > 1 && mouse_ਕੋਡ >= 0x80)
		// mouse-move event, using MOUSE_DRAG works
		mouse_ਕੋਡ = MOUSE_DRAG;
	    ਨਹੀਂਤਾਂ
#  endif
		// Keep the mouse_code before it's changed, so that we
		// remember that it was a mouse wheel click.
		wheel_ਕੋਡ = mouse_ਕੋਡ;
	}
#  ifdef FEAT_MOUSE_XTERM
	ਨਹੀਂਤਾਂ ਜੇ (held_button == MOUSE_RELEASE
#   ifdef FEAT_GUI
		&& !gui.ਵਿੱਚ_use
#   endif
		&& (mouse_ਕੋਡ == 0x23 || mouse_ਕੋਡ == 0x24
		    || mouse_ਕੋਡ == 0x40 || mouse_ਕੋਡ == 0x41))
	{
	    // Apparently 0x23 and 0x24 are used by rxvt scroll wheel.
	    // And 0x40 and 0x41 are used by some xterm emulator.
	    wheel_ਕੋਡ = mouse_ਕੋਡ - (mouse_ਕੋਡ >= 0x40 ? 0x40 : 0x23)
							      + MOUSEWHEEL_ਨੀਵਾਂ;
	}
#  endif

#  ਜੇ defined(UNIX)
	ਨਹੀਂਤਾਂ ਜੇ (use_xterm_mouse() > 1)
	{
	    ਜੇ (mouse_ਕੋਡ & MOUSE_DRAG_XTERM)
		mouse_ਕੋਡ |= MOUSE_DRAG;
	}
#  endif
#  ifdef FEAT_XCLIPBOARD
	ਨਹੀਂਤਾਂ ਜੇ (!(mouse_ਕੋਡ & MOUSE_DRAG & ~MOUSE_CLICK_ਨਕਾਬ))
	{
	    ਜੇ (is_release)
		stop_xterm_trace();
	    ਨਹੀਂਤਾਂ
		ਸ਼ੁਰੂ_xterm_trace(mouse_ਕੋਡ);
	}
#  endif
# endif
    }
#ਜੇ_ਅੰਤ // !UNIX || FEAT_MOUSE_XTERM
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MOUSE_NET
    ਜੇ (ਕੁੰਜੀ_name[0] == KS_NETTERM_MOUSE)
    {
	ਪੂਰਨ mc, mr;

	// expect a rather limited sequence like: balancing {
	// \033}6,45\r
	// '6' is the row, 45 is the column
	p = tp + *slen;
	mr = getdigits(&p);
	ਜੇ (*p++ != ',')
	    ਵਾਪਸ -1;
	mc = getdigits(&p);
	ਜੇ (*p++ != '\r')
	    ਵਾਪਸ -1;

	mouse_ਖਾਨਾ = mc - 1;
	mouse_ਪੰਕਤੀ = mr - 1;
	mouse_ਕੋਡ = MOUSE_ਖੱਬਾ;
	*slen += (ਪੂਰਨ)(p - (tp + *slen));
    }
#ਜੇ_ਅੰਤ	// FEAT_MOUSE_NET
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MOUSE_JSB
    ਜੇ (ਕੁੰਜੀ_name[0] == KS_JSBTERM_MOUSE)
    {
	ਪੂਰਨ mult, val, iter, button, ਸਥਿਤੀ;

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
	button = mouse_ਕੋਡ = 0;
	ਚੋਣ (*p++)
	{
	    ਹਾਲਤ 'L': button = 1; ਰੋਕੋ;
	    ਹਾਲਤ '-': ਰੋਕੋ;
	    ਹਾਲਤ 'x': ਰੋਕੋ; // ignore sequence
	    ਮੂਲ:  ਵਾਪਸ -1; // Unknown Result
	}
	ਚੋਣ (*p++)
	{
	    ਹਾਲਤ 'M': button |= 2; ਰੋਕੋ;
	    ਹਾਲਤ '-': ਰੋਕੋ;
	    ਹਾਲਤ 'x': ਰੋਕੋ; // ignore sequence
	    ਮੂਲ:  ਵਾਪਸ -1; // Unknown Result
	}
	ਚੋਣ (*p++)
	{
	    ਹਾਲਤ 'R': button |= 4; ਰੋਕੋ;
	    ਹਾਲਤ '-': ਰੋਕੋ;
	    ਹਾਲਤ 'x': ਰੋਕੋ; // ignore sequence
	    ਮੂਲ:  ਵਾਪਸ -1; // Unknown Result
	}
	ਸਥਿਤੀ = *p++;
	ਲਈ (val = 0, mult = 100, iter = 0; iter < 3; iter++,
		mult /= 10, p++)
	    ਜੇ (*p >= '0' && *p <= '9')
		val += (*p - '0') * mult;
	    ਨਹੀਂਤਾਂ
		ਵਾਪਸ -1;
	mouse_ਖਾਨਾ = val;
	ਲਈ (val = 0, mult = 100, iter = 0; iter < 3; iter++,
		mult /= 10, p++)
	    ਜੇ (*p >= '0' && *p <= '9')
		val += (*p - '0') * mult;
	    ਨਹੀਂਤਾਂ
		ਵਾਪਸ -1;
	mouse_ਪੰਕਤੀ = val;
	ਚੋਣ (*p++)
	{
	    ਹਾਲਤ 's': button |= 8; ਰੋਕੋ;  // SHIFT key Pressed
	    ਹਾਲਤ '-': ਰੋਕੋ;  // Not Pressed
	    ਹਾਲਤ 'x': ਰੋਕੋ;  // Not Reporting
	    ਮੂਲ:  ਵਾਪਸ -1; // Unknown Result
	}
	ਚੋਣ (*p++)
	{
	    ਹਾਲਤ 'c': button |= 16; ਰੋਕੋ;  // CTRL key Pressed
	    ਹਾਲਤ '-': ਰੋਕੋ;  // Not Pressed
	    ਹਾਲਤ 'x': ਰੋਕੋ;  // Not Reporting
	    ਮੂਲ:  ਵਾਪਸ -1; // Unknown Result
	}
	ਜੇ (*p++ != '\033')
	    ਵਾਪਸ -1;
	ਜੇ (*p++ != '\\')
	    ਵਾਪਸ -1;
	ਚੋਣ (ਸਥਿਤੀ)
	{
	    ਹਾਲਤ 'D': // Double Click
	    ਹਾਲਤ 'S': // Single Click
		ਜੇ (button & 1) mouse_ਕੋਡ |= MOUSE_ਖੱਬਾ;
		ਜੇ (button & 2) mouse_ਕੋਡ |= MOUSE_ਵਿਚਕਾਰ;
		ਜੇ (button & 4) mouse_ਕੋਡ |= MOUSE_ਸੱਜਾ;
		ਜੇ (button & 8) mouse_ਕੋਡ |= MOUSE_ਖਿਸਕਾਓ;
		ਜੇ (button & 16) mouse_ਕੋਡ |= MOUSE_CTRL;
		ਰੋਕੋ;
	    ਹਾਲਤ 'm': // Mouse move
		ਜੇ (button & 1) mouse_ਕੋਡ |= MOUSE_ਖੱਬਾ;
		ਜੇ (button & 2) mouse_ਕੋਡ |= MOUSE_ਵਿਚਕਾਰ;
		ਜੇ (button & 4) mouse_ਕੋਡ |= MOUSE_ਸੱਜਾ;
		ਜੇ (button & 8) mouse_ਕੋਡ |= MOUSE_ਖਿਸਕਾਓ;
		ਜੇ (button & 16) mouse_ਕੋਡ |= MOUSE_CTRL;
		ਜੇ ((button & 7) != 0)
		{
		    held_button = mouse_ਕੋਡ;
		    mouse_ਕੋਡ |= MOUSE_DRAG;
		}
		is_drag = TRUE;
		showmode();
		ਰੋਕੋ;
	    ਹਾਲਤ 'd': // Button Down
		ਜੇ (button & 1) mouse_ਕੋਡ |= MOUSE_ਖੱਬਾ;
		ਜੇ (button & 2) mouse_ਕੋਡ |= MOUSE_ਵਿਚਕਾਰ;
		ਜੇ (button & 4) mouse_ਕੋਡ |= MOUSE_ਸੱਜਾ;
		ਜੇ (button & 8) mouse_ਕੋਡ |= MOUSE_ਖਿਸਕਾਓ;
		ਜੇ (button & 16) mouse_ਕੋਡ |= MOUSE_CTRL;
		ਰੋਕੋ;
	    ਹਾਲਤ 'u': // Button Up
		is_release = TRUE;
		ਜੇ (button & 1)
		    mouse_ਕੋਡ |= MOUSE_ਖੱਬਾ;
		ਜੇ (button & 2)
		    mouse_ਕੋਡ |= MOUSE_ਵਿਚਕਾਰ;
		ਜੇ (button & 4)
		    mouse_ਕੋਡ |= MOUSE_ਸੱਜਾ;
		ਜੇ (button & 8)
		    mouse_ਕੋਡ |= MOUSE_ਖਿਸਕਾਓ;
		ਜੇ (button & 16)
		    mouse_ਕੋਡ |= MOUSE_CTRL;
		ਰੋਕੋ;
	    ਮੂਲ: ਵਾਪਸ -1; // Unknown Result
	}

	*slen += (p - (tp + *slen));
    }
#ਜੇ_ਅੰਤ // FEAT_MOUSE_JSB
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MOUSE_DEC
    ਜੇ (ਕੁੰਜੀ_name[0] == KS_DEC_MOUSE)
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
	ਪੂਰਨ Pe, Pb, Pr, Pc;

	p = tp + *slen;

	// get event status
	Pe = getdigits(&p);
	ਜੇ (*p++ != ';')
	    ਵਾਪਸ -1;

	// get button status
	Pb = getdigits(&p);
	ਜੇ (*p++ != ';')
	    ਵਾਪਸ -1;

	// get row status
	Pr = getdigits(&p);
	ਜੇ (*p++ != ';')
	    ਵਾਪਸ -1;

	// get column status
	Pc = getdigits(&p);

	// the page parameter is optional
	ਜੇ (*p == ';')
	{
	    p++;
	    (ਖਾਲੀ)getdigits(&p);
	}
	ਜੇ (*p++ != '&')
	    ਵਾਪਸ -1;
	ਜੇ (*p++ != 'w')
	    ਵਾਪਸ -1;

	mouse_ਕੋਡ = 0;
	ਚੋਣ (Pe)
	{
	    ਹਾਲਤ  0: ਵਾਪਸ -1; // position request while unavailable
	    ਹਾਲਤ  1: // a response to a locator position request includes
		     //	the status of all buttons
		     Pb &= 7;   // mask off and ignore fourth button
		     ਜੇ (Pb & 4)
			 mouse_ਕੋਡ  = MOUSE_ਖੱਬਾ;
		     ਜੇ (Pb & 2)
			 mouse_ਕੋਡ  = MOUSE_ਵਿਚਕਾਰ;
		     ਜੇ (Pb & 1)
			 mouse_ਕੋਡ  = MOUSE_ਸੱਜਾ;
		     ਜੇ (Pb)
		     {
			 held_button = mouse_ਕੋਡ;
			 mouse_ਕੋਡ |= MOUSE_DRAG;
			 WantQueryMouse = TRUE;
		     }
		     is_drag = TRUE;
		     showmode();
		     ਰੋਕੋ;
	    ਹਾਲਤ  2: mouse_ਕੋਡ = MOUSE_ਖੱਬਾ;
		     WantQueryMouse = TRUE;
		     ਰੋਕੋ;
	    ਹਾਲਤ  3: mouse_ਕੋਡ = MOUSE_ਖੱਬਾ;
		     is_release = TRUE;
		     ਰੋਕੋ;
	    ਹਾਲਤ  4: mouse_ਕੋਡ = MOUSE_ਵਿਚਕਾਰ;
		     WantQueryMouse = TRUE;
		     ਰੋਕੋ;
	    ਹਾਲਤ  5: mouse_ਕੋਡ = MOUSE_ਵਿਚਕਾਰ;
		     is_release = TRUE;
		     ਰੋਕੋ;
	    ਹਾਲਤ  6: mouse_ਕੋਡ = MOUSE_ਸੱਜਾ;
		     WantQueryMouse = TRUE;
		     ਰੋਕੋ;
	    ਹਾਲਤ  7: mouse_ਕੋਡ = MOUSE_ਸੱਜਾ;
		     is_release = TRUE;
		     ਰੋਕੋ;
	    ਹਾਲਤ  8: ਵਾਪਸ -1; // fourth button down
	    ਹਾਲਤ  9: ਵਾਪਸ -1; // fourth button up
	    ਹਾਲਤ 10: ਵਾਪਸ -1; // mouse outside of filter rectangle
	    ਮੂਲ: ਵਾਪਸ -1; // should never occur
	}

	mouse_ਖਾਨਾ = Pc - 1;
	mouse_ਪੰਕਤੀ = Pr - 1;

	*slen += (ਪੂਰਨ)(p - (tp + *slen));
    }
#ਜੇ_ਅੰਤ // FEAT_MOUSE_DEC
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MOUSE_PTERM
    ਜੇ (ਕੁੰਜੀ_name[0] == KS_PTERM_MOUSE)
    {
	ਪੂਰਨ button, ਅੰਕ_clicks, action;

	p = tp + *slen;

	action = getdigits(&p);
	ਜੇ (*p++ != ';')
	    ਵਾਪਸ -1;

	mouse_ਪੰਕਤੀ = getdigits(&p);
	ਜੇ (*p++ != ';')
	    ਵਾਪਸ -1;
	mouse_ਖਾਨਾ = getdigits(&p);
	ਜੇ (*p++ != ';')
	    ਵਾਪਸ -1;

	button = getdigits(&p);
	mouse_ਕੋਡ = 0;

	ਚੋਣ (button)
	{
	    ਹਾਲਤ 4: mouse_ਕੋਡ = MOUSE_ਖੱਬਾ; ਰੋਕੋ;
	    ਹਾਲਤ 1: mouse_ਕੋਡ = MOUSE_ਸੱਜਾ; ਰੋਕੋ;
	    ਹਾਲਤ 2: mouse_ਕੋਡ = MOUSE_ਵਿਚਕਾਰ; ਰੋਕੋ;
	    ਮੂਲ: ਵਾਪਸ -1;
	}

	ਚੋਣ (action)
	{
	    ਹਾਲਤ 31: // Initial press
		ਜੇ (*p++ != ';')
		    ਵਾਪਸ -1;

		ਅੰਕ_clicks = getdigits(&p); // Not used
		ਰੋਕੋ;

	    ਹਾਲਤ 32: // Release
		is_release = TRUE;
		ਰੋਕੋ;

	    ਹਾਲਤ 33: // Drag
		held_button = mouse_ਕੋਡ;
		mouse_ਕੋਡ |= MOUSE_DRAG;
		ਰੋਕੋ;

	    ਮੂਲ:
		ਵਾਪਸ -1;
	}

	ਜੇ (*p++ != 't')
	    ਵਾਪਸ -1;

	*slen += (p - (tp + *slen));
    }
#ਜੇ_ਅੰਤ // FEAT_MOUSE_PTERM

    // Interpret the mouse code
    current_button = (mouse_ਕੋਡ & MOUSE_CLICK_ਨਕਾਬ);
    ਜੇ (is_release)
	current_button |= MOUSE_RELEASE;

    ਜੇ (current_button == MOUSE_RELEASE
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MOUSE_XTERM
	    && wheel_ਕੋਡ == 0
#ਜੇ_ਅੰਤ
       )
    {
	/*
	 * If we get a mouse drag or release event when there is no mouse
	 * button held down (held_button == MOUSE_RELEASE), produce a K_IGNORE
	 * below.
	 * (can happen when you hold down two buttons and then let them go, or
	 * click in the menu bar, but not on a menu, and drag into the text).
	 */
	ਜੇ ((mouse_ਕੋਡ & MOUSE_DRAG) == MOUSE_DRAG)
	    is_drag = TRUE;
	current_button = held_button;
    }
    ਨਹੀਂਤਾਂ
    {
      ਜੇ (wheel_ਕੋਡ == 0)
      {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CHECK_DOUBLE_CLICK
# ifdef FEAT_MOUSE_GPM
	/*
	 * Only for Unix, when GUI not active, we handle multi-clicks here, but
	 * not for GPM mouse events.
	 */
#  ifdef FEAT_GUI
	ਜੇ (ਕੁੰਜੀ_name[0] != KS_GPM_MOUSE && !gui.ਵਿੱਚ_use)
#  ਨਹੀਂਤਾਂ
	    ਜੇ (ਕੁੰਜੀ_name[0] != KS_GPM_MOUSE)
#  endif
# ਨਹੀਂਤਾਂ
#  ifdef FEAT_GUI
		ਜੇ (!gui.ਵਿੱਚ_use)
#  endif
# endif
		{
		    /*
		     * Compute the time elapsed since the previous mouse click.
		     */
		    gettimeofday(&mouse_ਸਮਾਂ, ਨਿਰਾ);
		    ਜੇ (orig_mouse_ਸਮਾਂ.tv_sec == 0)
		    {
			/*
			 * Avoid computing the difference between mouse_time
			 * and orig_mouse_time for the first click, as the
			 * difference would be huge and would cause
			 * multiplication overflow.
			 */
			timediff = p_mouset;
		    }
		    ਨਹੀਂਤਾਂ
			timediff = ਸਮਾਂ_diff_ms(&orig_mouse_ਸਮਾਂ, &mouse_ਸਮਾਂ);
		    orig_mouse_ਸਮਾਂ = mouse_ਸਮਾਂ;
		    ਜੇ (mouse_ਕੋਡ == orig_mouse_ਕੋਡ
			    && timediff < p_mouset
			    && orig_ਅੰਕ_clicks != 4
			    && orig_mouse_ਖਾਨਾ == mouse_ਖਾਨਾ
			    && orig_mouse_ਪੰਕਤੀ == mouse_ਪੰਕਤੀ
			    && (is_mouse_topline(curwin)
				// Double click in tab pages line also works
				// when window contents changes.
				|| (mouse_ਪੰਕਤੀ == 0 && firstwin->w_winrow > 0))
		       )
			++orig_ਅੰਕ_clicks;
		    ਨਹੀਂਤਾਂ
			orig_ਅੰਕ_clicks = 1;
		    orig_mouse_ਖਾਨਾ = mouse_ਖਾਨਾ;
		    orig_mouse_ਪੰਕਤੀ = mouse_ਪੰਕਤੀ;
		    ਸੈੱਟ_mouse_topline(curwin);
		}
# ਜੇ defined(FEAT_GUI) || defined(FEAT_MOUSE_GPM)
		ਨਹੀਂਤਾਂ
		    orig_ਅੰਕ_clicks = ਅੰਕ_MOUSE_CLICKS(mouse_ਕੋਡ);
# endif
#ਨਹੀਂਤਾਂ
	orig_ਅੰਕ_clicks = ਅੰਕ_MOUSE_CLICKS(mouse_ਕੋਡ);
#ਜੇ_ਅੰਤ
	is_click = TRUE;
      }
      orig_mouse_ਕੋਡ = mouse_ਕੋਡ;
    }
    ਜੇ (!is_drag)
	held_button = mouse_ਕੋਡ & MOUSE_CLICK_ਨਕਾਬ;

    /*
     * Translate the actual mouse event into a pseudo mouse event.
     * First work out what modifiers are to be used.
     */
    ਜੇ (orig_mouse_ਕੋਡ & MOUSE_ਖਿਸਕਾਓ)
	*modifiers |= MOD_ਨਕਾਬ_ਖਿਸਕਾਓ;
    ਜੇ (orig_mouse_ਕੋਡ & MOUSE_CTRL)
	*modifiers |= MOD_ਨਕਾਬ_CTRL;
    ਜੇ (orig_mouse_ਕੋਡ & MOUSE_ALT)
	*modifiers |= MOD_ਨਕਾਬ_ALT;
    ਜੇ (orig_ਅੰਕ_clicks == 2)
	*modifiers |= MOD_ਨਕਾਬ_2_CLICK;
    ਨਹੀਂਤਾਂ ਜੇ (orig_ਅੰਕ_clicks == 3)
	*modifiers |= MOD_ਨਕਾਬ_3_CLICK;
    ਨਹੀਂਤਾਂ ਜੇ (orig_ਅੰਕ_clicks == 4)
	*modifiers |= MOD_ਨਕਾਬ_4_CLICK;

    // Work out our pseudo mouse event. Note that MOUSE_RELEASE gets added,
    // then it's not mouse up/down.
    ਕੁੰਜੀ_name[0] = KS_EXTRA;
    ਜੇ (wheel_ਕੋਡ != 0 && (!is_release || release_is_ambiguous))
    {
	ਜੇ (wheel_ਕੋਡ & MOUSE_CTRL)
	    *modifiers |= MOD_ਨਕਾਬ_CTRL;
	ਜੇ (wheel_ਕੋਡ & MOUSE_ALT)
	    *modifiers |= MOD_ਨਕਾਬ_ALT;

	ਜੇ (wheel_ਕੋਡ & 1 && wheel_ਕੋਡ & 2)
	    ਕੁੰਜੀ_name[1] = (ਪੂਰਨ)KE_MOUSELEFT;
	ਨਹੀਂਤਾਂ ਜੇ (wheel_ਕੋਡ & 2)
	    ਕੁੰਜੀ_name[1] = (ਪੂਰਨ)KE_MOUSERIGHT;
	ਨਹੀਂਤਾਂ ਜੇ (wheel_ਕੋਡ & 1)
	    ਕੁੰਜੀ_name[1] = (ਪੂਰਨ)KE_MOUSEUP;
	ਨਹੀਂਤਾਂ
	    ਕੁੰਜੀ_name[1] = (ਪੂਰਨ)KE_MOUSEDOWN;

	held_button = MOUSE_RELEASE;
    }
    ਨਹੀਂਤਾਂ
    {
#ਜੇ defined(UNIX)
	ਜੇ (use_xterm_mouse() && orig_mouse_ਕੋਡ >= MOUSESIDEBUTTONS_ਨੀਵਾਂ)
	    current_button = (current_button) ? MOUSE_X2 : MOUSE_X1;
#ਜੇ_ਅੰਤ
	ਕੁੰਜੀ_name[1] = get_pseudo_mouse_ਕੋਡ(current_button, is_click, is_drag);
    }


    // Make sure the mouse position is valid.  Some terminals may return weird
    // values.
    ਜੇ (mouse_ਖਾਨਾ >= Columns)
	mouse_ਖਾਨਾ = Columns - 1;
    ਜੇ (mouse_ਪੰਕਤੀ >= ਪੰਕਤੀਆਂ)
	mouse_ਪੰਕਤੀ = ਪੰਕਤੀਆਂ - 1;

    ਵਾਪਸ 0;
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
    ਪੂਰਨ
mouse_comp_pos(
    win_T	*win,
    ਪੂਰਨ		*rowp,
    ਪੂਰਨ		*colp,
    linenr_T	*lnump,
    ਪੂਰਨ		*plines_cache)
{
    ਪੂਰਨ		ਖਾਨਾ = *colp;
    ਪੂਰਨ		ਪੰਕਤੀ = *rowp;
    linenr_T	lnum;
    ਪੂਰਨ		retval = FALSE;
    ਪੂਰਨ		off;
    ਪੂਰਨ		ਗਿਣਤੀ_2;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RIGHTLEFT
    ਜੇ (win->w_p_rl)
	ਖਾਨਾ = win->w_ਚੌੜਾਈ - 1 - ਖਾਨਾ;
#ਜੇ_ਅੰਤ

    lnum = win->w_topline;

    ਜਦੋਂ (ਪੰਕਤੀ > 0)
    {
	ਪੂਰਨ cache_idx = lnum - win->w_topline;

	// Only "Rows" lines are cached, with folding we'll run out of entries
	// and use the slow way.
	ਜੇ (plines_cache != ਨਿਰਾ && cache_idx < ਪੰਕਤੀਆਂ
						&& plines_cache[cache_idx] > 0)
	    ਗਿਣਤੀ_2 = plines_cache[cache_idx];
	ਨਹੀਂਤਾਂ
	{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
	    // Don't include filler lines in "count"
	    ਜੇ (win->w_p_diff
# ifdef FEAT_FOLDING
		    && !hasFoldingWin(win, lnum, ਨਿਰਾ, ਨਿਰਾ, TRUE, ਨਿਰਾ)
# endif
		    )
	    {
		ਜੇ (lnum == win->w_topline)
		    ਪੰਕਤੀ -= win->w_topfill;
		ਨਹੀਂਤਾਂ
		    ਪੰਕਤੀ -= diff_check_fill(win, lnum);
		ਗਿਣਤੀ_2 = plines_win_nofill(win, lnum, FALSE);
	    }
	    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
		ਗਿਣਤੀ_2 = plines_win(win, lnum, FALSE);
	    ਜੇ (plines_cache != ਨਿਰਾ && cache_idx < ਪੰਕਤੀਆਂ)
		plines_cache[cache_idx] = ਗਿਣਤੀ_2;
	}

	ਜੇ (win->w_skipcol > 0 && lnum == win->w_topline)
	{
	    ਪੂਰਨ width1 = win->w_ਚੌੜਾਈ - win_ਖਾਨਾ_off(win);

	    ਜੇ (width1 > 0)
	    {
		ਪੂਰਨ skip_ਸਤਰਾਂ = 0;

		// Adjust for 'smoothscroll' clipping the top screen lines.
		// A similar formula is used in curs_columns().
		ਜੇ (win->w_skipcol > width1)
		    skip_ਸਤਰਾਂ = (win->w_skipcol - width1)
					    / (width1 + win_ਖਾਨਾ_off2(win)) + 1;
		ਨਹੀਂਤਾਂ ਜੇ (win->w_skipcol > 0)
		    skip_ਸਤਰਾਂ = 1;

		ਗਿਣਤੀ_2 -= skip_ਸਤਰਾਂ;
	    }
	}

	ਜੇ (ਗਿਣਤੀ_2 > ਪੰਕਤੀ)
	    ਰੋਕੋ;	// Position is in this buffer line.
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
	(ਖਾਲੀ)hasFoldingWin(win, lnum, ਨਿਰਾ, &lnum, TRUE, ਨਿਰਾ);
#ਜੇ_ਅੰਤ
	ਜੇ (lnum == win->w_ਬਫਰ->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
	{
	    retval = TRUE;
	    ਰੋਕੋ;		// past end of file
	}
	ਪੰਕਤੀ -= ਗਿਣਤੀ_2;
	++lnum;
    }

    ਜੇ (!retval)
    {
	// Compute the column without wrapping.
	off = win_ਖਾਨਾ_off(win) - win_ਖਾਨਾ_off2(win);
	ਜੇ (ਖਾਨਾ < off)
	    ਖਾਨਾ = off;
	ਖਾਨਾ += ਪੰਕਤੀ * (win->w_ਚੌੜਾਈ - off);

	// Add skip column for the topline.
	ਜੇ (lnum == win->w_topline)
	    ਖਾਨਾ += win->w_skipcol;
    }

    ਜੇ (!win->w_p_wrap)
	ਖਾਨਾ += win->w_leftcol;

    // skip line number and fold column in front of the line
    ਖਾਨਾ -= win_ਖਾਨਾ_off(win);
    ਜੇ (ਖਾਨਾ <= 0)
    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_NETBEANS_INTG
	// if mouse is clicked on the gutter, then inform the netbeans server
	ਜੇ (*colp < win_ਖਾਨਾ_off(win))
	    netbeans_gutter_click(lnum);
#ਜੇ_ਅੰਤ
	ਖਾਨਾ = 0;
    }

    *colp = ਖਾਨਾ;
    *rowp = ਪੰਕਤੀ;
    *lnump = lnum;
    ਵਾਪਸ retval;
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
mouse_ਲੱਭੋ_win(ਪੂਰਨ *rowp, ਪੂਰਨ *colp, mouse_ਲੱਭੋ_T popup ਅਣਵਰਤਿਆ)
{
    ਚੌਖਟਾ_T	*fp;
    win_T	*wp;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
    win_T	*pwp = ਨਿਰਾ;

    ਜੇ (popup != IGNORE_POPUP)
    {
	popup_reset_handled(POPUP_HANDLED_1);
	ਜਦੋਂ ((wp = ਲੱਭੋ_ਅਗਲਾ_popup(TRUE, POPUP_HANDLED_1)) != ਨਿਰਾ)
	{
	    ਜੇ (*rowp >= wp->w_winrow && *rowp < wp->w_winrow + popup_ਉਚਾਈ(wp)
		    && *colp >= wp->w_wincol
				    && *colp < wp->w_wincol + popup_ਚੌੜਾਈ(wp))
		pwp = wp;
	}
	ਜੇ (pwp != ਨਿਰਾ)
	{
	    ਜੇ (popup == FAIL_POPUP)
		ਵਾਪਸ ਨਿਰਾ;
	    *rowp -= pwp->w_winrow;
	    *colp -= pwp->w_wincol;
	    ਵਾਪਸ pwp;
	}
    }
#ਜੇ_ਅੰਤ

    fp = topframe;

    ਜੇ (*colp < firstwin->w_wincol
	    || *colp >= firstwin->w_wincol + fp->fr_ਚੌੜਾਈ
	    || *rowp < firstwin->w_winrow)
	ਵਾਪਸ ਨਿਰਾ;

    *rowp -= firstwin->w_winrow;
    *colp -= firstwin->w_wincol;
    ਲਈ (;;)
    {
	ਜੇ (fp->fr_layout == FR_ਪੱਤੀ)
	    ਰੋਕੋ;
	ਜੇ (fp->fr_layout == FR_ਪੰਕਤੀ)
	{
	    ਲਈ (fp = fp->fr_ਬੱਚਾ; fp->fr_ਅਗਲਾ != ਨਿਰਾ; fp = fp->fr_ਅਗਲਾ)
	    {
		ਜੇ (*colp < fp->fr_ਚੌੜਾਈ)
		    ਰੋਕੋ;
		*colp -= fp->fr_ਚੌੜਾਈ;
	    }
	}
	ਨਹੀਂਤਾਂ    // fr_layout == FR_COL
	{
	    ਲਈ (fp = fp->fr_ਬੱਚਾ; fp->fr_ਅਗਲਾ != ਨਿਰਾ; fp = fp->fr_ਅਗਲਾ)
	    {
		ਜੇ (*rowp < fp->fr_ਉਚਾਈ)
		    ਰੋਕੋ;
		*rowp -= fp->fr_ਉਚਾਈ;
	    }
	}
    }
    // When using a timer that closes a window the window might not actually
    // exist.
    FOR_ALL_WINDOWS(wp)
	ਜੇ (wp == fp->fr_win)
	{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MENU
	    *rowp -= wp->w_winbar_ਉਚਾਈ;
#ਜੇ_ਅੰਤ
	    ਵਾਪਸ wp;
	}
    ਵਾਪਸ ਨਿਰਾ;
}

#ਜੇ defined(NEED_VCOL2_ਖਾਨਾ) || defined(FEAT_BEVAL) || defined(FEAT_PROP_POPUP) \
	|| defined(FEAT_EVAL)
/*
 * Convert a virtual (screen) column to a character column.
 * The first column is zero.
 */
    ਪੂਰਨ
vcol2col(win_T *wp, linenr_T lnum, ਪੂਰਨ vcol, colnr_T *coladdp)
{
    char_u	    *ਸਤਰ;
    chartabsize_T   cts;

    // try to advance to the specified column
    ਸਤਰ = ml_get_buf(wp->w_ਬਫਰ, lnum, FALSE);
    init_chartabsize_arg(&cts, wp, lnum, 0, ਸਤਰ, ਸਤਰ);
    ਜਦੋਂ (cts.cts_vcol < vcol && *cts.cts_ptr != NUL)
    {
	ਪੂਰਨ ਮਾਪ = win_lbr_chartabsize(&cts, ਨਿਰਾ, ਨਿਰਾ);
	ਜੇ (cts.cts_vcol + ਮਾਪ > vcol)
	    ਰੋਕੋ;
	cts.cts_vcol += ਮਾਪ;
	MB_PTR_ADV(cts.cts_ptr);
    }
    clear_chartabsize_arg(&cts);

    ਜੇ (coladdp != ਨਿਰਾ)
	*coladdp = vcol - cts.cts_vcol;
    ਵਾਪਸ (ਪੂਰਨ)(cts.cts_ptr - ਸਤਰ);
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_EVAL)
/*
 * "getmousepos()" function.
 */
    ਖਾਲੀ
f_getmousepos(typval_T *argvars ਅਣਵਰਤਿਆ, typval_T *rettv)
{
    dict_T	*d;
    win_T	*wp;
    ਪੂਰਨ		ਪੰਕਤੀ = mouse_ਪੰਕਤੀ;
    ਪੂਰਨ		ਖਾਨਾ = mouse_ਖਾਨਾ;
    varnumber_T winid = 0;
    varnumber_T winrow = 0;
    varnumber_T wincol = 0;
    linenr_T	lnum = 0;
    varnumber_T ਕਾਲਮ = 0;
    colnr_T	coladd = 0;

    ਜੇ (rettv_dict_alloc(rettv) == FAIL)
	ਵਾਪਸ;
    d = rettv->vval.v_dict;

    dict_ਜੋੜੋ_ਸੰਖਿਆ(d, "screenrow", (varnumber_T)mouse_ਪੰਕਤੀ + 1);
    dict_ਜੋੜੋ_ਸੰਖਿਆ(d, "screencol", (varnumber_T)mouse_ਖਾਨਾ + 1);

    wp = mouse_ਲੱਭੋ_win(&ਪੰਕਤੀ, &ਖਾਨਾ, ਲੱਭੋ_POPUP);
    ਜੇ (wp != ਨਿਰਾ)
    {
	ਪੂਰਨ	ਸਿਖਰ_off = 0;
	ਪੂਰਨ	ਖੱਬਾ_off = 0;
	ਪੂਰਨ	ਉਚਾਈ = wp->w_ਉਚਾਈ + wp->w_ਸਥਿਤੀ_ਉਚਾਈ;

# ifdef FEAT_PROP_POPUP
	ਜੇ (WIN_IS_POPUP(wp))
	{
	    ਸਿਖਰ_off = popup_ਸਿਖਰ_extra(wp);
	    ਖੱਬਾ_off = popup_ਖੱਬਾ_extra(wp);
	    ਉਚਾਈ = popup_ਉਚਾਈ(wp);
	}
# endif
	ਜੇ (ਪੰਕਤੀ < ਉਚਾਈ)
	{
	    winid = wp->w_id;
	    winrow = ਪੰਕਤੀ + 1;
	    wincol = ਖਾਨਾ + 1;
	    ਪੰਕਤੀ -= ਸਿਖਰ_off;
	    ਖਾਨਾ -= ਖੱਬਾ_off;
	    ਜੇ (ਪੰਕਤੀ >= 0 && ਪੰਕਤੀ < wp->w_ਉਚਾਈ && ਖਾਨਾ >= 0 && ਖਾਨਾ < wp->w_ਚੌੜਾਈ)
	    {
		(ਖਾਲੀ)mouse_comp_pos(wp, &ਪੰਕਤੀ, &ਖਾਨਾ, &lnum, ਨਿਰਾ);
		ਖਾਨਾ = vcol2col(wp, lnum, ਖਾਨਾ, &coladd);
		ਕਾਲਮ = ਖਾਨਾ + 1;
	    }
	}
    }
    dict_ਜੋੜੋ_ਸੰਖਿਆ(d, "winid", winid);
    dict_ਜੋੜੋ_ਸੰਖਿਆ(d, "winrow", winrow);
    dict_ਜੋੜੋ_ਸੰਖਿਆ(d, "wincol", wincol);
    dict_ਜੋੜੋ_ਸੰਖਿਆ(d, "line", (varnumber_T)lnum);
    dict_ਜੋੜੋ_ਸੰਖਿਆ(d, "column", ਕਾਲਮ);
    dict_ਜੋੜੋ_ਸੰਖਿਆ(d, "coladd", coladd);
}
#ਜੇ_ਅੰਤ
