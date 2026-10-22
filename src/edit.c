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

#ضمن "vim.h"

#تعریف_کرو BACKSPACE_CHAR		    1
#تعریف_کرو BACKSPACE_لفظ		    2
#تعریف_کرو BACKSPACE_لفظ_NOT_SPACE    3
#تعریف_کرو BACKSPACE_سطر		    4

// Set when doing something for completion that may call edit() recursively,
// which is not allowed.
ساكن صحيح	compl_busy = FALSE;


ساكن فراغ ins_ctrl_v(فراغ);
ساكن فراغ داخل_کرو_special(صحيح, صحيح, صحيح);
ساكن فراغ redo_literal(صحيح c);
ساكن فراغ شروع_arrow_common(pos_T *سرا_داخل_کرو_pos, صحيح change);
#جے_تعریف_شدہ FEAT_SPELL
ساكن فراغ check_spell_redraw(فراغ);
#جے_انت
ساكن فراغ stop_داخل_کرو(pos_T *سرا_داخل_کرو_pos, صحيح esc, صحيح nomove);
ساكن صحيح  echeck_abbr(صحيح);
ساكن فراغ mb_replace_نکالو_ins(صحيح cc);
ساكن فراغ replace_flush(فراغ);
ساكن فراغ replace_do_bs(صحيح حد_بندی_خانہ);
ساكن صحيح del_char_after_خانہ(صحيح حد_بندی_خانہ);
ساكن فراغ ins_reg(فراغ);
ساكن فراغ ins_ctrl_g(فراغ);
ساكن فراغ ins_ctrl_hat(فراغ);
ساكن صحيح  ins_esc(لما *گنتی, صحيح cmdchar, صحيح nomove);
#جے_تعریف_شدہ FEAT_RIGHTLEFT
ساكن فراغ ins_ctrl_(فراغ);
#جے_انت
ساكن صحيح ins_شروع_select(صحيح c);
ساكن فراغ ins_داخل_کرو(صحيح replaceState);
ساكن فراغ ins_ctrl_o(فراغ);
ساكن فراغ ins_کھسکاؤ(صحيح c, صحيح lastc);
ساكن فراغ ins_del(فراغ);
ساكن صحيح  ins_bs(صحيح c, صحيح منوال, صحيح *inserted_space_p);
#إذا defined(FEAT_GUI_TABLINE)
ساكن فراغ ins_tabline(صحيح c);
#جے_انت
ساكن فراغ ins_بایاں(فراغ);
ساكن فراغ ins_home(صحيح c);
ساكن فراغ ins_سرا(صحيح c);
ساكن فراغ ins_s_بایاں(فراغ);
ساكن فراغ ins_دایاں(فراغ);
ساكن فراغ ins_s_دایاں(فراغ);
ساكن فراغ ins_اتے(صحيح startcol);
ساكن فراغ ins_pageup(فراغ);
ساكن فراغ ins_تھلے(صحيح startcol);
ساكن فراغ ins_pagedown(فراغ);
#جے_تعریف_شدہ FEAT_DND
ساكن فراغ ins_drop(فراغ);
#جے_انت
ساكن صحيح  ins_tab(فراغ);
#جے_تعریف_شدہ FEAT_DIGRAPHS
ساكن صحيح  ins_digraph(فراغ);
#جے_انت
ساكن صحيح  ins_ctrl_ey(صحيح tc);
#إذا defined(FEAT_EVAL)
ساكن char_u *do_داخل_کرو_char_pre(صحيح c);
#جے_انت

ساكن colnr_T	Insstart_textlen;	// length of line when insert started
ساكن colnr_T	Insstart_blank_vcol;	// vcol for first inserted blank
ساكن صحيح	update_Insstart_orig = TRUE; // set Insstart_orig to Insstart

ساكن string_T	آخری_داخل_کرو = {عدم, 0};    // the text of the previous insert,
					    // K_SPECIAL and CSI are escaped
ساكن صحيح	آخری_داخل_کرو_skip; // nr of chars in front of previous insert
ساكن صحيح	new_داخل_کرو_skip;  // nr of chars in front of current insert
ساكن صحيح	did_restart_edit;	// "restart_edit" when calling edit()

ساكن صحيح	can_cindent;		// may do cindenting on this line

#جے_تعریف_شدہ FEAT_RIGHTLEFT
ساكن صحيح	revins_on;		// reverse insert mode on
ساكن صحيح	revins_chars;		// how much to skip after edit
ساكن صحيح	revins_legal;		// was the last char 'legal'?
ساكن صحيح	revins_scol;		// start column of revins session
#جے_انت

ساكن صحيح	ins_need_undo;		// call u_save() before inserting a
					// char.  Set when edit() is called.
					// after that arrow_used is used.

ساكن صحيح	dont_sync_undo = FALSE;	// CTRL-G U prevents syncing undo for
					// the next left/right cursor key

// With 'autocompletedelay' set, arm the delay and let the main loop fire
// Insert-mode autocommands; the popup is shown later on K_COMPLETE_DELAY.
// Otherwise trigger completion right away.
#تعریف_کرو TRIGGER_AUTOCOMPLETE()			\
    افعل {					\
	update_screen(UPD_VALID);  /* Show char (deletion) immediately */ \
	باہرلا_flush();				\
	ins_compl_enable_autocomplete();	\
	ins_compl_arm_autostart();		\
	إذا (!ins_compl_arm_autocomplete_delay())\
	    اذهب docomplete;			\
    } طالما (0)

#تعریف_کرو MAY_TRIGGER_AUTOCOMPLETE(c)				\
    افعل {							\
	إذا (ins_compl_has_autocomplete() && !char_avail()	\
		&& curwin->w_cursor.خانہ > 0)			\
	{							\
	    (c) = char_before_cursor();				\
	    إذا (vim_isprintc(c))				\
		TRIGGER_AUTOCOMPLETE();				\
	}							\
    } طالما (0)

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
    صحيح
edit(
    صحيح		cmdchar,
    صحيح		startln,	// if set, insert at start of line
    لما	گنتی)
{
    صحيح		c = 0;
    char_u	*ptr;
    صحيح		lastc = 0;
    صحيح		mincol;
    ساكن linenr_T o_lnum = 0;
    صحيح		i;
    صحيح		did_backspace = TRUE;	    // previous char was backspace
    صحيح		سطر_is_white = FALSE;	    // line is empty before insert
    linenr_T	old_topline = 0;	    // topline before insertion
#جے_تعریف_شدہ FEAT_DIFF
    صحيح		old_topfill = -1;
#جے_انت
    صحيح		inserted_space = FALSE;     // just inserted a space
    صحيح		replaceState = منوال_REPLACE;
    صحيح		nomove = FALSE;		    // don't move cursor on return
#جے_تعریف_شدہ FEAT_کم_CHANNEL
    صحيح		cmdchar_todo = cmdchar;
#جے_انت
#جے_تعریف_شدہ FEAT_CONCEAL
    صحيح		cursor_سطر_was_concealed;
#جے_انت
    صحيح		ins_just_started = TRUE;

    // Remember whether editing was restarted after CTRL-O.
    did_restart_edit = restart_edit;

    // sleep before redrawing, needed for "CTRL-O :" that results in an
    // error message
    check_for_delay(TRUE);

    // set Insstart_orig to Insstart
    update_Insstart_orig = TRUE;

#جے_تعریف_شدہ HAVE_SANDBOX
    // Don't allow inserting in the sandbox.
    إذا (sandbox != 0)
    {
	emsg(_(e_not_allowed_وچ_sandbox));
	أرجع FALSE;
    }
#جے_انت
    // Don't allow changes in the buffer while editing the cmdline.  The
    // caller of getcmdline() may get confused.
    // Don't allow recursive insert mode when busy with completion.
    إذا (textlock != 0 || ins_compl_سرگرم() || compl_busy || pum_visible())
    {
	emsg(_(e_not_allowed_منزل_change_متن_or_change_کھڑکی));
	أرجع FALSE;
    }
    ins_compl_clear();	    // clear stuff for CTRL-X mode

    /*
     * Trigger InsertEnter autocommands.  Do not do this for "r<CR>" or "grx".
     */
    إذا (cmdchar != 'r' && cmdchar != 'v')
    {
	pos_T   save_cursor = curwin->w_cursor;

#جے_تعریف_شدہ FEAT_EVAL
	إذا (cmdchar == 'R')
	    ptr = (char_u *)"r";
	وإلا إذا (cmdchar == 'V')
	    ptr = (char_u *)"v";
	وإلا
	    ptr = (char_u *)"i";
	سیٹ_vim_var_string(VV_INSERTMODE, ptr, 1);
	سیٹ_vim_var_string(VV_CHAR, عدم, -1);  // clear v:char
#جے_انت
	ins_apply_autocmds(EVENT_INSERTENTER);

	// Check for changed highlighting, e.g. for ModeMsg.
	إذا (need_highlight_بدلیا)
	    highlight_بدلیا();

	// Make sure the cursor didn't move.  Do call check_cursor_col() in
	// case the text was modified.  Since Insert mode was not started yet
	// a call to check_cursor_col() may move the cursor, especially with
	// the "A" command, thus set State to avoid that. Also check that the
	// line number is still valid (lines may have been deleted).
	// Do not restore if v:char was set to a non-empty string.
	إذا (!EQUAL_POS(curwin->w_cursor, save_cursor)
#جے_تعریف_شدہ FEAT_EVAL
		&& *get_vim_var_str(VV_CHAR) == NUL
#جے_انت
		&& save_cursor.lnum <= curbuf->b_ml.ml_سطر_گنتی)
	{
	    صحيح save_state = State;

	    curwin->w_cursor = save_cursor;
	    State = منوال_داخل_کرو;
	    check_cursor_خانہ();
	    State = save_state;
	}
    }

#جے_تعریف_شدہ FEAT_CONCEAL
    // Check if the cursor line was concealed before changing State.
    cursor_سطر_was_concealed = curwin->w_p_cole > 0
						&& conceal_cursor_سطر(curwin);
#جے_انت

    /*
     * When doing a paste with the middle mouse button, Insstart is set to
     * where the paste started.
     */
    إذا (where_paste_started.lnum != 0)
	Insstart = where_paste_started;
    وإلا
    {
	Insstart = curwin->w_cursor;
	إذا (startln)
	    Insstart.خانہ = 0;
    }
    Insstart_textlen = (colnr_T)linetabsize_str(ml_get_curline());
    Insstart_blank_vcol = MAXCOL;
    إذا (!did_ai)
	ai_خانہ = 0;

    إذا (cmdchar != NUL && restart_edit == 0)
    {
	ResetRedobuff();
	Append_نمبر_منزل_Redobuff(گنتی);
	إذا (cmdchar == 'V' || cmdchar == 'v')
	{
	    // "gR" or "gr" command
	    Append_Char_منزل_Redobuff('g');
	    Append_Char_منزل_Redobuff((cmdchar == 'v') ? 'r' : 'R');
	}
	وإلا
	{
	    إذا (cmdchar == K_PS)
		Append_Char_منزل_Redobuff('a');
	    وإلا
		Append_Char_منزل_Redobuff(cmdchar);
	    إذا (cmdchar == 'g')		    // "gI" command
		Append_Char_منزل_Redobuff('I');
	    وإلا إذا (cmdchar == 'r')	    // "r<CR>" command
		گنتی = 1;		    // insert only one <CR>
	}
    }

    إذا (cmdchar == 'R')
    {
	State = منوال_REPLACE;
    }
    وإلا إذا (cmdchar == 'V' || cmdchar == 'v')
    {
	State = منوال_VREPLACE;
	replaceState = منوال_VREPLACE;
	orig_سطر_گنتی = curbuf->b_ml.ml_سطر_گنتی;
	vr_سطریں_بدلیا = 1;
    }
    وإلا
	State = منوال_داخل_کرو;

    may_trigger_modechanged();
    stop_داخل_کرو_منوال = FALSE;

#جے_تعریف_شدہ FEAT_CONCEAL
    // Check if the cursor line needs redrawing after changing State.  If
    // 'concealcursor' is "n" it needs to be redrawn without concealing.
    conceal_check_cursor_سطر(cursor_سطر_was_concealed);
#جے_انت

    // Need to position cursor again when on a TAB and when on a char with
    // virtual text.
    إذا (gchar_cursor() == TAB
#جے_تعریف_شدہ FEAT_PROP_POPUP
	    || curbuf->b_has_textprop
#جے_انت
       )
	curwin->w_valid &= ~(VALID_WROW|VALID_WCOL|VALID_VIRTCOL);

    /*
     * Enable langmap or IME, indicated by 'iminsert'.
     * Note that IME may enabled/disabled without us noticing here, thus the
     * 'iminsert' value may not reflect what is actually used.  It is updated
     * when hitting <Esc>.
     */
    إذا (curbuf->b_p_iminsert == B_IMODE_LMAP)
	State |= منوال_LANGMAP;
#جے_تعریف_شدہ HAVE_داخلہ_طریقہ
    im_سیٹ_سرگرم(curbuf->b_p_iminsert == B_IMODE_IM);
#جے_انت

    setmouse();
    clear_showcmd();
#جے_تعریف_شدہ FEAT_RIGHTLEFT
    // there is no reverse replace mode
    revins_on = (State == منوال_داخل_کرو && p_ri);
    إذا (revins_on)
	undisplay_dollar();
    revins_chars = 0;
    revins_legal = 0;
    revins_scol = -1;
#جے_انت
    إذا (!p_ek)
    {
	MAY_WANT_منزل_LOG_THIS;

	// Disable bracketed paste mode, we won't recognize the escape
	// sequences.
	باہرلا_str(T_BD);

	// Disable modifyOtherKeys, keys with modifiers would cause exiting
	// Insert mode.
	باہرلا_str_t_TE();
    }

    /*
     * Handle restarting Insert mode.
     * Don't do this for "CTRL-O ." (repeat an insert): In that case we get
     * here with something in the stuff buffer.
     */
    إذا (restart_edit != 0 && stuff_خالی_تھاواں())
    {
	/*
	 * After a paste we consider text typed to be part of the insert for
	 * the pasted text. You can backspace over the pasted text too.
	 */
	إذا (where_paste_started.lnum)
	    arrow_ورتیا = FALSE;
	وإلا
	    arrow_ورتیا = TRUE;
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
	إذا (((ins_at_eol && curwin->w_cursor.lnum == o_lnum)
		    || curwin->w_curswant > curwin->w_virtcol)
		&& *(ptr = ml_get_curline() + curwin->w_cursor.خانہ) != NUL)
	{
	    إذا (ptr[1] == NUL)
		++curwin->w_cursor.خانہ;
	    وإلا إذا (has_mbyte)
	    {
		i = (*mb_ptr2len)(ptr);
		إذا (ptr[i] == NUL)
		    curwin->w_cursor.خانہ += i;
	    }
	}
	ins_at_eol = FALSE;
    }
    وإلا
	arrow_ورتیا = FALSE;

    // we are in insert mode now, don't need to start it anymore
    need_شروع_insertmode = FALSE;

    // Need to save the line for undo before inserting the first char.
    ins_need_undo = TRUE;

    where_paste_started.lnum = 0;
    can_cindent = TRUE;
#جے_تعریف_شدہ FEAT_FOLDING
    // The cursor line is not in a closed fold, unless 'insertmode' is set or
    // restarting.
    إذا (!p_im && did_restart_edit == 0)
	foldOpenCursor();
#جے_انت

    /*
     * If 'showmode' is set, show the current (insert/replace/..) mode.
     * A warning message for changing a readonly file is given here, before
     * actually changing anything.  It's put after the mode, if any.
     */
    i = 0;
    إذا (p_smd && پیغام_silent == 0)
	i = showmode();

    إذا (!p_im && did_restart_edit == 0)
	change_warning(i == 0 ? 0 : i + 1);

#جے_تعریف_شدہ CURSOR_SHAPE
    ui_cursor_shape();		// may show different cursor shape
#جے_انت
#جے_تعریف_شدہ FEAT_DIGRAPHS
    do_digraph(-1);		// clear digraphs
#جے_انت

    /*
     * Get the current length of the redo buffer, those characters have to be
     * skipped if we want to get to the inserted characters.
     */
    string_T inserted = get_inserted();
    new_داخل_کرو_skip = (صحيح)inserted.لمائی;
    إذا (inserted.string != عدم)
	vim_free(inserted.string);

    old_indent = 0;

    /*
     * Main loop in Insert mode: repeat until Insert mode is left.
     */
    لكل (;;)
    {
#جے_تعریف_شدہ FEAT_RIGHTLEFT
	إذا (!revins_legal)
	    revins_scol = -1;	    // reset on illegal motions
	وإلا
	    revins_legal = 0;
#جے_انت
	إذا (arrow_ورتیا)	    // don't repeat insert when arrow key used
	    گنتی = 0;

	إذا (update_Insstart_orig)
	    Insstart_orig = Insstart;

	إذا (stop_داخل_کرو_منوال && !ins_compl_سرگرم())
	{
	    // ":stopinsert" used or 'insertmode' reset
	    گنتی = 0;
	    اذهب doESCkey;
	}

	// set curwin->w_curswant for next K_DOWN or K_UP
	إذا (!arrow_ورتیا)
	    curwin->w_سیٹ_curswant = true;

	// If there is no typeahead may check for timestamps (e.g., for when a
	// menu invoked a shell command).
	إذا (stuff_خالی_تھاواں())
	{
	    did_check_timestamps = FALSE;
	    إذا (need_check_timestamps)
		check_timestamps(FALSE);
	}

	/*
	 * When emsg() was called msg_scroll will have been set.
	 */
	پیغام_scroll = FALSE;

#جے_تعریف_شدہ FEAT_GUI
	// When 'mousefocus' is set a mouse movement may have taken us to
	// another window.  "need_mouse_correct" may then be set because of an
	// autocommand.
	إذا (need_mouse_correct)
	    gui_mouse_correct();
#جے_انت

#جے_تعریف_شدہ FEAT_FOLDING
	// Open fold at the cursor line, according to 'foldopen'.
	إذا (fdo_flags & FDO_داخل_کرو)
	    foldOpenCursor();
	// Close folds where the cursor isn't, according to 'foldclose'
	إذا (!char_avail())
	    foldCheckClose();
#جے_انت

#جے_تعریف_شدہ FEAT_کم_CHANNEL
	إذا (bt_prompt(curbuf))
	{
	    init_prompt(cmdchar_todo);
	    cmdchar_todo = NUL;
	}
#جے_انت

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
	إذا (curbuf->b_mod_سیٹ
		&& curwin->w_p_wrap
		&& !curwin->w_p_sms
		&& !did_backspace
		&& curwin->w_topline == old_topline
#جے_تعریف_شدہ FEAT_DIFF
		&& curwin->w_topfill == old_topfill
#جے_انت
		&& گنتی <= 1
		)
	{
	    mincol = curwin->w_wcol;
	    validate_cursor_خانہ();

	    إذا (
#جے_تعریف_شدہ FEAT_VARTABS
		curwin->w_wcol < mincol - tabstop_at(get_nolist_virtcol(),
						     curbuf->b_p_ts,
						     curbuf->b_p_vts_array,
						     FALSE)
#وإلا
		(صحيح)curwin->w_wcol < mincol - curbuf->b_p_ts
#جے_انت
		    && curwin->w_wrow ==
				   curwin->w_اچائی - 1 - get_scrolloff_قدر()
		    && (curwin->w_cursor.lnum != curwin->w_topline
#جے_تعریف_شدہ FEAT_DIFF
			|| curwin->w_topfill > 0
#جے_انت
		    ))
	    {
#جے_تعریف_شدہ FEAT_DIFF
		إذا (curwin->w_topfill > 0)
		    --curwin->w_topfill;
		وإلا
#جے_انت
#جے_تعریف_شدہ FEAT_FOLDING
		إذا (hasFolding(curwin->w_topline, عدم, &old_topline))
		    سیٹ_topline(curwin, old_topline + 1);
		وإلا
#جے_انت
		    سیٹ_topline(curwin, curwin->w_topline + 1);
	    }
	}

	// May need to adjust w_topline to show the cursor.
	إذا (گنتی <= 1)
	    update_topline();

	did_backspace = FALSE;

	إذا (گنتی <= 1)
	    validate_cursor();		// may set must_redraw

	/*
	 * Redraw the display when no characters are waiting.
	 * Also shows mode, ruler and positions cursor.
	 */
	ins_redraw(TRUE);

	إذا (curwin->w_p_scb)
	    do_check_scrollbind(TRUE);

	إذا (curwin->w_p_crb)
	    do_check_cursorbind();
	إذا (گنتی <= 1)
	    update_curswant();
	old_topline = curwin->w_topline;
#جے_تعریف_شدہ FEAT_DIFF
	old_topfill = curwin->w_topfill;
#جے_انت

#جے_تعریف_شدہ USE_ON_FLY_SCROLL
	dont_scroll = FALSE;		// allow scrolling here
#جے_انت
	// May request the keyboard protocol state now.
	may_send_t_RK();

	/*
	 * Get a character for Insert mode.  Ignore K_IGNORE and K_NOP.
	 */
	إذا (c != K_CURSORHOLD && c != K_COMPLETE_DELAY)
	    lastc = c;		// remember the previous char for CTRL-D

	// After using CTRL-G U the next cursor key will not break undo.
	إذا (dont_sync_undo == MAYBE)
	    dont_sync_undo = TRUE;
	وإلا
	    dont_sync_undo = FALSE;
	إذا (cmdchar == K_PS)
	    // Got here from normal mode when bracketed paste started.
	    c = K_PS;
	وإلا
	{
	    // Trigger autocomplete when entering Insert mode, either directly
	    // or via change commands like 'ciw', 'cw', etc., before the first
	    // character is typed.
	    إذا (ins_just_started)
	    {
		ins_just_started = FALSE;
		إذا (ins_compl_has_autocomplete() && !char_avail()
			&& curwin->w_cursor.خانہ > 0)
		{
		    c = char_before_cursor();
		    إذا (vim_isprintc(c))
		    {
			ins_compl_enable_autocomplete();
			ins_compl_arm_autostart();
			ins_compl_init_get_longest();
#جے_تعریف_شدہ FEAT_RIGHTLEFT
			إذا (p_hkmap)
			    c = hkmap(c);		// Hebrew mode mapping
#جے_انت
			// Defer until the delay expires (K_COMPLETE_DELAY), or
			// trigger now when no delay is in effect.
			إذا (!ins_compl_arm_autocomplete_delay())
			    اذهب docomplete;
		    }
		}
	    }

	    افعل
	    {
		c = محفوظ_vgetc();

		إذا (stop_داخل_کرو_منوال
#جے_تعریف_شدہ FEAT_TERMINAL
			|| (c == K_IGNORE && رکن_use_loop())
#جے_انت
		   )
		{
		    // Insert mode ended, possibly from a callback, or a timer
		    // must have opened a terminal window.
		    إذا (c != K_IGNORE && c != K_NOP)
			vungetc(c);
		    گنتی = 0;

		    إذا (!bt_prompt(curwin->w_بفر)
#جے_تعریف_شدہ FEAT_TERMINAL
			    && !bt_terminal(curwin->w_بفر)
#جے_انت
			    && stop_داخل_کرو_منوال)
			// :stopinsert command via callback or via server command
			nomove = FALSE;
		    وإلا
			nomove = TRUE;
		    ins_compl_prep(ESC);
		    اذهب doESCkey;
		}
	    } طالما (c == K_IGNORE || c == K_NOP);
	}

	// Don't want K_CURSORHOLD for the second key, e.g., after CTRL-V.
	did_cursorhold = TRUE;
	إذا (c != K_CURSORHOLD && c != K_COMPLETE_DELAY)
	{
	    // Don't want delayed autocompletion from the previous key either.
	    ins_compl_clear_autocomplete_delay();
	    ins_compl_disarm_autostart();
	    // A completion already on screen goes on being what it was.
	    إذا (!ins_compl_سرگرم())
		ins_compl_disable_autocomplete();
	}

#جے_تعریف_شدہ FEAT_RIGHTLEFT
	إذا (p_hkmap && کلید_Typed)
	    c = hkmap(c);		// Hebrew mode mapping
#جے_انت

	// If the window was made so small that nothing shows, make it at least
	// one line and one column when typing.
	إذا (کلید_Typed && !کلید_Stuffed)
	    win_ensure_ناپ();

	/*
	 * Special handling of keys while the popup menu is visible or wanted
	 * and the cursor is still in the completed word.  Only when there is
	 * a match, skip this when no matches were found.
	 */
	إذا (ins_compl_سرگرم() && curwin->w_cursor.خانہ >= ins_compl_خانہ()
		&& ins_compl_has_shown_میل() && pum_wanted())
	{
	    // BS: Delete one character from "compl_leader".
	    إذا ((c == K_BS || c == Ctrl_H)
			&& curwin->w_cursor.خانہ > ins_compl_خانہ()
			&& (c = ins_compl_bs()) == NUL)
		استمر;

	    // When no match was selected or it was edited.
	    إذا (!ins_compl_ورتیا_میل())
	    {
		// CTRL-L: Add one character from the current match to
		// "compl_leader".  Except when at the original match and
		// there is nothing to add, CTRL-L works like CTRL-P then.
		إذا (c == Ctrl_L
			&& (!ctrl_x_منوال_سطر_or_eval()
			    || ins_compl_long_shown_میل()))
		{
		    ins_compl_addfrommatch();
		    استمر;
		}

		// A non-white character that fits in with the current
		// completion: Add to "compl_leader".
		إذا (ins_compl_accept_char(c))
		{
#إذا defined(FEAT_EVAL)
		    // Trigger InsertCharPre.
		    char_u *str = do_داخل_کرو_char_pre(c);
		    char_u *p;

		    إذا (str != عدم)
		    {
			لكل (p = str; *p != NUL; MB_PTR_ADV(p))
			    ins_compl_addleader(PTR2CHAR(p));
			vim_free(str);
		    }
		    وإلا
#جے_انت
			ins_compl_addleader(c);
		    استمر;
		}

		// Pressing CTRL-Y selects the current match.  When
		// ins_compl_enter_selects() is set the Enter key does the
		// same.
		إذا ((c == Ctrl_Y || (ins_compl_enter_selects()
				    && (c == CAR || c == K_KENTER || c == NL)))
			&& stop_arrow() == OK)
		{
		    ins_compl_delete();
		    إذا (ins_compl_preinsert_longest()
			    && !ins_compl_is_میل_selected())
		    {
			ins_compl_داخل_کرو(FALSE, TRUE);
			ins_compl_init_get_longest();
			استمر;
		    }
		    وإلا
			ins_compl_داخل_کرو(FALSE, FALSE);
		}
		// Delete preinserted text when typing special chars
		وإلا إذا (IS_WHITE_NL_OR_NUL(c) && ins_compl_preinsert_effect())
		    ins_compl_delete();
	    }
	}

	// Prepare for or stop CTRL-X mode.  This doesn't do completion, but
	// it does fix up the text when finishing completion.
	ins_compl_init_get_longest();
	إذا (ins_compl_prep(c))
	    استمر;

	// CTRL-\ CTRL-N goes to Normal mode,
	// CTRL-\ CTRL-G goes to mode selected with 'insertmode',
	// CTRL-\ CTRL-O is like CTRL-O but without moving the cursor.
	إذا (c == Ctrl_BSL)
	{
	    // may need to redraw when no more chars available now
	    ins_redraw(FALSE);
	    ++no_mapping;
	    ++allow_کلیدیں;
	    c = plain_vgetc();
	    --no_mapping;
	    --allow_کلیدیں;
	    إذا (c != Ctrl_N && c != Ctrl_G && c != Ctrl_O)
	    {
		// it's something else
		vungetc(c);
		c = Ctrl_BSL;
	    }
	    وإلا إذا (c == Ctrl_G && p_im)
		استمر;
	    وإلا
	    {
		إذا (c == Ctrl_O)
		{
		    ins_ctrl_o();
		    ins_at_eol = FALSE;	// cursor keeps its column
		    nomove = TRUE;
		}
		گنتی = 0;
		اذهب doESCkey;
	    }
	}

#جے_تعریف_شدہ FEAT_DIGRAPHS
	c = do_digraph(c);
#جے_انت

	إذا ((c == Ctrl_V || c == Ctrl_Q) && ctrl_x_منوال_cmdline())
	    اذهب docomplete;
	إذا (c == Ctrl_V || c == Ctrl_Q)
	{
	    ins_ctrl_v();
	    c = Ctrl_V;	// pretend CTRL-V is last typed character
	    استمر;
	}

	إذا (cindent_on() && ctrl_x_منوال_none())
	{
	    // A key name preceded by a bang means this key is not to be
	    // inserted.  Skip ahead to the re-indenting below.
	    // A key name preceded by a star means that indenting has to be
	    // done before inserting the key.
	    سطر_is_white = inindent(0);
	    إذا (وچ_cinkeys(c, '!', سطر_is_white))
		اذهب force_cindent;
	    إذا (can_cindent && وچ_cinkeys(c, '*', سطر_is_white)
							&& stop_arrow() == OK)
		do_c_اظہاریہ_indent();
	}

#جے_تعریف_شدہ FEAT_RIGHTLEFT
	إذا (curwin->w_p_rl)
	    بدل (c)
	    {
		حالة K_بایاں:	c = K_دایاں; اكسر;
		حالة K_S_بایاں:	c = K_S_دایاں; اكسر;
		حالة K_C_بایاں:	c = K_C_دایاں; اكسر;
		حالة K_دایاں:	c = K_بایاں; اكسر;
		حالة K_S_دایاں: c = K_S_بایاں; اكسر;
		حالة K_C_دایاں: c = K_C_بایاں; اكسر;
	    }
#جے_انت

	/*
	 * If 'keymodel' contains "startsel", may start selection.  If it
	 * does, a CTRL-O and c will be stuffed, we need to get these
	 * characters.
	 */
	إذا (ins_شروع_select(c))
	    استمر;

	/*
	 * The big switch to handle a character in insert mode.
	 */
	بدل (c)
	{
	حالة ESC:	// End input mode
	    إذا (echeck_abbr(ESC + ABBR_OFF))
		اكسر;
	    // FALLTHROUGH

	حالة Ctrl_C:	// End input mode
	    إذا (c == Ctrl_C && cmdwin_type != 0)
	    {
		// Close the cmdline window.
		cmdwin_نتیجہ = K_IGNORE;
		got_int = FALSE; // don't stop executing autocommands et al.
		nomove = TRUE;
		اذهب doESCkey;
	    }
#جے_تعریف_شدہ FEAT_کم_CHANNEL
	    إذا (c == Ctrl_C && bt_prompt(curbuf))
	    {
		إذا (invoke_prompt_رکاوٹ())
		{
		    إذا (!bt_prompt(curbuf))
			// buffer changed to a non-prompt buffer, get out of
			// Insert mode
			اذهب doESCkey;
		    اكسر;
		}
	    }
#جے_انت

#جے_تعریف_شدہ UNIX
do_intr:
#جے_انت
	    // when 'insertmode' set, and not halfway a mapping, don't leave
	    // Insert mode
	    إذا (goto_im())
	    {
		إذا (got_int)
		{
		    (فراغ)vgetc();		// flush all buffers
		    got_int = FALSE;
		}
		وإلا
		    vim_beep(BO_IM);
		اكسر;
	    }
doESCkey:
	    // Drop a pending autocomplete so it does not outlive Insert mode.
	    ins_compl_clear_autocomplete_delay();
	    /*
	     * This is the ONLY return from edit()!
	     */
	    // Always update o_lnum, so that a "CTRL-O ." that adds a line
	    // still puts the cursor back after the inserted text.
	    إذا (ins_at_eol && gchar_cursor() == NUL)
		o_lnum = curwin->w_cursor.lnum;

	    إذا (ins_esc(&گنتی, cmdchar, nomove))
	    {
		// When CTRL-C was typed got_int will be set, with the result
		// that the autocommands won't be executed. When mapped got_int
		// is not set, but let's keep the behavior the same.
		إذا (cmdchar != 'r' && cmdchar != 'v' && c != Ctrl_C)
		    ins_apply_autocmds(EVENT_INSERTLEAVE);
		did_cursorhold = FALSE;

		// ins_redraw() triggers TextChangedI only when no characters
		// are in the typeahead buffer, so reset curbuf->b_last_changedtick only
		// if the TextChangedI was not blocked by char_avail() (e.g. using :norm!)
		// and the TextChangedI autocommand has been triggered.
		إذا (!char_avail() && curbuf->b_آخری_changedtick_i == CHANGEDTICK(curbuf))
		    curbuf->b_آخری_changedtick = CHANGEDTICK(curbuf);
		أرجع (c == Ctrl_O);
	    }
	    استمر;

	حالة Ctrl_Z:	// suspend when 'insertmode' set
	    إذا (!p_im)
		اذهب normalchar;	// insert CTRL-Z as normal char
	    do_cmdline_cmd((char_u *)"stop");
#جے_تعریف_شدہ CURSOR_SHAPE
	    ui_cursor_shape();		// may need to update cursor shape
#جے_انت
	    استمر;

	حالة Ctrl_O:	// execute one command
#جے_تعریف_شدہ FEAT_COMPL_FUNC
	    إذا (ctrl_x_منوال_omni())
		اذهب docomplete;
#جے_انت
	    إذا (echeck_abbr(Ctrl_O + ABBR_OFF))
		اكسر;
	    ins_ctrl_o();

	    // don't move the cursor left when 'virtualedit' has "onemore".
	    إذا (get_ve_flags() & VE_ONEMORE)
	    {
		ins_at_eol = FALSE;
		nomove = TRUE;
	    }
	    گنتی = 0;
	    اذهب doESCkey;

	حالة K_INS:	// toggle insert/replace mode
	حالة K_KINS:
	    ins_داخل_کرو(replaceState);
	    اكسر;

	حالة K_SELECT:	// end of Select mode mapping - ignore
	    اكسر;

	حالة K_HELP:	// Help key works like <ESC> <Help>
	حالة K_F1:
	حالة K_XF1:
	    stuffcharReadbuff(K_HELP);
	    إذا (p_im)
		need_شروع_insertmode = TRUE;
	    اذهب doESCkey;

#جے_تعریف_شدہ FEAT_NETBEANS_INTG
	حالة K_F21:	// NetBeans command
	    ++no_mapping;		// don't map the next key hits
	    i = plain_vgetc();
	    --no_mapping;
	    netbeans_keycommand(i);
	    اكسر;
#جے_انت

	حالة K_ZERO:	// Insert the previously inserted text.
	حالة NUL:
	حالة Ctrl_A:
	    // For ^@ the trailing ESC will end the insert, unless there is an
	    // error.
	    إذا (stuff_inserted(NUL, 1L, (c == Ctrl_A)) == FAIL
						   && c != Ctrl_A && !p_im)
		اذهب doESCkey;		// quit insert mode
	    inserted_space = FALSE;
	    اكسر;

	حالة Ctrl_R:	// insert the contents of a register
	    إذا (ctrl_x_منوال_رجسٹر() && !ins_compl_سرگرم())
		اذهب docomplete;
	    ins_reg();
	    auto_format(FALSE, TRUE);
	    inserted_space = FALSE;
	    اكسر;

	حالة Ctrl_G:	// commands starting with CTRL-G
	    ins_ctrl_g();
	    اكسر;

	حالة Ctrl_HAT:	// switch input mode and/or langmap
	    ins_ctrl_hat();
	    اكسر;

#جے_تعریف_شدہ FEAT_RIGHTLEFT
	حالة Ctrl__:	// switch between languages
	    إذا (!p_ari)
		اذهب normalchar;
	    ins_ctrl_();
	    اكسر;
#جے_انت

	حالة Ctrl_D:	// Make indent one shiftwidth smaller.
#إذا defined(FEAT_لبھو_ID)
	    إذا (ctrl_x_منوال_رستہ_مقرر_کردا())
		اذهب docomplete;
#جے_انت
	    // FALLTHROUGH

	حالة Ctrl_T:	// Make indent one shiftwidth greater.
	    إذا (c == Ctrl_T && ctrl_x_منوال_thesaurus())
	    {
		إذا (has_compl_option(FALSE))
		    اذهب docomplete;
		اكسر;
	    }

	    ins_کھسکاؤ(c, lastc);
	    auto_format(FALSE, TRUE);
	    inserted_space = FALSE;
	    اكسر;

	حالة K_DEL:	// delete character under the cursor
	حالة K_KDEL:
	    ins_del();
	    auto_format(FALSE, TRUE);
	    اكسر;

	حالة K_BS:	// delete character before the cursor
	حالة K_S_BS:
	حالة Ctrl_H:
	    did_backspace = ins_bs(c, BACKSPACE_CHAR, &inserted_space);
	    auto_format(FALSE, TRUE);
	    إذا (did_backspace)
		MAY_TRIGGER_AUTOCOMPLETE(c);
	    اكسر;

	حالة Ctrl_W:	// delete word before the cursor
#جے_تعریف_شدہ FEAT_کم_CHANNEL
	    إذا (bt_prompt(curbuf) && (mod_نقاب & MOD_نقاب_کھسکاؤ) == 0)
	    {
		// In a prompt window CTRL-W is used for window commands.
		// Use Shift-CTRL-W to delete a word.
		stuffcharReadbuff(Ctrl_W);
		restart_edit = 'A';
		nomove = TRUE;
		گنتی = 0;
		اذهب doESCkey;
	    }
#جے_انت
	    did_backspace = ins_bs(c, BACKSPACE_لفظ, &inserted_space);
	    auto_format(FALSE, TRUE);
	    إذا (did_backspace)
		MAY_TRIGGER_AUTOCOMPLETE(c);
	    اكسر;

	حالة Ctrl_U:	// delete all inserted text in current line
#جے_تعریف_شدہ FEAT_COMPL_FUNC
	    // CTRL-X CTRL-U completes with 'completefunc'.
	    إذا (ctrl_x_منوال_function())
		اذهب docomplete;
#جے_انت
	    did_backspace = ins_bs(c, BACKSPACE_سطر, &inserted_space);
	    auto_format(FALSE, TRUE);
	    inserted_space = FALSE;
	    إذا (did_backspace)
		MAY_TRIGGER_AUTOCOMPLETE(c);
	    اكسر;

	حالة K_LEFTMOUSE:   // mouse keys
	حالة K_LEFTMOUSE_NM:
	حالة K_LEFTDRAG:
	حالة K_LEFTRELEASE:
	حالة K_LEFTRELEASE_NM:
	حالة K_MOUSEMOVE:
	حالة K_MIDDLEMOUSE:
	حالة K_MIDDLEDRAG:
	حالة K_MIDDLERELEASE:
	حالة K_RIGHTMOUSE:
	حالة K_RIGHTDRAG:
	حالة K_RIGHTRELEASE:
	حالة K_X1MOUSE:
	حالة K_X1DRAG:
	حالة K_X1RELEASE:
	حالة K_X2MOUSE:
	حالة K_X2DRAG:
	حالة K_X2RELEASE:
	    ins_mouse(c);
	    اكسر;

	حالة K_MOUSEDOWN: // Default action for scroll wheel up: scroll up
	    ins_mousescroll(MSCR_تھلے);
	    اكسر;

	حالة K_MOUSEUP:	// Default action for scroll wheel down: scroll down
	    ins_mousescroll(MSCR_اتے);
	    اكسر;

	حالة K_MOUSELEFT: // Scroll wheel left
	    ins_mousescroll(MSCR_بایاں);
	    اكسر;

	حالة K_MOUSERIGHT: // Scroll wheel right
	    ins_mousescroll(MSCR_دایاں);
	    اكسر;

	حالة K_PS:
	    bracketed_paste(PASTE_داخل_کرو, FALSE, عدم);
	    إذا (cmdchar == K_PS)
		// invoked from normal mode, bail out
		اذهب doESCkey;
	    اكسر;
	حالة K_PE:
	    // Got K_PE without K_PS, ignore.
	    اكسر;

#جے_تعریف_شدہ FEAT_GUI_TABLINE
	حالة K_TABLINE:
	حالة K_TABMENU:
	    ins_tabline(c);
	    اكسر;
#جے_انت

	حالة K_IGNORE:	// Something mapped to nothing
	    اكسر;

	حالة K_COMMAND:		    // <Cmd>command<CR>
	حالة K_SCRIPT_COMMAND:	    // <ScriptCmd>command<CR>
	    {
		bufref_T    save_curbuf;
		varnumber_T tick = CHANGEDTICK(curbuf);

		سیٹ_bufref(&save_curbuf, curbuf);
		do_cmdkey_command(c, 0);

#جے_تعریف_شدہ FEAT_TERMINAL
		إذا (رکن_use_loop())
		    // Started a terminal that gets the input, exit Insert mode.
		    اذهب doESCkey;
#جے_انت
		إذا (curbuf->b_u_synced
			|| (bufref_valid(&save_curbuf)
			    && curbuf == save_curbuf.br_buf
			    && tick != CHANGEDTICK(curbuf)))
		{
		    // The command synced undo or changed this buffer.
		    // Save the cursor line before the next typed edit.
		    ins_need_undo = TRUE;
		}
	    }
	    اكسر;

	حالة K_CURSORHOLD:	// Didn't type something for a while.
	    ins_apply_autocmds(EVENT_CURSORHOLDI);
	    did_cursorhold = TRUE;
	    // If CTRL-G U was used apply it to the next typed key.
	    إذا (dont_sync_undo == TRUE)
		dont_sync_undo = MAYBE;
	    اكسر;

	حالة K_COMPLETE_DELAY:	// 'autocompletedelay' expired
	    // If CTRL-G U was used apply it to the next typed key.
	    إذا (dont_sync_undo == TRUE)
		dont_sync_undo = MAYBE;
	    ins_compl_clear_autocomplete_delay();
	    إذا (!ins_compl_has_autocomplete() || char_avail()
		    || curwin->w_cursor.خانہ == 0)
		اكسر;
	    c = char_before_cursor();
	    إذا (!vim_isprintc(c))
		اكسر;
	    // The completion may have been cleared while waiting, so re-enable
	    // autocomplete to match a zero delay.
	    ins_compl_enable_autocomplete();
	    ins_compl_arm_autostart();
	    اذهب docomplete;

#جے_تعریف_شدہ FEAT_GUI_MSWIN
	    // On MS-Windows ignore <M-F4>, we get it when closing the window
	    // was cancelled.
	حالة K_F4:
	    إذا (mod_نقاب != MOD_نقاب_ALT)
		اذهب normalchar;
	    اكسر;
#جے_انت

#جے_تعریف_شدہ FEAT_GUI
	حالة K_VER_SCROLLBAR:
	    ins_scroll();
	    اكسر;

	حالة K_HOR_SCROLLBAR:
	    ins_horscroll();
	    اكسر;
#جے_انت

	حالة K_HOME:	// <Home>
	حالة K_KHOME:
	حالة K_S_HOME:
	حالة K_C_HOME:
	    ins_home(c);
	    اكسر;

	حالة K_سرا:	// <End>
	حالة K_KEND:
	حالة K_S_سرا:
	حالة K_C_سرا:
	    ins_سرا(c);
	    اكسر;

	حالة K_بایاں:	// <Left>
	    إذا (mod_نقاب & (MOD_نقاب_کھسکاؤ|MOD_نقاب_CTRL))
		ins_s_بایاں();
	    وإلا
		ins_بایاں();
	    اكسر;

	حالة K_S_بایاں:	// <S-Left>
	حالة K_C_بایاں:
	    ins_s_بایاں();
	    اكسر;

	حالة K_دایاں:	// <Right>
	    إذا (mod_نقاب & (MOD_نقاب_کھسکاؤ|MOD_نقاب_CTRL))
		ins_s_دایاں();
	    وإلا
		ins_دایاں();
	    اكسر;

	حالة K_S_دایاں:	// <S-Right>
	حالة K_C_دایاں:
	    ins_s_دایاں();
	    اكسر;

	حالة K_اتے:	// <Up>
	    إذا (pum_visible())
		اذهب docomplete;
	    إذا (mod_نقاب & MOD_نقاب_کھسکاؤ)
		ins_pageup();
	    وإلا
		ins_اتے(FALSE);
	    اكسر;

	حالة K_S_اتے:	// <S-Up>
	حالة K_PAGEUP:
	حالة K_KPAGEUP:
	    إذا (pum_visible())
	    {
#جے_تعریف_شدہ FEAT_PROP_POPUP
		// CTRL-SHIFT-<Up> scrolls the info popup up a line,
		// CTRL-SHIFT-<PageUp> a page.  Shift is folded into K_S_UP but
		// stays in mod_mask for PageUp, hence the asymmetric check.
		إذا (c == K_S_اتے ? (mod_نقاب & MOD_نقاب_CTRL)
			: ((mod_نقاب & MOD_نقاب_CTRL)
			    && (mod_نقاب & MOD_نقاب_کھسکاؤ)))
		{
		    popup_scroll_info(-1, c != K_S_اتے);
		    اكسر;
		}
#جے_انت
		اذهب docomplete;
	    }
	    ins_pageup();
	    اكسر;

	حالة K_تھلے:	// <Down>
	    إذا (pum_visible())
		اذهب docomplete;
	    إذا (mod_نقاب & MOD_نقاب_کھسکاؤ)
		ins_pagedown();
	    وإلا
		ins_تھلے(FALSE);
	    اكسر;

	حالة K_S_تھلے:	// <S-Down>
	حالة K_PAGEDOWN:
	حالة K_KPAGEDOWN:
	    إذا (pum_visible())
	    {
#جے_تعریف_شدہ FEAT_PROP_POPUP
		// CTRL-SHIFT-<Down>/<PageDown> scroll the info popup down.
		إذا (c == K_S_تھلے ? (mod_نقاب & MOD_نقاب_CTRL)
			: ((mod_نقاب & MOD_نقاب_CTRL)
			    && (mod_نقاب & MOD_نقاب_کھسکاؤ)))
		{
		    popup_scroll_info(1, c != K_S_تھلے);
		    اكسر;
		}
#جے_انت
		اذهب docomplete;
	    }
	    ins_pagedown();
	    اكسر;

#جے_تعریف_شدہ FEAT_DND
	حالة K_DROP:	// drag-n-drop event
	    ins_drop();
	    اكسر;
#جے_انت

	حالة K_S_TAB:	// When not mapped, use like a normal TAB
	    c = TAB;
	    // FALLTHROUGH

	حالة TAB:	// TAB or Complete patterns along path
#إذا defined(FEAT_لبھو_ID)
	    إذا (ctrl_x_منوال_رستہ_patterns())
		اذهب docomplete;
#جے_انت
	    inserted_space = FALSE;
	    إذا (ins_tab())
		اذهب normalchar;	// insert TAB as a normal char
	    auto_format(FALSE, TRUE);
	    اكسر;

	حالة K_KENTER:	// <Enter>
	    c = CAR;
	    // FALLTHROUGH
	حالة CAR:
	حالة NL:
#إذا defined(FEAT_QUICKFIX)
	    // In a quickfix window a <CR> jumps to the error under the
	    // cursor.
	    إذا (bt_quickfix(curbuf) && c == CAR)
	    {
		إذا (curwin->w_llist_ref == عدم)    // quickfix window
		    do_cmdline_cmd((char_u *)".cc");
		وإلا				    // location list window
		    do_cmdline_cmd((char_u *)".ll");
		اكسر;
	    }
#جے_انت
	    إذا (cmdwin_type != 0)
	    {
		// Execute the command in the cmdline window.
		cmdwin_نتیجہ = CAR;
		اذهب doESCkey;
	    }
#جے_تعریف_شدہ FEAT_کم_CHANNEL
	    إذا (bt_prompt(curbuf))
	    {
		invoke_prompt_callback();
		إذا (!bt_prompt(curbuf))
		    // buffer changed to a non-prompt buffer, get out of
		    // Insert mode
		    اذهب doESCkey;
		اكسر;
	    }
#جے_انت
	    إذا (ins_eol(c) == FAIL && !p_im)
		اذهب doESCkey;	    // out of memory
	    auto_format(FALSE, FALSE);
	    inserted_space = FALSE;
	    اكسر;

	حالة Ctrl_K:	    // digraph or keyword completion
	    إذا (ctrl_x_منوال_dictionary())
	    {
		إذا (has_compl_option(TRUE))
		    اذهب docomplete;
		اكسر;
	    }
#جے_تعریف_شدہ FEAT_DIGRAPHS
	    c = ins_digraph();
	    إذا (c == NUL)
		اكسر;
#جے_انت
	    اذهب normalchar;

	حالة Ctrl_X:	// Enter CTRL-X mode
	    ins_ctrl_x();
	    اكسر;

	حالة Ctrl_RSB:	// Tag name completion after ^X
	    إذا (!ctrl_x_منوال_ٹیگ_سوچی())
		اذهب normalchar;
	    اذهب docomplete;

	حالة Ctrl_F:	// File name completion after ^X
	    إذا (!ctrl_x_منوال_files())
		اذهب normalchar;
	    اذهب docomplete;

	حالة 's':	// Spelling completion after ^X
	حالة Ctrl_S:
	    إذا (!ctrl_x_منوال_spell())
		اذهب normalchar;
	    اذهب docomplete;

	حالة Ctrl_L:	// Whole line completion after ^X
	    إذا (!ctrl_x_منوال_سارا_سطر())
	    {
		// CTRL-L with 'insertmode' set: Leave Insert mode
		إذا (p_im)
		{
		    إذا (echeck_abbr(Ctrl_L + ABBR_OFF))
			اكسر;
		    اذهب doESCkey;
		}
		اذهب normalchar;
	    }
	    // FALLTHROUGH

	حالة Ctrl_P:	// Do previous/next pattern completion
	حالة Ctrl_N:
#جے_تعریف_شدہ FEAT_PROP_POPUP
	    // CTRL-SHIFT-P/N scroll the info popup one line.
	    إذا (pum_visible() && (mod_نقاب & MOD_نقاب_کھسکاؤ)
		    && (c == Ctrl_P || c == Ctrl_N))
	    {
		popup_scroll_info(c == Ctrl_P ? -1 : 1, false);
		اكسر;
	    }
#جے_انت
	    // if 'complete' is empty then plain ^P is no longer special,
	    // but it is under other ^X modes
	    إذا (*curbuf->b_p_cpt == NUL
		    && (ctrl_x_منوال_عام() || ctrl_x_منوال_سارا_سطر())
		    && !compl_حالت_local())
		اذهب normalchar;

docomplete:
	    ins_compl_clear_autocomplete_delay();
	    compl_busy = TRUE;
#جے_تعریف_شدہ FEAT_FOLDING
	    disable_fold_update++;  // don't redraw folds here
#جے_انت
	    إذا (ins_complete(c, TRUE) == FAIL)
		compl_حالت_clear();
#جے_تعریف_شدہ FEAT_FOLDING
	    disable_fold_update--;
#جے_انت
	    compl_busy = FALSE;
	    can_si = may_do_si(); // allow smartindenting
	    اكسر;

	حالة Ctrl_Y:	// copy from previous line or scroll down
	حالة Ctrl_E:	// copy from next line	   or scroll up
	    c = ins_ctrl_ey(c);
	    اكسر;

	افتراضي:
#جے_تعریف_شدہ UNIX
	    إذا (c == intr_char)		// special interrupt char
		اذهب do_intr;
#جے_انت

normalchar:
	    /*
	     * Insert a normal character.
	     */
#إذا defined(FEAT_EVAL)
	    إذا (!p_paste)
	    {
		// Trigger InsertCharPre.
		char_u *str = do_داخل_کرو_char_pre(c);
		char_u *p;

		إذا (str != عدم)
		{
		    إذا (*str != NUL && stop_arrow() != FAIL)
		    {
			// Insert the new value of v:char literally.
			لكل (p = str; *p != NUL; MB_PTR_ADV(p))
			{
			    c = PTR2CHAR(p);
			    إذا (c == CAR || c == K_KENTER || c == NL)
				ins_eol(c);
			    وإلا
				ins_char(c);
			}
			Append_منزل_Redobuff_Lit(str, -1);
		    }
		    vim_free(str);
		    c = NUL;
		}

		// If the new value is already inserted or an empty string
		// then don't insert any character.
		إذا (c == NUL)
		    اكسر;
	    }
#جے_انت
	    // Try to perform smart-indenting.
	    ins_try_si(c);

	    إذا (c == ' ')
	    {
		inserted_space = TRUE;
		إذا (inindent(0))
		    can_cindent = FALSE;
		إذا (Insstart_blank_vcol == MAXCOL
			&& curwin->w_cursor.lnum == Insstart.lnum)
		    Insstart_blank_vcol = get_nolist_virtcol();
	    }

	    // Insert a normal character and check for abbreviations on a
	    // special character.  Let CTRL-] expand abbreviations without
	    // inserting it.
	    إذا (vim_iswordc(c) || (!echeck_abbr(
			// Add ABBR_OFF for characters above 0x100, this is
			// what check_abbr() expects.
				(has_mbyte && c >= 0x100) ? (c + ABBR_OFF) : c)
			&& c != Ctrl_RSB))
	    {
		داخل_کرو_special(c, FALSE, FALSE);
#جے_تعریف_شدہ FEAT_RIGHTLEFT
		revins_legal++;
		revins_chars++;
#جے_انت
	    }

	    auto_format(FALSE, TRUE);

#جے_تعریف_شدہ FEAT_FOLDING
	    // When inserting a character the cursor line must never be in a
	    // closed fold.
	    foldOpenCursor();
#جے_انت
	    // Trigger autocompletion
	    إذا (ins_compl_has_autocomplete() && !char_avail()
		    && vim_isprintc(c))
		TRIGGER_AUTOCOMPLETE();

	    اكسر;
	}   // end of switch (c)

	// If typed something may trigger CursorHoldI again; K_COMPLETE_DELAY is
	// injected, not typed.
	إذا (c != K_CURSORHOLD && c != K_COMPLETE_DELAY
#جے_تعریف_شدہ FEAT_COMPL_FUNC
		// but not in CTRL-X mode, a script can't restore the state
		&& ctrl_x_منوال_عام()
#جے_انت
	       )
	    did_cursorhold = FALSE;

	// Check if we need to cancel completion mode because the window
	// or tab page was changed
	إذا (ins_compl_سرگرم() && !ins_compl_win_سرگرم(curwin))
	    ins_compl_cancel();

	// If the cursor was moved we didn't just insert a space
	إذا (arrow_ورتیا)
	    inserted_space = FALSE;

	إذا (can_cindent && cindent_on() && ctrl_x_منوال_عام())
	{
force_cindent:
	    /*
	     * Indent now if a key was typed that is in 'cinkeys'.
	     */
	    إذا (وچ_cinkeys(c, ' ', سطر_is_white))
	    {
		إذا (stop_arrow() == OK)
		    // re-indent the current line
		    do_c_اظہاریہ_indent();
	    }
	}

    }	// for (;;)
    // NOTREACHED
}

    صحيح
ins_need_undo_get(فراغ)
{
    أرجع ins_need_undo;
}

/*
 * Redraw for Insert mode.
 * This is postponed until getting the next character to make '$' in the 'cpo'
 * option work correctly.
 * Only redraw when there are no characters available.  This speeds up
 * inserting sequences of characters (e.g., for CTRL-R).
 */
    فراغ
ins_redraw(صحيح ready)	    // not busy with something
{
#جے_تعریف_شدہ FEAT_CONCEAL
    linenr_T	conceal_old_cursor_سطر = 0;
    linenr_T	conceal_new_cursor_سطر = 0;
    صحيح		conceal_update_سطریں = FALSE;
#جے_انت

    إذا (char_avail())
	أرجع;

    // Trigger CursorMoved if the cursor moved.  Not when the popup menu is
    // visible, the command might delete it.
    إذا (ready && (has_cursormovedI()
#جے_تعریف_شدہ FEAT_PROP_POPUP
		|| popup_visible
#جے_انت
#إذا defined(FEAT_CONCEAL)
		|| curwin->w_p_cole > 0
#جے_انت
		)
	    && !EQUAL_POS(آخری_cursormoved, curwin->w_cursor)
	    && !pum_visible())
    {
#جے_تعریف_شدہ FEAT_SYN_HL
	// Need to update the screen first, to make sure syntax
	// highlighting is correct after making a change (e.g., inserting
	// a "(".  The autocommand may also require a redraw, so it's done
	// again below, unfortunately.
	إذا (syntax_present(curwin) && must_redraw)
	    update_screen(0);
#جے_انت
	إذا (has_cursormovedI())
	{
	    // Make sure curswant is correct, an autocommand may call
	    // getcurpos().
	    update_curswant();
	    ins_apply_autocmds(EVENT_CURSORMOVEDI);
	}
#جے_تعریف_شدہ FEAT_PROP_POPUP
	إذا (popup_visible)
	    popup_check_cursor_pos();
#جے_انت
#جے_تعریف_شدہ FEAT_CONCEAL
	إذا (curwin->w_p_cole > 0)
	{
	    conceal_old_cursor_سطر = آخری_cursormoved.lnum;
	    conceal_new_cursor_سطر = curwin->w_cursor.lnum;
	    conceal_update_سطریں = TRUE;
	}
#جے_انت
	آخری_cursormoved = curwin->w_cursor;
    }

    // Trigger TextChangedI if b_changedtick_i differs.
    إذا (ready && has_textchangedI()
	    && curbuf->b_آخری_changedtick_i != CHANGEDTICK(curbuf)
	    && !pum_visible())
    {
	aco_save_T	aco;
	varnumber_T	tick = CHANGEDTICK(curbuf);

	// Save and restore curwin and curbuf, in case the autocmd changes
	// them.
	aucmd_prepbuf(&aco, curbuf);
	apply_autocmds(EVENT_TEXTCHANGEDI, عدم, عدم, FALSE, curbuf);
	aucmd_restbuf(&aco);
	curbuf->b_آخری_changedtick_i = CHANGEDTICK(curbuf);
	إذا (tick != CHANGEDTICK(curbuf))  // see ins_apply_autocmds()
	    u_save(curwin->w_cursor.lnum,
					(linenr_T)(curwin->w_cursor.lnum + 1));
    }

    // Trigger TextChangedP if b_changedtick_pum differs. When the popupmenu
    // closes TextChangedI will need to trigger for backwards compatibility,
    // thus use different b_last_changedtick* variables.
    إذا (ready && has_textchangedP()
	    && curbuf->b_آخری_changedtick_pum != CHANGEDTICK(curbuf)
	    && pum_visible())
    {
	aco_save_T	aco;
	varnumber_T	tick = CHANGEDTICK(curbuf);

	// Save and restore curwin and curbuf, in case the autocmd changes
	// them.
	aucmd_prepbuf(&aco, curbuf);
	apply_autocmds(EVENT_TEXTCHANGEDP, عدم, عدم, FALSE, curbuf);
	aucmd_restbuf(&aco);
	curbuf->b_آخری_changedtick_pum = CHANGEDTICK(curbuf);
	إذا (tick != CHANGEDTICK(curbuf))  // see ins_apply_autocmds()
	    u_save(curwin->w_cursor.lnum,
					(linenr_T)(curwin->w_cursor.lnum + 1));
    }

    إذا (ready)
	may_trigger_win_scrolled_resized();

    // Trigger SafeState if nothing is pending.
    may_trigger_safestate(ready
	    && !ins_compl_سرگرم()
	    && !pum_visible());

#إذا defined(FEAT_CONCEAL)
    إذا ((conceal_update_سطریں
	    && (conceal_old_cursor_سطر != conceal_new_cursor_سطر
		|| conceal_cursor_سطر(curwin)))
	    || need_cursor_سطر_redraw)
    {
	إذا (conceal_old_cursor_سطر != conceal_new_cursor_سطر)
	    redrawWinline(curwin, conceal_old_cursor_سطر);
	redrawWinline(curwin, conceal_new_cursor_سطر == 0
			    ? curwin->w_cursor.lnum : conceal_new_cursor_سطر);
	curwin->w_valid &= ~VALID_CROW;
	need_cursor_سطر_redraw = FALSE;
    }
#جے_انت
    إذا (must_redraw)
	update_screen(0);
    وإلا إذا (clear_cmdline || redraw_cmdline)
	showmode();		// clear cmdline and show mode
    showruler(FALSE);
    setcursor();
    emsg_on_display = FALSE;	// may remove error message now
}

/*
 * Handle a CTRL-V or CTRL-Q typed in Insert mode.
 */
    ساكن فراغ
ins_ctrl_v(فراغ)
{
    صحيح		c;
    صحيح		did_putchar = FALSE;

    // may need to redraw when no more chars available now
    ins_redraw(FALSE);

    إذا (redrawing() && !char_avail())
    {
	edit_putchar('^', TRUE);
	did_putchar = TRUE;
    }
    Append_منزل_Redobuff((char_u *)CTRL_V_STR);	// CTRL-V

    جوڑو_منزل_showcmd_c(Ctrl_V);

    // Do not change any modifyOtherKeys ESC sequence to a normal key for
    // CTRL-SHIFT-V.
    c = get_literal(mod_نقاب & MOD_نقاب_کھسکاؤ);
    إذا (did_putchar)
	// when the line fits in 'columns' the '^' is at the start of the next
	// line and will not removed by the redraw
	edit_unputchar();
    clear_showcmd();

    داخل_کرو_special(c, FALSE, TRUE);
#جے_تعریف_شدہ FEAT_RIGHTLEFT
    revins_chars++;
    revins_legal++;
#جے_انت
}

/*
 * After getting an ESC or CSI for a literal key: If the typeahead buffer
 * contains a modifyOtherKeys sequence then decode it and return the result.
 * Otherwise return "c".
 * Note that this doesn't wait for characters, they must be in the typeahead
 * buffer already.
 */
    ساكن صحيح
decode_Modify_Other_کلیدیں(صحيح c)
{
    char_u  *p = typebuf.tb_buf + typebuf.tb_off;
    صحيح	    idx;
    صحيح	    form = 0;
    صحيح	    argidx = 0;
    صحيح	    arg[2] = {0, 0};

    // Recognize:
    // form 0: {lead}{key};{modifier}u
    // form 1: {lead}27;{modifier};{key}~
    إذا (typebuf.tb_len >= 4 && (c == CSI || (c == ESC && *p == '[')))
    {
	idx = (*p == '[');
	إذا (p[idx] == '2' && p[idx + 1] == '7' && p[idx + 2] == ';' &&
		kitty_protocol_state != KKPS_ENABLED)
	{
	    form = 1;
	    idx += 3;
	}
	طالما (idx < typebuf.tb_len && argidx < 2)
	{
	    إذا (p[idx] == ';')
		++argidx;
	    وإلا إذا (VIM_ISDIGIT(p[idx]))
		arg[argidx] = arg[argidx] * 10 + (p[idx] - '0');
	    وإلا
		اكسر;
	    ++idx;
	}
	صحيح kitty_no_mods = argidx == 0 && kitty_protocol_state == KKPS_ENABLED;
	إذا (idx < typebuf.tb_len
		&& p[idx] == (form == 1 ? '~' : 'u')
		&& (argidx == 1 || kitty_no_mods))
	{
	    // Match, consume the code.
	    typebuf.tb_off += idx + 1;
	    typebuf.tb_len -= idx + 1;
#إذا defined(FEAT_CLIENTSERVER) || defined(FEAT_EVAL)
	    إذا (typebuf.tb_len == 0)
		typebuf_was_filled = FALSE;
#جے_انت

	    mod_نقاب = kitty_no_mods ? 0 : decode_modifiers(arg[!form]);
	    c = ملاؤ_modify_Other_کلیدیں(arg[form], &mod_نقاب);
	}
    }

    أرجع c;
}

/*
 * Put a character directly onto the screen.  It's not stored in a buffer.
 * Used while handling CTRL-K, CTRL-V, etc. in Insert mode.
 */
ساكن صحيح  pc_حالت;
#تعریف_کرو PC_حالت_UNSET	0	// pc_bytes was not set
#تعریف_کرو PC_حالت_دایاں	1	// right half of double-wide char
#تعریف_کرو PC_حالت_بایاں	2	// left half of double-wide char
#تعریف_کرو PC_حالت_سیٹ	3	// pc_bytes was filled
ساكن char_u pc_بائٹاں[MB_MAXBYTES + 1]; // saved bytes
ساكن صحيح  pc_attr;
ساكن صحيح  pc_صف;
ساكن صحيح  pc_خانہ;

    فراغ
edit_putchar(صحيح c, صحيح highlight)
{
    صحيح	    attr;

    إذا (Screen_سطریں == عدم)
	أرجع;

    update_topline();	// just in case w_topline isn't valid
    validate_cursor();
    إذا (highlight)
	attr = HL_ATTR(HLF_8);
    وإلا
	attr = 0;
    pc_صف = W_WINROW(curwin) + curwin->w_wrow;
    pc_خانہ = curwin->w_wincol;
    pc_حالت = PC_حالت_UNSET;
#جے_تعریف_شدہ FEAT_RIGHTLEFT
    إذا (curwin->w_p_rl)
    {
	pc_خانہ += curwin->w_چوڑائی - 1 - curwin->w_wcol;
	إذا (has_mbyte)
	{
	    صحيح fix_خانہ = mb_fix_خانہ(pc_خانہ, pc_صف);

	    إذا (fix_خانہ != pc_خانہ)
	    {
		screen_putchar(' ', pc_صف, fix_خانہ, attr);
		--curwin->w_wcol;
		pc_حالت = PC_حالت_دایاں;
	    }
	}
    }
    وإلا
#جے_انت
    {
	pc_خانہ += curwin->w_wcol;
	إذا (mb_lefthalve(pc_صف, pc_خانہ))
	    pc_حالت = PC_حالت_بایاں;
    }

    // save the character to be able to put it back
    إذا (pc_حالت == PC_حالت_UNSET)
    {
	screen_getbytes(pc_صف, pc_خانہ, pc_بائٹاں, &pc_attr);
	pc_حالت = PC_حالت_سیٹ;
    }
    screen_putchar(c, pc_صف, pc_خانہ, attr);
}

#إذا defined(FEAT_کم_CHANNEL)
/*
 * Set the insert start position for when using a prompt buffer.
 */
    فراغ
سیٹ_insstart(linenr_T lnum, صحيح خانہ)
{
    Insstart.lnum = lnum;
    Insstart.خانہ = خانہ;
    Insstart_orig = Insstart;
    Insstart_textlen = Insstart.خانہ;
    Insstart_blank_vcol = MAXCOL;
    arrow_ورتیا = FALSE;
}
#جے_انت

/*
 * Undo the previous edit_putchar().
 */
    فراغ
edit_unputchar(فراغ)
{
    إذا (pc_حالت != PC_حالت_UNSET && pc_صف >= پیغام_scrolled)
    {
	إذا (pc_حالت == PC_حالت_دایاں)
	    ++curwin->w_wcol;
	إذا (pc_حالت == PC_حالت_دایاں || pc_حالت == PC_حالت_بایاں)
	    redrawWinline(curwin, curwin->w_cursor.lnum);
	وإلا
	    screen_puts(pc_بائٹاں, pc_صف - پیغام_scrolled, pc_خانہ, pc_attr);
    }
}

/*
 * Called when "$" is in 'cpoptions': display a '$' at the end of the changed
 * text.  Only works when cursor is in the line that changes.
 */
    فراغ
display_dollar(colnr_T خانہ_arg)
{
    colnr_T خانہ = خانہ_arg < 0 ? 0 : خانہ_arg;
    colnr_T save_خانہ;

    إذا (!redrawing())
	أرجع;

    cursor_off();
    save_خانہ = curwin->w_cursor.خانہ;
    curwin->w_cursor.خانہ = خانہ;
    إذا (has_mbyte)
    {
	char_u *p;

	// If on the last byte of a multi-byte move to the first byte.
	p = ml_get_curline();
	curwin->w_cursor.خانہ -= (*mb_سر_off)(p, p + خانہ);
    }
    curs_columns(FALSE);	    // recompute w_wrow and w_wcol
    إذا (curwin->w_wcol < curwin->w_چوڑائی)
    {
	edit_putchar('$', FALSE);
	dollar_vcol = curwin->w_virtcol;
    }
    curwin->w_cursor.خانہ = save_خانہ;
}

/*
 * Call this function before moving the cursor from the normal insert position
 * in insert mode.
 */
    فراغ
undisplay_dollar(فراغ)
{
    إذا (dollar_vcol < 0)
	أرجع;

    dollar_vcol = -1;
    redrawWinline(curwin, curwin->w_cursor.lnum);
}

/*
 * Truncate the space at the end of a line.  This is to be used only in an
 * insert mode.  It handles fixing the replace stack for MODE_REPLACE and
 * MODE_VREPLACE modes.
 */
    فراغ
truncate_spaces(char_u *سطر, size_t len)
{
    صحيح	    i;

    // find start of trailing white space
    لكل (i = (صحيح)len - 1; i >= 0 && VIM_ISWHITE(سطر[i]); i--)
    {
	إذا (State & REPLACE_FLAG)
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
    فراغ
backspace_until_کالم(صحيح خانہ)
{
    طالما ((صحيح)curwin->w_cursor.خانہ > خانہ)
    {
	curwin->w_cursor.خانہ--;
	إذا (State & REPLACE_FLAG)
	    replace_do_bs(خانہ);
	وإلا إذا (!del_char_after_خانہ(خانہ))
	    اكسر;
    }
}

/*
 * Like del_char(), but make sure not to go before column "limit_col".
 * Only matters when there are composing characters.
 * Return TRUE when something was deleted.
 */
    ساكن صحيح
del_char_after_خانہ(صحيح حد_بندی_خانہ بن_ورتیا)
{
    إذا (enc_utf8 && حد_بندی_خانہ >= 0)
    {
	colnr_T ecol = curwin->w_cursor.خانہ + 1;

	// Make sure the cursor is at the start of a character, but
	// skip forward again when going too far back because of a
	// composing character.
	mb_adjust_cursor();
	طالما (curwin->w_cursor.خانہ < (colnr_T)حد_بندی_خانہ)
	{
	    صحيح l = utf_ptr2len(ml_get_cursor());

	    إذا (l == 0)  // end of line
		اكسر;
	    curwin->w_cursor.خانہ += l;
	}
	إذا (*ml_get_cursor() == NUL || curwin->w_cursor.خانہ == ecol)
	    أرجع FALSE;
	del_بائٹاں((لما)((صحيح)ecol - curwin->w_cursor.خانہ), FALSE, TRUE);
    }
    وإلا
	(فراغ)del_char(FALSE);
    أرجع TRUE;
}

/*
 * Next character is interpreted literally.
 * A one, two or three digit decimal number is interpreted as its byte value.
 * If one or two digits are entered, the next character is given to vungetc().
 * For Unicode a character > 255 may be returned.
 * If "noReduceKeys" is TRUE do not change any modifyOtherKeys ESC sequence
 * into a normal key, return ESC.
 */
    صحيح
get_literal(صحيح no_Reduce_کلیدیں)
{
    صحيح		cc;
    صحيح		nc;
    صحيح		i;
    صحيح		hex = FALSE;
    صحيح		octal = FALSE;
    صحيح		unicode = 0;

    إذا (got_int)
	أرجع Ctrl_C;

#جے_تعریف_شدہ FEAT_GUI
    /*
     * In GUI there is no point inserting the internal code for a special key.
     * It is more useful to insert the string "<KEY>" instead.	This would
     * probably be useful in a text window too, but it would not be
     * vi-compatible (maybe there should be an option for it?) -- webb
     */
    إذا (gui.وچ_use)
    {
	++allow_کلیدیں;
	إذا (no_Reduce_کلیدیں)
	    ++no_reduce_کلیدیں;
    }
#جے_انت
#جے_تعریف_شدہ USE_ON_FLY_SCROLL
    dont_scroll = TRUE;		// disallow scrolling here
#جے_انت
    ++no_mapping;		// don't map the next key hits
    cc = 0;
    i = 0;
    لكل (;;)
    {
	nc = plain_vgetc();
	إذا ((nc == ESC || nc == CSI) && !no_Reduce_کلیدیں)
	    nc = decode_Modify_Other_کلیدیں(nc);

	إذا ((mod_نقاب & ~MOD_نقاب_کھسکاؤ) != 0)
	    // A character with non-Shift modifiers should not be a valid
	    // character for i_CTRL-V_digit.
	    اكسر;

	إذا ((State & منوال_CMDLINE) == 0 && MB_BYTE2LEN_CHECK(nc) == 1)
	    جوڑو_منزل_showcmd(nc);
	إذا (nc == 'x' || nc == 'X')
	    hex = TRUE;
	وإلا إذا (nc == 'o' || nc == 'O')
	    octal = TRUE;
	وإلا إذا (nc == 'u' || nc == 'U')
	    unicode = nc;
	وإلا
	{
	    إذا (hex || unicode != 0)
	    {
		إذا (!vim_isxdigit(nc))
		    اكسر;
		cc = cc * 16 + hex2nr(nc);
	    }
	    وإلا إذا (octal)
	    {
		إذا (nc < '0' || nc > '7')
		    اكسر;
		cc = cc * 8 + nc - '0';
	    }
	    وإلا
	    {
		إذا (!VIM_ISDIGIT(nc))
		    اكسر;
		cc = cc * 10 + nc - '0';
	    }

	    ++i;
	}

	إذا (cc > 255 && unicode == 0)
	    cc = 255;		// limit range to 0-255
	nc = 0;

	إذا (hex)		// hex: up to two chars
	{
	    إذا (i >= 2)
		اكسر;
	}
	وإلا إذا (unicode)	// Unicode: up to four or eight chars
	{
	    إذا ((unicode == 'u' && i >= 4) || (unicode == 'U' && i >= 8))
		اكسر;
	}
	وإلا إذا (i >= 3)	// decimal or octal: up to three chars
	    اكسر;
    }
    إذا (i == 0)	    // no number entered
    {
	إذا (nc == K_ZERO)   // NUL is stored as NL
	{
	    cc = '\n';
	    nc = 0;
	}
	وإلا
	{
	    cc = nc;
	    nc = 0;
	}
    }

    إذا (cc == 0)	// NUL is stored as NL
	cc = '\n';
    إذا (enc_dbcs && (cc & 0xff) == 0)
	cc = '?';	// don't accept an illegal DBCS char, the NUL in the
			// second byte will cause trouble!

    --no_mapping;
#جے_تعریف_شدہ FEAT_GUI
    إذا (gui.وچ_use)
    {
	--allow_کلیدیں;
	إذا (no_Reduce_کلیدیں)
	    --no_reduce_کلیدیں;
    }
#جے_انت
    إذا (nc)
    {
	vungetc(nc);
	// A character typed with i_CTRL-V_digit cannot have modifiers.
	mod_نقاب = 0;
    }
    got_int = FALSE;	    // CTRL-C typed after CTRL-V is not an interrupt
    أرجع cc;
}

/*
 * Insert character, taking care of special keys and mod_mask
 */
    ساكن فراغ
داخل_کرو_special(
    صحيح	    c,
    صحيح	    allow_modmask,
    صحيح	    ctrlv)	    // c was typed after CTRL-V
{
    char_u  *p;
    صحيح	    len;

    /*
     * Special function key, translate into "<Key>". Up to the last '>' is
     * inserted with ins_str(), so as not to replace characters in replace
     * mode.
     * Only use mod_mask for special keys, to avoid things like <S-Space>,
     * unless 'allow_modmask' is TRUE.
     */
#إذا defined(MACOS_X) || defined(FEAT_GUI_GTK)
    // Command-key never produces a normal key
    إذا (mod_نقاب & MOD_نقاب_CMD)
	allow_modmask = TRUE;
#جے_انت
    إذا (IS_SPECIAL(c) || (mod_نقاب && allow_modmask))
    {
	p = get_special_کلید_name(c, mod_نقاب);
	len = (صحيح)STRLEN(p);
	c = p[len - 1];
	إذا (len > 2)
	{
	    إذا (stop_arrow() == FAIL)
		أرجع;
	    p[len - 1] = NUL;
	    ins_str(p, len - 1);
	    Append_منزل_Redobuff_Lit(p, -1);
	    ctrlv = FALSE;
	}
    }
    إذا (stop_arrow() == OK)
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
    فراغ
insertchar(
    صحيح		c,			// character to insert or NUL
    صحيح		flags,			// INSCHAR_FORMAT, etc.
    صحيح		دوجا_indent)		// indent for second line if >= 0
{
    صحيح		textwidth;
    char_u	*p;
    صحيح		fo_ins_blank;
    صحيح		force_format = flags & INSCHAR_FORMAT;

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
    إذا (textwidth > 0
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
#إذا defined(FEAT_EVAL)
	صحيح     do_اندرونی = TRUE;
	colnr_T virtcol = get_nolist_virtcol()
				  + char2cells(c != NUL ? c : gchar_cursor());

	إذا (*curbuf->b_p_fex != NUL && (flags & INSCHAR_NO_FEX) == 0
		&& (force_format || virtcol > (colnr_T)textwidth))
	{
	    do_اندرونی = (fex_format(curwin->w_cursor.lnum, 1L, c) != 0);
	    // It may be required to save for undo again, e.g. when setline()
	    // was called.
	    ins_need_undo = TRUE;
	}
	إذا (do_اندرونی)
#جے_انت
	    اندرونی_format(textwidth, دوجا_indent, flags, c == NUL, c);
    }

    إذا (c == NUL)	    // only formatting was wanted
	أرجع;

    // Check whether this character should end a comment.
    إذا (did_ai && c == سرا_comment_pending)
    {
	char_u  *سطر;
	char_u	lead_سرا[COM_MAX_LEN];	    // end-comment string
	صحيح	درمیان_len, سرا_len;
	صحيح	i;

	/*
	 * Need to remove existing (middle) comment leader and insert end
	 * comment leader.  First, check what comment leader we can find.
	 */
	i = get_leader_len(سطر = ml_get_curline(), &p, FALSE, TRUE);
	إذا (i > 0 && vim_strchr(p, COM_درمیان) != عدم)	// Just checking
	{
	    // Skip middle-comment string
	    طالما (*p && p[-1] != ':')	// find end of middle flags
		++p;
	    درمیان_len = copy_option_part(&p, lead_سرا, COM_MAX_LEN, ",");
	    // Don't count trailing white space for middle_len
	    طالما (درمیان_len > 0 && VIM_ISWHITE(lead_سرا[درمیان_len - 1]))
		--درمیان_len;

	    // Find the end-comment string
	    طالما (*p && p[-1] != ':')	// find end of end flags
		++p;
	    سرا_len = copy_option_part(&p, lead_سرا, COM_MAX_LEN, ",");

	    // Skip white space before the cursor
	    i = curwin->w_cursor.خانہ;
	    طالما (--i >= 0 && VIM_ISWHITE(سطر[i]))
		;
	    i++;

	    // Skip to before the middle leader
	    i -= درمیان_len;

	    // Check some expected things before we go on
	    إذا (i >= 0 && سرا_len > 0 && lead_سرا[سرا_len - 1] == سرا_comment_pending)
	    {
		// Backspace over all the stuff we want to replace
		backspace_until_کالم(i);

		// Insert the end-comment string, except for the last
		// character, which will get inserted as normal later.
		ins_بائٹاں_len(lead_سرا, سرا_len - 1);
	    }
	}
    }
    سرا_comment_pending = NUL;

    did_ai = FALSE;
    did_si = FALSE;
    can_si = FALSE;
    can_si_پٹھ = FALSE;

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
#جے_تعریف_شدہ USE_ON_FLY_SCROLL
    dont_scroll = FALSE;		// allow scrolling here
#جے_انت

    إذا (       !ISSPECIAL(c)
	    && (!has_mbyte || (*mb_char2len)(c) == 1)
	    && !has_insertcharpre()
#جے_تعریف_شدہ FEAT_EVAL
	    // Skip typeahead if test_override("char_avail", 1) was called.
	    && !disable_char_avail_for_testing
#جے_انت
	    && vpeekc() != NUL
	    && !(State & REPLACE_FLAG)
	    && !cindent_on()
#جے_تعریف_شدہ FEAT_RIGHTLEFT
	    && !p_ri
#جے_انت
	   )
    {
#تعریف_کرو داخلہ_BUFLEN 100
	char_u		buf[داخلہ_BUFLEN + 1];
	صحيح		i;
	colnr_T		virtcol = 0;

	buf[0] = c;
	i = 1;
	إذا (textwidth > 0)
	    virtcol = get_nolist_virtcol();
	/*
	 * Stop the string when:
	 * - no more chars available
	 * - finding a special character (command key)
	 * - buffer is full
	 * - running into the 'textwidth' boundary
	 * - need to check for abbreviation: A non-word char after a word-char
	 */
	طالما (	   (c = vpeekc()) != NUL
		&& !ISSPECIAL(c)
		&& (!has_mbyte || MB_BYTE2LEN_CHECK(c) == 1)
		&& i < داخلہ_BUFLEN
		&& (textwidth == 0
		    || (virtcol += byte2cells(buf[i - 1])) < (colnr_T)textwidth)
		&& !(!no_abbr && !vim_iswordc(c) && vim_iswordc(buf[i - 1])))
	{
#جے_تعریف_شدہ FEAT_RIGHTLEFT
	    c = vgetc();
	    إذا (p_hkmap && کلید_Typed)
		c = hkmap(c);		    // Hebrew mode mapping
	    buf[i++] = c;
#وإلا
	    buf[i++] = vgetc();
#جے_انت
	}

#جے_تعریف_شدہ FEAT_DIGRAPHS
	do_digraph(-1);			// clear digraphs
	do_digraph(buf[i-1]);		// may be the start of a digraph
#جے_انت
	buf[i] = NUL;
	ins_str(buf, i);
	إذا (flags & INSCHAR_CTRLV)
	{
	    redo_literal(*buf);
	    i = 1;
	}
	وإلا
	    i = 0;
	إذا (buf[i] != NUL)
	    Append_منزل_Redobuff_Lit(buf + i, -1);
    }
    وإلا
    {
	صحيح		cc;

	إذا (has_mbyte && (cc = (*mb_char2len)(c)) > 1)
	{
	    char_u	buf[MB_MAXBYTES + 1];

	    (*mb_char2bytes)(c, buf);
	    buf[cc] = NUL;
	    ins_char_بائٹاں(buf, cc);
	    Append_Char_منزل_Redobuff(c);
	}
	وإلا
	{
	    ins_char(c);
	    إذا (flags & INSCHAR_CTRLV)
		redo_literal(c);
	    وإلا
		Append_Char_منزل_Redobuff(c);
	}
    }
}

/*
 * Put a character in the redo buffer, for when just after a CTRL-V.
 */
    ساكن فراغ
redo_literal(صحيح c)
{
    char_u	buf[10];

    // Only digits need special treatment.  Translate them into a string of
    // three digits.
    إذا (VIM_ISDIGIT(c))
    {
	vim_snprintf((حرف *)buf, حجم(buf), "%03d", c);
	Append_منزل_Redobuff(buf);
    }
    وإلا
	Append_Char_منزل_Redobuff(c);
}

/*
 * start_arrow() is called when an arrow key is used in insert mode.
 * For undo/redo it resembles hitting the <ESC> key.
 */
    فراغ
شروع_arrow(
    pos_T    *سرا_داخل_کرو_pos)		// can be NULL
{
    شروع_arrow_common(سرا_داخل_کرو_pos, TRUE);
}

/*
 * Like start_arrow() but with end_change argument.
 * Will prepare for redo of CTRL-G U if "end_change" is FALSE.
 */
    ساكن فراغ
شروع_arrow_with_change(
    pos_T    *سرا_داخل_کرو_pos,		// can be NULL
    صحيح	      سرا_change)		// end undoable change
{
    شروع_arrow_common(سرا_داخل_کرو_pos, سرا_change);
    إذا (!سرا_change)
    {
	Append_Char_منزل_Redobuff(Ctrl_G);
	Append_Char_منزل_Redobuff('U');
    }
}

    ساكن فراغ
شروع_arrow_common(
    pos_T    *سرا_داخل_کرو_pos,		// can be NULL
    صحيح	      سرا_change)		// end undoable change
{
    إذا (!arrow_ورتیا && سرا_change)	// something has been inserted
    {
	Append_منزل_Redobuff(ESC_STR);
	stop_داخل_کرو(سرا_داخل_کرو_pos, FALSE, FALSE);
	arrow_ورتیا = TRUE;	// this means we stopped the current insert
    }
#جے_تعریف_شدہ FEAT_SPELL
    check_spell_redraw();
#جے_انت
}

#جے_تعریف_شدہ FEAT_SPELL
/*
 * If we skipped highlighting word at cursor, do it now.
 * It may be skipped again, thus reset spell_redraw_lnum first.
 */
    ساكن فراغ
check_spell_redraw(فراغ)
{
    إذا (spell_redraw_lnum != 0)
    {
	linenr_T	lnum = spell_redraw_lnum;

	spell_redraw_lnum = 0;
	redrawWinline(curwin, lnum);
    }
}

#جے_انت

/*
 * stop_arrow() is called before a change is made in insert mode.
 * If an arrow key has been used, start a new insertion.
 * Returns FAIL if undo is impossible, shouldn't insert then.
 */
    صحيح
stop_arrow(فراغ)
{
    إذا (arrow_ورتیا)
    {
	Insstart = curwin->w_cursor;	// new insertion starts here
	إذا (Insstart.خانہ > Insstart_orig.خانہ && !ins_need_undo)
	    // Don't update the original insert position when moved to the
	    // right, except when nothing was inserted yet.
	    update_Insstart_orig = FALSE;
	Insstart_textlen = (colnr_T)linetabsize_str(ml_get_curline());

	إذا (u_save_cursor() == OK)
	{
	    arrow_ورتیا = FALSE;
	    ins_need_undo = FALSE;
	}

	ai_خانہ = 0;
	إذا (State & VREPLACE_FLAG)
	{
	    orig_سطر_گنتی = curbuf->b_ml.ml_سطر_گنتی;
	    vr_سطریں_بدلیا = 1;
	}
	ResetRedobuff();
	Append_منزل_Redobuff((char_u *)"1i");   // pretend we start an insertion
	new_داخل_کرو_skip = 2;
    }
    وإلا إذا (ins_need_undo)
    {
	إذا (u_save_cursor() == OK)
	{
	    // A command or event may have moved the cursor before the next
	    // edit. Pull Insstart back only when the cursor moved above it,
	    // so that later edits can properly decide whether an extra undo
	    // entry is needed. Advancing Insstart would mis-place '[ after a
	    // register paste.
	    إذا (LT_POS(curwin->w_cursor, Insstart))
	    {
		Insstart = curwin->w_cursor;
		Insstart_textlen = (colnr_T)linetabsize_str(ml_get_curline());
	    }
	    ins_need_undo = FALSE;
	}
    }

#جے_تعریف_شدہ FEAT_FOLDING
    // Always open fold at the cursor line when inserting something.
    foldOpenCursor();
#جے_انت

    أرجع (arrow_ورتیا || ins_need_undo ? FAIL : OK);
}

/*
 * Do a few things to stop inserting.
 * "end_insert_pos" is where insert ended.  It is NULL when we already jumped
 * to another window/buffer.
 */
    ساكن فراغ
stop_داخل_کرو(
    pos_T	*سرا_داخل_کرو_pos,
    صحيح		esc,			// called by ins_esc()
    صحيح		nomove)			// <c-\><c-o>, don't move cursor
{
    صحيح		cc;
    string_T	inserted;

    stop_redo_ins();
    replace_flush();		// abandon replace stack

    /*
     * Save the inserted text for later redo with ^@ and CTRL-A.
     * Don't do it when "restart_edit" was set and nothing was inserted,
     * otherwise CTRL-O w and then <Left> will clear "last_insert".
     */
    inserted = get_inserted();
    صحيح added = inserted.string == عدم ? 0 : (صحيح)inserted.لمائی - new_داخل_کرو_skip;
    إذا (did_restart_edit == 0 || added > 0)
    {
	vim_free(آخری_داخل_کرو.string);
	آخری_داخل_کرو = inserted;			    // structure copy
	آخری_داخل_کرو_skip = added < 0 ? 0 : new_داخل_کرو_skip;
    }
    وإلا
	vim_free(inserted.string);

    إذا (!arrow_ورتیا && سرا_داخل_کرو_pos != عدم)
    {
	// Auto-format now.  It may seem strange to do this when stopping an
	// insertion (or moving the cursor), but it's required when appending
	// a line and having it end in a space.  But only do it when something
	// was actually inserted, otherwise undo won't work.
	إذا (!ins_need_undo && has_format_option(FO_AUTO))
	{
	    pos_T   tpos = curwin->w_cursor;

	    // When the cursor is at the end of the line after a space the
	    // formatting will move it to the following word.  Avoid that by
	    // moving the cursor onto the space.
	    cc = 'x';
	    إذا (curwin->w_cursor.خانہ > 0 && gchar_cursor() == NUL)
	    {
		dec_cursor();
		cc = gchar_cursor();
		إذا (!VIM_ISWHITE(cc))
		    curwin->w_cursor = tpos;
	    }

	    auto_format(TRUE, FALSE);

	    إذا (VIM_ISWHITE(cc))
	    {
		إذا (gchar_cursor() != NUL)
		    inc_cursor();
		// If the cursor is still at the same character, also keep
		// the "coladd".
		إذا (gchar_cursor() == NUL
			&& curwin->w_cursor.lnum == tpos.lnum
			&& curwin->w_cursor.خانہ == tpos.خانہ)
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
	إذا (!nomove && did_ai && (esc || (vim_strchr(p_cpo, CPO_INDENT) == عدم
			&& curwin->w_cursor.lnum != سرا_داخل_کرو_pos->lnum))
		&& سرا_داخل_کرو_pos->lnum <= curbuf->b_ml.ml_سطر_گنتی)
	{
	    pos_T	tpos = curwin->w_cursor;
	    colnr_T	prev_خانہ = سرا_داخل_کرو_pos->خانہ;
	    colnr_T	strip_خانہ;

	    curwin->w_cursor = *سرا_داخل_کرو_pos;
	    check_cursor_خانہ();  // make sure it is not past the line

	    // Where the loop would actually start (back up if on NUL).
	    strip_خانہ = curwin->w_cursor.خانہ;
	    إذا (gchar_cursor() == NUL && strip_خانہ > 0)
		--strip_خانہ;

	    // Don't strip if non-whitespace follows: setline() from a <Cmd>
	    // mapping or CursorHoldI autocmd may have inserted content.
	    إذا (*skipwhite(ml_get_curline() + strip_خانہ) == NUL)
	    {
		curwin->w_cursor.خانہ = strip_خانہ;
		لكل (;;)
		{
		    إذا (gchar_cursor() == NUL && curwin->w_cursor.خانہ > 0)
			--curwin->w_cursor.خانہ;
		    cc = gchar_cursor();
		    إذا (!VIM_ISWHITE(cc))
			اكسر;
		    إذا (del_char(TRUE) == FAIL)
			اكسر;  // should not happen
		}
		إذا (curwin->w_cursor.lnum != tpos.lnum)
		    curwin->w_cursor = tpos;
		وإلا إذا (curwin->w_cursor.خانہ < prev_خانہ)
		{
		    // reset tpos, could have been invalidated in the loop above
		    tpos = curwin->w_cursor;
		    tpos.خانہ++;
		    إذا (cc != NUL && gchar_pos(&tpos) == NUL)
			++curwin->w_cursor.خانہ;	// put cursor back on the NUL
		}

		// <C-S-Right> may have started Visual mode, adjust the position for
		// deleted characters.
		إذا (VIsual_سرگرم)
		    check_visual_pos();
	    }
	    وإلا
		// Non-whitespace follows, keep original cursor.
		curwin->w_cursor = tpos;
	}
    }
    did_ai = FALSE;
    did_si = FALSE;
    can_si = FALSE;
    can_si_پٹھ = FALSE;

    // Set '[ and '] to the inserted text.  When end_insert_pos is NULL we are
    // now in a different buffer.
    إذا (سرا_داخل_کرو_pos != عدم)
    {
	curbuf->b_op_شروع = Insstart;
	curbuf->b_op_شروع_orig = Insstart_orig;
	curbuf->b_op_سرا = *سرا_داخل_کرو_pos;
    }
}

/*
 * Set the last inserted text to a single character.
 * Used for the replace command.
 */
    فراغ
سیٹ_آخری_داخل_کرو(صحيح c)
{
    char_u	*s;

    vim_free(آخری_داخل_کرو.string);
    آخری_داخل_کرو.string = alloc(MB_MAXBYTES * 3 + 5);
    إذا (آخری_داخل_کرو.string == عدم)
    {
	آخری_داخل_کرو.لمائی = 0;
	أرجع;
    }

    s = آخری_داخل_کرو.string;
    // Use the CTRL-V only when entering a special char
    إذا (c < ' ' || c == DEL)
	*s++ = Ctrl_V;
    s = جوڑو_char2buf(c, s);
    *s++ = ESC;
    *s = NUL;
    آخری_داخل_کرو.لمائی = (size_t)(s - آخری_داخل_کرو.string);
    آخری_داخل_کرو_skip = 0;
}

#إذا defined(EXITFREE)
    فراغ
free_آخری_داخل_کرو(فراغ)
{
    VIM_CLEAR_STRING(آخری_داخل_کرو);
}
#جے_انت

/*
 * Add character "c" to buffer "s".  Escape the special meaning of K_SPECIAL
 * and CSI.  Handle multi-byte characters.
 * Returns a pointer to after the added bytes.
 */
    char_u *
جوڑو_char2buf(صحيح c, char_u *s)
{
    char_u	عارضی[MB_MAXBYTES + 1];
    صحيح		i;
    صحيح		len;

    len = (*mb_char2bytes)(c, عارضی);
    لكل (i = 0; i < len; ++i)
    {
	c = عارضی[i];
	// Need to escape K_SPECIAL and CSI like in the typeahead buffer.
	إذا (c == K_SPECIAL)
	{
	    *s++ = K_SPECIAL;
	    *s++ = KS_SPECIAL;
	    *s++ = KE_FILLER;
	}
#جے_تعریف_شدہ FEAT_GUI
	وإلا إذا (c == CSI)
	{
	    *s++ = CSI;
	    *s++ = KS_EXTRA;
	    *s++ = (صحيح)KE_CSI;
	}
#جے_انت
	وإلا
	    *s++ = c;
    }
    أرجع s;
}

/*
 * move cursor to start of line
 * if flags & BL_WHITE	move to first non-white
 * if flags & BL_SOL	move to first non-white if startofline is set,
 *			    otherwise keep "curswant" column
 * if flags & BL_FIX	don't leave the cursor on a NUL.
 */
    فراغ
beginline(صحيح flags)
{
    إذا ((flags & BL_SOL) && !p_sol)
	coladvance(curwin->w_curswant);
    وإلا
    {
	curwin->w_cursor.خانہ = 0;
	curwin->w_cursor.coladd = 0;

	إذا (flags & (BL_WHITE | BL_SOL))
	{
	    char_u  *ptr;

	    لكل (ptr = ml_get_curline(); VIM_ISWHITE(*ptr)
			       && !((flags & BL_FIX) && ptr[1] == NUL); ++ptr)
		++curwin->w_cursor.خانہ;
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

    صحيح
oneright(فراغ)
{
    char_u	*ptr;
    صحيح		l;

    إذا (virtual_سرگرم())
    {
	pos_T	prevpos = curwin->w_cursor;

	// Adjust for multi-wide char (excluding TAB)
	ptr = ml_get_cursor();
	coladvance(getviscol() + ((*ptr != TAB
					  && vim_isprintc((*mb_ptr2char)(ptr)))
		    ? ptr2cells(ptr) : 1));
	curwin->w_سیٹ_curswant = true;
	// Return OK if the cursor moved, FAIL otherwise (at window edge).
	أرجع (prevpos.خانہ != curwin->w_cursor.خانہ
		    || prevpos.coladd != curwin->w_cursor.coladd) ? OK : FAIL;
    }

    ptr = ml_get_cursor();
    إذا (*ptr == NUL)
	أرجع FAIL;	    // already at the very end

    إذا (has_mbyte)
	l = (*mb_ptr2len)(ptr);
    وإلا
	l = 1;

    // move "l" bytes right, but don't end up on the NUL, unless 'virtualedit'
    // contains "onemore".
    إذا (ptr[l] == NUL && (get_ve_flags() & VE_ONEMORE) == 0)
	أرجع FAIL;
    curwin->w_cursor.خانہ += l;

    curwin->w_سیٹ_curswant = true;
    adjust_skipcol();
    أرجع OK;
}

    صحيح
oneleft(فراغ)
{
    إذا (virtual_سرگرم())
    {
#جے_تعریف_شدہ FEAT_LINEBREAK
	صحيح چوڑائی;
#جے_انت
	صحيح v = getviscol();

	إذا (v == 0)
	    أرجع FAIL;

#جے_تعریف_شدہ FEAT_LINEBREAK
	// We might get stuck on 'showbreak', skip over it.
	چوڑائی = 1;
	لكل (;;)
	{
	    coladvance(v - چوڑائی);
	    // getviscol() is slow, skip it when 'showbreak' is empty,
	    // 'breakindent' is not set and there are no multi-byte
	    // characters
	    إذا ((*get_showbreak_قدر(curwin) == NUL && !curwin->w_p_bri
					     && !has_mbyte) || getviscol() < v)
		اكسر;
	    ++چوڑائی;
	}
#وإلا
	coladvance(v - 1);
#جے_انت

	إذا (curwin->w_cursor.coladd == 1)
	{
	    char_u *ptr;

	    // Adjust for multi-wide char (not a TAB)
	    ptr = ml_get_cursor();
	    إذا (*ptr != TAB && vim_isprintc((*mb_ptr2char)(ptr))
							 && ptr2cells(ptr) > 1)
		curwin->w_cursor.coladd = 0;
	}

	curwin->w_سیٹ_curswant = true;
	adjust_skipcol();
	أرجع OK;
    }

    إذا (curwin->w_cursor.خانہ == 0)
	أرجع FAIL;

    curwin->w_سیٹ_curswant = true;
    --curwin->w_cursor.خانہ;

    // if the character on the left of the current cursor is a multi-byte
    // character, move to its first byte
    إذا (has_mbyte)
	mb_adjust_cursor();
    adjust_skipcol();
    أرجع OK;
}

/*
 * Move the cursor up "n" lines in window "wp".
 * Takes care of closed folds.
 */
    فراغ
cursor_اتے_inner(win_T *wp, لما n)
{
    linenr_T	lnum = wp->w_cursor.lnum;

    إذا (n >= lnum)
	lnum = 1;
    وإلا
#جے_تعریف_شدہ FEAT_FOLDING
	إذا (hasAnyFolding(wp))
    {
	/*
	 * Count each sequence of folded lines as one logical line.
	 */
	// go to the start of the current fold
	(فراغ)hasFoldingWin(wp, lnum, &lnum, عدم, TRUE, عدم);

	طالما (n--)
	{
	    // move up one line
	    --lnum;
	    إذا (lnum <= 1)
		اكسر;
	    // If we entered a fold, move to the beginning, unless in
	    // Insert mode or when 'foldopen' contains "all": it will open
	    // in a moment.
	    إذا (n > 0 || !((State & منوال_داخل_کرو) || (fdo_flags & FDO_ALL)))
		(فراغ)hasFoldingWin(wp, lnum, &lnum, عدم, TRUE, عدم);
	}
	إذا (lnum < 1)
	    lnum = 1;
    }
    وإلا
#جے_انت
	lnum -= n;

    wp->w_cursor.lnum = lnum;
}

    صحيح
cursor_اتے(
    لما	n,
    صحيح		upd_topline)	    // When TRUE: update topline
{
    // This fails if the cursor is already in the first line or the count is
    // larger than the line number and '-' is in 'cpoptions'
    linenr_T lnum = curwin->w_cursor.lnum;
    إذا (n > 0 && (lnum <= 1
		       || (n >= lnum && vim_strchr(p_cpo, CPO_MINUS) != عدم)))
	أرجع FAIL;
    cursor_اتے_inner(curwin, n);

    // try to advance to the column we want to be at
    coladvance(curwin->w_curswant);

    إذا (upd_topline)
	update_topline();	// make sure curwin->w_topline is valid

    أرجع OK;
}

/*
 * Move the cursor down "n" lines in window "wp".
 * Takes care of closed folds.
 */
    فراغ
cursor_تھلے_inner(win_T *wp, لما n)
{
    linenr_T	lnum = wp->w_cursor.lnum;
    linenr_T	سطر_گنتی = wp->w_بفر->b_ml.ml_سطر_گنتی;

    إذا (lnum + n >= سطر_گنتی)
	lnum = سطر_گنتی;
    وإلا
#جے_تعریف_شدہ FEAT_FOLDING
	إذا (hasAnyFolding(wp))
    {
	linenr_T	آخری;

	// count each sequence of folded lines as one logical line
	طالما (n--)
	{
	    إذا (hasFoldingWin(wp, lnum, عدم, &آخری, TRUE, عدم))
		lnum = آخری + 1;
	    وإلا
		++lnum;
	    إذا (lnum >= سطر_گنتی)
		اكسر;
	}
	إذا (lnum > سطر_گنتی)
	    lnum = سطر_گنتی;
    }
    وإلا
#جے_انت
	lnum += n;

    wp->w_cursor.lnum = lnum;
}

/*
 * Cursor down a number of logical lines.
 */
    صحيح
cursor_تھلے(
    لما	n,
    صحيح		upd_topline)	    // When TRUE: update topline
{
    linenr_T	lnum = curwin->w_cursor.lnum;
    linenr_T	سطر_گنتی = curwin->w_بفر->b_ml.ml_سطر_گنتی;
    // This fails if the cursor is already in the last (folded) line, or would
    // move beyond the last line and '-' is in 'cpoptions'.
#جے_تعریف_شدہ FEAT_FOLDING
    hasFoldingWin(curwin, lnum, عدم, &lnum, TRUE, عدم);
#جے_انت
    إذا (n > 0
	    && (lnum >= سطر_گنتی
		|| (lnum + n > سطر_گنتی
				     && vim_strchr(p_cpo, CPO_MINUS) != عدم)))
	أرجع FAIL;
    cursor_تھلے_inner(curwin, n);

    // try to advance to the column we want to be at
    coladvance(curwin->w_curswant);

    إذا (upd_topline)
	update_topline();	// make sure curwin->w_topline is valid

    أرجع OK;
}

/*
 * Stuff the last inserted text in the read buffer.
 * Last_insert actually is a copy of the redo buffer, so we
 * first have to remove the command.
 */
    صحيح
stuff_inserted(
    صحيح	    c,		// Command character to be inserted
    لما    گنتی,	// Repeat this many times
    صحيح	    no_esc)	// Don't add an ESC at the end
{
    string_T	داخل_کرو;				// text to be inserted
    char_u	آخری = ' ';

    داخل_کرو = get_آخری_داخل_کرو();
    إذا (داخل_کرو.string == عدم)
    {
	emsg(_(e_no_inserted_متن_yet));
	أرجع FAIL;
    }

    // may want to stuff the command character, to start Insert mode
    إذا (c != NUL)
	stuffcharReadbuff(c);

    إذا (داخل_کرو.لمائی > 0)
    {
	char_u	*p;

	// look for the last ESC in 'insert'
	لكل (p = داخل_کرو.string + داخل_کرو.لمائی - 1; p >= داخل_کرو.string; --p)
	{
	    إذا (*p == ESC)
	    {
		داخل_کرو.لمائی = (size_t)(p - داخل_کرو.string);
		اكسر;
	    }
	}
    }

    إذا (داخل_کرو.لمائی > 0)
    {
	char_u	*p = داخل_کرو.string + داخل_کرو.لمائی - 1;

	// when the last char is either "0" or "^" it will be quoted if no ESC
	// comes after it OR if it will insert more than once and "ptr"
	// starts with ^D.	-- Acevedo
	إذا ((*p == '0' || *p == '^')
		&& (no_esc || (*داخل_کرو.string == Ctrl_D && گنتی > 1)))
	{
	    آخری = *p;
	    --داخل_کرو.لمائی;
	}
    }

    افعل
    {
	stuffReadbuffLen(داخل_کرو.string, (لما)داخل_کرو.لمائی);
	// a trailing "0" is inserted as "<C-V>048", "^" as "<C-V>^"
	بدل (آخری)
	{
	حالة '0':
#تعریف_کرو متن_منزل_داخل_کرو "\026\060\064\070"
	    stuffReadbuffLen((char_u *)متن_منزل_داخل_کرو, STRLEN_LITERAL(متن_منزل_داخل_کرو));
#تعریف_رد متن_منزل_داخل_کرو
	    اكسر;

	حالة '^':
#تعریف_کرو متن_منزل_داخل_کرو "\026^"
	    stuffReadbuffLen((char_u *)متن_منزل_داخل_کرو, STRLEN_LITERAL(متن_منزل_داخل_کرو));
#تعریف_رد متن_منزل_داخل_کرو
	    اكسر;

	افتراضي:
	    اكسر;
	}
    }
    طالما (--گنتی > 0);

    // may want to stuff a trailing ESC, to get out of Insert mode
    إذا (!no_esc)
	stuffcharReadbuff(ESC);

    أرجع OK;
}

    string_T
get_آخری_داخل_کرو(فراغ)
{
    string_T داخل_کرو = {عدم, 0};

    إذا (آخری_داخل_کرو.string != عدم)
    {
	داخل_کرو.string = آخری_داخل_کرو.string + آخری_داخل_کرو_skip;
	داخل_کرو.لمائی = (size_t)(آخری_داخل_کرو.لمائی - آخری_داخل_کرو_skip);
    }

    أرجع داخل_کرو;
}

/*
 * Get last inserted string, and remove trailing <Esc>.
 * Returns pointer to allocated memory (must be freed) or NULL.
 */
    char_u *
get_آخری_داخل_کرو_save(فراغ)
{
    string_T	داخل_کرو = get_آخری_داخل_کرو();
    char_u	*s;

    إذا (داخل_کرو.string == عدم)
	أرجع عدم;
    s = vim_strnsave(داخل_کرو.string, داخل_کرو.لمائی);
    إذا (s == عدم)
	أرجع عدم;

    إذا (داخل_کرو.لمائی > 0 && s[داخل_کرو.لمائی - 1] == ESC)	// remove trailing ESC
	s[--داخل_کرو.لمائی] = NUL;
    أرجع s;
}

/*
 * Check the word in front of the cursor for an abbreviation.
 * Called when the non-id character "c" has been entered.
 * When an abbreviation is recognized it is removed from the text and
 * the replacement string is inserted in typebuf.tb_buf[], followed by "c".
 */
    ساكن صحيح
echeck_abbr(صحيح c)
{
    // Don't check for abbreviation in paste mode, when disabled and just
    // after moving around with cursor keys.
    إذا (p_paste || no_abbr || arrow_ورتیا)
	أرجع FALSE;

    أرجع check_abbr(c, ml_get_curline(), curwin->w_cursor.خانہ,
		curwin->w_cursor.lnum == Insstart.lnum ? Insstart.خانہ : 0);
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

ساكن char_u	*replace_ڈھیر = عدم;
ساكن لما	replace_ڈھیر_nr = 0;	    // next entry in replace stack
ساكن لما	replace_ڈھیر_len = 0;	    // max. number of entries

    فراغ
replace_ڈالو(
    صحيح	    c)	    // character that is replaced (NUL is none)
{
    char_u  *p;

    إذا (replace_ڈھیر_nr < replace_وتھ)	// nothing to do
	أرجع;
    إذا (replace_ڈھیر_len <= replace_ڈھیر_nr)
    {
	replace_ڈھیر_len += 50;
	p = ALLOC_MULT(char_u, replace_ڈھیر_len);
	إذا (p == عدم)	    // out of memory
	{
	    replace_ڈھیر_len -= 50;
	    أرجع;
	}
	إذا (replace_ڈھیر != عدم)
	{
	    mch_memmove(p, replace_ڈھیر,
				 (size_t)(replace_ڈھیر_nr * حجم(char_u)));
	    vim_free(replace_ڈھیر);
	}
	replace_ڈھیر = p;
    }
    p = replace_ڈھیر + replace_ڈھیر_nr - replace_وتھ;
    إذا (replace_وتھ)
	mch_memmove(p + 1, p, (size_t)(replace_وتھ * حجم(char_u)));
    *p = c;
    ++replace_ڈھیر_nr;
}

/*
 * Push a character onto the replace stack.  Handles a multi-byte character in
 * reverse byte order, so that the first byte is popped off first.
 * Return the number of bytes done (includes composing characters).
 */
    صحيح
replace_ڈالو_mb(char_u *p)
{
    صحيح l = (*mb_ptr2len)(p);
    صحيح j;

    لكل (j = l - 1; j >= 0; --j)
	replace_ڈالو(p[j]);
    أرجع l;
}

/*
 * Pop one item from the replace stack.
 * return -1 if stack empty
 * return replaced character or NUL otherwise
 */
    ساكن صحيح
replace_نکالو(فراغ)
{
    إذا (replace_ڈھیر_nr == 0)
	أرجع -1;
    أرجع (صحيح)replace_ڈھیر[--replace_ڈھیر_nr];
}

/*
 * Join the top two items on the replace stack.  This removes to "off"'th NUL
 * encountered.
 */
    فراغ
replace_join(
    صحيح	    off)	// offset for which NUL to remove
{
    صحيح	    i;

    لكل (i = replace_ڈھیر_nr; --i >= 0; )
	إذا (replace_ڈھیر[i] == NUL && off-- <= 0)
	{
	    --replace_ڈھیر_nr;
	    mch_memmove(replace_ڈھیر + i, replace_ڈھیر + i + 1,
					      (size_t)(replace_ڈھیر_nr - i));
	    أرجع;
	}
}

/*
 * Pop bytes from the replace stack until a NUL is found, and insert them
 * before the cursor.  Can only be used in MODE_REPLACE or MODE_VREPLACE state.
 */
    ساكن فراغ
replace_نکالو_ins(فراغ)
{
    صحيح	    cc;
    صحيح	    oldState = State;

    State = منوال_عام;			// don't want MODE_REPLACE here
    طالما ((cc = replace_نکالو()) > 0)
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
    ساكن فراغ
mb_replace_نکالو_ins(صحيح cc)
{
    صحيح		n;
    char_u	buf[MB_MAXBYTES + 1];
    صحيح		i;
    صحيح		c;

    إذا (has_mbyte && (n = MB_BYTE2LEN(cc)) > 1)
    {
	buf[0] = cc;
	لكل (i = 1; i < n; ++i)
	    buf[i] = replace_نکالو();
	ins_بائٹاں_len(buf, n);
    }
    وإلا
	ins_char(cc);

    إذا (enc_utf8)
	// Handle composing chars.
	لكل (;;)
	{
	    c = replace_نکالو();
	    إذا (c == -1)	    // stack empty
		اكسر;
	    إذا ((n = MB_BYTE2LEN(c)) == 1)
	    {
		// Not a multi-byte char, put it back.
		replace_ڈالو(c);
		اكسر;
	    }

	    buf[0] = c;
	    لكل (i = 1; i < n; ++i)
		buf[i] = replace_نکالو();
	    إذا (utf_iscomposing(utf_ptr2char(buf)))
		ins_بائٹاں_len(buf, n);
	    وإلا
	    {
		// Not a composing char, put it back.
		لكل (i = n - 1; i >= 0; --i)
		    replace_ڈالو(buf[i]);
		اكسر;
	    }

	}
}

/*
 * make the replace stack empty
 * (called when exiting replace mode)
 */
    ساكن فراغ
replace_flush(فراغ)
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
    ساكن فراغ
replace_do_bs(صحيح حد_بندی_خانہ)
{
    صحيح		cc;
    صحيح		orig_len = 0;
    صحيح		ins_len;
    صحيح		orig_vcols = 0;
    colnr_T	شروع_vcol;
    char_u	*p;
    صحيح		i;
    صحيح		vcol;

    cc = replace_نکالو();
    إذا (cc > 0)
    {
#جے_تعریف_شدہ FEAT_PROP_POPUP
	size_t	len_before = 0;  // init to shut up GCC

	إذا (curbuf->b_has_textprop)
	{
	    // Do not adjust text properties for individual delete and insert
	    // operations, do it afterwards on the resulting text.
	    len_before = ml_get_curline_len();
	    ++متن_prop_frozen;
	}
#جے_انت
	إذا (State & VREPLACE_FLAG)
	{
	    // Get the number of screen cells used by the character we are
	    // going to delete.
	    getvcol(curwin, &curwin->w_cursor, عدم, &شروع_vcol, عدم, 0);
	    orig_vcols = chartabsize(ml_get_cursor(), شروع_vcol);
	}
	إذا (has_mbyte)
	{
	    (فراغ)del_char_after_خانہ(حد_بندی_خانہ);
	    إذا (State & VREPLACE_FLAG)
		orig_len = ml_get_cursor_len();
	    replace_ڈالو(cc);
	}
	وإلا
	{
	    pchar_cursor(cc);
	    إذا (State & VREPLACE_FLAG)
		orig_len = ml_get_cursor_len() - 1;
	}
	replace_نکالو_ins();

	إذا (State & VREPLACE_FLAG)
	{
	    // Get the number of screen cells used by the inserted characters
	    p = ml_get_cursor();
	    ins_len = ml_get_cursor_len() - orig_len;
	    vcol = شروع_vcol;
	    لكل (i = 0; i < ins_len; ++i)
	    {
		vcol += chartabsize(p + i, vcol);
		i += (*mb_ptr2len)(p + i) - 1;
	    }
	    vcol -= شروع_vcol;

	    // Delete spaces that were inserted after the cursor to keep the
	    // text aligned.
	    curwin->w_cursor.خانہ += ins_len;
	    طالما (vcol > orig_vcols && gchar_cursor() == ' ')
	    {
		del_char(FALSE);
		++orig_vcols;
	    }
	    curwin->w_cursor.خانہ -= ins_len;
	}

	// mark the buffer as changed and prepare for displaying
	بدلیا_بائٹاں(curwin->w_cursor.lnum, curwin->w_cursor.خانہ);

#جے_تعریف_شدہ FEAT_PROP_POPUP
	إذا (curbuf->b_has_textprop)
	{
	    size_t len_now = ml_get_curline_len();

	    --متن_prop_frozen;
	    adjust_prop_columns(curwin->w_cursor.lnum, curwin->w_cursor.خانہ,
					   (صحيح)(len_now - len_before), 0);
	}
#جے_انت
    }
    وإلا إذا (cc == 0)
	(فراغ)del_char_after_خانہ(حد_بندی_خانہ);
}

#إذا defined(FEAT_RIGHTLEFT)
/*
 * Map Hebrew keyboard when in hkmap mode.
 */
    صحيح
hkmap(صحيح c)
{
    إذا (p_hkmapp)   // phonetic mapping, by Ilya Dogolazky
    {
	تعداد {hALEF=0, BET, GIMEL, DALET, HEI, VAV, ZAIN, HET, TET, IUD,
	    KAFsofit, hKAF, LAMED, MEMsofit, MEM, NUNsofit, NUN, SAMEH, AIN,
	    PEIsofit, PEI, ZADIsofit, ZADI, KOF, RESH, hSHIN, TAV};
	ساكن char_u map[26] =
	    {(char_u)hALEF/*a*/, (char_u)BET  /*b*/, (char_u)hKAF    /*c*/,
	     (char_u)DALET/*d*/, (char_u)-1   /*e*/, (char_u)PEIsofit/*f*/,
	     (char_u)GIMEL/*g*/, (char_u)HEI  /*h*/, (char_u)IUD     /*i*/,
	     (char_u)HET  /*j*/, (char_u)KOF  /*k*/, (char_u)LAMED   /*l*/,
	     (char_u)MEM  /*m*/, (char_u)NUN  /*n*/, (char_u)SAMEH   /*o*/,
	     (char_u)PEI  /*p*/, (char_u)-1   /*q*/, (char_u)RESH    /*r*/,
	     (char_u)ZAIN /*s*/, (char_u)TAV  /*t*/, (char_u)TET     /*u*/,
	     (char_u)VAV  /*v*/, (char_u)hSHIN/*w*/, (char_u)-1      /*x*/,
	     (char_u)AIN  /*y*/, (char_u)ZADI /*z*/};

	إذا (c == 'N' || c == 'M' || c == 'P' || c == 'C' || c == 'Z')
	    أرجع (صحيح)(map[CharOrd(c)] - 1 + p_aleph);
							    // '-1'='sofit'
	وإلا إذا (c == 'x')
	    أرجع 'X';
	وإلا إذا (c == 'q')
	    أرجع '\''; // {geresh}={'}
	وإلا إذا (c == 246)
	    أرجع ' ';  // \"o --> ' ' for a german keyboard
	وإلا إذا (c == 228)
	    أرجع ' ';  // \"a --> ' '      -- / --
	وإلا إذا (c == 252)
	    أرجع ' ';  // \"u --> ' '      -- / --
	// NOTE: islower() does not do the right thing for us on Linux so we
	// do this the same was as 5.7 and previous, so it works correctly on
	// all systems.  Specifically, the e.g. Delete and Arrow keys are
	// munged and won't work if e.g. searching for Hebrew text.
	وإلا إذا (c >= 'a' && c <= 'z')
	    أرجع (صحيح)(map[Char_Ord_کم(c)] + p_aleph);
	وإلا
	    أرجع c;
    }
    وإلا
    {
	بدل (c)
	{
	    حالة '`':	أرجع ';';
	    حالة '/':	أرجع '.';
	    حالة '\'':	أرجع ',';
	    حالة 'q':	أرجع '/';
	    حالة 'w':	أرجع '\'';

			// Hebrew letters - set offset from 'a'
	    حالة ',':	c = '{'; اكسر;
	    حالة '.':	c = 'v'; اكسر;
	    حالة ';':	c = 't'; اكسر;
	    افتراضي: {
			 ساكن حرف str[] = "zqbcxlsjphmkwonu ydafe rig";

			 إذا (c < 'a' || c > 'z')
			     أرجع c;
			 c = str[Char_Ord_کم(c)];
			 اكسر;
		     }
	}

	أرجع (صحيح)(Char_Ord_کم(c) + p_aleph);
    }
}
#جے_انت

    ساكن فراغ
ins_reg(فراغ)
{
    صحيح		need_redraw = FALSE;
    صحيح		regname;
    صحيح		literally = 0;
    صحيح		vis_سرگرم = VIsual_سرگرم;

    /*
     * If we are going to wait for a character, show a '"'.
     */
    pc_حالت = PC_حالت_UNSET;
    إذا (redrawing() && !char_avail())
    {
	// may need to redraw when no more chars available now
	ins_redraw(FALSE);

	edit_putchar('"', TRUE);
	جوڑو_منزل_showcmd_c(Ctrl_R);
    }

#جے_تعریف_شدہ USE_ON_FLY_SCROLL
    dont_scroll = TRUE;		// disallow scrolling here
#جے_انت

    /*
     * Don't map the register name. This also prevents the mode message to be
     * deleted when ESC is hit.
     */
    ++no_mapping;
    ++allow_کلیدیں;
    regname = plain_vgetc();
    LANGMAP_ADJUST(regname, TRUE);
    إذا (regname == Ctrl_R || regname == Ctrl_O || regname == Ctrl_P)
    {
	// Get a third key for literal register insertion
	literally = regname;
	جوڑو_منزل_showcmd_c(literally);
	regname = plain_vgetc();
	LANGMAP_ADJUST(regname, TRUE);
    }
    --no_mapping;
    --allow_کلیدیں;

#جے_تعریف_شدہ FEAT_EVAL
    // Don't call u_sync() while typing the expression or giving an error
    // message for it. Only call it explicitly.
    ++no_u_sync;
    إذا (regname == '=')
    {
	pos_T	curpos = curwin->w_cursor;
# ifdef HAVE_داخلہ_طریقہ
	صحيح	im_on = im_get_حالت();
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
	إذا (im_on)
	    im_سیٹ_سرگرم(TRUE);
# endif
    }
    إذا (regname == NUL || !valid_yank_reg(regname, FALSE))
    {
	vim_beep(BO_REG);
	need_redraw = TRUE;	// remove the '"'
    }
    وإلا
    {
#جے_انت
	إذا (literally == Ctrl_O || literally == Ctrl_P)
	{
	    // Append the command to the redo buffer.
	    Append_Char_منزل_Redobuff(Ctrl_R);
	    Append_Char_منزل_Redobuff(literally);
	    Append_Char_منزل_Redobuff(regname);

	    do_put(regname, عدم, BACKWARD, 1L,
		 (literally == Ctrl_P ? PUT_FIXINDENT : 0) | PUT_CURSEND);
	}
	وإلا إذا (داخل_کرو_reg(regname, literally) == FAIL)
	{
	    vim_beep(BO_REG);
	    need_redraw = TRUE;	// remove the '"'
	}
	وإلا إذا (stop_داخل_کرو_منوال)
	    // When the '=' register was used and a function was invoked that
	    // did ":stopinsert" then stuff_empty() returns FALSE but we won't
	    // insert anything, need to remove the '"'
	    need_redraw = TRUE;

#جے_تعریف_شدہ FEAT_EVAL
    }
    --no_u_sync;
    إذا (u_sync_once == 1)
	ins_need_undo = TRUE;
    u_sync_once = 0;
#جے_انت
    clear_showcmd();

    // If the inserted register is empty, we need to remove the '"'
    إذا (need_redraw || stuff_خالی_تھاواں())
	edit_unputchar();

    // Disallow starting Visual mode here, would get a weird mode.
    إذا (!vis_سرگرم && VIsual_سرگرم)
	سرا_visual_منوال();
}

/*
 * CTRL-G commands in Insert mode.
 */
    ساكن فراغ
ins_ctrl_g(فراغ)
{
    صحيح		c;

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
    بدل (c)
    {
	// CTRL-G k and CTRL-G <Up>: cursor up to Insstart.col
	حالة K_اتے:
	حالة Ctrl_K:
	حالة 'k': ins_اتے(TRUE);
		  اكسر;

	// CTRL-G j and CTRL-G <Down>: cursor down to Insstart.col
	حالة K_تھلے:
	حالة Ctrl_J:
	حالة 'j': ins_تھلے(TRUE);
		  اكسر;

	// CTRL-G u: start new undoable edit
	حالة 'u': u_sync(TRUE);
		  ins_need_undo = TRUE;

		  // Need to reset Insstart, esp. because a BS that joins
		  // a line to the previous one must save for undo.
		  update_Insstart_orig = FALSE;
		  Insstart = curwin->w_cursor;
		  اكسر;

	// CTRL-G U: do not break undo with the next char
	حالة 'U':
		  // Allow one left/right cursor movement with the next char,
		  // without breaking undo.
		  dont_sync_undo = MAYBE;
		  اكسر;

	حالة ESC:
		  // Esc after CTRL-G cancels it.
		  اكسر;

	// Unknown CTRL-G command, reserved for future expansion.
	افتراضي:  vim_beep(BO_CTRLG);
    }
}

/*
 * CTRL-^ in Insert mode.
 */
    ساكن فراغ
ins_ctrl_hat(فراغ)
{
    إذا (map_منزل_exists_منوال((char_u *)"", منوال_LANGMAP, FALSE))
    {
	// ":lmap" mappings exists, Toggle use of ":lmap" mappings.
	إذا (State & منوال_LANGMAP)
	{
	    curbuf->b_p_iminsert = B_IMODE_NONE;
	    State &= ~منوال_LANGMAP;
	}
	وإلا
	{
	    curbuf->b_p_iminsert = B_IMODE_LMAP;
	    State |= منوال_LANGMAP;
#جے_تعریف_شدہ HAVE_داخلہ_طریقہ
	    im_سیٹ_سرگرم(FALSE);
#جے_انت
	}
    }
#جے_تعریف_شدہ HAVE_داخلہ_طریقہ
    وإلا
    {
	// There are no ":lmap" mappings, toggle IM
	إذا (im_get_حالت())
	{
	    curbuf->b_p_iminsert = B_IMODE_NONE;
	    im_سیٹ_سرگرم(FALSE);
	}
	وإلا
	{
	    curbuf->b_p_iminsert = B_IMODE_IM;
	    State &= ~منوال_LANGMAP;
	    im_سیٹ_سرگرم(TRUE);
	}
    }
#جے_انت
    سیٹ_iminsert_global();
    showmode();
#جے_تعریف_شدہ FEAT_GUI
    // may show different cursor shape or color
    إذا (gui.وچ_use)
	gui_update_cursor(TRUE, FALSE);
#جے_انت
#إذا defined(FEAT_KEYMAP)
    // Show/unshow value of 'keymap' in status lines.
    حالت_redraw_curbuf();
#جے_انت
}

/*
 * Handle ESC in insert mode.
 * Returns TRUE when leaving insert mode, FALSE when going to repeat the
 * insert.
 */
    ساكن صحيح
ins_esc(
    لما	*گنتی,
    صحيح		cmdchar,
    صحيح		nomove)	    // don't move cursor
{
    صحيح		عارضی;
    ساكن صحيح	disabled_redraw = FALSE;
#جے_تعریف_شدہ FEAT_CONCEAL
    // Remember if the cursor line was concealed before changing State.
    صحيح		cursor_سطر_was_concealed = curwin->w_p_cole > 0
						&& conceal_cursor_سطر(curwin);
#جے_انت

#جے_تعریف_شدہ FEAT_SPELL
    check_spell_redraw();
#جے_انت

    عارضی = curwin->w_cursor.خانہ;
    إذا (disabled_redraw)
    {
	إذا (RedrawingDisabled > 0)
	    --RedrawingDisabled;
	disabled_redraw = FALSE;
    }
    إذا (!arrow_ورتیا)
    {
	/*
	 * Don't append the ESC for "r<CR>" and "grx".
	 * When 'insertmode' is set only CTRL-L stops Insert mode.  Needed for
	 * when "count" is non-zero.
	 */
	إذا (cmdchar != 'r' && cmdchar != 'v')
	    Append_منزل_Redobuff(p_im ? (char_u *)"\014" : ESC_STR);

	/*
	 * Repeating insert may take a long time.  Check for
	 * interrupt now and then.
	 */
	إذا (*گنتی > 0)
	{
	    سطر_breakcheck();
	    إذا (got_int)
		*گنتی = 0;
	}

	إذا (--*گنتی > 0)	// repeat what was typed
	{
	    // Vi repeats the insert without replacing characters.
	    إذا (vim_strchr(p_cpo, CPO_REPLCNT) != عدم)
		State &= ~REPLACE_FLAG;

	    (فراغ)شروع_redo_ins();
	    إذا (cmdchar == 'r' || cmdchar == 'v')
		stuffRedoReadbuff(ESC_STR);	// no ESC in redo buffer
	    ++RedrawingDisabled;
	    disabled_redraw = TRUE;
	    أرجع FALSE;	// repeat the insert
	}
	stop_داخل_کرو(&curwin->w_cursor, TRUE, nomove);
	undisplay_dollar();
    }

    إذا (cmdchar != 'r' && cmdchar != 'v')
	ins_apply_autocmds(EVENT_INSERTLEAVEPRE);

    // When an autoindent was removed, curswant stays after the
    // indent
    إذا (restart_edit == NUL && (colnr_T)عارضی == curwin->w_cursor.خانہ)
	curwin->w_سیٹ_curswant = true;

    // Remember the last Insert position in the '^ mark.
    إذا ((cmdmod.cmod_flags & CMOD_KEEPJUMPS) == 0)
	curbuf->b_آخری_داخل_کرو = curwin->w_cursor;

    /*
     * The cursor should end up on the last inserted character.
     * Don't do it for CTRL-O, unless past the end of the line.
     */
    إذا (!nomove
	    && (curwin->w_cursor.خانہ != 0
		|| curwin->w_cursor.coladd > 0)
	    && (restart_edit == NUL
		   || (gchar_cursor() == NUL && !VIsual_سرگرم))
#جے_تعریف_شدہ FEAT_RIGHTLEFT
	    && !revins_on
#جے_انت
				      )
    {
	إذا (curwin->w_cursor.coladd > 0 || get_ve_flags() == VE_ALL)
	{
	    oneleft();
	    إذا (restart_edit != NUL)
		++curwin->w_cursor.coladd;
	}
	وإلا
	{
	    --curwin->w_cursor.خانہ;
	    curwin->w_valid &= ~(VALID_WCOL|VALID_VIRTCOL);
	    // Correct cursor for multi-byte character.
	    إذا (has_mbyte)
		mb_adjust_cursor();
	}
    }

#جے_تعریف_شدہ HAVE_داخلہ_طریقہ
    // Disable IM to allow typing English directly for Normal mode commands.
    // When ":lmap" is enabled don't change 'iminsert' (IM can be enabled as
    // well).
    إذا (!(State & منوال_LANGMAP))
	im_save_حالت(&curbuf->b_p_iminsert);
    im_سیٹ_سرگرم(FALSE);
#جے_انت

    State = منوال_عام;
    may_trigger_modechanged();
    // need to position cursor again when on a TAB and when on a char with
    // virtual text.
    إذا (gchar_cursor() == TAB
#جے_تعریف_شدہ FEAT_PROP_POPUP
	    || curbuf->b_has_textprop
#جے_انت
       )
	curwin->w_valid &= ~(VALID_WROW|VALID_WCOL|VALID_VIRTCOL);

    setmouse();
#جے_تعریف_شدہ CURSOR_SHAPE
    ui_cursor_shape();		// may show different cursor shape
#جے_انت
    إذا (!p_ek)
    {
	MAY_WANT_منزل_LOG_THIS;

	// Re-enable bracketed paste mode.
	باہرلا_str_t_BE();

	// Re-enable modifyOtherKeys.
	باہرلا_str_t_TI();
    }
#جے_تعریف_شدہ FEAT_CONCEAL
    // Check if the cursor line needs redrawing after changing State.  If
    // 'concealcursor' is "i" it needs to be redrawn without concealing.
    conceal_check_cursor_سطر(cursor_سطر_was_concealed);
#جے_انت

    // When recording or for CTRL-O, need to display the new mode.
    // Otherwise remove the mode message.
    إذا (reg_recording != 0 || restart_edit != NUL)
	showmode();
    وإلا إذا (p_smd && (got_int || !skip_showmode()))
	پیغام("");

    أرجع TRUE;	    // exit Insert mode
}

#جے_تعریف_شدہ FEAT_RIGHTLEFT
/*
 * Toggle language: hkmap and revins_on.
 * Move to end of reverse inserted text.
 */
    ساكن فراغ
ins_ctrl_(فراغ)
{
    إذا (revins_on && revins_chars && revins_scol >= 0)
    {
	طالما (gchar_cursor() != NUL && revins_chars--)
	    ++curwin->w_cursor.خانہ;
    }
    p_ri = !p_ri;
    revins_on = (State == منوال_داخل_کرو && p_ri);
    إذا (revins_on)
    {
	revins_scol = curwin->w_cursor.خانہ;
	revins_legal++;
	revins_chars = 0;
	undisplay_dollar();
    }
    وإلا
	revins_scol = -1;
    p_hkmap = curwin->w_p_rl ^ p_ri;    // be consistent!
    showmode();
}
#جے_انت

/*
 * If 'keymodel' contains "startsel", may start selection.
 * Returns TRUE when a CTRL-O and other keys stuffed.
 */
    ساكن صحيح
ins_شروع_select(صحيح c)
{
    إذا (!km_startsel)
	أرجع FALSE;
    بدل (c)
    {
	حالة K_KHOME:
	حالة K_KEND:
	حالة K_PAGEUP:
	حالة K_KPAGEUP:
	حالة K_PAGEDOWN:
	حالة K_KPAGEDOWN:
#جے_تعریف_شدہ MACOS_X
	حالة K_بایاں:
	حالة K_دایاں:
	حالة K_اتے:
	حالة K_تھلے:
	حالة K_سرا:
	حالة K_HOME:
#جے_انت
	    إذا (!(mod_نقاب & MOD_نقاب_کھسکاؤ))
		اكسر;
	    // FALLTHROUGH
	حالة K_S_بایاں:
	حالة K_S_دایاں:
	حالة K_S_اتے:
	حالة K_S_تھلے:
	حالة K_S_سرا:
	حالة K_S_HOME:
	    // Start selection right away, the cursor can move with CTRL-O when
	    // beyond the end of the line.
	    شروع_selection();

	    // Execute the key in (insert) Select mode.
	    stuffcharReadbuff(Ctrl_O);
	    إذا (mod_نقاب)
	    {
		char_u	    buf[4];

		buf[0] = K_SPECIAL;
		buf[1] = KS_MODIFIER;
		buf[2] = mod_نقاب;
		buf[3] = NUL;
		stuffReadbuffLen(buf, 3L);
	    }
	    stuffcharReadbuff(c);
	    أرجع TRUE;
    }
    أرجع FALSE;
}

/*
 * <Insert> key in Insert mode: toggle insert/replace mode.
 */
    ساكن فراغ
ins_داخل_کرو(صحيح replaceState)
{
#جے_تعریف_شدہ FEAT_EVAL
    سیٹ_vim_var_string(VV_INSERTMODE,
		   (char_u *)((State & REPLACE_FLAG) ? "i"
			    : replaceState == منوال_VREPLACE ? "v" : "r"), 1);
#جے_انت
    ins_apply_autocmds(EVENT_INSERTCHANGE);
    إذا (State & REPLACE_FLAG)
	State = منوال_داخل_کرو | (State & منوال_LANGMAP);
    وإلا
	State = replaceState | (State & منوال_LANGMAP);
    may_trigger_modechanged();
    Append_Char_منزل_Redobuff(K_INS);
    showmode();
#جے_تعریف_شدہ CURSOR_SHAPE
    ui_cursor_shape();		// may show different cursor shape
#جے_انت
}

/*
 * Pressed CTRL-O in Insert mode.
 */
    ساكن فراغ
ins_ctrl_o(فراغ)
{
    restart_VIsual_select = 0;
    إذا (State & VREPLACE_FLAG)
	restart_edit = 'V';
    وإلا إذا (State & REPLACE_FLAG)
	restart_edit = 'R';
    وإلا
	restart_edit = 'I';
    إذا (virtual_سرگرم())
	ins_at_eol = FALSE;	// cursor always keeps its column
    وإلا
	ins_at_eol = (gchar_cursor() == NUL);
}

/*
 * If the cursor is on an indent, ^T/^D insert/delete one
 * shiftwidth.	Otherwise ^T/^D behave like a "<<" or ">>".
 * Always round the indent to 'shiftwidth', this is compatible
 * with vi.  But vi only supports ^T and ^D after an
 * autoindent, we support it everywhere.
 */
    ساكن فراغ
ins_کھسکاؤ(صحيح c, صحيح lastc)
{
    إذا (stop_arrow() == FAIL)
	أرجع;
    Append_Char_منزل_Redobuff(c);

    /*
     * 0^D and ^^D: remove all indent.
     */
    إذا (c == Ctrl_D && (lastc == '0' || lastc == '^')
						  && curwin->w_cursor.خانہ > 0)
    {
	--curwin->w_cursor.خانہ;
	(فراغ)del_char(FALSE);		// delete the '^' or '0'
	// In Replace mode, restore the characters that '^' or '0' replaced.
	إذا (State & REPLACE_FLAG)
	    replace_نکالو_ins();
	إذا (lastc == '^')
	    old_indent = get_indent();	// remember curr. indent
	change_indent(INDENT_سیٹ, 0, TRUE, 0, TRUE);
    }
    وإلا
	change_indent(c == Ctrl_D ? INDENT_DEC : INDENT_INC, 0, TRUE, 0, TRUE);

    إذا (did_ai && *skipwhite(ml_get_curline()) != NUL)
	did_ai = FALSE;
    did_si = FALSE;
    can_si = FALSE;
    can_si_پٹھ = FALSE;
    can_cindent = FALSE;	// no cindenting after ^D or ^T
}

    ساكن فراغ
ins_del(فراغ)
{
    صحيح	    عارضی;

    إذا (stop_arrow() == FAIL)
	أرجع;
    إذا (gchar_cursor() == NUL)		// delete newline
    {
	عارضی = curwin->w_cursor.خانہ;
	إذا (!can_bs(BS_EOL)		// only if "eol" included
		|| do_join(2, FALSE, TRUE, FALSE, FALSE) == FAIL)
	    vim_beep(BO_BS);
	وإلا
	{
	    curwin->w_cursor.خانہ = عارضی;
	    // Adjust orig_line_count in case more lines have been deleted than
	    // have been added. That makes sure, that open_line() later
	    // can access all buffer lines correctly
	    إذا (State & VREPLACE_FLAG &&
		    orig_سطر_گنتی > curbuf->b_ml.ml_سطر_گنتی)
		orig_سطر_گنتی = curbuf->b_ml.ml_سطر_گنتی;
	}
    }
    وإلا إذا (del_char(FALSE) == FAIL)  // delete char under cursor
	vim_beep(BO_BS);
    did_ai = FALSE;
    did_si = FALSE;
    can_si = FALSE;
    can_si_پٹھ = FALSE;
    Append_Char_منزل_Redobuff(K_DEL);
}

/*
 * Delete one character for ins_bs().
 */
    ساكن فراغ
ins_bs_one(فراغ)
{
    dec_cursor();
    إذا (State & REPLACE_FLAG)
    {
	// Don't delete characters before the insert point when in
	// Replace mode
	إذا (curwin->w_cursor.lnum != Insstart.lnum
		|| curwin->w_cursor.خانہ >= Insstart.خانہ)
	    replace_do_bs(-1);
    }
    وإلا
	(فراغ)del_char(FALSE);
}

/*
 * Handle Backspace, delete-word and delete-line in Insert mode.
 * Return TRUE when backspace was actually used.
 */
    ساكن صحيح
ins_bs(
    صحيح		c,
    صحيح		منوال,
    صحيح		*inserted_space_p)
{
    linenr_T	lnum;
    صحيح		cc;
    صحيح		عارضی = 0;	    // init for GCC
    colnr_T	save_خانہ;
    colnr_T	mincol;
    صحيح		did_backspace = FALSE;
    صحيح		وچ_indent;
    صحيح		oldState;
    صحيح		cpc[MAX_MCO];	    // composing characters
    صحيح		call_fix_indent = FALSE;

    /*
     * can't delete anything in an empty file
     * can't backup past first character in buffer
     * can't backup past starting point unless 'backspace' > 1
     * can backup to a previous line if 'backspace' == 0
     */
    إذا (       BUFEMPTY()
	    || (
#جے_تعریف_شدہ FEAT_RIGHTLEFT
		!revins_on &&
#جے_انت
		((curwin->w_cursor.lnum == 1 && curwin->w_cursor.خانہ == 0)
		    || (!can_bs(BS_شروع)
			&& ((arrow_ورتیا
#جے_تعریف_شدہ FEAT_کم_CHANNEL
				&& !bt_prompt(curbuf)
#جے_انت
			) || (curwin->w_cursor.lnum == Insstart_orig.lnum
				&& curwin->w_cursor.خانہ <= Insstart_orig.خانہ)))
		    || (!can_bs(BS_INDENT) && !arrow_ورتیا && ai_خانہ > 0
					 && curwin->w_cursor.خانہ <= ai_خانہ)
		    || (!can_bs(BS_EOL) && curwin->w_cursor.خانہ == 0))))
    {
	vim_beep(BO_BS);
	أرجع FALSE;
    }

    إذا (stop_arrow() == FAIL)
	أرجع FALSE;
    وچ_indent = inindent(0);
    إذا (وچ_indent)
	can_cindent = FALSE;
    سرا_comment_pending = NUL;	// After BS, don't auto-end comment
#جے_تعریف_شدہ FEAT_RIGHTLEFT
    إذا (revins_on)	    // put cursor after last inserted char
	inc_cursor();
#جے_انت

    // Virtualedit:
    //	BACKSPACE_CHAR eats a virtual space
    //	BACKSPACE_WORD eats all coladd
    //	BACKSPACE_LINE eats all coladd and keeps going
    إذا (curwin->w_cursor.coladd > 0)
    {
	إذا (منوال == BACKSPACE_CHAR)
	{
	    --curwin->w_cursor.coladd;
	    أرجع TRUE;
	}
	إذا (منوال == BACKSPACE_لفظ)
	{
	    curwin->w_cursor.coladd = 0;
	    أرجع TRUE;
	}
	curwin->w_cursor.coladd = 0;
    }

    /*
     * Delete newline!
     */
    إذا (curwin->w_cursor.خانہ == 0)
    {
	lnum = Insstart.lnum;
	إذا (curwin->w_cursor.lnum == lnum
#جے_تعریف_شدہ FEAT_RIGHTLEFT
			|| revins_on
#جے_انت
				    )
	{
	    إذا (u_save((linenr_T)(curwin->w_cursor.lnum - 2),
			       (linenr_T)(curwin->w_cursor.lnum + 1)) == FAIL)
		أرجع FALSE;
	    --Insstart.lnum;
	    Insstart.خانہ = ml_get_len(Insstart.lnum);
	}
	/*
	 * In replace mode:
	 * cc < 0: NL was inserted, delete it
	 * cc >= 0: NL was replaced, put original characters back
	 */
	cc = -1;
	إذا (State & REPLACE_FLAG)
	    cc = replace_نکالو();	    // returns -1 if NL was inserted
	/*
	 * In replace mode, in the line we started replacing, we only move the
	 * cursor.
	 */
	إذا ((State & REPLACE_FLAG) && curwin->w_cursor.lnum <= lnum)
	{
	    dec_cursor();
	}
	وإلا
	{
	    إذا (!(State & VREPLACE_FLAG)
				   || curwin->w_cursor.lnum > orig_سطر_گنتی)
	    {
		عارضی = gchar_cursor();	// remember current char
		--curwin->w_cursor.lnum;

		// When "aw" is in 'formatoptions' we must delete the space at
		// the end of the line, otherwise the line will be broken
		// again when auto-formatting.
		إذا (has_format_option(FO_AUTO)
					   && has_format_option(FO_WHITE_PAR))
		{
		    char_u  *ptr = ml_get_buf(curbuf, curwin->w_cursor.lnum,
									FALSE);
		    صحيح	    len = ml_get_curline_len();

		    إذا (len > 0 && ptr[len - 1] == ' ')
		    {
			char_u *newp = alloc(curbuf->b_ml.ml_سطر_len - 1);

			إذا (newp != عدم)
			{
			    mch_memmove(newp, ptr, len - 1);
			    newp[len - 1] = NUL;
			    إذا (curbuf->b_ml.ml_سطر_len > len + 1)
				mch_memmove(newp + len, ptr + len + 1,
					   curbuf->b_ml.ml_سطر_len - len - 1);

			    إذا (curbuf->b_ml.ml_flags
					      & (ML_سطر_DIRTY | ML_ALLOCATED))
				vim_free(curbuf->b_ml.ml_سطر_ptr);
			    curbuf->b_ml.ml_سطر_ptr = newp;
			    curbuf->b_ml.ml_سطر_len--;
			    curbuf->b_ml.ml_سطر_textlen--;
			    curbuf->b_ml.ml_flags |= ML_سطر_DIRTY;
			}
		    }
		}

		(فراغ)do_join(2, FALSE, FALSE, FALSE, FALSE);
		إذا (عارضی == NUL && gchar_cursor() != NUL)
		    inc_cursor();
	    }
	    وإلا
		dec_cursor();

	    /*
	     * In MODE_REPLACE mode we have to put back the text that was
	     * replaced by the NL. On the replace stack is first a
	     * NUL-terminated sequence of characters that were deleted and then
	     * the characters that NL replaced.
	     */
	    إذا (State & REPLACE_FLAG)
	    {
		/*
		 * Do the next ins_char() in MODE_NORMAL state, to
		 * prevent ins_char() from replacing characters and
		 * avoiding showmatch().
		 */
		oldState = State;
		State = منوال_عام;
		/*
		 * restore characters (blanks) deleted after cursor
		 */
		طالما (cc > 0)
		{
		    save_خانہ = curwin->w_cursor.خانہ;
		    mb_replace_نکالو_ins(cc);
		    curwin->w_cursor.خانہ = save_خانہ;
		    cc = replace_نکالو();
		}
		// restore the characters that NL replaced
		replace_نکالو_ins();
		State = oldState;
	    }
	}
	did_ai = FALSE;
    }
    وإلا
    {
	/*
	 * Delete character(s) before the cursor.
	 */
#جے_تعریف_شدہ FEAT_RIGHTLEFT
	إذا (revins_on)		// put cursor on last inserted char
	    dec_cursor();
#جے_انت
	mincol = 0;
						// keep indent
	إذا (منوال == BACKSPACE_سطر
		&& (curbuf->b_p_ai || cindent_on())
#جے_تعریف_شدہ FEAT_RIGHTLEFT
		&& !revins_on
#جے_انت
			    )
	{
	    save_خانہ = curwin->w_cursor.خانہ;
	    beginline(BL_WHITE);
	    إذا (curwin->w_cursor.خانہ < save_خانہ)
	    {
		mincol = curwin->w_cursor.خانہ;
		// should now fix the indent to match with the previous line
		call_fix_indent = TRUE;
	    }
	    curwin->w_cursor.خانہ = save_خانہ;
	}

	/*
	 * Handle deleting one 'shiftwidth' or 'softtabstop'.
	 */
	إذا (	   منوال == BACKSPACE_CHAR
		&& ((p_sta && وچ_indent)
		    || ((get_sts_قدر() != 0
#جے_تعریف_شدہ FEAT_VARTABS
			|| tabstop_گنتی(curbuf->b_p_vsts_array)
#جے_انت
			)
			&& curwin->w_cursor.خانہ > 0
			&& (*(ml_get_cursor() - 1) == TAB
			    || (*(ml_get_cursor() - 1) == ' '
				&& (!*inserted_space_p
				    || arrow_ورتیا))))))
	{
	    colnr_T	vcol = 0;
	    colnr_T	want_vcol;
	    char_u	*سطر;
	    char_u	*ptr;
	    char_u	*cursor_ptr;
	    char_u	*space_ptr;
	    colnr_T	space_vcol = 0;
	    صحيح		prev_space = FALSE;
	    colnr_T	want_خانہ;

	    *inserted_space_p = FALSE;

	    space_ptr = ptr = سطر = ml_get_curline();
	    cursor_ptr = سطر + curwin->w_cursor.خانہ;

	    // Compute virtual column of cursor position, and find the last
	    // whitespace before cursor that is preceded by non-whitespace.
	    // Use chartabsize() so that virtual text and wrapping are ignored.
	    طالما (ptr < cursor_ptr)
	    {
		صحيح	cur_space = VIM_ISWHITE(*ptr);

		إذا (!prev_space && cur_space)
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
	    إذا (p_sta && وچ_indent)
		want_vcol -= want_vcol % (صحيح)get_sw_قدر(curbuf);
	    وإلا
#جے_تعریف_شدہ FEAT_VARTABS
		want_vcol = tabstop_شروع(want_vcol, get_sts_قدر(),
						       curbuf->b_p_vsts_array);
#وإلا
		want_vcol -= want_vcol % (صحيح)get_sts_قدر();
#جے_انت

	    // Find the position to stop backspacing.
	    // Use chartabsize() so that virtual text and wrapping are ignored.
	    طالما (TRUE)
	    {
		صحيح ناپ = chartabsize(space_ptr, space_vcol);

		إذا (space_vcol + ناپ > want_vcol)
		    اكسر;
		space_vcol += ناپ;
		MB_PTR_ADV(space_ptr);
	    }
	    want_خانہ = space_ptr - سطر;

	    // Delete characters until we are at or before want_col.
	    طالما (curwin->w_cursor.خانہ > want_خانہ)
		ins_bs_one();

	    // Insert extra spaces until we are at want_vcol.
	    لكل (; space_vcol < want_vcol; space_vcol++)
	    {
		// Remember the first char we inserted
		إذا (curwin->w_cursor.lnum == Insstart_orig.lnum
				   && curwin->w_cursor.خانہ < Insstart_orig.خانہ)
		    Insstart_orig.خانہ = curwin->w_cursor.خانہ;

		إذا (State & VREPLACE_FLAG)
		    ins_char(' ');
		وإلا
		{
		    ins_str((char_u *)" ", 1);
		    إذا ((State & REPLACE_FLAG))
			replace_ڈالو(NUL);
		}
	    }
	}

	/*
	 * Delete up to starting point, start of line or previous word.
	 */
	وإلا
	{
	    صحيح cclass = 0, prev_cclass = 0;

	    إذا (has_mbyte)
		cclass = mb_get_class(ml_get_cursor());
	    افعل
	    {
#جے_تعریف_شدہ FEAT_RIGHTLEFT
		إذا (!revins_on) // put cursor on char to be deleted
#جے_انت
		    dec_cursor();

		cc = gchar_cursor();
		// look multi-byte character class
		إذا (has_mbyte)
		{
		    prev_cclass = cclass;
		    cclass = mb_get_class(ml_get_cursor());
		}

		// start of word?
		إذا (منوال == BACKSPACE_لفظ && !vim_isspace(cc))
		{
		    منوال = BACKSPACE_لفظ_NOT_SPACE;
		    عارضی = vim_iswordc(cc);
		}
		// end of word?
		وإلا إذا (منوال == BACKSPACE_لفظ_NOT_SPACE
			&& ((vim_isspace(cc) || vim_iswordc(cc) != عارضی)
			|| prev_cclass != cclass))
		{
#جے_تعریف_شدہ FEAT_RIGHTLEFT
		    إذا (!revins_on)
#جے_انت
			inc_cursor();
#جے_تعریف_شدہ FEAT_RIGHTLEFT
		    وإلا إذا (State & REPLACE_FLAG)
			dec_cursor();
#جے_انت
		    اكسر;
		}
		إذا (State & REPLACE_FLAG)
		    replace_do_bs(-1);
		وإلا
		{
		    إذا (enc_utf8 && p_deco)
			(فراغ)utfc_ptr2char(ml_get_cursor(), cpc);
		    (فراغ)del_char(FALSE);
		    /*
		     * If there are combining characters and 'delcombine' is set
		     * move the cursor back.  Don't back up before the base
		     * character.
		     */
		    إذا (enc_utf8 && p_deco && cpc[0] != NUL)
			inc_cursor();
#جے_تعریف_شدہ FEAT_RIGHTLEFT
		    إذا (revins_chars)
		    {
			revins_chars--;
			revins_legal++;
		    }
		    إذا (revins_on && gchar_cursor() == NUL)
			اكسر;
#جے_انت
		}
		// Just a single backspace?:
		إذا (منوال == BACKSPACE_CHAR)
		    اكسر;
	    } طالما (
#جے_تعریف_شدہ FEAT_RIGHTLEFT
		    revins_on ||
#جے_انت
		    (curwin->w_cursor.خانہ > mincol
		    &&  (can_bs(BS_NOSTOP)
			|| (curwin->w_cursor.lnum != Insstart_orig.lnum
			|| curwin->w_cursor.خانہ != Insstart_orig.خانہ)
		    )));
	}
	did_backspace = TRUE;
    }
    did_si = FALSE;
    can_si = FALSE;
    can_si_پٹھ = FALSE;
    إذا (curwin->w_cursor.خانہ <= 1)
	did_ai = FALSE;

    إذا (call_fix_indent)
	fix_indent();

    /*
     * It's a little strange to put backspaces into the redo
     * buffer, but it makes auto-indent a lot easier to deal
     * with.
     */
    Append_Char_منزل_Redobuff(c);

    // If deleted before the insertion point, adjust it
    إذا (curwin->w_cursor.lnum == Insstart_orig.lnum
				  && curwin->w_cursor.خانہ < Insstart_orig.خانہ)
	Insstart_orig.خانہ = curwin->w_cursor.خانہ;

    // vi behaviour: the cursor moves backward but the character that
    //		     was there remains visible
    // Vim behaviour: the cursor moves backward and the character that
    //		      was there is erased from the screen.
    // We can emulate the vi behaviour by pretending there is a dollar
    // displayed even when there isn't.
    //  --pkv Sun Jan 19 01:56:40 EST 2003
    إذا (vim_strchr(p_cpo, CPO_BACKSPACE) != عدم && dollar_vcol == -1)
	dollar_vcol = curwin->w_virtcol;

#جے_تعریف_شدہ FEAT_FOLDING
    // When deleting a char the cursor line must never be in a closed fold.
    // E.g., when 'foldmethod' is indent and deleting the first non-white
    // char before a Tab.
    إذا (did_backspace)
	foldOpenCursor();
#جے_انت

    أرجع did_backspace;
}

/*
 * Handle receiving P_PS: start paste mode.  Inserts the following text up to
 * P_PE literally.
 * When "drop" is TRUE then consume the text and drop it.
 */
    صحيح
bracketed_paste(paste_منوال_T منوال, صحيح drop, garray_T *gap)
{
    صحيح		c;
    char_u	buf[NUMBUFLEN + MB_MAXBYTES];
    صحيح		idx = 0;
    char_u	*سرا = لبھو_termcode((char_u *)"PE");
    صحيح		ret_char = -1;
    صحيح		save_allow_کلیدیں = allow_کلیدیں;
    صحيح		save_paste = p_paste;

    // If the end code is too long we can't detect it, read everything.
    إذا (سرا != عدم && STRLEN(سرا) >= NUMBUFLEN)
	سرا = عدم;
    ++no_mapping;
    allow_کلیدیں = 0;
    إذا (!p_paste)
	// Also have the side effects of setting 'paste' to make it work much
	// faster.
	سیٹ_option_قدر_give_err((char_u *)"paste", TRUE, عدم, 0);

    لكل (;;)
    {
	// When the end is not defined read everything there is.
	إذا (سرا == عدم && vpeekc() == NUL)
	    اكسر;
	افعل
	    c = vgetc();
	طالما (c == K_IGNORE || c == K_VER_SCROLLBAR || c == K_HOR_SCROLLBAR);

	إذا (c == NUL || got_int || (ex_عام_busy > 0 && c == Ctrl_C))
	    // When CTRL-C was encountered the typeahead will be flushed and we
	    // won't get the end sequence.  Except when using ":normal".
	    اكسر;

	إذا (has_mbyte)
	    idx += (*mb_char2bytes)(c, buf + idx);
	وإلا
	    buf[idx++] = c;
	buf[idx] = NUL;
	إذا (سرا != عدم && STRNCMP(buf, سرا, idx) == 0)
	{
	    إذا (سرا[idx] == NUL)
		اكسر; // Found the end of paste code.
	    استمر;
	}
	إذا (!drop)
	{
	    بدل (منوال)
	    {
		حالة PASTE_CMDLINE:
		    put_on_cmdline(buf, idx, TRUE);
		    اكسر;

		حالة PASTE_EX:
		    // add one for the NUL that is going to be appended
		    إذا (gap != عدم && ga_grow(gap, idx + 1) == OK)
		    {
			mch_memmove((حرف *)gap->ga_انکڑے + gap->ga_len,
							     buf, (size_t)idx);
			gap->ga_len += idx;
		    }
		    اكسر;

		حالة PASTE_داخل_کرو:
		    إذا (stop_arrow() == OK)
		    {
			c = buf[0];
			إذا (idx == 1 && (c == CAR || c == K_KENTER || c == NL))
			    ins_eol(c);
			وإلا
			{
			    ins_char_بائٹاں(buf, idx);
			    Append_منزل_Redobuff_Lit(buf, idx);
			}
		    }
		    اكسر;

		حالة PASTE_ONE_CHAR:
		    إذا (ret_char == -1)
		    {
			إذا (has_mbyte)
			    ret_char = (*mb_ptr2char)(buf);
			وإلا
			    ret_char = buf[0];
		    }
		    اكسر;
	    }
	}
	idx = 0;
    }

    --no_mapping;
    allow_کلیدیں = save_allow_کلیدیں;
    إذا (!save_paste)
	سیٹ_option_قدر_give_err((char_u *)"paste", FALSE, عدم, 0);

    أرجع ret_char;
}

#إذا defined(FEAT_GUI_TABLINE)
    ساكن فراغ
ins_tabline(صحيح c)
{
    // We will be leaving the current window, unless closing another tab.
    إذا (c != K_TABMENU || current_tabmenu != TABLINE_MENU_CLOSE
		|| (current_tab != 0 && current_tab != tabpage_اشاریہ(curtab)))
    {
	undisplay_dollar();
	شروع_arrow(&curwin->w_cursor);
	can_cindent = TRUE;
    }

    إذا (c == K_TABLINE)
	goto_tabpage(current_tab);
    وإلا
    {
	handle_tabmenu();
	redraw_statuslines();	// will redraw the tabline when needed
    }
}
#جے_انت

#إذا defined(FEAT_GUI)
    فراغ
ins_scroll(فراغ)
{
    pos_T	tpos;

    undisplay_dollar();
    tpos = curwin->w_cursor;
    إذا (gui_do_scroll())
    {
	شروع_arrow(&tpos);
	can_cindent = TRUE;
    }
}

    فراغ
ins_horscroll(فراغ)
{
    pos_T	tpos;

    undisplay_dollar();
    tpos = curwin->w_cursor;
    إذا (do_mousescroll_horiz(scrollbar_قدر))
    {
	شروع_arrow(&tpos);
	can_cindent = TRUE;
    }
}
#جے_انت

    ساكن فراغ
ins_بایاں(فراغ)
{
    pos_T	tpos;
    صحيح		سرا_change = dont_sync_undo == FALSE; // end undoable change

#جے_تعریف_شدہ FEAT_FOLDING
    إذا ((fdo_flags & FDO_HOR) && کلید_Typed)
	foldOpenCursor();
#جے_انت
    undisplay_dollar();
    tpos = curwin->w_cursor;
    إذا (oneleft() == OK)
    {
#إذا defined(FEAT_XIM) && defined(FEAT_GUI_GTK)
	// Only call start_arrow() when not busy with preediting, it will
	// break undo.  K_LEFT is inserted in im_correct_cursor().
	إذا (p_imst == IM_OVER_THE_SPOT || !im_is_preediting())
#جے_انت
	{
	    شروع_arrow_with_change(&tpos, سرا_change);
	    إذا (!سرا_change)
		Append_Char_منزل_Redobuff(K_بایاں);
	}
#جے_تعریف_شدہ FEAT_RIGHTLEFT
	// If exit reversed string, position is fixed
	إذا (revins_scol != -1 && (صحيح)curwin->w_cursor.خانہ >= revins_scol)
	    revins_legal++;
	revins_chars++;
#جے_انت
    }

    /*
     * if 'whichwrap' set for cursor in insert mode may go to
     * previous line
     */
    وإلا إذا (vim_strchr(p_ww, '[') != عدم && curwin->w_cursor.lnum > 1)
    {
	// always break undo when moving upwards/downwards, else undo may break
	شروع_arrow(&tpos);
	--(curwin->w_cursor.lnum);
	coladvance((colnr_T)MAXCOL);
	curwin->w_سیٹ_curswant = true;	// so we stay at the end
    }
    وإلا
	vim_beep(BO_CRSR);
    dont_sync_undo = FALSE;
}

    ساكن فراغ
ins_home(صحيح c)
{
    pos_T	tpos;

#جے_تعریف_شدہ FEAT_FOLDING
    إذا ((fdo_flags & FDO_HOR) && کلید_Typed)
	foldOpenCursor();
#جے_انت
    undisplay_dollar();
    tpos = curwin->w_cursor;
    إذا (c == K_C_HOME)
	curwin->w_cursor.lnum = 1;
    curwin->w_cursor.خانہ = 0;
    curwin->w_cursor.coladd = 0;
    curwin->w_curswant = 0;
    شروع_arrow(&tpos);
}

    ساكن فراغ
ins_سرا(صحيح c)
{
    pos_T	tpos;

#جے_تعریف_شدہ FEAT_FOLDING
    إذا ((fdo_flags & FDO_HOR) && کلید_Typed)
	foldOpenCursor();
#جے_انت
    undisplay_dollar();
    tpos = curwin->w_cursor;
    إذا (c == K_C_سرا)
	curwin->w_cursor.lnum = curbuf->b_ml.ml_سطر_گنتی;
    coladvance((colnr_T)MAXCOL);
    curwin->w_curswant = MAXCOL;

    شروع_arrow(&tpos);
}

    ساكن فراغ
ins_s_بایاں(فراغ)
{
    صحيح سرا_change = dont_sync_undo == FALSE; // end undoable change
#جے_تعریف_شدہ FEAT_FOLDING
    إذا ((fdo_flags & FDO_HOR) && کلید_Typed)
	foldOpenCursor();
#جے_انت
    undisplay_dollar();
    إذا (curwin->w_cursor.lnum > 1 || curwin->w_cursor.خانہ > 0)
    {
	شروع_arrow_with_change(&curwin->w_cursor, سرا_change);
	إذا (!سرا_change)
	    Append_Char_منزل_Redobuff(K_S_بایاں);
	(فراغ)bck_لفظ(1L, FALSE, FALSE);
	curwin->w_سیٹ_curswant = true;
    }
    وإلا
	vim_beep(BO_CRSR);
    dont_sync_undo = FALSE;
}

    ساكن فراغ
ins_دایاں(فراغ)
{
    صحيح سرا_change = dont_sync_undo == FALSE; // end undoable change

#جے_تعریف_شدہ FEAT_FOLDING
    إذا ((fdo_flags & FDO_HOR) && کلید_Typed)
	foldOpenCursor();
#جے_انت
    undisplay_dollar();
    إذا (gchar_cursor() != NUL || virtual_سرگرم())
    {
	شروع_arrow_with_change(&curwin->w_cursor, سرا_change);
	إذا (!سرا_change)
	    Append_Char_منزل_Redobuff(K_دایاں);
	curwin->w_سیٹ_curswant = true;
	إذا (virtual_سرگرم())
	    oneright();
	وإلا
	{
	    إذا (has_mbyte)
		curwin->w_cursor.خانہ += (*mb_ptr2len)(ml_get_cursor());
	    وإلا
		++curwin->w_cursor.خانہ;
	}

#جے_تعریف_شدہ FEAT_RIGHTLEFT
	revins_legal++;
	إذا (revins_chars)
	    revins_chars--;
#جے_انت
    }
    // if 'whichwrap' set for cursor in insert mode, may move the
    // cursor to the next line
    وإلا إذا (vim_strchr(p_ww, ']') != عدم
	    && curwin->w_cursor.lnum < curbuf->b_ml.ml_سطر_گنتی)
    {
	شروع_arrow(&curwin->w_cursor);
	curwin->w_سیٹ_curswant = true;
	++curwin->w_cursor.lnum;
	curwin->w_cursor.خانہ = 0;
    }
    وإلا
	vim_beep(BO_CRSR);
    dont_sync_undo = FALSE;
}

    ساكن فراغ
ins_s_دایاں(فراغ)
{
    صحيح سرا_change = dont_sync_undo == FALSE; // end undoable change
#جے_تعریف_شدہ FEAT_FOLDING
    إذا ((fdo_flags & FDO_HOR) && کلید_Typed)
	foldOpenCursor();
#جے_انت
    undisplay_dollar();
    إذا (curwin->w_cursor.lnum < curbuf->b_ml.ml_سطر_گنتی
	    || gchar_cursor() != NUL)
    {
	شروع_arrow_with_change(&curwin->w_cursor, سرا_change);
	إذا (!سرا_change)
	    Append_Char_منزل_Redobuff(K_S_دایاں);
	(فراغ)fwd_لفظ(1L, FALSE, 0);
	curwin->w_سیٹ_curswant = true;
    }
    وإلا
	vim_beep(BO_CRSR);
    dont_sync_undo = FALSE;
}

    ساكن فراغ
ins_اتے(
    صحيح		startcol)	// when TRUE move to Insstart.col
{
    pos_T	tpos;
    linenr_T	old_topline = curwin->w_topline;
#جے_تعریف_شدہ FEAT_DIFF
    صحيح		old_topfill = curwin->w_topfill;
#جے_انت

    undisplay_dollar();
    tpos = curwin->w_cursor;
    إذا (cursor_اتے(1L, TRUE) == OK)
    {
	إذا (startcol)
	    coladvance(getvcol_nolist(&Insstart));
	إذا (old_topline != curwin->w_topline
#جے_تعریف_شدہ FEAT_DIFF
		|| old_topfill != curwin->w_topfill
#جے_انت
		)
	    redraw_later(UPD_VALID);
	شروع_arrow(&tpos);
	can_cindent = TRUE;
    }
    وإلا
	vim_beep(BO_CRSR);
}

    ساكن فراغ
ins_pageup(فراغ)
{
    pos_T	tpos;

    undisplay_dollar();

    إذا (mod_نقاب & MOD_نقاب_CTRL)
    {
	// <C-PageUp>: tab page back
	إذا (پہلا_tabpage->tp_اگلا != عدم)
	{
	    شروع_arrow(&curwin->w_cursor);
	    goto_tabpage(-1);
	}
	أرجع;
    }

    tpos = curwin->w_cursor;
    إذا (pagescroll(BACKWARD, 1L, FALSE) == OK)
    {
	شروع_arrow(&tpos);
	can_cindent = TRUE;
    }
    وإلا
	vim_beep(BO_CRSR);
}

    ساكن فراغ
ins_تھلے(
    صحيح		startcol)	// when TRUE move to Insstart.col
{
    pos_T	tpos;
    linenr_T	old_topline = curwin->w_topline;
#جے_تعریف_شدہ FEAT_DIFF
    صحيح		old_topfill = curwin->w_topfill;
#جے_انت

    undisplay_dollar();
    tpos = curwin->w_cursor;
    إذا (cursor_تھلے(1L, TRUE) == OK)
    {
	إذا (startcol)
	    coladvance(getvcol_nolist(&Insstart));
	إذا (old_topline != curwin->w_topline
#جے_تعریف_شدہ FEAT_DIFF
		|| old_topfill != curwin->w_topfill
#جے_انت
		)
	    redraw_later(UPD_VALID);
	شروع_arrow(&tpos);
	can_cindent = TRUE;
    }
    وإلا
	vim_beep(BO_CRSR);
}

    ساكن فراغ
ins_pagedown(فراغ)
{
    pos_T	tpos;

    undisplay_dollar();

    إذا (mod_نقاب & MOD_نقاب_CTRL)
    {
	// <C-PageDown>: tab page forward
	إذا (پہلا_tabpage->tp_اگلا != عدم)
	{
	    شروع_arrow(&curwin->w_cursor);
	    goto_tabpage(0);
	}
	أرجع;
    }

    tpos = curwin->w_cursor;
    إذا (pagescroll(FORWARD, 1L, FALSE) == OK)
    {
	شروع_arrow(&tpos);
	can_cindent = TRUE;
    }
    وإلا
	vim_beep(BO_CRSR);
}

#جے_تعریف_شدہ FEAT_DND
    ساكن فراغ
ins_drop(فراغ)
{
    do_put('~', عدم, BACKWARD, 1L, PUT_CURSEND);
}
#جے_انت

/*
 * Handle TAB in Insert or Replace mode.
 * Return TRUE when the TAB needs to be inserted like a normal character.
 */
    ساكن صحيح
ins_tab(فراغ)
{
    صحيح		ind;
    صحيح		i;
    صحيح		عارضی;

    إذا (Insstart_blank_vcol == MAXCOL && curwin->w_cursor.lnum == Insstart.lnum)
	Insstart_blank_vcol = get_nolist_virtcol();
    إذا (echeck_abbr(TAB + ABBR_OFF))
	أرجع FALSE;

    ind = inindent(0);
    إذا (ind)
	can_cindent = FALSE;

    /*
     * When nothing special, insert TAB like a normal character.
     */
    إذا (!curbuf->b_p_et
#جے_تعریف_شدہ FEAT_VARTABS
	    && !(p_sta && ind
		// These five lines mean 'tabstop' != 'shiftwidth'
		&& ((tabstop_گنتی(curbuf->b_p_vts_array) > 1)
		    || (tabstop_گنتی(curbuf->b_p_vts_array) == 1
			&& tabstop_پہلا(curbuf->b_p_vts_array)
						       != get_sw_قدر(curbuf))
		    || (tabstop_گنتی(curbuf->b_p_vts_array) == 0
			&& curbuf->b_p_ts != get_sw_قدر(curbuf))))
	    && tabstop_گنتی(curbuf->b_p_vsts_array) == 0
#وإلا
	    && !(p_sta && ind && curbuf->b_p_ts != get_sw_قدر(curbuf))
#جے_انت
	    && get_sts_قدر() == 0)
	أرجع TRUE;

    إذا (stop_arrow() == FAIL)
	أرجع TRUE;

    did_ai = FALSE;
    did_si = FALSE;
    can_si = FALSE;
    can_si_پٹھ = FALSE;
    Append_منزل_Redobuff((char_u *)"\t");

#جے_تعریف_شدہ FEAT_VARTABS
    إذا (p_sta && ind)		// insert tab in indent, use 'shiftwidth'
    {
	عارضی = (صحيح)get_sw_قدر(curbuf);
	عارضی -= get_nolist_virtcol() % عارضی;
    }
    وإلا إذا (tabstop_گنتی(curbuf->b_p_vsts_array) > 0 || curbuf->b_p_sts != 0)
				// use 'softtabstop' when set
	عارضی = tabstop_padding(get_nolist_virtcol(), get_sts_قدر(),
						     curbuf->b_p_vsts_array);
    وإلا			// otherwise use 'tabstop'
	عارضی = tabstop_padding(get_nolist_virtcol(), curbuf->b_p_ts,
						     curbuf->b_p_vts_array);
#وإلا
    إذا (p_sta && ind)		// insert tab in indent, use 'shiftwidth'
	عارضی = (صحيح)get_sw_قدر(curbuf);
    وإلا إذا (curbuf->b_p_sts != 0) // use 'softtabstop' when set
	عارضی = (صحيح)get_sts_قدر();
    وإلا			// otherwise use 'tabstop'
	عارضی = (صحيح)curbuf->b_p_ts;
    عارضی -= get_nolist_virtcol() % عارضی;
#جے_انت

    /*
     * Insert the first space with ins_char().	It will delete one char in
     * replace mode.  Insert the rest with ins_str(); it will not delete any
     * chars.  For MODE_VREPLACE state, we use ins_char() for all characters.
     */
    ins_char(' ');
    طالما (--عارضی > 0)
    {
	إذا (State & VREPLACE_FLAG)
	    ins_char(' ');
	وإلا
	{
	    ins_str((char_u *)" ", 1);
	    إذا (State & REPLACE_FLAG)	    // no char replaced
		replace_ڈالو(NUL);
	}
    }

    /*
     * When 'expandtab' not set: Replace spaces by TABs where possible.
     */
#جے_تعریف_شدہ FEAT_VARTABS
    إذا (!curbuf->b_p_et && (tabstop_گنتی(curbuf->b_p_vsts_array) > 0
			    || get_sts_قدر() > 0
			    || (p_sta && ind)))
#وإلا
    إذا (!curbuf->b_p_et && (get_sts_قدر() || (p_sta && ind)))
#جے_انت
    {
	char_u		*ptr;
	char_u		*saved_سطر = عدم;	// init for GCC
	pos_T		pos;
	pos_T		fpos;
	pos_T		*cursor;
	colnr_T		want_vcol, vcol;
	صحيح		change_خانہ = -1;
	صحيح		save_list = curwin->w_p_list;
	char_u		*tab = (char_u *)"\t";
	chartabsize_T	cts;

	/*
	 * Get the current line.  For MODE_VREPLACE state, don't make real
	 * changes yet, just work on a copy of the line.
	 */
	إذا (State & VREPLACE_FLAG)
	{
	    pos = curwin->w_cursor;
	    cursor = &pos;
	    saved_سطر = vim_strnsave(ml_get_curline(), ml_get_curline_len());
	    إذا (saved_سطر == عدم)
		أرجع FALSE;
	    ptr = saved_سطر + pos.خانہ;
	}
	وإلا
	{
	    ptr = ml_get_cursor();
	    cursor = &curwin->w_cursor;
	}

	// When 'L' is not in 'cpoptions' a tab always takes up 'ts' spaces.
	إذا (vim_strchr(p_cpo, CPO_LISTWM) == عدم)
	    curwin->w_p_list = FALSE;

	// Find first white before the cursor
	fpos = curwin->w_cursor;
	طالما (fpos.خانہ > 0 && VIM_ISWHITE(ptr[-1]))
	{
	    --fpos.خانہ;
	    --ptr;
	}

	// In Replace mode, don't change characters before the insert point.
	إذا ((State & REPLACE_FLAG)
		&& fpos.lnum == Insstart.lnum
		&& fpos.خانہ < Insstart.خانہ)
	{
	    ptr += Insstart.خانہ - fpos.خانہ;
	    fpos.خانہ = Insstart.خانہ;
	}

	// compute virtual column numbers of first white and cursor
	getvcol(curwin, &fpos, &vcol, عدم, عدم, 0);
	getvcol(curwin, cursor, &want_vcol, عدم, عدم, 0);

	init_chartabsize_arg(&cts, curwin, 0, vcol, tab, tab);

	// Use as many TABs as possible.  Beware of 'breakindent', 'showbreak'
	// and 'linebreak' adding extra virtual columns.
	طالما (VIM_ISWHITE(*ptr))
	{
	    i = lbr_chartabsize(&cts);
	    إذا (cts.cts_vcol + i > want_vcol)
		اكسر;
	    إذا (*ptr != TAB)
	    {
		*ptr = TAB;
		إذا (change_خانہ < 0)
		{
		    change_خانہ = fpos.خانہ;  // Column of first change
		    // May have to adjust Insstart
		    إذا (fpos.lnum == Insstart.lnum && fpos.خانہ < Insstart.خانہ)
			Insstart.خانہ = fpos.خانہ;
		}
	    }
	    ++fpos.خانہ;
	    ++ptr;
	    cts.cts_vcol += i;
	}
	vcol = cts.cts_vcol;
	clear_chartabsize_arg(&cts);

	إذا (change_خانہ >= 0)
	{
	    صحيح		    repl_off = 0;

	    // Skip over the spaces we need.
	    init_chartabsize_arg(&cts, curwin, 0, vcol, ptr, ptr);
	    طالما (cts.cts_vcol < want_vcol && *cts.cts_ptr == ' ')
	    {
		cts.cts_vcol += lbr_chartabsize(&cts);
		++cts.cts_ptr;
		++repl_off;
	    }
	    ptr = cts.cts_ptr;
	    vcol = cts.cts_vcol;
	    clear_chartabsize_arg(&cts);

	    إذا (vcol > want_vcol)
	    {
		// Must have a char with 'showbreak' just before it.
		--ptr;
		--repl_off;
	    }
	    fpos.خانہ += repl_off;

	    // Delete following spaces.
	    i = cursor->خانہ - fpos.خانہ;
	    إذا (i > 0)
	    {
#جے_تعریف_شدہ FEAT_PROP_POPUP
		إذا (!(State & VREPLACE_FLAG))
		{
		    char_u  *newp;
		    صحيح	    خانہ;

		    newp = alloc(curbuf->b_ml.ml_سطر_len - i);
		    إذا (newp == عدم)
			أرجع FALSE;

		    خانہ = ptr - curbuf->b_ml.ml_سطر_ptr;
		    إذا (خانہ > 0)
			mch_memmove(newp, ptr - خانہ, خانہ);
		    mch_memmove(newp + خانہ, ptr + i,
					   curbuf->b_ml.ml_سطر_len - خانہ - i);

		    إذا (curbuf->b_ml.ml_flags & (ML_سطر_DIRTY | ML_ALLOCATED))
			vim_free(curbuf->b_ml.ml_سطر_ptr);
		    curbuf->b_ml.ml_سطر_ptr = newp;
		    curbuf->b_ml.ml_سطر_len -= i;
		    curbuf->b_ml.ml_سطر_textlen = 0;
		    curbuf->b_ml.ml_flags =
			   (curbuf->b_ml.ml_flags | ML_سطر_DIRTY) & ~ML_خالی_تھاواں;
		}
		وإلا
#جے_انت
		    STRMOVE(ptr, ptr + i);
		// correct replace stack.
		إذا ((State & REPLACE_FLAG) && !(State & VREPLACE_FLAG))
		    لكل (عارضی = i; --عارضی >= 0; )
			replace_join(repl_off);
	    }
#جے_تعریف_شدہ FEAT_NETBEANS_INTG
	    إذا (netbeans_سرگرم())
	    {
		netbeans_removed(curbuf, fpos.lnum, cursor->خانہ, (لما)(i + 1));
		netbeans_inserted(curbuf, fpos.lnum, cursor->خانہ,
							   (char_u *)"\t", 1);
	    }
#جے_انت
	    cursor->خانہ -= i;

	    /*
	     * In MODE_VREPLACE state, we haven't changed anything yet.  Do it
	     * now by backspacing over the changed spacing and then inserting
	     * the new spacing.
	     */
	    إذا (State & VREPLACE_FLAG)
	    {
		// Backspace from real cursor to change_col
		backspace_until_کالم(change_خانہ);

		// Insert each char in saved_line from changed_col to
		// ptr-cursor
		ins_بائٹاں_len(saved_سطر + change_خانہ,
						    cursor->خانہ - change_خانہ);
	    }
	}

	إذا (State & VREPLACE_FLAG)
	    vim_free(saved_سطر);
	curwin->w_p_list = save_list;
    }

    أرجع FALSE;
}

/*
 * Handle CR or NL in insert mode.
 * Return FAIL when out of memory or can't undo.
 */
    صحيح
ins_eol(صحيح c)
{
    صحيح	    i;

    إذا (echeck_abbr(c + ABBR_OFF))
	أرجع OK;
    إذا (stop_arrow() == FAIL)
	أرجع FAIL;
    undisplay_dollar();

    /*
     * Strange Vi behaviour: In Replace mode, typing a NL will not delete the
     * character under the cursor.  Only push a NUL on the replace stack,
     * nothing to put back when the NL is deleted.
     */
    إذا ((State & REPLACE_FLAG) && !(State & VREPLACE_FLAG))
	replace_ڈالو(NUL);

    /*
     * In MODE_VREPLACE state, a NL replaces the rest of the line, and starts
     * replacing the next line, so we push all of the characters left on the
     * line onto the replace stack.  This is not done here though, it is done
     * in open_line().
     */

    // Put cursor on NUL if on the last char and coladd is 1 (happens after
    // CTRL-O).
    إذا (virtual_سرگرم() && curwin->w_cursor.coladd > 0)
	coladvance(getviscol());

#جے_تعریف_شدہ FEAT_RIGHTLEFT
    // NL in reverse insert will always start in the end of
    // current line.
    إذا (revins_on)
	curwin->w_cursor.خانہ += ml_get_cursor_len();
#جے_انت

    Append_منزل_Redobuff(NL_STR);
    i = open_سطر(FORWARD,
	    has_format_option(FO_RET_COMS) ? OPENLINE_DO_COM : 0, old_indent,
	    عدم);
    old_indent = 0;
    can_cindent = TRUE;
#جے_تعریف_شدہ FEAT_FOLDING
    // When inserting a line the cursor line must never be in a closed fold.
    foldOpenCursor();
#جے_انت

    أرجع i;
}

#جے_تعریف_شدہ FEAT_DIGRAPHS
/*
 * Handle digraph in insert mode.
 * Returns character still to be inserted, or NUL when nothing remaining to be
 * done.
 */
    ساكن صحيح
ins_digraph(فراغ)
{
    صحيح	    c;
    صحيح	    cc;
    صحيح	    did_putchar = FALSE;

    pc_حالت = PC_حالت_UNSET;
    إذا (redrawing() && !char_avail())
    {
	// may need to redraw when no more chars available now
	ins_redraw(FALSE);

	edit_putchar('?', TRUE);
	did_putchar = TRUE;
	جوڑو_منزل_showcmd_c(Ctrl_K);
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
    إذا (did_putchar)
	// when the line fits in 'columns' the '?' is at the start of the next
	// line and will not be removed by the redraw
	edit_unputchar();

    إذا (IS_SPECIAL(c) || mod_نقاب)	    // special key
    {
	clear_showcmd();
	داخل_کرو_special(c, TRUE, FALSE);
	أرجع NUL;
    }
    إذا (c != ESC)
    {
	did_putchar = FALSE;
	إذا (redrawing() && !char_avail())
	{
	    // may need to redraw when no more chars available now
	    ins_redraw(FALSE);

	    إذا (char2cells(c) == 1)
	    {
		ins_redraw(FALSE);
		edit_putchar(c, TRUE);
		did_putchar = TRUE;
	    }
	    جوڑو_منزل_showcmd_c(c);
	}
	++no_mapping;
	++allow_کلیدیں;
	cc = plain_vgetc();
	--no_mapping;
	--allow_کلیدیں;
	إذا (did_putchar)
	    // when the line fits in 'columns' the '?' is at the start of the
	    // next line and will not be removed by a redraw
	    edit_unputchar();
	إذا (cc != ESC)
	{
	    Append_منزل_Redobuff((char_u *)CTRL_V_STR);
	    c = digraph_get(c, cc, TRUE);
	    clear_showcmd();
	    أرجع c;
	}
    }
    clear_showcmd();
    أرجع NUL;
}
#جے_انت

/*
 * Handle CTRL-E and CTRL-Y in Insert mode: copy char from other line.
 * Returns the char to be inserted, or NUL if none found.
 */
    صحيح
ins_copychar(linenr_T lnum)
{
    صحيح		    c;
    char_u	    *ptr, *prev_ptr;
    char_u	    *سطر;
    chartabsize_T   cts;

    إذا (lnum < 1 || lnum > curbuf->b_ml.ml_سطر_گنتی)
    {
	vim_beep(BO_COPY);
	أرجع NUL;
    }

    // try to advance to the cursor column
    validate_virtcol();
    سطر = ml_get(lnum);
    prev_ptr = سطر;
    init_chartabsize_arg(&cts, curwin, lnum, 0, سطر, سطر);
    طالما (cts.cts_vcol < curwin->w_virtcol && *cts.cts_ptr != NUL)
    {
	prev_ptr = cts.cts_ptr;
	cts.cts_vcol += lbr_chartabsize_adv(&cts);
    }
    إذا (cts.cts_vcol > curwin->w_virtcol)
	ptr = prev_ptr;
    وإلا
	ptr = cts.cts_ptr;
    clear_chartabsize_arg(&cts);

    c = (*mb_ptr2char)(ptr);
    إذا (c == NUL)
	vim_beep(BO_COPY);
    أرجع c;
}

/*
 * CTRL-Y or CTRL-E typed in Insert mode.
 */
    ساكن صحيح
ins_ctrl_ey(صحيح tc)
{
    صحيح	    c = tc;

    إذا (ctrl_x_منوال_scroll())
    {
	إذا (c == Ctrl_Y)
	    scrolldown_clamp();
	وإلا
	    scrollup_clamp();
	redraw_later(UPD_VALID);
    }
    وإلا
    {
	c = ins_copychar(curwin->w_cursor.lnum + (c == Ctrl_Y ? -1 : 1));
	إذا (c != NUL)
	{
	    لما	tw_save;

	    // The character must be taken literally, insert like it
	    // was typed after a CTRL-V, and pretend 'textwidth'
	    // wasn't set.  Digits, 'o' and 'x' are special after a
	    // CTRL-V, don't use it for these.
	    إذا (c < 256 && !محفوظ_isalnum(c))
		Append_منزل_Redobuff((char_u *)CTRL_V_STR);	// CTRL-V
	    tw_save = curbuf->b_p_tw;
	    curbuf->b_p_tw = -1;
	    داخل_کرو_special(c, TRUE, FALSE);
	    curbuf->b_p_tw = tw_save;
#جے_تعریف_شدہ FEAT_RIGHTLEFT
	    revins_chars++;
	    revins_legal++;
#جے_انت
	    c = Ctrl_V;	// pretend CTRL-V is last character
	    auto_format(FALSE, TRUE);
	}
    }
    أرجع c;
}

/*
 * Get the value that w_virtcol would have when 'list' is off.
 * Unless 'cpo' contains the 'L' flag.
 */
    colnr_T
get_nolist_virtcol(فراغ)
{
    // check validity of cursor in current buffer
    إذا (curwin->w_بفر == عدم
	|| curwin->w_بفر->b_ml.ml_mfp == عدم
	|| curwin->w_cursor.lnum > curwin->w_بفر->b_ml.ml_سطر_گنتی)
	أرجع 0;
    إذا (curwin->w_p_list && vim_strchr(p_cpo, CPO_LISTWM) == عدم)
	أرجع getvcol_nolist(&curwin->w_cursor);
    validate_virtcol();
    أرجع curwin->w_virtcol;
}

#إذا defined(FEAT_EVAL)
/*
 * Handle the InsertCharPre autocommand.
 * "c" is the character that was typed.
 * Return a pointer to allocated memory with the replacement string.
 * Return NULL to continue inserting "c".
 */
    ساكن char_u *
do_داخل_کرو_char_pre(صحيح c)
{
    char_u	*res;
    char_u	buf[MB_MAXBYTES + 1];
    size_t	buflen;
    صحيح		save_State = State;

    // Return quickly when there is nothing to do.
    إذا (!has_insertcharpre())
	أرجع عدم;

    إذا (c == Ctrl_RSB)
	أرجع عدم;

    إذا (has_mbyte)
	buflen = (*mb_char2bytes)(c, buf);
    وإلا
    {
	buf[0] = c;
	buflen = 1;
    }
    buf[buflen] = NUL;

    // Lock the text to avoid weird things from happening.
    ++textlock;
    سیٹ_vim_var_string(VV_CHAR, buf, (صحيح)buflen);  // set v:char

    res = عدم;
    إذا (ins_apply_autocmds(EVENT_INSERTCHARPRE))
    {
	// Get the value of v:char.  It may be empty or more than one
	// character.  Only use it when changed, otherwise continue with the
	// original character to avoid breaking autoindent.
	إذا (STRCMP(buf, get_vim_var_str(VV_CHAR)) != 0)
	    res = vim_strsave(get_vim_var_str(VV_CHAR));
    }

    سیٹ_vim_var_string(VV_CHAR, عدم, -1);  // clear v:char
    --textlock;

    // Restore the State, it may have been changed.
    State = save_State;

    أرجع res;
}
#جے_انت

    صحيح
get_can_cindent(فراغ)
{
    أرجع can_cindent;
}

    فراغ
سیٹ_can_cindent(صحيح val)
{
    can_cindent = val;
}

/*
 * Trigger "event" and take care of fixing undo.
 */
    صحيح
ins_apply_autocmds(event_T event)
{
    varnumber_T	tick = CHANGEDTICK(curbuf);
    صحيح r;

    r = apply_autocmds(event, عدم, عدم, FALSE, curbuf);

    // If u_savesub() was called then we are not prepared to start
    // a new line.  Call u_save() with no contents to fix that.
    // Except when leaving Insert mode.
    إذا (event != EVENT_INSERTLEAVE && tick != CHANGEDTICK(curbuf))
	u_save(curwin->w_cursor.lnum, (linenr_T)(curwin->w_cursor.lnum + 1));

    أرجع r;
}
