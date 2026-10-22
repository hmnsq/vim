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

#অন্তর্ভাব "vim.h"

#define BACKSPACE_CHAR		    1
#define BACKSPACE_WORD		    2
#define BACKSPACE_WORD_NOT_SPACE    3
#define BACKSPACE_সারি		    4

// Set when doing something for completion that may call edit() recursively,
// which is not allowed.
স্থির পূর্ণসংখ্যা	compl_busy = FALSE;


স্থির শূন্য ins_ctrl_v(শূন্য);
স্থির শূন্য ঢোকাও_special(পূর্ণসংখ্যা, পূর্ণসংখ্যা, পূর্ণসংখ্যা);
স্থির শূন্য redo_literal(পূর্ণসংখ্যা c);
স্থির শূন্য start_arrow_common(pos_T *end_ঢোকাও_pos, পূর্ণসংখ্যা change);
#ifdef FEAT_SPELL
স্থির শূন্য check_spell_redraw(শূন্য);
#endif
স্থির শূন্য stop_ঢোকাও(pos_T *end_ঢোকাও_pos, পূর্ণসংখ্যা esc, পূর্ণসংখ্যা nomove);
স্থির পূর্ণসংখ্যা  echeck_abbr(পূর্ণসংখ্যা);
স্থির শূন্য mb_replace_বের_করো_ins(পূর্ণসংখ্যা cc);
স্থির শূন্য replace_flush(শূন্য);
স্থির শূন্য replace_do_bs(পূর্ণসংখ্যা limit_col);
স্থির পূর্ণসংখ্যা del_char_after_col(পূর্ণসংখ্যা limit_col);
স্থির শূন্য ins_reg(শূন্য);
স্থির শূন্য ins_ctrl_g(শূন্য);
স্থির শূন্য ins_ctrl_hat(শূন্য);
স্থির পূর্ণসংখ্যা  ins_esc(long *গুনতি, পূর্ণসংখ্যা cmdchar, পূর্ণসংখ্যা nomove);
#ifdef FEAT_RIGHTLEFT
স্থির শূন্য ins_ctrl_(শূন্য);
#endif
স্থির পূর্ণসংখ্যা ins_start_select(পূর্ণসংখ্যা c);
স্থির শূন্য ins_ঢোকাও(পূর্ণসংখ্যা replaceState);
স্থির শূন্য ins_ctrl_o(শূন্য);
স্থির শূন্য ins_shift(পূর্ণসংখ্যা c, পূর্ণসংখ্যা lastc);
স্থির শূন্য ins_del(শূন্য);
স্থির পূর্ণসংখ্যা  ins_bs(পূর্ণসংখ্যা c, পূর্ণসংখ্যা mode, পূর্ণসংখ্যা *inserted_space_p);
#যদি defined(FEAT_GUI_TABLINE)
স্থির শূন্য ins_tabline(পূর্ণসংখ্যা c);
#endif
স্থির শূন্য ins_বাম(শূন্য);
স্থির শূন্য ins_home(পূর্ণসংখ্যা c);
স্থির শূন্য ins_end(পূর্ণসংখ্যা c);
স্থির শূন্য ins_s_বাম(শূন্য);
স্থির শূন্য ins_ডান(শূন্য);
স্থির শূন্য ins_s_ডান(শূন্য);
স্থির শূন্য ins_up(পূর্ণসংখ্যা startcol);
স্থির শূন্য ins_pageup(শূন্য);
স্থির শূন্য ins_down(পূর্ণসংখ্যা startcol);
স্থির শূন্য ins_pagedown(শূন্য);
#ifdef FEAT_DND
স্থির শূন্য ins_drop(শূন্য);
#endif
স্থির পূর্ণসংখ্যা  ins_tab(শূন্য);
#ifdef FEAT_DIGRAPHS
স্থির পূর্ণসংখ্যা  ins_digraph(শূন্য);
#endif
স্থির পূর্ণসংখ্যা  ins_ctrl_ey(পূর্ণসংখ্যা tc);
#যদি defined(FEAT_EVAL)
স্থির char_u *do_ঢোকাও_char_pre(পূর্ণসংখ্যা c);
#endif

স্থির colnr_T	Insstart_textlen;	// length of line when insert started
স্থির colnr_T	Insstart_blank_vcol;	// vcol for first inserted blank
স্থির পূর্ণসংখ্যা	update_Insstart_orig = TRUE; // set Insstart_orig to Insstart

স্থির string_T	last_ঢোকাও = {ফাঁকা, 0};    // the text of the previous insert,
					    // K_SPECIAL and CSI are escaped
স্থির পূর্ণসংখ্যা	last_ঢোকাও_skip; // nr of chars in front of previous insert
স্থির পূর্ণসংখ্যা	new_ঢোকাও_skip;  // nr of chars in front of current insert
স্থির পূর্ণসংখ্যা	did_restart_edit;	// "restart_edit" when calling edit()

স্থির পূর্ণসংখ্যা	can_cindent;		// may do cindenting on this line

#ifdef FEAT_RIGHTLEFT
স্থির পূর্ণসংখ্যা	revins_on;		// reverse insert mode on
স্থির পূর্ণসংখ্যা	revins_chars;		// how much to skip after edit
স্থির পূর্ণসংখ্যা	revins_legal;		// was the last char 'legal'?
স্থির পূর্ণসংখ্যা	revins_scol;		// start column of revins session
#endif

স্থির পূর্ণসংখ্যা	ins_need_undo;		// call u_save() before inserting a
					// char.  Set when edit() is called.
					// after that arrow_used is used.

স্থির পূর্ণসংখ্যা	dont_sync_undo = FALSE;	// CTRL-G U prevents syncing undo for
					// the next left/right cursor key

// With 'autocompletedelay' set, arm the delay and let the main loop fire
// Insert-mode autocommands; the popup is shown later on K_COMPLETE_DELAY.
// Otherwise trigger completion right away.
#define TRIGGER_AUTOCOMPLETE()			\
    করো {					\
	update_screen(UPD_VALID);  /* Show char (deletion) immediately */ \
	out_flush();				\
	ins_compl_enable_autocomplete();	\
	ins_compl_arm_autostart();		\
	যদি (!ins_compl_arm_autocomplete_delay())\
	    যাও docomplete;			\
    } যখন (0)

#define MAY_TRIGGER_AUTOCOMPLETE(c)				\
    করো {							\
	যদি (ins_compl_has_autocomplete() && !char_avail()	\
		&& curwin->w_cursor.col > 0)			\
	{							\
	    (c) = char_before_cursor();				\
	    যদি (vim_isprintc(c))				\
		TRIGGER_AUTOCOMPLETE();				\
	}							\
    } যখন (0)

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
    পূর্ণসংখ্যা
edit(
    পূর্ণসংখ্যা		cmdchar,
    পূর্ণসংখ্যা		startln,	// if set, insert at start of line
    long	গুনতি)
{
    পূর্ণসংখ্যা		c = 0;
    char_u	*ptr;
    পূর্ণসংখ্যা		lastc = 0;
    পূর্ণসংখ্যা		mincol;
    স্থির linenr_T o_lnum = 0;
    পূর্ণসংখ্যা		i;
    পূর্ণসংখ্যা		did_backspace = TRUE;	    // previous char was backspace
    পূর্ণসংখ্যা		সারি_is_white = FALSE;	    // line is empty before insert
    linenr_T	old_topline = 0;	    // topline before insertion
#ifdef FEAT_DIFF
    পূর্ণসংখ্যা		old_topfill = -1;
#endif
    পূর্ণসংখ্যা		inserted_space = FALSE;     // just inserted a space
    পূর্ণসংখ্যা		replaceState = MODE_REPLACE;
    পূর্ণসংখ্যা		nomove = FALSE;		    // don't move cursor on return
#ifdef FEAT_JOB_CHANNEL
    পূর্ণসংখ্যা		cmdchar_todo = cmdchar;
#endif
#ifdef FEAT_CONCEAL
    পূর্ণসংখ্যা		cursor_সারি_was_concealed;
#endif
    পূর্ণসংখ্যা		ins_just_started = TRUE;

    // Remember whether editing was restarted after CTRL-O.
    did_restart_edit = restart_edit;

    // sleep before redrawing, needed for "CTRL-O :" that results in an
    // error message
    check_for_delay(TRUE);

    // set Insstart_orig to Insstart
    update_Insstart_orig = TRUE;

#ifdef HAVE_SANDBOX
    // Don't allow inserting in the sandbox.
    যদি (sandbox != 0)
    {
	emsg(_(e_not_allowed_in_sandbox));
	ফেরত FALSE;
    }
#endif
    // Don't allow changes in the buffer while editing the cmdline.  The
    // caller of getcmdline() may get confused.
    // Don't allow recursive insert mode when busy with completion.
    যদি (textlock != 0 || ins_compl_active() || compl_busy || pum_visible())
    {
	emsg(_(e_not_allowed_to_change_text_or_change_window));
	ফেরত FALSE;
    }
    ins_compl_clear();	    // clear stuff for CTRL-X mode

    /*
     * Trigger InsertEnter autocommands.  Do not do this for "r<CR>" or "grx".
     */
    যদি (cmdchar != 'r' && cmdchar != 'v')
    {
	pos_T   save_cursor = curwin->w_cursor;

#ifdef FEAT_EVAL
	যদি (cmdchar == 'R')
	    ptr = (char_u *)"r";
	অন্যথা যদি (cmdchar == 'V')
	    ptr = (char_u *)"v";
	অন্যথা
	    ptr = (char_u *)"i";
	set_vim_var_string(VV_INSERTMODE, ptr, 1);
	set_vim_var_string(VV_CHAR, ফাঁকা, -1);  // clear v:char
#endif
	ins_apply_autocmds(EVENT_INSERTENTER);

	// Check for changed highlighting, e.g. for ModeMsg.
	যদি (need_highlight_changed)
	    highlight_changed();

	// Make sure the cursor didn't move.  Do call check_cursor_col() in
	// case the text was modified.  Since Insert mode was not started yet
	// a call to check_cursor_col() may move the cursor, especially with
	// the "A" command, thus set State to avoid that. Also check that the
	// line number is still valid (lines may have been deleted).
	// Do not restore if v:char was set to a non-empty string.
	যদি (!EQUAL_POS(curwin->w_cursor, save_cursor)
#ifdef FEAT_EVAL
		&& *get_vim_var_str(VV_CHAR) == NUL
#endif
		&& save_cursor.lnum <= curbuf->b_ml.ml_সারি_গুনতি)
	{
	    পূর্ণসংখ্যা save_state = State;

	    curwin->w_cursor = save_cursor;
	    State = MODE_ঢোকাও;
	    check_cursor_col();
	    State = save_state;
	}
    }

#ifdef FEAT_CONCEAL
    // Check if the cursor line was concealed before changing State.
    cursor_সারি_was_concealed = curwin->w_p_cole > 0
						&& conceal_cursor_সারি(curwin);
#endif

    /*
     * When doing a paste with the middle mouse button, Insstart is set to
     * where the paste started.
     */
    যদি (where_paste_started.lnum != 0)
	Insstart = where_paste_started;
    অন্যথা
    {
	Insstart = curwin->w_cursor;
	যদি (startln)
	    Insstart.col = 0;
    }
    Insstart_textlen = (colnr_T)linetabsize_str(ml_get_curline());
    Insstart_blank_vcol = MAXCOL;
    যদি (!did_ai)
	ai_col = 0;

    যদি (cmdchar != NUL && restart_edit == 0)
    {
	ResetRedobuff();
	Append_সংখ্যা_To_Redobuff(গুনতি);
	যদি (cmdchar == 'V' || cmdchar == 'v')
	{
	    // "gR" or "gr" command
	    AppendCharToRedobuff('g');
	    AppendCharToRedobuff((cmdchar == 'v') ? 'r' : 'R');
	}
	অন্যথা
	{
	    যদি (cmdchar == K_PS)
		AppendCharToRedobuff('a');
	    অন্যথা
		AppendCharToRedobuff(cmdchar);
	    যদি (cmdchar == 'g')		    // "gI" command
		AppendCharToRedobuff('I');
	    অন্যথা যদি (cmdchar == 'r')	    // "r<CR>" command
		গুনতি = 1;		    // insert only one <CR>
	}
    }

    যদি (cmdchar == 'R')
    {
	State = MODE_REPLACE;
    }
    অন্যথা যদি (cmdchar == 'V' || cmdchar == 'v')
    {
	State = MODE_VREPLACE;
	replaceState = MODE_VREPLACE;
	orig_সারি_গুনতি = curbuf->b_ml.ml_সারি_গুনতি;
	vr_সারিগুলি_changed = 1;
    }
    অন্যথা
	State = MODE_ঢোকাও;

    may_trigger_modechanged();
    stop_ঢোকাও_mode = FALSE;

#ifdef FEAT_CONCEAL
    // Check if the cursor line needs redrawing after changing State.  If
    // 'concealcursor' is "n" it needs to be redrawn without concealing.
    conceal_check_cursor_সারি(cursor_সারি_was_concealed);
#endif

    // Need to position cursor again when on a TAB and when on a char with
    // virtual text.
    যদি (gchar_cursor() == TAB
#ifdef FEAT_PROP_POPUP
	    || curbuf->b_has_textprop
#endif
       )
	curwin->w_valid &= ~(VALID_WROW|VALID_WCOL|VALID_VIRTCOL);

    /*
     * Enable langmap or IME, indicated by 'iminsert'.
     * Note that IME may enabled/disabled without us noticing here, thus the
     * 'iminsert' value may not reflect what is actually used.  It is updated
     * when hitting <Esc>.
     */
    যদি (curbuf->b_p_iminsert == B_IMODE_LMAP)
	State |= MODE_LANGMAP;
#ifdef HAVE_প্রবেশ_METHOD
    im_set_active(curbuf->b_p_iminsert == B_IMODE_IM);
#endif

    setmouse();
    clear_showcmd();
#ifdef FEAT_RIGHTLEFT
    // there is no reverse replace mode
    revins_on = (State == MODE_ঢোকাও && p_ri);
    যদি (revins_on)
	undisplay_dollar();
    revins_chars = 0;
    revins_legal = 0;
    revins_scol = -1;
#endif
    যদি (!p_ek)
    {
	MAY_WANT_TO_LOG_THIS;

	// Disable bracketed paste mode, we won't recognize the escape
	// sequences.
	out_str(T_BD);

	// Disable modifyOtherKeys, keys with modifiers would cause exiting
	// Insert mode.
	out_str_t_TE();
    }

    /*
     * Handle restarting Insert mode.
     * Don't do this for "CTRL-O ." (repeat an insert): In that case we get
     * here with something in the stuff buffer.
     */
    যদি (restart_edit != 0 && stuff_empty())
    {
	/*
	 * After a paste we consider text typed to be part of the insert for
	 * the pasted text. You can backspace over the pasted text too.
	 */
	যদি (where_paste_started.lnum)
	    arrow_used = FALSE;
	অন্যথা
	    arrow_used = TRUE;
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
	যদি (((ins_at_eol && curwin->w_cursor.lnum == o_lnum)
		    || curwin->w_curswant > curwin->w_virtcol)
		&& *(ptr = ml_get_curline() + curwin->w_cursor.col) != NUL)
	{
	    যদি (ptr[1] == NUL)
		++curwin->w_cursor.col;
	    অন্যথা যদি (has_mbyte)
	    {
		i = (*mb_ptr2len)(ptr);
		যদি (ptr[i] == NUL)
		    curwin->w_cursor.col += i;
	    }
	}
	ins_at_eol = FALSE;
    }
    অন্যথা
	arrow_used = FALSE;

    // we are in insert mode now, don't need to start it anymore
    need_start_insertmode = FALSE;

    // Need to save the line for undo before inserting the first char.
    ins_need_undo = TRUE;

    where_paste_started.lnum = 0;
    can_cindent = TRUE;
#ifdef FEAT_FOLDING
    // The cursor line is not in a closed fold, unless 'insertmode' is set or
    // restarting.
    যদি (!p_im && did_restart_edit == 0)
	foldOpenCursor();
#endif

    /*
     * If 'showmode' is set, show the current (insert/replace/..) mode.
     * A warning message for changing a readonly file is given here, before
     * actually changing anything.  It's put after the mode, if any.
     */
    i = 0;
    যদি (p_smd && বার্তা_silent == 0)
	i = showmode();

    যদি (!p_im && did_restart_edit == 0)
	change_warning(i == 0 ? 0 : i + 1);

#ifdef CURSOR_SHAPE
    ui_cursor_shape();		// may show different cursor shape
#endif
#ifdef FEAT_DIGRAPHS
    do_digraph(-1);		// clear digraphs
#endif

    /*
     * Get the current length of the redo buffer, those characters have to be
     * skipped if we want to get to the inserted characters.
     */
    string_T inserted = get_inserted();
    new_ঢোকাও_skip = (পূর্ণসংখ্যা)inserted.দৈর্ঘ্য;
    যদি (inserted.string != ফাঁকা)
	vim_free(inserted.string);

    old_indent = 0;

    /*
     * Main loop in Insert mode: repeat until Insert mode is left.
     */
    জন্য (;;)
    {
#ifdef FEAT_RIGHTLEFT
	যদি (!revins_legal)
	    revins_scol = -1;	    // reset on illegal motions
	অন্যথা
	    revins_legal = 0;
#endif
	যদি (arrow_used)	    // don't repeat insert when arrow key used
	    গুনতি = 0;

	যদি (update_Insstart_orig)
	    Insstart_orig = Insstart;

	যদি (stop_ঢোকাও_mode && !ins_compl_active())
	{
	    // ":stopinsert" used or 'insertmode' reset
	    গুনতি = 0;
	    যাও doESCkey;
	}

	// set curwin->w_curswant for next K_DOWN or K_UP
	যদি (!arrow_used)
	    curwin->w_set_curswant = true;

	// If there is no typeahead may check for timestamps (e.g., for when a
	// menu invoked a shell command).
	যদি (stuff_empty())
	{
	    did_check_timestamps = FALSE;
	    যদি (need_check_timestamps)
		check_timestamps(FALSE);
	}

	/*
	 * When emsg() was called msg_scroll will have been set.
	 */
	বার্তা_scroll = FALSE;

#ifdef FEAT_GUI
	// When 'mousefocus' is set a mouse movement may have taken us to
	// another window.  "need_mouse_correct" may then be set because of an
	// autocommand.
	যদি (need_mouse_correct)
	    gui_mouse_correct();
#endif

#ifdef FEAT_FOLDING
	// Open fold at the cursor line, according to 'foldopen'.
	যদি (fdo_flags & FDO_ঢোকাও)
	    foldOpenCursor();
	// Close folds where the cursor isn't, according to 'foldclose'
	যদি (!char_avail())
	    foldCheckClose();
#endif

#ifdef FEAT_JOB_CHANNEL
	যদি (bt_prompt(curbuf))
	{
	    init_prompt(cmdchar_todo);
	    cmdchar_todo = NUL;
	}
#endif

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
	যদি (curbuf->b_mod_set
		&& curwin->w_p_wrap
		&& !curwin->w_p_sms
		&& !did_backspace
		&& curwin->w_topline == old_topline
#ifdef FEAT_DIFF
		&& curwin->w_topfill == old_topfill
#endif
		&& গুনতি <= 1
		)
	{
	    mincol = curwin->w_wcol;
	    validate_cursor_col();

	    যদি (
#ifdef FEAT_VARTABS
		curwin->w_wcol < mincol - tabstop_at(get_nolist_virtcol(),
						     curbuf->b_p_ts,
						     curbuf->b_p_vts_array,
						     FALSE)
#অন্যথা
		(পূর্ণসংখ্যা)curwin->w_wcol < mincol - curbuf->b_p_ts
#endif
		    && curwin->w_wrow ==
				   curwin->w_height - 1 - get_scrolloff_মান()
		    && (curwin->w_cursor.lnum != curwin->w_topline
#ifdef FEAT_DIFF
			|| curwin->w_topfill > 0
#endif
		    ))
	    {
#ifdef FEAT_DIFF
		যদি (curwin->w_topfill > 0)
		    --curwin->w_topfill;
		অন্যথা
#endif
#ifdef FEAT_FOLDING
		যদি (hasFolding(curwin->w_topline, ফাঁকা, &old_topline))
		    set_topline(curwin, old_topline + 1);
		অন্যথা
#endif
		    set_topline(curwin, curwin->w_topline + 1);
	    }
	}

	// May need to adjust w_topline to show the cursor.
	যদি (গুনতি <= 1)
	    update_topline();

	did_backspace = FALSE;

	যদি (গুনতি <= 1)
	    validate_cursor();		// may set must_redraw

	/*
	 * Redraw the display when no characters are waiting.
	 * Also shows mode, ruler and positions cursor.
	 */
	ins_redraw(TRUE);

	যদি (curwin->w_p_scb)
	    do_check_scrollbind(TRUE);

	যদি (curwin->w_p_crb)
	    do_check_cursorbind();
	যদি (গুনতি <= 1)
	    update_curswant();
	old_topline = curwin->w_topline;
#ifdef FEAT_DIFF
	old_topfill = curwin->w_topfill;
#endif

#ifdef USE_ON_FLY_SCROLL
	dont_scroll = FALSE;		// allow scrolling here
#endif
	// May request the keyboard protocol state now.
	may_send_t_RK();

	/*
	 * Get a character for Insert mode.  Ignore K_IGNORE and K_NOP.
	 */
	যদি (c != K_CURSORHOLD && c != K_COMPLETE_DELAY)
	    lastc = c;		// remember the previous char for CTRL-D

	// After using CTRL-G U the next cursor key will not break undo.
	যদি (dont_sync_undo == MAYBE)
	    dont_sync_undo = TRUE;
	অন্যথা
	    dont_sync_undo = FALSE;
	যদি (cmdchar == K_PS)
	    // Got here from normal mode when bracketed paste started.
	    c = K_PS;
	অন্যথা
	{
	    // Trigger autocomplete when entering Insert mode, either directly
	    // or via change commands like 'ciw', 'cw', etc., before the first
	    // character is typed.
	    যদি (ins_just_started)
	    {
		ins_just_started = FALSE;
		যদি (ins_compl_has_autocomplete() && !char_avail()
			&& curwin->w_cursor.col > 0)
		{
		    c = char_before_cursor();
		    যদি (vim_isprintc(c))
		    {
			ins_compl_enable_autocomplete();
			ins_compl_arm_autostart();
			ins_compl_init_get_longest();
#ifdef FEAT_RIGHTLEFT
			যদি (p_hkmap)
			    c = hkmap(c);		// Hebrew mode mapping
#endif
			// Defer until the delay expires (K_COMPLETE_DELAY), or
			// trigger now when no delay is in effect.
			যদি (!ins_compl_arm_autocomplete_delay())
			    যাও docomplete;
		    }
		}
	    }

	    করো
	    {
		c = safe_vgetc();

		যদি (stop_ঢোকাও_mode
#ifdef FEAT_TERMINAL
			|| (c == K_IGNORE && term_use_loop())
#endif
		   )
		{
		    // Insert mode ended, possibly from a callback, or a timer
		    // must have opened a terminal window.
		    যদি (c != K_IGNORE && c != K_NOP)
			vungetc(c);
		    গুনতি = 0;

		    যদি (!bt_prompt(curwin->w_বাফার)
#ifdef FEAT_TERMINAL
			    && !bt_terminal(curwin->w_বাফার)
#endif
			    && stop_ঢোকাও_mode)
			// :stopinsert command via callback or via server command
			nomove = FALSE;
		    অন্যথা
			nomove = TRUE;
		    ins_compl_prep(ESC);
		    যাও doESCkey;
		}
	    } যখন (c == K_IGNORE || c == K_NOP);
	}

	// Don't want K_CURSORHOLD for the second key, e.g., after CTRL-V.
	did_cursorhold = TRUE;
	যদি (c != K_CURSORHOLD && c != K_COMPLETE_DELAY)
	{
	    // Don't want delayed autocompletion from the previous key either.
	    ins_compl_clear_autocomplete_delay();
	    ins_compl_disarm_autostart();
	    // A completion already on screen goes on being what it was.
	    যদি (!ins_compl_active())
		ins_compl_disable_autocomplete();
	}

#ifdef FEAT_RIGHTLEFT
	যদি (p_hkmap && চাবি_Typed)
	    c = hkmap(c);		// Hebrew mode mapping
#endif

	// If the window was made so small that nothing shows, make it at least
	// one line and one column when typing.
	যদি (চাবি_Typed && !চাবি_Stuffed)
	    win_ensure_আকার();

	/*
	 * Special handling of keys while the popup menu is visible or wanted
	 * and the cursor is still in the completed word.  Only when there is
	 * a match, skip this when no matches were found.
	 */
	যদি (ins_compl_active() && curwin->w_cursor.col >= ins_compl_col()
		&& ins_compl_has_shown_match() && pum_wanted())
	{
	    // BS: Delete one character from "compl_leader".
	    যদি ((c == K_BS || c == Ctrl_H)
			&& curwin->w_cursor.col > ins_compl_col()
			&& (c = ins_compl_bs()) == NUL)
		চালু;

	    // When no match was selected or it was edited.
	    যদি (!ins_compl_used_match())
	    {
		// CTRL-L: Add one character from the current match to
		// "compl_leader".  Except when at the original match and
		// there is nothing to add, CTRL-L works like CTRL-P then.
		যদি (c == Ctrl_L
			&& (!ctrl_x_mode_সারি_or_eval()
			    || ins_compl_long_shown_match()))
		{
		    ins_compl_addfrommatch();
		    চালু;
		}

		// A non-white character that fits in with the current
		// completion: Add to "compl_leader".
		যদি (ins_compl_accept_char(c))
		{
#যদি defined(FEAT_EVAL)
		    // Trigger InsertCharPre.
		    char_u *str = do_ঢোকাও_char_pre(c);
		    char_u *p;

		    যদি (str != ফাঁকা)
		    {
			জন্য (p = str; *p != NUL; MB_PTR_ADV(p))
			    ins_compl_addleader(PTR2CHAR(p));
			vim_free(str);
		    }
		    অন্যথা
#endif
			ins_compl_addleader(c);
		    চালু;
		}

		// Pressing CTRL-Y selects the current match.  When
		// ins_compl_enter_selects() is set the Enter key does the
		// same.
		যদি ((c == Ctrl_Y || (ins_compl_enter_selects()
				    && (c == CAR || c == K_KENTER || c == NL)))
			&& stop_arrow() == OK)
		{
		    ins_compl_delete();
		    যদি (ins_compl_preinsert_longest()
			    && !ins_compl_is_match_selected())
		    {
			ins_compl_ঢোকাও(FALSE, TRUE);
			ins_compl_init_get_longest();
			চালু;
		    }
		    অন্যথা
			ins_compl_ঢোকাও(FALSE, FALSE);
		}
		// Delete preinserted text when typing special chars
		অন্যথা যদি (IS_WHITE_NL_OR_NUL(c) && ins_compl_preinsert_effect())
		    ins_compl_delete();
	    }
	}

	// Prepare for or stop CTRL-X mode.  This doesn't do completion, but
	// it does fix up the text when finishing completion.
	ins_compl_init_get_longest();
	যদি (ins_compl_prep(c))
	    চালু;

	// CTRL-\ CTRL-N goes to Normal mode,
	// CTRL-\ CTRL-G goes to mode selected with 'insertmode',
	// CTRL-\ CTRL-O is like CTRL-O but without moving the cursor.
	যদি (c == Ctrl_BSL)
	{
	    // may need to redraw when no more chars available now
	    ins_redraw(FALSE);
	    ++no_mapping;
	    ++allow_চাবিগুলি;
	    c = plain_vgetc();
	    --no_mapping;
	    --allow_চাবিগুলি;
	    যদি (c != Ctrl_N && c != Ctrl_G && c != Ctrl_O)
	    {
		// it's something else
		vungetc(c);
		c = Ctrl_BSL;
	    }
	    অন্যথা যদি (c == Ctrl_G && p_im)
		চালু;
	    অন্যথা
	    {
		যদি (c == Ctrl_O)
		{
		    ins_ctrl_o();
		    ins_at_eol = FALSE;	// cursor keeps its column
		    nomove = TRUE;
		}
		গুনতি = 0;
		যাও doESCkey;
	    }
	}

#ifdef FEAT_DIGRAPHS
	c = do_digraph(c);
#endif

	যদি ((c == Ctrl_V || c == Ctrl_Q) && ctrl_x_mode_cmdline())
	    যাও docomplete;
	যদি (c == Ctrl_V || c == Ctrl_Q)
	{
	    ins_ctrl_v();
	    c = Ctrl_V;	// pretend CTRL-V is last typed character
	    চালু;
	}

	যদি (cindent_on() && ctrl_x_mode_none())
	{
	    // A key name preceded by a bang means this key is not to be
	    // inserted.  Skip ahead to the re-indenting below.
	    // A key name preceded by a star means that indenting has to be
	    // done before inserting the key.
	    সারি_is_white = inindent(0);
	    যদি (in_cinkeys(c, '!', সারি_is_white))
		যাও force_cindent;
	    যদি (can_cindent && in_cinkeys(c, '*', সারি_is_white)
							&& stop_arrow() == OK)
		do_c_রাশিমালা_indent();
	}

#ifdef FEAT_RIGHTLEFT
	যদি (curwin->w_p_rl)
	    নির্বাচন (c)
	    {
		ক্ষেত্র K_বাম:	c = K_ডান; বিরতি;
		ক্ষেত্র K_S_বাম:	c = K_S_ডান; বিরতি;
		ক্ষেত্র K_C_বাম:	c = K_C_ডান; বিরতি;
		ক্ষেত্র K_ডান:	c = K_বাম; বিরতি;
		ক্ষেত্র K_S_ডান: c = K_S_বাম; বিরতি;
		ক্ষেত্র K_C_ডান: c = K_C_বাম; বিরতি;
	    }
#endif

	/*
	 * If 'keymodel' contains "startsel", may start selection.  If it
	 * does, a CTRL-O and c will be stuffed, we need to get these
	 * characters.
	 */
	যদি (ins_start_select(c))
	    চালু;

	/*
	 * The big switch to handle a character in insert mode.
	 */
	নির্বাচন (c)
	{
	ক্ষেত্র ESC:	// End input mode
	    যদি (echeck_abbr(ESC + ABBR_OFF))
		বিরতি;
	    // FALLTHROUGH

	ক্ষেত্র Ctrl_C:	// End input mode
	    যদি (c == Ctrl_C && cmdwin_type != 0)
	    {
		// Close the cmdline window.
		cmdwin_ফলাফল = K_IGNORE;
		got_int = FALSE; // don't stop executing autocommands et al.
		nomove = TRUE;
		যাও doESCkey;
	    }
#ifdef FEAT_JOB_CHANNEL
	    যদি (c == Ctrl_C && bt_prompt(curbuf))
	    {
		যদি (invoke_prompt_interrupt())
		{
		    যদি (!bt_prompt(curbuf))
			// buffer changed to a non-prompt buffer, get out of
			// Insert mode
			যাও doESCkey;
		    বিরতি;
		}
	    }
#endif

#ifdef UNIX
do_intr:
#endif
	    // when 'insertmode' set, and not halfway a mapping, don't leave
	    // Insert mode
	    যদি (goto_im())
	    {
		যদি (got_int)
		{
		    (শূন্য)vgetc();		// flush all buffers
		    got_int = FALSE;
		}
		অন্যথা
		    vim_beep(BO_IM);
		বিরতি;
	    }
doESCkey:
	    // Drop a pending autocomplete so it does not outlive Insert mode.
	    ins_compl_clear_autocomplete_delay();
	    /*
	     * This is the ONLY return from edit()!
	     */
	    // Always update o_lnum, so that a "CTRL-O ." that adds a line
	    // still puts the cursor back after the inserted text.
	    যদি (ins_at_eol && gchar_cursor() == NUL)
		o_lnum = curwin->w_cursor.lnum;

	    যদি (ins_esc(&গুনতি, cmdchar, nomove))
	    {
		// When CTRL-C was typed got_int will be set, with the result
		// that the autocommands won't be executed. When mapped got_int
		// is not set, but let's keep the behavior the same.
		যদি (cmdchar != 'r' && cmdchar != 'v' && c != Ctrl_C)
		    ins_apply_autocmds(EVENT_INSERTLEAVE);
		did_cursorhold = FALSE;

		// ins_redraw() triggers TextChangedI only when no characters
		// are in the typeahead buffer, so reset curbuf->b_last_changedtick only
		// if the TextChangedI was not blocked by char_avail() (e.g. using :norm!)
		// and the TextChangedI autocommand has been triggered.
		যদি (!char_avail() && curbuf->b_last_changedtick_i == CHANGEDTICK(curbuf))
		    curbuf->b_last_changedtick = CHANGEDTICK(curbuf);
		ফেরত (c == Ctrl_O);
	    }
	    চালু;

	ক্ষেত্র Ctrl_Z:	// suspend when 'insertmode' set
	    যদি (!p_im)
		যাও normalchar;	// insert CTRL-Z as normal char
	    do_cmdline_cmd((char_u *)"stop");
#ifdef CURSOR_SHAPE
	    ui_cursor_shape();		// may need to update cursor shape
#endif
	    চালু;

	ক্ষেত্র Ctrl_O:	// execute one command
#ifdef FEAT_COMPL_FUNC
	    যদি (ctrl_x_mode_omni())
		যাও docomplete;
#endif
	    যদি (echeck_abbr(Ctrl_O + ABBR_OFF))
		বিরতি;
	    ins_ctrl_o();

	    // don't move the cursor left when 'virtualedit' has "onemore".
	    যদি (get_ve_flags() & VE_ONEMORE)
	    {
		ins_at_eol = FALSE;
		nomove = TRUE;
	    }
	    গুনতি = 0;
	    যাও doESCkey;

	ক্ষেত্র K_INS:	// toggle insert/replace mode
	ক্ষেত্র K_KINS:
	    ins_ঢোকাও(replaceState);
	    বিরতি;

	ক্ষেত্র K_SELECT:	// end of Select mode mapping - ignore
	    বিরতি;

	ক্ষেত্র K_HELP:	// Help key works like <ESC> <Help>
	ক্ষেত্র K_F1:
	ক্ষেত্র K_XF1:
	    stuffcharReadbuff(K_HELP);
	    যদি (p_im)
		need_start_insertmode = TRUE;
	    যাও doESCkey;

#ifdef FEAT_NETBEANS_INTG
	ক্ষেত্র K_F21:	// NetBeans command
	    ++no_mapping;		// don't map the next key hits
	    i = plain_vgetc();
	    --no_mapping;
	    netbeans_keycommand(i);
	    বিরতি;
#endif

	ক্ষেত্র K_ZERO:	// Insert the previously inserted text.
	ক্ষেত্র NUL:
	ক্ষেত্র Ctrl_A:
	    // For ^@ the trailing ESC will end the insert, unless there is an
	    // error.
	    যদি (stuff_inserted(NUL, 1L, (c == Ctrl_A)) == FAIL
						   && c != Ctrl_A && !p_im)
		যাও doESCkey;		// quit insert mode
	    inserted_space = FALSE;
	    বিরতি;

	ক্ষেত্র Ctrl_R:	// insert the contents of a register
	    যদি (ctrl_x_mode_register() && !ins_compl_active())
		যাও docomplete;
	    ins_reg();
	    auto_format(FALSE, TRUE);
	    inserted_space = FALSE;
	    বিরতি;

	ক্ষেত্র Ctrl_G:	// commands starting with CTRL-G
	    ins_ctrl_g();
	    বিরতি;

	ক্ষেত্র Ctrl_HAT:	// switch input mode and/or langmap
	    ins_ctrl_hat();
	    বিরতি;

#ifdef FEAT_RIGHTLEFT
	ক্ষেত্র Ctrl__:	// switch between languages
	    যদি (!p_ari)
		যাও normalchar;
	    ins_ctrl_();
	    বিরতি;
#endif

	ক্ষেত্র Ctrl_D:	// Make indent one shiftwidth smaller.
#যদি defined(FEAT_FIND_ID)
	    যদি (ctrl_x_mode_path_defines())
		যাও docomplete;
#endif
	    // FALLTHROUGH

	ক্ষেত্র Ctrl_T:	// Make indent one shiftwidth greater.
	    যদি (c == Ctrl_T && ctrl_x_mode_thesaurus())
	    {
		যদি (has_compl_option(FALSE))
		    যাও docomplete;
		বিরতি;
	    }

	    ins_shift(c, lastc);
	    auto_format(FALSE, TRUE);
	    inserted_space = FALSE;
	    বিরতি;

	ক্ষেত্র K_DEL:	// delete character under the cursor
	ক্ষেত্র K_KDEL:
	    ins_del();
	    auto_format(FALSE, TRUE);
	    বিরতি;

	ক্ষেত্র K_BS:	// delete character before the cursor
	ক্ষেত্র K_S_BS:
	ক্ষেত্র Ctrl_H:
	    did_backspace = ins_bs(c, BACKSPACE_CHAR, &inserted_space);
	    auto_format(FALSE, TRUE);
	    যদি (did_backspace)
		MAY_TRIGGER_AUTOCOMPLETE(c);
	    বিরতি;

	ক্ষেত্র Ctrl_W:	// delete word before the cursor
#ifdef FEAT_JOB_CHANNEL
	    যদি (bt_prompt(curbuf) && (mod_mask & MOD_MASK_SHIFT) == 0)
	    {
		// In a prompt window CTRL-W is used for window commands.
		// Use Shift-CTRL-W to delete a word.
		stuffcharReadbuff(Ctrl_W);
		restart_edit = 'A';
		nomove = TRUE;
		গুনতি = 0;
		যাও doESCkey;
	    }
#endif
	    did_backspace = ins_bs(c, BACKSPACE_WORD, &inserted_space);
	    auto_format(FALSE, TRUE);
	    যদি (did_backspace)
		MAY_TRIGGER_AUTOCOMPLETE(c);
	    বিরতি;

	ক্ষেত্র Ctrl_U:	// delete all inserted text in current line
#ifdef FEAT_COMPL_FUNC
	    // CTRL-X CTRL-U completes with 'completefunc'.
	    যদি (ctrl_x_mode_function())
		যাও docomplete;
#endif
	    did_backspace = ins_bs(c, BACKSPACE_সারি, &inserted_space);
	    auto_format(FALSE, TRUE);
	    inserted_space = FALSE;
	    যদি (did_backspace)
		MAY_TRIGGER_AUTOCOMPLETE(c);
	    বিরতি;

	ক্ষেত্র K_LEFTMOUSE:   // mouse keys
	ক্ষেত্র K_LEFTMOUSE_NM:
	ক্ষেত্র K_LEFTDRAG:
	ক্ষেত্র K_LEFTRELEASE:
	ক্ষেত্র K_LEFTRELEASE_NM:
	ক্ষেত্র K_MOUSEMOVE:
	ক্ষেত্র K_MIDDLEMOUSE:
	ক্ষেত্র K_MIDDLEDRAG:
	ক্ষেত্র K_MIDDLERELEASE:
	ক্ষেত্র K_RIGHTMOUSE:
	ক্ষেত্র K_RIGHTDRAG:
	ক্ষেত্র K_RIGHTRELEASE:
	ক্ষেত্র K_X1MOUSE:
	ক্ষেত্র K_X1DRAG:
	ক্ষেত্র K_X1RELEASE:
	ক্ষেত্র K_X2MOUSE:
	ক্ষেত্র K_X2DRAG:
	ক্ষেত্র K_X2RELEASE:
	    ins_mouse(c);
	    বিরতি;

	ক্ষেত্র K_MOUSEDOWN: // Default action for scroll wheel up: scroll up
	    ins_mousescroll(MSCR_DOWN);
	    বিরতি;

	ক্ষেত্র K_MOUSEUP:	// Default action for scroll wheel down: scroll down
	    ins_mousescroll(MSCR_UP);
	    বিরতি;

	ক্ষেত্র K_MOUSELEFT: // Scroll wheel left
	    ins_mousescroll(MSCR_বাম);
	    বিরতি;

	ক্ষেত্র K_MOUSERIGHT: // Scroll wheel right
	    ins_mousescroll(MSCR_ডান);
	    বিরতি;

	ক্ষেত্র K_PS:
	    bracketed_paste(PASTE_ঢোকাও, FALSE, ফাঁকা);
	    যদি (cmdchar == K_PS)
		// invoked from normal mode, bail out
		যাও doESCkey;
	    বিরতি;
	ক্ষেত্র K_PE:
	    // Got K_PE without K_PS, ignore.
	    বিরতি;

#ifdef FEAT_GUI_TABLINE
	ক্ষেত্র K_TABLINE:
	ক্ষেত্র K_TABMENU:
	    ins_tabline(c);
	    বিরতি;
#endif

	ক্ষেত্র K_IGNORE:	// Something mapped to nothing
	    বিরতি;

	ক্ষেত্র K_COMMAND:		    // <Cmd>command<CR>
	ক্ষেত্র K_SCRIPT_COMMAND:	    // <ScriptCmd>command<CR>
	    {
		bufref_T    save_curbuf;
		varnumber_T tick = CHANGEDTICK(curbuf);

		set_bufref(&save_curbuf, curbuf);
		do_cmdkey_command(c, 0);

#ifdef FEAT_TERMINAL
		যদি (term_use_loop())
		    // Started a terminal that gets the input, exit Insert mode.
		    যাও doESCkey;
#endif
		যদি (curbuf->b_u_synced
			|| (bufref_valid(&save_curbuf)
			    && curbuf == save_curbuf.br_buf
			    && tick != CHANGEDTICK(curbuf)))
		{
		    // The command synced undo or changed this buffer.
		    // Save the cursor line before the next typed edit.
		    ins_need_undo = TRUE;
		}
	    }
	    বিরতি;

	ক্ষেত্র K_CURSORHOLD:	// Didn't type something for a while.
	    ins_apply_autocmds(EVENT_CURSORHOLDI);
	    did_cursorhold = TRUE;
	    // If CTRL-G U was used apply it to the next typed key.
	    যদি (dont_sync_undo == TRUE)
		dont_sync_undo = MAYBE;
	    বিরতি;

	ক্ষেত্র K_COMPLETE_DELAY:	// 'autocompletedelay' expired
	    // If CTRL-G U was used apply it to the next typed key.
	    যদি (dont_sync_undo == TRUE)
		dont_sync_undo = MAYBE;
	    ins_compl_clear_autocomplete_delay();
	    যদি (!ins_compl_has_autocomplete() || char_avail()
		    || curwin->w_cursor.col == 0)
		বিরতি;
	    c = char_before_cursor();
	    যদি (!vim_isprintc(c))
		বিরতি;
	    // The completion may have been cleared while waiting, so re-enable
	    // autocomplete to match a zero delay.
	    ins_compl_enable_autocomplete();
	    ins_compl_arm_autostart();
	    যাও docomplete;

#ifdef FEAT_GUI_MSWIN
	    // On MS-Windows ignore <M-F4>, we get it when closing the window
	    // was cancelled.
	ক্ষেত্র K_F4:
	    যদি (mod_mask != MOD_MASK_ALT)
		যাও normalchar;
	    বিরতি;
#endif

#ifdef FEAT_GUI
	ক্ষেত্র K_VER_SCROLLBAR:
	    ins_scroll();
	    বিরতি;

	ক্ষেত্র K_HOR_SCROLLBAR:
	    ins_horscroll();
	    বিরতি;
#endif

	ক্ষেত্র K_HOME:	// <Home>
	ক্ষেত্র K_KHOME:
	ক্ষেত্র K_S_HOME:
	ক্ষেত্র K_C_HOME:
	    ins_home(c);
	    বিরতি;

	ক্ষেত্র K_END:	// <End>
	ক্ষেত্র K_KEND:
	ক্ষেত্র K_S_END:
	ক্ষেত্র K_C_END:
	    ins_end(c);
	    বিরতি;

	ক্ষেত্র K_বাম:	// <Left>
	    যদি (mod_mask & (MOD_MASK_SHIFT|MOD_MASK_CTRL))
		ins_s_বাম();
	    অন্যথা
		ins_বাম();
	    বিরতি;

	ক্ষেত্র K_S_বাম:	// <S-Left>
	ক্ষেত্র K_C_বাম:
	    ins_s_বাম();
	    বিরতি;

	ক্ষেত্র K_ডান:	// <Right>
	    যদি (mod_mask & (MOD_MASK_SHIFT|MOD_MASK_CTRL))
		ins_s_ডান();
	    অন্যথা
		ins_ডান();
	    বিরতি;

	ক্ষেত্র K_S_ডান:	// <S-Right>
	ক্ষেত্র K_C_ডান:
	    ins_s_ডান();
	    বিরতি;

	ক্ষেত্র K_UP:	// <Up>
	    যদি (pum_visible())
		যাও docomplete;
	    যদি (mod_mask & MOD_MASK_SHIFT)
		ins_pageup();
	    অন্যথা
		ins_up(FALSE);
	    বিরতি;

	ক্ষেত্র K_S_UP:	// <S-Up>
	ক্ষেত্র K_PAGEUP:
	ক্ষেত্র K_KPAGEUP:
	    যদি (pum_visible())
	    {
#ifdef FEAT_PROP_POPUP
		// CTRL-SHIFT-<Up> scrolls the info popup up a line,
		// CTRL-SHIFT-<PageUp> a page.  Shift is folded into K_S_UP but
		// stays in mod_mask for PageUp, hence the asymmetric check.
		যদি (c == K_S_UP ? (mod_mask & MOD_MASK_CTRL)
			: ((mod_mask & MOD_MASK_CTRL)
			    && (mod_mask & MOD_MASK_SHIFT)))
		{
		    popup_scroll_info(-1, c != K_S_UP);
		    বিরতি;
		}
#endif
		যাও docomplete;
	    }
	    ins_pageup();
	    বিরতি;

	ক্ষেত্র K_DOWN:	// <Down>
	    যদি (pum_visible())
		যাও docomplete;
	    যদি (mod_mask & MOD_MASK_SHIFT)
		ins_pagedown();
	    অন্যথা
		ins_down(FALSE);
	    বিরতি;

	ক্ষেত্র K_S_DOWN:	// <S-Down>
	ক্ষেত্র K_PAGEDOWN:
	ক্ষেত্র K_KPAGEDOWN:
	    যদি (pum_visible())
	    {
#ifdef FEAT_PROP_POPUP
		// CTRL-SHIFT-<Down>/<PageDown> scroll the info popup down.
		যদি (c == K_S_DOWN ? (mod_mask & MOD_MASK_CTRL)
			: ((mod_mask & MOD_MASK_CTRL)
			    && (mod_mask & MOD_MASK_SHIFT)))
		{
		    popup_scroll_info(1, c != K_S_DOWN);
		    বিরতি;
		}
#endif
		যাও docomplete;
	    }
	    ins_pagedown();
	    বিরতি;

#ifdef FEAT_DND
	ক্ষেত্র K_DROP:	// drag-n-drop event
	    ins_drop();
	    বিরতি;
#endif

	ক্ষেত্র K_S_TAB:	// When not mapped, use like a normal TAB
	    c = TAB;
	    // FALLTHROUGH

	ক্ষেত্র TAB:	// TAB or Complete patterns along path
#যদি defined(FEAT_FIND_ID)
	    যদি (ctrl_x_mode_path_patterns())
		যাও docomplete;
#endif
	    inserted_space = FALSE;
	    যদি (ins_tab())
		যাও normalchar;	// insert TAB as a normal char
	    auto_format(FALSE, TRUE);
	    বিরতি;

	ক্ষেত্র K_KENTER:	// <Enter>
	    c = CAR;
	    // FALLTHROUGH
	ক্ষেত্র CAR:
	ক্ষেত্র NL:
#যদি defined(FEAT_QUICKFIX)
	    // In a quickfix window a <CR> jumps to the error under the
	    // cursor.
	    যদি (bt_quickfix(curbuf) && c == CAR)
	    {
		যদি (curwin->w_llist_ref == ফাঁকা)    // quickfix window
		    do_cmdline_cmd((char_u *)".cc");
		অন্যথা				    // location list window
		    do_cmdline_cmd((char_u *)".ll");
		বিরতি;
	    }
#endif
	    যদি (cmdwin_type != 0)
	    {
		// Execute the command in the cmdline window.
		cmdwin_ফলাফল = CAR;
		যাও doESCkey;
	    }
#ifdef FEAT_JOB_CHANNEL
	    যদি (bt_prompt(curbuf))
	    {
		invoke_prompt_callback();
		যদি (!bt_prompt(curbuf))
		    // buffer changed to a non-prompt buffer, get out of
		    // Insert mode
		    যাও doESCkey;
		বিরতি;
	    }
#endif
	    যদি (ins_eol(c) == FAIL && !p_im)
		যাও doESCkey;	    // out of memory
	    auto_format(FALSE, FALSE);
	    inserted_space = FALSE;
	    বিরতি;

	ক্ষেত্র Ctrl_K:	    // digraph or keyword completion
	    যদি (ctrl_x_mode_dictionary())
	    {
		যদি (has_compl_option(TRUE))
		    যাও docomplete;
		বিরতি;
	    }
#ifdef FEAT_DIGRAPHS
	    c = ins_digraph();
	    যদি (c == NUL)
		বিরতি;
#endif
	    যাও normalchar;

	ক্ষেত্র Ctrl_X:	// Enter CTRL-X mode
	    ins_ctrl_x();
	    বিরতি;

	ক্ষেত্র Ctrl_RSB:	// Tag name completion after ^X
	    যদি (!ctrl_x_mode_tags())
		যাও normalchar;
	    যাও docomplete;

	ক্ষেত্র Ctrl_F:	// File name completion after ^X
	    যদি (!ctrl_x_mode_files())
		যাও normalchar;
	    যাও docomplete;

	ক্ষেত্র 's':	// Spelling completion after ^X
	ক্ষেত্র Ctrl_S:
	    যদি (!ctrl_x_mode_spell())
		যাও normalchar;
	    যাও docomplete;

	ক্ষেত্র Ctrl_L:	// Whole line completion after ^X
	    যদি (!ctrl_x_mode_whole_সারি())
	    {
		// CTRL-L with 'insertmode' set: Leave Insert mode
		যদি (p_im)
		{
		    যদি (echeck_abbr(Ctrl_L + ABBR_OFF))
			বিরতি;
		    যাও doESCkey;
		}
		যাও normalchar;
	    }
	    // FALLTHROUGH

	ক্ষেত্র Ctrl_P:	// Do previous/next pattern completion
	ক্ষেত্র Ctrl_N:
#ifdef FEAT_PROP_POPUP
	    // CTRL-SHIFT-P/N scroll the info popup one line.
	    যদি (pum_visible() && (mod_mask & MOD_MASK_SHIFT)
		    && (c == Ctrl_P || c == Ctrl_N))
	    {
		popup_scroll_info(c == Ctrl_P ? -1 : 1, false);
		বিরতি;
	    }
#endif
	    // if 'complete' is empty then plain ^P is no longer special,
	    // but it is under other ^X modes
	    যদি (*curbuf->b_p_cpt == NUL
		    && (ctrl_x_mode_normal() || ctrl_x_mode_whole_সারি())
		    && !compl_status_local())
		যাও normalchar;

docomplete:
	    ins_compl_clear_autocomplete_delay();
	    compl_busy = TRUE;
#ifdef FEAT_FOLDING
	    disable_fold_update++;  // don't redraw folds here
#endif
	    যদি (ins_complete(c, TRUE) == FAIL)
		compl_status_clear();
#ifdef FEAT_FOLDING
	    disable_fold_update--;
#endif
	    compl_busy = FALSE;
	    can_si = may_do_si(); // allow smartindenting
	    বিরতি;

	ক্ষেত্র Ctrl_Y:	// copy from previous line or scroll down
	ক্ষেত্র Ctrl_E:	// copy from next line	   or scroll up
	    c = ins_ctrl_ey(c);
	    বিরতি;

	পূর্বনির্ধারিত:
#ifdef UNIX
	    যদি (c == intr_char)		// special interrupt char
		যাও do_intr;
#endif

normalchar:
	    /*
	     * Insert a normal character.
	     */
#যদি defined(FEAT_EVAL)
	    যদি (!p_paste)
	    {
		// Trigger InsertCharPre.
		char_u *str = do_ঢোকাও_char_pre(c);
		char_u *p;

		যদি (str != ফাঁকা)
		{
		    যদি (*str != NUL && stop_arrow() != FAIL)
		    {
			// Insert the new value of v:char literally.
			জন্য (p = str; *p != NUL; MB_PTR_ADV(p))
			{
			    c = PTR2CHAR(p);
			    যদি (c == CAR || c == K_KENTER || c == NL)
				ins_eol(c);
			    অন্যথা
				ins_char(c);
			}
			AppendToRedobuffLit(str, -1);
		    }
		    vim_free(str);
		    c = NUL;
		}

		// If the new value is already inserted or an empty string
		// then don't insert any character.
		যদি (c == NUL)
		    বিরতি;
	    }
#endif
	    // Try to perform smart-indenting.
	    ins_try_si(c);

	    যদি (c == ' ')
	    {
		inserted_space = TRUE;
		যদি (inindent(0))
		    can_cindent = FALSE;
		যদি (Insstart_blank_vcol == MAXCOL
			&& curwin->w_cursor.lnum == Insstart.lnum)
		    Insstart_blank_vcol = get_nolist_virtcol();
	    }

	    // Insert a normal character and check for abbreviations on a
	    // special character.  Let CTRL-] expand abbreviations without
	    // inserting it.
	    যদি (vim_iswordc(c) || (!echeck_abbr(
			// Add ABBR_OFF for characters above 0x100, this is
			// what check_abbr() expects.
				(has_mbyte && c >= 0x100) ? (c + ABBR_OFF) : c)
			&& c != Ctrl_RSB))
	    {
		ঢোকাও_special(c, FALSE, FALSE);
#ifdef FEAT_RIGHTLEFT
		revins_legal++;
		revins_chars++;
#endif
	    }

	    auto_format(FALSE, TRUE);

#ifdef FEAT_FOLDING
	    // When inserting a character the cursor line must never be in a
	    // closed fold.
	    foldOpenCursor();
#endif
	    // Trigger autocompletion
	    যদি (ins_compl_has_autocomplete() && !char_avail()
		    && vim_isprintc(c))
		TRIGGER_AUTOCOMPLETE();

	    বিরতি;
	}   // end of switch (c)

	// If typed something may trigger CursorHoldI again; K_COMPLETE_DELAY is
	// injected, not typed.
	যদি (c != K_CURSORHOLD && c != K_COMPLETE_DELAY
#ifdef FEAT_COMPL_FUNC
		// but not in CTRL-X mode, a script can't restore the state
		&& ctrl_x_mode_normal()
#endif
	       )
	    did_cursorhold = FALSE;

	// Check if we need to cancel completion mode because the window
	// or tab page was changed
	যদি (ins_compl_active() && !ins_compl_win_active(curwin))
	    ins_compl_cancel();

	// If the cursor was moved we didn't just insert a space
	যদি (arrow_used)
	    inserted_space = FALSE;

	যদি (can_cindent && cindent_on() && ctrl_x_mode_normal())
	{
force_cindent:
	    /*
	     * Indent now if a key was typed that is in 'cinkeys'.
	     */
	    যদি (in_cinkeys(c, ' ', সারি_is_white))
	    {
		যদি (stop_arrow() == OK)
		    // re-indent the current line
		    do_c_রাশিমালা_indent();
	    }
	}

    }	// for (;;)
    // NOTREACHED
}

    পূর্ণসংখ্যা
ins_need_undo_get(শূন্য)
{
    ফেরত ins_need_undo;
}

/*
 * Redraw for Insert mode.
 * This is postponed until getting the next character to make '$' in the 'cpo'
 * option work correctly.
 * Only redraw when there are no characters available.  This speeds up
 * inserting sequences of characters (e.g., for CTRL-R).
 */
    শূন্য
ins_redraw(পূর্ণসংখ্যা ready)	    // not busy with something
{
#ifdef FEAT_CONCEAL
    linenr_T	conceal_old_cursor_সারি = 0;
    linenr_T	conceal_new_cursor_সারি = 0;
    পূর্ণসংখ্যা		conceal_update_সারিগুলি = FALSE;
#endif

    যদি (char_avail())
	ফেরত;

    // Trigger CursorMoved if the cursor moved.  Not when the popup menu is
    // visible, the command might delete it.
    যদি (ready && (has_cursormovedI()
#ifdef FEAT_PROP_POPUP
		|| popup_visible
#endif
#যদি defined(FEAT_CONCEAL)
		|| curwin->w_p_cole > 0
#endif
		)
	    && !EQUAL_POS(last_cursormoved, curwin->w_cursor)
	    && !pum_visible())
    {
#ifdef FEAT_SYN_HL
	// Need to update the screen first, to make sure syntax
	// highlighting is correct after making a change (e.g., inserting
	// a "(".  The autocommand may also require a redraw, so it's done
	// again below, unfortunately.
	যদি (syntax_present(curwin) && must_redraw)
	    update_screen(0);
#endif
	যদি (has_cursormovedI())
	{
	    // Make sure curswant is correct, an autocommand may call
	    // getcurpos().
	    update_curswant();
	    ins_apply_autocmds(EVENT_CURSORMOVEDI);
	}
#ifdef FEAT_PROP_POPUP
	যদি (popup_visible)
	    popup_check_cursor_pos();
#endif
#ifdef FEAT_CONCEAL
	যদি (curwin->w_p_cole > 0)
	{
	    conceal_old_cursor_সারি = last_cursormoved.lnum;
	    conceal_new_cursor_সারি = curwin->w_cursor.lnum;
	    conceal_update_সারিগুলি = TRUE;
	}
#endif
	last_cursormoved = curwin->w_cursor;
    }

    // Trigger TextChangedI if b_changedtick_i differs.
    যদি (ready && has_textchangedI()
	    && curbuf->b_last_changedtick_i != CHANGEDTICK(curbuf)
	    && !pum_visible())
    {
	aco_save_T	aco;
	varnumber_T	tick = CHANGEDTICK(curbuf);

	// Save and restore curwin and curbuf, in case the autocmd changes
	// them.
	aucmd_prepbuf(&aco, curbuf);
	apply_autocmds(EVENT_TEXTCHANGEDI, ফাঁকা, ফাঁকা, FALSE, curbuf);
	aucmd_restbuf(&aco);
	curbuf->b_last_changedtick_i = CHANGEDTICK(curbuf);
	যদি (tick != CHANGEDTICK(curbuf))  // see ins_apply_autocmds()
	    u_save(curwin->w_cursor.lnum,
					(linenr_T)(curwin->w_cursor.lnum + 1));
    }

    // Trigger TextChangedP if b_changedtick_pum differs. When the popupmenu
    // closes TextChangedI will need to trigger for backwards compatibility,
    // thus use different b_last_changedtick* variables.
    যদি (ready && has_textchangedP()
	    && curbuf->b_last_changedtick_pum != CHANGEDTICK(curbuf)
	    && pum_visible())
    {
	aco_save_T	aco;
	varnumber_T	tick = CHANGEDTICK(curbuf);

	// Save and restore curwin and curbuf, in case the autocmd changes
	// them.
	aucmd_prepbuf(&aco, curbuf);
	apply_autocmds(EVENT_TEXTCHANGEDP, ফাঁকা, ফাঁকা, FALSE, curbuf);
	aucmd_restbuf(&aco);
	curbuf->b_last_changedtick_pum = CHANGEDTICK(curbuf);
	যদি (tick != CHANGEDTICK(curbuf))  // see ins_apply_autocmds()
	    u_save(curwin->w_cursor.lnum,
					(linenr_T)(curwin->w_cursor.lnum + 1));
    }

    যদি (ready)
	may_trigger_win_scrolled_resized();

    // Trigger SafeState if nothing is pending.
    may_trigger_safestate(ready
	    && !ins_compl_active()
	    && !pum_visible());

#যদি defined(FEAT_CONCEAL)
    যদি ((conceal_update_সারিগুলি
	    && (conceal_old_cursor_সারি != conceal_new_cursor_সারি
		|| conceal_cursor_সারি(curwin)))
	    || need_cursor_সারি_redraw)
    {
	যদি (conceal_old_cursor_সারি != conceal_new_cursor_সারি)
	    redrawWinline(curwin, conceal_old_cursor_সারি);
	redrawWinline(curwin, conceal_new_cursor_সারি == 0
			    ? curwin->w_cursor.lnum : conceal_new_cursor_সারি);
	curwin->w_valid &= ~VALID_CROW;
	need_cursor_সারি_redraw = FALSE;
    }
#endif
    যদি (must_redraw)
	update_screen(0);
    অন্যথা যদি (clear_cmdline || redraw_cmdline)
	showmode();		// clear cmdline and show mode
    showruler(FALSE);
    setcursor();
    emsg_on_display = FALSE;	// may remove error message now
}

/*
 * Handle a CTRL-V or CTRL-Q typed in Insert mode.
 */
    স্থির শূন্য
ins_ctrl_v(শূন্য)
{
    পূর্ণসংখ্যা		c;
    পূর্ণসংখ্যা		did_putchar = FALSE;

    // may need to redraw when no more chars available now
    ins_redraw(FALSE);

    যদি (redrawing() && !char_avail())
    {
	edit_putchar('^', TRUE);
	did_putchar = TRUE;
    }
    AppendToRedobuff((char_u *)CTRL_V_STR);	// CTRL-V

    add_to_showcmd_c(Ctrl_V);

    // Do not change any modifyOtherKeys ESC sequence to a normal key for
    // CTRL-SHIFT-V.
    c = get_literal(mod_mask & MOD_MASK_SHIFT);
    যদি (did_putchar)
	// when the line fits in 'columns' the '^' is at the start of the next
	// line and will not removed by the redraw
	edit_unputchar();
    clear_showcmd();

    ঢোকাও_special(c, FALSE, TRUE);
#ifdef FEAT_RIGHTLEFT
    revins_chars++;
    revins_legal++;
#endif
}

/*
 * After getting an ESC or CSI for a literal key: If the typeahead buffer
 * contains a modifyOtherKeys sequence then decode it and return the result.
 * Otherwise return "c".
 * Note that this doesn't wait for characters, they must be in the typeahead
 * buffer already.
 */
    স্থির পূর্ণসংখ্যা
decode_Modify_Other_চাবিগুলি(পূর্ণসংখ্যা c)
{
    char_u  *p = typebuf.tb_buf + typebuf.tb_off;
    পূর্ণসংখ্যা	    idx;
    পূর্ণসংখ্যা	    form = 0;
    পূর্ণসংখ্যা	    argidx = 0;
    পূর্ণসংখ্যা	    arg[2] = {0, 0};

    // Recognize:
    // form 0: {lead}{key};{modifier}u
    // form 1: {lead}27;{modifier};{key}~
    যদি (typebuf.tb_len >= 4 && (c == CSI || (c == ESC && *p == '[')))
    {
	idx = (*p == '[');
	যদি (p[idx] == '2' && p[idx + 1] == '7' && p[idx + 2] == ';' &&
		kitty_protocol_state != KKPS_ENABLED)
	{
	    form = 1;
	    idx += 3;
	}
	যখন (idx < typebuf.tb_len && argidx < 2)
	{
	    যদি (p[idx] == ';')
		++argidx;
	    অন্যথা যদি (VIM_ISDIGIT(p[idx]))
		arg[argidx] = arg[argidx] * 10 + (p[idx] - '0');
	    অন্যথা
		বিরতি;
	    ++idx;
	}
	পূর্ণসংখ্যা kitty_no_mods = argidx == 0 && kitty_protocol_state == KKPS_ENABLED;
	যদি (idx < typebuf.tb_len
		&& p[idx] == (form == 1 ? '~' : 'u')
		&& (argidx == 1 || kitty_no_mods))
	{
	    // Match, consume the code.
	    typebuf.tb_off += idx + 1;
	    typebuf.tb_len -= idx + 1;
#যদি defined(FEAT_CLIENTSERVER) || defined(FEAT_EVAL)
	    যদি (typebuf.tb_len == 0)
		typebuf_was_filled = FALSE;
#endif

	    mod_mask = kitty_no_mods ? 0 : decode_modifiers(arg[!form]);
	    c = merge_modify_Other_চাবিগুলি(arg[form], &mod_mask);
	}
    }

    ফেরত c;
}

/*
 * Put a character directly onto the screen.  It's not stored in a buffer.
 * Used while handling CTRL-K, CTRL-V, etc. in Insert mode.
 */
স্থির পূর্ণসংখ্যা  pc_status;
#define PC_STATUS_UNSET	0	// pc_bytes was not set
#define PC_STATUS_ডান	1	// right half of double-wide char
#define PC_STATUS_বাম	2	// left half of double-wide char
#define PC_STATUS_SET	3	// pc_bytes was filled
স্থির char_u pc_bytes[MB_MAXBYTES + 1]; // saved bytes
স্থির পূর্ণসংখ্যা  pc_attr;
স্থির পূর্ণসংখ্যা  pc_row;
স্থির পূর্ণসংখ্যা  pc_col;

    শূন্য
edit_putchar(পূর্ণসংখ্যা c, পূর্ণসংখ্যা highlight)
{
    পূর্ণসংখ্যা	    attr;

    যদি (Screen_সারিগুলি == ফাঁকা)
	ফেরত;

    update_topline();	// just in case w_topline isn't valid
    validate_cursor();
    যদি (highlight)
	attr = HL_ATTR(HLF_8);
    অন্যথা
	attr = 0;
    pc_row = W_WINROW(curwin) + curwin->w_wrow;
    pc_col = curwin->w_wincol;
    pc_status = PC_STATUS_UNSET;
#ifdef FEAT_RIGHTLEFT
    যদি (curwin->w_p_rl)
    {
	pc_col += curwin->w_width - 1 - curwin->w_wcol;
	যদি (has_mbyte)
	{
	    পূর্ণসংখ্যা fix_col = mb_fix_col(pc_col, pc_row);

	    যদি (fix_col != pc_col)
	    {
		screen_putchar(' ', pc_row, fix_col, attr);
		--curwin->w_wcol;
		pc_status = PC_STATUS_ডান;
	    }
	}
    }
    অন্যথা
#endif
    {
	pc_col += curwin->w_wcol;
	যদি (mb_lefthalve(pc_row, pc_col))
	    pc_status = PC_STATUS_বাম;
    }

    // save the character to be able to put it back
    যদি (pc_status == PC_STATUS_UNSET)
    {
	screen_getbytes(pc_row, pc_col, pc_bytes, &pc_attr);
	pc_status = PC_STATUS_SET;
    }
    screen_putchar(c, pc_row, pc_col, attr);
}

#যদি defined(FEAT_JOB_CHANNEL)
/*
 * Set the insert start position for when using a prompt buffer.
 */
    শূন্য
set_insstart(linenr_T lnum, পূর্ণসংখ্যা col)
{
    Insstart.lnum = lnum;
    Insstart.col = col;
    Insstart_orig = Insstart;
    Insstart_textlen = Insstart.col;
    Insstart_blank_vcol = MAXCOL;
    arrow_used = FALSE;
}
#endif

/*
 * Undo the previous edit_putchar().
 */
    শূন্য
edit_unputchar(শূন্য)
{
    যদি (pc_status != PC_STATUS_UNSET && pc_row >= বার্তা_scrolled)
    {
	যদি (pc_status == PC_STATUS_ডান)
	    ++curwin->w_wcol;
	যদি (pc_status == PC_STATUS_ডান || pc_status == PC_STATUS_বাম)
	    redrawWinline(curwin, curwin->w_cursor.lnum);
	অন্যথা
	    screen_puts(pc_bytes, pc_row - বার্তা_scrolled, pc_col, pc_attr);
    }
}

/*
 * Called when "$" is in 'cpoptions': display a '$' at the end of the changed
 * text.  Only works when cursor is in the line that changes.
 */
    শূন্য
display_dollar(colnr_T col_arg)
{
    colnr_T col = col_arg < 0 ? 0 : col_arg;
    colnr_T save_col;

    যদি (!redrawing())
	ফেরত;

    cursor_off();
    save_col = curwin->w_cursor.col;
    curwin->w_cursor.col = col;
    যদি (has_mbyte)
    {
	char_u *p;

	// If on the last byte of a multi-byte move to the first byte.
	p = ml_get_curline();
	curwin->w_cursor.col -= (*mb_শীর্ষ_off)(p, p + col);
    }
    curs_columns(FALSE);	    // recompute w_wrow and w_wcol
    যদি (curwin->w_wcol < curwin->w_width)
    {
	edit_putchar('$', FALSE);
	dollar_vcol = curwin->w_virtcol;
    }
    curwin->w_cursor.col = save_col;
}

/*
 * Call this function before moving the cursor from the normal insert position
 * in insert mode.
 */
    শূন্য
undisplay_dollar(শূন্য)
{
    যদি (dollar_vcol < 0)
	ফেরত;

    dollar_vcol = -1;
    redrawWinline(curwin, curwin->w_cursor.lnum);
}

/*
 * Truncate the space at the end of a line.  This is to be used only in an
 * insert mode.  It handles fixing the replace stack for MODE_REPLACE and
 * MODE_VREPLACE modes.
 */
    শূন্য
truncate_spaces(char_u *সারি, size_t len)
{
    পূর্ণসংখ্যা	    i;

    // find start of trailing white space
    জন্য (i = (পূর্ণসংখ্যা)len - 1; i >= 0 && VIM_ISWHITE(সারি[i]); i--)
    {
	যদি (State & REPLACE_FLAG)
	    replace_join(0);	    // remove a NUL from the replace stack
    }
    সারি[i + 1] = NUL;
}

/*
 * Backspace the cursor until the given column.  Handles MODE_REPLACE and
 * MODE_VREPLACE modes correctly.  May also be used when not in insert mode at
 * all.  Will attempt not to go before "col" even when there is a composing
 * character.
 */
    শূন্য
backspace_until_column(পূর্ণসংখ্যা col)
{
    যখন ((পূর্ণসংখ্যা)curwin->w_cursor.col > col)
    {
	curwin->w_cursor.col--;
	যদি (State & REPLACE_FLAG)
	    replace_do_bs(col);
	অন্যথা যদি (!del_char_after_col(col))
	    বিরতি;
    }
}

/*
 * Like del_char(), but make sure not to go before column "limit_col".
 * Only matters when there are composing characters.
 * Return TRUE when something was deleted.
 */
    স্থির পূর্ণসংখ্যা
del_char_after_col(পূর্ণসংখ্যা limit_col UNUSED)
{
    যদি (enc_utf8 && limit_col >= 0)
    {
	colnr_T ecol = curwin->w_cursor.col + 1;

	// Make sure the cursor is at the start of a character, but
	// skip forward again when going too far back because of a
	// composing character.
	mb_adjust_cursor();
	যখন (curwin->w_cursor.col < (colnr_T)limit_col)
	{
	    পূর্ণসংখ্যা l = utf_ptr2len(ml_get_cursor());

	    যদি (l == 0)  // end of line
		বিরতি;
	    curwin->w_cursor.col += l;
	}
	যদি (*ml_get_cursor() == NUL || curwin->w_cursor.col == ecol)
	    ফেরত FALSE;
	del_bytes((long)((পূর্ণসংখ্যা)ecol - curwin->w_cursor.col), FALSE, TRUE);
    }
    অন্যথা
	(শূন্য)del_char(FALSE);
    ফেরত TRUE;
}

/*
 * Next character is interpreted literally.
 * A one, two or three digit decimal number is interpreted as its byte value.
 * If one or two digits are entered, the next character is given to vungetc().
 * For Unicode a character > 255 may be returned.
 * If "noReduceKeys" is TRUE do not change any modifyOtherKeys ESC sequence
 * into a normal key, return ESC.
 */
    পূর্ণসংখ্যা
get_literal(পূর্ণসংখ্যা no_Reduce_চাবিগুলি)
{
    পূর্ণসংখ্যা		cc;
    পূর্ণসংখ্যা		nc;
    পূর্ণসংখ্যা		i;
    পূর্ণসংখ্যা		hex = FALSE;
    পূর্ণসংখ্যা		octal = FALSE;
    পূর্ণসংখ্যা		unicode = 0;

    যদি (got_int)
	ফেরত Ctrl_C;

#ifdef FEAT_GUI
    /*
     * In GUI there is no point inserting the internal code for a special key.
     * It is more useful to insert the string "<KEY>" instead.	This would
     * probably be useful in a text window too, but it would not be
     * vi-compatible (maybe there should be an option for it?) -- webb
     */
    যদি (gui.in_use)
    {
	++allow_চাবিগুলি;
	যদি (no_Reduce_চাবিগুলি)
	    ++no_reduce_চাবিগুলি;
    }
#endif
#ifdef USE_ON_FLY_SCROLL
    dont_scroll = TRUE;		// disallow scrolling here
#endif
    ++no_mapping;		// don't map the next key hits
    cc = 0;
    i = 0;
    জন্য (;;)
    {
	nc = plain_vgetc();
	যদি ((nc == ESC || nc == CSI) && !no_Reduce_চাবিগুলি)
	    nc = decode_Modify_Other_চাবিগুলি(nc);

	যদি ((mod_mask & ~MOD_MASK_SHIFT) != 0)
	    // A character with non-Shift modifiers should not be a valid
	    // character for i_CTRL-V_digit.
	    বিরতি;

	যদি ((State & MODE_CMDLINE) == 0 && MB_BYTE2LEN_CHECK(nc) == 1)
	    add_to_showcmd(nc);
	যদি (nc == 'x' || nc == 'X')
	    hex = TRUE;
	অন্যথা যদি (nc == 'o' || nc == 'O')
	    octal = TRUE;
	অন্যথা যদি (nc == 'u' || nc == 'U')
	    unicode = nc;
	অন্যথা
	{
	    যদি (hex || unicode != 0)
	    {
		যদি (!vim_isxdigit(nc))
		    বিরতি;
		cc = cc * 16 + hex2nr(nc);
	    }
	    অন্যথা যদি (octal)
	    {
		যদি (nc < '0' || nc > '7')
		    বিরতি;
		cc = cc * 8 + nc - '0';
	    }
	    অন্যথা
	    {
		যদি (!VIM_ISDIGIT(nc))
		    বিরতি;
		cc = cc * 10 + nc - '0';
	    }

	    ++i;
	}

	যদি (cc > 255 && unicode == 0)
	    cc = 255;		// limit range to 0-255
	nc = 0;

	যদি (hex)		// hex: up to two chars
	{
	    যদি (i >= 2)
		বিরতি;
	}
	অন্যথা যদি (unicode)	// Unicode: up to four or eight chars
	{
	    যদি ((unicode == 'u' && i >= 4) || (unicode == 'U' && i >= 8))
		বিরতি;
	}
	অন্যথা যদি (i >= 3)	// decimal or octal: up to three chars
	    বিরতি;
    }
    যদি (i == 0)	    // no number entered
    {
	যদি (nc == K_ZERO)   // NUL is stored as NL
	{
	    cc = '\n';
	    nc = 0;
	}
	অন্যথা
	{
	    cc = nc;
	    nc = 0;
	}
    }

    যদি (cc == 0)	// NUL is stored as NL
	cc = '\n';
    যদি (enc_dbcs && (cc & 0xff) == 0)
	cc = '?';	// don't accept an illegal DBCS char, the NUL in the
			// second byte will cause trouble!

    --no_mapping;
#ifdef FEAT_GUI
    যদি (gui.in_use)
    {
	--allow_চাবিগুলি;
	যদি (no_Reduce_চাবিগুলি)
	    --no_reduce_চাবিগুলি;
    }
#endif
    যদি (nc)
    {
	vungetc(nc);
	// A character typed with i_CTRL-V_digit cannot have modifiers.
	mod_mask = 0;
    }
    got_int = FALSE;	    // CTRL-C typed after CTRL-V is not an interrupt
    ফেরত cc;
}

/*
 * Insert character, taking care of special keys and mod_mask
 */
    স্থির শূন্য
ঢোকাও_special(
    পূর্ণসংখ্যা	    c,
    পূর্ণসংখ্যা	    allow_modmask,
    পূর্ণসংখ্যা	    ctrlv)	    // c was typed after CTRL-V
{
    char_u  *p;
    পূর্ণসংখ্যা	    len;

    /*
     * Special function key, translate into "<Key>". Up to the last '>' is
     * inserted with ins_str(), so as not to replace characters in replace
     * mode.
     * Only use mod_mask for special keys, to avoid things like <S-Space>,
     * unless 'allow_modmask' is TRUE.
     */
#যদি defined(MACOS_X) || defined(FEAT_GUI_GTK)
    // Command-key never produces a normal key
    যদি (mod_mask & MOD_MASK_CMD)
	allow_modmask = TRUE;
#endif
    যদি (IS_SPECIAL(c) || (mod_mask && allow_modmask))
    {
	p = get_special_চাবি_name(c, mod_mask);
	len = (পূর্ণসংখ্যা)STRLEN(p);
	c = p[len - 1];
	যদি (len > 2)
	{
	    যদি (stop_arrow() == FAIL)
		ফেরত;
	    p[len - 1] = NUL;
	    ins_str(p, len - 1);
	    AppendToRedobuffLit(p, -1);
	    ctrlv = FALSE;
	}
    }
    যদি (stop_arrow() == OK)
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
#define ISSPECIAL(c)	((c) < ' ' || (c) >= DEL || (c) == '0' || (c) == '^')

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
    শূন্য
insertchar(
    পূর্ণসংখ্যা		c,			// character to insert or NUL
    পূর্ণসংখ্যা		flags,			// INSCHAR_FORMAT, etc.
    পূর্ণসংখ্যা		second_indent)		// indent for second line if >= 0
{
    পূর্ণসংখ্যা		textwidth;
    char_u	*p;
    পূর্ণসংখ্যা		fo_ins_blank;
    পূর্ণসংখ্যা		force_format = flags & INSCHAR_FORMAT;

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
    যদি (textwidth > 0
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
#যদি defined(FEAT_EVAL)
	পূর্ণসংখ্যা     do_internal = TRUE;
	colnr_T virtcol = get_nolist_virtcol()
				  + char2cells(c != NUL ? c : gchar_cursor());

	যদি (*curbuf->b_p_fex != NUL && (flags & INSCHAR_NO_FEX) == 0
		&& (force_format || virtcol > (colnr_T)textwidth))
	{
	    do_internal = (fex_format(curwin->w_cursor.lnum, 1L, c) != 0);
	    // It may be required to save for undo again, e.g. when setline()
	    // was called.
	    ins_need_undo = TRUE;
	}
	যদি (do_internal)
#endif
	    internal_format(textwidth, second_indent, flags, c == NUL, c);
    }

    যদি (c == NUL)	    // only formatting was wanted
	ফেরত;

    // Check whether this character should end a comment.
    যদি (did_ai && c == end_comment_pending)
    {
	char_u  *সারি;
	char_u	lead_end[COM_MAX_LEN];	    // end-comment string
	পূর্ণসংখ্যা	মধ্য_len, end_len;
	পূর্ণসংখ্যা	i;

	/*
	 * Need to remove existing (middle) comment leader and insert end
	 * comment leader.  First, check what comment leader we can find.
	 */
	i = get_leader_len(সারি = ml_get_curline(), &p, FALSE, TRUE);
	যদি (i > 0 && vim_strchr(p, COM_মধ্য) != ফাঁকা)	// Just checking
	{
	    // Skip middle-comment string
	    যখন (*p && p[-1] != ':')	// find end of middle flags
		++p;
	    মধ্য_len = copy_option_part(&p, lead_end, COM_MAX_LEN, ",");
	    // Don't count trailing white space for middle_len
	    যখন (মধ্য_len > 0 && VIM_ISWHITE(lead_end[মধ্য_len - 1]))
		--মধ্য_len;

	    // Find the end-comment string
	    যখন (*p && p[-1] != ':')	// find end of end flags
		++p;
	    end_len = copy_option_part(&p, lead_end, COM_MAX_LEN, ",");

	    // Skip white space before the cursor
	    i = curwin->w_cursor.col;
	    যখন (--i >= 0 && VIM_ISWHITE(সারি[i]))
		;
	    i++;

	    // Skip to before the middle leader
	    i -= মধ্য_len;

	    // Check some expected things before we go on
	    যদি (i >= 0 && end_len > 0 && lead_end[end_len - 1] == end_comment_pending)
	    {
		// Backspace over all the stuff we want to replace
		backspace_until_column(i);

		// Insert the end-comment string, except for the last
		// character, which will get inserted as normal later.
		ins_bytes_len(lead_end, end_len - 1);
	    }
	}
    }
    end_comment_pending = NUL;

    did_ai = FALSE;
    did_si = FALSE;
    can_si = FALSE;
    can_si_back = FALSE;

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
#ifdef USE_ON_FLY_SCROLL
    dont_scroll = FALSE;		// allow scrolling here
#endif

    যদি (       !ISSPECIAL(c)
	    && (!has_mbyte || (*mb_char2len)(c) == 1)
	    && !has_insertcharpre()
#ifdef FEAT_EVAL
	    // Skip typeahead if test_override("char_avail", 1) was called.
	    && !disable_char_avail_for_testing
#endif
	    && vpeekc() != NUL
	    && !(State & REPLACE_FLAG)
	    && !cindent_on()
#ifdef FEAT_RIGHTLEFT
	    && !p_ri
#endif
	   )
    {
#define প্রবেশ_BUFLEN 100
	char_u		buf[প্রবেশ_BUFLEN + 1];
	পূর্ণসংখ্যা		i;
	colnr_T		virtcol = 0;

	buf[0] = c;
	i = 1;
	যদি (textwidth > 0)
	    virtcol = get_nolist_virtcol();
	/*
	 * Stop the string when:
	 * - no more chars available
	 * - finding a special character (command key)
	 * - buffer is full
	 * - running into the 'textwidth' boundary
	 * - need to check for abbreviation: A non-word char after a word-char
	 */
	যখন (	   (c = vpeekc()) != NUL
		&& !ISSPECIAL(c)
		&& (!has_mbyte || MB_BYTE2LEN_CHECK(c) == 1)
		&& i < প্রবেশ_BUFLEN
		&& (textwidth == 0
		    || (virtcol += byte2cells(buf[i - 1])) < (colnr_T)textwidth)
		&& !(!no_abbr && !vim_iswordc(c) && vim_iswordc(buf[i - 1])))
	{
#ifdef FEAT_RIGHTLEFT
	    c = vgetc();
	    যদি (p_hkmap && চাবি_Typed)
		c = hkmap(c);		    // Hebrew mode mapping
	    buf[i++] = c;
#অন্যথা
	    buf[i++] = vgetc();
#endif
	}

#ifdef FEAT_DIGRAPHS
	do_digraph(-1);			// clear digraphs
	do_digraph(buf[i-1]);		// may be the start of a digraph
#endif
	buf[i] = NUL;
	ins_str(buf, i);
	যদি (flags & INSCHAR_CTRLV)
	{
	    redo_literal(*buf);
	    i = 1;
	}
	অন্যথা
	    i = 0;
	যদি (buf[i] != NUL)
	    AppendToRedobuffLit(buf + i, -1);
    }
    অন্যথা
    {
	পূর্ণসংখ্যা		cc;

	যদি (has_mbyte && (cc = (*mb_char2len)(c)) > 1)
	{
	    char_u	buf[MB_MAXBYTES + 1];

	    (*mb_char2bytes)(c, buf);
	    buf[cc] = NUL;
	    ins_char_bytes(buf, cc);
	    AppendCharToRedobuff(c);
	}
	অন্যথা
	{
	    ins_char(c);
	    যদি (flags & INSCHAR_CTRLV)
		redo_literal(c);
	    অন্যথা
		AppendCharToRedobuff(c);
	}
    }
}

/*
 * Put a character in the redo buffer, for when just after a CTRL-V.
 */
    স্থির শূন্য
redo_literal(পূর্ণসংখ্যা c)
{
    char_u	buf[10];

    // Only digits need special treatment.  Translate them into a string of
    // three digits.
    যদি (VIM_ISDIGIT(c))
    {
	vim_snprintf((অক্ষর *)buf, আকার(buf), "%03d", c);
	AppendToRedobuff(buf);
    }
    অন্যথা
	AppendCharToRedobuff(c);
}

/*
 * start_arrow() is called when an arrow key is used in insert mode.
 * For undo/redo it resembles hitting the <ESC> key.
 */
    শূন্য
start_arrow(
    pos_T    *end_ঢোকাও_pos)		// can be NULL
{
    start_arrow_common(end_ঢোকাও_pos, TRUE);
}

/*
 * Like start_arrow() but with end_change argument.
 * Will prepare for redo of CTRL-G U if "end_change" is FALSE.
 */
    স্থির শূন্য
start_arrow_with_change(
    pos_T    *end_ঢোকাও_pos,		// can be NULL
    পূর্ণসংখ্যা	      end_change)		// end undoable change
{
    start_arrow_common(end_ঢোকাও_pos, end_change);
    যদি (!end_change)
    {
	AppendCharToRedobuff(Ctrl_G);
	AppendCharToRedobuff('U');
    }
}

    স্থির শূন্য
start_arrow_common(
    pos_T    *end_ঢোকাও_pos,		// can be NULL
    পূর্ণসংখ্যা	      end_change)		// end undoable change
{
    যদি (!arrow_used && end_change)	// something has been inserted
    {
	AppendToRedobuff(ESC_STR);
	stop_ঢোকাও(end_ঢোকাও_pos, FALSE, FALSE);
	arrow_used = TRUE;	// this means we stopped the current insert
    }
#ifdef FEAT_SPELL
    check_spell_redraw();
#endif
}

#ifdef FEAT_SPELL
/*
 * If we skipped highlighting word at cursor, do it now.
 * It may be skipped again, thus reset spell_redraw_lnum first.
 */
    স্থির শূন্য
check_spell_redraw(শূন্য)
{
    যদি (spell_redraw_lnum != 0)
    {
	linenr_T	lnum = spell_redraw_lnum;

	spell_redraw_lnum = 0;
	redrawWinline(curwin, lnum);
    }
}

#endif

/*
 * stop_arrow() is called before a change is made in insert mode.
 * If an arrow key has been used, start a new insertion.
 * Returns FAIL if undo is impossible, shouldn't insert then.
 */
    পূর্ণসংখ্যা
stop_arrow(শূন্য)
{
    যদি (arrow_used)
    {
	Insstart = curwin->w_cursor;	// new insertion starts here
	যদি (Insstart.col > Insstart_orig.col && !ins_need_undo)
	    // Don't update the original insert position when moved to the
	    // right, except when nothing was inserted yet.
	    update_Insstart_orig = FALSE;
	Insstart_textlen = (colnr_T)linetabsize_str(ml_get_curline());

	যদি (u_save_cursor() == OK)
	{
	    arrow_used = FALSE;
	    ins_need_undo = FALSE;
	}

	ai_col = 0;
	যদি (State & VREPLACE_FLAG)
	{
	    orig_সারি_গুনতি = curbuf->b_ml.ml_সারি_গুনতি;
	    vr_সারিগুলি_changed = 1;
	}
	ResetRedobuff();
	AppendToRedobuff((char_u *)"1i");   // pretend we start an insertion
	new_ঢোকাও_skip = 2;
    }
    অন্যথা যদি (ins_need_undo)
    {
	যদি (u_save_cursor() == OK)
	{
	    // A command or event may have moved the cursor before the next
	    // edit. Pull Insstart back only when the cursor moved above it,
	    // so that later edits can properly decide whether an extra undo
	    // entry is needed. Advancing Insstart would mis-place '[ after a
	    // register paste.
	    যদি (LT_POS(curwin->w_cursor, Insstart))
	    {
		Insstart = curwin->w_cursor;
		Insstart_textlen = (colnr_T)linetabsize_str(ml_get_curline());
	    }
	    ins_need_undo = FALSE;
	}
    }

#ifdef FEAT_FOLDING
    // Always open fold at the cursor line when inserting something.
    foldOpenCursor();
#endif

    ফেরত (arrow_used || ins_need_undo ? FAIL : OK);
}

/*
 * Do a few things to stop inserting.
 * "end_insert_pos" is where insert ended.  It is NULL when we already jumped
 * to another window/buffer.
 */
    স্থির শূন্য
stop_ঢোকাও(
    pos_T	*end_ঢোকাও_pos,
    পূর্ণসংখ্যা		esc,			// called by ins_esc()
    পূর্ণসংখ্যা		nomove)			// <c-\><c-o>, don't move cursor
{
    পূর্ণসংখ্যা		cc;
    string_T	inserted;

    stop_redo_ins();
    replace_flush();		// abandon replace stack

    /*
     * Save the inserted text for later redo with ^@ and CTRL-A.
     * Don't do it when "restart_edit" was set and nothing was inserted,
     * otherwise CTRL-O w and then <Left> will clear "last_insert".
     */
    inserted = get_inserted();
    পূর্ণসংখ্যা added = inserted.string == ফাঁকা ? 0 : (পূর্ণসংখ্যা)inserted.দৈর্ঘ্য - new_ঢোকাও_skip;
    যদি (did_restart_edit == 0 || added > 0)
    {
	vim_free(last_ঢোকাও.string);
	last_ঢোকাও = inserted;			    // structure copy
	last_ঢোকাও_skip = added < 0 ? 0 : new_ঢোকাও_skip;
    }
    অন্যথা
	vim_free(inserted.string);

    যদি (!arrow_used && end_ঢোকাও_pos != ফাঁকা)
    {
	// Auto-format now.  It may seem strange to do this when stopping an
	// insertion (or moving the cursor), but it's required when appending
	// a line and having it end in a space.  But only do it when something
	// was actually inserted, otherwise undo won't work.
	যদি (!ins_need_undo && has_format_option(FO_AUTO))
	{
	    pos_T   tpos = curwin->w_cursor;

	    // When the cursor is at the end of the line after a space the
	    // formatting will move it to the following word.  Avoid that by
	    // moving the cursor onto the space.
	    cc = 'x';
	    যদি (curwin->w_cursor.col > 0 && gchar_cursor() == NUL)
	    {
		dec_cursor();
		cc = gchar_cursor();
		যদি (!VIM_ISWHITE(cc))
		    curwin->w_cursor = tpos;
	    }

	    auto_format(TRUE, FALSE);

	    যদি (VIM_ISWHITE(cc))
	    {
		যদি (gchar_cursor() != NUL)
		    inc_cursor();
		// If the cursor is still at the same character, also keep
		// the "coladd".
		যদি (gchar_cursor() == NUL
			&& curwin->w_cursor.lnum == tpos.lnum
			&& curwin->w_cursor.col == tpos.col)
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
	যদি (!nomove && did_ai && (esc || (vim_strchr(p_cpo, CPO_INDENT) == ফাঁকা
			&& curwin->w_cursor.lnum != end_ঢোকাও_pos->lnum))
		&& end_ঢোকাও_pos->lnum <= curbuf->b_ml.ml_সারি_গুনতি)
	{
	    pos_T	tpos = curwin->w_cursor;
	    colnr_T	পূর্ব_col = end_ঢোকাও_pos->col;
	    colnr_T	strip_col;

	    curwin->w_cursor = *end_ঢোকাও_pos;
	    check_cursor_col();  // make sure it is not past the line

	    // Where the loop would actually start (back up if on NUL).
	    strip_col = curwin->w_cursor.col;
	    যদি (gchar_cursor() == NUL && strip_col > 0)
		--strip_col;

	    // Don't strip if non-whitespace follows: setline() from a <Cmd>
	    // mapping or CursorHoldI autocmd may have inserted content.
	    যদি (*skipwhite(ml_get_curline() + strip_col) == NUL)
	    {
		curwin->w_cursor.col = strip_col;
		জন্য (;;)
		{
		    যদি (gchar_cursor() == NUL && curwin->w_cursor.col > 0)
			--curwin->w_cursor.col;
		    cc = gchar_cursor();
		    যদি (!VIM_ISWHITE(cc))
			বিরতি;
		    যদি (del_char(TRUE) == FAIL)
			বিরতি;  // should not happen
		}
		যদি (curwin->w_cursor.lnum != tpos.lnum)
		    curwin->w_cursor = tpos;
		অন্যথা যদি (curwin->w_cursor.col < পূর্ব_col)
		{
		    // reset tpos, could have been invalidated in the loop above
		    tpos = curwin->w_cursor;
		    tpos.col++;
		    যদি (cc != NUL && gchar_pos(&tpos) == NUL)
			++curwin->w_cursor.col;	// put cursor back on the NUL
		}

		// <C-S-Right> may have started Visual mode, adjust the position for
		// deleted characters.
		যদি (VIsual_active)
		    check_visual_pos();
	    }
	    অন্যথা
		// Non-whitespace follows, keep original cursor.
		curwin->w_cursor = tpos;
	}
    }
    did_ai = FALSE;
    did_si = FALSE;
    can_si = FALSE;
    can_si_back = FALSE;

    // Set '[ and '] to the inserted text.  When end_insert_pos is NULL we are
    // now in a different buffer.
    যদি (end_ঢোকাও_pos != ফাঁকা)
    {
	curbuf->b_op_start = Insstart;
	curbuf->b_op_start_orig = Insstart_orig;
	curbuf->b_op_end = *end_ঢোকাও_pos;
    }
}

/*
 * Set the last inserted text to a single character.
 * Used for the replace command.
 */
    শূন্য
set_last_ঢোকাও(পূর্ণসংখ্যা c)
{
    char_u	*s;

    vim_free(last_ঢোকাও.string);
    last_ঢোকাও.string = alloc(MB_MAXBYTES * 3 + 5);
    যদি (last_ঢোকাও.string == ফাঁকা)
    {
	last_ঢোকাও.দৈর্ঘ্য = 0;
	ফেরত;
    }

    s = last_ঢোকাও.string;
    // Use the CTRL-V only when entering a special char
    যদি (c < ' ' || c == DEL)
	*s++ = Ctrl_V;
    s = add_char2buf(c, s);
    *s++ = ESC;
    *s = NUL;
    last_ঢোকাও.দৈর্ঘ্য = (size_t)(s - last_ঢোকাও.string);
    last_ঢোকাও_skip = 0;
}

#যদি defined(EXITFREE)
    শূন্য
free_last_ঢোকাও(শূন্য)
{
    VIM_CLEAR_STRING(last_ঢোকাও);
}
#endif

/*
 * Add character "c" to buffer "s".  Escape the special meaning of K_SPECIAL
 * and CSI.  Handle multi-byte characters.
 * Returns a pointer to after the added bytes.
 */
    char_u *
add_char2buf(পূর্ণসংখ্যা c, char_u *s)
{
    char_u	অস্থায়ী[MB_MAXBYTES + 1];
    পূর্ণসংখ্যা		i;
    পূর্ণসংখ্যা		len;

    len = (*mb_char2bytes)(c, অস্থায়ী);
    জন্য (i = 0; i < len; ++i)
    {
	c = অস্থায়ী[i];
	// Need to escape K_SPECIAL and CSI like in the typeahead buffer.
	যদি (c == K_SPECIAL)
	{
	    *s++ = K_SPECIAL;
	    *s++ = KS_SPECIAL;
	    *s++ = KE_FILLER;
	}
#ifdef FEAT_GUI
	অন্যথা যদি (c == CSI)
	{
	    *s++ = CSI;
	    *s++ = KS_EXTRA;
	    *s++ = (পূর্ণসংখ্যা)KE_CSI;
	}
#endif
	অন্যথা
	    *s++ = c;
    }
    ফেরত s;
}

/*
 * move cursor to start of line
 * if flags & BL_WHITE	move to first non-white
 * if flags & BL_SOL	move to first non-white if startofline is set,
 *			    otherwise keep "curswant" column
 * if flags & BL_FIX	don't leave the cursor on a NUL.
 */
    শূন্য
beginline(পূর্ণসংখ্যা flags)
{
    যদি ((flags & BL_SOL) && !p_sol)
	coladvance(curwin->w_curswant);
    অন্যথা
    {
	curwin->w_cursor.col = 0;
	curwin->w_cursor.coladd = 0;

	যদি (flags & (BL_WHITE | BL_SOL))
	{
	    char_u  *ptr;

	    জন্য (ptr = ml_get_curline(); VIM_ISWHITE(*ptr)
			       && !((flags & BL_FIX) && ptr[1] == NUL); ++ptr)
		++curwin->w_cursor.col;
	}
	curwin->w_set_curswant = true;
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

    পূর্ণসংখ্যা
oneright(শূন্য)
{
    char_u	*ptr;
    পূর্ণসংখ্যা		l;

    যদি (virtual_active())
    {
	pos_T	prevpos = curwin->w_cursor;

	// Adjust for multi-wide char (excluding TAB)
	ptr = ml_get_cursor();
	coladvance(getviscol() + ((*ptr != TAB
					  && vim_isprintc((*mb_ptr2char)(ptr)))
		    ? ptr2cells(ptr) : 1));
	curwin->w_set_curswant = true;
	// Return OK if the cursor moved, FAIL otherwise (at window edge).
	ফেরত (prevpos.col != curwin->w_cursor.col
		    || prevpos.coladd != curwin->w_cursor.coladd) ? OK : FAIL;
    }

    ptr = ml_get_cursor();
    যদি (*ptr == NUL)
	ফেরত FAIL;	    // already at the very end

    যদি (has_mbyte)
	l = (*mb_ptr2len)(ptr);
    অন্যথা
	l = 1;

    // move "l" bytes right, but don't end up on the NUL, unless 'virtualedit'
    // contains "onemore".
    যদি (ptr[l] == NUL && (get_ve_flags() & VE_ONEMORE) == 0)
	ফেরত FAIL;
    curwin->w_cursor.col += l;

    curwin->w_set_curswant = true;
    adjust_skipcol();
    ফেরত OK;
}

    পূর্ণসংখ্যা
oneleft(শূন্য)
{
    যদি (virtual_active())
    {
#ifdef FEAT_LINEBREAK
	পূর্ণসংখ্যা width;
#endif
	পূর্ণসংখ্যা v = getviscol();

	যদি (v == 0)
	    ফেরত FAIL;

#ifdef FEAT_LINEBREAK
	// We might get stuck on 'showbreak', skip over it.
	width = 1;
	জন্য (;;)
	{
	    coladvance(v - width);
	    // getviscol() is slow, skip it when 'showbreak' is empty,
	    // 'breakindent' is not set and there are no multi-byte
	    // characters
	    যদি ((*get_showbreak_মান(curwin) == NUL && !curwin->w_p_bri
					     && !has_mbyte) || getviscol() < v)
		বিরতি;
	    ++width;
	}
#অন্যথা
	coladvance(v - 1);
#endif

	যদি (curwin->w_cursor.coladd == 1)
	{
	    char_u *ptr;

	    // Adjust for multi-wide char (not a TAB)
	    ptr = ml_get_cursor();
	    যদি (*ptr != TAB && vim_isprintc((*mb_ptr2char)(ptr))
							 && ptr2cells(ptr) > 1)
		curwin->w_cursor.coladd = 0;
	}

	curwin->w_set_curswant = true;
	adjust_skipcol();
	ফেরত OK;
    }

    যদি (curwin->w_cursor.col == 0)
	ফেরত FAIL;

    curwin->w_set_curswant = true;
    --curwin->w_cursor.col;

    // if the character on the left of the current cursor is a multi-byte
    // character, move to its first byte
    যদি (has_mbyte)
	mb_adjust_cursor();
    adjust_skipcol();
    ফেরত OK;
}

/*
 * Move the cursor up "n" lines in window "wp".
 * Takes care of closed folds.
 */
    শূন্য
cursor_up_inner(win_T *wp, long n)
{
    linenr_T	lnum = wp->w_cursor.lnum;

    যদি (n >= lnum)
	lnum = 1;
    অন্যথা
#ifdef FEAT_FOLDING
	যদি (hasAnyFolding(wp))
    {
	/*
	 * Count each sequence of folded lines as one logical line.
	 */
	// go to the start of the current fold
	(শূন্য)hasFoldingWin(wp, lnum, &lnum, ফাঁকা, TRUE, ফাঁকা);

	যখন (n--)
	{
	    // move up one line
	    --lnum;
	    যদি (lnum <= 1)
		বিরতি;
	    // If we entered a fold, move to the beginning, unless in
	    // Insert mode or when 'foldopen' contains "all": it will open
	    // in a moment.
	    যদি (n > 0 || !((State & MODE_ঢোকাও) || (fdo_flags & FDO_ALL)))
		(শূন্য)hasFoldingWin(wp, lnum, &lnum, ফাঁকা, TRUE, ফাঁকা);
	}
	যদি (lnum < 1)
	    lnum = 1;
    }
    অন্যথা
#endif
	lnum -= n;

    wp->w_cursor.lnum = lnum;
}

    পূর্ণসংখ্যা
cursor_up(
    long	n,
    পূর্ণসংখ্যা		upd_topline)	    // When TRUE: update topline
{
    // This fails if the cursor is already in the first line or the count is
    // larger than the line number and '-' is in 'cpoptions'
    linenr_T lnum = curwin->w_cursor.lnum;
    যদি (n > 0 && (lnum <= 1
		       || (n >= lnum && vim_strchr(p_cpo, CPO_MINUS) != ফাঁকা)))
	ফেরত FAIL;
    cursor_up_inner(curwin, n);

    // try to advance to the column we want to be at
    coladvance(curwin->w_curswant);

    যদি (upd_topline)
	update_topline();	// make sure curwin->w_topline is valid

    ফেরত OK;
}

/*
 * Move the cursor down "n" lines in window "wp".
 * Takes care of closed folds.
 */
    শূন্য
cursor_down_inner(win_T *wp, long n)
{
    linenr_T	lnum = wp->w_cursor.lnum;
    linenr_T	সারি_গুনতি = wp->w_বাফার->b_ml.ml_সারি_গুনতি;

    যদি (lnum + n >= সারি_গুনতি)
	lnum = সারি_গুনতি;
    অন্যথা
#ifdef FEAT_FOLDING
	যদি (hasAnyFolding(wp))
    {
	linenr_T	last;

	// count each sequence of folded lines as one logical line
	যখন (n--)
	{
	    যদি (hasFoldingWin(wp, lnum, ফাঁকা, &last, TRUE, ফাঁকা))
		lnum = last + 1;
	    অন্যথা
		++lnum;
	    যদি (lnum >= সারি_গুনতি)
		বিরতি;
	}
	যদি (lnum > সারি_গুনতি)
	    lnum = সারি_গুনতি;
    }
    অন্যথা
#endif
	lnum += n;

    wp->w_cursor.lnum = lnum;
}

/*
 * Cursor down a number of logical lines.
 */
    পূর্ণসংখ্যা
cursor_down(
    long	n,
    পূর্ণসংখ্যা		upd_topline)	    // When TRUE: update topline
{
    linenr_T	lnum = curwin->w_cursor.lnum;
    linenr_T	সারি_গুনতি = curwin->w_বাফার->b_ml.ml_সারি_গুনতি;
    // This fails if the cursor is already in the last (folded) line, or would
    // move beyond the last line and '-' is in 'cpoptions'.
#ifdef FEAT_FOLDING
    hasFoldingWin(curwin, lnum, ফাঁকা, &lnum, TRUE, ফাঁকা);
#endif
    যদি (n > 0
	    && (lnum >= সারি_গুনতি
		|| (lnum + n > সারি_গুনতি
				     && vim_strchr(p_cpo, CPO_MINUS) != ফাঁকা)))
	ফেরত FAIL;
    cursor_down_inner(curwin, n);

    // try to advance to the column we want to be at
    coladvance(curwin->w_curswant);

    যদি (upd_topline)
	update_topline();	// make sure curwin->w_topline is valid

    ফেরত OK;
}

/*
 * Stuff the last inserted text in the read buffer.
 * Last_insert actually is a copy of the redo buffer, so we
 * first have to remove the command.
 */
    পূর্ণসংখ্যা
stuff_inserted(
    পূর্ণসংখ্যা	    c,		// Command character to be inserted
    long    গুনতি,	// Repeat this many times
    পূর্ণসংখ্যা	    no_esc)	// Don't add an ESC at the end
{
    string_T	ঢোকাও;				// text to be inserted
    char_u	last = ' ';

    ঢোকাও = get_last_ঢোকাও();
    যদি (ঢোকাও.string == ফাঁকা)
    {
	emsg(_(e_no_inserted_text_yet));
	ফেরত FAIL;
    }

    // may want to stuff the command character, to start Insert mode
    যদি (c != NUL)
	stuffcharReadbuff(c);

    যদি (ঢোকাও.দৈর্ঘ্য > 0)
    {
	char_u	*p;

	// look for the last ESC in 'insert'
	জন্য (p = ঢোকাও.string + ঢোকাও.দৈর্ঘ্য - 1; p >= ঢোকাও.string; --p)
	{
	    যদি (*p == ESC)
	    {
		ঢোকাও.দৈর্ঘ্য = (size_t)(p - ঢোকাও.string);
		বিরতি;
	    }
	}
    }

    যদি (ঢোকাও.দৈর্ঘ্য > 0)
    {
	char_u	*p = ঢোকাও.string + ঢোকাও.দৈর্ঘ্য - 1;

	// when the last char is either "0" or "^" it will be quoted if no ESC
	// comes after it OR if it will insert more than once and "ptr"
	// starts with ^D.	-- Acevedo
	যদি ((*p == '0' || *p == '^')
		&& (no_esc || (*ঢোকাও.string == Ctrl_D && গুনতি > 1)))
	{
	    last = *p;
	    --ঢোকাও.দৈর্ঘ্য;
	}
    }

    করো
    {
	stuffReadbuffLen(ঢোকাও.string, (long)ঢোকাও.দৈর্ঘ্য);
	// a trailing "0" is inserted as "<C-V>048", "^" as "<C-V>^"
	নির্বাচন (last)
	{
	ক্ষেত্র '0':
#define TEXT_TO_ঢোকাও "\026\060\064\070"
	    stuffReadbuffLen((char_u *)TEXT_TO_ঢোকাও, STRLEN_LITERAL(TEXT_TO_ঢোকাও));
#undef TEXT_TO_ঢোকাও
	    বিরতি;

	ক্ষেত্র '^':
#define TEXT_TO_ঢোকাও "\026^"
	    stuffReadbuffLen((char_u *)TEXT_TO_ঢোকাও, STRLEN_LITERAL(TEXT_TO_ঢোকাও));
#undef TEXT_TO_ঢোকাও
	    বিরতি;

	পূর্বনির্ধারিত:
	    বিরতি;
	}
    }
    যখন (--গুনতি > 0);

    // may want to stuff a trailing ESC, to get out of Insert mode
    যদি (!no_esc)
	stuffcharReadbuff(ESC);

    ফেরত OK;
}

    string_T
get_last_ঢোকাও(শূন্য)
{
    string_T ঢোকাও = {ফাঁকা, 0};

    যদি (last_ঢোকাও.string != ফাঁকা)
    {
	ঢোকাও.string = last_ঢোকাও.string + last_ঢোকাও_skip;
	ঢোকাও.দৈর্ঘ্য = (size_t)(last_ঢোকাও.দৈর্ঘ্য - last_ঢোকাও_skip);
    }

    ফেরত ঢোকাও;
}

/*
 * Get last inserted string, and remove trailing <Esc>.
 * Returns pointer to allocated memory (must be freed) or NULL.
 */
    char_u *
get_last_ঢোকাও_save(শূন্য)
{
    string_T	ঢোকাও = get_last_ঢোকাও();
    char_u	*s;

    যদি (ঢোকাও.string == ফাঁকা)
	ফেরত ফাঁকা;
    s = vim_strnsave(ঢোকাও.string, ঢোকাও.দৈর্ঘ্য);
    যদি (s == ফাঁকা)
	ফেরত ফাঁকা;

    যদি (ঢোকাও.দৈর্ঘ্য > 0 && s[ঢোকাও.দৈর্ঘ্য - 1] == ESC)	// remove trailing ESC
	s[--ঢোকাও.দৈর্ঘ্য] = NUL;
    ফেরত s;
}

/*
 * Check the word in front of the cursor for an abbreviation.
 * Called when the non-id character "c" has been entered.
 * When an abbreviation is recognized it is removed from the text and
 * the replacement string is inserted in typebuf.tb_buf[], followed by "c".
 */
    স্থির পূর্ণসংখ্যা
echeck_abbr(পূর্ণসংখ্যা c)
{
    // Don't check for abbreviation in paste mode, when disabled and just
    // after moving around with cursor keys.
    যদি (p_paste || no_abbr || arrow_used)
	ফেরত FALSE;

    ফেরত check_abbr(c, ml_get_curline(), curwin->w_cursor.col,
		curwin->w_cursor.lnum == Insstart.lnum ? Insstart.col : 0);
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

স্থির char_u	*replace_স্তূপ = ফাঁকা;
স্থির long	replace_স্তূপ_nr = 0;	    // next entry in replace stack
স্থির long	replace_স্তূপ_len = 0;	    // max. number of entries

    শূন্য
replace_ঠেলো(
    পূর্ণসংখ্যা	    c)	    // character that is replaced (NUL is none)
{
    char_u  *p;

    যদি (replace_স্তূপ_nr < replace_offset)	// nothing to do
	ফেরত;
    যদি (replace_স্তূপ_len <= replace_স্তূপ_nr)
    {
	replace_স্তূপ_len += 50;
	p = ALLOC_MULT(char_u, replace_স্তূপ_len);
	যদি (p == ফাঁকা)	    // out of memory
	{
	    replace_স্তূপ_len -= 50;
	    ফেরত;
	}
	যদি (replace_স্তূপ != ফাঁকা)
	{
	    mch_memmove(p, replace_স্তূপ,
				 (size_t)(replace_স্তূপ_nr * আকার(char_u)));
	    vim_free(replace_স্তূপ);
	}
	replace_স্তূপ = p;
    }
    p = replace_স্তূপ + replace_স্তূপ_nr - replace_offset;
    যদি (replace_offset)
	mch_memmove(p + 1, p, (size_t)(replace_offset * আকার(char_u)));
    *p = c;
    ++replace_স্তূপ_nr;
}

/*
 * Push a character onto the replace stack.  Handles a multi-byte character in
 * reverse byte order, so that the first byte is popped off first.
 * Return the number of bytes done (includes composing characters).
 */
    পূর্ণসংখ্যা
replace_ঠেলো_mb(char_u *p)
{
    পূর্ণসংখ্যা l = (*mb_ptr2len)(p);
    পূর্ণসংখ্যা j;

    জন্য (j = l - 1; j >= 0; --j)
	replace_ঠেলো(p[j]);
    ফেরত l;
}

/*
 * Pop one item from the replace stack.
 * return -1 if stack empty
 * return replaced character or NUL otherwise
 */
    স্থির পূর্ণসংখ্যা
replace_বের_করো(শূন্য)
{
    যদি (replace_স্তূপ_nr == 0)
	ফেরত -1;
    ফেরত (পূর্ণসংখ্যা)replace_স্তূপ[--replace_স্তূপ_nr];
}

/*
 * Join the top two items on the replace stack.  This removes to "off"'th NUL
 * encountered.
 */
    শূন্য
replace_join(
    পূর্ণসংখ্যা	    off)	// offset for which NUL to remove
{
    পূর্ণসংখ্যা	    i;

    জন্য (i = replace_স্তূপ_nr; --i >= 0; )
	যদি (replace_স্তূপ[i] == NUL && off-- <= 0)
	{
	    --replace_স্তূপ_nr;
	    mch_memmove(replace_স্তূপ + i, replace_স্তূপ + i + 1,
					      (size_t)(replace_স্তূপ_nr - i));
	    ফেরত;
	}
}

/*
 * Pop bytes from the replace stack until a NUL is found, and insert them
 * before the cursor.  Can only be used in MODE_REPLACE or MODE_VREPLACE state.
 */
    স্থির শূন্য
replace_বের_করো_ins(শূন্য)
{
    পূর্ণসংখ্যা	    cc;
    পূর্ণসংখ্যা	    oldState = State;

    State = MODE_NORMAL;			// don't want MODE_REPLACE here
    যখন ((cc = replace_বের_করো()) > 0)
    {
	mb_replace_বের_করো_ins(cc);
	dec_cursor();
    }
    State = oldState;
}

/*
 * Insert bytes popped from the replace stack. "cc" is the first byte.  If it
 * indicates a multi-byte char, pop the other bytes too.
 */
    স্থির শূন্য
mb_replace_বের_করো_ins(পূর্ণসংখ্যা cc)
{
    পূর্ণসংখ্যা		n;
    char_u	buf[MB_MAXBYTES + 1];
    পূর্ণসংখ্যা		i;
    পূর্ণসংখ্যা		c;

    যদি (has_mbyte && (n = MB_BYTE2LEN(cc)) > 1)
    {
	buf[0] = cc;
	জন্য (i = 1; i < n; ++i)
	    buf[i] = replace_বের_করো();
	ins_bytes_len(buf, n);
    }
    অন্যথা
	ins_char(cc);

    যদি (enc_utf8)
	// Handle composing chars.
	জন্য (;;)
	{
	    c = replace_বের_করো();
	    যদি (c == -1)	    // stack empty
		বিরতি;
	    যদি ((n = MB_BYTE2LEN(c)) == 1)
	    {
		// Not a multi-byte char, put it back.
		replace_ঠেলো(c);
		বিরতি;
	    }

	    buf[0] = c;
	    জন্য (i = 1; i < n; ++i)
		buf[i] = replace_বের_করো();
	    যদি (utf_iscomposing(utf_ptr2char(buf)))
		ins_bytes_len(buf, n);
	    অন্যথা
	    {
		// Not a composing char, put it back.
		জন্য (i = n - 1; i >= 0; --i)
		    replace_ঠেলো(buf[i]);
		বিরতি;
	    }

	}
}

/*
 * make the replace stack empty
 * (called when exiting replace mode)
 */
    স্থির শূন্য
replace_flush(শূন্য)
{
    VIM_CLEAR(replace_স্তূপ);
    replace_স্তূপ_len = 0;
    replace_স্তূপ_nr = 0;
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
    স্থির শূন্য
replace_do_bs(পূর্ণসংখ্যা limit_col)
{
    পূর্ণসংখ্যা		cc;
    পূর্ণসংখ্যা		orig_len = 0;
    পূর্ণসংখ্যা		ins_len;
    পূর্ণসংখ্যা		orig_vcols = 0;
    colnr_T	start_vcol;
    char_u	*p;
    পূর্ণসংখ্যা		i;
    পূর্ণসংখ্যা		vcol;

    cc = replace_বের_করো();
    যদি (cc > 0)
    {
#ifdef FEAT_PROP_POPUP
	size_t	len_before = 0;  // init to shut up GCC

	যদি (curbuf->b_has_textprop)
	{
	    // Do not adjust text properties for individual delete and insert
	    // operations, do it afterwards on the resulting text.
	    len_before = ml_get_curline_len();
	    ++text_prop_frozen;
	}
#endif
	যদি (State & VREPLACE_FLAG)
	{
	    // Get the number of screen cells used by the character we are
	    // going to delete.
	    getvcol(curwin, &curwin->w_cursor, ফাঁকা, &start_vcol, ফাঁকা, 0);
	    orig_vcols = chartabsize(ml_get_cursor(), start_vcol);
	}
	যদি (has_mbyte)
	{
	    (শূন্য)del_char_after_col(limit_col);
	    যদি (State & VREPLACE_FLAG)
		orig_len = ml_get_cursor_len();
	    replace_ঠেলো(cc);
	}
	অন্যথা
	{
	    pchar_cursor(cc);
	    যদি (State & VREPLACE_FLAG)
		orig_len = ml_get_cursor_len() - 1;
	}
	replace_বের_করো_ins();

	যদি (State & VREPLACE_FLAG)
	{
	    // Get the number of screen cells used by the inserted characters
	    p = ml_get_cursor();
	    ins_len = ml_get_cursor_len() - orig_len;
	    vcol = start_vcol;
	    জন্য (i = 0; i < ins_len; ++i)
	    {
		vcol += chartabsize(p + i, vcol);
		i += (*mb_ptr2len)(p + i) - 1;
	    }
	    vcol -= start_vcol;

	    // Delete spaces that were inserted after the cursor to keep the
	    // text aligned.
	    curwin->w_cursor.col += ins_len;
	    যখন (vcol > orig_vcols && gchar_cursor() == ' ')
	    {
		del_char(FALSE);
		++orig_vcols;
	    }
	    curwin->w_cursor.col -= ins_len;
	}

	// mark the buffer as changed and prepare for displaying
	changed_bytes(curwin->w_cursor.lnum, curwin->w_cursor.col);

#ifdef FEAT_PROP_POPUP
	যদি (curbuf->b_has_textprop)
	{
	    size_t len_now = ml_get_curline_len();

	    --text_prop_frozen;
	    adjust_prop_columns(curwin->w_cursor.lnum, curwin->w_cursor.col,
					   (পূর্ণসংখ্যা)(len_now - len_before), 0);
	}
#endif
    }
    অন্যথা যদি (cc == 0)
	(শূন্য)del_char_after_col(limit_col);
}

#যদি defined(FEAT_RIGHTLEFT)
/*
 * Map Hebrew keyboard when in hkmap mode.
 */
    পূর্ণসংখ্যা
hkmap(পূর্ণসংখ্যা c)
{
    যদি (p_hkmapp)   // phonetic mapping, by Ilya Dogolazky
    {
	গণনা {hALEF=0, BET, GIMEL, DALET, HEI, VAV, ZAIN, HET, TET, IUD,
	    KAFsofit, hKAF, LAMED, MEMsofit, MEM, NUNsofit, NUN, SAMEH, AIN,
	    PEIsofit, PEI, ZADIsofit, ZADI, KOF, RESH, hSHIN, TAV};
	স্থির char_u map[26] =
	    {(char_u)hALEF/*a*/, (char_u)BET  /*b*/, (char_u)hKAF    /*c*/,
	     (char_u)DALET/*d*/, (char_u)-1   /*e*/, (char_u)PEIsofit/*f*/,
	     (char_u)GIMEL/*g*/, (char_u)HEI  /*h*/, (char_u)IUD     /*i*/,
	     (char_u)HET  /*j*/, (char_u)KOF  /*k*/, (char_u)LAMED   /*l*/,
	     (char_u)MEM  /*m*/, (char_u)NUN  /*n*/, (char_u)SAMEH   /*o*/,
	     (char_u)PEI  /*p*/, (char_u)-1   /*q*/, (char_u)RESH    /*r*/,
	     (char_u)ZAIN /*s*/, (char_u)TAV  /*t*/, (char_u)TET     /*u*/,
	     (char_u)VAV  /*v*/, (char_u)hSHIN/*w*/, (char_u)-1      /*x*/,
	     (char_u)AIN  /*y*/, (char_u)ZADI /*z*/};

	যদি (c == 'N' || c == 'M' || c == 'P' || c == 'C' || c == 'Z')
	    ফেরত (পূর্ণসংখ্যা)(map[CharOrd(c)] - 1 + p_aleph);
							    // '-1'='sofit'
	অন্যথা যদি (c == 'x')
	    ফেরত 'X';
	অন্যথা যদি (c == 'q')
	    ফেরত '\''; // {geresh}={'}
	অন্যথা যদি (c == 246)
	    ফেরত ' ';  // \"o --> ' ' for a german keyboard
	অন্যথা যদি (c == 228)
	    ফেরত ' ';  // \"a --> ' '      -- / --
	অন্যথা যদি (c == 252)
	    ফেরত ' ';  // \"u --> ' '      -- / --
	// NOTE: islower() does not do the right thing for us on Linux so we
	// do this the same was as 5.7 and previous, so it works correctly on
	// all systems.  Specifically, the e.g. Delete and Arrow keys are
	// munged and won't work if e.g. searching for Hebrew text.
	অন্যথা যদি (c >= 'a' && c <= 'z')
	    ফেরত (পূর্ণসংখ্যা)(map[Char_Ord_নিচের(c)] + p_aleph);
	অন্যথা
	    ফেরত c;
    }
    অন্যথা
    {
	নির্বাচন (c)
	{
	    ক্ষেত্র '`':	ফেরত ';';
	    ক্ষেত্র '/':	ফেরত '.';
	    ক্ষেত্র '\'':	ফেরত ',';
	    ক্ষেত্র 'q':	ফেরত '/';
	    ক্ষেত্র 'w':	ফেরত '\'';

			// Hebrew letters - set offset from 'a'
	    ক্ষেত্র ',':	c = '{'; বিরতি;
	    ক্ষেত্র '.':	c = 'v'; বিরতি;
	    ক্ষেত্র ';':	c = 't'; বিরতি;
	    পূর্বনির্ধারিত: {
			 স্থির অক্ষর str[] = "zqbcxlsjphmkwonu ydafe rig";

			 যদি (c < 'a' || c > 'z')
			     ফেরত c;
			 c = str[Char_Ord_নিচের(c)];
			 বিরতি;
		     }
	}

	ফেরত (পূর্ণসংখ্যা)(Char_Ord_নিচের(c) + p_aleph);
    }
}
#endif

    স্থির শূন্য
ins_reg(শূন্য)
{
    পূর্ণসংখ্যা		need_redraw = FALSE;
    পূর্ণসংখ্যা		regname;
    পূর্ণসংখ্যা		literally = 0;
    পূর্ণসংখ্যা		vis_active = VIsual_active;

    /*
     * If we are going to wait for a character, show a '"'.
     */
    pc_status = PC_STATUS_UNSET;
    যদি (redrawing() && !char_avail())
    {
	// may need to redraw when no more chars available now
	ins_redraw(FALSE);

	edit_putchar('"', TRUE);
	add_to_showcmd_c(Ctrl_R);
    }

#ifdef USE_ON_FLY_SCROLL
    dont_scroll = TRUE;		// disallow scrolling here
#endif

    /*
     * Don't map the register name. This also prevents the mode message to be
     * deleted when ESC is hit.
     */
    ++no_mapping;
    ++allow_চাবিগুলি;
    regname = plain_vgetc();
    LANGMAP_ADJUST(regname, TRUE);
    যদি (regname == Ctrl_R || regname == Ctrl_O || regname == Ctrl_P)
    {
	// Get a third key for literal register insertion
	literally = regname;
	add_to_showcmd_c(literally);
	regname = plain_vgetc();
	LANGMAP_ADJUST(regname, TRUE);
    }
    --no_mapping;
    --allow_চাবিগুলি;

#ifdef FEAT_EVAL
    // Don't call u_sync() while typing the expression or giving an error
    // message for it. Only call it explicitly.
    ++no_u_sync;
    যদি (regname == '=')
    {
	pos_T	curpos = curwin->w_cursor;
# ifdef HAVE_প্রবেশ_METHOD
	পূর্ণসংখ্যা	im_on = im_get_status();
# endif
	// Sync undo when evaluating the expression calls setline() or
	// append(), so that it can be undone separately.
	u_sync_once = 2;

	regname = get_রাশিমালা_register();

	// Cursor may be moved back a column.
	curwin->w_cursor = curpos;
	check_cursor();
# ifdef HAVE_প্রবেশ_METHOD
	// Restore the Input Method.
	যদি (im_on)
	    im_set_active(TRUE);
# endif
    }
    যদি (regname == NUL || !valid_yank_reg(regname, FALSE))
    {
	vim_beep(BO_REG);
	need_redraw = TRUE;	// remove the '"'
    }
    অন্যথা
    {
#endif
	যদি (literally == Ctrl_O || literally == Ctrl_P)
	{
	    // Append the command to the redo buffer.
	    AppendCharToRedobuff(Ctrl_R);
	    AppendCharToRedobuff(literally);
	    AppendCharToRedobuff(regname);

	    do_put(regname, ফাঁকা, BACKWARD, 1L,
		 (literally == Ctrl_P ? PUT_FIXINDENT : 0) | PUT_CURSEND);
	}
	অন্যথা যদি (ঢোকাও_reg(regname, literally) == FAIL)
	{
	    vim_beep(BO_REG);
	    need_redraw = TRUE;	// remove the '"'
	}
	অন্যথা যদি (stop_ঢোকাও_mode)
	    // When the '=' register was used and a function was invoked that
	    // did ":stopinsert" then stuff_empty() returns FALSE but we won't
	    // insert anything, need to remove the '"'
	    need_redraw = TRUE;

#ifdef FEAT_EVAL
    }
    --no_u_sync;
    যদি (u_sync_once == 1)
	ins_need_undo = TRUE;
    u_sync_once = 0;
#endif
    clear_showcmd();

    // If the inserted register is empty, we need to remove the '"'
    যদি (need_redraw || stuff_empty())
	edit_unputchar();

    // Disallow starting Visual mode here, would get a weird mode.
    যদি (!vis_active && VIsual_active)
	end_visual_mode();
}

/*
 * CTRL-G commands in Insert mode.
 */
    স্থির শূন্য
ins_ctrl_g(শূন্য)
{
    পূর্ণসংখ্যা		c;

    // Right after CTRL-X the cursor will be after the ruler.
    setcursor();

    /*
     * Don't map the second key. This also prevents the mode message to be
     * deleted when ESC is hit.
     */
    ++no_mapping;
    ++allow_চাবিগুলি;
    c = plain_vgetc();
    --no_mapping;
    --allow_চাবিগুলি;
    নির্বাচন (c)
    {
	// CTRL-G k and CTRL-G <Up>: cursor up to Insstart.col
	ক্ষেত্র K_UP:
	ক্ষেত্র Ctrl_K:
	ক্ষেত্র 'k': ins_up(TRUE);
		  বিরতি;

	// CTRL-G j and CTRL-G <Down>: cursor down to Insstart.col
	ক্ষেত্র K_DOWN:
	ক্ষেত্র Ctrl_J:
	ক্ষেত্র 'j': ins_down(TRUE);
		  বিরতি;

	// CTRL-G u: start new undoable edit
	ক্ষেত্র 'u': u_sync(TRUE);
		  ins_need_undo = TRUE;

		  // Need to reset Insstart, esp. because a BS that joins
		  // a line to the previous one must save for undo.
		  update_Insstart_orig = FALSE;
		  Insstart = curwin->w_cursor;
		  বিরতি;

	// CTRL-G U: do not break undo with the next char
	ক্ষেত্র 'U':
		  // Allow one left/right cursor movement with the next char,
		  // without breaking undo.
		  dont_sync_undo = MAYBE;
		  বিরতি;

	ক্ষেত্র ESC:
		  // Esc after CTRL-G cancels it.
		  বিরতি;

	// Unknown CTRL-G command, reserved for future expansion.
	পূর্বনির্ধারিত:  vim_beep(BO_CTRLG);
    }
}

/*
 * CTRL-^ in Insert mode.
 */
    স্থির শূন্য
ins_ctrl_hat(শূন্য)
{
    যদি (map_to_exists_mode((char_u *)"", MODE_LANGMAP, FALSE))
    {
	// ":lmap" mappings exists, Toggle use of ":lmap" mappings.
	যদি (State & MODE_LANGMAP)
	{
	    curbuf->b_p_iminsert = B_IMODE_NONE;
	    State &= ~MODE_LANGMAP;
	}
	অন্যথা
	{
	    curbuf->b_p_iminsert = B_IMODE_LMAP;
	    State |= MODE_LANGMAP;
#ifdef HAVE_প্রবেশ_METHOD
	    im_set_active(FALSE);
#endif
	}
    }
#ifdef HAVE_প্রবেশ_METHOD
    অন্যথা
    {
	// There are no ":lmap" mappings, toggle IM
	যদি (im_get_status())
	{
	    curbuf->b_p_iminsert = B_IMODE_NONE;
	    im_set_active(FALSE);
	}
	অন্যথা
	{
	    curbuf->b_p_iminsert = B_IMODE_IM;
	    State &= ~MODE_LANGMAP;
	    im_set_active(TRUE);
	}
    }
#endif
    set_iminsert_global();
    showmode();
#ifdef FEAT_GUI
    // may show different cursor shape or color
    যদি (gui.in_use)
	gui_update_cursor(TRUE, FALSE);
#endif
#যদি defined(FEAT_KEYMAP)
    // Show/unshow value of 'keymap' in status lines.
    status_redraw_curbuf();
#endif
}

/*
 * Handle ESC in insert mode.
 * Returns TRUE when leaving insert mode, FALSE when going to repeat the
 * insert.
 */
    স্থির পূর্ণসংখ্যা
ins_esc(
    long	*গুনতি,
    পূর্ণসংখ্যা		cmdchar,
    পূর্ণসংখ্যা		nomove)	    // don't move cursor
{
    পূর্ণসংখ্যা		অস্থায়ী;
    স্থির পূর্ণসংখ্যা	disabled_redraw = FALSE;
#ifdef FEAT_CONCEAL
    // Remember if the cursor line was concealed before changing State.
    পূর্ণসংখ্যা		cursor_সারি_was_concealed = curwin->w_p_cole > 0
						&& conceal_cursor_সারি(curwin);
#endif

#ifdef FEAT_SPELL
    check_spell_redraw();
#endif

    অস্থায়ী = curwin->w_cursor.col;
    যদি (disabled_redraw)
    {
	যদি (RedrawingDisabled > 0)
	    --RedrawingDisabled;
	disabled_redraw = FALSE;
    }
    যদি (!arrow_used)
    {
	/*
	 * Don't append the ESC for "r<CR>" and "grx".
	 * When 'insertmode' is set only CTRL-L stops Insert mode.  Needed for
	 * when "count" is non-zero.
	 */
	যদি (cmdchar != 'r' && cmdchar != 'v')
	    AppendToRedobuff(p_im ? (char_u *)"\014" : ESC_STR);

	/*
	 * Repeating insert may take a long time.  Check for
	 * interrupt now and then.
	 */
	যদি (*গুনতি > 0)
	{
	    সারি_breakcheck();
	    যদি (got_int)
		*গুনতি = 0;
	}

	যদি (--*গুনতি > 0)	// repeat what was typed
	{
	    // Vi repeats the insert without replacing characters.
	    যদি (vim_strchr(p_cpo, CPO_REPLCNT) != ফাঁকা)
		State &= ~REPLACE_FLAG;

	    (শূন্য)start_redo_ins();
	    যদি (cmdchar == 'r' || cmdchar == 'v')
		stuffRedoReadbuff(ESC_STR);	// no ESC in redo buffer
	    ++RedrawingDisabled;
	    disabled_redraw = TRUE;
	    ফেরত FALSE;	// repeat the insert
	}
	stop_ঢোকাও(&curwin->w_cursor, TRUE, nomove);
	undisplay_dollar();
    }

    যদি (cmdchar != 'r' && cmdchar != 'v')
	ins_apply_autocmds(EVENT_INSERTLEAVEPRE);

    // When an autoindent was removed, curswant stays after the
    // indent
    যদি (restart_edit == NUL && (colnr_T)অস্থায়ী == curwin->w_cursor.col)
	curwin->w_set_curswant = true;

    // Remember the last Insert position in the '^ mark.
    যদি ((cmdmod.cmod_flags & CMOD_KEEPJUMPS) == 0)
	curbuf->b_last_ঢোকাও = curwin->w_cursor;

    /*
     * The cursor should end up on the last inserted character.
     * Don't do it for CTRL-O, unless past the end of the line.
     */
    যদি (!nomove
	    && (curwin->w_cursor.col != 0
		|| curwin->w_cursor.coladd > 0)
	    && (restart_edit == NUL
		   || (gchar_cursor() == NUL && !VIsual_active))
#ifdef FEAT_RIGHTLEFT
	    && !revins_on
#endif
				      )
    {
	যদি (curwin->w_cursor.coladd > 0 || get_ve_flags() == VE_ALL)
	{
	    oneleft();
	    যদি (restart_edit != NUL)
		++curwin->w_cursor.coladd;
	}
	অন্যথা
	{
	    --curwin->w_cursor.col;
	    curwin->w_valid &= ~(VALID_WCOL|VALID_VIRTCOL);
	    // Correct cursor for multi-byte character.
	    যদি (has_mbyte)
		mb_adjust_cursor();
	}
    }

#ifdef HAVE_প্রবেশ_METHOD
    // Disable IM to allow typing English directly for Normal mode commands.
    // When ":lmap" is enabled don't change 'iminsert' (IM can be enabled as
    // well).
    যদি (!(State & MODE_LANGMAP))
	im_save_status(&curbuf->b_p_iminsert);
    im_set_active(FALSE);
#endif

    State = MODE_NORMAL;
    may_trigger_modechanged();
    // need to position cursor again when on a TAB and when on a char with
    // virtual text.
    যদি (gchar_cursor() == TAB
#ifdef FEAT_PROP_POPUP
	    || curbuf->b_has_textprop
#endif
       )
	curwin->w_valid &= ~(VALID_WROW|VALID_WCOL|VALID_VIRTCOL);

    setmouse();
#ifdef CURSOR_SHAPE
    ui_cursor_shape();		// may show different cursor shape
#endif
    যদি (!p_ek)
    {
	MAY_WANT_TO_LOG_THIS;

	// Re-enable bracketed paste mode.
	out_str_t_BE();

	// Re-enable modifyOtherKeys.
	out_str_t_TI();
    }
#ifdef FEAT_CONCEAL
    // Check if the cursor line needs redrawing after changing State.  If
    // 'concealcursor' is "i" it needs to be redrawn without concealing.
    conceal_check_cursor_সারি(cursor_সারি_was_concealed);
#endif

    // When recording or for CTRL-O, need to display the new mode.
    // Otherwise remove the mode message.
    যদি (reg_recording != 0 || restart_edit != NUL)
	showmode();
    অন্যথা যদি (p_smd && (got_int || !skip_showmode()))
	বার্তা("");

    ফেরত TRUE;	    // exit Insert mode
}

#ifdef FEAT_RIGHTLEFT
/*
 * Toggle language: hkmap and revins_on.
 * Move to end of reverse inserted text.
 */
    স্থির শূন্য
ins_ctrl_(শূন্য)
{
    যদি (revins_on && revins_chars && revins_scol >= 0)
    {
	যখন (gchar_cursor() != NUL && revins_chars--)
	    ++curwin->w_cursor.col;
    }
    p_ri = !p_ri;
    revins_on = (State == MODE_ঢোকাও && p_ri);
    যদি (revins_on)
    {
	revins_scol = curwin->w_cursor.col;
	revins_legal++;
	revins_chars = 0;
	undisplay_dollar();
    }
    অন্যথা
	revins_scol = -1;
    p_hkmap = curwin->w_p_rl ^ p_ri;    // be consistent!
    showmode();
}
#endif

/*
 * If 'keymodel' contains "startsel", may start selection.
 * Returns TRUE when a CTRL-O and other keys stuffed.
 */
    স্থির পূর্ণসংখ্যা
ins_start_select(পূর্ণসংখ্যা c)
{
    যদি (!km_startsel)
	ফেরত FALSE;
    নির্বাচন (c)
    {
	ক্ষেত্র K_KHOME:
	ক্ষেত্র K_KEND:
	ক্ষেত্র K_PAGEUP:
	ক্ষেত্র K_KPAGEUP:
	ক্ষেত্র K_PAGEDOWN:
	ক্ষেত্র K_KPAGEDOWN:
#ifdef MACOS_X
	ক্ষেত্র K_বাম:
	ক্ষেত্র K_ডান:
	ক্ষেত্র K_UP:
	ক্ষেত্র K_DOWN:
	ক্ষেত্র K_END:
	ক্ষেত্র K_HOME:
#endif
	    যদি (!(mod_mask & MOD_MASK_SHIFT))
		বিরতি;
	    // FALLTHROUGH
	ক্ষেত্র K_S_বাম:
	ক্ষেত্র K_S_ডান:
	ক্ষেত্র K_S_UP:
	ক্ষেত্র K_S_DOWN:
	ক্ষেত্র K_S_END:
	ক্ষেত্র K_S_HOME:
	    // Start selection right away, the cursor can move with CTRL-O when
	    // beyond the end of the line.
	    start_selection();

	    // Execute the key in (insert) Select mode.
	    stuffcharReadbuff(Ctrl_O);
	    যদি (mod_mask)
	    {
		char_u	    buf[4];

		buf[0] = K_SPECIAL;
		buf[1] = KS_MODIFIER;
		buf[2] = mod_mask;
		buf[3] = NUL;
		stuffReadbuffLen(buf, 3L);
	    }
	    stuffcharReadbuff(c);
	    ফেরত TRUE;
    }
    ফেরত FALSE;
}

/*
 * <Insert> key in Insert mode: toggle insert/replace mode.
 */
    স্থির শূন্য
ins_ঢোকাও(পূর্ণসংখ্যা replaceState)
{
#ifdef FEAT_EVAL
    set_vim_var_string(VV_INSERTMODE,
		   (char_u *)((State & REPLACE_FLAG) ? "i"
			    : replaceState == MODE_VREPLACE ? "v" : "r"), 1);
#endif
    ins_apply_autocmds(EVENT_INSERTCHANGE);
    যদি (State & REPLACE_FLAG)
	State = MODE_ঢোকাও | (State & MODE_LANGMAP);
    অন্যথা
	State = replaceState | (State & MODE_LANGMAP);
    may_trigger_modechanged();
    AppendCharToRedobuff(K_INS);
    showmode();
#ifdef CURSOR_SHAPE
    ui_cursor_shape();		// may show different cursor shape
#endif
}

/*
 * Pressed CTRL-O in Insert mode.
 */
    স্থির শূন্য
ins_ctrl_o(শূন্য)
{
    restart_VIsual_select = 0;
    যদি (State & VREPLACE_FLAG)
	restart_edit = 'V';
    অন্যথা যদি (State & REPLACE_FLAG)
	restart_edit = 'R';
    অন্যথা
	restart_edit = 'I';
    যদি (virtual_active())
	ins_at_eol = FALSE;	// cursor always keeps its column
    অন্যথা
	ins_at_eol = (gchar_cursor() == NUL);
}

/*
 * If the cursor is on an indent, ^T/^D insert/delete one
 * shiftwidth.	Otherwise ^T/^D behave like a "<<" or ">>".
 * Always round the indent to 'shiftwidth', this is compatible
 * with vi.  But vi only supports ^T and ^D after an
 * autoindent, we support it everywhere.
 */
    স্থির শূন্য
ins_shift(পূর্ণসংখ্যা c, পূর্ণসংখ্যা lastc)
{
    যদি (stop_arrow() == FAIL)
	ফেরত;
    AppendCharToRedobuff(c);

    /*
     * 0^D and ^^D: remove all indent.
     */
    যদি (c == Ctrl_D && (lastc == '0' || lastc == '^')
						  && curwin->w_cursor.col > 0)
    {
	--curwin->w_cursor.col;
	(শূন্য)del_char(FALSE);		// delete the '^' or '0'
	// In Replace mode, restore the characters that '^' or '0' replaced.
	যদি (State & REPLACE_FLAG)
	    replace_বের_করো_ins();
	যদি (lastc == '^')
	    old_indent = get_indent();	// remember curr. indent
	change_indent(INDENT_SET, 0, TRUE, 0, TRUE);
    }
    অন্যথা
	change_indent(c == Ctrl_D ? INDENT_DEC : INDENT_INC, 0, TRUE, 0, TRUE);

    যদি (did_ai && *skipwhite(ml_get_curline()) != NUL)
	did_ai = FALSE;
    did_si = FALSE;
    can_si = FALSE;
    can_si_back = FALSE;
    can_cindent = FALSE;	// no cindenting after ^D or ^T
}

    স্থির শূন্য
ins_del(শূন্য)
{
    পূর্ণসংখ্যা	    অস্থায়ী;

    যদি (stop_arrow() == FAIL)
	ফেরত;
    যদি (gchar_cursor() == NUL)		// delete newline
    {
	অস্থায়ী = curwin->w_cursor.col;
	যদি (!can_bs(BS_EOL)		// only if "eol" included
		|| do_join(2, FALSE, TRUE, FALSE, FALSE) == FAIL)
	    vim_beep(BO_BS);
	অন্যথা
	{
	    curwin->w_cursor.col = অস্থায়ী;
	    // Adjust orig_line_count in case more lines have been deleted than
	    // have been added. That makes sure, that open_line() later
	    // can access all buffer lines correctly
	    যদি (State & VREPLACE_FLAG &&
		    orig_সারি_গুনতি > curbuf->b_ml.ml_সারি_গুনতি)
		orig_সারি_গুনতি = curbuf->b_ml.ml_সারি_গুনতি;
	}
    }
    অন্যথা যদি (del_char(FALSE) == FAIL)  // delete char under cursor
	vim_beep(BO_BS);
    did_ai = FALSE;
    did_si = FALSE;
    can_si = FALSE;
    can_si_back = FALSE;
    AppendCharToRedobuff(K_DEL);
}

/*
 * Delete one character for ins_bs().
 */
    স্থির শূন্য
ins_bs_one(শূন্য)
{
    dec_cursor();
    যদি (State & REPLACE_FLAG)
    {
	// Don't delete characters before the insert point when in
	// Replace mode
	যদি (curwin->w_cursor.lnum != Insstart.lnum
		|| curwin->w_cursor.col >= Insstart.col)
	    replace_do_bs(-1);
    }
    অন্যথা
	(শূন্য)del_char(FALSE);
}

/*
 * Handle Backspace, delete-word and delete-line in Insert mode.
 * Return TRUE when backspace was actually used.
 */
    স্থির পূর্ণসংখ্যা
ins_bs(
    পূর্ণসংখ্যা		c,
    পূর্ণসংখ্যা		mode,
    পূর্ণসংখ্যা		*inserted_space_p)
{
    linenr_T	lnum;
    পূর্ণসংখ্যা		cc;
    পূর্ণসংখ্যা		অস্থায়ী = 0;	    // init for GCC
    colnr_T	save_col;
    colnr_T	mincol;
    পূর্ণসংখ্যা		did_backspace = FALSE;
    পূর্ণসংখ্যা		in_indent;
    পূর্ণসংখ্যা		oldState;
    পূর্ণসংখ্যা		cpc[MAX_MCO];	    // composing characters
    পূর্ণসংখ্যা		call_fix_indent = FALSE;

    /*
     * can't delete anything in an empty file
     * can't backup past first character in buffer
     * can't backup past starting point unless 'backspace' > 1
     * can backup to a previous line if 'backspace' == 0
     */
    যদি (       BUFEMPTY()
	    || (
#ifdef FEAT_RIGHTLEFT
		!revins_on &&
#endif
		((curwin->w_cursor.lnum == 1 && curwin->w_cursor.col == 0)
		    || (!can_bs(BS_START)
			&& ((arrow_used
#ifdef FEAT_JOB_CHANNEL
				&& !bt_prompt(curbuf)
#endif
			) || (curwin->w_cursor.lnum == Insstart_orig.lnum
				&& curwin->w_cursor.col <= Insstart_orig.col)))
		    || (!can_bs(BS_INDENT) && !arrow_used && ai_col > 0
					 && curwin->w_cursor.col <= ai_col)
		    || (!can_bs(BS_EOL) && curwin->w_cursor.col == 0))))
    {
	vim_beep(BO_BS);
	ফেরত FALSE;
    }

    যদি (stop_arrow() == FAIL)
	ফেরত FALSE;
    in_indent = inindent(0);
    যদি (in_indent)
	can_cindent = FALSE;
    end_comment_pending = NUL;	// After BS, don't auto-end comment
#ifdef FEAT_RIGHTLEFT
    যদি (revins_on)	    // put cursor after last inserted char
	inc_cursor();
#endif

    // Virtualedit:
    //	BACKSPACE_CHAR eats a virtual space
    //	BACKSPACE_WORD eats all coladd
    //	BACKSPACE_LINE eats all coladd and keeps going
    যদি (curwin->w_cursor.coladd > 0)
    {
	যদি (mode == BACKSPACE_CHAR)
	{
	    --curwin->w_cursor.coladd;
	    ফেরত TRUE;
	}
	যদি (mode == BACKSPACE_WORD)
	{
	    curwin->w_cursor.coladd = 0;
	    ফেরত TRUE;
	}
	curwin->w_cursor.coladd = 0;
    }

    /*
     * Delete newline!
     */
    যদি (curwin->w_cursor.col == 0)
    {
	lnum = Insstart.lnum;
	যদি (curwin->w_cursor.lnum == lnum
#ifdef FEAT_RIGHTLEFT
			|| revins_on
#endif
				    )
	{
	    যদি (u_save((linenr_T)(curwin->w_cursor.lnum - 2),
			       (linenr_T)(curwin->w_cursor.lnum + 1)) == FAIL)
		ফেরত FALSE;
	    --Insstart.lnum;
	    Insstart.col = ml_get_len(Insstart.lnum);
	}
	/*
	 * In replace mode:
	 * cc < 0: NL was inserted, delete it
	 * cc >= 0: NL was replaced, put original characters back
	 */
	cc = -1;
	যদি (State & REPLACE_FLAG)
	    cc = replace_বের_করো();	    // returns -1 if NL was inserted
	/*
	 * In replace mode, in the line we started replacing, we only move the
	 * cursor.
	 */
	যদি ((State & REPLACE_FLAG) && curwin->w_cursor.lnum <= lnum)
	{
	    dec_cursor();
	}
	অন্যথা
	{
	    যদি (!(State & VREPLACE_FLAG)
				   || curwin->w_cursor.lnum > orig_সারি_গুনতি)
	    {
		অস্থায়ী = gchar_cursor();	// remember current char
		--curwin->w_cursor.lnum;

		// When "aw" is in 'formatoptions' we must delete the space at
		// the end of the line, otherwise the line will be broken
		// again when auto-formatting.
		যদি (has_format_option(FO_AUTO)
					   && has_format_option(FO_WHITE_PAR))
		{
		    char_u  *ptr = ml_get_buf(curbuf, curwin->w_cursor.lnum,
									FALSE);
		    পূর্ণসংখ্যা	    len = ml_get_curline_len();

		    যদি (len > 0 && ptr[len - 1] == ' ')
		    {
			char_u *newp = alloc(curbuf->b_ml.ml_সারি_len - 1);

			যদি (newp != ফাঁকা)
			{
			    mch_memmove(newp, ptr, len - 1);
			    newp[len - 1] = NUL;
			    যদি (curbuf->b_ml.ml_সারি_len > len + 1)
				mch_memmove(newp + len, ptr + len + 1,
					   curbuf->b_ml.ml_সারি_len - len - 1);

			    যদি (curbuf->b_ml.ml_flags
					      & (ML_সারি_DIRTY | ML_ALLOCATED))
				vim_free(curbuf->b_ml.ml_সারি_ptr);
			    curbuf->b_ml.ml_সারি_ptr = newp;
			    curbuf->b_ml.ml_সারি_len--;
			    curbuf->b_ml.ml_সারি_textlen--;
			    curbuf->b_ml.ml_flags |= ML_সারি_DIRTY;
			}
		    }
		}

		(শূন্য)do_join(2, FALSE, FALSE, FALSE, FALSE);
		যদি (অস্থায়ী == NUL && gchar_cursor() != NUL)
		    inc_cursor();
	    }
	    অন্যথা
		dec_cursor();

	    /*
	     * In MODE_REPLACE mode we have to put back the text that was
	     * replaced by the NL. On the replace stack is first a
	     * NUL-terminated sequence of characters that were deleted and then
	     * the characters that NL replaced.
	     */
	    যদি (State & REPLACE_FLAG)
	    {
		/*
		 * Do the next ins_char() in MODE_NORMAL state, to
		 * prevent ins_char() from replacing characters and
		 * avoiding showmatch().
		 */
		oldState = State;
		State = MODE_NORMAL;
		/*
		 * restore characters (blanks) deleted after cursor
		 */
		যখন (cc > 0)
		{
		    save_col = curwin->w_cursor.col;
		    mb_replace_বের_করো_ins(cc);
		    curwin->w_cursor.col = save_col;
		    cc = replace_বের_করো();
		}
		// restore the characters that NL replaced
		replace_বের_করো_ins();
		State = oldState;
	    }
	}
	did_ai = FALSE;
    }
    অন্যথা
    {
	/*
	 * Delete character(s) before the cursor.
	 */
#ifdef FEAT_RIGHTLEFT
	যদি (revins_on)		// put cursor on last inserted char
	    dec_cursor();
#endif
	mincol = 0;
						// keep indent
	যদি (mode == BACKSPACE_সারি
		&& (curbuf->b_p_ai || cindent_on())
#ifdef FEAT_RIGHTLEFT
		&& !revins_on
#endif
			    )
	{
	    save_col = curwin->w_cursor.col;
	    beginline(BL_WHITE);
	    যদি (curwin->w_cursor.col < save_col)
	    {
		mincol = curwin->w_cursor.col;
		// should now fix the indent to match with the previous line
		call_fix_indent = TRUE;
	    }
	    curwin->w_cursor.col = save_col;
	}

	/*
	 * Handle deleting one 'shiftwidth' or 'softtabstop'.
	 */
	যদি (	   mode == BACKSPACE_CHAR
		&& ((p_sta && in_indent)
		    || ((get_sts_মান() != 0
#ifdef FEAT_VARTABS
			|| tabstop_গুনতি(curbuf->b_p_vsts_array)
#endif
			)
			&& curwin->w_cursor.col > 0
			&& (*(ml_get_cursor() - 1) == TAB
			    || (*(ml_get_cursor() - 1) == ' '
				&& (!*inserted_space_p
				    || arrow_used))))))
	{
	    colnr_T	vcol = 0;
	    colnr_T	want_vcol;
	    char_u	*সারি;
	    char_u	*ptr;
	    char_u	*cursor_ptr;
	    char_u	*space_ptr;
	    colnr_T	space_vcol = 0;
	    পূর্ণসংখ্যা		পূর্ব_space = FALSE;
	    colnr_T	want_col;

	    *inserted_space_p = FALSE;

	    space_ptr = ptr = সারি = ml_get_curline();
	    cursor_ptr = সারি + curwin->w_cursor.col;

	    // Compute virtual column of cursor position, and find the last
	    // whitespace before cursor that is preceded by non-whitespace.
	    // Use chartabsize() so that virtual text and wrapping are ignored.
	    যখন (ptr < cursor_ptr)
	    {
		পূর্ণসংখ্যা	cur_space = VIM_ISWHITE(*ptr);

		যদি (!পূর্ব_space && cur_space)
		{
		    space_ptr = ptr;
		    space_vcol = vcol;
		}
		vcol += chartabsize(ptr, vcol);
		MB_PTR_ADV(ptr);
		পূর্ব_space = cur_space;
	    }

	    // Compute the virtual column where we want to be.
	    want_vcol = vcol > 0 ? vcol - 1 : 0;
	    যদি (p_sta && in_indent)
		want_vcol -= want_vcol % (পূর্ণসংখ্যা)get_sw_মান(curbuf);
	    অন্যথা
#ifdef FEAT_VARTABS
		want_vcol = tabstop_start(want_vcol, get_sts_মান(),
						       curbuf->b_p_vsts_array);
#অন্যথা
		want_vcol -= want_vcol % (পূর্ণসংখ্যা)get_sts_মান();
#endif

	    // Find the position to stop backspacing.
	    // Use chartabsize() so that virtual text and wrapping are ignored.
	    যখন (TRUE)
	    {
		পূর্ণসংখ্যা আকার_2 = chartabsize(space_ptr, space_vcol);

		যদি (space_vcol + আকার_2 > want_vcol)
		    বিরতি;
		space_vcol += আকার_2;
		MB_PTR_ADV(space_ptr);
	    }
	    want_col = space_ptr - সারি;

	    // Delete characters until we are at or before want_col.
	    যখন (curwin->w_cursor.col > want_col)
		ins_bs_one();

	    // Insert extra spaces until we are at want_vcol.
	    জন্য (; space_vcol < want_vcol; space_vcol++)
	    {
		// Remember the first char we inserted
		যদি (curwin->w_cursor.lnum == Insstart_orig.lnum
				   && curwin->w_cursor.col < Insstart_orig.col)
		    Insstart_orig.col = curwin->w_cursor.col;

		যদি (State & VREPLACE_FLAG)
		    ins_char(' ');
		অন্যথা
		{
		    ins_str((char_u *)" ", 1);
		    যদি ((State & REPLACE_FLAG))
			replace_ঠেলো(NUL);
		}
	    }
	}

	/*
	 * Delete up to starting point, start of line or previous word.
	 */
	অন্যথা
	{
	    পূর্ণসংখ্যা cclass = 0, পূর্ব_cclass = 0;

	    যদি (has_mbyte)
		cclass = mb_get_class(ml_get_cursor());
	    করো
	    {
#ifdef FEAT_RIGHTLEFT
		যদি (!revins_on) // put cursor on char to be deleted
#endif
		    dec_cursor();

		cc = gchar_cursor();
		// look multi-byte character class
		যদি (has_mbyte)
		{
		    পূর্ব_cclass = cclass;
		    cclass = mb_get_class(ml_get_cursor());
		}

		// start of word?
		যদি (mode == BACKSPACE_WORD && !vim_isspace(cc))
		{
		    mode = BACKSPACE_WORD_NOT_SPACE;
		    অস্থায়ী = vim_iswordc(cc);
		}
		// end of word?
		অন্যথা যদি (mode == BACKSPACE_WORD_NOT_SPACE
			&& ((vim_isspace(cc) || vim_iswordc(cc) != অস্থায়ী)
			|| পূর্ব_cclass != cclass))
		{
#ifdef FEAT_RIGHTLEFT
		    যদি (!revins_on)
#endif
			inc_cursor();
#ifdef FEAT_RIGHTLEFT
		    অন্যথা যদি (State & REPLACE_FLAG)
			dec_cursor();
#endif
		    বিরতি;
		}
		যদি (State & REPLACE_FLAG)
		    replace_do_bs(-1);
		অন্যথা
		{
		    যদি (enc_utf8 && p_deco)
			(শূন্য)utfc_ptr2char(ml_get_cursor(), cpc);
		    (শূন্য)del_char(FALSE);
		    /*
		     * If there are combining characters and 'delcombine' is set
		     * move the cursor back.  Don't back up before the base
		     * character.
		     */
		    যদি (enc_utf8 && p_deco && cpc[0] != NUL)
			inc_cursor();
#ifdef FEAT_RIGHTLEFT
		    যদি (revins_chars)
		    {
			revins_chars--;
			revins_legal++;
		    }
		    যদি (revins_on && gchar_cursor() == NUL)
			বিরতি;
#endif
		}
		// Just a single backspace?:
		যদি (mode == BACKSPACE_CHAR)
		    বিরতি;
	    } যখন (
#ifdef FEAT_RIGHTLEFT
		    revins_on ||
#endif
		    (curwin->w_cursor.col > mincol
		    &&  (can_bs(BS_NOSTOP)
			|| (curwin->w_cursor.lnum != Insstart_orig.lnum
			|| curwin->w_cursor.col != Insstart_orig.col)
		    )));
	}
	did_backspace = TRUE;
    }
    did_si = FALSE;
    can_si = FALSE;
    can_si_back = FALSE;
    যদি (curwin->w_cursor.col <= 1)
	did_ai = FALSE;

    যদি (call_fix_indent)
	fix_indent();

    /*
     * It's a little strange to put backspaces into the redo
     * buffer, but it makes auto-indent a lot easier to deal
     * with.
     */
    AppendCharToRedobuff(c);

    // If deleted before the insertion point, adjust it
    যদি (curwin->w_cursor.lnum == Insstart_orig.lnum
				  && curwin->w_cursor.col < Insstart_orig.col)
	Insstart_orig.col = curwin->w_cursor.col;

    // vi behaviour: the cursor moves backward but the character that
    //		     was there remains visible
    // Vim behaviour: the cursor moves backward and the character that
    //		      was there is erased from the screen.
    // We can emulate the vi behaviour by pretending there is a dollar
    // displayed even when there isn't.
    //  --pkv Sun Jan 19 01:56:40 EST 2003
    যদি (vim_strchr(p_cpo, CPO_BACKSPACE) != ফাঁকা && dollar_vcol == -1)
	dollar_vcol = curwin->w_virtcol;

#ifdef FEAT_FOLDING
    // When deleting a char the cursor line must never be in a closed fold.
    // E.g., when 'foldmethod' is indent and deleting the first non-white
    // char before a Tab.
    যদি (did_backspace)
	foldOpenCursor();
#endif

    ফেরত did_backspace;
}

/*
 * Handle receiving P_PS: start paste mode.  Inserts the following text up to
 * P_PE literally.
 * When "drop" is TRUE then consume the text and drop it.
 */
    পূর্ণসংখ্যা
bracketed_paste(paste_mode_T mode, পূর্ণসংখ্যা drop, garray_T *gap)
{
    পূর্ণসংখ্যা		c;
    char_u	buf[NUMBUFLEN + MB_MAXBYTES];
    পূর্ণসংখ্যা		idx = 0;
    char_u	*end = find_termcode((char_u *)"PE");
    পূর্ণসংখ্যা		ret_char = -1;
    পূর্ণসংখ্যা		save_allow_চাবিগুলি = allow_চাবিগুলি;
    পূর্ণসংখ্যা		save_paste = p_paste;

    // If the end code is too long we can't detect it, read everything.
    যদি (end != ফাঁকা && STRLEN(end) >= NUMBUFLEN)
	end = ফাঁকা;
    ++no_mapping;
    allow_চাবিগুলি = 0;
    যদি (!p_paste)
	// Also have the side effects of setting 'paste' to make it work much
	// faster.
	set_option_মান_give_err((char_u *)"paste", TRUE, ফাঁকা, 0);

    জন্য (;;)
    {
	// When the end is not defined read everything there is.
	যদি (end == ফাঁকা && vpeekc() == NUL)
	    বিরতি;
	করো
	    c = vgetc();
	যখন (c == K_IGNORE || c == K_VER_SCROLLBAR || c == K_HOR_SCROLLBAR);

	যদি (c == NUL || got_int || (ex_normal_busy > 0 && c == Ctrl_C))
	    // When CTRL-C was encountered the typeahead will be flushed and we
	    // won't get the end sequence.  Except when using ":normal".
	    বিরতি;

	যদি (has_mbyte)
	    idx += (*mb_char2bytes)(c, buf + idx);
	অন্যথা
	    buf[idx++] = c;
	buf[idx] = NUL;
	যদি (end != ফাঁকা && STRNCMP(buf, end, idx) == 0)
	{
	    যদি (end[idx] == NUL)
		বিরতি; // Found the end of paste code.
	    চালু;
	}
	যদি (!drop)
	{
	    নির্বাচন (mode)
	    {
		ক্ষেত্র PASTE_CMDLINE:
		    put_on_cmdline(buf, idx, TRUE);
		    বিরতি;

		ক্ষেত্র PASTE_EX:
		    // add one for the NUL that is going to be appended
		    যদি (gap != ফাঁকা && ga_grow(gap, idx + 1) == OK)
		    {
			mch_memmove((অক্ষর *)gap->ga_তথ্য + gap->ga_len,
							     buf, (size_t)idx);
			gap->ga_len += idx;
		    }
		    বিরতি;

		ক্ষেত্র PASTE_ঢোকাও:
		    যদি (stop_arrow() == OK)
		    {
			c = buf[0];
			যদি (idx == 1 && (c == CAR || c == K_KENTER || c == NL))
			    ins_eol(c);
			অন্যথা
			{
			    ins_char_bytes(buf, idx);
			    AppendToRedobuffLit(buf, idx);
			}
		    }
		    বিরতি;

		ক্ষেত্র PASTE_ONE_CHAR:
		    যদি (ret_char == -1)
		    {
			যদি (has_mbyte)
			    ret_char = (*mb_ptr2char)(buf);
			অন্যথা
			    ret_char = buf[0];
		    }
		    বিরতি;
	    }
	}
	idx = 0;
    }

    --no_mapping;
    allow_চাবিগুলি = save_allow_চাবিগুলি;
    যদি (!save_paste)
	set_option_মান_give_err((char_u *)"paste", FALSE, ফাঁকা, 0);

    ফেরত ret_char;
}

#যদি defined(FEAT_GUI_TABLINE)
    স্থির শূন্য
ins_tabline(পূর্ণসংখ্যা c)
{
    // We will be leaving the current window, unless closing another tab.
    যদি (c != K_TABMENU || current_tabmenu != TABLINE_MENU_CLOSE
		|| (current_tab != 0 && current_tab != tabpage_সূচক(curtab)))
    {
	undisplay_dollar();
	start_arrow(&curwin->w_cursor);
	can_cindent = TRUE;
    }

    যদি (c == K_TABLINE)
	goto_tabpage(current_tab);
    অন্যথা
    {
	handle_tabmenu();
	redraw_statuslines();	// will redraw the tabline when needed
    }
}
#endif

#যদি defined(FEAT_GUI)
    শূন্য
ins_scroll(শূন্য)
{
    pos_T	tpos;

    undisplay_dollar();
    tpos = curwin->w_cursor;
    যদি (gui_do_scroll())
    {
	start_arrow(&tpos);
	can_cindent = TRUE;
    }
}

    শূন্য
ins_horscroll(শূন্য)
{
    pos_T	tpos;

    undisplay_dollar();
    tpos = curwin->w_cursor;
    যদি (do_mousescroll_horiz(scrollbar_মান))
    {
	start_arrow(&tpos);
	can_cindent = TRUE;
    }
}
#endif

    স্থির শূন্য
ins_বাম(শূন্য)
{
    pos_T	tpos;
    পূর্ণসংখ্যা		end_change = dont_sync_undo == FALSE; // end undoable change

#ifdef FEAT_FOLDING
    যদি ((fdo_flags & FDO_HOR) && চাবি_Typed)
	foldOpenCursor();
#endif
    undisplay_dollar();
    tpos = curwin->w_cursor;
    যদি (oneleft() == OK)
    {
#যদি defined(FEAT_XIM) && defined(FEAT_GUI_GTK)
	// Only call start_arrow() when not busy with preediting, it will
	// break undo.  K_LEFT is inserted in im_correct_cursor().
	যদি (p_imst == IM_OVER_THE_SPOT || !im_is_preediting())
#endif
	{
	    start_arrow_with_change(&tpos, end_change);
	    যদি (!end_change)
		AppendCharToRedobuff(K_বাম);
	}
#ifdef FEAT_RIGHTLEFT
	// If exit reversed string, position is fixed
	যদি (revins_scol != -1 && (পূর্ণসংখ্যা)curwin->w_cursor.col >= revins_scol)
	    revins_legal++;
	revins_chars++;
#endif
    }

    /*
     * if 'whichwrap' set for cursor in insert mode may go to
     * previous line
     */
    অন্যথা যদি (vim_strchr(p_ww, '[') != ফাঁকা && curwin->w_cursor.lnum > 1)
    {
	// always break undo when moving upwards/downwards, else undo may break
	start_arrow(&tpos);
	--(curwin->w_cursor.lnum);
	coladvance((colnr_T)MAXCOL);
	curwin->w_set_curswant = true;	// so we stay at the end
    }
    অন্যথা
	vim_beep(BO_CRSR);
    dont_sync_undo = FALSE;
}

    স্থির শূন্য
ins_home(পূর্ণসংখ্যা c)
{
    pos_T	tpos;

#ifdef FEAT_FOLDING
    যদি ((fdo_flags & FDO_HOR) && চাবি_Typed)
	foldOpenCursor();
#endif
    undisplay_dollar();
    tpos = curwin->w_cursor;
    যদি (c == K_C_HOME)
	curwin->w_cursor.lnum = 1;
    curwin->w_cursor.col = 0;
    curwin->w_cursor.coladd = 0;
    curwin->w_curswant = 0;
    start_arrow(&tpos);
}

    স্থির শূন্য
ins_end(পূর্ণসংখ্যা c)
{
    pos_T	tpos;

#ifdef FEAT_FOLDING
    যদি ((fdo_flags & FDO_HOR) && চাবি_Typed)
	foldOpenCursor();
#endif
    undisplay_dollar();
    tpos = curwin->w_cursor;
    যদি (c == K_C_END)
	curwin->w_cursor.lnum = curbuf->b_ml.ml_সারি_গুনতি;
    coladvance((colnr_T)MAXCOL);
    curwin->w_curswant = MAXCOL;

    start_arrow(&tpos);
}

    স্থির শূন্য
ins_s_বাম(শূন্য)
{
    পূর্ণসংখ্যা end_change = dont_sync_undo == FALSE; // end undoable change
#ifdef FEAT_FOLDING
    যদি ((fdo_flags & FDO_HOR) && চাবি_Typed)
	foldOpenCursor();
#endif
    undisplay_dollar();
    যদি (curwin->w_cursor.lnum > 1 || curwin->w_cursor.col > 0)
    {
	start_arrow_with_change(&curwin->w_cursor, end_change);
	যদি (!end_change)
	    AppendCharToRedobuff(K_S_বাম);
	(শূন্য)bck_word(1L, FALSE, FALSE);
	curwin->w_set_curswant = true;
    }
    অন্যথা
	vim_beep(BO_CRSR);
    dont_sync_undo = FALSE;
}

    স্থির শূন্য
ins_ডান(শূন্য)
{
    পূর্ণসংখ্যা end_change = dont_sync_undo == FALSE; // end undoable change

#ifdef FEAT_FOLDING
    যদি ((fdo_flags & FDO_HOR) && চাবি_Typed)
	foldOpenCursor();
#endif
    undisplay_dollar();
    যদি (gchar_cursor() != NUL || virtual_active())
    {
	start_arrow_with_change(&curwin->w_cursor, end_change);
	যদি (!end_change)
	    AppendCharToRedobuff(K_ডান);
	curwin->w_set_curswant = true;
	যদি (virtual_active())
	    oneright();
	অন্যথা
	{
	    যদি (has_mbyte)
		curwin->w_cursor.col += (*mb_ptr2len)(ml_get_cursor());
	    অন্যথা
		++curwin->w_cursor.col;
	}

#ifdef FEAT_RIGHTLEFT
	revins_legal++;
	যদি (revins_chars)
	    revins_chars--;
#endif
    }
    // if 'whichwrap' set for cursor in insert mode, may move the
    // cursor to the next line
    অন্যথা যদি (vim_strchr(p_ww, ']') != ফাঁকা
	    && curwin->w_cursor.lnum < curbuf->b_ml.ml_সারি_গুনতি)
    {
	start_arrow(&curwin->w_cursor);
	curwin->w_set_curswant = true;
	++curwin->w_cursor.lnum;
	curwin->w_cursor.col = 0;
    }
    অন্যথা
	vim_beep(BO_CRSR);
    dont_sync_undo = FALSE;
}

    স্থির শূন্য
ins_s_ডান(শূন্য)
{
    পূর্ণসংখ্যা end_change = dont_sync_undo == FALSE; // end undoable change
#ifdef FEAT_FOLDING
    যদি ((fdo_flags & FDO_HOR) && চাবি_Typed)
	foldOpenCursor();
#endif
    undisplay_dollar();
    যদি (curwin->w_cursor.lnum < curbuf->b_ml.ml_সারি_গুনতি
	    || gchar_cursor() != NUL)
    {
	start_arrow_with_change(&curwin->w_cursor, end_change);
	যদি (!end_change)
	    AppendCharToRedobuff(K_S_ডান);
	(শূন্য)fwd_word(1L, FALSE, 0);
	curwin->w_set_curswant = true;
    }
    অন্যথা
	vim_beep(BO_CRSR);
    dont_sync_undo = FALSE;
}

    স্থির শূন্য
ins_up(
    পূর্ণসংখ্যা		startcol)	// when TRUE move to Insstart.col
{
    pos_T	tpos;
    linenr_T	old_topline = curwin->w_topline;
#ifdef FEAT_DIFF
    পূর্ণসংখ্যা		old_topfill = curwin->w_topfill;
#endif

    undisplay_dollar();
    tpos = curwin->w_cursor;
    যদি (cursor_up(1L, TRUE) == OK)
    {
	যদি (startcol)
	    coladvance(getvcol_nolist(&Insstart));
	যদি (old_topline != curwin->w_topline
#ifdef FEAT_DIFF
		|| old_topfill != curwin->w_topfill
#endif
		)
	    redraw_later(UPD_VALID);
	start_arrow(&tpos);
	can_cindent = TRUE;
    }
    অন্যথা
	vim_beep(BO_CRSR);
}

    স্থির শূন্য
ins_pageup(শূন্য)
{
    pos_T	tpos;

    undisplay_dollar();

    যদি (mod_mask & MOD_MASK_CTRL)
    {
	// <C-PageUp>: tab page back
	যদি (first_tabpage->tp_পরবর্তী != ফাঁকা)
	{
	    start_arrow(&curwin->w_cursor);
	    goto_tabpage(-1);
	}
	ফেরত;
    }

    tpos = curwin->w_cursor;
    যদি (pagescroll(BACKWARD, 1L, FALSE) == OK)
    {
	start_arrow(&tpos);
	can_cindent = TRUE;
    }
    অন্যথা
	vim_beep(BO_CRSR);
}

    স্থির শূন্য
ins_down(
    পূর্ণসংখ্যা		startcol)	// when TRUE move to Insstart.col
{
    pos_T	tpos;
    linenr_T	old_topline = curwin->w_topline;
#ifdef FEAT_DIFF
    পূর্ণসংখ্যা		old_topfill = curwin->w_topfill;
#endif

    undisplay_dollar();
    tpos = curwin->w_cursor;
    যদি (cursor_down(1L, TRUE) == OK)
    {
	যদি (startcol)
	    coladvance(getvcol_nolist(&Insstart));
	যদি (old_topline != curwin->w_topline
#ifdef FEAT_DIFF
		|| old_topfill != curwin->w_topfill
#endif
		)
	    redraw_later(UPD_VALID);
	start_arrow(&tpos);
	can_cindent = TRUE;
    }
    অন্যথা
	vim_beep(BO_CRSR);
}

    স্থির শূন্য
ins_pagedown(শূন্য)
{
    pos_T	tpos;

    undisplay_dollar();

    যদি (mod_mask & MOD_MASK_CTRL)
    {
	// <C-PageDown>: tab page forward
	যদি (first_tabpage->tp_পরবর্তী != ফাঁকা)
	{
	    start_arrow(&curwin->w_cursor);
	    goto_tabpage(0);
	}
	ফেরত;
    }

    tpos = curwin->w_cursor;
    যদি (pagescroll(FORWARD, 1L, FALSE) == OK)
    {
	start_arrow(&tpos);
	can_cindent = TRUE;
    }
    অন্যথা
	vim_beep(BO_CRSR);
}

#ifdef FEAT_DND
    স্থির শূন্য
ins_drop(শূন্য)
{
    do_put('~', ফাঁকা, BACKWARD, 1L, PUT_CURSEND);
}
#endif

/*
 * Handle TAB in Insert or Replace mode.
 * Return TRUE when the TAB needs to be inserted like a normal character.
 */
    স্থির পূর্ণসংখ্যা
ins_tab(শূন্য)
{
    পূর্ণসংখ্যা		ind;
    পূর্ণসংখ্যা		i;
    পূর্ণসংখ্যা		অস্থায়ী;

    যদি (Insstart_blank_vcol == MAXCOL && curwin->w_cursor.lnum == Insstart.lnum)
	Insstart_blank_vcol = get_nolist_virtcol();
    যদি (echeck_abbr(TAB + ABBR_OFF))
	ফেরত FALSE;

    ind = inindent(0);
    যদি (ind)
	can_cindent = FALSE;

    /*
     * When nothing special, insert TAB like a normal character.
     */
    যদি (!curbuf->b_p_et
#ifdef FEAT_VARTABS
	    && !(p_sta && ind
		// These five lines mean 'tabstop' != 'shiftwidth'
		&& ((tabstop_গুনতি(curbuf->b_p_vts_array) > 1)
		    || (tabstop_গুনতি(curbuf->b_p_vts_array) == 1
			&& tabstop_first(curbuf->b_p_vts_array)
						       != get_sw_মান(curbuf))
		    || (tabstop_গুনতি(curbuf->b_p_vts_array) == 0
			&& curbuf->b_p_ts != get_sw_মান(curbuf))))
	    && tabstop_গুনতি(curbuf->b_p_vsts_array) == 0
#অন্যথা
	    && !(p_sta && ind && curbuf->b_p_ts != get_sw_মান(curbuf))
#endif
	    && get_sts_মান() == 0)
	ফেরত TRUE;

    যদি (stop_arrow() == FAIL)
	ফেরত TRUE;

    did_ai = FALSE;
    did_si = FALSE;
    can_si = FALSE;
    can_si_back = FALSE;
    AppendToRedobuff((char_u *)"\t");

#ifdef FEAT_VARTABS
    যদি (p_sta && ind)		// insert tab in indent, use 'shiftwidth'
    {
	অস্থায়ী = (পূর্ণসংখ্যা)get_sw_মান(curbuf);
	অস্থায়ী -= get_nolist_virtcol() % অস্থায়ী;
    }
    অন্যথা যদি (tabstop_গুনতি(curbuf->b_p_vsts_array) > 0 || curbuf->b_p_sts != 0)
				// use 'softtabstop' when set
	অস্থায়ী = tabstop_padding(get_nolist_virtcol(), get_sts_মান(),
						     curbuf->b_p_vsts_array);
    অন্যথা			// otherwise use 'tabstop'
	অস্থায়ী = tabstop_padding(get_nolist_virtcol(), curbuf->b_p_ts,
						     curbuf->b_p_vts_array);
#অন্যথা
    যদি (p_sta && ind)		// insert tab in indent, use 'shiftwidth'
	অস্থায়ী = (পূর্ণসংখ্যা)get_sw_মান(curbuf);
    অন্যথা যদি (curbuf->b_p_sts != 0) // use 'softtabstop' when set
	অস্থায়ী = (পূর্ণসংখ্যা)get_sts_মান();
    অন্যথা			// otherwise use 'tabstop'
	অস্থায়ী = (পূর্ণসংখ্যা)curbuf->b_p_ts;
    অস্থায়ী -= get_nolist_virtcol() % অস্থায়ী;
#endif

    /*
     * Insert the first space with ins_char().	It will delete one char in
     * replace mode.  Insert the rest with ins_str(); it will not delete any
     * chars.  For MODE_VREPLACE state, we use ins_char() for all characters.
     */
    ins_char(' ');
    যখন (--অস্থায়ী > 0)
    {
	যদি (State & VREPLACE_FLAG)
	    ins_char(' ');
	অন্যথা
	{
	    ins_str((char_u *)" ", 1);
	    যদি (State & REPLACE_FLAG)	    // no char replaced
		replace_ঠেলো(NUL);
	}
    }

    /*
     * When 'expandtab' not set: Replace spaces by TABs where possible.
     */
#ifdef FEAT_VARTABS
    যদি (!curbuf->b_p_et && (tabstop_গুনতি(curbuf->b_p_vsts_array) > 0
			    || get_sts_মান() > 0
			    || (p_sta && ind)))
#অন্যথা
    যদি (!curbuf->b_p_et && (get_sts_মান() || (p_sta && ind)))
#endif
    {
	char_u		*ptr;
	char_u		*saved_সারি = ফাঁকা;	// init for GCC
	pos_T		pos;
	pos_T		fpos;
	pos_T		*cursor;
	colnr_T		want_vcol, vcol;
	পূর্ণসংখ্যা		change_col = -1;
	পূর্ণসংখ্যা		save_list = curwin->w_p_list;
	char_u		*tab = (char_u *)"\t";
	chartabsize_T	cts;

	/*
	 * Get the current line.  For MODE_VREPLACE state, don't make real
	 * changes yet, just work on a copy of the line.
	 */
	যদি (State & VREPLACE_FLAG)
	{
	    pos = curwin->w_cursor;
	    cursor = &pos;
	    saved_সারি = vim_strnsave(ml_get_curline(), ml_get_curline_len());
	    যদি (saved_সারি == ফাঁকা)
		ফেরত FALSE;
	    ptr = saved_সারি + pos.col;
	}
	অন্যথা
	{
	    ptr = ml_get_cursor();
	    cursor = &curwin->w_cursor;
	}

	// When 'L' is not in 'cpoptions' a tab always takes up 'ts' spaces.
	যদি (vim_strchr(p_cpo, CPO_LISTWM) == ফাঁকা)
	    curwin->w_p_list = FALSE;

	// Find first white before the cursor
	fpos = curwin->w_cursor;
	যখন (fpos.col > 0 && VIM_ISWHITE(ptr[-1]))
	{
	    --fpos.col;
	    --ptr;
	}

	// In Replace mode, don't change characters before the insert point.
	যদি ((State & REPLACE_FLAG)
		&& fpos.lnum == Insstart.lnum
		&& fpos.col < Insstart.col)
	{
	    ptr += Insstart.col - fpos.col;
	    fpos.col = Insstart.col;
	}

	// compute virtual column numbers of first white and cursor
	getvcol(curwin, &fpos, &vcol, ফাঁকা, ফাঁকা, 0);
	getvcol(curwin, cursor, &want_vcol, ফাঁকা, ফাঁকা, 0);

	init_chartabsize_arg(&cts, curwin, 0, vcol, tab, tab);

	// Use as many TABs as possible.  Beware of 'breakindent', 'showbreak'
	// and 'linebreak' adding extra virtual columns.
	যখন (VIM_ISWHITE(*ptr))
	{
	    i = lbr_chartabsize(&cts);
	    যদি (cts.cts_vcol + i > want_vcol)
		বিরতি;
	    যদি (*ptr != TAB)
	    {
		*ptr = TAB;
		যদি (change_col < 0)
		{
		    change_col = fpos.col;  // Column of first change
		    // May have to adjust Insstart
		    যদি (fpos.lnum == Insstart.lnum && fpos.col < Insstart.col)
			Insstart.col = fpos.col;
		}
	    }
	    ++fpos.col;
	    ++ptr;
	    cts.cts_vcol += i;
	}
	vcol = cts.cts_vcol;
	clear_chartabsize_arg(&cts);

	যদি (change_col >= 0)
	{
	    পূর্ণসংখ্যা		    repl_off = 0;

	    // Skip over the spaces we need.
	    init_chartabsize_arg(&cts, curwin, 0, vcol, ptr, ptr);
	    যখন (cts.cts_vcol < want_vcol && *cts.cts_ptr == ' ')
	    {
		cts.cts_vcol += lbr_chartabsize(&cts);
		++cts.cts_ptr;
		++repl_off;
	    }
	    ptr = cts.cts_ptr;
	    vcol = cts.cts_vcol;
	    clear_chartabsize_arg(&cts);

	    যদি (vcol > want_vcol)
	    {
		// Must have a char with 'showbreak' just before it.
		--ptr;
		--repl_off;
	    }
	    fpos.col += repl_off;

	    // Delete following spaces.
	    i = cursor->col - fpos.col;
	    যদি (i > 0)
	    {
#ifdef FEAT_PROP_POPUP
		যদি (!(State & VREPLACE_FLAG))
		{
		    char_u  *newp;
		    পূর্ণসংখ্যা	    col;

		    newp = alloc(curbuf->b_ml.ml_সারি_len - i);
		    যদি (newp == ফাঁকা)
			ফেরত FALSE;

		    col = ptr - curbuf->b_ml.ml_সারি_ptr;
		    যদি (col > 0)
			mch_memmove(newp, ptr - col, col);
		    mch_memmove(newp + col, ptr + i,
					   curbuf->b_ml.ml_সারি_len - col - i);

		    যদি (curbuf->b_ml.ml_flags & (ML_সারি_DIRTY | ML_ALLOCATED))
			vim_free(curbuf->b_ml.ml_সারি_ptr);
		    curbuf->b_ml.ml_সারি_ptr = newp;
		    curbuf->b_ml.ml_সারি_len -= i;
		    curbuf->b_ml.ml_সারি_textlen = 0;
		    curbuf->b_ml.ml_flags =
			   (curbuf->b_ml.ml_flags | ML_সারি_DIRTY) & ~ML_EMPTY;
		}
		অন্যথা
#endif
		    STRMOVE(ptr, ptr + i);
		// correct replace stack.
		যদি ((State & REPLACE_FLAG) && !(State & VREPLACE_FLAG))
		    জন্য (অস্থায়ী = i; --অস্থায়ী >= 0; )
			replace_join(repl_off);
	    }
#ifdef FEAT_NETBEANS_INTG
	    যদি (netbeans_active())
	    {
		netbeans_removed(curbuf, fpos.lnum, cursor->col, (long)(i + 1));
		netbeans_inserted(curbuf, fpos.lnum, cursor->col,
							   (char_u *)"\t", 1);
	    }
#endif
	    cursor->col -= i;

	    /*
	     * In MODE_VREPLACE state, we haven't changed anything yet.  Do it
	     * now by backspacing over the changed spacing and then inserting
	     * the new spacing.
	     */
	    যদি (State & VREPLACE_FLAG)
	    {
		// Backspace from real cursor to change_col
		backspace_until_column(change_col);

		// Insert each char in saved_line from changed_col to
		// ptr-cursor
		ins_bytes_len(saved_সারি + change_col,
						    cursor->col - change_col);
	    }
	}

	যদি (State & VREPLACE_FLAG)
	    vim_free(saved_সারি);
	curwin->w_p_list = save_list;
    }

    ফেরত FALSE;
}

/*
 * Handle CR or NL in insert mode.
 * Return FAIL when out of memory or can't undo.
 */
    পূর্ণসংখ্যা
ins_eol(পূর্ণসংখ্যা c)
{
    পূর্ণসংখ্যা	    i;

    যদি (echeck_abbr(c + ABBR_OFF))
	ফেরত OK;
    যদি (stop_arrow() == FAIL)
	ফেরত FAIL;
    undisplay_dollar();

    /*
     * Strange Vi behaviour: In Replace mode, typing a NL will not delete the
     * character under the cursor.  Only push a NUL on the replace stack,
     * nothing to put back when the NL is deleted.
     */
    যদি ((State & REPLACE_FLAG) && !(State & VREPLACE_FLAG))
	replace_ঠেলো(NUL);

    /*
     * In MODE_VREPLACE state, a NL replaces the rest of the line, and starts
     * replacing the next line, so we push all of the characters left on the
     * line onto the replace stack.  This is not done here though, it is done
     * in open_line().
     */

    // Put cursor on NUL if on the last char and coladd is 1 (happens after
    // CTRL-O).
    যদি (virtual_active() && curwin->w_cursor.coladd > 0)
	coladvance(getviscol());

#ifdef FEAT_RIGHTLEFT
    // NL in reverse insert will always start in the end of
    // current line.
    যদি (revins_on)
	curwin->w_cursor.col += ml_get_cursor_len();
#endif

    AppendToRedobuff(NL_STR);
    i = open_সারি(FORWARD,
	    has_format_option(FO_RET_COMS) ? OPENLINE_DO_COM : 0, old_indent,
	    ফাঁকা);
    old_indent = 0;
    can_cindent = TRUE;
#ifdef FEAT_FOLDING
    // When inserting a line the cursor line must never be in a closed fold.
    foldOpenCursor();
#endif

    ফেরত i;
}

#ifdef FEAT_DIGRAPHS
/*
 * Handle digraph in insert mode.
 * Returns character still to be inserted, or NUL when nothing remaining to be
 * done.
 */
    স্থির পূর্ণসংখ্যা
ins_digraph(শূন্য)
{
    পূর্ণসংখ্যা	    c;
    পূর্ণসংখ্যা	    cc;
    পূর্ণসংখ্যা	    did_putchar = FALSE;

    pc_status = PC_STATUS_UNSET;
    যদি (redrawing() && !char_avail())
    {
	// may need to redraw when no more chars available now
	ins_redraw(FALSE);

	edit_putchar('?', TRUE);
	did_putchar = TRUE;
	add_to_showcmd_c(Ctrl_K);
    }

# ifdef USE_ON_FLY_SCROLL
    dont_scroll = TRUE;		// disallow scrolling here
# endif

    // don't map the digraph chars. This also prevents the
    // mode message to be deleted when ESC is hit
    ++no_mapping;
    ++allow_চাবিগুলি;
    c = plain_vgetc();
    --no_mapping;
    --allow_চাবিগুলি;
    যদি (did_putchar)
	// when the line fits in 'columns' the '?' is at the start of the next
	// line and will not be removed by the redraw
	edit_unputchar();

    যদি (IS_SPECIAL(c) || mod_mask)	    // special key
    {
	clear_showcmd();
	ঢোকাও_special(c, TRUE, FALSE);
	ফেরত NUL;
    }
    যদি (c != ESC)
    {
	did_putchar = FALSE;
	যদি (redrawing() && !char_avail())
	{
	    // may need to redraw when no more chars available now
	    ins_redraw(FALSE);

	    যদি (char2cells(c) == 1)
	    {
		ins_redraw(FALSE);
		edit_putchar(c, TRUE);
		did_putchar = TRUE;
	    }
	    add_to_showcmd_c(c);
	}
	++no_mapping;
	++allow_চাবিগুলি;
	cc = plain_vgetc();
	--no_mapping;
	--allow_চাবিগুলি;
	যদি (did_putchar)
	    // when the line fits in 'columns' the '?' is at the start of the
	    // next line and will not be removed by a redraw
	    edit_unputchar();
	যদি (cc != ESC)
	{
	    AppendToRedobuff((char_u *)CTRL_V_STR);
	    c = digraph_get(c, cc, TRUE);
	    clear_showcmd();
	    ফেরত c;
	}
    }
    clear_showcmd();
    ফেরত NUL;
}
#endif

/*
 * Handle CTRL-E and CTRL-Y in Insert mode: copy char from other line.
 * Returns the char to be inserted, or NUL if none found.
 */
    পূর্ণসংখ্যা
ins_copychar(linenr_T lnum)
{
    পূর্ণসংখ্যা		    c;
    char_u	    *ptr, *পূর্ব_ptr;
    char_u	    *সারি;
    chartabsize_T   cts;

    যদি (lnum < 1 || lnum > curbuf->b_ml.ml_সারি_গুনতি)
    {
	vim_beep(BO_COPY);
	ফেরত NUL;
    }

    // try to advance to the cursor column
    validate_virtcol();
    সারি = ml_get(lnum);
    পূর্ব_ptr = সারি;
    init_chartabsize_arg(&cts, curwin, lnum, 0, সারি, সারি);
    যখন (cts.cts_vcol < curwin->w_virtcol && *cts.cts_ptr != NUL)
    {
	পূর্ব_ptr = cts.cts_ptr;
	cts.cts_vcol += lbr_chartabsize_adv(&cts);
    }
    যদি (cts.cts_vcol > curwin->w_virtcol)
	ptr = পূর্ব_ptr;
    অন্যথা
	ptr = cts.cts_ptr;
    clear_chartabsize_arg(&cts);

    c = (*mb_ptr2char)(ptr);
    যদি (c == NUL)
	vim_beep(BO_COPY);
    ফেরত c;
}

/*
 * CTRL-Y or CTRL-E typed in Insert mode.
 */
    স্থির পূর্ণসংখ্যা
ins_ctrl_ey(পূর্ণসংখ্যা tc)
{
    পূর্ণসংখ্যা	    c = tc;

    যদি (ctrl_x_mode_scroll())
    {
	যদি (c == Ctrl_Y)
	    scrolldown_clamp();
	অন্যথা
	    scrollup_clamp();
	redraw_later(UPD_VALID);
    }
    অন্যথা
    {
	c = ins_copychar(curwin->w_cursor.lnum + (c == Ctrl_Y ? -1 : 1));
	যদি (c != NUL)
	{
	    long	tw_save;

	    // The character must be taken literally, insert like it
	    // was typed after a CTRL-V, and pretend 'textwidth'
	    // wasn't set.  Digits, 'o' and 'x' are special after a
	    // CTRL-V, don't use it for these.
	    যদি (c < 256 && !SAFE_isalnum(c))
		AppendToRedobuff((char_u *)CTRL_V_STR);	// CTRL-V
	    tw_save = curbuf->b_p_tw;
	    curbuf->b_p_tw = -1;
	    ঢোকাও_special(c, TRUE, FALSE);
	    curbuf->b_p_tw = tw_save;
#ifdef FEAT_RIGHTLEFT
	    revins_chars++;
	    revins_legal++;
#endif
	    c = Ctrl_V;	// pretend CTRL-V is last character
	    auto_format(FALSE, TRUE);
	}
    }
    ফেরত c;
}

/*
 * Get the value that w_virtcol would have when 'list' is off.
 * Unless 'cpo' contains the 'L' flag.
 */
    colnr_T
get_nolist_virtcol(শূন্য)
{
    // check validity of cursor in current buffer
    যদি (curwin->w_বাফার == ফাঁকা
	|| curwin->w_বাফার->b_ml.ml_mfp == ফাঁকা
	|| curwin->w_cursor.lnum > curwin->w_বাফার->b_ml.ml_সারি_গুনতি)
	ফেরত 0;
    যদি (curwin->w_p_list && vim_strchr(p_cpo, CPO_LISTWM) == ফাঁকা)
	ফেরত getvcol_nolist(&curwin->w_cursor);
    validate_virtcol();
    ফেরত curwin->w_virtcol;
}

#যদি defined(FEAT_EVAL)
/*
 * Handle the InsertCharPre autocommand.
 * "c" is the character that was typed.
 * Return a pointer to allocated memory with the replacement string.
 * Return NULL to continue inserting "c".
 */
    স্থির char_u *
do_ঢোকাও_char_pre(পূর্ণসংখ্যা c)
{
    char_u	*res;
    char_u	buf[MB_MAXBYTES + 1];
    size_t	buflen;
    পূর্ণসংখ্যা		save_State = State;

    // Return quickly when there is nothing to do.
    যদি (!has_insertcharpre())
	ফেরত ফাঁকা;

    যদি (c == Ctrl_RSB)
	ফেরত ফাঁকা;

    যদি (has_mbyte)
	buflen = (*mb_char2bytes)(c, buf);
    অন্যথা
    {
	buf[0] = c;
	buflen = 1;
    }
    buf[buflen] = NUL;

    // Lock the text to avoid weird things from happening.
    ++textlock;
    set_vim_var_string(VV_CHAR, buf, (পূর্ণসংখ্যা)buflen);  // set v:char

    res = ফাঁকা;
    যদি (ins_apply_autocmds(EVENT_INSERTCHARPRE))
    {
	// Get the value of v:char.  It may be empty or more than one
	// character.  Only use it when changed, otherwise continue with the
	// original character to avoid breaking autoindent.
	যদি (STRCMP(buf, get_vim_var_str(VV_CHAR)) != 0)
	    res = vim_strsave(get_vim_var_str(VV_CHAR));
    }

    set_vim_var_string(VV_CHAR, ফাঁকা, -1);  // clear v:char
    --textlock;

    // Restore the State, it may have been changed.
    State = save_State;

    ফেরত res;
}
#endif

    পূর্ণসংখ্যা
get_can_cindent(শূন্য)
{
    ফেরত can_cindent;
}

    শূন্য
set_can_cindent(পূর্ণসংখ্যা val)
{
    can_cindent = val;
}

/*
 * Trigger "event" and take care of fixing undo.
 */
    পূর্ণসংখ্যা
ins_apply_autocmds(event_T event)
{
    varnumber_T	tick = CHANGEDTICK(curbuf);
    পূর্ণসংখ্যা r;

    r = apply_autocmds(event, ফাঁকা, ফাঁকা, FALSE, curbuf);

    // If u_savesub() was called then we are not prepared to start
    // a new line.  Call u_save() with no contents to fix that.
    // Except when leaving Insert mode.
    যদি (event != EVENT_INSERTLEAVE && tick != CHANGEDTICK(curbuf))
	u_save(curwin->w_cursor.lnum, (linenr_T)(curwin->w_cursor.lnum + 1));

    ফেরত r;
}
