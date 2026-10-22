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

#شامل "vim.h"

/*
 * Horiziontal and vertical steps used when scrolling.
 * When negative scroll by a whole page.
 */
ایستا long mouse_hor_step = 6;
ایستا long mouse_vert_step = 3;
ایستا win_T *dragwin = پوچ;	// window being dragged
ایستا صحیح stl_click_handler(win_T *wp, صحیح mrow, صحیح mcol, صحیح which_button,
								    صحیح mods);
ایستا صحیح stl_click_handler_regions(stl_click_region_T *regions,
				    صحیح region_شمار, صحیح winid,
				    char_u *area_name, صحیح mrow, صحیح mcol,
				    صحیح which_button, صحیح mods);

    تهی
mouse_set_vert_scroll_step(long step)
{
    mouse_vert_step = step;
}

    تهی
mouse_set_hor_scroll_step(long step)
{
    mouse_hor_step = step;
}

#ifdef CHECK_DOUBLE_CLICK
/*
 * Return the duration from t1 to t2 in milliseconds.
 */
    ایستا long
time_diff_ms(ساختار timeval *t1, ساختار timeval *t2)
{
    // This handles wrapping of tv_usec correctly without any special case.
    // Example of 2 pairs (tv_sec, tv_usec) with a duration of 5 ms:
    //	   t1 = (1, 998000) t2 = (2, 3000) gives:
    //	   (2 - 1) * 1000 + (3000 - 998000) / 1000 -> 5 ms.
    بازگشت (t2->tv_sec - t1->tv_sec) * 1000
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
    ایستا صحیح
get_mouse_class(char_u *p)
{
    صحیح		c;

    اگر (has_mbyte && MB_BYTE2LEN(p[0]) > 1)
	بازگشت mb_get_class(p);

    c = *p;
    اگر (c == ' ' || c == '\t')
	بازگشت 0;

    اگر (vim_iswordc(c))
	بازگشت 2;

    // There are a few special cases where we want certain combinations of
    // characters to be considered as a single word.  These are things like
    // "->", "/ *", "*=", "+=", "&=", "<=", ">=", "!=" etc.  Otherwise, each
    // character is in its own class.
    اگر (c != NUL && vim_strchr((char_u *)"-+*/%<>&|^!=", c) != پوچ)
	بازگشت 1;
    بازگشت c;
}

/*
 * Move "pos" back to the start of the word it's in.
 */
    ایستا تهی
find_start_of_word(pos_T *pos)
{
    char_u	*سطر;
    صحیح		cclass;
    صحیح		col;

    سطر = ml_get(pos->lnum);
    cclass = get_mouse_class(سطر + pos->col);

    وقتی (pos->col > 0)
    {
	col = pos->col - 1;
	col -= (*mb_head_off)(سطر, سطر + col);
	اگر (get_mouse_class(سطر + col) != cclass)
	    بشکن;
	pos->col = col;
    }
}

/*
 * Move "pos" forward to the end of the word it's in.
 * When 'selection' is "exclusive", the position is just after the word.
 */
    ایستا تهی
find_end_of_word(pos_T *pos)
{
    char_u	*سطر;
    صحیح		cclass;
    صحیح		col;

    سطر = ml_get(pos->lnum);
    اگر (*p_sel == 'e' && pos->col > 0)
    {
	--pos->col;
	pos->col -= (*mb_head_off)(سطر, سطر + pos->col);
    }
    cclass = get_mouse_class(سطر + pos->col);
    وقتی (سطر[pos->col] != NUL)
    {
	col = pos->col + (*mb_ptr2len)(سطر + pos->col);
	اگر (get_mouse_class(سطر + col) != cclass)
	{
	    اگر (*p_sel == 'e')
		pos->col = col;
	    بشکن;
	}
	pos->col = col;
    }
}

#اگر defined(FEAT_GUI_MOTIF) || defined(FEAT_GUI_GTK) \
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
    ایستا صحیح
get_fpos_of_mouse(pos_T *mpos)
{
    win_T	*wp;
    صحیح		row = mouse_row;
    صحیح		col = mouse_col;

    اگر (row < 0 || col < 0)		// check if it makes sense
	بازگشت IN_UNKNOWN;

    // find the window where the row is in
    wp = mouse_find_win(&row, &col, FAIL_POPUP);
    اگر (wp == پوچ)
	بازگشت IN_UNKNOWN;
    // winpos and height may change in win_enter()!
    اگر (row >= wp->w_height)	// In (or below) status line
	بازگشت IN_STATUS_سطر;
    اگر (col >= wp->w_width)	// In vertical separator line
	بازگشت IN_SEP_سطر;

    اگر (wp != curwin)
	بازگشت IN_UNKNOWN;

    // compute the position in the buffer line from the posn on the screen
    اگر (mouse_comp_pos(curwin, &row, &col, &mpos->lnum, پوچ))
	بازگشت IN_STATUS_سطر; // past bottom

    mpos->col = vcol2col(wp, mpos->lnum, col, &mpos->coladd);
    بازگشت IN_BUFFER;
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
    صحیح
do_mouse(
    oparg_T	*oap,		// operator argument, can be NULL
    صحیح		c,		// K_LEFTMOUSE, etc
    صحیح		dir,		// Direction to 'put' if necessary
    long	شمار,
    صحیح		fixindent)	// PUT_FIXINDENT if fixing indent necessary
{
    ایستا صحیح	do_always = FALSE;	// ignore 'mouse' setting next time
    ایستا صحیح	got_click = FALSE;	// got a click some time back

    صحیح		which_button;	// MOUSE_LEFT, _MIDDLE or _RIGHT
    صحیح		is_click = FALSE; // If FALSE it's a drag or release event
    صحیح		is_drag = FALSE;  // If TRUE it's a drag event
    صحیح		jump_flags = 0;	// flags for jump_to_mouse()
    pos_T	start_visual;
    صحیح		moved;		// Has cursor moved?
    صحیح		in_status_سطر;	// mouse in status line
    ایستا صحیح	in_tab_سطر = FALSE; // mouse clicked in tab line
    ایستا صحیح	in_tabpanel = FALSE; // mouse clicked in tabpanel
#ifdef FEAT_TABPANEL
    ایستا bool	in_tabpanel_scrollbar = false; // dragging tabpanel scrollbar
#endif
    صحیح		in_sep_سطر;	// mouse in vertical separator line
    صحیح		c1, c2;
#اگر defined(FEAT_FOLDING)
    pos_T	save_cursor;
#endif
    win_T	*old_curwin = curwin;
    ایستا pos_T orig_cursor;
    colnr_T	leftcol, rightcol;
    pos_T	end_visual;
    صحیح		diff;
    صحیح		old_active = VIsual_active;
    صحیح		old_mode = VIsual_mode;
    صحیح		regname;

#اگر defined(FEAT_FOLDING)
    save_cursor = curwin->w_cursor;
#endif

    // When GUI is active, always recognize mouse events, otherwise:
    // - Ignore mouse event in normal mode if 'mouse' doesn't include 'n'.
    // - Ignore mouse event in visual mode if 'mouse' doesn't include 'v'.
    // - For command line and insert mode 'mouse' is checked before calling
    //	 do_mouse().
    اگر (do_always)
	do_always = FALSE;
    وگرنه
#ifdef FEAT_GUI
	اگر (!gui.in_use)
#endif
	{
	    اگر (VIsual_active)
	    {
		اگر (!mouse_has(MOUSE_VISUAL))
		    بازگشت FALSE;
	    }
	    وگرنه اگر (State == MODE_NORMAL && !mouse_has(MOUSE_NORMAL))
		بازگشت FALSE;
	}

    برای (;;)
    {
	which_button = get_mouse_button(KEY2TERMCAP1(c), &is_click, &is_drag);
	اگر (is_drag)
	{
	    // If the next character is the same mouse event then use that
	    // one. Speeds up dragging the status line.
	    // Note: Since characters added to the stuff buffer in the code
	    // below need to come before the next character, do not do this
	    // when the current character was stuffed.
	    اگر (!کلید_Stuffed && vpeekc() != NUL)
	    {
		صحیح nc;
		صحیح save_mouse_row = mouse_row;
		صحیح save_mouse_col = mouse_col;

		// Need to get the character, peeking doesn't get the actual
		// one.
		nc = safe_vgetc();
		اگر (c == nc)
		    ادامه;
		vungetc(nc);
		mouse_row = save_mouse_row;
		mouse_col = save_mouse_col;
	    }
	}
	بشکن;
    }

    اگر (c == K_MOUSEMOVE)
    {
	// Mouse moved without a button pressed.
#ifdef FEAT_BEVAL_TERM
	ui_may_remove_balloon();
	اگر (p_bevalterm)
	{
	    profile_setlimit(p_bdlay, &bevalexpr_due);
	    bevalexpr_due_set = TRUE;
	}
#endif
#ifdef FEAT_PROP_POPUP
	popup_handle_mouse_moved();
#endif
	بازگشت FALSE;
    }

#ifdef FEAT_MOUSESHAPE
    // May have stopped dragging the status or separator line.  The pointer is
    // most likely still on the status or separator line.
    اگر (!is_drag && drag_status_سطر)
    {
	drag_status_سطر = FALSE;
	update_mouseshape(SHAPE_IDX_STATUS);
    }
    اگر (!is_drag && drag_sep_سطر)
    {
	drag_sep_سطر = FALSE;
	update_mouseshape(SHAPE_IDX_VSEP);
    }
#endif

    // Ignore drag and release events if we didn't get a click.
    اگر (is_click)
    {
	got_click = TRUE;
	in_tab_سطر = FALSE;
	in_tabpanel = FALSE;
#ifdef FEAT_TABPANEL
	in_tabpanel_scrollbar = false;
#endif
    }
    وگرنه
    {
	اگر (!got_click)			// didn't get click, ignore
	    بازگشت FALSE;
	اگر (!is_drag)			// release, reset got_click
	{
	    got_click = FALSE;
	    اگر (in_tab_سطر || in_tabpanel
#ifdef FEAT_TABPANEL
		    || in_tabpanel_scrollbar
#endif
		    )
	    {
		in_tab_سطر = FALSE;
		in_tabpanel = FALSE;
#ifdef FEAT_TABPANEL
		in_tabpanel_scrollbar = false;
#endif
		بازگشت FALSE;
	    }
	}
    }

#ifdef FEAT_TABPANEL
    // Continue a scrollbar drag before any tab-selection handling.
    اگر (is_drag && in_tabpanel_scrollbar)
    {
	tabpanel_drag_scrollbar(mouse_row);
	بازگشت FALSE;
    }
#endif

    // CTRL right mouse button does CTRL-T
    اگر (is_click && (mod_mask & MOD_MASK_CTRL) && which_button == MOUSE_راست)
    {
	اگر (State & MODE_درج)
	    stuffcharReadbuff(Ctrl_O);
	اگر (شمار > 1)
	    stuffnumReadbuff(شمار);
	stuffcharReadbuff(Ctrl_T);
	got_click = FALSE;		// ignore drag&release now
	بازگشت FALSE;
    }

    // CTRL only works with left mouse button
    اگر ((mod_mask & MOD_MASK_CTRL) && which_button != MOUSE_چپ)
	بازگشت FALSE;

    // When a modifier is down, ignore drag and release events, as well as
    // multiple clicks and the middle mouse button.
    // Accept shift-leftmouse drags when 'mousemodel' is "popup.*".
    اگر ((mod_mask & (MOD_MASK_SHIFT | MOD_MASK_CTRL | MOD_MASK_ALT
							     | MOD_MASK_META))
	    && (!is_click
		|| (mod_mask & MOD_MASK_MULTI_CLICK)
		|| which_button == MOUSE_میانه)
	    && !((mod_mask & (MOD_MASK_SHIFT|MOD_MASK_ALT))
		&& mouse_model_popup()
		&& which_button == MOUSE_چپ)
	    && !((mod_mask & MOD_MASK_ALT)
		&& !mouse_model_popup()
		&& which_button == MOUSE_راست)
	    )
	بازگشت FALSE;

    // If the button press was used as the movement command for an operator
    // (eg "d<MOUSE>"), or it is the middle button that is held down, ignore
    // drag/release events.
    اگر (!is_click && which_button == MOUSE_میانه)
	بازگشت FALSE;

    اگر (oap != پوچ)
	regname = oap->regname;
    وگرنه
	regname = 0;

    // Middle mouse button does a 'put' of the selected text
    اگر (which_button == MOUSE_میانه)
    {
	اگر (State == MODE_NORMAL)
	{
	    // If an operator was pending, we don't know what the user wanted
	    // to do. Go back to normal mode: Clear the operator and beep().
	    اگر (oap != پوچ && oap->op_type != OP_NOP)
	    {
		clearopbeep(oap);
		بازگشت FALSE;
	    }

	    // If visual was active, yank the highlighted text and put it
	    // before the mouse pointer position.
	    // In Select mode replace the highlighted text with the clipboard.
	    اگر (VIsual_active)
	    {
		اگر (VIsual_select)
		{
		    stuffcharReadbuff(Ctrl_G);
		    stuffReadbuff((char_u *)"\"+p");
		}
		وگرنه
		{
		    stuffcharReadbuff('y');
		    stuffcharReadbuff(K_MIDDLEMOUSE);
		}
		do_always = TRUE;	// ignore 'mouse' setting next time
		بازگشت FALSE;
	    }
	    // The rest is below jump_to_mouse()
	}

	وگرنه اگر ((State & MODE_درج) == 0)
	    بازگشت FALSE;

	// Middle click in insert mode doesn't move the mouse, just insert the
	// contents of a register.  '.' register is special, can't insert that
	// with do_put().
	// Also paste at the cursor if the current mode isn't in 'mouse' (only
	// happens for the GUI).
	اگر ((State & MODE_درج) || !mouse_has(MOUSE_NORMAL))
	{
	    اگر (regname == '.')
		درج_reg(regname, TRUE);
	    وگرنه
	    {
#ifdef FEAT_CLIPBOARD
		اگر (clip_star.available && regname == 0)
		    regname = '*';
#endif
		اگر ((State & REPLACE_FLAG) && !yank_register_mline(regname))
		    درج_reg(regname, TRUE);
		وگرنه
		{
		    do_put(regname, پوچ, BACKWARD, 1L,
						      fixindent | PUT_CURSEND);

		    // Repeat it with CTRL-R CTRL-O r or CTRL-R CTRL-P r
		    AppendCharToRedobuff(Ctrl_R);
		    AppendCharToRedobuff(fixindent ? Ctrl_P : Ctrl_O);
		    AppendCharToRedobuff(regname == 0 ? '"' : regname);
		}
	    }
	    بازگشت FALSE;
	}
    }

    // When dragging or button-up stay in the same window.
    اگر (!is_click)
	jump_flags |= MOUSE_FOCUS | MOUSE_DID_MOVE;

    start_visual.lnum = 0;

    ساختار tabpage_label_info {
	bool is_panel;	    // label type. true: tabpanel, false: tab line
	bool just_in;	    // just in tabpage label area
	bool just_click;    // just click tabpage label area
	صحیح nr;		    // tabpage number
    } tp_label = { false, false, false, 0 };

    // Check for clicking in the tab page panel.
#اگر defined(FEAT_TABPANEL)
    اگر (mouse_col < firstwin->w_wincol
		|| mouse_col >= firstwin->w_wincol + topframe->fr_width)
    {
	// A click on the scrollbar column starts a drag interaction and
	// preempts tab-selection.
	اگر (is_click && !is_drag && mouse_on_tabpanel_scrollbar())
	{
	    in_tabpanel_scrollbar = TRUE;
	    tabpanel_drag_scrollbar(mouse_row);
	    بازگشت FALSE;
	}

	// Dispatch 'tabpanel' %[FuncName] click regions before falling through
	// to tab-page selection.  On drag events fall through to the normal
	// tab-drag handling.
	اگر (is_click && !is_drag
		&& stl_click_handler_regions(tabpanel_stl_click,
					    tabpanel_stl_click_شمار,
					    0, (char_u *)"tabpanel",
					    mouse_row, mouse_col,
					    which_button, mod_mask))
	    بازگشت FALSE;

	tp_label.is_panel = true;
	tp_label.just_in = true;
	tp_label.nr = get_tabpagenr_on_tabpanel();

	// click in a tab selects that tab page
	اگر (is_click && cmdwin_type == 0)
	    tp_label.just_click = true;
    }
    وگرنه
#endif
    // Check for clicking in the tab page line.
    اگر (TabPageIdxs != پوچ && mouse_row == 0 && firstwin->w_winrow > 0)
    {
	// Dispatch 'tabline' %[FuncName] click regions before falling through
	// to tab-page selection.  On drag events fall through to the normal
	// tab-drag handling.
	اگر (is_click && !is_drag
		&& stl_click_handler_regions(tabline_stl_click,
					    tabline_stl_click_شمار,
					    0, (char_u *)"tabline",
					    mouse_row, mouse_col,
					    which_button, mod_mask))
	    بازگشت FALSE;

	tp_label.just_in = true;
	tp_label.nr = TabPageIdxs[mouse_col];

	// click in a tab selects that tab page
	اگر (is_click && cmdwin_type == 0
		&& mouse_col < firstwin->w_wincol + topframe->fr_width)
	    tp_label.just_click = true;
    }

    اگر (tp_label.just_in)
    {
	اگر (is_drag)
	{
	    اگر (in_tabpanel || in_tab_سطر)
	    {
		c1 = tp_label.nr;
		tabpage_move(c1 <= 0 ? 9999 : c1 < tabpage_index(curtab)
							    ? c1 - 1 : c1);
	    }
	    بازگشت FALSE;
	}

	اگر (tp_label.just_click)
	{
	    اگر (tp_label.is_panel)
		in_tabpanel = TRUE;
	    وگرنه
		in_tab_سطر = TRUE;
	    c1 = tp_label.nr;
	    اگر (c1 >= 0)
	    {
		اگر ((mod_mask & MOD_MASK_MULTI_CLICK) == MOD_MASK_2CLICK
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
		وگرنه
		{
		    // Go to specified tab page, or next one if not clicking
		    // on a label.
		    goto_tabpage(c1);

		    // It's like clicking on the status line of a window.
		    اگر (curwin != old_curwin)
			end_visual_mode_keep_button();
		}
	    }
	    وگرنه
	    {
		tabpage_T	*tp;

		// Close the current or specified tab page.
		اگر (c1 == -999)
		    tp = curtab;
		وگرنه
		    tp = find_tabpage(-c1);
		اگر (tp == curtab)
		{
		    اگر (first_tabpage->tp_next != پوچ)
			tabpage_close(FALSE);
		}
		وگرنه اگر (tp != پوچ)
		    tabpage_close_other(tp, FALSE);
	    }
	}
	بازگشت TRUE;
    }
    وگرنه اگر (is_drag && (in_tabpanel || (in_tab_سطر && TabPageIdxs != پوچ)))
    {
#اگر defined(FEAT_TABPANEL)
	اگر (in_tabpanel)
	    c1 = get_tabpagenr_on_tabpanel();
	وگرنه
#endif
	    c1 = TabPageIdxs[mouse_col];
	tabpage_move(c1 <= 0 ? 9999 : c1 - 1);
	بازگشت FALSE;
    }

    // When 'mousemodel' is "popup" or "popup_setpos", translate mouse events:
    // right button up   -> pop-up menu
    // shift-left button -> right button
    // alt-left button   -> alt-right button
    اگر (mouse_model_popup())
    {
	اگر (which_button == MOUSE_راست
			    && !(mod_mask & (MOD_MASK_SHIFT | MOD_MASK_CTRL)))
	{
#ifdef USE_POPUP_SETPOS
# ifdef FEAT_GUI
	    اگر (gui.in_use)
	    {
#  اگر defined(FEAT_GUI_MOTIF) || defined(FEAT_GUI_GTK) \
			  || defined(FEAT_GUI_PHOTON)
		اگر (!is_click)
		    // Ignore right button release events, only shows the popup
		    // menu on the button down event.
		    بازگشت FALSE;
#  endif
#  اگر defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_HAIKU)
		اگر (is_click || is_drag)
		    // Ignore right button down and drag mouse events.  Windows
		    // only shows the popup menu on the button up event.
		    بازگشت FALSE;
#  endif
	    }
# endif
# اگر defined(FEAT_GUI) && defined(FEAT_TERM_POPUP_MENU)
	    وگرنه
# endif
# اگر defined(FEAT_TERM_POPUP_MENU)
	    اگر (!is_click)
		// Ignore right button release events, only shows the popup
		// menu on the button down event.
		بازگشت FALSE;
# endif

	    jump_flags = 0;
	    اگر (STRCMP(p_mousem, "popup_setpos") == 0)
	    {
		// First set the cursor position before showing the popup
		// menu.
		اگر (VIsual_active)
		{
		    pos_T    m_pos;

		    // set MOUSE_MAY_STOP_VIS if we are outside the
		    // selection or the current window (might have false
		    // negative here)
		    اگر (mouse_row < curwin->w_winrow
			 || mouse_row
				  > (curwin->w_winrow + curwin->w_height))
			jump_flags = MOUSE_MAY_STOP_VIS;
		    وگرنه اگر (get_fpos_of_mouse(&m_pos) != IN_BUFFER)
			jump_flags = MOUSE_MAY_STOP_VIS;
		    وگرنه
		    {
			اگر (VIsual_mode == 'V')
			{
			    اگر ((curwin->w_cursor.lnum <= VIsual.lnum
				    && (m_pos.lnum < curwin->w_cursor.lnum
					|| VIsual.lnum < m_pos.lnum))
				|| (VIsual.lnum < curwin->w_cursor.lnum
				    && (m_pos.lnum < VIsual.lnum
					|| curwin->w_cursor.lnum < m_pos.lnum)))
			    {
				jump_flags = MOUSE_MAY_STOP_VIS;
			    }
			}
			وگرنه اگر ((LTOREQ_POS(curwin->w_cursor, VIsual)
				    && (LT_POS(m_pos, curwin->w_cursor)
					|| LT_POS(VIsual, m_pos)))
				|| (LT_POS(VIsual, curwin->w_cursor)
				    && (LT_POS(m_pos, VIsual)
				      || LT_POS(curwin->w_cursor, m_pos))))
			{
			    jump_flags = MOUSE_MAY_STOP_VIS;
			}
			وگرنه اگر (VIsual_mode == Ctrl_V)
			{
			    getvcols(curwin, &curwin->w_cursor, &VIsual,
						     &leftcol, &rightcol, 0);
			    getvcol(curwin, &m_pos, پوچ, &m_pos.col, پوچ, 0);
			    اگر (m_pos.col < leftcol || m_pos.col > rightcol)
				jump_flags = MOUSE_MAY_STOP_VIS;
			}
		    }
		}
		وگرنه
		    jump_flags = MOUSE_MAY_STOP_VIS;
	    }
	    اگر (jump_flags)
	    {
		jump_flags = jump_to_mouse(jump_flags, پوچ, which_button);
		update_curbuf(VIsual_active ? UPD_INVERTED : UPD_VALID);
		setcursor();
		out_flush();    // Update before showing popup menu
	    }
# ifdef FEAT_MENU
	    show_popupmenu();
	    got_click = FALSE;	// ignore release events
# endif
	    بازگشت (jump_flags & CURSOR_MOVED) != 0;
#وگرنه
	    بازگشت FALSE;
#endif
	}
	اگر (which_button == MOUSE_چپ
				&& (mod_mask & (MOD_MASK_SHIFT|MOD_MASK_ALT)))
	{
	    which_button = MOUSE_راست;
	    mod_mask &= ~MOD_MASK_SHIFT;
	}
    }

    اگر ((State & (MODE_NORMAL | MODE_درج))
			    && !(mod_mask & (MOD_MASK_SHIFT | MOD_MASK_CTRL)))
    {
	اگر (which_button == MOUSE_چپ)
	{
	    اگر (is_click)
	    {
		// stop Visual mode for a left click in a window, but not when
		// on a status line
		اگر (VIsual_active)
		    jump_flags |= MOUSE_MAY_STOP_VIS;
	    }
	    وگرنه اگر (mouse_has(MOUSE_VISUAL))
		jump_flags |= MOUSE_MAY_VIS;
	}
	وگرنه اگر (which_button == MOUSE_راست)
	{
	    اگر (is_click && VIsual_active)
	    {
		// Remember the start and end of visual before moving the
		// cursor.
		اگر (LT_POS(curwin->w_cursor, VIsual))
		{
		    start_visual = curwin->w_cursor;
		    end_visual = VIsual;
		}
		وگرنه
		{
		    start_visual = VIsual;
		    end_visual = curwin->w_cursor;
		}
	    }
	    jump_flags |= MOUSE_FOCUS;
	    اگر (mouse_has(MOUSE_VISUAL))
		jump_flags |= MOUSE_MAY_VIS;
	}
    }

    // If an operator is pending, ignore all drags and releases until the
    // next mouse click.
    اگر (!is_drag && oap != پوچ && oap->op_type != OP_NOP)
    {
	got_click = FALSE;
	oap->motion_type = MCHAR;
    }

    // When releasing the button let jump_to_mouse() know.
    اگر (!is_click && !is_drag)
	jump_flags |= MOUSE_RELEASED;

    // JUMP!
    jump_flags = jump_to_mouse(jump_flags,
			oap == پوچ ? پوچ : &(oap->inclusive), which_button);

#ifdef FEAT_MENU
    // A click in the window toolbar has no side effects.
    اگر (jump_flags & MOUSE_WINBAR)
	بازگشت FALSE;
#endif
    moved = (jump_flags & CURSOR_MOVED);
    in_status_سطر = (jump_flags & IN_STATUS_سطر);
    in_sep_سطر = (jump_flags & IN_SEP_سطر);

    // Check for statusline click handler early, before visual mode or
    // other button-specific handling can interfere.
    اگر (in_status_سطر && is_click && !is_drag
	    && stl_click_handler(dragwin, mouse_row, mouse_col,
						which_button, mod_mask))
    {
#ifdef FEAT_MOUSESHAPE
	اگر (!drag_status_سطر)
	{
	    drag_status_سطر = TRUE;
	    update_mouseshape(-1);
	}
#endif
	بازگشت FALSE;
    }

#ifdef FEAT_NETBEANS_INTG
    اگر (isNetbeansBuffer(curbuf)
			    && !(jump_flags & (IN_STATUS_سطر | IN_SEP_سطر)))
    {
	صحیح کلید = KEY2TERMCAP1(c);

	اگر (کلید == (صحیح)KE_LEFTRELEASE || کلید == (صحیح)KE_MIDDLERELEASE
					       || کلید == (صحیح)KE_RIGHTRELEASE)
	    netbeans_button_release(which_button);
    }
#endif

    // When jumping to another window, clear a pending operator.  That's a bit
    // friendlier than beeping and not jumping to that window.
    اگر (curwin != old_curwin && oap != پوچ && oap->op_type != OP_NOP)
	clearop(oap);

#ifdef FEAT_FOLDING
    اگر (mod_mask == 0
	    && !is_drag
	    && (jump_flags & (MOUSE_FOLD_CLOSE | MOUSE_FOLD_OPEN))
	    && which_button == MOUSE_چپ)
    {
	// open or close a fold at this line
	اگر (jump_flags & MOUSE_FOLD_OPEN)
	    openFold(curwin->w_cursor.lnum, 1L);
	وگرنه
	    closeFold(curwin->w_cursor.lnum, 1L);
	// don't move the cursor if still in the same window
	اگر (curwin == old_curwin)
	    curwin->w_cursor = save_cursor;
    }
#endif

#اگر defined(FEAT_CLIPBOARD)
    اگر ((jump_flags & IN_OTHER_WIN) && !VIsual_active &&
	    (clip_star.available || clip_plus.available))
    {
	clip_modeless(which_button, is_click, is_drag);
	بازگشت FALSE;
    }
#endif

    // Set global flag that we are extending the Visual area with mouse
    // dragging; temporarily minimize 'scrolloff'.
    اگر (VIsual_active && is_drag && get_scrolloff_مقدار())
    {
	// In the very first line, allow scrolling one line
	اگر (mouse_row == 0)
	    mouse_dragging = 2;
	وگرنه
	    mouse_dragging = 1;
    }

    // When dragging the mouse above the window, scroll down.
    اگر (is_drag && mouse_row < 0 && !in_status_سطر)
    {
	scroll_redraw(FALSE, 1L);
	mouse_row = 0;
    }

    اگر (start_visual.lnum)		// right click in visual mode
    {
       // When ALT is pressed make Visual mode blockwise.
       اگر (mod_mask & MOD_MASK_ALT)
	   VIsual_mode = Ctrl_V;

	// In Visual-block mode, divide the area in four, pick up the corner
	// that is in the quarter that the cursor is in.
	اگر (VIsual_mode == Ctrl_V)
	{
	    getvcols(curwin, &start_visual, &end_visual,
						    &leftcol, &rightcol, 0);
	    اگر (curwin->w_curswant > (leftcol + rightcol) / 2)
		end_visual.col = leftcol;
	    وگرنه
		end_visual.col = rightcol;
	    اگر (curwin->w_cursor.lnum >=
				    (start_visual.lnum + end_visual.lnum) / 2)
		end_visual.lnum = start_visual.lnum;

	    // move VIsual to the right column
	    start_visual = curwin->w_cursor;	    // save the cursor pos
	    curwin->w_cursor = end_visual;
	    coladvance(end_visual.col);
	    VIsual = curwin->w_cursor;
	    curwin->w_cursor = start_visual;	    // restore the cursor
	}
	وگرنه
	{
	    // If the click is before the start of visual, change the start.
	    // If the click is after the end of visual, change the end.  If
	    // the click is inside the visual, change the closest side.
	    اگر (LT_POS(curwin->w_cursor, start_visual))
		VIsual = end_visual;
	    وگرنه اگر (LT_POS(end_visual, curwin->w_cursor))
		VIsual = start_visual;
	    وگرنه
	    {
		// In the same line, compare column number
		اگر (end_visual.lnum == start_visual.lnum)
		{
		    اگر (curwin->w_cursor.col - start_visual.col >
				    end_visual.col - curwin->w_cursor.col)
			VIsual = start_visual;
		    وگرنه
			VIsual = end_visual;
		}

		// In different lines, compare line number
		وگرنه
		{
		    diff = (curwin->w_cursor.lnum - start_visual.lnum) -
				(end_visual.lnum - curwin->w_cursor.lnum);

		    اگر (diff > 0)		// closest to end
			VIsual = start_visual;
		    وگرنه اگر (diff < 0)	// closest to start
			VIsual = end_visual;
		    وگرنه			// in the middle line
		    {
			اگر (curwin->w_cursor.col <
					(start_visual.col + end_visual.col) / 2)
			    VIsual = end_visual;
			وگرنه
			    VIsual = start_visual;
		    }
		}
	    }
	}
    }
    // If Visual mode started in insert mode, execute "CTRL-O"
    وگرنه اگر ((State & MODE_درج) && VIsual_active)
	stuffcharReadbuff(Ctrl_O);

    // Middle mouse click: Put text before cursor.
    اگر (which_button == MOUSE_میانه)
    {
#ifdef FEAT_CLIPBOARD
	اگر (clip_star.available && regname == 0)
	    regname = '*';
#endif
	اگر (yank_register_mline(regname))
	{
	    اگر (mouse_past_bottom)
		dir = FORWARD;
	}
	وگرنه اگر (mouse_past_eol)
	    dir = FORWARD;

	اگر (fixindent)
	{
	    c1 = (dir == BACKWARD) ? '[' : ']';
	    c2 = 'p';
	}
	وگرنه
	{
	    c1 = (dir == FORWARD) ? 'p' : 'P';
	    c2 = NUL;
	}
	prep_redo(regname, شمار, NUL, c1, NUL, c2, NUL);

	// Remember where the paste started, so in edit() Insstart can be set
	// to this position
	اگر (restart_edit != 0)
	    where_paste_started = curwin->w_cursor;
	do_put(regname, پوچ, dir, شمار, fixindent | PUT_CURSEND);
    }

#اگر defined(FEAT_QUICKFIX)
    // Ctrl-Mouse click or double click in a quickfix window jumps to the
    // error under the mouse pointer.
    وگرنه اگر (((mod_mask & MOD_MASK_CTRL)
		|| (mod_mask & MOD_MASK_MULTI_CLICK) == MOD_MASK_2CLICK)
	    && bt_quickfix(curbuf))
    {
	اگر (curwin->w_llist_ref == پوچ)	// quickfix window
	    do_cmdline_cmd((char_u *)".cc");
	وگرنه					// location list window
	    do_cmdline_cmd((char_u *)".ll");
	got_click = FALSE;		// ignore drag&release now
    }
#endif

    // Ctrl-Mouse click (or double click in a help window) jumps to the tag
    // under the mouse pointer.
    وگرنه اگر ((mod_mask & MOD_MASK_CTRL) || (curbuf->b_help
		     && (mod_mask & MOD_MASK_MULTI_CLICK) == MOD_MASK_2CLICK))
    {
	اگر (State & MODE_درج)
	    stuffcharReadbuff(Ctrl_O);
	stuffcharReadbuff(Ctrl_RSB);
	got_click = FALSE;		// ignore drag&release now
    }

    // Shift-Mouse click searches for the next occurrence of the word under
    // the mouse pointer
    وگرنه اگر ((mod_mask & MOD_MASK_SHIFT))
    {
	اگر ((State & MODE_درج) || (VIsual_active && VIsual_select))
	    stuffcharReadbuff(Ctrl_O);
	اگر (which_button == MOUSE_چپ)
	    stuffcharReadbuff('*');
	وگرنه	// MOUSE_RIGHT
	    stuffcharReadbuff('#');
    }

    // Handle double clicks, unless on status line
    وگرنه اگر (in_status_سطر)
    {
#ifdef FEAT_MOUSESHAPE
	اگر ((is_drag || is_click) && !drag_status_سطر)
	{
	    drag_status_سطر = TRUE;
	    update_mouseshape(-1);
	}
#endif
    }
    وگرنه اگر (in_sep_سطر)
    {
#ifdef FEAT_MOUSESHAPE
	اگر ((is_drag || is_click) && !drag_sep_سطر)
	{
	    drag_sep_سطر = TRUE;
	    update_mouseshape(-1);
	}
#endif
    }
    وگرنه اگر ((mod_mask & MOD_MASK_MULTI_CLICK)
				       && (State & (MODE_NORMAL | MODE_درج))
	     && mouse_has(MOUSE_VISUAL))
    {
	اگر (is_click || !VIsual_active)
	{
	    اگر (VIsual_active)
		orig_cursor = VIsual;
	    وگرنه
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
	    اگر ((mod_mask & MOD_MASK_MULTI_CLICK) == MOD_MASK_2CLICK)
	    {
		// Double click with ALT pressed makes it blockwise.
		اگر (mod_mask & MOD_MASK_ALT)
		    VIsual_mode = Ctrl_V;
		وگرنه
		    VIsual_mode = 'v';
	    }
	    وگرنه اگر ((mod_mask & MOD_MASK_MULTI_CLICK) == MOD_MASK_3CLICK)
		VIsual_mode = 'V';
	    وگرنه اگر ((mod_mask & MOD_MASK_MULTI_CLICK) == MOD_MASK_4CLICK)
		VIsual_mode = Ctrl_V;
#ifdef FEAT_CLIPBOARD
	    // Make sure the clipboard gets updated.  Needed because start and
	    // end may still be the same, and the selection needs to be owned
	    clip_star.vmode = NUL;
#endif
	}
	// A double click selects a word or a block.
	اگر ((mod_mask & MOD_MASK_MULTI_CLICK) == MOD_MASK_2CLICK)
	{
	    pos_T	*pos = پوچ;
	    صحیح		gc;

	    اگر (is_click)
	    {
		// If the character under the cursor (skipping white space) is
		// not a word character, try finding a match and select a (),
		// {}, [], #if/#endif, etc. block.
		end_visual = curwin->w_cursor;
		وقتی (gc = gchar_pos(&end_visual), VIM_ISWHITE(gc))
		    inc(&end_visual);
		اگر (oap != پوچ)
		    oap->motion_type = MCHAR;
		اگر (oap != پوچ
			&& VIsual_mode == 'v'
			&& !vim_iswordc(gchar_pos(&end_visual))
			&& EQUAL_POS(curwin->w_cursor, VIsual)
			&& (pos = findmatch(oap, NUL)) != پوچ)
		{
		    curwin->w_cursor = *pos;
		    اگر (oap->motion_type == MLINE)
			VIsual_mode = 'V';
		    وگرنه اگر (*p_sel == 'e')
		    {
			اگر (LT_POS(curwin->w_cursor, VIsual))
			    ++VIsual.col;
			وگرنه
			    ++curwin->w_cursor.col;
		    }
		}
	    }

	    اگر (pos == پوچ && (is_click || is_drag))
	    {
		// When not found a match or when dragging: extend to include
		// a word.
		اگر (LT_POS(curwin->w_cursor, orig_cursor))
		{
		    find_start_of_word(&curwin->w_cursor);
		    find_end_of_word(&VIsual);
		}
		وگرنه
		{
		    find_start_of_word(&VIsual);
		    اگر (*p_sel == 'e' && *ml_get_cursor() != NUL)
			curwin->w_cursor.col +=
					 (*mb_ptr2len)(ml_get_cursor());
		    find_end_of_word(&curwin->w_cursor);
		}
	    }
	    curwin->w_set_curswant = true;
	}
	اگر (is_click)
	    redraw_curbuf_later(UPD_INVERTED);	// update the inversion
    }
    وگرنه اگر (VIsual_active && !old_active)
    {
	اگر (mod_mask & MOD_MASK_ALT)
	    VIsual_mode = Ctrl_V;
	وگرنه
	    VIsual_mode = 'v';
    }

    // If Visual mode changed show it later.
    اگر ((!VIsual_active && old_active && mode_displayed)
	    || (VIsual_active && p_smd && پیام_silent == 0
				 && (!old_active || VIsual_mode != old_mode)))
	redraw_cmdline = TRUE;

    بازگشت moved;
}

    تهی
ins_mouse(صحیح c)
{
    pos_T	tpos;
    win_T	*old_curwin = curwin;

#ifdef FEAT_GUI
    // When GUI is active, also move/paste when 'mouse' is empty
    اگر (!gui.in_use)
#endif
	اگر (!mouse_has(MOUSE_درج))
	    بازگشت;

    undisplay_dollar();
    tpos = curwin->w_cursor;
    اگر (do_mouse(پوچ, c, BACKWARD, 1L, 0))
    {
	win_T	*new_curwin = curwin;

	اگر (curwin != old_curwin && win_valid(old_curwin))
	{
	    // Mouse took us to another window.  We need to go back to the
	    // previous one to stop insert there properly.
	    curwin = old_curwin;
	    curbuf = curwin->w_buffer;
#ifdef FEAT_JOB_CHANNEL
	    اگر (bt_prompt(curbuf))
		// Restart Insert mode when re-entering the prompt buffer.
		curbuf->b_prompt_درج = 'A';
#endif
	}
	start_arrow(curwin == old_curwin ? &tpos : پوچ);
	اگر (curwin != new_curwin && win_valid(new_curwin))
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
    ایستا تهی
do_mousescroll(cmdarg_T *cap)
{
    صحیح shift_or_ctrl = mod_mask & (MOD_MASK_SHIFT | MOD_MASK_CTRL);

#ifdef FEAT_TERMINAL
    اگر (term_use_loop())
	// This window is a terminal window, send the mouse event there.
	// Set "typed" to FALSE to avoid an endless loop.
	send_کلیدها_to_term(curbuf->b_term, cap->cmdchar, mod_mask, FALSE);
    وگرنه
#endif
    اگر (cap->arg == MSCR_UP || cap->arg == MSCR_DOWN)
    {
	// Vertical scrolling
	اگر (!(State & MODE_درج) && (mouse_vert_step < 0 || shift_or_ctrl))
	{
	    // whole page up or down
	    pagescroll(cap->arg == MSCR_UP ? FORWARD : BACKWARD, 1L, FALSE);
	}
	وگرنه
	{
	    اگر (mouse_vert_step < 0 || shift_or_ctrl)
	    {
		// whole page up or down
		cap->count1 = (long)(curwin->w_botline - curwin->w_topline);
	    }
	    // Don't scroll more than half the window height.
	    وگرنه اگر (curwin->w_height < mouse_vert_step * 2)
	    {
		cap->count1 = curwin->w_height / 2;
		اگر (cap->count1 == 0)
		    cap->count1 = 1;
	    }
	    وگرنه
	    {
		cap->count1 = mouse_vert_step;
	    }
	    cap->count0 = cap->count1;
	    nv_scroll_سطر(cap);
	}

#ifdef FEAT_PROP_POPUP
	اگر (WIN_IS_POPUP(curwin))
	    popup_set_firstline(curwin);
#endif
    }
    وگرنه
    {
	// Horizontal scrolling
	long step = (mouse_hor_step < 0 || shift_or_ctrl)
					    ? curwin->w_width : mouse_hor_step;
	long leftcol = curwin->w_leftcol
				     + (cap->arg == MSCR_راست ? -step : step);
	اگر (leftcol < 0)
	    leftcol = 0;
	do_mousescroll_horiz((long_u)leftcol);
    }
    may_trigger_win_scrolled_resized();
}

/*
 * Insert mode implementation for scrolling in direction "dir", which is
 * one of the MSCR_ values.
 */
    تهی
ins_mousescroll(صحیح dir)
{
    cmdarg_T	cap;
    oparg_T	oa;
    CLEAR_FIELD(cap);
    clear_oparg(&oa);
    cap.oap = &oa;
    cap.arg = dir;

#ifdef FEAT_TABPANEL
    اگر (mouse_row >= 0 && mouse_col >= 0
	    && (dir == MSCR_UP || dir == MSCR_DOWN)
	    && mouse_on_tabpanel())
    {
	(تهی)tabpanel_scroll(dir == MSCR_UP ? 1 : -1,
		mouse_vert_step > 0 ? mouse_vert_step : 3);
	بازگشت;
    }
#endif

    انتخاب (dir)
    {
	حالت MSCR_UP:
	    cap.cmdchar = K_MOUSEUP;
	    بشکن;
	حالت MSCR_DOWN:
	    cap.cmdchar = K_MOUSEDOWN;
	    بشکن;
	حالت MSCR_چپ:
	    cap.cmdchar = K_MOUSELEFT;
	    بشکن;
	حالت MSCR_راست:
	    cap.cmdchar = K_MOUSERIGHT;
	    بشکن;
	پیشفرض:
	    siemsg("Invalid ins_mousescroll() argument: %d", dir);
    }

    win_T *old_curwin = curwin;
    اگر (mouse_row >= 0 && mouse_col >= 0)
    {
	// Find the window at the mouse pointer coordinates.
	// NOTE: Must restore "curwin" to "old_curwin" before returning!
	صحیح row = mouse_row;
	صحیح col = mouse_col;
	curwin = mouse_find_win(&row, &col, FIND_POPUP);
	اگر (curwin == پوچ)
	{
	    curwin = old_curwin;
	    بازگشت;
	}
	curbuf = curwin->w_buffer;
    }

    اگر (curwin == old_curwin)
    {
	// Don't scroll the current window if the popup menu is visible.
	اگر (pum_visible())
	    بازگشت;

	undisplay_dollar();
    }

    linenr_T	orig_topline = curwin->w_topline;
    colnr_T	orig_leftcol = curwin->w_leftcol;
    pos_T	orig_cursor = curwin->w_cursor;

    // Call the common mouse scroll function shared with other modes.
    do_mousescroll(&cap);

    صحیح did_scroll = (orig_topline != curwin->w_topline
		   || orig_leftcol != curwin->w_leftcol);

    curwin->w_redr_status = true;
    curwin = old_curwin;
    curbuf = curwin->w_buffer;

    // If the window actually scrolled and the popup menu may overlay the
    // window, need to redraw it.
    اگر (did_scroll && pum_visible())
    {
	// TODO: Would be more efficient to only redraw the windows that are
	// overlapped by the popup menu.
	redraw_all_later(UPD_NOT_VALID);
	ins_compl_show_pum();
    }

    اگر (!EQUAL_POS(curwin->w_cursor, orig_cursor))
    {
	start_arrow(&orig_cursor);
	set_can_cindent(TRUE);
    }
}

#اگر defined(FEAT_PROP_POPUP)
/*
 * Command-line mode implementation for scrolling in direction "dir", which is
 * one of the MSCR_ values.  Scrolls the completion info popup when the mouse
 * pointer is on top of it.
 * Returns TRUE when the info popup was scrolled.
 */
    صحیح
cmdline_mousescroll(صحیح dir)
{
    cmdarg_T	cap;
    oparg_T	oa;

    CLEAR_FIELD(cap);
    clear_oparg(&oa);
    cap.oap = &oa;
    cap.arg = dir;

    انتخاب (dir)
    {
	حالت MSCR_UP:	 cap.cmdchar = K_MOUSEUP; بشکن;
	حالت MSCR_DOWN:	 cap.cmdchar = K_MOUSEDOWN; بشکن;
	حالت MSCR_چپ:	 cap.cmdchar = K_MOUSELEFT; بشکن;
	حالت MSCR_راست: cap.cmdchar = K_MOUSERIGHT; بشکن;
    }

    اگر (mouse_row < 0 || mouse_col < 0)
	بازگشت FALSE;

    صحیح	    row = mouse_row;
    صحیح	    col = mouse_col;
    win_T   *wp;

    // Only scroll when the mouse is on top of the info popup.
    wp = mouse_find_win(&row, &col, FIND_POPUP);
    اگر (wp == پوچ || !WIN_IS_POPUP(wp) || !(wp->w_popup_flags & POPF_INFO)
							|| !wp->w_has_scrollbar)
	بازگشت FALSE;

    win_T   *old_curwin = curwin;

    curwin = wp;
    curbuf = wp->w_buffer;
    // Call the common mouse scroll function shared with other modes.
    do_mousescroll(&cap);
    curwin = old_curwin;
    curbuf = curwin->w_buffer;

    // Cmdline mode doesn't normally call update_screen(), so redraw the
    // completion popup menu, which also repaints the info popup.
    اگر (cmdline_pum_active())
	cmdline_pum_display();
    بازگشت TRUE;
}
#endif

/*
 * Return TRUE if "c" is a mouse key.
 */
    صحیح
is_mouse_کلید(صحیح c)
{
    بازگشت c == K_LEFTMOUSE
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

ایستا ساختار mousetable
{
    صحیح	    pseudo_code;	// Code for pseudo mouse event
    صحیح	    button;		// Which mouse button is it?
    صحیح	    is_click;		// Is it a mouse button click event?
    صحیح	    is_drag;		// Is it a mouse drag event?
} mouse_table[] =
{
    {(صحیح)KE_LEFTMOUSE,		MOUSE_چپ,	TRUE,	FALSE},
#ifdef FEAT_GUI
    {(صحیح)KE_LEFTMOUSE_NM,	MOUSE_چپ,	TRUE,	FALSE},
#endif
    {(صحیح)KE_LEFTDRAG,		MOUSE_چپ,	FALSE,	TRUE},
    {(صحیح)KE_LEFTRELEASE,	MOUSE_چپ,	FALSE,	FALSE},
#ifdef FEAT_GUI
    {(صحیح)KE_LEFTRELEASE_NM,	MOUSE_چپ,	FALSE,	FALSE},
#endif
    {(صحیح)KE_MIDDLEMOUSE,	MOUSE_میانه,	TRUE,	FALSE},
    {(صحیح)KE_MIDDLEDRAG,	MOUSE_میانه,	FALSE,	TRUE},
    {(صحیح)KE_MIDDLERELEASE,	MOUSE_میانه,	FALSE,	FALSE},
    {(صحیح)KE_RIGHTMOUSE,	MOUSE_راست,	TRUE,	FALSE},
    {(صحیح)KE_RIGHTDRAG,		MOUSE_راست,	FALSE,	TRUE},
    {(صحیح)KE_RIGHTRELEASE,	MOUSE_راست,	FALSE,	FALSE},
    {(صحیح)KE_X1MOUSE,		MOUSE_X1,	TRUE,	FALSE},
    {(صحیح)KE_X1DRAG,		MOUSE_X1,	FALSE,	TRUE},
    {(صحیح)KE_X1RELEASE,		MOUSE_X1,	FALSE,	FALSE},
    {(صحیح)KE_X2MOUSE,		MOUSE_X2,	TRUE,	FALSE},
    {(صحیح)KE_X2DRAG,		MOUSE_X2,	FALSE,	TRUE},
    {(صحیح)KE_X2RELEASE,		MOUSE_X2,	FALSE,	FALSE},
    // DRAG without CLICK
    {(صحیح)KE_MOUSEMOVE,		MOUSE_RELEASE,	FALSE,	TRUE},
    // RELEASE without CLICK
    {(صحیح)KE_IGNORE,		MOUSE_RELEASE,	FALSE,	FALSE},
    {0,				0,		0,	0},
};

/*
 * Look up the given mouse code to return the relevant information in the other
 * arguments.  Return which button is down or was released.
 */
    صحیح
get_mouse_button(صحیح code, صحیح *is_click, صحیح *is_drag)
{
    صحیح	    i;

    برای (i = 0; mouse_table[i].pseudo_code; i++)
	اگر (code == mouse_table[i].pseudo_code)
	{
	    *is_click = mouse_table[i].is_click;
	    *is_drag = mouse_table[i].is_drag;
	    بازگشت mouse_table[i].button;
	}
    بازگشت 0;	    // Shouldn't get here
}

/*
 * Return the appropriate pseudo mouse event token (KE_LEFTMOUSE etc) based on
 * the given information about which mouse button is down, and whether the
 * mouse was clicked, dragged or released.
 */
    صحیح
get_pseudo_mouse_code(
    صحیح	    button,	// eg MOUSE_LEFT
    صحیح	    is_click,
    صحیح	    is_drag)
{
    صحیح	    i;

    برای (i = 0; mouse_table[i].pseudo_code; i++)
	اگر (button == mouse_table[i].button
	    && is_click == mouse_table[i].is_click
	    && is_drag == mouse_table[i].is_drag)
	{
#ifdef FEAT_GUI
	    // Trick: a non mappable left click and release has mouse_col -1
	    // or added MOUSE_COLOFF.  Used for 'mousefocus' in
	    // gui_mouse_moved()
	    اگر (mouse_col < 0 || mouse_col > MOUSE_COLOFF)
	    {
		اگر (mouse_col < 0)
		    mouse_col = 0;
		وگرنه
		    mouse_col -= MOUSE_COLOFF;
		اگر (mouse_table[i].pseudo_code == (صحیح)KE_LEFTMOUSE)
		    بازگشت (صحیح)KE_LEFTMOUSE_NM;
		اگر (mouse_table[i].pseudo_code == (صحیح)KE_LEFTRELEASE)
		    بازگشت (صحیح)KE_LEFTRELEASE_NM;
	    }
#endif
	    بازگشت mouse_table[i].pseudo_code;
	}
    بازگشت (صحیح)KE_IGNORE;	    // not recognized, ignore it
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
ایستا صحیح has_mouse_termcode = 0;

    تهی
set_mouse_termcode(
    صحیح		n,	// KS_MOUSE, KS_NETTERM_MOUSE or KS_DEC_MOUSE
    char_u	*s)
{
    char_u	name[2];

    name[0] = n;
    name[1] = KE_FILLER;
    add_termcode(name, s, FALSE);
#ifdef FEAT_MOUSE_JSB
    اگر (n == KS_JSBTERM_MOUSE)
	has_mouse_termcode |= HMT_JSBTERM;
    وگرنه
#endif
#ifdef FEAT_MOUSE_NET
    اگر (n == KS_NETTERM_MOUSE)
	has_mouse_termcode |= HMT_NETTERM;
    وگرنه
#endif
#ifdef FEAT_MOUSE_DEC
    اگر (n == KS_DEC_MOUSE)
	has_mouse_termcode |= HMT_DEC;
    وگرنه
#endif
#ifdef FEAT_MOUSE_PTERM
    اگر (n == KS_PTERM_MOUSE)
	has_mouse_termcode |= HMT_PTERM;
    وگرنه
#endif
#ifdef FEAT_MOUSE_URXVT
    اگر (n == KS_URXVT_MOUSE)
	has_mouse_termcode |= HMT_URXVT;
    وگرنه
#endif
#ifdef FEAT_MOUSE_GPM
    اگر (n == KS_GPM_MOUSE)
	has_mouse_termcode |= HMT_GPM;
    وگرنه
#endif
    اگر (n == KS_SGR_MOUSE)
	has_mouse_termcode |= HMT_SGR;
    وگرنه اگر (n == KS_SGR_MOUSE_RELEASE)
	has_mouse_termcode |= HMT_SGR_REL;
    وگرنه
	has_mouse_termcode |= HMT_NORMAL;
}

#اگر defined(UNIX) || defined(VMS)
    تهی
del_mouse_termcode(
    صحیح		n)	// KS_MOUSE, KS_NETTERM_MOUSE or KS_DEC_MOUSE
{
    char_u	name[2];

    name[0] = n;
    name[1] = KE_FILLER;
    del_termcode(name);
# ifdef FEAT_MOUSE_JSB
    اگر (n == KS_JSBTERM_MOUSE)
	has_mouse_termcode &= ~HMT_JSBTERM;
    وگرنه
# endif
# ifdef FEAT_MOUSE_NET
    اگر (n == KS_NETTERM_MOUSE)
	has_mouse_termcode &= ~HMT_NETTERM;
    وگرنه
# endif
# ifdef FEAT_MOUSE_DEC
    اگر (n == KS_DEC_MOUSE)
	has_mouse_termcode &= ~HMT_DEC;
    وگرنه
# endif
# ifdef FEAT_MOUSE_PTERM
    اگر (n == KS_PTERM_MOUSE)
	has_mouse_termcode &= ~HMT_PTERM;
    وگرنه
# endif
# ifdef FEAT_MOUSE_URXVT
    اگر (n == KS_URXVT_MOUSE)
	has_mouse_termcode &= ~HMT_URXVT;
    وگرنه
# endif
# ifdef FEAT_MOUSE_GPM
    اگر (n == KS_GPM_MOUSE)
	has_mouse_termcode &= ~HMT_GPM;
    وگرنه
# endif
    اگر (n == KS_SGR_MOUSE)
	has_mouse_termcode &= ~HMT_SGR;
    وگرنه اگر (n == KS_SGR_MOUSE_RELEASE)
	has_mouse_termcode &= ~HMT_SGR_REL;
    وگرنه
	has_mouse_termcode &= ~HMT_NORMAL;
}
#endif

/*
 * setmouse() - switch mouse on/off depending on current mode and 'mouse'
 */
    تهی
setmouse(تهی)
{
    صحیح	    checkfor;

#ifdef FEAT_MOUSESHAPE
    update_mouseshape(-1);
#endif

    // Should be outside proc, but may break MOUSESHAPE
#ifdef FEAT_GUI
    // In the GUI the mouse is always enabled.
    اگر (gui.in_use)
	بازگشت;
#endif
    // be quick when mouse is off
    اگر (*p_mouse == NUL || has_mouse_termcode == 0)
	بازگشت;

    // don't switch mouse on when not in raw mode (Ex mode)
    اگر (cur_tmode != TMODE_RAW)
    {
	mch_setmouse(FALSE);
	بازگشت;
    }

    اگر (VIsual_active)
	checkfor = MOUSE_VISUAL;
    وگرنه اگر (State == MODE_HITRETURN || State == MODE_ASKMORE
						     || State == MODE_SETWSIZE)
	checkfor = MOUSE_RETURN;
    وگرنه اگر (State & MODE_درج)
	checkfor = MOUSE_درج;
    وگرنه اگر (State & MODE_CMDLINE)
	checkfor = MOUSE_COMMAND;
    وگرنه اگر (State == MODE_CONFIRM || State == MODE_EXTERNCMD)
	checkfor = ' '; // don't use mouse for ":confirm" or ":!cmd"
    وگرنه
	checkfor = MOUSE_NORMAL;    // assume normal mode

    اگر (mouse_has(checkfor))
	mch_setmouse(TRUE);
    وگرنه
	mch_setmouse(FALSE);
}

/*
 * Return TRUE if
 * - "c" is in 'mouse', or
 * - 'a' is in 'mouse' and "c" is in MOUSE_A, or
 * - the current buffer is a help file and 'h' is in 'mouse' and we are in a
 *   normal editing mode (not at hit-return message).
 */
    صحیح
mouse_has(صحیح c)
{
    char_u	*p;

    برای (p = p_mouse; *p; ++p)
	انتخاب (*p)
	{
	    حالت 'a': اگر (vim_strchr((char_u *)MOUSE_A, c) != پوچ)
			  بازگشت TRUE;
		      بشکن;
	    حالت MOUSE_HELP: اگر (c != MOUSE_RETURN && curbuf->b_help)
				 بازگشت TRUE;
			     بشکن;
	    پیشفرض: اگر (c == *p) بازگشت TRUE; بشکن;
	}
    بازگشت FALSE;
}

/*
 * Return TRUE when 'mousemodel' is set to "popup" or "popup_setpos".
 */
    صحیح
mouse_model_popup(تهی)
{
    بازگشت (p_mousem[0] == 'p');
}

/*
 * Call a click-region callback function.
 * "regions"/"region_count" describe the resolved click regions,
 * "winid" is stored as the "winid" key in the info dict (0 for tabline).
 * Returns TRUE if the function was called and handled the click.
 */
    ایستا صحیح
stl_click_handler_regions(
	stl_click_region_T  *regions,
	صحیح		    region_شمار,
	صحیح		    winid,
	char_u		    *area_name,
	صحیح		    mrow,
	صحیح		    mcol,
	صحیح		    which_button,
	صحیح		    mods)
{
#ifdef FEAT_EVAL
    صحیح		n;
    صحیح		nclicks;
    char_u	button_str[2];
    char_u	mods_str[4];
    صحیح		mi = 0;
    dict_T	*info;
    typval_T	argvars[2];
    typval_T	rettv;
    funcexe_T	funcexe;
    صحیح		col = mcol;

    اگر (regions == پوچ || region_شمار == 0)
	بازگشت FALSE;

    // Find the click region at the given row and column.
    برای (n = 0; n < region_شمار; n++)
    {
	اگر (regions[n].row == mrow
		&& col >= regions[n].col_start
		&& col < regions[n].col_end)
	    بشکن;
    }
    اگر (n >= region_شمار || regions[n].funcname == پوچ)
	بازگشت FALSE;

    // Build the info dictionary.
    info = dict_alloc();
    اگر (info == پوچ)
	بازگشت FALSE;

    dict_add_عدد(info, "minwid", regions[n].minwid);

    // Determine number of clicks.
    // MOD_MASK_2CLICK=0x20, MOD_MASK_3CLICK=0x40, MOD_MASK_4CLICK=0x60
    nclicks = ((mods & MOD_MASK_MULTI_CLICK) >> 5) + 1;
    اگر (nclicks > 3)
	nclicks = 3;
    dict_add_عدد(info, "nclicks", nclicks);

    // Button.
    اگر (which_button == MOUSE_چپ)
	button_str[0] = 'l';
    وگرنه اگر (which_button == MOUSE_راست)
	button_str[0] = 'r';
    وگرنه
	button_str[0] = 'm';
    button_str[1] = NUL;
    dict_add_string_len(info, "button", button_str, 1);

    // Modifiers.
    اگر (mods & MOD_MASK_SHIFT)
	mods_str[mi++] = 's';
    اگر (mods & MOD_MASK_CTRL)
	mods_str[mi++] = 'c';
    اگر (mods & MOD_MASK_ALT)
	mods_str[mi++] = 'a';
    mods_str[mi] = NUL;
    dict_add_string_len(info, "mods", mods_str, mi);

    dict_add_عدد(info, "winid", winid);

    // "area": which option the clicked region belongs to.  Lets a shared
    // dispatcher distinguish 'statusline', 'tabline' and 'tabpanel' without
    // having to overload winid == 0.
    dict_add_string(info, "area", area_name);

    // Expose tab page number for 'tabpanel' regions.
    اگر (regions[n].tabnr > 0)
	dict_add_عدد(info, "tabnr", regions[n].tabnr);

    // Call the function with the info dict as argument.
    argvars[0].v_type = VAR_DICT;
    argvars[0].vval.v_dict = info;
    ++info->dv_refcount;
    argvars[1].v_type = VAR_UNKNOWN;

    rettv.v_type = VAR_عدد;
    rettv.vval.v_عدد = 0;

    CLEAR_FIELD(funcexe);
    funcexe.fe_evaluate = TRUE;
    (تهی)call_func(regions[n].funcname, -1,
					    &rettv, 1, argvars, &funcexe);

    n = (صحیح)rettv.vval.v_عدد;
    clear_tv(&rettv);
    dict_unref(info);

    اگر (n != 0)
    {
	// Make sure the tabline gets redrawn too when the callback asks for
	// a redraw (redraw_statuslines() only redraws the tabline when
	// redraw_tabline is set).  For tabpanel the whole screen needs to be
	// refreshed.
	اگر (winid == 0)
	    redraw_tabline = TRUE;
# ifdef FEAT_TABPANEL
	اگر (STRCMP(area_name, "tabpanel") == 0)
	    redraw_all_later(UPD_NOT_VALID);
# endif
	redraw_statuslines();
    }

    بازگشت TRUE;
#وگرنه
    (تهی)regions;
    (تهی)region_شمار;
    (تهی)winid;
    (تهی)area_name;
    (تهی)mrow;
    (تهی)mcol;
    (تهی)which_button;
    (تهی)mods;
    بازگشت FALSE;
#endif
}

/*
 * Call a statusline click handler function for window "wp".
 * Returns TRUE if the function was called and handled the click.
 */
    ایستا صحیح
stl_click_handler(win_T *wp, صحیح mrow, صحیح mcol, صحیح which_button, صحیح mods)
{
    اگر (wp == پوچ)
	بازگشت FALSE;
    بازگشت stl_click_handler_regions(wp->w_stl_click, wp->w_stl_click_شمار,
				wp->w_id, (char_u *)"statusline",
				mrow, mcol, which_button, mods);
}

// dragwin is declared near the top of the file

/*
 * Reset the window being dragged.  To be called when switching tab page.
 */
    تهی
reset_dragwin(تهی)
{
    dragwin = پوچ;
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
    صحیح
jump_to_mouse(
    صحیح		flags,
    صحیح		*inclusive,	// used for inclusive operator, can be NULL
    صحیح		which_button)	// MOUSE_LEFT, MOUSE_RIGHT, MOUSE_MIDDLE
{
    ایستا صحیح	on_status_سطر = 0;	// #lines below bottom of window
    ایستا صحیح	on_sep_سطر = 0;	// on separator right of window
#ifdef FEAT_MENU
    ایستا صحیح  in_winbar = FALSE;
#endif
#ifdef FEAT_PROP_POPUP
    ایستا صحیح   in_popup_win = FALSE;
    ایستا win_T *click_in_popup_win = پوچ;
#endif
    ایستا صحیح	prev_row = -1;
    ایستا صحیح	prev_col = -1;
    ایستا صحیح	did_drag = FALSE;	// drag was noticed

    win_T	*wp, *old_curwin;
    pos_T	old_cursor;
    صحیح		شمار;
    صحیح		first;
    صحیح		row = mouse_row;
    صحیح		col = mouse_col;
    colnr_T	col_from_screen = -1;
#ifdef FEAT_FOLDING
    صحیح		mouse_char = ' ';
#endif

    mouse_past_bottom = FALSE;
    mouse_past_eol = FALSE;

    اگر (flags & MOUSE_RELEASED)
    {
	// On button release we may change window focus if positioned on a
	// status line and no dragging happened.
	اگر (dragwin != پوچ && !did_drag)
	    flags &= ~(MOUSE_FOCUS | MOUSE_DID_MOVE);
	dragwin = پوچ;
	did_drag = FALSE;
#ifdef FEAT_PROP_POPUP
	اگر (click_in_popup_win != پوچ && popup_dragwin == پوچ)
	    popup_close_for_mouse_click(click_in_popup_win);

	popup_dragwin = پوچ;
	click_in_popup_win = پوچ;
#endif
    }

    اگر ((flags & MOUSE_DID_MOVE)
	    && prev_row == mouse_row
	    && prev_col == mouse_col)
    {
retnomove:
	// before moving the cursor for a left click which is NOT in a status
	// line, stop Visual mode
	اگر (on_status_سطر)
	    بازگشت IN_STATUS_سطر;
	اگر (on_sep_سطر)
	    بازگشت IN_SEP_سطر;
#ifdef FEAT_MENU
	اگر (in_winbar)
	{
	    // A quick second click may arrive as a double-click, but we use it
	    // as a second click in the WinBar.
	    اگر ((mod_mask & MOD_MASK_MULTI_CLICK) && !(flags & MOUSE_RELEASED))
	    {
		wp = mouse_find_win(&row, &col, FAIL_POPUP);
		اگر (wp == پوچ)
		    بازگشت IN_UNKNOWN;
		winbar_click(wp, col);
	    }
	    بازگشت IN_OTHER_WIN | MOUSE_WINBAR;
	}
#endif
	اگر (flags & MOUSE_MAY_STOP_VIS)
	{
	    end_visual_mode_keep_button();
	    redraw_curbuf_later(UPD_INVERTED);	// delete the inversion
	}
#اگر defined(FEAT_CLIPBOARD)
	// Continue a modeless selection in another window.
	اگر (cmdwin_type != 0 && row < cmdwin_win->w_winrow)
	    بازگشت IN_OTHER_WIN;
#endif
#ifdef FEAT_PROP_POPUP
	// Continue a modeless selection in a popup window or dragging it.
	اگر (in_popup_win)
	{
	    click_in_popup_win = پوچ;  // don't close it on release
	    اگر (popup_dragwin != پوچ)
	    {
		// dragging a popup window
		popup_drag(popup_dragwin);
		بازگشت IN_UNKNOWN;
	    }
	    بازگشت IN_OTHER_WIN;
	}
#endif
	بازگشت IN_BUFFER;
    }

    prev_row = mouse_row;
    prev_col = mouse_col;

    اگر (flags & MOUSE_SETPOS)
	برو retnomove;				// ugly goto...

    old_curwin = curwin;
    old_cursor = curwin->w_cursor;

    اگر (!(flags & MOUSE_FOCUS))
    {
	اگر (row < 0 || col < 0) // check if it makes sense
	    بازگشت IN_UNKNOWN;

	// find the window where the row is in and adjust "row" and "col" to be
	// relative to top-left of the window
	wp = mouse_find_win(&row, &col, FIND_POPUP);
	اگر (wp == پوچ)
	    بازگشت IN_UNKNOWN;
	dragwin = پوچ;

#ifdef FEAT_PROP_POPUP
	// Click in a popup window may start dragging or modeless selection,
	// but not much else.
	اگر (WIN_IS_POPUP(wp))
	{
	    on_sep_سطر = 0;
	    on_status_سطر = 0;
	    in_popup_win = TRUE;
	    اگر (which_button == MOUSE_چپ && popup_close_if_on_X(wp, row, col))
	    {
		بازگشت IN_UNKNOWN;
	    }
	    وگرنه اگر (((wp->w_popup_flags & (POPF_DRAG | POPF_RESIZE))
					      && popup_on_border(wp, row, col))
				       || (wp->w_popup_flags & POPF_DRAGALL))
	    {
		popup_dragwin = wp;
		popup_start_drag(wp, row, col);
		بازگشت IN_UNKNOWN;
	    }
	    // Only close on release, otherwise it's not possible to drag or do
	    // modeless selection.
	    وگرنه اگر (wp->w_popup_close == POPCLOSE_CLICK
		    && which_button == MOUSE_چپ)
	    {
		click_in_popup_win = wp;
	    }
	    وگرنه اگر (which_button == MOUSE_چپ)
		// If the click is in the scrollbar, may scroll up/down.
		popup_handle_scrollbar_click(wp, row, col);
# ifdef FEAT_CLIPBOARD
	    بازگشت IN_OTHER_WIN;
# وگرنه
	    بازگشت IN_UNKNOWN;
# endif
	}
	in_popup_win = FALSE;
	popup_dragwin = پوچ;
#endif
#ifdef FEAT_MENU
	اگر (row == -1)
	{
	    // A click in the window toolbar does not enter another window or
	    // change Visual highlighting.
	    winbar_click(wp, col);
	    in_winbar = TRUE;
	    بازگشت IN_OTHER_WIN | MOUSE_WINBAR;
	}
	in_winbar = FALSE;
#endif

	// winpos and height may change in win_enter()!
	اگر (row >= wp->w_height)		// In (or below) status line
	{
	    on_status_سطر = row - wp->w_height + 1;
	    dragwin = wp;
	}
	وگرنه
	    on_status_سطر = 0;
	اگر (col >= wp->w_width)		// In separator line
	{
	    on_sep_سطر = col - wp->w_width + 1;
	    dragwin = wp;
	}
	وگرنه
	    on_sep_سطر = 0;

	// The rightmost character of the status line might be a vertical
	// separator character if there is no connecting window to the right.
	اگر (on_status_سطر && on_sep_سطر)
	{
	    اگر (stl_connected(wp))
		on_sep_سطر = 0;
	    وگرنه
		on_status_سطر = 0;
	}

	// Before jumping to another buffer, or moving the cursor for a left
	// click, stop Visual mode.
	اگر (VIsual_active
		&& (wp->w_buffer != curwin->w_buffer
		    || (!on_status_سطر && !on_sep_سطر
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
	اگر (cmdwin_type != 0 && wp != cmdwin_win)
	{
	    // A click outside the command-line window: Use modeless
	    // selection if possible.  Allow dragging the status lines.
	    on_sep_سطر = 0;
#ifdef FEAT_CLIPBOARD
	    اگر (on_status_سطر)
		بازگشت IN_STATUS_سطر;
	    بازگشت IN_OTHER_WIN;
#وگرنه
	    row = 0;
	    col += wp->w_wincol;
	    wp = cmdwin_win;
#endif
	}
#اگر defined(FEAT_PROP_POPUP) && defined(FEAT_TERMINAL)
	اگر (popup_is_popup(curwin) && curbuf->b_term != پوچ)
	    // terminal in popup window: don't jump to another window
	    بازگشت IN_OTHER_WIN;
#endif
	// Only change window focus when not clicking on or dragging the
	// status line.  Do change focus when releasing the mouse button
	// (MOUSE_FOCUS was set above if we dragged first).
	اگر (dragwin == پوچ || (flags & MOUSE_RELEASED))
	    win_enter(wp, TRUE);		// can make wp invalid!

	اگر (curwin != old_curwin)
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
	اگر (on_status_سطر)			// In (or below) status line
	{
	    // Don't use start_arrow() if we're in the same window
	    اگر (curwin == old_curwin)
		بازگشت IN_STATUS_سطر;
	    وگرنه
		بازگشت IN_STATUS_سطر | CURSOR_MOVED;
	}
	اگر (on_sep_سطر)			// In (or below) status line
	{
	    // Don't use start_arrow() if we're in the same window
	    اگر (curwin == old_curwin)
		بازگشت IN_SEP_سطر;
	    وگرنه
		بازگشت IN_SEP_سطر | CURSOR_MOVED;
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
    وگرنه اگر (on_status_سطر && which_button == MOUSE_چپ)
    {
	اگر (dragwin != پوچ)
	{
	    // Drag the status line
	    شمار = row - W_WINROW(dragwin) - dragwin->w_height + 1
							     - on_status_سطر;
	    win_drag_status_سطر(dragwin, شمار);
	    did_drag |= شمار;
	}
	بازگشت IN_STATUS_سطر;			// Cursor didn't move
    }
    وگرنه اگر (on_sep_سطر && which_button == MOUSE_چپ)
    {
	اگر (dragwin != پوچ)
	{
	    // Drag the separator column
	    شمار = col - dragwin->w_wincol - dragwin->w_width + 1
								- on_sep_سطر;
	    win_drag_vsep_سطر(dragwin, شمار);
	    did_drag |= شمار;
	}
	بازگشت IN_SEP_سطر;			// Cursor didn't move
    }
#ifdef FEAT_MENU
    وگرنه اگر (in_winbar)
    {
	// After a click on the window toolbar don't start Visual mode.
	بازگشت IN_OTHER_WIN | MOUSE_WINBAR;
    }
#endif
    وگرنه // keep_window_focus must be TRUE
    {
	// before moving the cursor for a left click, stop Visual mode
	اگر (flags & MOUSE_MAY_STOP_VIS)
	{
	    end_visual_mode_keep_button();
	    redraw_curbuf_later(UPD_INVERTED);	// delete the inversion
	}

#اگر defined(FEAT_CLIPBOARD)
	// Continue a modeless selection in another window.
	اگر (cmdwin_type != 0 && row < cmdwin_win->w_winrow)
	    بازگشت IN_OTHER_WIN;
#endif
#ifdef FEAT_PROP_POPUP
	اگر (in_popup_win)
	{
	    اگر (popup_dragwin != پوچ)
	    {
		// dragging a popup window
		popup_drag(popup_dragwin);
		بازگشت IN_UNKNOWN;
	    }
	    // continue a modeless selection in a popup window
	    click_in_popup_win = پوچ;
	    بازگشت IN_OTHER_WIN;
	}
#endif

	row -= W_WINROW(curwin);
	col -= curwin->w_wincol;

	// When clicking beyond the end of the window, scroll the screen.
	// Scroll by however many rows outside the window we are.
	اگر (row < 0)
	{
	    شمار = 0;
	    برای (first = TRUE; curwin->w_topline > 1; )
	    {
#ifdef FEAT_DIFF
		اگر (curwin->w_topfill < diff_check_fill(curwin, curwin->w_topline))
		    ++شمار;
		وگرنه
#endif
		    شمار += plines(curwin->w_topline - 1);
		اگر (!first && شمار > -row)
		    بشکن;
		first = FALSE;
#ifdef FEAT_FOLDING
		(تهی)hasFolding(curwin->w_topline, &curwin->w_topline, پوچ);
#endif
#ifdef FEAT_DIFF
		اگر (curwin->w_topfill < diff_check_fill(curwin, curwin->w_topline))
		    ++curwin->w_topfill;
		وگرنه
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
	وگرنه اگر (row >= curwin->w_height)
	{
	    شمار = 0;
	    برای (first = TRUE; curwin->w_topline < curbuf->b_ml.ml_سطر_شمار; )
	    {
#ifdef FEAT_DIFF
		اگر (curwin->w_topfill > 0)
		    ++شمار;
		وگرنه
#endif
		    شمار += plines(curwin->w_topline);
		اگر (!first && شمار > row - curwin->w_height + 1)
		    بشکن;
		first = FALSE;
#ifdef FEAT_FOLDING
		اگر (hasFolding(curwin->w_topline, پوچ, &curwin->w_topline)
			&& curwin->w_topline == curbuf->b_ml.ml_سطر_شمار)
		    بشکن;
#endif
#ifdef FEAT_DIFF
		اگر (curwin->w_topfill > 0)
		    --curwin->w_topfill;
		وگرنه
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
	وگرنه اگر (row == 0)
	{
	    // When dragging the mouse, while the text has been scrolled up as
	    // far as it goes, moving the mouse in the top line should scroll
	    // the text down (done later when recomputing w_topline).
	    اگر (mouse_dragging > 0
		    && curwin->w_cursor.lnum
				       == curwin->w_buffer->b_ml.ml_سطر_شمار
		    && curwin->w_cursor.lnum == curwin->w_topline)
		curwin->w_valid &= ~(VALID_TOPLINE);
	}
    }

    اگر (prev_row >= W_WINROW(curwin)
	&& prev_row < W_WINROW(curwin) + curwin->w_height
	&& prev_col >= curwin->w_wincol && prev_col < W_ENDCOL(curwin)
						       && Screen_سطرها != پوچ)
    {
	صحیح off = سطر_Offset[prev_row] + prev_col;

	// Only use ScreenCols[] after the window was redrawn.  Mainly matters
	// for tests, a user would not click before redrawing.
	// Do not use when 'virtualedit' is active.
	اگر (curwin->w_redr_type <= UPD_VALID_NO_UPDATE)
	    col_from_screen = ScreenCols[off];
#ifdef FEAT_FOLDING
	// Remember the character under the mouse, it might be a '-' or '+' in
	// the fold column.
	mouse_char = enc_utf8 && Screen_سطرها_UC[off] != 0
				       ? Screen_سطرها_UC[off] : Screen_سطرها[off];
#endif
    }

#ifdef FEAT_FOLDING
    // Check for position outside of the fold column.
    اگر (
# ifdef FEAT_RIGHTLEFT
	    curwin->w_p_rl ? col < curwin->w_width - curwin->w_p_fdc :
# endif
	    col >= curwin->w_p_fdc + (cmdwin_win != curwin ? 0 : 1)
       )
	mouse_char = ' ';
#endif

    // compute the position in the buffer line from the posn on the screen
    اگر (mouse_comp_pos(curwin, &row, &col, &curwin->w_cursor.lnum, پوچ))
	mouse_past_bottom = TRUE;

    // Start Visual mode before coladvance(), for when 'sel' != "old"
    اگر ((flags & MOUSE_MAY_VIS) && !VIsual_active)
    {
	check_visual_highlight();
	VIsual = old_cursor;
	VIsual_active = TRUE;
	VIsual_reselect = TRUE;
	// if 'selectmode' contains "mouse", start Select mode
	may_start_select('o');
	setmouse();
	اگر (p_smd && پیام_silent == 0)
	    redraw_cmdline = TRUE;	// show visual mode later
    }

    اگر (col_from_screen >= 0)
    {
	// Use the virtual column from ScreenCols[], it is accurate also after
	// concealed characters.
	col = col_from_screen;
    }

    curwin->w_curswant = col;
    curwin->w_set_curswant = false;	// May still have been TRUE
    اگر (coladvance(col) == FAIL)	// Mouse click beyond end of line
    {
	اگر (inclusive != پوچ)
	    *inclusive = TRUE;
	mouse_past_eol = TRUE;
    }
    وگرنه اگر (inclusive != پوچ)
	*inclusive = FALSE;

    شمار = IN_BUFFER;
    اگر (curwin != old_curwin || curwin->w_cursor.lnum != old_cursor.lnum
	    || curwin->w_cursor.col != old_cursor.col)
	شمار |= CURSOR_MOVED;		// Cursor has moved

#ifdef FEAT_FOLDING
    اگر (mouse_char == curwin->w_fill_chars.foldclosed)
	شمار |= MOUSE_FOLD_OPEN;
    وگرنه اگر (mouse_char != ' ')
	شمار |= MOUSE_FOLD_CLOSE;
#endif

    بازگشت شمار;
}

/*
 * Make a horizontal scroll to "leftcol".
 * Return TRUE if the cursor moved, FALSE otherwise.
 */
    صحیح
do_mousescroll_horiz(long_u leftcol)
{
    اگر (curwin->w_p_wrap)
	بازگشت FALSE;  // no horizontal scrolling when wrapping

    اگر (curwin->w_leftcol == (colnr_T)leftcol)
	بازگشت FALSE;  // already there

    // When the line of the cursor is too short, move the cursor to the
    // longest visible line.
    اگر (
#ifdef FEAT_GUI
	    (!gui.in_use || vim_strchr(p_go, GO_HORSCROLL) == پوچ) &&
#endif
		    !virtual_active()
	    && (long)leftcol > scroll_سطر_len(curwin->w_cursor.lnum))
    {
	curwin->w_cursor.lnum = ui_find_longest_lnum();
	curwin->w_cursor.col = 0;
    }

    بازگشت set_leftcol((colnr_T)leftcol);
}

/*
 * Normal and Visual modes implementation for scrolling in direction
 * "cap->arg", which is one of the MSCR_ values.
 */
    تهی
nv_mousescroll(cmdarg_T *cap)
{
    win_T   *old_curwin = curwin;

#ifdef FEAT_TABPANEL
    اگر (mouse_row >= 0 && mouse_col >= 0
	    && (cap->arg == MSCR_UP || cap->arg == MSCR_DOWN)
	    && mouse_on_tabpanel())
    {
	(تهی)tabpanel_scroll(cap->arg == MSCR_UP ? 1 : -1,
		mouse_vert_step > 0 ? mouse_vert_step : 3);
	بازگشت;
    }
#endif

    اگر (mouse_row >= 0 && mouse_col >= 0)
    {
	// Find the window at the mouse pointer coordinates.
	// NOTE: Must restore "curwin" to "old_curwin" before returning!
	صحیح row = mouse_row;
	صحیح col = mouse_col;
	curwin = mouse_find_win(&row, &col, FIND_POPUP);
	اگر (curwin == پوچ)
	{
	    curwin = old_curwin;
	    بازگشت;
	}

#ifdef FEAT_PROP_POPUP
	اگر (WIN_IS_POPUP(curwin) && !curwin->w_has_scrollbar)
	{
	    // cannot scroll this popup window
	    curwin = old_curwin;
	    بازگشت;
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
    تهی
nv_mouse(cmdarg_T *cap)
{
    (تهی)do_mouse(cap->oap, cap->cmdchar, BACKWARD, cap->count1, 0);
}

ایستا صحیح	held_button = MOUSE_RELEASE;

    تهی
reset_held_button(تهی)
{
    held_button = MOUSE_RELEASE;
}

/*
 * Check if typebuf 'tp' contains a terminal mouse code and returns the
 * modifiers found in typebuf in 'modifiers'.
 */
    صحیح
check_termcode_mouse(
    char_u	*tp,
    صحیح		*slen,
    char_u	*کلید_name,
    char_u	*modifiers_start,
    صحیح		idx,
    صحیح		*modifiers)
{
    صحیح		j;
    char_u	*p;
#اگر !defined(UNIX) || defined(FEAT_MOUSE_XTERM) || defined(FEAT_GUI) \
    || defined(FEAT_MOUSE_GPM) || defined(FEAT_SYSMOUSE)
    char_u	bytes[6];
    صحیح		رقم_bytes;
#endif
    صحیح		mouse_code = 0;	    // init for GCC
    صحیح		is_click, is_drag;
    صحیح		is_release, release_is_ambiguous;
    صحیح		wheel_code = 0;
    صحیح		current_button;
    ایستا صحیح	orig_رقم_clicks = 1;
    ایستا صحیح	orig_mouse_code = 0x0;
#ifdef CHECK_DOUBLE_CLICK
    ایستا صحیح	orig_mouse_col = 0;
    ایستا صحیح	orig_mouse_row = 0;
    ایستا ساختار timeval  orig_mouse_time = {0, 0};
    // time of previous mouse click
    ساختار timeval  mouse_time;		// time of current mouse click
    long	timediff;		// elapsed time in msec
#endif

    is_click = is_drag = is_release = release_is_ambiguous = FALSE;

#اگر !defined(UNIX) || defined(FEAT_MOUSE_XTERM) || defined(FEAT_GUI) \
    || defined(FEAT_MOUSE_GPM) || defined(FEAT_SYSMOUSE)
    اگر (کلید_name[0] == KS_MOUSE
# ifdef FEAT_MOUSE_GPM
	    || کلید_name[0] == KS_GPM_MOUSE
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
	برای (;;)
	{
	    // For the GUI and for MS-Windows two bytes each are used for row
	    // and column.  Allows for more than 223 columns.
# اگر defined(FEAT_GUI) || defined(MSWIN)
	    اگر (TRUE
#  اگر defined(FEAT_GUI) && !defined(MSWIN)
		&& gui.in_use
#  endif
		)
	    {
		رقم_bytes = get_bytes_from_buf(tp + *slen, bytes, 5);
		اگر (رقم_bytes == -1)	// not enough coordinates
		    بازگشت -1;
		mouse_code = bytes[0];
		mouse_col = 128 * (bytes[1] - ' ' - 1)
		    + bytes[2] - ' ' - 1;
		mouse_row = 128 * (bytes[3] - ' ' - 1)
		    + bytes[4] - ' ' - 1;
	    }
	    وگرنه
# endif
	    {
		رقم_bytes = get_bytes_from_buf(tp + *slen, bytes, 3);
		اگر (رقم_bytes == -1)	// not enough coordinates
		    بازگشت -1;
		mouse_code = bytes[0];
		mouse_col = bytes[1] - ' ' - 1;
		mouse_row = bytes[2] - ' ' - 1;
	    }
	    *slen += رقم_bytes;

	    // If the following bytes is also a mouse code and it has the same
	    // code, dump this one and get the next.  This makes dragging a
	    // whole lot faster.
# ifdef FEAT_GUI
	    اگر (gui.in_use)
		j = 3;
	    وگرنه
# endif
		j = get_termcode_len(idx);
	    اگر (STRNCMP(tp, tp + *slen, (size_t)j) == 0
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
	    وگرنه
		بشکن;
	}
    }

    اگر (کلید_name[0] == KS_URXVT_MOUSE
	    || کلید_name[0] == KS_SGR_MOUSE
	    || کلید_name[0] == KS_SGR_MOUSE_RELEASE)
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
	اگر (p == پوچ)
	    بازگشت -1;

	mouse_code = getdigits(&p);
	اگر (*p++ != ';')
	    بازگشت -1;

	// when mouse reporting is SGR, add 32 to mouse code
	اگر (کلید_name[0] == KS_SGR_MOUSE
		|| کلید_name[0] == KS_SGR_MOUSE_RELEASE)
	    mouse_code += 32;

	mouse_col = getdigits(&p) - 1;
	اگر (*p++ != ';')
	    بازگشت -1;

	mouse_row = getdigits(&p) - 1;

	// The modifiers were the mouse coordinates, not the modifier keys
	// (alt/shift/ctrl/meta) state.
	*modifiers = 0;
    }

    اگر (کلید_name[0] == KS_SGR_MOUSE
	    || کلید_name[0] == KS_SGR_MOUSE_RELEASE)
    {
	اگر (کلید_name[0] == KS_SGR_MOUSE_RELEASE)
	{
	    is_release = TRUE;
	    // This is used below to set held_button.
	    mouse_code |= MOUSE_RELEASE;
	}
    }
    وگرنه
    {
	release_is_ambiguous = TRUE;
	اگر ((mouse_code & MOUSE_RELEASE) == MOUSE_RELEASE)
	    is_release = TRUE;
    }

    اگر (کلید_name[0] == KS_MOUSE
# ifdef FEAT_MOUSE_GPM
	    || کلید_name[0] == KS_GPM_MOUSE
# endif
# ifdef FEAT_MOUSE_URXVT
	    || کلید_name[0] == KS_URXVT_MOUSE
# endif
	    || کلید_name[0] == KS_SGR_MOUSE
	    || کلید_name[0] == KS_SGR_MOUSE_RELEASE)
    {
# اگر !defined(MSWIN)
	/*
	 * Handle old style mouse events.
	 * Recognize the xterm mouse wheel, but not in the GUI, the
	 * Linux console with GPM and the MS-DOS or Win32 console
	 * (multi-clicks use >= 0x60).
	 */
	اگر (mouse_code >= MOUSEWHEEL_کم && mouse_code < MOUSESIDEBUTTONS_کم
#  ifdef FEAT_GUI
		&& !gui.in_use
#  endif
#  ifdef FEAT_MOUSE_GPM
		&& کلید_name[0] != KS_GPM_MOUSE
#  endif
	   )
	{
#  اگر defined(UNIX)
	    اگر (use_xterm_mouse() > 1 && mouse_code >= 0x80)
		// mouse-move event, using MOUSE_DRAG works
		mouse_code = MOUSE_DRAG;
	    وگرنه
#  endif
		// Keep the mouse_code before it's changed, so that we
		// remember that it was a mouse wheel click.
		wheel_code = mouse_code;
	}
#  ifdef FEAT_MOUSE_XTERM
	وگرنه اگر (held_button == MOUSE_RELEASE
#   ifdef FEAT_GUI
		&& !gui.in_use
#   endif
		&& (mouse_code == 0x23 || mouse_code == 0x24
		    || mouse_code == 0x40 || mouse_code == 0x41))
	{
	    // Apparently 0x23 and 0x24 are used by rxvt scroll wheel.
	    // And 0x40 and 0x41 are used by some xterm emulator.
	    wheel_code = mouse_code - (mouse_code >= 0x40 ? 0x40 : 0x23)
							      + MOUSEWHEEL_کم;
	}
#  endif

#  اگر defined(UNIX)
	وگرنه اگر (use_xterm_mouse() > 1)
	{
	    اگر (mouse_code & MOUSE_DRAG_XTERM)
		mouse_code |= MOUSE_DRAG;
	}
#  endif
#  ifdef FEAT_XCLIPBOARD
	وگرنه اگر (!(mouse_code & MOUSE_DRAG & ~MOUSE_CLICK_MASK))
	{
	    اگر (is_release)
		stop_xterm_trace();
	    وگرنه
		start_xterm_trace(mouse_code);
	}
#  endif
# endif
    }
#endif // !UNIX || FEAT_MOUSE_XTERM
#ifdef FEAT_MOUSE_NET
    اگر (کلید_name[0] == KS_NETTERM_MOUSE)
    {
	صحیح mc, mr;

	// expect a rather limited sequence like: balancing {
	// \033}6,45\r
	// '6' is the row, 45 is the column
	p = tp + *slen;
	mr = getdigits(&p);
	اگر (*p++ != ',')
	    بازگشت -1;
	mc = getdigits(&p);
	اگر (*p++ != '\r')
	    بازگشت -1;

	mouse_col = mc - 1;
	mouse_row = mr - 1;
	mouse_code = MOUSE_چپ;
	*slen += (صحیح)(p - (tp + *slen));
    }
#endif	// FEAT_MOUSE_NET
#ifdef FEAT_MOUSE_JSB
    اگر (کلید_name[0] == KS_JSBTERM_MOUSE)
    {
	صحیح mult, val, iter, button, status;

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
	انتخاب (*p++)
	{
	    حالت 'L': button = 1; بشکن;
	    حالت '-': بشکن;
	    حالت 'x': بشکن; // ignore sequence
	    پیشفرض:  بازگشت -1; // Unknown Result
	}
	انتخاب (*p++)
	{
	    حالت 'M': button |= 2; بشکن;
	    حالت '-': بشکن;
	    حالت 'x': بشکن; // ignore sequence
	    پیشفرض:  بازگشت -1; // Unknown Result
	}
	انتخاب (*p++)
	{
	    حالت 'R': button |= 4; بشکن;
	    حالت '-': بشکن;
	    حالت 'x': بشکن; // ignore sequence
	    پیشفرض:  بازگشت -1; // Unknown Result
	}
	status = *p++;
	برای (val = 0, mult = 100, iter = 0; iter < 3; iter++,
		mult /= 10, p++)
	    اگر (*p >= '0' && *p <= '9')
		val += (*p - '0') * mult;
	    وگرنه
		بازگشت -1;
	mouse_col = val;
	برای (val = 0, mult = 100, iter = 0; iter < 3; iter++,
		mult /= 10, p++)
	    اگر (*p >= '0' && *p <= '9')
		val += (*p - '0') * mult;
	    وگرنه
		بازگشت -1;
	mouse_row = val;
	انتخاب (*p++)
	{
	    حالت 's': button |= 8; بشکن;  // SHIFT key Pressed
	    حالت '-': بشکن;  // Not Pressed
	    حالت 'x': بشکن;  // Not Reporting
	    پیشفرض:  بازگشت -1; // Unknown Result
	}
	انتخاب (*p++)
	{
	    حالت 'c': button |= 16; بشکن;  // CTRL key Pressed
	    حالت '-': بشکن;  // Not Pressed
	    حالت 'x': بشکن;  // Not Reporting
	    پیشفرض:  بازگشت -1; // Unknown Result
	}
	اگر (*p++ != '\033')
	    بازگشت -1;
	اگر (*p++ != '\\')
	    بازگشت -1;
	انتخاب (status)
	{
	    حالت 'D': // Double Click
	    حالت 'S': // Single Click
		اگر (button & 1) mouse_code |= MOUSE_چپ;
		اگر (button & 2) mouse_code |= MOUSE_میانه;
		اگر (button & 4) mouse_code |= MOUSE_راست;
		اگر (button & 8) mouse_code |= MOUSE_SHIFT;
		اگر (button & 16) mouse_code |= MOUSE_CTRL;
		بشکن;
	    حالت 'm': // Mouse move
		اگر (button & 1) mouse_code |= MOUSE_چپ;
		اگر (button & 2) mouse_code |= MOUSE_میانه;
		اگر (button & 4) mouse_code |= MOUSE_راست;
		اگر (button & 8) mouse_code |= MOUSE_SHIFT;
		اگر (button & 16) mouse_code |= MOUSE_CTRL;
		اگر ((button & 7) != 0)
		{
		    held_button = mouse_code;
		    mouse_code |= MOUSE_DRAG;
		}
		is_drag = TRUE;
		showmode();
		بشکن;
	    حالت 'd': // Button Down
		اگر (button & 1) mouse_code |= MOUSE_چپ;
		اگر (button & 2) mouse_code |= MOUSE_میانه;
		اگر (button & 4) mouse_code |= MOUSE_راست;
		اگر (button & 8) mouse_code |= MOUSE_SHIFT;
		اگر (button & 16) mouse_code |= MOUSE_CTRL;
		بشکن;
	    حالت 'u': // Button Up
		is_release = TRUE;
		اگر (button & 1)
		    mouse_code |= MOUSE_چپ;
		اگر (button & 2)
		    mouse_code |= MOUSE_میانه;
		اگر (button & 4)
		    mouse_code |= MOUSE_راست;
		اگر (button & 8)
		    mouse_code |= MOUSE_SHIFT;
		اگر (button & 16)
		    mouse_code |= MOUSE_CTRL;
		بشکن;
	    پیشفرض: بازگشت -1; // Unknown Result
	}

	*slen += (p - (tp + *slen));
    }
#endif // FEAT_MOUSE_JSB
#ifdef FEAT_MOUSE_DEC
    اگر (کلید_name[0] == KS_DEC_MOUSE)
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
	صحیح Pe, Pb, Pr, Pc;

	p = tp + *slen;

	// get event status
	Pe = getdigits(&p);
	اگر (*p++ != ';')
	    بازگشت -1;

	// get button status
	Pb = getdigits(&p);
	اگر (*p++ != ';')
	    بازگشت -1;

	// get row status
	Pr = getdigits(&p);
	اگر (*p++ != ';')
	    بازگشت -1;

	// get column status
	Pc = getdigits(&p);

	// the page parameter is optional
	اگر (*p == ';')
	{
	    p++;
	    (تهی)getdigits(&p);
	}
	اگر (*p++ != '&')
	    بازگشت -1;
	اگر (*p++ != 'w')
	    بازگشت -1;

	mouse_code = 0;
	انتخاب (Pe)
	{
	    حالت  0: بازگشت -1; // position request while unavailable
	    حالت  1: // a response to a locator position request includes
		     //	the status of all buttons
		     Pb &= 7;   // mask off and ignore fourth button
		     اگر (Pb & 4)
			 mouse_code  = MOUSE_چپ;
		     اگر (Pb & 2)
			 mouse_code  = MOUSE_میانه;
		     اگر (Pb & 1)
			 mouse_code  = MOUSE_راست;
		     اگر (Pb)
		     {
			 held_button = mouse_code;
			 mouse_code |= MOUSE_DRAG;
			 WantQueryMouse = TRUE;
		     }
		     is_drag = TRUE;
		     showmode();
		     بشکن;
	    حالت  2: mouse_code = MOUSE_چپ;
		     WantQueryMouse = TRUE;
		     بشکن;
	    حالت  3: mouse_code = MOUSE_چپ;
		     is_release = TRUE;
		     بشکن;
	    حالت  4: mouse_code = MOUSE_میانه;
		     WantQueryMouse = TRUE;
		     بشکن;
	    حالت  5: mouse_code = MOUSE_میانه;
		     is_release = TRUE;
		     بشکن;
	    حالت  6: mouse_code = MOUSE_راست;
		     WantQueryMouse = TRUE;
		     بشکن;
	    حالت  7: mouse_code = MOUSE_راست;
		     is_release = TRUE;
		     بشکن;
	    حالت  8: بازگشت -1; // fourth button down
	    حالت  9: بازگشت -1; // fourth button up
	    حالت 10: بازگشت -1; // mouse outside of filter rectangle
	    پیشفرض: بازگشت -1; // should never occur
	}

	mouse_col = Pc - 1;
	mouse_row = Pr - 1;

	*slen += (صحیح)(p - (tp + *slen));
    }
#endif // FEAT_MOUSE_DEC
#ifdef FEAT_MOUSE_PTERM
    اگر (کلید_name[0] == KS_PTERM_MOUSE)
    {
	صحیح button, رقم_clicks, action;

	p = tp + *slen;

	action = getdigits(&p);
	اگر (*p++ != ';')
	    بازگشت -1;

	mouse_row = getdigits(&p);
	اگر (*p++ != ';')
	    بازگشت -1;
	mouse_col = getdigits(&p);
	اگر (*p++ != ';')
	    بازگشت -1;

	button = getdigits(&p);
	mouse_code = 0;

	انتخاب (button)
	{
	    حالت 4: mouse_code = MOUSE_چپ; بشکن;
	    حالت 1: mouse_code = MOUSE_راست; بشکن;
	    حالت 2: mouse_code = MOUSE_میانه; بشکن;
	    پیشفرض: بازگشت -1;
	}

	انتخاب (action)
	{
	    حالت 31: // Initial press
		اگر (*p++ != ';')
		    بازگشت -1;

		رقم_clicks = getdigits(&p); // Not used
		بشکن;

	    حالت 32: // Release
		is_release = TRUE;
		بشکن;

	    حالت 33: // Drag
		held_button = mouse_code;
		mouse_code |= MOUSE_DRAG;
		بشکن;

	    پیشفرض:
		بازگشت -1;
	}

	اگر (*p++ != 't')
	    بازگشت -1;

	*slen += (p - (tp + *slen));
    }
#endif // FEAT_MOUSE_PTERM

    // Interpret the mouse code
    current_button = (mouse_code & MOUSE_CLICK_MASK);
    اگر (is_release)
	current_button |= MOUSE_RELEASE;

    اگر (current_button == MOUSE_RELEASE
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
	اگر ((mouse_code & MOUSE_DRAG) == MOUSE_DRAG)
	    is_drag = TRUE;
	current_button = held_button;
    }
    وگرنه
    {
      اگر (wheel_code == 0)
      {
#ifdef CHECK_DOUBLE_CLICK
# ifdef FEAT_MOUSE_GPM
	/*
	 * Only for Unix, when GUI not active, we handle multi-clicks here, but
	 * not for GPM mouse events.
	 */
#  ifdef FEAT_GUI
	اگر (کلید_name[0] != KS_GPM_MOUSE && !gui.in_use)
#  وگرنه
	    اگر (کلید_name[0] != KS_GPM_MOUSE)
#  endif
# وگرنه
#  ifdef FEAT_GUI
		اگر (!gui.in_use)
#  endif
# endif
		{
		    /*
		     * Compute the time elapsed since the previous mouse click.
		     */
		    gettimeofday(&mouse_time, پوچ);
		    اگر (orig_mouse_time.tv_sec == 0)
		    {
			/*
			 * Avoid computing the difference between mouse_time
			 * and orig_mouse_time for the first click, as the
			 * difference would be huge and would cause
			 * multiplication overflow.
			 */
			timediff = p_mouset;
		    }
		    وگرنه
			timediff = time_diff_ms(&orig_mouse_time, &mouse_time);
		    orig_mouse_time = mouse_time;
		    اگر (mouse_code == orig_mouse_code
			    && timediff < p_mouset
			    && orig_رقم_clicks != 4
			    && orig_mouse_col == mouse_col
			    && orig_mouse_row == mouse_row
			    && (is_mouse_topline(curwin)
				// Double click in tab pages line also works
				// when window contents changes.
				|| (mouse_row == 0 && firstwin->w_winrow > 0))
		       )
			++orig_رقم_clicks;
		    وگرنه
			orig_رقم_clicks = 1;
		    orig_mouse_col = mouse_col;
		    orig_mouse_row = mouse_row;
		    set_mouse_topline(curwin);
		}
# اگر defined(FEAT_GUI) || defined(FEAT_MOUSE_GPM)
		وگرنه
		    orig_رقم_clicks = رقم_MOUSE_CLICKS(mouse_code);
# endif
#وگرنه
	orig_رقم_clicks = رقم_MOUSE_CLICKS(mouse_code);
#endif
	is_click = TRUE;
      }
      orig_mouse_code = mouse_code;
    }
    اگر (!is_drag)
	held_button = mouse_code & MOUSE_CLICK_MASK;

    /*
     * Translate the actual mouse event into a pseudo mouse event.
     * First work out what modifiers are to be used.
     */
    اگر (orig_mouse_code & MOUSE_SHIFT)
	*modifiers |= MOD_MASK_SHIFT;
    اگر (orig_mouse_code & MOUSE_CTRL)
	*modifiers |= MOD_MASK_CTRL;
    اگر (orig_mouse_code & MOUSE_ALT)
	*modifiers |= MOD_MASK_ALT;
    اگر (orig_رقم_clicks == 2)
	*modifiers |= MOD_MASK_2CLICK;
    وگرنه اگر (orig_رقم_clicks == 3)
	*modifiers |= MOD_MASK_3CLICK;
    وگرنه اگر (orig_رقم_clicks == 4)
	*modifiers |= MOD_MASK_4CLICK;

    // Work out our pseudo mouse event. Note that MOUSE_RELEASE gets added,
    // then it's not mouse up/down.
    کلید_name[0] = KS_EXTRA;
    اگر (wheel_code != 0 && (!is_release || release_is_ambiguous))
    {
	اگر (wheel_code & MOUSE_CTRL)
	    *modifiers |= MOD_MASK_CTRL;
	اگر (wheel_code & MOUSE_ALT)
	    *modifiers |= MOD_MASK_ALT;

	اگر (wheel_code & 1 && wheel_code & 2)
	    کلید_name[1] = (صحیح)KE_MOUSELEFT;
	وگرنه اگر (wheel_code & 2)
	    کلید_name[1] = (صحیح)KE_MOUSERIGHT;
	وگرنه اگر (wheel_code & 1)
	    کلید_name[1] = (صحیح)KE_MOUSEUP;
	وگرنه
	    کلید_name[1] = (صحیح)KE_MOUSEDOWN;

	held_button = MOUSE_RELEASE;
    }
    وگرنه
    {
#اگر defined(UNIX)
	اگر (use_xterm_mouse() && orig_mouse_code >= MOUSESIDEBUTTONS_کم)
	    current_button = (current_button) ? MOUSE_X2 : MOUSE_X1;
#endif
	کلید_name[1] = get_pseudo_mouse_code(current_button, is_click, is_drag);
    }


    // Make sure the mouse position is valid.  Some terminals may return weird
    // values.
    اگر (mouse_col >= Columns)
	mouse_col = Columns - 1;
    اگر (mouse_row >= Rows)
	mouse_row = Rows - 1;

    بازگشت 0;
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
    صحیح
mouse_comp_pos(
    win_T	*win,
    صحیح		*rowp,
    صحیح		*colp,
    linenr_T	*lnump,
    صحیح		*plines_cache)
{
    صحیح		col = *colp;
    صحیح		row = *rowp;
    linenr_T	lnum;
    صحیح		retval = FALSE;
    صحیح		off;
    صحیح		شمار;

#ifdef FEAT_RIGHTLEFT
    اگر (win->w_p_rl)
	col = win->w_width - 1 - col;
#endif

    lnum = win->w_topline;

    وقتی (row > 0)
    {
	صحیح cache_idx = lnum - win->w_topline;

	// Only "Rows" lines are cached, with folding we'll run out of entries
	// and use the slow way.
	اگر (plines_cache != پوچ && cache_idx < Rows
						&& plines_cache[cache_idx] > 0)
	    شمار = plines_cache[cache_idx];
	وگرنه
	{
#ifdef FEAT_DIFF
	    // Don't include filler lines in "count"
	    اگر (win->w_p_diff
# ifdef FEAT_FOLDING
		    && !hasFoldingWin(win, lnum, پوچ, پوچ, TRUE, پوچ)
# endif
		    )
	    {
		اگر (lnum == win->w_topline)
		    row -= win->w_topfill;
		وگرنه
		    row -= diff_check_fill(win, lnum);
		شمار = plines_win_nofill(win, lnum, FALSE);
	    }
	    وگرنه
#endif
		شمار = plines_win(win, lnum, FALSE);
	    اگر (plines_cache != پوچ && cache_idx < Rows)
		plines_cache[cache_idx] = شمار;
	}

	اگر (win->w_skipcol > 0 && lnum == win->w_topline)
	{
	    صحیح width1 = win->w_width - win_col_off(win);

	    اگر (width1 > 0)
	    {
		صحیح skip_سطرها = 0;

		// Adjust for 'smoothscroll' clipping the top screen lines.
		// A similar formula is used in curs_columns().
		اگر (win->w_skipcol > width1)
		    skip_سطرها = (win->w_skipcol - width1)
					    / (width1 + win_col_off2(win)) + 1;
		وگرنه اگر (win->w_skipcol > 0)
		    skip_سطرها = 1;

		شمار -= skip_سطرها;
	    }
	}

	اگر (شمار > row)
	    بشکن;	// Position is in this buffer line.
#ifdef FEAT_FOLDING
	(تهی)hasFoldingWin(win, lnum, پوچ, &lnum, TRUE, پوچ);
#endif
	اگر (lnum == win->w_buffer->b_ml.ml_سطر_شمار)
	{
	    retval = TRUE;
	    بشکن;		// past end of file
	}
	row -= شمار;
	++lnum;
    }

    اگر (!retval)
    {
	// Compute the column without wrapping.
	off = win_col_off(win) - win_col_off2(win);
	اگر (col < off)
	    col = off;
	col += row * (win->w_width - off);

	// Add skip column for the topline.
	اگر (lnum == win->w_topline)
	    col += win->w_skipcol;
    }

    اگر (!win->w_p_wrap)
	col += win->w_leftcol;

    // skip line number and fold column in front of the line
    col -= win_col_off(win);
    اگر (col <= 0)
    {
#ifdef FEAT_NETBEANS_INTG
	// if mouse is clicked on the gutter, then inform the netbeans server
	اگر (*colp < win_col_off(win))
	    netbeans_gutter_click(lnum);
#endif
	col = 0;
    }

    *colp = col;
    *rowp = row;
    *lnump = lnum;
    بازگشت retval;
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
mouse_find_win(صحیح *rowp, صحیح *colp, mouse_find_T popup UNUSED)
{
    frame_T	*fp;
    win_T	*wp;

#ifdef FEAT_PROP_POPUP
    win_T	*pwp = پوچ;

    اگر (popup != IGNORE_POPUP)
    {
	popup_reset_handled(POPUP_HANDLED_1);
	وقتی ((wp = find_next_popup(TRUE, POPUP_HANDLED_1)) != پوچ)
	{
	    اگر (*rowp >= wp->w_winrow && *rowp < wp->w_winrow + popup_height(wp)
		    && *colp >= wp->w_wincol
				    && *colp < wp->w_wincol + popup_width(wp))
		pwp = wp;
	}
	اگر (pwp != پوچ)
	{
	    اگر (popup == FAIL_POPUP)
		بازگشت پوچ;
	    *rowp -= pwp->w_winrow;
	    *colp -= pwp->w_wincol;
	    بازگشت pwp;
	}
    }
#endif

    fp = topframe;

    اگر (*colp < firstwin->w_wincol
	    || *colp >= firstwin->w_wincol + fp->fr_width
	    || *rowp < firstwin->w_winrow)
	بازگشت پوچ;

    *rowp -= firstwin->w_winrow;
    *colp -= firstwin->w_wincol;
    برای (;;)
    {
	اگر (fp->fr_layout == FR_LEAF)
	    بشکن;
	اگر (fp->fr_layout == FR_ROW)
	{
	    برای (fp = fp->fr_child; fp->fr_next != پوچ; fp = fp->fr_next)
	    {
		اگر (*colp < fp->fr_width)
		    بشکن;
		*colp -= fp->fr_width;
	    }
	}
	وگرنه    // fr_layout == FR_COL
	{
	    برای (fp = fp->fr_child; fp->fr_next != پوچ; fp = fp->fr_next)
	    {
		اگر (*rowp < fp->fr_height)
		    بشکن;
		*rowp -= fp->fr_height;
	    }
	}
    }
    // When using a timer that closes a window the window might not actually
    // exist.
    FOR_ALL_WINDOWS(wp)
	اگر (wp == fp->fr_win)
	{
#ifdef FEAT_MENU
	    *rowp -= wp->w_winbar_height;
#endif
	    بازگشت wp;
	}
    بازگشت پوچ;
}

#اگر defined(NEED_VCOL2COL) || defined(FEAT_BEVAL) || defined(FEAT_PROP_POPUP) \
	|| defined(FEAT_EVAL)
/*
 * Convert a virtual (screen) column to a character column.
 * The first column is zero.
 */
    صحیح
vcol2col(win_T *wp, linenr_T lnum, صحیح vcol, colnr_T *coladdp)
{
    char_u	    *سطر;
    chartabsize_T   cts;

    // try to advance to the specified column
    سطر = ml_get_buf(wp->w_buffer, lnum, FALSE);
    init_chartabsize_arg(&cts, wp, lnum, 0, سطر, سطر);
    وقتی (cts.cts_vcol < vcol && *cts.cts_ptr != NUL)
    {
	صحیح size = win_lbr_chartabsize(&cts, پوچ, پوچ);
	اگر (cts.cts_vcol + size > vcol)
	    بشکن;
	cts.cts_vcol += size;
	MB_PTR_ADV(cts.cts_ptr);
    }
    clear_chartabsize_arg(&cts);

    اگر (coladdp != پوچ)
	*coladdp = vcol - cts.cts_vcol;
    بازگشت (صحیح)(cts.cts_ptr - سطر);
}
#endif

#اگر defined(FEAT_EVAL)
/*
 * "getmousepos()" function.
 */
    تهی
f_getmousepos(typval_T *argvars UNUSED, typval_T *rettv)
{
    dict_T	*d;
    win_T	*wp;
    صحیح		row = mouse_row;
    صحیح		col = mouse_col;
    varnumber_T winid = 0;
    varnumber_T winrow = 0;
    varnumber_T wincol = 0;
    linenr_T	lnum = 0;
    varnumber_T column = 0;
    colnr_T	coladd = 0;

    اگر (rettv_dict_alloc(rettv) == FAIL)
	بازگشت;
    d = rettv->vval.v_dict;

    dict_add_عدد(d, "screenrow", (varnumber_T)mouse_row + 1);
    dict_add_عدد(d, "screencol", (varnumber_T)mouse_col + 1);

    wp = mouse_find_win(&row, &col, FIND_POPUP);
    اگر (wp != پوچ)
    {
	صحیح	بالاترین_off = 0;
	صحیح	چپ_off = 0;
	صحیح	height = wp->w_height + wp->w_status_height;

# ifdef FEAT_PROP_POPUP
	اگر (WIN_IS_POPUP(wp))
	{
	    بالاترین_off = popup_بالاترین_extra(wp);
	    چپ_off = popup_چپ_extra(wp);
	    height = popup_height(wp);
	}
# endif
	اگر (row < height)
	{
	    winid = wp->w_id;
	    winrow = row + 1;
	    wincol = col + 1;
	    row -= بالاترین_off;
	    col -= چپ_off;
	    اگر (row >= 0 && row < wp->w_height && col >= 0 && col < wp->w_width)
	    {
		(تهی)mouse_comp_pos(wp, &row, &col, &lnum, پوچ);
		col = vcol2col(wp, lnum, col, &coladd);
		column = col + 1;
	    }
	}
    }
    dict_add_عدد(d, "winid", winid);
    dict_add_عدد(d, "winrow", winrow);
    dict_add_عدد(d, "wincol", wincol);
    dict_add_عدد(d, "line", (varnumber_T)lnum);
    dict_add_عدد(d, "column", column);
    dict_add_عدد(d, "coladd", coladd);
}
#endif
