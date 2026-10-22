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

#تعریف_کن BACKSPACE_CHAR		    1
#تعریف_کن BACKSPACE_کلمه		    2
#تعریف_کن BACKSPACE_کلمه_NOT_SPACE    3
#تعریف_کن BACKSPACE_سطر		    4

// Set when doing something for completion that may call edit() recursively,
// which is not allowed.
ساكن صحيح	compl_busy = FALSE;


ساكن فراغ ins_ctrl_v(فراغ);
ساكن فراغ درج_special(صحيح, صحيح, صحيح);
ساكن فراغ redo_literal(صحيح c);
ساكن فراغ شروع_arrow_common(pos_T *انتها_درج_pos, صحيح change);
#اگر_تعریف_شده FEAT_SPELL
ساكن فراغ check_spell_redraw(فراغ);
#پایان_اگر
ساكن فراغ stop_درج(pos_T *انتها_درج_pos, صحيح esc, صحيح nomove);
ساكن صحيح  echeck_abbr(صحيح);
ساكن فراغ mb_replace_برداشت_ins(صحيح cc);
ساكن فراغ replace_flush(فراغ);
ساكن فراغ replace_do_bs(صحيح حد_ستون);
ساكن صحيح del_char_after_ستون(صحيح حد_ستون);
ساكن فراغ ins_reg(فراغ);
ساكن فراغ ins_ctrl_g(فراغ);
ساكن فراغ ins_ctrl_hat(فراغ);
ساكن صحيح  ins_esc(دراز *شمار, صحيح cmdchar, صحيح nomove);
#اگر_تعریف_شده FEAT_RIGHTLEFT
ساكن فراغ ins_ctrl_(فراغ);
#پایان_اگر
ساكن صحيح ins_شروع_select(صحيح c);
ساكن فراغ ins_درج(صحيح replaceState);
ساكن فراغ ins_ctrl_o(فراغ);
ساكن فراغ ins_لغزش(صحيح c, صحيح lastc);
ساكن فراغ ins_del(فراغ);
ساكن صحيح  ins_bs(صحيح c, صحيح نما, صحيح *inserted_space_p);
#إذا defined(FEAT_GUI_TABLINE)
ساكن فراغ ins_tabline(صحيح c);
#پایان_اگر
ساكن فراغ ins_چپ(فراغ);
ساكن فراغ ins_home(صحيح c);
ساكن فراغ ins_انتها(صحيح c);
ساكن فراغ ins_s_چپ(فراغ);
ساكن فراغ ins_راست(فراغ);
ساكن فراغ ins_s_راست(فراغ);
ساكن فراغ ins_به_بالا(صحيح startcol);
ساكن فراغ ins_pageup(فراغ);
ساكن فراغ ins_پایین(صحيح startcol);
ساكن فراغ ins_pagedown(فراغ);
#اگر_تعریف_شده FEAT_DND
ساكن فراغ ins_drop(فراغ);
#پایان_اگر
ساكن صحيح  ins_tab(فراغ);
#اگر_تعریف_شده FEAT_DIGRAPHS
ساكن صحيح  ins_digraph(فراغ);
#پایان_اگر
ساكن صحيح  ins_ctrl_ey(صحيح tc);
#إذا defined(FEAT_EVAL)
ساكن char_u *do_درج_char_pre(صحيح c);
#پایان_اگر

ساكن colnr_T	Insstart_textlen;	// length of line when insert started
ساكن colnr_T	Insstart_blank_vcol;	// vcol for first inserted blank
ساكن صحيح	update_Insstart_orig = TRUE; // set Insstart_orig to Insstart

ساكن string_T	آخرین_درج = {عدم, 0};    // the text of the previous insert,
					    // K_SPECIAL and CSI are escaped
ساكن صحيح	آخرین_درج_skip; // nr of chars in front of previous insert
ساكن صحيح	new_درج_skip;  // nr of chars in front of current insert
ساكن صحيح	did_restart_edit;	// "restart_edit" when calling edit()

ساكن صحيح	can_cindent;		// may do cindenting on this line

#اگر_تعریف_شده FEAT_RIGHTLEFT
ساكن صحيح	revins_on;		// reverse insert mode on
ساكن صحيح	revins_chars;		// how much to skip after edit
ساكن صحيح	revins_legal;		// was the last char 'legal'?
ساكن صحيح	revins_scol;		// start column of revins session
#پایان_اگر

ساكن صحيح	ins_need_undo;		// call u_save() before inserting a
					// char.  Set when edit() is called.
					// after that arrow_used is used.

ساكن صحيح	dont_sync_undo = FALSE;	// CTRL-G U prevents syncing undo for
					// the next left/right cursor key

// With 'autocompletedelay' set, arm the delay and let the main loop fire
// Insert-mode autocommands; the popup is shown later on K_COMPLETE_DELAY.
// Otherwise trigger completion right away.
#تعریف_کن TRIGGER_AUTOCOMPLETE()			\
    افعل {					\
	update_screen(UPD_VALID);  /* Show char (deletion) immediately */ \
	بیرون_flush();				\
	ins_compl_enable_autocomplete();	\
	ins_compl_arm_autostart();		\
	إذا (!ins_compl_arm_autocomplete_delay())\
	    اذهب docomplete;			\
    } طالما (0)

#تعریف_کن MAY_TRIGGER_AUTOCOMPLETE(c)				\
    افعل {							\
	إذا (ins_compl_has_autocomplete() && !char_avail()	\
		&& curwin->w_cursor.ستون > 0)			\
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
    دراز	شمار)
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
#اگر_تعریف_شده FEAT_DIFF
    صحيح		old_topfill = -1;
#پایان_اگر
    صحيح		inserted_space = FALSE;     // just inserted a space
    صحيح		replaceState = نما_REPLACE;
    صحيح		nomove = FALSE;		    // don't move cursor on return
#اگر_تعریف_شده FEAT_کار_CHANNEL
    صحيح		cmdchar_todo = cmdchar;
#پایان_اگر
#اگر_تعریف_شده FEAT_CONCEAL
    صحيح		cursor_سطر_was_concealed;
#پایان_اگر
    صحيح		ins_just_started = TRUE;

    // Remember whether editing was restarted after CTRL-O.
    did_restart_edit = restart_edit;

    // sleep before redrawing, needed for "CTRL-O :" that results in an
    // error message
    check_for_delay(TRUE);

    // set Insstart_orig to Insstart
    update_Insstart_orig = TRUE;

#اگر_تعریف_شده HAVE_SANDBOX
    // Don't allow inserting in the sandbox.
    إذا (sandbox != 0)
    {
	emsg(_(e_not_allowed_درون_sandbox));
	أرجع FALSE;
    }
#پایان_اگر
    // Don't allow changes in the buffer while editing the cmdline.  The
    // caller of getcmdline() may get confused.
    // Don't allow recursive insert mode when busy with completion.
    إذا (textlock != 0 || ins_compl_فعال() || compl_busy || pum_visible())
    {
	emsg(_(e_not_allowed_مقصد_change_متن_or_change_پنجره));
	أرجع FALSE;
    }
    ins_compl_clear();	    // clear stuff for CTRL-X mode

    /*
     * Trigger InsertEnter autocommands.  Do not do this for "r<CR>" or "grx".
     */
    إذا (cmdchar != 'r' && cmdchar != 'v')
    {
	pos_T   save_cursor = curwin->w_cursor;

#اگر_تعریف_شده FEAT_EVAL
	إذا (cmdchar == 'R')
	    ptr = (char_u *)"r";
	وإلا إذا (cmdchar == 'V')
	    ptr = (char_u *)"v";
	وإلا
	    ptr = (char_u *)"i";
	مجموعه_vim_var_string(VV_INSERTMODE, ptr, 1);
	مجموعه_vim_var_string(VV_CHAR, عدم, -1);  // clear v:char
#پایان_اگر
	ins_apply_autocmds(EVENT_INSERTENTER);

	// Check for changed highlighting, e.g. for ModeMsg.
	إذا (need_highlight_تغییریافته)
	    highlight_تغییریافته();

	// Make sure the cursor didn't move.  Do call check_cursor_col() in
	// case the text was modified.  Since Insert mode was not started yet
	// a call to check_cursor_col() may move the cursor, especially with
	// the "A" command, thus set State to avoid that. Also check that the
	// line number is still valid (lines may have been deleted).
	// Do not restore if v:char was set to a non-empty string.
	إذا (!EQUAL_POS(curwin->w_cursor, save_cursor)
#اگر_تعریف_شده FEAT_EVAL
		&& *get_vim_var_str(VV_CHAR) == NUL
#پایان_اگر
		&& save_cursor.lnum <= curbuf->b_ml.ml_سطر_شمار)
	{
	    صحيح save_state = State;

	    curwin->w_cursor = save_cursor;
	    State = نما_درج;
	    check_cursor_ستون();
	    State = save_state;
	}
    }

#اگر_تعریف_شده FEAT_CONCEAL
    // Check if the cursor line was concealed before changing State.
    cursor_سطر_was_concealed = curwin->w_p_cole > 0
						&& conceal_cursor_سطر(curwin);
#پایان_اگر

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
	    Insstart.ستون = 0;
    }
    Insstart_textlen = (colnr_T)linetabsize_str(ml_get_curline());
    Insstart_blank_vcol = MAXCOL;
    إذا (!did_ai)
	ai_ستون = 0;

    إذا (cmdchar != NUL && restart_edit == 0)
    {
	ResetRedobuff();
	Append_عدد_مقصد_Redobuff(شمار);
	إذا (cmdchar == 'V' || cmdchar == 'v')
	{
	    // "gR" or "gr" command
	    Append_Char_مقصد_Redobuff('g');
	    Append_Char_مقصد_Redobuff((cmdchar == 'v') ? 'r' : 'R');
	}
	وإلا
	{
	    إذا (cmdchar == K_PS)
		Append_Char_مقصد_Redobuff('a');
	    وإلا
		Append_Char_مقصد_Redobuff(cmdchar);
	    إذا (cmdchar == 'g')		    // "gI" command
		Append_Char_مقصد_Redobuff('I');
	    وإلا إذا (cmdchar == 'r')	    // "r<CR>" command
		شمار = 1;		    // insert only one <CR>
	}
    }

    إذا (cmdchar == 'R')
    {
	State = نما_REPLACE;
    }
    وإلا إذا (cmdchar == 'V' || cmdchar == 'v')
    {
	State = نما_VREPLACE;
	replaceState = نما_VREPLACE;
	orig_سطر_شمار = curbuf->b_ml.ml_سطر_شمار;
	vr_سطرها_تغییریافته = 1;
    }
    وإلا
	State = نما_درج;

    may_trigger_modechanged();
    stop_درج_نما = FALSE;

#اگر_تعریف_شده FEAT_CONCEAL
    // Check if the cursor line needs redrawing after changing State.  If
    // 'concealcursor' is "n" it needs to be redrawn without concealing.
    conceal_check_cursor_سطر(cursor_سطر_was_concealed);
#پایان_اگر

    // Need to position cursor again when on a TAB and when on a char with
    // virtual text.
    إذا (gchar_cursor() == TAB
#اگر_تعریف_شده FEAT_PROP_POPUP
	    || curbuf->b_has_textprop
#پایان_اگر
       )
	curwin->w_valid &= ~(VALID_WROW|VALID_WCOL|VALID_VIRTCOL);

    /*
     * Enable langmap or IME, indicated by 'iminsert'.
     * Note that IME may enabled/disabled without us noticing here, thus the
     * 'iminsert' value may not reflect what is actually used.  It is updated
     * when hitting <Esc>.
     */
    إذا (curbuf->b_p_iminsert == B_IMODE_LMAP)
	State |= نما_LANGMAP;
#اگر_تعریف_شده HAVE_درون_داد_روش
    im_مجموعه_فعال(curbuf->b_p_iminsert == B_IMODE_IM);
#پایان_اگر

    setmouse();
    clear_showcmd();
#اگر_تعریف_شده FEAT_RIGHTLEFT
    // there is no reverse replace mode
    revins_on = (State == نما_درج && p_ri);
    إذا (revins_on)
	undisplay_dollar();
    revins_chars = 0;
    revins_legal = 0;
    revins_scol = -1;
#پایان_اگر
    إذا (!p_ek)
    {
	MAY_WANT_مقصد_LOG_THIS;

	// Disable bracketed paste mode, we won't recognize the escape
	// sequences.
	بیرون_str(T_BD);

	// Disable modifyOtherKeys, keys with modifiers would cause exiting
	// Insert mode.
	بیرون_str_t_TE();
    }

    /*
     * Handle restarting Insert mode.
     * Don't do this for "CTRL-O ." (repeat an insert): In that case we get
     * here with something in the stuff buffer.
     */
    إذا (restart_edit != 0 && stuff_خالی())
    {
	/*
	 * After a paste we consider text typed to be part of the insert for
	 * the pasted text. You can backspace over the pasted text too.
	 */
	إذا (where_paste_started.lnum)
	    arrow_استفاده_شده = FALSE;
	وإلا
	    arrow_استفاده_شده = TRUE;
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
		&& *(ptr = ml_get_curline() + curwin->w_cursor.ستون) != NUL)
	{
	    إذا (ptr[1] == NUL)
		++curwin->w_cursor.ستون;
	    وإلا إذا (has_mbyte)
	    {
		i = (*mb_ptr2len)(ptr);
		إذا (ptr[i] == NUL)
		    curwin->w_cursor.ستون += i;
	    }
	}
	ins_at_eol = FALSE;
    }
    وإلا
	arrow_استفاده_شده = FALSE;

    // we are in insert mode now, don't need to start it anymore
    need_شروع_insertmode = FALSE;

    // Need to save the line for undo before inserting the first char.
    ins_need_undo = TRUE;

    where_paste_started.lnum = 0;
    can_cindent = TRUE;
#اگر_تعریف_شده FEAT_FOLDING
    // The cursor line is not in a closed fold, unless 'insertmode' is set or
    // restarting.
    إذا (!p_im && did_restart_edit == 0)
	foldOpenCursor();
#پایان_اگر

    /*
     * If 'showmode' is set, show the current (insert/replace/..) mode.
     * A warning message for changing a readonly file is given here, before
     * actually changing anything.  It's put after the mode, if any.
     */
    i = 0;
    إذا (p_smd && پیام_silent == 0)
	i = showmode();

    إذا (!p_im && did_restart_edit == 0)
	change_warning(i == 0 ? 0 : i + 1);

#اگر_تعریف_شده CURSOR_SHAPE
    ui_cursor_shape();		// may show different cursor shape
#پایان_اگر
#اگر_تعریف_شده FEAT_DIGRAPHS
    do_digraph(-1);		// clear digraphs
#پایان_اگر

    /*
     * Get the current length of the redo buffer, those characters have to be
     * skipped if we want to get to the inserted characters.
     */
    string_T inserted = get_inserted();
    new_درج_skip = (صحيح)inserted.درازی;
    إذا (inserted.string != عدم)
	vim_free(inserted.string);

    old_indent = 0;

    /*
     * Main loop in Insert mode: repeat until Insert mode is left.
     */
    لكل (;;)
    {
#اگر_تعریف_شده FEAT_RIGHTLEFT
	إذا (!revins_legal)
	    revins_scol = -1;	    // reset on illegal motions
	وإلا
	    revins_legal = 0;
#پایان_اگر
	إذا (arrow_استفاده_شده)	    // don't repeat insert when arrow key used
	    شمار = 0;

	إذا (update_Insstart_orig)
	    Insstart_orig = Insstart;

	إذا (stop_درج_نما && !ins_compl_فعال())
	{
	    // ":stopinsert" used or 'insertmode' reset
	    شمار = 0;
	    اذهب doESCkey;
	}

	// set curwin->w_curswant for next K_DOWN or K_UP
	إذا (!arrow_استفاده_شده)
	    curwin->w_مجموعه_curswant = true;

	// If there is no typeahead may check for timestamps (e.g., for when a
	// menu invoked a shell command).
	إذا (stuff_خالی())
	{
	    did_check_timestamps = FALSE;
	    إذا (need_check_timestamps)
		check_timestamps(FALSE);
	}

	/*
	 * When emsg() was called msg_scroll will have been set.
	 */
	پیام_scroll = FALSE;

#اگر_تعریف_شده FEAT_GUI
	// When 'mousefocus' is set a mouse movement may have taken us to
	// another window.  "need_mouse_correct" may then be set because of an
	// autocommand.
	إذا (need_mouse_correct)
	    gui_mouse_correct();
#پایان_اگر

#اگر_تعریف_شده FEAT_FOLDING
	// Open fold at the cursor line, according to 'foldopen'.
	إذا (fdo_flags & FDO_درج)
	    foldOpenCursor();
	// Close folds where the cursor isn't, according to 'foldclose'
	إذا (!char_avail())
	    foldCheckClose();
#پایان_اگر

#اگر_تعریف_شده FEAT_کار_CHANNEL
	إذا (bt_prompt(curbuf))
	{
	    init_prompt(cmdchar_todo);
	    cmdchar_todo = NUL;
	}
#پایان_اگر

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
	إذا (curbuf->b_mod_مجموعه
		&& curwin->w_p_wrap
		&& !curwin->w_p_sms
		&& !did_backspace
		&& curwin->w_topline == old_topline
#اگر_تعریف_شده FEAT_DIFF
		&& curwin->w_topfill == old_topfill
#پایان_اگر
		&& شمار <= 1
		)
	{
	    mincol = curwin->w_wcol;
	    validate_cursor_ستون();

	    إذا (
#اگر_تعریف_شده FEAT_VARTABS
		curwin->w_wcol < mincol - tabstop_at(get_nolist_virtcol(),
						     curbuf->b_p_ts,
						     curbuf->b_p_vts_array,
						     FALSE)
#وإلا
		(صحيح)curwin->w_wcol < mincol - curbuf->b_p_ts
#پایان_اگر
		    && curwin->w_wrow ==
				   curwin->w_ارتفاع - 1 - get_scrolloff_مقدار()
		    && (curwin->w_cursor.lnum != curwin->w_topline
#اگر_تعریف_شده FEAT_DIFF
			|| curwin->w_topfill > 0
#پایان_اگر
		    ))
	    {
#اگر_تعریف_شده FEAT_DIFF
		إذا (curwin->w_topfill > 0)
		    --curwin->w_topfill;
		وإلا
#پایان_اگر
#اگر_تعریف_شده FEAT_FOLDING
		إذا (hasFolding(curwin->w_topline, عدم, &old_topline))
		    مجموعه_topline(curwin, old_topline + 1);
		وإلا
#پایان_اگر
		    مجموعه_topline(curwin, curwin->w_topline + 1);
	    }
	}

	// May need to adjust w_topline to show the cursor.
	إذا (شمار <= 1)
	    update_topline();

	did_backspace = FALSE;

	إذا (شمار <= 1)
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
	إذا (شمار <= 1)
	    update_curswant();
	old_topline = curwin->w_topline;
#اگر_تعریف_شده FEAT_DIFF
	old_topfill = curwin->w_topfill;
#پایان_اگر

#اگر_تعریف_شده USE_ON_FLY_SCROLL
	dont_scroll = FALSE;		// allow scrolling here
#پایان_اگر
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
			&& curwin->w_cursor.ستون > 0)
		{
		    c = char_before_cursor();
		    إذا (vim_isprintc(c))
		    {
			ins_compl_enable_autocomplete();
			ins_compl_arm_autostart();
			ins_compl_init_get_longest();
#اگر_تعریف_شده FEAT_RIGHTLEFT
			إذا (p_hkmap)
			    c = hkmap(c);		// Hebrew mode mapping
#پایان_اگر
			// Defer until the delay expires (K_COMPLETE_DELAY), or
			// trigger now when no delay is in effect.
			إذا (!ins_compl_arm_autocomplete_delay())
			    اذهب docomplete;
		    }
		}
	    }

	    افعل
	    {
		c = امن_vgetc();

		إذا (stop_درج_نما
#اگر_تعریف_شده FEAT_TERMINAL
			|| (c == K_IGNORE && جمله_use_loop())
#پایان_اگر
		   )
		{
		    // Insert mode ended, possibly from a callback, or a timer
		    // must have opened a terminal window.
		    إذا (c != K_IGNORE && c != K_NOP)
			vungetc(c);
		    شمار = 0;

		    إذا (!bt_prompt(curwin->w_میانگیر)
#اگر_تعریف_شده FEAT_TERMINAL
			    && !bt_terminal(curwin->w_میانگیر)
#پایان_اگر
			    && stop_درج_نما)
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
	    إذا (!ins_compl_فعال())
		ins_compl_disable_autocomplete();
	}

#اگر_تعریف_شده FEAT_RIGHTLEFT
	إذا (p_hkmap && کلید_Typed)
	    c = hkmap(c);		// Hebrew mode mapping
#پایان_اگر

	// If the window was made so small that nothing shows, make it at least
	// one line and one column when typing.
	إذا (کلید_Typed && !کلید_Stuffed)
	    win_ensure_اندازه();

	/*
	 * Special handling of keys while the popup menu is visible or wanted
	 * and the cursor is still in the completed word.  Only when there is
	 * a match, skip this when no matches were found.
	 */
	إذا (ins_compl_فعال() && curwin->w_cursor.ستون >= ins_compl_ستون()
		&& ins_compl_has_shown_تطابق() && pum_wanted())
	{
	    // BS: Delete one character from "compl_leader".
	    إذا ((c == K_BS || c == Ctrl_H)
			&& curwin->w_cursor.ستون > ins_compl_ستون()
			&& (c = ins_compl_bs()) == NUL)
		استمر;

	    // When no match was selected or it was edited.
	    إذا (!ins_compl_استفاده_شده_تطابق())
	    {
		// CTRL-L: Add one character from the current match to
		// "compl_leader".  Except when at the original match and
		// there is nothing to add, CTRL-L works like CTRL-P then.
		إذا (c == Ctrl_L
			&& (!ctrl_x_نما_سطر_or_eval()
			    || ins_compl_long_shown_تطابق()))
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
		    char_u *str = do_درج_char_pre(c);
		    char_u *p;

		    إذا (str != عدم)
		    {
			لكل (p = str; *p != NUL; MB_PTR_ADV(p))
			    ins_compl_addleader(PTR2CHAR(p));
			vim_free(str);
		    }
		    وإلا
#پایان_اگر
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
			    && !ins_compl_is_تطابق_selected())
		    {
			ins_compl_درج(FALSE, TRUE);
			ins_compl_init_get_longest();
			استمر;
		    }
		    وإلا
			ins_compl_درج(FALSE, FALSE);
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
	    ++allow_کلیدها;
	    c = plain_vgetc();
	    --no_mapping;
	    --allow_کلیدها;
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
		شمار = 0;
		اذهب doESCkey;
	    }
	}

#اگر_تعریف_شده FEAT_DIGRAPHS
	c = do_digraph(c);
#پایان_اگر

	إذا ((c == Ctrl_V || c == Ctrl_Q) && ctrl_x_نما_cmdline())
	    اذهب docomplete;
	إذا (c == Ctrl_V || c == Ctrl_Q)
	{
	    ins_ctrl_v();
	    c = Ctrl_V;	// pretend CTRL-V is last typed character
	    استمر;
	}

	إذا (cindent_on() && ctrl_x_نما_none())
	{
	    // A key name preceded by a bang means this key is not to be
	    // inserted.  Skip ahead to the re-indenting below.
	    // A key name preceded by a star means that indenting has to be
	    // done before inserting the key.
	    سطر_is_white = inindent(0);
	    إذا (درون_cinkeys(c, '!', سطر_is_white))
		اذهب force_cindent;
	    إذا (can_cindent && درون_cinkeys(c, '*', سطر_is_white)
							&& stop_arrow() == OK)
		do_c_عبارت_indent();
	}

#اگر_تعریف_شده FEAT_RIGHTLEFT
	إذا (curwin->w_p_rl)
	    بدل (c)
	    {
		حالة K_چپ:	c = K_راست; اكسر;
		حالة K_S_چپ:	c = K_S_راست; اكسر;
		حالة K_C_چپ:	c = K_C_راست; اكسر;
		حالة K_راست:	c = K_چپ; اكسر;
		حالة K_S_راست: c = K_S_چپ; اكسر;
		حالة K_C_راست: c = K_C_چپ; اكسر;
	    }
#پایان_اگر

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
		cmdwin_نتیجه = K_IGNORE;
		got_int = FALSE; // don't stop executing autocommands et al.
		nomove = TRUE;
		اذهب doESCkey;
	    }
#اگر_تعریف_شده FEAT_کار_CHANNEL
	    إذا (c == Ctrl_C && bt_prompt(curbuf))
	    {
		إذا (invoke_prompt_وقفه())
		{
		    إذا (!bt_prompt(curbuf))
			// buffer changed to a non-prompt buffer, get out of
			// Insert mode
			اذهب doESCkey;
		    اكسر;
		}
	    }
#پایان_اگر

#اگر_تعریف_شده UNIX
do_intr:
#پایان_اگر
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

	    إذا (ins_esc(&شمار, cmdchar, nomove))
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
		إذا (!char_avail() && curbuf->b_آخرین_changedtick_i == CHANGEDTICK(curbuf))
		    curbuf->b_آخرین_changedtick = CHANGEDTICK(curbuf);
		أرجع (c == Ctrl_O);
	    }
	    استمر;

	حالة Ctrl_Z:	// suspend when 'insertmode' set
	    إذا (!p_im)
		اذهب normalchar;	// insert CTRL-Z as normal char
	    do_cmdline_cmd((char_u *)"stop");
#اگر_تعریف_شده CURSOR_SHAPE
	    ui_cursor_shape();		// may need to update cursor shape
#پایان_اگر
	    استمر;

	حالة Ctrl_O:	// execute one command
#اگر_تعریف_شده FEAT_COMPL_FUNC
	    إذا (ctrl_x_نما_omni())
		اذهب docomplete;
#پایان_اگر
	    إذا (echeck_abbr(Ctrl_O + ABBR_OFF))
		اكسر;
	    ins_ctrl_o();

	    // don't move the cursor left when 'virtualedit' has "onemore".
	    إذا (get_ve_flags() & VE_ONEMORE)
	    {
		ins_at_eol = FALSE;
		nomove = TRUE;
	    }
	    شمار = 0;
	    اذهب doESCkey;

	حالة K_INS:	// toggle insert/replace mode
	حالة K_KINS:
	    ins_درج(replaceState);
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

#اگر_تعریف_شده FEAT_NETBEANS_INTG
	حالة K_F21:	// NetBeans command
	    ++no_mapping;		// don't map the next key hits
	    i = plain_vgetc();
	    --no_mapping;
	    netbeans_keycommand(i);
	    اكسر;
#پایان_اگر

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
	    إذا (ctrl_x_نما_ثبات() && !ins_compl_فعال())
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

#اگر_تعریف_شده FEAT_RIGHTLEFT
	حالة Ctrl__:	// switch between languages
	    إذا (!p_ari)
		اذهب normalchar;
	    ins_ctrl_();
	    اكسر;
#پایان_اگر

	حالة Ctrl_D:	// Make indent one shiftwidth smaller.
#إذا defined(FEAT_پیدا_کن_ID)
	    إذا (ctrl_x_نما_مسیر_تعریف_میکند())
		اذهب docomplete;
#پایان_اگر
	    // FALLTHROUGH

	حالة Ctrl_T:	// Make indent one shiftwidth greater.
	    إذا (c == Ctrl_T && ctrl_x_نما_thesaurus())
	    {
		إذا (has_compl_option(FALSE))
		    اذهب docomplete;
		اكسر;
	    }

	    ins_لغزش(c, lastc);
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
#اگر_تعریف_شده FEAT_کار_CHANNEL
	    إذا (bt_prompt(curbuf) && (mod_ماسک & MOD_ماسک_لغزش) == 0)
	    {
		// In a prompt window CTRL-W is used for window commands.
		// Use Shift-CTRL-W to delete a word.
		stuffcharReadbuff(Ctrl_W);
		restart_edit = 'A';
		nomove = TRUE;
		شمار = 0;
		اذهب doESCkey;
	    }
#پایان_اگر
	    did_backspace = ins_bs(c, BACKSPACE_کلمه, &inserted_space);
	    auto_format(FALSE, TRUE);
	    إذا (did_backspace)
		MAY_TRIGGER_AUTOCOMPLETE(c);
	    اكسر;

	حالة Ctrl_U:	// delete all inserted text in current line
#اگر_تعریف_شده FEAT_COMPL_FUNC
	    // CTRL-X CTRL-U completes with 'completefunc'.
	    إذا (ctrl_x_نما_function())
		اذهب docomplete;
#پایان_اگر
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
	    ins_mousescroll(MSCR_پایین);
	    اكسر;

	حالة K_MOUSEUP:	// Default action for scroll wheel down: scroll down
	    ins_mousescroll(MSCR_به_بالا);
	    اكسر;

	حالة K_MOUSELEFT: // Scroll wheel left
	    ins_mousescroll(MSCR_چپ);
	    اكسر;

	حالة K_MOUSERIGHT: // Scroll wheel right
	    ins_mousescroll(MSCR_راست);
	    اكسر;

	حالة K_PS:
	    bracketed_paste(PASTE_درج, FALSE, عدم);
	    إذا (cmdchar == K_PS)
		// invoked from normal mode, bail out
		اذهب doESCkey;
	    اكسر;
	حالة K_PE:
	    // Got K_PE without K_PS, ignore.
	    اكسر;

#اگر_تعریف_شده FEAT_GUI_TABLINE
	حالة K_TABLINE:
	حالة K_TABMENU:
	    ins_tabline(c);
	    اكسر;
#پایان_اگر

	حالة K_IGNORE:	// Something mapped to nothing
	    اكسر;

	حالة K_COMMAND:		    // <Cmd>command<CR>
	حالة K_SCRIPT_COMMAND:	    // <ScriptCmd>command<CR>
	    {
		bufref_T    save_curbuf;
		varnumber_T tick = CHANGEDTICK(curbuf);

		مجموعه_bufref(&save_curbuf, curbuf);
		do_cmdkey_command(c, 0);

#اگر_تعریف_شده FEAT_TERMINAL
		إذا (جمله_use_loop())
		    // Started a terminal that gets the input, exit Insert mode.
		    اذهب doESCkey;
#پایان_اگر
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
		    || curwin->w_cursor.ستون == 0)
		اكسر;
	    c = char_before_cursor();
	    إذا (!vim_isprintc(c))
		اكسر;
	    // The completion may have been cleared while waiting, so re-enable
	    // autocomplete to match a zero delay.
	    ins_compl_enable_autocomplete();
	    ins_compl_arm_autostart();
	    اذهب docomplete;

#اگر_تعریف_شده FEAT_GUI_MSWIN
	    // On MS-Windows ignore <M-F4>, we get it when closing the window
	    // was cancelled.
	حالة K_F4:
	    إذا (mod_ماسک != MOD_ماسک_ALT)
		اذهب normalchar;
	    اكسر;
#پایان_اگر

#اگر_تعریف_شده FEAT_GUI
	حالة K_VER_SCROLLBAR:
	    ins_scroll();
	    اكسر;

	حالة K_HOR_SCROLLBAR:
	    ins_horscroll();
	    اكسر;
#پایان_اگر

	حالة K_HOME:	// <Home>
	حالة K_KHOME:
	حالة K_S_HOME:
	حالة K_C_HOME:
	    ins_home(c);
	    اكسر;

	حالة K_انتها:	// <End>
	حالة K_KEND:
	حالة K_S_انتها:
	حالة K_C_انتها:
	    ins_انتها(c);
	    اكسر;

	حالة K_چپ:	// <Left>
	    إذا (mod_ماسک & (MOD_ماسک_لغزش|MOD_ماسک_CTRL))
		ins_s_چپ();
	    وإلا
		ins_چپ();
	    اكسر;

	حالة K_S_چپ:	// <S-Left>
	حالة K_C_چپ:
	    ins_s_چپ();
	    اكسر;

	حالة K_راست:	// <Right>
	    إذا (mod_ماسک & (MOD_ماسک_لغزش|MOD_ماسک_CTRL))
		ins_s_راست();
	    وإلا
		ins_راست();
	    اكسر;

	حالة K_S_راست:	// <S-Right>
	حالة K_C_راست:
	    ins_s_راست();
	    اكسر;

	حالة K_به_بالا:	// <Up>
	    إذا (pum_visible())
		اذهب docomplete;
	    إذا (mod_ماسک & MOD_ماسک_لغزش)
		ins_pageup();
	    وإلا
		ins_به_بالا(FALSE);
	    اكسر;

	حالة K_S_به_بالا:	// <S-Up>
	حالة K_PAGEUP:
	حالة K_KPAGEUP:
	    إذا (pum_visible())
	    {
#اگر_تعریف_شده FEAT_PROP_POPUP
		// CTRL-SHIFT-<Up> scrolls the info popup up a line,
		// CTRL-SHIFT-<PageUp> a page.  Shift is folded into K_S_UP but
		// stays in mod_mask for PageUp, hence the asymmetric check.
		إذا (c == K_S_به_بالا ? (mod_ماسک & MOD_ماسک_CTRL)
			: ((mod_ماسک & MOD_ماسک_CTRL)
			    && (mod_ماسک & MOD_ماسک_لغزش)))
		{
		    popup_scroll_info(-1, c != K_S_به_بالا);
		    اكسر;
		}
#پایان_اگر
		اذهب docomplete;
	    }
	    ins_pageup();
	    اكسر;

	حالة K_پایین:	// <Down>
	    إذا (pum_visible())
		اذهب docomplete;
	    إذا (mod_ماسک & MOD_ماسک_لغزش)
		ins_pagedown();
	    وإلا
		ins_پایین(FALSE);
	    اكسر;

	حالة K_S_پایین:	// <S-Down>
	حالة K_PAGEDOWN:
	حالة K_KPAGEDOWN:
	    إذا (pum_visible())
	    {
#اگر_تعریف_شده FEAT_PROP_POPUP
		// CTRL-SHIFT-<Down>/<PageDown> scroll the info popup down.
		إذا (c == K_S_پایین ? (mod_ماسک & MOD_ماسک_CTRL)
			: ((mod_ماسک & MOD_ماسک_CTRL)
			    && (mod_ماسک & MOD_ماسک_لغزش)))
		{
		    popup_scroll_info(1, c != K_S_پایین);
		    اكسر;
		}
#پایان_اگر
		اذهب docomplete;
	    }
	    ins_pagedown();
	    اكسر;

#اگر_تعریف_شده FEAT_DND
	حالة K_DROP:	// drag-n-drop event
	    ins_drop();
	    اكسر;
#پایان_اگر

	حالة K_S_TAB:	// When not mapped, use like a normal TAB
	    c = TAB;
	    // FALLTHROUGH

	حالة TAB:	// TAB or Complete patterns along path
#إذا defined(FEAT_پیدا_کن_ID)
	    إذا (ctrl_x_نما_مسیر_patterns())
		اذهب docomplete;
#پایان_اگر
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
#پایان_اگر
	    إذا (cmdwin_type != 0)
	    {
		// Execute the command in the cmdline window.
		cmdwin_نتیجه = CAR;
		اذهب doESCkey;
	    }
#اگر_تعریف_شده FEAT_کار_CHANNEL
	    إذا (bt_prompt(curbuf))
	    {
		invoke_prompt_callback();
		إذا (!bt_prompt(curbuf))
		    // buffer changed to a non-prompt buffer, get out of
		    // Insert mode
		    اذهب doESCkey;
		اكسر;
	    }
#پایان_اگر
	    إذا (ins_eol(c) == FAIL && !p_im)
		اذهب doESCkey;	    // out of memory
	    auto_format(FALSE, FALSE);
	    inserted_space = FALSE;
	    اكسر;

	حالة Ctrl_K:	    // digraph or keyword completion
	    إذا (ctrl_x_نما_dictionary())
	    {
		إذا (has_compl_option(TRUE))
		    اذهب docomplete;
		اكسر;
	    }
#اگر_تعریف_شده FEAT_DIGRAPHS
	    c = ins_digraph();
	    إذا (c == NUL)
		اكسر;
#پایان_اگر
	    اذهب normalchar;

	حالة Ctrl_X:	// Enter CTRL-X mode
	    ins_ctrl_x();
	    اكسر;

	حالة Ctrl_RSB:	// Tag name completion after ^X
	    إذا (!ctrl_x_نما_برچسبها())
		اذهب normalchar;
	    اذهب docomplete;

	حالة Ctrl_F:	// File name completion after ^X
	    إذا (!ctrl_x_نما_files())
		اذهب normalchar;
	    اذهب docomplete;

	حالة 's':	// Spelling completion after ^X
	حالة Ctrl_S:
	    إذا (!ctrl_x_نما_spell())
		اذهب normalchar;
	    اذهب docomplete;

	حالة Ctrl_L:	// Whole line completion after ^X
	    إذا (!ctrl_x_نما_کامل_سطر())
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
#اگر_تعریف_شده FEAT_PROP_POPUP
	    // CTRL-SHIFT-P/N scroll the info popup one line.
	    إذا (pum_visible() && (mod_ماسک & MOD_ماسک_لغزش)
		    && (c == Ctrl_P || c == Ctrl_N))
	    {
		popup_scroll_info(c == Ctrl_P ? -1 : 1, false);
		اكسر;
	    }
#پایان_اگر
	    // if 'complete' is empty then plain ^P is no longer special,
	    // but it is under other ^X modes
	    إذا (*curbuf->b_p_cpt == NUL
		    && (ctrl_x_نما_نرمال() || ctrl_x_نما_کامل_سطر())
		    && !compl_وضعیت_local())
		اذهب normalchar;

docomplete:
	    ins_compl_clear_autocomplete_delay();
	    compl_busy = TRUE;
#اگر_تعریف_شده FEAT_FOLDING
	    disable_fold_update++;  // don't redraw folds here
#پایان_اگر
	    إذا (ins_complete(c, TRUE) == FAIL)
		compl_وضعیت_clear();
#اگر_تعریف_شده FEAT_FOLDING
	    disable_fold_update--;
#پایان_اگر
	    compl_busy = FALSE;
	    can_si = may_do_si(); // allow smartindenting
	    اكسر;

	حالة Ctrl_Y:	// copy from previous line or scroll down
	حالة Ctrl_E:	// copy from next line	   or scroll up
	    c = ins_ctrl_ey(c);
	    اكسر;

	افتراضي:
#اگر_تعریف_شده UNIX
	    إذا (c == intr_char)		// special interrupt char
		اذهب do_intr;
#پایان_اگر

normalchar:
	    /*
	     * Insert a normal character.
	     */
#إذا defined(FEAT_EVAL)
	    إذا (!p_paste)
	    {
		// Trigger InsertCharPre.
		char_u *str = do_درج_char_pre(c);
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
			Append_مقصد_Redobuff_Lit(str, -1);
		    }
		    vim_free(str);
		    c = NUL;
		}

		// If the new value is already inserted or an empty string
		// then don't insert any character.
		إذا (c == NUL)
		    اكسر;
	    }
#پایان_اگر
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
		درج_special(c, FALSE, FALSE);
#اگر_تعریف_شده FEAT_RIGHTLEFT
		revins_legal++;
		revins_chars++;
#پایان_اگر
	    }

	    auto_format(FALSE, TRUE);

#اگر_تعریف_شده FEAT_FOLDING
	    // When inserting a character the cursor line must never be in a
	    // closed fold.
	    foldOpenCursor();
#پایان_اگر
	    // Trigger autocompletion
	    إذا (ins_compl_has_autocomplete() && !char_avail()
		    && vim_isprintc(c))
		TRIGGER_AUTOCOMPLETE();

	    اكسر;
	}   // end of switch (c)

	// If typed something may trigger CursorHoldI again; K_COMPLETE_DELAY is
	// injected, not typed.
	إذا (c != K_CURSORHOLD && c != K_COMPLETE_DELAY
#اگر_تعریف_شده FEAT_COMPL_FUNC
		// but not in CTRL-X mode, a script can't restore the state
		&& ctrl_x_نما_نرمال()
#پایان_اگر
	       )
	    did_cursorhold = FALSE;

	// Check if we need to cancel completion mode because the window
	// or tab page was changed
	إذا (ins_compl_فعال() && !ins_compl_win_فعال(curwin))
	    ins_compl_cancel();

	// If the cursor was moved we didn't just insert a space
	إذا (arrow_استفاده_شده)
	    inserted_space = FALSE;

	إذا (can_cindent && cindent_on() && ctrl_x_نما_نرمال())
	{
force_cindent:
	    /*
	     * Indent now if a key was typed that is in 'cinkeys'.
	     */
	    إذا (درون_cinkeys(c, ' ', سطر_is_white))
	    {
		إذا (stop_arrow() == OK)
		    // re-indent the current line
		    do_c_عبارت_indent();
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
#اگر_تعریف_شده FEAT_CONCEAL
    linenr_T	conceal_old_cursor_سطر = 0;
    linenr_T	conceal_new_cursor_سطر = 0;
    صحيح		conceal_update_سطرها = FALSE;
#پایان_اگر

    إذا (char_avail())
	أرجع;

    // Trigger CursorMoved if the cursor moved.  Not when the popup menu is
    // visible, the command might delete it.
    إذا (ready && (has_cursormovedI()
#اگر_تعریف_شده FEAT_PROP_POPUP
		|| popup_visible
#پایان_اگر
#إذا defined(FEAT_CONCEAL)
		|| curwin->w_p_cole > 0
#پایان_اگر
		)
	    && !EQUAL_POS(آخرین_cursormoved, curwin->w_cursor)
	    && !pum_visible())
    {
#اگر_تعریف_شده FEAT_SYN_HL
	// Need to update the screen first, to make sure syntax
	// highlighting is correct after making a change (e.g., inserting
	// a "(".  The autocommand may also require a redraw, so it's done
	// again below, unfortunately.
	إذا (syntax_present(curwin) && must_redraw)
	    update_screen(0);
#پایان_اگر
	إذا (has_cursormovedI())
	{
	    // Make sure curswant is correct, an autocommand may call
	    // getcurpos().
	    update_curswant();
	    ins_apply_autocmds(EVENT_CURSORMOVEDI);
	}
#اگر_تعریف_شده FEAT_PROP_POPUP
	إذا (popup_visible)
	    popup_check_cursor_pos();
#پایان_اگر
#اگر_تعریف_شده FEAT_CONCEAL
	إذا (curwin->w_p_cole > 0)
	{
	    conceal_old_cursor_سطر = آخرین_cursormoved.lnum;
	    conceal_new_cursor_سطر = curwin->w_cursor.lnum;
	    conceal_update_سطرها = TRUE;
	}
#پایان_اگر
	آخرین_cursormoved = curwin->w_cursor;
    }

    // Trigger TextChangedI if b_changedtick_i differs.
    إذا (ready && has_textchangedI()
	    && curbuf->b_آخرین_changedtick_i != CHANGEDTICK(curbuf)
	    && !pum_visible())
    {
	aco_save_T	aco;
	varnumber_T	tick = CHANGEDTICK(curbuf);

	// Save and restore curwin and curbuf, in case the autocmd changes
	// them.
	aucmd_prepbuf(&aco, curbuf);
	apply_autocmds(EVENT_TEXTCHANGEDI, عدم, عدم, FALSE, curbuf);
	aucmd_restbuf(&aco);
	curbuf->b_آخرین_changedtick_i = CHANGEDTICK(curbuf);
	إذا (tick != CHANGEDTICK(curbuf))  // see ins_apply_autocmds()
	    u_save(curwin->w_cursor.lnum,
					(linenr_T)(curwin->w_cursor.lnum + 1));
    }

    // Trigger TextChangedP if b_changedtick_pum differs. When the popupmenu
    // closes TextChangedI will need to trigger for backwards compatibility,
    // thus use different b_last_changedtick* variables.
    إذا (ready && has_textchangedP()
	    && curbuf->b_آخرین_changedtick_pum != CHANGEDTICK(curbuf)
	    && pum_visible())
    {
	aco_save_T	aco;
	varnumber_T	tick = CHANGEDTICK(curbuf);

	// Save and restore curwin and curbuf, in case the autocmd changes
	// them.
	aucmd_prepbuf(&aco, curbuf);
	apply_autocmds(EVENT_TEXTCHANGEDP, عدم, عدم, FALSE, curbuf);
	aucmd_restbuf(&aco);
	curbuf->b_آخرین_changedtick_pum = CHANGEDTICK(curbuf);
	إذا (tick != CHANGEDTICK(curbuf))  // see ins_apply_autocmds()
	    u_save(curwin->w_cursor.lnum,
					(linenr_T)(curwin->w_cursor.lnum + 1));
    }

    إذا (ready)
	may_trigger_win_scrolled_resized();

    // Trigger SafeState if nothing is pending.
    may_trigger_safestate(ready
	    && !ins_compl_فعال()
	    && !pum_visible());

#إذا defined(FEAT_CONCEAL)
    إذا ((conceal_update_سطرها
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
#پایان_اگر
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
    Append_مقصد_Redobuff((char_u *)CTRL_V_STR);	// CTRL-V

    جمع_کن_مقصد_showcmd_c(Ctrl_V);

    // Do not change any modifyOtherKeys ESC sequence to a normal key for
    // CTRL-SHIFT-V.
    c = get_literal(mod_ماسک & MOD_ماسک_لغزش);
    إذا (did_putchar)
	// when the line fits in 'columns' the '^' is at the start of the next
	// line and will not removed by the redraw
	edit_unputchar();
    clear_showcmd();

    درج_special(c, FALSE, TRUE);
#اگر_تعریف_شده FEAT_RIGHTLEFT
    revins_chars++;
    revins_legal++;
#پایان_اگر
}

/*
 * After getting an ESC or CSI for a literal key: If the typeahead buffer
 * contains a modifyOtherKeys sequence then decode it and return the result.
 * Otherwise return "c".
 * Note that this doesn't wait for characters, they must be in the typeahead
 * buffer already.
 */
    ساكن صحيح
decode_Modify_Other_کلیدها(صحيح c)
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
#پایان_اگر

	    mod_ماسک = kitty_no_mods ? 0 : decode_modifiers(arg[!form]);
	    c = ادغام_modify_Other_کلیدها(arg[form], &mod_ماسک);
	}
    }

    أرجع c;
}

/*
 * Put a character directly onto the screen.  It's not stored in a buffer.
 * Used while handling CTRL-K, CTRL-V, etc. in Insert mode.
 */
ساكن صحيح  pc_وضعیت;
#تعریف_کن PC_وضعیت_UNSET	0	// pc_bytes was not set
#تعریف_کن PC_وضعیت_راست	1	// right half of double-wide char
#تعریف_کن PC_وضعیت_چپ	2	// left half of double-wide char
#تعریف_کن PC_وضعیت_مجموعه	3	// pc_bytes was filled
ساكن char_u pc_بایتها[MB_MAXBYTES + 1]; // saved bytes
ساكن صحيح  pc_attr;
ساكن صحيح  pc_ردیف;
ساكن صحيح  pc_ستون;

    فراغ
edit_putchar(صحيح c, صحيح highlight)
{
    صحيح	    attr;

    إذا (Screen_سطرها == عدم)
	أرجع;

    update_topline();	// just in case w_topline isn't valid
    validate_cursor();
    إذا (highlight)
	attr = HL_ATTR(HLF_8);
    وإلا
	attr = 0;
    pc_ردیف = W_WINROW(curwin) + curwin->w_wrow;
    pc_ستون = curwin->w_wincol;
    pc_وضعیت = PC_وضعیت_UNSET;
#اگر_تعریف_شده FEAT_RIGHTLEFT
    إذا (curwin->w_p_rl)
    {
	pc_ستون += curwin->w_عرض - 1 - curwin->w_wcol;
	إذا (has_mbyte)
	{
	    صحيح fix_ستون = mb_fix_ستون(pc_ستون, pc_ردیف);

	    إذا (fix_ستون != pc_ستون)
	    {
		screen_putchar(' ', pc_ردیف, fix_ستون, attr);
		--curwin->w_wcol;
		pc_وضعیت = PC_وضعیت_راست;
	    }
	}
    }
    وإلا
#پایان_اگر
    {
	pc_ستون += curwin->w_wcol;
	إذا (mb_lefthalve(pc_ردیف, pc_ستون))
	    pc_وضعیت = PC_وضعیت_چپ;
    }

    // save the character to be able to put it back
    إذا (pc_وضعیت == PC_وضعیت_UNSET)
    {
	screen_getbytes(pc_ردیف, pc_ستون, pc_بایتها, &pc_attr);
	pc_وضعیت = PC_وضعیت_مجموعه;
    }
    screen_putchar(c, pc_ردیف, pc_ستون, attr);
}

#إذا defined(FEAT_کار_CHANNEL)
/*
 * Set the insert start position for when using a prompt buffer.
 */
    فراغ
مجموعه_insstart(linenr_T lnum, صحيح ستون)
{
    Insstart.lnum = lnum;
    Insstart.ستون = ستون;
    Insstart_orig = Insstart;
    Insstart_textlen = Insstart.ستون;
    Insstart_blank_vcol = MAXCOL;
    arrow_استفاده_شده = FALSE;
}
#پایان_اگر

/*
 * Undo the previous edit_putchar().
 */
    فراغ
edit_unputchar(فراغ)
{
    إذا (pc_وضعیت != PC_وضعیت_UNSET && pc_ردیف >= پیام_scrolled)
    {
	إذا (pc_وضعیت == PC_وضعیت_راست)
	    ++curwin->w_wcol;
	إذا (pc_وضعیت == PC_وضعیت_راست || pc_وضعیت == PC_وضعیت_چپ)
	    redrawWinline(curwin, curwin->w_cursor.lnum);
	وإلا
	    screen_puts(pc_بایتها, pc_ردیف - پیام_scrolled, pc_ستون, pc_attr);
    }
}

/*
 * Called when "$" is in 'cpoptions': display a '$' at the end of the changed
 * text.  Only works when cursor is in the line that changes.
 */
    فراغ
display_dollar(colnr_T ستون_arg)
{
    colnr_T ستون = ستون_arg < 0 ? 0 : ستون_arg;
    colnr_T save_ستون;

    إذا (!redrawing())
	أرجع;

    cursor_off();
    save_ستون = curwin->w_cursor.ستون;
    curwin->w_cursor.ستون = ستون;
    إذا (has_mbyte)
    {
	char_u *p;

	// If on the last byte of a multi-byte move to the first byte.
	p = ml_get_curline();
	curwin->w_cursor.ستون -= (*mb_سر_off)(p, p + ستون);
    }
    curs_columns(FALSE);	    // recompute w_wrow and w_wcol
    إذا (curwin->w_wcol < curwin->w_عرض)
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
backspace_until_ستونی(صحيح ستون)
{
    طالما ((صحيح)curwin->w_cursor.ستون > ستون)
    {
	curwin->w_cursor.ستون--;
	إذا (State & REPLACE_FLAG)
	    replace_do_bs(ستون);
	وإلا إذا (!del_char_after_ستون(ستون))
	    اكسر;
    }
}

/*
 * Like del_char(), but make sure not to go before column "limit_col".
 * Only matters when there are composing characters.
 * Return TRUE when something was deleted.
 */
    ساكن صحيح
del_char_after_ستون(صحيح حد_ستون استفادهنشده)
{
    إذا (enc_utf8 && حد_ستون >= 0)
    {
	colnr_T ecol = curwin->w_cursor.ستون + 1;

	// Make sure the cursor is at the start of a character, but
	// skip forward again when going too far back because of a
	// composing character.
	mb_adjust_cursor();
	طالما (curwin->w_cursor.ستون < (colnr_T)حد_ستون)
	{
	    صحيح l = utf_ptr2len(ml_get_cursor());

	    إذا (l == 0)  // end of line
		اكسر;
	    curwin->w_cursor.ستون += l;
	}
	إذا (*ml_get_cursor() == NUL || curwin->w_cursor.ستون == ecol)
	    أرجع FALSE;
	del_بایتها((دراز)((صحيح)ecol - curwin->w_cursor.ستون), FALSE, TRUE);
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
get_literal(صحيح no_Reduce_کلیدها)
{
    صحيح		cc;
    صحيح		nc;
    صحيح		i;
    صحيح		hex = FALSE;
    صحيح		octal = FALSE;
    صحيح		unicode = 0;

    إذا (got_int)
	أرجع Ctrl_C;

#اگر_تعریف_شده FEAT_GUI
    /*
     * In GUI there is no point inserting the internal code for a special key.
     * It is more useful to insert the string "<KEY>" instead.	This would
     * probably be useful in a text window too, but it would not be
     * vi-compatible (maybe there should be an option for it?) -- webb
     */
    إذا (gui.درون_use)
    {
	++allow_کلیدها;
	إذا (no_Reduce_کلیدها)
	    ++no_reduce_کلیدها;
    }
#پایان_اگر
#اگر_تعریف_شده USE_ON_FLY_SCROLL
    dont_scroll = TRUE;		// disallow scrolling here
#پایان_اگر
    ++no_mapping;		// don't map the next key hits
    cc = 0;
    i = 0;
    لكل (;;)
    {
	nc = plain_vgetc();
	إذا ((nc == ESC || nc == CSI) && !no_Reduce_کلیدها)
	    nc = decode_Modify_Other_کلیدها(nc);

	إذا ((mod_ماسک & ~MOD_ماسک_لغزش) != 0)
	    // A character with non-Shift modifiers should not be a valid
	    // character for i_CTRL-V_digit.
	    اكسر;

	إذا ((State & نما_CMDLINE) == 0 && MB_BYTE2LEN_CHECK(nc) == 1)
	    جمع_کن_مقصد_showcmd(nc);
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
#اگر_تعریف_شده FEAT_GUI
    إذا (gui.درون_use)
    {
	--allow_کلیدها;
	إذا (no_Reduce_کلیدها)
	    --no_reduce_کلیدها;
    }
#پایان_اگر
    إذا (nc)
    {
	vungetc(nc);
	// A character typed with i_CTRL-V_digit cannot have modifiers.
	mod_ماسک = 0;
    }
    got_int = FALSE;	    // CTRL-C typed after CTRL-V is not an interrupt
    أرجع cc;
}

/*
 * Insert character, taking care of special keys and mod_mask
 */
    ساكن فراغ
درج_special(
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
    إذا (mod_ماسک & MOD_ماسک_CMD)
	allow_modmask = TRUE;
#پایان_اگر
    إذا (IS_SPECIAL(c) || (mod_ماسک && allow_modmask))
    {
	p = get_special_کلید_name(c, mod_ماسک);
	len = (صحيح)STRLEN(p);
	c = p[len - 1];
	إذا (len > 2)
	{
	    إذا (stop_arrow() == FAIL)
		أرجع;
	    p[len - 1] = NUL;
	    ins_str(p, len - 1);
	    Append_مقصد_Redobuff_Lit(p, -1);
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
#تعریف_کن ISSPECIAL(c)	((c) < ' ' || (c) >= DEL || (c) == '0' || (c) == '^')

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
    صحيح		دومی_indent)		// indent for second line if >= 0
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
	صحيح     do_داخلی = TRUE;
	colnr_T virtcol = get_nolist_virtcol()
				  + char2cells(c != NUL ? c : gchar_cursor());

	إذا (*curbuf->b_p_fex != NUL && (flags & INSCHAR_NO_FEX) == 0
		&& (force_format || virtcol > (colnr_T)textwidth))
	{
	    do_داخلی = (fex_format(curwin->w_cursor.lnum, 1L, c) != 0);
	    // It may be required to save for undo again, e.g. when setline()
	    // was called.
	    ins_need_undo = TRUE;
	}
	إذا (do_داخلی)
#پایان_اگر
	    داخلی_format(textwidth, دومی_indent, flags, c == NUL, c);
    }

    إذا (c == NUL)	    // only formatting was wanted
	أرجع;

    // Check whether this character should end a comment.
    إذا (did_ai && c == انتها_comment_pending)
    {
	char_u  *سطر;
	char_u	lead_انتها[COM_MAX_LEN];	    // end-comment string
	صحيح	میانه_len, انتها_len;
	صحيح	i;

	/*
	 * Need to remove existing (middle) comment leader and insert end
	 * comment leader.  First, check what comment leader we can find.
	 */
	i = get_leader_len(سطر = ml_get_curline(), &p, FALSE, TRUE);
	إذا (i > 0 && vim_strchr(p, COM_میانه) != عدم)	// Just checking
	{
	    // Skip middle-comment string
	    طالما (*p && p[-1] != ':')	// find end of middle flags
		++p;
	    میانه_len = copy_option_part(&p, lead_انتها, COM_MAX_LEN, ",");
	    // Don't count trailing white space for middle_len
	    طالما (میانه_len > 0 && VIM_ISWHITE(lead_انتها[میانه_len - 1]))
		--میانه_len;

	    // Find the end-comment string
	    طالما (*p && p[-1] != ':')	// find end of end flags
		++p;
	    انتها_len = copy_option_part(&p, lead_انتها, COM_MAX_LEN, ",");

	    // Skip white space before the cursor
	    i = curwin->w_cursor.ستون;
	    طالما (--i >= 0 && VIM_ISWHITE(سطر[i]))
		;
	    i++;

	    // Skip to before the middle leader
	    i -= میانه_len;

	    // Check some expected things before we go on
	    إذا (i >= 0 && انتها_len > 0 && lead_انتها[انتها_len - 1] == انتها_comment_pending)
	    {
		// Backspace over all the stuff we want to replace
		backspace_until_ستونی(i);

		// Insert the end-comment string, except for the last
		// character, which will get inserted as normal later.
		ins_بایتها_len(lead_انتها, انتها_len - 1);
	    }
	}
    }
    انتها_comment_pending = NUL;

    did_ai = FALSE;
    did_si = FALSE;
    can_si = FALSE;
    can_si_پشت = FALSE;

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
#اگر_تعریف_شده USE_ON_FLY_SCROLL
    dont_scroll = FALSE;		// allow scrolling here
#پایان_اگر

    إذا (       !ISSPECIAL(c)
	    && (!has_mbyte || (*mb_char2len)(c) == 1)
	    && !has_insertcharpre()
#اگر_تعریف_شده FEAT_EVAL
	    // Skip typeahead if test_override("char_avail", 1) was called.
	    && !disable_char_avail_for_testing
#پایان_اگر
	    && vpeekc() != NUL
	    && !(State & REPLACE_FLAG)
	    && !cindent_on()
#اگر_تعریف_شده FEAT_RIGHTLEFT
	    && !p_ri
#پایان_اگر
	   )
    {
#تعریف_کن درون_داد_BUFLEN 100
	char_u		buf[درون_داد_BUFLEN + 1];
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
		&& i < درون_داد_BUFLEN
		&& (textwidth == 0
		    || (virtcol += byte2cells(buf[i - 1])) < (colnr_T)textwidth)
		&& !(!no_abbr && !vim_iswordc(c) && vim_iswordc(buf[i - 1])))
	{
#اگر_تعریف_شده FEAT_RIGHTLEFT
	    c = vgetc();
	    إذا (p_hkmap && کلید_Typed)
		c = hkmap(c);		    // Hebrew mode mapping
	    buf[i++] = c;
#وإلا
	    buf[i++] = vgetc();
#پایان_اگر
	}

#اگر_تعریف_شده FEAT_DIGRAPHS
	do_digraph(-1);			// clear digraphs
	do_digraph(buf[i-1]);		// may be the start of a digraph
#پایان_اگر
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
	    Append_مقصد_Redobuff_Lit(buf + i, -1);
    }
    وإلا
    {
	صحيح		cc;

	إذا (has_mbyte && (cc = (*mb_char2len)(c)) > 1)
	{
	    char_u	buf[MB_MAXBYTES + 1];

	    (*mb_char2bytes)(c, buf);
	    buf[cc] = NUL;
	    ins_char_بایتها(buf, cc);
	    Append_Char_مقصد_Redobuff(c);
	}
	وإلا
	{
	    ins_char(c);
	    إذا (flags & INSCHAR_CTRLV)
		redo_literal(c);
	    وإلا
		Append_Char_مقصد_Redobuff(c);
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
	Append_مقصد_Redobuff(buf);
    }
    وإلا
	Append_Char_مقصد_Redobuff(c);
}

/*
 * start_arrow() is called when an arrow key is used in insert mode.
 * For undo/redo it resembles hitting the <ESC> key.
 */
    فراغ
شروع_arrow(
    pos_T    *انتها_درج_pos)		// can be NULL
{
    شروع_arrow_common(انتها_درج_pos, TRUE);
}

/*
 * Like start_arrow() but with end_change argument.
 * Will prepare for redo of CTRL-G U if "end_change" is FALSE.
 */
    ساكن فراغ
شروع_arrow_with_change(
    pos_T    *انتها_درج_pos,		// can be NULL
    صحيح	      انتها_change)		// end undoable change
{
    شروع_arrow_common(انتها_درج_pos, انتها_change);
    إذا (!انتها_change)
    {
	Append_Char_مقصد_Redobuff(Ctrl_G);
	Append_Char_مقصد_Redobuff('U');
    }
}

    ساكن فراغ
شروع_arrow_common(
    pos_T    *انتها_درج_pos,		// can be NULL
    صحيح	      انتها_change)		// end undoable change
{
    إذا (!arrow_استفاده_شده && انتها_change)	// something has been inserted
    {
	Append_مقصد_Redobuff(ESC_STR);
	stop_درج(انتها_درج_pos, FALSE, FALSE);
	arrow_استفاده_شده = TRUE;	// this means we stopped the current insert
    }
#اگر_تعریف_شده FEAT_SPELL
    check_spell_redraw();
#پایان_اگر
}

#اگر_تعریف_شده FEAT_SPELL
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

#پایان_اگر

/*
 * stop_arrow() is called before a change is made in insert mode.
 * If an arrow key has been used, start a new insertion.
 * Returns FAIL if undo is impossible, shouldn't insert then.
 */
    صحيح
stop_arrow(فراغ)
{
    إذا (arrow_استفاده_شده)
    {
	Insstart = curwin->w_cursor;	// new insertion starts here
	إذا (Insstart.ستون > Insstart_orig.ستون && !ins_need_undo)
	    // Don't update the original insert position when moved to the
	    // right, except when nothing was inserted yet.
	    update_Insstart_orig = FALSE;
	Insstart_textlen = (colnr_T)linetabsize_str(ml_get_curline());

	إذا (u_save_cursor() == OK)
	{
	    arrow_استفاده_شده = FALSE;
	    ins_need_undo = FALSE;
	}

	ai_ستون = 0;
	إذا (State & VREPLACE_FLAG)
	{
	    orig_سطر_شمار = curbuf->b_ml.ml_سطر_شمار;
	    vr_سطرها_تغییریافته = 1;
	}
	ResetRedobuff();
	Append_مقصد_Redobuff((char_u *)"1i");   // pretend we start an insertion
	new_درج_skip = 2;
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

#اگر_تعریف_شده FEAT_FOLDING
    // Always open fold at the cursor line when inserting something.
    foldOpenCursor();
#پایان_اگر

    أرجع (arrow_استفاده_شده || ins_need_undo ? FAIL : OK);
}

/*
 * Do a few things to stop inserting.
 * "end_insert_pos" is where insert ended.  It is NULL when we already jumped
 * to another window/buffer.
 */
    ساكن فراغ
stop_درج(
    pos_T	*انتها_درج_pos,
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
    صحيح added = inserted.string == عدم ? 0 : (صحيح)inserted.درازی - new_درج_skip;
    إذا (did_restart_edit == 0 || added > 0)
    {
	vim_free(آخرین_درج.string);
	آخرین_درج = inserted;			    // structure copy
	آخرین_درج_skip = added < 0 ? 0 : new_درج_skip;
    }
    وإلا
	vim_free(inserted.string);

    إذا (!arrow_استفاده_شده && انتها_درج_pos != عدم)
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
	    إذا (curwin->w_cursor.ستون > 0 && gchar_cursor() == NUL)
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
	إذا (!nomove && did_ai && (esc || (vim_strchr(p_cpo, CPO_INDENT) == عدم
			&& curwin->w_cursor.lnum != انتها_درج_pos->lnum))
		&& انتها_درج_pos->lnum <= curbuf->b_ml.ml_سطر_شمار)
	{
	    pos_T	tpos = curwin->w_cursor;
	    colnr_T	prev_ستون = انتها_درج_pos->ستون;
	    colnr_T	strip_ستون;

	    curwin->w_cursor = *انتها_درج_pos;
	    check_cursor_ستون();  // make sure it is not past the line

	    // Where the loop would actually start (back up if on NUL).
	    strip_ستون = curwin->w_cursor.ستون;
	    إذا (gchar_cursor() == NUL && strip_ستون > 0)
		--strip_ستون;

	    // Don't strip if non-whitespace follows: setline() from a <Cmd>
	    // mapping or CursorHoldI autocmd may have inserted content.
	    إذا (*skipwhite(ml_get_curline() + strip_ستون) == NUL)
	    {
		curwin->w_cursor.ستون = strip_ستون;
		لكل (;;)
		{
		    إذا (gchar_cursor() == NUL && curwin->w_cursor.ستون > 0)
			--curwin->w_cursor.ستون;
		    cc = gchar_cursor();
		    إذا (!VIM_ISWHITE(cc))
			اكسر;
		    إذا (del_char(TRUE) == FAIL)
			اكسر;  // should not happen
		}
		إذا (curwin->w_cursor.lnum != tpos.lnum)
		    curwin->w_cursor = tpos;
		وإلا إذا (curwin->w_cursor.ستون < prev_ستون)
		{
		    // reset tpos, could have been invalidated in the loop above
		    tpos = curwin->w_cursor;
		    tpos.ستون++;
		    إذا (cc != NUL && gchar_pos(&tpos) == NUL)
			++curwin->w_cursor.ستون;	// put cursor back on the NUL
		}

		// <C-S-Right> may have started Visual mode, adjust the position for
		// deleted characters.
		إذا (VIsual_فعال)
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
    can_si_پشت = FALSE;

    // Set '[ and '] to the inserted text.  When end_insert_pos is NULL we are
    // now in a different buffer.
    إذا (انتها_درج_pos != عدم)
    {
	curbuf->b_op_شروع = Insstart;
	curbuf->b_op_شروع_orig = Insstart_orig;
	curbuf->b_op_انتها = *انتها_درج_pos;
    }
}

/*
 * Set the last inserted text to a single character.
 * Used for the replace command.
 */
    فراغ
مجموعه_آخرین_درج(صحيح c)
{
    char_u	*s;

    vim_free(آخرین_درج.string);
    آخرین_درج.string = alloc(MB_MAXBYTES * 3 + 5);
    إذا (آخرین_درج.string == عدم)
    {
	آخرین_درج.درازی = 0;
	أرجع;
    }

    s = آخرین_درج.string;
    // Use the CTRL-V only when entering a special char
    إذا (c < ' ' || c == DEL)
	*s++ = Ctrl_V;
    s = جمع_کن_char2buf(c, s);
    *s++ = ESC;
    *s = NUL;
    آخرین_درج.درازی = (size_t)(s - آخرین_درج.string);
    آخرین_درج_skip = 0;
}

#إذا defined(EXITFREE)
    فراغ
free_آخرین_درج(فراغ)
{
    VIM_CLEAR_STRING(آخرین_درج);
}
#پایان_اگر

/*
 * Add character "c" to buffer "s".  Escape the special meaning of K_SPECIAL
 * and CSI.  Handle multi-byte characters.
 * Returns a pointer to after the added bytes.
 */
    char_u *
جمع_کن_char2buf(صحيح c, char_u *s)
{
    char_u	موقت[MB_MAXBYTES + 1];
    صحيح		i;
    صحيح		len;

    len = (*mb_char2bytes)(c, موقت);
    لكل (i = 0; i < len; ++i)
    {
	c = موقت[i];
	// Need to escape K_SPECIAL and CSI like in the typeahead buffer.
	إذا (c == K_SPECIAL)
	{
	    *s++ = K_SPECIAL;
	    *s++ = KS_SPECIAL;
	    *s++ = KE_FILLER;
	}
#اگر_تعریف_شده FEAT_GUI
	وإلا إذا (c == CSI)
	{
	    *s++ = CSI;
	    *s++ = KS_EXTRA;
	    *s++ = (صحيح)KE_CSI;
	}
#پایان_اگر
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
	curwin->w_cursor.ستون = 0;
	curwin->w_cursor.coladd = 0;

	إذا (flags & (BL_WHITE | BL_SOL))
	{
	    char_u  *ptr;

	    لكل (ptr = ml_get_curline(); VIM_ISWHITE(*ptr)
			       && !((flags & BL_FIX) && ptr[1] == NUL); ++ptr)
		++curwin->w_cursor.ستون;
	}
	curwin->w_مجموعه_curswant = true;
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

    إذا (virtual_فعال())
    {
	pos_T	prevpos = curwin->w_cursor;

	// Adjust for multi-wide char (excluding TAB)
	ptr = ml_get_cursor();
	coladvance(getviscol() + ((*ptr != TAB
					  && vim_isprintc((*mb_ptr2char)(ptr)))
		    ? ptr2cells(ptr) : 1));
	curwin->w_مجموعه_curswant = true;
	// Return OK if the cursor moved, FAIL otherwise (at window edge).
	أرجع (prevpos.ستون != curwin->w_cursor.ستون
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
    curwin->w_cursor.ستون += l;

    curwin->w_مجموعه_curswant = true;
    adjust_skipcol();
    أرجع OK;
}

    صحيح
oneleft(فراغ)
{
    إذا (virtual_فعال())
    {
#اگر_تعریف_شده FEAT_LINEBREAK
	صحيح عرض;
#پایان_اگر
	صحيح v = getviscol();

	إذا (v == 0)
	    أرجع FAIL;

#اگر_تعریف_شده FEAT_LINEBREAK
	// We might get stuck on 'showbreak', skip over it.
	عرض = 1;
	لكل (;;)
	{
	    coladvance(v - عرض);
	    // getviscol() is slow, skip it when 'showbreak' is empty,
	    // 'breakindent' is not set and there are no multi-byte
	    // characters
	    إذا ((*get_showbreak_مقدار(curwin) == NUL && !curwin->w_p_bri
					     && !has_mbyte) || getviscol() < v)
		اكسر;
	    ++عرض;
	}
#وإلا
	coladvance(v - 1);
#پایان_اگر

	إذا (curwin->w_cursor.coladd == 1)
	{
	    char_u *ptr;

	    // Adjust for multi-wide char (not a TAB)
	    ptr = ml_get_cursor();
	    إذا (*ptr != TAB && vim_isprintc((*mb_ptr2char)(ptr))
							 && ptr2cells(ptr) > 1)
		curwin->w_cursor.coladd = 0;
	}

	curwin->w_مجموعه_curswant = true;
	adjust_skipcol();
	أرجع OK;
    }

    إذا (curwin->w_cursor.ستون == 0)
	أرجع FAIL;

    curwin->w_مجموعه_curswant = true;
    --curwin->w_cursor.ستون;

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
cursor_به_بالا_inner(win_T *wp, دراز n)
{
    linenr_T	lnum = wp->w_cursor.lnum;

    إذا (n >= lnum)
	lnum = 1;
    وإلا
#اگر_تعریف_شده FEAT_FOLDING
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
	    إذا (n > 0 || !((State & نما_درج) || (fdo_flags & FDO_ALL)))
		(فراغ)hasFoldingWin(wp, lnum, &lnum, عدم, TRUE, عدم);
	}
	إذا (lnum < 1)
	    lnum = 1;
    }
    وإلا
#پایان_اگر
	lnum -= n;

    wp->w_cursor.lnum = lnum;
}

    صحيح
cursor_به_بالا(
    دراز	n,
    صحيح		upd_topline)	    // When TRUE: update topline
{
    // This fails if the cursor is already in the first line or the count is
    // larger than the line number and '-' is in 'cpoptions'
    linenr_T lnum = curwin->w_cursor.lnum;
    إذا (n > 0 && (lnum <= 1
		       || (n >= lnum && vim_strchr(p_cpo, CPO_MINUS) != عدم)))
	أرجع FAIL;
    cursor_به_بالا_inner(curwin, n);

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
cursor_پایین_inner(win_T *wp, دراز n)
{
    linenr_T	lnum = wp->w_cursor.lnum;
    linenr_T	سطر_شمار = wp->w_میانگیر->b_ml.ml_سطر_شمار;

    إذا (lnum + n >= سطر_شمار)
	lnum = سطر_شمار;
    وإلا
#اگر_تعریف_شده FEAT_FOLDING
	إذا (hasAnyFolding(wp))
    {
	linenr_T	آخرین;

	// count each sequence of folded lines as one logical line
	طالما (n--)
	{
	    إذا (hasFoldingWin(wp, lnum, عدم, &آخرین, TRUE, عدم))
		lnum = آخرین + 1;
	    وإلا
		++lnum;
	    إذا (lnum >= سطر_شمار)
		اكسر;
	}
	إذا (lnum > سطر_شمار)
	    lnum = سطر_شمار;
    }
    وإلا
#پایان_اگر
	lnum += n;

    wp->w_cursor.lnum = lnum;
}

/*
 * Cursor down a number of logical lines.
 */
    صحيح
cursor_پایین(
    دراز	n,
    صحيح		upd_topline)	    // When TRUE: update topline
{
    linenr_T	lnum = curwin->w_cursor.lnum;
    linenr_T	سطر_شمار = curwin->w_میانگیر->b_ml.ml_سطر_شمار;
    // This fails if the cursor is already in the last (folded) line, or would
    // move beyond the last line and '-' is in 'cpoptions'.
#اگر_تعریف_شده FEAT_FOLDING
    hasFoldingWin(curwin, lnum, عدم, &lnum, TRUE, عدم);
#پایان_اگر
    إذا (n > 0
	    && (lnum >= سطر_شمار
		|| (lnum + n > سطر_شمار
				     && vim_strchr(p_cpo, CPO_MINUS) != عدم)))
	أرجع FAIL;
    cursor_پایین_inner(curwin, n);

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
    دراز    شمار,	// Repeat this many times
    صحيح	    no_esc)	// Don't add an ESC at the end
{
    string_T	درج;				// text to be inserted
    char_u	آخرین = ' ';

    درج = get_آخرین_درج();
    إذا (درج.string == عدم)
    {
	emsg(_(e_no_inserted_متن_yet));
	أرجع FAIL;
    }

    // may want to stuff the command character, to start Insert mode
    إذا (c != NUL)
	stuffcharReadbuff(c);

    إذا (درج.درازی > 0)
    {
	char_u	*p;

	// look for the last ESC in 'insert'
	لكل (p = درج.string + درج.درازی - 1; p >= درج.string; --p)
	{
	    إذا (*p == ESC)
	    {
		درج.درازی = (size_t)(p - درج.string);
		اكسر;
	    }
	}
    }

    إذا (درج.درازی > 0)
    {
	char_u	*p = درج.string + درج.درازی - 1;

	// when the last char is either "0" or "^" it will be quoted if no ESC
	// comes after it OR if it will insert more than once and "ptr"
	// starts with ^D.	-- Acevedo
	إذا ((*p == '0' || *p == '^')
		&& (no_esc || (*درج.string == Ctrl_D && شمار > 1)))
	{
	    آخرین = *p;
	    --درج.درازی;
	}
    }

    افعل
    {
	stuffReadbuffLen(درج.string, (دراز)درج.درازی);
	// a trailing "0" is inserted as "<C-V>048", "^" as "<C-V>^"
	بدل (آخرین)
	{
	حالة '0':
#تعریف_کن متن_مقصد_درج "\026\060\064\070"
	    stuffReadbuffLen((char_u *)متن_مقصد_درج, STRLEN_LITERAL(متن_مقصد_درج));
#لغو_تعریف متن_مقصد_درج
	    اكسر;

	حالة '^':
#تعریف_کن متن_مقصد_درج "\026^"
	    stuffReadbuffLen((char_u *)متن_مقصد_درج, STRLEN_LITERAL(متن_مقصد_درج));
#لغو_تعریف متن_مقصد_درج
	    اكسر;

	افتراضي:
	    اكسر;
	}
    }
    طالما (--شمار > 0);

    // may want to stuff a trailing ESC, to get out of Insert mode
    إذا (!no_esc)
	stuffcharReadbuff(ESC);

    أرجع OK;
}

    string_T
get_آخرین_درج(فراغ)
{
    string_T درج = {عدم, 0};

    إذا (آخرین_درج.string != عدم)
    {
	درج.string = آخرین_درج.string + آخرین_درج_skip;
	درج.درازی = (size_t)(آخرین_درج.درازی - آخرین_درج_skip);
    }

    أرجع درج;
}

/*
 * Get last inserted string, and remove trailing <Esc>.
 * Returns pointer to allocated memory (must be freed) or NULL.
 */
    char_u *
get_آخرین_درج_save(فراغ)
{
    string_T	درج = get_آخرین_درج();
    char_u	*s;

    إذا (درج.string == عدم)
	أرجع عدم;
    s = vim_strnsave(درج.string, درج.درازی);
    إذا (s == عدم)
	أرجع عدم;

    إذا (درج.درازی > 0 && s[درج.درازی - 1] == ESC)	// remove trailing ESC
	s[--درج.درازی] = NUL;
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
    إذا (p_paste || no_abbr || arrow_استفاده_شده)
	أرجع FALSE;

    أرجع check_abbr(c, ml_get_curline(), curwin->w_cursor.ستون,
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

ساكن char_u	*replace_پشته = عدم;
ساكن دراز	replace_پشته_nr = 0;	    // next entry in replace stack
ساكن دراز	replace_پشته_len = 0;	    // max. number of entries

    فراغ
replace_افزودن(
    صحيح	    c)	    // character that is replaced (NUL is none)
{
    char_u  *p;

    إذا (replace_پشته_nr < replace_جابه_جایی)	// nothing to do
	أرجع;
    إذا (replace_پشته_len <= replace_پشته_nr)
    {
	replace_پشته_len += 50;
	p = ALLOC_MULT(char_u, replace_پشته_len);
	إذا (p == عدم)	    // out of memory
	{
	    replace_پشته_len -= 50;
	    أرجع;
	}
	إذا (replace_پشته != عدم)
	{
	    mch_memmove(p, replace_پشته,
				 (size_t)(replace_پشته_nr * حجم(char_u)));
	    vim_free(replace_پشته);
	}
	replace_پشته = p;
    }
    p = replace_پشته + replace_پشته_nr - replace_جابه_جایی;
    إذا (replace_جابه_جایی)
	mch_memmove(p + 1, p, (size_t)(replace_جابه_جایی * حجم(char_u)));
    *p = c;
    ++replace_پشته_nr;
}

/*
 * Push a character onto the replace stack.  Handles a multi-byte character in
 * reverse byte order, so that the first byte is popped off first.
 * Return the number of bytes done (includes composing characters).
 */
    صحيح
replace_افزودن_mb(char_u *p)
{
    صحيح l = (*mb_ptr2len)(p);
    صحيح j;

    لكل (j = l - 1; j >= 0; --j)
	replace_افزودن(p[j]);
    أرجع l;
}

/*
 * Pop one item from the replace stack.
 * return -1 if stack empty
 * return replaced character or NUL otherwise
 */
    ساكن صحيح
replace_برداشت(فراغ)
{
    إذا (replace_پشته_nr == 0)
	أرجع -1;
    أرجع (صحيح)replace_پشته[--replace_پشته_nr];
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

    لكل (i = replace_پشته_nr; --i >= 0; )
	إذا (replace_پشته[i] == NUL && off-- <= 0)
	{
	    --replace_پشته_nr;
	    mch_memmove(replace_پشته + i, replace_پشته + i + 1,
					      (size_t)(replace_پشته_nr - i));
	    أرجع;
	}
}

/*
 * Pop bytes from the replace stack until a NUL is found, and insert them
 * before the cursor.  Can only be used in MODE_REPLACE or MODE_VREPLACE state.
 */
    ساكن فراغ
replace_برداشت_ins(فراغ)
{
    صحيح	    cc;
    صحيح	    oldState = State;

    State = نما_نرمال;			// don't want MODE_REPLACE here
    طالما ((cc = replace_برداشت()) > 0)
    {
	mb_replace_برداشت_ins(cc);
	dec_cursor();
    }
    State = oldState;
}

/*
 * Insert bytes popped from the replace stack. "cc" is the first byte.  If it
 * indicates a multi-byte char, pop the other bytes too.
 */
    ساكن فراغ
mb_replace_برداشت_ins(صحيح cc)
{
    صحيح		n;
    char_u	buf[MB_MAXBYTES + 1];
    صحيح		i;
    صحيح		c;

    إذا (has_mbyte && (n = MB_BYTE2LEN(cc)) > 1)
    {
	buf[0] = cc;
	لكل (i = 1; i < n; ++i)
	    buf[i] = replace_برداشت();
	ins_بایتها_len(buf, n);
    }
    وإلا
	ins_char(cc);

    إذا (enc_utf8)
	// Handle composing chars.
	لكل (;;)
	{
	    c = replace_برداشت();
	    إذا (c == -1)	    // stack empty
		اكسر;
	    إذا ((n = MB_BYTE2LEN(c)) == 1)
	    {
		// Not a multi-byte char, put it back.
		replace_افزودن(c);
		اكسر;
	    }

	    buf[0] = c;
	    لكل (i = 1; i < n; ++i)
		buf[i] = replace_برداشت();
	    إذا (utf_iscomposing(utf_ptr2char(buf)))
		ins_بایتها_len(buf, n);
	    وإلا
	    {
		// Not a composing char, put it back.
		لكل (i = n - 1; i >= 0; --i)
		    replace_افزودن(buf[i]);
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
    VIM_CLEAR(replace_پشته);
    replace_پشته_len = 0;
    replace_پشته_nr = 0;
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
replace_do_bs(صحيح حد_ستون)
{
    صحيح		cc;
    صحيح		orig_len = 0;
    صحيح		ins_len;
    صحيح		orig_vcols = 0;
    colnr_T	شروع_vcol;
    char_u	*p;
    صحيح		i;
    صحيح		vcol;

    cc = replace_برداشت();
    إذا (cc > 0)
    {
#اگر_تعریف_شده FEAT_PROP_POPUP
	size_t	len_before = 0;  // init to shut up GCC

	إذا (curbuf->b_has_textprop)
	{
	    // Do not adjust text properties for individual delete and insert
	    // operations, do it afterwards on the resulting text.
	    len_before = ml_get_curline_len();
	    ++متن_prop_frozen;
	}
#پایان_اگر
	إذا (State & VREPLACE_FLAG)
	{
	    // Get the number of screen cells used by the character we are
	    // going to delete.
	    getvcol(curwin, &curwin->w_cursor, عدم, &شروع_vcol, عدم, 0);
	    orig_vcols = chartabsize(ml_get_cursor(), شروع_vcol);
	}
	إذا (has_mbyte)
	{
	    (فراغ)del_char_after_ستون(حد_ستون);
	    إذا (State & VREPLACE_FLAG)
		orig_len = ml_get_cursor_len();
	    replace_افزودن(cc);
	}
	وإلا
	{
	    pchar_cursor(cc);
	    إذا (State & VREPLACE_FLAG)
		orig_len = ml_get_cursor_len() - 1;
	}
	replace_برداشت_ins();

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
	    curwin->w_cursor.ستون += ins_len;
	    طالما (vcol > orig_vcols && gchar_cursor() == ' ')
	    {
		del_char(FALSE);
		++orig_vcols;
	    }
	    curwin->w_cursor.ستون -= ins_len;
	}

	// mark the buffer as changed and prepare for displaying
	تغییریافته_بایتها(curwin->w_cursor.lnum, curwin->w_cursor.ستون);

#اگر_تعریف_شده FEAT_PROP_POPUP
	إذا (curbuf->b_has_textprop)
	{
	    size_t len_now = ml_get_curline_len();

	    --متن_prop_frozen;
	    adjust_prop_columns(curwin->w_cursor.lnum, curwin->w_cursor.ستون,
					   (صحيح)(len_now - len_before), 0);
	}
#پایان_اگر
    }
    وإلا إذا (cc == 0)
	(فراغ)del_char_after_ستون(حد_ستون);
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
#پایان_اگر

    ساكن فراغ
ins_reg(فراغ)
{
    صحيح		need_redraw = FALSE;
    صحيح		regname;
    صحيح		literally = 0;
    صحيح		vis_فعال = VIsual_فعال;

    /*
     * If we are going to wait for a character, show a '"'.
     */
    pc_وضعیت = PC_وضعیت_UNSET;
    إذا (redrawing() && !char_avail())
    {
	// may need to redraw when no more chars available now
	ins_redraw(FALSE);

	edit_putchar('"', TRUE);
	جمع_کن_مقصد_showcmd_c(Ctrl_R);
    }

#اگر_تعریف_شده USE_ON_FLY_SCROLL
    dont_scroll = TRUE;		// disallow scrolling here
#پایان_اگر

    /*
     * Don't map the register name. This also prevents the mode message to be
     * deleted when ESC is hit.
     */
    ++no_mapping;
    ++allow_کلیدها;
    regname = plain_vgetc();
    LANGMAP_ADJUST(regname, TRUE);
    إذا (regname == Ctrl_R || regname == Ctrl_O || regname == Ctrl_P)
    {
	// Get a third key for literal register insertion
	literally = regname;
	جمع_کن_مقصد_showcmd_c(literally);
	regname = plain_vgetc();
	LANGMAP_ADJUST(regname, TRUE);
    }
    --no_mapping;
    --allow_کلیدها;

#اگر_تعریف_شده FEAT_EVAL
    // Don't call u_sync() while typing the expression or giving an error
    // message for it. Only call it explicitly.
    ++no_u_sync;
    إذا (regname == '=')
    {
	pos_T	curpos = curwin->w_cursor;
# ifdef HAVE_درون_داد_روش
	صحيح	im_on = im_get_وضعیت();
# endif
	// Sync undo when evaluating the expression calls setline() or
	// append(), so that it can be undone separately.
	u_sync_once = 2;

	regname = get_عبارت_ثبات();

	// Cursor may be moved back a column.
	curwin->w_cursor = curpos;
	check_cursor();
# ifdef HAVE_درون_داد_روش
	// Restore the Input Method.
	إذا (im_on)
	    im_مجموعه_فعال(TRUE);
# endif
    }
    إذا (regname == NUL || !valid_yank_reg(regname, FALSE))
    {
	vim_beep(BO_REG);
	need_redraw = TRUE;	// remove the '"'
    }
    وإلا
    {
#پایان_اگر
	إذا (literally == Ctrl_O || literally == Ctrl_P)
	{
	    // Append the command to the redo buffer.
	    Append_Char_مقصد_Redobuff(Ctrl_R);
	    Append_Char_مقصد_Redobuff(literally);
	    Append_Char_مقصد_Redobuff(regname);

	    do_put(regname, عدم, BACKWARD, 1L,
		 (literally == Ctrl_P ? PUT_FIXINDENT : 0) | PUT_CURSEND);
	}
	وإلا إذا (درج_reg(regname, literally) == FAIL)
	{
	    vim_beep(BO_REG);
	    need_redraw = TRUE;	// remove the '"'
	}
	وإلا إذا (stop_درج_نما)
	    // When the '=' register was used and a function was invoked that
	    // did ":stopinsert" then stuff_empty() returns FALSE but we won't
	    // insert anything, need to remove the '"'
	    need_redraw = TRUE;

#اگر_تعریف_شده FEAT_EVAL
    }
    --no_u_sync;
    إذا (u_sync_once == 1)
	ins_need_undo = TRUE;
    u_sync_once = 0;
#پایان_اگر
    clear_showcmd();

    // If the inserted register is empty, we need to remove the '"'
    إذا (need_redraw || stuff_خالی())
	edit_unputchar();

    // Disallow starting Visual mode here, would get a weird mode.
    إذا (!vis_فعال && VIsual_فعال)
	انتها_visual_نما();
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
    ++allow_کلیدها;
    c = plain_vgetc();
    --no_mapping;
    --allow_کلیدها;
    بدل (c)
    {
	// CTRL-G k and CTRL-G <Up>: cursor up to Insstart.col
	حالة K_به_بالا:
	حالة Ctrl_K:
	حالة 'k': ins_به_بالا(TRUE);
		  اكسر;

	// CTRL-G j and CTRL-G <Down>: cursor down to Insstart.col
	حالة K_پایین:
	حالة Ctrl_J:
	حالة 'j': ins_پایین(TRUE);
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
    إذا (map_مقصد_exists_نما((char_u *)"", نما_LANGMAP, FALSE))
    {
	// ":lmap" mappings exists, Toggle use of ":lmap" mappings.
	إذا (State & نما_LANGMAP)
	{
	    curbuf->b_p_iminsert = B_IMODE_NONE;
	    State &= ~نما_LANGMAP;
	}
	وإلا
	{
	    curbuf->b_p_iminsert = B_IMODE_LMAP;
	    State |= نما_LANGMAP;
#اگر_تعریف_شده HAVE_درون_داد_روش
	    im_مجموعه_فعال(FALSE);
#پایان_اگر
	}
    }
#اگر_تعریف_شده HAVE_درون_داد_روش
    وإلا
    {
	// There are no ":lmap" mappings, toggle IM
	إذا (im_get_وضعیت())
	{
	    curbuf->b_p_iminsert = B_IMODE_NONE;
	    im_مجموعه_فعال(FALSE);
	}
	وإلا
	{
	    curbuf->b_p_iminsert = B_IMODE_IM;
	    State &= ~نما_LANGMAP;
	    im_مجموعه_فعال(TRUE);
	}
    }
#پایان_اگر
    مجموعه_iminsert_global();
    showmode();
#اگر_تعریف_شده FEAT_GUI
    // may show different cursor shape or color
    إذا (gui.درون_use)
	gui_update_cursor(TRUE, FALSE);
#پایان_اگر
#إذا defined(FEAT_KEYMAP)
    // Show/unshow value of 'keymap' in status lines.
    وضعیت_redraw_curbuf();
#پایان_اگر
}

/*
 * Handle ESC in insert mode.
 * Returns TRUE when leaving insert mode, FALSE when going to repeat the
 * insert.
 */
    ساكن صحيح
ins_esc(
    دراز	*شمار,
    صحيح		cmdchar,
    صحيح		nomove)	    // don't move cursor
{
    صحيح		موقت;
    ساكن صحيح	disabled_redraw = FALSE;
#اگر_تعریف_شده FEAT_CONCEAL
    // Remember if the cursor line was concealed before changing State.
    صحيح		cursor_سطر_was_concealed = curwin->w_p_cole > 0
						&& conceal_cursor_سطر(curwin);
#پایان_اگر

#اگر_تعریف_شده FEAT_SPELL
    check_spell_redraw();
#پایان_اگر

    موقت = curwin->w_cursor.ستون;
    إذا (disabled_redraw)
    {
	إذا (RedrawingDisabled > 0)
	    --RedrawingDisabled;
	disabled_redraw = FALSE;
    }
    إذا (!arrow_استفاده_شده)
    {
	/*
	 * Don't append the ESC for "r<CR>" and "grx".
	 * When 'insertmode' is set only CTRL-L stops Insert mode.  Needed for
	 * when "count" is non-zero.
	 */
	إذا (cmdchar != 'r' && cmdchar != 'v')
	    Append_مقصد_Redobuff(p_im ? (char_u *)"\014" : ESC_STR);

	/*
	 * Repeating insert may take a long time.  Check for
	 * interrupt now and then.
	 */
	إذا (*شمار > 0)
	{
	    سطر_breakcheck();
	    إذا (got_int)
		*شمار = 0;
	}

	إذا (--*شمار > 0)	// repeat what was typed
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
	stop_درج(&curwin->w_cursor, TRUE, nomove);
	undisplay_dollar();
    }

    إذا (cmdchar != 'r' && cmdchar != 'v')
	ins_apply_autocmds(EVENT_INSERTLEAVEPRE);

    // When an autoindent was removed, curswant stays after the
    // indent
    إذا (restart_edit == NUL && (colnr_T)موقت == curwin->w_cursor.ستون)
	curwin->w_مجموعه_curswant = true;

    // Remember the last Insert position in the '^ mark.
    إذا ((cmdmod.cmod_flags & CMOD_KEEPJUMPS) == 0)
	curbuf->b_آخرین_درج = curwin->w_cursor;

    /*
     * The cursor should end up on the last inserted character.
     * Don't do it for CTRL-O, unless past the end of the line.
     */
    إذا (!nomove
	    && (curwin->w_cursor.ستون != 0
		|| curwin->w_cursor.coladd > 0)
	    && (restart_edit == NUL
		   || (gchar_cursor() == NUL && !VIsual_فعال))
#اگر_تعریف_شده FEAT_RIGHTLEFT
	    && !revins_on
#پایان_اگر
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
	    --curwin->w_cursor.ستون;
	    curwin->w_valid &= ~(VALID_WCOL|VALID_VIRTCOL);
	    // Correct cursor for multi-byte character.
	    إذا (has_mbyte)
		mb_adjust_cursor();
	}
    }

#اگر_تعریف_شده HAVE_درون_داد_روش
    // Disable IM to allow typing English directly for Normal mode commands.
    // When ":lmap" is enabled don't change 'iminsert' (IM can be enabled as
    // well).
    إذا (!(State & نما_LANGMAP))
	im_save_وضعیت(&curbuf->b_p_iminsert);
    im_مجموعه_فعال(FALSE);
#پایان_اگر

    State = نما_نرمال;
    may_trigger_modechanged();
    // need to position cursor again when on a TAB and when on a char with
    // virtual text.
    إذا (gchar_cursor() == TAB
#اگر_تعریف_شده FEAT_PROP_POPUP
	    || curbuf->b_has_textprop
#پایان_اگر
       )
	curwin->w_valid &= ~(VALID_WROW|VALID_WCOL|VALID_VIRTCOL);

    setmouse();
#اگر_تعریف_شده CURSOR_SHAPE
    ui_cursor_shape();		// may show different cursor shape
#پایان_اگر
    إذا (!p_ek)
    {
	MAY_WANT_مقصد_LOG_THIS;

	// Re-enable bracketed paste mode.
	بیرون_str_t_BE();

	// Re-enable modifyOtherKeys.
	بیرون_str_t_TI();
    }
#اگر_تعریف_شده FEAT_CONCEAL
    // Check if the cursor line needs redrawing after changing State.  If
    // 'concealcursor' is "i" it needs to be redrawn without concealing.
    conceal_check_cursor_سطر(cursor_سطر_was_concealed);
#پایان_اگر

    // When recording or for CTRL-O, need to display the new mode.
    // Otherwise remove the mode message.
    إذا (reg_recording != 0 || restart_edit != NUL)
	showmode();
    وإلا إذا (p_smd && (got_int || !skip_showmode()))
	پیام("");

    أرجع TRUE;	    // exit Insert mode
}

#اگر_تعریف_شده FEAT_RIGHTLEFT
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
	    ++curwin->w_cursor.ستون;
    }
    p_ri = !p_ri;
    revins_on = (State == نما_درج && p_ri);
    إذا (revins_on)
    {
	revins_scol = curwin->w_cursor.ستون;
	revins_legal++;
	revins_chars = 0;
	undisplay_dollar();
    }
    وإلا
	revins_scol = -1;
    p_hkmap = curwin->w_p_rl ^ p_ri;    // be consistent!
    showmode();
}
#پایان_اگر

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
#اگر_تعریف_شده MACOS_X
	حالة K_چپ:
	حالة K_راست:
	حالة K_به_بالا:
	حالة K_پایین:
	حالة K_انتها:
	حالة K_HOME:
#پایان_اگر
	    إذا (!(mod_ماسک & MOD_ماسک_لغزش))
		اكسر;
	    // FALLTHROUGH
	حالة K_S_چپ:
	حالة K_S_راست:
	حالة K_S_به_بالا:
	حالة K_S_پایین:
	حالة K_S_انتها:
	حالة K_S_HOME:
	    // Start selection right away, the cursor can move with CTRL-O when
	    // beyond the end of the line.
	    شروع_selection();

	    // Execute the key in (insert) Select mode.
	    stuffcharReadbuff(Ctrl_O);
	    إذا (mod_ماسک)
	    {
		char_u	    buf[4];

		buf[0] = K_SPECIAL;
		buf[1] = KS_MODIFIER;
		buf[2] = mod_ماسک;
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
ins_درج(صحيح replaceState)
{
#اگر_تعریف_شده FEAT_EVAL
    مجموعه_vim_var_string(VV_INSERTMODE,
		   (char_u *)((State & REPLACE_FLAG) ? "i"
			    : replaceState == نما_VREPLACE ? "v" : "r"), 1);
#پایان_اگر
    ins_apply_autocmds(EVENT_INSERTCHANGE);
    إذا (State & REPLACE_FLAG)
	State = نما_درج | (State & نما_LANGMAP);
    وإلا
	State = replaceState | (State & نما_LANGMAP);
    may_trigger_modechanged();
    Append_Char_مقصد_Redobuff(K_INS);
    showmode();
#اگر_تعریف_شده CURSOR_SHAPE
    ui_cursor_shape();		// may show different cursor shape
#پایان_اگر
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
    إذا (virtual_فعال())
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
ins_لغزش(صحيح c, صحيح lastc)
{
    إذا (stop_arrow() == FAIL)
	أرجع;
    Append_Char_مقصد_Redobuff(c);

    /*
     * 0^D and ^^D: remove all indent.
     */
    إذا (c == Ctrl_D && (lastc == '0' || lastc == '^')
						  && curwin->w_cursor.ستون > 0)
    {
	--curwin->w_cursor.ستون;
	(فراغ)del_char(FALSE);		// delete the '^' or '0'
	// In Replace mode, restore the characters that '^' or '0' replaced.
	إذا (State & REPLACE_FLAG)
	    replace_برداشت_ins();
	إذا (lastc == '^')
	    old_indent = get_indent();	// remember curr. indent
	change_indent(INDENT_مجموعه, 0, TRUE, 0, TRUE);
    }
    وإلا
	change_indent(c == Ctrl_D ? INDENT_DEC : INDENT_INC, 0, TRUE, 0, TRUE);

    إذا (did_ai && *skipwhite(ml_get_curline()) != NUL)
	did_ai = FALSE;
    did_si = FALSE;
    can_si = FALSE;
    can_si_پشت = FALSE;
    can_cindent = FALSE;	// no cindenting after ^D or ^T
}

    ساكن فراغ
ins_del(فراغ)
{
    صحيح	    موقت;

    إذا (stop_arrow() == FAIL)
	أرجع;
    إذا (gchar_cursor() == NUL)		// delete newline
    {
	موقت = curwin->w_cursor.ستون;
	إذا (!can_bs(BS_EOL)		// only if "eol" included
		|| do_join(2, FALSE, TRUE, FALSE, FALSE) == FAIL)
	    vim_beep(BO_BS);
	وإلا
	{
	    curwin->w_cursor.ستون = موقت;
	    // Adjust orig_line_count in case more lines have been deleted than
	    // have been added. That makes sure, that open_line() later
	    // can access all buffer lines correctly
	    إذا (State & VREPLACE_FLAG &&
		    orig_سطر_شمار > curbuf->b_ml.ml_سطر_شمار)
		orig_سطر_شمار = curbuf->b_ml.ml_سطر_شمار;
	}
    }
    وإلا إذا (del_char(FALSE) == FAIL)  // delete char under cursor
	vim_beep(BO_BS);
    did_ai = FALSE;
    did_si = FALSE;
    can_si = FALSE;
    can_si_پشت = FALSE;
    Append_Char_مقصد_Redobuff(K_DEL);
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
		|| curwin->w_cursor.ستون >= Insstart.ستون)
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
    صحيح		نما,
    صحيح		*inserted_space_p)
{
    linenr_T	lnum;
    صحيح		cc;
    صحيح		موقت = 0;	    // init for GCC
    colnr_T	save_ستون;
    colnr_T	mincol;
    صحيح		did_backspace = FALSE;
    صحيح		درون_indent;
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
#اگر_تعریف_شده FEAT_RIGHTLEFT
		!revins_on &&
#پایان_اگر
		((curwin->w_cursor.lnum == 1 && curwin->w_cursor.ستون == 0)
		    || (!can_bs(BS_شروع)
			&& ((arrow_استفاده_شده
#اگر_تعریف_شده FEAT_کار_CHANNEL
				&& !bt_prompt(curbuf)
#پایان_اگر
			) || (curwin->w_cursor.lnum == Insstart_orig.lnum
				&& curwin->w_cursor.ستون <= Insstart_orig.ستون)))
		    || (!can_bs(BS_INDENT) && !arrow_استفاده_شده && ai_ستون > 0
					 && curwin->w_cursor.ستون <= ai_ستون)
		    || (!can_bs(BS_EOL) && curwin->w_cursor.ستون == 0))))
    {
	vim_beep(BO_BS);
	أرجع FALSE;
    }

    إذا (stop_arrow() == FAIL)
	أرجع FALSE;
    درون_indent = inindent(0);
    إذا (درون_indent)
	can_cindent = FALSE;
    انتها_comment_pending = NUL;	// After BS, don't auto-end comment
#اگر_تعریف_شده FEAT_RIGHTLEFT
    إذا (revins_on)	    // put cursor after last inserted char
	inc_cursor();
#پایان_اگر

    // Virtualedit:
    //	BACKSPACE_CHAR eats a virtual space
    //	BACKSPACE_WORD eats all coladd
    //	BACKSPACE_LINE eats all coladd and keeps going
    إذا (curwin->w_cursor.coladd > 0)
    {
	إذا (نما == BACKSPACE_CHAR)
	{
	    --curwin->w_cursor.coladd;
	    أرجع TRUE;
	}
	إذا (نما == BACKSPACE_کلمه)
	{
	    curwin->w_cursor.coladd = 0;
	    أرجع TRUE;
	}
	curwin->w_cursor.coladd = 0;
    }

    /*
     * Delete newline!
     */
    إذا (curwin->w_cursor.ستون == 0)
    {
	lnum = Insstart.lnum;
	إذا (curwin->w_cursor.lnum == lnum
#اگر_تعریف_شده FEAT_RIGHTLEFT
			|| revins_on
#پایان_اگر
				    )
	{
	    إذا (u_save((linenr_T)(curwin->w_cursor.lnum - 2),
			       (linenr_T)(curwin->w_cursor.lnum + 1)) == FAIL)
		أرجع FALSE;
	    --Insstart.lnum;
	    Insstart.ستون = ml_get_len(Insstart.lnum);
	}
	/*
	 * In replace mode:
	 * cc < 0: NL was inserted, delete it
	 * cc >= 0: NL was replaced, put original characters back
	 */
	cc = -1;
	إذا (State & REPLACE_FLAG)
	    cc = replace_برداشت();	    // returns -1 if NL was inserted
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
				   || curwin->w_cursor.lnum > orig_سطر_شمار)
	    {
		موقت = gchar_cursor();	// remember current char
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
		إذا (موقت == NUL && gchar_cursor() != NUL)
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
		State = نما_نرمال;
		/*
		 * restore characters (blanks) deleted after cursor
		 */
		طالما (cc > 0)
		{
		    save_ستون = curwin->w_cursor.ستون;
		    mb_replace_برداشت_ins(cc);
		    curwin->w_cursor.ستون = save_ستون;
		    cc = replace_برداشت();
		}
		// restore the characters that NL replaced
		replace_برداشت_ins();
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
#اگر_تعریف_شده FEAT_RIGHTLEFT
	إذا (revins_on)		// put cursor on last inserted char
	    dec_cursor();
#پایان_اگر
	mincol = 0;
						// keep indent
	إذا (نما == BACKSPACE_سطر
		&& (curbuf->b_p_ai || cindent_on())
#اگر_تعریف_شده FEAT_RIGHTLEFT
		&& !revins_on
#پایان_اگر
			    )
	{
	    save_ستون = curwin->w_cursor.ستون;
	    beginline(BL_WHITE);
	    إذا (curwin->w_cursor.ستون < save_ستون)
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
	إذا (	   نما == BACKSPACE_CHAR
		&& ((p_sta && درون_indent)
		    || ((get_sts_مقدار() != 0
#اگر_تعریف_شده FEAT_VARTABS
			|| tabstop_شمار(curbuf->b_p_vsts_array)
#پایان_اگر
			)
			&& curwin->w_cursor.ستون > 0
			&& (*(ml_get_cursor() - 1) == TAB
			    || (*(ml_get_cursor() - 1) == ' '
				&& (!*inserted_space_p
				    || arrow_استفاده_شده))))))
	{
	    colnr_T	vcol = 0;
	    colnr_T	want_vcol;
	    char_u	*سطر;
	    char_u	*ptr;
	    char_u	*cursor_ptr;
	    char_u	*space_ptr;
	    colnr_T	space_vcol = 0;
	    صحيح		prev_space = FALSE;
	    colnr_T	want_ستون;

	    *inserted_space_p = FALSE;

	    space_ptr = ptr = سطر = ml_get_curline();
	    cursor_ptr = سطر + curwin->w_cursor.ستون;

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
	    إذا (p_sta && درون_indent)
		want_vcol -= want_vcol % (صحيح)get_sw_مقدار(curbuf);
	    وإلا
#اگر_تعریف_شده FEAT_VARTABS
		want_vcol = tabstop_شروع(want_vcol, get_sts_مقدار(),
						       curbuf->b_p_vsts_array);
#وإلا
		want_vcol -= want_vcol % (صحيح)get_sts_مقدار();
#پایان_اگر

	    // Find the position to stop backspacing.
	    // Use chartabsize() so that virtual text and wrapping are ignored.
	    طالما (TRUE)
	    {
		صحيح اندازه = chartabsize(space_ptr, space_vcol);

		إذا (space_vcol + اندازه > want_vcol)
		    اكسر;
		space_vcol += اندازه;
		MB_PTR_ADV(space_ptr);
	    }
	    want_ستون = space_ptr - سطر;

	    // Delete characters until we are at or before want_col.
	    طالما (curwin->w_cursor.ستون > want_ستون)
		ins_bs_one();

	    // Insert extra spaces until we are at want_vcol.
	    لكل (; space_vcol < want_vcol; space_vcol++)
	    {
		// Remember the first char we inserted
		إذا (curwin->w_cursor.lnum == Insstart_orig.lnum
				   && curwin->w_cursor.ستون < Insstart_orig.ستون)
		    Insstart_orig.ستون = curwin->w_cursor.ستون;

		إذا (State & VREPLACE_FLAG)
		    ins_char(' ');
		وإلا
		{
		    ins_str((char_u *)" ", 1);
		    إذا ((State & REPLACE_FLAG))
			replace_افزودن(NUL);
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
#اگر_تعریف_شده FEAT_RIGHTLEFT
		إذا (!revins_on) // put cursor on char to be deleted
#پایان_اگر
		    dec_cursor();

		cc = gchar_cursor();
		// look multi-byte character class
		إذا (has_mbyte)
		{
		    prev_cclass = cclass;
		    cclass = mb_get_class(ml_get_cursor());
		}

		// start of word?
		إذا (نما == BACKSPACE_کلمه && !vim_isspace(cc))
		{
		    نما = BACKSPACE_کلمه_NOT_SPACE;
		    موقت = vim_iswordc(cc);
		}
		// end of word?
		وإلا إذا (نما == BACKSPACE_کلمه_NOT_SPACE
			&& ((vim_isspace(cc) || vim_iswordc(cc) != موقت)
			|| prev_cclass != cclass))
		{
#اگر_تعریف_شده FEAT_RIGHTLEFT
		    إذا (!revins_on)
#پایان_اگر
			inc_cursor();
#اگر_تعریف_شده FEAT_RIGHTLEFT
		    وإلا إذا (State & REPLACE_FLAG)
			dec_cursor();
#پایان_اگر
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
#اگر_تعریف_شده FEAT_RIGHTLEFT
		    إذا (revins_chars)
		    {
			revins_chars--;
			revins_legal++;
		    }
		    إذا (revins_on && gchar_cursor() == NUL)
			اكسر;
#پایان_اگر
		}
		// Just a single backspace?:
		إذا (نما == BACKSPACE_CHAR)
		    اكسر;
	    } طالما (
#اگر_تعریف_شده FEAT_RIGHTLEFT
		    revins_on ||
#پایان_اگر
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
    can_si_پشت = FALSE;
    إذا (curwin->w_cursor.ستون <= 1)
	did_ai = FALSE;

    إذا (call_fix_indent)
	fix_indent();

    /*
     * It's a little strange to put backspaces into the redo
     * buffer, but it makes auto-indent a lot easier to deal
     * with.
     */
    Append_Char_مقصد_Redobuff(c);

    // If deleted before the insertion point, adjust it
    إذا (curwin->w_cursor.lnum == Insstart_orig.lnum
				  && curwin->w_cursor.ستون < Insstart_orig.ستون)
	Insstart_orig.ستون = curwin->w_cursor.ستون;

    // vi behaviour: the cursor moves backward but the character that
    //		     was there remains visible
    // Vim behaviour: the cursor moves backward and the character that
    //		      was there is erased from the screen.
    // We can emulate the vi behaviour by pretending there is a dollar
    // displayed even when there isn't.
    //  --pkv Sun Jan 19 01:56:40 EST 2003
    إذا (vim_strchr(p_cpo, CPO_BACKSPACE) != عدم && dollar_vcol == -1)
	dollar_vcol = curwin->w_virtcol;

#اگر_تعریف_شده FEAT_FOLDING
    // When deleting a char the cursor line must never be in a closed fold.
    // E.g., when 'foldmethod' is indent and deleting the first non-white
    // char before a Tab.
    إذا (did_backspace)
	foldOpenCursor();
#پایان_اگر

    أرجع did_backspace;
}

/*
 * Handle receiving P_PS: start paste mode.  Inserts the following text up to
 * P_PE literally.
 * When "drop" is TRUE then consume the text and drop it.
 */
    صحيح
bracketed_paste(paste_نما_T نما, صحيح drop, garray_T *gap)
{
    صحيح		c;
    char_u	buf[NUMBUFLEN + MB_MAXBYTES];
    صحيح		idx = 0;
    char_u	*انتها = پیدا_کن_termcode((char_u *)"PE");
    صحيح		ret_char = -1;
    صحيح		save_allow_کلیدها = allow_کلیدها;
    صحيح		save_paste = p_paste;

    // If the end code is too long we can't detect it, read everything.
    إذا (انتها != عدم && STRLEN(انتها) >= NUMBUFLEN)
	انتها = عدم;
    ++no_mapping;
    allow_کلیدها = 0;
    إذا (!p_paste)
	// Also have the side effects of setting 'paste' to make it work much
	// faster.
	مجموعه_option_مقدار_give_err((char_u *)"paste", TRUE, عدم, 0);

    لكل (;;)
    {
	// When the end is not defined read everything there is.
	إذا (انتها == عدم && vpeekc() == NUL)
	    اكسر;
	افعل
	    c = vgetc();
	طالما (c == K_IGNORE || c == K_VER_SCROLLBAR || c == K_HOR_SCROLLBAR);

	إذا (c == NUL || got_int || (ex_نرمال_busy > 0 && c == Ctrl_C))
	    // When CTRL-C was encountered the typeahead will be flushed and we
	    // won't get the end sequence.  Except when using ":normal".
	    اكسر;

	إذا (has_mbyte)
	    idx += (*mb_char2bytes)(c, buf + idx);
	وإلا
	    buf[idx++] = c;
	buf[idx] = NUL;
	إذا (انتها != عدم && STRNCMP(buf, انتها, idx) == 0)
	{
	    إذا (انتها[idx] == NUL)
		اكسر; // Found the end of paste code.
	    استمر;
	}
	إذا (!drop)
	{
	    بدل (نما)
	    {
		حالة PASTE_CMDLINE:
		    put_on_cmdline(buf, idx, TRUE);
		    اكسر;

		حالة PASTE_EX:
		    // add one for the NUL that is going to be appended
		    إذا (gap != عدم && ga_grow(gap, idx + 1) == OK)
		    {
			mch_memmove((حرف *)gap->ga_داده + gap->ga_len,
							     buf, (size_t)idx);
			gap->ga_len += idx;
		    }
		    اكسر;

		حالة PASTE_درج:
		    إذا (stop_arrow() == OK)
		    {
			c = buf[0];
			إذا (idx == 1 && (c == CAR || c == K_KENTER || c == NL))
			    ins_eol(c);
			وإلا
			{
			    ins_char_بایتها(buf, idx);
			    Append_مقصد_Redobuff_Lit(buf, idx);
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
    allow_کلیدها = save_allow_کلیدها;
    إذا (!save_paste)
	مجموعه_option_مقدار_give_err((char_u *)"paste", FALSE, عدم, 0);

    أرجع ret_char;
}

#إذا defined(FEAT_GUI_TABLINE)
    ساكن فراغ
ins_tabline(صحيح c)
{
    // We will be leaving the current window, unless closing another tab.
    إذا (c != K_TABMENU || current_tabmenu != TABLINE_MENU_CLOSE
		|| (current_tab != 0 && current_tab != tabpage_شاخص(curtab)))
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
#پایان_اگر

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
    إذا (do_mousescroll_horiz(scrollbar_مقدار))
    {
	شروع_arrow(&tpos);
	can_cindent = TRUE;
    }
}
#پایان_اگر

    ساكن فراغ
ins_چپ(فراغ)
{
    pos_T	tpos;
    صحيح		انتها_change = dont_sync_undo == FALSE; // end undoable change

#اگر_تعریف_شده FEAT_FOLDING
    إذا ((fdo_flags & FDO_HOR) && کلید_Typed)
	foldOpenCursor();
#پایان_اگر
    undisplay_dollar();
    tpos = curwin->w_cursor;
    إذا (oneleft() == OK)
    {
#إذا defined(FEAT_XIM) && defined(FEAT_GUI_GTK)
	// Only call start_arrow() when not busy with preediting, it will
	// break undo.  K_LEFT is inserted in im_correct_cursor().
	إذا (p_imst == IM_OVER_THE_SPOT || !im_is_preediting())
#پایان_اگر
	{
	    شروع_arrow_with_change(&tpos, انتها_change);
	    إذا (!انتها_change)
		Append_Char_مقصد_Redobuff(K_چپ);
	}
#اگر_تعریف_شده FEAT_RIGHTLEFT
	// If exit reversed string, position is fixed
	إذا (revins_scol != -1 && (صحيح)curwin->w_cursor.ستون >= revins_scol)
	    revins_legal++;
	revins_chars++;
#پایان_اگر
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
	curwin->w_مجموعه_curswant = true;	// so we stay at the end
    }
    وإلا
	vim_beep(BO_CRSR);
    dont_sync_undo = FALSE;
}

    ساكن فراغ
ins_home(صحيح c)
{
    pos_T	tpos;

#اگر_تعریف_شده FEAT_FOLDING
    إذا ((fdo_flags & FDO_HOR) && کلید_Typed)
	foldOpenCursor();
#پایان_اگر
    undisplay_dollar();
    tpos = curwin->w_cursor;
    إذا (c == K_C_HOME)
	curwin->w_cursor.lnum = 1;
    curwin->w_cursor.ستون = 0;
    curwin->w_cursor.coladd = 0;
    curwin->w_curswant = 0;
    شروع_arrow(&tpos);
}

    ساكن فراغ
ins_انتها(صحيح c)
{
    pos_T	tpos;

#اگر_تعریف_شده FEAT_FOLDING
    إذا ((fdo_flags & FDO_HOR) && کلید_Typed)
	foldOpenCursor();
#پایان_اگر
    undisplay_dollar();
    tpos = curwin->w_cursor;
    إذا (c == K_C_انتها)
	curwin->w_cursor.lnum = curbuf->b_ml.ml_سطر_شمار;
    coladvance((colnr_T)MAXCOL);
    curwin->w_curswant = MAXCOL;

    شروع_arrow(&tpos);
}

    ساكن فراغ
ins_s_چپ(فراغ)
{
    صحيح انتها_change = dont_sync_undo == FALSE; // end undoable change
#اگر_تعریف_شده FEAT_FOLDING
    إذا ((fdo_flags & FDO_HOR) && کلید_Typed)
	foldOpenCursor();
#پایان_اگر
    undisplay_dollar();
    إذا (curwin->w_cursor.lnum > 1 || curwin->w_cursor.ستون > 0)
    {
	شروع_arrow_with_change(&curwin->w_cursor, انتها_change);
	إذا (!انتها_change)
	    Append_Char_مقصد_Redobuff(K_S_چپ);
	(فراغ)bck_کلمه(1L, FALSE, FALSE);
	curwin->w_مجموعه_curswant = true;
    }
    وإلا
	vim_beep(BO_CRSR);
    dont_sync_undo = FALSE;
}

    ساكن فراغ
ins_راست(فراغ)
{
    صحيح انتها_change = dont_sync_undo == FALSE; // end undoable change

#اگر_تعریف_شده FEAT_FOLDING
    إذا ((fdo_flags & FDO_HOR) && کلید_Typed)
	foldOpenCursor();
#پایان_اگر
    undisplay_dollar();
    إذا (gchar_cursor() != NUL || virtual_فعال())
    {
	شروع_arrow_with_change(&curwin->w_cursor, انتها_change);
	إذا (!انتها_change)
	    Append_Char_مقصد_Redobuff(K_راست);
	curwin->w_مجموعه_curswant = true;
	إذا (virtual_فعال())
	    oneright();
	وإلا
	{
	    إذا (has_mbyte)
		curwin->w_cursor.ستون += (*mb_ptr2len)(ml_get_cursor());
	    وإلا
		++curwin->w_cursor.ستون;
	}

#اگر_تعریف_شده FEAT_RIGHTLEFT
	revins_legal++;
	إذا (revins_chars)
	    revins_chars--;
#پایان_اگر
    }
    // if 'whichwrap' set for cursor in insert mode, may move the
    // cursor to the next line
    وإلا إذا (vim_strchr(p_ww, ']') != عدم
	    && curwin->w_cursor.lnum < curbuf->b_ml.ml_سطر_شمار)
    {
	شروع_arrow(&curwin->w_cursor);
	curwin->w_مجموعه_curswant = true;
	++curwin->w_cursor.lnum;
	curwin->w_cursor.ستون = 0;
    }
    وإلا
	vim_beep(BO_CRSR);
    dont_sync_undo = FALSE;
}

    ساكن فراغ
ins_s_راست(فراغ)
{
    صحيح انتها_change = dont_sync_undo == FALSE; // end undoable change
#اگر_تعریف_شده FEAT_FOLDING
    إذا ((fdo_flags & FDO_HOR) && کلید_Typed)
	foldOpenCursor();
#پایان_اگر
    undisplay_dollar();
    إذا (curwin->w_cursor.lnum < curbuf->b_ml.ml_سطر_شمار
	    || gchar_cursor() != NUL)
    {
	شروع_arrow_with_change(&curwin->w_cursor, انتها_change);
	إذا (!انتها_change)
	    Append_Char_مقصد_Redobuff(K_S_راست);
	(فراغ)fwd_کلمه(1L, FALSE, 0);
	curwin->w_مجموعه_curswant = true;
    }
    وإلا
	vim_beep(BO_CRSR);
    dont_sync_undo = FALSE;
}

    ساكن فراغ
ins_به_بالا(
    صحيح		startcol)	// when TRUE move to Insstart.col
{
    pos_T	tpos;
    linenr_T	old_topline = curwin->w_topline;
#اگر_تعریف_شده FEAT_DIFF
    صحيح		old_topfill = curwin->w_topfill;
#پایان_اگر

    undisplay_dollar();
    tpos = curwin->w_cursor;
    إذا (cursor_به_بالا(1L, TRUE) == OK)
    {
	إذا (startcol)
	    coladvance(getvcol_nolist(&Insstart));
	إذا (old_topline != curwin->w_topline
#اگر_تعریف_شده FEAT_DIFF
		|| old_topfill != curwin->w_topfill
#پایان_اگر
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

    إذا (mod_ماسک & MOD_ماسک_CTRL)
    {
	// <C-PageUp>: tab page back
	إذا (اولی_tabpage->tp_بعدی != عدم)
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
ins_پایین(
    صحيح		startcol)	// when TRUE move to Insstart.col
{
    pos_T	tpos;
    linenr_T	old_topline = curwin->w_topline;
#اگر_تعریف_شده FEAT_DIFF
    صحيح		old_topfill = curwin->w_topfill;
#پایان_اگر

    undisplay_dollar();
    tpos = curwin->w_cursor;
    إذا (cursor_پایین(1L, TRUE) == OK)
    {
	إذا (startcol)
	    coladvance(getvcol_nolist(&Insstart));
	إذا (old_topline != curwin->w_topline
#اگر_تعریف_شده FEAT_DIFF
		|| old_topfill != curwin->w_topfill
#پایان_اگر
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

    إذا (mod_ماسک & MOD_ماسک_CTRL)
    {
	// <C-PageDown>: tab page forward
	إذا (اولی_tabpage->tp_بعدی != عدم)
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

#اگر_تعریف_شده FEAT_DND
    ساكن فراغ
ins_drop(فراغ)
{
    do_put('~', عدم, BACKWARD, 1L, PUT_CURSEND);
}
#پایان_اگر

/*
 * Handle TAB in Insert or Replace mode.
 * Return TRUE when the TAB needs to be inserted like a normal character.
 */
    ساكن صحيح
ins_tab(فراغ)
{
    صحيح		ind;
    صحيح		i;
    صحيح		موقت;

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
#اگر_تعریف_شده FEAT_VARTABS
	    && !(p_sta && ind
		// These five lines mean 'tabstop' != 'shiftwidth'
		&& ((tabstop_شمار(curbuf->b_p_vts_array) > 1)
		    || (tabstop_شمار(curbuf->b_p_vts_array) == 1
			&& tabstop_اولی(curbuf->b_p_vts_array)
						       != get_sw_مقدار(curbuf))
		    || (tabstop_شمار(curbuf->b_p_vts_array) == 0
			&& curbuf->b_p_ts != get_sw_مقدار(curbuf))))
	    && tabstop_شمار(curbuf->b_p_vsts_array) == 0
#وإلا
	    && !(p_sta && ind && curbuf->b_p_ts != get_sw_مقدار(curbuf))
#پایان_اگر
	    && get_sts_مقدار() == 0)
	أرجع TRUE;

    إذا (stop_arrow() == FAIL)
	أرجع TRUE;

    did_ai = FALSE;
    did_si = FALSE;
    can_si = FALSE;
    can_si_پشت = FALSE;
    Append_مقصد_Redobuff((char_u *)"\t");

#اگر_تعریف_شده FEAT_VARTABS
    إذا (p_sta && ind)		// insert tab in indent, use 'shiftwidth'
    {
	موقت = (صحيح)get_sw_مقدار(curbuf);
	موقت -= get_nolist_virtcol() % موقت;
    }
    وإلا إذا (tabstop_شمار(curbuf->b_p_vsts_array) > 0 || curbuf->b_p_sts != 0)
				// use 'softtabstop' when set
	موقت = tabstop_padding(get_nolist_virtcol(), get_sts_مقدار(),
						     curbuf->b_p_vsts_array);
    وإلا			// otherwise use 'tabstop'
	موقت = tabstop_padding(get_nolist_virtcol(), curbuf->b_p_ts,
						     curbuf->b_p_vts_array);
#وإلا
    إذا (p_sta && ind)		// insert tab in indent, use 'shiftwidth'
	موقت = (صحيح)get_sw_مقدار(curbuf);
    وإلا إذا (curbuf->b_p_sts != 0) // use 'softtabstop' when set
	موقت = (صحيح)get_sts_مقدار();
    وإلا			// otherwise use 'tabstop'
	موقت = (صحيح)curbuf->b_p_ts;
    موقت -= get_nolist_virtcol() % موقت;
#پایان_اگر

    /*
     * Insert the first space with ins_char().	It will delete one char in
     * replace mode.  Insert the rest with ins_str(); it will not delete any
     * chars.  For MODE_VREPLACE state, we use ins_char() for all characters.
     */
    ins_char(' ');
    طالما (--موقت > 0)
    {
	إذا (State & VREPLACE_FLAG)
	    ins_char(' ');
	وإلا
	{
	    ins_str((char_u *)" ", 1);
	    إذا (State & REPLACE_FLAG)	    // no char replaced
		replace_افزودن(NUL);
	}
    }

    /*
     * When 'expandtab' not set: Replace spaces by TABs where possible.
     */
#اگر_تعریف_شده FEAT_VARTABS
    إذا (!curbuf->b_p_et && (tabstop_شمار(curbuf->b_p_vsts_array) > 0
			    || get_sts_مقدار() > 0
			    || (p_sta && ind)))
#وإلا
    إذا (!curbuf->b_p_et && (get_sts_مقدار() || (p_sta && ind)))
#پایان_اگر
    {
	char_u		*ptr;
	char_u		*saved_سطر = عدم;	// init for GCC
	pos_T		pos;
	pos_T		fpos;
	pos_T		*cursor;
	colnr_T		want_vcol, vcol;
	صحيح		change_ستون = -1;
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
	    ptr = saved_سطر + pos.ستون;
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
	طالما (fpos.ستون > 0 && VIM_ISWHITE(ptr[-1]))
	{
	    --fpos.ستون;
	    --ptr;
	}

	// In Replace mode, don't change characters before the insert point.
	إذا ((State & REPLACE_FLAG)
		&& fpos.lnum == Insstart.lnum
		&& fpos.ستون < Insstart.ستون)
	{
	    ptr += Insstart.ستون - fpos.ستون;
	    fpos.ستون = Insstart.ستون;
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
		إذا (change_ستون < 0)
		{
		    change_ستون = fpos.ستون;  // Column of first change
		    // May have to adjust Insstart
		    إذا (fpos.lnum == Insstart.lnum && fpos.ستون < Insstart.ستون)
			Insstart.ستون = fpos.ستون;
		}
	    }
	    ++fpos.ستون;
	    ++ptr;
	    cts.cts_vcol += i;
	}
	vcol = cts.cts_vcol;
	clear_chartabsize_arg(&cts);

	إذا (change_ستون >= 0)
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
	    fpos.ستون += repl_off;

	    // Delete following spaces.
	    i = cursor->ستون - fpos.ستون;
	    إذا (i > 0)
	    {
#اگر_تعریف_شده FEAT_PROP_POPUP
		إذا (!(State & VREPLACE_FLAG))
		{
		    char_u  *newp;
		    صحيح	    ستون;

		    newp = alloc(curbuf->b_ml.ml_سطر_len - i);
		    إذا (newp == عدم)
			أرجع FALSE;

		    ستون = ptr - curbuf->b_ml.ml_سطر_ptr;
		    إذا (ستون > 0)
			mch_memmove(newp, ptr - ستون, ستون);
		    mch_memmove(newp + ستون, ptr + i,
					   curbuf->b_ml.ml_سطر_len - ستون - i);

		    إذا (curbuf->b_ml.ml_flags & (ML_سطر_DIRTY | ML_ALLOCATED))
			vim_free(curbuf->b_ml.ml_سطر_ptr);
		    curbuf->b_ml.ml_سطر_ptr = newp;
		    curbuf->b_ml.ml_سطر_len -= i;
		    curbuf->b_ml.ml_سطر_textlen = 0;
		    curbuf->b_ml.ml_flags =
			   (curbuf->b_ml.ml_flags | ML_سطر_DIRTY) & ~ML_خالی;
		}
		وإلا
#پایان_اگر
		    STRMOVE(ptr, ptr + i);
		// correct replace stack.
		إذا ((State & REPLACE_FLAG) && !(State & VREPLACE_FLAG))
		    لكل (موقت = i; --موقت >= 0; )
			replace_join(repl_off);
	    }
#اگر_تعریف_شده FEAT_NETBEANS_INTG
	    إذا (netbeans_فعال())
	    {
		netbeans_removed(curbuf, fpos.lnum, cursor->ستون, (دراز)(i + 1));
		netbeans_inserted(curbuf, fpos.lnum, cursor->ستون,
							   (char_u *)"\t", 1);
	    }
#پایان_اگر
	    cursor->ستون -= i;

	    /*
	     * In MODE_VREPLACE state, we haven't changed anything yet.  Do it
	     * now by backspacing over the changed spacing and then inserting
	     * the new spacing.
	     */
	    إذا (State & VREPLACE_FLAG)
	    {
		// Backspace from real cursor to change_col
		backspace_until_ستونی(change_ستون);

		// Insert each char in saved_line from changed_col to
		// ptr-cursor
		ins_بایتها_len(saved_سطر + change_ستون,
						    cursor->ستون - change_ستون);
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
	replace_افزودن(NUL);

    /*
     * In MODE_VREPLACE state, a NL replaces the rest of the line, and starts
     * replacing the next line, so we push all of the characters left on the
     * line onto the replace stack.  This is not done here though, it is done
     * in open_line().
     */

    // Put cursor on NUL if on the last char and coladd is 1 (happens after
    // CTRL-O).
    إذا (virtual_فعال() && curwin->w_cursor.coladd > 0)
	coladvance(getviscol());

#اگر_تعریف_شده FEAT_RIGHTLEFT
    // NL in reverse insert will always start in the end of
    // current line.
    إذا (revins_on)
	curwin->w_cursor.ستون += ml_get_cursor_len();
#پایان_اگر

    Append_مقصد_Redobuff(NL_STR);
    i = open_سطر(FORWARD,
	    has_format_option(FO_RET_COMS) ? OPENLINE_DO_COM : 0, old_indent,
	    عدم);
    old_indent = 0;
    can_cindent = TRUE;
#اگر_تعریف_شده FEAT_FOLDING
    // When inserting a line the cursor line must never be in a closed fold.
    foldOpenCursor();
#پایان_اگر

    أرجع i;
}

#اگر_تعریف_شده FEAT_DIGRAPHS
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

    pc_وضعیت = PC_وضعیت_UNSET;
    إذا (redrawing() && !char_avail())
    {
	// may need to redraw when no more chars available now
	ins_redraw(FALSE);

	edit_putchar('?', TRUE);
	did_putchar = TRUE;
	جمع_کن_مقصد_showcmd_c(Ctrl_K);
    }

# ifdef USE_ON_FLY_SCROLL
    dont_scroll = TRUE;		// disallow scrolling here
# endif

    // don't map the digraph chars. This also prevents the
    // mode message to be deleted when ESC is hit
    ++no_mapping;
    ++allow_کلیدها;
    c = plain_vgetc();
    --no_mapping;
    --allow_کلیدها;
    إذا (did_putchar)
	// when the line fits in 'columns' the '?' is at the start of the next
	// line and will not be removed by the redraw
	edit_unputchar();

    إذا (IS_SPECIAL(c) || mod_ماسک)	    // special key
    {
	clear_showcmd();
	درج_special(c, TRUE, FALSE);
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
	    جمع_کن_مقصد_showcmd_c(c);
	}
	++no_mapping;
	++allow_کلیدها;
	cc = plain_vgetc();
	--no_mapping;
	--allow_کلیدها;
	إذا (did_putchar)
	    // when the line fits in 'columns' the '?' is at the start of the
	    // next line and will not be removed by a redraw
	    edit_unputchar();
	إذا (cc != ESC)
	{
	    Append_مقصد_Redobuff((char_u *)CTRL_V_STR);
	    c = digraph_get(c, cc, TRUE);
	    clear_showcmd();
	    أرجع c;
	}
    }
    clear_showcmd();
    أرجع NUL;
}
#پایان_اگر

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

    إذا (lnum < 1 || lnum > curbuf->b_ml.ml_سطر_شمار)
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

    إذا (ctrl_x_نما_scroll())
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
	    دراز	tw_save;

	    // The character must be taken literally, insert like it
	    // was typed after a CTRL-V, and pretend 'textwidth'
	    // wasn't set.  Digits, 'o' and 'x' are special after a
	    // CTRL-V, don't use it for these.
	    إذا (c < 256 && !امن_isalnum(c))
		Append_مقصد_Redobuff((char_u *)CTRL_V_STR);	// CTRL-V
	    tw_save = curbuf->b_p_tw;
	    curbuf->b_p_tw = -1;
	    درج_special(c, TRUE, FALSE);
	    curbuf->b_p_tw = tw_save;
#اگر_تعریف_شده FEAT_RIGHTLEFT
	    revins_chars++;
	    revins_legal++;
#پایان_اگر
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
    إذا (curwin->w_میانگیر == عدم
	|| curwin->w_میانگیر->b_ml.ml_mfp == عدم
	|| curwin->w_cursor.lnum > curwin->w_میانگیر->b_ml.ml_سطر_شمار)
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
do_درج_char_pre(صحيح c)
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
    مجموعه_vim_var_string(VV_CHAR, buf, (صحيح)buflen);  // set v:char

    res = عدم;
    إذا (ins_apply_autocmds(EVENT_INSERTCHARPRE))
    {
	// Get the value of v:char.  It may be empty or more than one
	// character.  Only use it when changed, otherwise continue with the
	// original character to avoid breaking autoindent.
	إذا (STRCMP(buf, get_vim_var_str(VV_CHAR)) != 0)
	    res = vim_strsave(get_vim_var_str(VV_CHAR));
    }

    مجموعه_vim_var_string(VV_CHAR, عدم, -1);  // clear v:char
    --textlock;

    // Restore the State, it may have been changed.
    State = save_State;

    أرجع res;
}
#پایان_اگر

    صحيح
get_can_cindent(فراغ)
{
    أرجع can_cindent;
}

    فراغ
مجموعه_can_cindent(صحيح val)
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
