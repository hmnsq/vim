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

#അന്തര്ഭാവ "vim.h"

#define BACKSPACE_CHAR		    1
#define BACKSPACE_WORD		    2
#define BACKSPACE_WORD_NOT_SPACE    3
#define BACKSPACE_വരി		    4

// Set when doing something for completion that may call edit() recursively,
// which is not allowed.
സ്ഥിതം പൂർണ്ണസംഖ്യ	compl_busy = FALSE;


സ്ഥിതം ശൂന്യം ins_ctrl_v(ശൂന്യം);
സ്ഥിതം ശൂന്യം തിരുകുക_special(പൂർണ്ണസംഖ്യ, പൂർണ്ണസംഖ്യ, പൂർണ്ണസംഖ്യ);
സ്ഥിതം ശൂന്യം redo_literal(പൂർണ്ണസംഖ്യ c);
സ്ഥിതം ശൂന്യം start_arrow_common(pos_T *end_തിരുകുക_pos, പൂർണ്ണസംഖ്യ change);
#ifdef FEAT_SPELL
സ്ഥിതം ശൂന്യം check_spell_redraw(ശൂന്യം);
#endif
സ്ഥിതം ശൂന്യം stop_തിരുകുക(pos_T *end_തിരുകുക_pos, പൂർണ്ണസംഖ്യ esc, പൂർണ്ണസംഖ്യ nomove);
സ്ഥിതം പൂർണ്ണസംഖ്യ  echeck_abbr(പൂർണ്ണസംഖ്യ);
സ്ഥിതം ശൂന്യം mb_replace_എടുക്കുക_ins(പൂർണ്ണസംഖ്യ cc);
സ്ഥിതം ശൂന്യം replace_flush(ശൂന്യം);
സ്ഥിതം ശൂന്യം replace_do_bs(പൂർണ്ണസംഖ്യ limit_col);
സ്ഥിതം പൂർണ്ണസംഖ്യ del_char_after_col(പൂർണ്ണസംഖ്യ limit_col);
സ്ഥിതം ശൂന്യം ins_reg(ശൂന്യം);
സ്ഥിതം ശൂന്യം ins_ctrl_g(ശൂന്യം);
സ്ഥിതം ശൂന്യം ins_ctrl_hat(ശൂന്യം);
സ്ഥിതം പൂർണ്ണസംഖ്യ  ins_esc(long *എണ്ണം, പൂർണ്ണസംഖ്യ cmdchar, പൂർണ്ണസംഖ്യ nomove);
#ifdef FEAT_RIGHTLEFT
സ്ഥിതം ശൂന്യം ins_ctrl_(ശൂന്യം);
#endif
സ്ഥിതം പൂർണ്ണസംഖ്യ ins_start_select(പൂർണ്ണസംഖ്യ c);
സ്ഥിതം ശൂന്യം ins_തിരുകുക(പൂർണ്ണസംഖ്യ replaceState);
സ്ഥിതം ശൂന്യം ins_ctrl_o(ശൂന്യം);
സ്ഥിതം ശൂന്യം ins_shift(പൂർണ്ണസംഖ്യ c, പൂർണ്ണസംഖ്യ lastc);
സ്ഥിതം ശൂന്യം ins_del(ശൂന്യം);
സ്ഥിതം പൂർണ്ണസംഖ്യ  ins_bs(പൂർണ്ണസംഖ്യ c, പൂർണ്ണസംഖ്യ mode, പൂർണ്ണസംഖ്യ *inserted_space_p);
#എങ്കിൽ defined(FEAT_GUI_TABLINE)
സ്ഥിതം ശൂന്യം ins_tabline(പൂർണ്ണസംഖ്യ c);
#endif
സ്ഥിതം ശൂന്യം ins_ഇടത്(ശൂന്യം);
സ്ഥിതം ശൂന്യം ins_home(പൂർണ്ണസംഖ്യ c);
സ്ഥിതം ശൂന്യം ins_end(പൂർണ്ണസംഖ്യ c);
സ്ഥിതം ശൂന്യം ins_s_ഇടത്(ശൂന്യം);
സ്ഥിതം ശൂന്യം ins_വലത്(ശൂന്യം);
സ്ഥിതം ശൂന്യം ins_s_വലത്(ശൂന്യം);
സ്ഥിതം ശൂന്യം ins_up(പൂർണ്ണസംഖ്യ startcol);
സ്ഥിതം ശൂന്യം ins_pageup(ശൂന്യം);
സ്ഥിതം ശൂന്യം ins_down(പൂർണ്ണസംഖ്യ startcol);
സ്ഥിതം ശൂന്യം ins_pagedown(ശൂന്യം);
#ifdef FEAT_DND
സ്ഥിതം ശൂന്യം ins_drop(ശൂന്യം);
#endif
സ്ഥിതം പൂർണ്ണസംഖ്യ  ins_tab(ശൂന്യം);
#ifdef FEAT_DIGRAPHS
സ്ഥിതം പൂർണ്ണസംഖ്യ  ins_digraph(ശൂന്യം);
#endif
സ്ഥിതം പൂർണ്ണസംഖ്യ  ins_ctrl_ey(പൂർണ്ണസംഖ്യ tc);
#എങ്കിൽ defined(FEAT_EVAL)
സ്ഥിതം char_u *do_തിരുകുക_char_pre(പൂർണ്ണസംഖ്യ c);
#endif

സ്ഥിതം colnr_T	Insstart_textlen;	// length of line when insert started
സ്ഥിതം colnr_T	Insstart_blank_vcol;	// vcol for first inserted blank
സ്ഥിതം പൂർണ്ണസംഖ്യ	update_Insstart_orig = TRUE; // set Insstart_orig to Insstart

സ്ഥിതം string_T	last_തിരുകുക = {ശൂന്യ, 0};    // the text of the previous insert,
					    // K_SPECIAL and CSI are escaped
സ്ഥിതം പൂർണ്ണസംഖ്യ	last_തിരുകുക_skip; // nr of chars in front of previous insert
സ്ഥിതം പൂർണ്ണസംഖ്യ	new_തിരുകുക_skip;  // nr of chars in front of current insert
സ്ഥിതം പൂർണ്ണസംഖ്യ	did_restart_edit;	// "restart_edit" when calling edit()

സ്ഥിതം പൂർണ്ണസംഖ്യ	can_cindent;		// may do cindenting on this line

#ifdef FEAT_RIGHTLEFT
സ്ഥിതം പൂർണ്ണസംഖ്യ	revins_on;		// reverse insert mode on
സ്ഥിതം പൂർണ്ണസംഖ്യ	revins_chars;		// how much to skip after edit
സ്ഥിതം പൂർണ്ണസംഖ്യ	revins_legal;		// was the last char 'legal'?
സ്ഥിതം പൂർണ്ണസംഖ്യ	revins_scol;		// start column of revins session
#endif

സ്ഥിതം പൂർണ്ണസംഖ്യ	ins_need_undo;		// call u_save() before inserting a
					// char.  Set when edit() is called.
					// after that arrow_used is used.

സ്ഥിതം പൂർണ്ണസംഖ്യ	dont_sync_undo = FALSE;	// CTRL-G U prevents syncing undo for
					// the next left/right cursor key

// With 'autocompletedelay' set, arm the delay and let the main loop fire
// Insert-mode autocommands; the popup is shown later on K_COMPLETE_DELAY.
// Otherwise trigger completion right away.
#define TRIGGER_AUTOCOMPLETE()			\
    ചെയ്യുക {					\
	update_screen(UPD_VALID);  /* Show char (deletion) immediately */ \
	out_flush();				\
	ins_compl_enable_autocomplete();	\
	ins_compl_arm_autostart();		\
	എങ്കിൽ (!ins_compl_arm_autocomplete_delay())\
	    പോകുക docomplete;			\
    } ആകുമ്പോൾ (0)

#define MAY_TRIGGER_AUTOCOMPLETE(c)				\
    ചെയ്യുക {							\
	എങ്കിൽ (ins_compl_has_autocomplete() && !char_avail()	\
		&& curwin->w_cursor.col > 0)			\
	{							\
	    (c) = char_before_cursor();				\
	    എങ്കിൽ (vim_isprintc(c))				\
		TRIGGER_AUTOCOMPLETE();				\
	}							\
    } ആകുമ്പോൾ (0)

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
    പൂർണ്ണസംഖ്യ
edit(
    പൂർണ്ണസംഖ്യ		cmdchar,
    പൂർണ്ണസംഖ്യ		startln,	// if set, insert at start of line
    long	എണ്ണം)
{
    പൂർണ്ണസംഖ്യ		c = 0;
    char_u	*ptr;
    പൂർണ്ണസംഖ്യ		lastc = 0;
    പൂർണ്ണസംഖ്യ		mincol;
    സ്ഥിതം linenr_T o_lnum = 0;
    പൂർണ്ണസംഖ്യ		i;
    പൂർണ്ണസംഖ്യ		did_backspace = TRUE;	    // previous char was backspace
    പൂർണ്ണസംഖ്യ		വരി_is_white = FALSE;	    // line is empty before insert
    linenr_T	old_topline = 0;	    // topline before insertion
#ifdef FEAT_DIFF
    പൂർണ്ണസംഖ്യ		old_topfill = -1;
#endif
    പൂർണ്ണസംഖ്യ		inserted_space = FALSE;     // just inserted a space
    പൂർണ്ണസംഖ്യ		replaceState = MODE_REPLACE;
    പൂർണ്ണസംഖ്യ		nomove = FALSE;		    // don't move cursor on return
#ifdef FEAT_JOB_CHANNEL
    പൂർണ്ണസംഖ്യ		cmdchar_todo = cmdchar;
#endif
#ifdef FEAT_CONCEAL
    പൂർണ്ണസംഖ്യ		cursor_വരി_was_concealed;
#endif
    പൂർണ്ണസംഖ്യ		ins_just_started = TRUE;

    // Remember whether editing was restarted after CTRL-O.
    did_restart_edit = restart_edit;

    // sleep before redrawing, needed for "CTRL-O :" that results in an
    // error message
    check_for_delay(TRUE);

    // set Insstart_orig to Insstart
    update_Insstart_orig = TRUE;

#ifdef HAVE_SANDBOX
    // Don't allow inserting in the sandbox.
    എങ്കിൽ (sandbox != 0)
    {
	emsg(_(e_not_allowed_in_sandbox));
	തിരികെ FALSE;
    }
#endif
    // Don't allow changes in the buffer while editing the cmdline.  The
    // caller of getcmdline() may get confused.
    // Don't allow recursive insert mode when busy with completion.
    എങ്കിൽ (textlock != 0 || ins_compl_active() || compl_busy || pum_visible())
    {
	emsg(_(e_not_allowed_to_change_text_or_change_window));
	തിരികെ FALSE;
    }
    ins_compl_clear();	    // clear stuff for CTRL-X mode

    /*
     * Trigger InsertEnter autocommands.  Do not do this for "r<CR>" or "grx".
     */
    എങ്കിൽ (cmdchar != 'r' && cmdchar != 'v')
    {
	pos_T   save_cursor = curwin->w_cursor;

#ifdef FEAT_EVAL
	എങ്കിൽ (cmdchar == 'R')
	    ptr = (char_u *)"r";
	അല്ലെങ്കിൽ എങ്കിൽ (cmdchar == 'V')
	    ptr = (char_u *)"v";
	അല്ലെങ്കിൽ
	    ptr = (char_u *)"i";
	set_vim_var_string(VV_INSERTMODE, ptr, 1);
	set_vim_var_string(VV_CHAR, ശൂന്യ, -1);  // clear v:char
#endif
	ins_apply_autocmds(EVENT_INSERTENTER);

	// Check for changed highlighting, e.g. for ModeMsg.
	എങ്കിൽ (need_highlight_changed)
	    highlight_changed();

	// Make sure the cursor didn't move.  Do call check_cursor_col() in
	// case the text was modified.  Since Insert mode was not started yet
	// a call to check_cursor_col() may move the cursor, especially with
	// the "A" command, thus set State to avoid that. Also check that the
	// line number is still valid (lines may have been deleted).
	// Do not restore if v:char was set to a non-empty string.
	എങ്കിൽ (!EQUAL_POS(curwin->w_cursor, save_cursor)
#ifdef FEAT_EVAL
		&& *get_vim_var_str(VV_CHAR) == NUL
#endif
		&& save_cursor.lnum <= curbuf->b_ml.ml_വരി_എണ്ണം)
	{
	    പൂർണ്ണസംഖ്യ save_state = State;

	    curwin->w_cursor = save_cursor;
	    State = MODE_തിരുകുക;
	    check_cursor_col();
	    State = save_state;
	}
    }

#ifdef FEAT_CONCEAL
    // Check if the cursor line was concealed before changing State.
    cursor_വരി_was_concealed = curwin->w_p_cole > 0
						&& conceal_cursor_വരി(curwin);
#endif

    /*
     * When doing a paste with the middle mouse button, Insstart is set to
     * where the paste started.
     */
    എങ്കിൽ (where_paste_started.lnum != 0)
	Insstart = where_paste_started;
    അല്ലെങ്കിൽ
    {
	Insstart = curwin->w_cursor;
	എങ്കിൽ (startln)
	    Insstart.col = 0;
    }
    Insstart_textlen = (colnr_T)linetabsize_str(ml_get_curline());
    Insstart_blank_vcol = MAXCOL;
    എങ്കിൽ (!did_ai)
	ai_col = 0;

    എങ്കിൽ (cmdchar != NUL && restart_edit == 0)
    {
	ResetRedobuff();
	Append_സംഖ്യ_To_Redobuff(എണ്ണം);
	എങ്കിൽ (cmdchar == 'V' || cmdchar == 'v')
	{
	    // "gR" or "gr" command
	    AppendCharToRedobuff('g');
	    AppendCharToRedobuff((cmdchar == 'v') ? 'r' : 'R');
	}
	അല്ലെങ്കിൽ
	{
	    എങ്കിൽ (cmdchar == K_PS)
		AppendCharToRedobuff('a');
	    അല്ലെങ്കിൽ
		AppendCharToRedobuff(cmdchar);
	    എങ്കിൽ (cmdchar == 'g')		    // "gI" command
		AppendCharToRedobuff('I');
	    അല്ലെങ്കിൽ എങ്കിൽ (cmdchar == 'r')	    // "r<CR>" command
		എണ്ണം = 1;		    // insert only one <CR>
	}
    }

    എങ്കിൽ (cmdchar == 'R')
    {
	State = MODE_REPLACE;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (cmdchar == 'V' || cmdchar == 'v')
    {
	State = MODE_VREPLACE;
	replaceState = MODE_VREPLACE;
	orig_വരി_എണ്ണം = curbuf->b_ml.ml_വരി_എണ്ണം;
	vr_വരികൾ_changed = 1;
    }
    അല്ലെങ്കിൽ
	State = MODE_തിരുകുക;

    may_trigger_modechanged();
    stop_തിരുകുക_mode = FALSE;

#ifdef FEAT_CONCEAL
    // Check if the cursor line needs redrawing after changing State.  If
    // 'concealcursor' is "n" it needs to be redrawn without concealing.
    conceal_check_cursor_വരി(cursor_വരി_was_concealed);
#endif

    // Need to position cursor again when on a TAB and when on a char with
    // virtual text.
    എങ്കിൽ (gchar_cursor() == TAB
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
    എങ്കിൽ (curbuf->b_p_iminsert == B_IMODE_LMAP)
	State |= MODE_LANGMAP;
#ifdef HAVE_നിവേശം_METHOD
    im_set_active(curbuf->b_p_iminsert == B_IMODE_IM);
#endif

    setmouse();
    clear_showcmd();
#ifdef FEAT_RIGHTLEFT
    // there is no reverse replace mode
    revins_on = (State == MODE_തിരുകുക && p_ri);
    എങ്കിൽ (revins_on)
	undisplay_dollar();
    revins_chars = 0;
    revins_legal = 0;
    revins_scol = -1;
#endif
    എങ്കിൽ (!p_ek)
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
    എങ്കിൽ (restart_edit != 0 && stuff_empty())
    {
	/*
	 * After a paste we consider text typed to be part of the insert for
	 * the pasted text. You can backspace over the pasted text too.
	 */
	എങ്കിൽ (where_paste_started.lnum)
	    arrow_used = FALSE;
	അല്ലെങ്കിൽ
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
	എങ്കിൽ (((ins_at_eol && curwin->w_cursor.lnum == o_lnum)
		    || curwin->w_curswant > curwin->w_virtcol)
		&& *(ptr = ml_get_curline() + curwin->w_cursor.col) != NUL)
	{
	    എങ്കിൽ (ptr[1] == NUL)
		++curwin->w_cursor.col;
	    അല്ലെങ്കിൽ എങ്കിൽ (has_mbyte)
	    {
		i = (*mb_ptr2len)(ptr);
		എങ്കിൽ (ptr[i] == NUL)
		    curwin->w_cursor.col += i;
	    }
	}
	ins_at_eol = FALSE;
    }
    അല്ലെങ്കിൽ
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
    എങ്കിൽ (!p_im && did_restart_edit == 0)
	foldOpenCursor();
#endif

    /*
     * If 'showmode' is set, show the current (insert/replace/..) mode.
     * A warning message for changing a readonly file is given here, before
     * actually changing anything.  It's put after the mode, if any.
     */
    i = 0;
    എങ്കിൽ (p_smd && സന്ദേശം_silent == 0)
	i = showmode();

    എങ്കിൽ (!p_im && did_restart_edit == 0)
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
    new_തിരുകുക_skip = (പൂർണ്ണസംഖ്യ)inserted.length;
    എങ്കിൽ (inserted.string != ശൂന്യ)
	vim_free(inserted.string);

    old_indent = 0;

    /*
     * Main loop in Insert mode: repeat until Insert mode is left.
     */
    വേണ്ടി (;;)
    {
#ifdef FEAT_RIGHTLEFT
	എങ്കിൽ (!revins_legal)
	    revins_scol = -1;	    // reset on illegal motions
	അല്ലെങ്കിൽ
	    revins_legal = 0;
#endif
	എങ്കിൽ (arrow_used)	    // don't repeat insert when arrow key used
	    എണ്ണം = 0;

	എങ്കിൽ (update_Insstart_orig)
	    Insstart_orig = Insstart;

	എങ്കിൽ (stop_തിരുകുക_mode && !ins_compl_active())
	{
	    // ":stopinsert" used or 'insertmode' reset
	    എണ്ണം = 0;
	    പോകുക doESCkey;
	}

	// set curwin->w_curswant for next K_DOWN or K_UP
	എങ്കിൽ (!arrow_used)
	    curwin->w_set_curswant = true;

	// If there is no typeahead may check for timestamps (e.g., for when a
	// menu invoked a shell command).
	എങ്കിൽ (stuff_empty())
	{
	    did_check_timestamps = FALSE;
	    എങ്കിൽ (need_check_timestamps)
		check_timestamps(FALSE);
	}

	/*
	 * When emsg() was called msg_scroll will have been set.
	 */
	സന്ദേശം_scroll = FALSE;

#ifdef FEAT_GUI
	// When 'mousefocus' is set a mouse movement may have taken us to
	// another window.  "need_mouse_correct" may then be set because of an
	// autocommand.
	എങ്കിൽ (need_mouse_correct)
	    gui_mouse_correct();
#endif

#ifdef FEAT_FOLDING
	// Open fold at the cursor line, according to 'foldopen'.
	എങ്കിൽ (fdo_flags & FDO_തിരുകുക)
	    foldOpenCursor();
	// Close folds where the cursor isn't, according to 'foldclose'
	എങ്കിൽ (!char_avail())
	    foldCheckClose();
#endif

#ifdef FEAT_JOB_CHANNEL
	എങ്കിൽ (bt_prompt(curbuf))
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
	എങ്കിൽ (curbuf->b_mod_set
		&& curwin->w_p_wrap
		&& !curwin->w_p_sms
		&& !did_backspace
		&& curwin->w_topline == old_topline
#ifdef FEAT_DIFF
		&& curwin->w_topfill == old_topfill
#endif
		&& എണ്ണം <= 1
		)
	{
	    mincol = curwin->w_wcol;
	    validate_cursor_col();

	    എങ്കിൽ (
#ifdef FEAT_VARTABS
		curwin->w_wcol < mincol - tabstop_at(get_nolist_virtcol(),
						     curbuf->b_p_ts,
						     curbuf->b_p_vts_array,
						     FALSE)
#അല്ലെങ്കിൽ
		(പൂർണ്ണസംഖ്യ)curwin->w_wcol < mincol - curbuf->b_p_ts
#endif
		    && curwin->w_wrow ==
				   curwin->w_height - 1 - get_scrolloff_മൂല്യം()
		    && (curwin->w_cursor.lnum != curwin->w_topline
#ifdef FEAT_DIFF
			|| curwin->w_topfill > 0
#endif
		    ))
	    {
#ifdef FEAT_DIFF
		എങ്കിൽ (curwin->w_topfill > 0)
		    --curwin->w_topfill;
		അല്ലെങ്കിൽ
#endif
#ifdef FEAT_FOLDING
		എങ്കിൽ (hasFolding(curwin->w_topline, ശൂന്യ, &old_topline))
		    set_topline(curwin, old_topline + 1);
		അല്ലെങ്കിൽ
#endif
		    set_topline(curwin, curwin->w_topline + 1);
	    }
	}

	// May need to adjust w_topline to show the cursor.
	എങ്കിൽ (എണ്ണം <= 1)
	    update_topline();

	did_backspace = FALSE;

	എങ്കിൽ (എണ്ണം <= 1)
	    validate_cursor();		// may set must_redraw

	/*
	 * Redraw the display when no characters are waiting.
	 * Also shows mode, ruler and positions cursor.
	 */
	ins_redraw(TRUE);

	എങ്കിൽ (curwin->w_p_scb)
	    do_check_scrollbind(TRUE);

	എങ്കിൽ (curwin->w_p_crb)
	    do_check_cursorbind();
	എങ്കിൽ (എണ്ണം <= 1)
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
	എങ്കിൽ (c != K_CURSORHOLD && c != K_COMPLETE_DELAY)
	    lastc = c;		// remember the previous char for CTRL-D

	// After using CTRL-G U the next cursor key will not break undo.
	എങ്കിൽ (dont_sync_undo == MAYBE)
	    dont_sync_undo = TRUE;
	അല്ലെങ്കിൽ
	    dont_sync_undo = FALSE;
	എങ്കിൽ (cmdchar == K_PS)
	    // Got here from normal mode when bracketed paste started.
	    c = K_PS;
	അല്ലെങ്കിൽ
	{
	    // Trigger autocomplete when entering Insert mode, either directly
	    // or via change commands like 'ciw', 'cw', etc., before the first
	    // character is typed.
	    എങ്കിൽ (ins_just_started)
	    {
		ins_just_started = FALSE;
		എങ്കിൽ (ins_compl_has_autocomplete() && !char_avail()
			&& curwin->w_cursor.col > 0)
		{
		    c = char_before_cursor();
		    എങ്കിൽ (vim_isprintc(c))
		    {
			ins_compl_enable_autocomplete();
			ins_compl_arm_autostart();
			ins_compl_init_get_longest();
#ifdef FEAT_RIGHTLEFT
			എങ്കിൽ (p_hkmap)
			    c = hkmap(c);		// Hebrew mode mapping
#endif
			// Defer until the delay expires (K_COMPLETE_DELAY), or
			// trigger now when no delay is in effect.
			എങ്കിൽ (!ins_compl_arm_autocomplete_delay())
			    പോകുക docomplete;
		    }
		}
	    }

	    ചെയ്യുക
	    {
		c = safe_vgetc();

		എങ്കിൽ (stop_തിരുകുക_mode
#ifdef FEAT_TERMINAL
			|| (c == K_IGNORE && term_use_loop())
#endif
		   )
		{
		    // Insert mode ended, possibly from a callback, or a timer
		    // must have opened a terminal window.
		    എങ്കിൽ (c != K_IGNORE && c != K_NOP)
			vungetc(c);
		    എണ്ണം = 0;

		    എങ്കിൽ (!bt_prompt(curwin->w_buffer)
#ifdef FEAT_TERMINAL
			    && !bt_terminal(curwin->w_buffer)
#endif
			    && stop_തിരുകുക_mode)
			// :stopinsert command via callback or via server command
			nomove = FALSE;
		    അല്ലെങ്കിൽ
			nomove = TRUE;
		    ins_compl_prep(ESC);
		    പോകുക doESCkey;
		}
	    } ആകുമ്പോൾ (c == K_IGNORE || c == K_NOP);
	}

	// Don't want K_CURSORHOLD for the second key, e.g., after CTRL-V.
	did_cursorhold = TRUE;
	എങ്കിൽ (c != K_CURSORHOLD && c != K_COMPLETE_DELAY)
	{
	    // Don't want delayed autocompletion from the previous key either.
	    ins_compl_clear_autocomplete_delay();
	    ins_compl_disarm_autostart();
	    // A completion already on screen goes on being what it was.
	    എങ്കിൽ (!ins_compl_active())
		ins_compl_disable_autocomplete();
	}

#ifdef FEAT_RIGHTLEFT
	എങ്കിൽ (p_hkmap && താക്കോൽ_Typed)
	    c = hkmap(c);		// Hebrew mode mapping
#endif

	// If the window was made so small that nothing shows, make it at least
	// one line and one column when typing.
	എങ്കിൽ (താക്കോൽ_Typed && !താക്കോൽ_Stuffed)
	    win_ensure_size();

	/*
	 * Special handling of keys while the popup menu is visible or wanted
	 * and the cursor is still in the completed word.  Only when there is
	 * a match, skip this when no matches were found.
	 */
	എങ്കിൽ (ins_compl_active() && curwin->w_cursor.col >= ins_compl_col()
		&& ins_compl_has_shown_match() && pum_wanted())
	{
	    // BS: Delete one character from "compl_leader".
	    എങ്കിൽ ((c == K_BS || c == Ctrl_H)
			&& curwin->w_cursor.col > ins_compl_col()
			&& (c = ins_compl_bs()) == NUL)
		തുടരുക;

	    // When no match was selected or it was edited.
	    എങ്കിൽ (!ins_compl_used_match())
	    {
		// CTRL-L: Add one character from the current match to
		// "compl_leader".  Except when at the original match and
		// there is nothing to add, CTRL-L works like CTRL-P then.
		എങ്കിൽ (c == Ctrl_L
			&& (!ctrl_x_mode_വരി_or_eval()
			    || ins_compl_long_shown_match()))
		{
		    ins_compl_addfrommatch();
		    തുടരുക;
		}

		// A non-white character that fits in with the current
		// completion: Add to "compl_leader".
		എങ്കിൽ (ins_compl_accept_char(c))
		{
#എങ്കിൽ defined(FEAT_EVAL)
		    // Trigger InsertCharPre.
		    char_u *str = do_തിരുകുക_char_pre(c);
		    char_u *p;

		    എങ്കിൽ (str != ശൂന്യ)
		    {
			വേണ്ടി (p = str; *p != NUL; MB_PTR_ADV(p))
			    ins_compl_addleader(PTR2CHAR(p));
			vim_free(str);
		    }
		    അല്ലെങ്കിൽ
#endif
			ins_compl_addleader(c);
		    തുടരുക;
		}

		// Pressing CTRL-Y selects the current match.  When
		// ins_compl_enter_selects() is set the Enter key does the
		// same.
		എങ്കിൽ ((c == Ctrl_Y || (ins_compl_enter_selects()
				    && (c == CAR || c == K_KENTER || c == NL)))
			&& stop_arrow() == OK)
		{
		    ins_compl_delete();
		    എങ്കിൽ (ins_compl_preinsert_longest()
			    && !ins_compl_is_match_selected())
		    {
			ins_compl_തിരുകുക(FALSE, TRUE);
			ins_compl_init_get_longest();
			തുടരുക;
		    }
		    അല്ലെങ്കിൽ
			ins_compl_തിരുകുക(FALSE, FALSE);
		}
		// Delete preinserted text when typing special chars
		അല്ലെങ്കിൽ എങ്കിൽ (IS_WHITE_NL_OR_NUL(c) && ins_compl_preinsert_effect())
		    ins_compl_delete();
	    }
	}

	// Prepare for or stop CTRL-X mode.  This doesn't do completion, but
	// it does fix up the text when finishing completion.
	ins_compl_init_get_longest();
	എങ്കിൽ (ins_compl_prep(c))
	    തുടരുക;

	// CTRL-\ CTRL-N goes to Normal mode,
	// CTRL-\ CTRL-G goes to mode selected with 'insertmode',
	// CTRL-\ CTRL-O is like CTRL-O but without moving the cursor.
	എങ്കിൽ (c == Ctrl_BSL)
	{
	    // may need to redraw when no more chars available now
	    ins_redraw(FALSE);
	    ++no_mapping;
	    ++allow_താക്കോലുകൾ;
	    c = plain_vgetc();
	    --no_mapping;
	    --allow_താക്കോലുകൾ;
	    എങ്കിൽ (c != Ctrl_N && c != Ctrl_G && c != Ctrl_O)
	    {
		// it's something else
		vungetc(c);
		c = Ctrl_BSL;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (c == Ctrl_G && p_im)
		തുടരുക;
	    അല്ലെങ്കിൽ
	    {
		എങ്കിൽ (c == Ctrl_O)
		{
		    ins_ctrl_o();
		    ins_at_eol = FALSE;	// cursor keeps its column
		    nomove = TRUE;
		}
		എണ്ണം = 0;
		പോകുക doESCkey;
	    }
	}

#ifdef FEAT_DIGRAPHS
	c = do_digraph(c);
#endif

	എങ്കിൽ ((c == Ctrl_V || c == Ctrl_Q) && ctrl_x_mode_cmdline())
	    പോകുക docomplete;
	എങ്കിൽ (c == Ctrl_V || c == Ctrl_Q)
	{
	    ins_ctrl_v();
	    c = Ctrl_V;	// pretend CTRL-V is last typed character
	    തുടരുക;
	}

	എങ്കിൽ (cindent_on() && ctrl_x_mode_none())
	{
	    // A key name preceded by a bang means this key is not to be
	    // inserted.  Skip ahead to the re-indenting below.
	    // A key name preceded by a star means that indenting has to be
	    // done before inserting the key.
	    വരി_is_white = inindent(0);
	    എങ്കിൽ (in_cinkeys(c, '!', വരി_is_white))
		പോകുക force_cindent;
	    എങ്കിൽ (can_cindent && in_cinkeys(c, '*', വരി_is_white)
							&& stop_arrow() == OK)
		do_c_വ്യഞ്ജകം_indent();
	}

#ifdef FEAT_RIGHTLEFT
	എങ്കിൽ (curwin->w_p_rl)
	    തിരഞ്ഞെടുപ്പ് (c)
	    {
		അവസ്ഥ K_ഇടത്:	c = K_വലത്; നിർത്തുക;
		അവസ്ഥ K_S_ഇടത്:	c = K_S_വലത്; നിർത്തുക;
		അവസ്ഥ K_C_ഇടത്:	c = K_C_വലത്; നിർത്തുക;
		അവസ്ഥ K_വലത്:	c = K_ഇടത്; നിർത്തുക;
		അവസ്ഥ K_S_വലത്: c = K_S_ഇടത്; നിർത്തുക;
		അവസ്ഥ K_C_വലത്: c = K_C_ഇടത്; നിർത്തുക;
	    }
#endif

	/*
	 * If 'keymodel' contains "startsel", may start selection.  If it
	 * does, a CTRL-O and c will be stuffed, we need to get these
	 * characters.
	 */
	എങ്കിൽ (ins_start_select(c))
	    തുടരുക;

	/*
	 * The big switch to handle a character in insert mode.
	 */
	തിരഞ്ഞെടുപ്പ് (c)
	{
	അവസ്ഥ ESC:	// End input mode
	    എങ്കിൽ (echeck_abbr(ESC + ABBR_OFF))
		നിർത്തുക;
	    // FALLTHROUGH

	അവസ്ഥ Ctrl_C:	// End input mode
	    എങ്കിൽ (c == Ctrl_C && cmdwin_type != 0)
	    {
		// Close the cmdline window.
		cmdwin_result = K_IGNORE;
		got_int = FALSE; // don't stop executing autocommands et al.
		nomove = TRUE;
		പോകുക doESCkey;
	    }
#ifdef FEAT_JOB_CHANNEL
	    എങ്കിൽ (c == Ctrl_C && bt_prompt(curbuf))
	    {
		എങ്കിൽ (invoke_prompt_interrupt())
		{
		    എങ്കിൽ (!bt_prompt(curbuf))
			// buffer changed to a non-prompt buffer, get out of
			// Insert mode
			പോകുക doESCkey;
		    നിർത്തുക;
		}
	    }
#endif

#ifdef UNIX
do_intr:
#endif
	    // when 'insertmode' set, and not halfway a mapping, don't leave
	    // Insert mode
	    എങ്കിൽ (goto_im())
	    {
		എങ്കിൽ (got_int)
		{
		    (ശൂന്യം)vgetc();		// flush all buffers
		    got_int = FALSE;
		}
		അല്ലെങ്കിൽ
		    vim_beep(BO_IM);
		നിർത്തുക;
	    }
doESCkey:
	    // Drop a pending autocomplete so it does not outlive Insert mode.
	    ins_compl_clear_autocomplete_delay();
	    /*
	     * This is the ONLY return from edit()!
	     */
	    // Always update o_lnum, so that a "CTRL-O ." that adds a line
	    // still puts the cursor back after the inserted text.
	    എങ്കിൽ (ins_at_eol && gchar_cursor() == NUL)
		o_lnum = curwin->w_cursor.lnum;

	    എങ്കിൽ (ins_esc(&എണ്ണം, cmdchar, nomove))
	    {
		// When CTRL-C was typed got_int will be set, with the result
		// that the autocommands won't be executed. When mapped got_int
		// is not set, but let's keep the behavior the same.
		എങ്കിൽ (cmdchar != 'r' && cmdchar != 'v' && c != Ctrl_C)
		    ins_apply_autocmds(EVENT_INSERTLEAVE);
		did_cursorhold = FALSE;

		// ins_redraw() triggers TextChangedI only when no characters
		// are in the typeahead buffer, so reset curbuf->b_last_changedtick only
		// if the TextChangedI was not blocked by char_avail() (e.g. using :norm!)
		// and the TextChangedI autocommand has been triggered.
		എങ്കിൽ (!char_avail() && curbuf->b_last_changedtick_i == CHANGEDTICK(curbuf))
		    curbuf->b_last_changedtick = CHANGEDTICK(curbuf);
		തിരികെ (c == Ctrl_O);
	    }
	    തുടരുക;

	അവസ്ഥ Ctrl_Z:	// suspend when 'insertmode' set
	    എങ്കിൽ (!p_im)
		പോകുക normalchar;	// insert CTRL-Z as normal char
	    do_cmdline_cmd((char_u *)"stop");
#ifdef CURSOR_SHAPE
	    ui_cursor_shape();		// may need to update cursor shape
#endif
	    തുടരുക;

	അവസ്ഥ Ctrl_O:	// execute one command
#ifdef FEAT_COMPL_FUNC
	    എങ്കിൽ (ctrl_x_mode_omni())
		പോകുക docomplete;
#endif
	    എങ്കിൽ (echeck_abbr(Ctrl_O + ABBR_OFF))
		നിർത്തുക;
	    ins_ctrl_o();

	    // don't move the cursor left when 'virtualedit' has "onemore".
	    എങ്കിൽ (get_ve_flags() & VE_ONEMORE)
	    {
		ins_at_eol = FALSE;
		nomove = TRUE;
	    }
	    എണ്ണം = 0;
	    പോകുക doESCkey;

	അവസ്ഥ K_INS:	// toggle insert/replace mode
	അവസ്ഥ K_KINS:
	    ins_തിരുകുക(replaceState);
	    നിർത്തുക;

	അവസ്ഥ K_SELECT:	// end of Select mode mapping - ignore
	    നിർത്തുക;

	അവസ്ഥ K_HELP:	// Help key works like <ESC> <Help>
	അവസ്ഥ K_F1:
	അവസ്ഥ K_XF1:
	    stuffcharReadbuff(K_HELP);
	    എങ്കിൽ (p_im)
		need_start_insertmode = TRUE;
	    പോകുക doESCkey;

#ifdef FEAT_NETBEANS_INTG
	അവസ്ഥ K_F21:	// NetBeans command
	    ++no_mapping;		// don't map the next key hits
	    i = plain_vgetc();
	    --no_mapping;
	    netbeans_keycommand(i);
	    നിർത്തുക;
#endif

	അവസ്ഥ K_ZERO:	// Insert the previously inserted text.
	അവസ്ഥ NUL:
	അവസ്ഥ Ctrl_A:
	    // For ^@ the trailing ESC will end the insert, unless there is an
	    // error.
	    എങ്കിൽ (stuff_inserted(NUL, 1L, (c == Ctrl_A)) == FAIL
						   && c != Ctrl_A && !p_im)
		പോകുക doESCkey;		// quit insert mode
	    inserted_space = FALSE;
	    നിർത്തുക;

	അവസ്ഥ Ctrl_R:	// insert the contents of a register
	    എങ്കിൽ (ctrl_x_mode_register() && !ins_compl_active())
		പോകുക docomplete;
	    ins_reg();
	    auto_format(FALSE, TRUE);
	    inserted_space = FALSE;
	    നിർത്തുക;

	അവസ്ഥ Ctrl_G:	// commands starting with CTRL-G
	    ins_ctrl_g();
	    നിർത്തുക;

	അവസ്ഥ Ctrl_HAT:	// switch input mode and/or langmap
	    ins_ctrl_hat();
	    നിർത്തുക;

#ifdef FEAT_RIGHTLEFT
	അവസ്ഥ Ctrl__:	// switch between languages
	    എങ്കിൽ (!p_ari)
		പോകുക normalchar;
	    ins_ctrl_();
	    നിർത്തുക;
#endif

	അവസ്ഥ Ctrl_D:	// Make indent one shiftwidth smaller.
#എങ്കിൽ defined(FEAT_FIND_ID)
	    എങ്കിൽ (ctrl_x_mode_path_defines())
		പോകുക docomplete;
#endif
	    // FALLTHROUGH

	അവസ്ഥ Ctrl_T:	// Make indent one shiftwidth greater.
	    എങ്കിൽ (c == Ctrl_T && ctrl_x_mode_thesaurus())
	    {
		എങ്കിൽ (has_compl_option(FALSE))
		    പോകുക docomplete;
		നിർത്തുക;
	    }

	    ins_shift(c, lastc);
	    auto_format(FALSE, TRUE);
	    inserted_space = FALSE;
	    നിർത്തുക;

	അവസ്ഥ K_DEL:	// delete character under the cursor
	അവസ്ഥ K_KDEL:
	    ins_del();
	    auto_format(FALSE, TRUE);
	    നിർത്തുക;

	അവസ്ഥ K_BS:	// delete character before the cursor
	അവസ്ഥ K_S_BS:
	അവസ്ഥ Ctrl_H:
	    did_backspace = ins_bs(c, BACKSPACE_CHAR, &inserted_space);
	    auto_format(FALSE, TRUE);
	    എങ്കിൽ (did_backspace)
		MAY_TRIGGER_AUTOCOMPLETE(c);
	    നിർത്തുക;

	അവസ്ഥ Ctrl_W:	// delete word before the cursor
#ifdef FEAT_JOB_CHANNEL
	    എങ്കിൽ (bt_prompt(curbuf) && (mod_mask & MOD_MASK_SHIFT) == 0)
	    {
		// In a prompt window CTRL-W is used for window commands.
		// Use Shift-CTRL-W to delete a word.
		stuffcharReadbuff(Ctrl_W);
		restart_edit = 'A';
		nomove = TRUE;
		എണ്ണം = 0;
		പോകുക doESCkey;
	    }
#endif
	    did_backspace = ins_bs(c, BACKSPACE_WORD, &inserted_space);
	    auto_format(FALSE, TRUE);
	    എങ്കിൽ (did_backspace)
		MAY_TRIGGER_AUTOCOMPLETE(c);
	    നിർത്തുക;

	അവസ്ഥ Ctrl_U:	// delete all inserted text in current line
#ifdef FEAT_COMPL_FUNC
	    // CTRL-X CTRL-U completes with 'completefunc'.
	    എങ്കിൽ (ctrl_x_mode_function())
		പോകുക docomplete;
#endif
	    did_backspace = ins_bs(c, BACKSPACE_വരി, &inserted_space);
	    auto_format(FALSE, TRUE);
	    inserted_space = FALSE;
	    എങ്കിൽ (did_backspace)
		MAY_TRIGGER_AUTOCOMPLETE(c);
	    നിർത്തുക;

	അവസ്ഥ K_LEFTMOUSE:   // mouse keys
	അവസ്ഥ K_LEFTMOUSE_NM:
	അവസ്ഥ K_LEFTDRAG:
	അവസ്ഥ K_LEFTRELEASE:
	അവസ്ഥ K_LEFTRELEASE_NM:
	അവസ്ഥ K_MOUSEMOVE:
	അവസ്ഥ K_MIDDLEMOUSE:
	അവസ്ഥ K_MIDDLEDRAG:
	അവസ്ഥ K_MIDDLERELEASE:
	അവസ്ഥ K_RIGHTMOUSE:
	അവസ്ഥ K_RIGHTDRAG:
	അവസ്ഥ K_RIGHTRELEASE:
	അവസ്ഥ K_X1MOUSE:
	അവസ്ഥ K_X1DRAG:
	അവസ്ഥ K_X1RELEASE:
	അവസ്ഥ K_X2MOUSE:
	അവസ്ഥ K_X2DRAG:
	അവസ്ഥ K_X2RELEASE:
	    ins_mouse(c);
	    നിർത്തുക;

	അവസ്ഥ K_MOUSEDOWN: // Default action for scroll wheel up: scroll up
	    ins_mousescroll(MSCR_DOWN);
	    നിർത്തുക;

	അവസ്ഥ K_MOUSEUP:	// Default action for scroll wheel down: scroll down
	    ins_mousescroll(MSCR_UP);
	    നിർത്തുക;

	അവസ്ഥ K_MOUSELEFT: // Scroll wheel left
	    ins_mousescroll(MSCR_ഇടത്);
	    നിർത്തുക;

	അവസ്ഥ K_MOUSERIGHT: // Scroll wheel right
	    ins_mousescroll(MSCR_വലത്);
	    നിർത്തുക;

	അവസ്ഥ K_PS:
	    bracketed_paste(PASTE_തിരുകുക, FALSE, ശൂന്യ);
	    എങ്കിൽ (cmdchar == K_PS)
		// invoked from normal mode, bail out
		പോകുക doESCkey;
	    നിർത്തുക;
	അവസ്ഥ K_PE:
	    // Got K_PE without K_PS, ignore.
	    നിർത്തുക;

#ifdef FEAT_GUI_TABLINE
	അവസ്ഥ K_TABLINE:
	അവസ്ഥ K_TABMENU:
	    ins_tabline(c);
	    നിർത്തുക;
#endif

	അവസ്ഥ K_IGNORE:	// Something mapped to nothing
	    നിർത്തുക;

	അവസ്ഥ K_COMMAND:		    // <Cmd>command<CR>
	അവസ്ഥ K_SCRIPT_COMMAND:	    // <ScriptCmd>command<CR>
	    {
		bufref_T    save_curbuf;
		varnumber_T tick = CHANGEDTICK(curbuf);

		set_bufref(&save_curbuf, curbuf);
		do_cmdkey_command(c, 0);

#ifdef FEAT_TERMINAL
		എങ്കിൽ (term_use_loop())
		    // Started a terminal that gets the input, exit Insert mode.
		    പോകുക doESCkey;
#endif
		എങ്കിൽ (curbuf->b_u_synced
			|| (bufref_valid(&save_curbuf)
			    && curbuf == save_curbuf.br_buf
			    && tick != CHANGEDTICK(curbuf)))
		{
		    // The command synced undo or changed this buffer.
		    // Save the cursor line before the next typed edit.
		    ins_need_undo = TRUE;
		}
	    }
	    നിർത്തുക;

	അവസ്ഥ K_CURSORHOLD:	// Didn't type something for a while.
	    ins_apply_autocmds(EVENT_CURSORHOLDI);
	    did_cursorhold = TRUE;
	    // If CTRL-G U was used apply it to the next typed key.
	    എങ്കിൽ (dont_sync_undo == TRUE)
		dont_sync_undo = MAYBE;
	    നിർത്തുക;

	അവസ്ഥ K_COMPLETE_DELAY:	// 'autocompletedelay' expired
	    // If CTRL-G U was used apply it to the next typed key.
	    എങ്കിൽ (dont_sync_undo == TRUE)
		dont_sync_undo = MAYBE;
	    ins_compl_clear_autocomplete_delay();
	    എങ്കിൽ (!ins_compl_has_autocomplete() || char_avail()
		    || curwin->w_cursor.col == 0)
		നിർത്തുക;
	    c = char_before_cursor();
	    എങ്കിൽ (!vim_isprintc(c))
		നിർത്തുക;
	    // The completion may have been cleared while waiting, so re-enable
	    // autocomplete to match a zero delay.
	    ins_compl_enable_autocomplete();
	    ins_compl_arm_autostart();
	    പോകുക docomplete;

#ifdef FEAT_GUI_MSWIN
	    // On MS-Windows ignore <M-F4>, we get it when closing the window
	    // was cancelled.
	അവസ്ഥ K_F4:
	    എങ്കിൽ (mod_mask != MOD_MASK_ALT)
		പോകുക normalchar;
	    നിർത്തുക;
#endif

#ifdef FEAT_GUI
	അവസ്ഥ K_VER_SCROLLBAR:
	    ins_scroll();
	    നിർത്തുക;

	അവസ്ഥ K_HOR_SCROLLBAR:
	    ins_horscroll();
	    നിർത്തുക;
#endif

	അവസ്ഥ K_HOME:	// <Home>
	അവസ്ഥ K_KHOME:
	അവസ്ഥ K_S_HOME:
	അവസ്ഥ K_C_HOME:
	    ins_home(c);
	    നിർത്തുക;

	അവസ്ഥ K_END:	// <End>
	അവസ്ഥ K_KEND:
	അവസ്ഥ K_S_END:
	അവസ്ഥ K_C_END:
	    ins_end(c);
	    നിർത്തുക;

	അവസ്ഥ K_ഇടത്:	// <Left>
	    എങ്കിൽ (mod_mask & (MOD_MASK_SHIFT|MOD_MASK_CTRL))
		ins_s_ഇടത്();
	    അല്ലെങ്കിൽ
		ins_ഇടത്();
	    നിർത്തുക;

	അവസ്ഥ K_S_ഇടത്:	// <S-Left>
	അവസ്ഥ K_C_ഇടത്:
	    ins_s_ഇടത്();
	    നിർത്തുക;

	അവസ്ഥ K_വലത്:	// <Right>
	    എങ്കിൽ (mod_mask & (MOD_MASK_SHIFT|MOD_MASK_CTRL))
		ins_s_വലത്();
	    അല്ലെങ്കിൽ
		ins_വലത്();
	    നിർത്തുക;

	അവസ്ഥ K_S_വലത്:	// <S-Right>
	അവസ്ഥ K_C_വലത്:
	    ins_s_വലത്();
	    നിർത്തുക;

	അവസ്ഥ K_UP:	// <Up>
	    എങ്കിൽ (pum_visible())
		പോകുക docomplete;
	    എങ്കിൽ (mod_mask & MOD_MASK_SHIFT)
		ins_pageup();
	    അല്ലെങ്കിൽ
		ins_up(FALSE);
	    നിർത്തുക;

	അവസ്ഥ K_S_UP:	// <S-Up>
	അവസ്ഥ K_PAGEUP:
	അവസ്ഥ K_KPAGEUP:
	    എങ്കിൽ (pum_visible())
	    {
#ifdef FEAT_PROP_POPUP
		// CTRL-SHIFT-<Up> scrolls the info popup up a line,
		// CTRL-SHIFT-<PageUp> a page.  Shift is folded into K_S_UP but
		// stays in mod_mask for PageUp, hence the asymmetric check.
		എങ്കിൽ (c == K_S_UP ? (mod_mask & MOD_MASK_CTRL)
			: ((mod_mask & MOD_MASK_CTRL)
			    && (mod_mask & MOD_MASK_SHIFT)))
		{
		    popup_scroll_info(-1, c != K_S_UP);
		    നിർത്തുക;
		}
#endif
		പോകുക docomplete;
	    }
	    ins_pageup();
	    നിർത്തുക;

	അവസ്ഥ K_DOWN:	// <Down>
	    എങ്കിൽ (pum_visible())
		പോകുക docomplete;
	    എങ്കിൽ (mod_mask & MOD_MASK_SHIFT)
		ins_pagedown();
	    അല്ലെങ്കിൽ
		ins_down(FALSE);
	    നിർത്തുക;

	അവസ്ഥ K_S_DOWN:	// <S-Down>
	അവസ്ഥ K_PAGEDOWN:
	അവസ്ഥ K_KPAGEDOWN:
	    എങ്കിൽ (pum_visible())
	    {
#ifdef FEAT_PROP_POPUP
		// CTRL-SHIFT-<Down>/<PageDown> scroll the info popup down.
		എങ്കിൽ (c == K_S_DOWN ? (mod_mask & MOD_MASK_CTRL)
			: ((mod_mask & MOD_MASK_CTRL)
			    && (mod_mask & MOD_MASK_SHIFT)))
		{
		    popup_scroll_info(1, c != K_S_DOWN);
		    നിർത്തുക;
		}
#endif
		പോകുക docomplete;
	    }
	    ins_pagedown();
	    നിർത്തുക;

#ifdef FEAT_DND
	അവസ്ഥ K_DROP:	// drag-n-drop event
	    ins_drop();
	    നിർത്തുക;
#endif

	അവസ്ഥ K_S_TAB:	// When not mapped, use like a normal TAB
	    c = TAB;
	    // FALLTHROUGH

	അവസ്ഥ TAB:	// TAB or Complete patterns along path
#എങ്കിൽ defined(FEAT_FIND_ID)
	    എങ്കിൽ (ctrl_x_mode_path_patterns())
		പോകുക docomplete;
#endif
	    inserted_space = FALSE;
	    എങ്കിൽ (ins_tab())
		പോകുക normalchar;	// insert TAB as a normal char
	    auto_format(FALSE, TRUE);
	    നിർത്തുക;

	അവസ്ഥ K_KENTER:	// <Enter>
	    c = CAR;
	    // FALLTHROUGH
	അവസ്ഥ CAR:
	അവസ്ഥ NL:
#എങ്കിൽ defined(FEAT_QUICKFIX)
	    // In a quickfix window a <CR> jumps to the error under the
	    // cursor.
	    എങ്കിൽ (bt_quickfix(curbuf) && c == CAR)
	    {
		എങ്കിൽ (curwin->w_llist_ref == ശൂന്യ)    // quickfix window
		    do_cmdline_cmd((char_u *)".cc");
		അല്ലെങ്കിൽ				    // location list window
		    do_cmdline_cmd((char_u *)".ll");
		നിർത്തുക;
	    }
#endif
	    എങ്കിൽ (cmdwin_type != 0)
	    {
		// Execute the command in the cmdline window.
		cmdwin_result = CAR;
		പോകുക doESCkey;
	    }
#ifdef FEAT_JOB_CHANNEL
	    എങ്കിൽ (bt_prompt(curbuf))
	    {
		invoke_prompt_callback();
		എങ്കിൽ (!bt_prompt(curbuf))
		    // buffer changed to a non-prompt buffer, get out of
		    // Insert mode
		    പോകുക doESCkey;
		നിർത്തുക;
	    }
#endif
	    എങ്കിൽ (ins_eol(c) == FAIL && !p_im)
		പോകുക doESCkey;	    // out of memory
	    auto_format(FALSE, FALSE);
	    inserted_space = FALSE;
	    നിർത്തുക;

	അവസ്ഥ Ctrl_K:	    // digraph or keyword completion
	    എങ്കിൽ (ctrl_x_mode_dictionary())
	    {
		എങ്കിൽ (has_compl_option(TRUE))
		    പോകുക docomplete;
		നിർത്തുക;
	    }
#ifdef FEAT_DIGRAPHS
	    c = ins_digraph();
	    എങ്കിൽ (c == NUL)
		നിർത്തുക;
#endif
	    പോകുക normalchar;

	അവസ്ഥ Ctrl_X:	// Enter CTRL-X mode
	    ins_ctrl_x();
	    നിർത്തുക;

	അവസ്ഥ Ctrl_RSB:	// Tag name completion after ^X
	    എങ്കിൽ (!ctrl_x_mode_tags())
		പോകുക normalchar;
	    പോകുക docomplete;

	അവസ്ഥ Ctrl_F:	// File name completion after ^X
	    എങ്കിൽ (!ctrl_x_mode_files())
		പോകുക normalchar;
	    പോകുക docomplete;

	അവസ്ഥ 's':	// Spelling completion after ^X
	അവസ്ഥ Ctrl_S:
	    എങ്കിൽ (!ctrl_x_mode_spell())
		പോകുക normalchar;
	    പോകുക docomplete;

	അവസ്ഥ Ctrl_L:	// Whole line completion after ^X
	    എങ്കിൽ (!ctrl_x_mode_whole_വരി())
	    {
		// CTRL-L with 'insertmode' set: Leave Insert mode
		എങ്കിൽ (p_im)
		{
		    എങ്കിൽ (echeck_abbr(Ctrl_L + ABBR_OFF))
			നിർത്തുക;
		    പോകുക doESCkey;
		}
		പോകുക normalchar;
	    }
	    // FALLTHROUGH

	അവസ്ഥ Ctrl_P:	// Do previous/next pattern completion
	അവസ്ഥ Ctrl_N:
#ifdef FEAT_PROP_POPUP
	    // CTRL-SHIFT-P/N scroll the info popup one line.
	    എങ്കിൽ (pum_visible() && (mod_mask & MOD_MASK_SHIFT)
		    && (c == Ctrl_P || c == Ctrl_N))
	    {
		popup_scroll_info(c == Ctrl_P ? -1 : 1, false);
		നിർത്തുക;
	    }
#endif
	    // if 'complete' is empty then plain ^P is no longer special,
	    // but it is under other ^X modes
	    എങ്കിൽ (*curbuf->b_p_cpt == NUL
		    && (ctrl_x_mode_normal() || ctrl_x_mode_whole_വരി())
		    && !compl_status_local())
		പോകുക normalchar;

docomplete:
	    ins_compl_clear_autocomplete_delay();
	    compl_busy = TRUE;
#ifdef FEAT_FOLDING
	    disable_fold_update++;  // don't redraw folds here
#endif
	    എങ്കിൽ (ins_complete(c, TRUE) == FAIL)
		compl_status_clear();
#ifdef FEAT_FOLDING
	    disable_fold_update--;
#endif
	    compl_busy = FALSE;
	    can_si = may_do_si(); // allow smartindenting
	    നിർത്തുക;

	അവസ്ഥ Ctrl_Y:	// copy from previous line or scroll down
	അവസ്ഥ Ctrl_E:	// copy from next line	   or scroll up
	    c = ins_ctrl_ey(c);
	    നിർത്തുക;

	സ്ഥിരസ്ഥിതി:
#ifdef UNIX
	    എങ്കിൽ (c == intr_char)		// special interrupt char
		പോകുക do_intr;
#endif

normalchar:
	    /*
	     * Insert a normal character.
	     */
#എങ്കിൽ defined(FEAT_EVAL)
	    എങ്കിൽ (!p_paste)
	    {
		// Trigger InsertCharPre.
		char_u *str = do_തിരുകുക_char_pre(c);
		char_u *p;

		എങ്കിൽ (str != ശൂന്യ)
		{
		    എങ്കിൽ (*str != NUL && stop_arrow() != FAIL)
		    {
			// Insert the new value of v:char literally.
			വേണ്ടി (p = str; *p != NUL; MB_PTR_ADV(p))
			{
			    c = PTR2CHAR(p);
			    എങ്കിൽ (c == CAR || c == K_KENTER || c == NL)
				ins_eol(c);
			    അല്ലെങ്കിൽ
				ins_char(c);
			}
			AppendToRedobuffLit(str, -1);
		    }
		    vim_free(str);
		    c = NUL;
		}

		// If the new value is already inserted or an empty string
		// then don't insert any character.
		എങ്കിൽ (c == NUL)
		    നിർത്തുക;
	    }
#endif
	    // Try to perform smart-indenting.
	    ins_try_si(c);

	    എങ്കിൽ (c == ' ')
	    {
		inserted_space = TRUE;
		എങ്കിൽ (inindent(0))
		    can_cindent = FALSE;
		എങ്കിൽ (Insstart_blank_vcol == MAXCOL
			&& curwin->w_cursor.lnum == Insstart.lnum)
		    Insstart_blank_vcol = get_nolist_virtcol();
	    }

	    // Insert a normal character and check for abbreviations on a
	    // special character.  Let CTRL-] expand abbreviations without
	    // inserting it.
	    എങ്കിൽ (vim_iswordc(c) || (!echeck_abbr(
			// Add ABBR_OFF for characters above 0x100, this is
			// what check_abbr() expects.
				(has_mbyte && c >= 0x100) ? (c + ABBR_OFF) : c)
			&& c != Ctrl_RSB))
	    {
		തിരുകുക_special(c, FALSE, FALSE);
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
	    എങ്കിൽ (ins_compl_has_autocomplete() && !char_avail()
		    && vim_isprintc(c))
		TRIGGER_AUTOCOMPLETE();

	    നിർത്തുക;
	}   // end of switch (c)

	// If typed something may trigger CursorHoldI again; K_COMPLETE_DELAY is
	// injected, not typed.
	എങ്കിൽ (c != K_CURSORHOLD && c != K_COMPLETE_DELAY
#ifdef FEAT_COMPL_FUNC
		// but not in CTRL-X mode, a script can't restore the state
		&& ctrl_x_mode_normal()
#endif
	       )
	    did_cursorhold = FALSE;

	// Check if we need to cancel completion mode because the window
	// or tab page was changed
	എങ്കിൽ (ins_compl_active() && !ins_compl_win_active(curwin))
	    ins_compl_cancel();

	// If the cursor was moved we didn't just insert a space
	എങ്കിൽ (arrow_used)
	    inserted_space = FALSE;

	എങ്കിൽ (can_cindent && cindent_on() && ctrl_x_mode_normal())
	{
force_cindent:
	    /*
	     * Indent now if a key was typed that is in 'cinkeys'.
	     */
	    എങ്കിൽ (in_cinkeys(c, ' ', വരി_is_white))
	    {
		എങ്കിൽ (stop_arrow() == OK)
		    // re-indent the current line
		    do_c_വ്യഞ്ജകം_indent();
	    }
	}

    }	// for (;;)
    // NOTREACHED
}

    പൂർണ്ണസംഖ്യ
ins_need_undo_get(ശൂന്യം)
{
    തിരികെ ins_need_undo;
}

/*
 * Redraw for Insert mode.
 * This is postponed until getting the next character to make '$' in the 'cpo'
 * option work correctly.
 * Only redraw when there are no characters available.  This speeds up
 * inserting sequences of characters (e.g., for CTRL-R).
 */
    ശൂന്യം
ins_redraw(പൂർണ്ണസംഖ്യ ready)	    // not busy with something
{
#ifdef FEAT_CONCEAL
    linenr_T	conceal_old_cursor_വരി = 0;
    linenr_T	conceal_new_cursor_വരി = 0;
    പൂർണ്ണസംഖ്യ		conceal_update_വരികൾ = FALSE;
#endif

    എങ്കിൽ (char_avail())
	തിരികെ;

    // Trigger CursorMoved if the cursor moved.  Not when the popup menu is
    // visible, the command might delete it.
    എങ്കിൽ (ready && (has_cursormovedI()
#ifdef FEAT_PROP_POPUP
		|| popup_visible
#endif
#എങ്കിൽ defined(FEAT_CONCEAL)
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
	എങ്കിൽ (syntax_present(curwin) && must_redraw)
	    update_screen(0);
#endif
	എങ്കിൽ (has_cursormovedI())
	{
	    // Make sure curswant is correct, an autocommand may call
	    // getcurpos().
	    update_curswant();
	    ins_apply_autocmds(EVENT_CURSORMOVEDI);
	}
#ifdef FEAT_PROP_POPUP
	എങ്കിൽ (popup_visible)
	    popup_check_cursor_pos();
#endif
#ifdef FEAT_CONCEAL
	എങ്കിൽ (curwin->w_p_cole > 0)
	{
	    conceal_old_cursor_വരി = last_cursormoved.lnum;
	    conceal_new_cursor_വരി = curwin->w_cursor.lnum;
	    conceal_update_വരികൾ = TRUE;
	}
#endif
	last_cursormoved = curwin->w_cursor;
    }

    // Trigger TextChangedI if b_changedtick_i differs.
    എങ്കിൽ (ready && has_textchangedI()
	    && curbuf->b_last_changedtick_i != CHANGEDTICK(curbuf)
	    && !pum_visible())
    {
	aco_save_T	aco;
	varnumber_T	tick = CHANGEDTICK(curbuf);

	// Save and restore curwin and curbuf, in case the autocmd changes
	// them.
	aucmd_prepbuf(&aco, curbuf);
	apply_autocmds(EVENT_TEXTCHANGEDI, ശൂന്യ, ശൂന്യ, FALSE, curbuf);
	aucmd_restbuf(&aco);
	curbuf->b_last_changedtick_i = CHANGEDTICK(curbuf);
	എങ്കിൽ (tick != CHANGEDTICK(curbuf))  // see ins_apply_autocmds()
	    u_save(curwin->w_cursor.lnum,
					(linenr_T)(curwin->w_cursor.lnum + 1));
    }

    // Trigger TextChangedP if b_changedtick_pum differs. When the popupmenu
    // closes TextChangedI will need to trigger for backwards compatibility,
    // thus use different b_last_changedtick* variables.
    എങ്കിൽ (ready && has_textchangedP()
	    && curbuf->b_last_changedtick_pum != CHANGEDTICK(curbuf)
	    && pum_visible())
    {
	aco_save_T	aco;
	varnumber_T	tick = CHANGEDTICK(curbuf);

	// Save and restore curwin and curbuf, in case the autocmd changes
	// them.
	aucmd_prepbuf(&aco, curbuf);
	apply_autocmds(EVENT_TEXTCHANGEDP, ശൂന്യ, ശൂന്യ, FALSE, curbuf);
	aucmd_restbuf(&aco);
	curbuf->b_last_changedtick_pum = CHANGEDTICK(curbuf);
	എങ്കിൽ (tick != CHANGEDTICK(curbuf))  // see ins_apply_autocmds()
	    u_save(curwin->w_cursor.lnum,
					(linenr_T)(curwin->w_cursor.lnum + 1));
    }

    എങ്കിൽ (ready)
	may_trigger_win_scrolled_resized();

    // Trigger SafeState if nothing is pending.
    may_trigger_safestate(ready
	    && !ins_compl_active()
	    && !pum_visible());

#എങ്കിൽ defined(FEAT_CONCEAL)
    എങ്കിൽ ((conceal_update_വരികൾ
	    && (conceal_old_cursor_വരി != conceal_new_cursor_വരി
		|| conceal_cursor_വരി(curwin)))
	    || need_cursor_വരി_redraw)
    {
	എങ്കിൽ (conceal_old_cursor_വരി != conceal_new_cursor_വരി)
	    redrawWinline(curwin, conceal_old_cursor_വരി);
	redrawWinline(curwin, conceal_new_cursor_വരി == 0
			    ? curwin->w_cursor.lnum : conceal_new_cursor_വരി);
	curwin->w_valid &= ~VALID_CROW;
	need_cursor_വരി_redraw = FALSE;
    }
#endif
    എങ്കിൽ (must_redraw)
	update_screen(0);
    അല്ലെങ്കിൽ എങ്കിൽ (clear_cmdline || redraw_cmdline)
	showmode();		// clear cmdline and show mode
    showruler(FALSE);
    setcursor();
    emsg_on_display = FALSE;	// may remove error message now
}

/*
 * Handle a CTRL-V or CTRL-Q typed in Insert mode.
 */
    സ്ഥിതം ശൂന്യം
ins_ctrl_v(ശൂന്യം)
{
    പൂർണ്ണസംഖ്യ		c;
    പൂർണ്ണസംഖ്യ		did_putchar = FALSE;

    // may need to redraw when no more chars available now
    ins_redraw(FALSE);

    എങ്കിൽ (redrawing() && !char_avail())
    {
	edit_putchar('^', TRUE);
	did_putchar = TRUE;
    }
    AppendToRedobuff((char_u *)CTRL_V_STR);	// CTRL-V

    add_to_showcmd_c(Ctrl_V);

    // Do not change any modifyOtherKeys ESC sequence to a normal key for
    // CTRL-SHIFT-V.
    c = get_literal(mod_mask & MOD_MASK_SHIFT);
    എങ്കിൽ (did_putchar)
	// when the line fits in 'columns' the '^' is at the start of the next
	// line and will not removed by the redraw
	edit_unputchar();
    clear_showcmd();

    തിരുകുക_special(c, FALSE, TRUE);
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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
decode_Modify_Other_താക്കോലുകൾ(പൂർണ്ണസംഖ്യ c)
{
    char_u  *p = typebuf.tb_buf + typebuf.tb_off;
    പൂർണ്ണസംഖ്യ	    idx;
    പൂർണ്ണസംഖ്യ	    form = 0;
    പൂർണ്ണസംഖ്യ	    argidx = 0;
    പൂർണ്ണസംഖ്യ	    arg[2] = {0, 0};

    // Recognize:
    // form 0: {lead}{key};{modifier}u
    // form 1: {lead}27;{modifier};{key}~
    എങ്കിൽ (typebuf.tb_len >= 4 && (c == CSI || (c == ESC && *p == '[')))
    {
	idx = (*p == '[');
	എങ്കിൽ (p[idx] == '2' && p[idx + 1] == '7' && p[idx + 2] == ';' &&
		kitty_protocol_state != KKPS_ENABLED)
	{
	    form = 1;
	    idx += 3;
	}
	ആകുമ്പോൾ (idx < typebuf.tb_len && argidx < 2)
	{
	    എങ്കിൽ (p[idx] == ';')
		++argidx;
	    അല്ലെങ്കിൽ എങ്കിൽ (VIM_ISDIGIT(p[idx]))
		arg[argidx] = arg[argidx] * 10 + (p[idx] - '0');
	    അല്ലെങ്കിൽ
		നിർത്തുക;
	    ++idx;
	}
	പൂർണ്ണസംഖ്യ kitty_no_mods = argidx == 0 && kitty_protocol_state == KKPS_ENABLED;
	എങ്കിൽ (idx < typebuf.tb_len
		&& p[idx] == (form == 1 ? '~' : 'u')
		&& (argidx == 1 || kitty_no_mods))
	{
	    // Match, consume the code.
	    typebuf.tb_off += idx + 1;
	    typebuf.tb_len -= idx + 1;
#എങ്കിൽ defined(FEAT_CLIENTSERVER) || defined(FEAT_EVAL)
	    എങ്കിൽ (typebuf.tb_len == 0)
		typebuf_was_filled = FALSE;
#endif

	    mod_mask = kitty_no_mods ? 0 : decode_modifiers(arg[!form]);
	    c = merge_modify_Other_താക്കോലുകൾ(arg[form], &mod_mask);
	}
    }

    തിരികെ c;
}

/*
 * Put a character directly onto the screen.  It's not stored in a buffer.
 * Used while handling CTRL-K, CTRL-V, etc. in Insert mode.
 */
സ്ഥിതം പൂർണ്ണസംഖ്യ  pc_status;
#define PC_STATUS_UNSET	0	// pc_bytes was not set
#define PC_STATUS_വലത്	1	// right half of double-wide char
#define PC_STATUS_ഇടത്	2	// left half of double-wide char
#define PC_STATUS_SET	3	// pc_bytes was filled
സ്ഥിതം char_u pc_bytes[MB_MAXBYTES + 1]; // saved bytes
സ്ഥിതം പൂർണ്ണസംഖ്യ  pc_attr;
സ്ഥിതം പൂർണ്ണസംഖ്യ  pc_row;
സ്ഥിതം പൂർണ്ണസംഖ്യ  pc_col;

    ശൂന്യം
edit_putchar(പൂർണ്ണസംഖ്യ c, പൂർണ്ണസംഖ്യ highlight)
{
    പൂർണ്ണസംഖ്യ	    attr;

    എങ്കിൽ (Screen_വരികൾ == ശൂന്യ)
	തിരികെ;

    update_topline();	// just in case w_topline isn't valid
    validate_cursor();
    എങ്കിൽ (highlight)
	attr = HL_ATTR(HLF_8);
    അല്ലെങ്കിൽ
	attr = 0;
    pc_row = W_WINROW(curwin) + curwin->w_wrow;
    pc_col = curwin->w_wincol;
    pc_status = PC_STATUS_UNSET;
#ifdef FEAT_RIGHTLEFT
    എങ്കിൽ (curwin->w_p_rl)
    {
	pc_col += curwin->w_width - 1 - curwin->w_wcol;
	എങ്കിൽ (has_mbyte)
	{
	    പൂർണ്ണസംഖ്യ fix_col = mb_fix_col(pc_col, pc_row);

	    എങ്കിൽ (fix_col != pc_col)
	    {
		screen_putchar(' ', pc_row, fix_col, attr);
		--curwin->w_wcol;
		pc_status = PC_STATUS_വലത്;
	    }
	}
    }
    അല്ലെങ്കിൽ
#endif
    {
	pc_col += curwin->w_wcol;
	എങ്കിൽ (mb_lefthalve(pc_row, pc_col))
	    pc_status = PC_STATUS_ഇടത്;
    }

    // save the character to be able to put it back
    എങ്കിൽ (pc_status == PC_STATUS_UNSET)
    {
	screen_getbytes(pc_row, pc_col, pc_bytes, &pc_attr);
	pc_status = PC_STATUS_SET;
    }
    screen_putchar(c, pc_row, pc_col, attr);
}

#എങ്കിൽ defined(FEAT_JOB_CHANNEL)
/*
 * Set the insert start position for when using a prompt buffer.
 */
    ശൂന്യം
set_insstart(linenr_T lnum, പൂർണ്ണസംഖ്യ col)
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
    ശൂന്യം
edit_unputchar(ശൂന്യം)
{
    എങ്കിൽ (pc_status != PC_STATUS_UNSET && pc_row >= സന്ദേശം_scrolled)
    {
	എങ്കിൽ (pc_status == PC_STATUS_വലത്)
	    ++curwin->w_wcol;
	എങ്കിൽ (pc_status == PC_STATUS_വലത് || pc_status == PC_STATUS_ഇടത്)
	    redrawWinline(curwin, curwin->w_cursor.lnum);
	അല്ലെങ്കിൽ
	    screen_puts(pc_bytes, pc_row - സന്ദേശം_scrolled, pc_col, pc_attr);
    }
}

/*
 * Called when "$" is in 'cpoptions': display a '$' at the end of the changed
 * text.  Only works when cursor is in the line that changes.
 */
    ശൂന്യം
display_dollar(colnr_T col_arg)
{
    colnr_T col = col_arg < 0 ? 0 : col_arg;
    colnr_T save_col;

    എങ്കിൽ (!redrawing())
	തിരികെ;

    cursor_off();
    save_col = curwin->w_cursor.col;
    curwin->w_cursor.col = col;
    എങ്കിൽ (has_mbyte)
    {
	char_u *p;

	// If on the last byte of a multi-byte move to the first byte.
	p = ml_get_curline();
	curwin->w_cursor.col -= (*mb_head_off)(p, p + col);
    }
    curs_columns(FALSE);	    // recompute w_wrow and w_wcol
    എങ്കിൽ (curwin->w_wcol < curwin->w_width)
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
    ശൂന്യം
undisplay_dollar(ശൂന്യം)
{
    എങ്കിൽ (dollar_vcol < 0)
	തിരികെ;

    dollar_vcol = -1;
    redrawWinline(curwin, curwin->w_cursor.lnum);
}

/*
 * Truncate the space at the end of a line.  This is to be used only in an
 * insert mode.  It handles fixing the replace stack for MODE_REPLACE and
 * MODE_VREPLACE modes.
 */
    ശൂന്യം
truncate_spaces(char_u *വരി, size_t len)
{
    പൂർണ്ണസംഖ്യ	    i;

    // find start of trailing white space
    വേണ്ടി (i = (പൂർണ്ണസംഖ്യ)len - 1; i >= 0 && VIM_ISWHITE(വരി[i]); i--)
    {
	എങ്കിൽ (State & REPLACE_FLAG)
	    replace_join(0);	    // remove a NUL from the replace stack
    }
    വരി[i + 1] = NUL;
}

/*
 * Backspace the cursor until the given column.  Handles MODE_REPLACE and
 * MODE_VREPLACE modes correctly.  May also be used when not in insert mode at
 * all.  Will attempt not to go before "col" even when there is a composing
 * character.
 */
    ശൂന്യം
backspace_until_column(പൂർണ്ണസംഖ്യ col)
{
    ആകുമ്പോൾ ((പൂർണ്ണസംഖ്യ)curwin->w_cursor.col > col)
    {
	curwin->w_cursor.col--;
	എങ്കിൽ (State & REPLACE_FLAG)
	    replace_do_bs(col);
	അല്ലെങ്കിൽ എങ്കിൽ (!del_char_after_col(col))
	    നിർത്തുക;
    }
}

/*
 * Like del_char(), but make sure not to go before column "limit_col".
 * Only matters when there are composing characters.
 * Return TRUE when something was deleted.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
del_char_after_col(പൂർണ്ണസംഖ്യ limit_col UNUSED)
{
    എങ്കിൽ (enc_utf8 && limit_col >= 0)
    {
	colnr_T ecol = curwin->w_cursor.col + 1;

	// Make sure the cursor is at the start of a character, but
	// skip forward again when going too far back because of a
	// composing character.
	mb_adjust_cursor();
	ആകുമ്പോൾ (curwin->w_cursor.col < (colnr_T)limit_col)
	{
	    പൂർണ്ണസംഖ്യ l = utf_ptr2len(ml_get_cursor());

	    എങ്കിൽ (l == 0)  // end of line
		നിർത്തുക;
	    curwin->w_cursor.col += l;
	}
	എങ്കിൽ (*ml_get_cursor() == NUL || curwin->w_cursor.col == ecol)
	    തിരികെ FALSE;
	del_bytes((long)((പൂർണ്ണസംഖ്യ)ecol - curwin->w_cursor.col), FALSE, TRUE);
    }
    അല്ലെങ്കിൽ
	(ശൂന്യം)del_char(FALSE);
    തിരികെ TRUE;
}

/*
 * Next character is interpreted literally.
 * A one, two or three digit decimal number is interpreted as its byte value.
 * If one or two digits are entered, the next character is given to vungetc().
 * For Unicode a character > 255 may be returned.
 * If "noReduceKeys" is TRUE do not change any modifyOtherKeys ESC sequence
 * into a normal key, return ESC.
 */
    പൂർണ്ണസംഖ്യ
get_literal(പൂർണ്ണസംഖ്യ no_Reduce_താക്കോലുകൾ)
{
    പൂർണ്ണസംഖ്യ		cc;
    പൂർണ്ണസംഖ്യ		nc;
    പൂർണ്ണസംഖ്യ		i;
    പൂർണ്ണസംഖ്യ		hex = FALSE;
    പൂർണ്ണസംഖ്യ		octal = FALSE;
    പൂർണ്ണസംഖ്യ		unicode = 0;

    എങ്കിൽ (got_int)
	തിരികെ Ctrl_C;

#ifdef FEAT_GUI
    /*
     * In GUI there is no point inserting the internal code for a special key.
     * It is more useful to insert the string "<KEY>" instead.	This would
     * probably be useful in a text window too, but it would not be
     * vi-compatible (maybe there should be an option for it?) -- webb
     */
    എങ്കിൽ (gui.in_use)
    {
	++allow_താക്കോലുകൾ;
	എങ്കിൽ (no_Reduce_താക്കോലുകൾ)
	    ++no_reduce_താക്കോലുകൾ;
    }
#endif
#ifdef USE_ON_FLY_SCROLL
    dont_scroll = TRUE;		// disallow scrolling here
#endif
    ++no_mapping;		// don't map the next key hits
    cc = 0;
    i = 0;
    വേണ്ടി (;;)
    {
	nc = plain_vgetc();
	എങ്കിൽ ((nc == ESC || nc == CSI) && !no_Reduce_താക്കോലുകൾ)
	    nc = decode_Modify_Other_താക്കോലുകൾ(nc);

	എങ്കിൽ ((mod_mask & ~MOD_MASK_SHIFT) != 0)
	    // A character with non-Shift modifiers should not be a valid
	    // character for i_CTRL-V_digit.
	    നിർത്തുക;

	എങ്കിൽ ((State & MODE_CMDLINE) == 0 && MB_BYTE2LEN_CHECK(nc) == 1)
	    add_to_showcmd(nc);
	എങ്കിൽ (nc == 'x' || nc == 'X')
	    hex = TRUE;
	അല്ലെങ്കിൽ എങ്കിൽ (nc == 'o' || nc == 'O')
	    octal = TRUE;
	അല്ലെങ്കിൽ എങ്കിൽ (nc == 'u' || nc == 'U')
	    unicode = nc;
	അല്ലെങ്കിൽ
	{
	    എങ്കിൽ (hex || unicode != 0)
	    {
		എങ്കിൽ (!vim_isxdigit(nc))
		    നിർത്തുക;
		cc = cc * 16 + hex2nr(nc);
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (octal)
	    {
		എങ്കിൽ (nc < '0' || nc > '7')
		    നിർത്തുക;
		cc = cc * 8 + nc - '0';
	    }
	    അല്ലെങ്കിൽ
	    {
		എങ്കിൽ (!VIM_ISDIGIT(nc))
		    നിർത്തുക;
		cc = cc * 10 + nc - '0';
	    }

	    ++i;
	}

	എങ്കിൽ (cc > 255 && unicode == 0)
	    cc = 255;		// limit range to 0-255
	nc = 0;

	എങ്കിൽ (hex)		// hex: up to two chars
	{
	    എങ്കിൽ (i >= 2)
		നിർത്തുക;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (unicode)	// Unicode: up to four or eight chars
	{
	    എങ്കിൽ ((unicode == 'u' && i >= 4) || (unicode == 'U' && i >= 8))
		നിർത്തുക;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (i >= 3)	// decimal or octal: up to three chars
	    നിർത്തുക;
    }
    എങ്കിൽ (i == 0)	    // no number entered
    {
	എങ്കിൽ (nc == K_ZERO)   // NUL is stored as NL
	{
	    cc = '\n';
	    nc = 0;
	}
	അല്ലെങ്കിൽ
	{
	    cc = nc;
	    nc = 0;
	}
    }

    എങ്കിൽ (cc == 0)	// NUL is stored as NL
	cc = '\n';
    എങ്കിൽ (enc_dbcs && (cc & 0xff) == 0)
	cc = '?';	// don't accept an illegal DBCS char, the NUL in the
			// second byte will cause trouble!

    --no_mapping;
#ifdef FEAT_GUI
    എങ്കിൽ (gui.in_use)
    {
	--allow_താക്കോലുകൾ;
	എങ്കിൽ (no_Reduce_താക്കോലുകൾ)
	    --no_reduce_താക്കോലുകൾ;
    }
#endif
    എങ്കിൽ (nc)
    {
	vungetc(nc);
	// A character typed with i_CTRL-V_digit cannot have modifiers.
	mod_mask = 0;
    }
    got_int = FALSE;	    // CTRL-C typed after CTRL-V is not an interrupt
    തിരികെ cc;
}

/*
 * Insert character, taking care of special keys and mod_mask
 */
    സ്ഥിതം ശൂന്യം
തിരുകുക_special(
    പൂർണ്ണസംഖ്യ	    c,
    പൂർണ്ണസംഖ്യ	    allow_modmask,
    പൂർണ്ണസംഖ്യ	    ctrlv)	    // c was typed after CTRL-V
{
    char_u  *p;
    പൂർണ്ണസംഖ്യ	    len;

    /*
     * Special function key, translate into "<Key>". Up to the last '>' is
     * inserted with ins_str(), so as not to replace characters in replace
     * mode.
     * Only use mod_mask for special keys, to avoid things like <S-Space>,
     * unless 'allow_modmask' is TRUE.
     */
#എങ്കിൽ defined(MACOS_X) || defined(FEAT_GUI_GTK)
    // Command-key never produces a normal key
    എങ്കിൽ (mod_mask & MOD_MASK_CMD)
	allow_modmask = TRUE;
#endif
    എങ്കിൽ (IS_SPECIAL(c) || (mod_mask && allow_modmask))
    {
	p = get_special_താക്കോൽ_name(c, mod_mask);
	len = (പൂർണ്ണസംഖ്യ)STRLEN(p);
	c = p[len - 1];
	എങ്കിൽ (len > 2)
	{
	    എങ്കിൽ (stop_arrow() == FAIL)
		തിരികെ;
	    p[len - 1] = NUL;
	    ins_str(p, len - 1);
	    AppendToRedobuffLit(p, -1);
	    ctrlv = FALSE;
	}
    }
    എങ്കിൽ (stop_arrow() == OK)
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
    ശൂന്യം
insertchar(
    പൂർണ്ണസംഖ്യ		c,			// character to insert or NUL
    പൂർണ്ണസംഖ്യ		flags,			// INSCHAR_FORMAT, etc.
    പൂർണ്ണസംഖ്യ		second_indent)		// indent for second line if >= 0
{
    പൂർണ്ണസംഖ്യ		textwidth;
    char_u	*p;
    പൂർണ്ണസംഖ്യ		fo_ins_blank;
    പൂർണ്ണസംഖ്യ		force_format = flags & INSCHAR_FORMAT;

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
    എങ്കിൽ (textwidth > 0
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
#എങ്കിൽ defined(FEAT_EVAL)
	പൂർണ്ണസംഖ്യ     do_internal = TRUE;
	colnr_T virtcol = get_nolist_virtcol()
				  + char2cells(c != NUL ? c : gchar_cursor());

	എങ്കിൽ (*curbuf->b_p_fex != NUL && (flags & INSCHAR_NO_FEX) == 0
		&& (force_format || virtcol > (colnr_T)textwidth))
	{
	    do_internal = (fex_format(curwin->w_cursor.lnum, 1L, c) != 0);
	    // It may be required to save for undo again, e.g. when setline()
	    // was called.
	    ins_need_undo = TRUE;
	}
	എങ്കിൽ (do_internal)
#endif
	    internal_format(textwidth, second_indent, flags, c == NUL, c);
    }

    എങ്കിൽ (c == NUL)	    // only formatting was wanted
	തിരികെ;

    // Check whether this character should end a comment.
    എങ്കിൽ (did_ai && c == end_comment_pending)
    {
	char_u  *വരി;
	char_u	lead_end[COM_MAX_LEN];	    // end-comment string
	പൂർണ്ണസംഖ്യ	നടുവ്_len, end_len;
	പൂർണ്ണസംഖ്യ	i;

	/*
	 * Need to remove existing (middle) comment leader and insert end
	 * comment leader.  First, check what comment leader we can find.
	 */
	i = get_leader_len(വരി = ml_get_curline(), &p, FALSE, TRUE);
	എങ്കിൽ (i > 0 && vim_strchr(p, COM_നടുവ്) != ശൂന്യ)	// Just checking
	{
	    // Skip middle-comment string
	    ആകുമ്പോൾ (*p && p[-1] != ':')	// find end of middle flags
		++p;
	    നടുവ്_len = copy_option_part(&p, lead_end, COM_MAX_LEN, ",");
	    // Don't count trailing white space for middle_len
	    ആകുമ്പോൾ (നടുവ്_len > 0 && VIM_ISWHITE(lead_end[നടുവ്_len - 1]))
		--നടുവ്_len;

	    // Find the end-comment string
	    ആകുമ്പോൾ (*p && p[-1] != ':')	// find end of end flags
		++p;
	    end_len = copy_option_part(&p, lead_end, COM_MAX_LEN, ",");

	    // Skip white space before the cursor
	    i = curwin->w_cursor.col;
	    ആകുമ്പോൾ (--i >= 0 && VIM_ISWHITE(വരി[i]))
		;
	    i++;

	    // Skip to before the middle leader
	    i -= നടുവ്_len;

	    // Check some expected things before we go on
	    എങ്കിൽ (i >= 0 && end_len > 0 && lead_end[end_len - 1] == end_comment_pending)
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

    എങ്കിൽ (       !ISSPECIAL(c)
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
#define നിവേശം_BUFLEN 100
	char_u		buf[നിവേശം_BUFLEN + 1];
	പൂർണ്ണസംഖ്യ		i;
	colnr_T		virtcol = 0;

	buf[0] = c;
	i = 1;
	എങ്കിൽ (textwidth > 0)
	    virtcol = get_nolist_virtcol();
	/*
	 * Stop the string when:
	 * - no more chars available
	 * - finding a special character (command key)
	 * - buffer is full
	 * - running into the 'textwidth' boundary
	 * - need to check for abbreviation: A non-word char after a word-char
	 */
	ആകുമ്പോൾ (	   (c = vpeekc()) != NUL
		&& !ISSPECIAL(c)
		&& (!has_mbyte || MB_BYTE2LEN_CHECK(c) == 1)
		&& i < നിവേശം_BUFLEN
		&& (textwidth == 0
		    || (virtcol += byte2cells(buf[i - 1])) < (colnr_T)textwidth)
		&& !(!no_abbr && !vim_iswordc(c) && vim_iswordc(buf[i - 1])))
	{
#ifdef FEAT_RIGHTLEFT
	    c = vgetc();
	    എങ്കിൽ (p_hkmap && താക്കോൽ_Typed)
		c = hkmap(c);		    // Hebrew mode mapping
	    buf[i++] = c;
#അല്ലെങ്കിൽ
	    buf[i++] = vgetc();
#endif
	}

#ifdef FEAT_DIGRAPHS
	do_digraph(-1);			// clear digraphs
	do_digraph(buf[i-1]);		// may be the start of a digraph
#endif
	buf[i] = NUL;
	ins_str(buf, i);
	എങ്കിൽ (flags & INSCHAR_CTRLV)
	{
	    redo_literal(*buf);
	    i = 1;
	}
	അല്ലെങ്കിൽ
	    i = 0;
	എങ്കിൽ (buf[i] != NUL)
	    AppendToRedobuffLit(buf + i, -1);
    }
    അല്ലെങ്കിൽ
    {
	പൂർണ്ണസംഖ്യ		cc;

	എങ്കിൽ (has_mbyte && (cc = (*mb_char2len)(c)) > 1)
	{
	    char_u	buf[MB_MAXBYTES + 1];

	    (*mb_char2bytes)(c, buf);
	    buf[cc] = NUL;
	    ins_char_bytes(buf, cc);
	    AppendCharToRedobuff(c);
	}
	അല്ലെങ്കിൽ
	{
	    ins_char(c);
	    എങ്കിൽ (flags & INSCHAR_CTRLV)
		redo_literal(c);
	    അല്ലെങ്കിൽ
		AppendCharToRedobuff(c);
	}
    }
}

/*
 * Put a character in the redo buffer, for when just after a CTRL-V.
 */
    സ്ഥിതം ശൂന്യം
redo_literal(പൂർണ്ണസംഖ്യ c)
{
    char_u	buf[10];

    // Only digits need special treatment.  Translate them into a string of
    // three digits.
    എങ്കിൽ (VIM_ISDIGIT(c))
    {
	vim_snprintf((അക്ഷരം *)buf, വലുപ്പം(buf), "%03d", c);
	AppendToRedobuff(buf);
    }
    അല്ലെങ്കിൽ
	AppendCharToRedobuff(c);
}

/*
 * start_arrow() is called when an arrow key is used in insert mode.
 * For undo/redo it resembles hitting the <ESC> key.
 */
    ശൂന്യം
start_arrow(
    pos_T    *end_തിരുകുക_pos)		// can be NULL
{
    start_arrow_common(end_തിരുകുക_pos, TRUE);
}

/*
 * Like start_arrow() but with end_change argument.
 * Will prepare for redo of CTRL-G U if "end_change" is FALSE.
 */
    സ്ഥിതം ശൂന്യം
start_arrow_with_change(
    pos_T    *end_തിരുകുക_pos,		// can be NULL
    പൂർണ്ണസംഖ്യ	      end_change)		// end undoable change
{
    start_arrow_common(end_തിരുകുക_pos, end_change);
    എങ്കിൽ (!end_change)
    {
	AppendCharToRedobuff(Ctrl_G);
	AppendCharToRedobuff('U');
    }
}

    സ്ഥിതം ശൂന്യം
start_arrow_common(
    pos_T    *end_തിരുകുക_pos,		// can be NULL
    പൂർണ്ണസംഖ്യ	      end_change)		// end undoable change
{
    എങ്കിൽ (!arrow_used && end_change)	// something has been inserted
    {
	AppendToRedobuff(ESC_STR);
	stop_തിരുകുക(end_തിരുകുക_pos, FALSE, FALSE);
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
    സ്ഥിതം ശൂന്യം
check_spell_redraw(ശൂന്യം)
{
    എങ്കിൽ (spell_redraw_lnum != 0)
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
    പൂർണ്ണസംഖ്യ
stop_arrow(ശൂന്യം)
{
    എങ്കിൽ (arrow_used)
    {
	Insstart = curwin->w_cursor;	// new insertion starts here
	എങ്കിൽ (Insstart.col > Insstart_orig.col && !ins_need_undo)
	    // Don't update the original insert position when moved to the
	    // right, except when nothing was inserted yet.
	    update_Insstart_orig = FALSE;
	Insstart_textlen = (colnr_T)linetabsize_str(ml_get_curline());

	എങ്കിൽ (u_save_cursor() == OK)
	{
	    arrow_used = FALSE;
	    ins_need_undo = FALSE;
	}

	ai_col = 0;
	എങ്കിൽ (State & VREPLACE_FLAG)
	{
	    orig_വരി_എണ്ണം = curbuf->b_ml.ml_വരി_എണ്ണം;
	    vr_വരികൾ_changed = 1;
	}
	ResetRedobuff();
	AppendToRedobuff((char_u *)"1i");   // pretend we start an insertion
	new_തിരുകുക_skip = 2;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (ins_need_undo)
    {
	എങ്കിൽ (u_save_cursor() == OK)
	{
	    // A command or event may have moved the cursor before the next
	    // edit. Pull Insstart back only when the cursor moved above it,
	    // so that later edits can properly decide whether an extra undo
	    // entry is needed. Advancing Insstart would mis-place '[ after a
	    // register paste.
	    എങ്കിൽ (LT_POS(curwin->w_cursor, Insstart))
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

    തിരികെ (arrow_used || ins_need_undo ? FAIL : OK);
}

/*
 * Do a few things to stop inserting.
 * "end_insert_pos" is where insert ended.  It is NULL when we already jumped
 * to another window/buffer.
 */
    സ്ഥിതം ശൂന്യം
stop_തിരുകുക(
    pos_T	*end_തിരുകുക_pos,
    പൂർണ്ണസംഖ്യ		esc,			// called by ins_esc()
    പൂർണ്ണസംഖ്യ		nomove)			// <c-\><c-o>, don't move cursor
{
    പൂർണ്ണസംഖ്യ		cc;
    string_T	inserted;

    stop_redo_ins();
    replace_flush();		// abandon replace stack

    /*
     * Save the inserted text for later redo with ^@ and CTRL-A.
     * Don't do it when "restart_edit" was set and nothing was inserted,
     * otherwise CTRL-O w and then <Left> will clear "last_insert".
     */
    inserted = get_inserted();
    പൂർണ്ണസംഖ്യ added = inserted.string == ശൂന്യ ? 0 : (പൂർണ്ണസംഖ്യ)inserted.length - new_തിരുകുക_skip;
    എങ്കിൽ (did_restart_edit == 0 || added > 0)
    {
	vim_free(last_തിരുകുക.string);
	last_തിരുകുക = inserted;			    // structure copy
	last_തിരുകുക_skip = added < 0 ? 0 : new_തിരുകുക_skip;
    }
    അല്ലെങ്കിൽ
	vim_free(inserted.string);

    എങ്കിൽ (!arrow_used && end_തിരുകുക_pos != ശൂന്യ)
    {
	// Auto-format now.  It may seem strange to do this when stopping an
	// insertion (or moving the cursor), but it's required when appending
	// a line and having it end in a space.  But only do it when something
	// was actually inserted, otherwise undo won't work.
	എങ്കിൽ (!ins_need_undo && has_format_option(FO_AUTO))
	{
	    pos_T   tpos = curwin->w_cursor;

	    // When the cursor is at the end of the line after a space the
	    // formatting will move it to the following word.  Avoid that by
	    // moving the cursor onto the space.
	    cc = 'x';
	    എങ്കിൽ (curwin->w_cursor.col > 0 && gchar_cursor() == NUL)
	    {
		dec_cursor();
		cc = gchar_cursor();
		എങ്കിൽ (!VIM_ISWHITE(cc))
		    curwin->w_cursor = tpos;
	    }

	    auto_format(TRUE, FALSE);

	    എങ്കിൽ (VIM_ISWHITE(cc))
	    {
		എങ്കിൽ (gchar_cursor() != NUL)
		    inc_cursor();
		// If the cursor is still at the same character, also keep
		// the "coladd".
		എങ്കിൽ (gchar_cursor() == NUL
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
	എങ്കിൽ (!nomove && did_ai && (esc || (vim_strchr(p_cpo, CPO_INDENT) == ശൂന്യ
			&& curwin->w_cursor.lnum != end_തിരുകുക_pos->lnum))
		&& end_തിരുകുക_pos->lnum <= curbuf->b_ml.ml_വരി_എണ്ണം)
	{
	    pos_T	tpos = curwin->w_cursor;
	    colnr_T	prev_col = end_തിരുകുക_pos->col;
	    colnr_T	strip_col;

	    curwin->w_cursor = *end_തിരുകുക_pos;
	    check_cursor_col();  // make sure it is not past the line

	    // Where the loop would actually start (back up if on NUL).
	    strip_col = curwin->w_cursor.col;
	    എങ്കിൽ (gchar_cursor() == NUL && strip_col > 0)
		--strip_col;

	    // Don't strip if non-whitespace follows: setline() from a <Cmd>
	    // mapping or CursorHoldI autocmd may have inserted content.
	    എങ്കിൽ (*skipwhite(ml_get_curline() + strip_col) == NUL)
	    {
		curwin->w_cursor.col = strip_col;
		വേണ്ടി (;;)
		{
		    എങ്കിൽ (gchar_cursor() == NUL && curwin->w_cursor.col > 0)
			--curwin->w_cursor.col;
		    cc = gchar_cursor();
		    എങ്കിൽ (!VIM_ISWHITE(cc))
			നിർത്തുക;
		    എങ്കിൽ (del_char(TRUE) == FAIL)
			നിർത്തുക;  // should not happen
		}
		എങ്കിൽ (curwin->w_cursor.lnum != tpos.lnum)
		    curwin->w_cursor = tpos;
		അല്ലെങ്കിൽ എങ്കിൽ (curwin->w_cursor.col < prev_col)
		{
		    // reset tpos, could have been invalidated in the loop above
		    tpos = curwin->w_cursor;
		    tpos.col++;
		    എങ്കിൽ (cc != NUL && gchar_pos(&tpos) == NUL)
			++curwin->w_cursor.col;	// put cursor back on the NUL
		}

		// <C-S-Right> may have started Visual mode, adjust the position for
		// deleted characters.
		എങ്കിൽ (VIsual_active)
		    check_visual_pos();
	    }
	    അല്ലെങ്കിൽ
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
    എങ്കിൽ (end_തിരുകുക_pos != ശൂന്യ)
    {
	curbuf->b_op_start = Insstart;
	curbuf->b_op_start_orig = Insstart_orig;
	curbuf->b_op_end = *end_തിരുകുക_pos;
    }
}

/*
 * Set the last inserted text to a single character.
 * Used for the replace command.
 */
    ശൂന്യം
set_last_തിരുകുക(പൂർണ്ണസംഖ്യ c)
{
    char_u	*s;

    vim_free(last_തിരുകുക.string);
    last_തിരുകുക.string = alloc(MB_MAXBYTES * 3 + 5);
    എങ്കിൽ (last_തിരുകുക.string == ശൂന്യ)
    {
	last_തിരുകുക.length = 0;
	തിരികെ;
    }

    s = last_തിരുകുക.string;
    // Use the CTRL-V only when entering a special char
    എങ്കിൽ (c < ' ' || c == DEL)
	*s++ = Ctrl_V;
    s = add_char2buf(c, s);
    *s++ = ESC;
    *s = NUL;
    last_തിരുകുക.length = (size_t)(s - last_തിരുകുക.string);
    last_തിരുകുക_skip = 0;
}

#എങ്കിൽ defined(EXITFREE)
    ശൂന്യം
free_last_തിരുകുക(ശൂന്യം)
{
    VIM_CLEAR_STRING(last_തിരുകുക);
}
#endif

/*
 * Add character "c" to buffer "s".  Escape the special meaning of K_SPECIAL
 * and CSI.  Handle multi-byte characters.
 * Returns a pointer to after the added bytes.
 */
    char_u *
add_char2buf(പൂർണ്ണസംഖ്യ c, char_u *s)
{
    char_u	temp[MB_MAXBYTES + 1];
    പൂർണ്ണസംഖ്യ		i;
    പൂർണ്ണസംഖ്യ		len;

    len = (*mb_char2bytes)(c, temp);
    വേണ്ടി (i = 0; i < len; ++i)
    {
	c = temp[i];
	// Need to escape K_SPECIAL and CSI like in the typeahead buffer.
	എങ്കിൽ (c == K_SPECIAL)
	{
	    *s++ = K_SPECIAL;
	    *s++ = KS_SPECIAL;
	    *s++ = KE_FILLER;
	}
#ifdef FEAT_GUI
	അല്ലെങ്കിൽ എങ്കിൽ (c == CSI)
	{
	    *s++ = CSI;
	    *s++ = KS_EXTRA;
	    *s++ = (പൂർണ്ണസംഖ്യ)KE_CSI;
	}
#endif
	അല്ലെങ്കിൽ
	    *s++ = c;
    }
    തിരികെ s;
}

/*
 * move cursor to start of line
 * if flags & BL_WHITE	move to first non-white
 * if flags & BL_SOL	move to first non-white if startofline is set,
 *			    otherwise keep "curswant" column
 * if flags & BL_FIX	don't leave the cursor on a NUL.
 */
    ശൂന്യം
beginline(പൂർണ്ണസംഖ്യ flags)
{
    എങ്കിൽ ((flags & BL_SOL) && !p_sol)
	coladvance(curwin->w_curswant);
    അല്ലെങ്കിൽ
    {
	curwin->w_cursor.col = 0;
	curwin->w_cursor.coladd = 0;

	എങ്കിൽ (flags & (BL_WHITE | BL_SOL))
	{
	    char_u  *ptr;

	    വേണ്ടി (ptr = ml_get_curline(); VIM_ISWHITE(*ptr)
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

    പൂർണ്ണസംഖ്യ
oneright(ശൂന്യം)
{
    char_u	*ptr;
    പൂർണ്ണസംഖ്യ		l;

    എങ്കിൽ (virtual_active())
    {
	pos_T	prevpos = curwin->w_cursor;

	// Adjust for multi-wide char (excluding TAB)
	ptr = ml_get_cursor();
	coladvance(getviscol() + ((*ptr != TAB
					  && vim_isprintc((*mb_ptr2char)(ptr)))
		    ? ptr2cells(ptr) : 1));
	curwin->w_set_curswant = true;
	// Return OK if the cursor moved, FAIL otherwise (at window edge).
	തിരികെ (prevpos.col != curwin->w_cursor.col
		    || prevpos.coladd != curwin->w_cursor.coladd) ? OK : FAIL;
    }

    ptr = ml_get_cursor();
    എങ്കിൽ (*ptr == NUL)
	തിരികെ FAIL;	    // already at the very end

    എങ്കിൽ (has_mbyte)
	l = (*mb_ptr2len)(ptr);
    അല്ലെങ്കിൽ
	l = 1;

    // move "l" bytes right, but don't end up on the NUL, unless 'virtualedit'
    // contains "onemore".
    എങ്കിൽ (ptr[l] == NUL && (get_ve_flags() & VE_ONEMORE) == 0)
	തിരികെ FAIL;
    curwin->w_cursor.col += l;

    curwin->w_set_curswant = true;
    adjust_skipcol();
    തിരികെ OK;
}

    പൂർണ്ണസംഖ്യ
oneleft(ശൂന്യം)
{
    എങ്കിൽ (virtual_active())
    {
#ifdef FEAT_LINEBREAK
	പൂർണ്ണസംഖ്യ width;
#endif
	പൂർണ്ണസംഖ്യ v = getviscol();

	എങ്കിൽ (v == 0)
	    തിരികെ FAIL;

#ifdef FEAT_LINEBREAK
	// We might get stuck on 'showbreak', skip over it.
	width = 1;
	വേണ്ടി (;;)
	{
	    coladvance(v - width);
	    // getviscol() is slow, skip it when 'showbreak' is empty,
	    // 'breakindent' is not set and there are no multi-byte
	    // characters
	    എങ്കിൽ ((*get_showbreak_മൂല്യം(curwin) == NUL && !curwin->w_p_bri
					     && !has_mbyte) || getviscol() < v)
		നിർത്തുക;
	    ++width;
	}
#അല്ലെങ്കിൽ
	coladvance(v - 1);
#endif

	എങ്കിൽ (curwin->w_cursor.coladd == 1)
	{
	    char_u *ptr;

	    // Adjust for multi-wide char (not a TAB)
	    ptr = ml_get_cursor();
	    എങ്കിൽ (*ptr != TAB && vim_isprintc((*mb_ptr2char)(ptr))
							 && ptr2cells(ptr) > 1)
		curwin->w_cursor.coladd = 0;
	}

	curwin->w_set_curswant = true;
	adjust_skipcol();
	തിരികെ OK;
    }

    എങ്കിൽ (curwin->w_cursor.col == 0)
	തിരികെ FAIL;

    curwin->w_set_curswant = true;
    --curwin->w_cursor.col;

    // if the character on the left of the current cursor is a multi-byte
    // character, move to its first byte
    എങ്കിൽ (has_mbyte)
	mb_adjust_cursor();
    adjust_skipcol();
    തിരികെ OK;
}

/*
 * Move the cursor up "n" lines in window "wp".
 * Takes care of closed folds.
 */
    ശൂന്യം
cursor_up_inner(win_T *wp, long n)
{
    linenr_T	lnum = wp->w_cursor.lnum;

    എങ്കിൽ (n >= lnum)
	lnum = 1;
    അല്ലെങ്കിൽ
#ifdef FEAT_FOLDING
	എങ്കിൽ (hasAnyFolding(wp))
    {
	/*
	 * Count each sequence of folded lines as one logical line.
	 */
	// go to the start of the current fold
	(ശൂന്യം)hasFoldingWin(wp, lnum, &lnum, ശൂന്യ, TRUE, ശൂന്യ);

	ആകുമ്പോൾ (n--)
	{
	    // move up one line
	    --lnum;
	    എങ്കിൽ (lnum <= 1)
		നിർത്തുക;
	    // If we entered a fold, move to the beginning, unless in
	    // Insert mode or when 'foldopen' contains "all": it will open
	    // in a moment.
	    എങ്കിൽ (n > 0 || !((State & MODE_തിരുകുക) || (fdo_flags & FDO_ALL)))
		(ശൂന്യം)hasFoldingWin(wp, lnum, &lnum, ശൂന്യ, TRUE, ശൂന്യ);
	}
	എങ്കിൽ (lnum < 1)
	    lnum = 1;
    }
    അല്ലെങ്കിൽ
#endif
	lnum -= n;

    wp->w_cursor.lnum = lnum;
}

    പൂർണ്ണസംഖ്യ
cursor_up(
    long	n,
    പൂർണ്ണസംഖ്യ		upd_topline)	    // When TRUE: update topline
{
    // This fails if the cursor is already in the first line or the count is
    // larger than the line number and '-' is in 'cpoptions'
    linenr_T lnum = curwin->w_cursor.lnum;
    എങ്കിൽ (n > 0 && (lnum <= 1
		       || (n >= lnum && vim_strchr(p_cpo, CPO_MINUS) != ശൂന്യ)))
	തിരികെ FAIL;
    cursor_up_inner(curwin, n);

    // try to advance to the column we want to be at
    coladvance(curwin->w_curswant);

    എങ്കിൽ (upd_topline)
	update_topline();	// make sure curwin->w_topline is valid

    തിരികെ OK;
}

/*
 * Move the cursor down "n" lines in window "wp".
 * Takes care of closed folds.
 */
    ശൂന്യം
cursor_down_inner(win_T *wp, long n)
{
    linenr_T	lnum = wp->w_cursor.lnum;
    linenr_T	വരി_എണ്ണം = wp->w_buffer->b_ml.ml_വരി_എണ്ണം;

    എങ്കിൽ (lnum + n >= വരി_എണ്ണം)
	lnum = വരി_എണ്ണം;
    അല്ലെങ്കിൽ
#ifdef FEAT_FOLDING
	എങ്കിൽ (hasAnyFolding(wp))
    {
	linenr_T	last;

	// count each sequence of folded lines as one logical line
	ആകുമ്പോൾ (n--)
	{
	    എങ്കിൽ (hasFoldingWin(wp, lnum, ശൂന്യ, &last, TRUE, ശൂന്യ))
		lnum = last + 1;
	    അല്ലെങ്കിൽ
		++lnum;
	    എങ്കിൽ (lnum >= വരി_എണ്ണം)
		നിർത്തുക;
	}
	എങ്കിൽ (lnum > വരി_എണ്ണം)
	    lnum = വരി_എണ്ണം;
    }
    അല്ലെങ്കിൽ
#endif
	lnum += n;

    wp->w_cursor.lnum = lnum;
}

/*
 * Cursor down a number of logical lines.
 */
    പൂർണ്ണസംഖ്യ
cursor_down(
    long	n,
    പൂർണ്ണസംഖ്യ		upd_topline)	    // When TRUE: update topline
{
    linenr_T	lnum = curwin->w_cursor.lnum;
    linenr_T	വരി_എണ്ണം = curwin->w_buffer->b_ml.ml_വരി_എണ്ണം;
    // This fails if the cursor is already in the last (folded) line, or would
    // move beyond the last line and '-' is in 'cpoptions'.
#ifdef FEAT_FOLDING
    hasFoldingWin(curwin, lnum, ശൂന്യ, &lnum, TRUE, ശൂന്യ);
#endif
    എങ്കിൽ (n > 0
	    && (lnum >= വരി_എണ്ണം
		|| (lnum + n > വരി_എണ്ണം
				     && vim_strchr(p_cpo, CPO_MINUS) != ശൂന്യ)))
	തിരികെ FAIL;
    cursor_down_inner(curwin, n);

    // try to advance to the column we want to be at
    coladvance(curwin->w_curswant);

    എങ്കിൽ (upd_topline)
	update_topline();	// make sure curwin->w_topline is valid

    തിരികെ OK;
}

/*
 * Stuff the last inserted text in the read buffer.
 * Last_insert actually is a copy of the redo buffer, so we
 * first have to remove the command.
 */
    പൂർണ്ണസംഖ്യ
stuff_inserted(
    പൂർണ്ണസംഖ്യ	    c,		// Command character to be inserted
    long    എണ്ണം,	// Repeat this many times
    പൂർണ്ണസംഖ്യ	    no_esc)	// Don't add an ESC at the end
{
    string_T	തിരുകുക;				// text to be inserted
    char_u	last = ' ';

    തിരുകുക = get_last_തിരുകുക();
    എങ്കിൽ (തിരുകുക.string == ശൂന്യ)
    {
	emsg(_(e_no_inserted_text_yet));
	തിരികെ FAIL;
    }

    // may want to stuff the command character, to start Insert mode
    എങ്കിൽ (c != NUL)
	stuffcharReadbuff(c);

    എങ്കിൽ (തിരുകുക.length > 0)
    {
	char_u	*p;

	// look for the last ESC in 'insert'
	വേണ്ടി (p = തിരുകുക.string + തിരുകുക.length - 1; p >= തിരുകുക.string; --p)
	{
	    എങ്കിൽ (*p == ESC)
	    {
		തിരുകുക.length = (size_t)(p - തിരുകുക.string);
		നിർത്തുക;
	    }
	}
    }

    എങ്കിൽ (തിരുകുക.length > 0)
    {
	char_u	*p = തിരുകുക.string + തിരുകുക.length - 1;

	// when the last char is either "0" or "^" it will be quoted if no ESC
	// comes after it OR if it will insert more than once and "ptr"
	// starts with ^D.	-- Acevedo
	എങ്കിൽ ((*p == '0' || *p == '^')
		&& (no_esc || (*തിരുകുക.string == Ctrl_D && എണ്ണം > 1)))
	{
	    last = *p;
	    --തിരുകുക.length;
	}
    }

    ചെയ്യുക
    {
	stuffReadbuffLen(തിരുകുക.string, (long)തിരുകുക.length);
	// a trailing "0" is inserted as "<C-V>048", "^" as "<C-V>^"
	തിരഞ്ഞെടുപ്പ് (last)
	{
	അവസ്ഥ '0':
#define TEXT_TO_തിരുകുക "\026\060\064\070"
	    stuffReadbuffLen((char_u *)TEXT_TO_തിരുകുക, STRLEN_LITERAL(TEXT_TO_തിരുകുക));
#undef TEXT_TO_തിരുകുക
	    നിർത്തുക;

	അവസ്ഥ '^':
#define TEXT_TO_തിരുകുക "\026^"
	    stuffReadbuffLen((char_u *)TEXT_TO_തിരുകുക, STRLEN_LITERAL(TEXT_TO_തിരുകുക));
#undef TEXT_TO_തിരുകുക
	    നിർത്തുക;

	സ്ഥിരസ്ഥിതി:
	    നിർത്തുക;
	}
    }
    ആകുമ്പോൾ (--എണ്ണം > 0);

    // may want to stuff a trailing ESC, to get out of Insert mode
    എങ്കിൽ (!no_esc)
	stuffcharReadbuff(ESC);

    തിരികെ OK;
}

    string_T
get_last_തിരുകുക(ശൂന്യം)
{
    string_T തിരുകുക = {ശൂന്യ, 0};

    എങ്കിൽ (last_തിരുകുക.string != ശൂന്യ)
    {
	തിരുകുക.string = last_തിരുകുക.string + last_തിരുകുക_skip;
	തിരുകുക.length = (size_t)(last_തിരുകുക.length - last_തിരുകുക_skip);
    }

    തിരികെ തിരുകുക;
}

/*
 * Get last inserted string, and remove trailing <Esc>.
 * Returns pointer to allocated memory (must be freed) or NULL.
 */
    char_u *
get_last_തിരുകുക_save(ശൂന്യം)
{
    string_T	തിരുകുക = get_last_തിരുകുക();
    char_u	*s;

    എങ്കിൽ (തിരുകുക.string == ശൂന്യ)
	തിരികെ ശൂന്യ;
    s = vim_strnsave(തിരുകുക.string, തിരുകുക.length);
    എങ്കിൽ (s == ശൂന്യ)
	തിരികെ ശൂന്യ;

    എങ്കിൽ (തിരുകുക.length > 0 && s[തിരുകുക.length - 1] == ESC)	// remove trailing ESC
	s[--തിരുകുക.length] = NUL;
    തിരികെ s;
}

/*
 * Check the word in front of the cursor for an abbreviation.
 * Called when the non-id character "c" has been entered.
 * When an abbreviation is recognized it is removed from the text and
 * the replacement string is inserted in typebuf.tb_buf[], followed by "c".
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
echeck_abbr(പൂർണ്ണസംഖ്യ c)
{
    // Don't check for abbreviation in paste mode, when disabled and just
    // after moving around with cursor keys.
    എങ്കിൽ (p_paste || no_abbr || arrow_used)
	തിരികെ FALSE;

    തിരികെ check_abbr(c, ml_get_curline(), curwin->w_cursor.col,
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

സ്ഥിതം char_u	*replace_അട്ടി = ശൂന്യ;
സ്ഥിതം long	replace_അട്ടി_nr = 0;	    // next entry in replace stack
സ്ഥിതം long	replace_അട്ടി_len = 0;	    // max. number of entries

    ശൂന്യം
replace_തള്ളുക(
    പൂർണ്ണസംഖ്യ	    c)	    // character that is replaced (NUL is none)
{
    char_u  *p;

    എങ്കിൽ (replace_അട്ടി_nr < replace_offset)	// nothing to do
	തിരികെ;
    എങ്കിൽ (replace_അട്ടി_len <= replace_അട്ടി_nr)
    {
	replace_അട്ടി_len += 50;
	p = ALLOC_MULT(char_u, replace_അട്ടി_len);
	എങ്കിൽ (p == ശൂന്യ)	    // out of memory
	{
	    replace_അട്ടി_len -= 50;
	    തിരികെ;
	}
	എങ്കിൽ (replace_അട്ടി != ശൂന്യ)
	{
	    mch_memmove(p, replace_അട്ടി,
				 (size_t)(replace_അട്ടി_nr * വലുപ്പം(char_u)));
	    vim_free(replace_അട്ടി);
	}
	replace_അട്ടി = p;
    }
    p = replace_അട്ടി + replace_അട്ടി_nr - replace_offset;
    എങ്കിൽ (replace_offset)
	mch_memmove(p + 1, p, (size_t)(replace_offset * വലുപ്പം(char_u)));
    *p = c;
    ++replace_അട്ടി_nr;
}

/*
 * Push a character onto the replace stack.  Handles a multi-byte character in
 * reverse byte order, so that the first byte is popped off first.
 * Return the number of bytes done (includes composing characters).
 */
    പൂർണ്ണസംഖ്യ
replace_തള്ളുക_mb(char_u *p)
{
    പൂർണ്ണസംഖ്യ l = (*mb_ptr2len)(p);
    പൂർണ്ണസംഖ്യ j;

    വേണ്ടി (j = l - 1; j >= 0; --j)
	replace_തള്ളുക(p[j]);
    തിരികെ l;
}

/*
 * Pop one item from the replace stack.
 * return -1 if stack empty
 * return replaced character or NUL otherwise
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
replace_എടുക്കുക(ശൂന്യം)
{
    എങ്കിൽ (replace_അട്ടി_nr == 0)
	തിരികെ -1;
    തിരികെ (പൂർണ്ണസംഖ്യ)replace_അട്ടി[--replace_അട്ടി_nr];
}

/*
 * Join the top two items on the replace stack.  This removes to "off"'th NUL
 * encountered.
 */
    ശൂന്യം
replace_join(
    പൂർണ്ണസംഖ്യ	    off)	// offset for which NUL to remove
{
    പൂർണ്ണസംഖ്യ	    i;

    വേണ്ടി (i = replace_അട്ടി_nr; --i >= 0; )
	എങ്കിൽ (replace_അട്ടി[i] == NUL && off-- <= 0)
	{
	    --replace_അട്ടി_nr;
	    mch_memmove(replace_അട്ടി + i, replace_അട്ടി + i + 1,
					      (size_t)(replace_അട്ടി_nr - i));
	    തിരികെ;
	}
}

/*
 * Pop bytes from the replace stack until a NUL is found, and insert them
 * before the cursor.  Can only be used in MODE_REPLACE or MODE_VREPLACE state.
 */
    സ്ഥിതം ശൂന്യം
replace_എടുക്കുക_ins(ശൂന്യം)
{
    പൂർണ്ണസംഖ്യ	    cc;
    പൂർണ്ണസംഖ്യ	    oldState = State;

    State = MODE_NORMAL;			// don't want MODE_REPLACE here
    ആകുമ്പോൾ ((cc = replace_എടുക്കുക()) > 0)
    {
	mb_replace_എടുക്കുക_ins(cc);
	dec_cursor();
    }
    State = oldState;
}

/*
 * Insert bytes popped from the replace stack. "cc" is the first byte.  If it
 * indicates a multi-byte char, pop the other bytes too.
 */
    സ്ഥിതം ശൂന്യം
mb_replace_എടുക്കുക_ins(പൂർണ്ണസംഖ്യ cc)
{
    പൂർണ്ണസംഖ്യ		n;
    char_u	buf[MB_MAXBYTES + 1];
    പൂർണ്ണസംഖ്യ		i;
    പൂർണ്ണസംഖ്യ		c;

    എങ്കിൽ (has_mbyte && (n = MB_BYTE2LEN(cc)) > 1)
    {
	buf[0] = cc;
	വേണ്ടി (i = 1; i < n; ++i)
	    buf[i] = replace_എടുക്കുക();
	ins_bytes_len(buf, n);
    }
    അല്ലെങ്കിൽ
	ins_char(cc);

    എങ്കിൽ (enc_utf8)
	// Handle composing chars.
	വേണ്ടി (;;)
	{
	    c = replace_എടുക്കുക();
	    എങ്കിൽ (c == -1)	    // stack empty
		നിർത്തുക;
	    എങ്കിൽ ((n = MB_BYTE2LEN(c)) == 1)
	    {
		// Not a multi-byte char, put it back.
		replace_തള്ളുക(c);
		നിർത്തുക;
	    }

	    buf[0] = c;
	    വേണ്ടി (i = 1; i < n; ++i)
		buf[i] = replace_എടുക്കുക();
	    എങ്കിൽ (utf_iscomposing(utf_ptr2char(buf)))
		ins_bytes_len(buf, n);
	    അല്ലെങ്കിൽ
	    {
		// Not a composing char, put it back.
		വേണ്ടി (i = n - 1; i >= 0; --i)
		    replace_തള്ളുക(buf[i]);
		നിർത്തുക;
	    }

	}
}

/*
 * make the replace stack empty
 * (called when exiting replace mode)
 */
    സ്ഥിതം ശൂന്യം
replace_flush(ശൂന്യം)
{
    VIM_CLEAR(replace_അട്ടി);
    replace_അട്ടി_len = 0;
    replace_അട്ടി_nr = 0;
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
    സ്ഥിതം ശൂന്യം
replace_do_bs(പൂർണ്ണസംഖ്യ limit_col)
{
    പൂർണ്ണസംഖ്യ		cc;
    പൂർണ്ണസംഖ്യ		orig_len = 0;
    പൂർണ്ണസംഖ്യ		ins_len;
    പൂർണ്ണസംഖ്യ		orig_vcols = 0;
    colnr_T	start_vcol;
    char_u	*p;
    പൂർണ്ണസംഖ്യ		i;
    പൂർണ്ണസംഖ്യ		vcol;

    cc = replace_എടുക്കുക();
    എങ്കിൽ (cc > 0)
    {
#ifdef FEAT_PROP_POPUP
	size_t	len_before = 0;  // init to shut up GCC

	എങ്കിൽ (curbuf->b_has_textprop)
	{
	    // Do not adjust text properties for individual delete and insert
	    // operations, do it afterwards on the resulting text.
	    len_before = ml_get_curline_len();
	    ++text_prop_frozen;
	}
#endif
	എങ്കിൽ (State & VREPLACE_FLAG)
	{
	    // Get the number of screen cells used by the character we are
	    // going to delete.
	    getvcol(curwin, &curwin->w_cursor, ശൂന്യ, &start_vcol, ശൂന്യ, 0);
	    orig_vcols = chartabsize(ml_get_cursor(), start_vcol);
	}
	എങ്കിൽ (has_mbyte)
	{
	    (ശൂന്യം)del_char_after_col(limit_col);
	    എങ്കിൽ (State & VREPLACE_FLAG)
		orig_len = ml_get_cursor_len();
	    replace_തള്ളുക(cc);
	}
	അല്ലെങ്കിൽ
	{
	    pchar_cursor(cc);
	    എങ്കിൽ (State & VREPLACE_FLAG)
		orig_len = ml_get_cursor_len() - 1;
	}
	replace_എടുക്കുക_ins();

	എങ്കിൽ (State & VREPLACE_FLAG)
	{
	    // Get the number of screen cells used by the inserted characters
	    p = ml_get_cursor();
	    ins_len = ml_get_cursor_len() - orig_len;
	    vcol = start_vcol;
	    വേണ്ടി (i = 0; i < ins_len; ++i)
	    {
		vcol += chartabsize(p + i, vcol);
		i += (*mb_ptr2len)(p + i) - 1;
	    }
	    vcol -= start_vcol;

	    // Delete spaces that were inserted after the cursor to keep the
	    // text aligned.
	    curwin->w_cursor.col += ins_len;
	    ആകുമ്പോൾ (vcol > orig_vcols && gchar_cursor() == ' ')
	    {
		del_char(FALSE);
		++orig_vcols;
	    }
	    curwin->w_cursor.col -= ins_len;
	}

	// mark the buffer as changed and prepare for displaying
	changed_bytes(curwin->w_cursor.lnum, curwin->w_cursor.col);

#ifdef FEAT_PROP_POPUP
	എങ്കിൽ (curbuf->b_has_textprop)
	{
	    size_t len_now = ml_get_curline_len();

	    --text_prop_frozen;
	    adjust_prop_columns(curwin->w_cursor.lnum, curwin->w_cursor.col,
					   (പൂർണ്ണസംഖ്യ)(len_now - len_before), 0);
	}
#endif
    }
    അല്ലെങ്കിൽ എങ്കിൽ (cc == 0)
	(ശൂന്യം)del_char_after_col(limit_col);
}

#എങ്കിൽ defined(FEAT_RIGHTLEFT)
/*
 * Map Hebrew keyboard when in hkmap mode.
 */
    പൂർണ്ണസംഖ്യ
hkmap(പൂർണ്ണസംഖ്യ c)
{
    എങ്കിൽ (p_hkmapp)   // phonetic mapping, by Ilya Dogolazky
    {
	എണ്ണൽ {hALEF=0, BET, GIMEL, DALET, HEI, VAV, ZAIN, HET, TET, IUD,
	    KAFsofit, hKAF, LAMED, MEMsofit, MEM, NUNsofit, NUN, SAMEH, AIN,
	    PEIsofit, PEI, ZADIsofit, ZADI, KOF, RESH, hSHIN, TAV};
	സ്ഥിതം char_u map[26] =
	    {(char_u)hALEF/*a*/, (char_u)BET  /*b*/, (char_u)hKAF    /*c*/,
	     (char_u)DALET/*d*/, (char_u)-1   /*e*/, (char_u)PEIsofit/*f*/,
	     (char_u)GIMEL/*g*/, (char_u)HEI  /*h*/, (char_u)IUD     /*i*/,
	     (char_u)HET  /*j*/, (char_u)KOF  /*k*/, (char_u)LAMED   /*l*/,
	     (char_u)MEM  /*m*/, (char_u)NUN  /*n*/, (char_u)SAMEH   /*o*/,
	     (char_u)PEI  /*p*/, (char_u)-1   /*q*/, (char_u)RESH    /*r*/,
	     (char_u)ZAIN /*s*/, (char_u)TAV  /*t*/, (char_u)TET     /*u*/,
	     (char_u)VAV  /*v*/, (char_u)hSHIN/*w*/, (char_u)-1      /*x*/,
	     (char_u)AIN  /*y*/, (char_u)ZADI /*z*/};

	എങ്കിൽ (c == 'N' || c == 'M' || c == 'P' || c == 'C' || c == 'Z')
	    തിരികെ (പൂർണ്ണസംഖ്യ)(map[CharOrd(c)] - 1 + p_aleph);
							    // '-1'='sofit'
	അല്ലെങ്കിൽ എങ്കിൽ (c == 'x')
	    തിരികെ 'X';
	അല്ലെങ്കിൽ എങ്കിൽ (c == 'q')
	    തിരികെ '\''; // {geresh}={'}
	അല്ലെങ്കിൽ എങ്കിൽ (c == 246)
	    തിരികെ ' ';  // \"o --> ' ' for a german keyboard
	അല്ലെങ്കിൽ എങ്കിൽ (c == 228)
	    തിരികെ ' ';  // \"a --> ' '      -- / --
	അല്ലെങ്കിൽ എങ്കിൽ (c == 252)
	    തിരികെ ' ';  // \"u --> ' '      -- / --
	// NOTE: islower() does not do the right thing for us on Linux so we
	// do this the same was as 5.7 and previous, so it works correctly on
	// all systems.  Specifically, the e.g. Delete and Arrow keys are
	// munged and won't work if e.g. searching for Hebrew text.
	അല്ലെങ്കിൽ എങ്കിൽ (c >= 'a' && c <= 'z')
	    തിരികെ (പൂർണ്ണസംഖ്യ)(map[Char_Ord_താഴെയുള്ള(c)] + p_aleph);
	അല്ലെങ്കിൽ
	    തിരികെ c;
    }
    അല്ലെങ്കിൽ
    {
	തിരഞ്ഞെടുപ്പ് (c)
	{
	    അവസ്ഥ '`':	തിരികെ ';';
	    അവസ്ഥ '/':	തിരികെ '.';
	    അവസ്ഥ '\'':	തിരികെ ',';
	    അവസ്ഥ 'q':	തിരികെ '/';
	    അവസ്ഥ 'w':	തിരികെ '\'';

			// Hebrew letters - set offset from 'a'
	    അവസ്ഥ ',':	c = '{'; നിർത്തുക;
	    അവസ്ഥ '.':	c = 'v'; നിർത്തുക;
	    അവസ്ഥ ';':	c = 't'; നിർത്തുക;
	    സ്ഥിരസ്ഥിതി: {
			 സ്ഥിതം അക്ഷരം str[] = "zqbcxlsjphmkwonu ydafe rig";

			 എങ്കിൽ (c < 'a' || c > 'z')
			     തിരികെ c;
			 c = str[Char_Ord_താഴെയുള്ള(c)];
			 നിർത്തുക;
		     }
	}

	തിരികെ (പൂർണ്ണസംഖ്യ)(Char_Ord_താഴെയുള്ള(c) + p_aleph);
    }
}
#endif

    സ്ഥിതം ശൂന്യം
ins_reg(ശൂന്യം)
{
    പൂർണ്ണസംഖ്യ		need_redraw = FALSE;
    പൂർണ്ണസംഖ്യ		regname;
    പൂർണ്ണസംഖ്യ		literally = 0;
    പൂർണ്ണസംഖ്യ		vis_active = VIsual_active;

    /*
     * If we are going to wait for a character, show a '"'.
     */
    pc_status = PC_STATUS_UNSET;
    എങ്കിൽ (redrawing() && !char_avail())
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
    ++allow_താക്കോലുകൾ;
    regname = plain_vgetc();
    LANGMAP_ADJUST(regname, TRUE);
    എങ്കിൽ (regname == Ctrl_R || regname == Ctrl_O || regname == Ctrl_P)
    {
	// Get a third key for literal register insertion
	literally = regname;
	add_to_showcmd_c(literally);
	regname = plain_vgetc();
	LANGMAP_ADJUST(regname, TRUE);
    }
    --no_mapping;
    --allow_താക്കോലുകൾ;

#ifdef FEAT_EVAL
    // Don't call u_sync() while typing the expression or giving an error
    // message for it. Only call it explicitly.
    ++no_u_sync;
    എങ്കിൽ (regname == '=')
    {
	pos_T	curpos = curwin->w_cursor;
# ifdef HAVE_നിവേശം_METHOD
	പൂർണ്ണസംഖ്യ	im_on = im_get_status();
# endif
	// Sync undo when evaluating the expression calls setline() or
	// append(), so that it can be undone separately.
	u_sync_once = 2;

	regname = get_വ്യഞ്ജകം_register();

	// Cursor may be moved back a column.
	curwin->w_cursor = curpos;
	check_cursor();
# ifdef HAVE_നിവേശം_METHOD
	// Restore the Input Method.
	എങ്കിൽ (im_on)
	    im_set_active(TRUE);
# endif
    }
    എങ്കിൽ (regname == NUL || !valid_yank_reg(regname, FALSE))
    {
	vim_beep(BO_REG);
	need_redraw = TRUE;	// remove the '"'
    }
    അല്ലെങ്കിൽ
    {
#endif
	എങ്കിൽ (literally == Ctrl_O || literally == Ctrl_P)
	{
	    // Append the command to the redo buffer.
	    AppendCharToRedobuff(Ctrl_R);
	    AppendCharToRedobuff(literally);
	    AppendCharToRedobuff(regname);

	    do_put(regname, ശൂന്യ, BACKWARD, 1L,
		 (literally == Ctrl_P ? PUT_FIXINDENT : 0) | PUT_CURSEND);
	}
	അല്ലെങ്കിൽ എങ്കിൽ (തിരുകുക_reg(regname, literally) == FAIL)
	{
	    vim_beep(BO_REG);
	    need_redraw = TRUE;	// remove the '"'
	}
	അല്ലെങ്കിൽ എങ്കിൽ (stop_തിരുകുക_mode)
	    // When the '=' register was used and a function was invoked that
	    // did ":stopinsert" then stuff_empty() returns FALSE but we won't
	    // insert anything, need to remove the '"'
	    need_redraw = TRUE;

#ifdef FEAT_EVAL
    }
    --no_u_sync;
    എങ്കിൽ (u_sync_once == 1)
	ins_need_undo = TRUE;
    u_sync_once = 0;
#endif
    clear_showcmd();

    // If the inserted register is empty, we need to remove the '"'
    എങ്കിൽ (need_redraw || stuff_empty())
	edit_unputchar();

    // Disallow starting Visual mode here, would get a weird mode.
    എങ്കിൽ (!vis_active && VIsual_active)
	end_visual_mode();
}

/*
 * CTRL-G commands in Insert mode.
 */
    സ്ഥിതം ശൂന്യം
ins_ctrl_g(ശൂന്യം)
{
    പൂർണ്ണസംഖ്യ		c;

    // Right after CTRL-X the cursor will be after the ruler.
    setcursor();

    /*
     * Don't map the second key. This also prevents the mode message to be
     * deleted when ESC is hit.
     */
    ++no_mapping;
    ++allow_താക്കോലുകൾ;
    c = plain_vgetc();
    --no_mapping;
    --allow_താക്കോലുകൾ;
    തിരഞ്ഞെടുപ്പ് (c)
    {
	// CTRL-G k and CTRL-G <Up>: cursor up to Insstart.col
	അവസ്ഥ K_UP:
	അവസ്ഥ Ctrl_K:
	അവസ്ഥ 'k': ins_up(TRUE);
		  നിർത്തുക;

	// CTRL-G j and CTRL-G <Down>: cursor down to Insstart.col
	അവസ്ഥ K_DOWN:
	അവസ്ഥ Ctrl_J:
	അവസ്ഥ 'j': ins_down(TRUE);
		  നിർത്തുക;

	// CTRL-G u: start new undoable edit
	അവസ്ഥ 'u': u_sync(TRUE);
		  ins_need_undo = TRUE;

		  // Need to reset Insstart, esp. because a BS that joins
		  // a line to the previous one must save for undo.
		  update_Insstart_orig = FALSE;
		  Insstart = curwin->w_cursor;
		  നിർത്തുക;

	// CTRL-G U: do not break undo with the next char
	അവസ്ഥ 'U':
		  // Allow one left/right cursor movement with the next char,
		  // without breaking undo.
		  dont_sync_undo = MAYBE;
		  നിർത്തുക;

	അവസ്ഥ ESC:
		  // Esc after CTRL-G cancels it.
		  നിർത്തുക;

	// Unknown CTRL-G command, reserved for future expansion.
	സ്ഥിരസ്ഥിതി:  vim_beep(BO_CTRLG);
    }
}

/*
 * CTRL-^ in Insert mode.
 */
    സ്ഥിതം ശൂന്യം
ins_ctrl_hat(ശൂന്യം)
{
    എങ്കിൽ (map_to_exists_mode((char_u *)"", MODE_LANGMAP, FALSE))
    {
	// ":lmap" mappings exists, Toggle use of ":lmap" mappings.
	എങ്കിൽ (State & MODE_LANGMAP)
	{
	    curbuf->b_p_iminsert = B_IMODE_NONE;
	    State &= ~MODE_LANGMAP;
	}
	അല്ലെങ്കിൽ
	{
	    curbuf->b_p_iminsert = B_IMODE_LMAP;
	    State |= MODE_LANGMAP;
#ifdef HAVE_നിവേശം_METHOD
	    im_set_active(FALSE);
#endif
	}
    }
#ifdef HAVE_നിവേശം_METHOD
    അല്ലെങ്കിൽ
    {
	// There are no ":lmap" mappings, toggle IM
	എങ്കിൽ (im_get_status())
	{
	    curbuf->b_p_iminsert = B_IMODE_NONE;
	    im_set_active(FALSE);
	}
	അല്ലെങ്കിൽ
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
    എങ്കിൽ (gui.in_use)
	gui_update_cursor(TRUE, FALSE);
#endif
#എങ്കിൽ defined(FEAT_KEYMAP)
    // Show/unshow value of 'keymap' in status lines.
    status_redraw_curbuf();
#endif
}

/*
 * Handle ESC in insert mode.
 * Returns TRUE when leaving insert mode, FALSE when going to repeat the
 * insert.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
ins_esc(
    long	*എണ്ണം,
    പൂർണ്ണസംഖ്യ		cmdchar,
    പൂർണ്ണസംഖ്യ		nomove)	    // don't move cursor
{
    പൂർണ്ണസംഖ്യ		temp;
    സ്ഥിതം പൂർണ്ണസംഖ്യ	disabled_redraw = FALSE;
#ifdef FEAT_CONCEAL
    // Remember if the cursor line was concealed before changing State.
    പൂർണ്ണസംഖ്യ		cursor_വരി_was_concealed = curwin->w_p_cole > 0
						&& conceal_cursor_വരി(curwin);
#endif

#ifdef FEAT_SPELL
    check_spell_redraw();
#endif

    temp = curwin->w_cursor.col;
    എങ്കിൽ (disabled_redraw)
    {
	എങ്കിൽ (RedrawingDisabled > 0)
	    --RedrawingDisabled;
	disabled_redraw = FALSE;
    }
    എങ്കിൽ (!arrow_used)
    {
	/*
	 * Don't append the ESC for "r<CR>" and "grx".
	 * When 'insertmode' is set only CTRL-L stops Insert mode.  Needed for
	 * when "count" is non-zero.
	 */
	എങ്കിൽ (cmdchar != 'r' && cmdchar != 'v')
	    AppendToRedobuff(p_im ? (char_u *)"\014" : ESC_STR);

	/*
	 * Repeating insert may take a long time.  Check for
	 * interrupt now and then.
	 */
	എങ്കിൽ (*എണ്ണം > 0)
	{
	    വരി_breakcheck();
	    എങ്കിൽ (got_int)
		*എണ്ണം = 0;
	}

	എങ്കിൽ (--*എണ്ണം > 0)	// repeat what was typed
	{
	    // Vi repeats the insert without replacing characters.
	    എങ്കിൽ (vim_strchr(p_cpo, CPO_REPLCNT) != ശൂന്യ)
		State &= ~REPLACE_FLAG;

	    (ശൂന്യം)start_redo_ins();
	    എങ്കിൽ (cmdchar == 'r' || cmdchar == 'v')
		stuffRedoReadbuff(ESC_STR);	// no ESC in redo buffer
	    ++RedrawingDisabled;
	    disabled_redraw = TRUE;
	    തിരികെ FALSE;	// repeat the insert
	}
	stop_തിരുകുക(&curwin->w_cursor, TRUE, nomove);
	undisplay_dollar();
    }

    എങ്കിൽ (cmdchar != 'r' && cmdchar != 'v')
	ins_apply_autocmds(EVENT_INSERTLEAVEPRE);

    // When an autoindent was removed, curswant stays after the
    // indent
    എങ്കിൽ (restart_edit == NUL && (colnr_T)temp == curwin->w_cursor.col)
	curwin->w_set_curswant = true;

    // Remember the last Insert position in the '^ mark.
    എങ്കിൽ ((cmdmod.cmod_flags & CMOD_KEEPJUMPS) == 0)
	curbuf->b_last_തിരുകുക = curwin->w_cursor;

    /*
     * The cursor should end up on the last inserted character.
     * Don't do it for CTRL-O, unless past the end of the line.
     */
    എങ്കിൽ (!nomove
	    && (curwin->w_cursor.col != 0
		|| curwin->w_cursor.coladd > 0)
	    && (restart_edit == NUL
		   || (gchar_cursor() == NUL && !VIsual_active))
#ifdef FEAT_RIGHTLEFT
	    && !revins_on
#endif
				      )
    {
	എങ്കിൽ (curwin->w_cursor.coladd > 0 || get_ve_flags() == VE_ALL)
	{
	    oneleft();
	    എങ്കിൽ (restart_edit != NUL)
		++curwin->w_cursor.coladd;
	}
	അല്ലെങ്കിൽ
	{
	    --curwin->w_cursor.col;
	    curwin->w_valid &= ~(VALID_WCOL|VALID_VIRTCOL);
	    // Correct cursor for multi-byte character.
	    എങ്കിൽ (has_mbyte)
		mb_adjust_cursor();
	}
    }

#ifdef HAVE_നിവേശം_METHOD
    // Disable IM to allow typing English directly for Normal mode commands.
    // When ":lmap" is enabled don't change 'iminsert' (IM can be enabled as
    // well).
    എങ്കിൽ (!(State & MODE_LANGMAP))
	im_save_status(&curbuf->b_p_iminsert);
    im_set_active(FALSE);
#endif

    State = MODE_NORMAL;
    may_trigger_modechanged();
    // need to position cursor again when on a TAB and when on a char with
    // virtual text.
    എങ്കിൽ (gchar_cursor() == TAB
#ifdef FEAT_PROP_POPUP
	    || curbuf->b_has_textprop
#endif
       )
	curwin->w_valid &= ~(VALID_WROW|VALID_WCOL|VALID_VIRTCOL);

    setmouse();
#ifdef CURSOR_SHAPE
    ui_cursor_shape();		// may show different cursor shape
#endif
    എങ്കിൽ (!p_ek)
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
    conceal_check_cursor_വരി(cursor_വരി_was_concealed);
#endif

    // When recording or for CTRL-O, need to display the new mode.
    // Otherwise remove the mode message.
    എങ്കിൽ (reg_recording != 0 || restart_edit != NUL)
	showmode();
    അല്ലെങ്കിൽ എങ്കിൽ (p_smd && (got_int || !skip_showmode()))
	സന്ദേശം("");

    തിരികെ TRUE;	    // exit Insert mode
}

#ifdef FEAT_RIGHTLEFT
/*
 * Toggle language: hkmap and revins_on.
 * Move to end of reverse inserted text.
 */
    സ്ഥിതം ശൂന്യം
ins_ctrl_(ശൂന്യം)
{
    എങ്കിൽ (revins_on && revins_chars && revins_scol >= 0)
    {
	ആകുമ്പോൾ (gchar_cursor() != NUL && revins_chars--)
	    ++curwin->w_cursor.col;
    }
    p_ri = !p_ri;
    revins_on = (State == MODE_തിരുകുക && p_ri);
    എങ്കിൽ (revins_on)
    {
	revins_scol = curwin->w_cursor.col;
	revins_legal++;
	revins_chars = 0;
	undisplay_dollar();
    }
    അല്ലെങ്കിൽ
	revins_scol = -1;
    p_hkmap = curwin->w_p_rl ^ p_ri;    // be consistent!
    showmode();
}
#endif

/*
 * If 'keymodel' contains "startsel", may start selection.
 * Returns TRUE when a CTRL-O and other keys stuffed.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
ins_start_select(പൂർണ്ണസംഖ്യ c)
{
    എങ്കിൽ (!km_startsel)
	തിരികെ FALSE;
    തിരഞ്ഞെടുപ്പ് (c)
    {
	അവസ്ഥ K_KHOME:
	അവസ്ഥ K_KEND:
	അവസ്ഥ K_PAGEUP:
	അവസ്ഥ K_KPAGEUP:
	അവസ്ഥ K_PAGEDOWN:
	അവസ്ഥ K_KPAGEDOWN:
#ifdef MACOS_X
	അവസ്ഥ K_ഇടത്:
	അവസ്ഥ K_വലത്:
	അവസ്ഥ K_UP:
	അവസ്ഥ K_DOWN:
	അവസ്ഥ K_END:
	അവസ്ഥ K_HOME:
#endif
	    എങ്കിൽ (!(mod_mask & MOD_MASK_SHIFT))
		നിർത്തുക;
	    // FALLTHROUGH
	അവസ്ഥ K_S_ഇടത്:
	അവസ്ഥ K_S_വലത്:
	അവസ്ഥ K_S_UP:
	അവസ്ഥ K_S_DOWN:
	അവസ്ഥ K_S_END:
	അവസ്ഥ K_S_HOME:
	    // Start selection right away, the cursor can move with CTRL-O when
	    // beyond the end of the line.
	    start_selection();

	    // Execute the key in (insert) Select mode.
	    stuffcharReadbuff(Ctrl_O);
	    എങ്കിൽ (mod_mask)
	    {
		char_u	    buf[4];

		buf[0] = K_SPECIAL;
		buf[1] = KS_MODIFIER;
		buf[2] = mod_mask;
		buf[3] = NUL;
		stuffReadbuffLen(buf, 3L);
	    }
	    stuffcharReadbuff(c);
	    തിരികെ TRUE;
    }
    തിരികെ FALSE;
}

/*
 * <Insert> key in Insert mode: toggle insert/replace mode.
 */
    സ്ഥിതം ശൂന്യം
ins_തിരുകുക(പൂർണ്ണസംഖ്യ replaceState)
{
#ifdef FEAT_EVAL
    set_vim_var_string(VV_INSERTMODE,
		   (char_u *)((State & REPLACE_FLAG) ? "i"
			    : replaceState == MODE_VREPLACE ? "v" : "r"), 1);
#endif
    ins_apply_autocmds(EVENT_INSERTCHANGE);
    എങ്കിൽ (State & REPLACE_FLAG)
	State = MODE_തിരുകുക | (State & MODE_LANGMAP);
    അല്ലെങ്കിൽ
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
    സ്ഥിതം ശൂന്യം
ins_ctrl_o(ശൂന്യം)
{
    restart_VIsual_select = 0;
    എങ്കിൽ (State & VREPLACE_FLAG)
	restart_edit = 'V';
    അല്ലെങ്കിൽ എങ്കിൽ (State & REPLACE_FLAG)
	restart_edit = 'R';
    അല്ലെങ്കിൽ
	restart_edit = 'I';
    എങ്കിൽ (virtual_active())
	ins_at_eol = FALSE;	// cursor always keeps its column
    അല്ലെങ്കിൽ
	ins_at_eol = (gchar_cursor() == NUL);
}

/*
 * If the cursor is on an indent, ^T/^D insert/delete one
 * shiftwidth.	Otherwise ^T/^D behave like a "<<" or ">>".
 * Always round the indent to 'shiftwidth', this is compatible
 * with vi.  But vi only supports ^T and ^D after an
 * autoindent, we support it everywhere.
 */
    സ്ഥിതം ശൂന്യം
ins_shift(പൂർണ്ണസംഖ്യ c, പൂർണ്ണസംഖ്യ lastc)
{
    എങ്കിൽ (stop_arrow() == FAIL)
	തിരികെ;
    AppendCharToRedobuff(c);

    /*
     * 0^D and ^^D: remove all indent.
     */
    എങ്കിൽ (c == Ctrl_D && (lastc == '0' || lastc == '^')
						  && curwin->w_cursor.col > 0)
    {
	--curwin->w_cursor.col;
	(ശൂന്യം)del_char(FALSE);		// delete the '^' or '0'
	// In Replace mode, restore the characters that '^' or '0' replaced.
	എങ്കിൽ (State & REPLACE_FLAG)
	    replace_എടുക്കുക_ins();
	എങ്കിൽ (lastc == '^')
	    old_indent = get_indent();	// remember curr. indent
	change_indent(INDENT_SET, 0, TRUE, 0, TRUE);
    }
    അല്ലെങ്കിൽ
	change_indent(c == Ctrl_D ? INDENT_DEC : INDENT_INC, 0, TRUE, 0, TRUE);

    എങ്കിൽ (did_ai && *skipwhite(ml_get_curline()) != NUL)
	did_ai = FALSE;
    did_si = FALSE;
    can_si = FALSE;
    can_si_back = FALSE;
    can_cindent = FALSE;	// no cindenting after ^D or ^T
}

    സ്ഥിതം ശൂന്യം
ins_del(ശൂന്യം)
{
    പൂർണ്ണസംഖ്യ	    temp;

    എങ്കിൽ (stop_arrow() == FAIL)
	തിരികെ;
    എങ്കിൽ (gchar_cursor() == NUL)		// delete newline
    {
	temp = curwin->w_cursor.col;
	എങ്കിൽ (!can_bs(BS_EOL)		// only if "eol" included
		|| do_join(2, FALSE, TRUE, FALSE, FALSE) == FAIL)
	    vim_beep(BO_BS);
	അല്ലെങ്കിൽ
	{
	    curwin->w_cursor.col = temp;
	    // Adjust orig_line_count in case more lines have been deleted than
	    // have been added. That makes sure, that open_line() later
	    // can access all buffer lines correctly
	    എങ്കിൽ (State & VREPLACE_FLAG &&
		    orig_വരി_എണ്ണം > curbuf->b_ml.ml_വരി_എണ്ണം)
		orig_വരി_എണ്ണം = curbuf->b_ml.ml_വരി_എണ്ണം;
	}
    }
    അല്ലെങ്കിൽ എങ്കിൽ (del_char(FALSE) == FAIL)  // delete char under cursor
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
    സ്ഥിതം ശൂന്യം
ins_bs_one(ശൂന്യം)
{
    dec_cursor();
    എങ്കിൽ (State & REPLACE_FLAG)
    {
	// Don't delete characters before the insert point when in
	// Replace mode
	എങ്കിൽ (curwin->w_cursor.lnum != Insstart.lnum
		|| curwin->w_cursor.col >= Insstart.col)
	    replace_do_bs(-1);
    }
    അല്ലെങ്കിൽ
	(ശൂന്യം)del_char(FALSE);
}

/*
 * Handle Backspace, delete-word and delete-line in Insert mode.
 * Return TRUE when backspace was actually used.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
ins_bs(
    പൂർണ്ണസംഖ്യ		c,
    പൂർണ്ണസംഖ്യ		mode,
    പൂർണ്ണസംഖ്യ		*inserted_space_p)
{
    linenr_T	lnum;
    പൂർണ്ണസംഖ്യ		cc;
    പൂർണ്ണസംഖ്യ		temp = 0;	    // init for GCC
    colnr_T	save_col;
    colnr_T	mincol;
    പൂർണ്ണസംഖ്യ		did_backspace = FALSE;
    പൂർണ്ണസംഖ്യ		in_indent;
    പൂർണ്ണസംഖ്യ		oldState;
    പൂർണ്ണസംഖ്യ		cpc[MAX_MCO];	    // composing characters
    പൂർണ്ണസംഖ്യ		call_fix_indent = FALSE;

    /*
     * can't delete anything in an empty file
     * can't backup past first character in buffer
     * can't backup past starting point unless 'backspace' > 1
     * can backup to a previous line if 'backspace' == 0
     */
    എങ്കിൽ (       BUFEMPTY()
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
	തിരികെ FALSE;
    }

    എങ്കിൽ (stop_arrow() == FAIL)
	തിരികെ FALSE;
    in_indent = inindent(0);
    എങ്കിൽ (in_indent)
	can_cindent = FALSE;
    end_comment_pending = NUL;	// After BS, don't auto-end comment
#ifdef FEAT_RIGHTLEFT
    എങ്കിൽ (revins_on)	    // put cursor after last inserted char
	inc_cursor();
#endif

    // Virtualedit:
    //	BACKSPACE_CHAR eats a virtual space
    //	BACKSPACE_WORD eats all coladd
    //	BACKSPACE_LINE eats all coladd and keeps going
    എങ്കിൽ (curwin->w_cursor.coladd > 0)
    {
	എങ്കിൽ (mode == BACKSPACE_CHAR)
	{
	    --curwin->w_cursor.coladd;
	    തിരികെ TRUE;
	}
	എങ്കിൽ (mode == BACKSPACE_WORD)
	{
	    curwin->w_cursor.coladd = 0;
	    തിരികെ TRUE;
	}
	curwin->w_cursor.coladd = 0;
    }

    /*
     * Delete newline!
     */
    എങ്കിൽ (curwin->w_cursor.col == 0)
    {
	lnum = Insstart.lnum;
	എങ്കിൽ (curwin->w_cursor.lnum == lnum
#ifdef FEAT_RIGHTLEFT
			|| revins_on
#endif
				    )
	{
	    എങ്കിൽ (u_save((linenr_T)(curwin->w_cursor.lnum - 2),
			       (linenr_T)(curwin->w_cursor.lnum + 1)) == FAIL)
		തിരികെ FALSE;
	    --Insstart.lnum;
	    Insstart.col = ml_get_len(Insstart.lnum);
	}
	/*
	 * In replace mode:
	 * cc < 0: NL was inserted, delete it
	 * cc >= 0: NL was replaced, put original characters back
	 */
	cc = -1;
	എങ്കിൽ (State & REPLACE_FLAG)
	    cc = replace_എടുക്കുക();	    // returns -1 if NL was inserted
	/*
	 * In replace mode, in the line we started replacing, we only move the
	 * cursor.
	 */
	എങ്കിൽ ((State & REPLACE_FLAG) && curwin->w_cursor.lnum <= lnum)
	{
	    dec_cursor();
	}
	അല്ലെങ്കിൽ
	{
	    എങ്കിൽ (!(State & VREPLACE_FLAG)
				   || curwin->w_cursor.lnum > orig_വരി_എണ്ണം)
	    {
		temp = gchar_cursor();	// remember current char
		--curwin->w_cursor.lnum;

		// When "aw" is in 'formatoptions' we must delete the space at
		// the end of the line, otherwise the line will be broken
		// again when auto-formatting.
		എങ്കിൽ (has_format_option(FO_AUTO)
					   && has_format_option(FO_WHITE_PAR))
		{
		    char_u  *ptr = ml_get_buf(curbuf, curwin->w_cursor.lnum,
									FALSE);
		    പൂർണ്ണസംഖ്യ	    len = ml_get_curline_len();

		    എങ്കിൽ (len > 0 && ptr[len - 1] == ' ')
		    {
			char_u *newp = alloc(curbuf->b_ml.ml_വരി_len - 1);

			എങ്കിൽ (newp != ശൂന്യ)
			{
			    mch_memmove(newp, ptr, len - 1);
			    newp[len - 1] = NUL;
			    എങ്കിൽ (curbuf->b_ml.ml_വരി_len > len + 1)
				mch_memmove(newp + len, ptr + len + 1,
					   curbuf->b_ml.ml_വരി_len - len - 1);

			    എങ്കിൽ (curbuf->b_ml.ml_flags
					      & (ML_വരി_DIRTY | ML_ALLOCATED))
				vim_free(curbuf->b_ml.ml_വരി_ptr);
			    curbuf->b_ml.ml_വരി_ptr = newp;
			    curbuf->b_ml.ml_വരി_len--;
			    curbuf->b_ml.ml_വരി_textlen--;
			    curbuf->b_ml.ml_flags |= ML_വരി_DIRTY;
			}
		    }
		}

		(ശൂന്യം)do_join(2, FALSE, FALSE, FALSE, FALSE);
		എങ്കിൽ (temp == NUL && gchar_cursor() != NUL)
		    inc_cursor();
	    }
	    അല്ലെങ്കിൽ
		dec_cursor();

	    /*
	     * In MODE_REPLACE mode we have to put back the text that was
	     * replaced by the NL. On the replace stack is first a
	     * NUL-terminated sequence of characters that were deleted and then
	     * the characters that NL replaced.
	     */
	    എങ്കിൽ (State & REPLACE_FLAG)
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
		ആകുമ്പോൾ (cc > 0)
		{
		    save_col = curwin->w_cursor.col;
		    mb_replace_എടുക്കുക_ins(cc);
		    curwin->w_cursor.col = save_col;
		    cc = replace_എടുക്കുക();
		}
		// restore the characters that NL replaced
		replace_എടുക്കുക_ins();
		State = oldState;
	    }
	}
	did_ai = FALSE;
    }
    അല്ലെങ്കിൽ
    {
	/*
	 * Delete character(s) before the cursor.
	 */
#ifdef FEAT_RIGHTLEFT
	എങ്കിൽ (revins_on)		// put cursor on last inserted char
	    dec_cursor();
#endif
	mincol = 0;
						// keep indent
	എങ്കിൽ (mode == BACKSPACE_വരി
		&& (curbuf->b_p_ai || cindent_on())
#ifdef FEAT_RIGHTLEFT
		&& !revins_on
#endif
			    )
	{
	    save_col = curwin->w_cursor.col;
	    beginline(BL_WHITE);
	    എങ്കിൽ (curwin->w_cursor.col < save_col)
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
	എങ്കിൽ (	   mode == BACKSPACE_CHAR
		&& ((p_sta && in_indent)
		    || ((get_sts_മൂല്യം() != 0
#ifdef FEAT_VARTABS
			|| tabstop_എണ്ണം(curbuf->b_p_vsts_array)
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
	    char_u	*വരി;
	    char_u	*ptr;
	    char_u	*cursor_ptr;
	    char_u	*space_ptr;
	    colnr_T	space_vcol = 0;
	    പൂർണ്ണസംഖ്യ		prev_space = FALSE;
	    colnr_T	want_col;

	    *inserted_space_p = FALSE;

	    space_ptr = ptr = വരി = ml_get_curline();
	    cursor_ptr = വരി + curwin->w_cursor.col;

	    // Compute virtual column of cursor position, and find the last
	    // whitespace before cursor that is preceded by non-whitespace.
	    // Use chartabsize() so that virtual text and wrapping are ignored.
	    ആകുമ്പോൾ (ptr < cursor_ptr)
	    {
		പൂർണ്ണസംഖ്യ	cur_space = VIM_ISWHITE(*ptr);

		എങ്കിൽ (!prev_space && cur_space)
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
	    എങ്കിൽ (p_sta && in_indent)
		want_vcol -= want_vcol % (പൂർണ്ണസംഖ്യ)get_sw_മൂല്യം(curbuf);
	    അല്ലെങ്കിൽ
#ifdef FEAT_VARTABS
		want_vcol = tabstop_start(want_vcol, get_sts_മൂല്യം(),
						       curbuf->b_p_vsts_array);
#അല്ലെങ്കിൽ
		want_vcol -= want_vcol % (പൂർണ്ണസംഖ്യ)get_sts_മൂല്യം();
#endif

	    // Find the position to stop backspacing.
	    // Use chartabsize() so that virtual text and wrapping are ignored.
	    ആകുമ്പോൾ (TRUE)
	    {
		പൂർണ്ണസംഖ്യ size = chartabsize(space_ptr, space_vcol);

		എങ്കിൽ (space_vcol + size > want_vcol)
		    നിർത്തുക;
		space_vcol += size;
		MB_PTR_ADV(space_ptr);
	    }
	    want_col = space_ptr - വരി;

	    // Delete characters until we are at or before want_col.
	    ആകുമ്പോൾ (curwin->w_cursor.col > want_col)
		ins_bs_one();

	    // Insert extra spaces until we are at want_vcol.
	    വേണ്ടി (; space_vcol < want_vcol; space_vcol++)
	    {
		// Remember the first char we inserted
		എങ്കിൽ (curwin->w_cursor.lnum == Insstart_orig.lnum
				   && curwin->w_cursor.col < Insstart_orig.col)
		    Insstart_orig.col = curwin->w_cursor.col;

		എങ്കിൽ (State & VREPLACE_FLAG)
		    ins_char(' ');
		അല്ലെങ്കിൽ
		{
		    ins_str((char_u *)" ", 1);
		    എങ്കിൽ ((State & REPLACE_FLAG))
			replace_തള്ളുക(NUL);
		}
	    }
	}

	/*
	 * Delete up to starting point, start of line or previous word.
	 */
	അല്ലെങ്കിൽ
	{
	    പൂർണ്ണസംഖ്യ cclass = 0, prev_cclass = 0;

	    എങ്കിൽ (has_mbyte)
		cclass = mb_get_class(ml_get_cursor());
	    ചെയ്യുക
	    {
#ifdef FEAT_RIGHTLEFT
		എങ്കിൽ (!revins_on) // put cursor on char to be deleted
#endif
		    dec_cursor();

		cc = gchar_cursor();
		// look multi-byte character class
		എങ്കിൽ (has_mbyte)
		{
		    prev_cclass = cclass;
		    cclass = mb_get_class(ml_get_cursor());
		}

		// start of word?
		എങ്കിൽ (mode == BACKSPACE_WORD && !vim_isspace(cc))
		{
		    mode = BACKSPACE_WORD_NOT_SPACE;
		    temp = vim_iswordc(cc);
		}
		// end of word?
		അല്ലെങ്കിൽ എങ്കിൽ (mode == BACKSPACE_WORD_NOT_SPACE
			&& ((vim_isspace(cc) || vim_iswordc(cc) != temp)
			|| prev_cclass != cclass))
		{
#ifdef FEAT_RIGHTLEFT
		    എങ്കിൽ (!revins_on)
#endif
			inc_cursor();
#ifdef FEAT_RIGHTLEFT
		    അല്ലെങ്കിൽ എങ്കിൽ (State & REPLACE_FLAG)
			dec_cursor();
#endif
		    നിർത്തുക;
		}
		എങ്കിൽ (State & REPLACE_FLAG)
		    replace_do_bs(-1);
		അല്ലെങ്കിൽ
		{
		    എങ്കിൽ (enc_utf8 && p_deco)
			(ശൂന്യം)utfc_ptr2char(ml_get_cursor(), cpc);
		    (ശൂന്യം)del_char(FALSE);
		    /*
		     * If there are combining characters and 'delcombine' is set
		     * move the cursor back.  Don't back up before the base
		     * character.
		     */
		    എങ്കിൽ (enc_utf8 && p_deco && cpc[0] != NUL)
			inc_cursor();
#ifdef FEAT_RIGHTLEFT
		    എങ്കിൽ (revins_chars)
		    {
			revins_chars--;
			revins_legal++;
		    }
		    എങ്കിൽ (revins_on && gchar_cursor() == NUL)
			നിർത്തുക;
#endif
		}
		// Just a single backspace?:
		എങ്കിൽ (mode == BACKSPACE_CHAR)
		    നിർത്തുക;
	    } ആകുമ്പോൾ (
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
    എങ്കിൽ (curwin->w_cursor.col <= 1)
	did_ai = FALSE;

    എങ്കിൽ (call_fix_indent)
	fix_indent();

    /*
     * It's a little strange to put backspaces into the redo
     * buffer, but it makes auto-indent a lot easier to deal
     * with.
     */
    AppendCharToRedobuff(c);

    // If deleted before the insertion point, adjust it
    എങ്കിൽ (curwin->w_cursor.lnum == Insstart_orig.lnum
				  && curwin->w_cursor.col < Insstart_orig.col)
	Insstart_orig.col = curwin->w_cursor.col;

    // vi behaviour: the cursor moves backward but the character that
    //		     was there remains visible
    // Vim behaviour: the cursor moves backward and the character that
    //		      was there is erased from the screen.
    // We can emulate the vi behaviour by pretending there is a dollar
    // displayed even when there isn't.
    //  --pkv Sun Jan 19 01:56:40 EST 2003
    എങ്കിൽ (vim_strchr(p_cpo, CPO_BACKSPACE) != ശൂന്യ && dollar_vcol == -1)
	dollar_vcol = curwin->w_virtcol;

#ifdef FEAT_FOLDING
    // When deleting a char the cursor line must never be in a closed fold.
    // E.g., when 'foldmethod' is indent and deleting the first non-white
    // char before a Tab.
    എങ്കിൽ (did_backspace)
	foldOpenCursor();
#endif

    തിരികെ did_backspace;
}

/*
 * Handle receiving P_PS: start paste mode.  Inserts the following text up to
 * P_PE literally.
 * When "drop" is TRUE then consume the text and drop it.
 */
    പൂർണ്ണസംഖ്യ
bracketed_paste(paste_mode_T mode, പൂർണ്ണസംഖ്യ drop, garray_T *gap)
{
    പൂർണ്ണസംഖ്യ		c;
    char_u	buf[NUMBUFLEN + MB_MAXBYTES];
    പൂർണ്ണസംഖ്യ		idx = 0;
    char_u	*end = find_termcode((char_u *)"PE");
    പൂർണ്ണസംഖ്യ		ret_char = -1;
    പൂർണ്ണസംഖ്യ		save_allow_താക്കോലുകൾ = allow_താക്കോലുകൾ;
    പൂർണ്ണസംഖ്യ		save_paste = p_paste;

    // If the end code is too long we can't detect it, read everything.
    എങ്കിൽ (end != ശൂന്യ && STRLEN(end) >= NUMBUFLEN)
	end = ശൂന്യ;
    ++no_mapping;
    allow_താക്കോലുകൾ = 0;
    എങ്കിൽ (!p_paste)
	// Also have the side effects of setting 'paste' to make it work much
	// faster.
	set_option_മൂല്യം_give_err((char_u *)"paste", TRUE, ശൂന്യ, 0);

    വേണ്ടി (;;)
    {
	// When the end is not defined read everything there is.
	എങ്കിൽ (end == ശൂന്യ && vpeekc() == NUL)
	    നിർത്തുക;
	ചെയ്യുക
	    c = vgetc();
	ആകുമ്പോൾ (c == K_IGNORE || c == K_VER_SCROLLBAR || c == K_HOR_SCROLLBAR);

	എങ്കിൽ (c == NUL || got_int || (ex_normal_busy > 0 && c == Ctrl_C))
	    // When CTRL-C was encountered the typeahead will be flushed and we
	    // won't get the end sequence.  Except when using ":normal".
	    നിർത്തുക;

	എങ്കിൽ (has_mbyte)
	    idx += (*mb_char2bytes)(c, buf + idx);
	അല്ലെങ്കിൽ
	    buf[idx++] = c;
	buf[idx] = NUL;
	എങ്കിൽ (end != ശൂന്യ && STRNCMP(buf, end, idx) == 0)
	{
	    എങ്കിൽ (end[idx] == NUL)
		നിർത്തുക; // Found the end of paste code.
	    തുടരുക;
	}
	എങ്കിൽ (!drop)
	{
	    തിരഞ്ഞെടുപ്പ് (mode)
	    {
		അവസ്ഥ PASTE_CMDLINE:
		    put_on_cmdline(buf, idx, TRUE);
		    നിർത്തുക;

		അവസ്ഥ PASTE_EX:
		    // add one for the NUL that is going to be appended
		    എങ്കിൽ (gap != ശൂന്യ && ga_grow(gap, idx + 1) == OK)
		    {
			mch_memmove((അക്ഷരം *)gap->ga_data + gap->ga_len,
							     buf, (size_t)idx);
			gap->ga_len += idx;
		    }
		    നിർത്തുക;

		അവസ്ഥ PASTE_തിരുകുക:
		    എങ്കിൽ (stop_arrow() == OK)
		    {
			c = buf[0];
			എങ്കിൽ (idx == 1 && (c == CAR || c == K_KENTER || c == NL))
			    ins_eol(c);
			അല്ലെങ്കിൽ
			{
			    ins_char_bytes(buf, idx);
			    AppendToRedobuffLit(buf, idx);
			}
		    }
		    നിർത്തുക;

		അവസ്ഥ PASTE_ONE_CHAR:
		    എങ്കിൽ (ret_char == -1)
		    {
			എങ്കിൽ (has_mbyte)
			    ret_char = (*mb_ptr2char)(buf);
			അല്ലെങ്കിൽ
			    ret_char = buf[0];
		    }
		    നിർത്തുക;
	    }
	}
	idx = 0;
    }

    --no_mapping;
    allow_താക്കോലുകൾ = save_allow_താക്കോലുകൾ;
    എങ്കിൽ (!save_paste)
	set_option_മൂല്യം_give_err((char_u *)"paste", FALSE, ശൂന്യ, 0);

    തിരികെ ret_char;
}

#എങ്കിൽ defined(FEAT_GUI_TABLINE)
    സ്ഥിതം ശൂന്യം
ins_tabline(പൂർണ്ണസംഖ്യ c)
{
    // We will be leaving the current window, unless closing another tab.
    എങ്കിൽ (c != K_TABMENU || current_tabmenu != TABLINE_MENU_CLOSE
		|| (current_tab != 0 && current_tab != tabpage_index(curtab)))
    {
	undisplay_dollar();
	start_arrow(&curwin->w_cursor);
	can_cindent = TRUE;
    }

    എങ്കിൽ (c == K_TABLINE)
	goto_tabpage(current_tab);
    അല്ലെങ്കിൽ
    {
	handle_tabmenu();
	redraw_statuslines();	// will redraw the tabline when needed
    }
}
#endif

#എങ്കിൽ defined(FEAT_GUI)
    ശൂന്യം
ins_scroll(ശൂന്യം)
{
    pos_T	tpos;

    undisplay_dollar();
    tpos = curwin->w_cursor;
    എങ്കിൽ (gui_do_scroll())
    {
	start_arrow(&tpos);
	can_cindent = TRUE;
    }
}

    ശൂന്യം
ins_horscroll(ശൂന്യം)
{
    pos_T	tpos;

    undisplay_dollar();
    tpos = curwin->w_cursor;
    എങ്കിൽ (do_mousescroll_horiz(scrollbar_മൂല്യം))
    {
	start_arrow(&tpos);
	can_cindent = TRUE;
    }
}
#endif

    സ്ഥിതം ശൂന്യം
ins_ഇടത്(ശൂന്യം)
{
    pos_T	tpos;
    പൂർണ്ണസംഖ്യ		end_change = dont_sync_undo == FALSE; // end undoable change

#ifdef FEAT_FOLDING
    എങ്കിൽ ((fdo_flags & FDO_HOR) && താക്കോൽ_Typed)
	foldOpenCursor();
#endif
    undisplay_dollar();
    tpos = curwin->w_cursor;
    എങ്കിൽ (oneleft() == OK)
    {
#എങ്കിൽ defined(FEAT_XIM) && defined(FEAT_GUI_GTK)
	// Only call start_arrow() when not busy with preediting, it will
	// break undo.  K_LEFT is inserted in im_correct_cursor().
	എങ്കിൽ (p_imst == IM_OVER_THE_SPOT || !im_is_preediting())
#endif
	{
	    start_arrow_with_change(&tpos, end_change);
	    എങ്കിൽ (!end_change)
		AppendCharToRedobuff(K_ഇടത്);
	}
#ifdef FEAT_RIGHTLEFT
	// If exit reversed string, position is fixed
	എങ്കിൽ (revins_scol != -1 && (പൂർണ്ണസംഖ്യ)curwin->w_cursor.col >= revins_scol)
	    revins_legal++;
	revins_chars++;
#endif
    }

    /*
     * if 'whichwrap' set for cursor in insert mode may go to
     * previous line
     */
    അല്ലെങ്കിൽ എങ്കിൽ (vim_strchr(p_ww, '[') != ശൂന്യ && curwin->w_cursor.lnum > 1)
    {
	// always break undo when moving upwards/downwards, else undo may break
	start_arrow(&tpos);
	--(curwin->w_cursor.lnum);
	coladvance((colnr_T)MAXCOL);
	curwin->w_set_curswant = true;	// so we stay at the end
    }
    അല്ലെങ്കിൽ
	vim_beep(BO_CRSR);
    dont_sync_undo = FALSE;
}

    സ്ഥിതം ശൂന്യം
ins_home(പൂർണ്ണസംഖ്യ c)
{
    pos_T	tpos;

#ifdef FEAT_FOLDING
    എങ്കിൽ ((fdo_flags & FDO_HOR) && താക്കോൽ_Typed)
	foldOpenCursor();
#endif
    undisplay_dollar();
    tpos = curwin->w_cursor;
    എങ്കിൽ (c == K_C_HOME)
	curwin->w_cursor.lnum = 1;
    curwin->w_cursor.col = 0;
    curwin->w_cursor.coladd = 0;
    curwin->w_curswant = 0;
    start_arrow(&tpos);
}

    സ്ഥിതം ശൂന്യം
ins_end(പൂർണ്ണസംഖ്യ c)
{
    pos_T	tpos;

#ifdef FEAT_FOLDING
    എങ്കിൽ ((fdo_flags & FDO_HOR) && താക്കോൽ_Typed)
	foldOpenCursor();
#endif
    undisplay_dollar();
    tpos = curwin->w_cursor;
    എങ്കിൽ (c == K_C_END)
	curwin->w_cursor.lnum = curbuf->b_ml.ml_വരി_എണ്ണം;
    coladvance((colnr_T)MAXCOL);
    curwin->w_curswant = MAXCOL;

    start_arrow(&tpos);
}

    സ്ഥിതം ശൂന്യം
ins_s_ഇടത്(ശൂന്യം)
{
    പൂർണ്ണസംഖ്യ end_change = dont_sync_undo == FALSE; // end undoable change
#ifdef FEAT_FOLDING
    എങ്കിൽ ((fdo_flags & FDO_HOR) && താക്കോൽ_Typed)
	foldOpenCursor();
#endif
    undisplay_dollar();
    എങ്കിൽ (curwin->w_cursor.lnum > 1 || curwin->w_cursor.col > 0)
    {
	start_arrow_with_change(&curwin->w_cursor, end_change);
	എങ്കിൽ (!end_change)
	    AppendCharToRedobuff(K_S_ഇടത്);
	(ശൂന്യം)bck_word(1L, FALSE, FALSE);
	curwin->w_set_curswant = true;
    }
    അല്ലെങ്കിൽ
	vim_beep(BO_CRSR);
    dont_sync_undo = FALSE;
}

    സ്ഥിതം ശൂന്യം
ins_വലത്(ശൂന്യം)
{
    പൂർണ്ണസംഖ്യ end_change = dont_sync_undo == FALSE; // end undoable change

#ifdef FEAT_FOLDING
    എങ്കിൽ ((fdo_flags & FDO_HOR) && താക്കോൽ_Typed)
	foldOpenCursor();
#endif
    undisplay_dollar();
    എങ്കിൽ (gchar_cursor() != NUL || virtual_active())
    {
	start_arrow_with_change(&curwin->w_cursor, end_change);
	എങ്കിൽ (!end_change)
	    AppendCharToRedobuff(K_വലത്);
	curwin->w_set_curswant = true;
	എങ്കിൽ (virtual_active())
	    oneright();
	അല്ലെങ്കിൽ
	{
	    എങ്കിൽ (has_mbyte)
		curwin->w_cursor.col += (*mb_ptr2len)(ml_get_cursor());
	    അല്ലെങ്കിൽ
		++curwin->w_cursor.col;
	}

#ifdef FEAT_RIGHTLEFT
	revins_legal++;
	എങ്കിൽ (revins_chars)
	    revins_chars--;
#endif
    }
    // if 'whichwrap' set for cursor in insert mode, may move the
    // cursor to the next line
    അല്ലെങ്കിൽ എങ്കിൽ (vim_strchr(p_ww, ']') != ശൂന്യ
	    && curwin->w_cursor.lnum < curbuf->b_ml.ml_വരി_എണ്ണം)
    {
	start_arrow(&curwin->w_cursor);
	curwin->w_set_curswant = true;
	++curwin->w_cursor.lnum;
	curwin->w_cursor.col = 0;
    }
    അല്ലെങ്കിൽ
	vim_beep(BO_CRSR);
    dont_sync_undo = FALSE;
}

    സ്ഥിതം ശൂന്യം
ins_s_വലത്(ശൂന്യം)
{
    പൂർണ്ണസംഖ്യ end_change = dont_sync_undo == FALSE; // end undoable change
#ifdef FEAT_FOLDING
    എങ്കിൽ ((fdo_flags & FDO_HOR) && താക്കോൽ_Typed)
	foldOpenCursor();
#endif
    undisplay_dollar();
    എങ്കിൽ (curwin->w_cursor.lnum < curbuf->b_ml.ml_വരി_എണ്ണം
	    || gchar_cursor() != NUL)
    {
	start_arrow_with_change(&curwin->w_cursor, end_change);
	എങ്കിൽ (!end_change)
	    AppendCharToRedobuff(K_S_വലത്);
	(ശൂന്യം)fwd_word(1L, FALSE, 0);
	curwin->w_set_curswant = true;
    }
    അല്ലെങ്കിൽ
	vim_beep(BO_CRSR);
    dont_sync_undo = FALSE;
}

    സ്ഥിതം ശൂന്യം
ins_up(
    പൂർണ്ണസംഖ്യ		startcol)	// when TRUE move to Insstart.col
{
    pos_T	tpos;
    linenr_T	old_topline = curwin->w_topline;
#ifdef FEAT_DIFF
    പൂർണ്ണസംഖ്യ		old_topfill = curwin->w_topfill;
#endif

    undisplay_dollar();
    tpos = curwin->w_cursor;
    എങ്കിൽ (cursor_up(1L, TRUE) == OK)
    {
	എങ്കിൽ (startcol)
	    coladvance(getvcol_nolist(&Insstart));
	എങ്കിൽ (old_topline != curwin->w_topline
#ifdef FEAT_DIFF
		|| old_topfill != curwin->w_topfill
#endif
		)
	    redraw_later(UPD_VALID);
	start_arrow(&tpos);
	can_cindent = TRUE;
    }
    അല്ലെങ്കിൽ
	vim_beep(BO_CRSR);
}

    സ്ഥിതം ശൂന്യം
ins_pageup(ശൂന്യം)
{
    pos_T	tpos;

    undisplay_dollar();

    എങ്കിൽ (mod_mask & MOD_MASK_CTRL)
    {
	// <C-PageUp>: tab page back
	എങ്കിൽ (first_tabpage->tp_next != ശൂന്യ)
	{
	    start_arrow(&curwin->w_cursor);
	    goto_tabpage(-1);
	}
	തിരികെ;
    }

    tpos = curwin->w_cursor;
    എങ്കിൽ (pagescroll(BACKWARD, 1L, FALSE) == OK)
    {
	start_arrow(&tpos);
	can_cindent = TRUE;
    }
    അല്ലെങ്കിൽ
	vim_beep(BO_CRSR);
}

    സ്ഥിതം ശൂന്യം
ins_down(
    പൂർണ്ണസംഖ്യ		startcol)	// when TRUE move to Insstart.col
{
    pos_T	tpos;
    linenr_T	old_topline = curwin->w_topline;
#ifdef FEAT_DIFF
    പൂർണ്ണസംഖ്യ		old_topfill = curwin->w_topfill;
#endif

    undisplay_dollar();
    tpos = curwin->w_cursor;
    എങ്കിൽ (cursor_down(1L, TRUE) == OK)
    {
	എങ്കിൽ (startcol)
	    coladvance(getvcol_nolist(&Insstart));
	എങ്കിൽ (old_topline != curwin->w_topline
#ifdef FEAT_DIFF
		|| old_topfill != curwin->w_topfill
#endif
		)
	    redraw_later(UPD_VALID);
	start_arrow(&tpos);
	can_cindent = TRUE;
    }
    അല്ലെങ്കിൽ
	vim_beep(BO_CRSR);
}

    സ്ഥിതം ശൂന്യം
ins_pagedown(ശൂന്യം)
{
    pos_T	tpos;

    undisplay_dollar();

    എങ്കിൽ (mod_mask & MOD_MASK_CTRL)
    {
	// <C-PageDown>: tab page forward
	എങ്കിൽ (first_tabpage->tp_next != ശൂന്യ)
	{
	    start_arrow(&curwin->w_cursor);
	    goto_tabpage(0);
	}
	തിരികെ;
    }

    tpos = curwin->w_cursor;
    എങ്കിൽ (pagescroll(FORWARD, 1L, FALSE) == OK)
    {
	start_arrow(&tpos);
	can_cindent = TRUE;
    }
    അല്ലെങ്കിൽ
	vim_beep(BO_CRSR);
}

#ifdef FEAT_DND
    സ്ഥിതം ശൂന്യം
ins_drop(ശൂന്യം)
{
    do_put('~', ശൂന്യ, BACKWARD, 1L, PUT_CURSEND);
}
#endif

/*
 * Handle TAB in Insert or Replace mode.
 * Return TRUE when the TAB needs to be inserted like a normal character.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
ins_tab(ശൂന്യം)
{
    പൂർണ്ണസംഖ്യ		ind;
    പൂർണ്ണസംഖ്യ		i;
    പൂർണ്ണസംഖ്യ		temp;

    എങ്കിൽ (Insstart_blank_vcol == MAXCOL && curwin->w_cursor.lnum == Insstart.lnum)
	Insstart_blank_vcol = get_nolist_virtcol();
    എങ്കിൽ (echeck_abbr(TAB + ABBR_OFF))
	തിരികെ FALSE;

    ind = inindent(0);
    എങ്കിൽ (ind)
	can_cindent = FALSE;

    /*
     * When nothing special, insert TAB like a normal character.
     */
    എങ്കിൽ (!curbuf->b_p_et
#ifdef FEAT_VARTABS
	    && !(p_sta && ind
		// These five lines mean 'tabstop' != 'shiftwidth'
		&& ((tabstop_എണ്ണം(curbuf->b_p_vts_array) > 1)
		    || (tabstop_എണ്ണം(curbuf->b_p_vts_array) == 1
			&& tabstop_first(curbuf->b_p_vts_array)
						       != get_sw_മൂല്യം(curbuf))
		    || (tabstop_എണ്ണം(curbuf->b_p_vts_array) == 0
			&& curbuf->b_p_ts != get_sw_മൂല്യം(curbuf))))
	    && tabstop_എണ്ണം(curbuf->b_p_vsts_array) == 0
#അല്ലെങ്കിൽ
	    && !(p_sta && ind && curbuf->b_p_ts != get_sw_മൂല്യം(curbuf))
#endif
	    && get_sts_മൂല്യം() == 0)
	തിരികെ TRUE;

    എങ്കിൽ (stop_arrow() == FAIL)
	തിരികെ TRUE;

    did_ai = FALSE;
    did_si = FALSE;
    can_si = FALSE;
    can_si_back = FALSE;
    AppendToRedobuff((char_u *)"\t");

#ifdef FEAT_VARTABS
    എങ്കിൽ (p_sta && ind)		// insert tab in indent, use 'shiftwidth'
    {
	temp = (പൂർണ്ണസംഖ്യ)get_sw_മൂല്യം(curbuf);
	temp -= get_nolist_virtcol() % temp;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (tabstop_എണ്ണം(curbuf->b_p_vsts_array) > 0 || curbuf->b_p_sts != 0)
				// use 'softtabstop' when set
	temp = tabstop_padding(get_nolist_virtcol(), get_sts_മൂല്യം(),
						     curbuf->b_p_vsts_array);
    അല്ലെങ്കിൽ			// otherwise use 'tabstop'
	temp = tabstop_padding(get_nolist_virtcol(), curbuf->b_p_ts,
						     curbuf->b_p_vts_array);
#അല്ലെങ്കിൽ
    എങ്കിൽ (p_sta && ind)		// insert tab in indent, use 'shiftwidth'
	temp = (പൂർണ്ണസംഖ്യ)get_sw_മൂല്യം(curbuf);
    അല്ലെങ്കിൽ എങ്കിൽ (curbuf->b_p_sts != 0) // use 'softtabstop' when set
	temp = (പൂർണ്ണസംഖ്യ)get_sts_മൂല്യം();
    അല്ലെങ്കിൽ			// otherwise use 'tabstop'
	temp = (പൂർണ്ണസംഖ്യ)curbuf->b_p_ts;
    temp -= get_nolist_virtcol() % temp;
#endif

    /*
     * Insert the first space with ins_char().	It will delete one char in
     * replace mode.  Insert the rest with ins_str(); it will not delete any
     * chars.  For MODE_VREPLACE state, we use ins_char() for all characters.
     */
    ins_char(' ');
    ആകുമ്പോൾ (--temp > 0)
    {
	എങ്കിൽ (State & VREPLACE_FLAG)
	    ins_char(' ');
	അല്ലെങ്കിൽ
	{
	    ins_str((char_u *)" ", 1);
	    എങ്കിൽ (State & REPLACE_FLAG)	    // no char replaced
		replace_തള്ളുക(NUL);
	}
    }

    /*
     * When 'expandtab' not set: Replace spaces by TABs where possible.
     */
#ifdef FEAT_VARTABS
    എങ്കിൽ (!curbuf->b_p_et && (tabstop_എണ്ണം(curbuf->b_p_vsts_array) > 0
			    || get_sts_മൂല്യം() > 0
			    || (p_sta && ind)))
#അല്ലെങ്കിൽ
    എങ്കിൽ (!curbuf->b_p_et && (get_sts_മൂല്യം() || (p_sta && ind)))
#endif
    {
	char_u		*ptr;
	char_u		*saved_വരി = ശൂന്യ;	// init for GCC
	pos_T		pos;
	pos_T		fpos;
	pos_T		*cursor;
	colnr_T		want_vcol, vcol;
	പൂർണ്ണസംഖ്യ		change_col = -1;
	പൂർണ്ണസംഖ്യ		save_list = curwin->w_p_list;
	char_u		*tab = (char_u *)"\t";
	chartabsize_T	cts;

	/*
	 * Get the current line.  For MODE_VREPLACE state, don't make real
	 * changes yet, just work on a copy of the line.
	 */
	എങ്കിൽ (State & VREPLACE_FLAG)
	{
	    pos = curwin->w_cursor;
	    cursor = &pos;
	    saved_വരി = vim_strnsave(ml_get_curline(), ml_get_curline_len());
	    എങ്കിൽ (saved_വരി == ശൂന്യ)
		തിരികെ FALSE;
	    ptr = saved_വരി + pos.col;
	}
	അല്ലെങ്കിൽ
	{
	    ptr = ml_get_cursor();
	    cursor = &curwin->w_cursor;
	}

	// When 'L' is not in 'cpoptions' a tab always takes up 'ts' spaces.
	എങ്കിൽ (vim_strchr(p_cpo, CPO_LISTWM) == ശൂന്യ)
	    curwin->w_p_list = FALSE;

	// Find first white before the cursor
	fpos = curwin->w_cursor;
	ആകുമ്പോൾ (fpos.col > 0 && VIM_ISWHITE(ptr[-1]))
	{
	    --fpos.col;
	    --ptr;
	}

	// In Replace mode, don't change characters before the insert point.
	എങ്കിൽ ((State & REPLACE_FLAG)
		&& fpos.lnum == Insstart.lnum
		&& fpos.col < Insstart.col)
	{
	    ptr += Insstart.col - fpos.col;
	    fpos.col = Insstart.col;
	}

	// compute virtual column numbers of first white and cursor
	getvcol(curwin, &fpos, &vcol, ശൂന്യ, ശൂന്യ, 0);
	getvcol(curwin, cursor, &want_vcol, ശൂന്യ, ശൂന്യ, 0);

	init_chartabsize_arg(&cts, curwin, 0, vcol, tab, tab);

	// Use as many TABs as possible.  Beware of 'breakindent', 'showbreak'
	// and 'linebreak' adding extra virtual columns.
	ആകുമ്പോൾ (VIM_ISWHITE(*ptr))
	{
	    i = lbr_chartabsize(&cts);
	    എങ്കിൽ (cts.cts_vcol + i > want_vcol)
		നിർത്തുക;
	    എങ്കിൽ (*ptr != TAB)
	    {
		*ptr = TAB;
		എങ്കിൽ (change_col < 0)
		{
		    change_col = fpos.col;  // Column of first change
		    // May have to adjust Insstart
		    എങ്കിൽ (fpos.lnum == Insstart.lnum && fpos.col < Insstart.col)
			Insstart.col = fpos.col;
		}
	    }
	    ++fpos.col;
	    ++ptr;
	    cts.cts_vcol += i;
	}
	vcol = cts.cts_vcol;
	clear_chartabsize_arg(&cts);

	എങ്കിൽ (change_col >= 0)
	{
	    പൂർണ്ണസംഖ്യ		    repl_off = 0;

	    // Skip over the spaces we need.
	    init_chartabsize_arg(&cts, curwin, 0, vcol, ptr, ptr);
	    ആകുമ്പോൾ (cts.cts_vcol < want_vcol && *cts.cts_ptr == ' ')
	    {
		cts.cts_vcol += lbr_chartabsize(&cts);
		++cts.cts_ptr;
		++repl_off;
	    }
	    ptr = cts.cts_ptr;
	    vcol = cts.cts_vcol;
	    clear_chartabsize_arg(&cts);

	    എങ്കിൽ (vcol > want_vcol)
	    {
		// Must have a char with 'showbreak' just before it.
		--ptr;
		--repl_off;
	    }
	    fpos.col += repl_off;

	    // Delete following spaces.
	    i = cursor->col - fpos.col;
	    എങ്കിൽ (i > 0)
	    {
#ifdef FEAT_PROP_POPUP
		എങ്കിൽ (!(State & VREPLACE_FLAG))
		{
		    char_u  *newp;
		    പൂർണ്ണസംഖ്യ	    col;

		    newp = alloc(curbuf->b_ml.ml_വരി_len - i);
		    എങ്കിൽ (newp == ശൂന്യ)
			തിരികെ FALSE;

		    col = ptr - curbuf->b_ml.ml_വരി_ptr;
		    എങ്കിൽ (col > 0)
			mch_memmove(newp, ptr - col, col);
		    mch_memmove(newp + col, ptr + i,
					   curbuf->b_ml.ml_വരി_len - col - i);

		    എങ്കിൽ (curbuf->b_ml.ml_flags & (ML_വരി_DIRTY | ML_ALLOCATED))
			vim_free(curbuf->b_ml.ml_വരി_ptr);
		    curbuf->b_ml.ml_വരി_ptr = newp;
		    curbuf->b_ml.ml_വരി_len -= i;
		    curbuf->b_ml.ml_വരി_textlen = 0;
		    curbuf->b_ml.ml_flags =
			   (curbuf->b_ml.ml_flags | ML_വരി_DIRTY) & ~ML_EMPTY;
		}
		അല്ലെങ്കിൽ
#endif
		    STRMOVE(ptr, ptr + i);
		// correct replace stack.
		എങ്കിൽ ((State & REPLACE_FLAG) && !(State & VREPLACE_FLAG))
		    വേണ്ടി (temp = i; --temp >= 0; )
			replace_join(repl_off);
	    }
#ifdef FEAT_NETBEANS_INTG
	    എങ്കിൽ (netbeans_active())
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
	    എങ്കിൽ (State & VREPLACE_FLAG)
	    {
		// Backspace from real cursor to change_col
		backspace_until_column(change_col);

		// Insert each char in saved_line from changed_col to
		// ptr-cursor
		ins_bytes_len(saved_വരി + change_col,
						    cursor->col - change_col);
	    }
	}

	എങ്കിൽ (State & VREPLACE_FLAG)
	    vim_free(saved_വരി);
	curwin->w_p_list = save_list;
    }

    തിരികെ FALSE;
}

/*
 * Handle CR or NL in insert mode.
 * Return FAIL when out of memory or can't undo.
 */
    പൂർണ്ണസംഖ്യ
ins_eol(പൂർണ്ണസംഖ്യ c)
{
    പൂർണ്ണസംഖ്യ	    i;

    എങ്കിൽ (echeck_abbr(c + ABBR_OFF))
	തിരികെ OK;
    എങ്കിൽ (stop_arrow() == FAIL)
	തിരികെ FAIL;
    undisplay_dollar();

    /*
     * Strange Vi behaviour: In Replace mode, typing a NL will not delete the
     * character under the cursor.  Only push a NUL on the replace stack,
     * nothing to put back when the NL is deleted.
     */
    എങ്കിൽ ((State & REPLACE_FLAG) && !(State & VREPLACE_FLAG))
	replace_തള്ളുക(NUL);

    /*
     * In MODE_VREPLACE state, a NL replaces the rest of the line, and starts
     * replacing the next line, so we push all of the characters left on the
     * line onto the replace stack.  This is not done here though, it is done
     * in open_line().
     */

    // Put cursor on NUL if on the last char and coladd is 1 (happens after
    // CTRL-O).
    എങ്കിൽ (virtual_active() && curwin->w_cursor.coladd > 0)
	coladvance(getviscol());

#ifdef FEAT_RIGHTLEFT
    // NL in reverse insert will always start in the end of
    // current line.
    എങ്കിൽ (revins_on)
	curwin->w_cursor.col += ml_get_cursor_len();
#endif

    AppendToRedobuff(NL_STR);
    i = open_വരി(FORWARD,
	    has_format_option(FO_RET_COMS) ? OPENLINE_DO_COM : 0, old_indent,
	    ശൂന്യ);
    old_indent = 0;
    can_cindent = TRUE;
#ifdef FEAT_FOLDING
    // When inserting a line the cursor line must never be in a closed fold.
    foldOpenCursor();
#endif

    തിരികെ i;
}

#ifdef FEAT_DIGRAPHS
/*
 * Handle digraph in insert mode.
 * Returns character still to be inserted, or NUL when nothing remaining to be
 * done.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
ins_digraph(ശൂന്യം)
{
    പൂർണ്ണസംഖ്യ	    c;
    പൂർണ്ണസംഖ്യ	    cc;
    പൂർണ്ണസംഖ്യ	    did_putchar = FALSE;

    pc_status = PC_STATUS_UNSET;
    എങ്കിൽ (redrawing() && !char_avail())
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
    ++allow_താക്കോലുകൾ;
    c = plain_vgetc();
    --no_mapping;
    --allow_താക്കോലുകൾ;
    എങ്കിൽ (did_putchar)
	// when the line fits in 'columns' the '?' is at the start of the next
	// line and will not be removed by the redraw
	edit_unputchar();

    എങ്കിൽ (IS_SPECIAL(c) || mod_mask)	    // special key
    {
	clear_showcmd();
	തിരുകുക_special(c, TRUE, FALSE);
	തിരികെ NUL;
    }
    എങ്കിൽ (c != ESC)
    {
	did_putchar = FALSE;
	എങ്കിൽ (redrawing() && !char_avail())
	{
	    // may need to redraw when no more chars available now
	    ins_redraw(FALSE);

	    എങ്കിൽ (char2cells(c) == 1)
	    {
		ins_redraw(FALSE);
		edit_putchar(c, TRUE);
		did_putchar = TRUE;
	    }
	    add_to_showcmd_c(c);
	}
	++no_mapping;
	++allow_താക്കോലുകൾ;
	cc = plain_vgetc();
	--no_mapping;
	--allow_താക്കോലുകൾ;
	എങ്കിൽ (did_putchar)
	    // when the line fits in 'columns' the '?' is at the start of the
	    // next line and will not be removed by a redraw
	    edit_unputchar();
	എങ്കിൽ (cc != ESC)
	{
	    AppendToRedobuff((char_u *)CTRL_V_STR);
	    c = digraph_get(c, cc, TRUE);
	    clear_showcmd();
	    തിരികെ c;
	}
    }
    clear_showcmd();
    തിരികെ NUL;
}
#endif

/*
 * Handle CTRL-E and CTRL-Y in Insert mode: copy char from other line.
 * Returns the char to be inserted, or NUL if none found.
 */
    പൂർണ്ണസംഖ്യ
ins_copychar(linenr_T lnum)
{
    പൂർണ്ണസംഖ്യ		    c;
    char_u	    *ptr, *prev_ptr;
    char_u	    *വരി;
    chartabsize_T   cts;

    എങ്കിൽ (lnum < 1 || lnum > curbuf->b_ml.ml_വരി_എണ്ണം)
    {
	vim_beep(BO_COPY);
	തിരികെ NUL;
    }

    // try to advance to the cursor column
    validate_virtcol();
    വരി = ml_get(lnum);
    prev_ptr = വരി;
    init_chartabsize_arg(&cts, curwin, lnum, 0, വരി, വരി);
    ആകുമ്പോൾ (cts.cts_vcol < curwin->w_virtcol && *cts.cts_ptr != NUL)
    {
	prev_ptr = cts.cts_ptr;
	cts.cts_vcol += lbr_chartabsize_adv(&cts);
    }
    എങ്കിൽ (cts.cts_vcol > curwin->w_virtcol)
	ptr = prev_ptr;
    അല്ലെങ്കിൽ
	ptr = cts.cts_ptr;
    clear_chartabsize_arg(&cts);

    c = (*mb_ptr2char)(ptr);
    എങ്കിൽ (c == NUL)
	vim_beep(BO_COPY);
    തിരികെ c;
}

/*
 * CTRL-Y or CTRL-E typed in Insert mode.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
ins_ctrl_ey(പൂർണ്ണസംഖ്യ tc)
{
    പൂർണ്ണസംഖ്യ	    c = tc;

    എങ്കിൽ (ctrl_x_mode_scroll())
    {
	എങ്കിൽ (c == Ctrl_Y)
	    scrolldown_clamp();
	അല്ലെങ്കിൽ
	    scrollup_clamp();
	redraw_later(UPD_VALID);
    }
    അല്ലെങ്കിൽ
    {
	c = ins_copychar(curwin->w_cursor.lnum + (c == Ctrl_Y ? -1 : 1));
	എങ്കിൽ (c != NUL)
	{
	    long	tw_save;

	    // The character must be taken literally, insert like it
	    // was typed after a CTRL-V, and pretend 'textwidth'
	    // wasn't set.  Digits, 'o' and 'x' are special after a
	    // CTRL-V, don't use it for these.
	    എങ്കിൽ (c < 256 && !SAFE_isalnum(c))
		AppendToRedobuff((char_u *)CTRL_V_STR);	// CTRL-V
	    tw_save = curbuf->b_p_tw;
	    curbuf->b_p_tw = -1;
	    തിരുകുക_special(c, TRUE, FALSE);
	    curbuf->b_p_tw = tw_save;
#ifdef FEAT_RIGHTLEFT
	    revins_chars++;
	    revins_legal++;
#endif
	    c = Ctrl_V;	// pretend CTRL-V is last character
	    auto_format(FALSE, TRUE);
	}
    }
    തിരികെ c;
}

/*
 * Get the value that w_virtcol would have when 'list' is off.
 * Unless 'cpo' contains the 'L' flag.
 */
    colnr_T
get_nolist_virtcol(ശൂന്യം)
{
    // check validity of cursor in current buffer
    എങ്കിൽ (curwin->w_buffer == ശൂന്യ
	|| curwin->w_buffer->b_ml.ml_mfp == ശൂന്യ
	|| curwin->w_cursor.lnum > curwin->w_buffer->b_ml.ml_വരി_എണ്ണം)
	തിരികെ 0;
    എങ്കിൽ (curwin->w_p_list && vim_strchr(p_cpo, CPO_LISTWM) == ശൂന്യ)
	തിരികെ getvcol_nolist(&curwin->w_cursor);
    validate_virtcol();
    തിരികെ curwin->w_virtcol;
}

#എങ്കിൽ defined(FEAT_EVAL)
/*
 * Handle the InsertCharPre autocommand.
 * "c" is the character that was typed.
 * Return a pointer to allocated memory with the replacement string.
 * Return NULL to continue inserting "c".
 */
    സ്ഥിതം char_u *
do_തിരുകുക_char_pre(പൂർണ്ണസംഖ്യ c)
{
    char_u	*res;
    char_u	buf[MB_MAXBYTES + 1];
    size_t	buflen;
    പൂർണ്ണസംഖ്യ		save_State = State;

    // Return quickly when there is nothing to do.
    എങ്കിൽ (!has_insertcharpre())
	തിരികെ ശൂന്യ;

    എങ്കിൽ (c == Ctrl_RSB)
	തിരികെ ശൂന്യ;

    എങ്കിൽ (has_mbyte)
	buflen = (*mb_char2bytes)(c, buf);
    അല്ലെങ്കിൽ
    {
	buf[0] = c;
	buflen = 1;
    }
    buf[buflen] = NUL;

    // Lock the text to avoid weird things from happening.
    ++textlock;
    set_vim_var_string(VV_CHAR, buf, (പൂർണ്ണസംഖ്യ)buflen);  // set v:char

    res = ശൂന്യ;
    എങ്കിൽ (ins_apply_autocmds(EVENT_INSERTCHARPRE))
    {
	// Get the value of v:char.  It may be empty or more than one
	// character.  Only use it when changed, otherwise continue with the
	// original character to avoid breaking autoindent.
	എങ്കിൽ (STRCMP(buf, get_vim_var_str(VV_CHAR)) != 0)
	    res = vim_strsave(get_vim_var_str(VV_CHAR));
    }

    set_vim_var_string(VV_CHAR, ശൂന്യ, -1);  // clear v:char
    --textlock;

    // Restore the State, it may have been changed.
    State = save_State;

    തിരികെ res;
}
#endif

    പൂർണ്ണസംഖ്യ
get_can_cindent(ശൂന്യം)
{
    തിരികെ can_cindent;
}

    ശൂന്യം
set_can_cindent(പൂർണ്ണസംഖ്യ val)
{
    can_cindent = val;
}

/*
 * Trigger "event" and take care of fixing undo.
 */
    പൂർണ്ണസംഖ്യ
ins_apply_autocmds(event_T event)
{
    varnumber_T	tick = CHANGEDTICK(curbuf);
    പൂർണ്ണസംഖ്യ r;

    r = apply_autocmds(event, ശൂന്യ, ശൂന്യ, FALSE, curbuf);

    // If u_savesub() was called then we are not prepared to start
    // a new line.  Call u_save() with no contents to fix that.
    // Except when leaving Insert mode.
    എങ്കിൽ (event != EVENT_INSERTLEAVE && tick != CHANGEDTICK(curbuf))
	u_save(curwin->w_cursor.lnum, (linenr_T)(curwin->w_cursor.lnum + 1));

    തിരികെ r;
}
