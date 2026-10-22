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

#ଅନ୍ତର୍ଭାଵ "vim.h"

/*
 * Horiziontal and vertical steps used when scrolling.
 * When negative scroll by a whole page.
 */
ସ୍ଥିର long mouse_hor_step = 6;
ସ୍ଥିର long mouse_vert_step = 3;
ସ୍ଥିର win_T *dragwin = ଖାଲି;	// window being dragged
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା stl_click_handler(win_T *wp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା mrow, ପୂର୍ଣ୍ଣସଂଖ୍ୟା mcol, ପୂର୍ଣ୍ଣସଂଖ୍ୟା which_button,
								    ପୂର୍ଣ୍ଣସଂଖ୍ୟା mods);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା stl_click_handler_regions(stl_click_region_T *regions,
				    ପୂର୍ଣ୍ଣସଂଖ୍ୟା region_ଗଣନା, ପୂର୍ଣ୍ଣସଂଖ୍ୟା winid,
				    char_u *area_name, ପୂର୍ଣ୍ଣସଂଖ୍ୟା mrow, ପୂର୍ଣ୍ଣସଂଖ୍ୟା mcol,
				    ପୂର୍ଣ୍ଣସଂଖ୍ୟା which_button, ପୂର୍ଣ୍ଣସଂଖ୍ୟା mods);

    ଶୂନ୍ୟ
mouse_set_vert_scroll_step(long step)
{
    mouse_vert_step = step;
}

    ଶୂନ୍ୟ
mouse_set_hor_scroll_step(long step)
{
    mouse_hor_step = step;
}

#ifdef CHECK_DOUBLE_CLICK
/*
 * Return the duration from t1 to t2 in milliseconds.
 */
    ସ୍ଥିର long
time_diff_ms(ସଂରଚନା timeval *t1, ସଂରଚନା timeval *t2)
{
    // This handles wrapping of tv_usec correctly without any special case.
    // Example of 2 pairs (tv_sec, tv_usec) with a duration of 5 ms:
    //	   t1 = (1, 998000) t2 = (2, 3000) gives:
    //	   (2 - 1) * 1000 + (3000 - 998000) / 1000 -> 5 ms.
    ଫେରନ୍ତୁ (t2->tv_sec - t1->tv_sec) * 1000
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
get_mouse_class(char_u *p)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		c;

    ଯଦି (has_mbyte && MB_BYTE2LEN(p[0]) > 1)
	ଫେରନ୍ତୁ mb_get_class(p);

    c = *p;
    ଯଦି (c == ' ' || c == '\t')
	ଫେରନ୍ତୁ 0;

    ଯଦି (vim_iswordc(c))
	ଫେରନ୍ତୁ 2;

    // There are a few special cases where we want certain combinations of
    // characters to be considered as a single word.  These are things like
    // "->", "/ *", "*=", "+=", "&=", "<=", ">=", "!=" etc.  Otherwise, each
    // character is in its own class.
    ଯଦି (c != NUL && vim_strchr((char_u *)"-+*/%<>&|^!=", c) != ଖାଲି)
	ଫେରନ୍ତୁ 1;
    ଫେରନ୍ତୁ c;
}

/*
 * Move "pos" back to the start of the word it's in.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
find_start_of_word(pos_T *pos)
{
    char_u	*ଧାଡ଼ି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		cclass;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		col;

    ଧାଡ଼ି = ml_get(pos->lnum);
    cclass = get_mouse_class(ଧାଡ଼ି + pos->col);

    ଯେତେବେଳେ (pos->col > 0)
    {
	col = pos->col - 1;
	col -= (*mb_head_off)(ଧାଡ଼ି, ଧାଡ଼ି + col);
	ଯଦି (get_mouse_class(ଧାଡ଼ି + col) != cclass)
	    ଭାଙ୍ଗ;
	pos->col = col;
    }
}

/*
 * Move "pos" forward to the end of the word it's in.
 * When 'selection' is "exclusive", the position is just after the word.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
find_end_of_word(pos_T *pos)
{
    char_u	*ଧାଡ଼ି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		cclass;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		col;

    ଧାଡ଼ି = ml_get(pos->lnum);
    ଯଦି (*p_sel == 'e' && pos->col > 0)
    {
	--pos->col;
	pos->col -= (*mb_head_off)(ଧାଡ଼ି, ଧାଡ଼ି + pos->col);
    }
    cclass = get_mouse_class(ଧାଡ଼ି + pos->col);
    ଯେତେବେଳେ (ଧାଡ଼ି[pos->col] != NUL)
    {
	col = pos->col + (*mb_ptr2len)(ଧାଡ଼ି + pos->col);
	ଯଦି (get_mouse_class(ଧାଡ଼ି + col) != cclass)
	{
	    ଯଦି (*p_sel == 'e')
		pos->col = col;
	    ଭାଙ୍ଗ;
	}
	pos->col = col;
    }
}

#ଯଦି defined(FEAT_GUI_MOTIF) || defined(FEAT_GUI_GTK) \
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
get_fpos_of_mouse(pos_T *mpos)
{
    win_T	*wp;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		row = mouse_row;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		col = mouse_col;

    ଯଦି (row < 0 || col < 0)		// check if it makes sense
	ଫେରନ୍ତୁ IN_UNKNOWN;

    // find the window where the row is in
    wp = mouse_find_win(&row, &col, FAIL_POPUP);
    ଯଦି (wp == ଖାଲି)
	ଫେରନ୍ତୁ IN_UNKNOWN;
    // winpos and height may change in win_enter()!
    ଯଦି (row >= wp->w_height)	// In (or below) status line
	ଫେରନ୍ତୁ IN_STATUS_ଧାଡ଼ି;
    ଯଦି (col >= wp->w_width)	// In vertical separator line
	ଫେରନ୍ତୁ IN_SEP_ଧାଡ଼ି;

    ଯଦି (wp != curwin)
	ଫେରନ୍ତୁ IN_UNKNOWN;

    // compute the position in the buffer line from the posn on the screen
    ଯଦି (mouse_comp_pos(curwin, &row, &col, &mpos->lnum, ଖାଲି))
	ଫେରନ୍ତୁ IN_STATUS_ଧାଡ଼ି; // past bottom

    mpos->col = vcol2col(wp, mpos->lnum, col, &mpos->coladd);
    ଫେରନ୍ତୁ IN_BUFFER;
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
do_mouse(
    oparg_T	*oap,		// operator argument, can be NULL
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		c,		// K_LEFTMOUSE, etc
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		dir,		// Direction to 'put' if necessary
    long	ଗଣନା_2,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		fixindent)	// PUT_FIXINDENT if fixing indent necessary
{
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	do_always = FALSE;	// ignore 'mouse' setting next time
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	got_click = FALSE;	// got a click some time back

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		which_button;	// MOUSE_LEFT, _MIDDLE or _RIGHT
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		is_click = FALSE; // If FALSE it's a drag or release event
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		is_drag = FALSE;  // If TRUE it's a drag event
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		jump_flags = 0;	// flags for jump_to_mouse()
    pos_T	start_visual;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		moved;		// Has cursor moved?
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		in_status_ଧାଡ଼ି;	// mouse in status line
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	in_tab_ଧାଡ଼ି = FALSE; // mouse clicked in tab line
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	in_tabpanel = FALSE; // mouse clicked in tabpanel
#ifdef FEAT_TABPANEL
    ସ୍ଥିର bool	in_tabpanel_scrollbar = false; // dragging tabpanel scrollbar
#endif
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		in_sep_ଧାଡ଼ି;	// mouse in vertical separator line
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		c1, c2;
#ଯଦି defined(FEAT_FOLDING)
    pos_T	save_cursor;
#endif
    win_T	*old_curwin = curwin;
    ସ୍ଥିର pos_T orig_cursor;
    colnr_T	leftcol, rightcol;
    pos_T	end_visual;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		diff;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		old_active = VIsual_active;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		old_mode = VIsual_mode;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		regname;

#ଯଦି defined(FEAT_FOLDING)
    save_cursor = curwin->w_cursor;
#endif

    // When GUI is active, always recognize mouse events, otherwise:
    // - Ignore mouse event in normal mode if 'mouse' doesn't include 'n'.
    // - Ignore mouse event in visual mode if 'mouse' doesn't include 'v'.
    // - For command line and insert mode 'mouse' is checked before calling
    //	 do_mouse().
    ଯଦି (do_always)
	do_always = FALSE;
    ଅନ୍ୟଥା
#ifdef FEAT_GUI
	ଯଦି (!gui.in_use)
#endif
	{
	    ଯଦି (VIsual_active)
	    {
		ଯଦି (!mouse_has(MOUSE_VISUAL))
		    ଫେରନ୍ତୁ FALSE;
	    }
	    ଅନ୍ୟଥା ଯଦି (State == MODE_NORMAL && !mouse_has(MOUSE_NORMAL))
		ଫେରନ୍ତୁ FALSE;
	}

    ପାଇଁ (;;)
    {
	which_button = get_mouse_button(KEY2TERMCAP1(c), &is_click, &is_drag);
	ଯଦି (is_drag)
	{
	    // If the next character is the same mouse event then use that
	    // one. Speeds up dragging the status line.
	    // Note: Since characters added to the stuff buffer in the code
	    // below need to come before the next character, do not do this
	    // when the current character was stuffed.
	    ଯଦି (!ଚାବି_Stuffed && vpeekc() != NUL)
	    {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା nc;
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା save_mouse_row = mouse_row;
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା save_mouse_col = mouse_col;

		// Need to get the character, peeking doesn't get the actual
		// one.
		nc = safe_vgetc();
		ଯଦି (c == nc)
		    ଜାରି;
		vungetc(nc);
		mouse_row = save_mouse_row;
		mouse_col = save_mouse_col;
	    }
	}
	ଭାଙ୍ଗ;
    }

    ଯଦି (c == K_MOUSEMOVE)
    {
	// Mouse moved without a button pressed.
#ifdef FEAT_BEVAL_TERM
	ui_may_remove_balloon();
	ଯଦି (p_bevalterm)
	{
	    profile_setlimit(p_bdlay, &bevalexpr_due);
	    bevalexpr_due_set = TRUE;
	}
#endif
#ifdef FEAT_PROP_POPUP
	popup_handle_mouse_moved();
#endif
	ଫେରନ୍ତୁ FALSE;
    }

#ifdef FEAT_MOUSESHAPE
    // May have stopped dragging the status or separator line.  The pointer is
    // most likely still on the status or separator line.
    ଯଦି (!is_drag && drag_status_ଧାଡ଼ି)
    {
	drag_status_ଧାଡ଼ି = FALSE;
	update_mouseshape(SHAPE_IDX_STATUS);
    }
    ଯଦି (!is_drag && drag_sep_ଧାଡ଼ି)
    {
	drag_sep_ଧାଡ଼ି = FALSE;
	update_mouseshape(SHAPE_IDX_VSEP);
    }
#endif

    // Ignore drag and release events if we didn't get a click.
    ଯଦି (is_click)
    {
	got_click = TRUE;
	in_tab_ଧାଡ଼ି = FALSE;
	in_tabpanel = FALSE;
#ifdef FEAT_TABPANEL
	in_tabpanel_scrollbar = false;
#endif
    }
    ଅନ୍ୟଥା
    {
	ଯଦି (!got_click)			// didn't get click, ignore
	    ଫେରନ୍ତୁ FALSE;
	ଯଦି (!is_drag)			// release, reset got_click
	{
	    got_click = FALSE;
	    ଯଦି (in_tab_ଧାଡ଼ି || in_tabpanel
#ifdef FEAT_TABPANEL
		    || in_tabpanel_scrollbar
#endif
		    )
	    {
		in_tab_ଧାଡ଼ି = FALSE;
		in_tabpanel = FALSE;
#ifdef FEAT_TABPANEL
		in_tabpanel_scrollbar = false;
#endif
		ଫେରନ୍ତୁ FALSE;
	    }
	}
    }

#ifdef FEAT_TABPANEL
    // Continue a scrollbar drag before any tab-selection handling.
    ଯଦି (is_drag && in_tabpanel_scrollbar)
    {
	tabpanel_drag_scrollbar(mouse_row);
	ଫେରନ୍ତୁ FALSE;
    }
#endif

    // CTRL right mouse button does CTRL-T
    ଯଦି (is_click && (mod_mask & MOD_MASK_CTRL) && which_button == MOUSE_ଡାହାଣ)
    {
	ଯଦି (State & MODE_ଭର୍ତ୍ତି)
	    stuffcharReadbuff(Ctrl_O);
	ଯଦି (ଗଣନା_2 > 1)
	    stuffnumReadbuff(ଗଣନା_2);
	stuffcharReadbuff(Ctrl_T);
	got_click = FALSE;		// ignore drag&release now
	ଫେରନ୍ତୁ FALSE;
    }

    // CTRL only works with left mouse button
    ଯଦି ((mod_mask & MOD_MASK_CTRL) && which_button != MOUSE_ବାମ)
	ଫେରନ୍ତୁ FALSE;

    // When a modifier is down, ignore drag and release events, as well as
    // multiple clicks and the middle mouse button.
    // Accept shift-leftmouse drags when 'mousemodel' is "popup.*".
    ଯଦି ((mod_mask & (MOD_MASK_SHIFT | MOD_MASK_CTRL | MOD_MASK_ALT
							     | MOD_MASK_META))
	    && (!is_click
		|| (mod_mask & MOD_MASK_MULTI_CLICK)
		|| which_button == MOUSE_ମଧ୍ୟ)
	    && !((mod_mask & (MOD_MASK_SHIFT|MOD_MASK_ALT))
		&& mouse_model_popup()
		&& which_button == MOUSE_ବାମ)
	    && !((mod_mask & MOD_MASK_ALT)
		&& !mouse_model_popup()
		&& which_button == MOUSE_ଡାହାଣ)
	    )
	ଫେରନ୍ତୁ FALSE;

    // If the button press was used as the movement command for an operator
    // (eg "d<MOUSE>"), or it is the middle button that is held down, ignore
    // drag/release events.
    ଯଦି (!is_click && which_button == MOUSE_ମଧ୍ୟ)
	ଫେରନ୍ତୁ FALSE;

    ଯଦି (oap != ଖାଲି)
	regname = oap->regname;
    ଅନ୍ୟଥା
	regname = 0;

    // Middle mouse button does a 'put' of the selected text
    ଯଦି (which_button == MOUSE_ମଧ୍ୟ)
    {
	ଯଦି (State == MODE_NORMAL)
	{
	    // If an operator was pending, we don't know what the user wanted
	    // to do. Go back to normal mode: Clear the operator and beep().
	    ଯଦି (oap != ଖାଲି && oap->op_type != OP_NOP)
	    {
		clearopbeep(oap);
		ଫେରନ୍ତୁ FALSE;
	    }

	    // If visual was active, yank the highlighted text and put it
	    // before the mouse pointer position.
	    // In Select mode replace the highlighted text with the clipboard.
	    ଯଦି (VIsual_active)
	    {
		ଯଦି (VIsual_select)
		{
		    stuffcharReadbuff(Ctrl_G);
		    stuffReadbuff((char_u *)"\"+p");
		}
		ଅନ୍ୟଥା
		{
		    stuffcharReadbuff('y');
		    stuffcharReadbuff(K_MIDDLEMOUSE);
		}
		do_always = TRUE;	// ignore 'mouse' setting next time
		ଫେରନ୍ତୁ FALSE;
	    }
	    // The rest is below jump_to_mouse()
	}

	ଅନ୍ୟଥା ଯଦି ((State & MODE_ଭର୍ତ୍ତି) == 0)
	    ଫେରନ୍ତୁ FALSE;

	// Middle click in insert mode doesn't move the mouse, just insert the
	// contents of a register.  '.' register is special, can't insert that
	// with do_put().
	// Also paste at the cursor if the current mode isn't in 'mouse' (only
	// happens for the GUI).
	ଯଦି ((State & MODE_ଭର୍ତ୍ତି) || !mouse_has(MOUSE_NORMAL))
	{
	    ଯଦି (regname == '.')
		ଭର୍ତ୍ତି_reg(regname, TRUE);
	    ଅନ୍ୟଥା
	    {
#ifdef FEAT_CLIPBOARD
		ଯଦି (clip_star.available && regname == 0)
		    regname = '*';
#endif
		ଯଦି ((State & REPLACE_FLAG) && !yank_register_mline(regname))
		    ଭର୍ତ୍ତି_reg(regname, TRUE);
		ଅନ୍ୟଥା
		{
		    do_put(regname, ଖାଲି, BACKWARD, 1L,
						      fixindent | PUT_CURSEND);

		    // Repeat it with CTRL-R CTRL-O r or CTRL-R CTRL-P r
		    AppendCharToRedobuff(Ctrl_R);
		    AppendCharToRedobuff(fixindent ? Ctrl_P : Ctrl_O);
		    AppendCharToRedobuff(regname == 0 ? '"' : regname);
		}
	    }
	    ଫେରନ୍ତୁ FALSE;
	}
    }

    // When dragging or button-up stay in the same window.
    ଯଦି (!is_click)
	jump_flags |= MOUSE_FOCUS | MOUSE_DID_MOVE;

    start_visual.lnum = 0;

    ସଂରଚନା tabpage_label_info {
	bool is_panel;	    // label type. true: tabpanel, false: tab line
	bool just_in;	    // just in tabpage label area
	bool just_click;    // just click tabpage label area
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା nr;		    // tabpage number
    } tp_label = { false, false, false, 0 };

    // Check for clicking in the tab page panel.
#ଯଦି defined(FEAT_TABPANEL)
    ଯଦି (mouse_col < firstwin->w_wincol
		|| mouse_col >= firstwin->w_wincol + topframe->fr_width)
    {
	// A click on the scrollbar column starts a drag interaction and
	// preempts tab-selection.
	ଯଦି (is_click && !is_drag && mouse_on_tabpanel_scrollbar())
	{
	    in_tabpanel_scrollbar = TRUE;
	    tabpanel_drag_scrollbar(mouse_row);
	    ଫେରନ୍ତୁ FALSE;
	}

	// Dispatch 'tabpanel' %[FuncName] click regions before falling through
	// to tab-page selection.  On drag events fall through to the normal
	// tab-drag handling.
	ଯଦି (is_click && !is_drag
		&& stl_click_handler_regions(tabpanel_stl_click,
					    tabpanel_stl_click_ଗଣନା,
					    0, (char_u *)"tabpanel",
					    mouse_row, mouse_col,
					    which_button, mod_mask))
	    ଫେରନ୍ତୁ FALSE;

	tp_label.is_panel = true;
	tp_label.just_in = true;
	tp_label.nr = get_tabpagenr_on_tabpanel();

	// click in a tab selects that tab page
	ଯଦି (is_click && cmdwin_type == 0)
	    tp_label.just_click = true;
    }
    ଅନ୍ୟଥା
#endif
    // Check for clicking in the tab page line.
    ଯଦି (TabPageIdxs != ଖାଲି && mouse_row == 0 && firstwin->w_winrow > 0)
    {
	// Dispatch 'tabline' %[FuncName] click regions before falling through
	// to tab-page selection.  On drag events fall through to the normal
	// tab-drag handling.
	ଯଦି (is_click && !is_drag
		&& stl_click_handler_regions(tabline_stl_click,
					    tabline_stl_click_ଗଣନା,
					    0, (char_u *)"tabline",
					    mouse_row, mouse_col,
					    which_button, mod_mask))
	    ଫେରନ୍ତୁ FALSE;

	tp_label.just_in = true;
	tp_label.nr = TabPageIdxs[mouse_col];

	// click in a tab selects that tab page
	ଯଦି (is_click && cmdwin_type == 0
		&& mouse_col < firstwin->w_wincol + topframe->fr_width)
	    tp_label.just_click = true;
    }

    ଯଦି (tp_label.just_in)
    {
	ଯଦି (is_drag)
	{
	    ଯଦି (in_tabpanel || in_tab_ଧାଡ଼ି)
	    {
		c1 = tp_label.nr;
		tabpage_move(c1 <= 0 ? 9999 : c1 < tabpage_index(curtab)
							    ? c1 - 1 : c1);
	    }
	    ଫେରନ୍ତୁ FALSE;
	}

	ଯଦି (tp_label.just_click)
	{
	    ଯଦି (tp_label.is_panel)
		in_tabpanel = TRUE;
	    ଅନ୍ୟଥା
		in_tab_ଧାଡ଼ି = TRUE;
	    c1 = tp_label.nr;
	    ଯଦି (c1 >= 0)
	    {
		ଯଦି ((mod_mask & MOD_MASK_MULTI_CLICK) == MOD_MASK_2CLICK
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
		ଅନ୍ୟଥା
		{
		    // Go to specified tab page, or next one if not clicking
		    // on a label.
		    goto_tabpage(c1);

		    // It's like clicking on the status line of a window.
		    ଯଦି (curwin != old_curwin)
			end_visual_mode_keep_button();
		}
	    }
	    ଅନ୍ୟଥା
	    {
		tabpage_T	*tp;

		// Close the current or specified tab page.
		ଯଦି (c1 == -999)
		    tp = curtab;
		ଅନ୍ୟଥା
		    tp = find_tabpage(-c1);
		ଯଦି (tp == curtab)
		{
		    ଯଦି (first_tabpage->tp_next != ଖାଲି)
			tabpage_close(FALSE);
		}
		ଅନ୍ୟଥା ଯଦି (tp != ଖାଲି)
		    tabpage_close_other(tp, FALSE);
	    }
	}
	ଫେରନ୍ତୁ TRUE;
    }
    ଅନ୍ୟଥା ଯଦି (is_drag && (in_tabpanel || (in_tab_ଧାଡ଼ି && TabPageIdxs != ଖାଲି)))
    {
#ଯଦି defined(FEAT_TABPANEL)
	ଯଦି (in_tabpanel)
	    c1 = get_tabpagenr_on_tabpanel();
	ଅନ୍ୟଥା
#endif
	    c1 = TabPageIdxs[mouse_col];
	tabpage_move(c1 <= 0 ? 9999 : c1 - 1);
	ଫେରନ୍ତୁ FALSE;
    }

    // When 'mousemodel' is "popup" or "popup_setpos", translate mouse events:
    // right button up   -> pop-up menu
    // shift-left button -> right button
    // alt-left button   -> alt-right button
    ଯଦି (mouse_model_popup())
    {
	ଯଦି (which_button == MOUSE_ଡାହାଣ
			    && !(mod_mask & (MOD_MASK_SHIFT | MOD_MASK_CTRL)))
	{
#ifdef USE_POPUP_SETPOS
# ifdef FEAT_GUI
	    ଯଦି (gui.in_use)
	    {
#  ଯଦି defined(FEAT_GUI_MOTIF) || defined(FEAT_GUI_GTK) \
			  || defined(FEAT_GUI_PHOTON)
		ଯଦି (!is_click)
		    // Ignore right button release events, only shows the popup
		    // menu on the button down event.
		    ଫେରନ୍ତୁ FALSE;
#  endif
#  ଯଦି defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_HAIKU)
		ଯଦି (is_click || is_drag)
		    // Ignore right button down and drag mouse events.  Windows
		    // only shows the popup menu on the button up event.
		    ଫେରନ୍ତୁ FALSE;
#  endif
	    }
# endif
# ଯଦି defined(FEAT_GUI) && defined(FEAT_TERM_POPUP_MENU)
	    ଅନ୍ୟଥା
# endif
# ଯଦି defined(FEAT_TERM_POPUP_MENU)
	    ଯଦି (!is_click)
		// Ignore right button release events, only shows the popup
		// menu on the button down event.
		ଫେରନ୍ତୁ FALSE;
# endif

	    jump_flags = 0;
	    ଯଦି (STRCMP(p_mousem, "popup_setpos") == 0)
	    {
		// First set the cursor position before showing the popup
		// menu.
		ଯଦି (VIsual_active)
		{
		    pos_T    m_pos;

		    // set MOUSE_MAY_STOP_VIS if we are outside the
		    // selection or the current window (might have false
		    // negative here)
		    ଯଦି (mouse_row < curwin->w_winrow
			 || mouse_row
				  > (curwin->w_winrow + curwin->w_height))
			jump_flags = MOUSE_MAY_STOP_VIS;
		    ଅନ୍ୟଥା ଯଦି (get_fpos_of_mouse(&m_pos) != IN_BUFFER)
			jump_flags = MOUSE_MAY_STOP_VIS;
		    ଅନ୍ୟଥା
		    {
			ଯଦି (VIsual_mode == 'V')
			{
			    ଯଦି ((curwin->w_cursor.lnum <= VIsual.lnum
				    && (m_pos.lnum < curwin->w_cursor.lnum
					|| VIsual.lnum < m_pos.lnum))
				|| (VIsual.lnum < curwin->w_cursor.lnum
				    && (m_pos.lnum < VIsual.lnum
					|| curwin->w_cursor.lnum < m_pos.lnum)))
			    {
				jump_flags = MOUSE_MAY_STOP_VIS;
			    }
			}
			ଅନ୍ୟଥା ଯଦି ((LTOREQ_POS(curwin->w_cursor, VIsual)
				    && (LT_POS(m_pos, curwin->w_cursor)
					|| LT_POS(VIsual, m_pos)))
				|| (LT_POS(VIsual, curwin->w_cursor)
				    && (LT_POS(m_pos, VIsual)
				      || LT_POS(curwin->w_cursor, m_pos))))
			{
			    jump_flags = MOUSE_MAY_STOP_VIS;
			}
			ଅନ୍ୟଥା ଯଦି (VIsual_mode == Ctrl_V)
			{
			    getvcols(curwin, &curwin->w_cursor, &VIsual,
						     &leftcol, &rightcol, 0);
			    getvcol(curwin, &m_pos, ଖାଲି, &m_pos.col, ଖାଲି, 0);
			    ଯଦି (m_pos.col < leftcol || m_pos.col > rightcol)
				jump_flags = MOUSE_MAY_STOP_VIS;
			}
		    }
		}
		ଅନ୍ୟଥା
		    jump_flags = MOUSE_MAY_STOP_VIS;
	    }
	    ଯଦି (jump_flags)
	    {
		jump_flags = jump_to_mouse(jump_flags, ଖାଲି, which_button);
		update_curbuf(VIsual_active ? UPD_INVERTED : UPD_VALID);
		setcursor();
		out_flush();    // Update before showing popup menu
	    }
# ifdef FEAT_MENU
	    show_popupmenu();
	    got_click = FALSE;	// ignore release events
# endif
	    ଫେରନ୍ତୁ (jump_flags & CURSOR_MOVED) != 0;
#ଅନ୍ୟଥା
	    ଫେରନ୍ତୁ FALSE;
#endif
	}
	ଯଦି (which_button == MOUSE_ବାମ
				&& (mod_mask & (MOD_MASK_SHIFT|MOD_MASK_ALT)))
	{
	    which_button = MOUSE_ଡାହାଣ;
	    mod_mask &= ~MOD_MASK_SHIFT;
	}
    }

    ଯଦି ((State & (MODE_NORMAL | MODE_ଭର୍ତ୍ତି))
			    && !(mod_mask & (MOD_MASK_SHIFT | MOD_MASK_CTRL)))
    {
	ଯଦି (which_button == MOUSE_ବାମ)
	{
	    ଯଦି (is_click)
	    {
		// stop Visual mode for a left click in a window, but not when
		// on a status line
		ଯଦି (VIsual_active)
		    jump_flags |= MOUSE_MAY_STOP_VIS;
	    }
	    ଅନ୍ୟଥା ଯଦି (mouse_has(MOUSE_VISUAL))
		jump_flags |= MOUSE_MAY_VIS;
	}
	ଅନ୍ୟଥା ଯଦି (which_button == MOUSE_ଡାହାଣ)
	{
	    ଯଦି (is_click && VIsual_active)
	    {
		// Remember the start and end of visual before moving the
		// cursor.
		ଯଦି (LT_POS(curwin->w_cursor, VIsual))
		{
		    start_visual = curwin->w_cursor;
		    end_visual = VIsual;
		}
		ଅନ୍ୟଥା
		{
		    start_visual = VIsual;
		    end_visual = curwin->w_cursor;
		}
	    }
	    jump_flags |= MOUSE_FOCUS;
	    ଯଦି (mouse_has(MOUSE_VISUAL))
		jump_flags |= MOUSE_MAY_VIS;
	}
    }

    // If an operator is pending, ignore all drags and releases until the
    // next mouse click.
    ଯଦି (!is_drag && oap != ଖାଲି && oap->op_type != OP_NOP)
    {
	got_click = FALSE;
	oap->motion_type = MCHAR;
    }

    // When releasing the button let jump_to_mouse() know.
    ଯଦି (!is_click && !is_drag)
	jump_flags |= MOUSE_RELEASED;

    // JUMP!
    jump_flags = jump_to_mouse(jump_flags,
			oap == ଖାଲି ? ଖାଲି : &(oap->inclusive), which_button);

#ifdef FEAT_MENU
    // A click in the window toolbar has no side effects.
    ଯଦି (jump_flags & MOUSE_WINBAR)
	ଫେରନ୍ତୁ FALSE;
#endif
    moved = (jump_flags & CURSOR_MOVED);
    in_status_ଧାଡ଼ି = (jump_flags & IN_STATUS_ଧାଡ଼ି);
    in_sep_ଧାଡ଼ି = (jump_flags & IN_SEP_ଧାଡ଼ି);

    // Check for statusline click handler early, before visual mode or
    // other button-specific handling can interfere.
    ଯଦି (in_status_ଧାଡ଼ି && is_click && !is_drag
	    && stl_click_handler(dragwin, mouse_row, mouse_col,
						which_button, mod_mask))
    {
#ifdef FEAT_MOUSESHAPE
	ଯଦି (!drag_status_ଧାଡ଼ି)
	{
	    drag_status_ଧାଡ଼ି = TRUE;
	    update_mouseshape(-1);
	}
#endif
	ଫେରନ୍ତୁ FALSE;
    }

#ifdef FEAT_NETBEANS_INTG
    ଯଦି (isNetbeansBuffer(curbuf)
			    && !(jump_flags & (IN_STATUS_ଧାଡ଼ି | IN_SEP_ଧାଡ଼ି)))
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଚାବି = KEY2TERMCAP1(c);

	ଯଦି (ଚାବି == (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)KE_LEFTRELEASE || ଚାବି == (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)KE_MIDDLERELEASE
					       || ଚାବି == (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)KE_RIGHTRELEASE)
	    netbeans_button_release(which_button);
    }
#endif

    // When jumping to another window, clear a pending operator.  That's a bit
    // friendlier than beeping and not jumping to that window.
    ଯଦି (curwin != old_curwin && oap != ଖାଲି && oap->op_type != OP_NOP)
	clearop(oap);

#ifdef FEAT_FOLDING
    ଯଦି (mod_mask == 0
	    && !is_drag
	    && (jump_flags & (MOUSE_FOLD_CLOSE | MOUSE_FOLD_OPEN))
	    && which_button == MOUSE_ବାମ)
    {
	// open or close a fold at this line
	ଯଦି (jump_flags & MOUSE_FOLD_OPEN)
	    openFold(curwin->w_cursor.lnum, 1L);
	ଅନ୍ୟଥା
	    closeFold(curwin->w_cursor.lnum, 1L);
	// don't move the cursor if still in the same window
	ଯଦି (curwin == old_curwin)
	    curwin->w_cursor = save_cursor;
    }
#endif

#ଯଦି defined(FEAT_CLIPBOARD)
    ଯଦି ((jump_flags & IN_OTHER_WIN) && !VIsual_active &&
	    (clip_star.available || clip_plus.available))
    {
	clip_modeless(which_button, is_click, is_drag);
	ଫେରନ୍ତୁ FALSE;
    }
#endif

    // Set global flag that we are extending the Visual area with mouse
    // dragging; temporarily minimize 'scrolloff'.
    ଯଦି (VIsual_active && is_drag && get_scrolloff_ମୂଲ୍ୟ())
    {
	// In the very first line, allow scrolling one line
	ଯଦି (mouse_row == 0)
	    mouse_dragging = 2;
	ଅନ୍ୟଥା
	    mouse_dragging = 1;
    }

    // When dragging the mouse above the window, scroll down.
    ଯଦି (is_drag && mouse_row < 0 && !in_status_ଧାଡ଼ି)
    {
	scroll_redraw(FALSE, 1L);
	mouse_row = 0;
    }

    ଯଦି (start_visual.lnum)		// right click in visual mode
    {
       // When ALT is pressed make Visual mode blockwise.
       ଯଦି (mod_mask & MOD_MASK_ALT)
	   VIsual_mode = Ctrl_V;

	// In Visual-block mode, divide the area in four, pick up the corner
	// that is in the quarter that the cursor is in.
	ଯଦି (VIsual_mode == Ctrl_V)
	{
	    getvcols(curwin, &start_visual, &end_visual,
						    &leftcol, &rightcol, 0);
	    ଯଦି (curwin->w_curswant > (leftcol + rightcol) / 2)
		end_visual.col = leftcol;
	    ଅନ୍ୟଥା
		end_visual.col = rightcol;
	    ଯଦି (curwin->w_cursor.lnum >=
				    (start_visual.lnum + end_visual.lnum) / 2)
		end_visual.lnum = start_visual.lnum;

	    // move VIsual to the right column
	    start_visual = curwin->w_cursor;	    // save the cursor pos
	    curwin->w_cursor = end_visual;
	    coladvance(end_visual.col);
	    VIsual = curwin->w_cursor;
	    curwin->w_cursor = start_visual;	    // restore the cursor
	}
	ଅନ୍ୟଥା
	{
	    // If the click is before the start of visual, change the start.
	    // If the click is after the end of visual, change the end.  If
	    // the click is inside the visual, change the closest side.
	    ଯଦି (LT_POS(curwin->w_cursor, start_visual))
		VIsual = end_visual;
	    ଅନ୍ୟଥା ଯଦି (LT_POS(end_visual, curwin->w_cursor))
		VIsual = start_visual;
	    ଅନ୍ୟଥା
	    {
		// In the same line, compare column number
		ଯଦି (end_visual.lnum == start_visual.lnum)
		{
		    ଯଦି (curwin->w_cursor.col - start_visual.col >
				    end_visual.col - curwin->w_cursor.col)
			VIsual = start_visual;
		    ଅନ୍ୟଥା
			VIsual = end_visual;
		}

		// In different lines, compare line number
		ଅନ୍ୟଥା
		{
		    diff = (curwin->w_cursor.lnum - start_visual.lnum) -
				(end_visual.lnum - curwin->w_cursor.lnum);

		    ଯଦି (diff > 0)		// closest to end
			VIsual = start_visual;
		    ଅନ୍ୟଥା ଯଦି (diff < 0)	// closest to start
			VIsual = end_visual;
		    ଅନ୍ୟଥା			// in the middle line
		    {
			ଯଦି (curwin->w_cursor.col <
					(start_visual.col + end_visual.col) / 2)
			    VIsual = end_visual;
			ଅନ୍ୟଥା
			    VIsual = start_visual;
		    }
		}
	    }
	}
    }
    // If Visual mode started in insert mode, execute "CTRL-O"
    ଅନ୍ୟଥା ଯଦି ((State & MODE_ଭର୍ତ୍ତି) && VIsual_active)
	stuffcharReadbuff(Ctrl_O);

    // Middle mouse click: Put text before cursor.
    ଯଦି (which_button == MOUSE_ମଧ୍ୟ)
    {
#ifdef FEAT_CLIPBOARD
	ଯଦି (clip_star.available && regname == 0)
	    regname = '*';
#endif
	ଯଦି (yank_register_mline(regname))
	{
	    ଯଦି (mouse_past_bottom)
		dir = FORWARD;
	}
	ଅନ୍ୟଥା ଯଦି (mouse_past_eol)
	    dir = FORWARD;

	ଯଦି (fixindent)
	{
	    c1 = (dir == BACKWARD) ? '[' : ']';
	    c2 = 'p';
	}
	ଅନ୍ୟଥା
	{
	    c1 = (dir == FORWARD) ? 'p' : 'P';
	    c2 = NUL;
	}
	prep_redo(regname, ଗଣନା_2, NUL, c1, NUL, c2, NUL);

	// Remember where the paste started, so in edit() Insstart can be set
	// to this position
	ଯଦି (restart_edit != 0)
	    where_paste_started = curwin->w_cursor;
	do_put(regname, ଖାଲି, dir, ଗଣନା_2, fixindent | PUT_CURSEND);
    }

#ଯଦି defined(FEAT_QUICKFIX)
    // Ctrl-Mouse click or double click in a quickfix window jumps to the
    // error under the mouse pointer.
    ଅନ୍ୟଥା ଯଦି (((mod_mask & MOD_MASK_CTRL)
		|| (mod_mask & MOD_MASK_MULTI_CLICK) == MOD_MASK_2CLICK)
	    && bt_quickfix(curbuf))
    {
	ଯଦି (curwin->w_llist_ref == ଖାଲି)	// quickfix window
	    do_cmdline_cmd((char_u *)".cc");
	ଅନ୍ୟଥା					// location list window
	    do_cmdline_cmd((char_u *)".ll");
	got_click = FALSE;		// ignore drag&release now
    }
#endif

    // Ctrl-Mouse click (or double click in a help window) jumps to the tag
    // under the mouse pointer.
    ଅନ୍ୟଥା ଯଦି ((mod_mask & MOD_MASK_CTRL) || (curbuf->b_help
		     && (mod_mask & MOD_MASK_MULTI_CLICK) == MOD_MASK_2CLICK))
    {
	ଯଦି (State & MODE_ଭର୍ତ୍ତି)
	    stuffcharReadbuff(Ctrl_O);
	stuffcharReadbuff(Ctrl_RSB);
	got_click = FALSE;		// ignore drag&release now
    }

    // Shift-Mouse click searches for the next occurrence of the word under
    // the mouse pointer
    ଅନ୍ୟଥା ଯଦି ((mod_mask & MOD_MASK_SHIFT))
    {
	ଯଦି ((State & MODE_ଭର୍ତ୍ତି) || (VIsual_active && VIsual_select))
	    stuffcharReadbuff(Ctrl_O);
	ଯଦି (which_button == MOUSE_ବାମ)
	    stuffcharReadbuff('*');
	ଅନ୍ୟଥା	// MOUSE_RIGHT
	    stuffcharReadbuff('#');
    }

    // Handle double clicks, unless on status line
    ଅନ୍ୟଥା ଯଦି (in_status_ଧାଡ଼ି)
    {
#ifdef FEAT_MOUSESHAPE
	ଯଦି ((is_drag || is_click) && !drag_status_ଧାଡ଼ି)
	{
	    drag_status_ଧାଡ଼ି = TRUE;
	    update_mouseshape(-1);
	}
#endif
    }
    ଅନ୍ୟଥା ଯଦି (in_sep_ଧାଡ଼ି)
    {
#ifdef FEAT_MOUSESHAPE
	ଯଦି ((is_drag || is_click) && !drag_sep_ଧାଡ଼ି)
	{
	    drag_sep_ଧାଡ଼ି = TRUE;
	    update_mouseshape(-1);
	}
#endif
    }
    ଅନ୍ୟଥା ଯଦି ((mod_mask & MOD_MASK_MULTI_CLICK)
				       && (State & (MODE_NORMAL | MODE_ଭର୍ତ୍ତି))
	     && mouse_has(MOUSE_VISUAL))
    {
	ଯଦି (is_click || !VIsual_active)
	{
	    ଯଦି (VIsual_active)
		orig_cursor = VIsual;
	    ଅନ୍ୟଥା
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
	    ଯଦି ((mod_mask & MOD_MASK_MULTI_CLICK) == MOD_MASK_2CLICK)
	    {
		// Double click with ALT pressed makes it blockwise.
		ଯଦି (mod_mask & MOD_MASK_ALT)
		    VIsual_mode = Ctrl_V;
		ଅନ୍ୟଥା
		    VIsual_mode = 'v';
	    }
	    ଅନ୍ୟଥା ଯଦି ((mod_mask & MOD_MASK_MULTI_CLICK) == MOD_MASK_3CLICK)
		VIsual_mode = 'V';
	    ଅନ୍ୟଥା ଯଦି ((mod_mask & MOD_MASK_MULTI_CLICK) == MOD_MASK_4CLICK)
		VIsual_mode = Ctrl_V;
#ifdef FEAT_CLIPBOARD
	    // Make sure the clipboard gets updated.  Needed because start and
	    // end may still be the same, and the selection needs to be owned
	    clip_star.vmode = NUL;
#endif
	}
	// A double click selects a word or a block.
	ଯଦି ((mod_mask & MOD_MASK_MULTI_CLICK) == MOD_MASK_2CLICK)
	{
	    pos_T	*pos = ଖାଲି;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		gc;

	    ଯଦି (is_click)
	    {
		// If the character under the cursor (skipping white space) is
		// not a word character, try finding a match and select a (),
		// {}, [], #if/#endif, etc. block.
		end_visual = curwin->w_cursor;
		ଯେତେବେଳେ (gc = gchar_pos(&end_visual), VIM_ISWHITE(gc))
		    inc(&end_visual);
		ଯଦି (oap != ଖାଲି)
		    oap->motion_type = MCHAR;
		ଯଦି (oap != ଖାଲି
			&& VIsual_mode == 'v'
			&& !vim_iswordc(gchar_pos(&end_visual))
			&& EQUAL_POS(curwin->w_cursor, VIsual)
			&& (pos = findmatch(oap, NUL)) != ଖାଲି)
		{
		    curwin->w_cursor = *pos;
		    ଯଦି (oap->motion_type == MLINE)
			VIsual_mode = 'V';
		    ଅନ୍ୟଥା ଯଦି (*p_sel == 'e')
		    {
			ଯଦି (LT_POS(curwin->w_cursor, VIsual))
			    ++VIsual.col;
			ଅନ୍ୟଥା
			    ++curwin->w_cursor.col;
		    }
		}
	    }

	    ଯଦି (pos == ଖାଲି && (is_click || is_drag))
	    {
		// When not found a match or when dragging: extend to include
		// a word.
		ଯଦି (LT_POS(curwin->w_cursor, orig_cursor))
		{
		    find_start_of_word(&curwin->w_cursor);
		    find_end_of_word(&VIsual);
		}
		ଅନ୍ୟଥା
		{
		    find_start_of_word(&VIsual);
		    ଯଦି (*p_sel == 'e' && *ml_get_cursor() != NUL)
			curwin->w_cursor.col +=
					 (*mb_ptr2len)(ml_get_cursor());
		    find_end_of_word(&curwin->w_cursor);
		}
	    }
	    curwin->w_set_curswant = true;
	}
	ଯଦି (is_click)
	    redraw_curbuf_later(UPD_INVERTED);	// update the inversion
    }
    ଅନ୍ୟଥା ଯଦି (VIsual_active && !old_active)
    {
	ଯଦି (mod_mask & MOD_MASK_ALT)
	    VIsual_mode = Ctrl_V;
	ଅନ୍ୟଥା
	    VIsual_mode = 'v';
    }

    // If Visual mode changed show it later.
    ଯଦି ((!VIsual_active && old_active && mode_displayed)
	    || (VIsual_active && p_smd && ସନ୍ଦେଶ_silent == 0
				 && (!old_active || VIsual_mode != old_mode)))
	redraw_cmdline = TRUE;

    ଫେରନ୍ତୁ moved;
}

    ଶୂନ୍ୟ
ins_mouse(ପୂର୍ଣ୍ଣସଂଖ୍ୟା c)
{
    pos_T	tpos;
    win_T	*old_curwin = curwin;

#ifdef FEAT_GUI
    // When GUI is active, also move/paste when 'mouse' is empty
    ଯଦି (!gui.in_use)
#endif
	ଯଦି (!mouse_has(MOUSE_ଭର୍ତ୍ତି))
	    ଫେରନ୍ତୁ;

    undisplay_dollar();
    tpos = curwin->w_cursor;
    ଯଦି (do_mouse(ଖାଲି, c, BACKWARD, 1L, 0))
    {
	win_T	*new_curwin = curwin;

	ଯଦି (curwin != old_curwin && win_valid(old_curwin))
	{
	    // Mouse took us to another window.  We need to go back to the
	    // previous one to stop insert there properly.
	    curwin = old_curwin;
	    curbuf = curwin->w_buffer;
#ifdef FEAT_JOB_CHANNEL
	    ଯଦି (bt_prompt(curbuf))
		// Restart Insert mode when re-entering the prompt buffer.
		curbuf->b_prompt_ଭର୍ତ୍ତି = 'A';
#endif
	}
	start_arrow(curwin == old_curwin ? &tpos : ଖାଲି);
	ଯଦି (curwin != new_curwin && win_valid(new_curwin))
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
    ସ୍ଥିର ଶୂନ୍ୟ
do_mousescroll(cmdarg_T *cap)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା shift_or_ctrl = mod_mask & (MOD_MASK_SHIFT | MOD_MASK_CTRL);

#ifdef FEAT_TERMINAL
    ଯଦି (term_use_loop())
	// This window is a terminal window, send the mouse event there.
	// Set "typed" to FALSE to avoid an endless loop.
	send_ଚାବିଗୁଡ଼ିକ_to_term(curbuf->b_term, cap->cmdchar, mod_mask, FALSE);
    ଅନ୍ୟଥା
#endif
    ଯଦି (cap->arg == MSCR_UP || cap->arg == MSCR_DOWN)
    {
	// Vertical scrolling
	ଯଦି (!(State & MODE_ଭର୍ତ୍ତି) && (mouse_vert_step < 0 || shift_or_ctrl))
	{
	    // whole page up or down
	    pagescroll(cap->arg == MSCR_UP ? FORWARD : BACKWARD, 1L, FALSE);
	}
	ଅନ୍ୟଥା
	{
	    ଯଦି (mouse_vert_step < 0 || shift_or_ctrl)
	    {
		// whole page up or down
		cap->count1 = (long)(curwin->w_botline - curwin->w_topline);
	    }
	    // Don't scroll more than half the window height.
	    ଅନ୍ୟଥା ଯଦି (curwin->w_height < mouse_vert_step * 2)
	    {
		cap->count1 = curwin->w_height / 2;
		ଯଦି (cap->count1 == 0)
		    cap->count1 = 1;
	    }
	    ଅନ୍ୟଥା
	    {
		cap->count1 = mouse_vert_step;
	    }
	    cap->count0 = cap->count1;
	    nv_scroll_ଧାଡ଼ି(cap);
	}

#ifdef FEAT_PROP_POPUP
	ଯଦି (WIN_IS_POPUP(curwin))
	    popup_set_firstline(curwin);
#endif
    }
    ଅନ୍ୟଥା
    {
	// Horizontal scrolling
	long step = (mouse_hor_step < 0 || shift_or_ctrl)
					    ? curwin->w_width : mouse_hor_step;
	long leftcol = curwin->w_leftcol
				     + (cap->arg == MSCR_ଡାହାଣ ? -step : step);
	ଯଦି (leftcol < 0)
	    leftcol = 0;
	do_mousescroll_horiz((long_u)leftcol);
    }
    may_trigger_win_scrolled_resized();
}

/*
 * Insert mode implementation for scrolling in direction "dir", which is
 * one of the MSCR_ values.
 */
    ଶୂନ୍ୟ
ins_mousescroll(ପୂର୍ଣ୍ଣସଂଖ୍ୟା dir)
{
    cmdarg_T	cap;
    oparg_T	oa;
    CLEAR_FIELD(cap);
    clear_oparg(&oa);
    cap.oap = &oa;
    cap.arg = dir;

#ifdef FEAT_TABPANEL
    ଯଦି (mouse_row >= 0 && mouse_col >= 0
	    && (dir == MSCR_UP || dir == MSCR_DOWN)
	    && mouse_on_tabpanel())
    {
	(ଶୂନ୍ୟ)tabpanel_scroll(dir == MSCR_UP ? 1 : -1,
		mouse_vert_step > 0 ? mouse_vert_step : 3);
	ଫେରନ୍ତୁ;
    }
#endif

    ବାଛ (dir)
    {
	ସ୍ଥିତି MSCR_UP:
	    cap.cmdchar = K_MOUSEUP;
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି MSCR_DOWN:
	    cap.cmdchar = K_MOUSEDOWN;
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି MSCR_ବାମ:
	    cap.cmdchar = K_MOUSELEFT;
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି MSCR_ଡାହାଣ:
	    cap.cmdchar = K_MOUSERIGHT;
	    ଭାଙ୍ଗ;
	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    siemsg("Invalid ins_mousescroll() argument: %d", dir);
    }

    win_T *old_curwin = curwin;
    ଯଦି (mouse_row >= 0 && mouse_col >= 0)
    {
	// Find the window at the mouse pointer coordinates.
	// NOTE: Must restore "curwin" to "old_curwin" before returning!
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା row = mouse_row;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା col = mouse_col;
	curwin = mouse_find_win(&row, &col, FIND_POPUP);
	ଯଦି (curwin == ଖାଲି)
	{
	    curwin = old_curwin;
	    ଫେରନ୍ତୁ;
	}
	curbuf = curwin->w_buffer;
    }

    ଯଦି (curwin == old_curwin)
    {
	// Don't scroll the current window if the popup menu is visible.
	ଯଦି (pum_visible())
	    ଫେରନ୍ତୁ;

	undisplay_dollar();
    }

    linenr_T	orig_topline = curwin->w_topline;
    colnr_T	orig_leftcol = curwin->w_leftcol;
    pos_T	orig_cursor = curwin->w_cursor;

    // Call the common mouse scroll function shared with other modes.
    do_mousescroll(&cap);

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା did_scroll = (orig_topline != curwin->w_topline
		   || orig_leftcol != curwin->w_leftcol);

    curwin->w_redr_status = true;
    curwin = old_curwin;
    curbuf = curwin->w_buffer;

    // If the window actually scrolled and the popup menu may overlay the
    // window, need to redraw it.
    ଯଦି (did_scroll && pum_visible())
    {
	// TODO: Would be more efficient to only redraw the windows that are
	// overlapped by the popup menu.
	redraw_all_later(UPD_NOT_VALID);
	ins_compl_show_pum();
    }

    ଯଦି (!EQUAL_POS(curwin->w_cursor, orig_cursor))
    {
	start_arrow(&orig_cursor);
	set_can_cindent(TRUE);
    }
}

#ଯଦି defined(FEAT_PROP_POPUP)
/*
 * Command-line mode implementation for scrolling in direction "dir", which is
 * one of the MSCR_ values.  Scrolls the completion info popup when the mouse
 * pointer is on top of it.
 * Returns TRUE when the info popup was scrolled.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
cmdline_mousescroll(ପୂର୍ଣ୍ଣସଂଖ୍ୟା dir)
{
    cmdarg_T	cap;
    oparg_T	oa;

    CLEAR_FIELD(cap);
    clear_oparg(&oa);
    cap.oap = &oa;
    cap.arg = dir;

    ବାଛ (dir)
    {
	ସ୍ଥିତି MSCR_UP:	 cap.cmdchar = K_MOUSEUP; ଭାଙ୍ଗ;
	ସ୍ଥିତି MSCR_DOWN:	 cap.cmdchar = K_MOUSEDOWN; ଭାଙ୍ଗ;
	ସ୍ଥିତି MSCR_ବାମ:	 cap.cmdchar = K_MOUSELEFT; ଭାଙ୍ଗ;
	ସ୍ଥିତି MSCR_ଡାହାଣ: cap.cmdchar = K_MOUSERIGHT; ଭାଙ୍ଗ;
    }

    ଯଦି (mouse_row < 0 || mouse_col < 0)
	ଫେରନ୍ତୁ FALSE;

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    row = mouse_row;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    col = mouse_col;
    win_T   *wp;

    // Only scroll when the mouse is on top of the info popup.
    wp = mouse_find_win(&row, &col, FIND_POPUP);
    ଯଦି (wp == ଖାଲି || !WIN_IS_POPUP(wp) || !(wp->w_popup_flags & POPF_INFO)
							|| !wp->w_has_scrollbar)
	ଫେରନ୍ତୁ FALSE;

    win_T   *old_curwin = curwin;

    curwin = wp;
    curbuf = wp->w_buffer;
    // Call the common mouse scroll function shared with other modes.
    do_mousescroll(&cap);
    curwin = old_curwin;
    curbuf = curwin->w_buffer;

    // Cmdline mode doesn't normally call update_screen(), so redraw the
    // completion popup menu, which also repaints the info popup.
    ଯଦି (cmdline_pum_active())
	cmdline_pum_display();
    ଫେରନ୍ତୁ TRUE;
}
#endif

/*
 * Return TRUE if "c" is a mouse key.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
is_mouse_ଚାବି(ପୂର୍ଣ୍ଣସଂଖ୍ୟା c)
{
    ଫେରନ୍ତୁ c == K_LEFTMOUSE
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

ସ୍ଥିର ସଂରଚନା mousetable
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    pseudo_code;	// Code for pseudo mouse event
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    button;		// Which mouse button is it?
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    is_click;		// Is it a mouse button click event?
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    is_drag;		// Is it a mouse drag event?
} mouse_table[] =
{
    {(ପୂର୍ଣ୍ଣସଂଖ୍ୟା)KE_LEFTMOUSE,		MOUSE_ବାମ,	TRUE,	FALSE},
#ifdef FEAT_GUI
    {(ପୂର୍ଣ୍ଣସଂଖ୍ୟା)KE_LEFTMOUSE_NM,	MOUSE_ବାମ,	TRUE,	FALSE},
#endif
    {(ପୂର୍ଣ୍ଣସଂଖ୍ୟା)KE_LEFTDRAG,		MOUSE_ବାମ,	FALSE,	TRUE},
    {(ପୂର୍ଣ୍ଣସଂଖ୍ୟା)KE_LEFTRELEASE,	MOUSE_ବାମ,	FALSE,	FALSE},
#ifdef FEAT_GUI
    {(ପୂର୍ଣ୍ଣସଂଖ୍ୟା)KE_LEFTRELEASE_NM,	MOUSE_ବାମ,	FALSE,	FALSE},
#endif
    {(ପୂର୍ଣ୍ଣସଂଖ୍ୟା)KE_MIDDLEMOUSE,	MOUSE_ମଧ୍ୟ,	TRUE,	FALSE},
    {(ପୂର୍ଣ୍ଣସଂଖ୍ୟା)KE_MIDDLEDRAG,	MOUSE_ମଧ୍ୟ,	FALSE,	TRUE},
    {(ପୂର୍ଣ୍ଣସଂଖ୍ୟା)KE_MIDDLERELEASE,	MOUSE_ମଧ୍ୟ,	FALSE,	FALSE},
    {(ପୂର୍ଣ୍ଣସଂଖ୍ୟା)KE_RIGHTMOUSE,	MOUSE_ଡାହାଣ,	TRUE,	FALSE},
    {(ପୂର୍ଣ୍ଣସଂଖ୍ୟା)KE_RIGHTDRAG,		MOUSE_ଡାହାଣ,	FALSE,	TRUE},
    {(ପୂର୍ଣ୍ଣସଂଖ୍ୟା)KE_RIGHTRELEASE,	MOUSE_ଡାହାଣ,	FALSE,	FALSE},
    {(ପୂର୍ଣ୍ଣସଂଖ୍ୟା)KE_X1MOUSE,		MOUSE_X1,	TRUE,	FALSE},
    {(ପୂର୍ଣ୍ଣସଂଖ୍ୟା)KE_X1DRAG,		MOUSE_X1,	FALSE,	TRUE},
    {(ପୂର୍ଣ୍ଣସଂଖ୍ୟା)KE_X1RELEASE,		MOUSE_X1,	FALSE,	FALSE},
    {(ପୂର୍ଣ୍ଣସଂଖ୍ୟା)KE_X2MOUSE,		MOUSE_X2,	TRUE,	FALSE},
    {(ପୂର୍ଣ୍ଣସଂଖ୍ୟା)KE_X2DRAG,		MOUSE_X2,	FALSE,	TRUE},
    {(ପୂର୍ଣ୍ଣସଂଖ୍ୟା)KE_X2RELEASE,		MOUSE_X2,	FALSE,	FALSE},
    // DRAG without CLICK
    {(ପୂର୍ଣ୍ଣସଂଖ୍ୟା)KE_MOUSEMOVE,		MOUSE_RELEASE,	FALSE,	TRUE},
    // RELEASE without CLICK
    {(ପୂର୍ଣ୍ଣସଂଖ୍ୟା)KE_IGNORE,		MOUSE_RELEASE,	FALSE,	FALSE},
    {0,				0,		0,	0},
};

/*
 * Look up the given mouse code to return the relevant information in the other
 * arguments.  Return which button is down or was released.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
get_mouse_button(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *is_click, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *is_drag)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    i;

    ପାଇଁ (i = 0; mouse_table[i].pseudo_code; i++)
	ଯଦି (code == mouse_table[i].pseudo_code)
	{
	    *is_click = mouse_table[i].is_click;
	    *is_drag = mouse_table[i].is_drag;
	    ଫେରନ୍ତୁ mouse_table[i].button;
	}
    ଫେରନ୍ତୁ 0;	    // Shouldn't get here
}

/*
 * Return the appropriate pseudo mouse event token (KE_LEFTMOUSE etc) based on
 * the given information about which mouse button is down, and whether the
 * mouse was clicked, dragged or released.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
get_pseudo_mouse_code(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    button,	// eg MOUSE_LEFT
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    is_click,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    is_drag)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    i;

    ପାଇଁ (i = 0; mouse_table[i].pseudo_code; i++)
	ଯଦି (button == mouse_table[i].button
	    && is_click == mouse_table[i].is_click
	    && is_drag == mouse_table[i].is_drag)
	{
#ifdef FEAT_GUI
	    // Trick: a non mappable left click and release has mouse_col -1
	    // or added MOUSE_COLOFF.  Used for 'mousefocus' in
	    // gui_mouse_moved()
	    ଯଦି (mouse_col < 0 || mouse_col > MOUSE_COLOFF)
	    {
		ଯଦି (mouse_col < 0)
		    mouse_col = 0;
		ଅନ୍ୟଥା
		    mouse_col -= MOUSE_COLOFF;
		ଯଦି (mouse_table[i].pseudo_code == (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)KE_LEFTMOUSE)
		    ଫେରନ୍ତୁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)KE_LEFTMOUSE_NM;
		ଯଦି (mouse_table[i].pseudo_code == (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)KE_LEFTRELEASE)
		    ଫେରନ୍ତୁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)KE_LEFTRELEASE_NM;
	    }
#endif
	    ଫେରନ୍ତୁ mouse_table[i].pseudo_code;
	}
    ଫେରନ୍ତୁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)KE_IGNORE;	    // not recognized, ignore it
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
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା has_mouse_termcode = 0;

    ଶୂନ୍ୟ
set_mouse_termcode(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		n,	// KS_MOUSE, KS_NETTERM_MOUSE or KS_DEC_MOUSE
    char_u	*s)
{
    char_u	name[2];

    name[0] = n;
    name[1] = KE_FILLER;
    add_termcode(name, s, FALSE);
#ifdef FEAT_MOUSE_JSB
    ଯଦି (n == KS_JSBTERM_MOUSE)
	has_mouse_termcode |= HMT_JSBTERM;
    ଅନ୍ୟଥା
#endif
#ifdef FEAT_MOUSE_NET
    ଯଦି (n == KS_NETTERM_MOUSE)
	has_mouse_termcode |= HMT_NETTERM;
    ଅନ୍ୟଥା
#endif
#ifdef FEAT_MOUSE_DEC
    ଯଦି (n == KS_DEC_MOUSE)
	has_mouse_termcode |= HMT_DEC;
    ଅନ୍ୟଥା
#endif
#ifdef FEAT_MOUSE_PTERM
    ଯଦି (n == KS_PTERM_MOUSE)
	has_mouse_termcode |= HMT_PTERM;
    ଅନ୍ୟଥା
#endif
#ifdef FEAT_MOUSE_URXVT
    ଯଦି (n == KS_URXVT_MOUSE)
	has_mouse_termcode |= HMT_URXVT;
    ଅନ୍ୟଥା
#endif
#ifdef FEAT_MOUSE_GPM
    ଯଦି (n == KS_GPM_MOUSE)
	has_mouse_termcode |= HMT_GPM;
    ଅନ୍ୟଥା
#endif
    ଯଦି (n == KS_SGR_MOUSE)
	has_mouse_termcode |= HMT_SGR;
    ଅନ୍ୟଥା ଯଦି (n == KS_SGR_MOUSE_RELEASE)
	has_mouse_termcode |= HMT_SGR_REL;
    ଅନ୍ୟଥା
	has_mouse_termcode |= HMT_NORMAL;
}

#ଯଦି defined(UNIX) || defined(VMS)
    ଶୂନ୍ୟ
del_mouse_termcode(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		n)	// KS_MOUSE, KS_NETTERM_MOUSE or KS_DEC_MOUSE
{
    char_u	name[2];

    name[0] = n;
    name[1] = KE_FILLER;
    del_termcode(name);
# ifdef FEAT_MOUSE_JSB
    ଯଦି (n == KS_JSBTERM_MOUSE)
	has_mouse_termcode &= ~HMT_JSBTERM;
    ଅନ୍ୟଥା
# endif
# ifdef FEAT_MOUSE_NET
    ଯଦି (n == KS_NETTERM_MOUSE)
	has_mouse_termcode &= ~HMT_NETTERM;
    ଅନ୍ୟଥା
# endif
# ifdef FEAT_MOUSE_DEC
    ଯଦି (n == KS_DEC_MOUSE)
	has_mouse_termcode &= ~HMT_DEC;
    ଅନ୍ୟଥା
# endif
# ifdef FEAT_MOUSE_PTERM
    ଯଦି (n == KS_PTERM_MOUSE)
	has_mouse_termcode &= ~HMT_PTERM;
    ଅନ୍ୟଥା
# endif
# ifdef FEAT_MOUSE_URXVT
    ଯଦି (n == KS_URXVT_MOUSE)
	has_mouse_termcode &= ~HMT_URXVT;
    ଅନ୍ୟଥା
# endif
# ifdef FEAT_MOUSE_GPM
    ଯଦି (n == KS_GPM_MOUSE)
	has_mouse_termcode &= ~HMT_GPM;
    ଅନ୍ୟଥା
# endif
    ଯଦି (n == KS_SGR_MOUSE)
	has_mouse_termcode &= ~HMT_SGR;
    ଅନ୍ୟଥା ଯଦି (n == KS_SGR_MOUSE_RELEASE)
	has_mouse_termcode &= ~HMT_SGR_REL;
    ଅନ୍ୟଥା
	has_mouse_termcode &= ~HMT_NORMAL;
}
#endif

/*
 * setmouse() - switch mouse on/off depending on current mode and 'mouse'
 */
    ଶୂନ୍ୟ
setmouse(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    checkfor;

#ifdef FEAT_MOUSESHAPE
    update_mouseshape(-1);
#endif

    // Should be outside proc, but may break MOUSESHAPE
#ifdef FEAT_GUI
    // In the GUI the mouse is always enabled.
    ଯଦି (gui.in_use)
	ଫେରନ୍ତୁ;
#endif
    // be quick when mouse is off
    ଯଦି (*p_mouse == NUL || has_mouse_termcode == 0)
	ଫେରନ୍ତୁ;

    // don't switch mouse on when not in raw mode (Ex mode)
    ଯଦି (cur_tmode != TMODE_RAW)
    {
	mch_setmouse(FALSE);
	ଫେରନ୍ତୁ;
    }

    ଯଦି (VIsual_active)
	checkfor = MOUSE_VISUAL;
    ଅନ୍ୟଥା ଯଦି (State == MODE_HITRETURN || State == MODE_ASKMORE
						     || State == MODE_SETWSIZE)
	checkfor = MOUSE_RETURN;
    ଅନ୍ୟଥା ଯଦି (State & MODE_ଭର୍ତ୍ତି)
	checkfor = MOUSE_ଭର୍ତ୍ତି;
    ଅନ୍ୟଥା ଯଦି (State & MODE_CMDLINE)
	checkfor = MOUSE_COMMAND;
    ଅନ୍ୟଥା ଯଦି (State == MODE_CONFIRM || State == MODE_EXTERNCMD)
	checkfor = ' '; // don't use mouse for ":confirm" or ":!cmd"
    ଅନ୍ୟଥା
	checkfor = MOUSE_NORMAL;    // assume normal mode

    ଯଦି (mouse_has(checkfor))
	mch_setmouse(TRUE);
    ଅନ୍ୟଥା
	mch_setmouse(FALSE);
}

/*
 * Return TRUE if
 * - "c" is in 'mouse', or
 * - 'a' is in 'mouse' and "c" is in MOUSE_A, or
 * - the current buffer is a help file and 'h' is in 'mouse' and we are in a
 *   normal editing mode (not at hit-return message).
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mouse_has(ପୂର୍ଣ୍ଣସଂଖ୍ୟା c)
{
    char_u	*p;

    ପାଇଁ (p = p_mouse; *p; ++p)
	ବାଛ (*p)
	{
	    ସ୍ଥିତି 'a': ଯଦି (vim_strchr((char_u *)MOUSE_A, c) != ଖାଲି)
			  ଫେରନ୍ତୁ TRUE;
		      ଭାଙ୍ଗ;
	    ସ୍ଥିତି MOUSE_HELP: ଯଦି (c != MOUSE_RETURN && curbuf->b_help)
				 ଫେରନ୍ତୁ TRUE;
			     ଭାଙ୍ଗ;
	    ପୂର୍ବନିର୍ଦ୍ଧାରିତ: ଯଦି (c == *p) ଫେରନ୍ତୁ TRUE; ଭାଙ୍ଗ;
	}
    ଫେରନ୍ତୁ FALSE;
}

/*
 * Return TRUE when 'mousemodel' is set to "popup" or "popup_setpos".
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mouse_model_popup(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ (p_mousem[0] == 'p');
}

/*
 * Call a click-region callback function.
 * "regions"/"region_count" describe the resolved click regions,
 * "winid" is stored as the "winid" key in the info dict (0 for tabline).
 * Returns TRUE if the function was called and handled the click.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
stl_click_handler_regions(
	stl_click_region_T  *regions,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    region_ଗଣନା,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    winid,
	char_u		    *area_name,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    mrow,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    mcol,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    which_button,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    mods)
{
#ifdef FEAT_EVAL
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		n;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		nclicks;
    char_u	button_str[2];
    char_u	mods_str[4];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		mi = 0;
    dict_T	*info;
    typval_T	argvars[2];
    typval_T	rettv;
    funcexe_T	funcexe;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		col = mcol;

    ଯଦି (regions == ଖାଲି || region_ଗଣନା == 0)
	ଫେରନ୍ତୁ FALSE;

    // Find the click region at the given row and column.
    ପାଇଁ (n = 0; n < region_ଗଣନା; n++)
    {
	ଯଦି (regions[n].row == mrow
		&& col >= regions[n].col_start
		&& col < regions[n].col_end)
	    ଭାଙ୍ଗ;
    }
    ଯଦି (n >= region_ଗଣନା || regions[n].funcname == ଖାଲି)
	ଫେରନ୍ତୁ FALSE;

    // Build the info dictionary.
    info = dict_alloc();
    ଯଦି (info == ଖାଲି)
	ଫେରନ୍ତୁ FALSE;

    dict_add_ସଂଖ୍ୟା(info, "minwid", regions[n].minwid);

    // Determine number of clicks.
    // MOD_MASK_2CLICK=0x20, MOD_MASK_3CLICK=0x40, MOD_MASK_4CLICK=0x60
    nclicks = ((mods & MOD_MASK_MULTI_CLICK) >> 5) + 1;
    ଯଦି (nclicks > 3)
	nclicks = 3;
    dict_add_ସଂଖ୍ୟା(info, "nclicks", nclicks);

    // Button.
    ଯଦି (which_button == MOUSE_ବାମ)
	button_str[0] = 'l';
    ଅନ୍ୟଥା ଯଦି (which_button == MOUSE_ଡାହାଣ)
	button_str[0] = 'r';
    ଅନ୍ୟଥା
	button_str[0] = 'm';
    button_str[1] = NUL;
    dict_add_string_len(info, "button", button_str, 1);

    // Modifiers.
    ଯଦି (mods & MOD_MASK_SHIFT)
	mods_str[mi++] = 's';
    ଯଦି (mods & MOD_MASK_CTRL)
	mods_str[mi++] = 'c';
    ଯଦି (mods & MOD_MASK_ALT)
	mods_str[mi++] = 'a';
    mods_str[mi] = NUL;
    dict_add_string_len(info, "mods", mods_str, mi);

    dict_add_ସଂଖ୍ୟା(info, "winid", winid);

    // "area": which option the clicked region belongs to.  Lets a shared
    // dispatcher distinguish 'statusline', 'tabline' and 'tabpanel' without
    // having to overload winid == 0.
    dict_add_string(info, "area", area_name);

    // Expose tab page number for 'tabpanel' regions.
    ଯଦି (regions[n].tabnr > 0)
	dict_add_ସଂଖ୍ୟା(info, "tabnr", regions[n].tabnr);

    // Call the function with the info dict as argument.
    argvars[0].v_type = VAR_DICT;
    argvars[0].vval.v_dict = info;
    ++info->dv_refcount;
    argvars[1].v_type = VAR_UNKNOWN;

    rettv.v_type = VAR_ସଂଖ୍ୟା;
    rettv.vval.v_ସଂଖ୍ୟା = 0;

    CLEAR_FIELD(funcexe);
    funcexe.fe_evaluate = TRUE;
    (ଶୂନ୍ୟ)call_func(regions[n].funcname, -1,
					    &rettv, 1, argvars, &funcexe);

    n = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)rettv.vval.v_ସଂଖ୍ୟା;
    clear_tv(&rettv);
    dict_unref(info);

    ଯଦି (n != 0)
    {
	// Make sure the tabline gets redrawn too when the callback asks for
	// a redraw (redraw_statuslines() only redraws the tabline when
	// redraw_tabline is set).  For tabpanel the whole screen needs to be
	// refreshed.
	ଯଦି (winid == 0)
	    redraw_tabline = TRUE;
# ifdef FEAT_TABPANEL
	ଯଦି (STRCMP(area_name, "tabpanel") == 0)
	    redraw_all_later(UPD_NOT_VALID);
# endif
	redraw_statuslines();
    }

    ଫେରନ୍ତୁ TRUE;
#ଅନ୍ୟଥା
    (ଶୂନ୍ୟ)regions;
    (ଶୂନ୍ୟ)region_ଗଣନା;
    (ଶୂନ୍ୟ)winid;
    (ଶୂନ୍ୟ)area_name;
    (ଶୂନ୍ୟ)mrow;
    (ଶୂନ୍ୟ)mcol;
    (ଶୂନ୍ୟ)which_button;
    (ଶୂନ୍ୟ)mods;
    ଫେରନ୍ତୁ FALSE;
#endif
}

/*
 * Call a statusline click handler function for window "wp".
 * Returns TRUE if the function was called and handled the click.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
stl_click_handler(win_T *wp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା mrow, ପୂର୍ଣ୍ଣସଂଖ୍ୟା mcol, ପୂର୍ଣ୍ଣସଂଖ୍ୟା which_button, ପୂର୍ଣ୍ଣସଂଖ୍ୟା mods)
{
    ଯଦି (wp == ଖାଲି)
	ଫେରନ୍ତୁ FALSE;
    ଫେରନ୍ତୁ stl_click_handler_regions(wp->w_stl_click, wp->w_stl_click_ଗଣନା,
				wp->w_id, (char_u *)"statusline",
				mrow, mcol, which_button, mods);
}

// dragwin is declared near the top of the file

/*
 * Reset the window being dragged.  To be called when switching tab page.
 */
    ଶୂନ୍ୟ
reset_dragwin(ଶୂନ୍ୟ)
{
    dragwin = ଖାଲି;
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
jump_to_mouse(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		flags,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*inclusive,	// used for inclusive operator, can be NULL
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		which_button)	// MOUSE_LEFT, MOUSE_RIGHT, MOUSE_MIDDLE
{
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	on_status_ଧାଡ଼ି = 0;	// #lines below bottom of window
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	on_sep_ଧାଡ଼ି = 0;	// on separator right of window
#ifdef FEAT_MENU
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା  in_winbar = FALSE;
#endif
#ifdef FEAT_PROP_POPUP
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା   in_popup_win = FALSE;
    ସ୍ଥିର win_T *click_in_popup_win = ଖାଲି;
#endif
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	prev_row = -1;
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	prev_col = -1;
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	did_drag = FALSE;	// drag was noticed

    win_T	*wp, *old_curwin;
    pos_T	old_cursor;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ଗଣନା_2;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		first;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		row = mouse_row;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		col = mouse_col;
    colnr_T	col_from_screen = -1;
#ifdef FEAT_FOLDING
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		mouse_char = ' ';
#endif

    mouse_past_bottom = FALSE;
    mouse_past_eol = FALSE;

    ଯଦି (flags & MOUSE_RELEASED)
    {
	// On button release we may change window focus if positioned on a
	// status line and no dragging happened.
	ଯଦି (dragwin != ଖାଲି && !did_drag)
	    flags &= ~(MOUSE_FOCUS | MOUSE_DID_MOVE);
	dragwin = ଖାଲି;
	did_drag = FALSE;
#ifdef FEAT_PROP_POPUP
	ଯଦି (click_in_popup_win != ଖାଲି && popup_dragwin == ଖାଲି)
	    popup_close_for_mouse_click(click_in_popup_win);

	popup_dragwin = ଖାଲି;
	click_in_popup_win = ଖାଲି;
#endif
    }

    ଯଦି ((flags & MOUSE_DID_MOVE)
	    && prev_row == mouse_row
	    && prev_col == mouse_col)
    {
retnomove:
	// before moving the cursor for a left click which is NOT in a status
	// line, stop Visual mode
	ଯଦି (on_status_ଧାଡ଼ି)
	    ଫେରନ୍ତୁ IN_STATUS_ଧାଡ଼ି;
	ଯଦି (on_sep_ଧାଡ଼ି)
	    ଫେରନ୍ତୁ IN_SEP_ଧାଡ଼ି;
#ifdef FEAT_MENU
	ଯଦି (in_winbar)
	{
	    // A quick second click may arrive as a double-click, but we use it
	    // as a second click in the WinBar.
	    ଯଦି ((mod_mask & MOD_MASK_MULTI_CLICK) && !(flags & MOUSE_RELEASED))
	    {
		wp = mouse_find_win(&row, &col, FAIL_POPUP);
		ଯଦି (wp == ଖାଲି)
		    ଫେରନ୍ତୁ IN_UNKNOWN;
		winbar_click(wp, col);
	    }
	    ଫେରନ୍ତୁ IN_OTHER_WIN | MOUSE_WINBAR;
	}
#endif
	ଯଦି (flags & MOUSE_MAY_STOP_VIS)
	{
	    end_visual_mode_keep_button();
	    redraw_curbuf_later(UPD_INVERTED);	// delete the inversion
	}
#ଯଦି defined(FEAT_CLIPBOARD)
	// Continue a modeless selection in another window.
	ଯଦି (cmdwin_type != 0 && row < cmdwin_win->w_winrow)
	    ଫେରନ୍ତୁ IN_OTHER_WIN;
#endif
#ifdef FEAT_PROP_POPUP
	// Continue a modeless selection in a popup window or dragging it.
	ଯଦି (in_popup_win)
	{
	    click_in_popup_win = ଖାଲି;  // don't close it on release
	    ଯଦି (popup_dragwin != ଖାଲି)
	    {
		// dragging a popup window
		popup_drag(popup_dragwin);
		ଫେରନ୍ତୁ IN_UNKNOWN;
	    }
	    ଫେରନ୍ତୁ IN_OTHER_WIN;
	}
#endif
	ଫେରନ୍ତୁ IN_BUFFER;
    }

    prev_row = mouse_row;
    prev_col = mouse_col;

    ଯଦି (flags & MOUSE_SETPOS)
	ଯାଅ retnomove;				// ugly goto...

    old_curwin = curwin;
    old_cursor = curwin->w_cursor;

    ଯଦି (!(flags & MOUSE_FOCUS))
    {
	ଯଦି (row < 0 || col < 0) // check if it makes sense
	    ଫେରନ୍ତୁ IN_UNKNOWN;

	// find the window where the row is in and adjust "row" and "col" to be
	// relative to top-left of the window
	wp = mouse_find_win(&row, &col, FIND_POPUP);
	ଯଦି (wp == ଖାଲି)
	    ଫେରନ୍ତୁ IN_UNKNOWN;
	dragwin = ଖାଲି;

#ifdef FEAT_PROP_POPUP
	// Click in a popup window may start dragging or modeless selection,
	// but not much else.
	ଯଦି (WIN_IS_POPUP(wp))
	{
	    on_sep_ଧାଡ଼ି = 0;
	    on_status_ଧାଡ଼ି = 0;
	    in_popup_win = TRUE;
	    ଯଦି (which_button == MOUSE_ବାମ && popup_close_if_on_X(wp, row, col))
	    {
		ଫେରନ୍ତୁ IN_UNKNOWN;
	    }
	    ଅନ୍ୟଥା ଯଦି (((wp->w_popup_flags & (POPF_DRAG | POPF_RESIZE))
					      && popup_on_border(wp, row, col))
				       || (wp->w_popup_flags & POPF_DRAGALL))
	    {
		popup_dragwin = wp;
		popup_start_drag(wp, row, col);
		ଫେରନ୍ତୁ IN_UNKNOWN;
	    }
	    // Only close on release, otherwise it's not possible to drag or do
	    // modeless selection.
	    ଅନ୍ୟଥା ଯଦି (wp->w_popup_close == POPCLOSE_CLICK
		    && which_button == MOUSE_ବାମ)
	    {
		click_in_popup_win = wp;
	    }
	    ଅନ୍ୟଥା ଯଦି (which_button == MOUSE_ବାମ)
		// If the click is in the scrollbar, may scroll up/down.
		popup_handle_scrollbar_click(wp, row, col);
# ifdef FEAT_CLIPBOARD
	    ଫେରନ୍ତୁ IN_OTHER_WIN;
# ଅନ୍ୟଥା
	    ଫେରନ୍ତୁ IN_UNKNOWN;
# endif
	}
	in_popup_win = FALSE;
	popup_dragwin = ଖାଲି;
#endif
#ifdef FEAT_MENU
	ଯଦି (row == -1)
	{
	    // A click in the window toolbar does not enter another window or
	    // change Visual highlighting.
	    winbar_click(wp, col);
	    in_winbar = TRUE;
	    ଫେରନ୍ତୁ IN_OTHER_WIN | MOUSE_WINBAR;
	}
	in_winbar = FALSE;
#endif

	// winpos and height may change in win_enter()!
	ଯଦି (row >= wp->w_height)		// In (or below) status line
	{
	    on_status_ଧାଡ଼ି = row - wp->w_height + 1;
	    dragwin = wp;
	}
	ଅନ୍ୟଥା
	    on_status_ଧାଡ଼ି = 0;
	ଯଦି (col >= wp->w_width)		// In separator line
	{
	    on_sep_ଧାଡ଼ି = col - wp->w_width + 1;
	    dragwin = wp;
	}
	ଅନ୍ୟଥା
	    on_sep_ଧାଡ଼ି = 0;

	// The rightmost character of the status line might be a vertical
	// separator character if there is no connecting window to the right.
	ଯଦି (on_status_ଧାଡ଼ି && on_sep_ଧାଡ଼ି)
	{
	    ଯଦି (stl_connected(wp))
		on_sep_ଧାଡ଼ି = 0;
	    ଅନ୍ୟଥା
		on_status_ଧାଡ଼ି = 0;
	}

	// Before jumping to another buffer, or moving the cursor for a left
	// click, stop Visual mode.
	ଯଦି (VIsual_active
		&& (wp->w_buffer != curwin->w_buffer
		    || (!on_status_ଧାଡ଼ି && !on_sep_ଧାଡ଼ି
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
	ଯଦି (cmdwin_type != 0 && wp != cmdwin_win)
	{
	    // A click outside the command-line window: Use modeless
	    // selection if possible.  Allow dragging the status lines.
	    on_sep_ଧାଡ଼ି = 0;
#ifdef FEAT_CLIPBOARD
	    ଯଦି (on_status_ଧାଡ଼ି)
		ଫେରନ୍ତୁ IN_STATUS_ଧାଡ଼ି;
	    ଫେରନ୍ତୁ IN_OTHER_WIN;
#ଅନ୍ୟଥା
	    row = 0;
	    col += wp->w_wincol;
	    wp = cmdwin_win;
#endif
	}
#ଯଦି defined(FEAT_PROP_POPUP) && defined(FEAT_TERMINAL)
	ଯଦି (popup_is_popup(curwin) && curbuf->b_term != ଖାଲି)
	    // terminal in popup window: don't jump to another window
	    ଫେରନ୍ତୁ IN_OTHER_WIN;
#endif
	// Only change window focus when not clicking on or dragging the
	// status line.  Do change focus when releasing the mouse button
	// (MOUSE_FOCUS was set above if we dragged first).
	ଯଦି (dragwin == ଖାଲି || (flags & MOUSE_RELEASED))
	    win_enter(wp, TRUE);		// can make wp invalid!

	ଯଦି (curwin != old_curwin)
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
	ଯଦି (on_status_ଧାଡ଼ି)			// In (or below) status line
	{
	    // Don't use start_arrow() if we're in the same window
	    ଯଦି (curwin == old_curwin)
		ଫେରନ୍ତୁ IN_STATUS_ଧାଡ଼ି;
	    ଅନ୍ୟଥା
		ଫେରନ୍ତୁ IN_STATUS_ଧାଡ଼ି | CURSOR_MOVED;
	}
	ଯଦି (on_sep_ଧାଡ଼ି)			// In (or below) status line
	{
	    // Don't use start_arrow() if we're in the same window
	    ଯଦି (curwin == old_curwin)
		ଫେରନ୍ତୁ IN_SEP_ଧାଡ଼ି;
	    ଅନ୍ୟଥା
		ଫେରନ୍ତୁ IN_SEP_ଧାଡ଼ି | CURSOR_MOVED;
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
    ଅନ୍ୟଥା ଯଦି (on_status_ଧାଡ଼ି && which_button == MOUSE_ବାମ)
    {
	ଯଦି (dragwin != ଖାଲି)
	{
	    // Drag the status line
	    ଗଣନା_2 = row - W_WINROW(dragwin) - dragwin->w_height + 1
							     - on_status_ଧାଡ଼ି;
	    win_drag_status_ଧାଡ଼ି(dragwin, ଗଣନା_2);
	    did_drag |= ଗଣନା_2;
	}
	ଫେରନ୍ତୁ IN_STATUS_ଧାଡ଼ି;			// Cursor didn't move
    }
    ଅନ୍ୟଥା ଯଦି (on_sep_ଧାଡ଼ି && which_button == MOUSE_ବାମ)
    {
	ଯଦି (dragwin != ଖାଲି)
	{
	    // Drag the separator column
	    ଗଣନା_2 = col - dragwin->w_wincol - dragwin->w_width + 1
								- on_sep_ଧାଡ଼ି;
	    win_drag_vsep_ଧାଡ଼ି(dragwin, ଗଣନା_2);
	    did_drag |= ଗଣନା_2;
	}
	ଫେରନ୍ତୁ IN_SEP_ଧାଡ଼ି;			// Cursor didn't move
    }
#ifdef FEAT_MENU
    ଅନ୍ୟଥା ଯଦି (in_winbar)
    {
	// After a click on the window toolbar don't start Visual mode.
	ଫେରନ୍ତୁ IN_OTHER_WIN | MOUSE_WINBAR;
    }
#endif
    ଅନ୍ୟଥା // keep_window_focus must be TRUE
    {
	// before moving the cursor for a left click, stop Visual mode
	ଯଦି (flags & MOUSE_MAY_STOP_VIS)
	{
	    end_visual_mode_keep_button();
	    redraw_curbuf_later(UPD_INVERTED);	// delete the inversion
	}

#ଯଦି defined(FEAT_CLIPBOARD)
	// Continue a modeless selection in another window.
	ଯଦି (cmdwin_type != 0 && row < cmdwin_win->w_winrow)
	    ଫେରନ୍ତୁ IN_OTHER_WIN;
#endif
#ifdef FEAT_PROP_POPUP
	ଯଦି (in_popup_win)
	{
	    ଯଦି (popup_dragwin != ଖାଲି)
	    {
		// dragging a popup window
		popup_drag(popup_dragwin);
		ଫେରନ୍ତୁ IN_UNKNOWN;
	    }
	    // continue a modeless selection in a popup window
	    click_in_popup_win = ଖାଲି;
	    ଫେରନ୍ତୁ IN_OTHER_WIN;
	}
#endif

	row -= W_WINROW(curwin);
	col -= curwin->w_wincol;

	// When clicking beyond the end of the window, scroll the screen.
	// Scroll by however many rows outside the window we are.
	ଯଦି (row < 0)
	{
	    ଗଣନା_2 = 0;
	    ପାଇଁ (first = TRUE; curwin->w_topline > 1; )
	    {
#ifdef FEAT_DIFF
		ଯଦି (curwin->w_topfill < diff_check_fill(curwin, curwin->w_topline))
		    ++ଗଣନା_2;
		ଅନ୍ୟଥା
#endif
		    ଗଣନା_2 += plines(curwin->w_topline - 1);
		ଯଦି (!first && ଗଣନା_2 > -row)
		    ଭାଙ୍ଗ;
		first = FALSE;
#ifdef FEAT_FOLDING
		(ଶୂନ୍ୟ)hasFolding(curwin->w_topline, &curwin->w_topline, ଖାଲି);
#endif
#ifdef FEAT_DIFF
		ଯଦି (curwin->w_topfill < diff_check_fill(curwin, curwin->w_topline))
		    ++curwin->w_topfill;
		ଅନ୍ୟଥା
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
	ଅନ୍ୟଥା ଯଦି (row >= curwin->w_height)
	{
	    ଗଣନା_2 = 0;
	    ପାଇଁ (first = TRUE; curwin->w_topline < curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା; )
	    {
#ifdef FEAT_DIFF
		ଯଦି (curwin->w_topfill > 0)
		    ++ଗଣନା_2;
		ଅନ୍ୟଥା
#endif
		    ଗଣନା_2 += plines(curwin->w_topline);
		ଯଦି (!first && ଗଣନା_2 > row - curwin->w_height + 1)
		    ଭାଙ୍ଗ;
		first = FALSE;
#ifdef FEAT_FOLDING
		ଯଦି (hasFolding(curwin->w_topline, ଖାଲି, &curwin->w_topline)
			&& curwin->w_topline == curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା)
		    ଭାଙ୍ଗ;
#endif
#ifdef FEAT_DIFF
		ଯଦି (curwin->w_topfill > 0)
		    --curwin->w_topfill;
		ଅନ୍ୟଥା
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
	ଅନ୍ୟଥା ଯଦି (row == 0)
	{
	    // When dragging the mouse, while the text has been scrolled up as
	    // far as it goes, moving the mouse in the top line should scroll
	    // the text down (done later when recomputing w_topline).
	    ଯଦି (mouse_dragging > 0
		    && curwin->w_cursor.lnum
				       == curwin->w_buffer->b_ml.ml_ଧାଡ଼ି_ଗଣନା
		    && curwin->w_cursor.lnum == curwin->w_topline)
		curwin->w_valid &= ~(VALID_TOPLINE);
	}
    }

    ଯଦି (prev_row >= W_WINROW(curwin)
	&& prev_row < W_WINROW(curwin) + curwin->w_height
	&& prev_col >= curwin->w_wincol && prev_col < W_ENDCOL(curwin)
						       && Screen_ଧାଡ଼ିଗୁଡ଼ିକ != ଖାଲି)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା off = ଧାଡ଼ି_Offset[prev_row] + prev_col;

	// Only use ScreenCols[] after the window was redrawn.  Mainly matters
	// for tests, a user would not click before redrawing.
	// Do not use when 'virtualedit' is active.
	ଯଦି (curwin->w_redr_type <= UPD_VALID_NO_UPDATE)
	    col_from_screen = ScreenCols[off];
#ifdef FEAT_FOLDING
	// Remember the character under the mouse, it might be a '-' or '+' in
	// the fold column.
	mouse_char = enc_utf8 && Screen_ଧାଡ଼ିଗୁଡ଼ିକ_UC[off] != 0
				       ? Screen_ଧାଡ଼ିଗୁଡ଼ିକ_UC[off] : Screen_ଧାଡ଼ିଗୁଡ଼ିକ[off];
#endif
    }

#ifdef FEAT_FOLDING
    // Check for position outside of the fold column.
    ଯଦି (
# ifdef FEAT_RIGHTLEFT
	    curwin->w_p_rl ? col < curwin->w_width - curwin->w_p_fdc :
# endif
	    col >= curwin->w_p_fdc + (cmdwin_win != curwin ? 0 : 1)
       )
	mouse_char = ' ';
#endif

    // compute the position in the buffer line from the posn on the screen
    ଯଦି (mouse_comp_pos(curwin, &row, &col, &curwin->w_cursor.lnum, ଖାଲି))
	mouse_past_bottom = TRUE;

    // Start Visual mode before coladvance(), for when 'sel' != "old"
    ଯଦି ((flags & MOUSE_MAY_VIS) && !VIsual_active)
    {
	check_visual_highlight();
	VIsual = old_cursor;
	VIsual_active = TRUE;
	VIsual_reselect = TRUE;
	// if 'selectmode' contains "mouse", start Select mode
	may_start_select('o');
	setmouse();
	ଯଦି (p_smd && ସନ୍ଦେଶ_silent == 0)
	    redraw_cmdline = TRUE;	// show visual mode later
    }

    ଯଦି (col_from_screen >= 0)
    {
	// Use the virtual column from ScreenCols[], it is accurate also after
	// concealed characters.
	col = col_from_screen;
    }

    curwin->w_curswant = col;
    curwin->w_set_curswant = false;	// May still have been TRUE
    ଯଦି (coladvance(col) == FAIL)	// Mouse click beyond end of line
    {
	ଯଦି (inclusive != ଖାଲି)
	    *inclusive = TRUE;
	mouse_past_eol = TRUE;
    }
    ଅନ୍ୟଥା ଯଦି (inclusive != ଖାଲି)
	*inclusive = FALSE;

    ଗଣନା_2 = IN_BUFFER;
    ଯଦି (curwin != old_curwin || curwin->w_cursor.lnum != old_cursor.lnum
	    || curwin->w_cursor.col != old_cursor.col)
	ଗଣନା_2 |= CURSOR_MOVED;		// Cursor has moved

#ifdef FEAT_FOLDING
    ଯଦି (mouse_char == curwin->w_fill_chars.foldclosed)
	ଗଣନା_2 |= MOUSE_FOLD_OPEN;
    ଅନ୍ୟଥା ଯଦି (mouse_char != ' ')
	ଗଣନା_2 |= MOUSE_FOLD_CLOSE;
#endif

    ଫେରନ୍ତୁ ଗଣନା_2;
}

/*
 * Make a horizontal scroll to "leftcol".
 * Return TRUE if the cursor moved, FALSE otherwise.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
do_mousescroll_horiz(long_u leftcol)
{
    ଯଦି (curwin->w_p_wrap)
	ଫେରନ୍ତୁ FALSE;  // no horizontal scrolling when wrapping

    ଯଦି (curwin->w_leftcol == (colnr_T)leftcol)
	ଫେରନ୍ତୁ FALSE;  // already there

    // When the line of the cursor is too short, move the cursor to the
    // longest visible line.
    ଯଦି (
#ifdef FEAT_GUI
	    (!gui.in_use || vim_strchr(p_go, GO_HORSCROLL) == ଖାଲି) &&
#endif
		    !virtual_active()
	    && (long)leftcol > scroll_ଧାଡ଼ି_len(curwin->w_cursor.lnum))
    {
	curwin->w_cursor.lnum = ui_find_longest_lnum();
	curwin->w_cursor.col = 0;
    }

    ଫେରନ୍ତୁ set_leftcol((colnr_T)leftcol);
}

/*
 * Normal and Visual modes implementation for scrolling in direction
 * "cap->arg", which is one of the MSCR_ values.
 */
    ଶୂନ୍ୟ
nv_mousescroll(cmdarg_T *cap)
{
    win_T   *old_curwin = curwin;

#ifdef FEAT_TABPANEL
    ଯଦି (mouse_row >= 0 && mouse_col >= 0
	    && (cap->arg == MSCR_UP || cap->arg == MSCR_DOWN)
	    && mouse_on_tabpanel())
    {
	(ଶୂନ୍ୟ)tabpanel_scroll(cap->arg == MSCR_UP ? 1 : -1,
		mouse_vert_step > 0 ? mouse_vert_step : 3);
	ଫେରନ୍ତୁ;
    }
#endif

    ଯଦି (mouse_row >= 0 && mouse_col >= 0)
    {
	// Find the window at the mouse pointer coordinates.
	// NOTE: Must restore "curwin" to "old_curwin" before returning!
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା row = mouse_row;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା col = mouse_col;
	curwin = mouse_find_win(&row, &col, FIND_POPUP);
	ଯଦି (curwin == ଖାଲି)
	{
	    curwin = old_curwin;
	    ଫେରନ୍ତୁ;
	}

#ifdef FEAT_PROP_POPUP
	ଯଦି (WIN_IS_POPUP(curwin) && !curwin->w_has_scrollbar)
	{
	    // cannot scroll this popup window
	    curwin = old_curwin;
	    ଫେରନ୍ତୁ;
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
    ଶୂନ୍ୟ
nv_mouse(cmdarg_T *cap)
{
    (ଶୂନ୍ୟ)do_mouse(cap->oap, cap->cmdchar, BACKWARD, cap->count1, 0);
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	held_button = MOUSE_RELEASE;

    ଶୂନ୍ୟ
reset_held_button(ଶୂନ୍ୟ)
{
    held_button = MOUSE_RELEASE;
}

/*
 * Check if typebuf 'tp' contains a terminal mouse code and returns the
 * modifiers found in typebuf in 'modifiers'.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
check_termcode_mouse(
    char_u	*tp,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*slen,
    char_u	*ଚାବି_name,
    char_u	*modifiers_start,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		idx,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*modifiers)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		j;
    char_u	*p;
#ଯଦି !defined(UNIX) || defined(FEAT_MOUSE_XTERM) || defined(FEAT_GUI) \
    || defined(FEAT_MOUSE_GPM) || defined(FEAT_SYSMOUSE)
    char_u	bytes[6];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ଅଙ୍କ_bytes;
#endif
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		mouse_code = 0;	    // init for GCC
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		is_click, is_drag;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		is_release, release_is_ambiguous;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		wheel_code = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		current_button;
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	orig_ଅଙ୍କ_clicks = 1;
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	orig_mouse_code = 0x0;
#ifdef CHECK_DOUBLE_CLICK
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	orig_mouse_col = 0;
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	orig_mouse_row = 0;
    ସ୍ଥିର ସଂରଚନା timeval  orig_mouse_time = {0, 0};
    // time of previous mouse click
    ସଂରଚନା timeval  mouse_time;		// time of current mouse click
    long	timediff;		// elapsed time in msec
#endif

    is_click = is_drag = is_release = release_is_ambiguous = FALSE;

#ଯଦି !defined(UNIX) || defined(FEAT_MOUSE_XTERM) || defined(FEAT_GUI) \
    || defined(FEAT_MOUSE_GPM) || defined(FEAT_SYSMOUSE)
    ଯଦି (ଚାବି_name[0] == KS_MOUSE
# ifdef FEAT_MOUSE_GPM
	    || ଚାବି_name[0] == KS_GPM_MOUSE
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
	ପାଇଁ (;;)
	{
	    // For the GUI and for MS-Windows two bytes each are used for row
	    // and column.  Allows for more than 223 columns.
# ଯଦି defined(FEAT_GUI) || defined(MSWIN)
	    ଯଦି (TRUE
#  ଯଦି defined(FEAT_GUI) && !defined(MSWIN)
		&& gui.in_use
#  endif
		)
	    {
		ଅଙ୍କ_bytes = get_bytes_from_buf(tp + *slen, bytes, 5);
		ଯଦି (ଅଙ୍କ_bytes == -1)	// not enough coordinates
		    ଫେରନ୍ତୁ -1;
		mouse_code = bytes[0];
		mouse_col = 128 * (bytes[1] - ' ' - 1)
		    + bytes[2] - ' ' - 1;
		mouse_row = 128 * (bytes[3] - ' ' - 1)
		    + bytes[4] - ' ' - 1;
	    }
	    ଅନ୍ୟଥା
# endif
	    {
		ଅଙ୍କ_bytes = get_bytes_from_buf(tp + *slen, bytes, 3);
		ଯଦି (ଅଙ୍କ_bytes == -1)	// not enough coordinates
		    ଫେରନ୍ତୁ -1;
		mouse_code = bytes[0];
		mouse_col = bytes[1] - ' ' - 1;
		mouse_row = bytes[2] - ' ' - 1;
	    }
	    *slen += ଅଙ୍କ_bytes;

	    // If the following bytes is also a mouse code and it has the same
	    // code, dump this one and get the next.  This makes dragging a
	    // whole lot faster.
# ifdef FEAT_GUI
	    ଯଦି (gui.in_use)
		j = 3;
	    ଅନ୍ୟଥା
# endif
		j = get_termcode_len(idx);
	    ଯଦି (STRNCMP(tp, tp + *slen, (size_t)j) == 0
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
	    ଅନ୍ୟଥା
		ଭାଙ୍ଗ;
	}
    }

    ଯଦି (ଚାବି_name[0] == KS_URXVT_MOUSE
	    || ଚାବି_name[0] == KS_SGR_MOUSE
	    || ଚାବି_name[0] == KS_SGR_MOUSE_RELEASE)
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
	ଯଦି (p == ଖାଲି)
	    ଫେରନ୍ତୁ -1;

	mouse_code = getdigits(&p);
	ଯଦି (*p++ != ';')
	    ଫେରନ୍ତୁ -1;

	// when mouse reporting is SGR, add 32 to mouse code
	ଯଦି (ଚାବି_name[0] == KS_SGR_MOUSE
		|| ଚାବି_name[0] == KS_SGR_MOUSE_RELEASE)
	    mouse_code += 32;

	mouse_col = getdigits(&p) - 1;
	ଯଦି (*p++ != ';')
	    ଫେରନ୍ତୁ -1;

	mouse_row = getdigits(&p) - 1;

	// The modifiers were the mouse coordinates, not the modifier keys
	// (alt/shift/ctrl/meta) state.
	*modifiers = 0;
    }

    ଯଦି (ଚାବି_name[0] == KS_SGR_MOUSE
	    || ଚାବି_name[0] == KS_SGR_MOUSE_RELEASE)
    {
	ଯଦି (ଚାବି_name[0] == KS_SGR_MOUSE_RELEASE)
	{
	    is_release = TRUE;
	    // This is used below to set held_button.
	    mouse_code |= MOUSE_RELEASE;
	}
    }
    ଅନ୍ୟଥା
    {
	release_is_ambiguous = TRUE;
	ଯଦି ((mouse_code & MOUSE_RELEASE) == MOUSE_RELEASE)
	    is_release = TRUE;
    }

    ଯଦି (ଚାବି_name[0] == KS_MOUSE
# ifdef FEAT_MOUSE_GPM
	    || ଚାବି_name[0] == KS_GPM_MOUSE
# endif
# ifdef FEAT_MOUSE_URXVT
	    || ଚାବି_name[0] == KS_URXVT_MOUSE
# endif
	    || ଚାବି_name[0] == KS_SGR_MOUSE
	    || ଚାବି_name[0] == KS_SGR_MOUSE_RELEASE)
    {
# ଯଦି !defined(MSWIN)
	/*
	 * Handle old style mouse events.
	 * Recognize the xterm mouse wheel, but not in the GUI, the
	 * Linux console with GPM and the MS-DOS or Win32 console
	 * (multi-clicks use >= 0x60).
	 */
	ଯଦି (mouse_code >= MOUSEWHEEL_ତଳ && mouse_code < MOUSESIDEBUTTONS_ତଳ
#  ifdef FEAT_GUI
		&& !gui.in_use
#  endif
#  ifdef FEAT_MOUSE_GPM
		&& ଚାବି_name[0] != KS_GPM_MOUSE
#  endif
	   )
	{
#  ଯଦି defined(UNIX)
	    ଯଦି (use_xterm_mouse() > 1 && mouse_code >= 0x80)
		// mouse-move event, using MOUSE_DRAG works
		mouse_code = MOUSE_DRAG;
	    ଅନ୍ୟଥା
#  endif
		// Keep the mouse_code before it's changed, so that we
		// remember that it was a mouse wheel click.
		wheel_code = mouse_code;
	}
#  ifdef FEAT_MOUSE_XTERM
	ଅନ୍ୟଥା ଯଦି (held_button == MOUSE_RELEASE
#   ifdef FEAT_GUI
		&& !gui.in_use
#   endif
		&& (mouse_code == 0x23 || mouse_code == 0x24
		    || mouse_code == 0x40 || mouse_code == 0x41))
	{
	    // Apparently 0x23 and 0x24 are used by rxvt scroll wheel.
	    // And 0x40 and 0x41 are used by some xterm emulator.
	    wheel_code = mouse_code - (mouse_code >= 0x40 ? 0x40 : 0x23)
							      + MOUSEWHEEL_ତଳ;
	}
#  endif

#  ଯଦି defined(UNIX)
	ଅନ୍ୟଥା ଯଦି (use_xterm_mouse() > 1)
	{
	    ଯଦି (mouse_code & MOUSE_DRAG_XTERM)
		mouse_code |= MOUSE_DRAG;
	}
#  endif
#  ifdef FEAT_XCLIPBOARD
	ଅନ୍ୟଥା ଯଦି (!(mouse_code & MOUSE_DRAG & ~MOUSE_CLICK_MASK))
	{
	    ଯଦି (is_release)
		stop_xterm_trace();
	    ଅନ୍ୟଥା
		start_xterm_trace(mouse_code);
	}
#  endif
# endif
    }
#endif // !UNIX || FEAT_MOUSE_XTERM
#ifdef FEAT_MOUSE_NET
    ଯଦି (ଚାବି_name[0] == KS_NETTERM_MOUSE)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା mc, mr;

	// expect a rather limited sequence like: balancing {
	// \033}6,45\r
	// '6' is the row, 45 is the column
	p = tp + *slen;
	mr = getdigits(&p);
	ଯଦି (*p++ != ',')
	    ଫେରନ୍ତୁ -1;
	mc = getdigits(&p);
	ଯଦି (*p++ != '\r')
	    ଫେରନ୍ତୁ -1;

	mouse_col = mc - 1;
	mouse_row = mr - 1;
	mouse_code = MOUSE_ବାମ;
	*slen += (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(p - (tp + *slen));
    }
#endif	// FEAT_MOUSE_NET
#ifdef FEAT_MOUSE_JSB
    ଯଦି (ଚାବି_name[0] == KS_JSBTERM_MOUSE)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା mult, val, iter, button, status;

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
	ବାଛ (*p++)
	{
	    ସ୍ଥିତି 'L': button = 1; ଭାଙ୍ଗ;
	    ସ୍ଥିତି '-': ଭାଙ୍ଗ;
	    ସ୍ଥିତି 'x': ଭାଙ୍ଗ; // ignore sequence
	    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:  ଫେରନ୍ତୁ -1; // Unknown Result
	}
	ବାଛ (*p++)
	{
	    ସ୍ଥିତି 'M': button |= 2; ଭାଙ୍ଗ;
	    ସ୍ଥିତି '-': ଭାଙ୍ଗ;
	    ସ୍ଥିତି 'x': ଭାଙ୍ଗ; // ignore sequence
	    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:  ଫେରନ୍ତୁ -1; // Unknown Result
	}
	ବାଛ (*p++)
	{
	    ସ୍ଥିତି 'R': button |= 4; ଭାଙ୍ଗ;
	    ସ୍ଥିତି '-': ଭାଙ୍ଗ;
	    ସ୍ଥିତି 'x': ଭାଙ୍ଗ; // ignore sequence
	    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:  ଫେରନ୍ତୁ -1; // Unknown Result
	}
	status = *p++;
	ପାଇଁ (val = 0, mult = 100, iter = 0; iter < 3; iter++,
		mult /= 10, p++)
	    ଯଦି (*p >= '0' && *p <= '9')
		val += (*p - '0') * mult;
	    ଅନ୍ୟଥା
		ଫେରନ୍ତୁ -1;
	mouse_col = val;
	ପାଇଁ (val = 0, mult = 100, iter = 0; iter < 3; iter++,
		mult /= 10, p++)
	    ଯଦି (*p >= '0' && *p <= '9')
		val += (*p - '0') * mult;
	    ଅନ୍ୟଥା
		ଫେରନ୍ତୁ -1;
	mouse_row = val;
	ବାଛ (*p++)
	{
	    ସ୍ଥିତି 's': button |= 8; ଭାଙ୍ଗ;  // SHIFT key Pressed
	    ସ୍ଥିତି '-': ଭାଙ୍ଗ;  // Not Pressed
	    ସ୍ଥିତି 'x': ଭାଙ୍ଗ;  // Not Reporting
	    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:  ଫେରନ୍ତୁ -1; // Unknown Result
	}
	ବାଛ (*p++)
	{
	    ସ୍ଥିତି 'c': button |= 16; ଭାଙ୍ଗ;  // CTRL key Pressed
	    ସ୍ଥିତି '-': ଭାଙ୍ଗ;  // Not Pressed
	    ସ୍ଥିତି 'x': ଭାଙ୍ଗ;  // Not Reporting
	    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:  ଫେରନ୍ତୁ -1; // Unknown Result
	}
	ଯଦି (*p++ != '\033')
	    ଫେରନ୍ତୁ -1;
	ଯଦି (*p++ != '\\')
	    ଫେରନ୍ତୁ -1;
	ବାଛ (status)
	{
	    ସ୍ଥିତି 'D': // Double Click
	    ସ୍ଥିତି 'S': // Single Click
		ଯଦି (button & 1) mouse_code |= MOUSE_ବାମ;
		ଯଦି (button & 2) mouse_code |= MOUSE_ମଧ୍ୟ;
		ଯଦି (button & 4) mouse_code |= MOUSE_ଡାହାଣ;
		ଯଦି (button & 8) mouse_code |= MOUSE_SHIFT;
		ଯଦି (button & 16) mouse_code |= MOUSE_CTRL;
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି 'm': // Mouse move
		ଯଦି (button & 1) mouse_code |= MOUSE_ବାମ;
		ଯଦି (button & 2) mouse_code |= MOUSE_ମଧ୍ୟ;
		ଯଦି (button & 4) mouse_code |= MOUSE_ଡାହାଣ;
		ଯଦି (button & 8) mouse_code |= MOUSE_SHIFT;
		ଯଦି (button & 16) mouse_code |= MOUSE_CTRL;
		ଯଦି ((button & 7) != 0)
		{
		    held_button = mouse_code;
		    mouse_code |= MOUSE_DRAG;
		}
		is_drag = TRUE;
		showmode();
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି 'd': // Button Down
		ଯଦି (button & 1) mouse_code |= MOUSE_ବାମ;
		ଯଦି (button & 2) mouse_code |= MOUSE_ମଧ୍ୟ;
		ଯଦି (button & 4) mouse_code |= MOUSE_ଡାହାଣ;
		ଯଦି (button & 8) mouse_code |= MOUSE_SHIFT;
		ଯଦି (button & 16) mouse_code |= MOUSE_CTRL;
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି 'u': // Button Up
		is_release = TRUE;
		ଯଦି (button & 1)
		    mouse_code |= MOUSE_ବାମ;
		ଯଦି (button & 2)
		    mouse_code |= MOUSE_ମଧ୍ୟ;
		ଯଦି (button & 4)
		    mouse_code |= MOUSE_ଡାହାଣ;
		ଯଦି (button & 8)
		    mouse_code |= MOUSE_SHIFT;
		ଯଦି (button & 16)
		    mouse_code |= MOUSE_CTRL;
		ଭାଙ୍ଗ;
	    ପୂର୍ବନିର୍ଦ୍ଧାରିତ: ଫେରନ୍ତୁ -1; // Unknown Result
	}

	*slen += (p - (tp + *slen));
    }
#endif // FEAT_MOUSE_JSB
#ifdef FEAT_MOUSE_DEC
    ଯଦି (ଚାବି_name[0] == KS_DEC_MOUSE)
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
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା Pe, Pb, Pr, Pc;

	p = tp + *slen;

	// get event status
	Pe = getdigits(&p);
	ଯଦି (*p++ != ';')
	    ଫେରନ୍ତୁ -1;

	// get button status
	Pb = getdigits(&p);
	ଯଦି (*p++ != ';')
	    ଫେରନ୍ତୁ -1;

	// get row status
	Pr = getdigits(&p);
	ଯଦି (*p++ != ';')
	    ଫେରନ୍ତୁ -1;

	// get column status
	Pc = getdigits(&p);

	// the page parameter is optional
	ଯଦି (*p == ';')
	{
	    p++;
	    (ଶୂନ୍ୟ)getdigits(&p);
	}
	ଯଦି (*p++ != '&')
	    ଫେରନ୍ତୁ -1;
	ଯଦି (*p++ != 'w')
	    ଫେରନ୍ତୁ -1;

	mouse_code = 0;
	ବାଛ (Pe)
	{
	    ସ୍ଥିତି  0: ଫେରନ୍ତୁ -1; // position request while unavailable
	    ସ୍ଥିତି  1: // a response to a locator position request includes
		     //	the status of all buttons
		     Pb &= 7;   // mask off and ignore fourth button
		     ଯଦି (Pb & 4)
			 mouse_code  = MOUSE_ବାମ;
		     ଯଦି (Pb & 2)
			 mouse_code  = MOUSE_ମଧ୍ୟ;
		     ଯଦି (Pb & 1)
			 mouse_code  = MOUSE_ଡାହାଣ;
		     ଯଦି (Pb)
		     {
			 held_button = mouse_code;
			 mouse_code |= MOUSE_DRAG;
			 WantQueryMouse = TRUE;
		     }
		     is_drag = TRUE;
		     showmode();
		     ଭାଙ୍ଗ;
	    ସ୍ଥିତି  2: mouse_code = MOUSE_ବାମ;
		     WantQueryMouse = TRUE;
		     ଭାଙ୍ଗ;
	    ସ୍ଥିତି  3: mouse_code = MOUSE_ବାମ;
		     is_release = TRUE;
		     ଭାଙ୍ଗ;
	    ସ୍ଥିତି  4: mouse_code = MOUSE_ମଧ୍ୟ;
		     WantQueryMouse = TRUE;
		     ଭାଙ୍ଗ;
	    ସ୍ଥିତି  5: mouse_code = MOUSE_ମଧ୍ୟ;
		     is_release = TRUE;
		     ଭାଙ୍ଗ;
	    ସ୍ଥିତି  6: mouse_code = MOUSE_ଡାହାଣ;
		     WantQueryMouse = TRUE;
		     ଭାଙ୍ଗ;
	    ସ୍ଥିତି  7: mouse_code = MOUSE_ଡାହାଣ;
		     is_release = TRUE;
		     ଭାଙ୍ଗ;
	    ସ୍ଥିତି  8: ଫେରନ୍ତୁ -1; // fourth button down
	    ସ୍ଥିତି  9: ଫେରନ୍ତୁ -1; // fourth button up
	    ସ୍ଥିତି 10: ଫେରନ୍ତୁ -1; // mouse outside of filter rectangle
	    ପୂର୍ବନିର୍ଦ୍ଧାରିତ: ଫେରନ୍ତୁ -1; // should never occur
	}

	mouse_col = Pc - 1;
	mouse_row = Pr - 1;

	*slen += (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(p - (tp + *slen));
    }
#endif // FEAT_MOUSE_DEC
#ifdef FEAT_MOUSE_PTERM
    ଯଦି (ଚାବି_name[0] == KS_PTERM_MOUSE)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା button, ଅଙ୍କ_clicks, action;

	p = tp + *slen;

	action = getdigits(&p);
	ଯଦି (*p++ != ';')
	    ଫେରନ୍ତୁ -1;

	mouse_row = getdigits(&p);
	ଯଦି (*p++ != ';')
	    ଫେରନ୍ତୁ -1;
	mouse_col = getdigits(&p);
	ଯଦି (*p++ != ';')
	    ଫେରନ୍ତୁ -1;

	button = getdigits(&p);
	mouse_code = 0;

	ବାଛ (button)
	{
	    ସ୍ଥିତି 4: mouse_code = MOUSE_ବାମ; ଭାଙ୍ଗ;
	    ସ୍ଥିତି 1: mouse_code = MOUSE_ଡାହାଣ; ଭାଙ୍ଗ;
	    ସ୍ଥିତି 2: mouse_code = MOUSE_ମଧ୍ୟ; ଭାଙ୍ଗ;
	    ପୂର୍ବନିର୍ଦ୍ଧାରିତ: ଫେରନ୍ତୁ -1;
	}

	ବାଛ (action)
	{
	    ସ୍ଥିତି 31: // Initial press
		ଯଦି (*p++ != ';')
		    ଫେରନ୍ତୁ -1;

		ଅଙ୍କ_clicks = getdigits(&p); // Not used
		ଭାଙ୍ଗ;

	    ସ୍ଥିତି 32: // Release
		is_release = TRUE;
		ଭାଙ୍ଗ;

	    ସ୍ଥିତି 33: // Drag
		held_button = mouse_code;
		mouse_code |= MOUSE_DRAG;
		ଭାଙ୍ଗ;

	    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
		ଫେରନ୍ତୁ -1;
	}

	ଯଦି (*p++ != 't')
	    ଫେରନ୍ତୁ -1;

	*slen += (p - (tp + *slen));
    }
#endif // FEAT_MOUSE_PTERM

    // Interpret the mouse code
    current_button = (mouse_code & MOUSE_CLICK_MASK);
    ଯଦି (is_release)
	current_button |= MOUSE_RELEASE;

    ଯଦି (current_button == MOUSE_RELEASE
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
	ଯଦି ((mouse_code & MOUSE_DRAG) == MOUSE_DRAG)
	    is_drag = TRUE;
	current_button = held_button;
    }
    ଅନ୍ୟଥା
    {
      ଯଦି (wheel_code == 0)
      {
#ifdef CHECK_DOUBLE_CLICK
# ifdef FEAT_MOUSE_GPM
	/*
	 * Only for Unix, when GUI not active, we handle multi-clicks here, but
	 * not for GPM mouse events.
	 */
#  ifdef FEAT_GUI
	ଯଦି (ଚାବି_name[0] != KS_GPM_MOUSE && !gui.in_use)
#  ଅନ୍ୟଥା
	    ଯଦି (ଚାବି_name[0] != KS_GPM_MOUSE)
#  endif
# ଅନ୍ୟଥା
#  ifdef FEAT_GUI
		ଯଦି (!gui.in_use)
#  endif
# endif
		{
		    /*
		     * Compute the time elapsed since the previous mouse click.
		     */
		    gettimeofday(&mouse_time, ଖାଲି);
		    ଯଦି (orig_mouse_time.tv_sec == 0)
		    {
			/*
			 * Avoid computing the difference between mouse_time
			 * and orig_mouse_time for the first click, as the
			 * difference would be huge and would cause
			 * multiplication overflow.
			 */
			timediff = p_mouset;
		    }
		    ଅନ୍ୟଥା
			timediff = time_diff_ms(&orig_mouse_time, &mouse_time);
		    orig_mouse_time = mouse_time;
		    ଯଦି (mouse_code == orig_mouse_code
			    && timediff < p_mouset
			    && orig_ଅଙ୍କ_clicks != 4
			    && orig_mouse_col == mouse_col
			    && orig_mouse_row == mouse_row
			    && (is_mouse_topline(curwin)
				// Double click in tab pages line also works
				// when window contents changes.
				|| (mouse_row == 0 && firstwin->w_winrow > 0))
		       )
			++orig_ଅଙ୍କ_clicks;
		    ଅନ୍ୟଥା
			orig_ଅଙ୍କ_clicks = 1;
		    orig_mouse_col = mouse_col;
		    orig_mouse_row = mouse_row;
		    set_mouse_topline(curwin);
		}
# ଯଦି defined(FEAT_GUI) || defined(FEAT_MOUSE_GPM)
		ଅନ୍ୟଥା
		    orig_ଅଙ୍କ_clicks = ଅଙ୍କ_MOUSE_CLICKS(mouse_code);
# endif
#ଅନ୍ୟଥା
	orig_ଅଙ୍କ_clicks = ଅଙ୍କ_MOUSE_CLICKS(mouse_code);
#endif
	is_click = TRUE;
      }
      orig_mouse_code = mouse_code;
    }
    ଯଦି (!is_drag)
	held_button = mouse_code & MOUSE_CLICK_MASK;

    /*
     * Translate the actual mouse event into a pseudo mouse event.
     * First work out what modifiers are to be used.
     */
    ଯଦି (orig_mouse_code & MOUSE_SHIFT)
	*modifiers |= MOD_MASK_SHIFT;
    ଯଦି (orig_mouse_code & MOUSE_CTRL)
	*modifiers |= MOD_MASK_CTRL;
    ଯଦି (orig_mouse_code & MOUSE_ALT)
	*modifiers |= MOD_MASK_ALT;
    ଯଦି (orig_ଅଙ୍କ_clicks == 2)
	*modifiers |= MOD_MASK_2CLICK;
    ଅନ୍ୟଥା ଯଦି (orig_ଅଙ୍କ_clicks == 3)
	*modifiers |= MOD_MASK_3CLICK;
    ଅନ୍ୟଥା ଯଦି (orig_ଅଙ୍କ_clicks == 4)
	*modifiers |= MOD_MASK_4CLICK;

    // Work out our pseudo mouse event. Note that MOUSE_RELEASE gets added,
    // then it's not mouse up/down.
    ଚାବି_name[0] = KS_EXTRA;
    ଯଦି (wheel_code != 0 && (!is_release || release_is_ambiguous))
    {
	ଯଦି (wheel_code & MOUSE_CTRL)
	    *modifiers |= MOD_MASK_CTRL;
	ଯଦି (wheel_code & MOUSE_ALT)
	    *modifiers |= MOD_MASK_ALT;

	ଯଦି (wheel_code & 1 && wheel_code & 2)
	    ଚାବି_name[1] = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)KE_MOUSELEFT;
	ଅନ୍ୟଥା ଯଦି (wheel_code & 2)
	    ଚାବି_name[1] = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)KE_MOUSERIGHT;
	ଅନ୍ୟଥା ଯଦି (wheel_code & 1)
	    ଚାବି_name[1] = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)KE_MOUSEUP;
	ଅନ୍ୟଥା
	    ଚାବି_name[1] = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)KE_MOUSEDOWN;

	held_button = MOUSE_RELEASE;
    }
    ଅନ୍ୟଥା
    {
#ଯଦି defined(UNIX)
	ଯଦି (use_xterm_mouse() && orig_mouse_code >= MOUSESIDEBUTTONS_ତଳ)
	    current_button = (current_button) ? MOUSE_X2 : MOUSE_X1;
#endif
	ଚାବି_name[1] = get_pseudo_mouse_code(current_button, is_click, is_drag);
    }


    // Make sure the mouse position is valid.  Some terminals may return weird
    // values.
    ଯଦି (mouse_col >= Columns)
	mouse_col = Columns - 1;
    ଯଦି (mouse_row >= Rows)
	mouse_row = Rows - 1;

    ଫେରନ୍ତୁ 0;
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mouse_comp_pos(
    win_T	*win,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*rowp,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*colp,
    linenr_T	*lnump,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*plines_cache)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		col = *colp;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		row = *rowp;
    linenr_T	lnum;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		retval = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		off;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ଗଣନା_2;

#ifdef FEAT_RIGHTLEFT
    ଯଦି (win->w_p_rl)
	col = win->w_width - 1 - col;
#endif

    lnum = win->w_topline;

    ଯେତେବେଳେ (row > 0)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା cache_idx = lnum - win->w_topline;

	// Only "Rows" lines are cached, with folding we'll run out of entries
	// and use the slow way.
	ଯଦି (plines_cache != ଖାଲି && cache_idx < Rows
						&& plines_cache[cache_idx] > 0)
	    ଗଣନା_2 = plines_cache[cache_idx];
	ଅନ୍ୟଥା
	{
#ifdef FEAT_DIFF
	    // Don't include filler lines in "count"
	    ଯଦି (win->w_p_diff
# ifdef FEAT_FOLDING
		    && !hasFoldingWin(win, lnum, ଖାଲି, ଖାଲି, TRUE, ଖାଲି)
# endif
		    )
	    {
		ଯଦି (lnum == win->w_topline)
		    row -= win->w_topfill;
		ଅନ୍ୟଥା
		    row -= diff_check_fill(win, lnum);
		ଗଣନା_2 = plines_win_nofill(win, lnum, FALSE);
	    }
	    ଅନ୍ୟଥା
#endif
		ଗଣନା_2 = plines_win(win, lnum, FALSE);
	    ଯଦି (plines_cache != ଖାଲି && cache_idx < Rows)
		plines_cache[cache_idx] = ଗଣନା_2;
	}

	ଯଦି (win->w_skipcol > 0 && lnum == win->w_topline)
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା width1 = win->w_width - win_col_off(win);

	    ଯଦି (width1 > 0)
	    {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା skip_ଧାଡ଼ିଗୁଡ଼ିକ = 0;

		// Adjust for 'smoothscroll' clipping the top screen lines.
		// A similar formula is used in curs_columns().
		ଯଦି (win->w_skipcol > width1)
		    skip_ଧାଡ଼ିଗୁଡ଼ିକ = (win->w_skipcol - width1)
					    / (width1 + win_col_off2(win)) + 1;
		ଅନ୍ୟଥା ଯଦି (win->w_skipcol > 0)
		    skip_ଧାଡ଼ିଗୁଡ଼ିକ = 1;

		ଗଣନା_2 -= skip_ଧାଡ଼ିଗୁଡ଼ିକ;
	    }
	}

	ଯଦି (ଗଣନା_2 > row)
	    ଭାଙ୍ଗ;	// Position is in this buffer line.
#ifdef FEAT_FOLDING
	(ଶୂନ୍ୟ)hasFoldingWin(win, lnum, ଖାଲି, &lnum, TRUE, ଖାଲି);
#endif
	ଯଦି (lnum == win->w_buffer->b_ml.ml_ଧାଡ଼ି_ଗଣନା)
	{
	    retval = TRUE;
	    ଭାଙ୍ଗ;		// past end of file
	}
	row -= ଗଣନା_2;
	++lnum;
    }

    ଯଦି (!retval)
    {
	// Compute the column without wrapping.
	off = win_col_off(win) - win_col_off2(win);
	ଯଦି (col < off)
	    col = off;
	col += row * (win->w_width - off);

	// Add skip column for the topline.
	ଯଦି (lnum == win->w_topline)
	    col += win->w_skipcol;
    }

    ଯଦି (!win->w_p_wrap)
	col += win->w_leftcol;

    // skip line number and fold column in front of the line
    col -= win_col_off(win);
    ଯଦି (col <= 0)
    {
#ifdef FEAT_NETBEANS_INTG
	// if mouse is clicked on the gutter, then inform the netbeans server
	ଯଦି (*colp < win_col_off(win))
	    netbeans_gutter_click(lnum);
#endif
	col = 0;
    }

    *colp = col;
    *rowp = row;
    *lnump = lnum;
    ଫେରନ୍ତୁ retval;
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
mouse_find_win(ପୂର୍ଣ୍ଣସଂଖ୍ୟା *rowp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *colp, mouse_find_T popup UNUSED)
{
    frame_T	*fp;
    win_T	*wp;

#ifdef FEAT_PROP_POPUP
    win_T	*pwp = ଖାଲି;

    ଯଦି (popup != IGNORE_POPUP)
    {
	popup_reset_handled(POPUP_HANDLED_1);
	ଯେତେବେଳେ ((wp = find_next_popup(TRUE, POPUP_HANDLED_1)) != ଖାଲି)
	{
	    ଯଦି (*rowp >= wp->w_winrow && *rowp < wp->w_winrow + popup_height(wp)
		    && *colp >= wp->w_wincol
				    && *colp < wp->w_wincol + popup_width(wp))
		pwp = wp;
	}
	ଯଦି (pwp != ଖାଲି)
	{
	    ଯଦି (popup == FAIL_POPUP)
		ଫେରନ୍ତୁ ଖାଲି;
	    *rowp -= pwp->w_winrow;
	    *colp -= pwp->w_wincol;
	    ଫେରନ୍ତୁ pwp;
	}
    }
#endif

    fp = topframe;

    ଯଦି (*colp < firstwin->w_wincol
	    || *colp >= firstwin->w_wincol + fp->fr_width
	    || *rowp < firstwin->w_winrow)
	ଫେରନ୍ତୁ ଖାଲି;

    *rowp -= firstwin->w_winrow;
    *colp -= firstwin->w_wincol;
    ପାଇଁ (;;)
    {
	ଯଦି (fp->fr_layout == FR_LEAF)
	    ଭାଙ୍ଗ;
	ଯଦି (fp->fr_layout == FR_ROW)
	{
	    ପାଇଁ (fp = fp->fr_child; fp->fr_next != ଖାଲି; fp = fp->fr_next)
	    {
		ଯଦି (*colp < fp->fr_width)
		    ଭାଙ୍ଗ;
		*colp -= fp->fr_width;
	    }
	}
	ଅନ୍ୟଥା    // fr_layout == FR_COL
	{
	    ପାଇଁ (fp = fp->fr_child; fp->fr_next != ଖାଲି; fp = fp->fr_next)
	    {
		ଯଦି (*rowp < fp->fr_height)
		    ଭାଙ୍ଗ;
		*rowp -= fp->fr_height;
	    }
	}
    }
    // When using a timer that closes a window the window might not actually
    // exist.
    FOR_ALL_WINDOWS(wp)
	ଯଦି (wp == fp->fr_win)
	{
#ifdef FEAT_MENU
	    *rowp -= wp->w_winbar_height;
#endif
	    ଫେରନ୍ତୁ wp;
	}
    ଫେରନ୍ତୁ ଖାଲି;
}

#ଯଦି defined(NEED_VCOL2COL) || defined(FEAT_BEVAL) || defined(FEAT_PROP_POPUP) \
	|| defined(FEAT_EVAL)
/*
 * Convert a virtual (screen) column to a character column.
 * The first column is zero.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
vcol2col(win_T *wp, linenr_T lnum, ପୂର୍ଣ୍ଣସଂଖ୍ୟା vcol, colnr_T *coladdp)
{
    char_u	    *ଧାଡ଼ି;
    chartabsize_T   cts;

    // try to advance to the specified column
    ଧାଡ଼ି = ml_get_buf(wp->w_buffer, lnum, FALSE);
    init_chartabsize_arg(&cts, wp, lnum, 0, ଧାଡ଼ି, ଧାଡ଼ି);
    ଯେତେବେଳେ (cts.cts_vcol < vcol && *cts.cts_ptr != NUL)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା size = win_lbr_chartabsize(&cts, ଖାଲି, ଖାଲି);
	ଯଦି (cts.cts_vcol + size > vcol)
	    ଭାଙ୍ଗ;
	cts.cts_vcol += size;
	MB_PTR_ADV(cts.cts_ptr);
    }
    clear_chartabsize_arg(&cts);

    ଯଦି (coladdp != ଖାଲି)
	*coladdp = vcol - cts.cts_vcol;
    ଫେରନ୍ତୁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(cts.cts_ptr - ଧାଡ଼ି);
}
#endif

#ଯଦି defined(FEAT_EVAL)
/*
 * "getmousepos()" function.
 */
    ଶୂନ୍ୟ
f_getmousepos(typval_T *argvars UNUSED, typval_T *rettv)
{
    dict_T	*d;
    win_T	*wp;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		row = mouse_row;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		col = mouse_col;
    varnumber_T winid = 0;
    varnumber_T winrow = 0;
    varnumber_T wincol = 0;
    linenr_T	lnum = 0;
    varnumber_T column = 0;
    colnr_T	coladd = 0;

    ଯଦି (rettv_dict_alloc(rettv) == FAIL)
	ଫେରନ୍ତୁ;
    d = rettv->vval.v_dict;

    dict_add_ସଂଖ୍ୟା(d, "screenrow", (varnumber_T)mouse_row + 1);
    dict_add_ସଂଖ୍ୟା(d, "screencol", (varnumber_T)mouse_col + 1);

    wp = mouse_find_win(&row, &col, FIND_POPUP);
    ଯଦି (wp != ଖାଲି)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	ଶୀର୍ଷ_off = 0;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	ବାମ_off = 0;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	height = wp->w_height + wp->w_status_height;

# ifdef FEAT_PROP_POPUP
	ଯଦି (WIN_IS_POPUP(wp))
	{
	    ଶୀର୍ଷ_off = popup_ଶୀର୍ଷ_extra(wp);
	    ବାମ_off = popup_ବାମ_extra(wp);
	    height = popup_height(wp);
	}
# endif
	ଯଦି (row < height)
	{
	    winid = wp->w_id;
	    winrow = row + 1;
	    wincol = col + 1;
	    row -= ଶୀର୍ଷ_off;
	    col -= ବାମ_off;
	    ଯଦି (row >= 0 && row < wp->w_height && col >= 0 && col < wp->w_width)
	    {
		(ଶୂନ୍ୟ)mouse_comp_pos(wp, &row, &col, &lnum, ଖାଲି);
		col = vcol2col(wp, lnum, col, &coladd);
		column = col + 1;
	    }
	}
    }
    dict_add_ସଂଖ୍ୟା(d, "winid", winid);
    dict_add_ସଂଖ୍ୟା(d, "winrow", winrow);
    dict_add_ସଂଖ୍ୟା(d, "wincol", wincol);
    dict_add_ସଂଖ୍ୟା(d, "line", (varnumber_T)lnum);
    dict_add_ସଂଖ୍ୟା(d, "column", column);
    dict_add_ସଂଖ୍ୟା(d, "coladd", coladd);
}
#endif
