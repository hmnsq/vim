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
#define BACKSPACE_पंक्ति		    4

// Set when doing something for completion that may call edit() recursively,
// which is not allowed.
स्थिर पूर्णाङ्क	compl_busy = FALSE;


स्थिर शून्य ins_ctrl_v(शून्य);
स्थिर शून्य जोड़ो_special(पूर्णाङ्क, पूर्णाङ्क, पूर्णाङ्क);
स्थिर शून्य redo_literal(पूर्णाङ्क c);
स्थिर शून्य start_arrow_common(pos_T *end_जोड़ो_pos, पूर्णाङ्क change);
#ifdef FEAT_SPELL
स्थिर शून्य check_spell_redraw(शून्य);
#endif
स्थिर शून्य stop_जोड़ो(pos_T *end_जोड़ो_pos, पूर्णाङ्क esc, पूर्णाङ्क nomove);
स्थिर पूर्णाङ्क  echeck_abbr(पूर्णाङ्क);
स्थिर शून्य mb_replace_निकालो_ins(पूर्णाङ्क cc);
स्थिर शून्य replace_flush(शून्य);
स्थिर शून्य replace_do_bs(पूर्णाङ्क limit_col);
स्थिर पूर्णाङ्क del_char_after_col(पूर्णाङ्क limit_col);
स्थिर शून्य ins_reg(शून्य);
स्थिर शून्य ins_ctrl_g(शून्य);
स्थिर शून्य ins_ctrl_hat(शून्य);
स्थिर पूर्णाङ्क  ins_esc(long *गिनती, पूर्णाङ्क cmdchar, पूर्णाङ्क nomove);
#ifdef FEAT_RIGHTLEFT
स्थिर शून्य ins_ctrl_(शून्य);
#endif
स्थिर पूर्णाङ्क ins_start_select(पूर्णाङ्क c);
स्थिर शून्य ins_जोड़ो(पूर्णाङ्क replaceState);
स्थिर शून्य ins_ctrl_o(शून्य);
स्थिर शून्य ins_shift(पूर्णाङ्क c, पूर्णाङ्क lastc);
स्थिर शून्य ins_del(शून्य);
स्थिर पूर्णाङ्क  ins_bs(पूर्णाङ्क c, पूर्णाङ्क mode, पूर्णाङ्क *inserted_space_p);
#यदि defined(FEAT_GUI_TABLINE)
स्थिर शून्य ins_tabline(पूर्णाङ्क c);
#endif
स्थिर शून्य ins_बायाँ(शून्य);
स्थिर शून्य ins_home(पूर्णाङ्क c);
स्थिर शून्य ins_end(पूर्णाङ्क c);
स्थिर शून्य ins_s_बायाँ(शून्य);
स्थिर शून्य ins_दायाँ(शून्य);
स्थिर शून्य ins_s_दायाँ(शून्य);
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
#यदि defined(FEAT_EVAL)
स्थिर char_u *do_जोड़ो_char_pre(पूर्णाङ्क c);
#endif

स्थिर colnr_T	Insstart_textlen;	// length of line when insert started
स्थिर colnr_T	Insstart_blank_vcol;	// vcol for first inserted blank
स्थिर पूर्णाङ्क	update_Insstart_orig = TRUE; // set Insstart_orig to Insstart

स्थिर string_T	last_जोड़ो = {रिक्त, 0};    // the text of the previous insert,
					    // K_SPECIAL and CSI are escaped
स्थिर पूर्णाङ्क	last_जोड़ो_skip; // nr of chars in front of previous insert
स्थिर पूर्णाङ्क	new_जोड़ो_skip;  // nr of chars in front of current insert
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
    कुरु {					\
	update_screen(UPD_VALID);  /* Show char (deletion) immediately */ \
	out_flush();				\
	ins_compl_enable_autocomplete();	\
	ins_compl_arm_autostart();		\
	यदि (!ins_compl_arm_autocomplete_delay())\
	    गच्छ docomplete;			\
    } यावत् (0)

#define MAY_TRIGGER_AUTOCOMPLETE(c)				\
    कुरु {							\
	यदि (ins_compl_has_autocomplete() && !char_avail()	\
		&& curwin->w_cursor.col > 0)			\
	{							\
	    (c) = char_before_cursor();				\
	    यदि (vim_isprintc(c))				\
		TRIGGER_AUTOCOMPLETE();				\
	}							\
    } यावत् (0)

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
    long	गिनती)
{
    पूर्णाङ्क		c = 0;
    char_u	*ptr;
    पूर्णाङ्क		lastc = 0;
    पूर्णाङ्क		mincol;
    स्थिर linenr_T o_lnum = 0;
    पूर्णाङ्क		i;
    पूर्णाङ्क		did_backspace = TRUE;	    // previous char was backspace
    पूर्णाङ्क		पंक्ति_is_white = FALSE;	    // line is empty before insert
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
    पूर्णाङ्क		cursor_पंक्ति_was_concealed;
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
    यदि (sandbox != 0)
    {
	emsg(_(e_not_allowed_in_sandbox));
	प्रत्यागम FALSE;
    }
#endif
    // Don't allow changes in the buffer while editing the cmdline.  The
    // caller of getcmdline() may get confused.
    // Don't allow recursive insert mode when busy with completion.
    यदि (textlock != 0 || ins_compl_active() || compl_busy || pum_visible())
    {
	emsg(_(e_not_allowed_to_change_text_or_change_window));
	प्रत्यागम FALSE;
    }
    ins_compl_clear();	    // clear stuff for CTRL-X mode

    /*
     * Trigger InsertEnter autocommands.  Do not do this for "r<CR>" or "grx".
     */
    यदि (cmdchar != 'r' && cmdchar != 'v')
    {
	pos_T   save_cursor = curwin->w_cursor;

#ifdef FEAT_EVAL
	यदि (cmdchar == 'R')
	    ptr = (char_u *)"r";
	अन्यथा यदि (cmdchar == 'V')
	    ptr = (char_u *)"v";
	अन्यथा
	    ptr = (char_u *)"i";
	set_vim_var_string(VV_INSERTMODE, ptr, 1);
	set_vim_var_string(VV_CHAR, रिक्त, -1);  // clear v:char
#endif
	ins_apply_autocmds(EVENT_INSERTENTER);

	// Check for changed highlighting, e.g. for ModeMsg.
	यदि (need_highlight_changed)
	    highlight_changed();

	// Make sure the cursor didn't move.  Do call check_cursor_col() in
	// case the text was modified.  Since Insert mode was not started yet
	// a call to check_cursor_col() may move the cursor, especially with
	// the "A" command, thus set State to avoid that. Also check that the
	// line number is still valid (lines may have been deleted).
	// Do not restore if v:char was set to a non-empty string.
	यदि (!EQUAL_POS(curwin->w_cursor, save_cursor)
#ifdef FEAT_EVAL
		&& *get_vim_var_str(VV_CHAR) == NUL
#endif
		&& save_cursor.lnum <= curbuf->b_ml.ml_पंक्ति_गिनती)
	{
	    पूर्णाङ्क save_state = State;

	    curwin->w_cursor = save_cursor;
	    State = MODE_जोड़ो;
	    check_cursor_col();
	    State = save_state;
	}
    }

#ifdef FEAT_CONCEAL
    // Check if the cursor line was concealed before changing State.
    cursor_पंक्ति_was_concealed = curwin->w_p_cole > 0
						&& conceal_cursor_पंक्ति(curwin);
#endif

    /*
     * When doing a paste with the middle mouse button, Insstart is set to
     * where the paste started.
     */
    यदि (where_paste_started.lnum != 0)
	Insstart = where_paste_started;
    अन्यथा
    {
	Insstart = curwin->w_cursor;
	यदि (startln)
	    Insstart.col = 0;
    }
    Insstart_textlen = (colnr_T)linetabsize_str(ml_get_curline());
    Insstart_blank_vcol = MAXCOL;
    यदि (!did_ai)
	ai_col = 0;

    यदि (cmdchar != NUL && restart_edit == 0)
    {
	ResetRedobuff();
	Append_संख्या_To_Redobuff(गिनती);
	यदि (cmdchar == 'V' || cmdchar == 'v')
	{
	    // "gR" or "gr" command
	    AppendCharToRedobuff('g');
	    AppendCharToRedobuff((cmdchar == 'v') ? 'r' : 'R');
	}
	अन्यथा
	{
	    यदि (cmdchar == K_PS)
		AppendCharToRedobuff('a');
	    अन्यथा
		AppendCharToRedobuff(cmdchar);
	    यदि (cmdchar == 'g')		    // "gI" command
		AppendCharToRedobuff('I');
	    अन्यथा यदि (cmdchar == 'r')	    // "r<CR>" command
		गिनती = 1;		    // insert only one <CR>
	}
    }

    यदि (cmdchar == 'R')
    {
	State = MODE_REPLACE;
    }
    अन्यथा यदि (cmdchar == 'V' || cmdchar == 'v')
    {
	State = MODE_VREPLACE;
	replaceState = MODE_VREPLACE;
	orig_पंक्ति_गिनती = curbuf->b_ml.ml_पंक्ति_गिनती;
	vr_पंक्तियाँ_changed = 1;
    }
    अन्यथा
	State = MODE_जोड़ो;

    may_trigger_modechanged();
    stop_जोड़ो_mode = FALSE;

#ifdef FEAT_CONCEAL
    // Check if the cursor line needs redrawing after changing State.  If
    // 'concealcursor' is "n" it needs to be redrawn without concealing.
    conceal_check_cursor_पंक्ति(cursor_पंक्ति_was_concealed);
#endif

    // Need to position cursor again when on a TAB and when on a char with
    // virtual text.
    यदि (gchar_cursor() == TAB
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
    यदि (curbuf->b_p_iminsert == B_IMODE_LMAP)
	State |= MODE_LANGMAP;
#ifdef HAVE_निवेश_METHOD
    im_set_active(curbuf->b_p_iminsert == B_IMODE_IM);
#endif

    setmouse();
    clear_showcmd();
#ifdef FEAT_RIGHTLEFT
    // there is no reverse replace mode
    revins_on = (State == MODE_जोड़ो && p_ri);
    यदि (revins_on)
	undisplay_dollar();
    revins_chars = 0;
    revins_legal = 0;
    revins_scol = -1;
#endif
    यदि (!p_ek)
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
    यदि (restart_edit != 0 && stuff_empty())
    {
	/*
	 * After a paste we consider text typed to be part of the insert for
	 * the pasted text. You can backspace over the pasted text too.
	 */
	यदि (where_paste_started.lnum)
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
	यदि (((ins_at_eol && curwin->w_cursor.lnum == o_lnum)
		    || curwin->w_curswant > curwin->w_virtcol)
		&& *(ptr = ml_get_curline() + curwin->w_cursor.col) != NUL)
	{
	    यदि (ptr[1] == NUL)
		++curwin->w_cursor.col;
	    अन्यथा यदि (has_mbyte)
	    {
		i = (*mb_ptr2len)(ptr);
		यदि (ptr[i] == NUL)
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
    यदि (!p_im && did_restart_edit == 0)
	foldOpenCursor();
#endif

    /*
     * If 'showmode' is set, show the current (insert/replace/..) mode.
     * A warning message for changing a readonly file is given here, before
     * actually changing anything.  It's put after the mode, if any.
     */
    i = 0;
    यदि (p_smd && संदेश_silent == 0)
	i = showmode();

    यदि (!p_im && did_restart_edit == 0)
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
    new_जोड़ो_skip = (पूर्णाङ्क)inserted.length;
    यदि (inserted.string != रिक्त)
	vim_free(inserted.string);

    old_indent = 0;

    /*
     * Main loop in Insert mode: repeat until Insert mode is left.
     */
    प्रति (;;)
    {
#ifdef FEAT_RIGHTLEFT
	यदि (!revins_legal)
	    revins_scol = -1;	    // reset on illegal motions
	अन्यथा
	    revins_legal = 0;
#endif
	यदि (arrow_used)	    // don't repeat insert when arrow key used
	    गिनती = 0;

	यदि (update_Insstart_orig)
	    Insstart_orig = Insstart;

	यदि (stop_जोड़ो_mode && !ins_compl_active())
	{
	    // ":stopinsert" used or 'insertmode' reset
	    गिनती = 0;
	    गच्छ doESCkey;
	}

	// set curwin->w_curswant for next K_DOWN or K_UP
	यदि (!arrow_used)
	    curwin->w_set_curswant = true;

	// If there is no typeahead may check for timestamps (e.g., for when a
	// menu invoked a shell command).
	यदि (stuff_empty())
	{
	    did_check_timestamps = FALSE;
	    यदि (need_check_timestamps)
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
	यदि (need_mouse_correct)
	    gui_mouse_correct();
#endif

#ifdef FEAT_FOLDING
	// Open fold at the cursor line, according to 'foldopen'.
	यदि (fdo_flags & FDO_जोड़ो)
	    foldOpenCursor();
	// Close folds where the cursor isn't, according to 'foldclose'
	यदि (!char_avail())
	    foldCheckClose();
#endif

#ifdef FEAT_JOB_CHANNEL
	यदि (bt_prompt(curbuf))
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
	यदि (curbuf->b_mod_set
		&& curwin->w_p_wrap
		&& !curwin->w_p_sms
		&& !did_backspace
		&& curwin->w_topline == old_topline
#ifdef FEAT_DIFF
		&& curwin->w_topfill == old_topfill
#endif
		&& गिनती <= 1
		)
	{
	    mincol = curwin->w_wcol;
	    validate_cursor_col();

	    यदि (
#ifdef FEAT_VARTABS
		curwin->w_wcol < mincol - tabstop_at(get_nolist_virtcol(),
						     curbuf->b_p_ts,
						     curbuf->b_p_vts_array,
						     FALSE)
#अन्यथा
		(पूर्णाङ्क)curwin->w_wcol < mincol - curbuf->b_p_ts
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
		यदि (curwin->w_topfill > 0)
		    --curwin->w_topfill;
		अन्यथा
#endif
#ifdef FEAT_FOLDING
		यदि (hasFolding(curwin->w_topline, रिक्त, &old_topline))
		    set_topline(curwin, old_topline + 1);
		अन्यथा
#endif
		    set_topline(curwin, curwin->w_topline + 1);
	    }
	}

	// May need to adjust w_topline to show the cursor.
	यदि (गिनती <= 1)
	    update_topline();

	did_backspace = FALSE;

	यदि (गिनती <= 1)
	    validate_cursor();		// may set must_redraw

	/*
	 * Redraw the display when no characters are waiting.
	 * Also shows mode, ruler and positions cursor.
	 */
	ins_redraw(TRUE);

	यदि (curwin->w_p_scb)
	    do_check_scrollbind(TRUE);

	यदि (curwin->w_p_crb)
	    do_check_cursorbind();
	यदि (गिनती <= 1)
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
	यदि (c != K_CURSORHOLD && c != K_COMPLETE_DELAY)
	    lastc = c;		// remember the previous char for CTRL-D

	// After using CTRL-G U the next cursor key will not break undo.
	यदि (dont_sync_undo == MAYBE)
	    dont_sync_undo = TRUE;
	अन्यथा
	    dont_sync_undo = FALSE;
	यदि (cmdchar == K_PS)
	    // Got here from normal mode when bracketed paste started.
	    c = K_PS;
	अन्यथा
	{
	    // Trigger autocomplete when entering Insert mode, either directly
	    // or via change commands like 'ciw', 'cw', etc., before the first
	    // character is typed.
	    यदि (ins_just_started)
	    {
		ins_just_started = FALSE;
		यदि (ins_compl_has_autocomplete() && !char_avail()
			&& curwin->w_cursor.col > 0)
		{
		    c = char_before_cursor();
		    यदि (vim_isprintc(c))
		    {
			ins_compl_enable_autocomplete();
			ins_compl_arm_autostart();
			ins_compl_init_get_longest();
#ifdef FEAT_RIGHTLEFT
			यदि (p_hkmap)
			    c = hkmap(c);		// Hebrew mode mapping
#endif
			// Defer until the delay expires (K_COMPLETE_DELAY), or
			// trigger now when no delay is in effect.
			यदि (!ins_compl_arm_autocomplete_delay())
			    गच्छ docomplete;
		    }
		}
	    }

	    कुरु
	    {
		c = safe_vgetc();

		यदि (stop_जोड़ो_mode
#ifdef FEAT_TERMINAL
			|| (c == K_IGNORE && term_use_loop())
#endif
		   )
		{
		    // Insert mode ended, possibly from a callback, or a timer
		    // must have opened a terminal window.
		    यदि (c != K_IGNORE && c != K_NOP)
			vungetc(c);
		    गिनती = 0;

		    यदि (!bt_prompt(curwin->w_buffer)
#ifdef FEAT_TERMINAL
			    && !bt_terminal(curwin->w_buffer)
#endif
			    && stop_जोड़ो_mode)
			// :stopinsert command via callback or via server command
			nomove = FALSE;
		    अन्यथा
			nomove = TRUE;
		    ins_compl_prep(ESC);
		    गच्छ doESCkey;
		}
	    } यावत् (c == K_IGNORE || c == K_NOP);
	}

	// Don't want K_CURSORHOLD for the second key, e.g., after CTRL-V.
	did_cursorhold = TRUE;
	यदि (c != K_CURSORHOLD && c != K_COMPLETE_DELAY)
	{
	    // Don't want delayed autocompletion from the previous key either.
	    ins_compl_clear_autocomplete_delay();
	    ins_compl_disarm_autostart();
	    // A completion already on screen goes on being what it was.
	    यदि (!ins_compl_active())
		ins_compl_disable_autocomplete();
	}

#ifdef FEAT_RIGHTLEFT
	यदि (p_hkmap && कुंजी_Typed)
	    c = hkmap(c);		// Hebrew mode mapping
#endif

	// If the window was made so small that nothing shows, make it at least
	// one line and one column when typing.
	यदि (कुंजी_Typed && !कुंजी_Stuffed)
	    win_ensure_size();

	/*
	 * Special handling of keys while the popup menu is visible or wanted
	 * and the cursor is still in the completed word.  Only when there is
	 * a match, skip this when no matches were found.
	 */
	यदि (ins_compl_active() && curwin->w_cursor.col >= ins_compl_col()
		&& ins_compl_has_shown_match() && pum_wanted())
	{
	    // BS: Delete one character from "compl_leader".
	    यदि ((c == K_BS || c == Ctrl_H)
			&& curwin->w_cursor.col > ins_compl_col()
			&& (c = ins_compl_bs()) == NUL)
		अनुवर्त;

	    // When no match was selected or it was edited.
	    यदि (!ins_compl_used_match())
	    {
		// CTRL-L: Add one character from the current match to
		// "compl_leader".  Except when at the original match and
		// there is nothing to add, CTRL-L works like CTRL-P then.
		यदि (c == Ctrl_L
			&& (!ctrl_x_mode_पंक्ति_or_eval()
			    || ins_compl_long_shown_match()))
		{
		    ins_compl_addfrommatch();
		    अनुवर्त;
		}

		// A non-white character that fits in with the current
		// completion: Add to "compl_leader".
		यदि (ins_compl_accept_char(c))
		{
#यदि defined(FEAT_EVAL)
		    // Trigger InsertCharPre.
		    char_u *str = do_जोड़ो_char_pre(c);
		    char_u *p;

		    यदि (str != रिक्त)
		    {
			प्रति (p = str; *p != NUL; MB_PTR_ADV(p))
			    ins_compl_addleader(PTR2CHAR(p));
			vim_free(str);
		    }
		    अन्यथा
#endif
			ins_compl_addleader(c);
		    अनुवर्त;
		}

		// Pressing CTRL-Y selects the current match.  When
		// ins_compl_enter_selects() is set the Enter key does the
		// same.
		यदि ((c == Ctrl_Y || (ins_compl_enter_selects()
				    && (c == CAR || c == K_KENTER || c == NL)))
			&& stop_arrow() == OK)
		{
		    ins_compl_delete();
		    यदि (ins_compl_preinsert_longest()
			    && !ins_compl_is_match_selected())
		    {
			ins_compl_जोड़ो(FALSE, TRUE);
			ins_compl_init_get_longest();
			अनुवर्त;
		    }
		    अन्यथा
			ins_compl_जोड़ो(FALSE, FALSE);
		}
		// Delete preinserted text when typing special chars
		अन्यथा यदि (IS_WHITE_NL_OR_NUL(c) && ins_compl_preinsert_effect())
		    ins_compl_delete();
	    }
	}

	// Prepare for or stop CTRL-X mode.  This doesn't do completion, but
	// it does fix up the text when finishing completion.
	ins_compl_init_get_longest();
	यदि (ins_compl_prep(c))
	    अनुवर्त;

	// CTRL-\ CTRL-N goes to Normal mode,
	// CTRL-\ CTRL-G goes to mode selected with 'insertmode',
	// CTRL-\ CTRL-O is like CTRL-O but without moving the cursor.
	यदि (c == Ctrl_BSL)
	{
	    // may need to redraw when no more chars available now
	    ins_redraw(FALSE);
	    ++no_mapping;
	    ++allow_कुंजियाँ;
	    c = plain_vgetc();
	    --no_mapping;
	    --allow_कुंजियाँ;
	    यदि (c != Ctrl_N && c != Ctrl_G && c != Ctrl_O)
	    {
		// it's something else
		vungetc(c);
		c = Ctrl_BSL;
	    }
	    अन्यथा यदि (c == Ctrl_G && p_im)
		अनुवर्त;
	    अन्यथा
	    {
		यदि (c == Ctrl_O)
		{
		    ins_ctrl_o();
		    ins_at_eol = FALSE;	// cursor keeps its column
		    nomove = TRUE;
		}
		गिनती = 0;
		गच्छ doESCkey;
	    }
	}

#ifdef FEAT_DIGRAPHS
	c = do_digraph(c);
#endif

	यदि ((c == Ctrl_V || c == Ctrl_Q) && ctrl_x_mode_cmdline())
	    गच्छ docomplete;
	यदि (c == Ctrl_V || c == Ctrl_Q)
	{
	    ins_ctrl_v();
	    c = Ctrl_V;	// pretend CTRL-V is last typed character
	    अनुवर्त;
	}

	यदि (cindent_on() && ctrl_x_mode_none())
	{
	    // A key name preceded by a bang means this key is not to be
	    // inserted.  Skip ahead to the re-indenting below.
	    // A key name preceded by a star means that indenting has to be
	    // done before inserting the key.
	    पंक्ति_is_white = inindent(0);
	    यदि (in_cinkeys(c, '!', पंक्ति_is_white))
		गच्छ force_cindent;
	    यदि (can_cindent && in_cinkeys(c, '*', पंक्ति_is_white)
							&& stop_arrow() == OK)
		do_c_व्यंजक_indent();
	}

#ifdef FEAT_RIGHTLEFT
	यदि (curwin->w_p_rl)
	    विकल्प (c)
	    {
		पक्ष K_बायाँ:	c = K_दायाँ; भङ्ग;
		पक्ष K_S_बायाँ:	c = K_S_दायाँ; भङ्ग;
		पक्ष K_C_बायाँ:	c = K_C_दायाँ; भङ्ग;
		पक्ष K_दायाँ:	c = K_बायाँ; भङ्ग;
		पक्ष K_S_दायाँ: c = K_S_बायाँ; भङ्ग;
		पक्ष K_C_दायाँ: c = K_C_बायाँ; भङ्ग;
	    }
#endif

	/*
	 * If 'keymodel' contains "startsel", may start selection.  If it
	 * does, a CTRL-O and c will be stuffed, we need to get these
	 * characters.
	 */
	यदि (ins_start_select(c))
	    अनुवर्त;

	/*
	 * The big switch to handle a character in insert mode.
	 */
	विकल्प (c)
	{
	पक्ष ESC:	// End input mode
	    यदि (echeck_abbr(ESC + ABBR_OFF))
		भङ्ग;
	    // FALLTHROUGH

	पक्ष Ctrl_C:	// End input mode
	    यदि (c == Ctrl_C && cmdwin_type != 0)
	    {
		// Close the cmdline window.
		cmdwin_result = K_IGNORE;
		got_int = FALSE; // don't stop executing autocommands et al.
		nomove = TRUE;
		गच्छ doESCkey;
	    }
#ifdef FEAT_JOB_CHANNEL
	    यदि (c == Ctrl_C && bt_prompt(curbuf))
	    {
		यदि (invoke_prompt_interrupt())
		{
		    यदि (!bt_prompt(curbuf))
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
	    यदि (goto_im())
	    {
		यदि (got_int)
		{
		    (शून्य)vgetc();		// flush all buffers
		    got_int = FALSE;
		}
		अन्यथा
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
	    यदि (ins_at_eol && gchar_cursor() == NUL)
		o_lnum = curwin->w_cursor.lnum;

	    यदि (ins_esc(&गिनती, cmdchar, nomove))
	    {
		// When CTRL-C was typed got_int will be set, with the result
		// that the autocommands won't be executed. When mapped got_int
		// is not set, but let's keep the behavior the same.
		यदि (cmdchar != 'r' && cmdchar != 'v' && c != Ctrl_C)
		    ins_apply_autocmds(EVENT_INSERTLEAVE);
		did_cursorhold = FALSE;

		// ins_redraw() triggers TextChangedI only when no characters
		// are in the typeahead buffer, so reset curbuf->b_last_changedtick only
		// if the TextChangedI was not blocked by char_avail() (e.g. using :norm!)
		// and the TextChangedI autocommand has been triggered.
		यदि (!char_avail() && curbuf->b_last_changedtick_i == CHANGEDTICK(curbuf))
		    curbuf->b_last_changedtick = CHANGEDTICK(curbuf);
		प्रत्यागम (c == Ctrl_O);
	    }
	    अनुवर्त;

	पक्ष Ctrl_Z:	// suspend when 'insertmode' set
	    यदि (!p_im)
		गच्छ normalchar;	// insert CTRL-Z as normal char
	    do_cmdline_cmd((char_u *)"stop");
#ifdef CURSOR_SHAPE
	    ui_cursor_shape();		// may need to update cursor shape
#endif
	    अनुवर्त;

	पक्ष Ctrl_O:	// execute one command
#ifdef FEAT_COMPL_FUNC
	    यदि (ctrl_x_mode_omni())
		गच्छ docomplete;
#endif
	    यदि (echeck_abbr(Ctrl_O + ABBR_OFF))
		भङ्ग;
	    ins_ctrl_o();

	    // don't move the cursor left when 'virtualedit' has "onemore".
	    यदि (get_ve_flags() & VE_ONEMORE)
	    {
		ins_at_eol = FALSE;
		nomove = TRUE;
	    }
	    गिनती = 0;
	    गच्छ doESCkey;

	पक्ष K_INS:	// toggle insert/replace mode
	पक्ष K_KINS:
	    ins_जोड़ो(replaceState);
	    भङ्ग;

	पक्ष K_SELECT:	// end of Select mode mapping - ignore
	    भङ्ग;

	पक्ष K_HELP:	// Help key works like <ESC> <Help>
	पक्ष K_F1:
	पक्ष K_XF1:
	    stuffcharReadbuff(K_HELP);
	    यदि (p_im)
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
	    यदि (stuff_inserted(NUL, 1L, (c == Ctrl_A)) == FAIL
						   && c != Ctrl_A && !p_im)
		गच्छ doESCkey;		// quit insert mode
	    inserted_space = FALSE;
	    भङ्ग;

	पक्ष Ctrl_R:	// insert the contents of a register
	    यदि (ctrl_x_mode_register() && !ins_compl_active())
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
	    यदि (!p_ari)
		गच्छ normalchar;
	    ins_ctrl_();
	    भङ्ग;
#endif

	पक्ष Ctrl_D:	// Make indent one shiftwidth smaller.
#यदि defined(FEAT_FIND_ID)
	    यदि (ctrl_x_mode_path_defines())
		गच्छ docomplete;
#endif
	    // FALLTHROUGH

	पक्ष Ctrl_T:	// Make indent one shiftwidth greater.
	    यदि (c == Ctrl_T && ctrl_x_mode_thesaurus())
	    {
		यदि (has_compl_option(FALSE))
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
	    यदि (did_backspace)
		MAY_TRIGGER_AUTOCOMPLETE(c);
	    भङ्ग;

	पक्ष Ctrl_W:	// delete word before the cursor
#ifdef FEAT_JOB_CHANNEL
	    यदि (bt_prompt(curbuf) && (mod_mask & MOD_MASK_SHIFT) == 0)
	    {
		// In a prompt window CTRL-W is used for window commands.
		// Use Shift-CTRL-W to delete a word.
		stuffcharReadbuff(Ctrl_W);
		restart_edit = 'A';
		nomove = TRUE;
		गिनती = 0;
		गच्छ doESCkey;
	    }
#endif
	    did_backspace = ins_bs(c, BACKSPACE_WORD, &inserted_space);
	    auto_format(FALSE, TRUE);
	    यदि (did_backspace)
		MAY_TRIGGER_AUTOCOMPLETE(c);
	    भङ्ग;

	पक्ष Ctrl_U:	// delete all inserted text in current line
#ifdef FEAT_COMPL_FUNC
	    // CTRL-X CTRL-U completes with 'completefunc'.
	    यदि (ctrl_x_mode_function())
		गच्छ docomplete;
#endif
	    did_backspace = ins_bs(c, BACKSPACE_पंक्ति, &inserted_space);
	    auto_format(FALSE, TRUE);
	    inserted_space = FALSE;
	    यदि (did_backspace)
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
	    ins_mousescroll(MSCR_बायाँ);
	    भङ्ग;

	पक्ष K_MOUSERIGHT: // Scroll wheel right
	    ins_mousescroll(MSCR_दायाँ);
	    भङ्ग;

	पक्ष K_PS:
	    bracketed_paste(PASTE_जोड़ो, FALSE, रिक्त);
	    यदि (cmdchar == K_PS)
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
		यदि (term_use_loop())
		    // Started a terminal that gets the input, exit Insert mode.
		    गच्छ doESCkey;
#endif
		यदि (curbuf->b_u_synced
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
	    यदि (dont_sync_undo == TRUE)
		dont_sync_undo = MAYBE;
	    भङ्ग;

	पक्ष K_COMPLETE_DELAY:	// 'autocompletedelay' expired
	    // If CTRL-G U was used apply it to the next typed key.
	    यदि (dont_sync_undo == TRUE)
		dont_sync_undo = MAYBE;
	    ins_compl_clear_autocomplete_delay();
	    यदि (!ins_compl_has_autocomplete() || char_avail()
		    || curwin->w_cursor.col == 0)
		भङ्ग;
	    c = char_before_cursor();
	    यदि (!vim_isprintc(c))
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
	    यदि (mod_mask != MOD_MASK_ALT)
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

	पक्ष K_बायाँ:	// <Left>
	    यदि (mod_mask & (MOD_MASK_SHIFT|MOD_MASK_CTRL))
		ins_s_बायाँ();
	    अन्यथा
		ins_बायाँ();
	    भङ्ग;

	पक्ष K_S_बायाँ:	// <S-Left>
	पक्ष K_C_बायाँ:
	    ins_s_बायाँ();
	    भङ्ग;

	पक्ष K_दायाँ:	// <Right>
	    यदि (mod_mask & (MOD_MASK_SHIFT|MOD_MASK_CTRL))
		ins_s_दायाँ();
	    अन्यथा
		ins_दायाँ();
	    भङ्ग;

	पक्ष K_S_दायाँ:	// <S-Right>
	पक्ष K_C_दायाँ:
	    ins_s_दायाँ();
	    भङ्ग;

	पक्ष K_UP:	// <Up>
	    यदि (pum_visible())
		गच्छ docomplete;
	    यदि (mod_mask & MOD_MASK_SHIFT)
		ins_pageup();
	    अन्यथा
		ins_up(FALSE);
	    भङ्ग;

	पक्ष K_S_UP:	// <S-Up>
	पक्ष K_PAGEUP:
	पक्ष K_KPAGEUP:
	    यदि (pum_visible())
	    {
#ifdef FEAT_PROP_POPUP
		// CTRL-SHIFT-<Up> scrolls the info popup up a line,
		// CTRL-SHIFT-<PageUp> a page.  Shift is folded into K_S_UP but
		// stays in mod_mask for PageUp, hence the asymmetric check.
		यदि (c == K_S_UP ? (mod_mask & MOD_MASK_CTRL)
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
	    यदि (pum_visible())
		गच्छ docomplete;
	    यदि (mod_mask & MOD_MASK_SHIFT)
		ins_pagedown();
	    अन्यथा
		ins_down(FALSE);
	    भङ्ग;

	पक्ष K_S_DOWN:	// <S-Down>
	पक्ष K_PAGEDOWN:
	पक्ष K_KPAGEDOWN:
	    यदि (pum_visible())
	    {
#ifdef FEAT_PROP_POPUP
		// CTRL-SHIFT-<Down>/<PageDown> scroll the info popup down.
		यदि (c == K_S_DOWN ? (mod_mask & MOD_MASK_CTRL)
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
#यदि defined(FEAT_FIND_ID)
	    यदि (ctrl_x_mode_path_patterns())
		गच्छ docomplete;
#endif
	    inserted_space = FALSE;
	    यदि (ins_tab())
		गच्छ normalchar;	// insert TAB as a normal char
	    auto_format(FALSE, TRUE);
	    भङ्ग;

	पक्ष K_KENTER:	// <Enter>
	    c = CAR;
	    // FALLTHROUGH
	पक्ष CAR:
	पक्ष NL:
#यदि defined(FEAT_QUICKFIX)
	    // In a quickfix window a <CR> jumps to the error under the
	    // cursor.
	    यदि (bt_quickfix(curbuf) && c == CAR)
	    {
		यदि (curwin->w_llist_ref == रिक्त)    // quickfix window
		    do_cmdline_cmd((char_u *)".cc");
		अन्यथा				    // location list window
		    do_cmdline_cmd((char_u *)".ll");
		भङ्ग;
	    }
#endif
	    यदि (cmdwin_type != 0)
	    {
		// Execute the command in the cmdline window.
		cmdwin_result = CAR;
		गच्छ doESCkey;
	    }
#ifdef FEAT_JOB_CHANNEL
	    यदि (bt_prompt(curbuf))
	    {
		invoke_prompt_callback();
		यदि (!bt_prompt(curbuf))
		    // buffer changed to a non-prompt buffer, get out of
		    // Insert mode
		    गच्छ doESCkey;
		भङ्ग;
	    }
#endif
	    यदि (ins_eol(c) == FAIL && !p_im)
		गच्छ doESCkey;	    // out of memory
	    auto_format(FALSE, FALSE);
	    inserted_space = FALSE;
	    भङ्ग;

	पक्ष Ctrl_K:	    // digraph or keyword completion
	    यदि (ctrl_x_mode_dictionary())
	    {
		यदि (has_compl_option(TRUE))
		    गच्छ docomplete;
		भङ्ग;
	    }
#ifdef FEAT_DIGRAPHS
	    c = ins_digraph();
	    यदि (c == NUL)
		भङ्ग;
#endif
	    गच्छ normalchar;

	पक्ष Ctrl_X:	// Enter CTRL-X mode
	    ins_ctrl_x();
	    भङ्ग;

	पक्ष Ctrl_RSB:	// Tag name completion after ^X
	    यदि (!ctrl_x_mode_tags())
		गच्छ normalchar;
	    गच्छ docomplete;

	पक्ष Ctrl_F:	// File name completion after ^X
	    यदि (!ctrl_x_mode_files())
		गच्छ normalchar;
	    गच्छ docomplete;

	पक्ष 's':	// Spelling completion after ^X
	पक्ष Ctrl_S:
	    यदि (!ctrl_x_mode_spell())
		गच्छ normalchar;
	    गच्छ docomplete;

	पक्ष Ctrl_L:	// Whole line completion after ^X
	    यदि (!ctrl_x_mode_whole_पंक्ति())
	    {
		// CTRL-L with 'insertmode' set: Leave Insert mode
		यदि (p_im)
		{
		    यदि (echeck_abbr(Ctrl_L + ABBR_OFF))
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
	    यदि (pum_visible() && (mod_mask & MOD_MASK_SHIFT)
		    && (c == Ctrl_P || c == Ctrl_N))
	    {
		popup_scroll_info(c == Ctrl_P ? -1 : 1, false);
		भङ्ग;
	    }
#endif
	    // if 'complete' is empty then plain ^P is no longer special,
	    // but it is under other ^X modes
	    यदि (*curbuf->b_p_cpt == NUL
		    && (ctrl_x_mode_normal() || ctrl_x_mode_whole_पंक्ति())
		    && !compl_status_local())
		गच्छ normalchar;

docomplete:
	    ins_compl_clear_autocomplete_delay();
	    compl_busy = TRUE;
#ifdef FEAT_FOLDING
	    disable_fold_update++;  // don't redraw folds here
#endif
	    यदि (ins_complete(c, TRUE) == FAIL)
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
	    यदि (c == intr_char)		// special interrupt char
		गच्छ do_intr;
#endif

normalchar:
	    /*
	     * Insert a normal character.
	     */
#यदि defined(FEAT_EVAL)
	    यदि (!p_paste)
	    {
		// Trigger InsertCharPre.
		char_u *str = do_जोड़ो_char_pre(c);
		char_u *p;

		यदि (str != रिक्त)
		{
		    यदि (*str != NUL && stop_arrow() != FAIL)
		    {
			// Insert the new value of v:char literally.
			प्रति (p = str; *p != NUL; MB_PTR_ADV(p))
			{
			    c = PTR2CHAR(p);
			    यदि (c == CAR || c == K_KENTER || c == NL)
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
		यदि (c == NUL)
		    भङ्ग;
	    }
#endif
	    // Try to perform smart-indenting.
	    ins_try_si(c);

	    यदि (c == ' ')
	    {
		inserted_space = TRUE;
		यदि (inindent(0))
		    can_cindent = FALSE;
		यदि (Insstart_blank_vcol == MAXCOL
			&& curwin->w_cursor.lnum == Insstart.lnum)
		    Insstart_blank_vcol = get_nolist_virtcol();
	    }

	    // Insert a normal character and check for abbreviations on a
	    // special character.  Let CTRL-] expand abbreviations without
	    // inserting it.
	    यदि (vim_iswordc(c) || (!echeck_abbr(
			// Add ABBR_OFF for characters above 0x100, this is
			// what check_abbr() expects.
				(has_mbyte && c >= 0x100) ? (c + ABBR_OFF) : c)
			&& c != Ctrl_RSB))
	    {
		जोड़ो_special(c, FALSE, FALSE);
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
	    यदि (ins_compl_has_autocomplete() && !char_avail()
		    && vim_isprintc(c))
		TRIGGER_AUTOCOMPLETE();

	    भङ्ग;
	}   // end of switch (c)

	// If typed something may trigger CursorHoldI again; K_COMPLETE_DELAY is
	// injected, not typed.
	यदि (c != K_CURSORHOLD && c != K_COMPLETE_DELAY
#ifdef FEAT_COMPL_FUNC
		// but not in CTRL-X mode, a script can't restore the state
		&& ctrl_x_mode_normal()
#endif
	       )
	    did_cursorhold = FALSE;

	// Check if we need to cancel completion mode because the window
	// or tab page was changed
	यदि (ins_compl_active() && !ins_compl_win_active(curwin))
	    ins_compl_cancel();

	// If the cursor was moved we didn't just insert a space
	यदि (arrow_used)
	    inserted_space = FALSE;

	यदि (can_cindent && cindent_on() && ctrl_x_mode_normal())
	{
force_cindent:
	    /*
	     * Indent now if a key was typed that is in 'cinkeys'.
	     */
	    यदि (in_cinkeys(c, ' ', पंक्ति_is_white))
	    {
		यदि (stop_arrow() == OK)
		    // re-indent the current line
		    do_c_व्यंजक_indent();
	    }
	}

    }	// for (;;)
    // NOTREACHED
}

    पूर्णाङ्क
ins_need_undo_get(शून्य)
{
    प्रत्यागम ins_need_undo;
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
    linenr_T	conceal_old_cursor_पंक्ति = 0;
    linenr_T	conceal_new_cursor_पंक्ति = 0;
    पूर्णाङ्क		conceal_update_पंक्तियाँ = FALSE;
#endif

    यदि (char_avail())
	प्रत्यागम;

    // Trigger CursorMoved if the cursor moved.  Not when the popup menu is
    // visible, the command might delete it.
    यदि (ready && (has_cursormovedI()
#ifdef FEAT_PROP_POPUP
		|| popup_visible
#endif
#यदि defined(FEAT_CONCEAL)
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
	यदि (syntax_present(curwin) && must_redraw)
	    update_screen(0);
#endif
	यदि (has_cursormovedI())
	{
	    // Make sure curswant is correct, an autocommand may call
	    // getcurpos().
	    update_curswant();
	    ins_apply_autocmds(EVENT_CURSORMOVEDI);
	}
#ifdef FEAT_PROP_POPUP
	यदि (popup_visible)
	    popup_check_cursor_pos();
#endif
#ifdef FEAT_CONCEAL
	यदि (curwin->w_p_cole > 0)
	{
	    conceal_old_cursor_पंक्ति = last_cursormoved.lnum;
	    conceal_new_cursor_पंक्ति = curwin->w_cursor.lnum;
	    conceal_update_पंक्तियाँ = TRUE;
	}
#endif
	last_cursormoved = curwin->w_cursor;
    }

    // Trigger TextChangedI if b_changedtick_i differs.
    यदि (ready && has_textchangedI()
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
	यदि (tick != CHANGEDTICK(curbuf))  // see ins_apply_autocmds()
	    u_save(curwin->w_cursor.lnum,
					(linenr_T)(curwin->w_cursor.lnum + 1));
    }

    // Trigger TextChangedP if b_changedtick_pum differs. When the popupmenu
    // closes TextChangedI will need to trigger for backwards compatibility,
    // thus use different b_last_changedtick* variables.
    यदि (ready && has_textchangedP()
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
	यदि (tick != CHANGEDTICK(curbuf))  // see ins_apply_autocmds()
	    u_save(curwin->w_cursor.lnum,
					(linenr_T)(curwin->w_cursor.lnum + 1));
    }

    यदि (ready)
	may_trigger_win_scrolled_resized();

    // Trigger SafeState if nothing is pending.
    may_trigger_safestate(ready
	    && !ins_compl_active()
	    && !pum_visible());

#यदि defined(FEAT_CONCEAL)
    यदि ((conceal_update_पंक्तियाँ
	    && (conceal_old_cursor_पंक्ति != conceal_new_cursor_पंक्ति
		|| conceal_cursor_पंक्ति(curwin)))
	    || need_cursor_पंक्ति_redraw)
    {
	यदि (conceal_old_cursor_पंक्ति != conceal_new_cursor_पंक्ति)
	    redrawWinline(curwin, conceal_old_cursor_पंक्ति);
	redrawWinline(curwin, conceal_new_cursor_पंक्ति == 0
			    ? curwin->w_cursor.lnum : conceal_new_cursor_पंक्ति);
	curwin->w_valid &= ~VALID_CROW;
	need_cursor_पंक्ति_redraw = FALSE;
    }
#endif
    यदि (must_redraw)
	update_screen(0);
    अन्यथा यदि (clear_cmdline || redraw_cmdline)
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

    यदि (redrawing() && !char_avail())
    {
	edit_putchar('^', TRUE);
	did_putchar = TRUE;
    }
    AppendToRedobuff((char_u *)CTRL_V_STR);	// CTRL-V

    add_to_showcmd_c(Ctrl_V);

    // Do not change any modifyOtherKeys ESC sequence to a normal key for
    // CTRL-SHIFT-V.
    c = get_literal(mod_mask & MOD_MASK_SHIFT);
    यदि (did_putchar)
	// when the line fits in 'columns' the '^' is at the start of the next
	// line and will not removed by the redraw
	edit_unputchar();
    clear_showcmd();

    जोड़ो_special(c, FALSE, TRUE);
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
decode_Modify_Other_कुंजियाँ(पूर्णाङ्क c)
{
    char_u  *p = typebuf.tb_buf + typebuf.tb_off;
    पूर्णाङ्क	    idx;
    पूर्णाङ्क	    form = 0;
    पूर्णाङ्क	    argidx = 0;
    पूर्णाङ्क	    arg[2] = {0, 0};

    // Recognize:
    // form 0: {lead}{key};{modifier}u
    // form 1: {lead}27;{modifier};{key}~
    यदि (typebuf.tb_len >= 4 && (c == CSI || (c == ESC && *p == '[')))
    {
	idx = (*p == '[');
	यदि (p[idx] == '2' && p[idx + 1] == '7' && p[idx + 2] == ';' &&
		kitty_protocol_state != KKPS_ENABLED)
	{
	    form = 1;
	    idx += 3;
	}
	यावत् (idx < typebuf.tb_len && argidx < 2)
	{
	    यदि (p[idx] == ';')
		++argidx;
	    अन्यथा यदि (VIM_ISDIGIT(p[idx]))
		arg[argidx] = arg[argidx] * 10 + (p[idx] - '0');
	    अन्यथा
		भङ्ग;
	    ++idx;
	}
	पूर्णाङ्क kitty_no_mods = argidx == 0 && kitty_protocol_state == KKPS_ENABLED;
	यदि (idx < typebuf.tb_len
		&& p[idx] == (form == 1 ? '~' : 'u')
		&& (argidx == 1 || kitty_no_mods))
	{
	    // Match, consume the code.
	    typebuf.tb_off += idx + 1;
	    typebuf.tb_len -= idx + 1;
#यदि defined(FEAT_CLIENTSERVER) || defined(FEAT_EVAL)
	    यदि (typebuf.tb_len == 0)
		typebuf_was_filled = FALSE;
#endif

	    mod_mask = kitty_no_mods ? 0 : decode_modifiers(arg[!form]);
	    c = merge_modify_Other_कुंजियाँ(arg[form], &mod_mask);
	}
    }

    प्रत्यागम c;
}

/*
 * Put a character directly onto the screen.  It's not stored in a buffer.
 * Used while handling CTRL-K, CTRL-V, etc. in Insert mode.
 */
स्थिर पूर्णाङ्क  pc_status;
#define PC_STATUS_UNSET	0	// pc_bytes was not set
#define PC_STATUS_दायाँ	1	// right half of double-wide char
#define PC_STATUS_बायाँ	2	// left half of double-wide char
#define PC_STATUS_SET	3	// pc_bytes was filled
स्थिर char_u pc_bytes[MB_MAXBYTES + 1]; // saved bytes
स्थिर पूर्णाङ्क  pc_attr;
स्थिर पूर्णाङ्क  pc_row;
स्थिर पूर्णाङ्क  pc_col;

    शून्य
edit_putchar(पूर्णाङ्क c, पूर्णाङ्क highlight)
{
    पूर्णाङ्क	    attr;

    यदि (Screen_पंक्तियाँ == रिक्त)
	प्रत्यागम;

    update_topline();	// just in case w_topline isn't valid
    validate_cursor();
    यदि (highlight)
	attr = HL_ATTR(HLF_8);
    अन्यथा
	attr = 0;
    pc_row = W_WINROW(curwin) + curwin->w_wrow;
    pc_col = curwin->w_wincol;
    pc_status = PC_STATUS_UNSET;
#ifdef FEAT_RIGHTLEFT
    यदि (curwin->w_p_rl)
    {
	pc_col += curwin->w_width - 1 - curwin->w_wcol;
	यदि (has_mbyte)
	{
	    पूर्णाङ्क fix_col = mb_fix_col(pc_col, pc_row);

	    यदि (fix_col != pc_col)
	    {
		screen_putchar(' ', pc_row, fix_col, attr);
		--curwin->w_wcol;
		pc_status = PC_STATUS_दायाँ;
	    }
	}
    }
    अन्यथा
#endif
    {
	pc_col += curwin->w_wcol;
	यदि (mb_lefthalve(pc_row, pc_col))
	    pc_status = PC_STATUS_बायाँ;
    }

    // save the character to be able to put it back
    यदि (pc_status == PC_STATUS_UNSET)
    {
	screen_getbytes(pc_row, pc_col, pc_bytes, &pc_attr);
	pc_status = PC_STATUS_SET;
    }
    screen_putchar(c, pc_row, pc_col, attr);
}

#यदि defined(FEAT_JOB_CHANNEL)
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
    यदि (pc_status != PC_STATUS_UNSET && pc_row >= संदेश_scrolled)
    {
	यदि (pc_status == PC_STATUS_दायाँ)
	    ++curwin->w_wcol;
	यदि (pc_status == PC_STATUS_दायाँ || pc_status == PC_STATUS_बायाँ)
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

    यदि (!redrawing())
	प्रत्यागम;

    cursor_off();
    save_col = curwin->w_cursor.col;
    curwin->w_cursor.col = col;
    यदि (has_mbyte)
    {
	char_u *p;

	// If on the last byte of a multi-byte move to the first byte.
	p = ml_get_curline();
	curwin->w_cursor.col -= (*mb_head_off)(p, p + col);
    }
    curs_columns(FALSE);	    // recompute w_wrow and w_wcol
    यदि (curwin->w_wcol < curwin->w_width)
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
    यदि (dollar_vcol < 0)
	प्रत्यागम;

    dollar_vcol = -1;
    redrawWinline(curwin, curwin->w_cursor.lnum);
}

/*
 * Truncate the space at the end of a line.  This is to be used only in an
 * insert mode.  It handles fixing the replace stack for MODE_REPLACE and
 * MODE_VREPLACE modes.
 */
    शून्य
truncate_spaces(char_u *पंक्ति, size_t len)
{
    पूर्णाङ्क	    i;

    // find start of trailing white space
    प्रति (i = (पूर्णाङ्क)len - 1; i >= 0 && VIM_ISWHITE(पंक्ति[i]); i--)
    {
	यदि (State & REPLACE_FLAG)
	    replace_join(0);	    // remove a NUL from the replace stack
    }
    पंक्ति[i + 1] = NUL;
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
    यावत् ((पूर्णाङ्क)curwin->w_cursor.col > col)
    {
	curwin->w_cursor.col--;
	यदि (State & REPLACE_FLAG)
	    replace_do_bs(col);
	अन्यथा यदि (!del_char_after_col(col))
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
    यदि (enc_utf8 && limit_col >= 0)
    {
	colnr_T ecol = curwin->w_cursor.col + 1;

	// Make sure the cursor is at the start of a character, but
	// skip forward again when going too far back because of a
	// composing character.
	mb_adjust_cursor();
	यावत् (curwin->w_cursor.col < (colnr_T)limit_col)
	{
	    पूर्णाङ्क l = utf_ptr2len(ml_get_cursor());

	    यदि (l == 0)  // end of line
		भङ्ग;
	    curwin->w_cursor.col += l;
	}
	यदि (*ml_get_cursor() == NUL || curwin->w_cursor.col == ecol)
	    प्रत्यागम FALSE;
	del_bytes((long)((पूर्णाङ्क)ecol - curwin->w_cursor.col), FALSE, TRUE);
    }
    अन्यथा
	(शून्य)del_char(FALSE);
    प्रत्यागम TRUE;
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
get_literal(पूर्णाङ्क no_Reduce_कुंजियाँ)
{
    पूर्णाङ्क		cc;
    पूर्णाङ्क		nc;
    पूर्णाङ्क		i;
    पूर्णाङ्क		hex = FALSE;
    पूर्णाङ्क		octal = FALSE;
    पूर्णाङ्क		unicode = 0;

    यदि (got_int)
	प्रत्यागम Ctrl_C;

#ifdef FEAT_GUI
    /*
     * In GUI there is no point inserting the internal code for a special key.
     * It is more useful to insert the string "<KEY>" instead.	This would
     * probably be useful in a text window too, but it would not be
     * vi-compatible (maybe there should be an option for it?) -- webb
     */
    यदि (gui.in_use)
    {
	++allow_कुंजियाँ;
	यदि (no_Reduce_कुंजियाँ)
	    ++no_reduce_कुंजियाँ;
    }
#endif
#ifdef USE_ON_FLY_SCROLL
    dont_scroll = TRUE;		// disallow scrolling here
#endif
    ++no_mapping;		// don't map the next key hits
    cc = 0;
    i = 0;
    प्रति (;;)
    {
	nc = plain_vgetc();
	यदि ((nc == ESC || nc == CSI) && !no_Reduce_कुंजियाँ)
	    nc = decode_Modify_Other_कुंजियाँ(nc);

	यदि ((mod_mask & ~MOD_MASK_SHIFT) != 0)
	    // A character with non-Shift modifiers should not be a valid
	    // character for i_CTRL-V_digit.
	    भङ्ग;

	यदि ((State & MODE_CMDLINE) == 0 && MB_BYTE2LEN_CHECK(nc) == 1)
	    add_to_showcmd(nc);
	यदि (nc == 'x' || nc == 'X')
	    hex = TRUE;
	अन्यथा यदि (nc == 'o' || nc == 'O')
	    octal = TRUE;
	अन्यथा यदि (nc == 'u' || nc == 'U')
	    unicode = nc;
	अन्यथा
	{
	    यदि (hex || unicode != 0)
	    {
		यदि (!vim_isxdigit(nc))
		    भङ्ग;
		cc = cc * 16 + hex2nr(nc);
	    }
	    अन्यथा यदि (octal)
	    {
		यदि (nc < '0' || nc > '7')
		    भङ्ग;
		cc = cc * 8 + nc - '0';
	    }
	    अन्यथा
	    {
		यदि (!VIM_ISDIGIT(nc))
		    भङ्ग;
		cc = cc * 10 + nc - '0';
	    }

	    ++i;
	}

	यदि (cc > 255 && unicode == 0)
	    cc = 255;		// limit range to 0-255
	nc = 0;

	यदि (hex)		// hex: up to two chars
	{
	    यदि (i >= 2)
		भङ्ग;
	}
	अन्यथा यदि (unicode)	// Unicode: up to four or eight chars
	{
	    यदि ((unicode == 'u' && i >= 4) || (unicode == 'U' && i >= 8))
		भङ्ग;
	}
	अन्यथा यदि (i >= 3)	// decimal or octal: up to three chars
	    भङ्ग;
    }
    यदि (i == 0)	    // no number entered
    {
	यदि (nc == K_ZERO)   // NUL is stored as NL
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

    यदि (cc == 0)	// NUL is stored as NL
	cc = '\n';
    यदि (enc_dbcs && (cc & 0xff) == 0)
	cc = '?';	// don't accept an illegal DBCS char, the NUL in the
			// second byte will cause trouble!

    --no_mapping;
#ifdef FEAT_GUI
    यदि (gui.in_use)
    {
	--allow_कुंजियाँ;
	यदि (no_Reduce_कुंजियाँ)
	    --no_reduce_कुंजियाँ;
    }
#endif
    यदि (nc)
    {
	vungetc(nc);
	// A character typed with i_CTRL-V_digit cannot have modifiers.
	mod_mask = 0;
    }
    got_int = FALSE;	    // CTRL-C typed after CTRL-V is not an interrupt
    प्रत्यागम cc;
}

/*
 * Insert character, taking care of special keys and mod_mask
 */
    स्थिर शून्य
जोड़ो_special(
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
#यदि defined(MACOS_X) || defined(FEAT_GUI_GTK)
    // Command-key never produces a normal key
    यदि (mod_mask & MOD_MASK_CMD)
	allow_modmask = TRUE;
#endif
    यदि (IS_SPECIAL(c) || (mod_mask && allow_modmask))
    {
	p = get_special_कुंजी_name(c, mod_mask);
	len = (पूर्णाङ्क)STRLEN(p);
	c = p[len - 1];
	यदि (len > 2)
	{
	    यदि (stop_arrow() == FAIL)
		प्रत्यागम;
	    p[len - 1] = NUL;
	    ins_str(p, len - 1);
	    AppendToRedobuffLit(p, -1);
	    ctrlv = FALSE;
	}
    }
    यदि (stop_arrow() == OK)
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
    यदि (textwidth > 0
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
#यदि defined(FEAT_EVAL)
	पूर्णाङ्क     do_internal = TRUE;
	colnr_T virtcol = get_nolist_virtcol()
				  + char2cells(c != NUL ? c : gchar_cursor());

	यदि (*curbuf->b_p_fex != NUL && (flags & INSCHAR_NO_FEX) == 0
		&& (force_format || virtcol > (colnr_T)textwidth))
	{
	    do_internal = (fex_format(curwin->w_cursor.lnum, 1L, c) != 0);
	    // It may be required to save for undo again, e.g. when setline()
	    // was called.
	    ins_need_undo = TRUE;
	}
	यदि (do_internal)
#endif
	    internal_format(textwidth, second_indent, flags, c == NUL, c);
    }

    यदि (c == NUL)	    // only formatting was wanted
	प्रत्यागम;

    // Check whether this character should end a comment.
    यदि (did_ai && c == end_comment_pending)
    {
	char_u  *पंक्ति;
	char_u	lead_end[COM_MAX_LEN];	    // end-comment string
	पूर्णाङ्क	मध्य_len, end_len;
	पूर्णाङ्क	i;

	/*
	 * Need to remove existing (middle) comment leader and insert end
	 * comment leader.  First, check what comment leader we can find.
	 */
	i = get_leader_len(पंक्ति = ml_get_curline(), &p, FALSE, TRUE);
	यदि (i > 0 && vim_strchr(p, COM_मध्य) != रिक्त)	// Just checking
	{
	    // Skip middle-comment string
	    यावत् (*p && p[-1] != ':')	// find end of middle flags
		++p;
	    मध्य_len = copy_option_part(&p, lead_end, COM_MAX_LEN, ",");
	    // Don't count trailing white space for middle_len
	    यावत् (मध्य_len > 0 && VIM_ISWHITE(lead_end[मध्य_len - 1]))
		--मध्य_len;

	    // Find the end-comment string
	    यावत् (*p && p[-1] != ':')	// find end of end flags
		++p;
	    end_len = copy_option_part(&p, lead_end, COM_MAX_LEN, ",");

	    // Skip white space before the cursor
	    i = curwin->w_cursor.col;
	    यावत् (--i >= 0 && VIM_ISWHITE(पंक्ति[i]))
		;
	    i++;

	    // Skip to before the middle leader
	    i -= मध्य_len;

	    // Check some expected things before we go on
	    यदि (i >= 0 && end_len > 0 && lead_end[end_len - 1] == end_comment_pending)
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

    यदि (       !ISSPECIAL(c)
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
#define निवेश_BUFLEN 100
	char_u		buf[निवेश_BUFLEN + 1];
	पूर्णाङ्क		i;
	colnr_T		virtcol = 0;

	buf[0] = c;
	i = 1;
	यदि (textwidth > 0)
	    virtcol = get_nolist_virtcol();
	/*
	 * Stop the string when:
	 * - no more chars available
	 * - finding a special character (command key)
	 * - buffer is full
	 * - running into the 'textwidth' boundary
	 * - need to check for abbreviation: A non-word char after a word-char
	 */
	यावत् (	   (c = vpeekc()) != NUL
		&& !ISSPECIAL(c)
		&& (!has_mbyte || MB_BYTE2LEN_CHECK(c) == 1)
		&& i < निवेश_BUFLEN
		&& (textwidth == 0
		    || (virtcol += byte2cells(buf[i - 1])) < (colnr_T)textwidth)
		&& !(!no_abbr && !vim_iswordc(c) && vim_iswordc(buf[i - 1])))
	{
#ifdef FEAT_RIGHTLEFT
	    c = vgetc();
	    यदि (p_hkmap && कुंजी_Typed)
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
	यदि (flags & INSCHAR_CTRLV)
	{
	    redo_literal(*buf);
	    i = 1;
	}
	अन्यथा
	    i = 0;
	यदि (buf[i] != NUL)
	    AppendToRedobuffLit(buf + i, -1);
    }
    अन्यथा
    {
	पूर्णाङ्क		cc;

	यदि (has_mbyte && (cc = (*mb_char2len)(c)) > 1)
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
	    यदि (flags & INSCHAR_CTRLV)
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
redo_literal(पूर्णाङ्क c)
{
    char_u	buf[10];

    // Only digits need special treatment.  Translate them into a string of
    // three digits.
    यदि (VIM_ISDIGIT(c))
    {
	vim_snprintf((वर्ण *)buf, परिमाण(buf), "%03d", c);
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
    pos_T    *end_जोड़ो_pos)		// can be NULL
{
    start_arrow_common(end_जोड़ो_pos, TRUE);
}

/*
 * Like start_arrow() but with end_change argument.
 * Will prepare for redo of CTRL-G U if "end_change" is FALSE.
 */
    स्थिर शून्य
start_arrow_with_change(
    pos_T    *end_जोड़ो_pos,		// can be NULL
    पूर्णाङ्क	      end_change)		// end undoable change
{
    start_arrow_common(end_जोड़ो_pos, end_change);
    यदि (!end_change)
    {
	AppendCharToRedobuff(Ctrl_G);
	AppendCharToRedobuff('U');
    }
}

    स्थिर शून्य
start_arrow_common(
    pos_T    *end_जोड़ो_pos,		// can be NULL
    पूर्णाङ्क	      end_change)		// end undoable change
{
    यदि (!arrow_used && end_change)	// something has been inserted
    {
	AppendToRedobuff(ESC_STR);
	stop_जोड़ो(end_जोड़ो_pos, FALSE, FALSE);
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
    यदि (spell_redraw_lnum != 0)
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
    यदि (arrow_used)
    {
	Insstart = curwin->w_cursor;	// new insertion starts here
	यदि (Insstart.col > Insstart_orig.col && !ins_need_undo)
	    // Don't update the original insert position when moved to the
	    // right, except when nothing was inserted yet.
	    update_Insstart_orig = FALSE;
	Insstart_textlen = (colnr_T)linetabsize_str(ml_get_curline());

	यदि (u_save_cursor() == OK)
	{
	    arrow_used = FALSE;
	    ins_need_undo = FALSE;
	}

	ai_col = 0;
	यदि (State & VREPLACE_FLAG)
	{
	    orig_पंक्ति_गिनती = curbuf->b_ml.ml_पंक्ति_गिनती;
	    vr_पंक्तियाँ_changed = 1;
	}
	ResetRedobuff();
	AppendToRedobuff((char_u *)"1i");   // pretend we start an insertion
	new_जोड़ो_skip = 2;
    }
    अन्यथा यदि (ins_need_undo)
    {
	यदि (u_save_cursor() == OK)
	{
	    // A command or event may have moved the cursor before the next
	    // edit. Pull Insstart back only when the cursor moved above it,
	    // so that later edits can properly decide whether an extra undo
	    // entry is needed. Advancing Insstart would mis-place '[ after a
	    // register paste.
	    यदि (LT_POS(curwin->w_cursor, Insstart))
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

    प्रत्यागम (arrow_used || ins_need_undo ? FAIL : OK);
}

/*
 * Do a few things to stop inserting.
 * "end_insert_pos" is where insert ended.  It is NULL when we already jumped
 * to another window/buffer.
 */
    स्थिर शून्य
stop_जोड़ो(
    pos_T	*end_जोड़ो_pos,
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
    पूर्णाङ्क added = inserted.string == रिक्त ? 0 : (पूर्णाङ्क)inserted.length - new_जोड़ो_skip;
    यदि (did_restart_edit == 0 || added > 0)
    {
	vim_free(last_जोड़ो.string);
	last_जोड़ो = inserted;			    // structure copy
	last_जोड़ो_skip = added < 0 ? 0 : new_जोड़ो_skip;
    }
    अन्यथा
	vim_free(inserted.string);

    यदि (!arrow_used && end_जोड़ो_pos != रिक्त)
    {
	// Auto-format now.  It may seem strange to do this when stopping an
	// insertion (or moving the cursor), but it's required when appending
	// a line and having it end in a space.  But only do it when something
	// was actually inserted, otherwise undo won't work.
	यदि (!ins_need_undo && has_format_option(FO_AUTO))
	{
	    pos_T   tpos = curwin->w_cursor;

	    // When the cursor is at the end of the line after a space the
	    // formatting will move it to the following word.  Avoid that by
	    // moving the cursor onto the space.
	    cc = 'x';
	    यदि (curwin->w_cursor.col > 0 && gchar_cursor() == NUL)
	    {
		dec_cursor();
		cc = gchar_cursor();
		यदि (!VIM_ISWHITE(cc))
		    curwin->w_cursor = tpos;
	    }

	    auto_format(TRUE, FALSE);

	    यदि (VIM_ISWHITE(cc))
	    {
		यदि (gchar_cursor() != NUL)
		    inc_cursor();
		// If the cursor is still at the same character, also keep
		// the "coladd".
		यदि (gchar_cursor() == NUL
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
	यदि (!nomove && did_ai && (esc || (vim_strchr(p_cpo, CPO_INDENT) == रिक्त
			&& curwin->w_cursor.lnum != end_जोड़ो_pos->lnum))
		&& end_जोड़ो_pos->lnum <= curbuf->b_ml.ml_पंक्ति_गिनती)
	{
	    pos_T	tpos = curwin->w_cursor;
	    colnr_T	prev_col = end_जोड़ो_pos->col;
	    colnr_T	strip_col;

	    curwin->w_cursor = *end_जोड़ो_pos;
	    check_cursor_col();  // make sure it is not past the line

	    // Where the loop would actually start (back up if on NUL).
	    strip_col = curwin->w_cursor.col;
	    यदि (gchar_cursor() == NUL && strip_col > 0)
		--strip_col;

	    // Don't strip if non-whitespace follows: setline() from a <Cmd>
	    // mapping or CursorHoldI autocmd may have inserted content.
	    यदि (*skipwhite(ml_get_curline() + strip_col) == NUL)
	    {
		curwin->w_cursor.col = strip_col;
		प्रति (;;)
		{
		    यदि (gchar_cursor() == NUL && curwin->w_cursor.col > 0)
			--curwin->w_cursor.col;
		    cc = gchar_cursor();
		    यदि (!VIM_ISWHITE(cc))
			भङ्ग;
		    यदि (del_char(TRUE) == FAIL)
			भङ्ग;  // should not happen
		}
		यदि (curwin->w_cursor.lnum != tpos.lnum)
		    curwin->w_cursor = tpos;
		अन्यथा यदि (curwin->w_cursor.col < prev_col)
		{
		    // reset tpos, could have been invalidated in the loop above
		    tpos = curwin->w_cursor;
		    tpos.col++;
		    यदि (cc != NUL && gchar_pos(&tpos) == NUL)
			++curwin->w_cursor.col;	// put cursor back on the NUL
		}

		// <C-S-Right> may have started Visual mode, adjust the position for
		// deleted characters.
		यदि (VIsual_active)
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
    यदि (end_जोड़ो_pos != रिक्त)
    {
	curbuf->b_op_start = Insstart;
	curbuf->b_op_start_orig = Insstart_orig;
	curbuf->b_op_end = *end_जोड़ो_pos;
    }
}

/*
 * Set the last inserted text to a single character.
 * Used for the replace command.
 */
    शून्य
set_last_जोड़ो(पूर्णाङ्क c)
{
    char_u	*s;

    vim_free(last_जोड़ो.string);
    last_जोड़ो.string = alloc(MB_MAXBYTES * 3 + 5);
    यदि (last_जोड़ो.string == रिक्त)
    {
	last_जोड़ो.length = 0;
	प्रत्यागम;
    }

    s = last_जोड़ो.string;
    // Use the CTRL-V only when entering a special char
    यदि (c < ' ' || c == DEL)
	*s++ = Ctrl_V;
    s = add_char2buf(c, s);
    *s++ = ESC;
    *s = NUL;
    last_जोड़ो.length = (size_t)(s - last_जोड़ो.string);
    last_जोड़ो_skip = 0;
}

#यदि defined(EXITFREE)
    शून्य
free_last_जोड़ो(शून्य)
{
    VIM_CLEAR_STRING(last_जोड़ो);
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
    प्रति (i = 0; i < len; ++i)
    {
	c = temp[i];
	// Need to escape K_SPECIAL and CSI like in the typeahead buffer.
	यदि (c == K_SPECIAL)
	{
	    *s++ = K_SPECIAL;
	    *s++ = KS_SPECIAL;
	    *s++ = KE_FILLER;
	}
#ifdef FEAT_GUI
	अन्यथा यदि (c == CSI)
	{
	    *s++ = CSI;
	    *s++ = KS_EXTRA;
	    *s++ = (पूर्णाङ्क)KE_CSI;
	}
#endif
	अन्यथा
	    *s++ = c;
    }
    प्रत्यागम s;
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
    यदि ((flags & BL_SOL) && !p_sol)
	coladvance(curwin->w_curswant);
    अन्यथा
    {
	curwin->w_cursor.col = 0;
	curwin->w_cursor.coladd = 0;

	यदि (flags & (BL_WHITE | BL_SOL))
	{
	    char_u  *ptr;

	    प्रति (ptr = ml_get_curline(); VIM_ISWHITE(*ptr)
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

    यदि (virtual_active())
    {
	pos_T	prevpos = curwin->w_cursor;

	// Adjust for multi-wide char (excluding TAB)
	ptr = ml_get_cursor();
	coladvance(getviscol() + ((*ptr != TAB
					  && vim_isprintc((*mb_ptr2char)(ptr)))
		    ? ptr2cells(ptr) : 1));
	curwin->w_set_curswant = true;
	// Return OK if the cursor moved, FAIL otherwise (at window edge).
	प्रत्यागम (prevpos.col != curwin->w_cursor.col
		    || prevpos.coladd != curwin->w_cursor.coladd) ? OK : FAIL;
    }

    ptr = ml_get_cursor();
    यदि (*ptr == NUL)
	प्रत्यागम FAIL;	    // already at the very end

    यदि (has_mbyte)
	l = (*mb_ptr2len)(ptr);
    अन्यथा
	l = 1;

    // move "l" bytes right, but don't end up on the NUL, unless 'virtualedit'
    // contains "onemore".
    यदि (ptr[l] == NUL && (get_ve_flags() & VE_ONEMORE) == 0)
	प्रत्यागम FAIL;
    curwin->w_cursor.col += l;

    curwin->w_set_curswant = true;
    adjust_skipcol();
    प्रत्यागम OK;
}

    पूर्णाङ्क
oneleft(शून्य)
{
    यदि (virtual_active())
    {
#ifdef FEAT_LINEBREAK
	पूर्णाङ्क width;
#endif
	पूर्णाङ्क v = getviscol();

	यदि (v == 0)
	    प्रत्यागम FAIL;

#ifdef FEAT_LINEBREAK
	// We might get stuck on 'showbreak', skip over it.
	width = 1;
	प्रति (;;)
	{
	    coladvance(v - width);
	    // getviscol() is slow, skip it when 'showbreak' is empty,
	    // 'breakindent' is not set and there are no multi-byte
	    // characters
	    यदि ((*get_showbreak_मूल्य(curwin) == NUL && !curwin->w_p_bri
					     && !has_mbyte) || getviscol() < v)
		भङ्ग;
	    ++width;
	}
#अन्यथा
	coladvance(v - 1);
#endif

	यदि (curwin->w_cursor.coladd == 1)
	{
	    char_u *ptr;

	    // Adjust for multi-wide char (not a TAB)
	    ptr = ml_get_cursor();
	    यदि (*ptr != TAB && vim_isprintc((*mb_ptr2char)(ptr))
							 && ptr2cells(ptr) > 1)
		curwin->w_cursor.coladd = 0;
	}

	curwin->w_set_curswant = true;
	adjust_skipcol();
	प्रत्यागम OK;
    }

    यदि (curwin->w_cursor.col == 0)
	प्रत्यागम FAIL;

    curwin->w_set_curswant = true;
    --curwin->w_cursor.col;

    // if the character on the left of the current cursor is a multi-byte
    // character, move to its first byte
    यदि (has_mbyte)
	mb_adjust_cursor();
    adjust_skipcol();
    प्रत्यागम OK;
}

/*
 * Move the cursor up "n" lines in window "wp".
 * Takes care of closed folds.
 */
    शून्य
cursor_up_inner(win_T *wp, long n)
{
    linenr_T	lnum = wp->w_cursor.lnum;

    यदि (n >= lnum)
	lnum = 1;
    अन्यथा
#ifdef FEAT_FOLDING
	यदि (hasAnyFolding(wp))
    {
	/*
	 * Count each sequence of folded lines as one logical line.
	 */
	// go to the start of the current fold
	(शून्य)hasFoldingWin(wp, lnum, &lnum, रिक्त, TRUE, रिक्त);

	यावत् (n--)
	{
	    // move up one line
	    --lnum;
	    यदि (lnum <= 1)
		भङ्ग;
	    // If we entered a fold, move to the beginning, unless in
	    // Insert mode or when 'foldopen' contains "all": it will open
	    // in a moment.
	    यदि (n > 0 || !((State & MODE_जोड़ो) || (fdo_flags & FDO_ALL)))
		(शून्य)hasFoldingWin(wp, lnum, &lnum, रिक्त, TRUE, रिक्त);
	}
	यदि (lnum < 1)
	    lnum = 1;
    }
    अन्यथा
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
    यदि (n > 0 && (lnum <= 1
		       || (n >= lnum && vim_strchr(p_cpo, CPO_MINUS) != रिक्त)))
	प्रत्यागम FAIL;
    cursor_up_inner(curwin, n);

    // try to advance to the column we want to be at
    coladvance(curwin->w_curswant);

    यदि (upd_topline)
	update_topline();	// make sure curwin->w_topline is valid

    प्रत्यागम OK;
}

/*
 * Move the cursor down "n" lines in window "wp".
 * Takes care of closed folds.
 */
    शून्य
cursor_down_inner(win_T *wp, long n)
{
    linenr_T	lnum = wp->w_cursor.lnum;
    linenr_T	पंक्ति_गिनती = wp->w_buffer->b_ml.ml_पंक्ति_गिनती;

    यदि (lnum + n >= पंक्ति_गिनती)
	lnum = पंक्ति_गिनती;
    अन्यथा
#ifdef FEAT_FOLDING
	यदि (hasAnyFolding(wp))
    {
	linenr_T	last;

	// count each sequence of folded lines as one logical line
	यावत् (n--)
	{
	    यदि (hasFoldingWin(wp, lnum, रिक्त, &last, TRUE, रिक्त))
		lnum = last + 1;
	    अन्यथा
		++lnum;
	    यदि (lnum >= पंक्ति_गिनती)
		भङ्ग;
	}
	यदि (lnum > पंक्ति_गिनती)
	    lnum = पंक्ति_गिनती;
    }
    अन्यथा
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
    linenr_T	पंक्ति_गिनती = curwin->w_buffer->b_ml.ml_पंक्ति_गिनती;
    // This fails if the cursor is already in the last (folded) line, or would
    // move beyond the last line and '-' is in 'cpoptions'.
#ifdef FEAT_FOLDING
    hasFoldingWin(curwin, lnum, रिक्त, &lnum, TRUE, रिक्त);
#endif
    यदि (n > 0
	    && (lnum >= पंक्ति_गिनती
		|| (lnum + n > पंक्ति_गिनती
				     && vim_strchr(p_cpo, CPO_MINUS) != रिक्त)))
	प्रत्यागम FAIL;
    cursor_down_inner(curwin, n);

    // try to advance to the column we want to be at
    coladvance(curwin->w_curswant);

    यदि (upd_topline)
	update_topline();	// make sure curwin->w_topline is valid

    प्रत्यागम OK;
}

/*
 * Stuff the last inserted text in the read buffer.
 * Last_insert actually is a copy of the redo buffer, so we
 * first have to remove the command.
 */
    पूर्णाङ्क
stuff_inserted(
    पूर्णाङ्क	    c,		// Command character to be inserted
    long    गिनती,	// Repeat this many times
    पूर्णाङ्क	    no_esc)	// Don't add an ESC at the end
{
    string_T	जोड़ो;				// text to be inserted
    char_u	last = ' ';

    जोड़ो = get_last_जोड़ो();
    यदि (जोड़ो.string == रिक्त)
    {
	emsg(_(e_no_inserted_text_yet));
	प्रत्यागम FAIL;
    }

    // may want to stuff the command character, to start Insert mode
    यदि (c != NUL)
	stuffcharReadbuff(c);

    यदि (जोड़ो.length > 0)
    {
	char_u	*p;

	// look for the last ESC in 'insert'
	प्रति (p = जोड़ो.string + जोड़ो.length - 1; p >= जोड़ो.string; --p)
	{
	    यदि (*p == ESC)
	    {
		जोड़ो.length = (size_t)(p - जोड़ो.string);
		भङ्ग;
	    }
	}
    }

    यदि (जोड़ो.length > 0)
    {
	char_u	*p = जोड़ो.string + जोड़ो.length - 1;

	// when the last char is either "0" or "^" it will be quoted if no ESC
	// comes after it OR if it will insert more than once and "ptr"
	// starts with ^D.	-- Acevedo
	यदि ((*p == '0' || *p == '^')
		&& (no_esc || (*जोड़ो.string == Ctrl_D && गिनती > 1)))
	{
	    last = *p;
	    --जोड़ो.length;
	}
    }

    कुरु
    {
	stuffReadbuffLen(जोड़ो.string, (long)जोड़ो.length);
	// a trailing "0" is inserted as "<C-V>048", "^" as "<C-V>^"
	विकल्प (last)
	{
	पक्ष '0':
#define TEXT_TO_जोड़ो "\026\060\064\070"
	    stuffReadbuffLen((char_u *)TEXT_TO_जोड़ो, STRLEN_LITERAL(TEXT_TO_जोड़ो));
#undef TEXT_TO_जोड़ो
	    भङ्ग;

	पक्ष '^':
#define TEXT_TO_जोड़ो "\026^"
	    stuffReadbuffLen((char_u *)TEXT_TO_जोड़ो, STRLEN_LITERAL(TEXT_TO_जोड़ो));
#undef TEXT_TO_जोड़ो
	    भङ्ग;

	स्वभाव:
	    भङ्ग;
	}
    }
    यावत् (--गिनती > 0);

    // may want to stuff a trailing ESC, to get out of Insert mode
    यदि (!no_esc)
	stuffcharReadbuff(ESC);

    प्रत्यागम OK;
}

    string_T
get_last_जोड़ो(शून्य)
{
    string_T जोड़ो = {रिक्त, 0};

    यदि (last_जोड़ो.string != रिक्त)
    {
	जोड़ो.string = last_जोड़ो.string + last_जोड़ो_skip;
	जोड़ो.length = (size_t)(last_जोड़ो.length - last_जोड़ो_skip);
    }

    प्रत्यागम जोड़ो;
}

/*
 * Get last inserted string, and remove trailing <Esc>.
 * Returns pointer to allocated memory (must be freed) or NULL.
 */
    char_u *
get_last_जोड़ो_save(शून्य)
{
    string_T	जोड़ो = get_last_जोड़ो();
    char_u	*s;

    यदि (जोड़ो.string == रिक्त)
	प्रत्यागम रिक्त;
    s = vim_strnsave(जोड़ो.string, जोड़ो.length);
    यदि (s == रिक्त)
	प्रत्यागम रिक्त;

    यदि (जोड़ो.length > 0 && s[जोड़ो.length - 1] == ESC)	// remove trailing ESC
	s[--जोड़ो.length] = NUL;
    प्रत्यागम s;
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
    यदि (p_paste || no_abbr || arrow_used)
	प्रत्यागम FALSE;

    प्रत्यागम check_abbr(c, ml_get_curline(), curwin->w_cursor.col,
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

स्थिर char_u	*replace_ढेर = रिक्त;
स्थिर long	replace_ढेर_nr = 0;	    // next entry in replace stack
स्थिर long	replace_ढेर_len = 0;	    // max. number of entries

    शून्य
replace_धकेलो(
    पूर्णाङ्क	    c)	    // character that is replaced (NUL is none)
{
    char_u  *p;

    यदि (replace_ढेर_nr < replace_offset)	// nothing to do
	प्रत्यागम;
    यदि (replace_ढेर_len <= replace_ढेर_nr)
    {
	replace_ढेर_len += 50;
	p = ALLOC_MULT(char_u, replace_ढेर_len);
	यदि (p == रिक्त)	    // out of memory
	{
	    replace_ढेर_len -= 50;
	    प्रत्यागम;
	}
	यदि (replace_ढेर != रिक्त)
	{
	    mch_memmove(p, replace_ढेर,
				 (size_t)(replace_ढेर_nr * परिमाण(char_u)));
	    vim_free(replace_ढेर);
	}
	replace_ढेर = p;
    }
    p = replace_ढेर + replace_ढेर_nr - replace_offset;
    यदि (replace_offset)
	mch_memmove(p + 1, p, (size_t)(replace_offset * परिमाण(char_u)));
    *p = c;
    ++replace_ढेर_nr;
}

/*
 * Push a character onto the replace stack.  Handles a multi-byte character in
 * reverse byte order, so that the first byte is popped off first.
 * Return the number of bytes done (includes composing characters).
 */
    पूर्णाङ्क
replace_धकेलो_mb(char_u *p)
{
    पूर्णाङ्क l = (*mb_ptr2len)(p);
    पूर्णाङ्क j;

    प्रति (j = l - 1; j >= 0; --j)
	replace_धकेलो(p[j]);
    प्रत्यागम l;
}

/*
 * Pop one item from the replace stack.
 * return -1 if stack empty
 * return replaced character or NUL otherwise
 */
    स्थिर पूर्णाङ्क
replace_निकालो(शून्य)
{
    यदि (replace_ढेर_nr == 0)
	प्रत्यागम -1;
    प्रत्यागम (पूर्णाङ्क)replace_ढेर[--replace_ढेर_nr];
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

    प्रति (i = replace_ढेर_nr; --i >= 0; )
	यदि (replace_ढेर[i] == NUL && off-- <= 0)
	{
	    --replace_ढेर_nr;
	    mch_memmove(replace_ढेर + i, replace_ढेर + i + 1,
					      (size_t)(replace_ढेर_nr - i));
	    प्रत्यागम;
	}
}

/*
 * Pop bytes from the replace stack until a NUL is found, and insert them
 * before the cursor.  Can only be used in MODE_REPLACE or MODE_VREPLACE state.
 */
    स्थिर शून्य
replace_निकालो_ins(शून्य)
{
    पूर्णाङ्क	    cc;
    पूर्णाङ्क	    oldState = State;

    State = MODE_NORMAL;			// don't want MODE_REPLACE here
    यावत् ((cc = replace_निकालो()) > 0)
    {
	mb_replace_निकालो_ins(cc);
	dec_cursor();
    }
    State = oldState;
}

/*
 * Insert bytes popped from the replace stack. "cc" is the first byte.  If it
 * indicates a multi-byte char, pop the other bytes too.
 */
    स्थिर शून्य
mb_replace_निकालो_ins(पूर्णाङ्क cc)
{
    पूर्णाङ्क		n;
    char_u	buf[MB_MAXBYTES + 1];
    पूर्णाङ्क		i;
    पूर्णाङ्क		c;

    यदि (has_mbyte && (n = MB_BYTE2LEN(cc)) > 1)
    {
	buf[0] = cc;
	प्रति (i = 1; i < n; ++i)
	    buf[i] = replace_निकालो();
	ins_bytes_len(buf, n);
    }
    अन्यथा
	ins_char(cc);

    यदि (enc_utf8)
	// Handle composing chars.
	प्रति (;;)
	{
	    c = replace_निकालो();
	    यदि (c == -1)	    // stack empty
		भङ्ग;
	    यदि ((n = MB_BYTE2LEN(c)) == 1)
	    {
		// Not a multi-byte char, put it back.
		replace_धकेलो(c);
		भङ्ग;
	    }

	    buf[0] = c;
	    प्रति (i = 1; i < n; ++i)
		buf[i] = replace_निकालो();
	    यदि (utf_iscomposing(utf_ptr2char(buf)))
		ins_bytes_len(buf, n);
	    अन्यथा
	    {
		// Not a composing char, put it back.
		प्रति (i = n - 1; i >= 0; --i)
		    replace_धकेलो(buf[i]);
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
    VIM_CLEAR(replace_ढेर);
    replace_ढेर_len = 0;
    replace_ढेर_nr = 0;
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

    cc = replace_निकालो();
    यदि (cc > 0)
    {
#ifdef FEAT_PROP_POPUP
	size_t	len_before = 0;  // init to shut up GCC

	यदि (curbuf->b_has_textprop)
	{
	    // Do not adjust text properties for individual delete and insert
	    // operations, do it afterwards on the resulting text.
	    len_before = ml_get_curline_len();
	    ++text_prop_frozen;
	}
#endif
	यदि (State & VREPLACE_FLAG)
	{
	    // Get the number of screen cells used by the character we are
	    // going to delete.
	    getvcol(curwin, &curwin->w_cursor, रिक्त, &start_vcol, रिक्त, 0);
	    orig_vcols = chartabsize(ml_get_cursor(), start_vcol);
	}
	यदि (has_mbyte)
	{
	    (शून्य)del_char_after_col(limit_col);
	    यदि (State & VREPLACE_FLAG)
		orig_len = ml_get_cursor_len();
	    replace_धकेलो(cc);
	}
	अन्यथा
	{
	    pchar_cursor(cc);
	    यदि (State & VREPLACE_FLAG)
		orig_len = ml_get_cursor_len() - 1;
	}
	replace_निकालो_ins();

	यदि (State & VREPLACE_FLAG)
	{
	    // Get the number of screen cells used by the inserted characters
	    p = ml_get_cursor();
	    ins_len = ml_get_cursor_len() - orig_len;
	    vcol = start_vcol;
	    प्रति (i = 0; i < ins_len; ++i)
	    {
		vcol += chartabsize(p + i, vcol);
		i += (*mb_ptr2len)(p + i) - 1;
	    }
	    vcol -= start_vcol;

	    // Delete spaces that were inserted after the cursor to keep the
	    // text aligned.
	    curwin->w_cursor.col += ins_len;
	    यावत् (vcol > orig_vcols && gchar_cursor() == ' ')
	    {
		del_char(FALSE);
		++orig_vcols;
	    }
	    curwin->w_cursor.col -= ins_len;
	}

	// mark the buffer as changed and prepare for displaying
	changed_bytes(curwin->w_cursor.lnum, curwin->w_cursor.col);

#ifdef FEAT_PROP_POPUP
	यदि (curbuf->b_has_textprop)
	{
	    size_t len_now = ml_get_curline_len();

	    --text_prop_frozen;
	    adjust_prop_columns(curwin->w_cursor.lnum, curwin->w_cursor.col,
					   (पूर्णाङ्क)(len_now - len_before), 0);
	}
#endif
    }
    अन्यथा यदि (cc == 0)
	(शून्य)del_char_after_col(limit_col);
}

#यदि defined(FEAT_RIGHTLEFT)
/*
 * Map Hebrew keyboard when in hkmap mode.
 */
    पूर्णाङ्क
hkmap(पूर्णाङ्क c)
{
    यदि (p_hkmapp)   // phonetic mapping, by Ilya Dogolazky
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

	यदि (c == 'N' || c == 'M' || c == 'P' || c == 'C' || c == 'Z')
	    प्रत्यागम (पूर्णाङ्क)(map[CharOrd(c)] - 1 + p_aleph);
							    // '-1'='sofit'
	अन्यथा यदि (c == 'x')
	    प्रत्यागम 'X';
	अन्यथा यदि (c == 'q')
	    प्रत्यागम '\''; // {geresh}={'}
	अन्यथा यदि (c == 246)
	    प्रत्यागम ' ';  // \"o --> ' ' for a german keyboard
	अन्यथा यदि (c == 228)
	    प्रत्यागम ' ';  // \"a --> ' '      -- / --
	अन्यथा यदि (c == 252)
	    प्रत्यागम ' ';  // \"u --> ' '      -- / --
	// NOTE: islower() does not do the right thing for us on Linux so we
	// do this the same was as 5.7 and previous, so it works correctly on
	// all systems.  Specifically, the e.g. Delete and Arrow keys are
	// munged and won't work if e.g. searching for Hebrew text.
	अन्यथा यदि (c >= 'a' && c <= 'z')
	    प्रत्यागम (पूर्णाङ्क)(map[Char_Ord_निचला(c)] + p_aleph);
	अन्यथा
	    प्रत्यागम c;
    }
    अन्यथा
    {
	विकल्प (c)
	{
	    पक्ष '`':	प्रत्यागम ';';
	    पक्ष '/':	प्रत्यागम '.';
	    पक्ष '\'':	प्रत्यागम ',';
	    पक्ष 'q':	प्रत्यागम '/';
	    पक्ष 'w':	प्रत्यागम '\'';

			// Hebrew letters - set offset from 'a'
	    पक्ष ',':	c = '{'; भङ्ग;
	    पक्ष '.':	c = 'v'; भङ्ग;
	    पक्ष ';':	c = 't'; भङ्ग;
	    स्वभाव: {
			 स्थिर वर्ण str[] = "zqbcxlsjphmkwonu ydafe rig";

			 यदि (c < 'a' || c > 'z')
			     प्रत्यागम c;
			 c = str[Char_Ord_निचला(c)];
			 भङ्ग;
		     }
	}

	प्रत्यागम (पूर्णाङ्क)(Char_Ord_निचला(c) + p_aleph);
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
    यदि (redrawing() && !char_avail())
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
    ++allow_कुंजियाँ;
    regname = plain_vgetc();
    LANGMAP_ADJUST(regname, TRUE);
    यदि (regname == Ctrl_R || regname == Ctrl_O || regname == Ctrl_P)
    {
	// Get a third key for literal register insertion
	literally = regname;
	add_to_showcmd_c(literally);
	regname = plain_vgetc();
	LANGMAP_ADJUST(regname, TRUE);
    }
    --no_mapping;
    --allow_कुंजियाँ;

#ifdef FEAT_EVAL
    // Don't call u_sync() while typing the expression or giving an error
    // message for it. Only call it explicitly.
    ++no_u_sync;
    यदि (regname == '=')
    {
	pos_T	curpos = curwin->w_cursor;
# ifdef HAVE_निवेश_METHOD
	पूर्णाङ्क	im_on = im_get_status();
# endif
	// Sync undo when evaluating the expression calls setline() or
	// append(), so that it can be undone separately.
	u_sync_once = 2;

	regname = get_व्यंजक_register();

	// Cursor may be moved back a column.
	curwin->w_cursor = curpos;
	check_cursor();
# ifdef HAVE_निवेश_METHOD
	// Restore the Input Method.
	यदि (im_on)
	    im_set_active(TRUE);
# endif
    }
    यदि (regname == NUL || !valid_yank_reg(regname, FALSE))
    {
	vim_beep(BO_REG);
	need_redraw = TRUE;	// remove the '"'
    }
    अन्यथा
    {
#endif
	यदि (literally == Ctrl_O || literally == Ctrl_P)
	{
	    // Append the command to the redo buffer.
	    AppendCharToRedobuff(Ctrl_R);
	    AppendCharToRedobuff(literally);
	    AppendCharToRedobuff(regname);

	    do_put(regname, रिक्त, BACKWARD, 1L,
		 (literally == Ctrl_P ? PUT_FIXINDENT : 0) | PUT_CURSEND);
	}
	अन्यथा यदि (जोड़ो_reg(regname, literally) == FAIL)
	{
	    vim_beep(BO_REG);
	    need_redraw = TRUE;	// remove the '"'
	}
	अन्यथा यदि (stop_जोड़ो_mode)
	    // When the '=' register was used and a function was invoked that
	    // did ":stopinsert" then stuff_empty() returns FALSE but we won't
	    // insert anything, need to remove the '"'
	    need_redraw = TRUE;

#ifdef FEAT_EVAL
    }
    --no_u_sync;
    यदि (u_sync_once == 1)
	ins_need_undo = TRUE;
    u_sync_once = 0;
#endif
    clear_showcmd();

    // If the inserted register is empty, we need to remove the '"'
    यदि (need_redraw || stuff_empty())
	edit_unputchar();

    // Disallow starting Visual mode here, would get a weird mode.
    यदि (!vis_active && VIsual_active)
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
    ++allow_कुंजियाँ;
    c = plain_vgetc();
    --no_mapping;
    --allow_कुंजियाँ;
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
    यदि (map_to_exists_mode((char_u *)"", MODE_LANGMAP, FALSE))
    {
	// ":lmap" mappings exists, Toggle use of ":lmap" mappings.
	यदि (State & MODE_LANGMAP)
	{
	    curbuf->b_p_iminsert = B_IMODE_NONE;
	    State &= ~MODE_LANGMAP;
	}
	अन्यथा
	{
	    curbuf->b_p_iminsert = B_IMODE_LMAP;
	    State |= MODE_LANGMAP;
#ifdef HAVE_निवेश_METHOD
	    im_set_active(FALSE);
#endif
	}
    }
#ifdef HAVE_निवेश_METHOD
    अन्यथा
    {
	// There are no ":lmap" mappings, toggle IM
	यदि (im_get_status())
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
    यदि (gui.in_use)
	gui_update_cursor(TRUE, FALSE);
#endif
#यदि defined(FEAT_KEYMAP)
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
    long	*गिनती,
    पूर्णाङ्क		cmdchar,
    पूर्णाङ्क		nomove)	    // don't move cursor
{
    पूर्णाङ्क		temp;
    स्थिर पूर्णाङ्क	disabled_redraw = FALSE;
#ifdef FEAT_CONCEAL
    // Remember if the cursor line was concealed before changing State.
    पूर्णाङ्क		cursor_पंक्ति_was_concealed = curwin->w_p_cole > 0
						&& conceal_cursor_पंक्ति(curwin);
#endif

#ifdef FEAT_SPELL
    check_spell_redraw();
#endif

    temp = curwin->w_cursor.col;
    यदि (disabled_redraw)
    {
	यदि (RedrawingDisabled > 0)
	    --RedrawingDisabled;
	disabled_redraw = FALSE;
    }
    यदि (!arrow_used)
    {
	/*
	 * Don't append the ESC for "r<CR>" and "grx".
	 * When 'insertmode' is set only CTRL-L stops Insert mode.  Needed for
	 * when "count" is non-zero.
	 */
	यदि (cmdchar != 'r' && cmdchar != 'v')
	    AppendToRedobuff(p_im ? (char_u *)"\014" : ESC_STR);

	/*
	 * Repeating insert may take a long time.  Check for
	 * interrupt now and then.
	 */
	यदि (*गिनती > 0)
	{
	    पंक्ति_breakcheck();
	    यदि (got_int)
		*गिनती = 0;
	}

	यदि (--*गिनती > 0)	// repeat what was typed
	{
	    // Vi repeats the insert without replacing characters.
	    यदि (vim_strchr(p_cpo, CPO_REPLCNT) != रिक्त)
		State &= ~REPLACE_FLAG;

	    (शून्य)start_redo_ins();
	    यदि (cmdchar == 'r' || cmdchar == 'v')
		stuffRedoReadbuff(ESC_STR);	// no ESC in redo buffer
	    ++RedrawingDisabled;
	    disabled_redraw = TRUE;
	    प्रत्यागम FALSE;	// repeat the insert
	}
	stop_जोड़ो(&curwin->w_cursor, TRUE, nomove);
	undisplay_dollar();
    }

    यदि (cmdchar != 'r' && cmdchar != 'v')
	ins_apply_autocmds(EVENT_INSERTLEAVEPRE);

    // When an autoindent was removed, curswant stays after the
    // indent
    यदि (restart_edit == NUL && (colnr_T)temp == curwin->w_cursor.col)
	curwin->w_set_curswant = true;

    // Remember the last Insert position in the '^ mark.
    यदि ((cmdmod.cmod_flags & CMOD_KEEPJUMPS) == 0)
	curbuf->b_last_जोड़ो = curwin->w_cursor;

    /*
     * The cursor should end up on the last inserted character.
     * Don't do it for CTRL-O, unless past the end of the line.
     */
    यदि (!nomove
	    && (curwin->w_cursor.col != 0
		|| curwin->w_cursor.coladd > 0)
	    && (restart_edit == NUL
		   || (gchar_cursor() == NUL && !VIsual_active))
#ifdef FEAT_RIGHTLEFT
	    && !revins_on
#endif
				      )
    {
	यदि (curwin->w_cursor.coladd > 0 || get_ve_flags() == VE_ALL)
	{
	    oneleft();
	    यदि (restart_edit != NUL)
		++curwin->w_cursor.coladd;
	}
	अन्यथा
	{
	    --curwin->w_cursor.col;
	    curwin->w_valid &= ~(VALID_WCOL|VALID_VIRTCOL);
	    // Correct cursor for multi-byte character.
	    यदि (has_mbyte)
		mb_adjust_cursor();
	}
    }

#ifdef HAVE_निवेश_METHOD
    // Disable IM to allow typing English directly for Normal mode commands.
    // When ":lmap" is enabled don't change 'iminsert' (IM can be enabled as
    // well).
    यदि (!(State & MODE_LANGMAP))
	im_save_status(&curbuf->b_p_iminsert);
    im_set_active(FALSE);
#endif

    State = MODE_NORMAL;
    may_trigger_modechanged();
    // need to position cursor again when on a TAB and when on a char with
    // virtual text.
    यदि (gchar_cursor() == TAB
#ifdef FEAT_PROP_POPUP
	    || curbuf->b_has_textprop
#endif
       )
	curwin->w_valid &= ~(VALID_WROW|VALID_WCOL|VALID_VIRTCOL);

    setmouse();
#ifdef CURSOR_SHAPE
    ui_cursor_shape();		// may show different cursor shape
#endif
    यदि (!p_ek)
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
    conceal_check_cursor_पंक्ति(cursor_पंक्ति_was_concealed);
#endif

    // When recording or for CTRL-O, need to display the new mode.
    // Otherwise remove the mode message.
    यदि (reg_recording != 0 || restart_edit != NUL)
	showmode();
    अन्यथा यदि (p_smd && (got_int || !skip_showmode()))
	संदेश("");

    प्रत्यागम TRUE;	    // exit Insert mode
}

#ifdef FEAT_RIGHTLEFT
/*
 * Toggle language: hkmap and revins_on.
 * Move to end of reverse inserted text.
 */
    स्थिर शून्य
ins_ctrl_(शून्य)
{
    यदि (revins_on && revins_chars && revins_scol >= 0)
    {
	यावत् (gchar_cursor() != NUL && revins_chars--)
	    ++curwin->w_cursor.col;
    }
    p_ri = !p_ri;
    revins_on = (State == MODE_जोड़ो && p_ri);
    यदि (revins_on)
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
    स्थिर पूर्णाङ्क
ins_start_select(पूर्णाङ्क c)
{
    यदि (!km_startsel)
	प्रत्यागम FALSE;
    विकल्प (c)
    {
	पक्ष K_KHOME:
	पक्ष K_KEND:
	पक्ष K_PAGEUP:
	पक्ष K_KPAGEUP:
	पक्ष K_PAGEDOWN:
	पक्ष K_KPAGEDOWN:
#ifdef MACOS_X
	पक्ष K_बायाँ:
	पक्ष K_दायाँ:
	पक्ष K_UP:
	पक्ष K_DOWN:
	पक्ष K_END:
	पक्ष K_HOME:
#endif
	    यदि (!(mod_mask & MOD_MASK_SHIFT))
		भङ्ग;
	    // FALLTHROUGH
	पक्ष K_S_बायाँ:
	पक्ष K_S_दायाँ:
	पक्ष K_S_UP:
	पक्ष K_S_DOWN:
	पक्ष K_S_END:
	पक्ष K_S_HOME:
	    // Start selection right away, the cursor can move with CTRL-O when
	    // beyond the end of the line.
	    start_selection();

	    // Execute the key in (insert) Select mode.
	    stuffcharReadbuff(Ctrl_O);
	    यदि (mod_mask)
	    {
		char_u	    buf[4];

		buf[0] = K_SPECIAL;
		buf[1] = KS_MODIFIER;
		buf[2] = mod_mask;
		buf[3] = NUL;
		stuffReadbuffLen(buf, 3L);
	    }
	    stuffcharReadbuff(c);
	    प्रत्यागम TRUE;
    }
    प्रत्यागम FALSE;
}

/*
 * <Insert> key in Insert mode: toggle insert/replace mode.
 */
    स्थिर शून्य
ins_जोड़ो(पूर्णाङ्क replaceState)
{
#ifdef FEAT_EVAL
    set_vim_var_string(VV_INSERTMODE,
		   (char_u *)((State & REPLACE_FLAG) ? "i"
			    : replaceState == MODE_VREPLACE ? "v" : "r"), 1);
#endif
    ins_apply_autocmds(EVENT_INSERTCHANGE);
    यदि (State & REPLACE_FLAG)
	State = MODE_जोड़ो | (State & MODE_LANGMAP);
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
    यदि (State & VREPLACE_FLAG)
	restart_edit = 'V';
    अन्यथा यदि (State & REPLACE_FLAG)
	restart_edit = 'R';
    अन्यथा
	restart_edit = 'I';
    यदि (virtual_active())
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
ins_shift(पूर्णाङ्क c, पूर्णाङ्क lastc)
{
    यदि (stop_arrow() == FAIL)
	प्रत्यागम;
    AppendCharToRedobuff(c);

    /*
     * 0^D and ^^D: remove all indent.
     */
    यदि (c == Ctrl_D && (lastc == '0' || lastc == '^')
						  && curwin->w_cursor.col > 0)
    {
	--curwin->w_cursor.col;
	(शून्य)del_char(FALSE);		// delete the '^' or '0'
	// In Replace mode, restore the characters that '^' or '0' replaced.
	यदि (State & REPLACE_FLAG)
	    replace_निकालो_ins();
	यदि (lastc == '^')
	    old_indent = get_indent();	// remember curr. indent
	change_indent(INDENT_SET, 0, TRUE, 0, TRUE);
    }
    अन्यथा
	change_indent(c == Ctrl_D ? INDENT_DEC : INDENT_INC, 0, TRUE, 0, TRUE);

    यदि (did_ai && *skipwhite(ml_get_curline()) != NUL)
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

    यदि (stop_arrow() == FAIL)
	प्रत्यागम;
    यदि (gchar_cursor() == NUL)		// delete newline
    {
	temp = curwin->w_cursor.col;
	यदि (!can_bs(BS_EOL)		// only if "eol" included
		|| do_join(2, FALSE, TRUE, FALSE, FALSE) == FAIL)
	    vim_beep(BO_BS);
	अन्यथा
	{
	    curwin->w_cursor.col = temp;
	    // Adjust orig_line_count in case more lines have been deleted than
	    // have been added. That makes sure, that open_line() later
	    // can access all buffer lines correctly
	    यदि (State & VREPLACE_FLAG &&
		    orig_पंक्ति_गिनती > curbuf->b_ml.ml_पंक्ति_गिनती)
		orig_पंक्ति_गिनती = curbuf->b_ml.ml_पंक्ति_गिनती;
	}
    }
    अन्यथा यदि (del_char(FALSE) == FAIL)  // delete char under cursor
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
    यदि (State & REPLACE_FLAG)
    {
	// Don't delete characters before the insert point when in
	// Replace mode
	यदि (curwin->w_cursor.lnum != Insstart.lnum
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
    यदि (       BUFEMPTY()
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
	प्रत्यागम FALSE;
    }

    यदि (stop_arrow() == FAIL)
	प्रत्यागम FALSE;
    in_indent = inindent(0);
    यदि (in_indent)
	can_cindent = FALSE;
    end_comment_pending = NUL;	// After BS, don't auto-end comment
#ifdef FEAT_RIGHTLEFT
    यदि (revins_on)	    // put cursor after last inserted char
	inc_cursor();
#endif

    // Virtualedit:
    //	BACKSPACE_CHAR eats a virtual space
    //	BACKSPACE_WORD eats all coladd
    //	BACKSPACE_LINE eats all coladd and keeps going
    यदि (curwin->w_cursor.coladd > 0)
    {
	यदि (mode == BACKSPACE_CHAR)
	{
	    --curwin->w_cursor.coladd;
	    प्रत्यागम TRUE;
	}
	यदि (mode == BACKSPACE_WORD)
	{
	    curwin->w_cursor.coladd = 0;
	    प्रत्यागम TRUE;
	}
	curwin->w_cursor.coladd = 0;
    }

    /*
     * Delete newline!
     */
    यदि (curwin->w_cursor.col == 0)
    {
	lnum = Insstart.lnum;
	यदि (curwin->w_cursor.lnum == lnum
#ifdef FEAT_RIGHTLEFT
			|| revins_on
#endif
				    )
	{
	    यदि (u_save((linenr_T)(curwin->w_cursor.lnum - 2),
			       (linenr_T)(curwin->w_cursor.lnum + 1)) == FAIL)
		प्रत्यागम FALSE;
	    --Insstart.lnum;
	    Insstart.col = ml_get_len(Insstart.lnum);
	}
	/*
	 * In replace mode:
	 * cc < 0: NL was inserted, delete it
	 * cc >= 0: NL was replaced, put original characters back
	 */
	cc = -1;
	यदि (State & REPLACE_FLAG)
	    cc = replace_निकालो();	    // returns -1 if NL was inserted
	/*
	 * In replace mode, in the line we started replacing, we only move the
	 * cursor.
	 */
	यदि ((State & REPLACE_FLAG) && curwin->w_cursor.lnum <= lnum)
	{
	    dec_cursor();
	}
	अन्यथा
	{
	    यदि (!(State & VREPLACE_FLAG)
				   || curwin->w_cursor.lnum > orig_पंक्ति_गिनती)
	    {
		temp = gchar_cursor();	// remember current char
		--curwin->w_cursor.lnum;

		// When "aw" is in 'formatoptions' we must delete the space at
		// the end of the line, otherwise the line will be broken
		// again when auto-formatting.
		यदि (has_format_option(FO_AUTO)
					   && has_format_option(FO_WHITE_PAR))
		{
		    char_u  *ptr = ml_get_buf(curbuf, curwin->w_cursor.lnum,
									FALSE);
		    पूर्णाङ्क	    len = ml_get_curline_len();

		    यदि (len > 0 && ptr[len - 1] == ' ')
		    {
			char_u *newp = alloc(curbuf->b_ml.ml_पंक्ति_len - 1);

			यदि (newp != रिक्त)
			{
			    mch_memmove(newp, ptr, len - 1);
			    newp[len - 1] = NUL;
			    यदि (curbuf->b_ml.ml_पंक्ति_len > len + 1)
				mch_memmove(newp + len, ptr + len + 1,
					   curbuf->b_ml.ml_पंक्ति_len - len - 1);

			    यदि (curbuf->b_ml.ml_flags
					      & (ML_पंक्ति_DIRTY | ML_ALLOCATED))
				vim_free(curbuf->b_ml.ml_पंक्ति_ptr);
			    curbuf->b_ml.ml_पंक्ति_ptr = newp;
			    curbuf->b_ml.ml_पंक्ति_len--;
			    curbuf->b_ml.ml_पंक्ति_textlen--;
			    curbuf->b_ml.ml_flags |= ML_पंक्ति_DIRTY;
			}
		    }
		}

		(शून्य)do_join(2, FALSE, FALSE, FALSE, FALSE);
		यदि (temp == NUL && gchar_cursor() != NUL)
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
	    यदि (State & REPLACE_FLAG)
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
		यावत् (cc > 0)
		{
		    save_col = curwin->w_cursor.col;
		    mb_replace_निकालो_ins(cc);
		    curwin->w_cursor.col = save_col;
		    cc = replace_निकालो();
		}
		// restore the characters that NL replaced
		replace_निकालो_ins();
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
	यदि (revins_on)		// put cursor on last inserted char
	    dec_cursor();
#endif
	mincol = 0;
						// keep indent
	यदि (mode == BACKSPACE_पंक्ति
		&& (curbuf->b_p_ai || cindent_on())
#ifdef FEAT_RIGHTLEFT
		&& !revins_on
#endif
			    )
	{
	    save_col = curwin->w_cursor.col;
	    beginline(BL_WHITE);
	    यदि (curwin->w_cursor.col < save_col)
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
	यदि (	   mode == BACKSPACE_CHAR
		&& ((p_sta && in_indent)
		    || ((get_sts_मूल्य() != 0
#ifdef FEAT_VARTABS
			|| tabstop_गिनती(curbuf->b_p_vsts_array)
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
	    char_u	*पंक्ति;
	    char_u	*ptr;
	    char_u	*cursor_ptr;
	    char_u	*space_ptr;
	    colnr_T	space_vcol = 0;
	    पूर्णाङ्क		prev_space = FALSE;
	    colnr_T	want_col;

	    *inserted_space_p = FALSE;

	    space_ptr = ptr = पंक्ति = ml_get_curline();
	    cursor_ptr = पंक्ति + curwin->w_cursor.col;

	    // Compute virtual column of cursor position, and find the last
	    // whitespace before cursor that is preceded by non-whitespace.
	    // Use chartabsize() so that virtual text and wrapping are ignored.
	    यावत् (ptr < cursor_ptr)
	    {
		पूर्णाङ्क	cur_space = VIM_ISWHITE(*ptr);

		यदि (!prev_space && cur_space)
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
	    यदि (p_sta && in_indent)
		want_vcol -= want_vcol % (पूर्णाङ्क)get_sw_मूल्य(curbuf);
	    अन्यथा
#ifdef FEAT_VARTABS
		want_vcol = tabstop_start(want_vcol, get_sts_मूल्य(),
						       curbuf->b_p_vsts_array);
#अन्यथा
		want_vcol -= want_vcol % (पूर्णाङ्क)get_sts_मूल्य();
#endif

	    // Find the position to stop backspacing.
	    // Use chartabsize() so that virtual text and wrapping are ignored.
	    यावत् (TRUE)
	    {
		पूर्णाङ्क size = chartabsize(space_ptr, space_vcol);

		यदि (space_vcol + size > want_vcol)
		    भङ्ग;
		space_vcol += size;
		MB_PTR_ADV(space_ptr);
	    }
	    want_col = space_ptr - पंक्ति;

	    // Delete characters until we are at or before want_col.
	    यावत् (curwin->w_cursor.col > want_col)
		ins_bs_one();

	    // Insert extra spaces until we are at want_vcol.
	    प्रति (; space_vcol < want_vcol; space_vcol++)
	    {
		// Remember the first char we inserted
		यदि (curwin->w_cursor.lnum == Insstart_orig.lnum
				   && curwin->w_cursor.col < Insstart_orig.col)
		    Insstart_orig.col = curwin->w_cursor.col;

		यदि (State & VREPLACE_FLAG)
		    ins_char(' ');
		अन्यथा
		{
		    ins_str((char_u *)" ", 1);
		    यदि ((State & REPLACE_FLAG))
			replace_धकेलो(NUL);
		}
	    }
	}

	/*
	 * Delete up to starting point, start of line or previous word.
	 */
	अन्यथा
	{
	    पूर्णाङ्क cclass = 0, prev_cclass = 0;

	    यदि (has_mbyte)
		cclass = mb_get_class(ml_get_cursor());
	    कुरु
	    {
#ifdef FEAT_RIGHTLEFT
		यदि (!revins_on) // put cursor on char to be deleted
#endif
		    dec_cursor();

		cc = gchar_cursor();
		// look multi-byte character class
		यदि (has_mbyte)
		{
		    prev_cclass = cclass;
		    cclass = mb_get_class(ml_get_cursor());
		}

		// start of word?
		यदि (mode == BACKSPACE_WORD && !vim_isspace(cc))
		{
		    mode = BACKSPACE_WORD_NOT_SPACE;
		    temp = vim_iswordc(cc);
		}
		// end of word?
		अन्यथा यदि (mode == BACKSPACE_WORD_NOT_SPACE
			&& ((vim_isspace(cc) || vim_iswordc(cc) != temp)
			|| prev_cclass != cclass))
		{
#ifdef FEAT_RIGHTLEFT
		    यदि (!revins_on)
#endif
			inc_cursor();
#ifdef FEAT_RIGHTLEFT
		    अन्यथा यदि (State & REPLACE_FLAG)
			dec_cursor();
#endif
		    भङ्ग;
		}
		यदि (State & REPLACE_FLAG)
		    replace_do_bs(-1);
		अन्यथा
		{
		    यदि (enc_utf8 && p_deco)
			(शून्य)utfc_ptr2char(ml_get_cursor(), cpc);
		    (शून्य)del_char(FALSE);
		    /*
		     * If there are combining characters and 'delcombine' is set
		     * move the cursor back.  Don't back up before the base
		     * character.
		     */
		    यदि (enc_utf8 && p_deco && cpc[0] != NUL)
			inc_cursor();
#ifdef FEAT_RIGHTLEFT
		    यदि (revins_chars)
		    {
			revins_chars--;
			revins_legal++;
		    }
		    यदि (revins_on && gchar_cursor() == NUL)
			भङ्ग;
#endif
		}
		// Just a single backspace?:
		यदि (mode == BACKSPACE_CHAR)
		    भङ्ग;
	    } यावत् (
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
    यदि (curwin->w_cursor.col <= 1)
	did_ai = FALSE;

    यदि (call_fix_indent)
	fix_indent();

    /*
     * It's a little strange to put backspaces into the redo
     * buffer, but it makes auto-indent a lot easier to deal
     * with.
     */
    AppendCharToRedobuff(c);

    // If deleted before the insertion point, adjust it
    यदि (curwin->w_cursor.lnum == Insstart_orig.lnum
				  && curwin->w_cursor.col < Insstart_orig.col)
	Insstart_orig.col = curwin->w_cursor.col;

    // vi behaviour: the cursor moves backward but the character that
    //		     was there remains visible
    // Vim behaviour: the cursor moves backward and the character that
    //		      was there is erased from the screen.
    // We can emulate the vi behaviour by pretending there is a dollar
    // displayed even when there isn't.
    //  --pkv Sun Jan 19 01:56:40 EST 2003
    यदि (vim_strchr(p_cpo, CPO_BACKSPACE) != रिक्त && dollar_vcol == -1)
	dollar_vcol = curwin->w_virtcol;

#ifdef FEAT_FOLDING
    // When deleting a char the cursor line must never be in a closed fold.
    // E.g., when 'foldmethod' is indent and deleting the first non-white
    // char before a Tab.
    यदि (did_backspace)
	foldOpenCursor();
#endif

    प्रत्यागम did_backspace;
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
    पूर्णाङ्क		save_allow_कुंजियाँ = allow_कुंजियाँ;
    पूर्णाङ्क		save_paste = p_paste;

    // If the end code is too long we can't detect it, read everything.
    यदि (end != रिक्त && STRLEN(end) >= NUMBUFLEN)
	end = रिक्त;
    ++no_mapping;
    allow_कुंजियाँ = 0;
    यदि (!p_paste)
	// Also have the side effects of setting 'paste' to make it work much
	// faster.
	set_option_मूल्य_give_err((char_u *)"paste", TRUE, रिक्त, 0);

    प्रति (;;)
    {
	// When the end is not defined read everything there is.
	यदि (end == रिक्त && vpeekc() == NUL)
	    भङ्ग;
	कुरु
	    c = vgetc();
	यावत् (c == K_IGNORE || c == K_VER_SCROLLBAR || c == K_HOR_SCROLLBAR);

	यदि (c == NUL || got_int || (ex_normal_busy > 0 && c == Ctrl_C))
	    // When CTRL-C was encountered the typeahead will be flushed and we
	    // won't get the end sequence.  Except when using ":normal".
	    भङ्ग;

	यदि (has_mbyte)
	    idx += (*mb_char2bytes)(c, buf + idx);
	अन्यथा
	    buf[idx++] = c;
	buf[idx] = NUL;
	यदि (end != रिक्त && STRNCMP(buf, end, idx) == 0)
	{
	    यदि (end[idx] == NUL)
		भङ्ग; // Found the end of paste code.
	    अनुवर्त;
	}
	यदि (!drop)
	{
	    विकल्प (mode)
	    {
		पक्ष PASTE_CMDLINE:
		    put_on_cmdline(buf, idx, TRUE);
		    भङ्ग;

		पक्ष PASTE_EX:
		    // add one for the NUL that is going to be appended
		    यदि (gap != रिक्त && ga_grow(gap, idx + 1) == OK)
		    {
			mch_memmove((वर्ण *)gap->ga_data + gap->ga_len,
							     buf, (size_t)idx);
			gap->ga_len += idx;
		    }
		    भङ्ग;

		पक्ष PASTE_जोड़ो:
		    यदि (stop_arrow() == OK)
		    {
			c = buf[0];
			यदि (idx == 1 && (c == CAR || c == K_KENTER || c == NL))
			    ins_eol(c);
			अन्यथा
			{
			    ins_char_bytes(buf, idx);
			    AppendToRedobuffLit(buf, idx);
			}
		    }
		    भङ्ग;

		पक्ष PASTE_ONE_CHAR:
		    यदि (ret_char == -1)
		    {
			यदि (has_mbyte)
			    ret_char = (*mb_ptr2char)(buf);
			अन्यथा
			    ret_char = buf[0];
		    }
		    भङ्ग;
	    }
	}
	idx = 0;
    }

    --no_mapping;
    allow_कुंजियाँ = save_allow_कुंजियाँ;
    यदि (!save_paste)
	set_option_मूल्य_give_err((char_u *)"paste", FALSE, रिक्त, 0);

    प्रत्यागम ret_char;
}

#यदि defined(FEAT_GUI_TABLINE)
    स्थिर शून्य
ins_tabline(पूर्णाङ्क c)
{
    // We will be leaving the current window, unless closing another tab.
    यदि (c != K_TABMENU || current_tabmenu != TABLINE_MENU_CLOSE
		|| (current_tab != 0 && current_tab != tabpage_index(curtab)))
    {
	undisplay_dollar();
	start_arrow(&curwin->w_cursor);
	can_cindent = TRUE;
    }

    यदि (c == K_TABLINE)
	goto_tabpage(current_tab);
    अन्यथा
    {
	handle_tabmenu();
	redraw_statuslines();	// will redraw the tabline when needed
    }
}
#endif

#यदि defined(FEAT_GUI)
    शून्य
ins_scroll(शून्य)
{
    pos_T	tpos;

    undisplay_dollar();
    tpos = curwin->w_cursor;
    यदि (gui_do_scroll())
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
    यदि (do_mousescroll_horiz(scrollbar_मूल्य))
    {
	start_arrow(&tpos);
	can_cindent = TRUE;
    }
}
#endif

    स्थिर शून्य
ins_बायाँ(शून्य)
{
    pos_T	tpos;
    पूर्णाङ्क		end_change = dont_sync_undo == FALSE; // end undoable change

#ifdef FEAT_FOLDING
    यदि ((fdo_flags & FDO_HOR) && कुंजी_Typed)
	foldOpenCursor();
#endif
    undisplay_dollar();
    tpos = curwin->w_cursor;
    यदि (oneleft() == OK)
    {
#यदि defined(FEAT_XIM) && defined(FEAT_GUI_GTK)
	// Only call start_arrow() when not busy with preediting, it will
	// break undo.  K_LEFT is inserted in im_correct_cursor().
	यदि (p_imst == IM_OVER_THE_SPOT || !im_is_preediting())
#endif
	{
	    start_arrow_with_change(&tpos, end_change);
	    यदि (!end_change)
		AppendCharToRedobuff(K_बायाँ);
	}
#ifdef FEAT_RIGHTLEFT
	// If exit reversed string, position is fixed
	यदि (revins_scol != -1 && (पूर्णाङ्क)curwin->w_cursor.col >= revins_scol)
	    revins_legal++;
	revins_chars++;
#endif
    }

    /*
     * if 'whichwrap' set for cursor in insert mode may go to
     * previous line
     */
    अन्यथा यदि (vim_strchr(p_ww, '[') != रिक्त && curwin->w_cursor.lnum > 1)
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
ins_home(पूर्णाङ्क c)
{
    pos_T	tpos;

#ifdef FEAT_FOLDING
    यदि ((fdo_flags & FDO_HOR) && कुंजी_Typed)
	foldOpenCursor();
#endif
    undisplay_dollar();
    tpos = curwin->w_cursor;
    यदि (c == K_C_HOME)
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
    यदि ((fdo_flags & FDO_HOR) && कुंजी_Typed)
	foldOpenCursor();
#endif
    undisplay_dollar();
    tpos = curwin->w_cursor;
    यदि (c == K_C_END)
	curwin->w_cursor.lnum = curbuf->b_ml.ml_पंक्ति_गिनती;
    coladvance((colnr_T)MAXCOL);
    curwin->w_curswant = MAXCOL;

    start_arrow(&tpos);
}

    स्थिर शून्य
ins_s_बायाँ(शून्य)
{
    पूर्णाङ्क end_change = dont_sync_undo == FALSE; // end undoable change
#ifdef FEAT_FOLDING
    यदि ((fdo_flags & FDO_HOR) && कुंजी_Typed)
	foldOpenCursor();
#endif
    undisplay_dollar();
    यदि (curwin->w_cursor.lnum > 1 || curwin->w_cursor.col > 0)
    {
	start_arrow_with_change(&curwin->w_cursor, end_change);
	यदि (!end_change)
	    AppendCharToRedobuff(K_S_बायाँ);
	(शून्य)bck_word(1L, FALSE, FALSE);
	curwin->w_set_curswant = true;
    }
    अन्यथा
	vim_beep(BO_CRSR);
    dont_sync_undo = FALSE;
}

    स्थिर शून्य
ins_दायाँ(शून्य)
{
    पूर्णाङ्क end_change = dont_sync_undo == FALSE; // end undoable change

#ifdef FEAT_FOLDING
    यदि ((fdo_flags & FDO_HOR) && कुंजी_Typed)
	foldOpenCursor();
#endif
    undisplay_dollar();
    यदि (gchar_cursor() != NUL || virtual_active())
    {
	start_arrow_with_change(&curwin->w_cursor, end_change);
	यदि (!end_change)
	    AppendCharToRedobuff(K_दायाँ);
	curwin->w_set_curswant = true;
	यदि (virtual_active())
	    oneright();
	अन्यथा
	{
	    यदि (has_mbyte)
		curwin->w_cursor.col += (*mb_ptr2len)(ml_get_cursor());
	    अन्यथा
		++curwin->w_cursor.col;
	}

#ifdef FEAT_RIGHTLEFT
	revins_legal++;
	यदि (revins_chars)
	    revins_chars--;
#endif
    }
    // if 'whichwrap' set for cursor in insert mode, may move the
    // cursor to the next line
    अन्यथा यदि (vim_strchr(p_ww, ']') != रिक्त
	    && curwin->w_cursor.lnum < curbuf->b_ml.ml_पंक्ति_गिनती)
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
ins_s_दायाँ(शून्य)
{
    पूर्णाङ्क end_change = dont_sync_undo == FALSE; // end undoable change
#ifdef FEAT_FOLDING
    यदि ((fdo_flags & FDO_HOR) && कुंजी_Typed)
	foldOpenCursor();
#endif
    undisplay_dollar();
    यदि (curwin->w_cursor.lnum < curbuf->b_ml.ml_पंक्ति_गिनती
	    || gchar_cursor() != NUL)
    {
	start_arrow_with_change(&curwin->w_cursor, end_change);
	यदि (!end_change)
	    AppendCharToRedobuff(K_S_दायाँ);
	(शून्य)fwd_word(1L, FALSE, 0);
	curwin->w_set_curswant = true;
    }
    अन्यथा
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
    यदि (cursor_up(1L, TRUE) == OK)
    {
	यदि (startcol)
	    coladvance(getvcol_nolist(&Insstart));
	यदि (old_topline != curwin->w_topline
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

    यदि (mod_mask & MOD_MASK_CTRL)
    {
	// <C-PageUp>: tab page back
	यदि (first_tabpage->tp_next != रिक्त)
	{
	    start_arrow(&curwin->w_cursor);
	    goto_tabpage(-1);
	}
	प्रत्यागम;
    }

    tpos = curwin->w_cursor;
    यदि (pagescroll(BACKWARD, 1L, FALSE) == OK)
    {
	start_arrow(&tpos);
	can_cindent = TRUE;
    }
    अन्यथा
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
    यदि (cursor_down(1L, TRUE) == OK)
    {
	यदि (startcol)
	    coladvance(getvcol_nolist(&Insstart));
	यदि (old_topline != curwin->w_topline
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

    यदि (mod_mask & MOD_MASK_CTRL)
    {
	// <C-PageDown>: tab page forward
	यदि (first_tabpage->tp_next != रिक्त)
	{
	    start_arrow(&curwin->w_cursor);
	    goto_tabpage(0);
	}
	प्रत्यागम;
    }

    tpos = curwin->w_cursor;
    यदि (pagescroll(FORWARD, 1L, FALSE) == OK)
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
    स्थिर पूर्णाङ्क
ins_tab(शून्य)
{
    पूर्णाङ्क		ind;
    पूर्णाङ्क		i;
    पूर्णाङ्क		temp;

    यदि (Insstart_blank_vcol == MAXCOL && curwin->w_cursor.lnum == Insstart.lnum)
	Insstart_blank_vcol = get_nolist_virtcol();
    यदि (echeck_abbr(TAB + ABBR_OFF))
	प्रत्यागम FALSE;

    ind = inindent(0);
    यदि (ind)
	can_cindent = FALSE;

    /*
     * When nothing special, insert TAB like a normal character.
     */
    यदि (!curbuf->b_p_et
#ifdef FEAT_VARTABS
	    && !(p_sta && ind
		// These five lines mean 'tabstop' != 'shiftwidth'
		&& ((tabstop_गिनती(curbuf->b_p_vts_array) > 1)
		    || (tabstop_गिनती(curbuf->b_p_vts_array) == 1
			&& tabstop_first(curbuf->b_p_vts_array)
						       != get_sw_मूल्य(curbuf))
		    || (tabstop_गिनती(curbuf->b_p_vts_array) == 0
			&& curbuf->b_p_ts != get_sw_मूल्य(curbuf))))
	    && tabstop_गिनती(curbuf->b_p_vsts_array) == 0
#अन्यथा
	    && !(p_sta && ind && curbuf->b_p_ts != get_sw_मूल्य(curbuf))
#endif
	    && get_sts_मूल्य() == 0)
	प्रत्यागम TRUE;

    यदि (stop_arrow() == FAIL)
	प्रत्यागम TRUE;

    did_ai = FALSE;
    did_si = FALSE;
    can_si = FALSE;
    can_si_back = FALSE;
    AppendToRedobuff((char_u *)"\t");

#ifdef FEAT_VARTABS
    यदि (p_sta && ind)		// insert tab in indent, use 'shiftwidth'
    {
	temp = (पूर्णाङ्क)get_sw_मूल्य(curbuf);
	temp -= get_nolist_virtcol() % temp;
    }
    अन्यथा यदि (tabstop_गिनती(curbuf->b_p_vsts_array) > 0 || curbuf->b_p_sts != 0)
				// use 'softtabstop' when set
	temp = tabstop_padding(get_nolist_virtcol(), get_sts_मूल्य(),
						     curbuf->b_p_vsts_array);
    अन्यथा			// otherwise use 'tabstop'
	temp = tabstop_padding(get_nolist_virtcol(), curbuf->b_p_ts,
						     curbuf->b_p_vts_array);
#अन्यथा
    यदि (p_sta && ind)		// insert tab in indent, use 'shiftwidth'
	temp = (पूर्णाङ्क)get_sw_मूल्य(curbuf);
    अन्यथा यदि (curbuf->b_p_sts != 0) // use 'softtabstop' when set
	temp = (पूर्णाङ्क)get_sts_मूल्य();
    अन्यथा			// otherwise use 'tabstop'
	temp = (पूर्णाङ्क)curbuf->b_p_ts;
    temp -= get_nolist_virtcol() % temp;
#endif

    /*
     * Insert the first space with ins_char().	It will delete one char in
     * replace mode.  Insert the rest with ins_str(); it will not delete any
     * chars.  For MODE_VREPLACE state, we use ins_char() for all characters.
     */
    ins_char(' ');
    यावत् (--temp > 0)
    {
	यदि (State & VREPLACE_FLAG)
	    ins_char(' ');
	अन्यथा
	{
	    ins_str((char_u *)" ", 1);
	    यदि (State & REPLACE_FLAG)	    // no char replaced
		replace_धकेलो(NUL);
	}
    }

    /*
     * When 'expandtab' not set: Replace spaces by TABs where possible.
     */
#ifdef FEAT_VARTABS
    यदि (!curbuf->b_p_et && (tabstop_गिनती(curbuf->b_p_vsts_array) > 0
			    || get_sts_मूल्य() > 0
			    || (p_sta && ind)))
#अन्यथा
    यदि (!curbuf->b_p_et && (get_sts_मूल्य() || (p_sta && ind)))
#endif
    {
	char_u		*ptr;
	char_u		*saved_पंक्ति = रिक्त;	// init for GCC
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
	यदि (State & VREPLACE_FLAG)
	{
	    pos = curwin->w_cursor;
	    cursor = &pos;
	    saved_पंक्ति = vim_strnsave(ml_get_curline(), ml_get_curline_len());
	    यदि (saved_पंक्ति == रिक्त)
		प्रत्यागम FALSE;
	    ptr = saved_पंक्ति + pos.col;
	}
	अन्यथा
	{
	    ptr = ml_get_cursor();
	    cursor = &curwin->w_cursor;
	}

	// When 'L' is not in 'cpoptions' a tab always takes up 'ts' spaces.
	यदि (vim_strchr(p_cpo, CPO_LISTWM) == रिक्त)
	    curwin->w_p_list = FALSE;

	// Find first white before the cursor
	fpos = curwin->w_cursor;
	यावत् (fpos.col > 0 && VIM_ISWHITE(ptr[-1]))
	{
	    --fpos.col;
	    --ptr;
	}

	// In Replace mode, don't change characters before the insert point.
	यदि ((State & REPLACE_FLAG)
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
	यावत् (VIM_ISWHITE(*ptr))
	{
	    i = lbr_chartabsize(&cts);
	    यदि (cts.cts_vcol + i > want_vcol)
		भङ्ग;
	    यदि (*ptr != TAB)
	    {
		*ptr = TAB;
		यदि (change_col < 0)
		{
		    change_col = fpos.col;  // Column of first change
		    // May have to adjust Insstart
		    यदि (fpos.lnum == Insstart.lnum && fpos.col < Insstart.col)
			Insstart.col = fpos.col;
		}
	    }
	    ++fpos.col;
	    ++ptr;
	    cts.cts_vcol += i;
	}
	vcol = cts.cts_vcol;
	clear_chartabsize_arg(&cts);

	यदि (change_col >= 0)
	{
	    पूर्णाङ्क		    repl_off = 0;

	    // Skip over the spaces we need.
	    init_chartabsize_arg(&cts, curwin, 0, vcol, ptr, ptr);
	    यावत् (cts.cts_vcol < want_vcol && *cts.cts_ptr == ' ')
	    {
		cts.cts_vcol += lbr_chartabsize(&cts);
		++cts.cts_ptr;
		++repl_off;
	    }
	    ptr = cts.cts_ptr;
	    vcol = cts.cts_vcol;
	    clear_chartabsize_arg(&cts);

	    यदि (vcol > want_vcol)
	    {
		// Must have a char with 'showbreak' just before it.
		--ptr;
		--repl_off;
	    }
	    fpos.col += repl_off;

	    // Delete following spaces.
	    i = cursor->col - fpos.col;
	    यदि (i > 0)
	    {
#ifdef FEAT_PROP_POPUP
		यदि (!(State & VREPLACE_FLAG))
		{
		    char_u  *newp;
		    पूर्णाङ्क	    col;

		    newp = alloc(curbuf->b_ml.ml_पंक्ति_len - i);
		    यदि (newp == रिक्त)
			प्रत्यागम FALSE;

		    col = ptr - curbuf->b_ml.ml_पंक्ति_ptr;
		    यदि (col > 0)
			mch_memmove(newp, ptr - col, col);
		    mch_memmove(newp + col, ptr + i,
					   curbuf->b_ml.ml_पंक्ति_len - col - i);

		    यदि (curbuf->b_ml.ml_flags & (ML_पंक्ति_DIRTY | ML_ALLOCATED))
			vim_free(curbuf->b_ml.ml_पंक्ति_ptr);
		    curbuf->b_ml.ml_पंक्ति_ptr = newp;
		    curbuf->b_ml.ml_पंक्ति_len -= i;
		    curbuf->b_ml.ml_पंक्ति_textlen = 0;
		    curbuf->b_ml.ml_flags =
			   (curbuf->b_ml.ml_flags | ML_पंक्ति_DIRTY) & ~ML_EMPTY;
		}
		अन्यथा
#endif
		    STRMOVE(ptr, ptr + i);
		// correct replace stack.
		यदि ((State & REPLACE_FLAG) && !(State & VREPLACE_FLAG))
		    प्रति (temp = i; --temp >= 0; )
			replace_join(repl_off);
	    }
#ifdef FEAT_NETBEANS_INTG
	    यदि (netbeans_active())
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
	    यदि (State & VREPLACE_FLAG)
	    {
		// Backspace from real cursor to change_col
		backspace_until_column(change_col);

		// Insert each char in saved_line from changed_col to
		// ptr-cursor
		ins_bytes_len(saved_पंक्ति + change_col,
						    cursor->col - change_col);
	    }
	}

	यदि (State & VREPLACE_FLAG)
	    vim_free(saved_पंक्ति);
	curwin->w_p_list = save_list;
    }

    प्रत्यागम FALSE;
}

/*
 * Handle CR or NL in insert mode.
 * Return FAIL when out of memory or can't undo.
 */
    पूर्णाङ्क
ins_eol(पूर्णाङ्क c)
{
    पूर्णाङ्क	    i;

    यदि (echeck_abbr(c + ABBR_OFF))
	प्रत्यागम OK;
    यदि (stop_arrow() == FAIL)
	प्रत्यागम FAIL;
    undisplay_dollar();

    /*
     * Strange Vi behaviour: In Replace mode, typing a NL will not delete the
     * character under the cursor.  Only push a NUL on the replace stack,
     * nothing to put back when the NL is deleted.
     */
    यदि ((State & REPLACE_FLAG) && !(State & VREPLACE_FLAG))
	replace_धकेलो(NUL);

    /*
     * In MODE_VREPLACE state, a NL replaces the rest of the line, and starts
     * replacing the next line, so we push all of the characters left on the
     * line onto the replace stack.  This is not done here though, it is done
     * in open_line().
     */

    // Put cursor on NUL if on the last char and coladd is 1 (happens after
    // CTRL-O).
    यदि (virtual_active() && curwin->w_cursor.coladd > 0)
	coladvance(getviscol());

#ifdef FEAT_RIGHTLEFT
    // NL in reverse insert will always start in the end of
    // current line.
    यदि (revins_on)
	curwin->w_cursor.col += ml_get_cursor_len();
#endif

    AppendToRedobuff(NL_STR);
    i = open_पंक्ति(FORWARD,
	    has_format_option(FO_RET_COMS) ? OPENLINE_DO_COM : 0, old_indent,
	    रिक्त);
    old_indent = 0;
    can_cindent = TRUE;
#ifdef FEAT_FOLDING
    // When inserting a line the cursor line must never be in a closed fold.
    foldOpenCursor();
#endif

    प्रत्यागम i;
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
    यदि (redrawing() && !char_avail())
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
    ++allow_कुंजियाँ;
    c = plain_vgetc();
    --no_mapping;
    --allow_कुंजियाँ;
    यदि (did_putchar)
	// when the line fits in 'columns' the '?' is at the start of the next
	// line and will not be removed by the redraw
	edit_unputchar();

    यदि (IS_SPECIAL(c) || mod_mask)	    // special key
    {
	clear_showcmd();
	जोड़ो_special(c, TRUE, FALSE);
	प्रत्यागम NUL;
    }
    यदि (c != ESC)
    {
	did_putchar = FALSE;
	यदि (redrawing() && !char_avail())
	{
	    // may need to redraw when no more chars available now
	    ins_redraw(FALSE);

	    यदि (char2cells(c) == 1)
	    {
		ins_redraw(FALSE);
		edit_putchar(c, TRUE);
		did_putchar = TRUE;
	    }
	    add_to_showcmd_c(c);
	}
	++no_mapping;
	++allow_कुंजियाँ;
	cc = plain_vgetc();
	--no_mapping;
	--allow_कुंजियाँ;
	यदि (did_putchar)
	    // when the line fits in 'columns' the '?' is at the start of the
	    // next line and will not be removed by a redraw
	    edit_unputchar();
	यदि (cc != ESC)
	{
	    AppendToRedobuff((char_u *)CTRL_V_STR);
	    c = digraph_get(c, cc, TRUE);
	    clear_showcmd();
	    प्रत्यागम c;
	}
    }
    clear_showcmd();
    प्रत्यागम NUL;
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
    char_u	    *पंक्ति;
    chartabsize_T   cts;

    यदि (lnum < 1 || lnum > curbuf->b_ml.ml_पंक्ति_गिनती)
    {
	vim_beep(BO_COPY);
	प्रत्यागम NUL;
    }

    // try to advance to the cursor column
    validate_virtcol();
    पंक्ति = ml_get(lnum);
    prev_ptr = पंक्ति;
    init_chartabsize_arg(&cts, curwin, lnum, 0, पंक्ति, पंक्ति);
    यावत् (cts.cts_vcol < curwin->w_virtcol && *cts.cts_ptr != NUL)
    {
	prev_ptr = cts.cts_ptr;
	cts.cts_vcol += lbr_chartabsize_adv(&cts);
    }
    यदि (cts.cts_vcol > curwin->w_virtcol)
	ptr = prev_ptr;
    अन्यथा
	ptr = cts.cts_ptr;
    clear_chartabsize_arg(&cts);

    c = (*mb_ptr2char)(ptr);
    यदि (c == NUL)
	vim_beep(BO_COPY);
    प्रत्यागम c;
}

/*
 * CTRL-Y or CTRL-E typed in Insert mode.
 */
    स्थिर पूर्णाङ्क
ins_ctrl_ey(पूर्णाङ्क tc)
{
    पूर्णाङ्क	    c = tc;

    यदि (ctrl_x_mode_scroll())
    {
	यदि (c == Ctrl_Y)
	    scrolldown_clamp();
	अन्यथा
	    scrollup_clamp();
	redraw_later(UPD_VALID);
    }
    अन्यथा
    {
	c = ins_copychar(curwin->w_cursor.lnum + (c == Ctrl_Y ? -1 : 1));
	यदि (c != NUL)
	{
	    long	tw_save;

	    // The character must be taken literally, insert like it
	    // was typed after a CTRL-V, and pretend 'textwidth'
	    // wasn't set.  Digits, 'o' and 'x' are special after a
	    // CTRL-V, don't use it for these.
	    यदि (c < 256 && !SAFE_isalnum(c))
		AppendToRedobuff((char_u *)CTRL_V_STR);	// CTRL-V
	    tw_save = curbuf->b_p_tw;
	    curbuf->b_p_tw = -1;
	    जोड़ो_special(c, TRUE, FALSE);
	    curbuf->b_p_tw = tw_save;
#ifdef FEAT_RIGHTLEFT
	    revins_chars++;
	    revins_legal++;
#endif
	    c = Ctrl_V;	// pretend CTRL-V is last character
	    auto_format(FALSE, TRUE);
	}
    }
    प्रत्यागम c;
}

/*
 * Get the value that w_virtcol would have when 'list' is off.
 * Unless 'cpo' contains the 'L' flag.
 */
    colnr_T
get_nolist_virtcol(शून्य)
{
    // check validity of cursor in current buffer
    यदि (curwin->w_buffer == रिक्त
	|| curwin->w_buffer->b_ml.ml_mfp == रिक्त
	|| curwin->w_cursor.lnum > curwin->w_buffer->b_ml.ml_पंक्ति_गिनती)
	प्रत्यागम 0;
    यदि (curwin->w_p_list && vim_strchr(p_cpo, CPO_LISTWM) == रिक्त)
	प्रत्यागम getvcol_nolist(&curwin->w_cursor);
    validate_virtcol();
    प्रत्यागम curwin->w_virtcol;
}

#यदि defined(FEAT_EVAL)
/*
 * Handle the InsertCharPre autocommand.
 * "c" is the character that was typed.
 * Return a pointer to allocated memory with the replacement string.
 * Return NULL to continue inserting "c".
 */
    स्थिर char_u *
do_जोड़ो_char_pre(पूर्णाङ्क c)
{
    char_u	*res;
    char_u	buf[MB_MAXBYTES + 1];
    size_t	buflen;
    पूर्णाङ्क		save_State = State;

    // Return quickly when there is nothing to do.
    यदि (!has_insertcharpre())
	प्रत्यागम रिक्त;

    यदि (c == Ctrl_RSB)
	प्रत्यागम रिक्त;

    यदि (has_mbyte)
	buflen = (*mb_char2bytes)(c, buf);
    अन्यथा
    {
	buf[0] = c;
	buflen = 1;
    }
    buf[buflen] = NUL;

    // Lock the text to avoid weird things from happening.
    ++textlock;
    set_vim_var_string(VV_CHAR, buf, (पूर्णाङ्क)buflen);  // set v:char

    res = रिक्त;
    यदि (ins_apply_autocmds(EVENT_INSERTCHARPRE))
    {
	// Get the value of v:char.  It may be empty or more than one
	// character.  Only use it when changed, otherwise continue with the
	// original character to avoid breaking autoindent.
	यदि (STRCMP(buf, get_vim_var_str(VV_CHAR)) != 0)
	    res = vim_strsave(get_vim_var_str(VV_CHAR));
    }

    set_vim_var_string(VV_CHAR, रिक्त, -1);  // clear v:char
    --textlock;

    // Restore the State, it may have been changed.
    State = save_State;

    प्रत्यागम res;
}
#endif

    पूर्णाङ्क
get_can_cindent(शून्य)
{
    प्रत्यागम can_cindent;
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
    यदि (event != EVENT_INSERTLEAVE && tick != CHANGEDTICK(curbuf))
	u_save(curwin->w_cursor.lnum, (linenr_T)(curwin->w_cursor.lnum + 1));

    प्रत्यागम r;
}
