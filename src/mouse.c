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

#include "vim.h"

/*
 * Horiziontal and vertical steps used when scrolling.
 * When negative scroll by a whole page.
 */
static long mouse_hor_step = 6;
static long mouse_vert_step = 3;
static win_T *dragwin = NULL;	// window being dragged
static puur_naa_nka stl_click_handler(win_T *wp, puur_naa_nka mrow, puur_naa_nka mcol, puur_naa_nka which_button,
								    puur_naa_nka mods);
static puur_naa_nka stl_click_handler_regions(stl_click_region_T *regions,
				    puur_naa_nka region_count, puur_naa_nka winid,
				    char_u *area_name, puur_naa_nka mrow, puur_naa_nka mcol,
				    puur_naa_nka which_button, puur_naa_nka mods);

    void
mouse_set_vert_scroll_step(long step)
{
    mouse_vert_step = step;
}

    void
mouse_set_hor_scroll_step(long step)
{
    mouse_hor_step = step;
}

#ifdef CHECK_DOUBLE_CLICK
/*
 * Return the duration from t1 to t2 in milliseconds.
 */
    static long
time_diff_ms(sa_nracanaa timeval *t1, sa_nracanaa timeval *t2)
{
    // This handles wrapping of tv_usec correctly without any special case.
    // Example of 2 pairs (tv_sec, tv_usec) with a duration of 5 ms:
    //	   t1 = (1, 998000) t2 = (2, 3000) gives:
    //	   (2 - 1) * 1000 + (3000 - 998000) / 1000 -> 5 ms.
    ᱨᱩᱣᱟᱹᱲ (t2->tv_sec - t1->tv_sec) * 1000
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
    static puur_naa_nka
get_mouse_class(char_u *p)
{
    puur_naa_nka		c;

    ᱡᱩᱫᱤ (has_mbyte && MB_BYTE2LEN(p[0]) > 1)
	ᱨᱩᱣᱟᱹᱲ mb_get_class(p);

    c = *p;
    ᱡᱩᱫᱤ (c == ' ' || c == '\t')
	ᱨᱩᱣᱟᱹᱲ 0;

    ᱡᱩᱫᱤ (vim_iswordc(c))
	ᱨᱩᱣᱟᱹᱲ 2;

    // There are a few special cases where we want certain combinations of
    // characters to be considered as a single word.  These are things like
    // "->", "/ *", "*=", "+=", "&=", "<=", ">=", "!=" etc.  Otherwise, each
    // character is in its own class.
    ᱡᱩᱫᱤ (c != NUL && vim_strchr((char_u *)"-+*/%<>&|^!=", c) != NULL)
	ᱨᱩᱣᱟᱹᱲ 1;
    ᱨᱩᱣᱟᱹᱲ c;
}

/*
 * Move "pos" back to the start of the word it's in.
 */
    static void
find_start_of_word(pos_T *pos)
{
    char_u	*line;
    puur_naa_nka		cclass;
    puur_naa_nka		col;

    line = ml_get(pos->lnum);
    cclass = get_mouse_class(line + pos->col);

    ᱡᱚᱠᱷᱚᱱ (pos->col > 0)
    {
	col = pos->col - 1;
	col -= (*mb_head_off)(line, line + col);
	ᱡᱩᱫᱤ (get_mouse_class(line + col) != cclass)
	    bha_nga;
	pos->col = col;
    }
}

/*
 * Move "pos" forward to the end of the word it's in.
 * When 'selection' is "exclusive", the position is just after the word.
 */
    static void
find_end_of_word(pos_T *pos)
{
    char_u	*line;
    puur_naa_nka		cclass;
    puur_naa_nka		col;

    line = ml_get(pos->lnum);
    ᱡᱩᱫᱤ (*p_sel == 'e' && pos->col > 0)
    {
	--pos->col;
	pos->col -= (*mb_head_off)(line, line + pos->col);
    }
    cclass = get_mouse_class(line + pos->col);
    ᱡᱚᱠᱷᱚᱱ (line[pos->col] != NUL)
    {
	col = pos->col + (*mb_ptr2len)(line + pos->col);
	ᱡᱩᱫᱤ (get_mouse_class(line + col) != cclass)
	{
	    ᱡᱩᱫᱤ (*p_sel == 'e')
		pos->col = col;
	    bha_nga;
	}
	pos->col = col;
    }
}

#ᱡᱩᱫᱤ defined(FEAT_GUI_MOTIF) || defined(FEAT_GUI_GTK) \
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
    static puur_naa_nka
get_fpos_of_mouse(pos_T *mpos)
{
    win_T	*wp;
    puur_naa_nka		row = mouse_row;
    puur_naa_nka		col = mouse_col;

    ᱡᱩᱫᱤ (row < 0 || col < 0)		// check if it makes sense
	ᱨᱩᱣᱟᱹᱲ IN_UNKNOWN;

    // find the window where the row is in
    wp = mouse_find_win(&row, &col, FAIL_POPUP);
    ᱡᱩᱫᱤ (wp == NULL)
	ᱨᱩᱣᱟᱹᱲ IN_UNKNOWN;
    // winpos and height may change in win_enter()!
    ᱡᱩᱫᱤ (row >= wp->w_height)	// In (or below) status line
	ᱨᱩᱣᱟᱹᱲ IN_STATUS_LINE;
    ᱡᱩᱫᱤ (col >= wp->w_width)	// In vertical separator line
	ᱨᱩᱣᱟᱹᱲ IN_SEP_LINE;

    ᱡᱩᱫᱤ (wp != curwin)
	ᱨᱩᱣᱟᱹᱲ IN_UNKNOWN;

    // compute the position in the buffer line from the posn on the screen
    ᱡᱩᱫᱤ (mouse_comp_pos(curwin, &row, &col, &mpos->lnum, NULL))
	ᱨᱩᱣᱟᱹᱲ IN_STATUS_LINE; // past bottom

    mpos->col = vcol2col(wp, mpos->lnum, col, &mpos->coladd);
    ᱨᱩᱣᱟᱹᱲ IN_BUFFER;
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
    puur_naa_nka
do_mouse(
    oparg_T	*oap,		// operator argument, can be NULL
    puur_naa_nka		c,		// K_LEFTMOUSE, etc
    puur_naa_nka		dir,		// Direction to 'put' if necessary
    long	count,
    puur_naa_nka		fixindent)	// PUT_FIXINDENT if fixing indent necessary
{
    static puur_naa_nka	do_always = FALSE;	// ignore 'mouse' setting next time
    static puur_naa_nka	got_click = FALSE;	// got a click some time back

    puur_naa_nka		which_button;	// MOUSE_LEFT, _MIDDLE or _RIGHT
    puur_naa_nka		is_click = FALSE; // If FALSE it's a drag or release event
    puur_naa_nka		is_drag = FALSE;  // If TRUE it's a drag event
    puur_naa_nka		jump_flags = 0;	// flags for jump_to_mouse()
    pos_T	start_visual;
    puur_naa_nka		moved;		// Has cursor moved?
    puur_naa_nka		in_status_line;	// mouse in status line
    static puur_naa_nka	in_tab_line = FALSE; // mouse clicked in tab line
    static puur_naa_nka	in_tabpanel = FALSE; // mouse clicked in tabpanel
#ifdef FEAT_TABPANEL
    static bool	in_tabpanel_scrollbar = false; // dragging tabpanel scrollbar
#endif
    puur_naa_nka		in_sep_line;	// mouse in vertical separator line
    puur_naa_nka		c1, c2;
#ᱡᱩᱫᱤ defined(FEAT_FOLDING)
    pos_T	save_cursor;
#endif
    win_T	*old_curwin = curwin;
    static pos_T orig_cursor;
    colnr_T	leftcol, rightcol;
    pos_T	end_visual;
    puur_naa_nka		diff;
    puur_naa_nka		old_active = VIsual_active;
    puur_naa_nka		old_mode = VIsual_mode;
    puur_naa_nka		regname;

#ᱡᱩᱫᱤ defined(FEAT_FOLDING)
    save_cursor = curwin->w_cursor;
#endif

    // When GUI is active, always recognize mouse events, otherwise:
    // - Ignore mouse event in normal mode if 'mouse' doesn't include 'n'.
    // - Ignore mouse event in visual mode if 'mouse' doesn't include 'v'.
    // - For command line and insert mode 'mouse' is checked before calling
    //	 do_mouse().
    ᱡᱩᱫᱤ (do_always)
	do_always = FALSE;
    ᱵᱟᱝ
#ifdef FEAT_GUI
	ᱡᱩᱫᱤ (!gui.in_use)
#endif
	{
	    ᱡᱩᱫᱤ (VIsual_active)
	    {
		ᱡᱩᱫᱤ (!mouse_has(MOUSE_VISUAL))
		    ᱨᱩᱣᱟᱹᱲ FALSE;
	    }
	    ᱵᱟᱝ ᱡᱩᱫᱤ (State == MODE_NORMAL && !mouse_has(MOUSE_NORMAL))
		ᱨᱩᱣᱟᱹᱲ FALSE;
	}

    for (;;)
    {
	which_button = get_mouse_button(KEY2TERMCAP1(c), &is_click, &is_drag);
	ᱡᱩᱫᱤ (is_drag)
	{
	    // If the next character is the same mouse event then use that
	    // one. Speeds up dragging the status line.
	    // Note: Since characters added to the stuff buffer in the code
	    // below need to come before the next character, do not do this
	    // when the current character was stuffed.
	    ᱡᱩᱫᱤ (!KeyStuffed && vpeekc() != NUL)
	    {
		puur_naa_nka nc;
		puur_naa_nka save_mouse_row = mouse_row;
		puur_naa_nka save_mouse_col = mouse_col;

		// Need to get the character, peeking doesn't get the actual
		// one.
		nc = safe_vgetc();
		ᱡᱩᱫᱤ (c == nc)
		    continue;
		vungetc(nc);
		mouse_row = save_mouse_row;
		mouse_col = save_mouse_col;
	    }
	}
	bha_nga;
    }

    ᱡᱩᱫᱤ (c == K_MOUSEMOVE)
    {
	// Mouse moved without a button pressed.
#ifdef FEAT_BEVAL_TERM
	ui_may_remove_balloon();
	ᱡᱩᱫᱤ (p_bevalterm)
	{
	    profile_setlimit(p_bdlay, &bevalexpr_due);
	    bevalexpr_due_set = TRUE;
	}
#endif
#ifdef FEAT_PROP_POPUP
	popup_handle_mouse_moved();
#endif
	ᱨᱩᱣᱟᱹᱲ FALSE;
    }

#ifdef FEAT_MOUSESHAPE
    // May have stopped dragging the status or separator line.  The pointer is
    // most likely still on the status or separator line.
    ᱡᱩᱫᱤ (!is_drag && drag_status_line)
    {
	drag_status_line = FALSE;
	update_mouseshape(SHAPE_IDX_STATUS);
    }
    ᱡᱩᱫᱤ (!is_drag && drag_sep_line)
    {
	drag_sep_line = FALSE;
	update_mouseshape(SHAPE_IDX_VSEP);
    }
#endif

    // Ignore drag and release events if we didn't get a click.
    ᱡᱩᱫᱤ (is_click)
    {
	got_click = TRUE;
	in_tab_line = FALSE;
	in_tabpanel = FALSE;
#ifdef FEAT_TABPANEL
	in_tabpanel_scrollbar = false;
#endif
    }
    ᱵᱟᱝ
    {
	ᱡᱩᱫᱤ (!got_click)			// didn't get click, ignore
	    ᱨᱩᱣᱟᱹᱲ FALSE;
	ᱡᱩᱫᱤ (!is_drag)			// release, reset got_click
	{
	    got_click = FALSE;
	    ᱡᱩᱫᱤ (in_tab_line || in_tabpanel
#ifdef FEAT_TABPANEL
		    || in_tabpanel_scrollbar
#endif
		    )
	    {
		in_tab_line = FALSE;
		in_tabpanel = FALSE;
#ifdef FEAT_TABPANEL
		in_tabpanel_scrollbar = false;
#endif
		ᱨᱩᱣᱟᱹᱲ FALSE;
	    }
	}
    }

#ifdef FEAT_TABPANEL
    // Continue a scrollbar drag before any tab-selection handling.
    ᱡᱩᱫᱤ (is_drag && in_tabpanel_scrollbar)
    {
	tabpanel_drag_scrollbar(mouse_row);
	ᱨᱩᱣᱟᱹᱲ FALSE;
    }
#endif

    // CTRL right mouse button does CTRL-T
    ᱡᱩᱫᱤ (is_click && (mod_mask & MOD_MASK_CTRL) && which_button == MOUSE_RIGHT)
    {
	ᱡᱩᱫᱤ (State & MODE_INSERT)
	    stuffcharReadbuff(Ctrl_O);
	ᱡᱩᱫᱤ (count > 1)
	    stuffnumReadbuff(count);
	stuffcharReadbuff(Ctrl_T);
	got_click = FALSE;		// ignore drag&release now
	ᱨᱩᱣᱟᱹᱲ FALSE;
    }

    // CTRL only works with left mouse button
    ᱡᱩᱫᱤ ((mod_mask & MOD_MASK_CTRL) && which_button != MOUSE_LEFT)
	ᱨᱩᱣᱟᱹᱲ FALSE;

    // When a modifier is down, ignore drag and release events, as well as
    // multiple clicks and the middle mouse button.
    // Accept shift-leftmouse drags when 'mousemodel' is "popup.*".
    ᱡᱩᱫᱤ ((mod_mask & (MOD_MASK_SHIFT | MOD_MASK_CTRL | MOD_MASK_ALT
							     | MOD_MASK_META))
	    && (!is_click
		|| (mod_mask & MOD_MASK_MULTI_CLICK)
		|| which_button == MOUSE_MIDDLE)
	    && !((mod_mask & (MOD_MASK_SHIFT|MOD_MASK_ALT))
		&& mouse_model_popup()
		&& which_button == MOUSE_LEFT)
	    && !((mod_mask & MOD_MASK_ALT)
		&& !mouse_model_popup()
		&& which_button == MOUSE_RIGHT)
	    )
	ᱨᱩᱣᱟᱹᱲ FALSE;

    // If the button press was used as the movement command for an operator
    // (eg "d<MOUSE>"), or it is the middle button that is held down, ignore
    // drag/release events.
    ᱡᱩᱫᱤ (!is_click && which_button == MOUSE_MIDDLE)
	ᱨᱩᱣᱟᱹᱲ FALSE;

    ᱡᱩᱫᱤ (oap != NULL)
	regname = oap->regname;
    ᱵᱟᱝ
	regname = 0;

    // Middle mouse button does a 'put' of the selected text
    ᱡᱩᱫᱤ (which_button == MOUSE_MIDDLE)
    {
	ᱡᱩᱫᱤ (State == MODE_NORMAL)
	{
	    // If an operator was pending, we don't know what the user wanted
	    // to do. Go back to normal mode: Clear the operator and beep().
	    ᱡᱩᱫᱤ (oap != NULL && oap->op_type != OP_NOP)
	    {
		clearopbeep(oap);
		ᱨᱩᱣᱟᱹᱲ FALSE;
	    }

	    // If visual was active, yank the highlighted text and put it
	    // before the mouse pointer position.
	    // In Select mode replace the highlighted text with the clipboard.
	    ᱡᱩᱫᱤ (VIsual_active)
	    {
		ᱡᱩᱫᱤ (VIsual_select)
		{
		    stuffcharReadbuff(Ctrl_G);
		    stuffReadbuff((char_u *)"\"+p");
		}
		ᱵᱟᱝ
		{
		    stuffcharReadbuff('y');
		    stuffcharReadbuff(K_MIDDLEMOUSE);
		}
		do_always = TRUE;	// ignore 'mouse' setting next time
		ᱨᱩᱣᱟᱹᱲ FALSE;
	    }
	    // The rest is below jump_to_mouse()
	}

	ᱵᱟᱝ ᱡᱩᱫᱤ ((State & MODE_INSERT) == 0)
	    ᱨᱩᱣᱟᱹᱲ FALSE;

	// Middle click in insert mode doesn't move the mouse, just insert the
	// contents of a register.  '.' register is special, can't insert that
	// with do_put().
	// Also paste at the cursor if the current mode isn't in 'mouse' (only
	// happens for the GUI).
	ᱡᱩᱫᱤ ((State & MODE_INSERT) || !mouse_has(MOUSE_NORMAL))
	{
	    ᱡᱩᱫᱤ (regname == '.')
		insert_reg(regname, TRUE);
	    ᱵᱟᱝ
	    {
#ifdef FEAT_CLIPBOARD
		ᱡᱩᱫᱤ (clip_star.available && regname == 0)
		    regname = '*';
#endif
		ᱡᱩᱫᱤ ((State & REPLACE_FLAG) && !yank_register_mline(regname))
		    insert_reg(regname, TRUE);
		ᱵᱟᱝ
		{
		    do_put(regname, NULL, BACKWARD, 1L,
						      fixindent | PUT_CURSEND);

		    // Repeat it with CTRL-R CTRL-O r or CTRL-R CTRL-P r
		    AppendCharToRedobuff(Ctrl_R);
		    AppendCharToRedobuff(fixindent ? Ctrl_P : Ctrl_O);
		    AppendCharToRedobuff(regname == 0 ? '"' : regname);
		}
	    }
	    ᱨᱩᱣᱟᱹᱲ FALSE;
	}
    }

    // When dragging or button-up stay in the same window.
    ᱡᱩᱫᱤ (!is_click)
	jump_flags |= MOUSE_FOCUS | MOUSE_DID_MOVE;

    start_visual.lnum = 0;

    sa_nracanaa tabpage_label_info {
	bool is_panel;	    // label type. true: tabpanel, false: tab line
	bool just_in;	    // just in tabpage label area
	bool just_click;    // just click tabpage label area
	puur_naa_nka nr;		    // tabpage number
    } tp_label = { false, false, false, 0 };

    // Check for clicking in the tab page panel.
#ᱡᱩᱫᱤ defined(FEAT_TABPANEL)
    ᱡᱩᱫᱤ (mouse_col < firstwin->w_wincol
		|| mouse_col >= firstwin->w_wincol + topframe->fr_width)
    {
	// A click on the scrollbar column starts a drag interaction and
	// preempts tab-selection.
	ᱡᱩᱫᱤ (is_click && !is_drag && mouse_on_tabpanel_scrollbar())
	{
	    in_tabpanel_scrollbar = TRUE;
	    tabpanel_drag_scrollbar(mouse_row);
	    ᱨᱩᱣᱟᱹᱲ FALSE;
	}

	// Dispatch 'tabpanel' %[FuncName] click regions before falling through
	// to tab-page selection.  On drag events fall through to the normal
	// tab-drag handling.
	ᱡᱩᱫᱤ (is_click && !is_drag
		&& stl_click_handler_regions(tabpanel_stl_click,
					    tabpanel_stl_click_count,
					    0, (char_u *)"tabpanel",
					    mouse_row, mouse_col,
					    which_button, mod_mask))
	    ᱨᱩᱣᱟᱹᱲ FALSE;

	tp_label.is_panel = true;
	tp_label.just_in = true;
	tp_label.nr = get_tabpagenr_on_tabpanel();

	// click in a tab selects that tab page
	ᱡᱩᱫᱤ (is_click && cmdwin_type == 0)
	    tp_label.just_click = true;
    }
    ᱵᱟᱝ
#endif
    // Check for clicking in the tab page line.
    ᱡᱩᱫᱤ (TabPageIdxs != NULL && mouse_row == 0 && firstwin->w_winrow > 0)
    {
	// Dispatch 'tabline' %[FuncName] click regions before falling through
	// to tab-page selection.  On drag events fall through to the normal
	// tab-drag handling.
	ᱡᱩᱫᱤ (is_click && !is_drag
		&& stl_click_handler_regions(tabline_stl_click,
					    tabline_stl_click_count,
					    0, (char_u *)"tabline",
					    mouse_row, mouse_col,
					    which_button, mod_mask))
	    ᱨᱩᱣᱟᱹᱲ FALSE;

	tp_label.just_in = true;
	tp_label.nr = TabPageIdxs[mouse_col];

	// click in a tab selects that tab page
	ᱡᱩᱫᱤ (is_click && cmdwin_type == 0
		&& mouse_col < firstwin->w_wincol + topframe->fr_width)
	    tp_label.just_click = true;
    }

    ᱡᱩᱫᱤ (tp_label.just_in)
    {
	ᱡᱩᱫᱤ (is_drag)
	{
	    ᱡᱩᱫᱤ (in_tabpanel || in_tab_line)
	    {
		c1 = tp_label.nr;
		tabpage_move(c1 <= 0 ? 9999 : c1 < tabpage_index(curtab)
							    ? c1 - 1 : c1);
	    }
	    ᱨᱩᱣᱟᱹᱲ FALSE;
	}

	ᱡᱩᱫᱤ (tp_label.just_click)
	{
	    ᱡᱩᱫᱤ (tp_label.is_panel)
		in_tabpanel = TRUE;
	    ᱵᱟᱝ
		in_tab_line = TRUE;
	    c1 = tp_label.nr;
	    ᱡᱩᱫᱤ (c1 >= 0)
	    {
		ᱡᱩᱫᱤ ((mod_mask & MOD_MASK_MULTI_CLICK) == MOD_MASK_2CLICK
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
		ᱵᱟᱝ
		{
		    // Go to specified tab page, or next one if not clicking
		    // on a label.
		    goto_tabpage(c1);

		    // It's like clicking on the status line of a window.
		    ᱡᱩᱫᱤ (curwin != old_curwin)
			end_visual_mode_keep_button();
		}
	    }
	    ᱵᱟᱝ
	    {
		tabpage_T	*tp;

		// Close the current or specified tab page.
		ᱡᱩᱫᱤ (c1 == -999)
		    tp = curtab;
		ᱵᱟᱝ
		    tp = find_tabpage(-c1);
		ᱡᱩᱫᱤ (tp == curtab)
		{
		    ᱡᱩᱫᱤ (first_tabpage->tp_next != NULL)
			tabpage_close(FALSE);
		}
		ᱵᱟᱝ ᱡᱩᱫᱤ (tp != NULL)
		    tabpage_close_other(tp, FALSE);
	    }
	}
	ᱨᱩᱣᱟᱹᱲ TRUE;
    }
    ᱵᱟᱝ ᱡᱩᱫᱤ (is_drag && (in_tabpanel || (in_tab_line && TabPageIdxs != NULL)))
    {
#ᱡᱩᱫᱤ defined(FEAT_TABPANEL)
	ᱡᱩᱫᱤ (in_tabpanel)
	    c1 = get_tabpagenr_on_tabpanel();
	ᱵᱟᱝ
#endif
	    c1 = TabPageIdxs[mouse_col];
	tabpage_move(c1 <= 0 ? 9999 : c1 - 1);
	ᱨᱩᱣᱟᱹᱲ FALSE;
    }

    // When 'mousemodel' is "popup" or "popup_setpos", translate mouse events:
    // right button up   -> pop-up menu
    // shift-left button -> right button
    // alt-left button   -> alt-right button
    ᱡᱩᱫᱤ (mouse_model_popup())
    {
	ᱡᱩᱫᱤ (which_button == MOUSE_RIGHT
			    && !(mod_mask & (MOD_MASK_SHIFT | MOD_MASK_CTRL)))
	{
#ifdef USE_POPUP_SETPOS
# ifdef FEAT_GUI
	    ᱡᱩᱫᱤ (gui.in_use)
	    {
#  ᱡᱩᱫᱤ defined(FEAT_GUI_MOTIF) || defined(FEAT_GUI_GTK) \
			  || defined(FEAT_GUI_PHOTON)
		ᱡᱩᱫᱤ (!is_click)
		    // Ignore right button release events, only shows the popup
		    // menu on the button down event.
		    ᱨᱩᱣᱟᱹᱲ FALSE;
#  endif
#  ᱡᱩᱫᱤ defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_HAIKU)
		ᱡᱩᱫᱤ (is_click || is_drag)
		    // Ignore right button down and drag mouse events.  Windows
		    // only shows the popup menu on the button up event.
		    ᱨᱩᱣᱟᱹᱲ FALSE;
#  endif
	    }
# endif
# ᱡᱩᱫᱤ defined(FEAT_GUI) && defined(FEAT_TERM_POPUP_MENU)
	    ᱵᱟᱝ
# endif
# ᱡᱩᱫᱤ defined(FEAT_TERM_POPUP_MENU)
	    ᱡᱩᱫᱤ (!is_click)
		// Ignore right button release events, only shows the popup
		// menu on the button down event.
		ᱨᱩᱣᱟᱹᱲ FALSE;
# endif

	    jump_flags = 0;
	    ᱡᱩᱫᱤ (STRCMP(p_mousem, "popup_setpos") == 0)
	    {
		// First set the cursor position before showing the popup
		// menu.
		ᱡᱩᱫᱤ (VIsual_active)
		{
		    pos_T    m_pos;

		    // set MOUSE_MAY_STOP_VIS if we are outside the
		    // selection or the current window (might have false
		    // negative here)
		    ᱡᱩᱫᱤ (mouse_row < curwin->w_winrow
			 || mouse_row
				  > (curwin->w_winrow + curwin->w_height))
			jump_flags = MOUSE_MAY_STOP_VIS;
		    ᱵᱟᱝ ᱡᱩᱫᱤ (get_fpos_of_mouse(&m_pos) != IN_BUFFER)
			jump_flags = MOUSE_MAY_STOP_VIS;
		    ᱵᱟᱝ
		    {
			ᱡᱩᱫᱤ (VIsual_mode == 'V')
			{
			    ᱡᱩᱫᱤ ((curwin->w_cursor.lnum <= VIsual.lnum
				    && (m_pos.lnum < curwin->w_cursor.lnum
					|| VIsual.lnum < m_pos.lnum))
				|| (VIsual.lnum < curwin->w_cursor.lnum
				    && (m_pos.lnum < VIsual.lnum
					|| curwin->w_cursor.lnum < m_pos.lnum)))
			    {
				jump_flags = MOUSE_MAY_STOP_VIS;
			    }
			}
			ᱵᱟᱝ ᱡᱩᱫᱤ ((LTOREQ_POS(curwin->w_cursor, VIsual)
				    && (LT_POS(m_pos, curwin->w_cursor)
					|| LT_POS(VIsual, m_pos)))
				|| (LT_POS(VIsual, curwin->w_cursor)
				    && (LT_POS(m_pos, VIsual)
				      || LT_POS(curwin->w_cursor, m_pos))))
			{
			    jump_flags = MOUSE_MAY_STOP_VIS;
			}
			ᱵᱟᱝ ᱡᱩᱫᱤ (VIsual_mode == Ctrl_V)
			{
			    getvcols(curwin, &curwin->w_cursor, &VIsual,
						     &leftcol, &rightcol, 0);
			    getvcol(curwin, &m_pos, NULL, &m_pos.col, NULL, 0);
			    ᱡᱩᱫᱤ (m_pos.col < leftcol || m_pos.col > rightcol)
				jump_flags = MOUSE_MAY_STOP_VIS;
			}
		    }
		}
		ᱵᱟᱝ
		    jump_flags = MOUSE_MAY_STOP_VIS;
	    }
	    ᱡᱩᱫᱤ (jump_flags)
	    {
		jump_flags = jump_to_mouse(jump_flags, NULL, which_button);
		update_curbuf(VIsual_active ? UPD_INVERTED : UPD_VALID);
		setcursor();
		out_flush();    // Update before showing popup menu
	    }
# ifdef FEAT_MENU
	    show_popupmenu();
	    got_click = FALSE;	// ignore release events
# endif
	    ᱨᱩᱣᱟᱹᱲ (jump_flags & CURSOR_MOVED) != 0;
#ᱵᱟᱝ
	    ᱨᱩᱣᱟᱹᱲ FALSE;
#endif
	}
	ᱡᱩᱫᱤ (which_button == MOUSE_LEFT
				&& (mod_mask & (MOD_MASK_SHIFT|MOD_MASK_ALT)))
	{
	    which_button = MOUSE_RIGHT;
	    mod_mask &= ~MOD_MASK_SHIFT;
	}
    }

    ᱡᱩᱫᱤ ((State & (MODE_NORMAL | MODE_INSERT))
			    && !(mod_mask & (MOD_MASK_SHIFT | MOD_MASK_CTRL)))
    {
	ᱡᱩᱫᱤ (which_button == MOUSE_LEFT)
	{
	    ᱡᱩᱫᱤ (is_click)
	    {
		// stop Visual mode for a left click in a window, but not when
		// on a status line
		ᱡᱩᱫᱤ (VIsual_active)
		    jump_flags |= MOUSE_MAY_STOP_VIS;
	    }
	    ᱵᱟᱝ ᱡᱩᱫᱤ (mouse_has(MOUSE_VISUAL))
		jump_flags |= MOUSE_MAY_VIS;
	}
	ᱵᱟᱝ ᱡᱩᱫᱤ (which_button == MOUSE_RIGHT)
	{
	    ᱡᱩᱫᱤ (is_click && VIsual_active)
	    {
		// Remember the start and end of visual before moving the
		// cursor.
		ᱡᱩᱫᱤ (LT_POS(curwin->w_cursor, VIsual))
		{
		    start_visual = curwin->w_cursor;
		    end_visual = VIsual;
		}
		ᱵᱟᱝ
		{
		    start_visual = VIsual;
		    end_visual = curwin->w_cursor;
		}
	    }
	    jump_flags |= MOUSE_FOCUS;
	    ᱡᱩᱫᱤ (mouse_has(MOUSE_VISUAL))
		jump_flags |= MOUSE_MAY_VIS;
	}
    }

    // If an operator is pending, ignore all drags and releases until the
    // next mouse click.
    ᱡᱩᱫᱤ (!is_drag && oap != NULL && oap->op_type != OP_NOP)
    {
	got_click = FALSE;
	oap->motion_type = MCHAR;
    }

    // When releasing the button let jump_to_mouse() know.
    ᱡᱩᱫᱤ (!is_click && !is_drag)
	jump_flags |= MOUSE_RELEASED;

    // JUMP!
    jump_flags = jump_to_mouse(jump_flags,
			oap == NULL ? NULL : &(oap->inclusive), which_button);

#ifdef FEAT_MENU
    // A click in the window toolbar has no side effects.
    ᱡᱩᱫᱤ (jump_flags & MOUSE_WINBAR)
	ᱨᱩᱣᱟᱹᱲ FALSE;
#endif
    moved = (jump_flags & CURSOR_MOVED);
    in_status_line = (jump_flags & IN_STATUS_LINE);
    in_sep_line = (jump_flags & IN_SEP_LINE);

    // Check for statusline click handler early, before visual mode or
    // other button-specific handling can interfere.
    ᱡᱩᱫᱤ (in_status_line && is_click && !is_drag
	    && stl_click_handler(dragwin, mouse_row, mouse_col,
						which_button, mod_mask))
    {
#ifdef FEAT_MOUSESHAPE
	ᱡᱩᱫᱤ (!drag_status_line)
	{
	    drag_status_line = TRUE;
	    update_mouseshape(-1);
	}
#endif
	ᱨᱩᱣᱟᱹᱲ FALSE;
    }

#ifdef FEAT_NETBEANS_INTG
    ᱡᱩᱫᱤ (isNetbeansBuffer(curbuf)
			    && !(jump_flags & (IN_STATUS_LINE | IN_SEP_LINE)))
    {
	puur_naa_nka key = KEY2TERMCAP1(c);

	ᱡᱩᱫᱤ (key == (puur_naa_nka)KE_LEFTRELEASE || key == (puur_naa_nka)KE_MIDDLERELEASE
					       || key == (puur_naa_nka)KE_RIGHTRELEASE)
	    netbeans_button_release(which_button);
    }
#endif

    // When jumping to another window, clear a pending operator.  That's a bit
    // friendlier than beeping and not jumping to that window.
    ᱡᱩᱫᱤ (curwin != old_curwin && oap != NULL && oap->op_type != OP_NOP)
	clearop(oap);

#ifdef FEAT_FOLDING
    ᱡᱩᱫᱤ (mod_mask == 0
	    && !is_drag
	    && (jump_flags & (MOUSE_FOLD_CLOSE | MOUSE_FOLD_OPEN))
	    && which_button == MOUSE_LEFT)
    {
	// open or close a fold at this line
	ᱡᱩᱫᱤ (jump_flags & MOUSE_FOLD_OPEN)
	    openFold(curwin->w_cursor.lnum, 1L);
	ᱵᱟᱝ
	    closeFold(curwin->w_cursor.lnum, 1L);
	// don't move the cursor if still in the same window
	ᱡᱩᱫᱤ (curwin == old_curwin)
	    curwin->w_cursor = save_cursor;
    }
#endif

#ᱡᱩᱫᱤ defined(FEAT_CLIPBOARD)
    ᱡᱩᱫᱤ ((jump_flags & IN_OTHER_WIN) && !VIsual_active &&
	    (clip_star.available || clip_plus.available))
    {
	clip_modeless(which_button, is_click, is_drag);
	ᱨᱩᱣᱟᱹᱲ FALSE;
    }
#endif

    // Set global flag that we are extending the Visual area with mouse
    // dragging; temporarily minimize 'scrolloff'.
    ᱡᱩᱫᱤ (VIsual_active && is_drag && get_scrolloff_value())
    {
	// In the very first line, allow scrolling one line
	ᱡᱩᱫᱤ (mouse_row == 0)
	    mouse_dragging = 2;
	ᱵᱟᱝ
	    mouse_dragging = 1;
    }

    // When dragging the mouse above the window, scroll down.
    ᱡᱩᱫᱤ (is_drag && mouse_row < 0 && !in_status_line)
    {
	scroll_redraw(FALSE, 1L);
	mouse_row = 0;
    }

    ᱡᱩᱫᱤ (start_visual.lnum)		// right click in visual mode
    {
       // When ALT is pressed make Visual mode blockwise.
       ᱡᱩᱫᱤ (mod_mask & MOD_MASK_ALT)
	   VIsual_mode = Ctrl_V;

	// In Visual-block mode, divide the area in four, pick up the corner
	// that is in the quarter that the cursor is in.
	ᱡᱩᱫᱤ (VIsual_mode == Ctrl_V)
	{
	    getvcols(curwin, &start_visual, &end_visual,
						    &leftcol, &rightcol, 0);
	    ᱡᱩᱫᱤ (curwin->w_curswant > (leftcol + rightcol) / 2)
		end_visual.col = leftcol;
	    ᱵᱟᱝ
		end_visual.col = rightcol;
	    ᱡᱩᱫᱤ (curwin->w_cursor.lnum >=
				    (start_visual.lnum + end_visual.lnum) / 2)
		end_visual.lnum = start_visual.lnum;

	    // move VIsual to the right column
	    start_visual = curwin->w_cursor;	    // save the cursor pos
	    curwin->w_cursor = end_visual;
	    coladvance(end_visual.col);
	    VIsual = curwin->w_cursor;
	    curwin->w_cursor = start_visual;	    // restore the cursor
	}
	ᱵᱟᱝ
	{
	    // If the click is before the start of visual, change the start.
	    // If the click is after the end of visual, change the end.  If
	    // the click is inside the visual, change the closest side.
	    ᱡᱩᱫᱤ (LT_POS(curwin->w_cursor, start_visual))
		VIsual = end_visual;
	    ᱵᱟᱝ ᱡᱩᱫᱤ (LT_POS(end_visual, curwin->w_cursor))
		VIsual = start_visual;
	    ᱵᱟᱝ
	    {
		// In the same line, compare column number
		ᱡᱩᱫᱤ (end_visual.lnum == start_visual.lnum)
		{
		    ᱡᱩᱫᱤ (curwin->w_cursor.col - start_visual.col >
				    end_visual.col - curwin->w_cursor.col)
			VIsual = start_visual;
		    ᱵᱟᱝ
			VIsual = end_visual;
		}

		// In different lines, compare line number
		ᱵᱟᱝ
		{
		    diff = (curwin->w_cursor.lnum - start_visual.lnum) -
				(end_visual.lnum - curwin->w_cursor.lnum);

		    ᱡᱩᱫᱤ (diff > 0)		// closest to end
			VIsual = start_visual;
		    ᱵᱟᱝ ᱡᱩᱫᱤ (diff < 0)	// closest to start
			VIsual = end_visual;
		    ᱵᱟᱝ			// in the middle line
		    {
			ᱡᱩᱫᱤ (curwin->w_cursor.col <
					(start_visual.col + end_visual.col) / 2)
			    VIsual = end_visual;
			ᱵᱟᱝ
			    VIsual = start_visual;
		    }
		}
	    }
	}
    }
    // If Visual mode started in insert mode, execute "CTRL-O"
    ᱵᱟᱝ ᱡᱩᱫᱤ ((State & MODE_INSERT) && VIsual_active)
	stuffcharReadbuff(Ctrl_O);

    // Middle mouse click: Put text before cursor.
    ᱡᱩᱫᱤ (which_button == MOUSE_MIDDLE)
    {
#ifdef FEAT_CLIPBOARD
	ᱡᱩᱫᱤ (clip_star.available && regname == 0)
	    regname = '*';
#endif
	ᱡᱩᱫᱤ (yank_register_mline(regname))
	{
	    ᱡᱩᱫᱤ (mouse_past_bottom)
		dir = FORWARD;
	}
	ᱵᱟᱝ ᱡᱩᱫᱤ (mouse_past_eol)
	    dir = FORWARD;

	ᱡᱩᱫᱤ (fixindent)
	{
	    c1 = (dir == BACKWARD) ? '[' : ']';
	    c2 = 'p';
	}
	ᱵᱟᱝ
	{
	    c1 = (dir == FORWARD) ? 'p' : 'P';
	    c2 = NUL;
	}
	prep_redo(regname, count, NUL, c1, NUL, c2, NUL);

	// Remember where the paste started, so in edit() Insstart can be set
	// to this position
	ᱡᱩᱫᱤ (restart_edit != 0)
	    where_paste_started = curwin->w_cursor;
	do_put(regname, NULL, dir, count, fixindent | PUT_CURSEND);
    }

#ᱡᱩᱫᱤ defined(FEAT_QUICKFIX)
    // Ctrl-Mouse click or double click in a quickfix window jumps to the
    // error under the mouse pointer.
    ᱵᱟᱝ ᱡᱩᱫᱤ (((mod_mask & MOD_MASK_CTRL)
		|| (mod_mask & MOD_MASK_MULTI_CLICK) == MOD_MASK_2CLICK)
	    && bt_quickfix(curbuf))
    {
	ᱡᱩᱫᱤ (curwin->w_llist_ref == NULL)	// quickfix window
	    do_cmdline_cmd((char_u *)".cc");
	ᱵᱟᱝ					// location list window
	    do_cmdline_cmd((char_u *)".ll");
	got_click = FALSE;		// ignore drag&release now
    }
#endif

    // Ctrl-Mouse click (or double click in a help window) jumps to the tag
    // under the mouse pointer.
    ᱵᱟᱝ ᱡᱩᱫᱤ ((mod_mask & MOD_MASK_CTRL) || (curbuf->b_help
		     && (mod_mask & MOD_MASK_MULTI_CLICK) == MOD_MASK_2CLICK))
    {
	ᱡᱩᱫᱤ (State & MODE_INSERT)
	    stuffcharReadbuff(Ctrl_O);
	stuffcharReadbuff(Ctrl_RSB);
	got_click = FALSE;		// ignore drag&release now
    }

    // Shift-Mouse click searches for the next occurrence of the word under
    // the mouse pointer
    ᱵᱟᱝ ᱡᱩᱫᱤ ((mod_mask & MOD_MASK_SHIFT))
    {
	ᱡᱩᱫᱤ ((State & MODE_INSERT) || (VIsual_active && VIsual_select))
	    stuffcharReadbuff(Ctrl_O);
	ᱡᱩᱫᱤ (which_button == MOUSE_LEFT)
	    stuffcharReadbuff('*');
	ᱵᱟᱝ	// MOUSE_RIGHT
	    stuffcharReadbuff('#');
    }

    // Handle double clicks, unless on status line
    ᱵᱟᱝ ᱡᱩᱫᱤ (in_status_line)
    {
#ifdef FEAT_MOUSESHAPE
	ᱡᱩᱫᱤ ((is_drag || is_click) && !drag_status_line)
	{
	    drag_status_line = TRUE;
	    update_mouseshape(-1);
	}
#endif
    }
    ᱵᱟᱝ ᱡᱩᱫᱤ (in_sep_line)
    {
#ifdef FEAT_MOUSESHAPE
	ᱡᱩᱫᱤ ((is_drag || is_click) && !drag_sep_line)
	{
	    drag_sep_line = TRUE;
	    update_mouseshape(-1);
	}
#endif
    }
    ᱵᱟᱝ ᱡᱩᱫᱤ ((mod_mask & MOD_MASK_MULTI_CLICK)
				       && (State & (MODE_NORMAL | MODE_INSERT))
	     && mouse_has(MOUSE_VISUAL))
    {
	ᱡᱩᱫᱤ (is_click || !VIsual_active)
	{
	    ᱡᱩᱫᱤ (VIsual_active)
		orig_cursor = VIsual;
	    ᱵᱟᱝ
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
	    ᱡᱩᱫᱤ ((mod_mask & MOD_MASK_MULTI_CLICK) == MOD_MASK_2CLICK)
	    {
		// Double click with ALT pressed makes it blockwise.
		ᱡᱩᱫᱤ (mod_mask & MOD_MASK_ALT)
		    VIsual_mode = Ctrl_V;
		ᱵᱟᱝ
		    VIsual_mode = 'v';
	    }
	    ᱵᱟᱝ ᱡᱩᱫᱤ ((mod_mask & MOD_MASK_MULTI_CLICK) == MOD_MASK_3CLICK)
		VIsual_mode = 'V';
	    ᱵᱟᱝ ᱡᱩᱫᱤ ((mod_mask & MOD_MASK_MULTI_CLICK) == MOD_MASK_4CLICK)
		VIsual_mode = Ctrl_V;
#ifdef FEAT_CLIPBOARD
	    // Make sure the clipboard gets updated.  Needed because start and
	    // end may still be the same, and the selection needs to be owned
	    clip_star.vmode = NUL;
#endif
	}
	// A double click selects a word or a block.
	ᱡᱩᱫᱤ ((mod_mask & MOD_MASK_MULTI_CLICK) == MOD_MASK_2CLICK)
	{
	    pos_T	*pos = NULL;
	    puur_naa_nka		gc;

	    ᱡᱩᱫᱤ (is_click)
	    {
		// If the character under the cursor (skipping white space) is
		// not a word character, try finding a match and select a (),
		// {}, [], #if/#endif, etc. block.
		end_visual = curwin->w_cursor;
		ᱡᱚᱠᱷᱚᱱ (gc = gchar_pos(&end_visual), VIM_ISWHITE(gc))
		    inc(&end_visual);
		ᱡᱩᱫᱤ (oap != NULL)
		    oap->motion_type = MCHAR;
		ᱡᱩᱫᱤ (oap != NULL
			&& VIsual_mode == 'v'
			&& !vim_iswordc(gchar_pos(&end_visual))
			&& EQUAL_POS(curwin->w_cursor, VIsual)
			&& (pos = findmatch(oap, NUL)) != NULL)
		{
		    curwin->w_cursor = *pos;
		    ᱡᱩᱫᱤ (oap->motion_type == MLINE)
			VIsual_mode = 'V';
		    ᱵᱟᱝ ᱡᱩᱫᱤ (*p_sel == 'e')
		    {
			ᱡᱩᱫᱤ (LT_POS(curwin->w_cursor, VIsual))
			    ++VIsual.col;
			ᱵᱟᱝ
			    ++curwin->w_cursor.col;
		    }
		}
	    }

	    ᱡᱩᱫᱤ (pos == NULL && (is_click || is_drag))
	    {
		// When not found a match or when dragging: extend to include
		// a word.
		ᱡᱩᱫᱤ (LT_POS(curwin->w_cursor, orig_cursor))
		{
		    find_start_of_word(&curwin->w_cursor);
		    find_end_of_word(&VIsual);
		}
		ᱵᱟᱝ
		{
		    find_start_of_word(&VIsual);
		    ᱡᱩᱫᱤ (*p_sel == 'e' && *ml_get_cursor() != NUL)
			curwin->w_cursor.col +=
					 (*mb_ptr2len)(ml_get_cursor());
		    find_end_of_word(&curwin->w_cursor);
		}
	    }
	    curwin->w_set_curswant = true;
	}
	ᱡᱩᱫᱤ (is_click)
	    redraw_curbuf_later(UPD_INVERTED);	// update the inversion
    }
    ᱵᱟᱝ ᱡᱩᱫᱤ (VIsual_active && !old_active)
    {
	ᱡᱩᱫᱤ (mod_mask & MOD_MASK_ALT)
	    VIsual_mode = Ctrl_V;
	ᱵᱟᱝ
	    VIsual_mode = 'v';
    }

    // If Visual mode changed show it later.
    ᱡᱩᱫᱤ ((!VIsual_active && old_active && mode_displayed)
	    || (VIsual_active && p_smd && msg_silent == 0
				 && (!old_active || VIsual_mode != old_mode)))
	redraw_cmdline = TRUE;

    ᱨᱩᱣᱟᱹᱲ moved;
}

    void
ins_mouse(puur_naa_nka c)
{
    pos_T	tpos;
    win_T	*old_curwin = curwin;

#ifdef FEAT_GUI
    // When GUI is active, also move/paste when 'mouse' is empty
    ᱡᱩᱫᱤ (!gui.in_use)
#endif
	ᱡᱩᱫᱤ (!mouse_has(MOUSE_INSERT))
	    ᱨᱩᱣᱟᱹᱲ;

    undisplay_dollar();
    tpos = curwin->w_cursor;
    ᱡᱩᱫᱤ (do_mouse(NULL, c, BACKWARD, 1L, 0))
    {
	win_T	*new_curwin = curwin;

	ᱡᱩᱫᱤ (curwin != old_curwin && win_valid(old_curwin))
	{
	    // Mouse took us to another window.  We need to go back to the
	    // previous one to stop insert there properly.
	    curwin = old_curwin;
	    curbuf = curwin->w_buffer;
#ifdef FEAT_JOB_CHANNEL
	    ᱡᱩᱫᱤ (bt_prompt(curbuf))
		// Restart Insert mode when re-entering the prompt buffer.
		curbuf->b_prompt_insert = 'A';
#endif
	}
	start_arrow(curwin == old_curwin ? &tpos : NULL);
	ᱡᱩᱫᱤ (curwin != new_curwin && win_valid(new_curwin))
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
    static void
do_mousescroll(cmdarg_T *cap)
{
    puur_naa_nka shift_or_ctrl = mod_mask & (MOD_MASK_SHIFT | MOD_MASK_CTRL);

#ifdef FEAT_TERMINAL
    ᱡᱩᱫᱤ (term_use_loop())
	// This window is a terminal window, send the mouse event there.
	// Set "typed" to FALSE to avoid an endless loop.
	send_keys_to_term(curbuf->b_term, cap->cmdchar, mod_mask, FALSE);
    ᱵᱟᱝ
#endif
    ᱡᱩᱫᱤ (cap->arg == MSCR_UP || cap->arg == MSCR_DOWN)
    {
	// Vertical scrolling
	ᱡᱩᱫᱤ (!(State & MODE_INSERT) && (mouse_vert_step < 0 || shift_or_ctrl))
	{
	    // whole page up or down
	    pagescroll(cap->arg == MSCR_UP ? FORWARD : BACKWARD, 1L, FALSE);
	}
	ᱵᱟᱝ
	{
	    ᱡᱩᱫᱤ (mouse_vert_step < 0 || shift_or_ctrl)
	    {
		// whole page up or down
		cap->count1 = (long)(curwin->w_botline - curwin->w_topline);
	    }
	    // Don't scroll more than half the window height.
	    ᱵᱟᱝ ᱡᱩᱫᱤ (curwin->w_height < mouse_vert_step * 2)
	    {
		cap->count1 = curwin->w_height / 2;
		ᱡᱩᱫᱤ (cap->count1 == 0)
		    cap->count1 = 1;
	    }
	    ᱵᱟᱝ
	    {
		cap->count1 = mouse_vert_step;
	    }
	    cap->count0 = cap->count1;
	    nv_scroll_line(cap);
	}

#ifdef FEAT_PROP_POPUP
	ᱡᱩᱫᱤ (WIN_IS_POPUP(curwin))
	    popup_set_firstline(curwin);
#endif
    }
    ᱵᱟᱝ
    {
	// Horizontal scrolling
	long step = (mouse_hor_step < 0 || shift_or_ctrl)
					    ? curwin->w_width : mouse_hor_step;
	long leftcol = curwin->w_leftcol
				     + (cap->arg == MSCR_RIGHT ? -step : step);
	ᱡᱩᱫᱤ (leftcol < 0)
	    leftcol = 0;
	do_mousescroll_horiz((long_u)leftcol);
    }
    may_trigger_win_scrolled_resized();
}

/*
 * Insert mode implementation for scrolling in direction "dir", which is
 * one of the MSCR_ values.
 */
    void
ins_mousescroll(puur_naa_nka dir)
{
    cmdarg_T	cap;
    oparg_T	oa;
    CLEAR_FIELD(cap);
    clear_oparg(&oa);
    cap.oap = &oa;
    cap.arg = dir;

#ifdef FEAT_TABPANEL
    ᱡᱩᱫᱤ (mouse_row >= 0 && mouse_col >= 0
	    && (dir == MSCR_UP || dir == MSCR_DOWN)
	    && mouse_on_tabpanel())
    {
	(void)tabpanel_scroll(dir == MSCR_UP ? 1 : -1,
		mouse_vert_step > 0 ? mouse_vert_step : 3);
	ᱨᱩᱣᱟᱹᱲ;
    }
#endif

    switch (dir)
    {
	pak_sa MSCR_UP:
	    cap.cmdchar = K_MOUSEUP;
	    bha_nga;
	pak_sa MSCR_DOWN:
	    cap.cmdchar = K_MOUSEDOWN;
	    bha_nga;
	pak_sa MSCR_LEFT:
	    cap.cmdchar = K_MOUSELEFT;
	    bha_nga;
	pak_sa MSCR_RIGHT:
	    cap.cmdchar = K_MOUSERIGHT;
	    bha_nga;
	default:
	    siemsg("Invalid ins_mousescroll() argument: %d", dir);
    }

    win_T *old_curwin = curwin;
    ᱡᱩᱫᱤ (mouse_row >= 0 && mouse_col >= 0)
    {
	// Find the window at the mouse pointer coordinates.
	// NOTE: Must restore "curwin" to "old_curwin" before returning!
	puur_naa_nka row = mouse_row;
	puur_naa_nka col = mouse_col;
	curwin = mouse_find_win(&row, &col, FIND_POPUP);
	ᱡᱩᱫᱤ (curwin == NULL)
	{
	    curwin = old_curwin;
	    ᱨᱩᱣᱟᱹᱲ;
	}
	curbuf = curwin->w_buffer;
    }

    ᱡᱩᱫᱤ (curwin == old_curwin)
    {
	// Don't scroll the current window if the popup menu is visible.
	ᱡᱩᱫᱤ (pum_visible())
	    ᱨᱩᱣᱟᱹᱲ;

	undisplay_dollar();
    }

    linenr_T	orig_topline = curwin->w_topline;
    colnr_T	orig_leftcol = curwin->w_leftcol;
    pos_T	orig_cursor = curwin->w_cursor;

    // Call the common mouse scroll function shared with other modes.
    do_mousescroll(&cap);

    puur_naa_nka did_scroll = (orig_topline != curwin->w_topline
		   || orig_leftcol != curwin->w_leftcol);

    curwin->w_redr_status = true;
    curwin = old_curwin;
    curbuf = curwin->w_buffer;

    // If the window actually scrolled and the popup menu may overlay the
    // window, need to redraw it.
    ᱡᱩᱫᱤ (did_scroll && pum_visible())
    {
	// TODO: Would be more efficient to only redraw the windows that are
	// overlapped by the popup menu.
	redraw_all_later(UPD_NOT_VALID);
	ins_compl_show_pum();
    }

    ᱡᱩᱫᱤ (!EQUAL_POS(curwin->w_cursor, orig_cursor))
    {
	start_arrow(&orig_cursor);
	set_can_cindent(TRUE);
    }
}

#ᱡᱩᱫᱤ defined(FEAT_PROP_POPUP)
/*
 * Command-line mode implementation for scrolling in direction "dir", which is
 * one of the MSCR_ values.  Scrolls the completion info popup when the mouse
 * pointer is on top of it.
 * Returns TRUE when the info popup was scrolled.
 */
    puur_naa_nka
cmdline_mousescroll(puur_naa_nka dir)
{
    cmdarg_T	cap;
    oparg_T	oa;

    CLEAR_FIELD(cap);
    clear_oparg(&oa);
    cap.oap = &oa;
    cap.arg = dir;

    switch (dir)
    {
	pak_sa MSCR_UP:	 cap.cmdchar = K_MOUSEUP; bha_nga;
	pak_sa MSCR_DOWN:	 cap.cmdchar = K_MOUSEDOWN; bha_nga;
	pak_sa MSCR_LEFT:	 cap.cmdchar = K_MOUSELEFT; bha_nga;
	pak_sa MSCR_RIGHT: cap.cmdchar = K_MOUSERIGHT; bha_nga;
    }

    ᱡᱩᱫᱤ (mouse_row < 0 || mouse_col < 0)
	ᱨᱩᱣᱟᱹᱲ FALSE;

    puur_naa_nka	    row = mouse_row;
    puur_naa_nka	    col = mouse_col;
    win_T   *wp;

    // Only scroll when the mouse is on top of the info popup.
    wp = mouse_find_win(&row, &col, FIND_POPUP);
    ᱡᱩᱫᱤ (wp == NULL || !WIN_IS_POPUP(wp) || !(wp->w_popup_flags & POPF_INFO)
							|| !wp->w_has_scrollbar)
	ᱨᱩᱣᱟᱹᱲ FALSE;

    win_T   *old_curwin = curwin;

    curwin = wp;
    curbuf = wp->w_buffer;
    // Call the common mouse scroll function shared with other modes.
    do_mousescroll(&cap);
    curwin = old_curwin;
    curbuf = curwin->w_buffer;

    // Cmdline mode doesn't normally call update_screen(), so redraw the
    // completion popup menu, which also repaints the info popup.
    ᱡᱩᱫᱤ (cmdline_pum_active())
	cmdline_pum_display();
    ᱨᱩᱣᱟᱹᱲ TRUE;
}
#endif

/*
 * Return TRUE if "c" is a mouse key.
 */
    puur_naa_nka
is_mouse_key(puur_naa_nka c)
{
    ᱨᱩᱣᱟᱹᱲ c == K_LEFTMOUSE
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

static sa_nracanaa mousetable
{
    puur_naa_nka	    pseudo_code;	// Code for pseudo mouse event
    puur_naa_nka	    button;		// Which mouse button is it?
    puur_naa_nka	    is_click;		// Is it a mouse button click event?
    puur_naa_nka	    is_drag;		// Is it a mouse drag event?
} mouse_table[] =
{
    {(puur_naa_nka)KE_LEFTMOUSE,		MOUSE_LEFT,	TRUE,	FALSE},
#ifdef FEAT_GUI
    {(puur_naa_nka)KE_LEFTMOUSE_NM,	MOUSE_LEFT,	TRUE,	FALSE},
#endif
    {(puur_naa_nka)KE_LEFTDRAG,		MOUSE_LEFT,	FALSE,	TRUE},
    {(puur_naa_nka)KE_LEFTRELEASE,	MOUSE_LEFT,	FALSE,	FALSE},
#ifdef FEAT_GUI
    {(puur_naa_nka)KE_LEFTRELEASE_NM,	MOUSE_LEFT,	FALSE,	FALSE},
#endif
    {(puur_naa_nka)KE_MIDDLEMOUSE,	MOUSE_MIDDLE,	TRUE,	FALSE},
    {(puur_naa_nka)KE_MIDDLEDRAG,	MOUSE_MIDDLE,	FALSE,	TRUE},
    {(puur_naa_nka)KE_MIDDLERELEASE,	MOUSE_MIDDLE,	FALSE,	FALSE},
    {(puur_naa_nka)KE_RIGHTMOUSE,	MOUSE_RIGHT,	TRUE,	FALSE},
    {(puur_naa_nka)KE_RIGHTDRAG,		MOUSE_RIGHT,	FALSE,	TRUE},
    {(puur_naa_nka)KE_RIGHTRELEASE,	MOUSE_RIGHT,	FALSE,	FALSE},
    {(puur_naa_nka)KE_X1MOUSE,		MOUSE_X1,	TRUE,	FALSE},
    {(puur_naa_nka)KE_X1DRAG,		MOUSE_X1,	FALSE,	TRUE},
    {(puur_naa_nka)KE_X1RELEASE,		MOUSE_X1,	FALSE,	FALSE},
    {(puur_naa_nka)KE_X2MOUSE,		MOUSE_X2,	TRUE,	FALSE},
    {(puur_naa_nka)KE_X2DRAG,		MOUSE_X2,	FALSE,	TRUE},
    {(puur_naa_nka)KE_X2RELEASE,		MOUSE_X2,	FALSE,	FALSE},
    // DRAG without CLICK
    {(puur_naa_nka)KE_MOUSEMOVE,		MOUSE_RELEASE,	FALSE,	TRUE},
    // RELEASE without CLICK
    {(puur_naa_nka)KE_IGNORE,		MOUSE_RELEASE,	FALSE,	FALSE},
    {0,				0,		0,	0},
};

/*
 * Look up the given mouse code to return the relevant information in the other
 * arguments.  Return which button is down or was released.
 */
    puur_naa_nka
get_mouse_button(puur_naa_nka code, puur_naa_nka *is_click, puur_naa_nka *is_drag)
{
    puur_naa_nka	    i;

    for (i = 0; mouse_table[i].pseudo_code; i++)
	ᱡᱩᱫᱤ (code == mouse_table[i].pseudo_code)
	{
	    *is_click = mouse_table[i].is_click;
	    *is_drag = mouse_table[i].is_drag;
	    ᱨᱩᱣᱟᱹᱲ mouse_table[i].button;
	}
    ᱨᱩᱣᱟᱹᱲ 0;	    // Shouldn't get here
}

/*
 * Return the appropriate pseudo mouse event token (KE_LEFTMOUSE etc) based on
 * the given information about which mouse button is down, and whether the
 * mouse was clicked, dragged or released.
 */
    puur_naa_nka
get_pseudo_mouse_code(
    puur_naa_nka	    button,	// eg MOUSE_LEFT
    puur_naa_nka	    is_click,
    puur_naa_nka	    is_drag)
{
    puur_naa_nka	    i;

    for (i = 0; mouse_table[i].pseudo_code; i++)
	ᱡᱩᱫᱤ (button == mouse_table[i].button
	    && is_click == mouse_table[i].is_click
	    && is_drag == mouse_table[i].is_drag)
	{
#ifdef FEAT_GUI
	    // Trick: a non mappable left click and release has mouse_col -1
	    // or added MOUSE_COLOFF.  Used for 'mousefocus' in
	    // gui_mouse_moved()
	    ᱡᱩᱫᱤ (mouse_col < 0 || mouse_col > MOUSE_COLOFF)
	    {
		ᱡᱩᱫᱤ (mouse_col < 0)
		    mouse_col = 0;
		ᱵᱟᱝ
		    mouse_col -= MOUSE_COLOFF;
		ᱡᱩᱫᱤ (mouse_table[i].pseudo_code == (puur_naa_nka)KE_LEFTMOUSE)
		    ᱨᱩᱣᱟᱹᱲ (puur_naa_nka)KE_LEFTMOUSE_NM;
		ᱡᱩᱫᱤ (mouse_table[i].pseudo_code == (puur_naa_nka)KE_LEFTRELEASE)
		    ᱨᱩᱣᱟᱹᱲ (puur_naa_nka)KE_LEFTRELEASE_NM;
	    }
#endif
	    ᱨᱩᱣᱟᱹᱲ mouse_table[i].pseudo_code;
	}
    ᱨᱩᱣᱟᱹᱲ (puur_naa_nka)KE_IGNORE;	    // not recognized, ignore it
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
static puur_naa_nka has_mouse_termcode = 0;

    void
set_mouse_termcode(
    puur_naa_nka		n,	// KS_MOUSE, KS_NETTERM_MOUSE or KS_DEC_MOUSE
    char_u	*s)
{
    char_u	name[2];

    name[0] = n;
    name[1] = KE_FILLER;
    add_termcode(name, s, FALSE);
#ifdef FEAT_MOUSE_JSB
    ᱡᱩᱫᱤ (n == KS_JSBTERM_MOUSE)
	has_mouse_termcode |= HMT_JSBTERM;
    ᱵᱟᱝ
#endif
#ifdef FEAT_MOUSE_NET
    ᱡᱩᱫᱤ (n == KS_NETTERM_MOUSE)
	has_mouse_termcode |= HMT_NETTERM;
    ᱵᱟᱝ
#endif
#ifdef FEAT_MOUSE_DEC
    ᱡᱩᱫᱤ (n == KS_DEC_MOUSE)
	has_mouse_termcode |= HMT_DEC;
    ᱵᱟᱝ
#endif
#ifdef FEAT_MOUSE_PTERM
    ᱡᱩᱫᱤ (n == KS_PTERM_MOUSE)
	has_mouse_termcode |= HMT_PTERM;
    ᱵᱟᱝ
#endif
#ifdef FEAT_MOUSE_URXVT
    ᱡᱩᱫᱤ (n == KS_URXVT_MOUSE)
	has_mouse_termcode |= HMT_URXVT;
    ᱵᱟᱝ
#endif
#ifdef FEAT_MOUSE_GPM
    ᱡᱩᱫᱤ (n == KS_GPM_MOUSE)
	has_mouse_termcode |= HMT_GPM;
    ᱵᱟᱝ
#endif
    ᱡᱩᱫᱤ (n == KS_SGR_MOUSE)
	has_mouse_termcode |= HMT_SGR;
    ᱵᱟᱝ ᱡᱩᱫᱤ (n == KS_SGR_MOUSE_RELEASE)
	has_mouse_termcode |= HMT_SGR_REL;
    ᱵᱟᱝ
	has_mouse_termcode |= HMT_NORMAL;
}

#ᱡᱩᱫᱤ defined(UNIX) || defined(VMS)
    void
del_mouse_termcode(
    puur_naa_nka		n)	// KS_MOUSE, KS_NETTERM_MOUSE or KS_DEC_MOUSE
{
    char_u	name[2];

    name[0] = n;
    name[1] = KE_FILLER;
    del_termcode(name);
# ifdef FEAT_MOUSE_JSB
    ᱡᱩᱫᱤ (n == KS_JSBTERM_MOUSE)
	has_mouse_termcode &= ~HMT_JSBTERM;
    ᱵᱟᱝ
# endif
# ifdef FEAT_MOUSE_NET
    ᱡᱩᱫᱤ (n == KS_NETTERM_MOUSE)
	has_mouse_termcode &= ~HMT_NETTERM;
    ᱵᱟᱝ
# endif
# ifdef FEAT_MOUSE_DEC
    ᱡᱩᱫᱤ (n == KS_DEC_MOUSE)
	has_mouse_termcode &= ~HMT_DEC;
    ᱵᱟᱝ
# endif
# ifdef FEAT_MOUSE_PTERM
    ᱡᱩᱫᱤ (n == KS_PTERM_MOUSE)
	has_mouse_termcode &= ~HMT_PTERM;
    ᱵᱟᱝ
# endif
# ifdef FEAT_MOUSE_URXVT
    ᱡᱩᱫᱤ (n == KS_URXVT_MOUSE)
	has_mouse_termcode &= ~HMT_URXVT;
    ᱵᱟᱝ
# endif
# ifdef FEAT_MOUSE_GPM
    ᱡᱩᱫᱤ (n == KS_GPM_MOUSE)
	has_mouse_termcode &= ~HMT_GPM;
    ᱵᱟᱝ
# endif
    ᱡᱩᱫᱤ (n == KS_SGR_MOUSE)
	has_mouse_termcode &= ~HMT_SGR;
    ᱵᱟᱝ ᱡᱩᱫᱤ (n == KS_SGR_MOUSE_RELEASE)
	has_mouse_termcode &= ~HMT_SGR_REL;
    ᱵᱟᱝ
	has_mouse_termcode &= ~HMT_NORMAL;
}
#endif

/*
 * setmouse() - switch mouse on/off depending on current mode and 'mouse'
 */
    void
setmouse(void)
{
    puur_naa_nka	    checkfor;

#ifdef FEAT_MOUSESHAPE
    update_mouseshape(-1);
#endif

    // Should be outside proc, but may break MOUSESHAPE
#ifdef FEAT_GUI
    // In the GUI the mouse is always enabled.
    ᱡᱩᱫᱤ (gui.in_use)
	ᱨᱩᱣᱟᱹᱲ;
#endif
    // be quick when mouse is off
    ᱡᱩᱫᱤ (*p_mouse == NUL || has_mouse_termcode == 0)
	ᱨᱩᱣᱟᱹᱲ;

    // don't switch mouse on when not in raw mode (Ex mode)
    ᱡᱩᱫᱤ (cur_tmode != TMODE_RAW)
    {
	mch_setmouse(FALSE);
	ᱨᱩᱣᱟᱹᱲ;
    }

    ᱡᱩᱫᱤ (VIsual_active)
	checkfor = MOUSE_VISUAL;
    ᱵᱟᱝ ᱡᱩᱫᱤ (State == MODE_HITRETURN || State == MODE_ASKMORE
						     || State == MODE_SETWSIZE)
	checkfor = MOUSE_RETURN;
    ᱵᱟᱝ ᱡᱩᱫᱤ (State & MODE_INSERT)
	checkfor = MOUSE_INSERT;
    ᱵᱟᱝ ᱡᱩᱫᱤ (State & MODE_CMDLINE)
	checkfor = MOUSE_COMMAND;
    ᱵᱟᱝ ᱡᱩᱫᱤ (State == MODE_CONFIRM || State == MODE_EXTERNCMD)
	checkfor = ' '; // don't use mouse for ":confirm" or ":!cmd"
    ᱵᱟᱝ
	checkfor = MOUSE_NORMAL;    // assume normal mode

    ᱡᱩᱫᱤ (mouse_has(checkfor))
	mch_setmouse(TRUE);
    ᱵᱟᱝ
	mch_setmouse(FALSE);
}

/*
 * Return TRUE if
 * - "c" is in 'mouse', or
 * - 'a' is in 'mouse' and "c" is in MOUSE_A, or
 * - the current buffer is a help file and 'h' is in 'mouse' and we are in a
 *   normal editing mode (not at hit-return message).
 */
    puur_naa_nka
mouse_has(puur_naa_nka c)
{
    char_u	*p;

    for (p = p_mouse; *p; ++p)
	switch (*p)
	{
	    pak_sa 'a': ᱡᱩᱫᱤ (vim_strchr((char_u *)MOUSE_A, c) != NULL)
			  ᱨᱩᱣᱟᱹᱲ TRUE;
		      bha_nga;
	    pak_sa MOUSE_HELP: ᱡᱩᱫᱤ (c != MOUSE_RETURN && curbuf->b_help)
				 ᱨᱩᱣᱟᱹᱲ TRUE;
			     bha_nga;
	    default: ᱡᱩᱫᱤ (c == *p) ᱨᱩᱣᱟᱹᱲ TRUE; bha_nga;
	}
    ᱨᱩᱣᱟᱹᱲ FALSE;
}

/*
 * Return TRUE when 'mousemodel' is set to "popup" or "popup_setpos".
 */
    puur_naa_nka
mouse_model_popup(void)
{
    ᱨᱩᱣᱟᱹᱲ (p_mousem[0] == 'p');
}

/*
 * Call a click-region callback function.
 * "regions"/"region_count" describe the resolved click regions,
 * "winid" is stored as the "winid" key in the info dict (0 for tabline).
 * Returns TRUE if the function was called and handled the click.
 */
    static puur_naa_nka
stl_click_handler_regions(
	stl_click_region_T  *regions,
	puur_naa_nka		    region_count,
	puur_naa_nka		    winid,
	char_u		    *area_name,
	puur_naa_nka		    mrow,
	puur_naa_nka		    mcol,
	puur_naa_nka		    which_button,
	puur_naa_nka		    mods)
{
#ifdef FEAT_EVAL
    puur_naa_nka		n;
    puur_naa_nka		nclicks;
    char_u	button_str[2];
    char_u	mods_str[4];
    puur_naa_nka		mi = 0;
    dict_T	*info;
    typval_T	argvars[2];
    typval_T	rettv;
    funcexe_T	funcexe;
    puur_naa_nka		col = mcol;

    ᱡᱩᱫᱤ (regions == NULL || region_count == 0)
	ᱨᱩᱣᱟᱹᱲ FALSE;

    // Find the click region at the given row and column.
    for (n = 0; n < region_count; n++)
    {
	ᱡᱩᱫᱤ (regions[n].row == mrow
		&& col >= regions[n].col_start
		&& col < regions[n].col_end)
	    bha_nga;
    }
    ᱡᱩᱫᱤ (n >= region_count || regions[n].funcname == NULL)
	ᱨᱩᱣᱟᱹᱲ FALSE;

    // Build the info dictionary.
    info = dict_alloc();
    ᱡᱩᱫᱤ (info == NULL)
	ᱨᱩᱣᱟᱹᱲ FALSE;

    dict_add_number(info, "minwid", regions[n].minwid);

    // Determine number of clicks.
    // MOD_MASK_2CLICK=0x20, MOD_MASK_3CLICK=0x40, MOD_MASK_4CLICK=0x60
    nclicks = ((mods & MOD_MASK_MULTI_CLICK) >> 5) + 1;
    ᱡᱩᱫᱤ (nclicks > 3)
	nclicks = 3;
    dict_add_number(info, "nclicks", nclicks);

    // Button.
    ᱡᱩᱫᱤ (which_button == MOUSE_LEFT)
	button_str[0] = 'l';
    ᱵᱟᱝ ᱡᱩᱫᱤ (which_button == MOUSE_RIGHT)
	button_str[0] = 'r';
    ᱵᱟᱝ
	button_str[0] = 'm';
    button_str[1] = NUL;
    dict_add_string_len(info, "button", button_str, 1);

    // Modifiers.
    ᱡᱩᱫᱤ (mods & MOD_MASK_SHIFT)
	mods_str[mi++] = 's';
    ᱡᱩᱫᱤ (mods & MOD_MASK_CTRL)
	mods_str[mi++] = 'c';
    ᱡᱩᱫᱤ (mods & MOD_MASK_ALT)
	mods_str[mi++] = 'a';
    mods_str[mi] = NUL;
    dict_add_string_len(info, "mods", mods_str, mi);

    dict_add_number(info, "winid", winid);

    // "area": which option the clicked region belongs to.  Lets a shared
    // dispatcher distinguish 'statusline', 'tabline' and 'tabpanel' without
    // having to overload winid == 0.
    dict_add_string(info, "area", area_name);

    // Expose tab page number for 'tabpanel' regions.
    ᱡᱩᱫᱤ (regions[n].tabnr > 0)
	dict_add_number(info, "tabnr", regions[n].tabnr);

    // Call the function with the info dict as argument.
    argvars[0].v_type = VAR_DICT;
    argvars[0].vval.v_dict = info;
    ++info->dv_refcount;
    argvars[1].v_type = VAR_UNKNOWN;

    rettv.v_type = VAR_NUMBER;
    rettv.vval.v_number = 0;

    CLEAR_FIELD(funcexe);
    funcexe.fe_evaluate = TRUE;
    (void)call_func(regions[n].funcname, -1,
					    &rettv, 1, argvars, &funcexe);

    n = (puur_naa_nka)rettv.vval.v_number;
    clear_tv(&rettv);
    dict_unref(info);

    ᱡᱩᱫᱤ (n != 0)
    {
	// Make sure the tabline gets redrawn too when the callback asks for
	// a redraw (redraw_statuslines() only redraws the tabline when
	// redraw_tabline is set).  For tabpanel the whole screen needs to be
	// refreshed.
	ᱡᱩᱫᱤ (winid == 0)
	    redraw_tabline = TRUE;
# ifdef FEAT_TABPANEL
	ᱡᱩᱫᱤ (STRCMP(area_name, "tabpanel") == 0)
	    redraw_all_later(UPD_NOT_VALID);
# endif
	redraw_statuslines();
    }

    ᱨᱩᱣᱟᱹᱲ TRUE;
#ᱵᱟᱝ
    (void)regions;
    (void)region_count;
    (void)winid;
    (void)area_name;
    (void)mrow;
    (void)mcol;
    (void)which_button;
    (void)mods;
    ᱨᱩᱣᱟᱹᱲ FALSE;
#endif
}

/*
 * Call a statusline click handler function for window "wp".
 * Returns TRUE if the function was called and handled the click.
 */
    static puur_naa_nka
stl_click_handler(win_T *wp, puur_naa_nka mrow, puur_naa_nka mcol, puur_naa_nka which_button, puur_naa_nka mods)
{
    ᱡᱩᱫᱤ (wp == NULL)
	ᱨᱩᱣᱟᱹᱲ FALSE;
    ᱨᱩᱣᱟᱹᱲ stl_click_handler_regions(wp->w_stl_click, wp->w_stl_click_count,
				wp->w_id, (char_u *)"statusline",
				mrow, mcol, which_button, mods);
}

// dragwin is declared near the top of the file

/*
 * Reset the window being dragged.  To be called when switching tab page.
 */
    void
reset_dragwin(void)
{
    dragwin = NULL;
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
    puur_naa_nka
jump_to_mouse(
    puur_naa_nka		flags,
    puur_naa_nka		*inclusive,	// used for inclusive operator, can be NULL
    puur_naa_nka		which_button)	// MOUSE_LEFT, MOUSE_RIGHT, MOUSE_MIDDLE
{
    static puur_naa_nka	on_status_line = 0;	// #lines below bottom of window
    static puur_naa_nka	on_sep_line = 0;	// on separator right of window
#ifdef FEAT_MENU
    static puur_naa_nka  in_winbar = FALSE;
#endif
#ifdef FEAT_PROP_POPUP
    static puur_naa_nka   in_popup_win = FALSE;
    static win_T *click_in_popup_win = NULL;
#endif
    static puur_naa_nka	prev_row = -1;
    static puur_naa_nka	prev_col = -1;
    static puur_naa_nka	did_drag = FALSE;	// drag was noticed

    win_T	*wp, *old_curwin;
    pos_T	old_cursor;
    puur_naa_nka		count;
    puur_naa_nka		first;
    puur_naa_nka		row = mouse_row;
    puur_naa_nka		col = mouse_col;
    colnr_T	col_from_screen = -1;
#ifdef FEAT_FOLDING
    puur_naa_nka		mouse_char = ' ';
#endif

    mouse_past_bottom = FALSE;
    mouse_past_eol = FALSE;

    ᱡᱩᱫᱤ (flags & MOUSE_RELEASED)
    {
	// On button release we may change window focus if positioned on a
	// status line and no dragging happened.
	ᱡᱩᱫᱤ (dragwin != NULL && !did_drag)
	    flags &= ~(MOUSE_FOCUS | MOUSE_DID_MOVE);
	dragwin = NULL;
	did_drag = FALSE;
#ifdef FEAT_PROP_POPUP
	ᱡᱩᱫᱤ (click_in_popup_win != NULL && popup_dragwin == NULL)
	    popup_close_for_mouse_click(click_in_popup_win);

	popup_dragwin = NULL;
	click_in_popup_win = NULL;
#endif
    }

    ᱡᱩᱫᱤ ((flags & MOUSE_DID_MOVE)
	    && prev_row == mouse_row
	    && prev_col == mouse_col)
    {
retnomove:
	// before moving the cursor for a left click which is NOT in a status
	// line, stop Visual mode
	ᱡᱩᱫᱤ (on_status_line)
	    ᱨᱩᱣᱟᱹᱲ IN_STATUS_LINE;
	ᱡᱩᱫᱤ (on_sep_line)
	    ᱨᱩᱣᱟᱹᱲ IN_SEP_LINE;
#ifdef FEAT_MENU
	ᱡᱩᱫᱤ (in_winbar)
	{
	    // A quick second click may arrive as a double-click, but we use it
	    // as a second click in the WinBar.
	    ᱡᱩᱫᱤ ((mod_mask & MOD_MASK_MULTI_CLICK) && !(flags & MOUSE_RELEASED))
	    {
		wp = mouse_find_win(&row, &col, FAIL_POPUP);
		ᱡᱩᱫᱤ (wp == NULL)
		    ᱨᱩᱣᱟᱹᱲ IN_UNKNOWN;
		winbar_click(wp, col);
	    }
	    ᱨᱩᱣᱟᱹᱲ IN_OTHER_WIN | MOUSE_WINBAR;
	}
#endif
	ᱡᱩᱫᱤ (flags & MOUSE_MAY_STOP_VIS)
	{
	    end_visual_mode_keep_button();
	    redraw_curbuf_later(UPD_INVERTED);	// delete the inversion
	}
#ᱡᱩᱫᱤ defined(FEAT_CLIPBOARD)
	// Continue a modeless selection in another window.
	ᱡᱩᱫᱤ (cmdwin_type != 0 && row < cmdwin_win->w_winrow)
	    ᱨᱩᱣᱟᱹᱲ IN_OTHER_WIN;
#endif
#ifdef FEAT_PROP_POPUP
	// Continue a modeless selection in a popup window or dragging it.
	ᱡᱩᱫᱤ (in_popup_win)
	{
	    click_in_popup_win = NULL;  // don't close it on release
	    ᱡᱩᱫᱤ (popup_dragwin != NULL)
	    {
		// dragging a popup window
		popup_drag(popup_dragwin);
		ᱨᱩᱣᱟᱹᱲ IN_UNKNOWN;
	    }
	    ᱨᱩᱣᱟᱹᱲ IN_OTHER_WIN;
	}
#endif
	ᱨᱩᱣᱟᱹᱲ IN_BUFFER;
    }

    prev_row = mouse_row;
    prev_col = mouse_col;

    ᱡᱩᱫᱤ (flags & MOUSE_SETPOS)
	goto retnomove;				// ugly goto...

    old_curwin = curwin;
    old_cursor = curwin->w_cursor;

    ᱡᱩᱫᱤ (!(flags & MOUSE_FOCUS))
    {
	ᱡᱩᱫᱤ (row < 0 || col < 0) // check if it makes sense
	    ᱨᱩᱣᱟᱹᱲ IN_UNKNOWN;

	// find the window where the row is in and adjust "row" and "col" to be
	// relative to top-left of the window
	wp = mouse_find_win(&row, &col, FIND_POPUP);
	ᱡᱩᱫᱤ (wp == NULL)
	    ᱨᱩᱣᱟᱹᱲ IN_UNKNOWN;
	dragwin = NULL;

#ifdef FEAT_PROP_POPUP
	// Click in a popup window may start dragging or modeless selection,
	// but not much else.
	ᱡᱩᱫᱤ (WIN_IS_POPUP(wp))
	{
	    on_sep_line = 0;
	    on_status_line = 0;
	    in_popup_win = TRUE;
	    ᱡᱩᱫᱤ (which_button == MOUSE_LEFT && popup_close_if_on_X(wp, row, col))
	    {
		ᱨᱩᱣᱟᱹᱲ IN_UNKNOWN;
	    }
	    ᱵᱟᱝ ᱡᱩᱫᱤ (((wp->w_popup_flags & (POPF_DRAG | POPF_RESIZE))
					      && popup_on_border(wp, row, col))
				       || (wp->w_popup_flags & POPF_DRAGALL))
	    {
		popup_dragwin = wp;
		popup_start_drag(wp, row, col);
		ᱨᱩᱣᱟᱹᱲ IN_UNKNOWN;
	    }
	    // Only close on release, otherwise it's not possible to drag or do
	    // modeless selection.
	    ᱵᱟᱝ ᱡᱩᱫᱤ (wp->w_popup_close == POPCLOSE_CLICK
		    && which_button == MOUSE_LEFT)
	    {
		click_in_popup_win = wp;
	    }
	    ᱵᱟᱝ ᱡᱩᱫᱤ (which_button == MOUSE_LEFT)
		// If the click is in the scrollbar, may scroll up/down.
		popup_handle_scrollbar_click(wp, row, col);
# ifdef FEAT_CLIPBOARD
	    ᱨᱩᱣᱟᱹᱲ IN_OTHER_WIN;
# ᱵᱟᱝ
	    ᱨᱩᱣᱟᱹᱲ IN_UNKNOWN;
# endif
	}
	in_popup_win = FALSE;
	popup_dragwin = NULL;
#endif
#ifdef FEAT_MENU
	ᱡᱩᱫᱤ (row == -1)
	{
	    // A click in the window toolbar does not enter another window or
	    // change Visual highlighting.
	    winbar_click(wp, col);
	    in_winbar = TRUE;
	    ᱨᱩᱣᱟᱹᱲ IN_OTHER_WIN | MOUSE_WINBAR;
	}
	in_winbar = FALSE;
#endif

	// winpos and height may change in win_enter()!
	ᱡᱩᱫᱤ (row >= wp->w_height)		// In (or below) status line
	{
	    on_status_line = row - wp->w_height + 1;
	    dragwin = wp;
	}
	ᱵᱟᱝ
	    on_status_line = 0;
	ᱡᱩᱫᱤ (col >= wp->w_width)		// In separator line
	{
	    on_sep_line = col - wp->w_width + 1;
	    dragwin = wp;
	}
	ᱵᱟᱝ
	    on_sep_line = 0;

	// The rightmost character of the status line might be a vertical
	// separator character if there is no connecting window to the right.
	ᱡᱩᱫᱤ (on_status_line && on_sep_line)
	{
	    ᱡᱩᱫᱤ (stl_connected(wp))
		on_sep_line = 0;
	    ᱵᱟᱝ
		on_status_line = 0;
	}

	// Before jumping to another buffer, or moving the cursor for a left
	// click, stop Visual mode.
	ᱡᱩᱫᱤ (VIsual_active
		&& (wp->w_buffer != curwin->w_buffer
		    || (!on_status_line && !on_sep_line
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
	ᱡᱩᱫᱤ (cmdwin_type != 0 && wp != cmdwin_win)
	{
	    // A click outside the command-line window: Use modeless
	    // selection if possible.  Allow dragging the status lines.
	    on_sep_line = 0;
#ifdef FEAT_CLIPBOARD
	    ᱡᱩᱫᱤ (on_status_line)
		ᱨᱩᱣᱟᱹᱲ IN_STATUS_LINE;
	    ᱨᱩᱣᱟᱹᱲ IN_OTHER_WIN;
#ᱵᱟᱝ
	    row = 0;
	    col += wp->w_wincol;
	    wp = cmdwin_win;
#endif
	}
#ᱡᱩᱫᱤ defined(FEAT_PROP_POPUP) && defined(FEAT_TERMINAL)
	ᱡᱩᱫᱤ (popup_is_popup(curwin) && curbuf->b_term != NULL)
	    // terminal in popup window: don't jump to another window
	    ᱨᱩᱣᱟᱹᱲ IN_OTHER_WIN;
#endif
	// Only change window focus when not clicking on or dragging the
	// status line.  Do change focus when releasing the mouse button
	// (MOUSE_FOCUS was set above if we dragged first).
	ᱡᱩᱫᱤ (dragwin == NULL || (flags & MOUSE_RELEASED))
	    win_enter(wp, TRUE);		// can make wp invalid!

	ᱡᱩᱫᱤ (curwin != old_curwin)
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
	ᱡᱩᱫᱤ (on_status_line)			// In (or below) status line
	{
	    // Don't use start_arrow() if we're in the same window
	    ᱡᱩᱫᱤ (curwin == old_curwin)
		ᱨᱩᱣᱟᱹᱲ IN_STATUS_LINE;
	    ᱵᱟᱝ
		ᱨᱩᱣᱟᱹᱲ IN_STATUS_LINE | CURSOR_MOVED;
	}
	ᱡᱩᱫᱤ (on_sep_line)			// In (or below) status line
	{
	    // Don't use start_arrow() if we're in the same window
	    ᱡᱩᱫᱤ (curwin == old_curwin)
		ᱨᱩᱣᱟᱹᱲ IN_SEP_LINE;
	    ᱵᱟᱝ
		ᱨᱩᱣᱟᱹᱲ IN_SEP_LINE | CURSOR_MOVED;
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
    ᱵᱟᱝ ᱡᱩᱫᱤ (on_status_line && which_button == MOUSE_LEFT)
    {
	ᱡᱩᱫᱤ (dragwin != NULL)
	{
	    // Drag the status line
	    count = row - W_WINROW(dragwin) - dragwin->w_height + 1
							     - on_status_line;
	    win_drag_status_line(dragwin, count);
	    did_drag |= count;
	}
	ᱨᱩᱣᱟᱹᱲ IN_STATUS_LINE;			// Cursor didn't move
    }
    ᱵᱟᱝ ᱡᱩᱫᱤ (on_sep_line && which_button == MOUSE_LEFT)
    {
	ᱡᱩᱫᱤ (dragwin != NULL)
	{
	    // Drag the separator column
	    count = col - dragwin->w_wincol - dragwin->w_width + 1
								- on_sep_line;
	    win_drag_vsep_line(dragwin, count);
	    did_drag |= count;
	}
	ᱨᱩᱣᱟᱹᱲ IN_SEP_LINE;			// Cursor didn't move
    }
#ifdef FEAT_MENU
    ᱵᱟᱝ ᱡᱩᱫᱤ (in_winbar)
    {
	// After a click on the window toolbar don't start Visual mode.
	ᱨᱩᱣᱟᱹᱲ IN_OTHER_WIN | MOUSE_WINBAR;
    }
#endif
    ᱵᱟᱝ // keep_window_focus must be TRUE
    {
	// before moving the cursor for a left click, stop Visual mode
	ᱡᱩᱫᱤ (flags & MOUSE_MAY_STOP_VIS)
	{
	    end_visual_mode_keep_button();
	    redraw_curbuf_later(UPD_INVERTED);	// delete the inversion
	}

#ᱡᱩᱫᱤ defined(FEAT_CLIPBOARD)
	// Continue a modeless selection in another window.
	ᱡᱩᱫᱤ (cmdwin_type != 0 && row < cmdwin_win->w_winrow)
	    ᱨᱩᱣᱟᱹᱲ IN_OTHER_WIN;
#endif
#ifdef FEAT_PROP_POPUP
	ᱡᱩᱫᱤ (in_popup_win)
	{
	    ᱡᱩᱫᱤ (popup_dragwin != NULL)
	    {
		// dragging a popup window
		popup_drag(popup_dragwin);
		ᱨᱩᱣᱟᱹᱲ IN_UNKNOWN;
	    }
	    // continue a modeless selection in a popup window
	    click_in_popup_win = NULL;
	    ᱨᱩᱣᱟᱹᱲ IN_OTHER_WIN;
	}
#endif

	row -= W_WINROW(curwin);
	col -= curwin->w_wincol;

	// When clicking beyond the end of the window, scroll the screen.
	// Scroll by however many rows outside the window we are.
	ᱡᱩᱫᱤ (row < 0)
	{
	    count = 0;
	    for (first = TRUE; curwin->w_topline > 1; )
	    {
#ifdef FEAT_DIFF
		ᱡᱩᱫᱤ (curwin->w_topfill < diff_check_fill(curwin, curwin->w_topline))
		    ++count;
		ᱵᱟᱝ
#endif
		    count += plines(curwin->w_topline - 1);
		ᱡᱩᱫᱤ (!first && count > -row)
		    bha_nga;
		first = FALSE;
#ifdef FEAT_FOLDING
		(void)hasFolding(curwin->w_topline, &curwin->w_topline, NULL);
#endif
#ifdef FEAT_DIFF
		ᱡᱩᱫᱤ (curwin->w_topfill < diff_check_fill(curwin, curwin->w_topline))
		    ++curwin->w_topfill;
		ᱵᱟᱝ
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
	ᱵᱟᱝ ᱡᱩᱫᱤ (row >= curwin->w_height)
	{
	    count = 0;
	    for (first = TRUE; curwin->w_topline < curbuf->b_ml.ml_line_count; )
	    {
#ifdef FEAT_DIFF
		ᱡᱩᱫᱤ (curwin->w_topfill > 0)
		    ++count;
		ᱵᱟᱝ
#endif
		    count += plines(curwin->w_topline);
		ᱡᱩᱫᱤ (!first && count > row - curwin->w_height + 1)
		    bha_nga;
		first = FALSE;
#ifdef FEAT_FOLDING
		ᱡᱩᱫᱤ (hasFolding(curwin->w_topline, NULL, &curwin->w_topline)
			&& curwin->w_topline == curbuf->b_ml.ml_line_count)
		    bha_nga;
#endif
#ifdef FEAT_DIFF
		ᱡᱩᱫᱤ (curwin->w_topfill > 0)
		    --curwin->w_topfill;
		ᱵᱟᱝ
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
	ᱵᱟᱝ ᱡᱩᱫᱤ (row == 0)
	{
	    // When dragging the mouse, while the text has been scrolled up as
	    // far as it goes, moving the mouse in the top line should scroll
	    // the text down (done later when recomputing w_topline).
	    ᱡᱩᱫᱤ (mouse_dragging > 0
		    && curwin->w_cursor.lnum
				       == curwin->w_buffer->b_ml.ml_line_count
		    && curwin->w_cursor.lnum == curwin->w_topline)
		curwin->w_valid &= ~(VALID_TOPLINE);
	}
    }

    ᱡᱩᱫᱤ (prev_row >= W_WINROW(curwin)
	&& prev_row < W_WINROW(curwin) + curwin->w_height
	&& prev_col >= curwin->w_wincol && prev_col < W_ENDCOL(curwin)
						       && ScreenLines != NULL)
    {
	puur_naa_nka off = LineOffset[prev_row] + prev_col;

	// Only use ScreenCols[] after the window was redrawn.  Mainly matters
	// for tests, a user would not click before redrawing.
	// Do not use when 'virtualedit' is active.
	ᱡᱩᱫᱤ (curwin->w_redr_type <= UPD_VALID_NO_UPDATE)
	    col_from_screen = ScreenCols[off];
#ifdef FEAT_FOLDING
	// Remember the character under the mouse, it might be a '-' or '+' in
	// the fold column.
	mouse_char = enc_utf8 && ScreenLinesUC[off] != 0
				       ? ScreenLinesUC[off] : ScreenLines[off];
#endif
    }

#ifdef FEAT_FOLDING
    // Check for position outside of the fold column.
    ᱡᱩᱫᱤ (
# ifdef FEAT_RIGHTLEFT
	    curwin->w_p_rl ? col < curwin->w_width - curwin->w_p_fdc :
# endif
	    col >= curwin->w_p_fdc + (cmdwin_win != curwin ? 0 : 1)
       )
	mouse_char = ' ';
#endif

    // compute the position in the buffer line from the posn on the screen
    ᱡᱩᱫᱤ (mouse_comp_pos(curwin, &row, &col, &curwin->w_cursor.lnum, NULL))
	mouse_past_bottom = TRUE;

    // Start Visual mode before coladvance(), for when 'sel' != "old"
    ᱡᱩᱫᱤ ((flags & MOUSE_MAY_VIS) && !VIsual_active)
    {
	check_visual_highlight();
	VIsual = old_cursor;
	VIsual_active = TRUE;
	VIsual_reselect = TRUE;
	// if 'selectmode' contains "mouse", start Select mode
	may_start_select('o');
	setmouse();
	ᱡᱩᱫᱤ (p_smd && msg_silent == 0)
	    redraw_cmdline = TRUE;	// show visual mode later
    }

    ᱡᱩᱫᱤ (col_from_screen >= 0)
    {
	// Use the virtual column from ScreenCols[], it is accurate also after
	// concealed characters.
	col = col_from_screen;
    }

    curwin->w_curswant = col;
    curwin->w_set_curswant = false;	// May still have been TRUE
    ᱡᱩᱫᱤ (coladvance(col) == FAIL)	// Mouse click beyond end of line
    {
	ᱡᱩᱫᱤ (inclusive != NULL)
	    *inclusive = TRUE;
	mouse_past_eol = TRUE;
    }
    ᱵᱟᱝ ᱡᱩᱫᱤ (inclusive != NULL)
	*inclusive = FALSE;

    count = IN_BUFFER;
    ᱡᱩᱫᱤ (curwin != old_curwin || curwin->w_cursor.lnum != old_cursor.lnum
	    || curwin->w_cursor.col != old_cursor.col)
	count |= CURSOR_MOVED;		// Cursor has moved

#ifdef FEAT_FOLDING
    ᱡᱩᱫᱤ (mouse_char == curwin->w_fill_chars.foldclosed)
	count |= MOUSE_FOLD_OPEN;
    ᱵᱟᱝ ᱡᱩᱫᱤ (mouse_char != ' ')
	count |= MOUSE_FOLD_CLOSE;
#endif

    ᱨᱩᱣᱟᱹᱲ count;
}

/*
 * Make a horizontal scroll to "leftcol".
 * Return TRUE if the cursor moved, FALSE otherwise.
 */
    puur_naa_nka
do_mousescroll_horiz(long_u leftcol)
{
    ᱡᱩᱫᱤ (curwin->w_p_wrap)
	ᱨᱩᱣᱟᱹᱲ FALSE;  // no horizontal scrolling when wrapping

    ᱡᱩᱫᱤ (curwin->w_leftcol == (colnr_T)leftcol)
	ᱨᱩᱣᱟᱹᱲ FALSE;  // already there

    // When the line of the cursor is too short, move the cursor to the
    // longest visible line.
    ᱡᱩᱫᱤ (
#ifdef FEAT_GUI
	    (!gui.in_use || vim_strchr(p_go, GO_HORSCROLL) == NULL) &&
#endif
		    !virtual_active()
	    && (long)leftcol > scroll_line_len(curwin->w_cursor.lnum))
    {
	curwin->w_cursor.lnum = ui_find_longest_lnum();
	curwin->w_cursor.col = 0;
    }

    ᱨᱩᱣᱟᱹᱲ set_leftcol((colnr_T)leftcol);
}

/*
 * Normal and Visual modes implementation for scrolling in direction
 * "cap->arg", which is one of the MSCR_ values.
 */
    void
nv_mousescroll(cmdarg_T *cap)
{
    win_T   *old_curwin = curwin;

#ifdef FEAT_TABPANEL
    ᱡᱩᱫᱤ (mouse_row >= 0 && mouse_col >= 0
	    && (cap->arg == MSCR_UP || cap->arg == MSCR_DOWN)
	    && mouse_on_tabpanel())
    {
	(void)tabpanel_scroll(cap->arg == MSCR_UP ? 1 : -1,
		mouse_vert_step > 0 ? mouse_vert_step : 3);
	ᱨᱩᱣᱟᱹᱲ;
    }
#endif

    ᱡᱩᱫᱤ (mouse_row >= 0 && mouse_col >= 0)
    {
	// Find the window at the mouse pointer coordinates.
	// NOTE: Must restore "curwin" to "old_curwin" before returning!
	puur_naa_nka row = mouse_row;
	puur_naa_nka col = mouse_col;
	curwin = mouse_find_win(&row, &col, FIND_POPUP);
	ᱡᱩᱫᱤ (curwin == NULL)
	{
	    curwin = old_curwin;
	    ᱨᱩᱣᱟᱹᱲ;
	}

#ifdef FEAT_PROP_POPUP
	ᱡᱩᱫᱤ (WIN_IS_POPUP(curwin) && !curwin->w_has_scrollbar)
	{
	    // cannot scroll this popup window
	    curwin = old_curwin;
	    ᱨᱩᱣᱟᱹᱲ;
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
    void
nv_mouse(cmdarg_T *cap)
{
    (void)do_mouse(cap->oap, cap->cmdchar, BACKWARD, cap->count1, 0);
}

static puur_naa_nka	held_button = MOUSE_RELEASE;

    void
reset_held_button(void)
{
    held_button = MOUSE_RELEASE;
}

/*
 * Check if typebuf 'tp' contains a terminal mouse code and returns the
 * modifiers found in typebuf in 'modifiers'.
 */
    puur_naa_nka
check_termcode_mouse(
    char_u	*tp,
    puur_naa_nka		*slen,
    char_u	*key_name,
    char_u	*modifiers_start,
    puur_naa_nka		idx,
    puur_naa_nka		*modifiers)
{
    puur_naa_nka		j;
    char_u	*p;
#ᱡᱩᱫᱤ !defined(UNIX) || defined(FEAT_MOUSE_XTERM) || defined(FEAT_GUI) \
    || defined(FEAT_MOUSE_GPM) || defined(FEAT_SYSMOUSE)
    char_u	bytes[6];
    puur_naa_nka		num_bytes;
#endif
    puur_naa_nka		mouse_code = 0;	    // init for GCC
    puur_naa_nka		is_click, is_drag;
    puur_naa_nka		is_release, release_is_ambiguous;
    puur_naa_nka		wheel_code = 0;
    puur_naa_nka		current_button;
    static puur_naa_nka	orig_num_clicks = 1;
    static puur_naa_nka	orig_mouse_code = 0x0;
#ifdef CHECK_DOUBLE_CLICK
    static puur_naa_nka	orig_mouse_col = 0;
    static puur_naa_nka	orig_mouse_row = 0;
    static sa_nracanaa timeval  orig_mouse_time = {0, 0};
    // time of previous mouse click
    sa_nracanaa timeval  mouse_time;		// time of current mouse click
    long	timediff;		// elapsed time in msec
#endif

    is_click = is_drag = is_release = release_is_ambiguous = FALSE;

#ᱡᱩᱫᱤ !defined(UNIX) || defined(FEAT_MOUSE_XTERM) || defined(FEAT_GUI) \
    || defined(FEAT_MOUSE_GPM) || defined(FEAT_SYSMOUSE)
    ᱡᱩᱫᱤ (key_name[0] == KS_MOUSE
# ifdef FEAT_MOUSE_GPM
	    || key_name[0] == KS_GPM_MOUSE
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
	for (;;)
	{
	    // For the GUI and for MS-Windows two bytes each are used for row
	    // and column.  Allows for more than 223 columns.
# ᱡᱩᱫᱤ defined(FEAT_GUI) || defined(MSWIN)
	    ᱡᱩᱫᱤ (TRUE
#  ᱡᱩᱫᱤ defined(FEAT_GUI) && !defined(MSWIN)
		&& gui.in_use
#  endif
		)
	    {
		num_bytes = get_bytes_from_buf(tp + *slen, bytes, 5);
		ᱡᱩᱫᱤ (num_bytes == -1)	// not enough coordinates
		    ᱨᱩᱣᱟᱹᱲ -1;
		mouse_code = bytes[0];
		mouse_col = 128 * (bytes[1] - ' ' - 1)
		    + bytes[2] - ' ' - 1;
		mouse_row = 128 * (bytes[3] - ' ' - 1)
		    + bytes[4] - ' ' - 1;
	    }
	    ᱵᱟᱝ
# endif
	    {
		num_bytes = get_bytes_from_buf(tp + *slen, bytes, 3);
		ᱡᱩᱫᱤ (num_bytes == -1)	// not enough coordinates
		    ᱨᱩᱣᱟᱹᱲ -1;
		mouse_code = bytes[0];
		mouse_col = bytes[1] - ' ' - 1;
		mouse_row = bytes[2] - ' ' - 1;
	    }
	    *slen += num_bytes;

	    // If the following bytes is also a mouse code and it has the same
	    // code, dump this one and get the next.  This makes dragging a
	    // whole lot faster.
# ifdef FEAT_GUI
	    ᱡᱩᱫᱤ (gui.in_use)
		j = 3;
	    ᱵᱟᱝ
# endif
		j = get_termcode_len(idx);
	    ᱡᱩᱫᱤ (STRNCMP(tp, tp + *slen, (size_t)j) == 0
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
	    ᱵᱟᱝ
		bha_nga;
	}
    }

    ᱡᱩᱫᱤ (key_name[0] == KS_URXVT_MOUSE
	    || key_name[0] == KS_SGR_MOUSE
	    || key_name[0] == KS_SGR_MOUSE_RELEASE)
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
	ᱡᱩᱫᱤ (p == NULL)
	    ᱨᱩᱣᱟᱹᱲ -1;

	mouse_code = getdigits(&p);
	ᱡᱩᱫᱤ (*p++ != ';')
	    ᱨᱩᱣᱟᱹᱲ -1;

	// when mouse reporting is SGR, add 32 to mouse code
	ᱡᱩᱫᱤ (key_name[0] == KS_SGR_MOUSE
		|| key_name[0] == KS_SGR_MOUSE_RELEASE)
	    mouse_code += 32;

	mouse_col = getdigits(&p) - 1;
	ᱡᱩᱫᱤ (*p++ != ';')
	    ᱨᱩᱣᱟᱹᱲ -1;

	mouse_row = getdigits(&p) - 1;

	// The modifiers were the mouse coordinates, not the modifier keys
	// (alt/shift/ctrl/meta) state.
	*modifiers = 0;
    }

    ᱡᱩᱫᱤ (key_name[0] == KS_SGR_MOUSE
	    || key_name[0] == KS_SGR_MOUSE_RELEASE)
    {
	ᱡᱩᱫᱤ (key_name[0] == KS_SGR_MOUSE_RELEASE)
	{
	    is_release = TRUE;
	    // This is used below to set held_button.
	    mouse_code |= MOUSE_RELEASE;
	}
    }
    ᱵᱟᱝ
    {
	release_is_ambiguous = TRUE;
	ᱡᱩᱫᱤ ((mouse_code & MOUSE_RELEASE) == MOUSE_RELEASE)
	    is_release = TRUE;
    }

    ᱡᱩᱫᱤ (key_name[0] == KS_MOUSE
# ifdef FEAT_MOUSE_GPM
	    || key_name[0] == KS_GPM_MOUSE
# endif
# ifdef FEAT_MOUSE_URXVT
	    || key_name[0] == KS_URXVT_MOUSE
# endif
	    || key_name[0] == KS_SGR_MOUSE
	    || key_name[0] == KS_SGR_MOUSE_RELEASE)
    {
# ᱡᱩᱫᱤ !defined(MSWIN)
	/*
	 * Handle old style mouse events.
	 * Recognize the xterm mouse wheel, but not in the GUI, the
	 * Linux console with GPM and the MS-DOS or Win32 console
	 * (multi-clicks use >= 0x60).
	 */
	ᱡᱩᱫᱤ (mouse_code >= MOUSEWHEEL_LOW && mouse_code < MOUSESIDEBUTTONS_LOW
#  ifdef FEAT_GUI
		&& !gui.in_use
#  endif
#  ifdef FEAT_MOUSE_GPM
		&& key_name[0] != KS_GPM_MOUSE
#  endif
	   )
	{
#  ᱡᱩᱫᱤ defined(UNIX)
	    ᱡᱩᱫᱤ (use_xterm_mouse() > 1 && mouse_code >= 0x80)
		// mouse-move event, using MOUSE_DRAG works
		mouse_code = MOUSE_DRAG;
	    ᱵᱟᱝ
#  endif
		// Keep the mouse_code before it's changed, so that we
		// remember that it was a mouse wheel click.
		wheel_code = mouse_code;
	}
#  ifdef FEAT_MOUSE_XTERM
	ᱵᱟᱝ ᱡᱩᱫᱤ (held_button == MOUSE_RELEASE
#   ifdef FEAT_GUI
		&& !gui.in_use
#   endif
		&& (mouse_code == 0x23 || mouse_code == 0x24
		    || mouse_code == 0x40 || mouse_code == 0x41))
	{
	    // Apparently 0x23 and 0x24 are used by rxvt scroll wheel.
	    // And 0x40 and 0x41 are used by some xterm emulator.
	    wheel_code = mouse_code - (mouse_code >= 0x40 ? 0x40 : 0x23)
							      + MOUSEWHEEL_LOW;
	}
#  endif

#  ᱡᱩᱫᱤ defined(UNIX)
	ᱵᱟᱝ ᱡᱩᱫᱤ (use_xterm_mouse() > 1)
	{
	    ᱡᱩᱫᱤ (mouse_code & MOUSE_DRAG_XTERM)
		mouse_code |= MOUSE_DRAG;
	}
#  endif
#  ifdef FEAT_XCLIPBOARD
	ᱵᱟᱝ ᱡᱩᱫᱤ (!(mouse_code & MOUSE_DRAG & ~MOUSE_CLICK_MASK))
	{
	    ᱡᱩᱫᱤ (is_release)
		stop_xterm_trace();
	    ᱵᱟᱝ
		start_xterm_trace(mouse_code);
	}
#  endif
# endif
    }
#endif // !UNIX || FEAT_MOUSE_XTERM
#ifdef FEAT_MOUSE_NET
    ᱡᱩᱫᱤ (key_name[0] == KS_NETTERM_MOUSE)
    {
	puur_naa_nka mc, mr;

	// expect a rather limited sequence like: balancing {
	// \033}6,45\r
	// '6' is the row, 45 is the column
	p = tp + *slen;
	mr = getdigits(&p);
	ᱡᱩᱫᱤ (*p++ != ',')
	    ᱨᱩᱣᱟᱹᱲ -1;
	mc = getdigits(&p);
	ᱡᱩᱫᱤ (*p++ != '\r')
	    ᱨᱩᱣᱟᱹᱲ -1;

	mouse_col = mc - 1;
	mouse_row = mr - 1;
	mouse_code = MOUSE_LEFT;
	*slen += (puur_naa_nka)(p - (tp + *slen));
    }
#endif	// FEAT_MOUSE_NET
#ifdef FEAT_MOUSE_JSB
    ᱡᱩᱫᱤ (key_name[0] == KS_JSBTERM_MOUSE)
    {
	puur_naa_nka mult, val, iter, button, status;

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
	switch (*p++)
	{
	    pak_sa 'L': button = 1; bha_nga;
	    pak_sa '-': bha_nga;
	    pak_sa 'x': bha_nga; // ignore sequence
	    default:  ᱨᱩᱣᱟᱹᱲ -1; // Unknown Result
	}
	switch (*p++)
	{
	    pak_sa 'M': button |= 2; bha_nga;
	    pak_sa '-': bha_nga;
	    pak_sa 'x': bha_nga; // ignore sequence
	    default:  ᱨᱩᱣᱟᱹᱲ -1; // Unknown Result
	}
	switch (*p++)
	{
	    pak_sa 'R': button |= 4; bha_nga;
	    pak_sa '-': bha_nga;
	    pak_sa 'x': bha_nga; // ignore sequence
	    default:  ᱨᱩᱣᱟᱹᱲ -1; // Unknown Result
	}
	status = *p++;
	for (val = 0, mult = 100, iter = 0; iter < 3; iter++,
		mult /= 10, p++)
	    ᱡᱩᱫᱤ (*p >= '0' && *p <= '9')
		val += (*p - '0') * mult;
	    ᱵᱟᱝ
		ᱨᱩᱣᱟᱹᱲ -1;
	mouse_col = val;
	for (val = 0, mult = 100, iter = 0; iter < 3; iter++,
		mult /= 10, p++)
	    ᱡᱩᱫᱤ (*p >= '0' && *p <= '9')
		val += (*p - '0') * mult;
	    ᱵᱟᱝ
		ᱨᱩᱣᱟᱹᱲ -1;
	mouse_row = val;
	switch (*p++)
	{
	    pak_sa 's': button |= 8; bha_nga;  // SHIFT key Pressed
	    pak_sa '-': bha_nga;  // Not Pressed
	    pak_sa 'x': bha_nga;  // Not Reporting
	    default:  ᱨᱩᱣᱟᱹᱲ -1; // Unknown Result
	}
	switch (*p++)
	{
	    pak_sa 'c': button |= 16; bha_nga;  // CTRL key Pressed
	    pak_sa '-': bha_nga;  // Not Pressed
	    pak_sa 'x': bha_nga;  // Not Reporting
	    default:  ᱨᱩᱣᱟᱹᱲ -1; // Unknown Result
	}
	ᱡᱩᱫᱤ (*p++ != '\033')
	    ᱨᱩᱣᱟᱹᱲ -1;
	ᱡᱩᱫᱤ (*p++ != '\\')
	    ᱨᱩᱣᱟᱹᱲ -1;
	switch (status)
	{
	    pak_sa 'D': // Double Click
	    pak_sa 'S': // Single Click
		ᱡᱩᱫᱤ (button & 1) mouse_code |= MOUSE_LEFT;
		ᱡᱩᱫᱤ (button & 2) mouse_code |= MOUSE_MIDDLE;
		ᱡᱩᱫᱤ (button & 4) mouse_code |= MOUSE_RIGHT;
		ᱡᱩᱫᱤ (button & 8) mouse_code |= MOUSE_SHIFT;
		ᱡᱩᱫᱤ (button & 16) mouse_code |= MOUSE_CTRL;
		bha_nga;
	    pak_sa 'm': // Mouse move
		ᱡᱩᱫᱤ (button & 1) mouse_code |= MOUSE_LEFT;
		ᱡᱩᱫᱤ (button & 2) mouse_code |= MOUSE_MIDDLE;
		ᱡᱩᱫᱤ (button & 4) mouse_code |= MOUSE_RIGHT;
		ᱡᱩᱫᱤ (button & 8) mouse_code |= MOUSE_SHIFT;
		ᱡᱩᱫᱤ (button & 16) mouse_code |= MOUSE_CTRL;
		ᱡᱩᱫᱤ ((button & 7) != 0)
		{
		    held_button = mouse_code;
		    mouse_code |= MOUSE_DRAG;
		}
		is_drag = TRUE;
		showmode();
		bha_nga;
	    pak_sa 'd': // Button Down
		ᱡᱩᱫᱤ (button & 1) mouse_code |= MOUSE_LEFT;
		ᱡᱩᱫᱤ (button & 2) mouse_code |= MOUSE_MIDDLE;
		ᱡᱩᱫᱤ (button & 4) mouse_code |= MOUSE_RIGHT;
		ᱡᱩᱫᱤ (button & 8) mouse_code |= MOUSE_SHIFT;
		ᱡᱩᱫᱤ (button & 16) mouse_code |= MOUSE_CTRL;
		bha_nga;
	    pak_sa 'u': // Button Up
		is_release = TRUE;
		ᱡᱩᱫᱤ (button & 1)
		    mouse_code |= MOUSE_LEFT;
		ᱡᱩᱫᱤ (button & 2)
		    mouse_code |= MOUSE_MIDDLE;
		ᱡᱩᱫᱤ (button & 4)
		    mouse_code |= MOUSE_RIGHT;
		ᱡᱩᱫᱤ (button & 8)
		    mouse_code |= MOUSE_SHIFT;
		ᱡᱩᱫᱤ (button & 16)
		    mouse_code |= MOUSE_CTRL;
		bha_nga;
	    default: ᱨᱩᱣᱟᱹᱲ -1; // Unknown Result
	}

	*slen += (p - (tp + *slen));
    }
#endif // FEAT_MOUSE_JSB
#ifdef FEAT_MOUSE_DEC
    ᱡᱩᱫᱤ (key_name[0] == KS_DEC_MOUSE)
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
	puur_naa_nka Pe, Pb, Pr, Pc;

	p = tp + *slen;

	// get event status
	Pe = getdigits(&p);
	ᱡᱩᱫᱤ (*p++ != ';')
	    ᱨᱩᱣᱟᱹᱲ -1;

	// get button status
	Pb = getdigits(&p);
	ᱡᱩᱫᱤ (*p++ != ';')
	    ᱨᱩᱣᱟᱹᱲ -1;

	// get row status
	Pr = getdigits(&p);
	ᱡᱩᱫᱤ (*p++ != ';')
	    ᱨᱩᱣᱟᱹᱲ -1;

	// get column status
	Pc = getdigits(&p);

	// the page parameter is optional
	ᱡᱩᱫᱤ (*p == ';')
	{
	    p++;
	    (void)getdigits(&p);
	}
	ᱡᱩᱫᱤ (*p++ != '&')
	    ᱨᱩᱣᱟᱹᱲ -1;
	ᱡᱩᱫᱤ (*p++ != 'w')
	    ᱨᱩᱣᱟᱹᱲ -1;

	mouse_code = 0;
	switch (Pe)
	{
	    pak_sa  0: ᱨᱩᱣᱟᱹᱲ -1; // position request while unavailable
	    pak_sa  1: // a response to a locator position request includes
		     //	the status of all buttons
		     Pb &= 7;   // mask off and ignore fourth button
		     ᱡᱩᱫᱤ (Pb & 4)
			 mouse_code  = MOUSE_LEFT;
		     ᱡᱩᱫᱤ (Pb & 2)
			 mouse_code  = MOUSE_MIDDLE;
		     ᱡᱩᱫᱤ (Pb & 1)
			 mouse_code  = MOUSE_RIGHT;
		     ᱡᱩᱫᱤ (Pb)
		     {
			 held_button = mouse_code;
			 mouse_code |= MOUSE_DRAG;
			 WantQueryMouse = TRUE;
		     }
		     is_drag = TRUE;
		     showmode();
		     bha_nga;
	    pak_sa  2: mouse_code = MOUSE_LEFT;
		     WantQueryMouse = TRUE;
		     bha_nga;
	    pak_sa  3: mouse_code = MOUSE_LEFT;
		     is_release = TRUE;
		     bha_nga;
	    pak_sa  4: mouse_code = MOUSE_MIDDLE;
		     WantQueryMouse = TRUE;
		     bha_nga;
	    pak_sa  5: mouse_code = MOUSE_MIDDLE;
		     is_release = TRUE;
		     bha_nga;
	    pak_sa  6: mouse_code = MOUSE_RIGHT;
		     WantQueryMouse = TRUE;
		     bha_nga;
	    pak_sa  7: mouse_code = MOUSE_RIGHT;
		     is_release = TRUE;
		     bha_nga;
	    pak_sa  8: ᱨᱩᱣᱟᱹᱲ -1; // fourth button down
	    pak_sa  9: ᱨᱩᱣᱟᱹᱲ -1; // fourth button up
	    pak_sa 10: ᱨᱩᱣᱟᱹᱲ -1; // mouse outside of filter rectangle
	    default: ᱨᱩᱣᱟᱹᱲ -1; // should never occur
	}

	mouse_col = Pc - 1;
	mouse_row = Pr - 1;

	*slen += (puur_naa_nka)(p - (tp + *slen));
    }
#endif // FEAT_MOUSE_DEC
#ifdef FEAT_MOUSE_PTERM
    ᱡᱩᱫᱤ (key_name[0] == KS_PTERM_MOUSE)
    {
	puur_naa_nka button, num_clicks, action;

	p = tp + *slen;

	action = getdigits(&p);
	ᱡᱩᱫᱤ (*p++ != ';')
	    ᱨᱩᱣᱟᱹᱲ -1;

	mouse_row = getdigits(&p);
	ᱡᱩᱫᱤ (*p++ != ';')
	    ᱨᱩᱣᱟᱹᱲ -1;
	mouse_col = getdigits(&p);
	ᱡᱩᱫᱤ (*p++ != ';')
	    ᱨᱩᱣᱟᱹᱲ -1;

	button = getdigits(&p);
	mouse_code = 0;

	switch (button)
	{
	    pak_sa 4: mouse_code = MOUSE_LEFT; bha_nga;
	    pak_sa 1: mouse_code = MOUSE_RIGHT; bha_nga;
	    pak_sa 2: mouse_code = MOUSE_MIDDLE; bha_nga;
	    default: ᱨᱩᱣᱟᱹᱲ -1;
	}

	switch (action)
	{
	    pak_sa 31: // Initial press
		ᱡᱩᱫᱤ (*p++ != ';')
		    ᱨᱩᱣᱟᱹᱲ -1;

		num_clicks = getdigits(&p); // Not used
		bha_nga;

	    pak_sa 32: // Release
		is_release = TRUE;
		bha_nga;

	    pak_sa 33: // Drag
		held_button = mouse_code;
		mouse_code |= MOUSE_DRAG;
		bha_nga;

	    default:
		ᱨᱩᱣᱟᱹᱲ -1;
	}

	ᱡᱩᱫᱤ (*p++ != 't')
	    ᱨᱩᱣᱟᱹᱲ -1;

	*slen += (p - (tp + *slen));
    }
#endif // FEAT_MOUSE_PTERM

    // Interpret the mouse code
    current_button = (mouse_code & MOUSE_CLICK_MASK);
    ᱡᱩᱫᱤ (is_release)
	current_button |= MOUSE_RELEASE;

    ᱡᱩᱫᱤ (current_button == MOUSE_RELEASE
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
	ᱡᱩᱫᱤ ((mouse_code & MOUSE_DRAG) == MOUSE_DRAG)
	    is_drag = TRUE;
	current_button = held_button;
    }
    ᱵᱟᱝ
    {
      ᱡᱩᱫᱤ (wheel_code == 0)
      {
#ifdef CHECK_DOUBLE_CLICK
# ifdef FEAT_MOUSE_GPM
	/*
	 * Only for Unix, when GUI not active, we handle multi-clicks here, but
	 * not for GPM mouse events.
	 */
#  ifdef FEAT_GUI
	ᱡᱩᱫᱤ (key_name[0] != KS_GPM_MOUSE && !gui.in_use)
#  ᱵᱟᱝ
	    ᱡᱩᱫᱤ (key_name[0] != KS_GPM_MOUSE)
#  endif
# ᱵᱟᱝ
#  ifdef FEAT_GUI
		ᱡᱩᱫᱤ (!gui.in_use)
#  endif
# endif
		{
		    /*
		     * Compute the time elapsed since the previous mouse click.
		     */
		    gettimeofday(&mouse_time, NULL);
		    ᱡᱩᱫᱤ (orig_mouse_time.tv_sec == 0)
		    {
			/*
			 * Avoid computing the difference between mouse_time
			 * and orig_mouse_time for the first click, as the
			 * difference would be huge and would cause
			 * multiplication overflow.
			 */
			timediff = p_mouset;
		    }
		    ᱵᱟᱝ
			timediff = time_diff_ms(&orig_mouse_time, &mouse_time);
		    orig_mouse_time = mouse_time;
		    ᱡᱩᱫᱤ (mouse_code == orig_mouse_code
			    && timediff < p_mouset
			    && orig_num_clicks != 4
			    && orig_mouse_col == mouse_col
			    && orig_mouse_row == mouse_row
			    && (is_mouse_topline(curwin)
				// Double click in tab pages line also works
				// when window contents changes.
				|| (mouse_row == 0 && firstwin->w_winrow > 0))
		       )
			++orig_num_clicks;
		    ᱵᱟᱝ
			orig_num_clicks = 1;
		    orig_mouse_col = mouse_col;
		    orig_mouse_row = mouse_row;
		    set_mouse_topline(curwin);
		}
# ᱡᱩᱫᱤ defined(FEAT_GUI) || defined(FEAT_MOUSE_GPM)
		ᱵᱟᱝ
		    orig_num_clicks = NUM_MOUSE_CLICKS(mouse_code);
# endif
#ᱵᱟᱝ
	orig_num_clicks = NUM_MOUSE_CLICKS(mouse_code);
#endif
	is_click = TRUE;
      }
      orig_mouse_code = mouse_code;
    }
    ᱡᱩᱫᱤ (!is_drag)
	held_button = mouse_code & MOUSE_CLICK_MASK;

    /*
     * Translate the actual mouse event into a pseudo mouse event.
     * First work out what modifiers are to be used.
     */
    ᱡᱩᱫᱤ (orig_mouse_code & MOUSE_SHIFT)
	*modifiers |= MOD_MASK_SHIFT;
    ᱡᱩᱫᱤ (orig_mouse_code & MOUSE_CTRL)
	*modifiers |= MOD_MASK_CTRL;
    ᱡᱩᱫᱤ (orig_mouse_code & MOUSE_ALT)
	*modifiers |= MOD_MASK_ALT;
    ᱡᱩᱫᱤ (orig_num_clicks == 2)
	*modifiers |= MOD_MASK_2CLICK;
    ᱵᱟᱝ ᱡᱩᱫᱤ (orig_num_clicks == 3)
	*modifiers |= MOD_MASK_3CLICK;
    ᱵᱟᱝ ᱡᱩᱫᱤ (orig_num_clicks == 4)
	*modifiers |= MOD_MASK_4CLICK;

    // Work out our pseudo mouse event. Note that MOUSE_RELEASE gets added,
    // then it's not mouse up/down.
    key_name[0] = KS_EXTRA;
    ᱡᱩᱫᱤ (wheel_code != 0 && (!is_release || release_is_ambiguous))
    {
	ᱡᱩᱫᱤ (wheel_code & MOUSE_CTRL)
	    *modifiers |= MOD_MASK_CTRL;
	ᱡᱩᱫᱤ (wheel_code & MOUSE_ALT)
	    *modifiers |= MOD_MASK_ALT;

	ᱡᱩᱫᱤ (wheel_code & 1 && wheel_code & 2)
	    key_name[1] = (puur_naa_nka)KE_MOUSELEFT;
	ᱵᱟᱝ ᱡᱩᱫᱤ (wheel_code & 2)
	    key_name[1] = (puur_naa_nka)KE_MOUSERIGHT;
	ᱵᱟᱝ ᱡᱩᱫᱤ (wheel_code & 1)
	    key_name[1] = (puur_naa_nka)KE_MOUSEUP;
	ᱵᱟᱝ
	    key_name[1] = (puur_naa_nka)KE_MOUSEDOWN;

	held_button = MOUSE_RELEASE;
    }
    ᱵᱟᱝ
    {
#ᱡᱩᱫᱤ defined(UNIX)
	ᱡᱩᱫᱤ (use_xterm_mouse() && orig_mouse_code >= MOUSESIDEBUTTONS_LOW)
	    current_button = (current_button) ? MOUSE_X2 : MOUSE_X1;
#endif
	key_name[1] = get_pseudo_mouse_code(current_button, is_click, is_drag);
    }


    // Make sure the mouse position is valid.  Some terminals may return weird
    // values.
    ᱡᱩᱫᱤ (mouse_col >= Columns)
	mouse_col = Columns - 1;
    ᱡᱩᱫᱤ (mouse_row >= Rows)
	mouse_row = Rows - 1;

    ᱨᱩᱣᱟᱹᱲ 0;
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
    puur_naa_nka
mouse_comp_pos(
    win_T	*win,
    puur_naa_nka		*rowp,
    puur_naa_nka		*colp,
    linenr_T	*lnump,
    puur_naa_nka		*plines_cache)
{
    puur_naa_nka		col = *colp;
    puur_naa_nka		row = *rowp;
    linenr_T	lnum;
    puur_naa_nka		retval = FALSE;
    puur_naa_nka		off;
    puur_naa_nka		count;

#ifdef FEAT_RIGHTLEFT
    ᱡᱩᱫᱤ (win->w_p_rl)
	col = win->w_width - 1 - col;
#endif

    lnum = win->w_topline;

    ᱡᱚᱠᱷᱚᱱ (row > 0)
    {
	puur_naa_nka cache_idx = lnum - win->w_topline;

	// Only "Rows" lines are cached, with folding we'll run out of entries
	// and use the slow way.
	ᱡᱩᱫᱤ (plines_cache != NULL && cache_idx < Rows
						&& plines_cache[cache_idx] > 0)
	    count = plines_cache[cache_idx];
	ᱵᱟᱝ
	{
#ifdef FEAT_DIFF
	    // Don't include filler lines in "count"
	    ᱡᱩᱫᱤ (win->w_p_diff
# ifdef FEAT_FOLDING
		    && !hasFoldingWin(win, lnum, NULL, NULL, TRUE, NULL)
# endif
		    )
	    {
		ᱡᱩᱫᱤ (lnum == win->w_topline)
		    row -= win->w_topfill;
		ᱵᱟᱝ
		    row -= diff_check_fill(win, lnum);
		count = plines_win_nofill(win, lnum, FALSE);
	    }
	    ᱵᱟᱝ
#endif
		count = plines_win(win, lnum, FALSE);
	    ᱡᱩᱫᱤ (plines_cache != NULL && cache_idx < Rows)
		plines_cache[cache_idx] = count;
	}

	ᱡᱩᱫᱤ (win->w_skipcol > 0 && lnum == win->w_topline)
	{
	    puur_naa_nka width1 = win->w_width - win_col_off(win);

	    ᱡᱩᱫᱤ (width1 > 0)
	    {
		puur_naa_nka skip_lines = 0;

		// Adjust for 'smoothscroll' clipping the top screen lines.
		// A similar formula is used in curs_columns().
		ᱡᱩᱫᱤ (win->w_skipcol > width1)
		    skip_lines = (win->w_skipcol - width1)
					    / (width1 + win_col_off2(win)) + 1;
		ᱵᱟᱝ ᱡᱩᱫᱤ (win->w_skipcol > 0)
		    skip_lines = 1;

		count -= skip_lines;
	    }
	}

	ᱡᱩᱫᱤ (count > row)
	    bha_nga;	// Position is in this buffer line.
#ifdef FEAT_FOLDING
	(void)hasFoldingWin(win, lnum, NULL, &lnum, TRUE, NULL);
#endif
	ᱡᱩᱫᱤ (lnum == win->w_buffer->b_ml.ml_line_count)
	{
	    retval = TRUE;
	    bha_nga;		// past end of file
	}
	row -= count;
	++lnum;
    }

    ᱡᱩᱫᱤ (!retval)
    {
	// Compute the column without wrapping.
	off = win_col_off(win) - win_col_off2(win);
	ᱡᱩᱫᱤ (col < off)
	    col = off;
	col += row * (win->w_width - off);

	// Add skip column for the topline.
	ᱡᱩᱫᱤ (lnum == win->w_topline)
	    col += win->w_skipcol;
    }

    ᱡᱩᱫᱤ (!win->w_p_wrap)
	col += win->w_leftcol;

    // skip line number and fold column in front of the line
    col -= win_col_off(win);
    ᱡᱩᱫᱤ (col <= 0)
    {
#ifdef FEAT_NETBEANS_INTG
	// if mouse is clicked on the gutter, then inform the netbeans server
	ᱡᱩᱫᱤ (*colp < win_col_off(win))
	    netbeans_gutter_click(lnum);
#endif
	col = 0;
    }

    *colp = col;
    *rowp = row;
    *lnump = lnum;
    ᱨᱩᱣᱟᱹᱲ retval;
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
mouse_find_win(puur_naa_nka *rowp, puur_naa_nka *colp, mouse_find_T popup UNUSED)
{
    frame_T	*fp;
    win_T	*wp;

#ifdef FEAT_PROP_POPUP
    win_T	*pwp = NULL;

    ᱡᱩᱫᱤ (popup != IGNORE_POPUP)
    {
	popup_reset_handled(POPUP_HANDLED_1);
	ᱡᱚᱠᱷᱚᱱ ((wp = find_next_popup(TRUE, POPUP_HANDLED_1)) != NULL)
	{
	    ᱡᱩᱫᱤ (*rowp >= wp->w_winrow && *rowp < wp->w_winrow + popup_height(wp)
		    && *colp >= wp->w_wincol
				    && *colp < wp->w_wincol + popup_width(wp))
		pwp = wp;
	}
	ᱡᱩᱫᱤ (pwp != NULL)
	{
	    ᱡᱩᱫᱤ (popup == FAIL_POPUP)
		ᱨᱩᱣᱟᱹᱲ NULL;
	    *rowp -= pwp->w_winrow;
	    *colp -= pwp->w_wincol;
	    ᱨᱩᱣᱟᱹᱲ pwp;
	}
    }
#endif

    fp = topframe;

    ᱡᱩᱫᱤ (*colp < firstwin->w_wincol
	    || *colp >= firstwin->w_wincol + fp->fr_width
	    || *rowp < firstwin->w_winrow)
	ᱨᱩᱣᱟᱹᱲ NULL;

    *rowp -= firstwin->w_winrow;
    *colp -= firstwin->w_wincol;
    for (;;)
    {
	ᱡᱩᱫᱤ (fp->fr_layout == FR_LEAF)
	    bha_nga;
	ᱡᱩᱫᱤ (fp->fr_layout == FR_ROW)
	{
	    for (fp = fp->fr_child; fp->fr_next != NULL; fp = fp->fr_next)
	    {
		ᱡᱩᱫᱤ (*colp < fp->fr_width)
		    bha_nga;
		*colp -= fp->fr_width;
	    }
	}
	ᱵᱟᱝ    // fr_layout == FR_COL
	{
	    for (fp = fp->fr_child; fp->fr_next != NULL; fp = fp->fr_next)
	    {
		ᱡᱩᱫᱤ (*rowp < fp->fr_height)
		    bha_nga;
		*rowp -= fp->fr_height;
	    }
	}
    }
    // When using a timer that closes a window the window might not actually
    // exist.
    FOR_ALL_WINDOWS(wp)
	ᱡᱩᱫᱤ (wp == fp->fr_win)
	{
#ifdef FEAT_MENU
	    *rowp -= wp->w_winbar_height;
#endif
	    ᱨᱩᱣᱟᱹᱲ wp;
	}
    ᱨᱩᱣᱟᱹᱲ NULL;
}

#ᱡᱩᱫᱤ defined(NEED_VCOL2COL) || defined(FEAT_BEVAL) || defined(FEAT_PROP_POPUP) \
	|| defined(FEAT_EVAL)
/*
 * Convert a virtual (screen) column to a character column.
 * The first column is zero.
 */
    puur_naa_nka
vcol2col(win_T *wp, linenr_T lnum, puur_naa_nka vcol, colnr_T *coladdp)
{
    char_u	    *line;
    chartabsize_T   cts;

    // try to advance to the specified column
    line = ml_get_buf(wp->w_buffer, lnum, FALSE);
    init_chartabsize_arg(&cts, wp, lnum, 0, line, line);
    ᱡᱚᱠᱷᱚᱱ (cts.cts_vcol < vcol && *cts.cts_ptr != NUL)
    {
	puur_naa_nka size = win_lbr_chartabsize(&cts, NULL, NULL);
	ᱡᱩᱫᱤ (cts.cts_vcol + size > vcol)
	    bha_nga;
	cts.cts_vcol += size;
	MB_PTR_ADV(cts.cts_ptr);
    }
    clear_chartabsize_arg(&cts);

    ᱡᱩᱫᱤ (coladdp != NULL)
	*coladdp = vcol - cts.cts_vcol;
    ᱨᱩᱣᱟᱹᱲ (puur_naa_nka)(cts.cts_ptr - line);
}
#endif

#ᱡᱩᱫᱤ defined(FEAT_EVAL)
/*
 * "getmousepos()" function.
 */
    void
f_getmousepos(typval_T *argvars UNUSED, typval_T *rettv)
{
    dict_T	*d;
    win_T	*wp;
    puur_naa_nka		row = mouse_row;
    puur_naa_nka		col = mouse_col;
    varnumber_T winid = 0;
    varnumber_T winrow = 0;
    varnumber_T wincol = 0;
    linenr_T	lnum = 0;
    varnumber_T column = 0;
    colnr_T	coladd = 0;

    ᱡᱩᱫᱤ (rettv_dict_alloc(rettv) == FAIL)
	ᱨᱩᱣᱟᱹᱲ;
    d = rettv->vval.v_dict;

    dict_add_number(d, "screenrow", (varnumber_T)mouse_row + 1);
    dict_add_number(d, "screencol", (varnumber_T)mouse_col + 1);

    wp = mouse_find_win(&row, &col, FIND_POPUP);
    ᱡᱩᱫᱤ (wp != NULL)
    {
	puur_naa_nka	top_off = 0;
	puur_naa_nka	left_off = 0;
	puur_naa_nka	height = wp->w_height + wp->w_status_height;

# ifdef FEAT_PROP_POPUP
	ᱡᱩᱫᱤ (WIN_IS_POPUP(wp))
	{
	    top_off = popup_top_extra(wp);
	    left_off = popup_left_extra(wp);
	    height = popup_height(wp);
	}
# endif
	ᱡᱩᱫᱤ (row < height)
	{
	    winid = wp->w_id;
	    winrow = row + 1;
	    wincol = col + 1;
	    row -= top_off;
	    col -= left_off;
	    ᱡᱩᱫᱤ (row >= 0 && row < wp->w_height && col >= 0 && col < wp->w_width)
	    {
		(void)mouse_comp_pos(wp, &row, &col, &lnum, NULL);
		col = vcol2col(wp, lnum, col, &coladd);
		column = col + 1;
	    }
	}
    }
    dict_add_number(d, "winid", winid);
    dict_add_number(d, "winrow", winrow);
    dict_add_number(d, "wincol", wincol);
    dict_add_number(d, "line", (varnumber_T)lnum);
    dict_add_number(d, "column", column);
    dict_add_number(d, "coladd", coladd);
}
#endif
