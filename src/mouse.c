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
ساکن طویل mouse_hor_قدم = 6;
ساکن طویل mouse_vert_قدم = 3;
ساکن win_T *dragwin = تہی;	// window being dragged
ساکن صحیح stl_click_handler(win_T *wp, صحیح mrow, صحیح mcol, صحیح which_button,
								    صحیح mods);
ساکن صحیح stl_click_handler_regions(stl_click_region_T *regions,
				    صحیح region_گنتی, صحیح winid,
				    char_u *area_name, صحیح mrow, صحیح mcol,
				    صحیح which_button, صحیح mods);

    خالی
mouse_سیٹ_vert_scroll_قدم(طویل قدم)
{
    mouse_vert_قدم = قدم;
}

    خالی
mouse_سیٹ_hor_scroll_قدم(طویل قدم)
{
    mouse_hor_قدم = قدم;
}

#اگر_تعریف_شدہ CHECK_DOUBLE_CLICK
/*
 * Return the duration from t1 to t2 in milliseconds.
 */
    ساکن طویل
وقت_diff_ms(ساخت timeval *t1, ساخت timeval *t2)
{
    // This handles wrapping of tv_usec correctly without any special case.
    // Example of 2 pairs (tv_sec, tv_usec) with a duration of 5 ms:
    //	   t1 = (1, 998000) t2 = (2, 3000) gives:
    //	   (2 - 1) * 1000 + (3000 - 998000) / 1000 -> 5 ms.
    واپس (t2->tv_sec - t1->tv_sec) * 1000
	 + (t2->tv_usec - t1->tv_usec) / 1000;
}
#اگر_ختم

/*
 * Get class of a character for selection: same class means same word.
 * 0: blank
 * 1: punctuation groups
 * 2: normal word character
 * >2: multi-byte word character.
 */
    ساکن صحیح
get_mouse_class(char_u *p)
{
    صحیح		c;

    اگر (has_mbyte && MB_BYTE2LEN(p[0]) > 1)
	واپس mb_get_class(p);

    c = *p;
    اگر (c == ' ' || c == '\t')
	واپس 0;

    اگر (vim_iswordc(c))
	واپس 2;

    // There are a few special cases where we want certain combinations of
    // characters to be considered as a single word.  These are things like
    // "->", "/ *", "*=", "+=", "&=", "<=", ">=", "!=" etc.  Otherwise, each
    // character is in its own class.
    اگر (c != NUL && vim_strchr((char_u *)"-+*/%<>&|^!=", c) != تہی)
	واپس 1;
    واپس c;
}

/*
 * Move "pos" back to the start of the word it's in.
 */
    ساکن خالی
ڈھونڈو_آغاز_کا_لفظ(pos_T *pos)
{
    char_u	*سطر;
    صحیح		cclass;
    صحیح		ستون;

    سطر = ml_get(pos->lnum);
    cclass = get_mouse_class(سطر + pos->ستون);

    جب (pos->ستون > 0)
    {
	ستون = pos->ستون - 1;
	ستون -= (*mb_سر_off)(سطر, سطر + ستون);
	اگر (get_mouse_class(سطر + ستون) != cclass)
	    توڑو;
	pos->ستون = ستون;
    }
}

/*
 * Move "pos" forward to the end of the word it's in.
 * When 'selection' is "exclusive", the position is just after the word.
 */
    ساکن خالی
ڈھونڈو_انتہا_کا_لفظ(pos_T *pos)
{
    char_u	*سطر;
    صحیح		cclass;
    صحیح		ستون;

    سطر = ml_get(pos->lnum);
    اگر (*p_sel == 'e' && pos->ستون > 0)
    {
	--pos->ستون;
	pos->ستون -= (*mb_سر_off)(سطر, سطر + pos->ستون);
    }
    cclass = get_mouse_class(سطر + pos->ستون);
    جب (سطر[pos->ستون] != NUL)
    {
	ستون = pos->ستون + (*mb_ptr2len)(سطر + pos->ستون);
	اگر (get_mouse_class(سطر + ستون) != cclass)
	{
	    اگر (*p_sel == 'e')
		pos->ستون = ستون;
	    توڑو;
	}
	pos->ستون = ستون;
    }
}

#اگر defined(FEAT_GUI_MOTIF) || defined(FEAT_GUI_GTK) \
	    || defined(FEAT_GUI_MSWIN) \
	    || defined(FEAT_GUI_PHOTON) \
	    || defined(FEAT_رکن_POPUP_MENU)
# define USE_POPUP_SETPOS
# define NEED_VCOL2_ستون

/*
 * Translate window coordinates to buffer position without any side effects.
 * Returns IN_BUFFER and sets "mpos->col" to the column when in buffer text.
 * The column is one for the first column.
 */
    ساکن صحیح
get_fpos_کا_mouse(pos_T *mpos)
{
    win_T	*wp;
    صحیح		صف = mouse_صف;
    صحیح		ستون = mouse_ستون;

    اگر (صف < 0 || ستون < 0)		// check if it makes sense
	واپس اندر_UNKNOWN;

    // find the window where the row is in
    wp = mouse_ڈھونڈو_win(&صف, &ستون, FAIL_POPUP);
    اگر (wp == تہی)
	واپس اندر_UNKNOWN;
    // winpos and height may change in win_enter()!
    اگر (صف >= wp->w_اونچائی)	// In (or below) status line
	واپس اندر_حالت_سطر;
    اگر (ستون >= wp->w_چوڑائی)	// In vertical separator line
	واپس اندر_SEP_سطر;

    اگر (wp != curwin)
	واپس اندر_UNKNOWN;

    // compute the position in the buffer line from the posn on the screen
    اگر (mouse_comp_pos(curwin, &صف, &ستون, &mpos->lnum, تہی))
	واپس اندر_حالت_سطر; // past bottom

    mpos->ستون = vcol2col(wp, mpos->lnum, ستون, &mpos->coladd);
    واپس اندر_بفر;
}
#اگر_ختم

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
    طویل	گنتی,
    صحیح		fixindent)	// PUT_FIXINDENT if fixing indent necessary
{
    ساکن صحیح	do_always = FALSE;	// ignore 'mouse' setting next time
    ساکن صحیح	got_click = FALSE;	// got a click some time back

    صحیح		which_button;	// MOUSE_LEFT, _MIDDLE or _RIGHT
    صحیح		is_click = FALSE; // If FALSE it's a drag or release event
    صحیح		is_drag = FALSE;  // If TRUE it's a drag event
    صحیح		jump_flags = 0;	// flags for jump_to_mouse()
    pos_T	آغاز_visual;
    صحیح		moved;		// Has cursor moved?
    صحیح		اندر_حالت_سطر_2;	// mouse in status line
    ساکن صحیح	اندر_tab_سطر = FALSE; // mouse clicked in tab line
    ساکن صحیح	اندر_tabpanel = FALSE; // mouse clicked in tabpanel
#اگر_تعریف_شدہ FEAT_TABPANEL
    ساکن bool	اندر_tabpanel_scrollbar = false; // dragging tabpanel scrollbar
#اگر_ختم
    صحیح		اندر_sep_سطر;	// mouse in vertical separator line
    صحیح		c1, c2;
#اگر defined(FEAT_FOLDING)
    pos_T	save_cursor;
#اگر_ختم
    win_T	*old_curwin = curwin;
    ساکن pos_T orig_cursor;
    colnr_T	leftcol, rightcol;
    pos_T	انتہا_visual;
    صحیح		diff;
    صحیح		old_فعال = VIsual_فعال;
    صحیح		old_منوال = VIsual_منوال;
    صحیح		regname;

#اگر defined(FEAT_FOLDING)
    save_cursor = curwin->w_cursor;
#اگر_ختم

    // When GUI is active, always recognize mouse events, otherwise:
    // - Ignore mouse event in normal mode if 'mouse' doesn't include 'n'.
    // - Ignore mouse event in visual mode if 'mouse' doesn't include 'v'.
    // - For command line and insert mode 'mouse' is checked before calling
    //	 do_mouse().
    اگر (do_always)
	do_always = FALSE;
    ورنہ
#اگر_تعریف_شدہ FEAT_GUI
	اگر (!gui.اندر_use)
#اگر_ختم
	{
	    اگر (VIsual_فعال)
	    {
		اگر (!mouse_has(MOUSE_VISUAL))
		    واپس FALSE;
	    }
	    ورنہ اگر (State == منوال_معمول && !mouse_has(MOUSE_معمول))
		واپس FALSE;
	}

    برائے (;;)
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
		صحیح save_mouse_صف = mouse_صف;
		صحیح save_mouse_ستون = mouse_ستون;

		// Need to get the character, peeking doesn't get the actual
		// one.
		nc = محفوظ_vgetc();
		اگر (c == nc)
		    جاری;
		vungetc(nc);
		mouse_صف = save_mouse_صف;
		mouse_ستون = save_mouse_ستون;
	    }
	}
	توڑو;
    }

    اگر (c == K_MOUSEMOVE)
    {
	// Mouse moved without a button pressed.
#اگر_تعریف_شدہ FEAT_BEVAL_رکن
	ui_may_remove_balloon();
	اگر (p_bevalterm)
	{
	    profile_setlimit(p_bdlay, &bevalexpr_due);
	    bevalexpr_due_سیٹ = TRUE;
	}
#اگر_ختم
#اگر_تعریف_شدہ FEAT_PROP_POPUP
	popup_handle_mouse_moved();
#اگر_ختم
	واپس FALSE;
    }

#اگر_تعریف_شدہ FEAT_MOUSESHAPE
    // May have stopped dragging the status or separator line.  The pointer is
    // most likely still on the status or separator line.
    اگر (!is_drag && drag_حالت_سطر)
    {
	drag_حالت_سطر = FALSE;
	update_mouseshape(SHAPE_IDX_حالت);
    }
    اگر (!is_drag && drag_sep_سطر)
    {
	drag_sep_سطر = FALSE;
	update_mouseshape(SHAPE_IDX_VSEP);
    }
#اگر_ختم

    // Ignore drag and release events if we didn't get a click.
    اگر (is_click)
    {
	got_click = TRUE;
	اندر_tab_سطر = FALSE;
	اندر_tabpanel = FALSE;
#اگر_تعریف_شدہ FEAT_TABPANEL
	اندر_tabpanel_scrollbar = false;
#اگر_ختم
    }
    ورنہ
    {
	اگر (!got_click)			// didn't get click, ignore
	    واپس FALSE;
	اگر (!is_drag)			// release, reset got_click
	{
	    got_click = FALSE;
	    اگر (اندر_tab_سطر || اندر_tabpanel
#اگر_تعریف_شدہ FEAT_TABPANEL
		    || اندر_tabpanel_scrollbar
#اگر_ختم
		    )
	    {
		اندر_tab_سطر = FALSE;
		اندر_tabpanel = FALSE;
#اگر_تعریف_شدہ FEAT_TABPANEL
		اندر_tabpanel_scrollbar = false;
#اگر_ختم
		واپس FALSE;
	    }
	}
    }

#اگر_تعریف_شدہ FEAT_TABPANEL
    // Continue a scrollbar drag before any tab-selection handling.
    اگر (is_drag && اندر_tabpanel_scrollbar)
    {
	tabpanel_drag_scrollbar(mouse_صف);
	واپس FALSE;
    }
#اگر_ختم

    // CTRL right mouse button does CTRL-T
    اگر (is_click && (mod_نقاب & MOD_نقاب_CTRL) && which_button == MOUSE_دایاں)
    {
	اگر (State & منوال_داخل_کرو)
	    stuffcharReadbuff(Ctrl_O);
	اگر (گنتی > 1)
	    stuffnumReadbuff(گنتی);
	stuffcharReadbuff(Ctrl_T);
	got_click = FALSE;		// ignore drag&release now
	واپس FALSE;
    }

    // CTRL only works with left mouse button
    اگر ((mod_نقاب & MOD_نقاب_CTRL) && which_button != MOUSE_بایاں)
	واپس FALSE;

    // When a modifier is down, ignore drag and release events, as well as
    // multiple clicks and the middle mouse button.
    // Accept shift-leftmouse drags when 'mousemodel' is "popup.*".
    اگر ((mod_نقاب & (MOD_نقاب_سرکاؤ | MOD_نقاب_CTRL | MOD_نقاب_ALT
							     | MOD_نقاب_META))
	    && (!is_click
		|| (mod_نقاب & MOD_نقاب_MULTI_CLICK)
		|| which_button == MOUSE_درمیان)
	    && !((mod_نقاب & (MOD_نقاب_سرکاؤ|MOD_نقاب_ALT))
		&& mouse_model_popup()
		&& which_button == MOUSE_بایاں)
	    && !((mod_نقاب & MOD_نقاب_ALT)
		&& !mouse_model_popup()
		&& which_button == MOUSE_دایاں)
	    )
	واپس FALSE;

    // If the button press was used as the movement command for an operator
    // (eg "d<MOUSE>"), or it is the middle button that is held down, ignore
    // drag/release events.
    اگر (!is_click && which_button == MOUSE_درمیان)
	واپس FALSE;

    اگر (oap != تہی)
	regname = oap->regname;
    ورنہ
	regname = 0;

    // Middle mouse button does a 'put' of the selected text
    اگر (which_button == MOUSE_درمیان)
    {
	اگر (State == منوال_معمول)
	{
	    // If an operator was pending, we don't know what the user wanted
	    // to do. Go back to normal mode: Clear the operator and beep().
	    اگر (oap != تہی && oap->op_type != OP_NOP)
	    {
		clearopbeep(oap);
		واپس FALSE;
	    }

	    // If visual was active, yank the highlighted text and put it
	    // before the mouse pointer position.
	    // In Select mode replace the highlighted text with the clipboard.
	    اگر (VIsual_فعال)
	    {
		اگر (VIsual_select)
		{
		    stuffcharReadbuff(Ctrl_G);
		    stuffReadbuff((char_u *)"\"+p");
		}
		ورنہ
		{
		    stuffcharReadbuff('y');
		    stuffcharReadbuff(K_MIDDLEMOUSE);
		}
		do_always = TRUE;	// ignore 'mouse' setting next time
		واپس FALSE;
	    }
	    // The rest is below jump_to_mouse()
	}

	ورنہ اگر ((State & منوال_داخل_کرو) == 0)
	    واپس FALSE;

	// Middle click in insert mode doesn't move the mouse, just insert the
	// contents of a register.  '.' register is special, can't insert that
	// with do_put().
	// Also paste at the cursor if the current mode isn't in 'mouse' (only
	// happens for the GUI).
	اگر ((State & منوال_داخل_کرو) || !mouse_has(MOUSE_معمول))
	{
	    اگر (regname == '.')
		داخل_کرو_reg(regname, TRUE);
	    ورنہ
	    {
#اگر_تعریف_شدہ FEAT_CLIPBOARD
		اگر (clip_ستارہ.دستیاب && regname == 0)
		    regname = '*';
#اگر_ختم
		اگر ((State & REPLACE_FLAG) && !yank_رجسٹر_mline(regname))
		    داخل_کرو_reg(regname, TRUE);
		ورنہ
		{
		    do_put(regname, تہی, BACKWARD, 1L,
						      fixindent | PUT_CURSEND);

		    // Repeat it with CTRL-R CTRL-O r or CTRL-R CTRL-P r
		    Append_Char_مقصد_Redobuff(Ctrl_R);
		    Append_Char_مقصد_Redobuff(fixindent ? Ctrl_P : Ctrl_O);
		    Append_Char_مقصد_Redobuff(regname == 0 ? '"' : regname);
		}
	    }
	    واپس FALSE;
	}
    }

    // When dragging or button-up stay in the same window.
    اگر (!is_click)
	jump_flags |= MOUSE_FOCUS | MOUSE_DID_MOVE;

    آغاز_visual.lnum = 0;

    ساخت tabpage_label_info {
	bool is_panel;	    // label type. true: tabpanel, false: tab line
	bool just_اندر;	    // just in tabpage label area
	bool just_click;    // just click tabpage label area
	صحیح nr;		    // tabpage number
    } tp_label = { false, false, false, 0 };

    // Check for clicking in the tab page panel.
#اگر defined(FEAT_TABPANEL)
    اگر (mouse_ستون < firstwin->w_wincol
		|| mouse_ستون >= firstwin->w_wincol + topframe->fr_چوڑائی)
    {
	// A click on the scrollbar column starts a drag interaction and
	// preempts tab-selection.
	اگر (is_click && !is_drag && mouse_on_tabpanel_scrollbar())
	{
	    اندر_tabpanel_scrollbar = TRUE;
	    tabpanel_drag_scrollbar(mouse_صف);
	    واپس FALSE;
	}

	// Dispatch 'tabpanel' %[FuncName] click regions before falling through
	// to tab-page selection.  On drag events fall through to the normal
	// tab-drag handling.
	اگر (is_click && !is_drag
		&& stl_click_handler_regions(tabpanel_stl_click,
					    tabpanel_stl_click_گنتی,
					    0, (char_u *)"tabpanel",
					    mouse_صف, mouse_ستون,
					    which_button, mod_نقاب))
	    واپس FALSE;

	tp_label.is_panel = true;
	tp_label.just_اندر = true;
	tp_label.nr = get_tabpagenr_on_tabpanel();

	// click in a tab selects that tab page
	اگر (is_click && cmdwin_type == 0)
	    tp_label.just_click = true;
    }
    ورنہ
#اگر_ختم
    // Check for clicking in the tab page line.
    اگر (Tab_صفحہ_Idxs != تہی && mouse_صف == 0 && firstwin->w_winrow > 0)
    {
	// Dispatch 'tabline' %[FuncName] click regions before falling through
	// to tab-page selection.  On drag events fall through to the normal
	// tab-drag handling.
	اگر (is_click && !is_drag
		&& stl_click_handler_regions(tabline_stl_click,
					    tabline_stl_click_گنتی,
					    0, (char_u *)"tabline",
					    mouse_صف, mouse_ستون,
					    which_button, mod_نقاب))
	    واپس FALSE;

	tp_label.just_اندر = true;
	tp_label.nr = Tab_صفحہ_Idxs[mouse_ستون];

	// click in a tab selects that tab page
	اگر (is_click && cmdwin_type == 0
		&& mouse_ستون < firstwin->w_wincol + topframe->fr_چوڑائی)
	    tp_label.just_click = true;
    }

    اگر (tp_label.just_اندر)
    {
	اگر (is_drag)
	{
	    اگر (اندر_tabpanel || اندر_tab_سطر)
	    {
		c1 = tp_label.nr;
		tabpage_move(c1 <= 0 ? 9999 : c1 < tabpage_اشاریہ(curtab)
							    ? c1 - 1 : c1);
	    }
	    واپس FALSE;
	}

	اگر (tp_label.just_click)
	{
	    اگر (tp_label.is_panel)
		اندر_tabpanel = TRUE;
	    ورنہ
		اندر_tab_سطر = TRUE;
	    c1 = tp_label.nr;
	    اگر (c1 >= 0)
	    {
		اگر ((mod_نقاب & MOD_نقاب_MULTI_CLICK) == MOD_نقاب_2_CLICK
						    && !tp_label.is_panel)
		{
		    // Double-click on the tabline opens a new, empty tab page.
		    // The tabpanel has no "empty area" (every row maps to a tab)
		    // and this behavior is not documented for tabpanel, so fall
		    // through to the regular tab-switch path there.
		    انتہا_visual_منوال_keep_button();
		    tabpage_new();
		    tabpage_move(c1 == 0 ? 9999 : c1 - 1);
		}
		ورنہ
		{
		    // Go to specified tab page, or next one if not clicking
		    // on a label.
		    goto_tabpage(c1);

		    // It's like clicking on the status line of a window.
		    اگر (curwin != old_curwin)
			انتہا_visual_منوال_keep_button();
		}
	    }
	    ورنہ
	    {
		tabpage_T	*tp;

		// Close the current or specified tab page.
		اگر (c1 == -999)
		    tp = curtab;
		ورنہ
		    tp = ڈھونڈو_tabpage(-c1);
		اگر (tp == curtab)
		{
		    اگر (پہلا_tabpage->tp_اگلا != تہی)
			tabpage_close(FALSE);
		}
		ورنہ اگر (tp != تہی)
		    tabpage_close_other(tp, FALSE);
	    }
	}
	واپس TRUE;
    }
    ورنہ اگر (is_drag && (اندر_tabpanel || (اندر_tab_سطر && Tab_صفحہ_Idxs != تہی)))
    {
#اگر defined(FEAT_TABPANEL)
	اگر (اندر_tabpanel)
	    c1 = get_tabpagenr_on_tabpanel();
	ورنہ
#اگر_ختم
	    c1 = Tab_صفحہ_Idxs[mouse_ستون];
	tabpage_move(c1 <= 0 ? 9999 : c1 - 1);
	واپس FALSE;
    }

    // When 'mousemodel' is "popup" or "popup_setpos", translate mouse events:
    // right button up   -> pop-up menu
    // shift-left button -> right button
    // alt-left button   -> alt-right button
    اگر (mouse_model_popup())
    {
	اگر (which_button == MOUSE_دایاں
			    && !(mod_نقاب & (MOD_نقاب_سرکاؤ | MOD_نقاب_CTRL)))
	{
#اگر_تعریف_شدہ USE_POPUP_SETPOS
# ifdef FEAT_GUI
	    اگر (gui.اندر_use)
	    {
#  اگر defined(FEAT_GUI_MOTIF) || defined(FEAT_GUI_GTK) \
			  || defined(FEAT_GUI_PHOTON)
		اگر (!is_click)
		    // Ignore right button release events, only shows the popup
		    // menu on the button down event.
		    واپس FALSE;
#  endif
#  اگر defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_HAIKU)
		اگر (is_click || is_drag)
		    // Ignore right button down and drag mouse events.  Windows
		    // only shows the popup menu on the button up event.
		    واپس FALSE;
#  endif
	    }
# endif
# اگر defined(FEAT_GUI) && defined(FEAT_رکن_POPUP_MENU)
	    ورنہ
# endif
# اگر defined(FEAT_رکن_POPUP_MENU)
	    اگر (!is_click)
		// Ignore right button release events, only shows the popup
		// menu on the button down event.
		واپس FALSE;
# endif

	    jump_flags = 0;
	    اگر (STRCMP(p_mousem, "popup_setpos") == 0)
	    {
		// First set the cursor position before showing the popup
		// menu.
		اگر (VIsual_فعال)
		{
		    pos_T    m_pos;

		    // set MOUSE_MAY_STOP_VIS if we are outside the
		    // selection or the current window (might have false
		    // negative here)
		    اگر (mouse_صف < curwin->w_winrow
			 || mouse_صف
				  > (curwin->w_winrow + curwin->w_اونچائی))
			jump_flags = MOUSE_MAY_STOP_VIS;
		    ورنہ اگر (get_fpos_کا_mouse(&m_pos) != اندر_بفر)
			jump_flags = MOUSE_MAY_STOP_VIS;
		    ورنہ
		    {
			اگر (VIsual_منوال == 'V')
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
			ورنہ اگر ((LTOREQ_POS(curwin->w_cursor, VIsual)
				    && (LT_POS(m_pos, curwin->w_cursor)
					|| LT_POS(VIsual, m_pos)))
				|| (LT_POS(VIsual, curwin->w_cursor)
				    && (LT_POS(m_pos, VIsual)
				      || LT_POS(curwin->w_cursor, m_pos))))
			{
			    jump_flags = MOUSE_MAY_STOP_VIS;
			}
			ورنہ اگر (VIsual_منوال == Ctrl_V)
			{
			    getvcols(curwin, &curwin->w_cursor, &VIsual,
						     &leftcol, &rightcol, 0);
			    getvcol(curwin, &m_pos, تہی, &m_pos.ستون, تہی, 0);
			    اگر (m_pos.ستون < leftcol || m_pos.ستون > rightcol)
				jump_flags = MOUSE_MAY_STOP_VIS;
			}
		    }
		}
		ورنہ
		    jump_flags = MOUSE_MAY_STOP_VIS;
	    }
	    اگر (jump_flags)
	    {
		jump_flags = jump_مقصد_mouse(jump_flags, تہی, which_button);
		update_curbuf(VIsual_فعال ? UPD_INVERTED : UPD_VALID);
		setcursor();
		برآمد_flush();    // Update before showing popup menu
	    }
# ifdef FEAT_MENU
	    show_popupmenu();
	    got_click = FALSE;	// ignore release events
# endif
	    واپس (jump_flags & CURSOR_MOVED) != 0;
#ورنہ
	    واپس FALSE;
#اگر_ختم
	}
	اگر (which_button == MOUSE_بایاں
				&& (mod_نقاب & (MOD_نقاب_سرکاؤ|MOD_نقاب_ALT)))
	{
	    which_button = MOUSE_دایاں;
	    mod_نقاب &= ~MOD_نقاب_سرکاؤ;
	}
    }

    اگر ((State & (منوال_معمول | منوال_داخل_کرو))
			    && !(mod_نقاب & (MOD_نقاب_سرکاؤ | MOD_نقاب_CTRL)))
    {
	اگر (which_button == MOUSE_بایاں)
	{
	    اگر (is_click)
	    {
		// stop Visual mode for a left click in a window, but not when
		// on a status line
		اگر (VIsual_فعال)
		    jump_flags |= MOUSE_MAY_STOP_VIS;
	    }
	    ورنہ اگر (mouse_has(MOUSE_VISUAL))
		jump_flags |= MOUSE_MAY_VIS;
	}
	ورنہ اگر (which_button == MOUSE_دایاں)
	{
	    اگر (is_click && VIsual_فعال)
	    {
		// Remember the start and end of visual before moving the
		// cursor.
		اگر (LT_POS(curwin->w_cursor, VIsual))
		{
		    آغاز_visual = curwin->w_cursor;
		    انتہا_visual = VIsual;
		}
		ورنہ
		{
		    آغاز_visual = VIsual;
		    انتہا_visual = curwin->w_cursor;
		}
	    }
	    jump_flags |= MOUSE_FOCUS;
	    اگر (mouse_has(MOUSE_VISUAL))
		jump_flags |= MOUSE_MAY_VIS;
	}
    }

    // If an operator is pending, ignore all drags and releases until the
    // next mouse click.
    اگر (!is_drag && oap != تہی && oap->op_type != OP_NOP)
    {
	got_click = FALSE;
	oap->motion_type = MCHAR;
    }

    // When releasing the button let jump_to_mouse() know.
    اگر (!is_click && !is_drag)
	jump_flags |= MOUSE_RELEASED;

    // JUMP!
    jump_flags = jump_مقصد_mouse(jump_flags,
			oap == تہی ? تہی : &(oap->inclusive), which_button);

#اگر_تعریف_شدہ FEAT_MENU
    // A click in the window toolbar has no side effects.
    اگر (jump_flags & MOUSE_WINBAR)
	واپس FALSE;
#اگر_ختم
    moved = (jump_flags & CURSOR_MOVED);
    اندر_حالت_سطر_2 = (jump_flags & اندر_حالت_سطر);
    اندر_sep_سطر = (jump_flags & اندر_SEP_سطر);

    // Check for statusline click handler early, before visual mode or
    // other button-specific handling can interfere.
    اگر (اندر_حالت_سطر_2 && is_click && !is_drag
	    && stl_click_handler(dragwin, mouse_صف, mouse_ستون,
						which_button, mod_نقاب))
    {
#اگر_تعریف_شدہ FEAT_MOUSESHAPE
	اگر (!drag_حالت_سطر)
	{
	    drag_حالت_سطر = TRUE;
	    update_mouseshape(-1);
	}
#اگر_ختم
	واپس FALSE;
    }

#اگر_تعریف_شدہ FEAT_NETBEANS_INTG
    اگر (is_Netbeans_بفر(curbuf)
			    && !(jump_flags & (اندر_حالت_سطر | اندر_SEP_سطر)))
    {
	صحیح کلید = KEY2TERMCAP1(c);

	اگر (کلید == (صحیح)KE_LEFTRELEASE || کلید == (صحیح)KE_MIDDLERELEASE
					       || کلید == (صحیح)KE_RIGHTRELEASE)
	    netbeans_button_release(which_button);
    }
#اگر_ختم

    // When jumping to another window, clear a pending operator.  That's a bit
    // friendlier than beeping and not jumping to that window.
    اگر (curwin != old_curwin && oap != تہی && oap->op_type != OP_NOP)
	clearop(oap);

#اگر_تعریف_شدہ FEAT_FOLDING
    اگر (mod_نقاب == 0
	    && !is_drag
	    && (jump_flags & (MOUSE_FOLD_CLOSE | MOUSE_FOLD_OPEN))
	    && which_button == MOUSE_بایاں)
    {
	// open or close a fold at this line
	اگر (jump_flags & MOUSE_FOLD_OPEN)
	    openFold(curwin->w_cursor.lnum, 1L);
	ورنہ
	    closeFold(curwin->w_cursor.lnum, 1L);
	// don't move the cursor if still in the same window
	اگر (curwin == old_curwin)
	    curwin->w_cursor = save_cursor;
    }
#اگر_ختم

#اگر defined(FEAT_CLIPBOARD)
    اگر ((jump_flags & اندر_OTHER_WIN) && !VIsual_فعال &&
	    (clip_ستارہ.دستیاب || clip_plus.دستیاب))
    {
	clip_modeless(which_button, is_click, is_drag);
	واپس FALSE;
    }
#اگر_ختم

    // Set global flag that we are extending the Visual area with mouse
    // dragging; temporarily minimize 'scrolloff'.
    اگر (VIsual_فعال && is_drag && get_scrolloff_قدر())
    {
	// In the very first line, allow scrolling one line
	اگر (mouse_صف == 0)
	    mouse_dragging = 2;
	ورنہ
	    mouse_dragging = 1;
    }

    // When dragging the mouse above the window, scroll down.
    اگر (is_drag && mouse_صف < 0 && !اندر_حالت_سطر_2)
    {
	scroll_redraw(FALSE, 1L);
	mouse_صف = 0;
    }

    اگر (آغاز_visual.lnum)		// right click in visual mode
    {
       // When ALT is pressed make Visual mode blockwise.
       اگر (mod_نقاب & MOD_نقاب_ALT)
	   VIsual_منوال = Ctrl_V;

	// In Visual-block mode, divide the area in four, pick up the corner
	// that is in the quarter that the cursor is in.
	اگر (VIsual_منوال == Ctrl_V)
	{
	    getvcols(curwin, &آغاز_visual, &انتہا_visual,
						    &leftcol, &rightcol, 0);
	    اگر (curwin->w_curswant > (leftcol + rightcol) / 2)
		انتہا_visual.ستون = leftcol;
	    ورنہ
		انتہا_visual.ستون = rightcol;
	    اگر (curwin->w_cursor.lnum >=
				    (آغاز_visual.lnum + انتہا_visual.lnum) / 2)
		انتہا_visual.lnum = آغاز_visual.lnum;

	    // move VIsual to the right column
	    آغاز_visual = curwin->w_cursor;	    // save the cursor pos
	    curwin->w_cursor = انتہا_visual;
	    coladvance(انتہا_visual.ستون);
	    VIsual = curwin->w_cursor;
	    curwin->w_cursor = آغاز_visual;	    // restore the cursor
	}
	ورنہ
	{
	    // If the click is before the start of visual, change the start.
	    // If the click is after the end of visual, change the end.  If
	    // the click is inside the visual, change the closest side.
	    اگر (LT_POS(curwin->w_cursor, آغاز_visual))
		VIsual = انتہا_visual;
	    ورنہ اگر (LT_POS(انتہا_visual, curwin->w_cursor))
		VIsual = آغاز_visual;
	    ورنہ
	    {
		// In the same line, compare column number
		اگر (انتہا_visual.lnum == آغاز_visual.lnum)
		{
		    اگر (curwin->w_cursor.ستون - آغاز_visual.ستون >
				    انتہا_visual.ستون - curwin->w_cursor.ستون)
			VIsual = آغاز_visual;
		    ورنہ
			VIsual = انتہا_visual;
		}

		// In different lines, compare line number
		ورنہ
		{
		    diff = (curwin->w_cursor.lnum - آغاز_visual.lnum) -
				(انتہا_visual.lnum - curwin->w_cursor.lnum);

		    اگر (diff > 0)		// closest to end
			VIsual = آغاز_visual;
		    ورنہ اگر (diff < 0)	// closest to start
			VIsual = انتہا_visual;
		    ورنہ			// in the middle line
		    {
			اگر (curwin->w_cursor.ستون <
					(آغاز_visual.ستون + انتہا_visual.ستون) / 2)
			    VIsual = انتہا_visual;
			ورنہ
			    VIsual = آغاز_visual;
		    }
		}
	    }
	}
    }
    // If Visual mode started in insert mode, execute "CTRL-O"
    ورنہ اگر ((State & منوال_داخل_کرو) && VIsual_فعال)
	stuffcharReadbuff(Ctrl_O);

    // Middle mouse click: Put text before cursor.
    اگر (which_button == MOUSE_درمیان)
    {
#اگر_تعریف_شدہ FEAT_CLIPBOARD
	اگر (clip_ستارہ.دستیاب && regname == 0)
	    regname = '*';
#اگر_ختم
	اگر (yank_رجسٹر_mline(regname))
	{
	    اگر (mouse_past_bottom)
		dir = FORWARD;
	}
	ورنہ اگر (mouse_past_eol)
	    dir = FORWARD;

	اگر (fixindent)
	{
	    c1 = (dir == BACKWARD) ? '[' : ']';
	    c2 = 'p';
	}
	ورنہ
	{
	    c1 = (dir == FORWARD) ? 'p' : 'P';
	    c2 = NUL;
	}
	prep_redo(regname, گنتی, NUL, c1, NUL, c2, NUL);

	// Remember where the paste started, so in edit() Insstart can be set
	// to this position
	اگر (restart_edit != 0)
	    where_paste_started = curwin->w_cursor;
	do_put(regname, تہی, dir, گنتی, fixindent | PUT_CURSEND);
    }

#اگر defined(FEAT_QUICKFIX)
    // Ctrl-Mouse click or double click in a quickfix window jumps to the
    // error under the mouse pointer.
    ورنہ اگر (((mod_نقاب & MOD_نقاب_CTRL)
		|| (mod_نقاب & MOD_نقاب_MULTI_CLICK) == MOD_نقاب_2_CLICK)
	    && bt_quickfix(curbuf))
    {
	اگر (curwin->w_llist_ref == تہی)	// quickfix window
	    do_cmdline_cmd((char_u *)".cc");
	ورنہ					// location list window
	    do_cmdline_cmd((char_u *)".ll");
	got_click = FALSE;		// ignore drag&release now
    }
#اگر_ختم

    // Ctrl-Mouse click (or double click in a help window) jumps to the tag
    // under the mouse pointer.
    ورنہ اگر ((mod_نقاب & MOD_نقاب_CTRL) || (curbuf->b_help
		     && (mod_نقاب & MOD_نقاب_MULTI_CLICK) == MOD_نقاب_2_CLICK))
    {
	اگر (State & منوال_داخل_کرو)
	    stuffcharReadbuff(Ctrl_O);
	stuffcharReadbuff(Ctrl_RSB);
	got_click = FALSE;		// ignore drag&release now
    }

    // Shift-Mouse click searches for the next occurrence of the word under
    // the mouse pointer
    ورنہ اگر ((mod_نقاب & MOD_نقاب_سرکاؤ))
    {
	اگر ((State & منوال_داخل_کرو) || (VIsual_فعال && VIsual_select))
	    stuffcharReadbuff(Ctrl_O);
	اگر (which_button == MOUSE_بایاں)
	    stuffcharReadbuff('*');
	ورنہ	// MOUSE_RIGHT
	    stuffcharReadbuff('#');
    }

    // Handle double clicks, unless on status line
    ورنہ اگر (اندر_حالت_سطر_2)
    {
#اگر_تعریف_شدہ FEAT_MOUSESHAPE
	اگر ((is_drag || is_click) && !drag_حالت_سطر)
	{
	    drag_حالت_سطر = TRUE;
	    update_mouseshape(-1);
	}
#اگر_ختم
    }
    ورنہ اگر (اندر_sep_سطر)
    {
#اگر_تعریف_شدہ FEAT_MOUSESHAPE
	اگر ((is_drag || is_click) && !drag_sep_سطر)
	{
	    drag_sep_سطر = TRUE;
	    update_mouseshape(-1);
	}
#اگر_ختم
    }
    ورنہ اگر ((mod_نقاب & MOD_نقاب_MULTI_CLICK)
				       && (State & (منوال_معمول | منوال_داخل_کرو))
	     && mouse_has(MOUSE_VISUAL))
    {
	اگر (is_click || !VIsual_فعال)
	{
	    اگر (VIsual_فعال)
		orig_cursor = VIsual;
	    ورنہ
	    {
		check_visual_highlight();
		VIsual = curwin->w_cursor;
		orig_cursor = VIsual;
		VIsual_فعال = TRUE;
		VIsual_reselect = TRUE;
		// start Select mode if 'selectmode' contains "mouse"
		may_آغاز_select('o');
		setmouse();
	    }
	    اگر ((mod_نقاب & MOD_نقاب_MULTI_CLICK) == MOD_نقاب_2_CLICK)
	    {
		// Double click with ALT pressed makes it blockwise.
		اگر (mod_نقاب & MOD_نقاب_ALT)
		    VIsual_منوال = Ctrl_V;
		ورنہ
		    VIsual_منوال = 'v';
	    }
	    ورنہ اگر ((mod_نقاب & MOD_نقاب_MULTI_CLICK) == MOD_نقاب_3_CLICK)
		VIsual_منوال = 'V';
	    ورنہ اگر ((mod_نقاب & MOD_نقاب_MULTI_CLICK) == MOD_نقاب_4_CLICK)
		VIsual_منوال = Ctrl_V;
#اگر_تعریف_شدہ FEAT_CLIPBOARD
	    // Make sure the clipboard gets updated.  Needed because start and
	    // end may still be the same, and the selection needs to be owned
	    clip_ستارہ.vmode = NUL;
#اگر_ختم
	}
	// A double click selects a word or a block.
	اگر ((mod_نقاب & MOD_نقاب_MULTI_CLICK) == MOD_نقاب_2_CLICK)
	{
	    pos_T	*pos = تہی;
	    صحیح		gc;

	    اگر (is_click)
	    {
		// If the character under the cursor (skipping white space) is
		// not a word character, try finding a match and select a (),
		// {}, [], #if/#endif, etc. block.
		انتہا_visual = curwin->w_cursor;
		جب (gc = gchar_pos(&انتہا_visual), VIM_ISWHITE(gc))
		    inc(&انتہا_visual);
		اگر (oap != تہی)
		    oap->motion_type = MCHAR;
		اگر (oap != تہی
			&& VIsual_منوال == 'v'
			&& !vim_iswordc(gchar_pos(&انتہا_visual))
			&& EQUAL_POS(curwin->w_cursor, VIsual)
			&& (pos = findmatch(oap, NUL)) != تہی)
		{
		    curwin->w_cursor = *pos;
		    اگر (oap->motion_type == MLINE)
			VIsual_منوال = 'V';
		    ورنہ اگر (*p_sel == 'e')
		    {
			اگر (LT_POS(curwin->w_cursor, VIsual))
			    ++VIsual.ستون;
			ورنہ
			    ++curwin->w_cursor.ستون;
		    }
		}
	    }

	    اگر (pos == تہی && (is_click || is_drag))
	    {
		// When not found a match or when dragging: extend to include
		// a word.
		اگر (LT_POS(curwin->w_cursor, orig_cursor))
		{
		    ڈھونڈو_آغاز_کا_لفظ(&curwin->w_cursor);
		    ڈھونڈو_انتہا_کا_لفظ(&VIsual);
		}
		ورنہ
		{
		    ڈھونڈو_آغاز_کا_لفظ(&VIsual);
		    اگر (*p_sel == 'e' && *ml_get_cursor() != NUL)
			curwin->w_cursor.ستون +=
					 (*mb_ptr2len)(ml_get_cursor());
		    ڈھونڈو_انتہا_کا_لفظ(&curwin->w_cursor);
		}
	    }
	    curwin->w_سیٹ_curswant = true;
	}
	اگر (is_click)
	    redraw_curbuf_later(UPD_INVERTED);	// update the inversion
    }
    ورنہ اگر (VIsual_فعال && !old_فعال)
    {
	اگر (mod_نقاب & MOD_نقاب_ALT)
	    VIsual_منوال = Ctrl_V;
	ورنہ
	    VIsual_منوال = 'v';
    }

    // If Visual mode changed show it later.
    اگر ((!VIsual_فعال && old_فعال && منوال_displayed)
	    || (VIsual_فعال && p_smd && پیغام_silent == 0
				 && (!old_فعال || VIsual_منوال != old_منوال)))
	redraw_cmdline = TRUE;

    واپس moved;
}

    خالی
ins_mouse(صحیح c)
{
    pos_T	tpos;
    win_T	*old_curwin = curwin;

#اگر_تعریف_شدہ FEAT_GUI
    // When GUI is active, also move/paste when 'mouse' is empty
    اگر (!gui.اندر_use)
#اگر_ختم
	اگر (!mouse_has(MOUSE_داخل_کرو))
	    واپس;

    undisplay_dollar();
    tpos = curwin->w_cursor;
    اگر (do_mouse(تہی, c, BACKWARD, 1L, 0))
    {
	win_T	*new_curwin = curwin;

	اگر (curwin != old_curwin && win_valid(old_curwin))
	{
	    // Mouse took us to another window.  We need to go back to the
	    // previous one to stop insert there properly.
	    curwin = old_curwin;
	    curbuf = curwin->w_بفر;
#اگر_تعریف_شدہ FEAT_کام_CHANNEL
	    اگر (bt_prompt(curbuf))
		// Restart Insert mode when re-entering the prompt buffer.
		curbuf->b_prompt_داخل_کرو = 'A';
#اگر_ختم
	}
	آغاز_arrow(curwin == old_curwin ? &tpos : تہی);
	اگر (curwin != new_curwin && win_valid(new_curwin))
	{
	    curwin = new_curwin;
	    curbuf = curwin->w_بفر;
	}
	سیٹ_can_cindent(TRUE);
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
    ساکن خالی
do_mousescroll(cmdarg_T *cap)
{
    صحیح سرکاؤ_or_ctrl = mod_نقاب & (MOD_نقاب_سرکاؤ | MOD_نقاب_CTRL);

#اگر_تعریف_شدہ FEAT_TERMINAL
    اگر (رکن_use_loop())
	// This window is a terminal window, send the mouse event there.
	// Set "typed" to FALSE to avoid an endless loop.
	send_کلیدیں_مقصد_رکن(curbuf->b_رکن, cap->cmdchar, mod_نقاب, FALSE);
    ورنہ
#اگر_ختم
    اگر (cap->arg == MSCR_اوپر || cap->arg == MSCR_نیچے_کو)
    {
	// Vertical scrolling
	اگر (!(State & منوال_داخل_کرو) && (mouse_vert_قدم < 0 || سرکاؤ_or_ctrl))
	{
	    // whole page up or down
	    pagescroll(cap->arg == MSCR_اوپر ? FORWARD : BACKWARD, 1L, FALSE);
	}
	ورنہ
	{
	    اگر (mouse_vert_قدم < 0 || سرکاؤ_or_ctrl)
	    {
		// whole page up or down
		cap->count1 = (طویل)(curwin->w_botline - curwin->w_topline);
	    }
	    // Don't scroll more than half the window height.
	    ورنہ اگر (curwin->w_اونچائی < mouse_vert_قدم * 2)
	    {
		cap->count1 = curwin->w_اونچائی / 2;
		اگر (cap->count1 == 0)
		    cap->count1 = 1;
	    }
	    ورنہ
	    {
		cap->count1 = mouse_vert_قدم;
	    }
	    cap->count0 = cap->count1;
	    nv_scroll_سطر(cap);
	}

#اگر_تعریف_شدہ FEAT_PROP_POPUP
	اگر (WIN_IS_POPUP(curwin))
	    popup_سیٹ_firstline(curwin);
#اگر_ختم
    }
    ورنہ
    {
	// Horizontal scrolling
	طویل قدم = (mouse_hor_قدم < 0 || سرکاؤ_or_ctrl)
					    ? curwin->w_چوڑائی : mouse_hor_قدم;
	طویل leftcol = curwin->w_leftcol
				     + (cap->arg == MSCR_دایاں ? -قدم : قدم);
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
    خالی
ins_mousescroll(صحیح dir)
{
    cmdarg_T	cap;
    oparg_T	oa;
    CLEAR_میدان(cap);
    clear_oparg(&oa);
    cap.oap = &oa;
    cap.arg = dir;

#اگر_تعریف_شدہ FEAT_TABPANEL
    اگر (mouse_صف >= 0 && mouse_ستون >= 0
	    && (dir == MSCR_اوپر || dir == MSCR_نیچے_کو)
	    && mouse_on_tabpanel())
    {
	(خالی)tabpanel_scroll(dir == MSCR_اوپر ? 1 : -1,
		mouse_vert_قدم > 0 ? mouse_vert_قدم : 3);
	واپس;
    }
#اگر_ختم

    انتخاب (dir)
    {
	حالت MSCR_اوپر:
	    cap.cmdchar = K_MOUSEUP;
	    توڑو;
	حالت MSCR_نیچے_کو:
	    cap.cmdchar = K_MOUSEDOWN;
	    توڑو;
	حالت MSCR_بایاں:
	    cap.cmdchar = K_MOUSELEFT;
	    توڑو;
	حالت MSCR_دایاں:
	    cap.cmdchar = K_MOUSERIGHT;
	    توڑو;
	طےشدہ:
	    siemsg("Invalid ins_mousescroll() argument: %d", dir);
    }

    win_T *old_curwin = curwin;
    اگر (mouse_صف >= 0 && mouse_ستون >= 0)
    {
	// Find the window at the mouse pointer coordinates.
	// NOTE: Must restore "curwin" to "old_curwin" before returning!
	صحیح صف = mouse_صف;
	صحیح ستون = mouse_ستون;
	curwin = mouse_ڈھونڈو_win(&صف, &ستون, ڈھونڈو_POPUP);
	اگر (curwin == تہی)
	{
	    curwin = old_curwin;
	    واپس;
	}
	curbuf = curwin->w_بفر;
    }

    اگر (curwin == old_curwin)
    {
	// Don't scroll the current window if the popup menu is visible.
	اگر (pum_visible())
	    واپس;

	undisplay_dollar();
    }

    linenr_T	orig_topline = curwin->w_topline;
    colnr_T	orig_leftcol = curwin->w_leftcol;
    pos_T	orig_cursor = curwin->w_cursor;

    // Call the common mouse scroll function shared with other modes.
    do_mousescroll(&cap);

    صحیح did_scroll = (orig_topline != curwin->w_topline
		   || orig_leftcol != curwin->w_leftcol);

    curwin->w_redr_حالت = true;
    curwin = old_curwin;
    curbuf = curwin->w_بفر;

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
	آغاز_arrow(&orig_cursor);
	سیٹ_can_cindent(TRUE);
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

    CLEAR_میدان(cap);
    clear_oparg(&oa);
    cap.oap = &oa;
    cap.arg = dir;

    انتخاب (dir)
    {
	حالت MSCR_اوپر:	 cap.cmdchar = K_MOUSEUP; توڑو;
	حالت MSCR_نیچے_کو:	 cap.cmdchar = K_MOUSEDOWN; توڑو;
	حالت MSCR_بایاں:	 cap.cmdchar = K_MOUSELEFT; توڑو;
	حالت MSCR_دایاں: cap.cmdchar = K_MOUSERIGHT; توڑو;
    }

    اگر (mouse_صف < 0 || mouse_ستون < 0)
	واپس FALSE;

    صحیح	    صف = mouse_صف;
    صحیح	    ستون = mouse_ستون;
    win_T   *wp;

    // Only scroll when the mouse is on top of the info popup.
    wp = mouse_ڈھونڈو_win(&صف, &ستون, ڈھونڈو_POPUP);
    اگر (wp == تہی || !WIN_IS_POPUP(wp) || !(wp->w_popup_flags & POPF_INFO)
							|| !wp->w_has_scrollbar)
	واپس FALSE;

    win_T   *old_curwin = curwin;

    curwin = wp;
    curbuf = wp->w_بفر;
    // Call the common mouse scroll function shared with other modes.
    do_mousescroll(&cap);
    curwin = old_curwin;
    curbuf = curwin->w_بفر;

    // Cmdline mode doesn't normally call update_screen(), so redraw the
    // completion popup menu, which also repaints the info popup.
    اگر (cmdline_pum_فعال())
	cmdline_pum_display();
    واپس TRUE;
}
#اگر_ختم

/*
 * Return TRUE if "c" is a mouse key.
 */
    صحیح
is_mouse_کلید(صحیح c)
{
    واپس c == K_LEFTMOUSE
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

ساکن ساخت mousetable
{
    صحیح	    pseudo_رمز;	// Code for pseudo mouse event
    صحیح	    button;		// Which mouse button is it?
    صحیح	    is_click;		// Is it a mouse button click event?
    صحیح	    is_drag;		// Is it a mouse drag event?
} mouse_جدول[] =
{
    {(صحیح)KE_LEFTMOUSE,		MOUSE_بایاں,	TRUE,	FALSE},
#اگر_تعریف_شدہ FEAT_GUI
    {(صحیح)KE_LEFTMOUSE_NM,	MOUSE_بایاں,	TRUE,	FALSE},
#اگر_ختم
    {(صحیح)KE_LEFTDRAG,		MOUSE_بایاں,	FALSE,	TRUE},
    {(صحیح)KE_LEFTRELEASE,	MOUSE_بایاں,	FALSE,	FALSE},
#اگر_تعریف_شدہ FEAT_GUI
    {(صحیح)KE_LEFTRELEASE_NM,	MOUSE_بایاں,	FALSE,	FALSE},
#اگر_ختم
    {(صحیح)KE_MIDDLEMOUSE,	MOUSE_درمیان,	TRUE,	FALSE},
    {(صحیح)KE_MIDDLEDRAG,	MOUSE_درمیان,	FALSE,	TRUE},
    {(صحیح)KE_MIDDLERELEASE,	MOUSE_درمیان,	FALSE,	FALSE},
    {(صحیح)KE_RIGHTMOUSE,	MOUSE_دایاں,	TRUE,	FALSE},
    {(صحیح)KE_RIGHTDRAG,		MOUSE_دایاں,	FALSE,	TRUE},
    {(صحیح)KE_RIGHTRELEASE,	MOUSE_دایاں,	FALSE,	FALSE},
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
get_mouse_button(صحیح رمز, صحیح *is_click, صحیح *is_drag)
{
    صحیح	    i;

    برائے (i = 0; mouse_جدول[i].pseudo_رمز; i++)
	اگر (رمز == mouse_جدول[i].pseudo_رمز)
	{
	    *is_click = mouse_جدول[i].is_click;
	    *is_drag = mouse_جدول[i].is_drag;
	    واپس mouse_جدول[i].button;
	}
    واپس 0;	    // Shouldn't get here
}

/*
 * Return the appropriate pseudo mouse event token (KE_LEFTMOUSE etc) based on
 * the given information about which mouse button is down, and whether the
 * mouse was clicked, dragged or released.
 */
    صحیح
get_pseudo_mouse_رمز(
    صحیح	    button,	// eg MOUSE_LEFT
    صحیح	    is_click,
    صحیح	    is_drag)
{
    صحیح	    i;

    برائے (i = 0; mouse_جدول[i].pseudo_رمز; i++)
	اگر (button == mouse_جدول[i].button
	    && is_click == mouse_جدول[i].is_click
	    && is_drag == mouse_جدول[i].is_drag)
	{
#اگر_تعریف_شدہ FEAT_GUI
	    // Trick: a non mappable left click and release has mouse_col -1
	    // or added MOUSE_COLOFF.  Used for 'mousefocus' in
	    // gui_mouse_moved()
	    اگر (mouse_ستون < 0 || mouse_ستون > MOUSE_COLOFF)
	    {
		اگر (mouse_ستون < 0)
		    mouse_ستون = 0;
		ورنہ
		    mouse_ستون -= MOUSE_COLOFF;
		اگر (mouse_جدول[i].pseudo_رمز == (صحیح)KE_LEFTMOUSE)
		    واپس (صحیح)KE_LEFTMOUSE_NM;
		اگر (mouse_جدول[i].pseudo_رمز == (صحیح)KE_LEFTRELEASE)
		    واپس (صحیح)KE_LEFTRELEASE_NM;
	    }
#اگر_ختم
	    واپس mouse_جدول[i].pseudo_رمز;
	}
    واپس (صحیح)KE_IGNORE;	    // not recognized, ignore it
}

#تعریف_کرو HMT_معمول	1
#تعریف_کرو HMT_NETTERM	2
#تعریف_کرو HMT_DEC	4
#تعریف_کرو HMT_JSBTERM	8
#تعریف_کرو HMT_PTERM	16
#تعریف_کرو HMT_URXVT	32
#تعریف_کرو HMT_GPM	64
#تعریف_کرو HMT_SGR	128
#تعریف_کرو HMT_SGR_REL	256
ساکن صحیح has_mouse_termcode = 0;

    خالی
سیٹ_mouse_termcode(
    صحیح		n,	// KS_MOUSE, KS_NETTERM_MOUSE or KS_DEC_MOUSE
    char_u	*s)
{
    char_u	name[2];

    name[0] = n;
    name[1] = KE_FILLER;
    جوڑو_termcode(name, s, FALSE);
#اگر_تعریف_شدہ FEAT_MOUSE_JSB
    اگر (n == KS_JSBTERM_MOUSE)
	has_mouse_termcode |= HMT_JSBTERM;
    ورنہ
#اگر_ختم
#اگر_تعریف_شدہ FEAT_MOUSE_NET
    اگر (n == KS_NETTERM_MOUSE)
	has_mouse_termcode |= HMT_NETTERM;
    ورنہ
#اگر_ختم
#اگر_تعریف_شدہ FEAT_MOUSE_DEC
    اگر (n == KS_DEC_MOUSE)
	has_mouse_termcode |= HMT_DEC;
    ورنہ
#اگر_ختم
#اگر_تعریف_شدہ FEAT_MOUSE_PTERM
    اگر (n == KS_PTERM_MOUSE)
	has_mouse_termcode |= HMT_PTERM;
    ورنہ
#اگر_ختم
#اگر_تعریف_شدہ FEAT_MOUSE_URXVT
    اگر (n == KS_URXVT_MOUSE)
	has_mouse_termcode |= HMT_URXVT;
    ورنہ
#اگر_ختم
#اگر_تعریف_شدہ FEAT_MOUSE_GPM
    اگر (n == KS_GPM_MOUSE)
	has_mouse_termcode |= HMT_GPM;
    ورنہ
#اگر_ختم
    اگر (n == KS_SGR_MOUSE)
	has_mouse_termcode |= HMT_SGR;
    ورنہ اگر (n == KS_SGR_MOUSE_RELEASE)
	has_mouse_termcode |= HMT_SGR_REL;
    ورنہ
	has_mouse_termcode |= HMT_معمول;
}

#اگر defined(UNIX) || defined(VMS)
    خالی
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
    ورنہ
# endif
# ifdef FEAT_MOUSE_NET
    اگر (n == KS_NETTERM_MOUSE)
	has_mouse_termcode &= ~HMT_NETTERM;
    ورنہ
# endif
# ifdef FEAT_MOUSE_DEC
    اگر (n == KS_DEC_MOUSE)
	has_mouse_termcode &= ~HMT_DEC;
    ورنہ
# endif
# ifdef FEAT_MOUSE_PTERM
    اگر (n == KS_PTERM_MOUSE)
	has_mouse_termcode &= ~HMT_PTERM;
    ورنہ
# endif
# ifdef FEAT_MOUSE_URXVT
    اگر (n == KS_URXVT_MOUSE)
	has_mouse_termcode &= ~HMT_URXVT;
    ورنہ
# endif
# ifdef FEAT_MOUSE_GPM
    اگر (n == KS_GPM_MOUSE)
	has_mouse_termcode &= ~HMT_GPM;
    ورنہ
# endif
    اگر (n == KS_SGR_MOUSE)
	has_mouse_termcode &= ~HMT_SGR;
    ورنہ اگر (n == KS_SGR_MOUSE_RELEASE)
	has_mouse_termcode &= ~HMT_SGR_REL;
    ورنہ
	has_mouse_termcode &= ~HMT_معمول;
}
#اگر_ختم

/*
 * setmouse() - switch mouse on/off depending on current mode and 'mouse'
 */
    خالی
setmouse(خالی)
{
    صحیح	    checkfor;

#اگر_تعریف_شدہ FEAT_MOUSESHAPE
    update_mouseshape(-1);
#اگر_ختم

    // Should be outside proc, but may break MOUSESHAPE
#اگر_تعریف_شدہ FEAT_GUI
    // In the GUI the mouse is always enabled.
    اگر (gui.اندر_use)
	واپس;
#اگر_ختم
    // be quick when mouse is off
    اگر (*p_mouse == NUL || has_mouse_termcode == 0)
	واپس;

    // don't switch mouse on when not in raw mode (Ex mode)
    اگر (cur_tmode != TMODE_RAW)
    {
	mch_setmouse(FALSE);
	واپس;
    }

    اگر (VIsual_فعال)
	checkfor = MOUSE_VISUAL;
    ورنہ اگر (State == منوال_HITRETURN || State == منوال_ASKMORE
						     || State == منوال_SETWSIZE)
	checkfor = MOUSE_RETURN;
    ورنہ اگر (State & منوال_داخل_کرو)
	checkfor = MOUSE_داخل_کرو;
    ورنہ اگر (State & منوال_CMDLINE)
	checkfor = MOUSE_COMMAND;
    ورنہ اگر (State == منوال_CONFIRM || State == منوال_EXTERNCMD)
	checkfor = ' '; // don't use mouse for ":confirm" or ":!cmd"
    ورنہ
	checkfor = MOUSE_معمول;    // assume normal mode

    اگر (mouse_has(checkfor))
	mch_setmouse(TRUE);
    ورنہ
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

    برائے (p = p_mouse; *p; ++p)
	انتخاب (*p)
	{
	    حالت 'a': اگر (vim_strchr((char_u *)MOUSE_A, c) != تہی)
			  واپس TRUE;
		      توڑو;
	    حالت MOUSE_HELP: اگر (c != MOUSE_RETURN && curbuf->b_help)
				 واپس TRUE;
			     توڑو;
	    طےشدہ: اگر (c == *p) واپس TRUE; توڑو;
	}
    واپس FALSE;
}

/*
 * Return TRUE when 'mousemodel' is set to "popup" or "popup_setpos".
 */
    صحیح
mouse_model_popup(خالی)
{
    واپس (p_mousem[0] == 'p');
}

/*
 * Call a click-region callback function.
 * "regions"/"region_count" describe the resolved click regions,
 * "winid" is stored as the "winid" key in the info dict (0 for tabline).
 * Returns TRUE if the function was called and handled the click.
 */
    ساکن صحیح
stl_click_handler_regions(
	stl_click_region_T  *regions,
	صحیح		    region_گنتی,
	صحیح		    winid,
	char_u		    *area_name,
	صحیح		    mrow,
	صحیح		    mcol,
	صحیح		    which_button,
	صحیح		    mods)
{
#اگر_تعریف_شدہ FEAT_EVAL
    صحیح		n;
    صحیح		nclicks;
    char_u	button_str[2];
    char_u	mods_str[4];
    صحیح		mi = 0;
    dict_T	*info;
    typval_T	argvars[2];
    typval_T	rettv;
    funcexe_T	funcexe;
    صحیح		ستون = mcol;

    اگر (regions == تہی || region_گنتی == 0)
	واپس FALSE;

    // Find the click region at the given row and column.
    برائے (n = 0; n < region_گنتی; n++)
    {
	اگر (regions[n].صف == mrow
		&& ستون >= regions[n].ستون_آغاز
		&& ستون < regions[n].ستون_انتہا)
	    توڑو;
    }
    اگر (n >= region_گنتی || regions[n].funcname == تہی)
	واپس FALSE;

    // Build the info dictionary.
    info = dict_alloc();
    اگر (info == تہی)
	واپس FALSE;

    dict_جوڑو_نمبر(info, "minwid", regions[n].minwid);

    // Determine number of clicks.
    // MOD_MASK_2CLICK=0x20, MOD_MASK_3CLICK=0x40, MOD_MASK_4CLICK=0x60
    nclicks = ((mods & MOD_نقاب_MULTI_CLICK) >> 5) + 1;
    اگر (nclicks > 3)
	nclicks = 3;
    dict_جوڑو_نمبر(info, "nclicks", nclicks);

    // Button.
    اگر (which_button == MOUSE_بایاں)
	button_str[0] = 'l';
    ورنہ اگر (which_button == MOUSE_دایاں)
	button_str[0] = 'r';
    ورنہ
	button_str[0] = 'm';
    button_str[1] = NUL;
    dict_جوڑو_string_len(info, "button", button_str, 1);

    // Modifiers.
    اگر (mods & MOD_نقاب_سرکاؤ)
	mods_str[mi++] = 's';
    اگر (mods & MOD_نقاب_CTRL)
	mods_str[mi++] = 'c';
    اگر (mods & MOD_نقاب_ALT)
	mods_str[mi++] = 'a';
    mods_str[mi] = NUL;
    dict_جوڑو_string_len(info, "mods", mods_str, mi);

    dict_جوڑو_نمبر(info, "winid", winid);

    // "area": which option the clicked region belongs to.  Lets a shared
    // dispatcher distinguish 'statusline', 'tabline' and 'tabpanel' without
    // having to overload winid == 0.
    dict_جوڑو_string(info, "area", area_name);

    // Expose tab page number for 'tabpanel' regions.
    اگر (regions[n].tabnr > 0)
	dict_جوڑو_نمبر(info, "tabnr", regions[n].tabnr);

    // Call the function with the info dict as argument.
    argvars[0].v_type = VAR_DICT;
    argvars[0].vval.v_dict = info;
    ++info->dv_refcount;
    argvars[1].v_type = VAR_UNKNOWN;

    rettv.v_type = VAR_نمبر;
    rettv.vval.v_نمبر = 0;

    CLEAR_میدان(funcexe);
    funcexe.fe_evaluate = TRUE;
    (خالی)call_func(regions[n].funcname, -1,
					    &rettv, 1, argvars, &funcexe);

    n = (صحیح)rettv.vval.v_نمبر;
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

    واپس TRUE;
#ورنہ
    (خالی)regions;
    (خالی)region_گنتی;
    (خالی)winid;
    (خالی)area_name;
    (خالی)mrow;
    (خالی)mcol;
    (خالی)which_button;
    (خالی)mods;
    واپس FALSE;
#اگر_ختم
}

/*
 * Call a statusline click handler function for window "wp".
 * Returns TRUE if the function was called and handled the click.
 */
    ساکن صحیح
stl_click_handler(win_T *wp, صحیح mrow, صحیح mcol, صحیح which_button, صحیح mods)
{
    اگر (wp == تہی)
	واپس FALSE;
    واپس stl_click_handler_regions(wp->w_stl_click, wp->w_stl_click_گنتی,
				wp->w_id, (char_u *)"statusline",
				mrow, mcol, which_button, mods);
}

// dragwin is declared near the top of the file

/*
 * Reset the window being dragged.  To be called when switching tab page.
 */
    خالی
reset_dragwin(خالی)
{
    dragwin = تہی;
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
jump_مقصد_mouse(
    صحیح		flags,
    صحیح		*inclusive,	// used for inclusive operator, can be NULL
    صحیح		which_button)	// MOUSE_LEFT, MOUSE_RIGHT, MOUSE_MIDDLE
{
    ساکن صحیح	on_حالت_سطر = 0;	// #lines below bottom of window
    ساکن صحیح	on_sep_سطر = 0;	// on separator right of window
#اگر_تعریف_شدہ FEAT_MENU
    ساکن صحیح  اندر_winbar = FALSE;
#اگر_ختم
#اگر_تعریف_شدہ FEAT_PROP_POPUP
    ساکن صحیح   اندر_popup_win = FALSE;
    ساکن win_T *click_اندر_popup_win = تہی;
#اگر_ختم
    ساکن صحیح	پچھلا_صف = -1;
    ساکن صحیح	پچھلا_ستون = -1;
    ساکن صحیح	did_drag = FALSE;	// drag was noticed

    win_T	*wp, *old_curwin;
    pos_T	old_cursor;
    صحیح		گنتی;
    صحیح		پہلا;
    صحیح		صف = mouse_صف;
    صحیح		ستون = mouse_ستون;
    colnr_T	ستون_from_screen = -1;
#اگر_تعریف_شدہ FEAT_FOLDING
    صحیح		mouse_char = ' ';
#اگر_ختم

    mouse_past_bottom = FALSE;
    mouse_past_eol = FALSE;

    اگر (flags & MOUSE_RELEASED)
    {
	// On button release we may change window focus if positioned on a
	// status line and no dragging happened.
	اگر (dragwin != تہی && !did_drag)
	    flags &= ~(MOUSE_FOCUS | MOUSE_DID_MOVE);
	dragwin = تہی;
	did_drag = FALSE;
#اگر_تعریف_شدہ FEAT_PROP_POPUP
	اگر (click_اندر_popup_win != تہی && popup_dragwin == تہی)
	    popup_close_for_mouse_click(click_اندر_popup_win);

	popup_dragwin = تہی;
	click_اندر_popup_win = تہی;
#اگر_ختم
    }

    اگر ((flags & MOUSE_DID_MOVE)
	    && پچھلا_صف == mouse_صف
	    && پچھلا_ستون == mouse_ستون)
    {
retnomove:
	// before moving the cursor for a left click which is NOT in a status
	// line, stop Visual mode
	اگر (on_حالت_سطر)
	    واپس اندر_حالت_سطر;
	اگر (on_sep_سطر)
	    واپس اندر_SEP_سطر;
#اگر_تعریف_شدہ FEAT_MENU
	اگر (اندر_winbar)
	{
	    // A quick second click may arrive as a double-click, but we use it
	    // as a second click in the WinBar.
	    اگر ((mod_نقاب & MOD_نقاب_MULTI_CLICK) && !(flags & MOUSE_RELEASED))
	    {
		wp = mouse_ڈھونڈو_win(&صف, &ستون, FAIL_POPUP);
		اگر (wp == تہی)
		    واپس اندر_UNKNOWN;
		winbar_click(wp, ستون);
	    }
	    واپس اندر_OTHER_WIN | MOUSE_WINBAR;
	}
#اگر_ختم
	اگر (flags & MOUSE_MAY_STOP_VIS)
	{
	    انتہا_visual_منوال_keep_button();
	    redraw_curbuf_later(UPD_INVERTED);	// delete the inversion
	}
#اگر defined(FEAT_CLIPBOARD)
	// Continue a modeless selection in another window.
	اگر (cmdwin_type != 0 && صف < cmdwin_win->w_winrow)
	    واپس اندر_OTHER_WIN;
#اگر_ختم
#اگر_تعریف_شدہ FEAT_PROP_POPUP
	// Continue a modeless selection in a popup window or dragging it.
	اگر (اندر_popup_win)
	{
	    click_اندر_popup_win = تہی;  // don't close it on release
	    اگر (popup_dragwin != تہی)
	    {
		// dragging a popup window
		popup_drag(popup_dragwin);
		واپس اندر_UNKNOWN;
	    }
	    واپس اندر_OTHER_WIN;
	}
#اگر_ختم
	واپس اندر_بفر;
    }

    پچھلا_صف = mouse_صف;
    پچھلا_ستون = mouse_ستون;

    اگر (flags & MOUSE_SETPOS)
	جاؤ retnomove;				// ugly goto...

    old_curwin = curwin;
    old_cursor = curwin->w_cursor;

    اگر (!(flags & MOUSE_FOCUS))
    {
	اگر (صف < 0 || ستون < 0) // check if it makes sense
	    واپس اندر_UNKNOWN;

	// find the window where the row is in and adjust "row" and "col" to be
	// relative to top-left of the window
	wp = mouse_ڈھونڈو_win(&صف, &ستون, ڈھونڈو_POPUP);
	اگر (wp == تہی)
	    واپس اندر_UNKNOWN;
	dragwin = تہی;

#اگر_تعریف_شدہ FEAT_PROP_POPUP
	// Click in a popup window may start dragging or modeless selection,
	// but not much else.
	اگر (WIN_IS_POPUP(wp))
	{
	    on_sep_سطر = 0;
	    on_حالت_سطر = 0;
	    اندر_popup_win = TRUE;
	    اگر (which_button == MOUSE_بایاں && popup_close_if_on_X(wp, صف, ستون))
	    {
		واپس اندر_UNKNOWN;
	    }
	    ورنہ اگر (((wp->w_popup_flags & (POPF_DRAG | POPF_RESIZE))
					      && popup_on_border(wp, صف, ستون))
				       || (wp->w_popup_flags & POPF_DRAGALL))
	    {
		popup_dragwin = wp;
		popup_آغاز_drag(wp, صف, ستون);
		واپس اندر_UNKNOWN;
	    }
	    // Only close on release, otherwise it's not possible to drag or do
	    // modeless selection.
	    ورنہ اگر (wp->w_popup_close == POPCLOSE_CLICK
		    && which_button == MOUSE_بایاں)
	    {
		click_اندر_popup_win = wp;
	    }
	    ورنہ اگر (which_button == MOUSE_بایاں)
		// If the click is in the scrollbar, may scroll up/down.
		popup_handle_scrollbar_click(wp, صف, ستون);
# ifdef FEAT_CLIPBOARD
	    واپس اندر_OTHER_WIN;
# ورنہ
	    واپس اندر_UNKNOWN;
# endif
	}
	اندر_popup_win = FALSE;
	popup_dragwin = تہی;
#اگر_ختم
#اگر_تعریف_شدہ FEAT_MENU
	اگر (صف == -1)
	{
	    // A click in the window toolbar does not enter another window or
	    // change Visual highlighting.
	    winbar_click(wp, ستون);
	    اندر_winbar = TRUE;
	    واپس اندر_OTHER_WIN | MOUSE_WINBAR;
	}
	اندر_winbar = FALSE;
#اگر_ختم

	// winpos and height may change in win_enter()!
	اگر (صف >= wp->w_اونچائی)		// In (or below) status line
	{
	    on_حالت_سطر = صف - wp->w_اونچائی + 1;
	    dragwin = wp;
	}
	ورنہ
	    on_حالت_سطر = 0;
	اگر (ستون >= wp->w_چوڑائی)		// In separator line
	{
	    on_sep_سطر = ستون - wp->w_چوڑائی + 1;
	    dragwin = wp;
	}
	ورنہ
	    on_sep_سطر = 0;

	// The rightmost character of the status line might be a vertical
	// separator character if there is no connecting window to the right.
	اگر (on_حالت_سطر && on_sep_سطر)
	{
	    اگر (stl_connected(wp))
		on_sep_سطر = 0;
	    ورنہ
		on_حالت_سطر = 0;
	}

	// Before jumping to another buffer, or moving the cursor for a left
	// click, stop Visual mode.
	اگر (VIsual_فعال
		&& (wp->w_بفر != curwin->w_بفر
		    || (!on_حالت_سطر && !on_sep_سطر
#اگر_تعریف_شدہ FEAT_FOLDING
			&& (
# ifdef FEAT_RIGHTLEFT
			    wp->w_p_rl ? ستون < wp->w_چوڑائی - wp->w_p_fdc :
# endif
			    ستون >= wp->w_p_fdc + (wp != cmdwin_win ? 0 : 1)
			    )
#اگر_ختم
			&& (flags & MOUSE_MAY_STOP_VIS))))
	{
	    انتہا_visual_منوال_keep_button();
	    redraw_curbuf_later(UPD_INVERTED);	// delete the inversion
	}
	اگر (cmdwin_type != 0 && wp != cmdwin_win)
	{
	    // A click outside the command-line window: Use modeless
	    // selection if possible.  Allow dragging the status lines.
	    on_sep_سطر = 0;
#اگر_تعریف_شدہ FEAT_CLIPBOARD
	    اگر (on_حالت_سطر)
		واپس اندر_حالت_سطر;
	    واپس اندر_OTHER_WIN;
#ورنہ
	    صف = 0;
	    ستون += wp->w_wincol;
	    wp = cmdwin_win;
#اگر_ختم
	}
#اگر defined(FEAT_PROP_POPUP) && defined(FEAT_TERMINAL)
	اگر (popup_is_popup(curwin) && curbuf->b_رکن != تہی)
	    // terminal in popup window: don't jump to another window
	    واپس اندر_OTHER_WIN;
#اگر_ختم
	// Only change window focus when not clicking on or dragging the
	// status line.  Do change focus when releasing the mouse button
	// (MOUSE_FOCUS was set above if we dragged first).
	اگر (dragwin == تہی || (flags & MOUSE_RELEASED))
	    win_enter(wp, TRUE);		// can make wp invalid!

	اگر (curwin != old_curwin)
	{
#اگر_تعریف_شدہ CHECK_DOUBLE_CLICK
	    // set topline, to be able to check for double click ourselves
	    سیٹ_mouse_topline(curwin);
#اگر_ختم
#اگر_تعریف_شدہ FEAT_TERMINAL
	    // when entering a terminal window may change state
	    رکن_win_entered();
#اگر_ختم
	}
	اگر (on_حالت_سطر)			// In (or below) status line
	{
	    // Don't use start_arrow() if we're in the same window
	    اگر (curwin == old_curwin)
		واپس اندر_حالت_سطر;
	    ورنہ
		واپس اندر_حالت_سطر | CURSOR_MOVED;
	}
	اگر (on_sep_سطر)			// In (or below) status line
	{
	    // Don't use start_arrow() if we're in the same window
	    اگر (curwin == old_curwin)
		واپس اندر_SEP_سطر;
	    ورنہ
		واپس اندر_SEP_سطر | CURSOR_MOVED;
	}

	curwin->w_cursor.lnum = curwin->w_topline;
#اگر_تعریف_شدہ FEAT_GUI
	// remember topline, needed for double click
	gui_پچھلا_topline = curwin->w_topline;
# ifdef FEAT_DIFF
	gui_پچھلا_topfill = curwin->w_topfill;
# endif
#اگر_ختم
    }
    ورنہ اگر (on_حالت_سطر && which_button == MOUSE_بایاں)
    {
	اگر (dragwin != تہی)
	{
	    // Drag the status line
	    گنتی = صف - W_WINROW(dragwin) - dragwin->w_اونچائی + 1
							     - on_حالت_سطر;
	    win_drag_حالت_سطر(dragwin, گنتی);
	    did_drag |= گنتی;
	}
	واپس اندر_حالت_سطر;			// Cursor didn't move
    }
    ورنہ اگر (on_sep_سطر && which_button == MOUSE_بایاں)
    {
	اگر (dragwin != تہی)
	{
	    // Drag the separator column
	    گنتی = ستون - dragwin->w_wincol - dragwin->w_چوڑائی + 1
								- on_sep_سطر;
	    win_drag_vsep_سطر(dragwin, گنتی);
	    did_drag |= گنتی;
	}
	واپس اندر_SEP_سطر;			// Cursor didn't move
    }
#اگر_تعریف_شدہ FEAT_MENU
    ورنہ اگر (اندر_winbar)
    {
	// After a click on the window toolbar don't start Visual mode.
	واپس اندر_OTHER_WIN | MOUSE_WINBAR;
    }
#اگر_ختم
    ورنہ // keep_window_focus must be TRUE
    {
	// before moving the cursor for a left click, stop Visual mode
	اگر (flags & MOUSE_MAY_STOP_VIS)
	{
	    انتہا_visual_منوال_keep_button();
	    redraw_curbuf_later(UPD_INVERTED);	// delete the inversion
	}

#اگر defined(FEAT_CLIPBOARD)
	// Continue a modeless selection in another window.
	اگر (cmdwin_type != 0 && صف < cmdwin_win->w_winrow)
	    واپس اندر_OTHER_WIN;
#اگر_ختم
#اگر_تعریف_شدہ FEAT_PROP_POPUP
	اگر (اندر_popup_win)
	{
	    اگر (popup_dragwin != تہی)
	    {
		// dragging a popup window
		popup_drag(popup_dragwin);
		واپس اندر_UNKNOWN;
	    }
	    // continue a modeless selection in a popup window
	    click_اندر_popup_win = تہی;
	    واپس اندر_OTHER_WIN;
	}
#اگر_ختم

	صف -= W_WINROW(curwin);
	ستون -= curwin->w_wincol;

	// When clicking beyond the end of the window, scroll the screen.
	// Scroll by however many rows outside the window we are.
	اگر (صف < 0)
	{
	    گنتی = 0;
	    برائے (پہلا = TRUE; curwin->w_topline > 1; )
	    {
#اگر_تعریف_شدہ FEAT_DIFF
		اگر (curwin->w_topfill < diff_check_fill(curwin, curwin->w_topline))
		    ++گنتی;
		ورنہ
#اگر_ختم
		    گنتی += plines(curwin->w_topline - 1);
		اگر (!پہلا && گنتی > -صف)
		    توڑو;
		پہلا = FALSE;
#اگر_تعریف_شدہ FEAT_FOLDING
		(خالی)hasFolding(curwin->w_topline, &curwin->w_topline, تہی);
#اگر_ختم
#اگر_تعریف_شدہ FEAT_DIFF
		اگر (curwin->w_topfill < diff_check_fill(curwin, curwin->w_topline))
		    ++curwin->w_topfill;
		ورنہ
#اگر_ختم
		{
		    --curwin->w_topline;
#اگر_تعریف_شدہ FEAT_DIFF
		    curwin->w_topfill = 0;
#اگر_ختم
		}
	    }
#اگر_تعریف_شدہ FEAT_DIFF
	    check_topfill(curwin, FALSE);
#اگر_ختم
	    curwin->w_valid &=
		      ~(VALID_WROW|VALID_CROW|VALID_BOTLINE|VALID_BOTLINE_AP);
	    redraw_later(UPD_VALID);
	    صف = 0;
	}
	ورنہ اگر (صف >= curwin->w_اونچائی)
	{
	    گنتی = 0;
	    برائے (پہلا = TRUE; curwin->w_topline < curbuf->b_ml.ml_سطر_گنتی; )
	    {
#اگر_تعریف_شدہ FEAT_DIFF
		اگر (curwin->w_topfill > 0)
		    ++گنتی;
		ورنہ
#اگر_ختم
		    گنتی += plines(curwin->w_topline);
		اگر (!پہلا && گنتی > صف - curwin->w_اونچائی + 1)
		    توڑو;
		پہلا = FALSE;
#اگر_تعریف_شدہ FEAT_FOLDING
		اگر (hasFolding(curwin->w_topline, تہی, &curwin->w_topline)
			&& curwin->w_topline == curbuf->b_ml.ml_سطر_گنتی)
		    توڑو;
#اگر_ختم
#اگر_تعریف_شدہ FEAT_DIFF
		اگر (curwin->w_topfill > 0)
		    --curwin->w_topfill;
		ورنہ
#اگر_ختم
		{
		    ++curwin->w_topline;
#اگر_تعریف_شدہ FEAT_DIFF
		    curwin->w_topfill =
				   diff_check_fill(curwin, curwin->w_topline);
#اگر_ختم
		}
	    }
#اگر_تعریف_شدہ FEAT_DIFF
	    check_topfill(curwin, FALSE);
#اگر_ختم
	    redraw_later(UPD_VALID);
	    curwin->w_valid &=
		      ~(VALID_WROW|VALID_CROW|VALID_BOTLINE|VALID_BOTLINE_AP);
	    صف = curwin->w_اونچائی - 1;
	}
	ورنہ اگر (صف == 0)
	{
	    // When dragging the mouse, while the text has been scrolled up as
	    // far as it goes, moving the mouse in the top line should scroll
	    // the text down (done later when recomputing w_topline).
	    اگر (mouse_dragging > 0
		    && curwin->w_cursor.lnum
				       == curwin->w_بفر->b_ml.ml_سطر_گنتی
		    && curwin->w_cursor.lnum == curwin->w_topline)
		curwin->w_valid &= ~(VALID_TOPLINE);
	}
    }

    اگر (پچھلا_صف >= W_WINROW(curwin)
	&& پچھلا_صف < W_WINROW(curwin) + curwin->w_اونچائی
	&& پچھلا_ستون >= curwin->w_wincol && پچھلا_ستون < W_ENDCOL(curwin)
						       && Screen_سطریں != تہی)
    {
	صحیح off = سطر_انحراف[پچھلا_صف] + پچھلا_ستون;

	// Only use ScreenCols[] after the window was redrawn.  Mainly matters
	// for tests, a user would not click before redrawing.
	// Do not use when 'virtualedit' is active.
	اگر (curwin->w_redr_type <= UPD_VALID_NO_UPDATE)
	    ستون_from_screen = Screen_ستون_تعداد[off];
#اگر_تعریف_شدہ FEAT_FOLDING
	// Remember the character under the mouse, it might be a '-' or '+' in
	// the fold column.
	mouse_char = enc_utf8 && Screen_سطریں_UC[off] != 0
				       ? Screen_سطریں_UC[off] : Screen_سطریں[off];
#اگر_ختم
    }

#اگر_تعریف_شدہ FEAT_FOLDING
    // Check for position outside of the fold column.
    اگر (
# ifdef FEAT_RIGHTLEFT
	    curwin->w_p_rl ? ستون < curwin->w_چوڑائی - curwin->w_p_fdc :
# endif
	    ستون >= curwin->w_p_fdc + (cmdwin_win != curwin ? 0 : 1)
       )
	mouse_char = ' ';
#اگر_ختم

    // compute the position in the buffer line from the posn on the screen
    اگر (mouse_comp_pos(curwin, &صف, &ستون, &curwin->w_cursor.lnum, تہی))
	mouse_past_bottom = TRUE;

    // Start Visual mode before coladvance(), for when 'sel' != "old"
    اگر ((flags & MOUSE_MAY_VIS) && !VIsual_فعال)
    {
	check_visual_highlight();
	VIsual = old_cursor;
	VIsual_فعال = TRUE;
	VIsual_reselect = TRUE;
	// if 'selectmode' contains "mouse", start Select mode
	may_آغاز_select('o');
	setmouse();
	اگر (p_smd && پیغام_silent == 0)
	    redraw_cmdline = TRUE;	// show visual mode later
    }

    اگر (ستون_from_screen >= 0)
    {
	// Use the virtual column from ScreenCols[], it is accurate also after
	// concealed characters.
	ستون = ستون_from_screen;
    }

    curwin->w_curswant = ستون;
    curwin->w_سیٹ_curswant = false;	// May still have been TRUE
    اگر (coladvance(ستون) == FAIL)	// Mouse click beyond end of line
    {
	اگر (inclusive != تہی)
	    *inclusive = TRUE;
	mouse_past_eol = TRUE;
    }
    ورنہ اگر (inclusive != تہی)
	*inclusive = FALSE;

    گنتی = اندر_بفر;
    اگر (curwin != old_curwin || curwin->w_cursor.lnum != old_cursor.lnum
	    || curwin->w_cursor.ستون != old_cursor.ستون)
	گنتی |= CURSOR_MOVED;		// Cursor has moved

#اگر_تعریف_شدہ FEAT_FOLDING
    اگر (mouse_char == curwin->w_fill_chars.foldclosed)
	گنتی |= MOUSE_FOLD_OPEN;
    ورنہ اگر (mouse_char != ' ')
	گنتی |= MOUSE_FOLD_CLOSE;
#اگر_ختم

    واپس گنتی;
}

/*
 * Make a horizontal scroll to "leftcol".
 * Return TRUE if the cursor moved, FALSE otherwise.
 */
    صحیح
do_mousescroll_horiz(long_u leftcol)
{
    اگر (curwin->w_p_wrap)
	واپس FALSE;  // no horizontal scrolling when wrapping

    اگر (curwin->w_leftcol == (colnr_T)leftcol)
	واپس FALSE;  // already there

    // When the line of the cursor is too short, move the cursor to the
    // longest visible line.
    اگر (
#اگر_تعریف_شدہ FEAT_GUI
	    (!gui.اندر_use || vim_strchr(p_go, GO_HORSCROLL) == تہی) &&
#اگر_ختم
		    !virtual_فعال()
	    && (طویل)leftcol > scroll_سطر_len(curwin->w_cursor.lnum))
    {
	curwin->w_cursor.lnum = ui_ڈھونڈو_longest_lnum();
	curwin->w_cursor.ستون = 0;
    }

    واپس سیٹ_leftcol((colnr_T)leftcol);
}

/*
 * Normal and Visual modes implementation for scrolling in direction
 * "cap->arg", which is one of the MSCR_ values.
 */
    خالی
nv_mousescroll(cmdarg_T *cap)
{
    win_T   *old_curwin = curwin;

#اگر_تعریف_شدہ FEAT_TABPANEL
    اگر (mouse_صف >= 0 && mouse_ستون >= 0
	    && (cap->arg == MSCR_اوپر || cap->arg == MSCR_نیچے_کو)
	    && mouse_on_tabpanel())
    {
	(خالی)tabpanel_scroll(cap->arg == MSCR_اوپر ? 1 : -1,
		mouse_vert_قدم > 0 ? mouse_vert_قدم : 3);
	واپس;
    }
#اگر_ختم

    اگر (mouse_صف >= 0 && mouse_ستون >= 0)
    {
	// Find the window at the mouse pointer coordinates.
	// NOTE: Must restore "curwin" to "old_curwin" before returning!
	صحیح صف = mouse_صف;
	صحیح ستون = mouse_ستون;
	curwin = mouse_ڈھونڈو_win(&صف, &ستون, ڈھونڈو_POPUP);
	اگر (curwin == تہی)
	{
	    curwin = old_curwin;
	    واپس;
	}

#اگر_تعریف_شدہ FEAT_PROP_POPUP
	اگر (WIN_IS_POPUP(curwin) && !curwin->w_has_scrollbar)
	{
	    // cannot scroll this popup window
	    curwin = old_curwin;
	    واپس;
	}
#اگر_ختم
	curbuf = curwin->w_بفر;
    }

    // Call the common mouse scroll function shared with other modes.
    do_mousescroll(cap);

    curwin->w_redr_حالت = true;
    curwin = old_curwin;
    curbuf = curwin->w_بفر;
}

/*
 * Mouse clicks and drags.
 */
    خالی
nv_mouse(cmdarg_T *cap)
{
    (خالی)do_mouse(cap->oap, cap->cmdchar, BACKWARD, cap->count1, 0);
}

ساکن صحیح	held_button = MOUSE_RELEASE;

    خالی
reset_held_button(خالی)
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
    char_u	*modifiers_آغاز,
    صحیح		idx,
    صحیح		*modifiers)
{
    صحیح		j;
    char_u	*p;
#اگر !defined(UNIX) || defined(FEAT_MOUSE_XTERM) || defined(FEAT_GUI) \
    || defined(FEAT_MOUSE_GPM) || defined(FEAT_SYSMOUSE)
    char_u	بائٹیں[6];
    صحیح		عدد_بائٹیں;
#اگر_ختم
    صحیح		mouse_رمز = 0;	    // init for GCC
    صحیح		is_click, is_drag;
    صحیح		is_release, release_is_ambiguous;
    صحیح		wheel_رمز = 0;
    صحیح		current_button;
    ساکن صحیح	orig_عدد_clicks = 1;
    ساکن صحیح	orig_mouse_رمز = 0x0;
#اگر_تعریف_شدہ CHECK_DOUBLE_CLICK
    ساکن صحیح	orig_mouse_ستون = 0;
    ساکن صحیح	orig_mouse_صف = 0;
    ساکن ساخت timeval  orig_mouse_وقت = {0, 0};
    // time of previous mouse click
    ساخت timeval  mouse_وقت;		// time of current mouse click
    طویل	timediff;		// elapsed time in msec
#اگر_ختم

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
	برائے (;;)
	{
	    // For the GUI and for MS-Windows two bytes each are used for row
	    // and column.  Allows for more than 223 columns.
# اگر defined(FEAT_GUI) || defined(MSWIN)
	    اگر (TRUE
#  اگر defined(FEAT_GUI) && !defined(MSWIN)
		&& gui.اندر_use
#  endif
		)
	    {
		عدد_بائٹیں = get_بائٹیں_from_buf(tp + *slen, بائٹیں, 5);
		اگر (عدد_بائٹیں == -1)	// not enough coordinates
		    واپس -1;
		mouse_رمز = بائٹیں[0];
		mouse_ستون = 128 * (بائٹیں[1] - ' ' - 1)
		    + بائٹیں[2] - ' ' - 1;
		mouse_صف = 128 * (بائٹیں[3] - ' ' - 1)
		    + بائٹیں[4] - ' ' - 1;
	    }
	    ورنہ
# endif
	    {
		عدد_بائٹیں = get_بائٹیں_from_buf(tp + *slen, بائٹیں, 3);
		اگر (عدد_بائٹیں == -1)	// not enough coordinates
		    واپس -1;
		mouse_رمز = بائٹیں[0];
		mouse_ستون = بائٹیں[1] - ' ' - 1;
		mouse_صف = بائٹیں[2] - ' ' - 1;
	    }
	    *slen += عدد_بائٹیں;

	    // If the following bytes is also a mouse code and it has the same
	    // code, dump this one and get the next.  This makes dragging a
	    // whole lot faster.
# ifdef FEAT_GUI
	    اگر (gui.اندر_use)
		j = 3;
	    ورنہ
# endif
		j = get_termcode_len(idx);
	    اگر (STRNCMP(tp, tp + *slen, (size_t)j) == 0
		    && tp[*slen + j] == mouse_رمز
		    && tp[*slen + j + 1] != NUL
		    && tp[*slen + j + 2] != NUL
# ifdef FEAT_GUI
		    && (!gui.اندر_use
			|| (tp[*slen + j + 3] != NUL
			    && tp[*slen + j + 4] != NUL))
# endif
	       )
		*slen += j;
	    ورنہ
		توڑو;
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
	p = modifiers_آغاز;
	اگر (p == تہی)
	    واپس -1;

	mouse_رمز = getdigits(&p);
	اگر (*p++ != ';')
	    واپس -1;

	// when mouse reporting is SGR, add 32 to mouse code
	اگر (کلید_name[0] == KS_SGR_MOUSE
		|| کلید_name[0] == KS_SGR_MOUSE_RELEASE)
	    mouse_رمز += 32;

	mouse_ستون = getdigits(&p) - 1;
	اگر (*p++ != ';')
	    واپس -1;

	mouse_صف = getdigits(&p) - 1;

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
	    mouse_رمز |= MOUSE_RELEASE;
	}
    }
    ورنہ
    {
	release_is_ambiguous = TRUE;
	اگر ((mouse_رمز & MOUSE_RELEASE) == MOUSE_RELEASE)
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
	اگر (mouse_رمز >= MOUSEWHEEL_کم && mouse_رمز < MOUSESIDEBUTTONS_کم
#  ifdef FEAT_GUI
		&& !gui.اندر_use
#  endif
#  ifdef FEAT_MOUSE_GPM
		&& کلید_name[0] != KS_GPM_MOUSE
#  endif
	   )
	{
#  اگر defined(UNIX)
	    اگر (use_xterm_mouse() > 1 && mouse_رمز >= 0x80)
		// mouse-move event, using MOUSE_DRAG works
		mouse_رمز = MOUSE_DRAG;
	    ورنہ
#  endif
		// Keep the mouse_code before it's changed, so that we
		// remember that it was a mouse wheel click.
		wheel_رمز = mouse_رمز;
	}
#  ifdef FEAT_MOUSE_XTERM
	ورنہ اگر (held_button == MOUSE_RELEASE
#   ifdef FEAT_GUI
		&& !gui.اندر_use
#   endif
		&& (mouse_رمز == 0x23 || mouse_رمز == 0x24
		    || mouse_رمز == 0x40 || mouse_رمز == 0x41))
	{
	    // Apparently 0x23 and 0x24 are used by rxvt scroll wheel.
	    // And 0x40 and 0x41 are used by some xterm emulator.
	    wheel_رمز = mouse_رمز - (mouse_رمز >= 0x40 ? 0x40 : 0x23)
							      + MOUSEWHEEL_کم;
	}
#  endif

#  اگر defined(UNIX)
	ورنہ اگر (use_xterm_mouse() > 1)
	{
	    اگر (mouse_رمز & MOUSE_DRAG_XTERM)
		mouse_رمز |= MOUSE_DRAG;
	}
#  endif
#  ifdef FEAT_XCLIPBOARD
	ورنہ اگر (!(mouse_رمز & MOUSE_DRAG & ~MOUSE_CLICK_نقاب))
	{
	    اگر (is_release)
		stop_xterm_trace();
	    ورنہ
		آغاز_xterm_trace(mouse_رمز);
	}
#  endif
# endif
    }
#اگر_ختم // !UNIX || FEAT_MOUSE_XTERM
#اگر_تعریف_شدہ FEAT_MOUSE_NET
    اگر (کلید_name[0] == KS_NETTERM_MOUSE)
    {
	صحیح mc, mr;

	// expect a rather limited sequence like: balancing {
	// \033}6,45\r
	// '6' is the row, 45 is the column
	p = tp + *slen;
	mr = getdigits(&p);
	اگر (*p++ != ',')
	    واپس -1;
	mc = getdigits(&p);
	اگر (*p++ != '\r')
	    واپس -1;

	mouse_ستون = mc - 1;
	mouse_صف = mr - 1;
	mouse_رمز = MOUSE_بایاں;
	*slen += (صحیح)(p - (tp + *slen));
    }
#اگر_ختم	// FEAT_MOUSE_NET
#اگر_تعریف_شدہ FEAT_MOUSE_JSB
    اگر (کلید_name[0] == KS_JSBTERM_MOUSE)
    {
	صحیح mult, val, iter, button, حالت_2;

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
	button = mouse_رمز = 0;
	انتخاب (*p++)
	{
	    حالت 'L': button = 1; توڑو;
	    حالت '-': توڑو;
	    حالت 'x': توڑو; // ignore sequence
	    طےشدہ:  واپس -1; // Unknown Result
	}
	انتخاب (*p++)
	{
	    حالت 'M': button |= 2; توڑو;
	    حالت '-': توڑو;
	    حالت 'x': توڑو; // ignore sequence
	    طےشدہ:  واپس -1; // Unknown Result
	}
	انتخاب (*p++)
	{
	    حالت 'R': button |= 4; توڑو;
	    حالت '-': توڑو;
	    حالت 'x': توڑو; // ignore sequence
	    طےشدہ:  واپس -1; // Unknown Result
	}
	حالت_2 = *p++;
	برائے (val = 0, mult = 100, iter = 0; iter < 3; iter++,
		mult /= 10, p++)
	    اگر (*p >= '0' && *p <= '9')
		val += (*p - '0') * mult;
	    ورنہ
		واپس -1;
	mouse_ستون = val;
	برائے (val = 0, mult = 100, iter = 0; iter < 3; iter++,
		mult /= 10, p++)
	    اگر (*p >= '0' && *p <= '9')
		val += (*p - '0') * mult;
	    ورنہ
		واپس -1;
	mouse_صف = val;
	انتخاب (*p++)
	{
	    حالت 's': button |= 8; توڑو;  // SHIFT key Pressed
	    حالت '-': توڑو;  // Not Pressed
	    حالت 'x': توڑو;  // Not Reporting
	    طےشدہ:  واپس -1; // Unknown Result
	}
	انتخاب (*p++)
	{
	    حالت 'c': button |= 16; توڑو;  // CTRL key Pressed
	    حالت '-': توڑو;  // Not Pressed
	    حالت 'x': توڑو;  // Not Reporting
	    طےشدہ:  واپس -1; // Unknown Result
	}
	اگر (*p++ != '\033')
	    واپس -1;
	اگر (*p++ != '\\')
	    واپس -1;
	انتخاب (حالت_2)
	{
	    حالت 'D': // Double Click
	    حالت 'S': // Single Click
		اگر (button & 1) mouse_رمز |= MOUSE_بایاں;
		اگر (button & 2) mouse_رمز |= MOUSE_درمیان;
		اگر (button & 4) mouse_رمز |= MOUSE_دایاں;
		اگر (button & 8) mouse_رمز |= MOUSE_سرکاؤ;
		اگر (button & 16) mouse_رمز |= MOUSE_CTRL;
		توڑو;
	    حالت 'm': // Mouse move
		اگر (button & 1) mouse_رمز |= MOUSE_بایاں;
		اگر (button & 2) mouse_رمز |= MOUSE_درمیان;
		اگر (button & 4) mouse_رمز |= MOUSE_دایاں;
		اگر (button & 8) mouse_رمز |= MOUSE_سرکاؤ;
		اگر (button & 16) mouse_رمز |= MOUSE_CTRL;
		اگر ((button & 7) != 0)
		{
		    held_button = mouse_رمز;
		    mouse_رمز |= MOUSE_DRAG;
		}
		is_drag = TRUE;
		showmode();
		توڑو;
	    حالت 'd': // Button Down
		اگر (button & 1) mouse_رمز |= MOUSE_بایاں;
		اگر (button & 2) mouse_رمز |= MOUSE_درمیان;
		اگر (button & 4) mouse_رمز |= MOUSE_دایاں;
		اگر (button & 8) mouse_رمز |= MOUSE_سرکاؤ;
		اگر (button & 16) mouse_رمز |= MOUSE_CTRL;
		توڑو;
	    حالت 'u': // Button Up
		is_release = TRUE;
		اگر (button & 1)
		    mouse_رمز |= MOUSE_بایاں;
		اگر (button & 2)
		    mouse_رمز |= MOUSE_درمیان;
		اگر (button & 4)
		    mouse_رمز |= MOUSE_دایاں;
		اگر (button & 8)
		    mouse_رمز |= MOUSE_سرکاؤ;
		اگر (button & 16)
		    mouse_رمز |= MOUSE_CTRL;
		توڑو;
	    طےشدہ: واپس -1; // Unknown Result
	}

	*slen += (p - (tp + *slen));
    }
#اگر_ختم // FEAT_MOUSE_JSB
#اگر_تعریف_شدہ FEAT_MOUSE_DEC
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
	    واپس -1;

	// get button status
	Pb = getdigits(&p);
	اگر (*p++ != ';')
	    واپس -1;

	// get row status
	Pr = getdigits(&p);
	اگر (*p++ != ';')
	    واپس -1;

	// get column status
	Pc = getdigits(&p);

	// the page parameter is optional
	اگر (*p == ';')
	{
	    p++;
	    (خالی)getdigits(&p);
	}
	اگر (*p++ != '&')
	    واپس -1;
	اگر (*p++ != 'w')
	    واپس -1;

	mouse_رمز = 0;
	انتخاب (Pe)
	{
	    حالت  0: واپس -1; // position request while unavailable
	    حالت  1: // a response to a locator position request includes
		     //	the status of all buttons
		     Pb &= 7;   // mask off and ignore fourth button
		     اگر (Pb & 4)
			 mouse_رمز  = MOUSE_بایاں;
		     اگر (Pb & 2)
			 mouse_رمز  = MOUSE_درمیان;
		     اگر (Pb & 1)
			 mouse_رمز  = MOUSE_دایاں;
		     اگر (Pb)
		     {
			 held_button = mouse_رمز;
			 mouse_رمز |= MOUSE_DRAG;
			 WantQueryMouse = TRUE;
		     }
		     is_drag = TRUE;
		     showmode();
		     توڑو;
	    حالت  2: mouse_رمز = MOUSE_بایاں;
		     WantQueryMouse = TRUE;
		     توڑو;
	    حالت  3: mouse_رمز = MOUSE_بایاں;
		     is_release = TRUE;
		     توڑو;
	    حالت  4: mouse_رمز = MOUSE_درمیان;
		     WantQueryMouse = TRUE;
		     توڑو;
	    حالت  5: mouse_رمز = MOUSE_درمیان;
		     is_release = TRUE;
		     توڑو;
	    حالت  6: mouse_رمز = MOUSE_دایاں;
		     WantQueryMouse = TRUE;
		     توڑو;
	    حالت  7: mouse_رمز = MOUSE_دایاں;
		     is_release = TRUE;
		     توڑو;
	    حالت  8: واپس -1; // fourth button down
	    حالت  9: واپس -1; // fourth button up
	    حالت 10: واپس -1; // mouse outside of filter rectangle
	    طےشدہ: واپس -1; // should never occur
	}

	mouse_ستون = Pc - 1;
	mouse_صف = Pr - 1;

	*slen += (صحیح)(p - (tp + *slen));
    }
#اگر_ختم // FEAT_MOUSE_DEC
#اگر_تعریف_شدہ FEAT_MOUSE_PTERM
    اگر (کلید_name[0] == KS_PTERM_MOUSE)
    {
	صحیح button, عدد_clicks, action;

	p = tp + *slen;

	action = getdigits(&p);
	اگر (*p++ != ';')
	    واپس -1;

	mouse_صف = getdigits(&p);
	اگر (*p++ != ';')
	    واپس -1;
	mouse_ستون = getdigits(&p);
	اگر (*p++ != ';')
	    واپس -1;

	button = getdigits(&p);
	mouse_رمز = 0;

	انتخاب (button)
	{
	    حالت 4: mouse_رمز = MOUSE_بایاں; توڑو;
	    حالت 1: mouse_رمز = MOUSE_دایاں; توڑو;
	    حالت 2: mouse_رمز = MOUSE_درمیان; توڑو;
	    طےشدہ: واپس -1;
	}

	انتخاب (action)
	{
	    حالت 31: // Initial press
		اگر (*p++ != ';')
		    واپس -1;

		عدد_clicks = getdigits(&p); // Not used
		توڑو;

	    حالت 32: // Release
		is_release = TRUE;
		توڑو;

	    حالت 33: // Drag
		held_button = mouse_رمز;
		mouse_رمز |= MOUSE_DRAG;
		توڑو;

	    طےشدہ:
		واپس -1;
	}

	اگر (*p++ != 't')
	    واپس -1;

	*slen += (p - (tp + *slen));
    }
#اگر_ختم // FEAT_MOUSE_PTERM

    // Interpret the mouse code
    current_button = (mouse_رمز & MOUSE_CLICK_نقاب);
    اگر (is_release)
	current_button |= MOUSE_RELEASE;

    اگر (current_button == MOUSE_RELEASE
#اگر_تعریف_شدہ FEAT_MOUSE_XTERM
	    && wheel_رمز == 0
#اگر_ختم
       )
    {
	/*
	 * If we get a mouse drag or release event when there is no mouse
	 * button held down (held_button == MOUSE_RELEASE), produce a K_IGNORE
	 * below.
	 * (can happen when you hold down two buttons and then let them go, or
	 * click in the menu bar, but not on a menu, and drag into the text).
	 */
	اگر ((mouse_رمز & MOUSE_DRAG) == MOUSE_DRAG)
	    is_drag = TRUE;
	current_button = held_button;
    }
    ورنہ
    {
      اگر (wheel_رمز == 0)
      {
#اگر_تعریف_شدہ CHECK_DOUBLE_CLICK
# ifdef FEAT_MOUSE_GPM
	/*
	 * Only for Unix, when GUI not active, we handle multi-clicks here, but
	 * not for GPM mouse events.
	 */
#  ifdef FEAT_GUI
	اگر (کلید_name[0] != KS_GPM_MOUSE && !gui.اندر_use)
#  ورنہ
	    اگر (کلید_name[0] != KS_GPM_MOUSE)
#  endif
# ورنہ
#  ifdef FEAT_GUI
		اگر (!gui.اندر_use)
#  endif
# endif
		{
		    /*
		     * Compute the time elapsed since the previous mouse click.
		     */
		    gettimeofday(&mouse_وقت, تہی);
		    اگر (orig_mouse_وقت.tv_sec == 0)
		    {
			/*
			 * Avoid computing the difference between mouse_time
			 * and orig_mouse_time for the first click, as the
			 * difference would be huge and would cause
			 * multiplication overflow.
			 */
			timediff = p_mouset;
		    }
		    ورنہ
			timediff = وقت_diff_ms(&orig_mouse_وقت, &mouse_وقت);
		    orig_mouse_وقت = mouse_وقت;
		    اگر (mouse_رمز == orig_mouse_رمز
			    && timediff < p_mouset
			    && orig_عدد_clicks != 4
			    && orig_mouse_ستون == mouse_ستون
			    && orig_mouse_صف == mouse_صف
			    && (is_mouse_topline(curwin)
				// Double click in tab pages line also works
				// when window contents changes.
				|| (mouse_صف == 0 && firstwin->w_winrow > 0))
		       )
			++orig_عدد_clicks;
		    ورنہ
			orig_عدد_clicks = 1;
		    orig_mouse_ستون = mouse_ستون;
		    orig_mouse_صف = mouse_صف;
		    سیٹ_mouse_topline(curwin);
		}
# اگر defined(FEAT_GUI) || defined(FEAT_MOUSE_GPM)
		ورنہ
		    orig_عدد_clicks = عدد_MOUSE_CLICKS(mouse_رمز);
# endif
#ورنہ
	orig_عدد_clicks = عدد_MOUSE_CLICKS(mouse_رمز);
#اگر_ختم
	is_click = TRUE;
      }
      orig_mouse_رمز = mouse_رمز;
    }
    اگر (!is_drag)
	held_button = mouse_رمز & MOUSE_CLICK_نقاب;

    /*
     * Translate the actual mouse event into a pseudo mouse event.
     * First work out what modifiers are to be used.
     */
    اگر (orig_mouse_رمز & MOUSE_سرکاؤ)
	*modifiers |= MOD_نقاب_سرکاؤ;
    اگر (orig_mouse_رمز & MOUSE_CTRL)
	*modifiers |= MOD_نقاب_CTRL;
    اگر (orig_mouse_رمز & MOUSE_ALT)
	*modifiers |= MOD_نقاب_ALT;
    اگر (orig_عدد_clicks == 2)
	*modifiers |= MOD_نقاب_2_CLICK;
    ورنہ اگر (orig_عدد_clicks == 3)
	*modifiers |= MOD_نقاب_3_CLICK;
    ورنہ اگر (orig_عدد_clicks == 4)
	*modifiers |= MOD_نقاب_4_CLICK;

    // Work out our pseudo mouse event. Note that MOUSE_RELEASE gets added,
    // then it's not mouse up/down.
    کلید_name[0] = KS_EXTRA;
    اگر (wheel_رمز != 0 && (!is_release || release_is_ambiguous))
    {
	اگر (wheel_رمز & MOUSE_CTRL)
	    *modifiers |= MOD_نقاب_CTRL;
	اگر (wheel_رمز & MOUSE_ALT)
	    *modifiers |= MOD_نقاب_ALT;

	اگر (wheel_رمز & 1 && wheel_رمز & 2)
	    کلید_name[1] = (صحیح)KE_MOUSELEFT;
	ورنہ اگر (wheel_رمز & 2)
	    کلید_name[1] = (صحیح)KE_MOUSERIGHT;
	ورنہ اگر (wheel_رمز & 1)
	    کلید_name[1] = (صحیح)KE_MOUSEUP;
	ورنہ
	    کلید_name[1] = (صحیح)KE_MOUSEDOWN;

	held_button = MOUSE_RELEASE;
    }
    ورنہ
    {
#اگر defined(UNIX)
	اگر (use_xterm_mouse() && orig_mouse_رمز >= MOUSESIDEBUTTONS_کم)
	    current_button = (current_button) ? MOUSE_X2 : MOUSE_X1;
#اگر_ختم
	کلید_name[1] = get_pseudo_mouse_رمز(current_button, is_click, is_drag);
    }


    // Make sure the mouse position is valid.  Some terminals may return weird
    // values.
    اگر (mouse_ستون >= Columns)
	mouse_ستون = Columns - 1;
    اگر (mouse_صف >= صفیں)
	mouse_صف = صفیں - 1;

    واپس 0;
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
    صحیح		ستون = *colp;
    صحیح		صف = *rowp;
    linenr_T	lnum;
    صحیح		retval = FALSE;
    صحیح		off;
    صحیح		گنتی;

#اگر_تعریف_شدہ FEAT_RIGHTLEFT
    اگر (win->w_p_rl)
	ستون = win->w_چوڑائی - 1 - ستون;
#اگر_ختم

    lnum = win->w_topline;

    جب (صف > 0)
    {
	صحیح cache_idx = lnum - win->w_topline;

	// Only "Rows" lines are cached, with folding we'll run out of entries
	// and use the slow way.
	اگر (plines_cache != تہی && cache_idx < صفیں
						&& plines_cache[cache_idx] > 0)
	    گنتی = plines_cache[cache_idx];
	ورنہ
	{
#اگر_تعریف_شدہ FEAT_DIFF
	    // Don't include filler lines in "count"
	    اگر (win->w_p_diff
# ifdef FEAT_FOLDING
		    && !hasFoldingWin(win, lnum, تہی, تہی, TRUE, تہی)
# endif
		    )
	    {
		اگر (lnum == win->w_topline)
		    صف -= win->w_topfill;
		ورنہ
		    صف -= diff_check_fill(win, lnum);
		گنتی = plines_win_nofill(win, lnum, FALSE);
	    }
	    ورنہ
#اگر_ختم
		گنتی = plines_win(win, lnum, FALSE);
	    اگر (plines_cache != تہی && cache_idx < صفیں)
		plines_cache[cache_idx] = گنتی;
	}

	اگر (win->w_skipcol > 0 && lnum == win->w_topline)
	{
	    صحیح width1 = win->w_چوڑائی - win_ستون_off(win);

	    اگر (width1 > 0)
	    {
		صحیح skip_سطریں = 0;

		// Adjust for 'smoothscroll' clipping the top screen lines.
		// A similar formula is used in curs_columns().
		اگر (win->w_skipcol > width1)
		    skip_سطریں = (win->w_skipcol - width1)
					    / (width1 + win_ستون_off2(win)) + 1;
		ورنہ اگر (win->w_skipcol > 0)
		    skip_سطریں = 1;

		گنتی -= skip_سطریں;
	    }
	}

	اگر (گنتی > صف)
	    توڑو;	// Position is in this buffer line.
#اگر_تعریف_شدہ FEAT_FOLDING
	(خالی)hasFoldingWin(win, lnum, تہی, &lnum, TRUE, تہی);
#اگر_ختم
	اگر (lnum == win->w_بفر->b_ml.ml_سطر_گنتی)
	{
	    retval = TRUE;
	    توڑو;		// past end of file
	}
	صف -= گنتی;
	++lnum;
    }

    اگر (!retval)
    {
	// Compute the column without wrapping.
	off = win_ستون_off(win) - win_ستون_off2(win);
	اگر (ستون < off)
	    ستون = off;
	ستون += صف * (win->w_چوڑائی - off);

	// Add skip column for the topline.
	اگر (lnum == win->w_topline)
	    ستون += win->w_skipcol;
    }

    اگر (!win->w_p_wrap)
	ستون += win->w_leftcol;

    // skip line number and fold column in front of the line
    ستون -= win_ستون_off(win);
    اگر (ستون <= 0)
    {
#اگر_تعریف_شدہ FEAT_NETBEANS_INTG
	// if mouse is clicked on the gutter, then inform the netbeans server
	اگر (*colp < win_ستون_off(win))
	    netbeans_gutter_click(lnum);
#اگر_ختم
	ستون = 0;
    }

    *colp = ستون;
    *rowp = صف;
    *lnump = lnum;
    واپس retval;
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
mouse_ڈھونڈو_win(صحیح *rowp, صحیح *colp, mouse_ڈھونڈو_T popup غیر_مستعمل)
{
    چوکھٹا_T	*fp;
    win_T	*wp;

#اگر_تعریف_شدہ FEAT_PROP_POPUP
    win_T	*pwp = تہی;

    اگر (popup != IGNORE_POPUP)
    {
	popup_reset_handled(POPUP_HANDLED_1);
	جب ((wp = ڈھونڈو_اگلا_popup(TRUE, POPUP_HANDLED_1)) != تہی)
	{
	    اگر (*rowp >= wp->w_winrow && *rowp < wp->w_winrow + popup_اونچائی(wp)
		    && *colp >= wp->w_wincol
				    && *colp < wp->w_wincol + popup_چوڑائی(wp))
		pwp = wp;
	}
	اگر (pwp != تہی)
	{
	    اگر (popup == FAIL_POPUP)
		واپس تہی;
	    *rowp -= pwp->w_winrow;
	    *colp -= pwp->w_wincol;
	    واپس pwp;
	}
    }
#اگر_ختم

    fp = topframe;

    اگر (*colp < firstwin->w_wincol
	    || *colp >= firstwin->w_wincol + fp->fr_چوڑائی
	    || *rowp < firstwin->w_winrow)
	واپس تہی;

    *rowp -= firstwin->w_winrow;
    *colp -= firstwin->w_wincol;
    برائے (;;)
    {
	اگر (fp->fr_layout == FR_برگ)
	    توڑو;
	اگر (fp->fr_layout == FR_صف)
	{
	    برائے (fp = fp->fr_بچہ; fp->fr_اگلا != تہی; fp = fp->fr_اگلا)
	    {
		اگر (*colp < fp->fr_چوڑائی)
		    توڑو;
		*colp -= fp->fr_چوڑائی;
	    }
	}
	ورنہ    // fr_layout == FR_COL
	{
	    برائے (fp = fp->fr_بچہ; fp->fr_اگلا != تہی; fp = fp->fr_اگلا)
	    {
		اگر (*rowp < fp->fr_اونچائی)
		    توڑو;
		*rowp -= fp->fr_اونچائی;
	    }
	}
    }
    // When using a timer that closes a window the window might not actually
    // exist.
    FOR_ALL_WINDOWS(wp)
	اگر (wp == fp->fr_win)
	{
#اگر_تعریف_شدہ FEAT_MENU
	    *rowp -= wp->w_winbar_اونچائی;
#اگر_ختم
	    واپس wp;
	}
    واپس تہی;
}

#اگر defined(NEED_VCOL2_ستون) || defined(FEAT_BEVAL) || defined(FEAT_PROP_POPUP) \
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
    سطر = ml_get_buf(wp->w_بفر, lnum, FALSE);
    init_chartabsize_arg(&cts, wp, lnum, 0, سطر, سطر);
    جب (cts.cts_vcol < vcol && *cts.cts_ptr != NUL)
    {
	صحیح سائز = win_lbr_chartabsize(&cts, تہی, تہی);
	اگر (cts.cts_vcol + سائز > vcol)
	    توڑو;
	cts.cts_vcol += سائز;
	MB_PTR_ADV(cts.cts_ptr);
    }
    clear_chartabsize_arg(&cts);

    اگر (coladdp != تہی)
	*coladdp = vcol - cts.cts_vcol;
    واپس (صحیح)(cts.cts_ptr - سطر);
}
#اگر_ختم

#اگر defined(FEAT_EVAL)
/*
 * "getmousepos()" function.
 */
    خالی
f_getmousepos(typval_T *argvars غیر_مستعمل, typval_T *rettv)
{
    dict_T	*d;
    win_T	*wp;
    صحیح		صف = mouse_صف;
    صحیح		ستون = mouse_ستون;
    varnumber_T winid = 0;
    varnumber_T winrow = 0;
    varnumber_T wincol = 0;
    linenr_T	lnum = 0;
    varnumber_T عمود = 0;
    colnr_T	coladd = 0;

    اگر (rettv_dict_alloc(rettv) == FAIL)
	واپس;
    d = rettv->vval.v_dict;

    dict_جوڑو_نمبر(d, "screenrow", (varnumber_T)mouse_صف + 1);
    dict_جوڑو_نمبر(d, "screencol", (varnumber_T)mouse_ستون + 1);

    wp = mouse_ڈھونڈو_win(&صف, &ستون, ڈھونڈو_POPUP);
    اگر (wp != تہی)
    {
	صحیح	چوٹی_off = 0;
	صحیح	بایاں_off = 0;
	صحیح	اونچائی = wp->w_اونچائی + wp->w_حالت_اونچائی;

# ifdef FEAT_PROP_POPUP
	اگر (WIN_IS_POPUP(wp))
	{
	    چوٹی_off = popup_چوٹی_extra(wp);
	    بایاں_off = popup_بایاں_extra(wp);
	    اونچائی = popup_اونچائی(wp);
	}
# endif
	اگر (صف < اونچائی)
	{
	    winid = wp->w_id;
	    winrow = صف + 1;
	    wincol = ستون + 1;
	    صف -= چوٹی_off;
	    ستون -= بایاں_off;
	    اگر (صف >= 0 && صف < wp->w_اونچائی && ستون >= 0 && ستون < wp->w_چوڑائی)
	    {
		(خالی)mouse_comp_pos(wp, &صف, &ستون, &lnum, تہی);
		ستون = vcol2col(wp, lnum, ستون, &coladd);
		عمود = ستون + 1;
	    }
	}
    }
    dict_جوڑو_نمبر(d, "winid", winid);
    dict_جوڑو_نمبر(d, "winrow", winrow);
    dict_جوڑو_نمبر(d, "wincol", wincol);
    dict_جوڑو_نمبر(d, "line", (varnumber_T)lnum);
    dict_جوڑو_نمبر(d, "column", عمود);
    dict_جوڑو_نمبر(d, "coladd", coladd);
}
#اگر_ختم
