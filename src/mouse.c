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
ಸ್ಥಿರ long mouse_hor_step = 6;
ಸ್ಥಿರ long mouse_vert_step = 3;
ಸ್ಥಿರ win_T *dragwin = ರಿಕ್ತ;	// window being dragged
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ stl_click_handler(win_T *wp, ಪೂರ್ಣಾಙ್ಕ mrow, ಪೂರ್ಣಾಙ್ಕ mcol, ಪೂರ್ಣಾಙ್ಕ which_button,
								    ಪೂರ್ಣಾಙ್ಕ mods);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ stl_click_handler_regions(stl_click_region_T *regions,
				    ಪೂರ್ಣಾಙ್ಕ region_ಎಣಿಕೆ, ಪೂರ್ಣಾಙ್ಕ winid,
				    char_u *area_name, ಪೂರ್ಣಾಙ್ಕ mrow, ಪೂರ್ಣಾಙ್ಕ mcol,
				    ಪೂರ್ಣಾಙ್ಕ which_button, ಪೂರ್ಣಾಙ್ಕ mods);

    ಶೂನ್ಯ
mouse_set_vert_scroll_step(long step)
{
    mouse_vert_step = step;
}

    ಶೂನ್ಯ
mouse_set_hor_scroll_step(long step)
{
    mouse_hor_step = step;
}

#ifdef CHECK_DOUBLE_CLICK
/*
 * Return the duration from t1 to t2 in milliseconds.
 */
    ಸ್ಥಿರ long
time_diff_ms(ಸಂರಚನಾ timeval *t1, ಸಂರಚನಾ timeval *t2)
{
    // This handles wrapping of tv_usec correctly without any special case.
    // Example of 2 pairs (tv_sec, tv_usec) with a duration of 5 ms:
    //	   t1 = (1, 998000) t2 = (2, 3000) gives:
    //	   (2 - 1) * 1000 + (3000 - 998000) / 1000 -> 5 ms.
    ಪ್ರತ್ಯಾಗಮ (t2->tv_sec - t1->tv_sec) * 1000
	 + (t2->tv_usec - t1->tv_usec) / 1000;
}
#endif

/*
 * Get class of a character for selection: same class means same word.
 * 0: blank
 * 1: punctuation groups
 * 2: normal word character
 * >2: multi-byte word character.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
get_mouse_class(char_u *p)
{
    ಪೂರ್ಣಾಙ್ಕ		c;

    ಯದಿ (has_mbyte && MB_BYTE2LEN(p[0]) > 1)
	ಪ್ರತ್ಯಾಗಮ mb_get_class(p);

    c = *p;
    ಯದಿ (c == ' ' || c == '\t')
	ಪ್ರತ್ಯಾಗಮ 0;

    ಯದಿ (vim_iswordc(c))
	ಪ್ರತ್ಯಾಗಮ 2;

    // There are a few special cases where we want certain combinations of
    // characters to be considered as a single word.  These are things like
    // "->", "/ *", "*=", "+=", "&=", "<=", ">=", "!=" etc.  Otherwise, each
    // character is in its own class.
    ಯದಿ (c != NUL && vim_strchr((char_u *)"-+*/%<>&|^!=", c) != ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ 1;
    ಪ್ರತ್ಯಾಗಮ c;
}

/*
 * Move "pos" back to the start of the word it's in.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
find_start_of_word(pos_T *pos)
{
    char_u	*ಸಾಲು;
    ಪೂರ್ಣಾಙ್ಕ		cclass;
    ಪೂರ್ಣಾಙ್ಕ		col;

    ಸಾಲು = ml_get(pos->lnum);
    cclass = get_mouse_class(ಸಾಲು + pos->col);

    ಯಾವತ್ (pos->col > 0)
    {
	col = pos->col - 1;
	col -= (*mb_head_off)(ಸಾಲು, ಸಾಲು + col);
	ಯದಿ (get_mouse_class(ಸಾಲು + col) != cclass)
	    ಭಙ್ಗ;
	pos->col = col;
    }
}

/*
 * Move "pos" forward to the end of the word it's in.
 * When 'selection' is "exclusive", the position is just after the word.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
find_end_of_word(pos_T *pos)
{
    char_u	*ಸಾಲು;
    ಪೂರ್ಣಾಙ್ಕ		cclass;
    ಪೂರ್ಣಾಙ್ಕ		col;

    ಸಾಲು = ml_get(pos->lnum);
    ಯದಿ (*p_sel == 'e' && pos->col > 0)
    {
	--pos->col;
	pos->col -= (*mb_head_off)(ಸಾಲು, ಸಾಲು + pos->col);
    }
    cclass = get_mouse_class(ಸಾಲು + pos->col);
    ಯಾವತ್ (ಸಾಲು[pos->col] != NUL)
    {
	col = pos->col + (*mb_ptr2len)(ಸಾಲು + pos->col);
	ಯದಿ (get_mouse_class(ಸಾಲು + col) != cclass)
	{
	    ಯದಿ (*p_sel == 'e')
		pos->col = col;
	    ಭಙ್ಗ;
	}
	pos->col = col;
    }
}

#ಯದಿ defined(FEAT_GUI_MOTIF) || defined(FEAT_GUI_GTK) \
	    || defined(FEAT_GUI_MSWIN) \
	    || defined(FEAT_GUI_PHOTON) \
	    || defined(FEAT_TERM_POPUP_MENU)
# define USE_POPUP_SETPOS
# define NEED_VCOL2COL

/*
 * Translate window coordinates to buffer position without any side effects.
 * Returns IN_BUFFER and sets "mpos->col" to the column when in buffer text.
 * The column is one for the first column.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
get_fpos_of_mouse(pos_T *mpos)
{
    win_T	*wp;
    ಪೂರ್ಣಾಙ್ಕ		row = mouse_row;
    ಪೂರ್ಣಾಙ್ಕ		col = mouse_col;

    ಯದಿ (row < 0 || col < 0)		// check if it makes sense
	ಪ್ರತ್ಯಾಗಮ IN_UNKNOWN;

    // find the window where the row is in
    wp = mouse_find_win(&row, &col, FAIL_POPUP);
    ಯದಿ (wp == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ IN_UNKNOWN;
    // winpos and height may change in win_enter()!
    ಯದಿ (row >= wp->w_height)	// In (or below) status line
	ಪ್ರತ್ಯಾಗಮ IN_STATUS_ಸಾಲು;
    ಯದಿ (col >= wp->w_width)	// In vertical separator line
	ಪ್ರತ್ಯಾಗಮ IN_SEP_ಸಾಲು;

    ಯದಿ (wp != curwin)
	ಪ್ರತ್ಯಾಗಮ IN_UNKNOWN;

    // compute the position in the buffer line from the posn on the screen
    ಯದಿ (mouse_comp_pos(curwin, &row, &col, &mpos->lnum, ರಿಕ್ತ))
	ಪ್ರತ್ಯಾಗಮ IN_STATUS_ಸಾಲು; // past bottom

    mpos->col = vcol2col(wp, mpos->lnum, col, &mpos->coladd);
    ಪ್ರತ್ಯಾಗಮ IN_BUFFER;
}
#endif

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
    ಪೂರ್ಣಾಙ್ಕ
do_mouse(
    oparg_T	*oap,		// operator argument, can be NULL
    ಪೂರ್ಣಾಙ್ಕ		c,		// K_LEFTMOUSE, etc
    ಪೂರ್ಣಾಙ್ಕ		dir,		// Direction to 'put' if necessary
    long	ಎಣಿಕೆ,
    ಪೂರ್ಣಾಙ್ಕ		fixindent)	// PUT_FIXINDENT if fixing indent necessary
{
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	do_always = FALSE;	// ignore 'mouse' setting next time
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	got_click = FALSE;	// got a click some time back

    ಪೂರ್ಣಾಙ್ಕ		which_button;	// MOUSE_LEFT, _MIDDLE or _RIGHT
    ಪೂರ್ಣಾಙ್ಕ		is_click = FALSE; // If FALSE it's a drag or release event
    ಪೂರ್ಣಾಙ್ಕ		is_drag = FALSE;  // If TRUE it's a drag event
    ಪೂರ್ಣಾಙ್ಕ		jump_flags = 0;	// flags for jump_to_mouse()
    pos_T	start_visual;
    ಪೂರ್ಣಾಙ್ಕ		moved;		// Has cursor moved?
    ಪೂರ್ಣಾಙ್ಕ		in_status_ಸಾಲು;	// mouse in status line
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	in_tab_ಸಾಲು = FALSE; // mouse clicked in tab line
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	in_tabpanel = FALSE; // mouse clicked in tabpanel
#ifdef FEAT_TABPANEL
    ಸ್ಥಿರ bool	in_tabpanel_scrollbar = false; // dragging tabpanel scrollbar
#endif
    ಪೂರ್ಣಾಙ್ಕ		in_sep_ಸಾಲು;	// mouse in vertical separator line
    ಪೂರ್ಣಾಙ್ಕ		c1, c2;
#ಯದಿ defined(FEAT_FOLDING)
    pos_T	save_cursor;
#endif
    win_T	*old_curwin = curwin;
    ಸ್ಥಿರ pos_T orig_cursor;
    colnr_T	leftcol, rightcol;
    pos_T	end_visual;
    ಪೂರ್ಣಾಙ್ಕ		diff;
    ಪೂರ್ಣಾಙ್ಕ		old_active = VIsual_active;
    ಪೂರ್ಣಾಙ್ಕ		old_mode = VIsual_mode;
    ಪೂರ್ಣಾಙ್ಕ		regname;

#ಯದಿ defined(FEAT_FOLDING)
    save_cursor = curwin->w_cursor;
#endif

    // When GUI is active, always recognize mouse events, otherwise:
    // - Ignore mouse event in normal mode if 'mouse' doesn't include 'n'.
    // - Ignore mouse event in visual mode if 'mouse' doesn't include 'v'.
    // - For command line and insert mode 'mouse' is checked before calling
    //	 do_mouse().
    ಯದಿ (do_always)
	do_always = FALSE;
    ಅನ್ಯಥಾ
#ifdef FEAT_GUI
	ಯದಿ (!gui.in_use)
#endif
	{
	    ಯದಿ (VIsual_active)
	    {
		ಯದಿ (!mouse_has(MOUSE_VISUAL))
		    ಪ್ರತ್ಯಾಗಮ FALSE;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (State == MODE_NORMAL && !mouse_has(MOUSE_NORMAL))
		ಪ್ರತ್ಯಾಗಮ FALSE;
	}

    ಪ್ರತಿ (;;)
    {
	which_button = get_mouse_button(KEY2TERMCAP1(c), &is_click, &is_drag);
	ಯದಿ (is_drag)
	{
	    // If the next character is the same mouse event then use that
	    // one. Speeds up dragging the status line.
	    // Note: Since characters added to the stuff buffer in the code
	    // below need to come before the next character, do not do this
	    // when the current character was stuffed.
	    ಯದಿ (!ಕೀಲಿ_Stuffed && vpeekc() != NUL)
	    {
		ಪೂರ್ಣಾಙ್ಕ nc;
		ಪೂರ್ಣಾಙ್ಕ save_mouse_row = mouse_row;
		ಪೂರ್ಣಾಙ್ಕ save_mouse_col = mouse_col;

		// Need to get the character, peeking doesn't get the actual
		// one.
		nc = safe_vgetc();
		ಯದಿ (c == nc)
		    ಅನುವರ್ತ;
		vungetc(nc);
		mouse_row = save_mouse_row;
		mouse_col = save_mouse_col;
	    }
	}
	ಭಙ್ಗ;
    }

    ಯದಿ (c == K_MOUSEMOVE)
    {
	// Mouse moved without a button pressed.
#ifdef FEAT_BEVAL_TERM
	ui_may_remove_balloon();
	ಯದಿ (p_bevalterm)
	{
	    profile_setlimit(p_bdlay, &bevalexpr_due);
	    bevalexpr_due_set = TRUE;
	}
#endif
#ifdef FEAT_PROP_POPUP
	popup_handle_mouse_moved();
#endif
	ಪ್ರತ್ಯಾಗಮ FALSE;
    }

#ifdef FEAT_MOUSESHAPE
    // May have stopped dragging the status or separator line.  The pointer is
    // most likely still on the status or separator line.
    ಯದಿ (!is_drag && drag_status_ಸಾಲು)
    {
	drag_status_ಸಾಲು = FALSE;
	update_mouseshape(SHAPE_IDX_STATUS);
    }
    ಯದಿ (!is_drag && drag_sep_ಸಾಲು)
    {
	drag_sep_ಸಾಲು = FALSE;
	update_mouseshape(SHAPE_IDX_VSEP);
    }
#endif

    // Ignore drag and release events if we didn't get a click.
    ಯದಿ (is_click)
    {
	got_click = TRUE;
	in_tab_ಸಾಲು = FALSE;
	in_tabpanel = FALSE;
#ifdef FEAT_TABPANEL
	in_tabpanel_scrollbar = false;
#endif
    }
    ಅನ್ಯಥಾ
    {
	ಯದಿ (!got_click)			// didn't get click, ignore
	    ಪ್ರತ್ಯಾಗಮ FALSE;
	ಯದಿ (!is_drag)			// release, reset got_click
	{
	    got_click = FALSE;
	    ಯದಿ (in_tab_ಸಾಲು || in_tabpanel
#ifdef FEAT_TABPANEL
		    || in_tabpanel_scrollbar
#endif
		    )
	    {
		in_tab_ಸಾಲು = FALSE;
		in_tabpanel = FALSE;
#ifdef FEAT_TABPANEL
		in_tabpanel_scrollbar = false;
#endif
		ಪ್ರತ್ಯಾಗಮ FALSE;
	    }
	}
    }

#ifdef FEAT_TABPANEL
    // Continue a scrollbar drag before any tab-selection handling.
    ಯದಿ (is_drag && in_tabpanel_scrollbar)
    {
	tabpanel_drag_scrollbar(mouse_row);
	ಪ್ರತ್ಯಾಗಮ FALSE;
    }
#endif

    // CTRL right mouse button does CTRL-T
    ಯದಿ (is_click && (mod_mask & MOD_MASK_CTRL) && which_button == MOUSE_ಬಲ)
    {
	ಯದಿ (State & MODE_ಸೇರಿಸು)
	    stuffcharReadbuff(Ctrl_O);
	ಯದಿ (ಎಣಿಕೆ > 1)
	    stuffnumReadbuff(ಎಣಿಕೆ);
	stuffcharReadbuff(Ctrl_T);
	got_click = FALSE;		// ignore drag&release now
	ಪ್ರತ್ಯಾಗಮ FALSE;
    }

    // CTRL only works with left mouse button
    ಯದಿ ((mod_mask & MOD_MASK_CTRL) && which_button != MOUSE_ಎಡ)
	ಪ್ರತ್ಯಾಗಮ FALSE;

    // When a modifier is down, ignore drag and release events, as well as
    // multiple clicks and the middle mouse button.
    // Accept shift-leftmouse drags when 'mousemodel' is "popup.*".
    ಯದಿ ((mod_mask & (MOD_MASK_SHIFT | MOD_MASK_CTRL | MOD_MASK_ALT
							     | MOD_MASK_META))
	    && (!is_click
		|| (mod_mask & MOD_MASK_MULTI_CLICK)
		|| which_button == MOUSE_ಮಧ್ಯ)
	    && !((mod_mask & (MOD_MASK_SHIFT|MOD_MASK_ALT))
		&& mouse_model_popup()
		&& which_button == MOUSE_ಎಡ)
	    && !((mod_mask & MOD_MASK_ALT)
		&& !mouse_model_popup()
		&& which_button == MOUSE_ಬಲ)
	    )
	ಪ್ರತ್ಯಾಗಮ FALSE;

    // If the button press was used as the movement command for an operator
    // (eg "d<MOUSE>"), or it is the middle button that is held down, ignore
    // drag/release events.
    ಯದಿ (!is_click && which_button == MOUSE_ಮಧ್ಯ)
	ಪ್ರತ್ಯಾಗಮ FALSE;

    ಯದಿ (oap != ರಿಕ್ತ)
	regname = oap->regname;
    ಅನ್ಯಥಾ
	regname = 0;

    // Middle mouse button does a 'put' of the selected text
    ಯದಿ (which_button == MOUSE_ಮಧ್ಯ)
    {
	ಯದಿ (State == MODE_NORMAL)
	{
	    // If an operator was pending, we don't know what the user wanted
	    // to do. Go back to normal mode: Clear the operator and beep().
	    ಯದಿ (oap != ರಿಕ್ತ && oap->op_type != OP_NOP)
	    {
		clearopbeep(oap);
		ಪ್ರತ್ಯಾಗಮ FALSE;
	    }

	    // If visual was active, yank the highlighted text and put it
	    // before the mouse pointer position.
	    // In Select mode replace the highlighted text with the clipboard.
	    ಯದಿ (VIsual_active)
	    {
		ಯದಿ (VIsual_select)
		{
		    stuffcharReadbuff(Ctrl_G);
		    stuffReadbuff((char_u *)"\"+p");
		}
		ಅನ್ಯಥಾ
		{
		    stuffcharReadbuff('y');
		    stuffcharReadbuff(K_MIDDLEMOUSE);
		}
		do_always = TRUE;	// ignore 'mouse' setting next time
		ಪ್ರತ್ಯಾಗಮ FALSE;
	    }
	    // The rest is below jump_to_mouse()
	}

	ಅನ್ಯಥಾ ಯದಿ ((State & MODE_ಸೇರಿಸು) == 0)
	    ಪ್ರತ್ಯಾಗಮ FALSE;

	// Middle click in insert mode doesn't move the mouse, just insert the
	// contents of a register.  '.' register is special, can't insert that
	// with do_put().
	// Also paste at the cursor if the current mode isn't in 'mouse' (only
	// happens for the GUI).
	ಯದಿ ((State & MODE_ಸೇರಿಸು) || !mouse_has(MOUSE_NORMAL))
	{
	    ಯದಿ (regname == '.')
		ಸೇರಿಸು_reg(regname, TRUE);
	    ಅನ್ಯಥಾ
	    {
#ifdef FEAT_CLIPBOARD
		ಯದಿ (clip_star.available && regname == 0)
		    regname = '*';
#endif
		ಯದಿ ((State & REPLACE_FLAG) && !yank_register_mline(regname))
		    ಸೇರಿಸು_reg(regname, TRUE);
		ಅನ್ಯಥಾ
		{
		    do_put(regname, ರಿಕ್ತ, BACKWARD, 1L,
						      fixindent | PUT_CURSEND);

		    // Repeat it with CTRL-R CTRL-O r or CTRL-R CTRL-P r
		    AppendCharToRedobuff(Ctrl_R);
		    AppendCharToRedobuff(fixindent ? Ctrl_P : Ctrl_O);
		    AppendCharToRedobuff(regname == 0 ? '"' : regname);
		}
	    }
	    ಪ್ರತ್ಯಾಗಮ FALSE;
	}
    }

    // When dragging or button-up stay in the same window.
    ಯದಿ (!is_click)
	jump_flags |= MOUSE_FOCUS | MOUSE_DID_MOVE;

    start_visual.lnum = 0;

    ಸಂರಚನಾ tabpage_label_info {
	bool is_panel;	    // label type. true: tabpanel, false: tab line
	bool just_in;	    // just in tabpage label area
	bool just_click;    // just click tabpage label area
	ಪೂರ್ಣಾಙ್ಕ nr;		    // tabpage number
    } tp_label = { false, false, false, 0 };

    // Check for clicking in the tab page panel.
#ಯದಿ defined(FEAT_TABPANEL)
    ಯದಿ (mouse_col < firstwin->w_wincol
		|| mouse_col >= firstwin->w_wincol + topframe->fr_width)
    {
	// A click on the scrollbar column starts a drag interaction and
	// preempts tab-selection.
	ಯದಿ (is_click && !is_drag && mouse_on_tabpanel_scrollbar())
	{
	    in_tabpanel_scrollbar = TRUE;
	    tabpanel_drag_scrollbar(mouse_row);
	    ಪ್ರತ್ಯಾಗಮ FALSE;
	}

	// Dispatch 'tabpanel' %[FuncName] click regions before falling through
	// to tab-page selection.  On drag events fall through to the normal
	// tab-drag handling.
	ಯದಿ (is_click && !is_drag
		&& stl_click_handler_regions(tabpanel_stl_click,
					    tabpanel_stl_click_ಎಣಿಕೆ,
					    0, (char_u *)"tabpanel",
					    mouse_row, mouse_col,
					    which_button, mod_mask))
	    ಪ್ರತ್ಯಾಗಮ FALSE;

	tp_label.is_panel = true;
	tp_label.just_in = true;
	tp_label.nr = get_tabpagenr_on_tabpanel();

	// click in a tab selects that tab page
	ಯದಿ (is_click && cmdwin_type == 0)
	    tp_label.just_click = true;
    }
    ಅನ್ಯಥಾ
#endif
    // Check for clicking in the tab page line.
    ಯದಿ (TabPageIdxs != ರಿಕ್ತ && mouse_row == 0 && firstwin->w_winrow > 0)
    {
	// Dispatch 'tabline' %[FuncName] click regions before falling through
	// to tab-page selection.  On drag events fall through to the normal
	// tab-drag handling.
	ಯದಿ (is_click && !is_drag
		&& stl_click_handler_regions(tabline_stl_click,
					    tabline_stl_click_ಎಣಿಕೆ,
					    0, (char_u *)"tabline",
					    mouse_row, mouse_col,
					    which_button, mod_mask))
	    ಪ್ರತ್ಯಾಗಮ FALSE;

	tp_label.just_in = true;
	tp_label.nr = TabPageIdxs[mouse_col];

	// click in a tab selects that tab page
	ಯದಿ (is_click && cmdwin_type == 0
		&& mouse_col < firstwin->w_wincol + topframe->fr_width)
	    tp_label.just_click = true;
    }

    ಯದಿ (tp_label.just_in)
    {
	ಯದಿ (is_drag)
	{
	    ಯದಿ (in_tabpanel || in_tab_ಸಾಲು)
	    {
		c1 = tp_label.nr;
		tabpage_move(c1 <= 0 ? 9999 : c1 < tabpage_index(curtab)
							    ? c1 - 1 : c1);
	    }
	    ಪ್ರತ್ಯಾಗಮ FALSE;
	}

	ಯದಿ (tp_label.just_click)
	{
	    ಯದಿ (tp_label.is_panel)
		in_tabpanel = TRUE;
	    ಅನ್ಯಥಾ
		in_tab_ಸಾಲು = TRUE;
	    c1 = tp_label.nr;
	    ಯದಿ (c1 >= 0)
	    {
		ಯದಿ ((mod_mask & MOD_MASK_MULTI_CLICK) == MOD_MASK_2CLICK
						    && !tp_label.is_panel)
		{
		    // Double-click on the tabline opens a new, empty tab page.
		    // The tabpanel has no "empty area" (every row maps to a tab)
		    // and this behavior is not documented for tabpanel, so fall
		    // through to the regular tab-switch path there.
		    end_visual_mode_keep_button();
		    tabpage_new();
		    tabpage_move(c1 == 0 ? 9999 : c1 - 1);
		}
		ಅನ್ಯಥಾ
		{
		    // Go to specified tab page, or next one if not clicking
		    // on a label.
		    goto_tabpage(c1);

		    // It's like clicking on the status line of a window.
		    ಯದಿ (curwin != old_curwin)
			end_visual_mode_keep_button();
		}
	    }
	    ಅನ್ಯಥಾ
	    {
		tabpage_T	*tp;

		// Close the current or specified tab page.
		ಯದಿ (c1 == -999)
		    tp = curtab;
		ಅನ್ಯಥಾ
		    tp = find_tabpage(-c1);
		ಯದಿ (tp == curtab)
		{
		    ಯದಿ (first_tabpage->tp_next != ರಿಕ್ತ)
			tabpage_close(FALSE);
		}
		ಅನ್ಯಥಾ ಯದಿ (tp != ರಿಕ್ತ)
		    tabpage_close_other(tp, FALSE);
	    }
	}
	ಪ್ರತ್ಯಾಗಮ TRUE;
    }
    ಅನ್ಯಥಾ ಯದಿ (is_drag && (in_tabpanel || (in_tab_ಸಾಲು && TabPageIdxs != ರಿಕ್ತ)))
    {
#ಯದಿ defined(FEAT_TABPANEL)
	ಯದಿ (in_tabpanel)
	    c1 = get_tabpagenr_on_tabpanel();
	ಅನ್ಯಥಾ
#endif
	    c1 = TabPageIdxs[mouse_col];
	tabpage_move(c1 <= 0 ? 9999 : c1 - 1);
	ಪ್ರತ್ಯಾಗಮ FALSE;
    }

    // When 'mousemodel' is "popup" or "popup_setpos", translate mouse events:
    // right button up   -> pop-up menu
    // shift-left button -> right button
    // alt-left button   -> alt-right button
    ಯದಿ (mouse_model_popup())
    {
	ಯದಿ (which_button == MOUSE_ಬಲ
			    && !(mod_mask & (MOD_MASK_SHIFT | MOD_MASK_CTRL)))
	{
#ifdef USE_POPUP_SETPOS
# ifdef FEAT_GUI
	    ಯದಿ (gui.in_use)
	    {
#  ಯದಿ defined(FEAT_GUI_MOTIF) || defined(FEAT_GUI_GTK) \
			  || defined(FEAT_GUI_PHOTON)
		ಯದಿ (!is_click)
		    // Ignore right button release events, only shows the popup
		    // menu on the button down event.
		    ಪ್ರತ್ಯಾಗಮ FALSE;
#  endif
#  ಯದಿ defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_HAIKU)
		ಯದಿ (is_click || is_drag)
		    // Ignore right button down and drag mouse events.  Windows
		    // only shows the popup menu on the button up event.
		    ಪ್ರತ್ಯಾಗಮ FALSE;
#  endif
	    }
# endif
# ಯದಿ defined(FEAT_GUI) && defined(FEAT_TERM_POPUP_MENU)
	    ಅನ್ಯಥಾ
# endif
# ಯದಿ defined(FEAT_TERM_POPUP_MENU)
	    ಯದಿ (!is_click)
		// Ignore right button release events, only shows the popup
		// menu on the button down event.
		ಪ್ರತ್ಯಾಗಮ FALSE;
# endif

	    jump_flags = 0;
	    ಯದಿ (STRCMP(p_mousem, "popup_setpos") == 0)
	    {
		// First set the cursor position before showing the popup
		// menu.
		ಯದಿ (VIsual_active)
		{
		    pos_T    m_pos;

		    // set MOUSE_MAY_STOP_VIS if we are outside the
		    // selection or the current window (might have false
		    // negative here)
		    ಯದಿ (mouse_row < curwin->w_winrow
			 || mouse_row
				  > (curwin->w_winrow + curwin->w_height))
			jump_flags = MOUSE_MAY_STOP_VIS;
		    ಅನ್ಯಥಾ ಯದಿ (get_fpos_of_mouse(&m_pos) != IN_BUFFER)
			jump_flags = MOUSE_MAY_STOP_VIS;
		    ಅನ್ಯಥಾ
		    {
			ಯದಿ (VIsual_mode == 'V')
			{
			    ಯದಿ ((curwin->w_cursor.lnum <= VIsual.lnum
				    && (m_pos.lnum < curwin->w_cursor.lnum
					|| VIsual.lnum < m_pos.lnum))
				|| (VIsual.lnum < curwin->w_cursor.lnum
				    && (m_pos.lnum < VIsual.lnum
					|| curwin->w_cursor.lnum < m_pos.lnum)))
			    {
				jump_flags = MOUSE_MAY_STOP_VIS;
			    }
			}
			ಅನ್ಯಥಾ ಯದಿ ((LTOREQ_POS(curwin->w_cursor, VIsual)
				    && (LT_POS(m_pos, curwin->w_cursor)
					|| LT_POS(VIsual, m_pos)))
				|| (LT_POS(VIsual, curwin->w_cursor)
				    && (LT_POS(m_pos, VIsual)
				      || LT_POS(curwin->w_cursor, m_pos))))
			{
			    jump_flags = MOUSE_MAY_STOP_VIS;
			}
			ಅನ್ಯಥಾ ಯದಿ (VIsual_mode == Ctrl_V)
			{
			    getvcols(curwin, &curwin->w_cursor, &VIsual,
						     &leftcol, &rightcol, 0);
			    getvcol(curwin, &m_pos, ರಿಕ್ತ, &m_pos.col, ರಿಕ್ತ, 0);
			    ಯದಿ (m_pos.col < leftcol || m_pos.col > rightcol)
				jump_flags = MOUSE_MAY_STOP_VIS;
			}
		    }
		}
		ಅನ್ಯಥಾ
		    jump_flags = MOUSE_MAY_STOP_VIS;
	    }
	    ಯದಿ (jump_flags)
	    {
		jump_flags = jump_to_mouse(jump_flags, ರಿಕ್ತ, which_button);
		update_curbuf(VIsual_active ? UPD_INVERTED : UPD_VALID);
		setcursor();
		out_flush();    // Update before showing popup menu
	    }
# ifdef FEAT_MENU
	    show_popupmenu();
	    got_click = FALSE;	// ignore release events
# endif
	    ಪ್ರತ್ಯಾಗಮ (jump_flags & CURSOR_MOVED) != 0;
#ಅನ್ಯಥಾ
	    ಪ್ರತ್ಯಾಗಮ FALSE;
#endif
	}
	ಯದಿ (which_button == MOUSE_ಎಡ
				&& (mod_mask & (MOD_MASK_SHIFT|MOD_MASK_ALT)))
	{
	    which_button = MOUSE_ಬಲ;
	    mod_mask &= ~MOD_MASK_SHIFT;
	}
    }

    ಯದಿ ((State & (MODE_NORMAL | MODE_ಸೇರಿಸು))
			    && !(mod_mask & (MOD_MASK_SHIFT | MOD_MASK_CTRL)))
    {
	ಯದಿ (which_button == MOUSE_ಎಡ)
	{
	    ಯದಿ (is_click)
	    {
		// stop Visual mode for a left click in a window, but not when
		// on a status line
		ಯದಿ (VIsual_active)
		    jump_flags |= MOUSE_MAY_STOP_VIS;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (mouse_has(MOUSE_VISUAL))
		jump_flags |= MOUSE_MAY_VIS;
	}
	ಅನ್ಯಥಾ ಯದಿ (which_button == MOUSE_ಬಲ)
	{
	    ಯದಿ (is_click && VIsual_active)
	    {
		// Remember the start and end of visual before moving the
		// cursor.
		ಯದಿ (LT_POS(curwin->w_cursor, VIsual))
		{
		    start_visual = curwin->w_cursor;
		    end_visual = VIsual;
		}
		ಅನ್ಯಥಾ
		{
		    start_visual = VIsual;
		    end_visual = curwin->w_cursor;
		}
	    }
	    jump_flags |= MOUSE_FOCUS;
	    ಯದಿ (mouse_has(MOUSE_VISUAL))
		jump_flags |= MOUSE_MAY_VIS;
	}
    }

    // If an operator is pending, ignore all drags and releases until the
    // next mouse click.
    ಯದಿ (!is_drag && oap != ರಿಕ್ತ && oap->op_type != OP_NOP)
    {
	got_click = FALSE;
	oap->motion_type = MCHAR;
    }

    // When releasing the button let jump_to_mouse() know.
    ಯದಿ (!is_click && !is_drag)
	jump_flags |= MOUSE_RELEASED;

    // JUMP!
    jump_flags = jump_to_mouse(jump_flags,
			oap == ರಿಕ್ತ ? ರಿಕ್ತ : &(oap->inclusive), which_button);

#ifdef FEAT_MENU
    // A click in the window toolbar has no side effects.
    ಯದಿ (jump_flags & MOUSE_WINBAR)
	ಪ್ರತ್ಯಾಗಮ FALSE;
#endif
    moved = (jump_flags & CURSOR_MOVED);
    in_status_ಸಾಲು = (jump_flags & IN_STATUS_ಸಾಲು);
    in_sep_ಸಾಲು = (jump_flags & IN_SEP_ಸಾಲು);

    // Check for statusline click handler early, before visual mode or
    // other button-specific handling can interfere.
    ಯದಿ (in_status_ಸಾಲು && is_click && !is_drag
	    && stl_click_handler(dragwin, mouse_row, mouse_col,
						which_button, mod_mask))
    {
#ifdef FEAT_MOUSESHAPE
	ಯದಿ (!drag_status_ಸಾಲು)
	{
	    drag_status_ಸಾಲು = TRUE;
	    update_mouseshape(-1);
	}
#endif
	ಪ್ರತ್ಯಾಗಮ FALSE;
    }

#ifdef FEAT_NETBEANS_INTG
    ಯದಿ (isNetbeansBuffer(curbuf)
			    && !(jump_flags & (IN_STATUS_ಸಾಲು | IN_SEP_ಸಾಲು)))
    {
	ಪೂರ್ಣಾಙ್ಕ ಕೀಲಿ = KEY2TERMCAP1(c);

	ಯದಿ (ಕೀಲಿ == (ಪೂರ್ಣಾಙ್ಕ)KE_LEFTRELEASE || ಕೀಲಿ == (ಪೂರ್ಣಾಙ್ಕ)KE_MIDDLERELEASE
					       || ಕೀಲಿ == (ಪೂರ್ಣಾಙ್ಕ)KE_RIGHTRELEASE)
	    netbeans_button_release(which_button);
    }
#endif

    // When jumping to another window, clear a pending operator.  That's a bit
    // friendlier than beeping and not jumping to that window.
    ಯದಿ (curwin != old_curwin && oap != ರಿಕ್ತ && oap->op_type != OP_NOP)
	clearop(oap);

#ifdef FEAT_FOLDING
    ಯದಿ (mod_mask == 0
	    && !is_drag
	    && (jump_flags & (MOUSE_FOLD_CLOSE | MOUSE_FOLD_OPEN))
	    && which_button == MOUSE_ಎಡ)
    {
	// open or close a fold at this line
	ಯದಿ (jump_flags & MOUSE_FOLD_OPEN)
	    openFold(curwin->w_cursor.lnum, 1L);
	ಅನ್ಯಥಾ
	    closeFold(curwin->w_cursor.lnum, 1L);
	// don't move the cursor if still in the same window
	ಯದಿ (curwin == old_curwin)
	    curwin->w_cursor = save_cursor;
    }
#endif

#ಯದಿ defined(FEAT_CLIPBOARD)
    ಯದಿ ((jump_flags & IN_OTHER_WIN) && !VIsual_active &&
	    (clip_star.available || clip_plus.available))
    {
	clip_modeless(which_button, is_click, is_drag);
	ಪ್ರತ್ಯಾಗಮ FALSE;
    }
#endif

    // Set global flag that we are extending the Visual area with mouse
    // dragging; temporarily minimize 'scrolloff'.
    ಯದಿ (VIsual_active && is_drag && get_scrolloff_ಮೌಲ್ಯ())
    {
	// In the very first line, allow scrolling one line
	ಯದಿ (mouse_row == 0)
	    mouse_dragging = 2;
	ಅನ್ಯಥಾ
	    mouse_dragging = 1;
    }

    // When dragging the mouse above the window, scroll down.
    ಯದಿ (is_drag && mouse_row < 0 && !in_status_ಸಾಲು)
    {
	scroll_redraw(FALSE, 1L);
	mouse_row = 0;
    }

    ಯದಿ (start_visual.lnum)		// right click in visual mode
    {
       // When ALT is pressed make Visual mode blockwise.
       ಯದಿ (mod_mask & MOD_MASK_ALT)
	   VIsual_mode = Ctrl_V;

	// In Visual-block mode, divide the area in four, pick up the corner
	// that is in the quarter that the cursor is in.
	ಯದಿ (VIsual_mode == Ctrl_V)
	{
	    getvcols(curwin, &start_visual, &end_visual,
						    &leftcol, &rightcol, 0);
	    ಯದಿ (curwin->w_curswant > (leftcol + rightcol) / 2)
		end_visual.col = leftcol;
	    ಅನ್ಯಥಾ
		end_visual.col = rightcol;
	    ಯದಿ (curwin->w_cursor.lnum >=
				    (start_visual.lnum + end_visual.lnum) / 2)
		end_visual.lnum = start_visual.lnum;

	    // move VIsual to the right column
	    start_visual = curwin->w_cursor;	    // save the cursor pos
	    curwin->w_cursor = end_visual;
	    coladvance(end_visual.col);
	    VIsual = curwin->w_cursor;
	    curwin->w_cursor = start_visual;	    // restore the cursor
	}
	ಅನ್ಯಥಾ
	{
	    // If the click is before the start of visual, change the start.
	    // If the click is after the end of visual, change the end.  If
	    // the click is inside the visual, change the closest side.
	    ಯದಿ (LT_POS(curwin->w_cursor, start_visual))
		VIsual = end_visual;
	    ಅನ್ಯಥಾ ಯದಿ (LT_POS(end_visual, curwin->w_cursor))
		VIsual = start_visual;
	    ಅನ್ಯಥಾ
	    {
		// In the same line, compare column number
		ಯದಿ (end_visual.lnum == start_visual.lnum)
		{
		    ಯದಿ (curwin->w_cursor.col - start_visual.col >
				    end_visual.col - curwin->w_cursor.col)
			VIsual = start_visual;
		    ಅನ್ಯಥಾ
			VIsual = end_visual;
		}

		// In different lines, compare line number
		ಅನ್ಯಥಾ
		{
		    diff = (curwin->w_cursor.lnum - start_visual.lnum) -
				(end_visual.lnum - curwin->w_cursor.lnum);

		    ಯದಿ (diff > 0)		// closest to end
			VIsual = start_visual;
		    ಅನ್ಯಥಾ ಯದಿ (diff < 0)	// closest to start
			VIsual = end_visual;
		    ಅನ್ಯಥಾ			// in the middle line
		    {
			ಯದಿ (curwin->w_cursor.col <
					(start_visual.col + end_visual.col) / 2)
			    VIsual = end_visual;
			ಅನ್ಯಥಾ
			    VIsual = start_visual;
		    }
		}
	    }
	}
    }
    // If Visual mode started in insert mode, execute "CTRL-O"
    ಅನ್ಯಥಾ ಯದಿ ((State & MODE_ಸೇರಿಸು) && VIsual_active)
	stuffcharReadbuff(Ctrl_O);

    // Middle mouse click: Put text before cursor.
    ಯದಿ (which_button == MOUSE_ಮಧ್ಯ)
    {
#ifdef FEAT_CLIPBOARD
	ಯದಿ (clip_star.available && regname == 0)
	    regname = '*';
#endif
	ಯದಿ (yank_register_mline(regname))
	{
	    ಯದಿ (mouse_past_bottom)
		dir = FORWARD;
	}
	ಅನ್ಯಥಾ ಯದಿ (mouse_past_eol)
	    dir = FORWARD;

	ಯದಿ (fixindent)
	{
	    c1 = (dir == BACKWARD) ? '[' : ']';
	    c2 = 'p';
	}
	ಅನ್ಯಥಾ
	{
	    c1 = (dir == FORWARD) ? 'p' : 'P';
	    c2 = NUL;
	}
	prep_redo(regname, ಎಣಿಕೆ, NUL, c1, NUL, c2, NUL);

	// Remember where the paste started, so in edit() Insstart can be set
	// to this position
	ಯದಿ (restart_edit != 0)
	    where_paste_started = curwin->w_cursor;
	do_put(regname, ರಿಕ್ತ, dir, ಎಣಿಕೆ, fixindent | PUT_CURSEND);
    }

#ಯದಿ defined(FEAT_QUICKFIX)
    // Ctrl-Mouse click or double click in a quickfix window jumps to the
    // error under the mouse pointer.
    ಅನ್ಯಥಾ ಯದಿ (((mod_mask & MOD_MASK_CTRL)
		|| (mod_mask & MOD_MASK_MULTI_CLICK) == MOD_MASK_2CLICK)
	    && bt_quickfix(curbuf))
    {
	ಯದಿ (curwin->w_llist_ref == ರಿಕ್ತ)	// quickfix window
	    do_cmdline_cmd((char_u *)".cc");
	ಅನ್ಯಥಾ					// location list window
	    do_cmdline_cmd((char_u *)".ll");
	got_click = FALSE;		// ignore drag&release now
    }
#endif

    // Ctrl-Mouse click (or double click in a help window) jumps to the tag
    // under the mouse pointer.
    ಅನ್ಯಥಾ ಯದಿ ((mod_mask & MOD_MASK_CTRL) || (curbuf->b_help
		     && (mod_mask & MOD_MASK_MULTI_CLICK) == MOD_MASK_2CLICK))
    {
	ಯದಿ (State & MODE_ಸೇರಿಸು)
	    stuffcharReadbuff(Ctrl_O);
	stuffcharReadbuff(Ctrl_RSB);
	got_click = FALSE;		// ignore drag&release now
    }

    // Shift-Mouse click searches for the next occurrence of the word under
    // the mouse pointer
    ಅನ್ಯಥಾ ಯದಿ ((mod_mask & MOD_MASK_SHIFT))
    {
	ಯದಿ ((State & MODE_ಸೇರಿಸು) || (VIsual_active && VIsual_select))
	    stuffcharReadbuff(Ctrl_O);
	ಯದಿ (which_button == MOUSE_ಎಡ)
	    stuffcharReadbuff('*');
	ಅನ್ಯಥಾ	// MOUSE_RIGHT
	    stuffcharReadbuff('#');
    }

    // Handle double clicks, unless on status line
    ಅನ್ಯಥಾ ಯದಿ (in_status_ಸಾಲು)
    {
#ifdef FEAT_MOUSESHAPE
	ಯದಿ ((is_drag || is_click) && !drag_status_ಸಾಲು)
	{
	    drag_status_ಸಾಲು = TRUE;
	    update_mouseshape(-1);
	}
#endif
    }
    ಅನ್ಯಥಾ ಯದಿ (in_sep_ಸಾಲು)
    {
#ifdef FEAT_MOUSESHAPE
	ಯದಿ ((is_drag || is_click) && !drag_sep_ಸಾಲು)
	{
	    drag_sep_ಸಾಲು = TRUE;
	    update_mouseshape(-1);
	}
#endif
    }
    ಅನ್ಯಥಾ ಯದಿ ((mod_mask & MOD_MASK_MULTI_CLICK)
				       && (State & (MODE_NORMAL | MODE_ಸೇರಿಸು))
	     && mouse_has(MOUSE_VISUAL))
    {
	ಯದಿ (is_click || !VIsual_active)
	{
	    ಯದಿ (VIsual_active)
		orig_cursor = VIsual;
	    ಅನ್ಯಥಾ
	    {
		check_visual_highlight();
		VIsual = curwin->w_cursor;
		orig_cursor = VIsual;
		VIsual_active = TRUE;
		VIsual_reselect = TRUE;
		// start Select mode if 'selectmode' contains "mouse"
		may_start_select('o');
		setmouse();
	    }
	    ಯದಿ ((mod_mask & MOD_MASK_MULTI_CLICK) == MOD_MASK_2CLICK)
	    {
		// Double click with ALT pressed makes it blockwise.
		ಯದಿ (mod_mask & MOD_MASK_ALT)
		    VIsual_mode = Ctrl_V;
		ಅನ್ಯಥಾ
		    VIsual_mode = 'v';
	    }
	    ಅನ್ಯಥಾ ಯದಿ ((mod_mask & MOD_MASK_MULTI_CLICK) == MOD_MASK_3CLICK)
		VIsual_mode = 'V';
	    ಅನ್ಯಥಾ ಯದಿ ((mod_mask & MOD_MASK_MULTI_CLICK) == MOD_MASK_4CLICK)
		VIsual_mode = Ctrl_V;
#ifdef FEAT_CLIPBOARD
	    // Make sure the clipboard gets updated.  Needed because start and
	    // end may still be the same, and the selection needs to be owned
	    clip_star.vmode = NUL;
#endif
	}
	// A double click selects a word or a block.
	ಯದಿ ((mod_mask & MOD_MASK_MULTI_CLICK) == MOD_MASK_2CLICK)
	{
	    pos_T	*pos = ರಿಕ್ತ;
	    ಪೂರ್ಣಾಙ್ಕ		gc;

	    ಯದಿ (is_click)
	    {
		// If the character under the cursor (skipping white space) is
		// not a word character, try finding a match and select a (),
		// {}, [], #if/#endif, etc. block.
		end_visual = curwin->w_cursor;
		ಯಾವತ್ (gc = gchar_pos(&end_visual), VIM_ISWHITE(gc))
		    inc(&end_visual);
		ಯದಿ (oap != ರಿಕ್ತ)
		    oap->motion_type = MCHAR;
		ಯದಿ (oap != ರಿಕ್ತ
			&& VIsual_mode == 'v'
			&& !vim_iswordc(gchar_pos(&end_visual))
			&& EQUAL_POS(curwin->w_cursor, VIsual)
			&& (pos = findmatch(oap, NUL)) != ರಿಕ್ತ)
		{
		    curwin->w_cursor = *pos;
		    ಯದಿ (oap->motion_type == MLINE)
			VIsual_mode = 'V';
		    ಅನ್ಯಥಾ ಯದಿ (*p_sel == 'e')
		    {
			ಯದಿ (LT_POS(curwin->w_cursor, VIsual))
			    ++VIsual.col;
			ಅನ್ಯಥಾ
			    ++curwin->w_cursor.col;
		    }
		}
	    }

	    ಯದಿ (pos == ರಿಕ್ತ && (is_click || is_drag))
	    {
		// When not found a match or when dragging: extend to include
		// a word.
		ಯದಿ (LT_POS(curwin->w_cursor, orig_cursor))
		{
		    find_start_of_word(&curwin->w_cursor);
		    find_end_of_word(&VIsual);
		}
		ಅನ್ಯಥಾ
		{
		    find_start_of_word(&VIsual);
		    ಯದಿ (*p_sel == 'e' && *ml_get_cursor() != NUL)
			curwin->w_cursor.col +=
					 (*mb_ptr2len)(ml_get_cursor());
		    find_end_of_word(&curwin->w_cursor);
		}
	    }
	    curwin->w_set_curswant = true;
	}
	ಯದಿ (is_click)
	    redraw_curbuf_later(UPD_INVERTED);	// update the inversion
    }
    ಅನ್ಯಥಾ ಯದಿ (VIsual_active && !old_active)
    {
	ಯದಿ (mod_mask & MOD_MASK_ALT)
	    VIsual_mode = Ctrl_V;
	ಅನ್ಯಥಾ
	    VIsual_mode = 'v';
    }

    // If Visual mode changed show it later.
    ಯದಿ ((!VIsual_active && old_active && mode_displayed)
	    || (VIsual_active && p_smd && ಸಂದೇಶ_silent == 0
				 && (!old_active || VIsual_mode != old_mode)))
	redraw_cmdline = TRUE;

    ಪ್ರತ್ಯಾಗಮ moved;
}

    ಶೂನ್ಯ
ins_mouse(ಪೂರ್ಣಾಙ್ಕ c)
{
    pos_T	tpos;
    win_T	*old_curwin = curwin;

#ifdef FEAT_GUI
    // When GUI is active, also move/paste when 'mouse' is empty
    ಯದಿ (!gui.in_use)
#endif
	ಯದಿ (!mouse_has(MOUSE_ಸೇರಿಸು))
	    ಪ್ರತ್ಯಾಗಮ;

    undisplay_dollar();
    tpos = curwin->w_cursor;
    ಯದಿ (do_mouse(ರಿಕ್ತ, c, BACKWARD, 1L, 0))
    {
	win_T	*new_curwin = curwin;

	ಯದಿ (curwin != old_curwin && win_valid(old_curwin))
	{
	    // Mouse took us to another window.  We need to go back to the
	    // previous one to stop insert there properly.
	    curwin = old_curwin;
	    curbuf = curwin->w_buffer;
#ifdef FEAT_JOB_CHANNEL
	    ಯದಿ (bt_prompt(curbuf))
		// Restart Insert mode when re-entering the prompt buffer.
		curbuf->b_prompt_ಸೇರಿಸು = 'A';
#endif
	}
	start_arrow(curwin == old_curwin ? &tpos : ರಿಕ್ತ);
	ಯದಿ (curwin != new_curwin && win_valid(new_curwin))
	{
	    curwin = new_curwin;
	    curbuf = curwin->w_buffer;
	}
	set_can_cindent(TRUE);
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
    ಪೂರ್ಣಾಙ್ಕ shift_or_ctrl = mod_mask & (MOD_MASK_SHIFT | MOD_MASK_CTRL);

#ifdef FEAT_TERMINAL
    ಯದಿ (term_use_loop())
	// This window is a terminal window, send the mouse event there.
	// Set "typed" to FALSE to avoid an endless loop.
	send_ಕೀಲಿಗಳು_to_term(curbuf->b_term, cap->cmdchar, mod_mask, FALSE);
    ಅನ್ಯಥಾ
#endif
    ಯದಿ (cap->arg == MSCR_UP || cap->arg == MSCR_DOWN)
    {
	// Vertical scrolling
	ಯದಿ (!(State & MODE_ಸೇರಿಸು) && (mouse_vert_step < 0 || shift_or_ctrl))
	{
	    // whole page up or down
	    pagescroll(cap->arg == MSCR_UP ? FORWARD : BACKWARD, 1L, FALSE);
	}
	ಅನ್ಯಥಾ
	{
	    ಯದಿ (mouse_vert_step < 0 || shift_or_ctrl)
	    {
		// whole page up or down
		cap->count1 = (long)(curwin->w_botline - curwin->w_topline);
	    }
	    // Don't scroll more than half the window height.
	    ಅನ್ಯಥಾ ಯದಿ (curwin->w_height < mouse_vert_step * 2)
	    {
		cap->count1 = curwin->w_height / 2;
		ಯದಿ (cap->count1 == 0)
		    cap->count1 = 1;
	    }
	    ಅನ್ಯಥಾ
	    {
		cap->count1 = mouse_vert_step;
	    }
	    cap->count0 = cap->count1;
	    nv_scroll_ಸಾಲು(cap);
	}

#ifdef FEAT_PROP_POPUP
	ಯದಿ (WIN_IS_POPUP(curwin))
	    popup_set_firstline(curwin);
#endif
    }
    ಅನ್ಯಥಾ
    {
	// Horizontal scrolling
	long step = (mouse_hor_step < 0 || shift_or_ctrl)
					    ? curwin->w_width : mouse_hor_step;
	long leftcol = curwin->w_leftcol
				     + (cap->arg == MSCR_ಬಲ ? -step : step);
	ಯದಿ (leftcol < 0)
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
ins_mousescroll(ಪೂರ್ಣಾಙ್ಕ dir)
{
    cmdarg_T	cap;
    oparg_T	oa;
    CLEAR_FIELD(cap);
    clear_oparg(&oa);
    cap.oap = &oa;
    cap.arg = dir;

#ifdef FEAT_TABPANEL
    ಯದಿ (mouse_row >= 0 && mouse_col >= 0
	    && (dir == MSCR_UP || dir == MSCR_DOWN)
	    && mouse_on_tabpanel())
    {
	(ಶೂನ್ಯ)tabpanel_scroll(dir == MSCR_UP ? 1 : -1,
		mouse_vert_step > 0 ? mouse_vert_step : 3);
	ಪ್ರತ್ಯಾಗಮ;
    }
#endif

    ವಿಕಲ್ಪ (dir)
    {
	ಪಕ್ಷ MSCR_UP:
	    cap.cmdchar = K_MOUSEUP;
	    ಭಙ್ಗ;
	ಪಕ್ಷ MSCR_DOWN:
	    cap.cmdchar = K_MOUSEDOWN;
	    ಭಙ್ಗ;
	ಪಕ್ಷ MSCR_ಎಡ:
	    cap.cmdchar = K_MOUSELEFT;
	    ಭಙ್ಗ;
	ಪಕ್ಷ MSCR_ಬಲ:
	    cap.cmdchar = K_MOUSERIGHT;
	    ಭಙ್ಗ;
	ಸ್ವಭಾವ:
	    siemsg("Invalid ins_mousescroll() argument: %d", dir);
    }

    win_T *old_curwin = curwin;
    ಯದಿ (mouse_row >= 0 && mouse_col >= 0)
    {
	// Find the window at the mouse pointer coordinates.
	// NOTE: Must restore "curwin" to "old_curwin" before returning!
	ಪೂರ್ಣಾಙ್ಕ row = mouse_row;
	ಪೂರ್ಣಾಙ್ಕ col = mouse_col;
	curwin = mouse_find_win(&row, &col, FIND_POPUP);
	ಯದಿ (curwin == ರಿಕ್ತ)
	{
	    curwin = old_curwin;
	    ಪ್ರತ್ಯಾಗಮ;
	}
	curbuf = curwin->w_buffer;
    }

    ಯದಿ (curwin == old_curwin)
    {
	// Don't scroll the current window if the popup menu is visible.
	ಯದಿ (pum_visible())
	    ಪ್ರತ್ಯಾಗಮ;

	undisplay_dollar();
    }

    linenr_T	orig_topline = curwin->w_topline;
    colnr_T	orig_leftcol = curwin->w_leftcol;
    pos_T	orig_cursor = curwin->w_cursor;

    // Call the common mouse scroll function shared with other modes.
    do_mousescroll(&cap);

    ಪೂರ್ಣಾಙ್ಕ did_scroll = (orig_topline != curwin->w_topline
		   || orig_leftcol != curwin->w_leftcol);

    curwin->w_redr_status = true;
    curwin = old_curwin;
    curbuf = curwin->w_buffer;

    // If the window actually scrolled and the popup menu may overlay the
    // window, need to redraw it.
    ಯದಿ (did_scroll && pum_visible())
    {
	// TODO: Would be more efficient to only redraw the windows that are
	// overlapped by the popup menu.
	redraw_all_later(UPD_NOT_VALID);
	ins_compl_show_pum();
    }

    ಯದಿ (!EQUAL_POS(curwin->w_cursor, orig_cursor))
    {
	start_arrow(&orig_cursor);
	set_can_cindent(TRUE);
    }
}

#ಯದಿ defined(FEAT_PROP_POPUP)
/*
 * Command-line mode implementation for scrolling in direction "dir", which is
 * one of the MSCR_ values.  Scrolls the completion info popup when the mouse
 * pointer is on top of it.
 * Returns TRUE when the info popup was scrolled.
 */
    ಪೂರ್ಣಾಙ್ಕ
cmdline_mousescroll(ಪೂರ್ಣಾಙ್ಕ dir)
{
    cmdarg_T	cap;
    oparg_T	oa;

    CLEAR_FIELD(cap);
    clear_oparg(&oa);
    cap.oap = &oa;
    cap.arg = dir;

    ವಿಕಲ್ಪ (dir)
    {
	ಪಕ್ಷ MSCR_UP:	 cap.cmdchar = K_MOUSEUP; ಭಙ್ಗ;
	ಪಕ್ಷ MSCR_DOWN:	 cap.cmdchar = K_MOUSEDOWN; ಭಙ್ಗ;
	ಪಕ್ಷ MSCR_ಎಡ:	 cap.cmdchar = K_MOUSELEFT; ಭಙ್ಗ;
	ಪಕ್ಷ MSCR_ಬಲ: cap.cmdchar = K_MOUSERIGHT; ಭಙ್ಗ;
    }

    ಯದಿ (mouse_row < 0 || mouse_col < 0)
	ಪ್ರತ್ಯಾಗಮ FALSE;

    ಪೂರ್ಣಾಙ್ಕ	    row = mouse_row;
    ಪೂರ್ಣಾಙ್ಕ	    col = mouse_col;
    win_T   *wp;

    // Only scroll when the mouse is on top of the info popup.
    wp = mouse_find_win(&row, &col, FIND_POPUP);
    ಯದಿ (wp == ರಿಕ್ತ || !WIN_IS_POPUP(wp) || !(wp->w_popup_flags & POPF_INFO)
							|| !wp->w_has_scrollbar)
	ಪ್ರತ್ಯಾಗಮ FALSE;

    win_T   *old_curwin = curwin;

    curwin = wp;
    curbuf = wp->w_buffer;
    // Call the common mouse scroll function shared with other modes.
    do_mousescroll(&cap);
    curwin = old_curwin;
    curbuf = curwin->w_buffer;

    // Cmdline mode doesn't normally call update_screen(), so redraw the
    // completion popup menu, which also repaints the info popup.
    ಯದಿ (cmdline_pum_active())
	cmdline_pum_display();
    ಪ್ರತ್ಯಾಗಮ TRUE;
}
#endif

/*
 * Return TRUE if "c" is a mouse key.
 */
    ಪೂರ್ಣಾಙ್ಕ
is_mouse_ಕೀಲಿ(ಪೂರ್ಣಾಙ್ಕ c)
{
    ಪ್ರತ್ಯಾಗಮ c == K_LEFTMOUSE
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

ಸ್ಥಿರ ಸಂರಚನಾ mousetable
{
    ಪೂರ್ಣಾಙ್ಕ	    pseudo_code;	// Code for pseudo mouse event
    ಪೂರ್ಣಾಙ್ಕ	    button;		// Which mouse button is it?
    ಪೂರ್ಣಾಙ್ಕ	    is_click;		// Is it a mouse button click event?
    ಪೂರ್ಣಾಙ್ಕ	    is_drag;		// Is it a mouse drag event?
} mouse_table[] =
{
    {(ಪೂರ್ಣಾಙ್ಕ)KE_LEFTMOUSE,		MOUSE_ಎಡ,	TRUE,	FALSE},
#ifdef FEAT_GUI
    {(ಪೂರ್ಣಾಙ್ಕ)KE_LEFTMOUSE_NM,	MOUSE_ಎಡ,	TRUE,	FALSE},
#endif
    {(ಪೂರ್ಣಾಙ್ಕ)KE_LEFTDRAG,		MOUSE_ಎಡ,	FALSE,	TRUE},
    {(ಪೂರ್ಣಾಙ್ಕ)KE_LEFTRELEASE,	MOUSE_ಎಡ,	FALSE,	FALSE},
#ifdef FEAT_GUI
    {(ಪೂರ್ಣಾಙ್ಕ)KE_LEFTRELEASE_NM,	MOUSE_ಎಡ,	FALSE,	FALSE},
#endif
    {(ಪೂರ್ಣಾಙ್ಕ)KE_MIDDLEMOUSE,	MOUSE_ಮಧ್ಯ,	TRUE,	FALSE},
    {(ಪೂರ್ಣಾಙ್ಕ)KE_MIDDLEDRAG,	MOUSE_ಮಧ್ಯ,	FALSE,	TRUE},
    {(ಪೂರ್ಣಾಙ್ಕ)KE_MIDDLERELEASE,	MOUSE_ಮಧ್ಯ,	FALSE,	FALSE},
    {(ಪೂರ್ಣಾಙ್ಕ)KE_RIGHTMOUSE,	MOUSE_ಬಲ,	TRUE,	FALSE},
    {(ಪೂರ್ಣಾಙ್ಕ)KE_RIGHTDRAG,		MOUSE_ಬಲ,	FALSE,	TRUE},
    {(ಪೂರ್ಣಾಙ್ಕ)KE_RIGHTRELEASE,	MOUSE_ಬಲ,	FALSE,	FALSE},
    {(ಪೂರ್ಣಾಙ್ಕ)KE_X1MOUSE,		MOUSE_X1,	TRUE,	FALSE},
    {(ಪೂರ್ಣಾಙ್ಕ)KE_X1DRAG,		MOUSE_X1,	FALSE,	TRUE},
    {(ಪೂರ್ಣಾಙ್ಕ)KE_X1RELEASE,		MOUSE_X1,	FALSE,	FALSE},
    {(ಪೂರ್ಣಾಙ್ಕ)KE_X2MOUSE,		MOUSE_X2,	TRUE,	FALSE},
    {(ಪೂರ್ಣಾಙ್ಕ)KE_X2DRAG,		MOUSE_X2,	FALSE,	TRUE},
    {(ಪೂರ್ಣಾಙ್ಕ)KE_X2RELEASE,		MOUSE_X2,	FALSE,	FALSE},
    // DRAG without CLICK
    {(ಪೂರ್ಣಾಙ್ಕ)KE_MOUSEMOVE,		MOUSE_RELEASE,	FALSE,	TRUE},
    // RELEASE without CLICK
    {(ಪೂರ್ಣಾಙ್ಕ)KE_IGNORE,		MOUSE_RELEASE,	FALSE,	FALSE},
    {0,				0,		0,	0},
};

/*
 * Look up the given mouse code to return the relevant information in the other
 * arguments.  Return which button is down or was released.
 */
    ಪೂರ್ಣಾಙ್ಕ
get_mouse_button(ಪೂರ್ಣಾಙ್ಕ code, ಪೂರ್ಣಾಙ್ಕ *is_click, ಪೂರ್ಣಾಙ್ಕ *is_drag)
{
    ಪೂರ್ಣಾಙ್ಕ	    i;

    ಪ್ರತಿ (i = 0; mouse_table[i].pseudo_code; i++)
	ಯದಿ (code == mouse_table[i].pseudo_code)
	{
	    *is_click = mouse_table[i].is_click;
	    *is_drag = mouse_table[i].is_drag;
	    ಪ್ರತ್ಯಾಗಮ mouse_table[i].button;
	}
    ಪ್ರತ್ಯಾಗಮ 0;	    // Shouldn't get here
}

/*
 * Return the appropriate pseudo mouse event token (KE_LEFTMOUSE etc) based on
 * the given information about which mouse button is down, and whether the
 * mouse was clicked, dragged or released.
 */
    ಪೂರ್ಣಾಙ್ಕ
get_pseudo_mouse_code(
    ಪೂರ್ಣಾಙ್ಕ	    button,	// eg MOUSE_LEFT
    ಪೂರ್ಣಾಙ್ಕ	    is_click,
    ಪೂರ್ಣಾಙ್ಕ	    is_drag)
{
    ಪೂರ್ಣಾಙ್ಕ	    i;

    ಪ್ರತಿ (i = 0; mouse_table[i].pseudo_code; i++)
	ಯದಿ (button == mouse_table[i].button
	    && is_click == mouse_table[i].is_click
	    && is_drag == mouse_table[i].is_drag)
	{
#ifdef FEAT_GUI
	    // Trick: a non mappable left click and release has mouse_col -1
	    // or added MOUSE_COLOFF.  Used for 'mousefocus' in
	    // gui_mouse_moved()
	    ಯದಿ (mouse_col < 0 || mouse_col > MOUSE_COLOFF)
	    {
		ಯದಿ (mouse_col < 0)
		    mouse_col = 0;
		ಅನ್ಯಥಾ
		    mouse_col -= MOUSE_COLOFF;
		ಯದಿ (mouse_table[i].pseudo_code == (ಪೂರ್ಣಾಙ್ಕ)KE_LEFTMOUSE)
		    ಪ್ರತ್ಯಾಗಮ (ಪೂರ್ಣಾಙ್ಕ)KE_LEFTMOUSE_NM;
		ಯದಿ (mouse_table[i].pseudo_code == (ಪೂರ್ಣಾಙ್ಕ)KE_LEFTRELEASE)
		    ಪ್ರತ್ಯಾಗಮ (ಪೂರ್ಣಾಙ್ಕ)KE_LEFTRELEASE_NM;
	    }
#endif
	    ಪ್ರತ್ಯಾಗಮ mouse_table[i].pseudo_code;
	}
    ಪ್ರತ್ಯಾಗಮ (ಪೂರ್ಣಾಙ್ಕ)KE_IGNORE;	    // not recognized, ignore it
}

#define HMT_NORMAL	1
#define HMT_NETTERM	2
#define HMT_DEC	4
#define HMT_JSBTERM	8
#define HMT_PTERM	16
#define HMT_URXVT	32
#define HMT_GPM	64
#define HMT_SGR	128
#define HMT_SGR_REL	256
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ has_mouse_termcode = 0;

    ಶೂನ್ಯ
set_mouse_termcode(
    ಪೂರ್ಣಾಙ್ಕ		n,	// KS_MOUSE, KS_NETTERM_MOUSE or KS_DEC_MOUSE
    char_u	*s)
{
    char_u	name[2];

    name[0] = n;
    name[1] = KE_FILLER;
    add_termcode(name, s, FALSE);
#ifdef FEAT_MOUSE_JSB
    ಯದಿ (n == KS_JSBTERM_MOUSE)
	has_mouse_termcode |= HMT_JSBTERM;
    ಅನ್ಯಥಾ
#endif
#ifdef FEAT_MOUSE_NET
    ಯದಿ (n == KS_NETTERM_MOUSE)
	has_mouse_termcode |= HMT_NETTERM;
    ಅನ್ಯಥಾ
#endif
#ifdef FEAT_MOUSE_DEC
    ಯದಿ (n == KS_DEC_MOUSE)
	has_mouse_termcode |= HMT_DEC;
    ಅನ್ಯಥಾ
#endif
#ifdef FEAT_MOUSE_PTERM
    ಯದಿ (n == KS_PTERM_MOUSE)
	has_mouse_termcode |= HMT_PTERM;
    ಅನ್ಯಥಾ
#endif
#ifdef FEAT_MOUSE_URXVT
    ಯದಿ (n == KS_URXVT_MOUSE)
	has_mouse_termcode |= HMT_URXVT;
    ಅನ್ಯಥಾ
#endif
#ifdef FEAT_MOUSE_GPM
    ಯದಿ (n == KS_GPM_MOUSE)
	has_mouse_termcode |= HMT_GPM;
    ಅನ್ಯಥಾ
#endif
    ಯದಿ (n == KS_SGR_MOUSE)
	has_mouse_termcode |= HMT_SGR;
    ಅನ್ಯಥಾ ಯದಿ (n == KS_SGR_MOUSE_RELEASE)
	has_mouse_termcode |= HMT_SGR_REL;
    ಅನ್ಯಥಾ
	has_mouse_termcode |= HMT_NORMAL;
}

#ಯದಿ defined(UNIX) || defined(VMS)
    ಶೂನ್ಯ
del_mouse_termcode(
    ಪೂರ್ಣಾಙ್ಕ		n)	// KS_MOUSE, KS_NETTERM_MOUSE or KS_DEC_MOUSE
{
    char_u	name[2];

    name[0] = n;
    name[1] = KE_FILLER;
    del_termcode(name);
# ifdef FEAT_MOUSE_JSB
    ಯದಿ (n == KS_JSBTERM_MOUSE)
	has_mouse_termcode &= ~HMT_JSBTERM;
    ಅನ್ಯಥಾ
# endif
# ifdef FEAT_MOUSE_NET
    ಯದಿ (n == KS_NETTERM_MOUSE)
	has_mouse_termcode &= ~HMT_NETTERM;
    ಅನ್ಯಥಾ
# endif
# ifdef FEAT_MOUSE_DEC
    ಯದಿ (n == KS_DEC_MOUSE)
	has_mouse_termcode &= ~HMT_DEC;
    ಅನ್ಯಥಾ
# endif
# ifdef FEAT_MOUSE_PTERM
    ಯದಿ (n == KS_PTERM_MOUSE)
	has_mouse_termcode &= ~HMT_PTERM;
    ಅನ್ಯಥಾ
# endif
# ifdef FEAT_MOUSE_URXVT
    ಯದಿ (n == KS_URXVT_MOUSE)
	has_mouse_termcode &= ~HMT_URXVT;
    ಅನ್ಯಥಾ
# endif
# ifdef FEAT_MOUSE_GPM
    ಯದಿ (n == KS_GPM_MOUSE)
	has_mouse_termcode &= ~HMT_GPM;
    ಅನ್ಯಥಾ
# endif
    ಯದಿ (n == KS_SGR_MOUSE)
	has_mouse_termcode &= ~HMT_SGR;
    ಅನ್ಯಥಾ ಯದಿ (n == KS_SGR_MOUSE_RELEASE)
	has_mouse_termcode &= ~HMT_SGR_REL;
    ಅನ್ಯಥಾ
	has_mouse_termcode &= ~HMT_NORMAL;
}
#endif

/*
 * setmouse() - switch mouse on/off depending on current mode and 'mouse'
 */
    ಶೂನ್ಯ
setmouse(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ	    checkfor;

#ifdef FEAT_MOUSESHAPE
    update_mouseshape(-1);
#endif

    // Should be outside proc, but may break MOUSESHAPE
#ifdef FEAT_GUI
    // In the GUI the mouse is always enabled.
    ಯದಿ (gui.in_use)
	ಪ್ರತ್ಯಾಗಮ;
#endif
    // be quick when mouse is off
    ಯದಿ (*p_mouse == NUL || has_mouse_termcode == 0)
	ಪ್ರತ್ಯಾಗಮ;

    // don't switch mouse on when not in raw mode (Ex mode)
    ಯದಿ (cur_tmode != TMODE_RAW)
    {
	mch_setmouse(FALSE);
	ಪ್ರತ್ಯಾಗಮ;
    }

    ಯದಿ (VIsual_active)
	checkfor = MOUSE_VISUAL;
    ಅನ್ಯಥಾ ಯದಿ (State == MODE_HITRETURN || State == MODE_ASKMORE
						     || State == MODE_SETWSIZE)
	checkfor = MOUSE_RETURN;
    ಅನ್ಯಥಾ ಯದಿ (State & MODE_ಸೇರಿಸು)
	checkfor = MOUSE_ಸೇರಿಸು;
    ಅನ್ಯಥಾ ಯದಿ (State & MODE_CMDLINE)
	checkfor = MOUSE_COMMAND;
    ಅನ್ಯಥಾ ಯದಿ (State == MODE_CONFIRM || State == MODE_EXTERNCMD)
	checkfor = ' '; // don't use mouse for ":confirm" or ":!cmd"
    ಅನ್ಯಥಾ
	checkfor = MOUSE_NORMAL;    // assume normal mode

    ಯದಿ (mouse_has(checkfor))
	mch_setmouse(TRUE);
    ಅನ್ಯಥಾ
	mch_setmouse(FALSE);
}

/*
 * Return TRUE if
 * - "c" is in 'mouse', or
 * - 'a' is in 'mouse' and "c" is in MOUSE_A, or
 * - the current buffer is a help file and 'h' is in 'mouse' and we are in a
 *   normal editing mode (not at hit-return message).
 */
    ಪೂರ್ಣಾಙ್ಕ
mouse_has(ಪೂರ್ಣಾಙ್ಕ c)
{
    char_u	*p;

    ಪ್ರತಿ (p = p_mouse; *p; ++p)
	ವಿಕಲ್ಪ (*p)
	{
	    ಪಕ್ಷ 'a': ಯದಿ (vim_strchr((char_u *)MOUSE_A, c) != ರಿಕ್ತ)
			  ಪ್ರತ್ಯಾಗಮ TRUE;
		      ಭಙ್ಗ;
	    ಪಕ್ಷ MOUSE_HELP: ಯದಿ (c != MOUSE_RETURN && curbuf->b_help)
				 ಪ್ರತ್ಯಾಗಮ TRUE;
			     ಭಙ್ಗ;
	    ಸ್ವಭಾವ: ಯದಿ (c == *p) ಪ್ರತ್ಯಾಗಮ TRUE; ಭಙ್ಗ;
	}
    ಪ್ರತ್ಯಾಗಮ FALSE;
}

/*
 * Return TRUE when 'mousemodel' is set to "popup" or "popup_setpos".
 */
    ಪೂರ್ಣಾಙ್ಕ
mouse_model_popup(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ (p_mousem[0] == 'p');
}

/*
 * Call a click-region callback function.
 * "regions"/"region_count" describe the resolved click regions,
 * "winid" is stored as the "winid" key in the info dict (0 for tabline).
 * Returns TRUE if the function was called and handled the click.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
stl_click_handler_regions(
	stl_click_region_T  *regions,
	ಪೂರ್ಣಾಙ್ಕ		    region_ಎಣಿಕೆ,
	ಪೂರ್ಣಾಙ್ಕ		    winid,
	char_u		    *area_name,
	ಪೂರ್ಣಾಙ್ಕ		    mrow,
	ಪೂರ್ಣಾಙ್ಕ		    mcol,
	ಪೂರ್ಣಾಙ್ಕ		    which_button,
	ಪೂರ್ಣಾಙ್ಕ		    mods)
{
#ifdef FEAT_EVAL
    ಪೂರ್ಣಾಙ್ಕ		n;
    ಪೂರ್ಣಾಙ್ಕ		nclicks;
    char_u	button_str[2];
    char_u	mods_str[4];
    ಪೂರ್ಣಾಙ್ಕ		mi = 0;
    dict_T	*info;
    typval_T	argvars[2];
    typval_T	rettv;
    funcexe_T	funcexe;
    ಪೂರ್ಣಾಙ್ಕ		col = mcol;

    ಯದಿ (regions == ರಿಕ್ತ || region_ಎಣಿಕೆ == 0)
	ಪ್ರತ್ಯಾಗಮ FALSE;

    // Find the click region at the given row and column.
    ಪ್ರತಿ (n = 0; n < region_ಎಣಿಕೆ; n++)
    {
	ಯದಿ (regions[n].row == mrow
		&& col >= regions[n].col_start
		&& col < regions[n].col_end)
	    ಭಙ್ಗ;
    }
    ಯದಿ (n >= region_ಎಣಿಕೆ || regions[n].funcname == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FALSE;

    // Build the info dictionary.
    info = dict_alloc();
    ಯದಿ (info == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FALSE;

    dict_add_ಸಂಖ್ಯೆ(info, "minwid", regions[n].minwid);

    // Determine number of clicks.
    // MOD_MASK_2CLICK=0x20, MOD_MASK_3CLICK=0x40, MOD_MASK_4CLICK=0x60
    nclicks = ((mods & MOD_MASK_MULTI_CLICK) >> 5) + 1;
    ಯದಿ (nclicks > 3)
	nclicks = 3;
    dict_add_ಸಂಖ್ಯೆ(info, "nclicks", nclicks);

    // Button.
    ಯದಿ (which_button == MOUSE_ಎಡ)
	button_str[0] = 'l';
    ಅನ್ಯಥಾ ಯದಿ (which_button == MOUSE_ಬಲ)
	button_str[0] = 'r';
    ಅನ್ಯಥಾ
	button_str[0] = 'm';
    button_str[1] = NUL;
    dict_add_string_len(info, "button", button_str, 1);

    // Modifiers.
    ಯದಿ (mods & MOD_MASK_SHIFT)
	mods_str[mi++] = 's';
    ಯದಿ (mods & MOD_MASK_CTRL)
	mods_str[mi++] = 'c';
    ಯದಿ (mods & MOD_MASK_ALT)
	mods_str[mi++] = 'a';
    mods_str[mi] = NUL;
    dict_add_string_len(info, "mods", mods_str, mi);

    dict_add_ಸಂಖ್ಯೆ(info, "winid", winid);

    // "area": which option the clicked region belongs to.  Lets a shared
    // dispatcher distinguish 'statusline', 'tabline' and 'tabpanel' without
    // having to overload winid == 0.
    dict_add_string(info, "area", area_name);

    // Expose tab page number for 'tabpanel' regions.
    ಯದಿ (regions[n].tabnr > 0)
	dict_add_ಸಂಖ್ಯೆ(info, "tabnr", regions[n].tabnr);

    // Call the function with the info dict as argument.
    argvars[0].v_type = VAR_DICT;
    argvars[0].vval.v_dict = info;
    ++info->dv_refcount;
    argvars[1].v_type = VAR_UNKNOWN;

    rettv.v_type = VAR_ಸಂಖ್ಯೆ;
    rettv.vval.v_ಸಂಖ್ಯೆ = 0;

    CLEAR_FIELD(funcexe);
    funcexe.fe_evaluate = TRUE;
    (ಶೂನ್ಯ)call_func(regions[n].funcname, -1,
					    &rettv, 1, argvars, &funcexe);

    n = (ಪೂರ್ಣಾಙ್ಕ)rettv.vval.v_ಸಂಖ್ಯೆ;
    clear_tv(&rettv);
    dict_unref(info);

    ಯದಿ (n != 0)
    {
	// Make sure the tabline gets redrawn too when the callback asks for
	// a redraw (redraw_statuslines() only redraws the tabline when
	// redraw_tabline is set).  For tabpanel the whole screen needs to be
	// refreshed.
	ಯದಿ (winid == 0)
	    redraw_tabline = TRUE;
# ifdef FEAT_TABPANEL
	ಯದಿ (STRCMP(area_name, "tabpanel") == 0)
	    redraw_all_later(UPD_NOT_VALID);
# endif
	redraw_statuslines();
    }

    ಪ್ರತ್ಯಾಗಮ TRUE;
#ಅನ್ಯಥಾ
    (ಶೂನ್ಯ)regions;
    (ಶೂನ್ಯ)region_ಎಣಿಕೆ;
    (ಶೂನ್ಯ)winid;
    (ಶೂನ್ಯ)area_name;
    (ಶೂನ್ಯ)mrow;
    (ಶೂನ್ಯ)mcol;
    (ಶೂನ್ಯ)which_button;
    (ಶೂನ್ಯ)mods;
    ಪ್ರತ್ಯಾಗಮ FALSE;
#endif
}

/*
 * Call a statusline click handler function for window "wp".
 * Returns TRUE if the function was called and handled the click.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
stl_click_handler(win_T *wp, ಪೂರ್ಣಾಙ್ಕ mrow, ಪೂರ್ಣಾಙ್ಕ mcol, ಪೂರ್ಣಾಙ್ಕ which_button, ಪೂರ್ಣಾಙ್ಕ mods)
{
    ಯದಿ (wp == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FALSE;
    ಪ್ರತ್ಯಾಗಮ stl_click_handler_regions(wp->w_stl_click, wp->w_stl_click_ಎಣಿಕೆ,
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
    dragwin = ರಿಕ್ತ;
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
    ಪೂರ್ಣಾಙ್ಕ
jump_to_mouse(
    ಪೂರ್ಣಾಙ್ಕ		flags,
    ಪೂರ್ಣಾಙ್ಕ		*inclusive,	// used for inclusive operator, can be NULL
    ಪೂರ್ಣಾಙ್ಕ		which_button)	// MOUSE_LEFT, MOUSE_RIGHT, MOUSE_MIDDLE
{
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	on_status_ಸಾಲು = 0;	// #lines below bottom of window
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	on_sep_ಸಾಲು = 0;	// on separator right of window
#ifdef FEAT_MENU
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ  in_winbar = FALSE;
#endif
#ifdef FEAT_PROP_POPUP
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ   in_popup_win = FALSE;
    ಸ್ಥಿರ win_T *click_in_popup_win = ರಿಕ್ತ;
#endif
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	prev_row = -1;
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	prev_col = -1;
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	did_drag = FALSE;	// drag was noticed

    win_T	*wp, *old_curwin;
    pos_T	old_cursor;
    ಪೂರ್ಣಾಙ್ಕ		ಎಣಿಕೆ;
    ಪೂರ್ಣಾಙ್ಕ		first;
    ಪೂರ್ಣಾಙ್ಕ		row = mouse_row;
    ಪೂರ್ಣಾಙ್ಕ		col = mouse_col;
    colnr_T	col_from_screen = -1;
#ifdef FEAT_FOLDING
    ಪೂರ್ಣಾಙ್ಕ		mouse_char = ' ';
#endif

    mouse_past_bottom = FALSE;
    mouse_past_eol = FALSE;

    ಯದಿ (flags & MOUSE_RELEASED)
    {
	// On button release we may change window focus if positioned on a
	// status line and no dragging happened.
	ಯದಿ (dragwin != ರಿಕ್ತ && !did_drag)
	    flags &= ~(MOUSE_FOCUS | MOUSE_DID_MOVE);
	dragwin = ರಿಕ್ತ;
	did_drag = FALSE;
#ifdef FEAT_PROP_POPUP
	ಯದಿ (click_in_popup_win != ರಿಕ್ತ && popup_dragwin == ರಿಕ್ತ)
	    popup_close_for_mouse_click(click_in_popup_win);

	popup_dragwin = ರಿಕ್ತ;
	click_in_popup_win = ರಿಕ್ತ;
#endif
    }

    ಯದಿ ((flags & MOUSE_DID_MOVE)
	    && prev_row == mouse_row
	    && prev_col == mouse_col)
    {
retnomove:
	// before moving the cursor for a left click which is NOT in a status
	// line, stop Visual mode
	ಯದಿ (on_status_ಸಾಲು)
	    ಪ್ರತ್ಯಾಗಮ IN_STATUS_ಸಾಲು;
	ಯದಿ (on_sep_ಸಾಲು)
	    ಪ್ರತ್ಯಾಗಮ IN_SEP_ಸಾಲು;
#ifdef FEAT_MENU
	ಯದಿ (in_winbar)
	{
	    // A quick second click may arrive as a double-click, but we use it
	    // as a second click in the WinBar.
	    ಯದಿ ((mod_mask & MOD_MASK_MULTI_CLICK) && !(flags & MOUSE_RELEASED))
	    {
		wp = mouse_find_win(&row, &col, FAIL_POPUP);
		ಯದಿ (wp == ರಿಕ್ತ)
		    ಪ್ರತ್ಯಾಗಮ IN_UNKNOWN;
		winbar_click(wp, col);
	    }
	    ಪ್ರತ್ಯಾಗಮ IN_OTHER_WIN | MOUSE_WINBAR;
	}
#endif
	ಯದಿ (flags & MOUSE_MAY_STOP_VIS)
	{
	    end_visual_mode_keep_button();
	    redraw_curbuf_later(UPD_INVERTED);	// delete the inversion
	}
#ಯದಿ defined(FEAT_CLIPBOARD)
	// Continue a modeless selection in another window.
	ಯದಿ (cmdwin_type != 0 && row < cmdwin_win->w_winrow)
	    ಪ್ರತ್ಯಾಗಮ IN_OTHER_WIN;
#endif
#ifdef FEAT_PROP_POPUP
	// Continue a modeless selection in a popup window or dragging it.
	ಯದಿ (in_popup_win)
	{
	    click_in_popup_win = ರಿಕ್ತ;  // don't close it on release
	    ಯದಿ (popup_dragwin != ರಿಕ್ತ)
	    {
		// dragging a popup window
		popup_drag(popup_dragwin);
		ಪ್ರತ್ಯಾಗಮ IN_UNKNOWN;
	    }
	    ಪ್ರತ್ಯಾಗಮ IN_OTHER_WIN;
	}
#endif
	ಪ್ರತ್ಯಾಗಮ IN_BUFFER;
    }

    prev_row = mouse_row;
    prev_col = mouse_col;

    ಯದಿ (flags & MOUSE_SETPOS)
	ಗಚ್ಛ retnomove;				// ugly goto...

    old_curwin = curwin;
    old_cursor = curwin->w_cursor;

    ಯದಿ (!(flags & MOUSE_FOCUS))
    {
	ಯದಿ (row < 0 || col < 0) // check if it makes sense
	    ಪ್ರತ್ಯಾಗಮ IN_UNKNOWN;

	// find the window where the row is in and adjust "row" and "col" to be
	// relative to top-left of the window
	wp = mouse_find_win(&row, &col, FIND_POPUP);
	ಯದಿ (wp == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ IN_UNKNOWN;
	dragwin = ರಿಕ್ತ;

#ifdef FEAT_PROP_POPUP
	// Click in a popup window may start dragging or modeless selection,
	// but not much else.
	ಯದಿ (WIN_IS_POPUP(wp))
	{
	    on_sep_ಸಾಲು = 0;
	    on_status_ಸಾಲು = 0;
	    in_popup_win = TRUE;
	    ಯದಿ (which_button == MOUSE_ಎಡ && popup_close_if_on_X(wp, row, col))
	    {
		ಪ್ರತ್ಯಾಗಮ IN_UNKNOWN;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (((wp->w_popup_flags & (POPF_DRAG | POPF_RESIZE))
					      && popup_on_border(wp, row, col))
				       || (wp->w_popup_flags & POPF_DRAGALL))
	    {
		popup_dragwin = wp;
		popup_start_drag(wp, row, col);
		ಪ್ರತ್ಯಾಗಮ IN_UNKNOWN;
	    }
	    // Only close on release, otherwise it's not possible to drag or do
	    // modeless selection.
	    ಅನ್ಯಥಾ ಯದಿ (wp->w_popup_close == POPCLOSE_CLICK
		    && which_button == MOUSE_ಎಡ)
	    {
		click_in_popup_win = wp;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (which_button == MOUSE_ಎಡ)
		// If the click is in the scrollbar, may scroll up/down.
		popup_handle_scrollbar_click(wp, row, col);
# ifdef FEAT_CLIPBOARD
	    ಪ್ರತ್ಯಾಗಮ IN_OTHER_WIN;
# ಅನ್ಯಥಾ
	    ಪ್ರತ್ಯಾಗಮ IN_UNKNOWN;
# endif
	}
	in_popup_win = FALSE;
	popup_dragwin = ರಿಕ್ತ;
#endif
#ifdef FEAT_MENU
	ಯದಿ (row == -1)
	{
	    // A click in the window toolbar does not enter another window or
	    // change Visual highlighting.
	    winbar_click(wp, col);
	    in_winbar = TRUE;
	    ಪ್ರತ್ಯಾಗಮ IN_OTHER_WIN | MOUSE_WINBAR;
	}
	in_winbar = FALSE;
#endif

	// winpos and height may change in win_enter()!
	ಯದಿ (row >= wp->w_height)		// In (or below) status line
	{
	    on_status_ಸಾಲು = row - wp->w_height + 1;
	    dragwin = wp;
	}
	ಅನ್ಯಥಾ
	    on_status_ಸಾಲು = 0;
	ಯದಿ (col >= wp->w_width)		// In separator line
	{
	    on_sep_ಸಾಲು = col - wp->w_width + 1;
	    dragwin = wp;
	}
	ಅನ್ಯಥಾ
	    on_sep_ಸಾಲು = 0;

	// The rightmost character of the status line might be a vertical
	// separator character if there is no connecting window to the right.
	ಯದಿ (on_status_ಸಾಲು && on_sep_ಸಾಲು)
	{
	    ಯದಿ (stl_connected(wp))
		on_sep_ಸಾಲು = 0;
	    ಅನ್ಯಥಾ
		on_status_ಸಾಲು = 0;
	}

	// Before jumping to another buffer, or moving the cursor for a left
	// click, stop Visual mode.
	ಯದಿ (VIsual_active
		&& (wp->w_buffer != curwin->w_buffer
		    || (!on_status_ಸಾಲು && !on_sep_ಸಾಲು
#ifdef FEAT_FOLDING
			&& (
# ifdef FEAT_RIGHTLEFT
			    wp->w_p_rl ? col < wp->w_width - wp->w_p_fdc :
# endif
			    col >= wp->w_p_fdc + (wp != cmdwin_win ? 0 : 1)
			    )
#endif
			&& (flags & MOUSE_MAY_STOP_VIS))))
	{
	    end_visual_mode_keep_button();
	    redraw_curbuf_later(UPD_INVERTED);	// delete the inversion
	}
	ಯದಿ (cmdwin_type != 0 && wp != cmdwin_win)
	{
	    // A click outside the command-line window: Use modeless
	    // selection if possible.  Allow dragging the status lines.
	    on_sep_ಸಾಲು = 0;
#ifdef FEAT_CLIPBOARD
	    ಯದಿ (on_status_ಸಾಲು)
		ಪ್ರತ್ಯಾಗಮ IN_STATUS_ಸಾಲು;
	    ಪ್ರತ್ಯಾಗಮ IN_OTHER_WIN;
#ಅನ್ಯಥಾ
	    row = 0;
	    col += wp->w_wincol;
	    wp = cmdwin_win;
#endif
	}
#ಯದಿ defined(FEAT_PROP_POPUP) && defined(FEAT_TERMINAL)
	ಯದಿ (popup_is_popup(curwin) && curbuf->b_term != ರಿಕ್ತ)
	    // terminal in popup window: don't jump to another window
	    ಪ್ರತ್ಯಾಗಮ IN_OTHER_WIN;
#endif
	// Only change window focus when not clicking on or dragging the
	// status line.  Do change focus when releasing the mouse button
	// (MOUSE_FOCUS was set above if we dragged first).
	ಯದಿ (dragwin == ರಿಕ್ತ || (flags & MOUSE_RELEASED))
	    win_enter(wp, TRUE);		// can make wp invalid!

	ಯದಿ (curwin != old_curwin)
	{
#ifdef CHECK_DOUBLE_CLICK
	    // set topline, to be able to check for double click ourselves
	    set_mouse_topline(curwin);
#endif
#ifdef FEAT_TERMINAL
	    // when entering a terminal window may change state
	    term_win_entered();
#endif
	}
	ಯದಿ (on_status_ಸಾಲು)			// In (or below) status line
	{
	    // Don't use start_arrow() if we're in the same window
	    ಯದಿ (curwin == old_curwin)
		ಪ್ರತ್ಯಾಗಮ IN_STATUS_ಸಾಲು;
	    ಅನ್ಯಥಾ
		ಪ್ರತ್ಯಾಗಮ IN_STATUS_ಸಾಲು | CURSOR_MOVED;
	}
	ಯದಿ (on_sep_ಸಾಲು)			// In (or below) status line
	{
	    // Don't use start_arrow() if we're in the same window
	    ಯದಿ (curwin == old_curwin)
		ಪ್ರತ್ಯಾಗಮ IN_SEP_ಸಾಲು;
	    ಅನ್ಯಥಾ
		ಪ್ರತ್ಯಾಗಮ IN_SEP_ಸಾಲು | CURSOR_MOVED;
	}

	curwin->w_cursor.lnum = curwin->w_topline;
#ifdef FEAT_GUI
	// remember topline, needed for double click
	gui_prev_topline = curwin->w_topline;
# ifdef FEAT_DIFF
	gui_prev_topfill = curwin->w_topfill;
# endif
#endif
    }
    ಅನ್ಯಥಾ ಯದಿ (on_status_ಸಾಲು && which_button == MOUSE_ಎಡ)
    {
	ಯದಿ (dragwin != ರಿಕ್ತ)
	{
	    // Drag the status line
	    ಎಣಿಕೆ = row - W_WINROW(dragwin) - dragwin->w_height + 1
							     - on_status_ಸಾಲು;
	    win_drag_status_ಸಾಲು(dragwin, ಎಣಿಕೆ);
	    did_drag |= ಎಣಿಕೆ;
	}
	ಪ್ರತ್ಯಾಗಮ IN_STATUS_ಸಾಲು;			// Cursor didn't move
    }
    ಅನ್ಯಥಾ ಯದಿ (on_sep_ಸಾಲು && which_button == MOUSE_ಎಡ)
    {
	ಯದಿ (dragwin != ರಿಕ್ತ)
	{
	    // Drag the separator column
	    ಎಣಿಕೆ = col - dragwin->w_wincol - dragwin->w_width + 1
								- on_sep_ಸಾಲು;
	    win_drag_vsep_ಸಾಲು(dragwin, ಎಣಿಕೆ);
	    did_drag |= ಎಣಿಕೆ;
	}
	ಪ್ರತ್ಯಾಗಮ IN_SEP_ಸಾಲು;			// Cursor didn't move
    }
#ifdef FEAT_MENU
    ಅನ್ಯಥಾ ಯದಿ (in_winbar)
    {
	// After a click on the window toolbar don't start Visual mode.
	ಪ್ರತ್ಯಾಗಮ IN_OTHER_WIN | MOUSE_WINBAR;
    }
#endif
    ಅನ್ಯಥಾ // keep_window_focus must be TRUE
    {
	// before moving the cursor for a left click, stop Visual mode
	ಯದಿ (flags & MOUSE_MAY_STOP_VIS)
	{
	    end_visual_mode_keep_button();
	    redraw_curbuf_later(UPD_INVERTED);	// delete the inversion
	}

#ಯದಿ defined(FEAT_CLIPBOARD)
	// Continue a modeless selection in another window.
	ಯದಿ (cmdwin_type != 0 && row < cmdwin_win->w_winrow)
	    ಪ್ರತ್ಯಾಗಮ IN_OTHER_WIN;
#endif
#ifdef FEAT_PROP_POPUP
	ಯದಿ (in_popup_win)
	{
	    ಯದಿ (popup_dragwin != ರಿಕ್ತ)
	    {
		// dragging a popup window
		popup_drag(popup_dragwin);
		ಪ್ರತ್ಯಾಗಮ IN_UNKNOWN;
	    }
	    // continue a modeless selection in a popup window
	    click_in_popup_win = ರಿಕ್ತ;
	    ಪ್ರತ್ಯಾಗಮ IN_OTHER_WIN;
	}
#endif

	row -= W_WINROW(curwin);
	col -= curwin->w_wincol;

	// When clicking beyond the end of the window, scroll the screen.
	// Scroll by however many rows outside the window we are.
	ಯದಿ (row < 0)
	{
	    ಎಣಿಕೆ = 0;
	    ಪ್ರತಿ (first = TRUE; curwin->w_topline > 1; )
	    {
#ifdef FEAT_DIFF
		ಯದಿ (curwin->w_topfill < diff_check_fill(curwin, curwin->w_topline))
		    ++ಎಣಿಕೆ;
		ಅನ್ಯಥಾ
#endif
		    ಎಣಿಕೆ += plines(curwin->w_topline - 1);
		ಯದಿ (!first && ಎಣಿಕೆ > -row)
		    ಭಙ್ಗ;
		first = FALSE;
#ifdef FEAT_FOLDING
		(ಶೂನ್ಯ)hasFolding(curwin->w_topline, &curwin->w_topline, ರಿಕ್ತ);
#endif
#ifdef FEAT_DIFF
		ಯದಿ (curwin->w_topfill < diff_check_fill(curwin, curwin->w_topline))
		    ++curwin->w_topfill;
		ಅನ್ಯಥಾ
#endif
		{
		    --curwin->w_topline;
#ifdef FEAT_DIFF
		    curwin->w_topfill = 0;
#endif
		}
	    }
#ifdef FEAT_DIFF
	    check_topfill(curwin, FALSE);
#endif
	    curwin->w_valid &=
		      ~(VALID_WROW|VALID_CROW|VALID_BOTLINE|VALID_BOTLINE_AP);
	    redraw_later(UPD_VALID);
	    row = 0;
	}
	ಅನ್ಯಥಾ ಯದಿ (row >= curwin->w_height)
	{
	    ಎಣಿಕೆ = 0;
	    ಪ್ರತಿ (first = TRUE; curwin->w_topline < curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ; )
	    {
#ifdef FEAT_DIFF
		ಯದಿ (curwin->w_topfill > 0)
		    ++ಎಣಿಕೆ;
		ಅನ್ಯಥಾ
#endif
		    ಎಣಿಕೆ += plines(curwin->w_topline);
		ಯದಿ (!first && ಎಣಿಕೆ > row - curwin->w_height + 1)
		    ಭಙ್ಗ;
		first = FALSE;
#ifdef FEAT_FOLDING
		ಯದಿ (hasFolding(curwin->w_topline, ರಿಕ್ತ, &curwin->w_topline)
			&& curwin->w_topline == curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
		    ಭಙ್ಗ;
#endif
#ifdef FEAT_DIFF
		ಯದಿ (curwin->w_topfill > 0)
		    --curwin->w_topfill;
		ಅನ್ಯಥಾ
#endif
		{
		    ++curwin->w_topline;
#ifdef FEAT_DIFF
		    curwin->w_topfill =
				   diff_check_fill(curwin, curwin->w_topline);
#endif
		}
	    }
#ifdef FEAT_DIFF
	    check_topfill(curwin, FALSE);
#endif
	    redraw_later(UPD_VALID);
	    curwin->w_valid &=
		      ~(VALID_WROW|VALID_CROW|VALID_BOTLINE|VALID_BOTLINE_AP);
	    row = curwin->w_height - 1;
	}
	ಅನ್ಯಥಾ ಯದಿ (row == 0)
	{
	    // When dragging the mouse, while the text has been scrolled up as
	    // far as it goes, moving the mouse in the top line should scroll
	    // the text down (done later when recomputing w_topline).
	    ಯದಿ (mouse_dragging > 0
		    && curwin->w_cursor.lnum
				       == curwin->w_buffer->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ
		    && curwin->w_cursor.lnum == curwin->w_topline)
		curwin->w_valid &= ~(VALID_TOPLINE);
	}
    }

    ಯದಿ (prev_row >= W_WINROW(curwin)
	&& prev_row < W_WINROW(curwin) + curwin->w_height
	&& prev_col >= curwin->w_wincol && prev_col < W_ENDCOL(curwin)
						       && Screen_ಸಾಲುಗಳು != ರಿಕ್ತ)
    {
	ಪೂರ್ಣಾಙ್ಕ off = ಸಾಲು_Offset[prev_row] + prev_col;

	// Only use ScreenCols[] after the window was redrawn.  Mainly matters
	// for tests, a user would not click before redrawing.
	// Do not use when 'virtualedit' is active.
	ಯದಿ (curwin->w_redr_type <= UPD_VALID_NO_UPDATE)
	    col_from_screen = ScreenCols[off];
#ifdef FEAT_FOLDING
	// Remember the character under the mouse, it might be a '-' or '+' in
	// the fold column.
	mouse_char = enc_utf8 && Screen_ಸಾಲುಗಳು_UC[off] != 0
				       ? Screen_ಸಾಲುಗಳು_UC[off] : Screen_ಸಾಲುಗಳು[off];
#endif
    }

#ifdef FEAT_FOLDING
    // Check for position outside of the fold column.
    ಯದಿ (
# ifdef FEAT_RIGHTLEFT
	    curwin->w_p_rl ? col < curwin->w_width - curwin->w_p_fdc :
# endif
	    col >= curwin->w_p_fdc + (cmdwin_win != curwin ? 0 : 1)
       )
	mouse_char = ' ';
#endif

    // compute the position in the buffer line from the posn on the screen
    ಯದಿ (mouse_comp_pos(curwin, &row, &col, &curwin->w_cursor.lnum, ರಿಕ್ತ))
	mouse_past_bottom = TRUE;

    // Start Visual mode before coladvance(), for when 'sel' != "old"
    ಯದಿ ((flags & MOUSE_MAY_VIS) && !VIsual_active)
    {
	check_visual_highlight();
	VIsual = old_cursor;
	VIsual_active = TRUE;
	VIsual_reselect = TRUE;
	// if 'selectmode' contains "mouse", start Select mode
	may_start_select('o');
	setmouse();
	ಯದಿ (p_smd && ಸಂದೇಶ_silent == 0)
	    redraw_cmdline = TRUE;	// show visual mode later
    }

    ಯದಿ (col_from_screen >= 0)
    {
	// Use the virtual column from ScreenCols[], it is accurate also after
	// concealed characters.
	col = col_from_screen;
    }

    curwin->w_curswant = col;
    curwin->w_set_curswant = false;	// May still have been TRUE
    ಯದಿ (coladvance(col) == FAIL)	// Mouse click beyond end of line
    {
	ಯದಿ (inclusive != ರಿಕ್ತ)
	    *inclusive = TRUE;
	mouse_past_eol = TRUE;
    }
    ಅನ್ಯಥಾ ಯದಿ (inclusive != ರಿಕ್ತ)
	*inclusive = FALSE;

    ಎಣಿಕೆ = IN_BUFFER;
    ಯದಿ (curwin != old_curwin || curwin->w_cursor.lnum != old_cursor.lnum
	    || curwin->w_cursor.col != old_cursor.col)
	ಎಣಿಕೆ |= CURSOR_MOVED;		// Cursor has moved

#ifdef FEAT_FOLDING
    ಯದಿ (mouse_char == curwin->w_fill_chars.foldclosed)
	ಎಣಿಕೆ |= MOUSE_FOLD_OPEN;
    ಅನ್ಯಥಾ ಯದಿ (mouse_char != ' ')
	ಎಣಿಕೆ |= MOUSE_FOLD_CLOSE;
#endif

    ಪ್ರತ್ಯಾಗಮ ಎಣಿಕೆ;
}

/*
 * Make a horizontal scroll to "leftcol".
 * Return TRUE if the cursor moved, FALSE otherwise.
 */
    ಪೂರ್ಣಾಙ್ಕ
do_mousescroll_horiz(long_u leftcol)
{
    ಯದಿ (curwin->w_p_wrap)
	ಪ್ರತ್ಯಾಗಮ FALSE;  // no horizontal scrolling when wrapping

    ಯದಿ (curwin->w_leftcol == (colnr_T)leftcol)
	ಪ್ರತ್ಯಾಗಮ FALSE;  // already there

    // When the line of the cursor is too short, move the cursor to the
    // longest visible line.
    ಯದಿ (
#ifdef FEAT_GUI
	    (!gui.in_use || vim_strchr(p_go, GO_HORSCROLL) == ರಿಕ್ತ) &&
#endif
		    !virtual_active()
	    && (long)leftcol > scroll_ಸಾಲು_len(curwin->w_cursor.lnum))
    {
	curwin->w_cursor.lnum = ui_find_longest_lnum();
	curwin->w_cursor.col = 0;
    }

    ಪ್ರತ್ಯಾಗಮ set_leftcol((colnr_T)leftcol);
}

/*
 * Normal and Visual modes implementation for scrolling in direction
 * "cap->arg", which is one of the MSCR_ values.
 */
    ಶೂನ್ಯ
nv_mousescroll(cmdarg_T *cap)
{
    win_T   *old_curwin = curwin;

#ifdef FEAT_TABPANEL
    ಯದಿ (mouse_row >= 0 && mouse_col >= 0
	    && (cap->arg == MSCR_UP || cap->arg == MSCR_DOWN)
	    && mouse_on_tabpanel())
    {
	(ಶೂನ್ಯ)tabpanel_scroll(cap->arg == MSCR_UP ? 1 : -1,
		mouse_vert_step > 0 ? mouse_vert_step : 3);
	ಪ್ರತ್ಯಾಗಮ;
    }
#endif

    ಯದಿ (mouse_row >= 0 && mouse_col >= 0)
    {
	// Find the window at the mouse pointer coordinates.
	// NOTE: Must restore "curwin" to "old_curwin" before returning!
	ಪೂರ್ಣಾಙ್ಕ row = mouse_row;
	ಪೂರ್ಣಾಙ್ಕ col = mouse_col;
	curwin = mouse_find_win(&row, &col, FIND_POPUP);
	ಯದಿ (curwin == ರಿಕ್ತ)
	{
	    curwin = old_curwin;
	    ಪ್ರತ್ಯಾಗಮ;
	}

#ifdef FEAT_PROP_POPUP
	ಯದಿ (WIN_IS_POPUP(curwin) && !curwin->w_has_scrollbar)
	{
	    // cannot scroll this popup window
	    curwin = old_curwin;
	    ಪ್ರತ್ಯಾಗಮ;
	}
#endif
	curbuf = curwin->w_buffer;
    }

    // Call the common mouse scroll function shared with other modes.
    do_mousescroll(cap);

    curwin->w_redr_status = true;
    curwin = old_curwin;
    curbuf = curwin->w_buffer;
}

/*
 * Mouse clicks and drags.
 */
    ಶೂನ್ಯ
nv_mouse(cmdarg_T *cap)
{
    (ಶೂನ್ಯ)do_mouse(cap->oap, cap->cmdchar, BACKWARD, cap->count1, 0);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	held_button = MOUSE_RELEASE;

    ಶೂನ್ಯ
reset_held_button(ಶೂನ್ಯ)
{
    held_button = MOUSE_RELEASE;
}

/*
 * Check if typebuf 'tp' contains a terminal mouse code and returns the
 * modifiers found in typebuf in 'modifiers'.
 */
    ಪೂರ್ಣಾಙ್ಕ
check_termcode_mouse(
    char_u	*tp,
    ಪೂರ್ಣಾಙ್ಕ		*slen,
    char_u	*ಕೀಲಿ_name,
    char_u	*modifiers_start,
    ಪೂರ್ಣಾಙ್ಕ		idx,
    ಪೂರ್ಣಾಙ್ಕ		*modifiers)
{
    ಪೂರ್ಣಾಙ್ಕ		j;
    char_u	*p;
#ಯದಿ !defined(UNIX) || defined(FEAT_MOUSE_XTERM) || defined(FEAT_GUI) \
    || defined(FEAT_MOUSE_GPM) || defined(FEAT_SYSMOUSE)
    char_u	bytes[6];
    ಪೂರ್ಣಾಙ್ಕ		ಅಂಕೆ_bytes;
#endif
    ಪೂರ್ಣಾಙ್ಕ		mouse_code = 0;	    // init for GCC
    ಪೂರ್ಣಾಙ್ಕ		is_click, is_drag;
    ಪೂರ್ಣಾಙ್ಕ		is_release, release_is_ambiguous;
    ಪೂರ್ಣಾಙ್ಕ		wheel_code = 0;
    ಪೂರ್ಣಾಙ್ಕ		current_button;
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	orig_ಅಂಕೆ_clicks = 1;
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	orig_mouse_code = 0x0;
#ifdef CHECK_DOUBLE_CLICK
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	orig_mouse_col = 0;
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	orig_mouse_row = 0;
    ಸ್ಥಿರ ಸಂರಚನಾ timeval  orig_mouse_time = {0, 0};
    // time of previous mouse click
    ಸಂರಚನಾ timeval  mouse_time;		// time of current mouse click
    long	timediff;		// elapsed time in msec
#endif

    is_click = is_drag = is_release = release_is_ambiguous = FALSE;

#ಯದಿ !defined(UNIX) || defined(FEAT_MOUSE_XTERM) || defined(FEAT_GUI) \
    || defined(FEAT_MOUSE_GPM) || defined(FEAT_SYSMOUSE)
    ಯದಿ (ಕೀಲಿ_name[0] == KS_MOUSE
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
	ಪ್ರತಿ (;;)
	{
	    // For the GUI and for MS-Windows two bytes each are used for row
	    // and column.  Allows for more than 223 columns.
# ಯದಿ defined(FEAT_GUI) || defined(MSWIN)
	    ಯದಿ (TRUE
#  ಯದಿ defined(FEAT_GUI) && !defined(MSWIN)
		&& gui.in_use
#  endif
		)
	    {
		ಅಂಕೆ_bytes = get_bytes_from_buf(tp + *slen, bytes, 5);
		ಯದಿ (ಅಂಕೆ_bytes == -1)	// not enough coordinates
		    ಪ್ರತ್ಯಾಗಮ -1;
		mouse_code = bytes[0];
		mouse_col = 128 * (bytes[1] - ' ' - 1)
		    + bytes[2] - ' ' - 1;
		mouse_row = 128 * (bytes[3] - ' ' - 1)
		    + bytes[4] - ' ' - 1;
	    }
	    ಅನ್ಯಥಾ
# endif
	    {
		ಅಂಕೆ_bytes = get_bytes_from_buf(tp + *slen, bytes, 3);
		ಯದಿ (ಅಂಕೆ_bytes == -1)	// not enough coordinates
		    ಪ್ರತ್ಯಾಗಮ -1;
		mouse_code = bytes[0];
		mouse_col = bytes[1] - ' ' - 1;
		mouse_row = bytes[2] - ' ' - 1;
	    }
	    *slen += ಅಂಕೆ_bytes;

	    // If the following bytes is also a mouse code and it has the same
	    // code, dump this one and get the next.  This makes dragging a
	    // whole lot faster.
# ifdef FEAT_GUI
	    ಯದಿ (gui.in_use)
		j = 3;
	    ಅನ್ಯಥಾ
# endif
		j = get_termcode_len(idx);
	    ಯದಿ (STRNCMP(tp, tp + *slen, (size_t)j) == 0
		    && tp[*slen + j] == mouse_code
		    && tp[*slen + j + 1] != NUL
		    && tp[*slen + j + 2] != NUL
# ifdef FEAT_GUI
		    && (!gui.in_use
			|| (tp[*slen + j + 3] != NUL
			    && tp[*slen + j + 4] != NUL))
# endif
	       )
		*slen += j;
	    ಅನ್ಯಥಾ
		ಭಙ್ಗ;
	}
    }

    ಯದಿ (ಕೀಲಿ_name[0] == KS_URXVT_MOUSE
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
	p = modifiers_start;
	ಯದಿ (p == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ -1;

	mouse_code = getdigits(&p);
	ಯದಿ (*p++ != ';')
	    ಪ್ರತ್ಯಾಗಮ -1;

	// when mouse reporting is SGR, add 32 to mouse code
	ಯದಿ (ಕೀಲಿ_name[0] == KS_SGR_MOUSE
		|| ಕೀಲಿ_name[0] == KS_SGR_MOUSE_RELEASE)
	    mouse_code += 32;

	mouse_col = getdigits(&p) - 1;
	ಯದಿ (*p++ != ';')
	    ಪ್ರತ್ಯಾಗಮ -1;

	mouse_row = getdigits(&p) - 1;

	// The modifiers were the mouse coordinates, not the modifier keys
	// (alt/shift/ctrl/meta) state.
	*modifiers = 0;
    }

    ಯದಿ (ಕೀಲಿ_name[0] == KS_SGR_MOUSE
	    || ಕೀಲಿ_name[0] == KS_SGR_MOUSE_RELEASE)
    {
	ಯದಿ (ಕೀಲಿ_name[0] == KS_SGR_MOUSE_RELEASE)
	{
	    is_release = TRUE;
	    // This is used below to set held_button.
	    mouse_code |= MOUSE_RELEASE;
	}
    }
    ಅನ್ಯಥಾ
    {
	release_is_ambiguous = TRUE;
	ಯದಿ ((mouse_code & MOUSE_RELEASE) == MOUSE_RELEASE)
	    is_release = TRUE;
    }

    ಯದಿ (ಕೀಲಿ_name[0] == KS_MOUSE
# ifdef FEAT_MOUSE_GPM
	    || ಕೀಲಿ_name[0] == KS_GPM_MOUSE
# endif
# ifdef FEAT_MOUSE_URXVT
	    || ಕೀಲಿ_name[0] == KS_URXVT_MOUSE
# endif
	    || ಕೀಲಿ_name[0] == KS_SGR_MOUSE
	    || ಕೀಲಿ_name[0] == KS_SGR_MOUSE_RELEASE)
    {
# ಯದಿ !defined(MSWIN)
	/*
	 * Handle old style mouse events.
	 * Recognize the xterm mouse wheel, but not in the GUI, the
	 * Linux console with GPM and the MS-DOS or Win32 console
	 * (multi-clicks use >= 0x60).
	 */
	ಯದಿ (mouse_code >= MOUSEWHEEL_ಕೆಳಗಿನ && mouse_code < MOUSESIDEBUTTONS_ಕೆಳಗಿನ
#  ifdef FEAT_GUI
		&& !gui.in_use
#  endif
#  ifdef FEAT_MOUSE_GPM
		&& ಕೀಲಿ_name[0] != KS_GPM_MOUSE
#  endif
	   )
	{
#  ಯದಿ defined(UNIX)
	    ಯದಿ (use_xterm_mouse() > 1 && mouse_code >= 0x80)
		// mouse-move event, using MOUSE_DRAG works
		mouse_code = MOUSE_DRAG;
	    ಅನ್ಯಥಾ
#  endif
		// Keep the mouse_code before it's changed, so that we
		// remember that it was a mouse wheel click.
		wheel_code = mouse_code;
	}
#  ifdef FEAT_MOUSE_XTERM
	ಅನ್ಯಥಾ ಯದಿ (held_button == MOUSE_RELEASE
#   ifdef FEAT_GUI
		&& !gui.in_use
#   endif
		&& (mouse_code == 0x23 || mouse_code == 0x24
		    || mouse_code == 0x40 || mouse_code == 0x41))
	{
	    // Apparently 0x23 and 0x24 are used by rxvt scroll wheel.
	    // And 0x40 and 0x41 are used by some xterm emulator.
	    wheel_code = mouse_code - (mouse_code >= 0x40 ? 0x40 : 0x23)
							      + MOUSEWHEEL_ಕೆಳಗಿನ;
	}
#  endif

#  ಯದಿ defined(UNIX)
	ಅನ್ಯಥಾ ಯದಿ (use_xterm_mouse() > 1)
	{
	    ಯದಿ (mouse_code & MOUSE_DRAG_XTERM)
		mouse_code |= MOUSE_DRAG;
	}
#  endif
#  ifdef FEAT_XCLIPBOARD
	ಅನ್ಯಥಾ ಯದಿ (!(mouse_code & MOUSE_DRAG & ~MOUSE_CLICK_MASK))
	{
	    ಯದಿ (is_release)
		stop_xterm_trace();
	    ಅನ್ಯಥಾ
		start_xterm_trace(mouse_code);
	}
#  endif
# endif
    }
#endif // !UNIX || FEAT_MOUSE_XTERM
#ifdef FEAT_MOUSE_NET
    ಯದಿ (ಕೀಲಿ_name[0] == KS_NETTERM_MOUSE)
    {
	ಪೂರ್ಣಾಙ್ಕ mc, mr;

	// expect a rather limited sequence like: balancing {
	// \033}6,45\r
	// '6' is the row, 45 is the column
	p = tp + *slen;
	mr = getdigits(&p);
	ಯದಿ (*p++ != ',')
	    ಪ್ರತ್ಯಾಗಮ -1;
	mc = getdigits(&p);
	ಯದಿ (*p++ != '\r')
	    ಪ್ರತ್ಯಾಗಮ -1;

	mouse_col = mc - 1;
	mouse_row = mr - 1;
	mouse_code = MOUSE_ಎಡ;
	*slen += (ಪೂರ್ಣಾಙ್ಕ)(p - (tp + *slen));
    }
#endif	// FEAT_MOUSE_NET
#ifdef FEAT_MOUSE_JSB
    ಯದಿ (ಕೀಲಿ_name[0] == KS_JSBTERM_MOUSE)
    {
	ಪೂರ್ಣಾಙ್ಕ mult, val, iter, button, status;

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
	button = mouse_code = 0;
	ವಿಕಲ್ಪ (*p++)
	{
	    ಪಕ್ಷ 'L': button = 1; ಭಙ್ಗ;
	    ಪಕ್ಷ '-': ಭಙ್ಗ;
	    ಪಕ್ಷ 'x': ಭಙ್ಗ; // ignore sequence
	    ಸ್ವಭಾವ:  ಪ್ರತ್ಯಾಗಮ -1; // Unknown Result
	}
	ವಿಕಲ್ಪ (*p++)
	{
	    ಪಕ್ಷ 'M': button |= 2; ಭಙ್ಗ;
	    ಪಕ್ಷ '-': ಭಙ್ಗ;
	    ಪಕ್ಷ 'x': ಭಙ್ಗ; // ignore sequence
	    ಸ್ವಭಾವ:  ಪ್ರತ್ಯಾಗಮ -1; // Unknown Result
	}
	ವಿಕಲ್ಪ (*p++)
	{
	    ಪಕ್ಷ 'R': button |= 4; ಭಙ್ಗ;
	    ಪಕ್ಷ '-': ಭಙ್ಗ;
	    ಪಕ್ಷ 'x': ಭಙ್ಗ; // ignore sequence
	    ಸ್ವಭಾವ:  ಪ್ರತ್ಯಾಗಮ -1; // Unknown Result
	}
	status = *p++;
	ಪ್ರತಿ (val = 0, mult = 100, iter = 0; iter < 3; iter++,
		mult /= 10, p++)
	    ಯದಿ (*p >= '0' && *p <= '9')
		val += (*p - '0') * mult;
	    ಅನ್ಯಥಾ
		ಪ್ರತ್ಯಾಗಮ -1;
	mouse_col = val;
	ಪ್ರತಿ (val = 0, mult = 100, iter = 0; iter < 3; iter++,
		mult /= 10, p++)
	    ಯದಿ (*p >= '0' && *p <= '9')
		val += (*p - '0') * mult;
	    ಅನ್ಯಥಾ
		ಪ್ರತ್ಯಾಗಮ -1;
	mouse_row = val;
	ವಿಕಲ್ಪ (*p++)
	{
	    ಪಕ್ಷ 's': button |= 8; ಭಙ್ಗ;  // SHIFT key Pressed
	    ಪಕ್ಷ '-': ಭಙ್ಗ;  // Not Pressed
	    ಪಕ್ಷ 'x': ಭಙ್ಗ;  // Not Reporting
	    ಸ್ವಭಾವ:  ಪ್ರತ್ಯಾಗಮ -1; // Unknown Result
	}
	ವಿಕಲ್ಪ (*p++)
	{
	    ಪಕ್ಷ 'c': button |= 16; ಭಙ್ಗ;  // CTRL key Pressed
	    ಪಕ್ಷ '-': ಭಙ್ಗ;  // Not Pressed
	    ಪಕ್ಷ 'x': ಭಙ್ಗ;  // Not Reporting
	    ಸ್ವಭಾವ:  ಪ್ರತ್ಯಾಗಮ -1; // Unknown Result
	}
	ಯದಿ (*p++ != '\033')
	    ಪ್ರತ್ಯಾಗಮ -1;
	ಯದಿ (*p++ != '\\')
	    ಪ್ರತ್ಯಾಗಮ -1;
	ವಿಕಲ್ಪ (status)
	{
	    ಪಕ್ಷ 'D': // Double Click
	    ಪಕ್ಷ 'S': // Single Click
		ಯದಿ (button & 1) mouse_code |= MOUSE_ಎಡ;
		ಯದಿ (button & 2) mouse_code |= MOUSE_ಮಧ್ಯ;
		ಯದಿ (button & 4) mouse_code |= MOUSE_ಬಲ;
		ಯದಿ (button & 8) mouse_code |= MOUSE_SHIFT;
		ಯದಿ (button & 16) mouse_code |= MOUSE_CTRL;
		ಭಙ್ಗ;
	    ಪಕ್ಷ 'm': // Mouse move
		ಯದಿ (button & 1) mouse_code |= MOUSE_ಎಡ;
		ಯದಿ (button & 2) mouse_code |= MOUSE_ಮಧ್ಯ;
		ಯದಿ (button & 4) mouse_code |= MOUSE_ಬಲ;
		ಯದಿ (button & 8) mouse_code |= MOUSE_SHIFT;
		ಯದಿ (button & 16) mouse_code |= MOUSE_CTRL;
		ಯದಿ ((button & 7) != 0)
		{
		    held_button = mouse_code;
		    mouse_code |= MOUSE_DRAG;
		}
		is_drag = TRUE;
		showmode();
		ಭಙ್ಗ;
	    ಪಕ್ಷ 'd': // Button Down
		ಯದಿ (button & 1) mouse_code |= MOUSE_ಎಡ;
		ಯದಿ (button & 2) mouse_code |= MOUSE_ಮಧ್ಯ;
		ಯದಿ (button & 4) mouse_code |= MOUSE_ಬಲ;
		ಯದಿ (button & 8) mouse_code |= MOUSE_SHIFT;
		ಯದಿ (button & 16) mouse_code |= MOUSE_CTRL;
		ಭಙ್ಗ;
	    ಪಕ್ಷ 'u': // Button Up
		is_release = TRUE;
		ಯದಿ (button & 1)
		    mouse_code |= MOUSE_ಎಡ;
		ಯದಿ (button & 2)
		    mouse_code |= MOUSE_ಮಧ್ಯ;
		ಯದಿ (button & 4)
		    mouse_code |= MOUSE_ಬಲ;
		ಯದಿ (button & 8)
		    mouse_code |= MOUSE_SHIFT;
		ಯದಿ (button & 16)
		    mouse_code |= MOUSE_CTRL;
		ಭಙ್ಗ;
	    ಸ್ವಭಾವ: ಪ್ರತ್ಯಾಗಮ -1; // Unknown Result
	}

	*slen += (p - (tp + *slen));
    }
#endif // FEAT_MOUSE_JSB
#ifdef FEAT_MOUSE_DEC
    ಯದಿ (ಕೀಲಿ_name[0] == KS_DEC_MOUSE)
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
	ಪೂರ್ಣಾಙ್ಕ Pe, Pb, Pr, Pc;

	p = tp + *slen;

	// get event status
	Pe = getdigits(&p);
	ಯದಿ (*p++ != ';')
	    ಪ್ರತ್ಯಾಗಮ -1;

	// get button status
	Pb = getdigits(&p);
	ಯದಿ (*p++ != ';')
	    ಪ್ರತ್ಯಾಗಮ -1;

	// get row status
	Pr = getdigits(&p);
	ಯದಿ (*p++ != ';')
	    ಪ್ರತ್ಯಾಗಮ -1;

	// get column status
	Pc = getdigits(&p);

	// the page parameter is optional
	ಯದಿ (*p == ';')
	{
	    p++;
	    (ಶೂನ್ಯ)getdigits(&p);
	}
	ಯದಿ (*p++ != '&')
	    ಪ್ರತ್ಯಾಗಮ -1;
	ಯದಿ (*p++ != 'w')
	    ಪ್ರತ್ಯಾಗಮ -1;

	mouse_code = 0;
	ವಿಕಲ್ಪ (Pe)
	{
	    ಪಕ್ಷ  0: ಪ್ರತ್ಯಾಗಮ -1; // position request while unavailable
	    ಪಕ್ಷ  1: // a response to a locator position request includes
		     //	the status of all buttons
		     Pb &= 7;   // mask off and ignore fourth button
		     ಯದಿ (Pb & 4)
			 mouse_code  = MOUSE_ಎಡ;
		     ಯದಿ (Pb & 2)
			 mouse_code  = MOUSE_ಮಧ್ಯ;
		     ಯದಿ (Pb & 1)
			 mouse_code  = MOUSE_ಬಲ;
		     ಯದಿ (Pb)
		     {
			 held_button = mouse_code;
			 mouse_code |= MOUSE_DRAG;
			 WantQueryMouse = TRUE;
		     }
		     is_drag = TRUE;
		     showmode();
		     ಭಙ್ಗ;
	    ಪಕ್ಷ  2: mouse_code = MOUSE_ಎಡ;
		     WantQueryMouse = TRUE;
		     ಭಙ್ಗ;
	    ಪಕ್ಷ  3: mouse_code = MOUSE_ಎಡ;
		     is_release = TRUE;
		     ಭಙ್ಗ;
	    ಪಕ್ಷ  4: mouse_code = MOUSE_ಮಧ್ಯ;
		     WantQueryMouse = TRUE;
		     ಭಙ್ಗ;
	    ಪಕ್ಷ  5: mouse_code = MOUSE_ಮಧ್ಯ;
		     is_release = TRUE;
		     ಭಙ್ಗ;
	    ಪಕ್ಷ  6: mouse_code = MOUSE_ಬಲ;
		     WantQueryMouse = TRUE;
		     ಭಙ್ಗ;
	    ಪಕ್ಷ  7: mouse_code = MOUSE_ಬಲ;
		     is_release = TRUE;
		     ಭಙ್ಗ;
	    ಪಕ್ಷ  8: ಪ್ರತ್ಯಾಗಮ -1; // fourth button down
	    ಪಕ್ಷ  9: ಪ್ರತ್ಯಾಗಮ -1; // fourth button up
	    ಪಕ್ಷ 10: ಪ್ರತ್ಯಾಗಮ -1; // mouse outside of filter rectangle
	    ಸ್ವಭಾವ: ಪ್ರತ್ಯಾಗಮ -1; // should never occur
	}

	mouse_col = Pc - 1;
	mouse_row = Pr - 1;

	*slen += (ಪೂರ್ಣಾಙ್ಕ)(p - (tp + *slen));
    }
#endif // FEAT_MOUSE_DEC
#ifdef FEAT_MOUSE_PTERM
    ಯದಿ (ಕೀಲಿ_name[0] == KS_PTERM_MOUSE)
    {
	ಪೂರ್ಣಾಙ್ಕ button, ಅಂಕೆ_clicks, action;

	p = tp + *slen;

	action = getdigits(&p);
	ಯದಿ (*p++ != ';')
	    ಪ್ರತ್ಯಾಗಮ -1;

	mouse_row = getdigits(&p);
	ಯದಿ (*p++ != ';')
	    ಪ್ರತ್ಯಾಗಮ -1;
	mouse_col = getdigits(&p);
	ಯದಿ (*p++ != ';')
	    ಪ್ರತ್ಯಾಗಮ -1;

	button = getdigits(&p);
	mouse_code = 0;

	ವಿಕಲ್ಪ (button)
	{
	    ಪಕ್ಷ 4: mouse_code = MOUSE_ಎಡ; ಭಙ್ಗ;
	    ಪಕ್ಷ 1: mouse_code = MOUSE_ಬಲ; ಭಙ್ಗ;
	    ಪಕ್ಷ 2: mouse_code = MOUSE_ಮಧ್ಯ; ಭಙ್ಗ;
	    ಸ್ವಭಾವ: ಪ್ರತ್ಯಾಗಮ -1;
	}

	ವಿಕಲ್ಪ (action)
	{
	    ಪಕ್ಷ 31: // Initial press
		ಯದಿ (*p++ != ';')
		    ಪ್ರತ್ಯಾಗಮ -1;

		ಅಂಕೆ_clicks = getdigits(&p); // Not used
		ಭಙ್ಗ;

	    ಪಕ್ಷ 32: // Release
		is_release = TRUE;
		ಭಙ್ಗ;

	    ಪಕ್ಷ 33: // Drag
		held_button = mouse_code;
		mouse_code |= MOUSE_DRAG;
		ಭಙ್ಗ;

	    ಸ್ವಭಾವ:
		ಪ್ರತ್ಯಾಗಮ -1;
	}

	ಯದಿ (*p++ != 't')
	    ಪ್ರತ್ಯಾಗಮ -1;

	*slen += (p - (tp + *slen));
    }
#endif // FEAT_MOUSE_PTERM

    // Interpret the mouse code
    current_button = (mouse_code & MOUSE_CLICK_MASK);
    ಯದಿ (is_release)
	current_button |= MOUSE_RELEASE;

    ಯದಿ (current_button == MOUSE_RELEASE
#ifdef FEAT_MOUSE_XTERM
	    && wheel_code == 0
#endif
       )
    {
	/*
	 * If we get a mouse drag or release event when there is no mouse
	 * button held down (held_button == MOUSE_RELEASE), produce a K_IGNORE
	 * below.
	 * (can happen when you hold down two buttons and then let them go, or
	 * click in the menu bar, but not on a menu, and drag into the text).
	 */
	ಯದಿ ((mouse_code & MOUSE_DRAG) == MOUSE_DRAG)
	    is_drag = TRUE;
	current_button = held_button;
    }
    ಅನ್ಯಥಾ
    {
      ಯದಿ (wheel_code == 0)
      {
#ifdef CHECK_DOUBLE_CLICK
# ifdef FEAT_MOUSE_GPM
	/*
	 * Only for Unix, when GUI not active, we handle multi-clicks here, but
	 * not for GPM mouse events.
	 */
#  ifdef FEAT_GUI
	ಯದಿ (ಕೀಲಿ_name[0] != KS_GPM_MOUSE && !gui.in_use)
#  ಅನ್ಯಥಾ
	    ಯದಿ (ಕೀಲಿ_name[0] != KS_GPM_MOUSE)
#  endif
# ಅನ್ಯಥಾ
#  ifdef FEAT_GUI
		ಯದಿ (!gui.in_use)
#  endif
# endif
		{
		    /*
		     * Compute the time elapsed since the previous mouse click.
		     */
		    gettimeofday(&mouse_time, ರಿಕ್ತ);
		    ಯದಿ (orig_mouse_time.tv_sec == 0)
		    {
			/*
			 * Avoid computing the difference between mouse_time
			 * and orig_mouse_time for the first click, as the
			 * difference would be huge and would cause
			 * multiplication overflow.
			 */
			timediff = p_mouset;
		    }
		    ಅನ್ಯಥಾ
			timediff = time_diff_ms(&orig_mouse_time, &mouse_time);
		    orig_mouse_time = mouse_time;
		    ಯದಿ (mouse_code == orig_mouse_code
			    && timediff < p_mouset
			    && orig_ಅಂಕೆ_clicks != 4
			    && orig_mouse_col == mouse_col
			    && orig_mouse_row == mouse_row
			    && (is_mouse_topline(curwin)
				// Double click in tab pages line also works
				// when window contents changes.
				|| (mouse_row == 0 && firstwin->w_winrow > 0))
		       )
			++orig_ಅಂಕೆ_clicks;
		    ಅನ್ಯಥಾ
			orig_ಅಂಕೆ_clicks = 1;
		    orig_mouse_col = mouse_col;
		    orig_mouse_row = mouse_row;
		    set_mouse_topline(curwin);
		}
# ಯದಿ defined(FEAT_GUI) || defined(FEAT_MOUSE_GPM)
		ಅನ್ಯಥಾ
		    orig_ಅಂಕೆ_clicks = ಅಂಕೆ_MOUSE_CLICKS(mouse_code);
# endif
#ಅನ್ಯಥಾ
	orig_ಅಂಕೆ_clicks = ಅಂಕೆ_MOUSE_CLICKS(mouse_code);
#endif
	is_click = TRUE;
      }
      orig_mouse_code = mouse_code;
    }
    ಯದಿ (!is_drag)
	held_button = mouse_code & MOUSE_CLICK_MASK;

    /*
     * Translate the actual mouse event into a pseudo mouse event.
     * First work out what modifiers are to be used.
     */
    ಯದಿ (orig_mouse_code & MOUSE_SHIFT)
	*modifiers |= MOD_MASK_SHIFT;
    ಯದಿ (orig_mouse_code & MOUSE_CTRL)
	*modifiers |= MOD_MASK_CTRL;
    ಯದಿ (orig_mouse_code & MOUSE_ALT)
	*modifiers |= MOD_MASK_ALT;
    ಯದಿ (orig_ಅಂಕೆ_clicks == 2)
	*modifiers |= MOD_MASK_2CLICK;
    ಅನ್ಯಥಾ ಯದಿ (orig_ಅಂಕೆ_clicks == 3)
	*modifiers |= MOD_MASK_3CLICK;
    ಅನ್ಯಥಾ ಯದಿ (orig_ಅಂಕೆ_clicks == 4)
	*modifiers |= MOD_MASK_4CLICK;

    // Work out our pseudo mouse event. Note that MOUSE_RELEASE gets added,
    // then it's not mouse up/down.
    ಕೀಲಿ_name[0] = KS_EXTRA;
    ಯದಿ (wheel_code != 0 && (!is_release || release_is_ambiguous))
    {
	ಯದಿ (wheel_code & MOUSE_CTRL)
	    *modifiers |= MOD_MASK_CTRL;
	ಯದಿ (wheel_code & MOUSE_ALT)
	    *modifiers |= MOD_MASK_ALT;

	ಯದಿ (wheel_code & 1 && wheel_code & 2)
	    ಕೀಲಿ_name[1] = (ಪೂರ್ಣಾಙ್ಕ)KE_MOUSELEFT;
	ಅನ್ಯಥಾ ಯದಿ (wheel_code & 2)
	    ಕೀಲಿ_name[1] = (ಪೂರ್ಣಾಙ್ಕ)KE_MOUSERIGHT;
	ಅನ್ಯಥಾ ಯದಿ (wheel_code & 1)
	    ಕೀಲಿ_name[1] = (ಪೂರ್ಣಾಙ್ಕ)KE_MOUSEUP;
	ಅನ್ಯಥಾ
	    ಕೀಲಿ_name[1] = (ಪೂರ್ಣಾಙ್ಕ)KE_MOUSEDOWN;

	held_button = MOUSE_RELEASE;
    }
    ಅನ್ಯಥಾ
    {
#ಯದಿ defined(UNIX)
	ಯದಿ (use_xterm_mouse() && orig_mouse_code >= MOUSESIDEBUTTONS_ಕೆಳಗಿನ)
	    current_button = (current_button) ? MOUSE_X2 : MOUSE_X1;
#endif
	ಕೀಲಿ_name[1] = get_pseudo_mouse_code(current_button, is_click, is_drag);
    }


    // Make sure the mouse position is valid.  Some terminals may return weird
    // values.
    ಯದಿ (mouse_col >= Columns)
	mouse_col = Columns - 1;
    ಯದಿ (mouse_row >= Rows)
	mouse_row = Rows - 1;

    ಪ್ರತ್ಯಾಗಮ 0;
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
    ಪೂರ್ಣಾಙ್ಕ
mouse_comp_pos(
    win_T	*win,
    ಪೂರ್ಣಾಙ್ಕ		*rowp,
    ಪೂರ್ಣಾಙ್ಕ		*colp,
    linenr_T	*lnump,
    ಪೂರ್ಣಾಙ್ಕ		*plines_cache)
{
    ಪೂರ್ಣಾಙ್ಕ		col = *colp;
    ಪೂರ್ಣಾಙ್ಕ		row = *rowp;
    linenr_T	lnum;
    ಪೂರ್ಣಾಙ್ಕ		retval = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		off;
    ಪೂರ್ಣಾಙ್ಕ		ಎಣಿಕೆ;

#ifdef FEAT_RIGHTLEFT
    ಯದಿ (win->w_p_rl)
	col = win->w_width - 1 - col;
#endif

    lnum = win->w_topline;

    ಯಾವತ್ (row > 0)
    {
	ಪೂರ್ಣಾಙ್ಕ cache_idx = lnum - win->w_topline;

	// Only "Rows" lines are cached, with folding we'll run out of entries
	// and use the slow way.
	ಯದಿ (plines_cache != ರಿಕ್ತ && cache_idx < Rows
						&& plines_cache[cache_idx] > 0)
	    ಎಣಿಕೆ = plines_cache[cache_idx];
	ಅನ್ಯಥಾ
	{
#ifdef FEAT_DIFF
	    // Don't include filler lines in "count"
	    ಯದಿ (win->w_p_diff
# ifdef FEAT_FOLDING
		    && !hasFoldingWin(win, lnum, ರಿಕ್ತ, ರಿಕ್ತ, TRUE, ರಿಕ್ತ)
# endif
		    )
	    {
		ಯದಿ (lnum == win->w_topline)
		    row -= win->w_topfill;
		ಅನ್ಯಥಾ
		    row -= diff_check_fill(win, lnum);
		ಎಣಿಕೆ = plines_win_nofill(win, lnum, FALSE);
	    }
	    ಅನ್ಯಥಾ
#endif
		ಎಣಿಕೆ = plines_win(win, lnum, FALSE);
	    ಯದಿ (plines_cache != ರಿಕ್ತ && cache_idx < Rows)
		plines_cache[cache_idx] = ಎಣಿಕೆ;
	}

	ಯದಿ (win->w_skipcol > 0 && lnum == win->w_topline)
	{
	    ಪೂರ್ಣಾಙ್ಕ width1 = win->w_width - win_col_off(win);

	    ಯದಿ (width1 > 0)
	    {
		ಪೂರ್ಣಾಙ್ಕ skip_ಸಾಲುಗಳು = 0;

		// Adjust for 'smoothscroll' clipping the top screen lines.
		// A similar formula is used in curs_columns().
		ಯದಿ (win->w_skipcol > width1)
		    skip_ಸಾಲುಗಳು = (win->w_skipcol - width1)
					    / (width1 + win_col_off2(win)) + 1;
		ಅನ್ಯಥಾ ಯದಿ (win->w_skipcol > 0)
		    skip_ಸಾಲುಗಳು = 1;

		ಎಣಿಕೆ -= skip_ಸಾಲುಗಳು;
	    }
	}

	ಯದಿ (ಎಣಿಕೆ > row)
	    ಭಙ್ಗ;	// Position is in this buffer line.
#ifdef FEAT_FOLDING
	(ಶೂನ್ಯ)hasFoldingWin(win, lnum, ರಿಕ್ತ, &lnum, TRUE, ರಿಕ್ತ);
#endif
	ಯದಿ (lnum == win->w_buffer->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	{
	    retval = TRUE;
	    ಭಙ್ಗ;		// past end of file
	}
	row -= ಎಣಿಕೆ;
	++lnum;
    }

    ಯದಿ (!retval)
    {
	// Compute the column without wrapping.
	off = win_col_off(win) - win_col_off2(win);
	ಯದಿ (col < off)
	    col = off;
	col += row * (win->w_width - off);

	// Add skip column for the topline.
	ಯದಿ (lnum == win->w_topline)
	    col += win->w_skipcol;
    }

    ಯದಿ (!win->w_p_wrap)
	col += win->w_leftcol;

    // skip line number and fold column in front of the line
    col -= win_col_off(win);
    ಯದಿ (col <= 0)
    {
#ifdef FEAT_NETBEANS_INTG
	// if mouse is clicked on the gutter, then inform the netbeans server
	ಯದಿ (*colp < win_col_off(win))
	    netbeans_gutter_click(lnum);
#endif
	col = 0;
    }

    *colp = col;
    *rowp = row;
    *lnump = lnum;
    ಪ್ರತ್ಯಾಗಮ retval;
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
mouse_find_win(ಪೂರ್ಣಾಙ್ಕ *rowp, ಪೂರ್ಣಾಙ್ಕ *colp, mouse_find_T popup UNUSED)
{
    frame_T	*fp;
    win_T	*wp;

#ifdef FEAT_PROP_POPUP
    win_T	*pwp = ರಿಕ್ತ;

    ಯದಿ (popup != IGNORE_POPUP)
    {
	popup_reset_handled(POPUP_HANDLED_1);
	ಯಾವತ್ ((wp = find_next_popup(TRUE, POPUP_HANDLED_1)) != ರಿಕ್ತ)
	{
	    ಯದಿ (*rowp >= wp->w_winrow && *rowp < wp->w_winrow + popup_height(wp)
		    && *colp >= wp->w_wincol
				    && *colp < wp->w_wincol + popup_width(wp))
		pwp = wp;
	}
	ಯದಿ (pwp != ರಿಕ್ತ)
	{
	    ಯದಿ (popup == FAIL_POPUP)
		ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	    *rowp -= pwp->w_winrow;
	    *colp -= pwp->w_wincol;
	    ಪ್ರತ್ಯಾಗಮ pwp;
	}
    }
#endif

    fp = topframe;

    ಯದಿ (*colp < firstwin->w_wincol
	    || *colp >= firstwin->w_wincol + fp->fr_width
	    || *rowp < firstwin->w_winrow)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    *rowp -= firstwin->w_winrow;
    *colp -= firstwin->w_wincol;
    ಪ್ರತಿ (;;)
    {
	ಯದಿ (fp->fr_layout == FR_LEAF)
	    ಭಙ್ಗ;
	ಯದಿ (fp->fr_layout == FR_ROW)
	{
	    ಪ್ರತಿ (fp = fp->fr_child; fp->fr_next != ರಿಕ್ತ; fp = fp->fr_next)
	    {
		ಯದಿ (*colp < fp->fr_width)
		    ಭಙ್ಗ;
		*colp -= fp->fr_width;
	    }
	}
	ಅನ್ಯಥಾ    // fr_layout == FR_COL
	{
	    ಪ್ರತಿ (fp = fp->fr_child; fp->fr_next != ರಿಕ್ತ; fp = fp->fr_next)
	    {
		ಯದಿ (*rowp < fp->fr_height)
		    ಭಙ್ಗ;
		*rowp -= fp->fr_height;
	    }
	}
    }
    // When using a timer that closes a window the window might not actually
    // exist.
    FOR_ALL_WINDOWS(wp)
	ಯದಿ (wp == fp->fr_win)
	{
#ifdef FEAT_MENU
	    *rowp -= wp->w_winbar_height;
#endif
	    ಪ್ರತ್ಯಾಗಮ wp;
	}
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

#ಯದಿ defined(NEED_VCOL2COL) || defined(FEAT_BEVAL) || defined(FEAT_PROP_POPUP) \
	|| defined(FEAT_EVAL)
/*
 * Convert a virtual (screen) column to a character column.
 * The first column is zero.
 */
    ಪೂರ್ಣಾಙ್ಕ
vcol2col(win_T *wp, linenr_T lnum, ಪೂರ್ಣಾಙ್ಕ vcol, colnr_T *coladdp)
{
    char_u	    *ಸಾಲು;
    chartabsize_T   cts;

    // try to advance to the specified column
    ಸಾಲು = ml_get_buf(wp->w_buffer, lnum, FALSE);
    init_chartabsize_arg(&cts, wp, lnum, 0, ಸಾಲು, ಸಾಲು);
    ಯಾವತ್ (cts.cts_vcol < vcol && *cts.cts_ptr != NUL)
    {
	ಪೂರ್ಣಾಙ್ಕ size = win_lbr_chartabsize(&cts, ರಿಕ್ತ, ರಿಕ್ತ);
	ಯದಿ (cts.cts_vcol + size > vcol)
	    ಭಙ್ಗ;
	cts.cts_vcol += size;
	MB_PTR_ADV(cts.cts_ptr);
    }
    clear_chartabsize_arg(&cts);

    ಯದಿ (coladdp != ರಿಕ್ತ)
	*coladdp = vcol - cts.cts_vcol;
    ಪ್ರತ್ಯಾಗಮ (ಪೂರ್ಣಾಙ್ಕ)(cts.cts_ptr - ಸಾಲು);
}
#endif

#ಯದಿ defined(FEAT_EVAL)
/*
 * "getmousepos()" function.
 */
    ಶೂನ್ಯ
f_getmousepos(typval_T *argvars UNUSED, typval_T *rettv)
{
    dict_T	*d;
    win_T	*wp;
    ಪೂರ್ಣಾಙ್ಕ		row = mouse_row;
    ಪೂರ್ಣಾಙ್ಕ		col = mouse_col;
    varnumber_T winid = 0;
    varnumber_T winrow = 0;
    varnumber_T wincol = 0;
    linenr_T	lnum = 0;
    varnumber_T column = 0;
    colnr_T	coladd = 0;

    ಯದಿ (rettv_dict_alloc(rettv) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;
    d = rettv->vval.v_dict;

    dict_add_ಸಂಖ್ಯೆ(d, "screenrow", (varnumber_T)mouse_row + 1);
    dict_add_ಸಂಖ್ಯೆ(d, "screencol", (varnumber_T)mouse_col + 1);

    wp = mouse_find_win(&row, &col, FIND_POPUP);
    ಯದಿ (wp != ರಿಕ್ತ)
    {
	ಪೂರ್ಣಾಙ್ಕ	ತುದಿ_off = 0;
	ಪೂರ್ಣಾಙ್ಕ	ಎಡ_off = 0;
	ಪೂರ್ಣಾಙ್ಕ	height = wp->w_height + wp->w_status_height;

# ifdef FEAT_PROP_POPUP
	ಯದಿ (WIN_IS_POPUP(wp))
	{
	    ತುದಿ_off = popup_ತುದಿ_extra(wp);
	    ಎಡ_off = popup_ಎಡ_extra(wp);
	    height = popup_height(wp);
	}
# endif
	ಯದಿ (row < height)
	{
	    winid = wp->w_id;
	    winrow = row + 1;
	    wincol = col + 1;
	    row -= ತುದಿ_off;
	    col -= ಎಡ_off;
	    ಯದಿ (row >= 0 && row < wp->w_height && col >= 0 && col < wp->w_width)
	    {
		(ಶೂನ್ಯ)mouse_comp_pos(wp, &row, &col, &lnum, ರಿಕ್ತ);
		col = vcol2col(wp, lnum, col, &coladd);
		column = col + 1;
	    }
	}
    }
    dict_add_ಸಂಖ್ಯೆ(d, "winid", winid);
    dict_add_ಸಂಖ್ಯೆ(d, "winrow", winrow);
    dict_add_ಸಂಖ್ಯೆ(d, "wincol", wincol);
    dict_add_ಸಂಖ್ಯೆ(d, "line", (varnumber_T)lnum);
    dict_add_ಸಂಖ್ಯೆ(d, "column", column);
    dict_add_ಸಂಖ್ಯೆ(d, "coladd", coladd);
}
#endif
