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

#అన్తర్భావ "vim.h"

/*
 * Horiziontal and vertical steps used when scrolling.
 * When negative scroll by a whole page.
 */
స్థిర దీర్ఘ mouse_hor_అడుగు = 6;
స్థిర దీర్ఘ mouse_vert_అడుగు = 3;
స్థిర win_T *dragwin = ఖాళీ;	// window being dragged
స్థిర పూర్ణాంకం stl_click_handler(win_T *wp, పూర్ణాంకం mrow, పూర్ణాంకం mcol, పూర్ణాంకం which_button,
								    పూర్ణాంకం mods);
స్థిర పూర్ణాంకం stl_click_handler_regions(stl_click_region_T *regions,
				    పూర్ణాంకం region_లెక్క, పూర్ణాంకం winid,
				    char_u *area_name, పూర్ణాంకం mrow, పూర్ణాంకం mcol,
				    పూర్ణాంకం which_button, పూర్ణాంకం mods);

    శూన్య
mouse_సమితి_vert_scroll_అడుగు(దీర్ఘ అడుగు)
{
    mouse_vert_అడుగు = అడుగు;
}

    శూన్య
mouse_సమితి_hor_scroll_అడుగు(దీర్ఘ అడుగు)
{
    mouse_hor_అడుగు = అడుగు;
}

#నిర్వచితమైతే CHECK_DOUBLE_CLICK
/*
 * Return the duration from t1 to t2 in milliseconds.
 */
    స్థిర దీర్ఘ
సమయం_diff_ms(నిర్మాణం timeval *t1, నిర్మాణం timeval *t2)
{
    // This handles wrapping of tv_usec correctly without any special case.
    // Example of 2 pairs (tv_sec, tv_usec) with a duration of 5 ms:
    //	   t1 = (1, 998000) t2 = (2, 3000) gives:
    //	   (2 - 1) * 1000 + (3000 - 998000) / 1000 -> 5 ms.
    తిరిగి (t2->tv_sec - t1->tv_sec) * 1000
	 + (t2->tv_usec - t1->tv_usec) / 1000;
}
#అయితే_ముగింపు

/*
 * Get class of a character for selection: same class means same word.
 * 0: blank
 * 1: punctuation groups
 * 2: normal word character
 * >2: multi-byte word character.
 */
    స్థిర పూర్ణాంకం
get_mouse_class(char_u *p)
{
    పూర్ణాంకం		c;

    ఒకవేళ (has_mbyte && MB_BYTE2LEN(p[0]) > 1)
	తిరిగి mb_get_class(p);

    c = *p;
    ఒకవేళ (c == ' ' || c == '\t')
	తిరిగి 0;

    ఒకవేళ (vim_iswordc(c))
	తిరిగి 2;

    // There are a few special cases where we want certain combinations of
    // characters to be considered as a single word.  These are things like
    // "->", "/ *", "*=", "+=", "&=", "<=", ">=", "!=" etc.  Otherwise, each
    // character is in its own class.
    ఒకవేళ (c != NUL && vim_strchr((char_u *)"-+*/%<>&|^!=", c) != ఖాళీ)
	తిరిగి 1;
    తిరిగి c;
}

/*
 * Move "pos" back to the start of the word it's in.
 */
    స్థిర శూన్య
కనుగొను_ప్రారంభం_యొక్క_మాట(pos_T *pos)
{
    char_u	*పంక్తి;
    పూర్ణాంకం		cclass;
    పూర్ణాంకం		నిలువు;

    పంక్తి = ml_get(pos->lnum);
    cclass = get_mouse_class(పంక్తి + pos->నిలువు);

    అయితే (pos->నిలువు > 0)
    {
	నిలువు = pos->నిలువు - 1;
	నిలువు -= (*mb_తల_off)(పంక్తి, పంక్తి + నిలువు);
	ఒకవేళ (get_mouse_class(పంక్తి + నిలువు) != cclass)
	    ఆపు;
	pos->నిలువు = నిలువు;
    }
}

/*
 * Move "pos" forward to the end of the word it's in.
 * When 'selection' is "exclusive", the position is just after the word.
 */
    స్థిర శూన్య
కనుగొను_చివర_యొక్క_మాట(pos_T *pos)
{
    char_u	*పంక్తి;
    పూర్ణాంకం		cclass;
    పూర్ణాంకం		నిలువు;

    పంక్తి = ml_get(pos->lnum);
    ఒకవేళ (*p_sel == 'e' && pos->నిలువు > 0)
    {
	--pos->నిలువు;
	pos->నిలువు -= (*mb_తల_off)(పంక్తి, పంక్తి + pos->నిలువు);
    }
    cclass = get_mouse_class(పంక్తి + pos->నిలువు);
    అయితే (పంక్తి[pos->నిలువు] != NUL)
    {
	నిలువు = pos->నిలువు + (*mb_ptr2len)(పంక్తి + pos->నిలువు);
	ఒకవేళ (get_mouse_class(పంక్తి + నిలువు) != cclass)
	{
	    ఒకవేళ (*p_sel == 'e')
		pos->నిలువు = నిలువు;
	    ఆపు;
	}
	pos->నిలువు = నిలువు;
    }
}

#ఒకవేళ defined(FEAT_GUI_MOTIF) || defined(FEAT_GUI_GTK) \
	    || defined(FEAT_GUI_MSWIN) \
	    || defined(FEAT_GUI_PHOTON) \
	    || defined(FEAT_పదం_POPUP_MENU)
# define USE_POPUP_SETPOS
# define NEED_VCOL2_నిలువు

/*
 * Translate window coordinates to buffer position without any side effects.
 * Returns IN_BUFFER and sets "mpos->col" to the column when in buffer text.
 * The column is one for the first column.
 */
    స్థిర పూర్ణాంకం
get_fpos_యొక్క_mouse(pos_T *mpos)
{
    win_T	*wp;
    పూర్ణాంకం		అడ్డువరుస = mouse_అడ్డువరుస;
    పూర్ణాంకం		నిలువు = mouse_నిలువు;

    ఒకవేళ (అడ్డువరుస < 0 || నిలువు < 0)		// check if it makes sense
	తిరిగి లోపల_UNKNOWN;

    // find the window where the row is in
    wp = mouse_కనుగొను_win(&అడ్డువరుస, &నిలువు, FAIL_POPUP);
    ఒకవేళ (wp == ఖాళీ)
	తిరిగి లోపల_UNKNOWN;
    // winpos and height may change in win_enter()!
    ఒకవేళ (అడ్డువరుస >= wp->w_ఎత్తు)	// In (or below) status line
	తిరిగి లోపల_స్థితి_పంక్తి;
    ఒకవేళ (నిలువు >= wp->w_వెడల్పు)	// In vertical separator line
	తిరిగి లోపల_SEP_పంక్తి;

    ఒకవేళ (wp != curwin)
	తిరిగి లోపల_UNKNOWN;

    // compute the position in the buffer line from the posn on the screen
    ఒకవేళ (mouse_comp_pos(curwin, &అడ్డువరుస, &నిలువు, &mpos->lnum, ఖాళీ))
	తిరిగి లోపల_స్థితి_పంక్తి; // past bottom

    mpos->నిలువు = vcol2col(wp, mpos->lnum, నిలువు, &mpos->coladd);
    తిరిగి లోపల_బఫరు;
}
#అయితే_ముగింపు

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
    పూర్ణాంకం
do_mouse(
    oparg_T	*oap,		// operator argument, can be NULL
    పూర్ణాంకం		c,		// K_LEFTMOUSE, etc
    పూర్ణాంకం		dir,		// Direction to 'put' if necessary
    దీర్ఘ	లెక్క,
    పూర్ణాంకం		fixindent)	// PUT_FIXINDENT if fixing indent necessary
{
    స్థిర పూర్ణాంకం	do_always = FALSE;	// ignore 'mouse' setting next time
    స్థిర పూర్ణాంకం	got_click = FALSE;	// got a click some time back

    పూర్ణాంకం		which_button;	// MOUSE_LEFT, _MIDDLE or _RIGHT
    పూర్ణాంకం		is_click = FALSE; // If FALSE it's a drag or release event
    పూర్ణాంకం		is_drag = FALSE;  // If TRUE it's a drag event
    పూర్ణాంకం		jump_flags = 0;	// flags for jump_to_mouse()
    pos_T	ప్రారంభం_visual;
    పూర్ణాంకం		moved;		// Has cursor moved?
    పూర్ణాంకం		లోపల_స్థితి_పంక్తి_2;	// mouse in status line
    స్థిర పూర్ణాంకం	లోపల_tab_పంక్తి = FALSE; // mouse clicked in tab line
    స్థిర పూర్ణాంకం	లోపల_tabpanel = FALSE; // mouse clicked in tabpanel
#నిర్వచితమైతే FEAT_TABPANEL
    స్థిర bool	లోపల_tabpanel_scrollbar = false; // dragging tabpanel scrollbar
#అయితే_ముగింపు
    పూర్ణాంకం		లోపల_sep_పంక్తి;	// mouse in vertical separator line
    పూర్ణాంకం		c1, c2;
#ఒకవేళ defined(FEAT_FOLDING)
    pos_T	save_cursor;
#అయితే_ముగింపు
    win_T	*old_curwin = curwin;
    స్థిర pos_T orig_cursor;
    colnr_T	leftcol, rightcol;
    pos_T	చివర_visual;
    పూర్ణాంకం		diff;
    పూర్ణాంకం		old_చురుకైన = VIsual_చురుకైన;
    పూర్ణాంకం		old_బహుళకం = VIsual_బహుళకం;
    పూర్ణాంకం		regname;

#ఒకవేళ defined(FEAT_FOLDING)
    save_cursor = curwin->w_cursor;
#అయితే_ముగింపు

    // When GUI is active, always recognize mouse events, otherwise:
    // - Ignore mouse event in normal mode if 'mouse' doesn't include 'n'.
    // - Ignore mouse event in visual mode if 'mouse' doesn't include 'v'.
    // - For command line and insert mode 'mouse' is checked before calling
    //	 do_mouse().
    ఒకవేళ (do_always)
	do_always = FALSE;
    లేకపోతే
#నిర్వచితమైతే FEAT_GUI
	ఒకవేళ (!gui.లోపల_use)
#అయితే_ముగింపు
	{
	    ఒకవేళ (VIsual_చురుకైన)
	    {
		ఒకవేళ (!mouse_has(MOUSE_VISUAL))
		    తిరిగి FALSE;
	    }
	    లేకపోతే ఒకవేళ (State == బహుళకం_సాధారణ && !mouse_has(MOUSE_సాధారణ))
		తిరిగి FALSE;
	}

    కొరకు (;;)
    {
	which_button = get_mouse_button(KEY2TERMCAP1(c), &is_click, &is_drag);
	ఒకవేళ (is_drag)
	{
	    // If the next character is the same mouse event then use that
	    // one. Speeds up dragging the status line.
	    // Note: Since characters added to the stuff buffer in the code
	    // below need to come before the next character, do not do this
	    // when the current character was stuffed.
	    ఒకవేళ (!కీలకం_Stuffed && vpeekc() != NUL)
	    {
		పూర్ణాంకం nc;
		పూర్ణాంకం save_mouse_అడ్డువరుస = mouse_అడ్డువరుస;
		పూర్ణాంకం save_mouse_నిలువు = mouse_నిలువు;

		// Need to get the character, peeking doesn't get the actual
		// one.
		nc = సురక్షిత_vgetc();
		ఒకవేళ (c == nc)
		    కొనసాగు;
		vungetc(nc);
		mouse_అడ్డువరుస = save_mouse_అడ్డువరుస;
		mouse_నిలువు = save_mouse_నిలువు;
	    }
	}
	ఆపు;
    }

    ఒకవేళ (c == K_MOUSEMOVE)
    {
	// Mouse moved without a button pressed.
#నిర్వచితమైతే FEAT_BEVAL_పదం
	ui_may_remove_balloon();
	ఒకవేళ (p_bevalterm)
	{
	    profile_setlimit(p_bdlay, &bevalexpr_due);
	    bevalexpr_due_సమితి = TRUE;
	}
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_PROP_POPUP
	popup_handle_mouse_moved();
#అయితే_ముగింపు
	తిరిగి FALSE;
    }

#నిర్వచితమైతే FEAT_MOUSESHAPE
    // May have stopped dragging the status or separator line.  The pointer is
    // most likely still on the status or separator line.
    ఒకవేళ (!is_drag && drag_స్థితి_పంక్తి)
    {
	drag_స్థితి_పంక్తి = FALSE;
	update_mouseshape(SHAPE_IDX_స్థితి);
    }
    ఒకవేళ (!is_drag && drag_sep_పంక్తి)
    {
	drag_sep_పంక్తి = FALSE;
	update_mouseshape(SHAPE_IDX_VSEP);
    }
#అయితే_ముగింపు

    // Ignore drag and release events if we didn't get a click.
    ఒకవేళ (is_click)
    {
	got_click = TRUE;
	లోపల_tab_పంక్తి = FALSE;
	లోపల_tabpanel = FALSE;
#నిర్వచితమైతే FEAT_TABPANEL
	లోపల_tabpanel_scrollbar = false;
#అయితే_ముగింపు
    }
    లేకపోతే
    {
	ఒకవేళ (!got_click)			// didn't get click, ignore
	    తిరిగి FALSE;
	ఒకవేళ (!is_drag)			// release, reset got_click
	{
	    got_click = FALSE;
	    ఒకవేళ (లోపల_tab_పంక్తి || లోపల_tabpanel
#నిర్వచితమైతే FEAT_TABPANEL
		    || లోపల_tabpanel_scrollbar
#అయితే_ముగింపు
		    )
	    {
		లోపల_tab_పంక్తి = FALSE;
		లోపల_tabpanel = FALSE;
#నిర్వచితమైతే FEAT_TABPANEL
		లోపల_tabpanel_scrollbar = false;
#అయితే_ముగింపు
		తిరిగి FALSE;
	    }
	}
    }

#నిర్వచితమైతే FEAT_TABPANEL
    // Continue a scrollbar drag before any tab-selection handling.
    ఒకవేళ (is_drag && లోపల_tabpanel_scrollbar)
    {
	tabpanel_drag_scrollbar(mouse_అడ్డువరుస);
	తిరిగి FALSE;
    }
#అయితే_ముగింపు

    // CTRL right mouse button does CTRL-T
    ఒకవేళ (is_click && (mod_ముసుగు & MOD_ముసుగు_CTRL) && which_button == MOUSE_కుడి)
    {
	ఒకవేళ (State & బహుళకం_చొప్పించు)
	    stuffcharReadbuff(Ctrl_O);
	ఒకవేళ (లెక్క > 1)
	    stuffnumReadbuff(లెక్క);
	stuffcharReadbuff(Ctrl_T);
	got_click = FALSE;		// ignore drag&release now
	తిరిగి FALSE;
    }

    // CTRL only works with left mouse button
    ఒకవేళ ((mod_ముసుగు & MOD_ముసుగు_CTRL) && which_button != MOUSE_ఎడమ)
	తిరిగి FALSE;

    // When a modifier is down, ignore drag and release events, as well as
    // multiple clicks and the middle mouse button.
    // Accept shift-leftmouse drags when 'mousemodel' is "popup.*".
    ఒకవేళ ((mod_ముసుగు & (MOD_ముసుగు_జరుపు | MOD_ముసుగు_CTRL | MOD_ముసుగు_ALT
							     | MOD_ముసుగు_META))
	    && (!is_click
		|| (mod_ముసుగు & MOD_ముసుగు_MULTI_CLICK)
		|| which_button == MOUSE_మధ్య)
	    && !((mod_ముసుగు & (MOD_ముసుగు_జరుపు|MOD_ముసుగు_ALT))
		&& mouse_model_popup()
		&& which_button == MOUSE_ఎడమ)
	    && !((mod_ముసుగు & MOD_ముసుగు_ALT)
		&& !mouse_model_popup()
		&& which_button == MOUSE_కుడి)
	    )
	తిరిగి FALSE;

    // If the button press was used as the movement command for an operator
    // (eg "d<MOUSE>"), or it is the middle button that is held down, ignore
    // drag/release events.
    ఒకవేళ (!is_click && which_button == MOUSE_మధ్య)
	తిరిగి FALSE;

    ఒకవేళ (oap != ఖాళీ)
	regname = oap->regname;
    లేకపోతే
	regname = 0;

    // Middle mouse button does a 'put' of the selected text
    ఒకవేళ (which_button == MOUSE_మధ్య)
    {
	ఒకవేళ (State == బహుళకం_సాధారణ)
	{
	    // If an operator was pending, we don't know what the user wanted
	    // to do. Go back to normal mode: Clear the operator and beep().
	    ఒకవేళ (oap != ఖాళీ && oap->op_type != OP_NOP)
	    {
		clearopbeep(oap);
		తిరిగి FALSE;
	    }

	    // If visual was active, yank the highlighted text and put it
	    // before the mouse pointer position.
	    // In Select mode replace the highlighted text with the clipboard.
	    ఒకవేళ (VIsual_చురుకైన)
	    {
		ఒకవేళ (VIsual_select)
		{
		    stuffcharReadbuff(Ctrl_G);
		    stuffReadbuff((char_u *)"\"+p");
		}
		లేకపోతే
		{
		    stuffcharReadbuff('y');
		    stuffcharReadbuff(K_MIDDLEMOUSE);
		}
		do_always = TRUE;	// ignore 'mouse' setting next time
		తిరిగి FALSE;
	    }
	    // The rest is below jump_to_mouse()
	}

	లేకపోతే ఒకవేళ ((State & బహుళకం_చొప్పించు) == 0)
	    తిరిగి FALSE;

	// Middle click in insert mode doesn't move the mouse, just insert the
	// contents of a register.  '.' register is special, can't insert that
	// with do_put().
	// Also paste at the cursor if the current mode isn't in 'mouse' (only
	// happens for the GUI).
	ఒకవేళ ((State & బహుళకం_చొప్పించు) || !mouse_has(MOUSE_సాధారణ))
	{
	    ఒకవేళ (regname == '.')
		చొప్పించు_reg(regname, TRUE);
	    లేకపోతే
	    {
#నిర్వచితమైతే FEAT_CLIPBOARD
		ఒకవేళ (clip_నక్షత్రం.అందుబాటు && regname == 0)
		    regname = '*';
#అయితే_ముగింపు
		ఒకవేళ ((State & REPLACE_FLAG) && !yank_రిజిస్టరు_mline(regname))
		    చొప్పించు_reg(regname, TRUE);
		లేకపోతే
		{
		    do_put(regname, ఖాళీ, BACKWARD, 1L,
						      fixindent | PUT_CURSEND);

		    // Repeat it with CTRL-R CTRL-O r or CTRL-R CTRL-P r
		    Append_Char_గమ్యం_Redobuff(Ctrl_R);
		    Append_Char_గమ్యం_Redobuff(fixindent ? Ctrl_P : Ctrl_O);
		    Append_Char_గమ్యం_Redobuff(regname == 0 ? '"' : regname);
		}
	    }
	    తిరిగి FALSE;
	}
    }

    // When dragging or button-up stay in the same window.
    ఒకవేళ (!is_click)
	jump_flags |= MOUSE_FOCUS | MOUSE_DID_MOVE;

    ప్రారంభం_visual.lnum = 0;

    నిర్మాణం tabpage_label_info {
	bool is_panel;	    // label type. true: tabpanel, false: tab line
	bool just_లోపల;	    // just in tabpage label area
	bool just_click;    // just click tabpage label area
	పూర్ణాంకం nr;		    // tabpage number
    } tp_label = { false, false, false, 0 };

    // Check for clicking in the tab page panel.
#ఒకవేళ defined(FEAT_TABPANEL)
    ఒకవేళ (mouse_నిలువు < firstwin->w_wincol
		|| mouse_నిలువు >= firstwin->w_wincol + topframe->fr_వెడల్పు)
    {
	// A click on the scrollbar column starts a drag interaction and
	// preempts tab-selection.
	ఒకవేళ (is_click && !is_drag && mouse_on_tabpanel_scrollbar())
	{
	    లోపల_tabpanel_scrollbar = TRUE;
	    tabpanel_drag_scrollbar(mouse_అడ్డువరుస);
	    తిరిగి FALSE;
	}

	// Dispatch 'tabpanel' %[FuncName] click regions before falling through
	// to tab-page selection.  On drag events fall through to the normal
	// tab-drag handling.
	ఒకవేళ (is_click && !is_drag
		&& stl_click_handler_regions(tabpanel_stl_click,
					    tabpanel_stl_click_లెక్క,
					    0, (char_u *)"tabpanel",
					    mouse_అడ్డువరుస, mouse_నిలువు,
					    which_button, mod_ముసుగు))
	    తిరిగి FALSE;

	tp_label.is_panel = true;
	tp_label.just_లోపల = true;
	tp_label.nr = get_tabpagenr_on_tabpanel();

	// click in a tab selects that tab page
	ఒకవేళ (is_click && cmdwin_type == 0)
	    tp_label.just_click = true;
    }
    లేకపోతే
#అయితే_ముగింపు
    // Check for clicking in the tab page line.
    ఒకవేళ (Tab_పుట_Idxs != ఖాళీ && mouse_అడ్డువరుస == 0 && firstwin->w_winrow > 0)
    {
	// Dispatch 'tabline' %[FuncName] click regions before falling through
	// to tab-page selection.  On drag events fall through to the normal
	// tab-drag handling.
	ఒకవేళ (is_click && !is_drag
		&& stl_click_handler_regions(tabline_stl_click,
					    tabline_stl_click_లెక్క,
					    0, (char_u *)"tabline",
					    mouse_అడ్డువరుస, mouse_నిలువు,
					    which_button, mod_ముసుగు))
	    తిరిగి FALSE;

	tp_label.just_లోపల = true;
	tp_label.nr = Tab_పుట_Idxs[mouse_నిలువు];

	// click in a tab selects that tab page
	ఒకవేళ (is_click && cmdwin_type == 0
		&& mouse_నిలువు < firstwin->w_wincol + topframe->fr_వెడల్పు)
	    tp_label.just_click = true;
    }

    ఒకవేళ (tp_label.just_లోపల)
    {
	ఒకవేళ (is_drag)
	{
	    ఒకవేళ (లోపల_tabpanel || లోపల_tab_పంక్తి)
	    {
		c1 = tp_label.nr;
		tabpage_move(c1 <= 0 ? 9999 : c1 < tabpage_సూచీ(curtab)
							    ? c1 - 1 : c1);
	    }
	    తిరిగి FALSE;
	}

	ఒకవేళ (tp_label.just_click)
	{
	    ఒకవేళ (tp_label.is_panel)
		లోపల_tabpanel = TRUE;
	    లేకపోతే
		లోపల_tab_పంక్తి = TRUE;
	    c1 = tp_label.nr;
	    ఒకవేళ (c1 >= 0)
	    {
		ఒకవేళ ((mod_ముసుగు & MOD_ముసుగు_MULTI_CLICK) == MOD_ముసుగు_2_CLICK
						    && !tp_label.is_panel)
		{
		    // Double-click on the tabline opens a new, empty tab page.
		    // The tabpanel has no "empty area" (every row maps to a tab)
		    // and this behavior is not documented for tabpanel, so fall
		    // through to the regular tab-switch path there.
		    చివర_visual_బహుళకం_keep_button();
		    tabpage_new();
		    tabpage_move(c1 == 0 ? 9999 : c1 - 1);
		}
		లేకపోతే
		{
		    // Go to specified tab page, or next one if not clicking
		    // on a label.
		    goto_tabpage(c1);

		    // It's like clicking on the status line of a window.
		    ఒకవేళ (curwin != old_curwin)
			చివర_visual_బహుళకం_keep_button();
		}
	    }
	    లేకపోతే
	    {
		tabpage_T	*tp;

		// Close the current or specified tab page.
		ఒకవేళ (c1 == -999)
		    tp = curtab;
		లేకపోతే
		    tp = కనుగొను_tabpage(-c1);
		ఒకవేళ (tp == curtab)
		{
		    ఒకవేళ (మొదటి_tabpage->tp_తదుపరి != ఖాళీ)
			tabpage_close(FALSE);
		}
		లేకపోతే ఒకవేళ (tp != ఖాళీ)
		    tabpage_close_other(tp, FALSE);
	    }
	}
	తిరిగి TRUE;
    }
    లేకపోతే ఒకవేళ (is_drag && (లోపల_tabpanel || (లోపల_tab_పంక్తి && Tab_పుట_Idxs != ఖాళీ)))
    {
#ఒకవేళ defined(FEAT_TABPANEL)
	ఒకవేళ (లోపల_tabpanel)
	    c1 = get_tabpagenr_on_tabpanel();
	లేకపోతే
#అయితే_ముగింపు
	    c1 = Tab_పుట_Idxs[mouse_నిలువు];
	tabpage_move(c1 <= 0 ? 9999 : c1 - 1);
	తిరిగి FALSE;
    }

    // When 'mousemodel' is "popup" or "popup_setpos", translate mouse events:
    // right button up   -> pop-up menu
    // shift-left button -> right button
    // alt-left button   -> alt-right button
    ఒకవేళ (mouse_model_popup())
    {
	ఒకవేళ (which_button == MOUSE_కుడి
			    && !(mod_ముసుగు & (MOD_ముసుగు_జరుపు | MOD_ముసుగు_CTRL)))
	{
#నిర్వచితమైతే USE_POPUP_SETPOS
# ifdef FEAT_GUI
	    ఒకవేళ (gui.లోపల_use)
	    {
#  ఒకవేళ defined(FEAT_GUI_MOTIF) || defined(FEAT_GUI_GTK) \
			  || defined(FEAT_GUI_PHOTON)
		ఒకవేళ (!is_click)
		    // Ignore right button release events, only shows the popup
		    // menu on the button down event.
		    తిరిగి FALSE;
#  endif
#  ఒకవేళ defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_HAIKU)
		ఒకవేళ (is_click || is_drag)
		    // Ignore right button down and drag mouse events.  Windows
		    // only shows the popup menu on the button up event.
		    తిరిగి FALSE;
#  endif
	    }
# endif
# ఒకవేళ defined(FEAT_GUI) && defined(FEAT_పదం_POPUP_MENU)
	    లేకపోతే
# endif
# ఒకవేళ defined(FEAT_పదం_POPUP_MENU)
	    ఒకవేళ (!is_click)
		// Ignore right button release events, only shows the popup
		// menu on the button down event.
		తిరిగి FALSE;
# endif

	    jump_flags = 0;
	    ఒకవేళ (STRCMP(p_mousem, "popup_setpos") == 0)
	    {
		// First set the cursor position before showing the popup
		// menu.
		ఒకవేళ (VIsual_చురుకైన)
		{
		    pos_T    m_pos;

		    // set MOUSE_MAY_STOP_VIS if we are outside the
		    // selection or the current window (might have false
		    // negative here)
		    ఒకవేళ (mouse_అడ్డువరుస < curwin->w_winrow
			 || mouse_అడ్డువరుస
				  > (curwin->w_winrow + curwin->w_ఎత్తు))
			jump_flags = MOUSE_MAY_STOP_VIS;
		    లేకపోతే ఒకవేళ (get_fpos_యొక్క_mouse(&m_pos) != లోపల_బఫరు)
			jump_flags = MOUSE_MAY_STOP_VIS;
		    లేకపోతే
		    {
			ఒకవేళ (VIsual_బహుళకం == 'V')
			{
			    ఒకవేళ ((curwin->w_cursor.lnum <= VIsual.lnum
				    && (m_pos.lnum < curwin->w_cursor.lnum
					|| VIsual.lnum < m_pos.lnum))
				|| (VIsual.lnum < curwin->w_cursor.lnum
				    && (m_pos.lnum < VIsual.lnum
					|| curwin->w_cursor.lnum < m_pos.lnum)))
			    {
				jump_flags = MOUSE_MAY_STOP_VIS;
			    }
			}
			లేకపోతే ఒకవేళ ((LTOREQ_POS(curwin->w_cursor, VIsual)
				    && (LT_POS(m_pos, curwin->w_cursor)
					|| LT_POS(VIsual, m_pos)))
				|| (LT_POS(VIsual, curwin->w_cursor)
				    && (LT_POS(m_pos, VIsual)
				      || LT_POS(curwin->w_cursor, m_pos))))
			{
			    jump_flags = MOUSE_MAY_STOP_VIS;
			}
			లేకపోతే ఒకవేళ (VIsual_బహుళకం == Ctrl_V)
			{
			    getvcols(curwin, &curwin->w_cursor, &VIsual,
						     &leftcol, &rightcol, 0);
			    getvcol(curwin, &m_pos, ఖాళీ, &m_pos.నిలువు, ఖాళీ, 0);
			    ఒకవేళ (m_pos.నిలువు < leftcol || m_pos.నిలువు > rightcol)
				jump_flags = MOUSE_MAY_STOP_VIS;
			}
		    }
		}
		లేకపోతే
		    jump_flags = MOUSE_MAY_STOP_VIS;
	    }
	    ఒకవేళ (jump_flags)
	    {
		jump_flags = jump_గమ్యం_mouse(jump_flags, ఖాళీ, which_button);
		update_curbuf(VIsual_చురుకైన ? UPD_INVERTED : UPD_VALID);
		setcursor();
		బహిర్గతం_flush();    // Update before showing popup menu
	    }
# ifdef FEAT_MENU
	    show_popupmenu();
	    got_click = FALSE;	// ignore release events
# endif
	    తిరిగి (jump_flags & CURSOR_MOVED) != 0;
#లేకపోతే
	    తిరిగి FALSE;
#అయితే_ముగింపు
	}
	ఒకవేళ (which_button == MOUSE_ఎడమ
				&& (mod_ముసుగు & (MOD_ముసుగు_జరుపు|MOD_ముసుగు_ALT)))
	{
	    which_button = MOUSE_కుడి;
	    mod_ముసుగు &= ~MOD_ముసుగు_జరుపు;
	}
    }

    ఒకవేళ ((State & (బహుళకం_సాధారణ | బహుళకం_చొప్పించు))
			    && !(mod_ముసుగు & (MOD_ముసుగు_జరుపు | MOD_ముసుగు_CTRL)))
    {
	ఒకవేళ (which_button == MOUSE_ఎడమ)
	{
	    ఒకవేళ (is_click)
	    {
		// stop Visual mode for a left click in a window, but not when
		// on a status line
		ఒకవేళ (VIsual_చురుకైన)
		    jump_flags |= MOUSE_MAY_STOP_VIS;
	    }
	    లేకపోతే ఒకవేళ (mouse_has(MOUSE_VISUAL))
		jump_flags |= MOUSE_MAY_VIS;
	}
	లేకపోతే ఒకవేళ (which_button == MOUSE_కుడి)
	{
	    ఒకవేళ (is_click && VIsual_చురుకైన)
	    {
		// Remember the start and end of visual before moving the
		// cursor.
		ఒకవేళ (LT_POS(curwin->w_cursor, VIsual))
		{
		    ప్రారంభం_visual = curwin->w_cursor;
		    చివర_visual = VIsual;
		}
		లేకపోతే
		{
		    ప్రారంభం_visual = VIsual;
		    చివర_visual = curwin->w_cursor;
		}
	    }
	    jump_flags |= MOUSE_FOCUS;
	    ఒకవేళ (mouse_has(MOUSE_VISUAL))
		jump_flags |= MOUSE_MAY_VIS;
	}
    }

    // If an operator is pending, ignore all drags and releases until the
    // next mouse click.
    ఒకవేళ (!is_drag && oap != ఖాళీ && oap->op_type != OP_NOP)
    {
	got_click = FALSE;
	oap->motion_type = MCHAR;
    }

    // When releasing the button let jump_to_mouse() know.
    ఒకవేళ (!is_click && !is_drag)
	jump_flags |= MOUSE_RELEASED;

    // JUMP!
    jump_flags = jump_గమ్యం_mouse(jump_flags,
			oap == ఖాళీ ? ఖాళీ : &(oap->inclusive), which_button);

#నిర్వచితమైతే FEAT_MENU
    // A click in the window toolbar has no side effects.
    ఒకవేళ (jump_flags & MOUSE_WINBAR)
	తిరిగి FALSE;
#అయితే_ముగింపు
    moved = (jump_flags & CURSOR_MOVED);
    లోపల_స్థితి_పంక్తి_2 = (jump_flags & లోపల_స్థితి_పంక్తి);
    లోపల_sep_పంక్తి = (jump_flags & లోపల_SEP_పంక్తి);

    // Check for statusline click handler early, before visual mode or
    // other button-specific handling can interfere.
    ఒకవేళ (లోపల_స్థితి_పంక్తి_2 && is_click && !is_drag
	    && stl_click_handler(dragwin, mouse_అడ్డువరుస, mouse_నిలువు,
						which_button, mod_ముసుగు))
    {
#నిర్వచితమైతే FEAT_MOUSESHAPE
	ఒకవేళ (!drag_స్థితి_పంక్తి)
	{
	    drag_స్థితి_పంక్తి = TRUE;
	    update_mouseshape(-1);
	}
#అయితే_ముగింపు
	తిరిగి FALSE;
    }

#నిర్వచితమైతే FEAT_NETBEANS_INTG
    ఒకవేళ (is_Netbeans_బఫరు(curbuf)
			    && !(jump_flags & (లోపల_స్థితి_పంక్తి | లోపల_SEP_పంక్తి)))
    {
	పూర్ణాంకం కీలకం = KEY2TERMCAP1(c);

	ఒకవేళ (కీలకం == (పూర్ణాంకం)KE_LEFTRELEASE || కీలకం == (పూర్ణాంకం)KE_MIDDLERELEASE
					       || కీలకం == (పూర్ణాంకం)KE_RIGHTRELEASE)
	    netbeans_button_release(which_button);
    }
#అయితే_ముగింపు

    // When jumping to another window, clear a pending operator.  That's a bit
    // friendlier than beeping and not jumping to that window.
    ఒకవేళ (curwin != old_curwin && oap != ఖాళీ && oap->op_type != OP_NOP)
	clearop(oap);

#నిర్వచితమైతే FEAT_FOLDING
    ఒకవేళ (mod_ముసుగు == 0
	    && !is_drag
	    && (jump_flags & (MOUSE_FOLD_CLOSE | MOUSE_FOLD_OPEN))
	    && which_button == MOUSE_ఎడమ)
    {
	// open or close a fold at this line
	ఒకవేళ (jump_flags & MOUSE_FOLD_OPEN)
	    openFold(curwin->w_cursor.lnum, 1L);
	లేకపోతే
	    closeFold(curwin->w_cursor.lnum, 1L);
	// don't move the cursor if still in the same window
	ఒకవేళ (curwin == old_curwin)
	    curwin->w_cursor = save_cursor;
    }
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_CLIPBOARD)
    ఒకవేళ ((jump_flags & లోపల_OTHER_WIN) && !VIsual_చురుకైన &&
	    (clip_నక్షత్రం.అందుబాటు || clip_plus.అందుబాటు))
    {
	clip_modeless(which_button, is_click, is_drag);
	తిరిగి FALSE;
    }
#అయితే_ముగింపు

    // Set global flag that we are extending the Visual area with mouse
    // dragging; temporarily minimize 'scrolloff'.
    ఒకవేళ (VIsual_చురుకైన && is_drag && get_scrolloff_విలువ())
    {
	// In the very first line, allow scrolling one line
	ఒకవేళ (mouse_అడ్డువరుస == 0)
	    mouse_dragging = 2;
	లేకపోతే
	    mouse_dragging = 1;
    }

    // When dragging the mouse above the window, scroll down.
    ఒకవేళ (is_drag && mouse_అడ్డువరుస < 0 && !లోపల_స్థితి_పంక్తి_2)
    {
	scroll_redraw(FALSE, 1L);
	mouse_అడ్డువరుస = 0;
    }

    ఒకవేళ (ప్రారంభం_visual.lnum)		// right click in visual mode
    {
       // When ALT is pressed make Visual mode blockwise.
       ఒకవేళ (mod_ముసుగు & MOD_ముసుగు_ALT)
	   VIsual_బహుళకం = Ctrl_V;

	// In Visual-block mode, divide the area in four, pick up the corner
	// that is in the quarter that the cursor is in.
	ఒకవేళ (VIsual_బహుళకం == Ctrl_V)
	{
	    getvcols(curwin, &ప్రారంభం_visual, &చివర_visual,
						    &leftcol, &rightcol, 0);
	    ఒకవేళ (curwin->w_curswant > (leftcol + rightcol) / 2)
		చివర_visual.నిలువు = leftcol;
	    లేకపోతే
		చివర_visual.నిలువు = rightcol;
	    ఒకవేళ (curwin->w_cursor.lnum >=
				    (ప్రారంభం_visual.lnum + చివర_visual.lnum) / 2)
		చివర_visual.lnum = ప్రారంభం_visual.lnum;

	    // move VIsual to the right column
	    ప్రారంభం_visual = curwin->w_cursor;	    // save the cursor pos
	    curwin->w_cursor = చివర_visual;
	    coladvance(చివర_visual.నిలువు);
	    VIsual = curwin->w_cursor;
	    curwin->w_cursor = ప్రారంభం_visual;	    // restore the cursor
	}
	లేకపోతే
	{
	    // If the click is before the start of visual, change the start.
	    // If the click is after the end of visual, change the end.  If
	    // the click is inside the visual, change the closest side.
	    ఒకవేళ (LT_POS(curwin->w_cursor, ప్రారంభం_visual))
		VIsual = చివర_visual;
	    లేకపోతే ఒకవేళ (LT_POS(చివర_visual, curwin->w_cursor))
		VIsual = ప్రారంభం_visual;
	    లేకపోతే
	    {
		// In the same line, compare column number
		ఒకవేళ (చివర_visual.lnum == ప్రారంభం_visual.lnum)
		{
		    ఒకవేళ (curwin->w_cursor.నిలువు - ప్రారంభం_visual.నిలువు >
				    చివర_visual.నిలువు - curwin->w_cursor.నిలువు)
			VIsual = ప్రారంభం_visual;
		    లేకపోతే
			VIsual = చివర_visual;
		}

		// In different lines, compare line number
		లేకపోతే
		{
		    diff = (curwin->w_cursor.lnum - ప్రారంభం_visual.lnum) -
				(చివర_visual.lnum - curwin->w_cursor.lnum);

		    ఒకవేళ (diff > 0)		// closest to end
			VIsual = ప్రారంభం_visual;
		    లేకపోతే ఒకవేళ (diff < 0)	// closest to start
			VIsual = చివర_visual;
		    లేకపోతే			// in the middle line
		    {
			ఒకవేళ (curwin->w_cursor.నిలువు <
					(ప్రారంభం_visual.నిలువు + చివర_visual.నిలువు) / 2)
			    VIsual = చివర_visual;
			లేకపోతే
			    VIsual = ప్రారంభం_visual;
		    }
		}
	    }
	}
    }
    // If Visual mode started in insert mode, execute "CTRL-O"
    లేకపోతే ఒకవేళ ((State & బహుళకం_చొప్పించు) && VIsual_చురుకైన)
	stuffcharReadbuff(Ctrl_O);

    // Middle mouse click: Put text before cursor.
    ఒకవేళ (which_button == MOUSE_మధ్య)
    {
#నిర్వచితమైతే FEAT_CLIPBOARD
	ఒకవేళ (clip_నక్షత్రం.అందుబాటు && regname == 0)
	    regname = '*';
#అయితే_ముగింపు
	ఒకవేళ (yank_రిజిస్టరు_mline(regname))
	{
	    ఒకవేళ (mouse_past_bottom)
		dir = FORWARD;
	}
	లేకపోతే ఒకవేళ (mouse_past_eol)
	    dir = FORWARD;

	ఒకవేళ (fixindent)
	{
	    c1 = (dir == BACKWARD) ? '[' : ']';
	    c2 = 'p';
	}
	లేకపోతే
	{
	    c1 = (dir == FORWARD) ? 'p' : 'P';
	    c2 = NUL;
	}
	prep_redo(regname, లెక్క, NUL, c1, NUL, c2, NUL);

	// Remember where the paste started, so in edit() Insstart can be set
	// to this position
	ఒకవేళ (restart_edit != 0)
	    where_paste_started = curwin->w_cursor;
	do_put(regname, ఖాళీ, dir, లెక్క, fixindent | PUT_CURSEND);
    }

#ఒకవేళ defined(FEAT_QUICKFIX)
    // Ctrl-Mouse click or double click in a quickfix window jumps to the
    // error under the mouse pointer.
    లేకపోతే ఒకవేళ (((mod_ముసుగు & MOD_ముసుగు_CTRL)
		|| (mod_ముసుగు & MOD_ముసుగు_MULTI_CLICK) == MOD_ముసుగు_2_CLICK)
	    && bt_quickfix(curbuf))
    {
	ఒకవేళ (curwin->w_llist_ref == ఖాళీ)	// quickfix window
	    do_cmdline_cmd((char_u *)".cc");
	లేకపోతే					// location list window
	    do_cmdline_cmd((char_u *)".ll");
	got_click = FALSE;		// ignore drag&release now
    }
#అయితే_ముగింపు

    // Ctrl-Mouse click (or double click in a help window) jumps to the tag
    // under the mouse pointer.
    లేకపోతే ఒకవేళ ((mod_ముసుగు & MOD_ముసుగు_CTRL) || (curbuf->b_help
		     && (mod_ముసుగు & MOD_ముసుగు_MULTI_CLICK) == MOD_ముసుగు_2_CLICK))
    {
	ఒకవేళ (State & బహుళకం_చొప్పించు)
	    stuffcharReadbuff(Ctrl_O);
	stuffcharReadbuff(Ctrl_RSB);
	got_click = FALSE;		// ignore drag&release now
    }

    // Shift-Mouse click searches for the next occurrence of the word under
    // the mouse pointer
    లేకపోతే ఒకవేళ ((mod_ముసుగు & MOD_ముసుగు_జరుపు))
    {
	ఒకవేళ ((State & బహుళకం_చొప్పించు) || (VIsual_చురుకైన && VIsual_select))
	    stuffcharReadbuff(Ctrl_O);
	ఒకవేళ (which_button == MOUSE_ఎడమ)
	    stuffcharReadbuff('*');
	లేకపోతే	// MOUSE_RIGHT
	    stuffcharReadbuff('#');
    }

    // Handle double clicks, unless on status line
    లేకపోతే ఒకవేళ (లోపల_స్థితి_పంక్తి_2)
    {
#నిర్వచితమైతే FEAT_MOUSESHAPE
	ఒకవేళ ((is_drag || is_click) && !drag_స్థితి_పంక్తి)
	{
	    drag_స్థితి_పంక్తి = TRUE;
	    update_mouseshape(-1);
	}
#అయితే_ముగింపు
    }
    లేకపోతే ఒకవేళ (లోపల_sep_పంక్తి)
    {
#నిర్వచితమైతే FEAT_MOUSESHAPE
	ఒకవేళ ((is_drag || is_click) && !drag_sep_పంక్తి)
	{
	    drag_sep_పంక్తి = TRUE;
	    update_mouseshape(-1);
	}
#అయితే_ముగింపు
    }
    లేకపోతే ఒకవేళ ((mod_ముసుగు & MOD_ముసుగు_MULTI_CLICK)
				       && (State & (బహుళకం_సాధారణ | బహుళకం_చొప్పించు))
	     && mouse_has(MOUSE_VISUAL))
    {
	ఒకవేళ (is_click || !VIsual_చురుకైన)
	{
	    ఒకవేళ (VIsual_చురుకైన)
		orig_cursor = VIsual;
	    లేకపోతే
	    {
		check_visual_highlight();
		VIsual = curwin->w_cursor;
		orig_cursor = VIsual;
		VIsual_చురుకైన = TRUE;
		VIsual_reselect = TRUE;
		// start Select mode if 'selectmode' contains "mouse"
		may_ప్రారంభం_select('o');
		setmouse();
	    }
	    ఒకవేళ ((mod_ముసుగు & MOD_ముసుగు_MULTI_CLICK) == MOD_ముసుగు_2_CLICK)
	    {
		// Double click with ALT pressed makes it blockwise.
		ఒకవేళ (mod_ముసుగు & MOD_ముసుగు_ALT)
		    VIsual_బహుళకం = Ctrl_V;
		లేకపోతే
		    VIsual_బహుళకం = 'v';
	    }
	    లేకపోతే ఒకవేళ ((mod_ముసుగు & MOD_ముసుగు_MULTI_CLICK) == MOD_ముసుగు_3_CLICK)
		VIsual_బహుళకం = 'V';
	    లేకపోతే ఒకవేళ ((mod_ముసుగు & MOD_ముసుగు_MULTI_CLICK) == MOD_ముసుగు_4_CLICK)
		VIsual_బహుళకం = Ctrl_V;
#నిర్వచితమైతే FEAT_CLIPBOARD
	    // Make sure the clipboard gets updated.  Needed because start and
	    // end may still be the same, and the selection needs to be owned
	    clip_నక్షత్రం.vmode = NUL;
#అయితే_ముగింపు
	}
	// A double click selects a word or a block.
	ఒకవేళ ((mod_ముసుగు & MOD_ముసుగు_MULTI_CLICK) == MOD_ముసుగు_2_CLICK)
	{
	    pos_T	*pos = ఖాళీ;
	    పూర్ణాంకం		gc;

	    ఒకవేళ (is_click)
	    {
		// If the character under the cursor (skipping white space) is
		// not a word character, try finding a match and select a (),
		// {}, [], #if/#endif, etc. block.
		చివర_visual = curwin->w_cursor;
		అయితే (gc = gchar_pos(&చివర_visual), VIM_ISWHITE(gc))
		    inc(&చివర_visual);
		ఒకవేళ (oap != ఖాళీ)
		    oap->motion_type = MCHAR;
		ఒకవేళ (oap != ఖాళీ
			&& VIsual_బహుళకం == 'v'
			&& !vim_iswordc(gchar_pos(&చివర_visual))
			&& EQUAL_POS(curwin->w_cursor, VIsual)
			&& (pos = findmatch(oap, NUL)) != ఖాళీ)
		{
		    curwin->w_cursor = *pos;
		    ఒకవేళ (oap->motion_type == MLINE)
			VIsual_బహుళకం = 'V';
		    లేకపోతే ఒకవేళ (*p_sel == 'e')
		    {
			ఒకవేళ (LT_POS(curwin->w_cursor, VIsual))
			    ++VIsual.నిలువు;
			లేకపోతే
			    ++curwin->w_cursor.నిలువు;
		    }
		}
	    }

	    ఒకవేళ (pos == ఖాళీ && (is_click || is_drag))
	    {
		// When not found a match or when dragging: extend to include
		// a word.
		ఒకవేళ (LT_POS(curwin->w_cursor, orig_cursor))
		{
		    కనుగొను_ప్రారంభం_యొక్క_మాట(&curwin->w_cursor);
		    కనుగొను_చివర_యొక్క_మాట(&VIsual);
		}
		లేకపోతే
		{
		    కనుగొను_ప్రారంభం_యొక్క_మాట(&VIsual);
		    ఒకవేళ (*p_sel == 'e' && *ml_get_cursor() != NUL)
			curwin->w_cursor.నిలువు +=
					 (*mb_ptr2len)(ml_get_cursor());
		    కనుగొను_చివర_యొక్క_మాట(&curwin->w_cursor);
		}
	    }
	    curwin->w_సమితి_curswant = true;
	}
	ఒకవేళ (is_click)
	    redraw_curbuf_later(UPD_INVERTED);	// update the inversion
    }
    లేకపోతే ఒకవేళ (VIsual_చురుకైన && !old_చురుకైన)
    {
	ఒకవేళ (mod_ముసుగు & MOD_ముసుగు_ALT)
	    VIsual_బహుళకం = Ctrl_V;
	లేకపోతే
	    VIsual_బహుళకం = 'v';
    }

    // If Visual mode changed show it later.
    ఒకవేళ ((!VIsual_చురుకైన && old_చురుకైన && బహుళకం_displayed)
	    || (VIsual_చురుకైన && p_smd && సందేశం_silent == 0
				 && (!old_చురుకైన || VIsual_బహుళకం != old_బహుళకం)))
	redraw_cmdline = TRUE;

    తిరిగి moved;
}

    శూన్య
ins_mouse(పూర్ణాంకం c)
{
    pos_T	tpos;
    win_T	*old_curwin = curwin;

#నిర్వచితమైతే FEAT_GUI
    // When GUI is active, also move/paste when 'mouse' is empty
    ఒకవేళ (!gui.లోపల_use)
#అయితే_ముగింపు
	ఒకవేళ (!mouse_has(MOUSE_చొప్పించు))
	    తిరిగి;

    undisplay_dollar();
    tpos = curwin->w_cursor;
    ఒకవేళ (do_mouse(ఖాళీ, c, BACKWARD, 1L, 0))
    {
	win_T	*new_curwin = curwin;

	ఒకవేళ (curwin != old_curwin && win_valid(old_curwin))
	{
	    // Mouse took us to another window.  We need to go back to the
	    // previous one to stop insert there properly.
	    curwin = old_curwin;
	    curbuf = curwin->w_బఫరు;
#నిర్వచితమైతే FEAT_పని_CHANNEL
	    ఒకవేళ (bt_prompt(curbuf))
		// Restart Insert mode when re-entering the prompt buffer.
		curbuf->b_prompt_చొప్పించు = 'A';
#అయితే_ముగింపు
	}
	ప్రారంభం_arrow(curwin == old_curwin ? &tpos : ఖాళీ);
	ఒకవేళ (curwin != new_curwin && win_valid(new_curwin))
	{
	    curwin = new_curwin;
	    curbuf = curwin->w_బఫరు;
	}
	సమితి_can_cindent(TRUE);
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
    స్థిర శూన్య
do_mousescroll(cmdarg_T *cap)
{
    పూర్ణాంకం జరుపు_or_ctrl = mod_ముసుగు & (MOD_ముసుగు_జరుపు | MOD_ముసుగు_CTRL);

#నిర్వచితమైతే FEAT_TERMINAL
    ఒకవేళ (పదం_use_loop())
	// This window is a terminal window, send the mouse event there.
	// Set "typed" to FALSE to avoid an endless loop.
	send_కీలకాలు_గమ్యం_పదం(curbuf->b_పదం, cap->cmdchar, mod_ముసుగు, FALSE);
    లేకపోతే
#అయితే_ముగింపు
    ఒకవేళ (cap->arg == MSCR_పైకి || cap->arg == MSCR_కిందికి)
    {
	// Vertical scrolling
	ఒకవేళ (!(State & బహుళకం_చొప్పించు) && (mouse_vert_అడుగు < 0 || జరుపు_or_ctrl))
	{
	    // whole page up or down
	    pagescroll(cap->arg == MSCR_పైకి ? FORWARD : BACKWARD, 1L, FALSE);
	}
	లేకపోతే
	{
	    ఒకవేళ (mouse_vert_అడుగు < 0 || జరుపు_or_ctrl)
	    {
		// whole page up or down
		cap->count1 = (దీర్ఘ)(curwin->w_botline - curwin->w_topline);
	    }
	    // Don't scroll more than half the window height.
	    లేకపోతే ఒకవేళ (curwin->w_ఎత్తు < mouse_vert_అడుగు * 2)
	    {
		cap->count1 = curwin->w_ఎత్తు / 2;
		ఒకవేళ (cap->count1 == 0)
		    cap->count1 = 1;
	    }
	    లేకపోతే
	    {
		cap->count1 = mouse_vert_అడుగు;
	    }
	    cap->count0 = cap->count1;
	    nv_scroll_పంక్తి(cap);
	}

#నిర్వచితమైతే FEAT_PROP_POPUP
	ఒకవేళ (WIN_IS_POPUP(curwin))
	    popup_సమితి_firstline(curwin);
#అయితే_ముగింపు
    }
    లేకపోతే
    {
	// Horizontal scrolling
	దీర్ఘ అడుగు = (mouse_hor_అడుగు < 0 || జరుపు_or_ctrl)
					    ? curwin->w_వెడల్పు : mouse_hor_అడుగు;
	దీర్ఘ leftcol = curwin->w_leftcol
				     + (cap->arg == MSCR_కుడి ? -అడుగు : అడుగు);
	ఒకవేళ (leftcol < 0)
	    leftcol = 0;
	do_mousescroll_horiz((long_u)leftcol);
    }
    may_trigger_win_scrolled_resized();
}

/*
 * Insert mode implementation for scrolling in direction "dir", which is
 * one of the MSCR_ values.
 */
    శూన్య
ins_mousescroll(పూర్ణాంకం dir)
{
    cmdarg_T	cap;
    oparg_T	oa;
    CLEAR_క్షేత్రం(cap);
    clear_oparg(&oa);
    cap.oap = &oa;
    cap.arg = dir;

#నిర్వచితమైతే FEAT_TABPANEL
    ఒకవేళ (mouse_అడ్డువరుస >= 0 && mouse_నిలువు >= 0
	    && (dir == MSCR_పైకి || dir == MSCR_కిందికి)
	    && mouse_on_tabpanel())
    {
	(శూన్య)tabpanel_scroll(dir == MSCR_పైకి ? 1 : -1,
		mouse_vert_అడుగు > 0 ? mouse_vert_అడుగు : 3);
	తిరిగి;
    }
#అయితే_ముగింపు

    ఎంపిక (dir)
    {
	స్థితి MSCR_పైకి:
	    cap.cmdchar = K_MOUSEUP;
	    ఆపు;
	స్థితి MSCR_కిందికి:
	    cap.cmdchar = K_MOUSEDOWN;
	    ఆపు;
	స్థితి MSCR_ఎడమ:
	    cap.cmdchar = K_MOUSELEFT;
	    ఆపు;
	స్థితి MSCR_కుడి:
	    cap.cmdchar = K_MOUSERIGHT;
	    ఆపు;
	అప్రమేయ:
	    siemsg("Invalid ins_mousescroll() argument: %d", dir);
    }

    win_T *old_curwin = curwin;
    ఒకవేళ (mouse_అడ్డువరుస >= 0 && mouse_నిలువు >= 0)
    {
	// Find the window at the mouse pointer coordinates.
	// NOTE: Must restore "curwin" to "old_curwin" before returning!
	పూర్ణాంకం అడ్డువరుస = mouse_అడ్డువరుస;
	పూర్ణాంకం నిలువు = mouse_నిలువు;
	curwin = mouse_కనుగొను_win(&అడ్డువరుస, &నిలువు, కనుగొను_POPUP);
	ఒకవేళ (curwin == ఖాళీ)
	{
	    curwin = old_curwin;
	    తిరిగి;
	}
	curbuf = curwin->w_బఫరు;
    }

    ఒకవేళ (curwin == old_curwin)
    {
	// Don't scroll the current window if the popup menu is visible.
	ఒకవేళ (pum_visible())
	    తిరిగి;

	undisplay_dollar();
    }

    linenr_T	orig_topline = curwin->w_topline;
    colnr_T	orig_leftcol = curwin->w_leftcol;
    pos_T	orig_cursor = curwin->w_cursor;

    // Call the common mouse scroll function shared with other modes.
    do_mousescroll(&cap);

    పూర్ణాంకం did_scroll = (orig_topline != curwin->w_topline
		   || orig_leftcol != curwin->w_leftcol);

    curwin->w_redr_స్థితి = true;
    curwin = old_curwin;
    curbuf = curwin->w_బఫరు;

    // If the window actually scrolled and the popup menu may overlay the
    // window, need to redraw it.
    ఒకవేళ (did_scroll && pum_visible())
    {
	// TODO: Would be more efficient to only redraw the windows that are
	// overlapped by the popup menu.
	redraw_all_later(UPD_NOT_VALID);
	ins_compl_show_pum();
    }

    ఒకవేళ (!EQUAL_POS(curwin->w_cursor, orig_cursor))
    {
	ప్రారంభం_arrow(&orig_cursor);
	సమితి_can_cindent(TRUE);
    }
}

#ఒకవేళ defined(FEAT_PROP_POPUP)
/*
 * Command-line mode implementation for scrolling in direction "dir", which is
 * one of the MSCR_ values.  Scrolls the completion info popup when the mouse
 * pointer is on top of it.
 * Returns TRUE when the info popup was scrolled.
 */
    పూర్ణాంకం
cmdline_mousescroll(పూర్ణాంకం dir)
{
    cmdarg_T	cap;
    oparg_T	oa;

    CLEAR_క్షేత్రం(cap);
    clear_oparg(&oa);
    cap.oap = &oa;
    cap.arg = dir;

    ఎంపిక (dir)
    {
	స్థితి MSCR_పైకి:	 cap.cmdchar = K_MOUSEUP; ఆపు;
	స్థితి MSCR_కిందికి:	 cap.cmdchar = K_MOUSEDOWN; ఆపు;
	స్థితి MSCR_ఎడమ:	 cap.cmdchar = K_MOUSELEFT; ఆపు;
	స్థితి MSCR_కుడి: cap.cmdchar = K_MOUSERIGHT; ఆపు;
    }

    ఒకవేళ (mouse_అడ్డువరుస < 0 || mouse_నిలువు < 0)
	తిరిగి FALSE;

    పూర్ణాంకం	    అడ్డువరుస = mouse_అడ్డువరుస;
    పూర్ణాంకం	    నిలువు = mouse_నిలువు;
    win_T   *wp;

    // Only scroll when the mouse is on top of the info popup.
    wp = mouse_కనుగొను_win(&అడ్డువరుస, &నిలువు, కనుగొను_POPUP);
    ఒకవేళ (wp == ఖాళీ || !WIN_IS_POPUP(wp) || !(wp->w_popup_flags & POPF_INFO)
							|| !wp->w_has_scrollbar)
	తిరిగి FALSE;

    win_T   *old_curwin = curwin;

    curwin = wp;
    curbuf = wp->w_బఫరు;
    // Call the common mouse scroll function shared with other modes.
    do_mousescroll(&cap);
    curwin = old_curwin;
    curbuf = curwin->w_బఫరు;

    // Cmdline mode doesn't normally call update_screen(), so redraw the
    // completion popup menu, which also repaints the info popup.
    ఒకవేళ (cmdline_pum_చురుకైన())
	cmdline_pum_display();
    తిరిగి TRUE;
}
#అయితే_ముగింపు

/*
 * Return TRUE if "c" is a mouse key.
 */
    పూర్ణాంకం
is_mouse_కీలకం(పూర్ణాంకం c)
{
    తిరిగి c == K_LEFTMOUSE
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

స్థిర నిర్మాణం mousetable
{
    పూర్ణాంకం	    pseudo_సంకేతం;	// Code for pseudo mouse event
    పూర్ణాంకం	    button;		// Which mouse button is it?
    పూర్ణాంకం	    is_click;		// Is it a mouse button click event?
    పూర్ణాంకం	    is_drag;		// Is it a mouse drag event?
} mouse_పట్టిక[] =
{
    {(పూర్ణాంకం)KE_LEFTMOUSE,		MOUSE_ఎడమ,	TRUE,	FALSE},
#నిర్వచితమైతే FEAT_GUI
    {(పూర్ణాంకం)KE_LEFTMOUSE_NM,	MOUSE_ఎడమ,	TRUE,	FALSE},
#అయితే_ముగింపు
    {(పూర్ణాంకం)KE_LEFTDRAG,		MOUSE_ఎడమ,	FALSE,	TRUE},
    {(పూర్ణాంకం)KE_LEFTRELEASE,	MOUSE_ఎడమ,	FALSE,	FALSE},
#నిర్వచితమైతే FEAT_GUI
    {(పూర్ణాంకం)KE_LEFTRELEASE_NM,	MOUSE_ఎడమ,	FALSE,	FALSE},
#అయితే_ముగింపు
    {(పూర్ణాంకం)KE_MIDDLEMOUSE,	MOUSE_మధ్య,	TRUE,	FALSE},
    {(పూర్ణాంకం)KE_MIDDLEDRAG,	MOUSE_మధ్య,	FALSE,	TRUE},
    {(పూర్ణాంకం)KE_MIDDLERELEASE,	MOUSE_మధ్య,	FALSE,	FALSE},
    {(పూర్ణాంకం)KE_RIGHTMOUSE,	MOUSE_కుడి,	TRUE,	FALSE},
    {(పూర్ణాంకం)KE_RIGHTDRAG,		MOUSE_కుడి,	FALSE,	TRUE},
    {(పూర్ణాంకం)KE_RIGHTRELEASE,	MOUSE_కుడి,	FALSE,	FALSE},
    {(పూర్ణాంకం)KE_X1MOUSE,		MOUSE_X1,	TRUE,	FALSE},
    {(పూర్ణాంకం)KE_X1DRAG,		MOUSE_X1,	FALSE,	TRUE},
    {(పూర్ణాంకం)KE_X1RELEASE,		MOUSE_X1,	FALSE,	FALSE},
    {(పూర్ణాంకం)KE_X2MOUSE,		MOUSE_X2,	TRUE,	FALSE},
    {(పూర్ణాంకం)KE_X2DRAG,		MOUSE_X2,	FALSE,	TRUE},
    {(పూర్ణాంకం)KE_X2RELEASE,		MOUSE_X2,	FALSE,	FALSE},
    // DRAG without CLICK
    {(పూర్ణాంకం)KE_MOUSEMOVE,		MOUSE_RELEASE,	FALSE,	TRUE},
    // RELEASE without CLICK
    {(పూర్ణాంకం)KE_IGNORE,		MOUSE_RELEASE,	FALSE,	FALSE},
    {0,				0,		0,	0},
};

/*
 * Look up the given mouse code to return the relevant information in the other
 * arguments.  Return which button is down or was released.
 */
    పూర్ణాంకం
get_mouse_button(పూర్ణాంకం సంకేతం_2, పూర్ణాంకం *is_click, పూర్ణాంకం *is_drag)
{
    పూర్ణాంకం	    i;

    కొరకు (i = 0; mouse_పట్టిక[i].pseudo_సంకేతం; i++)
	ఒకవేళ (సంకేతం_2 == mouse_పట్టిక[i].pseudo_సంకేతం)
	{
	    *is_click = mouse_పట్టిక[i].is_click;
	    *is_drag = mouse_పట్టిక[i].is_drag;
	    తిరిగి mouse_పట్టిక[i].button;
	}
    తిరిగి 0;	    // Shouldn't get here
}

/*
 * Return the appropriate pseudo mouse event token (KE_LEFTMOUSE etc) based on
 * the given information about which mouse button is down, and whether the
 * mouse was clicked, dragged or released.
 */
    పూర్ణాంకం
get_pseudo_mouse_సంకేతం(
    పూర్ణాంకం	    button,	// eg MOUSE_LEFT
    పూర్ణాంకం	    is_click,
    పూర్ణాంకం	    is_drag)
{
    పూర్ణాంకం	    i;

    కొరకు (i = 0; mouse_పట్టిక[i].pseudo_సంకేతం; i++)
	ఒకవేళ (button == mouse_పట్టిక[i].button
	    && is_click == mouse_పట్టిక[i].is_click
	    && is_drag == mouse_పట్టిక[i].is_drag)
	{
#నిర్వచితమైతే FEAT_GUI
	    // Trick: a non mappable left click and release has mouse_col -1
	    // or added MOUSE_COLOFF.  Used for 'mousefocus' in
	    // gui_mouse_moved()
	    ఒకవేళ (mouse_నిలువు < 0 || mouse_నిలువు > MOUSE_COLOFF)
	    {
		ఒకవేళ (mouse_నిలువు < 0)
		    mouse_నిలువు = 0;
		లేకపోతే
		    mouse_నిలువు -= MOUSE_COLOFF;
		ఒకవేళ (mouse_పట్టిక[i].pseudo_సంకేతం == (పూర్ణాంకం)KE_LEFTMOUSE)
		    తిరిగి (పూర్ణాంకం)KE_LEFTMOUSE_NM;
		ఒకవేళ (mouse_పట్టిక[i].pseudo_సంకేతం == (పూర్ణాంకం)KE_LEFTRELEASE)
		    తిరిగి (పూర్ణాంకం)KE_LEFTRELEASE_NM;
	    }
#అయితే_ముగింపు
	    తిరిగి mouse_పట్టిక[i].pseudo_సంకేతం;
	}
    తిరిగి (పూర్ణాంకం)KE_IGNORE;	    // not recognized, ignore it
}

#నిర్వచించు HMT_సాధారణ	1
#నిర్వచించు HMT_NETTERM	2
#నిర్వచించు HMT_DEC	4
#నిర్వచించు HMT_JSBTERM	8
#నిర్వచించు HMT_PTERM	16
#నిర్వచించు HMT_URXVT	32
#నిర్వచించు HMT_GPM	64
#నిర్వచించు HMT_SGR	128
#నిర్వచించు HMT_SGR_REL	256
స్థిర పూర్ణాంకం has_mouse_termcode = 0;

    శూన్య
సమితి_mouse_termcode(
    పూర్ణాంకం		n,	// KS_MOUSE, KS_NETTERM_MOUSE or KS_DEC_MOUSE
    char_u	*s)
{
    char_u	name[2];

    name[0] = n;
    name[1] = KE_FILLER;
    కూడు_termcode(name, s, FALSE);
#నిర్వచితమైతే FEAT_MOUSE_JSB
    ఒకవేళ (n == KS_JSBTERM_MOUSE)
	has_mouse_termcode |= HMT_JSBTERM;
    లేకపోతే
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_MOUSE_NET
    ఒకవేళ (n == KS_NETTERM_MOUSE)
	has_mouse_termcode |= HMT_NETTERM;
    లేకపోతే
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_MOUSE_DEC
    ఒకవేళ (n == KS_DEC_MOUSE)
	has_mouse_termcode |= HMT_DEC;
    లేకపోతే
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_MOUSE_PTERM
    ఒకవేళ (n == KS_PTERM_MOUSE)
	has_mouse_termcode |= HMT_PTERM;
    లేకపోతే
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_MOUSE_URXVT
    ఒకవేళ (n == KS_URXVT_MOUSE)
	has_mouse_termcode |= HMT_URXVT;
    లేకపోతే
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_MOUSE_GPM
    ఒకవేళ (n == KS_GPM_MOUSE)
	has_mouse_termcode |= HMT_GPM;
    లేకపోతే
#అయితే_ముగింపు
    ఒకవేళ (n == KS_SGR_MOUSE)
	has_mouse_termcode |= HMT_SGR;
    లేకపోతే ఒకవేళ (n == KS_SGR_MOUSE_RELEASE)
	has_mouse_termcode |= HMT_SGR_REL;
    లేకపోతే
	has_mouse_termcode |= HMT_సాధారణ;
}

#ఒకవేళ defined(UNIX) || defined(VMS)
    శూన్య
del_mouse_termcode(
    పూర్ణాంకం		n)	// KS_MOUSE, KS_NETTERM_MOUSE or KS_DEC_MOUSE
{
    char_u	name[2];

    name[0] = n;
    name[1] = KE_FILLER;
    del_termcode(name);
# ifdef FEAT_MOUSE_JSB
    ఒకవేళ (n == KS_JSBTERM_MOUSE)
	has_mouse_termcode &= ~HMT_JSBTERM;
    లేకపోతే
# endif
# ifdef FEAT_MOUSE_NET
    ఒకవేళ (n == KS_NETTERM_MOUSE)
	has_mouse_termcode &= ~HMT_NETTERM;
    లేకపోతే
# endif
# ifdef FEAT_MOUSE_DEC
    ఒకవేళ (n == KS_DEC_MOUSE)
	has_mouse_termcode &= ~HMT_DEC;
    లేకపోతే
# endif
# ifdef FEAT_MOUSE_PTERM
    ఒకవేళ (n == KS_PTERM_MOUSE)
	has_mouse_termcode &= ~HMT_PTERM;
    లేకపోతే
# endif
# ifdef FEAT_MOUSE_URXVT
    ఒకవేళ (n == KS_URXVT_MOUSE)
	has_mouse_termcode &= ~HMT_URXVT;
    లేకపోతే
# endif
# ifdef FEAT_MOUSE_GPM
    ఒకవేళ (n == KS_GPM_MOUSE)
	has_mouse_termcode &= ~HMT_GPM;
    లేకపోతే
# endif
    ఒకవేళ (n == KS_SGR_MOUSE)
	has_mouse_termcode &= ~HMT_SGR;
    లేకపోతే ఒకవేళ (n == KS_SGR_MOUSE_RELEASE)
	has_mouse_termcode &= ~HMT_SGR_REL;
    లేకపోతే
	has_mouse_termcode &= ~HMT_సాధారణ;
}
#అయితే_ముగింపు

/*
 * setmouse() - switch mouse on/off depending on current mode and 'mouse'
 */
    శూన్య
setmouse(శూన్య)
{
    పూర్ణాంకం	    checkfor;

#నిర్వచితమైతే FEAT_MOUSESHAPE
    update_mouseshape(-1);
#అయితే_ముగింపు

    // Should be outside proc, but may break MOUSESHAPE
#నిర్వచితమైతే FEAT_GUI
    // In the GUI the mouse is always enabled.
    ఒకవేళ (gui.లోపల_use)
	తిరిగి;
#అయితే_ముగింపు
    // be quick when mouse is off
    ఒకవేళ (*p_mouse == NUL || has_mouse_termcode == 0)
	తిరిగి;

    // don't switch mouse on when not in raw mode (Ex mode)
    ఒకవేళ (cur_tmode != TMODE_RAW)
    {
	mch_setmouse(FALSE);
	తిరిగి;
    }

    ఒకవేళ (VIsual_చురుకైన)
	checkfor = MOUSE_VISUAL;
    లేకపోతే ఒకవేళ (State == బహుళకం_HITRETURN || State == బహుళకం_ASKMORE
						     || State == బహుళకం_SETWSIZE)
	checkfor = MOUSE_RETURN;
    లేకపోతే ఒకవేళ (State & బహుళకం_చొప్పించు)
	checkfor = MOUSE_చొప్పించు;
    లేకపోతే ఒకవేళ (State & బహుళకం_CMDLINE)
	checkfor = MOUSE_COMMAND;
    లేకపోతే ఒకవేళ (State == బహుళకం_CONFIRM || State == బహుళకం_EXTERNCMD)
	checkfor = ' '; // don't use mouse for ":confirm" or ":!cmd"
    లేకపోతే
	checkfor = MOUSE_సాధారణ;    // assume normal mode

    ఒకవేళ (mouse_has(checkfor))
	mch_setmouse(TRUE);
    లేకపోతే
	mch_setmouse(FALSE);
}

/*
 * Return TRUE if
 * - "c" is in 'mouse', or
 * - 'a' is in 'mouse' and "c" is in MOUSE_A, or
 * - the current buffer is a help file and 'h' is in 'mouse' and we are in a
 *   normal editing mode (not at hit-return message).
 */
    పూర్ణాంకం
mouse_has(పూర్ణాంకం c)
{
    char_u	*p;

    కొరకు (p = p_mouse; *p; ++p)
	ఎంపిక (*p)
	{
	    స్థితి 'a': ఒకవేళ (vim_strchr((char_u *)MOUSE_A, c) != ఖాళీ)
			  తిరిగి TRUE;
		      ఆపు;
	    స్థితి MOUSE_HELP: ఒకవేళ (c != MOUSE_RETURN && curbuf->b_help)
				 తిరిగి TRUE;
			     ఆపు;
	    అప్రమేయ: ఒకవేళ (c == *p) తిరిగి TRUE; ఆపు;
	}
    తిరిగి FALSE;
}

/*
 * Return TRUE when 'mousemodel' is set to "popup" or "popup_setpos".
 */
    పూర్ణాంకం
mouse_model_popup(శూన్య)
{
    తిరిగి (p_mousem[0] == 'p');
}

/*
 * Call a click-region callback function.
 * "regions"/"region_count" describe the resolved click regions,
 * "winid" is stored as the "winid" key in the info dict (0 for tabline).
 * Returns TRUE if the function was called and handled the click.
 */
    స్థిర పూర్ణాంకం
stl_click_handler_regions(
	stl_click_region_T  *regions,
	పూర్ణాంకం		    region_లెక్క,
	పూర్ణాంకం		    winid,
	char_u		    *area_name,
	పూర్ణాంకం		    mrow,
	పూర్ణాంకం		    mcol,
	పూర్ణాంకం		    which_button,
	పూర్ణాంకం		    mods)
{
#నిర్వచితమైతే FEAT_EVAL
    పూర్ణాంకం		n;
    పూర్ణాంకం		nclicks;
    char_u	button_str[2];
    char_u	mods_str[4];
    పూర్ణాంకం		mi = 0;
    dict_T	*info;
    typval_T	argvars[2];
    typval_T	rettv;
    funcexe_T	funcexe;
    పూర్ణాంకం		నిలువు = mcol;

    ఒకవేళ (regions == ఖాళీ || region_లెక్క == 0)
	తిరిగి FALSE;

    // Find the click region at the given row and column.
    కొరకు (n = 0; n < region_లెక్క; n++)
    {
	ఒకవేళ (regions[n].అడ్డువరుస == mrow
		&& నిలువు >= regions[n].నిలువు_ప్రారంభం
		&& నిలువు < regions[n].నిలువు_చివర)
	    ఆపు;
    }
    ఒకవేళ (n >= region_లెక్క || regions[n].funcname == ఖాళీ)
	తిరిగి FALSE;

    // Build the info dictionary.
    info = dict_alloc();
    ఒకవేళ (info == ఖాళీ)
	తిరిగి FALSE;

    dict_కూడు_సంఖ్య(info, "minwid", regions[n].minwid);

    // Determine number of clicks.
    // MOD_MASK_2CLICK=0x20, MOD_MASK_3CLICK=0x40, MOD_MASK_4CLICK=0x60
    nclicks = ((mods & MOD_ముసుగు_MULTI_CLICK) >> 5) + 1;
    ఒకవేళ (nclicks > 3)
	nclicks = 3;
    dict_కూడు_సంఖ్య(info, "nclicks", nclicks);

    // Button.
    ఒకవేళ (which_button == MOUSE_ఎడమ)
	button_str[0] = 'l';
    లేకపోతే ఒకవేళ (which_button == MOUSE_కుడి)
	button_str[0] = 'r';
    లేకపోతే
	button_str[0] = 'm';
    button_str[1] = NUL;
    dict_కూడు_string_len(info, "button", button_str, 1);

    // Modifiers.
    ఒకవేళ (mods & MOD_ముసుగు_జరుపు)
	mods_str[mi++] = 's';
    ఒకవేళ (mods & MOD_ముసుగు_CTRL)
	mods_str[mi++] = 'c';
    ఒకవేళ (mods & MOD_ముసుగు_ALT)
	mods_str[mi++] = 'a';
    mods_str[mi] = NUL;
    dict_కూడు_string_len(info, "mods", mods_str, mi);

    dict_కూడు_సంఖ్య(info, "winid", winid);

    // "area": which option the clicked region belongs to.  Lets a shared
    // dispatcher distinguish 'statusline', 'tabline' and 'tabpanel' without
    // having to overload winid == 0.
    dict_కూడు_string(info, "area", area_name);

    // Expose tab page number for 'tabpanel' regions.
    ఒకవేళ (regions[n].tabnr > 0)
	dict_కూడు_సంఖ్య(info, "tabnr", regions[n].tabnr);

    // Call the function with the info dict as argument.
    argvars[0].v_type = VAR_DICT;
    argvars[0].vval.v_dict = info;
    ++info->dv_refcount;
    argvars[1].v_type = VAR_UNKNOWN;

    rettv.v_type = VAR_సంఖ్య;
    rettv.vval.v_సంఖ్య = 0;

    CLEAR_క్షేత్రం(funcexe);
    funcexe.fe_evaluate = TRUE;
    (శూన్య)call_func(regions[n].funcname, -1,
					    &rettv, 1, argvars, &funcexe);

    n = (పూర్ణాంకం)rettv.vval.v_సంఖ్య;
    clear_tv(&rettv);
    dict_unref(info);

    ఒకవేళ (n != 0)
    {
	// Make sure the tabline gets redrawn too when the callback asks for
	// a redraw (redraw_statuslines() only redraws the tabline when
	// redraw_tabline is set).  For tabpanel the whole screen needs to be
	// refreshed.
	ఒకవేళ (winid == 0)
	    redraw_tabline = TRUE;
# ifdef FEAT_TABPANEL
	ఒకవేళ (STRCMP(area_name, "tabpanel") == 0)
	    redraw_all_later(UPD_NOT_VALID);
# endif
	redraw_statuslines();
    }

    తిరిగి TRUE;
#లేకపోతే
    (శూన్య)regions;
    (శూన్య)region_లెక్క;
    (శూన్య)winid;
    (శూన్య)area_name;
    (శూన్య)mrow;
    (శూన్య)mcol;
    (శూన్య)which_button;
    (శూన్య)mods;
    తిరిగి FALSE;
#అయితే_ముగింపు
}

/*
 * Call a statusline click handler function for window "wp".
 * Returns TRUE if the function was called and handled the click.
 */
    స్థిర పూర్ణాంకం
stl_click_handler(win_T *wp, పూర్ణాంకం mrow, పూర్ణాంకం mcol, పూర్ణాంకం which_button, పూర్ణాంకం mods)
{
    ఒకవేళ (wp == ఖాళీ)
	తిరిగి FALSE;
    తిరిగి stl_click_handler_regions(wp->w_stl_click, wp->w_stl_click_లెక్క,
				wp->w_id, (char_u *)"statusline",
				mrow, mcol, which_button, mods);
}

// dragwin is declared near the top of the file

/*
 * Reset the window being dragged.  To be called when switching tab page.
 */
    శూన్య
reset_dragwin(శూన్య)
{
    dragwin = ఖాళీ;
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
    పూర్ణాంకం
jump_గమ్యం_mouse(
    పూర్ణాంకం		flags,
    పూర్ణాంకం		*inclusive,	// used for inclusive operator, can be NULL
    పూర్ణాంకం		which_button)	// MOUSE_LEFT, MOUSE_RIGHT, MOUSE_MIDDLE
{
    స్థిర పూర్ణాంకం	on_స్థితి_పంక్తి = 0;	// #lines below bottom of window
    స్థిర పూర్ణాంకం	on_sep_పంక్తి = 0;	// on separator right of window
#నిర్వచితమైతే FEAT_MENU
    స్థిర పూర్ణాంకం  లోపల_winbar = FALSE;
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_PROP_POPUP
    స్థిర పూర్ణాంకం   లోపల_popup_win = FALSE;
    స్థిర win_T *click_లోపల_popup_win = ఖాళీ;
#అయితే_ముగింపు
    స్థిర పూర్ణాంకం	prev_అడ్డువరుస = -1;
    స్థిర పూర్ణాంకం	prev_నిలువు = -1;
    స్థిర పూర్ణాంకం	did_drag = FALSE;	// drag was noticed

    win_T	*wp, *old_curwin;
    pos_T	old_cursor;
    పూర్ణాంకం		లెక్క;
    పూర్ణాంకం		మొదటి;
    పూర్ణాంకం		అడ్డువరుస = mouse_అడ్డువరుస;
    పూర్ణాంకం		నిలువు = mouse_నిలువు;
    colnr_T	నిలువు_from_screen = -1;
#నిర్వచితమైతే FEAT_FOLDING
    పూర్ణాంకం		mouse_char = ' ';
#అయితే_ముగింపు

    mouse_past_bottom = FALSE;
    mouse_past_eol = FALSE;

    ఒకవేళ (flags & MOUSE_RELEASED)
    {
	// On button release we may change window focus if positioned on a
	// status line and no dragging happened.
	ఒకవేళ (dragwin != ఖాళీ && !did_drag)
	    flags &= ~(MOUSE_FOCUS | MOUSE_DID_MOVE);
	dragwin = ఖాళీ;
	did_drag = FALSE;
#నిర్వచితమైతే FEAT_PROP_POPUP
	ఒకవేళ (click_లోపల_popup_win != ఖాళీ && popup_dragwin == ఖాళీ)
	    popup_close_for_mouse_click(click_లోపల_popup_win);

	popup_dragwin = ఖాళీ;
	click_లోపల_popup_win = ఖాళీ;
#అయితే_ముగింపు
    }

    ఒకవేళ ((flags & MOUSE_DID_MOVE)
	    && prev_అడ్డువరుస == mouse_అడ్డువరుస
	    && prev_నిలువు == mouse_నిలువు)
    {
retnomove:
	// before moving the cursor for a left click which is NOT in a status
	// line, stop Visual mode
	ఒకవేళ (on_స్థితి_పంక్తి)
	    తిరిగి లోపల_స్థితి_పంక్తి;
	ఒకవేళ (on_sep_పంక్తి)
	    తిరిగి లోపల_SEP_పంక్తి;
#నిర్వచితమైతే FEAT_MENU
	ఒకవేళ (లోపల_winbar)
	{
	    // A quick second click may arrive as a double-click, but we use it
	    // as a second click in the WinBar.
	    ఒకవేళ ((mod_ముసుగు & MOD_ముసుగు_MULTI_CLICK) && !(flags & MOUSE_RELEASED))
	    {
		wp = mouse_కనుగొను_win(&అడ్డువరుస, &నిలువు, FAIL_POPUP);
		ఒకవేళ (wp == ఖాళీ)
		    తిరిగి లోపల_UNKNOWN;
		winbar_click(wp, నిలువు);
	    }
	    తిరిగి లోపల_OTHER_WIN | MOUSE_WINBAR;
	}
#అయితే_ముగింపు
	ఒకవేళ (flags & MOUSE_MAY_STOP_VIS)
	{
	    చివర_visual_బహుళకం_keep_button();
	    redraw_curbuf_later(UPD_INVERTED);	// delete the inversion
	}
#ఒకవేళ defined(FEAT_CLIPBOARD)
	// Continue a modeless selection in another window.
	ఒకవేళ (cmdwin_type != 0 && అడ్డువరుస < cmdwin_win->w_winrow)
	    తిరిగి లోపల_OTHER_WIN;
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_PROP_POPUP
	// Continue a modeless selection in a popup window or dragging it.
	ఒకవేళ (లోపల_popup_win)
	{
	    click_లోపల_popup_win = ఖాళీ;  // don't close it on release
	    ఒకవేళ (popup_dragwin != ఖాళీ)
	    {
		// dragging a popup window
		popup_drag(popup_dragwin);
		తిరిగి లోపల_UNKNOWN;
	    }
	    తిరిగి లోపల_OTHER_WIN;
	}
#అయితే_ముగింపు
	తిరిగి లోపల_బఫరు;
    }

    prev_అడ్డువరుస = mouse_అడ్డువరుస;
    prev_నిలువు = mouse_నిలువు;

    ఒకవేళ (flags & MOUSE_SETPOS)
	వెళ్ళు retnomove;				// ugly goto...

    old_curwin = curwin;
    old_cursor = curwin->w_cursor;

    ఒకవేళ (!(flags & MOUSE_FOCUS))
    {
	ఒకవేళ (అడ్డువరుస < 0 || నిలువు < 0) // check if it makes sense
	    తిరిగి లోపల_UNKNOWN;

	// find the window where the row is in and adjust "row" and "col" to be
	// relative to top-left of the window
	wp = mouse_కనుగొను_win(&అడ్డువరుస, &నిలువు, కనుగొను_POPUP);
	ఒకవేళ (wp == ఖాళీ)
	    తిరిగి లోపల_UNKNOWN;
	dragwin = ఖాళీ;

#నిర్వచితమైతే FEAT_PROP_POPUP
	// Click in a popup window may start dragging or modeless selection,
	// but not much else.
	ఒకవేళ (WIN_IS_POPUP(wp))
	{
	    on_sep_పంక్తి = 0;
	    on_స్థితి_పంక్తి = 0;
	    లోపల_popup_win = TRUE;
	    ఒకవేళ (which_button == MOUSE_ఎడమ && popup_close_if_on_X(wp, అడ్డువరుస, నిలువు))
	    {
		తిరిగి లోపల_UNKNOWN;
	    }
	    లేకపోతే ఒకవేళ (((wp->w_popup_flags & (POPF_DRAG | POPF_RESIZE))
					      && popup_on_border(wp, అడ్డువరుస, నిలువు))
				       || (wp->w_popup_flags & POPF_DRAGALL))
	    {
		popup_dragwin = wp;
		popup_ప్రారంభం_drag(wp, అడ్డువరుస, నిలువు);
		తిరిగి లోపల_UNKNOWN;
	    }
	    // Only close on release, otherwise it's not possible to drag or do
	    // modeless selection.
	    లేకపోతే ఒకవేళ (wp->w_popup_close == POPCLOSE_CLICK
		    && which_button == MOUSE_ఎడమ)
	    {
		click_లోపల_popup_win = wp;
	    }
	    లేకపోతే ఒకవేళ (which_button == MOUSE_ఎడమ)
		// If the click is in the scrollbar, may scroll up/down.
		popup_handle_scrollbar_click(wp, అడ్డువరుస, నిలువు);
# ifdef FEAT_CLIPBOARD
	    తిరిగి లోపల_OTHER_WIN;
# లేకపోతే
	    తిరిగి లోపల_UNKNOWN;
# endif
	}
	లోపల_popup_win = FALSE;
	popup_dragwin = ఖాళీ;
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_MENU
	ఒకవేళ (అడ్డువరుస == -1)
	{
	    // A click in the window toolbar does not enter another window or
	    // change Visual highlighting.
	    winbar_click(wp, నిలువు);
	    లోపల_winbar = TRUE;
	    తిరిగి లోపల_OTHER_WIN | MOUSE_WINBAR;
	}
	లోపల_winbar = FALSE;
#అయితే_ముగింపు

	// winpos and height may change in win_enter()!
	ఒకవేళ (అడ్డువరుస >= wp->w_ఎత్తు)		// In (or below) status line
	{
	    on_స్థితి_పంక్తి = అడ్డువరుస - wp->w_ఎత్తు + 1;
	    dragwin = wp;
	}
	లేకపోతే
	    on_స్థితి_పంక్తి = 0;
	ఒకవేళ (నిలువు >= wp->w_వెడల్పు)		// In separator line
	{
	    on_sep_పంక్తి = నిలువు - wp->w_వెడల్పు + 1;
	    dragwin = wp;
	}
	లేకపోతే
	    on_sep_పంక్తి = 0;

	// The rightmost character of the status line might be a vertical
	// separator character if there is no connecting window to the right.
	ఒకవేళ (on_స్థితి_పంక్తి && on_sep_పంక్తి)
	{
	    ఒకవేళ (stl_connected(wp))
		on_sep_పంక్తి = 0;
	    లేకపోతే
		on_స్థితి_పంక్తి = 0;
	}

	// Before jumping to another buffer, or moving the cursor for a left
	// click, stop Visual mode.
	ఒకవేళ (VIsual_చురుకైన
		&& (wp->w_బఫరు != curwin->w_బఫరు
		    || (!on_స్థితి_పంక్తి && !on_sep_పంక్తి
#నిర్వచితమైతే FEAT_FOLDING
			&& (
# ifdef FEAT_RIGHTLEFT
			    wp->w_p_rl ? నిలువు < wp->w_వెడల్పు - wp->w_p_fdc :
# endif
			    నిలువు >= wp->w_p_fdc + (wp != cmdwin_win ? 0 : 1)
			    )
#అయితే_ముగింపు
			&& (flags & MOUSE_MAY_STOP_VIS))))
	{
	    చివర_visual_బహుళకం_keep_button();
	    redraw_curbuf_later(UPD_INVERTED);	// delete the inversion
	}
	ఒకవేళ (cmdwin_type != 0 && wp != cmdwin_win)
	{
	    // A click outside the command-line window: Use modeless
	    // selection if possible.  Allow dragging the status lines.
	    on_sep_పంక్తి = 0;
#నిర్వచితమైతే FEAT_CLIPBOARD
	    ఒకవేళ (on_స్థితి_పంక్తి)
		తిరిగి లోపల_స్థితి_పంక్తి;
	    తిరిగి లోపల_OTHER_WIN;
#లేకపోతే
	    అడ్డువరుస = 0;
	    నిలువు += wp->w_wincol;
	    wp = cmdwin_win;
#అయితే_ముగింపు
	}
#ఒకవేళ defined(FEAT_PROP_POPUP) && defined(FEAT_TERMINAL)
	ఒకవేళ (popup_is_popup(curwin) && curbuf->b_పదం != ఖాళీ)
	    // terminal in popup window: don't jump to another window
	    తిరిగి లోపల_OTHER_WIN;
#అయితే_ముగింపు
	// Only change window focus when not clicking on or dragging the
	// status line.  Do change focus when releasing the mouse button
	// (MOUSE_FOCUS was set above if we dragged first).
	ఒకవేళ (dragwin == ఖాళీ || (flags & MOUSE_RELEASED))
	    win_enter(wp, TRUE);		// can make wp invalid!

	ఒకవేళ (curwin != old_curwin)
	{
#నిర్వచితమైతే CHECK_DOUBLE_CLICK
	    // set topline, to be able to check for double click ourselves
	    సమితి_mouse_topline(curwin);
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_TERMINAL
	    // when entering a terminal window may change state
	    పదం_win_entered();
#అయితే_ముగింపు
	}
	ఒకవేళ (on_స్థితి_పంక్తి)			// In (or below) status line
	{
	    // Don't use start_arrow() if we're in the same window
	    ఒకవేళ (curwin == old_curwin)
		తిరిగి లోపల_స్థితి_పంక్తి;
	    లేకపోతే
		తిరిగి లోపల_స్థితి_పంక్తి | CURSOR_MOVED;
	}
	ఒకవేళ (on_sep_పంక్తి)			// In (or below) status line
	{
	    // Don't use start_arrow() if we're in the same window
	    ఒకవేళ (curwin == old_curwin)
		తిరిగి లోపల_SEP_పంక్తి;
	    లేకపోతే
		తిరిగి లోపల_SEP_పంక్తి | CURSOR_MOVED;
	}

	curwin->w_cursor.lnum = curwin->w_topline;
#నిర్వచితమైతే FEAT_GUI
	// remember topline, needed for double click
	gui_prev_topline = curwin->w_topline;
# ifdef FEAT_DIFF
	gui_prev_topfill = curwin->w_topfill;
# endif
#అయితే_ముగింపు
    }
    లేకపోతే ఒకవేళ (on_స్థితి_పంక్తి && which_button == MOUSE_ఎడమ)
    {
	ఒకవేళ (dragwin != ఖాళీ)
	{
	    // Drag the status line
	    లెక్క = అడ్డువరుస - W_WINROW(dragwin) - dragwin->w_ఎత్తు + 1
							     - on_స్థితి_పంక్తి;
	    win_drag_స్థితి_పంక్తి(dragwin, లెక్క);
	    did_drag |= లెక్క;
	}
	తిరిగి లోపల_స్థితి_పంక్తి;			// Cursor didn't move
    }
    లేకపోతే ఒకవేళ (on_sep_పంక్తి && which_button == MOUSE_ఎడమ)
    {
	ఒకవేళ (dragwin != ఖాళీ)
	{
	    // Drag the separator column
	    లెక్క = నిలువు - dragwin->w_wincol - dragwin->w_వెడల్పు + 1
								- on_sep_పంక్తి;
	    win_drag_vsep_పంక్తి(dragwin, లెక్క);
	    did_drag |= లెక్క;
	}
	తిరిగి లోపల_SEP_పంక్తి;			// Cursor didn't move
    }
#నిర్వచితమైతే FEAT_MENU
    లేకపోతే ఒకవేళ (లోపల_winbar)
    {
	// After a click on the window toolbar don't start Visual mode.
	తిరిగి లోపల_OTHER_WIN | MOUSE_WINBAR;
    }
#అయితే_ముగింపు
    లేకపోతే // keep_window_focus must be TRUE
    {
	// before moving the cursor for a left click, stop Visual mode
	ఒకవేళ (flags & MOUSE_MAY_STOP_VIS)
	{
	    చివర_visual_బహుళకం_keep_button();
	    redraw_curbuf_later(UPD_INVERTED);	// delete the inversion
	}

#ఒకవేళ defined(FEAT_CLIPBOARD)
	// Continue a modeless selection in another window.
	ఒకవేళ (cmdwin_type != 0 && అడ్డువరుస < cmdwin_win->w_winrow)
	    తిరిగి లోపల_OTHER_WIN;
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_PROP_POPUP
	ఒకవేళ (లోపల_popup_win)
	{
	    ఒకవేళ (popup_dragwin != ఖాళీ)
	    {
		// dragging a popup window
		popup_drag(popup_dragwin);
		తిరిగి లోపల_UNKNOWN;
	    }
	    // continue a modeless selection in a popup window
	    click_లోపల_popup_win = ఖాళీ;
	    తిరిగి లోపల_OTHER_WIN;
	}
#అయితే_ముగింపు

	అడ్డువరుస -= W_WINROW(curwin);
	నిలువు -= curwin->w_wincol;

	// When clicking beyond the end of the window, scroll the screen.
	// Scroll by however many rows outside the window we are.
	ఒకవేళ (అడ్డువరుస < 0)
	{
	    లెక్క = 0;
	    కొరకు (మొదటి = TRUE; curwin->w_topline > 1; )
	    {
#నిర్వచితమైతే FEAT_DIFF
		ఒకవేళ (curwin->w_topfill < diff_check_fill(curwin, curwin->w_topline))
		    ++లెక్క;
		లేకపోతే
#అయితే_ముగింపు
		    లెక్క += plines(curwin->w_topline - 1);
		ఒకవేళ (!మొదటి && లెక్క > -అడ్డువరుస)
		    ఆపు;
		మొదటి = FALSE;
#నిర్వచితమైతే FEAT_FOLDING
		(శూన్య)hasFolding(curwin->w_topline, &curwin->w_topline, ఖాళీ);
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_DIFF
		ఒకవేళ (curwin->w_topfill < diff_check_fill(curwin, curwin->w_topline))
		    ++curwin->w_topfill;
		లేకపోతే
#అయితే_ముగింపు
		{
		    --curwin->w_topline;
#నిర్వచితమైతే FEAT_DIFF
		    curwin->w_topfill = 0;
#అయితే_ముగింపు
		}
	    }
#నిర్వచితమైతే FEAT_DIFF
	    check_topfill(curwin, FALSE);
#అయితే_ముగింపు
	    curwin->w_valid &=
		      ~(VALID_WROW|VALID_CROW|VALID_BOTLINE|VALID_BOTLINE_AP);
	    redraw_later(UPD_VALID);
	    అడ్డువరుస = 0;
	}
	లేకపోతే ఒకవేళ (అడ్డువరుస >= curwin->w_ఎత్తు)
	{
	    లెక్క = 0;
	    కొరకు (మొదటి = TRUE; curwin->w_topline < curbuf->b_ml.ml_పంక్తి_లెక్క; )
	    {
#నిర్వచితమైతే FEAT_DIFF
		ఒకవేళ (curwin->w_topfill > 0)
		    ++లెక్క;
		లేకపోతే
#అయితే_ముగింపు
		    లెక్క += plines(curwin->w_topline);
		ఒకవేళ (!మొదటి && లెక్క > అడ్డువరుస - curwin->w_ఎత్తు + 1)
		    ఆపు;
		మొదటి = FALSE;
#నిర్వచితమైతే FEAT_FOLDING
		ఒకవేళ (hasFolding(curwin->w_topline, ఖాళీ, &curwin->w_topline)
			&& curwin->w_topline == curbuf->b_ml.ml_పంక్తి_లెక్క)
		    ఆపు;
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_DIFF
		ఒకవేళ (curwin->w_topfill > 0)
		    --curwin->w_topfill;
		లేకపోతే
#అయితే_ముగింపు
		{
		    ++curwin->w_topline;
#నిర్వచితమైతే FEAT_DIFF
		    curwin->w_topfill =
				   diff_check_fill(curwin, curwin->w_topline);
#అయితే_ముగింపు
		}
	    }
#నిర్వచితమైతే FEAT_DIFF
	    check_topfill(curwin, FALSE);
#అయితే_ముగింపు
	    redraw_later(UPD_VALID);
	    curwin->w_valid &=
		      ~(VALID_WROW|VALID_CROW|VALID_BOTLINE|VALID_BOTLINE_AP);
	    అడ్డువరుస = curwin->w_ఎత్తు - 1;
	}
	లేకపోతే ఒకవేళ (అడ్డువరుస == 0)
	{
	    // When dragging the mouse, while the text has been scrolled up as
	    // far as it goes, moving the mouse in the top line should scroll
	    // the text down (done later when recomputing w_topline).
	    ఒకవేళ (mouse_dragging > 0
		    && curwin->w_cursor.lnum
				       == curwin->w_బఫరు->b_ml.ml_పంక్తి_లెక్క
		    && curwin->w_cursor.lnum == curwin->w_topline)
		curwin->w_valid &= ~(VALID_TOPLINE);
	}
    }

    ఒకవేళ (prev_అడ్డువరుస >= W_WINROW(curwin)
	&& prev_అడ్డువరుస < W_WINROW(curwin) + curwin->w_ఎత్తు
	&& prev_నిలువు >= curwin->w_wincol && prev_నిలువు < W_ENDCOL(curwin)
						       && Screen_పంక్తులు != ఖాళీ)
    {
	పూర్ణాంకం off = పంక్తి_స్థానభ్రంశం[prev_అడ్డువరుస] + prev_నిలువు;

	// Only use ScreenCols[] after the window was redrawn.  Mainly matters
	// for tests, a user would not click before redrawing.
	// Do not use when 'virtualedit' is active.
	ఒకవేళ (curwin->w_redr_type <= UPD_VALID_NO_UPDATE)
	    నిలువు_from_screen = Screen_నిలువుల_సంఖ్య[off];
#నిర్వచితమైతే FEAT_FOLDING
	// Remember the character under the mouse, it might be a '-' or '+' in
	// the fold column.
	mouse_char = enc_utf8 && Screen_పంక్తులు_UC[off] != 0
				       ? Screen_పంక్తులు_UC[off] : Screen_పంక్తులు[off];
#అయితే_ముగింపు
    }

#నిర్వచితమైతే FEAT_FOLDING
    // Check for position outside of the fold column.
    ఒకవేళ (
# ifdef FEAT_RIGHTLEFT
	    curwin->w_p_rl ? నిలువు < curwin->w_వెడల్పు - curwin->w_p_fdc :
# endif
	    నిలువు >= curwin->w_p_fdc + (cmdwin_win != curwin ? 0 : 1)
       )
	mouse_char = ' ';
#అయితే_ముగింపు

    // compute the position in the buffer line from the posn on the screen
    ఒకవేళ (mouse_comp_pos(curwin, &అడ్డువరుస, &నిలువు, &curwin->w_cursor.lnum, ఖాళీ))
	mouse_past_bottom = TRUE;

    // Start Visual mode before coladvance(), for when 'sel' != "old"
    ఒకవేళ ((flags & MOUSE_MAY_VIS) && !VIsual_చురుకైన)
    {
	check_visual_highlight();
	VIsual = old_cursor;
	VIsual_చురుకైన = TRUE;
	VIsual_reselect = TRUE;
	// if 'selectmode' contains "mouse", start Select mode
	may_ప్రారంభం_select('o');
	setmouse();
	ఒకవేళ (p_smd && సందేశం_silent == 0)
	    redraw_cmdline = TRUE;	// show visual mode later
    }

    ఒకవేళ (నిలువు_from_screen >= 0)
    {
	// Use the virtual column from ScreenCols[], it is accurate also after
	// concealed characters.
	నిలువు = నిలువు_from_screen;
    }

    curwin->w_curswant = నిలువు;
    curwin->w_సమితి_curswant = false;	// May still have been TRUE
    ఒకవేళ (coladvance(నిలువు) == FAIL)	// Mouse click beyond end of line
    {
	ఒకవేళ (inclusive != ఖాళీ)
	    *inclusive = TRUE;
	mouse_past_eol = TRUE;
    }
    లేకపోతే ఒకవేళ (inclusive != ఖాళీ)
	*inclusive = FALSE;

    లెక్క = లోపల_బఫరు;
    ఒకవేళ (curwin != old_curwin || curwin->w_cursor.lnum != old_cursor.lnum
	    || curwin->w_cursor.నిలువు != old_cursor.నిలువు)
	లెక్క |= CURSOR_MOVED;		// Cursor has moved

#నిర్వచితమైతే FEAT_FOLDING
    ఒకవేళ (mouse_char == curwin->w_fill_chars.foldclosed)
	లెక్క |= MOUSE_FOLD_OPEN;
    లేకపోతే ఒకవేళ (mouse_char != ' ')
	లెక్క |= MOUSE_FOLD_CLOSE;
#అయితే_ముగింపు

    తిరిగి లెక్క;
}

/*
 * Make a horizontal scroll to "leftcol".
 * Return TRUE if the cursor moved, FALSE otherwise.
 */
    పూర్ణాంకం
do_mousescroll_horiz(long_u leftcol)
{
    ఒకవేళ (curwin->w_p_wrap)
	తిరిగి FALSE;  // no horizontal scrolling when wrapping

    ఒకవేళ (curwin->w_leftcol == (colnr_T)leftcol)
	తిరిగి FALSE;  // already there

    // When the line of the cursor is too short, move the cursor to the
    // longest visible line.
    ఒకవేళ (
#నిర్వచితమైతే FEAT_GUI
	    (!gui.లోపల_use || vim_strchr(p_go, GO_HORSCROLL) == ఖాళీ) &&
#అయితే_ముగింపు
		    !virtual_చురుకైన()
	    && (దీర్ఘ)leftcol > scroll_పంక్తి_len(curwin->w_cursor.lnum))
    {
	curwin->w_cursor.lnum = ui_కనుగొను_longest_lnum();
	curwin->w_cursor.నిలువు = 0;
    }

    తిరిగి సమితి_leftcol((colnr_T)leftcol);
}

/*
 * Normal and Visual modes implementation for scrolling in direction
 * "cap->arg", which is one of the MSCR_ values.
 */
    శూన్య
nv_mousescroll(cmdarg_T *cap)
{
    win_T   *old_curwin = curwin;

#నిర్వచితమైతే FEAT_TABPANEL
    ఒకవేళ (mouse_అడ్డువరుస >= 0 && mouse_నిలువు >= 0
	    && (cap->arg == MSCR_పైకి || cap->arg == MSCR_కిందికి)
	    && mouse_on_tabpanel())
    {
	(శూన్య)tabpanel_scroll(cap->arg == MSCR_పైకి ? 1 : -1,
		mouse_vert_అడుగు > 0 ? mouse_vert_అడుగు : 3);
	తిరిగి;
    }
#అయితే_ముగింపు

    ఒకవేళ (mouse_అడ్డువరుస >= 0 && mouse_నిలువు >= 0)
    {
	// Find the window at the mouse pointer coordinates.
	// NOTE: Must restore "curwin" to "old_curwin" before returning!
	పూర్ణాంకం అడ్డువరుస = mouse_అడ్డువరుస;
	పూర్ణాంకం నిలువు = mouse_నిలువు;
	curwin = mouse_కనుగొను_win(&అడ్డువరుస, &నిలువు, కనుగొను_POPUP);
	ఒకవేళ (curwin == ఖాళీ)
	{
	    curwin = old_curwin;
	    తిరిగి;
	}

#నిర్వచితమైతే FEAT_PROP_POPUP
	ఒకవేళ (WIN_IS_POPUP(curwin) && !curwin->w_has_scrollbar)
	{
	    // cannot scroll this popup window
	    curwin = old_curwin;
	    తిరిగి;
	}
#అయితే_ముగింపు
	curbuf = curwin->w_బఫరు;
    }

    // Call the common mouse scroll function shared with other modes.
    do_mousescroll(cap);

    curwin->w_redr_స్థితి = true;
    curwin = old_curwin;
    curbuf = curwin->w_బఫరు;
}

/*
 * Mouse clicks and drags.
 */
    శూన్య
nv_mouse(cmdarg_T *cap)
{
    (శూన్య)do_mouse(cap->oap, cap->cmdchar, BACKWARD, cap->count1, 0);
}

స్థిర పూర్ణాంకం	held_button = MOUSE_RELEASE;

    శూన్య
reset_held_button(శూన్య)
{
    held_button = MOUSE_RELEASE;
}

/*
 * Check if typebuf 'tp' contains a terminal mouse code and returns the
 * modifiers found in typebuf in 'modifiers'.
 */
    పూర్ణాంకం
check_termcode_mouse(
    char_u	*tp,
    పూర్ణాంకం		*slen,
    char_u	*కీలకం_name,
    char_u	*modifiers_ప్రారంభం,
    పూర్ణాంకం		idx,
    పూర్ణాంకం		*modifiers)
{
    పూర్ణాంకం		j;
    char_u	*p;
#ఒకవేళ !defined(UNIX) || defined(FEAT_MOUSE_XTERM) || defined(FEAT_GUI) \
    || defined(FEAT_MOUSE_GPM) || defined(FEAT_SYSMOUSE)
    char_u	బైట్లు[6];
    పూర్ణాంకం		అంకె_బైట్లు;
#అయితే_ముగింపు
    పూర్ణాంకం		mouse_సంకేతం = 0;	    // init for GCC
    పూర్ణాంకం		is_click, is_drag;
    పూర్ణాంకం		is_release, release_is_ambiguous;
    పూర్ణాంకం		wheel_సంకేతం = 0;
    పూర్ణాంకం		current_button;
    స్థిర పూర్ణాంకం	orig_అంకె_clicks = 1;
    స్థిర పూర్ణాంకం	orig_mouse_సంకేతం = 0x0;
#నిర్వచితమైతే CHECK_DOUBLE_CLICK
    స్థిర పూర్ణాంకం	orig_mouse_నిలువు = 0;
    స్థిర పూర్ణాంకం	orig_mouse_అడ్డువరుస = 0;
    స్థిర నిర్మాణం timeval  orig_mouse_సమయం = {0, 0};
    // time of previous mouse click
    నిర్మాణం timeval  mouse_సమయం;		// time of current mouse click
    దీర్ఘ	timediff;		// elapsed time in msec
#అయితే_ముగింపు

    is_click = is_drag = is_release = release_is_ambiguous = FALSE;

#ఒకవేళ !defined(UNIX) || defined(FEAT_MOUSE_XTERM) || defined(FEAT_GUI) \
    || defined(FEAT_MOUSE_GPM) || defined(FEAT_SYSMOUSE)
    ఒకవేళ (కీలకం_name[0] == KS_MOUSE
# ifdef FEAT_MOUSE_GPM
	    || కీలకం_name[0] == KS_GPM_MOUSE
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
	కొరకు (;;)
	{
	    // For the GUI and for MS-Windows two bytes each are used for row
	    // and column.  Allows for more than 223 columns.
# ఒకవేళ defined(FEAT_GUI) || defined(MSWIN)
	    ఒకవేళ (TRUE
#  ఒకవేళ defined(FEAT_GUI) && !defined(MSWIN)
		&& gui.లోపల_use
#  endif
		)
	    {
		అంకె_బైట్లు = get_బైట్లు_from_buf(tp + *slen, బైట్లు, 5);
		ఒకవేళ (అంకె_బైట్లు == -1)	// not enough coordinates
		    తిరిగి -1;
		mouse_సంకేతం = బైట్లు[0];
		mouse_నిలువు = 128 * (బైట్లు[1] - ' ' - 1)
		    + బైట్లు[2] - ' ' - 1;
		mouse_అడ్డువరుస = 128 * (బైట్లు[3] - ' ' - 1)
		    + బైట్లు[4] - ' ' - 1;
	    }
	    లేకపోతే
# endif
	    {
		అంకె_బైట్లు = get_బైట్లు_from_buf(tp + *slen, బైట్లు, 3);
		ఒకవేళ (అంకె_బైట్లు == -1)	// not enough coordinates
		    తిరిగి -1;
		mouse_సంకేతం = బైట్లు[0];
		mouse_నిలువు = బైట్లు[1] - ' ' - 1;
		mouse_అడ్డువరుస = బైట్లు[2] - ' ' - 1;
	    }
	    *slen += అంకె_బైట్లు;

	    // If the following bytes is also a mouse code and it has the same
	    // code, dump this one and get the next.  This makes dragging a
	    // whole lot faster.
# ifdef FEAT_GUI
	    ఒకవేళ (gui.లోపల_use)
		j = 3;
	    లేకపోతే
# endif
		j = get_termcode_len(idx);
	    ఒకవేళ (STRNCMP(tp, tp + *slen, (size_t)j) == 0
		    && tp[*slen + j] == mouse_సంకేతం
		    && tp[*slen + j + 1] != NUL
		    && tp[*slen + j + 2] != NUL
# ifdef FEAT_GUI
		    && (!gui.లోపల_use
			|| (tp[*slen + j + 3] != NUL
			    && tp[*slen + j + 4] != NUL))
# endif
	       )
		*slen += j;
	    లేకపోతే
		ఆపు;
	}
    }

    ఒకవేళ (కీలకం_name[0] == KS_URXVT_MOUSE
	    || కీలకం_name[0] == KS_SGR_MOUSE
	    || కీలకం_name[0] == KS_SGR_MOUSE_RELEASE)
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
	p = modifiers_ప్రారంభం;
	ఒకవేళ (p == ఖాళీ)
	    తిరిగి -1;

	mouse_సంకేతం = getdigits(&p);
	ఒకవేళ (*p++ != ';')
	    తిరిగి -1;

	// when mouse reporting is SGR, add 32 to mouse code
	ఒకవేళ (కీలకం_name[0] == KS_SGR_MOUSE
		|| కీలకం_name[0] == KS_SGR_MOUSE_RELEASE)
	    mouse_సంకేతం += 32;

	mouse_నిలువు = getdigits(&p) - 1;
	ఒకవేళ (*p++ != ';')
	    తిరిగి -1;

	mouse_అడ్డువరుస = getdigits(&p) - 1;

	// The modifiers were the mouse coordinates, not the modifier keys
	// (alt/shift/ctrl/meta) state.
	*modifiers = 0;
    }

    ఒకవేళ (కీలకం_name[0] == KS_SGR_MOUSE
	    || కీలకం_name[0] == KS_SGR_MOUSE_RELEASE)
    {
	ఒకవేళ (కీలకం_name[0] == KS_SGR_MOUSE_RELEASE)
	{
	    is_release = TRUE;
	    // This is used below to set held_button.
	    mouse_సంకేతం |= MOUSE_RELEASE;
	}
    }
    లేకపోతే
    {
	release_is_ambiguous = TRUE;
	ఒకవేళ ((mouse_సంకేతం & MOUSE_RELEASE) == MOUSE_RELEASE)
	    is_release = TRUE;
    }

    ఒకవేళ (కీలకం_name[0] == KS_MOUSE
# ifdef FEAT_MOUSE_GPM
	    || కీలకం_name[0] == KS_GPM_MOUSE
# endif
# ifdef FEAT_MOUSE_URXVT
	    || కీలకం_name[0] == KS_URXVT_MOUSE
# endif
	    || కీలకం_name[0] == KS_SGR_MOUSE
	    || కీలకం_name[0] == KS_SGR_MOUSE_RELEASE)
    {
# ఒకవేళ !defined(MSWIN)
	/*
	 * Handle old style mouse events.
	 * Recognize the xterm mouse wheel, but not in the GUI, the
	 * Linux console with GPM and the MS-DOS or Win32 console
	 * (multi-clicks use >= 0x60).
	 */
	ఒకవేళ (mouse_సంకేతం >= MOUSEWHEEL_దిగువ && mouse_సంకేతం < MOUSESIDEBUTTONS_దిగువ
#  ifdef FEAT_GUI
		&& !gui.లోపల_use
#  endif
#  ifdef FEAT_MOUSE_GPM
		&& కీలకం_name[0] != KS_GPM_MOUSE
#  endif
	   )
	{
#  ఒకవేళ defined(UNIX)
	    ఒకవేళ (use_xterm_mouse() > 1 && mouse_సంకేతం >= 0x80)
		// mouse-move event, using MOUSE_DRAG works
		mouse_సంకేతం = MOUSE_DRAG;
	    లేకపోతే
#  endif
		// Keep the mouse_code before it's changed, so that we
		// remember that it was a mouse wheel click.
		wheel_సంకేతం = mouse_సంకేతం;
	}
#  ifdef FEAT_MOUSE_XTERM
	లేకపోతే ఒకవేళ (held_button == MOUSE_RELEASE
#   ifdef FEAT_GUI
		&& !gui.లోపల_use
#   endif
		&& (mouse_సంకేతం == 0x23 || mouse_సంకేతం == 0x24
		    || mouse_సంకేతం == 0x40 || mouse_సంకేతం == 0x41))
	{
	    // Apparently 0x23 and 0x24 are used by rxvt scroll wheel.
	    // And 0x40 and 0x41 are used by some xterm emulator.
	    wheel_సంకేతం = mouse_సంకేతం - (mouse_సంకేతం >= 0x40 ? 0x40 : 0x23)
							      + MOUSEWHEEL_దిగువ;
	}
#  endif

#  ఒకవేళ defined(UNIX)
	లేకపోతే ఒకవేళ (use_xterm_mouse() > 1)
	{
	    ఒకవేళ (mouse_సంకేతం & MOUSE_DRAG_XTERM)
		mouse_సంకేతం |= MOUSE_DRAG;
	}
#  endif
#  ifdef FEAT_XCLIPBOARD
	లేకపోతే ఒకవేళ (!(mouse_సంకేతం & MOUSE_DRAG & ~MOUSE_CLICK_ముసుగు))
	{
	    ఒకవేళ (is_release)
		stop_xterm_trace();
	    లేకపోతే
		ప్రారంభం_xterm_trace(mouse_సంకేతం);
	}
#  endif
# endif
    }
#అయితే_ముగింపు // !UNIX || FEAT_MOUSE_XTERM
#నిర్వచితమైతే FEAT_MOUSE_NET
    ఒకవేళ (కీలకం_name[0] == KS_NETTERM_MOUSE)
    {
	పూర్ణాంకం mc, mr;

	// expect a rather limited sequence like: balancing {
	// \033}6,45\r
	// '6' is the row, 45 is the column
	p = tp + *slen;
	mr = getdigits(&p);
	ఒకవేళ (*p++ != ',')
	    తిరిగి -1;
	mc = getdigits(&p);
	ఒకవేళ (*p++ != '\r')
	    తిరిగి -1;

	mouse_నిలువు = mc - 1;
	mouse_అడ్డువరుస = mr - 1;
	mouse_సంకేతం = MOUSE_ఎడమ;
	*slen += (పూర్ణాంకం)(p - (tp + *slen));
    }
#అయితే_ముగింపు	// FEAT_MOUSE_NET
#నిర్వచితమైతే FEAT_MOUSE_JSB
    ఒకవేళ (కీలకం_name[0] == KS_JSBTERM_MOUSE)
    {
	పూర్ణాంకం mult, val, iter, button, స్థితి_2;

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
	button = mouse_సంకేతం = 0;
	ఎంపిక (*p++)
	{
	    స్థితి 'L': button = 1; ఆపు;
	    స్థితి '-': ఆపు;
	    స్థితి 'x': ఆపు; // ignore sequence
	    అప్రమేయ:  తిరిగి -1; // Unknown Result
	}
	ఎంపిక (*p++)
	{
	    స్థితి 'M': button |= 2; ఆపు;
	    స్థితి '-': ఆపు;
	    స్థితి 'x': ఆపు; // ignore sequence
	    అప్రమేయ:  తిరిగి -1; // Unknown Result
	}
	ఎంపిక (*p++)
	{
	    స్థితి 'R': button |= 4; ఆపు;
	    స్థితి '-': ఆపు;
	    స్థితి 'x': ఆపు; // ignore sequence
	    అప్రమేయ:  తిరిగి -1; // Unknown Result
	}
	స్థితి_2 = *p++;
	కొరకు (val = 0, mult = 100, iter = 0; iter < 3; iter++,
		mult /= 10, p++)
	    ఒకవేళ (*p >= '0' && *p <= '9')
		val += (*p - '0') * mult;
	    లేకపోతే
		తిరిగి -1;
	mouse_నిలువు = val;
	కొరకు (val = 0, mult = 100, iter = 0; iter < 3; iter++,
		mult /= 10, p++)
	    ఒకవేళ (*p >= '0' && *p <= '9')
		val += (*p - '0') * mult;
	    లేకపోతే
		తిరిగి -1;
	mouse_అడ్డువరుస = val;
	ఎంపిక (*p++)
	{
	    స్థితి 's': button |= 8; ఆపు;  // SHIFT key Pressed
	    స్థితి '-': ఆపు;  // Not Pressed
	    స్థితి 'x': ఆపు;  // Not Reporting
	    అప్రమేయ:  తిరిగి -1; // Unknown Result
	}
	ఎంపిక (*p++)
	{
	    స్థితి 'c': button |= 16; ఆపు;  // CTRL key Pressed
	    స్థితి '-': ఆపు;  // Not Pressed
	    స్థితి 'x': ఆపు;  // Not Reporting
	    అప్రమేయ:  తిరిగి -1; // Unknown Result
	}
	ఒకవేళ (*p++ != '\033')
	    తిరిగి -1;
	ఒకవేళ (*p++ != '\\')
	    తిరిగి -1;
	ఎంపిక (స్థితి_2)
	{
	    స్థితి 'D': // Double Click
	    స్థితి 'S': // Single Click
		ఒకవేళ (button & 1) mouse_సంకేతం |= MOUSE_ఎడమ;
		ఒకవేళ (button & 2) mouse_సంకేతం |= MOUSE_మధ్య;
		ఒకవేళ (button & 4) mouse_సంకేతం |= MOUSE_కుడి;
		ఒకవేళ (button & 8) mouse_సంకేతం |= MOUSE_జరుపు;
		ఒకవేళ (button & 16) mouse_సంకేతం |= MOUSE_CTRL;
		ఆపు;
	    స్థితి 'm': // Mouse move
		ఒకవేళ (button & 1) mouse_సంకేతం |= MOUSE_ఎడమ;
		ఒకవేళ (button & 2) mouse_సంకేతం |= MOUSE_మధ్య;
		ఒకవేళ (button & 4) mouse_సంకేతం |= MOUSE_కుడి;
		ఒకవేళ (button & 8) mouse_సంకేతం |= MOUSE_జరుపు;
		ఒకవేళ (button & 16) mouse_సంకేతం |= MOUSE_CTRL;
		ఒకవేళ ((button & 7) != 0)
		{
		    held_button = mouse_సంకేతం;
		    mouse_సంకేతం |= MOUSE_DRAG;
		}
		is_drag = TRUE;
		showmode();
		ఆపు;
	    స్థితి 'd': // Button Down
		ఒకవేళ (button & 1) mouse_సంకేతం |= MOUSE_ఎడమ;
		ఒకవేళ (button & 2) mouse_సంకేతం |= MOUSE_మధ్య;
		ఒకవేళ (button & 4) mouse_సంకేతం |= MOUSE_కుడి;
		ఒకవేళ (button & 8) mouse_సంకేతం |= MOUSE_జరుపు;
		ఒకవేళ (button & 16) mouse_సంకేతం |= MOUSE_CTRL;
		ఆపు;
	    స్థితి 'u': // Button Up
		is_release = TRUE;
		ఒకవేళ (button & 1)
		    mouse_సంకేతం |= MOUSE_ఎడమ;
		ఒకవేళ (button & 2)
		    mouse_సంకేతం |= MOUSE_మధ్య;
		ఒకవేళ (button & 4)
		    mouse_సంకేతం |= MOUSE_కుడి;
		ఒకవేళ (button & 8)
		    mouse_సంకేతం |= MOUSE_జరుపు;
		ఒకవేళ (button & 16)
		    mouse_సంకేతం |= MOUSE_CTRL;
		ఆపు;
	    అప్రమేయ: తిరిగి -1; // Unknown Result
	}

	*slen += (p - (tp + *slen));
    }
#అయితే_ముగింపు // FEAT_MOUSE_JSB
#నిర్వచితమైతే FEAT_MOUSE_DEC
    ఒకవేళ (కీలకం_name[0] == KS_DEC_MOUSE)
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
	పూర్ణాంకం Pe, Pb, Pr, Pc;

	p = tp + *slen;

	// get event status
	Pe = getdigits(&p);
	ఒకవేళ (*p++ != ';')
	    తిరిగి -1;

	// get button status
	Pb = getdigits(&p);
	ఒకవేళ (*p++ != ';')
	    తిరిగి -1;

	// get row status
	Pr = getdigits(&p);
	ఒకవేళ (*p++ != ';')
	    తిరిగి -1;

	// get column status
	Pc = getdigits(&p);

	// the page parameter is optional
	ఒకవేళ (*p == ';')
	{
	    p++;
	    (శూన్య)getdigits(&p);
	}
	ఒకవేళ (*p++ != '&')
	    తిరిగి -1;
	ఒకవేళ (*p++ != 'w')
	    తిరిగి -1;

	mouse_సంకేతం = 0;
	ఎంపిక (Pe)
	{
	    స్థితి  0: తిరిగి -1; // position request while unavailable
	    స్థితి  1: // a response to a locator position request includes
		     //	the status of all buttons
		     Pb &= 7;   // mask off and ignore fourth button
		     ఒకవేళ (Pb & 4)
			 mouse_సంకేతం  = MOUSE_ఎడమ;
		     ఒకవేళ (Pb & 2)
			 mouse_సంకేతం  = MOUSE_మధ్య;
		     ఒకవేళ (Pb & 1)
			 mouse_సంకేతం  = MOUSE_కుడి;
		     ఒకవేళ (Pb)
		     {
			 held_button = mouse_సంకేతం;
			 mouse_సంకేతం |= MOUSE_DRAG;
			 WantQueryMouse = TRUE;
		     }
		     is_drag = TRUE;
		     showmode();
		     ఆపు;
	    స్థితి  2: mouse_సంకేతం = MOUSE_ఎడమ;
		     WantQueryMouse = TRUE;
		     ఆపు;
	    స్థితి  3: mouse_సంకేతం = MOUSE_ఎడమ;
		     is_release = TRUE;
		     ఆపు;
	    స్థితి  4: mouse_సంకేతం = MOUSE_మధ్య;
		     WantQueryMouse = TRUE;
		     ఆపు;
	    స్థితి  5: mouse_సంకేతం = MOUSE_మధ్య;
		     is_release = TRUE;
		     ఆపు;
	    స్థితి  6: mouse_సంకేతం = MOUSE_కుడి;
		     WantQueryMouse = TRUE;
		     ఆపు;
	    స్థితి  7: mouse_సంకేతం = MOUSE_కుడి;
		     is_release = TRUE;
		     ఆపు;
	    స్థితి  8: తిరిగి -1; // fourth button down
	    స్థితి  9: తిరిగి -1; // fourth button up
	    స్థితి 10: తిరిగి -1; // mouse outside of filter rectangle
	    అప్రమేయ: తిరిగి -1; // should never occur
	}

	mouse_నిలువు = Pc - 1;
	mouse_అడ్డువరుస = Pr - 1;

	*slen += (పూర్ణాంకం)(p - (tp + *slen));
    }
#అయితే_ముగింపు // FEAT_MOUSE_DEC
#నిర్వచితమైతే FEAT_MOUSE_PTERM
    ఒకవేళ (కీలకం_name[0] == KS_PTERM_MOUSE)
    {
	పూర్ణాంకం button, అంకె_clicks, action;

	p = tp + *slen;

	action = getdigits(&p);
	ఒకవేళ (*p++ != ';')
	    తిరిగి -1;

	mouse_అడ్డువరుస = getdigits(&p);
	ఒకవేళ (*p++ != ';')
	    తిరిగి -1;
	mouse_నిలువు = getdigits(&p);
	ఒకవేళ (*p++ != ';')
	    తిరిగి -1;

	button = getdigits(&p);
	mouse_సంకేతం = 0;

	ఎంపిక (button)
	{
	    స్థితి 4: mouse_సంకేతం = MOUSE_ఎడమ; ఆపు;
	    స్థితి 1: mouse_సంకేతం = MOUSE_కుడి; ఆపు;
	    స్థితి 2: mouse_సంకేతం = MOUSE_మధ్య; ఆపు;
	    అప్రమేయ: తిరిగి -1;
	}

	ఎంపిక (action)
	{
	    స్థితి 31: // Initial press
		ఒకవేళ (*p++ != ';')
		    తిరిగి -1;

		అంకె_clicks = getdigits(&p); // Not used
		ఆపు;

	    స్థితి 32: // Release
		is_release = TRUE;
		ఆపు;

	    స్థితి 33: // Drag
		held_button = mouse_సంకేతం;
		mouse_సంకేతం |= MOUSE_DRAG;
		ఆపు;

	    అప్రమేయ:
		తిరిగి -1;
	}

	ఒకవేళ (*p++ != 't')
	    తిరిగి -1;

	*slen += (p - (tp + *slen));
    }
#అయితే_ముగింపు // FEAT_MOUSE_PTERM

    // Interpret the mouse code
    current_button = (mouse_సంకేతం & MOUSE_CLICK_ముసుగు);
    ఒకవేళ (is_release)
	current_button |= MOUSE_RELEASE;

    ఒకవేళ (current_button == MOUSE_RELEASE
#నిర్వచితమైతే FEAT_MOUSE_XTERM
	    && wheel_సంకేతం == 0
#అయితే_ముగింపు
       )
    {
	/*
	 * If we get a mouse drag or release event when there is no mouse
	 * button held down (held_button == MOUSE_RELEASE), produce a K_IGNORE
	 * below.
	 * (can happen when you hold down two buttons and then let them go, or
	 * click in the menu bar, but not on a menu, and drag into the text).
	 */
	ఒకవేళ ((mouse_సంకేతం & MOUSE_DRAG) == MOUSE_DRAG)
	    is_drag = TRUE;
	current_button = held_button;
    }
    లేకపోతే
    {
      ఒకవేళ (wheel_సంకేతం == 0)
      {
#నిర్వచితమైతే CHECK_DOUBLE_CLICK
# ifdef FEAT_MOUSE_GPM
	/*
	 * Only for Unix, when GUI not active, we handle multi-clicks here, but
	 * not for GPM mouse events.
	 */
#  ifdef FEAT_GUI
	ఒకవేళ (కీలకం_name[0] != KS_GPM_MOUSE && !gui.లోపల_use)
#  లేకపోతే
	    ఒకవేళ (కీలకం_name[0] != KS_GPM_MOUSE)
#  endif
# లేకపోతే
#  ifdef FEAT_GUI
		ఒకవేళ (!gui.లోపల_use)
#  endif
# endif
		{
		    /*
		     * Compute the time elapsed since the previous mouse click.
		     */
		    gettimeofday(&mouse_సమయం, ఖాళీ);
		    ఒకవేళ (orig_mouse_సమయం.tv_sec == 0)
		    {
			/*
			 * Avoid computing the difference between mouse_time
			 * and orig_mouse_time for the first click, as the
			 * difference would be huge and would cause
			 * multiplication overflow.
			 */
			timediff = p_mouset;
		    }
		    లేకపోతే
			timediff = సమయం_diff_ms(&orig_mouse_సమయం, &mouse_సమయం);
		    orig_mouse_సమయం = mouse_సమయం;
		    ఒకవేళ (mouse_సంకేతం == orig_mouse_సంకేతం
			    && timediff < p_mouset
			    && orig_అంకె_clicks != 4
			    && orig_mouse_నిలువు == mouse_నిలువు
			    && orig_mouse_అడ్డువరుస == mouse_అడ్డువరుస
			    && (is_mouse_topline(curwin)
				// Double click in tab pages line also works
				// when window contents changes.
				|| (mouse_అడ్డువరుస == 0 && firstwin->w_winrow > 0))
		       )
			++orig_అంకె_clicks;
		    లేకపోతే
			orig_అంకె_clicks = 1;
		    orig_mouse_నిలువు = mouse_నిలువు;
		    orig_mouse_అడ్డువరుస = mouse_అడ్డువరుస;
		    సమితి_mouse_topline(curwin);
		}
# ఒకవేళ defined(FEAT_GUI) || defined(FEAT_MOUSE_GPM)
		లేకపోతే
		    orig_అంకె_clicks = అంకె_MOUSE_CLICKS(mouse_సంకేతం);
# endif
#లేకపోతే
	orig_అంకె_clicks = అంకె_MOUSE_CLICKS(mouse_సంకేతం);
#అయితే_ముగింపు
	is_click = TRUE;
      }
      orig_mouse_సంకేతం = mouse_సంకేతం;
    }
    ఒకవేళ (!is_drag)
	held_button = mouse_సంకేతం & MOUSE_CLICK_ముసుగు;

    /*
     * Translate the actual mouse event into a pseudo mouse event.
     * First work out what modifiers are to be used.
     */
    ఒకవేళ (orig_mouse_సంకేతం & MOUSE_జరుపు)
	*modifiers |= MOD_ముసుగు_జరుపు;
    ఒకవేళ (orig_mouse_సంకేతం & MOUSE_CTRL)
	*modifiers |= MOD_ముసుగు_CTRL;
    ఒకవేళ (orig_mouse_సంకేతం & MOUSE_ALT)
	*modifiers |= MOD_ముసుగు_ALT;
    ఒకవేళ (orig_అంకె_clicks == 2)
	*modifiers |= MOD_ముసుగు_2_CLICK;
    లేకపోతే ఒకవేళ (orig_అంకె_clicks == 3)
	*modifiers |= MOD_ముసుగు_3_CLICK;
    లేకపోతే ఒకవేళ (orig_అంకె_clicks == 4)
	*modifiers |= MOD_ముసుగు_4_CLICK;

    // Work out our pseudo mouse event. Note that MOUSE_RELEASE gets added,
    // then it's not mouse up/down.
    కీలకం_name[0] = KS_EXTRA;
    ఒకవేళ (wheel_సంకేతం != 0 && (!is_release || release_is_ambiguous))
    {
	ఒకవేళ (wheel_సంకేతం & MOUSE_CTRL)
	    *modifiers |= MOD_ముసుగు_CTRL;
	ఒకవేళ (wheel_సంకేతం & MOUSE_ALT)
	    *modifiers |= MOD_ముసుగు_ALT;

	ఒకవేళ (wheel_సంకేతం & 1 && wheel_సంకేతం & 2)
	    కీలకం_name[1] = (పూర్ణాంకం)KE_MOUSELEFT;
	లేకపోతే ఒకవేళ (wheel_సంకేతం & 2)
	    కీలకం_name[1] = (పూర్ణాంకం)KE_MOUSERIGHT;
	లేకపోతే ఒకవేళ (wheel_సంకేతం & 1)
	    కీలకం_name[1] = (పూర్ణాంకం)KE_MOUSEUP;
	లేకపోతే
	    కీలకం_name[1] = (పూర్ణాంకం)KE_MOUSEDOWN;

	held_button = MOUSE_RELEASE;
    }
    లేకపోతే
    {
#ఒకవేళ defined(UNIX)
	ఒకవేళ (use_xterm_mouse() && orig_mouse_సంకేతం >= MOUSESIDEBUTTONS_దిగువ)
	    current_button = (current_button) ? MOUSE_X2 : MOUSE_X1;
#అయితే_ముగింపు
	కీలకం_name[1] = get_pseudo_mouse_సంకేతం(current_button, is_click, is_drag);
    }


    // Make sure the mouse position is valid.  Some terminals may return weird
    // values.
    ఒకవేళ (mouse_నిలువు >= Columns)
	mouse_నిలువు = Columns - 1;
    ఒకవేళ (mouse_అడ్డువరుస >= అడ్డువరుసలు)
	mouse_అడ్డువరుస = అడ్డువరుసలు - 1;

    తిరిగి 0;
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
    పూర్ణాంకం
mouse_comp_pos(
    win_T	*win,
    పూర్ణాంకం		*rowp,
    పూర్ణాంకం		*colp,
    linenr_T	*lnump,
    పూర్ణాంకం		*plines_cache)
{
    పూర్ణాంకం		నిలువు = *colp;
    పూర్ణాంకం		అడ్డువరుస = *rowp;
    linenr_T	lnum;
    పూర్ణాంకం		retval = FALSE;
    పూర్ణాంకం		off;
    పూర్ణాంకం		లెక్క;

#నిర్వచితమైతే FEAT_RIGHTLEFT
    ఒకవేళ (win->w_p_rl)
	నిలువు = win->w_వెడల్పు - 1 - నిలువు;
#అయితే_ముగింపు

    lnum = win->w_topline;

    అయితే (అడ్డువరుస > 0)
    {
	పూర్ణాంకం cache_idx = lnum - win->w_topline;

	// Only "Rows" lines are cached, with folding we'll run out of entries
	// and use the slow way.
	ఒకవేళ (plines_cache != ఖాళీ && cache_idx < అడ్డువరుసలు
						&& plines_cache[cache_idx] > 0)
	    లెక్క = plines_cache[cache_idx];
	లేకపోతే
	{
#నిర్వచితమైతే FEAT_DIFF
	    // Don't include filler lines in "count"
	    ఒకవేళ (win->w_p_diff
# ifdef FEAT_FOLDING
		    && !hasFoldingWin(win, lnum, ఖాళీ, ఖాళీ, TRUE, ఖాళీ)
# endif
		    )
	    {
		ఒకవేళ (lnum == win->w_topline)
		    అడ్డువరుస -= win->w_topfill;
		లేకపోతే
		    అడ్డువరుస -= diff_check_fill(win, lnum);
		లెక్క = plines_win_nofill(win, lnum, FALSE);
	    }
	    లేకపోతే
#అయితే_ముగింపు
		లెక్క = plines_win(win, lnum, FALSE);
	    ఒకవేళ (plines_cache != ఖాళీ && cache_idx < అడ్డువరుసలు)
		plines_cache[cache_idx] = లెక్క;
	}

	ఒకవేళ (win->w_skipcol > 0 && lnum == win->w_topline)
	{
	    పూర్ణాంకం width1 = win->w_వెడల్పు - win_నిలువు_off(win);

	    ఒకవేళ (width1 > 0)
	    {
		పూర్ణాంకం skip_పంక్తులు = 0;

		// Adjust for 'smoothscroll' clipping the top screen lines.
		// A similar formula is used in curs_columns().
		ఒకవేళ (win->w_skipcol > width1)
		    skip_పంక్తులు = (win->w_skipcol - width1)
					    / (width1 + win_నిలువు_off2(win)) + 1;
		లేకపోతే ఒకవేళ (win->w_skipcol > 0)
		    skip_పంక్తులు = 1;

		లెక్క -= skip_పంక్తులు;
	    }
	}

	ఒకవేళ (లెక్క > అడ్డువరుస)
	    ఆపు;	// Position is in this buffer line.
#నిర్వచితమైతే FEAT_FOLDING
	(శూన్య)hasFoldingWin(win, lnum, ఖాళీ, &lnum, TRUE, ఖాళీ);
#అయితే_ముగింపు
	ఒకవేళ (lnum == win->w_బఫరు->b_ml.ml_పంక్తి_లెక్క)
	{
	    retval = TRUE;
	    ఆపు;		// past end of file
	}
	అడ్డువరుస -= లెక్క;
	++lnum;
    }

    ఒకవేళ (!retval)
    {
	// Compute the column without wrapping.
	off = win_నిలువు_off(win) - win_నిలువు_off2(win);
	ఒకవేళ (నిలువు < off)
	    నిలువు = off;
	నిలువు += అడ్డువరుస * (win->w_వెడల్పు - off);

	// Add skip column for the topline.
	ఒకవేళ (lnum == win->w_topline)
	    నిలువు += win->w_skipcol;
    }

    ఒకవేళ (!win->w_p_wrap)
	నిలువు += win->w_leftcol;

    // skip line number and fold column in front of the line
    నిలువు -= win_నిలువు_off(win);
    ఒకవేళ (నిలువు <= 0)
    {
#నిర్వచితమైతే FEAT_NETBEANS_INTG
	// if mouse is clicked on the gutter, then inform the netbeans server
	ఒకవేళ (*colp < win_నిలువు_off(win))
	    netbeans_gutter_click(lnum);
#అయితే_ముగింపు
	నిలువు = 0;
    }

    *colp = నిలువు;
    *rowp = అడ్డువరుస;
    *lnump = lnum;
    తిరిగి retval;
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
mouse_కనుగొను_win(పూర్ణాంకం *rowp, పూర్ణాంకం *colp, mouse_కనుగొను_T popup వాడని)
{
    చట్రం_T	*fp;
    win_T	*wp;

#నిర్వచితమైతే FEAT_PROP_POPUP
    win_T	*pwp = ఖాళీ;

    ఒకవేళ (popup != IGNORE_POPUP)
    {
	popup_reset_handled(POPUP_HANDLED_1);
	అయితే ((wp = కనుగొను_తదుపరి_popup(TRUE, POPUP_HANDLED_1)) != ఖాళీ)
	{
	    ఒకవేళ (*rowp >= wp->w_winrow && *rowp < wp->w_winrow + popup_ఎత్తు(wp)
		    && *colp >= wp->w_wincol
				    && *colp < wp->w_wincol + popup_వెడల్పు(wp))
		pwp = wp;
	}
	ఒకవేళ (pwp != ఖాళీ)
	{
	    ఒకవేళ (popup == FAIL_POPUP)
		తిరిగి ఖాళీ;
	    *rowp -= pwp->w_winrow;
	    *colp -= pwp->w_wincol;
	    తిరిగి pwp;
	}
    }
#అయితే_ముగింపు

    fp = topframe;

    ఒకవేళ (*colp < firstwin->w_wincol
	    || *colp >= firstwin->w_wincol + fp->fr_వెడల్పు
	    || *rowp < firstwin->w_winrow)
	తిరిగి ఖాళీ;

    *rowp -= firstwin->w_winrow;
    *colp -= firstwin->w_wincol;
    కొరకు (;;)
    {
	ఒకవేళ (fp->fr_layout == FR_ఆకు)
	    ఆపు;
	ఒకవేళ (fp->fr_layout == FR_అడ్డువరుస)
	{
	    కొరకు (fp = fp->fr_బిడ్డ; fp->fr_తదుపరి != ఖాళీ; fp = fp->fr_తదుపరి)
	    {
		ఒకవేళ (*colp < fp->fr_వెడల్పు)
		    ఆపు;
		*colp -= fp->fr_వెడల్పు;
	    }
	}
	లేకపోతే    // fr_layout == FR_COL
	{
	    కొరకు (fp = fp->fr_బిడ్డ; fp->fr_తదుపరి != ఖాళీ; fp = fp->fr_తదుపరి)
	    {
		ఒకవేళ (*rowp < fp->fr_ఎత్తు)
		    ఆపు;
		*rowp -= fp->fr_ఎత్తు;
	    }
	}
    }
    // When using a timer that closes a window the window might not actually
    // exist.
    FOR_ALL_WINDOWS(wp)
	ఒకవేళ (wp == fp->fr_win)
	{
#నిర్వచితమైతే FEAT_MENU
	    *rowp -= wp->w_winbar_ఎత్తు;
#అయితే_ముగింపు
	    తిరిగి wp;
	}
    తిరిగి ఖాళీ;
}

#ఒకవేళ defined(NEED_VCOL2_నిలువు) || defined(FEAT_BEVAL) || defined(FEAT_PROP_POPUP) \
	|| defined(FEAT_EVAL)
/*
 * Convert a virtual (screen) column to a character column.
 * The first column is zero.
 */
    పూర్ణాంకం
vcol2col(win_T *wp, linenr_T lnum, పూర్ణాంకం vcol, colnr_T *coladdp)
{
    char_u	    *పంక్తి;
    chartabsize_T   cts;

    // try to advance to the specified column
    పంక్తి = ml_get_buf(wp->w_బఫరు, lnum, FALSE);
    init_chartabsize_arg(&cts, wp, lnum, 0, పంక్తి, పంక్తి);
    అయితే (cts.cts_vcol < vcol && *cts.cts_ptr != NUL)
    {
	పూర్ణాంకం కొలత = win_lbr_chartabsize(&cts, ఖాళీ, ఖాళీ);
	ఒకవేళ (cts.cts_vcol + కొలత > vcol)
	    ఆపు;
	cts.cts_vcol += కొలత;
	MB_PTR_ADV(cts.cts_ptr);
    }
    clear_chartabsize_arg(&cts);

    ఒకవేళ (coladdp != ఖాళీ)
	*coladdp = vcol - cts.cts_vcol;
    తిరిగి (పూర్ణాంకం)(cts.cts_ptr - పంక్తి);
}
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_EVAL)
/*
 * "getmousepos()" function.
 */
    శూన్య
f_getmousepos(typval_T *argvars వాడని, typval_T *rettv)
{
    dict_T	*d;
    win_T	*wp;
    పూర్ణాంకం		అడ్డువరుస = mouse_అడ్డువరుస;
    పూర్ణాంకం		నిలువు = mouse_నిలువు;
    varnumber_T winid = 0;
    varnumber_T winrow = 0;
    varnumber_T wincol = 0;
    linenr_T	lnum = 0;
    varnumber_T స్తంభం = 0;
    colnr_T	coladd = 0;

    ఒకవేళ (rettv_dict_alloc(rettv) == FAIL)
	తిరిగి;
    d = rettv->vval.v_dict;

    dict_కూడు_సంఖ్య(d, "screenrow", (varnumber_T)mouse_అడ్డువరుస + 1);
    dict_కూడు_సంఖ్య(d, "screencol", (varnumber_T)mouse_నిలువు + 1);

    wp = mouse_కనుగొను_win(&అడ్డువరుస, &నిలువు, కనుగొను_POPUP);
    ఒకవేళ (wp != ఖాళీ)
    {
	పూర్ణాంకం	శిఖరం_off = 0;
	పూర్ణాంకం	ఎడమ_off = 0;
	పూర్ణాంకం	ఎత్తు = wp->w_ఎత్తు + wp->w_స్థితి_ఎత్తు;

# ifdef FEAT_PROP_POPUP
	ఒకవేళ (WIN_IS_POPUP(wp))
	{
	    శిఖరం_off = popup_శిఖరం_extra(wp);
	    ఎడమ_off = popup_ఎడమ_extra(wp);
	    ఎత్తు = popup_ఎత్తు(wp);
	}
# endif
	ఒకవేళ (అడ్డువరుస < ఎత్తు)
	{
	    winid = wp->w_id;
	    winrow = అడ్డువరుస + 1;
	    wincol = నిలువు + 1;
	    అడ్డువరుస -= శిఖరం_off;
	    నిలువు -= ఎడమ_off;
	    ఒకవేళ (అడ్డువరుస >= 0 && అడ్డువరుస < wp->w_ఎత్తు && నిలువు >= 0 && నిలువు < wp->w_వెడల్పు)
	    {
		(శూన్య)mouse_comp_pos(wp, &అడ్డువరుస, &నిలువు, &lnum, ఖాళీ);
		నిలువు = vcol2col(wp, lnum, నిలువు, &coladd);
		స్తంభం = నిలువు + 1;
	    }
	}
    }
    dict_కూడు_సంఖ్య(d, "winid", winid);
    dict_కూడు_సంఖ్య(d, "winrow", winrow);
    dict_కూడు_సంఖ్య(d, "wincol", wincol);
    dict_కూడు_సంఖ్య(d, "line", (varnumber_T)lnum);
    dict_కూడు_సంఖ్య(d, "column", స్తంభం);
    dict_కూడు_సంఖ్య(d, "coladd", coladd);
}
#అయితే_ముగింపు
