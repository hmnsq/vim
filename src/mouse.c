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

#અન્તર્ભાવ "vim.h"

/*
 * Horiziontal and vertical steps used when scrolling.
 * When negative scroll by a whole page.
 */
સ્થિર long mouse_hor_step = 6;
સ્થિર long mouse_vert_step = 3;
સ્થિર win_T *dragwin = ખાલી;	// window being dragged
સ્થિર પૂર્ણાંક stl_click_handler(win_T *wp, પૂર્ણાંક mrow, પૂર્ણાંક mcol, પૂર્ણાંક which_button,
								    પૂર્ણાંક mods);
સ્થિર પૂર્ણાંક stl_click_handler_regions(stl_click_region_T *regions,
				    પૂર્ણાંક region_ગણતરી, પૂર્ણાંક winid,
				    char_u *area_name, પૂર્ણાંક mrow, પૂર્ણાંક mcol,
				    પૂર્ણાંક which_button, પૂર્ણાંક mods);

    શૂન્ય
mouse_set_vert_scroll_step(long step)
{
    mouse_vert_step = step;
}

    શૂન્ય
mouse_set_hor_scroll_step(long step)
{
    mouse_hor_step = step;
}

#ifdef CHECK_DOUBLE_CLICK
/*
 * Return the duration from t1 to t2 in milliseconds.
 */
    સ્થિર long
time_diff_ms(સંરચના timeval *t1, સંરચના timeval *t2)
{
    // This handles wrapping of tv_usec correctly without any special case.
    // Example of 2 pairs (tv_sec, tv_usec) with a duration of 5 ms:
    //	   t1 = (1, 998000) t2 = (2, 3000) gives:
    //	   (2 - 1) * 1000 + (3000 - 998000) / 1000 -> 5 ms.
    પાછા (t2->tv_sec - t1->tv_sec) * 1000
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
    સ્થિર પૂર્ણાંક
get_mouse_class(char_u *p)
{
    પૂર્ણાંક		c;

    જો (has_mbyte && MB_BYTE2LEN(p[0]) > 1)
	પાછા mb_get_class(p);

    c = *p;
    જો (c == ' ' || c == '\t')
	પાછા 0;

    જો (vim_iswordc(c))
	પાછા 2;

    // There are a few special cases where we want certain combinations of
    // characters to be considered as a single word.  These are things like
    // "->", "/ *", "*=", "+=", "&=", "<=", ">=", "!=" etc.  Otherwise, each
    // character is in its own class.
    જો (c != NUL && vim_strchr((char_u *)"-+*/%<>&|^!=", c) != ખાલી)
	પાછા 1;
    પાછા c;
}

/*
 * Move "pos" back to the start of the word it's in.
 */
    સ્થિર શૂન્ય
find_start_of_word(pos_T *pos)
{
    char_u	*લીટી;
    પૂર્ણાંક		cclass;
    પૂર્ણાંક		col;

    લીટી = ml_get(pos->lnum);
    cclass = get_mouse_class(લીટી + pos->col);

    જ્યારે (pos->col > 0)
    {
	col = pos->col - 1;
	col -= (*mb_head_off)(લીટી, લીટી + col);
	જો (get_mouse_class(લીટી + col) != cclass)
	    તોડો;
	pos->col = col;
    }
}

/*
 * Move "pos" forward to the end of the word it's in.
 * When 'selection' is "exclusive", the position is just after the word.
 */
    સ્થિર શૂન્ય
find_end_of_word(pos_T *pos)
{
    char_u	*લીટી;
    પૂર્ણાંક		cclass;
    પૂર્ણાંક		col;

    લીટી = ml_get(pos->lnum);
    જો (*p_sel == 'e' && pos->col > 0)
    {
	--pos->col;
	pos->col -= (*mb_head_off)(લીટી, લીટી + pos->col);
    }
    cclass = get_mouse_class(લીટી + pos->col);
    જ્યારે (લીટી[pos->col] != NUL)
    {
	col = pos->col + (*mb_ptr2len)(લીટી + pos->col);
	જો (get_mouse_class(લીટી + col) != cclass)
	{
	    જો (*p_sel == 'e')
		pos->col = col;
	    તોડો;
	}
	pos->col = col;
    }
}

#જો defined(FEAT_GUI_MOTIF) || defined(FEAT_GUI_GTK) \
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
    સ્થિર પૂર્ણાંક
get_fpos_of_mouse(pos_T *mpos)
{
    win_T	*wp;
    પૂર્ણાંક		row = mouse_row;
    પૂર્ણાંક		col = mouse_col;

    જો (row < 0 || col < 0)		// check if it makes sense
	પાછા IN_UNKNOWN;

    // find the window where the row is in
    wp = mouse_find_win(&row, &col, FAIL_POPUP);
    જો (wp == ખાલી)
	પાછા IN_UNKNOWN;
    // winpos and height may change in win_enter()!
    જો (row >= wp->w_height)	// In (or below) status line
	પાછા IN_STATUS_લીટી;
    જો (col >= wp->w_width)	// In vertical separator line
	પાછા IN_SEP_લીટી;

    જો (wp != curwin)
	પાછા IN_UNKNOWN;

    // compute the position in the buffer line from the posn on the screen
    જો (mouse_comp_pos(curwin, &row, &col, &mpos->lnum, ખાલી))
	પાછા IN_STATUS_લીટી; // past bottom

    mpos->col = vcol2col(wp, mpos->lnum, col, &mpos->coladd);
    પાછા IN_BUFFER;
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
    પૂર્ણાંક
do_mouse(
    oparg_T	*oap,		// operator argument, can be NULL
    પૂર્ણાંક		c,		// K_LEFTMOUSE, etc
    પૂર્ણાંક		dir,		// Direction to 'put' if necessary
    long	ગણતરી,
    પૂર્ણાંક		fixindent)	// PUT_FIXINDENT if fixing indent necessary
{
    સ્થિર પૂર્ણાંક	do_always = FALSE;	// ignore 'mouse' setting next time
    સ્થિર પૂર્ણાંક	got_click = FALSE;	// got a click some time back

    પૂર્ણાંક		which_button;	// MOUSE_LEFT, _MIDDLE or _RIGHT
    પૂર્ણાંક		is_click = FALSE; // If FALSE it's a drag or release event
    પૂર્ણાંક		is_drag = FALSE;  // If TRUE it's a drag event
    પૂર્ણાંક		jump_flags = 0;	// flags for jump_to_mouse()
    pos_T	start_visual;
    પૂર્ણાંક		moved;		// Has cursor moved?
    પૂર્ણાંક		in_status_લીટી;	// mouse in status line
    સ્થિર પૂર્ણાંક	in_tab_લીટી = FALSE; // mouse clicked in tab line
    સ્થિર પૂર્ણાંક	in_tabpanel = FALSE; // mouse clicked in tabpanel
#ifdef FEAT_TABPANEL
    સ્થિર bool	in_tabpanel_scrollbar = false; // dragging tabpanel scrollbar
#endif
    પૂર્ણાંક		in_sep_લીટી;	// mouse in vertical separator line
    પૂર્ણાંક		c1, c2;
#જો defined(FEAT_FOLDING)
    pos_T	save_cursor;
#endif
    win_T	*old_curwin = curwin;
    સ્થિર pos_T orig_cursor;
    colnr_T	leftcol, rightcol;
    pos_T	end_visual;
    પૂર્ણાંક		diff;
    પૂર્ણાંક		old_active = VIsual_active;
    પૂર્ણાંક		old_mode = VIsual_mode;
    પૂર્ણાંક		regname;

#જો defined(FEAT_FOLDING)
    save_cursor = curwin->w_cursor;
#endif

    // When GUI is active, always recognize mouse events, otherwise:
    // - Ignore mouse event in normal mode if 'mouse' doesn't include 'n'.
    // - Ignore mouse event in visual mode if 'mouse' doesn't include 'v'.
    // - For command line and insert mode 'mouse' is checked before calling
    //	 do_mouse().
    જો (do_always)
	do_always = FALSE;
    નહીંતર
#ifdef FEAT_GUI
	જો (!gui.in_use)
#endif
	{
	    જો (VIsual_active)
	    {
		જો (!mouse_has(MOUSE_VISUAL))
		    પાછા FALSE;
	    }
	    નહીંતર જો (State == MODE_NORMAL && !mouse_has(MOUSE_NORMAL))
		પાછા FALSE;
	}

    માટે (;;)
    {
	which_button = get_mouse_button(KEY2TERMCAP1(c), &is_click, &is_drag);
	જો (is_drag)
	{
	    // If the next character is the same mouse event then use that
	    // one. Speeds up dragging the status line.
	    // Note: Since characters added to the stuff buffer in the code
	    // below need to come before the next character, do not do this
	    // when the current character was stuffed.
	    જો (!ચાવી_Stuffed && vpeekc() != NUL)
	    {
		પૂર્ણાંક nc;
		પૂર્ણાંક save_mouse_row = mouse_row;
		પૂર્ણાંક save_mouse_col = mouse_col;

		// Need to get the character, peeking doesn't get the actual
		// one.
		nc = safe_vgetc();
		જો (c == nc)
		    ચાલુ;
		vungetc(nc);
		mouse_row = save_mouse_row;
		mouse_col = save_mouse_col;
	    }
	}
	તોડો;
    }

    જો (c == K_MOUSEMOVE)
    {
	// Mouse moved without a button pressed.
#ifdef FEAT_BEVAL_TERM
	ui_may_remove_balloon();
	જો (p_bevalterm)
	{
	    profile_setlimit(p_bdlay, &bevalexpr_due);
	    bevalexpr_due_set = TRUE;
	}
#endif
#ifdef FEAT_PROP_POPUP
	popup_handle_mouse_moved();
#endif
	પાછા FALSE;
    }

#ifdef FEAT_MOUSESHAPE
    // May have stopped dragging the status or separator line.  The pointer is
    // most likely still on the status or separator line.
    જો (!is_drag && drag_status_લીટી)
    {
	drag_status_લીટી = FALSE;
	update_mouseshape(SHAPE_IDX_STATUS);
    }
    જો (!is_drag && drag_sep_લીટી)
    {
	drag_sep_લીટી = FALSE;
	update_mouseshape(SHAPE_IDX_VSEP);
    }
#endif

    // Ignore drag and release events if we didn't get a click.
    જો (is_click)
    {
	got_click = TRUE;
	in_tab_લીટી = FALSE;
	in_tabpanel = FALSE;
#ifdef FEAT_TABPANEL
	in_tabpanel_scrollbar = false;
#endif
    }
    નહીંતર
    {
	જો (!got_click)			// didn't get click, ignore
	    પાછા FALSE;
	જો (!is_drag)			// release, reset got_click
	{
	    got_click = FALSE;
	    જો (in_tab_લીટી || in_tabpanel
#ifdef FEAT_TABPANEL
		    || in_tabpanel_scrollbar
#endif
		    )
	    {
		in_tab_લીટી = FALSE;
		in_tabpanel = FALSE;
#ifdef FEAT_TABPANEL
		in_tabpanel_scrollbar = false;
#endif
		પાછા FALSE;
	    }
	}
    }

#ifdef FEAT_TABPANEL
    // Continue a scrollbar drag before any tab-selection handling.
    જો (is_drag && in_tabpanel_scrollbar)
    {
	tabpanel_drag_scrollbar(mouse_row);
	પાછા FALSE;
    }
#endif

    // CTRL right mouse button does CTRL-T
    જો (is_click && (mod_mask & MOD_MASK_CTRL) && which_button == MOUSE_જમણું)
    {
	જો (State & MODE_ઉમેરો)
	    stuffcharReadbuff(Ctrl_O);
	જો (ગણતરી > 1)
	    stuffnumReadbuff(ગણતરી);
	stuffcharReadbuff(Ctrl_T);
	got_click = FALSE;		// ignore drag&release now
	પાછા FALSE;
    }

    // CTRL only works with left mouse button
    જો ((mod_mask & MOD_MASK_CTRL) && which_button != MOUSE_ડાબું)
	પાછા FALSE;

    // When a modifier is down, ignore drag and release events, as well as
    // multiple clicks and the middle mouse button.
    // Accept shift-leftmouse drags when 'mousemodel' is "popup.*".
    જો ((mod_mask & (MOD_MASK_SHIFT | MOD_MASK_CTRL | MOD_MASK_ALT
							     | MOD_MASK_META))
	    && (!is_click
		|| (mod_mask & MOD_MASK_MULTI_CLICK)
		|| which_button == MOUSE_મધ્ય)
	    && !((mod_mask & (MOD_MASK_SHIFT|MOD_MASK_ALT))
		&& mouse_model_popup()
		&& which_button == MOUSE_ડાબું)
	    && !((mod_mask & MOD_MASK_ALT)
		&& !mouse_model_popup()
		&& which_button == MOUSE_જમણું)
	    )
	પાછા FALSE;

    // If the button press was used as the movement command for an operator
    // (eg "d<MOUSE>"), or it is the middle button that is held down, ignore
    // drag/release events.
    જો (!is_click && which_button == MOUSE_મધ્ય)
	પાછા FALSE;

    જો (oap != ખાલી)
	regname = oap->regname;
    નહીંતર
	regname = 0;

    // Middle mouse button does a 'put' of the selected text
    જો (which_button == MOUSE_મધ્ય)
    {
	જો (State == MODE_NORMAL)
	{
	    // If an operator was pending, we don't know what the user wanted
	    // to do. Go back to normal mode: Clear the operator and beep().
	    જો (oap != ખાલી && oap->op_type != OP_NOP)
	    {
		clearopbeep(oap);
		પાછા FALSE;
	    }

	    // If visual was active, yank the highlighted text and put it
	    // before the mouse pointer position.
	    // In Select mode replace the highlighted text with the clipboard.
	    જો (VIsual_active)
	    {
		જો (VIsual_select)
		{
		    stuffcharReadbuff(Ctrl_G);
		    stuffReadbuff((char_u *)"\"+p");
		}
		નહીંતર
		{
		    stuffcharReadbuff('y');
		    stuffcharReadbuff(K_MIDDLEMOUSE);
		}
		do_always = TRUE;	// ignore 'mouse' setting next time
		પાછા FALSE;
	    }
	    // The rest is below jump_to_mouse()
	}

	નહીંતર જો ((State & MODE_ઉમેરો) == 0)
	    પાછા FALSE;

	// Middle click in insert mode doesn't move the mouse, just insert the
	// contents of a register.  '.' register is special, can't insert that
	// with do_put().
	// Also paste at the cursor if the current mode isn't in 'mouse' (only
	// happens for the GUI).
	જો ((State & MODE_ઉમેરો) || !mouse_has(MOUSE_NORMAL))
	{
	    જો (regname == '.')
		ઉમેરો_reg(regname, TRUE);
	    નહીંતર
	    {
#ifdef FEAT_CLIPBOARD
		જો (clip_star.available && regname == 0)
		    regname = '*';
#endif
		જો ((State & REPLACE_FLAG) && !yank_register_mline(regname))
		    ઉમેરો_reg(regname, TRUE);
		નહીંતર
		{
		    do_put(regname, ખાલી, BACKWARD, 1L,
						      fixindent | PUT_CURSEND);

		    // Repeat it with CTRL-R CTRL-O r or CTRL-R CTRL-P r
		    AppendCharToRedobuff(Ctrl_R);
		    AppendCharToRedobuff(fixindent ? Ctrl_P : Ctrl_O);
		    AppendCharToRedobuff(regname == 0 ? '"' : regname);
		}
	    }
	    પાછા FALSE;
	}
    }

    // When dragging or button-up stay in the same window.
    જો (!is_click)
	jump_flags |= MOUSE_FOCUS | MOUSE_DID_MOVE;

    start_visual.lnum = 0;

    સંરચના tabpage_label_info {
	bool is_panel;	    // label type. true: tabpanel, false: tab line
	bool just_in;	    // just in tabpage label area
	bool just_click;    // just click tabpage label area
	પૂર્ણાંક nr;		    // tabpage number
    } tp_label = { false, false, false, 0 };

    // Check for clicking in the tab page panel.
#જો defined(FEAT_TABPANEL)
    જો (mouse_col < firstwin->w_wincol
		|| mouse_col >= firstwin->w_wincol + topframe->fr_width)
    {
	// A click on the scrollbar column starts a drag interaction and
	// preempts tab-selection.
	જો (is_click && !is_drag && mouse_on_tabpanel_scrollbar())
	{
	    in_tabpanel_scrollbar = TRUE;
	    tabpanel_drag_scrollbar(mouse_row);
	    પાછા FALSE;
	}

	// Dispatch 'tabpanel' %[FuncName] click regions before falling through
	// to tab-page selection.  On drag events fall through to the normal
	// tab-drag handling.
	જો (is_click && !is_drag
		&& stl_click_handler_regions(tabpanel_stl_click,
					    tabpanel_stl_click_ગણતરી,
					    0, (char_u *)"tabpanel",
					    mouse_row, mouse_col,
					    which_button, mod_mask))
	    પાછા FALSE;

	tp_label.is_panel = true;
	tp_label.just_in = true;
	tp_label.nr = get_tabpagenr_on_tabpanel();

	// click in a tab selects that tab page
	જો (is_click && cmdwin_type == 0)
	    tp_label.just_click = true;
    }
    નહીંતર
#endif
    // Check for clicking in the tab page line.
    જો (TabPageIdxs != ખાલી && mouse_row == 0 && firstwin->w_winrow > 0)
    {
	// Dispatch 'tabline' %[FuncName] click regions before falling through
	// to tab-page selection.  On drag events fall through to the normal
	// tab-drag handling.
	જો (is_click && !is_drag
		&& stl_click_handler_regions(tabline_stl_click,
					    tabline_stl_click_ગણતરી,
					    0, (char_u *)"tabline",
					    mouse_row, mouse_col,
					    which_button, mod_mask))
	    પાછા FALSE;

	tp_label.just_in = true;
	tp_label.nr = TabPageIdxs[mouse_col];

	// click in a tab selects that tab page
	જો (is_click && cmdwin_type == 0
		&& mouse_col < firstwin->w_wincol + topframe->fr_width)
	    tp_label.just_click = true;
    }

    જો (tp_label.just_in)
    {
	જો (is_drag)
	{
	    જો (in_tabpanel || in_tab_લીટી)
	    {
		c1 = tp_label.nr;
		tabpage_move(c1 <= 0 ? 9999 : c1 < tabpage_index(curtab)
							    ? c1 - 1 : c1);
	    }
	    પાછા FALSE;
	}

	જો (tp_label.just_click)
	{
	    જો (tp_label.is_panel)
		in_tabpanel = TRUE;
	    નહીંતર
		in_tab_લીટી = TRUE;
	    c1 = tp_label.nr;
	    જો (c1 >= 0)
	    {
		જો ((mod_mask & MOD_MASK_MULTI_CLICK) == MOD_MASK_2CLICK
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
		નહીંતર
		{
		    // Go to specified tab page, or next one if not clicking
		    // on a label.
		    goto_tabpage(c1);

		    // It's like clicking on the status line of a window.
		    જો (curwin != old_curwin)
			end_visual_mode_keep_button();
		}
	    }
	    નહીંતર
	    {
		tabpage_T	*tp;

		// Close the current or specified tab page.
		જો (c1 == -999)
		    tp = curtab;
		નહીંતર
		    tp = find_tabpage(-c1);
		જો (tp == curtab)
		{
		    જો (first_tabpage->tp_next != ખાલી)
			tabpage_close(FALSE);
		}
		નહીંતર જો (tp != ખાલી)
		    tabpage_close_other(tp, FALSE);
	    }
	}
	પાછા TRUE;
    }
    નહીંતર જો (is_drag && (in_tabpanel || (in_tab_લીટી && TabPageIdxs != ખાલી)))
    {
#જો defined(FEAT_TABPANEL)
	જો (in_tabpanel)
	    c1 = get_tabpagenr_on_tabpanel();
	નહીંતર
#endif
	    c1 = TabPageIdxs[mouse_col];
	tabpage_move(c1 <= 0 ? 9999 : c1 - 1);
	પાછા FALSE;
    }

    // When 'mousemodel' is "popup" or "popup_setpos", translate mouse events:
    // right button up   -> pop-up menu
    // shift-left button -> right button
    // alt-left button   -> alt-right button
    જો (mouse_model_popup())
    {
	જો (which_button == MOUSE_જમણું
			    && !(mod_mask & (MOD_MASK_SHIFT | MOD_MASK_CTRL)))
	{
#ifdef USE_POPUP_SETPOS
# ifdef FEAT_GUI
	    જો (gui.in_use)
	    {
#  જો defined(FEAT_GUI_MOTIF) || defined(FEAT_GUI_GTK) \
			  || defined(FEAT_GUI_PHOTON)
		જો (!is_click)
		    // Ignore right button release events, only shows the popup
		    // menu on the button down event.
		    પાછા FALSE;
#  endif
#  જો defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_HAIKU)
		જો (is_click || is_drag)
		    // Ignore right button down and drag mouse events.  Windows
		    // only shows the popup menu on the button up event.
		    પાછા FALSE;
#  endif
	    }
# endif
# જો defined(FEAT_GUI) && defined(FEAT_TERM_POPUP_MENU)
	    નહીંતર
# endif
# જો defined(FEAT_TERM_POPUP_MENU)
	    જો (!is_click)
		// Ignore right button release events, only shows the popup
		// menu on the button down event.
		પાછા FALSE;
# endif

	    jump_flags = 0;
	    જો (STRCMP(p_mousem, "popup_setpos") == 0)
	    {
		// First set the cursor position before showing the popup
		// menu.
		જો (VIsual_active)
		{
		    pos_T    m_pos;

		    // set MOUSE_MAY_STOP_VIS if we are outside the
		    // selection or the current window (might have false
		    // negative here)
		    જો (mouse_row < curwin->w_winrow
			 || mouse_row
				  > (curwin->w_winrow + curwin->w_height))
			jump_flags = MOUSE_MAY_STOP_VIS;
		    નહીંતર જો (get_fpos_of_mouse(&m_pos) != IN_BUFFER)
			jump_flags = MOUSE_MAY_STOP_VIS;
		    નહીંતર
		    {
			જો (VIsual_mode == 'V')
			{
			    જો ((curwin->w_cursor.lnum <= VIsual.lnum
				    && (m_pos.lnum < curwin->w_cursor.lnum
					|| VIsual.lnum < m_pos.lnum))
				|| (VIsual.lnum < curwin->w_cursor.lnum
				    && (m_pos.lnum < VIsual.lnum
					|| curwin->w_cursor.lnum < m_pos.lnum)))
			    {
				jump_flags = MOUSE_MAY_STOP_VIS;
			    }
			}
			નહીંતર જો ((LTOREQ_POS(curwin->w_cursor, VIsual)
				    && (LT_POS(m_pos, curwin->w_cursor)
					|| LT_POS(VIsual, m_pos)))
				|| (LT_POS(VIsual, curwin->w_cursor)
				    && (LT_POS(m_pos, VIsual)
				      || LT_POS(curwin->w_cursor, m_pos))))
			{
			    jump_flags = MOUSE_MAY_STOP_VIS;
			}
			નહીંતર જો (VIsual_mode == Ctrl_V)
			{
			    getvcols(curwin, &curwin->w_cursor, &VIsual,
						     &leftcol, &rightcol, 0);
			    getvcol(curwin, &m_pos, ખાલી, &m_pos.col, ખાલી, 0);
			    જો (m_pos.col < leftcol || m_pos.col > rightcol)
				jump_flags = MOUSE_MAY_STOP_VIS;
			}
		    }
		}
		નહીંતર
		    jump_flags = MOUSE_MAY_STOP_VIS;
	    }
	    જો (jump_flags)
	    {
		jump_flags = jump_to_mouse(jump_flags, ખાલી, which_button);
		update_curbuf(VIsual_active ? UPD_INVERTED : UPD_VALID);
		setcursor();
		out_flush();    // Update before showing popup menu
	    }
# ifdef FEAT_MENU
	    show_popupmenu();
	    got_click = FALSE;	// ignore release events
# endif
	    પાછા (jump_flags & CURSOR_MOVED) != 0;
#નહીંતર
	    પાછા FALSE;
#endif
	}
	જો (which_button == MOUSE_ડાબું
				&& (mod_mask & (MOD_MASK_SHIFT|MOD_MASK_ALT)))
	{
	    which_button = MOUSE_જમણું;
	    mod_mask &= ~MOD_MASK_SHIFT;
	}
    }

    જો ((State & (MODE_NORMAL | MODE_ઉમેરો))
			    && !(mod_mask & (MOD_MASK_SHIFT | MOD_MASK_CTRL)))
    {
	જો (which_button == MOUSE_ડાબું)
	{
	    જો (is_click)
	    {
		// stop Visual mode for a left click in a window, but not when
		// on a status line
		જો (VIsual_active)
		    jump_flags |= MOUSE_MAY_STOP_VIS;
	    }
	    નહીંતર જો (mouse_has(MOUSE_VISUAL))
		jump_flags |= MOUSE_MAY_VIS;
	}
	નહીંતર જો (which_button == MOUSE_જમણું)
	{
	    જો (is_click && VIsual_active)
	    {
		// Remember the start and end of visual before moving the
		// cursor.
		જો (LT_POS(curwin->w_cursor, VIsual))
		{
		    start_visual = curwin->w_cursor;
		    end_visual = VIsual;
		}
		નહીંતર
		{
		    start_visual = VIsual;
		    end_visual = curwin->w_cursor;
		}
	    }
	    jump_flags |= MOUSE_FOCUS;
	    જો (mouse_has(MOUSE_VISUAL))
		jump_flags |= MOUSE_MAY_VIS;
	}
    }

    // If an operator is pending, ignore all drags and releases until the
    // next mouse click.
    જો (!is_drag && oap != ખાલી && oap->op_type != OP_NOP)
    {
	got_click = FALSE;
	oap->motion_type = MCHAR;
    }

    // When releasing the button let jump_to_mouse() know.
    જો (!is_click && !is_drag)
	jump_flags |= MOUSE_RELEASED;

    // JUMP!
    jump_flags = jump_to_mouse(jump_flags,
			oap == ખાલી ? ખાલી : &(oap->inclusive), which_button);

#ifdef FEAT_MENU
    // A click in the window toolbar has no side effects.
    જો (jump_flags & MOUSE_WINBAR)
	પાછા FALSE;
#endif
    moved = (jump_flags & CURSOR_MOVED);
    in_status_લીટી = (jump_flags & IN_STATUS_લીટી);
    in_sep_લીટી = (jump_flags & IN_SEP_લીટી);

    // Check for statusline click handler early, before visual mode or
    // other button-specific handling can interfere.
    જો (in_status_લીટી && is_click && !is_drag
	    && stl_click_handler(dragwin, mouse_row, mouse_col,
						which_button, mod_mask))
    {
#ifdef FEAT_MOUSESHAPE
	જો (!drag_status_લીટી)
	{
	    drag_status_લીટી = TRUE;
	    update_mouseshape(-1);
	}
#endif
	પાછા FALSE;
    }

#ifdef FEAT_NETBEANS_INTG
    જો (isNetbeansBuffer(curbuf)
			    && !(jump_flags & (IN_STATUS_લીટી | IN_SEP_લીટી)))
    {
	પૂર્ણાંક ચાવી = KEY2TERMCAP1(c);

	જો (ચાવી == (પૂર્ણાંક)KE_LEFTRELEASE || ચાવી == (પૂર્ણાંક)KE_MIDDLERELEASE
					       || ચાવી == (પૂર્ણાંક)KE_RIGHTRELEASE)
	    netbeans_button_release(which_button);
    }
#endif

    // When jumping to another window, clear a pending operator.  That's a bit
    // friendlier than beeping and not jumping to that window.
    જો (curwin != old_curwin && oap != ખાલી && oap->op_type != OP_NOP)
	clearop(oap);

#ifdef FEAT_FOLDING
    જો (mod_mask == 0
	    && !is_drag
	    && (jump_flags & (MOUSE_FOLD_CLOSE | MOUSE_FOLD_OPEN))
	    && which_button == MOUSE_ડાબું)
    {
	// open or close a fold at this line
	જો (jump_flags & MOUSE_FOLD_OPEN)
	    openFold(curwin->w_cursor.lnum, 1L);
	નહીંતર
	    closeFold(curwin->w_cursor.lnum, 1L);
	// don't move the cursor if still in the same window
	જો (curwin == old_curwin)
	    curwin->w_cursor = save_cursor;
    }
#endif

#જો defined(FEAT_CLIPBOARD)
    જો ((jump_flags & IN_OTHER_WIN) && !VIsual_active &&
	    (clip_star.available || clip_plus.available))
    {
	clip_modeless(which_button, is_click, is_drag);
	પાછા FALSE;
    }
#endif

    // Set global flag that we are extending the Visual area with mouse
    // dragging; temporarily minimize 'scrolloff'.
    જો (VIsual_active && is_drag && get_scrolloff_કિંમત())
    {
	// In the very first line, allow scrolling one line
	જો (mouse_row == 0)
	    mouse_dragging = 2;
	નહીંતર
	    mouse_dragging = 1;
    }

    // When dragging the mouse above the window, scroll down.
    જો (is_drag && mouse_row < 0 && !in_status_લીટી)
    {
	scroll_redraw(FALSE, 1L);
	mouse_row = 0;
    }

    જો (start_visual.lnum)		// right click in visual mode
    {
       // When ALT is pressed make Visual mode blockwise.
       જો (mod_mask & MOD_MASK_ALT)
	   VIsual_mode = Ctrl_V;

	// In Visual-block mode, divide the area in four, pick up the corner
	// that is in the quarter that the cursor is in.
	જો (VIsual_mode == Ctrl_V)
	{
	    getvcols(curwin, &start_visual, &end_visual,
						    &leftcol, &rightcol, 0);
	    જો (curwin->w_curswant > (leftcol + rightcol) / 2)
		end_visual.col = leftcol;
	    નહીંતર
		end_visual.col = rightcol;
	    જો (curwin->w_cursor.lnum >=
				    (start_visual.lnum + end_visual.lnum) / 2)
		end_visual.lnum = start_visual.lnum;

	    // move VIsual to the right column
	    start_visual = curwin->w_cursor;	    // save the cursor pos
	    curwin->w_cursor = end_visual;
	    coladvance(end_visual.col);
	    VIsual = curwin->w_cursor;
	    curwin->w_cursor = start_visual;	    // restore the cursor
	}
	નહીંતર
	{
	    // If the click is before the start of visual, change the start.
	    // If the click is after the end of visual, change the end.  If
	    // the click is inside the visual, change the closest side.
	    જો (LT_POS(curwin->w_cursor, start_visual))
		VIsual = end_visual;
	    નહીંતર જો (LT_POS(end_visual, curwin->w_cursor))
		VIsual = start_visual;
	    નહીંતર
	    {
		// In the same line, compare column number
		જો (end_visual.lnum == start_visual.lnum)
		{
		    જો (curwin->w_cursor.col - start_visual.col >
				    end_visual.col - curwin->w_cursor.col)
			VIsual = start_visual;
		    નહીંતર
			VIsual = end_visual;
		}

		// In different lines, compare line number
		નહીંતર
		{
		    diff = (curwin->w_cursor.lnum - start_visual.lnum) -
				(end_visual.lnum - curwin->w_cursor.lnum);

		    જો (diff > 0)		// closest to end
			VIsual = start_visual;
		    નહીંતર જો (diff < 0)	// closest to start
			VIsual = end_visual;
		    નહીંતર			// in the middle line
		    {
			જો (curwin->w_cursor.col <
					(start_visual.col + end_visual.col) / 2)
			    VIsual = end_visual;
			નહીંતર
			    VIsual = start_visual;
		    }
		}
	    }
	}
    }
    // If Visual mode started in insert mode, execute "CTRL-O"
    નહીંતર જો ((State & MODE_ઉમેરો) && VIsual_active)
	stuffcharReadbuff(Ctrl_O);

    // Middle mouse click: Put text before cursor.
    જો (which_button == MOUSE_મધ્ય)
    {
#ifdef FEAT_CLIPBOARD
	જો (clip_star.available && regname == 0)
	    regname = '*';
#endif
	જો (yank_register_mline(regname))
	{
	    જો (mouse_past_bottom)
		dir = FORWARD;
	}
	નહીંતર જો (mouse_past_eol)
	    dir = FORWARD;

	જો (fixindent)
	{
	    c1 = (dir == BACKWARD) ? '[' : ']';
	    c2 = 'p';
	}
	નહીંતર
	{
	    c1 = (dir == FORWARD) ? 'p' : 'P';
	    c2 = NUL;
	}
	prep_redo(regname, ગણતરી, NUL, c1, NUL, c2, NUL);

	// Remember where the paste started, so in edit() Insstart can be set
	// to this position
	જો (restart_edit != 0)
	    where_paste_started = curwin->w_cursor;
	do_put(regname, ખાલી, dir, ગણતરી, fixindent | PUT_CURSEND);
    }

#જો defined(FEAT_QUICKFIX)
    // Ctrl-Mouse click or double click in a quickfix window jumps to the
    // error under the mouse pointer.
    નહીંતર જો (((mod_mask & MOD_MASK_CTRL)
		|| (mod_mask & MOD_MASK_MULTI_CLICK) == MOD_MASK_2CLICK)
	    && bt_quickfix(curbuf))
    {
	જો (curwin->w_llist_ref == ખાલી)	// quickfix window
	    do_cmdline_cmd((char_u *)".cc");
	નહીંતર					// location list window
	    do_cmdline_cmd((char_u *)".ll");
	got_click = FALSE;		// ignore drag&release now
    }
#endif

    // Ctrl-Mouse click (or double click in a help window) jumps to the tag
    // under the mouse pointer.
    નહીંતર જો ((mod_mask & MOD_MASK_CTRL) || (curbuf->b_help
		     && (mod_mask & MOD_MASK_MULTI_CLICK) == MOD_MASK_2CLICK))
    {
	જો (State & MODE_ઉમેરો)
	    stuffcharReadbuff(Ctrl_O);
	stuffcharReadbuff(Ctrl_RSB);
	got_click = FALSE;		// ignore drag&release now
    }

    // Shift-Mouse click searches for the next occurrence of the word under
    // the mouse pointer
    નહીંતર જો ((mod_mask & MOD_MASK_SHIFT))
    {
	જો ((State & MODE_ઉમેરો) || (VIsual_active && VIsual_select))
	    stuffcharReadbuff(Ctrl_O);
	જો (which_button == MOUSE_ડાબું)
	    stuffcharReadbuff('*');
	નહીંતર	// MOUSE_RIGHT
	    stuffcharReadbuff('#');
    }

    // Handle double clicks, unless on status line
    નહીંતર જો (in_status_લીટી)
    {
#ifdef FEAT_MOUSESHAPE
	જો ((is_drag || is_click) && !drag_status_લીટી)
	{
	    drag_status_લીટી = TRUE;
	    update_mouseshape(-1);
	}
#endif
    }
    નહીંતર જો (in_sep_લીટી)
    {
#ifdef FEAT_MOUSESHAPE
	જો ((is_drag || is_click) && !drag_sep_લીટી)
	{
	    drag_sep_લીટી = TRUE;
	    update_mouseshape(-1);
	}
#endif
    }
    નહીંતર જો ((mod_mask & MOD_MASK_MULTI_CLICK)
				       && (State & (MODE_NORMAL | MODE_ઉમેરો))
	     && mouse_has(MOUSE_VISUAL))
    {
	જો (is_click || !VIsual_active)
	{
	    જો (VIsual_active)
		orig_cursor = VIsual;
	    નહીંતર
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
	    જો ((mod_mask & MOD_MASK_MULTI_CLICK) == MOD_MASK_2CLICK)
	    {
		// Double click with ALT pressed makes it blockwise.
		જો (mod_mask & MOD_MASK_ALT)
		    VIsual_mode = Ctrl_V;
		નહીંતર
		    VIsual_mode = 'v';
	    }
	    નહીંતર જો ((mod_mask & MOD_MASK_MULTI_CLICK) == MOD_MASK_3CLICK)
		VIsual_mode = 'V';
	    નહીંતર જો ((mod_mask & MOD_MASK_MULTI_CLICK) == MOD_MASK_4CLICK)
		VIsual_mode = Ctrl_V;
#ifdef FEAT_CLIPBOARD
	    // Make sure the clipboard gets updated.  Needed because start and
	    // end may still be the same, and the selection needs to be owned
	    clip_star.vmode = NUL;
#endif
	}
	// A double click selects a word or a block.
	જો ((mod_mask & MOD_MASK_MULTI_CLICK) == MOD_MASK_2CLICK)
	{
	    pos_T	*pos = ખાલી;
	    પૂર્ણાંક		gc;

	    જો (is_click)
	    {
		// If the character under the cursor (skipping white space) is
		// not a word character, try finding a match and select a (),
		// {}, [], #if/#endif, etc. block.
		end_visual = curwin->w_cursor;
		જ્યારે (gc = gchar_pos(&end_visual), VIM_ISWHITE(gc))
		    inc(&end_visual);
		જો (oap != ખાલી)
		    oap->motion_type = MCHAR;
		જો (oap != ખાલી
			&& VIsual_mode == 'v'
			&& !vim_iswordc(gchar_pos(&end_visual))
			&& EQUAL_POS(curwin->w_cursor, VIsual)
			&& (pos = findmatch(oap, NUL)) != ખાલી)
		{
		    curwin->w_cursor = *pos;
		    જો (oap->motion_type == MLINE)
			VIsual_mode = 'V';
		    નહીંતર જો (*p_sel == 'e')
		    {
			જો (LT_POS(curwin->w_cursor, VIsual))
			    ++VIsual.col;
			નહીંતર
			    ++curwin->w_cursor.col;
		    }
		}
	    }

	    જો (pos == ખાલી && (is_click || is_drag))
	    {
		// When not found a match or when dragging: extend to include
		// a word.
		જો (LT_POS(curwin->w_cursor, orig_cursor))
		{
		    find_start_of_word(&curwin->w_cursor);
		    find_end_of_word(&VIsual);
		}
		નહીંતર
		{
		    find_start_of_word(&VIsual);
		    જો (*p_sel == 'e' && *ml_get_cursor() != NUL)
			curwin->w_cursor.col +=
					 (*mb_ptr2len)(ml_get_cursor());
		    find_end_of_word(&curwin->w_cursor);
		}
	    }
	    curwin->w_set_curswant = true;
	}
	જો (is_click)
	    redraw_curbuf_later(UPD_INVERTED);	// update the inversion
    }
    નહીંતર જો (VIsual_active && !old_active)
    {
	જો (mod_mask & MOD_MASK_ALT)
	    VIsual_mode = Ctrl_V;
	નહીંતર
	    VIsual_mode = 'v';
    }

    // If Visual mode changed show it later.
    જો ((!VIsual_active && old_active && mode_displayed)
	    || (VIsual_active && p_smd && સંદેશ_silent == 0
				 && (!old_active || VIsual_mode != old_mode)))
	redraw_cmdline = TRUE;

    પાછા moved;
}

    શૂન્ય
ins_mouse(પૂર્ણાંક c)
{
    pos_T	tpos;
    win_T	*old_curwin = curwin;

#ifdef FEAT_GUI
    // When GUI is active, also move/paste when 'mouse' is empty
    જો (!gui.in_use)
#endif
	જો (!mouse_has(MOUSE_ઉમેરો))
	    પાછા;

    undisplay_dollar();
    tpos = curwin->w_cursor;
    જો (do_mouse(ખાલી, c, BACKWARD, 1L, 0))
    {
	win_T	*new_curwin = curwin;

	જો (curwin != old_curwin && win_valid(old_curwin))
	{
	    // Mouse took us to another window.  We need to go back to the
	    // previous one to stop insert there properly.
	    curwin = old_curwin;
	    curbuf = curwin->w_buffer;
#ifdef FEAT_JOB_CHANNEL
	    જો (bt_prompt(curbuf))
		// Restart Insert mode when re-entering the prompt buffer.
		curbuf->b_prompt_ઉમેરો = 'A';
#endif
	}
	start_arrow(curwin == old_curwin ? &tpos : ખાલી);
	જો (curwin != new_curwin && win_valid(new_curwin))
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
    સ્થિર શૂન્ય
do_mousescroll(cmdarg_T *cap)
{
    પૂર્ણાંક shift_or_ctrl = mod_mask & (MOD_MASK_SHIFT | MOD_MASK_CTRL);

#ifdef FEAT_TERMINAL
    જો (term_use_loop())
	// This window is a terminal window, send the mouse event there.
	// Set "typed" to FALSE to avoid an endless loop.
	send_ચાવીઓ_to_term(curbuf->b_term, cap->cmdchar, mod_mask, FALSE);
    નહીંતર
#endif
    જો (cap->arg == MSCR_UP || cap->arg == MSCR_DOWN)
    {
	// Vertical scrolling
	જો (!(State & MODE_ઉમેરો) && (mouse_vert_step < 0 || shift_or_ctrl))
	{
	    // whole page up or down
	    pagescroll(cap->arg == MSCR_UP ? FORWARD : BACKWARD, 1L, FALSE);
	}
	નહીંતર
	{
	    જો (mouse_vert_step < 0 || shift_or_ctrl)
	    {
		// whole page up or down
		cap->count1 = (long)(curwin->w_botline - curwin->w_topline);
	    }
	    // Don't scroll more than half the window height.
	    નહીંતર જો (curwin->w_height < mouse_vert_step * 2)
	    {
		cap->count1 = curwin->w_height / 2;
		જો (cap->count1 == 0)
		    cap->count1 = 1;
	    }
	    નહીંતર
	    {
		cap->count1 = mouse_vert_step;
	    }
	    cap->count0 = cap->count1;
	    nv_scroll_લીટી(cap);
	}

#ifdef FEAT_PROP_POPUP
	જો (WIN_IS_POPUP(curwin))
	    popup_set_firstline(curwin);
#endif
    }
    નહીંતર
    {
	// Horizontal scrolling
	long step = (mouse_hor_step < 0 || shift_or_ctrl)
					    ? curwin->w_width : mouse_hor_step;
	long leftcol = curwin->w_leftcol
				     + (cap->arg == MSCR_જમણું ? -step : step);
	જો (leftcol < 0)
	    leftcol = 0;
	do_mousescroll_horiz((long_u)leftcol);
    }
    may_trigger_win_scrolled_resized();
}

/*
 * Insert mode implementation for scrolling in direction "dir", which is
 * one of the MSCR_ values.
 */
    શૂન્ય
ins_mousescroll(પૂર્ણાંક dir)
{
    cmdarg_T	cap;
    oparg_T	oa;
    CLEAR_FIELD(cap);
    clear_oparg(&oa);
    cap.oap = &oa;
    cap.arg = dir;

#ifdef FEAT_TABPANEL
    જો (mouse_row >= 0 && mouse_col >= 0
	    && (dir == MSCR_UP || dir == MSCR_DOWN)
	    && mouse_on_tabpanel())
    {
	(શૂન્ય)tabpanel_scroll(dir == MSCR_UP ? 1 : -1,
		mouse_vert_step > 0 ? mouse_vert_step : 3);
	પાછા;
    }
#endif

    પસંદગી (dir)
    {
	સ્થિતિ MSCR_UP:
	    cap.cmdchar = K_MOUSEUP;
	    તોડો;
	સ્થિતિ MSCR_DOWN:
	    cap.cmdchar = K_MOUSEDOWN;
	    તોડો;
	સ્થિતિ MSCR_ડાબું:
	    cap.cmdchar = K_MOUSELEFT;
	    તોડો;
	સ્થિતિ MSCR_જમણું:
	    cap.cmdchar = K_MOUSERIGHT;
	    તોડો;
	મૂળભૂત:
	    siemsg("Invalid ins_mousescroll() argument: %d", dir);
    }

    win_T *old_curwin = curwin;
    જો (mouse_row >= 0 && mouse_col >= 0)
    {
	// Find the window at the mouse pointer coordinates.
	// NOTE: Must restore "curwin" to "old_curwin" before returning!
	પૂર્ણાંક row = mouse_row;
	પૂર્ણાંક col = mouse_col;
	curwin = mouse_find_win(&row, &col, FIND_POPUP);
	જો (curwin == ખાલી)
	{
	    curwin = old_curwin;
	    પાછા;
	}
	curbuf = curwin->w_buffer;
    }

    જો (curwin == old_curwin)
    {
	// Don't scroll the current window if the popup menu is visible.
	જો (pum_visible())
	    પાછા;

	undisplay_dollar();
    }

    linenr_T	orig_topline = curwin->w_topline;
    colnr_T	orig_leftcol = curwin->w_leftcol;
    pos_T	orig_cursor = curwin->w_cursor;

    // Call the common mouse scroll function shared with other modes.
    do_mousescroll(&cap);

    પૂર્ણાંક did_scroll = (orig_topline != curwin->w_topline
		   || orig_leftcol != curwin->w_leftcol);

    curwin->w_redr_status = true;
    curwin = old_curwin;
    curbuf = curwin->w_buffer;

    // If the window actually scrolled and the popup menu may overlay the
    // window, need to redraw it.
    જો (did_scroll && pum_visible())
    {
	// TODO: Would be more efficient to only redraw the windows that are
	// overlapped by the popup menu.
	redraw_all_later(UPD_NOT_VALID);
	ins_compl_show_pum();
    }

    જો (!EQUAL_POS(curwin->w_cursor, orig_cursor))
    {
	start_arrow(&orig_cursor);
	set_can_cindent(TRUE);
    }
}

#જો defined(FEAT_PROP_POPUP)
/*
 * Command-line mode implementation for scrolling in direction "dir", which is
 * one of the MSCR_ values.  Scrolls the completion info popup when the mouse
 * pointer is on top of it.
 * Returns TRUE when the info popup was scrolled.
 */
    પૂર્ણાંક
cmdline_mousescroll(પૂર્ણાંક dir)
{
    cmdarg_T	cap;
    oparg_T	oa;

    CLEAR_FIELD(cap);
    clear_oparg(&oa);
    cap.oap = &oa;
    cap.arg = dir;

    પસંદગી (dir)
    {
	સ્થિતિ MSCR_UP:	 cap.cmdchar = K_MOUSEUP; તોડો;
	સ્થિતિ MSCR_DOWN:	 cap.cmdchar = K_MOUSEDOWN; તોડો;
	સ્થિતિ MSCR_ડાબું:	 cap.cmdchar = K_MOUSELEFT; તોડો;
	સ્થિતિ MSCR_જમણું: cap.cmdchar = K_MOUSERIGHT; તોડો;
    }

    જો (mouse_row < 0 || mouse_col < 0)
	પાછા FALSE;

    પૂર્ણાંક	    row = mouse_row;
    પૂર્ણાંક	    col = mouse_col;
    win_T   *wp;

    // Only scroll when the mouse is on top of the info popup.
    wp = mouse_find_win(&row, &col, FIND_POPUP);
    જો (wp == ખાલી || !WIN_IS_POPUP(wp) || !(wp->w_popup_flags & POPF_INFO)
							|| !wp->w_has_scrollbar)
	પાછા FALSE;

    win_T   *old_curwin = curwin;

    curwin = wp;
    curbuf = wp->w_buffer;
    // Call the common mouse scroll function shared with other modes.
    do_mousescroll(&cap);
    curwin = old_curwin;
    curbuf = curwin->w_buffer;

    // Cmdline mode doesn't normally call update_screen(), so redraw the
    // completion popup menu, which also repaints the info popup.
    જો (cmdline_pum_active())
	cmdline_pum_display();
    પાછા TRUE;
}
#endif

/*
 * Return TRUE if "c" is a mouse key.
 */
    પૂર્ણાંક
is_mouse_ચાવી(પૂર્ણાંક c)
{
    પાછા c == K_LEFTMOUSE
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

સ્થિર સંરચના mousetable
{
    પૂર્ણાંક	    pseudo_code;	// Code for pseudo mouse event
    પૂર્ણાંક	    button;		// Which mouse button is it?
    પૂર્ણાંક	    is_click;		// Is it a mouse button click event?
    પૂર્ણાંક	    is_drag;		// Is it a mouse drag event?
} mouse_table[] =
{
    {(પૂર્ણાંક)KE_LEFTMOUSE,		MOUSE_ડાબું,	TRUE,	FALSE},
#ifdef FEAT_GUI
    {(પૂર્ણાંક)KE_LEFTMOUSE_NM,	MOUSE_ડાબું,	TRUE,	FALSE},
#endif
    {(પૂર્ણાંક)KE_LEFTDRAG,		MOUSE_ડાબું,	FALSE,	TRUE},
    {(પૂર્ણાંક)KE_LEFTRELEASE,	MOUSE_ડાબું,	FALSE,	FALSE},
#ifdef FEAT_GUI
    {(પૂર્ણાંક)KE_LEFTRELEASE_NM,	MOUSE_ડાબું,	FALSE,	FALSE},
#endif
    {(પૂર્ણાંક)KE_MIDDLEMOUSE,	MOUSE_મધ્ય,	TRUE,	FALSE},
    {(પૂર્ણાંક)KE_MIDDLEDRAG,	MOUSE_મધ્ય,	FALSE,	TRUE},
    {(પૂર્ણાંક)KE_MIDDLERELEASE,	MOUSE_મધ્ય,	FALSE,	FALSE},
    {(પૂર્ણાંક)KE_RIGHTMOUSE,	MOUSE_જમણું,	TRUE,	FALSE},
    {(પૂર્ણાંક)KE_RIGHTDRAG,		MOUSE_જમણું,	FALSE,	TRUE},
    {(પૂર્ણાંક)KE_RIGHTRELEASE,	MOUSE_જમણું,	FALSE,	FALSE},
    {(પૂર્ણાંક)KE_X1MOUSE,		MOUSE_X1,	TRUE,	FALSE},
    {(પૂર્ણાંક)KE_X1DRAG,		MOUSE_X1,	FALSE,	TRUE},
    {(પૂર્ણાંક)KE_X1RELEASE,		MOUSE_X1,	FALSE,	FALSE},
    {(પૂર્ણાંક)KE_X2MOUSE,		MOUSE_X2,	TRUE,	FALSE},
    {(પૂર્ણાંક)KE_X2DRAG,		MOUSE_X2,	FALSE,	TRUE},
    {(પૂર્ણાંક)KE_X2RELEASE,		MOUSE_X2,	FALSE,	FALSE},
    // DRAG without CLICK
    {(પૂર્ણાંક)KE_MOUSEMOVE,		MOUSE_RELEASE,	FALSE,	TRUE},
    // RELEASE without CLICK
    {(પૂર્ણાંક)KE_IGNORE,		MOUSE_RELEASE,	FALSE,	FALSE},
    {0,				0,		0,	0},
};

/*
 * Look up the given mouse code to return the relevant information in the other
 * arguments.  Return which button is down or was released.
 */
    પૂર્ણાંક
get_mouse_button(પૂર્ણાંક code, પૂર્ણાંક *is_click, પૂર્ણાંક *is_drag)
{
    પૂર્ણાંક	    i;

    માટે (i = 0; mouse_table[i].pseudo_code; i++)
	જો (code == mouse_table[i].pseudo_code)
	{
	    *is_click = mouse_table[i].is_click;
	    *is_drag = mouse_table[i].is_drag;
	    પાછા mouse_table[i].button;
	}
    પાછા 0;	    // Shouldn't get here
}

/*
 * Return the appropriate pseudo mouse event token (KE_LEFTMOUSE etc) based on
 * the given information about which mouse button is down, and whether the
 * mouse was clicked, dragged or released.
 */
    પૂર્ણાંક
get_pseudo_mouse_code(
    પૂર્ણાંક	    button,	// eg MOUSE_LEFT
    પૂર્ણાંક	    is_click,
    પૂર્ણાંક	    is_drag)
{
    પૂર્ણાંક	    i;

    માટે (i = 0; mouse_table[i].pseudo_code; i++)
	જો (button == mouse_table[i].button
	    && is_click == mouse_table[i].is_click
	    && is_drag == mouse_table[i].is_drag)
	{
#ifdef FEAT_GUI
	    // Trick: a non mappable left click and release has mouse_col -1
	    // or added MOUSE_COLOFF.  Used for 'mousefocus' in
	    // gui_mouse_moved()
	    જો (mouse_col < 0 || mouse_col > MOUSE_COLOFF)
	    {
		જો (mouse_col < 0)
		    mouse_col = 0;
		નહીંતર
		    mouse_col -= MOUSE_COLOFF;
		જો (mouse_table[i].pseudo_code == (પૂર્ણાંક)KE_LEFTMOUSE)
		    પાછા (પૂર્ણાંક)KE_LEFTMOUSE_NM;
		જો (mouse_table[i].pseudo_code == (પૂર્ણાંક)KE_LEFTRELEASE)
		    પાછા (પૂર્ણાંક)KE_LEFTRELEASE_NM;
	    }
#endif
	    પાછા mouse_table[i].pseudo_code;
	}
    પાછા (પૂર્ણાંક)KE_IGNORE;	    // not recognized, ignore it
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
સ્થિર પૂર્ણાંક has_mouse_termcode = 0;

    શૂન્ય
set_mouse_termcode(
    પૂર્ણાંક		n,	// KS_MOUSE, KS_NETTERM_MOUSE or KS_DEC_MOUSE
    char_u	*s)
{
    char_u	name[2];

    name[0] = n;
    name[1] = KE_FILLER;
    add_termcode(name, s, FALSE);
#ifdef FEAT_MOUSE_JSB
    જો (n == KS_JSBTERM_MOUSE)
	has_mouse_termcode |= HMT_JSBTERM;
    નહીંતર
#endif
#ifdef FEAT_MOUSE_NET
    જો (n == KS_NETTERM_MOUSE)
	has_mouse_termcode |= HMT_NETTERM;
    નહીંતર
#endif
#ifdef FEAT_MOUSE_DEC
    જો (n == KS_DEC_MOUSE)
	has_mouse_termcode |= HMT_DEC;
    નહીંતર
#endif
#ifdef FEAT_MOUSE_PTERM
    જો (n == KS_PTERM_MOUSE)
	has_mouse_termcode |= HMT_PTERM;
    નહીંતર
#endif
#ifdef FEAT_MOUSE_URXVT
    જો (n == KS_URXVT_MOUSE)
	has_mouse_termcode |= HMT_URXVT;
    નહીંતર
#endif
#ifdef FEAT_MOUSE_GPM
    જો (n == KS_GPM_MOUSE)
	has_mouse_termcode |= HMT_GPM;
    નહીંતર
#endif
    જો (n == KS_SGR_MOUSE)
	has_mouse_termcode |= HMT_SGR;
    નહીંતર જો (n == KS_SGR_MOUSE_RELEASE)
	has_mouse_termcode |= HMT_SGR_REL;
    નહીંતર
	has_mouse_termcode |= HMT_NORMAL;
}

#જો defined(UNIX) || defined(VMS)
    શૂન્ય
del_mouse_termcode(
    પૂર્ણાંક		n)	// KS_MOUSE, KS_NETTERM_MOUSE or KS_DEC_MOUSE
{
    char_u	name[2];

    name[0] = n;
    name[1] = KE_FILLER;
    del_termcode(name);
# ifdef FEAT_MOUSE_JSB
    જો (n == KS_JSBTERM_MOUSE)
	has_mouse_termcode &= ~HMT_JSBTERM;
    નહીંતર
# endif
# ifdef FEAT_MOUSE_NET
    જો (n == KS_NETTERM_MOUSE)
	has_mouse_termcode &= ~HMT_NETTERM;
    નહીંતર
# endif
# ifdef FEAT_MOUSE_DEC
    જો (n == KS_DEC_MOUSE)
	has_mouse_termcode &= ~HMT_DEC;
    નહીંતર
# endif
# ifdef FEAT_MOUSE_PTERM
    જો (n == KS_PTERM_MOUSE)
	has_mouse_termcode &= ~HMT_PTERM;
    નહીંતર
# endif
# ifdef FEAT_MOUSE_URXVT
    જો (n == KS_URXVT_MOUSE)
	has_mouse_termcode &= ~HMT_URXVT;
    નહીંતર
# endif
# ifdef FEAT_MOUSE_GPM
    જો (n == KS_GPM_MOUSE)
	has_mouse_termcode &= ~HMT_GPM;
    નહીંતર
# endif
    જો (n == KS_SGR_MOUSE)
	has_mouse_termcode &= ~HMT_SGR;
    નહીંતર જો (n == KS_SGR_MOUSE_RELEASE)
	has_mouse_termcode &= ~HMT_SGR_REL;
    નહીંતર
	has_mouse_termcode &= ~HMT_NORMAL;
}
#endif

/*
 * setmouse() - switch mouse on/off depending on current mode and 'mouse'
 */
    શૂન્ય
setmouse(શૂન્ય)
{
    પૂર્ણાંક	    checkfor;

#ifdef FEAT_MOUSESHAPE
    update_mouseshape(-1);
#endif

    // Should be outside proc, but may break MOUSESHAPE
#ifdef FEAT_GUI
    // In the GUI the mouse is always enabled.
    જો (gui.in_use)
	પાછા;
#endif
    // be quick when mouse is off
    જો (*p_mouse == NUL || has_mouse_termcode == 0)
	પાછા;

    // don't switch mouse on when not in raw mode (Ex mode)
    જો (cur_tmode != TMODE_RAW)
    {
	mch_setmouse(FALSE);
	પાછા;
    }

    જો (VIsual_active)
	checkfor = MOUSE_VISUAL;
    નહીંતર જો (State == MODE_HITRETURN || State == MODE_ASKMORE
						     || State == MODE_SETWSIZE)
	checkfor = MOUSE_RETURN;
    નહીંતર જો (State & MODE_ઉમેરો)
	checkfor = MOUSE_ઉમેરો;
    નહીંતર જો (State & MODE_CMDLINE)
	checkfor = MOUSE_COMMAND;
    નહીંતર જો (State == MODE_CONFIRM || State == MODE_EXTERNCMD)
	checkfor = ' '; // don't use mouse for ":confirm" or ":!cmd"
    નહીંતર
	checkfor = MOUSE_NORMAL;    // assume normal mode

    જો (mouse_has(checkfor))
	mch_setmouse(TRUE);
    નહીંતર
	mch_setmouse(FALSE);
}

/*
 * Return TRUE if
 * - "c" is in 'mouse', or
 * - 'a' is in 'mouse' and "c" is in MOUSE_A, or
 * - the current buffer is a help file and 'h' is in 'mouse' and we are in a
 *   normal editing mode (not at hit-return message).
 */
    પૂર્ણાંક
mouse_has(પૂર્ણાંક c)
{
    char_u	*p;

    માટે (p = p_mouse; *p; ++p)
	પસંદગી (*p)
	{
	    સ્થિતિ 'a': જો (vim_strchr((char_u *)MOUSE_A, c) != ખાલી)
			  પાછા TRUE;
		      તોડો;
	    સ્થિતિ MOUSE_HELP: જો (c != MOUSE_RETURN && curbuf->b_help)
				 પાછા TRUE;
			     તોડો;
	    મૂળભૂત: જો (c == *p) પાછા TRUE; તોડો;
	}
    પાછા FALSE;
}

/*
 * Return TRUE when 'mousemodel' is set to "popup" or "popup_setpos".
 */
    પૂર્ણાંક
mouse_model_popup(શૂન્ય)
{
    પાછા (p_mousem[0] == 'p');
}

/*
 * Call a click-region callback function.
 * "regions"/"region_count" describe the resolved click regions,
 * "winid" is stored as the "winid" key in the info dict (0 for tabline).
 * Returns TRUE if the function was called and handled the click.
 */
    સ્થિર પૂર્ણાંક
stl_click_handler_regions(
	stl_click_region_T  *regions,
	પૂર્ણાંક		    region_ગણતરી,
	પૂર્ણાંક		    winid,
	char_u		    *area_name,
	પૂર્ણાંક		    mrow,
	પૂર્ણાંક		    mcol,
	પૂર્ણાંક		    which_button,
	પૂર્ણાંક		    mods)
{
#ifdef FEAT_EVAL
    પૂર્ણાંક		n;
    પૂર્ણાંક		nclicks;
    char_u	button_str[2];
    char_u	mods_str[4];
    પૂર્ણાંક		mi = 0;
    dict_T	*info;
    typval_T	argvars[2];
    typval_T	rettv;
    funcexe_T	funcexe;
    પૂર્ણાંક		col = mcol;

    જો (regions == ખાલી || region_ગણતરી == 0)
	પાછા FALSE;

    // Find the click region at the given row and column.
    માટે (n = 0; n < region_ગણતરી; n++)
    {
	જો (regions[n].row == mrow
		&& col >= regions[n].col_start
		&& col < regions[n].col_end)
	    તોડો;
    }
    જો (n >= region_ગણતરી || regions[n].funcname == ખાલી)
	પાછા FALSE;

    // Build the info dictionary.
    info = dict_alloc();
    જો (info == ખાલી)
	પાછા FALSE;

    dict_add_સંખ્યા(info, "minwid", regions[n].minwid);

    // Determine number of clicks.
    // MOD_MASK_2CLICK=0x20, MOD_MASK_3CLICK=0x40, MOD_MASK_4CLICK=0x60
    nclicks = ((mods & MOD_MASK_MULTI_CLICK) >> 5) + 1;
    જો (nclicks > 3)
	nclicks = 3;
    dict_add_સંખ્યા(info, "nclicks", nclicks);

    // Button.
    જો (which_button == MOUSE_ડાબું)
	button_str[0] = 'l';
    નહીંતર જો (which_button == MOUSE_જમણું)
	button_str[0] = 'r';
    નહીંતર
	button_str[0] = 'm';
    button_str[1] = NUL;
    dict_add_string_len(info, "button", button_str, 1);

    // Modifiers.
    જો (mods & MOD_MASK_SHIFT)
	mods_str[mi++] = 's';
    જો (mods & MOD_MASK_CTRL)
	mods_str[mi++] = 'c';
    જો (mods & MOD_MASK_ALT)
	mods_str[mi++] = 'a';
    mods_str[mi] = NUL;
    dict_add_string_len(info, "mods", mods_str, mi);

    dict_add_સંખ્યા(info, "winid", winid);

    // "area": which option the clicked region belongs to.  Lets a shared
    // dispatcher distinguish 'statusline', 'tabline' and 'tabpanel' without
    // having to overload winid == 0.
    dict_add_string(info, "area", area_name);

    // Expose tab page number for 'tabpanel' regions.
    જો (regions[n].tabnr > 0)
	dict_add_સંખ્યા(info, "tabnr", regions[n].tabnr);

    // Call the function with the info dict as argument.
    argvars[0].v_type = VAR_DICT;
    argvars[0].vval.v_dict = info;
    ++info->dv_refcount;
    argvars[1].v_type = VAR_UNKNOWN;

    rettv.v_type = VAR_સંખ્યા;
    rettv.vval.v_સંખ્યા = 0;

    CLEAR_FIELD(funcexe);
    funcexe.fe_evaluate = TRUE;
    (શૂન્ય)call_func(regions[n].funcname, -1,
					    &rettv, 1, argvars, &funcexe);

    n = (પૂર્ણાંક)rettv.vval.v_સંખ્યા;
    clear_tv(&rettv);
    dict_unref(info);

    જો (n != 0)
    {
	// Make sure the tabline gets redrawn too when the callback asks for
	// a redraw (redraw_statuslines() only redraws the tabline when
	// redraw_tabline is set).  For tabpanel the whole screen needs to be
	// refreshed.
	જો (winid == 0)
	    redraw_tabline = TRUE;
# ifdef FEAT_TABPANEL
	જો (STRCMP(area_name, "tabpanel") == 0)
	    redraw_all_later(UPD_NOT_VALID);
# endif
	redraw_statuslines();
    }

    પાછા TRUE;
#નહીંતર
    (શૂન્ય)regions;
    (શૂન્ય)region_ગણતરી;
    (શૂન્ય)winid;
    (શૂન્ય)area_name;
    (શૂન્ય)mrow;
    (શૂન્ય)mcol;
    (શૂન્ય)which_button;
    (શૂન્ય)mods;
    પાછા FALSE;
#endif
}

/*
 * Call a statusline click handler function for window "wp".
 * Returns TRUE if the function was called and handled the click.
 */
    સ્થિર પૂર્ણાંક
stl_click_handler(win_T *wp, પૂર્ણાંક mrow, પૂર્ણાંક mcol, પૂર્ણાંક which_button, પૂર્ણાંક mods)
{
    જો (wp == ખાલી)
	પાછા FALSE;
    પાછા stl_click_handler_regions(wp->w_stl_click, wp->w_stl_click_ગણતરી,
				wp->w_id, (char_u *)"statusline",
				mrow, mcol, which_button, mods);
}

// dragwin is declared near the top of the file

/*
 * Reset the window being dragged.  To be called when switching tab page.
 */
    શૂન્ય
reset_dragwin(શૂન્ય)
{
    dragwin = ખાલી;
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
    પૂર્ણાંક
jump_to_mouse(
    પૂર્ણાંક		flags,
    પૂર્ણાંક		*inclusive,	// used for inclusive operator, can be NULL
    પૂર્ણાંક		which_button)	// MOUSE_LEFT, MOUSE_RIGHT, MOUSE_MIDDLE
{
    સ્થિર પૂર્ણાંક	on_status_લીટી = 0;	// #lines below bottom of window
    સ્થિર પૂર્ણાંક	on_sep_લીટી = 0;	// on separator right of window
#ifdef FEAT_MENU
    સ્થિર પૂર્ણાંક  in_winbar = FALSE;
#endif
#ifdef FEAT_PROP_POPUP
    સ્થિર પૂર્ણાંક   in_popup_win = FALSE;
    સ્થિર win_T *click_in_popup_win = ખાલી;
#endif
    સ્થિર પૂર્ણાંક	prev_row = -1;
    સ્થિર પૂર્ણાંક	prev_col = -1;
    સ્થિર પૂર્ણાંક	did_drag = FALSE;	// drag was noticed

    win_T	*wp, *old_curwin;
    pos_T	old_cursor;
    પૂર્ણાંક		ગણતરી;
    પૂર્ણાંક		first;
    પૂર્ણાંક		row = mouse_row;
    પૂર્ણાંક		col = mouse_col;
    colnr_T	col_from_screen = -1;
#ifdef FEAT_FOLDING
    પૂર્ણાંક		mouse_char = ' ';
#endif

    mouse_past_bottom = FALSE;
    mouse_past_eol = FALSE;

    જો (flags & MOUSE_RELEASED)
    {
	// On button release we may change window focus if positioned on a
	// status line and no dragging happened.
	જો (dragwin != ખાલી && !did_drag)
	    flags &= ~(MOUSE_FOCUS | MOUSE_DID_MOVE);
	dragwin = ખાલી;
	did_drag = FALSE;
#ifdef FEAT_PROP_POPUP
	જો (click_in_popup_win != ખાલી && popup_dragwin == ખાલી)
	    popup_close_for_mouse_click(click_in_popup_win);

	popup_dragwin = ખાલી;
	click_in_popup_win = ખાલી;
#endif
    }

    જો ((flags & MOUSE_DID_MOVE)
	    && prev_row == mouse_row
	    && prev_col == mouse_col)
    {
retnomove:
	// before moving the cursor for a left click which is NOT in a status
	// line, stop Visual mode
	જો (on_status_લીટી)
	    પાછા IN_STATUS_લીટી;
	જો (on_sep_લીટી)
	    પાછા IN_SEP_લીટી;
#ifdef FEAT_MENU
	જો (in_winbar)
	{
	    // A quick second click may arrive as a double-click, but we use it
	    // as a second click in the WinBar.
	    જો ((mod_mask & MOD_MASK_MULTI_CLICK) && !(flags & MOUSE_RELEASED))
	    {
		wp = mouse_find_win(&row, &col, FAIL_POPUP);
		જો (wp == ખાલી)
		    પાછા IN_UNKNOWN;
		winbar_click(wp, col);
	    }
	    પાછા IN_OTHER_WIN | MOUSE_WINBAR;
	}
#endif
	જો (flags & MOUSE_MAY_STOP_VIS)
	{
	    end_visual_mode_keep_button();
	    redraw_curbuf_later(UPD_INVERTED);	// delete the inversion
	}
#જો defined(FEAT_CLIPBOARD)
	// Continue a modeless selection in another window.
	જો (cmdwin_type != 0 && row < cmdwin_win->w_winrow)
	    પાછા IN_OTHER_WIN;
#endif
#ifdef FEAT_PROP_POPUP
	// Continue a modeless selection in a popup window or dragging it.
	જો (in_popup_win)
	{
	    click_in_popup_win = ખાલી;  // don't close it on release
	    જો (popup_dragwin != ખાલી)
	    {
		// dragging a popup window
		popup_drag(popup_dragwin);
		પાછા IN_UNKNOWN;
	    }
	    પાછા IN_OTHER_WIN;
	}
#endif
	પાછા IN_BUFFER;
    }

    prev_row = mouse_row;
    prev_col = mouse_col;

    જો (flags & MOUSE_SETPOS)
	જાઓ retnomove;				// ugly goto...

    old_curwin = curwin;
    old_cursor = curwin->w_cursor;

    જો (!(flags & MOUSE_FOCUS))
    {
	જો (row < 0 || col < 0) // check if it makes sense
	    પાછા IN_UNKNOWN;

	// find the window where the row is in and adjust "row" and "col" to be
	// relative to top-left of the window
	wp = mouse_find_win(&row, &col, FIND_POPUP);
	જો (wp == ખાલી)
	    પાછા IN_UNKNOWN;
	dragwin = ખાલી;

#ifdef FEAT_PROP_POPUP
	// Click in a popup window may start dragging or modeless selection,
	// but not much else.
	જો (WIN_IS_POPUP(wp))
	{
	    on_sep_લીટી = 0;
	    on_status_લીટી = 0;
	    in_popup_win = TRUE;
	    જો (which_button == MOUSE_ડાબું && popup_close_if_on_X(wp, row, col))
	    {
		પાછા IN_UNKNOWN;
	    }
	    નહીંતર જો (((wp->w_popup_flags & (POPF_DRAG | POPF_RESIZE))
					      && popup_on_border(wp, row, col))
				       || (wp->w_popup_flags & POPF_DRAGALL))
	    {
		popup_dragwin = wp;
		popup_start_drag(wp, row, col);
		પાછા IN_UNKNOWN;
	    }
	    // Only close on release, otherwise it's not possible to drag or do
	    // modeless selection.
	    નહીંતર જો (wp->w_popup_close == POPCLOSE_CLICK
		    && which_button == MOUSE_ડાબું)
	    {
		click_in_popup_win = wp;
	    }
	    નહીંતર જો (which_button == MOUSE_ડાબું)
		// If the click is in the scrollbar, may scroll up/down.
		popup_handle_scrollbar_click(wp, row, col);
# ifdef FEAT_CLIPBOARD
	    પાછા IN_OTHER_WIN;
# નહીંતર
	    પાછા IN_UNKNOWN;
# endif
	}
	in_popup_win = FALSE;
	popup_dragwin = ખાલી;
#endif
#ifdef FEAT_MENU
	જો (row == -1)
	{
	    // A click in the window toolbar does not enter another window or
	    // change Visual highlighting.
	    winbar_click(wp, col);
	    in_winbar = TRUE;
	    પાછા IN_OTHER_WIN | MOUSE_WINBAR;
	}
	in_winbar = FALSE;
#endif

	// winpos and height may change in win_enter()!
	જો (row >= wp->w_height)		// In (or below) status line
	{
	    on_status_લીટી = row - wp->w_height + 1;
	    dragwin = wp;
	}
	નહીંતર
	    on_status_લીટી = 0;
	જો (col >= wp->w_width)		// In separator line
	{
	    on_sep_લીટી = col - wp->w_width + 1;
	    dragwin = wp;
	}
	નહીંતર
	    on_sep_લીટી = 0;

	// The rightmost character of the status line might be a vertical
	// separator character if there is no connecting window to the right.
	જો (on_status_લીટી && on_sep_લીટી)
	{
	    જો (stl_connected(wp))
		on_sep_લીટી = 0;
	    નહીંતર
		on_status_લીટી = 0;
	}

	// Before jumping to another buffer, or moving the cursor for a left
	// click, stop Visual mode.
	જો (VIsual_active
		&& (wp->w_buffer != curwin->w_buffer
		    || (!on_status_લીટી && !on_sep_લીટી
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
	જો (cmdwin_type != 0 && wp != cmdwin_win)
	{
	    // A click outside the command-line window: Use modeless
	    // selection if possible.  Allow dragging the status lines.
	    on_sep_લીટી = 0;
#ifdef FEAT_CLIPBOARD
	    જો (on_status_લીટી)
		પાછા IN_STATUS_લીટી;
	    પાછા IN_OTHER_WIN;
#નહીંતર
	    row = 0;
	    col += wp->w_wincol;
	    wp = cmdwin_win;
#endif
	}
#જો defined(FEAT_PROP_POPUP) && defined(FEAT_TERMINAL)
	જો (popup_is_popup(curwin) && curbuf->b_term != ખાલી)
	    // terminal in popup window: don't jump to another window
	    પાછા IN_OTHER_WIN;
#endif
	// Only change window focus when not clicking on or dragging the
	// status line.  Do change focus when releasing the mouse button
	// (MOUSE_FOCUS was set above if we dragged first).
	જો (dragwin == ખાલી || (flags & MOUSE_RELEASED))
	    win_enter(wp, TRUE);		// can make wp invalid!

	જો (curwin != old_curwin)
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
	જો (on_status_લીટી)			// In (or below) status line
	{
	    // Don't use start_arrow() if we're in the same window
	    જો (curwin == old_curwin)
		પાછા IN_STATUS_લીટી;
	    નહીંતર
		પાછા IN_STATUS_લીટી | CURSOR_MOVED;
	}
	જો (on_sep_લીટી)			// In (or below) status line
	{
	    // Don't use start_arrow() if we're in the same window
	    જો (curwin == old_curwin)
		પાછા IN_SEP_લીટી;
	    નહીંતર
		પાછા IN_SEP_લીટી | CURSOR_MOVED;
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
    નહીંતર જો (on_status_લીટી && which_button == MOUSE_ડાબું)
    {
	જો (dragwin != ખાલી)
	{
	    // Drag the status line
	    ગણતરી = row - W_WINROW(dragwin) - dragwin->w_height + 1
							     - on_status_લીટી;
	    win_drag_status_લીટી(dragwin, ગણતરી);
	    did_drag |= ગણતરી;
	}
	પાછા IN_STATUS_લીટી;			// Cursor didn't move
    }
    નહીંતર જો (on_sep_લીટી && which_button == MOUSE_ડાબું)
    {
	જો (dragwin != ખાલી)
	{
	    // Drag the separator column
	    ગણતરી = col - dragwin->w_wincol - dragwin->w_width + 1
								- on_sep_લીટી;
	    win_drag_vsep_લીટી(dragwin, ગણતરી);
	    did_drag |= ગણતરી;
	}
	પાછા IN_SEP_લીટી;			// Cursor didn't move
    }
#ifdef FEAT_MENU
    નહીંતર જો (in_winbar)
    {
	// After a click on the window toolbar don't start Visual mode.
	પાછા IN_OTHER_WIN | MOUSE_WINBAR;
    }
#endif
    નહીંતર // keep_window_focus must be TRUE
    {
	// before moving the cursor for a left click, stop Visual mode
	જો (flags & MOUSE_MAY_STOP_VIS)
	{
	    end_visual_mode_keep_button();
	    redraw_curbuf_later(UPD_INVERTED);	// delete the inversion
	}

#જો defined(FEAT_CLIPBOARD)
	// Continue a modeless selection in another window.
	જો (cmdwin_type != 0 && row < cmdwin_win->w_winrow)
	    પાછા IN_OTHER_WIN;
#endif
#ifdef FEAT_PROP_POPUP
	જો (in_popup_win)
	{
	    જો (popup_dragwin != ખાલી)
	    {
		// dragging a popup window
		popup_drag(popup_dragwin);
		પાછા IN_UNKNOWN;
	    }
	    // continue a modeless selection in a popup window
	    click_in_popup_win = ખાલી;
	    પાછા IN_OTHER_WIN;
	}
#endif

	row -= W_WINROW(curwin);
	col -= curwin->w_wincol;

	// When clicking beyond the end of the window, scroll the screen.
	// Scroll by however many rows outside the window we are.
	જો (row < 0)
	{
	    ગણતરી = 0;
	    માટે (first = TRUE; curwin->w_topline > 1; )
	    {
#ifdef FEAT_DIFF
		જો (curwin->w_topfill < diff_check_fill(curwin, curwin->w_topline))
		    ++ગણતરી;
		નહીંતર
#endif
		    ગણતરી += plines(curwin->w_topline - 1);
		જો (!first && ગણતરી > -row)
		    તોડો;
		first = FALSE;
#ifdef FEAT_FOLDING
		(શૂન્ય)hasFolding(curwin->w_topline, &curwin->w_topline, ખાલી);
#endif
#ifdef FEAT_DIFF
		જો (curwin->w_topfill < diff_check_fill(curwin, curwin->w_topline))
		    ++curwin->w_topfill;
		નહીંતર
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
	નહીંતર જો (row >= curwin->w_height)
	{
	    ગણતરી = 0;
	    માટે (first = TRUE; curwin->w_topline < curbuf->b_ml.ml_લીટી_ગણતરી; )
	    {
#ifdef FEAT_DIFF
		જો (curwin->w_topfill > 0)
		    ++ગણતરી;
		નહીંતર
#endif
		    ગણતરી += plines(curwin->w_topline);
		જો (!first && ગણતરી > row - curwin->w_height + 1)
		    તોડો;
		first = FALSE;
#ifdef FEAT_FOLDING
		જો (hasFolding(curwin->w_topline, ખાલી, &curwin->w_topline)
			&& curwin->w_topline == curbuf->b_ml.ml_લીટી_ગણતરી)
		    તોડો;
#endif
#ifdef FEAT_DIFF
		જો (curwin->w_topfill > 0)
		    --curwin->w_topfill;
		નહીંતર
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
	નહીંતર જો (row == 0)
	{
	    // When dragging the mouse, while the text has been scrolled up as
	    // far as it goes, moving the mouse in the top line should scroll
	    // the text down (done later when recomputing w_topline).
	    જો (mouse_dragging > 0
		    && curwin->w_cursor.lnum
				       == curwin->w_buffer->b_ml.ml_લીટી_ગણતરી
		    && curwin->w_cursor.lnum == curwin->w_topline)
		curwin->w_valid &= ~(VALID_TOPLINE);
	}
    }

    જો (prev_row >= W_WINROW(curwin)
	&& prev_row < W_WINROW(curwin) + curwin->w_height
	&& prev_col >= curwin->w_wincol && prev_col < W_ENDCOL(curwin)
						       && Screen_લીટીઓ != ખાલી)
    {
	પૂર્ણાંક off = લીટી_Offset[prev_row] + prev_col;

	// Only use ScreenCols[] after the window was redrawn.  Mainly matters
	// for tests, a user would not click before redrawing.
	// Do not use when 'virtualedit' is active.
	જો (curwin->w_redr_type <= UPD_VALID_NO_UPDATE)
	    col_from_screen = ScreenCols[off];
#ifdef FEAT_FOLDING
	// Remember the character under the mouse, it might be a '-' or '+' in
	// the fold column.
	mouse_char = enc_utf8 && Screen_લીટીઓ_UC[off] != 0
				       ? Screen_લીટીઓ_UC[off] : Screen_લીટીઓ[off];
#endif
    }

#ifdef FEAT_FOLDING
    // Check for position outside of the fold column.
    જો (
# ifdef FEAT_RIGHTLEFT
	    curwin->w_p_rl ? col < curwin->w_width - curwin->w_p_fdc :
# endif
	    col >= curwin->w_p_fdc + (cmdwin_win != curwin ? 0 : 1)
       )
	mouse_char = ' ';
#endif

    // compute the position in the buffer line from the posn on the screen
    જો (mouse_comp_pos(curwin, &row, &col, &curwin->w_cursor.lnum, ખાલી))
	mouse_past_bottom = TRUE;

    // Start Visual mode before coladvance(), for when 'sel' != "old"
    જો ((flags & MOUSE_MAY_VIS) && !VIsual_active)
    {
	check_visual_highlight();
	VIsual = old_cursor;
	VIsual_active = TRUE;
	VIsual_reselect = TRUE;
	// if 'selectmode' contains "mouse", start Select mode
	may_start_select('o');
	setmouse();
	જો (p_smd && સંદેશ_silent == 0)
	    redraw_cmdline = TRUE;	// show visual mode later
    }

    જો (col_from_screen >= 0)
    {
	// Use the virtual column from ScreenCols[], it is accurate also after
	// concealed characters.
	col = col_from_screen;
    }

    curwin->w_curswant = col;
    curwin->w_set_curswant = false;	// May still have been TRUE
    જો (coladvance(col) == FAIL)	// Mouse click beyond end of line
    {
	જો (inclusive != ખાલી)
	    *inclusive = TRUE;
	mouse_past_eol = TRUE;
    }
    નહીંતર જો (inclusive != ખાલી)
	*inclusive = FALSE;

    ગણતરી = IN_BUFFER;
    જો (curwin != old_curwin || curwin->w_cursor.lnum != old_cursor.lnum
	    || curwin->w_cursor.col != old_cursor.col)
	ગણતરી |= CURSOR_MOVED;		// Cursor has moved

#ifdef FEAT_FOLDING
    જો (mouse_char == curwin->w_fill_chars.foldclosed)
	ગણતરી |= MOUSE_FOLD_OPEN;
    નહીંતર જો (mouse_char != ' ')
	ગણતરી |= MOUSE_FOLD_CLOSE;
#endif

    પાછા ગણતરી;
}

/*
 * Make a horizontal scroll to "leftcol".
 * Return TRUE if the cursor moved, FALSE otherwise.
 */
    પૂર્ણાંક
do_mousescroll_horiz(long_u leftcol)
{
    જો (curwin->w_p_wrap)
	પાછા FALSE;  // no horizontal scrolling when wrapping

    જો (curwin->w_leftcol == (colnr_T)leftcol)
	પાછા FALSE;  // already there

    // When the line of the cursor is too short, move the cursor to the
    // longest visible line.
    જો (
#ifdef FEAT_GUI
	    (!gui.in_use || vim_strchr(p_go, GO_HORSCROLL) == ખાલી) &&
#endif
		    !virtual_active()
	    && (long)leftcol > scroll_લીટી_len(curwin->w_cursor.lnum))
    {
	curwin->w_cursor.lnum = ui_find_longest_lnum();
	curwin->w_cursor.col = 0;
    }

    પાછા set_leftcol((colnr_T)leftcol);
}

/*
 * Normal and Visual modes implementation for scrolling in direction
 * "cap->arg", which is one of the MSCR_ values.
 */
    શૂન્ય
nv_mousescroll(cmdarg_T *cap)
{
    win_T   *old_curwin = curwin;

#ifdef FEAT_TABPANEL
    જો (mouse_row >= 0 && mouse_col >= 0
	    && (cap->arg == MSCR_UP || cap->arg == MSCR_DOWN)
	    && mouse_on_tabpanel())
    {
	(શૂન્ય)tabpanel_scroll(cap->arg == MSCR_UP ? 1 : -1,
		mouse_vert_step > 0 ? mouse_vert_step : 3);
	પાછા;
    }
#endif

    જો (mouse_row >= 0 && mouse_col >= 0)
    {
	// Find the window at the mouse pointer coordinates.
	// NOTE: Must restore "curwin" to "old_curwin" before returning!
	પૂર્ણાંક row = mouse_row;
	પૂર્ણાંક col = mouse_col;
	curwin = mouse_find_win(&row, &col, FIND_POPUP);
	જો (curwin == ખાલી)
	{
	    curwin = old_curwin;
	    પાછા;
	}

#ifdef FEAT_PROP_POPUP
	જો (WIN_IS_POPUP(curwin) && !curwin->w_has_scrollbar)
	{
	    // cannot scroll this popup window
	    curwin = old_curwin;
	    પાછા;
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
    શૂન્ય
nv_mouse(cmdarg_T *cap)
{
    (શૂન્ય)do_mouse(cap->oap, cap->cmdchar, BACKWARD, cap->count1, 0);
}

સ્થિર પૂર્ણાંક	held_button = MOUSE_RELEASE;

    શૂન્ય
reset_held_button(શૂન્ય)
{
    held_button = MOUSE_RELEASE;
}

/*
 * Check if typebuf 'tp' contains a terminal mouse code and returns the
 * modifiers found in typebuf in 'modifiers'.
 */
    પૂર્ણાંક
check_termcode_mouse(
    char_u	*tp,
    પૂર્ણાંક		*slen,
    char_u	*ચાવી_name,
    char_u	*modifiers_start,
    પૂર્ણાંક		idx,
    પૂર્ણાંક		*modifiers)
{
    પૂર્ણાંક		j;
    char_u	*p;
#જો !defined(UNIX) || defined(FEAT_MOUSE_XTERM) || defined(FEAT_GUI) \
    || defined(FEAT_MOUSE_GPM) || defined(FEAT_SYSMOUSE)
    char_u	bytes[6];
    પૂર્ણાંક		અંક_bytes;
#endif
    પૂર્ણાંક		mouse_code = 0;	    // init for GCC
    પૂર્ણાંક		is_click, is_drag;
    પૂર્ણાંક		is_release, release_is_ambiguous;
    પૂર્ણાંક		wheel_code = 0;
    પૂર્ણાંક		current_button;
    સ્થિર પૂર્ણાંક	orig_અંક_clicks = 1;
    સ્થિર પૂર્ણાંક	orig_mouse_code = 0x0;
#ifdef CHECK_DOUBLE_CLICK
    સ્થિર પૂર્ણાંક	orig_mouse_col = 0;
    સ્થિર પૂર્ણાંક	orig_mouse_row = 0;
    સ્થિર સંરચના timeval  orig_mouse_time = {0, 0};
    // time of previous mouse click
    સંરચના timeval  mouse_time;		// time of current mouse click
    long	timediff;		// elapsed time in msec
#endif

    is_click = is_drag = is_release = release_is_ambiguous = FALSE;

#જો !defined(UNIX) || defined(FEAT_MOUSE_XTERM) || defined(FEAT_GUI) \
    || defined(FEAT_MOUSE_GPM) || defined(FEAT_SYSMOUSE)
    જો (ચાવી_name[0] == KS_MOUSE
# ifdef FEAT_MOUSE_GPM
	    || ચાવી_name[0] == KS_GPM_MOUSE
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
	માટે (;;)
	{
	    // For the GUI and for MS-Windows two bytes each are used for row
	    // and column.  Allows for more than 223 columns.
# જો defined(FEAT_GUI) || defined(MSWIN)
	    જો (TRUE
#  જો defined(FEAT_GUI) && !defined(MSWIN)
		&& gui.in_use
#  endif
		)
	    {
		અંક_bytes = get_bytes_from_buf(tp + *slen, bytes, 5);
		જો (અંક_bytes == -1)	// not enough coordinates
		    પાછા -1;
		mouse_code = bytes[0];
		mouse_col = 128 * (bytes[1] - ' ' - 1)
		    + bytes[2] - ' ' - 1;
		mouse_row = 128 * (bytes[3] - ' ' - 1)
		    + bytes[4] - ' ' - 1;
	    }
	    નહીંતર
# endif
	    {
		અંક_bytes = get_bytes_from_buf(tp + *slen, bytes, 3);
		જો (અંક_bytes == -1)	// not enough coordinates
		    પાછા -1;
		mouse_code = bytes[0];
		mouse_col = bytes[1] - ' ' - 1;
		mouse_row = bytes[2] - ' ' - 1;
	    }
	    *slen += અંક_bytes;

	    // If the following bytes is also a mouse code and it has the same
	    // code, dump this one and get the next.  This makes dragging a
	    // whole lot faster.
# ifdef FEAT_GUI
	    જો (gui.in_use)
		j = 3;
	    નહીંતર
# endif
		j = get_termcode_len(idx);
	    જો (STRNCMP(tp, tp + *slen, (size_t)j) == 0
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
	    નહીંતર
		તોડો;
	}
    }

    જો (ચાવી_name[0] == KS_URXVT_MOUSE
	    || ચાવી_name[0] == KS_SGR_MOUSE
	    || ચાવી_name[0] == KS_SGR_MOUSE_RELEASE)
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
	જો (p == ખાલી)
	    પાછા -1;

	mouse_code = getdigits(&p);
	જો (*p++ != ';')
	    પાછા -1;

	// when mouse reporting is SGR, add 32 to mouse code
	જો (ચાવી_name[0] == KS_SGR_MOUSE
		|| ચાવી_name[0] == KS_SGR_MOUSE_RELEASE)
	    mouse_code += 32;

	mouse_col = getdigits(&p) - 1;
	જો (*p++ != ';')
	    પાછા -1;

	mouse_row = getdigits(&p) - 1;

	// The modifiers were the mouse coordinates, not the modifier keys
	// (alt/shift/ctrl/meta) state.
	*modifiers = 0;
    }

    જો (ચાવી_name[0] == KS_SGR_MOUSE
	    || ચાવી_name[0] == KS_SGR_MOUSE_RELEASE)
    {
	જો (ચાવી_name[0] == KS_SGR_MOUSE_RELEASE)
	{
	    is_release = TRUE;
	    // This is used below to set held_button.
	    mouse_code |= MOUSE_RELEASE;
	}
    }
    નહીંતર
    {
	release_is_ambiguous = TRUE;
	જો ((mouse_code & MOUSE_RELEASE) == MOUSE_RELEASE)
	    is_release = TRUE;
    }

    જો (ચાવી_name[0] == KS_MOUSE
# ifdef FEAT_MOUSE_GPM
	    || ચાવી_name[0] == KS_GPM_MOUSE
# endif
# ifdef FEAT_MOUSE_URXVT
	    || ચાવી_name[0] == KS_URXVT_MOUSE
# endif
	    || ચાવી_name[0] == KS_SGR_MOUSE
	    || ચાવી_name[0] == KS_SGR_MOUSE_RELEASE)
    {
# જો !defined(MSWIN)
	/*
	 * Handle old style mouse events.
	 * Recognize the xterm mouse wheel, but not in the GUI, the
	 * Linux console with GPM and the MS-DOS or Win32 console
	 * (multi-clicks use >= 0x60).
	 */
	જો (mouse_code >= MOUSEWHEEL_નીચલું && mouse_code < MOUSESIDEBUTTONS_નીચલું
#  ifdef FEAT_GUI
		&& !gui.in_use
#  endif
#  ifdef FEAT_MOUSE_GPM
		&& ચાવી_name[0] != KS_GPM_MOUSE
#  endif
	   )
	{
#  જો defined(UNIX)
	    જો (use_xterm_mouse() > 1 && mouse_code >= 0x80)
		// mouse-move event, using MOUSE_DRAG works
		mouse_code = MOUSE_DRAG;
	    નહીંતર
#  endif
		// Keep the mouse_code before it's changed, so that we
		// remember that it was a mouse wheel click.
		wheel_code = mouse_code;
	}
#  ifdef FEAT_MOUSE_XTERM
	નહીંતર જો (held_button == MOUSE_RELEASE
#   ifdef FEAT_GUI
		&& !gui.in_use
#   endif
		&& (mouse_code == 0x23 || mouse_code == 0x24
		    || mouse_code == 0x40 || mouse_code == 0x41))
	{
	    // Apparently 0x23 and 0x24 are used by rxvt scroll wheel.
	    // And 0x40 and 0x41 are used by some xterm emulator.
	    wheel_code = mouse_code - (mouse_code >= 0x40 ? 0x40 : 0x23)
							      + MOUSEWHEEL_નીચલું;
	}
#  endif

#  જો defined(UNIX)
	નહીંતર જો (use_xterm_mouse() > 1)
	{
	    જો (mouse_code & MOUSE_DRAG_XTERM)
		mouse_code |= MOUSE_DRAG;
	}
#  endif
#  ifdef FEAT_XCLIPBOARD
	નહીંતર જો (!(mouse_code & MOUSE_DRAG & ~MOUSE_CLICK_MASK))
	{
	    જો (is_release)
		stop_xterm_trace();
	    નહીંતર
		start_xterm_trace(mouse_code);
	}
#  endif
# endif
    }
#endif // !UNIX || FEAT_MOUSE_XTERM
#ifdef FEAT_MOUSE_NET
    જો (ચાવી_name[0] == KS_NETTERM_MOUSE)
    {
	પૂર્ણાંક mc, mr;

	// expect a rather limited sequence like: balancing {
	// \033}6,45\r
	// '6' is the row, 45 is the column
	p = tp + *slen;
	mr = getdigits(&p);
	જો (*p++ != ',')
	    પાછા -1;
	mc = getdigits(&p);
	જો (*p++ != '\r')
	    પાછા -1;

	mouse_col = mc - 1;
	mouse_row = mr - 1;
	mouse_code = MOUSE_ડાબું;
	*slen += (પૂર્ણાંક)(p - (tp + *slen));
    }
#endif	// FEAT_MOUSE_NET
#ifdef FEAT_MOUSE_JSB
    જો (ચાવી_name[0] == KS_JSBTERM_MOUSE)
    {
	પૂર્ણાંક mult, val, iter, button, status;

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
	પસંદગી (*p++)
	{
	    સ્થિતિ 'L': button = 1; તોડો;
	    સ્થિતિ '-': તોડો;
	    સ્થિતિ 'x': તોડો; // ignore sequence
	    મૂળભૂત:  પાછા -1; // Unknown Result
	}
	પસંદગી (*p++)
	{
	    સ્થિતિ 'M': button |= 2; તોડો;
	    સ્થિતિ '-': તોડો;
	    સ્થિતિ 'x': તોડો; // ignore sequence
	    મૂળભૂત:  પાછા -1; // Unknown Result
	}
	પસંદગી (*p++)
	{
	    સ્થિતિ 'R': button |= 4; તોડો;
	    સ્થિતિ '-': તોડો;
	    સ્થિતિ 'x': તોડો; // ignore sequence
	    મૂળભૂત:  પાછા -1; // Unknown Result
	}
	status = *p++;
	માટે (val = 0, mult = 100, iter = 0; iter < 3; iter++,
		mult /= 10, p++)
	    જો (*p >= '0' && *p <= '9')
		val += (*p - '0') * mult;
	    નહીંતર
		પાછા -1;
	mouse_col = val;
	માટે (val = 0, mult = 100, iter = 0; iter < 3; iter++,
		mult /= 10, p++)
	    જો (*p >= '0' && *p <= '9')
		val += (*p - '0') * mult;
	    નહીંતર
		પાછા -1;
	mouse_row = val;
	પસંદગી (*p++)
	{
	    સ્થિતિ 's': button |= 8; તોડો;  // SHIFT key Pressed
	    સ્થિતિ '-': તોડો;  // Not Pressed
	    સ્થિતિ 'x': તોડો;  // Not Reporting
	    મૂળભૂત:  પાછા -1; // Unknown Result
	}
	પસંદગી (*p++)
	{
	    સ્થિતિ 'c': button |= 16; તોડો;  // CTRL key Pressed
	    સ્થિતિ '-': તોડો;  // Not Pressed
	    સ્થિતિ 'x': તોડો;  // Not Reporting
	    મૂળભૂત:  પાછા -1; // Unknown Result
	}
	જો (*p++ != '\033')
	    પાછા -1;
	જો (*p++ != '\\')
	    પાછા -1;
	પસંદગી (status)
	{
	    સ્થિતિ 'D': // Double Click
	    સ્થિતિ 'S': // Single Click
		જો (button & 1) mouse_code |= MOUSE_ડાબું;
		જો (button & 2) mouse_code |= MOUSE_મધ્ય;
		જો (button & 4) mouse_code |= MOUSE_જમણું;
		જો (button & 8) mouse_code |= MOUSE_SHIFT;
		જો (button & 16) mouse_code |= MOUSE_CTRL;
		તોડો;
	    સ્થિતિ 'm': // Mouse move
		જો (button & 1) mouse_code |= MOUSE_ડાબું;
		જો (button & 2) mouse_code |= MOUSE_મધ્ય;
		જો (button & 4) mouse_code |= MOUSE_જમણું;
		જો (button & 8) mouse_code |= MOUSE_SHIFT;
		જો (button & 16) mouse_code |= MOUSE_CTRL;
		જો ((button & 7) != 0)
		{
		    held_button = mouse_code;
		    mouse_code |= MOUSE_DRAG;
		}
		is_drag = TRUE;
		showmode();
		તોડો;
	    સ્થિતિ 'd': // Button Down
		જો (button & 1) mouse_code |= MOUSE_ડાબું;
		જો (button & 2) mouse_code |= MOUSE_મધ્ય;
		જો (button & 4) mouse_code |= MOUSE_જમણું;
		જો (button & 8) mouse_code |= MOUSE_SHIFT;
		જો (button & 16) mouse_code |= MOUSE_CTRL;
		તોડો;
	    સ્થિતિ 'u': // Button Up
		is_release = TRUE;
		જો (button & 1)
		    mouse_code |= MOUSE_ડાબું;
		જો (button & 2)
		    mouse_code |= MOUSE_મધ્ય;
		જો (button & 4)
		    mouse_code |= MOUSE_જમણું;
		જો (button & 8)
		    mouse_code |= MOUSE_SHIFT;
		જો (button & 16)
		    mouse_code |= MOUSE_CTRL;
		તોડો;
	    મૂળભૂત: પાછા -1; // Unknown Result
	}

	*slen += (p - (tp + *slen));
    }
#endif // FEAT_MOUSE_JSB
#ifdef FEAT_MOUSE_DEC
    જો (ચાવી_name[0] == KS_DEC_MOUSE)
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
	પૂર્ણાંક Pe, Pb, Pr, Pc;

	p = tp + *slen;

	// get event status
	Pe = getdigits(&p);
	જો (*p++ != ';')
	    પાછા -1;

	// get button status
	Pb = getdigits(&p);
	જો (*p++ != ';')
	    પાછા -1;

	// get row status
	Pr = getdigits(&p);
	જો (*p++ != ';')
	    પાછા -1;

	// get column status
	Pc = getdigits(&p);

	// the page parameter is optional
	જો (*p == ';')
	{
	    p++;
	    (શૂન્ય)getdigits(&p);
	}
	જો (*p++ != '&')
	    પાછા -1;
	જો (*p++ != 'w')
	    પાછા -1;

	mouse_code = 0;
	પસંદગી (Pe)
	{
	    સ્થિતિ  0: પાછા -1; // position request while unavailable
	    સ્થિતિ  1: // a response to a locator position request includes
		     //	the status of all buttons
		     Pb &= 7;   // mask off and ignore fourth button
		     જો (Pb & 4)
			 mouse_code  = MOUSE_ડાબું;
		     જો (Pb & 2)
			 mouse_code  = MOUSE_મધ્ય;
		     જો (Pb & 1)
			 mouse_code  = MOUSE_જમણું;
		     જો (Pb)
		     {
			 held_button = mouse_code;
			 mouse_code |= MOUSE_DRAG;
			 WantQueryMouse = TRUE;
		     }
		     is_drag = TRUE;
		     showmode();
		     તોડો;
	    સ્થિતિ  2: mouse_code = MOUSE_ડાબું;
		     WantQueryMouse = TRUE;
		     તોડો;
	    સ્થિતિ  3: mouse_code = MOUSE_ડાબું;
		     is_release = TRUE;
		     તોડો;
	    સ્થિતિ  4: mouse_code = MOUSE_મધ્ય;
		     WantQueryMouse = TRUE;
		     તોડો;
	    સ્થિતિ  5: mouse_code = MOUSE_મધ્ય;
		     is_release = TRUE;
		     તોડો;
	    સ્થિતિ  6: mouse_code = MOUSE_જમણું;
		     WantQueryMouse = TRUE;
		     તોડો;
	    સ્થિતિ  7: mouse_code = MOUSE_જમણું;
		     is_release = TRUE;
		     તોડો;
	    સ્થિતિ  8: પાછા -1; // fourth button down
	    સ્થિતિ  9: પાછા -1; // fourth button up
	    સ્થિતિ 10: પાછા -1; // mouse outside of filter rectangle
	    મૂળભૂત: પાછા -1; // should never occur
	}

	mouse_col = Pc - 1;
	mouse_row = Pr - 1;

	*slen += (પૂર્ણાંક)(p - (tp + *slen));
    }
#endif // FEAT_MOUSE_DEC
#ifdef FEAT_MOUSE_PTERM
    જો (ચાવી_name[0] == KS_PTERM_MOUSE)
    {
	પૂર્ણાંક button, અંક_clicks, action;

	p = tp + *slen;

	action = getdigits(&p);
	જો (*p++ != ';')
	    પાછા -1;

	mouse_row = getdigits(&p);
	જો (*p++ != ';')
	    પાછા -1;
	mouse_col = getdigits(&p);
	જો (*p++ != ';')
	    પાછા -1;

	button = getdigits(&p);
	mouse_code = 0;

	પસંદગી (button)
	{
	    સ્થિતિ 4: mouse_code = MOUSE_ડાબું; તોડો;
	    સ્થિતિ 1: mouse_code = MOUSE_જમણું; તોડો;
	    સ્થિતિ 2: mouse_code = MOUSE_મધ્ય; તોડો;
	    મૂળભૂત: પાછા -1;
	}

	પસંદગી (action)
	{
	    સ્થિતિ 31: // Initial press
		જો (*p++ != ';')
		    પાછા -1;

		અંક_clicks = getdigits(&p); // Not used
		તોડો;

	    સ્થિતિ 32: // Release
		is_release = TRUE;
		તોડો;

	    સ્થિતિ 33: // Drag
		held_button = mouse_code;
		mouse_code |= MOUSE_DRAG;
		તોડો;

	    મૂળભૂત:
		પાછા -1;
	}

	જો (*p++ != 't')
	    પાછા -1;

	*slen += (p - (tp + *slen));
    }
#endif // FEAT_MOUSE_PTERM

    // Interpret the mouse code
    current_button = (mouse_code & MOUSE_CLICK_MASK);
    જો (is_release)
	current_button |= MOUSE_RELEASE;

    જો (current_button == MOUSE_RELEASE
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
	જો ((mouse_code & MOUSE_DRAG) == MOUSE_DRAG)
	    is_drag = TRUE;
	current_button = held_button;
    }
    નહીંતર
    {
      જો (wheel_code == 0)
      {
#ifdef CHECK_DOUBLE_CLICK
# ifdef FEAT_MOUSE_GPM
	/*
	 * Only for Unix, when GUI not active, we handle multi-clicks here, but
	 * not for GPM mouse events.
	 */
#  ifdef FEAT_GUI
	જો (ચાવી_name[0] != KS_GPM_MOUSE && !gui.in_use)
#  નહીંતર
	    જો (ચાવી_name[0] != KS_GPM_MOUSE)
#  endif
# નહીંતર
#  ifdef FEAT_GUI
		જો (!gui.in_use)
#  endif
# endif
		{
		    /*
		     * Compute the time elapsed since the previous mouse click.
		     */
		    gettimeofday(&mouse_time, ખાલી);
		    જો (orig_mouse_time.tv_sec == 0)
		    {
			/*
			 * Avoid computing the difference between mouse_time
			 * and orig_mouse_time for the first click, as the
			 * difference would be huge and would cause
			 * multiplication overflow.
			 */
			timediff = p_mouset;
		    }
		    નહીંતર
			timediff = time_diff_ms(&orig_mouse_time, &mouse_time);
		    orig_mouse_time = mouse_time;
		    જો (mouse_code == orig_mouse_code
			    && timediff < p_mouset
			    && orig_અંક_clicks != 4
			    && orig_mouse_col == mouse_col
			    && orig_mouse_row == mouse_row
			    && (is_mouse_topline(curwin)
				// Double click in tab pages line also works
				// when window contents changes.
				|| (mouse_row == 0 && firstwin->w_winrow > 0))
		       )
			++orig_અંક_clicks;
		    નહીંતર
			orig_અંક_clicks = 1;
		    orig_mouse_col = mouse_col;
		    orig_mouse_row = mouse_row;
		    set_mouse_topline(curwin);
		}
# જો defined(FEAT_GUI) || defined(FEAT_MOUSE_GPM)
		નહીંતર
		    orig_અંક_clicks = અંક_MOUSE_CLICKS(mouse_code);
# endif
#નહીંતર
	orig_અંક_clicks = અંક_MOUSE_CLICKS(mouse_code);
#endif
	is_click = TRUE;
      }
      orig_mouse_code = mouse_code;
    }
    જો (!is_drag)
	held_button = mouse_code & MOUSE_CLICK_MASK;

    /*
     * Translate the actual mouse event into a pseudo mouse event.
     * First work out what modifiers are to be used.
     */
    જો (orig_mouse_code & MOUSE_SHIFT)
	*modifiers |= MOD_MASK_SHIFT;
    જો (orig_mouse_code & MOUSE_CTRL)
	*modifiers |= MOD_MASK_CTRL;
    જો (orig_mouse_code & MOUSE_ALT)
	*modifiers |= MOD_MASK_ALT;
    જો (orig_અંક_clicks == 2)
	*modifiers |= MOD_MASK_2CLICK;
    નહીંતર જો (orig_અંક_clicks == 3)
	*modifiers |= MOD_MASK_3CLICK;
    નહીંતર જો (orig_અંક_clicks == 4)
	*modifiers |= MOD_MASK_4CLICK;

    // Work out our pseudo mouse event. Note that MOUSE_RELEASE gets added,
    // then it's not mouse up/down.
    ચાવી_name[0] = KS_EXTRA;
    જો (wheel_code != 0 && (!is_release || release_is_ambiguous))
    {
	જો (wheel_code & MOUSE_CTRL)
	    *modifiers |= MOD_MASK_CTRL;
	જો (wheel_code & MOUSE_ALT)
	    *modifiers |= MOD_MASK_ALT;

	જો (wheel_code & 1 && wheel_code & 2)
	    ચાવી_name[1] = (પૂર્ણાંક)KE_MOUSELEFT;
	નહીંતર જો (wheel_code & 2)
	    ચાવી_name[1] = (પૂર્ણાંક)KE_MOUSERIGHT;
	નહીંતર જો (wheel_code & 1)
	    ચાવી_name[1] = (પૂર્ણાંક)KE_MOUSEUP;
	નહીંતર
	    ચાવી_name[1] = (પૂર્ણાંક)KE_MOUSEDOWN;

	held_button = MOUSE_RELEASE;
    }
    નહીંતર
    {
#જો defined(UNIX)
	જો (use_xterm_mouse() && orig_mouse_code >= MOUSESIDEBUTTONS_નીચલું)
	    current_button = (current_button) ? MOUSE_X2 : MOUSE_X1;
#endif
	ચાવી_name[1] = get_pseudo_mouse_code(current_button, is_click, is_drag);
    }


    // Make sure the mouse position is valid.  Some terminals may return weird
    // values.
    જો (mouse_col >= Columns)
	mouse_col = Columns - 1;
    જો (mouse_row >= Rows)
	mouse_row = Rows - 1;

    પાછા 0;
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
    પૂર્ણાંક
mouse_comp_pos(
    win_T	*win,
    પૂર્ણાંક		*rowp,
    પૂર્ણાંક		*colp,
    linenr_T	*lnump,
    પૂર્ણાંક		*plines_cache)
{
    પૂર્ણાંક		col = *colp;
    પૂર્ણાંક		row = *rowp;
    linenr_T	lnum;
    પૂર્ણાંક		retval = FALSE;
    પૂર્ણાંક		off;
    પૂર્ણાંક		ગણતરી;

#ifdef FEAT_RIGHTLEFT
    જો (win->w_p_rl)
	col = win->w_width - 1 - col;
#endif

    lnum = win->w_topline;

    જ્યારે (row > 0)
    {
	પૂર્ણાંક cache_idx = lnum - win->w_topline;

	// Only "Rows" lines are cached, with folding we'll run out of entries
	// and use the slow way.
	જો (plines_cache != ખાલી && cache_idx < Rows
						&& plines_cache[cache_idx] > 0)
	    ગણતરી = plines_cache[cache_idx];
	નહીંતર
	{
#ifdef FEAT_DIFF
	    // Don't include filler lines in "count"
	    જો (win->w_p_diff
# ifdef FEAT_FOLDING
		    && !hasFoldingWin(win, lnum, ખાલી, ખાલી, TRUE, ખાલી)
# endif
		    )
	    {
		જો (lnum == win->w_topline)
		    row -= win->w_topfill;
		નહીંતર
		    row -= diff_check_fill(win, lnum);
		ગણતરી = plines_win_nofill(win, lnum, FALSE);
	    }
	    નહીંતર
#endif
		ગણતરી = plines_win(win, lnum, FALSE);
	    જો (plines_cache != ખાલી && cache_idx < Rows)
		plines_cache[cache_idx] = ગણતરી;
	}

	જો (win->w_skipcol > 0 && lnum == win->w_topline)
	{
	    પૂર્ણાંક width1 = win->w_width - win_col_off(win);

	    જો (width1 > 0)
	    {
		પૂર્ણાંક skip_લીટીઓ = 0;

		// Adjust for 'smoothscroll' clipping the top screen lines.
		// A similar formula is used in curs_columns().
		જો (win->w_skipcol > width1)
		    skip_લીટીઓ = (win->w_skipcol - width1)
					    / (width1 + win_col_off2(win)) + 1;
		નહીંતર જો (win->w_skipcol > 0)
		    skip_લીટીઓ = 1;

		ગણતરી -= skip_લીટીઓ;
	    }
	}

	જો (ગણતરી > row)
	    તોડો;	// Position is in this buffer line.
#ifdef FEAT_FOLDING
	(શૂન્ય)hasFoldingWin(win, lnum, ખાલી, &lnum, TRUE, ખાલી);
#endif
	જો (lnum == win->w_buffer->b_ml.ml_લીટી_ગણતરી)
	{
	    retval = TRUE;
	    તોડો;		// past end of file
	}
	row -= ગણતરી;
	++lnum;
    }

    જો (!retval)
    {
	// Compute the column without wrapping.
	off = win_col_off(win) - win_col_off2(win);
	જો (col < off)
	    col = off;
	col += row * (win->w_width - off);

	// Add skip column for the topline.
	જો (lnum == win->w_topline)
	    col += win->w_skipcol;
    }

    જો (!win->w_p_wrap)
	col += win->w_leftcol;

    // skip line number and fold column in front of the line
    col -= win_col_off(win);
    જો (col <= 0)
    {
#ifdef FEAT_NETBEANS_INTG
	// if mouse is clicked on the gutter, then inform the netbeans server
	જો (*colp < win_col_off(win))
	    netbeans_gutter_click(lnum);
#endif
	col = 0;
    }

    *colp = col;
    *rowp = row;
    *lnump = lnum;
    પાછા retval;
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
mouse_find_win(પૂર્ણાંક *rowp, પૂર્ણાંક *colp, mouse_find_T popup UNUSED)
{
    frame_T	*fp;
    win_T	*wp;

#ifdef FEAT_PROP_POPUP
    win_T	*pwp = ખાલી;

    જો (popup != IGNORE_POPUP)
    {
	popup_reset_handled(POPUP_HANDLED_1);
	જ્યારે ((wp = find_next_popup(TRUE, POPUP_HANDLED_1)) != ખાલી)
	{
	    જો (*rowp >= wp->w_winrow && *rowp < wp->w_winrow + popup_height(wp)
		    && *colp >= wp->w_wincol
				    && *colp < wp->w_wincol + popup_width(wp))
		pwp = wp;
	}
	જો (pwp != ખાલી)
	{
	    જો (popup == FAIL_POPUP)
		પાછા ખાલી;
	    *rowp -= pwp->w_winrow;
	    *colp -= pwp->w_wincol;
	    પાછા pwp;
	}
    }
#endif

    fp = topframe;

    જો (*colp < firstwin->w_wincol
	    || *colp >= firstwin->w_wincol + fp->fr_width
	    || *rowp < firstwin->w_winrow)
	પાછા ખાલી;

    *rowp -= firstwin->w_winrow;
    *colp -= firstwin->w_wincol;
    માટે (;;)
    {
	જો (fp->fr_layout == FR_LEAF)
	    તોડો;
	જો (fp->fr_layout == FR_ROW)
	{
	    માટે (fp = fp->fr_child; fp->fr_next != ખાલી; fp = fp->fr_next)
	    {
		જો (*colp < fp->fr_width)
		    તોડો;
		*colp -= fp->fr_width;
	    }
	}
	નહીંતર    // fr_layout == FR_COL
	{
	    માટે (fp = fp->fr_child; fp->fr_next != ખાલી; fp = fp->fr_next)
	    {
		જો (*rowp < fp->fr_height)
		    તોડો;
		*rowp -= fp->fr_height;
	    }
	}
    }
    // When using a timer that closes a window the window might not actually
    // exist.
    FOR_ALL_WINDOWS(wp)
	જો (wp == fp->fr_win)
	{
#ifdef FEAT_MENU
	    *rowp -= wp->w_winbar_height;
#endif
	    પાછા wp;
	}
    પાછા ખાલી;
}

#જો defined(NEED_VCOL2COL) || defined(FEAT_BEVAL) || defined(FEAT_PROP_POPUP) \
	|| defined(FEAT_EVAL)
/*
 * Convert a virtual (screen) column to a character column.
 * The first column is zero.
 */
    પૂર્ણાંક
vcol2col(win_T *wp, linenr_T lnum, પૂર્ણાંક vcol, colnr_T *coladdp)
{
    char_u	    *લીટી;
    chartabsize_T   cts;

    // try to advance to the specified column
    લીટી = ml_get_buf(wp->w_buffer, lnum, FALSE);
    init_chartabsize_arg(&cts, wp, lnum, 0, લીટી, લીટી);
    જ્યારે (cts.cts_vcol < vcol && *cts.cts_ptr != NUL)
    {
	પૂર્ણાંક size = win_lbr_chartabsize(&cts, ખાલી, ખાલી);
	જો (cts.cts_vcol + size > vcol)
	    તોડો;
	cts.cts_vcol += size;
	MB_PTR_ADV(cts.cts_ptr);
    }
    clear_chartabsize_arg(&cts);

    જો (coladdp != ખાલી)
	*coladdp = vcol - cts.cts_vcol;
    પાછા (પૂર્ણાંક)(cts.cts_ptr - લીટી);
}
#endif

#જો defined(FEAT_EVAL)
/*
 * "getmousepos()" function.
 */
    શૂન્ય
f_getmousepos(typval_T *argvars UNUSED, typval_T *rettv)
{
    dict_T	*d;
    win_T	*wp;
    પૂર્ણાંક		row = mouse_row;
    પૂર્ણાંક		col = mouse_col;
    varnumber_T winid = 0;
    varnumber_T winrow = 0;
    varnumber_T wincol = 0;
    linenr_T	lnum = 0;
    varnumber_T column = 0;
    colnr_T	coladd = 0;

    જો (rettv_dict_alloc(rettv) == FAIL)
	પાછા;
    d = rettv->vval.v_dict;

    dict_add_સંખ્યા(d, "screenrow", (varnumber_T)mouse_row + 1);
    dict_add_સંખ્યા(d, "screencol", (varnumber_T)mouse_col + 1);

    wp = mouse_find_win(&row, &col, FIND_POPUP);
    જો (wp != ખાલી)
    {
	પૂર્ણાંક	ટોચ_off = 0;
	પૂર્ણાંક	ડાબું_off = 0;
	પૂર્ણાંક	height = wp->w_height + wp->w_status_height;

# ifdef FEAT_PROP_POPUP
	જો (WIN_IS_POPUP(wp))
	{
	    ટોચ_off = popup_ટોચ_extra(wp);
	    ડાબું_off = popup_ડાબું_extra(wp);
	    height = popup_height(wp);
	}
# endif
	જો (row < height)
	{
	    winid = wp->w_id;
	    winrow = row + 1;
	    wincol = col + 1;
	    row -= ટોચ_off;
	    col -= ડાબું_off;
	    જો (row >= 0 && row < wp->w_height && col >= 0 && col < wp->w_width)
	    {
		(શૂન્ય)mouse_comp_pos(wp, &row, &col, &lnum, ખાલી);
		col = vcol2col(wp, lnum, col, &coladd);
		column = col + 1;
	    }
	}
    }
    dict_add_સંખ્યા(d, "winid", winid);
    dict_add_સંખ્યા(d, "winrow", winrow);
    dict_add_સંખ્યા(d, "wincol", wincol);
    dict_add_સંખ્યા(d, "line", (varnumber_T)lnum);
    dict_add_સંખ્યા(d, "column", column);
    dict_add_સંખ્યા(d, "coladd", coladd);
}
#endif
