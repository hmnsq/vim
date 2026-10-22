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

#অন্তর্ভাৱ "vim.h"

/*
 * Horiziontal and vertical steps used when scrolling.
 * When negative scroll by a whole page.
 */
স্থির long mouse_hor_step = 6;
স্থির long mouse_vert_step = 3;
স্থির win_T *dragwin = রিক্ত;	// window being dragged
স্থির পূর্ণাঙ্ক stl_click_handler(win_T *wp, পূর্ণাঙ্ক mrow, পূর্ণাঙ্ক mcol, পূর্ণাঙ্ক which_button,
								    পূর্ণাঙ্ক mods);
স্থির পূর্ণাঙ্ক stl_click_handler_regions(stl_click_region_T *regions,
				    পূর্ণাঙ্ক region_গুনতি, পূর্ণাঙ্ক winid,
				    char_u *area_name, পূর্ণাঙ্ক mrow, পূর্ণাঙ্ক mcol,
				    পূর্ণাঙ্ক which_button, পূর্ণাঙ্ক mods);

    শূন্য
mouse_set_vert_scroll_step(long step)
{
    mouse_vert_step = step;
}

    শূন্য
mouse_set_hor_scroll_step(long step)
{
    mouse_hor_step = step;
}

#ifdef CHECK_DOUBLE_CLICK
/*
 * Return the duration from t1 to t2 in milliseconds.
 */
    স্থির long
time_diff_ms(সংরচনা timeval *t1, সংরচনা timeval *t2)
{
    // This handles wrapping of tv_usec correctly without any special case.
    // Example of 2 pairs (tv_sec, tv_usec) with a duration of 5 ms:
    //	   t1 = (1, 998000) t2 = (2, 3000) gives:
    //	   (2 - 1) * 1000 + (3000 - 998000) / 1000 -> 5 ms.
    ঘূৰাই (t2->tv_sec - t1->tv_sec) * 1000
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
    স্থির পূর্ণাঙ্ক
get_mouse_class(char_u *p)
{
    পূর্ণাঙ্ক		c;

    যদি (has_mbyte && MB_BYTE2LEN(p[0]) > 1)
	ঘূৰাই mb_get_class(p);

    c = *p;
    যদি (c == ' ' || c == '\t')
	ঘূৰাই 0;

    যদি (vim_iswordc(c))
	ঘূৰাই 2;

    // There are a few special cases where we want certain combinations of
    // characters to be considered as a single word.  These are things like
    // "->", "/ *", "*=", "+=", "&=", "<=", ">=", "!=" etc.  Otherwise, each
    // character is in its own class.
    যদি (c != NUL && vim_strchr((char_u *)"-+*/%<>&|^!=", c) != রিক্ত)
	ঘূৰাই 1;
    ঘূৰাই c;
}

/*
 * Move "pos" back to the start of the word it's in.
 */
    স্থির শূন্য
find_start_of_word(pos_T *pos)
{
    char_u	*সাৰি;
    পূর্ণাঙ্ক		cclass;
    পূর্ণাঙ্ক		col;

    সাৰি = ml_get(pos->lnum);
    cclass = get_mouse_class(সাৰি + pos->col);

    যেতিয়ালৈ (pos->col > 0)
    {
	col = pos->col - 1;
	col -= (*mb_head_off)(সাৰি, সাৰি + col);
	যদি (get_mouse_class(সাৰি + col) != cclass)
	    ভঙ্গ;
	pos->col = col;
    }
}

/*
 * Move "pos" forward to the end of the word it's in.
 * When 'selection' is "exclusive", the position is just after the word.
 */
    স্থির শূন্য
find_end_of_word(pos_T *pos)
{
    char_u	*সাৰি;
    পূর্ণাঙ্ক		cclass;
    পূর্ণাঙ্ক		col;

    সাৰি = ml_get(pos->lnum);
    যদি (*p_sel == 'e' && pos->col > 0)
    {
	--pos->col;
	pos->col -= (*mb_head_off)(সাৰি, সাৰি + pos->col);
    }
    cclass = get_mouse_class(সাৰি + pos->col);
    যেতিয়ালৈ (সাৰি[pos->col] != NUL)
    {
	col = pos->col + (*mb_ptr2len)(সাৰি + pos->col);
	যদি (get_mouse_class(সাৰি + col) != cclass)
	{
	    যদি (*p_sel == 'e')
		pos->col = col;
	    ভঙ্গ;
	}
	pos->col = col;
    }
}

#যদি defined(FEAT_GUI_MOTIF) || defined(FEAT_GUI_GTK) \
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
    স্থির পূর্ণাঙ্ক
get_fpos_of_mouse(pos_T *mpos)
{
    win_T	*wp;
    পূর্ণাঙ্ক		row = mouse_row;
    পূর্ণাঙ্ক		col = mouse_col;

    যদি (row < 0 || col < 0)		// check if it makes sense
	ঘূৰাই IN_UNKNOWN;

    // find the window where the row is in
    wp = mouse_find_win(&row, &col, FAIL_POPUP);
    যদি (wp == রিক্ত)
	ঘূৰাই IN_UNKNOWN;
    // winpos and height may change in win_enter()!
    যদি (row >= wp->w_height)	// In (or below) status line
	ঘূৰাই IN_STATUS_সাৰি;
    যদি (col >= wp->w_width)	// In vertical separator line
	ঘূৰাই IN_SEP_সাৰি;

    যদি (wp != curwin)
	ঘূৰাই IN_UNKNOWN;

    // compute the position in the buffer line from the posn on the screen
    যদি (mouse_comp_pos(curwin, &row, &col, &mpos->lnum, রিক্ত))
	ঘূৰাই IN_STATUS_সাৰি; // past bottom

    mpos->col = vcol2col(wp, mpos->lnum, col, &mpos->coladd);
    ঘূৰাই IN_BUFFER;
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
    পূর্ণাঙ্ক
do_mouse(
    oparg_T	*oap,		// operator argument, can be NULL
    পূর্ণাঙ্ক		c,		// K_LEFTMOUSE, etc
    পূর্ণাঙ্ক		dir,		// Direction to 'put' if necessary
    long	গুনতি,
    পূর্ণাঙ্ক		fixindent)	// PUT_FIXINDENT if fixing indent necessary
{
    স্থির পূর্ণাঙ্ক	do_always = FALSE;	// ignore 'mouse' setting next time
    স্থির পূর্ণাঙ্ক	got_click = FALSE;	// got a click some time back

    পূর্ণাঙ্ক		which_button;	// MOUSE_LEFT, _MIDDLE or _RIGHT
    পূর্ণাঙ্ক		is_click = FALSE; // If FALSE it's a drag or release event
    পূর্ণাঙ্ক		is_drag = FALSE;  // If TRUE it's a drag event
    পূর্ণাঙ্ক		jump_flags = 0;	// flags for jump_to_mouse()
    pos_T	start_visual;
    পূর্ণাঙ্ক		moved;		// Has cursor moved?
    পূর্ণাঙ্ক		in_status_সাৰি;	// mouse in status line
    স্থির পূর্ণাঙ্ক	in_tab_সাৰি = FALSE; // mouse clicked in tab line
    স্থির পূর্ণাঙ্ক	in_tabpanel = FALSE; // mouse clicked in tabpanel
#ifdef FEAT_TABPANEL
    স্থির bool	in_tabpanel_scrollbar = false; // dragging tabpanel scrollbar
#endif
    পূর্ণাঙ্ক		in_sep_সাৰি;	// mouse in vertical separator line
    পূর্ণাঙ্ক		c1, c2;
#যদি defined(FEAT_FOLDING)
    pos_T	save_cursor;
#endif
    win_T	*old_curwin = curwin;
    স্থির pos_T orig_cursor;
    colnr_T	leftcol, rightcol;
    pos_T	end_visual;
    পূর্ণাঙ্ক		diff;
    পূর্ণাঙ্ক		old_active = VIsual_active;
    পূর্ণাঙ্ক		old_mode = VIsual_mode;
    পূর্ণাঙ্ক		regname;

#যদি defined(FEAT_FOLDING)
    save_cursor = curwin->w_cursor;
#endif

    // When GUI is active, always recognize mouse events, otherwise:
    // - Ignore mouse event in normal mode if 'mouse' doesn't include 'n'.
    // - Ignore mouse event in visual mode if 'mouse' doesn't include 'v'.
    // - For command line and insert mode 'mouse' is checked before calling
    //	 do_mouse().
    যদি (do_always)
	do_always = FALSE;
    অন্যথা
#ifdef FEAT_GUI
	যদি (!gui.in_use)
#endif
	{
	    যদি (VIsual_active)
	    {
		যদি (!mouse_has(MOUSE_VISUAL))
		    ঘূৰাই FALSE;
	    }
	    অন্যথা যদি (State == MODE_NORMAL && !mouse_has(MOUSE_NORMAL))
		ঘূৰাই FALSE;
	}

    বাবে (;;)
    {
	which_button = get_mouse_button(KEY2TERMCAP1(c), &is_click, &is_drag);
	যদি (is_drag)
	{
	    // If the next character is the same mouse event then use that
	    // one. Speeds up dragging the status line.
	    // Note: Since characters added to the stuff buffer in the code
	    // below need to come before the next character, do not do this
	    // when the current character was stuffed.
	    যদি (!চাবি_Stuffed && vpeekc() != NUL)
	    {
		পূর্ণাঙ্ক nc;
		পূর্ণাঙ্ক save_mouse_row = mouse_row;
		পূর্ণাঙ্ক save_mouse_col = mouse_col;

		// Need to get the character, peeking doesn't get the actual
		// one.
		nc = safe_vgetc();
		যদি (c == nc)
		    অনুৱর্ত;
		vungetc(nc);
		mouse_row = save_mouse_row;
		mouse_col = save_mouse_col;
	    }
	}
	ভঙ্গ;
    }

    যদি (c == K_MOUSEMOVE)
    {
	// Mouse moved without a button pressed.
#ifdef FEAT_BEVAL_TERM
	ui_may_remove_balloon();
	যদি (p_bevalterm)
	{
	    profile_setlimit(p_bdlay, &bevalexpr_due);
	    bevalexpr_due_set = TRUE;
	}
#endif
#ifdef FEAT_PROP_POPUP
	popup_handle_mouse_moved();
#endif
	ঘূৰাই FALSE;
    }

#ifdef FEAT_MOUSESHAPE
    // May have stopped dragging the status or separator line.  The pointer is
    // most likely still on the status or separator line.
    যদি (!is_drag && drag_status_সাৰি)
    {
	drag_status_সাৰি = FALSE;
	update_mouseshape(SHAPE_IDX_STATUS);
    }
    যদি (!is_drag && drag_sep_সাৰি)
    {
	drag_sep_সাৰি = FALSE;
	update_mouseshape(SHAPE_IDX_VSEP);
    }
#endif

    // Ignore drag and release events if we didn't get a click.
    যদি (is_click)
    {
	got_click = TRUE;
	in_tab_সাৰি = FALSE;
	in_tabpanel = FALSE;
#ifdef FEAT_TABPANEL
	in_tabpanel_scrollbar = false;
#endif
    }
    অন্যথা
    {
	যদি (!got_click)			// didn't get click, ignore
	    ঘূৰাই FALSE;
	যদি (!is_drag)			// release, reset got_click
	{
	    got_click = FALSE;
	    যদি (in_tab_সাৰি || in_tabpanel
#ifdef FEAT_TABPANEL
		    || in_tabpanel_scrollbar
#endif
		    )
	    {
		in_tab_সাৰি = FALSE;
		in_tabpanel = FALSE;
#ifdef FEAT_TABPANEL
		in_tabpanel_scrollbar = false;
#endif
		ঘূৰাই FALSE;
	    }
	}
    }

#ifdef FEAT_TABPANEL
    // Continue a scrollbar drag before any tab-selection handling.
    যদি (is_drag && in_tabpanel_scrollbar)
    {
	tabpanel_drag_scrollbar(mouse_row);
	ঘূৰাই FALSE;
    }
#endif

    // CTRL right mouse button does CTRL-T
    যদি (is_click && (mod_mask & MOD_MASK_CTRL) && which_button == MOUSE_ডান)
    {
	যদি (State & MODE_ঢোকাও)
	    stuffcharReadbuff(Ctrl_O);
	যদি (গুনতি > 1)
	    stuffnumReadbuff(গুনতি);
	stuffcharReadbuff(Ctrl_T);
	got_click = FALSE;		// ignore drag&release now
	ঘূৰাই FALSE;
    }

    // CTRL only works with left mouse button
    যদি ((mod_mask & MOD_MASK_CTRL) && which_button != MOUSE_বাম)
	ঘূৰাই FALSE;

    // When a modifier is down, ignore drag and release events, as well as
    // multiple clicks and the middle mouse button.
    // Accept shift-leftmouse drags when 'mousemodel' is "popup.*".
    যদি ((mod_mask & (MOD_MASK_SHIFT | MOD_MASK_CTRL | MOD_MASK_ALT
							     | MOD_MASK_META))
	    && (!is_click
		|| (mod_mask & MOD_MASK_MULTI_CLICK)
		|| which_button == MOUSE_মধ্য)
	    && !((mod_mask & (MOD_MASK_SHIFT|MOD_MASK_ALT))
		&& mouse_model_popup()
		&& which_button == MOUSE_বাম)
	    && !((mod_mask & MOD_MASK_ALT)
		&& !mouse_model_popup()
		&& which_button == MOUSE_ডান)
	    )
	ঘূৰাই FALSE;

    // If the button press was used as the movement command for an operator
    // (eg "d<MOUSE>"), or it is the middle button that is held down, ignore
    // drag/release events.
    যদি (!is_click && which_button == MOUSE_মধ্য)
	ঘূৰাই FALSE;

    যদি (oap != রিক্ত)
	regname = oap->regname;
    অন্যথা
	regname = 0;

    // Middle mouse button does a 'put' of the selected text
    যদি (which_button == MOUSE_মধ্য)
    {
	যদি (State == MODE_NORMAL)
	{
	    // If an operator was pending, we don't know what the user wanted
	    // to do. Go back to normal mode: Clear the operator and beep().
	    যদি (oap != রিক্ত && oap->op_type != OP_NOP)
	    {
		clearopbeep(oap);
		ঘূৰাই FALSE;
	    }

	    // If visual was active, yank the highlighted text and put it
	    // before the mouse pointer position.
	    // In Select mode replace the highlighted text with the clipboard.
	    যদি (VIsual_active)
	    {
		যদি (VIsual_select)
		{
		    stuffcharReadbuff(Ctrl_G);
		    stuffReadbuff((char_u *)"\"+p");
		}
		অন্যথা
		{
		    stuffcharReadbuff('y');
		    stuffcharReadbuff(K_MIDDLEMOUSE);
		}
		do_always = TRUE;	// ignore 'mouse' setting next time
		ঘূৰাই FALSE;
	    }
	    // The rest is below jump_to_mouse()
	}

	অন্যথা যদি ((State & MODE_ঢোকাও) == 0)
	    ঘূৰাই FALSE;

	// Middle click in insert mode doesn't move the mouse, just insert the
	// contents of a register.  '.' register is special, can't insert that
	// with do_put().
	// Also paste at the cursor if the current mode isn't in 'mouse' (only
	// happens for the GUI).
	যদি ((State & MODE_ঢোকাও) || !mouse_has(MOUSE_NORMAL))
	{
	    যদি (regname == '.')
		ঢোকাও_reg(regname, TRUE);
	    অন্যথা
	    {
#ifdef FEAT_CLIPBOARD
		যদি (clip_star.available && regname == 0)
		    regname = '*';
#endif
		যদি ((State & REPLACE_FLAG) && !yank_register_mline(regname))
		    ঢোকাও_reg(regname, TRUE);
		অন্যথা
		{
		    do_put(regname, রিক্ত, BACKWARD, 1L,
						      fixindent | PUT_CURSEND);

		    // Repeat it with CTRL-R CTRL-O r or CTRL-R CTRL-P r
		    AppendCharToRedobuff(Ctrl_R);
		    AppendCharToRedobuff(fixindent ? Ctrl_P : Ctrl_O);
		    AppendCharToRedobuff(regname == 0 ? '"' : regname);
		}
	    }
	    ঘূৰাই FALSE;
	}
    }

    // When dragging or button-up stay in the same window.
    যদি (!is_click)
	jump_flags |= MOUSE_FOCUS | MOUSE_DID_MOVE;

    start_visual.lnum = 0;

    সংরচনা tabpage_label_info {
	bool is_panel;	    // label type. true: tabpanel, false: tab line
	bool just_in;	    // just in tabpage label area
	bool just_click;    // just click tabpage label area
	পূর্ণাঙ্ক nr;		    // tabpage number
    } tp_label = { false, false, false, 0 };

    // Check for clicking in the tab page panel.
#যদি defined(FEAT_TABPANEL)
    যদি (mouse_col < firstwin->w_wincol
		|| mouse_col >= firstwin->w_wincol + topframe->fr_width)
    {
	// A click on the scrollbar column starts a drag interaction and
	// preempts tab-selection.
	যদি (is_click && !is_drag && mouse_on_tabpanel_scrollbar())
	{
	    in_tabpanel_scrollbar = TRUE;
	    tabpanel_drag_scrollbar(mouse_row);
	    ঘূৰাই FALSE;
	}

	// Dispatch 'tabpanel' %[FuncName] click regions before falling through
	// to tab-page selection.  On drag events fall through to the normal
	// tab-drag handling.
	যদি (is_click && !is_drag
		&& stl_click_handler_regions(tabpanel_stl_click,
					    tabpanel_stl_click_গুনতি,
					    0, (char_u *)"tabpanel",
					    mouse_row, mouse_col,
					    which_button, mod_mask))
	    ঘূৰাই FALSE;

	tp_label.is_panel = true;
	tp_label.just_in = true;
	tp_label.nr = get_tabpagenr_on_tabpanel();

	// click in a tab selects that tab page
	যদি (is_click && cmdwin_type == 0)
	    tp_label.just_click = true;
    }
    অন্যথা
#endif
    // Check for clicking in the tab page line.
    যদি (TabPageIdxs != রিক্ত && mouse_row == 0 && firstwin->w_winrow > 0)
    {
	// Dispatch 'tabline' %[FuncName] click regions before falling through
	// to tab-page selection.  On drag events fall through to the normal
	// tab-drag handling.
	যদি (is_click && !is_drag
		&& stl_click_handler_regions(tabline_stl_click,
					    tabline_stl_click_গুনতি,
					    0, (char_u *)"tabline",
					    mouse_row, mouse_col,
					    which_button, mod_mask))
	    ঘূৰাই FALSE;

	tp_label.just_in = true;
	tp_label.nr = TabPageIdxs[mouse_col];

	// click in a tab selects that tab page
	যদি (is_click && cmdwin_type == 0
		&& mouse_col < firstwin->w_wincol + topframe->fr_width)
	    tp_label.just_click = true;
    }

    যদি (tp_label.just_in)
    {
	যদি (is_drag)
	{
	    যদি (in_tabpanel || in_tab_সাৰি)
	    {
		c1 = tp_label.nr;
		tabpage_move(c1 <= 0 ? 9999 : c1 < tabpage_index(curtab)
							    ? c1 - 1 : c1);
	    }
	    ঘূৰাই FALSE;
	}

	যদি (tp_label.just_click)
	{
	    যদি (tp_label.is_panel)
		in_tabpanel = TRUE;
	    অন্যথা
		in_tab_সাৰি = TRUE;
	    c1 = tp_label.nr;
	    যদি (c1 >= 0)
	    {
		যদি ((mod_mask & MOD_MASK_MULTI_CLICK) == MOD_MASK_2CLICK
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
		অন্যথা
		{
		    // Go to specified tab page, or next one if not clicking
		    // on a label.
		    goto_tabpage(c1);

		    // It's like clicking on the status line of a window.
		    যদি (curwin != old_curwin)
			end_visual_mode_keep_button();
		}
	    }
	    অন্যথা
	    {
		tabpage_T	*tp;

		// Close the current or specified tab page.
		যদি (c1 == -999)
		    tp = curtab;
		অন্যথা
		    tp = find_tabpage(-c1);
		যদি (tp == curtab)
		{
		    যদি (first_tabpage->tp_next != রিক্ত)
			tabpage_close(FALSE);
		}
		অন্যথা যদি (tp != রিক্ত)
		    tabpage_close_other(tp, FALSE);
	    }
	}
	ঘূৰাই TRUE;
    }
    অন্যথা যদি (is_drag && (in_tabpanel || (in_tab_সাৰি && TabPageIdxs != রিক্ত)))
    {
#যদি defined(FEAT_TABPANEL)
	যদি (in_tabpanel)
	    c1 = get_tabpagenr_on_tabpanel();
	অন্যথা
#endif
	    c1 = TabPageIdxs[mouse_col];
	tabpage_move(c1 <= 0 ? 9999 : c1 - 1);
	ঘূৰাই FALSE;
    }

    // When 'mousemodel' is "popup" or "popup_setpos", translate mouse events:
    // right button up   -> pop-up menu
    // shift-left button -> right button
    // alt-left button   -> alt-right button
    যদি (mouse_model_popup())
    {
	যদি (which_button == MOUSE_ডান
			    && !(mod_mask & (MOD_MASK_SHIFT | MOD_MASK_CTRL)))
	{
#ifdef USE_POPUP_SETPOS
# ifdef FEAT_GUI
	    যদি (gui.in_use)
	    {
#  যদি defined(FEAT_GUI_MOTIF) || defined(FEAT_GUI_GTK) \
			  || defined(FEAT_GUI_PHOTON)
		যদি (!is_click)
		    // Ignore right button release events, only shows the popup
		    // menu on the button down event.
		    ঘূৰাই FALSE;
#  endif
#  যদি defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_HAIKU)
		যদি (is_click || is_drag)
		    // Ignore right button down and drag mouse events.  Windows
		    // only shows the popup menu on the button up event.
		    ঘূৰাই FALSE;
#  endif
	    }
# endif
# যদি defined(FEAT_GUI) && defined(FEAT_TERM_POPUP_MENU)
	    অন্যথা
# endif
# যদি defined(FEAT_TERM_POPUP_MENU)
	    যদি (!is_click)
		// Ignore right button release events, only shows the popup
		// menu on the button down event.
		ঘূৰাই FALSE;
# endif

	    jump_flags = 0;
	    যদি (STRCMP(p_mousem, "popup_setpos") == 0)
	    {
		// First set the cursor position before showing the popup
		// menu.
		যদি (VIsual_active)
		{
		    pos_T    m_pos;

		    // set MOUSE_MAY_STOP_VIS if we are outside the
		    // selection or the current window (might have false
		    // negative here)
		    যদি (mouse_row < curwin->w_winrow
			 || mouse_row
				  > (curwin->w_winrow + curwin->w_height))
			jump_flags = MOUSE_MAY_STOP_VIS;
		    অন্যথা যদি (get_fpos_of_mouse(&m_pos) != IN_BUFFER)
			jump_flags = MOUSE_MAY_STOP_VIS;
		    অন্যথা
		    {
			যদি (VIsual_mode == 'V')
			{
			    যদি ((curwin->w_cursor.lnum <= VIsual.lnum
				    && (m_pos.lnum < curwin->w_cursor.lnum
					|| VIsual.lnum < m_pos.lnum))
				|| (VIsual.lnum < curwin->w_cursor.lnum
				    && (m_pos.lnum < VIsual.lnum
					|| curwin->w_cursor.lnum < m_pos.lnum)))
			    {
				jump_flags = MOUSE_MAY_STOP_VIS;
			    }
			}
			অন্যথা যদি ((LTOREQ_POS(curwin->w_cursor, VIsual)
				    && (LT_POS(m_pos, curwin->w_cursor)
					|| LT_POS(VIsual, m_pos)))
				|| (LT_POS(VIsual, curwin->w_cursor)
				    && (LT_POS(m_pos, VIsual)
				      || LT_POS(curwin->w_cursor, m_pos))))
			{
			    jump_flags = MOUSE_MAY_STOP_VIS;
			}
			অন্যথা যদি (VIsual_mode == Ctrl_V)
			{
			    getvcols(curwin, &curwin->w_cursor, &VIsual,
						     &leftcol, &rightcol, 0);
			    getvcol(curwin, &m_pos, রিক্ত, &m_pos.col, রিক্ত, 0);
			    যদি (m_pos.col < leftcol || m_pos.col > rightcol)
				jump_flags = MOUSE_MAY_STOP_VIS;
			}
		    }
		}
		অন্যথা
		    jump_flags = MOUSE_MAY_STOP_VIS;
	    }
	    যদি (jump_flags)
	    {
		jump_flags = jump_to_mouse(jump_flags, রিক্ত, which_button);
		update_curbuf(VIsual_active ? UPD_INVERTED : UPD_VALID);
		setcursor();
		out_flush();    // Update before showing popup menu
	    }
# ifdef FEAT_MENU
	    show_popupmenu();
	    got_click = FALSE;	// ignore release events
# endif
	    ঘূৰাই (jump_flags & CURSOR_MOVED) != 0;
#অন্যথা
	    ঘূৰাই FALSE;
#endif
	}
	যদি (which_button == MOUSE_বাম
				&& (mod_mask & (MOD_MASK_SHIFT|MOD_MASK_ALT)))
	{
	    which_button = MOUSE_ডান;
	    mod_mask &= ~MOD_MASK_SHIFT;
	}
    }

    যদি ((State & (MODE_NORMAL | MODE_ঢোকাও))
			    && !(mod_mask & (MOD_MASK_SHIFT | MOD_MASK_CTRL)))
    {
	যদি (which_button == MOUSE_বাম)
	{
	    যদি (is_click)
	    {
		// stop Visual mode for a left click in a window, but not when
		// on a status line
		যদি (VIsual_active)
		    jump_flags |= MOUSE_MAY_STOP_VIS;
	    }
	    অন্যথা যদি (mouse_has(MOUSE_VISUAL))
		jump_flags |= MOUSE_MAY_VIS;
	}
	অন্যথা যদি (which_button == MOUSE_ডান)
	{
	    যদি (is_click && VIsual_active)
	    {
		// Remember the start and end of visual before moving the
		// cursor.
		যদি (LT_POS(curwin->w_cursor, VIsual))
		{
		    start_visual = curwin->w_cursor;
		    end_visual = VIsual;
		}
		অন্যথা
		{
		    start_visual = VIsual;
		    end_visual = curwin->w_cursor;
		}
	    }
	    jump_flags |= MOUSE_FOCUS;
	    যদি (mouse_has(MOUSE_VISUAL))
		jump_flags |= MOUSE_MAY_VIS;
	}
    }

    // If an operator is pending, ignore all drags and releases until the
    // next mouse click.
    যদি (!is_drag && oap != রিক্ত && oap->op_type != OP_NOP)
    {
	got_click = FALSE;
	oap->motion_type = MCHAR;
    }

    // When releasing the button let jump_to_mouse() know.
    যদি (!is_click && !is_drag)
	jump_flags |= MOUSE_RELEASED;

    // JUMP!
    jump_flags = jump_to_mouse(jump_flags,
			oap == রিক্ত ? রিক্ত : &(oap->inclusive), which_button);

#ifdef FEAT_MENU
    // A click in the window toolbar has no side effects.
    যদি (jump_flags & MOUSE_WINBAR)
	ঘূৰাই FALSE;
#endif
    moved = (jump_flags & CURSOR_MOVED);
    in_status_সাৰি = (jump_flags & IN_STATUS_সাৰি);
    in_sep_সাৰি = (jump_flags & IN_SEP_সাৰি);

    // Check for statusline click handler early, before visual mode or
    // other button-specific handling can interfere.
    যদি (in_status_সাৰি && is_click && !is_drag
	    && stl_click_handler(dragwin, mouse_row, mouse_col,
						which_button, mod_mask))
    {
#ifdef FEAT_MOUSESHAPE
	যদি (!drag_status_সাৰি)
	{
	    drag_status_সাৰি = TRUE;
	    update_mouseshape(-1);
	}
#endif
	ঘূৰাই FALSE;
    }

#ifdef FEAT_NETBEANS_INTG
    যদি (isNetbeansBuffer(curbuf)
			    && !(jump_flags & (IN_STATUS_সাৰি | IN_SEP_সাৰি)))
    {
	পূর্ণাঙ্ক চাবি = KEY2TERMCAP1(c);

	যদি (চাবি == (পূর্ণাঙ্ক)KE_LEFTRELEASE || চাবি == (পূর্ণাঙ্ক)KE_MIDDLERELEASE
					       || চাবি == (পূর্ণাঙ্ক)KE_RIGHTRELEASE)
	    netbeans_button_release(which_button);
    }
#endif

    // When jumping to another window, clear a pending operator.  That's a bit
    // friendlier than beeping and not jumping to that window.
    যদি (curwin != old_curwin && oap != রিক্ত && oap->op_type != OP_NOP)
	clearop(oap);

#ifdef FEAT_FOLDING
    যদি (mod_mask == 0
	    && !is_drag
	    && (jump_flags & (MOUSE_FOLD_CLOSE | MOUSE_FOLD_OPEN))
	    && which_button == MOUSE_বাম)
    {
	// open or close a fold at this line
	যদি (jump_flags & MOUSE_FOLD_OPEN)
	    openFold(curwin->w_cursor.lnum, 1L);
	অন্যথা
	    closeFold(curwin->w_cursor.lnum, 1L);
	// don't move the cursor if still in the same window
	যদি (curwin == old_curwin)
	    curwin->w_cursor = save_cursor;
    }
#endif

#যদি defined(FEAT_CLIPBOARD)
    যদি ((jump_flags & IN_OTHER_WIN) && !VIsual_active &&
	    (clip_star.available || clip_plus.available))
    {
	clip_modeless(which_button, is_click, is_drag);
	ঘূৰাই FALSE;
    }
#endif

    // Set global flag that we are extending the Visual area with mouse
    // dragging; temporarily minimize 'scrolloff'.
    যদি (VIsual_active && is_drag && get_scrolloff_মান())
    {
	// In the very first line, allow scrolling one line
	যদি (mouse_row == 0)
	    mouse_dragging = 2;
	অন্যথা
	    mouse_dragging = 1;
    }

    // When dragging the mouse above the window, scroll down.
    যদি (is_drag && mouse_row < 0 && !in_status_সাৰি)
    {
	scroll_redraw(FALSE, 1L);
	mouse_row = 0;
    }

    যদি (start_visual.lnum)		// right click in visual mode
    {
       // When ALT is pressed make Visual mode blockwise.
       যদি (mod_mask & MOD_MASK_ALT)
	   VIsual_mode = Ctrl_V;

	// In Visual-block mode, divide the area in four, pick up the corner
	// that is in the quarter that the cursor is in.
	যদি (VIsual_mode == Ctrl_V)
	{
	    getvcols(curwin, &start_visual, &end_visual,
						    &leftcol, &rightcol, 0);
	    যদি (curwin->w_curswant > (leftcol + rightcol) / 2)
		end_visual.col = leftcol;
	    অন্যথা
		end_visual.col = rightcol;
	    যদি (curwin->w_cursor.lnum >=
				    (start_visual.lnum + end_visual.lnum) / 2)
		end_visual.lnum = start_visual.lnum;

	    // move VIsual to the right column
	    start_visual = curwin->w_cursor;	    // save the cursor pos
	    curwin->w_cursor = end_visual;
	    coladvance(end_visual.col);
	    VIsual = curwin->w_cursor;
	    curwin->w_cursor = start_visual;	    // restore the cursor
	}
	অন্যথা
	{
	    // If the click is before the start of visual, change the start.
	    // If the click is after the end of visual, change the end.  If
	    // the click is inside the visual, change the closest side.
	    যদি (LT_POS(curwin->w_cursor, start_visual))
		VIsual = end_visual;
	    অন্যথা যদি (LT_POS(end_visual, curwin->w_cursor))
		VIsual = start_visual;
	    অন্যথা
	    {
		// In the same line, compare column number
		যদি (end_visual.lnum == start_visual.lnum)
		{
		    যদি (curwin->w_cursor.col - start_visual.col >
				    end_visual.col - curwin->w_cursor.col)
			VIsual = start_visual;
		    অন্যথা
			VIsual = end_visual;
		}

		// In different lines, compare line number
		অন্যথা
		{
		    diff = (curwin->w_cursor.lnum - start_visual.lnum) -
				(end_visual.lnum - curwin->w_cursor.lnum);

		    যদি (diff > 0)		// closest to end
			VIsual = start_visual;
		    অন্যথা যদি (diff < 0)	// closest to start
			VIsual = end_visual;
		    অন্যথা			// in the middle line
		    {
			যদি (curwin->w_cursor.col <
					(start_visual.col + end_visual.col) / 2)
			    VIsual = end_visual;
			অন্যথা
			    VIsual = start_visual;
		    }
		}
	    }
	}
    }
    // If Visual mode started in insert mode, execute "CTRL-O"
    অন্যথা যদি ((State & MODE_ঢোকাও) && VIsual_active)
	stuffcharReadbuff(Ctrl_O);

    // Middle mouse click: Put text before cursor.
    যদি (which_button == MOUSE_মধ্য)
    {
#ifdef FEAT_CLIPBOARD
	যদি (clip_star.available && regname == 0)
	    regname = '*';
#endif
	যদি (yank_register_mline(regname))
	{
	    যদি (mouse_past_bottom)
		dir = FORWARD;
	}
	অন্যথা যদি (mouse_past_eol)
	    dir = FORWARD;

	যদি (fixindent)
	{
	    c1 = (dir == BACKWARD) ? '[' : ']';
	    c2 = 'p';
	}
	অন্যথা
	{
	    c1 = (dir == FORWARD) ? 'p' : 'P';
	    c2 = NUL;
	}
	prep_redo(regname, গুনতি, NUL, c1, NUL, c2, NUL);

	// Remember where the paste started, so in edit() Insstart can be set
	// to this position
	যদি (restart_edit != 0)
	    where_paste_started = curwin->w_cursor;
	do_put(regname, রিক্ত, dir, গুনতি, fixindent | PUT_CURSEND);
    }

#যদি defined(FEAT_QUICKFIX)
    // Ctrl-Mouse click or double click in a quickfix window jumps to the
    // error under the mouse pointer.
    অন্যথা যদি (((mod_mask & MOD_MASK_CTRL)
		|| (mod_mask & MOD_MASK_MULTI_CLICK) == MOD_MASK_2CLICK)
	    && bt_quickfix(curbuf))
    {
	যদি (curwin->w_llist_ref == রিক্ত)	// quickfix window
	    do_cmdline_cmd((char_u *)".cc");
	অন্যথা					// location list window
	    do_cmdline_cmd((char_u *)".ll");
	got_click = FALSE;		// ignore drag&release now
    }
#endif

    // Ctrl-Mouse click (or double click in a help window) jumps to the tag
    // under the mouse pointer.
    অন্যথা যদি ((mod_mask & MOD_MASK_CTRL) || (curbuf->b_help
		     && (mod_mask & MOD_MASK_MULTI_CLICK) == MOD_MASK_2CLICK))
    {
	যদি (State & MODE_ঢোকাও)
	    stuffcharReadbuff(Ctrl_O);
	stuffcharReadbuff(Ctrl_RSB);
	got_click = FALSE;		// ignore drag&release now
    }

    // Shift-Mouse click searches for the next occurrence of the word under
    // the mouse pointer
    অন্যথা যদি ((mod_mask & MOD_MASK_SHIFT))
    {
	যদি ((State & MODE_ঢোকাও) || (VIsual_active && VIsual_select))
	    stuffcharReadbuff(Ctrl_O);
	যদি (which_button == MOUSE_বাম)
	    stuffcharReadbuff('*');
	অন্যথা	// MOUSE_RIGHT
	    stuffcharReadbuff('#');
    }

    // Handle double clicks, unless on status line
    অন্যথা যদি (in_status_সাৰি)
    {
#ifdef FEAT_MOUSESHAPE
	যদি ((is_drag || is_click) && !drag_status_সাৰি)
	{
	    drag_status_সাৰি = TRUE;
	    update_mouseshape(-1);
	}
#endif
    }
    অন্যথা যদি (in_sep_সাৰি)
    {
#ifdef FEAT_MOUSESHAPE
	যদি ((is_drag || is_click) && !drag_sep_সাৰি)
	{
	    drag_sep_সাৰি = TRUE;
	    update_mouseshape(-1);
	}
#endif
    }
    অন্যথা যদি ((mod_mask & MOD_MASK_MULTI_CLICK)
				       && (State & (MODE_NORMAL | MODE_ঢোকাও))
	     && mouse_has(MOUSE_VISUAL))
    {
	যদি (is_click || !VIsual_active)
	{
	    যদি (VIsual_active)
		orig_cursor = VIsual;
	    অন্যথা
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
	    যদি ((mod_mask & MOD_MASK_MULTI_CLICK) == MOD_MASK_2CLICK)
	    {
		// Double click with ALT pressed makes it blockwise.
		যদি (mod_mask & MOD_MASK_ALT)
		    VIsual_mode = Ctrl_V;
		অন্যথা
		    VIsual_mode = 'v';
	    }
	    অন্যথা যদি ((mod_mask & MOD_MASK_MULTI_CLICK) == MOD_MASK_3CLICK)
		VIsual_mode = 'V';
	    অন্যথা যদি ((mod_mask & MOD_MASK_MULTI_CLICK) == MOD_MASK_4CLICK)
		VIsual_mode = Ctrl_V;
#ifdef FEAT_CLIPBOARD
	    // Make sure the clipboard gets updated.  Needed because start and
	    // end may still be the same, and the selection needs to be owned
	    clip_star.vmode = NUL;
#endif
	}
	// A double click selects a word or a block.
	যদি ((mod_mask & MOD_MASK_MULTI_CLICK) == MOD_MASK_2CLICK)
	{
	    pos_T	*pos = রিক্ত;
	    পূর্ণাঙ্ক		gc;

	    যদি (is_click)
	    {
		// If the character under the cursor (skipping white space) is
		// not a word character, try finding a match and select a (),
		// {}, [], #if/#endif, etc. block.
		end_visual = curwin->w_cursor;
		যেতিয়ালৈ (gc = gchar_pos(&end_visual), VIM_ISWHITE(gc))
		    inc(&end_visual);
		যদি (oap != রিক্ত)
		    oap->motion_type = MCHAR;
		যদি (oap != রিক্ত
			&& VIsual_mode == 'v'
			&& !vim_iswordc(gchar_pos(&end_visual))
			&& EQUAL_POS(curwin->w_cursor, VIsual)
			&& (pos = findmatch(oap, NUL)) != রিক্ত)
		{
		    curwin->w_cursor = *pos;
		    যদি (oap->motion_type == MLINE)
			VIsual_mode = 'V';
		    অন্যথা যদি (*p_sel == 'e')
		    {
			যদি (LT_POS(curwin->w_cursor, VIsual))
			    ++VIsual.col;
			অন্যথা
			    ++curwin->w_cursor.col;
		    }
		}
	    }

	    যদি (pos == রিক্ত && (is_click || is_drag))
	    {
		// When not found a match or when dragging: extend to include
		// a word.
		যদি (LT_POS(curwin->w_cursor, orig_cursor))
		{
		    find_start_of_word(&curwin->w_cursor);
		    find_end_of_word(&VIsual);
		}
		অন্যথা
		{
		    find_start_of_word(&VIsual);
		    যদি (*p_sel == 'e' && *ml_get_cursor() != NUL)
			curwin->w_cursor.col +=
					 (*mb_ptr2len)(ml_get_cursor());
		    find_end_of_word(&curwin->w_cursor);
		}
	    }
	    curwin->w_set_curswant = true;
	}
	যদি (is_click)
	    redraw_curbuf_later(UPD_INVERTED);	// update the inversion
    }
    অন্যথা যদি (VIsual_active && !old_active)
    {
	যদি (mod_mask & MOD_MASK_ALT)
	    VIsual_mode = Ctrl_V;
	অন্যথা
	    VIsual_mode = 'v';
    }

    // If Visual mode changed show it later.
    যদি ((!VIsual_active && old_active && mode_displayed)
	    || (VIsual_active && p_smd && বাৰ্তা_silent == 0
				 && (!old_active || VIsual_mode != old_mode)))
	redraw_cmdline = TRUE;

    ঘূৰাই moved;
}

    শূন্য
ins_mouse(পূর্ণাঙ্ক c)
{
    pos_T	tpos;
    win_T	*old_curwin = curwin;

#ifdef FEAT_GUI
    // When GUI is active, also move/paste when 'mouse' is empty
    যদি (!gui.in_use)
#endif
	যদি (!mouse_has(MOUSE_ঢোকাও))
	    ঘূৰাই;

    undisplay_dollar();
    tpos = curwin->w_cursor;
    যদি (do_mouse(রিক্ত, c, BACKWARD, 1L, 0))
    {
	win_T	*new_curwin = curwin;

	যদি (curwin != old_curwin && win_valid(old_curwin))
	{
	    // Mouse took us to another window.  We need to go back to the
	    // previous one to stop insert there properly.
	    curwin = old_curwin;
	    curbuf = curwin->w_buffer;
#ifdef FEAT_JOB_CHANNEL
	    যদি (bt_prompt(curbuf))
		// Restart Insert mode when re-entering the prompt buffer.
		curbuf->b_prompt_ঢোকাও = 'A';
#endif
	}
	start_arrow(curwin == old_curwin ? &tpos : রিক্ত);
	যদি (curwin != new_curwin && win_valid(new_curwin))
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
    স্থির শূন্য
do_mousescroll(cmdarg_T *cap)
{
    পূর্ণাঙ্ক shift_or_ctrl = mod_mask & (MOD_MASK_SHIFT | MOD_MASK_CTRL);

#ifdef FEAT_TERMINAL
    যদি (term_use_loop())
	// This window is a terminal window, send the mouse event there.
	// Set "typed" to FALSE to avoid an endless loop.
	send_চাবিগুলি_to_term(curbuf->b_term, cap->cmdchar, mod_mask, FALSE);
    অন্যথা
#endif
    যদি (cap->arg == MSCR_UP || cap->arg == MSCR_DOWN)
    {
	// Vertical scrolling
	যদি (!(State & MODE_ঢোকাও) && (mouse_vert_step < 0 || shift_or_ctrl))
	{
	    // whole page up or down
	    pagescroll(cap->arg == MSCR_UP ? FORWARD : BACKWARD, 1L, FALSE);
	}
	অন্যথা
	{
	    যদি (mouse_vert_step < 0 || shift_or_ctrl)
	    {
		// whole page up or down
		cap->count1 = (long)(curwin->w_botline - curwin->w_topline);
	    }
	    // Don't scroll more than half the window height.
	    অন্যথা যদি (curwin->w_height < mouse_vert_step * 2)
	    {
		cap->count1 = curwin->w_height / 2;
		যদি (cap->count1 == 0)
		    cap->count1 = 1;
	    }
	    অন্যথা
	    {
		cap->count1 = mouse_vert_step;
	    }
	    cap->count0 = cap->count1;
	    nv_scroll_সাৰি(cap);
	}

#ifdef FEAT_PROP_POPUP
	যদি (WIN_IS_POPUP(curwin))
	    popup_set_firstline(curwin);
#endif
    }
    অন্যথা
    {
	// Horizontal scrolling
	long step = (mouse_hor_step < 0 || shift_or_ctrl)
					    ? curwin->w_width : mouse_hor_step;
	long leftcol = curwin->w_leftcol
				     + (cap->arg == MSCR_ডান ? -step : step);
	যদি (leftcol < 0)
	    leftcol = 0;
	do_mousescroll_horiz((long_u)leftcol);
    }
    may_trigger_win_scrolled_resized();
}

/*
 * Insert mode implementation for scrolling in direction "dir", which is
 * one of the MSCR_ values.
 */
    শূন্য
ins_mousescroll(পূর্ণাঙ্ক dir)
{
    cmdarg_T	cap;
    oparg_T	oa;
    CLEAR_FIELD(cap);
    clear_oparg(&oa);
    cap.oap = &oa;
    cap.arg = dir;

#ifdef FEAT_TABPANEL
    যদি (mouse_row >= 0 && mouse_col >= 0
	    && (dir == MSCR_UP || dir == MSCR_DOWN)
	    && mouse_on_tabpanel())
    {
	(শূন্য)tabpanel_scroll(dir == MSCR_UP ? 1 : -1,
		mouse_vert_step > 0 ? mouse_vert_step : 3);
	ঘূৰাই;
    }
#endif

    ৱিকল্প (dir)
    {
	পক্ষ MSCR_UP:
	    cap.cmdchar = K_MOUSEUP;
	    ভঙ্গ;
	পক্ষ MSCR_DOWN:
	    cap.cmdchar = K_MOUSEDOWN;
	    ভঙ্গ;
	পক্ষ MSCR_বাম:
	    cap.cmdchar = K_MOUSELEFT;
	    ভঙ্গ;
	পক্ষ MSCR_ডান:
	    cap.cmdchar = K_MOUSERIGHT;
	    ভঙ্গ;
	স্ৱভাৱ:
	    siemsg("Invalid ins_mousescroll() argument: %d", dir);
    }

    win_T *old_curwin = curwin;
    যদি (mouse_row >= 0 && mouse_col >= 0)
    {
	// Find the window at the mouse pointer coordinates.
	// NOTE: Must restore "curwin" to "old_curwin" before returning!
	পূর্ণাঙ্ক row = mouse_row;
	পূর্ণাঙ্ক col = mouse_col;
	curwin = mouse_find_win(&row, &col, FIND_POPUP);
	যদি (curwin == রিক্ত)
	{
	    curwin = old_curwin;
	    ঘূৰাই;
	}
	curbuf = curwin->w_buffer;
    }

    যদি (curwin == old_curwin)
    {
	// Don't scroll the current window if the popup menu is visible.
	যদি (pum_visible())
	    ঘূৰাই;

	undisplay_dollar();
    }

    linenr_T	orig_topline = curwin->w_topline;
    colnr_T	orig_leftcol = curwin->w_leftcol;
    pos_T	orig_cursor = curwin->w_cursor;

    // Call the common mouse scroll function shared with other modes.
    do_mousescroll(&cap);

    পূর্ণাঙ্ক did_scroll = (orig_topline != curwin->w_topline
		   || orig_leftcol != curwin->w_leftcol);

    curwin->w_redr_status = true;
    curwin = old_curwin;
    curbuf = curwin->w_buffer;

    // If the window actually scrolled and the popup menu may overlay the
    // window, need to redraw it.
    যদি (did_scroll && pum_visible())
    {
	// TODO: Would be more efficient to only redraw the windows that are
	// overlapped by the popup menu.
	redraw_all_later(UPD_NOT_VALID);
	ins_compl_show_pum();
    }

    যদি (!EQUAL_POS(curwin->w_cursor, orig_cursor))
    {
	start_arrow(&orig_cursor);
	set_can_cindent(TRUE);
    }
}

#যদি defined(FEAT_PROP_POPUP)
/*
 * Command-line mode implementation for scrolling in direction "dir", which is
 * one of the MSCR_ values.  Scrolls the completion info popup when the mouse
 * pointer is on top of it.
 * Returns TRUE when the info popup was scrolled.
 */
    পূর্ণাঙ্ক
cmdline_mousescroll(পূর্ণাঙ্ক dir)
{
    cmdarg_T	cap;
    oparg_T	oa;

    CLEAR_FIELD(cap);
    clear_oparg(&oa);
    cap.oap = &oa;
    cap.arg = dir;

    ৱিকল্প (dir)
    {
	পক্ষ MSCR_UP:	 cap.cmdchar = K_MOUSEUP; ভঙ্গ;
	পক্ষ MSCR_DOWN:	 cap.cmdchar = K_MOUSEDOWN; ভঙ্গ;
	পক্ষ MSCR_বাম:	 cap.cmdchar = K_MOUSELEFT; ভঙ্গ;
	পক্ষ MSCR_ডান: cap.cmdchar = K_MOUSERIGHT; ভঙ্গ;
    }

    যদি (mouse_row < 0 || mouse_col < 0)
	ঘূৰাই FALSE;

    পূর্ণাঙ্ক	    row = mouse_row;
    পূর্ণাঙ্ক	    col = mouse_col;
    win_T   *wp;

    // Only scroll when the mouse is on top of the info popup.
    wp = mouse_find_win(&row, &col, FIND_POPUP);
    যদি (wp == রিক্ত || !WIN_IS_POPUP(wp) || !(wp->w_popup_flags & POPF_INFO)
							|| !wp->w_has_scrollbar)
	ঘূৰাই FALSE;

    win_T   *old_curwin = curwin;

    curwin = wp;
    curbuf = wp->w_buffer;
    // Call the common mouse scroll function shared with other modes.
    do_mousescroll(&cap);
    curwin = old_curwin;
    curbuf = curwin->w_buffer;

    // Cmdline mode doesn't normally call update_screen(), so redraw the
    // completion popup menu, which also repaints the info popup.
    যদি (cmdline_pum_active())
	cmdline_pum_display();
    ঘূৰাই TRUE;
}
#endif

/*
 * Return TRUE if "c" is a mouse key.
 */
    পূর্ণাঙ্ক
is_mouse_চাবি(পূর্ণাঙ্ক c)
{
    ঘূৰাই c == K_LEFTMOUSE
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

স্থির সংরচনা mousetable
{
    পূর্ণাঙ্ক	    pseudo_code;	// Code for pseudo mouse event
    পূর্ণাঙ্ক	    button;		// Which mouse button is it?
    পূর্ণাঙ্ক	    is_click;		// Is it a mouse button click event?
    পূর্ণাঙ্ক	    is_drag;		// Is it a mouse drag event?
} mouse_table[] =
{
    {(পূর্ণাঙ্ক)KE_LEFTMOUSE,		MOUSE_বাম,	TRUE,	FALSE},
#ifdef FEAT_GUI
    {(পূর্ণাঙ্ক)KE_LEFTMOUSE_NM,	MOUSE_বাম,	TRUE,	FALSE},
#endif
    {(পূর্ণাঙ্ক)KE_LEFTDRAG,		MOUSE_বাম,	FALSE,	TRUE},
    {(পূর্ণাঙ্ক)KE_LEFTRELEASE,	MOUSE_বাম,	FALSE,	FALSE},
#ifdef FEAT_GUI
    {(পূর্ণাঙ্ক)KE_LEFTRELEASE_NM,	MOUSE_বাম,	FALSE,	FALSE},
#endif
    {(পূর্ণাঙ্ক)KE_MIDDLEMOUSE,	MOUSE_মধ্য,	TRUE,	FALSE},
    {(পূর্ণাঙ্ক)KE_MIDDLEDRAG,	MOUSE_মধ্য,	FALSE,	TRUE},
    {(পূর্ণাঙ্ক)KE_MIDDLERELEASE,	MOUSE_মধ্য,	FALSE,	FALSE},
    {(পূর্ণাঙ্ক)KE_RIGHTMOUSE,	MOUSE_ডান,	TRUE,	FALSE},
    {(পূর্ণাঙ্ক)KE_RIGHTDRAG,		MOUSE_ডান,	FALSE,	TRUE},
    {(পূর্ণাঙ্ক)KE_RIGHTRELEASE,	MOUSE_ডান,	FALSE,	FALSE},
    {(পূর্ণাঙ্ক)KE_X1MOUSE,		MOUSE_X1,	TRUE,	FALSE},
    {(পূর্ণাঙ্ক)KE_X1DRAG,		MOUSE_X1,	FALSE,	TRUE},
    {(পূর্ণাঙ্ক)KE_X1RELEASE,		MOUSE_X1,	FALSE,	FALSE},
    {(পূর্ণাঙ্ক)KE_X2MOUSE,		MOUSE_X2,	TRUE,	FALSE},
    {(পূর্ণাঙ্ক)KE_X2DRAG,		MOUSE_X2,	FALSE,	TRUE},
    {(পূর্ণাঙ্ক)KE_X2RELEASE,		MOUSE_X2,	FALSE,	FALSE},
    // DRAG without CLICK
    {(পূর্ণাঙ্ক)KE_MOUSEMOVE,		MOUSE_RELEASE,	FALSE,	TRUE},
    // RELEASE without CLICK
    {(পূর্ণাঙ্ক)KE_IGNORE,		MOUSE_RELEASE,	FALSE,	FALSE},
    {0,				0,		0,	0},
};

/*
 * Look up the given mouse code to return the relevant information in the other
 * arguments.  Return which button is down or was released.
 */
    পূর্ণাঙ্ক
get_mouse_button(পূর্ণাঙ্ক code, পূর্ণাঙ্ক *is_click, পূর্ণাঙ্ক *is_drag)
{
    পূর্ণাঙ্ক	    i;

    বাবে (i = 0; mouse_table[i].pseudo_code; i++)
	যদি (code == mouse_table[i].pseudo_code)
	{
	    *is_click = mouse_table[i].is_click;
	    *is_drag = mouse_table[i].is_drag;
	    ঘূৰাই mouse_table[i].button;
	}
    ঘূৰাই 0;	    // Shouldn't get here
}

/*
 * Return the appropriate pseudo mouse event token (KE_LEFTMOUSE etc) based on
 * the given information about which mouse button is down, and whether the
 * mouse was clicked, dragged or released.
 */
    পূর্ণাঙ্ক
get_pseudo_mouse_code(
    পূর্ণাঙ্ক	    button,	// eg MOUSE_LEFT
    পূর্ণাঙ্ক	    is_click,
    পূর্ণাঙ্ক	    is_drag)
{
    পূর্ণাঙ্ক	    i;

    বাবে (i = 0; mouse_table[i].pseudo_code; i++)
	যদি (button == mouse_table[i].button
	    && is_click == mouse_table[i].is_click
	    && is_drag == mouse_table[i].is_drag)
	{
#ifdef FEAT_GUI
	    // Trick: a non mappable left click and release has mouse_col -1
	    // or added MOUSE_COLOFF.  Used for 'mousefocus' in
	    // gui_mouse_moved()
	    যদি (mouse_col < 0 || mouse_col > MOUSE_COLOFF)
	    {
		যদি (mouse_col < 0)
		    mouse_col = 0;
		অন্যথা
		    mouse_col -= MOUSE_COLOFF;
		যদি (mouse_table[i].pseudo_code == (পূর্ণাঙ্ক)KE_LEFTMOUSE)
		    ঘূৰাই (পূর্ণাঙ্ক)KE_LEFTMOUSE_NM;
		যদি (mouse_table[i].pseudo_code == (পূর্ণাঙ্ক)KE_LEFTRELEASE)
		    ঘূৰাই (পূর্ণাঙ্ক)KE_LEFTRELEASE_NM;
	    }
#endif
	    ঘূৰাই mouse_table[i].pseudo_code;
	}
    ঘূৰাই (পূর্ণাঙ্ক)KE_IGNORE;	    // not recognized, ignore it
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
স্থির পূর্ণাঙ্ক has_mouse_termcode = 0;

    শূন্য
set_mouse_termcode(
    পূর্ণাঙ্ক		n,	// KS_MOUSE, KS_NETTERM_MOUSE or KS_DEC_MOUSE
    char_u	*s)
{
    char_u	name[2];

    name[0] = n;
    name[1] = KE_FILLER;
    add_termcode(name, s, FALSE);
#ifdef FEAT_MOUSE_JSB
    যদি (n == KS_JSBTERM_MOUSE)
	has_mouse_termcode |= HMT_JSBTERM;
    অন্যথা
#endif
#ifdef FEAT_MOUSE_NET
    যদি (n == KS_NETTERM_MOUSE)
	has_mouse_termcode |= HMT_NETTERM;
    অন্যথা
#endif
#ifdef FEAT_MOUSE_DEC
    যদি (n == KS_DEC_MOUSE)
	has_mouse_termcode |= HMT_DEC;
    অন্যথা
#endif
#ifdef FEAT_MOUSE_PTERM
    যদি (n == KS_PTERM_MOUSE)
	has_mouse_termcode |= HMT_PTERM;
    অন্যথা
#endif
#ifdef FEAT_MOUSE_URXVT
    যদি (n == KS_URXVT_MOUSE)
	has_mouse_termcode |= HMT_URXVT;
    অন্যথা
#endif
#ifdef FEAT_MOUSE_GPM
    যদি (n == KS_GPM_MOUSE)
	has_mouse_termcode |= HMT_GPM;
    অন্যথা
#endif
    যদি (n == KS_SGR_MOUSE)
	has_mouse_termcode |= HMT_SGR;
    অন্যথা যদি (n == KS_SGR_MOUSE_RELEASE)
	has_mouse_termcode |= HMT_SGR_REL;
    অন্যথা
	has_mouse_termcode |= HMT_NORMAL;
}

#যদি defined(UNIX) || defined(VMS)
    শূন্য
del_mouse_termcode(
    পূর্ণাঙ্ক		n)	// KS_MOUSE, KS_NETTERM_MOUSE or KS_DEC_MOUSE
{
    char_u	name[2];

    name[0] = n;
    name[1] = KE_FILLER;
    del_termcode(name);
# ifdef FEAT_MOUSE_JSB
    যদি (n == KS_JSBTERM_MOUSE)
	has_mouse_termcode &= ~HMT_JSBTERM;
    অন্যথা
# endif
# ifdef FEAT_MOUSE_NET
    যদি (n == KS_NETTERM_MOUSE)
	has_mouse_termcode &= ~HMT_NETTERM;
    অন্যথা
# endif
# ifdef FEAT_MOUSE_DEC
    যদি (n == KS_DEC_MOUSE)
	has_mouse_termcode &= ~HMT_DEC;
    অন্যথা
# endif
# ifdef FEAT_MOUSE_PTERM
    যদি (n == KS_PTERM_MOUSE)
	has_mouse_termcode &= ~HMT_PTERM;
    অন্যথা
# endif
# ifdef FEAT_MOUSE_URXVT
    যদি (n == KS_URXVT_MOUSE)
	has_mouse_termcode &= ~HMT_URXVT;
    অন্যথা
# endif
# ifdef FEAT_MOUSE_GPM
    যদি (n == KS_GPM_MOUSE)
	has_mouse_termcode &= ~HMT_GPM;
    অন্যথা
# endif
    যদি (n == KS_SGR_MOUSE)
	has_mouse_termcode &= ~HMT_SGR;
    অন্যথা যদি (n == KS_SGR_MOUSE_RELEASE)
	has_mouse_termcode &= ~HMT_SGR_REL;
    অন্যথা
	has_mouse_termcode &= ~HMT_NORMAL;
}
#endif

/*
 * setmouse() - switch mouse on/off depending on current mode and 'mouse'
 */
    শূন্য
setmouse(শূন্য)
{
    পূর্ণাঙ্ক	    checkfor;

#ifdef FEAT_MOUSESHAPE
    update_mouseshape(-1);
#endif

    // Should be outside proc, but may break MOUSESHAPE
#ifdef FEAT_GUI
    // In the GUI the mouse is always enabled.
    যদি (gui.in_use)
	ঘূৰাই;
#endif
    // be quick when mouse is off
    যদি (*p_mouse == NUL || has_mouse_termcode == 0)
	ঘূৰাই;

    // don't switch mouse on when not in raw mode (Ex mode)
    যদি (cur_tmode != TMODE_RAW)
    {
	mch_setmouse(FALSE);
	ঘূৰাই;
    }

    যদি (VIsual_active)
	checkfor = MOUSE_VISUAL;
    অন্যথা যদি (State == MODE_HITRETURN || State == MODE_ASKMORE
						     || State == MODE_SETWSIZE)
	checkfor = MOUSE_RETURN;
    অন্যথা যদি (State & MODE_ঢোকাও)
	checkfor = MOUSE_ঢোকাও;
    অন্যথা যদি (State & MODE_CMDLINE)
	checkfor = MOUSE_COMMAND;
    অন্যথা যদি (State == MODE_CONFIRM || State == MODE_EXTERNCMD)
	checkfor = ' '; // don't use mouse for ":confirm" or ":!cmd"
    অন্যথা
	checkfor = MOUSE_NORMAL;    // assume normal mode

    যদি (mouse_has(checkfor))
	mch_setmouse(TRUE);
    অন্যথা
	mch_setmouse(FALSE);
}

/*
 * Return TRUE if
 * - "c" is in 'mouse', or
 * - 'a' is in 'mouse' and "c" is in MOUSE_A, or
 * - the current buffer is a help file and 'h' is in 'mouse' and we are in a
 *   normal editing mode (not at hit-return message).
 */
    পূর্ণাঙ্ক
mouse_has(পূর্ণাঙ্ক c)
{
    char_u	*p;

    বাবে (p = p_mouse; *p; ++p)
	ৱিকল্প (*p)
	{
	    পক্ষ 'a': যদি (vim_strchr((char_u *)MOUSE_A, c) != রিক্ত)
			  ঘূৰাই TRUE;
		      ভঙ্গ;
	    পক্ষ MOUSE_HELP: যদি (c != MOUSE_RETURN && curbuf->b_help)
				 ঘূৰাই TRUE;
			     ভঙ্গ;
	    স্ৱভাৱ: যদি (c == *p) ঘূৰাই TRUE; ভঙ্গ;
	}
    ঘূৰাই FALSE;
}

/*
 * Return TRUE when 'mousemodel' is set to "popup" or "popup_setpos".
 */
    পূর্ণাঙ্ক
mouse_model_popup(শূন্য)
{
    ঘূৰাই (p_mousem[0] == 'p');
}

/*
 * Call a click-region callback function.
 * "regions"/"region_count" describe the resolved click regions,
 * "winid" is stored as the "winid" key in the info dict (0 for tabline).
 * Returns TRUE if the function was called and handled the click.
 */
    স্থির পূর্ণাঙ্ক
stl_click_handler_regions(
	stl_click_region_T  *regions,
	পূর্ণাঙ্ক		    region_গুনতি,
	পূর্ণাঙ্ক		    winid,
	char_u		    *area_name,
	পূর্ণাঙ্ক		    mrow,
	পূর্ণাঙ্ক		    mcol,
	পূর্ণাঙ্ক		    which_button,
	পূর্ণাঙ্ক		    mods)
{
#ifdef FEAT_EVAL
    পূর্ণাঙ্ক		n;
    পূর্ণাঙ্ক		nclicks;
    char_u	button_str[2];
    char_u	mods_str[4];
    পূর্ণাঙ্ক		mi = 0;
    dict_T	*info;
    typval_T	argvars[2];
    typval_T	rettv;
    funcexe_T	funcexe;
    পূর্ণাঙ্ক		col = mcol;

    যদি (regions == রিক্ত || region_গুনতি == 0)
	ঘূৰাই FALSE;

    // Find the click region at the given row and column.
    বাবে (n = 0; n < region_গুনতি; n++)
    {
	যদি (regions[n].row == mrow
		&& col >= regions[n].col_start
		&& col < regions[n].col_end)
	    ভঙ্গ;
    }
    যদি (n >= region_গুনতি || regions[n].funcname == রিক্ত)
	ঘূৰাই FALSE;

    // Build the info dictionary.
    info = dict_alloc();
    যদি (info == রিক্ত)
	ঘূৰাই FALSE;

    dict_add_সংখ্যা(info, "minwid", regions[n].minwid);

    // Determine number of clicks.
    // MOD_MASK_2CLICK=0x20, MOD_MASK_3CLICK=0x40, MOD_MASK_4CLICK=0x60
    nclicks = ((mods & MOD_MASK_MULTI_CLICK) >> 5) + 1;
    যদি (nclicks > 3)
	nclicks = 3;
    dict_add_সংখ্যা(info, "nclicks", nclicks);

    // Button.
    যদি (which_button == MOUSE_বাম)
	button_str[0] = 'l';
    অন্যথা যদি (which_button == MOUSE_ডান)
	button_str[0] = 'r';
    অন্যথা
	button_str[0] = 'm';
    button_str[1] = NUL;
    dict_add_string_len(info, "button", button_str, 1);

    // Modifiers.
    যদি (mods & MOD_MASK_SHIFT)
	mods_str[mi++] = 's';
    যদি (mods & MOD_MASK_CTRL)
	mods_str[mi++] = 'c';
    যদি (mods & MOD_MASK_ALT)
	mods_str[mi++] = 'a';
    mods_str[mi] = NUL;
    dict_add_string_len(info, "mods", mods_str, mi);

    dict_add_সংখ্যা(info, "winid", winid);

    // "area": which option the clicked region belongs to.  Lets a shared
    // dispatcher distinguish 'statusline', 'tabline' and 'tabpanel' without
    // having to overload winid == 0.
    dict_add_string(info, "area", area_name);

    // Expose tab page number for 'tabpanel' regions.
    যদি (regions[n].tabnr > 0)
	dict_add_সংখ্যা(info, "tabnr", regions[n].tabnr);

    // Call the function with the info dict as argument.
    argvars[0].v_type = VAR_DICT;
    argvars[0].vval.v_dict = info;
    ++info->dv_refcount;
    argvars[1].v_type = VAR_UNKNOWN;

    rettv.v_type = VAR_সংখ্যা;
    rettv.vval.v_সংখ্যা = 0;

    CLEAR_FIELD(funcexe);
    funcexe.fe_evaluate = TRUE;
    (শূন্য)call_func(regions[n].funcname, -1,
					    &rettv, 1, argvars, &funcexe);

    n = (পূর্ণাঙ্ক)rettv.vval.v_সংখ্যা;
    clear_tv(&rettv);
    dict_unref(info);

    যদি (n != 0)
    {
	// Make sure the tabline gets redrawn too when the callback asks for
	// a redraw (redraw_statuslines() only redraws the tabline when
	// redraw_tabline is set).  For tabpanel the whole screen needs to be
	// refreshed.
	যদি (winid == 0)
	    redraw_tabline = TRUE;
# ifdef FEAT_TABPANEL
	যদি (STRCMP(area_name, "tabpanel") == 0)
	    redraw_all_later(UPD_NOT_VALID);
# endif
	redraw_statuslines();
    }

    ঘূৰাই TRUE;
#অন্যথা
    (শূন্য)regions;
    (শূন্য)region_গুনতি;
    (শূন্য)winid;
    (শূন্য)area_name;
    (শূন্য)mrow;
    (শূন্য)mcol;
    (শূন্য)which_button;
    (শূন্য)mods;
    ঘূৰাই FALSE;
#endif
}

/*
 * Call a statusline click handler function for window "wp".
 * Returns TRUE if the function was called and handled the click.
 */
    স্থির পূর্ণাঙ্ক
stl_click_handler(win_T *wp, পূর্ণাঙ্ক mrow, পূর্ণাঙ্ক mcol, পূর্ণাঙ্ক which_button, পূর্ণাঙ্ক mods)
{
    যদি (wp == রিক্ত)
	ঘূৰাই FALSE;
    ঘূৰাই stl_click_handler_regions(wp->w_stl_click, wp->w_stl_click_গুনতি,
				wp->w_id, (char_u *)"statusline",
				mrow, mcol, which_button, mods);
}

// dragwin is declared near the top of the file

/*
 * Reset the window being dragged.  To be called when switching tab page.
 */
    শূন্য
reset_dragwin(শূন্য)
{
    dragwin = রিক্ত;
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
    পূর্ণাঙ্ক
jump_to_mouse(
    পূর্ণাঙ্ক		flags,
    পূর্ণাঙ্ক		*inclusive,	// used for inclusive operator, can be NULL
    পূর্ণাঙ্ক		which_button)	// MOUSE_LEFT, MOUSE_RIGHT, MOUSE_MIDDLE
{
    স্থির পূর্ণাঙ্ক	on_status_সাৰি = 0;	// #lines below bottom of window
    স্থির পূর্ণাঙ্ক	on_sep_সাৰি = 0;	// on separator right of window
#ifdef FEAT_MENU
    স্থির পূর্ণাঙ্ক  in_winbar = FALSE;
#endif
#ifdef FEAT_PROP_POPUP
    স্থির পূর্ণাঙ্ক   in_popup_win = FALSE;
    স্থির win_T *click_in_popup_win = রিক্ত;
#endif
    স্থির পূর্ণাঙ্ক	prev_row = -1;
    স্থির পূর্ণাঙ্ক	prev_col = -1;
    স্থির পূর্ণাঙ্ক	did_drag = FALSE;	// drag was noticed

    win_T	*wp, *old_curwin;
    pos_T	old_cursor;
    পূর্ণাঙ্ক		গুনতি;
    পূর্ণাঙ্ক		first;
    পূর্ণাঙ্ক		row = mouse_row;
    পূর্ণাঙ্ক		col = mouse_col;
    colnr_T	col_from_screen = -1;
#ifdef FEAT_FOLDING
    পূর্ণাঙ্ক		mouse_char = ' ';
#endif

    mouse_past_bottom = FALSE;
    mouse_past_eol = FALSE;

    যদি (flags & MOUSE_RELEASED)
    {
	// On button release we may change window focus if positioned on a
	// status line and no dragging happened.
	যদি (dragwin != রিক্ত && !did_drag)
	    flags &= ~(MOUSE_FOCUS | MOUSE_DID_MOVE);
	dragwin = রিক্ত;
	did_drag = FALSE;
#ifdef FEAT_PROP_POPUP
	যদি (click_in_popup_win != রিক্ত && popup_dragwin == রিক্ত)
	    popup_close_for_mouse_click(click_in_popup_win);

	popup_dragwin = রিক্ত;
	click_in_popup_win = রিক্ত;
#endif
    }

    যদি ((flags & MOUSE_DID_MOVE)
	    && prev_row == mouse_row
	    && prev_col == mouse_col)
    {
retnomove:
	// before moving the cursor for a left click which is NOT in a status
	// line, stop Visual mode
	যদি (on_status_সাৰি)
	    ঘূৰাই IN_STATUS_সাৰি;
	যদি (on_sep_সাৰি)
	    ঘূৰাই IN_SEP_সাৰি;
#ifdef FEAT_MENU
	যদি (in_winbar)
	{
	    // A quick second click may arrive as a double-click, but we use it
	    // as a second click in the WinBar.
	    যদি ((mod_mask & MOD_MASK_MULTI_CLICK) && !(flags & MOUSE_RELEASED))
	    {
		wp = mouse_find_win(&row, &col, FAIL_POPUP);
		যদি (wp == রিক্ত)
		    ঘূৰাই IN_UNKNOWN;
		winbar_click(wp, col);
	    }
	    ঘূৰাই IN_OTHER_WIN | MOUSE_WINBAR;
	}
#endif
	যদি (flags & MOUSE_MAY_STOP_VIS)
	{
	    end_visual_mode_keep_button();
	    redraw_curbuf_later(UPD_INVERTED);	// delete the inversion
	}
#যদি defined(FEAT_CLIPBOARD)
	// Continue a modeless selection in another window.
	যদি (cmdwin_type != 0 && row < cmdwin_win->w_winrow)
	    ঘূৰাই IN_OTHER_WIN;
#endif
#ifdef FEAT_PROP_POPUP
	// Continue a modeless selection in a popup window or dragging it.
	যদি (in_popup_win)
	{
	    click_in_popup_win = রিক্ত;  // don't close it on release
	    যদি (popup_dragwin != রিক্ত)
	    {
		// dragging a popup window
		popup_drag(popup_dragwin);
		ঘূৰাই IN_UNKNOWN;
	    }
	    ঘূৰাই IN_OTHER_WIN;
	}
#endif
	ঘূৰাই IN_BUFFER;
    }

    prev_row = mouse_row;
    prev_col = mouse_col;

    যদি (flags & MOUSE_SETPOS)
	গচ্ছ retnomove;				// ugly goto...

    old_curwin = curwin;
    old_cursor = curwin->w_cursor;

    যদি (!(flags & MOUSE_FOCUS))
    {
	যদি (row < 0 || col < 0) // check if it makes sense
	    ঘূৰাই IN_UNKNOWN;

	// find the window where the row is in and adjust "row" and "col" to be
	// relative to top-left of the window
	wp = mouse_find_win(&row, &col, FIND_POPUP);
	যদি (wp == রিক্ত)
	    ঘূৰাই IN_UNKNOWN;
	dragwin = রিক্ত;

#ifdef FEAT_PROP_POPUP
	// Click in a popup window may start dragging or modeless selection,
	// but not much else.
	যদি (WIN_IS_POPUP(wp))
	{
	    on_sep_সাৰি = 0;
	    on_status_সাৰি = 0;
	    in_popup_win = TRUE;
	    যদি (which_button == MOUSE_বাম && popup_close_if_on_X(wp, row, col))
	    {
		ঘূৰাই IN_UNKNOWN;
	    }
	    অন্যথা যদি (((wp->w_popup_flags & (POPF_DRAG | POPF_RESIZE))
					      && popup_on_border(wp, row, col))
				       || (wp->w_popup_flags & POPF_DRAGALL))
	    {
		popup_dragwin = wp;
		popup_start_drag(wp, row, col);
		ঘূৰাই IN_UNKNOWN;
	    }
	    // Only close on release, otherwise it's not possible to drag or do
	    // modeless selection.
	    অন্যথা যদি (wp->w_popup_close == POPCLOSE_CLICK
		    && which_button == MOUSE_বাম)
	    {
		click_in_popup_win = wp;
	    }
	    অন্যথা যদি (which_button == MOUSE_বাম)
		// If the click is in the scrollbar, may scroll up/down.
		popup_handle_scrollbar_click(wp, row, col);
# ifdef FEAT_CLIPBOARD
	    ঘূৰাই IN_OTHER_WIN;
# অন্যথা
	    ঘূৰাই IN_UNKNOWN;
# endif
	}
	in_popup_win = FALSE;
	popup_dragwin = রিক্ত;
#endif
#ifdef FEAT_MENU
	যদি (row == -1)
	{
	    // A click in the window toolbar does not enter another window or
	    // change Visual highlighting.
	    winbar_click(wp, col);
	    in_winbar = TRUE;
	    ঘূৰাই IN_OTHER_WIN | MOUSE_WINBAR;
	}
	in_winbar = FALSE;
#endif

	// winpos and height may change in win_enter()!
	যদি (row >= wp->w_height)		// In (or below) status line
	{
	    on_status_সাৰি = row - wp->w_height + 1;
	    dragwin = wp;
	}
	অন্যথা
	    on_status_সাৰি = 0;
	যদি (col >= wp->w_width)		// In separator line
	{
	    on_sep_সাৰি = col - wp->w_width + 1;
	    dragwin = wp;
	}
	অন্যথা
	    on_sep_সাৰি = 0;

	// The rightmost character of the status line might be a vertical
	// separator character if there is no connecting window to the right.
	যদি (on_status_সাৰি && on_sep_সাৰি)
	{
	    যদি (stl_connected(wp))
		on_sep_সাৰি = 0;
	    অন্যথা
		on_status_সাৰি = 0;
	}

	// Before jumping to another buffer, or moving the cursor for a left
	// click, stop Visual mode.
	যদি (VIsual_active
		&& (wp->w_buffer != curwin->w_buffer
		    || (!on_status_সাৰি && !on_sep_সাৰি
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
	যদি (cmdwin_type != 0 && wp != cmdwin_win)
	{
	    // A click outside the command-line window: Use modeless
	    // selection if possible.  Allow dragging the status lines.
	    on_sep_সাৰি = 0;
#ifdef FEAT_CLIPBOARD
	    যদি (on_status_সাৰি)
		ঘূৰাই IN_STATUS_সাৰি;
	    ঘূৰাই IN_OTHER_WIN;
#অন্যথা
	    row = 0;
	    col += wp->w_wincol;
	    wp = cmdwin_win;
#endif
	}
#যদি defined(FEAT_PROP_POPUP) && defined(FEAT_TERMINAL)
	যদি (popup_is_popup(curwin) && curbuf->b_term != রিক্ত)
	    // terminal in popup window: don't jump to another window
	    ঘূৰাই IN_OTHER_WIN;
#endif
	// Only change window focus when not clicking on or dragging the
	// status line.  Do change focus when releasing the mouse button
	// (MOUSE_FOCUS was set above if we dragged first).
	যদি (dragwin == রিক্ত || (flags & MOUSE_RELEASED))
	    win_enter(wp, TRUE);		// can make wp invalid!

	যদি (curwin != old_curwin)
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
	যদি (on_status_সাৰি)			// In (or below) status line
	{
	    // Don't use start_arrow() if we're in the same window
	    যদি (curwin == old_curwin)
		ঘূৰাই IN_STATUS_সাৰি;
	    অন্যথা
		ঘূৰাই IN_STATUS_সাৰি | CURSOR_MOVED;
	}
	যদি (on_sep_সাৰি)			// In (or below) status line
	{
	    // Don't use start_arrow() if we're in the same window
	    যদি (curwin == old_curwin)
		ঘূৰাই IN_SEP_সাৰি;
	    অন্যথা
		ঘূৰাই IN_SEP_সাৰি | CURSOR_MOVED;
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
    অন্যথা যদি (on_status_সাৰি && which_button == MOUSE_বাম)
    {
	যদি (dragwin != রিক্ত)
	{
	    // Drag the status line
	    গুনতি = row - W_WINROW(dragwin) - dragwin->w_height + 1
							     - on_status_সাৰি;
	    win_drag_status_সাৰি(dragwin, গুনতি);
	    did_drag |= গুনতি;
	}
	ঘূৰাই IN_STATUS_সাৰি;			// Cursor didn't move
    }
    অন্যথা যদি (on_sep_সাৰি && which_button == MOUSE_বাম)
    {
	যদি (dragwin != রিক্ত)
	{
	    // Drag the separator column
	    গুনতি = col - dragwin->w_wincol - dragwin->w_width + 1
								- on_sep_সাৰি;
	    win_drag_vsep_সাৰি(dragwin, গুনতি);
	    did_drag |= গুনতি;
	}
	ঘূৰাই IN_SEP_সাৰি;			// Cursor didn't move
    }
#ifdef FEAT_MENU
    অন্যথা যদি (in_winbar)
    {
	// After a click on the window toolbar don't start Visual mode.
	ঘূৰাই IN_OTHER_WIN | MOUSE_WINBAR;
    }
#endif
    অন্যথা // keep_window_focus must be TRUE
    {
	// before moving the cursor for a left click, stop Visual mode
	যদি (flags & MOUSE_MAY_STOP_VIS)
	{
	    end_visual_mode_keep_button();
	    redraw_curbuf_later(UPD_INVERTED);	// delete the inversion
	}

#যদি defined(FEAT_CLIPBOARD)
	// Continue a modeless selection in another window.
	যদি (cmdwin_type != 0 && row < cmdwin_win->w_winrow)
	    ঘূৰাই IN_OTHER_WIN;
#endif
#ifdef FEAT_PROP_POPUP
	যদি (in_popup_win)
	{
	    যদি (popup_dragwin != রিক্ত)
	    {
		// dragging a popup window
		popup_drag(popup_dragwin);
		ঘূৰাই IN_UNKNOWN;
	    }
	    // continue a modeless selection in a popup window
	    click_in_popup_win = রিক্ত;
	    ঘূৰাই IN_OTHER_WIN;
	}
#endif

	row -= W_WINROW(curwin);
	col -= curwin->w_wincol;

	// When clicking beyond the end of the window, scroll the screen.
	// Scroll by however many rows outside the window we are.
	যদি (row < 0)
	{
	    গুনতি = 0;
	    বাবে (first = TRUE; curwin->w_topline > 1; )
	    {
#ifdef FEAT_DIFF
		যদি (curwin->w_topfill < diff_check_fill(curwin, curwin->w_topline))
		    ++গুনতি;
		অন্যথা
#endif
		    গুনতি += plines(curwin->w_topline - 1);
		যদি (!first && গুনতি > -row)
		    ভঙ্গ;
		first = FALSE;
#ifdef FEAT_FOLDING
		(শূন্য)hasFolding(curwin->w_topline, &curwin->w_topline, রিক্ত);
#endif
#ifdef FEAT_DIFF
		যদি (curwin->w_topfill < diff_check_fill(curwin, curwin->w_topline))
		    ++curwin->w_topfill;
		অন্যথা
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
	অন্যথা যদি (row >= curwin->w_height)
	{
	    গুনতি = 0;
	    বাবে (first = TRUE; curwin->w_topline < curbuf->b_ml.ml_সাৰি_গুনতি; )
	    {
#ifdef FEAT_DIFF
		যদি (curwin->w_topfill > 0)
		    ++গুনতি;
		অন্যথা
#endif
		    গুনতি += plines(curwin->w_topline);
		যদি (!first && গুনতি > row - curwin->w_height + 1)
		    ভঙ্গ;
		first = FALSE;
#ifdef FEAT_FOLDING
		যদি (hasFolding(curwin->w_topline, রিক্ত, &curwin->w_topline)
			&& curwin->w_topline == curbuf->b_ml.ml_সাৰি_গুনতি)
		    ভঙ্গ;
#endif
#ifdef FEAT_DIFF
		যদি (curwin->w_topfill > 0)
		    --curwin->w_topfill;
		অন্যথা
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
	অন্যথা যদি (row == 0)
	{
	    // When dragging the mouse, while the text has been scrolled up as
	    // far as it goes, moving the mouse in the top line should scroll
	    // the text down (done later when recomputing w_topline).
	    যদি (mouse_dragging > 0
		    && curwin->w_cursor.lnum
				       == curwin->w_buffer->b_ml.ml_সাৰি_গুনতি
		    && curwin->w_cursor.lnum == curwin->w_topline)
		curwin->w_valid &= ~(VALID_TOPLINE);
	}
    }

    যদি (prev_row >= W_WINROW(curwin)
	&& prev_row < W_WINROW(curwin) + curwin->w_height
	&& prev_col >= curwin->w_wincol && prev_col < W_ENDCOL(curwin)
						       && Screen_সাৰিগুলি != রিক্ত)
    {
	পূর্ণাঙ্ক off = সাৰি_Offset[prev_row] + prev_col;

	// Only use ScreenCols[] after the window was redrawn.  Mainly matters
	// for tests, a user would not click before redrawing.
	// Do not use when 'virtualedit' is active.
	যদি (curwin->w_redr_type <= UPD_VALID_NO_UPDATE)
	    col_from_screen = ScreenCols[off];
#ifdef FEAT_FOLDING
	// Remember the character under the mouse, it might be a '-' or '+' in
	// the fold column.
	mouse_char = enc_utf8 && Screen_সাৰিগুলি_UC[off] != 0
				       ? Screen_সাৰিগুলি_UC[off] : Screen_সাৰিগুলি[off];
#endif
    }

#ifdef FEAT_FOLDING
    // Check for position outside of the fold column.
    যদি (
# ifdef FEAT_RIGHTLEFT
	    curwin->w_p_rl ? col < curwin->w_width - curwin->w_p_fdc :
# endif
	    col >= curwin->w_p_fdc + (cmdwin_win != curwin ? 0 : 1)
       )
	mouse_char = ' ';
#endif

    // compute the position in the buffer line from the posn on the screen
    যদি (mouse_comp_pos(curwin, &row, &col, &curwin->w_cursor.lnum, রিক্ত))
	mouse_past_bottom = TRUE;

    // Start Visual mode before coladvance(), for when 'sel' != "old"
    যদি ((flags & MOUSE_MAY_VIS) && !VIsual_active)
    {
	check_visual_highlight();
	VIsual = old_cursor;
	VIsual_active = TRUE;
	VIsual_reselect = TRUE;
	// if 'selectmode' contains "mouse", start Select mode
	may_start_select('o');
	setmouse();
	যদি (p_smd && বাৰ্তা_silent == 0)
	    redraw_cmdline = TRUE;	// show visual mode later
    }

    যদি (col_from_screen >= 0)
    {
	// Use the virtual column from ScreenCols[], it is accurate also after
	// concealed characters.
	col = col_from_screen;
    }

    curwin->w_curswant = col;
    curwin->w_set_curswant = false;	// May still have been TRUE
    যদি (coladvance(col) == FAIL)	// Mouse click beyond end of line
    {
	যদি (inclusive != রিক্ত)
	    *inclusive = TRUE;
	mouse_past_eol = TRUE;
    }
    অন্যথা যদি (inclusive != রিক্ত)
	*inclusive = FALSE;

    গুনতি = IN_BUFFER;
    যদি (curwin != old_curwin || curwin->w_cursor.lnum != old_cursor.lnum
	    || curwin->w_cursor.col != old_cursor.col)
	গুনতি |= CURSOR_MOVED;		// Cursor has moved

#ifdef FEAT_FOLDING
    যদি (mouse_char == curwin->w_fill_chars.foldclosed)
	গুনতি |= MOUSE_FOLD_OPEN;
    অন্যথা যদি (mouse_char != ' ')
	গুনতি |= MOUSE_FOLD_CLOSE;
#endif

    ঘূৰাই গুনতি;
}

/*
 * Make a horizontal scroll to "leftcol".
 * Return TRUE if the cursor moved, FALSE otherwise.
 */
    পূর্ণাঙ্ক
do_mousescroll_horiz(long_u leftcol)
{
    যদি (curwin->w_p_wrap)
	ঘূৰাই FALSE;  // no horizontal scrolling when wrapping

    যদি (curwin->w_leftcol == (colnr_T)leftcol)
	ঘূৰাই FALSE;  // already there

    // When the line of the cursor is too short, move the cursor to the
    // longest visible line.
    যদি (
#ifdef FEAT_GUI
	    (!gui.in_use || vim_strchr(p_go, GO_HORSCROLL) == রিক্ত) &&
#endif
		    !virtual_active()
	    && (long)leftcol > scroll_সাৰি_len(curwin->w_cursor.lnum))
    {
	curwin->w_cursor.lnum = ui_find_longest_lnum();
	curwin->w_cursor.col = 0;
    }

    ঘূৰাই set_leftcol((colnr_T)leftcol);
}

/*
 * Normal and Visual modes implementation for scrolling in direction
 * "cap->arg", which is one of the MSCR_ values.
 */
    শূন্য
nv_mousescroll(cmdarg_T *cap)
{
    win_T   *old_curwin = curwin;

#ifdef FEAT_TABPANEL
    যদি (mouse_row >= 0 && mouse_col >= 0
	    && (cap->arg == MSCR_UP || cap->arg == MSCR_DOWN)
	    && mouse_on_tabpanel())
    {
	(শূন্য)tabpanel_scroll(cap->arg == MSCR_UP ? 1 : -1,
		mouse_vert_step > 0 ? mouse_vert_step : 3);
	ঘূৰাই;
    }
#endif

    যদি (mouse_row >= 0 && mouse_col >= 0)
    {
	// Find the window at the mouse pointer coordinates.
	// NOTE: Must restore "curwin" to "old_curwin" before returning!
	পূর্ণাঙ্ক row = mouse_row;
	পূর্ণাঙ্ক col = mouse_col;
	curwin = mouse_find_win(&row, &col, FIND_POPUP);
	যদি (curwin == রিক্ত)
	{
	    curwin = old_curwin;
	    ঘূৰাই;
	}

#ifdef FEAT_PROP_POPUP
	যদি (WIN_IS_POPUP(curwin) && !curwin->w_has_scrollbar)
	{
	    // cannot scroll this popup window
	    curwin = old_curwin;
	    ঘূৰাই;
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
    শূন্য
nv_mouse(cmdarg_T *cap)
{
    (শূন্য)do_mouse(cap->oap, cap->cmdchar, BACKWARD, cap->count1, 0);
}

স্থির পূর্ণাঙ্ক	held_button = MOUSE_RELEASE;

    শূন্য
reset_held_button(শূন্য)
{
    held_button = MOUSE_RELEASE;
}

/*
 * Check if typebuf 'tp' contains a terminal mouse code and returns the
 * modifiers found in typebuf in 'modifiers'.
 */
    পূর্ণাঙ্ক
check_termcode_mouse(
    char_u	*tp,
    পূর্ণাঙ্ক		*slen,
    char_u	*চাবি_name,
    char_u	*modifiers_start,
    পূর্ণাঙ্ক		idx,
    পূর্ণাঙ্ক		*modifiers)
{
    পূর্ণাঙ্ক		j;
    char_u	*p;
#যদি !defined(UNIX) || defined(FEAT_MOUSE_XTERM) || defined(FEAT_GUI) \
    || defined(FEAT_MOUSE_GPM) || defined(FEAT_SYSMOUSE)
    char_u	bytes[6];
    পূর্ণাঙ্ক		অঙ্ক_bytes;
#endif
    পূর্ণাঙ্ক		mouse_code = 0;	    // init for GCC
    পূর্ণাঙ্ক		is_click, is_drag;
    পূর্ণাঙ্ক		is_release, release_is_ambiguous;
    পূর্ণাঙ্ক		wheel_code = 0;
    পূর্ণাঙ্ক		current_button;
    স্থির পূর্ণাঙ্ক	orig_অঙ্ক_clicks = 1;
    স্থির পূর্ণাঙ্ক	orig_mouse_code = 0x0;
#ifdef CHECK_DOUBLE_CLICK
    স্থির পূর্ণাঙ্ক	orig_mouse_col = 0;
    স্থির পূর্ণাঙ্ক	orig_mouse_row = 0;
    স্থির সংরচনা timeval  orig_mouse_time = {0, 0};
    // time of previous mouse click
    সংরচনা timeval  mouse_time;		// time of current mouse click
    long	timediff;		// elapsed time in msec
#endif

    is_click = is_drag = is_release = release_is_ambiguous = FALSE;

#যদি !defined(UNIX) || defined(FEAT_MOUSE_XTERM) || defined(FEAT_GUI) \
    || defined(FEAT_MOUSE_GPM) || defined(FEAT_SYSMOUSE)
    যদি (চাবি_name[0] == KS_MOUSE
# ifdef FEAT_MOUSE_GPM
	    || চাবি_name[0] == KS_GPM_MOUSE
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
	বাবে (;;)
	{
	    // For the GUI and for MS-Windows two bytes each are used for row
	    // and column.  Allows for more than 223 columns.
# যদি defined(FEAT_GUI) || defined(MSWIN)
	    যদি (TRUE
#  যদি defined(FEAT_GUI) && !defined(MSWIN)
		&& gui.in_use
#  endif
		)
	    {
		অঙ্ক_bytes = get_bytes_from_buf(tp + *slen, bytes, 5);
		যদি (অঙ্ক_bytes == -1)	// not enough coordinates
		    ঘূৰাই -1;
		mouse_code = bytes[0];
		mouse_col = 128 * (bytes[1] - ' ' - 1)
		    + bytes[2] - ' ' - 1;
		mouse_row = 128 * (bytes[3] - ' ' - 1)
		    + bytes[4] - ' ' - 1;
	    }
	    অন্যথা
# endif
	    {
		অঙ্ক_bytes = get_bytes_from_buf(tp + *slen, bytes, 3);
		যদি (অঙ্ক_bytes == -1)	// not enough coordinates
		    ঘূৰাই -1;
		mouse_code = bytes[0];
		mouse_col = bytes[1] - ' ' - 1;
		mouse_row = bytes[2] - ' ' - 1;
	    }
	    *slen += অঙ্ক_bytes;

	    // If the following bytes is also a mouse code and it has the same
	    // code, dump this one and get the next.  This makes dragging a
	    // whole lot faster.
# ifdef FEAT_GUI
	    যদি (gui.in_use)
		j = 3;
	    অন্যথা
# endif
		j = get_termcode_len(idx);
	    যদি (STRNCMP(tp, tp + *slen, (size_t)j) == 0
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
	    অন্যথা
		ভঙ্গ;
	}
    }

    যদি (চাবি_name[0] == KS_URXVT_MOUSE
	    || চাবি_name[0] == KS_SGR_MOUSE
	    || চাবি_name[0] == KS_SGR_MOUSE_RELEASE)
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
	যদি (p == রিক্ত)
	    ঘূৰাই -1;

	mouse_code = getdigits(&p);
	যদি (*p++ != ';')
	    ঘূৰাই -1;

	// when mouse reporting is SGR, add 32 to mouse code
	যদি (চাবি_name[0] == KS_SGR_MOUSE
		|| চাবি_name[0] == KS_SGR_MOUSE_RELEASE)
	    mouse_code += 32;

	mouse_col = getdigits(&p) - 1;
	যদি (*p++ != ';')
	    ঘূৰাই -1;

	mouse_row = getdigits(&p) - 1;

	// The modifiers were the mouse coordinates, not the modifier keys
	// (alt/shift/ctrl/meta) state.
	*modifiers = 0;
    }

    যদি (চাবি_name[0] == KS_SGR_MOUSE
	    || চাবি_name[0] == KS_SGR_MOUSE_RELEASE)
    {
	যদি (চাবি_name[0] == KS_SGR_MOUSE_RELEASE)
	{
	    is_release = TRUE;
	    // This is used below to set held_button.
	    mouse_code |= MOUSE_RELEASE;
	}
    }
    অন্যথা
    {
	release_is_ambiguous = TRUE;
	যদি ((mouse_code & MOUSE_RELEASE) == MOUSE_RELEASE)
	    is_release = TRUE;
    }

    যদি (চাবি_name[0] == KS_MOUSE
# ifdef FEAT_MOUSE_GPM
	    || চাবি_name[0] == KS_GPM_MOUSE
# endif
# ifdef FEAT_MOUSE_URXVT
	    || চাবি_name[0] == KS_URXVT_MOUSE
# endif
	    || চাবি_name[0] == KS_SGR_MOUSE
	    || চাবি_name[0] == KS_SGR_MOUSE_RELEASE)
    {
# যদি !defined(MSWIN)
	/*
	 * Handle old style mouse events.
	 * Recognize the xterm mouse wheel, but not in the GUI, the
	 * Linux console with GPM and the MS-DOS or Win32 console
	 * (multi-clicks use >= 0x60).
	 */
	যদি (mouse_code >= MOUSEWHEEL_নিচেৰ && mouse_code < MOUSESIDEBUTTONS_নিচেৰ
#  ifdef FEAT_GUI
		&& !gui.in_use
#  endif
#  ifdef FEAT_MOUSE_GPM
		&& চাবি_name[0] != KS_GPM_MOUSE
#  endif
	   )
	{
#  যদি defined(UNIX)
	    যদি (use_xterm_mouse() > 1 && mouse_code >= 0x80)
		// mouse-move event, using MOUSE_DRAG works
		mouse_code = MOUSE_DRAG;
	    অন্যথা
#  endif
		// Keep the mouse_code before it's changed, so that we
		// remember that it was a mouse wheel click.
		wheel_code = mouse_code;
	}
#  ifdef FEAT_MOUSE_XTERM
	অন্যথা যদি (held_button == MOUSE_RELEASE
#   ifdef FEAT_GUI
		&& !gui.in_use
#   endif
		&& (mouse_code == 0x23 || mouse_code == 0x24
		    || mouse_code == 0x40 || mouse_code == 0x41))
	{
	    // Apparently 0x23 and 0x24 are used by rxvt scroll wheel.
	    // And 0x40 and 0x41 are used by some xterm emulator.
	    wheel_code = mouse_code - (mouse_code >= 0x40 ? 0x40 : 0x23)
							      + MOUSEWHEEL_নিচেৰ;
	}
#  endif

#  যদি defined(UNIX)
	অন্যথা যদি (use_xterm_mouse() > 1)
	{
	    যদি (mouse_code & MOUSE_DRAG_XTERM)
		mouse_code |= MOUSE_DRAG;
	}
#  endif
#  ifdef FEAT_XCLIPBOARD
	অন্যথা যদি (!(mouse_code & MOUSE_DRAG & ~MOUSE_CLICK_MASK))
	{
	    যদি (is_release)
		stop_xterm_trace();
	    অন্যথা
		start_xterm_trace(mouse_code);
	}
#  endif
# endif
    }
#endif // !UNIX || FEAT_MOUSE_XTERM
#ifdef FEAT_MOUSE_NET
    যদি (চাবি_name[0] == KS_NETTERM_MOUSE)
    {
	পূর্ণাঙ্ক mc, mr;

	// expect a rather limited sequence like: balancing {
	// \033}6,45\r
	// '6' is the row, 45 is the column
	p = tp + *slen;
	mr = getdigits(&p);
	যদি (*p++ != ',')
	    ঘূৰাই -1;
	mc = getdigits(&p);
	যদি (*p++ != '\r')
	    ঘূৰাই -1;

	mouse_col = mc - 1;
	mouse_row = mr - 1;
	mouse_code = MOUSE_বাম;
	*slen += (পূর্ণাঙ্ক)(p - (tp + *slen));
    }
#endif	// FEAT_MOUSE_NET
#ifdef FEAT_MOUSE_JSB
    যদি (চাবি_name[0] == KS_JSBTERM_MOUSE)
    {
	পূর্ণাঙ্ক mult, val, iter, button, status;

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
	ৱিকল্প (*p++)
	{
	    পক্ষ 'L': button = 1; ভঙ্গ;
	    পক্ষ '-': ভঙ্গ;
	    পক্ষ 'x': ভঙ্গ; // ignore sequence
	    স্ৱভাৱ:  ঘূৰাই -1; // Unknown Result
	}
	ৱিকল্প (*p++)
	{
	    পক্ষ 'M': button |= 2; ভঙ্গ;
	    পক্ষ '-': ভঙ্গ;
	    পক্ষ 'x': ভঙ্গ; // ignore sequence
	    স্ৱভাৱ:  ঘূৰাই -1; // Unknown Result
	}
	ৱিকল্প (*p++)
	{
	    পক্ষ 'R': button |= 4; ভঙ্গ;
	    পক্ষ '-': ভঙ্গ;
	    পক্ষ 'x': ভঙ্গ; // ignore sequence
	    স্ৱভাৱ:  ঘূৰাই -1; // Unknown Result
	}
	status = *p++;
	বাবে (val = 0, mult = 100, iter = 0; iter < 3; iter++,
		mult /= 10, p++)
	    যদি (*p >= '0' && *p <= '9')
		val += (*p - '0') * mult;
	    অন্যথা
		ঘূৰাই -1;
	mouse_col = val;
	বাবে (val = 0, mult = 100, iter = 0; iter < 3; iter++,
		mult /= 10, p++)
	    যদি (*p >= '0' && *p <= '9')
		val += (*p - '0') * mult;
	    অন্যথা
		ঘূৰাই -1;
	mouse_row = val;
	ৱিকল্প (*p++)
	{
	    পক্ষ 's': button |= 8; ভঙ্গ;  // SHIFT key Pressed
	    পক্ষ '-': ভঙ্গ;  // Not Pressed
	    পক্ষ 'x': ভঙ্গ;  // Not Reporting
	    স্ৱভাৱ:  ঘূৰাই -1; // Unknown Result
	}
	ৱিকল্প (*p++)
	{
	    পক্ষ 'c': button |= 16; ভঙ্গ;  // CTRL key Pressed
	    পক্ষ '-': ভঙ্গ;  // Not Pressed
	    পক্ষ 'x': ভঙ্গ;  // Not Reporting
	    স্ৱভাৱ:  ঘূৰাই -1; // Unknown Result
	}
	যদি (*p++ != '\033')
	    ঘূৰাই -1;
	যদি (*p++ != '\\')
	    ঘূৰাই -1;
	ৱিকল্প (status)
	{
	    পক্ষ 'D': // Double Click
	    পক্ষ 'S': // Single Click
		যদি (button & 1) mouse_code |= MOUSE_বাম;
		যদি (button & 2) mouse_code |= MOUSE_মধ্য;
		যদি (button & 4) mouse_code |= MOUSE_ডান;
		যদি (button & 8) mouse_code |= MOUSE_SHIFT;
		যদি (button & 16) mouse_code |= MOUSE_CTRL;
		ভঙ্গ;
	    পক্ষ 'm': // Mouse move
		যদি (button & 1) mouse_code |= MOUSE_বাম;
		যদি (button & 2) mouse_code |= MOUSE_মধ্য;
		যদি (button & 4) mouse_code |= MOUSE_ডান;
		যদি (button & 8) mouse_code |= MOUSE_SHIFT;
		যদি (button & 16) mouse_code |= MOUSE_CTRL;
		যদি ((button & 7) != 0)
		{
		    held_button = mouse_code;
		    mouse_code |= MOUSE_DRAG;
		}
		is_drag = TRUE;
		showmode();
		ভঙ্গ;
	    পক্ষ 'd': // Button Down
		যদি (button & 1) mouse_code |= MOUSE_বাম;
		যদি (button & 2) mouse_code |= MOUSE_মধ্য;
		যদি (button & 4) mouse_code |= MOUSE_ডান;
		যদি (button & 8) mouse_code |= MOUSE_SHIFT;
		যদি (button & 16) mouse_code |= MOUSE_CTRL;
		ভঙ্গ;
	    পক্ষ 'u': // Button Up
		is_release = TRUE;
		যদি (button & 1)
		    mouse_code |= MOUSE_বাম;
		যদি (button & 2)
		    mouse_code |= MOUSE_মধ্য;
		যদি (button & 4)
		    mouse_code |= MOUSE_ডান;
		যদি (button & 8)
		    mouse_code |= MOUSE_SHIFT;
		যদি (button & 16)
		    mouse_code |= MOUSE_CTRL;
		ভঙ্গ;
	    স্ৱভাৱ: ঘূৰাই -1; // Unknown Result
	}

	*slen += (p - (tp + *slen));
    }
#endif // FEAT_MOUSE_JSB
#ifdef FEAT_MOUSE_DEC
    যদি (চাবি_name[0] == KS_DEC_MOUSE)
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
	পূর্ণাঙ্ক Pe, Pb, Pr, Pc;

	p = tp + *slen;

	// get event status
	Pe = getdigits(&p);
	যদি (*p++ != ';')
	    ঘূৰাই -1;

	// get button status
	Pb = getdigits(&p);
	যদি (*p++ != ';')
	    ঘূৰাই -1;

	// get row status
	Pr = getdigits(&p);
	যদি (*p++ != ';')
	    ঘূৰাই -1;

	// get column status
	Pc = getdigits(&p);

	// the page parameter is optional
	যদি (*p == ';')
	{
	    p++;
	    (শূন্য)getdigits(&p);
	}
	যদি (*p++ != '&')
	    ঘূৰাই -1;
	যদি (*p++ != 'w')
	    ঘূৰাই -1;

	mouse_code = 0;
	ৱিকল্প (Pe)
	{
	    পক্ষ  0: ঘূৰাই -1; // position request while unavailable
	    পক্ষ  1: // a response to a locator position request includes
		     //	the status of all buttons
		     Pb &= 7;   // mask off and ignore fourth button
		     যদি (Pb & 4)
			 mouse_code  = MOUSE_বাম;
		     যদি (Pb & 2)
			 mouse_code  = MOUSE_মধ্য;
		     যদি (Pb & 1)
			 mouse_code  = MOUSE_ডান;
		     যদি (Pb)
		     {
			 held_button = mouse_code;
			 mouse_code |= MOUSE_DRAG;
			 WantQueryMouse = TRUE;
		     }
		     is_drag = TRUE;
		     showmode();
		     ভঙ্গ;
	    পক্ষ  2: mouse_code = MOUSE_বাম;
		     WantQueryMouse = TRUE;
		     ভঙ্গ;
	    পক্ষ  3: mouse_code = MOUSE_বাম;
		     is_release = TRUE;
		     ভঙ্গ;
	    পক্ষ  4: mouse_code = MOUSE_মধ্য;
		     WantQueryMouse = TRUE;
		     ভঙ্গ;
	    পক্ষ  5: mouse_code = MOUSE_মধ্য;
		     is_release = TRUE;
		     ভঙ্গ;
	    পক্ষ  6: mouse_code = MOUSE_ডান;
		     WantQueryMouse = TRUE;
		     ভঙ্গ;
	    পক্ষ  7: mouse_code = MOUSE_ডান;
		     is_release = TRUE;
		     ভঙ্গ;
	    পক্ষ  8: ঘূৰাই -1; // fourth button down
	    পক্ষ  9: ঘূৰাই -1; // fourth button up
	    পক্ষ 10: ঘূৰাই -1; // mouse outside of filter rectangle
	    স্ৱভাৱ: ঘূৰাই -1; // should never occur
	}

	mouse_col = Pc - 1;
	mouse_row = Pr - 1;

	*slen += (পূর্ণাঙ্ক)(p - (tp + *slen));
    }
#endif // FEAT_MOUSE_DEC
#ifdef FEAT_MOUSE_PTERM
    যদি (চাবি_name[0] == KS_PTERM_MOUSE)
    {
	পূর্ণাঙ্ক button, অঙ্ক_clicks, action;

	p = tp + *slen;

	action = getdigits(&p);
	যদি (*p++ != ';')
	    ঘূৰাই -1;

	mouse_row = getdigits(&p);
	যদি (*p++ != ';')
	    ঘূৰাই -1;
	mouse_col = getdigits(&p);
	যদি (*p++ != ';')
	    ঘূৰাই -1;

	button = getdigits(&p);
	mouse_code = 0;

	ৱিকল্প (button)
	{
	    পক্ষ 4: mouse_code = MOUSE_বাম; ভঙ্গ;
	    পক্ষ 1: mouse_code = MOUSE_ডান; ভঙ্গ;
	    পক্ষ 2: mouse_code = MOUSE_মধ্য; ভঙ্গ;
	    স্ৱভাৱ: ঘূৰাই -1;
	}

	ৱিকল্প (action)
	{
	    পক্ষ 31: // Initial press
		যদি (*p++ != ';')
		    ঘূৰাই -1;

		অঙ্ক_clicks = getdigits(&p); // Not used
		ভঙ্গ;

	    পক্ষ 32: // Release
		is_release = TRUE;
		ভঙ্গ;

	    পক্ষ 33: // Drag
		held_button = mouse_code;
		mouse_code |= MOUSE_DRAG;
		ভঙ্গ;

	    স্ৱভাৱ:
		ঘূৰাই -1;
	}

	যদি (*p++ != 't')
	    ঘূৰাই -1;

	*slen += (p - (tp + *slen));
    }
#endif // FEAT_MOUSE_PTERM

    // Interpret the mouse code
    current_button = (mouse_code & MOUSE_CLICK_MASK);
    যদি (is_release)
	current_button |= MOUSE_RELEASE;

    যদি (current_button == MOUSE_RELEASE
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
	যদি ((mouse_code & MOUSE_DRAG) == MOUSE_DRAG)
	    is_drag = TRUE;
	current_button = held_button;
    }
    অন্যথা
    {
      যদি (wheel_code == 0)
      {
#ifdef CHECK_DOUBLE_CLICK
# ifdef FEAT_MOUSE_GPM
	/*
	 * Only for Unix, when GUI not active, we handle multi-clicks here, but
	 * not for GPM mouse events.
	 */
#  ifdef FEAT_GUI
	যদি (চাবি_name[0] != KS_GPM_MOUSE && !gui.in_use)
#  অন্যথা
	    যদি (চাবি_name[0] != KS_GPM_MOUSE)
#  endif
# অন্যথা
#  ifdef FEAT_GUI
		যদি (!gui.in_use)
#  endif
# endif
		{
		    /*
		     * Compute the time elapsed since the previous mouse click.
		     */
		    gettimeofday(&mouse_time, রিক্ত);
		    যদি (orig_mouse_time.tv_sec == 0)
		    {
			/*
			 * Avoid computing the difference between mouse_time
			 * and orig_mouse_time for the first click, as the
			 * difference would be huge and would cause
			 * multiplication overflow.
			 */
			timediff = p_mouset;
		    }
		    অন্যথা
			timediff = time_diff_ms(&orig_mouse_time, &mouse_time);
		    orig_mouse_time = mouse_time;
		    যদি (mouse_code == orig_mouse_code
			    && timediff < p_mouset
			    && orig_অঙ্ক_clicks != 4
			    && orig_mouse_col == mouse_col
			    && orig_mouse_row == mouse_row
			    && (is_mouse_topline(curwin)
				// Double click in tab pages line also works
				// when window contents changes.
				|| (mouse_row == 0 && firstwin->w_winrow > 0))
		       )
			++orig_অঙ্ক_clicks;
		    অন্যথা
			orig_অঙ্ক_clicks = 1;
		    orig_mouse_col = mouse_col;
		    orig_mouse_row = mouse_row;
		    set_mouse_topline(curwin);
		}
# যদি defined(FEAT_GUI) || defined(FEAT_MOUSE_GPM)
		অন্যথা
		    orig_অঙ্ক_clicks = অঙ্ক_MOUSE_CLICKS(mouse_code);
# endif
#অন্যথা
	orig_অঙ্ক_clicks = অঙ্ক_MOUSE_CLICKS(mouse_code);
#endif
	is_click = TRUE;
      }
      orig_mouse_code = mouse_code;
    }
    যদি (!is_drag)
	held_button = mouse_code & MOUSE_CLICK_MASK;

    /*
     * Translate the actual mouse event into a pseudo mouse event.
     * First work out what modifiers are to be used.
     */
    যদি (orig_mouse_code & MOUSE_SHIFT)
	*modifiers |= MOD_MASK_SHIFT;
    যদি (orig_mouse_code & MOUSE_CTRL)
	*modifiers |= MOD_MASK_CTRL;
    যদি (orig_mouse_code & MOUSE_ALT)
	*modifiers |= MOD_MASK_ALT;
    যদি (orig_অঙ্ক_clicks == 2)
	*modifiers |= MOD_MASK_2CLICK;
    অন্যথা যদি (orig_অঙ্ক_clicks == 3)
	*modifiers |= MOD_MASK_3CLICK;
    অন্যথা যদি (orig_অঙ্ক_clicks == 4)
	*modifiers |= MOD_MASK_4CLICK;

    // Work out our pseudo mouse event. Note that MOUSE_RELEASE gets added,
    // then it's not mouse up/down.
    চাবি_name[0] = KS_EXTRA;
    যদি (wheel_code != 0 && (!is_release || release_is_ambiguous))
    {
	যদি (wheel_code & MOUSE_CTRL)
	    *modifiers |= MOD_MASK_CTRL;
	যদি (wheel_code & MOUSE_ALT)
	    *modifiers |= MOD_MASK_ALT;

	যদি (wheel_code & 1 && wheel_code & 2)
	    চাবি_name[1] = (পূর্ণাঙ্ক)KE_MOUSELEFT;
	অন্যথা যদি (wheel_code & 2)
	    চাবি_name[1] = (পূর্ণাঙ্ক)KE_MOUSERIGHT;
	অন্যথা যদি (wheel_code & 1)
	    চাবি_name[1] = (পূর্ণাঙ্ক)KE_MOUSEUP;
	অন্যথা
	    চাবি_name[1] = (পূর্ণাঙ্ক)KE_MOUSEDOWN;

	held_button = MOUSE_RELEASE;
    }
    অন্যথা
    {
#যদি defined(UNIX)
	যদি (use_xterm_mouse() && orig_mouse_code >= MOUSESIDEBUTTONS_নিচেৰ)
	    current_button = (current_button) ? MOUSE_X2 : MOUSE_X1;
#endif
	চাবি_name[1] = get_pseudo_mouse_code(current_button, is_click, is_drag);
    }


    // Make sure the mouse position is valid.  Some terminals may return weird
    // values.
    যদি (mouse_col >= Columns)
	mouse_col = Columns - 1;
    যদি (mouse_row >= Rows)
	mouse_row = Rows - 1;

    ঘূৰাই 0;
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
    পূর্ণাঙ্ক
mouse_comp_pos(
    win_T	*win,
    পূর্ণাঙ্ক		*rowp,
    পূর্ণাঙ্ক		*colp,
    linenr_T	*lnump,
    পূর্ণাঙ্ক		*plines_cache)
{
    পূর্ণাঙ্ক		col = *colp;
    পূর্ণাঙ্ক		row = *rowp;
    linenr_T	lnum;
    পূর্ণাঙ্ক		retval = FALSE;
    পূর্ণাঙ্ক		off;
    পূর্ণাঙ্ক		গুনতি;

#ifdef FEAT_RIGHTLEFT
    যদি (win->w_p_rl)
	col = win->w_width - 1 - col;
#endif

    lnum = win->w_topline;

    যেতিয়ালৈ (row > 0)
    {
	পূর্ণাঙ্ক cache_idx = lnum - win->w_topline;

	// Only "Rows" lines are cached, with folding we'll run out of entries
	// and use the slow way.
	যদি (plines_cache != রিক্ত && cache_idx < Rows
						&& plines_cache[cache_idx] > 0)
	    গুনতি = plines_cache[cache_idx];
	অন্যথা
	{
#ifdef FEAT_DIFF
	    // Don't include filler lines in "count"
	    যদি (win->w_p_diff
# ifdef FEAT_FOLDING
		    && !hasFoldingWin(win, lnum, রিক্ত, রিক্ত, TRUE, রিক্ত)
# endif
		    )
	    {
		যদি (lnum == win->w_topline)
		    row -= win->w_topfill;
		অন্যথা
		    row -= diff_check_fill(win, lnum);
		গুনতি = plines_win_nofill(win, lnum, FALSE);
	    }
	    অন্যথা
#endif
		গুনতি = plines_win(win, lnum, FALSE);
	    যদি (plines_cache != রিক্ত && cache_idx < Rows)
		plines_cache[cache_idx] = গুনতি;
	}

	যদি (win->w_skipcol > 0 && lnum == win->w_topline)
	{
	    পূর্ণাঙ্ক width1 = win->w_width - win_col_off(win);

	    যদি (width1 > 0)
	    {
		পূর্ণাঙ্ক skip_সাৰিগুলি = 0;

		// Adjust for 'smoothscroll' clipping the top screen lines.
		// A similar formula is used in curs_columns().
		যদি (win->w_skipcol > width1)
		    skip_সাৰিগুলি = (win->w_skipcol - width1)
					    / (width1 + win_col_off2(win)) + 1;
		অন্যথা যদি (win->w_skipcol > 0)
		    skip_সাৰিগুলি = 1;

		গুনতি -= skip_সাৰিগুলি;
	    }
	}

	যদি (গুনতি > row)
	    ভঙ্গ;	// Position is in this buffer line.
#ifdef FEAT_FOLDING
	(শূন্য)hasFoldingWin(win, lnum, রিক্ত, &lnum, TRUE, রিক্ত);
#endif
	যদি (lnum == win->w_buffer->b_ml.ml_সাৰি_গুনতি)
	{
	    retval = TRUE;
	    ভঙ্গ;		// past end of file
	}
	row -= গুনতি;
	++lnum;
    }

    যদি (!retval)
    {
	// Compute the column without wrapping.
	off = win_col_off(win) - win_col_off2(win);
	যদি (col < off)
	    col = off;
	col += row * (win->w_width - off);

	// Add skip column for the topline.
	যদি (lnum == win->w_topline)
	    col += win->w_skipcol;
    }

    যদি (!win->w_p_wrap)
	col += win->w_leftcol;

    // skip line number and fold column in front of the line
    col -= win_col_off(win);
    যদি (col <= 0)
    {
#ifdef FEAT_NETBEANS_INTG
	// if mouse is clicked on the gutter, then inform the netbeans server
	যদি (*colp < win_col_off(win))
	    netbeans_gutter_click(lnum);
#endif
	col = 0;
    }

    *colp = col;
    *rowp = row;
    *lnump = lnum;
    ঘূৰাই retval;
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
mouse_find_win(পূর্ণাঙ্ক *rowp, পূর্ণাঙ্ক *colp, mouse_find_T popup UNUSED)
{
    frame_T	*fp;
    win_T	*wp;

#ifdef FEAT_PROP_POPUP
    win_T	*pwp = রিক্ত;

    যদি (popup != IGNORE_POPUP)
    {
	popup_reset_handled(POPUP_HANDLED_1);
	যেতিয়ালৈ ((wp = find_next_popup(TRUE, POPUP_HANDLED_1)) != রিক্ত)
	{
	    যদি (*rowp >= wp->w_winrow && *rowp < wp->w_winrow + popup_height(wp)
		    && *colp >= wp->w_wincol
				    && *colp < wp->w_wincol + popup_width(wp))
		pwp = wp;
	}
	যদি (pwp != রিক্ত)
	{
	    যদি (popup == FAIL_POPUP)
		ঘূৰাই রিক্ত;
	    *rowp -= pwp->w_winrow;
	    *colp -= pwp->w_wincol;
	    ঘূৰাই pwp;
	}
    }
#endif

    fp = topframe;

    যদি (*colp < firstwin->w_wincol
	    || *colp >= firstwin->w_wincol + fp->fr_width
	    || *rowp < firstwin->w_winrow)
	ঘূৰাই রিক্ত;

    *rowp -= firstwin->w_winrow;
    *colp -= firstwin->w_wincol;
    বাবে (;;)
    {
	যদি (fp->fr_layout == FR_LEAF)
	    ভঙ্গ;
	যদি (fp->fr_layout == FR_ROW)
	{
	    বাবে (fp = fp->fr_child; fp->fr_next != রিক্ত; fp = fp->fr_next)
	    {
		যদি (*colp < fp->fr_width)
		    ভঙ্গ;
		*colp -= fp->fr_width;
	    }
	}
	অন্যথা    // fr_layout == FR_COL
	{
	    বাবে (fp = fp->fr_child; fp->fr_next != রিক্ত; fp = fp->fr_next)
	    {
		যদি (*rowp < fp->fr_height)
		    ভঙ্গ;
		*rowp -= fp->fr_height;
	    }
	}
    }
    // When using a timer that closes a window the window might not actually
    // exist.
    FOR_ALL_WINDOWS(wp)
	যদি (wp == fp->fr_win)
	{
#ifdef FEAT_MENU
	    *rowp -= wp->w_winbar_height;
#endif
	    ঘূৰাই wp;
	}
    ঘূৰাই রিক্ত;
}

#যদি defined(NEED_VCOL2COL) || defined(FEAT_BEVAL) || defined(FEAT_PROP_POPUP) \
	|| defined(FEAT_EVAL)
/*
 * Convert a virtual (screen) column to a character column.
 * The first column is zero.
 */
    পূর্ণাঙ্ক
vcol2col(win_T *wp, linenr_T lnum, পূর্ণাঙ্ক vcol, colnr_T *coladdp)
{
    char_u	    *সাৰি;
    chartabsize_T   cts;

    // try to advance to the specified column
    সাৰি = ml_get_buf(wp->w_buffer, lnum, FALSE);
    init_chartabsize_arg(&cts, wp, lnum, 0, সাৰি, সাৰি);
    যেতিয়ালৈ (cts.cts_vcol < vcol && *cts.cts_ptr != NUL)
    {
	পূর্ণাঙ্ক size = win_lbr_chartabsize(&cts, রিক্ত, রিক্ত);
	যদি (cts.cts_vcol + size > vcol)
	    ভঙ্গ;
	cts.cts_vcol += size;
	MB_PTR_ADV(cts.cts_ptr);
    }
    clear_chartabsize_arg(&cts);

    যদি (coladdp != রিক্ত)
	*coladdp = vcol - cts.cts_vcol;
    ঘূৰাই (পূর্ণাঙ্ক)(cts.cts_ptr - সাৰি);
}
#endif

#যদি defined(FEAT_EVAL)
/*
 * "getmousepos()" function.
 */
    শূন্য
f_getmousepos(typval_T *argvars UNUSED, typval_T *rettv)
{
    dict_T	*d;
    win_T	*wp;
    পূর্ণাঙ্ক		row = mouse_row;
    পূর্ণাঙ্ক		col = mouse_col;
    varnumber_T winid = 0;
    varnumber_T winrow = 0;
    varnumber_T wincol = 0;
    linenr_T	lnum = 0;
    varnumber_T column = 0;
    colnr_T	coladd = 0;

    যদি (rettv_dict_alloc(rettv) == FAIL)
	ঘূৰাই;
    d = rettv->vval.v_dict;

    dict_add_সংখ্যা(d, "screenrow", (varnumber_T)mouse_row + 1);
    dict_add_সংখ্যা(d, "screencol", (varnumber_T)mouse_col + 1);

    wp = mouse_find_win(&row, &col, FIND_POPUP);
    যদি (wp != রিক্ত)
    {
	পূর্ণাঙ্ক	শীৰ্ষ_off = 0;
	পূর্ণাঙ্ক	বাম_off = 0;
	পূর্ণাঙ্ক	height = wp->w_height + wp->w_status_height;

# ifdef FEAT_PROP_POPUP
	যদি (WIN_IS_POPUP(wp))
	{
	    শীৰ্ষ_off = popup_শীৰ্ষ_extra(wp);
	    বাম_off = popup_বাম_extra(wp);
	    height = popup_height(wp);
	}
# endif
	যদি (row < height)
	{
	    winid = wp->w_id;
	    winrow = row + 1;
	    wincol = col + 1;
	    row -= শীৰ্ষ_off;
	    col -= বাম_off;
	    যদি (row >= 0 && row < wp->w_height && col >= 0 && col < wp->w_width)
	    {
		(শূন্য)mouse_comp_pos(wp, &row, &col, &lnum, রিক্ত);
		col = vcol2col(wp, lnum, col, &coladd);
		column = col + 1;
	    }
	}
    }
    dict_add_সংখ্যা(d, "winid", winid);
    dict_add_সংখ্যা(d, "winrow", winrow);
    dict_add_সংখ্যা(d, "wincol", wincol);
    dict_add_সংখ্যা(d, "line", (varnumber_T)lnum);
    dict_add_সংখ্যা(d, "column", column);
    dict_add_সংখ্যা(d, "coladd", coladd);
}
#endif
