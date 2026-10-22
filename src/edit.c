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
#define BACKSPACE_ओळ		    4

// Set when doing something for completion that may call edit() recursively,
// which is not allowed.
स्थिर पूर्णांक	compl_busy = FALSE;


स्थिर शून्य ins_ctrl_v(शून्य);
स्थिर शून्य घाला_special(पूर्णांक, पूर्णांक, पूर्णांक);
स्थिर शून्य redo_literal(पूर्णांक c);
स्थिर शून्य start_arrow_common(pos_T *end_घाला_pos, पूर्णांक change);
#ifdef FEAT_SPELL
स्थिर शून्य check_spell_redraw(शून्य);
#endif
स्थिर शून्य stop_घाला(pos_T *end_घाला_pos, पूर्णांक esc, पूर्णांक nomove);
स्थिर पूर्णांक  echeck_abbr(पूर्णांक);
स्थिर शून्य mb_replace_काढा_ins(पूर्णांक cc);
स्थिर शून्य replace_flush(शून्य);
स्थिर शून्य replace_do_bs(पूर्णांक limit_col);
स्थिर पूर्णांक del_char_after_col(पूर्णांक limit_col);
स्थिर शून्य ins_reg(शून्य);
स्थिर शून्य ins_ctrl_g(शून्य);
स्थिर शून्य ins_ctrl_hat(शून्य);
स्थिर पूर्णांक  ins_esc(दीर्घ *मोजणी, पूर्णांक cmdchar, पूर्णांक nomove);
#ifdef FEAT_RIGHTLEFT
स्थिर शून्य ins_ctrl_(शून्य);
#endif
स्थिर पूर्णांक ins_start_select(पूर्णांक c);
स्थिर शून्य ins_घाला(पूर्णांक replaceState);
स्थिर शून्य ins_ctrl_o(शून्य);
स्थिर शून्य ins_shift(पूर्णांक c, पूर्णांक lastc);
स्थिर शून्य ins_del(शून्य);
स्थिर पूर्णांक  ins_bs(पूर्णांक c, पूर्णांक mode, पूर्णांक *inserted_space_p);
#जर defined(FEAT_GUI_TABLINE)
स्थिर शून्य ins_tabline(पूर्णांक c);
#endif
स्थिर शून्य ins_डावा(शून्य);
स्थिर शून्य ins_home(पूर्णांक c);
स्थिर शून्य ins_end(पूर्णांक c);
स्थिर शून्य ins_s_डावा(शून्य);
स्थिर शून्य ins_उजवा(शून्य);
स्थिर शून्य ins_s_उजवा(शून्य);
स्थिर शून्य ins_up(पूर्णांक startcol);
स्थिर शून्य ins_pageup(शून्य);
स्थिर शून्य ins_down(पूर्णांक startcol);
स्थिर शून्य ins_pagedown(शून्य);
#ifdef FEAT_DND
स्थिर शून्य ins_drop(शून्य);
#endif
स्थिर पूर्णांक  ins_tab(शून्य);
#ifdef FEAT_DIGRAPHS
स्थिर पूर्णांक  ins_digraph(शून्य);
#endif
स्थिर पूर्णांक  ins_ctrl_ey(पूर्णांक tc);
#जर defined(FEAT_EVAL)
स्थिर char_u *do_घाला_char_pre(पूर्णांक c);
#endif

स्थिर colnr_T	Insstart_textlen;	// length of line when insert started
स्थिर colnr_T	Insstart_blank_vcol;	// vcol for first inserted blank
स्थिर पूर्णांक	update_Insstart_orig = TRUE; // set Insstart_orig to Insstart

स्थिर string_T	last_घाला = {रिक्त, 0};    // the text of the previous insert,
					    // K_SPECIAL and CSI are escaped
स्थिर पूर्णांक	last_घाला_skip; // nr of chars in front of previous insert
स्थिर पूर्णांक	new_घाला_skip;  // nr of chars in front of current insert
स्थिर पूर्णांक	did_restart_edit;	// "restart_edit" when calling edit()

स्थिर पूर्णांक	can_cindent;		// may do cindenting on this line

#ifdef FEAT_RIGHTLEFT
स्थिर पूर्णांक	revins_on;		// reverse insert mode on
स्थिर पूर्णांक	revins_chars;		// how much to skip after edit
स्थिर पूर्णांक	revins_legal;		// was the last char 'legal'?
स्थिर पूर्णांक	revins_scol;		// start column of revins session
#endif

स्थिर पूर्णांक	ins_need_undo;		// call u_save() before inserting a
					// char.  Set when edit() is called.
					// after that arrow_used is used.

स्थिर पूर्णांक	dont_sync_undo = FALSE;	// CTRL-G U prevents syncing undo for
					// the next left/right cursor key

// With 'autocompletedelay' set, arm the delay and let the main loop fire
// Insert-mode autocommands; the popup is shown later on K_COMPLETE_DELAY.
// Otherwise trigger completion right away.
#define TRIGGER_AUTOCOMPLETE()			\
    करा {					\
	update_screen(UPD_VALID);  /* Show char (deletion) immediately */ \
	out_flush();				\
	ins_compl_enable_autocomplete();	\
	ins_compl_arm_autostart();		\
	जर (!ins_compl_arm_autocomplete_delay())\
	    जा docomplete;			\
    } पर्यंत (0)

#define MAY_TRIGGER_AUTOCOMPLETE(c)				\
    करा {							\
	जर (ins_compl_has_autocomplete() && !char_avail()	\
		&& curwin->w_cursor.col > 0)			\
	{							\
	    (c) = char_before_cursor();				\
	    जर (vim_isprintc(c))				\
		TRIGGER_AUTOCOMPLETE();				\
	}							\
    } पर्यंत (0)

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
    पूर्णांक
edit(
    पूर्णांक		cmdchar,
    पूर्णांक		startln,	// if set, insert at start of line
    दीर्घ	मोजणी)
{
    पूर्णांक		c = 0;
    char_u	*ptr;
    पूर्णांक		lastc = 0;
    पूर्णांक		mincol;
    स्थिर linenr_T o_lnum = 0;
    पूर्णांक		i;
    पूर्णांक		did_backspace = TRUE;	    // previous char was backspace
    पूर्णांक		ओळ_is_white = FALSE;	    // line is empty before insert
    linenr_T	old_topline = 0;	    // topline before insertion
#ifdef FEAT_DIFF
    पूर्णांक		old_topfill = -1;
#endif
    पूर्णांक		inserted_space = FALSE;     // just inserted a space
    पूर्णांक		replaceState = MODE_REPLACE;
    पूर्णांक		nomove = FALSE;		    // don't move cursor on return
#ifdef FEAT_JOB_CHANNEL
    पूर्णांक		cmdchar_todo = cmdchar;
#endif
#ifdef FEAT_CONCEAL
    पूर्णांक		cursor_ओळ_was_concealed;
#endif
    पूर्णांक		ins_just_started = TRUE;

    // Remember whether editing was restarted after CTRL-O.
    did_restart_edit = restart_edit;

    // sleep before redrawing, needed for "CTRL-O :" that results in an
    // error message
    check_for_delay(TRUE);

    // set Insstart_orig to Insstart
    update_Insstart_orig = TRUE;

#ifdef HAVE_SANDBOX
    // Don't allow inserting in the sandbox.
    जर (sandbox != 0)
    {
	emsg(_(e_not_allowed_in_sandbox));
	परत FALSE;
    }
#endif
    // Don't allow changes in the buffer while editing the cmdline.  The
    // caller of getcmdline() may get confused.
    // Don't allow recursive insert mode when busy with completion.
    जर (textlock != 0 || ins_compl_active() || compl_busy || pum_visible())
    {
	emsg(_(e_not_allowed_to_change_text_or_change_window));
	परत FALSE;
    }
    ins_compl_clear();	    // clear stuff for CTRL-X mode

    /*
     * Trigger InsertEnter autocommands.  Do not do this for "r<CR>" or "grx".
     */
    जर (cmdchar != 'r' && cmdchar != 'v')
    {
	pos_T   save_cursor = curwin->w_cursor;

#ifdef FEAT_EVAL
	जर (cmdchar == 'R')
	    ptr = (char_u *)"r";
	अन्यथा जर (cmdchar == 'V')
	    ptr = (char_u *)"v";
	अन्यथा
	    ptr = (char_u *)"i";
	set_vim_var_string(VV_INSERTMODE, ptr, 1);
	set_vim_var_string(VV_CHAR, रिक्त, -1);  // clear v:char
#endif
	ins_apply_autocmds(EVENT_INSERTENTER);

	// Check for changed highlighting, e.g. for ModeMsg.
	जर (need_highlight_changed)
	    highlight_changed();

	// Make sure the cursor didn't move.  Do call check_cursor_col() in
	// case the text was modified.  Since Insert mode was not started yet
	// a call to check_cursor_col() may move the cursor, especially with
	// the "A" command, thus set State to avoid that. Also check that the
	// line number is still valid (lines may have been deleted).
	// Do not restore if v:char was set to a non-empty string.
	जर (!EQUAL_POS(curwin->w_cursor, save_cursor)
#ifdef FEAT_EVAL
		&& *get_vim_var_str(VV_CHAR) == NUL
#endif
		&& save_cursor.lnum <= curbuf->b_ml.ml_ओळ_मोजणी)
	{
	    पूर्णांक save_state = State;

	    curwin->w_cursor = save_cursor;
	    State = MODE_घाला;
	    check_cursor_col();
	    State = save_state;
	}
    }

#ifdef FEAT_CONCEAL
    // Check if the cursor line was concealed before changing State.
    cursor_ओळ_was_concealed = curwin->w_p_cole > 0
						&& conceal_cursor_ओळ(curwin);
#endif

    /*
     * When doing a paste with the middle mouse button, Insstart is set to
     * where the paste started.
     */
    जर (where_paste_started.lnum != 0)
	Insstart = where_paste_started;
    अन्यथा
    {
	Insstart = curwin->w_cursor;
	जर (startln)
	    Insstart.col = 0;
    }
    Insstart_textlen = (colnr_T)linetabsize_str(ml_get_curline());
    Insstart_blank_vcol = MAXCOL;
    जर (!did_ai)
	ai_col = 0;

    जर (cmdchar != NUL && restart_edit == 0)
    {
	ResetRedobuff();
	Append_संख्या_To_Redobuff(मोजणी);
	जर (cmdchar == 'V' || cmdchar == 'v')
	{
	    // "gR" or "gr" command
	    AppendCharToRedobuff('g');
	    AppendCharToRedobuff((cmdchar == 'v') ? 'r' : 'R');
	}
	अन्यथा
	{
	    जर (cmdchar == K_PS)
		AppendCharToRedobuff('a');
	    अन्यथा
		AppendCharToRedobuff(cmdchar);
	    जर (cmdchar == 'g')		    // "gI" command
		AppendCharToRedobuff('I');
	    अन्यथा जर (cmdchar == 'r')	    // "r<CR>" command
		मोजणी = 1;		    // insert only one <CR>
	}
    }

    जर (cmdchar == 'R')
    {
	State = MODE_REPLACE;
    }
    अन्यथा जर (cmdchar == 'V' || cmdchar == 'v')
    {
	State = MODE_VREPLACE;
	replaceState = MODE_VREPLACE;
	orig_ओळ_मोजणी = curbuf->b_ml.ml_ओळ_मोजणी;
	vr_ओळी_changed = 1;
    }
    अन्यथा
	State = MODE_घाला;

    may_trigger_modechanged();
    stop_घाला_mode = FALSE;

#ifdef FEAT_CONCEAL
    // Check if the cursor line needs redrawing after changing State.  If
    // 'concealcursor' is "n" it needs to be redrawn without concealing.
    conceal_check_cursor_ओळ(cursor_ओळ_was_concealed);
#endif

    // Need to position cursor again when on a TAB and when on a char with
    // virtual text.
    जर (gchar_cursor() == TAB
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
    जर (curbuf->b_p_iminsert == B_IMODE_LMAP)
	State |= MODE_LANGMAP;
#ifdef HAVE_आदान_METHOD
    im_set_active(curbuf->b_p_iminsert == B_IMODE_IM);
#endif

    setmouse();
    clear_showcmd();
#ifdef FEAT_RIGHTLEFT
    // there is no reverse replace mode
    revins_on = (State == MODE_घाला && p_ri);
    जर (revins_on)
	undisplay_dollar();
    revins_chars = 0;
    revins_legal = 0;
    revins_scol = -1;
#endif
    जर (!p_ek)
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
    जर (restart_edit != 0 && stuff_empty())
    {
	/*
	 * After a paste we consider text typed to be part of the insert for
	 * the pasted text. You can backspace over the pasted text too.
	 */
	जर (where_paste_started.lnum)
	    arrow_used = FALSE;
	अन्यथा
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
	जर (((ins_at_eol && curwin->w_cursor.lnum == o_lnum)
		    || curwin->w_curswant > curwin->w_virtcol)
		&& *(ptr = ml_get_curline() + curwin->w_cursor.col) != NUL)
	{
	    जर (ptr[1] == NUL)
		++curwin->w_cursor.col;
	    अन्यथा जर (has_mbyte)
	    {
		i = (*mb_ptr2len)(ptr);
		जर (ptr[i] == NUL)
		    curwin->w_cursor.col += i;
	    }
	}
	ins_at_eol = FALSE;
    }
    अन्यथा
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
    जर (!p_im && did_restart_edit == 0)
	foldOpenCursor();
#endif

    /*
     * If 'showmode' is set, show the current (insert/replace/..) mode.
     * A warning message for changing a readonly file is given here, before
     * actually changing anything.  It's put after the mode, if any.
     */
    i = 0;
    जर (p_smd && संदेश_silent == 0)
	i = showmode();

    जर (!p_im && did_restart_edit == 0)
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
    new_घाला_skip = (पूर्णांक)inserted.length;
    जर (inserted.string != रिक्त)
	vim_free(inserted.string);

    old_indent = 0;

    /*
     * Main loop in Insert mode: repeat until Insert mode is left.
     */
    साठी (;;)
    {
#ifdef FEAT_RIGHTLEFT
	जर (!revins_legal)
	    revins_scol = -1;	    // reset on illegal motions
	अन्यथा
	    revins_legal = 0;
#endif
	जर (arrow_used)	    // don't repeat insert when arrow key used
	    मोजणी = 0;

	जर (update_Insstart_orig)
	    Insstart_orig = Insstart;

	जर (stop_घाला_mode && !ins_compl_active())
	{
	    // ":stopinsert" used or 'insertmode' reset
	    मोजणी = 0;
	    जा doESCkey;
	}

	// set curwin->w_curswant for next K_DOWN or K_UP
	जर (!arrow_used)
	    curwin->w_set_curswant = true;

	// If there is no typeahead may check for timestamps (e.g., for when a
	// menu invoked a shell command).
	जर (stuff_empty())
	{
	    did_check_timestamps = FALSE;
	    जर (need_check_timestamps)
		check_timestamps(FALSE);
	}

	/*
	 * When emsg() was called msg_scroll will have been set.
	 */
	संदेश_scroll = FALSE;

#ifdef FEAT_GUI
	// When 'mousefocus' is set a mouse movement may have taken us to
	// another window.  "need_mouse_correct" may then be set because of an
	// autocommand.
	जर (need_mouse_correct)
	    gui_mouse_correct();
#endif

#ifdef FEAT_FOLDING
	// Open fold at the cursor line, according to 'foldopen'.
	जर (fdo_flags & FDO_घाला)
	    foldOpenCursor();
	// Close folds where the cursor isn't, according to 'foldclose'
	जर (!char_avail())
	    foldCheckClose();
#endif

#ifdef FEAT_JOB_CHANNEL
	जर (bt_prompt(curbuf))
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
	जर (curbuf->b_mod_set
		&& curwin->w_p_wrap
		&& !curwin->w_p_sms
		&& !did_backspace
		&& curwin->w_topline == old_topline
#ifdef FEAT_DIFF
		&& curwin->w_topfill == old_topfill
#endif
		&& मोजणी <= 1
		)
	{
	    mincol = curwin->w_wcol;
	    validate_cursor_col();

	    जर (
#ifdef FEAT_VARTABS
		curwin->w_wcol < mincol - tabstop_at(get_nolist_virtcol(),
						     curbuf->b_p_ts,
						     curbuf->b_p_vts_array,
						     FALSE)
#अन्यथा
		(पूर्णांक)curwin->w_wcol < mincol - curbuf->b_p_ts
#endif
		    && curwin->w_wrow ==
				   curwin->w_height - 1 - get_scrolloff_मूल्य()
		    && (curwin->w_cursor.lnum != curwin->w_topline
#ifdef FEAT_DIFF
			|| curwin->w_topfill > 0
#endif
		    ))
	    {
#ifdef FEAT_DIFF
		जर (curwin->w_topfill > 0)
		    --curwin->w_topfill;
		अन्यथा
#endif
#ifdef FEAT_FOLDING
		जर (hasFolding(curwin->w_topline, रिक्त, &old_topline))
		    set_topline(curwin, old_topline + 1);
		अन्यथा
#endif
		    set_topline(curwin, curwin->w_topline + 1);
	    }
	}

	// May need to adjust w_topline to show the cursor.
	जर (मोजणी <= 1)
	    update_topline();

	did_backspace = FALSE;

	जर (मोजणी <= 1)
	    validate_cursor();		// may set must_redraw

	/*
	 * Redraw the display when no characters are waiting.
	 * Also shows mode, ruler and positions cursor.
	 */
	ins_redraw(TRUE);

	जर (curwin->w_p_scb)
	    do_check_scrollbind(TRUE);

	जर (curwin->w_p_crb)
	    do_check_cursorbind();
	जर (मोजणी <= 1)
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
	जर (c != K_CURSORHOLD && c != K_COMPLETE_DELAY)
	    lastc = c;		// remember the previous char for CTRL-D

	// After using CTRL-G U the next cursor key will not break undo.
	जर (dont_sync_undo == MAYBE)
	    dont_sync_undo = TRUE;
	अन्यथा
	    dont_sync_undo = FALSE;
	जर (cmdchar == K_PS)
	    // Got here from normal mode when bracketed paste started.
	    c = K_PS;
	अन्यथा
	{
	    // Trigger autocomplete when entering Insert mode, either directly
	    // or via change commands like 'ciw', 'cw', etc., before the first
	    // character is typed.
	    जर (ins_just_started)
	    {
		ins_just_started = FALSE;
		जर (ins_compl_has_autocomplete() && !char_avail()
			&& curwin->w_cursor.col > 0)
		{
		    c = char_before_cursor();
		    जर (vim_isprintc(c))
		    {
			ins_compl_enable_autocomplete();
			ins_compl_arm_autostart();
			ins_compl_init_get_longest();
#ifdef FEAT_RIGHTLEFT
			जर (p_hkmap)
			    c = hkmap(c);		// Hebrew mode mapping
#endif
			// Defer until the delay expires (K_COMPLETE_DELAY), or
			// trigger now when no delay is in effect.
			जर (!ins_compl_arm_autocomplete_delay())
			    जा docomplete;
		    }
		}
	    }

	    करा
	    {
		c = safe_vgetc();

		जर (stop_घाला_mode
#ifdef FEAT_TERMINAL
			|| (c == K_IGNORE && term_use_loop())
#endif
		   )
		{
		    // Insert mode ended, possibly from a callback, or a timer
		    // must have opened a terminal window.
		    जर (c != K_IGNORE && c != K_NOP)
			vungetc(c);
		    मोजणी = 0;

		    जर (!bt_prompt(curwin->w_buffer)
#ifdef FEAT_TERMINAL
			    && !bt_terminal(curwin->w_buffer)
#endif
			    && stop_घाला_mode)
			// :stopinsert command via callback or via server command
			nomove = FALSE;
		    अन्यथा
			nomove = TRUE;
		    ins_compl_prep(ESC);
		    जा doESCkey;
		}
	    } पर्यंत (c == K_IGNORE || c == K_NOP);
	}

	// Don't want K_CURSORHOLD for the second key, e.g., after CTRL-V.
	did_cursorhold = TRUE;
	जर (c != K_CURSORHOLD && c != K_COMPLETE_DELAY)
	{
	    // Don't want delayed autocompletion from the previous key either.
	    ins_compl_clear_autocomplete_delay();
	    ins_compl_disarm_autostart();
	    // A completion already on screen goes on being what it was.
	    जर (!ins_compl_active())
		ins_compl_disable_autocomplete();
	}

#ifdef FEAT_RIGHTLEFT
	जर (p_hkmap && किल्ली_Typed)
	    c = hkmap(c);		// Hebrew mode mapping
#endif

	// If the window was made so small that nothing shows, make it at least
	// one line and one column when typing.
	जर (किल्ली_Typed && !किल्ली_Stuffed)
	    win_ensure_size();

	/*
	 * Special handling of keys while the popup menu is visible or wanted
	 * and the cursor is still in the completed word.  Only when there is
	 * a match, skip this when no matches were found.
	 */
	जर (ins_compl_active() && curwin->w_cursor.col >= ins_compl_col()
		&& ins_compl_has_shown_match() && pum_wanted())
	{
	    // BS: Delete one character from "compl_leader".
	    जर ((c == K_BS || c == Ctrl_H)
			&& curwin->w_cursor.col > ins_compl_col()
			&& (c = ins_compl_bs()) == NUL)
		सुरूठेवा;

	    // When no match was selected or it was edited.
	    जर (!ins_compl_used_match())
	    {
		// CTRL-L: Add one character from the current match to
		// "compl_leader".  Except when at the original match and
		// there is nothing to add, CTRL-L works like CTRL-P then.
		जर (c == Ctrl_L
			&& (!ctrl_x_mode_ओळ_or_eval()
			    || ins_compl_long_shown_match()))
		{
		    ins_compl_addfrommatch();
		    सुरूठेवा;
		}

		// A non-white character that fits in with the current
		// completion: Add to "compl_leader".
		जर (ins_compl_accept_char(c))
		{
#जर defined(FEAT_EVAL)
		    // Trigger InsertCharPre.
		    char_u *str = do_घाला_char_pre(c);
		    char_u *p;

		    जर (str != रिक्त)
		    {
			साठी (p = str; *p != NUL; MB_PTR_ADV(p))
			    ins_compl_addleader(PTR2CHAR(p));
			vim_free(str);
		    }
		    अन्यथा
#endif
			ins_compl_addleader(c);
		    सुरूठेवा;
		}

		// Pressing CTRL-Y selects the current match.  When
		// ins_compl_enter_selects() is set the Enter key does the
		// same.
		जर ((c == Ctrl_Y || (ins_compl_enter_selects()
				    && (c == CAR || c == K_KENTER || c == NL)))
			&& stop_arrow() == OK)
		{
		    ins_compl_delete();
		    जर (ins_compl_preinsert_longest()
			    && !ins_compl_is_match_selected())
		    {
			ins_compl_घाला(FALSE, TRUE);
			ins_compl_init_get_longest();
			सुरूठेवा;
		    }
		    अन्यथा
			ins_compl_घाला(FALSE, FALSE);
		}
		// Delete preinserted text when typing special chars
		अन्यथा जर (IS_WHITE_NL_OR_NUL(c) && ins_compl_preinsert_effect())
		    ins_compl_delete();
	    }
	}

	// Prepare for or stop CTRL-X mode.  This doesn't do completion, but
	// it does fix up the text when finishing completion.
	ins_compl_init_get_longest();
	जर (ins_compl_prep(c))
	    सुरूठेवा;

	// CTRL-\ CTRL-N goes to Normal mode,
	// CTRL-\ CTRL-G goes to mode selected with 'insertmode',
	// CTRL-\ CTRL-O is like CTRL-O but without moving the cursor.
	जर (c == Ctrl_BSL)
	{
	    // may need to redraw when no more chars available now
	    ins_redraw(FALSE);
	    ++no_mapping;
	    ++allow_किल्ल्या;
	    c = plain_vgetc();
	    --no_mapping;
	    --allow_किल्ल्या;
	    जर (c != Ctrl_N && c != Ctrl_G && c != Ctrl_O)
	    {
		// it's something else
		vungetc(c);
		c = Ctrl_BSL;
	    }
	    अन्यथा जर (c == Ctrl_G && p_im)
		सुरूठेवा;
	    अन्यथा
	    {
		जर (c == Ctrl_O)
		{
		    ins_ctrl_o();
		    ins_at_eol = FALSE;	// cursor keeps its column
		    nomove = TRUE;
		}
		मोजणी = 0;
		जा doESCkey;
	    }
	}

#ifdef FEAT_DIGRAPHS
	c = do_digraph(c);
#endif

	जर ((c == Ctrl_V || c == Ctrl_Q) && ctrl_x_mode_cmdline())
	    जा docomplete;
	जर (c == Ctrl_V || c == Ctrl_Q)
	{
	    ins_ctrl_v();
	    c = Ctrl_V;	// pretend CTRL-V is last typed character
	    सुरूठेवा;
	}

	जर (cindent_on() && ctrl_x_mode_none())
	{
	    // A key name preceded by a bang means this key is not to be
	    // inserted.  Skip ahead to the re-indenting below.
	    // A key name preceded by a star means that indenting has to be
	    // done before inserting the key.
	    ओळ_is_white = inindent(0);
	    जर (in_cinkeys(c, '!', ओळ_is_white))
		जा force_cindent;
	    जर (can_cindent && in_cinkeys(c, '*', ओळ_is_white)
							&& stop_arrow() == OK)
		do_c_अभिव्यक्ती_indent();
	}

#ifdef FEAT_RIGHTLEFT
	जर (curwin->w_p_rl)
	    निवड (c)
	    {
		स्थिती K_डावा:	c = K_उजवा; थांबा;
		स्थिती K_S_डावा:	c = K_S_उजवा; थांबा;
		स्थिती K_C_डावा:	c = K_C_उजवा; थांबा;
		स्थिती K_उजवा:	c = K_डावा; थांबा;
		स्थिती K_S_उजवा: c = K_S_डावा; थांबा;
		स्थिती K_C_उजवा: c = K_C_डावा; थांबा;
	    }
#endif

	/*
	 * If 'keymodel' contains "startsel", may start selection.  If it
	 * does, a CTRL-O and c will be stuffed, we need to get these
	 * characters.
	 */
	जर (ins_start_select(c))
	    सुरूठेवा;

	/*
	 * The big switch to handle a character in insert mode.
	 */
	निवड (c)
	{
	स्थिती ESC:	// End input mode
	    जर (echeck_abbr(ESC + ABBR_OFF))
		थांबा;
	    // FALLTHROUGH

	स्थिती Ctrl_C:	// End input mode
	    जर (c == Ctrl_C && cmdwin_type != 0)
	    {
		// Close the cmdline window.
		cmdwin_result = K_IGNORE;
		got_int = FALSE; // don't stop executing autocommands et al.
		nomove = TRUE;
		जा doESCkey;
	    }
#ifdef FEAT_JOB_CHANNEL
	    जर (c == Ctrl_C && bt_prompt(curbuf))
	    {
		जर (invoke_prompt_interrupt())
		{
		    जर (!bt_prompt(curbuf))
			// buffer changed to a non-prompt buffer, get out of
			// Insert mode
			जा doESCkey;
		    थांबा;
		}
	    }
#endif

#ifdef UNIX
do_intr:
#endif
	    // when 'insertmode' set, and not halfway a mapping, don't leave
	    // Insert mode
	    जर (goto_im())
	    {
		जर (got_int)
		{
		    (शून्य)vgetc();		// flush all buffers
		    got_int = FALSE;
		}
		अन्यथा
		    vim_beep(BO_IM);
		थांबा;
	    }
doESCkey:
	    // Drop a pending autocomplete so it does not outlive Insert mode.
	    ins_compl_clear_autocomplete_delay();
	    /*
	     * This is the ONLY return from edit()!
	     */
	    // Always update o_lnum, so that a "CTRL-O ." that adds a line
	    // still puts the cursor back after the inserted text.
	    जर (ins_at_eol && gchar_cursor() == NUL)
		o_lnum = curwin->w_cursor.lnum;

	    जर (ins_esc(&मोजणी, cmdchar, nomove))
	    {
		// When CTRL-C was typed got_int will be set, with the result
		// that the autocommands won't be executed. When mapped got_int
		// is not set, but let's keep the behavior the same.
		जर (cmdchar != 'r' && cmdchar != 'v' && c != Ctrl_C)
		    ins_apply_autocmds(EVENT_INSERTLEAVE);
		did_cursorhold = FALSE;

		// ins_redraw() triggers TextChangedI only when no characters
		// are in the typeahead buffer, so reset curbuf->b_last_changedtick only
		// if the TextChangedI was not blocked by char_avail() (e.g. using :norm!)
		// and the TextChangedI autocommand has been triggered.
		जर (!char_avail() && curbuf->b_last_changedtick_i == CHANGEDTICK(curbuf))
		    curbuf->b_last_changedtick = CHANGEDTICK(curbuf);
		परत (c == Ctrl_O);
	    }
	    सुरूठेवा;

	स्थिती Ctrl_Z:	// suspend when 'insertmode' set
	    जर (!p_im)
		जा normalchar;	// insert CTRL-Z as normal char
	    do_cmdline_cmd((char_u *)"stop");
#ifdef CURSOR_SHAPE
	    ui_cursor_shape();		// may need to update cursor shape
#endif
	    सुरूठेवा;

	स्थिती Ctrl_O:	// execute one command
#ifdef FEAT_COMPL_FUNC
	    जर (ctrl_x_mode_omni())
		जा docomplete;
#endif
	    जर (echeck_abbr(Ctrl_O + ABBR_OFF))
		थांबा;
	    ins_ctrl_o();

	    // don't move the cursor left when 'virtualedit' has "onemore".
	    जर (get_ve_flags() & VE_ONEMORE)
	    {
		ins_at_eol = FALSE;
		nomove = TRUE;
	    }
	    मोजणी = 0;
	    जा doESCkey;

	स्थिती K_INS:	// toggle insert/replace mode
	स्थिती K_KINS:
	    ins_घाला(replaceState);
	    थांबा;

	स्थिती K_SELECT:	// end of Select mode mapping - ignore
	    थांबा;

	स्थिती K_HELP:	// Help key works like <ESC> <Help>
	स्थिती K_F1:
	स्थिती K_XF1:
	    stuffcharReadbuff(K_HELP);
	    जर (p_im)
		need_start_insertmode = TRUE;
	    जा doESCkey;

#ifdef FEAT_NETBEANS_INTG
	स्थिती K_F21:	// NetBeans command
	    ++no_mapping;		// don't map the next key hits
	    i = plain_vgetc();
	    --no_mapping;
	    netbeans_keycommand(i);
	    थांबा;
#endif

	स्थिती K_ZERO:	// Insert the previously inserted text.
	स्थिती NUL:
	स्थिती Ctrl_A:
	    // For ^@ the trailing ESC will end the insert, unless there is an
	    // error.
	    जर (stuff_inserted(NUL, 1L, (c == Ctrl_A)) == FAIL
						   && c != Ctrl_A && !p_im)
		जा doESCkey;		// quit insert mode
	    inserted_space = FALSE;
	    थांबा;

	स्थिती Ctrl_R:	// insert the contents of a register
	    जर (ctrl_x_mode_register() && !ins_compl_active())
		जा docomplete;
	    ins_reg();
	    auto_format(FALSE, TRUE);
	    inserted_space = FALSE;
	    थांबा;

	स्थिती Ctrl_G:	// commands starting with CTRL-G
	    ins_ctrl_g();
	    थांबा;

	स्थिती Ctrl_HAT:	// switch input mode and/or langmap
	    ins_ctrl_hat();
	    थांबा;

#ifdef FEAT_RIGHTLEFT
	स्थिती Ctrl__:	// switch between languages
	    जर (!p_ari)
		जा normalchar;
	    ins_ctrl_();
	    थांबा;
#endif

	स्थिती Ctrl_D:	// Make indent one shiftwidth smaller.
#जर defined(FEAT_FIND_ID)
	    जर (ctrl_x_mode_path_defines())
		जा docomplete;
#endif
	    // FALLTHROUGH

	स्थिती Ctrl_T:	// Make indent one shiftwidth greater.
	    जर (c == Ctrl_T && ctrl_x_mode_thesaurus())
	    {
		जर (has_compl_option(FALSE))
		    जा docomplete;
		थांबा;
	    }

	    ins_shift(c, lastc);
	    auto_format(FALSE, TRUE);
	    inserted_space = FALSE;
	    थांबा;

	स्थिती K_DEL:	// delete character under the cursor
	स्थिती K_KDEL:
	    ins_del();
	    auto_format(FALSE, TRUE);
	    थांबा;

	स्थिती K_BS:	// delete character before the cursor
	स्थिती K_S_BS:
	स्थिती Ctrl_H:
	    did_backspace = ins_bs(c, BACKSPACE_CHAR, &inserted_space);
	    auto_format(FALSE, TRUE);
	    जर (did_backspace)
		MAY_TRIGGER_AUTOCOMPLETE(c);
	    थांबा;

	स्थिती Ctrl_W:	// delete word before the cursor
#ifdef FEAT_JOB_CHANNEL
	    जर (bt_prompt(curbuf) && (mod_mask & MOD_MASK_SHIFT) == 0)
	    {
		// In a prompt window CTRL-W is used for window commands.
		// Use Shift-CTRL-W to delete a word.
		stuffcharReadbuff(Ctrl_W);
		restart_edit = 'A';
		nomove = TRUE;
		मोजणी = 0;
		जा doESCkey;
	    }
#endif
	    did_backspace = ins_bs(c, BACKSPACE_WORD, &inserted_space);
	    auto_format(FALSE, TRUE);
	    जर (did_backspace)
		MAY_TRIGGER_AUTOCOMPLETE(c);
	    थांबा;

	स्थिती Ctrl_U:	// delete all inserted text in current line
#ifdef FEAT_COMPL_FUNC
	    // CTRL-X CTRL-U completes with 'completefunc'.
	    जर (ctrl_x_mode_function())
		जा docomplete;
#endif
	    did_backspace = ins_bs(c, BACKSPACE_ओळ, &inserted_space);
	    auto_format(FALSE, TRUE);
	    inserted_space = FALSE;
	    जर (did_backspace)
		MAY_TRIGGER_AUTOCOMPLETE(c);
	    थांबा;

	स्थिती K_LEFTMOUSE:   // mouse keys
	स्थिती K_LEFTMOUSE_NM:
	स्थिती K_LEFTDRAG:
	स्थिती K_LEFTRELEASE:
	स्थिती K_LEFTRELEASE_NM:
	स्थिती K_MOUSEMOVE:
	स्थिती K_MIDDLEMOUSE:
	स्थिती K_MIDDLEDRAG:
	स्थिती K_MIDDLERELEASE:
	स्थिती K_RIGHTMOUSE:
	स्थिती K_RIGHTDRAG:
	स्थिती K_RIGHTRELEASE:
	स्थिती K_X1MOUSE:
	स्थिती K_X1DRAG:
	स्थिती K_X1RELEASE:
	स्थिती K_X2MOUSE:
	स्थिती K_X2DRAG:
	स्थिती K_X2RELEASE:
	    ins_mouse(c);
	    थांबा;

	स्थिती K_MOUSEDOWN: // Default action for scroll wheel up: scroll up
	    ins_mousescroll(MSCR_DOWN);
	    थांबा;

	स्थिती K_MOUSEUP:	// Default action for scroll wheel down: scroll down
	    ins_mousescroll(MSCR_UP);
	    थांबा;

	स्थिती K_MOUSELEFT: // Scroll wheel left
	    ins_mousescroll(MSCR_डावा);
	    थांबा;

	स्थिती K_MOUSERIGHT: // Scroll wheel right
	    ins_mousescroll(MSCR_उजवा);
	    थांबा;

	स्थिती K_PS:
	    bracketed_paste(PASTE_घाला, FALSE, रिक्त);
	    जर (cmdchar == K_PS)
		// invoked from normal mode, bail out
		जा doESCkey;
	    थांबा;
	स्थिती K_PE:
	    // Got K_PE without K_PS, ignore.
	    थांबा;

#ifdef FEAT_GUI_TABLINE
	स्थिती K_TABLINE:
	स्थिती K_TABMENU:
	    ins_tabline(c);
	    थांबा;
#endif

	स्थिती K_IGNORE:	// Something mapped to nothing
	    थांबा;

	स्थिती K_COMMAND:		    // <Cmd>command<CR>
	स्थिती K_SCRIPT_COMMAND:	    // <ScriptCmd>command<CR>
	    {
		bufref_T    save_curbuf;
		varnumber_T tick = CHANGEDTICK(curbuf);

		set_bufref(&save_curbuf, curbuf);
		do_cmdkey_command(c, 0);

#ifdef FEAT_TERMINAL
		जर (term_use_loop())
		    // Started a terminal that gets the input, exit Insert mode.
		    जा doESCkey;
#endif
		जर (curbuf->b_u_synced
			|| (bufref_valid(&save_curbuf)
			    && curbuf == save_curbuf.br_buf
			    && tick != CHANGEDTICK(curbuf)))
		{
		    // The command synced undo or changed this buffer.
		    // Save the cursor line before the next typed edit.
		    ins_need_undo = TRUE;
		}
	    }
	    थांबा;

	स्थिती K_CURSORHOLD:	// Didn't type something for a while.
	    ins_apply_autocmds(EVENT_CURSORHOLDI);
	    did_cursorhold = TRUE;
	    // If CTRL-G U was used apply it to the next typed key.
	    जर (dont_sync_undo == TRUE)
		dont_sync_undo = MAYBE;
	    थांबा;

	स्थिती K_COMPLETE_DELAY:	// 'autocompletedelay' expired
	    // If CTRL-G U was used apply it to the next typed key.
	    जर (dont_sync_undo == TRUE)
		dont_sync_undo = MAYBE;
	    ins_compl_clear_autocomplete_delay();
	    जर (!ins_compl_has_autocomplete() || char_avail()
		    || curwin->w_cursor.col == 0)
		थांबा;
	    c = char_before_cursor();
	    जर (!vim_isprintc(c))
		थांबा;
	    // The completion may have been cleared while waiting, so re-enable
	    // autocomplete to match a zero delay.
	    ins_compl_enable_autocomplete();
	    ins_compl_arm_autostart();
	    जा docomplete;

#ifdef FEAT_GUI_MSWIN
	    // On MS-Windows ignore <M-F4>, we get it when closing the window
	    // was cancelled.
	स्थिती K_F4:
	    जर (mod_mask != MOD_MASK_ALT)
		जा normalchar;
	    थांबा;
#endif

#ifdef FEAT_GUI
	स्थिती K_VER_SCROLLBAR:
	    ins_scroll();
	    थांबा;

	स्थिती K_HOR_SCROLLBAR:
	    ins_horscroll();
	    थांबा;
#endif

	स्थिती K_HOME:	// <Home>
	स्थिती K_KHOME:
	स्थिती K_S_HOME:
	स्थिती K_C_HOME:
	    ins_home(c);
	    थांबा;

	स्थिती K_END:	// <End>
	स्थिती K_KEND:
	स्थिती K_S_END:
	स्थिती K_C_END:
	    ins_end(c);
	    थांबा;

	स्थिती K_डावा:	// <Left>
	    जर (mod_mask & (MOD_MASK_SHIFT|MOD_MASK_CTRL))
		ins_s_डावा();
	    अन्यथा
		ins_डावा();
	    थांबा;

	स्थिती K_S_डावा:	// <S-Left>
	स्थिती K_C_डावा:
	    ins_s_डावा();
	    थांबा;

	स्थिती K_उजवा:	// <Right>
	    जर (mod_mask & (MOD_MASK_SHIFT|MOD_MASK_CTRL))
		ins_s_उजवा();
	    अन्यथा
		ins_उजवा();
	    थांबा;

	स्थिती K_S_उजवा:	// <S-Right>
	स्थिती K_C_उजवा:
	    ins_s_उजवा();
	    थांबा;

	स्थिती K_UP:	// <Up>
	    जर (pum_visible())
		जा docomplete;
	    जर (mod_mask & MOD_MASK_SHIFT)
		ins_pageup();
	    अन्यथा
		ins_up(FALSE);
	    थांबा;

	स्थिती K_S_UP:	// <S-Up>
	स्थिती K_PAGEUP:
	स्थिती K_KPAGEUP:
	    जर (pum_visible())
	    {
#ifdef FEAT_PROP_POPUP
		// CTRL-SHIFT-<Up> scrolls the info popup up a line,
		// CTRL-SHIFT-<PageUp> a page.  Shift is folded into K_S_UP but
		// stays in mod_mask for PageUp, hence the asymmetric check.
		जर (c == K_S_UP ? (mod_mask & MOD_MASK_CTRL)
			: ((mod_mask & MOD_MASK_CTRL)
			    && (mod_mask & MOD_MASK_SHIFT)))
		{
		    popup_scroll_info(-1, c != K_S_UP);
		    थांबा;
		}
#endif
		जा docomplete;
	    }
	    ins_pageup();
	    थांबा;

	स्थिती K_DOWN:	// <Down>
	    जर (pum_visible())
		जा docomplete;
	    जर (mod_mask & MOD_MASK_SHIFT)
		ins_pagedown();
	    अन्यथा
		ins_down(FALSE);
	    थांबा;

	स्थिती K_S_DOWN:	// <S-Down>
	स्थिती K_PAGEDOWN:
	स्थिती K_KPAGEDOWN:
	    जर (pum_visible())
	    {
#ifdef FEAT_PROP_POPUP
		// CTRL-SHIFT-<Down>/<PageDown> scroll the info popup down.
		जर (c == K_S_DOWN ? (mod_mask & MOD_MASK_CTRL)
			: ((mod_mask & MOD_MASK_CTRL)
			    && (mod_mask & MOD_MASK_SHIFT)))
		{
		    popup_scroll_info(1, c != K_S_DOWN);
		    थांबा;
		}
#endif
		जा docomplete;
	    }
	    ins_pagedown();
	    थांबा;

#ifdef FEAT_DND
	स्थिती K_DROP:	// drag-n-drop event
	    ins_drop();
	    थांबा;
#endif

	स्थिती K_S_TAB:	// When not mapped, use like a normal TAB
	    c = TAB;
	    // FALLTHROUGH

	स्थिती TAB:	// TAB or Complete patterns along path
#जर defined(FEAT_FIND_ID)
	    जर (ctrl_x_mode_path_patterns())
		जा docomplete;
#endif
	    inserted_space = FALSE;
	    जर (ins_tab())
		जा normalchar;	// insert TAB as a normal char
	    auto_format(FALSE, TRUE);
	    थांबा;

	स्थिती K_KENTER:	// <Enter>
	    c = CAR;
	    // FALLTHROUGH
	स्थिती CAR:
	स्थिती NL:
#जर defined(FEAT_QUICKFIX)
	    // In a quickfix window a <CR> jumps to the error under the
	    // cursor.
	    जर (bt_quickfix(curbuf) && c == CAR)
	    {
		जर (curwin->w_llist_ref == रिक्त)    // quickfix window
		    do_cmdline_cmd((char_u *)".cc");
		अन्यथा				    // location list window
		    do_cmdline_cmd((char_u *)".ll");
		थांबा;
	    }
#endif
	    जर (cmdwin_type != 0)
	    {
		// Execute the command in the cmdline window.
		cmdwin_result = CAR;
		जा doESCkey;
	    }
#ifdef FEAT_JOB_CHANNEL
	    जर (bt_prompt(curbuf))
	    {
		invoke_prompt_callback();
		जर (!bt_prompt(curbuf))
		    // buffer changed to a non-prompt buffer, get out of
		    // Insert mode
		    जा doESCkey;
		थांबा;
	    }
#endif
	    जर (ins_eol(c) == FAIL && !p_im)
		जा doESCkey;	    // out of memory
	    auto_format(FALSE, FALSE);
	    inserted_space = FALSE;
	    थांबा;

	स्थिती Ctrl_K:	    // digraph or keyword completion
	    जर (ctrl_x_mode_dictionary())
	    {
		जर (has_compl_option(TRUE))
		    जा docomplete;
		थांबा;
	    }
#ifdef FEAT_DIGRAPHS
	    c = ins_digraph();
	    जर (c == NUL)
		थांबा;
#endif
	    जा normalchar;

	स्थिती Ctrl_X:	// Enter CTRL-X mode
	    ins_ctrl_x();
	    थांबा;

	स्थिती Ctrl_RSB:	// Tag name completion after ^X
	    जर (!ctrl_x_mode_tags())
		जा normalchar;
	    जा docomplete;

	स्थिती Ctrl_F:	// File name completion after ^X
	    जर (!ctrl_x_mode_files())
		जा normalchar;
	    जा docomplete;

	स्थिती 's':	// Spelling completion after ^X
	स्थिती Ctrl_S:
	    जर (!ctrl_x_mode_spell())
		जा normalchar;
	    जा docomplete;

	स्थिती Ctrl_L:	// Whole line completion after ^X
	    जर (!ctrl_x_mode_whole_ओळ())
	    {
		// CTRL-L with 'insertmode' set: Leave Insert mode
		जर (p_im)
		{
		    जर (echeck_abbr(Ctrl_L + ABBR_OFF))
			थांबा;
		    जा doESCkey;
		}
		जा normalchar;
	    }
	    // FALLTHROUGH

	स्थिती Ctrl_P:	// Do previous/next pattern completion
	स्थिती Ctrl_N:
#ifdef FEAT_PROP_POPUP
	    // CTRL-SHIFT-P/N scroll the info popup one line.
	    जर (pum_visible() && (mod_mask & MOD_MASK_SHIFT)
		    && (c == Ctrl_P || c == Ctrl_N))
	    {
		popup_scroll_info(c == Ctrl_P ? -1 : 1, false);
		थांबा;
	    }
#endif
	    // if 'complete' is empty then plain ^P is no longer special,
	    // but it is under other ^X modes
	    जर (*curbuf->b_p_cpt == NUL
		    && (ctrl_x_mode_normal() || ctrl_x_mode_whole_ओळ())
		    && !compl_status_local())
		जा normalchar;

docomplete:
	    ins_compl_clear_autocomplete_delay();
	    compl_busy = TRUE;
#ifdef FEAT_FOLDING
	    disable_fold_update++;  // don't redraw folds here
#endif
	    जर (ins_complete(c, TRUE) == FAIL)
		compl_status_clear();
#ifdef FEAT_FOLDING
	    disable_fold_update--;
#endif
	    compl_busy = FALSE;
	    can_si = may_do_si(); // allow smartindenting
	    थांबा;

	स्थिती Ctrl_Y:	// copy from previous line or scroll down
	स्थिती Ctrl_E:	// copy from next line	   or scroll up
	    c = ins_ctrl_ey(c);
	    थांबा;

	पूर्वनिर्धारित:
#ifdef UNIX
	    जर (c == intr_char)		// special interrupt char
		जा do_intr;
#endif

normalchar:
	    /*
	     * Insert a normal character.
	     */
#जर defined(FEAT_EVAL)
	    जर (!p_paste)
	    {
		// Trigger InsertCharPre.
		char_u *str = do_घाला_char_pre(c);
		char_u *p;

		जर (str != रिक्त)
		{
		    जर (*str != NUL && stop_arrow() != FAIL)
		    {
			// Insert the new value of v:char literally.
			साठी (p = str; *p != NUL; MB_PTR_ADV(p))
			{
			    c = PTR2CHAR(p);
			    जर (c == CAR || c == K_KENTER || c == NL)
				ins_eol(c);
			    अन्यथा
				ins_char(c);
			}
			AppendToRedobuffLit(str, -1);
		    }
		    vim_free(str);
		    c = NUL;
		}

		// If the new value is already inserted or an empty string
		// then don't insert any character.
		जर (c == NUL)
		    थांबा;
	    }
#endif
	    // Try to perform smart-indenting.
	    ins_try_si(c);

	    जर (c == ' ')
	    {
		inserted_space = TRUE;
		जर (inindent(0))
		    can_cindent = FALSE;
		जर (Insstart_blank_vcol == MAXCOL
			&& curwin->w_cursor.lnum == Insstart.lnum)
		    Insstart_blank_vcol = get_nolist_virtcol();
	    }

	    // Insert a normal character and check for abbreviations on a
	    // special character.  Let CTRL-] expand abbreviations without
	    // inserting it.
	    जर (vim_iswordc(c) || (!echeck_abbr(
			// Add ABBR_OFF for characters above 0x100, this is
			// what check_abbr() expects.
				(has_mbyte && c >= 0x100) ? (c + ABBR_OFF) : c)
			&& c != Ctrl_RSB))
	    {
		घाला_special(c, FALSE, FALSE);
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
	    जर (ins_compl_has_autocomplete() && !char_avail()
		    && vim_isprintc(c))
		TRIGGER_AUTOCOMPLETE();

	    थांबा;
	}   // end of switch (c)

	// If typed something may trigger CursorHoldI again; K_COMPLETE_DELAY is
	// injected, not typed.
	जर (c != K_CURSORHOLD && c != K_COMPLETE_DELAY
#ifdef FEAT_COMPL_FUNC
		// but not in CTRL-X mode, a script can't restore the state
		&& ctrl_x_mode_normal()
#endif
	       )
	    did_cursorhold = FALSE;

	// Check if we need to cancel completion mode because the window
	// or tab page was changed
	जर (ins_compl_active() && !ins_compl_win_active(curwin))
	    ins_compl_cancel();

	// If the cursor was moved we didn't just insert a space
	जर (arrow_used)
	    inserted_space = FALSE;

	जर (can_cindent && cindent_on() && ctrl_x_mode_normal())
	{
force_cindent:
	    /*
	     * Indent now if a key was typed that is in 'cinkeys'.
	     */
	    जर (in_cinkeys(c, ' ', ओळ_is_white))
	    {
		जर (stop_arrow() == OK)
		    // re-indent the current line
		    do_c_अभिव्यक्ती_indent();
	    }
	}

    }	// for (;;)
    // NOTREACHED
}

    पूर्णांक
ins_need_undo_get(शून्य)
{
    परत ins_need_undo;
}

/*
 * Redraw for Insert mode.
 * This is postponed until getting the next character to make '$' in the 'cpo'
 * option work correctly.
 * Only redraw when there are no characters available.  This speeds up
 * inserting sequences of characters (e.g., for CTRL-R).
 */
    शून्य
ins_redraw(पूर्णांक ready)	    // not busy with something
{
#ifdef FEAT_CONCEAL
    linenr_T	conceal_old_cursor_ओळ = 0;
    linenr_T	conceal_new_cursor_ओळ = 0;
    पूर्णांक		conceal_update_ओळी = FALSE;
#endif

    जर (char_avail())
	परत;

    // Trigger CursorMoved if the cursor moved.  Not when the popup menu is
    // visible, the command might delete it.
    जर (ready && (has_cursormovedI()
#ifdef FEAT_PROP_POPUP
		|| popup_visible
#endif
#जर defined(FEAT_CONCEAL)
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
	जर (syntax_present(curwin) && must_redraw)
	    update_screen(0);
#endif
	जर (has_cursormovedI())
	{
	    // Make sure curswant is correct, an autocommand may call
	    // getcurpos().
	    update_curswant();
	    ins_apply_autocmds(EVENT_CURSORMOVEDI);
	}
#ifdef FEAT_PROP_POPUP
	जर (popup_visible)
	    popup_check_cursor_pos();
#endif
#ifdef FEAT_CONCEAL
	जर (curwin->w_p_cole > 0)
	{
	    conceal_old_cursor_ओळ = last_cursormoved.lnum;
	    conceal_new_cursor_ओळ = curwin->w_cursor.lnum;
	    conceal_update_ओळी = TRUE;
	}
#endif
	last_cursormoved = curwin->w_cursor;
    }

    // Trigger TextChangedI if b_changedtick_i differs.
    जर (ready && has_textchangedI()
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
	जर (tick != CHANGEDTICK(curbuf))  // see ins_apply_autocmds()
	    u_save(curwin->w_cursor.lnum,
					(linenr_T)(curwin->w_cursor.lnum + 1));
    }

    // Trigger TextChangedP if b_changedtick_pum differs. When the popupmenu
    // closes TextChangedI will need to trigger for backwards compatibility,
    // thus use different b_last_changedtick* variables.
    जर (ready && has_textchangedP()
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
	जर (tick != CHANGEDTICK(curbuf))  // see ins_apply_autocmds()
	    u_save(curwin->w_cursor.lnum,
					(linenr_T)(curwin->w_cursor.lnum + 1));
    }

    जर (ready)
	may_trigger_win_scrolled_resized();

    // Trigger SafeState if nothing is pending.
    may_trigger_safestate(ready
	    && !ins_compl_active()
	    && !pum_visible());

#जर defined(FEAT_CONCEAL)
    जर ((conceal_update_ओळी
	    && (conceal_old_cursor_ओळ != conceal_new_cursor_ओळ
		|| conceal_cursor_ओळ(curwin)))
	    || need_cursor_ओळ_redraw)
    {
	जर (conceal_old_cursor_ओळ != conceal_new_cursor_ओळ)
	    redrawWinline(curwin, conceal_old_cursor_ओळ);
	redrawWinline(curwin, conceal_new_cursor_ओळ == 0
			    ? curwin->w_cursor.lnum : conceal_new_cursor_ओळ);
	curwin->w_valid &= ~VALID_CROW;
	need_cursor_ओळ_redraw = FALSE;
    }
#endif
    जर (must_redraw)
	update_screen(0);
    अन्यथा जर (clear_cmdline || redraw_cmdline)
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
    पूर्णांक		c;
    पूर्णांक		did_putchar = FALSE;

    // may need to redraw when no more chars available now
    ins_redraw(FALSE);

    जर (redrawing() && !char_avail())
    {
	edit_putchar('^', TRUE);
	did_putchar = TRUE;
    }
    AppendToRedobuff((char_u *)CTRL_V_STR);	// CTRL-V

    add_to_showcmd_c(Ctrl_V);

    // Do not change any modifyOtherKeys ESC sequence to a normal key for
    // CTRL-SHIFT-V.
    c = get_literal(mod_mask & MOD_MASK_SHIFT);
    जर (did_putchar)
	// when the line fits in 'columns' the '^' is at the start of the next
	// line and will not removed by the redraw
	edit_unputchar();
    clear_showcmd();

    घाला_special(c, FALSE, TRUE);
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
    स्थिर पूर्णांक
decode_Modify_Other_किल्ल्या(पूर्णांक c)
{
    char_u  *p = typebuf.tb_buf + typebuf.tb_off;
    पूर्णांक	    idx;
    पूर्णांक	    form = 0;
    पूर्णांक	    argidx = 0;
    पूर्णांक	    arg[2] = {0, 0};

    // Recognize:
    // form 0: {lead}{key};{modifier}u
    // form 1: {lead}27;{modifier};{key}~
    जर (typebuf.tb_len >= 4 && (c == CSI || (c == ESC && *p == '[')))
    {
	idx = (*p == '[');
	जर (p[idx] == '2' && p[idx + 1] == '7' && p[idx + 2] == ';' &&
		kitty_protocol_state != KKPS_ENABLED)
	{
	    form = 1;
	    idx += 3;
	}
	पर्यंत (idx < typebuf.tb_len && argidx < 2)
	{
	    जर (p[idx] == ';')
		++argidx;
	    अन्यथा जर (VIM_ISDIGIT(p[idx]))
		arg[argidx] = arg[argidx] * 10 + (p[idx] - '0');
	    अन्यथा
		थांबा;
	    ++idx;
	}
	पूर्णांक kitty_no_mods = argidx == 0 && kitty_protocol_state == KKPS_ENABLED;
	जर (idx < typebuf.tb_len
		&& p[idx] == (form == 1 ? '~' : 'u')
		&& (argidx == 1 || kitty_no_mods))
	{
	    // Match, consume the code.
	    typebuf.tb_off += idx + 1;
	    typebuf.tb_len -= idx + 1;
#जर defined(FEAT_CLIENTSERVER) || defined(FEAT_EVAL)
	    जर (typebuf.tb_len == 0)
		typebuf_was_filled = FALSE;
#endif

	    mod_mask = kitty_no_mods ? 0 : decode_modifiers(arg[!form]);
	    c = merge_modify_Other_किल्ल्या(arg[form], &mod_mask);
	}
    }

    परत c;
}

/*
 * Put a character directly onto the screen.  It's not stored in a buffer.
 * Used while handling CTRL-K, CTRL-V, etc. in Insert mode.
 */
स्थिर पूर्णांक  pc_status;
#define PC_STATUS_UNSET	0	// pc_bytes was not set
#define PC_STATUS_उजवा	1	// right half of double-wide char
#define PC_STATUS_डावा	2	// left half of double-wide char
#define PC_STATUS_SET	3	// pc_bytes was filled
स्थिर char_u pc_bytes[MB_MAXBYTES + 1]; // saved bytes
स्थिर पूर्णांक  pc_attr;
स्थिर पूर्णांक  pc_row;
स्थिर पूर्णांक  pc_col;

    शून्य
edit_putchar(पूर्णांक c, पूर्णांक highlight)
{
    पूर्णांक	    attr;

    जर (Screen_ओळी == रिक्त)
	परत;

    update_topline();	// just in case w_topline isn't valid
    validate_cursor();
    जर (highlight)
	attr = HL_ATTR(HLF_8);
    अन्यथा
	attr = 0;
    pc_row = W_WINROW(curwin) + curwin->w_wrow;
    pc_col = curwin->w_wincol;
    pc_status = PC_STATUS_UNSET;
#ifdef FEAT_RIGHTLEFT
    जर (curwin->w_p_rl)
    {
	pc_col += curwin->w_width - 1 - curwin->w_wcol;
	जर (has_mbyte)
	{
	    पूर्णांक fix_col = mb_fix_col(pc_col, pc_row);

	    जर (fix_col != pc_col)
	    {
		screen_putchar(' ', pc_row, fix_col, attr);
		--curwin->w_wcol;
		pc_status = PC_STATUS_उजवा;
	    }
	}
    }
    अन्यथा
#endif
    {
	pc_col += curwin->w_wcol;
	जर (mb_lefthalve(pc_row, pc_col))
	    pc_status = PC_STATUS_डावा;
    }

    // save the character to be able to put it back
    जर (pc_status == PC_STATUS_UNSET)
    {
	screen_getbytes(pc_row, pc_col, pc_bytes, &pc_attr);
	pc_status = PC_STATUS_SET;
    }
    screen_putchar(c, pc_row, pc_col, attr);
}

#जर defined(FEAT_JOB_CHANNEL)
/*
 * Set the insert start position for when using a prompt buffer.
 */
    शून्य
set_insstart(linenr_T lnum, पूर्णांक col)
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
    जर (pc_status != PC_STATUS_UNSET && pc_row >= संदेश_scrolled)
    {
	जर (pc_status == PC_STATUS_उजवा)
	    ++curwin->w_wcol;
	जर (pc_status == PC_STATUS_उजवा || pc_status == PC_STATUS_डावा)
	    redrawWinline(curwin, curwin->w_cursor.lnum);
	अन्यथा
	    screen_puts(pc_bytes, pc_row - संदेश_scrolled, pc_col, pc_attr);
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

    जर (!redrawing())
	परत;

    cursor_off();
    save_col = curwin->w_cursor.col;
    curwin->w_cursor.col = col;
    जर (has_mbyte)
    {
	char_u *p;

	// If on the last byte of a multi-byte move to the first byte.
	p = ml_get_curline();
	curwin->w_cursor.col -= (*mb_head_off)(p, p + col);
    }
    curs_columns(FALSE);	    // recompute w_wrow and w_wcol
    जर (curwin->w_wcol < curwin->w_width)
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
    जर (dollar_vcol < 0)
	परत;

    dollar_vcol = -1;
    redrawWinline(curwin, curwin->w_cursor.lnum);
}

/*
 * Truncate the space at the end of a line.  This is to be used only in an
 * insert mode.  It handles fixing the replace stack for MODE_REPLACE and
 * MODE_VREPLACE modes.
 */
    शून्य
truncate_spaces(char_u *ओळ, माप_प्रकार len)
{
    पूर्णांक	    i;

    // find start of trailing white space
    साठी (i = (पूर्णांक)len - 1; i >= 0 && VIM_ISWHITE(ओळ[i]); i--)
    {
	जर (State & REPLACE_FLAG)
	    replace_join(0);	    // remove a NUL from the replace stack
    }
    ओळ[i + 1] = NUL;
}

/*
 * Backspace the cursor until the given column.  Handles MODE_REPLACE and
 * MODE_VREPLACE modes correctly.  May also be used when not in insert mode at
 * all.  Will attempt not to go before "col" even when there is a composing
 * character.
 */
    शून्य
backspace_until_column(पूर्णांक col)
{
    पर्यंत ((पूर्णांक)curwin->w_cursor.col > col)
    {
	curwin->w_cursor.col--;
	जर (State & REPLACE_FLAG)
	    replace_do_bs(col);
	अन्यथा जर (!del_char_after_col(col))
	    थांबा;
    }
}

/*
 * Like del_char(), but make sure not to go before column "limit_col".
 * Only matters when there are composing characters.
 * Return TRUE when something was deleted.
 */
    स्थिर पूर्णांक
del_char_after_col(पूर्णांक limit_col UNUSED)
{
    जर (enc_utf8 && limit_col >= 0)
    {
	colnr_T ecol = curwin->w_cursor.col + 1;

	// Make sure the cursor is at the start of a character, but
	// skip forward again when going too far back because of a
	// composing character.
	mb_adjust_cursor();
	पर्यंत (curwin->w_cursor.col < (colnr_T)limit_col)
	{
	    पूर्णांक l = utf_ptr2len(ml_get_cursor());

	    जर (l == 0)  // end of line
		थांबा;
	    curwin->w_cursor.col += l;
	}
	जर (*ml_get_cursor() == NUL || curwin->w_cursor.col == ecol)
	    परत FALSE;
	del_bytes((दीर्घ)((पूर्णांक)ecol - curwin->w_cursor.col), FALSE, TRUE);
    }
    अन्यथा
	(शून्य)del_char(FALSE);
    परत TRUE;
}

/*
 * Next character is interpreted literally.
 * A one, two or three digit decimal number is interpreted as its byte value.
 * If one or two digits are entered, the next character is given to vungetc().
 * For Unicode a character > 255 may be returned.
 * If "noReduceKeys" is TRUE do not change any modifyOtherKeys ESC sequence
 * into a normal key, return ESC.
 */
    पूर्णांक
get_literal(पूर्णांक no_Reduce_किल्ल्या)
{
    पूर्णांक		cc;
    पूर्णांक		nc;
    पूर्णांक		i;
    पूर्णांक		hex = FALSE;
    पूर्णांक		octal = FALSE;
    पूर्णांक		unicode = 0;

    जर (got_int)
	परत Ctrl_C;

#ifdef FEAT_GUI
    /*
     * In GUI there is no point inserting the internal code for a special key.
     * It is more useful to insert the string "<KEY>" instead.	This would
     * probably be useful in a text window too, but it would not be
     * vi-compatible (maybe there should be an option for it?) -- webb
     */
    जर (gui.in_use)
    {
	++allow_किल्ल्या;
	जर (no_Reduce_किल्ल्या)
	    ++no_reduce_किल्ल्या;
    }
#endif
#ifdef USE_ON_FLY_SCROLL
    dont_scroll = TRUE;		// disallow scrolling here
#endif
    ++no_mapping;		// don't map the next key hits
    cc = 0;
    i = 0;
    साठी (;;)
    {
	nc = plain_vgetc();
	जर ((nc == ESC || nc == CSI) && !no_Reduce_किल्ल्या)
	    nc = decode_Modify_Other_किल्ल्या(nc);

	जर ((mod_mask & ~MOD_MASK_SHIFT) != 0)
	    // A character with non-Shift modifiers should not be a valid
	    // character for i_CTRL-V_digit.
	    थांबा;

	जर ((State & MODE_CMDLINE) == 0 && MB_BYTE2LEN_CHECK(nc) == 1)
	    add_to_showcmd(nc);
	जर (nc == 'x' || nc == 'X')
	    hex = TRUE;
	अन्यथा जर (nc == 'o' || nc == 'O')
	    octal = TRUE;
	अन्यथा जर (nc == 'u' || nc == 'U')
	    unicode = nc;
	अन्यथा
	{
	    जर (hex || unicode != 0)
	    {
		जर (!vim_isxdigit(nc))
		    थांबा;
		cc = cc * 16 + hex2nr(nc);
	    }
	    अन्यथा जर (octal)
	    {
		जर (nc < '0' || nc > '7')
		    थांबा;
		cc = cc * 8 + nc - '0';
	    }
	    अन्यथा
	    {
		जर (!VIM_ISDIGIT(nc))
		    थांबा;
		cc = cc * 10 + nc - '0';
	    }

	    ++i;
	}

	जर (cc > 255 && unicode == 0)
	    cc = 255;		// limit range to 0-255
	nc = 0;

	जर (hex)		// hex: up to two chars
	{
	    जर (i >= 2)
		थांबा;
	}
	अन्यथा जर (unicode)	// Unicode: up to four or eight chars
	{
	    जर ((unicode == 'u' && i >= 4) || (unicode == 'U' && i >= 8))
		थांबा;
	}
	अन्यथा जर (i >= 3)	// decimal or octal: up to three chars
	    थांबा;
    }
    जर (i == 0)	    // no number entered
    {
	जर (nc == K_ZERO)   // NUL is stored as NL
	{
	    cc = '\n';
	    nc = 0;
	}
	अन्यथा
	{
	    cc = nc;
	    nc = 0;
	}
    }

    जर (cc == 0)	// NUL is stored as NL
	cc = '\n';
    जर (enc_dbcs && (cc & 0xff) == 0)
	cc = '?';	// don't accept an illegal DBCS char, the NUL in the
			// second byte will cause trouble!

    --no_mapping;
#ifdef FEAT_GUI
    जर (gui.in_use)
    {
	--allow_किल्ल्या;
	जर (no_Reduce_किल्ल्या)
	    --no_reduce_किल्ल्या;
    }
#endif
    जर (nc)
    {
	vungetc(nc);
	// A character typed with i_CTRL-V_digit cannot have modifiers.
	mod_mask = 0;
    }
    got_int = FALSE;	    // CTRL-C typed after CTRL-V is not an interrupt
    परत cc;
}

/*
 * Insert character, taking care of special keys and mod_mask
 */
    स्थिर शून्य
घाला_special(
    पूर्णांक	    c,
    पूर्णांक	    allow_modmask,
    पूर्णांक	    ctrlv)	    // c was typed after CTRL-V
{
    char_u  *p;
    पूर्णांक	    len;

    /*
     * Special function key, translate into "<Key>". Up to the last '>' is
     * inserted with ins_str(), so as not to replace characters in replace
     * mode.
     * Only use mod_mask for special keys, to avoid things like <S-Space>,
     * unless 'allow_modmask' is TRUE.
     */
#जर defined(MACOS_X) || defined(FEAT_GUI_GTK)
    // Command-key never produces a normal key
    जर (mod_mask & MOD_MASK_CMD)
	allow_modmask = TRUE;
#endif
    जर (IS_SPECIAL(c) || (mod_mask && allow_modmask))
    {
	p = get_special_किल्ली_name(c, mod_mask);
	len = (पूर्णांक)STRLEN(p);
	c = p[len - 1];
	जर (len > 2)
	{
	    जर (stop_arrow() == FAIL)
		परत;
	    p[len - 1] = NUL;
	    ins_str(p, len - 1);
	    AppendToRedobuffLit(p, -1);
	    ctrlv = FALSE;
	}
    }
    जर (stop_arrow() == OK)
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
    पूर्णांक		c,			// character to insert or NUL
    पूर्णांक		flags,			// INSCHAR_FORMAT, etc.
    पूर्णांक		second_indent)		// indent for second line if >= 0
{
    पूर्णांक		textwidth;
    char_u	*p;
    पूर्णांक		fo_ins_blank;
    पूर्णांक		force_format = flags & INSCHAR_FORMAT;

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
    जर (textwidth > 0
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
#जर defined(FEAT_EVAL)
	पूर्णांक     do_internal = TRUE;
	colnr_T virtcol = get_nolist_virtcol()
				  + char2cells(c != NUL ? c : gchar_cursor());

	जर (*curbuf->b_p_fex != NUL && (flags & INSCHAR_NO_FEX) == 0
		&& (force_format || virtcol > (colnr_T)textwidth))
	{
	    do_internal = (fex_format(curwin->w_cursor.lnum, 1L, c) != 0);
	    // It may be required to save for undo again, e.g. when setline()
	    // was called.
	    ins_need_undo = TRUE;
	}
	जर (do_internal)
#endif
	    internal_format(textwidth, second_indent, flags, c == NUL, c);
    }

    जर (c == NUL)	    // only formatting was wanted
	परत;

    // Check whether this character should end a comment.
    जर (did_ai && c == end_comment_pending)
    {
	char_u  *ओळ;
	char_u	lead_end[COM_MAX_LEN];	    // end-comment string
	पूर्णांक	मधला_len, end_len;
	पूर्णांक	i;

	/*
	 * Need to remove existing (middle) comment leader and insert end
	 * comment leader.  First, check what comment leader we can find.
	 */
	i = get_leader_len(ओळ = ml_get_curline(), &p, FALSE, TRUE);
	जर (i > 0 && vim_strchr(p, COM_मधला) != रिक्त)	// Just checking
	{
	    // Skip middle-comment string
	    पर्यंत (*p && p[-1] != ':')	// find end of middle flags
		++p;
	    मधला_len = copy_option_part(&p, lead_end, COM_MAX_LEN, ",");
	    // Don't count trailing white space for middle_len
	    पर्यंत (मधला_len > 0 && VIM_ISWHITE(lead_end[मधला_len - 1]))
		--मधला_len;

	    // Find the end-comment string
	    पर्यंत (*p && p[-1] != ':')	// find end of end flags
		++p;
	    end_len = copy_option_part(&p, lead_end, COM_MAX_LEN, ",");

	    // Skip white space before the cursor
	    i = curwin->w_cursor.col;
	    पर्यंत (--i >= 0 && VIM_ISWHITE(ओळ[i]))
		;
	    i++;

	    // Skip to before the middle leader
	    i -= मधला_len;

	    // Check some expected things before we go on
	    जर (i >= 0 && end_len > 0 && lead_end[end_len - 1] == end_comment_pending)
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

    जर (       !ISSPECIAL(c)
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
#define आदान_BUFLEN 100
	char_u		buf[आदान_BUFLEN + 1];
	पूर्णांक		i;
	colnr_T		virtcol = 0;

	buf[0] = c;
	i = 1;
	जर (textwidth > 0)
	    virtcol = get_nolist_virtcol();
	/*
	 * Stop the string when:
	 * - no more chars available
	 * - finding a special character (command key)
	 * - buffer is full
	 * - running into the 'textwidth' boundary
	 * - need to check for abbreviation: A non-word char after a word-char
	 */
	पर्यंत (	   (c = vpeekc()) != NUL
		&& !ISSPECIAL(c)
		&& (!has_mbyte || MB_BYTE2LEN_CHECK(c) == 1)
		&& i < आदान_BUFLEN
		&& (textwidth == 0
		    || (virtcol += byte2cells(buf[i - 1])) < (colnr_T)textwidth)
		&& !(!no_abbr && !vim_iswordc(c) && vim_iswordc(buf[i - 1])))
	{
#ifdef FEAT_RIGHTLEFT
	    c = vgetc();
	    जर (p_hkmap && किल्ली_Typed)
		c = hkmap(c);		    // Hebrew mode mapping
	    buf[i++] = c;
#अन्यथा
	    buf[i++] = vgetc();
#endif
	}

#ifdef FEAT_DIGRAPHS
	do_digraph(-1);			// clear digraphs
	do_digraph(buf[i-1]);		// may be the start of a digraph
#endif
	buf[i] = NUL;
	ins_str(buf, i);
	जर (flags & INSCHAR_CTRLV)
	{
	    redo_literal(*buf);
	    i = 1;
	}
	अन्यथा
	    i = 0;
	जर (buf[i] != NUL)
	    AppendToRedobuffLit(buf + i, -1);
    }
    अन्यथा
    {
	पूर्णांक		cc;

	जर (has_mbyte && (cc = (*mb_char2len)(c)) > 1)
	{
	    char_u	buf[MB_MAXBYTES + 1];

	    (*mb_char2bytes)(c, buf);
	    buf[cc] = NUL;
	    ins_char_bytes(buf, cc);
	    AppendCharToRedobuff(c);
	}
	अन्यथा
	{
	    ins_char(c);
	    जर (flags & INSCHAR_CTRLV)
		redo_literal(c);
	    अन्यथा
		AppendCharToRedobuff(c);
	}
    }
}

/*
 * Put a character in the redo buffer, for when just after a CTRL-V.
 */
    स्थिर शून्य
redo_literal(पूर्णांक c)
{
    char_u	buf[10];

    // Only digits need special treatment.  Translate them into a string of
    // three digits.
    जर (VIM_ISDIGIT(c))
    {
	vim_snprintf((अक्षर *)buf, आकार(buf), "%03d", c);
	AppendToRedobuff(buf);
    }
    अन्यथा
	AppendCharToRedobuff(c);
}

/*
 * start_arrow() is called when an arrow key is used in insert mode.
 * For undo/redo it resembles hitting the <ESC> key.
 */
    शून्य
start_arrow(
    pos_T    *end_घाला_pos)		// can be NULL
{
    start_arrow_common(end_घाला_pos, TRUE);
}

/*
 * Like start_arrow() but with end_change argument.
 * Will prepare for redo of CTRL-G U if "end_change" is FALSE.
 */
    स्थिर शून्य
start_arrow_with_change(
    pos_T    *end_घाला_pos,		// can be NULL
    पूर्णांक	      end_change)		// end undoable change
{
    start_arrow_common(end_घाला_pos, end_change);
    जर (!end_change)
    {
	AppendCharToRedobuff(Ctrl_G);
	AppendCharToRedobuff('U');
    }
}

    स्थिर शून्य
start_arrow_common(
    pos_T    *end_घाला_pos,		// can be NULL
    पूर्णांक	      end_change)		// end undoable change
{
    जर (!arrow_used && end_change)	// something has been inserted
    {
	AppendToRedobuff(ESC_STR);
	stop_घाला(end_घाला_pos, FALSE, FALSE);
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
    जर (spell_redraw_lnum != 0)
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
    पूर्णांक
stop_arrow(शून्य)
{
    जर (arrow_used)
    {
	Insstart = curwin->w_cursor;	// new insertion starts here
	जर (Insstart.col > Insstart_orig.col && !ins_need_undo)
	    // Don't update the original insert position when moved to the
	    // right, except when nothing was inserted yet.
	    update_Insstart_orig = FALSE;
	Insstart_textlen = (colnr_T)linetabsize_str(ml_get_curline());

	जर (u_save_cursor() == OK)
	{
	    arrow_used = FALSE;
	    ins_need_undo = FALSE;
	}

	ai_col = 0;
	जर (State & VREPLACE_FLAG)
	{
	    orig_ओळ_मोजणी = curbuf->b_ml.ml_ओळ_मोजणी;
	    vr_ओळी_changed = 1;
	}
	ResetRedobuff();
	AppendToRedobuff((char_u *)"1i");   // pretend we start an insertion
	new_घाला_skip = 2;
    }
    अन्यथा जर (ins_need_undo)
    {
	जर (u_save_cursor() == OK)
	{
	    // A command or event may have moved the cursor before the next
	    // edit. Pull Insstart back only when the cursor moved above it,
	    // so that later edits can properly decide whether an extra undo
	    // entry is needed. Advancing Insstart would mis-place '[ after a
	    // register paste.
	    जर (LT_POS(curwin->w_cursor, Insstart))
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

    परत (arrow_used || ins_need_undo ? FAIL : OK);
}

/*
 * Do a few things to stop inserting.
 * "end_insert_pos" is where insert ended.  It is NULL when we already jumped
 * to another window/buffer.
 */
    स्थिर शून्य
stop_घाला(
    pos_T	*end_घाला_pos,
    पूर्णांक		esc,			// called by ins_esc()
    पूर्णांक		nomove)			// <c-\><c-o>, don't move cursor
{
    पूर्णांक		cc;
    string_T	inserted;

    stop_redo_ins();
    replace_flush();		// abandon replace stack

    /*
     * Save the inserted text for later redo with ^@ and CTRL-A.
     * Don't do it when "restart_edit" was set and nothing was inserted,
     * otherwise CTRL-O w and then <Left> will clear "last_insert".
     */
    inserted = get_inserted();
    पूर्णांक added = inserted.string == रिक्त ? 0 : (पूर्णांक)inserted.length - new_घाला_skip;
    जर (did_restart_edit == 0 || added > 0)
    {
	vim_free(last_घाला.string);
	last_घाला = inserted;			    // structure copy
	last_घाला_skip = added < 0 ? 0 : new_घाला_skip;
    }
    अन्यथा
	vim_free(inserted.string);

    जर (!arrow_used && end_घाला_pos != रिक्त)
    {
	// Auto-format now.  It may seem strange to do this when stopping an
	// insertion (or moving the cursor), but it's required when appending
	// a line and having it end in a space.  But only do it when something
	// was actually inserted, otherwise undo won't work.
	जर (!ins_need_undo && has_format_option(FO_AUTO))
	{
	    pos_T   tpos = curwin->w_cursor;

	    // When the cursor is at the end of the line after a space the
	    // formatting will move it to the following word.  Avoid that by
	    // moving the cursor onto the space.
	    cc = 'x';
	    जर (curwin->w_cursor.col > 0 && gchar_cursor() == NUL)
	    {
		dec_cursor();
		cc = gchar_cursor();
		जर (!VIM_ISWHITE(cc))
		    curwin->w_cursor = tpos;
	    }

	    auto_format(TRUE, FALSE);

	    जर (VIM_ISWHITE(cc))
	    {
		जर (gchar_cursor() != NUL)
		    inc_cursor();
		// If the cursor is still at the same character, also keep
		// the "coladd".
		जर (gchar_cursor() == NUL
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
	जर (!nomove && did_ai && (esc || (vim_strchr(p_cpo, CPO_INDENT) == रिक्त
			&& curwin->w_cursor.lnum != end_घाला_pos->lnum))
		&& end_घाला_pos->lnum <= curbuf->b_ml.ml_ओळ_मोजणी)
	{
	    pos_T	tpos = curwin->w_cursor;
	    colnr_T	prev_col = end_घाला_pos->col;
	    colnr_T	strip_col;

	    curwin->w_cursor = *end_घाला_pos;
	    check_cursor_col();  // make sure it is not past the line

	    // Where the loop would actually start (back up if on NUL).
	    strip_col = curwin->w_cursor.col;
	    जर (gchar_cursor() == NUL && strip_col > 0)
		--strip_col;

	    // Don't strip if non-whitespace follows: setline() from a <Cmd>
	    // mapping or CursorHoldI autocmd may have inserted content.
	    जर (*skipwhite(ml_get_curline() + strip_col) == NUL)
	    {
		curwin->w_cursor.col = strip_col;
		साठी (;;)
		{
		    जर (gchar_cursor() == NUL && curwin->w_cursor.col > 0)
			--curwin->w_cursor.col;
		    cc = gchar_cursor();
		    जर (!VIM_ISWHITE(cc))
			थांबा;
		    जर (del_char(TRUE) == FAIL)
			थांबा;  // should not happen
		}
		जर (curwin->w_cursor.lnum != tpos.lnum)
		    curwin->w_cursor = tpos;
		अन्यथा जर (curwin->w_cursor.col < prev_col)
		{
		    // reset tpos, could have been invalidated in the loop above
		    tpos = curwin->w_cursor;
		    tpos.col++;
		    जर (cc != NUL && gchar_pos(&tpos) == NUL)
			++curwin->w_cursor.col;	// put cursor back on the NUL
		}

		// <C-S-Right> may have started Visual mode, adjust the position for
		// deleted characters.
		जर (VIsual_active)
		    check_visual_pos();
	    }
	    अन्यथा
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
    जर (end_घाला_pos != रिक्त)
    {
	curbuf->b_op_start = Insstart;
	curbuf->b_op_start_orig = Insstart_orig;
	curbuf->b_op_end = *end_घाला_pos;
    }
}

/*
 * Set the last inserted text to a single character.
 * Used for the replace command.
 */
    शून्य
set_last_घाला(पूर्णांक c)
{
    char_u	*s;

    vim_free(last_घाला.string);
    last_घाला.string = alloc(MB_MAXBYTES * 3 + 5);
    जर (last_घाला.string == रिक्त)
    {
	last_घाला.length = 0;
	परत;
    }

    s = last_घाला.string;
    // Use the CTRL-V only when entering a special char
    जर (c < ' ' || c == DEL)
	*s++ = Ctrl_V;
    s = add_char2buf(c, s);
    *s++ = ESC;
    *s = NUL;
    last_घाला.length = (माप_प्रकार)(s - last_घाला.string);
    last_घाला_skip = 0;
}

#जर defined(EXITFREE)
    शून्य
free_last_घाला(शून्य)
{
    VIM_CLEAR_STRING(last_घाला);
}
#endif

/*
 * Add character "c" to buffer "s".  Escape the special meaning of K_SPECIAL
 * and CSI.  Handle multi-byte characters.
 * Returns a pointer to after the added bytes.
 */
    char_u *
add_char2buf(पूर्णांक c, char_u *s)
{
    char_u	temp[MB_MAXBYTES + 1];
    पूर्णांक		i;
    पूर्णांक		len;

    len = (*mb_char2bytes)(c, temp);
    साठी (i = 0; i < len; ++i)
    {
	c = temp[i];
	// Need to escape K_SPECIAL and CSI like in the typeahead buffer.
	जर (c == K_SPECIAL)
	{
	    *s++ = K_SPECIAL;
	    *s++ = KS_SPECIAL;
	    *s++ = KE_FILLER;
	}
#ifdef FEAT_GUI
	अन्यथा जर (c == CSI)
	{
	    *s++ = CSI;
	    *s++ = KS_EXTRA;
	    *s++ = (पूर्णांक)KE_CSI;
	}
#endif
	अन्यथा
	    *s++ = c;
    }
    परत s;
}

/*
 * move cursor to start of line
 * if flags & BL_WHITE	move to first non-white
 * if flags & BL_SOL	move to first non-white if startofline is set,
 *			    otherwise keep "curswant" column
 * if flags & BL_FIX	don't leave the cursor on a NUL.
 */
    शून्य
beginline(पूर्णांक flags)
{
    जर ((flags & BL_SOL) && !p_sol)
	coladvance(curwin->w_curswant);
    अन्यथा
    {
	curwin->w_cursor.col = 0;
	curwin->w_cursor.coladd = 0;

	जर (flags & (BL_WHITE | BL_SOL))
	{
	    char_u  *ptr;

	    साठी (ptr = ml_get_curline(); VIM_ISWHITE(*ptr)
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

    पूर्णांक
oneright(शून्य)
{
    char_u	*ptr;
    पूर्णांक		l;

    जर (virtual_active())
    {
	pos_T	prevpos = curwin->w_cursor;

	// Adjust for multi-wide char (excluding TAB)
	ptr = ml_get_cursor();
	coladvance(getviscol() + ((*ptr != TAB
					  && vim_isprintc((*mb_ptr2char)(ptr)))
		    ? ptr2cells(ptr) : 1));
	curwin->w_set_curswant = true;
	// Return OK if the cursor moved, FAIL otherwise (at window edge).
	परत (prevpos.col != curwin->w_cursor.col
		    || prevpos.coladd != curwin->w_cursor.coladd) ? OK : FAIL;
    }

    ptr = ml_get_cursor();
    जर (*ptr == NUL)
	परत FAIL;	    // already at the very end

    जर (has_mbyte)
	l = (*mb_ptr2len)(ptr);
    अन्यथा
	l = 1;

    // move "l" bytes right, but don't end up on the NUL, unless 'virtualedit'
    // contains "onemore".
    जर (ptr[l] == NUL && (get_ve_flags() & VE_ONEMORE) == 0)
	परत FAIL;
    curwin->w_cursor.col += l;

    curwin->w_set_curswant = true;
    adjust_skipcol();
    परत OK;
}

    पूर्णांक
oneleft(शून्य)
{
    जर (virtual_active())
    {
#ifdef FEAT_LINEBREAK
	पूर्णांक width;
#endif
	पूर्णांक v = getviscol();

	जर (v == 0)
	    परत FAIL;

#ifdef FEAT_LINEBREAK
	// We might get stuck on 'showbreak', skip over it.
	width = 1;
	साठी (;;)
	{
	    coladvance(v - width);
	    // getviscol() is slow, skip it when 'showbreak' is empty,
	    // 'breakindent' is not set and there are no multi-byte
	    // characters
	    जर ((*get_showbreak_मूल्य(curwin) == NUL && !curwin->w_p_bri
					     && !has_mbyte) || getviscol() < v)
		थांबा;
	    ++width;
	}
#अन्यथा
	coladvance(v - 1);
#endif

	जर (curwin->w_cursor.coladd == 1)
	{
	    char_u *ptr;

	    // Adjust for multi-wide char (not a TAB)
	    ptr = ml_get_cursor();
	    जर (*ptr != TAB && vim_isprintc((*mb_ptr2char)(ptr))
							 && ptr2cells(ptr) > 1)
		curwin->w_cursor.coladd = 0;
	}

	curwin->w_set_curswant = true;
	adjust_skipcol();
	परत OK;
    }

    जर (curwin->w_cursor.col == 0)
	परत FAIL;

    curwin->w_set_curswant = true;
    --curwin->w_cursor.col;

    // if the character on the left of the current cursor is a multi-byte
    // character, move to its first byte
    जर (has_mbyte)
	mb_adjust_cursor();
    adjust_skipcol();
    परत OK;
}

/*
 * Move the cursor up "n" lines in window "wp".
 * Takes care of closed folds.
 */
    शून्य
cursor_up_inner(win_T *wp, दीर्घ n)
{
    linenr_T	lnum = wp->w_cursor.lnum;

    जर (n >= lnum)
	lnum = 1;
    अन्यथा
#ifdef FEAT_FOLDING
	जर (hasAnyFolding(wp))
    {
	/*
	 * Count each sequence of folded lines as one logical line.
	 */
	// go to the start of the current fold
	(शून्य)hasFoldingWin(wp, lnum, &lnum, रिक्त, TRUE, रिक्त);

	पर्यंत (n--)
	{
	    // move up one line
	    --lnum;
	    जर (lnum <= 1)
		थांबा;
	    // If we entered a fold, move to the beginning, unless in
	    // Insert mode or when 'foldopen' contains "all": it will open
	    // in a moment.
	    जर (n > 0 || !((State & MODE_घाला) || (fdo_flags & FDO_ALL)))
		(शून्य)hasFoldingWin(wp, lnum, &lnum, रिक्त, TRUE, रिक्त);
	}
	जर (lnum < 1)
	    lnum = 1;
    }
    अन्यथा
#endif
	lnum -= n;

    wp->w_cursor.lnum = lnum;
}

    पूर्णांक
cursor_up(
    दीर्घ	n,
    पूर्णांक		upd_topline)	    // When TRUE: update topline
{
    // This fails if the cursor is already in the first line or the count is
    // larger than the line number and '-' is in 'cpoptions'
    linenr_T lnum = curwin->w_cursor.lnum;
    जर (n > 0 && (lnum <= 1
		       || (n >= lnum && vim_strchr(p_cpo, CPO_MINUS) != रिक्त)))
	परत FAIL;
    cursor_up_inner(curwin, n);

    // try to advance to the column we want to be at
    coladvance(curwin->w_curswant);

    जर (upd_topline)
	update_topline();	// make sure curwin->w_topline is valid

    परत OK;
}

/*
 * Move the cursor down "n" lines in window "wp".
 * Takes care of closed folds.
 */
    शून्य
cursor_down_inner(win_T *wp, दीर्घ n)
{
    linenr_T	lnum = wp->w_cursor.lnum;
    linenr_T	ओळ_मोजणी = wp->w_buffer->b_ml.ml_ओळ_मोजणी;

    जर (lnum + n >= ओळ_मोजणी)
	lnum = ओळ_मोजणी;
    अन्यथा
#ifdef FEAT_FOLDING
	जर (hasAnyFolding(wp))
    {
	linenr_T	last;

	// count each sequence of folded lines as one logical line
	पर्यंत (n--)
	{
	    जर (hasFoldingWin(wp, lnum, रिक्त, &last, TRUE, रिक्त))
		lnum = last + 1;
	    अन्यथा
		++lnum;
	    जर (lnum >= ओळ_मोजणी)
		थांबा;
	}
	जर (lnum > ओळ_मोजणी)
	    lnum = ओळ_मोजणी;
    }
    अन्यथा
#endif
	lnum += n;

    wp->w_cursor.lnum = lnum;
}

/*
 * Cursor down a number of logical lines.
 */
    पूर्णांक
cursor_down(
    दीर्घ	n,
    पूर्णांक		upd_topline)	    // When TRUE: update topline
{
    linenr_T	lnum = curwin->w_cursor.lnum;
    linenr_T	ओळ_मोजणी = curwin->w_buffer->b_ml.ml_ओळ_मोजणी;
    // This fails if the cursor is already in the last (folded) line, or would
    // move beyond the last line and '-' is in 'cpoptions'.
#ifdef FEAT_FOLDING
    hasFoldingWin(curwin, lnum, रिक्त, &lnum, TRUE, रिक्त);
#endif
    जर (n > 0
	    && (lnum >= ओळ_मोजणी
		|| (lnum + n > ओळ_मोजणी
				     && vim_strchr(p_cpo, CPO_MINUS) != रिक्त)))
	परत FAIL;
    cursor_down_inner(curwin, n);

    // try to advance to the column we want to be at
    coladvance(curwin->w_curswant);

    जर (upd_topline)
	update_topline();	// make sure curwin->w_topline is valid

    परत OK;
}

/*
 * Stuff the last inserted text in the read buffer.
 * Last_insert actually is a copy of the redo buffer, so we
 * first have to remove the command.
 */
    पूर्णांक
stuff_inserted(
    पूर्णांक	    c,		// Command character to be inserted
    दीर्घ    मोजणी,	// Repeat this many times
    पूर्णांक	    no_esc)	// Don't add an ESC at the end
{
    string_T	घाला;				// text to be inserted
    char_u	last = ' ';

    घाला = get_last_घाला();
    जर (घाला.string == रिक्त)
    {
	emsg(_(e_no_inserted_text_yet));
	परत FAIL;
    }

    // may want to stuff the command character, to start Insert mode
    जर (c != NUL)
	stuffcharReadbuff(c);

    जर (घाला.length > 0)
    {
	char_u	*p;

	// look for the last ESC in 'insert'
	साठी (p = घाला.string + घाला.length - 1; p >= घाला.string; --p)
	{
	    जर (*p == ESC)
	    {
		घाला.length = (माप_प्रकार)(p - घाला.string);
		थांबा;
	    }
	}
    }

    जर (घाला.length > 0)
    {
	char_u	*p = घाला.string + घाला.length - 1;

	// when the last char is either "0" or "^" it will be quoted if no ESC
	// comes after it OR if it will insert more than once and "ptr"
	// starts with ^D.	-- Acevedo
	जर ((*p == '0' || *p == '^')
		&& (no_esc || (*घाला.string == Ctrl_D && मोजणी > 1)))
	{
	    last = *p;
	    --घाला.length;
	}
    }

    करा
    {
	stuffReadbuffLen(घाला.string, (दीर्घ)घाला.length);
	// a trailing "0" is inserted as "<C-V>048", "^" as "<C-V>^"
	निवड (last)
	{
	स्थिती '0':
#define TEXT_TO_घाला "\026\060\064\070"
	    stuffReadbuffLen((char_u *)TEXT_TO_घाला, STRLEN_LITERAL(TEXT_TO_घाला));
#undef TEXT_TO_घाला
	    थांबा;

	स्थिती '^':
#define TEXT_TO_घाला "\026^"
	    stuffReadbuffLen((char_u *)TEXT_TO_घाला, STRLEN_LITERAL(TEXT_TO_घाला));
#undef TEXT_TO_घाला
	    थांबा;

	पूर्वनिर्धारित:
	    थांबा;
	}
    }
    पर्यंत (--मोजणी > 0);

    // may want to stuff a trailing ESC, to get out of Insert mode
    जर (!no_esc)
	stuffcharReadbuff(ESC);

    परत OK;
}

    string_T
get_last_घाला(शून्य)
{
    string_T घाला = {रिक्त, 0};

    जर (last_घाला.string != रिक्त)
    {
	घाला.string = last_घाला.string + last_घाला_skip;
	घाला.length = (माप_प्रकार)(last_घाला.length - last_घाला_skip);
    }

    परत घाला;
}

/*
 * Get last inserted string, and remove trailing <Esc>.
 * Returns pointer to allocated memory (must be freed) or NULL.
 */
    char_u *
get_last_घाला_save(शून्य)
{
    string_T	घाला = get_last_घाला();
    char_u	*s;

    जर (घाला.string == रिक्त)
	परत रिक्त;
    s = vim_strnsave(घाला.string, घाला.length);
    जर (s == रिक्त)
	परत रिक्त;

    जर (घाला.length > 0 && s[घाला.length - 1] == ESC)	// remove trailing ESC
	s[--घाला.length] = NUL;
    परत s;
}

/*
 * Check the word in front of the cursor for an abbreviation.
 * Called when the non-id character "c" has been entered.
 * When an abbreviation is recognized it is removed from the text and
 * the replacement string is inserted in typebuf.tb_buf[], followed by "c".
 */
    स्थिर पूर्णांक
echeck_abbr(पूर्णांक c)
{
    // Don't check for abbreviation in paste mode, when disabled and just
    // after moving around with cursor keys.
    जर (p_paste || no_abbr || arrow_used)
	परत FALSE;

    परत check_abbr(c, ml_get_curline(), curwin->w_cursor.col,
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

स्थिर char_u	*replace_चळत = रिक्त;
स्थिर दीर्घ	replace_चळत_nr = 0;	    // next entry in replace stack
स्थिर दीर्घ	replace_चळत_len = 0;	    // max. number of entries

    शून्य
replace_ढकला(
    पूर्णांक	    c)	    // character that is replaced (NUL is none)
{
    char_u  *p;

    जर (replace_चळत_nr < replace_offset)	// nothing to do
	परत;
    जर (replace_चळत_len <= replace_चळत_nr)
    {
	replace_चळत_len += 50;
	p = ALLOC_MULT(char_u, replace_चळत_len);
	जर (p == रिक्त)	    // out of memory
	{
	    replace_चळत_len -= 50;
	    परत;
	}
	जर (replace_चळत != रिक्त)
	{
	    mch_memmove(p, replace_चळत,
				 (माप_प्रकार)(replace_चळत_nr * आकार(char_u)));
	    vim_free(replace_चळत);
	}
	replace_चळत = p;
    }
    p = replace_चळत + replace_चळत_nr - replace_offset;
    जर (replace_offset)
	mch_memmove(p + 1, p, (माप_प्रकार)(replace_offset * आकार(char_u)));
    *p = c;
    ++replace_चळत_nr;
}

/*
 * Push a character onto the replace stack.  Handles a multi-byte character in
 * reverse byte order, so that the first byte is popped off first.
 * Return the number of bytes done (includes composing characters).
 */
    पूर्णांक
replace_ढकला_mb(char_u *p)
{
    पूर्णांक l = (*mb_ptr2len)(p);
    पूर्णांक j;

    साठी (j = l - 1; j >= 0; --j)
	replace_ढकला(p[j]);
    परत l;
}

/*
 * Pop one item from the replace stack.
 * return -1 if stack empty
 * return replaced character or NUL otherwise
 */
    स्थिर पूर्णांक
replace_काढा(शून्य)
{
    जर (replace_चळत_nr == 0)
	परत -1;
    परत (पूर्णांक)replace_चळत[--replace_चळत_nr];
}

/*
 * Join the top two items on the replace stack.  This removes to "off"'th NUL
 * encountered.
 */
    शून्य
replace_join(
    पूर्णांक	    off)	// offset for which NUL to remove
{
    पूर्णांक	    i;

    साठी (i = replace_चळत_nr; --i >= 0; )
	जर (replace_चळत[i] == NUL && off-- <= 0)
	{
	    --replace_चळत_nr;
	    mch_memmove(replace_चळत + i, replace_चळत + i + 1,
					      (माप_प्रकार)(replace_चळत_nr - i));
	    परत;
	}
}

/*
 * Pop bytes from the replace stack until a NUL is found, and insert them
 * before the cursor.  Can only be used in MODE_REPLACE or MODE_VREPLACE state.
 */
    स्थिर शून्य
replace_काढा_ins(शून्य)
{
    पूर्णांक	    cc;
    पूर्णांक	    oldState = State;

    State = MODE_NORMAL;			// don't want MODE_REPLACE here
    पर्यंत ((cc = replace_काढा()) > 0)
    {
	mb_replace_काढा_ins(cc);
	dec_cursor();
    }
    State = oldState;
}

/*
 * Insert bytes popped from the replace stack. "cc" is the first byte.  If it
 * indicates a multi-byte char, pop the other bytes too.
 */
    स्थिर शून्य
mb_replace_काढा_ins(पूर्णांक cc)
{
    पूर्णांक		n;
    char_u	buf[MB_MAXBYTES + 1];
    पूर्णांक		i;
    पूर्णांक		c;

    जर (has_mbyte && (n = MB_BYTE2LEN(cc)) > 1)
    {
	buf[0] = cc;
	साठी (i = 1; i < n; ++i)
	    buf[i] = replace_काढा();
	ins_bytes_len(buf, n);
    }
    अन्यथा
	ins_char(cc);

    जर (enc_utf8)
	// Handle composing chars.
	साठी (;;)
	{
	    c = replace_काढा();
	    जर (c == -1)	    // stack empty
		थांबा;
	    जर ((n = MB_BYTE2LEN(c)) == 1)
	    {
		// Not a multi-byte char, put it back.
		replace_ढकला(c);
		थांबा;
	    }

	    buf[0] = c;
	    साठी (i = 1; i < n; ++i)
		buf[i] = replace_काढा();
	    जर (utf_iscomposing(utf_ptr2char(buf)))
		ins_bytes_len(buf, n);
	    अन्यथा
	    {
		// Not a composing char, put it back.
		साठी (i = n - 1; i >= 0; --i)
		    replace_ढकला(buf[i]);
		थांबा;
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
    VIM_CLEAR(replace_चळत);
    replace_चळत_len = 0;
    replace_चळत_nr = 0;
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
replace_do_bs(पूर्णांक limit_col)
{
    पूर्णांक		cc;
    पूर्णांक		orig_len = 0;
    पूर्णांक		ins_len;
    पूर्णांक		orig_vcols = 0;
    colnr_T	start_vcol;
    char_u	*p;
    पूर्णांक		i;
    पूर्णांक		vcol;

    cc = replace_काढा();
    जर (cc > 0)
    {
#ifdef FEAT_PROP_POPUP
	माप_प्रकार	len_before = 0;  // init to shut up GCC

	जर (curbuf->b_has_textprop)
	{
	    // Do not adjust text properties for individual delete and insert
	    // operations, do it afterwards on the resulting text.
	    len_before = ml_get_curline_len();
	    ++text_prop_frozen;
	}
#endif
	जर (State & VREPLACE_FLAG)
	{
	    // Get the number of screen cells used by the character we are
	    // going to delete.
	    getvcol(curwin, &curwin->w_cursor, रिक्त, &start_vcol, रिक्त, 0);
	    orig_vcols = chartabsize(ml_get_cursor(), start_vcol);
	}
	जर (has_mbyte)
	{
	    (शून्य)del_char_after_col(limit_col);
	    जर (State & VREPLACE_FLAG)
		orig_len = ml_get_cursor_len();
	    replace_ढकला(cc);
	}
	अन्यथा
	{
	    pchar_cursor(cc);
	    जर (State & VREPLACE_FLAG)
		orig_len = ml_get_cursor_len() - 1;
	}
	replace_काढा_ins();

	जर (State & VREPLACE_FLAG)
	{
	    // Get the number of screen cells used by the inserted characters
	    p = ml_get_cursor();
	    ins_len = ml_get_cursor_len() - orig_len;
	    vcol = start_vcol;
	    साठी (i = 0; i < ins_len; ++i)
	    {
		vcol += chartabsize(p + i, vcol);
		i += (*mb_ptr2len)(p + i) - 1;
	    }
	    vcol -= start_vcol;

	    // Delete spaces that were inserted after the cursor to keep the
	    // text aligned.
	    curwin->w_cursor.col += ins_len;
	    पर्यंत (vcol > orig_vcols && gchar_cursor() == ' ')
	    {
		del_char(FALSE);
		++orig_vcols;
	    }
	    curwin->w_cursor.col -= ins_len;
	}

	// mark the buffer as changed and prepare for displaying
	changed_bytes(curwin->w_cursor.lnum, curwin->w_cursor.col);

#ifdef FEAT_PROP_POPUP
	जर (curbuf->b_has_textprop)
	{
	    माप_प्रकार len_now = ml_get_curline_len();

	    --text_prop_frozen;
	    adjust_prop_columns(curwin->w_cursor.lnum, curwin->w_cursor.col,
					   (पूर्णांक)(len_now - len_before), 0);
	}
#endif
    }
    अन्यथा जर (cc == 0)
	(शून्य)del_char_after_col(limit_col);
}

#जर defined(FEAT_RIGHTLEFT)
/*
 * Map Hebrew keyboard when in hkmap mode.
 */
    पूर्णांक
hkmap(पूर्णांक c)
{
    जर (p_hkmapp)   // phonetic mapping, by Ilya Dogolazky
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

	जर (c == 'N' || c == 'M' || c == 'P' || c == 'C' || c == 'Z')
	    परत (पूर्णांक)(map[CharOrd(c)] - 1 + p_aleph);
							    // '-1'='sofit'
	अन्यथा जर (c == 'x')
	    परत 'X';
	अन्यथा जर (c == 'q')
	    परत '\''; // {geresh}={'}
	अन्यथा जर (c == 246)
	    परत ' ';  // \"o --> ' ' for a german keyboard
	अन्यथा जर (c == 228)
	    परत ' ';  // \"a --> ' '      -- / --
	अन्यथा जर (c == 252)
	    परत ' ';  // \"u --> ' '      -- / --
	// NOTE: islower() does not do the right thing for us on Linux so we
	// do this the same was as 5.7 and previous, so it works correctly on
	// all systems.  Specifically, the e.g. Delete and Arrow keys are
	// munged and won't work if e.g. searching for Hebrew text.
	अन्यथा जर (c >= 'a' && c <= 'z')
	    परत (पूर्णांक)(map[Char_Ord_खालचा(c)] + p_aleph);
	अन्यथा
	    परत c;
    }
    अन्यथा
    {
	निवड (c)
	{
	    स्थिती '`':	परत ';';
	    स्थिती '/':	परत '.';
	    स्थिती '\'':	परत ',';
	    स्थिती 'q':	परत '/';
	    स्थिती 'w':	परत '\'';

			// Hebrew letters - set offset from 'a'
	    स्थिती ',':	c = '{'; थांबा;
	    स्थिती '.':	c = 'v'; थांबा;
	    स्थिती ';':	c = 't'; थांबा;
	    पूर्वनिर्धारित: {
			 स्थिर अक्षर str[] = "zqbcxlsjphmkwonu ydafe rig";

			 जर (c < 'a' || c > 'z')
			     परत c;
			 c = str[Char_Ord_खालचा(c)];
			 थांबा;
		     }
	}

	परत (पूर्णांक)(Char_Ord_खालचा(c) + p_aleph);
    }
}
#endif

    स्थिर शून्य
ins_reg(शून्य)
{
    पूर्णांक		need_redraw = FALSE;
    पूर्णांक		regname;
    पूर्णांक		literally = 0;
    पूर्णांक		vis_active = VIsual_active;

    /*
     * If we are going to wait for a character, show a '"'.
     */
    pc_status = PC_STATUS_UNSET;
    जर (redrawing() && !char_avail())
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
    ++allow_किल्ल्या;
    regname = plain_vgetc();
    LANGMAP_ADJUST(regname, TRUE);
    जर (regname == Ctrl_R || regname == Ctrl_O || regname == Ctrl_P)
    {
	// Get a third key for literal register insertion
	literally = regname;
	add_to_showcmd_c(literally);
	regname = plain_vgetc();
	LANGMAP_ADJUST(regname, TRUE);
    }
    --no_mapping;
    --allow_किल्ल्या;

#ifdef FEAT_EVAL
    // Don't call u_sync() while typing the expression or giving an error
    // message for it. Only call it explicitly.
    ++no_u_sync;
    जर (regname == '=')
    {
	pos_T	curpos = curwin->w_cursor;
# ifdef HAVE_आदान_METHOD
	पूर्णांक	im_on = im_get_status();
# endif
	// Sync undo when evaluating the expression calls setline() or
	// append(), so that it can be undone separately.
	u_sync_once = 2;

	regname = get_अभिव्यक्ती_register();

	// Cursor may be moved back a column.
	curwin->w_cursor = curpos;
	check_cursor();
# ifdef HAVE_आदान_METHOD
	// Restore the Input Method.
	जर (im_on)
	    im_set_active(TRUE);
# endif
    }
    जर (regname == NUL || !valid_yank_reg(regname, FALSE))
    {
	vim_beep(BO_REG);
	need_redraw = TRUE;	// remove the '"'
    }
    अन्यथा
    {
#endif
	जर (literally == Ctrl_O || literally == Ctrl_P)
	{
	    // Append the command to the redo buffer.
	    AppendCharToRedobuff(Ctrl_R);
	    AppendCharToRedobuff(literally);
	    AppendCharToRedobuff(regname);

	    do_put(regname, रिक्त, BACKWARD, 1L,
		 (literally == Ctrl_P ? PUT_FIXINDENT : 0) | PUT_CURSEND);
	}
	अन्यथा जर (घाला_reg(regname, literally) == FAIL)
	{
	    vim_beep(BO_REG);
	    need_redraw = TRUE;	// remove the '"'
	}
	अन्यथा जर (stop_घाला_mode)
	    // When the '=' register was used and a function was invoked that
	    // did ":stopinsert" then stuff_empty() returns FALSE but we won't
	    // insert anything, need to remove the '"'
	    need_redraw = TRUE;

#ifdef FEAT_EVAL
    }
    --no_u_sync;
    जर (u_sync_once == 1)
	ins_need_undo = TRUE;
    u_sync_once = 0;
#endif
    clear_showcmd();

    // If the inserted register is empty, we need to remove the '"'
    जर (need_redraw || stuff_empty())
	edit_unputchar();

    // Disallow starting Visual mode here, would get a weird mode.
    जर (!vis_active && VIsual_active)
	end_visual_mode();
}

/*
 * CTRL-G commands in Insert mode.
 */
    स्थिर शून्य
ins_ctrl_g(शून्य)
{
    पूर्णांक		c;

    // Right after CTRL-X the cursor will be after the ruler.
    setcursor();

    /*
     * Don't map the second key. This also prevents the mode message to be
     * deleted when ESC is hit.
     */
    ++no_mapping;
    ++allow_किल्ल्या;
    c = plain_vgetc();
    --no_mapping;
    --allow_किल्ल्या;
    निवड (c)
    {
	// CTRL-G k and CTRL-G <Up>: cursor up to Insstart.col
	स्थिती K_UP:
	स्थिती Ctrl_K:
	स्थिती 'k': ins_up(TRUE);
		  थांबा;

	// CTRL-G j and CTRL-G <Down>: cursor down to Insstart.col
	स्थिती K_DOWN:
	स्थिती Ctrl_J:
	स्थिती 'j': ins_down(TRUE);
		  थांबा;

	// CTRL-G u: start new undoable edit
	स्थिती 'u': u_sync(TRUE);
		  ins_need_undo = TRUE;

		  // Need to reset Insstart, esp. because a BS that joins
		  // a line to the previous one must save for undo.
		  update_Insstart_orig = FALSE;
		  Insstart = curwin->w_cursor;
		  थांबा;

	// CTRL-G U: do not break undo with the next char
	स्थिती 'U':
		  // Allow one left/right cursor movement with the next char,
		  // without breaking undo.
		  dont_sync_undo = MAYBE;
		  थांबा;

	स्थिती ESC:
		  // Esc after CTRL-G cancels it.
		  थांबा;

	// Unknown CTRL-G command, reserved for future expansion.
	पूर्वनिर्धारित:  vim_beep(BO_CTRLG);
    }
}

/*
 * CTRL-^ in Insert mode.
 */
    स्थिर शून्य
ins_ctrl_hat(शून्य)
{
    जर (map_to_exists_mode((char_u *)"", MODE_LANGMAP, FALSE))
    {
	// ":lmap" mappings exists, Toggle use of ":lmap" mappings.
	जर (State & MODE_LANGMAP)
	{
	    curbuf->b_p_iminsert = B_IMODE_NONE;
	    State &= ~MODE_LANGMAP;
	}
	अन्यथा
	{
	    curbuf->b_p_iminsert = B_IMODE_LMAP;
	    State |= MODE_LANGMAP;
#ifdef HAVE_आदान_METHOD
	    im_set_active(FALSE);
#endif
	}
    }
#ifdef HAVE_आदान_METHOD
    अन्यथा
    {
	// There are no ":lmap" mappings, toggle IM
	जर (im_get_status())
	{
	    curbuf->b_p_iminsert = B_IMODE_NONE;
	    im_set_active(FALSE);
	}
	अन्यथा
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
    जर (gui.in_use)
	gui_update_cursor(TRUE, FALSE);
#endif
#जर defined(FEAT_KEYMAP)
    // Show/unshow value of 'keymap' in status lines.
    status_redraw_curbuf();
#endif
}

/*
 * Handle ESC in insert mode.
 * Returns TRUE when leaving insert mode, FALSE when going to repeat the
 * insert.
 */
    स्थिर पूर्णांक
ins_esc(
    दीर्घ	*मोजणी,
    पूर्णांक		cmdchar,
    पूर्णांक		nomove)	    // don't move cursor
{
    पूर्णांक		temp;
    स्थिर पूर्णांक	disabled_redraw = FALSE;
#ifdef FEAT_CONCEAL
    // Remember if the cursor line was concealed before changing State.
    पूर्णांक		cursor_ओळ_was_concealed = curwin->w_p_cole > 0
						&& conceal_cursor_ओळ(curwin);
#endif

#ifdef FEAT_SPELL
    check_spell_redraw();
#endif

    temp = curwin->w_cursor.col;
    जर (disabled_redraw)
    {
	जर (RedrawingDisabled > 0)
	    --RedrawingDisabled;
	disabled_redraw = FALSE;
    }
    जर (!arrow_used)
    {
	/*
	 * Don't append the ESC for "r<CR>" and "grx".
	 * When 'insertmode' is set only CTRL-L stops Insert mode.  Needed for
	 * when "count" is non-zero.
	 */
	जर (cmdchar != 'r' && cmdchar != 'v')
	    AppendToRedobuff(p_im ? (char_u *)"\014" : ESC_STR);

	/*
	 * Repeating insert may take a long time.  Check for
	 * interrupt now and then.
	 */
	जर (*मोजणी > 0)
	{
	    ओळ_breakcheck();
	    जर (got_int)
		*मोजणी = 0;
	}

	जर (--*मोजणी > 0)	// repeat what was typed
	{
	    // Vi repeats the insert without replacing characters.
	    जर (vim_strchr(p_cpo, CPO_REPLCNT) != रिक्त)
		State &= ~REPLACE_FLAG;

	    (शून्य)start_redo_ins();
	    जर (cmdchar == 'r' || cmdchar == 'v')
		stuffRedoReadbuff(ESC_STR);	// no ESC in redo buffer
	    ++RedrawingDisabled;
	    disabled_redraw = TRUE;
	    परत FALSE;	// repeat the insert
	}
	stop_घाला(&curwin->w_cursor, TRUE, nomove);
	undisplay_dollar();
    }

    जर (cmdchar != 'r' && cmdchar != 'v')
	ins_apply_autocmds(EVENT_INSERTLEAVEPRE);

    // When an autoindent was removed, curswant stays after the
    // indent
    जर (restart_edit == NUL && (colnr_T)temp == curwin->w_cursor.col)
	curwin->w_set_curswant = true;

    // Remember the last Insert position in the '^ mark.
    जर ((cmdmod.cmod_flags & CMOD_KEEPJUMPS) == 0)
	curbuf->b_last_घाला = curwin->w_cursor;

    /*
     * The cursor should end up on the last inserted character.
     * Don't do it for CTRL-O, unless past the end of the line.
     */
    जर (!nomove
	    && (curwin->w_cursor.col != 0
		|| curwin->w_cursor.coladd > 0)
	    && (restart_edit == NUL
		   || (gchar_cursor() == NUL && !VIsual_active))
#ifdef FEAT_RIGHTLEFT
	    && !revins_on
#endif
				      )
    {
	जर (curwin->w_cursor.coladd > 0 || get_ve_flags() == VE_ALL)
	{
	    oneleft();
	    जर (restart_edit != NUL)
		++curwin->w_cursor.coladd;
	}
	अन्यथा
	{
	    --curwin->w_cursor.col;
	    curwin->w_valid &= ~(VALID_WCOL|VALID_VIRTCOL);
	    // Correct cursor for multi-byte character.
	    जर (has_mbyte)
		mb_adjust_cursor();
	}
    }

#ifdef HAVE_आदान_METHOD
    // Disable IM to allow typing English directly for Normal mode commands.
    // When ":lmap" is enabled don't change 'iminsert' (IM can be enabled as
    // well).
    जर (!(State & MODE_LANGMAP))
	im_save_status(&curbuf->b_p_iminsert);
    im_set_active(FALSE);
#endif

    State = MODE_NORMAL;
    may_trigger_modechanged();
    // need to position cursor again when on a TAB and when on a char with
    // virtual text.
    जर (gchar_cursor() == TAB
#ifdef FEAT_PROP_POPUP
	    || curbuf->b_has_textprop
#endif
       )
	curwin->w_valid &= ~(VALID_WROW|VALID_WCOL|VALID_VIRTCOL);

    setmouse();
#ifdef CURSOR_SHAPE
    ui_cursor_shape();		// may show different cursor shape
#endif
    जर (!p_ek)
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
    conceal_check_cursor_ओळ(cursor_ओळ_was_concealed);
#endif

    // When recording or for CTRL-O, need to display the new mode.
    // Otherwise remove the mode message.
    जर (reg_recording != 0 || restart_edit != NUL)
	showmode();
    अन्यथा जर (p_smd && (got_int || !skip_showmode()))
	संदेश("");

    परत TRUE;	    // exit Insert mode
}

#ifdef FEAT_RIGHTLEFT
/*
 * Toggle language: hkmap and revins_on.
 * Move to end of reverse inserted text.
 */
    स्थिर शून्य
ins_ctrl_(शून्य)
{
    जर (revins_on && revins_chars && revins_scol >= 0)
    {
	पर्यंत (gchar_cursor() != NUL && revins_chars--)
	    ++curwin->w_cursor.col;
    }
    p_ri = !p_ri;
    revins_on = (State == MODE_घाला && p_ri);
    जर (revins_on)
    {
	revins_scol = curwin->w_cursor.col;
	revins_legal++;
	revins_chars = 0;
	undisplay_dollar();
    }
    अन्यथा
	revins_scol = -1;
    p_hkmap = curwin->w_p_rl ^ p_ri;    // be consistent!
    showmode();
}
#endif

/*
 * If 'keymodel' contains "startsel", may start selection.
 * Returns TRUE when a CTRL-O and other keys stuffed.
 */
    स्थिर पूर्णांक
ins_start_select(पूर्णांक c)
{
    जर (!km_startsel)
	परत FALSE;
    निवड (c)
    {
	स्थिती K_KHOME:
	स्थिती K_KEND:
	स्थिती K_PAGEUP:
	स्थिती K_KPAGEUP:
	स्थिती K_PAGEDOWN:
	स्थिती K_KPAGEDOWN:
#ifdef MACOS_X
	स्थिती K_डावा:
	स्थिती K_उजवा:
	स्थिती K_UP:
	स्थिती K_DOWN:
	स्थिती K_END:
	स्थिती K_HOME:
#endif
	    जर (!(mod_mask & MOD_MASK_SHIFT))
		थांबा;
	    // FALLTHROUGH
	स्थिती K_S_डावा:
	स्थिती K_S_उजवा:
	स्थिती K_S_UP:
	स्थिती K_S_DOWN:
	स्थिती K_S_END:
	स्थिती K_S_HOME:
	    // Start selection right away, the cursor can move with CTRL-O when
	    // beyond the end of the line.
	    start_selection();

	    // Execute the key in (insert) Select mode.
	    stuffcharReadbuff(Ctrl_O);
	    जर (mod_mask)
	    {
		char_u	    buf[4];

		buf[0] = K_SPECIAL;
		buf[1] = KS_MODIFIER;
		buf[2] = mod_mask;
		buf[3] = NUL;
		stuffReadbuffLen(buf, 3L);
	    }
	    stuffcharReadbuff(c);
	    परत TRUE;
    }
    परत FALSE;
}

/*
 * <Insert> key in Insert mode: toggle insert/replace mode.
 */
    स्थिर शून्य
ins_घाला(पूर्णांक replaceState)
{
#ifdef FEAT_EVAL
    set_vim_var_string(VV_INSERTMODE,
		   (char_u *)((State & REPLACE_FLAG) ? "i"
			    : replaceState == MODE_VREPLACE ? "v" : "r"), 1);
#endif
    ins_apply_autocmds(EVENT_INSERTCHANGE);
    जर (State & REPLACE_FLAG)
	State = MODE_घाला | (State & MODE_LANGMAP);
    अन्यथा
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
    जर (State & VREPLACE_FLAG)
	restart_edit = 'V';
    अन्यथा जर (State & REPLACE_FLAG)
	restart_edit = 'R';
    अन्यथा
	restart_edit = 'I';
    जर (virtual_active())
	ins_at_eol = FALSE;	// cursor always keeps its column
    अन्यथा
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
ins_shift(पूर्णांक c, पूर्णांक lastc)
{
    जर (stop_arrow() == FAIL)
	परत;
    AppendCharToRedobuff(c);

    /*
     * 0^D and ^^D: remove all indent.
     */
    जर (c == Ctrl_D && (lastc == '0' || lastc == '^')
						  && curwin->w_cursor.col > 0)
    {
	--curwin->w_cursor.col;
	(शून्य)del_char(FALSE);		// delete the '^' or '0'
	// In Replace mode, restore the characters that '^' or '0' replaced.
	जर (State & REPLACE_FLAG)
	    replace_काढा_ins();
	जर (lastc == '^')
	    old_indent = get_indent();	// remember curr. indent
	change_indent(INDENT_SET, 0, TRUE, 0, TRUE);
    }
    अन्यथा
	change_indent(c == Ctrl_D ? INDENT_DEC : INDENT_INC, 0, TRUE, 0, TRUE);

    जर (did_ai && *skipwhite(ml_get_curline()) != NUL)
	did_ai = FALSE;
    did_si = FALSE;
    can_si = FALSE;
    can_si_back = FALSE;
    can_cindent = FALSE;	// no cindenting after ^D or ^T
}

    स्थिर शून्य
ins_del(शून्य)
{
    पूर्णांक	    temp;

    जर (stop_arrow() == FAIL)
	परत;
    जर (gchar_cursor() == NUL)		// delete newline
    {
	temp = curwin->w_cursor.col;
	जर (!can_bs(BS_EOL)		// only if "eol" included
		|| do_join(2, FALSE, TRUE, FALSE, FALSE) == FAIL)
	    vim_beep(BO_BS);
	अन्यथा
	{
	    curwin->w_cursor.col = temp;
	    // Adjust orig_line_count in case more lines have been deleted than
	    // have been added. That makes sure, that open_line() later
	    // can access all buffer lines correctly
	    जर (State & VREPLACE_FLAG &&
		    orig_ओळ_मोजणी > curbuf->b_ml.ml_ओळ_मोजणी)
		orig_ओळ_मोजणी = curbuf->b_ml.ml_ओळ_मोजणी;
	}
    }
    अन्यथा जर (del_char(FALSE) == FAIL)  // delete char under cursor
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
    जर (State & REPLACE_FLAG)
    {
	// Don't delete characters before the insert point when in
	// Replace mode
	जर (curwin->w_cursor.lnum != Insstart.lnum
		|| curwin->w_cursor.col >= Insstart.col)
	    replace_do_bs(-1);
    }
    अन्यथा
	(शून्य)del_char(FALSE);
}

/*
 * Handle Backspace, delete-word and delete-line in Insert mode.
 * Return TRUE when backspace was actually used.
 */
    स्थिर पूर्णांक
ins_bs(
    पूर्णांक		c,
    पूर्णांक		mode,
    पूर्णांक		*inserted_space_p)
{
    linenr_T	lnum;
    पूर्णांक		cc;
    पूर्णांक		temp = 0;	    // init for GCC
    colnr_T	save_col;
    colnr_T	mincol;
    पूर्णांक		did_backspace = FALSE;
    पूर्णांक		in_indent;
    पूर्णांक		oldState;
    पूर्णांक		cpc[MAX_MCO];	    // composing characters
    पूर्णांक		call_fix_indent = FALSE;

    /*
     * can't delete anything in an empty file
     * can't backup past first character in buffer
     * can't backup past starting point unless 'backspace' > 1
     * can backup to a previous line if 'backspace' == 0
     */
    जर (       BUFEMPTY()
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
	परत FALSE;
    }

    जर (stop_arrow() == FAIL)
	परत FALSE;
    in_indent = inindent(0);
    जर (in_indent)
	can_cindent = FALSE;
    end_comment_pending = NUL;	// After BS, don't auto-end comment
#ifdef FEAT_RIGHTLEFT
    जर (revins_on)	    // put cursor after last inserted char
	inc_cursor();
#endif

    // Virtualedit:
    //	BACKSPACE_CHAR eats a virtual space
    //	BACKSPACE_WORD eats all coladd
    //	BACKSPACE_LINE eats all coladd and keeps going
    जर (curwin->w_cursor.coladd > 0)
    {
	जर (mode == BACKSPACE_CHAR)
	{
	    --curwin->w_cursor.coladd;
	    परत TRUE;
	}
	जर (mode == BACKSPACE_WORD)
	{
	    curwin->w_cursor.coladd = 0;
	    परत TRUE;
	}
	curwin->w_cursor.coladd = 0;
    }

    /*
     * Delete newline!
     */
    जर (curwin->w_cursor.col == 0)
    {
	lnum = Insstart.lnum;
	जर (curwin->w_cursor.lnum == lnum
#ifdef FEAT_RIGHTLEFT
			|| revins_on
#endif
				    )
	{
	    जर (u_save((linenr_T)(curwin->w_cursor.lnum - 2),
			       (linenr_T)(curwin->w_cursor.lnum + 1)) == FAIL)
		परत FALSE;
	    --Insstart.lnum;
	    Insstart.col = ml_get_len(Insstart.lnum);
	}
	/*
	 * In replace mode:
	 * cc < 0: NL was inserted, delete it
	 * cc >= 0: NL was replaced, put original characters back
	 */
	cc = -1;
	जर (State & REPLACE_FLAG)
	    cc = replace_काढा();	    // returns -1 if NL was inserted
	/*
	 * In replace mode, in the line we started replacing, we only move the
	 * cursor.
	 */
	जर ((State & REPLACE_FLAG) && curwin->w_cursor.lnum <= lnum)
	{
	    dec_cursor();
	}
	अन्यथा
	{
	    जर (!(State & VREPLACE_FLAG)
				   || curwin->w_cursor.lnum > orig_ओळ_मोजणी)
	    {
		temp = gchar_cursor();	// remember current char
		--curwin->w_cursor.lnum;

		// When "aw" is in 'formatoptions' we must delete the space at
		// the end of the line, otherwise the line will be broken
		// again when auto-formatting.
		जर (has_format_option(FO_AUTO)
					   && has_format_option(FO_WHITE_PAR))
		{
		    char_u  *ptr = ml_get_buf(curbuf, curwin->w_cursor.lnum,
									FALSE);
		    पूर्णांक	    len = ml_get_curline_len();

		    जर (len > 0 && ptr[len - 1] == ' ')
		    {
			char_u *newp = alloc(curbuf->b_ml.ml_ओळ_len - 1);

			जर (newp != रिक्त)
			{
			    mch_memmove(newp, ptr, len - 1);
			    newp[len - 1] = NUL;
			    जर (curbuf->b_ml.ml_ओळ_len > len + 1)
				mch_memmove(newp + len, ptr + len + 1,
					   curbuf->b_ml.ml_ओळ_len - len - 1);

			    जर (curbuf->b_ml.ml_flags
					      & (ML_ओळ_DIRTY | ML_ALLOCATED))
				vim_free(curbuf->b_ml.ml_ओळ_ptr);
			    curbuf->b_ml.ml_ओळ_ptr = newp;
			    curbuf->b_ml.ml_ओळ_len--;
			    curbuf->b_ml.ml_ओळ_textlen--;
			    curbuf->b_ml.ml_flags |= ML_ओळ_DIRTY;
			}
		    }
		}

		(शून्य)do_join(2, FALSE, FALSE, FALSE, FALSE);
		जर (temp == NUL && gchar_cursor() != NUL)
		    inc_cursor();
	    }
	    अन्यथा
		dec_cursor();

	    /*
	     * In MODE_REPLACE mode we have to put back the text that was
	     * replaced by the NL. On the replace stack is first a
	     * NUL-terminated sequence of characters that were deleted and then
	     * the characters that NL replaced.
	     */
	    जर (State & REPLACE_FLAG)
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
		पर्यंत (cc > 0)
		{
		    save_col = curwin->w_cursor.col;
		    mb_replace_काढा_ins(cc);
		    curwin->w_cursor.col = save_col;
		    cc = replace_काढा();
		}
		// restore the characters that NL replaced
		replace_काढा_ins();
		State = oldState;
	    }
	}
	did_ai = FALSE;
    }
    अन्यथा
    {
	/*
	 * Delete character(s) before the cursor.
	 */
#ifdef FEAT_RIGHTLEFT
	जर (revins_on)		// put cursor on last inserted char
	    dec_cursor();
#endif
	mincol = 0;
						// keep indent
	जर (mode == BACKSPACE_ओळ
		&& (curbuf->b_p_ai || cindent_on())
#ifdef FEAT_RIGHTLEFT
		&& !revins_on
#endif
			    )
	{
	    save_col = curwin->w_cursor.col;
	    beginline(BL_WHITE);
	    जर (curwin->w_cursor.col < save_col)
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
	जर (	   mode == BACKSPACE_CHAR
		&& ((p_sta && in_indent)
		    || ((get_sts_मूल्य() != 0
#ifdef FEAT_VARTABS
			|| tabstop_मोजणी(curbuf->b_p_vsts_array)
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
	    char_u	*ओळ;
	    char_u	*ptr;
	    char_u	*cursor_ptr;
	    char_u	*space_ptr;
	    colnr_T	space_vcol = 0;
	    पूर्णांक		prev_space = FALSE;
	    colnr_T	want_col;

	    *inserted_space_p = FALSE;

	    space_ptr = ptr = ओळ = ml_get_curline();
	    cursor_ptr = ओळ + curwin->w_cursor.col;

	    // Compute virtual column of cursor position, and find the last
	    // whitespace before cursor that is preceded by non-whitespace.
	    // Use chartabsize() so that virtual text and wrapping are ignored.
	    पर्यंत (ptr < cursor_ptr)
	    {
		पूर्णांक	cur_space = VIM_ISWHITE(*ptr);

		जर (!prev_space && cur_space)
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
	    जर (p_sta && in_indent)
		want_vcol -= want_vcol % (पूर्णांक)get_sw_मूल्य(curbuf);
	    अन्यथा
#ifdef FEAT_VARTABS
		want_vcol = tabstop_start(want_vcol, get_sts_मूल्य(),
						       curbuf->b_p_vsts_array);
#अन्यथा
		want_vcol -= want_vcol % (पूर्णांक)get_sts_मूल्य();
#endif

	    // Find the position to stop backspacing.
	    // Use chartabsize() so that virtual text and wrapping are ignored.
	    पर्यंत (TRUE)
	    {
		पूर्णांक size = chartabsize(space_ptr, space_vcol);

		जर (space_vcol + size > want_vcol)
		    थांबा;
		space_vcol += size;
		MB_PTR_ADV(space_ptr);
	    }
	    want_col = space_ptr - ओळ;

	    // Delete characters until we are at or before want_col.
	    पर्यंत (curwin->w_cursor.col > want_col)
		ins_bs_one();

	    // Insert extra spaces until we are at want_vcol.
	    साठी (; space_vcol < want_vcol; space_vcol++)
	    {
		// Remember the first char we inserted
		जर (curwin->w_cursor.lnum == Insstart_orig.lnum
				   && curwin->w_cursor.col < Insstart_orig.col)
		    Insstart_orig.col = curwin->w_cursor.col;

		जर (State & VREPLACE_FLAG)
		    ins_char(' ');
		अन्यथा
		{
		    ins_str((char_u *)" ", 1);
		    जर ((State & REPLACE_FLAG))
			replace_ढकला(NUL);
		}
	    }
	}

	/*
	 * Delete up to starting point, start of line or previous word.
	 */
	अन्यथा
	{
	    पूर्णांक cclass = 0, prev_cclass = 0;

	    जर (has_mbyte)
		cclass = mb_get_class(ml_get_cursor());
	    करा
	    {
#ifdef FEAT_RIGHTLEFT
		जर (!revins_on) // put cursor on char to be deleted
#endif
		    dec_cursor();

		cc = gchar_cursor();
		// look multi-byte character class
		जर (has_mbyte)
		{
		    prev_cclass = cclass;
		    cclass = mb_get_class(ml_get_cursor());
		}

		// start of word?
		जर (mode == BACKSPACE_WORD && !vim_isspace(cc))
		{
		    mode = BACKSPACE_WORD_NOT_SPACE;
		    temp = vim_iswordc(cc);
		}
		// end of word?
		अन्यथा जर (mode == BACKSPACE_WORD_NOT_SPACE
			&& ((vim_isspace(cc) || vim_iswordc(cc) != temp)
			|| prev_cclass != cclass))
		{
#ifdef FEAT_RIGHTLEFT
		    जर (!revins_on)
#endif
			inc_cursor();
#ifdef FEAT_RIGHTLEFT
		    अन्यथा जर (State & REPLACE_FLAG)
			dec_cursor();
#endif
		    थांबा;
		}
		जर (State & REPLACE_FLAG)
		    replace_do_bs(-1);
		अन्यथा
		{
		    जर (enc_utf8 && p_deco)
			(शून्य)utfc_ptr2char(ml_get_cursor(), cpc);
		    (शून्य)del_char(FALSE);
		    /*
		     * If there are combining characters and 'delcombine' is set
		     * move the cursor back.  Don't back up before the base
		     * character.
		     */
		    जर (enc_utf8 && p_deco && cpc[0] != NUL)
			inc_cursor();
#ifdef FEAT_RIGHTLEFT
		    जर (revins_chars)
		    {
			revins_chars--;
			revins_legal++;
		    }
		    जर (revins_on && gchar_cursor() == NUL)
			थांबा;
#endif
		}
		// Just a single backspace?:
		जर (mode == BACKSPACE_CHAR)
		    थांबा;
	    } पर्यंत (
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
    जर (curwin->w_cursor.col <= 1)
	did_ai = FALSE;

    जर (call_fix_indent)
	fix_indent();

    /*
     * It's a little strange to put backspaces into the redo
     * buffer, but it makes auto-indent a lot easier to deal
     * with.
     */
    AppendCharToRedobuff(c);

    // If deleted before the insertion point, adjust it
    जर (curwin->w_cursor.lnum == Insstart_orig.lnum
				  && curwin->w_cursor.col < Insstart_orig.col)
	Insstart_orig.col = curwin->w_cursor.col;

    // vi behaviour: the cursor moves backward but the character that
    //		     was there remains visible
    // Vim behaviour: the cursor moves backward and the character that
    //		      was there is erased from the screen.
    // We can emulate the vi behaviour by pretending there is a dollar
    // displayed even when there isn't.
    //  --pkv Sun Jan 19 01:56:40 EST 2003
    जर (vim_strchr(p_cpo, CPO_BACKSPACE) != रिक्त && dollar_vcol == -1)
	dollar_vcol = curwin->w_virtcol;

#ifdef FEAT_FOLDING
    // When deleting a char the cursor line must never be in a closed fold.
    // E.g., when 'foldmethod' is indent and deleting the first non-white
    // char before a Tab.
    जर (did_backspace)
	foldOpenCursor();
#endif

    परत did_backspace;
}

/*
 * Handle receiving P_PS: start paste mode.  Inserts the following text up to
 * P_PE literally.
 * When "drop" is TRUE then consume the text and drop it.
 */
    पूर्णांक
bracketed_paste(paste_mode_T mode, पूर्णांक drop, garray_T *gap)
{
    पूर्णांक		c;
    char_u	buf[NUMBUFLEN + MB_MAXBYTES];
    पूर्णांक		idx = 0;
    char_u	*end = find_termcode((char_u *)"PE");
    पूर्णांक		ret_char = -1;
    पूर्णांक		save_allow_किल्ल्या = allow_किल्ल्या;
    पूर्णांक		save_paste = p_paste;

    // If the end code is too long we can't detect it, read everything.
    जर (end != रिक्त && STRLEN(end) >= NUMBUFLEN)
	end = रिक्त;
    ++no_mapping;
    allow_किल्ल्या = 0;
    जर (!p_paste)
	// Also have the side effects of setting 'paste' to make it work much
	// faster.
	set_option_मूल्य_give_err((char_u *)"paste", TRUE, रिक्त, 0);

    साठी (;;)
    {
	// When the end is not defined read everything there is.
	जर (end == रिक्त && vpeekc() == NUL)
	    थांबा;
	करा
	    c = vgetc();
	पर्यंत (c == K_IGNORE || c == K_VER_SCROLLBAR || c == K_HOR_SCROLLBAR);

	जर (c == NUL || got_int || (ex_normal_busy > 0 && c == Ctrl_C))
	    // When CTRL-C was encountered the typeahead will be flushed and we
	    // won't get the end sequence.  Except when using ":normal".
	    थांबा;

	जर (has_mbyte)
	    idx += (*mb_char2bytes)(c, buf + idx);
	अन्यथा
	    buf[idx++] = c;
	buf[idx] = NUL;
	जर (end != रिक्त && STRNCMP(buf, end, idx) == 0)
	{
	    जर (end[idx] == NUL)
		थांबा; // Found the end of paste code.
	    सुरूठेवा;
	}
	जर (!drop)
	{
	    निवड (mode)
	    {
		स्थिती PASTE_CMDLINE:
		    put_on_cmdline(buf, idx, TRUE);
		    थांबा;

		स्थिती PASTE_EX:
		    // add one for the NUL that is going to be appended
		    जर (gap != रिक्त && ga_grow(gap, idx + 1) == OK)
		    {
			mch_memmove((अक्षर *)gap->ga_data + gap->ga_len,
							     buf, (माप_प्रकार)idx);
			gap->ga_len += idx;
		    }
		    थांबा;

		स्थिती PASTE_घाला:
		    जर (stop_arrow() == OK)
		    {
			c = buf[0];
			जर (idx == 1 && (c == CAR || c == K_KENTER || c == NL))
			    ins_eol(c);
			अन्यथा
			{
			    ins_char_bytes(buf, idx);
			    AppendToRedobuffLit(buf, idx);
			}
		    }
		    थांबा;

		स्थिती PASTE_ONE_CHAR:
		    जर (ret_char == -1)
		    {
			जर (has_mbyte)
			    ret_char = (*mb_ptr2char)(buf);
			अन्यथा
			    ret_char = buf[0];
		    }
		    थांबा;
	    }
	}
	idx = 0;
    }

    --no_mapping;
    allow_किल्ल्या = save_allow_किल्ल्या;
    जर (!save_paste)
	set_option_मूल्य_give_err((char_u *)"paste", FALSE, रिक्त, 0);

    परत ret_char;
}

#जर defined(FEAT_GUI_TABLINE)
    स्थिर शून्य
ins_tabline(पूर्णांक c)
{
    // We will be leaving the current window, unless closing another tab.
    जर (c != K_TABMENU || current_tabmenu != TABLINE_MENU_CLOSE
		|| (current_tab != 0 && current_tab != tabpage_index(curtab)))
    {
	undisplay_dollar();
	start_arrow(&curwin->w_cursor);
	can_cindent = TRUE;
    }

    जर (c == K_TABLINE)
	goto_tabpage(current_tab);
    अन्यथा
    {
	handle_tabmenu();
	redraw_statuslines();	// will redraw the tabline when needed
    }
}
#endif

#जर defined(FEAT_GUI)
    शून्य
ins_scroll(शून्य)
{
    pos_T	tpos;

    undisplay_dollar();
    tpos = curwin->w_cursor;
    जर (gui_do_scroll())
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
    जर (do_mousescroll_horiz(scrollbar_मूल्य))
    {
	start_arrow(&tpos);
	can_cindent = TRUE;
    }
}
#endif

    स्थिर शून्य
ins_डावा(शून्य)
{
    pos_T	tpos;
    पूर्णांक		end_change = dont_sync_undo == FALSE; // end undoable change

#ifdef FEAT_FOLDING
    जर ((fdo_flags & FDO_HOR) && किल्ली_Typed)
	foldOpenCursor();
#endif
    undisplay_dollar();
    tpos = curwin->w_cursor;
    जर (oneleft() == OK)
    {
#जर defined(FEAT_XIM) && defined(FEAT_GUI_GTK)
	// Only call start_arrow() when not busy with preediting, it will
	// break undo.  K_LEFT is inserted in im_correct_cursor().
	जर (p_imst == IM_OVER_THE_SPOT || !im_is_preediting())
#endif
	{
	    start_arrow_with_change(&tpos, end_change);
	    जर (!end_change)
		AppendCharToRedobuff(K_डावा);
	}
#ifdef FEAT_RIGHTLEFT
	// If exit reversed string, position is fixed
	जर (revins_scol != -1 && (पूर्णांक)curwin->w_cursor.col >= revins_scol)
	    revins_legal++;
	revins_chars++;
#endif
    }

    /*
     * if 'whichwrap' set for cursor in insert mode may go to
     * previous line
     */
    अन्यथा जर (vim_strchr(p_ww, '[') != रिक्त && curwin->w_cursor.lnum > 1)
    {
	// always break undo when moving upwards/downwards, else undo may break
	start_arrow(&tpos);
	--(curwin->w_cursor.lnum);
	coladvance((colnr_T)MAXCOL);
	curwin->w_set_curswant = true;	// so we stay at the end
    }
    अन्यथा
	vim_beep(BO_CRSR);
    dont_sync_undo = FALSE;
}

    स्थिर शून्य
ins_home(पूर्णांक c)
{
    pos_T	tpos;

#ifdef FEAT_FOLDING
    जर ((fdo_flags & FDO_HOR) && किल्ली_Typed)
	foldOpenCursor();
#endif
    undisplay_dollar();
    tpos = curwin->w_cursor;
    जर (c == K_C_HOME)
	curwin->w_cursor.lnum = 1;
    curwin->w_cursor.col = 0;
    curwin->w_cursor.coladd = 0;
    curwin->w_curswant = 0;
    start_arrow(&tpos);
}

    स्थिर शून्य
ins_end(पूर्णांक c)
{
    pos_T	tpos;

#ifdef FEAT_FOLDING
    जर ((fdo_flags & FDO_HOR) && किल्ली_Typed)
	foldOpenCursor();
#endif
    undisplay_dollar();
    tpos = curwin->w_cursor;
    जर (c == K_C_END)
	curwin->w_cursor.lnum = curbuf->b_ml.ml_ओळ_मोजणी;
    coladvance((colnr_T)MAXCOL);
    curwin->w_curswant = MAXCOL;

    start_arrow(&tpos);
}

    स्थिर शून्य
ins_s_डावा(शून्य)
{
    पूर्णांक end_change = dont_sync_undo == FALSE; // end undoable change
#ifdef FEAT_FOLDING
    जर ((fdo_flags & FDO_HOR) && किल्ली_Typed)
	foldOpenCursor();
#endif
    undisplay_dollar();
    जर (curwin->w_cursor.lnum > 1 || curwin->w_cursor.col > 0)
    {
	start_arrow_with_change(&curwin->w_cursor, end_change);
	जर (!end_change)
	    AppendCharToRedobuff(K_S_डावा);
	(शून्य)bck_word(1L, FALSE, FALSE);
	curwin->w_set_curswant = true;
    }
    अन्यथा
	vim_beep(BO_CRSR);
    dont_sync_undo = FALSE;
}

    स्थिर शून्य
ins_उजवा(शून्य)
{
    पूर्णांक end_change = dont_sync_undo == FALSE; // end undoable change

#ifdef FEAT_FOLDING
    जर ((fdo_flags & FDO_HOR) && किल्ली_Typed)
	foldOpenCursor();
#endif
    undisplay_dollar();
    जर (gchar_cursor() != NUL || virtual_active())
    {
	start_arrow_with_change(&curwin->w_cursor, end_change);
	जर (!end_change)
	    AppendCharToRedobuff(K_उजवा);
	curwin->w_set_curswant = true;
	जर (virtual_active())
	    oneright();
	अन्यथा
	{
	    जर (has_mbyte)
		curwin->w_cursor.col += (*mb_ptr2len)(ml_get_cursor());
	    अन्यथा
		++curwin->w_cursor.col;
	}

#ifdef FEAT_RIGHTLEFT
	revins_legal++;
	जर (revins_chars)
	    revins_chars--;
#endif
    }
    // if 'whichwrap' set for cursor in insert mode, may move the
    // cursor to the next line
    अन्यथा जर (vim_strchr(p_ww, ']') != रिक्त
	    && curwin->w_cursor.lnum < curbuf->b_ml.ml_ओळ_मोजणी)
    {
	start_arrow(&curwin->w_cursor);
	curwin->w_set_curswant = true;
	++curwin->w_cursor.lnum;
	curwin->w_cursor.col = 0;
    }
    अन्यथा
	vim_beep(BO_CRSR);
    dont_sync_undo = FALSE;
}

    स्थिर शून्य
ins_s_उजवा(शून्य)
{
    पूर्णांक end_change = dont_sync_undo == FALSE; // end undoable change
#ifdef FEAT_FOLDING
    जर ((fdo_flags & FDO_HOR) && किल्ली_Typed)
	foldOpenCursor();
#endif
    undisplay_dollar();
    जर (curwin->w_cursor.lnum < curbuf->b_ml.ml_ओळ_मोजणी
	    || gchar_cursor() != NUL)
    {
	start_arrow_with_change(&curwin->w_cursor, end_change);
	जर (!end_change)
	    AppendCharToRedobuff(K_S_उजवा);
	(शून्य)fwd_word(1L, FALSE, 0);
	curwin->w_set_curswant = true;
    }
    अन्यथा
	vim_beep(BO_CRSR);
    dont_sync_undo = FALSE;
}

    स्थिर शून्य
ins_up(
    पूर्णांक		startcol)	// when TRUE move to Insstart.col
{
    pos_T	tpos;
    linenr_T	old_topline = curwin->w_topline;
#ifdef FEAT_DIFF
    पूर्णांक		old_topfill = curwin->w_topfill;
#endif

    undisplay_dollar();
    tpos = curwin->w_cursor;
    जर (cursor_up(1L, TRUE) == OK)
    {
	जर (startcol)
	    coladvance(getvcol_nolist(&Insstart));
	जर (old_topline != curwin->w_topline
#ifdef FEAT_DIFF
		|| old_topfill != curwin->w_topfill
#endif
		)
	    redraw_later(UPD_VALID);
	start_arrow(&tpos);
	can_cindent = TRUE;
    }
    अन्यथा
	vim_beep(BO_CRSR);
}

    स्थिर शून्य
ins_pageup(शून्य)
{
    pos_T	tpos;

    undisplay_dollar();

    जर (mod_mask & MOD_MASK_CTRL)
    {
	// <C-PageUp>: tab page back
	जर (first_tabpage->tp_next != रिक्त)
	{
	    start_arrow(&curwin->w_cursor);
	    goto_tabpage(-1);
	}
	परत;
    }

    tpos = curwin->w_cursor;
    जर (pagescroll(BACKWARD, 1L, FALSE) == OK)
    {
	start_arrow(&tpos);
	can_cindent = TRUE;
    }
    अन्यथा
	vim_beep(BO_CRSR);
}

    स्थिर शून्य
ins_down(
    पूर्णांक		startcol)	// when TRUE move to Insstart.col
{
    pos_T	tpos;
    linenr_T	old_topline = curwin->w_topline;
#ifdef FEAT_DIFF
    पूर्णांक		old_topfill = curwin->w_topfill;
#endif

    undisplay_dollar();
    tpos = curwin->w_cursor;
    जर (cursor_down(1L, TRUE) == OK)
    {
	जर (startcol)
	    coladvance(getvcol_nolist(&Insstart));
	जर (old_topline != curwin->w_topline
#ifdef FEAT_DIFF
		|| old_topfill != curwin->w_topfill
#endif
		)
	    redraw_later(UPD_VALID);
	start_arrow(&tpos);
	can_cindent = TRUE;
    }
    अन्यथा
	vim_beep(BO_CRSR);
}

    स्थिर शून्य
ins_pagedown(शून्य)
{
    pos_T	tpos;

    undisplay_dollar();

    जर (mod_mask & MOD_MASK_CTRL)
    {
	// <C-PageDown>: tab page forward
	जर (first_tabpage->tp_next != रिक्त)
	{
	    start_arrow(&curwin->w_cursor);
	    goto_tabpage(0);
	}
	परत;
    }

    tpos = curwin->w_cursor;
    जर (pagescroll(FORWARD, 1L, FALSE) == OK)
    {
	start_arrow(&tpos);
	can_cindent = TRUE;
    }
    अन्यथा
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
    स्थिर पूर्णांक
ins_tab(शून्य)
{
    पूर्णांक		ind;
    पूर्णांक		i;
    पूर्णांक		temp;

    जर (Insstart_blank_vcol == MAXCOL && curwin->w_cursor.lnum == Insstart.lnum)
	Insstart_blank_vcol = get_nolist_virtcol();
    जर (echeck_abbr(TAB + ABBR_OFF))
	परत FALSE;

    ind = inindent(0);
    जर (ind)
	can_cindent = FALSE;

    /*
     * When nothing special, insert TAB like a normal character.
     */
    जर (!curbuf->b_p_et
#ifdef FEAT_VARTABS
	    && !(p_sta && ind
		// These five lines mean 'tabstop' != 'shiftwidth'
		&& ((tabstop_मोजणी(curbuf->b_p_vts_array) > 1)
		    || (tabstop_मोजणी(curbuf->b_p_vts_array) == 1
			&& tabstop_first(curbuf->b_p_vts_array)
						       != get_sw_मूल्य(curbuf))
		    || (tabstop_मोजणी(curbuf->b_p_vts_array) == 0
			&& curbuf->b_p_ts != get_sw_मूल्य(curbuf))))
	    && tabstop_मोजणी(curbuf->b_p_vsts_array) == 0
#अन्यथा
	    && !(p_sta && ind && curbuf->b_p_ts != get_sw_मूल्य(curbuf))
#endif
	    && get_sts_मूल्य() == 0)
	परत TRUE;

    जर (stop_arrow() == FAIL)
	परत TRUE;

    did_ai = FALSE;
    did_si = FALSE;
    can_si = FALSE;
    can_si_back = FALSE;
    AppendToRedobuff((char_u *)"\t");

#ifdef FEAT_VARTABS
    जर (p_sta && ind)		// insert tab in indent, use 'shiftwidth'
    {
	temp = (पूर्णांक)get_sw_मूल्य(curbuf);
	temp -= get_nolist_virtcol() % temp;
    }
    अन्यथा जर (tabstop_मोजणी(curbuf->b_p_vsts_array) > 0 || curbuf->b_p_sts != 0)
				// use 'softtabstop' when set
	temp = tabstop_padding(get_nolist_virtcol(), get_sts_मूल्य(),
						     curbuf->b_p_vsts_array);
    अन्यथा			// otherwise use 'tabstop'
	temp = tabstop_padding(get_nolist_virtcol(), curbuf->b_p_ts,
						     curbuf->b_p_vts_array);
#अन्यथा
    जर (p_sta && ind)		// insert tab in indent, use 'shiftwidth'
	temp = (पूर्णांक)get_sw_मूल्य(curbuf);
    अन्यथा जर (curbuf->b_p_sts != 0) // use 'softtabstop' when set
	temp = (पूर्णांक)get_sts_मूल्य();
    अन्यथा			// otherwise use 'tabstop'
	temp = (पूर्णांक)curbuf->b_p_ts;
    temp -= get_nolist_virtcol() % temp;
#endif

    /*
     * Insert the first space with ins_char().	It will delete one char in
     * replace mode.  Insert the rest with ins_str(); it will not delete any
     * chars.  For MODE_VREPLACE state, we use ins_char() for all characters.
     */
    ins_char(' ');
    पर्यंत (--temp > 0)
    {
	जर (State & VREPLACE_FLAG)
	    ins_char(' ');
	अन्यथा
	{
	    ins_str((char_u *)" ", 1);
	    जर (State & REPLACE_FLAG)	    // no char replaced
		replace_ढकला(NUL);
	}
    }

    /*
     * When 'expandtab' not set: Replace spaces by TABs where possible.
     */
#ifdef FEAT_VARTABS
    जर (!curbuf->b_p_et && (tabstop_मोजणी(curbuf->b_p_vsts_array) > 0
			    || get_sts_मूल्य() > 0
			    || (p_sta && ind)))
#अन्यथा
    जर (!curbuf->b_p_et && (get_sts_मूल्य() || (p_sta && ind)))
#endif
    {
	char_u		*ptr;
	char_u		*saved_ओळ = रिक्त;	// init for GCC
	pos_T		pos;
	pos_T		fpos;
	pos_T		*cursor;
	colnr_T		want_vcol, vcol;
	पूर्णांक		change_col = -1;
	पूर्णांक		save_list = curwin->w_p_list;
	char_u		*tab = (char_u *)"\t";
	chartabsize_T	cts;

	/*
	 * Get the current line.  For MODE_VREPLACE state, don't make real
	 * changes yet, just work on a copy of the line.
	 */
	जर (State & VREPLACE_FLAG)
	{
	    pos = curwin->w_cursor;
	    cursor = &pos;
	    saved_ओळ = vim_strnsave(ml_get_curline(), ml_get_curline_len());
	    जर (saved_ओळ == रिक्त)
		परत FALSE;
	    ptr = saved_ओळ + pos.col;
	}
	अन्यथा
	{
	    ptr = ml_get_cursor();
	    cursor = &curwin->w_cursor;
	}

	// When 'L' is not in 'cpoptions' a tab always takes up 'ts' spaces.
	जर (vim_strchr(p_cpo, CPO_LISTWM) == रिक्त)
	    curwin->w_p_list = FALSE;

	// Find first white before the cursor
	fpos = curwin->w_cursor;
	पर्यंत (fpos.col > 0 && VIM_ISWHITE(ptr[-1]))
	{
	    --fpos.col;
	    --ptr;
	}

	// In Replace mode, don't change characters before the insert point.
	जर ((State & REPLACE_FLAG)
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
	पर्यंत (VIM_ISWHITE(*ptr))
	{
	    i = lbr_chartabsize(&cts);
	    जर (cts.cts_vcol + i > want_vcol)
		थांबा;
	    जर (*ptr != TAB)
	    {
		*ptr = TAB;
		जर (change_col < 0)
		{
		    change_col = fpos.col;  // Column of first change
		    // May have to adjust Insstart
		    जर (fpos.lnum == Insstart.lnum && fpos.col < Insstart.col)
			Insstart.col = fpos.col;
		}
	    }
	    ++fpos.col;
	    ++ptr;
	    cts.cts_vcol += i;
	}
	vcol = cts.cts_vcol;
	clear_chartabsize_arg(&cts);

	जर (change_col >= 0)
	{
	    पूर्णांक		    repl_off = 0;

	    // Skip over the spaces we need.
	    init_chartabsize_arg(&cts, curwin, 0, vcol, ptr, ptr);
	    पर्यंत (cts.cts_vcol < want_vcol && *cts.cts_ptr == ' ')
	    {
		cts.cts_vcol += lbr_chartabsize(&cts);
		++cts.cts_ptr;
		++repl_off;
	    }
	    ptr = cts.cts_ptr;
	    vcol = cts.cts_vcol;
	    clear_chartabsize_arg(&cts);

	    जर (vcol > want_vcol)
	    {
		// Must have a char with 'showbreak' just before it.
		--ptr;
		--repl_off;
	    }
	    fpos.col += repl_off;

	    // Delete following spaces.
	    i = cursor->col - fpos.col;
	    जर (i > 0)
	    {
#ifdef FEAT_PROP_POPUP
		जर (!(State & VREPLACE_FLAG))
		{
		    char_u  *newp;
		    पूर्णांक	    col;

		    newp = alloc(curbuf->b_ml.ml_ओळ_len - i);
		    जर (newp == रिक्त)
			परत FALSE;

		    col = ptr - curbuf->b_ml.ml_ओळ_ptr;
		    जर (col > 0)
			mch_memmove(newp, ptr - col, col);
		    mch_memmove(newp + col, ptr + i,
					   curbuf->b_ml.ml_ओळ_len - col - i);

		    जर (curbuf->b_ml.ml_flags & (ML_ओळ_DIRTY | ML_ALLOCATED))
			vim_free(curbuf->b_ml.ml_ओळ_ptr);
		    curbuf->b_ml.ml_ओळ_ptr = newp;
		    curbuf->b_ml.ml_ओळ_len -= i;
		    curbuf->b_ml.ml_ओळ_textlen = 0;
		    curbuf->b_ml.ml_flags =
			   (curbuf->b_ml.ml_flags | ML_ओळ_DIRTY) & ~ML_EMPTY;
		}
		अन्यथा
#endif
		    STRMOVE(ptr, ptr + i);
		// correct replace stack.
		जर ((State & REPLACE_FLAG) && !(State & VREPLACE_FLAG))
		    साठी (temp = i; --temp >= 0; )
			replace_join(repl_off);
	    }
#ifdef FEAT_NETBEANS_INTG
	    जर (netbeans_active())
	    {
		netbeans_removed(curbuf, fpos.lnum, cursor->col, (दीर्घ)(i + 1));
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
	    जर (State & VREPLACE_FLAG)
	    {
		// Backspace from real cursor to change_col
		backspace_until_column(change_col);

		// Insert each char in saved_line from changed_col to
		// ptr-cursor
		ins_bytes_len(saved_ओळ + change_col,
						    cursor->col - change_col);
	    }
	}

	जर (State & VREPLACE_FLAG)
	    vim_free(saved_ओळ);
	curwin->w_p_list = save_list;
    }

    परत FALSE;
}

/*
 * Handle CR or NL in insert mode.
 * Return FAIL when out of memory or can't undo.
 */
    पूर्णांक
ins_eol(पूर्णांक c)
{
    पूर्णांक	    i;

    जर (echeck_abbr(c + ABBR_OFF))
	परत OK;
    जर (stop_arrow() == FAIL)
	परत FAIL;
    undisplay_dollar();

    /*
     * Strange Vi behaviour: In Replace mode, typing a NL will not delete the
     * character under the cursor.  Only push a NUL on the replace stack,
     * nothing to put back when the NL is deleted.
     */
    जर ((State & REPLACE_FLAG) && !(State & VREPLACE_FLAG))
	replace_ढकला(NUL);

    /*
     * In MODE_VREPLACE state, a NL replaces the rest of the line, and starts
     * replacing the next line, so we push all of the characters left on the
     * line onto the replace stack.  This is not done here though, it is done
     * in open_line().
     */

    // Put cursor on NUL if on the last char and coladd is 1 (happens after
    // CTRL-O).
    जर (virtual_active() && curwin->w_cursor.coladd > 0)
	coladvance(getviscol());

#ifdef FEAT_RIGHTLEFT
    // NL in reverse insert will always start in the end of
    // current line.
    जर (revins_on)
	curwin->w_cursor.col += ml_get_cursor_len();
#endif

    AppendToRedobuff(NL_STR);
    i = open_ओळ(FORWARD,
	    has_format_option(FO_RET_COMS) ? OPENLINE_DO_COM : 0, old_indent,
	    रिक्त);
    old_indent = 0;
    can_cindent = TRUE;
#ifdef FEAT_FOLDING
    // When inserting a line the cursor line must never be in a closed fold.
    foldOpenCursor();
#endif

    परत i;
}

#ifdef FEAT_DIGRAPHS
/*
 * Handle digraph in insert mode.
 * Returns character still to be inserted, or NUL when nothing remaining to be
 * done.
 */
    स्थिर पूर्णांक
ins_digraph(शून्य)
{
    पूर्णांक	    c;
    पूर्णांक	    cc;
    पूर्णांक	    did_putchar = FALSE;

    pc_status = PC_STATUS_UNSET;
    जर (redrawing() && !char_avail())
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
    ++allow_किल्ल्या;
    c = plain_vgetc();
    --no_mapping;
    --allow_किल्ल्या;
    जर (did_putchar)
	// when the line fits in 'columns' the '?' is at the start of the next
	// line and will not be removed by the redraw
	edit_unputchar();

    जर (IS_SPECIAL(c) || mod_mask)	    // special key
    {
	clear_showcmd();
	घाला_special(c, TRUE, FALSE);
	परत NUL;
    }
    जर (c != ESC)
    {
	did_putchar = FALSE;
	जर (redrawing() && !char_avail())
	{
	    // may need to redraw when no more chars available now
	    ins_redraw(FALSE);

	    जर (char2cells(c) == 1)
	    {
		ins_redraw(FALSE);
		edit_putchar(c, TRUE);
		did_putchar = TRUE;
	    }
	    add_to_showcmd_c(c);
	}
	++no_mapping;
	++allow_किल्ल्या;
	cc = plain_vgetc();
	--no_mapping;
	--allow_किल्ल्या;
	जर (did_putchar)
	    // when the line fits in 'columns' the '?' is at the start of the
	    // next line and will not be removed by a redraw
	    edit_unputchar();
	जर (cc != ESC)
	{
	    AppendToRedobuff((char_u *)CTRL_V_STR);
	    c = digraph_get(c, cc, TRUE);
	    clear_showcmd();
	    परत c;
	}
    }
    clear_showcmd();
    परत NUL;
}
#endif

/*
 * Handle CTRL-E and CTRL-Y in Insert mode: copy char from other line.
 * Returns the char to be inserted, or NUL if none found.
 */
    पूर्णांक
ins_copychar(linenr_T lnum)
{
    पूर्णांक		    c;
    char_u	    *ptr, *prev_ptr;
    char_u	    *ओळ;
    chartabsize_T   cts;

    जर (lnum < 1 || lnum > curbuf->b_ml.ml_ओळ_मोजणी)
    {
	vim_beep(BO_COPY);
	परत NUL;
    }

    // try to advance to the cursor column
    validate_virtcol();
    ओळ = ml_get(lnum);
    prev_ptr = ओळ;
    init_chartabsize_arg(&cts, curwin, lnum, 0, ओळ, ओळ);
    पर्यंत (cts.cts_vcol < curwin->w_virtcol && *cts.cts_ptr != NUL)
    {
	prev_ptr = cts.cts_ptr;
	cts.cts_vcol += lbr_chartabsize_adv(&cts);
    }
    जर (cts.cts_vcol > curwin->w_virtcol)
	ptr = prev_ptr;
    अन्यथा
	ptr = cts.cts_ptr;
    clear_chartabsize_arg(&cts);

    c = (*mb_ptr2char)(ptr);
    जर (c == NUL)
	vim_beep(BO_COPY);
    परत c;
}

/*
 * CTRL-Y or CTRL-E typed in Insert mode.
 */
    स्थिर पूर्णांक
ins_ctrl_ey(पूर्णांक tc)
{
    पूर्णांक	    c = tc;

    जर (ctrl_x_mode_scroll())
    {
	जर (c == Ctrl_Y)
	    scrolldown_clamp();
	अन्यथा
	    scrollup_clamp();
	redraw_later(UPD_VALID);
    }
    अन्यथा
    {
	c = ins_copychar(curwin->w_cursor.lnum + (c == Ctrl_Y ? -1 : 1));
	जर (c != NUL)
	{
	    दीर्घ	tw_save;

	    // The character must be taken literally, insert like it
	    // was typed after a CTRL-V, and pretend 'textwidth'
	    // wasn't set.  Digits, 'o' and 'x' are special after a
	    // CTRL-V, don't use it for these.
	    जर (c < 256 && !SAFE_isalnum(c))
		AppendToRedobuff((char_u *)CTRL_V_STR);	// CTRL-V
	    tw_save = curbuf->b_p_tw;
	    curbuf->b_p_tw = -1;
	    घाला_special(c, TRUE, FALSE);
	    curbuf->b_p_tw = tw_save;
#ifdef FEAT_RIGHTLEFT
	    revins_chars++;
	    revins_legal++;
#endif
	    c = Ctrl_V;	// pretend CTRL-V is last character
	    auto_format(FALSE, TRUE);
	}
    }
    परत c;
}

/*
 * Get the value that w_virtcol would have when 'list' is off.
 * Unless 'cpo' contains the 'L' flag.
 */
    colnr_T
get_nolist_virtcol(शून्य)
{
    // check validity of cursor in current buffer
    जर (curwin->w_buffer == रिक्त
	|| curwin->w_buffer->b_ml.ml_mfp == रिक्त
	|| curwin->w_cursor.lnum > curwin->w_buffer->b_ml.ml_ओळ_मोजणी)
	परत 0;
    जर (curwin->w_p_list && vim_strchr(p_cpo, CPO_LISTWM) == रिक्त)
	परत getvcol_nolist(&curwin->w_cursor);
    validate_virtcol();
    परत curwin->w_virtcol;
}

#जर defined(FEAT_EVAL)
/*
 * Handle the InsertCharPre autocommand.
 * "c" is the character that was typed.
 * Return a pointer to allocated memory with the replacement string.
 * Return NULL to continue inserting "c".
 */
    स्थिर char_u *
do_घाला_char_pre(पूर्णांक c)
{
    char_u	*res;
    char_u	buf[MB_MAXBYTES + 1];
    माप_प्रकार	buflen;
    पूर्णांक		save_State = State;

    // Return quickly when there is nothing to do.
    जर (!has_insertcharpre())
	परत रिक्त;

    जर (c == Ctrl_RSB)
	परत रिक्त;

    जर (has_mbyte)
	buflen = (*mb_char2bytes)(c, buf);
    अन्यथा
    {
	buf[0] = c;
	buflen = 1;
    }
    buf[buflen] = NUL;

    // Lock the text to avoid weird things from happening.
    ++textlock;
    set_vim_var_string(VV_CHAR, buf, (पूर्णांक)buflen);  // set v:char

    res = रिक्त;
    जर (ins_apply_autocmds(EVENT_INSERTCHARPRE))
    {
	// Get the value of v:char.  It may be empty or more than one
	// character.  Only use it when changed, otherwise continue with the
	// original character to avoid breaking autoindent.
	जर (STRCMP(buf, get_vim_var_str(VV_CHAR)) != 0)
	    res = vim_strsave(get_vim_var_str(VV_CHAR));
    }

    set_vim_var_string(VV_CHAR, रिक्त, -1);  // clear v:char
    --textlock;

    // Restore the State, it may have been changed.
    State = save_State;

    परत res;
}
#endif

    पूर्णांक
get_can_cindent(शून्य)
{
    परत can_cindent;
}

    शून्य
set_can_cindent(पूर्णांक val)
{
    can_cindent = val;
}

/*
 * Trigger "event" and take care of fixing undo.
 */
    पूर्णांक
ins_apply_autocmds(event_T event)
{
    varnumber_T	tick = CHANGEDTICK(curbuf);
    पूर्णांक r;

    r = apply_autocmds(event, रिक्त, रिक्त, FALSE, curbuf);

    // If u_savesub() was called then we are not prepared to start
    // a new line.  Call u_save() with no contents to fix that.
    // Except when leaving Insert mode.
    जर (event != EVENT_INSERTLEAVE && tick != CHANGEDTICK(curbuf))
	u_save(curwin->w_cursor.lnum, (linenr_T)(curwin->w_cursor.lnum + 1));

    परत r;
}
