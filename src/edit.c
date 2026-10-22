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

#அந்தர்பாவ "vim.h"

#define BACKSPACE_CHAR		    1
#define BACKSPACE_WORD		    2
#define BACKSPACE_WORD_NOT_SPACE    3
#define BACKSPACE_வரி		    4

// Set when doing something for completion that may call edit() recursively,
// which is not allowed.
நிலையான முழு	compl_busy = FALSE;


நிலையான வெறுமை ins_ctrl_v(வெறுமை);
நிலையான வெறுமை செருகு_special(முழு, முழு, முழு);
நிலையான வெறுமை redo_literal(முழு c);
நிலையான வெறுமை start_arrow_common(pos_T *end_செருகு_pos, முழு change);
#ifdef FEAT_SPELL
நிலையான வெறுமை check_spell_redraw(வெறுமை);
#endif
நிலையான வெறுமை stop_செருகு(pos_T *end_செருகு_pos, முழு esc, முழு nomove);
நிலையான முழு  echeck_abbr(முழு);
நிலையான வெறுமை mb_replace_எடு_ins(முழு cc);
நிலையான வெறுமை replace_flush(வெறுமை);
நிலையான வெறுமை replace_do_bs(முழு limit_col);
நிலையான முழு del_char_after_col(முழு limit_col);
நிலையான வெறுமை ins_reg(வெறுமை);
நிலையான வெறுமை ins_ctrl_g(வெறுமை);
நிலையான வெறுமை ins_ctrl_hat(வெறுமை);
நிலையான முழு  ins_esc(long *மொத்தம், முழு cmdchar, முழு nomove);
#ifdef FEAT_RIGHTLEFT
நிலையான வெறுமை ins_ctrl_(வெறுமை);
#endif
நிலையான முழு ins_start_select(முழு c);
நிலையான வெறுமை ins_செருகு(முழு replaceState);
நிலையான வெறுமை ins_ctrl_o(வெறுமை);
நிலையான வெறுமை ins_shift(முழு c, முழு lastc);
நிலையான வெறுமை ins_del(வெறுமை);
நிலையான முழு  ins_bs(முழு c, முழு mode, முழு *inserted_space_p);
#எனில் defined(FEAT_GUI_TABLINE)
நிலையான வெறுமை ins_tabline(முழு c);
#endif
நிலையான வெறுமை ins_இடது(வெறுமை);
நிலையான வெறுமை ins_home(முழு c);
நிலையான வெறுமை ins_end(முழு c);
நிலையான வெறுமை ins_s_இடது(வெறுமை);
நிலையான வெறுமை ins_வலது(வெறுமை);
நிலையான வெறுமை ins_s_வலது(வெறுமை);
நிலையான வெறுமை ins_up(முழு startcol);
நிலையான வெறுமை ins_pageup(வெறுமை);
நிலையான வெறுமை ins_down(முழு startcol);
நிலையான வெறுமை ins_pagedown(வெறுமை);
#ifdef FEAT_DND
நிலையான வெறுமை ins_drop(வெறுமை);
#endif
நிலையான முழு  ins_tab(வெறுமை);
#ifdef FEAT_DIGRAPHS
நிலையான முழு  ins_digraph(வெறுமை);
#endif
நிலையான முழு  ins_ctrl_ey(முழு tc);
#எனில் defined(FEAT_EVAL)
நிலையான char_u *do_செருகு_char_pre(முழு c);
#endif

நிலையான colnr_T	Insstart_textlen;	// length of line when insert started
நிலையான colnr_T	Insstart_blank_vcol;	// vcol for first inserted blank
நிலையான முழு	update_Insstart_orig = TRUE; // set Insstart_orig to Insstart

நிலையான string_T	last_செருகு = {வெற்று, 0};    // the text of the previous insert,
					    // K_SPECIAL and CSI are escaped
நிலையான முழு	last_செருகு_skip; // nr of chars in front of previous insert
நிலையான முழு	new_செருகு_skip;  // nr of chars in front of current insert
நிலையான முழு	did_restart_edit;	// "restart_edit" when calling edit()

நிலையான முழு	can_cindent;		// may do cindenting on this line

#ifdef FEAT_RIGHTLEFT
நிலையான முழு	revins_on;		// reverse insert mode on
நிலையான முழு	revins_chars;		// how much to skip after edit
நிலையான முழு	revins_legal;		// was the last char 'legal'?
நிலையான முழு	revins_scol;		// start column of revins session
#endif

நிலையான முழு	ins_need_undo;		// call u_save() before inserting a
					// char.  Set when edit() is called.
					// after that arrow_used is used.

நிலையான முழு	dont_sync_undo = FALSE;	// CTRL-G U prevents syncing undo for
					// the next left/right cursor key

// With 'autocompletedelay' set, arm the delay and let the main loop fire
// Insert-mode autocommands; the popup is shown later on K_COMPLETE_DELAY.
// Otherwise trigger completion right away.
#define TRIGGER_AUTOCOMPLETE()			\
    செய் {					\
	update_screen(UPD_VALID);  /* Show char (deletion) immediately */ \
	out_flush();				\
	ins_compl_enable_autocomplete();	\
	ins_compl_arm_autostart();		\
	எனில் (!ins_compl_arm_autocomplete_delay())\
	    செல் docomplete;			\
    } போது (0)

#define MAY_TRIGGER_AUTOCOMPLETE(c)				\
    செய் {							\
	எனில் (ins_compl_has_autocomplete() && !char_avail()	\
		&& curwin->w_cursor.col > 0)			\
	{							\
	    (c) = char_before_cursor();				\
	    எனில் (vim_isprintc(c))				\
		TRIGGER_AUTOCOMPLETE();				\
	}							\
    } போது (0)

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
    முழு
edit(
    முழு		cmdchar,
    முழு		startln,	// if set, insert at start of line
    long	மொத்தம்)
{
    முழு		c = 0;
    char_u	*ptr;
    முழு		lastc = 0;
    முழு		mincol;
    நிலையான linenr_T o_lnum = 0;
    முழு		i;
    முழு		did_backspace = TRUE;	    // previous char was backspace
    முழு		வரி_is_white = FALSE;	    // line is empty before insert
    linenr_T	old_topline = 0;	    // topline before insertion
#ifdef FEAT_DIFF
    முழு		old_topfill = -1;
#endif
    முழு		inserted_space = FALSE;     // just inserted a space
    முழு		replaceState = MODE_REPLACE;
    முழு		nomove = FALSE;		    // don't move cursor on return
#ifdef FEAT_JOB_CHANNEL
    முழு		cmdchar_todo = cmdchar;
#endif
#ifdef FEAT_CONCEAL
    முழு		cursor_வரி_was_concealed;
#endif
    முழு		ins_just_started = TRUE;

    // Remember whether editing was restarted after CTRL-O.
    did_restart_edit = restart_edit;

    // sleep before redrawing, needed for "CTRL-O :" that results in an
    // error message
    check_for_delay(TRUE);

    // set Insstart_orig to Insstart
    update_Insstart_orig = TRUE;

#ifdef HAVE_SANDBOX
    // Don't allow inserting in the sandbox.
    எனில் (sandbox != 0)
    {
	emsg(_(e_not_allowed_in_sandbox));
	திரும்பு FALSE;
    }
#endif
    // Don't allow changes in the buffer while editing the cmdline.  The
    // caller of getcmdline() may get confused.
    // Don't allow recursive insert mode when busy with completion.
    எனில் (textlock != 0 || ins_compl_active() || compl_busy || pum_visible())
    {
	emsg(_(e_not_allowed_to_change_text_or_change_window));
	திரும்பு FALSE;
    }
    ins_compl_clear();	    // clear stuff for CTRL-X mode

    /*
     * Trigger InsertEnter autocommands.  Do not do this for "r<CR>" or "grx".
     */
    எனில் (cmdchar != 'r' && cmdchar != 'v')
    {
	pos_T   save_cursor = curwin->w_cursor;

#ifdef FEAT_EVAL
	எனில் (cmdchar == 'R')
	    ptr = (char_u *)"r";
	இல்லையெனில் எனில் (cmdchar == 'V')
	    ptr = (char_u *)"v";
	இல்லையெனில்
	    ptr = (char_u *)"i";
	set_vim_var_string(VV_INSERTMODE, ptr, 1);
	set_vim_var_string(VV_CHAR, வெற்று, -1);  // clear v:char
#endif
	ins_apply_autocmds(EVENT_INSERTENTER);

	// Check for changed highlighting, e.g. for ModeMsg.
	எனில் (need_highlight_changed)
	    highlight_changed();

	// Make sure the cursor didn't move.  Do call check_cursor_col() in
	// case the text was modified.  Since Insert mode was not started yet
	// a call to check_cursor_col() may move the cursor, especially with
	// the "A" command, thus set State to avoid that. Also check that the
	// line number is still valid (lines may have been deleted).
	// Do not restore if v:char was set to a non-empty string.
	எனில் (!EQUAL_POS(curwin->w_cursor, save_cursor)
#ifdef FEAT_EVAL
		&& *get_vim_var_str(VV_CHAR) == NUL
#endif
		&& save_cursor.lnum <= curbuf->b_ml.ml_வரி_மொத்தம்)
	{
	    முழு save_state = State;

	    curwin->w_cursor = save_cursor;
	    State = MODE_செருகு;
	    check_cursor_col();
	    State = save_state;
	}
    }

#ifdef FEAT_CONCEAL
    // Check if the cursor line was concealed before changing State.
    cursor_வரி_was_concealed = curwin->w_p_cole > 0
						&& conceal_cursor_வரி(curwin);
#endif

    /*
     * When doing a paste with the middle mouse button, Insstart is set to
     * where the paste started.
     */
    எனில் (where_paste_started.lnum != 0)
	Insstart = where_paste_started;
    இல்லையெனில்
    {
	Insstart = curwin->w_cursor;
	எனில் (startln)
	    Insstart.col = 0;
    }
    Insstart_textlen = (colnr_T)linetabsize_str(ml_get_curline());
    Insstart_blank_vcol = MAXCOL;
    எனில் (!did_ai)
	ai_col = 0;

    எனில் (cmdchar != NUL && restart_edit == 0)
    {
	ResetRedobuff();
	Append_எண்_To_Redobuff(மொத்தம்);
	எனில் (cmdchar == 'V' || cmdchar == 'v')
	{
	    // "gR" or "gr" command
	    AppendCharToRedobuff('g');
	    AppendCharToRedobuff((cmdchar == 'v') ? 'r' : 'R');
	}
	இல்லையெனில்
	{
	    எனில் (cmdchar == K_PS)
		AppendCharToRedobuff('a');
	    இல்லையெனில்
		AppendCharToRedobuff(cmdchar);
	    எனில் (cmdchar == 'g')		    // "gI" command
		AppendCharToRedobuff('I');
	    இல்லையெனில் எனில் (cmdchar == 'r')	    // "r<CR>" command
		மொத்தம் = 1;		    // insert only one <CR>
	}
    }

    எனில் (cmdchar == 'R')
    {
	State = MODE_REPLACE;
    }
    இல்லையெனில் எனில் (cmdchar == 'V' || cmdchar == 'v')
    {
	State = MODE_VREPLACE;
	replaceState = MODE_VREPLACE;
	orig_வரி_மொத்தம் = curbuf->b_ml.ml_வரி_மொத்தம்;
	vr_வரிகள்_changed = 1;
    }
    இல்லையெனில்
	State = MODE_செருகு;

    may_trigger_modechanged();
    stop_செருகு_mode = FALSE;

#ifdef FEAT_CONCEAL
    // Check if the cursor line needs redrawing after changing State.  If
    // 'concealcursor' is "n" it needs to be redrawn without concealing.
    conceal_check_cursor_வரி(cursor_வரி_was_concealed);
#endif

    // Need to position cursor again when on a TAB and when on a char with
    // virtual text.
    எனில் (gchar_cursor() == TAB
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
    எனில் (curbuf->b_p_iminsert == B_IMODE_LMAP)
	State |= MODE_LANGMAP;
#ifdef HAVE_உள்ளீடு_METHOD
    im_set_active(curbuf->b_p_iminsert == B_IMODE_IM);
#endif

    setmouse();
    clear_showcmd();
#ifdef FEAT_RIGHTLEFT
    // there is no reverse replace mode
    revins_on = (State == MODE_செருகு && p_ri);
    எனில் (revins_on)
	undisplay_dollar();
    revins_chars = 0;
    revins_legal = 0;
    revins_scol = -1;
#endif
    எனில் (!p_ek)
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
    எனில் (restart_edit != 0 && stuff_empty())
    {
	/*
	 * After a paste we consider text typed to be part of the insert for
	 * the pasted text. You can backspace over the pasted text too.
	 */
	எனில் (where_paste_started.lnum)
	    arrow_used = FALSE;
	இல்லையெனில்
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
	எனில் (((ins_at_eol && curwin->w_cursor.lnum == o_lnum)
		    || curwin->w_curswant > curwin->w_virtcol)
		&& *(ptr = ml_get_curline() + curwin->w_cursor.col) != NUL)
	{
	    எனில் (ptr[1] == NUL)
		++curwin->w_cursor.col;
	    இல்லையெனில் எனில் (has_mbyte)
	    {
		i = (*mb_ptr2len)(ptr);
		எனில் (ptr[i] == NUL)
		    curwin->w_cursor.col += i;
	    }
	}
	ins_at_eol = FALSE;
    }
    இல்லையெனில்
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
    எனில் (!p_im && did_restart_edit == 0)
	foldOpenCursor();
#endif

    /*
     * If 'showmode' is set, show the current (insert/replace/..) mode.
     * A warning message for changing a readonly file is given here, before
     * actually changing anything.  It's put after the mode, if any.
     */
    i = 0;
    எனில் (p_smd && செய்தி_silent == 0)
	i = showmode();

    எனில் (!p_im && did_restart_edit == 0)
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
    new_செருகு_skip = (முழு)inserted.length;
    எனில் (inserted.string != வெற்று)
	vim_free(inserted.string);

    old_indent = 0;

    /*
     * Main loop in Insert mode: repeat until Insert mode is left.
     */
    க்கு (;;)
    {
#ifdef FEAT_RIGHTLEFT
	எனில் (!revins_legal)
	    revins_scol = -1;	    // reset on illegal motions
	இல்லையெனில்
	    revins_legal = 0;
#endif
	எனில் (arrow_used)	    // don't repeat insert when arrow key used
	    மொத்தம் = 0;

	எனில் (update_Insstart_orig)
	    Insstart_orig = Insstart;

	எனில் (stop_செருகு_mode && !ins_compl_active())
	{
	    // ":stopinsert" used or 'insertmode' reset
	    மொத்தம் = 0;
	    செல் doESCkey;
	}

	// set curwin->w_curswant for next K_DOWN or K_UP
	எனில் (!arrow_used)
	    curwin->w_set_curswant = true;

	// If there is no typeahead may check for timestamps (e.g., for when a
	// menu invoked a shell command).
	எனில் (stuff_empty())
	{
	    did_check_timestamps = FALSE;
	    எனில் (need_check_timestamps)
		check_timestamps(FALSE);
	}

	/*
	 * When emsg() was called msg_scroll will have been set.
	 */
	செய்தி_scroll = FALSE;

#ifdef FEAT_GUI
	// When 'mousefocus' is set a mouse movement may have taken us to
	// another window.  "need_mouse_correct" may then be set because of an
	// autocommand.
	எனில் (need_mouse_correct)
	    gui_mouse_correct();
#endif

#ifdef FEAT_FOLDING
	// Open fold at the cursor line, according to 'foldopen'.
	எனில் (fdo_flags & FDO_செருகு)
	    foldOpenCursor();
	// Close folds where the cursor isn't, according to 'foldclose'
	எனில் (!char_avail())
	    foldCheckClose();
#endif

#ifdef FEAT_JOB_CHANNEL
	எனில் (bt_prompt(curbuf))
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
	எனில் (curbuf->b_mod_set
		&& curwin->w_p_wrap
		&& !curwin->w_p_sms
		&& !did_backspace
		&& curwin->w_topline == old_topline
#ifdef FEAT_DIFF
		&& curwin->w_topfill == old_topfill
#endif
		&& மொத்தம் <= 1
		)
	{
	    mincol = curwin->w_wcol;
	    validate_cursor_col();

	    எனில் (
#ifdef FEAT_VARTABS
		curwin->w_wcol < mincol - tabstop_at(get_nolist_virtcol(),
						     curbuf->b_p_ts,
						     curbuf->b_p_vts_array,
						     FALSE)
#இல்லையெனில்
		(முழு)curwin->w_wcol < mincol - curbuf->b_p_ts
#endif
		    && curwin->w_wrow ==
				   curwin->w_height - 1 - get_scrolloff_மதிப்பு()
		    && (curwin->w_cursor.lnum != curwin->w_topline
#ifdef FEAT_DIFF
			|| curwin->w_topfill > 0
#endif
		    ))
	    {
#ifdef FEAT_DIFF
		எனில் (curwin->w_topfill > 0)
		    --curwin->w_topfill;
		இல்லையெனில்
#endif
#ifdef FEAT_FOLDING
		எனில் (hasFolding(curwin->w_topline, வெற்று, &old_topline))
		    set_topline(curwin, old_topline + 1);
		இல்லையெனில்
#endif
		    set_topline(curwin, curwin->w_topline + 1);
	    }
	}

	// May need to adjust w_topline to show the cursor.
	எனில் (மொத்தம் <= 1)
	    update_topline();

	did_backspace = FALSE;

	எனில் (மொத்தம் <= 1)
	    validate_cursor();		// may set must_redraw

	/*
	 * Redraw the display when no characters are waiting.
	 * Also shows mode, ruler and positions cursor.
	 */
	ins_redraw(TRUE);

	எனில் (curwin->w_p_scb)
	    do_check_scrollbind(TRUE);

	எனில் (curwin->w_p_crb)
	    do_check_cursorbind();
	எனில் (மொத்தம் <= 1)
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
	எனில் (c != K_CURSORHOLD && c != K_COMPLETE_DELAY)
	    lastc = c;		// remember the previous char for CTRL-D

	// After using CTRL-G U the next cursor key will not break undo.
	எனில் (dont_sync_undo == MAYBE)
	    dont_sync_undo = TRUE;
	இல்லையெனில்
	    dont_sync_undo = FALSE;
	எனில் (cmdchar == K_PS)
	    // Got here from normal mode when bracketed paste started.
	    c = K_PS;
	இல்லையெனில்
	{
	    // Trigger autocomplete when entering Insert mode, either directly
	    // or via change commands like 'ciw', 'cw', etc., before the first
	    // character is typed.
	    எனில் (ins_just_started)
	    {
		ins_just_started = FALSE;
		எனில் (ins_compl_has_autocomplete() && !char_avail()
			&& curwin->w_cursor.col > 0)
		{
		    c = char_before_cursor();
		    எனில் (vim_isprintc(c))
		    {
			ins_compl_enable_autocomplete();
			ins_compl_arm_autostart();
			ins_compl_init_get_longest();
#ifdef FEAT_RIGHTLEFT
			எனில் (p_hkmap)
			    c = hkmap(c);		// Hebrew mode mapping
#endif
			// Defer until the delay expires (K_COMPLETE_DELAY), or
			// trigger now when no delay is in effect.
			எனில் (!ins_compl_arm_autocomplete_delay())
			    செல் docomplete;
		    }
		}
	    }

	    செய்
	    {
		c = safe_vgetc();

		எனில் (stop_செருகு_mode
#ifdef FEAT_TERMINAL
			|| (c == K_IGNORE && term_use_loop())
#endif
		   )
		{
		    // Insert mode ended, possibly from a callback, or a timer
		    // must have opened a terminal window.
		    எனில் (c != K_IGNORE && c != K_NOP)
			vungetc(c);
		    மொத்தம் = 0;

		    எனில் (!bt_prompt(curwin->w_buffer)
#ifdef FEAT_TERMINAL
			    && !bt_terminal(curwin->w_buffer)
#endif
			    && stop_செருகு_mode)
			// :stopinsert command via callback or via server command
			nomove = FALSE;
		    இல்லையெனில்
			nomove = TRUE;
		    ins_compl_prep(ESC);
		    செல் doESCkey;
		}
	    } போது (c == K_IGNORE || c == K_NOP);
	}

	// Don't want K_CURSORHOLD for the second key, e.g., after CTRL-V.
	did_cursorhold = TRUE;
	எனில் (c != K_CURSORHOLD && c != K_COMPLETE_DELAY)
	{
	    // Don't want delayed autocompletion from the previous key either.
	    ins_compl_clear_autocomplete_delay();
	    ins_compl_disarm_autostart();
	    // A completion already on screen goes on being what it was.
	    எனில் (!ins_compl_active())
		ins_compl_disable_autocomplete();
	}

#ifdef FEAT_RIGHTLEFT
	எனில் (p_hkmap && சாவி_Typed)
	    c = hkmap(c);		// Hebrew mode mapping
#endif

	// If the window was made so small that nothing shows, make it at least
	// one line and one column when typing.
	எனில் (சாவி_Typed && !சாவி_Stuffed)
	    win_ensure_size();

	/*
	 * Special handling of keys while the popup menu is visible or wanted
	 * and the cursor is still in the completed word.  Only when there is
	 * a match, skip this when no matches were found.
	 */
	எனில் (ins_compl_active() && curwin->w_cursor.col >= ins_compl_col()
		&& ins_compl_has_shown_match() && pum_wanted())
	{
	    // BS: Delete one character from "compl_leader".
	    எனில் ((c == K_BS || c == Ctrl_H)
			&& curwin->w_cursor.col > ins_compl_col()
			&& (c = ins_compl_bs()) == NUL)
		தொடர்;

	    // When no match was selected or it was edited.
	    எனில் (!ins_compl_used_match())
	    {
		// CTRL-L: Add one character from the current match to
		// "compl_leader".  Except when at the original match and
		// there is nothing to add, CTRL-L works like CTRL-P then.
		எனில் (c == Ctrl_L
			&& (!ctrl_x_mode_வரி_or_eval()
			    || ins_compl_long_shown_match()))
		{
		    ins_compl_addfrommatch();
		    தொடர்;
		}

		// A non-white character that fits in with the current
		// completion: Add to "compl_leader".
		எனில் (ins_compl_accept_char(c))
		{
#எனில் defined(FEAT_EVAL)
		    // Trigger InsertCharPre.
		    char_u *str = do_செருகு_char_pre(c);
		    char_u *p;

		    எனில் (str != வெற்று)
		    {
			க்கு (p = str; *p != NUL; MB_PTR_ADV(p))
			    ins_compl_addleader(PTR2CHAR(p));
			vim_free(str);
		    }
		    இல்லையெனில்
#endif
			ins_compl_addleader(c);
		    தொடர்;
		}

		// Pressing CTRL-Y selects the current match.  When
		// ins_compl_enter_selects() is set the Enter key does the
		// same.
		எனில் ((c == Ctrl_Y || (ins_compl_enter_selects()
				    && (c == CAR || c == K_KENTER || c == NL)))
			&& stop_arrow() == OK)
		{
		    ins_compl_delete();
		    எனில் (ins_compl_preinsert_longest()
			    && !ins_compl_is_match_selected())
		    {
			ins_compl_செருகு(FALSE, TRUE);
			ins_compl_init_get_longest();
			தொடர்;
		    }
		    இல்லையெனில்
			ins_compl_செருகு(FALSE, FALSE);
		}
		// Delete preinserted text when typing special chars
		இல்லையெனில் எனில் (IS_WHITE_NL_OR_NUL(c) && ins_compl_preinsert_effect())
		    ins_compl_delete();
	    }
	}

	// Prepare for or stop CTRL-X mode.  This doesn't do completion, but
	// it does fix up the text when finishing completion.
	ins_compl_init_get_longest();
	எனில் (ins_compl_prep(c))
	    தொடர்;

	// CTRL-\ CTRL-N goes to Normal mode,
	// CTRL-\ CTRL-G goes to mode selected with 'insertmode',
	// CTRL-\ CTRL-O is like CTRL-O but without moving the cursor.
	எனில் (c == Ctrl_BSL)
	{
	    // may need to redraw when no more chars available now
	    ins_redraw(FALSE);
	    ++no_mapping;
	    ++allow_சாவிகள்;
	    c = plain_vgetc();
	    --no_mapping;
	    --allow_சாவிகள்;
	    எனில் (c != Ctrl_N && c != Ctrl_G && c != Ctrl_O)
	    {
		// it's something else
		vungetc(c);
		c = Ctrl_BSL;
	    }
	    இல்லையெனில் எனில் (c == Ctrl_G && p_im)
		தொடர்;
	    இல்லையெனில்
	    {
		எனில் (c == Ctrl_O)
		{
		    ins_ctrl_o();
		    ins_at_eol = FALSE;	// cursor keeps its column
		    nomove = TRUE;
		}
		மொத்தம் = 0;
		செல் doESCkey;
	    }
	}

#ifdef FEAT_DIGRAPHS
	c = do_digraph(c);
#endif

	எனில் ((c == Ctrl_V || c == Ctrl_Q) && ctrl_x_mode_cmdline())
	    செல் docomplete;
	எனில் (c == Ctrl_V || c == Ctrl_Q)
	{
	    ins_ctrl_v();
	    c = Ctrl_V;	// pretend CTRL-V is last typed character
	    தொடர்;
	}

	எனில் (cindent_on() && ctrl_x_mode_none())
	{
	    // A key name preceded by a bang means this key is not to be
	    // inserted.  Skip ahead to the re-indenting below.
	    // A key name preceded by a star means that indenting has to be
	    // done before inserting the key.
	    வரி_is_white = inindent(0);
	    எனில் (in_cinkeys(c, '!', வரி_is_white))
		செல் force_cindent;
	    எனில் (can_cindent && in_cinkeys(c, '*', வரி_is_white)
							&& stop_arrow() == OK)
		do_c_கோவை_indent();
	}

#ifdef FEAT_RIGHTLEFT
	எனில் (curwin->w_p_rl)
	    தேர்வு (c)
	    {
		நிலை K_இடது:	c = K_வலது; நிறுத்து;
		நிலை K_S_இடது:	c = K_S_வலது; நிறுத்து;
		நிலை K_C_இடது:	c = K_C_வலது; நிறுத்து;
		நிலை K_வலது:	c = K_இடது; நிறுத்து;
		நிலை K_S_வலது: c = K_S_இடது; நிறுத்து;
		நிலை K_C_வலது: c = K_C_இடது; நிறுத்து;
	    }
#endif

	/*
	 * If 'keymodel' contains "startsel", may start selection.  If it
	 * does, a CTRL-O and c will be stuffed, we need to get these
	 * characters.
	 */
	எனில் (ins_start_select(c))
	    தொடர்;

	/*
	 * The big switch to handle a character in insert mode.
	 */
	தேர்வு (c)
	{
	நிலை ESC:	// End input mode
	    எனில் (echeck_abbr(ESC + ABBR_OFF))
		நிறுத்து;
	    // FALLTHROUGH

	நிலை Ctrl_C:	// End input mode
	    எனில் (c == Ctrl_C && cmdwin_type != 0)
	    {
		// Close the cmdline window.
		cmdwin_result = K_IGNORE;
		got_int = FALSE; // don't stop executing autocommands et al.
		nomove = TRUE;
		செல் doESCkey;
	    }
#ifdef FEAT_JOB_CHANNEL
	    எனில் (c == Ctrl_C && bt_prompt(curbuf))
	    {
		எனில் (invoke_prompt_interrupt())
		{
		    எனில் (!bt_prompt(curbuf))
			// buffer changed to a non-prompt buffer, get out of
			// Insert mode
			செல் doESCkey;
		    நிறுத்து;
		}
	    }
#endif

#ifdef UNIX
do_intr:
#endif
	    // when 'insertmode' set, and not halfway a mapping, don't leave
	    // Insert mode
	    எனில் (goto_im())
	    {
		எனில் (got_int)
		{
		    (வெறுமை)vgetc();		// flush all buffers
		    got_int = FALSE;
		}
		இல்லையெனில்
		    vim_beep(BO_IM);
		நிறுத்து;
	    }
doESCkey:
	    // Drop a pending autocomplete so it does not outlive Insert mode.
	    ins_compl_clear_autocomplete_delay();
	    /*
	     * This is the ONLY return from edit()!
	     */
	    // Always update o_lnum, so that a "CTRL-O ." that adds a line
	    // still puts the cursor back after the inserted text.
	    எனில் (ins_at_eol && gchar_cursor() == NUL)
		o_lnum = curwin->w_cursor.lnum;

	    எனில் (ins_esc(&மொத்தம், cmdchar, nomove))
	    {
		// When CTRL-C was typed got_int will be set, with the result
		// that the autocommands won't be executed. When mapped got_int
		// is not set, but let's keep the behavior the same.
		எனில் (cmdchar != 'r' && cmdchar != 'v' && c != Ctrl_C)
		    ins_apply_autocmds(EVENT_INSERTLEAVE);
		did_cursorhold = FALSE;

		// ins_redraw() triggers TextChangedI only when no characters
		// are in the typeahead buffer, so reset curbuf->b_last_changedtick only
		// if the TextChangedI was not blocked by char_avail() (e.g. using :norm!)
		// and the TextChangedI autocommand has been triggered.
		எனில் (!char_avail() && curbuf->b_last_changedtick_i == CHANGEDTICK(curbuf))
		    curbuf->b_last_changedtick = CHANGEDTICK(curbuf);
		திரும்பு (c == Ctrl_O);
	    }
	    தொடர்;

	நிலை Ctrl_Z:	// suspend when 'insertmode' set
	    எனில் (!p_im)
		செல் normalchar;	// insert CTRL-Z as normal char
	    do_cmdline_cmd((char_u *)"stop");
#ifdef CURSOR_SHAPE
	    ui_cursor_shape();		// may need to update cursor shape
#endif
	    தொடர்;

	நிலை Ctrl_O:	// execute one command
#ifdef FEAT_COMPL_FUNC
	    எனில் (ctrl_x_mode_omni())
		செல் docomplete;
#endif
	    எனில் (echeck_abbr(Ctrl_O + ABBR_OFF))
		நிறுத்து;
	    ins_ctrl_o();

	    // don't move the cursor left when 'virtualedit' has "onemore".
	    எனில் (get_ve_flags() & VE_ONEMORE)
	    {
		ins_at_eol = FALSE;
		nomove = TRUE;
	    }
	    மொத்தம் = 0;
	    செல் doESCkey;

	நிலை K_INS:	// toggle insert/replace mode
	நிலை K_KINS:
	    ins_செருகு(replaceState);
	    நிறுத்து;

	நிலை K_SELECT:	// end of Select mode mapping - ignore
	    நிறுத்து;

	நிலை K_HELP:	// Help key works like <ESC> <Help>
	நிலை K_F1:
	நிலை K_XF1:
	    stuffcharReadbuff(K_HELP);
	    எனில் (p_im)
		need_start_insertmode = TRUE;
	    செல் doESCkey;

#ifdef FEAT_NETBEANS_INTG
	நிலை K_F21:	// NetBeans command
	    ++no_mapping;		// don't map the next key hits
	    i = plain_vgetc();
	    --no_mapping;
	    netbeans_keycommand(i);
	    நிறுத்து;
#endif

	நிலை K_ZERO:	// Insert the previously inserted text.
	நிலை NUL:
	நிலை Ctrl_A:
	    // For ^@ the trailing ESC will end the insert, unless there is an
	    // error.
	    எனில் (stuff_inserted(NUL, 1L, (c == Ctrl_A)) == FAIL
						   && c != Ctrl_A && !p_im)
		செல் doESCkey;		// quit insert mode
	    inserted_space = FALSE;
	    நிறுத்து;

	நிலை Ctrl_R:	// insert the contents of a register
	    எனில் (ctrl_x_mode_register() && !ins_compl_active())
		செல் docomplete;
	    ins_reg();
	    auto_format(FALSE, TRUE);
	    inserted_space = FALSE;
	    நிறுத்து;

	நிலை Ctrl_G:	// commands starting with CTRL-G
	    ins_ctrl_g();
	    நிறுத்து;

	நிலை Ctrl_HAT:	// switch input mode and/or langmap
	    ins_ctrl_hat();
	    நிறுத்து;

#ifdef FEAT_RIGHTLEFT
	நிலை Ctrl__:	// switch between languages
	    எனில் (!p_ari)
		செல் normalchar;
	    ins_ctrl_();
	    நிறுத்து;
#endif

	நிலை Ctrl_D:	// Make indent one shiftwidth smaller.
#எனில் defined(FEAT_FIND_ID)
	    எனில் (ctrl_x_mode_path_defines())
		செல் docomplete;
#endif
	    // FALLTHROUGH

	நிலை Ctrl_T:	// Make indent one shiftwidth greater.
	    எனில் (c == Ctrl_T && ctrl_x_mode_thesaurus())
	    {
		எனில் (has_compl_option(FALSE))
		    செல் docomplete;
		நிறுத்து;
	    }

	    ins_shift(c, lastc);
	    auto_format(FALSE, TRUE);
	    inserted_space = FALSE;
	    நிறுத்து;

	நிலை K_DEL:	// delete character under the cursor
	நிலை K_KDEL:
	    ins_del();
	    auto_format(FALSE, TRUE);
	    நிறுத்து;

	நிலை K_BS:	// delete character before the cursor
	நிலை K_S_BS:
	நிலை Ctrl_H:
	    did_backspace = ins_bs(c, BACKSPACE_CHAR, &inserted_space);
	    auto_format(FALSE, TRUE);
	    எனில் (did_backspace)
		MAY_TRIGGER_AUTOCOMPLETE(c);
	    நிறுத்து;

	நிலை Ctrl_W:	// delete word before the cursor
#ifdef FEAT_JOB_CHANNEL
	    எனில் (bt_prompt(curbuf) && (mod_mask & MOD_MASK_SHIFT) == 0)
	    {
		// In a prompt window CTRL-W is used for window commands.
		// Use Shift-CTRL-W to delete a word.
		stuffcharReadbuff(Ctrl_W);
		restart_edit = 'A';
		nomove = TRUE;
		மொத்தம் = 0;
		செல் doESCkey;
	    }
#endif
	    did_backspace = ins_bs(c, BACKSPACE_WORD, &inserted_space);
	    auto_format(FALSE, TRUE);
	    எனில் (did_backspace)
		MAY_TRIGGER_AUTOCOMPLETE(c);
	    நிறுத்து;

	நிலை Ctrl_U:	// delete all inserted text in current line
#ifdef FEAT_COMPL_FUNC
	    // CTRL-X CTRL-U completes with 'completefunc'.
	    எனில் (ctrl_x_mode_function())
		செல் docomplete;
#endif
	    did_backspace = ins_bs(c, BACKSPACE_வரி, &inserted_space);
	    auto_format(FALSE, TRUE);
	    inserted_space = FALSE;
	    எனில் (did_backspace)
		MAY_TRIGGER_AUTOCOMPLETE(c);
	    நிறுத்து;

	நிலை K_LEFTMOUSE:   // mouse keys
	நிலை K_LEFTMOUSE_NM:
	நிலை K_LEFTDRAG:
	நிலை K_LEFTRELEASE:
	நிலை K_LEFTRELEASE_NM:
	நிலை K_MOUSEMOVE:
	நிலை K_MIDDLEMOUSE:
	நிலை K_MIDDLEDRAG:
	நிலை K_MIDDLERELEASE:
	நிலை K_RIGHTMOUSE:
	நிலை K_RIGHTDRAG:
	நிலை K_RIGHTRELEASE:
	நிலை K_X1MOUSE:
	நிலை K_X1DRAG:
	நிலை K_X1RELEASE:
	நிலை K_X2MOUSE:
	நிலை K_X2DRAG:
	நிலை K_X2RELEASE:
	    ins_mouse(c);
	    நிறுத்து;

	நிலை K_MOUSEDOWN: // Default action for scroll wheel up: scroll up
	    ins_mousescroll(MSCR_DOWN);
	    நிறுத்து;

	நிலை K_MOUSEUP:	// Default action for scroll wheel down: scroll down
	    ins_mousescroll(MSCR_UP);
	    நிறுத்து;

	நிலை K_MOUSELEFT: // Scroll wheel left
	    ins_mousescroll(MSCR_இடது);
	    நிறுத்து;

	நிலை K_MOUSERIGHT: // Scroll wheel right
	    ins_mousescroll(MSCR_வலது);
	    நிறுத்து;

	நிலை K_PS:
	    bracketed_paste(PASTE_செருகு, FALSE, வெற்று);
	    எனில் (cmdchar == K_PS)
		// invoked from normal mode, bail out
		செல் doESCkey;
	    நிறுத்து;
	நிலை K_PE:
	    // Got K_PE without K_PS, ignore.
	    நிறுத்து;

#ifdef FEAT_GUI_TABLINE
	நிலை K_TABLINE:
	நிலை K_TABMENU:
	    ins_tabline(c);
	    நிறுத்து;
#endif

	நிலை K_IGNORE:	// Something mapped to nothing
	    நிறுத்து;

	நிலை K_COMMAND:		    // <Cmd>command<CR>
	நிலை K_SCRIPT_COMMAND:	    // <ScriptCmd>command<CR>
	    {
		bufref_T    save_curbuf;
		varnumber_T tick = CHANGEDTICK(curbuf);

		set_bufref(&save_curbuf, curbuf);
		do_cmdkey_command(c, 0);

#ifdef FEAT_TERMINAL
		எனில் (term_use_loop())
		    // Started a terminal that gets the input, exit Insert mode.
		    செல் doESCkey;
#endif
		எனில் (curbuf->b_u_synced
			|| (bufref_valid(&save_curbuf)
			    && curbuf == save_curbuf.br_buf
			    && tick != CHANGEDTICK(curbuf)))
		{
		    // The command synced undo or changed this buffer.
		    // Save the cursor line before the next typed edit.
		    ins_need_undo = TRUE;
		}
	    }
	    நிறுத்து;

	நிலை K_CURSORHOLD:	// Didn't type something for a while.
	    ins_apply_autocmds(EVENT_CURSORHOLDI);
	    did_cursorhold = TRUE;
	    // If CTRL-G U was used apply it to the next typed key.
	    எனில் (dont_sync_undo == TRUE)
		dont_sync_undo = MAYBE;
	    நிறுத்து;

	நிலை K_COMPLETE_DELAY:	// 'autocompletedelay' expired
	    // If CTRL-G U was used apply it to the next typed key.
	    எனில் (dont_sync_undo == TRUE)
		dont_sync_undo = MAYBE;
	    ins_compl_clear_autocomplete_delay();
	    எனில் (!ins_compl_has_autocomplete() || char_avail()
		    || curwin->w_cursor.col == 0)
		நிறுத்து;
	    c = char_before_cursor();
	    எனில் (!vim_isprintc(c))
		நிறுத்து;
	    // The completion may have been cleared while waiting, so re-enable
	    // autocomplete to match a zero delay.
	    ins_compl_enable_autocomplete();
	    ins_compl_arm_autostart();
	    செல் docomplete;

#ifdef FEAT_GUI_MSWIN
	    // On MS-Windows ignore <M-F4>, we get it when closing the window
	    // was cancelled.
	நிலை K_F4:
	    எனில் (mod_mask != MOD_MASK_ALT)
		செல் normalchar;
	    நிறுத்து;
#endif

#ifdef FEAT_GUI
	நிலை K_VER_SCROLLBAR:
	    ins_scroll();
	    நிறுத்து;

	நிலை K_HOR_SCROLLBAR:
	    ins_horscroll();
	    நிறுத்து;
#endif

	நிலை K_HOME:	// <Home>
	நிலை K_KHOME:
	நிலை K_S_HOME:
	நிலை K_C_HOME:
	    ins_home(c);
	    நிறுத்து;

	நிலை K_END:	// <End>
	நிலை K_KEND:
	நிலை K_S_END:
	நிலை K_C_END:
	    ins_end(c);
	    நிறுத்து;

	நிலை K_இடது:	// <Left>
	    எனில் (mod_mask & (MOD_MASK_SHIFT|MOD_MASK_CTRL))
		ins_s_இடது();
	    இல்லையெனில்
		ins_இடது();
	    நிறுத்து;

	நிலை K_S_இடது:	// <S-Left>
	நிலை K_C_இடது:
	    ins_s_இடது();
	    நிறுத்து;

	நிலை K_வலது:	// <Right>
	    எனில் (mod_mask & (MOD_MASK_SHIFT|MOD_MASK_CTRL))
		ins_s_வலது();
	    இல்லையெனில்
		ins_வலது();
	    நிறுத்து;

	நிலை K_S_வலது:	// <S-Right>
	நிலை K_C_வலது:
	    ins_s_வலது();
	    நிறுத்து;

	நிலை K_UP:	// <Up>
	    எனில் (pum_visible())
		செல் docomplete;
	    எனில் (mod_mask & MOD_MASK_SHIFT)
		ins_pageup();
	    இல்லையெனில்
		ins_up(FALSE);
	    நிறுத்து;

	நிலை K_S_UP:	// <S-Up>
	நிலை K_PAGEUP:
	நிலை K_KPAGEUP:
	    எனில் (pum_visible())
	    {
#ifdef FEAT_PROP_POPUP
		// CTRL-SHIFT-<Up> scrolls the info popup up a line,
		// CTRL-SHIFT-<PageUp> a page.  Shift is folded into K_S_UP but
		// stays in mod_mask for PageUp, hence the asymmetric check.
		எனில் (c == K_S_UP ? (mod_mask & MOD_MASK_CTRL)
			: ((mod_mask & MOD_MASK_CTRL)
			    && (mod_mask & MOD_MASK_SHIFT)))
		{
		    popup_scroll_info(-1, c != K_S_UP);
		    நிறுத்து;
		}
#endif
		செல் docomplete;
	    }
	    ins_pageup();
	    நிறுத்து;

	நிலை K_DOWN:	// <Down>
	    எனில் (pum_visible())
		செல் docomplete;
	    எனில் (mod_mask & MOD_MASK_SHIFT)
		ins_pagedown();
	    இல்லையெனில்
		ins_down(FALSE);
	    நிறுத்து;

	நிலை K_S_DOWN:	// <S-Down>
	நிலை K_PAGEDOWN:
	நிலை K_KPAGEDOWN:
	    எனில் (pum_visible())
	    {
#ifdef FEAT_PROP_POPUP
		// CTRL-SHIFT-<Down>/<PageDown> scroll the info popup down.
		எனில் (c == K_S_DOWN ? (mod_mask & MOD_MASK_CTRL)
			: ((mod_mask & MOD_MASK_CTRL)
			    && (mod_mask & MOD_MASK_SHIFT)))
		{
		    popup_scroll_info(1, c != K_S_DOWN);
		    நிறுத்து;
		}
#endif
		செல் docomplete;
	    }
	    ins_pagedown();
	    நிறுத்து;

#ifdef FEAT_DND
	நிலை K_DROP:	// drag-n-drop event
	    ins_drop();
	    நிறுத்து;
#endif

	நிலை K_S_TAB:	// When not mapped, use like a normal TAB
	    c = TAB;
	    // FALLTHROUGH

	நிலை TAB:	// TAB or Complete patterns along path
#எனில் defined(FEAT_FIND_ID)
	    எனில் (ctrl_x_mode_path_patterns())
		செல் docomplete;
#endif
	    inserted_space = FALSE;
	    எனில் (ins_tab())
		செல் normalchar;	// insert TAB as a normal char
	    auto_format(FALSE, TRUE);
	    நிறுத்து;

	நிலை K_KENTER:	// <Enter>
	    c = CAR;
	    // FALLTHROUGH
	நிலை CAR:
	நிலை NL:
#எனில் defined(FEAT_QUICKFIX)
	    // In a quickfix window a <CR> jumps to the error under the
	    // cursor.
	    எனில் (bt_quickfix(curbuf) && c == CAR)
	    {
		எனில் (curwin->w_llist_ref == வெற்று)    // quickfix window
		    do_cmdline_cmd((char_u *)".cc");
		இல்லையெனில்				    // location list window
		    do_cmdline_cmd((char_u *)".ll");
		நிறுத்து;
	    }
#endif
	    எனில் (cmdwin_type != 0)
	    {
		// Execute the command in the cmdline window.
		cmdwin_result = CAR;
		செல் doESCkey;
	    }
#ifdef FEAT_JOB_CHANNEL
	    எனில் (bt_prompt(curbuf))
	    {
		invoke_prompt_callback();
		எனில் (!bt_prompt(curbuf))
		    // buffer changed to a non-prompt buffer, get out of
		    // Insert mode
		    செல் doESCkey;
		நிறுத்து;
	    }
#endif
	    எனில் (ins_eol(c) == FAIL && !p_im)
		செல் doESCkey;	    // out of memory
	    auto_format(FALSE, FALSE);
	    inserted_space = FALSE;
	    நிறுத்து;

	நிலை Ctrl_K:	    // digraph or keyword completion
	    எனில் (ctrl_x_mode_dictionary())
	    {
		எனில் (has_compl_option(TRUE))
		    செல் docomplete;
		நிறுத்து;
	    }
#ifdef FEAT_DIGRAPHS
	    c = ins_digraph();
	    எனில் (c == NUL)
		நிறுத்து;
#endif
	    செல் normalchar;

	நிலை Ctrl_X:	// Enter CTRL-X mode
	    ins_ctrl_x();
	    நிறுத்து;

	நிலை Ctrl_RSB:	// Tag name completion after ^X
	    எனில் (!ctrl_x_mode_tags())
		செல் normalchar;
	    செல் docomplete;

	நிலை Ctrl_F:	// File name completion after ^X
	    எனில் (!ctrl_x_mode_files())
		செல் normalchar;
	    செல் docomplete;

	நிலை 's':	// Spelling completion after ^X
	நிலை Ctrl_S:
	    எனில் (!ctrl_x_mode_spell())
		செல் normalchar;
	    செல் docomplete;

	நிலை Ctrl_L:	// Whole line completion after ^X
	    எனில் (!ctrl_x_mode_whole_வரி())
	    {
		// CTRL-L with 'insertmode' set: Leave Insert mode
		எனில் (p_im)
		{
		    எனில் (echeck_abbr(Ctrl_L + ABBR_OFF))
			நிறுத்து;
		    செல் doESCkey;
		}
		செல் normalchar;
	    }
	    // FALLTHROUGH

	நிலை Ctrl_P:	// Do previous/next pattern completion
	நிலை Ctrl_N:
#ifdef FEAT_PROP_POPUP
	    // CTRL-SHIFT-P/N scroll the info popup one line.
	    எனில் (pum_visible() && (mod_mask & MOD_MASK_SHIFT)
		    && (c == Ctrl_P || c == Ctrl_N))
	    {
		popup_scroll_info(c == Ctrl_P ? -1 : 1, false);
		நிறுத்து;
	    }
#endif
	    // if 'complete' is empty then plain ^P is no longer special,
	    // but it is under other ^X modes
	    எனில் (*curbuf->b_p_cpt == NUL
		    && (ctrl_x_mode_normal() || ctrl_x_mode_whole_வரி())
		    && !compl_status_local())
		செல் normalchar;

docomplete:
	    ins_compl_clear_autocomplete_delay();
	    compl_busy = TRUE;
#ifdef FEAT_FOLDING
	    disable_fold_update++;  // don't redraw folds here
#endif
	    எனில் (ins_complete(c, TRUE) == FAIL)
		compl_status_clear();
#ifdef FEAT_FOLDING
	    disable_fold_update--;
#endif
	    compl_busy = FALSE;
	    can_si = may_do_si(); // allow smartindenting
	    நிறுத்து;

	நிலை Ctrl_Y:	// copy from previous line or scroll down
	நிலை Ctrl_E:	// copy from next line	   or scroll up
	    c = ins_ctrl_ey(c);
	    நிறுத்து;

	இயல்பு:
#ifdef UNIX
	    எனில் (c == intr_char)		// special interrupt char
		செல் do_intr;
#endif

normalchar:
	    /*
	     * Insert a normal character.
	     */
#எனில் defined(FEAT_EVAL)
	    எனில் (!p_paste)
	    {
		// Trigger InsertCharPre.
		char_u *str = do_செருகு_char_pre(c);
		char_u *p;

		எனில் (str != வெற்று)
		{
		    எனில் (*str != NUL && stop_arrow() != FAIL)
		    {
			// Insert the new value of v:char literally.
			க்கு (p = str; *p != NUL; MB_PTR_ADV(p))
			{
			    c = PTR2CHAR(p);
			    எனில் (c == CAR || c == K_KENTER || c == NL)
				ins_eol(c);
			    இல்லையெனில்
				ins_char(c);
			}
			AppendToRedobuffLit(str, -1);
		    }
		    vim_free(str);
		    c = NUL;
		}

		// If the new value is already inserted or an empty string
		// then don't insert any character.
		எனில் (c == NUL)
		    நிறுத்து;
	    }
#endif
	    // Try to perform smart-indenting.
	    ins_try_si(c);

	    எனில் (c == ' ')
	    {
		inserted_space = TRUE;
		எனில் (inindent(0))
		    can_cindent = FALSE;
		எனில் (Insstart_blank_vcol == MAXCOL
			&& curwin->w_cursor.lnum == Insstart.lnum)
		    Insstart_blank_vcol = get_nolist_virtcol();
	    }

	    // Insert a normal character and check for abbreviations on a
	    // special character.  Let CTRL-] expand abbreviations without
	    // inserting it.
	    எனில் (vim_iswordc(c) || (!echeck_abbr(
			// Add ABBR_OFF for characters above 0x100, this is
			// what check_abbr() expects.
				(has_mbyte && c >= 0x100) ? (c + ABBR_OFF) : c)
			&& c != Ctrl_RSB))
	    {
		செருகு_special(c, FALSE, FALSE);
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
	    எனில் (ins_compl_has_autocomplete() && !char_avail()
		    && vim_isprintc(c))
		TRIGGER_AUTOCOMPLETE();

	    நிறுத்து;
	}   // end of switch (c)

	// If typed something may trigger CursorHoldI again; K_COMPLETE_DELAY is
	// injected, not typed.
	எனில் (c != K_CURSORHOLD && c != K_COMPLETE_DELAY
#ifdef FEAT_COMPL_FUNC
		// but not in CTRL-X mode, a script can't restore the state
		&& ctrl_x_mode_normal()
#endif
	       )
	    did_cursorhold = FALSE;

	// Check if we need to cancel completion mode because the window
	// or tab page was changed
	எனில் (ins_compl_active() && !ins_compl_win_active(curwin))
	    ins_compl_cancel();

	// If the cursor was moved we didn't just insert a space
	எனில் (arrow_used)
	    inserted_space = FALSE;

	எனில் (can_cindent && cindent_on() && ctrl_x_mode_normal())
	{
force_cindent:
	    /*
	     * Indent now if a key was typed that is in 'cinkeys'.
	     */
	    எனில் (in_cinkeys(c, ' ', வரி_is_white))
	    {
		எனில் (stop_arrow() == OK)
		    // re-indent the current line
		    do_c_கோவை_indent();
	    }
	}

    }	// for (;;)
    // NOTREACHED
}

    முழு
ins_need_undo_get(வெறுமை)
{
    திரும்பு ins_need_undo;
}

/*
 * Redraw for Insert mode.
 * This is postponed until getting the next character to make '$' in the 'cpo'
 * option work correctly.
 * Only redraw when there are no characters available.  This speeds up
 * inserting sequences of characters (e.g., for CTRL-R).
 */
    வெறுமை
ins_redraw(முழு ready)	    // not busy with something
{
#ifdef FEAT_CONCEAL
    linenr_T	conceal_old_cursor_வரி = 0;
    linenr_T	conceal_new_cursor_வரி = 0;
    முழு		conceal_update_வரிகள் = FALSE;
#endif

    எனில் (char_avail())
	திரும்பு;

    // Trigger CursorMoved if the cursor moved.  Not when the popup menu is
    // visible, the command might delete it.
    எனில் (ready && (has_cursormovedI()
#ifdef FEAT_PROP_POPUP
		|| popup_visible
#endif
#எனில் defined(FEAT_CONCEAL)
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
	எனில் (syntax_present(curwin) && must_redraw)
	    update_screen(0);
#endif
	எனில் (has_cursormovedI())
	{
	    // Make sure curswant is correct, an autocommand may call
	    // getcurpos().
	    update_curswant();
	    ins_apply_autocmds(EVENT_CURSORMOVEDI);
	}
#ifdef FEAT_PROP_POPUP
	எனில் (popup_visible)
	    popup_check_cursor_pos();
#endif
#ifdef FEAT_CONCEAL
	எனில் (curwin->w_p_cole > 0)
	{
	    conceal_old_cursor_வரி = last_cursormoved.lnum;
	    conceal_new_cursor_வரி = curwin->w_cursor.lnum;
	    conceal_update_வரிகள் = TRUE;
	}
#endif
	last_cursormoved = curwin->w_cursor;
    }

    // Trigger TextChangedI if b_changedtick_i differs.
    எனில் (ready && has_textchangedI()
	    && curbuf->b_last_changedtick_i != CHANGEDTICK(curbuf)
	    && !pum_visible())
    {
	aco_save_T	aco;
	varnumber_T	tick = CHANGEDTICK(curbuf);

	// Save and restore curwin and curbuf, in case the autocmd changes
	// them.
	aucmd_prepbuf(&aco, curbuf);
	apply_autocmds(EVENT_TEXTCHANGEDI, வெற்று, வெற்று, FALSE, curbuf);
	aucmd_restbuf(&aco);
	curbuf->b_last_changedtick_i = CHANGEDTICK(curbuf);
	எனில் (tick != CHANGEDTICK(curbuf))  // see ins_apply_autocmds()
	    u_save(curwin->w_cursor.lnum,
					(linenr_T)(curwin->w_cursor.lnum + 1));
    }

    // Trigger TextChangedP if b_changedtick_pum differs. When the popupmenu
    // closes TextChangedI will need to trigger for backwards compatibility,
    // thus use different b_last_changedtick* variables.
    எனில் (ready && has_textchangedP()
	    && curbuf->b_last_changedtick_pum != CHANGEDTICK(curbuf)
	    && pum_visible())
    {
	aco_save_T	aco;
	varnumber_T	tick = CHANGEDTICK(curbuf);

	// Save and restore curwin and curbuf, in case the autocmd changes
	// them.
	aucmd_prepbuf(&aco, curbuf);
	apply_autocmds(EVENT_TEXTCHANGEDP, வெற்று, வெற்று, FALSE, curbuf);
	aucmd_restbuf(&aco);
	curbuf->b_last_changedtick_pum = CHANGEDTICK(curbuf);
	எனில் (tick != CHANGEDTICK(curbuf))  // see ins_apply_autocmds()
	    u_save(curwin->w_cursor.lnum,
					(linenr_T)(curwin->w_cursor.lnum + 1));
    }

    எனில் (ready)
	may_trigger_win_scrolled_resized();

    // Trigger SafeState if nothing is pending.
    may_trigger_safestate(ready
	    && !ins_compl_active()
	    && !pum_visible());

#எனில் defined(FEAT_CONCEAL)
    எனில் ((conceal_update_வரிகள்
	    && (conceal_old_cursor_வரி != conceal_new_cursor_வரி
		|| conceal_cursor_வரி(curwin)))
	    || need_cursor_வரி_redraw)
    {
	எனில் (conceal_old_cursor_வரி != conceal_new_cursor_வரி)
	    redrawWinline(curwin, conceal_old_cursor_வரி);
	redrawWinline(curwin, conceal_new_cursor_வரி == 0
			    ? curwin->w_cursor.lnum : conceal_new_cursor_வரி);
	curwin->w_valid &= ~VALID_CROW;
	need_cursor_வரி_redraw = FALSE;
    }
#endif
    எனில் (must_redraw)
	update_screen(0);
    இல்லையெனில் எனில் (clear_cmdline || redraw_cmdline)
	showmode();		// clear cmdline and show mode
    showruler(FALSE);
    setcursor();
    emsg_on_display = FALSE;	// may remove error message now
}

/*
 * Handle a CTRL-V or CTRL-Q typed in Insert mode.
 */
    நிலையான வெறுமை
ins_ctrl_v(வெறுமை)
{
    முழு		c;
    முழு		did_putchar = FALSE;

    // may need to redraw when no more chars available now
    ins_redraw(FALSE);

    எனில் (redrawing() && !char_avail())
    {
	edit_putchar('^', TRUE);
	did_putchar = TRUE;
    }
    AppendToRedobuff((char_u *)CTRL_V_STR);	// CTRL-V

    add_to_showcmd_c(Ctrl_V);

    // Do not change any modifyOtherKeys ESC sequence to a normal key for
    // CTRL-SHIFT-V.
    c = get_literal(mod_mask & MOD_MASK_SHIFT);
    எனில் (did_putchar)
	// when the line fits in 'columns' the '^' is at the start of the next
	// line and will not removed by the redraw
	edit_unputchar();
    clear_showcmd();

    செருகு_special(c, FALSE, TRUE);
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
    நிலையான முழு
decode_Modify_Other_சாவிகள்(முழு c)
{
    char_u  *p = typebuf.tb_buf + typebuf.tb_off;
    முழு	    idx;
    முழு	    form = 0;
    முழு	    argidx = 0;
    முழு	    arg[2] = {0, 0};

    // Recognize:
    // form 0: {lead}{key};{modifier}u
    // form 1: {lead}27;{modifier};{key}~
    எனில் (typebuf.tb_len >= 4 && (c == CSI || (c == ESC && *p == '[')))
    {
	idx = (*p == '[');
	எனில் (p[idx] == '2' && p[idx + 1] == '7' && p[idx + 2] == ';' &&
		kitty_protocol_state != KKPS_ENABLED)
	{
	    form = 1;
	    idx += 3;
	}
	போது (idx < typebuf.tb_len && argidx < 2)
	{
	    எனில் (p[idx] == ';')
		++argidx;
	    இல்லையெனில் எனில் (VIM_ISDIGIT(p[idx]))
		arg[argidx] = arg[argidx] * 10 + (p[idx] - '0');
	    இல்லையெனில்
		நிறுத்து;
	    ++idx;
	}
	முழு kitty_no_mods = argidx == 0 && kitty_protocol_state == KKPS_ENABLED;
	எனில் (idx < typebuf.tb_len
		&& p[idx] == (form == 1 ? '~' : 'u')
		&& (argidx == 1 || kitty_no_mods))
	{
	    // Match, consume the code.
	    typebuf.tb_off += idx + 1;
	    typebuf.tb_len -= idx + 1;
#எனில் defined(FEAT_CLIENTSERVER) || defined(FEAT_EVAL)
	    எனில் (typebuf.tb_len == 0)
		typebuf_was_filled = FALSE;
#endif

	    mod_mask = kitty_no_mods ? 0 : decode_modifiers(arg[!form]);
	    c = merge_modify_Other_சாவிகள்(arg[form], &mod_mask);
	}
    }

    திரும்பு c;
}

/*
 * Put a character directly onto the screen.  It's not stored in a buffer.
 * Used while handling CTRL-K, CTRL-V, etc. in Insert mode.
 */
நிலையான முழு  pc_status;
#define PC_STATUS_UNSET	0	// pc_bytes was not set
#define PC_STATUS_வலது	1	// right half of double-wide char
#define PC_STATUS_இடது	2	// left half of double-wide char
#define PC_STATUS_SET	3	// pc_bytes was filled
நிலையான char_u pc_bytes[MB_MAXBYTES + 1]; // saved bytes
நிலையான முழு  pc_attr;
நிலையான முழு  pc_row;
நிலையான முழு  pc_col;

    வெறுமை
edit_putchar(முழு c, முழு highlight)
{
    முழு	    attr;

    எனில் (Screen_வரிகள் == வெற்று)
	திரும்பு;

    update_topline();	// just in case w_topline isn't valid
    validate_cursor();
    எனில் (highlight)
	attr = HL_ATTR(HLF_8);
    இல்லையெனில்
	attr = 0;
    pc_row = W_WINROW(curwin) + curwin->w_wrow;
    pc_col = curwin->w_wincol;
    pc_status = PC_STATUS_UNSET;
#ifdef FEAT_RIGHTLEFT
    எனில் (curwin->w_p_rl)
    {
	pc_col += curwin->w_width - 1 - curwin->w_wcol;
	எனில் (has_mbyte)
	{
	    முழு fix_col = mb_fix_col(pc_col, pc_row);

	    எனில் (fix_col != pc_col)
	    {
		screen_putchar(' ', pc_row, fix_col, attr);
		--curwin->w_wcol;
		pc_status = PC_STATUS_வலது;
	    }
	}
    }
    இல்லையெனில்
#endif
    {
	pc_col += curwin->w_wcol;
	எனில் (mb_lefthalve(pc_row, pc_col))
	    pc_status = PC_STATUS_இடது;
    }

    // save the character to be able to put it back
    எனில் (pc_status == PC_STATUS_UNSET)
    {
	screen_getbytes(pc_row, pc_col, pc_bytes, &pc_attr);
	pc_status = PC_STATUS_SET;
    }
    screen_putchar(c, pc_row, pc_col, attr);
}

#எனில் defined(FEAT_JOB_CHANNEL)
/*
 * Set the insert start position for when using a prompt buffer.
 */
    வெறுமை
set_insstart(linenr_T lnum, முழு col)
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
    வெறுமை
edit_unputchar(வெறுமை)
{
    எனில் (pc_status != PC_STATUS_UNSET && pc_row >= செய்தி_scrolled)
    {
	எனில் (pc_status == PC_STATUS_வலது)
	    ++curwin->w_wcol;
	எனில் (pc_status == PC_STATUS_வலது || pc_status == PC_STATUS_இடது)
	    redrawWinline(curwin, curwin->w_cursor.lnum);
	இல்லையெனில்
	    screen_puts(pc_bytes, pc_row - செய்தி_scrolled, pc_col, pc_attr);
    }
}

/*
 * Called when "$" is in 'cpoptions': display a '$' at the end of the changed
 * text.  Only works when cursor is in the line that changes.
 */
    வெறுமை
display_dollar(colnr_T col_arg)
{
    colnr_T col = col_arg < 0 ? 0 : col_arg;
    colnr_T save_col;

    எனில் (!redrawing())
	திரும்பு;

    cursor_off();
    save_col = curwin->w_cursor.col;
    curwin->w_cursor.col = col;
    எனில் (has_mbyte)
    {
	char_u *p;

	// If on the last byte of a multi-byte move to the first byte.
	p = ml_get_curline();
	curwin->w_cursor.col -= (*mb_head_off)(p, p + col);
    }
    curs_columns(FALSE);	    // recompute w_wrow and w_wcol
    எனில் (curwin->w_wcol < curwin->w_width)
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
    வெறுமை
undisplay_dollar(வெறுமை)
{
    எனில் (dollar_vcol < 0)
	திரும்பு;

    dollar_vcol = -1;
    redrawWinline(curwin, curwin->w_cursor.lnum);
}

/*
 * Truncate the space at the end of a line.  This is to be used only in an
 * insert mode.  It handles fixing the replace stack for MODE_REPLACE and
 * MODE_VREPLACE modes.
 */
    வெறுமை
truncate_spaces(char_u *வரி, size_t len)
{
    முழு	    i;

    // find start of trailing white space
    க்கு (i = (முழு)len - 1; i >= 0 && VIM_ISWHITE(வரி[i]); i--)
    {
	எனில் (State & REPLACE_FLAG)
	    replace_join(0);	    // remove a NUL from the replace stack
    }
    வரி[i + 1] = NUL;
}

/*
 * Backspace the cursor until the given column.  Handles MODE_REPLACE and
 * MODE_VREPLACE modes correctly.  May also be used when not in insert mode at
 * all.  Will attempt not to go before "col" even when there is a composing
 * character.
 */
    வெறுமை
backspace_until_column(முழு col)
{
    போது ((முழு)curwin->w_cursor.col > col)
    {
	curwin->w_cursor.col--;
	எனில் (State & REPLACE_FLAG)
	    replace_do_bs(col);
	இல்லையெனில் எனில் (!del_char_after_col(col))
	    நிறுத்து;
    }
}

/*
 * Like del_char(), but make sure not to go before column "limit_col".
 * Only matters when there are composing characters.
 * Return TRUE when something was deleted.
 */
    நிலையான முழு
del_char_after_col(முழு limit_col UNUSED)
{
    எனில் (enc_utf8 && limit_col >= 0)
    {
	colnr_T ecol = curwin->w_cursor.col + 1;

	// Make sure the cursor is at the start of a character, but
	// skip forward again when going too far back because of a
	// composing character.
	mb_adjust_cursor();
	போது (curwin->w_cursor.col < (colnr_T)limit_col)
	{
	    முழு l = utf_ptr2len(ml_get_cursor());

	    எனில் (l == 0)  // end of line
		நிறுத்து;
	    curwin->w_cursor.col += l;
	}
	எனில் (*ml_get_cursor() == NUL || curwin->w_cursor.col == ecol)
	    திரும்பு FALSE;
	del_bytes((long)((முழு)ecol - curwin->w_cursor.col), FALSE, TRUE);
    }
    இல்லையெனில்
	(வெறுமை)del_char(FALSE);
    திரும்பு TRUE;
}

/*
 * Next character is interpreted literally.
 * A one, two or three digit decimal number is interpreted as its byte value.
 * If one or two digits are entered, the next character is given to vungetc().
 * For Unicode a character > 255 may be returned.
 * If "noReduceKeys" is TRUE do not change any modifyOtherKeys ESC sequence
 * into a normal key, return ESC.
 */
    முழு
get_literal(முழு no_Reduce_சாவிகள்)
{
    முழு		cc;
    முழு		nc;
    முழு		i;
    முழு		hex = FALSE;
    முழு		octal = FALSE;
    முழு		unicode = 0;

    எனில் (got_int)
	திரும்பு Ctrl_C;

#ifdef FEAT_GUI
    /*
     * In GUI there is no point inserting the internal code for a special key.
     * It is more useful to insert the string "<KEY>" instead.	This would
     * probably be useful in a text window too, but it would not be
     * vi-compatible (maybe there should be an option for it?) -- webb
     */
    எனில் (gui.in_use)
    {
	++allow_சாவிகள்;
	எனில் (no_Reduce_சாவிகள்)
	    ++no_reduce_சாவிகள்;
    }
#endif
#ifdef USE_ON_FLY_SCROLL
    dont_scroll = TRUE;		// disallow scrolling here
#endif
    ++no_mapping;		// don't map the next key hits
    cc = 0;
    i = 0;
    க்கு (;;)
    {
	nc = plain_vgetc();
	எனில் ((nc == ESC || nc == CSI) && !no_Reduce_சாவிகள்)
	    nc = decode_Modify_Other_சாவிகள்(nc);

	எனில் ((mod_mask & ~MOD_MASK_SHIFT) != 0)
	    // A character with non-Shift modifiers should not be a valid
	    // character for i_CTRL-V_digit.
	    நிறுத்து;

	எனில் ((State & MODE_CMDLINE) == 0 && MB_BYTE2LEN_CHECK(nc) == 1)
	    add_to_showcmd(nc);
	எனில் (nc == 'x' || nc == 'X')
	    hex = TRUE;
	இல்லையெனில் எனில் (nc == 'o' || nc == 'O')
	    octal = TRUE;
	இல்லையெனில் எனில் (nc == 'u' || nc == 'U')
	    unicode = nc;
	இல்லையெனில்
	{
	    எனில் (hex || unicode != 0)
	    {
		எனில் (!vim_isxdigit(nc))
		    நிறுத்து;
		cc = cc * 16 + hex2nr(nc);
	    }
	    இல்லையெனில் எனில் (octal)
	    {
		எனில் (nc < '0' || nc > '7')
		    நிறுத்து;
		cc = cc * 8 + nc - '0';
	    }
	    இல்லையெனில்
	    {
		எனில் (!VIM_ISDIGIT(nc))
		    நிறுத்து;
		cc = cc * 10 + nc - '0';
	    }

	    ++i;
	}

	எனில் (cc > 255 && unicode == 0)
	    cc = 255;		// limit range to 0-255
	nc = 0;

	எனில் (hex)		// hex: up to two chars
	{
	    எனில் (i >= 2)
		நிறுத்து;
	}
	இல்லையெனில் எனில் (unicode)	// Unicode: up to four or eight chars
	{
	    எனில் ((unicode == 'u' && i >= 4) || (unicode == 'U' && i >= 8))
		நிறுத்து;
	}
	இல்லையெனில் எனில் (i >= 3)	// decimal or octal: up to three chars
	    நிறுத்து;
    }
    எனில் (i == 0)	    // no number entered
    {
	எனில் (nc == K_ZERO)   // NUL is stored as NL
	{
	    cc = '\n';
	    nc = 0;
	}
	இல்லையெனில்
	{
	    cc = nc;
	    nc = 0;
	}
    }

    எனில் (cc == 0)	// NUL is stored as NL
	cc = '\n';
    எனில் (enc_dbcs && (cc & 0xff) == 0)
	cc = '?';	// don't accept an illegal DBCS char, the NUL in the
			// second byte will cause trouble!

    --no_mapping;
#ifdef FEAT_GUI
    எனில் (gui.in_use)
    {
	--allow_சாவிகள்;
	எனில் (no_Reduce_சாவிகள்)
	    --no_reduce_சாவிகள்;
    }
#endif
    எனில் (nc)
    {
	vungetc(nc);
	// A character typed with i_CTRL-V_digit cannot have modifiers.
	mod_mask = 0;
    }
    got_int = FALSE;	    // CTRL-C typed after CTRL-V is not an interrupt
    திரும்பு cc;
}

/*
 * Insert character, taking care of special keys and mod_mask
 */
    நிலையான வெறுமை
செருகு_special(
    முழு	    c,
    முழு	    allow_modmask,
    முழு	    ctrlv)	    // c was typed after CTRL-V
{
    char_u  *p;
    முழு	    len;

    /*
     * Special function key, translate into "<Key>". Up to the last '>' is
     * inserted with ins_str(), so as not to replace characters in replace
     * mode.
     * Only use mod_mask for special keys, to avoid things like <S-Space>,
     * unless 'allow_modmask' is TRUE.
     */
#எனில் defined(MACOS_X) || defined(FEAT_GUI_GTK)
    // Command-key never produces a normal key
    எனில் (mod_mask & MOD_MASK_CMD)
	allow_modmask = TRUE;
#endif
    எனில் (IS_SPECIAL(c) || (mod_mask && allow_modmask))
    {
	p = get_special_சாவி_name(c, mod_mask);
	len = (முழு)STRLEN(p);
	c = p[len - 1];
	எனில் (len > 2)
	{
	    எனில் (stop_arrow() == FAIL)
		திரும்பு;
	    p[len - 1] = NUL;
	    ins_str(p, len - 1);
	    AppendToRedobuffLit(p, -1);
	    ctrlv = FALSE;
	}
    }
    எனில் (stop_arrow() == OK)
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
    வெறுமை
insertchar(
    முழு		c,			// character to insert or NUL
    முழு		flags,			// INSCHAR_FORMAT, etc.
    முழு		second_indent)		// indent for second line if >= 0
{
    முழு		textwidth;
    char_u	*p;
    முழு		fo_ins_blank;
    முழு		force_format = flags & INSCHAR_FORMAT;

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
    எனில் (textwidth > 0
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
#எனில் defined(FEAT_EVAL)
	முழு     do_internal = TRUE;
	colnr_T virtcol = get_nolist_virtcol()
				  + char2cells(c != NUL ? c : gchar_cursor());

	எனில் (*curbuf->b_p_fex != NUL && (flags & INSCHAR_NO_FEX) == 0
		&& (force_format || virtcol > (colnr_T)textwidth))
	{
	    do_internal = (fex_format(curwin->w_cursor.lnum, 1L, c) != 0);
	    // It may be required to save for undo again, e.g. when setline()
	    // was called.
	    ins_need_undo = TRUE;
	}
	எனில் (do_internal)
#endif
	    internal_format(textwidth, second_indent, flags, c == NUL, c);
    }

    எனில் (c == NUL)	    // only formatting was wanted
	திரும்பு;

    // Check whether this character should end a comment.
    எனில் (did_ai && c == end_comment_pending)
    {
	char_u  *வரி;
	char_u	lead_end[COM_MAX_LEN];	    // end-comment string
	முழு	நடு_len, end_len;
	முழு	i;

	/*
	 * Need to remove existing (middle) comment leader and insert end
	 * comment leader.  First, check what comment leader we can find.
	 */
	i = get_leader_len(வரி = ml_get_curline(), &p, FALSE, TRUE);
	எனில் (i > 0 && vim_strchr(p, COM_நடு) != வெற்று)	// Just checking
	{
	    // Skip middle-comment string
	    போது (*p && p[-1] != ':')	// find end of middle flags
		++p;
	    நடு_len = copy_option_part(&p, lead_end, COM_MAX_LEN, ",");
	    // Don't count trailing white space for middle_len
	    போது (நடு_len > 0 && VIM_ISWHITE(lead_end[நடு_len - 1]))
		--நடு_len;

	    // Find the end-comment string
	    போது (*p && p[-1] != ':')	// find end of end flags
		++p;
	    end_len = copy_option_part(&p, lead_end, COM_MAX_LEN, ",");

	    // Skip white space before the cursor
	    i = curwin->w_cursor.col;
	    போது (--i >= 0 && VIM_ISWHITE(வரி[i]))
		;
	    i++;

	    // Skip to before the middle leader
	    i -= நடு_len;

	    // Check some expected things before we go on
	    எனில் (i >= 0 && end_len > 0 && lead_end[end_len - 1] == end_comment_pending)
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

    எனில் (       !ISSPECIAL(c)
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
#define உள்ளீடு_BUFLEN 100
	char_u		buf[உள்ளீடு_BUFLEN + 1];
	முழு		i;
	colnr_T		virtcol = 0;

	buf[0] = c;
	i = 1;
	எனில் (textwidth > 0)
	    virtcol = get_nolist_virtcol();
	/*
	 * Stop the string when:
	 * - no more chars available
	 * - finding a special character (command key)
	 * - buffer is full
	 * - running into the 'textwidth' boundary
	 * - need to check for abbreviation: A non-word char after a word-char
	 */
	போது (	   (c = vpeekc()) != NUL
		&& !ISSPECIAL(c)
		&& (!has_mbyte || MB_BYTE2LEN_CHECK(c) == 1)
		&& i < உள்ளீடு_BUFLEN
		&& (textwidth == 0
		    || (virtcol += byte2cells(buf[i - 1])) < (colnr_T)textwidth)
		&& !(!no_abbr && !vim_iswordc(c) && vim_iswordc(buf[i - 1])))
	{
#ifdef FEAT_RIGHTLEFT
	    c = vgetc();
	    எனில் (p_hkmap && சாவி_Typed)
		c = hkmap(c);		    // Hebrew mode mapping
	    buf[i++] = c;
#இல்லையெனில்
	    buf[i++] = vgetc();
#endif
	}

#ifdef FEAT_DIGRAPHS
	do_digraph(-1);			// clear digraphs
	do_digraph(buf[i-1]);		// may be the start of a digraph
#endif
	buf[i] = NUL;
	ins_str(buf, i);
	எனில் (flags & INSCHAR_CTRLV)
	{
	    redo_literal(*buf);
	    i = 1;
	}
	இல்லையெனில்
	    i = 0;
	எனில் (buf[i] != NUL)
	    AppendToRedobuffLit(buf + i, -1);
    }
    இல்லையெனில்
    {
	முழு		cc;

	எனில் (has_mbyte && (cc = (*mb_char2len)(c)) > 1)
	{
	    char_u	buf[MB_MAXBYTES + 1];

	    (*mb_char2bytes)(c, buf);
	    buf[cc] = NUL;
	    ins_char_bytes(buf, cc);
	    AppendCharToRedobuff(c);
	}
	இல்லையெனில்
	{
	    ins_char(c);
	    எனில் (flags & INSCHAR_CTRLV)
		redo_literal(c);
	    இல்லையெனில்
		AppendCharToRedobuff(c);
	}
    }
}

/*
 * Put a character in the redo buffer, for when just after a CTRL-V.
 */
    நிலையான வெறுமை
redo_literal(முழு c)
{
    char_u	buf[10];

    // Only digits need special treatment.  Translate them into a string of
    // three digits.
    எனில் (VIM_ISDIGIT(c))
    {
	vim_snprintf((எழுத்து *)buf, அளவு(buf), "%03d", c);
	AppendToRedobuff(buf);
    }
    இல்லையெனில்
	AppendCharToRedobuff(c);
}

/*
 * start_arrow() is called when an arrow key is used in insert mode.
 * For undo/redo it resembles hitting the <ESC> key.
 */
    வெறுமை
start_arrow(
    pos_T    *end_செருகு_pos)		// can be NULL
{
    start_arrow_common(end_செருகு_pos, TRUE);
}

/*
 * Like start_arrow() but with end_change argument.
 * Will prepare for redo of CTRL-G U if "end_change" is FALSE.
 */
    நிலையான வெறுமை
start_arrow_with_change(
    pos_T    *end_செருகு_pos,		// can be NULL
    முழு	      end_change)		// end undoable change
{
    start_arrow_common(end_செருகு_pos, end_change);
    எனில் (!end_change)
    {
	AppendCharToRedobuff(Ctrl_G);
	AppendCharToRedobuff('U');
    }
}

    நிலையான வெறுமை
start_arrow_common(
    pos_T    *end_செருகு_pos,		// can be NULL
    முழு	      end_change)		// end undoable change
{
    எனில் (!arrow_used && end_change)	// something has been inserted
    {
	AppendToRedobuff(ESC_STR);
	stop_செருகு(end_செருகு_pos, FALSE, FALSE);
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
    நிலையான வெறுமை
check_spell_redraw(வெறுமை)
{
    எனில் (spell_redraw_lnum != 0)
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
    முழு
stop_arrow(வெறுமை)
{
    எனில் (arrow_used)
    {
	Insstart = curwin->w_cursor;	// new insertion starts here
	எனில் (Insstart.col > Insstart_orig.col && !ins_need_undo)
	    // Don't update the original insert position when moved to the
	    // right, except when nothing was inserted yet.
	    update_Insstart_orig = FALSE;
	Insstart_textlen = (colnr_T)linetabsize_str(ml_get_curline());

	எனில் (u_save_cursor() == OK)
	{
	    arrow_used = FALSE;
	    ins_need_undo = FALSE;
	}

	ai_col = 0;
	எனில் (State & VREPLACE_FLAG)
	{
	    orig_வரி_மொத்தம் = curbuf->b_ml.ml_வரி_மொத்தம்;
	    vr_வரிகள்_changed = 1;
	}
	ResetRedobuff();
	AppendToRedobuff((char_u *)"1i");   // pretend we start an insertion
	new_செருகு_skip = 2;
    }
    இல்லையெனில் எனில் (ins_need_undo)
    {
	எனில் (u_save_cursor() == OK)
	{
	    // A command or event may have moved the cursor before the next
	    // edit. Pull Insstart back only when the cursor moved above it,
	    // so that later edits can properly decide whether an extra undo
	    // entry is needed. Advancing Insstart would mis-place '[ after a
	    // register paste.
	    எனில் (LT_POS(curwin->w_cursor, Insstart))
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

    திரும்பு (arrow_used || ins_need_undo ? FAIL : OK);
}

/*
 * Do a few things to stop inserting.
 * "end_insert_pos" is where insert ended.  It is NULL when we already jumped
 * to another window/buffer.
 */
    நிலையான வெறுமை
stop_செருகு(
    pos_T	*end_செருகு_pos,
    முழு		esc,			// called by ins_esc()
    முழு		nomove)			// <c-\><c-o>, don't move cursor
{
    முழு		cc;
    string_T	inserted;

    stop_redo_ins();
    replace_flush();		// abandon replace stack

    /*
     * Save the inserted text for later redo with ^@ and CTRL-A.
     * Don't do it when "restart_edit" was set and nothing was inserted,
     * otherwise CTRL-O w and then <Left> will clear "last_insert".
     */
    inserted = get_inserted();
    முழு added = inserted.string == வெற்று ? 0 : (முழு)inserted.length - new_செருகு_skip;
    எனில் (did_restart_edit == 0 || added > 0)
    {
	vim_free(last_செருகு.string);
	last_செருகு = inserted;			    // structure copy
	last_செருகு_skip = added < 0 ? 0 : new_செருகு_skip;
    }
    இல்லையெனில்
	vim_free(inserted.string);

    எனில் (!arrow_used && end_செருகு_pos != வெற்று)
    {
	// Auto-format now.  It may seem strange to do this when stopping an
	// insertion (or moving the cursor), but it's required when appending
	// a line and having it end in a space.  But only do it when something
	// was actually inserted, otherwise undo won't work.
	எனில் (!ins_need_undo && has_format_option(FO_AUTO))
	{
	    pos_T   tpos = curwin->w_cursor;

	    // When the cursor is at the end of the line after a space the
	    // formatting will move it to the following word.  Avoid that by
	    // moving the cursor onto the space.
	    cc = 'x';
	    எனில் (curwin->w_cursor.col > 0 && gchar_cursor() == NUL)
	    {
		dec_cursor();
		cc = gchar_cursor();
		எனில் (!VIM_ISWHITE(cc))
		    curwin->w_cursor = tpos;
	    }

	    auto_format(TRUE, FALSE);

	    எனில் (VIM_ISWHITE(cc))
	    {
		எனில் (gchar_cursor() != NUL)
		    inc_cursor();
		// If the cursor is still at the same character, also keep
		// the "coladd".
		எனில் (gchar_cursor() == NUL
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
	எனில் (!nomove && did_ai && (esc || (vim_strchr(p_cpo, CPO_INDENT) == வெற்று
			&& curwin->w_cursor.lnum != end_செருகு_pos->lnum))
		&& end_செருகு_pos->lnum <= curbuf->b_ml.ml_வரி_மொத்தம்)
	{
	    pos_T	tpos = curwin->w_cursor;
	    colnr_T	prev_col = end_செருகு_pos->col;
	    colnr_T	strip_col;

	    curwin->w_cursor = *end_செருகு_pos;
	    check_cursor_col();  // make sure it is not past the line

	    // Where the loop would actually start (back up if on NUL).
	    strip_col = curwin->w_cursor.col;
	    எனில் (gchar_cursor() == NUL && strip_col > 0)
		--strip_col;

	    // Don't strip if non-whitespace follows: setline() from a <Cmd>
	    // mapping or CursorHoldI autocmd may have inserted content.
	    எனில் (*skipwhite(ml_get_curline() + strip_col) == NUL)
	    {
		curwin->w_cursor.col = strip_col;
		க்கு (;;)
		{
		    எனில் (gchar_cursor() == NUL && curwin->w_cursor.col > 0)
			--curwin->w_cursor.col;
		    cc = gchar_cursor();
		    எனில் (!VIM_ISWHITE(cc))
			நிறுத்து;
		    எனில் (del_char(TRUE) == FAIL)
			நிறுத்து;  // should not happen
		}
		எனில் (curwin->w_cursor.lnum != tpos.lnum)
		    curwin->w_cursor = tpos;
		இல்லையெனில் எனில் (curwin->w_cursor.col < prev_col)
		{
		    // reset tpos, could have been invalidated in the loop above
		    tpos = curwin->w_cursor;
		    tpos.col++;
		    எனில் (cc != NUL && gchar_pos(&tpos) == NUL)
			++curwin->w_cursor.col;	// put cursor back on the NUL
		}

		// <C-S-Right> may have started Visual mode, adjust the position for
		// deleted characters.
		எனில் (VIsual_active)
		    check_visual_pos();
	    }
	    இல்லையெனில்
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
    எனில் (end_செருகு_pos != வெற்று)
    {
	curbuf->b_op_start = Insstart;
	curbuf->b_op_start_orig = Insstart_orig;
	curbuf->b_op_end = *end_செருகு_pos;
    }
}

/*
 * Set the last inserted text to a single character.
 * Used for the replace command.
 */
    வெறுமை
set_last_செருகு(முழு c)
{
    char_u	*s;

    vim_free(last_செருகு.string);
    last_செருகு.string = alloc(MB_MAXBYTES * 3 + 5);
    எனில் (last_செருகு.string == வெற்று)
    {
	last_செருகு.length = 0;
	திரும்பு;
    }

    s = last_செருகு.string;
    // Use the CTRL-V only when entering a special char
    எனில் (c < ' ' || c == DEL)
	*s++ = Ctrl_V;
    s = add_char2buf(c, s);
    *s++ = ESC;
    *s = NUL;
    last_செருகு.length = (size_t)(s - last_செருகு.string);
    last_செருகு_skip = 0;
}

#எனில் defined(EXITFREE)
    வெறுமை
free_last_செருகு(வெறுமை)
{
    VIM_CLEAR_STRING(last_செருகு);
}
#endif

/*
 * Add character "c" to buffer "s".  Escape the special meaning of K_SPECIAL
 * and CSI.  Handle multi-byte characters.
 * Returns a pointer to after the added bytes.
 */
    char_u *
add_char2buf(முழு c, char_u *s)
{
    char_u	temp[MB_MAXBYTES + 1];
    முழு		i;
    முழு		len;

    len = (*mb_char2bytes)(c, temp);
    க்கு (i = 0; i < len; ++i)
    {
	c = temp[i];
	// Need to escape K_SPECIAL and CSI like in the typeahead buffer.
	எனில் (c == K_SPECIAL)
	{
	    *s++ = K_SPECIAL;
	    *s++ = KS_SPECIAL;
	    *s++ = KE_FILLER;
	}
#ifdef FEAT_GUI
	இல்லையெனில் எனில் (c == CSI)
	{
	    *s++ = CSI;
	    *s++ = KS_EXTRA;
	    *s++ = (முழு)KE_CSI;
	}
#endif
	இல்லையெனில்
	    *s++ = c;
    }
    திரும்பு s;
}

/*
 * move cursor to start of line
 * if flags & BL_WHITE	move to first non-white
 * if flags & BL_SOL	move to first non-white if startofline is set,
 *			    otherwise keep "curswant" column
 * if flags & BL_FIX	don't leave the cursor on a NUL.
 */
    வெறுமை
beginline(முழு flags)
{
    எனில் ((flags & BL_SOL) && !p_sol)
	coladvance(curwin->w_curswant);
    இல்லையெனில்
    {
	curwin->w_cursor.col = 0;
	curwin->w_cursor.coladd = 0;

	எனில் (flags & (BL_WHITE | BL_SOL))
	{
	    char_u  *ptr;

	    க்கு (ptr = ml_get_curline(); VIM_ISWHITE(*ptr)
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

    முழு
oneright(வெறுமை)
{
    char_u	*ptr;
    முழு		l;

    எனில் (virtual_active())
    {
	pos_T	prevpos = curwin->w_cursor;

	// Adjust for multi-wide char (excluding TAB)
	ptr = ml_get_cursor();
	coladvance(getviscol() + ((*ptr != TAB
					  && vim_isprintc((*mb_ptr2char)(ptr)))
		    ? ptr2cells(ptr) : 1));
	curwin->w_set_curswant = true;
	// Return OK if the cursor moved, FAIL otherwise (at window edge).
	திரும்பு (prevpos.col != curwin->w_cursor.col
		    || prevpos.coladd != curwin->w_cursor.coladd) ? OK : FAIL;
    }

    ptr = ml_get_cursor();
    எனில் (*ptr == NUL)
	திரும்பு FAIL;	    // already at the very end

    எனில் (has_mbyte)
	l = (*mb_ptr2len)(ptr);
    இல்லையெனில்
	l = 1;

    // move "l" bytes right, but don't end up on the NUL, unless 'virtualedit'
    // contains "onemore".
    எனில் (ptr[l] == NUL && (get_ve_flags() & VE_ONEMORE) == 0)
	திரும்பு FAIL;
    curwin->w_cursor.col += l;

    curwin->w_set_curswant = true;
    adjust_skipcol();
    திரும்பு OK;
}

    முழு
oneleft(வெறுமை)
{
    எனில் (virtual_active())
    {
#ifdef FEAT_LINEBREAK
	முழு width;
#endif
	முழு v = getviscol();

	எனில் (v == 0)
	    திரும்பு FAIL;

#ifdef FEAT_LINEBREAK
	// We might get stuck on 'showbreak', skip over it.
	width = 1;
	க்கு (;;)
	{
	    coladvance(v - width);
	    // getviscol() is slow, skip it when 'showbreak' is empty,
	    // 'breakindent' is not set and there are no multi-byte
	    // characters
	    எனில் ((*get_showbreak_மதிப்பு(curwin) == NUL && !curwin->w_p_bri
					     && !has_mbyte) || getviscol() < v)
		நிறுத்து;
	    ++width;
	}
#இல்லையெனில்
	coladvance(v - 1);
#endif

	எனில் (curwin->w_cursor.coladd == 1)
	{
	    char_u *ptr;

	    // Adjust for multi-wide char (not a TAB)
	    ptr = ml_get_cursor();
	    எனில் (*ptr != TAB && vim_isprintc((*mb_ptr2char)(ptr))
							 && ptr2cells(ptr) > 1)
		curwin->w_cursor.coladd = 0;
	}

	curwin->w_set_curswant = true;
	adjust_skipcol();
	திரும்பு OK;
    }

    எனில் (curwin->w_cursor.col == 0)
	திரும்பு FAIL;

    curwin->w_set_curswant = true;
    --curwin->w_cursor.col;

    // if the character on the left of the current cursor is a multi-byte
    // character, move to its first byte
    எனில் (has_mbyte)
	mb_adjust_cursor();
    adjust_skipcol();
    திரும்பு OK;
}

/*
 * Move the cursor up "n" lines in window "wp".
 * Takes care of closed folds.
 */
    வெறுமை
cursor_up_inner(win_T *wp, long n)
{
    linenr_T	lnum = wp->w_cursor.lnum;

    எனில் (n >= lnum)
	lnum = 1;
    இல்லையெனில்
#ifdef FEAT_FOLDING
	எனில் (hasAnyFolding(wp))
    {
	/*
	 * Count each sequence of folded lines as one logical line.
	 */
	// go to the start of the current fold
	(வெறுமை)hasFoldingWin(wp, lnum, &lnum, வெற்று, TRUE, வெற்று);

	போது (n--)
	{
	    // move up one line
	    --lnum;
	    எனில் (lnum <= 1)
		நிறுத்து;
	    // If we entered a fold, move to the beginning, unless in
	    // Insert mode or when 'foldopen' contains "all": it will open
	    // in a moment.
	    எனில் (n > 0 || !((State & MODE_செருகு) || (fdo_flags & FDO_ALL)))
		(வெறுமை)hasFoldingWin(wp, lnum, &lnum, வெற்று, TRUE, வெற்று);
	}
	எனில் (lnum < 1)
	    lnum = 1;
    }
    இல்லையெனில்
#endif
	lnum -= n;

    wp->w_cursor.lnum = lnum;
}

    முழு
cursor_up(
    long	n,
    முழு		upd_topline)	    // When TRUE: update topline
{
    // This fails if the cursor is already in the first line or the count is
    // larger than the line number and '-' is in 'cpoptions'
    linenr_T lnum = curwin->w_cursor.lnum;
    எனில் (n > 0 && (lnum <= 1
		       || (n >= lnum && vim_strchr(p_cpo, CPO_MINUS) != வெற்று)))
	திரும்பு FAIL;
    cursor_up_inner(curwin, n);

    // try to advance to the column we want to be at
    coladvance(curwin->w_curswant);

    எனில் (upd_topline)
	update_topline();	// make sure curwin->w_topline is valid

    திரும்பு OK;
}

/*
 * Move the cursor down "n" lines in window "wp".
 * Takes care of closed folds.
 */
    வெறுமை
cursor_down_inner(win_T *wp, long n)
{
    linenr_T	lnum = wp->w_cursor.lnum;
    linenr_T	வரி_மொத்தம் = wp->w_buffer->b_ml.ml_வரி_மொத்தம்;

    எனில் (lnum + n >= வரி_மொத்தம்)
	lnum = வரி_மொத்தம்;
    இல்லையெனில்
#ifdef FEAT_FOLDING
	எனில் (hasAnyFolding(wp))
    {
	linenr_T	last;

	// count each sequence of folded lines as one logical line
	போது (n--)
	{
	    எனில் (hasFoldingWin(wp, lnum, வெற்று, &last, TRUE, வெற்று))
		lnum = last + 1;
	    இல்லையெனில்
		++lnum;
	    எனில் (lnum >= வரி_மொத்தம்)
		நிறுத்து;
	}
	எனில் (lnum > வரி_மொத்தம்)
	    lnum = வரி_மொத்தம்;
    }
    இல்லையெனில்
#endif
	lnum += n;

    wp->w_cursor.lnum = lnum;
}

/*
 * Cursor down a number of logical lines.
 */
    முழு
cursor_down(
    long	n,
    முழு		upd_topline)	    // When TRUE: update topline
{
    linenr_T	lnum = curwin->w_cursor.lnum;
    linenr_T	வரி_மொத்தம் = curwin->w_buffer->b_ml.ml_வரி_மொத்தம்;
    // This fails if the cursor is already in the last (folded) line, or would
    // move beyond the last line and '-' is in 'cpoptions'.
#ifdef FEAT_FOLDING
    hasFoldingWin(curwin, lnum, வெற்று, &lnum, TRUE, வெற்று);
#endif
    எனில் (n > 0
	    && (lnum >= வரி_மொத்தம்
		|| (lnum + n > வரி_மொத்தம்
				     && vim_strchr(p_cpo, CPO_MINUS) != வெற்று)))
	திரும்பு FAIL;
    cursor_down_inner(curwin, n);

    // try to advance to the column we want to be at
    coladvance(curwin->w_curswant);

    எனில் (upd_topline)
	update_topline();	// make sure curwin->w_topline is valid

    திரும்பு OK;
}

/*
 * Stuff the last inserted text in the read buffer.
 * Last_insert actually is a copy of the redo buffer, so we
 * first have to remove the command.
 */
    முழு
stuff_inserted(
    முழு	    c,		// Command character to be inserted
    long    மொத்தம்,	// Repeat this many times
    முழு	    no_esc)	// Don't add an ESC at the end
{
    string_T	செருகு;				// text to be inserted
    char_u	last = ' ';

    செருகு = get_last_செருகு();
    எனில் (செருகு.string == வெற்று)
    {
	emsg(_(e_no_inserted_text_yet));
	திரும்பு FAIL;
    }

    // may want to stuff the command character, to start Insert mode
    எனில் (c != NUL)
	stuffcharReadbuff(c);

    எனில் (செருகு.length > 0)
    {
	char_u	*p;

	// look for the last ESC in 'insert'
	க்கு (p = செருகு.string + செருகு.length - 1; p >= செருகு.string; --p)
	{
	    எனில் (*p == ESC)
	    {
		செருகு.length = (size_t)(p - செருகு.string);
		நிறுத்து;
	    }
	}
    }

    எனில் (செருகு.length > 0)
    {
	char_u	*p = செருகு.string + செருகு.length - 1;

	// when the last char is either "0" or "^" it will be quoted if no ESC
	// comes after it OR if it will insert more than once and "ptr"
	// starts with ^D.	-- Acevedo
	எனில் ((*p == '0' || *p == '^')
		&& (no_esc || (*செருகு.string == Ctrl_D && மொத்தம் > 1)))
	{
	    last = *p;
	    --செருகு.length;
	}
    }

    செய்
    {
	stuffReadbuffLen(செருகு.string, (long)செருகு.length);
	// a trailing "0" is inserted as "<C-V>048", "^" as "<C-V>^"
	தேர்வு (last)
	{
	நிலை '0':
#define TEXT_TO_செருகு "\026\060\064\070"
	    stuffReadbuffLen((char_u *)TEXT_TO_செருகு, STRLEN_LITERAL(TEXT_TO_செருகு));
#undef TEXT_TO_செருகு
	    நிறுத்து;

	நிலை '^':
#define TEXT_TO_செருகு "\026^"
	    stuffReadbuffLen((char_u *)TEXT_TO_செருகு, STRLEN_LITERAL(TEXT_TO_செருகு));
#undef TEXT_TO_செருகு
	    நிறுத்து;

	இயல்பு:
	    நிறுத்து;
	}
    }
    போது (--மொத்தம் > 0);

    // may want to stuff a trailing ESC, to get out of Insert mode
    எனில் (!no_esc)
	stuffcharReadbuff(ESC);

    திரும்பு OK;
}

    string_T
get_last_செருகு(வெறுமை)
{
    string_T செருகு = {வெற்று, 0};

    எனில் (last_செருகு.string != வெற்று)
    {
	செருகு.string = last_செருகு.string + last_செருகு_skip;
	செருகு.length = (size_t)(last_செருகு.length - last_செருகு_skip);
    }

    திரும்பு செருகு;
}

/*
 * Get last inserted string, and remove trailing <Esc>.
 * Returns pointer to allocated memory (must be freed) or NULL.
 */
    char_u *
get_last_செருகு_save(வெறுமை)
{
    string_T	செருகு = get_last_செருகு();
    char_u	*s;

    எனில் (செருகு.string == வெற்று)
	திரும்பு வெற்று;
    s = vim_strnsave(செருகு.string, செருகு.length);
    எனில் (s == வெற்று)
	திரும்பு வெற்று;

    எனில் (செருகு.length > 0 && s[செருகு.length - 1] == ESC)	// remove trailing ESC
	s[--செருகு.length] = NUL;
    திரும்பு s;
}

/*
 * Check the word in front of the cursor for an abbreviation.
 * Called when the non-id character "c" has been entered.
 * When an abbreviation is recognized it is removed from the text and
 * the replacement string is inserted in typebuf.tb_buf[], followed by "c".
 */
    நிலையான முழு
echeck_abbr(முழு c)
{
    // Don't check for abbreviation in paste mode, when disabled and just
    // after moving around with cursor keys.
    எனில் (p_paste || no_abbr || arrow_used)
	திரும்பு FALSE;

    திரும்பு check_abbr(c, ml_get_curline(), curwin->w_cursor.col,
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

நிலையான char_u	*replace_அடுக்கு = வெற்று;
நிலையான long	replace_அடுக்கு_nr = 0;	    // next entry in replace stack
நிலையான long	replace_அடுக்கு_len = 0;	    // max. number of entries

    வெறுமை
replace_தள்ளு(
    முழு	    c)	    // character that is replaced (NUL is none)
{
    char_u  *p;

    எனில் (replace_அடுக்கு_nr < replace_offset)	// nothing to do
	திரும்பு;
    எனில் (replace_அடுக்கு_len <= replace_அடுக்கு_nr)
    {
	replace_அடுக்கு_len += 50;
	p = ALLOC_MULT(char_u, replace_அடுக்கு_len);
	எனில் (p == வெற்று)	    // out of memory
	{
	    replace_அடுக்கு_len -= 50;
	    திரும்பு;
	}
	எனில் (replace_அடுக்கு != வெற்று)
	{
	    mch_memmove(p, replace_அடுக்கு,
				 (size_t)(replace_அடுக்கு_nr * அளவு(char_u)));
	    vim_free(replace_அடுக்கு);
	}
	replace_அடுக்கு = p;
    }
    p = replace_அடுக்கு + replace_அடுக்கு_nr - replace_offset;
    எனில் (replace_offset)
	mch_memmove(p + 1, p, (size_t)(replace_offset * அளவு(char_u)));
    *p = c;
    ++replace_அடுக்கு_nr;
}

/*
 * Push a character onto the replace stack.  Handles a multi-byte character in
 * reverse byte order, so that the first byte is popped off first.
 * Return the number of bytes done (includes composing characters).
 */
    முழு
replace_தள்ளு_mb(char_u *p)
{
    முழு l = (*mb_ptr2len)(p);
    முழு j;

    க்கு (j = l - 1; j >= 0; --j)
	replace_தள்ளு(p[j]);
    திரும்பு l;
}

/*
 * Pop one item from the replace stack.
 * return -1 if stack empty
 * return replaced character or NUL otherwise
 */
    நிலையான முழு
replace_எடு(வெறுமை)
{
    எனில் (replace_அடுக்கு_nr == 0)
	திரும்பு -1;
    திரும்பு (முழு)replace_அடுக்கு[--replace_அடுக்கு_nr];
}

/*
 * Join the top two items on the replace stack.  This removes to "off"'th NUL
 * encountered.
 */
    வெறுமை
replace_join(
    முழு	    off)	// offset for which NUL to remove
{
    முழு	    i;

    க்கு (i = replace_அடுக்கு_nr; --i >= 0; )
	எனில் (replace_அடுக்கு[i] == NUL && off-- <= 0)
	{
	    --replace_அடுக்கு_nr;
	    mch_memmove(replace_அடுக்கு + i, replace_அடுக்கு + i + 1,
					      (size_t)(replace_அடுக்கு_nr - i));
	    திரும்பு;
	}
}

/*
 * Pop bytes from the replace stack until a NUL is found, and insert them
 * before the cursor.  Can only be used in MODE_REPLACE or MODE_VREPLACE state.
 */
    நிலையான வெறுமை
replace_எடு_ins(வெறுமை)
{
    முழு	    cc;
    முழு	    oldState = State;

    State = MODE_NORMAL;			// don't want MODE_REPLACE here
    போது ((cc = replace_எடு()) > 0)
    {
	mb_replace_எடு_ins(cc);
	dec_cursor();
    }
    State = oldState;
}

/*
 * Insert bytes popped from the replace stack. "cc" is the first byte.  If it
 * indicates a multi-byte char, pop the other bytes too.
 */
    நிலையான வெறுமை
mb_replace_எடு_ins(முழு cc)
{
    முழு		n;
    char_u	buf[MB_MAXBYTES + 1];
    முழு		i;
    முழு		c;

    எனில் (has_mbyte && (n = MB_BYTE2LEN(cc)) > 1)
    {
	buf[0] = cc;
	க்கு (i = 1; i < n; ++i)
	    buf[i] = replace_எடு();
	ins_bytes_len(buf, n);
    }
    இல்லையெனில்
	ins_char(cc);

    எனில் (enc_utf8)
	// Handle composing chars.
	க்கு (;;)
	{
	    c = replace_எடு();
	    எனில் (c == -1)	    // stack empty
		நிறுத்து;
	    எனில் ((n = MB_BYTE2LEN(c)) == 1)
	    {
		// Not a multi-byte char, put it back.
		replace_தள்ளு(c);
		நிறுத்து;
	    }

	    buf[0] = c;
	    க்கு (i = 1; i < n; ++i)
		buf[i] = replace_எடு();
	    எனில் (utf_iscomposing(utf_ptr2char(buf)))
		ins_bytes_len(buf, n);
	    இல்லையெனில்
	    {
		// Not a composing char, put it back.
		க்கு (i = n - 1; i >= 0; --i)
		    replace_தள்ளு(buf[i]);
		நிறுத்து;
	    }

	}
}

/*
 * make the replace stack empty
 * (called when exiting replace mode)
 */
    நிலையான வெறுமை
replace_flush(வெறுமை)
{
    VIM_CLEAR(replace_அடுக்கு);
    replace_அடுக்கு_len = 0;
    replace_அடுக்கு_nr = 0;
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
    நிலையான வெறுமை
replace_do_bs(முழு limit_col)
{
    முழு		cc;
    முழு		orig_len = 0;
    முழு		ins_len;
    முழு		orig_vcols = 0;
    colnr_T	start_vcol;
    char_u	*p;
    முழு		i;
    முழு		vcol;

    cc = replace_எடு();
    எனில் (cc > 0)
    {
#ifdef FEAT_PROP_POPUP
	size_t	len_before = 0;  // init to shut up GCC

	எனில் (curbuf->b_has_textprop)
	{
	    // Do not adjust text properties for individual delete and insert
	    // operations, do it afterwards on the resulting text.
	    len_before = ml_get_curline_len();
	    ++text_prop_frozen;
	}
#endif
	எனில் (State & VREPLACE_FLAG)
	{
	    // Get the number of screen cells used by the character we are
	    // going to delete.
	    getvcol(curwin, &curwin->w_cursor, வெற்று, &start_vcol, வெற்று, 0);
	    orig_vcols = chartabsize(ml_get_cursor(), start_vcol);
	}
	எனில் (has_mbyte)
	{
	    (வெறுமை)del_char_after_col(limit_col);
	    எனில் (State & VREPLACE_FLAG)
		orig_len = ml_get_cursor_len();
	    replace_தள்ளு(cc);
	}
	இல்லையெனில்
	{
	    pchar_cursor(cc);
	    எனில் (State & VREPLACE_FLAG)
		orig_len = ml_get_cursor_len() - 1;
	}
	replace_எடு_ins();

	எனில் (State & VREPLACE_FLAG)
	{
	    // Get the number of screen cells used by the inserted characters
	    p = ml_get_cursor();
	    ins_len = ml_get_cursor_len() - orig_len;
	    vcol = start_vcol;
	    க்கு (i = 0; i < ins_len; ++i)
	    {
		vcol += chartabsize(p + i, vcol);
		i += (*mb_ptr2len)(p + i) - 1;
	    }
	    vcol -= start_vcol;

	    // Delete spaces that were inserted after the cursor to keep the
	    // text aligned.
	    curwin->w_cursor.col += ins_len;
	    போது (vcol > orig_vcols && gchar_cursor() == ' ')
	    {
		del_char(FALSE);
		++orig_vcols;
	    }
	    curwin->w_cursor.col -= ins_len;
	}

	// mark the buffer as changed and prepare for displaying
	changed_bytes(curwin->w_cursor.lnum, curwin->w_cursor.col);

#ifdef FEAT_PROP_POPUP
	எனில் (curbuf->b_has_textprop)
	{
	    size_t len_now = ml_get_curline_len();

	    --text_prop_frozen;
	    adjust_prop_columns(curwin->w_cursor.lnum, curwin->w_cursor.col,
					   (முழு)(len_now - len_before), 0);
	}
#endif
    }
    இல்லையெனில் எனில் (cc == 0)
	(வெறுமை)del_char_after_col(limit_col);
}

#எனில் defined(FEAT_RIGHTLEFT)
/*
 * Map Hebrew keyboard when in hkmap mode.
 */
    முழு
hkmap(முழு c)
{
    எனில் (p_hkmapp)   // phonetic mapping, by Ilya Dogolazky
    {
	எண்ணிக்கை {hALEF=0, BET, GIMEL, DALET, HEI, VAV, ZAIN, HET, TET, IUD,
	    KAFsofit, hKAF, LAMED, MEMsofit, MEM, NUNsofit, NUN, SAMEH, AIN,
	    PEIsofit, PEI, ZADIsofit, ZADI, KOF, RESH, hSHIN, TAV};
	நிலையான char_u map[26] =
	    {(char_u)hALEF/*a*/, (char_u)BET  /*b*/, (char_u)hKAF    /*c*/,
	     (char_u)DALET/*d*/, (char_u)-1   /*e*/, (char_u)PEIsofit/*f*/,
	     (char_u)GIMEL/*g*/, (char_u)HEI  /*h*/, (char_u)IUD     /*i*/,
	     (char_u)HET  /*j*/, (char_u)KOF  /*k*/, (char_u)LAMED   /*l*/,
	     (char_u)MEM  /*m*/, (char_u)NUN  /*n*/, (char_u)SAMEH   /*o*/,
	     (char_u)PEI  /*p*/, (char_u)-1   /*q*/, (char_u)RESH    /*r*/,
	     (char_u)ZAIN /*s*/, (char_u)TAV  /*t*/, (char_u)TET     /*u*/,
	     (char_u)VAV  /*v*/, (char_u)hSHIN/*w*/, (char_u)-1      /*x*/,
	     (char_u)AIN  /*y*/, (char_u)ZADI /*z*/};

	எனில் (c == 'N' || c == 'M' || c == 'P' || c == 'C' || c == 'Z')
	    திரும்பு (முழு)(map[CharOrd(c)] - 1 + p_aleph);
							    // '-1'='sofit'
	இல்லையெனில் எனில் (c == 'x')
	    திரும்பு 'X';
	இல்லையெனில் எனில் (c == 'q')
	    திரும்பு '\''; // {geresh}={'}
	இல்லையெனில் எனில் (c == 246)
	    திரும்பு ' ';  // \"o --> ' ' for a german keyboard
	இல்லையெனில் எனில் (c == 228)
	    திரும்பு ' ';  // \"a --> ' '      -- / --
	இல்லையெனில் எனில் (c == 252)
	    திரும்பு ' ';  // \"u --> ' '      -- / --
	// NOTE: islower() does not do the right thing for us on Linux so we
	// do this the same was as 5.7 and previous, so it works correctly on
	// all systems.  Specifically, the e.g. Delete and Arrow keys are
	// munged and won't work if e.g. searching for Hebrew text.
	இல்லையெனில் எனில் (c >= 'a' && c <= 'z')
	    திரும்பு (முழு)(map[Char_Ord_கீழ்(c)] + p_aleph);
	இல்லையெனில்
	    திரும்பு c;
    }
    இல்லையெனில்
    {
	தேர்வு (c)
	{
	    நிலை '`':	திரும்பு ';';
	    நிலை '/':	திரும்பு '.';
	    நிலை '\'':	திரும்பு ',';
	    நிலை 'q':	திரும்பு '/';
	    நிலை 'w':	திரும்பு '\'';

			// Hebrew letters - set offset from 'a'
	    நிலை ',':	c = '{'; நிறுத்து;
	    நிலை '.':	c = 'v'; நிறுத்து;
	    நிலை ';':	c = 't'; நிறுத்து;
	    இயல்பு: {
			 நிலையான எழுத்து str[] = "zqbcxlsjphmkwonu ydafe rig";

			 எனில் (c < 'a' || c > 'z')
			     திரும்பு c;
			 c = str[Char_Ord_கீழ்(c)];
			 நிறுத்து;
		     }
	}

	திரும்பு (முழு)(Char_Ord_கீழ்(c) + p_aleph);
    }
}
#endif

    நிலையான வெறுமை
ins_reg(வெறுமை)
{
    முழு		need_redraw = FALSE;
    முழு		regname;
    முழு		literally = 0;
    முழு		vis_active = VIsual_active;

    /*
     * If we are going to wait for a character, show a '"'.
     */
    pc_status = PC_STATUS_UNSET;
    எனில் (redrawing() && !char_avail())
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
    ++allow_சாவிகள்;
    regname = plain_vgetc();
    LANGMAP_ADJUST(regname, TRUE);
    எனில் (regname == Ctrl_R || regname == Ctrl_O || regname == Ctrl_P)
    {
	// Get a third key for literal register insertion
	literally = regname;
	add_to_showcmd_c(literally);
	regname = plain_vgetc();
	LANGMAP_ADJUST(regname, TRUE);
    }
    --no_mapping;
    --allow_சாவிகள்;

#ifdef FEAT_EVAL
    // Don't call u_sync() while typing the expression or giving an error
    // message for it. Only call it explicitly.
    ++no_u_sync;
    எனில் (regname == '=')
    {
	pos_T	curpos = curwin->w_cursor;
# ifdef HAVE_உள்ளீடு_METHOD
	முழு	im_on = im_get_status();
# endif
	// Sync undo when evaluating the expression calls setline() or
	// append(), so that it can be undone separately.
	u_sync_once = 2;

	regname = get_கோவை_register();

	// Cursor may be moved back a column.
	curwin->w_cursor = curpos;
	check_cursor();
# ifdef HAVE_உள்ளீடு_METHOD
	// Restore the Input Method.
	எனில் (im_on)
	    im_set_active(TRUE);
# endif
    }
    எனில் (regname == NUL || !valid_yank_reg(regname, FALSE))
    {
	vim_beep(BO_REG);
	need_redraw = TRUE;	// remove the '"'
    }
    இல்லையெனில்
    {
#endif
	எனில் (literally == Ctrl_O || literally == Ctrl_P)
	{
	    // Append the command to the redo buffer.
	    AppendCharToRedobuff(Ctrl_R);
	    AppendCharToRedobuff(literally);
	    AppendCharToRedobuff(regname);

	    do_put(regname, வெற்று, BACKWARD, 1L,
		 (literally == Ctrl_P ? PUT_FIXINDENT : 0) | PUT_CURSEND);
	}
	இல்லையெனில் எனில் (செருகு_reg(regname, literally) == FAIL)
	{
	    vim_beep(BO_REG);
	    need_redraw = TRUE;	// remove the '"'
	}
	இல்லையெனில் எனில் (stop_செருகு_mode)
	    // When the '=' register was used and a function was invoked that
	    // did ":stopinsert" then stuff_empty() returns FALSE but we won't
	    // insert anything, need to remove the '"'
	    need_redraw = TRUE;

#ifdef FEAT_EVAL
    }
    --no_u_sync;
    எனில் (u_sync_once == 1)
	ins_need_undo = TRUE;
    u_sync_once = 0;
#endif
    clear_showcmd();

    // If the inserted register is empty, we need to remove the '"'
    எனில் (need_redraw || stuff_empty())
	edit_unputchar();

    // Disallow starting Visual mode here, would get a weird mode.
    எனில் (!vis_active && VIsual_active)
	end_visual_mode();
}

/*
 * CTRL-G commands in Insert mode.
 */
    நிலையான வெறுமை
ins_ctrl_g(வெறுமை)
{
    முழு		c;

    // Right after CTRL-X the cursor will be after the ruler.
    setcursor();

    /*
     * Don't map the second key. This also prevents the mode message to be
     * deleted when ESC is hit.
     */
    ++no_mapping;
    ++allow_சாவிகள்;
    c = plain_vgetc();
    --no_mapping;
    --allow_சாவிகள்;
    தேர்வு (c)
    {
	// CTRL-G k and CTRL-G <Up>: cursor up to Insstart.col
	நிலை K_UP:
	நிலை Ctrl_K:
	நிலை 'k': ins_up(TRUE);
		  நிறுத்து;

	// CTRL-G j and CTRL-G <Down>: cursor down to Insstart.col
	நிலை K_DOWN:
	நிலை Ctrl_J:
	நிலை 'j': ins_down(TRUE);
		  நிறுத்து;

	// CTRL-G u: start new undoable edit
	நிலை 'u': u_sync(TRUE);
		  ins_need_undo = TRUE;

		  // Need to reset Insstart, esp. because a BS that joins
		  // a line to the previous one must save for undo.
		  update_Insstart_orig = FALSE;
		  Insstart = curwin->w_cursor;
		  நிறுத்து;

	// CTRL-G U: do not break undo with the next char
	நிலை 'U':
		  // Allow one left/right cursor movement with the next char,
		  // without breaking undo.
		  dont_sync_undo = MAYBE;
		  நிறுத்து;

	நிலை ESC:
		  // Esc after CTRL-G cancels it.
		  நிறுத்து;

	// Unknown CTRL-G command, reserved for future expansion.
	இயல்பு:  vim_beep(BO_CTRLG);
    }
}

/*
 * CTRL-^ in Insert mode.
 */
    நிலையான வெறுமை
ins_ctrl_hat(வெறுமை)
{
    எனில் (map_to_exists_mode((char_u *)"", MODE_LANGMAP, FALSE))
    {
	// ":lmap" mappings exists, Toggle use of ":lmap" mappings.
	எனில் (State & MODE_LANGMAP)
	{
	    curbuf->b_p_iminsert = B_IMODE_NONE;
	    State &= ~MODE_LANGMAP;
	}
	இல்லையெனில்
	{
	    curbuf->b_p_iminsert = B_IMODE_LMAP;
	    State |= MODE_LANGMAP;
#ifdef HAVE_உள்ளீடு_METHOD
	    im_set_active(FALSE);
#endif
	}
    }
#ifdef HAVE_உள்ளீடு_METHOD
    இல்லையெனில்
    {
	// There are no ":lmap" mappings, toggle IM
	எனில் (im_get_status())
	{
	    curbuf->b_p_iminsert = B_IMODE_NONE;
	    im_set_active(FALSE);
	}
	இல்லையெனில்
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
    எனில் (gui.in_use)
	gui_update_cursor(TRUE, FALSE);
#endif
#எனில் defined(FEAT_KEYMAP)
    // Show/unshow value of 'keymap' in status lines.
    status_redraw_curbuf();
#endif
}

/*
 * Handle ESC in insert mode.
 * Returns TRUE when leaving insert mode, FALSE when going to repeat the
 * insert.
 */
    நிலையான முழு
ins_esc(
    long	*மொத்தம்,
    முழு		cmdchar,
    முழு		nomove)	    // don't move cursor
{
    முழு		temp;
    நிலையான முழு	disabled_redraw = FALSE;
#ifdef FEAT_CONCEAL
    // Remember if the cursor line was concealed before changing State.
    முழு		cursor_வரி_was_concealed = curwin->w_p_cole > 0
						&& conceal_cursor_வரி(curwin);
#endif

#ifdef FEAT_SPELL
    check_spell_redraw();
#endif

    temp = curwin->w_cursor.col;
    எனில் (disabled_redraw)
    {
	எனில் (RedrawingDisabled > 0)
	    --RedrawingDisabled;
	disabled_redraw = FALSE;
    }
    எனில் (!arrow_used)
    {
	/*
	 * Don't append the ESC for "r<CR>" and "grx".
	 * When 'insertmode' is set only CTRL-L stops Insert mode.  Needed for
	 * when "count" is non-zero.
	 */
	எனில் (cmdchar != 'r' && cmdchar != 'v')
	    AppendToRedobuff(p_im ? (char_u *)"\014" : ESC_STR);

	/*
	 * Repeating insert may take a long time.  Check for
	 * interrupt now and then.
	 */
	எனில் (*மொத்தம் > 0)
	{
	    வரி_breakcheck();
	    எனில் (got_int)
		*மொத்தம் = 0;
	}

	எனில் (--*மொத்தம் > 0)	// repeat what was typed
	{
	    // Vi repeats the insert without replacing characters.
	    எனில் (vim_strchr(p_cpo, CPO_REPLCNT) != வெற்று)
		State &= ~REPLACE_FLAG;

	    (வெறுமை)start_redo_ins();
	    எனில் (cmdchar == 'r' || cmdchar == 'v')
		stuffRedoReadbuff(ESC_STR);	// no ESC in redo buffer
	    ++RedrawingDisabled;
	    disabled_redraw = TRUE;
	    திரும்பு FALSE;	// repeat the insert
	}
	stop_செருகு(&curwin->w_cursor, TRUE, nomove);
	undisplay_dollar();
    }

    எனில் (cmdchar != 'r' && cmdchar != 'v')
	ins_apply_autocmds(EVENT_INSERTLEAVEPRE);

    // When an autoindent was removed, curswant stays after the
    // indent
    எனில் (restart_edit == NUL && (colnr_T)temp == curwin->w_cursor.col)
	curwin->w_set_curswant = true;

    // Remember the last Insert position in the '^ mark.
    எனில் ((cmdmod.cmod_flags & CMOD_KEEPJUMPS) == 0)
	curbuf->b_last_செருகு = curwin->w_cursor;

    /*
     * The cursor should end up on the last inserted character.
     * Don't do it for CTRL-O, unless past the end of the line.
     */
    எனில் (!nomove
	    && (curwin->w_cursor.col != 0
		|| curwin->w_cursor.coladd > 0)
	    && (restart_edit == NUL
		   || (gchar_cursor() == NUL && !VIsual_active))
#ifdef FEAT_RIGHTLEFT
	    && !revins_on
#endif
				      )
    {
	எனில் (curwin->w_cursor.coladd > 0 || get_ve_flags() == VE_ALL)
	{
	    oneleft();
	    எனில் (restart_edit != NUL)
		++curwin->w_cursor.coladd;
	}
	இல்லையெனில்
	{
	    --curwin->w_cursor.col;
	    curwin->w_valid &= ~(VALID_WCOL|VALID_VIRTCOL);
	    // Correct cursor for multi-byte character.
	    எனில் (has_mbyte)
		mb_adjust_cursor();
	}
    }

#ifdef HAVE_உள்ளீடு_METHOD
    // Disable IM to allow typing English directly for Normal mode commands.
    // When ":lmap" is enabled don't change 'iminsert' (IM can be enabled as
    // well).
    எனில் (!(State & MODE_LANGMAP))
	im_save_status(&curbuf->b_p_iminsert);
    im_set_active(FALSE);
#endif

    State = MODE_NORMAL;
    may_trigger_modechanged();
    // need to position cursor again when on a TAB and when on a char with
    // virtual text.
    எனில் (gchar_cursor() == TAB
#ifdef FEAT_PROP_POPUP
	    || curbuf->b_has_textprop
#endif
       )
	curwin->w_valid &= ~(VALID_WROW|VALID_WCOL|VALID_VIRTCOL);

    setmouse();
#ifdef CURSOR_SHAPE
    ui_cursor_shape();		// may show different cursor shape
#endif
    எனில் (!p_ek)
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
    conceal_check_cursor_வரி(cursor_வரி_was_concealed);
#endif

    // When recording or for CTRL-O, need to display the new mode.
    // Otherwise remove the mode message.
    எனில் (reg_recording != 0 || restart_edit != NUL)
	showmode();
    இல்லையெனில் எனில் (p_smd && (got_int || !skip_showmode()))
	செய்தி("");

    திரும்பு TRUE;	    // exit Insert mode
}

#ifdef FEAT_RIGHTLEFT
/*
 * Toggle language: hkmap and revins_on.
 * Move to end of reverse inserted text.
 */
    நிலையான வெறுமை
ins_ctrl_(வெறுமை)
{
    எனில் (revins_on && revins_chars && revins_scol >= 0)
    {
	போது (gchar_cursor() != NUL && revins_chars--)
	    ++curwin->w_cursor.col;
    }
    p_ri = !p_ri;
    revins_on = (State == MODE_செருகு && p_ri);
    எனில் (revins_on)
    {
	revins_scol = curwin->w_cursor.col;
	revins_legal++;
	revins_chars = 0;
	undisplay_dollar();
    }
    இல்லையெனில்
	revins_scol = -1;
    p_hkmap = curwin->w_p_rl ^ p_ri;    // be consistent!
    showmode();
}
#endif

/*
 * If 'keymodel' contains "startsel", may start selection.
 * Returns TRUE when a CTRL-O and other keys stuffed.
 */
    நிலையான முழு
ins_start_select(முழு c)
{
    எனில் (!km_startsel)
	திரும்பு FALSE;
    தேர்வு (c)
    {
	நிலை K_KHOME:
	நிலை K_KEND:
	நிலை K_PAGEUP:
	நிலை K_KPAGEUP:
	நிலை K_PAGEDOWN:
	நிலை K_KPAGEDOWN:
#ifdef MACOS_X
	நிலை K_இடது:
	நிலை K_வலது:
	நிலை K_UP:
	நிலை K_DOWN:
	நிலை K_END:
	நிலை K_HOME:
#endif
	    எனில் (!(mod_mask & MOD_MASK_SHIFT))
		நிறுத்து;
	    // FALLTHROUGH
	நிலை K_S_இடது:
	நிலை K_S_வலது:
	நிலை K_S_UP:
	நிலை K_S_DOWN:
	நிலை K_S_END:
	நிலை K_S_HOME:
	    // Start selection right away, the cursor can move with CTRL-O when
	    // beyond the end of the line.
	    start_selection();

	    // Execute the key in (insert) Select mode.
	    stuffcharReadbuff(Ctrl_O);
	    எனில் (mod_mask)
	    {
		char_u	    buf[4];

		buf[0] = K_SPECIAL;
		buf[1] = KS_MODIFIER;
		buf[2] = mod_mask;
		buf[3] = NUL;
		stuffReadbuffLen(buf, 3L);
	    }
	    stuffcharReadbuff(c);
	    திரும்பு TRUE;
    }
    திரும்பு FALSE;
}

/*
 * <Insert> key in Insert mode: toggle insert/replace mode.
 */
    நிலையான வெறுமை
ins_செருகு(முழு replaceState)
{
#ifdef FEAT_EVAL
    set_vim_var_string(VV_INSERTMODE,
		   (char_u *)((State & REPLACE_FLAG) ? "i"
			    : replaceState == MODE_VREPLACE ? "v" : "r"), 1);
#endif
    ins_apply_autocmds(EVENT_INSERTCHANGE);
    எனில் (State & REPLACE_FLAG)
	State = MODE_செருகு | (State & MODE_LANGMAP);
    இல்லையெனில்
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
    நிலையான வெறுமை
ins_ctrl_o(வெறுமை)
{
    restart_VIsual_select = 0;
    எனில் (State & VREPLACE_FLAG)
	restart_edit = 'V';
    இல்லையெனில் எனில் (State & REPLACE_FLAG)
	restart_edit = 'R';
    இல்லையெனில்
	restart_edit = 'I';
    எனில் (virtual_active())
	ins_at_eol = FALSE;	// cursor always keeps its column
    இல்லையெனில்
	ins_at_eol = (gchar_cursor() == NUL);
}

/*
 * If the cursor is on an indent, ^T/^D insert/delete one
 * shiftwidth.	Otherwise ^T/^D behave like a "<<" or ">>".
 * Always round the indent to 'shiftwidth', this is compatible
 * with vi.  But vi only supports ^T and ^D after an
 * autoindent, we support it everywhere.
 */
    நிலையான வெறுமை
ins_shift(முழு c, முழு lastc)
{
    எனில் (stop_arrow() == FAIL)
	திரும்பு;
    AppendCharToRedobuff(c);

    /*
     * 0^D and ^^D: remove all indent.
     */
    எனில் (c == Ctrl_D && (lastc == '0' || lastc == '^')
						  && curwin->w_cursor.col > 0)
    {
	--curwin->w_cursor.col;
	(வெறுமை)del_char(FALSE);		// delete the '^' or '0'
	// In Replace mode, restore the characters that '^' or '0' replaced.
	எனில் (State & REPLACE_FLAG)
	    replace_எடு_ins();
	எனில் (lastc == '^')
	    old_indent = get_indent();	// remember curr. indent
	change_indent(INDENT_SET, 0, TRUE, 0, TRUE);
    }
    இல்லையெனில்
	change_indent(c == Ctrl_D ? INDENT_DEC : INDENT_INC, 0, TRUE, 0, TRUE);

    எனில் (did_ai && *skipwhite(ml_get_curline()) != NUL)
	did_ai = FALSE;
    did_si = FALSE;
    can_si = FALSE;
    can_si_back = FALSE;
    can_cindent = FALSE;	// no cindenting after ^D or ^T
}

    நிலையான வெறுமை
ins_del(வெறுமை)
{
    முழு	    temp;

    எனில் (stop_arrow() == FAIL)
	திரும்பு;
    எனில் (gchar_cursor() == NUL)		// delete newline
    {
	temp = curwin->w_cursor.col;
	எனில் (!can_bs(BS_EOL)		// only if "eol" included
		|| do_join(2, FALSE, TRUE, FALSE, FALSE) == FAIL)
	    vim_beep(BO_BS);
	இல்லையெனில்
	{
	    curwin->w_cursor.col = temp;
	    // Adjust orig_line_count in case more lines have been deleted than
	    // have been added. That makes sure, that open_line() later
	    // can access all buffer lines correctly
	    எனில் (State & VREPLACE_FLAG &&
		    orig_வரி_மொத்தம் > curbuf->b_ml.ml_வரி_மொத்தம்)
		orig_வரி_மொத்தம் = curbuf->b_ml.ml_வரி_மொத்தம்;
	}
    }
    இல்லையெனில் எனில் (del_char(FALSE) == FAIL)  // delete char under cursor
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
    நிலையான வெறுமை
ins_bs_one(வெறுமை)
{
    dec_cursor();
    எனில் (State & REPLACE_FLAG)
    {
	// Don't delete characters before the insert point when in
	// Replace mode
	எனில் (curwin->w_cursor.lnum != Insstart.lnum
		|| curwin->w_cursor.col >= Insstart.col)
	    replace_do_bs(-1);
    }
    இல்லையெனில்
	(வெறுமை)del_char(FALSE);
}

/*
 * Handle Backspace, delete-word and delete-line in Insert mode.
 * Return TRUE when backspace was actually used.
 */
    நிலையான முழு
ins_bs(
    முழு		c,
    முழு		mode,
    முழு		*inserted_space_p)
{
    linenr_T	lnum;
    முழு		cc;
    முழு		temp = 0;	    // init for GCC
    colnr_T	save_col;
    colnr_T	mincol;
    முழு		did_backspace = FALSE;
    முழு		in_indent;
    முழு		oldState;
    முழு		cpc[MAX_MCO];	    // composing characters
    முழு		call_fix_indent = FALSE;

    /*
     * can't delete anything in an empty file
     * can't backup past first character in buffer
     * can't backup past starting point unless 'backspace' > 1
     * can backup to a previous line if 'backspace' == 0
     */
    எனில் (       BUFEMPTY()
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
	திரும்பு FALSE;
    }

    எனில் (stop_arrow() == FAIL)
	திரும்பு FALSE;
    in_indent = inindent(0);
    எனில் (in_indent)
	can_cindent = FALSE;
    end_comment_pending = NUL;	// After BS, don't auto-end comment
#ifdef FEAT_RIGHTLEFT
    எனில் (revins_on)	    // put cursor after last inserted char
	inc_cursor();
#endif

    // Virtualedit:
    //	BACKSPACE_CHAR eats a virtual space
    //	BACKSPACE_WORD eats all coladd
    //	BACKSPACE_LINE eats all coladd and keeps going
    எனில் (curwin->w_cursor.coladd > 0)
    {
	எனில் (mode == BACKSPACE_CHAR)
	{
	    --curwin->w_cursor.coladd;
	    திரும்பு TRUE;
	}
	எனில் (mode == BACKSPACE_WORD)
	{
	    curwin->w_cursor.coladd = 0;
	    திரும்பு TRUE;
	}
	curwin->w_cursor.coladd = 0;
    }

    /*
     * Delete newline!
     */
    எனில் (curwin->w_cursor.col == 0)
    {
	lnum = Insstart.lnum;
	எனில் (curwin->w_cursor.lnum == lnum
#ifdef FEAT_RIGHTLEFT
			|| revins_on
#endif
				    )
	{
	    எனில் (u_save((linenr_T)(curwin->w_cursor.lnum - 2),
			       (linenr_T)(curwin->w_cursor.lnum + 1)) == FAIL)
		திரும்பு FALSE;
	    --Insstart.lnum;
	    Insstart.col = ml_get_len(Insstart.lnum);
	}
	/*
	 * In replace mode:
	 * cc < 0: NL was inserted, delete it
	 * cc >= 0: NL was replaced, put original characters back
	 */
	cc = -1;
	எனில் (State & REPLACE_FLAG)
	    cc = replace_எடு();	    // returns -1 if NL was inserted
	/*
	 * In replace mode, in the line we started replacing, we only move the
	 * cursor.
	 */
	எனில் ((State & REPLACE_FLAG) && curwin->w_cursor.lnum <= lnum)
	{
	    dec_cursor();
	}
	இல்லையெனில்
	{
	    எனில் (!(State & VREPLACE_FLAG)
				   || curwin->w_cursor.lnum > orig_வரி_மொத்தம்)
	    {
		temp = gchar_cursor();	// remember current char
		--curwin->w_cursor.lnum;

		// When "aw" is in 'formatoptions' we must delete the space at
		// the end of the line, otherwise the line will be broken
		// again when auto-formatting.
		எனில் (has_format_option(FO_AUTO)
					   && has_format_option(FO_WHITE_PAR))
		{
		    char_u  *ptr = ml_get_buf(curbuf, curwin->w_cursor.lnum,
									FALSE);
		    முழு	    len = ml_get_curline_len();

		    எனில் (len > 0 && ptr[len - 1] == ' ')
		    {
			char_u *newp = alloc(curbuf->b_ml.ml_வரி_len - 1);

			எனில் (newp != வெற்று)
			{
			    mch_memmove(newp, ptr, len - 1);
			    newp[len - 1] = NUL;
			    எனில் (curbuf->b_ml.ml_வரி_len > len + 1)
				mch_memmove(newp + len, ptr + len + 1,
					   curbuf->b_ml.ml_வரி_len - len - 1);

			    எனில் (curbuf->b_ml.ml_flags
					      & (ML_வரி_DIRTY | ML_ALLOCATED))
				vim_free(curbuf->b_ml.ml_வரி_ptr);
			    curbuf->b_ml.ml_வரி_ptr = newp;
			    curbuf->b_ml.ml_வரி_len--;
			    curbuf->b_ml.ml_வரி_textlen--;
			    curbuf->b_ml.ml_flags |= ML_வரி_DIRTY;
			}
		    }
		}

		(வெறுமை)do_join(2, FALSE, FALSE, FALSE, FALSE);
		எனில் (temp == NUL && gchar_cursor() != NUL)
		    inc_cursor();
	    }
	    இல்லையெனில்
		dec_cursor();

	    /*
	     * In MODE_REPLACE mode we have to put back the text that was
	     * replaced by the NL. On the replace stack is first a
	     * NUL-terminated sequence of characters that were deleted and then
	     * the characters that NL replaced.
	     */
	    எனில் (State & REPLACE_FLAG)
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
		போது (cc > 0)
		{
		    save_col = curwin->w_cursor.col;
		    mb_replace_எடு_ins(cc);
		    curwin->w_cursor.col = save_col;
		    cc = replace_எடு();
		}
		// restore the characters that NL replaced
		replace_எடு_ins();
		State = oldState;
	    }
	}
	did_ai = FALSE;
    }
    இல்லையெனில்
    {
	/*
	 * Delete character(s) before the cursor.
	 */
#ifdef FEAT_RIGHTLEFT
	எனில் (revins_on)		// put cursor on last inserted char
	    dec_cursor();
#endif
	mincol = 0;
						// keep indent
	எனில் (mode == BACKSPACE_வரி
		&& (curbuf->b_p_ai || cindent_on())
#ifdef FEAT_RIGHTLEFT
		&& !revins_on
#endif
			    )
	{
	    save_col = curwin->w_cursor.col;
	    beginline(BL_WHITE);
	    எனில் (curwin->w_cursor.col < save_col)
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
	எனில் (	   mode == BACKSPACE_CHAR
		&& ((p_sta && in_indent)
		    || ((get_sts_மதிப்பு() != 0
#ifdef FEAT_VARTABS
			|| tabstop_மொத்தம்(curbuf->b_p_vsts_array)
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
	    char_u	*வரி;
	    char_u	*ptr;
	    char_u	*cursor_ptr;
	    char_u	*space_ptr;
	    colnr_T	space_vcol = 0;
	    முழு		prev_space = FALSE;
	    colnr_T	want_col;

	    *inserted_space_p = FALSE;

	    space_ptr = ptr = வரி = ml_get_curline();
	    cursor_ptr = வரி + curwin->w_cursor.col;

	    // Compute virtual column of cursor position, and find the last
	    // whitespace before cursor that is preceded by non-whitespace.
	    // Use chartabsize() so that virtual text and wrapping are ignored.
	    போது (ptr < cursor_ptr)
	    {
		முழு	cur_space = VIM_ISWHITE(*ptr);

		எனில் (!prev_space && cur_space)
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
	    எனில் (p_sta && in_indent)
		want_vcol -= want_vcol % (முழு)get_sw_மதிப்பு(curbuf);
	    இல்லையெனில்
#ifdef FEAT_VARTABS
		want_vcol = tabstop_start(want_vcol, get_sts_மதிப்பு(),
						       curbuf->b_p_vsts_array);
#இல்லையெனில்
		want_vcol -= want_vcol % (முழு)get_sts_மதிப்பு();
#endif

	    // Find the position to stop backspacing.
	    // Use chartabsize() so that virtual text and wrapping are ignored.
	    போது (TRUE)
	    {
		முழு size = chartabsize(space_ptr, space_vcol);

		எனில் (space_vcol + size > want_vcol)
		    நிறுத்து;
		space_vcol += size;
		MB_PTR_ADV(space_ptr);
	    }
	    want_col = space_ptr - வரி;

	    // Delete characters until we are at or before want_col.
	    போது (curwin->w_cursor.col > want_col)
		ins_bs_one();

	    // Insert extra spaces until we are at want_vcol.
	    க்கு (; space_vcol < want_vcol; space_vcol++)
	    {
		// Remember the first char we inserted
		எனில் (curwin->w_cursor.lnum == Insstart_orig.lnum
				   && curwin->w_cursor.col < Insstart_orig.col)
		    Insstart_orig.col = curwin->w_cursor.col;

		எனில் (State & VREPLACE_FLAG)
		    ins_char(' ');
		இல்லையெனில்
		{
		    ins_str((char_u *)" ", 1);
		    எனில் ((State & REPLACE_FLAG))
			replace_தள்ளு(NUL);
		}
	    }
	}

	/*
	 * Delete up to starting point, start of line or previous word.
	 */
	இல்லையெனில்
	{
	    முழு cclass = 0, prev_cclass = 0;

	    எனில் (has_mbyte)
		cclass = mb_get_class(ml_get_cursor());
	    செய்
	    {
#ifdef FEAT_RIGHTLEFT
		எனில் (!revins_on) // put cursor on char to be deleted
#endif
		    dec_cursor();

		cc = gchar_cursor();
		// look multi-byte character class
		எனில் (has_mbyte)
		{
		    prev_cclass = cclass;
		    cclass = mb_get_class(ml_get_cursor());
		}

		// start of word?
		எனில் (mode == BACKSPACE_WORD && !vim_isspace(cc))
		{
		    mode = BACKSPACE_WORD_NOT_SPACE;
		    temp = vim_iswordc(cc);
		}
		// end of word?
		இல்லையெனில் எனில் (mode == BACKSPACE_WORD_NOT_SPACE
			&& ((vim_isspace(cc) || vim_iswordc(cc) != temp)
			|| prev_cclass != cclass))
		{
#ifdef FEAT_RIGHTLEFT
		    எனில் (!revins_on)
#endif
			inc_cursor();
#ifdef FEAT_RIGHTLEFT
		    இல்லையெனில் எனில் (State & REPLACE_FLAG)
			dec_cursor();
#endif
		    நிறுத்து;
		}
		எனில் (State & REPLACE_FLAG)
		    replace_do_bs(-1);
		இல்லையெனில்
		{
		    எனில் (enc_utf8 && p_deco)
			(வெறுமை)utfc_ptr2char(ml_get_cursor(), cpc);
		    (வெறுமை)del_char(FALSE);
		    /*
		     * If there are combining characters and 'delcombine' is set
		     * move the cursor back.  Don't back up before the base
		     * character.
		     */
		    எனில் (enc_utf8 && p_deco && cpc[0] != NUL)
			inc_cursor();
#ifdef FEAT_RIGHTLEFT
		    எனில் (revins_chars)
		    {
			revins_chars--;
			revins_legal++;
		    }
		    எனில் (revins_on && gchar_cursor() == NUL)
			நிறுத்து;
#endif
		}
		// Just a single backspace?:
		எனில் (mode == BACKSPACE_CHAR)
		    நிறுத்து;
	    } போது (
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
    எனில் (curwin->w_cursor.col <= 1)
	did_ai = FALSE;

    எனில் (call_fix_indent)
	fix_indent();

    /*
     * It's a little strange to put backspaces into the redo
     * buffer, but it makes auto-indent a lot easier to deal
     * with.
     */
    AppendCharToRedobuff(c);

    // If deleted before the insertion point, adjust it
    எனில் (curwin->w_cursor.lnum == Insstart_orig.lnum
				  && curwin->w_cursor.col < Insstart_orig.col)
	Insstart_orig.col = curwin->w_cursor.col;

    // vi behaviour: the cursor moves backward but the character that
    //		     was there remains visible
    // Vim behaviour: the cursor moves backward and the character that
    //		      was there is erased from the screen.
    // We can emulate the vi behaviour by pretending there is a dollar
    // displayed even when there isn't.
    //  --pkv Sun Jan 19 01:56:40 EST 2003
    எனில் (vim_strchr(p_cpo, CPO_BACKSPACE) != வெற்று && dollar_vcol == -1)
	dollar_vcol = curwin->w_virtcol;

#ifdef FEAT_FOLDING
    // When deleting a char the cursor line must never be in a closed fold.
    // E.g., when 'foldmethod' is indent and deleting the first non-white
    // char before a Tab.
    எனில் (did_backspace)
	foldOpenCursor();
#endif

    திரும்பு did_backspace;
}

/*
 * Handle receiving P_PS: start paste mode.  Inserts the following text up to
 * P_PE literally.
 * When "drop" is TRUE then consume the text and drop it.
 */
    முழு
bracketed_paste(paste_mode_T mode, முழு drop, garray_T *gap)
{
    முழு		c;
    char_u	buf[NUMBUFLEN + MB_MAXBYTES];
    முழு		idx = 0;
    char_u	*end = find_termcode((char_u *)"PE");
    முழு		ret_char = -1;
    முழு		save_allow_சாவிகள் = allow_சாவிகள்;
    முழு		save_paste = p_paste;

    // If the end code is too long we can't detect it, read everything.
    எனில் (end != வெற்று && STRLEN(end) >= NUMBUFLEN)
	end = வெற்று;
    ++no_mapping;
    allow_சாவிகள் = 0;
    எனில் (!p_paste)
	// Also have the side effects of setting 'paste' to make it work much
	// faster.
	set_option_மதிப்பு_give_err((char_u *)"paste", TRUE, வெற்று, 0);

    க்கு (;;)
    {
	// When the end is not defined read everything there is.
	எனில் (end == வெற்று && vpeekc() == NUL)
	    நிறுத்து;
	செய்
	    c = vgetc();
	போது (c == K_IGNORE || c == K_VER_SCROLLBAR || c == K_HOR_SCROLLBAR);

	எனில் (c == NUL || got_int || (ex_normal_busy > 0 && c == Ctrl_C))
	    // When CTRL-C was encountered the typeahead will be flushed and we
	    // won't get the end sequence.  Except when using ":normal".
	    நிறுத்து;

	எனில் (has_mbyte)
	    idx += (*mb_char2bytes)(c, buf + idx);
	இல்லையெனில்
	    buf[idx++] = c;
	buf[idx] = NUL;
	எனில் (end != வெற்று && STRNCMP(buf, end, idx) == 0)
	{
	    எனில் (end[idx] == NUL)
		நிறுத்து; // Found the end of paste code.
	    தொடர்;
	}
	எனில் (!drop)
	{
	    தேர்வு (mode)
	    {
		நிலை PASTE_CMDLINE:
		    put_on_cmdline(buf, idx, TRUE);
		    நிறுத்து;

		நிலை PASTE_EX:
		    // add one for the NUL that is going to be appended
		    எனில் (gap != வெற்று && ga_grow(gap, idx + 1) == OK)
		    {
			mch_memmove((எழுத்து *)gap->ga_data + gap->ga_len,
							     buf, (size_t)idx);
			gap->ga_len += idx;
		    }
		    நிறுத்து;

		நிலை PASTE_செருகு:
		    எனில் (stop_arrow() == OK)
		    {
			c = buf[0];
			எனில் (idx == 1 && (c == CAR || c == K_KENTER || c == NL))
			    ins_eol(c);
			இல்லையெனில்
			{
			    ins_char_bytes(buf, idx);
			    AppendToRedobuffLit(buf, idx);
			}
		    }
		    நிறுத்து;

		நிலை PASTE_ONE_CHAR:
		    எனில் (ret_char == -1)
		    {
			எனில் (has_mbyte)
			    ret_char = (*mb_ptr2char)(buf);
			இல்லையெனில்
			    ret_char = buf[0];
		    }
		    நிறுத்து;
	    }
	}
	idx = 0;
    }

    --no_mapping;
    allow_சாவிகள் = save_allow_சாவிகள்;
    எனில் (!save_paste)
	set_option_மதிப்பு_give_err((char_u *)"paste", FALSE, வெற்று, 0);

    திரும்பு ret_char;
}

#எனில் defined(FEAT_GUI_TABLINE)
    நிலையான வெறுமை
ins_tabline(முழு c)
{
    // We will be leaving the current window, unless closing another tab.
    எனில் (c != K_TABMENU || current_tabmenu != TABLINE_MENU_CLOSE
		|| (current_tab != 0 && current_tab != tabpage_index(curtab)))
    {
	undisplay_dollar();
	start_arrow(&curwin->w_cursor);
	can_cindent = TRUE;
    }

    எனில் (c == K_TABLINE)
	goto_tabpage(current_tab);
    இல்லையெனில்
    {
	handle_tabmenu();
	redraw_statuslines();	// will redraw the tabline when needed
    }
}
#endif

#எனில் defined(FEAT_GUI)
    வெறுமை
ins_scroll(வெறுமை)
{
    pos_T	tpos;

    undisplay_dollar();
    tpos = curwin->w_cursor;
    எனில் (gui_do_scroll())
    {
	start_arrow(&tpos);
	can_cindent = TRUE;
    }
}

    வெறுமை
ins_horscroll(வெறுமை)
{
    pos_T	tpos;

    undisplay_dollar();
    tpos = curwin->w_cursor;
    எனில் (do_mousescroll_horiz(scrollbar_மதிப்பு))
    {
	start_arrow(&tpos);
	can_cindent = TRUE;
    }
}
#endif

    நிலையான வெறுமை
ins_இடது(வெறுமை)
{
    pos_T	tpos;
    முழு		end_change = dont_sync_undo == FALSE; // end undoable change

#ifdef FEAT_FOLDING
    எனில் ((fdo_flags & FDO_HOR) && சாவி_Typed)
	foldOpenCursor();
#endif
    undisplay_dollar();
    tpos = curwin->w_cursor;
    எனில் (oneleft() == OK)
    {
#எனில் defined(FEAT_XIM) && defined(FEAT_GUI_GTK)
	// Only call start_arrow() when not busy with preediting, it will
	// break undo.  K_LEFT is inserted in im_correct_cursor().
	எனில் (p_imst == IM_OVER_THE_SPOT || !im_is_preediting())
#endif
	{
	    start_arrow_with_change(&tpos, end_change);
	    எனில் (!end_change)
		AppendCharToRedobuff(K_இடது);
	}
#ifdef FEAT_RIGHTLEFT
	// If exit reversed string, position is fixed
	எனில் (revins_scol != -1 && (முழு)curwin->w_cursor.col >= revins_scol)
	    revins_legal++;
	revins_chars++;
#endif
    }

    /*
     * if 'whichwrap' set for cursor in insert mode may go to
     * previous line
     */
    இல்லையெனில் எனில் (vim_strchr(p_ww, '[') != வெற்று && curwin->w_cursor.lnum > 1)
    {
	// always break undo when moving upwards/downwards, else undo may break
	start_arrow(&tpos);
	--(curwin->w_cursor.lnum);
	coladvance((colnr_T)MAXCOL);
	curwin->w_set_curswant = true;	// so we stay at the end
    }
    இல்லையெனில்
	vim_beep(BO_CRSR);
    dont_sync_undo = FALSE;
}

    நிலையான வெறுமை
ins_home(முழு c)
{
    pos_T	tpos;

#ifdef FEAT_FOLDING
    எனில் ((fdo_flags & FDO_HOR) && சாவி_Typed)
	foldOpenCursor();
#endif
    undisplay_dollar();
    tpos = curwin->w_cursor;
    எனில் (c == K_C_HOME)
	curwin->w_cursor.lnum = 1;
    curwin->w_cursor.col = 0;
    curwin->w_cursor.coladd = 0;
    curwin->w_curswant = 0;
    start_arrow(&tpos);
}

    நிலையான வெறுமை
ins_end(முழு c)
{
    pos_T	tpos;

#ifdef FEAT_FOLDING
    எனில் ((fdo_flags & FDO_HOR) && சாவி_Typed)
	foldOpenCursor();
#endif
    undisplay_dollar();
    tpos = curwin->w_cursor;
    எனில் (c == K_C_END)
	curwin->w_cursor.lnum = curbuf->b_ml.ml_வரி_மொத்தம்;
    coladvance((colnr_T)MAXCOL);
    curwin->w_curswant = MAXCOL;

    start_arrow(&tpos);
}

    நிலையான வெறுமை
ins_s_இடது(வெறுமை)
{
    முழு end_change = dont_sync_undo == FALSE; // end undoable change
#ifdef FEAT_FOLDING
    எனில் ((fdo_flags & FDO_HOR) && சாவி_Typed)
	foldOpenCursor();
#endif
    undisplay_dollar();
    எனில் (curwin->w_cursor.lnum > 1 || curwin->w_cursor.col > 0)
    {
	start_arrow_with_change(&curwin->w_cursor, end_change);
	எனில் (!end_change)
	    AppendCharToRedobuff(K_S_இடது);
	(வெறுமை)bck_word(1L, FALSE, FALSE);
	curwin->w_set_curswant = true;
    }
    இல்லையெனில்
	vim_beep(BO_CRSR);
    dont_sync_undo = FALSE;
}

    நிலையான வெறுமை
ins_வலது(வெறுமை)
{
    முழு end_change = dont_sync_undo == FALSE; // end undoable change

#ifdef FEAT_FOLDING
    எனில் ((fdo_flags & FDO_HOR) && சாவி_Typed)
	foldOpenCursor();
#endif
    undisplay_dollar();
    எனில் (gchar_cursor() != NUL || virtual_active())
    {
	start_arrow_with_change(&curwin->w_cursor, end_change);
	எனில் (!end_change)
	    AppendCharToRedobuff(K_வலது);
	curwin->w_set_curswant = true;
	எனில் (virtual_active())
	    oneright();
	இல்லையெனில்
	{
	    எனில் (has_mbyte)
		curwin->w_cursor.col += (*mb_ptr2len)(ml_get_cursor());
	    இல்லையெனில்
		++curwin->w_cursor.col;
	}

#ifdef FEAT_RIGHTLEFT
	revins_legal++;
	எனில் (revins_chars)
	    revins_chars--;
#endif
    }
    // if 'whichwrap' set for cursor in insert mode, may move the
    // cursor to the next line
    இல்லையெனில் எனில் (vim_strchr(p_ww, ']') != வெற்று
	    && curwin->w_cursor.lnum < curbuf->b_ml.ml_வரி_மொத்தம்)
    {
	start_arrow(&curwin->w_cursor);
	curwin->w_set_curswant = true;
	++curwin->w_cursor.lnum;
	curwin->w_cursor.col = 0;
    }
    இல்லையெனில்
	vim_beep(BO_CRSR);
    dont_sync_undo = FALSE;
}

    நிலையான வெறுமை
ins_s_வலது(வெறுமை)
{
    முழு end_change = dont_sync_undo == FALSE; // end undoable change
#ifdef FEAT_FOLDING
    எனில் ((fdo_flags & FDO_HOR) && சாவி_Typed)
	foldOpenCursor();
#endif
    undisplay_dollar();
    எனில் (curwin->w_cursor.lnum < curbuf->b_ml.ml_வரி_மொத்தம்
	    || gchar_cursor() != NUL)
    {
	start_arrow_with_change(&curwin->w_cursor, end_change);
	எனில் (!end_change)
	    AppendCharToRedobuff(K_S_வலது);
	(வெறுமை)fwd_word(1L, FALSE, 0);
	curwin->w_set_curswant = true;
    }
    இல்லையெனில்
	vim_beep(BO_CRSR);
    dont_sync_undo = FALSE;
}

    நிலையான வெறுமை
ins_up(
    முழு		startcol)	// when TRUE move to Insstart.col
{
    pos_T	tpos;
    linenr_T	old_topline = curwin->w_topline;
#ifdef FEAT_DIFF
    முழு		old_topfill = curwin->w_topfill;
#endif

    undisplay_dollar();
    tpos = curwin->w_cursor;
    எனில் (cursor_up(1L, TRUE) == OK)
    {
	எனில் (startcol)
	    coladvance(getvcol_nolist(&Insstart));
	எனில் (old_topline != curwin->w_topline
#ifdef FEAT_DIFF
		|| old_topfill != curwin->w_topfill
#endif
		)
	    redraw_later(UPD_VALID);
	start_arrow(&tpos);
	can_cindent = TRUE;
    }
    இல்லையெனில்
	vim_beep(BO_CRSR);
}

    நிலையான வெறுமை
ins_pageup(வெறுமை)
{
    pos_T	tpos;

    undisplay_dollar();

    எனில் (mod_mask & MOD_MASK_CTRL)
    {
	// <C-PageUp>: tab page back
	எனில் (first_tabpage->tp_next != வெற்று)
	{
	    start_arrow(&curwin->w_cursor);
	    goto_tabpage(-1);
	}
	திரும்பு;
    }

    tpos = curwin->w_cursor;
    எனில் (pagescroll(BACKWARD, 1L, FALSE) == OK)
    {
	start_arrow(&tpos);
	can_cindent = TRUE;
    }
    இல்லையெனில்
	vim_beep(BO_CRSR);
}

    நிலையான வெறுமை
ins_down(
    முழு		startcol)	// when TRUE move to Insstart.col
{
    pos_T	tpos;
    linenr_T	old_topline = curwin->w_topline;
#ifdef FEAT_DIFF
    முழு		old_topfill = curwin->w_topfill;
#endif

    undisplay_dollar();
    tpos = curwin->w_cursor;
    எனில் (cursor_down(1L, TRUE) == OK)
    {
	எனில் (startcol)
	    coladvance(getvcol_nolist(&Insstart));
	எனில் (old_topline != curwin->w_topline
#ifdef FEAT_DIFF
		|| old_topfill != curwin->w_topfill
#endif
		)
	    redraw_later(UPD_VALID);
	start_arrow(&tpos);
	can_cindent = TRUE;
    }
    இல்லையெனில்
	vim_beep(BO_CRSR);
}

    நிலையான வெறுமை
ins_pagedown(வெறுமை)
{
    pos_T	tpos;

    undisplay_dollar();

    எனில் (mod_mask & MOD_MASK_CTRL)
    {
	// <C-PageDown>: tab page forward
	எனில் (first_tabpage->tp_next != வெற்று)
	{
	    start_arrow(&curwin->w_cursor);
	    goto_tabpage(0);
	}
	திரும்பு;
    }

    tpos = curwin->w_cursor;
    எனில் (pagescroll(FORWARD, 1L, FALSE) == OK)
    {
	start_arrow(&tpos);
	can_cindent = TRUE;
    }
    இல்லையெனில்
	vim_beep(BO_CRSR);
}

#ifdef FEAT_DND
    நிலையான வெறுமை
ins_drop(வெறுமை)
{
    do_put('~', வெற்று, BACKWARD, 1L, PUT_CURSEND);
}
#endif

/*
 * Handle TAB in Insert or Replace mode.
 * Return TRUE when the TAB needs to be inserted like a normal character.
 */
    நிலையான முழு
ins_tab(வெறுமை)
{
    முழு		ind;
    முழு		i;
    முழு		temp;

    எனில் (Insstart_blank_vcol == MAXCOL && curwin->w_cursor.lnum == Insstart.lnum)
	Insstart_blank_vcol = get_nolist_virtcol();
    எனில் (echeck_abbr(TAB + ABBR_OFF))
	திரும்பு FALSE;

    ind = inindent(0);
    எனில் (ind)
	can_cindent = FALSE;

    /*
     * When nothing special, insert TAB like a normal character.
     */
    எனில் (!curbuf->b_p_et
#ifdef FEAT_VARTABS
	    && !(p_sta && ind
		// These five lines mean 'tabstop' != 'shiftwidth'
		&& ((tabstop_மொத்தம்(curbuf->b_p_vts_array) > 1)
		    || (tabstop_மொத்தம்(curbuf->b_p_vts_array) == 1
			&& tabstop_first(curbuf->b_p_vts_array)
						       != get_sw_மதிப்பு(curbuf))
		    || (tabstop_மொத்தம்(curbuf->b_p_vts_array) == 0
			&& curbuf->b_p_ts != get_sw_மதிப்பு(curbuf))))
	    && tabstop_மொத்தம்(curbuf->b_p_vsts_array) == 0
#இல்லையெனில்
	    && !(p_sta && ind && curbuf->b_p_ts != get_sw_மதிப்பு(curbuf))
#endif
	    && get_sts_மதிப்பு() == 0)
	திரும்பு TRUE;

    எனில் (stop_arrow() == FAIL)
	திரும்பு TRUE;

    did_ai = FALSE;
    did_si = FALSE;
    can_si = FALSE;
    can_si_back = FALSE;
    AppendToRedobuff((char_u *)"\t");

#ifdef FEAT_VARTABS
    எனில் (p_sta && ind)		// insert tab in indent, use 'shiftwidth'
    {
	temp = (முழு)get_sw_மதிப்பு(curbuf);
	temp -= get_nolist_virtcol() % temp;
    }
    இல்லையெனில் எனில் (tabstop_மொத்தம்(curbuf->b_p_vsts_array) > 0 || curbuf->b_p_sts != 0)
				// use 'softtabstop' when set
	temp = tabstop_padding(get_nolist_virtcol(), get_sts_மதிப்பு(),
						     curbuf->b_p_vsts_array);
    இல்லையெனில்			// otherwise use 'tabstop'
	temp = tabstop_padding(get_nolist_virtcol(), curbuf->b_p_ts,
						     curbuf->b_p_vts_array);
#இல்லையெனில்
    எனில் (p_sta && ind)		// insert tab in indent, use 'shiftwidth'
	temp = (முழு)get_sw_மதிப்பு(curbuf);
    இல்லையெனில் எனில் (curbuf->b_p_sts != 0) // use 'softtabstop' when set
	temp = (முழு)get_sts_மதிப்பு();
    இல்லையெனில்			// otherwise use 'tabstop'
	temp = (முழு)curbuf->b_p_ts;
    temp -= get_nolist_virtcol() % temp;
#endif

    /*
     * Insert the first space with ins_char().	It will delete one char in
     * replace mode.  Insert the rest with ins_str(); it will not delete any
     * chars.  For MODE_VREPLACE state, we use ins_char() for all characters.
     */
    ins_char(' ');
    போது (--temp > 0)
    {
	எனில் (State & VREPLACE_FLAG)
	    ins_char(' ');
	இல்லையெனில்
	{
	    ins_str((char_u *)" ", 1);
	    எனில் (State & REPLACE_FLAG)	    // no char replaced
		replace_தள்ளு(NUL);
	}
    }

    /*
     * When 'expandtab' not set: Replace spaces by TABs where possible.
     */
#ifdef FEAT_VARTABS
    எனில் (!curbuf->b_p_et && (tabstop_மொத்தம்(curbuf->b_p_vsts_array) > 0
			    || get_sts_மதிப்பு() > 0
			    || (p_sta && ind)))
#இல்லையெனில்
    எனில் (!curbuf->b_p_et && (get_sts_மதிப்பு() || (p_sta && ind)))
#endif
    {
	char_u		*ptr;
	char_u		*saved_வரி = வெற்று;	// init for GCC
	pos_T		pos;
	pos_T		fpos;
	pos_T		*cursor;
	colnr_T		want_vcol, vcol;
	முழு		change_col = -1;
	முழு		save_list = curwin->w_p_list;
	char_u		*tab = (char_u *)"\t";
	chartabsize_T	cts;

	/*
	 * Get the current line.  For MODE_VREPLACE state, don't make real
	 * changes yet, just work on a copy of the line.
	 */
	எனில் (State & VREPLACE_FLAG)
	{
	    pos = curwin->w_cursor;
	    cursor = &pos;
	    saved_வரி = vim_strnsave(ml_get_curline(), ml_get_curline_len());
	    எனில் (saved_வரி == வெற்று)
		திரும்பு FALSE;
	    ptr = saved_வரி + pos.col;
	}
	இல்லையெனில்
	{
	    ptr = ml_get_cursor();
	    cursor = &curwin->w_cursor;
	}

	// When 'L' is not in 'cpoptions' a tab always takes up 'ts' spaces.
	எனில் (vim_strchr(p_cpo, CPO_LISTWM) == வெற்று)
	    curwin->w_p_list = FALSE;

	// Find first white before the cursor
	fpos = curwin->w_cursor;
	போது (fpos.col > 0 && VIM_ISWHITE(ptr[-1]))
	{
	    --fpos.col;
	    --ptr;
	}

	// In Replace mode, don't change characters before the insert point.
	எனில் ((State & REPLACE_FLAG)
		&& fpos.lnum == Insstart.lnum
		&& fpos.col < Insstart.col)
	{
	    ptr += Insstart.col - fpos.col;
	    fpos.col = Insstart.col;
	}

	// compute virtual column numbers of first white and cursor
	getvcol(curwin, &fpos, &vcol, வெற்று, வெற்று, 0);
	getvcol(curwin, cursor, &want_vcol, வெற்று, வெற்று, 0);

	init_chartabsize_arg(&cts, curwin, 0, vcol, tab, tab);

	// Use as many TABs as possible.  Beware of 'breakindent', 'showbreak'
	// and 'linebreak' adding extra virtual columns.
	போது (VIM_ISWHITE(*ptr))
	{
	    i = lbr_chartabsize(&cts);
	    எனில் (cts.cts_vcol + i > want_vcol)
		நிறுத்து;
	    எனில் (*ptr != TAB)
	    {
		*ptr = TAB;
		எனில் (change_col < 0)
		{
		    change_col = fpos.col;  // Column of first change
		    // May have to adjust Insstart
		    எனில் (fpos.lnum == Insstart.lnum && fpos.col < Insstart.col)
			Insstart.col = fpos.col;
		}
	    }
	    ++fpos.col;
	    ++ptr;
	    cts.cts_vcol += i;
	}
	vcol = cts.cts_vcol;
	clear_chartabsize_arg(&cts);

	எனில் (change_col >= 0)
	{
	    முழு		    repl_off = 0;

	    // Skip over the spaces we need.
	    init_chartabsize_arg(&cts, curwin, 0, vcol, ptr, ptr);
	    போது (cts.cts_vcol < want_vcol && *cts.cts_ptr == ' ')
	    {
		cts.cts_vcol += lbr_chartabsize(&cts);
		++cts.cts_ptr;
		++repl_off;
	    }
	    ptr = cts.cts_ptr;
	    vcol = cts.cts_vcol;
	    clear_chartabsize_arg(&cts);

	    எனில் (vcol > want_vcol)
	    {
		// Must have a char with 'showbreak' just before it.
		--ptr;
		--repl_off;
	    }
	    fpos.col += repl_off;

	    // Delete following spaces.
	    i = cursor->col - fpos.col;
	    எனில் (i > 0)
	    {
#ifdef FEAT_PROP_POPUP
		எனில் (!(State & VREPLACE_FLAG))
		{
		    char_u  *newp;
		    முழு	    col;

		    newp = alloc(curbuf->b_ml.ml_வரி_len - i);
		    எனில் (newp == வெற்று)
			திரும்பு FALSE;

		    col = ptr - curbuf->b_ml.ml_வரி_ptr;
		    எனில் (col > 0)
			mch_memmove(newp, ptr - col, col);
		    mch_memmove(newp + col, ptr + i,
					   curbuf->b_ml.ml_வரி_len - col - i);

		    எனில் (curbuf->b_ml.ml_flags & (ML_வரி_DIRTY | ML_ALLOCATED))
			vim_free(curbuf->b_ml.ml_வரி_ptr);
		    curbuf->b_ml.ml_வரி_ptr = newp;
		    curbuf->b_ml.ml_வரி_len -= i;
		    curbuf->b_ml.ml_வரி_textlen = 0;
		    curbuf->b_ml.ml_flags =
			   (curbuf->b_ml.ml_flags | ML_வரி_DIRTY) & ~ML_EMPTY;
		}
		இல்லையெனில்
#endif
		    STRMOVE(ptr, ptr + i);
		// correct replace stack.
		எனில் ((State & REPLACE_FLAG) && !(State & VREPLACE_FLAG))
		    க்கு (temp = i; --temp >= 0; )
			replace_join(repl_off);
	    }
#ifdef FEAT_NETBEANS_INTG
	    எனில் (netbeans_active())
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
	    எனில் (State & VREPLACE_FLAG)
	    {
		// Backspace from real cursor to change_col
		backspace_until_column(change_col);

		// Insert each char in saved_line from changed_col to
		// ptr-cursor
		ins_bytes_len(saved_வரி + change_col,
						    cursor->col - change_col);
	    }
	}

	எனில் (State & VREPLACE_FLAG)
	    vim_free(saved_வரி);
	curwin->w_p_list = save_list;
    }

    திரும்பு FALSE;
}

/*
 * Handle CR or NL in insert mode.
 * Return FAIL when out of memory or can't undo.
 */
    முழு
ins_eol(முழு c)
{
    முழு	    i;

    எனில் (echeck_abbr(c + ABBR_OFF))
	திரும்பு OK;
    எனில் (stop_arrow() == FAIL)
	திரும்பு FAIL;
    undisplay_dollar();

    /*
     * Strange Vi behaviour: In Replace mode, typing a NL will not delete the
     * character under the cursor.  Only push a NUL on the replace stack,
     * nothing to put back when the NL is deleted.
     */
    எனில் ((State & REPLACE_FLAG) && !(State & VREPLACE_FLAG))
	replace_தள்ளு(NUL);

    /*
     * In MODE_VREPLACE state, a NL replaces the rest of the line, and starts
     * replacing the next line, so we push all of the characters left on the
     * line onto the replace stack.  This is not done here though, it is done
     * in open_line().
     */

    // Put cursor on NUL if on the last char and coladd is 1 (happens after
    // CTRL-O).
    எனில் (virtual_active() && curwin->w_cursor.coladd > 0)
	coladvance(getviscol());

#ifdef FEAT_RIGHTLEFT
    // NL in reverse insert will always start in the end of
    // current line.
    எனில் (revins_on)
	curwin->w_cursor.col += ml_get_cursor_len();
#endif

    AppendToRedobuff(NL_STR);
    i = open_வரி(FORWARD,
	    has_format_option(FO_RET_COMS) ? OPENLINE_DO_COM : 0, old_indent,
	    வெற்று);
    old_indent = 0;
    can_cindent = TRUE;
#ifdef FEAT_FOLDING
    // When inserting a line the cursor line must never be in a closed fold.
    foldOpenCursor();
#endif

    திரும்பு i;
}

#ifdef FEAT_DIGRAPHS
/*
 * Handle digraph in insert mode.
 * Returns character still to be inserted, or NUL when nothing remaining to be
 * done.
 */
    நிலையான முழு
ins_digraph(வெறுமை)
{
    முழு	    c;
    முழு	    cc;
    முழு	    did_putchar = FALSE;

    pc_status = PC_STATUS_UNSET;
    எனில் (redrawing() && !char_avail())
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
    ++allow_சாவிகள்;
    c = plain_vgetc();
    --no_mapping;
    --allow_சாவிகள்;
    எனில் (did_putchar)
	// when the line fits in 'columns' the '?' is at the start of the next
	// line and will not be removed by the redraw
	edit_unputchar();

    எனில் (IS_SPECIAL(c) || mod_mask)	    // special key
    {
	clear_showcmd();
	செருகு_special(c, TRUE, FALSE);
	திரும்பு NUL;
    }
    எனில் (c != ESC)
    {
	did_putchar = FALSE;
	எனில் (redrawing() && !char_avail())
	{
	    // may need to redraw when no more chars available now
	    ins_redraw(FALSE);

	    எனில் (char2cells(c) == 1)
	    {
		ins_redraw(FALSE);
		edit_putchar(c, TRUE);
		did_putchar = TRUE;
	    }
	    add_to_showcmd_c(c);
	}
	++no_mapping;
	++allow_சாவிகள்;
	cc = plain_vgetc();
	--no_mapping;
	--allow_சாவிகள்;
	எனில் (did_putchar)
	    // when the line fits in 'columns' the '?' is at the start of the
	    // next line and will not be removed by a redraw
	    edit_unputchar();
	எனில் (cc != ESC)
	{
	    AppendToRedobuff((char_u *)CTRL_V_STR);
	    c = digraph_get(c, cc, TRUE);
	    clear_showcmd();
	    திரும்பு c;
	}
    }
    clear_showcmd();
    திரும்பு NUL;
}
#endif

/*
 * Handle CTRL-E and CTRL-Y in Insert mode: copy char from other line.
 * Returns the char to be inserted, or NUL if none found.
 */
    முழு
ins_copychar(linenr_T lnum)
{
    முழு		    c;
    char_u	    *ptr, *prev_ptr;
    char_u	    *வரி;
    chartabsize_T   cts;

    எனில் (lnum < 1 || lnum > curbuf->b_ml.ml_வரி_மொத்தம்)
    {
	vim_beep(BO_COPY);
	திரும்பு NUL;
    }

    // try to advance to the cursor column
    validate_virtcol();
    வரி = ml_get(lnum);
    prev_ptr = வரி;
    init_chartabsize_arg(&cts, curwin, lnum, 0, வரி, வரி);
    போது (cts.cts_vcol < curwin->w_virtcol && *cts.cts_ptr != NUL)
    {
	prev_ptr = cts.cts_ptr;
	cts.cts_vcol += lbr_chartabsize_adv(&cts);
    }
    எனில் (cts.cts_vcol > curwin->w_virtcol)
	ptr = prev_ptr;
    இல்லையெனில்
	ptr = cts.cts_ptr;
    clear_chartabsize_arg(&cts);

    c = (*mb_ptr2char)(ptr);
    எனில் (c == NUL)
	vim_beep(BO_COPY);
    திரும்பு c;
}

/*
 * CTRL-Y or CTRL-E typed in Insert mode.
 */
    நிலையான முழு
ins_ctrl_ey(முழு tc)
{
    முழு	    c = tc;

    எனில் (ctrl_x_mode_scroll())
    {
	எனில் (c == Ctrl_Y)
	    scrolldown_clamp();
	இல்லையெனில்
	    scrollup_clamp();
	redraw_later(UPD_VALID);
    }
    இல்லையெனில்
    {
	c = ins_copychar(curwin->w_cursor.lnum + (c == Ctrl_Y ? -1 : 1));
	எனில் (c != NUL)
	{
	    long	tw_save;

	    // The character must be taken literally, insert like it
	    // was typed after a CTRL-V, and pretend 'textwidth'
	    // wasn't set.  Digits, 'o' and 'x' are special after a
	    // CTRL-V, don't use it for these.
	    எனில் (c < 256 && !SAFE_isalnum(c))
		AppendToRedobuff((char_u *)CTRL_V_STR);	// CTRL-V
	    tw_save = curbuf->b_p_tw;
	    curbuf->b_p_tw = -1;
	    செருகு_special(c, TRUE, FALSE);
	    curbuf->b_p_tw = tw_save;
#ifdef FEAT_RIGHTLEFT
	    revins_chars++;
	    revins_legal++;
#endif
	    c = Ctrl_V;	// pretend CTRL-V is last character
	    auto_format(FALSE, TRUE);
	}
    }
    திரும்பு c;
}

/*
 * Get the value that w_virtcol would have when 'list' is off.
 * Unless 'cpo' contains the 'L' flag.
 */
    colnr_T
get_nolist_virtcol(வெறுமை)
{
    // check validity of cursor in current buffer
    எனில் (curwin->w_buffer == வெற்று
	|| curwin->w_buffer->b_ml.ml_mfp == வெற்று
	|| curwin->w_cursor.lnum > curwin->w_buffer->b_ml.ml_வரி_மொத்தம்)
	திரும்பு 0;
    எனில் (curwin->w_p_list && vim_strchr(p_cpo, CPO_LISTWM) == வெற்று)
	திரும்பு getvcol_nolist(&curwin->w_cursor);
    validate_virtcol();
    திரும்பு curwin->w_virtcol;
}

#எனில் defined(FEAT_EVAL)
/*
 * Handle the InsertCharPre autocommand.
 * "c" is the character that was typed.
 * Return a pointer to allocated memory with the replacement string.
 * Return NULL to continue inserting "c".
 */
    நிலையான char_u *
do_செருகு_char_pre(முழு c)
{
    char_u	*res;
    char_u	buf[MB_MAXBYTES + 1];
    size_t	buflen;
    முழு		save_State = State;

    // Return quickly when there is nothing to do.
    எனில் (!has_insertcharpre())
	திரும்பு வெற்று;

    எனில் (c == Ctrl_RSB)
	திரும்பு வெற்று;

    எனில் (has_mbyte)
	buflen = (*mb_char2bytes)(c, buf);
    இல்லையெனில்
    {
	buf[0] = c;
	buflen = 1;
    }
    buf[buflen] = NUL;

    // Lock the text to avoid weird things from happening.
    ++textlock;
    set_vim_var_string(VV_CHAR, buf, (முழு)buflen);  // set v:char

    res = வெற்று;
    எனில் (ins_apply_autocmds(EVENT_INSERTCHARPRE))
    {
	// Get the value of v:char.  It may be empty or more than one
	// character.  Only use it when changed, otherwise continue with the
	// original character to avoid breaking autoindent.
	எனில் (STRCMP(buf, get_vim_var_str(VV_CHAR)) != 0)
	    res = vim_strsave(get_vim_var_str(VV_CHAR));
    }

    set_vim_var_string(VV_CHAR, வெற்று, -1);  // clear v:char
    --textlock;

    // Restore the State, it may have been changed.
    State = save_State;

    திரும்பு res;
}
#endif

    முழு
get_can_cindent(வெறுமை)
{
    திரும்பு can_cindent;
}

    வெறுமை
set_can_cindent(முழு val)
{
    can_cindent = val;
}

/*
 * Trigger "event" and take care of fixing undo.
 */
    முழு
ins_apply_autocmds(event_T event)
{
    varnumber_T	tick = CHANGEDTICK(curbuf);
    முழு r;

    r = apply_autocmds(event, வெற்று, வெற்று, FALSE, curbuf);

    // If u_savesub() was called then we are not prepared to start
    // a new line.  Call u_save() with no contents to fix that.
    // Except when leaving Insert mode.
    எனில் (event != EVENT_INSERTLEAVE && tick != CHANGEDTICK(curbuf))
	u_save(curwin->w_cursor.lnum, (linenr_T)(curwin->w_cursor.lnum + 1));

    திரும்பு r;
}
