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

#ضمن "vim.h"

/*
 * Horiziontal and vertical steps used when scrolling.
 * When negative scroll by a whole page.
 */
ساكن long mouse_hor_step = 6;
ساكن long mouse_vert_step = 3;
ساكن win_T *dragwin = عدم;	// window being dragged
ساكن صحيح stl_click_handler(win_T *wp, صحيح mrow, صحيح mcol, صحيح which_button,
								    صحيح mods);
ساكن صحيح stl_click_handler_regions(stl_click_region_T *regions,
				    صحيح region_ژماردن, صحيح winid,
				    char_u *area_name, صحيح mrow, صحيح mcol,
				    صحيح which_button, صحيح mods);

    فراغ
mouse_set_vert_scroll_step(long step)
{
    mouse_vert_step = step;
}

    فراغ
mouse_set_hor_scroll_step(long step)
{
    mouse_hor_step = step;
}

#ifdef CHECK_DOUBLE_CLICK
/*
 * Return the duration from t1 to t2 in milliseconds.
 */
    ساكن long
time_diff_ms(بنية timeval *t1, بنية timeval *t2)
{
    // This handles wrapping of tv_usec correctly without any special case.
    // Example of 2 pairs (tv_sec, tv_usec) with a duration of 5 ms:
    //	   t1 = (1, 998000) t2 = (2, 3000) gives:
    //	   (2 - 1) * 1000 + (3000 - 998000) / 1000 -> 5 ms.
    أرجع (t2->tv_sec - t1->tv_sec) * 1000
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
    ساكن صحيح
get_mouse_class(char_u *p)
{
    صحيح		c;

    إذا (has_mbyte && MB_BYTE2LEN(p[0]) > 1)
	أرجع mb_get_class(p);

    c = *p;
    إذا (c == ' ' || c == '\t')
	أرجع 0;

    إذا (vim_iswordc(c))
	أرجع 2;

    // There are a few special cases where we want certain combinations of
    // characters to be considered as a single word.  These are things like
    // "->", "/ *", "*=", "+=", "&=", "<=", ">=", "!=" etc.  Otherwise, each
    // character is in its own class.
    إذا (c != NUL && vim_strchr((char_u *)"-+*/%<>&|^!=", c) != عدم)
	أرجع 1;
    أرجع c;
}

/*
 * Move "pos" back to the start of the word it's in.
 */
    ساكن فراغ
find_start_of_word(pos_T *pos)
{
    char_u	*هێڵ;
    صحيح		cclass;
    صحيح		col;

    هێڵ = ml_get(pos->lnum);
    cclass = get_mouse_class(هێڵ + pos->col);

    طالما (pos->col > 0)
    {
	col = pos->col - 1;
	col -= (*mb_head_off)(هێڵ, هێڵ + col);
	إذا (get_mouse_class(هێڵ + col) != cclass)
	    اكسر;
	pos->col = col;
    }
}

/*
 * Move "pos" forward to the end of the word it's in.
 * When 'selection' is "exclusive", the position is just after the word.
 */
    ساكن فراغ
find_end_of_word(pos_T *pos)
{
    char_u	*هێڵ;
    صحيح		cclass;
    صحيح		col;

    هێڵ = ml_get(pos->lnum);
    إذا (*p_sel == 'e' && pos->col > 0)
    {
	--pos->col;
	pos->col -= (*mb_head_off)(هێڵ, هێڵ + pos->col);
    }
    cclass = get_mouse_class(هێڵ + pos->col);
    طالما (هێڵ[pos->col] != NUL)
    {
	col = pos->col + (*mb_ptr2len)(هێڵ + pos->col);
	إذا (get_mouse_class(هێڵ + col) != cclass)
	{
	    إذا (*p_sel == 'e')
		pos->col = col;
	    اكسر;
	}
	pos->col = col;
    }
}

#إذا defined(FEAT_GUI_MOTIF) || defined(FEAT_GUI_GTK) \
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
    ساكن صحيح
get_fpos_of_mouse(pos_T *mpos)
{
    win_T	*wp;
    صحيح		row = mouse_row;
    صحيح		col = mouse_col;

    إذا (row < 0 || col < 0)		// check if it makes sense
	أرجع IN_UNKNOWN;

    // find the window where the row is in
    wp = mouse_find_win(&row, &col, FAIL_POPUP);
    إذا (wp == عدم)
	أرجع IN_UNKNOWN;
    // winpos and height may change in win_enter()!
    إذا (row >= wp->w_height)	// In (or below) status line
	أرجع IN_STATUS_هێڵ;
    إذا (col >= wp->w_width)	// In vertical separator line
	أرجع IN_SEP_هێڵ;

    إذا (wp != curwin)
	أرجع IN_UNKNOWN;

    // compute the position in the buffer line from the posn on the screen
    إذا (mouse_comp_pos(curwin, &row, &col, &mpos->lnum, عدم))
	أرجع IN_STATUS_هێڵ; // past bottom

    mpos->col = vcol2col(wp, mpos->lnum, col, &mpos->coladd);
    أرجع IN_BUFFER;
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
    صحيح
do_mouse(
    oparg_T	*oap,		// operator argument, can be NULL
    صحيح		c,		// K_LEFTMOUSE, etc
    صحيح		dir,		// Direction to 'put' if necessary
    long	ژماردن,
    صحيح		fixindent)	// PUT_FIXINDENT if fixing indent necessary
{
    ساكن صحيح	do_always = FALSE;	// ignore 'mouse' setting next time
    ساكن صحيح	got_click = FALSE;	// got a click some time back

    صحيح		which_button;	// MOUSE_LEFT, _MIDDLE or _RIGHT
    صحيح		is_click = FALSE; // If FALSE it's a drag or release event
    صحيح		is_drag = FALSE;  // If TRUE it's a drag event
    صحيح		jump_flags = 0;	// flags for jump_to_mouse()
    pos_T	start_visual;
    صحيح		moved;		// Has cursor moved?
    صحيح		in_status_هێڵ;	// mouse in status line
    ساكن صحيح	in_tab_هێڵ = FALSE; // mouse clicked in tab line
    ساكن صحيح	in_tabpanel = FALSE; // mouse clicked in tabpanel
#ifdef FEAT_TABPANEL
    ساكن bool	in_tabpanel_scrollbar = false; // dragging tabpanel scrollbar
#endif
    صحيح		in_sep_هێڵ;	// mouse in vertical separator line
    صحيح		c1, c2;
#إذا defined(FEAT_FOLDING)
    pos_T	save_cursor;
#endif
    win_T	*old_curwin = curwin;
    ساكن pos_T orig_cursor;
    colnr_T	leftcol, rightcol;
    pos_T	end_visual;
    صحيح		diff;
    صحيح		old_active = VIsual_active;
    صحيح		old_mode = VIsual_mode;
    صحيح		regname;

#إذا defined(FEAT_FOLDING)
    save_cursor = curwin->w_cursor;
#endif

    // When GUI is active, always recognize mouse events, otherwise:
    // - Ignore mouse event in normal mode if 'mouse' doesn't include 'n'.
    // - Ignore mouse event in visual mode if 'mouse' doesn't include 'v'.
    // - For command line and insert mode 'mouse' is checked before calling
    //	 do_mouse().
    إذا (do_always)
	do_always = FALSE;
    وإلا
#ifdef FEAT_GUI
	إذا (!gui.in_use)
#endif
	{
	    إذا (VIsual_active)
	    {
		إذا (!mouse_has(MOUSE_VISUAL))
		    أرجع FALSE;
	    }
	    وإلا إذا (State == MODE_NORMAL && !mouse_has(MOUSE_NORMAL))
		أرجع FALSE;
	}

    لكل (;;)
    {
	which_button = get_mouse_button(KEY2TERMCAP1(c), &is_click, &is_drag);
	إذا (is_drag)
	{
	    // If the next character is the same mouse event then use that
	    // one. Speeds up dragging the status line.
	    // Note: Since characters added to the stuff buffer in the code
	    // below need to come before the next character, do not do this
	    // when the current character was stuffed.
	    إذا (!کلیل_Stuffed && vpeekc() != NUL)
	    {
		صحيح nc;
		صحيح save_mouse_row = mouse_row;
		صحيح save_mouse_col = mouse_col;

		// Need to get the character, peeking doesn't get the actual
		// one.
		nc = safe_vgetc();
		إذا (c == nc)
		    استمر;
		vungetc(nc);
		mouse_row = save_mouse_row;
		mouse_col = save_mouse_col;
	    }
	}
	اكسر;
    }

    إذا (c == K_MOUSEMOVE)
    {
	// Mouse moved without a button pressed.
#ifdef FEAT_BEVAL_TERM
	ui_may_remove_balloon();
	إذا (p_bevalterm)
	{
	    profile_setlimit(p_bdlay, &bevalexpr_due);
	    bevalexpr_due_set = TRUE;
	}
#endif
#ifdef FEAT_PROP_POPUP
	popup_handle_mouse_moved();
#endif
	أرجع FALSE;
    }

#ifdef FEAT_MOUSESHAPE
    // May have stopped dragging the status or separator line.  The pointer is
    // most likely still on the status or separator line.
    إذا (!is_drag && drag_status_هێڵ)
    {
	drag_status_هێڵ = FALSE;
	update_mouseshape(SHAPE_IDX_STATUS);
    }
    إذا (!is_drag && drag_sep_هێڵ)
    {
	drag_sep_هێڵ = FALSE;
	update_mouseshape(SHAPE_IDX_VSEP);
    }
#endif

    // Ignore drag and release events if we didn't get a click.
    إذا (is_click)
    {
	got_click = TRUE;
	in_tab_هێڵ = FALSE;
	in_tabpanel = FALSE;
#ifdef FEAT_TABPANEL
	in_tabpanel_scrollbar = false;
#endif
    }
    وإلا
    {
	إذا (!got_click)			// didn't get click, ignore
	    أرجع FALSE;
	إذا (!is_drag)			// release, reset got_click
	{
	    got_click = FALSE;
	    إذا (in_tab_هێڵ || in_tabpanel
#ifdef FEAT_TABPANEL
		    || in_tabpanel_scrollbar
#endif
		    )
	    {
		in_tab_هێڵ = FALSE;
		in_tabpanel = FALSE;
#ifdef FEAT_TABPANEL
		in_tabpanel_scrollbar = false;
#endif
		أرجع FALSE;
	    }
	}
    }

#ifdef FEAT_TABPANEL
    // Continue a scrollbar drag before any tab-selection handling.
    إذا (is_drag && in_tabpanel_scrollbar)
    {
	tabpanel_drag_scrollbar(mouse_row);
	أرجع FALSE;
    }
#endif

    // CTRL right mouse button does CTRL-T
    إذا (is_click && (mod_mask & MOD_MASK_CTRL) && which_button == MOUSE_ڕاست)
    {
	إذا (State & MODE_زیادکردن)
	    stuffcharReadbuff(Ctrl_O);
	إذا (ژماردن > 1)
	    stuffnumReadbuff(ژماردن);
	stuffcharReadbuff(Ctrl_T);
	got_click = FALSE;		// ignore drag&release now
	أرجع FALSE;
    }

    // CTRL only works with left mouse button
    إذا ((mod_mask & MOD_MASK_CTRL) && which_button != MOUSE_چەپ)
	أرجع FALSE;

    // When a modifier is down, ignore drag and release events, as well as
    // multiple clicks and the middle mouse button.
    // Accept shift-leftmouse drags when 'mousemodel' is "popup.*".
    إذا ((mod_mask & (MOD_MASK_SHIFT | MOD_MASK_CTRL | MOD_MASK_ALT
							     | MOD_MASK_META))
	    && (!is_click
		|| (mod_mask & MOD_MASK_MULTI_CLICK)
		|| which_button == MOUSE_ناوەڕاست)
	    && !((mod_mask & (MOD_MASK_SHIFT|MOD_MASK_ALT))
		&& mouse_model_popup()
		&& which_button == MOUSE_چەپ)
	    && !((mod_mask & MOD_MASK_ALT)
		&& !mouse_model_popup()
		&& which_button == MOUSE_ڕاست)
	    )
	أرجع FALSE;

    // If the button press was used as the movement command for an operator
    // (eg "d<MOUSE>"), or it is the middle button that is held down, ignore
    // drag/release events.
    إذا (!is_click && which_button == MOUSE_ناوەڕاست)
	أرجع FALSE;

    إذا (oap != عدم)
	regname = oap->regname;
    وإلا
	regname = 0;

    // Middle mouse button does a 'put' of the selected text
    إذا (which_button == MOUSE_ناوەڕاست)
    {
	إذا (State == MODE_NORMAL)
	{
	    // If an operator was pending, we don't know what the user wanted
	    // to do. Go back to normal mode: Clear the operator and beep().
	    إذا (oap != عدم && oap->op_type != OP_NOP)
	    {
		clearopbeep(oap);
		أرجع FALSE;
	    }

	    // If visual was active, yank the highlighted text and put it
	    // before the mouse pointer position.
	    // In Select mode replace the highlighted text with the clipboard.
	    إذا (VIsual_active)
	    {
		إذا (VIsual_select)
		{
		    stuffcharReadbuff(Ctrl_G);
		    stuffReadbuff((char_u *)"\"+p");
		}
		وإلا
		{
		    stuffcharReadbuff('y');
		    stuffcharReadbuff(K_MIDDLEMOUSE);
		}
		do_always = TRUE;	// ignore 'mouse' setting next time
		أرجع FALSE;
	    }
	    // The rest is below jump_to_mouse()
	}

	وإلا إذا ((State & MODE_زیادکردن) == 0)
	    أرجع FALSE;

	// Middle click in insert mode doesn't move the mouse, just insert the
	// contents of a register.  '.' register is special, can't insert that
	// with do_put().
	// Also paste at the cursor if the current mode isn't in 'mouse' (only
	// happens for the GUI).
	إذا ((State & MODE_زیادکردن) || !mouse_has(MOUSE_NORMAL))
	{
	    إذا (regname == '.')
		زیادکردن_reg(regname, TRUE);
	    وإلا
	    {
#ifdef FEAT_CLIPBOARD
		إذا (clip_star.available && regname == 0)
		    regname = '*';
#endif
		إذا ((State & REPLACE_FLAG) && !yank_register_mline(regname))
		    زیادکردن_reg(regname, TRUE);
		وإلا
		{
		    do_put(regname, عدم, BACKWARD, 1L,
						      fixindent | PUT_CURSEND);

		    // Repeat it with CTRL-R CTRL-O r or CTRL-R CTRL-P r
		    AppendCharToRedobuff(Ctrl_R);
		    AppendCharToRedobuff(fixindent ? Ctrl_P : Ctrl_O);
		    AppendCharToRedobuff(regname == 0 ? '"' : regname);
		}
	    }
	    أرجع FALSE;
	}
    }

    // When dragging or button-up stay in the same window.
    إذا (!is_click)
	jump_flags |= MOUSE_FOCUS | MOUSE_DID_MOVE;

    start_visual.lnum = 0;

    بنية tabpage_label_info {
	bool is_panel;	    // label type. true: tabpanel, false: tab line
	bool just_in;	    // just in tabpage label area
	bool just_click;    // just click tabpage label area
	صحيح nr;		    // tabpage number
    } tp_label = { false, false, false, 0 };

    // Check for clicking in the tab page panel.
#إذا defined(FEAT_TABPANEL)
    إذا (mouse_col < firstwin->w_wincol
		|| mouse_col >= firstwin->w_wincol + topframe->fr_width)
    {
	// A click on the scrollbar column starts a drag interaction and
	// preempts tab-selection.
	إذا (is_click && !is_drag && mouse_on_tabpanel_scrollbar())
	{
	    in_tabpanel_scrollbar = TRUE;
	    tabpanel_drag_scrollbar(mouse_row);
	    أرجع FALSE;
	}

	// Dispatch 'tabpanel' %[FuncName] click regions before falling through
	// to tab-page selection.  On drag events fall through to the normal
	// tab-drag handling.
	إذا (is_click && !is_drag
		&& stl_click_handler_regions(tabpanel_stl_click,
					    tabpanel_stl_click_ژماردن,
					    0, (char_u *)"tabpanel",
					    mouse_row, mouse_col,
					    which_button, mod_mask))
	    أرجع FALSE;

	tp_label.is_panel = true;
	tp_label.just_in = true;
	tp_label.nr = get_tabpagenr_on_tabpanel();

	// click in a tab selects that tab page
	إذا (is_click && cmdwin_type == 0)
	    tp_label.just_click = true;
    }
    وإلا
#endif
    // Check for clicking in the tab page line.
    إذا (TabPageIdxs != عدم && mouse_row == 0 && firstwin->w_winrow > 0)
    {
	// Dispatch 'tabline' %[FuncName] click regions before falling through
	// to tab-page selection.  On drag events fall through to the normal
	// tab-drag handling.
	إذا (is_click && !is_drag
		&& stl_click_handler_regions(tabline_stl_click,
					    tabline_stl_click_ژماردن,
					    0, (char_u *)"tabline",
					    mouse_row, mouse_col,
					    which_button, mod_mask))
	    أرجع FALSE;

	tp_label.just_in = true;
	tp_label.nr = TabPageIdxs[mouse_col];

	// click in a tab selects that tab page
	إذا (is_click && cmdwin_type == 0
		&& mouse_col < firstwin->w_wincol + topframe->fr_width)
	    tp_label.just_click = true;
    }

    إذا (tp_label.just_in)
    {
	إذا (is_drag)
	{
	    إذا (in_tabpanel || in_tab_هێڵ)
	    {
		c1 = tp_label.nr;
		tabpage_move(c1 <= 0 ? 9999 : c1 < tabpage_index(curtab)
							    ? c1 - 1 : c1);
	    }
	    أرجع FALSE;
	}

	إذا (tp_label.just_click)
	{
	    إذا (tp_label.is_panel)
		in_tabpanel = TRUE;
	    وإلا
		in_tab_هێڵ = TRUE;
	    c1 = tp_label.nr;
	    إذا (c1 >= 0)
	    {
		إذا ((mod_mask & MOD_MASK_MULTI_CLICK) == MOD_MASK_2CLICK
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
		وإلا
		{
		    // Go to specified tab page, or next one if not clicking
		    // on a label.
		    goto_tabpage(c1);

		    // It's like clicking on the status line of a window.
		    إذا (curwin != old_curwin)
			end_visual_mode_keep_button();
		}
	    }
	    وإلا
	    {
		tabpage_T	*tp;

		// Close the current or specified tab page.
		إذا (c1 == -999)
		    tp = curtab;
		وإلا
		    tp = find_tabpage(-c1);
		إذا (tp == curtab)
		{
		    إذا (first_tabpage->tp_next != عدم)
			tabpage_close(FALSE);
		}
		وإلا إذا (tp != عدم)
		    tabpage_close_other(tp, FALSE);
	    }
	}
	أرجع TRUE;
    }
    وإلا إذا (is_drag && (in_tabpanel || (in_tab_هێڵ && TabPageIdxs != عدم)))
    {
#إذا defined(FEAT_TABPANEL)
	إذا (in_tabpanel)
	    c1 = get_tabpagenr_on_tabpanel();
	وإلا
#endif
	    c1 = TabPageIdxs[mouse_col];
	tabpage_move(c1 <= 0 ? 9999 : c1 - 1);
	أرجع FALSE;
    }

    // When 'mousemodel' is "popup" or "popup_setpos", translate mouse events:
    // right button up   -> pop-up menu
    // shift-left button -> right button
    // alt-left button   -> alt-right button
    إذا (mouse_model_popup())
    {
	إذا (which_button == MOUSE_ڕاست
			    && !(mod_mask & (MOD_MASK_SHIFT | MOD_MASK_CTRL)))
	{
#ifdef USE_POPUP_SETPOS
# ifdef FEAT_GUI
	    إذا (gui.in_use)
	    {
#  إذا defined(FEAT_GUI_MOTIF) || defined(FEAT_GUI_GTK) \
			  || defined(FEAT_GUI_PHOTON)
		إذا (!is_click)
		    // Ignore right button release events, only shows the popup
		    // menu on the button down event.
		    أرجع FALSE;
#  endif
#  إذا defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_HAIKU)
		إذا (is_click || is_drag)
		    // Ignore right button down and drag mouse events.  Windows
		    // only shows the popup menu on the button up event.
		    أرجع FALSE;
#  endif
	    }
# endif
# إذا defined(FEAT_GUI) && defined(FEAT_TERM_POPUP_MENU)
	    وإلا
# endif
# إذا defined(FEAT_TERM_POPUP_MENU)
	    إذا (!is_click)
		// Ignore right button release events, only shows the popup
		// menu on the button down event.
		أرجع FALSE;
# endif

	    jump_flags = 0;
	    إذا (STRCMP(p_mousem, "popup_setpos") == 0)
	    {
		// First set the cursor position before showing the popup
		// menu.
		إذا (VIsual_active)
		{
		    pos_T    m_pos;

		    // set MOUSE_MAY_STOP_VIS if we are outside the
		    // selection or the current window (might have false
		    // negative here)
		    إذا (mouse_row < curwin->w_winrow
			 || mouse_row
				  > (curwin->w_winrow + curwin->w_height))
			jump_flags = MOUSE_MAY_STOP_VIS;
		    وإلا إذا (get_fpos_of_mouse(&m_pos) != IN_BUFFER)
			jump_flags = MOUSE_MAY_STOP_VIS;
		    وإلا
		    {
			إذا (VIsual_mode == 'V')
			{
			    إذا ((curwin->w_cursor.lnum <= VIsual.lnum
				    && (m_pos.lnum < curwin->w_cursor.lnum
					|| VIsual.lnum < m_pos.lnum))
				|| (VIsual.lnum < curwin->w_cursor.lnum
				    && (m_pos.lnum < VIsual.lnum
					|| curwin->w_cursor.lnum < m_pos.lnum)))
			    {
				jump_flags = MOUSE_MAY_STOP_VIS;
			    }
			}
			وإلا إذا ((LTOREQ_POS(curwin->w_cursor, VIsual)
				    && (LT_POS(m_pos, curwin->w_cursor)
					|| LT_POS(VIsual, m_pos)))
				|| (LT_POS(VIsual, curwin->w_cursor)
				    && (LT_POS(m_pos, VIsual)
				      || LT_POS(curwin->w_cursor, m_pos))))
			{
			    jump_flags = MOUSE_MAY_STOP_VIS;
			}
			وإلا إذا (VIsual_mode == Ctrl_V)
			{
			    getvcols(curwin, &curwin->w_cursor, &VIsual,
						     &leftcol, &rightcol, 0);
			    getvcol(curwin, &m_pos, عدم, &m_pos.col, عدم, 0);
			    إذا (m_pos.col < leftcol || m_pos.col > rightcol)
				jump_flags = MOUSE_MAY_STOP_VIS;
			}
		    }
		}
		وإلا
		    jump_flags = MOUSE_MAY_STOP_VIS;
	    }
	    إذا (jump_flags)
	    {
		jump_flags = jump_to_mouse(jump_flags, عدم, which_button);
		update_curbuf(VIsual_active ? UPD_INVERTED : UPD_VALID);
		setcursor();
		out_flush();    // Update before showing popup menu
	    }
# ifdef FEAT_MENU
	    show_popupmenu();
	    got_click = FALSE;	// ignore release events
# endif
	    أرجع (jump_flags & CURSOR_MOVED) != 0;
#وإلا
	    أرجع FALSE;
#endif
	}
	إذا (which_button == MOUSE_چەپ
				&& (mod_mask & (MOD_MASK_SHIFT|MOD_MASK_ALT)))
	{
	    which_button = MOUSE_ڕاست;
	    mod_mask &= ~MOD_MASK_SHIFT;
	}
    }

    إذا ((State & (MODE_NORMAL | MODE_زیادکردن))
			    && !(mod_mask & (MOD_MASK_SHIFT | MOD_MASK_CTRL)))
    {
	إذا (which_button == MOUSE_چەپ)
	{
	    إذا (is_click)
	    {
		// stop Visual mode for a left click in a window, but not when
		// on a status line
		إذا (VIsual_active)
		    jump_flags |= MOUSE_MAY_STOP_VIS;
	    }
	    وإلا إذا (mouse_has(MOUSE_VISUAL))
		jump_flags |= MOUSE_MAY_VIS;
	}
	وإلا إذا (which_button == MOUSE_ڕاست)
	{
	    إذا (is_click && VIsual_active)
	    {
		// Remember the start and end of visual before moving the
		// cursor.
		إذا (LT_POS(curwin->w_cursor, VIsual))
		{
		    start_visual = curwin->w_cursor;
		    end_visual = VIsual;
		}
		وإلا
		{
		    start_visual = VIsual;
		    end_visual = curwin->w_cursor;
		}
	    }
	    jump_flags |= MOUSE_FOCUS;
	    إذا (mouse_has(MOUSE_VISUAL))
		jump_flags |= MOUSE_MAY_VIS;
	}
    }

    // If an operator is pending, ignore all drags and releases until the
    // next mouse click.
    إذا (!is_drag && oap != عدم && oap->op_type != OP_NOP)
    {
	got_click = FALSE;
	oap->motion_type = MCHAR;
    }

    // When releasing the button let jump_to_mouse() know.
    إذا (!is_click && !is_drag)
	jump_flags |= MOUSE_RELEASED;

    // JUMP!
    jump_flags = jump_to_mouse(jump_flags,
			oap == عدم ? عدم : &(oap->inclusive), which_button);

#ifdef FEAT_MENU
    // A click in the window toolbar has no side effects.
    إذا (jump_flags & MOUSE_WINBAR)
	أرجع FALSE;
#endif
    moved = (jump_flags & CURSOR_MOVED);
    in_status_هێڵ = (jump_flags & IN_STATUS_هێڵ);
    in_sep_هێڵ = (jump_flags & IN_SEP_هێڵ);

    // Check for statusline click handler early, before visual mode or
    // other button-specific handling can interfere.
    إذا (in_status_هێڵ && is_click && !is_drag
	    && stl_click_handler(dragwin, mouse_row, mouse_col,
						which_button, mod_mask))
    {
#ifdef FEAT_MOUSESHAPE
	إذا (!drag_status_هێڵ)
	{
	    drag_status_هێڵ = TRUE;
	    update_mouseshape(-1);
	}
#endif
	أرجع FALSE;
    }

#ifdef FEAT_NETBEANS_INTG
    إذا (isNetbeansBuffer(curbuf)
			    && !(jump_flags & (IN_STATUS_هێڵ | IN_SEP_هێڵ)))
    {
	صحيح کلیل = KEY2TERMCAP1(c);

	إذا (کلیل == (صحيح)KE_LEFTRELEASE || کلیل == (صحيح)KE_MIDDLERELEASE
					       || کلیل == (صحيح)KE_RIGHTRELEASE)
	    netbeans_button_release(which_button);
    }
#endif

    // When jumping to another window, clear a pending operator.  That's a bit
    // friendlier than beeping and not jumping to that window.
    إذا (curwin != old_curwin && oap != عدم && oap->op_type != OP_NOP)
	clearop(oap);

#ifdef FEAT_FOLDING
    إذا (mod_mask == 0
	    && !is_drag
	    && (jump_flags & (MOUSE_FOLD_CLOSE | MOUSE_FOLD_OPEN))
	    && which_button == MOUSE_چەپ)
    {
	// open or close a fold at this line
	إذا (jump_flags & MOUSE_FOLD_OPEN)
	    openFold(curwin->w_cursor.lnum, 1L);
	وإلا
	    closeFold(curwin->w_cursor.lnum, 1L);
	// don't move the cursor if still in the same window
	إذا (curwin == old_curwin)
	    curwin->w_cursor = save_cursor;
    }
#endif

#إذا defined(FEAT_CLIPBOARD)
    إذا ((jump_flags & IN_OTHER_WIN) && !VIsual_active &&
	    (clip_star.available || clip_plus.available))
    {
	clip_modeless(which_button, is_click, is_drag);
	أرجع FALSE;
    }
#endif

    // Set global flag that we are extending the Visual area with mouse
    // dragging; temporarily minimize 'scrolloff'.
    إذا (VIsual_active && is_drag && get_scrolloff_بەها())
    {
	// In the very first line, allow scrolling one line
	إذا (mouse_row == 0)
	    mouse_dragging = 2;
	وإلا
	    mouse_dragging = 1;
    }

    // When dragging the mouse above the window, scroll down.
    إذا (is_drag && mouse_row < 0 && !in_status_هێڵ)
    {
	scroll_redraw(FALSE, 1L);
	mouse_row = 0;
    }

    إذا (start_visual.lnum)		// right click in visual mode
    {
       // When ALT is pressed make Visual mode blockwise.
       إذا (mod_mask & MOD_MASK_ALT)
	   VIsual_mode = Ctrl_V;

	// In Visual-block mode, divide the area in four, pick up the corner
	// that is in the quarter that the cursor is in.
	إذا (VIsual_mode == Ctrl_V)
	{
	    getvcols(curwin, &start_visual, &end_visual,
						    &leftcol, &rightcol, 0);
	    إذا (curwin->w_curswant > (leftcol + rightcol) / 2)
		end_visual.col = leftcol;
	    وإلا
		end_visual.col = rightcol;
	    إذا (curwin->w_cursor.lnum >=
				    (start_visual.lnum + end_visual.lnum) / 2)
		end_visual.lnum = start_visual.lnum;

	    // move VIsual to the right column
	    start_visual = curwin->w_cursor;	    // save the cursor pos
	    curwin->w_cursor = end_visual;
	    coladvance(end_visual.col);
	    VIsual = curwin->w_cursor;
	    curwin->w_cursor = start_visual;	    // restore the cursor
	}
	وإلا
	{
	    // If the click is before the start of visual, change the start.
	    // If the click is after the end of visual, change the end.  If
	    // the click is inside the visual, change the closest side.
	    إذا (LT_POS(curwin->w_cursor, start_visual))
		VIsual = end_visual;
	    وإلا إذا (LT_POS(end_visual, curwin->w_cursor))
		VIsual = start_visual;
	    وإلا
	    {
		// In the same line, compare column number
		إذا (end_visual.lnum == start_visual.lnum)
		{
		    إذا (curwin->w_cursor.col - start_visual.col >
				    end_visual.col - curwin->w_cursor.col)
			VIsual = start_visual;
		    وإلا
			VIsual = end_visual;
		}

		// In different lines, compare line number
		وإلا
		{
		    diff = (curwin->w_cursor.lnum - start_visual.lnum) -
				(end_visual.lnum - curwin->w_cursor.lnum);

		    إذا (diff > 0)		// closest to end
			VIsual = start_visual;
		    وإلا إذا (diff < 0)	// closest to start
			VIsual = end_visual;
		    وإلا			// in the middle line
		    {
			إذا (curwin->w_cursor.col <
					(start_visual.col + end_visual.col) / 2)
			    VIsual = end_visual;
			وإلا
			    VIsual = start_visual;
		    }
		}
	    }
	}
    }
    // If Visual mode started in insert mode, execute "CTRL-O"
    وإلا إذا ((State & MODE_زیادکردن) && VIsual_active)
	stuffcharReadbuff(Ctrl_O);

    // Middle mouse click: Put text before cursor.
    إذا (which_button == MOUSE_ناوەڕاست)
    {
#ifdef FEAT_CLIPBOARD
	إذا (clip_star.available && regname == 0)
	    regname = '*';
#endif
	إذا (yank_register_mline(regname))
	{
	    إذا (mouse_past_bottom)
		dir = FORWARD;
	}
	وإلا إذا (mouse_past_eol)
	    dir = FORWARD;

	إذا (fixindent)
	{
	    c1 = (dir == BACKWARD) ? '[' : ']';
	    c2 = 'p';
	}
	وإلا
	{
	    c1 = (dir == FORWARD) ? 'p' : 'P';
	    c2 = NUL;
	}
	prep_redo(regname, ژماردن, NUL, c1, NUL, c2, NUL);

	// Remember where the paste started, so in edit() Insstart can be set
	// to this position
	إذا (restart_edit != 0)
	    where_paste_started = curwin->w_cursor;
	do_put(regname, عدم, dir, ژماردن, fixindent | PUT_CURSEND);
    }

#إذا defined(FEAT_QUICKFIX)
    // Ctrl-Mouse click or double click in a quickfix window jumps to the
    // error under the mouse pointer.
    وإلا إذا (((mod_mask & MOD_MASK_CTRL)
		|| (mod_mask & MOD_MASK_MULTI_CLICK) == MOD_MASK_2CLICK)
	    && bt_quickfix(curbuf))
    {
	إذا (curwin->w_llist_ref == عدم)	// quickfix window
	    do_cmdline_cmd((char_u *)".cc");
	وإلا					// location list window
	    do_cmdline_cmd((char_u *)".ll");
	got_click = FALSE;		// ignore drag&release now
    }
#endif

    // Ctrl-Mouse click (or double click in a help window) jumps to the tag
    // under the mouse pointer.
    وإلا إذا ((mod_mask & MOD_MASK_CTRL) || (curbuf->b_help
		     && (mod_mask & MOD_MASK_MULTI_CLICK) == MOD_MASK_2CLICK))
    {
	إذا (State & MODE_زیادکردن)
	    stuffcharReadbuff(Ctrl_O);
	stuffcharReadbuff(Ctrl_RSB);
	got_click = FALSE;		// ignore drag&release now
    }

    // Shift-Mouse click searches for the next occurrence of the word under
    // the mouse pointer
    وإلا إذا ((mod_mask & MOD_MASK_SHIFT))
    {
	إذا ((State & MODE_زیادکردن) || (VIsual_active && VIsual_select))
	    stuffcharReadbuff(Ctrl_O);
	إذا (which_button == MOUSE_چەپ)
	    stuffcharReadbuff('*');
	وإلا	// MOUSE_RIGHT
	    stuffcharReadbuff('#');
    }

    // Handle double clicks, unless on status line
    وإلا إذا (in_status_هێڵ)
    {
#ifdef FEAT_MOUSESHAPE
	إذا ((is_drag || is_click) && !drag_status_هێڵ)
	{
	    drag_status_هێڵ = TRUE;
	    update_mouseshape(-1);
	}
#endif
    }
    وإلا إذا (in_sep_هێڵ)
    {
#ifdef FEAT_MOUSESHAPE
	إذا ((is_drag || is_click) && !drag_sep_هێڵ)
	{
	    drag_sep_هێڵ = TRUE;
	    update_mouseshape(-1);
	}
#endif
    }
    وإلا إذا ((mod_mask & MOD_MASK_MULTI_CLICK)
				       && (State & (MODE_NORMAL | MODE_زیادکردن))
	     && mouse_has(MOUSE_VISUAL))
    {
	إذا (is_click || !VIsual_active)
	{
	    إذا (VIsual_active)
		orig_cursor = VIsual;
	    وإلا
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
	    إذا ((mod_mask & MOD_MASK_MULTI_CLICK) == MOD_MASK_2CLICK)
	    {
		// Double click with ALT pressed makes it blockwise.
		إذا (mod_mask & MOD_MASK_ALT)
		    VIsual_mode = Ctrl_V;
		وإلا
		    VIsual_mode = 'v';
	    }
	    وإلا إذا ((mod_mask & MOD_MASK_MULTI_CLICK) == MOD_MASK_3CLICK)
		VIsual_mode = 'V';
	    وإلا إذا ((mod_mask & MOD_MASK_MULTI_CLICK) == MOD_MASK_4CLICK)
		VIsual_mode = Ctrl_V;
#ifdef FEAT_CLIPBOARD
	    // Make sure the clipboard gets updated.  Needed because start and
	    // end may still be the same, and the selection needs to be owned
	    clip_star.vmode = NUL;
#endif
	}
	// A double click selects a word or a block.
	إذا ((mod_mask & MOD_MASK_MULTI_CLICK) == MOD_MASK_2CLICK)
	{
	    pos_T	*pos = عدم;
	    صحيح		gc;

	    إذا (is_click)
	    {
		// If the character under the cursor (skipping white space) is
		// not a word character, try finding a match and select a (),
		// {}, [], #if/#endif, etc. block.
		end_visual = curwin->w_cursor;
		طالما (gc = gchar_pos(&end_visual), VIM_ISWHITE(gc))
		    inc(&end_visual);
		إذا (oap != عدم)
		    oap->motion_type = MCHAR;
		إذا (oap != عدم
			&& VIsual_mode == 'v'
			&& !vim_iswordc(gchar_pos(&end_visual))
			&& EQUAL_POS(curwin->w_cursor, VIsual)
			&& (pos = findmatch(oap, NUL)) != عدم)
		{
		    curwin->w_cursor = *pos;
		    إذا (oap->motion_type == MLINE)
			VIsual_mode = 'V';
		    وإلا إذا (*p_sel == 'e')
		    {
			إذا (LT_POS(curwin->w_cursor, VIsual))
			    ++VIsual.col;
			وإلا
			    ++curwin->w_cursor.col;
		    }
		}
	    }

	    إذا (pos == عدم && (is_click || is_drag))
	    {
		// When not found a match or when dragging: extend to include
		// a word.
		إذا (LT_POS(curwin->w_cursor, orig_cursor))
		{
		    find_start_of_word(&curwin->w_cursor);
		    find_end_of_word(&VIsual);
		}
		وإلا
		{
		    find_start_of_word(&VIsual);
		    إذا (*p_sel == 'e' && *ml_get_cursor() != NUL)
			curwin->w_cursor.col +=
					 (*mb_ptr2len)(ml_get_cursor());
		    find_end_of_word(&curwin->w_cursor);
		}
	    }
	    curwin->w_set_curswant = true;
	}
	إذا (is_click)
	    redraw_curbuf_later(UPD_INVERTED);	// update the inversion
    }
    وإلا إذا (VIsual_active && !old_active)
    {
	إذا (mod_mask & MOD_MASK_ALT)
	    VIsual_mode = Ctrl_V;
	وإلا
	    VIsual_mode = 'v';
    }

    // If Visual mode changed show it later.
    إذا ((!VIsual_active && old_active && mode_displayed)
	    || (VIsual_active && p_smd && پەیام_silent == 0
				 && (!old_active || VIsual_mode != old_mode)))
	redraw_cmdline = TRUE;

    أرجع moved;
}

    فراغ
ins_mouse(صحيح c)
{
    pos_T	tpos;
    win_T	*old_curwin = curwin;

#ifdef FEAT_GUI
    // When GUI is active, also move/paste when 'mouse' is empty
    إذا (!gui.in_use)
#endif
	إذا (!mouse_has(MOUSE_زیادکردن))
	    أرجع;

    undisplay_dollar();
    tpos = curwin->w_cursor;
    إذا (do_mouse(عدم, c, BACKWARD, 1L, 0))
    {
	win_T	*new_curwin = curwin;

	إذا (curwin != old_curwin && win_valid(old_curwin))
	{
	    // Mouse took us to another window.  We need to go back to the
	    // previous one to stop insert there properly.
	    curwin = old_curwin;
	    curbuf = curwin->w_buffer;
#ifdef FEAT_JOB_CHANNEL
	    إذا (bt_prompt(curbuf))
		// Restart Insert mode when re-entering the prompt buffer.
		curbuf->b_prompt_زیادکردن = 'A';
#endif
	}
	start_arrow(curwin == old_curwin ? &tpos : عدم);
	إذا (curwin != new_curwin && win_valid(new_curwin))
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
    ساكن فراغ
do_mousescroll(cmdarg_T *cap)
{
    صحيح shift_or_ctrl = mod_mask & (MOD_MASK_SHIFT | MOD_MASK_CTRL);

#ifdef FEAT_TERMINAL
    إذا (term_use_loop())
	// This window is a terminal window, send the mouse event there.
	// Set "typed" to FALSE to avoid an endless loop.
	send_کلیلەکان_to_term(curbuf->b_term, cap->cmdchar, mod_mask, FALSE);
    وإلا
#endif
    إذا (cap->arg == MSCR_UP || cap->arg == MSCR_DOWN)
    {
	// Vertical scrolling
	إذا (!(State & MODE_زیادکردن) && (mouse_vert_step < 0 || shift_or_ctrl))
	{
	    // whole page up or down
	    pagescroll(cap->arg == MSCR_UP ? FORWARD : BACKWARD, 1L, FALSE);
	}
	وإلا
	{
	    إذا (mouse_vert_step < 0 || shift_or_ctrl)
	    {
		// whole page up or down
		cap->count1 = (long)(curwin->w_botline - curwin->w_topline);
	    }
	    // Don't scroll more than half the window height.
	    وإلا إذا (curwin->w_height < mouse_vert_step * 2)
	    {
		cap->count1 = curwin->w_height / 2;
		إذا (cap->count1 == 0)
		    cap->count1 = 1;
	    }
	    وإلا
	    {
		cap->count1 = mouse_vert_step;
	    }
	    cap->count0 = cap->count1;
	    nv_scroll_هێڵ(cap);
	}

#ifdef FEAT_PROP_POPUP
	إذا (WIN_IS_POPUP(curwin))
	    popup_set_firstline(curwin);
#endif
    }
    وإلا
    {
	// Horizontal scrolling
	long step = (mouse_hor_step < 0 || shift_or_ctrl)
					    ? curwin->w_width : mouse_hor_step;
	long leftcol = curwin->w_leftcol
				     + (cap->arg == MSCR_ڕاست ? -step : step);
	إذا (leftcol < 0)
	    leftcol = 0;
	do_mousescroll_horiz((long_u)leftcol);
    }
    may_trigger_win_scrolled_resized();
}

/*
 * Insert mode implementation for scrolling in direction "dir", which is
 * one of the MSCR_ values.
 */
    فراغ
ins_mousescroll(صحيح dir)
{
    cmdarg_T	cap;
    oparg_T	oa;
    CLEAR_FIELD(cap);
    clear_oparg(&oa);
    cap.oap = &oa;
    cap.arg = dir;

#ifdef FEAT_TABPANEL
    إذا (mouse_row >= 0 && mouse_col >= 0
	    && (dir == MSCR_UP || dir == MSCR_DOWN)
	    && mouse_on_tabpanel())
    {
	(فراغ)tabpanel_scroll(dir == MSCR_UP ? 1 : -1,
		mouse_vert_step > 0 ? mouse_vert_step : 3);
	أرجع;
    }
#endif

    بدل (dir)
    {
	حالة MSCR_UP:
	    cap.cmdchar = K_MOUSEUP;
	    اكسر;
	حالة MSCR_DOWN:
	    cap.cmdchar = K_MOUSEDOWN;
	    اكسر;
	حالة MSCR_چەپ:
	    cap.cmdchar = K_MOUSELEFT;
	    اكسر;
	حالة MSCR_ڕاست:
	    cap.cmdchar = K_MOUSERIGHT;
	    اكسر;
	افتراضي:
	    siemsg("Invalid ins_mousescroll() argument: %d", dir);
    }

    win_T *old_curwin = curwin;
    إذا (mouse_row >= 0 && mouse_col >= 0)
    {
	// Find the window at the mouse pointer coordinates.
	// NOTE: Must restore "curwin" to "old_curwin" before returning!
	صحيح row = mouse_row;
	صحيح col = mouse_col;
	curwin = mouse_find_win(&row, &col, FIND_POPUP);
	إذا (curwin == عدم)
	{
	    curwin = old_curwin;
	    أرجع;
	}
	curbuf = curwin->w_buffer;
    }

    إذا (curwin == old_curwin)
    {
	// Don't scroll the current window if the popup menu is visible.
	إذا (pum_visible())
	    أرجع;

	undisplay_dollar();
    }

    linenr_T	orig_topline = curwin->w_topline;
    colnr_T	orig_leftcol = curwin->w_leftcol;
    pos_T	orig_cursor = curwin->w_cursor;

    // Call the common mouse scroll function shared with other modes.
    do_mousescroll(&cap);

    صحيح did_scroll = (orig_topline != curwin->w_topline
		   || orig_leftcol != curwin->w_leftcol);

    curwin->w_redr_status = true;
    curwin = old_curwin;
    curbuf = curwin->w_buffer;

    // If the window actually scrolled and the popup menu may overlay the
    // window, need to redraw it.
    إذا (did_scroll && pum_visible())
    {
	// TODO: Would be more efficient to only redraw the windows that are
	// overlapped by the popup menu.
	redraw_all_later(UPD_NOT_VALID);
	ins_compl_show_pum();
    }

    إذا (!EQUAL_POS(curwin->w_cursor, orig_cursor))
    {
	start_arrow(&orig_cursor);
	set_can_cindent(TRUE);
    }
}

#إذا defined(FEAT_PROP_POPUP)
/*
 * Command-line mode implementation for scrolling in direction "dir", which is
 * one of the MSCR_ values.  Scrolls the completion info popup when the mouse
 * pointer is on top of it.
 * Returns TRUE when the info popup was scrolled.
 */
    صحيح
cmdline_mousescroll(صحيح dir)
{
    cmdarg_T	cap;
    oparg_T	oa;

    CLEAR_FIELD(cap);
    clear_oparg(&oa);
    cap.oap = &oa;
    cap.arg = dir;

    بدل (dir)
    {
	حالة MSCR_UP:	 cap.cmdchar = K_MOUSEUP; اكسر;
	حالة MSCR_DOWN:	 cap.cmdchar = K_MOUSEDOWN; اكسر;
	حالة MSCR_چەپ:	 cap.cmdchar = K_MOUSELEFT; اكسر;
	حالة MSCR_ڕاست: cap.cmdchar = K_MOUSERIGHT; اكسر;
    }

    إذا (mouse_row < 0 || mouse_col < 0)
	أرجع FALSE;

    صحيح	    row = mouse_row;
    صحيح	    col = mouse_col;
    win_T   *wp;

    // Only scroll when the mouse is on top of the info popup.
    wp = mouse_find_win(&row, &col, FIND_POPUP);
    إذا (wp == عدم || !WIN_IS_POPUP(wp) || !(wp->w_popup_flags & POPF_INFO)
							|| !wp->w_has_scrollbar)
	أرجع FALSE;

    win_T   *old_curwin = curwin;

    curwin = wp;
    curbuf = wp->w_buffer;
    // Call the common mouse scroll function shared with other modes.
    do_mousescroll(&cap);
    curwin = old_curwin;
    curbuf = curwin->w_buffer;

    // Cmdline mode doesn't normally call update_screen(), so redraw the
    // completion popup menu, which also repaints the info popup.
    إذا (cmdline_pum_active())
	cmdline_pum_display();
    أرجع TRUE;
}
#endif

/*
 * Return TRUE if "c" is a mouse key.
 */
    صحيح
is_mouse_کلیل(صحيح c)
{
    أرجع c == K_LEFTMOUSE
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

ساكن بنية mousetable
{
    صحيح	    pseudo_code;	// Code for pseudo mouse event
    صحيح	    button;		// Which mouse button is it?
    صحيح	    is_click;		// Is it a mouse button click event?
    صحيح	    is_drag;		// Is it a mouse drag event?
} mouse_table[] =
{
    {(صحيح)KE_LEFTMOUSE,		MOUSE_چەپ,	TRUE,	FALSE},
#ifdef FEAT_GUI
    {(صحيح)KE_LEFTMOUSE_NM,	MOUSE_چەپ,	TRUE,	FALSE},
#endif
    {(صحيح)KE_LEFTDRAG,		MOUSE_چەپ,	FALSE,	TRUE},
    {(صحيح)KE_LEFTRELEASE,	MOUSE_چەپ,	FALSE,	FALSE},
#ifdef FEAT_GUI
    {(صحيح)KE_LEFTRELEASE_NM,	MOUSE_چەپ,	FALSE,	FALSE},
#endif
    {(صحيح)KE_MIDDLEMOUSE,	MOUSE_ناوەڕاست,	TRUE,	FALSE},
    {(صحيح)KE_MIDDLEDRAG,	MOUSE_ناوەڕاست,	FALSE,	TRUE},
    {(صحيح)KE_MIDDLERELEASE,	MOUSE_ناوەڕاست,	FALSE,	FALSE},
    {(صحيح)KE_RIGHTMOUSE,	MOUSE_ڕاست,	TRUE,	FALSE},
    {(صحيح)KE_RIGHTDRAG,		MOUSE_ڕاست,	FALSE,	TRUE},
    {(صحيح)KE_RIGHTRELEASE,	MOUSE_ڕاست,	FALSE,	FALSE},
    {(صحيح)KE_X1MOUSE,		MOUSE_X1,	TRUE,	FALSE},
    {(صحيح)KE_X1DRAG,		MOUSE_X1,	FALSE,	TRUE},
    {(صحيح)KE_X1RELEASE,		MOUSE_X1,	FALSE,	FALSE},
    {(صحيح)KE_X2MOUSE,		MOUSE_X2,	TRUE,	FALSE},
    {(صحيح)KE_X2DRAG,		MOUSE_X2,	FALSE,	TRUE},
    {(صحيح)KE_X2RELEASE,		MOUSE_X2,	FALSE,	FALSE},
    // DRAG without CLICK
    {(صحيح)KE_MOUSEMOVE,		MOUSE_RELEASE,	FALSE,	TRUE},
    // RELEASE without CLICK
    {(صحيح)KE_IGNORE,		MOUSE_RELEASE,	FALSE,	FALSE},
    {0,				0,		0,	0},
};

/*
 * Look up the given mouse code to return the relevant information in the other
 * arguments.  Return which button is down or was released.
 */
    صحيح
get_mouse_button(صحيح code, صحيح *is_click, صحيح *is_drag)
{
    صحيح	    i;

    لكل (i = 0; mouse_table[i].pseudo_code; i++)
	إذا (code == mouse_table[i].pseudo_code)
	{
	    *is_click = mouse_table[i].is_click;
	    *is_drag = mouse_table[i].is_drag;
	    أرجع mouse_table[i].button;
	}
    أرجع 0;	    // Shouldn't get here
}

/*
 * Return the appropriate pseudo mouse event token (KE_LEFTMOUSE etc) based on
 * the given information about which mouse button is down, and whether the
 * mouse was clicked, dragged or released.
 */
    صحيح
get_pseudo_mouse_code(
    صحيح	    button,	// eg MOUSE_LEFT
    صحيح	    is_click,
    صحيح	    is_drag)
{
    صحيح	    i;

    لكل (i = 0; mouse_table[i].pseudo_code; i++)
	إذا (button == mouse_table[i].button
	    && is_click == mouse_table[i].is_click
	    && is_drag == mouse_table[i].is_drag)
	{
#ifdef FEAT_GUI
	    // Trick: a non mappable left click and release has mouse_col -1
	    // or added MOUSE_COLOFF.  Used for 'mousefocus' in
	    // gui_mouse_moved()
	    إذا (mouse_col < 0 || mouse_col > MOUSE_COLOFF)
	    {
		إذا (mouse_col < 0)
		    mouse_col = 0;
		وإلا
		    mouse_col -= MOUSE_COLOFF;
		إذا (mouse_table[i].pseudo_code == (صحيح)KE_LEFTMOUSE)
		    أرجع (صحيح)KE_LEFTMOUSE_NM;
		إذا (mouse_table[i].pseudo_code == (صحيح)KE_LEFTRELEASE)
		    أرجع (صحيح)KE_LEFTRELEASE_NM;
	    }
#endif
	    أرجع mouse_table[i].pseudo_code;
	}
    أرجع (صحيح)KE_IGNORE;	    // not recognized, ignore it
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
ساكن صحيح has_mouse_termcode = 0;

    فراغ
set_mouse_termcode(
    صحيح		n,	// KS_MOUSE, KS_NETTERM_MOUSE or KS_DEC_MOUSE
    char_u	*s)
{
    char_u	name[2];

    name[0] = n;
    name[1] = KE_FILLER;
    add_termcode(name, s, FALSE);
#ifdef FEAT_MOUSE_JSB
    إذا (n == KS_JSBTERM_MOUSE)
	has_mouse_termcode |= HMT_JSBTERM;
    وإلا
#endif
#ifdef FEAT_MOUSE_NET
    إذا (n == KS_NETTERM_MOUSE)
	has_mouse_termcode |= HMT_NETTERM;
    وإلا
#endif
#ifdef FEAT_MOUSE_DEC
    إذا (n == KS_DEC_MOUSE)
	has_mouse_termcode |= HMT_DEC;
    وإلا
#endif
#ifdef FEAT_MOUSE_PTERM
    إذا (n == KS_PTERM_MOUSE)
	has_mouse_termcode |= HMT_PTERM;
    وإلا
#endif
#ifdef FEAT_MOUSE_URXVT
    إذا (n == KS_URXVT_MOUSE)
	has_mouse_termcode |= HMT_URXVT;
    وإلا
#endif
#ifdef FEAT_MOUSE_GPM
    إذا (n == KS_GPM_MOUSE)
	has_mouse_termcode |= HMT_GPM;
    وإلا
#endif
    إذا (n == KS_SGR_MOUSE)
	has_mouse_termcode |= HMT_SGR;
    وإلا إذا (n == KS_SGR_MOUSE_RELEASE)
	has_mouse_termcode |= HMT_SGR_REL;
    وإلا
	has_mouse_termcode |= HMT_NORMAL;
}

#إذا defined(UNIX) || defined(VMS)
    فراغ
del_mouse_termcode(
    صحيح		n)	// KS_MOUSE, KS_NETTERM_MOUSE or KS_DEC_MOUSE
{
    char_u	name[2];

    name[0] = n;
    name[1] = KE_FILLER;
    del_termcode(name);
# ifdef FEAT_MOUSE_JSB
    إذا (n == KS_JSBTERM_MOUSE)
	has_mouse_termcode &= ~HMT_JSBTERM;
    وإلا
# endif
# ifdef FEAT_MOUSE_NET
    إذا (n == KS_NETTERM_MOUSE)
	has_mouse_termcode &= ~HMT_NETTERM;
    وإلا
# endif
# ifdef FEAT_MOUSE_DEC
    إذا (n == KS_DEC_MOUSE)
	has_mouse_termcode &= ~HMT_DEC;
    وإلا
# endif
# ifdef FEAT_MOUSE_PTERM
    إذا (n == KS_PTERM_MOUSE)
	has_mouse_termcode &= ~HMT_PTERM;
    وإلا
# endif
# ifdef FEAT_MOUSE_URXVT
    إذا (n == KS_URXVT_MOUSE)
	has_mouse_termcode &= ~HMT_URXVT;
    وإلا
# endif
# ifdef FEAT_MOUSE_GPM
    إذا (n == KS_GPM_MOUSE)
	has_mouse_termcode &= ~HMT_GPM;
    وإلا
# endif
    إذا (n == KS_SGR_MOUSE)
	has_mouse_termcode &= ~HMT_SGR;
    وإلا إذا (n == KS_SGR_MOUSE_RELEASE)
	has_mouse_termcode &= ~HMT_SGR_REL;
    وإلا
	has_mouse_termcode &= ~HMT_NORMAL;
}
#endif

/*
 * setmouse() - switch mouse on/off depending on current mode and 'mouse'
 */
    فراغ
setmouse(فراغ)
{
    صحيح	    checkfor;

#ifdef FEAT_MOUSESHAPE
    update_mouseshape(-1);
#endif

    // Should be outside proc, but may break MOUSESHAPE
#ifdef FEAT_GUI
    // In the GUI the mouse is always enabled.
    إذا (gui.in_use)
	أرجع;
#endif
    // be quick when mouse is off
    إذا (*p_mouse == NUL || has_mouse_termcode == 0)
	أرجع;

    // don't switch mouse on when not in raw mode (Ex mode)
    إذا (cur_tmode != TMODE_RAW)
    {
	mch_setmouse(FALSE);
	أرجع;
    }

    إذا (VIsual_active)
	checkfor = MOUSE_VISUAL;
    وإلا إذا (State == MODE_HITRETURN || State == MODE_ASKMORE
						     || State == MODE_SETWSIZE)
	checkfor = MOUSE_RETURN;
    وإلا إذا (State & MODE_زیادکردن)
	checkfor = MOUSE_زیادکردن;
    وإلا إذا (State & MODE_CMDLINE)
	checkfor = MOUSE_COMMAND;
    وإلا إذا (State == MODE_CONFIRM || State == MODE_EXTERNCMD)
	checkfor = ' '; // don't use mouse for ":confirm" or ":!cmd"
    وإلا
	checkfor = MOUSE_NORMAL;    // assume normal mode

    إذا (mouse_has(checkfor))
	mch_setmouse(TRUE);
    وإلا
	mch_setmouse(FALSE);
}

/*
 * Return TRUE if
 * - "c" is in 'mouse', or
 * - 'a' is in 'mouse' and "c" is in MOUSE_A, or
 * - the current buffer is a help file and 'h' is in 'mouse' and we are in a
 *   normal editing mode (not at hit-return message).
 */
    صحيح
mouse_has(صحيح c)
{
    char_u	*p;

    لكل (p = p_mouse; *p; ++p)
	بدل (*p)
	{
	    حالة 'a': إذا (vim_strchr((char_u *)MOUSE_A, c) != عدم)
			  أرجع TRUE;
		      اكسر;
	    حالة MOUSE_HELP: إذا (c != MOUSE_RETURN && curbuf->b_help)
				 أرجع TRUE;
			     اكسر;
	    افتراضي: إذا (c == *p) أرجع TRUE; اكسر;
	}
    أرجع FALSE;
}

/*
 * Return TRUE when 'mousemodel' is set to "popup" or "popup_setpos".
 */
    صحيح
mouse_model_popup(فراغ)
{
    أرجع (p_mousem[0] == 'p');
}

/*
 * Call a click-region callback function.
 * "regions"/"region_count" describe the resolved click regions,
 * "winid" is stored as the "winid" key in the info dict (0 for tabline).
 * Returns TRUE if the function was called and handled the click.
 */
    ساكن صحيح
stl_click_handler_regions(
	stl_click_region_T  *regions,
	صحيح		    region_ژماردن,
	صحيح		    winid,
	char_u		    *area_name,
	صحيح		    mrow,
	صحيح		    mcol,
	صحيح		    which_button,
	صحيح		    mods)
{
#ifdef FEAT_EVAL
    صحيح		n;
    صحيح		nclicks;
    char_u	button_str[2];
    char_u	mods_str[4];
    صحيح		mi = 0;
    dict_T	*info;
    typval_T	argvars[2];
    typval_T	rettv;
    funcexe_T	funcexe;
    صحيح		col = mcol;

    إذا (regions == عدم || region_ژماردن == 0)
	أرجع FALSE;

    // Find the click region at the given row and column.
    لكل (n = 0; n < region_ژماردن; n++)
    {
	إذا (regions[n].row == mrow
		&& col >= regions[n].col_start
		&& col < regions[n].col_end)
	    اكسر;
    }
    إذا (n >= region_ژماردن || regions[n].funcname == عدم)
	أرجع FALSE;

    // Build the info dictionary.
    info = dict_alloc();
    إذا (info == عدم)
	أرجع FALSE;

    dict_add_ژمار(info, "minwid", regions[n].minwid);

    // Determine number of clicks.
    // MOD_MASK_2CLICK=0x20, MOD_MASK_3CLICK=0x40, MOD_MASK_4CLICK=0x60
    nclicks = ((mods & MOD_MASK_MULTI_CLICK) >> 5) + 1;
    إذا (nclicks > 3)
	nclicks = 3;
    dict_add_ژمار(info, "nclicks", nclicks);

    // Button.
    إذا (which_button == MOUSE_چەپ)
	button_str[0] = 'l';
    وإلا إذا (which_button == MOUSE_ڕاست)
	button_str[0] = 'r';
    وإلا
	button_str[0] = 'm';
    button_str[1] = NUL;
    dict_add_string_len(info, "button", button_str, 1);

    // Modifiers.
    إذا (mods & MOD_MASK_SHIFT)
	mods_str[mi++] = 's';
    إذا (mods & MOD_MASK_CTRL)
	mods_str[mi++] = 'c';
    إذا (mods & MOD_MASK_ALT)
	mods_str[mi++] = 'a';
    mods_str[mi] = NUL;
    dict_add_string_len(info, "mods", mods_str, mi);

    dict_add_ژمار(info, "winid", winid);

    // "area": which option the clicked region belongs to.  Lets a shared
    // dispatcher distinguish 'statusline', 'tabline' and 'tabpanel' without
    // having to overload winid == 0.
    dict_add_string(info, "area", area_name);

    // Expose tab page number for 'tabpanel' regions.
    إذا (regions[n].tabnr > 0)
	dict_add_ژمار(info, "tabnr", regions[n].tabnr);

    // Call the function with the info dict as argument.
    argvars[0].v_type = VAR_DICT;
    argvars[0].vval.v_dict = info;
    ++info->dv_refcount;
    argvars[1].v_type = VAR_UNKNOWN;

    rettv.v_type = VAR_ژمار;
    rettv.vval.v_ژمار = 0;

    CLEAR_FIELD(funcexe);
    funcexe.fe_evaluate = TRUE;
    (فراغ)call_func(regions[n].funcname, -1,
					    &rettv, 1, argvars, &funcexe);

    n = (صحيح)rettv.vval.v_ژمار;
    clear_tv(&rettv);
    dict_unref(info);

    إذا (n != 0)
    {
	// Make sure the tabline gets redrawn too when the callback asks for
	// a redraw (redraw_statuslines() only redraws the tabline when
	// redraw_tabline is set).  For tabpanel the whole screen needs to be
	// refreshed.
	إذا (winid == 0)
	    redraw_tabline = TRUE;
# ifdef FEAT_TABPANEL
	إذا (STRCMP(area_name, "tabpanel") == 0)
	    redraw_all_later(UPD_NOT_VALID);
# endif
	redraw_statuslines();
    }

    أرجع TRUE;
#وإلا
    (فراغ)regions;
    (فراغ)region_ژماردن;
    (فراغ)winid;
    (فراغ)area_name;
    (فراغ)mrow;
    (فراغ)mcol;
    (فراغ)which_button;
    (فراغ)mods;
    أرجع FALSE;
#endif
}

/*
 * Call a statusline click handler function for window "wp".
 * Returns TRUE if the function was called and handled the click.
 */
    ساكن صحيح
stl_click_handler(win_T *wp, صحيح mrow, صحيح mcol, صحيح which_button, صحيح mods)
{
    إذا (wp == عدم)
	أرجع FALSE;
    أرجع stl_click_handler_regions(wp->w_stl_click, wp->w_stl_click_ژماردن,
				wp->w_id, (char_u *)"statusline",
				mrow, mcol, which_button, mods);
}

// dragwin is declared near the top of the file

/*
 * Reset the window being dragged.  To be called when switching tab page.
 */
    فراغ
reset_dragwin(فراغ)
{
    dragwin = عدم;
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
    صحيح
jump_to_mouse(
    صحيح		flags,
    صحيح		*inclusive,	// used for inclusive operator, can be NULL
    صحيح		which_button)	// MOUSE_LEFT, MOUSE_RIGHT, MOUSE_MIDDLE
{
    ساكن صحيح	on_status_هێڵ = 0;	// #lines below bottom of window
    ساكن صحيح	on_sep_هێڵ = 0;	// on separator right of window
#ifdef FEAT_MENU
    ساكن صحيح  in_winbar = FALSE;
#endif
#ifdef FEAT_PROP_POPUP
    ساكن صحيح   in_popup_win = FALSE;
    ساكن win_T *click_in_popup_win = عدم;
#endif
    ساكن صحيح	prev_row = -1;
    ساكن صحيح	prev_col = -1;
    ساكن صحيح	did_drag = FALSE;	// drag was noticed

    win_T	*wp, *old_curwin;
    pos_T	old_cursor;
    صحيح		ژماردن;
    صحيح		first;
    صحيح		row = mouse_row;
    صحيح		col = mouse_col;
    colnr_T	col_from_screen = -1;
#ifdef FEAT_FOLDING
    صحيح		mouse_char = ' ';
#endif

    mouse_past_bottom = FALSE;
    mouse_past_eol = FALSE;

    إذا (flags & MOUSE_RELEASED)
    {
	// On button release we may change window focus if positioned on a
	// status line and no dragging happened.
	إذا (dragwin != عدم && !did_drag)
	    flags &= ~(MOUSE_FOCUS | MOUSE_DID_MOVE);
	dragwin = عدم;
	did_drag = FALSE;
#ifdef FEAT_PROP_POPUP
	إذا (click_in_popup_win != عدم && popup_dragwin == عدم)
	    popup_close_for_mouse_click(click_in_popup_win);

	popup_dragwin = عدم;
	click_in_popup_win = عدم;
#endif
    }

    إذا ((flags & MOUSE_DID_MOVE)
	    && prev_row == mouse_row
	    && prev_col == mouse_col)
    {
retnomove:
	// before moving the cursor for a left click which is NOT in a status
	// line, stop Visual mode
	إذا (on_status_هێڵ)
	    أرجع IN_STATUS_هێڵ;
	إذا (on_sep_هێڵ)
	    أرجع IN_SEP_هێڵ;
#ifdef FEAT_MENU
	إذا (in_winbar)
	{
	    // A quick second click may arrive as a double-click, but we use it
	    // as a second click in the WinBar.
	    إذا ((mod_mask & MOD_MASK_MULTI_CLICK) && !(flags & MOUSE_RELEASED))
	    {
		wp = mouse_find_win(&row, &col, FAIL_POPUP);
		إذا (wp == عدم)
		    أرجع IN_UNKNOWN;
		winbar_click(wp, col);
	    }
	    أرجع IN_OTHER_WIN | MOUSE_WINBAR;
	}
#endif
	إذا (flags & MOUSE_MAY_STOP_VIS)
	{
	    end_visual_mode_keep_button();
	    redraw_curbuf_later(UPD_INVERTED);	// delete the inversion
	}
#إذا defined(FEAT_CLIPBOARD)
	// Continue a modeless selection in another window.
	إذا (cmdwin_type != 0 && row < cmdwin_win->w_winrow)
	    أرجع IN_OTHER_WIN;
#endif
#ifdef FEAT_PROP_POPUP
	// Continue a modeless selection in a popup window or dragging it.
	إذا (in_popup_win)
	{
	    click_in_popup_win = عدم;  // don't close it on release
	    إذا (popup_dragwin != عدم)
	    {
		// dragging a popup window
		popup_drag(popup_dragwin);
		أرجع IN_UNKNOWN;
	    }
	    أرجع IN_OTHER_WIN;
	}
#endif
	أرجع IN_BUFFER;
    }

    prev_row = mouse_row;
    prev_col = mouse_col;

    إذا (flags & MOUSE_SETPOS)
	اذهب retnomove;				// ugly goto...

    old_curwin = curwin;
    old_cursor = curwin->w_cursor;

    إذا (!(flags & MOUSE_FOCUS))
    {
	إذا (row < 0 || col < 0) // check if it makes sense
	    أرجع IN_UNKNOWN;

	// find the window where the row is in and adjust "row" and "col" to be
	// relative to top-left of the window
	wp = mouse_find_win(&row, &col, FIND_POPUP);
	إذا (wp == عدم)
	    أرجع IN_UNKNOWN;
	dragwin = عدم;

#ifdef FEAT_PROP_POPUP
	// Click in a popup window may start dragging or modeless selection,
	// but not much else.
	إذا (WIN_IS_POPUP(wp))
	{
	    on_sep_هێڵ = 0;
	    on_status_هێڵ = 0;
	    in_popup_win = TRUE;
	    إذا (which_button == MOUSE_چەپ && popup_close_if_on_X(wp, row, col))
	    {
		أرجع IN_UNKNOWN;
	    }
	    وإلا إذا (((wp->w_popup_flags & (POPF_DRAG | POPF_RESIZE))
					      && popup_on_border(wp, row, col))
				       || (wp->w_popup_flags & POPF_DRAGALL))
	    {
		popup_dragwin = wp;
		popup_start_drag(wp, row, col);
		أرجع IN_UNKNOWN;
	    }
	    // Only close on release, otherwise it's not possible to drag or do
	    // modeless selection.
	    وإلا إذا (wp->w_popup_close == POPCLOSE_CLICK
		    && which_button == MOUSE_چەپ)
	    {
		click_in_popup_win = wp;
	    }
	    وإلا إذا (which_button == MOUSE_چەپ)
		// If the click is in the scrollbar, may scroll up/down.
		popup_handle_scrollbar_click(wp, row, col);
# ifdef FEAT_CLIPBOARD
	    أرجع IN_OTHER_WIN;
# وإلا
	    أرجع IN_UNKNOWN;
# endif
	}
	in_popup_win = FALSE;
	popup_dragwin = عدم;
#endif
#ifdef FEAT_MENU
	إذا (row == -1)
	{
	    // A click in the window toolbar does not enter another window or
	    // change Visual highlighting.
	    winbar_click(wp, col);
	    in_winbar = TRUE;
	    أرجع IN_OTHER_WIN | MOUSE_WINBAR;
	}
	in_winbar = FALSE;
#endif

	// winpos and height may change in win_enter()!
	إذا (row >= wp->w_height)		// In (or below) status line
	{
	    on_status_هێڵ = row - wp->w_height + 1;
	    dragwin = wp;
	}
	وإلا
	    on_status_هێڵ = 0;
	إذا (col >= wp->w_width)		// In separator line
	{
	    on_sep_هێڵ = col - wp->w_width + 1;
	    dragwin = wp;
	}
	وإلا
	    on_sep_هێڵ = 0;

	// The rightmost character of the status line might be a vertical
	// separator character if there is no connecting window to the right.
	إذا (on_status_هێڵ && on_sep_هێڵ)
	{
	    إذا (stl_connected(wp))
		on_sep_هێڵ = 0;
	    وإلا
		on_status_هێڵ = 0;
	}

	// Before jumping to another buffer, or moving the cursor for a left
	// click, stop Visual mode.
	إذا (VIsual_active
		&& (wp->w_buffer != curwin->w_buffer
		    || (!on_status_هێڵ && !on_sep_هێڵ
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
	إذا (cmdwin_type != 0 && wp != cmdwin_win)
	{
	    // A click outside the command-line window: Use modeless
	    // selection if possible.  Allow dragging the status lines.
	    on_sep_هێڵ = 0;
#ifdef FEAT_CLIPBOARD
	    إذا (on_status_هێڵ)
		أرجع IN_STATUS_هێڵ;
	    أرجع IN_OTHER_WIN;
#وإلا
	    row = 0;
	    col += wp->w_wincol;
	    wp = cmdwin_win;
#endif
	}
#إذا defined(FEAT_PROP_POPUP) && defined(FEAT_TERMINAL)
	إذا (popup_is_popup(curwin) && curbuf->b_term != عدم)
	    // terminal in popup window: don't jump to another window
	    أرجع IN_OTHER_WIN;
#endif
	// Only change window focus when not clicking on or dragging the
	// status line.  Do change focus when releasing the mouse button
	// (MOUSE_FOCUS was set above if we dragged first).
	إذا (dragwin == عدم || (flags & MOUSE_RELEASED))
	    win_enter(wp, TRUE);		// can make wp invalid!

	إذا (curwin != old_curwin)
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
	إذا (on_status_هێڵ)			// In (or below) status line
	{
	    // Don't use start_arrow() if we're in the same window
	    إذا (curwin == old_curwin)
		أرجع IN_STATUS_هێڵ;
	    وإلا
		أرجع IN_STATUS_هێڵ | CURSOR_MOVED;
	}
	إذا (on_sep_هێڵ)			// In (or below) status line
	{
	    // Don't use start_arrow() if we're in the same window
	    إذا (curwin == old_curwin)
		أرجع IN_SEP_هێڵ;
	    وإلا
		أرجع IN_SEP_هێڵ | CURSOR_MOVED;
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
    وإلا إذا (on_status_هێڵ && which_button == MOUSE_چەپ)
    {
	إذا (dragwin != عدم)
	{
	    // Drag the status line
	    ژماردن = row - W_WINROW(dragwin) - dragwin->w_height + 1
							     - on_status_هێڵ;
	    win_drag_status_هێڵ(dragwin, ژماردن);
	    did_drag |= ژماردن;
	}
	أرجع IN_STATUS_هێڵ;			// Cursor didn't move
    }
    وإلا إذا (on_sep_هێڵ && which_button == MOUSE_چەپ)
    {
	إذا (dragwin != عدم)
	{
	    // Drag the separator column
	    ژماردن = col - dragwin->w_wincol - dragwin->w_width + 1
								- on_sep_هێڵ;
	    win_drag_vsep_هێڵ(dragwin, ژماردن);
	    did_drag |= ژماردن;
	}
	أرجع IN_SEP_هێڵ;			// Cursor didn't move
    }
#ifdef FEAT_MENU
    وإلا إذا (in_winbar)
    {
	// After a click on the window toolbar don't start Visual mode.
	أرجع IN_OTHER_WIN | MOUSE_WINBAR;
    }
#endif
    وإلا // keep_window_focus must be TRUE
    {
	// before moving the cursor for a left click, stop Visual mode
	إذا (flags & MOUSE_MAY_STOP_VIS)
	{
	    end_visual_mode_keep_button();
	    redraw_curbuf_later(UPD_INVERTED);	// delete the inversion
	}

#إذا defined(FEAT_CLIPBOARD)
	// Continue a modeless selection in another window.
	إذا (cmdwin_type != 0 && row < cmdwin_win->w_winrow)
	    أرجع IN_OTHER_WIN;
#endif
#ifdef FEAT_PROP_POPUP
	إذا (in_popup_win)
	{
	    إذا (popup_dragwin != عدم)
	    {
		// dragging a popup window
		popup_drag(popup_dragwin);
		أرجع IN_UNKNOWN;
	    }
	    // continue a modeless selection in a popup window
	    click_in_popup_win = عدم;
	    أرجع IN_OTHER_WIN;
	}
#endif

	row -= W_WINROW(curwin);
	col -= curwin->w_wincol;

	// When clicking beyond the end of the window, scroll the screen.
	// Scroll by however many rows outside the window we are.
	إذا (row < 0)
	{
	    ژماردن = 0;
	    لكل (first = TRUE; curwin->w_topline > 1; )
	    {
#ifdef FEAT_DIFF
		إذا (curwin->w_topfill < diff_check_fill(curwin, curwin->w_topline))
		    ++ژماردن;
		وإلا
#endif
		    ژماردن += plines(curwin->w_topline - 1);
		إذا (!first && ژماردن > -row)
		    اكسر;
		first = FALSE;
#ifdef FEAT_FOLDING
		(فراغ)hasFolding(curwin->w_topline, &curwin->w_topline, عدم);
#endif
#ifdef FEAT_DIFF
		إذا (curwin->w_topfill < diff_check_fill(curwin, curwin->w_topline))
		    ++curwin->w_topfill;
		وإلا
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
	وإلا إذا (row >= curwin->w_height)
	{
	    ژماردن = 0;
	    لكل (first = TRUE; curwin->w_topline < curbuf->b_ml.ml_هێڵ_ژماردن; )
	    {
#ifdef FEAT_DIFF
		إذا (curwin->w_topfill > 0)
		    ++ژماردن;
		وإلا
#endif
		    ژماردن += plines(curwin->w_topline);
		إذا (!first && ژماردن > row - curwin->w_height + 1)
		    اكسر;
		first = FALSE;
#ifdef FEAT_FOLDING
		إذا (hasFolding(curwin->w_topline, عدم, &curwin->w_topline)
			&& curwin->w_topline == curbuf->b_ml.ml_هێڵ_ژماردن)
		    اكسر;
#endif
#ifdef FEAT_DIFF
		إذا (curwin->w_topfill > 0)
		    --curwin->w_topfill;
		وإلا
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
	وإلا إذا (row == 0)
	{
	    // When dragging the mouse, while the text has been scrolled up as
	    // far as it goes, moving the mouse in the top line should scroll
	    // the text down (done later when recomputing w_topline).
	    إذا (mouse_dragging > 0
		    && curwin->w_cursor.lnum
				       == curwin->w_buffer->b_ml.ml_هێڵ_ژماردن
		    && curwin->w_cursor.lnum == curwin->w_topline)
		curwin->w_valid &= ~(VALID_TOPLINE);
	}
    }

    إذا (prev_row >= W_WINROW(curwin)
	&& prev_row < W_WINROW(curwin) + curwin->w_height
	&& prev_col >= curwin->w_wincol && prev_col < W_ENDCOL(curwin)
						       && Screen_هێڵەکان != عدم)
    {
	صحيح off = هێڵ_Offset[prev_row] + prev_col;

	// Only use ScreenCols[] after the window was redrawn.  Mainly matters
	// for tests, a user would not click before redrawing.
	// Do not use when 'virtualedit' is active.
	إذا (curwin->w_redr_type <= UPD_VALID_NO_UPDATE)
	    col_from_screen = ScreenCols[off];
#ifdef FEAT_FOLDING
	// Remember the character under the mouse, it might be a '-' or '+' in
	// the fold column.
	mouse_char = enc_utf8 && Screen_هێڵەکان_UC[off] != 0
				       ? Screen_هێڵەکان_UC[off] : Screen_هێڵەکان[off];
#endif
    }

#ifdef FEAT_FOLDING
    // Check for position outside of the fold column.
    إذا (
# ifdef FEAT_RIGHTLEFT
	    curwin->w_p_rl ? col < curwin->w_width - curwin->w_p_fdc :
# endif
	    col >= curwin->w_p_fdc + (cmdwin_win != curwin ? 0 : 1)
       )
	mouse_char = ' ';
#endif

    // compute the position in the buffer line from the posn on the screen
    إذا (mouse_comp_pos(curwin, &row, &col, &curwin->w_cursor.lnum, عدم))
	mouse_past_bottom = TRUE;

    // Start Visual mode before coladvance(), for when 'sel' != "old"
    إذا ((flags & MOUSE_MAY_VIS) && !VIsual_active)
    {
	check_visual_highlight();
	VIsual = old_cursor;
	VIsual_active = TRUE;
	VIsual_reselect = TRUE;
	// if 'selectmode' contains "mouse", start Select mode
	may_start_select('o');
	setmouse();
	إذا (p_smd && پەیام_silent == 0)
	    redraw_cmdline = TRUE;	// show visual mode later
    }

    إذا (col_from_screen >= 0)
    {
	// Use the virtual column from ScreenCols[], it is accurate also after
	// concealed characters.
	col = col_from_screen;
    }

    curwin->w_curswant = col;
    curwin->w_set_curswant = false;	// May still have been TRUE
    إذا (coladvance(col) == FAIL)	// Mouse click beyond end of line
    {
	إذا (inclusive != عدم)
	    *inclusive = TRUE;
	mouse_past_eol = TRUE;
    }
    وإلا إذا (inclusive != عدم)
	*inclusive = FALSE;

    ژماردن = IN_BUFFER;
    إذا (curwin != old_curwin || curwin->w_cursor.lnum != old_cursor.lnum
	    || curwin->w_cursor.col != old_cursor.col)
	ژماردن |= CURSOR_MOVED;		// Cursor has moved

#ifdef FEAT_FOLDING
    إذا (mouse_char == curwin->w_fill_chars.foldclosed)
	ژماردن |= MOUSE_FOLD_OPEN;
    وإلا إذا (mouse_char != ' ')
	ژماردن |= MOUSE_FOLD_CLOSE;
#endif

    أرجع ژماردن;
}

/*
 * Make a horizontal scroll to "leftcol".
 * Return TRUE if the cursor moved, FALSE otherwise.
 */
    صحيح
do_mousescroll_horiz(long_u leftcol)
{
    إذا (curwin->w_p_wrap)
	أرجع FALSE;  // no horizontal scrolling when wrapping

    إذا (curwin->w_leftcol == (colnr_T)leftcol)
	أرجع FALSE;  // already there

    // When the line of the cursor is too short, move the cursor to the
    // longest visible line.
    إذا (
#ifdef FEAT_GUI
	    (!gui.in_use || vim_strchr(p_go, GO_HORSCROLL) == عدم) &&
#endif
		    !virtual_active()
	    && (long)leftcol > scroll_هێڵ_len(curwin->w_cursor.lnum))
    {
	curwin->w_cursor.lnum = ui_find_longest_lnum();
	curwin->w_cursor.col = 0;
    }

    أرجع set_leftcol((colnr_T)leftcol);
}

/*
 * Normal and Visual modes implementation for scrolling in direction
 * "cap->arg", which is one of the MSCR_ values.
 */
    فراغ
nv_mousescroll(cmdarg_T *cap)
{
    win_T   *old_curwin = curwin;

#ifdef FEAT_TABPANEL
    إذا (mouse_row >= 0 && mouse_col >= 0
	    && (cap->arg == MSCR_UP || cap->arg == MSCR_DOWN)
	    && mouse_on_tabpanel())
    {
	(فراغ)tabpanel_scroll(cap->arg == MSCR_UP ? 1 : -1,
		mouse_vert_step > 0 ? mouse_vert_step : 3);
	أرجع;
    }
#endif

    إذا (mouse_row >= 0 && mouse_col >= 0)
    {
	// Find the window at the mouse pointer coordinates.
	// NOTE: Must restore "curwin" to "old_curwin" before returning!
	صحيح row = mouse_row;
	صحيح col = mouse_col;
	curwin = mouse_find_win(&row, &col, FIND_POPUP);
	إذا (curwin == عدم)
	{
	    curwin = old_curwin;
	    أرجع;
	}

#ifdef FEAT_PROP_POPUP
	إذا (WIN_IS_POPUP(curwin) && !curwin->w_has_scrollbar)
	{
	    // cannot scroll this popup window
	    curwin = old_curwin;
	    أرجع;
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
    فراغ
nv_mouse(cmdarg_T *cap)
{
    (فراغ)do_mouse(cap->oap, cap->cmdchar, BACKWARD, cap->count1, 0);
}

ساكن صحيح	held_button = MOUSE_RELEASE;

    فراغ
reset_held_button(فراغ)
{
    held_button = MOUSE_RELEASE;
}

/*
 * Check if typebuf 'tp' contains a terminal mouse code and returns the
 * modifiers found in typebuf in 'modifiers'.
 */
    صحيح
check_termcode_mouse(
    char_u	*tp,
    صحيح		*slen,
    char_u	*کلیل_name,
    char_u	*modifiers_start,
    صحيح		idx,
    صحيح		*modifiers)
{
    صحيح		j;
    char_u	*p;
#إذا !defined(UNIX) || defined(FEAT_MOUSE_XTERM) || defined(FEAT_GUI) \
    || defined(FEAT_MOUSE_GPM) || defined(FEAT_SYSMOUSE)
    char_u	bytes[6];
    صحيح		ژمارە_bytes;
#endif
    صحيح		mouse_code = 0;	    // init for GCC
    صحيح		is_click, is_drag;
    صحيح		is_release, release_is_ambiguous;
    صحيح		wheel_code = 0;
    صحيح		current_button;
    ساكن صحيح	orig_ژمارە_clicks = 1;
    ساكن صحيح	orig_mouse_code = 0x0;
#ifdef CHECK_DOUBLE_CLICK
    ساكن صحيح	orig_mouse_col = 0;
    ساكن صحيح	orig_mouse_row = 0;
    ساكن بنية timeval  orig_mouse_time = {0, 0};
    // time of previous mouse click
    بنية timeval  mouse_time;		// time of current mouse click
    long	timediff;		// elapsed time in msec
#endif

    is_click = is_drag = is_release = release_is_ambiguous = FALSE;

#إذا !defined(UNIX) || defined(FEAT_MOUSE_XTERM) || defined(FEAT_GUI) \
    || defined(FEAT_MOUSE_GPM) || defined(FEAT_SYSMOUSE)
    إذا (کلیل_name[0] == KS_MOUSE
# ifdef FEAT_MOUSE_GPM
	    || کلیل_name[0] == KS_GPM_MOUSE
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
	لكل (;;)
	{
	    // For the GUI and for MS-Windows two bytes each are used for row
	    // and column.  Allows for more than 223 columns.
# إذا defined(FEAT_GUI) || defined(MSWIN)
	    إذا (TRUE
#  إذا defined(FEAT_GUI) && !defined(MSWIN)
		&& gui.in_use
#  endif
		)
	    {
		ژمارە_bytes = get_bytes_from_buf(tp + *slen, bytes, 5);
		إذا (ژمارە_bytes == -1)	// not enough coordinates
		    أرجع -1;
		mouse_code = bytes[0];
		mouse_col = 128 * (bytes[1] - ' ' - 1)
		    + bytes[2] - ' ' - 1;
		mouse_row = 128 * (bytes[3] - ' ' - 1)
		    + bytes[4] - ' ' - 1;
	    }
	    وإلا
# endif
	    {
		ژمارە_bytes = get_bytes_from_buf(tp + *slen, bytes, 3);
		إذا (ژمارە_bytes == -1)	// not enough coordinates
		    أرجع -1;
		mouse_code = bytes[0];
		mouse_col = bytes[1] - ' ' - 1;
		mouse_row = bytes[2] - ' ' - 1;
	    }
	    *slen += ژمارە_bytes;

	    // If the following bytes is also a mouse code and it has the same
	    // code, dump this one and get the next.  This makes dragging a
	    // whole lot faster.
# ifdef FEAT_GUI
	    إذا (gui.in_use)
		j = 3;
	    وإلا
# endif
		j = get_termcode_len(idx);
	    إذا (STRNCMP(tp, tp + *slen, (size_t)j) == 0
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
	    وإلا
		اكسر;
	}
    }

    إذا (کلیل_name[0] == KS_URXVT_MOUSE
	    || کلیل_name[0] == KS_SGR_MOUSE
	    || کلیل_name[0] == KS_SGR_MOUSE_RELEASE)
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
	إذا (p == عدم)
	    أرجع -1;

	mouse_code = getdigits(&p);
	إذا (*p++ != ';')
	    أرجع -1;

	// when mouse reporting is SGR, add 32 to mouse code
	إذا (کلیل_name[0] == KS_SGR_MOUSE
		|| کلیل_name[0] == KS_SGR_MOUSE_RELEASE)
	    mouse_code += 32;

	mouse_col = getdigits(&p) - 1;
	إذا (*p++ != ';')
	    أرجع -1;

	mouse_row = getdigits(&p) - 1;

	// The modifiers were the mouse coordinates, not the modifier keys
	// (alt/shift/ctrl/meta) state.
	*modifiers = 0;
    }

    إذا (کلیل_name[0] == KS_SGR_MOUSE
	    || کلیل_name[0] == KS_SGR_MOUSE_RELEASE)
    {
	إذا (کلیل_name[0] == KS_SGR_MOUSE_RELEASE)
	{
	    is_release = TRUE;
	    // This is used below to set held_button.
	    mouse_code |= MOUSE_RELEASE;
	}
    }
    وإلا
    {
	release_is_ambiguous = TRUE;
	إذا ((mouse_code & MOUSE_RELEASE) == MOUSE_RELEASE)
	    is_release = TRUE;
    }

    إذا (کلیل_name[0] == KS_MOUSE
# ifdef FEAT_MOUSE_GPM
	    || کلیل_name[0] == KS_GPM_MOUSE
# endif
# ifdef FEAT_MOUSE_URXVT
	    || کلیل_name[0] == KS_URXVT_MOUSE
# endif
	    || کلیل_name[0] == KS_SGR_MOUSE
	    || کلیل_name[0] == KS_SGR_MOUSE_RELEASE)
    {
# إذا !defined(MSWIN)
	/*
	 * Handle old style mouse events.
	 * Recognize the xterm mouse wheel, but not in the GUI, the
	 * Linux console with GPM and the MS-DOS or Win32 console
	 * (multi-clicks use >= 0x60).
	 */
	إذا (mouse_code >= MOUSEWHEEL_نزم && mouse_code < MOUSESIDEBUTTONS_نزم
#  ifdef FEAT_GUI
		&& !gui.in_use
#  endif
#  ifdef FEAT_MOUSE_GPM
		&& کلیل_name[0] != KS_GPM_MOUSE
#  endif
	   )
	{
#  إذا defined(UNIX)
	    إذا (use_xterm_mouse() > 1 && mouse_code >= 0x80)
		// mouse-move event, using MOUSE_DRAG works
		mouse_code = MOUSE_DRAG;
	    وإلا
#  endif
		// Keep the mouse_code before it's changed, so that we
		// remember that it was a mouse wheel click.
		wheel_code = mouse_code;
	}
#  ifdef FEAT_MOUSE_XTERM
	وإلا إذا (held_button == MOUSE_RELEASE
#   ifdef FEAT_GUI
		&& !gui.in_use
#   endif
		&& (mouse_code == 0x23 || mouse_code == 0x24
		    || mouse_code == 0x40 || mouse_code == 0x41))
	{
	    // Apparently 0x23 and 0x24 are used by rxvt scroll wheel.
	    // And 0x40 and 0x41 are used by some xterm emulator.
	    wheel_code = mouse_code - (mouse_code >= 0x40 ? 0x40 : 0x23)
							      + MOUSEWHEEL_نزم;
	}
#  endif

#  إذا defined(UNIX)
	وإلا إذا (use_xterm_mouse() > 1)
	{
	    إذا (mouse_code & MOUSE_DRAG_XTERM)
		mouse_code |= MOUSE_DRAG;
	}
#  endif
#  ifdef FEAT_XCLIPBOARD
	وإلا إذا (!(mouse_code & MOUSE_DRAG & ~MOUSE_CLICK_MASK))
	{
	    إذا (is_release)
		stop_xterm_trace();
	    وإلا
		start_xterm_trace(mouse_code);
	}
#  endif
# endif
    }
#endif // !UNIX || FEAT_MOUSE_XTERM
#ifdef FEAT_MOUSE_NET
    إذا (کلیل_name[0] == KS_NETTERM_MOUSE)
    {
	صحيح mc, mr;

	// expect a rather limited sequence like: balancing {
	// \033}6,45\r
	// '6' is the row, 45 is the column
	p = tp + *slen;
	mr = getdigits(&p);
	إذا (*p++ != ',')
	    أرجع -1;
	mc = getdigits(&p);
	إذا (*p++ != '\r')
	    أرجع -1;

	mouse_col = mc - 1;
	mouse_row = mr - 1;
	mouse_code = MOUSE_چەپ;
	*slen += (صحيح)(p - (tp + *slen));
    }
#endif	// FEAT_MOUSE_NET
#ifdef FEAT_MOUSE_JSB
    إذا (کلیل_name[0] == KS_JSBTERM_MOUSE)
    {
	صحيح mult, val, iter, button, status;

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
	بدل (*p++)
	{
	    حالة 'L': button = 1; اكسر;
	    حالة '-': اكسر;
	    حالة 'x': اكسر; // ignore sequence
	    افتراضي:  أرجع -1; // Unknown Result
	}
	بدل (*p++)
	{
	    حالة 'M': button |= 2; اكسر;
	    حالة '-': اكسر;
	    حالة 'x': اكسر; // ignore sequence
	    افتراضي:  أرجع -1; // Unknown Result
	}
	بدل (*p++)
	{
	    حالة 'R': button |= 4; اكسر;
	    حالة '-': اكسر;
	    حالة 'x': اكسر; // ignore sequence
	    افتراضي:  أرجع -1; // Unknown Result
	}
	status = *p++;
	لكل (val = 0, mult = 100, iter = 0; iter < 3; iter++,
		mult /= 10, p++)
	    إذا (*p >= '0' && *p <= '9')
		val += (*p - '0') * mult;
	    وإلا
		أرجع -1;
	mouse_col = val;
	لكل (val = 0, mult = 100, iter = 0; iter < 3; iter++,
		mult /= 10, p++)
	    إذا (*p >= '0' && *p <= '9')
		val += (*p - '0') * mult;
	    وإلا
		أرجع -1;
	mouse_row = val;
	بدل (*p++)
	{
	    حالة 's': button |= 8; اكسر;  // SHIFT key Pressed
	    حالة '-': اكسر;  // Not Pressed
	    حالة 'x': اكسر;  // Not Reporting
	    افتراضي:  أرجع -1; // Unknown Result
	}
	بدل (*p++)
	{
	    حالة 'c': button |= 16; اكسر;  // CTRL key Pressed
	    حالة '-': اكسر;  // Not Pressed
	    حالة 'x': اكسر;  // Not Reporting
	    افتراضي:  أرجع -1; // Unknown Result
	}
	إذا (*p++ != '\033')
	    أرجع -1;
	إذا (*p++ != '\\')
	    أرجع -1;
	بدل (status)
	{
	    حالة 'D': // Double Click
	    حالة 'S': // Single Click
		إذا (button & 1) mouse_code |= MOUSE_چەپ;
		إذا (button & 2) mouse_code |= MOUSE_ناوەڕاست;
		إذا (button & 4) mouse_code |= MOUSE_ڕاست;
		إذا (button & 8) mouse_code |= MOUSE_SHIFT;
		إذا (button & 16) mouse_code |= MOUSE_CTRL;
		اكسر;
	    حالة 'm': // Mouse move
		إذا (button & 1) mouse_code |= MOUSE_چەپ;
		إذا (button & 2) mouse_code |= MOUSE_ناوەڕاست;
		إذا (button & 4) mouse_code |= MOUSE_ڕاست;
		إذا (button & 8) mouse_code |= MOUSE_SHIFT;
		إذا (button & 16) mouse_code |= MOUSE_CTRL;
		إذا ((button & 7) != 0)
		{
		    held_button = mouse_code;
		    mouse_code |= MOUSE_DRAG;
		}
		is_drag = TRUE;
		showmode();
		اكسر;
	    حالة 'd': // Button Down
		إذا (button & 1) mouse_code |= MOUSE_چەپ;
		إذا (button & 2) mouse_code |= MOUSE_ناوەڕاست;
		إذا (button & 4) mouse_code |= MOUSE_ڕاست;
		إذا (button & 8) mouse_code |= MOUSE_SHIFT;
		إذا (button & 16) mouse_code |= MOUSE_CTRL;
		اكسر;
	    حالة 'u': // Button Up
		is_release = TRUE;
		إذا (button & 1)
		    mouse_code |= MOUSE_چەپ;
		إذا (button & 2)
		    mouse_code |= MOUSE_ناوەڕاست;
		إذا (button & 4)
		    mouse_code |= MOUSE_ڕاست;
		إذا (button & 8)
		    mouse_code |= MOUSE_SHIFT;
		إذا (button & 16)
		    mouse_code |= MOUSE_CTRL;
		اكسر;
	    افتراضي: أرجع -1; // Unknown Result
	}

	*slen += (p - (tp + *slen));
    }
#endif // FEAT_MOUSE_JSB
#ifdef FEAT_MOUSE_DEC
    إذا (کلیل_name[0] == KS_DEC_MOUSE)
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
	صحيح Pe, Pb, Pr, Pc;

	p = tp + *slen;

	// get event status
	Pe = getdigits(&p);
	إذا (*p++ != ';')
	    أرجع -1;

	// get button status
	Pb = getdigits(&p);
	إذا (*p++ != ';')
	    أرجع -1;

	// get row status
	Pr = getdigits(&p);
	إذا (*p++ != ';')
	    أرجع -1;

	// get column status
	Pc = getdigits(&p);

	// the page parameter is optional
	إذا (*p == ';')
	{
	    p++;
	    (فراغ)getdigits(&p);
	}
	إذا (*p++ != '&')
	    أرجع -1;
	إذا (*p++ != 'w')
	    أرجع -1;

	mouse_code = 0;
	بدل (Pe)
	{
	    حالة  0: أرجع -1; // position request while unavailable
	    حالة  1: // a response to a locator position request includes
		     //	the status of all buttons
		     Pb &= 7;   // mask off and ignore fourth button
		     إذا (Pb & 4)
			 mouse_code  = MOUSE_چەپ;
		     إذا (Pb & 2)
			 mouse_code  = MOUSE_ناوەڕاست;
		     إذا (Pb & 1)
			 mouse_code  = MOUSE_ڕاست;
		     إذا (Pb)
		     {
			 held_button = mouse_code;
			 mouse_code |= MOUSE_DRAG;
			 WantQueryMouse = TRUE;
		     }
		     is_drag = TRUE;
		     showmode();
		     اكسر;
	    حالة  2: mouse_code = MOUSE_چەپ;
		     WantQueryMouse = TRUE;
		     اكسر;
	    حالة  3: mouse_code = MOUSE_چەپ;
		     is_release = TRUE;
		     اكسر;
	    حالة  4: mouse_code = MOUSE_ناوەڕاست;
		     WantQueryMouse = TRUE;
		     اكسر;
	    حالة  5: mouse_code = MOUSE_ناوەڕاست;
		     is_release = TRUE;
		     اكسر;
	    حالة  6: mouse_code = MOUSE_ڕاست;
		     WantQueryMouse = TRUE;
		     اكسر;
	    حالة  7: mouse_code = MOUSE_ڕاست;
		     is_release = TRUE;
		     اكسر;
	    حالة  8: أرجع -1; // fourth button down
	    حالة  9: أرجع -1; // fourth button up
	    حالة 10: أرجع -1; // mouse outside of filter rectangle
	    افتراضي: أرجع -1; // should never occur
	}

	mouse_col = Pc - 1;
	mouse_row = Pr - 1;

	*slen += (صحيح)(p - (tp + *slen));
    }
#endif // FEAT_MOUSE_DEC
#ifdef FEAT_MOUSE_PTERM
    إذا (کلیل_name[0] == KS_PTERM_MOUSE)
    {
	صحيح button, ژمارە_clicks, action;

	p = tp + *slen;

	action = getdigits(&p);
	إذا (*p++ != ';')
	    أرجع -1;

	mouse_row = getdigits(&p);
	إذا (*p++ != ';')
	    أرجع -1;
	mouse_col = getdigits(&p);
	إذا (*p++ != ';')
	    أرجع -1;

	button = getdigits(&p);
	mouse_code = 0;

	بدل (button)
	{
	    حالة 4: mouse_code = MOUSE_چەپ; اكسر;
	    حالة 1: mouse_code = MOUSE_ڕاست; اكسر;
	    حالة 2: mouse_code = MOUSE_ناوەڕاست; اكسر;
	    افتراضي: أرجع -1;
	}

	بدل (action)
	{
	    حالة 31: // Initial press
		إذا (*p++ != ';')
		    أرجع -1;

		ژمارە_clicks = getdigits(&p); // Not used
		اكسر;

	    حالة 32: // Release
		is_release = TRUE;
		اكسر;

	    حالة 33: // Drag
		held_button = mouse_code;
		mouse_code |= MOUSE_DRAG;
		اكسر;

	    افتراضي:
		أرجع -1;
	}

	إذا (*p++ != 't')
	    أرجع -1;

	*slen += (p - (tp + *slen));
    }
#endif // FEAT_MOUSE_PTERM

    // Interpret the mouse code
    current_button = (mouse_code & MOUSE_CLICK_MASK);
    إذا (is_release)
	current_button |= MOUSE_RELEASE;

    إذا (current_button == MOUSE_RELEASE
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
	إذا ((mouse_code & MOUSE_DRAG) == MOUSE_DRAG)
	    is_drag = TRUE;
	current_button = held_button;
    }
    وإلا
    {
      إذا (wheel_code == 0)
      {
#ifdef CHECK_DOUBLE_CLICK
# ifdef FEAT_MOUSE_GPM
	/*
	 * Only for Unix, when GUI not active, we handle multi-clicks here, but
	 * not for GPM mouse events.
	 */
#  ifdef FEAT_GUI
	إذا (کلیل_name[0] != KS_GPM_MOUSE && !gui.in_use)
#  وإلا
	    إذا (کلیل_name[0] != KS_GPM_MOUSE)
#  endif
# وإلا
#  ifdef FEAT_GUI
		إذا (!gui.in_use)
#  endif
# endif
		{
		    /*
		     * Compute the time elapsed since the previous mouse click.
		     */
		    gettimeofday(&mouse_time, عدم);
		    إذا (orig_mouse_time.tv_sec == 0)
		    {
			/*
			 * Avoid computing the difference between mouse_time
			 * and orig_mouse_time for the first click, as the
			 * difference would be huge and would cause
			 * multiplication overflow.
			 */
			timediff = p_mouset;
		    }
		    وإلا
			timediff = time_diff_ms(&orig_mouse_time, &mouse_time);
		    orig_mouse_time = mouse_time;
		    إذا (mouse_code == orig_mouse_code
			    && timediff < p_mouset
			    && orig_ژمارە_clicks != 4
			    && orig_mouse_col == mouse_col
			    && orig_mouse_row == mouse_row
			    && (is_mouse_topline(curwin)
				// Double click in tab pages line also works
				// when window contents changes.
				|| (mouse_row == 0 && firstwin->w_winrow > 0))
		       )
			++orig_ژمارە_clicks;
		    وإلا
			orig_ژمارە_clicks = 1;
		    orig_mouse_col = mouse_col;
		    orig_mouse_row = mouse_row;
		    set_mouse_topline(curwin);
		}
# إذا defined(FEAT_GUI) || defined(FEAT_MOUSE_GPM)
		وإلا
		    orig_ژمارە_clicks = ژمارە_MOUSE_CLICKS(mouse_code);
# endif
#وإلا
	orig_ژمارە_clicks = ژمارە_MOUSE_CLICKS(mouse_code);
#endif
	is_click = TRUE;
      }
      orig_mouse_code = mouse_code;
    }
    إذا (!is_drag)
	held_button = mouse_code & MOUSE_CLICK_MASK;

    /*
     * Translate the actual mouse event into a pseudo mouse event.
     * First work out what modifiers are to be used.
     */
    إذا (orig_mouse_code & MOUSE_SHIFT)
	*modifiers |= MOD_MASK_SHIFT;
    إذا (orig_mouse_code & MOUSE_CTRL)
	*modifiers |= MOD_MASK_CTRL;
    إذا (orig_mouse_code & MOUSE_ALT)
	*modifiers |= MOD_MASK_ALT;
    إذا (orig_ژمارە_clicks == 2)
	*modifiers |= MOD_MASK_2CLICK;
    وإلا إذا (orig_ژمارە_clicks == 3)
	*modifiers |= MOD_MASK_3CLICK;
    وإلا إذا (orig_ژمارە_clicks == 4)
	*modifiers |= MOD_MASK_4CLICK;

    // Work out our pseudo mouse event. Note that MOUSE_RELEASE gets added,
    // then it's not mouse up/down.
    کلیل_name[0] = KS_EXTRA;
    إذا (wheel_code != 0 && (!is_release || release_is_ambiguous))
    {
	إذا (wheel_code & MOUSE_CTRL)
	    *modifiers |= MOD_MASK_CTRL;
	إذا (wheel_code & MOUSE_ALT)
	    *modifiers |= MOD_MASK_ALT;

	إذا (wheel_code & 1 && wheel_code & 2)
	    کلیل_name[1] = (صحيح)KE_MOUSELEFT;
	وإلا إذا (wheel_code & 2)
	    کلیل_name[1] = (صحيح)KE_MOUSERIGHT;
	وإلا إذا (wheel_code & 1)
	    کلیل_name[1] = (صحيح)KE_MOUSEUP;
	وإلا
	    کلیل_name[1] = (صحيح)KE_MOUSEDOWN;

	held_button = MOUSE_RELEASE;
    }
    وإلا
    {
#إذا defined(UNIX)
	إذا (use_xterm_mouse() && orig_mouse_code >= MOUSESIDEBUTTONS_نزم)
	    current_button = (current_button) ? MOUSE_X2 : MOUSE_X1;
#endif
	کلیل_name[1] = get_pseudo_mouse_code(current_button, is_click, is_drag);
    }


    // Make sure the mouse position is valid.  Some terminals may return weird
    // values.
    إذا (mouse_col >= Columns)
	mouse_col = Columns - 1;
    إذا (mouse_row >= Rows)
	mouse_row = Rows - 1;

    أرجع 0;
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
    صحيح
mouse_comp_pos(
    win_T	*win,
    صحيح		*rowp,
    صحيح		*colp,
    linenr_T	*lnump,
    صحيح		*plines_cache)
{
    صحيح		col = *colp;
    صحيح		row = *rowp;
    linenr_T	lnum;
    صحيح		retval = FALSE;
    صحيح		off;
    صحيح		ژماردن;

#ifdef FEAT_RIGHTLEFT
    إذا (win->w_p_rl)
	col = win->w_width - 1 - col;
#endif

    lnum = win->w_topline;

    طالما (row > 0)
    {
	صحيح cache_idx = lnum - win->w_topline;

	// Only "Rows" lines are cached, with folding we'll run out of entries
	// and use the slow way.
	إذا (plines_cache != عدم && cache_idx < Rows
						&& plines_cache[cache_idx] > 0)
	    ژماردن = plines_cache[cache_idx];
	وإلا
	{
#ifdef FEAT_DIFF
	    // Don't include filler lines in "count"
	    إذا (win->w_p_diff
# ifdef FEAT_FOLDING
		    && !hasFoldingWin(win, lnum, عدم, عدم, TRUE, عدم)
# endif
		    )
	    {
		إذا (lnum == win->w_topline)
		    row -= win->w_topfill;
		وإلا
		    row -= diff_check_fill(win, lnum);
		ژماردن = plines_win_nofill(win, lnum, FALSE);
	    }
	    وإلا
#endif
		ژماردن = plines_win(win, lnum, FALSE);
	    إذا (plines_cache != عدم && cache_idx < Rows)
		plines_cache[cache_idx] = ژماردن;
	}

	إذا (win->w_skipcol > 0 && lnum == win->w_topline)
	{
	    صحيح width1 = win->w_width - win_col_off(win);

	    إذا (width1 > 0)
	    {
		صحيح skip_هێڵەکان = 0;

		// Adjust for 'smoothscroll' clipping the top screen lines.
		// A similar formula is used in curs_columns().
		إذا (win->w_skipcol > width1)
		    skip_هێڵەکان = (win->w_skipcol - width1)
					    / (width1 + win_col_off2(win)) + 1;
		وإلا إذا (win->w_skipcol > 0)
		    skip_هێڵەکان = 1;

		ژماردن -= skip_هێڵەکان;
	    }
	}

	إذا (ژماردن > row)
	    اكسر;	// Position is in this buffer line.
#ifdef FEAT_FOLDING
	(فراغ)hasFoldingWin(win, lnum, عدم, &lnum, TRUE, عدم);
#endif
	إذا (lnum == win->w_buffer->b_ml.ml_هێڵ_ژماردن)
	{
	    retval = TRUE;
	    اكسر;		// past end of file
	}
	row -= ژماردن;
	++lnum;
    }

    إذا (!retval)
    {
	// Compute the column without wrapping.
	off = win_col_off(win) - win_col_off2(win);
	إذا (col < off)
	    col = off;
	col += row * (win->w_width - off);

	// Add skip column for the topline.
	إذا (lnum == win->w_topline)
	    col += win->w_skipcol;
    }

    إذا (!win->w_p_wrap)
	col += win->w_leftcol;

    // skip line number and fold column in front of the line
    col -= win_col_off(win);
    إذا (col <= 0)
    {
#ifdef FEAT_NETBEANS_INTG
	// if mouse is clicked on the gutter, then inform the netbeans server
	إذا (*colp < win_col_off(win))
	    netbeans_gutter_click(lnum);
#endif
	col = 0;
    }

    *colp = col;
    *rowp = row;
    *lnump = lnum;
    أرجع retval;
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
mouse_find_win(صحيح *rowp, صحيح *colp, mouse_find_T popup UNUSED)
{
    frame_T	*fp;
    win_T	*wp;

#ifdef FEAT_PROP_POPUP
    win_T	*pwp = عدم;

    إذا (popup != IGNORE_POPUP)
    {
	popup_reset_handled(POPUP_HANDLED_1);
	طالما ((wp = find_next_popup(TRUE, POPUP_HANDLED_1)) != عدم)
	{
	    إذا (*rowp >= wp->w_winrow && *rowp < wp->w_winrow + popup_height(wp)
		    && *colp >= wp->w_wincol
				    && *colp < wp->w_wincol + popup_width(wp))
		pwp = wp;
	}
	إذا (pwp != عدم)
	{
	    إذا (popup == FAIL_POPUP)
		أرجع عدم;
	    *rowp -= pwp->w_winrow;
	    *colp -= pwp->w_wincol;
	    أرجع pwp;
	}
    }
#endif

    fp = topframe;

    إذا (*colp < firstwin->w_wincol
	    || *colp >= firstwin->w_wincol + fp->fr_width
	    || *rowp < firstwin->w_winrow)
	أرجع عدم;

    *rowp -= firstwin->w_winrow;
    *colp -= firstwin->w_wincol;
    لكل (;;)
    {
	إذا (fp->fr_layout == FR_LEAF)
	    اكسر;
	إذا (fp->fr_layout == FR_ROW)
	{
	    لكل (fp = fp->fr_child; fp->fr_next != عدم; fp = fp->fr_next)
	    {
		إذا (*colp < fp->fr_width)
		    اكسر;
		*colp -= fp->fr_width;
	    }
	}
	وإلا    // fr_layout == FR_COL
	{
	    لكل (fp = fp->fr_child; fp->fr_next != عدم; fp = fp->fr_next)
	    {
		إذا (*rowp < fp->fr_height)
		    اكسر;
		*rowp -= fp->fr_height;
	    }
	}
    }
    // When using a timer that closes a window the window might not actually
    // exist.
    FOR_ALL_WINDOWS(wp)
	إذا (wp == fp->fr_win)
	{
#ifdef FEAT_MENU
	    *rowp -= wp->w_winbar_height;
#endif
	    أرجع wp;
	}
    أرجع عدم;
}

#إذا defined(NEED_VCOL2COL) || defined(FEAT_BEVAL) || defined(FEAT_PROP_POPUP) \
	|| defined(FEAT_EVAL)
/*
 * Convert a virtual (screen) column to a character column.
 * The first column is zero.
 */
    صحيح
vcol2col(win_T *wp, linenr_T lnum, صحيح vcol, colnr_T *coladdp)
{
    char_u	    *هێڵ;
    chartabsize_T   cts;

    // try to advance to the specified column
    هێڵ = ml_get_buf(wp->w_buffer, lnum, FALSE);
    init_chartabsize_arg(&cts, wp, lnum, 0, هێڵ, هێڵ);
    طالما (cts.cts_vcol < vcol && *cts.cts_ptr != NUL)
    {
	صحيح size = win_lbr_chartabsize(&cts, عدم, عدم);
	إذا (cts.cts_vcol + size > vcol)
	    اكسر;
	cts.cts_vcol += size;
	MB_PTR_ADV(cts.cts_ptr);
    }
    clear_chartabsize_arg(&cts);

    إذا (coladdp != عدم)
	*coladdp = vcol - cts.cts_vcol;
    أرجع (صحيح)(cts.cts_ptr - هێڵ);
}
#endif

#إذا defined(FEAT_EVAL)
/*
 * "getmousepos()" function.
 */
    فراغ
f_getmousepos(typval_T *argvars UNUSED, typval_T *rettv)
{
    dict_T	*d;
    win_T	*wp;
    صحيح		row = mouse_row;
    صحيح		col = mouse_col;
    varnumber_T winid = 0;
    varnumber_T winrow = 0;
    varnumber_T wincol = 0;
    linenr_T	lnum = 0;
    varnumber_T column = 0;
    colnr_T	coladd = 0;

    إذا (rettv_dict_alloc(rettv) == FAIL)
	أرجع;
    d = rettv->vval.v_dict;

    dict_add_ژمار(d, "screenrow", (varnumber_T)mouse_row + 1);
    dict_add_ژمار(d, "screencol", (varnumber_T)mouse_col + 1);

    wp = mouse_find_win(&row, &col, FIND_POPUP);
    إذا (wp != عدم)
    {
	صحيح	لووتکە_off = 0;
	صحيح	چەپ_off = 0;
	صحيح	height = wp->w_height + wp->w_status_height;

# ifdef FEAT_PROP_POPUP
	إذا (WIN_IS_POPUP(wp))
	{
	    لووتکە_off = popup_لووتکە_extra(wp);
	    چەپ_off = popup_چەپ_extra(wp);
	    height = popup_height(wp);
	}
# endif
	إذا (row < height)
	{
	    winid = wp->w_id;
	    winrow = row + 1;
	    wincol = col + 1;
	    row -= لووتکە_off;
	    col -= چەپ_off;
	    إذا (row >= 0 && row < wp->w_height && col >= 0 && col < wp->w_width)
	    {
		(فراغ)mouse_comp_pos(wp, &row, &col, &lnum, عدم);
		col = vcol2col(wp, lnum, col, &coladd);
		column = col + 1;
	    }
	}
    }
    dict_add_ژمار(d, "winid", winid);
    dict_add_ژمار(d, "winrow", winrow);
    dict_add_ژمار(d, "wincol", wincol);
    dict_add_ژمار(d, "line", (varnumber_T)lnum);
    dict_add_ژمار(d, "column", column);
    dict_add_ژمار(d, "coladd", coladd);
}
#endif
