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

#شامل "vim.h"

#تعریف_کرو BACKSPACE_CHAR		    1
#تعریف_کرو BACKSPACE_لفظ		    2
#تعریف_کرو BACKSPACE_لفظ_NOT_SPACE    3
#تعریف_کرو BACKSPACE_سطر		    4

// Set when doing something for completion that may call edit() recursively,
// which is not allowed.
ساکن صحیح	compl_busy = FALSE;


ساکن خالی ins_ctrl_v(خالی);
ساکن خالی داخل_کرو_special(صحیح, صحیح, صحیح);
ساکن خالی redo_literal(صحیح c);
ساکن خالی آغاز_arrow_common(pos_T *انتہا_داخل_کرو_pos, صحیح change);
#اگر_تعریف_شدہ FEAT_SPELL
ساکن خالی check_spell_redraw(خالی);
#اگر_ختم
ساکن خالی stop_داخل_کرو(pos_T *انتہا_داخل_کرو_pos, صحیح esc, صحیح nomove);
ساکن صحیح  echeck_abbr(صحیح);
ساکن خالی mb_replace_نکالو_ins(صحیح cc);
ساکن خالی replace_flush(خالی);
ساکن خالی replace_do_bs(صحیح حد_بندی_ستون);
ساکن صحیح del_char_after_ستون(صحیح حد_بندی_ستون);
ساکن خالی ins_reg(خالی);
ساکن خالی ins_ctrl_g(خالی);
ساکن خالی ins_ctrl_hat(خالی);
ساکن صحیح  ins_esc(طویل *گنتی, صحیح cmdchar, صحیح nomove);
#اگر_تعریف_شدہ FEAT_RIGHTLEFT
ساکن خالی ins_ctrl_(خالی);
#اگر_ختم
ساکن صحیح ins_آغاز_select(صحیح c);
ساکن خالی ins_داخل_کرو(صحیح replaceState);
ساکن خالی ins_ctrl_o(خالی);
ساکن خالی ins_سرکاؤ(صحیح c, صحیح lastc);
ساکن خالی ins_del(خالی);
ساکن صحیح  ins_bs(صحیح c, صحیح منوال, صحیح *inserted_space_p);
#اگر defined(FEAT_GUI_TABLINE)
ساکن خالی ins_tabline(صحیح c);
#اگر_ختم
ساکن خالی ins_بایاں(خالی);
ساکن خالی ins_home(صحیح c);
ساکن خالی ins_انتہا(صحیح c);
ساکن خالی ins_s_بایاں(خالی);
ساکن خالی ins_دایاں(خالی);
ساکن خالی ins_s_دایاں(خالی);
ساکن خالی ins_اوپر(صحیح startcol);
ساکن خالی ins_pageup(خالی);
ساکن خالی ins_نیچے_کو(صحیح startcol);
ساکن خالی ins_pagedown(خالی);
#اگر_تعریف_شدہ FEAT_DND
ساکن خالی ins_drop(خالی);
#اگر_ختم
ساکن صحیح  ins_tab(خالی);
#اگر_تعریف_شدہ FEAT_DIGRAPHS
ساکن صحیح  ins_digraph(خالی);
#اگر_ختم
ساکن صحیح  ins_ctrl_ey(صحیح tc);
#اگر defined(FEAT_EVAL)
ساکن char_u *do_داخل_کرو_char_pre(صحیح c);
#اگر_ختم

ساکن colnr_T	Insstart_textlen;	// length of line when insert started
ساکن colnr_T	Insstart_blank_vcol;	// vcol for first inserted blank
ساکن صحیح	update_Insstart_orig = TRUE; // set Insstart_orig to Insstart

ساکن string_T	آخری_داخل_کرو = {تہی, 0};    // the text of the previous insert,
					    // K_SPECIAL and CSI are escaped
ساکن صحیح	آخری_داخل_کرو_skip; // nr of chars in front of previous insert
ساکن صحیح	new_داخل_کرو_skip;  // nr of chars in front of current insert
ساکن صحیح	did_restart_edit;	// "restart_edit" when calling edit()

ساکن صحیح	can_cindent;		// may do cindenting on this line

#اگر_تعریف_شدہ FEAT_RIGHTLEFT
ساکن صحیح	revins_on;		// reverse insert mode on
ساکن صحیح	revins_chars;		// how much to skip after edit
ساکن صحیح	revins_legal;		// was the last char 'legal'?
ساکن صحیح	revins_scol;		// start column of revins session
#اگر_ختم

ساکن صحیح	ins_need_undo;		// call u_save() before inserting a
					// char.  Set when edit() is called.
					// after that arrow_used is used.

ساکن صحیح	dont_sync_undo = FALSE;	// CTRL-G U prevents syncing undo for
					// the next left/right cursor key

// With 'autocompletedelay' set, arm the delay and let the main loop fire
// Insert-mode autocommands; the popup is shown later on K_COMPLETE_DELAY.
// Otherwise trigger completion right away.
#تعریف_کرو TRIGGER_AUTOCOMPLETE()			\
    کرو {					\
	update_screen(UPD_VALID);  /* Show char (deletion) immediately */ \
	برآمد_flush();				\
	ins_compl_enable_autocomplete();	\
	ins_compl_arm_autostart();		\
	اگر (!ins_compl_arm_autocomplete_delay())\
	    جاؤ docomplete;			\
    } جب (0)

#تعریف_کرو MAY_TRIGGER_AUTOCOMPLETE(c)				\
    کرو {							\
	اگر (ins_compl_has_autocomplete() && !char_avail()	\
		&& curwin->w_cursor.ستون > 0)			\
	{							\
	    (c) = char_before_cursor();				\
	    اگر (vim_isprintc(c))				\
		TRIGGER_AUTOCOMPLETE();				\
	}							\
    } جب (0)

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
    صحیح
edit(
    صحیح		cmdchar,
    صحیح		startln,	// if set, insert at start of line
    طویل	گنتی)
{
    صحیح		c = 0;
    char_u	*ptr;
    صحیح		lastc = 0;
    صحیح		mincol;
    ساکن linenr_T o_lnum = 0;
    صحیح		i;
    صحیح		did_backspace = TRUE;	    // previous char was backspace
    صحیح		سطر_is_white = FALSE;	    // line is empty before insert
    linenr_T	old_topline = 0;	    // topline before insertion
#اگر_تعریف_شدہ FEAT_DIFF
    صحیح		old_topfill = -1;
#اگر_ختم
    صحیح		inserted_space = FALSE;     // just inserted a space
    صحیح		replaceState = منوال_REPLACE;
    صحیح		nomove = FALSE;		    // don't move cursor on return
#اگر_تعریف_شدہ FEAT_کام_CHANNEL
    صحیح		cmdchar_todo = cmdchar;
#اگر_ختم
#اگر_تعریف_شدہ FEAT_CONCEAL
    صحیح		cursor_سطر_was_concealed;
#اگر_ختم
    صحیح		ins_just_started = TRUE;

    // Remember whether editing was restarted after CTRL-O.
    did_restart_edit = restart_edit;

    // sleep before redrawing, needed for "CTRL-O :" that results in an
    // error message
    check_for_delay(TRUE);

    // set Insstart_orig to Insstart
    update_Insstart_orig = TRUE;

#اگر_تعریف_شدہ HAVE_SANDBOX
    // Don't allow inserting in the sandbox.
    اگر (sandbox != 0)
    {
	emsg(_(e_not_allowed_اندر_sandbox));
	واپس FALSE;
    }
#اگر_ختم
    // Don't allow changes in the buffer while editing the cmdline.  The
    // caller of getcmdline() may get confused.
    // Don't allow recursive insert mode when busy with completion.
    اگر (textlock != 0 || ins_compl_فعال() || compl_busy || pum_visible())
    {
	emsg(_(e_not_allowed_مقصد_change_متن_or_change_کھڑکی));
	واپس FALSE;
    }
    ins_compl_clear();	    // clear stuff for CTRL-X mode

    /*
     * Trigger InsertEnter autocommands.  Do not do this for "r<CR>" or "grx".
     */
    اگر (cmdchar != 'r' && cmdchar != 'v')
    {
	pos_T   save_cursor = curwin->w_cursor;

#اگر_تعریف_شدہ FEAT_EVAL
	اگر (cmdchar == 'R')
	    ptr = (char_u *)"r";
	ورنہ اگر (cmdchar == 'V')
	    ptr = (char_u *)"v";
	ورنہ
	    ptr = (char_u *)"i";
	سیٹ_vim_var_string(VV_INSERTMODE, ptr, 1);
	سیٹ_vim_var_string(VV_CHAR, تہی, -1);  // clear v:char
#اگر_ختم
	ins_apply_autocmds(EVENT_INSERTENTER);

	// Check for changed highlighting, e.g. for ModeMsg.
	اگر (need_highlight_بدلا)
	    highlight_بدلا();

	// Make sure the cursor didn't move.  Do call check_cursor_col() in
	// case the text was modified.  Since Insert mode was not started yet
	// a call to check_cursor_col() may move the cursor, especially with
	// the "A" command, thus set State to avoid that. Also check that the
	// line number is still valid (lines may have been deleted).
	// Do not restore if v:char was set to a non-empty string.
	اگر (!EQUAL_POS(curwin->w_cursor, save_cursor)
#اگر_تعریف_شدہ FEAT_EVAL
		&& *get_vim_var_str(VV_CHAR) == NUL
#اگر_ختم
		&& save_cursor.lnum <= curbuf->b_ml.ml_سطر_گنتی)
	{
	    صحیح save_state = State;

	    curwin->w_cursor = save_cursor;
	    State = منوال_داخل_کرو;
	    check_cursor_ستون();
	    State = save_state;
	}
    }

#اگر_تعریف_شدہ FEAT_CONCEAL
    // Check if the cursor line was concealed before changing State.
    cursor_سطر_was_concealed = curwin->w_p_cole > 0
						&& conceal_cursor_سطر(curwin);
#اگر_ختم

    /*
     * When doing a paste with the middle mouse button, Insstart is set to
     * where the paste started.
     */
    اگر (where_paste_started.lnum != 0)
	Insstart = where_paste_started;
    ورنہ
    {
	Insstart = curwin->w_cursor;
	اگر (startln)
	    Insstart.ستون = 0;
    }
    Insstart_textlen = (colnr_T)linetabsize_str(ml_get_curline());
    Insstart_blank_vcol = MAXCOL;
    اگر (!did_ai)
	ai_ستون = 0;

    اگر (cmdchar != NUL && restart_edit == 0)
    {
	ResetRedobuff();
	Append_نمبر_مقصد_Redobuff(گنتی);
	اگر (cmdchar == 'V' || cmdchar == 'v')
	{
	    // "gR" or "gr" command
	    Append_Char_مقصد_Redobuff('g');
	    Append_Char_مقصد_Redobuff((cmdchar == 'v') ? 'r' : 'R');
	}
	ورنہ
	{
	    اگر (cmdchar == K_PS)
		Append_Char_مقصد_Redobuff('a');
	    ورنہ
		Append_Char_مقصد_Redobuff(cmdchar);
	    اگر (cmdchar == 'g')		    // "gI" command
		Append_Char_مقصد_Redobuff('I');
	    ورنہ اگر (cmdchar == 'r')	    // "r<CR>" command
		گنتی = 1;		    // insert only one <CR>
	}
    }

    اگر (cmdchar == 'R')
    {
	State = منوال_REPLACE;
    }
    ورنہ اگر (cmdchar == 'V' || cmdchar == 'v')
    {
	State = منوال_VREPLACE;
	replaceState = منوال_VREPLACE;
	orig_سطر_گنتی = curbuf->b_ml.ml_سطر_گنتی;
	vr_سطریں_بدلا = 1;
    }
    ورنہ
	State = منوال_داخل_کرو;

    may_trigger_modechanged();
    stop_داخل_کرو_منوال = FALSE;

#اگر_تعریف_شدہ FEAT_CONCEAL
    // Check if the cursor line needs redrawing after changing State.  If
    // 'concealcursor' is "n" it needs to be redrawn without concealing.
    conceal_check_cursor_سطر(cursor_سطر_was_concealed);
#اگر_ختم

    // Need to position cursor again when on a TAB and when on a char with
    // virtual text.
    اگر (gchar_cursor() == TAB
#اگر_تعریف_شدہ FEAT_PROP_POPUP
	    || curbuf->b_has_textprop
#اگر_ختم
       )
	curwin->w_valid &= ~(VALID_WROW|VALID_WCOL|VALID_VIRTCOL);

    /*
     * Enable langmap or IME, indicated by 'iminsert'.
     * Note that IME may enabled/disabled without us noticing here, thus the
     * 'iminsert' value may not reflect what is actually used.  It is updated
     * when hitting <Esc>.
     */
    اگر (curbuf->b_p_iminsert == B_IMODE_LMAP)
	State |= منوال_LANGMAP;
#اگر_تعریف_شدہ HAVE_داخلہ_طریقہ
    im_سیٹ_فعال(curbuf->b_p_iminsert == B_IMODE_IM);
#اگر_ختم

    setmouse();
    clear_showcmd();
#اگر_تعریف_شدہ FEAT_RIGHTLEFT
    // there is no reverse replace mode
    revins_on = (State == منوال_داخل_کرو && p_ri);
    اگر (revins_on)
	undisplay_dollar();
    revins_chars = 0;
    revins_legal = 0;
    revins_scol = -1;
#اگر_ختم
    اگر (!p_ek)
    {
	MAY_WANT_مقصد_LOG_THIS;

	// Disable bracketed paste mode, we won't recognize the escape
	// sequences.
	برآمد_str(T_BD);

	// Disable modifyOtherKeys, keys with modifiers would cause exiting
	// Insert mode.
	برآمد_str_t_TE();
    }

    /*
     * Handle restarting Insert mode.
     * Don't do this for "CTRL-O ." (repeat an insert): In that case we get
     * here with something in the stuff buffer.
     */
    اگر (restart_edit != 0 && stuff_خالی_خانے())
    {
	/*
	 * After a paste we consider text typed to be part of the insert for
	 * the pasted text. You can backspace over the pasted text too.
	 */
	اگر (where_paste_started.lnum)
	    arrow_مستعمل = FALSE;
	ورنہ
	    arrow_مستعمل = TRUE;
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
	اگر (((ins_at_eol && curwin->w_cursor.lnum == o_lnum)
		    || curwin->w_curswant > curwin->w_virtcol)
		&& *(ptr = ml_get_curline() + curwin->w_cursor.ستون) != NUL)
	{
	    اگر (ptr[1] == NUL)
		++curwin->w_cursor.ستون;
	    ورنہ اگر (has_mbyte)
	    {
		i = (*mb_ptr2len)(ptr);
		اگر (ptr[i] == NUL)
		    curwin->w_cursor.ستون += i;
	    }
	}
	ins_at_eol = FALSE;
    }
    ورنہ
	arrow_مستعمل = FALSE;

    // we are in insert mode now, don't need to start it anymore
    need_آغاز_insertmode = FALSE;

    // Need to save the line for undo before inserting the first char.
    ins_need_undo = TRUE;

    where_paste_started.lnum = 0;
    can_cindent = TRUE;
#اگر_تعریف_شدہ FEAT_FOLDING
    // The cursor line is not in a closed fold, unless 'insertmode' is set or
    // restarting.
    اگر (!p_im && did_restart_edit == 0)
	foldOpenCursor();
#اگر_ختم

    /*
     * If 'showmode' is set, show the current (insert/replace/..) mode.
     * A warning message for changing a readonly file is given here, before
     * actually changing anything.  It's put after the mode, if any.
     */
    i = 0;
    اگر (p_smd && پیغام_silent == 0)
	i = showmode();

    اگر (!p_im && did_restart_edit == 0)
	change_warning(i == 0 ? 0 : i + 1);

#اگر_تعریف_شدہ CURSOR_SHAPE
    ui_cursor_shape();		// may show different cursor shape
#اگر_ختم
#اگر_تعریف_شدہ FEAT_DIGRAPHS
    do_digraph(-1);		// clear digraphs
#اگر_ختم

    /*
     * Get the current length of the redo buffer, those characters have to be
     * skipped if we want to get to the inserted characters.
     */
    string_T inserted = get_inserted();
    new_داخل_کرو_skip = (صحیح)inserted.لمبائی;
    اگر (inserted.string != تہی)
	vim_free(inserted.string);

    old_indent = 0;

    /*
     * Main loop in Insert mode: repeat until Insert mode is left.
     */
    برائے (;;)
    {
#اگر_تعریف_شدہ FEAT_RIGHTLEFT
	اگر (!revins_legal)
	    revins_scol = -1;	    // reset on illegal motions
	ورنہ
	    revins_legal = 0;
#اگر_ختم
	اگر (arrow_مستعمل)	    // don't repeat insert when arrow key used
	    گنتی = 0;

	اگر (update_Insstart_orig)
	    Insstart_orig = Insstart;

	اگر (stop_داخل_کرو_منوال && !ins_compl_فعال())
	{
	    // ":stopinsert" used or 'insertmode' reset
	    گنتی = 0;
	    جاؤ doESCkey;
	}

	// set curwin->w_curswant for next K_DOWN or K_UP
	اگر (!arrow_مستعمل)
	    curwin->w_سیٹ_curswant = true;

	// If there is no typeahead may check for timestamps (e.g., for when a
	// menu invoked a shell command).
	اگر (stuff_خالی_خانے())
	{
	    did_check_timestamps = FALSE;
	    اگر (need_check_timestamps)
		check_timestamps(FALSE);
	}

	/*
	 * When emsg() was called msg_scroll will have been set.
	 */
	پیغام_scroll = FALSE;

#اگر_تعریف_شدہ FEAT_GUI
	// When 'mousefocus' is set a mouse movement may have taken us to
	// another window.  "need_mouse_correct" may then be set because of an
	// autocommand.
	اگر (need_mouse_correct)
	    gui_mouse_correct();
#اگر_ختم

#اگر_تعریف_شدہ FEAT_FOLDING
	// Open fold at the cursor line, according to 'foldopen'.
	اگر (fdo_flags & FDO_داخل_کرو)
	    foldOpenCursor();
	// Close folds where the cursor isn't, according to 'foldclose'
	اگر (!char_avail())
	    foldCheckClose();
#اگر_ختم

#اگر_تعریف_شدہ FEAT_کام_CHANNEL
	اگر (bt_prompt(curbuf))
	{
	    init_prompt(cmdchar_todo);
	    cmdchar_todo = NUL;
	}
#اگر_ختم

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
	اگر (curbuf->b_mod_سیٹ
		&& curwin->w_p_wrap
		&& !curwin->w_p_sms
		&& !did_backspace
		&& curwin->w_topline == old_topline
#اگر_تعریف_شدہ FEAT_DIFF
		&& curwin->w_topfill == old_topfill
#اگر_ختم
		&& گنتی <= 1
		)
	{
	    mincol = curwin->w_wcol;
	    validate_cursor_ستون();

	    اگر (
#اگر_تعریف_شدہ FEAT_VARTABS
		curwin->w_wcol < mincol - tabstop_at(get_nolist_virtcol(),
						     curbuf->b_p_ts,
						     curbuf->b_p_vts_array,
						     FALSE)
#ورنہ
		(صحیح)curwin->w_wcol < mincol - curbuf->b_p_ts
#اگر_ختم
		    && curwin->w_wrow ==
				   curwin->w_اونچائی - 1 - get_scrolloff_قدر()
		    && (curwin->w_cursor.lnum != curwin->w_topline
#اگر_تعریف_شدہ FEAT_DIFF
			|| curwin->w_topfill > 0
#اگر_ختم
		    ))
	    {
#اگر_تعریف_شدہ FEAT_DIFF
		اگر (curwin->w_topfill > 0)
		    --curwin->w_topfill;
		ورنہ
#اگر_ختم
#اگر_تعریف_شدہ FEAT_FOLDING
		اگر (hasFolding(curwin->w_topline, تہی, &old_topline))
		    سیٹ_topline(curwin, old_topline + 1);
		ورنہ
#اگر_ختم
		    سیٹ_topline(curwin, curwin->w_topline + 1);
	    }
	}

	// May need to adjust w_topline to show the cursor.
	اگر (گنتی <= 1)
	    update_topline();

	did_backspace = FALSE;

	اگر (گنتی <= 1)
	    validate_cursor();		// may set must_redraw

	/*
	 * Redraw the display when no characters are waiting.
	 * Also shows mode, ruler and positions cursor.
	 */
	ins_redraw(TRUE);

	اگر (curwin->w_p_scb)
	    do_check_scrollbind(TRUE);

	اگر (curwin->w_p_crb)
	    do_check_cursorbind();
	اگر (گنتی <= 1)
	    update_curswant();
	old_topline = curwin->w_topline;
#اگر_تعریف_شدہ FEAT_DIFF
	old_topfill = curwin->w_topfill;
#اگر_ختم

#اگر_تعریف_شدہ USE_ON_FLY_SCROLL
	dont_scroll = FALSE;		// allow scrolling here
#اگر_ختم
	// May request the keyboard protocol state now.
	may_send_t_RK();

	/*
	 * Get a character for Insert mode.  Ignore K_IGNORE and K_NOP.
	 */
	اگر (c != K_CURSORHOLD && c != K_COMPLETE_DELAY)
	    lastc = c;		// remember the previous char for CTRL-D

	// After using CTRL-G U the next cursor key will not break undo.
	اگر (dont_sync_undo == MAYBE)
	    dont_sync_undo = TRUE;
	ورنہ
	    dont_sync_undo = FALSE;
	اگر (cmdchar == K_PS)
	    // Got here from normal mode when bracketed paste started.
	    c = K_PS;
	ورنہ
	{
	    // Trigger autocomplete when entering Insert mode, either directly
	    // or via change commands like 'ciw', 'cw', etc., before the first
	    // character is typed.
	    اگر (ins_just_started)
	    {
		ins_just_started = FALSE;
		اگر (ins_compl_has_autocomplete() && !char_avail()
			&& curwin->w_cursor.ستون > 0)
		{
		    c = char_before_cursor();
		    اگر (vim_isprintc(c))
		    {
			ins_compl_enable_autocomplete();
			ins_compl_arm_autostart();
			ins_compl_init_get_longest();
#اگر_تعریف_شدہ FEAT_RIGHTLEFT
			اگر (p_hkmap)
			    c = hkmap(c);		// Hebrew mode mapping
#اگر_ختم
			// Defer until the delay expires (K_COMPLETE_DELAY), or
			// trigger now when no delay is in effect.
			اگر (!ins_compl_arm_autocomplete_delay())
			    جاؤ docomplete;
		    }
		}
	    }

	    کرو
	    {
		c = محفوظ_vgetc();

		اگر (stop_داخل_کرو_منوال
#اگر_تعریف_شدہ FEAT_TERMINAL
			|| (c == K_IGNORE && رکن_use_loop())
#اگر_ختم
		   )
		{
		    // Insert mode ended, possibly from a callback, or a timer
		    // must have opened a terminal window.
		    اگر (c != K_IGNORE && c != K_NOP)
			vungetc(c);
		    گنتی = 0;

		    اگر (!bt_prompt(curwin->w_بفر)
#اگر_تعریف_شدہ FEAT_TERMINAL
			    && !bt_terminal(curwin->w_بفر)
#اگر_ختم
			    && stop_داخل_کرو_منوال)
			// :stopinsert command via callback or via server command
			nomove = FALSE;
		    ورنہ
			nomove = TRUE;
		    ins_compl_prep(ESC);
		    جاؤ doESCkey;
		}
	    } جب (c == K_IGNORE || c == K_NOP);
	}

	// Don't want K_CURSORHOLD for the second key, e.g., after CTRL-V.
	did_cursorhold = TRUE;
	اگر (c != K_CURSORHOLD && c != K_COMPLETE_DELAY)
	{
	    // Don't want delayed autocompletion from the previous key either.
	    ins_compl_clear_autocomplete_delay();
	    ins_compl_disarm_autostart();
	    // A completion already on screen goes on being what it was.
	    اگر (!ins_compl_فعال())
		ins_compl_disable_autocomplete();
	}

#اگر_تعریف_شدہ FEAT_RIGHTLEFT
	اگر (p_hkmap && کلید_Typed)
	    c = hkmap(c);		// Hebrew mode mapping
#اگر_ختم

	// If the window was made so small that nothing shows, make it at least
	// one line and one column when typing.
	اگر (کلید_Typed && !کلید_Stuffed)
	    win_ensure_سائز();

	/*
	 * Special handling of keys while the popup menu is visible or wanted
	 * and the cursor is still in the completed word.  Only when there is
	 * a match, skip this when no matches were found.
	 */
	اگر (ins_compl_فعال() && curwin->w_cursor.ستون >= ins_compl_ستون()
		&& ins_compl_has_shown_مطابقت() && pum_wanted())
	{
	    // BS: Delete one character from "compl_leader".
	    اگر ((c == K_BS || c == Ctrl_H)
			&& curwin->w_cursor.ستون > ins_compl_ستون()
			&& (c = ins_compl_bs()) == NUL)
		جاری;

	    // When no match was selected or it was edited.
	    اگر (!ins_compl_مستعمل_مطابقت())
	    {
		// CTRL-L: Add one character from the current match to
		// "compl_leader".  Except when at the original match and
		// there is nothing to add, CTRL-L works like CTRL-P then.
		اگر (c == Ctrl_L
			&& (!ctrl_x_منوال_سطر_or_eval()
			    || ins_compl_long_shown_مطابقت()))
		{
		    ins_compl_addfrommatch();
		    جاری;
		}

		// A non-white character that fits in with the current
		// completion: Add to "compl_leader".
		اگر (ins_compl_accept_char(c))
		{
#اگر defined(FEAT_EVAL)
		    // Trigger InsertCharPre.
		    char_u *str = do_داخل_کرو_char_pre(c);
		    char_u *p;

		    اگر (str != تہی)
		    {
			برائے (p = str; *p != NUL; MB_PTR_ADV(p))
			    ins_compl_addleader(PTR2CHAR(p));
			vim_free(str);
		    }
		    ورنہ
#اگر_ختم
			ins_compl_addleader(c);
		    جاری;
		}

		// Pressing CTRL-Y selects the current match.  When
		// ins_compl_enter_selects() is set the Enter key does the
		// same.
		اگر ((c == Ctrl_Y || (ins_compl_enter_selects()
				    && (c == CAR || c == K_KENTER || c == NL)))
			&& stop_arrow() == OK)
		{
		    ins_compl_delete();
		    اگر (ins_compl_preinsert_longest()
			    && !ins_compl_is_مطابقت_selected())
		    {
			ins_compl_داخل_کرو(FALSE, TRUE);
			ins_compl_init_get_longest();
			جاری;
		    }
		    ورنہ
			ins_compl_داخل_کرو(FALSE, FALSE);
		}
		// Delete preinserted text when typing special chars
		ورنہ اگر (IS_WHITE_NL_OR_NUL(c) && ins_compl_preinsert_effect())
		    ins_compl_delete();
	    }
	}

	// Prepare for or stop CTRL-X mode.  This doesn't do completion, but
	// it does fix up the text when finishing completion.
	ins_compl_init_get_longest();
	اگر (ins_compl_prep(c))
	    جاری;

	// CTRL-\ CTRL-N goes to Normal mode,
	// CTRL-\ CTRL-G goes to mode selected with 'insertmode',
	// CTRL-\ CTRL-O is like CTRL-O but without moving the cursor.
	اگر (c == Ctrl_BSL)
	{
	    // may need to redraw when no more chars available now
	    ins_redraw(FALSE);
	    ++no_mapping;
	    ++allow_کلیدیں;
	    c = plain_vgetc();
	    --no_mapping;
	    --allow_کلیدیں;
	    اگر (c != Ctrl_N && c != Ctrl_G && c != Ctrl_O)
	    {
		// it's something else
		vungetc(c);
		c = Ctrl_BSL;
	    }
	    ورنہ اگر (c == Ctrl_G && p_im)
		جاری;
	    ورنہ
	    {
		اگر (c == Ctrl_O)
		{
		    ins_ctrl_o();
		    ins_at_eol = FALSE;	// cursor keeps its column
		    nomove = TRUE;
		}
		گنتی = 0;
		جاؤ doESCkey;
	    }
	}

#اگر_تعریف_شدہ FEAT_DIGRAPHS
	c = do_digraph(c);
#اگر_ختم

	اگر ((c == Ctrl_V || c == Ctrl_Q) && ctrl_x_منوال_cmdline())
	    جاؤ docomplete;
	اگر (c == Ctrl_V || c == Ctrl_Q)
	{
	    ins_ctrl_v();
	    c = Ctrl_V;	// pretend CTRL-V is last typed character
	    جاری;
	}

	اگر (cindent_on() && ctrl_x_منوال_none())
	{
	    // A key name preceded by a bang means this key is not to be
	    // inserted.  Skip ahead to the re-indenting below.
	    // A key name preceded by a star means that indenting has to be
	    // done before inserting the key.
	    سطر_is_white = inindent(0);
	    اگر (اندر_cinkeys(c, '!', سطر_is_white))
		جاؤ force_cindent;
	    اگر (can_cindent && اندر_cinkeys(c, '*', سطر_is_white)
							&& stop_arrow() == OK)
		do_c_اظہاریہ_indent();
	}

#اگر_تعریف_شدہ FEAT_RIGHTLEFT
	اگر (curwin->w_p_rl)
	    انتخاب (c)
	    {
		حالت K_بایاں:	c = K_دایاں; توڑو;
		حالت K_S_بایاں:	c = K_S_دایاں; توڑو;
		حالت K_C_بایاں:	c = K_C_دایاں; توڑو;
		حالت K_دایاں:	c = K_بایاں; توڑو;
		حالت K_S_دایاں: c = K_S_بایاں; توڑو;
		حالت K_C_دایاں: c = K_C_بایاں; توڑو;
	    }
#اگر_ختم

	/*
	 * If 'keymodel' contains "startsel", may start selection.  If it
	 * does, a CTRL-O and c will be stuffed, we need to get these
	 * characters.
	 */
	اگر (ins_آغاز_select(c))
	    جاری;

	/*
	 * The big switch to handle a character in insert mode.
	 */
	انتخاب (c)
	{
	حالت ESC:	// End input mode
	    اگر (echeck_abbr(ESC + ABBR_OFF))
		توڑو;
	    // FALLTHROUGH

	حالت Ctrl_C:	// End input mode
	    اگر (c == Ctrl_C && cmdwin_type != 0)
	    {
		// Close the cmdline window.
		cmdwin_نتیجہ = K_IGNORE;
		got_int = FALSE; // don't stop executing autocommands et al.
		nomove = TRUE;
		جاؤ doESCkey;
	    }
#اگر_تعریف_شدہ FEAT_کام_CHANNEL
	    اگر (c == Ctrl_C && bt_prompt(curbuf))
	    {
		اگر (invoke_prompt_مداخلت())
		{
		    اگر (!bt_prompt(curbuf))
			// buffer changed to a non-prompt buffer, get out of
			// Insert mode
			جاؤ doESCkey;
		    توڑو;
		}
	    }
#اگر_ختم

#اگر_تعریف_شدہ UNIX
do_intr:
#اگر_ختم
	    // when 'insertmode' set, and not halfway a mapping, don't leave
	    // Insert mode
	    اگر (goto_im())
	    {
		اگر (got_int)
		{
		    (خالی)vgetc();		// flush all buffers
		    got_int = FALSE;
		}
		ورنہ
		    vim_beep(BO_IM);
		توڑو;
	    }
doESCkey:
	    // Drop a pending autocomplete so it does not outlive Insert mode.
	    ins_compl_clear_autocomplete_delay();
	    /*
	     * This is the ONLY return from edit()!
	     */
	    // Always update o_lnum, so that a "CTRL-O ." that adds a line
	    // still puts the cursor back after the inserted text.
	    اگر (ins_at_eol && gchar_cursor() == NUL)
		o_lnum = curwin->w_cursor.lnum;

	    اگر (ins_esc(&گنتی, cmdchar, nomove))
	    {
		// When CTRL-C was typed got_int will be set, with the result
		// that the autocommands won't be executed. When mapped got_int
		// is not set, but let's keep the behavior the same.
		اگر (cmdchar != 'r' && cmdchar != 'v' && c != Ctrl_C)
		    ins_apply_autocmds(EVENT_INSERTLEAVE);
		did_cursorhold = FALSE;

		// ins_redraw() triggers TextChangedI only when no characters
		// are in the typeahead buffer, so reset curbuf->b_last_changedtick only
		// if the TextChangedI was not blocked by char_avail() (e.g. using :norm!)
		// and the TextChangedI autocommand has been triggered.
		اگر (!char_avail() && curbuf->b_آخری_changedtick_i == CHANGEDTICK(curbuf))
		    curbuf->b_آخری_changedtick = CHANGEDTICK(curbuf);
		واپس (c == Ctrl_O);
	    }
	    جاری;

	حالت Ctrl_Z:	// suspend when 'insertmode' set
	    اگر (!p_im)
		جاؤ normalchar;	// insert CTRL-Z as normal char
	    do_cmdline_cmd((char_u *)"stop");
#اگر_تعریف_شدہ CURSOR_SHAPE
	    ui_cursor_shape();		// may need to update cursor shape
#اگر_ختم
	    جاری;

	حالت Ctrl_O:	// execute one command
#اگر_تعریف_شدہ FEAT_COMPL_FUNC
	    اگر (ctrl_x_منوال_omni())
		جاؤ docomplete;
#اگر_ختم
	    اگر (echeck_abbr(Ctrl_O + ABBR_OFF))
		توڑو;
	    ins_ctrl_o();

	    // don't move the cursor left when 'virtualedit' has "onemore".
	    اگر (get_ve_flags() & VE_ONEMORE)
	    {
		ins_at_eol = FALSE;
		nomove = TRUE;
	    }
	    گنتی = 0;
	    جاؤ doESCkey;

	حالت K_INS:	// toggle insert/replace mode
	حالت K_KINS:
	    ins_داخل_کرو(replaceState);
	    توڑو;

	حالت K_SELECT:	// end of Select mode mapping - ignore
	    توڑو;

	حالت K_HELP:	// Help key works like <ESC> <Help>
	حالت K_F1:
	حالت K_XF1:
	    stuffcharReadbuff(K_HELP);
	    اگر (p_im)
		need_آغاز_insertmode = TRUE;
	    جاؤ doESCkey;

#اگر_تعریف_شدہ FEAT_NETBEANS_INTG
	حالت K_F21:	// NetBeans command
	    ++no_mapping;		// don't map the next key hits
	    i = plain_vgetc();
	    --no_mapping;
	    netbeans_keycommand(i);
	    توڑو;
#اگر_ختم

	حالت K_ZERO:	// Insert the previously inserted text.
	حالت NUL:
	حالت Ctrl_A:
	    // For ^@ the trailing ESC will end the insert, unless there is an
	    // error.
	    اگر (stuff_inserted(NUL, 1L, (c == Ctrl_A)) == FAIL
						   && c != Ctrl_A && !p_im)
		جاؤ doESCkey;		// quit insert mode
	    inserted_space = FALSE;
	    توڑو;

	حالت Ctrl_R:	// insert the contents of a register
	    اگر (ctrl_x_منوال_رجسٹر() && !ins_compl_فعال())
		جاؤ docomplete;
	    ins_reg();
	    auto_format(FALSE, TRUE);
	    inserted_space = FALSE;
	    توڑو;

	حالت Ctrl_G:	// commands starting with CTRL-G
	    ins_ctrl_g();
	    توڑو;

	حالت Ctrl_HAT:	// switch input mode and/or langmap
	    ins_ctrl_hat();
	    توڑو;

#اگر_تعریف_شدہ FEAT_RIGHTLEFT
	حالت Ctrl__:	// switch between languages
	    اگر (!p_ari)
		جاؤ normalchar;
	    ins_ctrl_();
	    توڑو;
#اگر_ختم

	حالت Ctrl_D:	// Make indent one shiftwidth smaller.
#اگر defined(FEAT_ڈھونڈو_ID)
	    اگر (ctrl_x_منوال_راستہ_متعین_کرتا())
		جاؤ docomplete;
#اگر_ختم
	    // FALLTHROUGH

	حالت Ctrl_T:	// Make indent one shiftwidth greater.
	    اگر (c == Ctrl_T && ctrl_x_منوال_thesaurus())
	    {
		اگر (has_compl_option(FALSE))
		    جاؤ docomplete;
		توڑو;
	    }

	    ins_سرکاؤ(c, lastc);
	    auto_format(FALSE, TRUE);
	    inserted_space = FALSE;
	    توڑو;

	حالت K_DEL:	// delete character under the cursor
	حالت K_KDEL:
	    ins_del();
	    auto_format(FALSE, TRUE);
	    توڑو;

	حالت K_BS:	// delete character before the cursor
	حالت K_S_BS:
	حالت Ctrl_H:
	    did_backspace = ins_bs(c, BACKSPACE_CHAR, &inserted_space);
	    auto_format(FALSE, TRUE);
	    اگر (did_backspace)
		MAY_TRIGGER_AUTOCOMPLETE(c);
	    توڑو;

	حالت Ctrl_W:	// delete word before the cursor
#اگر_تعریف_شدہ FEAT_کام_CHANNEL
	    اگر (bt_prompt(curbuf) && (mod_نقاب & MOD_نقاب_سرکاؤ) == 0)
	    {
		// In a prompt window CTRL-W is used for window commands.
		// Use Shift-CTRL-W to delete a word.
		stuffcharReadbuff(Ctrl_W);
		restart_edit = 'A';
		nomove = TRUE;
		گنتی = 0;
		جاؤ doESCkey;
	    }
#اگر_ختم
	    did_backspace = ins_bs(c, BACKSPACE_لفظ, &inserted_space);
	    auto_format(FALSE, TRUE);
	    اگر (did_backspace)
		MAY_TRIGGER_AUTOCOMPLETE(c);
	    توڑو;

	حالت Ctrl_U:	// delete all inserted text in current line
#اگر_تعریف_شدہ FEAT_COMPL_FUNC
	    // CTRL-X CTRL-U completes with 'completefunc'.
	    اگر (ctrl_x_منوال_function())
		جاؤ docomplete;
#اگر_ختم
	    did_backspace = ins_bs(c, BACKSPACE_سطر, &inserted_space);
	    auto_format(FALSE, TRUE);
	    inserted_space = FALSE;
	    اگر (did_backspace)
		MAY_TRIGGER_AUTOCOMPLETE(c);
	    توڑو;

	حالت K_LEFTMOUSE:   // mouse keys
	حالت K_LEFTMOUSE_NM:
	حالت K_LEFTDRAG:
	حالت K_LEFTRELEASE:
	حالت K_LEFTRELEASE_NM:
	حالت K_MOUSEMOVE:
	حالت K_MIDDLEMOUSE:
	حالت K_MIDDLEDRAG:
	حالت K_MIDDLERELEASE:
	حالت K_RIGHTMOUSE:
	حالت K_RIGHTDRAG:
	حالت K_RIGHTRELEASE:
	حالت K_X1MOUSE:
	حالت K_X1DRAG:
	حالت K_X1RELEASE:
	حالت K_X2MOUSE:
	حالت K_X2DRAG:
	حالت K_X2RELEASE:
	    ins_mouse(c);
	    توڑو;

	حالت K_MOUSEDOWN: // Default action for scroll wheel up: scroll up
	    ins_mousescroll(MSCR_نیچے_کو);
	    توڑو;

	حالت K_MOUSEUP:	// Default action for scroll wheel down: scroll down
	    ins_mousescroll(MSCR_اوپر);
	    توڑو;

	حالت K_MOUSELEFT: // Scroll wheel left
	    ins_mousescroll(MSCR_بایاں);
	    توڑو;

	حالت K_MOUSERIGHT: // Scroll wheel right
	    ins_mousescroll(MSCR_دایاں);
	    توڑو;

	حالت K_PS:
	    bracketed_paste(PASTE_داخل_کرو, FALSE, تہی);
	    اگر (cmdchar == K_PS)
		// invoked from normal mode, bail out
		جاؤ doESCkey;
	    توڑو;
	حالت K_PE:
	    // Got K_PE without K_PS, ignore.
	    توڑو;

#اگر_تعریف_شدہ FEAT_GUI_TABLINE
	حالت K_TABLINE:
	حالت K_TABMENU:
	    ins_tabline(c);
	    توڑو;
#اگر_ختم

	حالت K_IGNORE:	// Something mapped to nothing
	    توڑو;

	حالت K_COMMAND:		    // <Cmd>command<CR>
	حالت K_SCRIPT_COMMAND:	    // <ScriptCmd>command<CR>
	    {
		bufref_T    save_curbuf;
		varnumber_T tick = CHANGEDTICK(curbuf);

		سیٹ_bufref(&save_curbuf, curbuf);
		do_cmdkey_command(c, 0);

#اگر_تعریف_شدہ FEAT_TERMINAL
		اگر (رکن_use_loop())
		    // Started a terminal that gets the input, exit Insert mode.
		    جاؤ doESCkey;
#اگر_ختم
		اگر (curbuf->b_u_synced
			|| (bufref_valid(&save_curbuf)
			    && curbuf == save_curbuf.br_buf
			    && tick != CHANGEDTICK(curbuf)))
		{
		    // The command synced undo or changed this buffer.
		    // Save the cursor line before the next typed edit.
		    ins_need_undo = TRUE;
		}
	    }
	    توڑو;

	حالت K_CURSORHOLD:	// Didn't type something for a while.
	    ins_apply_autocmds(EVENT_CURSORHOLDI);
	    did_cursorhold = TRUE;
	    // If CTRL-G U was used apply it to the next typed key.
	    اگر (dont_sync_undo == TRUE)
		dont_sync_undo = MAYBE;
	    توڑو;

	حالت K_COMPLETE_DELAY:	// 'autocompletedelay' expired
	    // If CTRL-G U was used apply it to the next typed key.
	    اگر (dont_sync_undo == TRUE)
		dont_sync_undo = MAYBE;
	    ins_compl_clear_autocomplete_delay();
	    اگر (!ins_compl_has_autocomplete() || char_avail()
		    || curwin->w_cursor.ستون == 0)
		توڑو;
	    c = char_before_cursor();
	    اگر (!vim_isprintc(c))
		توڑو;
	    // The completion may have been cleared while waiting, so re-enable
	    // autocomplete to match a zero delay.
	    ins_compl_enable_autocomplete();
	    ins_compl_arm_autostart();
	    جاؤ docomplete;

#اگر_تعریف_شدہ FEAT_GUI_MSWIN
	    // On MS-Windows ignore <M-F4>, we get it when closing the window
	    // was cancelled.
	حالت K_F4:
	    اگر (mod_نقاب != MOD_نقاب_ALT)
		جاؤ normalchar;
	    توڑو;
#اگر_ختم

#اگر_تعریف_شدہ FEAT_GUI
	حالت K_VER_SCROLLBAR:
	    ins_scroll();
	    توڑو;

	حالت K_HOR_SCROLLBAR:
	    ins_horscroll();
	    توڑو;
#اگر_ختم

	حالت K_HOME:	// <Home>
	حالت K_KHOME:
	حالت K_S_HOME:
	حالت K_C_HOME:
	    ins_home(c);
	    توڑو;

	حالت K_انتہا:	// <End>
	حالت K_KEND:
	حالت K_S_انتہا:
	حالت K_C_انتہا:
	    ins_انتہا(c);
	    توڑو;

	حالت K_بایاں:	// <Left>
	    اگر (mod_نقاب & (MOD_نقاب_سرکاؤ|MOD_نقاب_CTRL))
		ins_s_بایاں();
	    ورنہ
		ins_بایاں();
	    توڑو;

	حالت K_S_بایاں:	// <S-Left>
	حالت K_C_بایاں:
	    ins_s_بایاں();
	    توڑو;

	حالت K_دایاں:	// <Right>
	    اگر (mod_نقاب & (MOD_نقاب_سرکاؤ|MOD_نقاب_CTRL))
		ins_s_دایاں();
	    ورنہ
		ins_دایاں();
	    توڑو;

	حالت K_S_دایاں:	// <S-Right>
	حالت K_C_دایاں:
	    ins_s_دایاں();
	    توڑو;

	حالت K_اوپر:	// <Up>
	    اگر (pum_visible())
		جاؤ docomplete;
	    اگر (mod_نقاب & MOD_نقاب_سرکاؤ)
		ins_pageup();
	    ورنہ
		ins_اوپر(FALSE);
	    توڑو;

	حالت K_S_اوپر:	// <S-Up>
	حالت K_PAGEUP:
	حالت K_KPAGEUP:
	    اگر (pum_visible())
	    {
#اگر_تعریف_شدہ FEAT_PROP_POPUP
		// CTRL-SHIFT-<Up> scrolls the info popup up a line,
		// CTRL-SHIFT-<PageUp> a page.  Shift is folded into K_S_UP but
		// stays in mod_mask for PageUp, hence the asymmetric check.
		اگر (c == K_S_اوپر ? (mod_نقاب & MOD_نقاب_CTRL)
			: ((mod_نقاب & MOD_نقاب_CTRL)
			    && (mod_نقاب & MOD_نقاب_سرکاؤ)))
		{
		    popup_scroll_info(-1, c != K_S_اوپر);
		    توڑو;
		}
#اگر_ختم
		جاؤ docomplete;
	    }
	    ins_pageup();
	    توڑو;

	حالت K_نیچے_کو:	// <Down>
	    اگر (pum_visible())
		جاؤ docomplete;
	    اگر (mod_نقاب & MOD_نقاب_سرکاؤ)
		ins_pagedown();
	    ورنہ
		ins_نیچے_کو(FALSE);
	    توڑو;

	حالت K_S_نیچے_کو:	// <S-Down>
	حالت K_PAGEDOWN:
	حالت K_KPAGEDOWN:
	    اگر (pum_visible())
	    {
#اگر_تعریف_شدہ FEAT_PROP_POPUP
		// CTRL-SHIFT-<Down>/<PageDown> scroll the info popup down.
		اگر (c == K_S_نیچے_کو ? (mod_نقاب & MOD_نقاب_CTRL)
			: ((mod_نقاب & MOD_نقاب_CTRL)
			    && (mod_نقاب & MOD_نقاب_سرکاؤ)))
		{
		    popup_scroll_info(1, c != K_S_نیچے_کو);
		    توڑو;
		}
#اگر_ختم
		جاؤ docomplete;
	    }
	    ins_pagedown();
	    توڑو;

#اگر_تعریف_شدہ FEAT_DND
	حالت K_DROP:	// drag-n-drop event
	    ins_drop();
	    توڑو;
#اگر_ختم

	حالت K_S_TAB:	// When not mapped, use like a normal TAB
	    c = TAB;
	    // FALLTHROUGH

	حالت TAB:	// TAB or Complete patterns along path
#اگر defined(FEAT_ڈھونڈو_ID)
	    اگر (ctrl_x_منوال_راستہ_patterns())
		جاؤ docomplete;
#اگر_ختم
	    inserted_space = FALSE;
	    اگر (ins_tab())
		جاؤ normalchar;	// insert TAB as a normal char
	    auto_format(FALSE, TRUE);
	    توڑو;

	حالت K_KENTER:	// <Enter>
	    c = CAR;
	    // FALLTHROUGH
	حالت CAR:
	حالت NL:
#اگر defined(FEAT_QUICKFIX)
	    // In a quickfix window a <CR> jumps to the error under the
	    // cursor.
	    اگر (bt_quickfix(curbuf) && c == CAR)
	    {
		اگر (curwin->w_llist_ref == تہی)    // quickfix window
		    do_cmdline_cmd((char_u *)".cc");
		ورنہ				    // location list window
		    do_cmdline_cmd((char_u *)".ll");
		توڑو;
	    }
#اگر_ختم
	    اگر (cmdwin_type != 0)
	    {
		// Execute the command in the cmdline window.
		cmdwin_نتیجہ = CAR;
		جاؤ doESCkey;
	    }
#اگر_تعریف_شدہ FEAT_کام_CHANNEL
	    اگر (bt_prompt(curbuf))
	    {
		invoke_prompt_callback();
		اگر (!bt_prompt(curbuf))
		    // buffer changed to a non-prompt buffer, get out of
		    // Insert mode
		    جاؤ doESCkey;
		توڑو;
	    }
#اگر_ختم
	    اگر (ins_eol(c) == FAIL && !p_im)
		جاؤ doESCkey;	    // out of memory
	    auto_format(FALSE, FALSE);
	    inserted_space = FALSE;
	    توڑو;

	حالت Ctrl_K:	    // digraph or keyword completion
	    اگر (ctrl_x_منوال_dictionary())
	    {
		اگر (has_compl_option(TRUE))
		    جاؤ docomplete;
		توڑو;
	    }
#اگر_تعریف_شدہ FEAT_DIGRAPHS
	    c = ins_digraph();
	    اگر (c == NUL)
		توڑو;
#اگر_ختم
	    جاؤ normalchar;

	حالت Ctrl_X:	// Enter CTRL-X mode
	    ins_ctrl_x();
	    توڑو;

	حالت Ctrl_RSB:	// Tag name completion after ^X
	    اگر (!ctrl_x_منوال_لیبل_فہرست())
		جاؤ normalchar;
	    جاؤ docomplete;

	حالت Ctrl_F:	// File name completion after ^X
	    اگر (!ctrl_x_منوال_files())
		جاؤ normalchar;
	    جاؤ docomplete;

	حالت 's':	// Spelling completion after ^X
	حالت Ctrl_S:
	    اگر (!ctrl_x_منوال_spell())
		جاؤ normalchar;
	    جاؤ docomplete;

	حالت Ctrl_L:	// Whole line completion after ^X
	    اگر (!ctrl_x_منوال_پورا_سطر())
	    {
		// CTRL-L with 'insertmode' set: Leave Insert mode
		اگر (p_im)
		{
		    اگر (echeck_abbr(Ctrl_L + ABBR_OFF))
			توڑو;
		    جاؤ doESCkey;
		}
		جاؤ normalchar;
	    }
	    // FALLTHROUGH

	حالت Ctrl_P:	// Do previous/next pattern completion
	حالت Ctrl_N:
#اگر_تعریف_شدہ FEAT_PROP_POPUP
	    // CTRL-SHIFT-P/N scroll the info popup one line.
	    اگر (pum_visible() && (mod_نقاب & MOD_نقاب_سرکاؤ)
		    && (c == Ctrl_P || c == Ctrl_N))
	    {
		popup_scroll_info(c == Ctrl_P ? -1 : 1, false);
		توڑو;
	    }
#اگر_ختم
	    // if 'complete' is empty then plain ^P is no longer special,
	    // but it is under other ^X modes
	    اگر (*curbuf->b_p_cpt == NUL
		    && (ctrl_x_منوال_معمول() || ctrl_x_منوال_پورا_سطر())
		    && !compl_حالت_local())
		جاؤ normalchar;

docomplete:
	    ins_compl_clear_autocomplete_delay();
	    compl_busy = TRUE;
#اگر_تعریف_شدہ FEAT_FOLDING
	    disable_fold_update++;  // don't redraw folds here
#اگر_ختم
	    اگر (ins_complete(c, TRUE) == FAIL)
		compl_حالت_clear();
#اگر_تعریف_شدہ FEAT_FOLDING
	    disable_fold_update--;
#اگر_ختم
	    compl_busy = FALSE;
	    can_si = may_do_si(); // allow smartindenting
	    توڑو;

	حالت Ctrl_Y:	// copy from previous line or scroll down
	حالت Ctrl_E:	// copy from next line	   or scroll up
	    c = ins_ctrl_ey(c);
	    توڑو;

	طےشدہ:
#اگر_تعریف_شدہ UNIX
	    اگر (c == intr_char)		// special interrupt char
		جاؤ do_intr;
#اگر_ختم

normalchar:
	    /*
	     * Insert a normal character.
	     */
#اگر defined(FEAT_EVAL)
	    اگر (!p_paste)
	    {
		// Trigger InsertCharPre.
		char_u *str = do_داخل_کرو_char_pre(c);
		char_u *p;

		اگر (str != تہی)
		{
		    اگر (*str != NUL && stop_arrow() != FAIL)
		    {
			// Insert the new value of v:char literally.
			برائے (p = str; *p != NUL; MB_PTR_ADV(p))
			{
			    c = PTR2CHAR(p);
			    اگر (c == CAR || c == K_KENTER || c == NL)
				ins_eol(c);
			    ورنہ
				ins_char(c);
			}
			Append_مقصد_Redobuff_Lit(str, -1);
		    }
		    vim_free(str);
		    c = NUL;
		}

		// If the new value is already inserted or an empty string
		// then don't insert any character.
		اگر (c == NUL)
		    توڑو;
	    }
#اگر_ختم
	    // Try to perform smart-indenting.
	    ins_try_si(c);

	    اگر (c == ' ')
	    {
		inserted_space = TRUE;
		اگر (inindent(0))
		    can_cindent = FALSE;
		اگر (Insstart_blank_vcol == MAXCOL
			&& curwin->w_cursor.lnum == Insstart.lnum)
		    Insstart_blank_vcol = get_nolist_virtcol();
	    }

	    // Insert a normal character and check for abbreviations on a
	    // special character.  Let CTRL-] expand abbreviations without
	    // inserting it.
	    اگر (vim_iswordc(c) || (!echeck_abbr(
			// Add ABBR_OFF for characters above 0x100, this is
			// what check_abbr() expects.
				(has_mbyte && c >= 0x100) ? (c + ABBR_OFF) : c)
			&& c != Ctrl_RSB))
	    {
		داخل_کرو_special(c, FALSE, FALSE);
#اگر_تعریف_شدہ FEAT_RIGHTLEFT
		revins_legal++;
		revins_chars++;
#اگر_ختم
	    }

	    auto_format(FALSE, TRUE);

#اگر_تعریف_شدہ FEAT_FOLDING
	    // When inserting a character the cursor line must never be in a
	    // closed fold.
	    foldOpenCursor();
#اگر_ختم
	    // Trigger autocompletion
	    اگر (ins_compl_has_autocomplete() && !char_avail()
		    && vim_isprintc(c))
		TRIGGER_AUTOCOMPLETE();

	    توڑو;
	}   // end of switch (c)

	// If typed something may trigger CursorHoldI again; K_COMPLETE_DELAY is
	// injected, not typed.
	اگر (c != K_CURSORHOLD && c != K_COMPLETE_DELAY
#اگر_تعریف_شدہ FEAT_COMPL_FUNC
		// but not in CTRL-X mode, a script can't restore the state
		&& ctrl_x_منوال_معمول()
#اگر_ختم
	       )
	    did_cursorhold = FALSE;

	// Check if we need to cancel completion mode because the window
	// or tab page was changed
	اگر (ins_compl_فعال() && !ins_compl_win_فعال(curwin))
	    ins_compl_cancel();

	// If the cursor was moved we didn't just insert a space
	اگر (arrow_مستعمل)
	    inserted_space = FALSE;

	اگر (can_cindent && cindent_on() && ctrl_x_منوال_معمول())
	{
force_cindent:
	    /*
	     * Indent now if a key was typed that is in 'cinkeys'.
	     */
	    اگر (اندر_cinkeys(c, ' ', سطر_is_white))
	    {
		اگر (stop_arrow() == OK)
		    // re-indent the current line
		    do_c_اظہاریہ_indent();
	    }
	}

    }	// for (;;)
    // NOTREACHED
}

    صحیح
ins_need_undo_get(خالی)
{
    واپس ins_need_undo;
}

/*
 * Redraw for Insert mode.
 * This is postponed until getting the next character to make '$' in the 'cpo'
 * option work correctly.
 * Only redraw when there are no characters available.  This speeds up
 * inserting sequences of characters (e.g., for CTRL-R).
 */
    خالی
ins_redraw(صحیح ready)	    // not busy with something
{
#اگر_تعریف_شدہ FEAT_CONCEAL
    linenr_T	conceal_old_cursor_سطر = 0;
    linenr_T	conceal_new_cursor_سطر = 0;
    صحیح		conceal_update_سطریں = FALSE;
#اگر_ختم

    اگر (char_avail())
	واپس;

    // Trigger CursorMoved if the cursor moved.  Not when the popup menu is
    // visible, the command might delete it.
    اگر (ready && (has_cursormovedI()
#اگر_تعریف_شدہ FEAT_PROP_POPUP
		|| popup_visible
#اگر_ختم
#اگر defined(FEAT_CONCEAL)
		|| curwin->w_p_cole > 0
#اگر_ختم
		)
	    && !EQUAL_POS(آخری_cursormoved, curwin->w_cursor)
	    && !pum_visible())
    {
#اگر_تعریف_شدہ FEAT_SYN_HL
	// Need to update the screen first, to make sure syntax
	// highlighting is correct after making a change (e.g., inserting
	// a "(".  The autocommand may also require a redraw, so it's done
	// again below, unfortunately.
	اگر (syntax_present(curwin) && must_redraw)
	    update_screen(0);
#اگر_ختم
	اگر (has_cursormovedI())
	{
	    // Make sure curswant is correct, an autocommand may call
	    // getcurpos().
	    update_curswant();
	    ins_apply_autocmds(EVENT_CURSORMOVEDI);
	}
#اگر_تعریف_شدہ FEAT_PROP_POPUP
	اگر (popup_visible)
	    popup_check_cursor_pos();
#اگر_ختم
#اگر_تعریف_شدہ FEAT_CONCEAL
	اگر (curwin->w_p_cole > 0)
	{
	    conceal_old_cursor_سطر = آخری_cursormoved.lnum;
	    conceal_new_cursor_سطر = curwin->w_cursor.lnum;
	    conceal_update_سطریں = TRUE;
	}
#اگر_ختم
	آخری_cursormoved = curwin->w_cursor;
    }

    // Trigger TextChangedI if b_changedtick_i differs.
    اگر (ready && has_textchangedI()
	    && curbuf->b_آخری_changedtick_i != CHANGEDTICK(curbuf)
	    && !pum_visible())
    {
	aco_save_T	aco;
	varnumber_T	tick = CHANGEDTICK(curbuf);

	// Save and restore curwin and curbuf, in case the autocmd changes
	// them.
	aucmd_prepbuf(&aco, curbuf);
	apply_autocmds(EVENT_TEXTCHANGEDI, تہی, تہی, FALSE, curbuf);
	aucmd_restbuf(&aco);
	curbuf->b_آخری_changedtick_i = CHANGEDTICK(curbuf);
	اگر (tick != CHANGEDTICK(curbuf))  // see ins_apply_autocmds()
	    u_save(curwin->w_cursor.lnum,
					(linenr_T)(curwin->w_cursor.lnum + 1));
    }

    // Trigger TextChangedP if b_changedtick_pum differs. When the popupmenu
    // closes TextChangedI will need to trigger for backwards compatibility,
    // thus use different b_last_changedtick* variables.
    اگر (ready && has_textchangedP()
	    && curbuf->b_آخری_changedtick_pum != CHANGEDTICK(curbuf)
	    && pum_visible())
    {
	aco_save_T	aco;
	varnumber_T	tick = CHANGEDTICK(curbuf);

	// Save and restore curwin and curbuf, in case the autocmd changes
	// them.
	aucmd_prepbuf(&aco, curbuf);
	apply_autocmds(EVENT_TEXTCHANGEDP, تہی, تہی, FALSE, curbuf);
	aucmd_restbuf(&aco);
	curbuf->b_آخری_changedtick_pum = CHANGEDTICK(curbuf);
	اگر (tick != CHANGEDTICK(curbuf))  // see ins_apply_autocmds()
	    u_save(curwin->w_cursor.lnum,
					(linenr_T)(curwin->w_cursor.lnum + 1));
    }

    اگر (ready)
	may_trigger_win_scrolled_resized();

    // Trigger SafeState if nothing is pending.
    may_trigger_safestate(ready
	    && !ins_compl_فعال()
	    && !pum_visible());

#اگر defined(FEAT_CONCEAL)
    اگر ((conceal_update_سطریں
	    && (conceal_old_cursor_سطر != conceal_new_cursor_سطر
		|| conceal_cursor_سطر(curwin)))
	    || need_cursor_سطر_redraw)
    {
	اگر (conceal_old_cursor_سطر != conceal_new_cursor_سطر)
	    redrawWinline(curwin, conceal_old_cursor_سطر);
	redrawWinline(curwin, conceal_new_cursor_سطر == 0
			    ? curwin->w_cursor.lnum : conceal_new_cursor_سطر);
	curwin->w_valid &= ~VALID_CROW;
	need_cursor_سطر_redraw = FALSE;
    }
#اگر_ختم
    اگر (must_redraw)
	update_screen(0);
    ورنہ اگر (clear_cmdline || redraw_cmdline)
	showmode();		// clear cmdline and show mode
    showruler(FALSE);
    setcursor();
    emsg_on_display = FALSE;	// may remove error message now
}

/*
 * Handle a CTRL-V or CTRL-Q typed in Insert mode.
 */
    ساکن خالی
ins_ctrl_v(خالی)
{
    صحیح		c;
    صحیح		did_putchar = FALSE;

    // may need to redraw when no more chars available now
    ins_redraw(FALSE);

    اگر (redrawing() && !char_avail())
    {
	edit_putchar('^', TRUE);
	did_putchar = TRUE;
    }
    Append_مقصد_Redobuff((char_u *)CTRL_V_STR);	// CTRL-V

    جوڑو_مقصد_showcmd_c(Ctrl_V);

    // Do not change any modifyOtherKeys ESC sequence to a normal key for
    // CTRL-SHIFT-V.
    c = get_literal(mod_نقاب & MOD_نقاب_سرکاؤ);
    اگر (did_putchar)
	// when the line fits in 'columns' the '^' is at the start of the next
	// line and will not removed by the redraw
	edit_unputchar();
    clear_showcmd();

    داخل_کرو_special(c, FALSE, TRUE);
#اگر_تعریف_شدہ FEAT_RIGHTLEFT
    revins_chars++;
    revins_legal++;
#اگر_ختم
}

/*
 * After getting an ESC or CSI for a literal key: If the typeahead buffer
 * contains a modifyOtherKeys sequence then decode it and return the result.
 * Otherwise return "c".
 * Note that this doesn't wait for characters, they must be in the typeahead
 * buffer already.
 */
    ساکن صحیح
decode_Modify_Other_کلیدیں(صحیح c)
{
    char_u  *p = typebuf.tb_buf + typebuf.tb_off;
    صحیح	    idx;
    صحیح	    form = 0;
    صحیح	    argidx = 0;
    صحیح	    arg[2] = {0, 0};

    // Recognize:
    // form 0: {lead}{key};{modifier}u
    // form 1: {lead}27;{modifier};{key}~
    اگر (typebuf.tb_len >= 4 && (c == CSI || (c == ESC && *p == '[')))
    {
	idx = (*p == '[');
	اگر (p[idx] == '2' && p[idx + 1] == '7' && p[idx + 2] == ';' &&
		kitty_protocol_state != KKPS_ENABLED)
	{
	    form = 1;
	    idx += 3;
	}
	جب (idx < typebuf.tb_len && argidx < 2)
	{
	    اگر (p[idx] == ';')
		++argidx;
	    ورنہ اگر (VIM_ISDIGIT(p[idx]))
		arg[argidx] = arg[argidx] * 10 + (p[idx] - '0');
	    ورنہ
		توڑو;
	    ++idx;
	}
	صحیح kitty_no_mods = argidx == 0 && kitty_protocol_state == KKPS_ENABLED;
	اگر (idx < typebuf.tb_len
		&& p[idx] == (form == 1 ? '~' : 'u')
		&& (argidx == 1 || kitty_no_mods))
	{
	    // Match, consume the code.
	    typebuf.tb_off += idx + 1;
	    typebuf.tb_len -= idx + 1;
#اگر defined(FEAT_CLIENTSERVER) || defined(FEAT_EVAL)
	    اگر (typebuf.tb_len == 0)
		typebuf_was_filled = FALSE;
#اگر_ختم

	    mod_نقاب = kitty_no_mods ? 0 : decode_modifiers(arg[!form]);
	    c = ملاؤ_modify_Other_کلیدیں(arg[form], &mod_نقاب);
	}
    }

    واپس c;
}

/*
 * Put a character directly onto the screen.  It's not stored in a buffer.
 * Used while handling CTRL-K, CTRL-V, etc. in Insert mode.
 */
ساکن صحیح  pc_حالت;
#تعریف_کرو PC_حالت_UNSET	0	// pc_bytes was not set
#تعریف_کرو PC_حالت_دایاں	1	// right half of double-wide char
#تعریف_کرو PC_حالت_بایاں	2	// left half of double-wide char
#تعریف_کرو PC_حالت_سیٹ	3	// pc_bytes was filled
ساکن char_u pc_بائٹیں[MB_MAXBYTES + 1]; // saved bytes
ساکن صحیح  pc_attr;
ساکن صحیح  pc_صف;
ساکن صحیح  pc_ستون;

    خالی
edit_putchar(صحیح c, صحیح highlight)
{
    صحیح	    attr;

    اگر (Screen_سطریں == تہی)
	واپس;

    update_topline();	// just in case w_topline isn't valid
    validate_cursor();
    اگر (highlight)
	attr = HL_ATTR(HLF_8);
    ورنہ
	attr = 0;
    pc_صف = W_WINROW(curwin) + curwin->w_wrow;
    pc_ستون = curwin->w_wincol;
    pc_حالت = PC_حالت_UNSET;
#اگر_تعریف_شدہ FEAT_RIGHTLEFT
    اگر (curwin->w_p_rl)
    {
	pc_ستون += curwin->w_چوڑائی - 1 - curwin->w_wcol;
	اگر (has_mbyte)
	{
	    صحیح fix_ستون = mb_fix_ستون(pc_ستون, pc_صف);

	    اگر (fix_ستون != pc_ستون)
	    {
		screen_putchar(' ', pc_صف, fix_ستون, attr);
		--curwin->w_wcol;
		pc_حالت = PC_حالت_دایاں;
	    }
	}
    }
    ورنہ
#اگر_ختم
    {
	pc_ستون += curwin->w_wcol;
	اگر (mb_lefthalve(pc_صف, pc_ستون))
	    pc_حالت = PC_حالت_بایاں;
    }

    // save the character to be able to put it back
    اگر (pc_حالت == PC_حالت_UNSET)
    {
	screen_getbytes(pc_صف, pc_ستون, pc_بائٹیں, &pc_attr);
	pc_حالت = PC_حالت_سیٹ;
    }
    screen_putchar(c, pc_صف, pc_ستون, attr);
}

#اگر defined(FEAT_کام_CHANNEL)
/*
 * Set the insert start position for when using a prompt buffer.
 */
    خالی
سیٹ_insstart(linenr_T lnum, صحیح ستون)
{
    Insstart.lnum = lnum;
    Insstart.ستون = ستون;
    Insstart_orig = Insstart;
    Insstart_textlen = Insstart.ستون;
    Insstart_blank_vcol = MAXCOL;
    arrow_مستعمل = FALSE;
}
#اگر_ختم

/*
 * Undo the previous edit_putchar().
 */
    خالی
edit_unputchar(خالی)
{
    اگر (pc_حالت != PC_حالت_UNSET && pc_صف >= پیغام_scrolled)
    {
	اگر (pc_حالت == PC_حالت_دایاں)
	    ++curwin->w_wcol;
	اگر (pc_حالت == PC_حالت_دایاں || pc_حالت == PC_حالت_بایاں)
	    redrawWinline(curwin, curwin->w_cursor.lnum);
	ورنہ
	    screen_puts(pc_بائٹیں, pc_صف - پیغام_scrolled, pc_ستون, pc_attr);
    }
}

/*
 * Called when "$" is in 'cpoptions': display a '$' at the end of the changed
 * text.  Only works when cursor is in the line that changes.
 */
    خالی
display_dollar(colnr_T ستون_arg)
{
    colnr_T ستون = ستون_arg < 0 ? 0 : ستون_arg;
    colnr_T save_ستون;

    اگر (!redrawing())
	واپس;

    cursor_off();
    save_ستون = curwin->w_cursor.ستون;
    curwin->w_cursor.ستون = ستون;
    اگر (has_mbyte)
    {
	char_u *p;

	// If on the last byte of a multi-byte move to the first byte.
	p = ml_get_curline();
	curwin->w_cursor.ستون -= (*mb_سر_off)(p, p + ستون);
    }
    curs_columns(FALSE);	    // recompute w_wrow and w_wcol
    اگر (curwin->w_wcol < curwin->w_چوڑائی)
    {
	edit_putchar('$', FALSE);
	dollar_vcol = curwin->w_virtcol;
    }
    curwin->w_cursor.ستون = save_ستون;
}

/*
 * Call this function before moving the cursor from the normal insert position
 * in insert mode.
 */
    خالی
undisplay_dollar(خالی)
{
    اگر (dollar_vcol < 0)
	واپس;

    dollar_vcol = -1;
    redrawWinline(curwin, curwin->w_cursor.lnum);
}

/*
 * Truncate the space at the end of a line.  This is to be used only in an
 * insert mode.  It handles fixing the replace stack for MODE_REPLACE and
 * MODE_VREPLACE modes.
 */
    خالی
truncate_spaces(char_u *سطر, size_t len)
{
    صحیح	    i;

    // find start of trailing white space
    برائے (i = (صحیح)len - 1; i >= 0 && VIM_ISWHITE(سطر[i]); i--)
    {
	اگر (State & REPLACE_FLAG)
	    replace_join(0);	    // remove a NUL from the replace stack
    }
    سطر[i + 1] = NUL;
}

/*
 * Backspace the cursor until the given column.  Handles MODE_REPLACE and
 * MODE_VREPLACE modes correctly.  May also be used when not in insert mode at
 * all.  Will attempt not to go before "col" even when there is a composing
 * character.
 */
    خالی
backspace_until_عمود(صحیح ستون)
{
    جب ((صحیح)curwin->w_cursor.ستون > ستون)
    {
	curwin->w_cursor.ستون--;
	اگر (State & REPLACE_FLAG)
	    replace_do_bs(ستون);
	ورنہ اگر (!del_char_after_ستون(ستون))
	    توڑو;
    }
}

/*
 * Like del_char(), but make sure not to go before column "limit_col".
 * Only matters when there are composing characters.
 * Return TRUE when something was deleted.
 */
    ساکن صحیح
del_char_after_ستون(صحیح حد_بندی_ستون غیر_مستعمل)
{
    اگر (enc_utf8 && حد_بندی_ستون >= 0)
    {
	colnr_T ecol = curwin->w_cursor.ستون + 1;

	// Make sure the cursor is at the start of a character, but
	// skip forward again when going too far back because of a
	// composing character.
	mb_adjust_cursor();
	جب (curwin->w_cursor.ستون < (colnr_T)حد_بندی_ستون)
	{
	    صحیح l = utf_ptr2len(ml_get_cursor());

	    اگر (l == 0)  // end of line
		توڑو;
	    curwin->w_cursor.ستون += l;
	}
	اگر (*ml_get_cursor() == NUL || curwin->w_cursor.ستون == ecol)
	    واپس FALSE;
	del_بائٹیں((طویل)((صحیح)ecol - curwin->w_cursor.ستون), FALSE, TRUE);
    }
    ورنہ
	(خالی)del_char(FALSE);
    واپس TRUE;
}

/*
 * Next character is interpreted literally.
 * A one, two or three digit decimal number is interpreted as its byte value.
 * If one or two digits are entered, the next character is given to vungetc().
 * For Unicode a character > 255 may be returned.
 * If "noReduceKeys" is TRUE do not change any modifyOtherKeys ESC sequence
 * into a normal key, return ESC.
 */
    صحیح
get_literal(صحیح no_Reduce_کلیدیں)
{
    صحیح		cc;
    صحیح		nc;
    صحیح		i;
    صحیح		hex = FALSE;
    صحیح		octal = FALSE;
    صحیح		unicode = 0;

    اگر (got_int)
	واپس Ctrl_C;

#اگر_تعریف_شدہ FEAT_GUI
    /*
     * In GUI there is no point inserting the internal code for a special key.
     * It is more useful to insert the string "<KEY>" instead.	This would
     * probably be useful in a text window too, but it would not be
     * vi-compatible (maybe there should be an option for it?) -- webb
     */
    اگر (gui.اندر_use)
    {
	++allow_کلیدیں;
	اگر (no_Reduce_کلیدیں)
	    ++no_reduce_کلیدیں;
    }
#اگر_ختم
#اگر_تعریف_شدہ USE_ON_FLY_SCROLL
    dont_scroll = TRUE;		// disallow scrolling here
#اگر_ختم
    ++no_mapping;		// don't map the next key hits
    cc = 0;
    i = 0;
    برائے (;;)
    {
	nc = plain_vgetc();
	اگر ((nc == ESC || nc == CSI) && !no_Reduce_کلیدیں)
	    nc = decode_Modify_Other_کلیدیں(nc);

	اگر ((mod_نقاب & ~MOD_نقاب_سرکاؤ) != 0)
	    // A character with non-Shift modifiers should not be a valid
	    // character for i_CTRL-V_digit.
	    توڑو;

	اگر ((State & منوال_CMDLINE) == 0 && MB_BYTE2LEN_CHECK(nc) == 1)
	    جوڑو_مقصد_showcmd(nc);
	اگر (nc == 'x' || nc == 'X')
	    hex = TRUE;
	ورنہ اگر (nc == 'o' || nc == 'O')
	    octal = TRUE;
	ورنہ اگر (nc == 'u' || nc == 'U')
	    unicode = nc;
	ورنہ
	{
	    اگر (hex || unicode != 0)
	    {
		اگر (!vim_isxdigit(nc))
		    توڑو;
		cc = cc * 16 + hex2nr(nc);
	    }
	    ورنہ اگر (octal)
	    {
		اگر (nc < '0' || nc > '7')
		    توڑو;
		cc = cc * 8 + nc - '0';
	    }
	    ورنہ
	    {
		اگر (!VIM_ISDIGIT(nc))
		    توڑو;
		cc = cc * 10 + nc - '0';
	    }

	    ++i;
	}

	اگر (cc > 255 && unicode == 0)
	    cc = 255;		// limit range to 0-255
	nc = 0;

	اگر (hex)		// hex: up to two chars
	{
	    اگر (i >= 2)
		توڑو;
	}
	ورنہ اگر (unicode)	// Unicode: up to four or eight chars
	{
	    اگر ((unicode == 'u' && i >= 4) || (unicode == 'U' && i >= 8))
		توڑو;
	}
	ورنہ اگر (i >= 3)	// decimal or octal: up to three chars
	    توڑو;
    }
    اگر (i == 0)	    // no number entered
    {
	اگر (nc == K_ZERO)   // NUL is stored as NL
	{
	    cc = '\n';
	    nc = 0;
	}
	ورنہ
	{
	    cc = nc;
	    nc = 0;
	}
    }

    اگر (cc == 0)	// NUL is stored as NL
	cc = '\n';
    اگر (enc_dbcs && (cc & 0xff) == 0)
	cc = '?';	// don't accept an illegal DBCS char, the NUL in the
			// second byte will cause trouble!

    --no_mapping;
#اگر_تعریف_شدہ FEAT_GUI
    اگر (gui.اندر_use)
    {
	--allow_کلیدیں;
	اگر (no_Reduce_کلیدیں)
	    --no_reduce_کلیدیں;
    }
#اگر_ختم
    اگر (nc)
    {
	vungetc(nc);
	// A character typed with i_CTRL-V_digit cannot have modifiers.
	mod_نقاب = 0;
    }
    got_int = FALSE;	    // CTRL-C typed after CTRL-V is not an interrupt
    واپس cc;
}

/*
 * Insert character, taking care of special keys and mod_mask
 */
    ساکن خالی
داخل_کرو_special(
    صحیح	    c,
    صحیح	    allow_modmask,
    صحیح	    ctrlv)	    // c was typed after CTRL-V
{
    char_u  *p;
    صحیح	    len;

    /*
     * Special function key, translate into "<Key>". Up to the last '>' is
     * inserted with ins_str(), so as not to replace characters in replace
     * mode.
     * Only use mod_mask for special keys, to avoid things like <S-Space>,
     * unless 'allow_modmask' is TRUE.
     */
#اگر defined(MACOS_X) || defined(FEAT_GUI_GTK)
    // Command-key never produces a normal key
    اگر (mod_نقاب & MOD_نقاب_CMD)
	allow_modmask = TRUE;
#اگر_ختم
    اگر (IS_SPECIAL(c) || (mod_نقاب && allow_modmask))
    {
	p = get_special_کلید_name(c, mod_نقاب);
	len = (صحیح)STRLEN(p);
	c = p[len - 1];
	اگر (len > 2)
	{
	    اگر (stop_arrow() == FAIL)
		واپس;
	    p[len - 1] = NUL;
	    ins_str(p, len - 1);
	    Append_مقصد_Redobuff_Lit(p, -1);
	    ctrlv = FALSE;
	}
    }
    اگر (stop_arrow() == OK)
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
#تعریف_کرو ISSPECIAL(c)	((c) < ' ' || (c) >= DEL || (c) == '0' || (c) == '^')

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
    خالی
insertchar(
    صحیح		c,			// character to insert or NUL
    صحیح		flags,			// INSCHAR_FORMAT, etc.
    صحیح		دوسرا_indent)		// indent for second line if >= 0
{
    صحیح		textwidth;
    char_u	*p;
    صحیح		fo_ins_blank;
    صحیح		force_format = flags & INSCHAR_FORMAT;

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
    اگر (textwidth > 0
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
#اگر defined(FEAT_EVAL)
	صحیح     do_اندرونی = TRUE;
	colnr_T virtcol = get_nolist_virtcol()
				  + char2cells(c != NUL ? c : gchar_cursor());

	اگر (*curbuf->b_p_fex != NUL && (flags & INSCHAR_NO_FEX) == 0
		&& (force_format || virtcol > (colnr_T)textwidth))
	{
	    do_اندرونی = (fex_format(curwin->w_cursor.lnum, 1L, c) != 0);
	    // It may be required to save for undo again, e.g. when setline()
	    // was called.
	    ins_need_undo = TRUE;
	}
	اگر (do_اندرونی)
#اگر_ختم
	    اندرونی_format(textwidth, دوسرا_indent, flags, c == NUL, c);
    }

    اگر (c == NUL)	    // only formatting was wanted
	واپس;

    // Check whether this character should end a comment.
    اگر (did_ai && c == انتہا_comment_pending)
    {
	char_u  *سطر;
	char_u	lead_انتہا[COM_MAX_LEN];	    // end-comment string
	صحیح	درمیان_len, انتہا_len;
	صحیح	i;

	/*
	 * Need to remove existing (middle) comment leader and insert end
	 * comment leader.  First, check what comment leader we can find.
	 */
	i = get_leader_len(سطر = ml_get_curline(), &p, FALSE, TRUE);
	اگر (i > 0 && vim_strchr(p, COM_درمیان) != تہی)	// Just checking
	{
	    // Skip middle-comment string
	    جب (*p && p[-1] != ':')	// find end of middle flags
		++p;
	    درمیان_len = copy_option_part(&p, lead_انتہا, COM_MAX_LEN, ",");
	    // Don't count trailing white space for middle_len
	    جب (درمیان_len > 0 && VIM_ISWHITE(lead_انتہا[درمیان_len - 1]))
		--درمیان_len;

	    // Find the end-comment string
	    جب (*p && p[-1] != ':')	// find end of end flags
		++p;
	    انتہا_len = copy_option_part(&p, lead_انتہا, COM_MAX_LEN, ",");

	    // Skip white space before the cursor
	    i = curwin->w_cursor.ستون;
	    جب (--i >= 0 && VIM_ISWHITE(سطر[i]))
		;
	    i++;

	    // Skip to before the middle leader
	    i -= درمیان_len;

	    // Check some expected things before we go on
	    اگر (i >= 0 && انتہا_len > 0 && lead_انتہا[انتہا_len - 1] == انتہا_comment_pending)
	    {
		// Backspace over all the stuff we want to replace
		backspace_until_عمود(i);

		// Insert the end-comment string, except for the last
		// character, which will get inserted as normal later.
		ins_بائٹیں_len(lead_انتہا, انتہا_len - 1);
	    }
	}
    }
    انتہا_comment_pending = NUL;

    did_ai = FALSE;
    did_si = FALSE;
    can_si = FALSE;
    can_si_عقب = FALSE;

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
#اگر_تعریف_شدہ USE_ON_FLY_SCROLL
    dont_scroll = FALSE;		// allow scrolling here
#اگر_ختم

    اگر (       !ISSPECIAL(c)
	    && (!has_mbyte || (*mb_char2len)(c) == 1)
	    && !has_insertcharpre()
#اگر_تعریف_شدہ FEAT_EVAL
	    // Skip typeahead if test_override("char_avail", 1) was called.
	    && !disable_char_avail_for_testing
#اگر_ختم
	    && vpeekc() != NUL
	    && !(State & REPLACE_FLAG)
	    && !cindent_on()
#اگر_تعریف_شدہ FEAT_RIGHTLEFT
	    && !p_ri
#اگر_ختم
	   )
    {
#تعریف_کرو داخلہ_BUFLEN 100
	char_u		buf[داخلہ_BUFLEN + 1];
	صحیح		i;
	colnr_T		virtcol = 0;

	buf[0] = c;
	i = 1;
	اگر (textwidth > 0)
	    virtcol = get_nolist_virtcol();
	/*
	 * Stop the string when:
	 * - no more chars available
	 * - finding a special character (command key)
	 * - buffer is full
	 * - running into the 'textwidth' boundary
	 * - need to check for abbreviation: A non-word char after a word-char
	 */
	جب (	   (c = vpeekc()) != NUL
		&& !ISSPECIAL(c)
		&& (!has_mbyte || MB_BYTE2LEN_CHECK(c) == 1)
		&& i < داخلہ_BUFLEN
		&& (textwidth == 0
		    || (virtcol += byte2cells(buf[i - 1])) < (colnr_T)textwidth)
		&& !(!no_abbr && !vim_iswordc(c) && vim_iswordc(buf[i - 1])))
	{
#اگر_تعریف_شدہ FEAT_RIGHTLEFT
	    c = vgetc();
	    اگر (p_hkmap && کلید_Typed)
		c = hkmap(c);		    // Hebrew mode mapping
	    buf[i++] = c;
#ورنہ
	    buf[i++] = vgetc();
#اگر_ختم
	}

#اگر_تعریف_شدہ FEAT_DIGRAPHS
	do_digraph(-1);			// clear digraphs
	do_digraph(buf[i-1]);		// may be the start of a digraph
#اگر_ختم
	buf[i] = NUL;
	ins_str(buf, i);
	اگر (flags & INSCHAR_CTRLV)
	{
	    redo_literal(*buf);
	    i = 1;
	}
	ورنہ
	    i = 0;
	اگر (buf[i] != NUL)
	    Append_مقصد_Redobuff_Lit(buf + i, -1);
    }
    ورنہ
    {
	صحیح		cc;

	اگر (has_mbyte && (cc = (*mb_char2len)(c)) > 1)
	{
	    char_u	buf[MB_MAXBYTES + 1];

	    (*mb_char2bytes)(c, buf);
	    buf[cc] = NUL;
	    ins_char_بائٹیں(buf, cc);
	    Append_Char_مقصد_Redobuff(c);
	}
	ورنہ
	{
	    ins_char(c);
	    اگر (flags & INSCHAR_CTRLV)
		redo_literal(c);
	    ورنہ
		Append_Char_مقصد_Redobuff(c);
	}
    }
}

/*
 * Put a character in the redo buffer, for when just after a CTRL-V.
 */
    ساکن خالی
redo_literal(صحیح c)
{
    char_u	buf[10];

    // Only digits need special treatment.  Translate them into a string of
    // three digits.
    اگر (VIM_ISDIGIT(c))
    {
	vim_snprintf((حرف *)buf, حجم(buf), "%03d", c);
	Append_مقصد_Redobuff(buf);
    }
    ورنہ
	Append_Char_مقصد_Redobuff(c);
}

/*
 * start_arrow() is called when an arrow key is used in insert mode.
 * For undo/redo it resembles hitting the <ESC> key.
 */
    خالی
آغاز_arrow(
    pos_T    *انتہا_داخل_کرو_pos)		// can be NULL
{
    آغاز_arrow_common(انتہا_داخل_کرو_pos, TRUE);
}

/*
 * Like start_arrow() but with end_change argument.
 * Will prepare for redo of CTRL-G U if "end_change" is FALSE.
 */
    ساکن خالی
آغاز_arrow_with_change(
    pos_T    *انتہا_داخل_کرو_pos,		// can be NULL
    صحیح	      انتہا_change)		// end undoable change
{
    آغاز_arrow_common(انتہا_داخل_کرو_pos, انتہا_change);
    اگر (!انتہا_change)
    {
	Append_Char_مقصد_Redobuff(Ctrl_G);
	Append_Char_مقصد_Redobuff('U');
    }
}

    ساکن خالی
آغاز_arrow_common(
    pos_T    *انتہا_داخل_کرو_pos,		// can be NULL
    صحیح	      انتہا_change)		// end undoable change
{
    اگر (!arrow_مستعمل && انتہا_change)	// something has been inserted
    {
	Append_مقصد_Redobuff(ESC_STR);
	stop_داخل_کرو(انتہا_داخل_کرو_pos, FALSE, FALSE);
	arrow_مستعمل = TRUE;	// this means we stopped the current insert
    }
#اگر_تعریف_شدہ FEAT_SPELL
    check_spell_redraw();
#اگر_ختم
}

#اگر_تعریف_شدہ FEAT_SPELL
/*
 * If we skipped highlighting word at cursor, do it now.
 * It may be skipped again, thus reset spell_redraw_lnum first.
 */
    ساکن خالی
check_spell_redraw(خالی)
{
    اگر (spell_redraw_lnum != 0)
    {
	linenr_T	lnum = spell_redraw_lnum;

	spell_redraw_lnum = 0;
	redrawWinline(curwin, lnum);
    }
}

#اگر_ختم

/*
 * stop_arrow() is called before a change is made in insert mode.
 * If an arrow key has been used, start a new insertion.
 * Returns FAIL if undo is impossible, shouldn't insert then.
 */
    صحیح
stop_arrow(خالی)
{
    اگر (arrow_مستعمل)
    {
	Insstart = curwin->w_cursor;	// new insertion starts here
	اگر (Insstart.ستون > Insstart_orig.ستون && !ins_need_undo)
	    // Don't update the original insert position when moved to the
	    // right, except when nothing was inserted yet.
	    update_Insstart_orig = FALSE;
	Insstart_textlen = (colnr_T)linetabsize_str(ml_get_curline());

	اگر (u_save_cursor() == OK)
	{
	    arrow_مستعمل = FALSE;
	    ins_need_undo = FALSE;
	}

	ai_ستون = 0;
	اگر (State & VREPLACE_FLAG)
	{
	    orig_سطر_گنتی = curbuf->b_ml.ml_سطر_گنتی;
	    vr_سطریں_بدلا = 1;
	}
	ResetRedobuff();
	Append_مقصد_Redobuff((char_u *)"1i");   // pretend we start an insertion
	new_داخل_کرو_skip = 2;
    }
    ورنہ اگر (ins_need_undo)
    {
	اگر (u_save_cursor() == OK)
	{
	    // A command or event may have moved the cursor before the next
	    // edit. Pull Insstart back only when the cursor moved above it,
	    // so that later edits can properly decide whether an extra undo
	    // entry is needed. Advancing Insstart would mis-place '[ after a
	    // register paste.
	    اگر (LT_POS(curwin->w_cursor, Insstart))
	    {
		Insstart = curwin->w_cursor;
		Insstart_textlen = (colnr_T)linetabsize_str(ml_get_curline());
	    }
	    ins_need_undo = FALSE;
	}
    }

#اگر_تعریف_شدہ FEAT_FOLDING
    // Always open fold at the cursor line when inserting something.
    foldOpenCursor();
#اگر_ختم

    واپس (arrow_مستعمل || ins_need_undo ? FAIL : OK);
}

/*
 * Do a few things to stop inserting.
 * "end_insert_pos" is where insert ended.  It is NULL when we already jumped
 * to another window/buffer.
 */
    ساکن خالی
stop_داخل_کرو(
    pos_T	*انتہا_داخل_کرو_pos,
    صحیح		esc,			// called by ins_esc()
    صحیح		nomove)			// <c-\><c-o>, don't move cursor
{
    صحیح		cc;
    string_T	inserted;

    stop_redo_ins();
    replace_flush();		// abandon replace stack

    /*
     * Save the inserted text for later redo with ^@ and CTRL-A.
     * Don't do it when "restart_edit" was set and nothing was inserted,
     * otherwise CTRL-O w and then <Left> will clear "last_insert".
     */
    inserted = get_inserted();
    صحیح added = inserted.string == تہی ? 0 : (صحیح)inserted.لمبائی - new_داخل_کرو_skip;
    اگر (did_restart_edit == 0 || added > 0)
    {
	vim_free(آخری_داخل_کرو.string);
	آخری_داخل_کرو = inserted;			    // structure copy
	آخری_داخل_کرو_skip = added < 0 ? 0 : new_داخل_کرو_skip;
    }
    ورنہ
	vim_free(inserted.string);

    اگر (!arrow_مستعمل && انتہا_داخل_کرو_pos != تہی)
    {
	// Auto-format now.  It may seem strange to do this when stopping an
	// insertion (or moving the cursor), but it's required when appending
	// a line and having it end in a space.  But only do it when something
	// was actually inserted, otherwise undo won't work.
	اگر (!ins_need_undo && has_format_option(FO_AUTO))
	{
	    pos_T   tpos = curwin->w_cursor;

	    // When the cursor is at the end of the line after a space the
	    // formatting will move it to the following word.  Avoid that by
	    // moving the cursor onto the space.
	    cc = 'x';
	    اگر (curwin->w_cursor.ستون > 0 && gchar_cursor() == NUL)
	    {
		dec_cursor();
		cc = gchar_cursor();
		اگر (!VIM_ISWHITE(cc))
		    curwin->w_cursor = tpos;
	    }

	    auto_format(TRUE, FALSE);

	    اگر (VIM_ISWHITE(cc))
	    {
		اگر (gchar_cursor() != NUL)
		    inc_cursor();
		// If the cursor is still at the same character, also keep
		// the "coladd".
		اگر (gchar_cursor() == NUL
			&& curwin->w_cursor.lnum == tpos.lnum
			&& curwin->w_cursor.ستون == tpos.ستون)
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
	اگر (!nomove && did_ai && (esc || (vim_strchr(p_cpo, CPO_INDENT) == تہی
			&& curwin->w_cursor.lnum != انتہا_داخل_کرو_pos->lnum))
		&& انتہا_داخل_کرو_pos->lnum <= curbuf->b_ml.ml_سطر_گنتی)
	{
	    pos_T	tpos = curwin->w_cursor;
	    colnr_T	پچھلا_ستون = انتہا_داخل_کرو_pos->ستون;
	    colnr_T	strip_ستون;

	    curwin->w_cursor = *انتہا_داخل_کرو_pos;
	    check_cursor_ستون();  // make sure it is not past the line

	    // Where the loop would actually start (back up if on NUL).
	    strip_ستون = curwin->w_cursor.ستون;
	    اگر (gchar_cursor() == NUL && strip_ستون > 0)
		--strip_ستون;

	    // Don't strip if non-whitespace follows: setline() from a <Cmd>
	    // mapping or CursorHoldI autocmd may have inserted content.
	    اگر (*skipwhite(ml_get_curline() + strip_ستون) == NUL)
	    {
		curwin->w_cursor.ستون = strip_ستون;
		برائے (;;)
		{
		    اگر (gchar_cursor() == NUL && curwin->w_cursor.ستون > 0)
			--curwin->w_cursor.ستون;
		    cc = gchar_cursor();
		    اگر (!VIM_ISWHITE(cc))
			توڑو;
		    اگر (del_char(TRUE) == FAIL)
			توڑو;  // should not happen
		}
		اگر (curwin->w_cursor.lnum != tpos.lnum)
		    curwin->w_cursor = tpos;
		ورنہ اگر (curwin->w_cursor.ستون < پچھلا_ستون)
		{
		    // reset tpos, could have been invalidated in the loop above
		    tpos = curwin->w_cursor;
		    tpos.ستون++;
		    اگر (cc != NUL && gchar_pos(&tpos) == NUL)
			++curwin->w_cursor.ستون;	// put cursor back on the NUL
		}

		// <C-S-Right> may have started Visual mode, adjust the position for
		// deleted characters.
		اگر (VIsual_فعال)
		    check_visual_pos();
	    }
	    ورنہ
		// Non-whitespace follows, keep original cursor.
		curwin->w_cursor = tpos;
	}
    }
    did_ai = FALSE;
    did_si = FALSE;
    can_si = FALSE;
    can_si_عقب = FALSE;

    // Set '[ and '] to the inserted text.  When end_insert_pos is NULL we are
    // now in a different buffer.
    اگر (انتہا_داخل_کرو_pos != تہی)
    {
	curbuf->b_op_آغاز = Insstart;
	curbuf->b_op_آغاز_orig = Insstart_orig;
	curbuf->b_op_انتہا = *انتہا_داخل_کرو_pos;
    }
}

/*
 * Set the last inserted text to a single character.
 * Used for the replace command.
 */
    خالی
سیٹ_آخری_داخل_کرو(صحیح c)
{
    char_u	*s;

    vim_free(آخری_داخل_کرو.string);
    آخری_داخل_کرو.string = alloc(MB_MAXBYTES * 3 + 5);
    اگر (آخری_داخل_کرو.string == تہی)
    {
	آخری_داخل_کرو.لمبائی = 0;
	واپس;
    }

    s = آخری_داخل_کرو.string;
    // Use the CTRL-V only when entering a special char
    اگر (c < ' ' || c == DEL)
	*s++ = Ctrl_V;
    s = جوڑو_char2buf(c, s);
    *s++ = ESC;
    *s = NUL;
    آخری_داخل_کرو.لمبائی = (size_t)(s - آخری_داخل_کرو.string);
    آخری_داخل_کرو_skip = 0;
}

#اگر defined(EXITFREE)
    خالی
free_آخری_داخل_کرو(خالی)
{
    VIM_CLEAR_STRING(آخری_داخل_کرو);
}
#اگر_ختم

/*
 * Add character "c" to buffer "s".  Escape the special meaning of K_SPECIAL
 * and CSI.  Handle multi-byte characters.
 * Returns a pointer to after the added bytes.
 */
    char_u *
جوڑو_char2buf(صحیح c, char_u *s)
{
    char_u	عارضی[MB_MAXBYTES + 1];
    صحیح		i;
    صحیح		len;

    len = (*mb_char2bytes)(c, عارضی);
    برائے (i = 0; i < len; ++i)
    {
	c = عارضی[i];
	// Need to escape K_SPECIAL and CSI like in the typeahead buffer.
	اگر (c == K_SPECIAL)
	{
	    *s++ = K_SPECIAL;
	    *s++ = KS_SPECIAL;
	    *s++ = KE_FILLER;
	}
#اگر_تعریف_شدہ FEAT_GUI
	ورنہ اگر (c == CSI)
	{
	    *s++ = CSI;
	    *s++ = KS_EXTRA;
	    *s++ = (صحیح)KE_CSI;
	}
#اگر_ختم
	ورنہ
	    *s++ = c;
    }
    واپس s;
}

/*
 * move cursor to start of line
 * if flags & BL_WHITE	move to first non-white
 * if flags & BL_SOL	move to first non-white if startofline is set,
 *			    otherwise keep "curswant" column
 * if flags & BL_FIX	don't leave the cursor on a NUL.
 */
    خالی
beginline(صحیح flags)
{
    اگر ((flags & BL_SOL) && !p_sol)
	coladvance(curwin->w_curswant);
    ورنہ
    {
	curwin->w_cursor.ستون = 0;
	curwin->w_cursor.coladd = 0;

	اگر (flags & (BL_WHITE | BL_SOL))
	{
	    char_u  *ptr;

	    برائے (ptr = ml_get_curline(); VIM_ISWHITE(*ptr)
			       && !((flags & BL_FIX) && ptr[1] == NUL); ++ptr)
		++curwin->w_cursor.ستون;
	}
	curwin->w_سیٹ_curswant = true;
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

    صحیح
oneright(خالی)
{
    char_u	*ptr;
    صحیح		l;

    اگر (virtual_فعال())
    {
	pos_T	prevpos = curwin->w_cursor;

	// Adjust for multi-wide char (excluding TAB)
	ptr = ml_get_cursor();
	coladvance(getviscol() + ((*ptr != TAB
					  && vim_isprintc((*mb_ptr2char)(ptr)))
		    ? ptr2cells(ptr) : 1));
	curwin->w_سیٹ_curswant = true;
	// Return OK if the cursor moved, FAIL otherwise (at window edge).
	واپس (prevpos.ستون != curwin->w_cursor.ستون
		    || prevpos.coladd != curwin->w_cursor.coladd) ? OK : FAIL;
    }

    ptr = ml_get_cursor();
    اگر (*ptr == NUL)
	واپس FAIL;	    // already at the very end

    اگر (has_mbyte)
	l = (*mb_ptr2len)(ptr);
    ورنہ
	l = 1;

    // move "l" bytes right, but don't end up on the NUL, unless 'virtualedit'
    // contains "onemore".
    اگر (ptr[l] == NUL && (get_ve_flags() & VE_ONEMORE) == 0)
	واپس FAIL;
    curwin->w_cursor.ستون += l;

    curwin->w_سیٹ_curswant = true;
    adjust_skipcol();
    واپس OK;
}

    صحیح
oneleft(خالی)
{
    اگر (virtual_فعال())
    {
#اگر_تعریف_شدہ FEAT_LINEBREAK
	صحیح چوڑائی;
#اگر_ختم
	صحیح v = getviscol();

	اگر (v == 0)
	    واپس FAIL;

#اگر_تعریف_شدہ FEAT_LINEBREAK
	// We might get stuck on 'showbreak', skip over it.
	چوڑائی = 1;
	برائے (;;)
	{
	    coladvance(v - چوڑائی);
	    // getviscol() is slow, skip it when 'showbreak' is empty,
	    // 'breakindent' is not set and there are no multi-byte
	    // characters
	    اگر ((*get_showbreak_قدر(curwin) == NUL && !curwin->w_p_bri
					     && !has_mbyte) || getviscol() < v)
		توڑو;
	    ++چوڑائی;
	}
#ورنہ
	coladvance(v - 1);
#اگر_ختم

	اگر (curwin->w_cursor.coladd == 1)
	{
	    char_u *ptr;

	    // Adjust for multi-wide char (not a TAB)
	    ptr = ml_get_cursor();
	    اگر (*ptr != TAB && vim_isprintc((*mb_ptr2char)(ptr))
							 && ptr2cells(ptr) > 1)
		curwin->w_cursor.coladd = 0;
	}

	curwin->w_سیٹ_curswant = true;
	adjust_skipcol();
	واپس OK;
    }

    اگر (curwin->w_cursor.ستون == 0)
	واپس FAIL;

    curwin->w_سیٹ_curswant = true;
    --curwin->w_cursor.ستون;

    // if the character on the left of the current cursor is a multi-byte
    // character, move to its first byte
    اگر (has_mbyte)
	mb_adjust_cursor();
    adjust_skipcol();
    واپس OK;
}

/*
 * Move the cursor up "n" lines in window "wp".
 * Takes care of closed folds.
 */
    خالی
cursor_اوپر_inner(win_T *wp, طویل n)
{
    linenr_T	lnum = wp->w_cursor.lnum;

    اگر (n >= lnum)
	lnum = 1;
    ورنہ
#اگر_تعریف_شدہ FEAT_FOLDING
	اگر (hasAnyFolding(wp))
    {
	/*
	 * Count each sequence of folded lines as one logical line.
	 */
	// go to the start of the current fold
	(خالی)hasFoldingWin(wp, lnum, &lnum, تہی, TRUE, تہی);

	جب (n--)
	{
	    // move up one line
	    --lnum;
	    اگر (lnum <= 1)
		توڑو;
	    // If we entered a fold, move to the beginning, unless in
	    // Insert mode or when 'foldopen' contains "all": it will open
	    // in a moment.
	    اگر (n > 0 || !((State & منوال_داخل_کرو) || (fdo_flags & FDO_ALL)))
		(خالی)hasFoldingWin(wp, lnum, &lnum, تہی, TRUE, تہی);
	}
	اگر (lnum < 1)
	    lnum = 1;
    }
    ورنہ
#اگر_ختم
	lnum -= n;

    wp->w_cursor.lnum = lnum;
}

    صحیح
cursor_اوپر(
    طویل	n,
    صحیح		upd_topline)	    // When TRUE: update topline
{
    // This fails if the cursor is already in the first line or the count is
    // larger than the line number and '-' is in 'cpoptions'
    linenr_T lnum = curwin->w_cursor.lnum;
    اگر (n > 0 && (lnum <= 1
		       || (n >= lnum && vim_strchr(p_cpo, CPO_MINUS) != تہی)))
	واپس FAIL;
    cursor_اوپر_inner(curwin, n);

    // try to advance to the column we want to be at
    coladvance(curwin->w_curswant);

    اگر (upd_topline)
	update_topline();	// make sure curwin->w_topline is valid

    واپس OK;
}

/*
 * Move the cursor down "n" lines in window "wp".
 * Takes care of closed folds.
 */
    خالی
cursor_نیچے_کو_inner(win_T *wp, طویل n)
{
    linenr_T	lnum = wp->w_cursor.lnum;
    linenr_T	سطر_گنتی = wp->w_بفر->b_ml.ml_سطر_گنتی;

    اگر (lnum + n >= سطر_گنتی)
	lnum = سطر_گنتی;
    ورنہ
#اگر_تعریف_شدہ FEAT_FOLDING
	اگر (hasAnyFolding(wp))
    {
	linenr_T	آخری;

	// count each sequence of folded lines as one logical line
	جب (n--)
	{
	    اگر (hasFoldingWin(wp, lnum, تہی, &آخری, TRUE, تہی))
		lnum = آخری + 1;
	    ورنہ
		++lnum;
	    اگر (lnum >= سطر_گنتی)
		توڑو;
	}
	اگر (lnum > سطر_گنتی)
	    lnum = سطر_گنتی;
    }
    ورنہ
#اگر_ختم
	lnum += n;

    wp->w_cursor.lnum = lnum;
}

/*
 * Cursor down a number of logical lines.
 */
    صحیح
cursor_نیچے_کو(
    طویل	n,
    صحیح		upd_topline)	    // When TRUE: update topline
{
    linenr_T	lnum = curwin->w_cursor.lnum;
    linenr_T	سطر_گنتی = curwin->w_بفر->b_ml.ml_سطر_گنتی;
    // This fails if the cursor is already in the last (folded) line, or would
    // move beyond the last line and '-' is in 'cpoptions'.
#اگر_تعریف_شدہ FEAT_FOLDING
    hasFoldingWin(curwin, lnum, تہی, &lnum, TRUE, تہی);
#اگر_ختم
    اگر (n > 0
	    && (lnum >= سطر_گنتی
		|| (lnum + n > سطر_گنتی
				     && vim_strchr(p_cpo, CPO_MINUS) != تہی)))
	واپس FAIL;
    cursor_نیچے_کو_inner(curwin, n);

    // try to advance to the column we want to be at
    coladvance(curwin->w_curswant);

    اگر (upd_topline)
	update_topline();	// make sure curwin->w_topline is valid

    واپس OK;
}

/*
 * Stuff the last inserted text in the read buffer.
 * Last_insert actually is a copy of the redo buffer, so we
 * first have to remove the command.
 */
    صحیح
stuff_inserted(
    صحیح	    c,		// Command character to be inserted
    طویل    گنتی,	// Repeat this many times
    صحیح	    no_esc)	// Don't add an ESC at the end
{
    string_T	داخل_کرو;				// text to be inserted
    char_u	آخری = ' ';

    داخل_کرو = get_آخری_داخل_کرو();
    اگر (داخل_کرو.string == تہی)
    {
	emsg(_(e_no_inserted_متن_yet));
	واپس FAIL;
    }

    // may want to stuff the command character, to start Insert mode
    اگر (c != NUL)
	stuffcharReadbuff(c);

    اگر (داخل_کرو.لمبائی > 0)
    {
	char_u	*p;

	// look for the last ESC in 'insert'
	برائے (p = داخل_کرو.string + داخل_کرو.لمبائی - 1; p >= داخل_کرو.string; --p)
	{
	    اگر (*p == ESC)
	    {
		داخل_کرو.لمبائی = (size_t)(p - داخل_کرو.string);
		توڑو;
	    }
	}
    }

    اگر (داخل_کرو.لمبائی > 0)
    {
	char_u	*p = داخل_کرو.string + داخل_کرو.لمبائی - 1;

	// when the last char is either "0" or "^" it will be quoted if no ESC
	// comes after it OR if it will insert more than once and "ptr"
	// starts with ^D.	-- Acevedo
	اگر ((*p == '0' || *p == '^')
		&& (no_esc || (*داخل_کرو.string == Ctrl_D && گنتی > 1)))
	{
	    آخری = *p;
	    --داخل_کرو.لمبائی;
	}
    }

    کرو
    {
	stuffReadbuffLen(داخل_کرو.string, (طویل)داخل_کرو.لمبائی);
	// a trailing "0" is inserted as "<C-V>048", "^" as "<C-V>^"
	انتخاب (آخری)
	{
	حالت '0':
#تعریف_کرو متن_مقصد_داخل_کرو "\026\060\064\070"
	    stuffReadbuffLen((char_u *)متن_مقصد_داخل_کرو, STRLEN_LITERAL(متن_مقصد_داخل_کرو));
#تعریف_منسوخ متن_مقصد_داخل_کرو
	    توڑو;

	حالت '^':
#تعریف_کرو متن_مقصد_داخل_کرو "\026^"
	    stuffReadbuffLen((char_u *)متن_مقصد_داخل_کرو, STRLEN_LITERAL(متن_مقصد_داخل_کرو));
#تعریف_منسوخ متن_مقصد_داخل_کرو
	    توڑو;

	طےشدہ:
	    توڑو;
	}
    }
    جب (--گنتی > 0);

    // may want to stuff a trailing ESC, to get out of Insert mode
    اگر (!no_esc)
	stuffcharReadbuff(ESC);

    واپس OK;
}

    string_T
get_آخری_داخل_کرو(خالی)
{
    string_T داخل_کرو = {تہی, 0};

    اگر (آخری_داخل_کرو.string != تہی)
    {
	داخل_کرو.string = آخری_داخل_کرو.string + آخری_داخل_کرو_skip;
	داخل_کرو.لمبائی = (size_t)(آخری_داخل_کرو.لمبائی - آخری_داخل_کرو_skip);
    }

    واپس داخل_کرو;
}

/*
 * Get last inserted string, and remove trailing <Esc>.
 * Returns pointer to allocated memory (must be freed) or NULL.
 */
    char_u *
get_آخری_داخل_کرو_save(خالی)
{
    string_T	داخل_کرو = get_آخری_داخل_کرو();
    char_u	*s;

    اگر (داخل_کرو.string == تہی)
	واپس تہی;
    s = vim_strnsave(داخل_کرو.string, داخل_کرو.لمبائی);
    اگر (s == تہی)
	واپس تہی;

    اگر (داخل_کرو.لمبائی > 0 && s[داخل_کرو.لمبائی - 1] == ESC)	// remove trailing ESC
	s[--داخل_کرو.لمبائی] = NUL;
    واپس s;
}

/*
 * Check the word in front of the cursor for an abbreviation.
 * Called when the non-id character "c" has been entered.
 * When an abbreviation is recognized it is removed from the text and
 * the replacement string is inserted in typebuf.tb_buf[], followed by "c".
 */
    ساکن صحیح
echeck_abbr(صحیح c)
{
    // Don't check for abbreviation in paste mode, when disabled and just
    // after moving around with cursor keys.
    اگر (p_paste || no_abbr || arrow_مستعمل)
	واپس FALSE;

    واپس check_abbr(c, ml_get_curline(), curwin->w_cursor.ستون,
		curwin->w_cursor.lnum == Insstart.lnum ? Insstart.ستون : 0);
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

ساکن char_u	*replace_ڈھیر = تہی;
ساکن طویل	replace_ڈھیر_nr = 0;	    // next entry in replace stack
ساکن طویل	replace_ڈھیر_len = 0;	    // max. number of entries

    خالی
replace_ڈالو(
    صحیح	    c)	    // character that is replaced (NUL is none)
{
    char_u  *p;

    اگر (replace_ڈھیر_nr < replace_انحراف)	// nothing to do
	واپس;
    اگر (replace_ڈھیر_len <= replace_ڈھیر_nr)
    {
	replace_ڈھیر_len += 50;
	p = ALLOC_MULT(char_u, replace_ڈھیر_len);
	اگر (p == تہی)	    // out of memory
	{
	    replace_ڈھیر_len -= 50;
	    واپس;
	}
	اگر (replace_ڈھیر != تہی)
	{
	    mch_memmove(p, replace_ڈھیر,
				 (size_t)(replace_ڈھیر_nr * حجم(char_u)));
	    vim_free(replace_ڈھیر);
	}
	replace_ڈھیر = p;
    }
    p = replace_ڈھیر + replace_ڈھیر_nr - replace_انحراف;
    اگر (replace_انحراف)
	mch_memmove(p + 1, p, (size_t)(replace_انحراف * حجم(char_u)));
    *p = c;
    ++replace_ڈھیر_nr;
}

/*
 * Push a character onto the replace stack.  Handles a multi-byte character in
 * reverse byte order, so that the first byte is popped off first.
 * Return the number of bytes done (includes composing characters).
 */
    صحیح
replace_ڈالو_mb(char_u *p)
{
    صحیح l = (*mb_ptr2len)(p);
    صحیح j;

    برائے (j = l - 1; j >= 0; --j)
	replace_ڈالو(p[j]);
    واپس l;
}

/*
 * Pop one item from the replace stack.
 * return -1 if stack empty
 * return replaced character or NUL otherwise
 */
    ساکن صحیح
replace_نکالو(خالی)
{
    اگر (replace_ڈھیر_nr == 0)
	واپس -1;
    واپس (صحیح)replace_ڈھیر[--replace_ڈھیر_nr];
}

/*
 * Join the top two items on the replace stack.  This removes to "off"'th NUL
 * encountered.
 */
    خالی
replace_join(
    صحیح	    off)	// offset for which NUL to remove
{
    صحیح	    i;

    برائے (i = replace_ڈھیر_nr; --i >= 0; )
	اگر (replace_ڈھیر[i] == NUL && off-- <= 0)
	{
	    --replace_ڈھیر_nr;
	    mch_memmove(replace_ڈھیر + i, replace_ڈھیر + i + 1,
					      (size_t)(replace_ڈھیر_nr - i));
	    واپس;
	}
}

/*
 * Pop bytes from the replace stack until a NUL is found, and insert them
 * before the cursor.  Can only be used in MODE_REPLACE or MODE_VREPLACE state.
 */
    ساکن خالی
replace_نکالو_ins(خالی)
{
    صحیح	    cc;
    صحیح	    oldState = State;

    State = منوال_معمول;			// don't want MODE_REPLACE here
    جب ((cc = replace_نکالو()) > 0)
    {
	mb_replace_نکالو_ins(cc);
	dec_cursor();
    }
    State = oldState;
}

/*
 * Insert bytes popped from the replace stack. "cc" is the first byte.  If it
 * indicates a multi-byte char, pop the other bytes too.
 */
    ساکن خالی
mb_replace_نکالو_ins(صحیح cc)
{
    صحیح		n;
    char_u	buf[MB_MAXBYTES + 1];
    صحیح		i;
    صحیح		c;

    اگر (has_mbyte && (n = MB_BYTE2LEN(cc)) > 1)
    {
	buf[0] = cc;
	برائے (i = 1; i < n; ++i)
	    buf[i] = replace_نکالو();
	ins_بائٹیں_len(buf, n);
    }
    ورنہ
	ins_char(cc);

    اگر (enc_utf8)
	// Handle composing chars.
	برائے (;;)
	{
	    c = replace_نکالو();
	    اگر (c == -1)	    // stack empty
		توڑو;
	    اگر ((n = MB_BYTE2LEN(c)) == 1)
	    {
		// Not a multi-byte char, put it back.
		replace_ڈالو(c);
		توڑو;
	    }

	    buf[0] = c;
	    برائے (i = 1; i < n; ++i)
		buf[i] = replace_نکالو();
	    اگر (utf_iscomposing(utf_ptr2char(buf)))
		ins_بائٹیں_len(buf, n);
	    ورنہ
	    {
		// Not a composing char, put it back.
		برائے (i = n - 1; i >= 0; --i)
		    replace_ڈالو(buf[i]);
		توڑو;
	    }

	}
}

/*
 * make the replace stack empty
 * (called when exiting replace mode)
 */
    ساکن خالی
replace_flush(خالی)
{
    VIM_CLEAR(replace_ڈھیر);
    replace_ڈھیر_len = 0;
    replace_ڈھیر_nr = 0;
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
    ساکن خالی
replace_do_bs(صحیح حد_بندی_ستون)
{
    صحیح		cc;
    صحیح		orig_len = 0;
    صحیح		ins_len;
    صحیح		orig_vcols = 0;
    colnr_T	آغاز_vcol;
    char_u	*p;
    صحیح		i;
    صحیح		vcol;

    cc = replace_نکالو();
    اگر (cc > 0)
    {
#اگر_تعریف_شدہ FEAT_PROP_POPUP
	size_t	len_before = 0;  // init to shut up GCC

	اگر (curbuf->b_has_textprop)
	{
	    // Do not adjust text properties for individual delete and insert
	    // operations, do it afterwards on the resulting text.
	    len_before = ml_get_curline_len();
	    ++متن_prop_frozen;
	}
#اگر_ختم
	اگر (State & VREPLACE_FLAG)
	{
	    // Get the number of screen cells used by the character we are
	    // going to delete.
	    getvcol(curwin, &curwin->w_cursor, تہی, &آغاز_vcol, تہی, 0);
	    orig_vcols = chartabsize(ml_get_cursor(), آغاز_vcol);
	}
	اگر (has_mbyte)
	{
	    (خالی)del_char_after_ستون(حد_بندی_ستون);
	    اگر (State & VREPLACE_FLAG)
		orig_len = ml_get_cursor_len();
	    replace_ڈالو(cc);
	}
	ورنہ
	{
	    pchar_cursor(cc);
	    اگر (State & VREPLACE_FLAG)
		orig_len = ml_get_cursor_len() - 1;
	}
	replace_نکالو_ins();

	اگر (State & VREPLACE_FLAG)
	{
	    // Get the number of screen cells used by the inserted characters
	    p = ml_get_cursor();
	    ins_len = ml_get_cursor_len() - orig_len;
	    vcol = آغاز_vcol;
	    برائے (i = 0; i < ins_len; ++i)
	    {
		vcol += chartabsize(p + i, vcol);
		i += (*mb_ptr2len)(p + i) - 1;
	    }
	    vcol -= آغاز_vcol;

	    // Delete spaces that were inserted after the cursor to keep the
	    // text aligned.
	    curwin->w_cursor.ستون += ins_len;
	    جب (vcol > orig_vcols && gchar_cursor() == ' ')
	    {
		del_char(FALSE);
		++orig_vcols;
	    }
	    curwin->w_cursor.ستون -= ins_len;
	}

	// mark the buffer as changed and prepare for displaying
	بدلا_بائٹیں(curwin->w_cursor.lnum, curwin->w_cursor.ستون);

#اگر_تعریف_شدہ FEAT_PROP_POPUP
	اگر (curbuf->b_has_textprop)
	{
	    size_t len_now = ml_get_curline_len();

	    --متن_prop_frozen;
	    adjust_prop_columns(curwin->w_cursor.lnum, curwin->w_cursor.ستون,
					   (صحیح)(len_now - len_before), 0);
	}
#اگر_ختم
    }
    ورنہ اگر (cc == 0)
	(خالی)del_char_after_ستون(حد_بندی_ستون);
}

#اگر defined(FEAT_RIGHTLEFT)
/*
 * Map Hebrew keyboard when in hkmap mode.
 */
    صحیح
hkmap(صحیح c)
{
    اگر (p_hkmapp)   // phonetic mapping, by Ilya Dogolazky
    {
	شمار {hALEF=0, BET, GIMEL, DALET, HEI, VAV, ZAIN, HET, TET, IUD,
	    KAFsofit, hKAF, LAMED, MEMsofit, MEM, NUNsofit, NUN, SAMEH, AIN,
	    PEIsofit, PEI, ZADIsofit, ZADI, KOF, RESH, hSHIN, TAV};
	ساکن char_u map[26] =
	    {(char_u)hALEF/*a*/, (char_u)BET  /*b*/, (char_u)hKAF    /*c*/,
	     (char_u)DALET/*d*/, (char_u)-1   /*e*/, (char_u)PEIsofit/*f*/,
	     (char_u)GIMEL/*g*/, (char_u)HEI  /*h*/, (char_u)IUD     /*i*/,
	     (char_u)HET  /*j*/, (char_u)KOF  /*k*/, (char_u)LAMED   /*l*/,
	     (char_u)MEM  /*m*/, (char_u)NUN  /*n*/, (char_u)SAMEH   /*o*/,
	     (char_u)PEI  /*p*/, (char_u)-1   /*q*/, (char_u)RESH    /*r*/,
	     (char_u)ZAIN /*s*/, (char_u)TAV  /*t*/, (char_u)TET     /*u*/,
	     (char_u)VAV  /*v*/, (char_u)hSHIN/*w*/, (char_u)-1      /*x*/,
	     (char_u)AIN  /*y*/, (char_u)ZADI /*z*/};

	اگر (c == 'N' || c == 'M' || c == 'P' || c == 'C' || c == 'Z')
	    واپس (صحیح)(map[CharOrd(c)] - 1 + p_aleph);
							    // '-1'='sofit'
	ورنہ اگر (c == 'x')
	    واپس 'X';
	ورنہ اگر (c == 'q')
	    واپس '\''; // {geresh}={'}
	ورنہ اگر (c == 246)
	    واپس ' ';  // \"o --> ' ' for a german keyboard
	ورنہ اگر (c == 228)
	    واپس ' ';  // \"a --> ' '      -- / --
	ورنہ اگر (c == 252)
	    واپس ' ';  // \"u --> ' '      -- / --
	// NOTE: islower() does not do the right thing for us on Linux so we
	// do this the same was as 5.7 and previous, so it works correctly on
	// all systems.  Specifically, the e.g. Delete and Arrow keys are
	// munged and won't work if e.g. searching for Hebrew text.
	ورنہ اگر (c >= 'a' && c <= 'z')
	    واپس (صحیح)(map[Char_Ord_کم(c)] + p_aleph);
	ورنہ
	    واپس c;
    }
    ورنہ
    {
	انتخاب (c)
	{
	    حالت '`':	واپس ';';
	    حالت '/':	واپس '.';
	    حالت '\'':	واپس ',';
	    حالت 'q':	واپس '/';
	    حالت 'w':	واپس '\'';

			// Hebrew letters - set offset from 'a'
	    حالت ',':	c = '{'; توڑو;
	    حالت '.':	c = 'v'; توڑو;
	    حالت ';':	c = 't'; توڑو;
	    طےشدہ: {
			 ساکن حرف str[] = "zqbcxlsjphmkwonu ydafe rig";

			 اگر (c < 'a' || c > 'z')
			     واپس c;
			 c = str[Char_Ord_کم(c)];
			 توڑو;
		     }
	}

	واپس (صحیح)(Char_Ord_کم(c) + p_aleph);
    }
}
#اگر_ختم

    ساکن خالی
ins_reg(خالی)
{
    صحیح		need_redraw = FALSE;
    صحیح		regname;
    صحیح		literally = 0;
    صحیح		vis_فعال = VIsual_فعال;

    /*
     * If we are going to wait for a character, show a '"'.
     */
    pc_حالت = PC_حالت_UNSET;
    اگر (redrawing() && !char_avail())
    {
	// may need to redraw when no more chars available now
	ins_redraw(FALSE);

	edit_putchar('"', TRUE);
	جوڑو_مقصد_showcmd_c(Ctrl_R);
    }

#اگر_تعریف_شدہ USE_ON_FLY_SCROLL
    dont_scroll = TRUE;		// disallow scrolling here
#اگر_ختم

    /*
     * Don't map the register name. This also prevents the mode message to be
     * deleted when ESC is hit.
     */
    ++no_mapping;
    ++allow_کلیدیں;
    regname = plain_vgetc();
    LANGMAP_ADJUST(regname, TRUE);
    اگر (regname == Ctrl_R || regname == Ctrl_O || regname == Ctrl_P)
    {
	// Get a third key for literal register insertion
	literally = regname;
	جوڑو_مقصد_showcmd_c(literally);
	regname = plain_vgetc();
	LANGMAP_ADJUST(regname, TRUE);
    }
    --no_mapping;
    --allow_کلیدیں;

#اگر_تعریف_شدہ FEAT_EVAL
    // Don't call u_sync() while typing the expression or giving an error
    // message for it. Only call it explicitly.
    ++no_u_sync;
    اگر (regname == '=')
    {
	pos_T	curpos = curwin->w_cursor;
# ifdef HAVE_داخلہ_طریقہ
	صحیح	im_on = im_get_حالت();
# endif
	// Sync undo when evaluating the expression calls setline() or
	// append(), so that it can be undone separately.
	u_sync_once = 2;

	regname = get_اظہاریہ_رجسٹر();

	// Cursor may be moved back a column.
	curwin->w_cursor = curpos;
	check_cursor();
# ifdef HAVE_داخلہ_طریقہ
	// Restore the Input Method.
	اگر (im_on)
	    im_سیٹ_فعال(TRUE);
# endif
    }
    اگر (regname == NUL || !valid_yank_reg(regname, FALSE))
    {
	vim_beep(BO_REG);
	need_redraw = TRUE;	// remove the '"'
    }
    ورنہ
    {
#اگر_ختم
	اگر (literally == Ctrl_O || literally == Ctrl_P)
	{
	    // Append the command to the redo buffer.
	    Append_Char_مقصد_Redobuff(Ctrl_R);
	    Append_Char_مقصد_Redobuff(literally);
	    Append_Char_مقصد_Redobuff(regname);

	    do_put(regname, تہی, BACKWARD, 1L,
		 (literally == Ctrl_P ? PUT_FIXINDENT : 0) | PUT_CURSEND);
	}
	ورنہ اگر (داخل_کرو_reg(regname, literally) == FAIL)
	{
	    vim_beep(BO_REG);
	    need_redraw = TRUE;	// remove the '"'
	}
	ورنہ اگر (stop_داخل_کرو_منوال)
	    // When the '=' register was used and a function was invoked that
	    // did ":stopinsert" then stuff_empty() returns FALSE but we won't
	    // insert anything, need to remove the '"'
	    need_redraw = TRUE;

#اگر_تعریف_شدہ FEAT_EVAL
    }
    --no_u_sync;
    اگر (u_sync_once == 1)
	ins_need_undo = TRUE;
    u_sync_once = 0;
#اگر_ختم
    clear_showcmd();

    // If the inserted register is empty, we need to remove the '"'
    اگر (need_redraw || stuff_خالی_خانے())
	edit_unputchar();

    // Disallow starting Visual mode here, would get a weird mode.
    اگر (!vis_فعال && VIsual_فعال)
	انتہا_visual_منوال();
}

/*
 * CTRL-G commands in Insert mode.
 */
    ساکن خالی
ins_ctrl_g(خالی)
{
    صحیح		c;

    // Right after CTRL-X the cursor will be after the ruler.
    setcursor();

    /*
     * Don't map the second key. This also prevents the mode message to be
     * deleted when ESC is hit.
     */
    ++no_mapping;
    ++allow_کلیدیں;
    c = plain_vgetc();
    --no_mapping;
    --allow_کلیدیں;
    انتخاب (c)
    {
	// CTRL-G k and CTRL-G <Up>: cursor up to Insstart.col
	حالت K_اوپر:
	حالت Ctrl_K:
	حالت 'k': ins_اوپر(TRUE);
		  توڑو;

	// CTRL-G j and CTRL-G <Down>: cursor down to Insstart.col
	حالت K_نیچے_کو:
	حالت Ctrl_J:
	حالت 'j': ins_نیچے_کو(TRUE);
		  توڑو;

	// CTRL-G u: start new undoable edit
	حالت 'u': u_sync(TRUE);
		  ins_need_undo = TRUE;

		  // Need to reset Insstart, esp. because a BS that joins
		  // a line to the previous one must save for undo.
		  update_Insstart_orig = FALSE;
		  Insstart = curwin->w_cursor;
		  توڑو;

	// CTRL-G U: do not break undo with the next char
	حالت 'U':
		  // Allow one left/right cursor movement with the next char,
		  // without breaking undo.
		  dont_sync_undo = MAYBE;
		  توڑو;

	حالت ESC:
		  // Esc after CTRL-G cancels it.
		  توڑو;

	// Unknown CTRL-G command, reserved for future expansion.
	طےشدہ:  vim_beep(BO_CTRLG);
    }
}

/*
 * CTRL-^ in Insert mode.
 */
    ساکن خالی
ins_ctrl_hat(خالی)
{
    اگر (map_مقصد_exists_منوال((char_u *)"", منوال_LANGMAP, FALSE))
    {
	// ":lmap" mappings exists, Toggle use of ":lmap" mappings.
	اگر (State & منوال_LANGMAP)
	{
	    curbuf->b_p_iminsert = B_IMODE_NONE;
	    State &= ~منوال_LANGMAP;
	}
	ورنہ
	{
	    curbuf->b_p_iminsert = B_IMODE_LMAP;
	    State |= منوال_LANGMAP;
#اگر_تعریف_شدہ HAVE_داخلہ_طریقہ
	    im_سیٹ_فعال(FALSE);
#اگر_ختم
	}
    }
#اگر_تعریف_شدہ HAVE_داخلہ_طریقہ
    ورنہ
    {
	// There are no ":lmap" mappings, toggle IM
	اگر (im_get_حالت())
	{
	    curbuf->b_p_iminsert = B_IMODE_NONE;
	    im_سیٹ_فعال(FALSE);
	}
	ورنہ
	{
	    curbuf->b_p_iminsert = B_IMODE_IM;
	    State &= ~منوال_LANGMAP;
	    im_سیٹ_فعال(TRUE);
	}
    }
#اگر_ختم
    سیٹ_iminsert_global();
    showmode();
#اگر_تعریف_شدہ FEAT_GUI
    // may show different cursor shape or color
    اگر (gui.اندر_use)
	gui_update_cursor(TRUE, FALSE);
#اگر_ختم
#اگر defined(FEAT_KEYMAP)
    // Show/unshow value of 'keymap' in status lines.
    حالت_redraw_curbuf();
#اگر_ختم
}

/*
 * Handle ESC in insert mode.
 * Returns TRUE when leaving insert mode, FALSE when going to repeat the
 * insert.
 */
    ساکن صحیح
ins_esc(
    طویل	*گنتی,
    صحیح		cmdchar,
    صحیح		nomove)	    // don't move cursor
{
    صحیح		عارضی;
    ساکن صحیح	disabled_redraw = FALSE;
#اگر_تعریف_شدہ FEAT_CONCEAL
    // Remember if the cursor line was concealed before changing State.
    صحیح		cursor_سطر_was_concealed = curwin->w_p_cole > 0
						&& conceal_cursor_سطر(curwin);
#اگر_ختم

#اگر_تعریف_شدہ FEAT_SPELL
    check_spell_redraw();
#اگر_ختم

    عارضی = curwin->w_cursor.ستون;
    اگر (disabled_redraw)
    {
	اگر (RedrawingDisabled > 0)
	    --RedrawingDisabled;
	disabled_redraw = FALSE;
    }
    اگر (!arrow_مستعمل)
    {
	/*
	 * Don't append the ESC for "r<CR>" and "grx".
	 * When 'insertmode' is set only CTRL-L stops Insert mode.  Needed for
	 * when "count" is non-zero.
	 */
	اگر (cmdchar != 'r' && cmdchar != 'v')
	    Append_مقصد_Redobuff(p_im ? (char_u *)"\014" : ESC_STR);

	/*
	 * Repeating insert may take a long time.  Check for
	 * interrupt now and then.
	 */
	اگر (*گنتی > 0)
	{
	    سطر_breakcheck();
	    اگر (got_int)
		*گنتی = 0;
	}

	اگر (--*گنتی > 0)	// repeat what was typed
	{
	    // Vi repeats the insert without replacing characters.
	    اگر (vim_strchr(p_cpo, CPO_REPLCNT) != تہی)
		State &= ~REPLACE_FLAG;

	    (خالی)آغاز_redo_ins();
	    اگر (cmdchar == 'r' || cmdchar == 'v')
		stuffRedoReadbuff(ESC_STR);	// no ESC in redo buffer
	    ++RedrawingDisabled;
	    disabled_redraw = TRUE;
	    واپس FALSE;	// repeat the insert
	}
	stop_داخل_کرو(&curwin->w_cursor, TRUE, nomove);
	undisplay_dollar();
    }

    اگر (cmdchar != 'r' && cmdchar != 'v')
	ins_apply_autocmds(EVENT_INSERTLEAVEPRE);

    // When an autoindent was removed, curswant stays after the
    // indent
    اگر (restart_edit == NUL && (colnr_T)عارضی == curwin->w_cursor.ستون)
	curwin->w_سیٹ_curswant = true;

    // Remember the last Insert position in the '^ mark.
    اگر ((cmdmod.cmod_flags & CMOD_KEEPJUMPS) == 0)
	curbuf->b_آخری_داخل_کرو = curwin->w_cursor;

    /*
     * The cursor should end up on the last inserted character.
     * Don't do it for CTRL-O, unless past the end of the line.
     */
    اگر (!nomove
	    && (curwin->w_cursor.ستون != 0
		|| curwin->w_cursor.coladd > 0)
	    && (restart_edit == NUL
		   || (gchar_cursor() == NUL && !VIsual_فعال))
#اگر_تعریف_شدہ FEAT_RIGHTLEFT
	    && !revins_on
#اگر_ختم
				      )
    {
	اگر (curwin->w_cursor.coladd > 0 || get_ve_flags() == VE_ALL)
	{
	    oneleft();
	    اگر (restart_edit != NUL)
		++curwin->w_cursor.coladd;
	}
	ورنہ
	{
	    --curwin->w_cursor.ستون;
	    curwin->w_valid &= ~(VALID_WCOL|VALID_VIRTCOL);
	    // Correct cursor for multi-byte character.
	    اگر (has_mbyte)
		mb_adjust_cursor();
	}
    }

#اگر_تعریف_شدہ HAVE_داخلہ_طریقہ
    // Disable IM to allow typing English directly for Normal mode commands.
    // When ":lmap" is enabled don't change 'iminsert' (IM can be enabled as
    // well).
    اگر (!(State & منوال_LANGMAP))
	im_save_حالت(&curbuf->b_p_iminsert);
    im_سیٹ_فعال(FALSE);
#اگر_ختم

    State = منوال_معمول;
    may_trigger_modechanged();
    // need to position cursor again when on a TAB and when on a char with
    // virtual text.
    اگر (gchar_cursor() == TAB
#اگر_تعریف_شدہ FEAT_PROP_POPUP
	    || curbuf->b_has_textprop
#اگر_ختم
       )
	curwin->w_valid &= ~(VALID_WROW|VALID_WCOL|VALID_VIRTCOL);

    setmouse();
#اگر_تعریف_شدہ CURSOR_SHAPE
    ui_cursor_shape();		// may show different cursor shape
#اگر_ختم
    اگر (!p_ek)
    {
	MAY_WANT_مقصد_LOG_THIS;

	// Re-enable bracketed paste mode.
	برآمد_str_t_BE();

	// Re-enable modifyOtherKeys.
	برآمد_str_t_TI();
    }
#اگر_تعریف_شدہ FEAT_CONCEAL
    // Check if the cursor line needs redrawing after changing State.  If
    // 'concealcursor' is "i" it needs to be redrawn without concealing.
    conceal_check_cursor_سطر(cursor_سطر_was_concealed);
#اگر_ختم

    // When recording or for CTRL-O, need to display the new mode.
    // Otherwise remove the mode message.
    اگر (reg_recording != 0 || restart_edit != NUL)
	showmode();
    ورنہ اگر (p_smd && (got_int || !skip_showmode()))
	پیغام("");

    واپس TRUE;	    // exit Insert mode
}

#اگر_تعریف_شدہ FEAT_RIGHTLEFT
/*
 * Toggle language: hkmap and revins_on.
 * Move to end of reverse inserted text.
 */
    ساکن خالی
ins_ctrl_(خالی)
{
    اگر (revins_on && revins_chars && revins_scol >= 0)
    {
	جب (gchar_cursor() != NUL && revins_chars--)
	    ++curwin->w_cursor.ستون;
    }
    p_ri = !p_ri;
    revins_on = (State == منوال_داخل_کرو && p_ri);
    اگر (revins_on)
    {
	revins_scol = curwin->w_cursor.ستون;
	revins_legal++;
	revins_chars = 0;
	undisplay_dollar();
    }
    ورنہ
	revins_scol = -1;
    p_hkmap = curwin->w_p_rl ^ p_ri;    // be consistent!
    showmode();
}
#اگر_ختم

/*
 * If 'keymodel' contains "startsel", may start selection.
 * Returns TRUE when a CTRL-O and other keys stuffed.
 */
    ساکن صحیح
ins_آغاز_select(صحیح c)
{
    اگر (!km_startsel)
	واپس FALSE;
    انتخاب (c)
    {
	حالت K_KHOME:
	حالت K_KEND:
	حالت K_PAGEUP:
	حالت K_KPAGEUP:
	حالت K_PAGEDOWN:
	حالت K_KPAGEDOWN:
#اگر_تعریف_شدہ MACOS_X
	حالت K_بایاں:
	حالت K_دایاں:
	حالت K_اوپر:
	حالت K_نیچے_کو:
	حالت K_انتہا:
	حالت K_HOME:
#اگر_ختم
	    اگر (!(mod_نقاب & MOD_نقاب_سرکاؤ))
		توڑو;
	    // FALLTHROUGH
	حالت K_S_بایاں:
	حالت K_S_دایاں:
	حالت K_S_اوپر:
	حالت K_S_نیچے_کو:
	حالت K_S_انتہا:
	حالت K_S_HOME:
	    // Start selection right away, the cursor can move with CTRL-O when
	    // beyond the end of the line.
	    آغاز_selection();

	    // Execute the key in (insert) Select mode.
	    stuffcharReadbuff(Ctrl_O);
	    اگر (mod_نقاب)
	    {
		char_u	    buf[4];

		buf[0] = K_SPECIAL;
		buf[1] = KS_MODIFIER;
		buf[2] = mod_نقاب;
		buf[3] = NUL;
		stuffReadbuffLen(buf, 3L);
	    }
	    stuffcharReadbuff(c);
	    واپس TRUE;
    }
    واپس FALSE;
}

/*
 * <Insert> key in Insert mode: toggle insert/replace mode.
 */
    ساکن خالی
ins_داخل_کرو(صحیح replaceState)
{
#اگر_تعریف_شدہ FEAT_EVAL
    سیٹ_vim_var_string(VV_INSERTMODE,
		   (char_u *)((State & REPLACE_FLAG) ? "i"
			    : replaceState == منوال_VREPLACE ? "v" : "r"), 1);
#اگر_ختم
    ins_apply_autocmds(EVENT_INSERTCHANGE);
    اگر (State & REPLACE_FLAG)
	State = منوال_داخل_کرو | (State & منوال_LANGMAP);
    ورنہ
	State = replaceState | (State & منوال_LANGMAP);
    may_trigger_modechanged();
    Append_Char_مقصد_Redobuff(K_INS);
    showmode();
#اگر_تعریف_شدہ CURSOR_SHAPE
    ui_cursor_shape();		// may show different cursor shape
#اگر_ختم
}

/*
 * Pressed CTRL-O in Insert mode.
 */
    ساکن خالی
ins_ctrl_o(خالی)
{
    restart_VIsual_select = 0;
    اگر (State & VREPLACE_FLAG)
	restart_edit = 'V';
    ورنہ اگر (State & REPLACE_FLAG)
	restart_edit = 'R';
    ورنہ
	restart_edit = 'I';
    اگر (virtual_فعال())
	ins_at_eol = FALSE;	// cursor always keeps its column
    ورنہ
	ins_at_eol = (gchar_cursor() == NUL);
}

/*
 * If the cursor is on an indent, ^T/^D insert/delete one
 * shiftwidth.	Otherwise ^T/^D behave like a "<<" or ">>".
 * Always round the indent to 'shiftwidth', this is compatible
 * with vi.  But vi only supports ^T and ^D after an
 * autoindent, we support it everywhere.
 */
    ساکن خالی
ins_سرکاؤ(صحیح c, صحیح lastc)
{
    اگر (stop_arrow() == FAIL)
	واپس;
    Append_Char_مقصد_Redobuff(c);

    /*
     * 0^D and ^^D: remove all indent.
     */
    اگر (c == Ctrl_D && (lastc == '0' || lastc == '^')
						  && curwin->w_cursor.ستون > 0)
    {
	--curwin->w_cursor.ستون;
	(خالی)del_char(FALSE);		// delete the '^' or '0'
	// In Replace mode, restore the characters that '^' or '0' replaced.
	اگر (State & REPLACE_FLAG)
	    replace_نکالو_ins();
	اگر (lastc == '^')
	    old_indent = get_indent();	// remember curr. indent
	change_indent(INDENT_سیٹ, 0, TRUE, 0, TRUE);
    }
    ورنہ
	change_indent(c == Ctrl_D ? INDENT_DEC : INDENT_INC, 0, TRUE, 0, TRUE);

    اگر (did_ai && *skipwhite(ml_get_curline()) != NUL)
	did_ai = FALSE;
    did_si = FALSE;
    can_si = FALSE;
    can_si_عقب = FALSE;
    can_cindent = FALSE;	// no cindenting after ^D or ^T
}

    ساکن خالی
ins_del(خالی)
{
    صحیح	    عارضی;

    اگر (stop_arrow() == FAIL)
	واپس;
    اگر (gchar_cursor() == NUL)		// delete newline
    {
	عارضی = curwin->w_cursor.ستون;
	اگر (!can_bs(BS_EOL)		// only if "eol" included
		|| do_join(2, FALSE, TRUE, FALSE, FALSE) == FAIL)
	    vim_beep(BO_BS);
	ورنہ
	{
	    curwin->w_cursor.ستون = عارضی;
	    // Adjust orig_line_count in case more lines have been deleted than
	    // have been added. That makes sure, that open_line() later
	    // can access all buffer lines correctly
	    اگر (State & VREPLACE_FLAG &&
		    orig_سطر_گنتی > curbuf->b_ml.ml_سطر_گنتی)
		orig_سطر_گنتی = curbuf->b_ml.ml_سطر_گنتی;
	}
    }
    ورنہ اگر (del_char(FALSE) == FAIL)  // delete char under cursor
	vim_beep(BO_BS);
    did_ai = FALSE;
    did_si = FALSE;
    can_si = FALSE;
    can_si_عقب = FALSE;
    Append_Char_مقصد_Redobuff(K_DEL);
}

/*
 * Delete one character for ins_bs().
 */
    ساکن خالی
ins_bs_one(خالی)
{
    dec_cursor();
    اگر (State & REPLACE_FLAG)
    {
	// Don't delete characters before the insert point when in
	// Replace mode
	اگر (curwin->w_cursor.lnum != Insstart.lnum
		|| curwin->w_cursor.ستون >= Insstart.ستون)
	    replace_do_bs(-1);
    }
    ورنہ
	(خالی)del_char(FALSE);
}

/*
 * Handle Backspace, delete-word and delete-line in Insert mode.
 * Return TRUE when backspace was actually used.
 */
    ساکن صحیح
ins_bs(
    صحیح		c,
    صحیح		منوال,
    صحیح		*inserted_space_p)
{
    linenr_T	lnum;
    صحیح		cc;
    صحیح		عارضی = 0;	    // init for GCC
    colnr_T	save_ستون;
    colnr_T	mincol;
    صحیح		did_backspace = FALSE;
    صحیح		اندر_indent;
    صحیح		oldState;
    صحیح		cpc[MAX_MCO];	    // composing characters
    صحیح		call_fix_indent = FALSE;

    /*
     * can't delete anything in an empty file
     * can't backup past first character in buffer
     * can't backup past starting point unless 'backspace' > 1
     * can backup to a previous line if 'backspace' == 0
     */
    اگر (       BUFEMPTY()
	    || (
#اگر_تعریف_شدہ FEAT_RIGHTLEFT
		!revins_on &&
#اگر_ختم
		((curwin->w_cursor.lnum == 1 && curwin->w_cursor.ستون == 0)
		    || (!can_bs(BS_آغاز)
			&& ((arrow_مستعمل
#اگر_تعریف_شدہ FEAT_کام_CHANNEL
				&& !bt_prompt(curbuf)
#اگر_ختم
			) || (curwin->w_cursor.lnum == Insstart_orig.lnum
				&& curwin->w_cursor.ستون <= Insstart_orig.ستون)))
		    || (!can_bs(BS_INDENT) && !arrow_مستعمل && ai_ستون > 0
					 && curwin->w_cursor.ستون <= ai_ستون)
		    || (!can_bs(BS_EOL) && curwin->w_cursor.ستون == 0))))
    {
	vim_beep(BO_BS);
	واپس FALSE;
    }

    اگر (stop_arrow() == FAIL)
	واپس FALSE;
    اندر_indent = inindent(0);
    اگر (اندر_indent)
	can_cindent = FALSE;
    انتہا_comment_pending = NUL;	// After BS, don't auto-end comment
#اگر_تعریف_شدہ FEAT_RIGHTLEFT
    اگر (revins_on)	    // put cursor after last inserted char
	inc_cursor();
#اگر_ختم

    // Virtualedit:
    //	BACKSPACE_CHAR eats a virtual space
    //	BACKSPACE_WORD eats all coladd
    //	BACKSPACE_LINE eats all coladd and keeps going
    اگر (curwin->w_cursor.coladd > 0)
    {
	اگر (منوال == BACKSPACE_CHAR)
	{
	    --curwin->w_cursor.coladd;
	    واپس TRUE;
	}
	اگر (منوال == BACKSPACE_لفظ)
	{
	    curwin->w_cursor.coladd = 0;
	    واپس TRUE;
	}
	curwin->w_cursor.coladd = 0;
    }

    /*
     * Delete newline!
     */
    اگر (curwin->w_cursor.ستون == 0)
    {
	lnum = Insstart.lnum;
	اگر (curwin->w_cursor.lnum == lnum
#اگر_تعریف_شدہ FEAT_RIGHTLEFT
			|| revins_on
#اگر_ختم
				    )
	{
	    اگر (u_save((linenr_T)(curwin->w_cursor.lnum - 2),
			       (linenr_T)(curwin->w_cursor.lnum + 1)) == FAIL)
		واپس FALSE;
	    --Insstart.lnum;
	    Insstart.ستون = ml_get_len(Insstart.lnum);
	}
	/*
	 * In replace mode:
	 * cc < 0: NL was inserted, delete it
	 * cc >= 0: NL was replaced, put original characters back
	 */
	cc = -1;
	اگر (State & REPLACE_FLAG)
	    cc = replace_نکالو();	    // returns -1 if NL was inserted
	/*
	 * In replace mode, in the line we started replacing, we only move the
	 * cursor.
	 */
	اگر ((State & REPLACE_FLAG) && curwin->w_cursor.lnum <= lnum)
	{
	    dec_cursor();
	}
	ورنہ
	{
	    اگر (!(State & VREPLACE_FLAG)
				   || curwin->w_cursor.lnum > orig_سطر_گنتی)
	    {
		عارضی = gchar_cursor();	// remember current char
		--curwin->w_cursor.lnum;

		// When "aw" is in 'formatoptions' we must delete the space at
		// the end of the line, otherwise the line will be broken
		// again when auto-formatting.
		اگر (has_format_option(FO_AUTO)
					   && has_format_option(FO_WHITE_PAR))
		{
		    char_u  *ptr = ml_get_buf(curbuf, curwin->w_cursor.lnum,
									FALSE);
		    صحیح	    len = ml_get_curline_len();

		    اگر (len > 0 && ptr[len - 1] == ' ')
		    {
			char_u *newp = alloc(curbuf->b_ml.ml_سطر_len - 1);

			اگر (newp != تہی)
			{
			    mch_memmove(newp, ptr, len - 1);
			    newp[len - 1] = NUL;
			    اگر (curbuf->b_ml.ml_سطر_len > len + 1)
				mch_memmove(newp + len, ptr + len + 1,
					   curbuf->b_ml.ml_سطر_len - len - 1);

			    اگر (curbuf->b_ml.ml_flags
					      & (ML_سطر_DIRTY | ML_ALLOCATED))
				vim_free(curbuf->b_ml.ml_سطر_ptr);
			    curbuf->b_ml.ml_سطر_ptr = newp;
			    curbuf->b_ml.ml_سطر_len--;
			    curbuf->b_ml.ml_سطر_textlen--;
			    curbuf->b_ml.ml_flags |= ML_سطر_DIRTY;
			}
		    }
		}

		(خالی)do_join(2, FALSE, FALSE, FALSE, FALSE);
		اگر (عارضی == NUL && gchar_cursor() != NUL)
		    inc_cursor();
	    }
	    ورنہ
		dec_cursor();

	    /*
	     * In MODE_REPLACE mode we have to put back the text that was
	     * replaced by the NL. On the replace stack is first a
	     * NUL-terminated sequence of characters that were deleted and then
	     * the characters that NL replaced.
	     */
	    اگر (State & REPLACE_FLAG)
	    {
		/*
		 * Do the next ins_char() in MODE_NORMAL state, to
		 * prevent ins_char() from replacing characters and
		 * avoiding showmatch().
		 */
		oldState = State;
		State = منوال_معمول;
		/*
		 * restore characters (blanks) deleted after cursor
		 */
		جب (cc > 0)
		{
		    save_ستون = curwin->w_cursor.ستون;
		    mb_replace_نکالو_ins(cc);
		    curwin->w_cursor.ستون = save_ستون;
		    cc = replace_نکالو();
		}
		// restore the characters that NL replaced
		replace_نکالو_ins();
		State = oldState;
	    }
	}
	did_ai = FALSE;
    }
    ورنہ
    {
	/*
	 * Delete character(s) before the cursor.
	 */
#اگر_تعریف_شدہ FEAT_RIGHTLEFT
	اگر (revins_on)		// put cursor on last inserted char
	    dec_cursor();
#اگر_ختم
	mincol = 0;
						// keep indent
	اگر (منوال == BACKSPACE_سطر
		&& (curbuf->b_p_ai || cindent_on())
#اگر_تعریف_شدہ FEAT_RIGHTLEFT
		&& !revins_on
#اگر_ختم
			    )
	{
	    save_ستون = curwin->w_cursor.ستون;
	    beginline(BL_WHITE);
	    اگر (curwin->w_cursor.ستون < save_ستون)
	    {
		mincol = curwin->w_cursor.ستون;
		// should now fix the indent to match with the previous line
		call_fix_indent = TRUE;
	    }
	    curwin->w_cursor.ستون = save_ستون;
	}

	/*
	 * Handle deleting one 'shiftwidth' or 'softtabstop'.
	 */
	اگر (	   منوال == BACKSPACE_CHAR
		&& ((p_sta && اندر_indent)
		    || ((get_sts_قدر() != 0
#اگر_تعریف_شدہ FEAT_VARTABS
			|| tabstop_گنتی(curbuf->b_p_vsts_array)
#اگر_ختم
			)
			&& curwin->w_cursor.ستون > 0
			&& (*(ml_get_cursor() - 1) == TAB
			    || (*(ml_get_cursor() - 1) == ' '
				&& (!*inserted_space_p
				    || arrow_مستعمل))))))
	{
	    colnr_T	vcol = 0;
	    colnr_T	want_vcol;
	    char_u	*سطر;
	    char_u	*ptr;
	    char_u	*cursor_ptr;
	    char_u	*space_ptr;
	    colnr_T	space_vcol = 0;
	    صحیح		پچھلا_space = FALSE;
	    colnr_T	want_ستون;

	    *inserted_space_p = FALSE;

	    space_ptr = ptr = سطر = ml_get_curline();
	    cursor_ptr = سطر + curwin->w_cursor.ستون;

	    // Compute virtual column of cursor position, and find the last
	    // whitespace before cursor that is preceded by non-whitespace.
	    // Use chartabsize() so that virtual text and wrapping are ignored.
	    جب (ptr < cursor_ptr)
	    {
		صحیح	cur_space = VIM_ISWHITE(*ptr);

		اگر (!پچھلا_space && cur_space)
		{
		    space_ptr = ptr;
		    space_vcol = vcol;
		}
		vcol += chartabsize(ptr, vcol);
		MB_PTR_ADV(ptr);
		پچھلا_space = cur_space;
	    }

	    // Compute the virtual column where we want to be.
	    want_vcol = vcol > 0 ? vcol - 1 : 0;
	    اگر (p_sta && اندر_indent)
		want_vcol -= want_vcol % (صحیح)get_sw_قدر(curbuf);
	    ورنہ
#اگر_تعریف_شدہ FEAT_VARTABS
		want_vcol = tabstop_آغاز(want_vcol, get_sts_قدر(),
						       curbuf->b_p_vsts_array);
#ورنہ
		want_vcol -= want_vcol % (صحیح)get_sts_قدر();
#اگر_ختم

	    // Find the position to stop backspacing.
	    // Use chartabsize() so that virtual text and wrapping are ignored.
	    جب (TRUE)
	    {
		صحیح سائز = chartabsize(space_ptr, space_vcol);

		اگر (space_vcol + سائز > want_vcol)
		    توڑو;
		space_vcol += سائز;
		MB_PTR_ADV(space_ptr);
	    }
	    want_ستون = space_ptr - سطر;

	    // Delete characters until we are at or before want_col.
	    جب (curwin->w_cursor.ستون > want_ستون)
		ins_bs_one();

	    // Insert extra spaces until we are at want_vcol.
	    برائے (; space_vcol < want_vcol; space_vcol++)
	    {
		// Remember the first char we inserted
		اگر (curwin->w_cursor.lnum == Insstart_orig.lnum
				   && curwin->w_cursor.ستون < Insstart_orig.ستون)
		    Insstart_orig.ستون = curwin->w_cursor.ستون;

		اگر (State & VREPLACE_FLAG)
		    ins_char(' ');
		ورنہ
		{
		    ins_str((char_u *)" ", 1);
		    اگر ((State & REPLACE_FLAG))
			replace_ڈالو(NUL);
		}
	    }
	}

	/*
	 * Delete up to starting point, start of line or previous word.
	 */
	ورنہ
	{
	    صحیح cclass = 0, پچھلا_cclass = 0;

	    اگر (has_mbyte)
		cclass = mb_get_class(ml_get_cursor());
	    کرو
	    {
#اگر_تعریف_شدہ FEAT_RIGHTLEFT
		اگر (!revins_on) // put cursor on char to be deleted
#اگر_ختم
		    dec_cursor();

		cc = gchar_cursor();
		// look multi-byte character class
		اگر (has_mbyte)
		{
		    پچھلا_cclass = cclass;
		    cclass = mb_get_class(ml_get_cursor());
		}

		// start of word?
		اگر (منوال == BACKSPACE_لفظ && !vim_isspace(cc))
		{
		    منوال = BACKSPACE_لفظ_NOT_SPACE;
		    عارضی = vim_iswordc(cc);
		}
		// end of word?
		ورنہ اگر (منوال == BACKSPACE_لفظ_NOT_SPACE
			&& ((vim_isspace(cc) || vim_iswordc(cc) != عارضی)
			|| پچھلا_cclass != cclass))
		{
#اگر_تعریف_شدہ FEAT_RIGHTLEFT
		    اگر (!revins_on)
#اگر_ختم
			inc_cursor();
#اگر_تعریف_شدہ FEAT_RIGHTLEFT
		    ورنہ اگر (State & REPLACE_FLAG)
			dec_cursor();
#اگر_ختم
		    توڑو;
		}
		اگر (State & REPLACE_FLAG)
		    replace_do_bs(-1);
		ورنہ
		{
		    اگر (enc_utf8 && p_deco)
			(خالی)utfc_ptr2char(ml_get_cursor(), cpc);
		    (خالی)del_char(FALSE);
		    /*
		     * If there are combining characters and 'delcombine' is set
		     * move the cursor back.  Don't back up before the base
		     * character.
		     */
		    اگر (enc_utf8 && p_deco && cpc[0] != NUL)
			inc_cursor();
#اگر_تعریف_شدہ FEAT_RIGHTLEFT
		    اگر (revins_chars)
		    {
			revins_chars--;
			revins_legal++;
		    }
		    اگر (revins_on && gchar_cursor() == NUL)
			توڑو;
#اگر_ختم
		}
		// Just a single backspace?:
		اگر (منوال == BACKSPACE_CHAR)
		    توڑو;
	    } جب (
#اگر_تعریف_شدہ FEAT_RIGHTLEFT
		    revins_on ||
#اگر_ختم
		    (curwin->w_cursor.ستون > mincol
		    &&  (can_bs(BS_NOSTOP)
			|| (curwin->w_cursor.lnum != Insstart_orig.lnum
			|| curwin->w_cursor.ستون != Insstart_orig.ستون)
		    )));
	}
	did_backspace = TRUE;
    }
    did_si = FALSE;
    can_si = FALSE;
    can_si_عقب = FALSE;
    اگر (curwin->w_cursor.ستون <= 1)
	did_ai = FALSE;

    اگر (call_fix_indent)
	fix_indent();

    /*
     * It's a little strange to put backspaces into the redo
     * buffer, but it makes auto-indent a lot easier to deal
     * with.
     */
    Append_Char_مقصد_Redobuff(c);

    // If deleted before the insertion point, adjust it
    اگر (curwin->w_cursor.lnum == Insstart_orig.lnum
				  && curwin->w_cursor.ستون < Insstart_orig.ستون)
	Insstart_orig.ستون = curwin->w_cursor.ستون;

    // vi behaviour: the cursor moves backward but the character that
    //		     was there remains visible
    // Vim behaviour: the cursor moves backward and the character that
    //		      was there is erased from the screen.
    // We can emulate the vi behaviour by pretending there is a dollar
    // displayed even when there isn't.
    //  --pkv Sun Jan 19 01:56:40 EST 2003
    اگر (vim_strchr(p_cpo, CPO_BACKSPACE) != تہی && dollar_vcol == -1)
	dollar_vcol = curwin->w_virtcol;

#اگر_تعریف_شدہ FEAT_FOLDING
    // When deleting a char the cursor line must never be in a closed fold.
    // E.g., when 'foldmethod' is indent and deleting the first non-white
    // char before a Tab.
    اگر (did_backspace)
	foldOpenCursor();
#اگر_ختم

    واپس did_backspace;
}

/*
 * Handle receiving P_PS: start paste mode.  Inserts the following text up to
 * P_PE literally.
 * When "drop" is TRUE then consume the text and drop it.
 */
    صحیح
bracketed_paste(paste_منوال_T منوال, صحیح drop, garray_T *gap)
{
    صحیح		c;
    char_u	buf[NUMBUFLEN + MB_MAXBYTES];
    صحیح		idx = 0;
    char_u	*انتہا = ڈھونڈو_termcode((char_u *)"PE");
    صحیح		ret_char = -1;
    صحیح		save_allow_کلیدیں = allow_کلیدیں;
    صحیح		save_paste = p_paste;

    // If the end code is too long we can't detect it, read everything.
    اگر (انتہا != تہی && STRLEN(انتہا) >= NUMBUFLEN)
	انتہا = تہی;
    ++no_mapping;
    allow_کلیدیں = 0;
    اگر (!p_paste)
	// Also have the side effects of setting 'paste' to make it work much
	// faster.
	سیٹ_option_قدر_give_err((char_u *)"paste", TRUE, تہی, 0);

    برائے (;;)
    {
	// When the end is not defined read everything there is.
	اگر (انتہا == تہی && vpeekc() == NUL)
	    توڑو;
	کرو
	    c = vgetc();
	جب (c == K_IGNORE || c == K_VER_SCROLLBAR || c == K_HOR_SCROLLBAR);

	اگر (c == NUL || got_int || (ex_معمول_busy > 0 && c == Ctrl_C))
	    // When CTRL-C was encountered the typeahead will be flushed and we
	    // won't get the end sequence.  Except when using ":normal".
	    توڑو;

	اگر (has_mbyte)
	    idx += (*mb_char2bytes)(c, buf + idx);
	ورنہ
	    buf[idx++] = c;
	buf[idx] = NUL;
	اگر (انتہا != تہی && STRNCMP(buf, انتہا, idx) == 0)
	{
	    اگر (انتہا[idx] == NUL)
		توڑو; // Found the end of paste code.
	    جاری;
	}
	اگر (!drop)
	{
	    انتخاب (منوال)
	    {
		حالت PASTE_CMDLINE:
		    put_on_cmdline(buf, idx, TRUE);
		    توڑو;

		حالت PASTE_EX:
		    // add one for the NUL that is going to be appended
		    اگر (gap != تہی && ga_grow(gap, idx + 1) == OK)
		    {
			mch_memmove((حرف *)gap->ga_ڈیٹا + gap->ga_len,
							     buf, (size_t)idx);
			gap->ga_len += idx;
		    }
		    توڑو;

		حالت PASTE_داخل_کرو:
		    اگر (stop_arrow() == OK)
		    {
			c = buf[0];
			اگر (idx == 1 && (c == CAR || c == K_KENTER || c == NL))
			    ins_eol(c);
			ورنہ
			{
			    ins_char_بائٹیں(buf, idx);
			    Append_مقصد_Redobuff_Lit(buf, idx);
			}
		    }
		    توڑو;

		حالت PASTE_ONE_CHAR:
		    اگر (ret_char == -1)
		    {
			اگر (has_mbyte)
			    ret_char = (*mb_ptr2char)(buf);
			ورنہ
			    ret_char = buf[0];
		    }
		    توڑو;
	    }
	}
	idx = 0;
    }

    --no_mapping;
    allow_کلیدیں = save_allow_کلیدیں;
    اگر (!save_paste)
	سیٹ_option_قدر_give_err((char_u *)"paste", FALSE, تہی, 0);

    واپس ret_char;
}

#اگر defined(FEAT_GUI_TABLINE)
    ساکن خالی
ins_tabline(صحیح c)
{
    // We will be leaving the current window, unless closing another tab.
    اگر (c != K_TABMENU || current_tabmenu != TABLINE_MENU_CLOSE
		|| (current_tab != 0 && current_tab != tabpage_اشاریہ(curtab)))
    {
	undisplay_dollar();
	آغاز_arrow(&curwin->w_cursor);
	can_cindent = TRUE;
    }

    اگر (c == K_TABLINE)
	goto_tabpage(current_tab);
    ورنہ
    {
	handle_tabmenu();
	redraw_statuslines();	// will redraw the tabline when needed
    }
}
#اگر_ختم

#اگر defined(FEAT_GUI)
    خالی
ins_scroll(خالی)
{
    pos_T	tpos;

    undisplay_dollar();
    tpos = curwin->w_cursor;
    اگر (gui_do_scroll())
    {
	آغاز_arrow(&tpos);
	can_cindent = TRUE;
    }
}

    خالی
ins_horscroll(خالی)
{
    pos_T	tpos;

    undisplay_dollar();
    tpos = curwin->w_cursor;
    اگر (do_mousescroll_horiz(scrollbar_قدر))
    {
	آغاز_arrow(&tpos);
	can_cindent = TRUE;
    }
}
#اگر_ختم

    ساکن خالی
ins_بایاں(خالی)
{
    pos_T	tpos;
    صحیح		انتہا_change = dont_sync_undo == FALSE; // end undoable change

#اگر_تعریف_شدہ FEAT_FOLDING
    اگر ((fdo_flags & FDO_HOR) && کلید_Typed)
	foldOpenCursor();
#اگر_ختم
    undisplay_dollar();
    tpos = curwin->w_cursor;
    اگر (oneleft() == OK)
    {
#اگر defined(FEAT_XIM) && defined(FEAT_GUI_GTK)
	// Only call start_arrow() when not busy with preediting, it will
	// break undo.  K_LEFT is inserted in im_correct_cursor().
	اگر (p_imst == IM_OVER_THE_SPOT || !im_is_preediting())
#اگر_ختم
	{
	    آغاز_arrow_with_change(&tpos, انتہا_change);
	    اگر (!انتہا_change)
		Append_Char_مقصد_Redobuff(K_بایاں);
	}
#اگر_تعریف_شدہ FEAT_RIGHTLEFT
	// If exit reversed string, position is fixed
	اگر (revins_scol != -1 && (صحیح)curwin->w_cursor.ستون >= revins_scol)
	    revins_legal++;
	revins_chars++;
#اگر_ختم
    }

    /*
     * if 'whichwrap' set for cursor in insert mode may go to
     * previous line
     */
    ورنہ اگر (vim_strchr(p_ww, '[') != تہی && curwin->w_cursor.lnum > 1)
    {
	// always break undo when moving upwards/downwards, else undo may break
	آغاز_arrow(&tpos);
	--(curwin->w_cursor.lnum);
	coladvance((colnr_T)MAXCOL);
	curwin->w_سیٹ_curswant = true;	// so we stay at the end
    }
    ورنہ
	vim_beep(BO_CRSR);
    dont_sync_undo = FALSE;
}

    ساکن خالی
ins_home(صحیح c)
{
    pos_T	tpos;

#اگر_تعریف_شدہ FEAT_FOLDING
    اگر ((fdo_flags & FDO_HOR) && کلید_Typed)
	foldOpenCursor();
#اگر_ختم
    undisplay_dollar();
    tpos = curwin->w_cursor;
    اگر (c == K_C_HOME)
	curwin->w_cursor.lnum = 1;
    curwin->w_cursor.ستون = 0;
    curwin->w_cursor.coladd = 0;
    curwin->w_curswant = 0;
    آغاز_arrow(&tpos);
}

    ساکن خالی
ins_انتہا(صحیح c)
{
    pos_T	tpos;

#اگر_تعریف_شدہ FEAT_FOLDING
    اگر ((fdo_flags & FDO_HOR) && کلید_Typed)
	foldOpenCursor();
#اگر_ختم
    undisplay_dollar();
    tpos = curwin->w_cursor;
    اگر (c == K_C_انتہا)
	curwin->w_cursor.lnum = curbuf->b_ml.ml_سطر_گنتی;
    coladvance((colnr_T)MAXCOL);
    curwin->w_curswant = MAXCOL;

    آغاز_arrow(&tpos);
}

    ساکن خالی
ins_s_بایاں(خالی)
{
    صحیح انتہا_change = dont_sync_undo == FALSE; // end undoable change
#اگر_تعریف_شدہ FEAT_FOLDING
    اگر ((fdo_flags & FDO_HOR) && کلید_Typed)
	foldOpenCursor();
#اگر_ختم
    undisplay_dollar();
    اگر (curwin->w_cursor.lnum > 1 || curwin->w_cursor.ستون > 0)
    {
	آغاز_arrow_with_change(&curwin->w_cursor, انتہا_change);
	اگر (!انتہا_change)
	    Append_Char_مقصد_Redobuff(K_S_بایاں);
	(خالی)bck_لفظ(1L, FALSE, FALSE);
	curwin->w_سیٹ_curswant = true;
    }
    ورنہ
	vim_beep(BO_CRSR);
    dont_sync_undo = FALSE;
}

    ساکن خالی
ins_دایاں(خالی)
{
    صحیح انتہا_change = dont_sync_undo == FALSE; // end undoable change

#اگر_تعریف_شدہ FEAT_FOLDING
    اگر ((fdo_flags & FDO_HOR) && کلید_Typed)
	foldOpenCursor();
#اگر_ختم
    undisplay_dollar();
    اگر (gchar_cursor() != NUL || virtual_فعال())
    {
	آغاز_arrow_with_change(&curwin->w_cursor, انتہا_change);
	اگر (!انتہا_change)
	    Append_Char_مقصد_Redobuff(K_دایاں);
	curwin->w_سیٹ_curswant = true;
	اگر (virtual_فعال())
	    oneright();
	ورنہ
	{
	    اگر (has_mbyte)
		curwin->w_cursor.ستون += (*mb_ptr2len)(ml_get_cursor());
	    ورنہ
		++curwin->w_cursor.ستون;
	}

#اگر_تعریف_شدہ FEAT_RIGHTLEFT
	revins_legal++;
	اگر (revins_chars)
	    revins_chars--;
#اگر_ختم
    }
    // if 'whichwrap' set for cursor in insert mode, may move the
    // cursor to the next line
    ورنہ اگر (vim_strchr(p_ww, ']') != تہی
	    && curwin->w_cursor.lnum < curbuf->b_ml.ml_سطر_گنتی)
    {
	آغاز_arrow(&curwin->w_cursor);
	curwin->w_سیٹ_curswant = true;
	++curwin->w_cursor.lnum;
	curwin->w_cursor.ستون = 0;
    }
    ورنہ
	vim_beep(BO_CRSR);
    dont_sync_undo = FALSE;
}

    ساکن خالی
ins_s_دایاں(خالی)
{
    صحیح انتہا_change = dont_sync_undo == FALSE; // end undoable change
#اگر_تعریف_شدہ FEAT_FOLDING
    اگر ((fdo_flags & FDO_HOR) && کلید_Typed)
	foldOpenCursor();
#اگر_ختم
    undisplay_dollar();
    اگر (curwin->w_cursor.lnum < curbuf->b_ml.ml_سطر_گنتی
	    || gchar_cursor() != NUL)
    {
	آغاز_arrow_with_change(&curwin->w_cursor, انتہا_change);
	اگر (!انتہا_change)
	    Append_Char_مقصد_Redobuff(K_S_دایاں);
	(خالی)fwd_لفظ(1L, FALSE, 0);
	curwin->w_سیٹ_curswant = true;
    }
    ورنہ
	vim_beep(BO_CRSR);
    dont_sync_undo = FALSE;
}

    ساکن خالی
ins_اوپر(
    صحیح		startcol)	// when TRUE move to Insstart.col
{
    pos_T	tpos;
    linenr_T	old_topline = curwin->w_topline;
#اگر_تعریف_شدہ FEAT_DIFF
    صحیح		old_topfill = curwin->w_topfill;
#اگر_ختم

    undisplay_dollar();
    tpos = curwin->w_cursor;
    اگر (cursor_اوپر(1L, TRUE) == OK)
    {
	اگر (startcol)
	    coladvance(getvcol_nolist(&Insstart));
	اگر (old_topline != curwin->w_topline
#اگر_تعریف_شدہ FEAT_DIFF
		|| old_topfill != curwin->w_topfill
#اگر_ختم
		)
	    redraw_later(UPD_VALID);
	آغاز_arrow(&tpos);
	can_cindent = TRUE;
    }
    ورنہ
	vim_beep(BO_CRSR);
}

    ساکن خالی
ins_pageup(خالی)
{
    pos_T	tpos;

    undisplay_dollar();

    اگر (mod_نقاب & MOD_نقاب_CTRL)
    {
	// <C-PageUp>: tab page back
	اگر (پہلا_tabpage->tp_اگلا != تہی)
	{
	    آغاز_arrow(&curwin->w_cursor);
	    goto_tabpage(-1);
	}
	واپس;
    }

    tpos = curwin->w_cursor;
    اگر (pagescroll(BACKWARD, 1L, FALSE) == OK)
    {
	آغاز_arrow(&tpos);
	can_cindent = TRUE;
    }
    ورنہ
	vim_beep(BO_CRSR);
}

    ساکن خالی
ins_نیچے_کو(
    صحیح		startcol)	// when TRUE move to Insstart.col
{
    pos_T	tpos;
    linenr_T	old_topline = curwin->w_topline;
#اگر_تعریف_شدہ FEAT_DIFF
    صحیح		old_topfill = curwin->w_topfill;
#اگر_ختم

    undisplay_dollar();
    tpos = curwin->w_cursor;
    اگر (cursor_نیچے_کو(1L, TRUE) == OK)
    {
	اگر (startcol)
	    coladvance(getvcol_nolist(&Insstart));
	اگر (old_topline != curwin->w_topline
#اگر_تعریف_شدہ FEAT_DIFF
		|| old_topfill != curwin->w_topfill
#اگر_ختم
		)
	    redraw_later(UPD_VALID);
	آغاز_arrow(&tpos);
	can_cindent = TRUE;
    }
    ورنہ
	vim_beep(BO_CRSR);
}

    ساکن خالی
ins_pagedown(خالی)
{
    pos_T	tpos;

    undisplay_dollar();

    اگر (mod_نقاب & MOD_نقاب_CTRL)
    {
	// <C-PageDown>: tab page forward
	اگر (پہلا_tabpage->tp_اگلا != تہی)
	{
	    آغاز_arrow(&curwin->w_cursor);
	    goto_tabpage(0);
	}
	واپس;
    }

    tpos = curwin->w_cursor;
    اگر (pagescroll(FORWARD, 1L, FALSE) == OK)
    {
	آغاز_arrow(&tpos);
	can_cindent = TRUE;
    }
    ورنہ
	vim_beep(BO_CRSR);
}

#اگر_تعریف_شدہ FEAT_DND
    ساکن خالی
ins_drop(خالی)
{
    do_put('~', تہی, BACKWARD, 1L, PUT_CURSEND);
}
#اگر_ختم

/*
 * Handle TAB in Insert or Replace mode.
 * Return TRUE when the TAB needs to be inserted like a normal character.
 */
    ساکن صحیح
ins_tab(خالی)
{
    صحیح		ind;
    صحیح		i;
    صحیح		عارضی;

    اگر (Insstart_blank_vcol == MAXCOL && curwin->w_cursor.lnum == Insstart.lnum)
	Insstart_blank_vcol = get_nolist_virtcol();
    اگر (echeck_abbr(TAB + ABBR_OFF))
	واپس FALSE;

    ind = inindent(0);
    اگر (ind)
	can_cindent = FALSE;

    /*
     * When nothing special, insert TAB like a normal character.
     */
    اگر (!curbuf->b_p_et
#اگر_تعریف_شدہ FEAT_VARTABS
	    && !(p_sta && ind
		// These five lines mean 'tabstop' != 'shiftwidth'
		&& ((tabstop_گنتی(curbuf->b_p_vts_array) > 1)
		    || (tabstop_گنتی(curbuf->b_p_vts_array) == 1
			&& tabstop_پہلا(curbuf->b_p_vts_array)
						       != get_sw_قدر(curbuf))
		    || (tabstop_گنتی(curbuf->b_p_vts_array) == 0
			&& curbuf->b_p_ts != get_sw_قدر(curbuf))))
	    && tabstop_گنتی(curbuf->b_p_vsts_array) == 0
#ورنہ
	    && !(p_sta && ind && curbuf->b_p_ts != get_sw_قدر(curbuf))
#اگر_ختم
	    && get_sts_قدر() == 0)
	واپس TRUE;

    اگر (stop_arrow() == FAIL)
	واپس TRUE;

    did_ai = FALSE;
    did_si = FALSE;
    can_si = FALSE;
    can_si_عقب = FALSE;
    Append_مقصد_Redobuff((char_u *)"\t");

#اگر_تعریف_شدہ FEAT_VARTABS
    اگر (p_sta && ind)		// insert tab in indent, use 'shiftwidth'
    {
	عارضی = (صحیح)get_sw_قدر(curbuf);
	عارضی -= get_nolist_virtcol() % عارضی;
    }
    ورنہ اگر (tabstop_گنتی(curbuf->b_p_vsts_array) > 0 || curbuf->b_p_sts != 0)
				// use 'softtabstop' when set
	عارضی = tabstop_padding(get_nolist_virtcol(), get_sts_قدر(),
						     curbuf->b_p_vsts_array);
    ورنہ			// otherwise use 'tabstop'
	عارضی = tabstop_padding(get_nolist_virtcol(), curbuf->b_p_ts,
						     curbuf->b_p_vts_array);
#ورنہ
    اگر (p_sta && ind)		// insert tab in indent, use 'shiftwidth'
	عارضی = (صحیح)get_sw_قدر(curbuf);
    ورنہ اگر (curbuf->b_p_sts != 0) // use 'softtabstop' when set
	عارضی = (صحیح)get_sts_قدر();
    ورنہ			// otherwise use 'tabstop'
	عارضی = (صحیح)curbuf->b_p_ts;
    عارضی -= get_nolist_virtcol() % عارضی;
#اگر_ختم

    /*
     * Insert the first space with ins_char().	It will delete one char in
     * replace mode.  Insert the rest with ins_str(); it will not delete any
     * chars.  For MODE_VREPLACE state, we use ins_char() for all characters.
     */
    ins_char(' ');
    جب (--عارضی > 0)
    {
	اگر (State & VREPLACE_FLAG)
	    ins_char(' ');
	ورنہ
	{
	    ins_str((char_u *)" ", 1);
	    اگر (State & REPLACE_FLAG)	    // no char replaced
		replace_ڈالو(NUL);
	}
    }

    /*
     * When 'expandtab' not set: Replace spaces by TABs where possible.
     */
#اگر_تعریف_شدہ FEAT_VARTABS
    اگر (!curbuf->b_p_et && (tabstop_گنتی(curbuf->b_p_vsts_array) > 0
			    || get_sts_قدر() > 0
			    || (p_sta && ind)))
#ورنہ
    اگر (!curbuf->b_p_et && (get_sts_قدر() || (p_sta && ind)))
#اگر_ختم
    {
	char_u		*ptr;
	char_u		*saved_سطر = تہی;	// init for GCC
	pos_T		pos;
	pos_T		fpos;
	pos_T		*cursor;
	colnr_T		want_vcol, vcol;
	صحیح		change_ستون = -1;
	صحیح		save_list = curwin->w_p_list;
	char_u		*tab = (char_u *)"\t";
	chartabsize_T	cts;

	/*
	 * Get the current line.  For MODE_VREPLACE state, don't make real
	 * changes yet, just work on a copy of the line.
	 */
	اگر (State & VREPLACE_FLAG)
	{
	    pos = curwin->w_cursor;
	    cursor = &pos;
	    saved_سطر = vim_strnsave(ml_get_curline(), ml_get_curline_len());
	    اگر (saved_سطر == تہی)
		واپس FALSE;
	    ptr = saved_سطر + pos.ستون;
	}
	ورنہ
	{
	    ptr = ml_get_cursor();
	    cursor = &curwin->w_cursor;
	}

	// When 'L' is not in 'cpoptions' a tab always takes up 'ts' spaces.
	اگر (vim_strchr(p_cpo, CPO_LISTWM) == تہی)
	    curwin->w_p_list = FALSE;

	// Find first white before the cursor
	fpos = curwin->w_cursor;
	جب (fpos.ستون > 0 && VIM_ISWHITE(ptr[-1]))
	{
	    --fpos.ستون;
	    --ptr;
	}

	// In Replace mode, don't change characters before the insert point.
	اگر ((State & REPLACE_FLAG)
		&& fpos.lnum == Insstart.lnum
		&& fpos.ستون < Insstart.ستون)
	{
	    ptr += Insstart.ستون - fpos.ستون;
	    fpos.ستون = Insstart.ستون;
	}

	// compute virtual column numbers of first white and cursor
	getvcol(curwin, &fpos, &vcol, تہی, تہی, 0);
	getvcol(curwin, cursor, &want_vcol, تہی, تہی, 0);

	init_chartabsize_arg(&cts, curwin, 0, vcol, tab, tab);

	// Use as many TABs as possible.  Beware of 'breakindent', 'showbreak'
	// and 'linebreak' adding extra virtual columns.
	جب (VIM_ISWHITE(*ptr))
	{
	    i = lbr_chartabsize(&cts);
	    اگر (cts.cts_vcol + i > want_vcol)
		توڑو;
	    اگر (*ptr != TAB)
	    {
		*ptr = TAB;
		اگر (change_ستون < 0)
		{
		    change_ستون = fpos.ستون;  // Column of first change
		    // May have to adjust Insstart
		    اگر (fpos.lnum == Insstart.lnum && fpos.ستون < Insstart.ستون)
			Insstart.ستون = fpos.ستون;
		}
	    }
	    ++fpos.ستون;
	    ++ptr;
	    cts.cts_vcol += i;
	}
	vcol = cts.cts_vcol;
	clear_chartabsize_arg(&cts);

	اگر (change_ستون >= 0)
	{
	    صحیح		    repl_off = 0;

	    // Skip over the spaces we need.
	    init_chartabsize_arg(&cts, curwin, 0, vcol, ptr, ptr);
	    جب (cts.cts_vcol < want_vcol && *cts.cts_ptr == ' ')
	    {
		cts.cts_vcol += lbr_chartabsize(&cts);
		++cts.cts_ptr;
		++repl_off;
	    }
	    ptr = cts.cts_ptr;
	    vcol = cts.cts_vcol;
	    clear_chartabsize_arg(&cts);

	    اگر (vcol > want_vcol)
	    {
		// Must have a char with 'showbreak' just before it.
		--ptr;
		--repl_off;
	    }
	    fpos.ستون += repl_off;

	    // Delete following spaces.
	    i = cursor->ستون - fpos.ستون;
	    اگر (i > 0)
	    {
#اگر_تعریف_شدہ FEAT_PROP_POPUP
		اگر (!(State & VREPLACE_FLAG))
		{
		    char_u  *newp;
		    صحیح	    ستون;

		    newp = alloc(curbuf->b_ml.ml_سطر_len - i);
		    اگر (newp == تہی)
			واپس FALSE;

		    ستون = ptr - curbuf->b_ml.ml_سطر_ptr;
		    اگر (ستون > 0)
			mch_memmove(newp, ptr - ستون, ستون);
		    mch_memmove(newp + ستون, ptr + i,
					   curbuf->b_ml.ml_سطر_len - ستون - i);

		    اگر (curbuf->b_ml.ml_flags & (ML_سطر_DIRTY | ML_ALLOCATED))
			vim_free(curbuf->b_ml.ml_سطر_ptr);
		    curbuf->b_ml.ml_سطر_ptr = newp;
		    curbuf->b_ml.ml_سطر_len -= i;
		    curbuf->b_ml.ml_سطر_textlen = 0;
		    curbuf->b_ml.ml_flags =
			   (curbuf->b_ml.ml_flags | ML_سطر_DIRTY) & ~ML_خالی_خانے;
		}
		ورنہ
#اگر_ختم
		    STRMOVE(ptr, ptr + i);
		// correct replace stack.
		اگر ((State & REPLACE_FLAG) && !(State & VREPLACE_FLAG))
		    برائے (عارضی = i; --عارضی >= 0; )
			replace_join(repl_off);
	    }
#اگر_تعریف_شدہ FEAT_NETBEANS_INTG
	    اگر (netbeans_فعال())
	    {
		netbeans_removed(curbuf, fpos.lnum, cursor->ستون, (طویل)(i + 1));
		netbeans_inserted(curbuf, fpos.lnum, cursor->ستون,
							   (char_u *)"\t", 1);
	    }
#اگر_ختم
	    cursor->ستون -= i;

	    /*
	     * In MODE_VREPLACE state, we haven't changed anything yet.  Do it
	     * now by backspacing over the changed spacing and then inserting
	     * the new spacing.
	     */
	    اگر (State & VREPLACE_FLAG)
	    {
		// Backspace from real cursor to change_col
		backspace_until_عمود(change_ستون);

		// Insert each char in saved_line from changed_col to
		// ptr-cursor
		ins_بائٹیں_len(saved_سطر + change_ستون,
						    cursor->ستون - change_ستون);
	    }
	}

	اگر (State & VREPLACE_FLAG)
	    vim_free(saved_سطر);
	curwin->w_p_list = save_list;
    }

    واپس FALSE;
}

/*
 * Handle CR or NL in insert mode.
 * Return FAIL when out of memory or can't undo.
 */
    صحیح
ins_eol(صحیح c)
{
    صحیح	    i;

    اگر (echeck_abbr(c + ABBR_OFF))
	واپس OK;
    اگر (stop_arrow() == FAIL)
	واپس FAIL;
    undisplay_dollar();

    /*
     * Strange Vi behaviour: In Replace mode, typing a NL will not delete the
     * character under the cursor.  Only push a NUL on the replace stack,
     * nothing to put back when the NL is deleted.
     */
    اگر ((State & REPLACE_FLAG) && !(State & VREPLACE_FLAG))
	replace_ڈالو(NUL);

    /*
     * In MODE_VREPLACE state, a NL replaces the rest of the line, and starts
     * replacing the next line, so we push all of the characters left on the
     * line onto the replace stack.  This is not done here though, it is done
     * in open_line().
     */

    // Put cursor on NUL if on the last char and coladd is 1 (happens after
    // CTRL-O).
    اگر (virtual_فعال() && curwin->w_cursor.coladd > 0)
	coladvance(getviscol());

#اگر_تعریف_شدہ FEAT_RIGHTLEFT
    // NL in reverse insert will always start in the end of
    // current line.
    اگر (revins_on)
	curwin->w_cursor.ستون += ml_get_cursor_len();
#اگر_ختم

    Append_مقصد_Redobuff(NL_STR);
    i = open_سطر(FORWARD,
	    has_format_option(FO_RET_COMS) ? OPENLINE_DO_COM : 0, old_indent,
	    تہی);
    old_indent = 0;
    can_cindent = TRUE;
#اگر_تعریف_شدہ FEAT_FOLDING
    // When inserting a line the cursor line must never be in a closed fold.
    foldOpenCursor();
#اگر_ختم

    واپس i;
}

#اگر_تعریف_شدہ FEAT_DIGRAPHS
/*
 * Handle digraph in insert mode.
 * Returns character still to be inserted, or NUL when nothing remaining to be
 * done.
 */
    ساکن صحیح
ins_digraph(خالی)
{
    صحیح	    c;
    صحیح	    cc;
    صحیح	    did_putchar = FALSE;

    pc_حالت = PC_حالت_UNSET;
    اگر (redrawing() && !char_avail())
    {
	// may need to redraw when no more chars available now
	ins_redraw(FALSE);

	edit_putchar('?', TRUE);
	did_putchar = TRUE;
	جوڑو_مقصد_showcmd_c(Ctrl_K);
    }

# ifdef USE_ON_FLY_SCROLL
    dont_scroll = TRUE;		// disallow scrolling here
# endif

    // don't map the digraph chars. This also prevents the
    // mode message to be deleted when ESC is hit
    ++no_mapping;
    ++allow_کلیدیں;
    c = plain_vgetc();
    --no_mapping;
    --allow_کلیدیں;
    اگر (did_putchar)
	// when the line fits in 'columns' the '?' is at the start of the next
	// line and will not be removed by the redraw
	edit_unputchar();

    اگر (IS_SPECIAL(c) || mod_نقاب)	    // special key
    {
	clear_showcmd();
	داخل_کرو_special(c, TRUE, FALSE);
	واپس NUL;
    }
    اگر (c != ESC)
    {
	did_putchar = FALSE;
	اگر (redrawing() && !char_avail())
	{
	    // may need to redraw when no more chars available now
	    ins_redraw(FALSE);

	    اگر (char2cells(c) == 1)
	    {
		ins_redraw(FALSE);
		edit_putchar(c, TRUE);
		did_putchar = TRUE;
	    }
	    جوڑو_مقصد_showcmd_c(c);
	}
	++no_mapping;
	++allow_کلیدیں;
	cc = plain_vgetc();
	--no_mapping;
	--allow_کلیدیں;
	اگر (did_putchar)
	    // when the line fits in 'columns' the '?' is at the start of the
	    // next line and will not be removed by a redraw
	    edit_unputchar();
	اگر (cc != ESC)
	{
	    Append_مقصد_Redobuff((char_u *)CTRL_V_STR);
	    c = digraph_get(c, cc, TRUE);
	    clear_showcmd();
	    واپس c;
	}
    }
    clear_showcmd();
    واپس NUL;
}
#اگر_ختم

/*
 * Handle CTRL-E and CTRL-Y in Insert mode: copy char from other line.
 * Returns the char to be inserted, or NUL if none found.
 */
    صحیح
ins_copychar(linenr_T lnum)
{
    صحیح		    c;
    char_u	    *ptr, *پچھلا_ptr;
    char_u	    *سطر;
    chartabsize_T   cts;

    اگر (lnum < 1 || lnum > curbuf->b_ml.ml_سطر_گنتی)
    {
	vim_beep(BO_COPY);
	واپس NUL;
    }

    // try to advance to the cursor column
    validate_virtcol();
    سطر = ml_get(lnum);
    پچھلا_ptr = سطر;
    init_chartabsize_arg(&cts, curwin, lnum, 0, سطر, سطر);
    جب (cts.cts_vcol < curwin->w_virtcol && *cts.cts_ptr != NUL)
    {
	پچھلا_ptr = cts.cts_ptr;
	cts.cts_vcol += lbr_chartabsize_adv(&cts);
    }
    اگر (cts.cts_vcol > curwin->w_virtcol)
	ptr = پچھلا_ptr;
    ورنہ
	ptr = cts.cts_ptr;
    clear_chartabsize_arg(&cts);

    c = (*mb_ptr2char)(ptr);
    اگر (c == NUL)
	vim_beep(BO_COPY);
    واپس c;
}

/*
 * CTRL-Y or CTRL-E typed in Insert mode.
 */
    ساکن صحیح
ins_ctrl_ey(صحیح tc)
{
    صحیح	    c = tc;

    اگر (ctrl_x_منوال_scroll())
    {
	اگر (c == Ctrl_Y)
	    scrolldown_clamp();
	ورنہ
	    scrollup_clamp();
	redraw_later(UPD_VALID);
    }
    ورنہ
    {
	c = ins_copychar(curwin->w_cursor.lnum + (c == Ctrl_Y ? -1 : 1));
	اگر (c != NUL)
	{
	    طویل	tw_save;

	    // The character must be taken literally, insert like it
	    // was typed after a CTRL-V, and pretend 'textwidth'
	    // wasn't set.  Digits, 'o' and 'x' are special after a
	    // CTRL-V, don't use it for these.
	    اگر (c < 256 && !محفوظ_isalnum(c))
		Append_مقصد_Redobuff((char_u *)CTRL_V_STR);	// CTRL-V
	    tw_save = curbuf->b_p_tw;
	    curbuf->b_p_tw = -1;
	    داخل_کرو_special(c, TRUE, FALSE);
	    curbuf->b_p_tw = tw_save;
#اگر_تعریف_شدہ FEAT_RIGHTLEFT
	    revins_chars++;
	    revins_legal++;
#اگر_ختم
	    c = Ctrl_V;	// pretend CTRL-V is last character
	    auto_format(FALSE, TRUE);
	}
    }
    واپس c;
}

/*
 * Get the value that w_virtcol would have when 'list' is off.
 * Unless 'cpo' contains the 'L' flag.
 */
    colnr_T
get_nolist_virtcol(خالی)
{
    // check validity of cursor in current buffer
    اگر (curwin->w_بفر == تہی
	|| curwin->w_بفر->b_ml.ml_mfp == تہی
	|| curwin->w_cursor.lnum > curwin->w_بفر->b_ml.ml_سطر_گنتی)
	واپس 0;
    اگر (curwin->w_p_list && vim_strchr(p_cpo, CPO_LISTWM) == تہی)
	واپس getvcol_nolist(&curwin->w_cursor);
    validate_virtcol();
    واپس curwin->w_virtcol;
}

#اگر defined(FEAT_EVAL)
/*
 * Handle the InsertCharPre autocommand.
 * "c" is the character that was typed.
 * Return a pointer to allocated memory with the replacement string.
 * Return NULL to continue inserting "c".
 */
    ساکن char_u *
do_داخل_کرو_char_pre(صحیح c)
{
    char_u	*res;
    char_u	buf[MB_MAXBYTES + 1];
    size_t	buflen;
    صحیح		save_State = State;

    // Return quickly when there is nothing to do.
    اگر (!has_insertcharpre())
	واپس تہی;

    اگر (c == Ctrl_RSB)
	واپس تہی;

    اگر (has_mbyte)
	buflen = (*mb_char2bytes)(c, buf);
    ورنہ
    {
	buf[0] = c;
	buflen = 1;
    }
    buf[buflen] = NUL;

    // Lock the text to avoid weird things from happening.
    ++textlock;
    سیٹ_vim_var_string(VV_CHAR, buf, (صحیح)buflen);  // set v:char

    res = تہی;
    اگر (ins_apply_autocmds(EVENT_INSERTCHARPRE))
    {
	// Get the value of v:char.  It may be empty or more than one
	// character.  Only use it when changed, otherwise continue with the
	// original character to avoid breaking autoindent.
	اگر (STRCMP(buf, get_vim_var_str(VV_CHAR)) != 0)
	    res = vim_strsave(get_vim_var_str(VV_CHAR));
    }

    سیٹ_vim_var_string(VV_CHAR, تہی, -1);  // clear v:char
    --textlock;

    // Restore the State, it may have been changed.
    State = save_State;

    واپس res;
}
#اگر_ختم

    صحیح
get_can_cindent(خالی)
{
    واپس can_cindent;
}

    خالی
سیٹ_can_cindent(صحیح val)
{
    can_cindent = val;
}

/*
 * Trigger "event" and take care of fixing undo.
 */
    صحیح
ins_apply_autocmds(event_T event)
{
    varnumber_T	tick = CHANGEDTICK(curbuf);
    صحیح r;

    r = apply_autocmds(event, تہی, تہی, FALSE, curbuf);

    // If u_savesub() was called then we are not prepared to start
    // a new line.  Call u_save() with no contents to fix that.
    // Except when leaving Insert mode.
    اگر (event != EVENT_INSERTLEAVE && tick != CHANGEDTICK(curbuf))
	u_save(curwin->w_cursor.lnum, (linenr_T)(curwin->w_cursor.lnum + 1));

    واپس r;
}
