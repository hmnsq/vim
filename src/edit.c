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

#અન્તર્ભાવ "vim.h"

#define BACKSPACE_CHAR		    1
#define BACKSPACE_WORD		    2
#define BACKSPACE_WORD_NOT_SPACE    3
#define BACKSPACE_લીટી		    4

// Set when doing something for completion that may call edit() recursively,
// which is not allowed.
સ્થિર પૂર્ણાંક	compl_busy = FALSE;


સ્થિર શૂન્ય ins_ctrl_v(શૂન્ય);
સ્થિર શૂન્ય ઉમેરો_special(પૂર્ણાંક, પૂર્ણાંક, પૂર્ણાંક);
સ્થિર શૂન્ય redo_literal(પૂર્ણાંક c);
સ્થિર શૂન્ય start_arrow_common(pos_T *end_ઉમેરો_pos, પૂર્ણાંક change);
#ifdef FEAT_SPELL
સ્થિર શૂન્ય check_spell_redraw(શૂન્ય);
#endif
સ્થિર શૂન્ય stop_ઉમેરો(pos_T *end_ઉમેરો_pos, પૂર્ણાંક esc, પૂર્ણાંક nomove);
સ્થિર પૂર્ણાંક  echeck_abbr(પૂર્ણાંક);
સ્થિર શૂન્ય mb_replace_કાઢો_ins(પૂર્ણાંક cc);
સ્થિર શૂન્ય replace_flush(શૂન્ય);
સ્થિર શૂન્ય replace_do_bs(પૂર્ણાંક limit_col);
સ્થિર પૂર્ણાંક del_char_after_col(પૂર્ણાંક limit_col);
સ્થિર શૂન્ય ins_reg(શૂન્ય);
સ્થિર શૂન્ય ins_ctrl_g(શૂન્ય);
સ્થિર શૂન્ય ins_ctrl_hat(શૂન્ય);
સ્થિર પૂર્ણાંક  ins_esc(long *ગણતરી, પૂર્ણાંક cmdchar, પૂર્ણાંક nomove);
#ifdef FEAT_RIGHTLEFT
સ્થિર શૂન્ય ins_ctrl_(શૂન્ય);
#endif
સ્થિર પૂર્ણાંક ins_start_select(પૂર્ણાંક c);
સ્થિર શૂન્ય ins_ઉમેરો(પૂર્ણાંક replaceState);
સ્થિર શૂન્ય ins_ctrl_o(શૂન્ય);
સ્થિર શૂન્ય ins_shift(પૂર્ણાંક c, પૂર્ણાંક lastc);
સ્થિર શૂન્ય ins_del(શૂન્ય);
સ્થિર પૂર્ણાંક  ins_bs(પૂર્ણાંક c, પૂર્ણાંક mode, પૂર્ણાંક *inserted_space_p);
#જો defined(FEAT_GUI_TABLINE)
સ્થિર શૂન્ય ins_tabline(પૂર્ણાંક c);
#endif
સ્થિર શૂન્ય ins_ડાબું(શૂન્ય);
સ્થિર શૂન્ય ins_home(પૂર્ણાંક c);
સ્થિર શૂન્ય ins_end(પૂર્ણાંક c);
સ્થિર શૂન્ય ins_s_ડાબું(શૂન્ય);
સ્થિર શૂન્ય ins_જમણું(શૂન્ય);
સ્થિર શૂન્ય ins_s_જમણું(શૂન્ય);
સ્થિર શૂન્ય ins_up(પૂર્ણાંક startcol);
સ્થિર શૂન્ય ins_pageup(શૂન્ય);
સ્થિર શૂન્ય ins_down(પૂર્ણાંક startcol);
સ્થિર શૂન્ય ins_pagedown(શૂન્ય);
#ifdef FEAT_DND
સ્થિર શૂન્ય ins_drop(શૂન્ય);
#endif
સ્થિર પૂર્ણાંક  ins_tab(શૂન્ય);
#ifdef FEAT_DIGRAPHS
સ્થિર પૂર્ણાંક  ins_digraph(શૂન્ય);
#endif
સ્થિર પૂર્ણાંક  ins_ctrl_ey(પૂર્ણાંક tc);
#જો defined(FEAT_EVAL)
સ્થિર char_u *do_ઉમેરો_char_pre(પૂર્ણાંક c);
#endif

સ્થિર colnr_T	Insstart_textlen;	// length of line when insert started
સ્થિર colnr_T	Insstart_blank_vcol;	// vcol for first inserted blank
સ્થિર પૂર્ણાંક	update_Insstart_orig = TRUE; // set Insstart_orig to Insstart

સ્થિર string_T	last_ઉમેરો = {ખાલી, 0};    // the text of the previous insert,
					    // K_SPECIAL and CSI are escaped
સ્થિર પૂર્ણાંક	last_ઉમેરો_skip; // nr of chars in front of previous insert
સ્થિર પૂર્ણાંક	new_ઉમેરો_skip;  // nr of chars in front of current insert
સ્થિર પૂર્ણાંક	did_restart_edit;	// "restart_edit" when calling edit()

સ્થિર પૂર્ણાંક	can_cindent;		// may do cindenting on this line

#ifdef FEAT_RIGHTLEFT
સ્થિર પૂર્ણાંક	revins_on;		// reverse insert mode on
સ્થિર પૂર્ણાંક	revins_chars;		// how much to skip after edit
સ્થિર પૂર્ણાંક	revins_legal;		// was the last char 'legal'?
સ્થિર પૂર્ણાંક	revins_scol;		// start column of revins session
#endif

સ્થિર પૂર્ણાંક	ins_need_undo;		// call u_save() before inserting a
					// char.  Set when edit() is called.
					// after that arrow_used is used.

સ્થિર પૂર્ણાંક	dont_sync_undo = FALSE;	// CTRL-G U prevents syncing undo for
					// the next left/right cursor key

// With 'autocompletedelay' set, arm the delay and let the main loop fire
// Insert-mode autocommands; the popup is shown later on K_COMPLETE_DELAY.
// Otherwise trigger completion right away.
#define TRIGGER_AUTOCOMPLETE()			\
    કરો {					\
	update_screen(UPD_VALID);  /* Show char (deletion) immediately */ \
	out_flush();				\
	ins_compl_enable_autocomplete();	\
	ins_compl_arm_autostart();		\
	જો (!ins_compl_arm_autocomplete_delay())\
	    જાઓ docomplete;			\
    } જ્યારે (0)

#define MAY_TRIGGER_AUTOCOMPLETE(c)				\
    કરો {							\
	જો (ins_compl_has_autocomplete() && !char_avail()	\
		&& curwin->w_cursor.col > 0)			\
	{							\
	    (c) = char_before_cursor();				\
	    જો (vim_isprintc(c))				\
		TRIGGER_AUTOCOMPLETE();				\
	}							\
    } જ્યારે (0)

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
    પૂર્ણાંક
edit(
    પૂર્ણાંક		cmdchar,
    પૂર્ણાંક		startln,	// if set, insert at start of line
    long	ગણતરી)
{
    પૂર્ણાંક		c = 0;
    char_u	*ptr;
    પૂર્ણાંક		lastc = 0;
    પૂર્ણાંક		mincol;
    સ્થિર linenr_T o_lnum = 0;
    પૂર્ણાંક		i;
    પૂર્ણાંક		did_backspace = TRUE;	    // previous char was backspace
    પૂર્ણાંક		લીટી_is_white = FALSE;	    // line is empty before insert
    linenr_T	old_topline = 0;	    // topline before insertion
#ifdef FEAT_DIFF
    પૂર્ણાંક		old_topfill = -1;
#endif
    પૂર્ણાંક		inserted_space = FALSE;     // just inserted a space
    પૂર્ણાંક		replaceState = MODE_REPLACE;
    પૂર્ણાંક		nomove = FALSE;		    // don't move cursor on return
#ifdef FEAT_JOB_CHANNEL
    પૂર્ણાંક		cmdchar_todo = cmdchar;
#endif
#ifdef FEAT_CONCEAL
    પૂર્ણાંક		cursor_લીટી_was_concealed;
#endif
    પૂર્ણાંક		ins_just_started = TRUE;

    // Remember whether editing was restarted after CTRL-O.
    did_restart_edit = restart_edit;

    // sleep before redrawing, needed for "CTRL-O :" that results in an
    // error message
    check_for_delay(TRUE);

    // set Insstart_orig to Insstart
    update_Insstart_orig = TRUE;

#ifdef HAVE_SANDBOX
    // Don't allow inserting in the sandbox.
    જો (sandbox != 0)
    {
	emsg(_(e_not_allowed_in_sandbox));
	પાછા FALSE;
    }
#endif
    // Don't allow changes in the buffer while editing the cmdline.  The
    // caller of getcmdline() may get confused.
    // Don't allow recursive insert mode when busy with completion.
    જો (textlock != 0 || ins_compl_active() || compl_busy || pum_visible())
    {
	emsg(_(e_not_allowed_to_change_text_or_change_window));
	પાછા FALSE;
    }
    ins_compl_clear();	    // clear stuff for CTRL-X mode

    /*
     * Trigger InsertEnter autocommands.  Do not do this for "r<CR>" or "grx".
     */
    જો (cmdchar != 'r' && cmdchar != 'v')
    {
	pos_T   save_cursor = curwin->w_cursor;

#ifdef FEAT_EVAL
	જો (cmdchar == 'R')
	    ptr = (char_u *)"r";
	નહીંતર જો (cmdchar == 'V')
	    ptr = (char_u *)"v";
	નહીંતર
	    ptr = (char_u *)"i";
	set_vim_var_string(VV_INSERTMODE, ptr, 1);
	set_vim_var_string(VV_CHAR, ખાલી, -1);  // clear v:char
#endif
	ins_apply_autocmds(EVENT_INSERTENTER);

	// Check for changed highlighting, e.g. for ModeMsg.
	જો (need_highlight_changed)
	    highlight_changed();

	// Make sure the cursor didn't move.  Do call check_cursor_col() in
	// case the text was modified.  Since Insert mode was not started yet
	// a call to check_cursor_col() may move the cursor, especially with
	// the "A" command, thus set State to avoid that. Also check that the
	// line number is still valid (lines may have been deleted).
	// Do not restore if v:char was set to a non-empty string.
	જો (!EQUAL_POS(curwin->w_cursor, save_cursor)
#ifdef FEAT_EVAL
		&& *get_vim_var_str(VV_CHAR) == NUL
#endif
		&& save_cursor.lnum <= curbuf->b_ml.ml_લીટી_ગણતરી)
	{
	    પૂર્ણાંક save_state = State;

	    curwin->w_cursor = save_cursor;
	    State = MODE_ઉમેરો;
	    check_cursor_col();
	    State = save_state;
	}
    }

#ifdef FEAT_CONCEAL
    // Check if the cursor line was concealed before changing State.
    cursor_લીટી_was_concealed = curwin->w_p_cole > 0
						&& conceal_cursor_લીટી(curwin);
#endif

    /*
     * When doing a paste with the middle mouse button, Insstart is set to
     * where the paste started.
     */
    જો (where_paste_started.lnum != 0)
	Insstart = where_paste_started;
    નહીંતર
    {
	Insstart = curwin->w_cursor;
	જો (startln)
	    Insstart.col = 0;
    }
    Insstart_textlen = (colnr_T)linetabsize_str(ml_get_curline());
    Insstart_blank_vcol = MAXCOL;
    જો (!did_ai)
	ai_col = 0;

    જો (cmdchar != NUL && restart_edit == 0)
    {
	ResetRedobuff();
	Append_સંખ્યા_To_Redobuff(ગણતરી);
	જો (cmdchar == 'V' || cmdchar == 'v')
	{
	    // "gR" or "gr" command
	    AppendCharToRedobuff('g');
	    AppendCharToRedobuff((cmdchar == 'v') ? 'r' : 'R');
	}
	નહીંતર
	{
	    જો (cmdchar == K_PS)
		AppendCharToRedobuff('a');
	    નહીંતર
		AppendCharToRedobuff(cmdchar);
	    જો (cmdchar == 'g')		    // "gI" command
		AppendCharToRedobuff('I');
	    નહીંતર જો (cmdchar == 'r')	    // "r<CR>" command
		ગણતરી = 1;		    // insert only one <CR>
	}
    }

    જો (cmdchar == 'R')
    {
	State = MODE_REPLACE;
    }
    નહીંતર જો (cmdchar == 'V' || cmdchar == 'v')
    {
	State = MODE_VREPLACE;
	replaceState = MODE_VREPLACE;
	orig_લીટી_ગણતરી = curbuf->b_ml.ml_લીટી_ગણતરી;
	vr_લીટીઓ_changed = 1;
    }
    નહીંતર
	State = MODE_ઉમેરો;

    may_trigger_modechanged();
    stop_ઉમેરો_mode = FALSE;

#ifdef FEAT_CONCEAL
    // Check if the cursor line needs redrawing after changing State.  If
    // 'concealcursor' is "n" it needs to be redrawn without concealing.
    conceal_check_cursor_લીટી(cursor_લીટી_was_concealed);
#endif

    // Need to position cursor again when on a TAB and when on a char with
    // virtual text.
    જો (gchar_cursor() == TAB
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
    જો (curbuf->b_p_iminsert == B_IMODE_LMAP)
	State |= MODE_LANGMAP;
#ifdef HAVE_આગત_METHOD
    im_set_active(curbuf->b_p_iminsert == B_IMODE_IM);
#endif

    setmouse();
    clear_showcmd();
#ifdef FEAT_RIGHTLEFT
    // there is no reverse replace mode
    revins_on = (State == MODE_ઉમેરો && p_ri);
    જો (revins_on)
	undisplay_dollar();
    revins_chars = 0;
    revins_legal = 0;
    revins_scol = -1;
#endif
    જો (!p_ek)
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
    જો (restart_edit != 0 && stuff_empty())
    {
	/*
	 * After a paste we consider text typed to be part of the insert for
	 * the pasted text. You can backspace over the pasted text too.
	 */
	જો (where_paste_started.lnum)
	    arrow_used = FALSE;
	નહીંતર
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
	જો (((ins_at_eol && curwin->w_cursor.lnum == o_lnum)
		    || curwin->w_curswant > curwin->w_virtcol)
		&& *(ptr = ml_get_curline() + curwin->w_cursor.col) != NUL)
	{
	    જો (ptr[1] == NUL)
		++curwin->w_cursor.col;
	    નહીંતર જો (has_mbyte)
	    {
		i = (*mb_ptr2len)(ptr);
		જો (ptr[i] == NUL)
		    curwin->w_cursor.col += i;
	    }
	}
	ins_at_eol = FALSE;
    }
    નહીંતર
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
    જો (!p_im && did_restart_edit == 0)
	foldOpenCursor();
#endif

    /*
     * If 'showmode' is set, show the current (insert/replace/..) mode.
     * A warning message for changing a readonly file is given here, before
     * actually changing anything.  It's put after the mode, if any.
     */
    i = 0;
    જો (p_smd && સંદેશ_silent == 0)
	i = showmode();

    જો (!p_im && did_restart_edit == 0)
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
    new_ઉમેરો_skip = (પૂર્ણાંક)inserted.length;
    જો (inserted.string != ખાલી)
	vim_free(inserted.string);

    old_indent = 0;

    /*
     * Main loop in Insert mode: repeat until Insert mode is left.
     */
    માટે (;;)
    {
#ifdef FEAT_RIGHTLEFT
	જો (!revins_legal)
	    revins_scol = -1;	    // reset on illegal motions
	નહીંતર
	    revins_legal = 0;
#endif
	જો (arrow_used)	    // don't repeat insert when arrow key used
	    ગણતરી = 0;

	જો (update_Insstart_orig)
	    Insstart_orig = Insstart;

	જો (stop_ઉમેરો_mode && !ins_compl_active())
	{
	    // ":stopinsert" used or 'insertmode' reset
	    ગણતરી = 0;
	    જાઓ doESCkey;
	}

	// set curwin->w_curswant for next K_DOWN or K_UP
	જો (!arrow_used)
	    curwin->w_set_curswant = true;

	// If there is no typeahead may check for timestamps (e.g., for when a
	// menu invoked a shell command).
	જો (stuff_empty())
	{
	    did_check_timestamps = FALSE;
	    જો (need_check_timestamps)
		check_timestamps(FALSE);
	}

	/*
	 * When emsg() was called msg_scroll will have been set.
	 */
	સંદેશ_scroll = FALSE;

#ifdef FEAT_GUI
	// When 'mousefocus' is set a mouse movement may have taken us to
	// another window.  "need_mouse_correct" may then be set because of an
	// autocommand.
	જો (need_mouse_correct)
	    gui_mouse_correct();
#endif

#ifdef FEAT_FOLDING
	// Open fold at the cursor line, according to 'foldopen'.
	જો (fdo_flags & FDO_ઉમેરો)
	    foldOpenCursor();
	// Close folds where the cursor isn't, according to 'foldclose'
	જો (!char_avail())
	    foldCheckClose();
#endif

#ifdef FEAT_JOB_CHANNEL
	જો (bt_prompt(curbuf))
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
	જો (curbuf->b_mod_set
		&& curwin->w_p_wrap
		&& !curwin->w_p_sms
		&& !did_backspace
		&& curwin->w_topline == old_topline
#ifdef FEAT_DIFF
		&& curwin->w_topfill == old_topfill
#endif
		&& ગણતરી <= 1
		)
	{
	    mincol = curwin->w_wcol;
	    validate_cursor_col();

	    જો (
#ifdef FEAT_VARTABS
		curwin->w_wcol < mincol - tabstop_at(get_nolist_virtcol(),
						     curbuf->b_p_ts,
						     curbuf->b_p_vts_array,
						     FALSE)
#નહીંતર
		(પૂર્ણાંક)curwin->w_wcol < mincol - curbuf->b_p_ts
#endif
		    && curwin->w_wrow ==
				   curwin->w_height - 1 - get_scrolloff_કિંમત()
		    && (curwin->w_cursor.lnum != curwin->w_topline
#ifdef FEAT_DIFF
			|| curwin->w_topfill > 0
#endif
		    ))
	    {
#ifdef FEAT_DIFF
		જો (curwin->w_topfill > 0)
		    --curwin->w_topfill;
		નહીંતર
#endif
#ifdef FEAT_FOLDING
		જો (hasFolding(curwin->w_topline, ખાલી, &old_topline))
		    set_topline(curwin, old_topline + 1);
		નહીંતર
#endif
		    set_topline(curwin, curwin->w_topline + 1);
	    }
	}

	// May need to adjust w_topline to show the cursor.
	જો (ગણતરી <= 1)
	    update_topline();

	did_backspace = FALSE;

	જો (ગણતરી <= 1)
	    validate_cursor();		// may set must_redraw

	/*
	 * Redraw the display when no characters are waiting.
	 * Also shows mode, ruler and positions cursor.
	 */
	ins_redraw(TRUE);

	જો (curwin->w_p_scb)
	    do_check_scrollbind(TRUE);

	જો (curwin->w_p_crb)
	    do_check_cursorbind();
	જો (ગણતરી <= 1)
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
	જો (c != K_CURSORHOLD && c != K_COMPLETE_DELAY)
	    lastc = c;		// remember the previous char for CTRL-D

	// After using CTRL-G U the next cursor key will not break undo.
	જો (dont_sync_undo == MAYBE)
	    dont_sync_undo = TRUE;
	નહીંતર
	    dont_sync_undo = FALSE;
	જો (cmdchar == K_PS)
	    // Got here from normal mode when bracketed paste started.
	    c = K_PS;
	નહીંતર
	{
	    // Trigger autocomplete when entering Insert mode, either directly
	    // or via change commands like 'ciw', 'cw', etc., before the first
	    // character is typed.
	    જો (ins_just_started)
	    {
		ins_just_started = FALSE;
		જો (ins_compl_has_autocomplete() && !char_avail()
			&& curwin->w_cursor.col > 0)
		{
		    c = char_before_cursor();
		    જો (vim_isprintc(c))
		    {
			ins_compl_enable_autocomplete();
			ins_compl_arm_autostart();
			ins_compl_init_get_longest();
#ifdef FEAT_RIGHTLEFT
			જો (p_hkmap)
			    c = hkmap(c);		// Hebrew mode mapping
#endif
			// Defer until the delay expires (K_COMPLETE_DELAY), or
			// trigger now when no delay is in effect.
			જો (!ins_compl_arm_autocomplete_delay())
			    જાઓ docomplete;
		    }
		}
	    }

	    કરો
	    {
		c = safe_vgetc();

		જો (stop_ઉમેરો_mode
#ifdef FEAT_TERMINAL
			|| (c == K_IGNORE && term_use_loop())
#endif
		   )
		{
		    // Insert mode ended, possibly from a callback, or a timer
		    // must have opened a terminal window.
		    જો (c != K_IGNORE && c != K_NOP)
			vungetc(c);
		    ગણતરી = 0;

		    જો (!bt_prompt(curwin->w_buffer)
#ifdef FEAT_TERMINAL
			    && !bt_terminal(curwin->w_buffer)
#endif
			    && stop_ઉમેરો_mode)
			// :stopinsert command via callback or via server command
			nomove = FALSE;
		    નહીંતર
			nomove = TRUE;
		    ins_compl_prep(ESC);
		    જાઓ doESCkey;
		}
	    } જ્યારે (c == K_IGNORE || c == K_NOP);
	}

	// Don't want K_CURSORHOLD for the second key, e.g., after CTRL-V.
	did_cursorhold = TRUE;
	જો (c != K_CURSORHOLD && c != K_COMPLETE_DELAY)
	{
	    // Don't want delayed autocompletion from the previous key either.
	    ins_compl_clear_autocomplete_delay();
	    ins_compl_disarm_autostart();
	    // A completion already on screen goes on being what it was.
	    જો (!ins_compl_active())
		ins_compl_disable_autocomplete();
	}

#ifdef FEAT_RIGHTLEFT
	જો (p_hkmap && ચાવી_Typed)
	    c = hkmap(c);		// Hebrew mode mapping
#endif

	// If the window was made so small that nothing shows, make it at least
	// one line and one column when typing.
	જો (ચાવી_Typed && !ચાવી_Stuffed)
	    win_ensure_size();

	/*
	 * Special handling of keys while the popup menu is visible or wanted
	 * and the cursor is still in the completed word.  Only when there is
	 * a match, skip this when no matches were found.
	 */
	જો (ins_compl_active() && curwin->w_cursor.col >= ins_compl_col()
		&& ins_compl_has_shown_match() && pum_wanted())
	{
	    // BS: Delete one character from "compl_leader".
	    જો ((c == K_BS || c == Ctrl_H)
			&& curwin->w_cursor.col > ins_compl_col()
			&& (c = ins_compl_bs()) == NUL)
		ચાલુ;

	    // When no match was selected or it was edited.
	    જો (!ins_compl_used_match())
	    {
		// CTRL-L: Add one character from the current match to
		// "compl_leader".  Except when at the original match and
		// there is nothing to add, CTRL-L works like CTRL-P then.
		જો (c == Ctrl_L
			&& (!ctrl_x_mode_લીટી_or_eval()
			    || ins_compl_long_shown_match()))
		{
		    ins_compl_addfrommatch();
		    ચાલુ;
		}

		// A non-white character that fits in with the current
		// completion: Add to "compl_leader".
		જો (ins_compl_accept_char(c))
		{
#જો defined(FEAT_EVAL)
		    // Trigger InsertCharPre.
		    char_u *str = do_ઉમેરો_char_pre(c);
		    char_u *p;

		    જો (str != ખાલી)
		    {
			માટે (p = str; *p != NUL; MB_PTR_ADV(p))
			    ins_compl_addleader(PTR2CHAR(p));
			vim_free(str);
		    }
		    નહીંતર
#endif
			ins_compl_addleader(c);
		    ચાલુ;
		}

		// Pressing CTRL-Y selects the current match.  When
		// ins_compl_enter_selects() is set the Enter key does the
		// same.
		જો ((c == Ctrl_Y || (ins_compl_enter_selects()
				    && (c == CAR || c == K_KENTER || c == NL)))
			&& stop_arrow() == OK)
		{
		    ins_compl_delete();
		    જો (ins_compl_preinsert_longest()
			    && !ins_compl_is_match_selected())
		    {
			ins_compl_ઉમેરો(FALSE, TRUE);
			ins_compl_init_get_longest();
			ચાલુ;
		    }
		    નહીંતર
			ins_compl_ઉમેરો(FALSE, FALSE);
		}
		// Delete preinserted text when typing special chars
		નહીંતર જો (IS_WHITE_NL_OR_NUL(c) && ins_compl_preinsert_effect())
		    ins_compl_delete();
	    }
	}

	// Prepare for or stop CTRL-X mode.  This doesn't do completion, but
	// it does fix up the text when finishing completion.
	ins_compl_init_get_longest();
	જો (ins_compl_prep(c))
	    ચાલુ;

	// CTRL-\ CTRL-N goes to Normal mode,
	// CTRL-\ CTRL-G goes to mode selected with 'insertmode',
	// CTRL-\ CTRL-O is like CTRL-O but without moving the cursor.
	જો (c == Ctrl_BSL)
	{
	    // may need to redraw when no more chars available now
	    ins_redraw(FALSE);
	    ++no_mapping;
	    ++allow_ચાવીઓ;
	    c = plain_vgetc();
	    --no_mapping;
	    --allow_ચાવીઓ;
	    જો (c != Ctrl_N && c != Ctrl_G && c != Ctrl_O)
	    {
		// it's something else
		vungetc(c);
		c = Ctrl_BSL;
	    }
	    નહીંતર જો (c == Ctrl_G && p_im)
		ચાલુ;
	    નહીંતર
	    {
		જો (c == Ctrl_O)
		{
		    ins_ctrl_o();
		    ins_at_eol = FALSE;	// cursor keeps its column
		    nomove = TRUE;
		}
		ગણતરી = 0;
		જાઓ doESCkey;
	    }
	}

#ifdef FEAT_DIGRAPHS
	c = do_digraph(c);
#endif

	જો ((c == Ctrl_V || c == Ctrl_Q) && ctrl_x_mode_cmdline())
	    જાઓ docomplete;
	જો (c == Ctrl_V || c == Ctrl_Q)
	{
	    ins_ctrl_v();
	    c = Ctrl_V;	// pretend CTRL-V is last typed character
	    ચાલુ;
	}

	જો (cindent_on() && ctrl_x_mode_none())
	{
	    // A key name preceded by a bang means this key is not to be
	    // inserted.  Skip ahead to the re-indenting below.
	    // A key name preceded by a star means that indenting has to be
	    // done before inserting the key.
	    લીટી_is_white = inindent(0);
	    જો (in_cinkeys(c, '!', લીટી_is_white))
		જાઓ force_cindent;
	    જો (can_cindent && in_cinkeys(c, '*', લીટી_is_white)
							&& stop_arrow() == OK)
		do_c_પદાવલિ_indent();
	}

#ifdef FEAT_RIGHTLEFT
	જો (curwin->w_p_rl)
	    પસંદગી (c)
	    {
		સ્થિતિ K_ડાબું:	c = K_જમણું; તોડો;
		સ્થિતિ K_S_ડાબું:	c = K_S_જમણું; તોડો;
		સ્થિતિ K_C_ડાબું:	c = K_C_જમણું; તોડો;
		સ્થિતિ K_જમણું:	c = K_ડાબું; તોડો;
		સ્થિતિ K_S_જમણું: c = K_S_ડાબું; તોડો;
		સ્થિતિ K_C_જમણું: c = K_C_ડાબું; તોડો;
	    }
#endif

	/*
	 * If 'keymodel' contains "startsel", may start selection.  If it
	 * does, a CTRL-O and c will be stuffed, we need to get these
	 * characters.
	 */
	જો (ins_start_select(c))
	    ચાલુ;

	/*
	 * The big switch to handle a character in insert mode.
	 */
	પસંદગી (c)
	{
	સ્થિતિ ESC:	// End input mode
	    જો (echeck_abbr(ESC + ABBR_OFF))
		તોડો;
	    // FALLTHROUGH

	સ્થિતિ Ctrl_C:	// End input mode
	    જો (c == Ctrl_C && cmdwin_type != 0)
	    {
		// Close the cmdline window.
		cmdwin_result = K_IGNORE;
		got_int = FALSE; // don't stop executing autocommands et al.
		nomove = TRUE;
		જાઓ doESCkey;
	    }
#ifdef FEAT_JOB_CHANNEL
	    જો (c == Ctrl_C && bt_prompt(curbuf))
	    {
		જો (invoke_prompt_interrupt())
		{
		    જો (!bt_prompt(curbuf))
			// buffer changed to a non-prompt buffer, get out of
			// Insert mode
			જાઓ doESCkey;
		    તોડો;
		}
	    }
#endif

#ifdef UNIX
do_intr:
#endif
	    // when 'insertmode' set, and not halfway a mapping, don't leave
	    // Insert mode
	    જો (goto_im())
	    {
		જો (got_int)
		{
		    (શૂન્ય)vgetc();		// flush all buffers
		    got_int = FALSE;
		}
		નહીંતર
		    vim_beep(BO_IM);
		તોડો;
	    }
doESCkey:
	    // Drop a pending autocomplete so it does not outlive Insert mode.
	    ins_compl_clear_autocomplete_delay();
	    /*
	     * This is the ONLY return from edit()!
	     */
	    // Always update o_lnum, so that a "CTRL-O ." that adds a line
	    // still puts the cursor back after the inserted text.
	    જો (ins_at_eol && gchar_cursor() == NUL)
		o_lnum = curwin->w_cursor.lnum;

	    જો (ins_esc(&ગણતરી, cmdchar, nomove))
	    {
		// When CTRL-C was typed got_int will be set, with the result
		// that the autocommands won't be executed. When mapped got_int
		// is not set, but let's keep the behavior the same.
		જો (cmdchar != 'r' && cmdchar != 'v' && c != Ctrl_C)
		    ins_apply_autocmds(EVENT_INSERTLEAVE);
		did_cursorhold = FALSE;

		// ins_redraw() triggers TextChangedI only when no characters
		// are in the typeahead buffer, so reset curbuf->b_last_changedtick only
		// if the TextChangedI was not blocked by char_avail() (e.g. using :norm!)
		// and the TextChangedI autocommand has been triggered.
		જો (!char_avail() && curbuf->b_last_changedtick_i == CHANGEDTICK(curbuf))
		    curbuf->b_last_changedtick = CHANGEDTICK(curbuf);
		પાછા (c == Ctrl_O);
	    }
	    ચાલુ;

	સ્થિતિ Ctrl_Z:	// suspend when 'insertmode' set
	    જો (!p_im)
		જાઓ normalchar;	// insert CTRL-Z as normal char
	    do_cmdline_cmd((char_u *)"stop");
#ifdef CURSOR_SHAPE
	    ui_cursor_shape();		// may need to update cursor shape
#endif
	    ચાલુ;

	સ્થિતિ Ctrl_O:	// execute one command
#ifdef FEAT_COMPL_FUNC
	    જો (ctrl_x_mode_omni())
		જાઓ docomplete;
#endif
	    જો (echeck_abbr(Ctrl_O + ABBR_OFF))
		તોડો;
	    ins_ctrl_o();

	    // don't move the cursor left when 'virtualedit' has "onemore".
	    જો (get_ve_flags() & VE_ONEMORE)
	    {
		ins_at_eol = FALSE;
		nomove = TRUE;
	    }
	    ગણતરી = 0;
	    જાઓ doESCkey;

	સ્થિતિ K_INS:	// toggle insert/replace mode
	સ્થિતિ K_KINS:
	    ins_ઉમેરો(replaceState);
	    તોડો;

	સ્થિતિ K_SELECT:	// end of Select mode mapping - ignore
	    તોડો;

	સ્થિતિ K_HELP:	// Help key works like <ESC> <Help>
	સ્થિતિ K_F1:
	સ્થિતિ K_XF1:
	    stuffcharReadbuff(K_HELP);
	    જો (p_im)
		need_start_insertmode = TRUE;
	    જાઓ doESCkey;

#ifdef FEAT_NETBEANS_INTG
	સ્થિતિ K_F21:	// NetBeans command
	    ++no_mapping;		// don't map the next key hits
	    i = plain_vgetc();
	    --no_mapping;
	    netbeans_keycommand(i);
	    તોડો;
#endif

	સ્થિતિ K_ZERO:	// Insert the previously inserted text.
	સ્થિતિ NUL:
	સ્થિતિ Ctrl_A:
	    // For ^@ the trailing ESC will end the insert, unless there is an
	    // error.
	    જો (stuff_inserted(NUL, 1L, (c == Ctrl_A)) == FAIL
						   && c != Ctrl_A && !p_im)
		જાઓ doESCkey;		// quit insert mode
	    inserted_space = FALSE;
	    તોડો;

	સ્થિતિ Ctrl_R:	// insert the contents of a register
	    જો (ctrl_x_mode_register() && !ins_compl_active())
		જાઓ docomplete;
	    ins_reg();
	    auto_format(FALSE, TRUE);
	    inserted_space = FALSE;
	    તોડો;

	સ્થિતિ Ctrl_G:	// commands starting with CTRL-G
	    ins_ctrl_g();
	    તોડો;

	સ્થિતિ Ctrl_HAT:	// switch input mode and/or langmap
	    ins_ctrl_hat();
	    તોડો;

#ifdef FEAT_RIGHTLEFT
	સ્થિતિ Ctrl__:	// switch between languages
	    જો (!p_ari)
		જાઓ normalchar;
	    ins_ctrl_();
	    તોડો;
#endif

	સ્થિતિ Ctrl_D:	// Make indent one shiftwidth smaller.
#જો defined(FEAT_FIND_ID)
	    જો (ctrl_x_mode_path_defines())
		જાઓ docomplete;
#endif
	    // FALLTHROUGH

	સ્થિતિ Ctrl_T:	// Make indent one shiftwidth greater.
	    જો (c == Ctrl_T && ctrl_x_mode_thesaurus())
	    {
		જો (has_compl_option(FALSE))
		    જાઓ docomplete;
		તોડો;
	    }

	    ins_shift(c, lastc);
	    auto_format(FALSE, TRUE);
	    inserted_space = FALSE;
	    તોડો;

	સ્થિતિ K_DEL:	// delete character under the cursor
	સ્થિતિ K_KDEL:
	    ins_del();
	    auto_format(FALSE, TRUE);
	    તોડો;

	સ્થિતિ K_BS:	// delete character before the cursor
	સ્થિતિ K_S_BS:
	સ્થિતિ Ctrl_H:
	    did_backspace = ins_bs(c, BACKSPACE_CHAR, &inserted_space);
	    auto_format(FALSE, TRUE);
	    જો (did_backspace)
		MAY_TRIGGER_AUTOCOMPLETE(c);
	    તોડો;

	સ્થિતિ Ctrl_W:	// delete word before the cursor
#ifdef FEAT_JOB_CHANNEL
	    જો (bt_prompt(curbuf) && (mod_mask & MOD_MASK_SHIFT) == 0)
	    {
		// In a prompt window CTRL-W is used for window commands.
		// Use Shift-CTRL-W to delete a word.
		stuffcharReadbuff(Ctrl_W);
		restart_edit = 'A';
		nomove = TRUE;
		ગણતરી = 0;
		જાઓ doESCkey;
	    }
#endif
	    did_backspace = ins_bs(c, BACKSPACE_WORD, &inserted_space);
	    auto_format(FALSE, TRUE);
	    જો (did_backspace)
		MAY_TRIGGER_AUTOCOMPLETE(c);
	    તોડો;

	સ્થિતિ Ctrl_U:	// delete all inserted text in current line
#ifdef FEAT_COMPL_FUNC
	    // CTRL-X CTRL-U completes with 'completefunc'.
	    જો (ctrl_x_mode_function())
		જાઓ docomplete;
#endif
	    did_backspace = ins_bs(c, BACKSPACE_લીટી, &inserted_space);
	    auto_format(FALSE, TRUE);
	    inserted_space = FALSE;
	    જો (did_backspace)
		MAY_TRIGGER_AUTOCOMPLETE(c);
	    તોડો;

	સ્થિતિ K_LEFTMOUSE:   // mouse keys
	સ્થિતિ K_LEFTMOUSE_NM:
	સ્થિતિ K_LEFTDRAG:
	સ્થિતિ K_LEFTRELEASE:
	સ્થિતિ K_LEFTRELEASE_NM:
	સ્થિતિ K_MOUSEMOVE:
	સ્થિતિ K_MIDDLEMOUSE:
	સ્થિતિ K_MIDDLEDRAG:
	સ્થિતિ K_MIDDLERELEASE:
	સ્થિતિ K_RIGHTMOUSE:
	સ્થિતિ K_RIGHTDRAG:
	સ્થિતિ K_RIGHTRELEASE:
	સ્થિતિ K_X1MOUSE:
	સ્થિતિ K_X1DRAG:
	સ્થિતિ K_X1RELEASE:
	સ્થિતિ K_X2MOUSE:
	સ્થિતિ K_X2DRAG:
	સ્થિતિ K_X2RELEASE:
	    ins_mouse(c);
	    તોડો;

	સ્થિતિ K_MOUSEDOWN: // Default action for scroll wheel up: scroll up
	    ins_mousescroll(MSCR_DOWN);
	    તોડો;

	સ્થિતિ K_MOUSEUP:	// Default action for scroll wheel down: scroll down
	    ins_mousescroll(MSCR_UP);
	    તોડો;

	સ્થિતિ K_MOUSELEFT: // Scroll wheel left
	    ins_mousescroll(MSCR_ડાબું);
	    તોડો;

	સ્થિતિ K_MOUSERIGHT: // Scroll wheel right
	    ins_mousescroll(MSCR_જમણું);
	    તોડો;

	સ્થિતિ K_PS:
	    bracketed_paste(PASTE_ઉમેરો, FALSE, ખાલી);
	    જો (cmdchar == K_PS)
		// invoked from normal mode, bail out
		જાઓ doESCkey;
	    તોડો;
	સ્થિતિ K_PE:
	    // Got K_PE without K_PS, ignore.
	    તોડો;

#ifdef FEAT_GUI_TABLINE
	સ્થિતિ K_TABLINE:
	સ્થિતિ K_TABMENU:
	    ins_tabline(c);
	    તોડો;
#endif

	સ્થિતિ K_IGNORE:	// Something mapped to nothing
	    તોડો;

	સ્થિતિ K_COMMAND:		    // <Cmd>command<CR>
	સ્થિતિ K_SCRIPT_COMMAND:	    // <ScriptCmd>command<CR>
	    {
		bufref_T    save_curbuf;
		varnumber_T tick = CHANGEDTICK(curbuf);

		set_bufref(&save_curbuf, curbuf);
		do_cmdkey_command(c, 0);

#ifdef FEAT_TERMINAL
		જો (term_use_loop())
		    // Started a terminal that gets the input, exit Insert mode.
		    જાઓ doESCkey;
#endif
		જો (curbuf->b_u_synced
			|| (bufref_valid(&save_curbuf)
			    && curbuf == save_curbuf.br_buf
			    && tick != CHANGEDTICK(curbuf)))
		{
		    // The command synced undo or changed this buffer.
		    // Save the cursor line before the next typed edit.
		    ins_need_undo = TRUE;
		}
	    }
	    તોડો;

	સ્થિતિ K_CURSORHOLD:	// Didn't type something for a while.
	    ins_apply_autocmds(EVENT_CURSORHOLDI);
	    did_cursorhold = TRUE;
	    // If CTRL-G U was used apply it to the next typed key.
	    જો (dont_sync_undo == TRUE)
		dont_sync_undo = MAYBE;
	    તોડો;

	સ્થિતિ K_COMPLETE_DELAY:	// 'autocompletedelay' expired
	    // If CTRL-G U was used apply it to the next typed key.
	    જો (dont_sync_undo == TRUE)
		dont_sync_undo = MAYBE;
	    ins_compl_clear_autocomplete_delay();
	    જો (!ins_compl_has_autocomplete() || char_avail()
		    || curwin->w_cursor.col == 0)
		તોડો;
	    c = char_before_cursor();
	    જો (!vim_isprintc(c))
		તોડો;
	    // The completion may have been cleared while waiting, so re-enable
	    // autocomplete to match a zero delay.
	    ins_compl_enable_autocomplete();
	    ins_compl_arm_autostart();
	    જાઓ docomplete;

#ifdef FEAT_GUI_MSWIN
	    // On MS-Windows ignore <M-F4>, we get it when closing the window
	    // was cancelled.
	સ્થિતિ K_F4:
	    જો (mod_mask != MOD_MASK_ALT)
		જાઓ normalchar;
	    તોડો;
#endif

#ifdef FEAT_GUI
	સ્થિતિ K_VER_SCROLLBAR:
	    ins_scroll();
	    તોડો;

	સ્થિતિ K_HOR_SCROLLBAR:
	    ins_horscroll();
	    તોડો;
#endif

	સ્થિતિ K_HOME:	// <Home>
	સ્થિતિ K_KHOME:
	સ્થિતિ K_S_HOME:
	સ્થિતિ K_C_HOME:
	    ins_home(c);
	    તોડો;

	સ્થિતિ K_END:	// <End>
	સ્થિતિ K_KEND:
	સ્થિતિ K_S_END:
	સ્થિતિ K_C_END:
	    ins_end(c);
	    તોડો;

	સ્થિતિ K_ડાબું:	// <Left>
	    જો (mod_mask & (MOD_MASK_SHIFT|MOD_MASK_CTRL))
		ins_s_ડાબું();
	    નહીંતર
		ins_ડાબું();
	    તોડો;

	સ્થિતિ K_S_ડાબું:	// <S-Left>
	સ્થિતિ K_C_ડાબું:
	    ins_s_ડાબું();
	    તોડો;

	સ્થિતિ K_જમણું:	// <Right>
	    જો (mod_mask & (MOD_MASK_SHIFT|MOD_MASK_CTRL))
		ins_s_જમણું();
	    નહીંતર
		ins_જમણું();
	    તોડો;

	સ્થિતિ K_S_જમણું:	// <S-Right>
	સ્થિતિ K_C_જમણું:
	    ins_s_જમણું();
	    તોડો;

	સ્થિતિ K_UP:	// <Up>
	    જો (pum_visible())
		જાઓ docomplete;
	    જો (mod_mask & MOD_MASK_SHIFT)
		ins_pageup();
	    નહીંતર
		ins_up(FALSE);
	    તોડો;

	સ્થિતિ K_S_UP:	// <S-Up>
	સ્થિતિ K_PAGEUP:
	સ્થિતિ K_KPAGEUP:
	    જો (pum_visible())
	    {
#ifdef FEAT_PROP_POPUP
		// CTRL-SHIFT-<Up> scrolls the info popup up a line,
		// CTRL-SHIFT-<PageUp> a page.  Shift is folded into K_S_UP but
		// stays in mod_mask for PageUp, hence the asymmetric check.
		જો (c == K_S_UP ? (mod_mask & MOD_MASK_CTRL)
			: ((mod_mask & MOD_MASK_CTRL)
			    && (mod_mask & MOD_MASK_SHIFT)))
		{
		    popup_scroll_info(-1, c != K_S_UP);
		    તોડો;
		}
#endif
		જાઓ docomplete;
	    }
	    ins_pageup();
	    તોડો;

	સ્થિતિ K_DOWN:	// <Down>
	    જો (pum_visible())
		જાઓ docomplete;
	    જો (mod_mask & MOD_MASK_SHIFT)
		ins_pagedown();
	    નહીંતર
		ins_down(FALSE);
	    તોડો;

	સ્થિતિ K_S_DOWN:	// <S-Down>
	સ્થિતિ K_PAGEDOWN:
	સ્થિતિ K_KPAGEDOWN:
	    જો (pum_visible())
	    {
#ifdef FEAT_PROP_POPUP
		// CTRL-SHIFT-<Down>/<PageDown> scroll the info popup down.
		જો (c == K_S_DOWN ? (mod_mask & MOD_MASK_CTRL)
			: ((mod_mask & MOD_MASK_CTRL)
			    && (mod_mask & MOD_MASK_SHIFT)))
		{
		    popup_scroll_info(1, c != K_S_DOWN);
		    તોડો;
		}
#endif
		જાઓ docomplete;
	    }
	    ins_pagedown();
	    તોડો;

#ifdef FEAT_DND
	સ્થિતિ K_DROP:	// drag-n-drop event
	    ins_drop();
	    તોડો;
#endif

	સ્થિતિ K_S_TAB:	// When not mapped, use like a normal TAB
	    c = TAB;
	    // FALLTHROUGH

	સ્થિતિ TAB:	// TAB or Complete patterns along path
#જો defined(FEAT_FIND_ID)
	    જો (ctrl_x_mode_path_patterns())
		જાઓ docomplete;
#endif
	    inserted_space = FALSE;
	    જો (ins_tab())
		જાઓ normalchar;	// insert TAB as a normal char
	    auto_format(FALSE, TRUE);
	    તોડો;

	સ્થિતિ K_KENTER:	// <Enter>
	    c = CAR;
	    // FALLTHROUGH
	સ્થિતિ CAR:
	સ્થિતિ NL:
#જો defined(FEAT_QUICKFIX)
	    // In a quickfix window a <CR> jumps to the error under the
	    // cursor.
	    જો (bt_quickfix(curbuf) && c == CAR)
	    {
		જો (curwin->w_llist_ref == ખાલી)    // quickfix window
		    do_cmdline_cmd((char_u *)".cc");
		નહીંતર				    // location list window
		    do_cmdline_cmd((char_u *)".ll");
		તોડો;
	    }
#endif
	    જો (cmdwin_type != 0)
	    {
		// Execute the command in the cmdline window.
		cmdwin_result = CAR;
		જાઓ doESCkey;
	    }
#ifdef FEAT_JOB_CHANNEL
	    જો (bt_prompt(curbuf))
	    {
		invoke_prompt_callback();
		જો (!bt_prompt(curbuf))
		    // buffer changed to a non-prompt buffer, get out of
		    // Insert mode
		    જાઓ doESCkey;
		તોડો;
	    }
#endif
	    જો (ins_eol(c) == FAIL && !p_im)
		જાઓ doESCkey;	    // out of memory
	    auto_format(FALSE, FALSE);
	    inserted_space = FALSE;
	    તોડો;

	સ્થિતિ Ctrl_K:	    // digraph or keyword completion
	    જો (ctrl_x_mode_dictionary())
	    {
		જો (has_compl_option(TRUE))
		    જાઓ docomplete;
		તોડો;
	    }
#ifdef FEAT_DIGRAPHS
	    c = ins_digraph();
	    જો (c == NUL)
		તોડો;
#endif
	    જાઓ normalchar;

	સ્થિતિ Ctrl_X:	// Enter CTRL-X mode
	    ins_ctrl_x();
	    તોડો;

	સ્થિતિ Ctrl_RSB:	// Tag name completion after ^X
	    જો (!ctrl_x_mode_tags())
		જાઓ normalchar;
	    જાઓ docomplete;

	સ્થિતિ Ctrl_F:	// File name completion after ^X
	    જો (!ctrl_x_mode_files())
		જાઓ normalchar;
	    જાઓ docomplete;

	સ્થિતિ 's':	// Spelling completion after ^X
	સ્થિતિ Ctrl_S:
	    જો (!ctrl_x_mode_spell())
		જાઓ normalchar;
	    જાઓ docomplete;

	સ્થિતિ Ctrl_L:	// Whole line completion after ^X
	    જો (!ctrl_x_mode_whole_લીટી())
	    {
		// CTRL-L with 'insertmode' set: Leave Insert mode
		જો (p_im)
		{
		    જો (echeck_abbr(Ctrl_L + ABBR_OFF))
			તોડો;
		    જાઓ doESCkey;
		}
		જાઓ normalchar;
	    }
	    // FALLTHROUGH

	સ્થિતિ Ctrl_P:	// Do previous/next pattern completion
	સ્થિતિ Ctrl_N:
#ifdef FEAT_PROP_POPUP
	    // CTRL-SHIFT-P/N scroll the info popup one line.
	    જો (pum_visible() && (mod_mask & MOD_MASK_SHIFT)
		    && (c == Ctrl_P || c == Ctrl_N))
	    {
		popup_scroll_info(c == Ctrl_P ? -1 : 1, false);
		તોડો;
	    }
#endif
	    // if 'complete' is empty then plain ^P is no longer special,
	    // but it is under other ^X modes
	    જો (*curbuf->b_p_cpt == NUL
		    && (ctrl_x_mode_normal() || ctrl_x_mode_whole_લીટી())
		    && !compl_status_local())
		જાઓ normalchar;

docomplete:
	    ins_compl_clear_autocomplete_delay();
	    compl_busy = TRUE;
#ifdef FEAT_FOLDING
	    disable_fold_update++;  // don't redraw folds here
#endif
	    જો (ins_complete(c, TRUE) == FAIL)
		compl_status_clear();
#ifdef FEAT_FOLDING
	    disable_fold_update--;
#endif
	    compl_busy = FALSE;
	    can_si = may_do_si(); // allow smartindenting
	    તોડો;

	સ્થિતિ Ctrl_Y:	// copy from previous line or scroll down
	સ્થિતિ Ctrl_E:	// copy from next line	   or scroll up
	    c = ins_ctrl_ey(c);
	    તોડો;

	મૂળભૂત:
#ifdef UNIX
	    જો (c == intr_char)		// special interrupt char
		જાઓ do_intr;
#endif

normalchar:
	    /*
	     * Insert a normal character.
	     */
#જો defined(FEAT_EVAL)
	    જો (!p_paste)
	    {
		// Trigger InsertCharPre.
		char_u *str = do_ઉમેરો_char_pre(c);
		char_u *p;

		જો (str != ખાલી)
		{
		    જો (*str != NUL && stop_arrow() != FAIL)
		    {
			// Insert the new value of v:char literally.
			માટે (p = str; *p != NUL; MB_PTR_ADV(p))
			{
			    c = PTR2CHAR(p);
			    જો (c == CAR || c == K_KENTER || c == NL)
				ins_eol(c);
			    નહીંતર
				ins_char(c);
			}
			AppendToRedobuffLit(str, -1);
		    }
		    vim_free(str);
		    c = NUL;
		}

		// If the new value is already inserted or an empty string
		// then don't insert any character.
		જો (c == NUL)
		    તોડો;
	    }
#endif
	    // Try to perform smart-indenting.
	    ins_try_si(c);

	    જો (c == ' ')
	    {
		inserted_space = TRUE;
		જો (inindent(0))
		    can_cindent = FALSE;
		જો (Insstart_blank_vcol == MAXCOL
			&& curwin->w_cursor.lnum == Insstart.lnum)
		    Insstart_blank_vcol = get_nolist_virtcol();
	    }

	    // Insert a normal character and check for abbreviations on a
	    // special character.  Let CTRL-] expand abbreviations without
	    // inserting it.
	    જો (vim_iswordc(c) || (!echeck_abbr(
			// Add ABBR_OFF for characters above 0x100, this is
			// what check_abbr() expects.
				(has_mbyte && c >= 0x100) ? (c + ABBR_OFF) : c)
			&& c != Ctrl_RSB))
	    {
		ઉમેરો_special(c, FALSE, FALSE);
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
	    જો (ins_compl_has_autocomplete() && !char_avail()
		    && vim_isprintc(c))
		TRIGGER_AUTOCOMPLETE();

	    તોડો;
	}   // end of switch (c)

	// If typed something may trigger CursorHoldI again; K_COMPLETE_DELAY is
	// injected, not typed.
	જો (c != K_CURSORHOLD && c != K_COMPLETE_DELAY
#ifdef FEAT_COMPL_FUNC
		// but not in CTRL-X mode, a script can't restore the state
		&& ctrl_x_mode_normal()
#endif
	       )
	    did_cursorhold = FALSE;

	// Check if we need to cancel completion mode because the window
	// or tab page was changed
	જો (ins_compl_active() && !ins_compl_win_active(curwin))
	    ins_compl_cancel();

	// If the cursor was moved we didn't just insert a space
	જો (arrow_used)
	    inserted_space = FALSE;

	જો (can_cindent && cindent_on() && ctrl_x_mode_normal())
	{
force_cindent:
	    /*
	     * Indent now if a key was typed that is in 'cinkeys'.
	     */
	    જો (in_cinkeys(c, ' ', લીટી_is_white))
	    {
		જો (stop_arrow() == OK)
		    // re-indent the current line
		    do_c_પદાવલિ_indent();
	    }
	}

    }	// for (;;)
    // NOTREACHED
}

    પૂર્ણાંક
ins_need_undo_get(શૂન્ય)
{
    પાછા ins_need_undo;
}

/*
 * Redraw for Insert mode.
 * This is postponed until getting the next character to make '$' in the 'cpo'
 * option work correctly.
 * Only redraw when there are no characters available.  This speeds up
 * inserting sequences of characters (e.g., for CTRL-R).
 */
    શૂન્ય
ins_redraw(પૂર્ણાંક ready)	    // not busy with something
{
#ifdef FEAT_CONCEAL
    linenr_T	conceal_old_cursor_લીટી = 0;
    linenr_T	conceal_new_cursor_લીટી = 0;
    પૂર્ણાંક		conceal_update_લીટીઓ = FALSE;
#endif

    જો (char_avail())
	પાછા;

    // Trigger CursorMoved if the cursor moved.  Not when the popup menu is
    // visible, the command might delete it.
    જો (ready && (has_cursormovedI()
#ifdef FEAT_PROP_POPUP
		|| popup_visible
#endif
#જો defined(FEAT_CONCEAL)
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
	જો (syntax_present(curwin) && must_redraw)
	    update_screen(0);
#endif
	જો (has_cursormovedI())
	{
	    // Make sure curswant is correct, an autocommand may call
	    // getcurpos().
	    update_curswant();
	    ins_apply_autocmds(EVENT_CURSORMOVEDI);
	}
#ifdef FEAT_PROP_POPUP
	જો (popup_visible)
	    popup_check_cursor_pos();
#endif
#ifdef FEAT_CONCEAL
	જો (curwin->w_p_cole > 0)
	{
	    conceal_old_cursor_લીટી = last_cursormoved.lnum;
	    conceal_new_cursor_લીટી = curwin->w_cursor.lnum;
	    conceal_update_લીટીઓ = TRUE;
	}
#endif
	last_cursormoved = curwin->w_cursor;
    }

    // Trigger TextChangedI if b_changedtick_i differs.
    જો (ready && has_textchangedI()
	    && curbuf->b_last_changedtick_i != CHANGEDTICK(curbuf)
	    && !pum_visible())
    {
	aco_save_T	aco;
	varnumber_T	tick = CHANGEDTICK(curbuf);

	// Save and restore curwin and curbuf, in case the autocmd changes
	// them.
	aucmd_prepbuf(&aco, curbuf);
	apply_autocmds(EVENT_TEXTCHANGEDI, ખાલી, ખાલી, FALSE, curbuf);
	aucmd_restbuf(&aco);
	curbuf->b_last_changedtick_i = CHANGEDTICK(curbuf);
	જો (tick != CHANGEDTICK(curbuf))  // see ins_apply_autocmds()
	    u_save(curwin->w_cursor.lnum,
					(linenr_T)(curwin->w_cursor.lnum + 1));
    }

    // Trigger TextChangedP if b_changedtick_pum differs. When the popupmenu
    // closes TextChangedI will need to trigger for backwards compatibility,
    // thus use different b_last_changedtick* variables.
    જો (ready && has_textchangedP()
	    && curbuf->b_last_changedtick_pum != CHANGEDTICK(curbuf)
	    && pum_visible())
    {
	aco_save_T	aco;
	varnumber_T	tick = CHANGEDTICK(curbuf);

	// Save and restore curwin and curbuf, in case the autocmd changes
	// them.
	aucmd_prepbuf(&aco, curbuf);
	apply_autocmds(EVENT_TEXTCHANGEDP, ખાલી, ખાલી, FALSE, curbuf);
	aucmd_restbuf(&aco);
	curbuf->b_last_changedtick_pum = CHANGEDTICK(curbuf);
	જો (tick != CHANGEDTICK(curbuf))  // see ins_apply_autocmds()
	    u_save(curwin->w_cursor.lnum,
					(linenr_T)(curwin->w_cursor.lnum + 1));
    }

    જો (ready)
	may_trigger_win_scrolled_resized();

    // Trigger SafeState if nothing is pending.
    may_trigger_safestate(ready
	    && !ins_compl_active()
	    && !pum_visible());

#જો defined(FEAT_CONCEAL)
    જો ((conceal_update_લીટીઓ
	    && (conceal_old_cursor_લીટી != conceal_new_cursor_લીટી
		|| conceal_cursor_લીટી(curwin)))
	    || need_cursor_લીટી_redraw)
    {
	જો (conceal_old_cursor_લીટી != conceal_new_cursor_લીટી)
	    redrawWinline(curwin, conceal_old_cursor_લીટી);
	redrawWinline(curwin, conceal_new_cursor_લીટી == 0
			    ? curwin->w_cursor.lnum : conceal_new_cursor_લીટી);
	curwin->w_valid &= ~VALID_CROW;
	need_cursor_લીટી_redraw = FALSE;
    }
#endif
    જો (must_redraw)
	update_screen(0);
    નહીંતર જો (clear_cmdline || redraw_cmdline)
	showmode();		// clear cmdline and show mode
    showruler(FALSE);
    setcursor();
    emsg_on_display = FALSE;	// may remove error message now
}

/*
 * Handle a CTRL-V or CTRL-Q typed in Insert mode.
 */
    સ્થિર શૂન્ય
ins_ctrl_v(શૂન્ય)
{
    પૂર્ણાંક		c;
    પૂર્ણાંક		did_putchar = FALSE;

    // may need to redraw when no more chars available now
    ins_redraw(FALSE);

    જો (redrawing() && !char_avail())
    {
	edit_putchar('^', TRUE);
	did_putchar = TRUE;
    }
    AppendToRedobuff((char_u *)CTRL_V_STR);	// CTRL-V

    add_to_showcmd_c(Ctrl_V);

    // Do not change any modifyOtherKeys ESC sequence to a normal key for
    // CTRL-SHIFT-V.
    c = get_literal(mod_mask & MOD_MASK_SHIFT);
    જો (did_putchar)
	// when the line fits in 'columns' the '^' is at the start of the next
	// line and will not removed by the redraw
	edit_unputchar();
    clear_showcmd();

    ઉમેરો_special(c, FALSE, TRUE);
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
    સ્થિર પૂર્ણાંક
decode_Modify_Other_ચાવીઓ(પૂર્ણાંક c)
{
    char_u  *p = typebuf.tb_buf + typebuf.tb_off;
    પૂર્ણાંક	    idx;
    પૂર્ણાંક	    form = 0;
    પૂર્ણાંક	    argidx = 0;
    પૂર્ણાંક	    arg[2] = {0, 0};

    // Recognize:
    // form 0: {lead}{key};{modifier}u
    // form 1: {lead}27;{modifier};{key}~
    જો (typebuf.tb_len >= 4 && (c == CSI || (c == ESC && *p == '[')))
    {
	idx = (*p == '[');
	જો (p[idx] == '2' && p[idx + 1] == '7' && p[idx + 2] == ';' &&
		kitty_protocol_state != KKPS_ENABLED)
	{
	    form = 1;
	    idx += 3;
	}
	જ્યારે (idx < typebuf.tb_len && argidx < 2)
	{
	    જો (p[idx] == ';')
		++argidx;
	    નહીંતર જો (VIM_ISDIGIT(p[idx]))
		arg[argidx] = arg[argidx] * 10 + (p[idx] - '0');
	    નહીંતર
		તોડો;
	    ++idx;
	}
	પૂર્ણાંક kitty_no_mods = argidx == 0 && kitty_protocol_state == KKPS_ENABLED;
	જો (idx < typebuf.tb_len
		&& p[idx] == (form == 1 ? '~' : 'u')
		&& (argidx == 1 || kitty_no_mods))
	{
	    // Match, consume the code.
	    typebuf.tb_off += idx + 1;
	    typebuf.tb_len -= idx + 1;
#જો defined(FEAT_CLIENTSERVER) || defined(FEAT_EVAL)
	    જો (typebuf.tb_len == 0)
		typebuf_was_filled = FALSE;
#endif

	    mod_mask = kitty_no_mods ? 0 : decode_modifiers(arg[!form]);
	    c = merge_modify_Other_ચાવીઓ(arg[form], &mod_mask);
	}
    }

    પાછા c;
}

/*
 * Put a character directly onto the screen.  It's not stored in a buffer.
 * Used while handling CTRL-K, CTRL-V, etc. in Insert mode.
 */
સ્થિર પૂર્ણાંક  pc_status;
#define PC_STATUS_UNSET	0	// pc_bytes was not set
#define PC_STATUS_જમણું	1	// right half of double-wide char
#define PC_STATUS_ડાબું	2	// left half of double-wide char
#define PC_STATUS_SET	3	// pc_bytes was filled
સ્થિર char_u pc_bytes[MB_MAXBYTES + 1]; // saved bytes
સ્થિર પૂર્ણાંક  pc_attr;
સ્થિર પૂર્ણાંક  pc_row;
સ્થિર પૂર્ણાંક  pc_col;

    શૂન્ય
edit_putchar(પૂર્ણાંક c, પૂર્ણાંક highlight)
{
    પૂર્ણાંક	    attr;

    જો (Screen_લીટીઓ == ખાલી)
	પાછા;

    update_topline();	// just in case w_topline isn't valid
    validate_cursor();
    જો (highlight)
	attr = HL_ATTR(HLF_8);
    નહીંતર
	attr = 0;
    pc_row = W_WINROW(curwin) + curwin->w_wrow;
    pc_col = curwin->w_wincol;
    pc_status = PC_STATUS_UNSET;
#ifdef FEAT_RIGHTLEFT
    જો (curwin->w_p_rl)
    {
	pc_col += curwin->w_width - 1 - curwin->w_wcol;
	જો (has_mbyte)
	{
	    પૂર્ણાંક fix_col = mb_fix_col(pc_col, pc_row);

	    જો (fix_col != pc_col)
	    {
		screen_putchar(' ', pc_row, fix_col, attr);
		--curwin->w_wcol;
		pc_status = PC_STATUS_જમણું;
	    }
	}
    }
    નહીંતર
#endif
    {
	pc_col += curwin->w_wcol;
	જો (mb_lefthalve(pc_row, pc_col))
	    pc_status = PC_STATUS_ડાબું;
    }

    // save the character to be able to put it back
    જો (pc_status == PC_STATUS_UNSET)
    {
	screen_getbytes(pc_row, pc_col, pc_bytes, &pc_attr);
	pc_status = PC_STATUS_SET;
    }
    screen_putchar(c, pc_row, pc_col, attr);
}

#જો defined(FEAT_JOB_CHANNEL)
/*
 * Set the insert start position for when using a prompt buffer.
 */
    શૂન્ય
set_insstart(linenr_T lnum, પૂર્ણાંક col)
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
    શૂન્ય
edit_unputchar(શૂન્ય)
{
    જો (pc_status != PC_STATUS_UNSET && pc_row >= સંદેશ_scrolled)
    {
	જો (pc_status == PC_STATUS_જમણું)
	    ++curwin->w_wcol;
	જો (pc_status == PC_STATUS_જમણું || pc_status == PC_STATUS_ડાબું)
	    redrawWinline(curwin, curwin->w_cursor.lnum);
	નહીંતર
	    screen_puts(pc_bytes, pc_row - સંદેશ_scrolled, pc_col, pc_attr);
    }
}

/*
 * Called when "$" is in 'cpoptions': display a '$' at the end of the changed
 * text.  Only works when cursor is in the line that changes.
 */
    શૂન્ય
display_dollar(colnr_T col_arg)
{
    colnr_T col = col_arg < 0 ? 0 : col_arg;
    colnr_T save_col;

    જો (!redrawing())
	પાછા;

    cursor_off();
    save_col = curwin->w_cursor.col;
    curwin->w_cursor.col = col;
    જો (has_mbyte)
    {
	char_u *p;

	// If on the last byte of a multi-byte move to the first byte.
	p = ml_get_curline();
	curwin->w_cursor.col -= (*mb_head_off)(p, p + col);
    }
    curs_columns(FALSE);	    // recompute w_wrow and w_wcol
    જો (curwin->w_wcol < curwin->w_width)
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
    શૂન્ય
undisplay_dollar(શૂન્ય)
{
    જો (dollar_vcol < 0)
	પાછા;

    dollar_vcol = -1;
    redrawWinline(curwin, curwin->w_cursor.lnum);
}

/*
 * Truncate the space at the end of a line.  This is to be used only in an
 * insert mode.  It handles fixing the replace stack for MODE_REPLACE and
 * MODE_VREPLACE modes.
 */
    શૂન્ય
truncate_spaces(char_u *લીટી, size_t len)
{
    પૂર્ણાંક	    i;

    // find start of trailing white space
    માટે (i = (પૂર્ણાંક)len - 1; i >= 0 && VIM_ISWHITE(લીટી[i]); i--)
    {
	જો (State & REPLACE_FLAG)
	    replace_join(0);	    // remove a NUL from the replace stack
    }
    લીટી[i + 1] = NUL;
}

/*
 * Backspace the cursor until the given column.  Handles MODE_REPLACE and
 * MODE_VREPLACE modes correctly.  May also be used when not in insert mode at
 * all.  Will attempt not to go before "col" even when there is a composing
 * character.
 */
    શૂન્ય
backspace_until_column(પૂર્ણાંક col)
{
    જ્યારે ((પૂર્ણાંક)curwin->w_cursor.col > col)
    {
	curwin->w_cursor.col--;
	જો (State & REPLACE_FLAG)
	    replace_do_bs(col);
	નહીંતર જો (!del_char_after_col(col))
	    તોડો;
    }
}

/*
 * Like del_char(), but make sure not to go before column "limit_col".
 * Only matters when there are composing characters.
 * Return TRUE when something was deleted.
 */
    સ્થિર પૂર્ણાંક
del_char_after_col(પૂર્ણાંક limit_col UNUSED)
{
    જો (enc_utf8 && limit_col >= 0)
    {
	colnr_T ecol = curwin->w_cursor.col + 1;

	// Make sure the cursor is at the start of a character, but
	// skip forward again when going too far back because of a
	// composing character.
	mb_adjust_cursor();
	જ્યારે (curwin->w_cursor.col < (colnr_T)limit_col)
	{
	    પૂર્ણાંક l = utf_ptr2len(ml_get_cursor());

	    જો (l == 0)  // end of line
		તોડો;
	    curwin->w_cursor.col += l;
	}
	જો (*ml_get_cursor() == NUL || curwin->w_cursor.col == ecol)
	    પાછા FALSE;
	del_bytes((long)((પૂર્ણાંક)ecol - curwin->w_cursor.col), FALSE, TRUE);
    }
    નહીંતર
	(શૂન્ય)del_char(FALSE);
    પાછા TRUE;
}

/*
 * Next character is interpreted literally.
 * A one, two or three digit decimal number is interpreted as its byte value.
 * If one or two digits are entered, the next character is given to vungetc().
 * For Unicode a character > 255 may be returned.
 * If "noReduceKeys" is TRUE do not change any modifyOtherKeys ESC sequence
 * into a normal key, return ESC.
 */
    પૂર્ણાંક
get_literal(પૂર્ણાંક no_Reduce_ચાવીઓ)
{
    પૂર્ણાંક		cc;
    પૂર્ણાંક		nc;
    પૂર્ણાંક		i;
    પૂર્ણાંક		hex = FALSE;
    પૂર્ણાંક		octal = FALSE;
    પૂર્ણાંક		unicode = 0;

    જો (got_int)
	પાછા Ctrl_C;

#ifdef FEAT_GUI
    /*
     * In GUI there is no point inserting the internal code for a special key.
     * It is more useful to insert the string "<KEY>" instead.	This would
     * probably be useful in a text window too, but it would not be
     * vi-compatible (maybe there should be an option for it?) -- webb
     */
    જો (gui.in_use)
    {
	++allow_ચાવીઓ;
	જો (no_Reduce_ચાવીઓ)
	    ++no_reduce_ચાવીઓ;
    }
#endif
#ifdef USE_ON_FLY_SCROLL
    dont_scroll = TRUE;		// disallow scrolling here
#endif
    ++no_mapping;		// don't map the next key hits
    cc = 0;
    i = 0;
    માટે (;;)
    {
	nc = plain_vgetc();
	જો ((nc == ESC || nc == CSI) && !no_Reduce_ચાવીઓ)
	    nc = decode_Modify_Other_ચાવીઓ(nc);

	જો ((mod_mask & ~MOD_MASK_SHIFT) != 0)
	    // A character with non-Shift modifiers should not be a valid
	    // character for i_CTRL-V_digit.
	    તોડો;

	જો ((State & MODE_CMDLINE) == 0 && MB_BYTE2LEN_CHECK(nc) == 1)
	    add_to_showcmd(nc);
	જો (nc == 'x' || nc == 'X')
	    hex = TRUE;
	નહીંતર જો (nc == 'o' || nc == 'O')
	    octal = TRUE;
	નહીંતર જો (nc == 'u' || nc == 'U')
	    unicode = nc;
	નહીંતર
	{
	    જો (hex || unicode != 0)
	    {
		જો (!vim_isxdigit(nc))
		    તોડો;
		cc = cc * 16 + hex2nr(nc);
	    }
	    નહીંતર જો (octal)
	    {
		જો (nc < '0' || nc > '7')
		    તોડો;
		cc = cc * 8 + nc - '0';
	    }
	    નહીંતર
	    {
		જો (!VIM_ISDIGIT(nc))
		    તોડો;
		cc = cc * 10 + nc - '0';
	    }

	    ++i;
	}

	જો (cc > 255 && unicode == 0)
	    cc = 255;		// limit range to 0-255
	nc = 0;

	જો (hex)		// hex: up to two chars
	{
	    જો (i >= 2)
		તોડો;
	}
	નહીંતર જો (unicode)	// Unicode: up to four or eight chars
	{
	    જો ((unicode == 'u' && i >= 4) || (unicode == 'U' && i >= 8))
		તોડો;
	}
	નહીંતર જો (i >= 3)	// decimal or octal: up to three chars
	    તોડો;
    }
    જો (i == 0)	    // no number entered
    {
	જો (nc == K_ZERO)   // NUL is stored as NL
	{
	    cc = '\n';
	    nc = 0;
	}
	નહીંતર
	{
	    cc = nc;
	    nc = 0;
	}
    }

    જો (cc == 0)	// NUL is stored as NL
	cc = '\n';
    જો (enc_dbcs && (cc & 0xff) == 0)
	cc = '?';	// don't accept an illegal DBCS char, the NUL in the
			// second byte will cause trouble!

    --no_mapping;
#ifdef FEAT_GUI
    જો (gui.in_use)
    {
	--allow_ચાવીઓ;
	જો (no_Reduce_ચાવીઓ)
	    --no_reduce_ચાવીઓ;
    }
#endif
    જો (nc)
    {
	vungetc(nc);
	// A character typed with i_CTRL-V_digit cannot have modifiers.
	mod_mask = 0;
    }
    got_int = FALSE;	    // CTRL-C typed after CTRL-V is not an interrupt
    પાછા cc;
}

/*
 * Insert character, taking care of special keys and mod_mask
 */
    સ્થિર શૂન્ય
ઉમેરો_special(
    પૂર્ણાંક	    c,
    પૂર્ણાંક	    allow_modmask,
    પૂર્ણાંક	    ctrlv)	    // c was typed after CTRL-V
{
    char_u  *p;
    પૂર્ણાંક	    len;

    /*
     * Special function key, translate into "<Key>". Up to the last '>' is
     * inserted with ins_str(), so as not to replace characters in replace
     * mode.
     * Only use mod_mask for special keys, to avoid things like <S-Space>,
     * unless 'allow_modmask' is TRUE.
     */
#જો defined(MACOS_X) || defined(FEAT_GUI_GTK)
    // Command-key never produces a normal key
    જો (mod_mask & MOD_MASK_CMD)
	allow_modmask = TRUE;
#endif
    જો (IS_SPECIAL(c) || (mod_mask && allow_modmask))
    {
	p = get_special_ચાવી_name(c, mod_mask);
	len = (પૂર્ણાંક)STRLEN(p);
	c = p[len - 1];
	જો (len > 2)
	{
	    જો (stop_arrow() == FAIL)
		પાછા;
	    p[len - 1] = NUL;
	    ins_str(p, len - 1);
	    AppendToRedobuffLit(p, -1);
	    ctrlv = FALSE;
	}
    }
    જો (stop_arrow() == OK)
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
    શૂન્ય
insertchar(
    પૂર્ણાંક		c,			// character to insert or NUL
    પૂર્ણાંક		flags,			// INSCHAR_FORMAT, etc.
    પૂર્ણાંક		second_indent)		// indent for second line if >= 0
{
    પૂર્ણાંક		textwidth;
    char_u	*p;
    પૂર્ણાંક		fo_ins_blank;
    પૂર્ણાંક		force_format = flags & INSCHAR_FORMAT;

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
    જો (textwidth > 0
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
#જો defined(FEAT_EVAL)
	પૂર્ણાંક     do_internal = TRUE;
	colnr_T virtcol = get_nolist_virtcol()
				  + char2cells(c != NUL ? c : gchar_cursor());

	જો (*curbuf->b_p_fex != NUL && (flags & INSCHAR_NO_FEX) == 0
		&& (force_format || virtcol > (colnr_T)textwidth))
	{
	    do_internal = (fex_format(curwin->w_cursor.lnum, 1L, c) != 0);
	    // It may be required to save for undo again, e.g. when setline()
	    // was called.
	    ins_need_undo = TRUE;
	}
	જો (do_internal)
#endif
	    internal_format(textwidth, second_indent, flags, c == NUL, c);
    }

    જો (c == NUL)	    // only formatting was wanted
	પાછા;

    // Check whether this character should end a comment.
    જો (did_ai && c == end_comment_pending)
    {
	char_u  *લીટી;
	char_u	lead_end[COM_MAX_LEN];	    // end-comment string
	પૂર્ણાંક	મધ્ય_len, end_len;
	પૂર્ણાંક	i;

	/*
	 * Need to remove existing (middle) comment leader and insert end
	 * comment leader.  First, check what comment leader we can find.
	 */
	i = get_leader_len(લીટી = ml_get_curline(), &p, FALSE, TRUE);
	જો (i > 0 && vim_strchr(p, COM_મધ્ય) != ખાલી)	// Just checking
	{
	    // Skip middle-comment string
	    જ્યારે (*p && p[-1] != ':')	// find end of middle flags
		++p;
	    મધ્ય_len = copy_option_part(&p, lead_end, COM_MAX_LEN, ",");
	    // Don't count trailing white space for middle_len
	    જ્યારે (મધ્ય_len > 0 && VIM_ISWHITE(lead_end[મધ્ય_len - 1]))
		--મધ્ય_len;

	    // Find the end-comment string
	    જ્યારે (*p && p[-1] != ':')	// find end of end flags
		++p;
	    end_len = copy_option_part(&p, lead_end, COM_MAX_LEN, ",");

	    // Skip white space before the cursor
	    i = curwin->w_cursor.col;
	    જ્યારે (--i >= 0 && VIM_ISWHITE(લીટી[i]))
		;
	    i++;

	    // Skip to before the middle leader
	    i -= મધ્ય_len;

	    // Check some expected things before we go on
	    જો (i >= 0 && end_len > 0 && lead_end[end_len - 1] == end_comment_pending)
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

    જો (       !ISSPECIAL(c)
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
#define આગત_BUFLEN 100
	char_u		buf[આગત_BUFLEN + 1];
	પૂર્ણાંક		i;
	colnr_T		virtcol = 0;

	buf[0] = c;
	i = 1;
	જો (textwidth > 0)
	    virtcol = get_nolist_virtcol();
	/*
	 * Stop the string when:
	 * - no more chars available
	 * - finding a special character (command key)
	 * - buffer is full
	 * - running into the 'textwidth' boundary
	 * - need to check for abbreviation: A non-word char after a word-char
	 */
	જ્યારે (	   (c = vpeekc()) != NUL
		&& !ISSPECIAL(c)
		&& (!has_mbyte || MB_BYTE2LEN_CHECK(c) == 1)
		&& i < આગત_BUFLEN
		&& (textwidth == 0
		    || (virtcol += byte2cells(buf[i - 1])) < (colnr_T)textwidth)
		&& !(!no_abbr && !vim_iswordc(c) && vim_iswordc(buf[i - 1])))
	{
#ifdef FEAT_RIGHTLEFT
	    c = vgetc();
	    જો (p_hkmap && ચાવી_Typed)
		c = hkmap(c);		    // Hebrew mode mapping
	    buf[i++] = c;
#નહીંતર
	    buf[i++] = vgetc();
#endif
	}

#ifdef FEAT_DIGRAPHS
	do_digraph(-1);			// clear digraphs
	do_digraph(buf[i-1]);		// may be the start of a digraph
#endif
	buf[i] = NUL;
	ins_str(buf, i);
	જો (flags & INSCHAR_CTRLV)
	{
	    redo_literal(*buf);
	    i = 1;
	}
	નહીંતર
	    i = 0;
	જો (buf[i] != NUL)
	    AppendToRedobuffLit(buf + i, -1);
    }
    નહીંતર
    {
	પૂર્ણાંક		cc;

	જો (has_mbyte && (cc = (*mb_char2len)(c)) > 1)
	{
	    char_u	buf[MB_MAXBYTES + 1];

	    (*mb_char2bytes)(c, buf);
	    buf[cc] = NUL;
	    ins_char_bytes(buf, cc);
	    AppendCharToRedobuff(c);
	}
	નહીંતર
	{
	    ins_char(c);
	    જો (flags & INSCHAR_CTRLV)
		redo_literal(c);
	    નહીંતર
		AppendCharToRedobuff(c);
	}
    }
}

/*
 * Put a character in the redo buffer, for when just after a CTRL-V.
 */
    સ્થિર શૂન્ય
redo_literal(પૂર્ણાંક c)
{
    char_u	buf[10];

    // Only digits need special treatment.  Translate them into a string of
    // three digits.
    જો (VIM_ISDIGIT(c))
    {
	vim_snprintf((અક્ષર *)buf, કદ(buf), "%03d", c);
	AppendToRedobuff(buf);
    }
    નહીંતર
	AppendCharToRedobuff(c);
}

/*
 * start_arrow() is called when an arrow key is used in insert mode.
 * For undo/redo it resembles hitting the <ESC> key.
 */
    શૂન્ય
start_arrow(
    pos_T    *end_ઉમેરો_pos)		// can be NULL
{
    start_arrow_common(end_ઉમેરો_pos, TRUE);
}

/*
 * Like start_arrow() but with end_change argument.
 * Will prepare for redo of CTRL-G U if "end_change" is FALSE.
 */
    સ્થિર શૂન્ય
start_arrow_with_change(
    pos_T    *end_ઉમેરો_pos,		// can be NULL
    પૂર્ણાંક	      end_change)		// end undoable change
{
    start_arrow_common(end_ઉમેરો_pos, end_change);
    જો (!end_change)
    {
	AppendCharToRedobuff(Ctrl_G);
	AppendCharToRedobuff('U');
    }
}

    સ્થિર શૂન્ય
start_arrow_common(
    pos_T    *end_ઉમેરો_pos,		// can be NULL
    પૂર્ણાંક	      end_change)		// end undoable change
{
    જો (!arrow_used && end_change)	// something has been inserted
    {
	AppendToRedobuff(ESC_STR);
	stop_ઉમેરો(end_ઉમેરો_pos, FALSE, FALSE);
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
    સ્થિર શૂન્ય
check_spell_redraw(શૂન્ય)
{
    જો (spell_redraw_lnum != 0)
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
    પૂર્ણાંક
stop_arrow(શૂન્ય)
{
    જો (arrow_used)
    {
	Insstart = curwin->w_cursor;	// new insertion starts here
	જો (Insstart.col > Insstart_orig.col && !ins_need_undo)
	    // Don't update the original insert position when moved to the
	    // right, except when nothing was inserted yet.
	    update_Insstart_orig = FALSE;
	Insstart_textlen = (colnr_T)linetabsize_str(ml_get_curline());

	જો (u_save_cursor() == OK)
	{
	    arrow_used = FALSE;
	    ins_need_undo = FALSE;
	}

	ai_col = 0;
	જો (State & VREPLACE_FLAG)
	{
	    orig_લીટી_ગણતરી = curbuf->b_ml.ml_લીટી_ગણતરી;
	    vr_લીટીઓ_changed = 1;
	}
	ResetRedobuff();
	AppendToRedobuff((char_u *)"1i");   // pretend we start an insertion
	new_ઉમેરો_skip = 2;
    }
    નહીંતર જો (ins_need_undo)
    {
	જો (u_save_cursor() == OK)
	{
	    // A command or event may have moved the cursor before the next
	    // edit. Pull Insstart back only when the cursor moved above it,
	    // so that later edits can properly decide whether an extra undo
	    // entry is needed. Advancing Insstart would mis-place '[ after a
	    // register paste.
	    જો (LT_POS(curwin->w_cursor, Insstart))
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

    પાછા (arrow_used || ins_need_undo ? FAIL : OK);
}

/*
 * Do a few things to stop inserting.
 * "end_insert_pos" is where insert ended.  It is NULL when we already jumped
 * to another window/buffer.
 */
    સ્થિર શૂન્ય
stop_ઉમેરો(
    pos_T	*end_ઉમેરો_pos,
    પૂર્ણાંક		esc,			// called by ins_esc()
    પૂર્ણાંક		nomove)			// <c-\><c-o>, don't move cursor
{
    પૂર્ણાંક		cc;
    string_T	inserted;

    stop_redo_ins();
    replace_flush();		// abandon replace stack

    /*
     * Save the inserted text for later redo with ^@ and CTRL-A.
     * Don't do it when "restart_edit" was set and nothing was inserted,
     * otherwise CTRL-O w and then <Left> will clear "last_insert".
     */
    inserted = get_inserted();
    પૂર્ણાંક added = inserted.string == ખાલી ? 0 : (પૂર્ણાંક)inserted.length - new_ઉમેરો_skip;
    જો (did_restart_edit == 0 || added > 0)
    {
	vim_free(last_ઉમેરો.string);
	last_ઉમેરો = inserted;			    // structure copy
	last_ઉમેરો_skip = added < 0 ? 0 : new_ઉમેરો_skip;
    }
    નહીંતર
	vim_free(inserted.string);

    જો (!arrow_used && end_ઉમેરો_pos != ખાલી)
    {
	// Auto-format now.  It may seem strange to do this when stopping an
	// insertion (or moving the cursor), but it's required when appending
	// a line and having it end in a space.  But only do it when something
	// was actually inserted, otherwise undo won't work.
	જો (!ins_need_undo && has_format_option(FO_AUTO))
	{
	    pos_T   tpos = curwin->w_cursor;

	    // When the cursor is at the end of the line after a space the
	    // formatting will move it to the following word.  Avoid that by
	    // moving the cursor onto the space.
	    cc = 'x';
	    જો (curwin->w_cursor.col > 0 && gchar_cursor() == NUL)
	    {
		dec_cursor();
		cc = gchar_cursor();
		જો (!VIM_ISWHITE(cc))
		    curwin->w_cursor = tpos;
	    }

	    auto_format(TRUE, FALSE);

	    જો (VIM_ISWHITE(cc))
	    {
		જો (gchar_cursor() != NUL)
		    inc_cursor();
		// If the cursor is still at the same character, also keep
		// the "coladd".
		જો (gchar_cursor() == NUL
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
	જો (!nomove && did_ai && (esc || (vim_strchr(p_cpo, CPO_INDENT) == ખાલી
			&& curwin->w_cursor.lnum != end_ઉમેરો_pos->lnum))
		&& end_ઉમેરો_pos->lnum <= curbuf->b_ml.ml_લીટી_ગણતરી)
	{
	    pos_T	tpos = curwin->w_cursor;
	    colnr_T	prev_col = end_ઉમેરો_pos->col;
	    colnr_T	strip_col;

	    curwin->w_cursor = *end_ઉમેરો_pos;
	    check_cursor_col();  // make sure it is not past the line

	    // Where the loop would actually start (back up if on NUL).
	    strip_col = curwin->w_cursor.col;
	    જો (gchar_cursor() == NUL && strip_col > 0)
		--strip_col;

	    // Don't strip if non-whitespace follows: setline() from a <Cmd>
	    // mapping or CursorHoldI autocmd may have inserted content.
	    જો (*skipwhite(ml_get_curline() + strip_col) == NUL)
	    {
		curwin->w_cursor.col = strip_col;
		માટે (;;)
		{
		    જો (gchar_cursor() == NUL && curwin->w_cursor.col > 0)
			--curwin->w_cursor.col;
		    cc = gchar_cursor();
		    જો (!VIM_ISWHITE(cc))
			તોડો;
		    જો (del_char(TRUE) == FAIL)
			તોડો;  // should not happen
		}
		જો (curwin->w_cursor.lnum != tpos.lnum)
		    curwin->w_cursor = tpos;
		નહીંતર જો (curwin->w_cursor.col < prev_col)
		{
		    // reset tpos, could have been invalidated in the loop above
		    tpos = curwin->w_cursor;
		    tpos.col++;
		    જો (cc != NUL && gchar_pos(&tpos) == NUL)
			++curwin->w_cursor.col;	// put cursor back on the NUL
		}

		// <C-S-Right> may have started Visual mode, adjust the position for
		// deleted characters.
		જો (VIsual_active)
		    check_visual_pos();
	    }
	    નહીંતર
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
    જો (end_ઉમેરો_pos != ખાલી)
    {
	curbuf->b_op_start = Insstart;
	curbuf->b_op_start_orig = Insstart_orig;
	curbuf->b_op_end = *end_ઉમેરો_pos;
    }
}

/*
 * Set the last inserted text to a single character.
 * Used for the replace command.
 */
    શૂન્ય
set_last_ઉમેરો(પૂર્ણાંક c)
{
    char_u	*s;

    vim_free(last_ઉમેરો.string);
    last_ઉમેરો.string = alloc(MB_MAXBYTES * 3 + 5);
    જો (last_ઉમેરો.string == ખાલી)
    {
	last_ઉમેરો.length = 0;
	પાછા;
    }

    s = last_ઉમેરો.string;
    // Use the CTRL-V only when entering a special char
    જો (c < ' ' || c == DEL)
	*s++ = Ctrl_V;
    s = add_char2buf(c, s);
    *s++ = ESC;
    *s = NUL;
    last_ઉમેરો.length = (size_t)(s - last_ઉમેરો.string);
    last_ઉમેરો_skip = 0;
}

#જો defined(EXITFREE)
    શૂન્ય
free_last_ઉમેરો(શૂન્ય)
{
    VIM_CLEAR_STRING(last_ઉમેરો);
}
#endif

/*
 * Add character "c" to buffer "s".  Escape the special meaning of K_SPECIAL
 * and CSI.  Handle multi-byte characters.
 * Returns a pointer to after the added bytes.
 */
    char_u *
add_char2buf(પૂર્ણાંક c, char_u *s)
{
    char_u	temp[MB_MAXBYTES + 1];
    પૂર્ણાંક		i;
    પૂર્ણાંક		len;

    len = (*mb_char2bytes)(c, temp);
    માટે (i = 0; i < len; ++i)
    {
	c = temp[i];
	// Need to escape K_SPECIAL and CSI like in the typeahead buffer.
	જો (c == K_SPECIAL)
	{
	    *s++ = K_SPECIAL;
	    *s++ = KS_SPECIAL;
	    *s++ = KE_FILLER;
	}
#ifdef FEAT_GUI
	નહીંતર જો (c == CSI)
	{
	    *s++ = CSI;
	    *s++ = KS_EXTRA;
	    *s++ = (પૂર્ણાંક)KE_CSI;
	}
#endif
	નહીંતર
	    *s++ = c;
    }
    પાછા s;
}

/*
 * move cursor to start of line
 * if flags & BL_WHITE	move to first non-white
 * if flags & BL_SOL	move to first non-white if startofline is set,
 *			    otherwise keep "curswant" column
 * if flags & BL_FIX	don't leave the cursor on a NUL.
 */
    શૂન્ય
beginline(પૂર્ણાંક flags)
{
    જો ((flags & BL_SOL) && !p_sol)
	coladvance(curwin->w_curswant);
    નહીંતર
    {
	curwin->w_cursor.col = 0;
	curwin->w_cursor.coladd = 0;

	જો (flags & (BL_WHITE | BL_SOL))
	{
	    char_u  *ptr;

	    માટે (ptr = ml_get_curline(); VIM_ISWHITE(*ptr)
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

    પૂર્ણાંક
oneright(શૂન્ય)
{
    char_u	*ptr;
    પૂર્ણાંક		l;

    જો (virtual_active())
    {
	pos_T	prevpos = curwin->w_cursor;

	// Adjust for multi-wide char (excluding TAB)
	ptr = ml_get_cursor();
	coladvance(getviscol() + ((*ptr != TAB
					  && vim_isprintc((*mb_ptr2char)(ptr)))
		    ? ptr2cells(ptr) : 1));
	curwin->w_set_curswant = true;
	// Return OK if the cursor moved, FAIL otherwise (at window edge).
	પાછા (prevpos.col != curwin->w_cursor.col
		    || prevpos.coladd != curwin->w_cursor.coladd) ? OK : FAIL;
    }

    ptr = ml_get_cursor();
    જો (*ptr == NUL)
	પાછા FAIL;	    // already at the very end

    જો (has_mbyte)
	l = (*mb_ptr2len)(ptr);
    નહીંતર
	l = 1;

    // move "l" bytes right, but don't end up on the NUL, unless 'virtualedit'
    // contains "onemore".
    જો (ptr[l] == NUL && (get_ve_flags() & VE_ONEMORE) == 0)
	પાછા FAIL;
    curwin->w_cursor.col += l;

    curwin->w_set_curswant = true;
    adjust_skipcol();
    પાછા OK;
}

    પૂર્ણાંક
oneleft(શૂન્ય)
{
    જો (virtual_active())
    {
#ifdef FEAT_LINEBREAK
	પૂર્ણાંક width;
#endif
	પૂર્ણાંક v = getviscol();

	જો (v == 0)
	    પાછા FAIL;

#ifdef FEAT_LINEBREAK
	// We might get stuck on 'showbreak', skip over it.
	width = 1;
	માટે (;;)
	{
	    coladvance(v - width);
	    // getviscol() is slow, skip it when 'showbreak' is empty,
	    // 'breakindent' is not set and there are no multi-byte
	    // characters
	    જો ((*get_showbreak_કિંમત(curwin) == NUL && !curwin->w_p_bri
					     && !has_mbyte) || getviscol() < v)
		તોડો;
	    ++width;
	}
#નહીંતર
	coladvance(v - 1);
#endif

	જો (curwin->w_cursor.coladd == 1)
	{
	    char_u *ptr;

	    // Adjust for multi-wide char (not a TAB)
	    ptr = ml_get_cursor();
	    જો (*ptr != TAB && vim_isprintc((*mb_ptr2char)(ptr))
							 && ptr2cells(ptr) > 1)
		curwin->w_cursor.coladd = 0;
	}

	curwin->w_set_curswant = true;
	adjust_skipcol();
	પાછા OK;
    }

    જો (curwin->w_cursor.col == 0)
	પાછા FAIL;

    curwin->w_set_curswant = true;
    --curwin->w_cursor.col;

    // if the character on the left of the current cursor is a multi-byte
    // character, move to its first byte
    જો (has_mbyte)
	mb_adjust_cursor();
    adjust_skipcol();
    પાછા OK;
}

/*
 * Move the cursor up "n" lines in window "wp".
 * Takes care of closed folds.
 */
    શૂન્ય
cursor_up_inner(win_T *wp, long n)
{
    linenr_T	lnum = wp->w_cursor.lnum;

    જો (n >= lnum)
	lnum = 1;
    નહીંતર
#ifdef FEAT_FOLDING
	જો (hasAnyFolding(wp))
    {
	/*
	 * Count each sequence of folded lines as one logical line.
	 */
	// go to the start of the current fold
	(શૂન્ય)hasFoldingWin(wp, lnum, &lnum, ખાલી, TRUE, ખાલી);

	જ્યારે (n--)
	{
	    // move up one line
	    --lnum;
	    જો (lnum <= 1)
		તોડો;
	    // If we entered a fold, move to the beginning, unless in
	    // Insert mode or when 'foldopen' contains "all": it will open
	    // in a moment.
	    જો (n > 0 || !((State & MODE_ઉમેરો) || (fdo_flags & FDO_ALL)))
		(શૂન્ય)hasFoldingWin(wp, lnum, &lnum, ખાલી, TRUE, ખાલી);
	}
	જો (lnum < 1)
	    lnum = 1;
    }
    નહીંતર
#endif
	lnum -= n;

    wp->w_cursor.lnum = lnum;
}

    પૂર્ણાંક
cursor_up(
    long	n,
    પૂર્ણાંક		upd_topline)	    // When TRUE: update topline
{
    // This fails if the cursor is already in the first line or the count is
    // larger than the line number and '-' is in 'cpoptions'
    linenr_T lnum = curwin->w_cursor.lnum;
    જો (n > 0 && (lnum <= 1
		       || (n >= lnum && vim_strchr(p_cpo, CPO_MINUS) != ખાલી)))
	પાછા FAIL;
    cursor_up_inner(curwin, n);

    // try to advance to the column we want to be at
    coladvance(curwin->w_curswant);

    જો (upd_topline)
	update_topline();	// make sure curwin->w_topline is valid

    પાછા OK;
}

/*
 * Move the cursor down "n" lines in window "wp".
 * Takes care of closed folds.
 */
    શૂન્ય
cursor_down_inner(win_T *wp, long n)
{
    linenr_T	lnum = wp->w_cursor.lnum;
    linenr_T	લીટી_ગણતરી = wp->w_buffer->b_ml.ml_લીટી_ગણતરી;

    જો (lnum + n >= લીટી_ગણતરી)
	lnum = લીટી_ગણતરી;
    નહીંતર
#ifdef FEAT_FOLDING
	જો (hasAnyFolding(wp))
    {
	linenr_T	last;

	// count each sequence of folded lines as one logical line
	જ્યારે (n--)
	{
	    જો (hasFoldingWin(wp, lnum, ખાલી, &last, TRUE, ખાલી))
		lnum = last + 1;
	    નહીંતર
		++lnum;
	    જો (lnum >= લીટી_ગણતરી)
		તોડો;
	}
	જો (lnum > લીટી_ગણતરી)
	    lnum = લીટી_ગણતરી;
    }
    નહીંતર
#endif
	lnum += n;

    wp->w_cursor.lnum = lnum;
}

/*
 * Cursor down a number of logical lines.
 */
    પૂર્ણાંક
cursor_down(
    long	n,
    પૂર્ણાંક		upd_topline)	    // When TRUE: update topline
{
    linenr_T	lnum = curwin->w_cursor.lnum;
    linenr_T	લીટી_ગણતરી = curwin->w_buffer->b_ml.ml_લીટી_ગણતરી;
    // This fails if the cursor is already in the last (folded) line, or would
    // move beyond the last line and '-' is in 'cpoptions'.
#ifdef FEAT_FOLDING
    hasFoldingWin(curwin, lnum, ખાલી, &lnum, TRUE, ખાલી);
#endif
    જો (n > 0
	    && (lnum >= લીટી_ગણતરી
		|| (lnum + n > લીટી_ગણતરી
				     && vim_strchr(p_cpo, CPO_MINUS) != ખાલી)))
	પાછા FAIL;
    cursor_down_inner(curwin, n);

    // try to advance to the column we want to be at
    coladvance(curwin->w_curswant);

    જો (upd_topline)
	update_topline();	// make sure curwin->w_topline is valid

    પાછા OK;
}

/*
 * Stuff the last inserted text in the read buffer.
 * Last_insert actually is a copy of the redo buffer, so we
 * first have to remove the command.
 */
    પૂર્ણાંક
stuff_inserted(
    પૂર્ણાંક	    c,		// Command character to be inserted
    long    ગણતરી,	// Repeat this many times
    પૂર્ણાંક	    no_esc)	// Don't add an ESC at the end
{
    string_T	ઉમેરો;				// text to be inserted
    char_u	last = ' ';

    ઉમેરો = get_last_ઉમેરો();
    જો (ઉમેરો.string == ખાલી)
    {
	emsg(_(e_no_inserted_text_yet));
	પાછા FAIL;
    }

    // may want to stuff the command character, to start Insert mode
    જો (c != NUL)
	stuffcharReadbuff(c);

    જો (ઉમેરો.length > 0)
    {
	char_u	*p;

	// look for the last ESC in 'insert'
	માટે (p = ઉમેરો.string + ઉમેરો.length - 1; p >= ઉમેરો.string; --p)
	{
	    જો (*p == ESC)
	    {
		ઉમેરો.length = (size_t)(p - ઉમેરો.string);
		તોડો;
	    }
	}
    }

    જો (ઉમેરો.length > 0)
    {
	char_u	*p = ઉમેરો.string + ઉમેરો.length - 1;

	// when the last char is either "0" or "^" it will be quoted if no ESC
	// comes after it OR if it will insert more than once and "ptr"
	// starts with ^D.	-- Acevedo
	જો ((*p == '0' || *p == '^')
		&& (no_esc || (*ઉમેરો.string == Ctrl_D && ગણતરી > 1)))
	{
	    last = *p;
	    --ઉમેરો.length;
	}
    }

    કરો
    {
	stuffReadbuffLen(ઉમેરો.string, (long)ઉમેરો.length);
	// a trailing "0" is inserted as "<C-V>048", "^" as "<C-V>^"
	પસંદગી (last)
	{
	સ્થિતિ '0':
#define TEXT_TO_ઉમેરો "\026\060\064\070"
	    stuffReadbuffLen((char_u *)TEXT_TO_ઉમેરો, STRLEN_LITERAL(TEXT_TO_ઉમેરો));
#undef TEXT_TO_ઉમેરો
	    તોડો;

	સ્થિતિ '^':
#define TEXT_TO_ઉમેરો "\026^"
	    stuffReadbuffLen((char_u *)TEXT_TO_ઉમેરો, STRLEN_LITERAL(TEXT_TO_ઉમેરો));
#undef TEXT_TO_ઉમેરો
	    તોડો;

	મૂળભૂત:
	    તોડો;
	}
    }
    જ્યારે (--ગણતરી > 0);

    // may want to stuff a trailing ESC, to get out of Insert mode
    જો (!no_esc)
	stuffcharReadbuff(ESC);

    પાછા OK;
}

    string_T
get_last_ઉમેરો(શૂન્ય)
{
    string_T ઉમેરો = {ખાલી, 0};

    જો (last_ઉમેરો.string != ખાલી)
    {
	ઉમેરો.string = last_ઉમેરો.string + last_ઉમેરો_skip;
	ઉમેરો.length = (size_t)(last_ઉમેરો.length - last_ઉમેરો_skip);
    }

    પાછા ઉમેરો;
}

/*
 * Get last inserted string, and remove trailing <Esc>.
 * Returns pointer to allocated memory (must be freed) or NULL.
 */
    char_u *
get_last_ઉમેરો_save(શૂન્ય)
{
    string_T	ઉમેરો = get_last_ઉમેરો();
    char_u	*s;

    જો (ઉમેરો.string == ખાલી)
	પાછા ખાલી;
    s = vim_strnsave(ઉમેરો.string, ઉમેરો.length);
    જો (s == ખાલી)
	પાછા ખાલી;

    જો (ઉમેરો.length > 0 && s[ઉમેરો.length - 1] == ESC)	// remove trailing ESC
	s[--ઉમેરો.length] = NUL;
    પાછા s;
}

/*
 * Check the word in front of the cursor for an abbreviation.
 * Called when the non-id character "c" has been entered.
 * When an abbreviation is recognized it is removed from the text and
 * the replacement string is inserted in typebuf.tb_buf[], followed by "c".
 */
    સ્થિર પૂર્ણાંક
echeck_abbr(પૂર્ણાંક c)
{
    // Don't check for abbreviation in paste mode, when disabled and just
    // after moving around with cursor keys.
    જો (p_paste || no_abbr || arrow_used)
	પાછા FALSE;

    પાછા check_abbr(c, ml_get_curline(), curwin->w_cursor.col,
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

સ્થિર char_u	*replace_ઢગલો = ખાલી;
સ્થિર long	replace_ઢગલો_nr = 0;	    // next entry in replace stack
સ્થિર long	replace_ઢગલો_len = 0;	    // max. number of entries

    શૂન્ય
replace_ધકેલો(
    પૂર્ણાંક	    c)	    // character that is replaced (NUL is none)
{
    char_u  *p;

    જો (replace_ઢગલો_nr < replace_offset)	// nothing to do
	પાછા;
    જો (replace_ઢગલો_len <= replace_ઢગલો_nr)
    {
	replace_ઢગલો_len += 50;
	p = ALLOC_MULT(char_u, replace_ઢગલો_len);
	જો (p == ખાલી)	    // out of memory
	{
	    replace_ઢગલો_len -= 50;
	    પાછા;
	}
	જો (replace_ઢગલો != ખાલી)
	{
	    mch_memmove(p, replace_ઢગલો,
				 (size_t)(replace_ઢગલો_nr * કદ(char_u)));
	    vim_free(replace_ઢગલો);
	}
	replace_ઢગલો = p;
    }
    p = replace_ઢગલો + replace_ઢગલો_nr - replace_offset;
    જો (replace_offset)
	mch_memmove(p + 1, p, (size_t)(replace_offset * કદ(char_u)));
    *p = c;
    ++replace_ઢગલો_nr;
}

/*
 * Push a character onto the replace stack.  Handles a multi-byte character in
 * reverse byte order, so that the first byte is popped off first.
 * Return the number of bytes done (includes composing characters).
 */
    પૂર્ણાંક
replace_ધકેલો_mb(char_u *p)
{
    પૂર્ણાંક l = (*mb_ptr2len)(p);
    પૂર્ણાંક j;

    માટે (j = l - 1; j >= 0; --j)
	replace_ધકેલો(p[j]);
    પાછા l;
}

/*
 * Pop one item from the replace stack.
 * return -1 if stack empty
 * return replaced character or NUL otherwise
 */
    સ્થિર પૂર્ણાંક
replace_કાઢો(શૂન્ય)
{
    જો (replace_ઢગલો_nr == 0)
	પાછા -1;
    પાછા (પૂર્ણાંક)replace_ઢગલો[--replace_ઢગલો_nr];
}

/*
 * Join the top two items on the replace stack.  This removes to "off"'th NUL
 * encountered.
 */
    શૂન્ય
replace_join(
    પૂર્ણાંક	    off)	// offset for which NUL to remove
{
    પૂર્ણાંક	    i;

    માટે (i = replace_ઢગલો_nr; --i >= 0; )
	જો (replace_ઢગલો[i] == NUL && off-- <= 0)
	{
	    --replace_ઢગલો_nr;
	    mch_memmove(replace_ઢગલો + i, replace_ઢગલો + i + 1,
					      (size_t)(replace_ઢગલો_nr - i));
	    પાછા;
	}
}

/*
 * Pop bytes from the replace stack until a NUL is found, and insert them
 * before the cursor.  Can only be used in MODE_REPLACE or MODE_VREPLACE state.
 */
    સ્થિર શૂન્ય
replace_કાઢો_ins(શૂન્ય)
{
    પૂર્ણાંક	    cc;
    પૂર્ણાંક	    oldState = State;

    State = MODE_NORMAL;			// don't want MODE_REPLACE here
    જ્યારે ((cc = replace_કાઢો()) > 0)
    {
	mb_replace_કાઢો_ins(cc);
	dec_cursor();
    }
    State = oldState;
}

/*
 * Insert bytes popped from the replace stack. "cc" is the first byte.  If it
 * indicates a multi-byte char, pop the other bytes too.
 */
    સ્થિર શૂન્ય
mb_replace_કાઢો_ins(પૂર્ણાંક cc)
{
    પૂર્ણાંક		n;
    char_u	buf[MB_MAXBYTES + 1];
    પૂર્ણાંક		i;
    પૂર્ણાંક		c;

    જો (has_mbyte && (n = MB_BYTE2LEN(cc)) > 1)
    {
	buf[0] = cc;
	માટે (i = 1; i < n; ++i)
	    buf[i] = replace_કાઢો();
	ins_bytes_len(buf, n);
    }
    નહીંતર
	ins_char(cc);

    જો (enc_utf8)
	// Handle composing chars.
	માટે (;;)
	{
	    c = replace_કાઢો();
	    જો (c == -1)	    // stack empty
		તોડો;
	    જો ((n = MB_BYTE2LEN(c)) == 1)
	    {
		// Not a multi-byte char, put it back.
		replace_ધકેલો(c);
		તોડો;
	    }

	    buf[0] = c;
	    માટે (i = 1; i < n; ++i)
		buf[i] = replace_કાઢો();
	    જો (utf_iscomposing(utf_ptr2char(buf)))
		ins_bytes_len(buf, n);
	    નહીંતર
	    {
		// Not a composing char, put it back.
		માટે (i = n - 1; i >= 0; --i)
		    replace_ધકેલો(buf[i]);
		તોડો;
	    }

	}
}

/*
 * make the replace stack empty
 * (called when exiting replace mode)
 */
    સ્થિર શૂન્ય
replace_flush(શૂન્ય)
{
    VIM_CLEAR(replace_ઢગલો);
    replace_ઢગલો_len = 0;
    replace_ઢગલો_nr = 0;
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
    સ્થિર શૂન્ય
replace_do_bs(પૂર્ણાંક limit_col)
{
    પૂર્ણાંક		cc;
    પૂર્ણાંક		orig_len = 0;
    પૂર્ણાંક		ins_len;
    પૂર્ણાંક		orig_vcols = 0;
    colnr_T	start_vcol;
    char_u	*p;
    પૂર્ણાંક		i;
    પૂર્ણાંક		vcol;

    cc = replace_કાઢો();
    જો (cc > 0)
    {
#ifdef FEAT_PROP_POPUP
	size_t	len_before = 0;  // init to shut up GCC

	જો (curbuf->b_has_textprop)
	{
	    // Do not adjust text properties for individual delete and insert
	    // operations, do it afterwards on the resulting text.
	    len_before = ml_get_curline_len();
	    ++text_prop_frozen;
	}
#endif
	જો (State & VREPLACE_FLAG)
	{
	    // Get the number of screen cells used by the character we are
	    // going to delete.
	    getvcol(curwin, &curwin->w_cursor, ખાલી, &start_vcol, ખાલી, 0);
	    orig_vcols = chartabsize(ml_get_cursor(), start_vcol);
	}
	જો (has_mbyte)
	{
	    (શૂન્ય)del_char_after_col(limit_col);
	    જો (State & VREPLACE_FLAG)
		orig_len = ml_get_cursor_len();
	    replace_ધકેલો(cc);
	}
	નહીંતર
	{
	    pchar_cursor(cc);
	    જો (State & VREPLACE_FLAG)
		orig_len = ml_get_cursor_len() - 1;
	}
	replace_કાઢો_ins();

	જો (State & VREPLACE_FLAG)
	{
	    // Get the number of screen cells used by the inserted characters
	    p = ml_get_cursor();
	    ins_len = ml_get_cursor_len() - orig_len;
	    vcol = start_vcol;
	    માટે (i = 0; i < ins_len; ++i)
	    {
		vcol += chartabsize(p + i, vcol);
		i += (*mb_ptr2len)(p + i) - 1;
	    }
	    vcol -= start_vcol;

	    // Delete spaces that were inserted after the cursor to keep the
	    // text aligned.
	    curwin->w_cursor.col += ins_len;
	    જ્યારે (vcol > orig_vcols && gchar_cursor() == ' ')
	    {
		del_char(FALSE);
		++orig_vcols;
	    }
	    curwin->w_cursor.col -= ins_len;
	}

	// mark the buffer as changed and prepare for displaying
	changed_bytes(curwin->w_cursor.lnum, curwin->w_cursor.col);

#ifdef FEAT_PROP_POPUP
	જો (curbuf->b_has_textprop)
	{
	    size_t len_now = ml_get_curline_len();

	    --text_prop_frozen;
	    adjust_prop_columns(curwin->w_cursor.lnum, curwin->w_cursor.col,
					   (પૂર્ણાંક)(len_now - len_before), 0);
	}
#endif
    }
    નહીંતર જો (cc == 0)
	(શૂન્ય)del_char_after_col(limit_col);
}

#જો defined(FEAT_RIGHTLEFT)
/*
 * Map Hebrew keyboard when in hkmap mode.
 */
    પૂર્ણાંક
hkmap(પૂર્ણાંક c)
{
    જો (p_hkmapp)   // phonetic mapping, by Ilya Dogolazky
    {
	ગણના {hALEF=0, BET, GIMEL, DALET, HEI, VAV, ZAIN, HET, TET, IUD,
	    KAFsofit, hKAF, LAMED, MEMsofit, MEM, NUNsofit, NUN, SAMEH, AIN,
	    PEIsofit, PEI, ZADIsofit, ZADI, KOF, RESH, hSHIN, TAV};
	સ્થિર char_u map[26] =
	    {(char_u)hALEF/*a*/, (char_u)BET  /*b*/, (char_u)hKAF    /*c*/,
	     (char_u)DALET/*d*/, (char_u)-1   /*e*/, (char_u)PEIsofit/*f*/,
	     (char_u)GIMEL/*g*/, (char_u)HEI  /*h*/, (char_u)IUD     /*i*/,
	     (char_u)HET  /*j*/, (char_u)KOF  /*k*/, (char_u)LAMED   /*l*/,
	     (char_u)MEM  /*m*/, (char_u)NUN  /*n*/, (char_u)SAMEH   /*o*/,
	     (char_u)PEI  /*p*/, (char_u)-1   /*q*/, (char_u)RESH    /*r*/,
	     (char_u)ZAIN /*s*/, (char_u)TAV  /*t*/, (char_u)TET     /*u*/,
	     (char_u)VAV  /*v*/, (char_u)hSHIN/*w*/, (char_u)-1      /*x*/,
	     (char_u)AIN  /*y*/, (char_u)ZADI /*z*/};

	જો (c == 'N' || c == 'M' || c == 'P' || c == 'C' || c == 'Z')
	    પાછા (પૂર્ણાંક)(map[CharOrd(c)] - 1 + p_aleph);
							    // '-1'='sofit'
	નહીંતર જો (c == 'x')
	    પાછા 'X';
	નહીંતર જો (c == 'q')
	    પાછા '\''; // {geresh}={'}
	નહીંતર જો (c == 246)
	    પાછા ' ';  // \"o --> ' ' for a german keyboard
	નહીંતર જો (c == 228)
	    પાછા ' ';  // \"a --> ' '      -- / --
	નહીંતર જો (c == 252)
	    પાછા ' ';  // \"u --> ' '      -- / --
	// NOTE: islower() does not do the right thing for us on Linux so we
	// do this the same was as 5.7 and previous, so it works correctly on
	// all systems.  Specifically, the e.g. Delete and Arrow keys are
	// munged and won't work if e.g. searching for Hebrew text.
	નહીંતર જો (c >= 'a' && c <= 'z')
	    પાછા (પૂર્ણાંક)(map[Char_Ord_નીચલું(c)] + p_aleph);
	નહીંતર
	    પાછા c;
    }
    નહીંતર
    {
	પસંદગી (c)
	{
	    સ્થિતિ '`':	પાછા ';';
	    સ્થિતિ '/':	પાછા '.';
	    સ્થિતિ '\'':	પાછા ',';
	    સ્થિતિ 'q':	પાછા '/';
	    સ્થિતિ 'w':	પાછા '\'';

			// Hebrew letters - set offset from 'a'
	    સ્થિતિ ',':	c = '{'; તોડો;
	    સ્થિતિ '.':	c = 'v'; તોડો;
	    સ્થિતિ ';':	c = 't'; તોડો;
	    મૂળભૂત: {
			 સ્થિર અક્ષર str[] = "zqbcxlsjphmkwonu ydafe rig";

			 જો (c < 'a' || c > 'z')
			     પાછા c;
			 c = str[Char_Ord_નીચલું(c)];
			 તોડો;
		     }
	}

	પાછા (પૂર્ણાંક)(Char_Ord_નીચલું(c) + p_aleph);
    }
}
#endif

    સ્થિર શૂન્ય
ins_reg(શૂન્ય)
{
    પૂર્ણાંક		need_redraw = FALSE;
    પૂર્ણાંક		regname;
    પૂર્ણાંક		literally = 0;
    પૂર્ણાંક		vis_active = VIsual_active;

    /*
     * If we are going to wait for a character, show a '"'.
     */
    pc_status = PC_STATUS_UNSET;
    જો (redrawing() && !char_avail())
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
    ++allow_ચાવીઓ;
    regname = plain_vgetc();
    LANGMAP_ADJUST(regname, TRUE);
    જો (regname == Ctrl_R || regname == Ctrl_O || regname == Ctrl_P)
    {
	// Get a third key for literal register insertion
	literally = regname;
	add_to_showcmd_c(literally);
	regname = plain_vgetc();
	LANGMAP_ADJUST(regname, TRUE);
    }
    --no_mapping;
    --allow_ચાવીઓ;

#ifdef FEAT_EVAL
    // Don't call u_sync() while typing the expression or giving an error
    // message for it. Only call it explicitly.
    ++no_u_sync;
    જો (regname == '=')
    {
	pos_T	curpos = curwin->w_cursor;
# ifdef HAVE_આગત_METHOD
	પૂર્ણાંક	im_on = im_get_status();
# endif
	// Sync undo when evaluating the expression calls setline() or
	// append(), so that it can be undone separately.
	u_sync_once = 2;

	regname = get_પદાવલિ_register();

	// Cursor may be moved back a column.
	curwin->w_cursor = curpos;
	check_cursor();
# ifdef HAVE_આગત_METHOD
	// Restore the Input Method.
	જો (im_on)
	    im_set_active(TRUE);
# endif
    }
    જો (regname == NUL || !valid_yank_reg(regname, FALSE))
    {
	vim_beep(BO_REG);
	need_redraw = TRUE;	// remove the '"'
    }
    નહીંતર
    {
#endif
	જો (literally == Ctrl_O || literally == Ctrl_P)
	{
	    // Append the command to the redo buffer.
	    AppendCharToRedobuff(Ctrl_R);
	    AppendCharToRedobuff(literally);
	    AppendCharToRedobuff(regname);

	    do_put(regname, ખાલી, BACKWARD, 1L,
		 (literally == Ctrl_P ? PUT_FIXINDENT : 0) | PUT_CURSEND);
	}
	નહીંતર જો (ઉમેરો_reg(regname, literally) == FAIL)
	{
	    vim_beep(BO_REG);
	    need_redraw = TRUE;	// remove the '"'
	}
	નહીંતર જો (stop_ઉમેરો_mode)
	    // When the '=' register was used and a function was invoked that
	    // did ":stopinsert" then stuff_empty() returns FALSE but we won't
	    // insert anything, need to remove the '"'
	    need_redraw = TRUE;

#ifdef FEAT_EVAL
    }
    --no_u_sync;
    જો (u_sync_once == 1)
	ins_need_undo = TRUE;
    u_sync_once = 0;
#endif
    clear_showcmd();

    // If the inserted register is empty, we need to remove the '"'
    જો (need_redraw || stuff_empty())
	edit_unputchar();

    // Disallow starting Visual mode here, would get a weird mode.
    જો (!vis_active && VIsual_active)
	end_visual_mode();
}

/*
 * CTRL-G commands in Insert mode.
 */
    સ્થિર શૂન્ય
ins_ctrl_g(શૂન્ય)
{
    પૂર્ણાંક		c;

    // Right after CTRL-X the cursor will be after the ruler.
    setcursor();

    /*
     * Don't map the second key. This also prevents the mode message to be
     * deleted when ESC is hit.
     */
    ++no_mapping;
    ++allow_ચાવીઓ;
    c = plain_vgetc();
    --no_mapping;
    --allow_ચાવીઓ;
    પસંદગી (c)
    {
	// CTRL-G k and CTRL-G <Up>: cursor up to Insstart.col
	સ્થિતિ K_UP:
	સ્થિતિ Ctrl_K:
	સ્થિતિ 'k': ins_up(TRUE);
		  તોડો;

	// CTRL-G j and CTRL-G <Down>: cursor down to Insstart.col
	સ્થિતિ K_DOWN:
	સ્થિતિ Ctrl_J:
	સ્થિતિ 'j': ins_down(TRUE);
		  તોડો;

	// CTRL-G u: start new undoable edit
	સ્થિતિ 'u': u_sync(TRUE);
		  ins_need_undo = TRUE;

		  // Need to reset Insstart, esp. because a BS that joins
		  // a line to the previous one must save for undo.
		  update_Insstart_orig = FALSE;
		  Insstart = curwin->w_cursor;
		  તોડો;

	// CTRL-G U: do not break undo with the next char
	સ્થિતિ 'U':
		  // Allow one left/right cursor movement with the next char,
		  // without breaking undo.
		  dont_sync_undo = MAYBE;
		  તોડો;

	સ્થિતિ ESC:
		  // Esc after CTRL-G cancels it.
		  તોડો;

	// Unknown CTRL-G command, reserved for future expansion.
	મૂળભૂત:  vim_beep(BO_CTRLG);
    }
}

/*
 * CTRL-^ in Insert mode.
 */
    સ્થિર શૂન્ય
ins_ctrl_hat(શૂન્ય)
{
    જો (map_to_exists_mode((char_u *)"", MODE_LANGMAP, FALSE))
    {
	// ":lmap" mappings exists, Toggle use of ":lmap" mappings.
	જો (State & MODE_LANGMAP)
	{
	    curbuf->b_p_iminsert = B_IMODE_NONE;
	    State &= ~MODE_LANGMAP;
	}
	નહીંતર
	{
	    curbuf->b_p_iminsert = B_IMODE_LMAP;
	    State |= MODE_LANGMAP;
#ifdef HAVE_આગત_METHOD
	    im_set_active(FALSE);
#endif
	}
    }
#ifdef HAVE_આગત_METHOD
    નહીંતર
    {
	// There are no ":lmap" mappings, toggle IM
	જો (im_get_status())
	{
	    curbuf->b_p_iminsert = B_IMODE_NONE;
	    im_set_active(FALSE);
	}
	નહીંતર
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
    જો (gui.in_use)
	gui_update_cursor(TRUE, FALSE);
#endif
#જો defined(FEAT_KEYMAP)
    // Show/unshow value of 'keymap' in status lines.
    status_redraw_curbuf();
#endif
}

/*
 * Handle ESC in insert mode.
 * Returns TRUE when leaving insert mode, FALSE when going to repeat the
 * insert.
 */
    સ્થિર પૂર્ણાંક
ins_esc(
    long	*ગણતરી,
    પૂર્ણાંક		cmdchar,
    પૂર્ણાંક		nomove)	    // don't move cursor
{
    પૂર્ણાંક		temp;
    સ્થિર પૂર્ણાંક	disabled_redraw = FALSE;
#ifdef FEAT_CONCEAL
    // Remember if the cursor line was concealed before changing State.
    પૂર્ણાંક		cursor_લીટી_was_concealed = curwin->w_p_cole > 0
						&& conceal_cursor_લીટી(curwin);
#endif

#ifdef FEAT_SPELL
    check_spell_redraw();
#endif

    temp = curwin->w_cursor.col;
    જો (disabled_redraw)
    {
	જો (RedrawingDisabled > 0)
	    --RedrawingDisabled;
	disabled_redraw = FALSE;
    }
    જો (!arrow_used)
    {
	/*
	 * Don't append the ESC for "r<CR>" and "grx".
	 * When 'insertmode' is set only CTRL-L stops Insert mode.  Needed for
	 * when "count" is non-zero.
	 */
	જો (cmdchar != 'r' && cmdchar != 'v')
	    AppendToRedobuff(p_im ? (char_u *)"\014" : ESC_STR);

	/*
	 * Repeating insert may take a long time.  Check for
	 * interrupt now and then.
	 */
	જો (*ગણતરી > 0)
	{
	    લીટી_breakcheck();
	    જો (got_int)
		*ગણતરી = 0;
	}

	જો (--*ગણતરી > 0)	// repeat what was typed
	{
	    // Vi repeats the insert without replacing characters.
	    જો (vim_strchr(p_cpo, CPO_REPLCNT) != ખાલી)
		State &= ~REPLACE_FLAG;

	    (શૂન્ય)start_redo_ins();
	    જો (cmdchar == 'r' || cmdchar == 'v')
		stuffRedoReadbuff(ESC_STR);	// no ESC in redo buffer
	    ++RedrawingDisabled;
	    disabled_redraw = TRUE;
	    પાછા FALSE;	// repeat the insert
	}
	stop_ઉમેરો(&curwin->w_cursor, TRUE, nomove);
	undisplay_dollar();
    }

    જો (cmdchar != 'r' && cmdchar != 'v')
	ins_apply_autocmds(EVENT_INSERTLEAVEPRE);

    // When an autoindent was removed, curswant stays after the
    // indent
    જો (restart_edit == NUL && (colnr_T)temp == curwin->w_cursor.col)
	curwin->w_set_curswant = true;

    // Remember the last Insert position in the '^ mark.
    જો ((cmdmod.cmod_flags & CMOD_KEEPJUMPS) == 0)
	curbuf->b_last_ઉમેરો = curwin->w_cursor;

    /*
     * The cursor should end up on the last inserted character.
     * Don't do it for CTRL-O, unless past the end of the line.
     */
    જો (!nomove
	    && (curwin->w_cursor.col != 0
		|| curwin->w_cursor.coladd > 0)
	    && (restart_edit == NUL
		   || (gchar_cursor() == NUL && !VIsual_active))
#ifdef FEAT_RIGHTLEFT
	    && !revins_on
#endif
				      )
    {
	જો (curwin->w_cursor.coladd > 0 || get_ve_flags() == VE_ALL)
	{
	    oneleft();
	    જો (restart_edit != NUL)
		++curwin->w_cursor.coladd;
	}
	નહીંતર
	{
	    --curwin->w_cursor.col;
	    curwin->w_valid &= ~(VALID_WCOL|VALID_VIRTCOL);
	    // Correct cursor for multi-byte character.
	    જો (has_mbyte)
		mb_adjust_cursor();
	}
    }

#ifdef HAVE_આગત_METHOD
    // Disable IM to allow typing English directly for Normal mode commands.
    // When ":lmap" is enabled don't change 'iminsert' (IM can be enabled as
    // well).
    જો (!(State & MODE_LANGMAP))
	im_save_status(&curbuf->b_p_iminsert);
    im_set_active(FALSE);
#endif

    State = MODE_NORMAL;
    may_trigger_modechanged();
    // need to position cursor again when on a TAB and when on a char with
    // virtual text.
    જો (gchar_cursor() == TAB
#ifdef FEAT_PROP_POPUP
	    || curbuf->b_has_textprop
#endif
       )
	curwin->w_valid &= ~(VALID_WROW|VALID_WCOL|VALID_VIRTCOL);

    setmouse();
#ifdef CURSOR_SHAPE
    ui_cursor_shape();		// may show different cursor shape
#endif
    જો (!p_ek)
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
    conceal_check_cursor_લીટી(cursor_લીટી_was_concealed);
#endif

    // When recording or for CTRL-O, need to display the new mode.
    // Otherwise remove the mode message.
    જો (reg_recording != 0 || restart_edit != NUL)
	showmode();
    નહીંતર જો (p_smd && (got_int || !skip_showmode()))
	સંદેશ("");

    પાછા TRUE;	    // exit Insert mode
}

#ifdef FEAT_RIGHTLEFT
/*
 * Toggle language: hkmap and revins_on.
 * Move to end of reverse inserted text.
 */
    સ્થિર શૂન્ય
ins_ctrl_(શૂન્ય)
{
    જો (revins_on && revins_chars && revins_scol >= 0)
    {
	જ્યારે (gchar_cursor() != NUL && revins_chars--)
	    ++curwin->w_cursor.col;
    }
    p_ri = !p_ri;
    revins_on = (State == MODE_ઉમેરો && p_ri);
    જો (revins_on)
    {
	revins_scol = curwin->w_cursor.col;
	revins_legal++;
	revins_chars = 0;
	undisplay_dollar();
    }
    નહીંતર
	revins_scol = -1;
    p_hkmap = curwin->w_p_rl ^ p_ri;    // be consistent!
    showmode();
}
#endif

/*
 * If 'keymodel' contains "startsel", may start selection.
 * Returns TRUE when a CTRL-O and other keys stuffed.
 */
    સ્થિર પૂર્ણાંક
ins_start_select(પૂર્ણાંક c)
{
    જો (!km_startsel)
	પાછા FALSE;
    પસંદગી (c)
    {
	સ્થિતિ K_KHOME:
	સ્થિતિ K_KEND:
	સ્થિતિ K_PAGEUP:
	સ્થિતિ K_KPAGEUP:
	સ્થિતિ K_PAGEDOWN:
	સ્થિતિ K_KPAGEDOWN:
#ifdef MACOS_X
	સ્થિતિ K_ડાબું:
	સ્થિતિ K_જમણું:
	સ્થિતિ K_UP:
	સ્થિતિ K_DOWN:
	સ્થિતિ K_END:
	સ્થિતિ K_HOME:
#endif
	    જો (!(mod_mask & MOD_MASK_SHIFT))
		તોડો;
	    // FALLTHROUGH
	સ્થિતિ K_S_ડાબું:
	સ્થિતિ K_S_જમણું:
	સ્થિતિ K_S_UP:
	સ્થિતિ K_S_DOWN:
	સ્થિતિ K_S_END:
	સ્થિતિ K_S_HOME:
	    // Start selection right away, the cursor can move with CTRL-O when
	    // beyond the end of the line.
	    start_selection();

	    // Execute the key in (insert) Select mode.
	    stuffcharReadbuff(Ctrl_O);
	    જો (mod_mask)
	    {
		char_u	    buf[4];

		buf[0] = K_SPECIAL;
		buf[1] = KS_MODIFIER;
		buf[2] = mod_mask;
		buf[3] = NUL;
		stuffReadbuffLen(buf, 3L);
	    }
	    stuffcharReadbuff(c);
	    પાછા TRUE;
    }
    પાછા FALSE;
}

/*
 * <Insert> key in Insert mode: toggle insert/replace mode.
 */
    સ્થિર શૂન્ય
ins_ઉમેરો(પૂર્ણાંક replaceState)
{
#ifdef FEAT_EVAL
    set_vim_var_string(VV_INSERTMODE,
		   (char_u *)((State & REPLACE_FLAG) ? "i"
			    : replaceState == MODE_VREPLACE ? "v" : "r"), 1);
#endif
    ins_apply_autocmds(EVENT_INSERTCHANGE);
    જો (State & REPLACE_FLAG)
	State = MODE_ઉમેરો | (State & MODE_LANGMAP);
    નહીંતર
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
    સ્થિર શૂન્ય
ins_ctrl_o(શૂન્ય)
{
    restart_VIsual_select = 0;
    જો (State & VREPLACE_FLAG)
	restart_edit = 'V';
    નહીંતર જો (State & REPLACE_FLAG)
	restart_edit = 'R';
    નહીંતર
	restart_edit = 'I';
    જો (virtual_active())
	ins_at_eol = FALSE;	// cursor always keeps its column
    નહીંતર
	ins_at_eol = (gchar_cursor() == NUL);
}

/*
 * If the cursor is on an indent, ^T/^D insert/delete one
 * shiftwidth.	Otherwise ^T/^D behave like a "<<" or ">>".
 * Always round the indent to 'shiftwidth', this is compatible
 * with vi.  But vi only supports ^T and ^D after an
 * autoindent, we support it everywhere.
 */
    સ્થિર શૂન્ય
ins_shift(પૂર્ણાંક c, પૂર્ણાંક lastc)
{
    જો (stop_arrow() == FAIL)
	પાછા;
    AppendCharToRedobuff(c);

    /*
     * 0^D and ^^D: remove all indent.
     */
    જો (c == Ctrl_D && (lastc == '0' || lastc == '^')
						  && curwin->w_cursor.col > 0)
    {
	--curwin->w_cursor.col;
	(શૂન્ય)del_char(FALSE);		// delete the '^' or '0'
	// In Replace mode, restore the characters that '^' or '0' replaced.
	જો (State & REPLACE_FLAG)
	    replace_કાઢો_ins();
	જો (lastc == '^')
	    old_indent = get_indent();	// remember curr. indent
	change_indent(INDENT_SET, 0, TRUE, 0, TRUE);
    }
    નહીંતર
	change_indent(c == Ctrl_D ? INDENT_DEC : INDENT_INC, 0, TRUE, 0, TRUE);

    જો (did_ai && *skipwhite(ml_get_curline()) != NUL)
	did_ai = FALSE;
    did_si = FALSE;
    can_si = FALSE;
    can_si_back = FALSE;
    can_cindent = FALSE;	// no cindenting after ^D or ^T
}

    સ્થિર શૂન્ય
ins_del(શૂન્ય)
{
    પૂર્ણાંક	    temp;

    જો (stop_arrow() == FAIL)
	પાછા;
    જો (gchar_cursor() == NUL)		// delete newline
    {
	temp = curwin->w_cursor.col;
	જો (!can_bs(BS_EOL)		// only if "eol" included
		|| do_join(2, FALSE, TRUE, FALSE, FALSE) == FAIL)
	    vim_beep(BO_BS);
	નહીંતર
	{
	    curwin->w_cursor.col = temp;
	    // Adjust orig_line_count in case more lines have been deleted than
	    // have been added. That makes sure, that open_line() later
	    // can access all buffer lines correctly
	    જો (State & VREPLACE_FLAG &&
		    orig_લીટી_ગણતરી > curbuf->b_ml.ml_લીટી_ગણતરી)
		orig_લીટી_ગણતરી = curbuf->b_ml.ml_લીટી_ગણતરી;
	}
    }
    નહીંતર જો (del_char(FALSE) == FAIL)  // delete char under cursor
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
    સ્થિર શૂન્ય
ins_bs_one(શૂન્ય)
{
    dec_cursor();
    જો (State & REPLACE_FLAG)
    {
	// Don't delete characters before the insert point when in
	// Replace mode
	જો (curwin->w_cursor.lnum != Insstart.lnum
		|| curwin->w_cursor.col >= Insstart.col)
	    replace_do_bs(-1);
    }
    નહીંતર
	(શૂન્ય)del_char(FALSE);
}

/*
 * Handle Backspace, delete-word and delete-line in Insert mode.
 * Return TRUE when backspace was actually used.
 */
    સ્થિર પૂર્ણાંક
ins_bs(
    પૂર્ણાંક		c,
    પૂર્ણાંક		mode,
    પૂર્ણાંક		*inserted_space_p)
{
    linenr_T	lnum;
    પૂર્ણાંક		cc;
    પૂર્ણાંક		temp = 0;	    // init for GCC
    colnr_T	save_col;
    colnr_T	mincol;
    પૂર્ણાંક		did_backspace = FALSE;
    પૂર્ણાંક		in_indent;
    પૂર્ણાંક		oldState;
    પૂર્ણાંક		cpc[MAX_MCO];	    // composing characters
    પૂર્ણાંક		call_fix_indent = FALSE;

    /*
     * can't delete anything in an empty file
     * can't backup past first character in buffer
     * can't backup past starting point unless 'backspace' > 1
     * can backup to a previous line if 'backspace' == 0
     */
    જો (       BUFEMPTY()
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
	પાછા FALSE;
    }

    જો (stop_arrow() == FAIL)
	પાછા FALSE;
    in_indent = inindent(0);
    જો (in_indent)
	can_cindent = FALSE;
    end_comment_pending = NUL;	// After BS, don't auto-end comment
#ifdef FEAT_RIGHTLEFT
    જો (revins_on)	    // put cursor after last inserted char
	inc_cursor();
#endif

    // Virtualedit:
    //	BACKSPACE_CHAR eats a virtual space
    //	BACKSPACE_WORD eats all coladd
    //	BACKSPACE_LINE eats all coladd and keeps going
    જો (curwin->w_cursor.coladd > 0)
    {
	જો (mode == BACKSPACE_CHAR)
	{
	    --curwin->w_cursor.coladd;
	    પાછા TRUE;
	}
	જો (mode == BACKSPACE_WORD)
	{
	    curwin->w_cursor.coladd = 0;
	    પાછા TRUE;
	}
	curwin->w_cursor.coladd = 0;
    }

    /*
     * Delete newline!
     */
    જો (curwin->w_cursor.col == 0)
    {
	lnum = Insstart.lnum;
	જો (curwin->w_cursor.lnum == lnum
#ifdef FEAT_RIGHTLEFT
			|| revins_on
#endif
				    )
	{
	    જો (u_save((linenr_T)(curwin->w_cursor.lnum - 2),
			       (linenr_T)(curwin->w_cursor.lnum + 1)) == FAIL)
		પાછા FALSE;
	    --Insstart.lnum;
	    Insstart.col = ml_get_len(Insstart.lnum);
	}
	/*
	 * In replace mode:
	 * cc < 0: NL was inserted, delete it
	 * cc >= 0: NL was replaced, put original characters back
	 */
	cc = -1;
	જો (State & REPLACE_FLAG)
	    cc = replace_કાઢો();	    // returns -1 if NL was inserted
	/*
	 * In replace mode, in the line we started replacing, we only move the
	 * cursor.
	 */
	જો ((State & REPLACE_FLAG) && curwin->w_cursor.lnum <= lnum)
	{
	    dec_cursor();
	}
	નહીંતર
	{
	    જો (!(State & VREPLACE_FLAG)
				   || curwin->w_cursor.lnum > orig_લીટી_ગણતરી)
	    {
		temp = gchar_cursor();	// remember current char
		--curwin->w_cursor.lnum;

		// When "aw" is in 'formatoptions' we must delete the space at
		// the end of the line, otherwise the line will be broken
		// again when auto-formatting.
		જો (has_format_option(FO_AUTO)
					   && has_format_option(FO_WHITE_PAR))
		{
		    char_u  *ptr = ml_get_buf(curbuf, curwin->w_cursor.lnum,
									FALSE);
		    પૂર્ણાંક	    len = ml_get_curline_len();

		    જો (len > 0 && ptr[len - 1] == ' ')
		    {
			char_u *newp = alloc(curbuf->b_ml.ml_લીટી_len - 1);

			જો (newp != ખાલી)
			{
			    mch_memmove(newp, ptr, len - 1);
			    newp[len - 1] = NUL;
			    જો (curbuf->b_ml.ml_લીટી_len > len + 1)
				mch_memmove(newp + len, ptr + len + 1,
					   curbuf->b_ml.ml_લીટી_len - len - 1);

			    જો (curbuf->b_ml.ml_flags
					      & (ML_લીટી_DIRTY | ML_ALLOCATED))
				vim_free(curbuf->b_ml.ml_લીટી_ptr);
			    curbuf->b_ml.ml_લીટી_ptr = newp;
			    curbuf->b_ml.ml_લીટી_len--;
			    curbuf->b_ml.ml_લીટી_textlen--;
			    curbuf->b_ml.ml_flags |= ML_લીટી_DIRTY;
			}
		    }
		}

		(શૂન્ય)do_join(2, FALSE, FALSE, FALSE, FALSE);
		જો (temp == NUL && gchar_cursor() != NUL)
		    inc_cursor();
	    }
	    નહીંતર
		dec_cursor();

	    /*
	     * In MODE_REPLACE mode we have to put back the text that was
	     * replaced by the NL. On the replace stack is first a
	     * NUL-terminated sequence of characters that were deleted and then
	     * the characters that NL replaced.
	     */
	    જો (State & REPLACE_FLAG)
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
		જ્યારે (cc > 0)
		{
		    save_col = curwin->w_cursor.col;
		    mb_replace_કાઢો_ins(cc);
		    curwin->w_cursor.col = save_col;
		    cc = replace_કાઢો();
		}
		// restore the characters that NL replaced
		replace_કાઢો_ins();
		State = oldState;
	    }
	}
	did_ai = FALSE;
    }
    નહીંતર
    {
	/*
	 * Delete character(s) before the cursor.
	 */
#ifdef FEAT_RIGHTLEFT
	જો (revins_on)		// put cursor on last inserted char
	    dec_cursor();
#endif
	mincol = 0;
						// keep indent
	જો (mode == BACKSPACE_લીટી
		&& (curbuf->b_p_ai || cindent_on())
#ifdef FEAT_RIGHTLEFT
		&& !revins_on
#endif
			    )
	{
	    save_col = curwin->w_cursor.col;
	    beginline(BL_WHITE);
	    જો (curwin->w_cursor.col < save_col)
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
	જો (	   mode == BACKSPACE_CHAR
		&& ((p_sta && in_indent)
		    || ((get_sts_કિંમત() != 0
#ifdef FEAT_VARTABS
			|| tabstop_ગણતરી(curbuf->b_p_vsts_array)
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
	    char_u	*લીટી;
	    char_u	*ptr;
	    char_u	*cursor_ptr;
	    char_u	*space_ptr;
	    colnr_T	space_vcol = 0;
	    પૂર્ણાંક		prev_space = FALSE;
	    colnr_T	want_col;

	    *inserted_space_p = FALSE;

	    space_ptr = ptr = લીટી = ml_get_curline();
	    cursor_ptr = લીટી + curwin->w_cursor.col;

	    // Compute virtual column of cursor position, and find the last
	    // whitespace before cursor that is preceded by non-whitespace.
	    // Use chartabsize() so that virtual text and wrapping are ignored.
	    જ્યારે (ptr < cursor_ptr)
	    {
		પૂર્ણાંક	cur_space = VIM_ISWHITE(*ptr);

		જો (!prev_space && cur_space)
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
	    જો (p_sta && in_indent)
		want_vcol -= want_vcol % (પૂર્ણાંક)get_sw_કિંમત(curbuf);
	    નહીંતર
#ifdef FEAT_VARTABS
		want_vcol = tabstop_start(want_vcol, get_sts_કિંમત(),
						       curbuf->b_p_vsts_array);
#નહીંતર
		want_vcol -= want_vcol % (પૂર્ણાંક)get_sts_કિંમત();
#endif

	    // Find the position to stop backspacing.
	    // Use chartabsize() so that virtual text and wrapping are ignored.
	    જ્યારે (TRUE)
	    {
		પૂર્ણાંક size = chartabsize(space_ptr, space_vcol);

		જો (space_vcol + size > want_vcol)
		    તોડો;
		space_vcol += size;
		MB_PTR_ADV(space_ptr);
	    }
	    want_col = space_ptr - લીટી;

	    // Delete characters until we are at or before want_col.
	    જ્યારે (curwin->w_cursor.col > want_col)
		ins_bs_one();

	    // Insert extra spaces until we are at want_vcol.
	    માટે (; space_vcol < want_vcol; space_vcol++)
	    {
		// Remember the first char we inserted
		જો (curwin->w_cursor.lnum == Insstart_orig.lnum
				   && curwin->w_cursor.col < Insstart_orig.col)
		    Insstart_orig.col = curwin->w_cursor.col;

		જો (State & VREPLACE_FLAG)
		    ins_char(' ');
		નહીંતર
		{
		    ins_str((char_u *)" ", 1);
		    જો ((State & REPLACE_FLAG))
			replace_ધકેલો(NUL);
		}
	    }
	}

	/*
	 * Delete up to starting point, start of line or previous word.
	 */
	નહીંતર
	{
	    પૂર્ણાંક cclass = 0, prev_cclass = 0;

	    જો (has_mbyte)
		cclass = mb_get_class(ml_get_cursor());
	    કરો
	    {
#ifdef FEAT_RIGHTLEFT
		જો (!revins_on) // put cursor on char to be deleted
#endif
		    dec_cursor();

		cc = gchar_cursor();
		// look multi-byte character class
		જો (has_mbyte)
		{
		    prev_cclass = cclass;
		    cclass = mb_get_class(ml_get_cursor());
		}

		// start of word?
		જો (mode == BACKSPACE_WORD && !vim_isspace(cc))
		{
		    mode = BACKSPACE_WORD_NOT_SPACE;
		    temp = vim_iswordc(cc);
		}
		// end of word?
		નહીંતર જો (mode == BACKSPACE_WORD_NOT_SPACE
			&& ((vim_isspace(cc) || vim_iswordc(cc) != temp)
			|| prev_cclass != cclass))
		{
#ifdef FEAT_RIGHTLEFT
		    જો (!revins_on)
#endif
			inc_cursor();
#ifdef FEAT_RIGHTLEFT
		    નહીંતર જો (State & REPLACE_FLAG)
			dec_cursor();
#endif
		    તોડો;
		}
		જો (State & REPLACE_FLAG)
		    replace_do_bs(-1);
		નહીંતર
		{
		    જો (enc_utf8 && p_deco)
			(શૂન્ય)utfc_ptr2char(ml_get_cursor(), cpc);
		    (શૂન્ય)del_char(FALSE);
		    /*
		     * If there are combining characters and 'delcombine' is set
		     * move the cursor back.  Don't back up before the base
		     * character.
		     */
		    જો (enc_utf8 && p_deco && cpc[0] != NUL)
			inc_cursor();
#ifdef FEAT_RIGHTLEFT
		    જો (revins_chars)
		    {
			revins_chars--;
			revins_legal++;
		    }
		    જો (revins_on && gchar_cursor() == NUL)
			તોડો;
#endif
		}
		// Just a single backspace?:
		જો (mode == BACKSPACE_CHAR)
		    તોડો;
	    } જ્યારે (
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
    જો (curwin->w_cursor.col <= 1)
	did_ai = FALSE;

    જો (call_fix_indent)
	fix_indent();

    /*
     * It's a little strange to put backspaces into the redo
     * buffer, but it makes auto-indent a lot easier to deal
     * with.
     */
    AppendCharToRedobuff(c);

    // If deleted before the insertion point, adjust it
    જો (curwin->w_cursor.lnum == Insstart_orig.lnum
				  && curwin->w_cursor.col < Insstart_orig.col)
	Insstart_orig.col = curwin->w_cursor.col;

    // vi behaviour: the cursor moves backward but the character that
    //		     was there remains visible
    // Vim behaviour: the cursor moves backward and the character that
    //		      was there is erased from the screen.
    // We can emulate the vi behaviour by pretending there is a dollar
    // displayed even when there isn't.
    //  --pkv Sun Jan 19 01:56:40 EST 2003
    જો (vim_strchr(p_cpo, CPO_BACKSPACE) != ખાલી && dollar_vcol == -1)
	dollar_vcol = curwin->w_virtcol;

#ifdef FEAT_FOLDING
    // When deleting a char the cursor line must never be in a closed fold.
    // E.g., when 'foldmethod' is indent and deleting the first non-white
    // char before a Tab.
    જો (did_backspace)
	foldOpenCursor();
#endif

    પાછા did_backspace;
}

/*
 * Handle receiving P_PS: start paste mode.  Inserts the following text up to
 * P_PE literally.
 * When "drop" is TRUE then consume the text and drop it.
 */
    પૂર્ણાંક
bracketed_paste(paste_mode_T mode, પૂર્ણાંક drop, garray_T *gap)
{
    પૂર્ણાંક		c;
    char_u	buf[NUMBUFLEN + MB_MAXBYTES];
    પૂર્ણાંક		idx = 0;
    char_u	*end = find_termcode((char_u *)"PE");
    પૂર્ણાંક		ret_char = -1;
    પૂર્ણાંક		save_allow_ચાવીઓ = allow_ચાવીઓ;
    પૂર્ણાંક		save_paste = p_paste;

    // If the end code is too long we can't detect it, read everything.
    જો (end != ખાલી && STRLEN(end) >= NUMBUFLEN)
	end = ખાલી;
    ++no_mapping;
    allow_ચાવીઓ = 0;
    જો (!p_paste)
	// Also have the side effects of setting 'paste' to make it work much
	// faster.
	set_option_કિંમત_give_err((char_u *)"paste", TRUE, ખાલી, 0);

    માટે (;;)
    {
	// When the end is not defined read everything there is.
	જો (end == ખાલી && vpeekc() == NUL)
	    તોડો;
	કરો
	    c = vgetc();
	જ્યારે (c == K_IGNORE || c == K_VER_SCROLLBAR || c == K_HOR_SCROLLBAR);

	જો (c == NUL || got_int || (ex_normal_busy > 0 && c == Ctrl_C))
	    // When CTRL-C was encountered the typeahead will be flushed and we
	    // won't get the end sequence.  Except when using ":normal".
	    તોડો;

	જો (has_mbyte)
	    idx += (*mb_char2bytes)(c, buf + idx);
	નહીંતર
	    buf[idx++] = c;
	buf[idx] = NUL;
	જો (end != ખાલી && STRNCMP(buf, end, idx) == 0)
	{
	    જો (end[idx] == NUL)
		તોડો; // Found the end of paste code.
	    ચાલુ;
	}
	જો (!drop)
	{
	    પસંદગી (mode)
	    {
		સ્થિતિ PASTE_CMDLINE:
		    put_on_cmdline(buf, idx, TRUE);
		    તોડો;

		સ્થિતિ PASTE_EX:
		    // add one for the NUL that is going to be appended
		    જો (gap != ખાલી && ga_grow(gap, idx + 1) == OK)
		    {
			mch_memmove((અક્ષર *)gap->ga_data + gap->ga_len,
							     buf, (size_t)idx);
			gap->ga_len += idx;
		    }
		    તોડો;

		સ્થિતિ PASTE_ઉમેરો:
		    જો (stop_arrow() == OK)
		    {
			c = buf[0];
			જો (idx == 1 && (c == CAR || c == K_KENTER || c == NL))
			    ins_eol(c);
			નહીંતર
			{
			    ins_char_bytes(buf, idx);
			    AppendToRedobuffLit(buf, idx);
			}
		    }
		    તોડો;

		સ્થિતિ PASTE_ONE_CHAR:
		    જો (ret_char == -1)
		    {
			જો (has_mbyte)
			    ret_char = (*mb_ptr2char)(buf);
			નહીંતર
			    ret_char = buf[0];
		    }
		    તોડો;
	    }
	}
	idx = 0;
    }

    --no_mapping;
    allow_ચાવીઓ = save_allow_ચાવીઓ;
    જો (!save_paste)
	set_option_કિંમત_give_err((char_u *)"paste", FALSE, ખાલી, 0);

    પાછા ret_char;
}

#જો defined(FEAT_GUI_TABLINE)
    સ્થિર શૂન્ય
ins_tabline(પૂર્ણાંક c)
{
    // We will be leaving the current window, unless closing another tab.
    જો (c != K_TABMENU || current_tabmenu != TABLINE_MENU_CLOSE
		|| (current_tab != 0 && current_tab != tabpage_index(curtab)))
    {
	undisplay_dollar();
	start_arrow(&curwin->w_cursor);
	can_cindent = TRUE;
    }

    જો (c == K_TABLINE)
	goto_tabpage(current_tab);
    નહીંતર
    {
	handle_tabmenu();
	redraw_statuslines();	// will redraw the tabline when needed
    }
}
#endif

#જો defined(FEAT_GUI)
    શૂન્ય
ins_scroll(શૂન્ય)
{
    pos_T	tpos;

    undisplay_dollar();
    tpos = curwin->w_cursor;
    જો (gui_do_scroll())
    {
	start_arrow(&tpos);
	can_cindent = TRUE;
    }
}

    શૂન્ય
ins_horscroll(શૂન્ય)
{
    pos_T	tpos;

    undisplay_dollar();
    tpos = curwin->w_cursor;
    જો (do_mousescroll_horiz(scrollbar_કિંમત))
    {
	start_arrow(&tpos);
	can_cindent = TRUE;
    }
}
#endif

    સ્થિર શૂન્ય
ins_ડાબું(શૂન્ય)
{
    pos_T	tpos;
    પૂર્ણાંક		end_change = dont_sync_undo == FALSE; // end undoable change

#ifdef FEAT_FOLDING
    જો ((fdo_flags & FDO_HOR) && ચાવી_Typed)
	foldOpenCursor();
#endif
    undisplay_dollar();
    tpos = curwin->w_cursor;
    જો (oneleft() == OK)
    {
#જો defined(FEAT_XIM) && defined(FEAT_GUI_GTK)
	// Only call start_arrow() when not busy with preediting, it will
	// break undo.  K_LEFT is inserted in im_correct_cursor().
	જો (p_imst == IM_OVER_THE_SPOT || !im_is_preediting())
#endif
	{
	    start_arrow_with_change(&tpos, end_change);
	    જો (!end_change)
		AppendCharToRedobuff(K_ડાબું);
	}
#ifdef FEAT_RIGHTLEFT
	// If exit reversed string, position is fixed
	જો (revins_scol != -1 && (પૂર્ણાંક)curwin->w_cursor.col >= revins_scol)
	    revins_legal++;
	revins_chars++;
#endif
    }

    /*
     * if 'whichwrap' set for cursor in insert mode may go to
     * previous line
     */
    નહીંતર જો (vim_strchr(p_ww, '[') != ખાલી && curwin->w_cursor.lnum > 1)
    {
	// always break undo when moving upwards/downwards, else undo may break
	start_arrow(&tpos);
	--(curwin->w_cursor.lnum);
	coladvance((colnr_T)MAXCOL);
	curwin->w_set_curswant = true;	// so we stay at the end
    }
    નહીંતર
	vim_beep(BO_CRSR);
    dont_sync_undo = FALSE;
}

    સ્થિર શૂન્ય
ins_home(પૂર્ણાંક c)
{
    pos_T	tpos;

#ifdef FEAT_FOLDING
    જો ((fdo_flags & FDO_HOR) && ચાવી_Typed)
	foldOpenCursor();
#endif
    undisplay_dollar();
    tpos = curwin->w_cursor;
    જો (c == K_C_HOME)
	curwin->w_cursor.lnum = 1;
    curwin->w_cursor.col = 0;
    curwin->w_cursor.coladd = 0;
    curwin->w_curswant = 0;
    start_arrow(&tpos);
}

    સ્થિર શૂન્ય
ins_end(પૂર્ણાંક c)
{
    pos_T	tpos;

#ifdef FEAT_FOLDING
    જો ((fdo_flags & FDO_HOR) && ચાવી_Typed)
	foldOpenCursor();
#endif
    undisplay_dollar();
    tpos = curwin->w_cursor;
    જો (c == K_C_END)
	curwin->w_cursor.lnum = curbuf->b_ml.ml_લીટી_ગણતરી;
    coladvance((colnr_T)MAXCOL);
    curwin->w_curswant = MAXCOL;

    start_arrow(&tpos);
}

    સ્થિર શૂન્ય
ins_s_ડાબું(શૂન્ય)
{
    પૂર્ણાંક end_change = dont_sync_undo == FALSE; // end undoable change
#ifdef FEAT_FOLDING
    જો ((fdo_flags & FDO_HOR) && ચાવી_Typed)
	foldOpenCursor();
#endif
    undisplay_dollar();
    જો (curwin->w_cursor.lnum > 1 || curwin->w_cursor.col > 0)
    {
	start_arrow_with_change(&curwin->w_cursor, end_change);
	જો (!end_change)
	    AppendCharToRedobuff(K_S_ડાબું);
	(શૂન્ય)bck_word(1L, FALSE, FALSE);
	curwin->w_set_curswant = true;
    }
    નહીંતર
	vim_beep(BO_CRSR);
    dont_sync_undo = FALSE;
}

    સ્થિર શૂન્ય
ins_જમણું(શૂન્ય)
{
    પૂર્ણાંક end_change = dont_sync_undo == FALSE; // end undoable change

#ifdef FEAT_FOLDING
    જો ((fdo_flags & FDO_HOR) && ચાવી_Typed)
	foldOpenCursor();
#endif
    undisplay_dollar();
    જો (gchar_cursor() != NUL || virtual_active())
    {
	start_arrow_with_change(&curwin->w_cursor, end_change);
	જો (!end_change)
	    AppendCharToRedobuff(K_જમણું);
	curwin->w_set_curswant = true;
	જો (virtual_active())
	    oneright();
	નહીંતર
	{
	    જો (has_mbyte)
		curwin->w_cursor.col += (*mb_ptr2len)(ml_get_cursor());
	    નહીંતર
		++curwin->w_cursor.col;
	}

#ifdef FEAT_RIGHTLEFT
	revins_legal++;
	જો (revins_chars)
	    revins_chars--;
#endif
    }
    // if 'whichwrap' set for cursor in insert mode, may move the
    // cursor to the next line
    નહીંતર જો (vim_strchr(p_ww, ']') != ખાલી
	    && curwin->w_cursor.lnum < curbuf->b_ml.ml_લીટી_ગણતરી)
    {
	start_arrow(&curwin->w_cursor);
	curwin->w_set_curswant = true;
	++curwin->w_cursor.lnum;
	curwin->w_cursor.col = 0;
    }
    નહીંતર
	vim_beep(BO_CRSR);
    dont_sync_undo = FALSE;
}

    સ્થિર શૂન્ય
ins_s_જમણું(શૂન્ય)
{
    પૂર્ણાંક end_change = dont_sync_undo == FALSE; // end undoable change
#ifdef FEAT_FOLDING
    જો ((fdo_flags & FDO_HOR) && ચાવી_Typed)
	foldOpenCursor();
#endif
    undisplay_dollar();
    જો (curwin->w_cursor.lnum < curbuf->b_ml.ml_લીટી_ગણતરી
	    || gchar_cursor() != NUL)
    {
	start_arrow_with_change(&curwin->w_cursor, end_change);
	જો (!end_change)
	    AppendCharToRedobuff(K_S_જમણું);
	(શૂન્ય)fwd_word(1L, FALSE, 0);
	curwin->w_set_curswant = true;
    }
    નહીંતર
	vim_beep(BO_CRSR);
    dont_sync_undo = FALSE;
}

    સ્થિર શૂન્ય
ins_up(
    પૂર્ણાંક		startcol)	// when TRUE move to Insstart.col
{
    pos_T	tpos;
    linenr_T	old_topline = curwin->w_topline;
#ifdef FEAT_DIFF
    પૂર્ણાંક		old_topfill = curwin->w_topfill;
#endif

    undisplay_dollar();
    tpos = curwin->w_cursor;
    જો (cursor_up(1L, TRUE) == OK)
    {
	જો (startcol)
	    coladvance(getvcol_nolist(&Insstart));
	જો (old_topline != curwin->w_topline
#ifdef FEAT_DIFF
		|| old_topfill != curwin->w_topfill
#endif
		)
	    redraw_later(UPD_VALID);
	start_arrow(&tpos);
	can_cindent = TRUE;
    }
    નહીંતર
	vim_beep(BO_CRSR);
}

    સ્થિર શૂન્ય
ins_pageup(શૂન્ય)
{
    pos_T	tpos;

    undisplay_dollar();

    જો (mod_mask & MOD_MASK_CTRL)
    {
	// <C-PageUp>: tab page back
	જો (first_tabpage->tp_next != ખાલી)
	{
	    start_arrow(&curwin->w_cursor);
	    goto_tabpage(-1);
	}
	પાછા;
    }

    tpos = curwin->w_cursor;
    જો (pagescroll(BACKWARD, 1L, FALSE) == OK)
    {
	start_arrow(&tpos);
	can_cindent = TRUE;
    }
    નહીંતર
	vim_beep(BO_CRSR);
}

    સ્થિર શૂન્ય
ins_down(
    પૂર્ણાંક		startcol)	// when TRUE move to Insstart.col
{
    pos_T	tpos;
    linenr_T	old_topline = curwin->w_topline;
#ifdef FEAT_DIFF
    પૂર્ણાંક		old_topfill = curwin->w_topfill;
#endif

    undisplay_dollar();
    tpos = curwin->w_cursor;
    જો (cursor_down(1L, TRUE) == OK)
    {
	જો (startcol)
	    coladvance(getvcol_nolist(&Insstart));
	જો (old_topline != curwin->w_topline
#ifdef FEAT_DIFF
		|| old_topfill != curwin->w_topfill
#endif
		)
	    redraw_later(UPD_VALID);
	start_arrow(&tpos);
	can_cindent = TRUE;
    }
    નહીંતર
	vim_beep(BO_CRSR);
}

    સ્થિર શૂન્ય
ins_pagedown(શૂન્ય)
{
    pos_T	tpos;

    undisplay_dollar();

    જો (mod_mask & MOD_MASK_CTRL)
    {
	// <C-PageDown>: tab page forward
	જો (first_tabpage->tp_next != ખાલી)
	{
	    start_arrow(&curwin->w_cursor);
	    goto_tabpage(0);
	}
	પાછા;
    }

    tpos = curwin->w_cursor;
    જો (pagescroll(FORWARD, 1L, FALSE) == OK)
    {
	start_arrow(&tpos);
	can_cindent = TRUE;
    }
    નહીંતર
	vim_beep(BO_CRSR);
}

#ifdef FEAT_DND
    સ્થિર શૂન્ય
ins_drop(શૂન્ય)
{
    do_put('~', ખાલી, BACKWARD, 1L, PUT_CURSEND);
}
#endif

/*
 * Handle TAB in Insert or Replace mode.
 * Return TRUE when the TAB needs to be inserted like a normal character.
 */
    સ્થિર પૂર્ણાંક
ins_tab(શૂન્ય)
{
    પૂર્ણાંક		ind;
    પૂર્ણાંક		i;
    પૂર્ણાંક		temp;

    જો (Insstart_blank_vcol == MAXCOL && curwin->w_cursor.lnum == Insstart.lnum)
	Insstart_blank_vcol = get_nolist_virtcol();
    જો (echeck_abbr(TAB + ABBR_OFF))
	પાછા FALSE;

    ind = inindent(0);
    જો (ind)
	can_cindent = FALSE;

    /*
     * When nothing special, insert TAB like a normal character.
     */
    જો (!curbuf->b_p_et
#ifdef FEAT_VARTABS
	    && !(p_sta && ind
		// These five lines mean 'tabstop' != 'shiftwidth'
		&& ((tabstop_ગણતરી(curbuf->b_p_vts_array) > 1)
		    || (tabstop_ગણતરી(curbuf->b_p_vts_array) == 1
			&& tabstop_first(curbuf->b_p_vts_array)
						       != get_sw_કિંમત(curbuf))
		    || (tabstop_ગણતરી(curbuf->b_p_vts_array) == 0
			&& curbuf->b_p_ts != get_sw_કિંમત(curbuf))))
	    && tabstop_ગણતરી(curbuf->b_p_vsts_array) == 0
#નહીંતર
	    && !(p_sta && ind && curbuf->b_p_ts != get_sw_કિંમત(curbuf))
#endif
	    && get_sts_કિંમત() == 0)
	પાછા TRUE;

    જો (stop_arrow() == FAIL)
	પાછા TRUE;

    did_ai = FALSE;
    did_si = FALSE;
    can_si = FALSE;
    can_si_back = FALSE;
    AppendToRedobuff((char_u *)"\t");

#ifdef FEAT_VARTABS
    જો (p_sta && ind)		// insert tab in indent, use 'shiftwidth'
    {
	temp = (પૂર્ણાંક)get_sw_કિંમત(curbuf);
	temp -= get_nolist_virtcol() % temp;
    }
    નહીંતર જો (tabstop_ગણતરી(curbuf->b_p_vsts_array) > 0 || curbuf->b_p_sts != 0)
				// use 'softtabstop' when set
	temp = tabstop_padding(get_nolist_virtcol(), get_sts_કિંમત(),
						     curbuf->b_p_vsts_array);
    નહીંતર			// otherwise use 'tabstop'
	temp = tabstop_padding(get_nolist_virtcol(), curbuf->b_p_ts,
						     curbuf->b_p_vts_array);
#નહીંતર
    જો (p_sta && ind)		// insert tab in indent, use 'shiftwidth'
	temp = (પૂર્ણાંક)get_sw_કિંમત(curbuf);
    નહીંતર જો (curbuf->b_p_sts != 0) // use 'softtabstop' when set
	temp = (પૂર્ણાંક)get_sts_કિંમત();
    નહીંતર			// otherwise use 'tabstop'
	temp = (પૂર્ણાંક)curbuf->b_p_ts;
    temp -= get_nolist_virtcol() % temp;
#endif

    /*
     * Insert the first space with ins_char().	It will delete one char in
     * replace mode.  Insert the rest with ins_str(); it will not delete any
     * chars.  For MODE_VREPLACE state, we use ins_char() for all characters.
     */
    ins_char(' ');
    જ્યારે (--temp > 0)
    {
	જો (State & VREPLACE_FLAG)
	    ins_char(' ');
	નહીંતર
	{
	    ins_str((char_u *)" ", 1);
	    જો (State & REPLACE_FLAG)	    // no char replaced
		replace_ધકેલો(NUL);
	}
    }

    /*
     * When 'expandtab' not set: Replace spaces by TABs where possible.
     */
#ifdef FEAT_VARTABS
    જો (!curbuf->b_p_et && (tabstop_ગણતરી(curbuf->b_p_vsts_array) > 0
			    || get_sts_કિંમત() > 0
			    || (p_sta && ind)))
#નહીંતર
    જો (!curbuf->b_p_et && (get_sts_કિંમત() || (p_sta && ind)))
#endif
    {
	char_u		*ptr;
	char_u		*saved_લીટી = ખાલી;	// init for GCC
	pos_T		pos;
	pos_T		fpos;
	pos_T		*cursor;
	colnr_T		want_vcol, vcol;
	પૂર્ણાંક		change_col = -1;
	પૂર્ણાંક		save_list = curwin->w_p_list;
	char_u		*tab = (char_u *)"\t";
	chartabsize_T	cts;

	/*
	 * Get the current line.  For MODE_VREPLACE state, don't make real
	 * changes yet, just work on a copy of the line.
	 */
	જો (State & VREPLACE_FLAG)
	{
	    pos = curwin->w_cursor;
	    cursor = &pos;
	    saved_લીટી = vim_strnsave(ml_get_curline(), ml_get_curline_len());
	    જો (saved_લીટી == ખાલી)
		પાછા FALSE;
	    ptr = saved_લીટી + pos.col;
	}
	નહીંતર
	{
	    ptr = ml_get_cursor();
	    cursor = &curwin->w_cursor;
	}

	// When 'L' is not in 'cpoptions' a tab always takes up 'ts' spaces.
	જો (vim_strchr(p_cpo, CPO_LISTWM) == ખાલી)
	    curwin->w_p_list = FALSE;

	// Find first white before the cursor
	fpos = curwin->w_cursor;
	જ્યારે (fpos.col > 0 && VIM_ISWHITE(ptr[-1]))
	{
	    --fpos.col;
	    --ptr;
	}

	// In Replace mode, don't change characters before the insert point.
	જો ((State & REPLACE_FLAG)
		&& fpos.lnum == Insstart.lnum
		&& fpos.col < Insstart.col)
	{
	    ptr += Insstart.col - fpos.col;
	    fpos.col = Insstart.col;
	}

	// compute virtual column numbers of first white and cursor
	getvcol(curwin, &fpos, &vcol, ખાલી, ખાલી, 0);
	getvcol(curwin, cursor, &want_vcol, ખાલી, ખાલી, 0);

	init_chartabsize_arg(&cts, curwin, 0, vcol, tab, tab);

	// Use as many TABs as possible.  Beware of 'breakindent', 'showbreak'
	// and 'linebreak' adding extra virtual columns.
	જ્યારે (VIM_ISWHITE(*ptr))
	{
	    i = lbr_chartabsize(&cts);
	    જો (cts.cts_vcol + i > want_vcol)
		તોડો;
	    જો (*ptr != TAB)
	    {
		*ptr = TAB;
		જો (change_col < 0)
		{
		    change_col = fpos.col;  // Column of first change
		    // May have to adjust Insstart
		    જો (fpos.lnum == Insstart.lnum && fpos.col < Insstart.col)
			Insstart.col = fpos.col;
		}
	    }
	    ++fpos.col;
	    ++ptr;
	    cts.cts_vcol += i;
	}
	vcol = cts.cts_vcol;
	clear_chartabsize_arg(&cts);

	જો (change_col >= 0)
	{
	    પૂર્ણાંક		    repl_off = 0;

	    // Skip over the spaces we need.
	    init_chartabsize_arg(&cts, curwin, 0, vcol, ptr, ptr);
	    જ્યારે (cts.cts_vcol < want_vcol && *cts.cts_ptr == ' ')
	    {
		cts.cts_vcol += lbr_chartabsize(&cts);
		++cts.cts_ptr;
		++repl_off;
	    }
	    ptr = cts.cts_ptr;
	    vcol = cts.cts_vcol;
	    clear_chartabsize_arg(&cts);

	    જો (vcol > want_vcol)
	    {
		// Must have a char with 'showbreak' just before it.
		--ptr;
		--repl_off;
	    }
	    fpos.col += repl_off;

	    // Delete following spaces.
	    i = cursor->col - fpos.col;
	    જો (i > 0)
	    {
#ifdef FEAT_PROP_POPUP
		જો (!(State & VREPLACE_FLAG))
		{
		    char_u  *newp;
		    પૂર્ણાંક	    col;

		    newp = alloc(curbuf->b_ml.ml_લીટી_len - i);
		    જો (newp == ખાલી)
			પાછા FALSE;

		    col = ptr - curbuf->b_ml.ml_લીટી_ptr;
		    જો (col > 0)
			mch_memmove(newp, ptr - col, col);
		    mch_memmove(newp + col, ptr + i,
					   curbuf->b_ml.ml_લીટી_len - col - i);

		    જો (curbuf->b_ml.ml_flags & (ML_લીટી_DIRTY | ML_ALLOCATED))
			vim_free(curbuf->b_ml.ml_લીટી_ptr);
		    curbuf->b_ml.ml_લીટી_ptr = newp;
		    curbuf->b_ml.ml_લીટી_len -= i;
		    curbuf->b_ml.ml_લીટી_textlen = 0;
		    curbuf->b_ml.ml_flags =
			   (curbuf->b_ml.ml_flags | ML_લીટી_DIRTY) & ~ML_EMPTY;
		}
		નહીંતર
#endif
		    STRMOVE(ptr, ptr + i);
		// correct replace stack.
		જો ((State & REPLACE_FLAG) && !(State & VREPLACE_FLAG))
		    માટે (temp = i; --temp >= 0; )
			replace_join(repl_off);
	    }
#ifdef FEAT_NETBEANS_INTG
	    જો (netbeans_active())
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
	    જો (State & VREPLACE_FLAG)
	    {
		// Backspace from real cursor to change_col
		backspace_until_column(change_col);

		// Insert each char in saved_line from changed_col to
		// ptr-cursor
		ins_bytes_len(saved_લીટી + change_col,
						    cursor->col - change_col);
	    }
	}

	જો (State & VREPLACE_FLAG)
	    vim_free(saved_લીટી);
	curwin->w_p_list = save_list;
    }

    પાછા FALSE;
}

/*
 * Handle CR or NL in insert mode.
 * Return FAIL when out of memory or can't undo.
 */
    પૂર્ણાંક
ins_eol(પૂર્ણાંક c)
{
    પૂર્ણાંક	    i;

    જો (echeck_abbr(c + ABBR_OFF))
	પાછા OK;
    જો (stop_arrow() == FAIL)
	પાછા FAIL;
    undisplay_dollar();

    /*
     * Strange Vi behaviour: In Replace mode, typing a NL will not delete the
     * character under the cursor.  Only push a NUL on the replace stack,
     * nothing to put back when the NL is deleted.
     */
    જો ((State & REPLACE_FLAG) && !(State & VREPLACE_FLAG))
	replace_ધકેલો(NUL);

    /*
     * In MODE_VREPLACE state, a NL replaces the rest of the line, and starts
     * replacing the next line, so we push all of the characters left on the
     * line onto the replace stack.  This is not done here though, it is done
     * in open_line().
     */

    // Put cursor on NUL if on the last char and coladd is 1 (happens after
    // CTRL-O).
    જો (virtual_active() && curwin->w_cursor.coladd > 0)
	coladvance(getviscol());

#ifdef FEAT_RIGHTLEFT
    // NL in reverse insert will always start in the end of
    // current line.
    જો (revins_on)
	curwin->w_cursor.col += ml_get_cursor_len();
#endif

    AppendToRedobuff(NL_STR);
    i = open_લીટી(FORWARD,
	    has_format_option(FO_RET_COMS) ? OPENLINE_DO_COM : 0, old_indent,
	    ખાલી);
    old_indent = 0;
    can_cindent = TRUE;
#ifdef FEAT_FOLDING
    // When inserting a line the cursor line must never be in a closed fold.
    foldOpenCursor();
#endif

    પાછા i;
}

#ifdef FEAT_DIGRAPHS
/*
 * Handle digraph in insert mode.
 * Returns character still to be inserted, or NUL when nothing remaining to be
 * done.
 */
    સ્થિર પૂર્ણાંક
ins_digraph(શૂન્ય)
{
    પૂર્ણાંક	    c;
    પૂર્ણાંક	    cc;
    પૂર્ણાંક	    did_putchar = FALSE;

    pc_status = PC_STATUS_UNSET;
    જો (redrawing() && !char_avail())
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
    ++allow_ચાવીઓ;
    c = plain_vgetc();
    --no_mapping;
    --allow_ચાવીઓ;
    જો (did_putchar)
	// when the line fits in 'columns' the '?' is at the start of the next
	// line and will not be removed by the redraw
	edit_unputchar();

    જો (IS_SPECIAL(c) || mod_mask)	    // special key
    {
	clear_showcmd();
	ઉમેરો_special(c, TRUE, FALSE);
	પાછા NUL;
    }
    જો (c != ESC)
    {
	did_putchar = FALSE;
	જો (redrawing() && !char_avail())
	{
	    // may need to redraw when no more chars available now
	    ins_redraw(FALSE);

	    જો (char2cells(c) == 1)
	    {
		ins_redraw(FALSE);
		edit_putchar(c, TRUE);
		did_putchar = TRUE;
	    }
	    add_to_showcmd_c(c);
	}
	++no_mapping;
	++allow_ચાવીઓ;
	cc = plain_vgetc();
	--no_mapping;
	--allow_ચાવીઓ;
	જો (did_putchar)
	    // when the line fits in 'columns' the '?' is at the start of the
	    // next line and will not be removed by a redraw
	    edit_unputchar();
	જો (cc != ESC)
	{
	    AppendToRedobuff((char_u *)CTRL_V_STR);
	    c = digraph_get(c, cc, TRUE);
	    clear_showcmd();
	    પાછા c;
	}
    }
    clear_showcmd();
    પાછા NUL;
}
#endif

/*
 * Handle CTRL-E and CTRL-Y in Insert mode: copy char from other line.
 * Returns the char to be inserted, or NUL if none found.
 */
    પૂર્ણાંક
ins_copychar(linenr_T lnum)
{
    પૂર્ણાંક		    c;
    char_u	    *ptr, *prev_ptr;
    char_u	    *લીટી;
    chartabsize_T   cts;

    જો (lnum < 1 || lnum > curbuf->b_ml.ml_લીટી_ગણતરી)
    {
	vim_beep(BO_COPY);
	પાછા NUL;
    }

    // try to advance to the cursor column
    validate_virtcol();
    લીટી = ml_get(lnum);
    prev_ptr = લીટી;
    init_chartabsize_arg(&cts, curwin, lnum, 0, લીટી, લીટી);
    જ્યારે (cts.cts_vcol < curwin->w_virtcol && *cts.cts_ptr != NUL)
    {
	prev_ptr = cts.cts_ptr;
	cts.cts_vcol += lbr_chartabsize_adv(&cts);
    }
    જો (cts.cts_vcol > curwin->w_virtcol)
	ptr = prev_ptr;
    નહીંતર
	ptr = cts.cts_ptr;
    clear_chartabsize_arg(&cts);

    c = (*mb_ptr2char)(ptr);
    જો (c == NUL)
	vim_beep(BO_COPY);
    પાછા c;
}

/*
 * CTRL-Y or CTRL-E typed in Insert mode.
 */
    સ્થિર પૂર્ણાંક
ins_ctrl_ey(પૂર્ણાંક tc)
{
    પૂર્ણાંક	    c = tc;

    જો (ctrl_x_mode_scroll())
    {
	જો (c == Ctrl_Y)
	    scrolldown_clamp();
	નહીંતર
	    scrollup_clamp();
	redraw_later(UPD_VALID);
    }
    નહીંતર
    {
	c = ins_copychar(curwin->w_cursor.lnum + (c == Ctrl_Y ? -1 : 1));
	જો (c != NUL)
	{
	    long	tw_save;

	    // The character must be taken literally, insert like it
	    // was typed after a CTRL-V, and pretend 'textwidth'
	    // wasn't set.  Digits, 'o' and 'x' are special after a
	    // CTRL-V, don't use it for these.
	    જો (c < 256 && !SAFE_isalnum(c))
		AppendToRedobuff((char_u *)CTRL_V_STR);	// CTRL-V
	    tw_save = curbuf->b_p_tw;
	    curbuf->b_p_tw = -1;
	    ઉમેરો_special(c, TRUE, FALSE);
	    curbuf->b_p_tw = tw_save;
#ifdef FEAT_RIGHTLEFT
	    revins_chars++;
	    revins_legal++;
#endif
	    c = Ctrl_V;	// pretend CTRL-V is last character
	    auto_format(FALSE, TRUE);
	}
    }
    પાછા c;
}

/*
 * Get the value that w_virtcol would have when 'list' is off.
 * Unless 'cpo' contains the 'L' flag.
 */
    colnr_T
get_nolist_virtcol(શૂન્ય)
{
    // check validity of cursor in current buffer
    જો (curwin->w_buffer == ખાલી
	|| curwin->w_buffer->b_ml.ml_mfp == ખાલી
	|| curwin->w_cursor.lnum > curwin->w_buffer->b_ml.ml_લીટી_ગણતરી)
	પાછા 0;
    જો (curwin->w_p_list && vim_strchr(p_cpo, CPO_LISTWM) == ખાલી)
	પાછા getvcol_nolist(&curwin->w_cursor);
    validate_virtcol();
    પાછા curwin->w_virtcol;
}

#જો defined(FEAT_EVAL)
/*
 * Handle the InsertCharPre autocommand.
 * "c" is the character that was typed.
 * Return a pointer to allocated memory with the replacement string.
 * Return NULL to continue inserting "c".
 */
    સ્થિર char_u *
do_ઉમેરો_char_pre(પૂર્ણાંક c)
{
    char_u	*res;
    char_u	buf[MB_MAXBYTES + 1];
    size_t	buflen;
    પૂર્ણાંક		save_State = State;

    // Return quickly when there is nothing to do.
    જો (!has_insertcharpre())
	પાછા ખાલી;

    જો (c == Ctrl_RSB)
	પાછા ખાલી;

    જો (has_mbyte)
	buflen = (*mb_char2bytes)(c, buf);
    નહીંતર
    {
	buf[0] = c;
	buflen = 1;
    }
    buf[buflen] = NUL;

    // Lock the text to avoid weird things from happening.
    ++textlock;
    set_vim_var_string(VV_CHAR, buf, (પૂર્ણાંક)buflen);  // set v:char

    res = ખાલી;
    જો (ins_apply_autocmds(EVENT_INSERTCHARPRE))
    {
	// Get the value of v:char.  It may be empty or more than one
	// character.  Only use it when changed, otherwise continue with the
	// original character to avoid breaking autoindent.
	જો (STRCMP(buf, get_vim_var_str(VV_CHAR)) != 0)
	    res = vim_strsave(get_vim_var_str(VV_CHAR));
    }

    set_vim_var_string(VV_CHAR, ખાલી, -1);  // clear v:char
    --textlock;

    // Restore the State, it may have been changed.
    State = save_State;

    પાછા res;
}
#endif

    પૂર્ણાંક
get_can_cindent(શૂન્ય)
{
    પાછા can_cindent;
}

    શૂન્ય
set_can_cindent(પૂર્ણાંક val)
{
    can_cindent = val;
}

/*
 * Trigger "event" and take care of fixing undo.
 */
    પૂર્ણાંક
ins_apply_autocmds(event_T event)
{
    varnumber_T	tick = CHANGEDTICK(curbuf);
    પૂર્ણાંક r;

    r = apply_autocmds(event, ખાલી, ખાલી, FALSE, curbuf);

    // If u_savesub() was called then we are not prepared to start
    // a new line.  Call u_save() with no contents to fix that.
    // Except when leaving Insert mode.
    જો (event != EVENT_INSERTLEAVE && tick != CHANGEDTICK(curbuf))
	u_save(curwin->w_cursor.lnum, (linenr_T)(curwin->w_cursor.lnum + 1));

    પાછા r;
}
