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

#അന്തര്ഭാവ "vim.h"

/*
 * Horiziontal and vertical steps used when scrolling.
 * When negative scroll by a whole page.
 */
സ്ഥിതം long mouse_hor_step = 6;
സ്ഥിതം long mouse_vert_step = 3;
സ്ഥിതം win_T *dragwin = ശൂന്യ;	// window being dragged
സ്ഥിതം പൂർണ്ണസംഖ്യ stl_click_handler(win_T *wp, പൂർണ്ണസംഖ്യ mrow, പൂർണ്ണസംഖ്യ mcol, പൂർണ്ണസംഖ്യ which_button,
								    പൂർണ്ണസംഖ്യ mods);
സ്ഥിതം പൂർണ്ണസംഖ്യ stl_click_handler_regions(stl_click_region_T *regions,
				    പൂർണ്ണസംഖ്യ region_എണ്ണം, പൂർണ്ണസംഖ്യ winid,
				    char_u *area_name, പൂർണ്ണസംഖ്യ mrow, പൂർണ്ണസംഖ്യ mcol,
				    പൂർണ്ണസംഖ്യ which_button, പൂർണ്ണസംഖ്യ mods);

    ശൂന്യം
mouse_set_vert_scroll_step(long step)
{
    mouse_vert_step = step;
}

    ശൂന്യം
mouse_set_hor_scroll_step(long step)
{
    mouse_hor_step = step;
}

#ifdef CHECK_DOUBLE_CLICK
/*
 * Return the duration from t1 to t2 in milliseconds.
 */
    സ്ഥിതം long
time_diff_ms(ഘടന timeval *t1, ഘടന timeval *t2)
{
    // This handles wrapping of tv_usec correctly without any special case.
    // Example of 2 pairs (tv_sec, tv_usec) with a duration of 5 ms:
    //	   t1 = (1, 998000) t2 = (2, 3000) gives:
    //	   (2 - 1) * 1000 + (3000 - 998000) / 1000 -> 5 ms.
    തിരികെ (t2->tv_sec - t1->tv_sec) * 1000
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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
get_mouse_class(char_u *p)
{
    പൂർണ്ണസംഖ്യ		c;

    എങ്കിൽ (has_mbyte && MB_BYTE2LEN(p[0]) > 1)
	തിരികെ mb_get_class(p);

    c = *p;
    എങ്കിൽ (c == ' ' || c == '\t')
	തിരികെ 0;

    എങ്കിൽ (vim_iswordc(c))
	തിരികെ 2;

    // There are a few special cases where we want certain combinations of
    // characters to be considered as a single word.  These are things like
    // "->", "/ *", "*=", "+=", "&=", "<=", ">=", "!=" etc.  Otherwise, each
    // character is in its own class.
    എങ്കിൽ (c != NUL && vim_strchr((char_u *)"-+*/%<>&|^!=", c) != ശൂന്യ)
	തിരികെ 1;
    തിരികെ c;
}

/*
 * Move "pos" back to the start of the word it's in.
 */
    സ്ഥിതം ശൂന്യം
find_start_of_word(pos_T *pos)
{
    char_u	*വരി;
    പൂർണ്ണസംഖ്യ		cclass;
    പൂർണ്ണസംഖ്യ		col;

    വരി = ml_get(pos->lnum);
    cclass = get_mouse_class(വരി + pos->col);

    ആകുമ്പോൾ (pos->col > 0)
    {
	col = pos->col - 1;
	col -= (*mb_head_off)(വരി, വരി + col);
	എങ്കിൽ (get_mouse_class(വരി + col) != cclass)
	    നിർത്തുക;
	pos->col = col;
    }
}

/*
 * Move "pos" forward to the end of the word it's in.
 * When 'selection' is "exclusive", the position is just after the word.
 */
    സ്ഥിതം ശൂന്യം
find_end_of_word(pos_T *pos)
{
    char_u	*വരി;
    പൂർണ്ണസംഖ്യ		cclass;
    പൂർണ്ണസംഖ്യ		col;

    വരി = ml_get(pos->lnum);
    എങ്കിൽ (*p_sel == 'e' && pos->col > 0)
    {
	--pos->col;
	pos->col -= (*mb_head_off)(വരി, വരി + pos->col);
    }
    cclass = get_mouse_class(വരി + pos->col);
    ആകുമ്പോൾ (വരി[pos->col] != NUL)
    {
	col = pos->col + (*mb_ptr2len)(വരി + pos->col);
	എങ്കിൽ (get_mouse_class(വരി + col) != cclass)
	{
	    എങ്കിൽ (*p_sel == 'e')
		pos->col = col;
	    നിർത്തുക;
	}
	pos->col = col;
    }
}

#എങ്കിൽ defined(FEAT_GUI_MOTIF) || defined(FEAT_GUI_GTK) \
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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
get_fpos_of_mouse(pos_T *mpos)
{
    win_T	*wp;
    പൂർണ്ണസംഖ്യ		row = mouse_row;
    പൂർണ്ണസംഖ്യ		col = mouse_col;

    എങ്കിൽ (row < 0 || col < 0)		// check if it makes sense
	തിരികെ IN_UNKNOWN;

    // find the window where the row is in
    wp = mouse_find_win(&row, &col, FAIL_POPUP);
    എങ്കിൽ (wp == ശൂന്യ)
	തിരികെ IN_UNKNOWN;
    // winpos and height may change in win_enter()!
    എങ്കിൽ (row >= wp->w_height)	// In (or below) status line
	തിരികെ IN_STATUS_വരി;
    എങ്കിൽ (col >= wp->w_width)	// In vertical separator line
	തിരികെ IN_SEP_വരി;

    എങ്കിൽ (wp != curwin)
	തിരികെ IN_UNKNOWN;

    // compute the position in the buffer line from the posn on the screen
    എങ്കിൽ (mouse_comp_pos(curwin, &row, &col, &mpos->lnum, ശൂന്യ))
	തിരികെ IN_STATUS_വരി; // past bottom

    mpos->col = vcol2col(wp, mpos->lnum, col, &mpos->coladd);
    തിരികെ IN_BUFFER;
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
    പൂർണ്ണസംഖ്യ
do_mouse(
    oparg_T	*oap,		// operator argument, can be NULL
    പൂർണ്ണസംഖ്യ		c,		// K_LEFTMOUSE, etc
    പൂർണ്ണസംഖ്യ		dir,		// Direction to 'put' if necessary
    long	എണ്ണം,
    പൂർണ്ണസംഖ്യ		fixindent)	// PUT_FIXINDENT if fixing indent necessary
{
    സ്ഥിതം പൂർണ്ണസംഖ്യ	do_always = FALSE;	// ignore 'mouse' setting next time
    സ്ഥിതം പൂർണ്ണസംഖ്യ	got_click = FALSE;	// got a click some time back

    പൂർണ്ണസംഖ്യ		which_button;	// MOUSE_LEFT, _MIDDLE or _RIGHT
    പൂർണ്ണസംഖ്യ		is_click = FALSE; // If FALSE it's a drag or release event
    പൂർണ്ണസംഖ്യ		is_drag = FALSE;  // If TRUE it's a drag event
    പൂർണ്ണസംഖ്യ		jump_flags = 0;	// flags for jump_to_mouse()
    pos_T	start_visual;
    പൂർണ്ണസംഖ്യ		moved;		// Has cursor moved?
    പൂർണ്ണസംഖ്യ		in_status_വരി;	// mouse in status line
    സ്ഥിതം പൂർണ്ണസംഖ്യ	in_tab_വരി = FALSE; // mouse clicked in tab line
    സ്ഥിതം പൂർണ്ണസംഖ്യ	in_tabpanel = FALSE; // mouse clicked in tabpanel
#ifdef FEAT_TABPANEL
    സ്ഥിതം bool	in_tabpanel_scrollbar = false; // dragging tabpanel scrollbar
#endif
    പൂർണ്ണസംഖ്യ		in_sep_വരി;	// mouse in vertical separator line
    പൂർണ്ണസംഖ്യ		c1, c2;
#എങ്കിൽ defined(FEAT_FOLDING)
    pos_T	save_cursor;
#endif
    win_T	*old_curwin = curwin;
    സ്ഥിതം pos_T orig_cursor;
    colnr_T	leftcol, rightcol;
    pos_T	end_visual;
    പൂർണ്ണസംഖ്യ		diff;
    പൂർണ്ണസംഖ്യ		old_active = VIsual_active;
    പൂർണ്ണസംഖ്യ		old_mode = VIsual_mode;
    പൂർണ്ണസംഖ്യ		regname;

#എങ്കിൽ defined(FEAT_FOLDING)
    save_cursor = curwin->w_cursor;
#endif

    // When GUI is active, always recognize mouse events, otherwise:
    // - Ignore mouse event in normal mode if 'mouse' doesn't include 'n'.
    // - Ignore mouse event in visual mode if 'mouse' doesn't include 'v'.
    // - For command line and insert mode 'mouse' is checked before calling
    //	 do_mouse().
    എങ്കിൽ (do_always)
	do_always = FALSE;
    അല്ലെങ്കിൽ
#ifdef FEAT_GUI
	എങ്കിൽ (!gui.in_use)
#endif
	{
	    എങ്കിൽ (VIsual_active)
	    {
		എങ്കിൽ (!mouse_has(MOUSE_VISUAL))
		    തിരികെ FALSE;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (State == MODE_NORMAL && !mouse_has(MOUSE_NORMAL))
		തിരികെ FALSE;
	}

    വേണ്ടി (;;)
    {
	which_button = get_mouse_button(KEY2TERMCAP1(c), &is_click, &is_drag);
	എങ്കിൽ (is_drag)
	{
	    // If the next character is the same mouse event then use that
	    // one. Speeds up dragging the status line.
	    // Note: Since characters added to the stuff buffer in the code
	    // below need to come before the next character, do not do this
	    // when the current character was stuffed.
	    എങ്കിൽ (!താക്കോൽ_Stuffed && vpeekc() != NUL)
	    {
		പൂർണ്ണസംഖ്യ nc;
		പൂർണ്ണസംഖ്യ save_mouse_row = mouse_row;
		പൂർണ്ണസംഖ്യ save_mouse_col = mouse_col;

		// Need to get the character, peeking doesn't get the actual
		// one.
		nc = safe_vgetc();
		എങ്കിൽ (c == nc)
		    തുടരുക;
		vungetc(nc);
		mouse_row = save_mouse_row;
		mouse_col = save_mouse_col;
	    }
	}
	നിർത്തുക;
    }

    എങ്കിൽ (c == K_MOUSEMOVE)
    {
	// Mouse moved without a button pressed.
#ifdef FEAT_BEVAL_TERM
	ui_may_remove_balloon();
	എങ്കിൽ (p_bevalterm)
	{
	    profile_setlimit(p_bdlay, &bevalexpr_due);
	    bevalexpr_due_set = TRUE;
	}
#endif
#ifdef FEAT_PROP_POPUP
	popup_handle_mouse_moved();
#endif
	തിരികെ FALSE;
    }

#ifdef FEAT_MOUSESHAPE
    // May have stopped dragging the status or separator line.  The pointer is
    // most likely still on the status or separator line.
    എങ്കിൽ (!is_drag && drag_status_വരി)
    {
	drag_status_വരി = FALSE;
	update_mouseshape(SHAPE_IDX_STATUS);
    }
    എങ്കിൽ (!is_drag && drag_sep_വരി)
    {
	drag_sep_വരി = FALSE;
	update_mouseshape(SHAPE_IDX_VSEP);
    }
#endif

    // Ignore drag and release events if we didn't get a click.
    എങ്കിൽ (is_click)
    {
	got_click = TRUE;
	in_tab_വരി = FALSE;
	in_tabpanel = FALSE;
#ifdef FEAT_TABPANEL
	in_tabpanel_scrollbar = false;
#endif
    }
    അല്ലെങ്കിൽ
    {
	എങ്കിൽ (!got_click)			// didn't get click, ignore
	    തിരികെ FALSE;
	എങ്കിൽ (!is_drag)			// release, reset got_click
	{
	    got_click = FALSE;
	    എങ്കിൽ (in_tab_വരി || in_tabpanel
#ifdef FEAT_TABPANEL
		    || in_tabpanel_scrollbar
#endif
		    )
	    {
		in_tab_വരി = FALSE;
		in_tabpanel = FALSE;
#ifdef FEAT_TABPANEL
		in_tabpanel_scrollbar = false;
#endif
		തിരികെ FALSE;
	    }
	}
    }

#ifdef FEAT_TABPANEL
    // Continue a scrollbar drag before any tab-selection handling.
    എങ്കിൽ (is_drag && in_tabpanel_scrollbar)
    {
	tabpanel_drag_scrollbar(mouse_row);
	തിരികെ FALSE;
    }
#endif

    // CTRL right mouse button does CTRL-T
    എങ്കിൽ (is_click && (mod_mask & MOD_MASK_CTRL) && which_button == MOUSE_വലത്)
    {
	എങ്കിൽ (State & MODE_തിരുകുക)
	    stuffcharReadbuff(Ctrl_O);
	എങ്കിൽ (എണ്ണം > 1)
	    stuffnumReadbuff(എണ്ണം);
	stuffcharReadbuff(Ctrl_T);
	got_click = FALSE;		// ignore drag&release now
	തിരികെ FALSE;
    }

    // CTRL only works with left mouse button
    എങ്കിൽ ((mod_mask & MOD_MASK_CTRL) && which_button != MOUSE_ഇടത്)
	തിരികെ FALSE;

    // When a modifier is down, ignore drag and release events, as well as
    // multiple clicks and the middle mouse button.
    // Accept shift-leftmouse drags when 'mousemodel' is "popup.*".
    എങ്കിൽ ((mod_mask & (MOD_MASK_SHIFT | MOD_MASK_CTRL | MOD_MASK_ALT
							     | MOD_MASK_META))
	    && (!is_click
		|| (mod_mask & MOD_MASK_MULTI_CLICK)
		|| which_button == MOUSE_നടുവ്)
	    && !((mod_mask & (MOD_MASK_SHIFT|MOD_MASK_ALT))
		&& mouse_model_popup()
		&& which_button == MOUSE_ഇടത്)
	    && !((mod_mask & MOD_MASK_ALT)
		&& !mouse_model_popup()
		&& which_button == MOUSE_വലത്)
	    )
	തിരികെ FALSE;

    // If the button press was used as the movement command for an operator
    // (eg "d<MOUSE>"), or it is the middle button that is held down, ignore
    // drag/release events.
    എങ്കിൽ (!is_click && which_button == MOUSE_നടുവ്)
	തിരികെ FALSE;

    എങ്കിൽ (oap != ശൂന്യ)
	regname = oap->regname;
    അല്ലെങ്കിൽ
	regname = 0;

    // Middle mouse button does a 'put' of the selected text
    എങ്കിൽ (which_button == MOUSE_നടുവ്)
    {
	എങ്കിൽ (State == MODE_NORMAL)
	{
	    // If an operator was pending, we don't know what the user wanted
	    // to do. Go back to normal mode: Clear the operator and beep().
	    എങ്കിൽ (oap != ശൂന്യ && oap->op_type != OP_NOP)
	    {
		clearopbeep(oap);
		തിരികെ FALSE;
	    }

	    // If visual was active, yank the highlighted text and put it
	    // before the mouse pointer position.
	    // In Select mode replace the highlighted text with the clipboard.
	    എങ്കിൽ (VIsual_active)
	    {
		എങ്കിൽ (VIsual_select)
		{
		    stuffcharReadbuff(Ctrl_G);
		    stuffReadbuff((char_u *)"\"+p");
		}
		അല്ലെങ്കിൽ
		{
		    stuffcharReadbuff('y');
		    stuffcharReadbuff(K_MIDDLEMOUSE);
		}
		do_always = TRUE;	// ignore 'mouse' setting next time
		തിരികെ FALSE;
	    }
	    // The rest is below jump_to_mouse()
	}

	അല്ലെങ്കിൽ എങ്കിൽ ((State & MODE_തിരുകുക) == 0)
	    തിരികെ FALSE;

	// Middle click in insert mode doesn't move the mouse, just insert the
	// contents of a register.  '.' register is special, can't insert that
	// with do_put().
	// Also paste at the cursor if the current mode isn't in 'mouse' (only
	// happens for the GUI).
	എങ്കിൽ ((State & MODE_തിരുകുക) || !mouse_has(MOUSE_NORMAL))
	{
	    എങ്കിൽ (regname == '.')
		തിരുകുക_reg(regname, TRUE);
	    അല്ലെങ്കിൽ
	    {
#ifdef FEAT_CLIPBOARD
		എങ്കിൽ (clip_star.available && regname == 0)
		    regname = '*';
#endif
		എങ്കിൽ ((State & REPLACE_FLAG) && !yank_register_mline(regname))
		    തിരുകുക_reg(regname, TRUE);
		അല്ലെങ്കിൽ
		{
		    do_put(regname, ശൂന്യ, BACKWARD, 1L,
						      fixindent | PUT_CURSEND);

		    // Repeat it with CTRL-R CTRL-O r or CTRL-R CTRL-P r
		    AppendCharToRedobuff(Ctrl_R);
		    AppendCharToRedobuff(fixindent ? Ctrl_P : Ctrl_O);
		    AppendCharToRedobuff(regname == 0 ? '"' : regname);
		}
	    }
	    തിരികെ FALSE;
	}
    }

    // When dragging or button-up stay in the same window.
    എങ്കിൽ (!is_click)
	jump_flags |= MOUSE_FOCUS | MOUSE_DID_MOVE;

    start_visual.lnum = 0;

    ഘടന tabpage_label_info {
	bool is_panel;	    // label type. true: tabpanel, false: tab line
	bool just_in;	    // just in tabpage label area
	bool just_click;    // just click tabpage label area
	പൂർണ്ണസംഖ്യ nr;		    // tabpage number
    } tp_label = { false, false, false, 0 };

    // Check for clicking in the tab page panel.
#എങ്കിൽ defined(FEAT_TABPANEL)
    എങ്കിൽ (mouse_col < firstwin->w_wincol
		|| mouse_col >= firstwin->w_wincol + topframe->fr_width)
    {
	// A click on the scrollbar column starts a drag interaction and
	// preempts tab-selection.
	എങ്കിൽ (is_click && !is_drag && mouse_on_tabpanel_scrollbar())
	{
	    in_tabpanel_scrollbar = TRUE;
	    tabpanel_drag_scrollbar(mouse_row);
	    തിരികെ FALSE;
	}

	// Dispatch 'tabpanel' %[FuncName] click regions before falling through
	// to tab-page selection.  On drag events fall through to the normal
	// tab-drag handling.
	എങ്കിൽ (is_click && !is_drag
		&& stl_click_handler_regions(tabpanel_stl_click,
					    tabpanel_stl_click_എണ്ണം,
					    0, (char_u *)"tabpanel",
					    mouse_row, mouse_col,
					    which_button, mod_mask))
	    തിരികെ FALSE;

	tp_label.is_panel = true;
	tp_label.just_in = true;
	tp_label.nr = get_tabpagenr_on_tabpanel();

	// click in a tab selects that tab page
	എങ്കിൽ (is_click && cmdwin_type == 0)
	    tp_label.just_click = true;
    }
    അല്ലെങ്കിൽ
#endif
    // Check for clicking in the tab page line.
    എങ്കിൽ (TabPageIdxs != ശൂന്യ && mouse_row == 0 && firstwin->w_winrow > 0)
    {
	// Dispatch 'tabline' %[FuncName] click regions before falling through
	// to tab-page selection.  On drag events fall through to the normal
	// tab-drag handling.
	എങ്കിൽ (is_click && !is_drag
		&& stl_click_handler_regions(tabline_stl_click,
					    tabline_stl_click_എണ്ണം,
					    0, (char_u *)"tabline",
					    mouse_row, mouse_col,
					    which_button, mod_mask))
	    തിരികെ FALSE;

	tp_label.just_in = true;
	tp_label.nr = TabPageIdxs[mouse_col];

	// click in a tab selects that tab page
	എങ്കിൽ (is_click && cmdwin_type == 0
		&& mouse_col < firstwin->w_wincol + topframe->fr_width)
	    tp_label.just_click = true;
    }

    എങ്കിൽ (tp_label.just_in)
    {
	എങ്കിൽ (is_drag)
	{
	    എങ്കിൽ (in_tabpanel || in_tab_വരി)
	    {
		c1 = tp_label.nr;
		tabpage_move(c1 <= 0 ? 9999 : c1 < tabpage_index(curtab)
							    ? c1 - 1 : c1);
	    }
	    തിരികെ FALSE;
	}

	എങ്കിൽ (tp_label.just_click)
	{
	    എങ്കിൽ (tp_label.is_panel)
		in_tabpanel = TRUE;
	    അല്ലെങ്കിൽ
		in_tab_വരി = TRUE;
	    c1 = tp_label.nr;
	    എങ്കിൽ (c1 >= 0)
	    {
		എങ്കിൽ ((mod_mask & MOD_MASK_MULTI_CLICK) == MOD_MASK_2CLICK
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
		അല്ലെങ്കിൽ
		{
		    // Go to specified tab page, or next one if not clicking
		    // on a label.
		    goto_tabpage(c1);

		    // It's like clicking on the status line of a window.
		    എങ്കിൽ (curwin != old_curwin)
			end_visual_mode_keep_button();
		}
	    }
	    അല്ലെങ്കിൽ
	    {
		tabpage_T	*tp;

		// Close the current or specified tab page.
		എങ്കിൽ (c1 == -999)
		    tp = curtab;
		അല്ലെങ്കിൽ
		    tp = find_tabpage(-c1);
		എങ്കിൽ (tp == curtab)
		{
		    എങ്കിൽ (first_tabpage->tp_next != ശൂന്യ)
			tabpage_close(FALSE);
		}
		അല്ലെങ്കിൽ എങ്കിൽ (tp != ശൂന്യ)
		    tabpage_close_other(tp, FALSE);
	    }
	}
	തിരികെ TRUE;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (is_drag && (in_tabpanel || (in_tab_വരി && TabPageIdxs != ശൂന്യ)))
    {
#എങ്കിൽ defined(FEAT_TABPANEL)
	എങ്കിൽ (in_tabpanel)
	    c1 = get_tabpagenr_on_tabpanel();
	അല്ലെങ്കിൽ
#endif
	    c1 = TabPageIdxs[mouse_col];
	tabpage_move(c1 <= 0 ? 9999 : c1 - 1);
	തിരികെ FALSE;
    }

    // When 'mousemodel' is "popup" or "popup_setpos", translate mouse events:
    // right button up   -> pop-up menu
    // shift-left button -> right button
    // alt-left button   -> alt-right button
    എങ്കിൽ (mouse_model_popup())
    {
	എങ്കിൽ (which_button == MOUSE_വലത്
			    && !(mod_mask & (MOD_MASK_SHIFT | MOD_MASK_CTRL)))
	{
#ifdef USE_POPUP_SETPOS
# ifdef FEAT_GUI
	    എങ്കിൽ (gui.in_use)
	    {
#  എങ്കിൽ defined(FEAT_GUI_MOTIF) || defined(FEAT_GUI_GTK) \
			  || defined(FEAT_GUI_PHOTON)
		എങ്കിൽ (!is_click)
		    // Ignore right button release events, only shows the popup
		    // menu on the button down event.
		    തിരികെ FALSE;
#  endif
#  എങ്കിൽ defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_HAIKU)
		എങ്കിൽ (is_click || is_drag)
		    // Ignore right button down and drag mouse events.  Windows
		    // only shows the popup menu on the button up event.
		    തിരികെ FALSE;
#  endif
	    }
# endif
# എങ്കിൽ defined(FEAT_GUI) && defined(FEAT_TERM_POPUP_MENU)
	    അല്ലെങ്കിൽ
# endif
# എങ്കിൽ defined(FEAT_TERM_POPUP_MENU)
	    എങ്കിൽ (!is_click)
		// Ignore right button release events, only shows the popup
		// menu on the button down event.
		തിരികെ FALSE;
# endif

	    jump_flags = 0;
	    എങ്കിൽ (STRCMP(p_mousem, "popup_setpos") == 0)
	    {
		// First set the cursor position before showing the popup
		// menu.
		എങ്കിൽ (VIsual_active)
		{
		    pos_T    m_pos;

		    // set MOUSE_MAY_STOP_VIS if we are outside the
		    // selection or the current window (might have false
		    // negative here)
		    എങ്കിൽ (mouse_row < curwin->w_winrow
			 || mouse_row
				  > (curwin->w_winrow + curwin->w_height))
			jump_flags = MOUSE_MAY_STOP_VIS;
		    അല്ലെങ്കിൽ എങ്കിൽ (get_fpos_of_mouse(&m_pos) != IN_BUFFER)
			jump_flags = MOUSE_MAY_STOP_VIS;
		    അല്ലെങ്കിൽ
		    {
			എങ്കിൽ (VIsual_mode == 'V')
			{
			    എങ്കിൽ ((curwin->w_cursor.lnum <= VIsual.lnum
				    && (m_pos.lnum < curwin->w_cursor.lnum
					|| VIsual.lnum < m_pos.lnum))
				|| (VIsual.lnum < curwin->w_cursor.lnum
				    && (m_pos.lnum < VIsual.lnum
					|| curwin->w_cursor.lnum < m_pos.lnum)))
			    {
				jump_flags = MOUSE_MAY_STOP_VIS;
			    }
			}
			അല്ലെങ്കിൽ എങ്കിൽ ((LTOREQ_POS(curwin->w_cursor, VIsual)
				    && (LT_POS(m_pos, curwin->w_cursor)
					|| LT_POS(VIsual, m_pos)))
				|| (LT_POS(VIsual, curwin->w_cursor)
				    && (LT_POS(m_pos, VIsual)
				      || LT_POS(curwin->w_cursor, m_pos))))
			{
			    jump_flags = MOUSE_MAY_STOP_VIS;
			}
			അല്ലെങ്കിൽ എങ്കിൽ (VIsual_mode == Ctrl_V)
			{
			    getvcols(curwin, &curwin->w_cursor, &VIsual,
						     &leftcol, &rightcol, 0);
			    getvcol(curwin, &m_pos, ശൂന്യ, &m_pos.col, ശൂന്യ, 0);
			    എങ്കിൽ (m_pos.col < leftcol || m_pos.col > rightcol)
				jump_flags = MOUSE_MAY_STOP_VIS;
			}
		    }
		}
		അല്ലെങ്കിൽ
		    jump_flags = MOUSE_MAY_STOP_VIS;
	    }
	    എങ്കിൽ (jump_flags)
	    {
		jump_flags = jump_to_mouse(jump_flags, ശൂന്യ, which_button);
		update_curbuf(VIsual_active ? UPD_INVERTED : UPD_VALID);
		setcursor();
		out_flush();    // Update before showing popup menu
	    }
# ifdef FEAT_MENU
	    show_popupmenu();
	    got_click = FALSE;	// ignore release events
# endif
	    തിരികെ (jump_flags & CURSOR_MOVED) != 0;
#അല്ലെങ്കിൽ
	    തിരികെ FALSE;
#endif
	}
	എങ്കിൽ (which_button == MOUSE_ഇടത്
				&& (mod_mask & (MOD_MASK_SHIFT|MOD_MASK_ALT)))
	{
	    which_button = MOUSE_വലത്;
	    mod_mask &= ~MOD_MASK_SHIFT;
	}
    }

    എങ്കിൽ ((State & (MODE_NORMAL | MODE_തിരുകുക))
			    && !(mod_mask & (MOD_MASK_SHIFT | MOD_MASK_CTRL)))
    {
	എങ്കിൽ (which_button == MOUSE_ഇടത്)
	{
	    എങ്കിൽ (is_click)
	    {
		// stop Visual mode for a left click in a window, but not when
		// on a status line
		എങ്കിൽ (VIsual_active)
		    jump_flags |= MOUSE_MAY_STOP_VIS;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (mouse_has(MOUSE_VISUAL))
		jump_flags |= MOUSE_MAY_VIS;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (which_button == MOUSE_വലത്)
	{
	    എങ്കിൽ (is_click && VIsual_active)
	    {
		// Remember the start and end of visual before moving the
		// cursor.
		എങ്കിൽ (LT_POS(curwin->w_cursor, VIsual))
		{
		    start_visual = curwin->w_cursor;
		    end_visual = VIsual;
		}
		അല്ലെങ്കിൽ
		{
		    start_visual = VIsual;
		    end_visual = curwin->w_cursor;
		}
	    }
	    jump_flags |= MOUSE_FOCUS;
	    എങ്കിൽ (mouse_has(MOUSE_VISUAL))
		jump_flags |= MOUSE_MAY_VIS;
	}
    }

    // If an operator is pending, ignore all drags and releases until the
    // next mouse click.
    എങ്കിൽ (!is_drag && oap != ശൂന്യ && oap->op_type != OP_NOP)
    {
	got_click = FALSE;
	oap->motion_type = MCHAR;
    }

    // When releasing the button let jump_to_mouse() know.
    എങ്കിൽ (!is_click && !is_drag)
	jump_flags |= MOUSE_RELEASED;

    // JUMP!
    jump_flags = jump_to_mouse(jump_flags,
			oap == ശൂന്യ ? ശൂന്യ : &(oap->inclusive), which_button);

#ifdef FEAT_MENU
    // A click in the window toolbar has no side effects.
    എങ്കിൽ (jump_flags & MOUSE_WINBAR)
	തിരികെ FALSE;
#endif
    moved = (jump_flags & CURSOR_MOVED);
    in_status_വരി = (jump_flags & IN_STATUS_വരി);
    in_sep_വരി = (jump_flags & IN_SEP_വരി);

    // Check for statusline click handler early, before visual mode or
    // other button-specific handling can interfere.
    എങ്കിൽ (in_status_വരി && is_click && !is_drag
	    && stl_click_handler(dragwin, mouse_row, mouse_col,
						which_button, mod_mask))
    {
#ifdef FEAT_MOUSESHAPE
	എങ്കിൽ (!drag_status_വരി)
	{
	    drag_status_വരി = TRUE;
	    update_mouseshape(-1);
	}
#endif
	തിരികെ FALSE;
    }

#ifdef FEAT_NETBEANS_INTG
    എങ്കിൽ (isNetbeansBuffer(curbuf)
			    && !(jump_flags & (IN_STATUS_വരി | IN_SEP_വരി)))
    {
	പൂർണ്ണസംഖ്യ താക്കോൽ = KEY2TERMCAP1(c);

	എങ്കിൽ (താക്കോൽ == (പൂർണ്ണസംഖ്യ)KE_LEFTRELEASE || താക്കോൽ == (പൂർണ്ണസംഖ്യ)KE_MIDDLERELEASE
					       || താക്കോൽ == (പൂർണ്ണസംഖ്യ)KE_RIGHTRELEASE)
	    netbeans_button_release(which_button);
    }
#endif

    // When jumping to another window, clear a pending operator.  That's a bit
    // friendlier than beeping and not jumping to that window.
    എങ്കിൽ (curwin != old_curwin && oap != ശൂന്യ && oap->op_type != OP_NOP)
	clearop(oap);

#ifdef FEAT_FOLDING
    എങ്കിൽ (mod_mask == 0
	    && !is_drag
	    && (jump_flags & (MOUSE_FOLD_CLOSE | MOUSE_FOLD_OPEN))
	    && which_button == MOUSE_ഇടത്)
    {
	// open or close a fold at this line
	എങ്കിൽ (jump_flags & MOUSE_FOLD_OPEN)
	    openFold(curwin->w_cursor.lnum, 1L);
	അല്ലെങ്കിൽ
	    closeFold(curwin->w_cursor.lnum, 1L);
	// don't move the cursor if still in the same window
	എങ്കിൽ (curwin == old_curwin)
	    curwin->w_cursor = save_cursor;
    }
#endif

#എങ്കിൽ defined(FEAT_CLIPBOARD)
    എങ്കിൽ ((jump_flags & IN_OTHER_WIN) && !VIsual_active &&
	    (clip_star.available || clip_plus.available))
    {
	clip_modeless(which_button, is_click, is_drag);
	തിരികെ FALSE;
    }
#endif

    // Set global flag that we are extending the Visual area with mouse
    // dragging; temporarily minimize 'scrolloff'.
    എങ്കിൽ (VIsual_active && is_drag && get_scrolloff_മൂല്യം())
    {
	// In the very first line, allow scrolling one line
	എങ്കിൽ (mouse_row == 0)
	    mouse_dragging = 2;
	അല്ലെങ്കിൽ
	    mouse_dragging = 1;
    }

    // When dragging the mouse above the window, scroll down.
    എങ്കിൽ (is_drag && mouse_row < 0 && !in_status_വരി)
    {
	scroll_redraw(FALSE, 1L);
	mouse_row = 0;
    }

    എങ്കിൽ (start_visual.lnum)		// right click in visual mode
    {
       // When ALT is pressed make Visual mode blockwise.
       എങ്കിൽ (mod_mask & MOD_MASK_ALT)
	   VIsual_mode = Ctrl_V;

	// In Visual-block mode, divide the area in four, pick up the corner
	// that is in the quarter that the cursor is in.
	എങ്കിൽ (VIsual_mode == Ctrl_V)
	{
	    getvcols(curwin, &start_visual, &end_visual,
						    &leftcol, &rightcol, 0);
	    എങ്കിൽ (curwin->w_curswant > (leftcol + rightcol) / 2)
		end_visual.col = leftcol;
	    അല്ലെങ്കിൽ
		end_visual.col = rightcol;
	    എങ്കിൽ (curwin->w_cursor.lnum >=
				    (start_visual.lnum + end_visual.lnum) / 2)
		end_visual.lnum = start_visual.lnum;

	    // move VIsual to the right column
	    start_visual = curwin->w_cursor;	    // save the cursor pos
	    curwin->w_cursor = end_visual;
	    coladvance(end_visual.col);
	    VIsual = curwin->w_cursor;
	    curwin->w_cursor = start_visual;	    // restore the cursor
	}
	അല്ലെങ്കിൽ
	{
	    // If the click is before the start of visual, change the start.
	    // If the click is after the end of visual, change the end.  If
	    // the click is inside the visual, change the closest side.
	    എങ്കിൽ (LT_POS(curwin->w_cursor, start_visual))
		VIsual = end_visual;
	    അല്ലെങ്കിൽ എങ്കിൽ (LT_POS(end_visual, curwin->w_cursor))
		VIsual = start_visual;
	    അല്ലെങ്കിൽ
	    {
		// In the same line, compare column number
		എങ്കിൽ (end_visual.lnum == start_visual.lnum)
		{
		    എങ്കിൽ (curwin->w_cursor.col - start_visual.col >
				    end_visual.col - curwin->w_cursor.col)
			VIsual = start_visual;
		    അല്ലെങ്കിൽ
			VIsual = end_visual;
		}

		// In different lines, compare line number
		അല്ലെങ്കിൽ
		{
		    diff = (curwin->w_cursor.lnum - start_visual.lnum) -
				(end_visual.lnum - curwin->w_cursor.lnum);

		    എങ്കിൽ (diff > 0)		// closest to end
			VIsual = start_visual;
		    അല്ലെങ്കിൽ എങ്കിൽ (diff < 0)	// closest to start
			VIsual = end_visual;
		    അല്ലെങ്കിൽ			// in the middle line
		    {
			എങ്കിൽ (curwin->w_cursor.col <
					(start_visual.col + end_visual.col) / 2)
			    VIsual = end_visual;
			അല്ലെങ്കിൽ
			    VIsual = start_visual;
		    }
		}
	    }
	}
    }
    // If Visual mode started in insert mode, execute "CTRL-O"
    അല്ലെങ്കിൽ എങ്കിൽ ((State & MODE_തിരുകുക) && VIsual_active)
	stuffcharReadbuff(Ctrl_O);

    // Middle mouse click: Put text before cursor.
    എങ്കിൽ (which_button == MOUSE_നടുവ്)
    {
#ifdef FEAT_CLIPBOARD
	എങ്കിൽ (clip_star.available && regname == 0)
	    regname = '*';
#endif
	എങ്കിൽ (yank_register_mline(regname))
	{
	    എങ്കിൽ (mouse_past_bottom)
		dir = FORWARD;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (mouse_past_eol)
	    dir = FORWARD;

	എങ്കിൽ (fixindent)
	{
	    c1 = (dir == BACKWARD) ? '[' : ']';
	    c2 = 'p';
	}
	അല്ലെങ്കിൽ
	{
	    c1 = (dir == FORWARD) ? 'p' : 'P';
	    c2 = NUL;
	}
	prep_redo(regname, എണ്ണം, NUL, c1, NUL, c2, NUL);

	// Remember where the paste started, so in edit() Insstart can be set
	// to this position
	എങ്കിൽ (restart_edit != 0)
	    where_paste_started = curwin->w_cursor;
	do_put(regname, ശൂന്യ, dir, എണ്ണം, fixindent | PUT_CURSEND);
    }

#എങ്കിൽ defined(FEAT_QUICKFIX)
    // Ctrl-Mouse click or double click in a quickfix window jumps to the
    // error under the mouse pointer.
    അല്ലെങ്കിൽ എങ്കിൽ (((mod_mask & MOD_MASK_CTRL)
		|| (mod_mask & MOD_MASK_MULTI_CLICK) == MOD_MASK_2CLICK)
	    && bt_quickfix(curbuf))
    {
	എങ്കിൽ (curwin->w_llist_ref == ശൂന്യ)	// quickfix window
	    do_cmdline_cmd((char_u *)".cc");
	അല്ലെങ്കിൽ					// location list window
	    do_cmdline_cmd((char_u *)".ll");
	got_click = FALSE;		// ignore drag&release now
    }
#endif

    // Ctrl-Mouse click (or double click in a help window) jumps to the tag
    // under the mouse pointer.
    അല്ലെങ്കിൽ എങ്കിൽ ((mod_mask & MOD_MASK_CTRL) || (curbuf->b_help
		     && (mod_mask & MOD_MASK_MULTI_CLICK) == MOD_MASK_2CLICK))
    {
	എങ്കിൽ (State & MODE_തിരുകുക)
	    stuffcharReadbuff(Ctrl_O);
	stuffcharReadbuff(Ctrl_RSB);
	got_click = FALSE;		// ignore drag&release now
    }

    // Shift-Mouse click searches for the next occurrence of the word under
    // the mouse pointer
    അല്ലെങ്കിൽ എങ്കിൽ ((mod_mask & MOD_MASK_SHIFT))
    {
	എങ്കിൽ ((State & MODE_തിരുകുക) || (VIsual_active && VIsual_select))
	    stuffcharReadbuff(Ctrl_O);
	എങ്കിൽ (which_button == MOUSE_ഇടത്)
	    stuffcharReadbuff('*');
	അല്ലെങ്കിൽ	// MOUSE_RIGHT
	    stuffcharReadbuff('#');
    }

    // Handle double clicks, unless on status line
    അല്ലെങ്കിൽ എങ്കിൽ (in_status_വരി)
    {
#ifdef FEAT_MOUSESHAPE
	എങ്കിൽ ((is_drag || is_click) && !drag_status_വരി)
	{
	    drag_status_വരി = TRUE;
	    update_mouseshape(-1);
	}
#endif
    }
    അല്ലെങ്കിൽ എങ്കിൽ (in_sep_വരി)
    {
#ifdef FEAT_MOUSESHAPE
	എങ്കിൽ ((is_drag || is_click) && !drag_sep_വരി)
	{
	    drag_sep_വരി = TRUE;
	    update_mouseshape(-1);
	}
#endif
    }
    അല്ലെങ്കിൽ എങ്കിൽ ((mod_mask & MOD_MASK_MULTI_CLICK)
				       && (State & (MODE_NORMAL | MODE_തിരുകുക))
	     && mouse_has(MOUSE_VISUAL))
    {
	എങ്കിൽ (is_click || !VIsual_active)
	{
	    എങ്കിൽ (VIsual_active)
		orig_cursor = VIsual;
	    അല്ലെങ്കിൽ
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
	    എങ്കിൽ ((mod_mask & MOD_MASK_MULTI_CLICK) == MOD_MASK_2CLICK)
	    {
		// Double click with ALT pressed makes it blockwise.
		എങ്കിൽ (mod_mask & MOD_MASK_ALT)
		    VIsual_mode = Ctrl_V;
		അല്ലെങ്കിൽ
		    VIsual_mode = 'v';
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ ((mod_mask & MOD_MASK_MULTI_CLICK) == MOD_MASK_3CLICK)
		VIsual_mode = 'V';
	    അല്ലെങ്കിൽ എങ്കിൽ ((mod_mask & MOD_MASK_MULTI_CLICK) == MOD_MASK_4CLICK)
		VIsual_mode = Ctrl_V;
#ifdef FEAT_CLIPBOARD
	    // Make sure the clipboard gets updated.  Needed because start and
	    // end may still be the same, and the selection needs to be owned
	    clip_star.vmode = NUL;
#endif
	}
	// A double click selects a word or a block.
	എങ്കിൽ ((mod_mask & MOD_MASK_MULTI_CLICK) == MOD_MASK_2CLICK)
	{
	    pos_T	*pos = ശൂന്യ;
	    പൂർണ്ണസംഖ്യ		gc;

	    എങ്കിൽ (is_click)
	    {
		// If the character under the cursor (skipping white space) is
		// not a word character, try finding a match and select a (),
		// {}, [], #if/#endif, etc. block.
		end_visual = curwin->w_cursor;
		ആകുമ്പോൾ (gc = gchar_pos(&end_visual), VIM_ISWHITE(gc))
		    inc(&end_visual);
		എങ്കിൽ (oap != ശൂന്യ)
		    oap->motion_type = MCHAR;
		എങ്കിൽ (oap != ശൂന്യ
			&& VIsual_mode == 'v'
			&& !vim_iswordc(gchar_pos(&end_visual))
			&& EQUAL_POS(curwin->w_cursor, VIsual)
			&& (pos = findmatch(oap, NUL)) != ശൂന്യ)
		{
		    curwin->w_cursor = *pos;
		    എങ്കിൽ (oap->motion_type == MLINE)
			VIsual_mode = 'V';
		    അല്ലെങ്കിൽ എങ്കിൽ (*p_sel == 'e')
		    {
			എങ്കിൽ (LT_POS(curwin->w_cursor, VIsual))
			    ++VIsual.col;
			അല്ലെങ്കിൽ
			    ++curwin->w_cursor.col;
		    }
		}
	    }

	    എങ്കിൽ (pos == ശൂന്യ && (is_click || is_drag))
	    {
		// When not found a match or when dragging: extend to include
		// a word.
		എങ്കിൽ (LT_POS(curwin->w_cursor, orig_cursor))
		{
		    find_start_of_word(&curwin->w_cursor);
		    find_end_of_word(&VIsual);
		}
		അല്ലെങ്കിൽ
		{
		    find_start_of_word(&VIsual);
		    എങ്കിൽ (*p_sel == 'e' && *ml_get_cursor() != NUL)
			curwin->w_cursor.col +=
					 (*mb_ptr2len)(ml_get_cursor());
		    find_end_of_word(&curwin->w_cursor);
		}
	    }
	    curwin->w_set_curswant = true;
	}
	എങ്കിൽ (is_click)
	    redraw_curbuf_later(UPD_INVERTED);	// update the inversion
    }
    അല്ലെങ്കിൽ എങ്കിൽ (VIsual_active && !old_active)
    {
	എങ്കിൽ (mod_mask & MOD_MASK_ALT)
	    VIsual_mode = Ctrl_V;
	അല്ലെങ്കിൽ
	    VIsual_mode = 'v';
    }

    // If Visual mode changed show it later.
    എങ്കിൽ ((!VIsual_active && old_active && mode_displayed)
	    || (VIsual_active && p_smd && സന്ദേശം_silent == 0
				 && (!old_active || VIsual_mode != old_mode)))
	redraw_cmdline = TRUE;

    തിരികെ moved;
}

    ശൂന്യം
ins_mouse(പൂർണ്ണസംഖ്യ c)
{
    pos_T	tpos;
    win_T	*old_curwin = curwin;

#ifdef FEAT_GUI
    // When GUI is active, also move/paste when 'mouse' is empty
    എങ്കിൽ (!gui.in_use)
#endif
	എങ്കിൽ (!mouse_has(MOUSE_തിരുകുക))
	    തിരികെ;

    undisplay_dollar();
    tpos = curwin->w_cursor;
    എങ്കിൽ (do_mouse(ശൂന്യ, c, BACKWARD, 1L, 0))
    {
	win_T	*new_curwin = curwin;

	എങ്കിൽ (curwin != old_curwin && win_valid(old_curwin))
	{
	    // Mouse took us to another window.  We need to go back to the
	    // previous one to stop insert there properly.
	    curwin = old_curwin;
	    curbuf = curwin->w_buffer;
#ifdef FEAT_JOB_CHANNEL
	    എങ്കിൽ (bt_prompt(curbuf))
		// Restart Insert mode when re-entering the prompt buffer.
		curbuf->b_prompt_തിരുകുക = 'A';
#endif
	}
	start_arrow(curwin == old_curwin ? &tpos : ശൂന്യ);
	എങ്കിൽ (curwin != new_curwin && win_valid(new_curwin))
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
    സ്ഥിതം ശൂന്യം
do_mousescroll(cmdarg_T *cap)
{
    പൂർണ്ണസംഖ്യ shift_or_ctrl = mod_mask & (MOD_MASK_SHIFT | MOD_MASK_CTRL);

#ifdef FEAT_TERMINAL
    എങ്കിൽ (term_use_loop())
	// This window is a terminal window, send the mouse event there.
	// Set "typed" to FALSE to avoid an endless loop.
	send_താക്കോലുകൾ_to_term(curbuf->b_term, cap->cmdchar, mod_mask, FALSE);
    അല്ലെങ്കിൽ
#endif
    എങ്കിൽ (cap->arg == MSCR_UP || cap->arg == MSCR_DOWN)
    {
	// Vertical scrolling
	എങ്കിൽ (!(State & MODE_തിരുകുക) && (mouse_vert_step < 0 || shift_or_ctrl))
	{
	    // whole page up or down
	    pagescroll(cap->arg == MSCR_UP ? FORWARD : BACKWARD, 1L, FALSE);
	}
	അല്ലെങ്കിൽ
	{
	    എങ്കിൽ (mouse_vert_step < 0 || shift_or_ctrl)
	    {
		// whole page up or down
		cap->count1 = (long)(curwin->w_botline - curwin->w_topline);
	    }
	    // Don't scroll more than half the window height.
	    അല്ലെങ്കിൽ എങ്കിൽ (curwin->w_height < mouse_vert_step * 2)
	    {
		cap->count1 = curwin->w_height / 2;
		എങ്കിൽ (cap->count1 == 0)
		    cap->count1 = 1;
	    }
	    അല്ലെങ്കിൽ
	    {
		cap->count1 = mouse_vert_step;
	    }
	    cap->count0 = cap->count1;
	    nv_scroll_വരി(cap);
	}

#ifdef FEAT_PROP_POPUP
	എങ്കിൽ (WIN_IS_POPUP(curwin))
	    popup_set_firstline(curwin);
#endif
    }
    അല്ലെങ്കിൽ
    {
	// Horizontal scrolling
	long step = (mouse_hor_step < 0 || shift_or_ctrl)
					    ? curwin->w_width : mouse_hor_step;
	long leftcol = curwin->w_leftcol
				     + (cap->arg == MSCR_വലത് ? -step : step);
	എങ്കിൽ (leftcol < 0)
	    leftcol = 0;
	do_mousescroll_horiz((long_u)leftcol);
    }
    may_trigger_win_scrolled_resized();
}

/*
 * Insert mode implementation for scrolling in direction "dir", which is
 * one of the MSCR_ values.
 */
    ശൂന്യം
ins_mousescroll(പൂർണ്ണസംഖ്യ dir)
{
    cmdarg_T	cap;
    oparg_T	oa;
    CLEAR_FIELD(cap);
    clear_oparg(&oa);
    cap.oap = &oa;
    cap.arg = dir;

#ifdef FEAT_TABPANEL
    എങ്കിൽ (mouse_row >= 0 && mouse_col >= 0
	    && (dir == MSCR_UP || dir == MSCR_DOWN)
	    && mouse_on_tabpanel())
    {
	(ശൂന്യം)tabpanel_scroll(dir == MSCR_UP ? 1 : -1,
		mouse_vert_step > 0 ? mouse_vert_step : 3);
	തിരികെ;
    }
#endif

    തിരഞ്ഞെടുപ്പ് (dir)
    {
	അവസ്ഥ MSCR_UP:
	    cap.cmdchar = K_MOUSEUP;
	    നിർത്തുക;
	അവസ്ഥ MSCR_DOWN:
	    cap.cmdchar = K_MOUSEDOWN;
	    നിർത്തുക;
	അവസ്ഥ MSCR_ഇടത്:
	    cap.cmdchar = K_MOUSELEFT;
	    നിർത്തുക;
	അവസ്ഥ MSCR_വലത്:
	    cap.cmdchar = K_MOUSERIGHT;
	    നിർത്തുക;
	സ്ഥിരസ്ഥിതി:
	    siemsg("Invalid ins_mousescroll() argument: %d", dir);
    }

    win_T *old_curwin = curwin;
    എങ്കിൽ (mouse_row >= 0 && mouse_col >= 0)
    {
	// Find the window at the mouse pointer coordinates.
	// NOTE: Must restore "curwin" to "old_curwin" before returning!
	പൂർണ്ണസംഖ്യ row = mouse_row;
	പൂർണ്ണസംഖ്യ col = mouse_col;
	curwin = mouse_find_win(&row, &col, FIND_POPUP);
	എങ്കിൽ (curwin == ശൂന്യ)
	{
	    curwin = old_curwin;
	    തിരികെ;
	}
	curbuf = curwin->w_buffer;
    }

    എങ്കിൽ (curwin == old_curwin)
    {
	// Don't scroll the current window if the popup menu is visible.
	എങ്കിൽ (pum_visible())
	    തിരികെ;

	undisplay_dollar();
    }

    linenr_T	orig_topline = curwin->w_topline;
    colnr_T	orig_leftcol = curwin->w_leftcol;
    pos_T	orig_cursor = curwin->w_cursor;

    // Call the common mouse scroll function shared with other modes.
    do_mousescroll(&cap);

    പൂർണ്ണസംഖ്യ did_scroll = (orig_topline != curwin->w_topline
		   || orig_leftcol != curwin->w_leftcol);

    curwin->w_redr_status = true;
    curwin = old_curwin;
    curbuf = curwin->w_buffer;

    // If the window actually scrolled and the popup menu may overlay the
    // window, need to redraw it.
    എങ്കിൽ (did_scroll && pum_visible())
    {
	// TODO: Would be more efficient to only redraw the windows that are
	// overlapped by the popup menu.
	redraw_all_later(UPD_NOT_VALID);
	ins_compl_show_pum();
    }

    എങ്കിൽ (!EQUAL_POS(curwin->w_cursor, orig_cursor))
    {
	start_arrow(&orig_cursor);
	set_can_cindent(TRUE);
    }
}

#എങ്കിൽ defined(FEAT_PROP_POPUP)
/*
 * Command-line mode implementation for scrolling in direction "dir", which is
 * one of the MSCR_ values.  Scrolls the completion info popup when the mouse
 * pointer is on top of it.
 * Returns TRUE when the info popup was scrolled.
 */
    പൂർണ്ണസംഖ്യ
cmdline_mousescroll(പൂർണ്ണസംഖ്യ dir)
{
    cmdarg_T	cap;
    oparg_T	oa;

    CLEAR_FIELD(cap);
    clear_oparg(&oa);
    cap.oap = &oa;
    cap.arg = dir;

    തിരഞ്ഞെടുപ്പ് (dir)
    {
	അവസ്ഥ MSCR_UP:	 cap.cmdchar = K_MOUSEUP; നിർത്തുക;
	അവസ്ഥ MSCR_DOWN:	 cap.cmdchar = K_MOUSEDOWN; നിർത്തുക;
	അവസ്ഥ MSCR_ഇടത്:	 cap.cmdchar = K_MOUSELEFT; നിർത്തുക;
	അവസ്ഥ MSCR_വലത്: cap.cmdchar = K_MOUSERIGHT; നിർത്തുക;
    }

    എങ്കിൽ (mouse_row < 0 || mouse_col < 0)
	തിരികെ FALSE;

    പൂർണ്ണസംഖ്യ	    row = mouse_row;
    പൂർണ്ണസംഖ്യ	    col = mouse_col;
    win_T   *wp;

    // Only scroll when the mouse is on top of the info popup.
    wp = mouse_find_win(&row, &col, FIND_POPUP);
    എങ്കിൽ (wp == ശൂന്യ || !WIN_IS_POPUP(wp) || !(wp->w_popup_flags & POPF_INFO)
							|| !wp->w_has_scrollbar)
	തിരികെ FALSE;

    win_T   *old_curwin = curwin;

    curwin = wp;
    curbuf = wp->w_buffer;
    // Call the common mouse scroll function shared with other modes.
    do_mousescroll(&cap);
    curwin = old_curwin;
    curbuf = curwin->w_buffer;

    // Cmdline mode doesn't normally call update_screen(), so redraw the
    // completion popup menu, which also repaints the info popup.
    എങ്കിൽ (cmdline_pum_active())
	cmdline_pum_display();
    തിരികെ TRUE;
}
#endif

/*
 * Return TRUE if "c" is a mouse key.
 */
    പൂർണ്ണസംഖ്യ
is_mouse_താക്കോൽ(പൂർണ്ണസംഖ്യ c)
{
    തിരികെ c == K_LEFTMOUSE
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

സ്ഥിതം ഘടന mousetable
{
    പൂർണ്ണസംഖ്യ	    pseudo_code;	// Code for pseudo mouse event
    പൂർണ്ണസംഖ്യ	    button;		// Which mouse button is it?
    പൂർണ്ണസംഖ്യ	    is_click;		// Is it a mouse button click event?
    പൂർണ്ണസംഖ്യ	    is_drag;		// Is it a mouse drag event?
} mouse_table[] =
{
    {(പൂർണ്ണസംഖ്യ)KE_LEFTMOUSE,		MOUSE_ഇടത്,	TRUE,	FALSE},
#ifdef FEAT_GUI
    {(പൂർണ്ണസംഖ്യ)KE_LEFTMOUSE_NM,	MOUSE_ഇടത്,	TRUE,	FALSE},
#endif
    {(പൂർണ്ണസംഖ്യ)KE_LEFTDRAG,		MOUSE_ഇടത്,	FALSE,	TRUE},
    {(പൂർണ്ണസംഖ്യ)KE_LEFTRELEASE,	MOUSE_ഇടത്,	FALSE,	FALSE},
#ifdef FEAT_GUI
    {(പൂർണ്ണസംഖ്യ)KE_LEFTRELEASE_NM,	MOUSE_ഇടത്,	FALSE,	FALSE},
#endif
    {(പൂർണ്ണസംഖ്യ)KE_MIDDLEMOUSE,	MOUSE_നടുവ്,	TRUE,	FALSE},
    {(പൂർണ്ണസംഖ്യ)KE_MIDDLEDRAG,	MOUSE_നടുവ്,	FALSE,	TRUE},
    {(പൂർണ്ണസംഖ്യ)KE_MIDDLERELEASE,	MOUSE_നടുവ്,	FALSE,	FALSE},
    {(പൂർണ്ണസംഖ്യ)KE_RIGHTMOUSE,	MOUSE_വലത്,	TRUE,	FALSE},
    {(പൂർണ്ണസംഖ്യ)KE_RIGHTDRAG,		MOUSE_വലത്,	FALSE,	TRUE},
    {(പൂർണ്ണസംഖ്യ)KE_RIGHTRELEASE,	MOUSE_വലത്,	FALSE,	FALSE},
    {(പൂർണ്ണസംഖ്യ)KE_X1MOUSE,		MOUSE_X1,	TRUE,	FALSE},
    {(പൂർണ്ണസംഖ്യ)KE_X1DRAG,		MOUSE_X1,	FALSE,	TRUE},
    {(പൂർണ്ണസംഖ്യ)KE_X1RELEASE,		MOUSE_X1,	FALSE,	FALSE},
    {(പൂർണ്ണസംഖ്യ)KE_X2MOUSE,		MOUSE_X2,	TRUE,	FALSE},
    {(പൂർണ്ണസംഖ്യ)KE_X2DRAG,		MOUSE_X2,	FALSE,	TRUE},
    {(പൂർണ്ണസംഖ്യ)KE_X2RELEASE,		MOUSE_X2,	FALSE,	FALSE},
    // DRAG without CLICK
    {(പൂർണ്ണസംഖ്യ)KE_MOUSEMOVE,		MOUSE_RELEASE,	FALSE,	TRUE},
    // RELEASE without CLICK
    {(പൂർണ്ണസംഖ്യ)KE_IGNORE,		MOUSE_RELEASE,	FALSE,	FALSE},
    {0,				0,		0,	0},
};

/*
 * Look up the given mouse code to return the relevant information in the other
 * arguments.  Return which button is down or was released.
 */
    പൂർണ്ണസംഖ്യ
get_mouse_button(പൂർണ്ണസംഖ്യ code, പൂർണ്ണസംഖ്യ *is_click, പൂർണ്ണസംഖ്യ *is_drag)
{
    പൂർണ്ണസംഖ്യ	    i;

    വേണ്ടി (i = 0; mouse_table[i].pseudo_code; i++)
	എങ്കിൽ (code == mouse_table[i].pseudo_code)
	{
	    *is_click = mouse_table[i].is_click;
	    *is_drag = mouse_table[i].is_drag;
	    തിരികെ mouse_table[i].button;
	}
    തിരികെ 0;	    // Shouldn't get here
}

/*
 * Return the appropriate pseudo mouse event token (KE_LEFTMOUSE etc) based on
 * the given information about which mouse button is down, and whether the
 * mouse was clicked, dragged or released.
 */
    പൂർണ്ണസംഖ്യ
get_pseudo_mouse_code(
    പൂർണ്ണസംഖ്യ	    button,	// eg MOUSE_LEFT
    പൂർണ്ണസംഖ്യ	    is_click,
    പൂർണ്ണസംഖ്യ	    is_drag)
{
    പൂർണ്ണസംഖ്യ	    i;

    വേണ്ടി (i = 0; mouse_table[i].pseudo_code; i++)
	എങ്കിൽ (button == mouse_table[i].button
	    && is_click == mouse_table[i].is_click
	    && is_drag == mouse_table[i].is_drag)
	{
#ifdef FEAT_GUI
	    // Trick: a non mappable left click and release has mouse_col -1
	    // or added MOUSE_COLOFF.  Used for 'mousefocus' in
	    // gui_mouse_moved()
	    എങ്കിൽ (mouse_col < 0 || mouse_col > MOUSE_COLOFF)
	    {
		എങ്കിൽ (mouse_col < 0)
		    mouse_col = 0;
		അല്ലെങ്കിൽ
		    mouse_col -= MOUSE_COLOFF;
		എങ്കിൽ (mouse_table[i].pseudo_code == (പൂർണ്ണസംഖ്യ)KE_LEFTMOUSE)
		    തിരികെ (പൂർണ്ണസംഖ്യ)KE_LEFTMOUSE_NM;
		എങ്കിൽ (mouse_table[i].pseudo_code == (പൂർണ്ണസംഖ്യ)KE_LEFTRELEASE)
		    തിരികെ (പൂർണ്ണസംഖ്യ)KE_LEFTRELEASE_NM;
	    }
#endif
	    തിരികെ mouse_table[i].pseudo_code;
	}
    തിരികെ (പൂർണ്ണസംഖ്യ)KE_IGNORE;	    // not recognized, ignore it
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
സ്ഥിതം പൂർണ്ണസംഖ്യ has_mouse_termcode = 0;

    ശൂന്യം
set_mouse_termcode(
    പൂർണ്ണസംഖ്യ		n,	// KS_MOUSE, KS_NETTERM_MOUSE or KS_DEC_MOUSE
    char_u	*s)
{
    char_u	name[2];

    name[0] = n;
    name[1] = KE_FILLER;
    add_termcode(name, s, FALSE);
#ifdef FEAT_MOUSE_JSB
    എങ്കിൽ (n == KS_JSBTERM_MOUSE)
	has_mouse_termcode |= HMT_JSBTERM;
    അല്ലെങ്കിൽ
#endif
#ifdef FEAT_MOUSE_NET
    എങ്കിൽ (n == KS_NETTERM_MOUSE)
	has_mouse_termcode |= HMT_NETTERM;
    അല്ലെങ്കിൽ
#endif
#ifdef FEAT_MOUSE_DEC
    എങ്കിൽ (n == KS_DEC_MOUSE)
	has_mouse_termcode |= HMT_DEC;
    അല്ലെങ്കിൽ
#endif
#ifdef FEAT_MOUSE_PTERM
    എങ്കിൽ (n == KS_PTERM_MOUSE)
	has_mouse_termcode |= HMT_PTERM;
    അല്ലെങ്കിൽ
#endif
#ifdef FEAT_MOUSE_URXVT
    എങ്കിൽ (n == KS_URXVT_MOUSE)
	has_mouse_termcode |= HMT_URXVT;
    അല്ലെങ്കിൽ
#endif
#ifdef FEAT_MOUSE_GPM
    എങ്കിൽ (n == KS_GPM_MOUSE)
	has_mouse_termcode |= HMT_GPM;
    അല്ലെങ്കിൽ
#endif
    എങ്കിൽ (n == KS_SGR_MOUSE)
	has_mouse_termcode |= HMT_SGR;
    അല്ലെങ്കിൽ എങ്കിൽ (n == KS_SGR_MOUSE_RELEASE)
	has_mouse_termcode |= HMT_SGR_REL;
    അല്ലെങ്കിൽ
	has_mouse_termcode |= HMT_NORMAL;
}

#എങ്കിൽ defined(UNIX) || defined(VMS)
    ശൂന്യം
del_mouse_termcode(
    പൂർണ്ണസംഖ്യ		n)	// KS_MOUSE, KS_NETTERM_MOUSE or KS_DEC_MOUSE
{
    char_u	name[2];

    name[0] = n;
    name[1] = KE_FILLER;
    del_termcode(name);
# ifdef FEAT_MOUSE_JSB
    എങ്കിൽ (n == KS_JSBTERM_MOUSE)
	has_mouse_termcode &= ~HMT_JSBTERM;
    അല്ലെങ്കിൽ
# endif
# ifdef FEAT_MOUSE_NET
    എങ്കിൽ (n == KS_NETTERM_MOUSE)
	has_mouse_termcode &= ~HMT_NETTERM;
    അല്ലെങ്കിൽ
# endif
# ifdef FEAT_MOUSE_DEC
    എങ്കിൽ (n == KS_DEC_MOUSE)
	has_mouse_termcode &= ~HMT_DEC;
    അല്ലെങ്കിൽ
# endif
# ifdef FEAT_MOUSE_PTERM
    എങ്കിൽ (n == KS_PTERM_MOUSE)
	has_mouse_termcode &= ~HMT_PTERM;
    അല്ലെങ്കിൽ
# endif
# ifdef FEAT_MOUSE_URXVT
    എങ്കിൽ (n == KS_URXVT_MOUSE)
	has_mouse_termcode &= ~HMT_URXVT;
    അല്ലെങ്കിൽ
# endif
# ifdef FEAT_MOUSE_GPM
    എങ്കിൽ (n == KS_GPM_MOUSE)
	has_mouse_termcode &= ~HMT_GPM;
    അല്ലെങ്കിൽ
# endif
    എങ്കിൽ (n == KS_SGR_MOUSE)
	has_mouse_termcode &= ~HMT_SGR;
    അല്ലെങ്കിൽ എങ്കിൽ (n == KS_SGR_MOUSE_RELEASE)
	has_mouse_termcode &= ~HMT_SGR_REL;
    അല്ലെങ്കിൽ
	has_mouse_termcode &= ~HMT_NORMAL;
}
#endif

/*
 * setmouse() - switch mouse on/off depending on current mode and 'mouse'
 */
    ശൂന്യം
setmouse(ശൂന്യം)
{
    പൂർണ്ണസംഖ്യ	    checkfor;

#ifdef FEAT_MOUSESHAPE
    update_mouseshape(-1);
#endif

    // Should be outside proc, but may break MOUSESHAPE
#ifdef FEAT_GUI
    // In the GUI the mouse is always enabled.
    എങ്കിൽ (gui.in_use)
	തിരികെ;
#endif
    // be quick when mouse is off
    എങ്കിൽ (*p_mouse == NUL || has_mouse_termcode == 0)
	തിരികെ;

    // don't switch mouse on when not in raw mode (Ex mode)
    എങ്കിൽ (cur_tmode != TMODE_RAW)
    {
	mch_setmouse(FALSE);
	തിരികെ;
    }

    എങ്കിൽ (VIsual_active)
	checkfor = MOUSE_VISUAL;
    അല്ലെങ്കിൽ എങ്കിൽ (State == MODE_HITRETURN || State == MODE_ASKMORE
						     || State == MODE_SETWSIZE)
	checkfor = MOUSE_RETURN;
    അല്ലെങ്കിൽ എങ്കിൽ (State & MODE_തിരുകുക)
	checkfor = MOUSE_തിരുകുക;
    അല്ലെങ്കിൽ എങ്കിൽ (State & MODE_CMDLINE)
	checkfor = MOUSE_COMMAND;
    അല്ലെങ്കിൽ എങ്കിൽ (State == MODE_CONFIRM || State == MODE_EXTERNCMD)
	checkfor = ' '; // don't use mouse for ":confirm" or ":!cmd"
    അല്ലെങ്കിൽ
	checkfor = MOUSE_NORMAL;    // assume normal mode

    എങ്കിൽ (mouse_has(checkfor))
	mch_setmouse(TRUE);
    അല്ലെങ്കിൽ
	mch_setmouse(FALSE);
}

/*
 * Return TRUE if
 * - "c" is in 'mouse', or
 * - 'a' is in 'mouse' and "c" is in MOUSE_A, or
 * - the current buffer is a help file and 'h' is in 'mouse' and we are in a
 *   normal editing mode (not at hit-return message).
 */
    പൂർണ്ണസംഖ്യ
mouse_has(പൂർണ്ണസംഖ്യ c)
{
    char_u	*p;

    വേണ്ടി (p = p_mouse; *p; ++p)
	തിരഞ്ഞെടുപ്പ് (*p)
	{
	    അവസ്ഥ 'a': എങ്കിൽ (vim_strchr((char_u *)MOUSE_A, c) != ശൂന്യ)
			  തിരികെ TRUE;
		      നിർത്തുക;
	    അവസ്ഥ MOUSE_HELP: എങ്കിൽ (c != MOUSE_RETURN && curbuf->b_help)
				 തിരികെ TRUE;
			     നിർത്തുക;
	    സ്ഥിരസ്ഥിതി: എങ്കിൽ (c == *p) തിരികെ TRUE; നിർത്തുക;
	}
    തിരികെ FALSE;
}

/*
 * Return TRUE when 'mousemodel' is set to "popup" or "popup_setpos".
 */
    പൂർണ്ണസംഖ്യ
mouse_model_popup(ശൂന്യം)
{
    തിരികെ (p_mousem[0] == 'p');
}

/*
 * Call a click-region callback function.
 * "regions"/"region_count" describe the resolved click regions,
 * "winid" is stored as the "winid" key in the info dict (0 for tabline).
 * Returns TRUE if the function was called and handled the click.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
stl_click_handler_regions(
	stl_click_region_T  *regions,
	പൂർണ്ണസംഖ്യ		    region_എണ്ണം,
	പൂർണ്ണസംഖ്യ		    winid,
	char_u		    *area_name,
	പൂർണ്ണസംഖ്യ		    mrow,
	പൂർണ്ണസംഖ്യ		    mcol,
	പൂർണ്ണസംഖ്യ		    which_button,
	പൂർണ്ണസംഖ്യ		    mods)
{
#ifdef FEAT_EVAL
    പൂർണ്ണസംഖ്യ		n;
    പൂർണ്ണസംഖ്യ		nclicks;
    char_u	button_str[2];
    char_u	mods_str[4];
    പൂർണ്ണസംഖ്യ		mi = 0;
    dict_T	*info;
    typval_T	argvars[2];
    typval_T	rettv;
    funcexe_T	funcexe;
    പൂർണ്ണസംഖ്യ		col = mcol;

    എങ്കിൽ (regions == ശൂന്യ || region_എണ്ണം == 0)
	തിരികെ FALSE;

    // Find the click region at the given row and column.
    വേണ്ടി (n = 0; n < region_എണ്ണം; n++)
    {
	എങ്കിൽ (regions[n].row == mrow
		&& col >= regions[n].col_start
		&& col < regions[n].col_end)
	    നിർത്തുക;
    }
    എങ്കിൽ (n >= region_എണ്ണം || regions[n].funcname == ശൂന്യ)
	തിരികെ FALSE;

    // Build the info dictionary.
    info = dict_alloc();
    എങ്കിൽ (info == ശൂന്യ)
	തിരികെ FALSE;

    dict_add_സംഖ്യ(info, "minwid", regions[n].minwid);

    // Determine number of clicks.
    // MOD_MASK_2CLICK=0x20, MOD_MASK_3CLICK=0x40, MOD_MASK_4CLICK=0x60
    nclicks = ((mods & MOD_MASK_MULTI_CLICK) >> 5) + 1;
    എങ്കിൽ (nclicks > 3)
	nclicks = 3;
    dict_add_സംഖ്യ(info, "nclicks", nclicks);

    // Button.
    എങ്കിൽ (which_button == MOUSE_ഇടത്)
	button_str[0] = 'l';
    അല്ലെങ്കിൽ എങ്കിൽ (which_button == MOUSE_വലത്)
	button_str[0] = 'r';
    അല്ലെങ്കിൽ
	button_str[0] = 'm';
    button_str[1] = NUL;
    dict_add_string_len(info, "button", button_str, 1);

    // Modifiers.
    എങ്കിൽ (mods & MOD_MASK_SHIFT)
	mods_str[mi++] = 's';
    എങ്കിൽ (mods & MOD_MASK_CTRL)
	mods_str[mi++] = 'c';
    എങ്കിൽ (mods & MOD_MASK_ALT)
	mods_str[mi++] = 'a';
    mods_str[mi] = NUL;
    dict_add_string_len(info, "mods", mods_str, mi);

    dict_add_സംഖ്യ(info, "winid", winid);

    // "area": which option the clicked region belongs to.  Lets a shared
    // dispatcher distinguish 'statusline', 'tabline' and 'tabpanel' without
    // having to overload winid == 0.
    dict_add_string(info, "area", area_name);

    // Expose tab page number for 'tabpanel' regions.
    എങ്കിൽ (regions[n].tabnr > 0)
	dict_add_സംഖ്യ(info, "tabnr", regions[n].tabnr);

    // Call the function with the info dict as argument.
    argvars[0].v_type = VAR_DICT;
    argvars[0].vval.v_dict = info;
    ++info->dv_refcount;
    argvars[1].v_type = VAR_UNKNOWN;

    rettv.v_type = VAR_സംഖ്യ;
    rettv.vval.v_സംഖ്യ = 0;

    CLEAR_FIELD(funcexe);
    funcexe.fe_evaluate = TRUE;
    (ശൂന്യം)call_func(regions[n].funcname, -1,
					    &rettv, 1, argvars, &funcexe);

    n = (പൂർണ്ണസംഖ്യ)rettv.vval.v_സംഖ്യ;
    clear_tv(&rettv);
    dict_unref(info);

    എങ്കിൽ (n != 0)
    {
	// Make sure the tabline gets redrawn too when the callback asks for
	// a redraw (redraw_statuslines() only redraws the tabline when
	// redraw_tabline is set).  For tabpanel the whole screen needs to be
	// refreshed.
	എങ്കിൽ (winid == 0)
	    redraw_tabline = TRUE;
# ifdef FEAT_TABPANEL
	എങ്കിൽ (STRCMP(area_name, "tabpanel") == 0)
	    redraw_all_later(UPD_NOT_VALID);
# endif
	redraw_statuslines();
    }

    തിരികെ TRUE;
#അല്ലെങ്കിൽ
    (ശൂന്യം)regions;
    (ശൂന്യം)region_എണ്ണം;
    (ശൂന്യം)winid;
    (ശൂന്യം)area_name;
    (ശൂന്യം)mrow;
    (ശൂന്യം)mcol;
    (ശൂന്യം)which_button;
    (ശൂന്യം)mods;
    തിരികെ FALSE;
#endif
}

/*
 * Call a statusline click handler function for window "wp".
 * Returns TRUE if the function was called and handled the click.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
stl_click_handler(win_T *wp, പൂർണ്ണസംഖ്യ mrow, പൂർണ്ണസംഖ്യ mcol, പൂർണ്ണസംഖ്യ which_button, പൂർണ്ണസംഖ്യ mods)
{
    എങ്കിൽ (wp == ശൂന്യ)
	തിരികെ FALSE;
    തിരികെ stl_click_handler_regions(wp->w_stl_click, wp->w_stl_click_എണ്ണം,
				wp->w_id, (char_u *)"statusline",
				mrow, mcol, which_button, mods);
}

// dragwin is declared near the top of the file

/*
 * Reset the window being dragged.  To be called when switching tab page.
 */
    ശൂന്യം
reset_dragwin(ശൂന്യം)
{
    dragwin = ശൂന്യ;
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
    പൂർണ്ണസംഖ്യ
jump_to_mouse(
    പൂർണ്ണസംഖ്യ		flags,
    പൂർണ്ണസംഖ്യ		*inclusive,	// used for inclusive operator, can be NULL
    പൂർണ്ണസംഖ്യ		which_button)	// MOUSE_LEFT, MOUSE_RIGHT, MOUSE_MIDDLE
{
    സ്ഥിതം പൂർണ്ണസംഖ്യ	on_status_വരി = 0;	// #lines below bottom of window
    സ്ഥിതം പൂർണ്ണസംഖ്യ	on_sep_വരി = 0;	// on separator right of window
#ifdef FEAT_MENU
    സ്ഥിതം പൂർണ്ണസംഖ്യ  in_winbar = FALSE;
#endif
#ifdef FEAT_PROP_POPUP
    സ്ഥിതം പൂർണ്ണസംഖ്യ   in_popup_win = FALSE;
    സ്ഥിതം win_T *click_in_popup_win = ശൂന്യ;
#endif
    സ്ഥിതം പൂർണ്ണസംഖ്യ	prev_row = -1;
    സ്ഥിതം പൂർണ്ണസംഖ്യ	prev_col = -1;
    സ്ഥിതം പൂർണ്ണസംഖ്യ	did_drag = FALSE;	// drag was noticed

    win_T	*wp, *old_curwin;
    pos_T	old_cursor;
    പൂർണ്ണസംഖ്യ		എണ്ണം;
    പൂർണ്ണസംഖ്യ		first;
    പൂർണ്ണസംഖ്യ		row = mouse_row;
    പൂർണ്ണസംഖ്യ		col = mouse_col;
    colnr_T	col_from_screen = -1;
#ifdef FEAT_FOLDING
    പൂർണ്ണസംഖ്യ		mouse_char = ' ';
#endif

    mouse_past_bottom = FALSE;
    mouse_past_eol = FALSE;

    എങ്കിൽ (flags & MOUSE_RELEASED)
    {
	// On button release we may change window focus if positioned on a
	// status line and no dragging happened.
	എങ്കിൽ (dragwin != ശൂന്യ && !did_drag)
	    flags &= ~(MOUSE_FOCUS | MOUSE_DID_MOVE);
	dragwin = ശൂന്യ;
	did_drag = FALSE;
#ifdef FEAT_PROP_POPUP
	എങ്കിൽ (click_in_popup_win != ശൂന്യ && popup_dragwin == ശൂന്യ)
	    popup_close_for_mouse_click(click_in_popup_win);

	popup_dragwin = ശൂന്യ;
	click_in_popup_win = ശൂന്യ;
#endif
    }

    എങ്കിൽ ((flags & MOUSE_DID_MOVE)
	    && prev_row == mouse_row
	    && prev_col == mouse_col)
    {
retnomove:
	// before moving the cursor for a left click which is NOT in a status
	// line, stop Visual mode
	എങ്കിൽ (on_status_വരി)
	    തിരികെ IN_STATUS_വരി;
	എങ്കിൽ (on_sep_വരി)
	    തിരികെ IN_SEP_വരി;
#ifdef FEAT_MENU
	എങ്കിൽ (in_winbar)
	{
	    // A quick second click may arrive as a double-click, but we use it
	    // as a second click in the WinBar.
	    എങ്കിൽ ((mod_mask & MOD_MASK_MULTI_CLICK) && !(flags & MOUSE_RELEASED))
	    {
		wp = mouse_find_win(&row, &col, FAIL_POPUP);
		എങ്കിൽ (wp == ശൂന്യ)
		    തിരികെ IN_UNKNOWN;
		winbar_click(wp, col);
	    }
	    തിരികെ IN_OTHER_WIN | MOUSE_WINBAR;
	}
#endif
	എങ്കിൽ (flags & MOUSE_MAY_STOP_VIS)
	{
	    end_visual_mode_keep_button();
	    redraw_curbuf_later(UPD_INVERTED);	// delete the inversion
	}
#എങ്കിൽ defined(FEAT_CLIPBOARD)
	// Continue a modeless selection in another window.
	എങ്കിൽ (cmdwin_type != 0 && row < cmdwin_win->w_winrow)
	    തിരികെ IN_OTHER_WIN;
#endif
#ifdef FEAT_PROP_POPUP
	// Continue a modeless selection in a popup window or dragging it.
	എങ്കിൽ (in_popup_win)
	{
	    click_in_popup_win = ശൂന്യ;  // don't close it on release
	    എങ്കിൽ (popup_dragwin != ശൂന്യ)
	    {
		// dragging a popup window
		popup_drag(popup_dragwin);
		തിരികെ IN_UNKNOWN;
	    }
	    തിരികെ IN_OTHER_WIN;
	}
#endif
	തിരികെ IN_BUFFER;
    }

    prev_row = mouse_row;
    prev_col = mouse_col;

    എങ്കിൽ (flags & MOUSE_SETPOS)
	പോകുക retnomove;				// ugly goto...

    old_curwin = curwin;
    old_cursor = curwin->w_cursor;

    എങ്കിൽ (!(flags & MOUSE_FOCUS))
    {
	എങ്കിൽ (row < 0 || col < 0) // check if it makes sense
	    തിരികെ IN_UNKNOWN;

	// find the window where the row is in and adjust "row" and "col" to be
	// relative to top-left of the window
	wp = mouse_find_win(&row, &col, FIND_POPUP);
	എങ്കിൽ (wp == ശൂന്യ)
	    തിരികെ IN_UNKNOWN;
	dragwin = ശൂന്യ;

#ifdef FEAT_PROP_POPUP
	// Click in a popup window may start dragging or modeless selection,
	// but not much else.
	എങ്കിൽ (WIN_IS_POPUP(wp))
	{
	    on_sep_വരി = 0;
	    on_status_വരി = 0;
	    in_popup_win = TRUE;
	    എങ്കിൽ (which_button == MOUSE_ഇടത് && popup_close_if_on_X(wp, row, col))
	    {
		തിരികെ IN_UNKNOWN;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (((wp->w_popup_flags & (POPF_DRAG | POPF_RESIZE))
					      && popup_on_border(wp, row, col))
				       || (wp->w_popup_flags & POPF_DRAGALL))
	    {
		popup_dragwin = wp;
		popup_start_drag(wp, row, col);
		തിരികെ IN_UNKNOWN;
	    }
	    // Only close on release, otherwise it's not possible to drag or do
	    // modeless selection.
	    അല്ലെങ്കിൽ എങ്കിൽ (wp->w_popup_close == POPCLOSE_CLICK
		    && which_button == MOUSE_ഇടത്)
	    {
		click_in_popup_win = wp;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (which_button == MOUSE_ഇടത്)
		// If the click is in the scrollbar, may scroll up/down.
		popup_handle_scrollbar_click(wp, row, col);
# ifdef FEAT_CLIPBOARD
	    തിരികെ IN_OTHER_WIN;
# അല്ലെങ്കിൽ
	    തിരികെ IN_UNKNOWN;
# endif
	}
	in_popup_win = FALSE;
	popup_dragwin = ശൂന്യ;
#endif
#ifdef FEAT_MENU
	എങ്കിൽ (row == -1)
	{
	    // A click in the window toolbar does not enter another window or
	    // change Visual highlighting.
	    winbar_click(wp, col);
	    in_winbar = TRUE;
	    തിരികെ IN_OTHER_WIN | MOUSE_WINBAR;
	}
	in_winbar = FALSE;
#endif

	// winpos and height may change in win_enter()!
	എങ്കിൽ (row >= wp->w_height)		// In (or below) status line
	{
	    on_status_വരി = row - wp->w_height + 1;
	    dragwin = wp;
	}
	അല്ലെങ്കിൽ
	    on_status_വരി = 0;
	എങ്കിൽ (col >= wp->w_width)		// In separator line
	{
	    on_sep_വരി = col - wp->w_width + 1;
	    dragwin = wp;
	}
	അല്ലെങ്കിൽ
	    on_sep_വരി = 0;

	// The rightmost character of the status line might be a vertical
	// separator character if there is no connecting window to the right.
	എങ്കിൽ (on_status_വരി && on_sep_വരി)
	{
	    എങ്കിൽ (stl_connected(wp))
		on_sep_വരി = 0;
	    അല്ലെങ്കിൽ
		on_status_വരി = 0;
	}

	// Before jumping to another buffer, or moving the cursor for a left
	// click, stop Visual mode.
	എങ്കിൽ (VIsual_active
		&& (wp->w_buffer != curwin->w_buffer
		    || (!on_status_വരി && !on_sep_വരി
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
	എങ്കിൽ (cmdwin_type != 0 && wp != cmdwin_win)
	{
	    // A click outside the command-line window: Use modeless
	    // selection if possible.  Allow dragging the status lines.
	    on_sep_വരി = 0;
#ifdef FEAT_CLIPBOARD
	    എങ്കിൽ (on_status_വരി)
		തിരികെ IN_STATUS_വരി;
	    തിരികെ IN_OTHER_WIN;
#അല്ലെങ്കിൽ
	    row = 0;
	    col += wp->w_wincol;
	    wp = cmdwin_win;
#endif
	}
#എങ്കിൽ defined(FEAT_PROP_POPUP) && defined(FEAT_TERMINAL)
	എങ്കിൽ (popup_is_popup(curwin) && curbuf->b_term != ശൂന്യ)
	    // terminal in popup window: don't jump to another window
	    തിരികെ IN_OTHER_WIN;
#endif
	// Only change window focus when not clicking on or dragging the
	// status line.  Do change focus when releasing the mouse button
	// (MOUSE_FOCUS was set above if we dragged first).
	എങ്കിൽ (dragwin == ശൂന്യ || (flags & MOUSE_RELEASED))
	    win_enter(wp, TRUE);		// can make wp invalid!

	എങ്കിൽ (curwin != old_curwin)
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
	എങ്കിൽ (on_status_വരി)			// In (or below) status line
	{
	    // Don't use start_arrow() if we're in the same window
	    എങ്കിൽ (curwin == old_curwin)
		തിരികെ IN_STATUS_വരി;
	    അല്ലെങ്കിൽ
		തിരികെ IN_STATUS_വരി | CURSOR_MOVED;
	}
	എങ്കിൽ (on_sep_വരി)			// In (or below) status line
	{
	    // Don't use start_arrow() if we're in the same window
	    എങ്കിൽ (curwin == old_curwin)
		തിരികെ IN_SEP_വരി;
	    അല്ലെങ്കിൽ
		തിരികെ IN_SEP_വരി | CURSOR_MOVED;
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
    അല്ലെങ്കിൽ എങ്കിൽ (on_status_വരി && which_button == MOUSE_ഇടത്)
    {
	എങ്കിൽ (dragwin != ശൂന്യ)
	{
	    // Drag the status line
	    എണ്ണം = row - W_WINROW(dragwin) - dragwin->w_height + 1
							     - on_status_വരി;
	    win_drag_status_വരി(dragwin, എണ്ണം);
	    did_drag |= എണ്ണം;
	}
	തിരികെ IN_STATUS_വരി;			// Cursor didn't move
    }
    അല്ലെങ്കിൽ എങ്കിൽ (on_sep_വരി && which_button == MOUSE_ഇടത്)
    {
	എങ്കിൽ (dragwin != ശൂന്യ)
	{
	    // Drag the separator column
	    എണ്ണം = col - dragwin->w_wincol - dragwin->w_width + 1
								- on_sep_വരി;
	    win_drag_vsep_വരി(dragwin, എണ്ണം);
	    did_drag |= എണ്ണം;
	}
	തിരികെ IN_SEP_വരി;			// Cursor didn't move
    }
#ifdef FEAT_MENU
    അല്ലെങ്കിൽ എങ്കിൽ (in_winbar)
    {
	// After a click on the window toolbar don't start Visual mode.
	തിരികെ IN_OTHER_WIN | MOUSE_WINBAR;
    }
#endif
    അല്ലെങ്കിൽ // keep_window_focus must be TRUE
    {
	// before moving the cursor for a left click, stop Visual mode
	എങ്കിൽ (flags & MOUSE_MAY_STOP_VIS)
	{
	    end_visual_mode_keep_button();
	    redraw_curbuf_later(UPD_INVERTED);	// delete the inversion
	}

#എങ്കിൽ defined(FEAT_CLIPBOARD)
	// Continue a modeless selection in another window.
	എങ്കിൽ (cmdwin_type != 0 && row < cmdwin_win->w_winrow)
	    തിരികെ IN_OTHER_WIN;
#endif
#ifdef FEAT_PROP_POPUP
	എങ്കിൽ (in_popup_win)
	{
	    എങ്കിൽ (popup_dragwin != ശൂന്യ)
	    {
		// dragging a popup window
		popup_drag(popup_dragwin);
		തിരികെ IN_UNKNOWN;
	    }
	    // continue a modeless selection in a popup window
	    click_in_popup_win = ശൂന്യ;
	    തിരികെ IN_OTHER_WIN;
	}
#endif

	row -= W_WINROW(curwin);
	col -= curwin->w_wincol;

	// When clicking beyond the end of the window, scroll the screen.
	// Scroll by however many rows outside the window we are.
	എങ്കിൽ (row < 0)
	{
	    എണ്ണം = 0;
	    വേണ്ടി (first = TRUE; curwin->w_topline > 1; )
	    {
#ifdef FEAT_DIFF
		എങ്കിൽ (curwin->w_topfill < diff_check_fill(curwin, curwin->w_topline))
		    ++എണ്ണം;
		അല്ലെങ്കിൽ
#endif
		    എണ്ണം += plines(curwin->w_topline - 1);
		എങ്കിൽ (!first && എണ്ണം > -row)
		    നിർത്തുക;
		first = FALSE;
#ifdef FEAT_FOLDING
		(ശൂന്യം)hasFolding(curwin->w_topline, &curwin->w_topline, ശൂന്യ);
#endif
#ifdef FEAT_DIFF
		എങ്കിൽ (curwin->w_topfill < diff_check_fill(curwin, curwin->w_topline))
		    ++curwin->w_topfill;
		അല്ലെങ്കിൽ
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
	അല്ലെങ്കിൽ എങ്കിൽ (row >= curwin->w_height)
	{
	    എണ്ണം = 0;
	    വേണ്ടി (first = TRUE; curwin->w_topline < curbuf->b_ml.ml_വരി_എണ്ണം; )
	    {
#ifdef FEAT_DIFF
		എങ്കിൽ (curwin->w_topfill > 0)
		    ++എണ്ണം;
		അല്ലെങ്കിൽ
#endif
		    എണ്ണം += plines(curwin->w_topline);
		എങ്കിൽ (!first && എണ്ണം > row - curwin->w_height + 1)
		    നിർത്തുക;
		first = FALSE;
#ifdef FEAT_FOLDING
		എങ്കിൽ (hasFolding(curwin->w_topline, ശൂന്യ, &curwin->w_topline)
			&& curwin->w_topline == curbuf->b_ml.ml_വരി_എണ്ണം)
		    നിർത്തുക;
#endif
#ifdef FEAT_DIFF
		എങ്കിൽ (curwin->w_topfill > 0)
		    --curwin->w_topfill;
		അല്ലെങ്കിൽ
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
	അല്ലെങ്കിൽ എങ്കിൽ (row == 0)
	{
	    // When dragging the mouse, while the text has been scrolled up as
	    // far as it goes, moving the mouse in the top line should scroll
	    // the text down (done later when recomputing w_topline).
	    എങ്കിൽ (mouse_dragging > 0
		    && curwin->w_cursor.lnum
				       == curwin->w_buffer->b_ml.ml_വരി_എണ്ണം
		    && curwin->w_cursor.lnum == curwin->w_topline)
		curwin->w_valid &= ~(VALID_TOPLINE);
	}
    }

    എങ്കിൽ (prev_row >= W_WINROW(curwin)
	&& prev_row < W_WINROW(curwin) + curwin->w_height
	&& prev_col >= curwin->w_wincol && prev_col < W_ENDCOL(curwin)
						       && Screen_വരികൾ != ശൂന്യ)
    {
	പൂർണ്ണസംഖ്യ off = വരി_Offset[prev_row] + prev_col;

	// Only use ScreenCols[] after the window was redrawn.  Mainly matters
	// for tests, a user would not click before redrawing.
	// Do not use when 'virtualedit' is active.
	എങ്കിൽ (curwin->w_redr_type <= UPD_VALID_NO_UPDATE)
	    col_from_screen = ScreenCols[off];
#ifdef FEAT_FOLDING
	// Remember the character under the mouse, it might be a '-' or '+' in
	// the fold column.
	mouse_char = enc_utf8 && Screen_വരികൾ_UC[off] != 0
				       ? Screen_വരികൾ_UC[off] : Screen_വരികൾ[off];
#endif
    }

#ifdef FEAT_FOLDING
    // Check for position outside of the fold column.
    എങ്കിൽ (
# ifdef FEAT_RIGHTLEFT
	    curwin->w_p_rl ? col < curwin->w_width - curwin->w_p_fdc :
# endif
	    col >= curwin->w_p_fdc + (cmdwin_win != curwin ? 0 : 1)
       )
	mouse_char = ' ';
#endif

    // compute the position in the buffer line from the posn on the screen
    എങ്കിൽ (mouse_comp_pos(curwin, &row, &col, &curwin->w_cursor.lnum, ശൂന്യ))
	mouse_past_bottom = TRUE;

    // Start Visual mode before coladvance(), for when 'sel' != "old"
    എങ്കിൽ ((flags & MOUSE_MAY_VIS) && !VIsual_active)
    {
	check_visual_highlight();
	VIsual = old_cursor;
	VIsual_active = TRUE;
	VIsual_reselect = TRUE;
	// if 'selectmode' contains "mouse", start Select mode
	may_start_select('o');
	setmouse();
	എങ്കിൽ (p_smd && സന്ദേശം_silent == 0)
	    redraw_cmdline = TRUE;	// show visual mode later
    }

    എങ്കിൽ (col_from_screen >= 0)
    {
	// Use the virtual column from ScreenCols[], it is accurate also after
	// concealed characters.
	col = col_from_screen;
    }

    curwin->w_curswant = col;
    curwin->w_set_curswant = false;	// May still have been TRUE
    എങ്കിൽ (coladvance(col) == FAIL)	// Mouse click beyond end of line
    {
	എങ്കിൽ (inclusive != ശൂന്യ)
	    *inclusive = TRUE;
	mouse_past_eol = TRUE;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (inclusive != ശൂന്യ)
	*inclusive = FALSE;

    എണ്ണം = IN_BUFFER;
    എങ്കിൽ (curwin != old_curwin || curwin->w_cursor.lnum != old_cursor.lnum
	    || curwin->w_cursor.col != old_cursor.col)
	എണ്ണം |= CURSOR_MOVED;		// Cursor has moved

#ifdef FEAT_FOLDING
    എങ്കിൽ (mouse_char == curwin->w_fill_chars.foldclosed)
	എണ്ണം |= MOUSE_FOLD_OPEN;
    അല്ലെങ്കിൽ എങ്കിൽ (mouse_char != ' ')
	എണ്ണം |= MOUSE_FOLD_CLOSE;
#endif

    തിരികെ എണ്ണം;
}

/*
 * Make a horizontal scroll to "leftcol".
 * Return TRUE if the cursor moved, FALSE otherwise.
 */
    പൂർണ്ണസംഖ്യ
do_mousescroll_horiz(long_u leftcol)
{
    എങ്കിൽ (curwin->w_p_wrap)
	തിരികെ FALSE;  // no horizontal scrolling when wrapping

    എങ്കിൽ (curwin->w_leftcol == (colnr_T)leftcol)
	തിരികെ FALSE;  // already there

    // When the line of the cursor is too short, move the cursor to the
    // longest visible line.
    എങ്കിൽ (
#ifdef FEAT_GUI
	    (!gui.in_use || vim_strchr(p_go, GO_HORSCROLL) == ശൂന്യ) &&
#endif
		    !virtual_active()
	    && (long)leftcol > scroll_വരി_len(curwin->w_cursor.lnum))
    {
	curwin->w_cursor.lnum = ui_find_longest_lnum();
	curwin->w_cursor.col = 0;
    }

    തിരികെ set_leftcol((colnr_T)leftcol);
}

/*
 * Normal and Visual modes implementation for scrolling in direction
 * "cap->arg", which is one of the MSCR_ values.
 */
    ശൂന്യം
nv_mousescroll(cmdarg_T *cap)
{
    win_T   *old_curwin = curwin;

#ifdef FEAT_TABPANEL
    എങ്കിൽ (mouse_row >= 0 && mouse_col >= 0
	    && (cap->arg == MSCR_UP || cap->arg == MSCR_DOWN)
	    && mouse_on_tabpanel())
    {
	(ശൂന്യം)tabpanel_scroll(cap->arg == MSCR_UP ? 1 : -1,
		mouse_vert_step > 0 ? mouse_vert_step : 3);
	തിരികെ;
    }
#endif

    എങ്കിൽ (mouse_row >= 0 && mouse_col >= 0)
    {
	// Find the window at the mouse pointer coordinates.
	// NOTE: Must restore "curwin" to "old_curwin" before returning!
	പൂർണ്ണസംഖ്യ row = mouse_row;
	പൂർണ്ണസംഖ്യ col = mouse_col;
	curwin = mouse_find_win(&row, &col, FIND_POPUP);
	എങ്കിൽ (curwin == ശൂന്യ)
	{
	    curwin = old_curwin;
	    തിരികെ;
	}

#ifdef FEAT_PROP_POPUP
	എങ്കിൽ (WIN_IS_POPUP(curwin) && !curwin->w_has_scrollbar)
	{
	    // cannot scroll this popup window
	    curwin = old_curwin;
	    തിരികെ;
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
    ശൂന്യം
nv_mouse(cmdarg_T *cap)
{
    (ശൂന്യം)do_mouse(cap->oap, cap->cmdchar, BACKWARD, cap->count1, 0);
}

സ്ഥിതം പൂർണ്ണസംഖ്യ	held_button = MOUSE_RELEASE;

    ശൂന്യം
reset_held_button(ശൂന്യം)
{
    held_button = MOUSE_RELEASE;
}

/*
 * Check if typebuf 'tp' contains a terminal mouse code and returns the
 * modifiers found in typebuf in 'modifiers'.
 */
    പൂർണ്ണസംഖ്യ
check_termcode_mouse(
    char_u	*tp,
    പൂർണ്ണസംഖ്യ		*slen,
    char_u	*താക്കോൽ_name,
    char_u	*modifiers_start,
    പൂർണ്ണസംഖ്യ		idx,
    പൂർണ്ണസംഖ്യ		*modifiers)
{
    പൂർണ്ണസംഖ്യ		j;
    char_u	*p;
#എങ്കിൽ !defined(UNIX) || defined(FEAT_MOUSE_XTERM) || defined(FEAT_GUI) \
    || defined(FEAT_MOUSE_GPM) || defined(FEAT_SYSMOUSE)
    char_u	bytes[6];
    പൂർണ്ണസംഖ്യ		അക്കം_bytes;
#endif
    പൂർണ്ണസംഖ്യ		mouse_code = 0;	    // init for GCC
    പൂർണ്ണസംഖ്യ		is_click, is_drag;
    പൂർണ്ണസംഖ്യ		is_release, release_is_ambiguous;
    പൂർണ്ണസംഖ്യ		wheel_code = 0;
    പൂർണ്ണസംഖ്യ		current_button;
    സ്ഥിതം പൂർണ്ണസംഖ്യ	orig_അക്കം_clicks = 1;
    സ്ഥിതം പൂർണ്ണസംഖ്യ	orig_mouse_code = 0x0;
#ifdef CHECK_DOUBLE_CLICK
    സ്ഥിതം പൂർണ്ണസംഖ്യ	orig_mouse_col = 0;
    സ്ഥിതം പൂർണ്ണസംഖ്യ	orig_mouse_row = 0;
    സ്ഥിതം ഘടന timeval  orig_mouse_time = {0, 0};
    // time of previous mouse click
    ഘടന timeval  mouse_time;		// time of current mouse click
    long	timediff;		// elapsed time in msec
#endif

    is_click = is_drag = is_release = release_is_ambiguous = FALSE;

#എങ്കിൽ !defined(UNIX) || defined(FEAT_MOUSE_XTERM) || defined(FEAT_GUI) \
    || defined(FEAT_MOUSE_GPM) || defined(FEAT_SYSMOUSE)
    എങ്കിൽ (താക്കോൽ_name[0] == KS_MOUSE
# ifdef FEAT_MOUSE_GPM
	    || താക്കോൽ_name[0] == KS_GPM_MOUSE
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
	വേണ്ടി (;;)
	{
	    // For the GUI and for MS-Windows two bytes each are used for row
	    // and column.  Allows for more than 223 columns.
# എങ്കിൽ defined(FEAT_GUI) || defined(MSWIN)
	    എങ്കിൽ (TRUE
#  എങ്കിൽ defined(FEAT_GUI) && !defined(MSWIN)
		&& gui.in_use
#  endif
		)
	    {
		അക്കം_bytes = get_bytes_from_buf(tp + *slen, bytes, 5);
		എങ്കിൽ (അക്കം_bytes == -1)	// not enough coordinates
		    തിരികെ -1;
		mouse_code = bytes[0];
		mouse_col = 128 * (bytes[1] - ' ' - 1)
		    + bytes[2] - ' ' - 1;
		mouse_row = 128 * (bytes[3] - ' ' - 1)
		    + bytes[4] - ' ' - 1;
	    }
	    അല്ലെങ്കിൽ
# endif
	    {
		അക്കം_bytes = get_bytes_from_buf(tp + *slen, bytes, 3);
		എങ്കിൽ (അക്കം_bytes == -1)	// not enough coordinates
		    തിരികെ -1;
		mouse_code = bytes[0];
		mouse_col = bytes[1] - ' ' - 1;
		mouse_row = bytes[2] - ' ' - 1;
	    }
	    *slen += അക്കം_bytes;

	    // If the following bytes is also a mouse code and it has the same
	    // code, dump this one and get the next.  This makes dragging a
	    // whole lot faster.
# ifdef FEAT_GUI
	    എങ്കിൽ (gui.in_use)
		j = 3;
	    അല്ലെങ്കിൽ
# endif
		j = get_termcode_len(idx);
	    എങ്കിൽ (STRNCMP(tp, tp + *slen, (size_t)j) == 0
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
	    അല്ലെങ്കിൽ
		നിർത്തുക;
	}
    }

    എങ്കിൽ (താക്കോൽ_name[0] == KS_URXVT_MOUSE
	    || താക്കോൽ_name[0] == KS_SGR_MOUSE
	    || താക്കോൽ_name[0] == KS_SGR_MOUSE_RELEASE)
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
	എങ്കിൽ (p == ശൂന്യ)
	    തിരികെ -1;

	mouse_code = getdigits(&p);
	എങ്കിൽ (*p++ != ';')
	    തിരികെ -1;

	// when mouse reporting is SGR, add 32 to mouse code
	എങ്കിൽ (താക്കോൽ_name[0] == KS_SGR_MOUSE
		|| താക്കോൽ_name[0] == KS_SGR_MOUSE_RELEASE)
	    mouse_code += 32;

	mouse_col = getdigits(&p) - 1;
	എങ്കിൽ (*p++ != ';')
	    തിരികെ -1;

	mouse_row = getdigits(&p) - 1;

	// The modifiers were the mouse coordinates, not the modifier keys
	// (alt/shift/ctrl/meta) state.
	*modifiers = 0;
    }

    എങ്കിൽ (താക്കോൽ_name[0] == KS_SGR_MOUSE
	    || താക്കോൽ_name[0] == KS_SGR_MOUSE_RELEASE)
    {
	എങ്കിൽ (താക്കോൽ_name[0] == KS_SGR_MOUSE_RELEASE)
	{
	    is_release = TRUE;
	    // This is used below to set held_button.
	    mouse_code |= MOUSE_RELEASE;
	}
    }
    അല്ലെങ്കിൽ
    {
	release_is_ambiguous = TRUE;
	എങ്കിൽ ((mouse_code & MOUSE_RELEASE) == MOUSE_RELEASE)
	    is_release = TRUE;
    }

    എങ്കിൽ (താക്കോൽ_name[0] == KS_MOUSE
# ifdef FEAT_MOUSE_GPM
	    || താക്കോൽ_name[0] == KS_GPM_MOUSE
# endif
# ifdef FEAT_MOUSE_URXVT
	    || താക്കോൽ_name[0] == KS_URXVT_MOUSE
# endif
	    || താക്കോൽ_name[0] == KS_SGR_MOUSE
	    || താക്കോൽ_name[0] == KS_SGR_MOUSE_RELEASE)
    {
# എങ്കിൽ !defined(MSWIN)
	/*
	 * Handle old style mouse events.
	 * Recognize the xterm mouse wheel, but not in the GUI, the
	 * Linux console with GPM and the MS-DOS or Win32 console
	 * (multi-clicks use >= 0x60).
	 */
	എങ്കിൽ (mouse_code >= MOUSEWHEEL_താഴെയുള്ള && mouse_code < MOUSESIDEBUTTONS_താഴെയുള്ള
#  ifdef FEAT_GUI
		&& !gui.in_use
#  endif
#  ifdef FEAT_MOUSE_GPM
		&& താക്കോൽ_name[0] != KS_GPM_MOUSE
#  endif
	   )
	{
#  എങ്കിൽ defined(UNIX)
	    എങ്കിൽ (use_xterm_mouse() > 1 && mouse_code >= 0x80)
		// mouse-move event, using MOUSE_DRAG works
		mouse_code = MOUSE_DRAG;
	    അല്ലെങ്കിൽ
#  endif
		// Keep the mouse_code before it's changed, so that we
		// remember that it was a mouse wheel click.
		wheel_code = mouse_code;
	}
#  ifdef FEAT_MOUSE_XTERM
	അല്ലെങ്കിൽ എങ്കിൽ (held_button == MOUSE_RELEASE
#   ifdef FEAT_GUI
		&& !gui.in_use
#   endif
		&& (mouse_code == 0x23 || mouse_code == 0x24
		    || mouse_code == 0x40 || mouse_code == 0x41))
	{
	    // Apparently 0x23 and 0x24 are used by rxvt scroll wheel.
	    // And 0x40 and 0x41 are used by some xterm emulator.
	    wheel_code = mouse_code - (mouse_code >= 0x40 ? 0x40 : 0x23)
							      + MOUSEWHEEL_താഴെയുള്ള;
	}
#  endif

#  എങ്കിൽ defined(UNIX)
	അല്ലെങ്കിൽ എങ്കിൽ (use_xterm_mouse() > 1)
	{
	    എങ്കിൽ (mouse_code & MOUSE_DRAG_XTERM)
		mouse_code |= MOUSE_DRAG;
	}
#  endif
#  ifdef FEAT_XCLIPBOARD
	അല്ലെങ്കിൽ എങ്കിൽ (!(mouse_code & MOUSE_DRAG & ~MOUSE_CLICK_MASK))
	{
	    എങ്കിൽ (is_release)
		stop_xterm_trace();
	    അല്ലെങ്കിൽ
		start_xterm_trace(mouse_code);
	}
#  endif
# endif
    }
#endif // !UNIX || FEAT_MOUSE_XTERM
#ifdef FEAT_MOUSE_NET
    എങ്കിൽ (താക്കോൽ_name[0] == KS_NETTERM_MOUSE)
    {
	പൂർണ്ണസംഖ്യ mc, mr;

	// expect a rather limited sequence like: balancing {
	// \033}6,45\r
	// '6' is the row, 45 is the column
	p = tp + *slen;
	mr = getdigits(&p);
	എങ്കിൽ (*p++ != ',')
	    തിരികെ -1;
	mc = getdigits(&p);
	എങ്കിൽ (*p++ != '\r')
	    തിരികെ -1;

	mouse_col = mc - 1;
	mouse_row = mr - 1;
	mouse_code = MOUSE_ഇടത്;
	*slen += (പൂർണ്ണസംഖ്യ)(p - (tp + *slen));
    }
#endif	// FEAT_MOUSE_NET
#ifdef FEAT_MOUSE_JSB
    എങ്കിൽ (താക്കോൽ_name[0] == KS_JSBTERM_MOUSE)
    {
	പൂർണ്ണസംഖ്യ mult, val, iter, button, status;

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
	തിരഞ്ഞെടുപ്പ് (*p++)
	{
	    അവസ്ഥ 'L': button = 1; നിർത്തുക;
	    അവസ്ഥ '-': നിർത്തുക;
	    അവസ്ഥ 'x': നിർത്തുക; // ignore sequence
	    സ്ഥിരസ്ഥിതി:  തിരികെ -1; // Unknown Result
	}
	തിരഞ്ഞെടുപ്പ് (*p++)
	{
	    അവസ്ഥ 'M': button |= 2; നിർത്തുക;
	    അവസ്ഥ '-': നിർത്തുക;
	    അവസ്ഥ 'x': നിർത്തുക; // ignore sequence
	    സ്ഥിരസ്ഥിതി:  തിരികെ -1; // Unknown Result
	}
	തിരഞ്ഞെടുപ്പ് (*p++)
	{
	    അവസ്ഥ 'R': button |= 4; നിർത്തുക;
	    അവസ്ഥ '-': നിർത്തുക;
	    അവസ്ഥ 'x': നിർത്തുക; // ignore sequence
	    സ്ഥിരസ്ഥിതി:  തിരികെ -1; // Unknown Result
	}
	status = *p++;
	വേണ്ടി (val = 0, mult = 100, iter = 0; iter < 3; iter++,
		mult /= 10, p++)
	    എങ്കിൽ (*p >= '0' && *p <= '9')
		val += (*p - '0') * mult;
	    അല്ലെങ്കിൽ
		തിരികെ -1;
	mouse_col = val;
	വേണ്ടി (val = 0, mult = 100, iter = 0; iter < 3; iter++,
		mult /= 10, p++)
	    എങ്കിൽ (*p >= '0' && *p <= '9')
		val += (*p - '0') * mult;
	    അല്ലെങ്കിൽ
		തിരികെ -1;
	mouse_row = val;
	തിരഞ്ഞെടുപ്പ് (*p++)
	{
	    അവസ്ഥ 's': button |= 8; നിർത്തുക;  // SHIFT key Pressed
	    അവസ്ഥ '-': നിർത്തുക;  // Not Pressed
	    അവസ്ഥ 'x': നിർത്തുക;  // Not Reporting
	    സ്ഥിരസ്ഥിതി:  തിരികെ -1; // Unknown Result
	}
	തിരഞ്ഞെടുപ്പ് (*p++)
	{
	    അവസ്ഥ 'c': button |= 16; നിർത്തുക;  // CTRL key Pressed
	    അവസ്ഥ '-': നിർത്തുക;  // Not Pressed
	    അവസ്ഥ 'x': നിർത്തുക;  // Not Reporting
	    സ്ഥിരസ്ഥിതി:  തിരികെ -1; // Unknown Result
	}
	എങ്കിൽ (*p++ != '\033')
	    തിരികെ -1;
	എങ്കിൽ (*p++ != '\\')
	    തിരികെ -1;
	തിരഞ്ഞെടുപ്പ് (status)
	{
	    അവസ്ഥ 'D': // Double Click
	    അവസ്ഥ 'S': // Single Click
		എങ്കിൽ (button & 1) mouse_code |= MOUSE_ഇടത്;
		എങ്കിൽ (button & 2) mouse_code |= MOUSE_നടുവ്;
		എങ്കിൽ (button & 4) mouse_code |= MOUSE_വലത്;
		എങ്കിൽ (button & 8) mouse_code |= MOUSE_SHIFT;
		എങ്കിൽ (button & 16) mouse_code |= MOUSE_CTRL;
		നിർത്തുക;
	    അവസ്ഥ 'm': // Mouse move
		എങ്കിൽ (button & 1) mouse_code |= MOUSE_ഇടത്;
		എങ്കിൽ (button & 2) mouse_code |= MOUSE_നടുവ്;
		എങ്കിൽ (button & 4) mouse_code |= MOUSE_വലത്;
		എങ്കിൽ (button & 8) mouse_code |= MOUSE_SHIFT;
		എങ്കിൽ (button & 16) mouse_code |= MOUSE_CTRL;
		എങ്കിൽ ((button & 7) != 0)
		{
		    held_button = mouse_code;
		    mouse_code |= MOUSE_DRAG;
		}
		is_drag = TRUE;
		showmode();
		നിർത്തുക;
	    അവസ്ഥ 'd': // Button Down
		എങ്കിൽ (button & 1) mouse_code |= MOUSE_ഇടത്;
		എങ്കിൽ (button & 2) mouse_code |= MOUSE_നടുവ്;
		എങ്കിൽ (button & 4) mouse_code |= MOUSE_വലത്;
		എങ്കിൽ (button & 8) mouse_code |= MOUSE_SHIFT;
		എങ്കിൽ (button & 16) mouse_code |= MOUSE_CTRL;
		നിർത്തുക;
	    അവസ്ഥ 'u': // Button Up
		is_release = TRUE;
		എങ്കിൽ (button & 1)
		    mouse_code |= MOUSE_ഇടത്;
		എങ്കിൽ (button & 2)
		    mouse_code |= MOUSE_നടുവ്;
		എങ്കിൽ (button & 4)
		    mouse_code |= MOUSE_വലത്;
		എങ്കിൽ (button & 8)
		    mouse_code |= MOUSE_SHIFT;
		എങ്കിൽ (button & 16)
		    mouse_code |= MOUSE_CTRL;
		നിർത്തുക;
	    സ്ഥിരസ്ഥിതി: തിരികെ -1; // Unknown Result
	}

	*slen += (p - (tp + *slen));
    }
#endif // FEAT_MOUSE_JSB
#ifdef FEAT_MOUSE_DEC
    എങ്കിൽ (താക്കോൽ_name[0] == KS_DEC_MOUSE)
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
	പൂർണ്ണസംഖ്യ Pe, Pb, Pr, Pc;

	p = tp + *slen;

	// get event status
	Pe = getdigits(&p);
	എങ്കിൽ (*p++ != ';')
	    തിരികെ -1;

	// get button status
	Pb = getdigits(&p);
	എങ്കിൽ (*p++ != ';')
	    തിരികെ -1;

	// get row status
	Pr = getdigits(&p);
	എങ്കിൽ (*p++ != ';')
	    തിരികെ -1;

	// get column status
	Pc = getdigits(&p);

	// the page parameter is optional
	എങ്കിൽ (*p == ';')
	{
	    p++;
	    (ശൂന്യം)getdigits(&p);
	}
	എങ്കിൽ (*p++ != '&')
	    തിരികെ -1;
	എങ്കിൽ (*p++ != 'w')
	    തിരികെ -1;

	mouse_code = 0;
	തിരഞ്ഞെടുപ്പ് (Pe)
	{
	    അവസ്ഥ  0: തിരികെ -1; // position request while unavailable
	    അവസ്ഥ  1: // a response to a locator position request includes
		     //	the status of all buttons
		     Pb &= 7;   // mask off and ignore fourth button
		     എങ്കിൽ (Pb & 4)
			 mouse_code  = MOUSE_ഇടത്;
		     എങ്കിൽ (Pb & 2)
			 mouse_code  = MOUSE_നടുവ്;
		     എങ്കിൽ (Pb & 1)
			 mouse_code  = MOUSE_വലത്;
		     എങ്കിൽ (Pb)
		     {
			 held_button = mouse_code;
			 mouse_code |= MOUSE_DRAG;
			 WantQueryMouse = TRUE;
		     }
		     is_drag = TRUE;
		     showmode();
		     നിർത്തുക;
	    അവസ്ഥ  2: mouse_code = MOUSE_ഇടത്;
		     WantQueryMouse = TRUE;
		     നിർത്തുക;
	    അവസ്ഥ  3: mouse_code = MOUSE_ഇടത്;
		     is_release = TRUE;
		     നിർത്തുക;
	    അവസ്ഥ  4: mouse_code = MOUSE_നടുവ്;
		     WantQueryMouse = TRUE;
		     നിർത്തുക;
	    അവസ്ഥ  5: mouse_code = MOUSE_നടുവ്;
		     is_release = TRUE;
		     നിർത്തുക;
	    അവസ്ഥ  6: mouse_code = MOUSE_വലത്;
		     WantQueryMouse = TRUE;
		     നിർത്തുക;
	    അവസ്ഥ  7: mouse_code = MOUSE_വലത്;
		     is_release = TRUE;
		     നിർത്തുക;
	    അവസ്ഥ  8: തിരികെ -1; // fourth button down
	    അവസ്ഥ  9: തിരികെ -1; // fourth button up
	    അവസ്ഥ 10: തിരികെ -1; // mouse outside of filter rectangle
	    സ്ഥിരസ്ഥിതി: തിരികെ -1; // should never occur
	}

	mouse_col = Pc - 1;
	mouse_row = Pr - 1;

	*slen += (പൂർണ്ണസംഖ്യ)(p - (tp + *slen));
    }
#endif // FEAT_MOUSE_DEC
#ifdef FEAT_MOUSE_PTERM
    എങ്കിൽ (താക്കോൽ_name[0] == KS_PTERM_MOUSE)
    {
	പൂർണ്ണസംഖ്യ button, അക്കം_clicks, action;

	p = tp + *slen;

	action = getdigits(&p);
	എങ്കിൽ (*p++ != ';')
	    തിരികെ -1;

	mouse_row = getdigits(&p);
	എങ്കിൽ (*p++ != ';')
	    തിരികെ -1;
	mouse_col = getdigits(&p);
	എങ്കിൽ (*p++ != ';')
	    തിരികെ -1;

	button = getdigits(&p);
	mouse_code = 0;

	തിരഞ്ഞെടുപ്പ് (button)
	{
	    അവസ്ഥ 4: mouse_code = MOUSE_ഇടത്; നിർത്തുക;
	    അവസ്ഥ 1: mouse_code = MOUSE_വലത്; നിർത്തുക;
	    അവസ്ഥ 2: mouse_code = MOUSE_നടുവ്; നിർത്തുക;
	    സ്ഥിരസ്ഥിതി: തിരികെ -1;
	}

	തിരഞ്ഞെടുപ്പ് (action)
	{
	    അവസ്ഥ 31: // Initial press
		എങ്കിൽ (*p++ != ';')
		    തിരികെ -1;

		അക്കം_clicks = getdigits(&p); // Not used
		നിർത്തുക;

	    അവസ്ഥ 32: // Release
		is_release = TRUE;
		നിർത്തുക;

	    അവസ്ഥ 33: // Drag
		held_button = mouse_code;
		mouse_code |= MOUSE_DRAG;
		നിർത്തുക;

	    സ്ഥിരസ്ഥിതി:
		തിരികെ -1;
	}

	എങ്കിൽ (*p++ != 't')
	    തിരികെ -1;

	*slen += (p - (tp + *slen));
    }
#endif // FEAT_MOUSE_PTERM

    // Interpret the mouse code
    current_button = (mouse_code & MOUSE_CLICK_MASK);
    എങ്കിൽ (is_release)
	current_button |= MOUSE_RELEASE;

    എങ്കിൽ (current_button == MOUSE_RELEASE
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
	എങ്കിൽ ((mouse_code & MOUSE_DRAG) == MOUSE_DRAG)
	    is_drag = TRUE;
	current_button = held_button;
    }
    അല്ലെങ്കിൽ
    {
      എങ്കിൽ (wheel_code == 0)
      {
#ifdef CHECK_DOUBLE_CLICK
# ifdef FEAT_MOUSE_GPM
	/*
	 * Only for Unix, when GUI not active, we handle multi-clicks here, but
	 * not for GPM mouse events.
	 */
#  ifdef FEAT_GUI
	എങ്കിൽ (താക്കോൽ_name[0] != KS_GPM_MOUSE && !gui.in_use)
#  അല്ലെങ്കിൽ
	    എങ്കിൽ (താക്കോൽ_name[0] != KS_GPM_MOUSE)
#  endif
# അല്ലെങ്കിൽ
#  ifdef FEAT_GUI
		എങ്കിൽ (!gui.in_use)
#  endif
# endif
		{
		    /*
		     * Compute the time elapsed since the previous mouse click.
		     */
		    gettimeofday(&mouse_time, ശൂന്യ);
		    എങ്കിൽ (orig_mouse_time.tv_sec == 0)
		    {
			/*
			 * Avoid computing the difference between mouse_time
			 * and orig_mouse_time for the first click, as the
			 * difference would be huge and would cause
			 * multiplication overflow.
			 */
			timediff = p_mouset;
		    }
		    അല്ലെങ്കിൽ
			timediff = time_diff_ms(&orig_mouse_time, &mouse_time);
		    orig_mouse_time = mouse_time;
		    എങ്കിൽ (mouse_code == orig_mouse_code
			    && timediff < p_mouset
			    && orig_അക്കം_clicks != 4
			    && orig_mouse_col == mouse_col
			    && orig_mouse_row == mouse_row
			    && (is_mouse_topline(curwin)
				// Double click in tab pages line also works
				// when window contents changes.
				|| (mouse_row == 0 && firstwin->w_winrow > 0))
		       )
			++orig_അക്കം_clicks;
		    അല്ലെങ്കിൽ
			orig_അക്കം_clicks = 1;
		    orig_mouse_col = mouse_col;
		    orig_mouse_row = mouse_row;
		    set_mouse_topline(curwin);
		}
# എങ്കിൽ defined(FEAT_GUI) || defined(FEAT_MOUSE_GPM)
		അല്ലെങ്കിൽ
		    orig_അക്കം_clicks = അക്കം_MOUSE_CLICKS(mouse_code);
# endif
#അല്ലെങ്കിൽ
	orig_അക്കം_clicks = അക്കം_MOUSE_CLICKS(mouse_code);
#endif
	is_click = TRUE;
      }
      orig_mouse_code = mouse_code;
    }
    എങ്കിൽ (!is_drag)
	held_button = mouse_code & MOUSE_CLICK_MASK;

    /*
     * Translate the actual mouse event into a pseudo mouse event.
     * First work out what modifiers are to be used.
     */
    എങ്കിൽ (orig_mouse_code & MOUSE_SHIFT)
	*modifiers |= MOD_MASK_SHIFT;
    എങ്കിൽ (orig_mouse_code & MOUSE_CTRL)
	*modifiers |= MOD_MASK_CTRL;
    എങ്കിൽ (orig_mouse_code & MOUSE_ALT)
	*modifiers |= MOD_MASK_ALT;
    എങ്കിൽ (orig_അക്കം_clicks == 2)
	*modifiers |= MOD_MASK_2CLICK;
    അല്ലെങ്കിൽ എങ്കിൽ (orig_അക്കം_clicks == 3)
	*modifiers |= MOD_MASK_3CLICK;
    അല്ലെങ്കിൽ എങ്കിൽ (orig_അക്കം_clicks == 4)
	*modifiers |= MOD_MASK_4CLICK;

    // Work out our pseudo mouse event. Note that MOUSE_RELEASE gets added,
    // then it's not mouse up/down.
    താക്കോൽ_name[0] = KS_EXTRA;
    എങ്കിൽ (wheel_code != 0 && (!is_release || release_is_ambiguous))
    {
	എങ്കിൽ (wheel_code & MOUSE_CTRL)
	    *modifiers |= MOD_MASK_CTRL;
	എങ്കിൽ (wheel_code & MOUSE_ALT)
	    *modifiers |= MOD_MASK_ALT;

	എങ്കിൽ (wheel_code & 1 && wheel_code & 2)
	    താക്കോൽ_name[1] = (പൂർണ്ണസംഖ്യ)KE_MOUSELEFT;
	അല്ലെങ്കിൽ എങ്കിൽ (wheel_code & 2)
	    താക്കോൽ_name[1] = (പൂർണ്ണസംഖ്യ)KE_MOUSERIGHT;
	അല്ലെങ്കിൽ എങ്കിൽ (wheel_code & 1)
	    താക്കോൽ_name[1] = (പൂർണ്ണസംഖ്യ)KE_MOUSEUP;
	അല്ലെങ്കിൽ
	    താക്കോൽ_name[1] = (പൂർണ്ണസംഖ്യ)KE_MOUSEDOWN;

	held_button = MOUSE_RELEASE;
    }
    അല്ലെങ്കിൽ
    {
#എങ്കിൽ defined(UNIX)
	എങ്കിൽ (use_xterm_mouse() && orig_mouse_code >= MOUSESIDEBUTTONS_താഴെയുള്ള)
	    current_button = (current_button) ? MOUSE_X2 : MOUSE_X1;
#endif
	താക്കോൽ_name[1] = get_pseudo_mouse_code(current_button, is_click, is_drag);
    }


    // Make sure the mouse position is valid.  Some terminals may return weird
    // values.
    എങ്കിൽ (mouse_col >= Columns)
	mouse_col = Columns - 1;
    എങ്കിൽ (mouse_row >= Rows)
	mouse_row = Rows - 1;

    തിരികെ 0;
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
    പൂർണ്ണസംഖ്യ
mouse_comp_pos(
    win_T	*win,
    പൂർണ്ണസംഖ്യ		*rowp,
    പൂർണ്ണസംഖ്യ		*colp,
    linenr_T	*lnump,
    പൂർണ്ണസംഖ്യ		*plines_cache)
{
    പൂർണ്ണസംഖ്യ		col = *colp;
    പൂർണ്ണസംഖ്യ		row = *rowp;
    linenr_T	lnum;
    പൂർണ്ണസംഖ്യ		retval = FALSE;
    പൂർണ്ണസംഖ്യ		off;
    പൂർണ്ണസംഖ്യ		എണ്ണം;

#ifdef FEAT_RIGHTLEFT
    എങ്കിൽ (win->w_p_rl)
	col = win->w_width - 1 - col;
#endif

    lnum = win->w_topline;

    ആകുമ്പോൾ (row > 0)
    {
	പൂർണ്ണസംഖ്യ cache_idx = lnum - win->w_topline;

	// Only "Rows" lines are cached, with folding we'll run out of entries
	// and use the slow way.
	എങ്കിൽ (plines_cache != ശൂന്യ && cache_idx < Rows
						&& plines_cache[cache_idx] > 0)
	    എണ്ണം = plines_cache[cache_idx];
	അല്ലെങ്കിൽ
	{
#ifdef FEAT_DIFF
	    // Don't include filler lines in "count"
	    എങ്കിൽ (win->w_p_diff
# ifdef FEAT_FOLDING
		    && !hasFoldingWin(win, lnum, ശൂന്യ, ശൂന്യ, TRUE, ശൂന്യ)
# endif
		    )
	    {
		എങ്കിൽ (lnum == win->w_topline)
		    row -= win->w_topfill;
		അല്ലെങ്കിൽ
		    row -= diff_check_fill(win, lnum);
		എണ്ണം = plines_win_nofill(win, lnum, FALSE);
	    }
	    അല്ലെങ്കിൽ
#endif
		എണ്ണം = plines_win(win, lnum, FALSE);
	    എങ്കിൽ (plines_cache != ശൂന്യ && cache_idx < Rows)
		plines_cache[cache_idx] = എണ്ണം;
	}

	എങ്കിൽ (win->w_skipcol > 0 && lnum == win->w_topline)
	{
	    പൂർണ്ണസംഖ്യ width1 = win->w_width - win_col_off(win);

	    എങ്കിൽ (width1 > 0)
	    {
		പൂർണ്ണസംഖ്യ skip_വരികൾ = 0;

		// Adjust for 'smoothscroll' clipping the top screen lines.
		// A similar formula is used in curs_columns().
		എങ്കിൽ (win->w_skipcol > width1)
		    skip_വരികൾ = (win->w_skipcol - width1)
					    / (width1 + win_col_off2(win)) + 1;
		അല്ലെങ്കിൽ എങ്കിൽ (win->w_skipcol > 0)
		    skip_വരികൾ = 1;

		എണ്ണം -= skip_വരികൾ;
	    }
	}

	എങ്കിൽ (എണ്ണം > row)
	    നിർത്തുക;	// Position is in this buffer line.
#ifdef FEAT_FOLDING
	(ശൂന്യം)hasFoldingWin(win, lnum, ശൂന്യ, &lnum, TRUE, ശൂന്യ);
#endif
	എങ്കിൽ (lnum == win->w_buffer->b_ml.ml_വരി_എണ്ണം)
	{
	    retval = TRUE;
	    നിർത്തുക;		// past end of file
	}
	row -= എണ്ണം;
	++lnum;
    }

    എങ്കിൽ (!retval)
    {
	// Compute the column without wrapping.
	off = win_col_off(win) - win_col_off2(win);
	എങ്കിൽ (col < off)
	    col = off;
	col += row * (win->w_width - off);

	// Add skip column for the topline.
	എങ്കിൽ (lnum == win->w_topline)
	    col += win->w_skipcol;
    }

    എങ്കിൽ (!win->w_p_wrap)
	col += win->w_leftcol;

    // skip line number and fold column in front of the line
    col -= win_col_off(win);
    എങ്കിൽ (col <= 0)
    {
#ifdef FEAT_NETBEANS_INTG
	// if mouse is clicked on the gutter, then inform the netbeans server
	എങ്കിൽ (*colp < win_col_off(win))
	    netbeans_gutter_click(lnum);
#endif
	col = 0;
    }

    *colp = col;
    *rowp = row;
    *lnump = lnum;
    തിരികെ retval;
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
mouse_find_win(പൂർണ്ണസംഖ്യ *rowp, പൂർണ്ണസംഖ്യ *colp, mouse_find_T popup UNUSED)
{
    frame_T	*fp;
    win_T	*wp;

#ifdef FEAT_PROP_POPUP
    win_T	*pwp = ശൂന്യ;

    എങ്കിൽ (popup != IGNORE_POPUP)
    {
	popup_reset_handled(POPUP_HANDLED_1);
	ആകുമ്പോൾ ((wp = find_next_popup(TRUE, POPUP_HANDLED_1)) != ശൂന്യ)
	{
	    എങ്കിൽ (*rowp >= wp->w_winrow && *rowp < wp->w_winrow + popup_height(wp)
		    && *colp >= wp->w_wincol
				    && *colp < wp->w_wincol + popup_width(wp))
		pwp = wp;
	}
	എങ്കിൽ (pwp != ശൂന്യ)
	{
	    എങ്കിൽ (popup == FAIL_POPUP)
		തിരികെ ശൂന്യ;
	    *rowp -= pwp->w_winrow;
	    *colp -= pwp->w_wincol;
	    തിരികെ pwp;
	}
    }
#endif

    fp = topframe;

    എങ്കിൽ (*colp < firstwin->w_wincol
	    || *colp >= firstwin->w_wincol + fp->fr_width
	    || *rowp < firstwin->w_winrow)
	തിരികെ ശൂന്യ;

    *rowp -= firstwin->w_winrow;
    *colp -= firstwin->w_wincol;
    വേണ്ടി (;;)
    {
	എങ്കിൽ (fp->fr_layout == FR_LEAF)
	    നിർത്തുക;
	എങ്കിൽ (fp->fr_layout == FR_ROW)
	{
	    വേണ്ടി (fp = fp->fr_child; fp->fr_next != ശൂന്യ; fp = fp->fr_next)
	    {
		എങ്കിൽ (*colp < fp->fr_width)
		    നിർത്തുക;
		*colp -= fp->fr_width;
	    }
	}
	അല്ലെങ്കിൽ    // fr_layout == FR_COL
	{
	    വേണ്ടി (fp = fp->fr_child; fp->fr_next != ശൂന്യ; fp = fp->fr_next)
	    {
		എങ്കിൽ (*rowp < fp->fr_height)
		    നിർത്തുക;
		*rowp -= fp->fr_height;
	    }
	}
    }
    // When using a timer that closes a window the window might not actually
    // exist.
    FOR_ALL_WINDOWS(wp)
	എങ്കിൽ (wp == fp->fr_win)
	{
#ifdef FEAT_MENU
	    *rowp -= wp->w_winbar_height;
#endif
	    തിരികെ wp;
	}
    തിരികെ ശൂന്യ;
}

#എങ്കിൽ defined(NEED_VCOL2COL) || defined(FEAT_BEVAL) || defined(FEAT_PROP_POPUP) \
	|| defined(FEAT_EVAL)
/*
 * Convert a virtual (screen) column to a character column.
 * The first column is zero.
 */
    പൂർണ്ണസംഖ്യ
vcol2col(win_T *wp, linenr_T lnum, പൂർണ്ണസംഖ്യ vcol, colnr_T *coladdp)
{
    char_u	    *വരി;
    chartabsize_T   cts;

    // try to advance to the specified column
    വരി = ml_get_buf(wp->w_buffer, lnum, FALSE);
    init_chartabsize_arg(&cts, wp, lnum, 0, വരി, വരി);
    ആകുമ്പോൾ (cts.cts_vcol < vcol && *cts.cts_ptr != NUL)
    {
	പൂർണ്ണസംഖ്യ size = win_lbr_chartabsize(&cts, ശൂന്യ, ശൂന്യ);
	എങ്കിൽ (cts.cts_vcol + size > vcol)
	    നിർത്തുക;
	cts.cts_vcol += size;
	MB_PTR_ADV(cts.cts_ptr);
    }
    clear_chartabsize_arg(&cts);

    എങ്കിൽ (coladdp != ശൂന്യ)
	*coladdp = vcol - cts.cts_vcol;
    തിരികെ (പൂർണ്ണസംഖ്യ)(cts.cts_ptr - വരി);
}
#endif

#എങ്കിൽ defined(FEAT_EVAL)
/*
 * "getmousepos()" function.
 */
    ശൂന്യം
f_getmousepos(typval_T *argvars UNUSED, typval_T *rettv)
{
    dict_T	*d;
    win_T	*wp;
    പൂർണ്ണസംഖ്യ		row = mouse_row;
    പൂർണ്ണസംഖ്യ		col = mouse_col;
    varnumber_T winid = 0;
    varnumber_T winrow = 0;
    varnumber_T wincol = 0;
    linenr_T	lnum = 0;
    varnumber_T column = 0;
    colnr_T	coladd = 0;

    എങ്കിൽ (rettv_dict_alloc(rettv) == FAIL)
	തിരികെ;
    d = rettv->vval.v_dict;

    dict_add_സംഖ്യ(d, "screenrow", (varnumber_T)mouse_row + 1);
    dict_add_സംഖ്യ(d, "screencol", (varnumber_T)mouse_col + 1);

    wp = mouse_find_win(&row, &col, FIND_POPUP);
    എങ്കിൽ (wp != ശൂന്യ)
    {
	പൂർണ്ണസംഖ്യ	മുകൾ_off = 0;
	പൂർണ്ണസംഖ്യ	ഇടത്_off = 0;
	പൂർണ്ണസംഖ്യ	height = wp->w_height + wp->w_status_height;

# ifdef FEAT_PROP_POPUP
	എങ്കിൽ (WIN_IS_POPUP(wp))
	{
	    മുകൾ_off = popup_മുകൾ_extra(wp);
	    ഇടത്_off = popup_ഇടത്_extra(wp);
	    height = popup_height(wp);
	}
# endif
	എങ്കിൽ (row < height)
	{
	    winid = wp->w_id;
	    winrow = row + 1;
	    wincol = col + 1;
	    row -= മുകൾ_off;
	    col -= ഇടത്_off;
	    എങ്കിൽ (row >= 0 && row < wp->w_height && col >= 0 && col < wp->w_width)
	    {
		(ശൂന്യം)mouse_comp_pos(wp, &row, &col, &lnum, ശൂന്യ);
		col = vcol2col(wp, lnum, col, &coladd);
		column = col + 1;
	    }
	}
    }
    dict_add_സംഖ്യ(d, "winid", winid);
    dict_add_സംഖ്യ(d, "winrow", winrow);
    dict_add_സംഖ്യ(d, "wincol", wincol);
    dict_add_സംഖ്യ(d, "line", (varnumber_T)lnum);
    dict_add_സംഖ്യ(d, "column", column);
    dict_add_സംഖ്യ(d, "coladd", coladd);
}
#endif
