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

#अन्तर्भाव "vim.h"

#define BACKSPACE_CHAR		    1
#define BACKSPACE_WORD		    2
#define BACKSPACE_WORD_NOT_SPACE    3
#define BACKSPACE_पङ्क्तिः		    4

// Set when doing something for completion that may call edit() recursively,
// which is not allowed.
स्थिर पूर्णाङ्क	compl_busy = FALSE;


स्थिर शून्य ins_ctrl_v(शून्य);
स्थिर शून्य निवेशय_special(पूर्णाङ्क, पूर्णाङ्क, पूर्णाङ्क);
स्थिर शून्य redo_literal(पूर्णाङ्क c);
स्थिर शून्य start_arrow_common(pos_T *end_निवेशय_pos, पूर्णाङ्क change);
#ifdef FEAT_SPELL
स्थिर शून्य check_spell_redraw(शून्य);
#endif
स्थिर शून्य stop_निवेशय(pos_T *end_निवेशय_pos, पूर्णाङ्क esc, पूर्णाङ्क nomove);
स्थिर पूर्णाङ्क  echeck_abbr(पूर्णाङ्क);
स्थिर शून्य mb_replace_निष्कासय_ins(पूर्णाङ्क cc);
स्थिर शून्य replace_flush(शून्य);
स्थिर शून्य replace_do_bs(पूर्णाङ्क limit_col);
स्थिर पूर्णाङ्क del_char_after_col(पूर्णाङ्क limit_col);
स्थिर शून्य ins_reg(शून्य);
स्थिर शून्य ins_ctrl_g(शून्य);
स्थिर शून्य ins_ctrl_hat(शून्य);
स्थिर पूर्णाङ्क  ins_esc(long *गणनम्, पूर्णाङ्क cmdchar, पूर्णाङ्क nomove);
#ifdef FEAT_RIGHTLEFT
स्थिर शून्य ins_ctrl_(शून्य);
#endif
स्थिर पूर्णाङ्क ins_start_select(पूर्णाङ्क c);
स्थिर शून्य ins_निवेशय(पूर्णाङ्क replaceState);
स्थिर शून्य ins_ctrl_o(शून्य);
स्थिर शून्य ins_shift(पूर्णाङ्क c, पूर्णाङ्क lastc);
स्थिर शून्य ins_del(शून्य);
स्थिर पूर्णाङ्क  ins_bs(पूर्णाङ्क c, पूर्णाङ्क mode, पूर्णाङ्क *inserted_space_p);
#सचे defined(FEAT_GUI_TABLINE)
स्थिर शून्य ins_tabline(पूर्णाङ्क c);
#endif
स्थिर शून्य ins_वामः(शून्य);
स्थिर शून्य ins_home(पूर्णाङ्क c);
स्थिर शून्य ins_end(पूर्णाङ्क c);
स्थिर शून्य ins_s_वामः(शून्य);
स्थिर शून्य ins_दक्षिणः(शून्य);
स्थिर शून्य ins_s_दक्षिणः(शून्य);
स्थिर शून्य ins_up(पूर्णाङ्क startcol);
स्थिर शून्य ins_pageup(शून्य);
स्थिर शून्य ins_down(पूर्णाङ्क startcol);
स्थिर शून्य ins_pagedown(शून्य);
#ifdef FEAT_DND
स्थिर शून्य ins_drop(शून्य);
#endif
स्थिर पूर्णाङ्क  ins_tab(शून्य);
#ifdef FEAT_DIGRAPHS
स्थिर पूर्णाङ्क  ins_digraph(शून्य);
#endif
स्थिर पूर्णाङ्क  ins_ctrl_ey(पूर्णाङ्क tc);
#सचे defined(FEAT_EVAL)
स्थिर char_u *do_निवेशय_char_pre(पूर्णाङ्क c);
#endif

स्थिर colnr_T	Insstart_textlen;	// length of line when insert started
स्थिर colnr_T	Insstart_blank_vcol;	// vcol for first inserted blank
स्थिर पूर्णाङ्क	update_Insstart_orig = TRUE; // set Insstart_orig to Insstart

स्थिर string_T	last_निवेशय = {रिक्त, 0};    // the text of the previous insert,
					    // K_SPECIAL and CSI are escaped
स्थिर पूर्णाङ्क	last_निवेशय_skip; // nr of chars in front of previous insert
स्थिर पूर्णाङ्क	new_निवेशय_skip;  // nr of chars in front of current insert
स्थिर पूर्णाङ्क	did_restart_edit;	// "restart_edit" when calling edit()

स्थिर पूर्णाङ्क	can_cindent;		// may do cindenting on this line

#ifdef FEAT_RIGHTLEFT
स्थिर पूर्णाङ्क	revins_on;		// reverse insert mode on
स्थिर पूर्णाङ्क	revins_chars;		// how much to skip after edit
स्थिर पूर्णाङ्क	revins_legal;		// was the last char 'legal'?
स्थिर पूर्णाङ्क	revins_scol;		// start column of revins session
#endif

स्थिर पूर्णाङ्क	ins_need_undo;		// call u_save() before inserting a
					// char.  Set when edit() is called.
					// after that arrow_used is used.

स्थिर पूर्णाङ्क	dont_sync_undo = FALSE;	// CTRL-G U prevents syncing undo for
					// the next left/right cursor key

// With 'autocompletedelay' set, arm the delay and let the main loop fire
// Insert-mode autocommands; the popup is shown later on K_COMPLETE_DELAY.
// Otherwise trigger completion right away.
#define TRIGGER_AUTOCOMPLETE()			\
    करोहि {					\
	update_screen(UPD_VALID);  /* Show char (deletion) immediately */ \
	out_flush();				\
	ins_compl_enable_autocomplete();	\
	ins_compl_arm_autostart();		\
	सचे (!ins_compl_arm_autocomplete_delay())\
	    गच्छ docomplete;			\
    } याव (0)

#define MAY_TRIGGER_AUTOCOMPLETE(c)				\
    करोहि {							\
	सचे (ins_compl_has_autocomplete() && !char_avail()	\
		&& curwin->w_cursor.col > 0)			\
	{							\
	    (c) = char_before_cursor();				\
	    सचे (vim_isprintc(c))				\
		TRIGGER_AUTOCOMPLETE();				\
	}							\
    } याव (0)

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
    पूर्णाङ्क
edit(
    पूर्णाङ्क		cmdchar,
    पूर्णाङ्क		startln,	// if set, insert at start of line
    long	गणनम्)
{
    पूर्णाङ्क		c = 0;
    char_u	*ptr;
    पूर्णाङ्क		lastc = 0;
    पूर्णाङ्क		mincol;
    स्थिर linenr_T o_lnum = 0;
    पूर्णाङ्क		i;
    पूर्णाङ्क		did_backspace = TRUE;	    // previous char was backspace
    पूर्णाङ्क		पङ्क्तिः_is_white = FALSE;	    // line is empty before insert
    linenr_T	old_topline = 0;	    // topline before insertion
#ifdef FEAT_DIFF
    पूर्णाङ्क		old_topfill = -1;
#endif
    पूर्णाङ्क		inserted_space = FALSE;     // just inserted a space
    पूर्णाङ्क		replaceState = MODE_REPLACE;
    पूर्णाङ्क		nomove = FALSE;		    // don't move cursor on return
#ifdef FEAT_JOB_CHANNEL
    पूर्णाङ्क		cmdchar_todo = cmdchar;
#endif
#ifdef FEAT_CONCEAL
    पूर्णाङ्क		cursor_पङ्क्तिः_was_concealed;
#endif
    पूर्णाङ्क		ins_just_started = TRUE;

    // Remember whether editing was restarted after CTRL-O.
    did_restart_edit = restart_edit;

    // sleep before redrawing, needed for "CTRL-O :" that results in an
    // error message
    check_for_delay(TRUE);

    // set Insstart_orig to Insstart
    update_Insstart_orig = TRUE;

#ifdef HAVE_SANDBOX
    // Don't allow inserting in the sandbox.
    सचे (sandbox != 0)
    {
	emsg(_(e_not_allowed_in_sandbox));
	पटिनिवत्त FALSE;
    }
#endif
    // Don't allow changes in the buffer while editing the cmdline.  The
    // caller of getcmdline() may get confused.
    // Don't allow recursive insert mode when busy with completion.
    सचे (textlock != 0 || ins_compl_active() || compl_busy || pum_visible())
    {
	emsg(_(e_not_allowed_to_change_text_or_change_window));
	पटिनिवत्त FALSE;
    }
    ins_compl_clear();	    // clear stuff for CTRL-X mode

    /*
     * Trigger InsertEnter autocommands.  Do not do this for "r<CR>" or "grx".
     */
    सचे (cmdchar != 'r' && cmdchar != 'v')
    {
	pos_T   save_cursor = curwin->w_cursor;

#ifdef FEAT_EVAL
	सचे (cmdchar == 'R')
	    ptr = (char_u *)"r";
	अञ्ञथा सचे (cmdchar == 'V')
	    ptr = (char_u *)"v";
	अञ्ञथा
	    ptr = (char_u *)"i";
	set_vim_var_string(VV_INSERTMODE, ptr, 1);
	set_vim_var_string(VV_CHAR, रिक्त, -1);  // clear v:char
#endif
	ins_apply_autocmds(EVENT_INSERTENTER);

	// Check for changed highlighting, e.g. for ModeMsg.
	सचे (need_highlight_changed)
	    highlight_changed();

	// Make sure the cursor didn't move.  Do call check_cursor_col() in
	// case the text was modified.  Since Insert mode was not started yet
	// a call to check_cursor_col() may move the cursor, especially with
	// the "A" command, thus set State to avoid that. Also check that the
	// line number is still valid (lines may have been deleted).
	// Do not restore if v:char was set to a non-empty string.
	सचे (!EQUAL_POS(curwin->w_cursor, save_cursor)
#ifdef FEAT_EVAL
		&& *get_vim_var_str(VV_CHAR) == NUL
#endif
		&& save_cursor.lnum <= curbuf->b_ml.ml_पङ्क्तिः_गणनम्)
	{
	    पूर्णाङ्क save_state = State;

	    curwin->w_cursor = save_cursor;
	    State = MODE_निवेशय;
	    check_cursor_col();
	    State = save_state;
	}
    }

#ifdef FEAT_CONCEAL
    // Check if the cursor line was concealed before changing State.
    cursor_पङ्क्तिः_was_concealed = curwin->w_p_cole > 0
						&& conceal_cursor_पङ्क्तिः(curwin);
#endif

    /*
     * When doing a paste with the middle mouse button, Insstart is set to
     * where the paste started.
     */
    सचे (where_paste_started.lnum != 0)
	Insstart = where_paste_started;
    अञ्ञथा
    {
	Insstart = curwin->w_cursor;
	सचे (startln)
	    Insstart.col = 0;
    }
    Insstart_textlen = (colnr_T)linetabsize_str(ml_get_curline());
    Insstart_blank_vcol = MAXCOL;
    सचे (!did_ai)
	ai_col = 0;

    सचे (cmdchar != NUL && restart_edit == 0)
    {
	ResetRedobuff();
	Append_सङ्ख्या_To_Redobuff(गणनम्);
	सचे (cmdchar == 'V' || cmdchar == 'v')
	{
	    // "gR" or "gr" command
	    AppendCharToRedobuff('g');
	    AppendCharToRedobuff((cmdchar == 'v') ? 'r' : 'R');
	}
	अञ्ञथा
	{
	    सचे (cmdchar == K_PS)
		AppendCharToRedobuff('a');
	    अञ्ञथा
		AppendCharToRedobuff(cmdchar);
	    सचे (cmdchar == 'g')		    // "gI" command
		AppendCharToRedobuff('I');
	    अञ्ञथा सचे (cmdchar == 'r')	    // "r<CR>" command
		गणनम् = 1;		    // insert only one <CR>
	}
    }

    सचे (cmdchar == 'R')
    {
	State = MODE_REPLACE;
    }
    अञ्ञथा सचे (cmdchar == 'V' || cmdchar == 'v')
    {
	State = MODE_VREPLACE;
	replaceState = MODE_VREPLACE;
	orig_पङ्क्तिः_गणनम् = curbuf->b_ml.ml_पङ्क्तिः_गणनम्;
	vr_पङ्क्तयः_changed = 1;
    }
    अञ्ञथा
	State = MODE_निवेशय;

    may_trigger_modechanged();
    stop_निवेशय_mode = FALSE;

#ifdef FEAT_CONCEAL
    // Check if the cursor line needs redrawing after changing State.  If
    // 'concealcursor' is "n" it needs to be redrawn without concealing.
    conceal_check_cursor_पङ्क्तिः(cursor_पङ्क्तिः_was_concealed);
#endif

    // Need to position cursor again when on a TAB and when on a char with
    // virtual text.
    सचे (gchar_cursor() == TAB
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
    सचे (curbuf->b_p_iminsert == B_IMODE_LMAP)
	State |= MODE_LANGMAP;
#ifdef HAVE_निवेशः_METHOD
    im_set_active(curbuf->b_p_iminsert == B_IMODE_IM);
#endif

    setmouse();
    clear_showcmd();
#ifdef FEAT_RIGHTLEFT
    // there is no reverse replace mode
    revins_on = (State == MODE_निवेशय && p_ri);
    सचे (revins_on)
	undisplay_dollar();
    revins_chars = 0;
    revins_legal = 0;
    revins_scol = -1;
#endif
    सचे (!p_ek)
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
    सचे (restart_edit != 0 && stuff_empty())
    {
	/*
	 * After a paste we consider text typed to be part of the insert for
	 * the pasted text. You can backspace over the pasted text too.
	 */
	सचे (where_paste_started.lnum)
	    arrow_used = FALSE;
	अञ्ञथा
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
	सचे (((ins_at_eol && curwin->w_cursor.lnum == o_lnum)
		    || curwin->w_curswant > curwin->w_virtcol)
		&& *(ptr = ml_get_curline() + curwin->w_cursor.col) != NUL)
	{
	    सचे (ptr[1] == NUL)
		++curwin->w_cursor.col;
	    अञ्ञथा सचे (has_mbyte)
	    {
		i = (*mb_ptr2len)(ptr);
		सचे (ptr[i] == NUL)
		    curwin->w_cursor.col += i;
	    }
	}
	ins_at_eol = FALSE;
    }
    अञ्ञथा
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
    सचे (!p_im && did_restart_edit == 0)
	foldOpenCursor();
#endif

    /*
     * If 'showmode' is set, show the current (insert/replace/..) mode.
     * A warning message for changing a readonly file is given here, before
     * actually changing anything.  It's put after the mode, if any.
     */
    i = 0;
    सचे (p_smd && सन्देशः_silent == 0)
	i = showmode();

    सचे (!p_im && did_restart_edit == 0)
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
    new_निवेशय_skip = (पूर्णाङ्क)inserted.length;
    सचे (inserted.string != रिक्त)
	vim_free(inserted.string);

    old_indent = 0;

    /*
     * Main loop in Insert mode: repeat until Insert mode is left.
     */
    अत्थाय (;;)
    {
#ifdef FEAT_RIGHTLEFT
	सचे (!revins_legal)
	    revins_scol = -1;	    // reset on illegal motions
	अञ्ञथा
	    revins_legal = 0;
#endif
	सचे (arrow_used)	    // don't repeat insert when arrow key used
	    गणनम् = 0;

	सचे (update_Insstart_orig)
	    Insstart_orig = Insstart;

	सचे (stop_निवेशय_mode && !ins_compl_active())
	{
	    // ":stopinsert" used or 'insertmode' reset
	    गणनम् = 0;
	    गच्छ doESCkey;
	}

	// set curwin->w_curswant for next K_DOWN or K_UP
	सचे (!arrow_used)
	    curwin->w_set_curswant = true;

	// If there is no typeahead may check for timestamps (e.g., for when a
	// menu invoked a shell command).
	सचे (stuff_empty())
	{
	    did_check_timestamps = FALSE;
	    सचे (need_check_timestamps)
		check_timestamps(FALSE);
	}

	/*
	 * When emsg() was called msg_scroll will have been set.
	 */
	सन्देशः_scroll = FALSE;

#ifdef FEAT_GUI
	// When 'mousefocus' is set a mouse movement may have taken us to
	// another window.  "need_mouse_correct" may then be set because of an
	// autocommand.
	सचे (need_mouse_correct)
	    gui_mouse_correct();
#endif

#ifdef FEAT_FOLDING
	// Open fold at the cursor line, according to 'foldopen'.
	सचे (fdo_flags & FDO_निवेशय)
	    foldOpenCursor();
	// Close folds where the cursor isn't, according to 'foldclose'
	सचे (!char_avail())
	    foldCheckClose();
#endif

#ifdef FEAT_JOB_CHANNEL
	सचे (bt_prompt(curbuf))
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
	सचे (curbuf->b_mod_set
		&& curwin->w_p_wrap
		&& !curwin->w_p_sms
		&& !did_backspace
		&& curwin->w_topline == old_topline
#ifdef FEAT_DIFF
		&& curwin->w_topfill == old_topfill
#endif
		&& गणनम् <= 1
		)
	{
	    mincol = curwin->w_wcol;
	    validate_cursor_col();

	    सचे (
#ifdef FEAT_VARTABS
		curwin->w_wcol < mincol - tabstop_at(get_nolist_virtcol(),
						     curbuf->b_p_ts,
						     curbuf->b_p_vts_array,
						     FALSE)
#अञ्ञथा
		(पूर्णाङ्क)curwin->w_wcol < mincol - curbuf->b_p_ts
#endif
		    && curwin->w_wrow ==
				   curwin->w_height - 1 - get_scrolloff_मानम्()
		    && (curwin->w_cursor.lnum != curwin->w_topline
#ifdef FEAT_DIFF
			|| curwin->w_topfill > 0
#endif
		    ))
	    {
#ifdef FEAT_DIFF
		सचे (curwin->w_topfill > 0)
		    --curwin->w_topfill;
		अञ्ञथा
#endif
#ifdef FEAT_FOLDING
		सचे (hasFolding(curwin->w_topline, रिक्त, &old_topline))
		    set_topline(curwin, old_topline + 1);
		अञ्ञथा
#endif
		    set_topline(curwin, curwin->w_topline + 1);
	    }
	}

	// May need to adjust w_topline to show the cursor.
	सचे (गणनम् <= 1)
	    update_topline();

	did_backspace = FALSE;

	सचे (गणनम् <= 1)
	    validate_cursor();		// may set must_redraw

	/*
	 * Redraw the display when no characters are waiting.
	 * Also shows mode, ruler and positions cursor.
	 */
	ins_redraw(TRUE);

	सचे (curwin->w_p_scb)
	    do_check_scrollbind(TRUE);

	सचे (curwin->w_p_crb)
	    do_check_cursorbind();
	सचे (गणनम् <= 1)
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
	सचे (c != K_CURSORHOLD && c != K_COMPLETE_DELAY)
	    lastc = c;		// remember the previous char for CTRL-D

	// After using CTRL-G U the next cursor key will not break undo.
	सचे (dont_sync_undo == MAYBE)
	    dont_sync_undo = TRUE;
	अञ्ञथा
	    dont_sync_undo = FALSE;
	सचे (cmdchar == K_PS)
	    // Got here from normal mode when bracketed paste started.
	    c = K_PS;
	अञ्ञथा
	{
	    // Trigger autocomplete when entering Insert mode, either directly
	    // or via change commands like 'ciw', 'cw', etc., before the first
	    // character is typed.
	    सचे (ins_just_started)
	    {
		ins_just_started = FALSE;
		सचे (ins_compl_has_autocomplete() && !char_avail()
			&& curwin->w_cursor.col > 0)
		{
		    c = char_before_cursor();
		    सचे (vim_isprintc(c))
		    {
			ins_compl_enable_autocomplete();
			ins_compl_arm_autostart();
			ins_compl_init_get_longest();
#ifdef FEAT_RIGHTLEFT
			सचे (p_hkmap)
			    c = hkmap(c);		// Hebrew mode mapping
#endif
			// Defer until the delay expires (K_COMPLETE_DELAY), or
			// trigger now when no delay is in effect.
			सचे (!ins_compl_arm_autocomplete_delay())
			    गच्छ docomplete;
		    }
		}
	    }

	    करोहि
	    {
		c = safe_vgetc();

		सचे (stop_निवेशय_mode
#ifdef FEAT_TERMINAL
			|| (c == K_IGNORE && term_use_loop())
#endif
		   )
		{
		    // Insert mode ended, possibly from a callback, or a timer
		    // must have opened a terminal window.
		    सचे (c != K_IGNORE && c != K_NOP)
			vungetc(c);
		    गणनम् = 0;

		    सचे (!bt_prompt(curwin->w_buffer)
#ifdef FEAT_TERMINAL
			    && !bt_terminal(curwin->w_buffer)
#endif
			    && stop_निवेशय_mode)
			// :stopinsert command via callback or via server command
			nomove = FALSE;
		    अञ्ञथा
			nomove = TRUE;
		    ins_compl_prep(ESC);
		    गच्छ doESCkey;
		}
	    } याव (c == K_IGNORE || c == K_NOP);
	}

	// Don't want K_CURSORHOLD for the second key, e.g., after CTRL-V.
	did_cursorhold = TRUE;
	सचे (c != K_CURSORHOLD && c != K_COMPLETE_DELAY)
	{
	    // Don't want delayed autocompletion from the previous key either.
	    ins_compl_clear_autocomplete_delay();
	    ins_compl_disarm_autostart();
	    // A completion already on screen goes on being what it was.
	    सचे (!ins_compl_active())
		ins_compl_disable_autocomplete();
	}

#ifdef FEAT_RIGHTLEFT
	सचे (p_hkmap && कुञ्चिका_Typed)
	    c = hkmap(c);		// Hebrew mode mapping
#endif

	// If the window was made so small that nothing shows, make it at least
	// one line and one column when typing.
	सचे (कुञ्चिका_Typed && !कुञ्चिका_Stuffed)
	    win_ensure_size();

	/*
	 * Special handling of keys while the popup menu is visible or wanted
	 * and the cursor is still in the completed word.  Only when there is
	 * a match, skip this when no matches were found.
	 */
	सचे (ins_compl_active() && curwin->w_cursor.col >= ins_compl_col()
		&& ins_compl_has_shown_match() && pum_wanted())
	{
	    // BS: Delete one character from "compl_leader".
	    सचे ((c == K_BS || c == Ctrl_H)
			&& curwin->w_cursor.col > ins_compl_col()
			&& (c = ins_compl_bs()) == NUL)
		अनुवर्त;

	    // When no match was selected or it was edited.
	    सचे (!ins_compl_used_match())
	    {
		// CTRL-L: Add one character from the current match to
		// "compl_leader".  Except when at the original match and
		// there is nothing to add, CTRL-L works like CTRL-P then.
		सचे (c == Ctrl_L
			&& (!ctrl_x_mode_पङ्क्तिः_or_eval()
			    || ins_compl_long_shown_match()))
		{
		    ins_compl_addfrommatch();
		    अनुवर्त;
		}

		// A non-white character that fits in with the current
		// completion: Add to "compl_leader".
		सचे (ins_compl_accept_char(c))
		{
#सचे defined(FEAT_EVAL)
		    // Trigger InsertCharPre.
		    char_u *str = do_निवेशय_char_pre(c);
		    char_u *p;

		    सचे (str != रिक्त)
		    {
			अत्थाय (p = str; *p != NUL; MB_PTR_ADV(p))
			    ins_compl_addleader(PTR2CHAR(p));
			vim_free(str);
		    }
		    अञ्ञथा
#endif
			ins_compl_addleader(c);
		    अनुवर्त;
		}

		// Pressing CTRL-Y selects the current match.  When
		// ins_compl_enter_selects() is set the Enter key does the
		// same.
		सचे ((c == Ctrl_Y || (ins_compl_enter_selects()
				    && (c == CAR || c == K_KENTER || c == NL)))
			&& stop_arrow() == OK)
		{
		    ins_compl_delete();
		    सचे (ins_compl_preinsert_longest()
			    && !ins_compl_is_match_selected())
		    {
			ins_compl_निवेशय(FALSE, TRUE);
			ins_compl_init_get_longest();
			अनुवर्त;
		    }
		    अञ्ञथा
			ins_compl_निवेशय(FALSE, FALSE);
		}
		// Delete preinserted text when typing special chars
		अञ्ञथा सचे (IS_WHITE_NL_OR_NUL(c) && ins_compl_preinsert_effect())
		    ins_compl_delete();
	    }
	}

	// Prepare for or stop CTRL-X mode.  This doesn't do completion, but
	// it does fix up the text when finishing completion.
	ins_compl_init_get_longest();
	सचे (ins_compl_prep(c))
	    अनुवर्त;

	// CTRL-\ CTRL-N goes to Normal mode,
	// CTRL-\ CTRL-G goes to mode selected with 'insertmode',
	// CTRL-\ CTRL-O is like CTRL-O but without moving the cursor.
	सचे (c == Ctrl_BSL)
	{
	    // may need to redraw when no more chars available now
	    ins_redraw(FALSE);
	    ++no_mapping;
	    ++allow_कुञ्चिकाः;
	    c = plain_vgetc();
	    --no_mapping;
	    --allow_कुञ्चिकाः;
	    सचे (c != Ctrl_N && c != Ctrl_G && c != Ctrl_O)
	    {
		// it's something else
		vungetc(c);
		c = Ctrl_BSL;
	    }
	    अञ्ञथा सचे (c == Ctrl_G && p_im)
		अनुवर्त;
	    अञ्ञथा
	    {
		सचे (c == Ctrl_O)
		{
		    ins_ctrl_o();
		    ins_at_eol = FALSE;	// cursor keeps its column
		    nomove = TRUE;
		}
		गणनम् = 0;
		गच्छ doESCkey;
	    }
	}

#ifdef FEAT_DIGRAPHS
	c = do_digraph(c);
#endif

	सचे ((c == Ctrl_V || c == Ctrl_Q) && ctrl_x_mode_cmdline())
	    गच्छ docomplete;
	सचे (c == Ctrl_V || c == Ctrl_Q)
	{
	    ins_ctrl_v();
	    c = Ctrl_V;	// pretend CTRL-V is last typed character
	    अनुवर्त;
	}

	सचे (cindent_on() && ctrl_x_mode_none())
	{
	    // A key name preceded by a bang means this key is not to be
	    // inserted.  Skip ahead to the re-indenting below.
	    // A key name preceded by a star means that indenting has to be
	    // done before inserting the key.
	    पङ्क्तिः_is_white = inindent(0);
	    सचे (in_cinkeys(c, '!', पङ्क्तिः_is_white))
		गच्छ force_cindent;
	    सचे (can_cindent && in_cinkeys(c, '*', पङ्क्तिः_is_white)
							&& stop_arrow() == OK)
		do_c_व्यञ्जकम्_indent();
	}

#ifdef FEAT_RIGHTLEFT
	सचे (curwin->w_p_rl)
	    विकल्प (c)
	    {
		पक्ष K_वामः:	c = K_दक्षिणः; भङ्ग;
		पक्ष K_S_वामः:	c = K_S_दक्षिणः; भङ्ग;
		पक्ष K_C_वामः:	c = K_C_दक्षिणः; भङ्ग;
		पक्ष K_दक्षिणः:	c = K_वामः; भङ्ग;
		पक्ष K_S_दक्षिणः: c = K_S_वामः; भङ्ग;
		पक्ष K_C_दक्षिणः: c = K_C_वामः; भङ्ग;
	    }
#endif

	/*
	 * If 'keymodel' contains "startsel", may start selection.  If it
	 * does, a CTRL-O and c will be stuffed, we need to get these
	 * characters.
	 */
	सचे (ins_start_select(c))
	    अनुवर्त;

	/*
	 * The big switch to handle a character in insert mode.
	 */
	विकल्प (c)
	{
	पक्ष ESC:	// End input mode
	    सचे (echeck_abbr(ESC + ABBR_OFF))
		भङ्ग;
	    // FALLTHROUGH

	पक्ष Ctrl_C:	// End input mode
	    सचे (c == Ctrl_C && cmdwin_type != 0)
	    {
		// Close the cmdline window.
		cmdwin_result = K_IGNORE;
		got_int = FALSE; // don't stop executing autocommands et al.
		nomove = TRUE;
		गच्छ doESCkey;
	    }
#ifdef FEAT_JOB_CHANNEL
	    सचे (c == Ctrl_C && bt_prompt(curbuf))
	    {
		सचे (invoke_prompt_interrupt())
		{
		    सचे (!bt_prompt(curbuf))
			// buffer changed to a non-prompt buffer, get out of
			// Insert mode
			गच्छ doESCkey;
		    भङ्ग;
		}
	    }
#endif

#ifdef UNIX
do_intr:
#endif
	    // when 'insertmode' set, and not halfway a mapping, don't leave
	    // Insert mode
	    सचे (goto_im())
	    {
		सचे (got_int)
		{
		    (शून्य)vgetc();		// flush all buffers
		    got_int = FALSE;
		}
		अञ्ञथा
		    vim_beep(BO_IM);
		भङ्ग;
	    }
doESCkey:
	    // Drop a pending autocomplete so it does not outlive Insert mode.
	    ins_compl_clear_autocomplete_delay();
	    /*
	     * This is the ONLY return from edit()!
	     */
	    // Always update o_lnum, so that a "CTRL-O ." that adds a line
	    // still puts the cursor back after the inserted text.
	    सचे (ins_at_eol && gchar_cursor() == NUL)
		o_lnum = curwin->w_cursor.lnum;

	    सचे (ins_esc(&गणनम्, cmdchar, nomove))
	    {
		// When CTRL-C was typed got_int will be set, with the result
		// that the autocommands won't be executed. When mapped got_int
		// is not set, but let's keep the behavior the same.
		सचे (cmdchar != 'r' && cmdchar != 'v' && c != Ctrl_C)
		    ins_apply_autocmds(EVENT_INSERTLEAVE);
		did_cursorhold = FALSE;

		// ins_redraw() triggers TextChangedI only when no characters
		// are in the typeahead buffer, so reset curbuf->b_last_changedtick only
		// if the TextChangedI was not blocked by char_avail() (e.g. using :norm!)
		// and the TextChangedI autocommand has been triggered.
		सचे (!char_avail() && curbuf->b_last_changedtick_i == CHANGEDTICK(curbuf))
		    curbuf->b_last_changedtick = CHANGEDTICK(curbuf);
		पटिनिवत्त (c == Ctrl_O);
	    }
	    अनुवर्त;

	पक्ष Ctrl_Z:	// suspend when 'insertmode' set
	    सचे (!p_im)
		गच्छ normalchar;	// insert CTRL-Z as normal char
	    do_cmdline_cmd((char_u *)"stop");
#ifdef CURSOR_SHAPE
	    ui_cursor_shape();		// may need to update cursor shape
#endif
	    अनुवर्त;

	पक्ष Ctrl_O:	// execute one command
#ifdef FEAT_COMPL_FUNC
	    सचे (ctrl_x_mode_omni())
		गच्छ docomplete;
#endif
	    सचे (echeck_abbr(Ctrl_O + ABBR_OFF))
		भङ्ग;
	    ins_ctrl_o();

	    // don't move the cursor left when 'virtualedit' has "onemore".
	    सचे (get_ve_flags() & VE_ONEMORE)
	    {
		ins_at_eol = FALSE;
		nomove = TRUE;
	    }
	    गणनम् = 0;
	    गच्छ doESCkey;

	पक्ष K_INS:	// toggle insert/replace mode
	पक्ष K_KINS:
	    ins_निवेशय(replaceState);
	    भङ्ग;

	पक्ष K_SELECT:	// end of Select mode mapping - ignore
	    भङ्ग;

	पक्ष K_HELP:	// Help key works like <ESC> <Help>
	पक्ष K_F1:
	पक्ष K_XF1:
	    stuffcharReadbuff(K_HELP);
	    सचे (p_im)
		need_start_insertmode = TRUE;
	    गच्छ doESCkey;

#ifdef FEAT_NETBEANS_INTG
	पक्ष K_F21:	// NetBeans command
	    ++no_mapping;		// don't map the next key hits
	    i = plain_vgetc();
	    --no_mapping;
	    netbeans_keycommand(i);
	    भङ्ग;
#endif

	पक्ष K_ZERO:	// Insert the previously inserted text.
	पक्ष NUL:
	पक्ष Ctrl_A:
	    // For ^@ the trailing ESC will end the insert, unless there is an
	    // error.
	    सचे (stuff_inserted(NUL, 1L, (c == Ctrl_A)) == FAIL
						   && c != Ctrl_A && !p_im)
		गच्छ doESCkey;		// quit insert mode
	    inserted_space = FALSE;
	    भङ्ग;

	पक्ष Ctrl_R:	// insert the contents of a register
	    सचे (ctrl_x_mode_register() && !ins_compl_active())
		गच्छ docomplete;
	    ins_reg();
	    auto_format(FALSE, TRUE);
	    inserted_space = FALSE;
	    भङ्ग;

	पक्ष Ctrl_G:	// commands starting with CTRL-G
	    ins_ctrl_g();
	    भङ्ग;

	पक्ष Ctrl_HAT:	// switch input mode and/or langmap
	    ins_ctrl_hat();
	    भङ्ग;

#ifdef FEAT_RIGHTLEFT
	पक्ष Ctrl__:	// switch between languages
	    सचे (!p_ari)
		गच्छ normalchar;
	    ins_ctrl_();
	    भङ्ग;
#endif

	पक्ष Ctrl_D:	// Make indent one shiftwidth smaller.
#सचे defined(FEAT_FIND_ID)
	    सचे (ctrl_x_mode_path_defines())
		गच्छ docomplete;
#endif
	    // FALLTHROUGH

	पक्ष Ctrl_T:	// Make indent one shiftwidth greater.
	    सचे (c == Ctrl_T && ctrl_x_mode_thesaurus())
	    {
		सचे (has_compl_option(FALSE))
		    गच्छ docomplete;
		भङ्ग;
	    }

	    ins_shift(c, lastc);
	    auto_format(FALSE, TRUE);
	    inserted_space = FALSE;
	    भङ्ग;

	पक्ष K_DEL:	// delete character under the cursor
	पक्ष K_KDEL:
	    ins_del();
	    auto_format(FALSE, TRUE);
	    भङ्ग;

	पक्ष K_BS:	// delete character before the cursor
	पक्ष K_S_BS:
	पक्ष Ctrl_H:
	    did_backspace = ins_bs(c, BACKSPACE_CHAR, &inserted_space);
	    auto_format(FALSE, TRUE);
	    सचे (did_backspace)
		MAY_TRIGGER_AUTOCOMPLETE(c);
	    भङ्ग;

	पक्ष Ctrl_W:	// delete word before the cursor
#ifdef FEAT_JOB_CHANNEL
	    सचे (bt_prompt(curbuf) && (mod_mask & MOD_MASK_SHIFT) == 0)
	    {
		// In a prompt window CTRL-W is used for window commands.
		// Use Shift-CTRL-W to delete a word.
		stuffcharReadbuff(Ctrl_W);
		restart_edit = 'A';
		nomove = TRUE;
		गणनम् = 0;
		गच्छ doESCkey;
	    }
#endif
	    did_backspace = ins_bs(c, BACKSPACE_WORD, &inserted_space);
	    auto_format(FALSE, TRUE);
	    सचे (did_backspace)
		MAY_TRIGGER_AUTOCOMPLETE(c);
	    भङ्ग;

	पक्ष Ctrl_U:	// delete all inserted text in current line
#ifdef FEAT_COMPL_FUNC
	    // CTRL-X CTRL-U completes with 'completefunc'.
	    सचे (ctrl_x_mode_function())
		गच्छ docomplete;
#endif
	    did_backspace = ins_bs(c, BACKSPACE_पङ्क्तिः, &inserted_space);
	    auto_format(FALSE, TRUE);
	    inserted_space = FALSE;
	    सचे (did_backspace)
		MAY_TRIGGER_AUTOCOMPLETE(c);
	    भङ्ग;

	पक्ष K_LEFTMOUSE:   // mouse keys
	पक्ष K_LEFTMOUSE_NM:
	पक्ष K_LEFTDRAG:
	पक्ष K_LEFTRELEASE:
	पक्ष K_LEFTRELEASE_NM:
	पक्ष K_MOUSEMOVE:
	पक्ष K_MIDDLEMOUSE:
	पक्ष K_MIDDLEDRAG:
	पक्ष K_MIDDLERELEASE:
	पक्ष K_RIGHTMOUSE:
	पक्ष K_RIGHTDRAG:
	पक्ष K_RIGHTRELEASE:
	पक्ष K_X1MOUSE:
	पक्ष K_X1DRAG:
	पक्ष K_X1RELEASE:
	पक्ष K_X2MOUSE:
	पक्ष K_X2DRAG:
	पक्ष K_X2RELEASE:
	    ins_mouse(c);
	    भङ्ग;

	पक्ष K_MOUSEDOWN: // Default action for scroll wheel up: scroll up
	    ins_mousescroll(MSCR_DOWN);
	    भङ्ग;

	पक्ष K_MOUSEUP:	// Default action for scroll wheel down: scroll down
	    ins_mousescroll(MSCR_UP);
	    भङ्ग;

	पक्ष K_MOUSELEFT: // Scroll wheel left
	    ins_mousescroll(MSCR_वामः);
	    भङ्ग;

	पक्ष K_MOUSERIGHT: // Scroll wheel right
	    ins_mousescroll(MSCR_दक्षिणः);
	    भङ्ग;

	पक्ष K_PS:
	    bracketed_paste(PASTE_निवेशय, FALSE, रिक्त);
	    सचे (cmdchar == K_PS)
		// invoked from normal mode, bail out
		गच्छ doESCkey;
	    भङ्ग;
	पक्ष K_PE:
	    // Got K_PE without K_PS, ignore.
	    भङ्ग;

#ifdef FEAT_GUI_TABLINE
	पक्ष K_TABLINE:
	पक्ष K_TABMENU:
	    ins_tabline(c);
	    भङ्ग;
#endif

	पक्ष K_IGNORE:	// Something mapped to nothing
	    भङ्ग;

	पक्ष K_COMMAND:		    // <Cmd>command<CR>
	पक्ष K_SCRIPT_COMMAND:	    // <ScriptCmd>command<CR>
	    {
		bufref_T    save_curbuf;
		varnumber_T tick = CHANGEDTICK(curbuf);

		set_bufref(&save_curbuf, curbuf);
		do_cmdkey_command(c, 0);

#ifdef FEAT_TERMINAL
		सचे (term_use_loop())
		    // Started a terminal that gets the input, exit Insert mode.
		    गच्छ doESCkey;
#endif
		सचे (curbuf->b_u_synced
			|| (bufref_valid(&save_curbuf)
			    && curbuf == save_curbuf.br_buf
			    && tick != CHANGEDTICK(curbuf)))
		{
		    // The command synced undo or changed this buffer.
		    // Save the cursor line before the next typed edit.
		    ins_need_undo = TRUE;
		}
	    }
	    भङ्ग;

	पक्ष K_CURSORHOLD:	// Didn't type something for a while.
	    ins_apply_autocmds(EVENT_CURSORHOLDI);
	    did_cursorhold = TRUE;
	    // If CTRL-G U was used apply it to the next typed key.
	    सचे (dont_sync_undo == TRUE)
		dont_sync_undo = MAYBE;
	    भङ्ग;

	पक्ष K_COMPLETE_DELAY:	// 'autocompletedelay' expired
	    // If CTRL-G U was used apply it to the next typed key.
	    सचे (dont_sync_undo == TRUE)
		dont_sync_undo = MAYBE;
	    ins_compl_clear_autocomplete_delay();
	    सचे (!ins_compl_has_autocomplete() || char_avail()
		    || curwin->w_cursor.col == 0)
		भङ्ग;
	    c = char_before_cursor();
	    सचे (!vim_isprintc(c))
		भङ्ग;
	    // The completion may have been cleared while waiting, so re-enable
	    // autocomplete to match a zero delay.
	    ins_compl_enable_autocomplete();
	    ins_compl_arm_autostart();
	    गच्छ docomplete;

#ifdef FEAT_GUI_MSWIN
	    // On MS-Windows ignore <M-F4>, we get it when closing the window
	    // was cancelled.
	पक्ष K_F4:
	    सचे (mod_mask != MOD_MASK_ALT)
		गच्छ normalchar;
	    भङ्ग;
#endif

#ifdef FEAT_GUI
	पक्ष K_VER_SCROLLBAR:
	    ins_scroll();
	    भङ्ग;

	पक्ष K_HOR_SCROLLBAR:
	    ins_horscroll();
	    भङ्ग;
#endif

	पक्ष K_HOME:	// <Home>
	पक्ष K_KHOME:
	पक्ष K_S_HOME:
	पक्ष K_C_HOME:
	    ins_home(c);
	    भङ्ग;

	पक्ष K_END:	// <End>
	पक्ष K_KEND:
	पक्ष K_S_END:
	पक्ष K_C_END:
	    ins_end(c);
	    भङ्ग;

	पक्ष K_वामः:	// <Left>
	    सचे (mod_mask & (MOD_MASK_SHIFT|MOD_MASK_CTRL))
		ins_s_वामः();
	    अञ्ञथा
		ins_वामः();
	    भङ्ग;

	पक्ष K_S_वामः:	// <S-Left>
	पक्ष K_C_वामः:
	    ins_s_वामः();
	    भङ्ग;

	पक्ष K_दक्षिणः:	// <Right>
	    सचे (mod_mask & (MOD_MASK_SHIFT|MOD_MASK_CTRL))
		ins_s_दक्षिणः();
	    अञ्ञथा
		ins_दक्षिणः();
	    भङ्ग;

	पक्ष K_S_दक्षिणः:	// <S-Right>
	पक्ष K_C_दक्षिणः:
	    ins_s_दक्षिणः();
	    भङ्ग;

	पक्ष K_UP:	// <Up>
	    सचे (pum_visible())
		गच्छ docomplete;
	    सचे (mod_mask & MOD_MASK_SHIFT)
		ins_pageup();
	    अञ्ञथा
		ins_up(FALSE);
	    भङ्ग;

	पक्ष K_S_UP:	// <S-Up>
	पक्ष K_PAGEUP:
	पक्ष K_KPAGEUP:
	    सचे (pum_visible())
	    {
#ifdef FEAT_PROP_POPUP
		// CTRL-SHIFT-<Up> scrolls the info popup up a line,
		// CTRL-SHIFT-<PageUp> a page.  Shift is folded into K_S_UP but
		// stays in mod_mask for PageUp, hence the asymmetric check.
		सचे (c == K_S_UP ? (mod_mask & MOD_MASK_CTRL)
			: ((mod_mask & MOD_MASK_CTRL)
			    && (mod_mask & MOD_MASK_SHIFT)))
		{
		    popup_scroll_info(-1, c != K_S_UP);
		    भङ्ग;
		}
#endif
		गच्छ docomplete;
	    }
	    ins_pageup();
	    भङ्ग;

	पक्ष K_DOWN:	// <Down>
	    सचे (pum_visible())
		गच्छ docomplete;
	    सचे (mod_mask & MOD_MASK_SHIFT)
		ins_pagedown();
	    अञ्ञथा
		ins_down(FALSE);
	    भङ्ग;

	पक्ष K_S_DOWN:	// <S-Down>
	पक्ष K_PAGEDOWN:
	पक्ष K_KPAGEDOWN:
	    सचे (pum_visible())
	    {
#ifdef FEAT_PROP_POPUP
		// CTRL-SHIFT-<Down>/<PageDown> scroll the info popup down.
		सचे (c == K_S_DOWN ? (mod_mask & MOD_MASK_CTRL)
			: ((mod_mask & MOD_MASK_CTRL)
			    && (mod_mask & MOD_MASK_SHIFT)))
		{
		    popup_scroll_info(1, c != K_S_DOWN);
		    भङ्ग;
		}
#endif
		गच्छ docomplete;
	    }
	    ins_pagedown();
	    भङ्ग;

#ifdef FEAT_DND
	पक्ष K_DROP:	// drag-n-drop event
	    ins_drop();
	    भङ्ग;
#endif

	पक्ष K_S_TAB:	// When not mapped, use like a normal TAB
	    c = TAB;
	    // FALLTHROUGH

	पक्ष TAB:	// TAB or Complete patterns along path
#सचे defined(FEAT_FIND_ID)
	    सचे (ctrl_x_mode_path_patterns())
		गच्छ docomplete;
#endif
	    inserted_space = FALSE;
	    सचे (ins_tab())
		गच्छ normalchar;	// insert TAB as a normal char
	    auto_format(FALSE, TRUE);
	    भङ्ग;

	पक्ष K_KENTER:	// <Enter>
	    c = CAR;
	    // FALLTHROUGH
	पक्ष CAR:
	पक्ष NL:
#सचे defined(FEAT_QUICKFIX)
	    // In a quickfix window a <CR> jumps to the error under the
	    // cursor.
	    सचे (bt_quickfix(curbuf) && c == CAR)
	    {
		सचे (curwin->w_llist_ref == रिक्त)    // quickfix window
		    do_cmdline_cmd((char_u *)".cc");
		अञ्ञथा				    // location list window
		    do_cmdline_cmd((char_u *)".ll");
		भङ्ग;
	    }
#endif
	    सचे (cmdwin_type != 0)
	    {
		// Execute the command in the cmdline window.
		cmdwin_result = CAR;
		गच्छ doESCkey;
	    }
#ifdef FEAT_JOB_CHANNEL
	    सचे (bt_prompt(curbuf))
	    {
		invoke_prompt_callback();
		सचे (!bt_prompt(curbuf))
		    // buffer changed to a non-prompt buffer, get out of
		    // Insert mode
		    गच्छ doESCkey;
		भङ्ग;
	    }
#endif
	    सचे (ins_eol(c) == FAIL && !p_im)
		गच्छ doESCkey;	    // out of memory
	    auto_format(FALSE, FALSE);
	    inserted_space = FALSE;
	    भङ्ग;

	पक्ष Ctrl_K:	    // digraph or keyword completion
	    सचे (ctrl_x_mode_dictionary())
	    {
		सचे (has_compl_option(TRUE))
		    गच्छ docomplete;
		भङ्ग;
	    }
#ifdef FEAT_DIGRAPHS
	    c = ins_digraph();
	    सचे (c == NUL)
		भङ्ग;
#endif
	    गच्छ normalchar;

	पक्ष Ctrl_X:	// Enter CTRL-X mode
	    ins_ctrl_x();
	    भङ्ग;

	पक्ष Ctrl_RSB:	// Tag name completion after ^X
	    सचे (!ctrl_x_mode_tags())
		गच्छ normalchar;
	    गच्छ docomplete;

	पक्ष Ctrl_F:	// File name completion after ^X
	    सचे (!ctrl_x_mode_files())
		गच्छ normalchar;
	    गच्छ docomplete;

	पक्ष 's':	// Spelling completion after ^X
	पक्ष Ctrl_S:
	    सचे (!ctrl_x_mode_spell())
		गच्छ normalchar;
	    गच्छ docomplete;

	पक्ष Ctrl_L:	// Whole line completion after ^X
	    सचे (!ctrl_x_mode_whole_पङ्क्तिः())
	    {
		// CTRL-L with 'insertmode' set: Leave Insert mode
		सचे (p_im)
		{
		    सचे (echeck_abbr(Ctrl_L + ABBR_OFF))
			भङ्ग;
		    गच्छ doESCkey;
		}
		गच्छ normalchar;
	    }
	    // FALLTHROUGH

	पक्ष Ctrl_P:	// Do previous/next pattern completion
	पक्ष Ctrl_N:
#ifdef FEAT_PROP_POPUP
	    // CTRL-SHIFT-P/N scroll the info popup one line.
	    सचे (pum_visible() && (mod_mask & MOD_MASK_SHIFT)
		    && (c == Ctrl_P || c == Ctrl_N))
	    {
		popup_scroll_info(c == Ctrl_P ? -1 : 1, false);
		भङ्ग;
	    }
#endif
	    // if 'complete' is empty then plain ^P is no longer special,
	    // but it is under other ^X modes
	    सचे (*curbuf->b_p_cpt == NUL
		    && (ctrl_x_mode_normal() || ctrl_x_mode_whole_पङ्क्तिः())
		    && !compl_status_local())
		गच्छ normalchar;

docomplete:
	    ins_compl_clear_autocomplete_delay();
	    compl_busy = TRUE;
#ifdef FEAT_FOLDING
	    disable_fold_update++;  // don't redraw folds here
#endif
	    सचे (ins_complete(c, TRUE) == FAIL)
		compl_status_clear();
#ifdef FEAT_FOLDING
	    disable_fold_update--;
#endif
	    compl_busy = FALSE;
	    can_si = may_do_si(); // allow smartindenting
	    भङ्ग;

	पक्ष Ctrl_Y:	// copy from previous line or scroll down
	पक्ष Ctrl_E:	// copy from next line	   or scroll up
	    c = ins_ctrl_ey(c);
	    भङ्ग;

	स्वभाव:
#ifdef UNIX
	    सचे (c == intr_char)		// special interrupt char
		गच्छ do_intr;
#endif

normalchar:
	    /*
	     * Insert a normal character.
	     */
#सचे defined(FEAT_EVAL)
	    सचे (!p_paste)
	    {
		// Trigger InsertCharPre.
		char_u *str = do_निवेशय_char_pre(c);
		char_u *p;

		सचे (str != रिक्त)
		{
		    सचे (*str != NUL && stop_arrow() != FAIL)
		    {
			// Insert the new value of v:char literally.
			अत्थाय (p = str; *p != NUL; MB_PTR_ADV(p))
			{
			    c = PTR2CHAR(p);
			    सचे (c == CAR || c == K_KENTER || c == NL)
				ins_eol(c);
			    अञ्ञथा
				ins_char(c);
			}
			AppendToRedobuffLit(str, -1);
		    }
		    vim_free(str);
		    c = NUL;
		}

		// If the new value is already inserted or an empty string
		// then don't insert any character.
		सचे (c == NUL)
		    भङ्ग;
	    }
#endif
	    // Try to perform smart-indenting.
	    ins_try_si(c);

	    सचे (c == ' ')
	    {
		inserted_space = TRUE;
		सचे (inindent(0))
		    can_cindent = FALSE;
		सचे (Insstart_blank_vcol == MAXCOL
			&& curwin->w_cursor.lnum == Insstart.lnum)
		    Insstart_blank_vcol = get_nolist_virtcol();
	    }

	    // Insert a normal character and check for abbreviations on a
	    // special character.  Let CTRL-] expand abbreviations without
	    // inserting it.
	    सचे (vim_iswordc(c) || (!echeck_abbr(
			// Add ABBR_OFF for characters above 0x100, this is
			// what check_abbr() expects.
				(has_mbyte && c >= 0x100) ? (c + ABBR_OFF) : c)
			&& c != Ctrl_RSB))
	    {
		निवेशय_special(c, FALSE, FALSE);
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
	    सचे (ins_compl_has_autocomplete() && !char_avail()
		    && vim_isprintc(c))
		TRIGGER_AUTOCOMPLETE();

	    भङ्ग;
	}   // end of switch (c)

	// If typed something may trigger CursorHoldI again; K_COMPLETE_DELAY is
	// injected, not typed.
	सचे (c != K_CURSORHOLD && c != K_COMPLETE_DELAY
#ifdef FEAT_COMPL_FUNC
		// but not in CTRL-X mode, a script can't restore the state
		&& ctrl_x_mode_normal()
#endif
	       )
	    did_cursorhold = FALSE;

	// Check if we need to cancel completion mode because the window
	// or tab page was changed
	सचे (ins_compl_active() && !ins_compl_win_active(curwin))
	    ins_compl_cancel();

	// If the cursor was moved we didn't just insert a space
	सचे (arrow_used)
	    inserted_space = FALSE;

	सचे (can_cindent && cindent_on() && ctrl_x_mode_normal())
	{
force_cindent:
	    /*
	     * Indent now if a key was typed that is in 'cinkeys'.
	     */
	    सचे (in_cinkeys(c, ' ', पङ्क्तिः_is_white))
	    {
		सचे (stop_arrow() == OK)
		    // re-indent the current line
		    do_c_व्यञ्जकम्_indent();
	    }
	}

    }	// for (;;)
    // NOTREACHED
}

    पूर्णाङ्क
ins_need_undo_get(शून्य)
{
    पटिनिवत्त ins_need_undo;
}

/*
 * Redraw for Insert mode.
 * This is postponed until getting the next character to make '$' in the 'cpo'
 * option work correctly.
 * Only redraw when there are no characters available.  This speeds up
 * inserting sequences of characters (e.g., for CTRL-R).
 */
    शून्य
ins_redraw(पूर्णाङ्क ready)	    // not busy with something
{
#ifdef FEAT_CONCEAL
    linenr_T	conceal_old_cursor_पङ्क्तिः = 0;
    linenr_T	conceal_new_cursor_पङ्क्तिः = 0;
    पूर्णाङ्क		conceal_update_पङ्क्तयः = FALSE;
#endif

    सचे (char_avail())
	पटिनिवत्त;

    // Trigger CursorMoved if the cursor moved.  Not when the popup menu is
    // visible, the command might delete it.
    सचे (ready && (has_cursormovedI()
#ifdef FEAT_PROP_POPUP
		|| popup_visible
#endif
#सचे defined(FEAT_CONCEAL)
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
	सचे (syntax_present(curwin) && must_redraw)
	    update_screen(0);
#endif
	सचे (has_cursormovedI())
	{
	    // Make sure curswant is correct, an autocommand may call
	    // getcurpos().
	    update_curswant();
	    ins_apply_autocmds(EVENT_CURSORMOVEDI);
	}
#ifdef FEAT_PROP_POPUP
	सचे (popup_visible)
	    popup_check_cursor_pos();
#endif
#ifdef FEAT_CONCEAL
	सचे (curwin->w_p_cole > 0)
	{
	    conceal_old_cursor_पङ्क्तिः = last_cursormoved.lnum;
	    conceal_new_cursor_पङ्क्तिः = curwin->w_cursor.lnum;
	    conceal_update_पङ्क्तयः = TRUE;
	}
#endif
	last_cursormoved = curwin->w_cursor;
    }

    // Trigger TextChangedI if b_changedtick_i differs.
    सचे (ready && has_textchangedI()
	    && curbuf->b_last_changedtick_i != CHANGEDTICK(curbuf)
	    && !pum_visible())
    {
	aco_save_T	aco;
	varnumber_T	tick = CHANGEDTICK(curbuf);

	// Save and restore curwin and curbuf, in case the autocmd changes
	// them.
	aucmd_prepbuf(&aco, curbuf);
	apply_autocmds(EVENT_TEXTCHANGEDI, रिक्त, रिक्त, FALSE, curbuf);
	aucmd_restbuf(&aco);
	curbuf->b_last_changedtick_i = CHANGEDTICK(curbuf);
	सचे (tick != CHANGEDTICK(curbuf))  // see ins_apply_autocmds()
	    u_save(curwin->w_cursor.lnum,
					(linenr_T)(curwin->w_cursor.lnum + 1));
    }

    // Trigger TextChangedP if b_changedtick_pum differs. When the popupmenu
    // closes TextChangedI will need to trigger for backwards compatibility,
    // thus use different b_last_changedtick* variables.
    सचे (ready && has_textchangedP()
	    && curbuf->b_last_changedtick_pum != CHANGEDTICK(curbuf)
	    && pum_visible())
    {
	aco_save_T	aco;
	varnumber_T	tick = CHANGEDTICK(curbuf);

	// Save and restore curwin and curbuf, in case the autocmd changes
	// them.
	aucmd_prepbuf(&aco, curbuf);
	apply_autocmds(EVENT_TEXTCHANGEDP, रिक्त, रिक्त, FALSE, curbuf);
	aucmd_restbuf(&aco);
	curbuf->b_last_changedtick_pum = CHANGEDTICK(curbuf);
	सचे (tick != CHANGEDTICK(curbuf))  // see ins_apply_autocmds()
	    u_save(curwin->w_cursor.lnum,
					(linenr_T)(curwin->w_cursor.lnum + 1));
    }

    सचे (ready)
	may_trigger_win_scrolled_resized();

    // Trigger SafeState if nothing is pending.
    may_trigger_safestate(ready
	    && !ins_compl_active()
	    && !pum_visible());

#सचे defined(FEAT_CONCEAL)
    सचे ((conceal_update_पङ्क्तयः
	    && (conceal_old_cursor_पङ्क्तिः != conceal_new_cursor_पङ्क्तिः
		|| conceal_cursor_पङ्क्तिः(curwin)))
	    || need_cursor_पङ्क्तिः_redraw)
    {
	सचे (conceal_old_cursor_पङ्क्तिः != conceal_new_cursor_पङ्क्तिः)
	    redrawWinline(curwin, conceal_old_cursor_पङ्क्तिः);
	redrawWinline(curwin, conceal_new_cursor_पङ्क्तिः == 0
			    ? curwin->w_cursor.lnum : conceal_new_cursor_पङ्क्तिः);
	curwin->w_valid &= ~VALID_CROW;
	need_cursor_पङ्क्तिः_redraw = FALSE;
    }
#endif
    सचे (must_redraw)
	update_screen(0);
    अञ्ञथा सचे (clear_cmdline || redraw_cmdline)
	showmode();		// clear cmdline and show mode
    showruler(FALSE);
    setcursor();
    emsg_on_display = FALSE;	// may remove error message now
}

/*
 * Handle a CTRL-V or CTRL-Q typed in Insert mode.
 */
    स्थिर शून्य
ins_ctrl_v(शून्य)
{
    पूर्णाङ्क		c;
    पूर्णाङ्क		did_putchar = FALSE;

    // may need to redraw when no more chars available now
    ins_redraw(FALSE);

    सचे (redrawing() && !char_avail())
    {
	edit_putchar('^', TRUE);
	did_putchar = TRUE;
    }
    AppendToRedobuff((char_u *)CTRL_V_STR);	// CTRL-V

    add_to_showcmd_c(Ctrl_V);

    // Do not change any modifyOtherKeys ESC sequence to a normal key for
    // CTRL-SHIFT-V.
    c = get_literal(mod_mask & MOD_MASK_SHIFT);
    सचे (did_putchar)
	// when the line fits in 'columns' the '^' is at the start of the next
	// line and will not removed by the redraw
	edit_unputchar();
    clear_showcmd();

    निवेशय_special(c, FALSE, TRUE);
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
    स्थिर पूर्णाङ्क
decode_Modify_Other_कुञ्चिकाः(पूर्णाङ्क c)
{
    char_u  *p = typebuf.tb_buf + typebuf.tb_off;
    पूर्णाङ्क	    idx;
    पूर्णाङ्क	    form = 0;
    पूर्णाङ्क	    argidx = 0;
    पूर्णाङ्क	    arg[2] = {0, 0};

    // Recognize:
    // form 0: {lead}{key};{modifier}u
    // form 1: {lead}27;{modifier};{key}~
    सचे (typebuf.tb_len >= 4 && (c == CSI || (c == ESC && *p == '[')))
    {
	idx = (*p == '[');
	सचे (p[idx] == '2' && p[idx + 1] == '7' && p[idx + 2] == ';' &&
		kitty_protocol_state != KKPS_ENABLED)
	{
	    form = 1;
	    idx += 3;
	}
	याव (idx < typebuf.tb_len && argidx < 2)
	{
	    सचे (p[idx] == ';')
		++argidx;
	    अञ्ञथा सचे (VIM_ISDIGIT(p[idx]))
		arg[argidx] = arg[argidx] * 10 + (p[idx] - '0');
	    अञ्ञथा
		भङ्ग;
	    ++idx;
	}
	पूर्णाङ्क kitty_no_mods = argidx == 0 && kitty_protocol_state == KKPS_ENABLED;
	सचे (idx < typebuf.tb_len
		&& p[idx] == (form == 1 ? '~' : 'u')
		&& (argidx == 1 || kitty_no_mods))
	{
	    // Match, consume the code.
	    typebuf.tb_off += idx + 1;
	    typebuf.tb_len -= idx + 1;
#सचे defined(FEAT_CLIENTSERVER) || defined(FEAT_EVAL)
	    सचे (typebuf.tb_len == 0)
		typebuf_was_filled = FALSE;
#endif

	    mod_mask = kitty_no_mods ? 0 : decode_modifiers(arg[!form]);
	    c = merge_modify_Other_कुञ्चिकाः(arg[form], &mod_mask);
	}
    }

    पटिनिवत्त c;
}

/*
 * Put a character directly onto the screen.  It's not stored in a buffer.
 * Used while handling CTRL-K, CTRL-V, etc. in Insert mode.
 */
स्थिर पूर्णाङ्क  pc_status;
#define PC_STATUS_UNSET	0	// pc_bytes was not set
#define PC_STATUS_दक्षिणः	1	// right half of double-wide char
#define PC_STATUS_वामः	2	// left half of double-wide char
#define PC_STATUS_SET	3	// pc_bytes was filled
स्थिर char_u pc_bytes[MB_MAXBYTES + 1]; // saved bytes
स्थिर पूर्णाङ्क  pc_attr;
स्थिर पूर्णाङ्क  pc_row;
स्थिर पूर्णाङ्क  pc_col;

    शून्य
edit_putchar(पूर्णाङ्क c, पूर्णाङ्क highlight)
{
    पूर्णाङ्क	    attr;

    सचे (Screen_पङ्क्तयः == रिक्त)
	पटिनिवत्त;

    update_topline();	// just in case w_topline isn't valid
    validate_cursor();
    सचे (highlight)
	attr = HL_ATTR(HLF_8);
    अञ्ञथा
	attr = 0;
    pc_row = W_WINROW(curwin) + curwin->w_wrow;
    pc_col = curwin->w_wincol;
    pc_status = PC_STATUS_UNSET;
#ifdef FEAT_RIGHTLEFT
    सचे (curwin->w_p_rl)
    {
	pc_col += curwin->w_width - 1 - curwin->w_wcol;
	सचे (has_mbyte)
	{
	    पूर्णाङ्क fix_col = mb_fix_col(pc_col, pc_row);

	    सचे (fix_col != pc_col)
	    {
		screen_putchar(' ', pc_row, fix_col, attr);
		--curwin->w_wcol;
		pc_status = PC_STATUS_दक्षिणः;
	    }
	}
    }
    अञ्ञथा
#endif
    {
	pc_col += curwin->w_wcol;
	सचे (mb_lefthalve(pc_row, pc_col))
	    pc_status = PC_STATUS_वामः;
    }

    // save the character to be able to put it back
    सचे (pc_status == PC_STATUS_UNSET)
    {
	screen_getbytes(pc_row, pc_col, pc_bytes, &pc_attr);
	pc_status = PC_STATUS_SET;
    }
    screen_putchar(c, pc_row, pc_col, attr);
}

#सचे defined(FEAT_JOB_CHANNEL)
/*
 * Set the insert start position for when using a prompt buffer.
 */
    शून्य
set_insstart(linenr_T lnum, पूर्णाङ्क col)
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
    शून्य
edit_unputchar(शून्य)
{
    सचे (pc_status != PC_STATUS_UNSET && pc_row >= सन्देशः_scrolled)
    {
	सचे (pc_status == PC_STATUS_दक्षिणः)
	    ++curwin->w_wcol;
	सचे (pc_status == PC_STATUS_दक्षिणः || pc_status == PC_STATUS_वामः)
	    redrawWinline(curwin, curwin->w_cursor.lnum);
	अञ्ञथा
	    screen_puts(pc_bytes, pc_row - सन्देशः_scrolled, pc_col, pc_attr);
    }
}

/*
 * Called when "$" is in 'cpoptions': display a '$' at the end of the changed
 * text.  Only works when cursor is in the line that changes.
 */
    शून्य
display_dollar(colnr_T col_arg)
{
    colnr_T col = col_arg < 0 ? 0 : col_arg;
    colnr_T save_col;

    सचे (!redrawing())
	पटिनिवत्त;

    cursor_off();
    save_col = curwin->w_cursor.col;
    curwin->w_cursor.col = col;
    सचे (has_mbyte)
    {
	char_u *p;

	// If on the last byte of a multi-byte move to the first byte.
	p = ml_get_curline();
	curwin->w_cursor.col -= (*mb_head_off)(p, p + col);
    }
    curs_columns(FALSE);	    // recompute w_wrow and w_wcol
    सचे (curwin->w_wcol < curwin->w_width)
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
    शून्य
undisplay_dollar(शून्य)
{
    सचे (dollar_vcol < 0)
	पटिनिवत्त;

    dollar_vcol = -1;
    redrawWinline(curwin, curwin->w_cursor.lnum);
}

/*
 * Truncate the space at the end of a line.  This is to be used only in an
 * insert mode.  It handles fixing the replace stack for MODE_REPLACE and
 * MODE_VREPLACE modes.
 */
    शून्य
truncate_spaces(char_u *पङ्क्तिः, size_t len)
{
    पूर्णाङ्क	    i;

    // find start of trailing white space
    अत्थाय (i = (पूर्णाङ्क)len - 1; i >= 0 && VIM_ISWHITE(पङ्क्तिः[i]); i--)
    {
	सचे (State & REPLACE_FLAG)
	    replace_join(0);	    // remove a NUL from the replace stack
    }
    पङ्क्तिः[i + 1] = NUL;
}

/*
 * Backspace the cursor until the given column.  Handles MODE_REPLACE and
 * MODE_VREPLACE modes correctly.  May also be used when not in insert mode at
 * all.  Will attempt not to go before "col" even when there is a composing
 * character.
 */
    शून्य
backspace_until_column(पूर्णाङ्क col)
{
    याव ((पूर्णाङ्क)curwin->w_cursor.col > col)
    {
	curwin->w_cursor.col--;
	सचे (State & REPLACE_FLAG)
	    replace_do_bs(col);
	अञ्ञथा सचे (!del_char_after_col(col))
	    भङ्ग;
    }
}

/*
 * Like del_char(), but make sure not to go before column "limit_col".
 * Only matters when there are composing characters.
 * Return TRUE when something was deleted.
 */
    स्थिर पूर्णाङ्क
del_char_after_col(पूर्णाङ्क limit_col UNUSED)
{
    सचे (enc_utf8 && limit_col >= 0)
    {
	colnr_T ecol = curwin->w_cursor.col + 1;

	// Make sure the cursor is at the start of a character, but
	// skip forward again when going too far back because of a
	// composing character.
	mb_adjust_cursor();
	याव (curwin->w_cursor.col < (colnr_T)limit_col)
	{
	    पूर्णाङ्क l = utf_ptr2len(ml_get_cursor());

	    सचे (l == 0)  // end of line
		भङ्ग;
	    curwin->w_cursor.col += l;
	}
	सचे (*ml_get_cursor() == NUL || curwin->w_cursor.col == ecol)
	    पटिनिवत्त FALSE;
	del_bytes((long)((पूर्णाङ्क)ecol - curwin->w_cursor.col), FALSE, TRUE);
    }
    अञ्ञथा
	(शून्य)del_char(FALSE);
    पटिनिवत्त TRUE;
}

/*
 * Next character is interpreted literally.
 * A one, two or three digit decimal number is interpreted as its byte value.
 * If one or two digits are entered, the next character is given to vungetc().
 * For Unicode a character > 255 may be returned.
 * If "noReduceKeys" is TRUE do not change any modifyOtherKeys ESC sequence
 * into a normal key, return ESC.
 */
    पूर्णाङ्क
get_literal(पूर्णाङ्क no_Reduce_कुञ्चिकाः)
{
    पूर्णाङ्क		cc;
    पूर्णाङ्क		nc;
    पूर्णाङ्क		i;
    पूर्णाङ्क		hex = FALSE;
    पूर्णाङ्क		octal = FALSE;
    पूर्णाङ्क		unicode = 0;

    सचे (got_int)
	पटिनिवत्त Ctrl_C;

#ifdef FEAT_GUI
    /*
     * In GUI there is no point inserting the internal code for a special key.
     * It is more useful to insert the string "<KEY>" instead.	This would
     * probably be useful in a text window too, but it would not be
     * vi-compatible (maybe there should be an option for it?) -- webb
     */
    सचे (gui.in_use)
    {
	++allow_कुञ्चिकाः;
	सचे (no_Reduce_कुञ्चिकाः)
	    ++no_reduce_कुञ्चिकाः;
    }
#endif
#ifdef USE_ON_FLY_SCROLL
    dont_scroll = TRUE;		// disallow scrolling here
#endif
    ++no_mapping;		// don't map the next key hits
    cc = 0;
    i = 0;
    अत्थाय (;;)
    {
	nc = plain_vgetc();
	सचे ((nc == ESC || nc == CSI) && !no_Reduce_कुञ्चिकाः)
	    nc = decode_Modify_Other_कुञ्चिकाः(nc);

	सचे ((mod_mask & ~MOD_MASK_SHIFT) != 0)
	    // A character with non-Shift modifiers should not be a valid
	    // character for i_CTRL-V_digit.
	    भङ्ग;

	सचे ((State & MODE_CMDLINE) == 0 && MB_BYTE2LEN_CHECK(nc) == 1)
	    add_to_showcmd(nc);
	सचे (nc == 'x' || nc == 'X')
	    hex = TRUE;
	अञ्ञथा सचे (nc == 'o' || nc == 'O')
	    octal = TRUE;
	अञ्ञथा सचे (nc == 'u' || nc == 'U')
	    unicode = nc;
	अञ्ञथा
	{
	    सचे (hex || unicode != 0)
	    {
		सचे (!vim_isxdigit(nc))
		    भङ्ग;
		cc = cc * 16 + hex2nr(nc);
	    }
	    अञ्ञथा सचे (octal)
	    {
		सचे (nc < '0' || nc > '7')
		    भङ्ग;
		cc = cc * 8 + nc - '0';
	    }
	    अञ्ञथा
	    {
		सचे (!VIM_ISDIGIT(nc))
		    भङ्ग;
		cc = cc * 10 + nc - '0';
	    }

	    ++i;
	}

	सचे (cc > 255 && unicode == 0)
	    cc = 255;		// limit range to 0-255
	nc = 0;

	सचे (hex)		// hex: up to two chars
	{
	    सचे (i >= 2)
		भङ्ग;
	}
	अञ्ञथा सचे (unicode)	// Unicode: up to four or eight chars
	{
	    सचे ((unicode == 'u' && i >= 4) || (unicode == 'U' && i >= 8))
		भङ्ग;
	}
	अञ्ञथा सचे (i >= 3)	// decimal or octal: up to three chars
	    भङ्ग;
    }
    सचे (i == 0)	    // no number entered
    {
	सचे (nc == K_ZERO)   // NUL is stored as NL
	{
	    cc = '\n';
	    nc = 0;
	}
	अञ्ञथा
	{
	    cc = nc;
	    nc = 0;
	}
    }

    सचे (cc == 0)	// NUL is stored as NL
	cc = '\n';
    सचे (enc_dbcs && (cc & 0xff) == 0)
	cc = '?';	// don't accept an illegal DBCS char, the NUL in the
			// second byte will cause trouble!

    --no_mapping;
#ifdef FEAT_GUI
    सचे (gui.in_use)
    {
	--allow_कुञ्चिकाः;
	सचे (no_Reduce_कुञ्चिकाः)
	    --no_reduce_कुञ्चिकाः;
    }
#endif
    सचे (nc)
    {
	vungetc(nc);
	// A character typed with i_CTRL-V_digit cannot have modifiers.
	mod_mask = 0;
    }
    got_int = FALSE;	    // CTRL-C typed after CTRL-V is not an interrupt
    पटिनिवत्त cc;
}

/*
 * Insert character, taking care of special keys and mod_mask
 */
    स्थिर शून्य
निवेशय_special(
    पूर्णाङ्क	    c,
    पूर्णाङ्क	    allow_modmask,
    पूर्णाङ्क	    ctrlv)	    // c was typed after CTRL-V
{
    char_u  *p;
    पूर्णाङ्क	    len;

    /*
     * Special function key, translate into "<Key>". Up to the last '>' is
     * inserted with ins_str(), so as not to replace characters in replace
     * mode.
     * Only use mod_mask for special keys, to avoid things like <S-Space>,
     * unless 'allow_modmask' is TRUE.
     */
#सचे defined(MACOS_X) || defined(FEAT_GUI_GTK)
    // Command-key never produces a normal key
    सचे (mod_mask & MOD_MASK_CMD)
	allow_modmask = TRUE;
#endif
    सचे (IS_SPECIAL(c) || (mod_mask && allow_modmask))
    {
	p = get_special_कुञ्चिका_name(c, mod_mask);
	len = (पूर्णाङ्क)STRLEN(p);
	c = p[len - 1];
	सचे (len > 2)
	{
	    सचे (stop_arrow() == FAIL)
		पटिनिवत्त;
	    p[len - 1] = NUL;
	    ins_str(p, len - 1);
	    AppendToRedobuffLit(p, -1);
	    ctrlv = FALSE;
	}
    }
    सचे (stop_arrow() == OK)
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
    शून्य
insertchar(
    पूर्णाङ्क		c,			// character to insert or NUL
    पूर्णाङ्क		flags,			// INSCHAR_FORMAT, etc.
    पूर्णाङ्क		second_indent)		// indent for second line if >= 0
{
    पूर्णाङ्क		textwidth;
    char_u	*p;
    पूर्णाङ्क		fo_ins_blank;
    पूर्णाङ्क		force_format = flags & INSCHAR_FORMAT;

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
    सचे (textwidth > 0
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
#सचे defined(FEAT_EVAL)
	पूर्णाङ्क     do_internal = TRUE;
	colnr_T virtcol = get_nolist_virtcol()
				  + char2cells(c != NUL ? c : gchar_cursor());

	सचे (*curbuf->b_p_fex != NUL && (flags & INSCHAR_NO_FEX) == 0
		&& (force_format || virtcol > (colnr_T)textwidth))
	{
	    do_internal = (fex_format(curwin->w_cursor.lnum, 1L, c) != 0);
	    // It may be required to save for undo again, e.g. when setline()
	    // was called.
	    ins_need_undo = TRUE;
	}
	सचे (do_internal)
#endif
	    internal_format(textwidth, second_indent, flags, c == NUL, c);
    }

    सचे (c == NUL)	    // only formatting was wanted
	पटिनिवत्त;

    // Check whether this character should end a comment.
    सचे (did_ai && c == end_comment_pending)
    {
	char_u  *पङ्क्तिः;
	char_u	lead_end[COM_MAX_LEN];	    // end-comment string
	पूर्णाङ्क	मध्यः_len, end_len;
	पूर्णाङ्क	i;

	/*
	 * Need to remove existing (middle) comment leader and insert end
	 * comment leader.  First, check what comment leader we can find.
	 */
	i = get_leader_len(पङ्क्तिः = ml_get_curline(), &p, FALSE, TRUE);
	सचे (i > 0 && vim_strchr(p, COM_मध्यः) != रिक्त)	// Just checking
	{
	    // Skip middle-comment string
	    याव (*p && p[-1] != ':')	// find end of middle flags
		++p;
	    मध्यः_len = copy_option_part(&p, lead_end, COM_MAX_LEN, ",");
	    // Don't count trailing white space for middle_len
	    याव (मध्यः_len > 0 && VIM_ISWHITE(lead_end[मध्यः_len - 1]))
		--मध्यः_len;

	    // Find the end-comment string
	    याव (*p && p[-1] != ':')	// find end of end flags
		++p;
	    end_len = copy_option_part(&p, lead_end, COM_MAX_LEN, ",");

	    // Skip white space before the cursor
	    i = curwin->w_cursor.col;
	    याव (--i >= 0 && VIM_ISWHITE(पङ्क्तिः[i]))
		;
	    i++;

	    // Skip to before the middle leader
	    i -= मध्यः_len;

	    // Check some expected things before we go on
	    सचे (i >= 0 && end_len > 0 && lead_end[end_len - 1] == end_comment_pending)
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

    सचे (       !ISSPECIAL(c)
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
#define निवेशः_BUFLEN 100
	char_u		buf[निवेशः_BUFLEN + 1];
	पूर्णाङ्क		i;
	colnr_T		virtcol = 0;

	buf[0] = c;
	i = 1;
	सचे (textwidth > 0)
	    virtcol = get_nolist_virtcol();
	/*
	 * Stop the string when:
	 * - no more chars available
	 * - finding a special character (command key)
	 * - buffer is full
	 * - running into the 'textwidth' boundary
	 * - need to check for abbreviation: A non-word char after a word-char
	 */
	याव (	   (c = vpeekc()) != NUL
		&& !ISSPECIAL(c)
		&& (!has_mbyte || MB_BYTE2LEN_CHECK(c) == 1)
		&& i < निवेशः_BUFLEN
		&& (textwidth == 0
		    || (virtcol += byte2cells(buf[i - 1])) < (colnr_T)textwidth)
		&& !(!no_abbr && !vim_iswordc(c) && vim_iswordc(buf[i - 1])))
	{
#ifdef FEAT_RIGHTLEFT
	    c = vgetc();
	    सचे (p_hkmap && कुञ्चिका_Typed)
		c = hkmap(c);		    // Hebrew mode mapping
	    buf[i++] = c;
#अञ्ञथा
	    buf[i++] = vgetc();
#endif
	}

#ifdef FEAT_DIGRAPHS
	do_digraph(-1);			// clear digraphs
	do_digraph(buf[i-1]);		// may be the start of a digraph
#endif
	buf[i] = NUL;
	ins_str(buf, i);
	सचे (flags & INSCHAR_CTRLV)
	{
	    redo_literal(*buf);
	    i = 1;
	}
	अञ्ञथा
	    i = 0;
	सचे (buf[i] != NUL)
	    AppendToRedobuffLit(buf + i, -1);
    }
    अञ्ञथा
    {
	पूर्णाङ्क		cc;

	सचे (has_mbyte && (cc = (*mb_char2len)(c)) > 1)
	{
	    char_u	buf[MB_MAXBYTES + 1];

	    (*mb_char2bytes)(c, buf);
	    buf[cc] = NUL;
	    ins_char_bytes(buf, cc);
	    AppendCharToRedobuff(c);
	}
	अञ्ञथा
	{
	    ins_char(c);
	    सचे (flags & INSCHAR_CTRLV)
		redo_literal(c);
	    अञ्ञथा
		AppendCharToRedobuff(c);
	}
    }
}

/*
 * Put a character in the redo buffer, for when just after a CTRL-V.
 */
    स्थिर शून्य
redo_literal(पूर्णाङ्क c)
{
    char_u	buf[10];

    // Only digits need special treatment.  Translate them into a string of
    // three digits.
    सचे (VIM_ISDIGIT(c))
    {
	vim_snprintf((वर्ण *)buf, परिमाण(buf), "%03d", c);
	AppendToRedobuff(buf);
    }
    अञ्ञथा
	AppendCharToRedobuff(c);
}

/*
 * start_arrow() is called when an arrow key is used in insert mode.
 * For undo/redo it resembles hitting the <ESC> key.
 */
    शून्य
start_arrow(
    pos_T    *end_निवेशय_pos)		// can be NULL
{
    start_arrow_common(end_निवेशय_pos, TRUE);
}

/*
 * Like start_arrow() but with end_change argument.
 * Will prepare for redo of CTRL-G U if "end_change" is FALSE.
 */
    स्थिर शून्य
start_arrow_with_change(
    pos_T    *end_निवेशय_pos,		// can be NULL
    पूर्णाङ्क	      end_change)		// end undoable change
{
    start_arrow_common(end_निवेशय_pos, end_change);
    सचे (!end_change)
    {
	AppendCharToRedobuff(Ctrl_G);
	AppendCharToRedobuff('U');
    }
}

    स्थिर शून्य
start_arrow_common(
    pos_T    *end_निवेशय_pos,		// can be NULL
    पूर्णाङ्क	      end_change)		// end undoable change
{
    सचे (!arrow_used && end_change)	// something has been inserted
    {
	AppendToRedobuff(ESC_STR);
	stop_निवेशय(end_निवेशय_pos, FALSE, FALSE);
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
    स्थिर शून्य
check_spell_redraw(शून्य)
{
    सचे (spell_redraw_lnum != 0)
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
    पूर्णाङ्क
stop_arrow(शून्य)
{
    सचे (arrow_used)
    {
	Insstart = curwin->w_cursor;	// new insertion starts here
	सचे (Insstart.col > Insstart_orig.col && !ins_need_undo)
	    // Don't update the original insert position when moved to the
	    // right, except when nothing was inserted yet.
	    update_Insstart_orig = FALSE;
	Insstart_textlen = (colnr_T)linetabsize_str(ml_get_curline());

	सचे (u_save_cursor() == OK)
	{
	    arrow_used = FALSE;
	    ins_need_undo = FALSE;
	}

	ai_col = 0;
	सचे (State & VREPLACE_FLAG)
	{
	    orig_पङ्क्तिः_गणनम् = curbuf->b_ml.ml_पङ्क्तिः_गणनम्;
	    vr_पङ्क्तयः_changed = 1;
	}
	ResetRedobuff();
	AppendToRedobuff((char_u *)"1i");   // pretend we start an insertion
	new_निवेशय_skip = 2;
    }
    अञ्ञथा सचे (ins_need_undo)
    {
	सचे (u_save_cursor() == OK)
	{
	    // A command or event may have moved the cursor before the next
	    // edit. Pull Insstart back only when the cursor moved above it,
	    // so that later edits can properly decide whether an extra undo
	    // entry is needed. Advancing Insstart would mis-place '[ after a
	    // register paste.
	    सचे (LT_POS(curwin->w_cursor, Insstart))
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

    पटिनिवत्त (arrow_used || ins_need_undo ? FAIL : OK);
}

/*
 * Do a few things to stop inserting.
 * "end_insert_pos" is where insert ended.  It is NULL when we already jumped
 * to another window/buffer.
 */
    स्थिर शून्य
stop_निवेशय(
    pos_T	*end_निवेशय_pos,
    पूर्णाङ्क		esc,			// called by ins_esc()
    पूर्णाङ्क		nomove)			// <c-\><c-o>, don't move cursor
{
    पूर्णाङ्क		cc;
    string_T	inserted;

    stop_redo_ins();
    replace_flush();		// abandon replace stack

    /*
     * Save the inserted text for later redo with ^@ and CTRL-A.
     * Don't do it when "restart_edit" was set and nothing was inserted,
     * otherwise CTRL-O w and then <Left> will clear "last_insert".
     */
    inserted = get_inserted();
    पूर्णाङ्क added = inserted.string == रिक्त ? 0 : (पूर्णाङ्क)inserted.length - new_निवेशय_skip;
    सचे (did_restart_edit == 0 || added > 0)
    {
	vim_free(last_निवेशय.string);
	last_निवेशय = inserted;			    // structure copy
	last_निवेशय_skip = added < 0 ? 0 : new_निवेशय_skip;
    }
    अञ्ञथा
	vim_free(inserted.string);

    सचे (!arrow_used && end_निवेशय_pos != रिक्त)
    {
	// Auto-format now.  It may seem strange to do this when stopping an
	// insertion (or moving the cursor), but it's required when appending
	// a line and having it end in a space.  But only do it when something
	// was actually inserted, otherwise undo won't work.
	सचे (!ins_need_undo && has_format_option(FO_AUTO))
	{
	    pos_T   tpos = curwin->w_cursor;

	    // When the cursor is at the end of the line after a space the
	    // formatting will move it to the following word.  Avoid that by
	    // moving the cursor onto the space.
	    cc = 'x';
	    सचे (curwin->w_cursor.col > 0 && gchar_cursor() == NUL)
	    {
		dec_cursor();
		cc = gchar_cursor();
		सचे (!VIM_ISWHITE(cc))
		    curwin->w_cursor = tpos;
	    }

	    auto_format(TRUE, FALSE);

	    सचे (VIM_ISWHITE(cc))
	    {
		सचे (gchar_cursor() != NUL)
		    inc_cursor();
		// If the cursor is still at the same character, also keep
		// the "coladd".
		सचे (gchar_cursor() == NUL
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
	सचे (!nomove && did_ai && (esc || (vim_strchr(p_cpo, CPO_INDENT) == रिक्त
			&& curwin->w_cursor.lnum != end_निवेशय_pos->lnum))
		&& end_निवेशय_pos->lnum <= curbuf->b_ml.ml_पङ्क्तिः_गणनम्)
	{
	    pos_T	tpos = curwin->w_cursor;
	    colnr_T	prev_col = end_निवेशय_pos->col;
	    colnr_T	strip_col;

	    curwin->w_cursor = *end_निवेशय_pos;
	    check_cursor_col();  // make sure it is not past the line

	    // Where the loop would actually start (back up if on NUL).
	    strip_col = curwin->w_cursor.col;
	    सचे (gchar_cursor() == NUL && strip_col > 0)
		--strip_col;

	    // Don't strip if non-whitespace follows: setline() from a <Cmd>
	    // mapping or CursorHoldI autocmd may have inserted content.
	    सचे (*skipwhite(ml_get_curline() + strip_col) == NUL)
	    {
		curwin->w_cursor.col = strip_col;
		अत्थाय (;;)
		{
		    सचे (gchar_cursor() == NUL && curwin->w_cursor.col > 0)
			--curwin->w_cursor.col;
		    cc = gchar_cursor();
		    सचे (!VIM_ISWHITE(cc))
			भङ्ग;
		    सचे (del_char(TRUE) == FAIL)
			भङ्ग;  // should not happen
		}
		सचे (curwin->w_cursor.lnum != tpos.lnum)
		    curwin->w_cursor = tpos;
		अञ्ञथा सचे (curwin->w_cursor.col < prev_col)
		{
		    // reset tpos, could have been invalidated in the loop above
		    tpos = curwin->w_cursor;
		    tpos.col++;
		    सचे (cc != NUL && gchar_pos(&tpos) == NUL)
			++curwin->w_cursor.col;	// put cursor back on the NUL
		}

		// <C-S-Right> may have started Visual mode, adjust the position for
		// deleted characters.
		सचे (VIsual_active)
		    check_visual_pos();
	    }
	    अञ्ञथा
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
    सचे (end_निवेशय_pos != रिक्त)
    {
	curbuf->b_op_start = Insstart;
	curbuf->b_op_start_orig = Insstart_orig;
	curbuf->b_op_end = *end_निवेशय_pos;
    }
}

/*
 * Set the last inserted text to a single character.
 * Used for the replace command.
 */
    शून्य
set_last_निवेशय(पूर्णाङ्क c)
{
    char_u	*s;

    vim_free(last_निवेशय.string);
    last_निवेशय.string = alloc(MB_MAXBYTES * 3 + 5);
    सचे (last_निवेशय.string == रिक्त)
    {
	last_निवेशय.length = 0;
	पटिनिवत्त;
    }

    s = last_निवेशय.string;
    // Use the CTRL-V only when entering a special char
    सचे (c < ' ' || c == DEL)
	*s++ = Ctrl_V;
    s = add_char2buf(c, s);
    *s++ = ESC;
    *s = NUL;
    last_निवेशय.length = (size_t)(s - last_निवेशय.string);
    last_निवेशय_skip = 0;
}

#सचे defined(EXITFREE)
    शून्य
free_last_निवेशय(शून्य)
{
    VIM_CLEAR_STRING(last_निवेशय);
}
#endif

/*
 * Add character "c" to buffer "s".  Escape the special meaning of K_SPECIAL
 * and CSI.  Handle multi-byte characters.
 * Returns a pointer to after the added bytes.
 */
    char_u *
add_char2buf(पूर्णाङ्क c, char_u *s)
{
    char_u	temp[MB_MAXBYTES + 1];
    पूर्णाङ्क		i;
    पूर्णाङ्क		len;

    len = (*mb_char2bytes)(c, temp);
    अत्थाय (i = 0; i < len; ++i)
    {
	c = temp[i];
	// Need to escape K_SPECIAL and CSI like in the typeahead buffer.
	सचे (c == K_SPECIAL)
	{
	    *s++ = K_SPECIAL;
	    *s++ = KS_SPECIAL;
	    *s++ = KE_FILLER;
	}
#ifdef FEAT_GUI
	अञ्ञथा सचे (c == CSI)
	{
	    *s++ = CSI;
	    *s++ = KS_EXTRA;
	    *s++ = (पूर्णाङ्क)KE_CSI;
	}
#endif
	अञ्ञथा
	    *s++ = c;
    }
    पटिनिवत्त s;
}

/*
 * move cursor to start of line
 * if flags & BL_WHITE	move to first non-white
 * if flags & BL_SOL	move to first non-white if startofline is set,
 *			    otherwise keep "curswant" column
 * if flags & BL_FIX	don't leave the cursor on a NUL.
 */
    शून्य
beginline(पूर्णाङ्क flags)
{
    सचे ((flags & BL_SOL) && !p_sol)
	coladvance(curwin->w_curswant);
    अञ्ञथा
    {
	curwin->w_cursor.col = 0;
	curwin->w_cursor.coladd = 0;

	सचे (flags & (BL_WHITE | BL_SOL))
	{
	    char_u  *ptr;

	    अत्थाय (ptr = ml_get_curline(); VIM_ISWHITE(*ptr)
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

    पूर्णाङ्क
oneright(शून्य)
{
    char_u	*ptr;
    पूर्णाङ्क		l;

    सचे (virtual_active())
    {
	pos_T	prevpos = curwin->w_cursor;

	// Adjust for multi-wide char (excluding TAB)
	ptr = ml_get_cursor();
	coladvance(getviscol() + ((*ptr != TAB
					  && vim_isprintc((*mb_ptr2char)(ptr)))
		    ? ptr2cells(ptr) : 1));
	curwin->w_set_curswant = true;
	// Return OK if the cursor moved, FAIL otherwise (at window edge).
	पटिनिवत्त (prevpos.col != curwin->w_cursor.col
		    || prevpos.coladd != curwin->w_cursor.coladd) ? OK : FAIL;
    }

    ptr = ml_get_cursor();
    सचे (*ptr == NUL)
	पटिनिवत्त FAIL;	    // already at the very end

    सचे (has_mbyte)
	l = (*mb_ptr2len)(ptr);
    अञ्ञथा
	l = 1;

    // move "l" bytes right, but don't end up on the NUL, unless 'virtualedit'
    // contains "onemore".
    सचे (ptr[l] == NUL && (get_ve_flags() & VE_ONEMORE) == 0)
	पटिनिवत्त FAIL;
    curwin->w_cursor.col += l;

    curwin->w_set_curswant = true;
    adjust_skipcol();
    पटिनिवत्त OK;
}

    पूर्णाङ्क
oneleft(शून्य)
{
    सचे (virtual_active())
    {
#ifdef FEAT_LINEBREAK
	पूर्णाङ्क width;
#endif
	पूर्णाङ्क v = getviscol();

	सचे (v == 0)
	    पटिनिवत्त FAIL;

#ifdef FEAT_LINEBREAK
	// We might get stuck on 'showbreak', skip over it.
	width = 1;
	अत्थाय (;;)
	{
	    coladvance(v - width);
	    // getviscol() is slow, skip it when 'showbreak' is empty,
	    // 'breakindent' is not set and there are no multi-byte
	    // characters
	    सचे ((*get_showbreak_मानम्(curwin) == NUL && !curwin->w_p_bri
					     && !has_mbyte) || getviscol() < v)
		भङ्ग;
	    ++width;
	}
#अञ्ञथा
	coladvance(v - 1);
#endif

	सचे (curwin->w_cursor.coladd == 1)
	{
	    char_u *ptr;

	    // Adjust for multi-wide char (not a TAB)
	    ptr = ml_get_cursor();
	    सचे (*ptr != TAB && vim_isprintc((*mb_ptr2char)(ptr))
							 && ptr2cells(ptr) > 1)
		curwin->w_cursor.coladd = 0;
	}

	curwin->w_set_curswant = true;
	adjust_skipcol();
	पटिनिवत्त OK;
    }

    सचे (curwin->w_cursor.col == 0)
	पटिनिवत्त FAIL;

    curwin->w_set_curswant = true;
    --curwin->w_cursor.col;

    // if the character on the left of the current cursor is a multi-byte
    // character, move to its first byte
    सचे (has_mbyte)
	mb_adjust_cursor();
    adjust_skipcol();
    पटिनिवत्त OK;
}

/*
 * Move the cursor up "n" lines in window "wp".
 * Takes care of closed folds.
 */
    शून्य
cursor_up_inner(win_T *wp, long n)
{
    linenr_T	lnum = wp->w_cursor.lnum;

    सचे (n >= lnum)
	lnum = 1;
    अञ्ञथा
#ifdef FEAT_FOLDING
	सचे (hasAnyFolding(wp))
    {
	/*
	 * Count each sequence of folded lines as one logical line.
	 */
	// go to the start of the current fold
	(शून्य)hasFoldingWin(wp, lnum, &lnum, रिक्त, TRUE, रिक्त);

	याव (n--)
	{
	    // move up one line
	    --lnum;
	    सचे (lnum <= 1)
		भङ्ग;
	    // If we entered a fold, move to the beginning, unless in
	    // Insert mode or when 'foldopen' contains "all": it will open
	    // in a moment.
	    सचे (n > 0 || !((State & MODE_निवेशय) || (fdo_flags & FDO_ALL)))
		(शून्य)hasFoldingWin(wp, lnum, &lnum, रिक्त, TRUE, रिक्त);
	}
	सचे (lnum < 1)
	    lnum = 1;
    }
    अञ्ञथा
#endif
	lnum -= n;

    wp->w_cursor.lnum = lnum;
}

    पूर्णाङ्क
cursor_up(
    long	n,
    पूर्णाङ्क		upd_topline)	    // When TRUE: update topline
{
    // This fails if the cursor is already in the first line or the count is
    // larger than the line number and '-' is in 'cpoptions'
    linenr_T lnum = curwin->w_cursor.lnum;
    सचे (n > 0 && (lnum <= 1
		       || (n >= lnum && vim_strchr(p_cpo, CPO_MINUS) != रिक्त)))
	पटिनिवत्त FAIL;
    cursor_up_inner(curwin, n);

    // try to advance to the column we want to be at
    coladvance(curwin->w_curswant);

    सचे (upd_topline)
	update_topline();	// make sure curwin->w_topline is valid

    पटिनिवत्त OK;
}

/*
 * Move the cursor down "n" lines in window "wp".
 * Takes care of closed folds.
 */
    शून्य
cursor_down_inner(win_T *wp, long n)
{
    linenr_T	lnum = wp->w_cursor.lnum;
    linenr_T	पङ्क्तिः_गणनम् = wp->w_buffer->b_ml.ml_पङ्क्तिः_गणनम्;

    सचे (lnum + n >= पङ्क्तिः_गणनम्)
	lnum = पङ्क्तिः_गणनम्;
    अञ्ञथा
#ifdef FEAT_FOLDING
	सचे (hasAnyFolding(wp))
    {
	linenr_T	last;

	// count each sequence of folded lines as one logical line
	याव (n--)
	{
	    सचे (hasFoldingWin(wp, lnum, रिक्त, &last, TRUE, रिक्त))
		lnum = last + 1;
	    अञ्ञथा
		++lnum;
	    सचे (lnum >= पङ्क्तिः_गणनम्)
		भङ्ग;
	}
	सचे (lnum > पङ्क्तिः_गणनम्)
	    lnum = पङ्क्तिः_गणनम्;
    }
    अञ्ञथा
#endif
	lnum += n;

    wp->w_cursor.lnum = lnum;
}

/*
 * Cursor down a number of logical lines.
 */
    पूर्णाङ्क
cursor_down(
    long	n,
    पूर्णाङ्क		upd_topline)	    // When TRUE: update topline
{
    linenr_T	lnum = curwin->w_cursor.lnum;
    linenr_T	पङ्क्तिः_गणनम् = curwin->w_buffer->b_ml.ml_पङ्क्तिः_गणनम्;
    // This fails if the cursor is already in the last (folded) line, or would
    // move beyond the last line and '-' is in 'cpoptions'.
#ifdef FEAT_FOLDING
    hasFoldingWin(curwin, lnum, रिक्त, &lnum, TRUE, रिक्त);
#endif
    सचे (n > 0
	    && (lnum >= पङ्क्तिः_गणनम्
		|| (lnum + n > पङ्क्तिः_गणनम्
				     && vim_strchr(p_cpo, CPO_MINUS) != रिक्त)))
	पटिनिवत्त FAIL;
    cursor_down_inner(curwin, n);

    // try to advance to the column we want to be at
    coladvance(curwin->w_curswant);

    सचे (upd_topline)
	update_topline();	// make sure curwin->w_topline is valid

    पटिनिवत्त OK;
}

/*
 * Stuff the last inserted text in the read buffer.
 * Last_insert actually is a copy of the redo buffer, so we
 * first have to remove the command.
 */
    पूर्णाङ्क
stuff_inserted(
    पूर्णाङ्क	    c,		// Command character to be inserted
    long    गणनम्,	// Repeat this many times
    पूर्णाङ्क	    no_esc)	// Don't add an ESC at the end
{
    string_T	निवेशय;				// text to be inserted
    char_u	last = ' ';

    निवेशय = get_last_निवेशय();
    सचे (निवेशय.string == रिक्त)
    {
	emsg(_(e_no_inserted_text_yet));
	पटिनिवत्त FAIL;
    }

    // may want to stuff the command character, to start Insert mode
    सचे (c != NUL)
	stuffcharReadbuff(c);

    सचे (निवेशय.length > 0)
    {
	char_u	*p;

	// look for the last ESC in 'insert'
	अत्थाय (p = निवेशय.string + निवेशय.length - 1; p >= निवेशय.string; --p)
	{
	    सचे (*p == ESC)
	    {
		निवेशय.length = (size_t)(p - निवेशय.string);
		भङ्ग;
	    }
	}
    }

    सचे (निवेशय.length > 0)
    {
	char_u	*p = निवेशय.string + निवेशय.length - 1;

	// when the last char is either "0" or "^" it will be quoted if no ESC
	// comes after it OR if it will insert more than once and "ptr"
	// starts with ^D.	-- Acevedo
	सचे ((*p == '0' || *p == '^')
		&& (no_esc || (*निवेशय.string == Ctrl_D && गणनम् > 1)))
	{
	    last = *p;
	    --निवेशय.length;
	}
    }

    करोहि
    {
	stuffReadbuffLen(निवेशय.string, (long)निवेशय.length);
	// a trailing "0" is inserted as "<C-V>048", "^" as "<C-V>^"
	विकल्प (last)
	{
	पक्ष '0':
#define TEXT_TO_निवेशय "\026\060\064\070"
	    stuffReadbuffLen((char_u *)TEXT_TO_निवेशय, STRLEN_LITERAL(TEXT_TO_निवेशय));
#undef TEXT_TO_निवेशय
	    भङ्ग;

	पक्ष '^':
#define TEXT_TO_निवेशय "\026^"
	    stuffReadbuffLen((char_u *)TEXT_TO_निवेशय, STRLEN_LITERAL(TEXT_TO_निवेशय));
#undef TEXT_TO_निवेशय
	    भङ्ग;

	स्वभाव:
	    भङ्ग;
	}
    }
    याव (--गणनम् > 0);

    // may want to stuff a trailing ESC, to get out of Insert mode
    सचे (!no_esc)
	stuffcharReadbuff(ESC);

    पटिनिवत्त OK;
}

    string_T
get_last_निवेशय(शून्य)
{
    string_T निवेशय = {रिक्त, 0};

    सचे (last_निवेशय.string != रिक्त)
    {
	निवेशय.string = last_निवेशय.string + last_निवेशय_skip;
	निवेशय.length = (size_t)(last_निवेशय.length - last_निवेशय_skip);
    }

    पटिनिवत्त निवेशय;
}

/*
 * Get last inserted string, and remove trailing <Esc>.
 * Returns pointer to allocated memory (must be freed) or NULL.
 */
    char_u *
get_last_निवेशय_save(शून्य)
{
    string_T	निवेशय = get_last_निवेशय();
    char_u	*s;

    सचे (निवेशय.string == रिक्त)
	पटिनिवत्त रिक्त;
    s = vim_strnsave(निवेशय.string, निवेशय.length);
    सचे (s == रिक्त)
	पटिनिवत्त रिक्त;

    सचे (निवेशय.length > 0 && s[निवेशय.length - 1] == ESC)	// remove trailing ESC
	s[--निवेशय.length] = NUL;
    पटिनिवत्त s;
}

/*
 * Check the word in front of the cursor for an abbreviation.
 * Called when the non-id character "c" has been entered.
 * When an abbreviation is recognized it is removed from the text and
 * the replacement string is inserted in typebuf.tb_buf[], followed by "c".
 */
    स्थिर पूर्णाङ्क
echeck_abbr(पूर्णाङ्क c)
{
    // Don't check for abbreviation in paste mode, when disabled and just
    // after moving around with cursor keys.
    सचे (p_paste || no_abbr || arrow_used)
	पटिनिवत्त FALSE;

    पटिनिवत्त check_abbr(c, ml_get_curline(), curwin->w_cursor.col,
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

स्थिर char_u	*replace_राशिः = रिक्त;
स्थिर long	replace_राशिः_nr = 0;	    // next entry in replace stack
स्थिर long	replace_राशिः_len = 0;	    // max. number of entries

    शून्य
replace_प्रक्षिप(
    पूर्णाङ्क	    c)	    // character that is replaced (NUL is none)
{
    char_u  *p;

    सचे (replace_राशिः_nr < replace_offset)	// nothing to do
	पटिनिवत्त;
    सचे (replace_राशिः_len <= replace_राशिः_nr)
    {
	replace_राशिः_len += 50;
	p = ALLOC_MULT(char_u, replace_राशिः_len);
	सचे (p == रिक्त)	    // out of memory
	{
	    replace_राशिः_len -= 50;
	    पटिनिवत्त;
	}
	सचे (replace_राशिः != रिक्त)
	{
	    mch_memmove(p, replace_राशिः,
				 (size_t)(replace_राशिः_nr * परिमाण(char_u)));
	    vim_free(replace_राशिः);
	}
	replace_राशिः = p;
    }
    p = replace_राशिः + replace_राशिः_nr - replace_offset;
    सचे (replace_offset)
	mch_memmove(p + 1, p, (size_t)(replace_offset * परिमाण(char_u)));
    *p = c;
    ++replace_राशिः_nr;
}

/*
 * Push a character onto the replace stack.  Handles a multi-byte character in
 * reverse byte order, so that the first byte is popped off first.
 * Return the number of bytes done (includes composing characters).
 */
    पूर्णाङ्क
replace_प्रक्षिप_mb(char_u *p)
{
    पूर्णाङ्क l = (*mb_ptr2len)(p);
    पूर्णाङ्क j;

    अत्थाय (j = l - 1; j >= 0; --j)
	replace_प्रक्षिप(p[j]);
    पटिनिवत्त l;
}

/*
 * Pop one item from the replace stack.
 * return -1 if stack empty
 * return replaced character or NUL otherwise
 */
    स्थिर पूर्णाङ्क
replace_निष्कासय(शून्य)
{
    सचे (replace_राशिः_nr == 0)
	पटिनिवत्त -1;
    पटिनिवत्त (पूर्णाङ्क)replace_राशिः[--replace_राशिः_nr];
}

/*
 * Join the top two items on the replace stack.  This removes to "off"'th NUL
 * encountered.
 */
    शून्य
replace_join(
    पूर्णाङ्क	    off)	// offset for which NUL to remove
{
    पूर्णाङ्क	    i;

    अत्थाय (i = replace_राशिः_nr; --i >= 0; )
	सचे (replace_राशिः[i] == NUL && off-- <= 0)
	{
	    --replace_राशिः_nr;
	    mch_memmove(replace_राशिः + i, replace_राशिः + i + 1,
					      (size_t)(replace_राशिः_nr - i));
	    पटिनिवत्त;
	}
}

/*
 * Pop bytes from the replace stack until a NUL is found, and insert them
 * before the cursor.  Can only be used in MODE_REPLACE or MODE_VREPLACE state.
 */
    स्थिर शून्य
replace_निष्कासय_ins(शून्य)
{
    पूर्णाङ्क	    cc;
    पूर्णाङ्क	    oldState = State;

    State = MODE_NORMAL;			// don't want MODE_REPLACE here
    याव ((cc = replace_निष्कासय()) > 0)
    {
	mb_replace_निष्कासय_ins(cc);
	dec_cursor();
    }
    State = oldState;
}

/*
 * Insert bytes popped from the replace stack. "cc" is the first byte.  If it
 * indicates a multi-byte char, pop the other bytes too.
 */
    स्थिर शून्य
mb_replace_निष्कासय_ins(पूर्णाङ्क cc)
{
    पूर्णाङ्क		n;
    char_u	buf[MB_MAXBYTES + 1];
    पूर्णाङ्क		i;
    पूर्णाङ्क		c;

    सचे (has_mbyte && (n = MB_BYTE2LEN(cc)) > 1)
    {
	buf[0] = cc;
	अत्थाय (i = 1; i < n; ++i)
	    buf[i] = replace_निष्कासय();
	ins_bytes_len(buf, n);
    }
    अञ्ञथा
	ins_char(cc);

    सचे (enc_utf8)
	// Handle composing chars.
	अत्थाय (;;)
	{
	    c = replace_निष्कासय();
	    सचे (c == -1)	    // stack empty
		भङ्ग;
	    सचे ((n = MB_BYTE2LEN(c)) == 1)
	    {
		// Not a multi-byte char, put it back.
		replace_प्रक्षिप(c);
		भङ्ग;
	    }

	    buf[0] = c;
	    अत्थाय (i = 1; i < n; ++i)
		buf[i] = replace_निष्कासय();
	    सचे (utf_iscomposing(utf_ptr2char(buf)))
		ins_bytes_len(buf, n);
	    अञ्ञथा
	    {
		// Not a composing char, put it back.
		अत्थाय (i = n - 1; i >= 0; --i)
		    replace_प्रक्षिप(buf[i]);
		भङ्ग;
	    }

	}
}

/*
 * make the replace stack empty
 * (called when exiting replace mode)
 */
    स्थिर शून्य
replace_flush(शून्य)
{
    VIM_CLEAR(replace_राशिः);
    replace_राशिः_len = 0;
    replace_राशिः_nr = 0;
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
    स्थिर शून्य
replace_do_bs(पूर्णाङ्क limit_col)
{
    पूर्णाङ्क		cc;
    पूर्णाङ्क		orig_len = 0;
    पूर्णाङ्क		ins_len;
    पूर्णाङ्क		orig_vcols = 0;
    colnr_T	start_vcol;
    char_u	*p;
    पूर्णाङ्क		i;
    पूर्णाङ्क		vcol;

    cc = replace_निष्कासय();
    सचे (cc > 0)
    {
#ifdef FEAT_PROP_POPUP
	size_t	len_before = 0;  // init to shut up GCC

	सचे (curbuf->b_has_textprop)
	{
	    // Do not adjust text properties for individual delete and insert
	    // operations, do it afterwards on the resulting text.
	    len_before = ml_get_curline_len();
	    ++text_prop_frozen;
	}
#endif
	सचे (State & VREPLACE_FLAG)
	{
	    // Get the number of screen cells used by the character we are
	    // going to delete.
	    getvcol(curwin, &curwin->w_cursor, रिक्त, &start_vcol, रिक्त, 0);
	    orig_vcols = chartabsize(ml_get_cursor(), start_vcol);
	}
	सचे (has_mbyte)
	{
	    (शून्य)del_char_after_col(limit_col);
	    सचे (State & VREPLACE_FLAG)
		orig_len = ml_get_cursor_len();
	    replace_प्रक्षिप(cc);
	}
	अञ्ञथा
	{
	    pchar_cursor(cc);
	    सचे (State & VREPLACE_FLAG)
		orig_len = ml_get_cursor_len() - 1;
	}
	replace_निष्कासय_ins();

	सचे (State & VREPLACE_FLAG)
	{
	    // Get the number of screen cells used by the inserted characters
	    p = ml_get_cursor();
	    ins_len = ml_get_cursor_len() - orig_len;
	    vcol = start_vcol;
	    अत्थाय (i = 0; i < ins_len; ++i)
	    {
		vcol += chartabsize(p + i, vcol);
		i += (*mb_ptr2len)(p + i) - 1;
	    }
	    vcol -= start_vcol;

	    // Delete spaces that were inserted after the cursor to keep the
	    // text aligned.
	    curwin->w_cursor.col += ins_len;
	    याव (vcol > orig_vcols && gchar_cursor() == ' ')
	    {
		del_char(FALSE);
		++orig_vcols;
	    }
	    curwin->w_cursor.col -= ins_len;
	}

	// mark the buffer as changed and prepare for displaying
	changed_bytes(curwin->w_cursor.lnum, curwin->w_cursor.col);

#ifdef FEAT_PROP_POPUP
	सचे (curbuf->b_has_textprop)
	{
	    size_t len_now = ml_get_curline_len();

	    --text_prop_frozen;
	    adjust_prop_columns(curwin->w_cursor.lnum, curwin->w_cursor.col,
					   (पूर्णाङ्क)(len_now - len_before), 0);
	}
#endif
    }
    अञ्ञथा सचे (cc == 0)
	(शून्य)del_char_after_col(limit_col);
}

#सचे defined(FEAT_RIGHTLEFT)
/*
 * Map Hebrew keyboard when in hkmap mode.
 */
    पूर्णाङ्क
hkmap(पूर्णाङ्क c)
{
    सचे (p_hkmapp)   // phonetic mapping, by Ilya Dogolazky
    {
	गणना {hALEF=0, BET, GIMEL, DALET, HEI, VAV, ZAIN, HET, TET, IUD,
	    KAFsofit, hKAF, LAMED, MEMsofit, MEM, NUNsofit, NUN, SAMEH, AIN,
	    PEIsofit, PEI, ZADIsofit, ZADI, KOF, RESH, hSHIN, TAV};
	स्थिर char_u map[26] =
	    {(char_u)hALEF/*a*/, (char_u)BET  /*b*/, (char_u)hKAF    /*c*/,
	     (char_u)DALET/*d*/, (char_u)-1   /*e*/, (char_u)PEIsofit/*f*/,
	     (char_u)GIMEL/*g*/, (char_u)HEI  /*h*/, (char_u)IUD     /*i*/,
	     (char_u)HET  /*j*/, (char_u)KOF  /*k*/, (char_u)LAMED   /*l*/,
	     (char_u)MEM  /*m*/, (char_u)NUN  /*n*/, (char_u)SAMEH   /*o*/,
	     (char_u)PEI  /*p*/, (char_u)-1   /*q*/, (char_u)RESH    /*r*/,
	     (char_u)ZAIN /*s*/, (char_u)TAV  /*t*/, (char_u)TET     /*u*/,
	     (char_u)VAV  /*v*/, (char_u)hSHIN/*w*/, (char_u)-1      /*x*/,
	     (char_u)AIN  /*y*/, (char_u)ZADI /*z*/};

	सचे (c == 'N' || c == 'M' || c == 'P' || c == 'C' || c == 'Z')
	    पटिनिवत्त (पूर्णाङ्क)(map[CharOrd(c)] - 1 + p_aleph);
							    // '-1'='sofit'
	अञ्ञथा सचे (c == 'x')
	    पटिनिवत्त 'X';
	अञ्ञथा सचे (c == 'q')
	    पटिनिवत्त '\''; // {geresh}={'}
	अञ्ञथा सचे (c == 246)
	    पटिनिवत्त ' ';  // \"o --> ' ' for a german keyboard
	अञ्ञथा सचे (c == 228)
	    पटिनिवत्त ' ';  // \"a --> ' '      -- / --
	अञ्ञथा सचे (c == 252)
	    पटिनिवत्त ' ';  // \"u --> ' '      -- / --
	// NOTE: islower() does not do the right thing for us on Linux so we
	// do this the same was as 5.7 and previous, so it works correctly on
	// all systems.  Specifically, the e.g. Delete and Arrow keys are
	// munged and won't work if e.g. searching for Hebrew text.
	अञ्ञथा सचे (c >= 'a' && c <= 'z')
	    पटिनिवत्त (पूर्णाङ्क)(map[Char_Ord_नीचः(c)] + p_aleph);
	अञ्ञथा
	    पटिनिवत्त c;
    }
    अञ्ञथा
    {
	विकल्प (c)
	{
	    पक्ष '`':	पटिनिवत्त ';';
	    पक्ष '/':	पटिनिवत्त '.';
	    पक्ष '\'':	पटिनिवत्त ',';
	    पक्ष 'q':	पटिनिवत्त '/';
	    पक्ष 'w':	पटिनिवत्त '\'';

			// Hebrew letters - set offset from 'a'
	    पक्ष ',':	c = '{'; भङ्ग;
	    पक्ष '.':	c = 'v'; भङ्ग;
	    पक्ष ';':	c = 't'; भङ्ग;
	    स्वभाव: {
			 स्थिर वर्ण str[] = "zqbcxlsjphmkwonu ydafe rig";

			 सचे (c < 'a' || c > 'z')
			     पटिनिवत्त c;
			 c = str[Char_Ord_नीचः(c)];
			 भङ्ग;
		     }
	}

	पटिनिवत्त (पूर्णाङ्क)(Char_Ord_नीचः(c) + p_aleph);
    }
}
#endif

    स्थिर शून्य
ins_reg(शून्य)
{
    पूर्णाङ्क		need_redraw = FALSE;
    पूर्णाङ्क		regname;
    पूर्णाङ्क		literally = 0;
    पूर्णाङ्क		vis_active = VIsual_active;

    /*
     * If we are going to wait for a character, show a '"'.
     */
    pc_status = PC_STATUS_UNSET;
    सचे (redrawing() && !char_avail())
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
    ++allow_कुञ्चिकाः;
    regname = plain_vgetc();
    LANGMAP_ADJUST(regname, TRUE);
    सचे (regname == Ctrl_R || regname == Ctrl_O || regname == Ctrl_P)
    {
	// Get a third key for literal register insertion
	literally = regname;
	add_to_showcmd_c(literally);
	regname = plain_vgetc();
	LANGMAP_ADJUST(regname, TRUE);
    }
    --no_mapping;
    --allow_कुञ्चिकाः;

#ifdef FEAT_EVAL
    // Don't call u_sync() while typing the expression or giving an error
    // message for it. Only call it explicitly.
    ++no_u_sync;
    सचे (regname == '=')
    {
	pos_T	curpos = curwin->w_cursor;
# ifdef HAVE_निवेशः_METHOD
	पूर्णाङ्क	im_on = im_get_status();
# endif
	// Sync undo when evaluating the expression calls setline() or
	// append(), so that it can be undone separately.
	u_sync_once = 2;

	regname = get_व्यञ्जकम्_register();

	// Cursor may be moved back a column.
	curwin->w_cursor = curpos;
	check_cursor();
# ifdef HAVE_निवेशः_METHOD
	// Restore the Input Method.
	सचे (im_on)
	    im_set_active(TRUE);
# endif
    }
    सचे (regname == NUL || !valid_yank_reg(regname, FALSE))
    {
	vim_beep(BO_REG);
	need_redraw = TRUE;	// remove the '"'
    }
    अञ्ञथा
    {
#endif
	सचे (literally == Ctrl_O || literally == Ctrl_P)
	{
	    // Append the command to the redo buffer.
	    AppendCharToRedobuff(Ctrl_R);
	    AppendCharToRedobuff(literally);
	    AppendCharToRedobuff(regname);

	    do_put(regname, रिक्त, BACKWARD, 1L,
		 (literally == Ctrl_P ? PUT_FIXINDENT : 0) | PUT_CURSEND);
	}
	अञ्ञथा सचे (निवेशय_reg(regname, literally) == FAIL)
	{
	    vim_beep(BO_REG);
	    need_redraw = TRUE;	// remove the '"'
	}
	अञ्ञथा सचे (stop_निवेशय_mode)
	    // When the '=' register was used and a function was invoked that
	    // did ":stopinsert" then stuff_empty() returns FALSE but we won't
	    // insert anything, need to remove the '"'
	    need_redraw = TRUE;

#ifdef FEAT_EVAL
    }
    --no_u_sync;
    सचे (u_sync_once == 1)
	ins_need_undo = TRUE;
    u_sync_once = 0;
#endif
    clear_showcmd();

    // If the inserted register is empty, we need to remove the '"'
    सचे (need_redraw || stuff_empty())
	edit_unputchar();

    // Disallow starting Visual mode here, would get a weird mode.
    सचे (!vis_active && VIsual_active)
	end_visual_mode();
}

/*
 * CTRL-G commands in Insert mode.
 */
    स्थिर शून्य
ins_ctrl_g(शून्य)
{
    पूर्णाङ्क		c;

    // Right after CTRL-X the cursor will be after the ruler.
    setcursor();

    /*
     * Don't map the second key. This also prevents the mode message to be
     * deleted when ESC is hit.
     */
    ++no_mapping;
    ++allow_कुञ्चिकाः;
    c = plain_vgetc();
    --no_mapping;
    --allow_कुञ्चिकाः;
    विकल्प (c)
    {
	// CTRL-G k and CTRL-G <Up>: cursor up to Insstart.col
	पक्ष K_UP:
	पक्ष Ctrl_K:
	पक्ष 'k': ins_up(TRUE);
		  भङ्ग;

	// CTRL-G j and CTRL-G <Down>: cursor down to Insstart.col
	पक्ष K_DOWN:
	पक्ष Ctrl_J:
	पक्ष 'j': ins_down(TRUE);
		  भङ्ग;

	// CTRL-G u: start new undoable edit
	पक्ष 'u': u_sync(TRUE);
		  ins_need_undo = TRUE;

		  // Need to reset Insstart, esp. because a BS that joins
		  // a line to the previous one must save for undo.
		  update_Insstart_orig = FALSE;
		  Insstart = curwin->w_cursor;
		  भङ्ग;

	// CTRL-G U: do not break undo with the next char
	पक्ष 'U':
		  // Allow one left/right cursor movement with the next char,
		  // without breaking undo.
		  dont_sync_undo = MAYBE;
		  भङ्ग;

	पक्ष ESC:
		  // Esc after CTRL-G cancels it.
		  भङ्ग;

	// Unknown CTRL-G command, reserved for future expansion.
	स्वभाव:  vim_beep(BO_CTRLG);
    }
}

/*
 * CTRL-^ in Insert mode.
 */
    स्थिर शून्य
ins_ctrl_hat(शून्य)
{
    सचे (map_to_exists_mode((char_u *)"", MODE_LANGMAP, FALSE))
    {
	// ":lmap" mappings exists, Toggle use of ":lmap" mappings.
	सचे (State & MODE_LANGMAP)
	{
	    curbuf->b_p_iminsert = B_IMODE_NONE;
	    State &= ~MODE_LANGMAP;
	}
	अञ्ञथा
	{
	    curbuf->b_p_iminsert = B_IMODE_LMAP;
	    State |= MODE_LANGMAP;
#ifdef HAVE_निवेशः_METHOD
	    im_set_active(FALSE);
#endif
	}
    }
#ifdef HAVE_निवेशः_METHOD
    अञ्ञथा
    {
	// There are no ":lmap" mappings, toggle IM
	सचे (im_get_status())
	{
	    curbuf->b_p_iminsert = B_IMODE_NONE;
	    im_set_active(FALSE);
	}
	अञ्ञथा
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
    सचे (gui.in_use)
	gui_update_cursor(TRUE, FALSE);
#endif
#सचे defined(FEAT_KEYMAP)
    // Show/unshow value of 'keymap' in status lines.
    status_redraw_curbuf();
#endif
}

/*
 * Handle ESC in insert mode.
 * Returns TRUE when leaving insert mode, FALSE when going to repeat the
 * insert.
 */
    स्थिर पूर्णाङ्क
ins_esc(
    long	*गणनम्,
    पूर्णाङ्क		cmdchar,
    पूर्णाङ्क		nomove)	    // don't move cursor
{
    पूर्णाङ्क		temp;
    स्थिर पूर्णाङ्क	disabled_redraw = FALSE;
#ifdef FEAT_CONCEAL
    // Remember if the cursor line was concealed before changing State.
    पूर्णाङ्क		cursor_पङ्क्तिः_was_concealed = curwin->w_p_cole > 0
						&& conceal_cursor_पङ्क्तिः(curwin);
#endif

#ifdef FEAT_SPELL
    check_spell_redraw();
#endif

    temp = curwin->w_cursor.col;
    सचे (disabled_redraw)
    {
	सचे (RedrawingDisabled > 0)
	    --RedrawingDisabled;
	disabled_redraw = FALSE;
    }
    सचे (!arrow_used)
    {
	/*
	 * Don't append the ESC for "r<CR>" and "grx".
	 * When 'insertmode' is set only CTRL-L stops Insert mode.  Needed for
	 * when "count" is non-zero.
	 */
	सचे (cmdchar != 'r' && cmdchar != 'v')
	    AppendToRedobuff(p_im ? (char_u *)"\014" : ESC_STR);

	/*
	 * Repeating insert may take a long time.  Check for
	 * interrupt now and then.
	 */
	सचे (*गणनम् > 0)
	{
	    पङ्क्तिः_breakcheck();
	    सचे (got_int)
		*गणनम् = 0;
	}

	सचे (--*गणनम् > 0)	// repeat what was typed
	{
	    // Vi repeats the insert without replacing characters.
	    सचे (vim_strchr(p_cpo, CPO_REPLCNT) != रिक्त)
		State &= ~REPLACE_FLAG;

	    (शून्य)start_redo_ins();
	    सचे (cmdchar == 'r' || cmdchar == 'v')
		stuffRedoReadbuff(ESC_STR);	// no ESC in redo buffer
	    ++RedrawingDisabled;
	    disabled_redraw = TRUE;
	    पटिनिवत्त FALSE;	// repeat the insert
	}
	stop_निवेशय(&curwin->w_cursor, TRUE, nomove);
	undisplay_dollar();
    }

    सचे (cmdchar != 'r' && cmdchar != 'v')
	ins_apply_autocmds(EVENT_INSERTLEAVEPRE);

    // When an autoindent was removed, curswant stays after the
    // indent
    सचे (restart_edit == NUL && (colnr_T)temp == curwin->w_cursor.col)
	curwin->w_set_curswant = true;

    // Remember the last Insert position in the '^ mark.
    सचे ((cmdmod.cmod_flags & CMOD_KEEPJUMPS) == 0)
	curbuf->b_last_निवेशय = curwin->w_cursor;

    /*
     * The cursor should end up on the last inserted character.
     * Don't do it for CTRL-O, unless past the end of the line.
     */
    सचे (!nomove
	    && (curwin->w_cursor.col != 0
		|| curwin->w_cursor.coladd > 0)
	    && (restart_edit == NUL
		   || (gchar_cursor() == NUL && !VIsual_active))
#ifdef FEAT_RIGHTLEFT
	    && !revins_on
#endif
				      )
    {
	सचे (curwin->w_cursor.coladd > 0 || get_ve_flags() == VE_ALL)
	{
	    oneleft();
	    सचे (restart_edit != NUL)
		++curwin->w_cursor.coladd;
	}
	अञ्ञथा
	{
	    --curwin->w_cursor.col;
	    curwin->w_valid &= ~(VALID_WCOL|VALID_VIRTCOL);
	    // Correct cursor for multi-byte character.
	    सचे (has_mbyte)
		mb_adjust_cursor();
	}
    }

#ifdef HAVE_निवेशः_METHOD
    // Disable IM to allow typing English directly for Normal mode commands.
    // When ":lmap" is enabled don't change 'iminsert' (IM can be enabled as
    // well).
    सचे (!(State & MODE_LANGMAP))
	im_save_status(&curbuf->b_p_iminsert);
    im_set_active(FALSE);
#endif

    State = MODE_NORMAL;
    may_trigger_modechanged();
    // need to position cursor again when on a TAB and when on a char with
    // virtual text.
    सचे (gchar_cursor() == TAB
#ifdef FEAT_PROP_POPUP
	    || curbuf->b_has_textprop
#endif
       )
	curwin->w_valid &= ~(VALID_WROW|VALID_WCOL|VALID_VIRTCOL);

    setmouse();
#ifdef CURSOR_SHAPE
    ui_cursor_shape();		// may show different cursor shape
#endif
    सचे (!p_ek)
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
    conceal_check_cursor_पङ्क्तिः(cursor_पङ्क्तिः_was_concealed);
#endif

    // When recording or for CTRL-O, need to display the new mode.
    // Otherwise remove the mode message.
    सचे (reg_recording != 0 || restart_edit != NUL)
	showmode();
    अञ्ञथा सचे (p_smd && (got_int || !skip_showmode()))
	सन्देशः("");

    पटिनिवत्त TRUE;	    // exit Insert mode
}

#ifdef FEAT_RIGHTLEFT
/*
 * Toggle language: hkmap and revins_on.
 * Move to end of reverse inserted text.
 */
    स्थिर शून्य
ins_ctrl_(शून्य)
{
    सचे (revins_on && revins_chars && revins_scol >= 0)
    {
	याव (gchar_cursor() != NUL && revins_chars--)
	    ++curwin->w_cursor.col;
    }
    p_ri = !p_ri;
    revins_on = (State == MODE_निवेशय && p_ri);
    सचे (revins_on)
    {
	revins_scol = curwin->w_cursor.col;
	revins_legal++;
	revins_chars = 0;
	undisplay_dollar();
    }
    अञ्ञथा
	revins_scol = -1;
    p_hkmap = curwin->w_p_rl ^ p_ri;    // be consistent!
    showmode();
}
#endif

/*
 * If 'keymodel' contains "startsel", may start selection.
 * Returns TRUE when a CTRL-O and other keys stuffed.
 */
    स्थिर पूर्णाङ्क
ins_start_select(पूर्णाङ्क c)
{
    सचे (!km_startsel)
	पटिनिवत्त FALSE;
    विकल्प (c)
    {
	पक्ष K_KHOME:
	पक्ष K_KEND:
	पक्ष K_PAGEUP:
	पक्ष K_KPAGEUP:
	पक्ष K_PAGEDOWN:
	पक्ष K_KPAGEDOWN:
#ifdef MACOS_X
	पक्ष K_वामः:
	पक्ष K_दक्षिणः:
	पक्ष K_UP:
	पक्ष K_DOWN:
	पक्ष K_END:
	पक्ष K_HOME:
#endif
	    सचे (!(mod_mask & MOD_MASK_SHIFT))
		भङ्ग;
	    // FALLTHROUGH
	पक्ष K_S_वामः:
	पक्ष K_S_दक्षिणः:
	पक्ष K_S_UP:
	पक्ष K_S_DOWN:
	पक्ष K_S_END:
	पक्ष K_S_HOME:
	    // Start selection right away, the cursor can move with CTRL-O when
	    // beyond the end of the line.
	    start_selection();

	    // Execute the key in (insert) Select mode.
	    stuffcharReadbuff(Ctrl_O);
	    सचे (mod_mask)
	    {
		char_u	    buf[4];

		buf[0] = K_SPECIAL;
		buf[1] = KS_MODIFIER;
		buf[2] = mod_mask;
		buf[3] = NUL;
		stuffReadbuffLen(buf, 3L);
	    }
	    stuffcharReadbuff(c);
	    पटिनिवत्त TRUE;
    }
    पटिनिवत्त FALSE;
}

/*
 * <Insert> key in Insert mode: toggle insert/replace mode.
 */
    स्थिर शून्य
ins_निवेशय(पूर्णाङ्क replaceState)
{
#ifdef FEAT_EVAL
    set_vim_var_string(VV_INSERTMODE,
		   (char_u *)((State & REPLACE_FLAG) ? "i"
			    : replaceState == MODE_VREPLACE ? "v" : "r"), 1);
#endif
    ins_apply_autocmds(EVENT_INSERTCHANGE);
    सचे (State & REPLACE_FLAG)
	State = MODE_निवेशय | (State & MODE_LANGMAP);
    अञ्ञथा
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
    स्थिर शून्य
ins_ctrl_o(शून्य)
{
    restart_VIsual_select = 0;
    सचे (State & VREPLACE_FLAG)
	restart_edit = 'V';
    अञ्ञथा सचे (State & REPLACE_FLAG)
	restart_edit = 'R';
    अञ्ञथा
	restart_edit = 'I';
    सचे (virtual_active())
	ins_at_eol = FALSE;	// cursor always keeps its column
    अञ्ञथा
	ins_at_eol = (gchar_cursor() == NUL);
}

/*
 * If the cursor is on an indent, ^T/^D insert/delete one
 * shiftwidth.	Otherwise ^T/^D behave like a "<<" or ">>".
 * Always round the indent to 'shiftwidth', this is compatible
 * with vi.  But vi only supports ^T and ^D after an
 * autoindent, we support it everywhere.
 */
    स्थिर शून्य
ins_shift(पूर्णाङ्क c, पूर्णाङ्क lastc)
{
    सचे (stop_arrow() == FAIL)
	पटिनिवत्त;
    AppendCharToRedobuff(c);

    /*
     * 0^D and ^^D: remove all indent.
     */
    सचे (c == Ctrl_D && (lastc == '0' || lastc == '^')
						  && curwin->w_cursor.col > 0)
    {
	--curwin->w_cursor.col;
	(शून्य)del_char(FALSE);		// delete the '^' or '0'
	// In Replace mode, restore the characters that '^' or '0' replaced.
	सचे (State & REPLACE_FLAG)
	    replace_निष्कासय_ins();
	सचे (lastc == '^')
	    old_indent = get_indent();	// remember curr. indent
	change_indent(INDENT_SET, 0, TRUE, 0, TRUE);
    }
    अञ्ञथा
	change_indent(c == Ctrl_D ? INDENT_DEC : INDENT_INC, 0, TRUE, 0, TRUE);

    सचे (did_ai && *skipwhite(ml_get_curline()) != NUL)
	did_ai = FALSE;
    did_si = FALSE;
    can_si = FALSE;
    can_si_back = FALSE;
    can_cindent = FALSE;	// no cindenting after ^D or ^T
}

    स्थिर शून्य
ins_del(शून्य)
{
    पूर्णाङ्क	    temp;

    सचे (stop_arrow() == FAIL)
	पटिनिवत्त;
    सचे (gchar_cursor() == NUL)		// delete newline
    {
	temp = curwin->w_cursor.col;
	सचे (!can_bs(BS_EOL)		// only if "eol" included
		|| do_join(2, FALSE, TRUE, FALSE, FALSE) == FAIL)
	    vim_beep(BO_BS);
	अञ्ञथा
	{
	    curwin->w_cursor.col = temp;
	    // Adjust orig_line_count in case more lines have been deleted than
	    // have been added. That makes sure, that open_line() later
	    // can access all buffer lines correctly
	    सचे (State & VREPLACE_FLAG &&
		    orig_पङ्क्तिः_गणनम् > curbuf->b_ml.ml_पङ्क्तिः_गणनम्)
		orig_पङ्क्तिः_गणनम् = curbuf->b_ml.ml_पङ्क्तिः_गणनम्;
	}
    }
    अञ्ञथा सचे (del_char(FALSE) == FAIL)  // delete char under cursor
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
    स्थिर शून्य
ins_bs_one(शून्य)
{
    dec_cursor();
    सचे (State & REPLACE_FLAG)
    {
	// Don't delete characters before the insert point when in
	// Replace mode
	सचे (curwin->w_cursor.lnum != Insstart.lnum
		|| curwin->w_cursor.col >= Insstart.col)
	    replace_do_bs(-1);
    }
    अञ्ञथा
	(शून्य)del_char(FALSE);
}

/*
 * Handle Backspace, delete-word and delete-line in Insert mode.
 * Return TRUE when backspace was actually used.
 */
    स्थिर पूर्णाङ्क
ins_bs(
    पूर्णाङ्क		c,
    पूर्णाङ्क		mode,
    पूर्णाङ्क		*inserted_space_p)
{
    linenr_T	lnum;
    पूर्णाङ्क		cc;
    पूर्णाङ्क		temp = 0;	    // init for GCC
    colnr_T	save_col;
    colnr_T	mincol;
    पूर्णाङ्क		did_backspace = FALSE;
    पूर्णाङ्क		in_indent;
    पूर्णाङ्क		oldState;
    पूर्णाङ्क		cpc[MAX_MCO];	    // composing characters
    पूर्णाङ्क		call_fix_indent = FALSE;

    /*
     * can't delete anything in an empty file
     * can't backup past first character in buffer
     * can't backup past starting point unless 'backspace' > 1
     * can backup to a previous line if 'backspace' == 0
     */
    सचे (       BUFEMPTY()
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
	पटिनिवत्त FALSE;
    }

    सचे (stop_arrow() == FAIL)
	पटिनिवत्त FALSE;
    in_indent = inindent(0);
    सचे (in_indent)
	can_cindent = FALSE;
    end_comment_pending = NUL;	// After BS, don't auto-end comment
#ifdef FEAT_RIGHTLEFT
    सचे (revins_on)	    // put cursor after last inserted char
	inc_cursor();
#endif

    // Virtualedit:
    //	BACKSPACE_CHAR eats a virtual space
    //	BACKSPACE_WORD eats all coladd
    //	BACKSPACE_LINE eats all coladd and keeps going
    सचे (curwin->w_cursor.coladd > 0)
    {
	सचे (mode == BACKSPACE_CHAR)
	{
	    --curwin->w_cursor.coladd;
	    पटिनिवत्त TRUE;
	}
	सचे (mode == BACKSPACE_WORD)
	{
	    curwin->w_cursor.coladd = 0;
	    पटिनिवत्त TRUE;
	}
	curwin->w_cursor.coladd = 0;
    }

    /*
     * Delete newline!
     */
    सचे (curwin->w_cursor.col == 0)
    {
	lnum = Insstart.lnum;
	सचे (curwin->w_cursor.lnum == lnum
#ifdef FEAT_RIGHTLEFT
			|| revins_on
#endif
				    )
	{
	    सचे (u_save((linenr_T)(curwin->w_cursor.lnum - 2),
			       (linenr_T)(curwin->w_cursor.lnum + 1)) == FAIL)
		पटिनिवत्त FALSE;
	    --Insstart.lnum;
	    Insstart.col = ml_get_len(Insstart.lnum);
	}
	/*
	 * In replace mode:
	 * cc < 0: NL was inserted, delete it
	 * cc >= 0: NL was replaced, put original characters back
	 */
	cc = -1;
	सचे (State & REPLACE_FLAG)
	    cc = replace_निष्कासय();	    // returns -1 if NL was inserted
	/*
	 * In replace mode, in the line we started replacing, we only move the
	 * cursor.
	 */
	सचे ((State & REPLACE_FLAG) && curwin->w_cursor.lnum <= lnum)
	{
	    dec_cursor();
	}
	अञ्ञथा
	{
	    सचे (!(State & VREPLACE_FLAG)
				   || curwin->w_cursor.lnum > orig_पङ्क्तिः_गणनम्)
	    {
		temp = gchar_cursor();	// remember current char
		--curwin->w_cursor.lnum;

		// When "aw" is in 'formatoptions' we must delete the space at
		// the end of the line, otherwise the line will be broken
		// again when auto-formatting.
		सचे (has_format_option(FO_AUTO)
					   && has_format_option(FO_WHITE_PAR))
		{
		    char_u  *ptr = ml_get_buf(curbuf, curwin->w_cursor.lnum,
									FALSE);
		    पूर्णाङ्क	    len = ml_get_curline_len();

		    सचे (len > 0 && ptr[len - 1] == ' ')
		    {
			char_u *newp = alloc(curbuf->b_ml.ml_पङ्क्तिः_len - 1);

			सचे (newp != रिक्त)
			{
			    mch_memmove(newp, ptr, len - 1);
			    newp[len - 1] = NUL;
			    सचे (curbuf->b_ml.ml_पङ्क्तिः_len > len + 1)
				mch_memmove(newp + len, ptr + len + 1,
					   curbuf->b_ml.ml_पङ्क्तिः_len - len - 1);

			    सचे (curbuf->b_ml.ml_flags
					      & (ML_पङ्क्तिः_DIRTY | ML_ALLOCATED))
				vim_free(curbuf->b_ml.ml_पङ्क्तिः_ptr);
			    curbuf->b_ml.ml_पङ्क्तिः_ptr = newp;
			    curbuf->b_ml.ml_पङ्क्तिः_len--;
			    curbuf->b_ml.ml_पङ्क्तिः_textlen--;
			    curbuf->b_ml.ml_flags |= ML_पङ्क्तिः_DIRTY;
			}
		    }
		}

		(शून्य)do_join(2, FALSE, FALSE, FALSE, FALSE);
		सचे (temp == NUL && gchar_cursor() != NUL)
		    inc_cursor();
	    }
	    अञ्ञथा
		dec_cursor();

	    /*
	     * In MODE_REPLACE mode we have to put back the text that was
	     * replaced by the NL. On the replace stack is first a
	     * NUL-terminated sequence of characters that were deleted and then
	     * the characters that NL replaced.
	     */
	    सचे (State & REPLACE_FLAG)
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
		याव (cc > 0)
		{
		    save_col = curwin->w_cursor.col;
		    mb_replace_निष्कासय_ins(cc);
		    curwin->w_cursor.col = save_col;
		    cc = replace_निष्कासय();
		}
		// restore the characters that NL replaced
		replace_निष्कासय_ins();
		State = oldState;
	    }
	}
	did_ai = FALSE;
    }
    अञ्ञथा
    {
	/*
	 * Delete character(s) before the cursor.
	 */
#ifdef FEAT_RIGHTLEFT
	सचे (revins_on)		// put cursor on last inserted char
	    dec_cursor();
#endif
	mincol = 0;
						// keep indent
	सचे (mode == BACKSPACE_पङ्क्तिः
		&& (curbuf->b_p_ai || cindent_on())
#ifdef FEAT_RIGHTLEFT
		&& !revins_on
#endif
			    )
	{
	    save_col = curwin->w_cursor.col;
	    beginline(BL_WHITE);
	    सचे (curwin->w_cursor.col < save_col)
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
	सचे (	   mode == BACKSPACE_CHAR
		&& ((p_sta && in_indent)
		    || ((get_sts_मानम्() != 0
#ifdef FEAT_VARTABS
			|| tabstop_गणनम्(curbuf->b_p_vsts_array)
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
	    char_u	*पङ्क्तिः;
	    char_u	*ptr;
	    char_u	*cursor_ptr;
	    char_u	*space_ptr;
	    colnr_T	space_vcol = 0;
	    पूर्णाङ्क		prev_space = FALSE;
	    colnr_T	want_col;

	    *inserted_space_p = FALSE;

	    space_ptr = ptr = पङ्क्तिः = ml_get_curline();
	    cursor_ptr = पङ्क्तिः + curwin->w_cursor.col;

	    // Compute virtual column of cursor position, and find the last
	    // whitespace before cursor that is preceded by non-whitespace.
	    // Use chartabsize() so that virtual text and wrapping are ignored.
	    याव (ptr < cursor_ptr)
	    {
		पूर्णाङ्क	cur_space = VIM_ISWHITE(*ptr);

		सचे (!prev_space && cur_space)
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
	    सचे (p_sta && in_indent)
		want_vcol -= want_vcol % (पूर्णाङ्क)get_sw_मानम्(curbuf);
	    अञ्ञथा
#ifdef FEAT_VARTABS
		want_vcol = tabstop_start(want_vcol, get_sts_मानम्(),
						       curbuf->b_p_vsts_array);
#अञ्ञथा
		want_vcol -= want_vcol % (पूर्णाङ्क)get_sts_मानम्();
#endif

	    // Find the position to stop backspacing.
	    // Use chartabsize() so that virtual text and wrapping are ignored.
	    याव (TRUE)
	    {
		पूर्णाङ्क size = chartabsize(space_ptr, space_vcol);

		सचे (space_vcol + size > want_vcol)
		    भङ्ग;
		space_vcol += size;
		MB_PTR_ADV(space_ptr);
	    }
	    want_col = space_ptr - पङ्क्तिः;

	    // Delete characters until we are at or before want_col.
	    याव (curwin->w_cursor.col > want_col)
		ins_bs_one();

	    // Insert extra spaces until we are at want_vcol.
	    अत्थाय (; space_vcol < want_vcol; space_vcol++)
	    {
		// Remember the first char we inserted
		सचे (curwin->w_cursor.lnum == Insstart_orig.lnum
				   && curwin->w_cursor.col < Insstart_orig.col)
		    Insstart_orig.col = curwin->w_cursor.col;

		सचे (State & VREPLACE_FLAG)
		    ins_char(' ');
		अञ्ञथा
		{
		    ins_str((char_u *)" ", 1);
		    सचे ((State & REPLACE_FLAG))
			replace_प्रक्षिप(NUL);
		}
	    }
	}

	/*
	 * Delete up to starting point, start of line or previous word.
	 */
	अञ्ञथा
	{
	    पूर्णाङ्क cclass = 0, prev_cclass = 0;

	    सचे (has_mbyte)
		cclass = mb_get_class(ml_get_cursor());
	    करोहि
	    {
#ifdef FEAT_RIGHTLEFT
		सचे (!revins_on) // put cursor on char to be deleted
#endif
		    dec_cursor();

		cc = gchar_cursor();
		// look multi-byte character class
		सचे (has_mbyte)
		{
		    prev_cclass = cclass;
		    cclass = mb_get_class(ml_get_cursor());
		}

		// start of word?
		सचे (mode == BACKSPACE_WORD && !vim_isspace(cc))
		{
		    mode = BACKSPACE_WORD_NOT_SPACE;
		    temp = vim_iswordc(cc);
		}
		// end of word?
		अञ्ञथा सचे (mode == BACKSPACE_WORD_NOT_SPACE
			&& ((vim_isspace(cc) || vim_iswordc(cc) != temp)
			|| prev_cclass != cclass))
		{
#ifdef FEAT_RIGHTLEFT
		    सचे (!revins_on)
#endif
			inc_cursor();
#ifdef FEAT_RIGHTLEFT
		    अञ्ञथा सचे (State & REPLACE_FLAG)
			dec_cursor();
#endif
		    भङ्ग;
		}
		सचे (State & REPLACE_FLAG)
		    replace_do_bs(-1);
		अञ्ञथा
		{
		    सचे (enc_utf8 && p_deco)
			(शून्य)utfc_ptr2char(ml_get_cursor(), cpc);
		    (शून्य)del_char(FALSE);
		    /*
		     * If there are combining characters and 'delcombine' is set
		     * move the cursor back.  Don't back up before the base
		     * character.
		     */
		    सचे (enc_utf8 && p_deco && cpc[0] != NUL)
			inc_cursor();
#ifdef FEAT_RIGHTLEFT
		    सचे (revins_chars)
		    {
			revins_chars--;
			revins_legal++;
		    }
		    सचे (revins_on && gchar_cursor() == NUL)
			भङ्ग;
#endif
		}
		// Just a single backspace?:
		सचे (mode == BACKSPACE_CHAR)
		    भङ्ग;
	    } याव (
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
    सचे (curwin->w_cursor.col <= 1)
	did_ai = FALSE;

    सचे (call_fix_indent)
	fix_indent();

    /*
     * It's a little strange to put backspaces into the redo
     * buffer, but it makes auto-indent a lot easier to deal
     * with.
     */
    AppendCharToRedobuff(c);

    // If deleted before the insertion point, adjust it
    सचे (curwin->w_cursor.lnum == Insstart_orig.lnum
				  && curwin->w_cursor.col < Insstart_orig.col)
	Insstart_orig.col = curwin->w_cursor.col;

    // vi behaviour: the cursor moves backward but the character that
    //		     was there remains visible
    // Vim behaviour: the cursor moves backward and the character that
    //		      was there is erased from the screen.
    // We can emulate the vi behaviour by pretending there is a dollar
    // displayed even when there isn't.
    //  --pkv Sun Jan 19 01:56:40 EST 2003
    सचे (vim_strchr(p_cpo, CPO_BACKSPACE) != रिक्त && dollar_vcol == -1)
	dollar_vcol = curwin->w_virtcol;

#ifdef FEAT_FOLDING
    // When deleting a char the cursor line must never be in a closed fold.
    // E.g., when 'foldmethod' is indent and deleting the first non-white
    // char before a Tab.
    सचे (did_backspace)
	foldOpenCursor();
#endif

    पटिनिवत्त did_backspace;
}

/*
 * Handle receiving P_PS: start paste mode.  Inserts the following text up to
 * P_PE literally.
 * When "drop" is TRUE then consume the text and drop it.
 */
    पूर्णाङ्क
bracketed_paste(paste_mode_T mode, पूर्णाङ्क drop, garray_T *gap)
{
    पूर्णाङ्क		c;
    char_u	buf[NUMBUFLEN + MB_MAXBYTES];
    पूर्णाङ्क		idx = 0;
    char_u	*end = find_termcode((char_u *)"PE");
    पूर्णाङ्क		ret_char = -1;
    पूर्णाङ्क		save_allow_कुञ्चिकाः = allow_कुञ्चिकाः;
    पूर्णाङ्क		save_paste = p_paste;

    // If the end code is too long we can't detect it, read everything.
    सचे (end != रिक्त && STRLEN(end) >= NUMBUFLEN)
	end = रिक्त;
    ++no_mapping;
    allow_कुञ्चिकाः = 0;
    सचे (!p_paste)
	// Also have the side effects of setting 'paste' to make it work much
	// faster.
	set_option_मानम्_give_err((char_u *)"paste", TRUE, रिक्त, 0);

    अत्थाय (;;)
    {
	// When the end is not defined read everything there is.
	सचे (end == रिक्त && vpeekc() == NUL)
	    भङ्ग;
	करोहि
	    c = vgetc();
	याव (c == K_IGNORE || c == K_VER_SCROLLBAR || c == K_HOR_SCROLLBAR);

	सचे (c == NUL || got_int || (ex_normal_busy > 0 && c == Ctrl_C))
	    // When CTRL-C was encountered the typeahead will be flushed and we
	    // won't get the end sequence.  Except when using ":normal".
	    भङ्ग;

	सचे (has_mbyte)
	    idx += (*mb_char2bytes)(c, buf + idx);
	अञ्ञथा
	    buf[idx++] = c;
	buf[idx] = NUL;
	सचे (end != रिक्त && STRNCMP(buf, end, idx) == 0)
	{
	    सचे (end[idx] == NUL)
		भङ्ग; // Found the end of paste code.
	    अनुवर्त;
	}
	सचे (!drop)
	{
	    विकल्प (mode)
	    {
		पक्ष PASTE_CMDLINE:
		    put_on_cmdline(buf, idx, TRUE);
		    भङ्ग;

		पक्ष PASTE_EX:
		    // add one for the NUL that is going to be appended
		    सचे (gap != रिक्त && ga_grow(gap, idx + 1) == OK)
		    {
			mch_memmove((वर्ण *)gap->ga_data + gap->ga_len,
							     buf, (size_t)idx);
			gap->ga_len += idx;
		    }
		    भङ्ग;

		पक्ष PASTE_निवेशय:
		    सचे (stop_arrow() == OK)
		    {
			c = buf[0];
			सचे (idx == 1 && (c == CAR || c == K_KENTER || c == NL))
			    ins_eol(c);
			अञ्ञथा
			{
			    ins_char_bytes(buf, idx);
			    AppendToRedobuffLit(buf, idx);
			}
		    }
		    भङ्ग;

		पक्ष PASTE_ONE_CHAR:
		    सचे (ret_char == -1)
		    {
			सचे (has_mbyte)
			    ret_char = (*mb_ptr2char)(buf);
			अञ्ञथा
			    ret_char = buf[0];
		    }
		    भङ्ग;
	    }
	}
	idx = 0;
    }

    --no_mapping;
    allow_कुञ्चिकाः = save_allow_कुञ्चिकाः;
    सचे (!save_paste)
	set_option_मानम्_give_err((char_u *)"paste", FALSE, रिक्त, 0);

    पटिनिवत्त ret_char;
}

#सचे defined(FEAT_GUI_TABLINE)
    स्थिर शून्य
ins_tabline(पूर्णाङ्क c)
{
    // We will be leaving the current window, unless closing another tab.
    सचे (c != K_TABMENU || current_tabmenu != TABLINE_MENU_CLOSE
		|| (current_tab != 0 && current_tab != tabpage_index(curtab)))
    {
	undisplay_dollar();
	start_arrow(&curwin->w_cursor);
	can_cindent = TRUE;
    }

    सचे (c == K_TABLINE)
	goto_tabpage(current_tab);
    अञ्ञथा
    {
	handle_tabmenu();
	redraw_statuslines();	// will redraw the tabline when needed
    }
}
#endif

#सचे defined(FEAT_GUI)
    शून्य
ins_scroll(शून्य)
{
    pos_T	tpos;

    undisplay_dollar();
    tpos = curwin->w_cursor;
    सचे (gui_do_scroll())
    {
	start_arrow(&tpos);
	can_cindent = TRUE;
    }
}

    शून्य
ins_horscroll(शून्य)
{
    pos_T	tpos;

    undisplay_dollar();
    tpos = curwin->w_cursor;
    सचे (do_mousescroll_horiz(scrollbar_मानम्))
    {
	start_arrow(&tpos);
	can_cindent = TRUE;
    }
}
#endif

    स्थिर शून्य
ins_वामः(शून्य)
{
    pos_T	tpos;
    पूर्णाङ्क		end_change = dont_sync_undo == FALSE; // end undoable change

#ifdef FEAT_FOLDING
    सचे ((fdo_flags & FDO_HOR) && कुञ्चिका_Typed)
	foldOpenCursor();
#endif
    undisplay_dollar();
    tpos = curwin->w_cursor;
    सचे (oneleft() == OK)
    {
#सचे defined(FEAT_XIM) && defined(FEAT_GUI_GTK)
	// Only call start_arrow() when not busy with preediting, it will
	// break undo.  K_LEFT is inserted in im_correct_cursor().
	सचे (p_imst == IM_OVER_THE_SPOT || !im_is_preediting())
#endif
	{
	    start_arrow_with_change(&tpos, end_change);
	    सचे (!end_change)
		AppendCharToRedobuff(K_वामः);
	}
#ifdef FEAT_RIGHTLEFT
	// If exit reversed string, position is fixed
	सचे (revins_scol != -1 && (पूर्णाङ्क)curwin->w_cursor.col >= revins_scol)
	    revins_legal++;
	revins_chars++;
#endif
    }

    /*
     * if 'whichwrap' set for cursor in insert mode may go to
     * previous line
     */
    अञ्ञथा सचे (vim_strchr(p_ww, '[') != रिक्त && curwin->w_cursor.lnum > 1)
    {
	// always break undo when moving upwards/downwards, else undo may break
	start_arrow(&tpos);
	--(curwin->w_cursor.lnum);
	coladvance((colnr_T)MAXCOL);
	curwin->w_set_curswant = true;	// so we stay at the end
    }
    अञ्ञथा
	vim_beep(BO_CRSR);
    dont_sync_undo = FALSE;
}

    स्थिर शून्य
ins_home(पूर्णाङ्क c)
{
    pos_T	tpos;

#ifdef FEAT_FOLDING
    सचे ((fdo_flags & FDO_HOR) && कुञ्चिका_Typed)
	foldOpenCursor();
#endif
    undisplay_dollar();
    tpos = curwin->w_cursor;
    सचे (c == K_C_HOME)
	curwin->w_cursor.lnum = 1;
    curwin->w_cursor.col = 0;
    curwin->w_cursor.coladd = 0;
    curwin->w_curswant = 0;
    start_arrow(&tpos);
}

    स्थिर शून्य
ins_end(पूर्णाङ्क c)
{
    pos_T	tpos;

#ifdef FEAT_FOLDING
    सचे ((fdo_flags & FDO_HOR) && कुञ्चिका_Typed)
	foldOpenCursor();
#endif
    undisplay_dollar();
    tpos = curwin->w_cursor;
    सचे (c == K_C_END)
	curwin->w_cursor.lnum = curbuf->b_ml.ml_पङ्क्तिः_गणनम्;
    coladvance((colnr_T)MAXCOL);
    curwin->w_curswant = MAXCOL;

    start_arrow(&tpos);
}

    स्थिर शून्य
ins_s_वामः(शून्य)
{
    पूर्णाङ्क end_change = dont_sync_undo == FALSE; // end undoable change
#ifdef FEAT_FOLDING
    सचे ((fdo_flags & FDO_HOR) && कुञ्चिका_Typed)
	foldOpenCursor();
#endif
    undisplay_dollar();
    सचे (curwin->w_cursor.lnum > 1 || curwin->w_cursor.col > 0)
    {
	start_arrow_with_change(&curwin->w_cursor, end_change);
	सचे (!end_change)
	    AppendCharToRedobuff(K_S_वामः);
	(शून्य)bck_word(1L, FALSE, FALSE);
	curwin->w_set_curswant = true;
    }
    अञ्ञथा
	vim_beep(BO_CRSR);
    dont_sync_undo = FALSE;
}

    स्थिर शून्य
ins_दक्षिणः(शून्य)
{
    पूर्णाङ्क end_change = dont_sync_undo == FALSE; // end undoable change

#ifdef FEAT_FOLDING
    सचे ((fdo_flags & FDO_HOR) && कुञ्चिका_Typed)
	foldOpenCursor();
#endif
    undisplay_dollar();
    सचे (gchar_cursor() != NUL || virtual_active())
    {
	start_arrow_with_change(&curwin->w_cursor, end_change);
	सचे (!end_change)
	    AppendCharToRedobuff(K_दक्षिणः);
	curwin->w_set_curswant = true;
	सचे (virtual_active())
	    oneright();
	अञ्ञथा
	{
	    सचे (has_mbyte)
		curwin->w_cursor.col += (*mb_ptr2len)(ml_get_cursor());
	    अञ्ञथा
		++curwin->w_cursor.col;
	}

#ifdef FEAT_RIGHTLEFT
	revins_legal++;
	सचे (revins_chars)
	    revins_chars--;
#endif
    }
    // if 'whichwrap' set for cursor in insert mode, may move the
    // cursor to the next line
    अञ्ञथा सचे (vim_strchr(p_ww, ']') != रिक्त
	    && curwin->w_cursor.lnum < curbuf->b_ml.ml_पङ्क्तिः_गणनम्)
    {
	start_arrow(&curwin->w_cursor);
	curwin->w_set_curswant = true;
	++curwin->w_cursor.lnum;
	curwin->w_cursor.col = 0;
    }
    अञ्ञथा
	vim_beep(BO_CRSR);
    dont_sync_undo = FALSE;
}

    स्थिर शून्य
ins_s_दक्षिणः(शून्य)
{
    पूर्णाङ्क end_change = dont_sync_undo == FALSE; // end undoable change
#ifdef FEAT_FOLDING
    सचे ((fdo_flags & FDO_HOR) && कुञ्चिका_Typed)
	foldOpenCursor();
#endif
    undisplay_dollar();
    सचे (curwin->w_cursor.lnum < curbuf->b_ml.ml_पङ्क्तिः_गणनम्
	    || gchar_cursor() != NUL)
    {
	start_arrow_with_change(&curwin->w_cursor, end_change);
	सचे (!end_change)
	    AppendCharToRedobuff(K_S_दक्षिणः);
	(शून्य)fwd_word(1L, FALSE, 0);
	curwin->w_set_curswant = true;
    }
    अञ्ञथा
	vim_beep(BO_CRSR);
    dont_sync_undo = FALSE;
}

    स्थिर शून्य
ins_up(
    पूर्णाङ्क		startcol)	// when TRUE move to Insstart.col
{
    pos_T	tpos;
    linenr_T	old_topline = curwin->w_topline;
#ifdef FEAT_DIFF
    पूर्णाङ्क		old_topfill = curwin->w_topfill;
#endif

    undisplay_dollar();
    tpos = curwin->w_cursor;
    सचे (cursor_up(1L, TRUE) == OK)
    {
	सचे (startcol)
	    coladvance(getvcol_nolist(&Insstart));
	सचे (old_topline != curwin->w_topline
#ifdef FEAT_DIFF
		|| old_topfill != curwin->w_topfill
#endif
		)
	    redraw_later(UPD_VALID);
	start_arrow(&tpos);
	can_cindent = TRUE;
    }
    अञ्ञथा
	vim_beep(BO_CRSR);
}

    स्थिर शून्य
ins_pageup(शून्य)
{
    pos_T	tpos;

    undisplay_dollar();

    सचे (mod_mask & MOD_MASK_CTRL)
    {
	// <C-PageUp>: tab page back
	सचे (first_tabpage->tp_next != रिक्त)
	{
	    start_arrow(&curwin->w_cursor);
	    goto_tabpage(-1);
	}
	पटिनिवत्त;
    }

    tpos = curwin->w_cursor;
    सचे (pagescroll(BACKWARD, 1L, FALSE) == OK)
    {
	start_arrow(&tpos);
	can_cindent = TRUE;
    }
    अञ्ञथा
	vim_beep(BO_CRSR);
}

    स्थिर शून्य
ins_down(
    पूर्णाङ्क		startcol)	// when TRUE move to Insstart.col
{
    pos_T	tpos;
    linenr_T	old_topline = curwin->w_topline;
#ifdef FEAT_DIFF
    पूर्णाङ्क		old_topfill = curwin->w_topfill;
#endif

    undisplay_dollar();
    tpos = curwin->w_cursor;
    सचे (cursor_down(1L, TRUE) == OK)
    {
	सचे (startcol)
	    coladvance(getvcol_nolist(&Insstart));
	सचे (old_topline != curwin->w_topline
#ifdef FEAT_DIFF
		|| old_topfill != curwin->w_topfill
#endif
		)
	    redraw_later(UPD_VALID);
	start_arrow(&tpos);
	can_cindent = TRUE;
    }
    अञ्ञथा
	vim_beep(BO_CRSR);
}

    स्थिर शून्य
ins_pagedown(शून्य)
{
    pos_T	tpos;

    undisplay_dollar();

    सचे (mod_mask & MOD_MASK_CTRL)
    {
	// <C-PageDown>: tab page forward
	सचे (first_tabpage->tp_next != रिक्त)
	{
	    start_arrow(&curwin->w_cursor);
	    goto_tabpage(0);
	}
	पटिनिवत्त;
    }

    tpos = curwin->w_cursor;
    सचे (pagescroll(FORWARD, 1L, FALSE) == OK)
    {
	start_arrow(&tpos);
	can_cindent = TRUE;
    }
    अञ्ञथा
	vim_beep(BO_CRSR);
}

#ifdef FEAT_DND
    स्थिर शून्य
ins_drop(शून्य)
{
    do_put('~', रिक्त, BACKWARD, 1L, PUT_CURSEND);
}
#endif

/*
 * Handle TAB in Insert or Replace mode.
 * Return TRUE when the TAB needs to be inserted like a normal character.
 */
    स्थिर पूर्णाङ्क
ins_tab(शून्य)
{
    पूर्णाङ्क		ind;
    पूर्णाङ्क		i;
    पूर्णाङ्क		temp;

    सचे (Insstart_blank_vcol == MAXCOL && curwin->w_cursor.lnum == Insstart.lnum)
	Insstart_blank_vcol = get_nolist_virtcol();
    सचे (echeck_abbr(TAB + ABBR_OFF))
	पटिनिवत्त FALSE;

    ind = inindent(0);
    सचे (ind)
	can_cindent = FALSE;

    /*
     * When nothing special, insert TAB like a normal character.
     */
    सचे (!curbuf->b_p_et
#ifdef FEAT_VARTABS
	    && !(p_sta && ind
		// These five lines mean 'tabstop' != 'shiftwidth'
		&& ((tabstop_गणनम्(curbuf->b_p_vts_array) > 1)
		    || (tabstop_गणनम्(curbuf->b_p_vts_array) == 1
			&& tabstop_first(curbuf->b_p_vts_array)
						       != get_sw_मानम्(curbuf))
		    || (tabstop_गणनम्(curbuf->b_p_vts_array) == 0
			&& curbuf->b_p_ts != get_sw_मानम्(curbuf))))
	    && tabstop_गणनम्(curbuf->b_p_vsts_array) == 0
#अञ्ञथा
	    && !(p_sta && ind && curbuf->b_p_ts != get_sw_मानम्(curbuf))
#endif
	    && get_sts_मानम्() == 0)
	पटिनिवत्त TRUE;

    सचे (stop_arrow() == FAIL)
	पटिनिवत्त TRUE;

    did_ai = FALSE;
    did_si = FALSE;
    can_si = FALSE;
    can_si_back = FALSE;
    AppendToRedobuff((char_u *)"\t");

#ifdef FEAT_VARTABS
    सचे (p_sta && ind)		// insert tab in indent, use 'shiftwidth'
    {
	temp = (पूर्णाङ्क)get_sw_मानम्(curbuf);
	temp -= get_nolist_virtcol() % temp;
    }
    अञ्ञथा सचे (tabstop_गणनम्(curbuf->b_p_vsts_array) > 0 || curbuf->b_p_sts != 0)
				// use 'softtabstop' when set
	temp = tabstop_padding(get_nolist_virtcol(), get_sts_मानम्(),
						     curbuf->b_p_vsts_array);
    अञ्ञथा			// otherwise use 'tabstop'
	temp = tabstop_padding(get_nolist_virtcol(), curbuf->b_p_ts,
						     curbuf->b_p_vts_array);
#अञ्ञथा
    सचे (p_sta && ind)		// insert tab in indent, use 'shiftwidth'
	temp = (पूर्णाङ्क)get_sw_मानम्(curbuf);
    अञ्ञथा सचे (curbuf->b_p_sts != 0) // use 'softtabstop' when set
	temp = (पूर्णाङ्क)get_sts_मानम्();
    अञ्ञथा			// otherwise use 'tabstop'
	temp = (पूर्णाङ्क)curbuf->b_p_ts;
    temp -= get_nolist_virtcol() % temp;
#endif

    /*
     * Insert the first space with ins_char().	It will delete one char in
     * replace mode.  Insert the rest with ins_str(); it will not delete any
     * chars.  For MODE_VREPLACE state, we use ins_char() for all characters.
     */
    ins_char(' ');
    याव (--temp > 0)
    {
	सचे (State & VREPLACE_FLAG)
	    ins_char(' ');
	अञ्ञथा
	{
	    ins_str((char_u *)" ", 1);
	    सचे (State & REPLACE_FLAG)	    // no char replaced
		replace_प्रक्षिप(NUL);
	}
    }

    /*
     * When 'expandtab' not set: Replace spaces by TABs where possible.
     */
#ifdef FEAT_VARTABS
    सचे (!curbuf->b_p_et && (tabstop_गणनम्(curbuf->b_p_vsts_array) > 0
			    || get_sts_मानम्() > 0
			    || (p_sta && ind)))
#अञ्ञथा
    सचे (!curbuf->b_p_et && (get_sts_मानम्() || (p_sta && ind)))
#endif
    {
	char_u		*ptr;
	char_u		*saved_पङ्क्तिः = रिक्त;	// init for GCC
	pos_T		pos;
	pos_T		fpos;
	pos_T		*cursor;
	colnr_T		want_vcol, vcol;
	पूर्णाङ्क		change_col = -1;
	पूर्णाङ्क		save_list = curwin->w_p_list;
	char_u		*tab = (char_u *)"\t";
	chartabsize_T	cts;

	/*
	 * Get the current line.  For MODE_VREPLACE state, don't make real
	 * changes yet, just work on a copy of the line.
	 */
	सचे (State & VREPLACE_FLAG)
	{
	    pos = curwin->w_cursor;
	    cursor = &pos;
	    saved_पङ्क्तिः = vim_strnsave(ml_get_curline(), ml_get_curline_len());
	    सचे (saved_पङ्क्तिः == रिक्त)
		पटिनिवत्त FALSE;
	    ptr = saved_पङ्क्तिः + pos.col;
	}
	अञ्ञथा
	{
	    ptr = ml_get_cursor();
	    cursor = &curwin->w_cursor;
	}

	// When 'L' is not in 'cpoptions' a tab always takes up 'ts' spaces.
	सचे (vim_strchr(p_cpo, CPO_LISTWM) == रिक्त)
	    curwin->w_p_list = FALSE;

	// Find first white before the cursor
	fpos = curwin->w_cursor;
	याव (fpos.col > 0 && VIM_ISWHITE(ptr[-1]))
	{
	    --fpos.col;
	    --ptr;
	}

	// In Replace mode, don't change characters before the insert point.
	सचे ((State & REPLACE_FLAG)
		&& fpos.lnum == Insstart.lnum
		&& fpos.col < Insstart.col)
	{
	    ptr += Insstart.col - fpos.col;
	    fpos.col = Insstart.col;
	}

	// compute virtual column numbers of first white and cursor
	getvcol(curwin, &fpos, &vcol, रिक्त, रिक्त, 0);
	getvcol(curwin, cursor, &want_vcol, रिक्त, रिक्त, 0);

	init_chartabsize_arg(&cts, curwin, 0, vcol, tab, tab);

	// Use as many TABs as possible.  Beware of 'breakindent', 'showbreak'
	// and 'linebreak' adding extra virtual columns.
	याव (VIM_ISWHITE(*ptr))
	{
	    i = lbr_chartabsize(&cts);
	    सचे (cts.cts_vcol + i > want_vcol)
		भङ्ग;
	    सचे (*ptr != TAB)
	    {
		*ptr = TAB;
		सचे (change_col < 0)
		{
		    change_col = fpos.col;  // Column of first change
		    // May have to adjust Insstart
		    सचे (fpos.lnum == Insstart.lnum && fpos.col < Insstart.col)
			Insstart.col = fpos.col;
		}
	    }
	    ++fpos.col;
	    ++ptr;
	    cts.cts_vcol += i;
	}
	vcol = cts.cts_vcol;
	clear_chartabsize_arg(&cts);

	सचे (change_col >= 0)
	{
	    पूर्णाङ्क		    repl_off = 0;

	    // Skip over the spaces we need.
	    init_chartabsize_arg(&cts, curwin, 0, vcol, ptr, ptr);
	    याव (cts.cts_vcol < want_vcol && *cts.cts_ptr == ' ')
	    {
		cts.cts_vcol += lbr_chartabsize(&cts);
		++cts.cts_ptr;
		++repl_off;
	    }
	    ptr = cts.cts_ptr;
	    vcol = cts.cts_vcol;
	    clear_chartabsize_arg(&cts);

	    सचे (vcol > want_vcol)
	    {
		// Must have a char with 'showbreak' just before it.
		--ptr;
		--repl_off;
	    }
	    fpos.col += repl_off;

	    // Delete following spaces.
	    i = cursor->col - fpos.col;
	    सचे (i > 0)
	    {
#ifdef FEAT_PROP_POPUP
		सचे (!(State & VREPLACE_FLAG))
		{
		    char_u  *newp;
		    पूर्णाङ्क	    col;

		    newp = alloc(curbuf->b_ml.ml_पङ्क्तिः_len - i);
		    सचे (newp == रिक्त)
			पटिनिवत्त FALSE;

		    col = ptr - curbuf->b_ml.ml_पङ्क्तिः_ptr;
		    सचे (col > 0)
			mch_memmove(newp, ptr - col, col);
		    mch_memmove(newp + col, ptr + i,
					   curbuf->b_ml.ml_पङ्क्तिः_len - col - i);

		    सचे (curbuf->b_ml.ml_flags & (ML_पङ्क्तिः_DIRTY | ML_ALLOCATED))
			vim_free(curbuf->b_ml.ml_पङ्क्तिः_ptr);
		    curbuf->b_ml.ml_पङ्क्तिः_ptr = newp;
		    curbuf->b_ml.ml_पङ्क्तिः_len -= i;
		    curbuf->b_ml.ml_पङ्क्तिः_textlen = 0;
		    curbuf->b_ml.ml_flags =
			   (curbuf->b_ml.ml_flags | ML_पङ्क्तिः_DIRTY) & ~ML_EMPTY;
		}
		अञ्ञथा
#endif
		    STRMOVE(ptr, ptr + i);
		// correct replace stack.
		सचे ((State & REPLACE_FLAG) && !(State & VREPLACE_FLAG))
		    अत्थाय (temp = i; --temp >= 0; )
			replace_join(repl_off);
	    }
#ifdef FEAT_NETBEANS_INTG
	    सचे (netbeans_active())
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
	    सचे (State & VREPLACE_FLAG)
	    {
		// Backspace from real cursor to change_col
		backspace_until_column(change_col);

		// Insert each char in saved_line from changed_col to
		// ptr-cursor
		ins_bytes_len(saved_पङ्क्तिः + change_col,
						    cursor->col - change_col);
	    }
	}

	सचे (State & VREPLACE_FLAG)
	    vim_free(saved_पङ्क्तिः);
	curwin->w_p_list = save_list;
    }

    पटिनिवत्त FALSE;
}

/*
 * Handle CR or NL in insert mode.
 * Return FAIL when out of memory or can't undo.
 */
    पूर्णाङ्क
ins_eol(पूर्णाङ्क c)
{
    पूर्णाङ्क	    i;

    सचे (echeck_abbr(c + ABBR_OFF))
	पटिनिवत्त OK;
    सचे (stop_arrow() == FAIL)
	पटिनिवत्त FAIL;
    undisplay_dollar();

    /*
     * Strange Vi behaviour: In Replace mode, typing a NL will not delete the
     * character under the cursor.  Only push a NUL on the replace stack,
     * nothing to put back when the NL is deleted.
     */
    सचे ((State & REPLACE_FLAG) && !(State & VREPLACE_FLAG))
	replace_प्रक्षिप(NUL);

    /*
     * In MODE_VREPLACE state, a NL replaces the rest of the line, and starts
     * replacing the next line, so we push all of the characters left on the
     * line onto the replace stack.  This is not done here though, it is done
     * in open_line().
     */

    // Put cursor on NUL if on the last char and coladd is 1 (happens after
    // CTRL-O).
    सचे (virtual_active() && curwin->w_cursor.coladd > 0)
	coladvance(getviscol());

#ifdef FEAT_RIGHTLEFT
    // NL in reverse insert will always start in the end of
    // current line.
    सचे (revins_on)
	curwin->w_cursor.col += ml_get_cursor_len();
#endif

    AppendToRedobuff(NL_STR);
    i = open_पङ्क्तिः(FORWARD,
	    has_format_option(FO_RET_COMS) ? OPENLINE_DO_COM : 0, old_indent,
	    रिक्त);
    old_indent = 0;
    can_cindent = TRUE;
#ifdef FEAT_FOLDING
    // When inserting a line the cursor line must never be in a closed fold.
    foldOpenCursor();
#endif

    पटिनिवत्त i;
}

#ifdef FEAT_DIGRAPHS
/*
 * Handle digraph in insert mode.
 * Returns character still to be inserted, or NUL when nothing remaining to be
 * done.
 */
    स्थिर पूर्णाङ्क
ins_digraph(शून्य)
{
    पूर्णाङ्क	    c;
    पूर्णाङ्क	    cc;
    पूर्णाङ्क	    did_putchar = FALSE;

    pc_status = PC_STATUS_UNSET;
    सचे (redrawing() && !char_avail())
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
    ++allow_कुञ्चिकाः;
    c = plain_vgetc();
    --no_mapping;
    --allow_कुञ्चिकाः;
    सचे (did_putchar)
	// when the line fits in 'columns' the '?' is at the start of the next
	// line and will not be removed by the redraw
	edit_unputchar();

    सचे (IS_SPECIAL(c) || mod_mask)	    // special key
    {
	clear_showcmd();
	निवेशय_special(c, TRUE, FALSE);
	पटिनिवत्त NUL;
    }
    सचे (c != ESC)
    {
	did_putchar = FALSE;
	सचे (redrawing() && !char_avail())
	{
	    // may need to redraw when no more chars available now
	    ins_redraw(FALSE);

	    सचे (char2cells(c) == 1)
	    {
		ins_redraw(FALSE);
		edit_putchar(c, TRUE);
		did_putchar = TRUE;
	    }
	    add_to_showcmd_c(c);
	}
	++no_mapping;
	++allow_कुञ्चिकाः;
	cc = plain_vgetc();
	--no_mapping;
	--allow_कुञ्चिकाः;
	सचे (did_putchar)
	    // when the line fits in 'columns' the '?' is at the start of the
	    // next line and will not be removed by a redraw
	    edit_unputchar();
	सचे (cc != ESC)
	{
	    AppendToRedobuff((char_u *)CTRL_V_STR);
	    c = digraph_get(c, cc, TRUE);
	    clear_showcmd();
	    पटिनिवत्त c;
	}
    }
    clear_showcmd();
    पटिनिवत्त NUL;
}
#endif

/*
 * Handle CTRL-E and CTRL-Y in Insert mode: copy char from other line.
 * Returns the char to be inserted, or NUL if none found.
 */
    पूर्णाङ्क
ins_copychar(linenr_T lnum)
{
    पूर्णाङ्क		    c;
    char_u	    *ptr, *prev_ptr;
    char_u	    *पङ्क्तिः;
    chartabsize_T   cts;

    सचे (lnum < 1 || lnum > curbuf->b_ml.ml_पङ्क्तिः_गणनम्)
    {
	vim_beep(BO_COPY);
	पटिनिवत्त NUL;
    }

    // try to advance to the cursor column
    validate_virtcol();
    पङ्क्तिः = ml_get(lnum);
    prev_ptr = पङ्क्तिः;
    init_chartabsize_arg(&cts, curwin, lnum, 0, पङ्क्तिः, पङ्क्तिः);
    याव (cts.cts_vcol < curwin->w_virtcol && *cts.cts_ptr != NUL)
    {
	prev_ptr = cts.cts_ptr;
	cts.cts_vcol += lbr_chartabsize_adv(&cts);
    }
    सचे (cts.cts_vcol > curwin->w_virtcol)
	ptr = prev_ptr;
    अञ्ञथा
	ptr = cts.cts_ptr;
    clear_chartabsize_arg(&cts);

    c = (*mb_ptr2char)(ptr);
    सचे (c == NUL)
	vim_beep(BO_COPY);
    पटिनिवत्त c;
}

/*
 * CTRL-Y or CTRL-E typed in Insert mode.
 */
    स्थिर पूर्णाङ्क
ins_ctrl_ey(पूर्णाङ्क tc)
{
    पूर्णाङ्क	    c = tc;

    सचे (ctrl_x_mode_scroll())
    {
	सचे (c == Ctrl_Y)
	    scrolldown_clamp();
	अञ्ञथा
	    scrollup_clamp();
	redraw_later(UPD_VALID);
    }
    अञ्ञथा
    {
	c = ins_copychar(curwin->w_cursor.lnum + (c == Ctrl_Y ? -1 : 1));
	सचे (c != NUL)
	{
	    long	tw_save;

	    // The character must be taken literally, insert like it
	    // was typed after a CTRL-V, and pretend 'textwidth'
	    // wasn't set.  Digits, 'o' and 'x' are special after a
	    // CTRL-V, don't use it for these.
	    सचे (c < 256 && !SAFE_isalnum(c))
		AppendToRedobuff((char_u *)CTRL_V_STR);	// CTRL-V
	    tw_save = curbuf->b_p_tw;
	    curbuf->b_p_tw = -1;
	    निवेशय_special(c, TRUE, FALSE);
	    curbuf->b_p_tw = tw_save;
#ifdef FEAT_RIGHTLEFT
	    revins_chars++;
	    revins_legal++;
#endif
	    c = Ctrl_V;	// pretend CTRL-V is last character
	    auto_format(FALSE, TRUE);
	}
    }
    पटिनिवत्त c;
}

/*
 * Get the value that w_virtcol would have when 'list' is off.
 * Unless 'cpo' contains the 'L' flag.
 */
    colnr_T
get_nolist_virtcol(शून्य)
{
    // check validity of cursor in current buffer
    सचे (curwin->w_buffer == रिक्त
	|| curwin->w_buffer->b_ml.ml_mfp == रिक्त
	|| curwin->w_cursor.lnum > curwin->w_buffer->b_ml.ml_पङ्क्तिः_गणनम्)
	पटिनिवत्त 0;
    सचे (curwin->w_p_list && vim_strchr(p_cpo, CPO_LISTWM) == रिक्त)
	पटिनिवत्त getvcol_nolist(&curwin->w_cursor);
    validate_virtcol();
    पटिनिवत्त curwin->w_virtcol;
}

#सचे defined(FEAT_EVAL)
/*
 * Handle the InsertCharPre autocommand.
 * "c" is the character that was typed.
 * Return a pointer to allocated memory with the replacement string.
 * Return NULL to continue inserting "c".
 */
    स्थिर char_u *
do_निवेशय_char_pre(पूर्णाङ्क c)
{
    char_u	*res;
    char_u	buf[MB_MAXBYTES + 1];
    size_t	buflen;
    पूर्णाङ्क		save_State = State;

    // Return quickly when there is nothing to do.
    सचे (!has_insertcharpre())
	पटिनिवत्त रिक्त;

    सचे (c == Ctrl_RSB)
	पटिनिवत्त रिक्त;

    सचे (has_mbyte)
	buflen = (*mb_char2bytes)(c, buf);
    अञ्ञथा
    {
	buf[0] = c;
	buflen = 1;
    }
    buf[buflen] = NUL;

    // Lock the text to avoid weird things from happening.
    ++textlock;
    set_vim_var_string(VV_CHAR, buf, (पूर्णाङ्क)buflen);  // set v:char

    res = रिक्त;
    सचे (ins_apply_autocmds(EVENT_INSERTCHARPRE))
    {
	// Get the value of v:char.  It may be empty or more than one
	// character.  Only use it when changed, otherwise continue with the
	// original character to avoid breaking autoindent.
	सचे (STRCMP(buf, get_vim_var_str(VV_CHAR)) != 0)
	    res = vim_strsave(get_vim_var_str(VV_CHAR));
    }

    set_vim_var_string(VV_CHAR, रिक्त, -1);  // clear v:char
    --textlock;

    // Restore the State, it may have been changed.
    State = save_State;

    पटिनिवत्त res;
}
#endif

    पूर्णाङ्क
get_can_cindent(शून्य)
{
    पटिनिवत्त can_cindent;
}

    शून्य
set_can_cindent(पूर्णाङ्क val)
{
    can_cindent = val;
}

/*
 * Trigger "event" and take care of fixing undo.
 */
    पूर्णाङ्क
ins_apply_autocmds(event_T event)
{
    varnumber_T	tick = CHANGEDTICK(curbuf);
    पूर्णाङ्क r;

    r = apply_autocmds(event, रिक्त, रिक्त, FALSE, curbuf);

    // If u_savesub() was called then we are not prepared to start
    // a new line.  Call u_save() with no contents to fix that.
    // Except when leaving Insert mode.
    सचे (event != EVENT_INSERTLEAVE && tick != CHANGEDTICK(curbuf))
	u_save(curwin->w_cursor.lnum, (linenr_T)(curwin->w_cursor.lnum + 1));

    पटिनिवत्त r;
}
