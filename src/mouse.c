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

#அந்தர்பாவ "vim.h"

/*
 * Horiziontal and vertical steps used when scrolling.
 * When negative scroll by a whole page.
 */
நிலையான long mouse_hor_step = 6;
நிலையான long mouse_vert_step = 3;
நிலையான win_T *dragwin = வெற்று;	// window being dragged
நிலையான முழு stl_click_handler(win_T *wp, முழு mrow, முழு mcol, முழு which_button,
								    முழு mods);
நிலையான முழு stl_click_handler_regions(stl_click_region_T *regions,
				    முழு region_மொத்தம், முழு winid,
				    char_u *area_name, முழு mrow, முழு mcol,
				    முழு which_button, முழு mods);

    வெறுமை
mouse_set_vert_scroll_step(long step)
{
    mouse_vert_step = step;
}

    வெறுமை
mouse_set_hor_scroll_step(long step)
{
    mouse_hor_step = step;
}

#ifdef CHECK_DOUBLE_CLICK
/*
 * Return the duration from t1 to t2 in milliseconds.
 */
    நிலையான long
time_diff_ms(கட்டமைப்பு timeval *t1, கட்டமைப்பு timeval *t2)
{
    // This handles wrapping of tv_usec correctly without any special case.
    // Example of 2 pairs (tv_sec, tv_usec) with a duration of 5 ms:
    //	   t1 = (1, 998000) t2 = (2, 3000) gives:
    //	   (2 - 1) * 1000 + (3000 - 998000) / 1000 -> 5 ms.
    திரும்பு (t2->tv_sec - t1->tv_sec) * 1000
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
    நிலையான முழு
get_mouse_class(char_u *p)
{
    முழு		c;

    எனில் (has_mbyte && MB_BYTE2LEN(p[0]) > 1)
	திரும்பு mb_get_class(p);

    c = *p;
    எனில் (c == ' ' || c == '\t')
	திரும்பு 0;

    எனில் (vim_iswordc(c))
	திரும்பு 2;

    // There are a few special cases where we want certain combinations of
    // characters to be considered as a single word.  These are things like
    // "->", "/ *", "*=", "+=", "&=", "<=", ">=", "!=" etc.  Otherwise, each
    // character is in its own class.
    எனில் (c != NUL && vim_strchr((char_u *)"-+*/%<>&|^!=", c) != வெற்று)
	திரும்பு 1;
    திரும்பு c;
}

/*
 * Move "pos" back to the start of the word it's in.
 */
    நிலையான வெறுமை
find_start_of_word(pos_T *pos)
{
    char_u	*வரி;
    முழு		cclass;
    முழு		col;

    வரி = ml_get(pos->lnum);
    cclass = get_mouse_class(வரி + pos->col);

    போது (pos->col > 0)
    {
	col = pos->col - 1;
	col -= (*mb_head_off)(வரி, வரி + col);
	எனில் (get_mouse_class(வரி + col) != cclass)
	    நிறுத்து;
	pos->col = col;
    }
}

/*
 * Move "pos" forward to the end of the word it's in.
 * When 'selection' is "exclusive", the position is just after the word.
 */
    நிலையான வெறுமை
find_end_of_word(pos_T *pos)
{
    char_u	*வரி;
    முழு		cclass;
    முழு		col;

    வரி = ml_get(pos->lnum);
    எனில் (*p_sel == 'e' && pos->col > 0)
    {
	--pos->col;
	pos->col -= (*mb_head_off)(வரி, வரி + pos->col);
    }
    cclass = get_mouse_class(வரி + pos->col);
    போது (வரி[pos->col] != NUL)
    {
	col = pos->col + (*mb_ptr2len)(வரி + pos->col);
	எனில் (get_mouse_class(வரி + col) != cclass)
	{
	    எனில் (*p_sel == 'e')
		pos->col = col;
	    நிறுத்து;
	}
	pos->col = col;
    }
}

#எனில் defined(FEAT_GUI_MOTIF) || defined(FEAT_GUI_GTK) \
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
    நிலையான முழு
get_fpos_of_mouse(pos_T *mpos)
{
    win_T	*wp;
    முழு		row = mouse_row;
    முழு		col = mouse_col;

    எனில் (row < 0 || col < 0)		// check if it makes sense
	திரும்பு IN_UNKNOWN;

    // find the window where the row is in
    wp = mouse_find_win(&row, &col, FAIL_POPUP);
    எனில் (wp == வெற்று)
	திரும்பு IN_UNKNOWN;
    // winpos and height may change in win_enter()!
    எனில் (row >= wp->w_height)	// In (or below) status line
	திரும்பு IN_STATUS_வரி;
    எனில் (col >= wp->w_width)	// In vertical separator line
	திரும்பு IN_SEP_வரி;

    எனில் (wp != curwin)
	திரும்பு IN_UNKNOWN;

    // compute the position in the buffer line from the posn on the screen
    எனில் (mouse_comp_pos(curwin, &row, &col, &mpos->lnum, வெற்று))
	திரும்பு IN_STATUS_வரி; // past bottom

    mpos->col = vcol2col(wp, mpos->lnum, col, &mpos->coladd);
    திரும்பு IN_BUFFER;
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
    முழு
do_mouse(
    oparg_T	*oap,		// operator argument, can be NULL
    முழு		c,		// K_LEFTMOUSE, etc
    முழு		dir,		// Direction to 'put' if necessary
    long	மொத்தம்,
    முழு		fixindent)	// PUT_FIXINDENT if fixing indent necessary
{
    நிலையான முழு	do_always = FALSE;	// ignore 'mouse' setting next time
    நிலையான முழு	got_click = FALSE;	// got a click some time back

    முழு		which_button;	// MOUSE_LEFT, _MIDDLE or _RIGHT
    முழு		is_click = FALSE; // If FALSE it's a drag or release event
    முழு		is_drag = FALSE;  // If TRUE it's a drag event
    முழு		jump_flags = 0;	// flags for jump_to_mouse()
    pos_T	start_visual;
    முழு		moved;		// Has cursor moved?
    முழு		in_status_வரி;	// mouse in status line
    நிலையான முழு	in_tab_வரி = FALSE; // mouse clicked in tab line
    நிலையான முழு	in_tabpanel = FALSE; // mouse clicked in tabpanel
#ifdef FEAT_TABPANEL
    நிலையான bool	in_tabpanel_scrollbar = false; // dragging tabpanel scrollbar
#endif
    முழு		in_sep_வரி;	// mouse in vertical separator line
    முழு		c1, c2;
#எனில் defined(FEAT_FOLDING)
    pos_T	save_cursor;
#endif
    win_T	*old_curwin = curwin;
    நிலையான pos_T orig_cursor;
    colnr_T	leftcol, rightcol;
    pos_T	end_visual;
    முழு		diff;
    முழு		old_active = VIsual_active;
    முழு		old_mode = VIsual_mode;
    முழு		regname;

#எனில் defined(FEAT_FOLDING)
    save_cursor = curwin->w_cursor;
#endif

    // When GUI is active, always recognize mouse events, otherwise:
    // - Ignore mouse event in normal mode if 'mouse' doesn't include 'n'.
    // - Ignore mouse event in visual mode if 'mouse' doesn't include 'v'.
    // - For command line and insert mode 'mouse' is checked before calling
    //	 do_mouse().
    எனில் (do_always)
	do_always = FALSE;
    இல்லையெனில்
#ifdef FEAT_GUI
	எனில் (!gui.in_use)
#endif
	{
	    எனில் (VIsual_active)
	    {
		எனில் (!mouse_has(MOUSE_VISUAL))
		    திரும்பு FALSE;
	    }
	    இல்லையெனில் எனில் (State == MODE_NORMAL && !mouse_has(MOUSE_NORMAL))
		திரும்பு FALSE;
	}

    க்கு (;;)
    {
	which_button = get_mouse_button(KEY2TERMCAP1(c), &is_click, &is_drag);
	எனில் (is_drag)
	{
	    // If the next character is the same mouse event then use that
	    // one. Speeds up dragging the status line.
	    // Note: Since characters added to the stuff buffer in the code
	    // below need to come before the next character, do not do this
	    // when the current character was stuffed.
	    எனில் (!சாவி_Stuffed && vpeekc() != NUL)
	    {
		முழு nc;
		முழு save_mouse_row = mouse_row;
		முழு save_mouse_col = mouse_col;

		// Need to get the character, peeking doesn't get the actual
		// one.
		nc = safe_vgetc();
		எனில் (c == nc)
		    தொடர்;
		vungetc(nc);
		mouse_row = save_mouse_row;
		mouse_col = save_mouse_col;
	    }
	}
	நிறுத்து;
    }

    எனில் (c == K_MOUSEMOVE)
    {
	// Mouse moved without a button pressed.
#ifdef FEAT_BEVAL_TERM
	ui_may_remove_balloon();
	எனில் (p_bevalterm)
	{
	    profile_setlimit(p_bdlay, &bevalexpr_due);
	    bevalexpr_due_set = TRUE;
	}
#endif
#ifdef FEAT_PROP_POPUP
	popup_handle_mouse_moved();
#endif
	திரும்பு FALSE;
    }

#ifdef FEAT_MOUSESHAPE
    // May have stopped dragging the status or separator line.  The pointer is
    // most likely still on the status or separator line.
    எனில் (!is_drag && drag_status_வரி)
    {
	drag_status_வரி = FALSE;
	update_mouseshape(SHAPE_IDX_STATUS);
    }
    எனில் (!is_drag && drag_sep_வரி)
    {
	drag_sep_வரி = FALSE;
	update_mouseshape(SHAPE_IDX_VSEP);
    }
#endif

    // Ignore drag and release events if we didn't get a click.
    எனில் (is_click)
    {
	got_click = TRUE;
	in_tab_வரி = FALSE;
	in_tabpanel = FALSE;
#ifdef FEAT_TABPANEL
	in_tabpanel_scrollbar = false;
#endif
    }
    இல்லையெனில்
    {
	எனில் (!got_click)			// didn't get click, ignore
	    திரும்பு FALSE;
	எனில் (!is_drag)			// release, reset got_click
	{
	    got_click = FALSE;
	    எனில் (in_tab_வரி || in_tabpanel
#ifdef FEAT_TABPANEL
		    || in_tabpanel_scrollbar
#endif
		    )
	    {
		in_tab_வரி = FALSE;
		in_tabpanel = FALSE;
#ifdef FEAT_TABPANEL
		in_tabpanel_scrollbar = false;
#endif
		திரும்பு FALSE;
	    }
	}
    }

#ifdef FEAT_TABPANEL
    // Continue a scrollbar drag before any tab-selection handling.
    எனில் (is_drag && in_tabpanel_scrollbar)
    {
	tabpanel_drag_scrollbar(mouse_row);
	திரும்பு FALSE;
    }
#endif

    // CTRL right mouse button does CTRL-T
    எனில் (is_click && (mod_mask & MOD_MASK_CTRL) && which_button == MOUSE_வலது)
    {
	எனில் (State & MODE_செருகு)
	    stuffcharReadbuff(Ctrl_O);
	எனில் (மொத்தம் > 1)
	    stuffnumReadbuff(மொத்தம்);
	stuffcharReadbuff(Ctrl_T);
	got_click = FALSE;		// ignore drag&release now
	திரும்பு FALSE;
    }

    // CTRL only works with left mouse button
    எனில் ((mod_mask & MOD_MASK_CTRL) && which_button != MOUSE_இடது)
	திரும்பு FALSE;

    // When a modifier is down, ignore drag and release events, as well as
    // multiple clicks and the middle mouse button.
    // Accept shift-leftmouse drags when 'mousemodel' is "popup.*".
    எனில் ((mod_mask & (MOD_MASK_SHIFT | MOD_MASK_CTRL | MOD_MASK_ALT
							     | MOD_MASK_META))
	    && (!is_click
		|| (mod_mask & MOD_MASK_MULTI_CLICK)
		|| which_button == MOUSE_நடு)
	    && !((mod_mask & (MOD_MASK_SHIFT|MOD_MASK_ALT))
		&& mouse_model_popup()
		&& which_button == MOUSE_இடது)
	    && !((mod_mask & MOD_MASK_ALT)
		&& !mouse_model_popup()
		&& which_button == MOUSE_வலது)
	    )
	திரும்பு FALSE;

    // If the button press was used as the movement command for an operator
    // (eg "d<MOUSE>"), or it is the middle button that is held down, ignore
    // drag/release events.
    எனில் (!is_click && which_button == MOUSE_நடு)
	திரும்பு FALSE;

    எனில் (oap != வெற்று)
	regname = oap->regname;
    இல்லையெனில்
	regname = 0;

    // Middle mouse button does a 'put' of the selected text
    எனில் (which_button == MOUSE_நடு)
    {
	எனில் (State == MODE_NORMAL)
	{
	    // If an operator was pending, we don't know what the user wanted
	    // to do. Go back to normal mode: Clear the operator and beep().
	    எனில் (oap != வெற்று && oap->op_type != OP_NOP)
	    {
		clearopbeep(oap);
		திரும்பு FALSE;
	    }

	    // If visual was active, yank the highlighted text and put it
	    // before the mouse pointer position.
	    // In Select mode replace the highlighted text with the clipboard.
	    எனில் (VIsual_active)
	    {
		எனில் (VIsual_select)
		{
		    stuffcharReadbuff(Ctrl_G);
		    stuffReadbuff((char_u *)"\"+p");
		}
		இல்லையெனில்
		{
		    stuffcharReadbuff('y');
		    stuffcharReadbuff(K_MIDDLEMOUSE);
		}
		do_always = TRUE;	// ignore 'mouse' setting next time
		திரும்பு FALSE;
	    }
	    // The rest is below jump_to_mouse()
	}

	இல்லையெனில் எனில் ((State & MODE_செருகு) == 0)
	    திரும்பு FALSE;

	// Middle click in insert mode doesn't move the mouse, just insert the
	// contents of a register.  '.' register is special, can't insert that
	// with do_put().
	// Also paste at the cursor if the current mode isn't in 'mouse' (only
	// happens for the GUI).
	எனில் ((State & MODE_செருகு) || !mouse_has(MOUSE_NORMAL))
	{
	    எனில் (regname == '.')
		செருகு_reg(regname, TRUE);
	    இல்லையெனில்
	    {
#ifdef FEAT_CLIPBOARD
		எனில் (clip_star.available && regname == 0)
		    regname = '*';
#endif
		எனில் ((State & REPLACE_FLAG) && !yank_register_mline(regname))
		    செருகு_reg(regname, TRUE);
		இல்லையெனில்
		{
		    do_put(regname, வெற்று, BACKWARD, 1L,
						      fixindent | PUT_CURSEND);

		    // Repeat it with CTRL-R CTRL-O r or CTRL-R CTRL-P r
		    AppendCharToRedobuff(Ctrl_R);
		    AppendCharToRedobuff(fixindent ? Ctrl_P : Ctrl_O);
		    AppendCharToRedobuff(regname == 0 ? '"' : regname);
		}
	    }
	    திரும்பு FALSE;
	}
    }

    // When dragging or button-up stay in the same window.
    எனில் (!is_click)
	jump_flags |= MOUSE_FOCUS | MOUSE_DID_MOVE;

    start_visual.lnum = 0;

    கட்டமைப்பு tabpage_label_info {
	bool is_panel;	    // label type. true: tabpanel, false: tab line
	bool just_in;	    // just in tabpage label area
	bool just_click;    // just click tabpage label area
	முழு nr;		    // tabpage number
    } tp_label = { false, false, false, 0 };

    // Check for clicking in the tab page panel.
#எனில் defined(FEAT_TABPANEL)
    எனில் (mouse_col < firstwin->w_wincol
		|| mouse_col >= firstwin->w_wincol + topframe->fr_width)
    {
	// A click on the scrollbar column starts a drag interaction and
	// preempts tab-selection.
	எனில் (is_click && !is_drag && mouse_on_tabpanel_scrollbar())
	{
	    in_tabpanel_scrollbar = TRUE;
	    tabpanel_drag_scrollbar(mouse_row);
	    திரும்பு FALSE;
	}

	// Dispatch 'tabpanel' %[FuncName] click regions before falling through
	// to tab-page selection.  On drag events fall through to the normal
	// tab-drag handling.
	எனில் (is_click && !is_drag
		&& stl_click_handler_regions(tabpanel_stl_click,
					    tabpanel_stl_click_மொத்தம்,
					    0, (char_u *)"tabpanel",
					    mouse_row, mouse_col,
					    which_button, mod_mask))
	    திரும்பு FALSE;

	tp_label.is_panel = true;
	tp_label.just_in = true;
	tp_label.nr = get_tabpagenr_on_tabpanel();

	// click in a tab selects that tab page
	எனில் (is_click && cmdwin_type == 0)
	    tp_label.just_click = true;
    }
    இல்லையெனில்
#endif
    // Check for clicking in the tab page line.
    எனில் (TabPageIdxs != வெற்று && mouse_row == 0 && firstwin->w_winrow > 0)
    {
	// Dispatch 'tabline' %[FuncName] click regions before falling through
	// to tab-page selection.  On drag events fall through to the normal
	// tab-drag handling.
	எனில் (is_click && !is_drag
		&& stl_click_handler_regions(tabline_stl_click,
					    tabline_stl_click_மொத்தம்,
					    0, (char_u *)"tabline",
					    mouse_row, mouse_col,
					    which_button, mod_mask))
	    திரும்பு FALSE;

	tp_label.just_in = true;
	tp_label.nr = TabPageIdxs[mouse_col];

	// click in a tab selects that tab page
	எனில் (is_click && cmdwin_type == 0
		&& mouse_col < firstwin->w_wincol + topframe->fr_width)
	    tp_label.just_click = true;
    }

    எனில் (tp_label.just_in)
    {
	எனில் (is_drag)
	{
	    எனில் (in_tabpanel || in_tab_வரி)
	    {
		c1 = tp_label.nr;
		tabpage_move(c1 <= 0 ? 9999 : c1 < tabpage_index(curtab)
							    ? c1 - 1 : c1);
	    }
	    திரும்பு FALSE;
	}

	எனில் (tp_label.just_click)
	{
	    எனில் (tp_label.is_panel)
		in_tabpanel = TRUE;
	    இல்லையெனில்
		in_tab_வரி = TRUE;
	    c1 = tp_label.nr;
	    எனில் (c1 >= 0)
	    {
		எனில் ((mod_mask & MOD_MASK_MULTI_CLICK) == MOD_MASK_2CLICK
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
		இல்லையெனில்
		{
		    // Go to specified tab page, or next one if not clicking
		    // on a label.
		    goto_tabpage(c1);

		    // It's like clicking on the status line of a window.
		    எனில் (curwin != old_curwin)
			end_visual_mode_keep_button();
		}
	    }
	    இல்லையெனில்
	    {
		tabpage_T	*tp;

		// Close the current or specified tab page.
		எனில் (c1 == -999)
		    tp = curtab;
		இல்லையெனில்
		    tp = find_tabpage(-c1);
		எனில் (tp == curtab)
		{
		    எனில் (first_tabpage->tp_next != வெற்று)
			tabpage_close(FALSE);
		}
		இல்லையெனில் எனில் (tp != வெற்று)
		    tabpage_close_other(tp, FALSE);
	    }
	}
	திரும்பு TRUE;
    }
    இல்லையெனில் எனில் (is_drag && (in_tabpanel || (in_tab_வரி && TabPageIdxs != வெற்று)))
    {
#எனில் defined(FEAT_TABPANEL)
	எனில் (in_tabpanel)
	    c1 = get_tabpagenr_on_tabpanel();
	இல்லையெனில்
#endif
	    c1 = TabPageIdxs[mouse_col];
	tabpage_move(c1 <= 0 ? 9999 : c1 - 1);
	திரும்பு FALSE;
    }

    // When 'mousemodel' is "popup" or "popup_setpos", translate mouse events:
    // right button up   -> pop-up menu
    // shift-left button -> right button
    // alt-left button   -> alt-right button
    எனில் (mouse_model_popup())
    {
	எனில் (which_button == MOUSE_வலது
			    && !(mod_mask & (MOD_MASK_SHIFT | MOD_MASK_CTRL)))
	{
#ifdef USE_POPUP_SETPOS
# ifdef FEAT_GUI
	    எனில் (gui.in_use)
	    {
#  எனில் defined(FEAT_GUI_MOTIF) || defined(FEAT_GUI_GTK) \
			  || defined(FEAT_GUI_PHOTON)
		எனில் (!is_click)
		    // Ignore right button release events, only shows the popup
		    // menu on the button down event.
		    திரும்பு FALSE;
#  endif
#  எனில் defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_HAIKU)
		எனில் (is_click || is_drag)
		    // Ignore right button down and drag mouse events.  Windows
		    // only shows the popup menu on the button up event.
		    திரும்பு FALSE;
#  endif
	    }
# endif
# எனில் defined(FEAT_GUI) && defined(FEAT_TERM_POPUP_MENU)
	    இல்லையெனில்
# endif
# எனில் defined(FEAT_TERM_POPUP_MENU)
	    எனில் (!is_click)
		// Ignore right button release events, only shows the popup
		// menu on the button down event.
		திரும்பு FALSE;
# endif

	    jump_flags = 0;
	    எனில் (STRCMP(p_mousem, "popup_setpos") == 0)
	    {
		// First set the cursor position before showing the popup
		// menu.
		எனில் (VIsual_active)
		{
		    pos_T    m_pos;

		    // set MOUSE_MAY_STOP_VIS if we are outside the
		    // selection or the current window (might have false
		    // negative here)
		    எனில் (mouse_row < curwin->w_winrow
			 || mouse_row
				  > (curwin->w_winrow + curwin->w_height))
			jump_flags = MOUSE_MAY_STOP_VIS;
		    இல்லையெனில் எனில் (get_fpos_of_mouse(&m_pos) != IN_BUFFER)
			jump_flags = MOUSE_MAY_STOP_VIS;
		    இல்லையெனில்
		    {
			எனில் (VIsual_mode == 'V')
			{
			    எனில் ((curwin->w_cursor.lnum <= VIsual.lnum
				    && (m_pos.lnum < curwin->w_cursor.lnum
					|| VIsual.lnum < m_pos.lnum))
				|| (VIsual.lnum < curwin->w_cursor.lnum
				    && (m_pos.lnum < VIsual.lnum
					|| curwin->w_cursor.lnum < m_pos.lnum)))
			    {
				jump_flags = MOUSE_MAY_STOP_VIS;
			    }
			}
			இல்லையெனில் எனில் ((LTOREQ_POS(curwin->w_cursor, VIsual)
				    && (LT_POS(m_pos, curwin->w_cursor)
					|| LT_POS(VIsual, m_pos)))
				|| (LT_POS(VIsual, curwin->w_cursor)
				    && (LT_POS(m_pos, VIsual)
				      || LT_POS(curwin->w_cursor, m_pos))))
			{
			    jump_flags = MOUSE_MAY_STOP_VIS;
			}
			இல்லையெனில் எனில் (VIsual_mode == Ctrl_V)
			{
			    getvcols(curwin, &curwin->w_cursor, &VIsual,
						     &leftcol, &rightcol, 0);
			    getvcol(curwin, &m_pos, வெற்று, &m_pos.col, வெற்று, 0);
			    எனில் (m_pos.col < leftcol || m_pos.col > rightcol)
				jump_flags = MOUSE_MAY_STOP_VIS;
			}
		    }
		}
		இல்லையெனில்
		    jump_flags = MOUSE_MAY_STOP_VIS;
	    }
	    எனில் (jump_flags)
	    {
		jump_flags = jump_to_mouse(jump_flags, வெற்று, which_button);
		update_curbuf(VIsual_active ? UPD_INVERTED : UPD_VALID);
		setcursor();
		out_flush();    // Update before showing popup menu
	    }
# ifdef FEAT_MENU
	    show_popupmenu();
	    got_click = FALSE;	// ignore release events
# endif
	    திரும்பு (jump_flags & CURSOR_MOVED) != 0;
#இல்லையெனில்
	    திரும்பு FALSE;
#endif
	}
	எனில் (which_button == MOUSE_இடது
				&& (mod_mask & (MOD_MASK_SHIFT|MOD_MASK_ALT)))
	{
	    which_button = MOUSE_வலது;
	    mod_mask &= ~MOD_MASK_SHIFT;
	}
    }

    எனில் ((State & (MODE_NORMAL | MODE_செருகு))
			    && !(mod_mask & (MOD_MASK_SHIFT | MOD_MASK_CTRL)))
    {
	எனில் (which_button == MOUSE_இடது)
	{
	    எனில் (is_click)
	    {
		// stop Visual mode for a left click in a window, but not when
		// on a status line
		எனில் (VIsual_active)
		    jump_flags |= MOUSE_MAY_STOP_VIS;
	    }
	    இல்லையெனில் எனில் (mouse_has(MOUSE_VISUAL))
		jump_flags |= MOUSE_MAY_VIS;
	}
	இல்லையெனில் எனில் (which_button == MOUSE_வலது)
	{
	    எனில் (is_click && VIsual_active)
	    {
		// Remember the start and end of visual before moving the
		// cursor.
		எனில் (LT_POS(curwin->w_cursor, VIsual))
		{
		    start_visual = curwin->w_cursor;
		    end_visual = VIsual;
		}
		இல்லையெனில்
		{
		    start_visual = VIsual;
		    end_visual = curwin->w_cursor;
		}
	    }
	    jump_flags |= MOUSE_FOCUS;
	    எனில் (mouse_has(MOUSE_VISUAL))
		jump_flags |= MOUSE_MAY_VIS;
	}
    }

    // If an operator is pending, ignore all drags and releases until the
    // next mouse click.
    எனில் (!is_drag && oap != வெற்று && oap->op_type != OP_NOP)
    {
	got_click = FALSE;
	oap->motion_type = MCHAR;
    }

    // When releasing the button let jump_to_mouse() know.
    எனில் (!is_click && !is_drag)
	jump_flags |= MOUSE_RELEASED;

    // JUMP!
    jump_flags = jump_to_mouse(jump_flags,
			oap == வெற்று ? வெற்று : &(oap->inclusive), which_button);

#ifdef FEAT_MENU
    // A click in the window toolbar has no side effects.
    எனில் (jump_flags & MOUSE_WINBAR)
	திரும்பு FALSE;
#endif
    moved = (jump_flags & CURSOR_MOVED);
    in_status_வரி = (jump_flags & IN_STATUS_வரி);
    in_sep_வரி = (jump_flags & IN_SEP_வரி);

    // Check for statusline click handler early, before visual mode or
    // other button-specific handling can interfere.
    எனில் (in_status_வரி && is_click && !is_drag
	    && stl_click_handler(dragwin, mouse_row, mouse_col,
						which_button, mod_mask))
    {
#ifdef FEAT_MOUSESHAPE
	எனில் (!drag_status_வரி)
	{
	    drag_status_வரி = TRUE;
	    update_mouseshape(-1);
	}
#endif
	திரும்பு FALSE;
    }

#ifdef FEAT_NETBEANS_INTG
    எனில் (isNetbeansBuffer(curbuf)
			    && !(jump_flags & (IN_STATUS_வரி | IN_SEP_வரி)))
    {
	முழு சாவி = KEY2TERMCAP1(c);

	எனில் (சாவி == (முழு)KE_LEFTRELEASE || சாவி == (முழு)KE_MIDDLERELEASE
					       || சாவி == (முழு)KE_RIGHTRELEASE)
	    netbeans_button_release(which_button);
    }
#endif

    // When jumping to another window, clear a pending operator.  That's a bit
    // friendlier than beeping and not jumping to that window.
    எனில் (curwin != old_curwin && oap != வெற்று && oap->op_type != OP_NOP)
	clearop(oap);

#ifdef FEAT_FOLDING
    எனில் (mod_mask == 0
	    && !is_drag
	    && (jump_flags & (MOUSE_FOLD_CLOSE | MOUSE_FOLD_OPEN))
	    && which_button == MOUSE_இடது)
    {
	// open or close a fold at this line
	எனில் (jump_flags & MOUSE_FOLD_OPEN)
	    openFold(curwin->w_cursor.lnum, 1L);
	இல்லையெனில்
	    closeFold(curwin->w_cursor.lnum, 1L);
	// don't move the cursor if still in the same window
	எனில் (curwin == old_curwin)
	    curwin->w_cursor = save_cursor;
    }
#endif

#எனில் defined(FEAT_CLIPBOARD)
    எனில் ((jump_flags & IN_OTHER_WIN) && !VIsual_active &&
	    (clip_star.available || clip_plus.available))
    {
	clip_modeless(which_button, is_click, is_drag);
	திரும்பு FALSE;
    }
#endif

    // Set global flag that we are extending the Visual area with mouse
    // dragging; temporarily minimize 'scrolloff'.
    எனில் (VIsual_active && is_drag && get_scrolloff_மதிப்பு())
    {
	// In the very first line, allow scrolling one line
	எனில் (mouse_row == 0)
	    mouse_dragging = 2;
	இல்லையெனில்
	    mouse_dragging = 1;
    }

    // When dragging the mouse above the window, scroll down.
    எனில் (is_drag && mouse_row < 0 && !in_status_வரி)
    {
	scroll_redraw(FALSE, 1L);
	mouse_row = 0;
    }

    எனில் (start_visual.lnum)		// right click in visual mode
    {
       // When ALT is pressed make Visual mode blockwise.
       எனில் (mod_mask & MOD_MASK_ALT)
	   VIsual_mode = Ctrl_V;

	// In Visual-block mode, divide the area in four, pick up the corner
	// that is in the quarter that the cursor is in.
	எனில் (VIsual_mode == Ctrl_V)
	{
	    getvcols(curwin, &start_visual, &end_visual,
						    &leftcol, &rightcol, 0);
	    எனில் (curwin->w_curswant > (leftcol + rightcol) / 2)
		end_visual.col = leftcol;
	    இல்லையெனில்
		end_visual.col = rightcol;
	    எனில் (curwin->w_cursor.lnum >=
				    (start_visual.lnum + end_visual.lnum) / 2)
		end_visual.lnum = start_visual.lnum;

	    // move VIsual to the right column
	    start_visual = curwin->w_cursor;	    // save the cursor pos
	    curwin->w_cursor = end_visual;
	    coladvance(end_visual.col);
	    VIsual = curwin->w_cursor;
	    curwin->w_cursor = start_visual;	    // restore the cursor
	}
	இல்லையெனில்
	{
	    // If the click is before the start of visual, change the start.
	    // If the click is after the end of visual, change the end.  If
	    // the click is inside the visual, change the closest side.
	    எனில் (LT_POS(curwin->w_cursor, start_visual))
		VIsual = end_visual;
	    இல்லையெனில் எனில் (LT_POS(end_visual, curwin->w_cursor))
		VIsual = start_visual;
	    இல்லையெனில்
	    {
		// In the same line, compare column number
		எனில் (end_visual.lnum == start_visual.lnum)
		{
		    எனில் (curwin->w_cursor.col - start_visual.col >
				    end_visual.col - curwin->w_cursor.col)
			VIsual = start_visual;
		    இல்லையெனில்
			VIsual = end_visual;
		}

		// In different lines, compare line number
		இல்லையெனில்
		{
		    diff = (curwin->w_cursor.lnum - start_visual.lnum) -
				(end_visual.lnum - curwin->w_cursor.lnum);

		    எனில் (diff > 0)		// closest to end
			VIsual = start_visual;
		    இல்லையெனில் எனில் (diff < 0)	// closest to start
			VIsual = end_visual;
		    இல்லையெனில்			// in the middle line
		    {
			எனில் (curwin->w_cursor.col <
					(start_visual.col + end_visual.col) / 2)
			    VIsual = end_visual;
			இல்லையெனில்
			    VIsual = start_visual;
		    }
		}
	    }
	}
    }
    // If Visual mode started in insert mode, execute "CTRL-O"
    இல்லையெனில் எனில் ((State & MODE_செருகு) && VIsual_active)
	stuffcharReadbuff(Ctrl_O);

    // Middle mouse click: Put text before cursor.
    எனில் (which_button == MOUSE_நடு)
    {
#ifdef FEAT_CLIPBOARD
	எனில் (clip_star.available && regname == 0)
	    regname = '*';
#endif
	எனில் (yank_register_mline(regname))
	{
	    எனில் (mouse_past_bottom)
		dir = FORWARD;
	}
	இல்லையெனில் எனில் (mouse_past_eol)
	    dir = FORWARD;

	எனில் (fixindent)
	{
	    c1 = (dir == BACKWARD) ? '[' : ']';
	    c2 = 'p';
	}
	இல்லையெனில்
	{
	    c1 = (dir == FORWARD) ? 'p' : 'P';
	    c2 = NUL;
	}
	prep_redo(regname, மொத்தம், NUL, c1, NUL, c2, NUL);

	// Remember where the paste started, so in edit() Insstart can be set
	// to this position
	எனில் (restart_edit != 0)
	    where_paste_started = curwin->w_cursor;
	do_put(regname, வெற்று, dir, மொத்தம், fixindent | PUT_CURSEND);
    }

#எனில் defined(FEAT_QUICKFIX)
    // Ctrl-Mouse click or double click in a quickfix window jumps to the
    // error under the mouse pointer.
    இல்லையெனில் எனில் (((mod_mask & MOD_MASK_CTRL)
		|| (mod_mask & MOD_MASK_MULTI_CLICK) == MOD_MASK_2CLICK)
	    && bt_quickfix(curbuf))
    {
	எனில் (curwin->w_llist_ref == வெற்று)	// quickfix window
	    do_cmdline_cmd((char_u *)".cc");
	இல்லையெனில்					// location list window
	    do_cmdline_cmd((char_u *)".ll");
	got_click = FALSE;		// ignore drag&release now
    }
#endif

    // Ctrl-Mouse click (or double click in a help window) jumps to the tag
    // under the mouse pointer.
    இல்லையெனில் எனில் ((mod_mask & MOD_MASK_CTRL) || (curbuf->b_help
		     && (mod_mask & MOD_MASK_MULTI_CLICK) == MOD_MASK_2CLICK))
    {
	எனில் (State & MODE_செருகு)
	    stuffcharReadbuff(Ctrl_O);
	stuffcharReadbuff(Ctrl_RSB);
	got_click = FALSE;		// ignore drag&release now
    }

    // Shift-Mouse click searches for the next occurrence of the word under
    // the mouse pointer
    இல்லையெனில் எனில் ((mod_mask & MOD_MASK_SHIFT))
    {
	எனில் ((State & MODE_செருகு) || (VIsual_active && VIsual_select))
	    stuffcharReadbuff(Ctrl_O);
	எனில் (which_button == MOUSE_இடது)
	    stuffcharReadbuff('*');
	இல்லையெனில்	// MOUSE_RIGHT
	    stuffcharReadbuff('#');
    }

    // Handle double clicks, unless on status line
    இல்லையெனில் எனில் (in_status_வரி)
    {
#ifdef FEAT_MOUSESHAPE
	எனில் ((is_drag || is_click) && !drag_status_வரி)
	{
	    drag_status_வரி = TRUE;
	    update_mouseshape(-1);
	}
#endif
    }
    இல்லையெனில் எனில் (in_sep_வரி)
    {
#ifdef FEAT_MOUSESHAPE
	எனில் ((is_drag || is_click) && !drag_sep_வரி)
	{
	    drag_sep_வரி = TRUE;
	    update_mouseshape(-1);
	}
#endif
    }
    இல்லையெனில் எனில் ((mod_mask & MOD_MASK_MULTI_CLICK)
				       && (State & (MODE_NORMAL | MODE_செருகு))
	     && mouse_has(MOUSE_VISUAL))
    {
	எனில் (is_click || !VIsual_active)
	{
	    எனில் (VIsual_active)
		orig_cursor = VIsual;
	    இல்லையெனில்
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
	    எனில் ((mod_mask & MOD_MASK_MULTI_CLICK) == MOD_MASK_2CLICK)
	    {
		// Double click with ALT pressed makes it blockwise.
		எனில் (mod_mask & MOD_MASK_ALT)
		    VIsual_mode = Ctrl_V;
		இல்லையெனில்
		    VIsual_mode = 'v';
	    }
	    இல்லையெனில் எனில் ((mod_mask & MOD_MASK_MULTI_CLICK) == MOD_MASK_3CLICK)
		VIsual_mode = 'V';
	    இல்லையெனில் எனில் ((mod_mask & MOD_MASK_MULTI_CLICK) == MOD_MASK_4CLICK)
		VIsual_mode = Ctrl_V;
#ifdef FEAT_CLIPBOARD
	    // Make sure the clipboard gets updated.  Needed because start and
	    // end may still be the same, and the selection needs to be owned
	    clip_star.vmode = NUL;
#endif
	}
	// A double click selects a word or a block.
	எனில் ((mod_mask & MOD_MASK_MULTI_CLICK) == MOD_MASK_2CLICK)
	{
	    pos_T	*pos = வெற்று;
	    முழு		gc;

	    எனில் (is_click)
	    {
		// If the character under the cursor (skipping white space) is
		// not a word character, try finding a match and select a (),
		// {}, [], #if/#endif, etc. block.
		end_visual = curwin->w_cursor;
		போது (gc = gchar_pos(&end_visual), VIM_ISWHITE(gc))
		    inc(&end_visual);
		எனில் (oap != வெற்று)
		    oap->motion_type = MCHAR;
		எனில் (oap != வெற்று
			&& VIsual_mode == 'v'
			&& !vim_iswordc(gchar_pos(&end_visual))
			&& EQUAL_POS(curwin->w_cursor, VIsual)
			&& (pos = findmatch(oap, NUL)) != வெற்று)
		{
		    curwin->w_cursor = *pos;
		    எனில் (oap->motion_type == MLINE)
			VIsual_mode = 'V';
		    இல்லையெனில் எனில் (*p_sel == 'e')
		    {
			எனில் (LT_POS(curwin->w_cursor, VIsual))
			    ++VIsual.col;
			இல்லையெனில்
			    ++curwin->w_cursor.col;
		    }
		}
	    }

	    எனில் (pos == வெற்று && (is_click || is_drag))
	    {
		// When not found a match or when dragging: extend to include
		// a word.
		எனில் (LT_POS(curwin->w_cursor, orig_cursor))
		{
		    find_start_of_word(&curwin->w_cursor);
		    find_end_of_word(&VIsual);
		}
		இல்லையெனில்
		{
		    find_start_of_word(&VIsual);
		    எனில் (*p_sel == 'e' && *ml_get_cursor() != NUL)
			curwin->w_cursor.col +=
					 (*mb_ptr2len)(ml_get_cursor());
		    find_end_of_word(&curwin->w_cursor);
		}
	    }
	    curwin->w_set_curswant = true;
	}
	எனில் (is_click)
	    redraw_curbuf_later(UPD_INVERTED);	// update the inversion
    }
    இல்லையெனில் எனில் (VIsual_active && !old_active)
    {
	எனில் (mod_mask & MOD_MASK_ALT)
	    VIsual_mode = Ctrl_V;
	இல்லையெனில்
	    VIsual_mode = 'v';
    }

    // If Visual mode changed show it later.
    எனில் ((!VIsual_active && old_active && mode_displayed)
	    || (VIsual_active && p_smd && செய்தி_silent == 0
				 && (!old_active || VIsual_mode != old_mode)))
	redraw_cmdline = TRUE;

    திரும்பு moved;
}

    வெறுமை
ins_mouse(முழு c)
{
    pos_T	tpos;
    win_T	*old_curwin = curwin;

#ifdef FEAT_GUI
    // When GUI is active, also move/paste when 'mouse' is empty
    எனில் (!gui.in_use)
#endif
	எனில் (!mouse_has(MOUSE_செருகு))
	    திரும்பு;

    undisplay_dollar();
    tpos = curwin->w_cursor;
    எனில் (do_mouse(வெற்று, c, BACKWARD, 1L, 0))
    {
	win_T	*new_curwin = curwin;

	எனில் (curwin != old_curwin && win_valid(old_curwin))
	{
	    // Mouse took us to another window.  We need to go back to the
	    // previous one to stop insert there properly.
	    curwin = old_curwin;
	    curbuf = curwin->w_buffer;
#ifdef FEAT_JOB_CHANNEL
	    எனில் (bt_prompt(curbuf))
		// Restart Insert mode when re-entering the prompt buffer.
		curbuf->b_prompt_செருகு = 'A';
#endif
	}
	start_arrow(curwin == old_curwin ? &tpos : வெற்று);
	எனில் (curwin != new_curwin && win_valid(new_curwin))
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
    நிலையான வெறுமை
do_mousescroll(cmdarg_T *cap)
{
    முழு shift_or_ctrl = mod_mask & (MOD_MASK_SHIFT | MOD_MASK_CTRL);

#ifdef FEAT_TERMINAL
    எனில் (term_use_loop())
	// This window is a terminal window, send the mouse event there.
	// Set "typed" to FALSE to avoid an endless loop.
	send_சாவிகள்_to_term(curbuf->b_term, cap->cmdchar, mod_mask, FALSE);
    இல்லையெனில்
#endif
    எனில் (cap->arg == MSCR_UP || cap->arg == MSCR_DOWN)
    {
	// Vertical scrolling
	எனில் (!(State & MODE_செருகு) && (mouse_vert_step < 0 || shift_or_ctrl))
	{
	    // whole page up or down
	    pagescroll(cap->arg == MSCR_UP ? FORWARD : BACKWARD, 1L, FALSE);
	}
	இல்லையெனில்
	{
	    எனில் (mouse_vert_step < 0 || shift_or_ctrl)
	    {
		// whole page up or down
		cap->count1 = (long)(curwin->w_botline - curwin->w_topline);
	    }
	    // Don't scroll more than half the window height.
	    இல்லையெனில் எனில் (curwin->w_height < mouse_vert_step * 2)
	    {
		cap->count1 = curwin->w_height / 2;
		எனில் (cap->count1 == 0)
		    cap->count1 = 1;
	    }
	    இல்லையெனில்
	    {
		cap->count1 = mouse_vert_step;
	    }
	    cap->count0 = cap->count1;
	    nv_scroll_வரி(cap);
	}

#ifdef FEAT_PROP_POPUP
	எனில் (WIN_IS_POPUP(curwin))
	    popup_set_firstline(curwin);
#endif
    }
    இல்லையெனில்
    {
	// Horizontal scrolling
	long step = (mouse_hor_step < 0 || shift_or_ctrl)
					    ? curwin->w_width : mouse_hor_step;
	long leftcol = curwin->w_leftcol
				     + (cap->arg == MSCR_வலது ? -step : step);
	எனில் (leftcol < 0)
	    leftcol = 0;
	do_mousescroll_horiz((long_u)leftcol);
    }
    may_trigger_win_scrolled_resized();
}

/*
 * Insert mode implementation for scrolling in direction "dir", which is
 * one of the MSCR_ values.
 */
    வெறுமை
ins_mousescroll(முழு dir)
{
    cmdarg_T	cap;
    oparg_T	oa;
    CLEAR_FIELD(cap);
    clear_oparg(&oa);
    cap.oap = &oa;
    cap.arg = dir;

#ifdef FEAT_TABPANEL
    எனில் (mouse_row >= 0 && mouse_col >= 0
	    && (dir == MSCR_UP || dir == MSCR_DOWN)
	    && mouse_on_tabpanel())
    {
	(வெறுமை)tabpanel_scroll(dir == MSCR_UP ? 1 : -1,
		mouse_vert_step > 0 ? mouse_vert_step : 3);
	திரும்பு;
    }
#endif

    தேர்வு (dir)
    {
	நிலை MSCR_UP:
	    cap.cmdchar = K_MOUSEUP;
	    நிறுத்து;
	நிலை MSCR_DOWN:
	    cap.cmdchar = K_MOUSEDOWN;
	    நிறுத்து;
	நிலை MSCR_இடது:
	    cap.cmdchar = K_MOUSELEFT;
	    நிறுத்து;
	நிலை MSCR_வலது:
	    cap.cmdchar = K_MOUSERIGHT;
	    நிறுத்து;
	இயல்பு:
	    siemsg("Invalid ins_mousescroll() argument: %d", dir);
    }

    win_T *old_curwin = curwin;
    எனில் (mouse_row >= 0 && mouse_col >= 0)
    {
	// Find the window at the mouse pointer coordinates.
	// NOTE: Must restore "curwin" to "old_curwin" before returning!
	முழு row = mouse_row;
	முழு col = mouse_col;
	curwin = mouse_find_win(&row, &col, FIND_POPUP);
	எனில் (curwin == வெற்று)
	{
	    curwin = old_curwin;
	    திரும்பு;
	}
	curbuf = curwin->w_buffer;
    }

    எனில் (curwin == old_curwin)
    {
	// Don't scroll the current window if the popup menu is visible.
	எனில் (pum_visible())
	    திரும்பு;

	undisplay_dollar();
    }

    linenr_T	orig_topline = curwin->w_topline;
    colnr_T	orig_leftcol = curwin->w_leftcol;
    pos_T	orig_cursor = curwin->w_cursor;

    // Call the common mouse scroll function shared with other modes.
    do_mousescroll(&cap);

    முழு did_scroll = (orig_topline != curwin->w_topline
		   || orig_leftcol != curwin->w_leftcol);

    curwin->w_redr_status = true;
    curwin = old_curwin;
    curbuf = curwin->w_buffer;

    // If the window actually scrolled and the popup menu may overlay the
    // window, need to redraw it.
    எனில் (did_scroll && pum_visible())
    {
	// TODO: Would be more efficient to only redraw the windows that are
	// overlapped by the popup menu.
	redraw_all_later(UPD_NOT_VALID);
	ins_compl_show_pum();
    }

    எனில் (!EQUAL_POS(curwin->w_cursor, orig_cursor))
    {
	start_arrow(&orig_cursor);
	set_can_cindent(TRUE);
    }
}

#எனில் defined(FEAT_PROP_POPUP)
/*
 * Command-line mode implementation for scrolling in direction "dir", which is
 * one of the MSCR_ values.  Scrolls the completion info popup when the mouse
 * pointer is on top of it.
 * Returns TRUE when the info popup was scrolled.
 */
    முழு
cmdline_mousescroll(முழு dir)
{
    cmdarg_T	cap;
    oparg_T	oa;

    CLEAR_FIELD(cap);
    clear_oparg(&oa);
    cap.oap = &oa;
    cap.arg = dir;

    தேர்வு (dir)
    {
	நிலை MSCR_UP:	 cap.cmdchar = K_MOUSEUP; நிறுத்து;
	நிலை MSCR_DOWN:	 cap.cmdchar = K_MOUSEDOWN; நிறுத்து;
	நிலை MSCR_இடது:	 cap.cmdchar = K_MOUSELEFT; நிறுத்து;
	நிலை MSCR_வலது: cap.cmdchar = K_MOUSERIGHT; நிறுத்து;
    }

    எனில் (mouse_row < 0 || mouse_col < 0)
	திரும்பு FALSE;

    முழு	    row = mouse_row;
    முழு	    col = mouse_col;
    win_T   *wp;

    // Only scroll when the mouse is on top of the info popup.
    wp = mouse_find_win(&row, &col, FIND_POPUP);
    எனில் (wp == வெற்று || !WIN_IS_POPUP(wp) || !(wp->w_popup_flags & POPF_INFO)
							|| !wp->w_has_scrollbar)
	திரும்பு FALSE;

    win_T   *old_curwin = curwin;

    curwin = wp;
    curbuf = wp->w_buffer;
    // Call the common mouse scroll function shared with other modes.
    do_mousescroll(&cap);
    curwin = old_curwin;
    curbuf = curwin->w_buffer;

    // Cmdline mode doesn't normally call update_screen(), so redraw the
    // completion popup menu, which also repaints the info popup.
    எனில் (cmdline_pum_active())
	cmdline_pum_display();
    திரும்பு TRUE;
}
#endif

/*
 * Return TRUE if "c" is a mouse key.
 */
    முழு
is_mouse_சாவி(முழு c)
{
    திரும்பு c == K_LEFTMOUSE
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

நிலையான கட்டமைப்பு mousetable
{
    முழு	    pseudo_code;	// Code for pseudo mouse event
    முழு	    button;		// Which mouse button is it?
    முழு	    is_click;		// Is it a mouse button click event?
    முழு	    is_drag;		// Is it a mouse drag event?
} mouse_table[] =
{
    {(முழு)KE_LEFTMOUSE,		MOUSE_இடது,	TRUE,	FALSE},
#ifdef FEAT_GUI
    {(முழு)KE_LEFTMOUSE_NM,	MOUSE_இடது,	TRUE,	FALSE},
#endif
    {(முழு)KE_LEFTDRAG,		MOUSE_இடது,	FALSE,	TRUE},
    {(முழு)KE_LEFTRELEASE,	MOUSE_இடது,	FALSE,	FALSE},
#ifdef FEAT_GUI
    {(முழு)KE_LEFTRELEASE_NM,	MOUSE_இடது,	FALSE,	FALSE},
#endif
    {(முழு)KE_MIDDLEMOUSE,	MOUSE_நடு,	TRUE,	FALSE},
    {(முழு)KE_MIDDLEDRAG,	MOUSE_நடு,	FALSE,	TRUE},
    {(முழு)KE_MIDDLERELEASE,	MOUSE_நடு,	FALSE,	FALSE},
    {(முழு)KE_RIGHTMOUSE,	MOUSE_வலது,	TRUE,	FALSE},
    {(முழு)KE_RIGHTDRAG,		MOUSE_வலது,	FALSE,	TRUE},
    {(முழு)KE_RIGHTRELEASE,	MOUSE_வலது,	FALSE,	FALSE},
    {(முழு)KE_X1MOUSE,		MOUSE_X1,	TRUE,	FALSE},
    {(முழு)KE_X1DRAG,		MOUSE_X1,	FALSE,	TRUE},
    {(முழு)KE_X1RELEASE,		MOUSE_X1,	FALSE,	FALSE},
    {(முழு)KE_X2MOUSE,		MOUSE_X2,	TRUE,	FALSE},
    {(முழு)KE_X2DRAG,		MOUSE_X2,	FALSE,	TRUE},
    {(முழு)KE_X2RELEASE,		MOUSE_X2,	FALSE,	FALSE},
    // DRAG without CLICK
    {(முழு)KE_MOUSEMOVE,		MOUSE_RELEASE,	FALSE,	TRUE},
    // RELEASE without CLICK
    {(முழு)KE_IGNORE,		MOUSE_RELEASE,	FALSE,	FALSE},
    {0,				0,		0,	0},
};

/*
 * Look up the given mouse code to return the relevant information in the other
 * arguments.  Return which button is down or was released.
 */
    முழு
get_mouse_button(முழு code, முழு *is_click, முழு *is_drag)
{
    முழு	    i;

    க்கு (i = 0; mouse_table[i].pseudo_code; i++)
	எனில் (code == mouse_table[i].pseudo_code)
	{
	    *is_click = mouse_table[i].is_click;
	    *is_drag = mouse_table[i].is_drag;
	    திரும்பு mouse_table[i].button;
	}
    திரும்பு 0;	    // Shouldn't get here
}

/*
 * Return the appropriate pseudo mouse event token (KE_LEFTMOUSE etc) based on
 * the given information about which mouse button is down, and whether the
 * mouse was clicked, dragged or released.
 */
    முழு
get_pseudo_mouse_code(
    முழு	    button,	// eg MOUSE_LEFT
    முழு	    is_click,
    முழு	    is_drag)
{
    முழு	    i;

    க்கு (i = 0; mouse_table[i].pseudo_code; i++)
	எனில் (button == mouse_table[i].button
	    && is_click == mouse_table[i].is_click
	    && is_drag == mouse_table[i].is_drag)
	{
#ifdef FEAT_GUI
	    // Trick: a non mappable left click and release has mouse_col -1
	    // or added MOUSE_COLOFF.  Used for 'mousefocus' in
	    // gui_mouse_moved()
	    எனில் (mouse_col < 0 || mouse_col > MOUSE_COLOFF)
	    {
		எனில் (mouse_col < 0)
		    mouse_col = 0;
		இல்லையெனில்
		    mouse_col -= MOUSE_COLOFF;
		எனில் (mouse_table[i].pseudo_code == (முழு)KE_LEFTMOUSE)
		    திரும்பு (முழு)KE_LEFTMOUSE_NM;
		எனில் (mouse_table[i].pseudo_code == (முழு)KE_LEFTRELEASE)
		    திரும்பு (முழு)KE_LEFTRELEASE_NM;
	    }
#endif
	    திரும்பு mouse_table[i].pseudo_code;
	}
    திரும்பு (முழு)KE_IGNORE;	    // not recognized, ignore it
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
நிலையான முழு has_mouse_termcode = 0;

    வெறுமை
set_mouse_termcode(
    முழு		n,	// KS_MOUSE, KS_NETTERM_MOUSE or KS_DEC_MOUSE
    char_u	*s)
{
    char_u	name[2];

    name[0] = n;
    name[1] = KE_FILLER;
    add_termcode(name, s, FALSE);
#ifdef FEAT_MOUSE_JSB
    எனில் (n == KS_JSBTERM_MOUSE)
	has_mouse_termcode |= HMT_JSBTERM;
    இல்லையெனில்
#endif
#ifdef FEAT_MOUSE_NET
    எனில் (n == KS_NETTERM_MOUSE)
	has_mouse_termcode |= HMT_NETTERM;
    இல்லையெனில்
#endif
#ifdef FEAT_MOUSE_DEC
    எனில் (n == KS_DEC_MOUSE)
	has_mouse_termcode |= HMT_DEC;
    இல்லையெனில்
#endif
#ifdef FEAT_MOUSE_PTERM
    எனில் (n == KS_PTERM_MOUSE)
	has_mouse_termcode |= HMT_PTERM;
    இல்லையெனில்
#endif
#ifdef FEAT_MOUSE_URXVT
    எனில் (n == KS_URXVT_MOUSE)
	has_mouse_termcode |= HMT_URXVT;
    இல்லையெனில்
#endif
#ifdef FEAT_MOUSE_GPM
    எனில் (n == KS_GPM_MOUSE)
	has_mouse_termcode |= HMT_GPM;
    இல்லையெனில்
#endif
    எனில் (n == KS_SGR_MOUSE)
	has_mouse_termcode |= HMT_SGR;
    இல்லையெனில் எனில் (n == KS_SGR_MOUSE_RELEASE)
	has_mouse_termcode |= HMT_SGR_REL;
    இல்லையெனில்
	has_mouse_termcode |= HMT_NORMAL;
}

#எனில் defined(UNIX) || defined(VMS)
    வெறுமை
del_mouse_termcode(
    முழு		n)	// KS_MOUSE, KS_NETTERM_MOUSE or KS_DEC_MOUSE
{
    char_u	name[2];

    name[0] = n;
    name[1] = KE_FILLER;
    del_termcode(name);
# ifdef FEAT_MOUSE_JSB
    எனில் (n == KS_JSBTERM_MOUSE)
	has_mouse_termcode &= ~HMT_JSBTERM;
    இல்லையெனில்
# endif
# ifdef FEAT_MOUSE_NET
    எனில் (n == KS_NETTERM_MOUSE)
	has_mouse_termcode &= ~HMT_NETTERM;
    இல்லையெனில்
# endif
# ifdef FEAT_MOUSE_DEC
    எனில் (n == KS_DEC_MOUSE)
	has_mouse_termcode &= ~HMT_DEC;
    இல்லையெனில்
# endif
# ifdef FEAT_MOUSE_PTERM
    எனில் (n == KS_PTERM_MOUSE)
	has_mouse_termcode &= ~HMT_PTERM;
    இல்லையெனில்
# endif
# ifdef FEAT_MOUSE_URXVT
    எனில் (n == KS_URXVT_MOUSE)
	has_mouse_termcode &= ~HMT_URXVT;
    இல்லையெனில்
# endif
# ifdef FEAT_MOUSE_GPM
    எனில் (n == KS_GPM_MOUSE)
	has_mouse_termcode &= ~HMT_GPM;
    இல்லையெனில்
# endif
    எனில் (n == KS_SGR_MOUSE)
	has_mouse_termcode &= ~HMT_SGR;
    இல்லையெனில் எனில் (n == KS_SGR_MOUSE_RELEASE)
	has_mouse_termcode &= ~HMT_SGR_REL;
    இல்லையெனில்
	has_mouse_termcode &= ~HMT_NORMAL;
}
#endif

/*
 * setmouse() - switch mouse on/off depending on current mode and 'mouse'
 */
    வெறுமை
setmouse(வெறுமை)
{
    முழு	    checkfor;

#ifdef FEAT_MOUSESHAPE
    update_mouseshape(-1);
#endif

    // Should be outside proc, but may break MOUSESHAPE
#ifdef FEAT_GUI
    // In the GUI the mouse is always enabled.
    எனில் (gui.in_use)
	திரும்பு;
#endif
    // be quick when mouse is off
    எனில் (*p_mouse == NUL || has_mouse_termcode == 0)
	திரும்பு;

    // don't switch mouse on when not in raw mode (Ex mode)
    எனில் (cur_tmode != TMODE_RAW)
    {
	mch_setmouse(FALSE);
	திரும்பு;
    }

    எனில் (VIsual_active)
	checkfor = MOUSE_VISUAL;
    இல்லையெனில் எனில் (State == MODE_HITRETURN || State == MODE_ASKMORE
						     || State == MODE_SETWSIZE)
	checkfor = MOUSE_RETURN;
    இல்லையெனில் எனில் (State & MODE_செருகு)
	checkfor = MOUSE_செருகு;
    இல்லையெனில் எனில் (State & MODE_CMDLINE)
	checkfor = MOUSE_COMMAND;
    இல்லையெனில் எனில் (State == MODE_CONFIRM || State == MODE_EXTERNCMD)
	checkfor = ' '; // don't use mouse for ":confirm" or ":!cmd"
    இல்லையெனில்
	checkfor = MOUSE_NORMAL;    // assume normal mode

    எனில் (mouse_has(checkfor))
	mch_setmouse(TRUE);
    இல்லையெனில்
	mch_setmouse(FALSE);
}

/*
 * Return TRUE if
 * - "c" is in 'mouse', or
 * - 'a' is in 'mouse' and "c" is in MOUSE_A, or
 * - the current buffer is a help file and 'h' is in 'mouse' and we are in a
 *   normal editing mode (not at hit-return message).
 */
    முழு
mouse_has(முழு c)
{
    char_u	*p;

    க்கு (p = p_mouse; *p; ++p)
	தேர்வு (*p)
	{
	    நிலை 'a': எனில் (vim_strchr((char_u *)MOUSE_A, c) != வெற்று)
			  திரும்பு TRUE;
		      நிறுத்து;
	    நிலை MOUSE_HELP: எனில் (c != MOUSE_RETURN && curbuf->b_help)
				 திரும்பு TRUE;
			     நிறுத்து;
	    இயல்பு: எனில் (c == *p) திரும்பு TRUE; நிறுத்து;
	}
    திரும்பு FALSE;
}

/*
 * Return TRUE when 'mousemodel' is set to "popup" or "popup_setpos".
 */
    முழு
mouse_model_popup(வெறுமை)
{
    திரும்பு (p_mousem[0] == 'p');
}

/*
 * Call a click-region callback function.
 * "regions"/"region_count" describe the resolved click regions,
 * "winid" is stored as the "winid" key in the info dict (0 for tabline).
 * Returns TRUE if the function was called and handled the click.
 */
    நிலையான முழு
stl_click_handler_regions(
	stl_click_region_T  *regions,
	முழு		    region_மொத்தம்,
	முழு		    winid,
	char_u		    *area_name,
	முழு		    mrow,
	முழு		    mcol,
	முழு		    which_button,
	முழு		    mods)
{
#ifdef FEAT_EVAL
    முழு		n;
    முழு		nclicks;
    char_u	button_str[2];
    char_u	mods_str[4];
    முழு		mi = 0;
    dict_T	*info;
    typval_T	argvars[2];
    typval_T	rettv;
    funcexe_T	funcexe;
    முழு		col = mcol;

    எனில் (regions == வெற்று || region_மொத்தம் == 0)
	திரும்பு FALSE;

    // Find the click region at the given row and column.
    க்கு (n = 0; n < region_மொத்தம்; n++)
    {
	எனில் (regions[n].row == mrow
		&& col >= regions[n].col_start
		&& col < regions[n].col_end)
	    நிறுத்து;
    }
    எனில் (n >= region_மொத்தம் || regions[n].funcname == வெற்று)
	திரும்பு FALSE;

    // Build the info dictionary.
    info = dict_alloc();
    எனில் (info == வெற்று)
	திரும்பு FALSE;

    dict_add_எண்(info, "minwid", regions[n].minwid);

    // Determine number of clicks.
    // MOD_MASK_2CLICK=0x20, MOD_MASK_3CLICK=0x40, MOD_MASK_4CLICK=0x60
    nclicks = ((mods & MOD_MASK_MULTI_CLICK) >> 5) + 1;
    எனில் (nclicks > 3)
	nclicks = 3;
    dict_add_எண்(info, "nclicks", nclicks);

    // Button.
    எனில் (which_button == MOUSE_இடது)
	button_str[0] = 'l';
    இல்லையெனில் எனில் (which_button == MOUSE_வலது)
	button_str[0] = 'r';
    இல்லையெனில்
	button_str[0] = 'm';
    button_str[1] = NUL;
    dict_add_string_len(info, "button", button_str, 1);

    // Modifiers.
    எனில் (mods & MOD_MASK_SHIFT)
	mods_str[mi++] = 's';
    எனில் (mods & MOD_MASK_CTRL)
	mods_str[mi++] = 'c';
    எனில் (mods & MOD_MASK_ALT)
	mods_str[mi++] = 'a';
    mods_str[mi] = NUL;
    dict_add_string_len(info, "mods", mods_str, mi);

    dict_add_எண்(info, "winid", winid);

    // "area": which option the clicked region belongs to.  Lets a shared
    // dispatcher distinguish 'statusline', 'tabline' and 'tabpanel' without
    // having to overload winid == 0.
    dict_add_string(info, "area", area_name);

    // Expose tab page number for 'tabpanel' regions.
    எனில் (regions[n].tabnr > 0)
	dict_add_எண்(info, "tabnr", regions[n].tabnr);

    // Call the function with the info dict as argument.
    argvars[0].v_type = VAR_DICT;
    argvars[0].vval.v_dict = info;
    ++info->dv_refcount;
    argvars[1].v_type = VAR_UNKNOWN;

    rettv.v_type = VAR_எண்;
    rettv.vval.v_எண் = 0;

    CLEAR_FIELD(funcexe);
    funcexe.fe_evaluate = TRUE;
    (வெறுமை)call_func(regions[n].funcname, -1,
					    &rettv, 1, argvars, &funcexe);

    n = (முழு)rettv.vval.v_எண்;
    clear_tv(&rettv);
    dict_unref(info);

    எனில் (n != 0)
    {
	// Make sure the tabline gets redrawn too when the callback asks for
	// a redraw (redraw_statuslines() only redraws the tabline when
	// redraw_tabline is set).  For tabpanel the whole screen needs to be
	// refreshed.
	எனில் (winid == 0)
	    redraw_tabline = TRUE;
# ifdef FEAT_TABPANEL
	எனில் (STRCMP(area_name, "tabpanel") == 0)
	    redraw_all_later(UPD_NOT_VALID);
# endif
	redraw_statuslines();
    }

    திரும்பு TRUE;
#இல்லையெனில்
    (வெறுமை)regions;
    (வெறுமை)region_மொத்தம்;
    (வெறுமை)winid;
    (வெறுமை)area_name;
    (வெறுமை)mrow;
    (வெறுமை)mcol;
    (வெறுமை)which_button;
    (வெறுமை)mods;
    திரும்பு FALSE;
#endif
}

/*
 * Call a statusline click handler function for window "wp".
 * Returns TRUE if the function was called and handled the click.
 */
    நிலையான முழு
stl_click_handler(win_T *wp, முழு mrow, முழு mcol, முழு which_button, முழு mods)
{
    எனில் (wp == வெற்று)
	திரும்பு FALSE;
    திரும்பு stl_click_handler_regions(wp->w_stl_click, wp->w_stl_click_மொத்தம்,
				wp->w_id, (char_u *)"statusline",
				mrow, mcol, which_button, mods);
}

// dragwin is declared near the top of the file

/*
 * Reset the window being dragged.  To be called when switching tab page.
 */
    வெறுமை
reset_dragwin(வெறுமை)
{
    dragwin = வெற்று;
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
    முழு
jump_to_mouse(
    முழு		flags,
    முழு		*inclusive,	// used for inclusive operator, can be NULL
    முழு		which_button)	// MOUSE_LEFT, MOUSE_RIGHT, MOUSE_MIDDLE
{
    நிலையான முழு	on_status_வரி = 0;	// #lines below bottom of window
    நிலையான முழு	on_sep_வரி = 0;	// on separator right of window
#ifdef FEAT_MENU
    நிலையான முழு  in_winbar = FALSE;
#endif
#ifdef FEAT_PROP_POPUP
    நிலையான முழு   in_popup_win = FALSE;
    நிலையான win_T *click_in_popup_win = வெற்று;
#endif
    நிலையான முழு	prev_row = -1;
    நிலையான முழு	prev_col = -1;
    நிலையான முழு	did_drag = FALSE;	// drag was noticed

    win_T	*wp, *old_curwin;
    pos_T	old_cursor;
    முழு		மொத்தம்;
    முழு		first;
    முழு		row = mouse_row;
    முழு		col = mouse_col;
    colnr_T	col_from_screen = -1;
#ifdef FEAT_FOLDING
    முழு		mouse_char = ' ';
#endif

    mouse_past_bottom = FALSE;
    mouse_past_eol = FALSE;

    எனில் (flags & MOUSE_RELEASED)
    {
	// On button release we may change window focus if positioned on a
	// status line and no dragging happened.
	எனில் (dragwin != வெற்று && !did_drag)
	    flags &= ~(MOUSE_FOCUS | MOUSE_DID_MOVE);
	dragwin = வெற்று;
	did_drag = FALSE;
#ifdef FEAT_PROP_POPUP
	எனில் (click_in_popup_win != வெற்று && popup_dragwin == வெற்று)
	    popup_close_for_mouse_click(click_in_popup_win);

	popup_dragwin = வெற்று;
	click_in_popup_win = வெற்று;
#endif
    }

    எனில் ((flags & MOUSE_DID_MOVE)
	    && prev_row == mouse_row
	    && prev_col == mouse_col)
    {
retnomove:
	// before moving the cursor for a left click which is NOT in a status
	// line, stop Visual mode
	எனில் (on_status_வரி)
	    திரும்பு IN_STATUS_வரி;
	எனில் (on_sep_வரி)
	    திரும்பு IN_SEP_வரி;
#ifdef FEAT_MENU
	எனில் (in_winbar)
	{
	    // A quick second click may arrive as a double-click, but we use it
	    // as a second click in the WinBar.
	    எனில் ((mod_mask & MOD_MASK_MULTI_CLICK) && !(flags & MOUSE_RELEASED))
	    {
		wp = mouse_find_win(&row, &col, FAIL_POPUP);
		எனில் (wp == வெற்று)
		    திரும்பு IN_UNKNOWN;
		winbar_click(wp, col);
	    }
	    திரும்பு IN_OTHER_WIN | MOUSE_WINBAR;
	}
#endif
	எனில் (flags & MOUSE_MAY_STOP_VIS)
	{
	    end_visual_mode_keep_button();
	    redraw_curbuf_later(UPD_INVERTED);	// delete the inversion
	}
#எனில் defined(FEAT_CLIPBOARD)
	// Continue a modeless selection in another window.
	எனில் (cmdwin_type != 0 && row < cmdwin_win->w_winrow)
	    திரும்பு IN_OTHER_WIN;
#endif
#ifdef FEAT_PROP_POPUP
	// Continue a modeless selection in a popup window or dragging it.
	எனில் (in_popup_win)
	{
	    click_in_popup_win = வெற்று;  // don't close it on release
	    எனில் (popup_dragwin != வெற்று)
	    {
		// dragging a popup window
		popup_drag(popup_dragwin);
		திரும்பு IN_UNKNOWN;
	    }
	    திரும்பு IN_OTHER_WIN;
	}
#endif
	திரும்பு IN_BUFFER;
    }

    prev_row = mouse_row;
    prev_col = mouse_col;

    எனில் (flags & MOUSE_SETPOS)
	செல் retnomove;				// ugly goto...

    old_curwin = curwin;
    old_cursor = curwin->w_cursor;

    எனில் (!(flags & MOUSE_FOCUS))
    {
	எனில் (row < 0 || col < 0) // check if it makes sense
	    திரும்பு IN_UNKNOWN;

	// find the window where the row is in and adjust "row" and "col" to be
	// relative to top-left of the window
	wp = mouse_find_win(&row, &col, FIND_POPUP);
	எனில் (wp == வெற்று)
	    திரும்பு IN_UNKNOWN;
	dragwin = வெற்று;

#ifdef FEAT_PROP_POPUP
	// Click in a popup window may start dragging or modeless selection,
	// but not much else.
	எனில் (WIN_IS_POPUP(wp))
	{
	    on_sep_வரி = 0;
	    on_status_வரி = 0;
	    in_popup_win = TRUE;
	    எனில் (which_button == MOUSE_இடது && popup_close_if_on_X(wp, row, col))
	    {
		திரும்பு IN_UNKNOWN;
	    }
	    இல்லையெனில் எனில் (((wp->w_popup_flags & (POPF_DRAG | POPF_RESIZE))
					      && popup_on_border(wp, row, col))
				       || (wp->w_popup_flags & POPF_DRAGALL))
	    {
		popup_dragwin = wp;
		popup_start_drag(wp, row, col);
		திரும்பு IN_UNKNOWN;
	    }
	    // Only close on release, otherwise it's not possible to drag or do
	    // modeless selection.
	    இல்லையெனில் எனில் (wp->w_popup_close == POPCLOSE_CLICK
		    && which_button == MOUSE_இடது)
	    {
		click_in_popup_win = wp;
	    }
	    இல்லையெனில் எனில் (which_button == MOUSE_இடது)
		// If the click is in the scrollbar, may scroll up/down.
		popup_handle_scrollbar_click(wp, row, col);
# ifdef FEAT_CLIPBOARD
	    திரும்பு IN_OTHER_WIN;
# இல்லையெனில்
	    திரும்பு IN_UNKNOWN;
# endif
	}
	in_popup_win = FALSE;
	popup_dragwin = வெற்று;
#endif
#ifdef FEAT_MENU
	எனில் (row == -1)
	{
	    // A click in the window toolbar does not enter another window or
	    // change Visual highlighting.
	    winbar_click(wp, col);
	    in_winbar = TRUE;
	    திரும்பு IN_OTHER_WIN | MOUSE_WINBAR;
	}
	in_winbar = FALSE;
#endif

	// winpos and height may change in win_enter()!
	எனில் (row >= wp->w_height)		// In (or below) status line
	{
	    on_status_வரி = row - wp->w_height + 1;
	    dragwin = wp;
	}
	இல்லையெனில்
	    on_status_வரி = 0;
	எனில் (col >= wp->w_width)		// In separator line
	{
	    on_sep_வரி = col - wp->w_width + 1;
	    dragwin = wp;
	}
	இல்லையெனில்
	    on_sep_வரி = 0;

	// The rightmost character of the status line might be a vertical
	// separator character if there is no connecting window to the right.
	எனில் (on_status_வரி && on_sep_வரி)
	{
	    எனில் (stl_connected(wp))
		on_sep_வரி = 0;
	    இல்லையெனில்
		on_status_வரி = 0;
	}

	// Before jumping to another buffer, or moving the cursor for a left
	// click, stop Visual mode.
	எனில் (VIsual_active
		&& (wp->w_buffer != curwin->w_buffer
		    || (!on_status_வரி && !on_sep_வரி
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
	எனில் (cmdwin_type != 0 && wp != cmdwin_win)
	{
	    // A click outside the command-line window: Use modeless
	    // selection if possible.  Allow dragging the status lines.
	    on_sep_வரி = 0;
#ifdef FEAT_CLIPBOARD
	    எனில் (on_status_வரி)
		திரும்பு IN_STATUS_வரி;
	    திரும்பு IN_OTHER_WIN;
#இல்லையெனில்
	    row = 0;
	    col += wp->w_wincol;
	    wp = cmdwin_win;
#endif
	}
#எனில் defined(FEAT_PROP_POPUP) && defined(FEAT_TERMINAL)
	எனில் (popup_is_popup(curwin) && curbuf->b_term != வெற்று)
	    // terminal in popup window: don't jump to another window
	    திரும்பு IN_OTHER_WIN;
#endif
	// Only change window focus when not clicking on or dragging the
	// status line.  Do change focus when releasing the mouse button
	// (MOUSE_FOCUS was set above if we dragged first).
	எனில் (dragwin == வெற்று || (flags & MOUSE_RELEASED))
	    win_enter(wp, TRUE);		// can make wp invalid!

	எனில் (curwin != old_curwin)
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
	எனில் (on_status_வரி)			// In (or below) status line
	{
	    // Don't use start_arrow() if we're in the same window
	    எனில் (curwin == old_curwin)
		திரும்பு IN_STATUS_வரி;
	    இல்லையெனில்
		திரும்பு IN_STATUS_வரி | CURSOR_MOVED;
	}
	எனில் (on_sep_வரி)			// In (or below) status line
	{
	    // Don't use start_arrow() if we're in the same window
	    எனில் (curwin == old_curwin)
		திரும்பு IN_SEP_வரி;
	    இல்லையெனில்
		திரும்பு IN_SEP_வரி | CURSOR_MOVED;
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
    இல்லையெனில் எனில் (on_status_வரி && which_button == MOUSE_இடது)
    {
	எனில் (dragwin != வெற்று)
	{
	    // Drag the status line
	    மொத்தம் = row - W_WINROW(dragwin) - dragwin->w_height + 1
							     - on_status_வரி;
	    win_drag_status_வரி(dragwin, மொத்தம்);
	    did_drag |= மொத்தம்;
	}
	திரும்பு IN_STATUS_வரி;			// Cursor didn't move
    }
    இல்லையெனில் எனில் (on_sep_வரி && which_button == MOUSE_இடது)
    {
	எனில் (dragwin != வெற்று)
	{
	    // Drag the separator column
	    மொத்தம் = col - dragwin->w_wincol - dragwin->w_width + 1
								- on_sep_வரி;
	    win_drag_vsep_வரி(dragwin, மொத்தம்);
	    did_drag |= மொத்தம்;
	}
	திரும்பு IN_SEP_வரி;			// Cursor didn't move
    }
#ifdef FEAT_MENU
    இல்லையெனில் எனில் (in_winbar)
    {
	// After a click on the window toolbar don't start Visual mode.
	திரும்பு IN_OTHER_WIN | MOUSE_WINBAR;
    }
#endif
    இல்லையெனில் // keep_window_focus must be TRUE
    {
	// before moving the cursor for a left click, stop Visual mode
	எனில் (flags & MOUSE_MAY_STOP_VIS)
	{
	    end_visual_mode_keep_button();
	    redraw_curbuf_later(UPD_INVERTED);	// delete the inversion
	}

#எனில் defined(FEAT_CLIPBOARD)
	// Continue a modeless selection in another window.
	எனில் (cmdwin_type != 0 && row < cmdwin_win->w_winrow)
	    திரும்பு IN_OTHER_WIN;
#endif
#ifdef FEAT_PROP_POPUP
	எனில் (in_popup_win)
	{
	    எனில் (popup_dragwin != வெற்று)
	    {
		// dragging a popup window
		popup_drag(popup_dragwin);
		திரும்பு IN_UNKNOWN;
	    }
	    // continue a modeless selection in a popup window
	    click_in_popup_win = வெற்று;
	    திரும்பு IN_OTHER_WIN;
	}
#endif

	row -= W_WINROW(curwin);
	col -= curwin->w_wincol;

	// When clicking beyond the end of the window, scroll the screen.
	// Scroll by however many rows outside the window we are.
	எனில் (row < 0)
	{
	    மொத்தம் = 0;
	    க்கு (first = TRUE; curwin->w_topline > 1; )
	    {
#ifdef FEAT_DIFF
		எனில் (curwin->w_topfill < diff_check_fill(curwin, curwin->w_topline))
		    ++மொத்தம்;
		இல்லையெனில்
#endif
		    மொத்தம் += plines(curwin->w_topline - 1);
		எனில் (!first && மொத்தம் > -row)
		    நிறுத்து;
		first = FALSE;
#ifdef FEAT_FOLDING
		(வெறுமை)hasFolding(curwin->w_topline, &curwin->w_topline, வெற்று);
#endif
#ifdef FEAT_DIFF
		எனில் (curwin->w_topfill < diff_check_fill(curwin, curwin->w_topline))
		    ++curwin->w_topfill;
		இல்லையெனில்
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
	இல்லையெனில் எனில் (row >= curwin->w_height)
	{
	    மொத்தம் = 0;
	    க்கு (first = TRUE; curwin->w_topline < curbuf->b_ml.ml_வரி_மொத்தம்; )
	    {
#ifdef FEAT_DIFF
		எனில் (curwin->w_topfill > 0)
		    ++மொத்தம்;
		இல்லையெனில்
#endif
		    மொத்தம் += plines(curwin->w_topline);
		எனில் (!first && மொத்தம் > row - curwin->w_height + 1)
		    நிறுத்து;
		first = FALSE;
#ifdef FEAT_FOLDING
		எனில் (hasFolding(curwin->w_topline, வெற்று, &curwin->w_topline)
			&& curwin->w_topline == curbuf->b_ml.ml_வரி_மொத்தம்)
		    நிறுத்து;
#endif
#ifdef FEAT_DIFF
		எனில் (curwin->w_topfill > 0)
		    --curwin->w_topfill;
		இல்லையெனில்
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
	இல்லையெனில் எனில் (row == 0)
	{
	    // When dragging the mouse, while the text has been scrolled up as
	    // far as it goes, moving the mouse in the top line should scroll
	    // the text down (done later when recomputing w_topline).
	    எனில் (mouse_dragging > 0
		    && curwin->w_cursor.lnum
				       == curwin->w_buffer->b_ml.ml_வரி_மொத்தம்
		    && curwin->w_cursor.lnum == curwin->w_topline)
		curwin->w_valid &= ~(VALID_TOPLINE);
	}
    }

    எனில் (prev_row >= W_WINROW(curwin)
	&& prev_row < W_WINROW(curwin) + curwin->w_height
	&& prev_col >= curwin->w_wincol && prev_col < W_ENDCOL(curwin)
						       && Screen_வரிகள் != வெற்று)
    {
	முழு off = வரி_Offset[prev_row] + prev_col;

	// Only use ScreenCols[] after the window was redrawn.  Mainly matters
	// for tests, a user would not click before redrawing.
	// Do not use when 'virtualedit' is active.
	எனில் (curwin->w_redr_type <= UPD_VALID_NO_UPDATE)
	    col_from_screen = ScreenCols[off];
#ifdef FEAT_FOLDING
	// Remember the character under the mouse, it might be a '-' or '+' in
	// the fold column.
	mouse_char = enc_utf8 && Screen_வரிகள்_UC[off] != 0
				       ? Screen_வரிகள்_UC[off] : Screen_வரிகள்[off];
#endif
    }

#ifdef FEAT_FOLDING
    // Check for position outside of the fold column.
    எனில் (
# ifdef FEAT_RIGHTLEFT
	    curwin->w_p_rl ? col < curwin->w_width - curwin->w_p_fdc :
# endif
	    col >= curwin->w_p_fdc + (cmdwin_win != curwin ? 0 : 1)
       )
	mouse_char = ' ';
#endif

    // compute the position in the buffer line from the posn on the screen
    எனில் (mouse_comp_pos(curwin, &row, &col, &curwin->w_cursor.lnum, வெற்று))
	mouse_past_bottom = TRUE;

    // Start Visual mode before coladvance(), for when 'sel' != "old"
    எனில் ((flags & MOUSE_MAY_VIS) && !VIsual_active)
    {
	check_visual_highlight();
	VIsual = old_cursor;
	VIsual_active = TRUE;
	VIsual_reselect = TRUE;
	// if 'selectmode' contains "mouse", start Select mode
	may_start_select('o');
	setmouse();
	எனில் (p_smd && செய்தி_silent == 0)
	    redraw_cmdline = TRUE;	// show visual mode later
    }

    எனில் (col_from_screen >= 0)
    {
	// Use the virtual column from ScreenCols[], it is accurate also after
	// concealed characters.
	col = col_from_screen;
    }

    curwin->w_curswant = col;
    curwin->w_set_curswant = false;	// May still have been TRUE
    எனில் (coladvance(col) == FAIL)	// Mouse click beyond end of line
    {
	எனில் (inclusive != வெற்று)
	    *inclusive = TRUE;
	mouse_past_eol = TRUE;
    }
    இல்லையெனில் எனில் (inclusive != வெற்று)
	*inclusive = FALSE;

    மொத்தம் = IN_BUFFER;
    எனில் (curwin != old_curwin || curwin->w_cursor.lnum != old_cursor.lnum
	    || curwin->w_cursor.col != old_cursor.col)
	மொத்தம் |= CURSOR_MOVED;		// Cursor has moved

#ifdef FEAT_FOLDING
    எனில் (mouse_char == curwin->w_fill_chars.foldclosed)
	மொத்தம் |= MOUSE_FOLD_OPEN;
    இல்லையெனில் எனில் (mouse_char != ' ')
	மொத்தம் |= MOUSE_FOLD_CLOSE;
#endif

    திரும்பு மொத்தம்;
}

/*
 * Make a horizontal scroll to "leftcol".
 * Return TRUE if the cursor moved, FALSE otherwise.
 */
    முழு
do_mousescroll_horiz(long_u leftcol)
{
    எனில் (curwin->w_p_wrap)
	திரும்பு FALSE;  // no horizontal scrolling when wrapping

    எனில் (curwin->w_leftcol == (colnr_T)leftcol)
	திரும்பு FALSE;  // already there

    // When the line of the cursor is too short, move the cursor to the
    // longest visible line.
    எனில் (
#ifdef FEAT_GUI
	    (!gui.in_use || vim_strchr(p_go, GO_HORSCROLL) == வெற்று) &&
#endif
		    !virtual_active()
	    && (long)leftcol > scroll_வரி_len(curwin->w_cursor.lnum))
    {
	curwin->w_cursor.lnum = ui_find_longest_lnum();
	curwin->w_cursor.col = 0;
    }

    திரும்பு set_leftcol((colnr_T)leftcol);
}

/*
 * Normal and Visual modes implementation for scrolling in direction
 * "cap->arg", which is one of the MSCR_ values.
 */
    வெறுமை
nv_mousescroll(cmdarg_T *cap)
{
    win_T   *old_curwin = curwin;

#ifdef FEAT_TABPANEL
    எனில் (mouse_row >= 0 && mouse_col >= 0
	    && (cap->arg == MSCR_UP || cap->arg == MSCR_DOWN)
	    && mouse_on_tabpanel())
    {
	(வெறுமை)tabpanel_scroll(cap->arg == MSCR_UP ? 1 : -1,
		mouse_vert_step > 0 ? mouse_vert_step : 3);
	திரும்பு;
    }
#endif

    எனில் (mouse_row >= 0 && mouse_col >= 0)
    {
	// Find the window at the mouse pointer coordinates.
	// NOTE: Must restore "curwin" to "old_curwin" before returning!
	முழு row = mouse_row;
	முழு col = mouse_col;
	curwin = mouse_find_win(&row, &col, FIND_POPUP);
	எனில் (curwin == வெற்று)
	{
	    curwin = old_curwin;
	    திரும்பு;
	}

#ifdef FEAT_PROP_POPUP
	எனில் (WIN_IS_POPUP(curwin) && !curwin->w_has_scrollbar)
	{
	    // cannot scroll this popup window
	    curwin = old_curwin;
	    திரும்பு;
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
    வெறுமை
nv_mouse(cmdarg_T *cap)
{
    (வெறுமை)do_mouse(cap->oap, cap->cmdchar, BACKWARD, cap->count1, 0);
}

நிலையான முழு	held_button = MOUSE_RELEASE;

    வெறுமை
reset_held_button(வெறுமை)
{
    held_button = MOUSE_RELEASE;
}

/*
 * Check if typebuf 'tp' contains a terminal mouse code and returns the
 * modifiers found in typebuf in 'modifiers'.
 */
    முழு
check_termcode_mouse(
    char_u	*tp,
    முழு		*slen,
    char_u	*சாவி_name,
    char_u	*modifiers_start,
    முழு		idx,
    முழு		*modifiers)
{
    முழு		j;
    char_u	*p;
#எனில் !defined(UNIX) || defined(FEAT_MOUSE_XTERM) || defined(FEAT_GUI) \
    || defined(FEAT_MOUSE_GPM) || defined(FEAT_SYSMOUSE)
    char_u	bytes[6];
    முழு		இலக்கம்_bytes;
#endif
    முழு		mouse_code = 0;	    // init for GCC
    முழு		is_click, is_drag;
    முழு		is_release, release_is_ambiguous;
    முழு		wheel_code = 0;
    முழு		current_button;
    நிலையான முழு	orig_இலக்கம்_clicks = 1;
    நிலையான முழு	orig_mouse_code = 0x0;
#ifdef CHECK_DOUBLE_CLICK
    நிலையான முழு	orig_mouse_col = 0;
    நிலையான முழு	orig_mouse_row = 0;
    நிலையான கட்டமைப்பு timeval  orig_mouse_time = {0, 0};
    // time of previous mouse click
    கட்டமைப்பு timeval  mouse_time;		// time of current mouse click
    long	timediff;		// elapsed time in msec
#endif

    is_click = is_drag = is_release = release_is_ambiguous = FALSE;

#எனில் !defined(UNIX) || defined(FEAT_MOUSE_XTERM) || defined(FEAT_GUI) \
    || defined(FEAT_MOUSE_GPM) || defined(FEAT_SYSMOUSE)
    எனில் (சாவி_name[0] == KS_MOUSE
# ifdef FEAT_MOUSE_GPM
	    || சாவி_name[0] == KS_GPM_MOUSE
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
	க்கு (;;)
	{
	    // For the GUI and for MS-Windows two bytes each are used for row
	    // and column.  Allows for more than 223 columns.
# எனில் defined(FEAT_GUI) || defined(MSWIN)
	    எனில் (TRUE
#  எனில் defined(FEAT_GUI) && !defined(MSWIN)
		&& gui.in_use
#  endif
		)
	    {
		இலக்கம்_bytes = get_bytes_from_buf(tp + *slen, bytes, 5);
		எனில் (இலக்கம்_bytes == -1)	// not enough coordinates
		    திரும்பு -1;
		mouse_code = bytes[0];
		mouse_col = 128 * (bytes[1] - ' ' - 1)
		    + bytes[2] - ' ' - 1;
		mouse_row = 128 * (bytes[3] - ' ' - 1)
		    + bytes[4] - ' ' - 1;
	    }
	    இல்லையெனில்
# endif
	    {
		இலக்கம்_bytes = get_bytes_from_buf(tp + *slen, bytes, 3);
		எனில் (இலக்கம்_bytes == -1)	// not enough coordinates
		    திரும்பு -1;
		mouse_code = bytes[0];
		mouse_col = bytes[1] - ' ' - 1;
		mouse_row = bytes[2] - ' ' - 1;
	    }
	    *slen += இலக்கம்_bytes;

	    // If the following bytes is also a mouse code and it has the same
	    // code, dump this one and get the next.  This makes dragging a
	    // whole lot faster.
# ifdef FEAT_GUI
	    எனில் (gui.in_use)
		j = 3;
	    இல்லையெனில்
# endif
		j = get_termcode_len(idx);
	    எனில் (STRNCMP(tp, tp + *slen, (size_t)j) == 0
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
	    இல்லையெனில்
		நிறுத்து;
	}
    }

    எனில் (சாவி_name[0] == KS_URXVT_MOUSE
	    || சாவி_name[0] == KS_SGR_MOUSE
	    || சாவி_name[0] == KS_SGR_MOUSE_RELEASE)
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
	எனில் (p == வெற்று)
	    திரும்பு -1;

	mouse_code = getdigits(&p);
	எனில் (*p++ != ';')
	    திரும்பு -1;

	// when mouse reporting is SGR, add 32 to mouse code
	எனில் (சாவி_name[0] == KS_SGR_MOUSE
		|| சாவி_name[0] == KS_SGR_MOUSE_RELEASE)
	    mouse_code += 32;

	mouse_col = getdigits(&p) - 1;
	எனில் (*p++ != ';')
	    திரும்பு -1;

	mouse_row = getdigits(&p) - 1;

	// The modifiers were the mouse coordinates, not the modifier keys
	// (alt/shift/ctrl/meta) state.
	*modifiers = 0;
    }

    எனில் (சாவி_name[0] == KS_SGR_MOUSE
	    || சாவி_name[0] == KS_SGR_MOUSE_RELEASE)
    {
	எனில் (சாவி_name[0] == KS_SGR_MOUSE_RELEASE)
	{
	    is_release = TRUE;
	    // This is used below to set held_button.
	    mouse_code |= MOUSE_RELEASE;
	}
    }
    இல்லையெனில்
    {
	release_is_ambiguous = TRUE;
	எனில் ((mouse_code & MOUSE_RELEASE) == MOUSE_RELEASE)
	    is_release = TRUE;
    }

    எனில் (சாவி_name[0] == KS_MOUSE
# ifdef FEAT_MOUSE_GPM
	    || சாவி_name[0] == KS_GPM_MOUSE
# endif
# ifdef FEAT_MOUSE_URXVT
	    || சாவி_name[0] == KS_URXVT_MOUSE
# endif
	    || சாவி_name[0] == KS_SGR_MOUSE
	    || சாவி_name[0] == KS_SGR_MOUSE_RELEASE)
    {
# எனில் !defined(MSWIN)
	/*
	 * Handle old style mouse events.
	 * Recognize the xterm mouse wheel, but not in the GUI, the
	 * Linux console with GPM and the MS-DOS or Win32 console
	 * (multi-clicks use >= 0x60).
	 */
	எனில் (mouse_code >= MOUSEWHEEL_கீழ் && mouse_code < MOUSESIDEBUTTONS_கீழ்
#  ifdef FEAT_GUI
		&& !gui.in_use
#  endif
#  ifdef FEAT_MOUSE_GPM
		&& சாவி_name[0] != KS_GPM_MOUSE
#  endif
	   )
	{
#  எனில் defined(UNIX)
	    எனில் (use_xterm_mouse() > 1 && mouse_code >= 0x80)
		// mouse-move event, using MOUSE_DRAG works
		mouse_code = MOUSE_DRAG;
	    இல்லையெனில்
#  endif
		// Keep the mouse_code before it's changed, so that we
		// remember that it was a mouse wheel click.
		wheel_code = mouse_code;
	}
#  ifdef FEAT_MOUSE_XTERM
	இல்லையெனில் எனில் (held_button == MOUSE_RELEASE
#   ifdef FEAT_GUI
		&& !gui.in_use
#   endif
		&& (mouse_code == 0x23 || mouse_code == 0x24
		    || mouse_code == 0x40 || mouse_code == 0x41))
	{
	    // Apparently 0x23 and 0x24 are used by rxvt scroll wheel.
	    // And 0x40 and 0x41 are used by some xterm emulator.
	    wheel_code = mouse_code - (mouse_code >= 0x40 ? 0x40 : 0x23)
							      + MOUSEWHEEL_கீழ்;
	}
#  endif

#  எனில் defined(UNIX)
	இல்லையெனில் எனில் (use_xterm_mouse() > 1)
	{
	    எனில் (mouse_code & MOUSE_DRAG_XTERM)
		mouse_code |= MOUSE_DRAG;
	}
#  endif
#  ifdef FEAT_XCLIPBOARD
	இல்லையெனில் எனில் (!(mouse_code & MOUSE_DRAG & ~MOUSE_CLICK_MASK))
	{
	    எனில் (is_release)
		stop_xterm_trace();
	    இல்லையெனில்
		start_xterm_trace(mouse_code);
	}
#  endif
# endif
    }
#endif // !UNIX || FEAT_MOUSE_XTERM
#ifdef FEAT_MOUSE_NET
    எனில் (சாவி_name[0] == KS_NETTERM_MOUSE)
    {
	முழு mc, mr;

	// expect a rather limited sequence like: balancing {
	// \033}6,45\r
	// '6' is the row, 45 is the column
	p = tp + *slen;
	mr = getdigits(&p);
	எனில் (*p++ != ',')
	    திரும்பு -1;
	mc = getdigits(&p);
	எனில் (*p++ != '\r')
	    திரும்பு -1;

	mouse_col = mc - 1;
	mouse_row = mr - 1;
	mouse_code = MOUSE_இடது;
	*slen += (முழு)(p - (tp + *slen));
    }
#endif	// FEAT_MOUSE_NET
#ifdef FEAT_MOUSE_JSB
    எனில் (சாவி_name[0] == KS_JSBTERM_MOUSE)
    {
	முழு mult, val, iter, button, status;

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
	தேர்வு (*p++)
	{
	    நிலை 'L': button = 1; நிறுத்து;
	    நிலை '-': நிறுத்து;
	    நிலை 'x': நிறுத்து; // ignore sequence
	    இயல்பு:  திரும்பு -1; // Unknown Result
	}
	தேர்வு (*p++)
	{
	    நிலை 'M': button |= 2; நிறுத்து;
	    நிலை '-': நிறுத்து;
	    நிலை 'x': நிறுத்து; // ignore sequence
	    இயல்பு:  திரும்பு -1; // Unknown Result
	}
	தேர்வு (*p++)
	{
	    நிலை 'R': button |= 4; நிறுத்து;
	    நிலை '-': நிறுத்து;
	    நிலை 'x': நிறுத்து; // ignore sequence
	    இயல்பு:  திரும்பு -1; // Unknown Result
	}
	status = *p++;
	க்கு (val = 0, mult = 100, iter = 0; iter < 3; iter++,
		mult /= 10, p++)
	    எனில் (*p >= '0' && *p <= '9')
		val += (*p - '0') * mult;
	    இல்லையெனில்
		திரும்பு -1;
	mouse_col = val;
	க்கு (val = 0, mult = 100, iter = 0; iter < 3; iter++,
		mult /= 10, p++)
	    எனில் (*p >= '0' && *p <= '9')
		val += (*p - '0') * mult;
	    இல்லையெனில்
		திரும்பு -1;
	mouse_row = val;
	தேர்வு (*p++)
	{
	    நிலை 's': button |= 8; நிறுத்து;  // SHIFT key Pressed
	    நிலை '-': நிறுத்து;  // Not Pressed
	    நிலை 'x': நிறுத்து;  // Not Reporting
	    இயல்பு:  திரும்பு -1; // Unknown Result
	}
	தேர்வு (*p++)
	{
	    நிலை 'c': button |= 16; நிறுத்து;  // CTRL key Pressed
	    நிலை '-': நிறுத்து;  // Not Pressed
	    நிலை 'x': நிறுத்து;  // Not Reporting
	    இயல்பு:  திரும்பு -1; // Unknown Result
	}
	எனில் (*p++ != '\033')
	    திரும்பு -1;
	எனில் (*p++ != '\\')
	    திரும்பு -1;
	தேர்வு (status)
	{
	    நிலை 'D': // Double Click
	    நிலை 'S': // Single Click
		எனில் (button & 1) mouse_code |= MOUSE_இடது;
		எனில் (button & 2) mouse_code |= MOUSE_நடு;
		எனில் (button & 4) mouse_code |= MOUSE_வலது;
		எனில் (button & 8) mouse_code |= MOUSE_SHIFT;
		எனில் (button & 16) mouse_code |= MOUSE_CTRL;
		நிறுத்து;
	    நிலை 'm': // Mouse move
		எனில் (button & 1) mouse_code |= MOUSE_இடது;
		எனில் (button & 2) mouse_code |= MOUSE_நடு;
		எனில் (button & 4) mouse_code |= MOUSE_வலது;
		எனில் (button & 8) mouse_code |= MOUSE_SHIFT;
		எனில் (button & 16) mouse_code |= MOUSE_CTRL;
		எனில் ((button & 7) != 0)
		{
		    held_button = mouse_code;
		    mouse_code |= MOUSE_DRAG;
		}
		is_drag = TRUE;
		showmode();
		நிறுத்து;
	    நிலை 'd': // Button Down
		எனில் (button & 1) mouse_code |= MOUSE_இடது;
		எனில் (button & 2) mouse_code |= MOUSE_நடு;
		எனில் (button & 4) mouse_code |= MOUSE_வலது;
		எனில் (button & 8) mouse_code |= MOUSE_SHIFT;
		எனில் (button & 16) mouse_code |= MOUSE_CTRL;
		நிறுத்து;
	    நிலை 'u': // Button Up
		is_release = TRUE;
		எனில் (button & 1)
		    mouse_code |= MOUSE_இடது;
		எனில் (button & 2)
		    mouse_code |= MOUSE_நடு;
		எனில் (button & 4)
		    mouse_code |= MOUSE_வலது;
		எனில் (button & 8)
		    mouse_code |= MOUSE_SHIFT;
		எனில் (button & 16)
		    mouse_code |= MOUSE_CTRL;
		நிறுத்து;
	    இயல்பு: திரும்பு -1; // Unknown Result
	}

	*slen += (p - (tp + *slen));
    }
#endif // FEAT_MOUSE_JSB
#ifdef FEAT_MOUSE_DEC
    எனில் (சாவி_name[0] == KS_DEC_MOUSE)
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
	முழு Pe, Pb, Pr, Pc;

	p = tp + *slen;

	// get event status
	Pe = getdigits(&p);
	எனில் (*p++ != ';')
	    திரும்பு -1;

	// get button status
	Pb = getdigits(&p);
	எனில் (*p++ != ';')
	    திரும்பு -1;

	// get row status
	Pr = getdigits(&p);
	எனில் (*p++ != ';')
	    திரும்பு -1;

	// get column status
	Pc = getdigits(&p);

	// the page parameter is optional
	எனில் (*p == ';')
	{
	    p++;
	    (வெறுமை)getdigits(&p);
	}
	எனில் (*p++ != '&')
	    திரும்பு -1;
	எனில் (*p++ != 'w')
	    திரும்பு -1;

	mouse_code = 0;
	தேர்வு (Pe)
	{
	    நிலை  0: திரும்பு -1; // position request while unavailable
	    நிலை  1: // a response to a locator position request includes
		     //	the status of all buttons
		     Pb &= 7;   // mask off and ignore fourth button
		     எனில் (Pb & 4)
			 mouse_code  = MOUSE_இடது;
		     எனில் (Pb & 2)
			 mouse_code  = MOUSE_நடு;
		     எனில் (Pb & 1)
			 mouse_code  = MOUSE_வலது;
		     எனில் (Pb)
		     {
			 held_button = mouse_code;
			 mouse_code |= MOUSE_DRAG;
			 WantQueryMouse = TRUE;
		     }
		     is_drag = TRUE;
		     showmode();
		     நிறுத்து;
	    நிலை  2: mouse_code = MOUSE_இடது;
		     WantQueryMouse = TRUE;
		     நிறுத்து;
	    நிலை  3: mouse_code = MOUSE_இடது;
		     is_release = TRUE;
		     நிறுத்து;
	    நிலை  4: mouse_code = MOUSE_நடு;
		     WantQueryMouse = TRUE;
		     நிறுத்து;
	    நிலை  5: mouse_code = MOUSE_நடு;
		     is_release = TRUE;
		     நிறுத்து;
	    நிலை  6: mouse_code = MOUSE_வலது;
		     WantQueryMouse = TRUE;
		     நிறுத்து;
	    நிலை  7: mouse_code = MOUSE_வலது;
		     is_release = TRUE;
		     நிறுத்து;
	    நிலை  8: திரும்பு -1; // fourth button down
	    நிலை  9: திரும்பு -1; // fourth button up
	    நிலை 10: திரும்பு -1; // mouse outside of filter rectangle
	    இயல்பு: திரும்பு -1; // should never occur
	}

	mouse_col = Pc - 1;
	mouse_row = Pr - 1;

	*slen += (முழு)(p - (tp + *slen));
    }
#endif // FEAT_MOUSE_DEC
#ifdef FEAT_MOUSE_PTERM
    எனில் (சாவி_name[0] == KS_PTERM_MOUSE)
    {
	முழு button, இலக்கம்_clicks, action;

	p = tp + *slen;

	action = getdigits(&p);
	எனில் (*p++ != ';')
	    திரும்பு -1;

	mouse_row = getdigits(&p);
	எனில் (*p++ != ';')
	    திரும்பு -1;
	mouse_col = getdigits(&p);
	எனில் (*p++ != ';')
	    திரும்பு -1;

	button = getdigits(&p);
	mouse_code = 0;

	தேர்வு (button)
	{
	    நிலை 4: mouse_code = MOUSE_இடது; நிறுத்து;
	    நிலை 1: mouse_code = MOUSE_வலது; நிறுத்து;
	    நிலை 2: mouse_code = MOUSE_நடு; நிறுத்து;
	    இயல்பு: திரும்பு -1;
	}

	தேர்வு (action)
	{
	    நிலை 31: // Initial press
		எனில் (*p++ != ';')
		    திரும்பு -1;

		இலக்கம்_clicks = getdigits(&p); // Not used
		நிறுத்து;

	    நிலை 32: // Release
		is_release = TRUE;
		நிறுத்து;

	    நிலை 33: // Drag
		held_button = mouse_code;
		mouse_code |= MOUSE_DRAG;
		நிறுத்து;

	    இயல்பு:
		திரும்பு -1;
	}

	எனில் (*p++ != 't')
	    திரும்பு -1;

	*slen += (p - (tp + *slen));
    }
#endif // FEAT_MOUSE_PTERM

    // Interpret the mouse code
    current_button = (mouse_code & MOUSE_CLICK_MASK);
    எனில் (is_release)
	current_button |= MOUSE_RELEASE;

    எனில் (current_button == MOUSE_RELEASE
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
	எனில் ((mouse_code & MOUSE_DRAG) == MOUSE_DRAG)
	    is_drag = TRUE;
	current_button = held_button;
    }
    இல்லையெனில்
    {
      எனில் (wheel_code == 0)
      {
#ifdef CHECK_DOUBLE_CLICK
# ifdef FEAT_MOUSE_GPM
	/*
	 * Only for Unix, when GUI not active, we handle multi-clicks here, but
	 * not for GPM mouse events.
	 */
#  ifdef FEAT_GUI
	எனில் (சாவி_name[0] != KS_GPM_MOUSE && !gui.in_use)
#  இல்லையெனில்
	    எனில் (சாவி_name[0] != KS_GPM_MOUSE)
#  endif
# இல்லையெனில்
#  ifdef FEAT_GUI
		எனில் (!gui.in_use)
#  endif
# endif
		{
		    /*
		     * Compute the time elapsed since the previous mouse click.
		     */
		    gettimeofday(&mouse_time, வெற்று);
		    எனில் (orig_mouse_time.tv_sec == 0)
		    {
			/*
			 * Avoid computing the difference between mouse_time
			 * and orig_mouse_time for the first click, as the
			 * difference would be huge and would cause
			 * multiplication overflow.
			 */
			timediff = p_mouset;
		    }
		    இல்லையெனில்
			timediff = time_diff_ms(&orig_mouse_time, &mouse_time);
		    orig_mouse_time = mouse_time;
		    எனில் (mouse_code == orig_mouse_code
			    && timediff < p_mouset
			    && orig_இலக்கம்_clicks != 4
			    && orig_mouse_col == mouse_col
			    && orig_mouse_row == mouse_row
			    && (is_mouse_topline(curwin)
				// Double click in tab pages line also works
				// when window contents changes.
				|| (mouse_row == 0 && firstwin->w_winrow > 0))
		       )
			++orig_இலக்கம்_clicks;
		    இல்லையெனில்
			orig_இலக்கம்_clicks = 1;
		    orig_mouse_col = mouse_col;
		    orig_mouse_row = mouse_row;
		    set_mouse_topline(curwin);
		}
# எனில் defined(FEAT_GUI) || defined(FEAT_MOUSE_GPM)
		இல்லையெனில்
		    orig_இலக்கம்_clicks = இலக்கம்_MOUSE_CLICKS(mouse_code);
# endif
#இல்லையெனில்
	orig_இலக்கம்_clicks = இலக்கம்_MOUSE_CLICKS(mouse_code);
#endif
	is_click = TRUE;
      }
      orig_mouse_code = mouse_code;
    }
    எனில் (!is_drag)
	held_button = mouse_code & MOUSE_CLICK_MASK;

    /*
     * Translate the actual mouse event into a pseudo mouse event.
     * First work out what modifiers are to be used.
     */
    எனில் (orig_mouse_code & MOUSE_SHIFT)
	*modifiers |= MOD_MASK_SHIFT;
    எனில் (orig_mouse_code & MOUSE_CTRL)
	*modifiers |= MOD_MASK_CTRL;
    எனில் (orig_mouse_code & MOUSE_ALT)
	*modifiers |= MOD_MASK_ALT;
    எனில் (orig_இலக்கம்_clicks == 2)
	*modifiers |= MOD_MASK_2CLICK;
    இல்லையெனில் எனில் (orig_இலக்கம்_clicks == 3)
	*modifiers |= MOD_MASK_3CLICK;
    இல்லையெனில் எனில் (orig_இலக்கம்_clicks == 4)
	*modifiers |= MOD_MASK_4CLICK;

    // Work out our pseudo mouse event. Note that MOUSE_RELEASE gets added,
    // then it's not mouse up/down.
    சாவி_name[0] = KS_EXTRA;
    எனில் (wheel_code != 0 && (!is_release || release_is_ambiguous))
    {
	எனில் (wheel_code & MOUSE_CTRL)
	    *modifiers |= MOD_MASK_CTRL;
	எனில் (wheel_code & MOUSE_ALT)
	    *modifiers |= MOD_MASK_ALT;

	எனில் (wheel_code & 1 && wheel_code & 2)
	    சாவி_name[1] = (முழு)KE_MOUSELEFT;
	இல்லையெனில் எனில் (wheel_code & 2)
	    சாவி_name[1] = (முழு)KE_MOUSERIGHT;
	இல்லையெனில் எனில் (wheel_code & 1)
	    சாவி_name[1] = (முழு)KE_MOUSEUP;
	இல்லையெனில்
	    சாவி_name[1] = (முழு)KE_MOUSEDOWN;

	held_button = MOUSE_RELEASE;
    }
    இல்லையெனில்
    {
#எனில் defined(UNIX)
	எனில் (use_xterm_mouse() && orig_mouse_code >= MOUSESIDEBUTTONS_கீழ்)
	    current_button = (current_button) ? MOUSE_X2 : MOUSE_X1;
#endif
	சாவி_name[1] = get_pseudo_mouse_code(current_button, is_click, is_drag);
    }


    // Make sure the mouse position is valid.  Some terminals may return weird
    // values.
    எனில் (mouse_col >= Columns)
	mouse_col = Columns - 1;
    எனில் (mouse_row >= Rows)
	mouse_row = Rows - 1;

    திரும்பு 0;
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
    முழு
mouse_comp_pos(
    win_T	*win,
    முழு		*rowp,
    முழு		*colp,
    linenr_T	*lnump,
    முழு		*plines_cache)
{
    முழு		col = *colp;
    முழு		row = *rowp;
    linenr_T	lnum;
    முழு		retval = FALSE;
    முழு		off;
    முழு		மொத்தம்;

#ifdef FEAT_RIGHTLEFT
    எனில் (win->w_p_rl)
	col = win->w_width - 1 - col;
#endif

    lnum = win->w_topline;

    போது (row > 0)
    {
	முழு cache_idx = lnum - win->w_topline;

	// Only "Rows" lines are cached, with folding we'll run out of entries
	// and use the slow way.
	எனில் (plines_cache != வெற்று && cache_idx < Rows
						&& plines_cache[cache_idx] > 0)
	    மொத்தம் = plines_cache[cache_idx];
	இல்லையெனில்
	{
#ifdef FEAT_DIFF
	    // Don't include filler lines in "count"
	    எனில் (win->w_p_diff
# ifdef FEAT_FOLDING
		    && !hasFoldingWin(win, lnum, வெற்று, வெற்று, TRUE, வெற்று)
# endif
		    )
	    {
		எனில் (lnum == win->w_topline)
		    row -= win->w_topfill;
		இல்லையெனில்
		    row -= diff_check_fill(win, lnum);
		மொத்தம் = plines_win_nofill(win, lnum, FALSE);
	    }
	    இல்லையெனில்
#endif
		மொத்தம் = plines_win(win, lnum, FALSE);
	    எனில் (plines_cache != வெற்று && cache_idx < Rows)
		plines_cache[cache_idx] = மொத்தம்;
	}

	எனில் (win->w_skipcol > 0 && lnum == win->w_topline)
	{
	    முழு width1 = win->w_width - win_col_off(win);

	    எனில் (width1 > 0)
	    {
		முழு skip_வரிகள் = 0;

		// Adjust for 'smoothscroll' clipping the top screen lines.
		// A similar formula is used in curs_columns().
		எனில் (win->w_skipcol > width1)
		    skip_வரிகள் = (win->w_skipcol - width1)
					    / (width1 + win_col_off2(win)) + 1;
		இல்லையெனில் எனில் (win->w_skipcol > 0)
		    skip_வரிகள் = 1;

		மொத்தம் -= skip_வரிகள்;
	    }
	}

	எனில் (மொத்தம் > row)
	    நிறுத்து;	// Position is in this buffer line.
#ifdef FEAT_FOLDING
	(வெறுமை)hasFoldingWin(win, lnum, வெற்று, &lnum, TRUE, வெற்று);
#endif
	எனில் (lnum == win->w_buffer->b_ml.ml_வரி_மொத்தம்)
	{
	    retval = TRUE;
	    நிறுத்து;		// past end of file
	}
	row -= மொத்தம்;
	++lnum;
    }

    எனில் (!retval)
    {
	// Compute the column without wrapping.
	off = win_col_off(win) - win_col_off2(win);
	எனில் (col < off)
	    col = off;
	col += row * (win->w_width - off);

	// Add skip column for the topline.
	எனில் (lnum == win->w_topline)
	    col += win->w_skipcol;
    }

    எனில் (!win->w_p_wrap)
	col += win->w_leftcol;

    // skip line number and fold column in front of the line
    col -= win_col_off(win);
    எனில் (col <= 0)
    {
#ifdef FEAT_NETBEANS_INTG
	// if mouse is clicked on the gutter, then inform the netbeans server
	எனில் (*colp < win_col_off(win))
	    netbeans_gutter_click(lnum);
#endif
	col = 0;
    }

    *colp = col;
    *rowp = row;
    *lnump = lnum;
    திரும்பு retval;
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
mouse_find_win(முழு *rowp, முழு *colp, mouse_find_T popup UNUSED)
{
    frame_T	*fp;
    win_T	*wp;

#ifdef FEAT_PROP_POPUP
    win_T	*pwp = வெற்று;

    எனில் (popup != IGNORE_POPUP)
    {
	popup_reset_handled(POPUP_HANDLED_1);
	போது ((wp = find_next_popup(TRUE, POPUP_HANDLED_1)) != வெற்று)
	{
	    எனில் (*rowp >= wp->w_winrow && *rowp < wp->w_winrow + popup_height(wp)
		    && *colp >= wp->w_wincol
				    && *colp < wp->w_wincol + popup_width(wp))
		pwp = wp;
	}
	எனில் (pwp != வெற்று)
	{
	    எனில் (popup == FAIL_POPUP)
		திரும்பு வெற்று;
	    *rowp -= pwp->w_winrow;
	    *colp -= pwp->w_wincol;
	    திரும்பு pwp;
	}
    }
#endif

    fp = topframe;

    எனில் (*colp < firstwin->w_wincol
	    || *colp >= firstwin->w_wincol + fp->fr_width
	    || *rowp < firstwin->w_winrow)
	திரும்பு வெற்று;

    *rowp -= firstwin->w_winrow;
    *colp -= firstwin->w_wincol;
    க்கு (;;)
    {
	எனில் (fp->fr_layout == FR_LEAF)
	    நிறுத்து;
	எனில் (fp->fr_layout == FR_ROW)
	{
	    க்கு (fp = fp->fr_child; fp->fr_next != வெற்று; fp = fp->fr_next)
	    {
		எனில் (*colp < fp->fr_width)
		    நிறுத்து;
		*colp -= fp->fr_width;
	    }
	}
	இல்லையெனில்    // fr_layout == FR_COL
	{
	    க்கு (fp = fp->fr_child; fp->fr_next != வெற்று; fp = fp->fr_next)
	    {
		எனில் (*rowp < fp->fr_height)
		    நிறுத்து;
		*rowp -= fp->fr_height;
	    }
	}
    }
    // When using a timer that closes a window the window might not actually
    // exist.
    FOR_ALL_WINDOWS(wp)
	எனில் (wp == fp->fr_win)
	{
#ifdef FEAT_MENU
	    *rowp -= wp->w_winbar_height;
#endif
	    திரும்பு wp;
	}
    திரும்பு வெற்று;
}

#எனில் defined(NEED_VCOL2COL) || defined(FEAT_BEVAL) || defined(FEAT_PROP_POPUP) \
	|| defined(FEAT_EVAL)
/*
 * Convert a virtual (screen) column to a character column.
 * The first column is zero.
 */
    முழு
vcol2col(win_T *wp, linenr_T lnum, முழு vcol, colnr_T *coladdp)
{
    char_u	    *வரி;
    chartabsize_T   cts;

    // try to advance to the specified column
    வரி = ml_get_buf(wp->w_buffer, lnum, FALSE);
    init_chartabsize_arg(&cts, wp, lnum, 0, வரி, வரி);
    போது (cts.cts_vcol < vcol && *cts.cts_ptr != NUL)
    {
	முழு size = win_lbr_chartabsize(&cts, வெற்று, வெற்று);
	எனில் (cts.cts_vcol + size > vcol)
	    நிறுத்து;
	cts.cts_vcol += size;
	MB_PTR_ADV(cts.cts_ptr);
    }
    clear_chartabsize_arg(&cts);

    எனில் (coladdp != வெற்று)
	*coladdp = vcol - cts.cts_vcol;
    திரும்பு (முழு)(cts.cts_ptr - வரி);
}
#endif

#எனில் defined(FEAT_EVAL)
/*
 * "getmousepos()" function.
 */
    வெறுமை
f_getmousepos(typval_T *argvars UNUSED, typval_T *rettv)
{
    dict_T	*d;
    win_T	*wp;
    முழு		row = mouse_row;
    முழு		col = mouse_col;
    varnumber_T winid = 0;
    varnumber_T winrow = 0;
    varnumber_T wincol = 0;
    linenr_T	lnum = 0;
    varnumber_T column = 0;
    colnr_T	coladd = 0;

    எனில் (rettv_dict_alloc(rettv) == FAIL)
	திரும்பு;
    d = rettv->vval.v_dict;

    dict_add_எண்(d, "screenrow", (varnumber_T)mouse_row + 1);
    dict_add_எண்(d, "screencol", (varnumber_T)mouse_col + 1);

    wp = mouse_find_win(&row, &col, FIND_POPUP);
    எனில் (wp != வெற்று)
    {
	முழு	உச்சி_off = 0;
	முழு	இடது_off = 0;
	முழு	height = wp->w_height + wp->w_status_height;

# ifdef FEAT_PROP_POPUP
	எனில் (WIN_IS_POPUP(wp))
	{
	    உச்சி_off = popup_உச்சி_extra(wp);
	    இடது_off = popup_இடது_extra(wp);
	    height = popup_height(wp);
	}
# endif
	எனில் (row < height)
	{
	    winid = wp->w_id;
	    winrow = row + 1;
	    wincol = col + 1;
	    row -= உச்சி_off;
	    col -= இடது_off;
	    எனில் (row >= 0 && row < wp->w_height && col >= 0 && col < wp->w_width)
	    {
		(வெறுமை)mouse_comp_pos(wp, &row, &col, &lnum, வெற்று);
		col = vcol2col(wp, lnum, col, &coladd);
		column = col + 1;
	    }
	}
    }
    dict_add_எண்(d, "winid", winid);
    dict_add_எண்(d, "winrow", winrow);
    dict_add_எண்(d, "wincol", wincol);
    dict_add_எண்(d, "line", (varnumber_T)lnum);
    dict_add_எண்(d, "column", column);
    dict_add_எண்(d, "coladd", coladd);
}
#endif
