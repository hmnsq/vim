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
ساكن دراز mouse_hor_گام = 6;
ساكن دراز mouse_vert_گام = 3;
ساكن win_T *dragwin = عدم;	// window being dragged
ساكن صحيح stl_click_handler(win_T *wp, صحيح mrow, صحيح mcol, صحيح which_button,
								    صحيح mods);
ساكن صحيح stl_click_handler_regions(stl_click_region_T *regions,
				    صحيح region_شمار, صحيح winid,
				    char_u *area_name, صحيح mrow, صحيح mcol,
				    صحيح which_button, صحيح mods);

    فراغ
mouse_مجموعه_vert_scroll_گام(دراز گام)
{
    mouse_vert_گام = گام;
}

    فراغ
mouse_مجموعه_hor_scroll_گام(دراز گام)
{
    mouse_hor_گام = گام;
}

#اگر_تعریف_شده CHECK_DOUBLE_CLICK
/*
 * Return the duration from t1 to t2 in milliseconds.
 */
    ساكن دراز
زمان_diff_ms(بنية timeval *t1, بنية timeval *t2)
{
    // This handles wrapping of tv_usec correctly without any special case.
    // Example of 2 pairs (tv_sec, tv_usec) with a duration of 5 ms:
    //	   t1 = (1, 998000) t2 = (2, 3000) gives:
    //	   (2 - 1) * 1000 + (3000 - 998000) / 1000 -> 5 ms.
    أرجع (t2->tv_sec - t1->tv_sec) * 1000
	 + (t2->tv_usec - t1->tv_usec) / 1000;
}
#پایان_اگر

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
پیدا_کن_شروع_از_کلمه(pos_T *pos)
{
    char_u	*سطر;
    صحيح		cclass;
    صحيح		ستون;

    سطر = ml_get(pos->lnum);
    cclass = get_mouse_class(سطر + pos->ستون);

    طالما (pos->ستون > 0)
    {
	ستون = pos->ستون - 1;
	ستون -= (*mb_سر_off)(سطر, سطر + ستون);
	إذا (get_mouse_class(سطر + ستون) != cclass)
	    اكسر;
	pos->ستون = ستون;
    }
}

/*
 * Move "pos" forward to the end of the word it's in.
 * When 'selection' is "exclusive", the position is just after the word.
 */
    ساكن فراغ
پیدا_کن_انتها_از_کلمه(pos_T *pos)
{
    char_u	*سطر;
    صحيح		cclass;
    صحيح		ستون;

    سطر = ml_get(pos->lnum);
    إذا (*p_sel == 'e' && pos->ستون > 0)
    {
	--pos->ستون;
	pos->ستون -= (*mb_سر_off)(سطر, سطر + pos->ستون);
    }
    cclass = get_mouse_class(سطر + pos->ستون);
    طالما (سطر[pos->ستون] != NUL)
    {
	ستون = pos->ستون + (*mb_ptr2len)(سطر + pos->ستون);
	إذا (get_mouse_class(سطر + ستون) != cclass)
	{
	    إذا (*p_sel == 'e')
		pos->ستون = ستون;
	    اكسر;
	}
	pos->ستون = ستون;
    }
}

#إذا defined(FEAT_GUI_MOTIF) || defined(FEAT_GUI_GTK) \
	    || defined(FEAT_GUI_MSWIN) \
	    || defined(FEAT_GUI_PHOTON) \
	    || defined(FEAT_جمله_POPUP_MENU)
# define USE_POPUP_SETPOS
# define NEED_VCOL2_ستون

/*
 * Translate window coordinates to buffer position without any side effects.
 * Returns IN_BUFFER and sets "mpos->col" to the column when in buffer text.
 * The column is one for the first column.
 */
    ساكن صحيح
get_fpos_از_mouse(pos_T *mpos)
{
    win_T	*wp;
    صحيح		ردیف = mouse_ردیف;
    صحيح		ستون = mouse_ستون;

    إذا (ردیف < 0 || ستون < 0)		// check if it makes sense
	أرجع درون_UNKNOWN;

    // find the window where the row is in
    wp = mouse_پیدا_کن_win(&ردیف, &ستون, FAIL_POPUP);
    إذا (wp == عدم)
	أرجع درون_UNKNOWN;
    // winpos and height may change in win_enter()!
    إذا (ردیف >= wp->w_ارتفاع)	// In (or below) status line
	أرجع درون_وضعیت_سطر;
    إذا (ستون >= wp->w_عرض)	// In vertical separator line
	أرجع درون_SEP_سطر;

    إذا (wp != curwin)
	أرجع درون_UNKNOWN;

    // compute the position in the buffer line from the posn on the screen
    إذا (mouse_comp_pos(curwin, &ردیف, &ستون, &mpos->lnum, عدم))
	أرجع درون_وضعیت_سطر; // past bottom

    mpos->ستون = vcol2col(wp, mpos->lnum, ستون, &mpos->coladd);
    أرجع درون_میانگیر;
}
#پایان_اگر

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
    دراز	شمار,
    صحيح		fixindent)	// PUT_FIXINDENT if fixing indent necessary
{
    ساكن صحيح	do_always = FALSE;	// ignore 'mouse' setting next time
    ساكن صحيح	got_click = FALSE;	// got a click some time back

    صحيح		which_button;	// MOUSE_LEFT, _MIDDLE or _RIGHT
    صحيح		is_click = FALSE; // If FALSE it's a drag or release event
    صحيح		is_drag = FALSE;  // If TRUE it's a drag event
    صحيح		jump_flags = 0;	// flags for jump_to_mouse()
    pos_T	شروع_visual;
    صحيح		moved;		// Has cursor moved?
    صحيح		درون_وضعیت_سطر_2;	// mouse in status line
    ساكن صحيح	درون_tab_سطر = FALSE; // mouse clicked in tab line
    ساكن صحيح	درون_tabpanel = FALSE; // mouse clicked in tabpanel
#اگر_تعریف_شده FEAT_TABPANEL
    ساكن bool	درون_tabpanel_scrollbar = false; // dragging tabpanel scrollbar
#پایان_اگر
    صحيح		درون_sep_سطر;	// mouse in vertical separator line
    صحيح		c1, c2;
#إذا defined(FEAT_FOLDING)
    pos_T	save_cursor;
#پایان_اگر
    win_T	*old_curwin = curwin;
    ساكن pos_T orig_cursor;
    colnr_T	leftcol, rightcol;
    pos_T	انتها_visual;
    صحيح		diff;
    صحيح		old_فعال = VIsual_فعال;
    صحيح		old_نما = VIsual_نما;
    صحيح		regname;

#إذا defined(FEAT_FOLDING)
    save_cursor = curwin->w_cursor;
#پایان_اگر

    // When GUI is active, always recognize mouse events, otherwise:
    // - Ignore mouse event in normal mode if 'mouse' doesn't include 'n'.
    // - Ignore mouse event in visual mode if 'mouse' doesn't include 'v'.
    // - For command line and insert mode 'mouse' is checked before calling
    //	 do_mouse().
    إذا (do_always)
	do_always = FALSE;
    وإلا
#اگر_تعریف_شده FEAT_GUI
	إذا (!gui.درون_use)
#پایان_اگر
	{
	    إذا (VIsual_فعال)
	    {
		إذا (!mouse_has(MOUSE_VISUAL))
		    أرجع FALSE;
	    }
	    وإلا إذا (State == نما_نرمال && !mouse_has(MOUSE_نرمال))
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
	    إذا (!کلید_Stuffed && vpeekc() != NUL)
	    {
		صحيح nc;
		صحيح save_mouse_ردیف = mouse_ردیف;
		صحيح save_mouse_ستون = mouse_ستون;

		// Need to get the character, peeking doesn't get the actual
		// one.
		nc = امن_vgetc();
		إذا (c == nc)
		    استمر;
		vungetc(nc);
		mouse_ردیف = save_mouse_ردیف;
		mouse_ستون = save_mouse_ستون;
	    }
	}
	اكسر;
    }

    إذا (c == K_MOUSEMOVE)
    {
	// Mouse moved without a button pressed.
#اگر_تعریف_شده FEAT_BEVAL_جمله
	ui_may_remove_balloon();
	إذا (p_bevalterm)
	{
	    profile_setlimit(p_bdlay, &bevalexpr_due);
	    bevalexpr_due_مجموعه = TRUE;
	}
#پایان_اگر
#اگر_تعریف_شده FEAT_PROP_POPUP
	popup_handle_mouse_moved();
#پایان_اگر
	أرجع FALSE;
    }

#اگر_تعریف_شده FEAT_MOUSESHAPE
    // May have stopped dragging the status or separator line.  The pointer is
    // most likely still on the status or separator line.
    إذا (!is_drag && drag_وضعیت_سطر)
    {
	drag_وضعیت_سطر = FALSE;
	update_mouseshape(SHAPE_IDX_وضعیت);
    }
    إذا (!is_drag && drag_sep_سطر)
    {
	drag_sep_سطر = FALSE;
	update_mouseshape(SHAPE_IDX_VSEP);
    }
#پایان_اگر

    // Ignore drag and release events if we didn't get a click.
    إذا (is_click)
    {
	got_click = TRUE;
	درون_tab_سطر = FALSE;
	درون_tabpanel = FALSE;
#اگر_تعریف_شده FEAT_TABPANEL
	درون_tabpanel_scrollbar = false;
#پایان_اگر
    }
    وإلا
    {
	إذا (!got_click)			// didn't get click, ignore
	    أرجع FALSE;
	إذا (!is_drag)			// release, reset got_click
	{
	    got_click = FALSE;
	    إذا (درون_tab_سطر || درون_tabpanel
#اگر_تعریف_شده FEAT_TABPANEL
		    || درون_tabpanel_scrollbar
#پایان_اگر
		    )
	    {
		درون_tab_سطر = FALSE;
		درون_tabpanel = FALSE;
#اگر_تعریف_شده FEAT_TABPANEL
		درون_tabpanel_scrollbar = false;
#پایان_اگر
		أرجع FALSE;
	    }
	}
    }

#اگر_تعریف_شده FEAT_TABPANEL
    // Continue a scrollbar drag before any tab-selection handling.
    إذا (is_drag && درون_tabpanel_scrollbar)
    {
	tabpanel_drag_scrollbar(mouse_ردیف);
	أرجع FALSE;
    }
#پایان_اگر

    // CTRL right mouse button does CTRL-T
    إذا (is_click && (mod_ماسک & MOD_ماسک_CTRL) && which_button == MOUSE_راست)
    {
	إذا (State & نما_درج)
	    stuffcharReadbuff(Ctrl_O);
	إذا (شمار > 1)
	    stuffnumReadbuff(شمار);
	stuffcharReadbuff(Ctrl_T);
	got_click = FALSE;		// ignore drag&release now
	أرجع FALSE;
    }

    // CTRL only works with left mouse button
    إذا ((mod_ماسک & MOD_ماسک_CTRL) && which_button != MOUSE_چپ)
	أرجع FALSE;

    // When a modifier is down, ignore drag and release events, as well as
    // multiple clicks and the middle mouse button.
    // Accept shift-leftmouse drags when 'mousemodel' is "popup.*".
    إذا ((mod_ماسک & (MOD_ماسک_لغزش | MOD_ماسک_CTRL | MOD_ماسک_ALT
							     | MOD_ماسک_META))
	    && (!is_click
		|| (mod_ماسک & MOD_ماسک_MULTI_CLICK)
		|| which_button == MOUSE_میانه)
	    && !((mod_ماسک & (MOD_ماسک_لغزش|MOD_ماسک_ALT))
		&& mouse_model_popup()
		&& which_button == MOUSE_چپ)
	    && !((mod_ماسک & MOD_ماسک_ALT)
		&& !mouse_model_popup()
		&& which_button == MOUSE_راست)
	    )
	أرجع FALSE;

    // If the button press was used as the movement command for an operator
    // (eg "d<MOUSE>"), or it is the middle button that is held down, ignore
    // drag/release events.
    إذا (!is_click && which_button == MOUSE_میانه)
	أرجع FALSE;

    إذا (oap != عدم)
	regname = oap->regname;
    وإلا
	regname = 0;

    // Middle mouse button does a 'put' of the selected text
    إذا (which_button == MOUSE_میانه)
    {
	إذا (State == نما_نرمال)
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
	    إذا (VIsual_فعال)
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

	وإلا إذا ((State & نما_درج) == 0)
	    أرجع FALSE;

	// Middle click in insert mode doesn't move the mouse, just insert the
	// contents of a register.  '.' register is special, can't insert that
	// with do_put().
	// Also paste at the cursor if the current mode isn't in 'mouse' (only
	// happens for the GUI).
	إذا ((State & نما_درج) || !mouse_has(MOUSE_نرمال))
	{
	    إذا (regname == '.')
		درج_reg(regname, TRUE);
	    وإلا
	    {
#اگر_تعریف_شده FEAT_CLIPBOARD
		إذا (clip_ستاره.موجود && regname == 0)
		    regname = '*';
#پایان_اگر
		إذا ((State & REPLACE_FLAG) && !yank_ثبات_mline(regname))
		    درج_reg(regname, TRUE);
		وإلا
		{
		    do_put(regname, عدم, BACKWARD, 1L,
						      fixindent | PUT_CURSEND);

		    // Repeat it with CTRL-R CTRL-O r or CTRL-R CTRL-P r
		    Append_Char_مقصد_Redobuff(Ctrl_R);
		    Append_Char_مقصد_Redobuff(fixindent ? Ctrl_P : Ctrl_O);
		    Append_Char_مقصد_Redobuff(regname == 0 ? '"' : regname);
		}
	    }
	    أرجع FALSE;
	}
    }

    // When dragging or button-up stay in the same window.
    إذا (!is_click)
	jump_flags |= MOUSE_FOCUS | MOUSE_DID_MOVE;

    شروع_visual.lnum = 0;

    بنية tabpage_label_info {
	bool is_panel;	    // label type. true: tabpanel, false: tab line
	bool just_درون;	    // just in tabpage label area
	bool just_click;    // just click tabpage label area
	صحيح nr;		    // tabpage number
    } tp_label = { false, false, false, 0 };

    // Check for clicking in the tab page panel.
#إذا defined(FEAT_TABPANEL)
    إذا (mouse_ستون < firstwin->w_wincol
		|| mouse_ستون >= firstwin->w_wincol + topframe->fr_عرض)
    {
	// A click on the scrollbar column starts a drag interaction and
	// preempts tab-selection.
	إذا (is_click && !is_drag && mouse_on_tabpanel_scrollbar())
	{
	    درون_tabpanel_scrollbar = TRUE;
	    tabpanel_drag_scrollbar(mouse_ردیف);
	    أرجع FALSE;
	}

	// Dispatch 'tabpanel' %[FuncName] click regions before falling through
	// to tab-page selection.  On drag events fall through to the normal
	// tab-drag handling.
	إذا (is_click && !is_drag
		&& stl_click_handler_regions(tabpanel_stl_click,
					    tabpanel_stl_click_شمار,
					    0, (char_u *)"tabpanel",
					    mouse_ردیف, mouse_ستون,
					    which_button, mod_ماسک))
	    أرجع FALSE;

	tp_label.is_panel = true;
	tp_label.just_درون = true;
	tp_label.nr = get_tabpagenr_on_tabpanel();

	// click in a tab selects that tab page
	إذا (is_click && cmdwin_type == 0)
	    tp_label.just_click = true;
    }
    وإلا
#پایان_اگر
    // Check for clicking in the tab page line.
    إذا (Tab_صفحه_Idxs != عدم && mouse_ردیف == 0 && firstwin->w_winrow > 0)
    {
	// Dispatch 'tabline' %[FuncName] click regions before falling through
	// to tab-page selection.  On drag events fall through to the normal
	// tab-drag handling.
	إذا (is_click && !is_drag
		&& stl_click_handler_regions(tabline_stl_click,
					    tabline_stl_click_شمار,
					    0, (char_u *)"tabline",
					    mouse_ردیف, mouse_ستون,
					    which_button, mod_ماسک))
	    أرجع FALSE;

	tp_label.just_درون = true;
	tp_label.nr = Tab_صفحه_Idxs[mouse_ستون];

	// click in a tab selects that tab page
	إذا (is_click && cmdwin_type == 0
		&& mouse_ستون < firstwin->w_wincol + topframe->fr_عرض)
	    tp_label.just_click = true;
    }

    إذا (tp_label.just_درون)
    {
	إذا (is_drag)
	{
	    إذا (درون_tabpanel || درون_tab_سطر)
	    {
		c1 = tp_label.nr;
		tabpage_move(c1 <= 0 ? 9999 : c1 < tabpage_شاخص(curtab)
							    ? c1 - 1 : c1);
	    }
	    أرجع FALSE;
	}

	إذا (tp_label.just_click)
	{
	    إذا (tp_label.is_panel)
		درون_tabpanel = TRUE;
	    وإلا
		درون_tab_سطر = TRUE;
	    c1 = tp_label.nr;
	    إذا (c1 >= 0)
	    {
		إذا ((mod_ماسک & MOD_ماسک_MULTI_CLICK) == MOD_ماسک_2_CLICK
						    && !tp_label.is_panel)
		{
		    // Double-click on the tabline opens a new, empty tab page.
		    // The tabpanel has no "empty area" (every row maps to a tab)
		    // and this behavior is not documented for tabpanel, so fall
		    // through to the regular tab-switch path there.
		    انتها_visual_نما_keep_button();
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
			انتها_visual_نما_keep_button();
		}
	    }
	    وإلا
	    {
		tabpage_T	*tp;

		// Close the current or specified tab page.
		إذا (c1 == -999)
		    tp = curtab;
		وإلا
		    tp = پیدا_کن_tabpage(-c1);
		إذا (tp == curtab)
		{
		    إذا (اولی_tabpage->tp_بعدی != عدم)
			tabpage_close(FALSE);
		}
		وإلا إذا (tp != عدم)
		    tabpage_close_other(tp, FALSE);
	    }
	}
	أرجع TRUE;
    }
    وإلا إذا (is_drag && (درون_tabpanel || (درون_tab_سطر && Tab_صفحه_Idxs != عدم)))
    {
#إذا defined(FEAT_TABPANEL)
	إذا (درون_tabpanel)
	    c1 = get_tabpagenr_on_tabpanel();
	وإلا
#پایان_اگر
	    c1 = Tab_صفحه_Idxs[mouse_ستون];
	tabpage_move(c1 <= 0 ? 9999 : c1 - 1);
	أرجع FALSE;
    }

    // When 'mousemodel' is "popup" or "popup_setpos", translate mouse events:
    // right button up   -> pop-up menu
    // shift-left button -> right button
    // alt-left button   -> alt-right button
    إذا (mouse_model_popup())
    {
	إذا (which_button == MOUSE_راست
			    && !(mod_ماسک & (MOD_ماسک_لغزش | MOD_ماسک_CTRL)))
	{
#اگر_تعریف_شده USE_POPUP_SETPOS
# ifdef FEAT_GUI
	    إذا (gui.درون_use)
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
# إذا defined(FEAT_GUI) && defined(FEAT_جمله_POPUP_MENU)
	    وإلا
# endif
# إذا defined(FEAT_جمله_POPUP_MENU)
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
		إذا (VIsual_فعال)
		{
		    pos_T    m_pos;

		    // set MOUSE_MAY_STOP_VIS if we are outside the
		    // selection or the current window (might have false
		    // negative here)
		    إذا (mouse_ردیف < curwin->w_winrow
			 || mouse_ردیف
				  > (curwin->w_winrow + curwin->w_ارتفاع))
			jump_flags = MOUSE_MAY_STOP_VIS;
		    وإلا إذا (get_fpos_از_mouse(&m_pos) != درون_میانگیر)
			jump_flags = MOUSE_MAY_STOP_VIS;
		    وإلا
		    {
			إذا (VIsual_نما == 'V')
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
			وإلا إذا (VIsual_نما == Ctrl_V)
			{
			    getvcols(curwin, &curwin->w_cursor, &VIsual,
						     &leftcol, &rightcol, 0);
			    getvcol(curwin, &m_pos, عدم, &m_pos.ستون, عدم, 0);
			    إذا (m_pos.ستون < leftcol || m_pos.ستون > rightcol)
				jump_flags = MOUSE_MAY_STOP_VIS;
			}
		    }
		}
		وإلا
		    jump_flags = MOUSE_MAY_STOP_VIS;
	    }
	    إذا (jump_flags)
	    {
		jump_flags = jump_مقصد_mouse(jump_flags, عدم, which_button);
		update_curbuf(VIsual_فعال ? UPD_INVERTED : UPD_VALID);
		setcursor();
		بیرون_flush();    // Update before showing popup menu
	    }
# ifdef FEAT_MENU
	    show_popupmenu();
	    got_click = FALSE;	// ignore release events
# endif
	    أرجع (jump_flags & CURSOR_MOVED) != 0;
#وإلا
	    أرجع FALSE;
#پایان_اگر
	}
	إذا (which_button == MOUSE_چپ
				&& (mod_ماسک & (MOD_ماسک_لغزش|MOD_ماسک_ALT)))
	{
	    which_button = MOUSE_راست;
	    mod_ماسک &= ~MOD_ماسک_لغزش;
	}
    }

    إذا ((State & (نما_نرمال | نما_درج))
			    && !(mod_ماسک & (MOD_ماسک_لغزش | MOD_ماسک_CTRL)))
    {
	إذا (which_button == MOUSE_چپ)
	{
	    إذا (is_click)
	    {
		// stop Visual mode for a left click in a window, but not when
		// on a status line
		إذا (VIsual_فعال)
		    jump_flags |= MOUSE_MAY_STOP_VIS;
	    }
	    وإلا إذا (mouse_has(MOUSE_VISUAL))
		jump_flags |= MOUSE_MAY_VIS;
	}
	وإلا إذا (which_button == MOUSE_راست)
	{
	    إذا (is_click && VIsual_فعال)
	    {
		// Remember the start and end of visual before moving the
		// cursor.
		إذا (LT_POS(curwin->w_cursor, VIsual))
		{
		    شروع_visual = curwin->w_cursor;
		    انتها_visual = VIsual;
		}
		وإلا
		{
		    شروع_visual = VIsual;
		    انتها_visual = curwin->w_cursor;
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
    jump_flags = jump_مقصد_mouse(jump_flags,
			oap == عدم ? عدم : &(oap->inclusive), which_button);

#اگر_تعریف_شده FEAT_MENU
    // A click in the window toolbar has no side effects.
    إذا (jump_flags & MOUSE_WINBAR)
	أرجع FALSE;
#پایان_اگر
    moved = (jump_flags & CURSOR_MOVED);
    درون_وضعیت_سطر_2 = (jump_flags & درون_وضعیت_سطر);
    درون_sep_سطر = (jump_flags & درون_SEP_سطر);

    // Check for statusline click handler early, before visual mode or
    // other button-specific handling can interfere.
    إذا (درون_وضعیت_سطر_2 && is_click && !is_drag
	    && stl_click_handler(dragwin, mouse_ردیف, mouse_ستون,
						which_button, mod_ماسک))
    {
#اگر_تعریف_شده FEAT_MOUSESHAPE
	إذا (!drag_وضعیت_سطر)
	{
	    drag_وضعیت_سطر = TRUE;
	    update_mouseshape(-1);
	}
#پایان_اگر
	أرجع FALSE;
    }

#اگر_تعریف_شده FEAT_NETBEANS_INTG
    إذا (is_Netbeans_میانگیر(curbuf)
			    && !(jump_flags & (درون_وضعیت_سطر | درون_SEP_سطر)))
    {
	صحيح کلید = KEY2TERMCAP1(c);

	إذا (کلید == (صحيح)KE_LEFTRELEASE || کلید == (صحيح)KE_MIDDLERELEASE
					       || کلید == (صحيح)KE_RIGHTRELEASE)
	    netbeans_button_release(which_button);
    }
#پایان_اگر

    // When jumping to another window, clear a pending operator.  That's a bit
    // friendlier than beeping and not jumping to that window.
    إذا (curwin != old_curwin && oap != عدم && oap->op_type != OP_NOP)
	clearop(oap);

#اگر_تعریف_شده FEAT_FOLDING
    إذا (mod_ماسک == 0
	    && !is_drag
	    && (jump_flags & (MOUSE_FOLD_CLOSE | MOUSE_FOLD_OPEN))
	    && which_button == MOUSE_چپ)
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
#پایان_اگر

#إذا defined(FEAT_CLIPBOARD)
    إذا ((jump_flags & درون_OTHER_WIN) && !VIsual_فعال &&
	    (clip_ستاره.موجود || clip_plus.موجود))
    {
	clip_modeless(which_button, is_click, is_drag);
	أرجع FALSE;
    }
#پایان_اگر

    // Set global flag that we are extending the Visual area with mouse
    // dragging; temporarily minimize 'scrolloff'.
    إذا (VIsual_فعال && is_drag && get_scrolloff_مقدار())
    {
	// In the very first line, allow scrolling one line
	إذا (mouse_ردیف == 0)
	    mouse_dragging = 2;
	وإلا
	    mouse_dragging = 1;
    }

    // When dragging the mouse above the window, scroll down.
    إذا (is_drag && mouse_ردیف < 0 && !درون_وضعیت_سطر_2)
    {
	scroll_redraw(FALSE, 1L);
	mouse_ردیف = 0;
    }

    إذا (شروع_visual.lnum)		// right click in visual mode
    {
       // When ALT is pressed make Visual mode blockwise.
       إذا (mod_ماسک & MOD_ماسک_ALT)
	   VIsual_نما = Ctrl_V;

	// In Visual-block mode, divide the area in four, pick up the corner
	// that is in the quarter that the cursor is in.
	إذا (VIsual_نما == Ctrl_V)
	{
	    getvcols(curwin, &شروع_visual, &انتها_visual,
						    &leftcol, &rightcol, 0);
	    إذا (curwin->w_curswant > (leftcol + rightcol) / 2)
		انتها_visual.ستون = leftcol;
	    وإلا
		انتها_visual.ستون = rightcol;
	    إذا (curwin->w_cursor.lnum >=
				    (شروع_visual.lnum + انتها_visual.lnum) / 2)
		انتها_visual.lnum = شروع_visual.lnum;

	    // move VIsual to the right column
	    شروع_visual = curwin->w_cursor;	    // save the cursor pos
	    curwin->w_cursor = انتها_visual;
	    coladvance(انتها_visual.ستون);
	    VIsual = curwin->w_cursor;
	    curwin->w_cursor = شروع_visual;	    // restore the cursor
	}
	وإلا
	{
	    // If the click is before the start of visual, change the start.
	    // If the click is after the end of visual, change the end.  If
	    // the click is inside the visual, change the closest side.
	    إذا (LT_POS(curwin->w_cursor, شروع_visual))
		VIsual = انتها_visual;
	    وإلا إذا (LT_POS(انتها_visual, curwin->w_cursor))
		VIsual = شروع_visual;
	    وإلا
	    {
		// In the same line, compare column number
		إذا (انتها_visual.lnum == شروع_visual.lnum)
		{
		    إذا (curwin->w_cursor.ستون - شروع_visual.ستون >
				    انتها_visual.ستون - curwin->w_cursor.ستون)
			VIsual = شروع_visual;
		    وإلا
			VIsual = انتها_visual;
		}

		// In different lines, compare line number
		وإلا
		{
		    diff = (curwin->w_cursor.lnum - شروع_visual.lnum) -
				(انتها_visual.lnum - curwin->w_cursor.lnum);

		    إذا (diff > 0)		// closest to end
			VIsual = شروع_visual;
		    وإلا إذا (diff < 0)	// closest to start
			VIsual = انتها_visual;
		    وإلا			// in the middle line
		    {
			إذا (curwin->w_cursor.ستون <
					(شروع_visual.ستون + انتها_visual.ستون) / 2)
			    VIsual = انتها_visual;
			وإلا
			    VIsual = شروع_visual;
		    }
		}
	    }
	}
    }
    // If Visual mode started in insert mode, execute "CTRL-O"
    وإلا إذا ((State & نما_درج) && VIsual_فعال)
	stuffcharReadbuff(Ctrl_O);

    // Middle mouse click: Put text before cursor.
    إذا (which_button == MOUSE_میانه)
    {
#اگر_تعریف_شده FEAT_CLIPBOARD
	إذا (clip_ستاره.موجود && regname == 0)
	    regname = '*';
#پایان_اگر
	إذا (yank_ثبات_mline(regname))
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
	prep_redo(regname, شمار, NUL, c1, NUL, c2, NUL);

	// Remember where the paste started, so in edit() Insstart can be set
	// to this position
	إذا (restart_edit != 0)
	    where_paste_started = curwin->w_cursor;
	do_put(regname, عدم, dir, شمار, fixindent | PUT_CURSEND);
    }

#إذا defined(FEAT_QUICKFIX)
    // Ctrl-Mouse click or double click in a quickfix window jumps to the
    // error under the mouse pointer.
    وإلا إذا (((mod_ماسک & MOD_ماسک_CTRL)
		|| (mod_ماسک & MOD_ماسک_MULTI_CLICK) == MOD_ماسک_2_CLICK)
	    && bt_quickfix(curbuf))
    {
	إذا (curwin->w_llist_ref == عدم)	// quickfix window
	    do_cmdline_cmd((char_u *)".cc");
	وإلا					// location list window
	    do_cmdline_cmd((char_u *)".ll");
	got_click = FALSE;		// ignore drag&release now
    }
#پایان_اگر

    // Ctrl-Mouse click (or double click in a help window) jumps to the tag
    // under the mouse pointer.
    وإلا إذا ((mod_ماسک & MOD_ماسک_CTRL) || (curbuf->b_help
		     && (mod_ماسک & MOD_ماسک_MULTI_CLICK) == MOD_ماسک_2_CLICK))
    {
	إذا (State & نما_درج)
	    stuffcharReadbuff(Ctrl_O);
	stuffcharReadbuff(Ctrl_RSB);
	got_click = FALSE;		// ignore drag&release now
    }

    // Shift-Mouse click searches for the next occurrence of the word under
    // the mouse pointer
    وإلا إذا ((mod_ماسک & MOD_ماسک_لغزش))
    {
	إذا ((State & نما_درج) || (VIsual_فعال && VIsual_select))
	    stuffcharReadbuff(Ctrl_O);
	إذا (which_button == MOUSE_چپ)
	    stuffcharReadbuff('*');
	وإلا	// MOUSE_RIGHT
	    stuffcharReadbuff('#');
    }

    // Handle double clicks, unless on status line
    وإلا إذا (درون_وضعیت_سطر_2)
    {
#اگر_تعریف_شده FEAT_MOUSESHAPE
	إذا ((is_drag || is_click) && !drag_وضعیت_سطر)
	{
	    drag_وضعیت_سطر = TRUE;
	    update_mouseshape(-1);
	}
#پایان_اگر
    }
    وإلا إذا (درون_sep_سطر)
    {
#اگر_تعریف_شده FEAT_MOUSESHAPE
	إذا ((is_drag || is_click) && !drag_sep_سطر)
	{
	    drag_sep_سطر = TRUE;
	    update_mouseshape(-1);
	}
#پایان_اگر
    }
    وإلا إذا ((mod_ماسک & MOD_ماسک_MULTI_CLICK)
				       && (State & (نما_نرمال | نما_درج))
	     && mouse_has(MOUSE_VISUAL))
    {
	إذا (is_click || !VIsual_فعال)
	{
	    إذا (VIsual_فعال)
		orig_cursor = VIsual;
	    وإلا
	    {
		check_visual_highlight();
		VIsual = curwin->w_cursor;
		orig_cursor = VIsual;
		VIsual_فعال = TRUE;
		VIsual_reselect = TRUE;
		// start Select mode if 'selectmode' contains "mouse"
		may_شروع_select('o');
		setmouse();
	    }
	    إذا ((mod_ماسک & MOD_ماسک_MULTI_CLICK) == MOD_ماسک_2_CLICK)
	    {
		// Double click with ALT pressed makes it blockwise.
		إذا (mod_ماسک & MOD_ماسک_ALT)
		    VIsual_نما = Ctrl_V;
		وإلا
		    VIsual_نما = 'v';
	    }
	    وإلا إذا ((mod_ماسک & MOD_ماسک_MULTI_CLICK) == MOD_ماسک_3_CLICK)
		VIsual_نما = 'V';
	    وإلا إذا ((mod_ماسک & MOD_ماسک_MULTI_CLICK) == MOD_ماسک_4_CLICK)
		VIsual_نما = Ctrl_V;
#اگر_تعریف_شده FEAT_CLIPBOARD
	    // Make sure the clipboard gets updated.  Needed because start and
	    // end may still be the same, and the selection needs to be owned
	    clip_ستاره.vmode = NUL;
#پایان_اگر
	}
	// A double click selects a word or a block.
	إذا ((mod_ماسک & MOD_ماسک_MULTI_CLICK) == MOD_ماسک_2_CLICK)
	{
	    pos_T	*pos = عدم;
	    صحيح		gc;

	    إذا (is_click)
	    {
		// If the character under the cursor (skipping white space) is
		// not a word character, try finding a match and select a (),
		// {}, [], #if/#endif, etc. block.
		انتها_visual = curwin->w_cursor;
		طالما (gc = gchar_pos(&انتها_visual), VIM_ISWHITE(gc))
		    inc(&انتها_visual);
		إذا (oap != عدم)
		    oap->motion_type = MCHAR;
		إذا (oap != عدم
			&& VIsual_نما == 'v'
			&& !vim_iswordc(gchar_pos(&انتها_visual))
			&& EQUAL_POS(curwin->w_cursor, VIsual)
			&& (pos = findmatch(oap, NUL)) != عدم)
		{
		    curwin->w_cursor = *pos;
		    إذا (oap->motion_type == MLINE)
			VIsual_نما = 'V';
		    وإلا إذا (*p_sel == 'e')
		    {
			إذا (LT_POS(curwin->w_cursor, VIsual))
			    ++VIsual.ستون;
			وإلا
			    ++curwin->w_cursor.ستون;
		    }
		}
	    }

	    إذا (pos == عدم && (is_click || is_drag))
	    {
		// When not found a match or when dragging: extend to include
		// a word.
		إذا (LT_POS(curwin->w_cursor, orig_cursor))
		{
		    پیدا_کن_شروع_از_کلمه(&curwin->w_cursor);
		    پیدا_کن_انتها_از_کلمه(&VIsual);
		}
		وإلا
		{
		    پیدا_کن_شروع_از_کلمه(&VIsual);
		    إذا (*p_sel == 'e' && *ml_get_cursor() != NUL)
			curwin->w_cursor.ستون +=
					 (*mb_ptr2len)(ml_get_cursor());
		    پیدا_کن_انتها_از_کلمه(&curwin->w_cursor);
		}
	    }
	    curwin->w_مجموعه_curswant = true;
	}
	إذا (is_click)
	    redraw_curbuf_later(UPD_INVERTED);	// update the inversion
    }
    وإلا إذا (VIsual_فعال && !old_فعال)
    {
	إذا (mod_ماسک & MOD_ماسک_ALT)
	    VIsual_نما = Ctrl_V;
	وإلا
	    VIsual_نما = 'v';
    }

    // If Visual mode changed show it later.
    إذا ((!VIsual_فعال && old_فعال && نما_displayed)
	    || (VIsual_فعال && p_smd && پیام_silent == 0
				 && (!old_فعال || VIsual_نما != old_نما)))
	redraw_cmdline = TRUE;

    أرجع moved;
}

    فراغ
ins_mouse(صحيح c)
{
    pos_T	tpos;
    win_T	*old_curwin = curwin;

#اگر_تعریف_شده FEAT_GUI
    // When GUI is active, also move/paste when 'mouse' is empty
    إذا (!gui.درون_use)
#پایان_اگر
	إذا (!mouse_has(MOUSE_درج))
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
	    curbuf = curwin->w_میانگیر;
#اگر_تعریف_شده FEAT_کار_CHANNEL
	    إذا (bt_prompt(curbuf))
		// Restart Insert mode when re-entering the prompt buffer.
		curbuf->b_prompt_درج = 'A';
#پایان_اگر
	}
	شروع_arrow(curwin == old_curwin ? &tpos : عدم);
	إذا (curwin != new_curwin && win_valid(new_curwin))
	{
	    curwin = new_curwin;
	    curbuf = curwin->w_میانگیر;
	}
	مجموعه_can_cindent(TRUE);
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
    صحيح لغزش_or_ctrl = mod_ماسک & (MOD_ماسک_لغزش | MOD_ماسک_CTRL);

#اگر_تعریف_شده FEAT_TERMINAL
    إذا (جمله_use_loop())
	// This window is a terminal window, send the mouse event there.
	// Set "typed" to FALSE to avoid an endless loop.
	send_کلیدها_مقصد_جمله(curbuf->b_جمله, cap->cmdchar, mod_ماسک, FALSE);
    وإلا
#پایان_اگر
    إذا (cap->arg == MSCR_به_بالا || cap->arg == MSCR_پایین)
    {
	// Vertical scrolling
	إذا (!(State & نما_درج) && (mouse_vert_گام < 0 || لغزش_or_ctrl))
	{
	    // whole page up or down
	    pagescroll(cap->arg == MSCR_به_بالا ? FORWARD : BACKWARD, 1L, FALSE);
	}
	وإلا
	{
	    إذا (mouse_vert_گام < 0 || لغزش_or_ctrl)
	    {
		// whole page up or down
		cap->count1 = (دراز)(curwin->w_botline - curwin->w_topline);
	    }
	    // Don't scroll more than half the window height.
	    وإلا إذا (curwin->w_ارتفاع < mouse_vert_گام * 2)
	    {
		cap->count1 = curwin->w_ارتفاع / 2;
		إذا (cap->count1 == 0)
		    cap->count1 = 1;
	    }
	    وإلا
	    {
		cap->count1 = mouse_vert_گام;
	    }
	    cap->count0 = cap->count1;
	    nv_scroll_سطر(cap);
	}

#اگر_تعریف_شده FEAT_PROP_POPUP
	إذا (WIN_IS_POPUP(curwin))
	    popup_مجموعه_firstline(curwin);
#پایان_اگر
    }
    وإلا
    {
	// Horizontal scrolling
	دراز گام = (mouse_hor_گام < 0 || لغزش_or_ctrl)
					    ? curwin->w_عرض : mouse_hor_گام;
	دراز leftcol = curwin->w_leftcol
				     + (cap->arg == MSCR_راست ? -گام : گام);
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
    CLEAR_فیلد(cap);
    clear_oparg(&oa);
    cap.oap = &oa;
    cap.arg = dir;

#اگر_تعریف_شده FEAT_TABPANEL
    إذا (mouse_ردیف >= 0 && mouse_ستون >= 0
	    && (dir == MSCR_به_بالا || dir == MSCR_پایین)
	    && mouse_on_tabpanel())
    {
	(فراغ)tabpanel_scroll(dir == MSCR_به_بالا ? 1 : -1,
		mouse_vert_گام > 0 ? mouse_vert_گام : 3);
	أرجع;
    }
#پایان_اگر

    بدل (dir)
    {
	حالة MSCR_به_بالا:
	    cap.cmdchar = K_MOUSEUP;
	    اكسر;
	حالة MSCR_پایین:
	    cap.cmdchar = K_MOUSEDOWN;
	    اكسر;
	حالة MSCR_چپ:
	    cap.cmdchar = K_MOUSELEFT;
	    اكسر;
	حالة MSCR_راست:
	    cap.cmdchar = K_MOUSERIGHT;
	    اكسر;
	افتراضي:
	    siemsg("Invalid ins_mousescroll() argument: %d", dir);
    }

    win_T *old_curwin = curwin;
    إذا (mouse_ردیف >= 0 && mouse_ستون >= 0)
    {
	// Find the window at the mouse pointer coordinates.
	// NOTE: Must restore "curwin" to "old_curwin" before returning!
	صحيح ردیف = mouse_ردیف;
	صحيح ستون = mouse_ستون;
	curwin = mouse_پیدا_کن_win(&ردیف, &ستون, پیدا_کن_POPUP);
	إذا (curwin == عدم)
	{
	    curwin = old_curwin;
	    أرجع;
	}
	curbuf = curwin->w_میانگیر;
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

    curwin->w_redr_وضعیت = true;
    curwin = old_curwin;
    curbuf = curwin->w_میانگیر;

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
	شروع_arrow(&orig_cursor);
	مجموعه_can_cindent(TRUE);
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

    CLEAR_فیلد(cap);
    clear_oparg(&oa);
    cap.oap = &oa;
    cap.arg = dir;

    بدل (dir)
    {
	حالة MSCR_به_بالا:	 cap.cmdchar = K_MOUSEUP; اكسر;
	حالة MSCR_پایین:	 cap.cmdchar = K_MOUSEDOWN; اكسر;
	حالة MSCR_چپ:	 cap.cmdchar = K_MOUSELEFT; اكسر;
	حالة MSCR_راست: cap.cmdchar = K_MOUSERIGHT; اكسر;
    }

    إذا (mouse_ردیف < 0 || mouse_ستون < 0)
	أرجع FALSE;

    صحيح	    ردیف = mouse_ردیف;
    صحيح	    ستون = mouse_ستون;
    win_T   *wp;

    // Only scroll when the mouse is on top of the info popup.
    wp = mouse_پیدا_کن_win(&ردیف, &ستون, پیدا_کن_POPUP);
    إذا (wp == عدم || !WIN_IS_POPUP(wp) || !(wp->w_popup_flags & POPF_INFO)
							|| !wp->w_has_scrollbar)
	أرجع FALSE;

    win_T   *old_curwin = curwin;

    curwin = wp;
    curbuf = wp->w_میانگیر;
    // Call the common mouse scroll function shared with other modes.
    do_mousescroll(&cap);
    curwin = old_curwin;
    curbuf = curwin->w_میانگیر;

    // Cmdline mode doesn't normally call update_screen(), so redraw the
    // completion popup menu, which also repaints the info popup.
    إذا (cmdline_pum_فعال())
	cmdline_pum_display();
    أرجع TRUE;
}
#پایان_اگر

/*
 * Return TRUE if "c" is a mouse key.
 */
    صحيح
is_mouse_کلید(صحيح c)
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
    صحيح	    pseudo_کد;	// Code for pseudo mouse event
    صحيح	    button;		// Which mouse button is it?
    صحيح	    is_click;		// Is it a mouse button click event?
    صحيح	    is_drag;		// Is it a mouse drag event?
} mouse_جدول[] =
{
    {(صحيح)KE_LEFTMOUSE,		MOUSE_چپ,	TRUE,	FALSE},
#اگر_تعریف_شده FEAT_GUI
    {(صحيح)KE_LEFTMOUSE_NM,	MOUSE_چپ,	TRUE,	FALSE},
#پایان_اگر
    {(صحيح)KE_LEFTDRAG,		MOUSE_چپ,	FALSE,	TRUE},
    {(صحيح)KE_LEFTRELEASE,	MOUSE_چپ,	FALSE,	FALSE},
#اگر_تعریف_شده FEAT_GUI
    {(صحيح)KE_LEFTRELEASE_NM,	MOUSE_چپ,	FALSE,	FALSE},
#پایان_اگر
    {(صحيح)KE_MIDDLEMOUSE,	MOUSE_میانه,	TRUE,	FALSE},
    {(صحيح)KE_MIDDLEDRAG,	MOUSE_میانه,	FALSE,	TRUE},
    {(صحيح)KE_MIDDLERELEASE,	MOUSE_میانه,	FALSE,	FALSE},
    {(صحيح)KE_RIGHTMOUSE,	MOUSE_راست,	TRUE,	FALSE},
    {(صحيح)KE_RIGHTDRAG,		MOUSE_راست,	FALSE,	TRUE},
    {(صحيح)KE_RIGHTRELEASE,	MOUSE_راست,	FALSE,	FALSE},
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
get_mouse_button(صحيح کد, صحيح *is_click, صحيح *is_drag)
{
    صحيح	    i;

    لكل (i = 0; mouse_جدول[i].pseudo_کد; i++)
	إذا (کد == mouse_جدول[i].pseudo_کد)
	{
	    *is_click = mouse_جدول[i].is_click;
	    *is_drag = mouse_جدول[i].is_drag;
	    أرجع mouse_جدول[i].button;
	}
    أرجع 0;	    // Shouldn't get here
}

/*
 * Return the appropriate pseudo mouse event token (KE_LEFTMOUSE etc) based on
 * the given information about which mouse button is down, and whether the
 * mouse was clicked, dragged or released.
 */
    صحيح
get_pseudo_mouse_کد(
    صحيح	    button,	// eg MOUSE_LEFT
    صحيح	    is_click,
    صحيح	    is_drag)
{
    صحيح	    i;

    لكل (i = 0; mouse_جدول[i].pseudo_کد; i++)
	إذا (button == mouse_جدول[i].button
	    && is_click == mouse_جدول[i].is_click
	    && is_drag == mouse_جدول[i].is_drag)
	{
#اگر_تعریف_شده FEAT_GUI
	    // Trick: a non mappable left click and release has mouse_col -1
	    // or added MOUSE_COLOFF.  Used for 'mousefocus' in
	    // gui_mouse_moved()
	    إذا (mouse_ستون < 0 || mouse_ستون > MOUSE_COLOFF)
	    {
		إذا (mouse_ستون < 0)
		    mouse_ستون = 0;
		وإلا
		    mouse_ستون -= MOUSE_COLOFF;
		إذا (mouse_جدول[i].pseudo_کد == (صحيح)KE_LEFTMOUSE)
		    أرجع (صحيح)KE_LEFTMOUSE_NM;
		إذا (mouse_جدول[i].pseudo_کد == (صحيح)KE_LEFTRELEASE)
		    أرجع (صحيح)KE_LEFTRELEASE_NM;
	    }
#پایان_اگر
	    أرجع mouse_جدول[i].pseudo_کد;
	}
    أرجع (صحيح)KE_IGNORE;	    // not recognized, ignore it
}

#تعریف_کن HMT_نرمال	1
#تعریف_کن HMT_NETTERM	2
#تعریف_کن HMT_DEC	4
#تعریف_کن HMT_JSBTERM	8
#تعریف_کن HMT_PTERM	16
#تعریف_کن HMT_URXVT	32
#تعریف_کن HMT_GPM	64
#تعریف_کن HMT_SGR	128
#تعریف_کن HMT_SGR_REL	256
ساكن صحيح has_mouse_termcode = 0;

    فراغ
مجموعه_mouse_termcode(
    صحيح		n,	// KS_MOUSE, KS_NETTERM_MOUSE or KS_DEC_MOUSE
    char_u	*s)
{
    char_u	name[2];

    name[0] = n;
    name[1] = KE_FILLER;
    جمع_کن_termcode(name, s, FALSE);
#اگر_تعریف_شده FEAT_MOUSE_JSB
    إذا (n == KS_JSBTERM_MOUSE)
	has_mouse_termcode |= HMT_JSBTERM;
    وإلا
#پایان_اگر
#اگر_تعریف_شده FEAT_MOUSE_NET
    إذا (n == KS_NETTERM_MOUSE)
	has_mouse_termcode |= HMT_NETTERM;
    وإلا
#پایان_اگر
#اگر_تعریف_شده FEAT_MOUSE_DEC
    إذا (n == KS_DEC_MOUSE)
	has_mouse_termcode |= HMT_DEC;
    وإلا
#پایان_اگر
#اگر_تعریف_شده FEAT_MOUSE_PTERM
    إذا (n == KS_PTERM_MOUSE)
	has_mouse_termcode |= HMT_PTERM;
    وإلا
#پایان_اگر
#اگر_تعریف_شده FEAT_MOUSE_URXVT
    إذا (n == KS_URXVT_MOUSE)
	has_mouse_termcode |= HMT_URXVT;
    وإلا
#پایان_اگر
#اگر_تعریف_شده FEAT_MOUSE_GPM
    إذا (n == KS_GPM_MOUSE)
	has_mouse_termcode |= HMT_GPM;
    وإلا
#پایان_اگر
    إذا (n == KS_SGR_MOUSE)
	has_mouse_termcode |= HMT_SGR;
    وإلا إذا (n == KS_SGR_MOUSE_RELEASE)
	has_mouse_termcode |= HMT_SGR_REL;
    وإلا
	has_mouse_termcode |= HMT_نرمال;
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
	has_mouse_termcode &= ~HMT_نرمال;
}
#پایان_اگر

/*
 * setmouse() - switch mouse on/off depending on current mode and 'mouse'
 */
    فراغ
setmouse(فراغ)
{
    صحيح	    checkfor;

#اگر_تعریف_شده FEAT_MOUSESHAPE
    update_mouseshape(-1);
#پایان_اگر

    // Should be outside proc, but may break MOUSESHAPE
#اگر_تعریف_شده FEAT_GUI
    // In the GUI the mouse is always enabled.
    إذا (gui.درون_use)
	أرجع;
#پایان_اگر
    // be quick when mouse is off
    إذا (*p_mouse == NUL || has_mouse_termcode == 0)
	أرجع;

    // don't switch mouse on when not in raw mode (Ex mode)
    إذا (cur_tmode != TMODE_RAW)
    {
	mch_setmouse(FALSE);
	أرجع;
    }

    إذا (VIsual_فعال)
	checkfor = MOUSE_VISUAL;
    وإلا إذا (State == نما_HITRETURN || State == نما_ASKMORE
						     || State == نما_SETWSIZE)
	checkfor = MOUSE_RETURN;
    وإلا إذا (State & نما_درج)
	checkfor = MOUSE_درج;
    وإلا إذا (State & نما_CMDLINE)
	checkfor = MOUSE_COMMAND;
    وإلا إذا (State == نما_CONFIRM || State == نما_EXTERNCMD)
	checkfor = ' '; // don't use mouse for ":confirm" or ":!cmd"
    وإلا
	checkfor = MOUSE_نرمال;    // assume normal mode

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
	صحيح		    region_شمار,
	صحيح		    winid,
	char_u		    *area_name,
	صحيح		    mrow,
	صحيح		    mcol,
	صحيح		    which_button,
	صحيح		    mods)
{
#اگر_تعریف_شده FEAT_EVAL
    صحيح		n;
    صحيح		nclicks;
    char_u	button_str[2];
    char_u	mods_str[4];
    صحيح		mi = 0;
    dict_T	*info;
    typval_T	argvars[2];
    typval_T	rettv;
    funcexe_T	funcexe;
    صحيح		ستون = mcol;

    إذا (regions == عدم || region_شمار == 0)
	أرجع FALSE;

    // Find the click region at the given row and column.
    لكل (n = 0; n < region_شمار; n++)
    {
	إذا (regions[n].ردیف == mrow
		&& ستون >= regions[n].ستون_شروع
		&& ستون < regions[n].ستون_انتها)
	    اكسر;
    }
    إذا (n >= region_شمار || regions[n].funcname == عدم)
	أرجع FALSE;

    // Build the info dictionary.
    info = dict_alloc();
    إذا (info == عدم)
	أرجع FALSE;

    dict_جمع_کن_عدد(info, "minwid", regions[n].minwid);

    // Determine number of clicks.
    // MOD_MASK_2CLICK=0x20, MOD_MASK_3CLICK=0x40, MOD_MASK_4CLICK=0x60
    nclicks = ((mods & MOD_ماسک_MULTI_CLICK) >> 5) + 1;
    إذا (nclicks > 3)
	nclicks = 3;
    dict_جمع_کن_عدد(info, "nclicks", nclicks);

    // Button.
    إذا (which_button == MOUSE_چپ)
	button_str[0] = 'l';
    وإلا إذا (which_button == MOUSE_راست)
	button_str[0] = 'r';
    وإلا
	button_str[0] = 'm';
    button_str[1] = NUL;
    dict_جمع_کن_string_len(info, "button", button_str, 1);

    // Modifiers.
    إذا (mods & MOD_ماسک_لغزش)
	mods_str[mi++] = 's';
    إذا (mods & MOD_ماسک_CTRL)
	mods_str[mi++] = 'c';
    إذا (mods & MOD_ماسک_ALT)
	mods_str[mi++] = 'a';
    mods_str[mi] = NUL;
    dict_جمع_کن_string_len(info, "mods", mods_str, mi);

    dict_جمع_کن_عدد(info, "winid", winid);

    // "area": which option the clicked region belongs to.  Lets a shared
    // dispatcher distinguish 'statusline', 'tabline' and 'tabpanel' without
    // having to overload winid == 0.
    dict_جمع_کن_string(info, "area", area_name);

    // Expose tab page number for 'tabpanel' regions.
    إذا (regions[n].tabnr > 0)
	dict_جمع_کن_عدد(info, "tabnr", regions[n].tabnr);

    // Call the function with the info dict as argument.
    argvars[0].v_type = VAR_DICT;
    argvars[0].vval.v_dict = info;
    ++info->dv_refcount;
    argvars[1].v_type = VAR_UNKNOWN;

    rettv.v_type = VAR_عدد;
    rettv.vval.v_عدد = 0;

    CLEAR_فیلد(funcexe);
    funcexe.fe_evaluate = TRUE;
    (فراغ)call_func(regions[n].funcname, -1,
					    &rettv, 1, argvars, &funcexe);

    n = (صحيح)rettv.vval.v_عدد;
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
    (فراغ)region_شمار;
    (فراغ)winid;
    (فراغ)area_name;
    (فراغ)mrow;
    (فراغ)mcol;
    (فراغ)which_button;
    (فراغ)mods;
    أرجع FALSE;
#پایان_اگر
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
    أرجع stl_click_handler_regions(wp->w_stl_click, wp->w_stl_click_شمار,
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
jump_مقصد_mouse(
    صحيح		flags,
    صحيح		*inclusive,	// used for inclusive operator, can be NULL
    صحيح		which_button)	// MOUSE_LEFT, MOUSE_RIGHT, MOUSE_MIDDLE
{
    ساكن صحيح	on_وضعیت_سطر = 0;	// #lines below bottom of window
    ساكن صحيح	on_sep_سطر = 0;	// on separator right of window
#اگر_تعریف_شده FEAT_MENU
    ساكن صحيح  درون_winbar = FALSE;
#پایان_اگر
#اگر_تعریف_شده FEAT_PROP_POPUP
    ساكن صحيح   درون_popup_win = FALSE;
    ساكن win_T *click_درون_popup_win = عدم;
#پایان_اگر
    ساكن صحيح	prev_ردیف = -1;
    ساكن صحيح	prev_ستون = -1;
    ساكن صحيح	did_drag = FALSE;	// drag was noticed

    win_T	*wp, *old_curwin;
    pos_T	old_cursor;
    صحيح		شمار;
    صحيح		اولی;
    صحيح		ردیف = mouse_ردیف;
    صحيح		ستون = mouse_ستون;
    colnr_T	ستون_from_screen = -1;
#اگر_تعریف_شده FEAT_FOLDING
    صحيح		mouse_char = ' ';
#پایان_اگر

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
#اگر_تعریف_شده FEAT_PROP_POPUP
	إذا (click_درون_popup_win != عدم && popup_dragwin == عدم)
	    popup_close_for_mouse_click(click_درون_popup_win);

	popup_dragwin = عدم;
	click_درون_popup_win = عدم;
#پایان_اگر
    }

    إذا ((flags & MOUSE_DID_MOVE)
	    && prev_ردیف == mouse_ردیف
	    && prev_ستون == mouse_ستون)
    {
retnomove:
	// before moving the cursor for a left click which is NOT in a status
	// line, stop Visual mode
	إذا (on_وضعیت_سطر)
	    أرجع درون_وضعیت_سطر;
	إذا (on_sep_سطر)
	    أرجع درون_SEP_سطر;
#اگر_تعریف_شده FEAT_MENU
	إذا (درون_winbar)
	{
	    // A quick second click may arrive as a double-click, but we use it
	    // as a second click in the WinBar.
	    إذا ((mod_ماسک & MOD_ماسک_MULTI_CLICK) && !(flags & MOUSE_RELEASED))
	    {
		wp = mouse_پیدا_کن_win(&ردیف, &ستون, FAIL_POPUP);
		إذا (wp == عدم)
		    أرجع درون_UNKNOWN;
		winbar_click(wp, ستون);
	    }
	    أرجع درون_OTHER_WIN | MOUSE_WINBAR;
	}
#پایان_اگر
	إذا (flags & MOUSE_MAY_STOP_VIS)
	{
	    انتها_visual_نما_keep_button();
	    redraw_curbuf_later(UPD_INVERTED);	// delete the inversion
	}
#إذا defined(FEAT_CLIPBOARD)
	// Continue a modeless selection in another window.
	إذا (cmdwin_type != 0 && ردیف < cmdwin_win->w_winrow)
	    أرجع درون_OTHER_WIN;
#پایان_اگر
#اگر_تعریف_شده FEAT_PROP_POPUP
	// Continue a modeless selection in a popup window or dragging it.
	إذا (درون_popup_win)
	{
	    click_درون_popup_win = عدم;  // don't close it on release
	    إذا (popup_dragwin != عدم)
	    {
		// dragging a popup window
		popup_drag(popup_dragwin);
		أرجع درون_UNKNOWN;
	    }
	    أرجع درون_OTHER_WIN;
	}
#پایان_اگر
	أرجع درون_میانگیر;
    }

    prev_ردیف = mouse_ردیف;
    prev_ستون = mouse_ستون;

    إذا (flags & MOUSE_SETPOS)
	اذهب retnomove;				// ugly goto...

    old_curwin = curwin;
    old_cursor = curwin->w_cursor;

    إذا (!(flags & MOUSE_FOCUS))
    {
	إذا (ردیف < 0 || ستون < 0) // check if it makes sense
	    أرجع درون_UNKNOWN;

	// find the window where the row is in and adjust "row" and "col" to be
	// relative to top-left of the window
	wp = mouse_پیدا_کن_win(&ردیف, &ستون, پیدا_کن_POPUP);
	إذا (wp == عدم)
	    أرجع درون_UNKNOWN;
	dragwin = عدم;

#اگر_تعریف_شده FEAT_PROP_POPUP
	// Click in a popup window may start dragging or modeless selection,
	// but not much else.
	إذا (WIN_IS_POPUP(wp))
	{
	    on_sep_سطر = 0;
	    on_وضعیت_سطر = 0;
	    درون_popup_win = TRUE;
	    إذا (which_button == MOUSE_چپ && popup_close_if_on_X(wp, ردیف, ستون))
	    {
		أرجع درون_UNKNOWN;
	    }
	    وإلا إذا (((wp->w_popup_flags & (POPF_DRAG | POPF_RESIZE))
					      && popup_on_border(wp, ردیف, ستون))
				       || (wp->w_popup_flags & POPF_DRAGALL))
	    {
		popup_dragwin = wp;
		popup_شروع_drag(wp, ردیف, ستون);
		أرجع درون_UNKNOWN;
	    }
	    // Only close on release, otherwise it's not possible to drag or do
	    // modeless selection.
	    وإلا إذا (wp->w_popup_close == POPCLOSE_CLICK
		    && which_button == MOUSE_چپ)
	    {
		click_درون_popup_win = wp;
	    }
	    وإلا إذا (which_button == MOUSE_چپ)
		// If the click is in the scrollbar, may scroll up/down.
		popup_handle_scrollbar_click(wp, ردیف, ستون);
# ifdef FEAT_CLIPBOARD
	    أرجع درون_OTHER_WIN;
# وإلا
	    أرجع درون_UNKNOWN;
# endif
	}
	درون_popup_win = FALSE;
	popup_dragwin = عدم;
#پایان_اگر
#اگر_تعریف_شده FEAT_MENU
	إذا (ردیف == -1)
	{
	    // A click in the window toolbar does not enter another window or
	    // change Visual highlighting.
	    winbar_click(wp, ستون);
	    درون_winbar = TRUE;
	    أرجع درون_OTHER_WIN | MOUSE_WINBAR;
	}
	درون_winbar = FALSE;
#پایان_اگر

	// winpos and height may change in win_enter()!
	إذا (ردیف >= wp->w_ارتفاع)		// In (or below) status line
	{
	    on_وضعیت_سطر = ردیف - wp->w_ارتفاع + 1;
	    dragwin = wp;
	}
	وإلا
	    on_وضعیت_سطر = 0;
	إذا (ستون >= wp->w_عرض)		// In separator line
	{
	    on_sep_سطر = ستون - wp->w_عرض + 1;
	    dragwin = wp;
	}
	وإلا
	    on_sep_سطر = 0;

	// The rightmost character of the status line might be a vertical
	// separator character if there is no connecting window to the right.
	إذا (on_وضعیت_سطر && on_sep_سطر)
	{
	    إذا (stl_connected(wp))
		on_sep_سطر = 0;
	    وإلا
		on_وضعیت_سطر = 0;
	}

	// Before jumping to another buffer, or moving the cursor for a left
	// click, stop Visual mode.
	إذا (VIsual_فعال
		&& (wp->w_میانگیر != curwin->w_میانگیر
		    || (!on_وضعیت_سطر && !on_sep_سطر
#اگر_تعریف_شده FEAT_FOLDING
			&& (
# ifdef FEAT_RIGHTLEFT
			    wp->w_p_rl ? ستون < wp->w_عرض - wp->w_p_fdc :
# endif
			    ستون >= wp->w_p_fdc + (wp != cmdwin_win ? 0 : 1)
			    )
#پایان_اگر
			&& (flags & MOUSE_MAY_STOP_VIS))))
	{
	    انتها_visual_نما_keep_button();
	    redraw_curbuf_later(UPD_INVERTED);	// delete the inversion
	}
	إذا (cmdwin_type != 0 && wp != cmdwin_win)
	{
	    // A click outside the command-line window: Use modeless
	    // selection if possible.  Allow dragging the status lines.
	    on_sep_سطر = 0;
#اگر_تعریف_شده FEAT_CLIPBOARD
	    إذا (on_وضعیت_سطر)
		أرجع درون_وضعیت_سطر;
	    أرجع درون_OTHER_WIN;
#وإلا
	    ردیف = 0;
	    ستون += wp->w_wincol;
	    wp = cmdwin_win;
#پایان_اگر
	}
#إذا defined(FEAT_PROP_POPUP) && defined(FEAT_TERMINAL)
	إذا (popup_is_popup(curwin) && curbuf->b_جمله != عدم)
	    // terminal in popup window: don't jump to another window
	    أرجع درون_OTHER_WIN;
#پایان_اگر
	// Only change window focus when not clicking on or dragging the
	// status line.  Do change focus when releasing the mouse button
	// (MOUSE_FOCUS was set above if we dragged first).
	إذا (dragwin == عدم || (flags & MOUSE_RELEASED))
	    win_enter(wp, TRUE);		// can make wp invalid!

	إذا (curwin != old_curwin)
	{
#اگر_تعریف_شده CHECK_DOUBLE_CLICK
	    // set topline, to be able to check for double click ourselves
	    مجموعه_mouse_topline(curwin);
#پایان_اگر
#اگر_تعریف_شده FEAT_TERMINAL
	    // when entering a terminal window may change state
	    جمله_win_entered();
#پایان_اگر
	}
	إذا (on_وضعیت_سطر)			// In (or below) status line
	{
	    // Don't use start_arrow() if we're in the same window
	    إذا (curwin == old_curwin)
		أرجع درون_وضعیت_سطر;
	    وإلا
		أرجع درون_وضعیت_سطر | CURSOR_MOVED;
	}
	إذا (on_sep_سطر)			// In (or below) status line
	{
	    // Don't use start_arrow() if we're in the same window
	    إذا (curwin == old_curwin)
		أرجع درون_SEP_سطر;
	    وإلا
		أرجع درون_SEP_سطر | CURSOR_MOVED;
	}

	curwin->w_cursor.lnum = curwin->w_topline;
#اگر_تعریف_شده FEAT_GUI
	// remember topline, needed for double click
	gui_prev_topline = curwin->w_topline;
# ifdef FEAT_DIFF
	gui_prev_topfill = curwin->w_topfill;
# endif
#پایان_اگر
    }
    وإلا إذا (on_وضعیت_سطر && which_button == MOUSE_چپ)
    {
	إذا (dragwin != عدم)
	{
	    // Drag the status line
	    شمار = ردیف - W_WINROW(dragwin) - dragwin->w_ارتفاع + 1
							     - on_وضعیت_سطر;
	    win_drag_وضعیت_سطر(dragwin, شمار);
	    did_drag |= شمار;
	}
	أرجع درون_وضعیت_سطر;			// Cursor didn't move
    }
    وإلا إذا (on_sep_سطر && which_button == MOUSE_چپ)
    {
	إذا (dragwin != عدم)
	{
	    // Drag the separator column
	    شمار = ستون - dragwin->w_wincol - dragwin->w_عرض + 1
								- on_sep_سطر;
	    win_drag_vsep_سطر(dragwin, شمار);
	    did_drag |= شمار;
	}
	أرجع درون_SEP_سطر;			// Cursor didn't move
    }
#اگر_تعریف_شده FEAT_MENU
    وإلا إذا (درون_winbar)
    {
	// After a click on the window toolbar don't start Visual mode.
	أرجع درون_OTHER_WIN | MOUSE_WINBAR;
    }
#پایان_اگر
    وإلا // keep_window_focus must be TRUE
    {
	// before moving the cursor for a left click, stop Visual mode
	إذا (flags & MOUSE_MAY_STOP_VIS)
	{
	    انتها_visual_نما_keep_button();
	    redraw_curbuf_later(UPD_INVERTED);	// delete the inversion
	}

#إذا defined(FEAT_CLIPBOARD)
	// Continue a modeless selection in another window.
	إذا (cmdwin_type != 0 && ردیف < cmdwin_win->w_winrow)
	    أرجع درون_OTHER_WIN;
#پایان_اگر
#اگر_تعریف_شده FEAT_PROP_POPUP
	إذا (درون_popup_win)
	{
	    إذا (popup_dragwin != عدم)
	    {
		// dragging a popup window
		popup_drag(popup_dragwin);
		أرجع درون_UNKNOWN;
	    }
	    // continue a modeless selection in a popup window
	    click_درون_popup_win = عدم;
	    أرجع درون_OTHER_WIN;
	}
#پایان_اگر

	ردیف -= W_WINROW(curwin);
	ستون -= curwin->w_wincol;

	// When clicking beyond the end of the window, scroll the screen.
	// Scroll by however many rows outside the window we are.
	إذا (ردیف < 0)
	{
	    شمار = 0;
	    لكل (اولی = TRUE; curwin->w_topline > 1; )
	    {
#اگر_تعریف_شده FEAT_DIFF
		إذا (curwin->w_topfill < diff_check_fill(curwin, curwin->w_topline))
		    ++شمار;
		وإلا
#پایان_اگر
		    شمار += plines(curwin->w_topline - 1);
		إذا (!اولی && شمار > -ردیف)
		    اكسر;
		اولی = FALSE;
#اگر_تعریف_شده FEAT_FOLDING
		(فراغ)hasFolding(curwin->w_topline, &curwin->w_topline, عدم);
#پایان_اگر
#اگر_تعریف_شده FEAT_DIFF
		إذا (curwin->w_topfill < diff_check_fill(curwin, curwin->w_topline))
		    ++curwin->w_topfill;
		وإلا
#پایان_اگر
		{
		    --curwin->w_topline;
#اگر_تعریف_شده FEAT_DIFF
		    curwin->w_topfill = 0;
#پایان_اگر
		}
	    }
#اگر_تعریف_شده FEAT_DIFF
	    check_topfill(curwin, FALSE);
#پایان_اگر
	    curwin->w_valid &=
		      ~(VALID_WROW|VALID_CROW|VALID_BOTLINE|VALID_BOTLINE_AP);
	    redraw_later(UPD_VALID);
	    ردیف = 0;
	}
	وإلا إذا (ردیف >= curwin->w_ارتفاع)
	{
	    شمار = 0;
	    لكل (اولی = TRUE; curwin->w_topline < curbuf->b_ml.ml_سطر_شمار; )
	    {
#اگر_تعریف_شده FEAT_DIFF
		إذا (curwin->w_topfill > 0)
		    ++شمار;
		وإلا
#پایان_اگر
		    شمار += plines(curwin->w_topline);
		إذا (!اولی && شمار > ردیف - curwin->w_ارتفاع + 1)
		    اكسر;
		اولی = FALSE;
#اگر_تعریف_شده FEAT_FOLDING
		إذا (hasFolding(curwin->w_topline, عدم, &curwin->w_topline)
			&& curwin->w_topline == curbuf->b_ml.ml_سطر_شمار)
		    اكسر;
#پایان_اگر
#اگر_تعریف_شده FEAT_DIFF
		إذا (curwin->w_topfill > 0)
		    --curwin->w_topfill;
		وإلا
#پایان_اگر
		{
		    ++curwin->w_topline;
#اگر_تعریف_شده FEAT_DIFF
		    curwin->w_topfill =
				   diff_check_fill(curwin, curwin->w_topline);
#پایان_اگر
		}
	    }
#اگر_تعریف_شده FEAT_DIFF
	    check_topfill(curwin, FALSE);
#پایان_اگر
	    redraw_later(UPD_VALID);
	    curwin->w_valid &=
		      ~(VALID_WROW|VALID_CROW|VALID_BOTLINE|VALID_BOTLINE_AP);
	    ردیف = curwin->w_ارتفاع - 1;
	}
	وإلا إذا (ردیف == 0)
	{
	    // When dragging the mouse, while the text has been scrolled up as
	    // far as it goes, moving the mouse in the top line should scroll
	    // the text down (done later when recomputing w_topline).
	    إذا (mouse_dragging > 0
		    && curwin->w_cursor.lnum
				       == curwin->w_میانگیر->b_ml.ml_سطر_شمار
		    && curwin->w_cursor.lnum == curwin->w_topline)
		curwin->w_valid &= ~(VALID_TOPLINE);
	}
    }

    إذا (prev_ردیف >= W_WINROW(curwin)
	&& prev_ردیف < W_WINROW(curwin) + curwin->w_ارتفاع
	&& prev_ستون >= curwin->w_wincol && prev_ستون < W_ENDCOL(curwin)
						       && Screen_سطرها != عدم)
    {
	صحيح off = سطر_جابه_جایی[prev_ردیف] + prev_ستون;

	// Only use ScreenCols[] after the window was redrawn.  Mainly matters
	// for tests, a user would not click before redrawing.
	// Do not use when 'virtualedit' is active.
	إذا (curwin->w_redr_type <= UPD_VALID_NO_UPDATE)
	    ستون_from_screen = Screen_تعداد_ستون[off];
#اگر_تعریف_شده FEAT_FOLDING
	// Remember the character under the mouse, it might be a '-' or '+' in
	// the fold column.
	mouse_char = enc_utf8 && Screen_سطرها_UC[off] != 0
				       ? Screen_سطرها_UC[off] : Screen_سطرها[off];
#پایان_اگر
    }

#اگر_تعریف_شده FEAT_FOLDING
    // Check for position outside of the fold column.
    إذا (
# ifdef FEAT_RIGHTLEFT
	    curwin->w_p_rl ? ستون < curwin->w_عرض - curwin->w_p_fdc :
# endif
	    ستون >= curwin->w_p_fdc + (cmdwin_win != curwin ? 0 : 1)
       )
	mouse_char = ' ';
#پایان_اگر

    // compute the position in the buffer line from the posn on the screen
    إذا (mouse_comp_pos(curwin, &ردیف, &ستون, &curwin->w_cursor.lnum, عدم))
	mouse_past_bottom = TRUE;

    // Start Visual mode before coladvance(), for when 'sel' != "old"
    إذا ((flags & MOUSE_MAY_VIS) && !VIsual_فعال)
    {
	check_visual_highlight();
	VIsual = old_cursor;
	VIsual_فعال = TRUE;
	VIsual_reselect = TRUE;
	// if 'selectmode' contains "mouse", start Select mode
	may_شروع_select('o');
	setmouse();
	إذا (p_smd && پیام_silent == 0)
	    redraw_cmdline = TRUE;	// show visual mode later
    }

    إذا (ستون_from_screen >= 0)
    {
	// Use the virtual column from ScreenCols[], it is accurate also after
	// concealed characters.
	ستون = ستون_from_screen;
    }

    curwin->w_curswant = ستون;
    curwin->w_مجموعه_curswant = false;	// May still have been TRUE
    إذا (coladvance(ستون) == FAIL)	// Mouse click beyond end of line
    {
	إذا (inclusive != عدم)
	    *inclusive = TRUE;
	mouse_past_eol = TRUE;
    }
    وإلا إذا (inclusive != عدم)
	*inclusive = FALSE;

    شمار = درون_میانگیر;
    إذا (curwin != old_curwin || curwin->w_cursor.lnum != old_cursor.lnum
	    || curwin->w_cursor.ستون != old_cursor.ستون)
	شمار |= CURSOR_MOVED;		// Cursor has moved

#اگر_تعریف_شده FEAT_FOLDING
    إذا (mouse_char == curwin->w_fill_chars.foldclosed)
	شمار |= MOUSE_FOLD_OPEN;
    وإلا إذا (mouse_char != ' ')
	شمار |= MOUSE_FOLD_CLOSE;
#پایان_اگر

    أرجع شمار;
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
#اگر_تعریف_شده FEAT_GUI
	    (!gui.درون_use || vim_strchr(p_go, GO_HORSCROLL) == عدم) &&
#پایان_اگر
		    !virtual_فعال()
	    && (دراز)leftcol > scroll_سطر_len(curwin->w_cursor.lnum))
    {
	curwin->w_cursor.lnum = ui_پیدا_کن_longest_lnum();
	curwin->w_cursor.ستون = 0;
    }

    أرجع مجموعه_leftcol((colnr_T)leftcol);
}

/*
 * Normal and Visual modes implementation for scrolling in direction
 * "cap->arg", which is one of the MSCR_ values.
 */
    فراغ
nv_mousescroll(cmdarg_T *cap)
{
    win_T   *old_curwin = curwin;

#اگر_تعریف_شده FEAT_TABPANEL
    إذا (mouse_ردیف >= 0 && mouse_ستون >= 0
	    && (cap->arg == MSCR_به_بالا || cap->arg == MSCR_پایین)
	    && mouse_on_tabpanel())
    {
	(فراغ)tabpanel_scroll(cap->arg == MSCR_به_بالا ? 1 : -1,
		mouse_vert_گام > 0 ? mouse_vert_گام : 3);
	أرجع;
    }
#پایان_اگر

    إذا (mouse_ردیف >= 0 && mouse_ستون >= 0)
    {
	// Find the window at the mouse pointer coordinates.
	// NOTE: Must restore "curwin" to "old_curwin" before returning!
	صحيح ردیف = mouse_ردیف;
	صحيح ستون = mouse_ستون;
	curwin = mouse_پیدا_کن_win(&ردیف, &ستون, پیدا_کن_POPUP);
	إذا (curwin == عدم)
	{
	    curwin = old_curwin;
	    أرجع;
	}

#اگر_تعریف_شده FEAT_PROP_POPUP
	إذا (WIN_IS_POPUP(curwin) && !curwin->w_has_scrollbar)
	{
	    // cannot scroll this popup window
	    curwin = old_curwin;
	    أرجع;
	}
#پایان_اگر
	curbuf = curwin->w_میانگیر;
    }

    // Call the common mouse scroll function shared with other modes.
    do_mousescroll(cap);

    curwin->w_redr_وضعیت = true;
    curwin = old_curwin;
    curbuf = curwin->w_میانگیر;
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
    char_u	*کلید_name,
    char_u	*modifiers_شروع,
    صحيح		idx,
    صحيح		*modifiers)
{
    صحيح		j;
    char_u	*p;
#إذا !defined(UNIX) || defined(FEAT_MOUSE_XTERM) || defined(FEAT_GUI) \
    || defined(FEAT_MOUSE_GPM) || defined(FEAT_SYSMOUSE)
    char_u	بایتها[6];
    صحيح		رقم_بایتها;
#پایان_اگر
    صحيح		mouse_کد = 0;	    // init for GCC
    صحيح		is_click, is_drag;
    صحيح		is_release, release_is_ambiguous;
    صحيح		wheel_کد = 0;
    صحيح		current_button;
    ساكن صحيح	orig_رقم_clicks = 1;
    ساكن صحيح	orig_mouse_کد = 0x0;
#اگر_تعریف_شده CHECK_DOUBLE_CLICK
    ساكن صحيح	orig_mouse_ستون = 0;
    ساكن صحيح	orig_mouse_ردیف = 0;
    ساكن بنية timeval  orig_mouse_زمان = {0, 0};
    // time of previous mouse click
    بنية timeval  mouse_زمان;		// time of current mouse click
    دراز	timediff;		// elapsed time in msec
#پایان_اگر

    is_click = is_drag = is_release = release_is_ambiguous = FALSE;

#إذا !defined(UNIX) || defined(FEAT_MOUSE_XTERM) || defined(FEAT_GUI) \
    || defined(FEAT_MOUSE_GPM) || defined(FEAT_SYSMOUSE)
    إذا (کلید_name[0] == KS_MOUSE
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
	لكل (;;)
	{
	    // For the GUI and for MS-Windows two bytes each are used for row
	    // and column.  Allows for more than 223 columns.
# إذا defined(FEAT_GUI) || defined(MSWIN)
	    إذا (TRUE
#  إذا defined(FEAT_GUI) && !defined(MSWIN)
		&& gui.درون_use
#  endif
		)
	    {
		رقم_بایتها = get_بایتها_from_buf(tp + *slen, بایتها, 5);
		إذا (رقم_بایتها == -1)	// not enough coordinates
		    أرجع -1;
		mouse_کد = بایتها[0];
		mouse_ستون = 128 * (بایتها[1] - ' ' - 1)
		    + بایتها[2] - ' ' - 1;
		mouse_ردیف = 128 * (بایتها[3] - ' ' - 1)
		    + بایتها[4] - ' ' - 1;
	    }
	    وإلا
# endif
	    {
		رقم_بایتها = get_بایتها_from_buf(tp + *slen, بایتها, 3);
		إذا (رقم_بایتها == -1)	// not enough coordinates
		    أرجع -1;
		mouse_کد = بایتها[0];
		mouse_ستون = بایتها[1] - ' ' - 1;
		mouse_ردیف = بایتها[2] - ' ' - 1;
	    }
	    *slen += رقم_بایتها;

	    // If the following bytes is also a mouse code and it has the same
	    // code, dump this one and get the next.  This makes dragging a
	    // whole lot faster.
# ifdef FEAT_GUI
	    إذا (gui.درون_use)
		j = 3;
	    وإلا
# endif
		j = get_termcode_len(idx);
	    إذا (STRNCMP(tp, tp + *slen, (size_t)j) == 0
		    && tp[*slen + j] == mouse_کد
		    && tp[*slen + j + 1] != NUL
		    && tp[*slen + j + 2] != NUL
# ifdef FEAT_GUI
		    && (!gui.درون_use
			|| (tp[*slen + j + 3] != NUL
			    && tp[*slen + j + 4] != NUL))
# endif
	       )
		*slen += j;
	    وإلا
		اكسر;
	}
    }

    إذا (کلید_name[0] == KS_URXVT_MOUSE
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
	p = modifiers_شروع;
	إذا (p == عدم)
	    أرجع -1;

	mouse_کد = getdigits(&p);
	إذا (*p++ != ';')
	    أرجع -1;

	// when mouse reporting is SGR, add 32 to mouse code
	إذا (کلید_name[0] == KS_SGR_MOUSE
		|| کلید_name[0] == KS_SGR_MOUSE_RELEASE)
	    mouse_کد += 32;

	mouse_ستون = getdigits(&p) - 1;
	إذا (*p++ != ';')
	    أرجع -1;

	mouse_ردیف = getdigits(&p) - 1;

	// The modifiers were the mouse coordinates, not the modifier keys
	// (alt/shift/ctrl/meta) state.
	*modifiers = 0;
    }

    إذا (کلید_name[0] == KS_SGR_MOUSE
	    || کلید_name[0] == KS_SGR_MOUSE_RELEASE)
    {
	إذا (کلید_name[0] == KS_SGR_MOUSE_RELEASE)
	{
	    is_release = TRUE;
	    // This is used below to set held_button.
	    mouse_کد |= MOUSE_RELEASE;
	}
    }
    وإلا
    {
	release_is_ambiguous = TRUE;
	إذا ((mouse_کد & MOUSE_RELEASE) == MOUSE_RELEASE)
	    is_release = TRUE;
    }

    إذا (کلید_name[0] == KS_MOUSE
# ifdef FEAT_MOUSE_GPM
	    || کلید_name[0] == KS_GPM_MOUSE
# endif
# ifdef FEAT_MOUSE_URXVT
	    || کلید_name[0] == KS_URXVT_MOUSE
# endif
	    || کلید_name[0] == KS_SGR_MOUSE
	    || کلید_name[0] == KS_SGR_MOUSE_RELEASE)
    {
# إذا !defined(MSWIN)
	/*
	 * Handle old style mouse events.
	 * Recognize the xterm mouse wheel, but not in the GUI, the
	 * Linux console with GPM and the MS-DOS or Win32 console
	 * (multi-clicks use >= 0x60).
	 */
	إذا (mouse_کد >= MOUSEWHEEL_کم && mouse_کد < MOUSESIDEBUTTONS_کم
#  ifdef FEAT_GUI
		&& !gui.درون_use
#  endif
#  ifdef FEAT_MOUSE_GPM
		&& کلید_name[0] != KS_GPM_MOUSE
#  endif
	   )
	{
#  إذا defined(UNIX)
	    إذا (use_xterm_mouse() > 1 && mouse_کد >= 0x80)
		// mouse-move event, using MOUSE_DRAG works
		mouse_کد = MOUSE_DRAG;
	    وإلا
#  endif
		// Keep the mouse_code before it's changed, so that we
		// remember that it was a mouse wheel click.
		wheel_کد = mouse_کد;
	}
#  ifdef FEAT_MOUSE_XTERM
	وإلا إذا (held_button == MOUSE_RELEASE
#   ifdef FEAT_GUI
		&& !gui.درون_use
#   endif
		&& (mouse_کد == 0x23 || mouse_کد == 0x24
		    || mouse_کد == 0x40 || mouse_کد == 0x41))
	{
	    // Apparently 0x23 and 0x24 are used by rxvt scroll wheel.
	    // And 0x40 and 0x41 are used by some xterm emulator.
	    wheel_کد = mouse_کد - (mouse_کد >= 0x40 ? 0x40 : 0x23)
							      + MOUSEWHEEL_کم;
	}
#  endif

#  إذا defined(UNIX)
	وإلا إذا (use_xterm_mouse() > 1)
	{
	    إذا (mouse_کد & MOUSE_DRAG_XTERM)
		mouse_کد |= MOUSE_DRAG;
	}
#  endif
#  ifdef FEAT_XCLIPBOARD
	وإلا إذا (!(mouse_کد & MOUSE_DRAG & ~MOUSE_CLICK_ماسک))
	{
	    إذا (is_release)
		stop_xterm_trace();
	    وإلا
		شروع_xterm_trace(mouse_کد);
	}
#  endif
# endif
    }
#پایان_اگر // !UNIX || FEAT_MOUSE_XTERM
#اگر_تعریف_شده FEAT_MOUSE_NET
    إذا (کلید_name[0] == KS_NETTERM_MOUSE)
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

	mouse_ستون = mc - 1;
	mouse_ردیف = mr - 1;
	mouse_کد = MOUSE_چپ;
	*slen += (صحيح)(p - (tp + *slen));
    }
#پایان_اگر	// FEAT_MOUSE_NET
#اگر_تعریف_شده FEAT_MOUSE_JSB
    إذا (کلید_name[0] == KS_JSBTERM_MOUSE)
    {
	صحيح mult, val, iter, button, وضعیت;

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
	button = mouse_کد = 0;
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
	وضعیت = *p++;
	لكل (val = 0, mult = 100, iter = 0; iter < 3; iter++,
		mult /= 10, p++)
	    إذا (*p >= '0' && *p <= '9')
		val += (*p - '0') * mult;
	    وإلا
		أرجع -1;
	mouse_ستون = val;
	لكل (val = 0, mult = 100, iter = 0; iter < 3; iter++,
		mult /= 10, p++)
	    إذا (*p >= '0' && *p <= '9')
		val += (*p - '0') * mult;
	    وإلا
		أرجع -1;
	mouse_ردیف = val;
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
	بدل (وضعیت)
	{
	    حالة 'D': // Double Click
	    حالة 'S': // Single Click
		إذا (button & 1) mouse_کد |= MOUSE_چپ;
		إذا (button & 2) mouse_کد |= MOUSE_میانه;
		إذا (button & 4) mouse_کد |= MOUSE_راست;
		إذا (button & 8) mouse_کد |= MOUSE_لغزش;
		إذا (button & 16) mouse_کد |= MOUSE_CTRL;
		اكسر;
	    حالة 'm': // Mouse move
		إذا (button & 1) mouse_کد |= MOUSE_چپ;
		إذا (button & 2) mouse_کد |= MOUSE_میانه;
		إذا (button & 4) mouse_کد |= MOUSE_راست;
		إذا (button & 8) mouse_کد |= MOUSE_لغزش;
		إذا (button & 16) mouse_کد |= MOUSE_CTRL;
		إذا ((button & 7) != 0)
		{
		    held_button = mouse_کد;
		    mouse_کد |= MOUSE_DRAG;
		}
		is_drag = TRUE;
		showmode();
		اكسر;
	    حالة 'd': // Button Down
		إذا (button & 1) mouse_کد |= MOUSE_چپ;
		إذا (button & 2) mouse_کد |= MOUSE_میانه;
		إذا (button & 4) mouse_کد |= MOUSE_راست;
		إذا (button & 8) mouse_کد |= MOUSE_لغزش;
		إذا (button & 16) mouse_کد |= MOUSE_CTRL;
		اكسر;
	    حالة 'u': // Button Up
		is_release = TRUE;
		إذا (button & 1)
		    mouse_کد |= MOUSE_چپ;
		إذا (button & 2)
		    mouse_کد |= MOUSE_میانه;
		إذا (button & 4)
		    mouse_کد |= MOUSE_راست;
		إذا (button & 8)
		    mouse_کد |= MOUSE_لغزش;
		إذا (button & 16)
		    mouse_کد |= MOUSE_CTRL;
		اكسر;
	    افتراضي: أرجع -1; // Unknown Result
	}

	*slen += (p - (tp + *slen));
    }
#پایان_اگر // FEAT_MOUSE_JSB
#اگر_تعریف_شده FEAT_MOUSE_DEC
    إذا (کلید_name[0] == KS_DEC_MOUSE)
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

	mouse_کد = 0;
	بدل (Pe)
	{
	    حالة  0: أرجع -1; // position request while unavailable
	    حالة  1: // a response to a locator position request includes
		     //	the status of all buttons
		     Pb &= 7;   // mask off and ignore fourth button
		     إذا (Pb & 4)
			 mouse_کد  = MOUSE_چپ;
		     إذا (Pb & 2)
			 mouse_کد  = MOUSE_میانه;
		     إذا (Pb & 1)
			 mouse_کد  = MOUSE_راست;
		     إذا (Pb)
		     {
			 held_button = mouse_کد;
			 mouse_کد |= MOUSE_DRAG;
			 WantQueryMouse = TRUE;
		     }
		     is_drag = TRUE;
		     showmode();
		     اكسر;
	    حالة  2: mouse_کد = MOUSE_چپ;
		     WantQueryMouse = TRUE;
		     اكسر;
	    حالة  3: mouse_کد = MOUSE_چپ;
		     is_release = TRUE;
		     اكسر;
	    حالة  4: mouse_کد = MOUSE_میانه;
		     WantQueryMouse = TRUE;
		     اكسر;
	    حالة  5: mouse_کد = MOUSE_میانه;
		     is_release = TRUE;
		     اكسر;
	    حالة  6: mouse_کد = MOUSE_راست;
		     WantQueryMouse = TRUE;
		     اكسر;
	    حالة  7: mouse_کد = MOUSE_راست;
		     is_release = TRUE;
		     اكسر;
	    حالة  8: أرجع -1; // fourth button down
	    حالة  9: أرجع -1; // fourth button up
	    حالة 10: أرجع -1; // mouse outside of filter rectangle
	    افتراضي: أرجع -1; // should never occur
	}

	mouse_ستون = Pc - 1;
	mouse_ردیف = Pr - 1;

	*slen += (صحيح)(p - (tp + *slen));
    }
#پایان_اگر // FEAT_MOUSE_DEC
#اگر_تعریف_شده FEAT_MOUSE_PTERM
    إذا (کلید_name[0] == KS_PTERM_MOUSE)
    {
	صحيح button, رقم_clicks, action;

	p = tp + *slen;

	action = getdigits(&p);
	إذا (*p++ != ';')
	    أرجع -1;

	mouse_ردیف = getdigits(&p);
	إذا (*p++ != ';')
	    أرجع -1;
	mouse_ستون = getdigits(&p);
	إذا (*p++ != ';')
	    أرجع -1;

	button = getdigits(&p);
	mouse_کد = 0;

	بدل (button)
	{
	    حالة 4: mouse_کد = MOUSE_چپ; اكسر;
	    حالة 1: mouse_کد = MOUSE_راست; اكسر;
	    حالة 2: mouse_کد = MOUSE_میانه; اكسر;
	    افتراضي: أرجع -1;
	}

	بدل (action)
	{
	    حالة 31: // Initial press
		إذا (*p++ != ';')
		    أرجع -1;

		رقم_clicks = getdigits(&p); // Not used
		اكسر;

	    حالة 32: // Release
		is_release = TRUE;
		اكسر;

	    حالة 33: // Drag
		held_button = mouse_کد;
		mouse_کد |= MOUSE_DRAG;
		اكسر;

	    افتراضي:
		أرجع -1;
	}

	إذا (*p++ != 't')
	    أرجع -1;

	*slen += (p - (tp + *slen));
    }
#پایان_اگر // FEAT_MOUSE_PTERM

    // Interpret the mouse code
    current_button = (mouse_کد & MOUSE_CLICK_ماسک);
    إذا (is_release)
	current_button |= MOUSE_RELEASE;

    إذا (current_button == MOUSE_RELEASE
#اگر_تعریف_شده FEAT_MOUSE_XTERM
	    && wheel_کد == 0
#پایان_اگر
       )
    {
	/*
	 * If we get a mouse drag or release event when there is no mouse
	 * button held down (held_button == MOUSE_RELEASE), produce a K_IGNORE
	 * below.
	 * (can happen when you hold down two buttons and then let them go, or
	 * click in the menu bar, but not on a menu, and drag into the text).
	 */
	إذا ((mouse_کد & MOUSE_DRAG) == MOUSE_DRAG)
	    is_drag = TRUE;
	current_button = held_button;
    }
    وإلا
    {
      إذا (wheel_کد == 0)
      {
#اگر_تعریف_شده CHECK_DOUBLE_CLICK
# ifdef FEAT_MOUSE_GPM
	/*
	 * Only for Unix, when GUI not active, we handle multi-clicks here, but
	 * not for GPM mouse events.
	 */
#  ifdef FEAT_GUI
	إذا (کلید_name[0] != KS_GPM_MOUSE && !gui.درون_use)
#  وإلا
	    إذا (کلید_name[0] != KS_GPM_MOUSE)
#  endif
# وإلا
#  ifdef FEAT_GUI
		إذا (!gui.درون_use)
#  endif
# endif
		{
		    /*
		     * Compute the time elapsed since the previous mouse click.
		     */
		    gettimeofday(&mouse_زمان, عدم);
		    إذا (orig_mouse_زمان.tv_sec == 0)
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
			timediff = زمان_diff_ms(&orig_mouse_زمان, &mouse_زمان);
		    orig_mouse_زمان = mouse_زمان;
		    إذا (mouse_کد == orig_mouse_کد
			    && timediff < p_mouset
			    && orig_رقم_clicks != 4
			    && orig_mouse_ستون == mouse_ستون
			    && orig_mouse_ردیف == mouse_ردیف
			    && (is_mouse_topline(curwin)
				// Double click in tab pages line also works
				// when window contents changes.
				|| (mouse_ردیف == 0 && firstwin->w_winrow > 0))
		       )
			++orig_رقم_clicks;
		    وإلا
			orig_رقم_clicks = 1;
		    orig_mouse_ستون = mouse_ستون;
		    orig_mouse_ردیف = mouse_ردیف;
		    مجموعه_mouse_topline(curwin);
		}
# إذا defined(FEAT_GUI) || defined(FEAT_MOUSE_GPM)
		وإلا
		    orig_رقم_clicks = رقم_MOUSE_CLICKS(mouse_کد);
# endif
#وإلا
	orig_رقم_clicks = رقم_MOUSE_CLICKS(mouse_کد);
#پایان_اگر
	is_click = TRUE;
      }
      orig_mouse_کد = mouse_کد;
    }
    إذا (!is_drag)
	held_button = mouse_کد & MOUSE_CLICK_ماسک;

    /*
     * Translate the actual mouse event into a pseudo mouse event.
     * First work out what modifiers are to be used.
     */
    إذا (orig_mouse_کد & MOUSE_لغزش)
	*modifiers |= MOD_ماسک_لغزش;
    إذا (orig_mouse_کد & MOUSE_CTRL)
	*modifiers |= MOD_ماسک_CTRL;
    إذا (orig_mouse_کد & MOUSE_ALT)
	*modifiers |= MOD_ماسک_ALT;
    إذا (orig_رقم_clicks == 2)
	*modifiers |= MOD_ماسک_2_CLICK;
    وإلا إذا (orig_رقم_clicks == 3)
	*modifiers |= MOD_ماسک_3_CLICK;
    وإلا إذا (orig_رقم_clicks == 4)
	*modifiers |= MOD_ماسک_4_CLICK;

    // Work out our pseudo mouse event. Note that MOUSE_RELEASE gets added,
    // then it's not mouse up/down.
    کلید_name[0] = KS_EXTRA;
    إذا (wheel_کد != 0 && (!is_release || release_is_ambiguous))
    {
	إذا (wheel_کد & MOUSE_CTRL)
	    *modifiers |= MOD_ماسک_CTRL;
	إذا (wheel_کد & MOUSE_ALT)
	    *modifiers |= MOD_ماسک_ALT;

	إذا (wheel_کد & 1 && wheel_کد & 2)
	    کلید_name[1] = (صحيح)KE_MOUSELEFT;
	وإلا إذا (wheel_کد & 2)
	    کلید_name[1] = (صحيح)KE_MOUSERIGHT;
	وإلا إذا (wheel_کد & 1)
	    کلید_name[1] = (صحيح)KE_MOUSEUP;
	وإلا
	    کلید_name[1] = (صحيح)KE_MOUSEDOWN;

	held_button = MOUSE_RELEASE;
    }
    وإلا
    {
#إذا defined(UNIX)
	إذا (use_xterm_mouse() && orig_mouse_کد >= MOUSESIDEBUTTONS_کم)
	    current_button = (current_button) ? MOUSE_X2 : MOUSE_X1;
#پایان_اگر
	کلید_name[1] = get_pseudo_mouse_کد(current_button, is_click, is_drag);
    }


    // Make sure the mouse position is valid.  Some terminals may return weird
    // values.
    إذا (mouse_ستون >= Columns)
	mouse_ستون = Columns - 1;
    إذا (mouse_ردیف >= ردیفها)
	mouse_ردیف = ردیفها - 1;

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
    صحيح		ستون = *colp;
    صحيح		ردیف = *rowp;
    linenr_T	lnum;
    صحيح		retval = FALSE;
    صحيح		off;
    صحيح		شمار;

#اگر_تعریف_شده FEAT_RIGHTLEFT
    إذا (win->w_p_rl)
	ستون = win->w_عرض - 1 - ستون;
#پایان_اگر

    lnum = win->w_topline;

    طالما (ردیف > 0)
    {
	صحيح cache_idx = lnum - win->w_topline;

	// Only "Rows" lines are cached, with folding we'll run out of entries
	// and use the slow way.
	إذا (plines_cache != عدم && cache_idx < ردیفها
						&& plines_cache[cache_idx] > 0)
	    شمار = plines_cache[cache_idx];
	وإلا
	{
#اگر_تعریف_شده FEAT_DIFF
	    // Don't include filler lines in "count"
	    إذا (win->w_p_diff
# ifdef FEAT_FOLDING
		    && !hasFoldingWin(win, lnum, عدم, عدم, TRUE, عدم)
# endif
		    )
	    {
		إذا (lnum == win->w_topline)
		    ردیف -= win->w_topfill;
		وإلا
		    ردیف -= diff_check_fill(win, lnum);
		شمار = plines_win_nofill(win, lnum, FALSE);
	    }
	    وإلا
#پایان_اگر
		شمار = plines_win(win, lnum, FALSE);
	    إذا (plines_cache != عدم && cache_idx < ردیفها)
		plines_cache[cache_idx] = شمار;
	}

	إذا (win->w_skipcol > 0 && lnum == win->w_topline)
	{
	    صحيح width1 = win->w_عرض - win_ستون_off(win);

	    إذا (width1 > 0)
	    {
		صحيح skip_سطرها = 0;

		// Adjust for 'smoothscroll' clipping the top screen lines.
		// A similar formula is used in curs_columns().
		إذا (win->w_skipcol > width1)
		    skip_سطرها = (win->w_skipcol - width1)
					    / (width1 + win_ستون_off2(win)) + 1;
		وإلا إذا (win->w_skipcol > 0)
		    skip_سطرها = 1;

		شمار -= skip_سطرها;
	    }
	}

	إذا (شمار > ردیف)
	    اكسر;	// Position is in this buffer line.
#اگر_تعریف_شده FEAT_FOLDING
	(فراغ)hasFoldingWin(win, lnum, عدم, &lnum, TRUE, عدم);
#پایان_اگر
	إذا (lnum == win->w_میانگیر->b_ml.ml_سطر_شمار)
	{
	    retval = TRUE;
	    اكسر;		// past end of file
	}
	ردیف -= شمار;
	++lnum;
    }

    إذا (!retval)
    {
	// Compute the column without wrapping.
	off = win_ستون_off(win) - win_ستون_off2(win);
	إذا (ستون < off)
	    ستون = off;
	ستون += ردیف * (win->w_عرض - off);

	// Add skip column for the topline.
	إذا (lnum == win->w_topline)
	    ستون += win->w_skipcol;
    }

    إذا (!win->w_p_wrap)
	ستون += win->w_leftcol;

    // skip line number and fold column in front of the line
    ستون -= win_ستون_off(win);
    إذا (ستون <= 0)
    {
#اگر_تعریف_شده FEAT_NETBEANS_INTG
	// if mouse is clicked on the gutter, then inform the netbeans server
	إذا (*colp < win_ستون_off(win))
	    netbeans_gutter_click(lnum);
#پایان_اگر
	ستون = 0;
    }

    *colp = ستون;
    *rowp = ردیف;
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
mouse_پیدا_کن_win(صحيح *rowp, صحيح *colp, mouse_پیدا_کن_T popup استفادهنشده)
{
    قاب_T	*fp;
    win_T	*wp;

#اگر_تعریف_شده FEAT_PROP_POPUP
    win_T	*pwp = عدم;

    إذا (popup != IGNORE_POPUP)
    {
	popup_reset_handled(POPUP_HANDLED_1);
	طالما ((wp = پیدا_کن_بعدی_popup(TRUE, POPUP_HANDLED_1)) != عدم)
	{
	    إذا (*rowp >= wp->w_winrow && *rowp < wp->w_winrow + popup_ارتفاع(wp)
		    && *colp >= wp->w_wincol
				    && *colp < wp->w_wincol + popup_عرض(wp))
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
#پایان_اگر

    fp = topframe;

    إذا (*colp < firstwin->w_wincol
	    || *colp >= firstwin->w_wincol + fp->fr_عرض
	    || *rowp < firstwin->w_winrow)
	أرجع عدم;

    *rowp -= firstwin->w_winrow;
    *colp -= firstwin->w_wincol;
    لكل (;;)
    {
	إذا (fp->fr_layout == FR_برگ)
	    اكسر;
	إذا (fp->fr_layout == FR_ردیف)
	{
	    لكل (fp = fp->fr_فرزند; fp->fr_بعدی != عدم; fp = fp->fr_بعدی)
	    {
		إذا (*colp < fp->fr_عرض)
		    اكسر;
		*colp -= fp->fr_عرض;
	    }
	}
	وإلا    // fr_layout == FR_COL
	{
	    لكل (fp = fp->fr_فرزند; fp->fr_بعدی != عدم; fp = fp->fr_بعدی)
	    {
		إذا (*rowp < fp->fr_ارتفاع)
		    اكسر;
		*rowp -= fp->fr_ارتفاع;
	    }
	}
    }
    // When using a timer that closes a window the window might not actually
    // exist.
    FOR_ALL_WINDOWS(wp)
	إذا (wp == fp->fr_win)
	{
#اگر_تعریف_شده FEAT_MENU
	    *rowp -= wp->w_winbar_ارتفاع;
#پایان_اگر
	    أرجع wp;
	}
    أرجع عدم;
}

#إذا defined(NEED_VCOL2_ستون) || defined(FEAT_BEVAL) || defined(FEAT_PROP_POPUP) \
	|| defined(FEAT_EVAL)
/*
 * Convert a virtual (screen) column to a character column.
 * The first column is zero.
 */
    صحيح
vcol2col(win_T *wp, linenr_T lnum, صحيح vcol, colnr_T *coladdp)
{
    char_u	    *سطر;
    chartabsize_T   cts;

    // try to advance to the specified column
    سطر = ml_get_buf(wp->w_میانگیر, lnum, FALSE);
    init_chartabsize_arg(&cts, wp, lnum, 0, سطر, سطر);
    طالما (cts.cts_vcol < vcol && *cts.cts_ptr != NUL)
    {
	صحيح اندازه = win_lbr_chartabsize(&cts, عدم, عدم);
	إذا (cts.cts_vcol + اندازه > vcol)
	    اكسر;
	cts.cts_vcol += اندازه;
	MB_PTR_ADV(cts.cts_ptr);
    }
    clear_chartabsize_arg(&cts);

    إذا (coladdp != عدم)
	*coladdp = vcol - cts.cts_vcol;
    أرجع (صحيح)(cts.cts_ptr - سطر);
}
#پایان_اگر

#إذا defined(FEAT_EVAL)
/*
 * "getmousepos()" function.
 */
    فراغ
f_getmousepos(typval_T *argvars استفادهنشده, typval_T *rettv)
{
    dict_T	*d;
    win_T	*wp;
    صحيح		ردیف = mouse_ردیف;
    صحيح		ستون = mouse_ستون;
    varnumber_T winid = 0;
    varnumber_T winrow = 0;
    varnumber_T wincol = 0;
    linenr_T	lnum = 0;
    varnumber_T ستونی = 0;
    colnr_T	coladd = 0;

    إذا (rettv_dict_alloc(rettv) == FAIL)
	أرجع;
    d = rettv->vval.v_dict;

    dict_جمع_کن_عدد(d, "screenrow", (varnumber_T)mouse_ردیف + 1);
    dict_جمع_کن_عدد(d, "screencol", (varnumber_T)mouse_ستون + 1);

    wp = mouse_پیدا_کن_win(&ردیف, &ستون, پیدا_کن_POPUP);
    إذا (wp != عدم)
    {
	صحيح	بالاترین_off = 0;
	صحيح	چپ_off = 0;
	صحيح	ارتفاع = wp->w_ارتفاع + wp->w_وضعیت_ارتفاع;

# ifdef FEAT_PROP_POPUP
	إذا (WIN_IS_POPUP(wp))
	{
	    بالاترین_off = popup_بالاترین_extra(wp);
	    چپ_off = popup_چپ_extra(wp);
	    ارتفاع = popup_ارتفاع(wp);
	}
# endif
	إذا (ردیف < ارتفاع)
	{
	    winid = wp->w_id;
	    winrow = ردیف + 1;
	    wincol = ستون + 1;
	    ردیف -= بالاترین_off;
	    ستون -= چپ_off;
	    إذا (ردیف >= 0 && ردیف < wp->w_ارتفاع && ستون >= 0 && ستون < wp->w_عرض)
	    {
		(فراغ)mouse_comp_pos(wp, &ردیف, &ستون, &lnum, عدم);
		ستون = vcol2col(wp, lnum, ستون, &coladd);
		ستونی = ستون + 1;
	    }
	}
    }
    dict_جمع_کن_عدد(d, "winid", winid);
    dict_جمع_کن_عدد(d, "winrow", winrow);
    dict_جمع_کن_عدد(d, "wincol", wincol);
    dict_جمع_کن_عدد(d, "line", (varnumber_T)lnum);
    dict_جمع_کن_عدد(d, "column", ستونی);
    dict_جمع_کن_عدد(d, "coladd", coladd);
}
#پایان_اگر
