/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

/*
 * edit.c: functions for Insert mode
 */

#అన్తర్భావ "vim.h"

#నిర్వచించు BACKSPACE_CHAR		    1
#నిర్వచించు BACKSPACE_మాట		    2
#నిర్వచించు BACKSPACE_మాట_NOT_SPACE    3
#నిర్వచించు BACKSPACE_పంక్తి		    4

// Set when doing something for completion that may call edit() recursively,
// which is not allowed.
స్థిర పూర్ణాంకం	compl_busy = FALSE;


స్థిర శూన్య ins_ctrl_v(శూన్య);
స్థిర శూన్య చొప్పించు_special(పూర్ణాంకం, పూర్ణాంకం, పూర్ణాంకం);
స్థిర శూన్య redo_literal(పూర్ణాంకం c);
స్థిర శూన్య ప్రారంభం_arrow_common(pos_T *చివర_చొప్పించు_pos, పూర్ణాంకం change);
#నిర్వచితమైతే FEAT_SPELL
స్థిర శూన్య check_spell_redraw(శూన్య);
#అయితే_ముగింపు
స్థిర శూన్య stop_చొప్పించు(pos_T *చివర_చొప్పించు_pos, పూర్ణాంకం esc, పూర్ణాంకం nomove);
స్థిర పూర్ణాంకం  echeck_abbr(పూర్ణాంకం);
స్థిర శూన్య mb_replace_తీయి_ins(పూర్ణాంకం cc);
స్థిర శూన్య replace_flush(శూన్య);
స్థిర శూన్య replace_do_bs(పూర్ణాంకం హద్దు_నిలువు);
స్థిర పూర్ణాంకం del_char_after_నిలువు(పూర్ణాంకం హద్దు_నిలువు);
స్థిర శూన్య ins_reg(శూన్య);
స్థిర శూన్య ins_ctrl_g(శూన్య);
స్థిర శూన్య ins_ctrl_hat(శూన్య);
స్థిర పూర్ణాంకం  ins_esc(దీర్ఘ *లెక్క, పూర్ణాంకం cmdchar, పూర్ణాంకం nomove);
#నిర్వచితమైతే FEAT_RIGHTLEFT
స్థిర శూన్య ins_ctrl_(శూన్య);
#అయితే_ముగింపు
స్థిర పూర్ణాంకం ins_ప్రారంభం_select(పూర్ణాంకం c);
స్థిర శూన్య ins_చొప్పించు(పూర్ణాంకం replaceState);
స్థిర శూన్య ins_ctrl_o(శూన్య);
స్థిర శూన్య ins_జరుపు(పూర్ణాంకం c, పూర్ణాంకం lastc);
స్థిర శూన్య ins_del(శూన్య);
స్థిర పూర్ణాంకం  ins_bs(పూర్ణాంకం c, పూర్ణాంకం బహుళకం, పూర్ణాంకం *inserted_space_p);
#ఒకవేళ defined(FEAT_GUI_TABLINE)
స్థిర శూన్య ins_tabline(పూర్ణాంకం c);
#అయితే_ముగింపు
స్థిర శూన్య ins_ఎడమ(శూన్య);
స్థిర శూన్య ins_home(పూర్ణాంకం c);
స్థిర శూన్య ins_చివర(పూర్ణాంకం c);
స్థిర శూన్య ins_s_ఎడమ(శూన్య);
స్థిర శూన్య ins_కుడి(శూన్య);
స్థిర శూన్య ins_s_కుడి(శూన్య);
స్థిర శూన్య ins_పైకి(పూర్ణాంకం startcol);
స్థిర శూన్య ins_pageup(శూన్య);
స్థిర శూన్య ins_కిందికి(పూర్ణాంకం startcol);
స్థిర శూన్య ins_pagedown(శూన్య);
#నిర్వచితమైతే FEAT_DND
స్థిర శూన్య ins_drop(శూన్య);
#అయితే_ముగింపు
స్థిర పూర్ణాంకం  ins_tab(శూన్య);
#నిర్వచితమైతే FEAT_DIGRAPHS
స్థిర పూర్ణాంకం  ins_digraph(శూన్య);
#అయితే_ముగింపు
స్థిర పూర్ణాంకం  ins_ctrl_ey(పూర్ణాంకం tc);
#ఒకవేళ defined(FEAT_EVAL)
స్థిర char_u *do_చొప్పించు_char_pre(పూర్ణాంకం c);
#అయితే_ముగింపు

స్థిర colnr_T	Insstart_textlen;	// length of line when insert started
స్థిర colnr_T	Insstart_blank_vcol;	// vcol for first inserted blank
స్థిర పూర్ణాంకం	update_Insstart_orig = TRUE; // set Insstart_orig to Insstart

స్థిర string_T	చివరి_చొప్పించు = {ఖాళీ, 0};    // the text of the previous insert,
					    // K_SPECIAL and CSI are escaped
స్థిర పూర్ణాంకం	చివరి_చొప్పించు_skip; // nr of chars in front of previous insert
స్థిర పూర్ణాంకం	new_చొప్పించు_skip;  // nr of chars in front of current insert
స్థిర పూర్ణాంకం	did_restart_edit;	// "restart_edit" when calling edit()

స్థిర పూర్ణాంకం	can_cindent;		// may do cindenting on this line

#నిర్వచితమైతే FEAT_RIGHTLEFT
స్థిర పూర్ణాంకం	revins_on;		// reverse insert mode on
స్థిర పూర్ణాంకం	revins_chars;		// how much to skip after edit
స్థిర పూర్ణాంకం	revins_legal;		// was the last char 'legal'?
స్థిర పూర్ణాంకం	revins_scol;		// start column of revins session
#అయితే_ముగింపు

స్థిర పూర్ణాంకం	ins_need_undo;		// call u_save() before inserting a
					// char.  Set when edit() is called.
					// after that arrow_used is used.

స్థిర పూర్ణాంకం	dont_sync_undo = FALSE;	// CTRL-G U prevents syncing undo for
					// the next left/right cursor key

// With 'autocompletedelay' set, arm the delay and let the main loop fire
// Insert-mode autocommands; the popup is shown later on K_COMPLETE_DELAY.
// Otherwise trigger completion right away.
#నిర్వచించు TRIGGER_AUTOCOMPLETE()			\
    చేయి {					\
	update_screen(UPD_VALID);  /* Show char (deletion) immediately */ \
	బహిర్గతం_flush();				\
	ins_compl_enable_autocomplete();	\
	ins_compl_arm_autostart();		\
	ఒకవేళ (!ins_compl_arm_autocomplete_delay())\
	    వెళ్ళు docomplete;			\
    } అయితే (0)

#నిర్వచించు MAY_TRIGGER_AUTOCOMPLETE(c)				\
    చేయి {							\
	ఒకవేళ (ins_compl_has_autocomplete() && !char_avail()	\
		&& curwin->w_cursor.నిలువు > 0)			\
	{							\
	    (c) = char_before_cursor();				\
	    ఒకవేళ (vim_isprintc(c))				\
		TRIGGER_AUTOCOMPLETE();				\
	}							\
    } అయితే (0)

/*
 * edit(): Start inserting text.
 *
 * "cmdchar" can be:
 * 'i'	normal insert command
 * 'a'	normal append command
 * K_PS bracketed paste
 * 'R'	replace command
 * 'r'	"r<CR>" command: insert one <CR>.  Note: count can be > 1, for redo,
 *	but still only one <CR> is inserted.  The <Esc> is not used for redo.
 * 'g'	"gI" command.
 * 'V'	"gR" command for Virtual Replace mode.
 * 'v'	"gr" command for single character Virtual Replace mode.
 *
 * This function is not called recursively.  For CTRL-O commands, it returns
 * and lets the caller handle the Normal-mode command.
 *
 * Return TRUE if a CTRL-O command caused the return (insert mode pending).
 */
    పూర్ణాంకం
edit(
    పూర్ణాంకం		cmdchar,
    పూర్ణాంకం		startln,	// if set, insert at start of line
    దీర్ఘ	లెక్క)
{
    పూర్ణాంకం		c = 0;
    char_u	*ptr;
    పూర్ణాంకం		lastc = 0;
    పూర్ణాంకం		mincol;
    స్థిర linenr_T o_lnum = 0;
    పూర్ణాంకం		i;
    పూర్ణాంకం		did_backspace = TRUE;	    // previous char was backspace
    పూర్ణాంకం		పంక్తి_is_white = FALSE;	    // line is empty before insert
    linenr_T	old_topline = 0;	    // topline before insertion
#నిర్వచితమైతే FEAT_DIFF
    పూర్ణాంకం		old_topfill = -1;
#అయితే_ముగింపు
    పూర్ణాంకం		inserted_space = FALSE;     // just inserted a space
    పూర్ణాంకం		replaceState = బహుళకం_REPLACE;
    పూర్ణాంకం		nomove = FALSE;		    // don't move cursor on return
#నిర్వచితమైతే FEAT_పని_CHANNEL
    పూర్ణాంకం		cmdchar_todo = cmdchar;
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_CONCEAL
    పూర్ణాంకం		cursor_పంక్తి_was_concealed;
#అయితే_ముగింపు
    పూర్ణాంకం		ins_just_started = TRUE;

    // Remember whether editing was restarted after CTRL-O.
    did_restart_edit = restart_edit;

    // sleep before redrawing, needed for "CTRL-O :" that results in an
    // error message
    check_for_delay(TRUE);

    // set Insstart_orig to Insstart
    update_Insstart_orig = TRUE;

#నిర్వచితమైతే HAVE_SANDBOX
    // Don't allow inserting in the sandbox.
    ఒకవేళ (sandbox != 0)
    {
	emsg(_(e_not_allowed_లోపల_sandbox));
	తిరిగి FALSE;
    }
#అయితే_ముగింపు
    // Don't allow changes in the buffer while editing the cmdline.  The
    // caller of getcmdline() may get confused.
    // Don't allow recursive insert mode when busy with completion.
    ఒకవేళ (textlock != 0 || ins_compl_చురుకైన() || compl_busy || pum_visible())
    {
	emsg(_(e_not_allowed_గమ్యం_change_పాఠ్యం_or_change_కిటికీ));
	తిరిగి FALSE;
    }
    ins_compl_clear();	    // clear stuff for CTRL-X mode

    /*
     * Trigger InsertEnter autocommands.  Do not do this for "r<CR>" or "grx".
     */
    ఒకవేళ (cmdchar != 'r' && cmdchar != 'v')
    {
	pos_T   save_cursor = curwin->w_cursor;

#నిర్వచితమైతే FEAT_EVAL
	ఒకవేళ (cmdchar == 'R')
	    ptr = (char_u *)"r";
	లేకపోతే ఒకవేళ (cmdchar == 'V')
	    ptr = (char_u *)"v";
	లేకపోతే
	    ptr = (char_u *)"i";
	సమితి_vim_var_string(VV_INSERTMODE, ptr, 1);
	సమితి_vim_var_string(VV_CHAR, ఖాళీ, -1);  // clear v:char
#అయితే_ముగింపు
	ins_apply_autocmds(EVENT_INSERTENTER);

	// Check for changed highlighting, e.g. for ModeMsg.
	ఒకవేళ (need_highlight_మారింది)
	    highlight_మారింది();

	// Make sure the cursor didn't move.  Do call check_cursor_col() in
	// case the text was modified.  Since Insert mode was not started yet
	// a call to check_cursor_col() may move the cursor, especially with
	// the "A" command, thus set State to avoid that. Also check that the
	// line number is still valid (lines may have been deleted).
	// Do not restore if v:char was set to a non-empty string.
	ఒకవేళ (!EQUAL_POS(curwin->w_cursor, save_cursor)
#నిర్వచితమైతే FEAT_EVAL
		&& *get_vim_var_str(VV_CHAR) == NUL
#అయితే_ముగింపు
		&& save_cursor.lnum <= curbuf->b_ml.ml_పంక్తి_లెక్క)
	{
	    పూర్ణాంకం save_state = State;

	    curwin->w_cursor = save_cursor;
	    State = బహుళకం_చొప్పించు;
	    check_cursor_నిలువు();
	    State = save_state;
	}
    }

#నిర్వచితమైతే FEAT_CONCEAL
    // Check if the cursor line was concealed before changing State.
    cursor_పంక్తి_was_concealed = curwin->w_p_cole > 0
						&& conceal_cursor_పంక్తి(curwin);
#అయితే_ముగింపు

    /*
     * When doing a paste with the middle mouse button, Insstart is set to
     * where the paste started.
     */
    ఒకవేళ (where_paste_started.lnum != 0)
	Insstart = where_paste_started;
    లేకపోతే
    {
	Insstart = curwin->w_cursor;
	ఒకవేళ (startln)
	    Insstart.నిలువు = 0;
    }
    Insstart_textlen = (colnr_T)linetabsize_str(ml_get_curline());
    Insstart_blank_vcol = MAXCOL;
    ఒకవేళ (!did_ai)
	ai_నిలువు = 0;

    ఒకవేళ (cmdchar != NUL && restart_edit == 0)
    {
	ResetRedobuff();
	Append_సంఖ్య_గమ్యం_Redobuff(లెక్క);
	ఒకవేళ (cmdchar == 'V' || cmdchar == 'v')
	{
	    // "gR" or "gr" command
	    Append_Char_గమ్యం_Redobuff('g');
	    Append_Char_గమ్యం_Redobuff((cmdchar == 'v') ? 'r' : 'R');
	}
	లేకపోతే
	{
	    ఒకవేళ (cmdchar == K_PS)
		Append_Char_గమ్యం_Redobuff('a');
	    లేకపోతే
		Append_Char_గమ్యం_Redobuff(cmdchar);
	    ఒకవేళ (cmdchar == 'g')		    // "gI" command
		Append_Char_గమ్యం_Redobuff('I');
	    లేకపోతే ఒకవేళ (cmdchar == 'r')	    // "r<CR>" command
		లెక్క = 1;		    // insert only one <CR>
	}
    }

    ఒకవేళ (cmdchar == 'R')
    {
	State = బహుళకం_REPLACE;
    }
    లేకపోతే ఒకవేళ (cmdchar == 'V' || cmdchar == 'v')
    {
	State = బహుళకం_VREPLACE;
	replaceState = బహుళకం_VREPLACE;
	orig_పంక్తి_లెక్క = curbuf->b_ml.ml_పంక్తి_లెక్క;
	vr_పంక్తులు_మారింది = 1;
    }
    లేకపోతే
	State = బహుళకం_చొప్పించు;

    may_trigger_modechanged();
    stop_చొప్పించు_బహుళకం = FALSE;

#నిర్వచితమైతే FEAT_CONCEAL
    // Check if the cursor line needs redrawing after changing State.  If
    // 'concealcursor' is "n" it needs to be redrawn without concealing.
    conceal_check_cursor_పంక్తి(cursor_పంక్తి_was_concealed);
#అయితే_ముగింపు

    // Need to position cursor again when on a TAB and when on a char with
    // virtual text.
    ఒకవేళ (gchar_cursor() == TAB
#నిర్వచితమైతే FEAT_PROP_POPUP
	    || curbuf->b_has_textprop
#అయితే_ముగింపు
       )
	curwin->w_valid &= ~(VALID_WROW|VALID_WCOL|VALID_VIRTCOL);

    /*
     * Enable langmap or IME, indicated by 'iminsert'.
     * Note that IME may enabled/disabled without us noticing here, thus the
     * 'iminsert' value may not reflect what is actually used.  It is updated
     * when hitting <Esc>.
     */
    ఒకవేళ (curbuf->b_p_iminsert == B_IMODE_LMAP)
	State |= బహుళకం_LANGMAP;
#నిర్వచితమైతే HAVE_ఆదానం_పద్ధతి
    im_సమితి_చురుకైన(curbuf->b_p_iminsert == B_IMODE_IM);
#అయితే_ముగింపు

    setmouse();
    clear_showcmd();
#నిర్వచితమైతే FEAT_RIGHTLEFT
    // there is no reverse replace mode
    revins_on = (State == బహుళకం_చొప్పించు && p_ri);
    ఒకవేళ (revins_on)
	undisplay_dollar();
    revins_chars = 0;
    revins_legal = 0;
    revins_scol = -1;
#అయితే_ముగింపు
    ఒకవేళ (!p_ek)
    {
	MAY_WANT_గమ్యం_LOG_THIS;

	// Disable bracketed paste mode, we won't recognize the escape
	// sequences.
	బహిర్గతం_str(T_BD);

	// Disable modifyOtherKeys, keys with modifiers would cause exiting
	// Insert mode.
	బహిర్గతం_str_t_TE();
    }

    /*
     * Handle restarting Insert mode.
     * Don't do this for "CTRL-O ." (repeat an insert): In that case we get
     * here with something in the stuff buffer.
     */
    ఒకవేళ (restart_edit != 0 && stuff_ఖాళీలు())
    {
	/*
	 * After a paste we consider text typed to be part of the insert for
	 * the pasted text. You can backspace over the pasted text too.
	 */
	ఒకవేళ (where_paste_started.lnum)
	    arrow_వాడిన = FALSE;
	లేకపోతే
	    arrow_వాడిన = TRUE;
	restart_edit = 0;

	/*
	 * If the cursor was after the end-of-line before the CTRL-O and it is
	 * now at the end-of-line, put it after the end-of-line (this is not
	 * correct in very rare cases).
	 * Also do this if curswant is greater than the current virtual
	 * column.  Eg after "^O$" or "^O80|".
	 */
	validate_virtcol();
	update_curswant();
	ఒకవేళ (((ins_at_eol && curwin->w_cursor.lnum == o_lnum)
		    || curwin->w_curswant > curwin->w_virtcol)
		&& *(ptr = ml_get_curline() + curwin->w_cursor.నిలువు) != NUL)
	{
	    ఒకవేళ (ptr[1] == NUL)
		++curwin->w_cursor.నిలువు;
	    లేకపోతే ఒకవేళ (has_mbyte)
	    {
		i = (*mb_ptr2len)(ptr);
		ఒకవేళ (ptr[i] == NUL)
		    curwin->w_cursor.నిలువు += i;
	    }
	}
	ins_at_eol = FALSE;
    }
    లేకపోతే
	arrow_వాడిన = FALSE;

    // we are in insert mode now, don't need to start it anymore
    need_ప్రారంభం_insertmode = FALSE;

    // Need to save the line for undo before inserting the first char.
    ins_need_undo = TRUE;

    where_paste_started.lnum = 0;
    can_cindent = TRUE;
#నిర్వచితమైతే FEAT_FOLDING
    // The cursor line is not in a closed fold, unless 'insertmode' is set or
    // restarting.
    ఒకవేళ (!p_im && did_restart_edit == 0)
	foldOpenCursor();
#అయితే_ముగింపు

    /*
     * If 'showmode' is set, show the current (insert/replace/..) mode.
     * A warning message for changing a readonly file is given here, before
     * actually changing anything.  It's put after the mode, if any.
     */
    i = 0;
    ఒకవేళ (p_smd && సందేశం_silent == 0)
	i = showmode();

    ఒకవేళ (!p_im && did_restart_edit == 0)
	change_warning(i == 0 ? 0 : i + 1);

#నిర్వచితమైతే CURSOR_SHAPE
    ui_cursor_shape();		// may show different cursor shape
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_DIGRAPHS
    do_digraph(-1);		// clear digraphs
#అయితే_ముగింపు

    /*
     * Get the current length of the redo buffer, those characters have to be
     * skipped if we want to get to the inserted characters.
     */
    string_T inserted = get_inserted();
    new_చొప్పించు_skip = (పూర్ణాంకం)inserted.నిడివి;
    ఒకవేళ (inserted.string != ఖాళీ)
	vim_free(inserted.string);

    old_indent = 0;

    /*
     * Main loop in Insert mode: repeat until Insert mode is left.
     */
    కొరకు (;;)
    {
#నిర్వచితమైతే FEAT_RIGHTLEFT
	ఒకవేళ (!revins_legal)
	    revins_scol = -1;	    // reset on illegal motions
	లేకపోతే
	    revins_legal = 0;
#అయితే_ముగింపు
	ఒకవేళ (arrow_వాడిన)	    // don't repeat insert when arrow key used
	    లెక్క = 0;

	ఒకవేళ (update_Insstart_orig)
	    Insstart_orig = Insstart;

	ఒకవేళ (stop_చొప్పించు_బహుళకం && !ins_compl_చురుకైన())
	{
	    // ":stopinsert" used or 'insertmode' reset
	    లెక్క = 0;
	    వెళ్ళు doESCkey;
	}

	// set curwin->w_curswant for next K_DOWN or K_UP
	ఒకవేళ (!arrow_వాడిన)
	    curwin->w_సమితి_curswant = true;

	// If there is no typeahead may check for timestamps (e.g., for when a
	// menu invoked a shell command).
	ఒకవేళ (stuff_ఖాళీలు())
	{
	    did_check_timestamps = FALSE;
	    ఒకవేళ (need_check_timestamps)
		check_timestamps(FALSE);
	}

	/*
	 * When emsg() was called msg_scroll will have been set.
	 */
	సందేశం_scroll = FALSE;

#నిర్వచితమైతే FEAT_GUI
	// When 'mousefocus' is set a mouse movement may have taken us to
	// another window.  "need_mouse_correct" may then be set because of an
	// autocommand.
	ఒకవేళ (need_mouse_correct)
	    gui_mouse_correct();
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_FOLDING
	// Open fold at the cursor line, according to 'foldopen'.
	ఒకవేళ (fdo_flags & FDO_చొప్పించు)
	    foldOpenCursor();
	// Close folds where the cursor isn't, according to 'foldclose'
	ఒకవేళ (!char_avail())
	    foldCheckClose();
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_పని_CHANNEL
	ఒకవేళ (bt_prompt(curbuf))
	{
	    init_prompt(cmdchar_todo);
	    cmdchar_todo = NUL;
	}
#అయితే_ముగింపు

	/*
	 * If we inserted a character at the last position of the last line in
	 * the window, scroll the window one line up. This avoids an extra
	 * redraw.
	 * This is detected when the cursor column is smaller after inserting
	 * something.
	 * Don't do this when the topline changed already, it has
	 * already been adjusted (by insertchar() calling open_line())).
	 * Also don't do this when 'smoothscroll' is set, as the window should
	 * then be scrolled by screen lines.
	 */
	ఒకవేళ (curbuf->b_mod_సమితి
		&& curwin->w_p_wrap
		&& !curwin->w_p_sms
		&& !did_backspace
		&& curwin->w_topline == old_topline
#నిర్వచితమైతే FEAT_DIFF
		&& curwin->w_topfill == old_topfill
#అయితే_ముగింపు
		&& లెక్క <= 1
		)
	{
	    mincol = curwin->w_wcol;
	    validate_cursor_నిలువు();

	    ఒకవేళ (
#నిర్వచితమైతే FEAT_VARTABS
		curwin->w_wcol < mincol - tabstop_at(get_nolist_virtcol(),
						     curbuf->b_p_ts,
						     curbuf->b_p_vts_array,
						     FALSE)
#లేకపోతే
		(పూర్ణాంకం)curwin->w_wcol < mincol - curbuf->b_p_ts
#అయితే_ముగింపు
		    && curwin->w_wrow ==
				   curwin->w_ఎత్తు - 1 - get_scrolloff_విలువ()
		    && (curwin->w_cursor.lnum != curwin->w_topline
#నిర్వచితమైతే FEAT_DIFF
			|| curwin->w_topfill > 0
#అయితే_ముగింపు
		    ))
	    {
#నిర్వచితమైతే FEAT_DIFF
		ఒకవేళ (curwin->w_topfill > 0)
		    --curwin->w_topfill;
		లేకపోతే
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_FOLDING
		ఒకవేళ (hasFolding(curwin->w_topline, ఖాళీ, &old_topline))
		    సమితి_topline(curwin, old_topline + 1);
		లేకపోతే
#అయితే_ముగింపు
		    సమితి_topline(curwin, curwin->w_topline + 1);
	    }
	}

	// May need to adjust w_topline to show the cursor.
	ఒకవేళ (లెక్క <= 1)
	    update_topline();

	did_backspace = FALSE;

	ఒకవేళ (లెక్క <= 1)
	    validate_cursor();		// may set must_redraw

	/*
	 * Redraw the display when no characters are waiting.
	 * Also shows mode, ruler and positions cursor.
	 */
	ins_redraw(TRUE);

	ఒకవేళ (curwin->w_p_scb)
	    do_check_scrollbind(TRUE);

	ఒకవేళ (curwin->w_p_crb)
	    do_check_cursorbind();
	ఒకవేళ (లెక్క <= 1)
	    update_curswant();
	old_topline = curwin->w_topline;
#నిర్వచితమైతే FEAT_DIFF
	old_topfill = curwin->w_topfill;
#అయితే_ముగింపు

#నిర్వచితమైతే USE_ON_FLY_SCROLL
	dont_scroll = FALSE;		// allow scrolling here
#అయితే_ముగింపు
	// May request the keyboard protocol state now.
	may_send_t_RK();

	/*
	 * Get a character for Insert mode.  Ignore K_IGNORE and K_NOP.
	 */
	ఒకవేళ (c != K_CURSORHOLD && c != K_COMPLETE_DELAY)
	    lastc = c;		// remember the previous char for CTRL-D

	// After using CTRL-G U the next cursor key will not break undo.
	ఒకవేళ (dont_sync_undo == MAYBE)
	    dont_sync_undo = TRUE;
	లేకపోతే
	    dont_sync_undo = FALSE;
	ఒకవేళ (cmdchar == K_PS)
	    // Got here from normal mode when bracketed paste started.
	    c = K_PS;
	లేకపోతే
	{
	    // Trigger autocomplete when entering Insert mode, either directly
	    // or via change commands like 'ciw', 'cw', etc., before the first
	    // character is typed.
	    ఒకవేళ (ins_just_started)
	    {
		ins_just_started = FALSE;
		ఒకవేళ (ins_compl_has_autocomplete() && !char_avail()
			&& curwin->w_cursor.నిలువు > 0)
		{
		    c = char_before_cursor();
		    ఒకవేళ (vim_isprintc(c))
		    {
			ins_compl_enable_autocomplete();
			ins_compl_arm_autostart();
			ins_compl_init_get_longest();
#నిర్వచితమైతే FEAT_RIGHTLEFT
			ఒకవేళ (p_hkmap)
			    c = hkmap(c);		// Hebrew mode mapping
#అయితే_ముగింపు
			// Defer until the delay expires (K_COMPLETE_DELAY), or
			// trigger now when no delay is in effect.
			ఒకవేళ (!ins_compl_arm_autocomplete_delay())
			    వెళ్ళు docomplete;
		    }
		}
	    }

	    చేయి
	    {
		c = సురక్షిత_vgetc();

		ఒకవేళ (stop_చొప్పించు_బహుళకం
#నిర్వచితమైతే FEAT_TERMINAL
			|| (c == K_IGNORE && పదం_use_loop())
#అయితే_ముగింపు
		   )
		{
		    // Insert mode ended, possibly from a callback, or a timer
		    // must have opened a terminal window.
		    ఒకవేళ (c != K_IGNORE && c != K_NOP)
			vungetc(c);
		    లెక్క = 0;

		    ఒకవేళ (!bt_prompt(curwin->w_బఫరు)
#నిర్వచితమైతే FEAT_TERMINAL
			    && !bt_terminal(curwin->w_బఫరు)
#అయితే_ముగింపు
			    && stop_చొప్పించు_బహుళకం)
			// :stopinsert command via callback or via server command
			nomove = FALSE;
		    లేకపోతే
			nomove = TRUE;
		    ins_compl_prep(ESC);
		    వెళ్ళు doESCkey;
		}
	    } అయితే (c == K_IGNORE || c == K_NOP);
	}

	// Don't want K_CURSORHOLD for the second key, e.g., after CTRL-V.
	did_cursorhold = TRUE;
	ఒకవేళ (c != K_CURSORHOLD && c != K_COMPLETE_DELAY)
	{
	    // Don't want delayed autocompletion from the previous key either.
	    ins_compl_clear_autocomplete_delay();
	    ins_compl_disarm_autostart();
	    // A completion already on screen goes on being what it was.
	    ఒకవేళ (!ins_compl_చురుకైన())
		ins_compl_disable_autocomplete();
	}

#నిర్వచితమైతే FEAT_RIGHTLEFT
	ఒకవేళ (p_hkmap && కీలకం_Typed)
	    c = hkmap(c);		// Hebrew mode mapping
#అయితే_ముగింపు

	// If the window was made so small that nothing shows, make it at least
	// one line and one column when typing.
	ఒకవేళ (కీలకం_Typed && !కీలకం_Stuffed)
	    win_ensure_కొలత();

	/*
	 * Special handling of keys while the popup menu is visible or wanted
	 * and the cursor is still in the completed word.  Only when there is
	 * a match, skip this when no matches were found.
	 */
	ఒకవేళ (ins_compl_చురుకైన() && curwin->w_cursor.నిలువు >= ins_compl_నిలువు()
		&& ins_compl_has_shown_సరిపోలిక() && pum_wanted())
	{
	    // BS: Delete one character from "compl_leader".
	    ఒకవేళ ((c == K_BS || c == Ctrl_H)
			&& curwin->w_cursor.నిలువు > ins_compl_నిలువు()
			&& (c = ins_compl_bs()) == NUL)
		కొనసాగు;

	    // When no match was selected or it was edited.
	    ఒకవేళ (!ins_compl_వాడిన_సరిపోలిక())
	    {
		// CTRL-L: Add one character from the current match to
		// "compl_leader".  Except when at the original match and
		// there is nothing to add, CTRL-L works like CTRL-P then.
		ఒకవేళ (c == Ctrl_L
			&& (!ctrl_x_బహుళకం_పంక్తి_or_eval()
			    || ins_compl_long_shown_సరిపోలిక()))
		{
		    ins_compl_addfrommatch();
		    కొనసాగు;
		}

		// A non-white character that fits in with the current
		// completion: Add to "compl_leader".
		ఒకవేళ (ins_compl_accept_char(c))
		{
#ఒకవేళ defined(FEAT_EVAL)
		    // Trigger InsertCharPre.
		    char_u *str = do_చొప్పించు_char_pre(c);
		    char_u *p;

		    ఒకవేళ (str != ఖాళీ)
		    {
			కొరకు (p = str; *p != NUL; MB_PTR_ADV(p))
			    ins_compl_addleader(PTR2CHAR(p));
			vim_free(str);
		    }
		    లేకపోతే
#అయితే_ముగింపు
			ins_compl_addleader(c);
		    కొనసాగు;
		}

		// Pressing CTRL-Y selects the current match.  When
		// ins_compl_enter_selects() is set the Enter key does the
		// same.
		ఒకవేళ ((c == Ctrl_Y || (ins_compl_enter_selects()
				    && (c == CAR || c == K_KENTER || c == NL)))
			&& stop_arrow() == OK)
		{
		    ins_compl_delete();
		    ఒకవేళ (ins_compl_preinsert_longest()
			    && !ins_compl_is_సరిపోలిక_selected())
		    {
			ins_compl_చొప్పించు(FALSE, TRUE);
			ins_compl_init_get_longest();
			కొనసాగు;
		    }
		    లేకపోతే
			ins_compl_చొప్పించు(FALSE, FALSE);
		}
		// Delete preinserted text when typing special chars
		లేకపోతే ఒకవేళ (IS_WHITE_NL_OR_NUL(c) && ins_compl_preinsert_effect())
		    ins_compl_delete();
	    }
	}

	// Prepare for or stop CTRL-X mode.  This doesn't do completion, but
	// it does fix up the text when finishing completion.
	ins_compl_init_get_longest();
	ఒకవేళ (ins_compl_prep(c))
	    కొనసాగు;

	// CTRL-\ CTRL-N goes to Normal mode,
	// CTRL-\ CTRL-G goes to mode selected with 'insertmode',
	// CTRL-\ CTRL-O is like CTRL-O but without moving the cursor.
	ఒకవేళ (c == Ctrl_BSL)
	{
	    // may need to redraw when no more chars available now
	    ins_redraw(FALSE);
	    ++no_mapping;
	    ++allow_కీలకాలు;
	    c = plain_vgetc();
	    --no_mapping;
	    --allow_కీలకాలు;
	    ఒకవేళ (c != Ctrl_N && c != Ctrl_G && c != Ctrl_O)
	    {
		// it's something else
		vungetc(c);
		c = Ctrl_BSL;
	    }
	    లేకపోతే ఒకవేళ (c == Ctrl_G && p_im)
		కొనసాగు;
	    లేకపోతే
	    {
		ఒకవేళ (c == Ctrl_O)
		{
		    ins_ctrl_o();
		    ins_at_eol = FALSE;	// cursor keeps its column
		    nomove = TRUE;
		}
		లెక్క = 0;
		వెళ్ళు doESCkey;
	    }
	}

#నిర్వచితమైతే FEAT_DIGRAPHS
	c = do_digraph(c);
#అయితే_ముగింపు

	ఒకవేళ ((c == Ctrl_V || c == Ctrl_Q) && ctrl_x_బహుళకం_cmdline())
	    వెళ్ళు docomplete;
	ఒకవేళ (c == Ctrl_V || c == Ctrl_Q)
	{
	    ins_ctrl_v();
	    c = Ctrl_V;	// pretend CTRL-V is last typed character
	    కొనసాగు;
	}

	ఒకవేళ (cindent_on() && ctrl_x_బహుళకం_none())
	{
	    // A key name preceded by a bang means this key is not to be
	    // inserted.  Skip ahead to the re-indenting below.
	    // A key name preceded by a star means that indenting has to be
	    // done before inserting the key.
	    పంక్తి_is_white = inindent(0);
	    ఒకవేళ (లోపల_cinkeys(c, '!', పంక్తి_is_white))
		వెళ్ళు force_cindent;
	    ఒకవేళ (can_cindent && లోపల_cinkeys(c, '*', పంక్తి_is_white)
							&& stop_arrow() == OK)
		do_c_సమాసం_indent();
	}

#నిర్వచితమైతే FEAT_RIGHTLEFT
	ఒకవేళ (curwin->w_p_rl)
	    ఎంపిక (c)
	    {
		స్థితి K_ఎడమ:	c = K_కుడి; ఆపు;
		స్థితి K_S_ఎడమ:	c = K_S_కుడి; ఆపు;
		స్థితి K_C_ఎడమ:	c = K_C_కుడి; ఆపు;
		స్థితి K_కుడి:	c = K_ఎడమ; ఆపు;
		స్థితి K_S_కుడి: c = K_S_ఎడమ; ఆపు;
		స్థితి K_C_కుడి: c = K_C_ఎడమ; ఆపు;
	    }
#అయితే_ముగింపు

	/*
	 * If 'keymodel' contains "startsel", may start selection.  If it
	 * does, a CTRL-O and c will be stuffed, we need to get these
	 * characters.
	 */
	ఒకవేళ (ins_ప్రారంభం_select(c))
	    కొనసాగు;

	/*
	 * The big switch to handle a character in insert mode.
	 */
	ఎంపిక (c)
	{
	స్థితి ESC:	// End input mode
	    ఒకవేళ (echeck_abbr(ESC + ABBR_OFF))
		ఆపు;
	    // FALLTHROUGH

	స్థితి Ctrl_C:	// End input mode
	    ఒకవేళ (c == Ctrl_C && cmdwin_type != 0)
	    {
		// Close the cmdline window.
		cmdwin_ఫలితం = K_IGNORE;
		got_int = FALSE; // don't stop executing autocommands et al.
		nomove = TRUE;
		వెళ్ళు doESCkey;
	    }
#నిర్వచితమైతే FEAT_పని_CHANNEL
	    ఒకవేళ (c == Ctrl_C && bt_prompt(curbuf))
	    {
		ఒకవేళ (invoke_prompt_అంతరాయం())
		{
		    ఒకవేళ (!bt_prompt(curbuf))
			// buffer changed to a non-prompt buffer, get out of
			// Insert mode
			వెళ్ళు doESCkey;
		    ఆపు;
		}
	    }
#అయితే_ముగింపు

#నిర్వచితమైతే UNIX
do_intr:
#అయితే_ముగింపు
	    // when 'insertmode' set, and not halfway a mapping, don't leave
	    // Insert mode
	    ఒకవేళ (goto_im())
	    {
		ఒకవేళ (got_int)
		{
		    (శూన్య)vgetc();		// flush all buffers
		    got_int = FALSE;
		}
		లేకపోతే
		    vim_beep(BO_IM);
		ఆపు;
	    }
doESCkey:
	    // Drop a pending autocomplete so it does not outlive Insert mode.
	    ins_compl_clear_autocomplete_delay();
	    /*
	     * This is the ONLY return from edit()!
	     */
	    // Always update o_lnum, so that a "CTRL-O ." that adds a line
	    // still puts the cursor back after the inserted text.
	    ఒకవేళ (ins_at_eol && gchar_cursor() == NUL)
		o_lnum = curwin->w_cursor.lnum;

	    ఒకవేళ (ins_esc(&లెక్క, cmdchar, nomove))
	    {
		// When CTRL-C was typed got_int will be set, with the result
		// that the autocommands won't be executed. When mapped got_int
		// is not set, but let's keep the behavior the same.
		ఒకవేళ (cmdchar != 'r' && cmdchar != 'v' && c != Ctrl_C)
		    ins_apply_autocmds(EVENT_INSERTLEAVE);
		did_cursorhold = FALSE;

		// ins_redraw() triggers TextChangedI only when no characters
		// are in the typeahead buffer, so reset curbuf->b_last_changedtick only
		// if the TextChangedI was not blocked by char_avail() (e.g. using :norm!)
		// and the TextChangedI autocommand has been triggered.
		ఒకవేళ (!char_avail() && curbuf->b_చివరి_changedtick_i == CHANGEDTICK(curbuf))
		    curbuf->b_చివరి_changedtick = CHANGEDTICK(curbuf);
		తిరిగి (c == Ctrl_O);
	    }
	    కొనసాగు;

	స్థితి Ctrl_Z:	// suspend when 'insertmode' set
	    ఒకవేళ (!p_im)
		వెళ్ళు normalchar;	// insert CTRL-Z as normal char
	    do_cmdline_cmd((char_u *)"stop");
#నిర్వచితమైతే CURSOR_SHAPE
	    ui_cursor_shape();		// may need to update cursor shape
#అయితే_ముగింపు
	    కొనసాగు;

	స్థితి Ctrl_O:	// execute one command
#నిర్వచితమైతే FEAT_COMPL_FUNC
	    ఒకవేళ (ctrl_x_బహుళకం_omni())
		వెళ్ళు docomplete;
#అయితే_ముగింపు
	    ఒకవేళ (echeck_abbr(Ctrl_O + ABBR_OFF))
		ఆపు;
	    ins_ctrl_o();

	    // don't move the cursor left when 'virtualedit' has "onemore".
	    ఒకవేళ (get_ve_flags() & VE_ONEMORE)
	    {
		ins_at_eol = FALSE;
		nomove = TRUE;
	    }
	    లెక్క = 0;
	    వెళ్ళు doESCkey;

	స్థితి K_INS:	// toggle insert/replace mode
	స్థితి K_KINS:
	    ins_చొప్పించు(replaceState);
	    ఆపు;

	స్థితి K_SELECT:	// end of Select mode mapping - ignore
	    ఆపు;

	స్థితి K_HELP:	// Help key works like <ESC> <Help>
	స్థితి K_F1:
	స్థితి K_XF1:
	    stuffcharReadbuff(K_HELP);
	    ఒకవేళ (p_im)
		need_ప్రారంభం_insertmode = TRUE;
	    వెళ్ళు doESCkey;

#నిర్వచితమైతే FEAT_NETBEANS_INTG
	స్థితి K_F21:	// NetBeans command
	    ++no_mapping;		// don't map the next key hits
	    i = plain_vgetc();
	    --no_mapping;
	    netbeans_keycommand(i);
	    ఆపు;
#అయితే_ముగింపు

	స్థితి K_ZERO:	// Insert the previously inserted text.
	స్థితి NUL:
	స్థితి Ctrl_A:
	    // For ^@ the trailing ESC will end the insert, unless there is an
	    // error.
	    ఒకవేళ (stuff_inserted(NUL, 1L, (c == Ctrl_A)) == FAIL
						   && c != Ctrl_A && !p_im)
		వెళ్ళు doESCkey;		// quit insert mode
	    inserted_space = FALSE;
	    ఆపు;

	స్థితి Ctrl_R:	// insert the contents of a register
	    ఒకవేళ (ctrl_x_బహుళకం_రిజిస్టరు() && !ins_compl_చురుకైన())
		వెళ్ళు docomplete;
	    ins_reg();
	    auto_format(FALSE, TRUE);
	    inserted_space = FALSE;
	    ఆపు;

	స్థితి Ctrl_G:	// commands starting with CTRL-G
	    ins_ctrl_g();
	    ఆపు;

	స్థితి Ctrl_HAT:	// switch input mode and/or langmap
	    ins_ctrl_hat();
	    ఆపు;

#నిర్వచితమైతే FEAT_RIGHTLEFT
	స్థితి Ctrl__:	// switch between languages
	    ఒకవేళ (!p_ari)
		వెళ్ళు normalchar;
	    ins_ctrl_();
	    ఆపు;
#అయితే_ముగింపు

	స్థితి Ctrl_D:	// Make indent one shiftwidth smaller.
#ఒకవేళ defined(FEAT_కనుగొను_ID)
	    ఒకవేళ (ctrl_x_బహుళకం_మార్గం_నిర్వచిస్తుంది())
		వెళ్ళు docomplete;
#అయితే_ముగింపు
	    // FALLTHROUGH

	స్థితి Ctrl_T:	// Make indent one shiftwidth greater.
	    ఒకవేళ (c == Ctrl_T && ctrl_x_బహుళకం_thesaurus())
	    {
		ఒకవేళ (has_compl_option(FALSE))
		    వెళ్ళు docomplete;
		ఆపు;
	    }

	    ins_జరుపు(c, lastc);
	    auto_format(FALSE, TRUE);
	    inserted_space = FALSE;
	    ఆపు;

	స్థితి K_DEL:	// delete character under the cursor
	స్థితి K_KDEL:
	    ins_del();
	    auto_format(FALSE, TRUE);
	    ఆపు;

	స్థితి K_BS:	// delete character before the cursor
	స్థితి K_S_BS:
	స్థితి Ctrl_H:
	    did_backspace = ins_bs(c, BACKSPACE_CHAR, &inserted_space);
	    auto_format(FALSE, TRUE);
	    ఒకవేళ (did_backspace)
		MAY_TRIGGER_AUTOCOMPLETE(c);
	    ఆపు;

	స్థితి Ctrl_W:	// delete word before the cursor
#నిర్వచితమైతే FEAT_పని_CHANNEL
	    ఒకవేళ (bt_prompt(curbuf) && (mod_ముసుగు & MOD_ముసుగు_జరుపు) == 0)
	    {
		// In a prompt window CTRL-W is used for window commands.
		// Use Shift-CTRL-W to delete a word.
		stuffcharReadbuff(Ctrl_W);
		restart_edit = 'A';
		nomove = TRUE;
		లెక్క = 0;
		వెళ్ళు doESCkey;
	    }
#అయితే_ముగింపు
	    did_backspace = ins_bs(c, BACKSPACE_మాట, &inserted_space);
	    auto_format(FALSE, TRUE);
	    ఒకవేళ (did_backspace)
		MAY_TRIGGER_AUTOCOMPLETE(c);
	    ఆపు;

	స్థితి Ctrl_U:	// delete all inserted text in current line
#నిర్వచితమైతే FEAT_COMPL_FUNC
	    // CTRL-X CTRL-U completes with 'completefunc'.
	    ఒకవేళ (ctrl_x_బహుళకం_function())
		వెళ్ళు docomplete;
#అయితే_ముగింపు
	    did_backspace = ins_bs(c, BACKSPACE_పంక్తి, &inserted_space);
	    auto_format(FALSE, TRUE);
	    inserted_space = FALSE;
	    ఒకవేళ (did_backspace)
		MAY_TRIGGER_AUTOCOMPLETE(c);
	    ఆపు;

	స్థితి K_LEFTMOUSE:   // mouse keys
	స్థితి K_LEFTMOUSE_NM:
	స్థితి K_LEFTDRAG:
	స్థితి K_LEFTRELEASE:
	స్థితి K_LEFTRELEASE_NM:
	స్థితి K_MOUSEMOVE:
	స్థితి K_MIDDLEMOUSE:
	స్థితి K_MIDDLEDRAG:
	స్థితి K_MIDDLERELEASE:
	స్థితి K_RIGHTMOUSE:
	స్థితి K_RIGHTDRAG:
	స్థితి K_RIGHTRELEASE:
	స్థితి K_X1MOUSE:
	స్థితి K_X1DRAG:
	స్థితి K_X1RELEASE:
	స్థితి K_X2MOUSE:
	స్థితి K_X2DRAG:
	స్థితి K_X2RELEASE:
	    ins_mouse(c);
	    ఆపు;

	స్థితి K_MOUSEDOWN: // Default action for scroll wheel up: scroll up
	    ins_mousescroll(MSCR_కిందికి);
	    ఆపు;

	స్థితి K_MOUSEUP:	// Default action for scroll wheel down: scroll down
	    ins_mousescroll(MSCR_పైకి);
	    ఆపు;

	స్థితి K_MOUSELEFT: // Scroll wheel left
	    ins_mousescroll(MSCR_ఎడమ);
	    ఆపు;

	స్థితి K_MOUSERIGHT: // Scroll wheel right
	    ins_mousescroll(MSCR_కుడి);
	    ఆపు;

	స్థితి K_PS:
	    bracketed_paste(PASTE_చొప్పించు, FALSE, ఖాళీ);
	    ఒకవేళ (cmdchar == K_PS)
		// invoked from normal mode, bail out
		వెళ్ళు doESCkey;
	    ఆపు;
	స్థితి K_PE:
	    // Got K_PE without K_PS, ignore.
	    ఆపు;

#నిర్వచితమైతే FEAT_GUI_TABLINE
	స్థితి K_TABLINE:
	స్థితి K_TABMENU:
	    ins_tabline(c);
	    ఆపు;
#అయితే_ముగింపు

	స్థితి K_IGNORE:	// Something mapped to nothing
	    ఆపు;

	స్థితి K_COMMAND:		    // <Cmd>command<CR>
	స్థితి K_SCRIPT_COMMAND:	    // <ScriptCmd>command<CR>
	    {
		bufref_T    save_curbuf;
		varnumber_T tick = CHANGEDTICK(curbuf);

		సమితి_bufref(&save_curbuf, curbuf);
		do_cmdkey_command(c, 0);

#నిర్వచితమైతే FEAT_TERMINAL
		ఒకవేళ (పదం_use_loop())
		    // Started a terminal that gets the input, exit Insert mode.
		    వెళ్ళు doESCkey;
#అయితే_ముగింపు
		ఒకవేళ (curbuf->b_u_synced
			|| (bufref_valid(&save_curbuf)
			    && curbuf == save_curbuf.br_buf
			    && tick != CHANGEDTICK(curbuf)))
		{
		    // The command synced undo or changed this buffer.
		    // Save the cursor line before the next typed edit.
		    ins_need_undo = TRUE;
		}
	    }
	    ఆపు;

	స్థితి K_CURSORHOLD:	// Didn't type something for a while.
	    ins_apply_autocmds(EVENT_CURSORHOLDI);
	    did_cursorhold = TRUE;
	    // If CTRL-G U was used apply it to the next typed key.
	    ఒకవేళ (dont_sync_undo == TRUE)
		dont_sync_undo = MAYBE;
	    ఆపు;

	స్థితి K_COMPLETE_DELAY:	// 'autocompletedelay' expired
	    // If CTRL-G U was used apply it to the next typed key.
	    ఒకవేళ (dont_sync_undo == TRUE)
		dont_sync_undo = MAYBE;
	    ins_compl_clear_autocomplete_delay();
	    ఒకవేళ (!ins_compl_has_autocomplete() || char_avail()
		    || curwin->w_cursor.నిలువు == 0)
		ఆపు;
	    c = char_before_cursor();
	    ఒకవేళ (!vim_isprintc(c))
		ఆపు;
	    // The completion may have been cleared while waiting, so re-enable
	    // autocomplete to match a zero delay.
	    ins_compl_enable_autocomplete();
	    ins_compl_arm_autostart();
	    వెళ్ళు docomplete;

#నిర్వచితమైతే FEAT_GUI_MSWIN
	    // On MS-Windows ignore <M-F4>, we get it when closing the window
	    // was cancelled.
	స్థితి K_F4:
	    ఒకవేళ (mod_ముసుగు != MOD_ముసుగు_ALT)
		వెళ్ళు normalchar;
	    ఆపు;
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_GUI
	స్థితి K_VER_SCROLLBAR:
	    ins_scroll();
	    ఆపు;

	స్థితి K_HOR_SCROLLBAR:
	    ins_horscroll();
	    ఆపు;
#అయితే_ముగింపు

	స్థితి K_HOME:	// <Home>
	స్థితి K_KHOME:
	స్థితి K_S_HOME:
	స్థితి K_C_HOME:
	    ins_home(c);
	    ఆపు;

	స్థితి K_చివర:	// <End>
	స్థితి K_KEND:
	స్థితి K_S_చివర:
	స్థితి K_C_చివర:
	    ins_చివర(c);
	    ఆపు;

	స్థితి K_ఎడమ:	// <Left>
	    ఒకవేళ (mod_ముసుగు & (MOD_ముసుగు_జరుపు|MOD_ముసుగు_CTRL))
		ins_s_ఎడమ();
	    లేకపోతే
		ins_ఎడమ();
	    ఆపు;

	స్థితి K_S_ఎడమ:	// <S-Left>
	స్థితి K_C_ఎడమ:
	    ins_s_ఎడమ();
	    ఆపు;

	స్థితి K_కుడి:	// <Right>
	    ఒకవేళ (mod_ముసుగు & (MOD_ముసుగు_జరుపు|MOD_ముసుగు_CTRL))
		ins_s_కుడి();
	    లేకపోతే
		ins_కుడి();
	    ఆపు;

	స్థితి K_S_కుడి:	// <S-Right>
	స్థితి K_C_కుడి:
	    ins_s_కుడి();
	    ఆపు;

	స్థితి K_పైకి:	// <Up>
	    ఒకవేళ (pum_visible())
		వెళ్ళు docomplete;
	    ఒకవేళ (mod_ముసుగు & MOD_ముసుగు_జరుపు)
		ins_pageup();
	    లేకపోతే
		ins_పైకి(FALSE);
	    ఆపు;

	స్థితి K_S_పైకి:	// <S-Up>
	స్థితి K_PAGEUP:
	స్థితి K_KPAGEUP:
	    ఒకవేళ (pum_visible())
	    {
#నిర్వచితమైతే FEAT_PROP_POPUP
		// CTRL-SHIFT-<Up> scrolls the info popup up a line,
		// CTRL-SHIFT-<PageUp> a page.  Shift is folded into K_S_UP but
		// stays in mod_mask for PageUp, hence the asymmetric check.
		ఒకవేళ (c == K_S_పైకి ? (mod_ముసుగు & MOD_ముసుగు_CTRL)
			: ((mod_ముసుగు & MOD_ముసుగు_CTRL)
			    && (mod_ముసుగు & MOD_ముసుగు_జరుపు)))
		{
		    popup_scroll_info(-1, c != K_S_పైకి);
		    ఆపు;
		}
#అయితే_ముగింపు
		వెళ్ళు docomplete;
	    }
	    ins_pageup();
	    ఆపు;

	స్థితి K_కిందికి:	// <Down>
	    ఒకవేళ (pum_visible())
		వెళ్ళు docomplete;
	    ఒకవేళ (mod_ముసుగు & MOD_ముసుగు_జరుపు)
		ins_pagedown();
	    లేకపోతే
		ins_కిందికి(FALSE);
	    ఆపు;

	స్థితి K_S_కిందికి:	// <S-Down>
	స్థితి K_PAGEDOWN:
	స్థితి K_KPAGEDOWN:
	    ఒకవేళ (pum_visible())
	    {
#నిర్వచితమైతే FEAT_PROP_POPUP
		// CTRL-SHIFT-<Down>/<PageDown> scroll the info popup down.
		ఒకవేళ (c == K_S_కిందికి ? (mod_ముసుగు & MOD_ముసుగు_CTRL)
			: ((mod_ముసుగు & MOD_ముసుగు_CTRL)
			    && (mod_ముసుగు & MOD_ముసుగు_జరుపు)))
		{
		    popup_scroll_info(1, c != K_S_కిందికి);
		    ఆపు;
		}
#అయితే_ముగింపు
		వెళ్ళు docomplete;
	    }
	    ins_pagedown();
	    ఆపు;

#నిర్వచితమైతే FEAT_DND
	స్థితి K_DROP:	// drag-n-drop event
	    ins_drop();
	    ఆపు;
#అయితే_ముగింపు

	స్థితి K_S_TAB:	// When not mapped, use like a normal TAB
	    c = TAB;
	    // FALLTHROUGH

	స్థితి TAB:	// TAB or Complete patterns along path
#ఒకవేళ defined(FEAT_కనుగొను_ID)
	    ఒకవేళ (ctrl_x_బహుళకం_మార్గం_patterns())
		వెళ్ళు docomplete;
#అయితే_ముగింపు
	    inserted_space = FALSE;
	    ఒకవేళ (ins_tab())
		వెళ్ళు normalchar;	// insert TAB as a normal char
	    auto_format(FALSE, TRUE);
	    ఆపు;

	స్థితి K_KENTER:	// <Enter>
	    c = CAR;
	    // FALLTHROUGH
	స్థితి CAR:
	స్థితి NL:
#ఒకవేళ defined(FEAT_QUICKFIX)
	    // In a quickfix window a <CR> jumps to the error under the
	    // cursor.
	    ఒకవేళ (bt_quickfix(curbuf) && c == CAR)
	    {
		ఒకవేళ (curwin->w_llist_ref == ఖాళీ)    // quickfix window
		    do_cmdline_cmd((char_u *)".cc");
		లేకపోతే				    // location list window
		    do_cmdline_cmd((char_u *)".ll");
		ఆపు;
	    }
#అయితే_ముగింపు
	    ఒకవేళ (cmdwin_type != 0)
	    {
		// Execute the command in the cmdline window.
		cmdwin_ఫలితం = CAR;
		వెళ్ళు doESCkey;
	    }
#నిర్వచితమైతే FEAT_పని_CHANNEL
	    ఒకవేళ (bt_prompt(curbuf))
	    {
		invoke_prompt_callback();
		ఒకవేళ (!bt_prompt(curbuf))
		    // buffer changed to a non-prompt buffer, get out of
		    // Insert mode
		    వెళ్ళు doESCkey;
		ఆపు;
	    }
#అయితే_ముగింపు
	    ఒకవేళ (ins_eol(c) == FAIL && !p_im)
		వెళ్ళు doESCkey;	    // out of memory
	    auto_format(FALSE, FALSE);
	    inserted_space = FALSE;
	    ఆపు;

	స్థితి Ctrl_K:	    // digraph or keyword completion
	    ఒకవేళ (ctrl_x_బహుళకం_dictionary())
	    {
		ఒకవేళ (has_compl_option(TRUE))
		    వెళ్ళు docomplete;
		ఆపు;
	    }
#నిర్వచితమైతే FEAT_DIGRAPHS
	    c = ins_digraph();
	    ఒకవేళ (c == NUL)
		ఆపు;
#అయితే_ముగింపు
	    వెళ్ళు normalchar;

	స్థితి Ctrl_X:	// Enter CTRL-X mode
	    ins_ctrl_x();
	    ఆపు;

	స్థితి Ctrl_RSB:	// Tag name completion after ^X
	    ఒకవేళ (!ctrl_x_బహుళకం_ట్యాగులు())
		వెళ్ళు normalchar;
	    వెళ్ళు docomplete;

	స్థితి Ctrl_F:	// File name completion after ^X
	    ఒకవేళ (!ctrl_x_బహుళకం_files())
		వెళ్ళు normalchar;
	    వెళ్ళు docomplete;

	స్థితి 's':	// Spelling completion after ^X
	స్థితి Ctrl_S:
	    ఒకవేళ (!ctrl_x_బహుళకం_spell())
		వెళ్ళు normalchar;
	    వెళ్ళు docomplete;

	స్థితి Ctrl_L:	// Whole line completion after ^X
	    ఒకవేళ (!ctrl_x_బహుళకం_సంపూర్ణ_పంక్తి())
	    {
		// CTRL-L with 'insertmode' set: Leave Insert mode
		ఒకవేళ (p_im)
		{
		    ఒకవేళ (echeck_abbr(Ctrl_L + ABBR_OFF))
			ఆపు;
		    వెళ్ళు doESCkey;
		}
		వెళ్ళు normalchar;
	    }
	    // FALLTHROUGH

	స్థితి Ctrl_P:	// Do previous/next pattern completion
	స్థితి Ctrl_N:
#నిర్వచితమైతే FEAT_PROP_POPUP
	    // CTRL-SHIFT-P/N scroll the info popup one line.
	    ఒకవేళ (pum_visible() && (mod_ముసుగు & MOD_ముసుగు_జరుపు)
		    && (c == Ctrl_P || c == Ctrl_N))
	    {
		popup_scroll_info(c == Ctrl_P ? -1 : 1, false);
		ఆపు;
	    }
#అయితే_ముగింపు
	    // if 'complete' is empty then plain ^P is no longer special,
	    // but it is under other ^X modes
	    ఒకవేళ (*curbuf->b_p_cpt == NUL
		    && (ctrl_x_బహుళకం_సాధారణ() || ctrl_x_బహుళకం_సంపూర్ణ_పంక్తి())
		    && !compl_స్థితి_local())
		వెళ్ళు normalchar;

docomplete:
	    ins_compl_clear_autocomplete_delay();
	    compl_busy = TRUE;
#నిర్వచితమైతే FEAT_FOLDING
	    disable_fold_update++;  // don't redraw folds here
#అయితే_ముగింపు
	    ఒకవేళ (ins_complete(c, TRUE) == FAIL)
		compl_స్థితి_clear();
#నిర్వచితమైతే FEAT_FOLDING
	    disable_fold_update--;
#అయితే_ముగింపు
	    compl_busy = FALSE;
	    can_si = may_do_si(); // allow smartindenting
	    ఆపు;

	స్థితి Ctrl_Y:	// copy from previous line or scroll down
	స్థితి Ctrl_E:	// copy from next line	   or scroll up
	    c = ins_ctrl_ey(c);
	    ఆపు;

	అప్రమేయ:
#నిర్వచితమైతే UNIX
	    ఒకవేళ (c == intr_char)		// special interrupt char
		వెళ్ళు do_intr;
#అయితే_ముగింపు

normalchar:
	    /*
	     * Insert a normal character.
	     */
#ఒకవేళ defined(FEAT_EVAL)
	    ఒకవేళ (!p_paste)
	    {
		// Trigger InsertCharPre.
		char_u *str = do_చొప్పించు_char_pre(c);
		char_u *p;

		ఒకవేళ (str != ఖాళీ)
		{
		    ఒకవేళ (*str != NUL && stop_arrow() != FAIL)
		    {
			// Insert the new value of v:char literally.
			కొరకు (p = str; *p != NUL; MB_PTR_ADV(p))
			{
			    c = PTR2CHAR(p);
			    ఒకవేళ (c == CAR || c == K_KENTER || c == NL)
				ins_eol(c);
			    లేకపోతే
				ins_char(c);
			}
			Append_గమ్యం_Redobuff_Lit(str, -1);
		    }
		    vim_free(str);
		    c = NUL;
		}

		// If the new value is already inserted or an empty string
		// then don't insert any character.
		ఒకవేళ (c == NUL)
		    ఆపు;
	    }
#అయితే_ముగింపు
	    // Try to perform smart-indenting.
	    ins_try_si(c);

	    ఒకవేళ (c == ' ')
	    {
		inserted_space = TRUE;
		ఒకవేళ (inindent(0))
		    can_cindent = FALSE;
		ఒకవేళ (Insstart_blank_vcol == MAXCOL
			&& curwin->w_cursor.lnum == Insstart.lnum)
		    Insstart_blank_vcol = get_nolist_virtcol();
	    }

	    // Insert a normal character and check for abbreviations on a
	    // special character.  Let CTRL-] expand abbreviations without
	    // inserting it.
	    ఒకవేళ (vim_iswordc(c) || (!echeck_abbr(
			// Add ABBR_OFF for characters above 0x100, this is
			// what check_abbr() expects.
				(has_mbyte && c >= 0x100) ? (c + ABBR_OFF) : c)
			&& c != Ctrl_RSB))
	    {
		చొప్పించు_special(c, FALSE, FALSE);
#నిర్వచితమైతే FEAT_RIGHTLEFT
		revins_legal++;
		revins_chars++;
#అయితే_ముగింపు
	    }

	    auto_format(FALSE, TRUE);

#నిర్వచితమైతే FEAT_FOLDING
	    // When inserting a character the cursor line must never be in a
	    // closed fold.
	    foldOpenCursor();
#అయితే_ముగింపు
	    // Trigger autocompletion
	    ఒకవేళ (ins_compl_has_autocomplete() && !char_avail()
		    && vim_isprintc(c))
		TRIGGER_AUTOCOMPLETE();

	    ఆపు;
	}   // end of switch (c)

	// If typed something may trigger CursorHoldI again; K_COMPLETE_DELAY is
	// injected, not typed.
	ఒకవేళ (c != K_CURSORHOLD && c != K_COMPLETE_DELAY
#నిర్వచితమైతే FEAT_COMPL_FUNC
		// but not in CTRL-X mode, a script can't restore the state
		&& ctrl_x_బహుళకం_సాధారణ()
#అయితే_ముగింపు
	       )
	    did_cursorhold = FALSE;

	// Check if we need to cancel completion mode because the window
	// or tab page was changed
	ఒకవేళ (ins_compl_చురుకైన() && !ins_compl_win_చురుకైన(curwin))
	    ins_compl_cancel();

	// If the cursor was moved we didn't just insert a space
	ఒకవేళ (arrow_వాడిన)
	    inserted_space = FALSE;

	ఒకవేళ (can_cindent && cindent_on() && ctrl_x_బహుళకం_సాధారణ())
	{
force_cindent:
	    /*
	     * Indent now if a key was typed that is in 'cinkeys'.
	     */
	    ఒకవేళ (లోపల_cinkeys(c, ' ', పంక్తి_is_white))
	    {
		ఒకవేళ (stop_arrow() == OK)
		    // re-indent the current line
		    do_c_సమాసం_indent();
	    }
	}

    }	// for (;;)
    // NOTREACHED
}

    పూర్ణాంకం
ins_need_undo_get(శూన్య)
{
    తిరిగి ins_need_undo;
}

/*
 * Redraw for Insert mode.
 * This is postponed until getting the next character to make '$' in the 'cpo'
 * option work correctly.
 * Only redraw when there are no characters available.  This speeds up
 * inserting sequences of characters (e.g., for CTRL-R).
 */
    శూన్య
ins_redraw(పూర్ణాంకం ready)	    // not busy with something
{
#నిర్వచితమైతే FEAT_CONCEAL
    linenr_T	conceal_old_cursor_పంక్తి = 0;
    linenr_T	conceal_new_cursor_పంక్తి = 0;
    పూర్ణాంకం		conceal_update_పంక్తులు = FALSE;
#అయితే_ముగింపు

    ఒకవేళ (char_avail())
	తిరిగి;

    // Trigger CursorMoved if the cursor moved.  Not when the popup menu is
    // visible, the command might delete it.
    ఒకవేళ (ready && (has_cursormovedI()
#నిర్వచితమైతే FEAT_PROP_POPUP
		|| popup_visible
#అయితే_ముగింపు
#ఒకవేళ defined(FEAT_CONCEAL)
		|| curwin->w_p_cole > 0
#అయితే_ముగింపు
		)
	    && !EQUAL_POS(చివరి_cursormoved, curwin->w_cursor)
	    && !pum_visible())
    {
#నిర్వచితమైతే FEAT_SYN_HL
	// Need to update the screen first, to make sure syntax
	// highlighting is correct after making a change (e.g., inserting
	// a "(".  The autocommand may also require a redraw, so it's done
	// again below, unfortunately.
	ఒకవేళ (syntax_present(curwin) && must_redraw)
	    update_screen(0);
#అయితే_ముగింపు
	ఒకవేళ (has_cursormovedI())
	{
	    // Make sure curswant is correct, an autocommand may call
	    // getcurpos().
	    update_curswant();
	    ins_apply_autocmds(EVENT_CURSORMOVEDI);
	}
#నిర్వచితమైతే FEAT_PROP_POPUP
	ఒకవేళ (popup_visible)
	    popup_check_cursor_pos();
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_CONCEAL
	ఒకవేళ (curwin->w_p_cole > 0)
	{
	    conceal_old_cursor_పంక్తి = చివరి_cursormoved.lnum;
	    conceal_new_cursor_పంక్తి = curwin->w_cursor.lnum;
	    conceal_update_పంక్తులు = TRUE;
	}
#అయితే_ముగింపు
	చివరి_cursormoved = curwin->w_cursor;
    }

    // Trigger TextChangedI if b_changedtick_i differs.
    ఒకవేళ (ready && has_textchangedI()
	    && curbuf->b_చివరి_changedtick_i != CHANGEDTICK(curbuf)
	    && !pum_visible())
    {
	aco_save_T	aco;
	varnumber_T	tick = CHANGEDTICK(curbuf);

	// Save and restore curwin and curbuf, in case the autocmd changes
	// them.
	aucmd_prepbuf(&aco, curbuf);
	apply_autocmds(EVENT_TEXTCHANGEDI, ఖాళీ, ఖాళీ, FALSE, curbuf);
	aucmd_restbuf(&aco);
	curbuf->b_చివరి_changedtick_i = CHANGEDTICK(curbuf);
	ఒకవేళ (tick != CHANGEDTICK(curbuf))  // see ins_apply_autocmds()
	    u_save(curwin->w_cursor.lnum,
					(linenr_T)(curwin->w_cursor.lnum + 1));
    }

    // Trigger TextChangedP if b_changedtick_pum differs. When the popupmenu
    // closes TextChangedI will need to trigger for backwards compatibility,
    // thus use different b_last_changedtick* variables.
    ఒకవేళ (ready && has_textchangedP()
	    && curbuf->b_చివరి_changedtick_pum != CHANGEDTICK(curbuf)
	    && pum_visible())
    {
	aco_save_T	aco;
	varnumber_T	tick = CHANGEDTICK(curbuf);

	// Save and restore curwin and curbuf, in case the autocmd changes
	// them.
	aucmd_prepbuf(&aco, curbuf);
	apply_autocmds(EVENT_TEXTCHANGEDP, ఖాళీ, ఖాళీ, FALSE, curbuf);
	aucmd_restbuf(&aco);
	curbuf->b_చివరి_changedtick_pum = CHANGEDTICK(curbuf);
	ఒకవేళ (tick != CHANGEDTICK(curbuf))  // see ins_apply_autocmds()
	    u_save(curwin->w_cursor.lnum,
					(linenr_T)(curwin->w_cursor.lnum + 1));
    }

    ఒకవేళ (ready)
	may_trigger_win_scrolled_resized();

    // Trigger SafeState if nothing is pending.
    may_trigger_safestate(ready
	    && !ins_compl_చురుకైన()
	    && !pum_visible());

#ఒకవేళ defined(FEAT_CONCEAL)
    ఒకవేళ ((conceal_update_పంక్తులు
	    && (conceal_old_cursor_పంక్తి != conceal_new_cursor_పంక్తి
		|| conceal_cursor_పంక్తి(curwin)))
	    || need_cursor_పంక్తి_redraw)
    {
	ఒకవేళ (conceal_old_cursor_పంక్తి != conceal_new_cursor_పంక్తి)
	    redrawWinline(curwin, conceal_old_cursor_పంక్తి);
	redrawWinline(curwin, conceal_new_cursor_పంక్తి == 0
			    ? curwin->w_cursor.lnum : conceal_new_cursor_పంక్తి);
	curwin->w_valid &= ~VALID_CROW;
	need_cursor_పంక్తి_redraw = FALSE;
    }
#అయితే_ముగింపు
    ఒకవేళ (must_redraw)
	update_screen(0);
    లేకపోతే ఒకవేళ (clear_cmdline || redraw_cmdline)
	showmode();		// clear cmdline and show mode
    showruler(FALSE);
    setcursor();
    emsg_on_display = FALSE;	// may remove error message now
}

/*
 * Handle a CTRL-V or CTRL-Q typed in Insert mode.
 */
    స్థిర శూన్య
ins_ctrl_v(శూన్య)
{
    పూర్ణాంకం		c;
    పూర్ణాంకం		did_putchar = FALSE;

    // may need to redraw when no more chars available now
    ins_redraw(FALSE);

    ఒకవేళ (redrawing() && !char_avail())
    {
	edit_putchar('^', TRUE);
	did_putchar = TRUE;
    }
    Append_గమ్యం_Redobuff((char_u *)CTRL_V_STR);	// CTRL-V

    కూడు_గమ్యం_showcmd_c(Ctrl_V);

    // Do not change any modifyOtherKeys ESC sequence to a normal key for
    // CTRL-SHIFT-V.
    c = get_literal(mod_ముసుగు & MOD_ముసుగు_జరుపు);
    ఒకవేళ (did_putchar)
	// when the line fits in 'columns' the '^' is at the start of the next
	// line and will not removed by the redraw
	edit_unputchar();
    clear_showcmd();

    చొప్పించు_special(c, FALSE, TRUE);
#నిర్వచితమైతే FEAT_RIGHTLEFT
    revins_chars++;
    revins_legal++;
#అయితే_ముగింపు
}

/*
 * After getting an ESC or CSI for a literal key: If the typeahead buffer
 * contains a modifyOtherKeys sequence then decode it and return the result.
 * Otherwise return "c".
 * Note that this doesn't wait for characters, they must be in the typeahead
 * buffer already.
 */
    స్థిర పూర్ణాంకం
decode_Modify_Other_కీలకాలు(పూర్ణాంకం c)
{
    char_u  *p = typebuf.tb_buf + typebuf.tb_off;
    పూర్ణాంకం	    idx;
    పూర్ణాంకం	    form = 0;
    పూర్ణాంకం	    argidx = 0;
    పూర్ణాంకం	    arg[2] = {0, 0};

    // Recognize:
    // form 0: {lead}{key};{modifier}u
    // form 1: {lead}27;{modifier};{key}~
    ఒకవేళ (typebuf.tb_len >= 4 && (c == CSI || (c == ESC && *p == '[')))
    {
	idx = (*p == '[');
	ఒకవేళ (p[idx] == '2' && p[idx + 1] == '7' && p[idx + 2] == ';' &&
		kitty_protocol_state != KKPS_ENABLED)
	{
	    form = 1;
	    idx += 3;
	}
	అయితే (idx < typebuf.tb_len && argidx < 2)
	{
	    ఒకవేళ (p[idx] == ';')
		++argidx;
	    లేకపోతే ఒకవేళ (VIM_ISDIGIT(p[idx]))
		arg[argidx] = arg[argidx] * 10 + (p[idx] - '0');
	    లేకపోతే
		ఆపు;
	    ++idx;
	}
	పూర్ణాంకం kitty_no_mods = argidx == 0 && kitty_protocol_state == KKPS_ENABLED;
	ఒకవేళ (idx < typebuf.tb_len
		&& p[idx] == (form == 1 ? '~' : 'u')
		&& (argidx == 1 || kitty_no_mods))
	{
	    // Match, consume the code.
	    typebuf.tb_off += idx + 1;
	    typebuf.tb_len -= idx + 1;
#ఒకవేళ defined(FEAT_CLIENTSERVER) || defined(FEAT_EVAL)
	    ఒకవేళ (typebuf.tb_len == 0)
		typebuf_was_filled = FALSE;
#అయితే_ముగింపు

	    mod_ముసుగు = kitty_no_mods ? 0 : decode_modifiers(arg[!form]);
	    c = విలీనం_modify_Other_కీలకాలు(arg[form], &mod_ముసుగు);
	}
    }

    తిరిగి c;
}

/*
 * Put a character directly onto the screen.  It's not stored in a buffer.
 * Used while handling CTRL-K, CTRL-V, etc. in Insert mode.
 */
స్థిర పూర్ణాంకం  pc_స్థితి;
#నిర్వచించు PC_స్థితి_UNSET	0	// pc_bytes was not set
#నిర్వచించు PC_స్థితి_కుడి	1	// right half of double-wide char
#నిర్వచించు PC_స్థితి_ఎడమ	2	// left half of double-wide char
#నిర్వచించు PC_స్థితి_సమితి	3	// pc_bytes was filled
స్థిర char_u pc_బైట్లు[MB_MAXBYTES + 1]; // saved bytes
స్థిర పూర్ణాంకం  pc_attr;
స్థిర పూర్ణాంకం  pc_అడ్డువరుస;
స్థిర పూర్ణాంకం  pc_నిలువు;

    శూన్య
edit_putchar(పూర్ణాంకం c, పూర్ణాంకం highlight)
{
    పూర్ణాంకం	    attr;

    ఒకవేళ (Screen_పంక్తులు == ఖాళీ)
	తిరిగి;

    update_topline();	// just in case w_topline isn't valid
    validate_cursor();
    ఒకవేళ (highlight)
	attr = HL_ATTR(HLF_8);
    లేకపోతే
	attr = 0;
    pc_అడ్డువరుస = W_WINROW(curwin) + curwin->w_wrow;
    pc_నిలువు = curwin->w_wincol;
    pc_స్థితి = PC_స్థితి_UNSET;
#నిర్వచితమైతే FEAT_RIGHTLEFT
    ఒకవేళ (curwin->w_p_rl)
    {
	pc_నిలువు += curwin->w_వెడల్పు - 1 - curwin->w_wcol;
	ఒకవేళ (has_mbyte)
	{
	    పూర్ణాంకం fix_నిలువు = mb_fix_నిలువు(pc_నిలువు, pc_అడ్డువరుస);

	    ఒకవేళ (fix_నిలువు != pc_నిలువు)
	    {
		screen_putchar(' ', pc_అడ్డువరుస, fix_నిలువు, attr);
		--curwin->w_wcol;
		pc_స్థితి = PC_స్థితి_కుడి;
	    }
	}
    }
    లేకపోతే
#అయితే_ముగింపు
    {
	pc_నిలువు += curwin->w_wcol;
	ఒకవేళ (mb_lefthalve(pc_అడ్డువరుస, pc_నిలువు))
	    pc_స్థితి = PC_స్థితి_ఎడమ;
    }

    // save the character to be able to put it back
    ఒకవేళ (pc_స్థితి == PC_స్థితి_UNSET)
    {
	screen_getbytes(pc_అడ్డువరుస, pc_నిలువు, pc_బైట్లు, &pc_attr);
	pc_స్థితి = PC_స్థితి_సమితి;
    }
    screen_putchar(c, pc_అడ్డువరుస, pc_నిలువు, attr);
}

#ఒకవేళ defined(FEAT_పని_CHANNEL)
/*
 * Set the insert start position for when using a prompt buffer.
 */
    శూన్య
సమితి_insstart(linenr_T lnum, పూర్ణాంకం నిలువు)
{
    Insstart.lnum = lnum;
    Insstart.నిలువు = నిలువు;
    Insstart_orig = Insstart;
    Insstart_textlen = Insstart.నిలువు;
    Insstart_blank_vcol = MAXCOL;
    arrow_వాడిన = FALSE;
}
#అయితే_ముగింపు

/*
 * Undo the previous edit_putchar().
 */
    శూన్య
edit_unputchar(శూన్య)
{
    ఒకవేళ (pc_స్థితి != PC_స్థితి_UNSET && pc_అడ్డువరుస >= సందేశం_scrolled)
    {
	ఒకవేళ (pc_స్థితి == PC_స్థితి_కుడి)
	    ++curwin->w_wcol;
	ఒకవేళ (pc_స్థితి == PC_స్థితి_కుడి || pc_స్థితి == PC_స్థితి_ఎడమ)
	    redrawWinline(curwin, curwin->w_cursor.lnum);
	లేకపోతే
	    screen_puts(pc_బైట్లు, pc_అడ్డువరుస - సందేశం_scrolled, pc_నిలువు, pc_attr);
    }
}

/*
 * Called when "$" is in 'cpoptions': display a '$' at the end of the changed
 * text.  Only works when cursor is in the line that changes.
 */
    శూన్య
display_dollar(colnr_T నిలువు_arg)
{
    colnr_T నిలువు = నిలువు_arg < 0 ? 0 : నిలువు_arg;
    colnr_T save_నిలువు;

    ఒకవేళ (!redrawing())
	తిరిగి;

    cursor_off();
    save_నిలువు = curwin->w_cursor.నిలువు;
    curwin->w_cursor.నిలువు = నిలువు;
    ఒకవేళ (has_mbyte)
    {
	char_u *p;

	// If on the last byte of a multi-byte move to the first byte.
	p = ml_get_curline();
	curwin->w_cursor.నిలువు -= (*mb_తల_off)(p, p + నిలువు);
    }
    curs_columns(FALSE);	    // recompute w_wrow and w_wcol
    ఒకవేళ (curwin->w_wcol < curwin->w_వెడల్పు)
    {
	edit_putchar('$', FALSE);
	dollar_vcol = curwin->w_virtcol;
    }
    curwin->w_cursor.నిలువు = save_నిలువు;
}

/*
 * Call this function before moving the cursor from the normal insert position
 * in insert mode.
 */
    శూన్య
undisplay_dollar(శూన్య)
{
    ఒకవేళ (dollar_vcol < 0)
	తిరిగి;

    dollar_vcol = -1;
    redrawWinline(curwin, curwin->w_cursor.lnum);
}

/*
 * Truncate the space at the end of a line.  This is to be used only in an
 * insert mode.  It handles fixing the replace stack for MODE_REPLACE and
 * MODE_VREPLACE modes.
 */
    శూన్య
truncate_spaces(char_u *పంక్తి, size_t len)
{
    పూర్ణాంకం	    i;

    // find start of trailing white space
    కొరకు (i = (పూర్ణాంకం)len - 1; i >= 0 && VIM_ISWHITE(పంక్తి[i]); i--)
    {
	ఒకవేళ (State & REPLACE_FLAG)
	    replace_join(0);	    // remove a NUL from the replace stack
    }
    పంక్తి[i + 1] = NUL;
}

/*
 * Backspace the cursor until the given column.  Handles MODE_REPLACE and
 * MODE_VREPLACE modes correctly.  May also be used when not in insert mode at
 * all.  Will attempt not to go before "col" even when there is a composing
 * character.
 */
    శూన్య
backspace_until_స్తంభం(పూర్ణాంకం నిలువు)
{
    అయితే ((పూర్ణాంకం)curwin->w_cursor.నిలువు > నిలువు)
    {
	curwin->w_cursor.నిలువు--;
	ఒకవేళ (State & REPLACE_FLAG)
	    replace_do_bs(నిలువు);
	లేకపోతే ఒకవేళ (!del_char_after_నిలువు(నిలువు))
	    ఆపు;
    }
}

/*
 * Like del_char(), but make sure not to go before column "limit_col".
 * Only matters when there are composing characters.
 * Return TRUE when something was deleted.
 */
    స్థిర పూర్ణాంకం
del_char_after_నిలువు(పూర్ణాంకం హద్దు_నిలువు వాడని)
{
    ఒకవేళ (enc_utf8 && హద్దు_నిలువు >= 0)
    {
	colnr_T ecol = curwin->w_cursor.నిలువు + 1;

	// Make sure the cursor is at the start of a character, but
	// skip forward again when going too far back because of a
	// composing character.
	mb_adjust_cursor();
	అయితే (curwin->w_cursor.నిలువు < (colnr_T)హద్దు_నిలువు)
	{
	    పూర్ణాంకం l = utf_ptr2len(ml_get_cursor());

	    ఒకవేళ (l == 0)  // end of line
		ఆపు;
	    curwin->w_cursor.నిలువు += l;
	}
	ఒకవేళ (*ml_get_cursor() == NUL || curwin->w_cursor.నిలువు == ecol)
	    తిరిగి FALSE;
	del_బైట్లు((దీర్ఘ)((పూర్ణాంకం)ecol - curwin->w_cursor.నిలువు), FALSE, TRUE);
    }
    లేకపోతే
	(శూన్య)del_char(FALSE);
    తిరిగి TRUE;
}

/*
 * Next character is interpreted literally.
 * A one, two or three digit decimal number is interpreted as its byte value.
 * If one or two digits are entered, the next character is given to vungetc().
 * For Unicode a character > 255 may be returned.
 * If "noReduceKeys" is TRUE do not change any modifyOtherKeys ESC sequence
 * into a normal key, return ESC.
 */
    పూర్ణాంకం
get_literal(పూర్ణాంకం no_Reduce_కీలకాలు)
{
    పూర్ణాంకం		cc;
    పూర్ణాంకం		nc;
    పూర్ణాంకం		i;
    పూర్ణాంకం		hex = FALSE;
    పూర్ణాంకం		octal = FALSE;
    పూర్ణాంకం		unicode = 0;

    ఒకవేళ (got_int)
	తిరిగి Ctrl_C;

#నిర్వచితమైతే FEAT_GUI
    /*
     * In GUI there is no point inserting the internal code for a special key.
     * It is more useful to insert the string "<KEY>" instead.	This would
     * probably be useful in a text window too, but it would not be
     * vi-compatible (maybe there should be an option for it?) -- webb
     */
    ఒకవేళ (gui.లోపల_use)
    {
	++allow_కీలకాలు;
	ఒకవేళ (no_Reduce_కీలకాలు)
	    ++no_reduce_కీలకాలు;
    }
#అయితే_ముగింపు
#నిర్వచితమైతే USE_ON_FLY_SCROLL
    dont_scroll = TRUE;		// disallow scrolling here
#అయితే_ముగింపు
    ++no_mapping;		// don't map the next key hits
    cc = 0;
    i = 0;
    కొరకు (;;)
    {
	nc = plain_vgetc();
	ఒకవేళ ((nc == ESC || nc == CSI) && !no_Reduce_కీలకాలు)
	    nc = decode_Modify_Other_కీలకాలు(nc);

	ఒకవేళ ((mod_ముసుగు & ~MOD_ముసుగు_జరుపు) != 0)
	    // A character with non-Shift modifiers should not be a valid
	    // character for i_CTRL-V_digit.
	    ఆపు;

	ఒకవేళ ((State & బహుళకం_CMDLINE) == 0 && MB_BYTE2LEN_CHECK(nc) == 1)
	    కూడు_గమ్యం_showcmd(nc);
	ఒకవేళ (nc == 'x' || nc == 'X')
	    hex = TRUE;
	లేకపోతే ఒకవేళ (nc == 'o' || nc == 'O')
	    octal = TRUE;
	లేకపోతే ఒకవేళ (nc == 'u' || nc == 'U')
	    unicode = nc;
	లేకపోతే
	{
	    ఒకవేళ (hex || unicode != 0)
	    {
		ఒకవేళ (!vim_isxdigit(nc))
		    ఆపు;
		cc = cc * 16 + hex2nr(nc);
	    }
	    లేకపోతే ఒకవేళ (octal)
	    {
		ఒకవేళ (nc < '0' || nc > '7')
		    ఆపు;
		cc = cc * 8 + nc - '0';
	    }
	    లేకపోతే
	    {
		ఒకవేళ (!VIM_ISDIGIT(nc))
		    ఆపు;
		cc = cc * 10 + nc - '0';
	    }

	    ++i;
	}

	ఒకవేళ (cc > 255 && unicode == 0)
	    cc = 255;		// limit range to 0-255
	nc = 0;

	ఒకవేళ (hex)		// hex: up to two chars
	{
	    ఒకవేళ (i >= 2)
		ఆపు;
	}
	లేకపోతే ఒకవేళ (unicode)	// Unicode: up to four or eight chars
	{
	    ఒకవేళ ((unicode == 'u' && i >= 4) || (unicode == 'U' && i >= 8))
		ఆపు;
	}
	లేకపోతే ఒకవేళ (i >= 3)	// decimal or octal: up to three chars
	    ఆపు;
    }
    ఒకవేళ (i == 0)	    // no number entered
    {
	ఒకవేళ (nc == K_ZERO)   // NUL is stored as NL
	{
	    cc = '\n';
	    nc = 0;
	}
	లేకపోతే
	{
	    cc = nc;
	    nc = 0;
	}
    }

    ఒకవేళ (cc == 0)	// NUL is stored as NL
	cc = '\n';
    ఒకవేళ (enc_dbcs && (cc & 0xff) == 0)
	cc = '?';	// don't accept an illegal DBCS char, the NUL in the
			// second byte will cause trouble!

    --no_mapping;
#నిర్వచితమైతే FEAT_GUI
    ఒకవేళ (gui.లోపల_use)
    {
	--allow_కీలకాలు;
	ఒకవేళ (no_Reduce_కీలకాలు)
	    --no_reduce_కీలకాలు;
    }
#అయితే_ముగింపు
    ఒకవేళ (nc)
    {
	vungetc(nc);
	// A character typed with i_CTRL-V_digit cannot have modifiers.
	mod_ముసుగు = 0;
    }
    got_int = FALSE;	    // CTRL-C typed after CTRL-V is not an interrupt
    తిరిగి cc;
}

/*
 * Insert character, taking care of special keys and mod_mask
 */
    స్థిర శూన్య
చొప్పించు_special(
    పూర్ణాంకం	    c,
    పూర్ణాంకం	    allow_modmask,
    పూర్ణాంకం	    ctrlv)	    // c was typed after CTRL-V
{
    char_u  *p;
    పూర్ణాంకం	    len;

    /*
     * Special function key, translate into "<Key>". Up to the last '>' is
     * inserted with ins_str(), so as not to replace characters in replace
     * mode.
     * Only use mod_mask for special keys, to avoid things like <S-Space>,
     * unless 'allow_modmask' is TRUE.
     */
#ఒకవేళ defined(MACOS_X) || defined(FEAT_GUI_GTK)
    // Command-key never produces a normal key
    ఒకవేళ (mod_ముసుగు & MOD_ముసుగు_CMD)
	allow_modmask = TRUE;
#అయితే_ముగింపు
    ఒకవేళ (IS_SPECIAL(c) || (mod_ముసుగు && allow_modmask))
    {
	p = get_special_కీలకం_name(c, mod_ముసుగు);
	len = (పూర్ణాంకం)STRLEN(p);
	c = p[len - 1];
	ఒకవేళ (len > 2)
	{
	    ఒకవేళ (stop_arrow() == FAIL)
		తిరిగి;
	    p[len - 1] = NUL;
	    ins_str(p, len - 1);
	    Append_గమ్యం_Redobuff_Lit(p, -1);
	    ctrlv = FALSE;
	}
    }
    ఒకవేళ (stop_arrow() == OK)
	insertchar(c, ctrlv ? INSCHAR_CTRLV : 0, -1);
}

/*
 * Special characters in this context are those that need processing other
 * than the simple insertion that can be performed here. This includes ESC
 * which terminates the insert, and CR/NL which need special processing to
 * open up a new line. This routine tries to optimize insertions performed by
 * the "redo", "undo" or "put" commands, so it needs to know when it should
 * stop and defer processing to the "normal" mechanism.
 * '0' and '^' are special, because they can be followed by CTRL-D.
 */
#నిర్వచించు ISSPECIAL(c)	((c) < ' ' || (c) >= DEL || (c) == '0' || (c) == '^')

/*
 * "flags": INSCHAR_FORMAT - force formatting
 *	    INSCHAR_CTRLV  - char typed just after CTRL-V
 *	    INSCHAR_NO_FEX - don't use 'formatexpr'
 *
 *   NOTE: passes the flags value straight through to internal_format() which,
 *	   beside INSCHAR_FORMAT (above), is also looking for these:
 *	    INSCHAR_DO_COM   - format comments
 *	    INSCHAR_COM_LIST - format comments with num list or 2nd line indent
 */
    శూన్య
insertchar(
    పూర్ణాంకం		c,			// character to insert or NUL
    పూర్ణాంకం		flags,			// INSCHAR_FORMAT, etc.
    పూర్ణాంకం		రెండవ_indent)		// indent for second line if >= 0
{
    పూర్ణాంకం		textwidth;
    char_u	*p;
    పూర్ణాంకం		fo_ins_blank;
    పూర్ణాంకం		force_format = flags & INSCHAR_FORMAT;

    textwidth = comp_textwidth(force_format);
    fo_ins_blank = has_format_option(FO_INS_BLANK);

    /*
     * Try to break the line in two or more pieces when:
     * - Always do this if we have been called to do formatting only.
     * - Always do this when 'formatoptions' has the 'a' flag and the line
     *   ends in white space.
     * - Otherwise:
     *	 - Don't do this if inserting a blank
     *	 - Don't do this if an existing character is being replaced, unless
     *	   we're in MODE_VREPLACE state.
     *	 - Do this if the cursor is not on the line where insert started
     *	 or - 'formatoptions' doesn't have 'l' or the line was not too long
     *	       before the insert.
     *	    - 'formatoptions' doesn't have 'b' or a blank was inserted at or
     *	      before 'textwidth'
     */
    ఒకవేళ (textwidth > 0
	    && (force_format
		|| (!VIM_ISWHITE(c)
		    && !((State & REPLACE_FLAG)
			&& !(State & VREPLACE_FLAG)
			&& *ml_get_cursor() != NUL)
		    && (curwin->w_cursor.lnum != Insstart.lnum
			|| ((!has_format_option(FO_INS_LONG)
				|| Insstart_textlen <= (colnr_T)textwidth)
			    && (!fo_ins_blank
				|| Insstart_blank_vcol <= (colnr_T)textwidth
			    ))))))
    {
	// Format with 'formatexpr' when it's set.  Use internal formatting
	// when 'formatexpr' isn't set or it returns non-zero.
#ఒకవేళ defined(FEAT_EVAL)
	పూర్ణాంకం     do_అంతర్గత = TRUE;
	colnr_T virtcol = get_nolist_virtcol()
				  + char2cells(c != NUL ? c : gchar_cursor());

	ఒకవేళ (*curbuf->b_p_fex != NUL && (flags & INSCHAR_NO_FEX) == 0
		&& (force_format || virtcol > (colnr_T)textwidth))
	{
	    do_అంతర్గత = (fex_format(curwin->w_cursor.lnum, 1L, c) != 0);
	    // It may be required to save for undo again, e.g. when setline()
	    // was called.
	    ins_need_undo = TRUE;
	}
	ఒకవేళ (do_అంతర్గత)
#అయితే_ముగింపు
	    అంతర్గత_format(textwidth, రెండవ_indent, flags, c == NUL, c);
    }

    ఒకవేళ (c == NUL)	    // only formatting was wanted
	తిరిగి;

    // Check whether this character should end a comment.
    ఒకవేళ (did_ai && c == చివర_comment_pending)
    {
	char_u  *పంక్తి;
	char_u	lead_చివర[COM_MAX_LEN];	    // end-comment string
	పూర్ణాంకం	మధ్య_len, చివర_len;
	పూర్ణాంకం	i;

	/*
	 * Need to remove existing (middle) comment leader and insert end
	 * comment leader.  First, check what comment leader we can find.
	 */
	i = get_leader_len(పంక్తి = ml_get_curline(), &p, FALSE, TRUE);
	ఒకవేళ (i > 0 && vim_strchr(p, COM_మధ్య) != ఖాళీ)	// Just checking
	{
	    // Skip middle-comment string
	    అయితే (*p && p[-1] != ':')	// find end of middle flags
		++p;
	    మధ్య_len = copy_option_part(&p, lead_చివర, COM_MAX_LEN, ",");
	    // Don't count trailing white space for middle_len
	    అయితే (మధ్య_len > 0 && VIM_ISWHITE(lead_చివర[మధ్య_len - 1]))
		--మధ్య_len;

	    // Find the end-comment string
	    అయితే (*p && p[-1] != ':')	// find end of end flags
		++p;
	    చివర_len = copy_option_part(&p, lead_చివర, COM_MAX_LEN, ",");

	    // Skip white space before the cursor
	    i = curwin->w_cursor.నిలువు;
	    అయితే (--i >= 0 && VIM_ISWHITE(పంక్తి[i]))
		;
	    i++;

	    // Skip to before the middle leader
	    i -= మధ్య_len;

	    // Check some expected things before we go on
	    ఒకవేళ (i >= 0 && చివర_len > 0 && lead_చివర[చివర_len - 1] == చివర_comment_pending)
	    {
		// Backspace over all the stuff we want to replace
		backspace_until_స్తంభం(i);

		// Insert the end-comment string, except for the last
		// character, which will get inserted as normal later.
		ins_బైట్లు_len(lead_చివర, చివర_len - 1);
	    }
	}
    }
    చివర_comment_pending = NUL;

    did_ai = FALSE;
    did_si = FALSE;
    can_si = FALSE;
    can_si_వెనుక = FALSE;

    /*
     * If there's any pending input, grab up to INPUT_BUFLEN at once.
     * This speeds up normal text input considerably.
     * Don't do this when 'cindent' or 'indentexpr' is set, because we might
     * need to re-indent at a ':', or any other character (but not what
     * 'paste' is set)..
     * Don't do this when there an InsertCharPre autocommand is defined,
     * because we need to fire the event for every character.
     * Do the check for InsertCharPre before the call to vpeekc() because the
     * InsertCharPre autocommand could change the input buffer.
     */
#నిర్వచితమైతే USE_ON_FLY_SCROLL
    dont_scroll = FALSE;		// allow scrolling here
#అయితే_ముగింపు

    ఒకవేళ (       !ISSPECIAL(c)
	    && (!has_mbyte || (*mb_char2len)(c) == 1)
	    && !has_insertcharpre()
#నిర్వచితమైతే FEAT_EVAL
	    // Skip typeahead if test_override("char_avail", 1) was called.
	    && !disable_char_avail_for_testing
#అయితే_ముగింపు
	    && vpeekc() != NUL
	    && !(State & REPLACE_FLAG)
	    && !cindent_on()
#నిర్వచితమైతే FEAT_RIGHTLEFT
	    && !p_ri
#అయితే_ముగింపు
	   )
    {
#నిర్వచించు ఆదానం_BUFLEN 100
	char_u		buf[ఆదానం_BUFLEN + 1];
	పూర్ణాంకం		i;
	colnr_T		virtcol = 0;

	buf[0] = c;
	i = 1;
	ఒకవేళ (textwidth > 0)
	    virtcol = get_nolist_virtcol();
	/*
	 * Stop the string when:
	 * - no more chars available
	 * - finding a special character (command key)
	 * - buffer is full
	 * - running into the 'textwidth' boundary
	 * - need to check for abbreviation: A non-word char after a word-char
	 */
	అయితే (	   (c = vpeekc()) != NUL
		&& !ISSPECIAL(c)
		&& (!has_mbyte || MB_BYTE2LEN_CHECK(c) == 1)
		&& i < ఆదానం_BUFLEN
		&& (textwidth == 0
		    || (virtcol += byte2cells(buf[i - 1])) < (colnr_T)textwidth)
		&& !(!no_abbr && !vim_iswordc(c) && vim_iswordc(buf[i - 1])))
	{
#నిర్వచితమైతే FEAT_RIGHTLEFT
	    c = vgetc();
	    ఒకవేళ (p_hkmap && కీలకం_Typed)
		c = hkmap(c);		    // Hebrew mode mapping
	    buf[i++] = c;
#లేకపోతే
	    buf[i++] = vgetc();
#అయితే_ముగింపు
	}

#నిర్వచితమైతే FEAT_DIGRAPHS
	do_digraph(-1);			// clear digraphs
	do_digraph(buf[i-1]);		// may be the start of a digraph
#అయితే_ముగింపు
	buf[i] = NUL;
	ins_str(buf, i);
	ఒకవేళ (flags & INSCHAR_CTRLV)
	{
	    redo_literal(*buf);
	    i = 1;
	}
	లేకపోతే
	    i = 0;
	ఒకవేళ (buf[i] != NUL)
	    Append_గమ్యం_Redobuff_Lit(buf + i, -1);
    }
    లేకపోతే
    {
	పూర్ణాంకం		cc;

	ఒకవేళ (has_mbyte && (cc = (*mb_char2len)(c)) > 1)
	{
	    char_u	buf[MB_MAXBYTES + 1];

	    (*mb_char2bytes)(c, buf);
	    buf[cc] = NUL;
	    ins_char_బైట్లు(buf, cc);
	    Append_Char_గమ్యం_Redobuff(c);
	}
	లేకపోతే
	{
	    ins_char(c);
	    ఒకవేళ (flags & INSCHAR_CTRLV)
		redo_literal(c);
	    లేకపోతే
		Append_Char_గమ్యం_Redobuff(c);
	}
    }
}

/*
 * Put a character in the redo buffer, for when just after a CTRL-V.
 */
    స్థిర శూన్య
redo_literal(పూర్ణాంకం c)
{
    char_u	buf[10];

    // Only digits need special treatment.  Translate them into a string of
    // three digits.
    ఒకవేళ (VIM_ISDIGIT(c))
    {
	vim_snprintf((అక్షరం *)buf, పరిమాణం(buf), "%03d", c);
	Append_గమ్యం_Redobuff(buf);
    }
    లేకపోతే
	Append_Char_గమ్యం_Redobuff(c);
}

/*
 * start_arrow() is called when an arrow key is used in insert mode.
 * For undo/redo it resembles hitting the <ESC> key.
 */
    శూన్య
ప్రారంభం_arrow(
    pos_T    *చివర_చొప్పించు_pos)		// can be NULL
{
    ప్రారంభం_arrow_common(చివర_చొప్పించు_pos, TRUE);
}

/*
 * Like start_arrow() but with end_change argument.
 * Will prepare for redo of CTRL-G U if "end_change" is FALSE.
 */
    స్థిర శూన్య
ప్రారంభం_arrow_with_change(
    pos_T    *చివర_చొప్పించు_pos,		// can be NULL
    పూర్ణాంకం	      చివర_change)		// end undoable change
{
    ప్రారంభం_arrow_common(చివర_చొప్పించు_pos, చివర_change);
    ఒకవేళ (!చివర_change)
    {
	Append_Char_గమ్యం_Redobuff(Ctrl_G);
	Append_Char_గమ్యం_Redobuff('U');
    }
}

    స్థిర శూన్య
ప్రారంభం_arrow_common(
    pos_T    *చివర_చొప్పించు_pos,		// can be NULL
    పూర్ణాంకం	      చివర_change)		// end undoable change
{
    ఒకవేళ (!arrow_వాడిన && చివర_change)	// something has been inserted
    {
	Append_గమ్యం_Redobuff(ESC_STR);
	stop_చొప్పించు(చివర_చొప్పించు_pos, FALSE, FALSE);
	arrow_వాడిన = TRUE;	// this means we stopped the current insert
    }
#నిర్వచితమైతే FEAT_SPELL
    check_spell_redraw();
#అయితే_ముగింపు
}

#నిర్వచితమైతే FEAT_SPELL
/*
 * If we skipped highlighting word at cursor, do it now.
 * It may be skipped again, thus reset spell_redraw_lnum first.
 */
    స్థిర శూన్య
check_spell_redraw(శూన్య)
{
    ఒకవేళ (spell_redraw_lnum != 0)
    {
	linenr_T	lnum = spell_redraw_lnum;

	spell_redraw_lnum = 0;
	redrawWinline(curwin, lnum);
    }
}

#అయితే_ముగింపు

/*
 * stop_arrow() is called before a change is made in insert mode.
 * If an arrow key has been used, start a new insertion.
 * Returns FAIL if undo is impossible, shouldn't insert then.
 */
    పూర్ణాంకం
stop_arrow(శూన్య)
{
    ఒకవేళ (arrow_వాడిన)
    {
	Insstart = curwin->w_cursor;	// new insertion starts here
	ఒకవేళ (Insstart.నిలువు > Insstart_orig.నిలువు && !ins_need_undo)
	    // Don't update the original insert position when moved to the
	    // right, except when nothing was inserted yet.
	    update_Insstart_orig = FALSE;
	Insstart_textlen = (colnr_T)linetabsize_str(ml_get_curline());

	ఒకవేళ (u_save_cursor() == OK)
	{
	    arrow_వాడిన = FALSE;
	    ins_need_undo = FALSE;
	}

	ai_నిలువు = 0;
	ఒకవేళ (State & VREPLACE_FLAG)
	{
	    orig_పంక్తి_లెక్క = curbuf->b_ml.ml_పంక్తి_లెక్క;
	    vr_పంక్తులు_మారింది = 1;
	}
	ResetRedobuff();
	Append_గమ్యం_Redobuff((char_u *)"1i");   // pretend we start an insertion
	new_చొప్పించు_skip = 2;
    }
    లేకపోతే ఒకవేళ (ins_need_undo)
    {
	ఒకవేళ (u_save_cursor() == OK)
	{
	    // A command or event may have moved the cursor before the next
	    // edit. Pull Insstart back only when the cursor moved above it,
	    // so that later edits can properly decide whether an extra undo
	    // entry is needed. Advancing Insstart would mis-place '[ after a
	    // register paste.
	    ఒకవేళ (LT_POS(curwin->w_cursor, Insstart))
	    {
		Insstart = curwin->w_cursor;
		Insstart_textlen = (colnr_T)linetabsize_str(ml_get_curline());
	    }
	    ins_need_undo = FALSE;
	}
    }

#నిర్వచితమైతే FEAT_FOLDING
    // Always open fold at the cursor line when inserting something.
    foldOpenCursor();
#అయితే_ముగింపు

    తిరిగి (arrow_వాడిన || ins_need_undo ? FAIL : OK);
}

/*
 * Do a few things to stop inserting.
 * "end_insert_pos" is where insert ended.  It is NULL when we already jumped
 * to another window/buffer.
 */
    స్థిర శూన్య
stop_చొప్పించు(
    pos_T	*చివర_చొప్పించు_pos,
    పూర్ణాంకం		esc,			// called by ins_esc()
    పూర్ణాంకం		nomove)			// <c-\><c-o>, don't move cursor
{
    పూర్ణాంకం		cc;
    string_T	inserted;

    stop_redo_ins();
    replace_flush();		// abandon replace stack

    /*
     * Save the inserted text for later redo with ^@ and CTRL-A.
     * Don't do it when "restart_edit" was set and nothing was inserted,
     * otherwise CTRL-O w and then <Left> will clear "last_insert".
     */
    inserted = get_inserted();
    పూర్ణాంకం added = inserted.string == ఖాళీ ? 0 : (పూర్ణాంకం)inserted.నిడివి - new_చొప్పించు_skip;
    ఒకవేళ (did_restart_edit == 0 || added > 0)
    {
	vim_free(చివరి_చొప్పించు.string);
	చివరి_చొప్పించు = inserted;			    // structure copy
	చివరి_చొప్పించు_skip = added < 0 ? 0 : new_చొప్పించు_skip;
    }
    లేకపోతే
	vim_free(inserted.string);

    ఒకవేళ (!arrow_వాడిన && చివర_చొప్పించు_pos != ఖాళీ)
    {
	// Auto-format now.  It may seem strange to do this when stopping an
	// insertion (or moving the cursor), but it's required when appending
	// a line and having it end in a space.  But only do it when something
	// was actually inserted, otherwise undo won't work.
	ఒకవేళ (!ins_need_undo && has_format_option(FO_AUTO))
	{
	    pos_T   tpos = curwin->w_cursor;

	    // When the cursor is at the end of the line after a space the
	    // formatting will move it to the following word.  Avoid that by
	    // moving the cursor onto the space.
	    cc = 'x';
	    ఒకవేళ (curwin->w_cursor.నిలువు > 0 && gchar_cursor() == NUL)
	    {
		dec_cursor();
		cc = gchar_cursor();
		ఒకవేళ (!VIM_ISWHITE(cc))
		    curwin->w_cursor = tpos;
	    }

	    auto_format(TRUE, FALSE);

	    ఒకవేళ (VIM_ISWHITE(cc))
	    {
		ఒకవేళ (gchar_cursor() != NUL)
		    inc_cursor();
		// If the cursor is still at the same character, also keep
		// the "coladd".
		ఒకవేళ (gchar_cursor() == NUL
			&& curwin->w_cursor.lnum == tpos.lnum
			&& curwin->w_cursor.నిలువు == tpos.నిలువు)
		    curwin->w_cursor.coladd = tpos.coladd;
	    }
	}

	// If a space was inserted for auto-formatting, remove it now.
	check_auto_format(TRUE);

	// If we just did an auto-indent, remove the white space from the end
	// of the line, and put the cursor back.
	// Do this when ESC was used or moving the cursor up/down.
	// Check for the old position still being valid, just in case the text
	// got changed unexpectedly.
	ఒకవేళ (!nomove && did_ai && (esc || (vim_strchr(p_cpo, CPO_INDENT) == ఖాళీ
			&& curwin->w_cursor.lnum != చివర_చొప్పించు_pos->lnum))
		&& చివర_చొప్పించు_pos->lnum <= curbuf->b_ml.ml_పంక్తి_లెక్క)
	{
	    pos_T	tpos = curwin->w_cursor;
	    colnr_T	prev_నిలువు = చివర_చొప్పించు_pos->నిలువు;
	    colnr_T	strip_నిలువు;

	    curwin->w_cursor = *చివర_చొప్పించు_pos;
	    check_cursor_నిలువు();  // make sure it is not past the line

	    // Where the loop would actually start (back up if on NUL).
	    strip_నిలువు = curwin->w_cursor.నిలువు;
	    ఒకవేళ (gchar_cursor() == NUL && strip_నిలువు > 0)
		--strip_నిలువు;

	    // Don't strip if non-whitespace follows: setline() from a <Cmd>
	    // mapping or CursorHoldI autocmd may have inserted content.
	    ఒకవేళ (*skipwhite(ml_get_curline() + strip_నిలువు) == NUL)
	    {
		curwin->w_cursor.నిలువు = strip_నిలువు;
		కొరకు (;;)
		{
		    ఒకవేళ (gchar_cursor() == NUL && curwin->w_cursor.నిలువు > 0)
			--curwin->w_cursor.నిలువు;
		    cc = gchar_cursor();
		    ఒకవేళ (!VIM_ISWHITE(cc))
			ఆపు;
		    ఒకవేళ (del_char(TRUE) == FAIL)
			ఆపు;  // should not happen
		}
		ఒకవేళ (curwin->w_cursor.lnum != tpos.lnum)
		    curwin->w_cursor = tpos;
		లేకపోతే ఒకవేళ (curwin->w_cursor.నిలువు < prev_నిలువు)
		{
		    // reset tpos, could have been invalidated in the loop above
		    tpos = curwin->w_cursor;
		    tpos.నిలువు++;
		    ఒకవేళ (cc != NUL && gchar_pos(&tpos) == NUL)
			++curwin->w_cursor.నిలువు;	// put cursor back on the NUL
		}

		// <C-S-Right> may have started Visual mode, adjust the position for
		// deleted characters.
		ఒకవేళ (VIsual_చురుకైన)
		    check_visual_pos();
	    }
	    లేకపోతే
		// Non-whitespace follows, keep original cursor.
		curwin->w_cursor = tpos;
	}
    }
    did_ai = FALSE;
    did_si = FALSE;
    can_si = FALSE;
    can_si_వెనుక = FALSE;

    // Set '[ and '] to the inserted text.  When end_insert_pos is NULL we are
    // now in a different buffer.
    ఒకవేళ (చివర_చొప్పించు_pos != ఖాళీ)
    {
	curbuf->b_op_ప్రారంభం = Insstart;
	curbuf->b_op_ప్రారంభం_orig = Insstart_orig;
	curbuf->b_op_చివర = *చివర_చొప్పించు_pos;
    }
}

/*
 * Set the last inserted text to a single character.
 * Used for the replace command.
 */
    శూన్య
సమితి_చివరి_చొప్పించు(పూర్ణాంకం c)
{
    char_u	*s;

    vim_free(చివరి_చొప్పించు.string);
    చివరి_చొప్పించు.string = alloc(MB_MAXBYTES * 3 + 5);
    ఒకవేళ (చివరి_చొప్పించు.string == ఖాళీ)
    {
	చివరి_చొప్పించు.నిడివి = 0;
	తిరిగి;
    }

    s = చివరి_చొప్పించు.string;
    // Use the CTRL-V only when entering a special char
    ఒకవేళ (c < ' ' || c == DEL)
	*s++ = Ctrl_V;
    s = కూడు_char2buf(c, s);
    *s++ = ESC;
    *s = NUL;
    చివరి_చొప్పించు.నిడివి = (size_t)(s - చివరి_చొప్పించు.string);
    చివరి_చొప్పించు_skip = 0;
}

#ఒకవేళ defined(EXITFREE)
    శూన్య
free_చివరి_చొప్పించు(శూన్య)
{
    VIM_CLEAR_STRING(చివరి_చొప్పించు);
}
#అయితే_ముగింపు

/*
 * Add character "c" to buffer "s".  Escape the special meaning of K_SPECIAL
 * and CSI.  Handle multi-byte characters.
 * Returns a pointer to after the added bytes.
 */
    char_u *
కూడు_char2buf(పూర్ణాంకం c, char_u *s)
{
    char_u	తాత్కాలిక[MB_MAXBYTES + 1];
    పూర్ణాంకం		i;
    పూర్ణాంకం		len;

    len = (*mb_char2bytes)(c, తాత్కాలిక);
    కొరకు (i = 0; i < len; ++i)
    {
	c = తాత్కాలిక[i];
	// Need to escape K_SPECIAL and CSI like in the typeahead buffer.
	ఒకవేళ (c == K_SPECIAL)
	{
	    *s++ = K_SPECIAL;
	    *s++ = KS_SPECIAL;
	    *s++ = KE_FILLER;
	}
#నిర్వచితమైతే FEAT_GUI
	లేకపోతే ఒకవేళ (c == CSI)
	{
	    *s++ = CSI;
	    *s++ = KS_EXTRA;
	    *s++ = (పూర్ణాంకం)KE_CSI;
	}
#అయితే_ముగింపు
	లేకపోతే
	    *s++ = c;
    }
    తిరిగి s;
}

/*
 * move cursor to start of line
 * if flags & BL_WHITE	move to first non-white
 * if flags & BL_SOL	move to first non-white if startofline is set,
 *			    otherwise keep "curswant" column
 * if flags & BL_FIX	don't leave the cursor on a NUL.
 */
    శూన్య
beginline(పూర్ణాంకం flags)
{
    ఒకవేళ ((flags & BL_SOL) && !p_sol)
	coladvance(curwin->w_curswant);
    లేకపోతే
    {
	curwin->w_cursor.నిలువు = 0;
	curwin->w_cursor.coladd = 0;

	ఒకవేళ (flags & (BL_WHITE | BL_SOL))
	{
	    char_u  *ptr;

	    కొరకు (ptr = ml_get_curline(); VIM_ISWHITE(*ptr)
			       && !((flags & BL_FIX) && ptr[1] == NUL); ++ptr)
		++curwin->w_cursor.నిలువు;
	}
	curwin->w_సమితి_curswant = true;
    }
    adjust_skipcol();
}

/*
 * oneright oneleft cursor_down cursor_up
 *
 * Move one char {right,left,down,up}.
 * Doesn't move onto the NUL past the end of the line, unless it is allowed.
 * Return OK when successful, FAIL when we hit a line of file boundary.
 */

    పూర్ణాంకం
oneright(శూన్య)
{
    char_u	*ptr;
    పూర్ణాంకం		l;

    ఒకవేళ (virtual_చురుకైన())
    {
	pos_T	prevpos = curwin->w_cursor;

	// Adjust for multi-wide char (excluding TAB)
	ptr = ml_get_cursor();
	coladvance(getviscol() + ((*ptr != TAB
					  && vim_isprintc((*mb_ptr2char)(ptr)))
		    ? ptr2cells(ptr) : 1));
	curwin->w_సమితి_curswant = true;
	// Return OK if the cursor moved, FAIL otherwise (at window edge).
	తిరిగి (prevpos.నిలువు != curwin->w_cursor.నిలువు
		    || prevpos.coladd != curwin->w_cursor.coladd) ? OK : FAIL;
    }

    ptr = ml_get_cursor();
    ఒకవేళ (*ptr == NUL)
	తిరిగి FAIL;	    // already at the very end

    ఒకవేళ (has_mbyte)
	l = (*mb_ptr2len)(ptr);
    లేకపోతే
	l = 1;

    // move "l" bytes right, but don't end up on the NUL, unless 'virtualedit'
    // contains "onemore".
    ఒకవేళ (ptr[l] == NUL && (get_ve_flags() & VE_ONEMORE) == 0)
	తిరిగి FAIL;
    curwin->w_cursor.నిలువు += l;

    curwin->w_సమితి_curswant = true;
    adjust_skipcol();
    తిరిగి OK;
}

    పూర్ణాంకం
oneleft(శూన్య)
{
    ఒకవేళ (virtual_చురుకైన())
    {
#నిర్వచితమైతే FEAT_LINEBREAK
	పూర్ణాంకం వెడల్పు;
#అయితే_ముగింపు
	పూర్ణాంకం v = getviscol();

	ఒకవేళ (v == 0)
	    తిరిగి FAIL;

#నిర్వచితమైతే FEAT_LINEBREAK
	// We might get stuck on 'showbreak', skip over it.
	వెడల్పు = 1;
	కొరకు (;;)
	{
	    coladvance(v - వెడల్పు);
	    // getviscol() is slow, skip it when 'showbreak' is empty,
	    // 'breakindent' is not set and there are no multi-byte
	    // characters
	    ఒకవేళ ((*get_showbreak_విలువ(curwin) == NUL && !curwin->w_p_bri
					     && !has_mbyte) || getviscol() < v)
		ఆపు;
	    ++వెడల్పు;
	}
#లేకపోతే
	coladvance(v - 1);
#అయితే_ముగింపు

	ఒకవేళ (curwin->w_cursor.coladd == 1)
	{
	    char_u *ptr;

	    // Adjust for multi-wide char (not a TAB)
	    ptr = ml_get_cursor();
	    ఒకవేళ (*ptr != TAB && vim_isprintc((*mb_ptr2char)(ptr))
							 && ptr2cells(ptr) > 1)
		curwin->w_cursor.coladd = 0;
	}

	curwin->w_సమితి_curswant = true;
	adjust_skipcol();
	తిరిగి OK;
    }

    ఒకవేళ (curwin->w_cursor.నిలువు == 0)
	తిరిగి FAIL;

    curwin->w_సమితి_curswant = true;
    --curwin->w_cursor.నిలువు;

    // if the character on the left of the current cursor is a multi-byte
    // character, move to its first byte
    ఒకవేళ (has_mbyte)
	mb_adjust_cursor();
    adjust_skipcol();
    తిరిగి OK;
}

/*
 * Move the cursor up "n" lines in window "wp".
 * Takes care of closed folds.
 */
    శూన్య
cursor_పైకి_inner(win_T *wp, దీర్ఘ n)
{
    linenr_T	lnum = wp->w_cursor.lnum;

    ఒకవేళ (n >= lnum)
	lnum = 1;
    లేకపోతే
#నిర్వచితమైతే FEAT_FOLDING
	ఒకవేళ (hasAnyFolding(wp))
    {
	/*
	 * Count each sequence of folded lines as one logical line.
	 */
	// go to the start of the current fold
	(శూన్య)hasFoldingWin(wp, lnum, &lnum, ఖాళీ, TRUE, ఖాళీ);

	అయితే (n--)
	{
	    // move up one line
	    --lnum;
	    ఒకవేళ (lnum <= 1)
		ఆపు;
	    // If we entered a fold, move to the beginning, unless in
	    // Insert mode or when 'foldopen' contains "all": it will open
	    // in a moment.
	    ఒకవేళ (n > 0 || !((State & బహుళకం_చొప్పించు) || (fdo_flags & FDO_ALL)))
		(శూన్య)hasFoldingWin(wp, lnum, &lnum, ఖాళీ, TRUE, ఖాళీ);
	}
	ఒకవేళ (lnum < 1)
	    lnum = 1;
    }
    లేకపోతే
#అయితే_ముగింపు
	lnum -= n;

    wp->w_cursor.lnum = lnum;
}

    పూర్ణాంకం
cursor_పైకి(
    దీర్ఘ	n,
    పూర్ణాంకం		upd_topline)	    // When TRUE: update topline
{
    // This fails if the cursor is already in the first line or the count is
    // larger than the line number and '-' is in 'cpoptions'
    linenr_T lnum = curwin->w_cursor.lnum;
    ఒకవేళ (n > 0 && (lnum <= 1
		       || (n >= lnum && vim_strchr(p_cpo, CPO_MINUS) != ఖాళీ)))
	తిరిగి FAIL;
    cursor_పైకి_inner(curwin, n);

    // try to advance to the column we want to be at
    coladvance(curwin->w_curswant);

    ఒకవేళ (upd_topline)
	update_topline();	// make sure curwin->w_topline is valid

    తిరిగి OK;
}

/*
 * Move the cursor down "n" lines in window "wp".
 * Takes care of closed folds.
 */
    శూన్య
cursor_కిందికి_inner(win_T *wp, దీర్ఘ n)
{
    linenr_T	lnum = wp->w_cursor.lnum;
    linenr_T	పంక్తి_లెక్క = wp->w_బఫరు->b_ml.ml_పంక్తి_లెక్క;

    ఒకవేళ (lnum + n >= పంక్తి_లెక్క)
	lnum = పంక్తి_లెక్క;
    లేకపోతే
#నిర్వచితమైతే FEAT_FOLDING
	ఒకవేళ (hasAnyFolding(wp))
    {
	linenr_T	చివరి;

	// count each sequence of folded lines as one logical line
	అయితే (n--)
	{
	    ఒకవేళ (hasFoldingWin(wp, lnum, ఖాళీ, &చివరి, TRUE, ఖాళీ))
		lnum = చివరి + 1;
	    లేకపోతే
		++lnum;
	    ఒకవేళ (lnum >= పంక్తి_లెక్క)
		ఆపు;
	}
	ఒకవేళ (lnum > పంక్తి_లెక్క)
	    lnum = పంక్తి_లెక్క;
    }
    లేకపోతే
#అయితే_ముగింపు
	lnum += n;

    wp->w_cursor.lnum = lnum;
}

/*
 * Cursor down a number of logical lines.
 */
    పూర్ణాంకం
cursor_కిందికి(
    దీర్ఘ	n,
    పూర్ణాంకం		upd_topline)	    // When TRUE: update topline
{
    linenr_T	lnum = curwin->w_cursor.lnum;
    linenr_T	పంక్తి_లెక్క = curwin->w_బఫరు->b_ml.ml_పంక్తి_లెక్క;
    // This fails if the cursor is already in the last (folded) line, or would
    // move beyond the last line and '-' is in 'cpoptions'.
#నిర్వచితమైతే FEAT_FOLDING
    hasFoldingWin(curwin, lnum, ఖాళీ, &lnum, TRUE, ఖాళీ);
#అయితే_ముగింపు
    ఒకవేళ (n > 0
	    && (lnum >= పంక్తి_లెక్క
		|| (lnum + n > పంక్తి_లెక్క
				     && vim_strchr(p_cpo, CPO_MINUS) != ఖాళీ)))
	తిరిగి FAIL;
    cursor_కిందికి_inner(curwin, n);

    // try to advance to the column we want to be at
    coladvance(curwin->w_curswant);

    ఒకవేళ (upd_topline)
	update_topline();	// make sure curwin->w_topline is valid

    తిరిగి OK;
}

/*
 * Stuff the last inserted text in the read buffer.
 * Last_insert actually is a copy of the redo buffer, so we
 * first have to remove the command.
 */
    పూర్ణాంకం
stuff_inserted(
    పూర్ణాంకం	    c,		// Command character to be inserted
    దీర్ఘ    లెక్క,	// Repeat this many times
    పూర్ణాంకం	    no_esc)	// Don't add an ESC at the end
{
    string_T	చొప్పించు;				// text to be inserted
    char_u	చివరి = ' ';

    చొప్పించు = get_చివరి_చొప్పించు();
    ఒకవేళ (చొప్పించు.string == ఖాళీ)
    {
	emsg(_(e_no_inserted_పాఠ్యం_yet));
	తిరిగి FAIL;
    }

    // may want to stuff the command character, to start Insert mode
    ఒకవేళ (c != NUL)
	stuffcharReadbuff(c);

    ఒకవేళ (చొప్పించు.నిడివి > 0)
    {
	char_u	*p;

	// look for the last ESC in 'insert'
	కొరకు (p = చొప్పించు.string + చొప్పించు.నిడివి - 1; p >= చొప్పించు.string; --p)
	{
	    ఒకవేళ (*p == ESC)
	    {
		చొప్పించు.నిడివి = (size_t)(p - చొప్పించు.string);
		ఆపు;
	    }
	}
    }

    ఒకవేళ (చొప్పించు.నిడివి > 0)
    {
	char_u	*p = చొప్పించు.string + చొప్పించు.నిడివి - 1;

	// when the last char is either "0" or "^" it will be quoted if no ESC
	// comes after it OR if it will insert more than once and "ptr"
	// starts with ^D.	-- Acevedo
	ఒకవేళ ((*p == '0' || *p == '^')
		&& (no_esc || (*చొప్పించు.string == Ctrl_D && లెక్క > 1)))
	{
	    చివరి = *p;
	    --చొప్పించు.నిడివి;
	}
    }

    చేయి
    {
	stuffReadbuffLen(చొప్పించు.string, (దీర్ఘ)చొప్పించు.నిడివి);
	// a trailing "0" is inserted as "<C-V>048", "^" as "<C-V>^"
	ఎంపిక (చివరి)
	{
	స్థితి '0':
#నిర్వచించు పాఠ్యం_గమ్యం_చొప్పించు "\026\060\064\070"
	    stuffReadbuffLen((char_u *)పాఠ్యం_గమ్యం_చొప్పించు, STRLEN_LITERAL(పాఠ్యం_గమ్యం_చొప్పించు));
#నిర్వచనం_రద్దు పాఠ్యం_గమ్యం_చొప్పించు
	    ఆపు;

	స్థితి '^':
#నిర్వచించు పాఠ్యం_గమ్యం_చొప్పించు "\026^"
	    stuffReadbuffLen((char_u *)పాఠ్యం_గమ్యం_చొప్పించు, STRLEN_LITERAL(పాఠ్యం_గమ్యం_చొప్పించు));
#నిర్వచనం_రద్దు పాఠ్యం_గమ్యం_చొప్పించు
	    ఆపు;

	అప్రమేయ:
	    ఆపు;
	}
    }
    అయితే (--లెక్క > 0);

    // may want to stuff a trailing ESC, to get out of Insert mode
    ఒకవేళ (!no_esc)
	stuffcharReadbuff(ESC);

    తిరిగి OK;
}

    string_T
get_చివరి_చొప్పించు(శూన్య)
{
    string_T చొప్పించు = {ఖాళీ, 0};

    ఒకవేళ (చివరి_చొప్పించు.string != ఖాళీ)
    {
	చొప్పించు.string = చివరి_చొప్పించు.string + చివరి_చొప్పించు_skip;
	చొప్పించు.నిడివి = (size_t)(చివరి_చొప్పించు.నిడివి - చివరి_చొప్పించు_skip);
    }

    తిరిగి చొప్పించు;
}

/*
 * Get last inserted string, and remove trailing <Esc>.
 * Returns pointer to allocated memory (must be freed) or NULL.
 */
    char_u *
get_చివరి_చొప్పించు_save(శూన్య)
{
    string_T	చొప్పించు = get_చివరి_చొప్పించు();
    char_u	*s;

    ఒకవేళ (చొప్పించు.string == ఖాళీ)
	తిరిగి ఖాళీ;
    s = vim_strnsave(చొప్పించు.string, చొప్పించు.నిడివి);
    ఒకవేళ (s == ఖాళీ)
	తిరిగి ఖాళీ;

    ఒకవేళ (చొప్పించు.నిడివి > 0 && s[చొప్పించు.నిడివి - 1] == ESC)	// remove trailing ESC
	s[--చొప్పించు.నిడివి] = NUL;
    తిరిగి s;
}

/*
 * Check the word in front of the cursor for an abbreviation.
 * Called when the non-id character "c" has been entered.
 * When an abbreviation is recognized it is removed from the text and
 * the replacement string is inserted in typebuf.tb_buf[], followed by "c".
 */
    స్థిర పూర్ణాంకం
echeck_abbr(పూర్ణాంకం c)
{
    // Don't check for abbreviation in paste mode, when disabled and just
    // after moving around with cursor keys.
    ఒకవేళ (p_paste || no_abbr || arrow_వాడిన)
	తిరిగి FALSE;

    తిరిగి check_abbr(c, ml_get_curline(), curwin->w_cursor.నిలువు,
		curwin->w_cursor.lnum == Insstart.lnum ? Insstart.నిలువు : 0);
}

/*
 * replace-stack functions
 *
 * When replacing characters, the replaced characters are remembered for each
 * new character.  This is used to re-insert the old text when backspacing.
 *
 * There is a NUL headed list of characters for each character that is
 * currently in the file after the insertion point.  When BS is used, one NUL
 * headed list is put back for the deleted character.
 *
 * For a newline, there are two NUL headed lists.  One contains the characters
 * that the NL replaced.  The extra one stores the characters after the cursor
 * that were deleted (always white space).
 *
 * Replace_offset is normally 0, in which case replace_push will add a new
 * character at the end of the stack.  If replace_offset is not 0, that many
 * characters will be left on the stack above the newly inserted character.
 */

స్థిర char_u	*replace_దొంతర = ఖాళీ;
స్థిర దీర్ఘ	replace_దొంతర_nr = 0;	    // next entry in replace stack
స్థిర దీర్ఘ	replace_దొంతర_len = 0;	    // max. number of entries

    శూన్య
replace_నెట్టు(
    పూర్ణాంకం	    c)	    // character that is replaced (NUL is none)
{
    char_u  *p;

    ఒకవేళ (replace_దొంతర_nr < replace_స్థానభ్రంశం)	// nothing to do
	తిరిగి;
    ఒకవేళ (replace_దొంతర_len <= replace_దొంతర_nr)
    {
	replace_దొంతర_len += 50;
	p = ALLOC_MULT(char_u, replace_దొంతర_len);
	ఒకవేళ (p == ఖాళీ)	    // out of memory
	{
	    replace_దొంతర_len -= 50;
	    తిరిగి;
	}
	ఒకవేళ (replace_దొంతర != ఖాళీ)
	{
	    mch_memmove(p, replace_దొంతర,
				 (size_t)(replace_దొంతర_nr * పరిమాణం(char_u)));
	    vim_free(replace_దొంతర);
	}
	replace_దొంతర = p;
    }
    p = replace_దొంతర + replace_దొంతర_nr - replace_స్థానభ్రంశం;
    ఒకవేళ (replace_స్థానభ్రంశం)
	mch_memmove(p + 1, p, (size_t)(replace_స్థానభ్రంశం * పరిమాణం(char_u)));
    *p = c;
    ++replace_దొంతర_nr;
}

/*
 * Push a character onto the replace stack.  Handles a multi-byte character in
 * reverse byte order, so that the first byte is popped off first.
 * Return the number of bytes done (includes composing characters).
 */
    పూర్ణాంకం
replace_నెట్టు_mb(char_u *p)
{
    పూర్ణాంకం l = (*mb_ptr2len)(p);
    పూర్ణాంకం j;

    కొరకు (j = l - 1; j >= 0; --j)
	replace_నెట్టు(p[j]);
    తిరిగి l;
}

/*
 * Pop one item from the replace stack.
 * return -1 if stack empty
 * return replaced character or NUL otherwise
 */
    స్థిర పూర్ణాంకం
replace_తీయి(శూన్య)
{
    ఒకవేళ (replace_దొంతర_nr == 0)
	తిరిగి -1;
    తిరిగి (పూర్ణాంకం)replace_దొంతర[--replace_దొంతర_nr];
}

/*
 * Join the top two items on the replace stack.  This removes to "off"'th NUL
 * encountered.
 */
    శూన్య
replace_join(
    పూర్ణాంకం	    off)	// offset for which NUL to remove
{
    పూర్ణాంకం	    i;

    కొరకు (i = replace_దొంతర_nr; --i >= 0; )
	ఒకవేళ (replace_దొంతర[i] == NUL && off-- <= 0)
	{
	    --replace_దొంతర_nr;
	    mch_memmove(replace_దొంతర + i, replace_దొంతర + i + 1,
					      (size_t)(replace_దొంతర_nr - i));
	    తిరిగి;
	}
}

/*
 * Pop bytes from the replace stack until a NUL is found, and insert them
 * before the cursor.  Can only be used in MODE_REPLACE or MODE_VREPLACE state.
 */
    స్థిర శూన్య
replace_తీయి_ins(శూన్య)
{
    పూర్ణాంకం	    cc;
    పూర్ణాంకం	    oldState = State;

    State = బహుళకం_సాధారణ;			// don't want MODE_REPLACE here
    అయితే ((cc = replace_తీయి()) > 0)
    {
	mb_replace_తీయి_ins(cc);
	dec_cursor();
    }
    State = oldState;
}

/*
 * Insert bytes popped from the replace stack. "cc" is the first byte.  If it
 * indicates a multi-byte char, pop the other bytes too.
 */
    స్థిర శూన్య
mb_replace_తీయి_ins(పూర్ణాంకం cc)
{
    పూర్ణాంకం		n;
    char_u	buf[MB_MAXBYTES + 1];
    పూర్ణాంకం		i;
    పూర్ణాంకం		c;

    ఒకవేళ (has_mbyte && (n = MB_BYTE2LEN(cc)) > 1)
    {
	buf[0] = cc;
	కొరకు (i = 1; i < n; ++i)
	    buf[i] = replace_తీయి();
	ins_బైట్లు_len(buf, n);
    }
    లేకపోతే
	ins_char(cc);

    ఒకవేళ (enc_utf8)
	// Handle composing chars.
	కొరకు (;;)
	{
	    c = replace_తీయి();
	    ఒకవేళ (c == -1)	    // stack empty
		ఆపు;
	    ఒకవేళ ((n = MB_BYTE2LEN(c)) == 1)
	    {
		// Not a multi-byte char, put it back.
		replace_నెట్టు(c);
		ఆపు;
	    }

	    buf[0] = c;
	    కొరకు (i = 1; i < n; ++i)
		buf[i] = replace_తీయి();
	    ఒకవేళ (utf_iscomposing(utf_ptr2char(buf)))
		ins_బైట్లు_len(buf, n);
	    లేకపోతే
	    {
		// Not a composing char, put it back.
		కొరకు (i = n - 1; i >= 0; --i)
		    replace_నెట్టు(buf[i]);
		ఆపు;
	    }

	}
}

/*
 * make the replace stack empty
 * (called when exiting replace mode)
 */
    స్థిర శూన్య
replace_flush(శూన్య)
{
    VIM_CLEAR(replace_దొంతర);
    replace_దొంతర_len = 0;
    replace_దొంతర_nr = 0;
}

/*
 * Handle doing a BS for one character.
 * cc < 0: replace stack empty, just move cursor
 * cc == 0: character was inserted, delete it
 * cc > 0: character was replaced, put cc (first byte of original char) back
 * and check for more characters to be put back
 * When "limit_col" is >= 0, don't delete before this column.  Matters when
 * using composing characters, use del_char_after_col() instead of del_char().
 */
    స్థిర శూన్య
replace_do_bs(పూర్ణాంకం హద్దు_నిలువు)
{
    పూర్ణాంకం		cc;
    పూర్ణాంకం		orig_len = 0;
    పూర్ణాంకం		ins_len;
    పూర్ణాంకం		orig_vcols = 0;
    colnr_T	ప్రారంభం_vcol;
    char_u	*p;
    పూర్ణాంకం		i;
    పూర్ణాంకం		vcol;

    cc = replace_తీయి();
    ఒకవేళ (cc > 0)
    {
#నిర్వచితమైతే FEAT_PROP_POPUP
	size_t	len_before = 0;  // init to shut up GCC

	ఒకవేళ (curbuf->b_has_textprop)
	{
	    // Do not adjust text properties for individual delete and insert
	    // operations, do it afterwards on the resulting text.
	    len_before = ml_get_curline_len();
	    ++పాఠ్యం_prop_frozen;
	}
#అయితే_ముగింపు
	ఒకవేళ (State & VREPLACE_FLAG)
	{
	    // Get the number of screen cells used by the character we are
	    // going to delete.
	    getvcol(curwin, &curwin->w_cursor, ఖాళీ, &ప్రారంభం_vcol, ఖాళీ, 0);
	    orig_vcols = chartabsize(ml_get_cursor(), ప్రారంభం_vcol);
	}
	ఒకవేళ (has_mbyte)
	{
	    (శూన్య)del_char_after_నిలువు(హద్దు_నిలువు);
	    ఒకవేళ (State & VREPLACE_FLAG)
		orig_len = ml_get_cursor_len();
	    replace_నెట్టు(cc);
	}
	లేకపోతే
	{
	    pchar_cursor(cc);
	    ఒకవేళ (State & VREPLACE_FLAG)
		orig_len = ml_get_cursor_len() - 1;
	}
	replace_తీయి_ins();

	ఒకవేళ (State & VREPLACE_FLAG)
	{
	    // Get the number of screen cells used by the inserted characters
	    p = ml_get_cursor();
	    ins_len = ml_get_cursor_len() - orig_len;
	    vcol = ప్రారంభం_vcol;
	    కొరకు (i = 0; i < ins_len; ++i)
	    {
		vcol += chartabsize(p + i, vcol);
		i += (*mb_ptr2len)(p + i) - 1;
	    }
	    vcol -= ప్రారంభం_vcol;

	    // Delete spaces that were inserted after the cursor to keep the
	    // text aligned.
	    curwin->w_cursor.నిలువు += ins_len;
	    అయితే (vcol > orig_vcols && gchar_cursor() == ' ')
	    {
		del_char(FALSE);
		++orig_vcols;
	    }
	    curwin->w_cursor.నిలువు -= ins_len;
	}

	// mark the buffer as changed and prepare for displaying
	మారింది_బైట్లు(curwin->w_cursor.lnum, curwin->w_cursor.నిలువు);

#నిర్వచితమైతే FEAT_PROP_POPUP
	ఒకవేళ (curbuf->b_has_textprop)
	{
	    size_t len_now = ml_get_curline_len();

	    --పాఠ్యం_prop_frozen;
	    adjust_prop_columns(curwin->w_cursor.lnum, curwin->w_cursor.నిలువు,
					   (పూర్ణాంకం)(len_now - len_before), 0);
	}
#అయితే_ముగింపు
    }
    లేకపోతే ఒకవేళ (cc == 0)
	(శూన్య)del_char_after_నిలువు(హద్దు_నిలువు);
}

#ఒకవేళ defined(FEAT_RIGHTLEFT)
/*
 * Map Hebrew keyboard when in hkmap mode.
 */
    పూర్ణాంకం
hkmap(పూర్ణాంకం c)
{
    ఒకవేళ (p_hkmapp)   // phonetic mapping, by Ilya Dogolazky
    {
	గణన {hALEF=0, BET, GIMEL, DALET, HEI, VAV, ZAIN, HET, TET, IUD,
	    KAFsofit, hKAF, LAMED, MEMsofit, MEM, NUNsofit, NUN, SAMEH, AIN,
	    PEIsofit, PEI, ZADIsofit, ZADI, KOF, RESH, hSHIN, TAV};
	స్థిర char_u map[26] =
	    {(char_u)hALEF/*a*/, (char_u)BET  /*b*/, (char_u)hKAF    /*c*/,
	     (char_u)DALET/*d*/, (char_u)-1   /*e*/, (char_u)PEIsofit/*f*/,
	     (char_u)GIMEL/*g*/, (char_u)HEI  /*h*/, (char_u)IUD     /*i*/,
	     (char_u)HET  /*j*/, (char_u)KOF  /*k*/, (char_u)LAMED   /*l*/,
	     (char_u)MEM  /*m*/, (char_u)NUN  /*n*/, (char_u)SAMEH   /*o*/,
	     (char_u)PEI  /*p*/, (char_u)-1   /*q*/, (char_u)RESH    /*r*/,
	     (char_u)ZAIN /*s*/, (char_u)TAV  /*t*/, (char_u)TET     /*u*/,
	     (char_u)VAV  /*v*/, (char_u)hSHIN/*w*/, (char_u)-1      /*x*/,
	     (char_u)AIN  /*y*/, (char_u)ZADI /*z*/};

	ఒకవేళ (c == 'N' || c == 'M' || c == 'P' || c == 'C' || c == 'Z')
	    తిరిగి (పూర్ణాంకం)(map[CharOrd(c)] - 1 + p_aleph);
							    // '-1'='sofit'
	లేకపోతే ఒకవేళ (c == 'x')
	    తిరిగి 'X';
	లేకపోతే ఒకవేళ (c == 'q')
	    తిరిగి '\''; // {geresh}={'}
	లేకపోతే ఒకవేళ (c == 246)
	    తిరిగి ' ';  // \"o --> ' ' for a german keyboard
	లేకపోతే ఒకవేళ (c == 228)
	    తిరిగి ' ';  // \"a --> ' '      -- / --
	లేకపోతే ఒకవేళ (c == 252)
	    తిరిగి ' ';  // \"u --> ' '      -- / --
	// NOTE: islower() does not do the right thing for us on Linux so we
	// do this the same was as 5.7 and previous, so it works correctly on
	// all systems.  Specifically, the e.g. Delete and Arrow keys are
	// munged and won't work if e.g. searching for Hebrew text.
	లేకపోతే ఒకవేళ (c >= 'a' && c <= 'z')
	    తిరిగి (పూర్ణాంకం)(map[Char_Ord_దిగువ(c)] + p_aleph);
	లేకపోతే
	    తిరిగి c;
    }
    లేకపోతే
    {
	ఎంపిక (c)
	{
	    స్థితి '`':	తిరిగి ';';
	    స్థితి '/':	తిరిగి '.';
	    స్థితి '\'':	తిరిగి ',';
	    స్థితి 'q':	తిరిగి '/';
	    స్థితి 'w':	తిరిగి '\'';

			// Hebrew letters - set offset from 'a'
	    స్థితి ',':	c = '{'; ఆపు;
	    స్థితి '.':	c = 'v'; ఆపు;
	    స్థితి ';':	c = 't'; ఆపు;
	    అప్రమేయ: {
			 స్థిర అక్షరం str[] = "zqbcxlsjphmkwonu ydafe rig";

			 ఒకవేళ (c < 'a' || c > 'z')
			     తిరిగి c;
			 c = str[Char_Ord_దిగువ(c)];
			 ఆపు;
		     }
	}

	తిరిగి (పూర్ణాంకం)(Char_Ord_దిగువ(c) + p_aleph);
    }
}
#అయితే_ముగింపు

    స్థిర శూన్య
ins_reg(శూన్య)
{
    పూర్ణాంకం		need_redraw = FALSE;
    పూర్ణాంకం		regname;
    పూర్ణాంకం		literally = 0;
    పూర్ణాంకం		vis_చురుకైన = VIsual_చురుకైన;

    /*
     * If we are going to wait for a character, show a '"'.
     */
    pc_స్థితి = PC_స్థితి_UNSET;
    ఒకవేళ (redrawing() && !char_avail())
    {
	// may need to redraw when no more chars available now
	ins_redraw(FALSE);

	edit_putchar('"', TRUE);
	కూడు_గమ్యం_showcmd_c(Ctrl_R);
    }

#నిర్వచితమైతే USE_ON_FLY_SCROLL
    dont_scroll = TRUE;		// disallow scrolling here
#అయితే_ముగింపు

    /*
     * Don't map the register name. This also prevents the mode message to be
     * deleted when ESC is hit.
     */
    ++no_mapping;
    ++allow_కీలకాలు;
    regname = plain_vgetc();
    LANGMAP_ADJUST(regname, TRUE);
    ఒకవేళ (regname == Ctrl_R || regname == Ctrl_O || regname == Ctrl_P)
    {
	// Get a third key for literal register insertion
	literally = regname;
	కూడు_గమ్యం_showcmd_c(literally);
	regname = plain_vgetc();
	LANGMAP_ADJUST(regname, TRUE);
    }
    --no_mapping;
    --allow_కీలకాలు;

#నిర్వచితమైతే FEAT_EVAL
    // Don't call u_sync() while typing the expression or giving an error
    // message for it. Only call it explicitly.
    ++no_u_sync;
    ఒకవేళ (regname == '=')
    {
	pos_T	curpos = curwin->w_cursor;
# ifdef HAVE_ఆదానం_పద్ధతి
	పూర్ణాంకం	im_on = im_get_స్థితి();
# endif
	// Sync undo when evaluating the expression calls setline() or
	// append(), so that it can be undone separately.
	u_sync_once = 2;

	regname = get_సమాసం_రిజిస్టరు();

	// Cursor may be moved back a column.
	curwin->w_cursor = curpos;
	check_cursor();
# ifdef HAVE_ఆదానం_పద్ధతి
	// Restore the Input Method.
	ఒకవేళ (im_on)
	    im_సమితి_చురుకైన(TRUE);
# endif
    }
    ఒకవేళ (regname == NUL || !valid_yank_reg(regname, FALSE))
    {
	vim_beep(BO_REG);
	need_redraw = TRUE;	// remove the '"'
    }
    లేకపోతే
    {
#అయితే_ముగింపు
	ఒకవేళ (literally == Ctrl_O || literally == Ctrl_P)
	{
	    // Append the command to the redo buffer.
	    Append_Char_గమ్యం_Redobuff(Ctrl_R);
	    Append_Char_గమ్యం_Redobuff(literally);
	    Append_Char_గమ్యం_Redobuff(regname);

	    do_put(regname, ఖాళీ, BACKWARD, 1L,
		 (literally == Ctrl_P ? PUT_FIXINDENT : 0) | PUT_CURSEND);
	}
	లేకపోతే ఒకవేళ (చొప్పించు_reg(regname, literally) == FAIL)
	{
	    vim_beep(BO_REG);
	    need_redraw = TRUE;	// remove the '"'
	}
	లేకపోతే ఒకవేళ (stop_చొప్పించు_బహుళకం)
	    // When the '=' register was used and a function was invoked that
	    // did ":stopinsert" then stuff_empty() returns FALSE but we won't
	    // insert anything, need to remove the '"'
	    need_redraw = TRUE;

#నిర్వచితమైతే FEAT_EVAL
    }
    --no_u_sync;
    ఒకవేళ (u_sync_once == 1)
	ins_need_undo = TRUE;
    u_sync_once = 0;
#అయితే_ముగింపు
    clear_showcmd();

    // If the inserted register is empty, we need to remove the '"'
    ఒకవేళ (need_redraw || stuff_ఖాళీలు())
	edit_unputchar();

    // Disallow starting Visual mode here, would get a weird mode.
    ఒకవేళ (!vis_చురుకైన && VIsual_చురుకైన)
	చివర_visual_బహుళకం();
}

/*
 * CTRL-G commands in Insert mode.
 */
    స్థిర శూన్య
ins_ctrl_g(శూన్య)
{
    పూర్ణాంకం		c;

    // Right after CTRL-X the cursor will be after the ruler.
    setcursor();

    /*
     * Don't map the second key. This also prevents the mode message to be
     * deleted when ESC is hit.
     */
    ++no_mapping;
    ++allow_కీలకాలు;
    c = plain_vgetc();
    --no_mapping;
    --allow_కీలకాలు;
    ఎంపిక (c)
    {
	// CTRL-G k and CTRL-G <Up>: cursor up to Insstart.col
	స్థితి K_పైకి:
	స్థితి Ctrl_K:
	స్థితి 'k': ins_పైకి(TRUE);
		  ఆపు;

	// CTRL-G j and CTRL-G <Down>: cursor down to Insstart.col
	స్థితి K_కిందికి:
	స్థితి Ctrl_J:
	స్థితి 'j': ins_కిందికి(TRUE);
		  ఆపు;

	// CTRL-G u: start new undoable edit
	స్థితి 'u': u_sync(TRUE);
		  ins_need_undo = TRUE;

		  // Need to reset Insstart, esp. because a BS that joins
		  // a line to the previous one must save for undo.
		  update_Insstart_orig = FALSE;
		  Insstart = curwin->w_cursor;
		  ఆపు;

	// CTRL-G U: do not break undo with the next char
	స్థితి 'U':
		  // Allow one left/right cursor movement with the next char,
		  // without breaking undo.
		  dont_sync_undo = MAYBE;
		  ఆపు;

	స్థితి ESC:
		  // Esc after CTRL-G cancels it.
		  ఆపు;

	// Unknown CTRL-G command, reserved for future expansion.
	అప్రమేయ:  vim_beep(BO_CTRLG);
    }
}

/*
 * CTRL-^ in Insert mode.
 */
    స్థిర శూన్య
ins_ctrl_hat(శూన్య)
{
    ఒకవేళ (map_గమ్యం_exists_బహుళకం((char_u *)"", బహుళకం_LANGMAP, FALSE))
    {
	// ":lmap" mappings exists, Toggle use of ":lmap" mappings.
	ఒకవేళ (State & బహుళకం_LANGMAP)
	{
	    curbuf->b_p_iminsert = B_IMODE_NONE;
	    State &= ~బహుళకం_LANGMAP;
	}
	లేకపోతే
	{
	    curbuf->b_p_iminsert = B_IMODE_LMAP;
	    State |= బహుళకం_LANGMAP;
#నిర్వచితమైతే HAVE_ఆదానం_పద్ధతి
	    im_సమితి_చురుకైన(FALSE);
#అయితే_ముగింపు
	}
    }
#నిర్వచితమైతే HAVE_ఆదానం_పద్ధతి
    లేకపోతే
    {
	// There are no ":lmap" mappings, toggle IM
	ఒకవేళ (im_get_స్థితి())
	{
	    curbuf->b_p_iminsert = B_IMODE_NONE;
	    im_సమితి_చురుకైన(FALSE);
	}
	లేకపోతే
	{
	    curbuf->b_p_iminsert = B_IMODE_IM;
	    State &= ~బహుళకం_LANGMAP;
	    im_సమితి_చురుకైన(TRUE);
	}
    }
#అయితే_ముగింపు
    సమితి_iminsert_global();
    showmode();
#నిర్వచితమైతే FEAT_GUI
    // may show different cursor shape or color
    ఒకవేళ (gui.లోపల_use)
	gui_update_cursor(TRUE, FALSE);
#అయితే_ముగింపు
#ఒకవేళ defined(FEAT_KEYMAP)
    // Show/unshow value of 'keymap' in status lines.
    స్థితి_redraw_curbuf();
#అయితే_ముగింపు
}

/*
 * Handle ESC in insert mode.
 * Returns TRUE when leaving insert mode, FALSE when going to repeat the
 * insert.
 */
    స్థిర పూర్ణాంకం
ins_esc(
    దీర్ఘ	*లెక్క,
    పూర్ణాంకం		cmdchar,
    పూర్ణాంకం		nomove)	    // don't move cursor
{
    పూర్ణాంకం		తాత్కాలిక;
    స్థిర పూర్ణాంకం	disabled_redraw = FALSE;
#నిర్వచితమైతే FEAT_CONCEAL
    // Remember if the cursor line was concealed before changing State.
    పూర్ణాంకం		cursor_పంక్తి_was_concealed = curwin->w_p_cole > 0
						&& conceal_cursor_పంక్తి(curwin);
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_SPELL
    check_spell_redraw();
#అయితే_ముగింపు

    తాత్కాలిక = curwin->w_cursor.నిలువు;
    ఒకవేళ (disabled_redraw)
    {
	ఒకవేళ (RedrawingDisabled > 0)
	    --RedrawingDisabled;
	disabled_redraw = FALSE;
    }
    ఒకవేళ (!arrow_వాడిన)
    {
	/*
	 * Don't append the ESC for "r<CR>" and "grx".
	 * When 'insertmode' is set only CTRL-L stops Insert mode.  Needed for
	 * when "count" is non-zero.
	 */
	ఒకవేళ (cmdchar != 'r' && cmdchar != 'v')
	    Append_గమ్యం_Redobuff(p_im ? (char_u *)"\014" : ESC_STR);

	/*
	 * Repeating insert may take a long time.  Check for
	 * interrupt now and then.
	 */
	ఒకవేళ (*లెక్క > 0)
	{
	    పంక్తి_breakcheck();
	    ఒకవేళ (got_int)
		*లెక్క = 0;
	}

	ఒకవేళ (--*లెక్క > 0)	// repeat what was typed
	{
	    // Vi repeats the insert without replacing characters.
	    ఒకవేళ (vim_strchr(p_cpo, CPO_REPLCNT) != ఖాళీ)
		State &= ~REPLACE_FLAG;

	    (శూన్య)ప్రారంభం_redo_ins();
	    ఒకవేళ (cmdchar == 'r' || cmdchar == 'v')
		stuffRedoReadbuff(ESC_STR);	// no ESC in redo buffer
	    ++RedrawingDisabled;
	    disabled_redraw = TRUE;
	    తిరిగి FALSE;	// repeat the insert
	}
	stop_చొప్పించు(&curwin->w_cursor, TRUE, nomove);
	undisplay_dollar();
    }

    ఒకవేళ (cmdchar != 'r' && cmdchar != 'v')
	ins_apply_autocmds(EVENT_INSERTLEAVEPRE);

    // When an autoindent was removed, curswant stays after the
    // indent
    ఒకవేళ (restart_edit == NUL && (colnr_T)తాత్కాలిక == curwin->w_cursor.నిలువు)
	curwin->w_సమితి_curswant = true;

    // Remember the last Insert position in the '^ mark.
    ఒకవేళ ((cmdmod.cmod_flags & CMOD_KEEPJUMPS) == 0)
	curbuf->b_చివరి_చొప్పించు = curwin->w_cursor;

    /*
     * The cursor should end up on the last inserted character.
     * Don't do it for CTRL-O, unless past the end of the line.
     */
    ఒకవేళ (!nomove
	    && (curwin->w_cursor.నిలువు != 0
		|| curwin->w_cursor.coladd > 0)
	    && (restart_edit == NUL
		   || (gchar_cursor() == NUL && !VIsual_చురుకైన))
#నిర్వచితమైతే FEAT_RIGHTLEFT
	    && !revins_on
#అయితే_ముగింపు
				      )
    {
	ఒకవేళ (curwin->w_cursor.coladd > 0 || get_ve_flags() == VE_ALL)
	{
	    oneleft();
	    ఒకవేళ (restart_edit != NUL)
		++curwin->w_cursor.coladd;
	}
	లేకపోతే
	{
	    --curwin->w_cursor.నిలువు;
	    curwin->w_valid &= ~(VALID_WCOL|VALID_VIRTCOL);
	    // Correct cursor for multi-byte character.
	    ఒకవేళ (has_mbyte)
		mb_adjust_cursor();
	}
    }

#నిర్వచితమైతే HAVE_ఆదానం_పద్ధతి
    // Disable IM to allow typing English directly for Normal mode commands.
    // When ":lmap" is enabled don't change 'iminsert' (IM can be enabled as
    // well).
    ఒకవేళ (!(State & బహుళకం_LANGMAP))
	im_save_స్థితి(&curbuf->b_p_iminsert);
    im_సమితి_చురుకైన(FALSE);
#అయితే_ముగింపు

    State = బహుళకం_సాధారణ;
    may_trigger_modechanged();
    // need to position cursor again when on a TAB and when on a char with
    // virtual text.
    ఒకవేళ (gchar_cursor() == TAB
#నిర్వచితమైతే FEAT_PROP_POPUP
	    || curbuf->b_has_textprop
#అయితే_ముగింపు
       )
	curwin->w_valid &= ~(VALID_WROW|VALID_WCOL|VALID_VIRTCOL);

    setmouse();
#నిర్వచితమైతే CURSOR_SHAPE
    ui_cursor_shape();		// may show different cursor shape
#అయితే_ముగింపు
    ఒకవేళ (!p_ek)
    {
	MAY_WANT_గమ్యం_LOG_THIS;

	// Re-enable bracketed paste mode.
	బహిర్గతం_str_t_BE();

	// Re-enable modifyOtherKeys.
	బహిర్గతం_str_t_TI();
    }
#నిర్వచితమైతే FEAT_CONCEAL
    // Check if the cursor line needs redrawing after changing State.  If
    // 'concealcursor' is "i" it needs to be redrawn without concealing.
    conceal_check_cursor_పంక్తి(cursor_పంక్తి_was_concealed);
#అయితే_ముగింపు

    // When recording or for CTRL-O, need to display the new mode.
    // Otherwise remove the mode message.
    ఒకవేళ (reg_recording != 0 || restart_edit != NUL)
	showmode();
    లేకపోతే ఒకవేళ (p_smd && (got_int || !skip_showmode()))
	సందేశం("");

    తిరిగి TRUE;	    // exit Insert mode
}

#నిర్వచితమైతే FEAT_RIGHTLEFT
/*
 * Toggle language: hkmap and revins_on.
 * Move to end of reverse inserted text.
 */
    స్థిర శూన్య
ins_ctrl_(శూన్య)
{
    ఒకవేళ (revins_on && revins_chars && revins_scol >= 0)
    {
	అయితే (gchar_cursor() != NUL && revins_chars--)
	    ++curwin->w_cursor.నిలువు;
    }
    p_ri = !p_ri;
    revins_on = (State == బహుళకం_చొప్పించు && p_ri);
    ఒకవేళ (revins_on)
    {
	revins_scol = curwin->w_cursor.నిలువు;
	revins_legal++;
	revins_chars = 0;
	undisplay_dollar();
    }
    లేకపోతే
	revins_scol = -1;
    p_hkmap = curwin->w_p_rl ^ p_ri;    // be consistent!
    showmode();
}
#అయితే_ముగింపు

/*
 * If 'keymodel' contains "startsel", may start selection.
 * Returns TRUE when a CTRL-O and other keys stuffed.
 */
    స్థిర పూర్ణాంకం
ins_ప్రారంభం_select(పూర్ణాంకం c)
{
    ఒకవేళ (!km_startsel)
	తిరిగి FALSE;
    ఎంపిక (c)
    {
	స్థితి K_KHOME:
	స్థితి K_KEND:
	స్థితి K_PAGEUP:
	స్థితి K_KPAGEUP:
	స్థితి K_PAGEDOWN:
	స్థితి K_KPAGEDOWN:
#నిర్వచితమైతే MACOS_X
	స్థితి K_ఎడమ:
	స్థితి K_కుడి:
	స్థితి K_పైకి:
	స్థితి K_కిందికి:
	స్థితి K_చివర:
	స్థితి K_HOME:
#అయితే_ముగింపు
	    ఒకవేళ (!(mod_ముసుగు & MOD_ముసుగు_జరుపు))
		ఆపు;
	    // FALLTHROUGH
	స్థితి K_S_ఎడమ:
	స్థితి K_S_కుడి:
	స్థితి K_S_పైకి:
	స్థితి K_S_కిందికి:
	స్థితి K_S_చివర:
	స్థితి K_S_HOME:
	    // Start selection right away, the cursor can move with CTRL-O when
	    // beyond the end of the line.
	    ప్రారంభం_selection();

	    // Execute the key in (insert) Select mode.
	    stuffcharReadbuff(Ctrl_O);
	    ఒకవేళ (mod_ముసుగు)
	    {
		char_u	    buf[4];

		buf[0] = K_SPECIAL;
		buf[1] = KS_MODIFIER;
		buf[2] = mod_ముసుగు;
		buf[3] = NUL;
		stuffReadbuffLen(buf, 3L);
	    }
	    stuffcharReadbuff(c);
	    తిరిగి TRUE;
    }
    తిరిగి FALSE;
}

/*
 * <Insert> key in Insert mode: toggle insert/replace mode.
 */
    స్థిర శూన్య
ins_చొప్పించు(పూర్ణాంకం replaceState)
{
#నిర్వచితమైతే FEAT_EVAL
    సమితి_vim_var_string(VV_INSERTMODE,
		   (char_u *)((State & REPLACE_FLAG) ? "i"
			    : replaceState == బహుళకం_VREPLACE ? "v" : "r"), 1);
#అయితే_ముగింపు
    ins_apply_autocmds(EVENT_INSERTCHANGE);
    ఒకవేళ (State & REPLACE_FLAG)
	State = బహుళకం_చొప్పించు | (State & బహుళకం_LANGMAP);
    లేకపోతే
	State = replaceState | (State & బహుళకం_LANGMAP);
    may_trigger_modechanged();
    Append_Char_గమ్యం_Redobuff(K_INS);
    showmode();
#నిర్వచితమైతే CURSOR_SHAPE
    ui_cursor_shape();		// may show different cursor shape
#అయితే_ముగింపు
}

/*
 * Pressed CTRL-O in Insert mode.
 */
    స్థిర శూన్య
ins_ctrl_o(శూన్య)
{
    restart_VIsual_select = 0;
    ఒకవేళ (State & VREPLACE_FLAG)
	restart_edit = 'V';
    లేకపోతే ఒకవేళ (State & REPLACE_FLAG)
	restart_edit = 'R';
    లేకపోతే
	restart_edit = 'I';
    ఒకవేళ (virtual_చురుకైన())
	ins_at_eol = FALSE;	// cursor always keeps its column
    లేకపోతే
	ins_at_eol = (gchar_cursor() == NUL);
}

/*
 * If the cursor is on an indent, ^T/^D insert/delete one
 * shiftwidth.	Otherwise ^T/^D behave like a "<<" or ">>".
 * Always round the indent to 'shiftwidth', this is compatible
 * with vi.  But vi only supports ^T and ^D after an
 * autoindent, we support it everywhere.
 */
    స్థిర శూన్య
ins_జరుపు(పూర్ణాంకం c, పూర్ణాంకం lastc)
{
    ఒకవేళ (stop_arrow() == FAIL)
	తిరిగి;
    Append_Char_గమ్యం_Redobuff(c);

    /*
     * 0^D and ^^D: remove all indent.
     */
    ఒకవేళ (c == Ctrl_D && (lastc == '0' || lastc == '^')
						  && curwin->w_cursor.నిలువు > 0)
    {
	--curwin->w_cursor.నిలువు;
	(శూన్య)del_char(FALSE);		// delete the '^' or '0'
	// In Replace mode, restore the characters that '^' or '0' replaced.
	ఒకవేళ (State & REPLACE_FLAG)
	    replace_తీయి_ins();
	ఒకవేళ (lastc == '^')
	    old_indent = get_indent();	// remember curr. indent
	change_indent(INDENT_సమితి, 0, TRUE, 0, TRUE);
    }
    లేకపోతే
	change_indent(c == Ctrl_D ? INDENT_DEC : INDENT_INC, 0, TRUE, 0, TRUE);

    ఒకవేళ (did_ai && *skipwhite(ml_get_curline()) != NUL)
	did_ai = FALSE;
    did_si = FALSE;
    can_si = FALSE;
    can_si_వెనుక = FALSE;
    can_cindent = FALSE;	// no cindenting after ^D or ^T
}

    స్థిర శూన్య
ins_del(శూన్య)
{
    పూర్ణాంకం	    తాత్కాలిక;

    ఒకవేళ (stop_arrow() == FAIL)
	తిరిగి;
    ఒకవేళ (gchar_cursor() == NUL)		// delete newline
    {
	తాత్కాలిక = curwin->w_cursor.నిలువు;
	ఒకవేళ (!can_bs(BS_EOL)		// only if "eol" included
		|| do_join(2, FALSE, TRUE, FALSE, FALSE) == FAIL)
	    vim_beep(BO_BS);
	లేకపోతే
	{
	    curwin->w_cursor.నిలువు = తాత్కాలిక;
	    // Adjust orig_line_count in case more lines have been deleted than
	    // have been added. That makes sure, that open_line() later
	    // can access all buffer lines correctly
	    ఒకవేళ (State & VREPLACE_FLAG &&
		    orig_పంక్తి_లెక్క > curbuf->b_ml.ml_పంక్తి_లెక్క)
		orig_పంక్తి_లెక్క = curbuf->b_ml.ml_పంక్తి_లెక్క;
	}
    }
    లేకపోతే ఒకవేళ (del_char(FALSE) == FAIL)  // delete char under cursor
	vim_beep(BO_BS);
    did_ai = FALSE;
    did_si = FALSE;
    can_si = FALSE;
    can_si_వెనుక = FALSE;
    Append_Char_గమ్యం_Redobuff(K_DEL);
}

/*
 * Delete one character for ins_bs().
 */
    స్థిర శూన్య
ins_bs_one(శూన్య)
{
    dec_cursor();
    ఒకవేళ (State & REPLACE_FLAG)
    {
	// Don't delete characters before the insert point when in
	// Replace mode
	ఒకవేళ (curwin->w_cursor.lnum != Insstart.lnum
		|| curwin->w_cursor.నిలువు >= Insstart.నిలువు)
	    replace_do_bs(-1);
    }
    లేకపోతే
	(శూన్య)del_char(FALSE);
}

/*
 * Handle Backspace, delete-word and delete-line in Insert mode.
 * Return TRUE when backspace was actually used.
 */
    స్థిర పూర్ణాంకం
ins_bs(
    పూర్ణాంకం		c,
    పూర్ణాంకం		బహుళకం,
    పూర్ణాంకం		*inserted_space_p)
{
    linenr_T	lnum;
    పూర్ణాంకం		cc;
    పూర్ణాంకం		తాత్కాలిక = 0;	    // init for GCC
    colnr_T	save_నిలువు;
    colnr_T	mincol;
    పూర్ణాంకం		did_backspace = FALSE;
    పూర్ణాంకం		లోపల_indent;
    పూర్ణాంకం		oldState;
    పూర్ణాంకం		cpc[MAX_MCO];	    // composing characters
    పూర్ణాంకం		call_fix_indent = FALSE;

    /*
     * can't delete anything in an empty file
     * can't backup past first character in buffer
     * can't backup past starting point unless 'backspace' > 1
     * can backup to a previous line if 'backspace' == 0
     */
    ఒకవేళ (       BUFEMPTY()
	    || (
#నిర్వచితమైతే FEAT_RIGHTLEFT
		!revins_on &&
#అయితే_ముగింపు
		((curwin->w_cursor.lnum == 1 && curwin->w_cursor.నిలువు == 0)
		    || (!can_bs(BS_ప్రారంభం)
			&& ((arrow_వాడిన
#నిర్వచితమైతే FEAT_పని_CHANNEL
				&& !bt_prompt(curbuf)
#అయితే_ముగింపు
			) || (curwin->w_cursor.lnum == Insstart_orig.lnum
				&& curwin->w_cursor.నిలువు <= Insstart_orig.నిలువు)))
		    || (!can_bs(BS_INDENT) && !arrow_వాడిన && ai_నిలువు > 0
					 && curwin->w_cursor.నిలువు <= ai_నిలువు)
		    || (!can_bs(BS_EOL) && curwin->w_cursor.నిలువు == 0))))
    {
	vim_beep(BO_BS);
	తిరిగి FALSE;
    }

    ఒకవేళ (stop_arrow() == FAIL)
	తిరిగి FALSE;
    లోపల_indent = inindent(0);
    ఒకవేళ (లోపల_indent)
	can_cindent = FALSE;
    చివర_comment_pending = NUL;	// After BS, don't auto-end comment
#నిర్వచితమైతే FEAT_RIGHTLEFT
    ఒకవేళ (revins_on)	    // put cursor after last inserted char
	inc_cursor();
#అయితే_ముగింపు

    // Virtualedit:
    //	BACKSPACE_CHAR eats a virtual space
    //	BACKSPACE_WORD eats all coladd
    //	BACKSPACE_LINE eats all coladd and keeps going
    ఒకవేళ (curwin->w_cursor.coladd > 0)
    {
	ఒకవేళ (బహుళకం == BACKSPACE_CHAR)
	{
	    --curwin->w_cursor.coladd;
	    తిరిగి TRUE;
	}
	ఒకవేళ (బహుళకం == BACKSPACE_మాట)
	{
	    curwin->w_cursor.coladd = 0;
	    తిరిగి TRUE;
	}
	curwin->w_cursor.coladd = 0;
    }

    /*
     * Delete newline!
     */
    ఒకవేళ (curwin->w_cursor.నిలువు == 0)
    {
	lnum = Insstart.lnum;
	ఒకవేళ (curwin->w_cursor.lnum == lnum
#నిర్వచితమైతే FEAT_RIGHTLEFT
			|| revins_on
#అయితే_ముగింపు
				    )
	{
	    ఒకవేళ (u_save((linenr_T)(curwin->w_cursor.lnum - 2),
			       (linenr_T)(curwin->w_cursor.lnum + 1)) == FAIL)
		తిరిగి FALSE;
	    --Insstart.lnum;
	    Insstart.నిలువు = ml_get_len(Insstart.lnum);
	}
	/*
	 * In replace mode:
	 * cc < 0: NL was inserted, delete it
	 * cc >= 0: NL was replaced, put original characters back
	 */
	cc = -1;
	ఒకవేళ (State & REPLACE_FLAG)
	    cc = replace_తీయి();	    // returns -1 if NL was inserted
	/*
	 * In replace mode, in the line we started replacing, we only move the
	 * cursor.
	 */
	ఒకవేళ ((State & REPLACE_FLAG) && curwin->w_cursor.lnum <= lnum)
	{
	    dec_cursor();
	}
	లేకపోతే
	{
	    ఒకవేళ (!(State & VREPLACE_FLAG)
				   || curwin->w_cursor.lnum > orig_పంక్తి_లెక్క)
	    {
		తాత్కాలిక = gchar_cursor();	// remember current char
		--curwin->w_cursor.lnum;

		// When "aw" is in 'formatoptions' we must delete the space at
		// the end of the line, otherwise the line will be broken
		// again when auto-formatting.
		ఒకవేళ (has_format_option(FO_AUTO)
					   && has_format_option(FO_WHITE_PAR))
		{
		    char_u  *ptr = ml_get_buf(curbuf, curwin->w_cursor.lnum,
									FALSE);
		    పూర్ణాంకం	    len = ml_get_curline_len();

		    ఒకవేళ (len > 0 && ptr[len - 1] == ' ')
		    {
			char_u *newp = alloc(curbuf->b_ml.ml_పంక్తి_len - 1);

			ఒకవేళ (newp != ఖాళీ)
			{
			    mch_memmove(newp, ptr, len - 1);
			    newp[len - 1] = NUL;
			    ఒకవేళ (curbuf->b_ml.ml_పంక్తి_len > len + 1)
				mch_memmove(newp + len, ptr + len + 1,
					   curbuf->b_ml.ml_పంక్తి_len - len - 1);

			    ఒకవేళ (curbuf->b_ml.ml_flags
					      & (ML_పంక్తి_DIRTY | ML_ALLOCATED))
				vim_free(curbuf->b_ml.ml_పంక్తి_ptr);
			    curbuf->b_ml.ml_పంక్తి_ptr = newp;
			    curbuf->b_ml.ml_పంక్తి_len--;
			    curbuf->b_ml.ml_పంక్తి_textlen--;
			    curbuf->b_ml.ml_flags |= ML_పంక్తి_DIRTY;
			}
		    }
		}

		(శూన్య)do_join(2, FALSE, FALSE, FALSE, FALSE);
		ఒకవేళ (తాత్కాలిక == NUL && gchar_cursor() != NUL)
		    inc_cursor();
	    }
	    లేకపోతే
		dec_cursor();

	    /*
	     * In MODE_REPLACE mode we have to put back the text that was
	     * replaced by the NL. On the replace stack is first a
	     * NUL-terminated sequence of characters that were deleted and then
	     * the characters that NL replaced.
	     */
	    ఒకవేళ (State & REPLACE_FLAG)
	    {
		/*
		 * Do the next ins_char() in MODE_NORMAL state, to
		 * prevent ins_char() from replacing characters and
		 * avoiding showmatch().
		 */
		oldState = State;
		State = బహుళకం_సాధారణ;
		/*
		 * restore characters (blanks) deleted after cursor
		 */
		అయితే (cc > 0)
		{
		    save_నిలువు = curwin->w_cursor.నిలువు;
		    mb_replace_తీయి_ins(cc);
		    curwin->w_cursor.నిలువు = save_నిలువు;
		    cc = replace_తీయి();
		}
		// restore the characters that NL replaced
		replace_తీయి_ins();
		State = oldState;
	    }
	}
	did_ai = FALSE;
    }
    లేకపోతే
    {
	/*
	 * Delete character(s) before the cursor.
	 */
#నిర్వచితమైతే FEAT_RIGHTLEFT
	ఒకవేళ (revins_on)		// put cursor on last inserted char
	    dec_cursor();
#అయితే_ముగింపు
	mincol = 0;
						// keep indent
	ఒకవేళ (బహుళకం == BACKSPACE_పంక్తి
		&& (curbuf->b_p_ai || cindent_on())
#నిర్వచితమైతే FEAT_RIGHTLEFT
		&& !revins_on
#అయితే_ముగింపు
			    )
	{
	    save_నిలువు = curwin->w_cursor.నిలువు;
	    beginline(BL_WHITE);
	    ఒకవేళ (curwin->w_cursor.నిలువు < save_నిలువు)
	    {
		mincol = curwin->w_cursor.నిలువు;
		// should now fix the indent to match with the previous line
		call_fix_indent = TRUE;
	    }
	    curwin->w_cursor.నిలువు = save_నిలువు;
	}

	/*
	 * Handle deleting one 'shiftwidth' or 'softtabstop'.
	 */
	ఒకవేళ (	   బహుళకం == BACKSPACE_CHAR
		&& ((p_sta && లోపల_indent)
		    || ((get_sts_విలువ() != 0
#నిర్వచితమైతే FEAT_VARTABS
			|| tabstop_లెక్క(curbuf->b_p_vsts_array)
#అయితే_ముగింపు
			)
			&& curwin->w_cursor.నిలువు > 0
			&& (*(ml_get_cursor() - 1) == TAB
			    || (*(ml_get_cursor() - 1) == ' '
				&& (!*inserted_space_p
				    || arrow_వాడిన))))))
	{
	    colnr_T	vcol = 0;
	    colnr_T	want_vcol;
	    char_u	*పంక్తి;
	    char_u	*ptr;
	    char_u	*cursor_ptr;
	    char_u	*space_ptr;
	    colnr_T	space_vcol = 0;
	    పూర్ణాంకం		prev_space = FALSE;
	    colnr_T	want_నిలువు;

	    *inserted_space_p = FALSE;

	    space_ptr = ptr = పంక్తి = ml_get_curline();
	    cursor_ptr = పంక్తి + curwin->w_cursor.నిలువు;

	    // Compute virtual column of cursor position, and find the last
	    // whitespace before cursor that is preceded by non-whitespace.
	    // Use chartabsize() so that virtual text and wrapping are ignored.
	    అయితే (ptr < cursor_ptr)
	    {
		పూర్ణాంకం	cur_space = VIM_ISWHITE(*ptr);

		ఒకవేళ (!prev_space && cur_space)
		{
		    space_ptr = ptr;
		    space_vcol = vcol;
		}
		vcol += chartabsize(ptr, vcol);
		MB_PTR_ADV(ptr);
		prev_space = cur_space;
	    }

	    // Compute the virtual column where we want to be.
	    want_vcol = vcol > 0 ? vcol - 1 : 0;
	    ఒకవేళ (p_sta && లోపల_indent)
		want_vcol -= want_vcol % (పూర్ణాంకం)get_sw_విలువ(curbuf);
	    లేకపోతే
#నిర్వచితమైతే FEAT_VARTABS
		want_vcol = tabstop_ప్రారంభం(want_vcol, get_sts_విలువ(),
						       curbuf->b_p_vsts_array);
#లేకపోతే
		want_vcol -= want_vcol % (పూర్ణాంకం)get_sts_విలువ();
#అయితే_ముగింపు

	    // Find the position to stop backspacing.
	    // Use chartabsize() so that virtual text and wrapping are ignored.
	    అయితే (TRUE)
	    {
		పూర్ణాంకం కొలత = chartabsize(space_ptr, space_vcol);

		ఒకవేళ (space_vcol + కొలత > want_vcol)
		    ఆపు;
		space_vcol += కొలత;
		MB_PTR_ADV(space_ptr);
	    }
	    want_నిలువు = space_ptr - పంక్తి;

	    // Delete characters until we are at or before want_col.
	    అయితే (curwin->w_cursor.నిలువు > want_నిలువు)
		ins_bs_one();

	    // Insert extra spaces until we are at want_vcol.
	    కొరకు (; space_vcol < want_vcol; space_vcol++)
	    {
		// Remember the first char we inserted
		ఒకవేళ (curwin->w_cursor.lnum == Insstart_orig.lnum
				   && curwin->w_cursor.నిలువు < Insstart_orig.నిలువు)
		    Insstart_orig.నిలువు = curwin->w_cursor.నిలువు;

		ఒకవేళ (State & VREPLACE_FLAG)
		    ins_char(' ');
		లేకపోతే
		{
		    ins_str((char_u *)" ", 1);
		    ఒకవేళ ((State & REPLACE_FLAG))
			replace_నెట్టు(NUL);
		}
	    }
	}

	/*
	 * Delete up to starting point, start of line or previous word.
	 */
	లేకపోతే
	{
	    పూర్ణాంకం cclass = 0, prev_cclass = 0;

	    ఒకవేళ (has_mbyte)
		cclass = mb_get_class(ml_get_cursor());
	    చేయి
	    {
#నిర్వచితమైతే FEAT_RIGHTLEFT
		ఒకవేళ (!revins_on) // put cursor on char to be deleted
#అయితే_ముగింపు
		    dec_cursor();

		cc = gchar_cursor();
		// look multi-byte character class
		ఒకవేళ (has_mbyte)
		{
		    prev_cclass = cclass;
		    cclass = mb_get_class(ml_get_cursor());
		}

		// start of word?
		ఒకవేళ (బహుళకం == BACKSPACE_మాట && !vim_isspace(cc))
		{
		    బహుళకం = BACKSPACE_మాట_NOT_SPACE;
		    తాత్కాలిక = vim_iswordc(cc);
		}
		// end of word?
		లేకపోతే ఒకవేళ (బహుళకం == BACKSPACE_మాట_NOT_SPACE
			&& ((vim_isspace(cc) || vim_iswordc(cc) != తాత్కాలిక)
			|| prev_cclass != cclass))
		{
#నిర్వచితమైతే FEAT_RIGHTLEFT
		    ఒకవేళ (!revins_on)
#అయితే_ముగింపు
			inc_cursor();
#నిర్వచితమైతే FEAT_RIGHTLEFT
		    లేకపోతే ఒకవేళ (State & REPLACE_FLAG)
			dec_cursor();
#అయితే_ముగింపు
		    ఆపు;
		}
		ఒకవేళ (State & REPLACE_FLAG)
		    replace_do_bs(-1);
		లేకపోతే
		{
		    ఒకవేళ (enc_utf8 && p_deco)
			(శూన్య)utfc_ptr2char(ml_get_cursor(), cpc);
		    (శూన్య)del_char(FALSE);
		    /*
		     * If there are combining characters and 'delcombine' is set
		     * move the cursor back.  Don't back up before the base
		     * character.
		     */
		    ఒకవేళ (enc_utf8 && p_deco && cpc[0] != NUL)
			inc_cursor();
#నిర్వచితమైతే FEAT_RIGHTLEFT
		    ఒకవేళ (revins_chars)
		    {
			revins_chars--;
			revins_legal++;
		    }
		    ఒకవేళ (revins_on && gchar_cursor() == NUL)
			ఆపు;
#అయితే_ముగింపు
		}
		// Just a single backspace?:
		ఒకవేళ (బహుళకం == BACKSPACE_CHAR)
		    ఆపు;
	    } అయితే (
#నిర్వచితమైతే FEAT_RIGHTLEFT
		    revins_on ||
#అయితే_ముగింపు
		    (curwin->w_cursor.నిలువు > mincol
		    &&  (can_bs(BS_NOSTOP)
			|| (curwin->w_cursor.lnum != Insstart_orig.lnum
			|| curwin->w_cursor.నిలువు != Insstart_orig.నిలువు)
		    )));
	}
	did_backspace = TRUE;
    }
    did_si = FALSE;
    can_si = FALSE;
    can_si_వెనుక = FALSE;
    ఒకవేళ (curwin->w_cursor.నిలువు <= 1)
	did_ai = FALSE;

    ఒకవేళ (call_fix_indent)
	fix_indent();

    /*
     * It's a little strange to put backspaces into the redo
     * buffer, but it makes auto-indent a lot easier to deal
     * with.
     */
    Append_Char_గమ్యం_Redobuff(c);

    // If deleted before the insertion point, adjust it
    ఒకవేళ (curwin->w_cursor.lnum == Insstart_orig.lnum
				  && curwin->w_cursor.నిలువు < Insstart_orig.నిలువు)
	Insstart_orig.నిలువు = curwin->w_cursor.నిలువు;

    // vi behaviour: the cursor moves backward but the character that
    //		     was there remains visible
    // Vim behaviour: the cursor moves backward and the character that
    //		      was there is erased from the screen.
    // We can emulate the vi behaviour by pretending there is a dollar
    // displayed even when there isn't.
    //  --pkv Sun Jan 19 01:56:40 EST 2003
    ఒకవేళ (vim_strchr(p_cpo, CPO_BACKSPACE) != ఖాళీ && dollar_vcol == -1)
	dollar_vcol = curwin->w_virtcol;

#నిర్వచితమైతే FEAT_FOLDING
    // When deleting a char the cursor line must never be in a closed fold.
    // E.g., when 'foldmethod' is indent and deleting the first non-white
    // char before a Tab.
    ఒకవేళ (did_backspace)
	foldOpenCursor();
#అయితే_ముగింపు

    తిరిగి did_backspace;
}

/*
 * Handle receiving P_PS: start paste mode.  Inserts the following text up to
 * P_PE literally.
 * When "drop" is TRUE then consume the text and drop it.
 */
    పూర్ణాంకం
bracketed_paste(paste_బహుళకం_T బహుళకం, పూర్ణాంకం drop, garray_T *gap)
{
    పూర్ణాంకం		c;
    char_u	buf[NUMBUFLEN + MB_MAXBYTES];
    పూర్ణాంకం		idx = 0;
    char_u	*చివర = కనుగొను_termcode((char_u *)"PE");
    పూర్ణాంకం		ret_char = -1;
    పూర్ణాంకం		save_allow_కీలకాలు = allow_కీలకాలు;
    పూర్ణాంకం		save_paste = p_paste;

    // If the end code is too long we can't detect it, read everything.
    ఒకవేళ (చివర != ఖాళీ && STRLEN(చివర) >= NUMBUFLEN)
	చివర = ఖాళీ;
    ++no_mapping;
    allow_కీలకాలు = 0;
    ఒకవేళ (!p_paste)
	// Also have the side effects of setting 'paste' to make it work much
	// faster.
	సమితి_option_విలువ_give_err((char_u *)"paste", TRUE, ఖాళీ, 0);

    కొరకు (;;)
    {
	// When the end is not defined read everything there is.
	ఒకవేళ (చివర == ఖాళీ && vpeekc() == NUL)
	    ఆపు;
	చేయి
	    c = vgetc();
	అయితే (c == K_IGNORE || c == K_VER_SCROLLBAR || c == K_HOR_SCROLLBAR);

	ఒకవేళ (c == NUL || got_int || (ex_సాధారణ_busy > 0 && c == Ctrl_C))
	    // When CTRL-C was encountered the typeahead will be flushed and we
	    // won't get the end sequence.  Except when using ":normal".
	    ఆపు;

	ఒకవేళ (has_mbyte)
	    idx += (*mb_char2bytes)(c, buf + idx);
	లేకపోతే
	    buf[idx++] = c;
	buf[idx] = NUL;
	ఒకవేళ (చివర != ఖాళీ && STRNCMP(buf, చివర, idx) == 0)
	{
	    ఒకవేళ (చివర[idx] == NUL)
		ఆపు; // Found the end of paste code.
	    కొనసాగు;
	}
	ఒకవేళ (!drop)
	{
	    ఎంపిక (బహుళకం)
	    {
		స్థితి PASTE_CMDLINE:
		    put_on_cmdline(buf, idx, TRUE);
		    ఆపు;

		స్థితి PASTE_EX:
		    // add one for the NUL that is going to be appended
		    ఒకవేళ (gap != ఖాళీ && ga_grow(gap, idx + 1) == OK)
		    {
			mch_memmove((అక్షరం *)gap->ga_దత్తాంశం + gap->ga_len,
							     buf, (size_t)idx);
			gap->ga_len += idx;
		    }
		    ఆపు;

		స్థితి PASTE_చొప్పించు:
		    ఒకవేళ (stop_arrow() == OK)
		    {
			c = buf[0];
			ఒకవేళ (idx == 1 && (c == CAR || c == K_KENTER || c == NL))
			    ins_eol(c);
			లేకపోతే
			{
			    ins_char_బైట్లు(buf, idx);
			    Append_గమ్యం_Redobuff_Lit(buf, idx);
			}
		    }
		    ఆపు;

		స్థితి PASTE_ONE_CHAR:
		    ఒకవేళ (ret_char == -1)
		    {
			ఒకవేళ (has_mbyte)
			    ret_char = (*mb_ptr2char)(buf);
			లేకపోతే
			    ret_char = buf[0];
		    }
		    ఆపు;
	    }
	}
	idx = 0;
    }

    --no_mapping;
    allow_కీలకాలు = save_allow_కీలకాలు;
    ఒకవేళ (!save_paste)
	సమితి_option_విలువ_give_err((char_u *)"paste", FALSE, ఖాళీ, 0);

    తిరిగి ret_char;
}

#ఒకవేళ defined(FEAT_GUI_TABLINE)
    స్థిర శూన్య
ins_tabline(పూర్ణాంకం c)
{
    // We will be leaving the current window, unless closing another tab.
    ఒకవేళ (c != K_TABMENU || current_tabmenu != TABLINE_MENU_CLOSE
		|| (current_tab != 0 && current_tab != tabpage_సూచీ(curtab)))
    {
	undisplay_dollar();
	ప్రారంభం_arrow(&curwin->w_cursor);
	can_cindent = TRUE;
    }

    ఒకవేళ (c == K_TABLINE)
	goto_tabpage(current_tab);
    లేకపోతే
    {
	handle_tabmenu();
	redraw_statuslines();	// will redraw the tabline when needed
    }
}
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_GUI)
    శూన్య
ins_scroll(శూన్య)
{
    pos_T	tpos;

    undisplay_dollar();
    tpos = curwin->w_cursor;
    ఒకవేళ (gui_do_scroll())
    {
	ప్రారంభం_arrow(&tpos);
	can_cindent = TRUE;
    }
}

    శూన్య
ins_horscroll(శూన్య)
{
    pos_T	tpos;

    undisplay_dollar();
    tpos = curwin->w_cursor;
    ఒకవేళ (do_mousescroll_horiz(scrollbar_విలువ))
    {
	ప్రారంభం_arrow(&tpos);
	can_cindent = TRUE;
    }
}
#అయితే_ముగింపు

    స్థిర శూన్య
ins_ఎడమ(శూన్య)
{
    pos_T	tpos;
    పూర్ణాంకం		చివర_change = dont_sync_undo == FALSE; // end undoable change

#నిర్వచితమైతే FEAT_FOLDING
    ఒకవేళ ((fdo_flags & FDO_HOR) && కీలకం_Typed)
	foldOpenCursor();
#అయితే_ముగింపు
    undisplay_dollar();
    tpos = curwin->w_cursor;
    ఒకవేళ (oneleft() == OK)
    {
#ఒకవేళ defined(FEAT_XIM) && defined(FEAT_GUI_GTK)
	// Only call start_arrow() when not busy with preediting, it will
	// break undo.  K_LEFT is inserted in im_correct_cursor().
	ఒకవేళ (p_imst == IM_OVER_THE_SPOT || !im_is_preediting())
#అయితే_ముగింపు
	{
	    ప్రారంభం_arrow_with_change(&tpos, చివర_change);
	    ఒకవేళ (!చివర_change)
		Append_Char_గమ్యం_Redobuff(K_ఎడమ);
	}
#నిర్వచితమైతే FEAT_RIGHTLEFT
	// If exit reversed string, position is fixed
	ఒకవేళ (revins_scol != -1 && (పూర్ణాంకం)curwin->w_cursor.నిలువు >= revins_scol)
	    revins_legal++;
	revins_chars++;
#అయితే_ముగింపు
    }

    /*
     * if 'whichwrap' set for cursor in insert mode may go to
     * previous line
     */
    లేకపోతే ఒకవేళ (vim_strchr(p_ww, '[') != ఖాళీ && curwin->w_cursor.lnum > 1)
    {
	// always break undo when moving upwards/downwards, else undo may break
	ప్రారంభం_arrow(&tpos);
	--(curwin->w_cursor.lnum);
	coladvance((colnr_T)MAXCOL);
	curwin->w_సమితి_curswant = true;	// so we stay at the end
    }
    లేకపోతే
	vim_beep(BO_CRSR);
    dont_sync_undo = FALSE;
}

    స్థిర శూన్య
ins_home(పూర్ణాంకం c)
{
    pos_T	tpos;

#నిర్వచితమైతే FEAT_FOLDING
    ఒకవేళ ((fdo_flags & FDO_HOR) && కీలకం_Typed)
	foldOpenCursor();
#అయితే_ముగింపు
    undisplay_dollar();
    tpos = curwin->w_cursor;
    ఒకవేళ (c == K_C_HOME)
	curwin->w_cursor.lnum = 1;
    curwin->w_cursor.నిలువు = 0;
    curwin->w_cursor.coladd = 0;
    curwin->w_curswant = 0;
    ప్రారంభం_arrow(&tpos);
}

    స్థిర శూన్య
ins_చివర(పూర్ణాంకం c)
{
    pos_T	tpos;

#నిర్వచితమైతే FEAT_FOLDING
    ఒకవేళ ((fdo_flags & FDO_HOR) && కీలకం_Typed)
	foldOpenCursor();
#అయితే_ముగింపు
    undisplay_dollar();
    tpos = curwin->w_cursor;
    ఒకవేళ (c == K_C_చివర)
	curwin->w_cursor.lnum = curbuf->b_ml.ml_పంక్తి_లెక్క;
    coladvance((colnr_T)MAXCOL);
    curwin->w_curswant = MAXCOL;

    ప్రారంభం_arrow(&tpos);
}

    స్థిర శూన్య
ins_s_ఎడమ(శూన్య)
{
    పూర్ణాంకం చివర_change = dont_sync_undo == FALSE; // end undoable change
#నిర్వచితమైతే FEAT_FOLDING
    ఒకవేళ ((fdo_flags & FDO_HOR) && కీలకం_Typed)
	foldOpenCursor();
#అయితే_ముగింపు
    undisplay_dollar();
    ఒకవేళ (curwin->w_cursor.lnum > 1 || curwin->w_cursor.నిలువు > 0)
    {
	ప్రారంభం_arrow_with_change(&curwin->w_cursor, చివర_change);
	ఒకవేళ (!చివర_change)
	    Append_Char_గమ్యం_Redobuff(K_S_ఎడమ);
	(శూన్య)bck_మాట(1L, FALSE, FALSE);
	curwin->w_సమితి_curswant = true;
    }
    లేకపోతే
	vim_beep(BO_CRSR);
    dont_sync_undo = FALSE;
}

    స్థిర శూన్య
ins_కుడి(శూన్య)
{
    పూర్ణాంకం చివర_change = dont_sync_undo == FALSE; // end undoable change

#నిర్వచితమైతే FEAT_FOLDING
    ఒకవేళ ((fdo_flags & FDO_HOR) && కీలకం_Typed)
	foldOpenCursor();
#అయితే_ముగింపు
    undisplay_dollar();
    ఒకవేళ (gchar_cursor() != NUL || virtual_చురుకైన())
    {
	ప్రారంభం_arrow_with_change(&curwin->w_cursor, చివర_change);
	ఒకవేళ (!చివర_change)
	    Append_Char_గమ్యం_Redobuff(K_కుడి);
	curwin->w_సమితి_curswant = true;
	ఒకవేళ (virtual_చురుకైన())
	    oneright();
	లేకపోతే
	{
	    ఒకవేళ (has_mbyte)
		curwin->w_cursor.నిలువు += (*mb_ptr2len)(ml_get_cursor());
	    లేకపోతే
		++curwin->w_cursor.నిలువు;
	}

#నిర్వచితమైతే FEAT_RIGHTLEFT
	revins_legal++;
	ఒకవేళ (revins_chars)
	    revins_chars--;
#అయితే_ముగింపు
    }
    // if 'whichwrap' set for cursor in insert mode, may move the
    // cursor to the next line
    లేకపోతే ఒకవేళ (vim_strchr(p_ww, ']') != ఖాళీ
	    && curwin->w_cursor.lnum < curbuf->b_ml.ml_పంక్తి_లెక్క)
    {
	ప్రారంభం_arrow(&curwin->w_cursor);
	curwin->w_సమితి_curswant = true;
	++curwin->w_cursor.lnum;
	curwin->w_cursor.నిలువు = 0;
    }
    లేకపోతే
	vim_beep(BO_CRSR);
    dont_sync_undo = FALSE;
}

    స్థిర శూన్య
ins_s_కుడి(శూన్య)
{
    పూర్ణాంకం చివర_change = dont_sync_undo == FALSE; // end undoable change
#నిర్వచితమైతే FEAT_FOLDING
    ఒకవేళ ((fdo_flags & FDO_HOR) && కీలకం_Typed)
	foldOpenCursor();
#అయితే_ముగింపు
    undisplay_dollar();
    ఒకవేళ (curwin->w_cursor.lnum < curbuf->b_ml.ml_పంక్తి_లెక్క
	    || gchar_cursor() != NUL)
    {
	ప్రారంభం_arrow_with_change(&curwin->w_cursor, చివర_change);
	ఒకవేళ (!చివర_change)
	    Append_Char_గమ్యం_Redobuff(K_S_కుడి);
	(శూన్య)fwd_మాట(1L, FALSE, 0);
	curwin->w_సమితి_curswant = true;
    }
    లేకపోతే
	vim_beep(BO_CRSR);
    dont_sync_undo = FALSE;
}

    స్థిర శూన్య
ins_పైకి(
    పూర్ణాంకం		startcol)	// when TRUE move to Insstart.col
{
    pos_T	tpos;
    linenr_T	old_topline = curwin->w_topline;
#నిర్వచితమైతే FEAT_DIFF
    పూర్ణాంకం		old_topfill = curwin->w_topfill;
#అయితే_ముగింపు

    undisplay_dollar();
    tpos = curwin->w_cursor;
    ఒకవేళ (cursor_పైకి(1L, TRUE) == OK)
    {
	ఒకవేళ (startcol)
	    coladvance(getvcol_nolist(&Insstart));
	ఒకవేళ (old_topline != curwin->w_topline
#నిర్వచితమైతే FEAT_DIFF
		|| old_topfill != curwin->w_topfill
#అయితే_ముగింపు
		)
	    redraw_later(UPD_VALID);
	ప్రారంభం_arrow(&tpos);
	can_cindent = TRUE;
    }
    లేకపోతే
	vim_beep(BO_CRSR);
}

    స్థిర శూన్య
ins_pageup(శూన్య)
{
    pos_T	tpos;

    undisplay_dollar();

    ఒకవేళ (mod_ముసుగు & MOD_ముసుగు_CTRL)
    {
	// <C-PageUp>: tab page back
	ఒకవేళ (మొదటి_tabpage->tp_తదుపరి != ఖాళీ)
	{
	    ప్రారంభం_arrow(&curwin->w_cursor);
	    goto_tabpage(-1);
	}
	తిరిగి;
    }

    tpos = curwin->w_cursor;
    ఒకవేళ (pagescroll(BACKWARD, 1L, FALSE) == OK)
    {
	ప్రారంభం_arrow(&tpos);
	can_cindent = TRUE;
    }
    లేకపోతే
	vim_beep(BO_CRSR);
}

    స్థిర శూన్య
ins_కిందికి(
    పూర్ణాంకం		startcol)	// when TRUE move to Insstart.col
{
    pos_T	tpos;
    linenr_T	old_topline = curwin->w_topline;
#నిర్వచితమైతే FEAT_DIFF
    పూర్ణాంకం		old_topfill = curwin->w_topfill;
#అయితే_ముగింపు

    undisplay_dollar();
    tpos = curwin->w_cursor;
    ఒకవేళ (cursor_కిందికి(1L, TRUE) == OK)
    {
	ఒకవేళ (startcol)
	    coladvance(getvcol_nolist(&Insstart));
	ఒకవేళ (old_topline != curwin->w_topline
#నిర్వచితమైతే FEAT_DIFF
		|| old_topfill != curwin->w_topfill
#అయితే_ముగింపు
		)
	    redraw_later(UPD_VALID);
	ప్రారంభం_arrow(&tpos);
	can_cindent = TRUE;
    }
    లేకపోతే
	vim_beep(BO_CRSR);
}

    స్థిర శూన్య
ins_pagedown(శూన్య)
{
    pos_T	tpos;

    undisplay_dollar();

    ఒకవేళ (mod_ముసుగు & MOD_ముసుగు_CTRL)
    {
	// <C-PageDown>: tab page forward
	ఒకవేళ (మొదటి_tabpage->tp_తదుపరి != ఖాళీ)
	{
	    ప్రారంభం_arrow(&curwin->w_cursor);
	    goto_tabpage(0);
	}
	తిరిగి;
    }

    tpos = curwin->w_cursor;
    ఒకవేళ (pagescroll(FORWARD, 1L, FALSE) == OK)
    {
	ప్రారంభం_arrow(&tpos);
	can_cindent = TRUE;
    }
    లేకపోతే
	vim_beep(BO_CRSR);
}

#నిర్వచితమైతే FEAT_DND
    స్థిర శూన్య
ins_drop(శూన్య)
{
    do_put('~', ఖాళీ, BACKWARD, 1L, PUT_CURSEND);
}
#అయితే_ముగింపు

/*
 * Handle TAB in Insert or Replace mode.
 * Return TRUE when the TAB needs to be inserted like a normal character.
 */
    స్థిర పూర్ణాంకం
ins_tab(శూన్య)
{
    పూర్ణాంకం		ind;
    పూర్ణాంకం		i;
    పూర్ణాంకం		తాత్కాలిక;

    ఒకవేళ (Insstart_blank_vcol == MAXCOL && curwin->w_cursor.lnum == Insstart.lnum)
	Insstart_blank_vcol = get_nolist_virtcol();
    ఒకవేళ (echeck_abbr(TAB + ABBR_OFF))
	తిరిగి FALSE;

    ind = inindent(0);
    ఒకవేళ (ind)
	can_cindent = FALSE;

    /*
     * When nothing special, insert TAB like a normal character.
     */
    ఒకవేళ (!curbuf->b_p_et
#నిర్వచితమైతే FEAT_VARTABS
	    && !(p_sta && ind
		// These five lines mean 'tabstop' != 'shiftwidth'
		&& ((tabstop_లెక్క(curbuf->b_p_vts_array) > 1)
		    || (tabstop_లెక్క(curbuf->b_p_vts_array) == 1
			&& tabstop_మొదటి(curbuf->b_p_vts_array)
						       != get_sw_విలువ(curbuf))
		    || (tabstop_లెక్క(curbuf->b_p_vts_array) == 0
			&& curbuf->b_p_ts != get_sw_విలువ(curbuf))))
	    && tabstop_లెక్క(curbuf->b_p_vsts_array) == 0
#లేకపోతే
	    && !(p_sta && ind && curbuf->b_p_ts != get_sw_విలువ(curbuf))
#అయితే_ముగింపు
	    && get_sts_విలువ() == 0)
	తిరిగి TRUE;

    ఒకవేళ (stop_arrow() == FAIL)
	తిరిగి TRUE;

    did_ai = FALSE;
    did_si = FALSE;
    can_si = FALSE;
    can_si_వెనుక = FALSE;
    Append_గమ్యం_Redobuff((char_u *)"\t");

#నిర్వచితమైతే FEAT_VARTABS
    ఒకవేళ (p_sta && ind)		// insert tab in indent, use 'shiftwidth'
    {
	తాత్కాలిక = (పూర్ణాంకం)get_sw_విలువ(curbuf);
	తాత్కాలిక -= get_nolist_virtcol() % తాత్కాలిక;
    }
    లేకపోతే ఒకవేళ (tabstop_లెక్క(curbuf->b_p_vsts_array) > 0 || curbuf->b_p_sts != 0)
				// use 'softtabstop' when set
	తాత్కాలిక = tabstop_padding(get_nolist_virtcol(), get_sts_విలువ(),
						     curbuf->b_p_vsts_array);
    లేకపోతే			// otherwise use 'tabstop'
	తాత్కాలిక = tabstop_padding(get_nolist_virtcol(), curbuf->b_p_ts,
						     curbuf->b_p_vts_array);
#లేకపోతే
    ఒకవేళ (p_sta && ind)		// insert tab in indent, use 'shiftwidth'
	తాత్కాలిక = (పూర్ణాంకం)get_sw_విలువ(curbuf);
    లేకపోతే ఒకవేళ (curbuf->b_p_sts != 0) // use 'softtabstop' when set
	తాత్కాలిక = (పూర్ణాంకం)get_sts_విలువ();
    లేకపోతే			// otherwise use 'tabstop'
	తాత్కాలిక = (పూర్ణాంకం)curbuf->b_p_ts;
    తాత్కాలిక -= get_nolist_virtcol() % తాత్కాలిక;
#అయితే_ముగింపు

    /*
     * Insert the first space with ins_char().	It will delete one char in
     * replace mode.  Insert the rest with ins_str(); it will not delete any
     * chars.  For MODE_VREPLACE state, we use ins_char() for all characters.
     */
    ins_char(' ');
    అయితే (--తాత్కాలిక > 0)
    {
	ఒకవేళ (State & VREPLACE_FLAG)
	    ins_char(' ');
	లేకపోతే
	{
	    ins_str((char_u *)" ", 1);
	    ఒకవేళ (State & REPLACE_FLAG)	    // no char replaced
		replace_నెట్టు(NUL);
	}
    }

    /*
     * When 'expandtab' not set: Replace spaces by TABs where possible.
     */
#నిర్వచితమైతే FEAT_VARTABS
    ఒకవేళ (!curbuf->b_p_et && (tabstop_లెక్క(curbuf->b_p_vsts_array) > 0
			    || get_sts_విలువ() > 0
			    || (p_sta && ind)))
#లేకపోతే
    ఒకవేళ (!curbuf->b_p_et && (get_sts_విలువ() || (p_sta && ind)))
#అయితే_ముగింపు
    {
	char_u		*ptr;
	char_u		*saved_పంక్తి = ఖాళీ;	// init for GCC
	pos_T		pos;
	pos_T		fpos;
	pos_T		*cursor;
	colnr_T		want_vcol, vcol;
	పూర్ణాంకం		change_నిలువు = -1;
	పూర్ణాంకం		save_list = curwin->w_p_list;
	char_u		*tab = (char_u *)"\t";
	chartabsize_T	cts;

	/*
	 * Get the current line.  For MODE_VREPLACE state, don't make real
	 * changes yet, just work on a copy of the line.
	 */
	ఒకవేళ (State & VREPLACE_FLAG)
	{
	    pos = curwin->w_cursor;
	    cursor = &pos;
	    saved_పంక్తి = vim_strnsave(ml_get_curline(), ml_get_curline_len());
	    ఒకవేళ (saved_పంక్తి == ఖాళీ)
		తిరిగి FALSE;
	    ptr = saved_పంక్తి + pos.నిలువు;
	}
	లేకపోతే
	{
	    ptr = ml_get_cursor();
	    cursor = &curwin->w_cursor;
	}

	// When 'L' is not in 'cpoptions' a tab always takes up 'ts' spaces.
	ఒకవేళ (vim_strchr(p_cpo, CPO_LISTWM) == ఖాళీ)
	    curwin->w_p_list = FALSE;

	// Find first white before the cursor
	fpos = curwin->w_cursor;
	అయితే (fpos.నిలువు > 0 && VIM_ISWHITE(ptr[-1]))
	{
	    --fpos.నిలువు;
	    --ptr;
	}

	// In Replace mode, don't change characters before the insert point.
	ఒకవేళ ((State & REPLACE_FLAG)
		&& fpos.lnum == Insstart.lnum
		&& fpos.నిలువు < Insstart.నిలువు)
	{
	    ptr += Insstart.నిలువు - fpos.నిలువు;
	    fpos.నిలువు = Insstart.నిలువు;
	}

	// compute virtual column numbers of first white and cursor
	getvcol(curwin, &fpos, &vcol, ఖాళీ, ఖాళీ, 0);
	getvcol(curwin, cursor, &want_vcol, ఖాళీ, ఖాళీ, 0);

	init_chartabsize_arg(&cts, curwin, 0, vcol, tab, tab);

	// Use as many TABs as possible.  Beware of 'breakindent', 'showbreak'
	// and 'linebreak' adding extra virtual columns.
	అయితే (VIM_ISWHITE(*ptr))
	{
	    i = lbr_chartabsize(&cts);
	    ఒకవేళ (cts.cts_vcol + i > want_vcol)
		ఆపు;
	    ఒకవేళ (*ptr != TAB)
	    {
		*ptr = TAB;
		ఒకవేళ (change_నిలువు < 0)
		{
		    change_నిలువు = fpos.నిలువు;  // Column of first change
		    // May have to adjust Insstart
		    ఒకవేళ (fpos.lnum == Insstart.lnum && fpos.నిలువు < Insstart.నిలువు)
			Insstart.నిలువు = fpos.నిలువు;
		}
	    }
	    ++fpos.నిలువు;
	    ++ptr;
	    cts.cts_vcol += i;
	}
	vcol = cts.cts_vcol;
	clear_chartabsize_arg(&cts);

	ఒకవేళ (change_నిలువు >= 0)
	{
	    పూర్ణాంకం		    repl_off = 0;

	    // Skip over the spaces we need.
	    init_chartabsize_arg(&cts, curwin, 0, vcol, ptr, ptr);
	    అయితే (cts.cts_vcol < want_vcol && *cts.cts_ptr == ' ')
	    {
		cts.cts_vcol += lbr_chartabsize(&cts);
		++cts.cts_ptr;
		++repl_off;
	    }
	    ptr = cts.cts_ptr;
	    vcol = cts.cts_vcol;
	    clear_chartabsize_arg(&cts);

	    ఒకవేళ (vcol > want_vcol)
	    {
		// Must have a char with 'showbreak' just before it.
		--ptr;
		--repl_off;
	    }
	    fpos.నిలువు += repl_off;

	    // Delete following spaces.
	    i = cursor->నిలువు - fpos.నిలువు;
	    ఒకవేళ (i > 0)
	    {
#నిర్వచితమైతే FEAT_PROP_POPUP
		ఒకవేళ (!(State & VREPLACE_FLAG))
		{
		    char_u  *newp;
		    పూర్ణాంకం	    నిలువు;

		    newp = alloc(curbuf->b_ml.ml_పంక్తి_len - i);
		    ఒకవేళ (newp == ఖాళీ)
			తిరిగి FALSE;

		    నిలువు = ptr - curbuf->b_ml.ml_పంక్తి_ptr;
		    ఒకవేళ (నిలువు > 0)
			mch_memmove(newp, ptr - నిలువు, నిలువు);
		    mch_memmove(newp + నిలువు, ptr + i,
					   curbuf->b_ml.ml_పంక్తి_len - నిలువు - i);

		    ఒకవేళ (curbuf->b_ml.ml_flags & (ML_పంక్తి_DIRTY | ML_ALLOCATED))
			vim_free(curbuf->b_ml.ml_పంక్తి_ptr);
		    curbuf->b_ml.ml_పంక్తి_ptr = newp;
		    curbuf->b_ml.ml_పంక్తి_len -= i;
		    curbuf->b_ml.ml_పంక్తి_textlen = 0;
		    curbuf->b_ml.ml_flags =
			   (curbuf->b_ml.ml_flags | ML_పంక్తి_DIRTY) & ~ML_ఖాళీలు;
		}
		లేకపోతే
#అయితే_ముగింపు
		    STRMOVE(ptr, ptr + i);
		// correct replace stack.
		ఒకవేళ ((State & REPLACE_FLAG) && !(State & VREPLACE_FLAG))
		    కొరకు (తాత్కాలిక = i; --తాత్కాలిక >= 0; )
			replace_join(repl_off);
	    }
#నిర్వచితమైతే FEAT_NETBEANS_INTG
	    ఒకవేళ (netbeans_చురుకైన())
	    {
		netbeans_removed(curbuf, fpos.lnum, cursor->నిలువు, (దీర్ఘ)(i + 1));
		netbeans_inserted(curbuf, fpos.lnum, cursor->నిలువు,
							   (char_u *)"\t", 1);
	    }
#అయితే_ముగింపు
	    cursor->నిలువు -= i;

	    /*
	     * In MODE_VREPLACE state, we haven't changed anything yet.  Do it
	     * now by backspacing over the changed spacing and then inserting
	     * the new spacing.
	     */
	    ఒకవేళ (State & VREPLACE_FLAG)
	    {
		// Backspace from real cursor to change_col
		backspace_until_స్తంభం(change_నిలువు);

		// Insert each char in saved_line from changed_col to
		// ptr-cursor
		ins_బైట్లు_len(saved_పంక్తి + change_నిలువు,
						    cursor->నిలువు - change_నిలువు);
	    }
	}

	ఒకవేళ (State & VREPLACE_FLAG)
	    vim_free(saved_పంక్తి);
	curwin->w_p_list = save_list;
    }

    తిరిగి FALSE;
}

/*
 * Handle CR or NL in insert mode.
 * Return FAIL when out of memory or can't undo.
 */
    పూర్ణాంకం
ins_eol(పూర్ణాంకం c)
{
    పూర్ణాంకం	    i;

    ఒకవేళ (echeck_abbr(c + ABBR_OFF))
	తిరిగి OK;
    ఒకవేళ (stop_arrow() == FAIL)
	తిరిగి FAIL;
    undisplay_dollar();

    /*
     * Strange Vi behaviour: In Replace mode, typing a NL will not delete the
     * character under the cursor.  Only push a NUL on the replace stack,
     * nothing to put back when the NL is deleted.
     */
    ఒకవేళ ((State & REPLACE_FLAG) && !(State & VREPLACE_FLAG))
	replace_నెట్టు(NUL);

    /*
     * In MODE_VREPLACE state, a NL replaces the rest of the line, and starts
     * replacing the next line, so we push all of the characters left on the
     * line onto the replace stack.  This is not done here though, it is done
     * in open_line().
     */

    // Put cursor on NUL if on the last char and coladd is 1 (happens after
    // CTRL-O).
    ఒకవేళ (virtual_చురుకైన() && curwin->w_cursor.coladd > 0)
	coladvance(getviscol());

#నిర్వచితమైతే FEAT_RIGHTLEFT
    // NL in reverse insert will always start in the end of
    // current line.
    ఒకవేళ (revins_on)
	curwin->w_cursor.నిలువు += ml_get_cursor_len();
#అయితే_ముగింపు

    Append_గమ్యం_Redobuff(NL_STR);
    i = open_పంక్తి(FORWARD,
	    has_format_option(FO_RET_COMS) ? OPENLINE_DO_COM : 0, old_indent,
	    ఖాళీ);
    old_indent = 0;
    can_cindent = TRUE;
#నిర్వచితమైతే FEAT_FOLDING
    // When inserting a line the cursor line must never be in a closed fold.
    foldOpenCursor();
#అయితే_ముగింపు

    తిరిగి i;
}

#నిర్వచితమైతే FEAT_DIGRAPHS
/*
 * Handle digraph in insert mode.
 * Returns character still to be inserted, or NUL when nothing remaining to be
 * done.
 */
    స్థిర పూర్ణాంకం
ins_digraph(శూన్య)
{
    పూర్ణాంకం	    c;
    పూర్ణాంకం	    cc;
    పూర్ణాంకం	    did_putchar = FALSE;

    pc_స్థితి = PC_స్థితి_UNSET;
    ఒకవేళ (redrawing() && !char_avail())
    {
	// may need to redraw when no more chars available now
	ins_redraw(FALSE);

	edit_putchar('?', TRUE);
	did_putchar = TRUE;
	కూడు_గమ్యం_showcmd_c(Ctrl_K);
    }

# ifdef USE_ON_FLY_SCROLL
    dont_scroll = TRUE;		// disallow scrolling here
# endif

    // don't map the digraph chars. This also prevents the
    // mode message to be deleted when ESC is hit
    ++no_mapping;
    ++allow_కీలకాలు;
    c = plain_vgetc();
    --no_mapping;
    --allow_కీలకాలు;
    ఒకవేళ (did_putchar)
	// when the line fits in 'columns' the '?' is at the start of the next
	// line and will not be removed by the redraw
	edit_unputchar();

    ఒకవేళ (IS_SPECIAL(c) || mod_ముసుగు)	    // special key
    {
	clear_showcmd();
	చొప్పించు_special(c, TRUE, FALSE);
	తిరిగి NUL;
    }
    ఒకవేళ (c != ESC)
    {
	did_putchar = FALSE;
	ఒకవేళ (redrawing() && !char_avail())
	{
	    // may need to redraw when no more chars available now
	    ins_redraw(FALSE);

	    ఒకవేళ (char2cells(c) == 1)
	    {
		ins_redraw(FALSE);
		edit_putchar(c, TRUE);
		did_putchar = TRUE;
	    }
	    కూడు_గమ్యం_showcmd_c(c);
	}
	++no_mapping;
	++allow_కీలకాలు;
	cc = plain_vgetc();
	--no_mapping;
	--allow_కీలకాలు;
	ఒకవేళ (did_putchar)
	    // when the line fits in 'columns' the '?' is at the start of the
	    // next line and will not be removed by a redraw
	    edit_unputchar();
	ఒకవేళ (cc != ESC)
	{
	    Append_గమ్యం_Redobuff((char_u *)CTRL_V_STR);
	    c = digraph_get(c, cc, TRUE);
	    clear_showcmd();
	    తిరిగి c;
	}
    }
    clear_showcmd();
    తిరిగి NUL;
}
#అయితే_ముగింపు

/*
 * Handle CTRL-E and CTRL-Y in Insert mode: copy char from other line.
 * Returns the char to be inserted, or NUL if none found.
 */
    పూర్ణాంకం
ins_copychar(linenr_T lnum)
{
    పూర్ణాంకం		    c;
    char_u	    *ptr, *prev_ptr;
    char_u	    *పంక్తి;
    chartabsize_T   cts;

    ఒకవేళ (lnum < 1 || lnum > curbuf->b_ml.ml_పంక్తి_లెక్క)
    {
	vim_beep(BO_COPY);
	తిరిగి NUL;
    }

    // try to advance to the cursor column
    validate_virtcol();
    పంక్తి = ml_get(lnum);
    prev_ptr = పంక్తి;
    init_chartabsize_arg(&cts, curwin, lnum, 0, పంక్తి, పంక్తి);
    అయితే (cts.cts_vcol < curwin->w_virtcol && *cts.cts_ptr != NUL)
    {
	prev_ptr = cts.cts_ptr;
	cts.cts_vcol += lbr_chartabsize_adv(&cts);
    }
    ఒకవేళ (cts.cts_vcol > curwin->w_virtcol)
	ptr = prev_ptr;
    లేకపోతే
	ptr = cts.cts_ptr;
    clear_chartabsize_arg(&cts);

    c = (*mb_ptr2char)(ptr);
    ఒకవేళ (c == NUL)
	vim_beep(BO_COPY);
    తిరిగి c;
}

/*
 * CTRL-Y or CTRL-E typed in Insert mode.
 */
    స్థిర పూర్ణాంకం
ins_ctrl_ey(పూర్ణాంకం tc)
{
    పూర్ణాంకం	    c = tc;

    ఒకవేళ (ctrl_x_బహుళకం_scroll())
    {
	ఒకవేళ (c == Ctrl_Y)
	    scrolldown_clamp();
	లేకపోతే
	    scrollup_clamp();
	redraw_later(UPD_VALID);
    }
    లేకపోతే
    {
	c = ins_copychar(curwin->w_cursor.lnum + (c == Ctrl_Y ? -1 : 1));
	ఒకవేళ (c != NUL)
	{
	    దీర్ఘ	tw_save;

	    // The character must be taken literally, insert like it
	    // was typed after a CTRL-V, and pretend 'textwidth'
	    // wasn't set.  Digits, 'o' and 'x' are special after a
	    // CTRL-V, don't use it for these.
	    ఒకవేళ (c < 256 && !సురక్షిత_isalnum(c))
		Append_గమ్యం_Redobuff((char_u *)CTRL_V_STR);	// CTRL-V
	    tw_save = curbuf->b_p_tw;
	    curbuf->b_p_tw = -1;
	    చొప్పించు_special(c, TRUE, FALSE);
	    curbuf->b_p_tw = tw_save;
#నిర్వచితమైతే FEAT_RIGHTLEFT
	    revins_chars++;
	    revins_legal++;
#అయితే_ముగింపు
	    c = Ctrl_V;	// pretend CTRL-V is last character
	    auto_format(FALSE, TRUE);
	}
    }
    తిరిగి c;
}

/*
 * Get the value that w_virtcol would have when 'list' is off.
 * Unless 'cpo' contains the 'L' flag.
 */
    colnr_T
get_nolist_virtcol(శూన్య)
{
    // check validity of cursor in current buffer
    ఒకవేళ (curwin->w_బఫరు == ఖాళీ
	|| curwin->w_బఫరు->b_ml.ml_mfp == ఖాళీ
	|| curwin->w_cursor.lnum > curwin->w_బఫరు->b_ml.ml_పంక్తి_లెక్క)
	తిరిగి 0;
    ఒకవేళ (curwin->w_p_list && vim_strchr(p_cpo, CPO_LISTWM) == ఖాళీ)
	తిరిగి getvcol_nolist(&curwin->w_cursor);
    validate_virtcol();
    తిరిగి curwin->w_virtcol;
}

#ఒకవేళ defined(FEAT_EVAL)
/*
 * Handle the InsertCharPre autocommand.
 * "c" is the character that was typed.
 * Return a pointer to allocated memory with the replacement string.
 * Return NULL to continue inserting "c".
 */
    స్థిర char_u *
do_చొప్పించు_char_pre(పూర్ణాంకం c)
{
    char_u	*res;
    char_u	buf[MB_MAXBYTES + 1];
    size_t	buflen;
    పూర్ణాంకం		save_State = State;

    // Return quickly when there is nothing to do.
    ఒకవేళ (!has_insertcharpre())
	తిరిగి ఖాళీ;

    ఒకవేళ (c == Ctrl_RSB)
	తిరిగి ఖాళీ;

    ఒకవేళ (has_mbyte)
	buflen = (*mb_char2bytes)(c, buf);
    లేకపోతే
    {
	buf[0] = c;
	buflen = 1;
    }
    buf[buflen] = NUL;

    // Lock the text to avoid weird things from happening.
    ++textlock;
    సమితి_vim_var_string(VV_CHAR, buf, (పూర్ణాంకం)buflen);  // set v:char

    res = ఖాళీ;
    ఒకవేళ (ins_apply_autocmds(EVENT_INSERTCHARPRE))
    {
	// Get the value of v:char.  It may be empty or more than one
	// character.  Only use it when changed, otherwise continue with the
	// original character to avoid breaking autoindent.
	ఒకవేళ (STRCMP(buf, get_vim_var_str(VV_CHAR)) != 0)
	    res = vim_strsave(get_vim_var_str(VV_CHAR));
    }

    సమితి_vim_var_string(VV_CHAR, ఖాళీ, -1);  // clear v:char
    --textlock;

    // Restore the State, it may have been changed.
    State = save_State;

    తిరిగి res;
}
#అయితే_ముగింపు

    పూర్ణాంకం
get_can_cindent(శూన్య)
{
    తిరిగి can_cindent;
}

    శూన్య
సమితి_can_cindent(పూర్ణాంకం val)
{
    can_cindent = val;
}

/*
 * Trigger "event" and take care of fixing undo.
 */
    పూర్ణాంకం
ins_apply_autocmds(event_T event)
{
    varnumber_T	tick = CHANGEDTICK(curbuf);
    పూర్ణాంకం r;

    r = apply_autocmds(event, ఖాళీ, ఖాళీ, FALSE, curbuf);

    // If u_savesub() was called then we are not prepared to start
    // a new line.  Call u_save() with no contents to fix that.
    // Except when leaving Insert mode.
    ఒకవేళ (event != EVENT_INSERTLEAVE && tick != CHANGEDTICK(curbuf))
	u_save(curwin->w_cursor.lnum, (linenr_T)(curwin->w_cursor.lnum + 1));

    తిరిగి r;
}
