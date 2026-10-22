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

#अन्तर्भाव "vim.h"

/*
 * Horiziontal and vertical steps used when scrolling.
 * When negative scroll by a whole page.
 */
स्थिर दीर्घ mouse_hor_step = 6;
स्थिर दीर्घ mouse_vert_step = 3;
स्थिर win_T *dragwin = रिक्त;	// window being dragged
स्थिर पूर्णांक stl_click_handler(win_T *wp, पूर्णांक mrow, पूर्णांक mcol, पूर्णांक which_button,
								    पूर्णांक mods);
स्थिर पूर्णांक stl_click_handler_regions(stl_click_region_T *regions,
				    पूर्णांक region_मोजणी, पूर्णांक winid,
				    char_u *area_name, पूर्णांक mrow, पूर्णांक mcol,
				    पूर्णांक which_button, पूर्णांक mods);

    शून्य
mouse_set_vert_scroll_step(दीर्घ step)
{
    mouse_vert_step = step;
}

    शून्य
mouse_set_hor_scroll_step(दीर्घ step)
{
    mouse_hor_step = step;
}

#ifdef CHECK_DOUBLE_CLICK
/*
 * Return the duration from t1 to t2 in milliseconds.
 */
    स्थिर दीर्घ
time_diff_ms(संरचना timeval *t1, संरचना timeval *t2)
{
    // This handles wrapping of tv_usec correctly without any special case.
    // Example of 2 pairs (tv_sec, tv_usec) with a duration of 5 ms:
    //	   t1 = (1, 998000) t2 = (2, 3000) gives:
    //	   (2 - 1) * 1000 + (3000 - 998000) / 1000 -> 5 ms.
    परत (t2->tv_sec - t1->tv_sec) * 1000
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
    स्थिर पूर्णांक
get_mouse_class(char_u *p)
{
    पूर्णांक		c;

    जर (has_mbyte && MB_BYTE2LEN(p[0]) > 1)
	परत mb_get_class(p);

    c = *p;
    जर (c == ' ' || c == '\t')
	परत 0;

    जर (vim_iswordc(c))
	परत 2;

    // There are a few special cases where we want certain combinations of
    // characters to be considered as a single word.  These are things like
    // "->", "/ *", "*=", "+=", "&=", "<=", ">=", "!=" etc.  Otherwise, each
    // character is in its own class.
    जर (c != NUL && vim_strchr((char_u *)"-+*/%<>&|^!=", c) != रिक्त)
	परत 1;
    परत c;
}

/*
 * Move "pos" back to the start of the word it's in.
 */
    स्थिर शून्य
find_start_of_word(pos_T *pos)
{
    char_u	*ओळ;
    पूर्णांक		cclass;
    पूर्णांक		col;

    ओळ = ml_get(pos->lnum);
    cclass = get_mouse_class(ओळ + pos->col);

    पर्यंत (pos->col > 0)
    {
	col = pos->col - 1;
	col -= (*mb_head_off)(ओळ, ओळ + col);
	जर (get_mouse_class(ओळ + col) != cclass)
	    थांबा;
	pos->col = col;
    }
}

/*
 * Move "pos" forward to the end of the word it's in.
 * When 'selection' is "exclusive", the position is just after the word.
 */
    स्थिर शून्य
find_end_of_word(pos_T *pos)
{
    char_u	*ओळ;
    पूर्णांक		cclass;
    पूर्णांक		col;

    ओळ = ml_get(pos->lnum);
    जर (*p_sel == 'e' && pos->col > 0)
    {
	--pos->col;
	pos->col -= (*mb_head_off)(ओळ, ओळ + pos->col);
    }
    cclass = get_mouse_class(ओळ + pos->col);
    पर्यंत (ओळ[pos->col] != NUL)
    {
	col = pos->col + (*mb_ptr2len)(ओळ + pos->col);
	जर (get_mouse_class(ओळ + col) != cclass)
	{
	    जर (*p_sel == 'e')
		pos->col = col;
	    थांबा;
	}
	pos->col = col;
    }
}

#जर defined(FEAT_GUI_MOTIF) || defined(FEAT_GUI_GTK) \
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
    स्थिर पूर्णांक
get_fpos_of_mouse(pos_T *mpos)
{
    win_T	*wp;
    पूर्णांक		row = mouse_row;
    पूर्णांक		col = mouse_col;

    जर (row < 0 || col < 0)		// check if it makes sense
	परत IN_UNKNOWN;

    // find the window where the row is in
    wp = mouse_find_win(&row, &col, FAIL_POPUP);
    जर (wp == रिक्त)
	परत IN_UNKNOWN;
    // winpos and height may change in win_enter()!
    जर (row >= wp->w_height)	// In (or below) status line
	परत IN_STATUS_ओळ;
    जर (col >= wp->w_width)	// In vertical separator line
	परत IN_SEP_ओळ;

    जर (wp != curwin)
	परत IN_UNKNOWN;

    // compute the position in the buffer line from the posn on the screen
    जर (mouse_comp_pos(curwin, &row, &col, &mpos->lnum, रिक्त))
	परत IN_STATUS_ओळ; // past bottom

    mpos->col = vcol2col(wp, mpos->lnum, col, &mpos->coladd);
    परत IN_BUFFER;
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
    पूर्णांक
do_mouse(
    oparg_T	*oap,		// operator argument, can be NULL
    पूर्णांक		c,		// K_LEFTMOUSE, etc
    पूर्णांक		dir,		// Direction to 'put' if necessary
    दीर्घ	मोजणी,
    पूर्णांक		fixindent)	// PUT_FIXINDENT if fixing indent necessary
{
    स्थिर पूर्णांक	do_always = FALSE;	// ignore 'mouse' setting next time
    स्थिर पूर्णांक	got_click = FALSE;	// got a click some time back

    पूर्णांक		which_button;	// MOUSE_LEFT, _MIDDLE or _RIGHT
    पूर्णांक		is_click = FALSE; // If FALSE it's a drag or release event
    पूर्णांक		is_drag = FALSE;  // If TRUE it's a drag event
    पूर्णांक		jump_flags = 0;	// flags for jump_to_mouse()
    pos_T	start_visual;
    पूर्णांक		moved;		// Has cursor moved?
    पूर्णांक		in_status_ओळ;	// mouse in status line
    स्थिर पूर्णांक	in_tab_ओळ = FALSE; // mouse clicked in tab line
    स्थिर पूर्णांक	in_tabpanel = FALSE; // mouse clicked in tabpanel
#ifdef FEAT_TABPANEL
    स्थिर bool	in_tabpanel_scrollbar = false; // dragging tabpanel scrollbar
#endif
    पूर्णांक		in_sep_ओळ;	// mouse in vertical separator line
    पूर्णांक		c1, c2;
#जर defined(FEAT_FOLDING)
    pos_T	save_cursor;
#endif
    win_T	*old_curwin = curwin;
    स्थिर pos_T orig_cursor;
    colnr_T	leftcol, rightcol;
    pos_T	end_visual;
    पूर्णांक		diff;
    पूर्णांक		old_active = VIsual_active;
    पूर्णांक		old_mode = VIsual_mode;
    पूर्णांक		regname;

#जर defined(FEAT_FOLDING)
    save_cursor = curwin->w_cursor;
#endif

    // When GUI is active, always recognize mouse events, otherwise:
    // - Ignore mouse event in normal mode if 'mouse' doesn't include 'n'.
    // - Ignore mouse event in visual mode if 'mouse' doesn't include 'v'.
    // - For command line and insert mode 'mouse' is checked before calling
    //	 do_mouse().
    जर (do_always)
	do_always = FALSE;
    अन्यथा
#ifdef FEAT_GUI
	जर (!gui.in_use)
#endif
	{
	    जर (VIsual_active)
	    {
		जर (!mouse_has(MOUSE_VISUAL))
		    परत FALSE;
	    }
	    अन्यथा जर (State == MODE_NORMAL && !mouse_has(MOUSE_NORMAL))
		परत FALSE;
	}

    साठी (;;)
    {
	which_button = get_mouse_button(KEY2TERMCAP1(c), &is_click, &is_drag);
	जर (is_drag)
	{
	    // If the next character is the same mouse event then use that
	    // one. Speeds up dragging the status line.
	    // Note: Since characters added to the stuff buffer in the code
	    // below need to come before the next character, do not do this
	    // when the current character was stuffed.
	    जर (!किल्ली_Stuffed && vpeekc() != NUL)
	    {
		पूर्णांक nc;
		पूर्णांक save_mouse_row = mouse_row;
		पूर्णांक save_mouse_col = mouse_col;

		// Need to get the character, peeking doesn't get the actual
		// one.
		nc = safe_vgetc();
		जर (c == nc)
		    सुरूठेवा;
		vungetc(nc);
		mouse_row = save_mouse_row;
		mouse_col = save_mouse_col;
	    }
	}
	थांबा;
    }

    जर (c == K_MOUSEMOVE)
    {
	// Mouse moved without a button pressed.
#ifdef FEAT_BEVAL_TERM
	ui_may_remove_balloon();
	जर (p_bevalterm)
	{
	    profile_setlimit(p_bdlay, &bevalexpr_due);
	    bevalexpr_due_set = TRUE;
	}
#endif
#ifdef FEAT_PROP_POPUP
	popup_handle_mouse_moved();
#endif
	परत FALSE;
    }

#ifdef FEAT_MOUSESHAPE
    // May have stopped dragging the status or separator line.  The pointer is
    // most likely still on the status or separator line.
    जर (!is_drag && drag_status_ओळ)
    {
	drag_status_ओळ = FALSE;
	update_mouseshape(SHAPE_IDX_STATUS);
    }
    जर (!is_drag && drag_sep_ओळ)
    {
	drag_sep_ओळ = FALSE;
	update_mouseshape(SHAPE_IDX_VSEP);
    }
#endif

    // Ignore drag and release events if we didn't get a click.
    जर (is_click)
    {
	got_click = TRUE;
	in_tab_ओळ = FALSE;
	in_tabpanel = FALSE;
#ifdef FEAT_TABPANEL
	in_tabpanel_scrollbar = false;
#endif
    }
    अन्यथा
    {
	जर (!got_click)			// didn't get click, ignore
	    परत FALSE;
	जर (!is_drag)			// release, reset got_click
	{
	    got_click = FALSE;
	    जर (in_tab_ओळ || in_tabpanel
#ifdef FEAT_TABPANEL
		    || in_tabpanel_scrollbar
#endif
		    )
	    {
		in_tab_ओळ = FALSE;
		in_tabpanel = FALSE;
#ifdef FEAT_TABPANEL
		in_tabpanel_scrollbar = false;
#endif
		परत FALSE;
	    }
	}
    }

#ifdef FEAT_TABPANEL
    // Continue a scrollbar drag before any tab-selection handling.
    जर (is_drag && in_tabpanel_scrollbar)
    {
	tabpanel_drag_scrollbar(mouse_row);
	परत FALSE;
    }
#endif

    // CTRL right mouse button does CTRL-T
    जर (is_click && (mod_mask & MOD_MASK_CTRL) && which_button == MOUSE_उजवा)
    {
	जर (State & MODE_घाला)
	    stuffcharReadbuff(Ctrl_O);
	जर (मोजणी > 1)
	    stuffnumReadbuff(मोजणी);
	stuffcharReadbuff(Ctrl_T);
	got_click = FALSE;		// ignore drag&release now
	परत FALSE;
    }

    // CTRL only works with left mouse button
    जर ((mod_mask & MOD_MASK_CTRL) && which_button != MOUSE_डावा)
	परत FALSE;

    // When a modifier is down, ignore drag and release events, as well as
    // multiple clicks and the middle mouse button.
    // Accept shift-leftmouse drags when 'mousemodel' is "popup.*".
    जर ((mod_mask & (MOD_MASK_SHIFT | MOD_MASK_CTRL | MOD_MASK_ALT
							     | MOD_MASK_META))
	    && (!is_click
		|| (mod_mask & MOD_MASK_MULTI_CLICK)
		|| which_button == MOUSE_मधला)
	    && !((mod_mask & (MOD_MASK_SHIFT|MOD_MASK_ALT))
		&& mouse_model_popup()
		&& which_button == MOUSE_डावा)
	    && !((mod_mask & MOD_MASK_ALT)
		&& !mouse_model_popup()
		&& which_button == MOUSE_उजवा)
	    )
	परत FALSE;

    // If the button press was used as the movement command for an operator
    // (eg "d<MOUSE>"), or it is the middle button that is held down, ignore
    // drag/release events.
    जर (!is_click && which_button == MOUSE_मधला)
	परत FALSE;

    जर (oap != रिक्त)
	regname = oap->regname;
    अन्यथा
	regname = 0;

    // Middle mouse button does a 'put' of the selected text
    जर (which_button == MOUSE_मधला)
    {
	जर (State == MODE_NORMAL)
	{
	    // If an operator was pending, we don't know what the user wanted
	    // to do. Go back to normal mode: Clear the operator and beep().
	    जर (oap != रिक्त && oap->op_type != OP_NOP)
	    {
		clearopbeep(oap);
		परत FALSE;
	    }

	    // If visual was active, yank the highlighted text and put it
	    // before the mouse pointer position.
	    // In Select mode replace the highlighted text with the clipboard.
	    जर (VIsual_active)
	    {
		जर (VIsual_select)
		{
		    stuffcharReadbuff(Ctrl_G);
		    stuffReadbuff((char_u *)"\"+p");
		}
		अन्यथा
		{
		    stuffcharReadbuff('y');
		    stuffcharReadbuff(K_MIDDLEMOUSE);
		}
		do_always = TRUE;	// ignore 'mouse' setting next time
		परत FALSE;
	    }
	    // The rest is below jump_to_mouse()
	}

	अन्यथा जर ((State & MODE_घाला) == 0)
	    परत FALSE;

	// Middle click in insert mode doesn't move the mouse, just insert the
	// contents of a register.  '.' register is special, can't insert that
	// with do_put().
	// Also paste at the cursor if the current mode isn't in 'mouse' (only
	// happens for the GUI).
	जर ((State & MODE_घाला) || !mouse_has(MOUSE_NORMAL))
	{
	    जर (regname == '.')
		घाला_reg(regname, TRUE);
	    अन्यथा
	    {
#ifdef FEAT_CLIPBOARD
		जर (clip_star.available && regname == 0)
		    regname = '*';
#endif
		जर ((State & REPLACE_FLAG) && !yank_register_mline(regname))
		    घाला_reg(regname, TRUE);
		अन्यथा
		{
		    do_put(regname, रिक्त, BACKWARD, 1L,
						      fixindent | PUT_CURSEND);

		    // Repeat it with CTRL-R CTRL-O r or CTRL-R CTRL-P r
		    AppendCharToRedobuff(Ctrl_R);
		    AppendCharToRedobuff(fixindent ? Ctrl_P : Ctrl_O);
		    AppendCharToRedobuff(regname == 0 ? '"' : regname);
		}
	    }
	    परत FALSE;
	}
    }

    // When dragging or button-up stay in the same window.
    जर (!is_click)
	jump_flags |= MOUSE_FOCUS | MOUSE_DID_MOVE;

    start_visual.lnum = 0;

    संरचना tabpage_label_info {
	bool is_panel;	    // label type. true: tabpanel, false: tab line
	bool just_in;	    // just in tabpage label area
	bool just_click;    // just click tabpage label area
	पूर्णांक nr;		    // tabpage number
    } tp_label = { false, false, false, 0 };

    // Check for clicking in the tab page panel.
#जर defined(FEAT_TABPANEL)
    जर (mouse_col < firstwin->w_wincol
		|| mouse_col >= firstwin->w_wincol + topframe->fr_width)
    {
	// A click on the scrollbar column starts a drag interaction and
	// preempts tab-selection.
	जर (is_click && !is_drag && mouse_on_tabpanel_scrollbar())
	{
	    in_tabpanel_scrollbar = TRUE;
	    tabpanel_drag_scrollbar(mouse_row);
	    परत FALSE;
	}

	// Dispatch 'tabpanel' %[FuncName] click regions before falling through
	// to tab-page selection.  On drag events fall through to the normal
	// tab-drag handling.
	जर (is_click && !is_drag
		&& stl_click_handler_regions(tabpanel_stl_click,
					    tabpanel_stl_click_मोजणी,
					    0, (char_u *)"tabpanel",
					    mouse_row, mouse_col,
					    which_button, mod_mask))
	    परत FALSE;

	tp_label.is_panel = true;
	tp_label.just_in = true;
	tp_label.nr = get_tabpagenr_on_tabpanel();

	// click in a tab selects that tab page
	जर (is_click && cmdwin_type == 0)
	    tp_label.just_click = true;
    }
    अन्यथा
#endif
    // Check for clicking in the tab page line.
    जर (TabPageIdxs != रिक्त && mouse_row == 0 && firstwin->w_winrow > 0)
    {
	// Dispatch 'tabline' %[FuncName] click regions before falling through
	// to tab-page selection.  On drag events fall through to the normal
	// tab-drag handling.
	जर (is_click && !is_drag
		&& stl_click_handler_regions(tabline_stl_click,
					    tabline_stl_click_मोजणी,
					    0, (char_u *)"tabline",
					    mouse_row, mouse_col,
					    which_button, mod_mask))
	    परत FALSE;

	tp_label.just_in = true;
	tp_label.nr = TabPageIdxs[mouse_col];

	// click in a tab selects that tab page
	जर (is_click && cmdwin_type == 0
		&& mouse_col < firstwin->w_wincol + topframe->fr_width)
	    tp_label.just_click = true;
    }

    जर (tp_label.just_in)
    {
	जर (is_drag)
	{
	    जर (in_tabpanel || in_tab_ओळ)
	    {
		c1 = tp_label.nr;
		tabpage_move(c1 <= 0 ? 9999 : c1 < tabpage_index(curtab)
							    ? c1 - 1 : c1);
	    }
	    परत FALSE;
	}

	जर (tp_label.just_click)
	{
	    जर (tp_label.is_panel)
		in_tabpanel = TRUE;
	    अन्यथा
		in_tab_ओळ = TRUE;
	    c1 = tp_label.nr;
	    जर (c1 >= 0)
	    {
		जर ((mod_mask & MOD_MASK_MULTI_CLICK) == MOD_MASK_2CLICK
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
		अन्यथा
		{
		    // Go to specified tab page, or next one if not clicking
		    // on a label.
		    goto_tabpage(c1);

		    // It's like clicking on the status line of a window.
		    जर (curwin != old_curwin)
			end_visual_mode_keep_button();
		}
	    }
	    अन्यथा
	    {
		tabpage_T	*tp;

		// Close the current or specified tab page.
		जर (c1 == -999)
		    tp = curtab;
		अन्यथा
		    tp = find_tabpage(-c1);
		जर (tp == curtab)
		{
		    जर (first_tabpage->tp_next != रिक्त)
			tabpage_close(FALSE);
		}
		अन्यथा जर (tp != रिक्त)
		    tabpage_close_other(tp, FALSE);
	    }
	}
	परत TRUE;
    }
    अन्यथा जर (is_drag && (in_tabpanel || (in_tab_ओळ && TabPageIdxs != रिक्त)))
    {
#जर defined(FEAT_TABPANEL)
	जर (in_tabpanel)
	    c1 = get_tabpagenr_on_tabpanel();
	अन्यथा
#endif
	    c1 = TabPageIdxs[mouse_col];
	tabpage_move(c1 <= 0 ? 9999 : c1 - 1);
	परत FALSE;
    }

    // When 'mousemodel' is "popup" or "popup_setpos", translate mouse events:
    // right button up   -> pop-up menu
    // shift-left button -> right button
    // alt-left button   -> alt-right button
    जर (mouse_model_popup())
    {
	जर (which_button == MOUSE_उजवा
			    && !(mod_mask & (MOD_MASK_SHIFT | MOD_MASK_CTRL)))
	{
#ifdef USE_POPUP_SETPOS
# ifdef FEAT_GUI
	    जर (gui.in_use)
	    {
#  जर defined(FEAT_GUI_MOTIF) || defined(FEAT_GUI_GTK) \
			  || defined(FEAT_GUI_PHOTON)
		जर (!is_click)
		    // Ignore right button release events, only shows the popup
		    // menu on the button down event.
		    परत FALSE;
#  endif
#  जर defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_HAIKU)
		जर (is_click || is_drag)
		    // Ignore right button down and drag mouse events.  Windows
		    // only shows the popup menu on the button up event.
		    परत FALSE;
#  endif
	    }
# endif
# जर defined(FEAT_GUI) && defined(FEAT_TERM_POPUP_MENU)
	    अन्यथा
# endif
# जर defined(FEAT_TERM_POPUP_MENU)
	    जर (!is_click)
		// Ignore right button release events, only shows the popup
		// menu on the button down event.
		परत FALSE;
# endif

	    jump_flags = 0;
	    जर (STRCMP(p_mousem, "popup_setpos") == 0)
	    {
		// First set the cursor position before showing the popup
		// menu.
		जर (VIsual_active)
		{
		    pos_T    m_pos;

		    // set MOUSE_MAY_STOP_VIS if we are outside the
		    // selection or the current window (might have false
		    // negative here)
		    जर (mouse_row < curwin->w_winrow
			 || mouse_row
				  > (curwin->w_winrow + curwin->w_height))
			jump_flags = MOUSE_MAY_STOP_VIS;
		    अन्यथा जर (get_fpos_of_mouse(&m_pos) != IN_BUFFER)
			jump_flags = MOUSE_MAY_STOP_VIS;
		    अन्यथा
		    {
			जर (VIsual_mode == 'V')
			{
			    जर ((curwin->w_cursor.lnum <= VIsual.lnum
				    && (m_pos.lnum < curwin->w_cursor.lnum
					|| VIsual.lnum < m_pos.lnum))
				|| (VIsual.lnum < curwin->w_cursor.lnum
				    && (m_pos.lnum < VIsual.lnum
					|| curwin->w_cursor.lnum < m_pos.lnum)))
			    {
				jump_flags = MOUSE_MAY_STOP_VIS;
			    }
			}
			अन्यथा जर ((LTOREQ_POS(curwin->w_cursor, VIsual)
				    && (LT_POS(m_pos, curwin->w_cursor)
					|| LT_POS(VIsual, m_pos)))
				|| (LT_POS(VIsual, curwin->w_cursor)
				    && (LT_POS(m_pos, VIsual)
				      || LT_POS(curwin->w_cursor, m_pos))))
			{
			    jump_flags = MOUSE_MAY_STOP_VIS;
			}
			अन्यथा जर (VIsual_mode == Ctrl_V)
			{
			    getvcols(curwin, &curwin->w_cursor, &VIsual,
						     &leftcol, &rightcol, 0);
			    getvcol(curwin, &m_pos, रिक्त, &m_pos.col, रिक्त, 0);
			    जर (m_pos.col < leftcol || m_pos.col > rightcol)
				jump_flags = MOUSE_MAY_STOP_VIS;
			}
		    }
		}
		अन्यथा
		    jump_flags = MOUSE_MAY_STOP_VIS;
	    }
	    जर (jump_flags)
	    {
		jump_flags = jump_to_mouse(jump_flags, रिक्त, which_button);
		update_curbuf(VIsual_active ? UPD_INVERTED : UPD_VALID);
		setcursor();
		out_flush();    // Update before showing popup menu
	    }
# ifdef FEAT_MENU
	    show_popupmenu();
	    got_click = FALSE;	// ignore release events
# endif
	    परत (jump_flags & CURSOR_MOVED) != 0;
#अन्यथा
	    परत FALSE;
#endif
	}
	जर (which_button == MOUSE_डावा
				&& (mod_mask & (MOD_MASK_SHIFT|MOD_MASK_ALT)))
	{
	    which_button = MOUSE_उजवा;
	    mod_mask &= ~MOD_MASK_SHIFT;
	}
    }

    जर ((State & (MODE_NORMAL | MODE_घाला))
			    && !(mod_mask & (MOD_MASK_SHIFT | MOD_MASK_CTRL)))
    {
	जर (which_button == MOUSE_डावा)
	{
	    जर (is_click)
	    {
		// stop Visual mode for a left click in a window, but not when
		// on a status line
		जर (VIsual_active)
		    jump_flags |= MOUSE_MAY_STOP_VIS;
	    }
	    अन्यथा जर (mouse_has(MOUSE_VISUAL))
		jump_flags |= MOUSE_MAY_VIS;
	}
	अन्यथा जर (which_button == MOUSE_उजवा)
	{
	    जर (is_click && VIsual_active)
	    {
		// Remember the start and end of visual before moving the
		// cursor.
		जर (LT_POS(curwin->w_cursor, VIsual))
		{
		    start_visual = curwin->w_cursor;
		    end_visual = VIsual;
		}
		अन्यथा
		{
		    start_visual = VIsual;
		    end_visual = curwin->w_cursor;
		}
	    }
	    jump_flags |= MOUSE_FOCUS;
	    जर (mouse_has(MOUSE_VISUAL))
		jump_flags |= MOUSE_MAY_VIS;
	}
    }

    // If an operator is pending, ignore all drags and releases until the
    // next mouse click.
    जर (!is_drag && oap != रिक्त && oap->op_type != OP_NOP)
    {
	got_click = FALSE;
	oap->motion_type = MCHAR;
    }

    // When releasing the button let jump_to_mouse() know.
    जर (!is_click && !is_drag)
	jump_flags |= MOUSE_RELEASED;

    // JUMP!
    jump_flags = jump_to_mouse(jump_flags,
			oap == रिक्त ? रिक्त : &(oap->inclusive), which_button);

#ifdef FEAT_MENU
    // A click in the window toolbar has no side effects.
    जर (jump_flags & MOUSE_WINBAR)
	परत FALSE;
#endif
    moved = (jump_flags & CURSOR_MOVED);
    in_status_ओळ = (jump_flags & IN_STATUS_ओळ);
    in_sep_ओळ = (jump_flags & IN_SEP_ओळ);

    // Check for statusline click handler early, before visual mode or
    // other button-specific handling can interfere.
    जर (in_status_ओळ && is_click && !is_drag
	    && stl_click_handler(dragwin, mouse_row, mouse_col,
						which_button, mod_mask))
    {
#ifdef FEAT_MOUSESHAPE
	जर (!drag_status_ओळ)
	{
	    drag_status_ओळ = TRUE;
	    update_mouseshape(-1);
	}
#endif
	परत FALSE;
    }

#ifdef FEAT_NETBEANS_INTG
    जर (isNetbeansBuffer(curbuf)
			    && !(jump_flags & (IN_STATUS_ओळ | IN_SEP_ओळ)))
    {
	पूर्णांक किल्ली = KEY2TERMCAP1(c);

	जर (किल्ली == (पूर्णांक)KE_LEFTRELEASE || किल्ली == (पूर्णांक)KE_MIDDLERELEASE
					       || किल्ली == (पूर्णांक)KE_RIGHTRELEASE)
	    netbeans_button_release(which_button);
    }
#endif

    // When jumping to another window, clear a pending operator.  That's a bit
    // friendlier than beeping and not jumping to that window.
    जर (curwin != old_curwin && oap != रिक्त && oap->op_type != OP_NOP)
	clearop(oap);

#ifdef FEAT_FOLDING
    जर (mod_mask == 0
	    && !is_drag
	    && (jump_flags & (MOUSE_FOLD_CLOSE | MOUSE_FOLD_OPEN))
	    && which_button == MOUSE_डावा)
    {
	// open or close a fold at this line
	जर (jump_flags & MOUSE_FOLD_OPEN)
	    openFold(curwin->w_cursor.lnum, 1L);
	अन्यथा
	    closeFold(curwin->w_cursor.lnum, 1L);
	// don't move the cursor if still in the same window
	जर (curwin == old_curwin)
	    curwin->w_cursor = save_cursor;
    }
#endif

#जर defined(FEAT_CLIPBOARD)
    जर ((jump_flags & IN_OTHER_WIN) && !VIsual_active &&
	    (clip_star.available || clip_plus.available))
    {
	clip_modeless(which_button, is_click, is_drag);
	परत FALSE;
    }
#endif

    // Set global flag that we are extending the Visual area with mouse
    // dragging; temporarily minimize 'scrolloff'.
    जर (VIsual_active && is_drag && get_scrolloff_मूल्य())
    {
	// In the very first line, allow scrolling one line
	जर (mouse_row == 0)
	    mouse_dragging = 2;
	अन्यथा
	    mouse_dragging = 1;
    }

    // When dragging the mouse above the window, scroll down.
    जर (is_drag && mouse_row < 0 && !in_status_ओळ)
    {
	scroll_redraw(FALSE, 1L);
	mouse_row = 0;
    }

    जर (start_visual.lnum)		// right click in visual mode
    {
       // When ALT is pressed make Visual mode blockwise.
       जर (mod_mask & MOD_MASK_ALT)
	   VIsual_mode = Ctrl_V;

	// In Visual-block mode, divide the area in four, pick up the corner
	// that is in the quarter that the cursor is in.
	जर (VIsual_mode == Ctrl_V)
	{
	    getvcols(curwin, &start_visual, &end_visual,
						    &leftcol, &rightcol, 0);
	    जर (curwin->w_curswant > (leftcol + rightcol) / 2)
		end_visual.col = leftcol;
	    अन्यथा
		end_visual.col = rightcol;
	    जर (curwin->w_cursor.lnum >=
				    (start_visual.lnum + end_visual.lnum) / 2)
		end_visual.lnum = start_visual.lnum;

	    // move VIsual to the right column
	    start_visual = curwin->w_cursor;	    // save the cursor pos
	    curwin->w_cursor = end_visual;
	    coladvance(end_visual.col);
	    VIsual = curwin->w_cursor;
	    curwin->w_cursor = start_visual;	    // restore the cursor
	}
	अन्यथा
	{
	    // If the click is before the start of visual, change the start.
	    // If the click is after the end of visual, change the end.  If
	    // the click is inside the visual, change the closest side.
	    जर (LT_POS(curwin->w_cursor, start_visual))
		VIsual = end_visual;
	    अन्यथा जर (LT_POS(end_visual, curwin->w_cursor))
		VIsual = start_visual;
	    अन्यथा
	    {
		// In the same line, compare column number
		जर (end_visual.lnum == start_visual.lnum)
		{
		    जर (curwin->w_cursor.col - start_visual.col >
				    end_visual.col - curwin->w_cursor.col)
			VIsual = start_visual;
		    अन्यथा
			VIsual = end_visual;
		}

		// In different lines, compare line number
		अन्यथा
		{
		    diff = (curwin->w_cursor.lnum - start_visual.lnum) -
				(end_visual.lnum - curwin->w_cursor.lnum);

		    जर (diff > 0)		// closest to end
			VIsual = start_visual;
		    अन्यथा जर (diff < 0)	// closest to start
			VIsual = end_visual;
		    अन्यथा			// in the middle line
		    {
			जर (curwin->w_cursor.col <
					(start_visual.col + end_visual.col) / 2)
			    VIsual = end_visual;
			अन्यथा
			    VIsual = start_visual;
		    }
		}
	    }
	}
    }
    // If Visual mode started in insert mode, execute "CTRL-O"
    अन्यथा जर ((State & MODE_घाला) && VIsual_active)
	stuffcharReadbuff(Ctrl_O);

    // Middle mouse click: Put text before cursor.
    जर (which_button == MOUSE_मधला)
    {
#ifdef FEAT_CLIPBOARD
	जर (clip_star.available && regname == 0)
	    regname = '*';
#endif
	जर (yank_register_mline(regname))
	{
	    जर (mouse_past_bottom)
		dir = FORWARD;
	}
	अन्यथा जर (mouse_past_eol)
	    dir = FORWARD;

	जर (fixindent)
	{
	    c1 = (dir == BACKWARD) ? '[' : ']';
	    c2 = 'p';
	}
	अन्यथा
	{
	    c1 = (dir == FORWARD) ? 'p' : 'P';
	    c2 = NUL;
	}
	prep_redo(regname, मोजणी, NUL, c1, NUL, c2, NUL);

	// Remember where the paste started, so in edit() Insstart can be set
	// to this position
	जर (restart_edit != 0)
	    where_paste_started = curwin->w_cursor;
	do_put(regname, रिक्त, dir, मोजणी, fixindent | PUT_CURSEND);
    }

#जर defined(FEAT_QUICKFIX)
    // Ctrl-Mouse click or double click in a quickfix window jumps to the
    // error under the mouse pointer.
    अन्यथा जर (((mod_mask & MOD_MASK_CTRL)
		|| (mod_mask & MOD_MASK_MULTI_CLICK) == MOD_MASK_2CLICK)
	    && bt_quickfix(curbuf))
    {
	जर (curwin->w_llist_ref == रिक्त)	// quickfix window
	    do_cmdline_cmd((char_u *)".cc");
	अन्यथा					// location list window
	    do_cmdline_cmd((char_u *)".ll");
	got_click = FALSE;		// ignore drag&release now
    }
#endif

    // Ctrl-Mouse click (or double click in a help window) jumps to the tag
    // under the mouse pointer.
    अन्यथा जर ((mod_mask & MOD_MASK_CTRL) || (curbuf->b_help
		     && (mod_mask & MOD_MASK_MULTI_CLICK) == MOD_MASK_2CLICK))
    {
	जर (State & MODE_घाला)
	    stuffcharReadbuff(Ctrl_O);
	stuffcharReadbuff(Ctrl_RSB);
	got_click = FALSE;		// ignore drag&release now
    }

    // Shift-Mouse click searches for the next occurrence of the word under
    // the mouse pointer
    अन्यथा जर ((mod_mask & MOD_MASK_SHIFT))
    {
	जर ((State & MODE_घाला) || (VIsual_active && VIsual_select))
	    stuffcharReadbuff(Ctrl_O);
	जर (which_button == MOUSE_डावा)
	    stuffcharReadbuff('*');
	अन्यथा	// MOUSE_RIGHT
	    stuffcharReadbuff('#');
    }

    // Handle double clicks, unless on status line
    अन्यथा जर (in_status_ओळ)
    {
#ifdef FEAT_MOUSESHAPE
	जर ((is_drag || is_click) && !drag_status_ओळ)
	{
	    drag_status_ओळ = TRUE;
	    update_mouseshape(-1);
	}
#endif
    }
    अन्यथा जर (in_sep_ओळ)
    {
#ifdef FEAT_MOUSESHAPE
	जर ((is_drag || is_click) && !drag_sep_ओळ)
	{
	    drag_sep_ओळ = TRUE;
	    update_mouseshape(-1);
	}
#endif
    }
    अन्यथा जर ((mod_mask & MOD_MASK_MULTI_CLICK)
				       && (State & (MODE_NORMAL | MODE_घाला))
	     && mouse_has(MOUSE_VISUAL))
    {
	जर (is_click || !VIsual_active)
	{
	    जर (VIsual_active)
		orig_cursor = VIsual;
	    अन्यथा
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
	    जर ((mod_mask & MOD_MASK_MULTI_CLICK) == MOD_MASK_2CLICK)
	    {
		// Double click with ALT pressed makes it blockwise.
		जर (mod_mask & MOD_MASK_ALT)
		    VIsual_mode = Ctrl_V;
		अन्यथा
		    VIsual_mode = 'v';
	    }
	    अन्यथा जर ((mod_mask & MOD_MASK_MULTI_CLICK) == MOD_MASK_3CLICK)
		VIsual_mode = 'V';
	    अन्यथा जर ((mod_mask & MOD_MASK_MULTI_CLICK) == MOD_MASK_4CLICK)
		VIsual_mode = Ctrl_V;
#ifdef FEAT_CLIPBOARD
	    // Make sure the clipboard gets updated.  Needed because start and
	    // end may still be the same, and the selection needs to be owned
	    clip_star.vmode = NUL;
#endif
	}
	// A double click selects a word or a block.
	जर ((mod_mask & MOD_MASK_MULTI_CLICK) == MOD_MASK_2CLICK)
	{
	    pos_T	*pos = रिक्त;
	    पूर्णांक		gc;

	    जर (is_click)
	    {
		// If the character under the cursor (skipping white space) is
		// not a word character, try finding a match and select a (),
		// {}, [], #if/#endif, etc. block.
		end_visual = curwin->w_cursor;
		पर्यंत (gc = gchar_pos(&end_visual), VIM_ISWHITE(gc))
		    inc(&end_visual);
		जर (oap != रिक्त)
		    oap->motion_type = MCHAR;
		जर (oap != रिक्त
			&& VIsual_mode == 'v'
			&& !vim_iswordc(gchar_pos(&end_visual))
			&& EQUAL_POS(curwin->w_cursor, VIsual)
			&& (pos = findmatch(oap, NUL)) != रिक्त)
		{
		    curwin->w_cursor = *pos;
		    जर (oap->motion_type == MLINE)
			VIsual_mode = 'V';
		    अन्यथा जर (*p_sel == 'e')
		    {
			जर (LT_POS(curwin->w_cursor, VIsual))
			    ++VIsual.col;
			अन्यथा
			    ++curwin->w_cursor.col;
		    }
		}
	    }

	    जर (pos == रिक्त && (is_click || is_drag))
	    {
		// When not found a match or when dragging: extend to include
		// a word.
		जर (LT_POS(curwin->w_cursor, orig_cursor))
		{
		    find_start_of_word(&curwin->w_cursor);
		    find_end_of_word(&VIsual);
		}
		अन्यथा
		{
		    find_start_of_word(&VIsual);
		    जर (*p_sel == 'e' && *ml_get_cursor() != NUL)
			curwin->w_cursor.col +=
					 (*mb_ptr2len)(ml_get_cursor());
		    find_end_of_word(&curwin->w_cursor);
		}
	    }
	    curwin->w_set_curswant = true;
	}
	जर (is_click)
	    redraw_curbuf_later(UPD_INVERTED);	// update the inversion
    }
    अन्यथा जर (VIsual_active && !old_active)
    {
	जर (mod_mask & MOD_MASK_ALT)
	    VIsual_mode = Ctrl_V;
	अन्यथा
	    VIsual_mode = 'v';
    }

    // If Visual mode changed show it later.
    जर ((!VIsual_active && old_active && mode_displayed)
	    || (VIsual_active && p_smd && संदेश_silent == 0
				 && (!old_active || VIsual_mode != old_mode)))
	redraw_cmdline = TRUE;

    परत moved;
}

    शून्य
ins_mouse(पूर्णांक c)
{
    pos_T	tpos;
    win_T	*old_curwin = curwin;

#ifdef FEAT_GUI
    // When GUI is active, also move/paste when 'mouse' is empty
    जर (!gui.in_use)
#endif
	जर (!mouse_has(MOUSE_घाला))
	    परत;

    undisplay_dollar();
    tpos = curwin->w_cursor;
    जर (do_mouse(रिक्त, c, BACKWARD, 1L, 0))
    {
	win_T	*new_curwin = curwin;

	जर (curwin != old_curwin && win_valid(old_curwin))
	{
	    // Mouse took us to another window.  We need to go back to the
	    // previous one to stop insert there properly.
	    curwin = old_curwin;
	    curbuf = curwin->w_buffer;
#ifdef FEAT_JOB_CHANNEL
	    जर (bt_prompt(curbuf))
		// Restart Insert mode when re-entering the prompt buffer.
		curbuf->b_prompt_घाला = 'A';
#endif
	}
	start_arrow(curwin == old_curwin ? &tpos : रिक्त);
	जर (curwin != new_curwin && win_valid(new_curwin))
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
    स्थिर शून्य
do_mousescroll(cmdarg_T *cap)
{
    पूर्णांक shift_or_ctrl = mod_mask & (MOD_MASK_SHIFT | MOD_MASK_CTRL);

#ifdef FEAT_TERMINAL
    जर (term_use_loop())
	// This window is a terminal window, send the mouse event there.
	// Set "typed" to FALSE to avoid an endless loop.
	send_किल्ल्या_to_term(curbuf->b_term, cap->cmdchar, mod_mask, FALSE);
    अन्यथा
#endif
    जर (cap->arg == MSCR_UP || cap->arg == MSCR_DOWN)
    {
	// Vertical scrolling
	जर (!(State & MODE_घाला) && (mouse_vert_step < 0 || shift_or_ctrl))
	{
	    // whole page up or down
	    pagescroll(cap->arg == MSCR_UP ? FORWARD : BACKWARD, 1L, FALSE);
	}
	अन्यथा
	{
	    जर (mouse_vert_step < 0 || shift_or_ctrl)
	    {
		// whole page up or down
		cap->count1 = (दीर्घ)(curwin->w_botline - curwin->w_topline);
	    }
	    // Don't scroll more than half the window height.
	    अन्यथा जर (curwin->w_height < mouse_vert_step * 2)
	    {
		cap->count1 = curwin->w_height / 2;
		जर (cap->count1 == 0)
		    cap->count1 = 1;
	    }
	    अन्यथा
	    {
		cap->count1 = mouse_vert_step;
	    }
	    cap->count0 = cap->count1;
	    nv_scroll_ओळ(cap);
	}

#ifdef FEAT_PROP_POPUP
	जर (WIN_IS_POPUP(curwin))
	    popup_set_firstline(curwin);
#endif
    }
    अन्यथा
    {
	// Horizontal scrolling
	दीर्घ step = (mouse_hor_step < 0 || shift_or_ctrl)
					    ? curwin->w_width : mouse_hor_step;
	दीर्घ leftcol = curwin->w_leftcol
				     + (cap->arg == MSCR_उजवा ? -step : step);
	जर (leftcol < 0)
	    leftcol = 0;
	do_mousescroll_horiz((long_u)leftcol);
    }
    may_trigger_win_scrolled_resized();
}

/*
 * Insert mode implementation for scrolling in direction "dir", which is
 * one of the MSCR_ values.
 */
    शून्य
ins_mousescroll(पूर्णांक dir)
{
    cmdarg_T	cap;
    oparg_T	oa;
    CLEAR_FIELD(cap);
    clear_oparg(&oa);
    cap.oap = &oa;
    cap.arg = dir;

#ifdef FEAT_TABPANEL
    जर (mouse_row >= 0 && mouse_col >= 0
	    && (dir == MSCR_UP || dir == MSCR_DOWN)
	    && mouse_on_tabpanel())
    {
	(शून्य)tabpanel_scroll(dir == MSCR_UP ? 1 : -1,
		mouse_vert_step > 0 ? mouse_vert_step : 3);
	परत;
    }
#endif

    निवड (dir)
    {
	स्थिती MSCR_UP:
	    cap.cmdchar = K_MOUSEUP;
	    थांबा;
	स्थिती MSCR_DOWN:
	    cap.cmdchar = K_MOUSEDOWN;
	    थांबा;
	स्थिती MSCR_डावा:
	    cap.cmdchar = K_MOUSELEFT;
	    थांबा;
	स्थिती MSCR_उजवा:
	    cap.cmdchar = K_MOUSERIGHT;
	    थांबा;
	पूर्वनिर्धारित:
	    siemsg("Invalid ins_mousescroll() argument: %d", dir);
    }

    win_T *old_curwin = curwin;
    जर (mouse_row >= 0 && mouse_col >= 0)
    {
	// Find the window at the mouse pointer coordinates.
	// NOTE: Must restore "curwin" to "old_curwin" before returning!
	पूर्णांक row = mouse_row;
	पूर्णांक col = mouse_col;
	curwin = mouse_find_win(&row, &col, FIND_POPUP);
	जर (curwin == रिक्त)
	{
	    curwin = old_curwin;
	    परत;
	}
	curbuf = curwin->w_buffer;
    }

    जर (curwin == old_curwin)
    {
	// Don't scroll the current window if the popup menu is visible.
	जर (pum_visible())
	    परत;

	undisplay_dollar();
    }

    linenr_T	orig_topline = curwin->w_topline;
    colnr_T	orig_leftcol = curwin->w_leftcol;
    pos_T	orig_cursor = curwin->w_cursor;

    // Call the common mouse scroll function shared with other modes.
    do_mousescroll(&cap);

    पूर्णांक did_scroll = (orig_topline != curwin->w_topline
		   || orig_leftcol != curwin->w_leftcol);

    curwin->w_redr_status = true;
    curwin = old_curwin;
    curbuf = curwin->w_buffer;

    // If the window actually scrolled and the popup menu may overlay the
    // window, need to redraw it.
    जर (did_scroll && pum_visible())
    {
	// TODO: Would be more efficient to only redraw the windows that are
	// overlapped by the popup menu.
	redraw_all_later(UPD_NOT_VALID);
	ins_compl_show_pum();
    }

    जर (!EQUAL_POS(curwin->w_cursor, orig_cursor))
    {
	start_arrow(&orig_cursor);
	set_can_cindent(TRUE);
    }
}

#जर defined(FEAT_PROP_POPUP)
/*
 * Command-line mode implementation for scrolling in direction "dir", which is
 * one of the MSCR_ values.  Scrolls the completion info popup when the mouse
 * pointer is on top of it.
 * Returns TRUE when the info popup was scrolled.
 */
    पूर्णांक
cmdline_mousescroll(पूर्णांक dir)
{
    cmdarg_T	cap;
    oparg_T	oa;

    CLEAR_FIELD(cap);
    clear_oparg(&oa);
    cap.oap = &oa;
    cap.arg = dir;

    निवड (dir)
    {
	स्थिती MSCR_UP:	 cap.cmdchar = K_MOUSEUP; थांबा;
	स्थिती MSCR_DOWN:	 cap.cmdchar = K_MOUSEDOWN; थांबा;
	स्थिती MSCR_डावा:	 cap.cmdchar = K_MOUSELEFT; थांबा;
	स्थिती MSCR_उजवा: cap.cmdchar = K_MOUSERIGHT; थांबा;
    }

    जर (mouse_row < 0 || mouse_col < 0)
	परत FALSE;

    पूर्णांक	    row = mouse_row;
    पूर्णांक	    col = mouse_col;
    win_T   *wp;

    // Only scroll when the mouse is on top of the info popup.
    wp = mouse_find_win(&row, &col, FIND_POPUP);
    जर (wp == रिक्त || !WIN_IS_POPUP(wp) || !(wp->w_popup_flags & POPF_INFO)
							|| !wp->w_has_scrollbar)
	परत FALSE;

    win_T   *old_curwin = curwin;

    curwin = wp;
    curbuf = wp->w_buffer;
    // Call the common mouse scroll function shared with other modes.
    do_mousescroll(&cap);
    curwin = old_curwin;
    curbuf = curwin->w_buffer;

    // Cmdline mode doesn't normally call update_screen(), so redraw the
    // completion popup menu, which also repaints the info popup.
    जर (cmdline_pum_active())
	cmdline_pum_display();
    परत TRUE;
}
#endif

/*
 * Return TRUE if "c" is a mouse key.
 */
    पूर्णांक
is_mouse_किल्ली(पूर्णांक c)
{
    परत c == K_LEFTMOUSE
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

स्थिर संरचना mousetable
{
    पूर्णांक	    pseudo_code;	// Code for pseudo mouse event
    पूर्णांक	    button;		// Which mouse button is it?
    पूर्णांक	    is_click;		// Is it a mouse button click event?
    पूर्णांक	    is_drag;		// Is it a mouse drag event?
} mouse_table[] =
{
    {(पूर्णांक)KE_LEFTMOUSE,		MOUSE_डावा,	TRUE,	FALSE},
#ifdef FEAT_GUI
    {(पूर्णांक)KE_LEFTMOUSE_NM,	MOUSE_डावा,	TRUE,	FALSE},
#endif
    {(पूर्णांक)KE_LEFTDRAG,		MOUSE_डावा,	FALSE,	TRUE},
    {(पूर्णांक)KE_LEFTRELEASE,	MOUSE_डावा,	FALSE,	FALSE},
#ifdef FEAT_GUI
    {(पूर्णांक)KE_LEFTRELEASE_NM,	MOUSE_डावा,	FALSE,	FALSE},
#endif
    {(पूर्णांक)KE_MIDDLEMOUSE,	MOUSE_मधला,	TRUE,	FALSE},
    {(पूर्णांक)KE_MIDDLEDRAG,	MOUSE_मधला,	FALSE,	TRUE},
    {(पूर्णांक)KE_MIDDLERELEASE,	MOUSE_मधला,	FALSE,	FALSE},
    {(पूर्णांक)KE_RIGHTMOUSE,	MOUSE_उजवा,	TRUE,	FALSE},
    {(पूर्णांक)KE_RIGHTDRAG,		MOUSE_उजवा,	FALSE,	TRUE},
    {(पूर्णांक)KE_RIGHTRELEASE,	MOUSE_उजवा,	FALSE,	FALSE},
    {(पूर्णांक)KE_X1MOUSE,		MOUSE_X1,	TRUE,	FALSE},
    {(पूर्णांक)KE_X1DRAG,		MOUSE_X1,	FALSE,	TRUE},
    {(पूर्णांक)KE_X1RELEASE,		MOUSE_X1,	FALSE,	FALSE},
    {(पूर्णांक)KE_X2MOUSE,		MOUSE_X2,	TRUE,	FALSE},
    {(पूर्णांक)KE_X2DRAG,		MOUSE_X2,	FALSE,	TRUE},
    {(पूर्णांक)KE_X2RELEASE,		MOUSE_X2,	FALSE,	FALSE},
    // DRAG without CLICK
    {(पूर्णांक)KE_MOUSEMOVE,		MOUSE_RELEASE,	FALSE,	TRUE},
    // RELEASE without CLICK
    {(पूर्णांक)KE_IGNORE,		MOUSE_RELEASE,	FALSE,	FALSE},
    {0,				0,		0,	0},
};

/*
 * Look up the given mouse code to return the relevant information in the other
 * arguments.  Return which button is down or was released.
 */
    पूर्णांक
get_mouse_button(पूर्णांक code, पूर्णांक *is_click, पूर्णांक *is_drag)
{
    पूर्णांक	    i;

    साठी (i = 0; mouse_table[i].pseudo_code; i++)
	जर (code == mouse_table[i].pseudo_code)
	{
	    *is_click = mouse_table[i].is_click;
	    *is_drag = mouse_table[i].is_drag;
	    परत mouse_table[i].button;
	}
    परत 0;	    // Shouldn't get here
}

/*
 * Return the appropriate pseudo mouse event token (KE_LEFTMOUSE etc) based on
 * the given information about which mouse button is down, and whether the
 * mouse was clicked, dragged or released.
 */
    पूर्णांक
get_pseudo_mouse_code(
    पूर्णांक	    button,	// eg MOUSE_LEFT
    पूर्णांक	    is_click,
    पूर्णांक	    is_drag)
{
    पूर्णांक	    i;

    साठी (i = 0; mouse_table[i].pseudo_code; i++)
	जर (button == mouse_table[i].button
	    && is_click == mouse_table[i].is_click
	    && is_drag == mouse_table[i].is_drag)
	{
#ifdef FEAT_GUI
	    // Trick: a non mappable left click and release has mouse_col -1
	    // or added MOUSE_COLOFF.  Used for 'mousefocus' in
	    // gui_mouse_moved()
	    जर (mouse_col < 0 || mouse_col > MOUSE_COLOFF)
	    {
		जर (mouse_col < 0)
		    mouse_col = 0;
		अन्यथा
		    mouse_col -= MOUSE_COLOFF;
		जर (mouse_table[i].pseudo_code == (पूर्णांक)KE_LEFTMOUSE)
		    परत (पूर्णांक)KE_LEFTMOUSE_NM;
		जर (mouse_table[i].pseudo_code == (पूर्णांक)KE_LEFTRELEASE)
		    परत (पूर्णांक)KE_LEFTRELEASE_NM;
	    }
#endif
	    परत mouse_table[i].pseudo_code;
	}
    परत (पूर्णांक)KE_IGNORE;	    // not recognized, ignore it
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
स्थिर पूर्णांक has_mouse_termcode = 0;

    शून्य
set_mouse_termcode(
    पूर्णांक		n,	// KS_MOUSE, KS_NETTERM_MOUSE or KS_DEC_MOUSE
    char_u	*s)
{
    char_u	name[2];

    name[0] = n;
    name[1] = KE_FILLER;
    add_termcode(name, s, FALSE);
#ifdef FEAT_MOUSE_JSB
    जर (n == KS_JSBTERM_MOUSE)
	has_mouse_termcode |= HMT_JSBTERM;
    अन्यथा
#endif
#ifdef FEAT_MOUSE_NET
    जर (n == KS_NETTERM_MOUSE)
	has_mouse_termcode |= HMT_NETTERM;
    अन्यथा
#endif
#ifdef FEAT_MOUSE_DEC
    जर (n == KS_DEC_MOUSE)
	has_mouse_termcode |= HMT_DEC;
    अन्यथा
#endif
#ifdef FEAT_MOUSE_PTERM
    जर (n == KS_PTERM_MOUSE)
	has_mouse_termcode |= HMT_PTERM;
    अन्यथा
#endif
#ifdef FEAT_MOUSE_URXVT
    जर (n == KS_URXVT_MOUSE)
	has_mouse_termcode |= HMT_URXVT;
    अन्यथा
#endif
#ifdef FEAT_MOUSE_GPM
    जर (n == KS_GPM_MOUSE)
	has_mouse_termcode |= HMT_GPM;
    अन्यथा
#endif
    जर (n == KS_SGR_MOUSE)
	has_mouse_termcode |= HMT_SGR;
    अन्यथा जर (n == KS_SGR_MOUSE_RELEASE)
	has_mouse_termcode |= HMT_SGR_REL;
    अन्यथा
	has_mouse_termcode |= HMT_NORMAL;
}

#जर defined(UNIX) || defined(VMS)
    शून्य
del_mouse_termcode(
    पूर्णांक		n)	// KS_MOUSE, KS_NETTERM_MOUSE or KS_DEC_MOUSE
{
    char_u	name[2];

    name[0] = n;
    name[1] = KE_FILLER;
    del_termcode(name);
# ifdef FEAT_MOUSE_JSB
    जर (n == KS_JSBTERM_MOUSE)
	has_mouse_termcode &= ~HMT_JSBTERM;
    अन्यथा
# endif
# ifdef FEAT_MOUSE_NET
    जर (n == KS_NETTERM_MOUSE)
	has_mouse_termcode &= ~HMT_NETTERM;
    अन्यथा
# endif
# ifdef FEAT_MOUSE_DEC
    जर (n == KS_DEC_MOUSE)
	has_mouse_termcode &= ~HMT_DEC;
    अन्यथा
# endif
# ifdef FEAT_MOUSE_PTERM
    जर (n == KS_PTERM_MOUSE)
	has_mouse_termcode &= ~HMT_PTERM;
    अन्यथा
# endif
# ifdef FEAT_MOUSE_URXVT
    जर (n == KS_URXVT_MOUSE)
	has_mouse_termcode &= ~HMT_URXVT;
    अन्यथा
# endif
# ifdef FEAT_MOUSE_GPM
    जर (n == KS_GPM_MOUSE)
	has_mouse_termcode &= ~HMT_GPM;
    अन्यथा
# endif
    जर (n == KS_SGR_MOUSE)
	has_mouse_termcode &= ~HMT_SGR;
    अन्यथा जर (n == KS_SGR_MOUSE_RELEASE)
	has_mouse_termcode &= ~HMT_SGR_REL;
    अन्यथा
	has_mouse_termcode &= ~HMT_NORMAL;
}
#endif

/*
 * setmouse() - switch mouse on/off depending on current mode and 'mouse'
 */
    शून्य
setmouse(शून्य)
{
    पूर्णांक	    checkfor;

#ifdef FEAT_MOUSESHAPE
    update_mouseshape(-1);
#endif

    // Should be outside proc, but may break MOUSESHAPE
#ifdef FEAT_GUI
    // In the GUI the mouse is always enabled.
    जर (gui.in_use)
	परत;
#endif
    // be quick when mouse is off
    जर (*p_mouse == NUL || has_mouse_termcode == 0)
	परत;

    // don't switch mouse on when not in raw mode (Ex mode)
    जर (cur_tmode != TMODE_RAW)
    {
	mch_setmouse(FALSE);
	परत;
    }

    जर (VIsual_active)
	checkfor = MOUSE_VISUAL;
    अन्यथा जर (State == MODE_HITRETURN || State == MODE_ASKMORE
						     || State == MODE_SETWSIZE)
	checkfor = MOUSE_RETURN;
    अन्यथा जर (State & MODE_घाला)
	checkfor = MOUSE_घाला;
    अन्यथा जर (State & MODE_CMDLINE)
	checkfor = MOUSE_COMMAND;
    अन्यथा जर (State == MODE_CONFIRM || State == MODE_EXTERNCMD)
	checkfor = ' '; // don't use mouse for ":confirm" or ":!cmd"
    अन्यथा
	checkfor = MOUSE_NORMAL;    // assume normal mode

    जर (mouse_has(checkfor))
	mch_setmouse(TRUE);
    अन्यथा
	mch_setmouse(FALSE);
}

/*
 * Return TRUE if
 * - "c" is in 'mouse', or
 * - 'a' is in 'mouse' and "c" is in MOUSE_A, or
 * - the current buffer is a help file and 'h' is in 'mouse' and we are in a
 *   normal editing mode (not at hit-return message).
 */
    पूर्णांक
mouse_has(पूर्णांक c)
{
    char_u	*p;

    साठी (p = p_mouse; *p; ++p)
	निवड (*p)
	{
	    स्थिती 'a': जर (vim_strchr((char_u *)MOUSE_A, c) != रिक्त)
			  परत TRUE;
		      थांबा;
	    स्थिती MOUSE_HELP: जर (c != MOUSE_RETURN && curbuf->b_help)
				 परत TRUE;
			     थांबा;
	    पूर्वनिर्धारित: जर (c == *p) परत TRUE; थांबा;
	}
    परत FALSE;
}

/*
 * Return TRUE when 'mousemodel' is set to "popup" or "popup_setpos".
 */
    पूर्णांक
mouse_model_popup(शून्य)
{
    परत (p_mousem[0] == 'p');
}

/*
 * Call a click-region callback function.
 * "regions"/"region_count" describe the resolved click regions,
 * "winid" is stored as the "winid" key in the info dict (0 for tabline).
 * Returns TRUE if the function was called and handled the click.
 */
    स्थिर पूर्णांक
stl_click_handler_regions(
	stl_click_region_T  *regions,
	पूर्णांक		    region_मोजणी,
	पूर्णांक		    winid,
	char_u		    *area_name,
	पूर्णांक		    mrow,
	पूर्णांक		    mcol,
	पूर्णांक		    which_button,
	पूर्णांक		    mods)
{
#ifdef FEAT_EVAL
    पूर्णांक		n;
    पूर्णांक		nclicks;
    char_u	button_str[2];
    char_u	mods_str[4];
    पूर्णांक		mi = 0;
    dict_T	*info;
    typval_T	argvars[2];
    typval_T	rettv;
    funcexe_T	funcexe;
    पूर्णांक		col = mcol;

    जर (regions == रिक्त || region_मोजणी == 0)
	परत FALSE;

    // Find the click region at the given row and column.
    साठी (n = 0; n < region_मोजणी; n++)
    {
	जर (regions[n].row == mrow
		&& col >= regions[n].col_start
		&& col < regions[n].col_end)
	    थांबा;
    }
    जर (n >= region_मोजणी || regions[n].funcname == रिक्त)
	परत FALSE;

    // Build the info dictionary.
    info = dict_alloc();
    जर (info == रिक्त)
	परत FALSE;

    dict_add_संख्या(info, "minwid", regions[n].minwid);

    // Determine number of clicks.
    // MOD_MASK_2CLICK=0x20, MOD_MASK_3CLICK=0x40, MOD_MASK_4CLICK=0x60
    nclicks = ((mods & MOD_MASK_MULTI_CLICK) >> 5) + 1;
    जर (nclicks > 3)
	nclicks = 3;
    dict_add_संख्या(info, "nclicks", nclicks);

    // Button.
    जर (which_button == MOUSE_डावा)
	button_str[0] = 'l';
    अन्यथा जर (which_button == MOUSE_उजवा)
	button_str[0] = 'r';
    अन्यथा
	button_str[0] = 'm';
    button_str[1] = NUL;
    dict_add_string_len(info, "button", button_str, 1);

    // Modifiers.
    जर (mods & MOD_MASK_SHIFT)
	mods_str[mi++] = 's';
    जर (mods & MOD_MASK_CTRL)
	mods_str[mi++] = 'c';
    जर (mods & MOD_MASK_ALT)
	mods_str[mi++] = 'a';
    mods_str[mi] = NUL;
    dict_add_string_len(info, "mods", mods_str, mi);

    dict_add_संख्या(info, "winid", winid);

    // "area": which option the clicked region belongs to.  Lets a shared
    // dispatcher distinguish 'statusline', 'tabline' and 'tabpanel' without
    // having to overload winid == 0.
    dict_add_string(info, "area", area_name);

    // Expose tab page number for 'tabpanel' regions.
    जर (regions[n].tabnr > 0)
	dict_add_संख्या(info, "tabnr", regions[n].tabnr);

    // Call the function with the info dict as argument.
    argvars[0].v_type = VAR_DICT;
    argvars[0].vval.v_dict = info;
    ++info->dv_refcount;
    argvars[1].v_type = VAR_UNKNOWN;

    rettv.v_type = VAR_संख्या;
    rettv.vval.v_संख्या = 0;

    CLEAR_FIELD(funcexe);
    funcexe.fe_evaluate = TRUE;
    (शून्य)call_func(regions[n].funcname, -1,
					    &rettv, 1, argvars, &funcexe);

    n = (पूर्णांक)rettv.vval.v_संख्या;
    clear_tv(&rettv);
    dict_unref(info);

    जर (n != 0)
    {
	// Make sure the tabline gets redrawn too when the callback asks for
	// a redraw (redraw_statuslines() only redraws the tabline when
	// redraw_tabline is set).  For tabpanel the whole screen needs to be
	// refreshed.
	जर (winid == 0)
	    redraw_tabline = TRUE;
# ifdef FEAT_TABPANEL
	जर (STRCMP(area_name, "tabpanel") == 0)
	    redraw_all_later(UPD_NOT_VALID);
# endif
	redraw_statuslines();
    }

    परत TRUE;
#अन्यथा
    (शून्य)regions;
    (शून्य)region_मोजणी;
    (शून्य)winid;
    (शून्य)area_name;
    (शून्य)mrow;
    (शून्य)mcol;
    (शून्य)which_button;
    (शून्य)mods;
    परत FALSE;
#endif
}

/*
 * Call a statusline click handler function for window "wp".
 * Returns TRUE if the function was called and handled the click.
 */
    स्थिर पूर्णांक
stl_click_handler(win_T *wp, पूर्णांक mrow, पूर्णांक mcol, पूर्णांक which_button, पूर्णांक mods)
{
    जर (wp == रिक्त)
	परत FALSE;
    परत stl_click_handler_regions(wp->w_stl_click, wp->w_stl_click_मोजणी,
				wp->w_id, (char_u *)"statusline",
				mrow, mcol, which_button, mods);
}

// dragwin is declared near the top of the file

/*
 * Reset the window being dragged.  To be called when switching tab page.
 */
    शून्य
reset_dragwin(शून्य)
{
    dragwin = रिक्त;
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
    पूर्णांक
jump_to_mouse(
    पूर्णांक		flags,
    पूर्णांक		*inclusive,	// used for inclusive operator, can be NULL
    पूर्णांक		which_button)	// MOUSE_LEFT, MOUSE_RIGHT, MOUSE_MIDDLE
{
    स्थिर पूर्णांक	on_status_ओळ = 0;	// #lines below bottom of window
    स्थिर पूर्णांक	on_sep_ओळ = 0;	// on separator right of window
#ifdef FEAT_MENU
    स्थिर पूर्णांक  in_winbar = FALSE;
#endif
#ifdef FEAT_PROP_POPUP
    स्थिर पूर्णांक   in_popup_win = FALSE;
    स्थिर win_T *click_in_popup_win = रिक्त;
#endif
    स्थिर पूर्णांक	prev_row = -1;
    स्थिर पूर्णांक	prev_col = -1;
    स्थिर पूर्णांक	did_drag = FALSE;	// drag was noticed

    win_T	*wp, *old_curwin;
    pos_T	old_cursor;
    पूर्णांक		मोजणी;
    पूर्णांक		first;
    पूर्णांक		row = mouse_row;
    पूर्णांक		col = mouse_col;
    colnr_T	col_from_screen = -1;
#ifdef FEAT_FOLDING
    पूर्णांक		mouse_char = ' ';
#endif

    mouse_past_bottom = FALSE;
    mouse_past_eol = FALSE;

    जर (flags & MOUSE_RELEASED)
    {
	// On button release we may change window focus if positioned on a
	// status line and no dragging happened.
	जर (dragwin != रिक्त && !did_drag)
	    flags &= ~(MOUSE_FOCUS | MOUSE_DID_MOVE);
	dragwin = रिक्त;
	did_drag = FALSE;
#ifdef FEAT_PROP_POPUP
	जर (click_in_popup_win != रिक्त && popup_dragwin == रिक्त)
	    popup_close_for_mouse_click(click_in_popup_win);

	popup_dragwin = रिक्त;
	click_in_popup_win = रिक्त;
#endif
    }

    जर ((flags & MOUSE_DID_MOVE)
	    && prev_row == mouse_row
	    && prev_col == mouse_col)
    {
retnomove:
	// before moving the cursor for a left click which is NOT in a status
	// line, stop Visual mode
	जर (on_status_ओळ)
	    परत IN_STATUS_ओळ;
	जर (on_sep_ओळ)
	    परत IN_SEP_ओळ;
#ifdef FEAT_MENU
	जर (in_winbar)
	{
	    // A quick second click may arrive as a double-click, but we use it
	    // as a second click in the WinBar.
	    जर ((mod_mask & MOD_MASK_MULTI_CLICK) && !(flags & MOUSE_RELEASED))
	    {
		wp = mouse_find_win(&row, &col, FAIL_POPUP);
		जर (wp == रिक्त)
		    परत IN_UNKNOWN;
		winbar_click(wp, col);
	    }
	    परत IN_OTHER_WIN | MOUSE_WINBAR;
	}
#endif
	जर (flags & MOUSE_MAY_STOP_VIS)
	{
	    end_visual_mode_keep_button();
	    redraw_curbuf_later(UPD_INVERTED);	// delete the inversion
	}
#जर defined(FEAT_CLIPBOARD)
	// Continue a modeless selection in another window.
	जर (cmdwin_type != 0 && row < cmdwin_win->w_winrow)
	    परत IN_OTHER_WIN;
#endif
#ifdef FEAT_PROP_POPUP
	// Continue a modeless selection in a popup window or dragging it.
	जर (in_popup_win)
	{
	    click_in_popup_win = रिक्त;  // don't close it on release
	    जर (popup_dragwin != रिक्त)
	    {
		// dragging a popup window
		popup_drag(popup_dragwin);
		परत IN_UNKNOWN;
	    }
	    परत IN_OTHER_WIN;
	}
#endif
	परत IN_BUFFER;
    }

    prev_row = mouse_row;
    prev_col = mouse_col;

    जर (flags & MOUSE_SETPOS)
	जा retnomove;				// ugly goto...

    old_curwin = curwin;
    old_cursor = curwin->w_cursor;

    जर (!(flags & MOUSE_FOCUS))
    {
	जर (row < 0 || col < 0) // check if it makes sense
	    परत IN_UNKNOWN;

	// find the window where the row is in and adjust "row" and "col" to be
	// relative to top-left of the window
	wp = mouse_find_win(&row, &col, FIND_POPUP);
	जर (wp == रिक्त)
	    परत IN_UNKNOWN;
	dragwin = रिक्त;

#ifdef FEAT_PROP_POPUP
	// Click in a popup window may start dragging or modeless selection,
	// but not much else.
	जर (WIN_IS_POPUP(wp))
	{
	    on_sep_ओळ = 0;
	    on_status_ओळ = 0;
	    in_popup_win = TRUE;
	    जर (which_button == MOUSE_डावा && popup_close_if_on_X(wp, row, col))
	    {
		परत IN_UNKNOWN;
	    }
	    अन्यथा जर (((wp->w_popup_flags & (POPF_DRAG | POPF_RESIZE))
					      && popup_on_border(wp, row, col))
				       || (wp->w_popup_flags & POPF_DRAGALL))
	    {
		popup_dragwin = wp;
		popup_start_drag(wp, row, col);
		परत IN_UNKNOWN;
	    }
	    // Only close on release, otherwise it's not possible to drag or do
	    // modeless selection.
	    अन्यथा जर (wp->w_popup_close == POPCLOSE_CLICK
		    && which_button == MOUSE_डावा)
	    {
		click_in_popup_win = wp;
	    }
	    अन्यथा जर (which_button == MOUSE_डावा)
		// If the click is in the scrollbar, may scroll up/down.
		popup_handle_scrollbar_click(wp, row, col);
# ifdef FEAT_CLIPBOARD
	    परत IN_OTHER_WIN;
# अन्यथा
	    परत IN_UNKNOWN;
# endif
	}
	in_popup_win = FALSE;
	popup_dragwin = रिक्त;
#endif
#ifdef FEAT_MENU
	जर (row == -1)
	{
	    // A click in the window toolbar does not enter another window or
	    // change Visual highlighting.
	    winbar_click(wp, col);
	    in_winbar = TRUE;
	    परत IN_OTHER_WIN | MOUSE_WINBAR;
	}
	in_winbar = FALSE;
#endif

	// winpos and height may change in win_enter()!
	जर (row >= wp->w_height)		// In (or below) status line
	{
	    on_status_ओळ = row - wp->w_height + 1;
	    dragwin = wp;
	}
	अन्यथा
	    on_status_ओळ = 0;
	जर (col >= wp->w_width)		// In separator line
	{
	    on_sep_ओळ = col - wp->w_width + 1;
	    dragwin = wp;
	}
	अन्यथा
	    on_sep_ओळ = 0;

	// The rightmost character of the status line might be a vertical
	// separator character if there is no connecting window to the right.
	जर (on_status_ओळ && on_sep_ओळ)
	{
	    जर (stl_connected(wp))
		on_sep_ओळ = 0;
	    अन्यथा
		on_status_ओळ = 0;
	}

	// Before jumping to another buffer, or moving the cursor for a left
	// click, stop Visual mode.
	जर (VIsual_active
		&& (wp->w_buffer != curwin->w_buffer
		    || (!on_status_ओळ && !on_sep_ओळ
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
	जर (cmdwin_type != 0 && wp != cmdwin_win)
	{
	    // A click outside the command-line window: Use modeless
	    // selection if possible.  Allow dragging the status lines.
	    on_sep_ओळ = 0;
#ifdef FEAT_CLIPBOARD
	    जर (on_status_ओळ)
		परत IN_STATUS_ओळ;
	    परत IN_OTHER_WIN;
#अन्यथा
	    row = 0;
	    col += wp->w_wincol;
	    wp = cmdwin_win;
#endif
	}
#जर defined(FEAT_PROP_POPUP) && defined(FEAT_TERMINAL)
	जर (popup_is_popup(curwin) && curbuf->b_term != रिक्त)
	    // terminal in popup window: don't jump to another window
	    परत IN_OTHER_WIN;
#endif
	// Only change window focus when not clicking on or dragging the
	// status line.  Do change focus when releasing the mouse button
	// (MOUSE_FOCUS was set above if we dragged first).
	जर (dragwin == रिक्त || (flags & MOUSE_RELEASED))
	    win_enter(wp, TRUE);		// can make wp invalid!

	जर (curwin != old_curwin)
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
	जर (on_status_ओळ)			// In (or below) status line
	{
	    // Don't use start_arrow() if we're in the same window
	    जर (curwin == old_curwin)
		परत IN_STATUS_ओळ;
	    अन्यथा
		परत IN_STATUS_ओळ | CURSOR_MOVED;
	}
	जर (on_sep_ओळ)			// In (or below) status line
	{
	    // Don't use start_arrow() if we're in the same window
	    जर (curwin == old_curwin)
		परत IN_SEP_ओळ;
	    अन्यथा
		परत IN_SEP_ओळ | CURSOR_MOVED;
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
    अन्यथा जर (on_status_ओळ && which_button == MOUSE_डावा)
    {
	जर (dragwin != रिक्त)
	{
	    // Drag the status line
	    मोजणी = row - W_WINROW(dragwin) - dragwin->w_height + 1
							     - on_status_ओळ;
	    win_drag_status_ओळ(dragwin, मोजणी);
	    did_drag |= मोजणी;
	}
	परत IN_STATUS_ओळ;			// Cursor didn't move
    }
    अन्यथा जर (on_sep_ओळ && which_button == MOUSE_डावा)
    {
	जर (dragwin != रिक्त)
	{
	    // Drag the separator column
	    मोजणी = col - dragwin->w_wincol - dragwin->w_width + 1
								- on_sep_ओळ;
	    win_drag_vsep_ओळ(dragwin, मोजणी);
	    did_drag |= मोजणी;
	}
	परत IN_SEP_ओळ;			// Cursor didn't move
    }
#ifdef FEAT_MENU
    अन्यथा जर (in_winbar)
    {
	// After a click on the window toolbar don't start Visual mode.
	परत IN_OTHER_WIN | MOUSE_WINBAR;
    }
#endif
    अन्यथा // keep_window_focus must be TRUE
    {
	// before moving the cursor for a left click, stop Visual mode
	जर (flags & MOUSE_MAY_STOP_VIS)
	{
	    end_visual_mode_keep_button();
	    redraw_curbuf_later(UPD_INVERTED);	// delete the inversion
	}

#जर defined(FEAT_CLIPBOARD)
	// Continue a modeless selection in another window.
	जर (cmdwin_type != 0 && row < cmdwin_win->w_winrow)
	    परत IN_OTHER_WIN;
#endif
#ifdef FEAT_PROP_POPUP
	जर (in_popup_win)
	{
	    जर (popup_dragwin != रिक्त)
	    {
		// dragging a popup window
		popup_drag(popup_dragwin);
		परत IN_UNKNOWN;
	    }
	    // continue a modeless selection in a popup window
	    click_in_popup_win = रिक्त;
	    परत IN_OTHER_WIN;
	}
#endif

	row -= W_WINROW(curwin);
	col -= curwin->w_wincol;

	// When clicking beyond the end of the window, scroll the screen.
	// Scroll by however many rows outside the window we are.
	जर (row < 0)
	{
	    मोजणी = 0;
	    साठी (first = TRUE; curwin->w_topline > 1; )
	    {
#ifdef FEAT_DIFF
		जर (curwin->w_topfill < diff_check_fill(curwin, curwin->w_topline))
		    ++मोजणी;
		अन्यथा
#endif
		    मोजणी += plines(curwin->w_topline - 1);
		जर (!first && मोजणी > -row)
		    थांबा;
		first = FALSE;
#ifdef FEAT_FOLDING
		(शून्य)hasFolding(curwin->w_topline, &curwin->w_topline, रिक्त);
#endif
#ifdef FEAT_DIFF
		जर (curwin->w_topfill < diff_check_fill(curwin, curwin->w_topline))
		    ++curwin->w_topfill;
		अन्यथा
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
	अन्यथा जर (row >= curwin->w_height)
	{
	    मोजणी = 0;
	    साठी (first = TRUE; curwin->w_topline < curbuf->b_ml.ml_ओळ_मोजणी; )
	    {
#ifdef FEAT_DIFF
		जर (curwin->w_topfill > 0)
		    ++मोजणी;
		अन्यथा
#endif
		    मोजणी += plines(curwin->w_topline);
		जर (!first && मोजणी > row - curwin->w_height + 1)
		    थांबा;
		first = FALSE;
#ifdef FEAT_FOLDING
		जर (hasFolding(curwin->w_topline, रिक्त, &curwin->w_topline)
			&& curwin->w_topline == curbuf->b_ml.ml_ओळ_मोजणी)
		    थांबा;
#endif
#ifdef FEAT_DIFF
		जर (curwin->w_topfill > 0)
		    --curwin->w_topfill;
		अन्यथा
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
	अन्यथा जर (row == 0)
	{
	    // When dragging the mouse, while the text has been scrolled up as
	    // far as it goes, moving the mouse in the top line should scroll
	    // the text down (done later when recomputing w_topline).
	    जर (mouse_dragging > 0
		    && curwin->w_cursor.lnum
				       == curwin->w_buffer->b_ml.ml_ओळ_मोजणी
		    && curwin->w_cursor.lnum == curwin->w_topline)
		curwin->w_valid &= ~(VALID_TOPLINE);
	}
    }

    जर (prev_row >= W_WINROW(curwin)
	&& prev_row < W_WINROW(curwin) + curwin->w_height
	&& prev_col >= curwin->w_wincol && prev_col < W_ENDCOL(curwin)
						       && Screen_ओळी != रिक्त)
    {
	पूर्णांक off = ओळ_Offset[prev_row] + prev_col;

	// Only use ScreenCols[] after the window was redrawn.  Mainly matters
	// for tests, a user would not click before redrawing.
	// Do not use when 'virtualedit' is active.
	जर (curwin->w_redr_type <= UPD_VALID_NO_UPDATE)
	    col_from_screen = ScreenCols[off];
#ifdef FEAT_FOLDING
	// Remember the character under the mouse, it might be a '-' or '+' in
	// the fold column.
	mouse_char = enc_utf8 && Screen_ओळी_UC[off] != 0
				       ? Screen_ओळी_UC[off] : Screen_ओळी[off];
#endif
    }

#ifdef FEAT_FOLDING
    // Check for position outside of the fold column.
    जर (
# ifdef FEAT_RIGHTLEFT
	    curwin->w_p_rl ? col < curwin->w_width - curwin->w_p_fdc :
# endif
	    col >= curwin->w_p_fdc + (cmdwin_win != curwin ? 0 : 1)
       )
	mouse_char = ' ';
#endif

    // compute the position in the buffer line from the posn on the screen
    जर (mouse_comp_pos(curwin, &row, &col, &curwin->w_cursor.lnum, रिक्त))
	mouse_past_bottom = TRUE;

    // Start Visual mode before coladvance(), for when 'sel' != "old"
    जर ((flags & MOUSE_MAY_VIS) && !VIsual_active)
    {
	check_visual_highlight();
	VIsual = old_cursor;
	VIsual_active = TRUE;
	VIsual_reselect = TRUE;
	// if 'selectmode' contains "mouse", start Select mode
	may_start_select('o');
	setmouse();
	जर (p_smd && संदेश_silent == 0)
	    redraw_cmdline = TRUE;	// show visual mode later
    }

    जर (col_from_screen >= 0)
    {
	// Use the virtual column from ScreenCols[], it is accurate also after
	// concealed characters.
	col = col_from_screen;
    }

    curwin->w_curswant = col;
    curwin->w_set_curswant = false;	// May still have been TRUE
    जर (coladvance(col) == FAIL)	// Mouse click beyond end of line
    {
	जर (inclusive != रिक्त)
	    *inclusive = TRUE;
	mouse_past_eol = TRUE;
    }
    अन्यथा जर (inclusive != रिक्त)
	*inclusive = FALSE;

    मोजणी = IN_BUFFER;
    जर (curwin != old_curwin || curwin->w_cursor.lnum != old_cursor.lnum
	    || curwin->w_cursor.col != old_cursor.col)
	मोजणी |= CURSOR_MOVED;		// Cursor has moved

#ifdef FEAT_FOLDING
    जर (mouse_char == curwin->w_fill_chars.foldclosed)
	मोजणी |= MOUSE_FOLD_OPEN;
    अन्यथा जर (mouse_char != ' ')
	मोजणी |= MOUSE_FOLD_CLOSE;
#endif

    परत मोजणी;
}

/*
 * Make a horizontal scroll to "leftcol".
 * Return TRUE if the cursor moved, FALSE otherwise.
 */
    पूर्णांक
do_mousescroll_horiz(long_u leftcol)
{
    जर (curwin->w_p_wrap)
	परत FALSE;  // no horizontal scrolling when wrapping

    जर (curwin->w_leftcol == (colnr_T)leftcol)
	परत FALSE;  // already there

    // When the line of the cursor is too short, move the cursor to the
    // longest visible line.
    जर (
#ifdef FEAT_GUI
	    (!gui.in_use || vim_strchr(p_go, GO_HORSCROLL) == रिक्त) &&
#endif
		    !virtual_active()
	    && (दीर्घ)leftcol > scroll_ओळ_len(curwin->w_cursor.lnum))
    {
	curwin->w_cursor.lnum = ui_find_longest_lnum();
	curwin->w_cursor.col = 0;
    }

    परत set_leftcol((colnr_T)leftcol);
}

/*
 * Normal and Visual modes implementation for scrolling in direction
 * "cap->arg", which is one of the MSCR_ values.
 */
    शून्य
nv_mousescroll(cmdarg_T *cap)
{
    win_T   *old_curwin = curwin;

#ifdef FEAT_TABPANEL
    जर (mouse_row >= 0 && mouse_col >= 0
	    && (cap->arg == MSCR_UP || cap->arg == MSCR_DOWN)
	    && mouse_on_tabpanel())
    {
	(शून्य)tabpanel_scroll(cap->arg == MSCR_UP ? 1 : -1,
		mouse_vert_step > 0 ? mouse_vert_step : 3);
	परत;
    }
#endif

    जर (mouse_row >= 0 && mouse_col >= 0)
    {
	// Find the window at the mouse pointer coordinates.
	// NOTE: Must restore "curwin" to "old_curwin" before returning!
	पूर्णांक row = mouse_row;
	पूर्णांक col = mouse_col;
	curwin = mouse_find_win(&row, &col, FIND_POPUP);
	जर (curwin == रिक्त)
	{
	    curwin = old_curwin;
	    परत;
	}

#ifdef FEAT_PROP_POPUP
	जर (WIN_IS_POPUP(curwin) && !curwin->w_has_scrollbar)
	{
	    // cannot scroll this popup window
	    curwin = old_curwin;
	    परत;
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
    शून्य
nv_mouse(cmdarg_T *cap)
{
    (शून्य)do_mouse(cap->oap, cap->cmdchar, BACKWARD, cap->count1, 0);
}

स्थिर पूर्णांक	held_button = MOUSE_RELEASE;

    शून्य
reset_held_button(शून्य)
{
    held_button = MOUSE_RELEASE;
}

/*
 * Check if typebuf 'tp' contains a terminal mouse code and returns the
 * modifiers found in typebuf in 'modifiers'.
 */
    पूर्णांक
check_termcode_mouse(
    char_u	*tp,
    पूर्णांक		*slen,
    char_u	*किल्ली_name,
    char_u	*modifiers_start,
    पूर्णांक		idx,
    पूर्णांक		*modifiers)
{
    पूर्णांक		j;
    char_u	*p;
#जर !defined(UNIX) || defined(FEAT_MOUSE_XTERM) || defined(FEAT_GUI) \
    || defined(FEAT_MOUSE_GPM) || defined(FEAT_SYSMOUSE)
    char_u	bytes[6];
    पूर्णांक		अंक_bytes;
#endif
    पूर्णांक		mouse_code = 0;	    // init for GCC
    पूर्णांक		is_click, is_drag;
    पूर्णांक		is_release, release_is_ambiguous;
    पूर्णांक		wheel_code = 0;
    पूर्णांक		current_button;
    स्थिर पूर्णांक	orig_अंक_clicks = 1;
    स्थिर पूर्णांक	orig_mouse_code = 0x0;
#ifdef CHECK_DOUBLE_CLICK
    स्थिर पूर्णांक	orig_mouse_col = 0;
    स्थिर पूर्णांक	orig_mouse_row = 0;
    स्थिर संरचना timeval  orig_mouse_time = {0, 0};
    // time of previous mouse click
    संरचना timeval  mouse_time;		// time of current mouse click
    दीर्घ	timediff;		// elapsed time in msec
#endif

    is_click = is_drag = is_release = release_is_ambiguous = FALSE;

#जर !defined(UNIX) || defined(FEAT_MOUSE_XTERM) || defined(FEAT_GUI) \
    || defined(FEAT_MOUSE_GPM) || defined(FEAT_SYSMOUSE)
    जर (किल्ली_name[0] == KS_MOUSE
# ifdef FEAT_MOUSE_GPM
	    || किल्ली_name[0] == KS_GPM_MOUSE
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
	साठी (;;)
	{
	    // For the GUI and for MS-Windows two bytes each are used for row
	    // and column.  Allows for more than 223 columns.
# जर defined(FEAT_GUI) || defined(MSWIN)
	    जर (TRUE
#  जर defined(FEAT_GUI) && !defined(MSWIN)
		&& gui.in_use
#  endif
		)
	    {
		अंक_bytes = get_bytes_from_buf(tp + *slen, bytes, 5);
		जर (अंक_bytes == -1)	// not enough coordinates
		    परत -1;
		mouse_code = bytes[0];
		mouse_col = 128 * (bytes[1] - ' ' - 1)
		    + bytes[2] - ' ' - 1;
		mouse_row = 128 * (bytes[3] - ' ' - 1)
		    + bytes[4] - ' ' - 1;
	    }
	    अन्यथा
# endif
	    {
		अंक_bytes = get_bytes_from_buf(tp + *slen, bytes, 3);
		जर (अंक_bytes == -1)	// not enough coordinates
		    परत -1;
		mouse_code = bytes[0];
		mouse_col = bytes[1] - ' ' - 1;
		mouse_row = bytes[2] - ' ' - 1;
	    }
	    *slen += अंक_bytes;

	    // If the following bytes is also a mouse code and it has the same
	    // code, dump this one and get the next.  This makes dragging a
	    // whole lot faster.
# ifdef FEAT_GUI
	    जर (gui.in_use)
		j = 3;
	    अन्यथा
# endif
		j = get_termcode_len(idx);
	    जर (STRNCMP(tp, tp + *slen, (माप_प्रकार)j) == 0
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
	    अन्यथा
		थांबा;
	}
    }

    जर (किल्ली_name[0] == KS_URXVT_MOUSE
	    || किल्ली_name[0] == KS_SGR_MOUSE
	    || किल्ली_name[0] == KS_SGR_MOUSE_RELEASE)
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
	जर (p == रिक्त)
	    परत -1;

	mouse_code = getdigits(&p);
	जर (*p++ != ';')
	    परत -1;

	// when mouse reporting is SGR, add 32 to mouse code
	जर (किल्ली_name[0] == KS_SGR_MOUSE
		|| किल्ली_name[0] == KS_SGR_MOUSE_RELEASE)
	    mouse_code += 32;

	mouse_col = getdigits(&p) - 1;
	जर (*p++ != ';')
	    परत -1;

	mouse_row = getdigits(&p) - 1;

	// The modifiers were the mouse coordinates, not the modifier keys
	// (alt/shift/ctrl/meta) state.
	*modifiers = 0;
    }

    जर (किल्ली_name[0] == KS_SGR_MOUSE
	    || किल्ली_name[0] == KS_SGR_MOUSE_RELEASE)
    {
	जर (किल्ली_name[0] == KS_SGR_MOUSE_RELEASE)
	{
	    is_release = TRUE;
	    // This is used below to set held_button.
	    mouse_code |= MOUSE_RELEASE;
	}
    }
    अन्यथा
    {
	release_is_ambiguous = TRUE;
	जर ((mouse_code & MOUSE_RELEASE) == MOUSE_RELEASE)
	    is_release = TRUE;
    }

    जर (किल्ली_name[0] == KS_MOUSE
# ifdef FEAT_MOUSE_GPM
	    || किल्ली_name[0] == KS_GPM_MOUSE
# endif
# ifdef FEAT_MOUSE_URXVT
	    || किल्ली_name[0] == KS_URXVT_MOUSE
# endif
	    || किल्ली_name[0] == KS_SGR_MOUSE
	    || किल्ली_name[0] == KS_SGR_MOUSE_RELEASE)
    {
# जर !defined(MSWIN)
	/*
	 * Handle old style mouse events.
	 * Recognize the xterm mouse wheel, but not in the GUI, the
	 * Linux console with GPM and the MS-DOS or Win32 console
	 * (multi-clicks use >= 0x60).
	 */
	जर (mouse_code >= MOUSEWHEEL_खालचा && mouse_code < MOUSESIDEBUTTONS_खालचा
#  ifdef FEAT_GUI
		&& !gui.in_use
#  endif
#  ifdef FEAT_MOUSE_GPM
		&& किल्ली_name[0] != KS_GPM_MOUSE
#  endif
	   )
	{
#  जर defined(UNIX)
	    जर (use_xterm_mouse() > 1 && mouse_code >= 0x80)
		// mouse-move event, using MOUSE_DRAG works
		mouse_code = MOUSE_DRAG;
	    अन्यथा
#  endif
		// Keep the mouse_code before it's changed, so that we
		// remember that it was a mouse wheel click.
		wheel_code = mouse_code;
	}
#  ifdef FEAT_MOUSE_XTERM
	अन्यथा जर (held_button == MOUSE_RELEASE
#   ifdef FEAT_GUI
		&& !gui.in_use
#   endif
		&& (mouse_code == 0x23 || mouse_code == 0x24
		    || mouse_code == 0x40 || mouse_code == 0x41))
	{
	    // Apparently 0x23 and 0x24 are used by rxvt scroll wheel.
	    // And 0x40 and 0x41 are used by some xterm emulator.
	    wheel_code = mouse_code - (mouse_code >= 0x40 ? 0x40 : 0x23)
							      + MOUSEWHEEL_खालचा;
	}
#  endif

#  जर defined(UNIX)
	अन्यथा जर (use_xterm_mouse() > 1)
	{
	    जर (mouse_code & MOUSE_DRAG_XTERM)
		mouse_code |= MOUSE_DRAG;
	}
#  endif
#  ifdef FEAT_XCLIPBOARD
	अन्यथा जर (!(mouse_code & MOUSE_DRAG & ~MOUSE_CLICK_MASK))
	{
	    जर (is_release)
		stop_xterm_trace();
	    अन्यथा
		start_xterm_trace(mouse_code);
	}
#  endif
# endif
    }
#endif // !UNIX || FEAT_MOUSE_XTERM
#ifdef FEAT_MOUSE_NET
    जर (किल्ली_name[0] == KS_NETTERM_MOUSE)
    {
	पूर्णांक mc, mr;

	// expect a rather limited sequence like: balancing {
	// \033}6,45\r
	// '6' is the row, 45 is the column
	p = tp + *slen;
	mr = getdigits(&p);
	जर (*p++ != ',')
	    परत -1;
	mc = getdigits(&p);
	जर (*p++ != '\r')
	    परत -1;

	mouse_col = mc - 1;
	mouse_row = mr - 1;
	mouse_code = MOUSE_डावा;
	*slen += (पूर्णांक)(p - (tp + *slen));
    }
#endif	// FEAT_MOUSE_NET
#ifdef FEAT_MOUSE_JSB
    जर (किल्ली_name[0] == KS_JSBTERM_MOUSE)
    {
	पूर्णांक mult, val, iter, button, status;

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
	निवड (*p++)
	{
	    स्थिती 'L': button = 1; थांबा;
	    स्थिती '-': थांबा;
	    स्थिती 'x': थांबा; // ignore sequence
	    पूर्वनिर्धारित:  परत -1; // Unknown Result
	}
	निवड (*p++)
	{
	    स्थिती 'M': button |= 2; थांबा;
	    स्थिती '-': थांबा;
	    स्थिती 'x': थांबा; // ignore sequence
	    पूर्वनिर्धारित:  परत -1; // Unknown Result
	}
	निवड (*p++)
	{
	    स्थिती 'R': button |= 4; थांबा;
	    स्थिती '-': थांबा;
	    स्थिती 'x': थांबा; // ignore sequence
	    पूर्वनिर्धारित:  परत -1; // Unknown Result
	}
	status = *p++;
	साठी (val = 0, mult = 100, iter = 0; iter < 3; iter++,
		mult /= 10, p++)
	    जर (*p >= '0' && *p <= '9')
		val += (*p - '0') * mult;
	    अन्यथा
		परत -1;
	mouse_col = val;
	साठी (val = 0, mult = 100, iter = 0; iter < 3; iter++,
		mult /= 10, p++)
	    जर (*p >= '0' && *p <= '9')
		val += (*p - '0') * mult;
	    अन्यथा
		परत -1;
	mouse_row = val;
	निवड (*p++)
	{
	    स्थिती 's': button |= 8; थांबा;  // SHIFT key Pressed
	    स्थिती '-': थांबा;  // Not Pressed
	    स्थिती 'x': थांबा;  // Not Reporting
	    पूर्वनिर्धारित:  परत -1; // Unknown Result
	}
	निवड (*p++)
	{
	    स्थिती 'c': button |= 16; थांबा;  // CTRL key Pressed
	    स्थिती '-': थांबा;  // Not Pressed
	    स्थिती 'x': थांबा;  // Not Reporting
	    पूर्वनिर्धारित:  परत -1; // Unknown Result
	}
	जर (*p++ != '\033')
	    परत -1;
	जर (*p++ != '\\')
	    परत -1;
	निवड (status)
	{
	    स्थिती 'D': // Double Click
	    स्थिती 'S': // Single Click
		जर (button & 1) mouse_code |= MOUSE_डावा;
		जर (button & 2) mouse_code |= MOUSE_मधला;
		जर (button & 4) mouse_code |= MOUSE_उजवा;
		जर (button & 8) mouse_code |= MOUSE_SHIFT;
		जर (button & 16) mouse_code |= MOUSE_CTRL;
		थांबा;
	    स्थिती 'm': // Mouse move
		जर (button & 1) mouse_code |= MOUSE_डावा;
		जर (button & 2) mouse_code |= MOUSE_मधला;
		जर (button & 4) mouse_code |= MOUSE_उजवा;
		जर (button & 8) mouse_code |= MOUSE_SHIFT;
		जर (button & 16) mouse_code |= MOUSE_CTRL;
		जर ((button & 7) != 0)
		{
		    held_button = mouse_code;
		    mouse_code |= MOUSE_DRAG;
		}
		is_drag = TRUE;
		showmode();
		थांबा;
	    स्थिती 'd': // Button Down
		जर (button & 1) mouse_code |= MOUSE_डावा;
		जर (button & 2) mouse_code |= MOUSE_मधला;
		जर (button & 4) mouse_code |= MOUSE_उजवा;
		जर (button & 8) mouse_code |= MOUSE_SHIFT;
		जर (button & 16) mouse_code |= MOUSE_CTRL;
		थांबा;
	    स्थिती 'u': // Button Up
		is_release = TRUE;
		जर (button & 1)
		    mouse_code |= MOUSE_डावा;
		जर (button & 2)
		    mouse_code |= MOUSE_मधला;
		जर (button & 4)
		    mouse_code |= MOUSE_उजवा;
		जर (button & 8)
		    mouse_code |= MOUSE_SHIFT;
		जर (button & 16)
		    mouse_code |= MOUSE_CTRL;
		थांबा;
	    पूर्वनिर्धारित: परत -1; // Unknown Result
	}

	*slen += (p - (tp + *slen));
    }
#endif // FEAT_MOUSE_JSB
#ifdef FEAT_MOUSE_DEC
    जर (किल्ली_name[0] == KS_DEC_MOUSE)
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
	पूर्णांक Pe, Pb, Pr, Pc;

	p = tp + *slen;

	// get event status
	Pe = getdigits(&p);
	जर (*p++ != ';')
	    परत -1;

	// get button status
	Pb = getdigits(&p);
	जर (*p++ != ';')
	    परत -1;

	// get row status
	Pr = getdigits(&p);
	जर (*p++ != ';')
	    परत -1;

	// get column status
	Pc = getdigits(&p);

	// the page parameter is optional
	जर (*p == ';')
	{
	    p++;
	    (शून्य)getdigits(&p);
	}
	जर (*p++ != '&')
	    परत -1;
	जर (*p++ != 'w')
	    परत -1;

	mouse_code = 0;
	निवड (Pe)
	{
	    स्थिती  0: परत -1; // position request while unavailable
	    स्थिती  1: // a response to a locator position request includes
		     //	the status of all buttons
		     Pb &= 7;   // mask off and ignore fourth button
		     जर (Pb & 4)
			 mouse_code  = MOUSE_डावा;
		     जर (Pb & 2)
			 mouse_code  = MOUSE_मधला;
		     जर (Pb & 1)
			 mouse_code  = MOUSE_उजवा;
		     जर (Pb)
		     {
			 held_button = mouse_code;
			 mouse_code |= MOUSE_DRAG;
			 WantQueryMouse = TRUE;
		     }
		     is_drag = TRUE;
		     showmode();
		     थांबा;
	    स्थिती  2: mouse_code = MOUSE_डावा;
		     WantQueryMouse = TRUE;
		     थांबा;
	    स्थिती  3: mouse_code = MOUSE_डावा;
		     is_release = TRUE;
		     थांबा;
	    स्थिती  4: mouse_code = MOUSE_मधला;
		     WantQueryMouse = TRUE;
		     थांबा;
	    स्थिती  5: mouse_code = MOUSE_मधला;
		     is_release = TRUE;
		     थांबा;
	    स्थिती  6: mouse_code = MOUSE_उजवा;
		     WantQueryMouse = TRUE;
		     थांबा;
	    स्थिती  7: mouse_code = MOUSE_उजवा;
		     is_release = TRUE;
		     थांबा;
	    स्थिती  8: परत -1; // fourth button down
	    स्थिती  9: परत -1; // fourth button up
	    स्थिती 10: परत -1; // mouse outside of filter rectangle
	    पूर्वनिर्धारित: परत -1; // should never occur
	}

	mouse_col = Pc - 1;
	mouse_row = Pr - 1;

	*slen += (पूर्णांक)(p - (tp + *slen));
    }
#endif // FEAT_MOUSE_DEC
#ifdef FEAT_MOUSE_PTERM
    जर (किल्ली_name[0] == KS_PTERM_MOUSE)
    {
	पूर्णांक button, अंक_clicks, action;

	p = tp + *slen;

	action = getdigits(&p);
	जर (*p++ != ';')
	    परत -1;

	mouse_row = getdigits(&p);
	जर (*p++ != ';')
	    परत -1;
	mouse_col = getdigits(&p);
	जर (*p++ != ';')
	    परत -1;

	button = getdigits(&p);
	mouse_code = 0;

	निवड (button)
	{
	    स्थिती 4: mouse_code = MOUSE_डावा; थांबा;
	    स्थिती 1: mouse_code = MOUSE_उजवा; थांबा;
	    स्थिती 2: mouse_code = MOUSE_मधला; थांबा;
	    पूर्वनिर्धारित: परत -1;
	}

	निवड (action)
	{
	    स्थिती 31: // Initial press
		जर (*p++ != ';')
		    परत -1;

		अंक_clicks = getdigits(&p); // Not used
		थांबा;

	    स्थिती 32: // Release
		is_release = TRUE;
		थांबा;

	    स्थिती 33: // Drag
		held_button = mouse_code;
		mouse_code |= MOUSE_DRAG;
		थांबा;

	    पूर्वनिर्धारित:
		परत -1;
	}

	जर (*p++ != 't')
	    परत -1;

	*slen += (p - (tp + *slen));
    }
#endif // FEAT_MOUSE_PTERM

    // Interpret the mouse code
    current_button = (mouse_code & MOUSE_CLICK_MASK);
    जर (is_release)
	current_button |= MOUSE_RELEASE;

    जर (current_button == MOUSE_RELEASE
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
	जर ((mouse_code & MOUSE_DRAG) == MOUSE_DRAG)
	    is_drag = TRUE;
	current_button = held_button;
    }
    अन्यथा
    {
      जर (wheel_code == 0)
      {
#ifdef CHECK_DOUBLE_CLICK
# ifdef FEAT_MOUSE_GPM
	/*
	 * Only for Unix, when GUI not active, we handle multi-clicks here, but
	 * not for GPM mouse events.
	 */
#  ifdef FEAT_GUI
	जर (किल्ली_name[0] != KS_GPM_MOUSE && !gui.in_use)
#  अन्यथा
	    जर (किल्ली_name[0] != KS_GPM_MOUSE)
#  endif
# अन्यथा
#  ifdef FEAT_GUI
		जर (!gui.in_use)
#  endif
# endif
		{
		    /*
		     * Compute the time elapsed since the previous mouse click.
		     */
		    gettimeofday(&mouse_time, रिक्त);
		    जर (orig_mouse_time.tv_sec == 0)
		    {
			/*
			 * Avoid computing the difference between mouse_time
			 * and orig_mouse_time for the first click, as the
			 * difference would be huge and would cause
			 * multiplication overflow.
			 */
			timediff = p_mouset;
		    }
		    अन्यथा
			timediff = time_diff_ms(&orig_mouse_time, &mouse_time);
		    orig_mouse_time = mouse_time;
		    जर (mouse_code == orig_mouse_code
			    && timediff < p_mouset
			    && orig_अंक_clicks != 4
			    && orig_mouse_col == mouse_col
			    && orig_mouse_row == mouse_row
			    && (is_mouse_topline(curwin)
				// Double click in tab pages line also works
				// when window contents changes.
				|| (mouse_row == 0 && firstwin->w_winrow > 0))
		       )
			++orig_अंक_clicks;
		    अन्यथा
			orig_अंक_clicks = 1;
		    orig_mouse_col = mouse_col;
		    orig_mouse_row = mouse_row;
		    set_mouse_topline(curwin);
		}
# जर defined(FEAT_GUI) || defined(FEAT_MOUSE_GPM)
		अन्यथा
		    orig_अंक_clicks = अंक_MOUSE_CLICKS(mouse_code);
# endif
#अन्यथा
	orig_अंक_clicks = अंक_MOUSE_CLICKS(mouse_code);
#endif
	is_click = TRUE;
      }
      orig_mouse_code = mouse_code;
    }
    जर (!is_drag)
	held_button = mouse_code & MOUSE_CLICK_MASK;

    /*
     * Translate the actual mouse event into a pseudo mouse event.
     * First work out what modifiers are to be used.
     */
    जर (orig_mouse_code & MOUSE_SHIFT)
	*modifiers |= MOD_MASK_SHIFT;
    जर (orig_mouse_code & MOUSE_CTRL)
	*modifiers |= MOD_MASK_CTRL;
    जर (orig_mouse_code & MOUSE_ALT)
	*modifiers |= MOD_MASK_ALT;
    जर (orig_अंक_clicks == 2)
	*modifiers |= MOD_MASK_2CLICK;
    अन्यथा जर (orig_अंक_clicks == 3)
	*modifiers |= MOD_MASK_3CLICK;
    अन्यथा जर (orig_अंक_clicks == 4)
	*modifiers |= MOD_MASK_4CLICK;

    // Work out our pseudo mouse event. Note that MOUSE_RELEASE gets added,
    // then it's not mouse up/down.
    किल्ली_name[0] = KS_EXTRA;
    जर (wheel_code != 0 && (!is_release || release_is_ambiguous))
    {
	जर (wheel_code & MOUSE_CTRL)
	    *modifiers |= MOD_MASK_CTRL;
	जर (wheel_code & MOUSE_ALT)
	    *modifiers |= MOD_MASK_ALT;

	जर (wheel_code & 1 && wheel_code & 2)
	    किल्ली_name[1] = (पूर्णांक)KE_MOUSELEFT;
	अन्यथा जर (wheel_code & 2)
	    किल्ली_name[1] = (पूर्णांक)KE_MOUSERIGHT;
	अन्यथा जर (wheel_code & 1)
	    किल्ली_name[1] = (पूर्णांक)KE_MOUSEUP;
	अन्यथा
	    किल्ली_name[1] = (पूर्णांक)KE_MOUSEDOWN;

	held_button = MOUSE_RELEASE;
    }
    अन्यथा
    {
#जर defined(UNIX)
	जर (use_xterm_mouse() && orig_mouse_code >= MOUSESIDEBUTTONS_खालचा)
	    current_button = (current_button) ? MOUSE_X2 : MOUSE_X1;
#endif
	किल्ली_name[1] = get_pseudo_mouse_code(current_button, is_click, is_drag);
    }


    // Make sure the mouse position is valid.  Some terminals may return weird
    // values.
    जर (mouse_col >= Columns)
	mouse_col = Columns - 1;
    जर (mouse_row >= Rows)
	mouse_row = Rows - 1;

    परत 0;
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
    पूर्णांक
mouse_comp_pos(
    win_T	*win,
    पूर्णांक		*rowp,
    पूर्णांक		*colp,
    linenr_T	*lnump,
    पूर्णांक		*plines_cache)
{
    पूर्णांक		col = *colp;
    पूर्णांक		row = *rowp;
    linenr_T	lnum;
    पूर्णांक		retval = FALSE;
    पूर्णांक		off;
    पूर्णांक		मोजणी;

#ifdef FEAT_RIGHTLEFT
    जर (win->w_p_rl)
	col = win->w_width - 1 - col;
#endif

    lnum = win->w_topline;

    पर्यंत (row > 0)
    {
	पूर्णांक cache_idx = lnum - win->w_topline;

	// Only "Rows" lines are cached, with folding we'll run out of entries
	// and use the slow way.
	जर (plines_cache != रिक्त && cache_idx < Rows
						&& plines_cache[cache_idx] > 0)
	    मोजणी = plines_cache[cache_idx];
	अन्यथा
	{
#ifdef FEAT_DIFF
	    // Don't include filler lines in "count"
	    जर (win->w_p_diff
# ifdef FEAT_FOLDING
		    && !hasFoldingWin(win, lnum, रिक्त, रिक्त, TRUE, रिक्त)
# endif
		    )
	    {
		जर (lnum == win->w_topline)
		    row -= win->w_topfill;
		अन्यथा
		    row -= diff_check_fill(win, lnum);
		मोजणी = plines_win_nofill(win, lnum, FALSE);
	    }
	    अन्यथा
#endif
		मोजणी = plines_win(win, lnum, FALSE);
	    जर (plines_cache != रिक्त && cache_idx < Rows)
		plines_cache[cache_idx] = मोजणी;
	}

	जर (win->w_skipcol > 0 && lnum == win->w_topline)
	{
	    पूर्णांक width1 = win->w_width - win_col_off(win);

	    जर (width1 > 0)
	    {
		पूर्णांक skip_ओळी = 0;

		// Adjust for 'smoothscroll' clipping the top screen lines.
		// A similar formula is used in curs_columns().
		जर (win->w_skipcol > width1)
		    skip_ओळी = (win->w_skipcol - width1)
					    / (width1 + win_col_off2(win)) + 1;
		अन्यथा जर (win->w_skipcol > 0)
		    skip_ओळी = 1;

		मोजणी -= skip_ओळी;
	    }
	}

	जर (मोजणी > row)
	    थांबा;	// Position is in this buffer line.
#ifdef FEAT_FOLDING
	(शून्य)hasFoldingWin(win, lnum, रिक्त, &lnum, TRUE, रिक्त);
#endif
	जर (lnum == win->w_buffer->b_ml.ml_ओळ_मोजणी)
	{
	    retval = TRUE;
	    थांबा;		// past end of file
	}
	row -= मोजणी;
	++lnum;
    }

    जर (!retval)
    {
	// Compute the column without wrapping.
	off = win_col_off(win) - win_col_off2(win);
	जर (col < off)
	    col = off;
	col += row * (win->w_width - off);

	// Add skip column for the topline.
	जर (lnum == win->w_topline)
	    col += win->w_skipcol;
    }

    जर (!win->w_p_wrap)
	col += win->w_leftcol;

    // skip line number and fold column in front of the line
    col -= win_col_off(win);
    जर (col <= 0)
    {
#ifdef FEAT_NETBEANS_INTG
	// if mouse is clicked on the gutter, then inform the netbeans server
	जर (*colp < win_col_off(win))
	    netbeans_gutter_click(lnum);
#endif
	col = 0;
    }

    *colp = col;
    *rowp = row;
    *lnump = lnum;
    परत retval;
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
mouse_find_win(पूर्णांक *rowp, पूर्णांक *colp, mouse_find_T popup UNUSED)
{
    frame_T	*fp;
    win_T	*wp;

#ifdef FEAT_PROP_POPUP
    win_T	*pwp = रिक्त;

    जर (popup != IGNORE_POPUP)
    {
	popup_reset_handled(POPUP_HANDLED_1);
	पर्यंत ((wp = find_next_popup(TRUE, POPUP_HANDLED_1)) != रिक्त)
	{
	    जर (*rowp >= wp->w_winrow && *rowp < wp->w_winrow + popup_height(wp)
		    && *colp >= wp->w_wincol
				    && *colp < wp->w_wincol + popup_width(wp))
		pwp = wp;
	}
	जर (pwp != रिक्त)
	{
	    जर (popup == FAIL_POPUP)
		परत रिक्त;
	    *rowp -= pwp->w_winrow;
	    *colp -= pwp->w_wincol;
	    परत pwp;
	}
    }
#endif

    fp = topframe;

    जर (*colp < firstwin->w_wincol
	    || *colp >= firstwin->w_wincol + fp->fr_width
	    || *rowp < firstwin->w_winrow)
	परत रिक्त;

    *rowp -= firstwin->w_winrow;
    *colp -= firstwin->w_wincol;
    साठी (;;)
    {
	जर (fp->fr_layout == FR_LEAF)
	    थांबा;
	जर (fp->fr_layout == FR_ROW)
	{
	    साठी (fp = fp->fr_child; fp->fr_next != रिक्त; fp = fp->fr_next)
	    {
		जर (*colp < fp->fr_width)
		    थांबा;
		*colp -= fp->fr_width;
	    }
	}
	अन्यथा    // fr_layout == FR_COL
	{
	    साठी (fp = fp->fr_child; fp->fr_next != रिक्त; fp = fp->fr_next)
	    {
		जर (*rowp < fp->fr_height)
		    थांबा;
		*rowp -= fp->fr_height;
	    }
	}
    }
    // When using a timer that closes a window the window might not actually
    // exist.
    FOR_ALL_WINDOWS(wp)
	जर (wp == fp->fr_win)
	{
#ifdef FEAT_MENU
	    *rowp -= wp->w_winbar_height;
#endif
	    परत wp;
	}
    परत रिक्त;
}

#जर defined(NEED_VCOL2COL) || defined(FEAT_BEVAL) || defined(FEAT_PROP_POPUP) \
	|| defined(FEAT_EVAL)
/*
 * Convert a virtual (screen) column to a character column.
 * The first column is zero.
 */
    पूर्णांक
vcol2col(win_T *wp, linenr_T lnum, पूर्णांक vcol, colnr_T *coladdp)
{
    char_u	    *ओळ;
    chartabsize_T   cts;

    // try to advance to the specified column
    ओळ = ml_get_buf(wp->w_buffer, lnum, FALSE);
    init_chartabsize_arg(&cts, wp, lnum, 0, ओळ, ओळ);
    पर्यंत (cts.cts_vcol < vcol && *cts.cts_ptr != NUL)
    {
	पूर्णांक size = win_lbr_chartabsize(&cts, रिक्त, रिक्त);
	जर (cts.cts_vcol + size > vcol)
	    थांबा;
	cts.cts_vcol += size;
	MB_PTR_ADV(cts.cts_ptr);
    }
    clear_chartabsize_arg(&cts);

    जर (coladdp != रिक्त)
	*coladdp = vcol - cts.cts_vcol;
    परत (पूर्णांक)(cts.cts_ptr - ओळ);
}
#endif

#जर defined(FEAT_EVAL)
/*
 * "getmousepos()" function.
 */
    शून्य
f_getmousepos(typval_T *argvars UNUSED, typval_T *rettv)
{
    dict_T	*d;
    win_T	*wp;
    पूर्णांक		row = mouse_row;
    पूर्णांक		col = mouse_col;
    varnumber_T winid = 0;
    varnumber_T winrow = 0;
    varnumber_T wincol = 0;
    linenr_T	lnum = 0;
    varnumber_T column = 0;
    colnr_T	coladd = 0;

    जर (rettv_dict_alloc(rettv) == FAIL)
	परत;
    d = rettv->vval.v_dict;

    dict_add_संख्या(d, "screenrow", (varnumber_T)mouse_row + 1);
    dict_add_संख्या(d, "screencol", (varnumber_T)mouse_col + 1);

    wp = mouse_find_win(&row, &col, FIND_POPUP);
    जर (wp != रिक्त)
    {
	पूर्णांक	शीर्ष_off = 0;
	पूर्णांक	डावा_off = 0;
	पूर्णांक	height = wp->w_height + wp->w_status_height;

# ifdef FEAT_PROP_POPUP
	जर (WIN_IS_POPUP(wp))
	{
	    शीर्ष_off = popup_शीर्ष_extra(wp);
	    डावा_off = popup_डावा_extra(wp);
	    height = popup_height(wp);
	}
# endif
	जर (row < height)
	{
	    winid = wp->w_id;
	    winrow = row + 1;
	    wincol = col + 1;
	    row -= शीर्ष_off;
	    col -= डावा_off;
	    जर (row >= 0 && row < wp->w_height && col >= 0 && col < wp->w_width)
	    {
		(शून्य)mouse_comp_pos(wp, &row, &col, &lnum, रिक्त);
		col = vcol2col(wp, lnum, col, &coladd);
		column = col + 1;
	    }
	}
    }
    dict_add_संख्या(d, "winid", winid);
    dict_add_संख्या(d, "winrow", winrow);
    dict_add_संख्या(d, "wincol", wincol);
    dict_add_संख्या(d, "line", (varnumber_T)lnum);
    dict_add_संख्या(d, "column", column);
    dict_add_संख्या(d, "coladd", coladd);
}
#endif
