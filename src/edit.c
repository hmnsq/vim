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

#ਅਨ੍ਤਰ੍ਭਾਵ "vim.h"

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BACKSPACE_CHAR		    1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BACKSPACE_ਸ਼ਬਦ		    2
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BACKSPACE_ਸ਼ਬਦ_NOT_SPACE    3
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BACKSPACE_ਸਤਰ		    4

// Set when doing something for completion that may call edit() recursively,
// which is not allowed.
ਸਥਿਰ ਪੂਰਨ	compl_busy = FALSE;


ਸਥਿਰ ਖਾਲੀ ins_ctrl_v(ਖਾਲੀ);
ਸਥਿਰ ਖਾਲੀ ਪਾਓ_special(ਪੂਰਨ, ਪੂਰਨ, ਪੂਰਨ);
ਸਥਿਰ ਖਾਲੀ redo_literal(ਪੂਰਨ c);
ਸਥਿਰ ਖਾਲੀ ਸ਼ੁਰੂ_arrow_common(pos_T *ਸਿਰਾ_ਪਾਓ_pos, ਪੂਰਨ change);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SPELL
ਸਥਿਰ ਖਾਲੀ check_spell_redraw(ਖਾਲੀ);
#ਜੇ_ਅੰਤ
ਸਥਿਰ ਖਾਲੀ stop_ਪਾਓ(pos_T *ਸਿਰਾ_ਪਾਓ_pos, ਪੂਰਨ esc, ਪੂਰਨ nomove);
ਸਥਿਰ ਪੂਰਨ  echeck_abbr(ਪੂਰਨ);
ਸਥਿਰ ਖਾਲੀ mb_replace_ਕੱਢੋ_ins(ਪੂਰਨ cc);
ਸਥਿਰ ਖਾਲੀ replace_flush(ਖਾਲੀ);
ਸਥਿਰ ਖਾਲੀ replace_do_bs(ਪੂਰਨ ਹੱਦਬੰਦੀ_ਖਾਨਾ);
ਸਥਿਰ ਪੂਰਨ del_char_after_ਖਾਨਾ(ਪੂਰਨ ਹੱਦਬੰਦੀ_ਖਾਨਾ);
ਸਥਿਰ ਖਾਲੀ ins_reg(ਖਾਲੀ);
ਸਥਿਰ ਖਾਲੀ ins_ctrl_g(ਖਾਲੀ);
ਸਥਿਰ ਖਾਲੀ ins_ctrl_hat(ਖਾਲੀ);
ਸਥਿਰ ਪੂਰਨ  ins_esc(ਲੰਮਾ *ਗਿਣਤੀ_2, ਪੂਰਨ cmdchar, ਪੂਰਨ nomove);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RIGHTLEFT
ਸਥਿਰ ਖਾਲੀ ins_ctrl_(ਖਾਲੀ);
#ਜੇ_ਅੰਤ
ਸਥਿਰ ਪੂਰਨ ins_ਸ਼ੁਰੂ_select(ਪੂਰਨ c);
ਸਥਿਰ ਖਾਲੀ ins_ਪਾਓ(ਪੂਰਨ replaceState);
ਸਥਿਰ ਖਾਲੀ ins_ctrl_o(ਖਾਲੀ);
ਸਥਿਰ ਖਾਲੀ ins_ਖਿਸਕਾਓ(ਪੂਰਨ c, ਪੂਰਨ lastc);
ਸਥਿਰ ਖਾਲੀ ins_del(ਖਾਲੀ);
ਸਥਿਰ ਪੂਰਨ  ins_bs(ਪੂਰਨ c, ਪੂਰਨ ਬਹੁਲਕ, ਪੂਰਨ *inserted_space_p);
#ਜੇ defined(FEAT_GUI_TABLINE)
ਸਥਿਰ ਖਾਲੀ ins_tabline(ਪੂਰਨ c);
#ਜੇ_ਅੰਤ
ਸਥਿਰ ਖਾਲੀ ins_ਖੱਬਾ(ਖਾਲੀ);
ਸਥਿਰ ਖਾਲੀ ins_home(ਪੂਰਨ c);
ਸਥਿਰ ਖਾਲੀ ins_ਸਿਰਾ(ਪੂਰਨ c);
ਸਥਿਰ ਖਾਲੀ ins_s_ਖੱਬਾ(ਖਾਲੀ);
ਸਥਿਰ ਖਾਲੀ ins_ਸੱਜਾ(ਖਾਲੀ);
ਸਥਿਰ ਖਾਲੀ ins_s_ਸੱਜਾ(ਖਾਲੀ);
ਸਥਿਰ ਖਾਲੀ ins_ਉੱਪਰ(ਪੂਰਨ startcol);
ਸਥਿਰ ਖਾਲੀ ins_pageup(ਖਾਲੀ);
ਸਥਿਰ ਖਾਲੀ ins_ਥੱਲੇ(ਪੂਰਨ startcol);
ਸਥਿਰ ਖਾਲੀ ins_pagedown(ਖਾਲੀ);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DND
ਸਥਿਰ ਖਾਲੀ ins_drop(ਖਾਲੀ);
#ਜੇ_ਅੰਤ
ਸਥਿਰ ਪੂਰਨ  ins_tab(ਖਾਲੀ);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIGRAPHS
ਸਥਿਰ ਪੂਰਨ  ins_digraph(ਖਾਲੀ);
#ਜੇ_ਅੰਤ
ਸਥਿਰ ਪੂਰਨ  ins_ctrl_ey(ਪੂਰਨ tc);
#ਜੇ defined(FEAT_EVAL)
ਸਥਿਰ char_u *do_ਪਾਓ_char_pre(ਪੂਰਨ c);
#ਜੇ_ਅੰਤ

ਸਥਿਰ colnr_T	Insstart_textlen;	// length of line when insert started
ਸਥਿਰ colnr_T	Insstart_blank_vcol;	// vcol for first inserted blank
ਸਥਿਰ ਪੂਰਨ	update_Insstart_orig = TRUE; // set Insstart_orig to Insstart

ਸਥਿਰ string_T	ਆਖਰੀ_ਪਾਓ = {ਨਿਰਾ, 0};    // the text of the previous insert,
					    // K_SPECIAL and CSI are escaped
ਸਥਿਰ ਪੂਰਨ	ਆਖਰੀ_ਪਾਓ_skip; // nr of chars in front of previous insert
ਸਥਿਰ ਪੂਰਨ	new_ਪਾਓ_skip;  // nr of chars in front of current insert
ਸਥਿਰ ਪੂਰਨ	did_restart_edit;	// "restart_edit" when calling edit()

ਸਥਿਰ ਪੂਰਨ	can_cindent;		// may do cindenting on this line

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RIGHTLEFT
ਸਥਿਰ ਪੂਰਨ	revins_on;		// reverse insert mode on
ਸਥਿਰ ਪੂਰਨ	revins_chars;		// how much to skip after edit
ਸਥਿਰ ਪੂਰਨ	revins_legal;		// was the last char 'legal'?
ਸਥਿਰ ਪੂਰਨ	revins_scol;		// start column of revins session
#ਜੇ_ਅੰਤ

ਸਥਿਰ ਪੂਰਨ	ins_need_undo;		// call u_save() before inserting a
					// char.  Set when edit() is called.
					// after that arrow_used is used.

ਸਥਿਰ ਪੂਰਨ	dont_sync_undo = FALSE;	// CTRL-G U prevents syncing undo for
					// the next left/right cursor key

// With 'autocompletedelay' set, arm the delay and let the main loop fire
// Insert-mode autocommands; the popup is shown later on K_COMPLETE_DELAY.
// Otherwise trigger completion right away.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TRIGGER_AUTOCOMPLETE()			\
    ਕਰੋ {					\
	update_screen(UPD_VALID);  /* Show char (deletion) immediately */ \
	ਬਾਹਰਲਾ_flush();				\
	ins_compl_enable_autocomplete();	\
	ins_compl_arm_autostart();		\
	ਜੇ (!ins_compl_arm_autocomplete_delay())\
	    ਜਾਓ docomplete;			\
    } ਜਦੋਂ (0)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MAY_TRIGGER_AUTOCOMPLETE(c)				\
    ਕਰੋ {							\
	ਜੇ (ins_compl_has_autocomplete() && !char_avail()	\
		&& curwin->w_cursor.ਖਾਨਾ > 0)			\
	{							\
	    (c) = char_before_cursor();				\
	    ਜੇ (vim_isprintc(c))				\
		TRIGGER_AUTOCOMPLETE();				\
	}							\
    } ਜਦੋਂ (0)

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
    ਪੂਰਨ
edit(
    ਪੂਰਨ		cmdchar,
    ਪੂਰਨ		startln,	// if set, insert at start of line
    ਲੰਮਾ	ਗਿਣਤੀ_2)
{
    ਪੂਰਨ		c = 0;
    char_u	*ptr;
    ਪੂਰਨ		lastc = 0;
    ਪੂਰਨ		mincol;
    ਸਥਿਰ linenr_T o_lnum = 0;
    ਪੂਰਨ		i;
    ਪੂਰਨ		did_backspace = TRUE;	    // previous char was backspace
    ਪੂਰਨ		ਸਤਰ_is_white = FALSE;	    // line is empty before insert
    linenr_T	old_topline = 0;	    // topline before insertion
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
    ਪੂਰਨ		old_topfill = -1;
#ਜੇ_ਅੰਤ
    ਪੂਰਨ		inserted_space = FALSE;     // just inserted a space
    ਪੂਰਨ		replaceState = ਬਹੁਲਕ_REPLACE;
    ਪੂਰਨ		nomove = FALSE;		    // don't move cursor on return
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਕੰਮ_CHANNEL
    ਪੂਰਨ		cmdchar_todo = cmdchar;
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CONCEAL
    ਪੂਰਨ		cursor_ਸਤਰ_was_concealed;
#ਜੇ_ਅੰਤ
    ਪੂਰਨ		ins_just_started = TRUE;

    // Remember whether editing was restarted after CTRL-O.
    did_restart_edit = restart_edit;

    // sleep before redrawing, needed for "CTRL-O :" that results in an
    // error message
    check_for_delay(TRUE);

    // set Insstart_orig to Insstart
    update_Insstart_orig = TRUE;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_SANDBOX
    // Don't allow inserting in the sandbox.
    ਜੇ (sandbox != 0)
    {
	emsg(_(e_not_allowed_ਵਿੱਚ_sandbox));
	ਵਾਪਸ FALSE;
    }
#ਜੇ_ਅੰਤ
    // Don't allow changes in the buffer while editing the cmdline.  The
    // caller of getcmdline() may get confused.
    // Don't allow recursive insert mode when busy with completion.
    ਜੇ (textlock != 0 || ins_compl_ਸਰਗਰਮ() || compl_busy || pum_visible())
    {
	emsg(_(e_not_allowed_ਮੰਜ਼ਿਲ_change_ਪਾਠ_or_change_ਖਿੜਕੀ));
	ਵਾਪਸ FALSE;
    }
    ins_compl_clear();	    // clear stuff for CTRL-X mode

    /*
     * Trigger InsertEnter autocommands.  Do not do this for "r<CR>" or "grx".
     */
    ਜੇ (cmdchar != 'r' && cmdchar != 'v')
    {
	pos_T   save_cursor = curwin->w_cursor;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
	ਜੇ (cmdchar == 'R')
	    ptr = (char_u *)"r";
	ਨਹੀਂਤਾਂ ਜੇ (cmdchar == 'V')
	    ptr = (char_u *)"v";
	ਨਹੀਂਤਾਂ
	    ptr = (char_u *)"i";
	ਸੈੱਟ_vim_var_string(VV_INSERTMODE, ptr, 1);
	ਸੈੱਟ_vim_var_string(VV_CHAR, ਨਿਰਾ, -1);  // clear v:char
#ਜੇ_ਅੰਤ
	ins_apply_autocmds(EVENT_INSERTENTER);

	// Check for changed highlighting, e.g. for ModeMsg.
	ਜੇ (need_highlight_ਬਦਲਿਆ)
	    highlight_ਬਦਲਿਆ();

	// Make sure the cursor didn't move.  Do call check_cursor_col() in
	// case the text was modified.  Since Insert mode was not started yet
	// a call to check_cursor_col() may move the cursor, especially with
	// the "A" command, thus set State to avoid that. Also check that the
	// line number is still valid (lines may have been deleted).
	// Do not restore if v:char was set to a non-empty string.
	ਜੇ (!EQUAL_POS(curwin->w_cursor, save_cursor)
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
		&& *get_vim_var_str(VV_CHAR) == NUL
#ਜੇ_ਅੰਤ
		&& save_cursor.lnum <= curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
	{
	    ਪੂਰਨ save_state = State;

	    curwin->w_cursor = save_cursor;
	    State = ਬਹੁਲਕ_ਪਾਓ;
	    check_cursor_ਖਾਨਾ();
	    State = save_state;
	}
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CONCEAL
    // Check if the cursor line was concealed before changing State.
    cursor_ਸਤਰ_was_concealed = curwin->w_p_cole > 0
						&& conceal_cursor_ਸਤਰ(curwin);
#ਜੇ_ਅੰਤ

    /*
     * When doing a paste with the middle mouse button, Insstart is set to
     * where the paste started.
     */
    ਜੇ (where_paste_started.lnum != 0)
	Insstart = where_paste_started;
    ਨਹੀਂਤਾਂ
    {
	Insstart = curwin->w_cursor;
	ਜੇ (startln)
	    Insstart.ਖਾਨਾ = 0;
    }
    Insstart_textlen = (colnr_T)linetabsize_str(ml_get_curline());
    Insstart_blank_vcol = MAXCOL;
    ਜੇ (!did_ai)
	ai_ਖਾਨਾ = 0;

    ਜੇ (cmdchar != NUL && restart_edit == 0)
    {
	ResetRedobuff();
	Append_ਸੰਖਿਆ_ਮੰਜ਼ਿਲ_Redobuff(ਗਿਣਤੀ_2);
	ਜੇ (cmdchar == 'V' || cmdchar == 'v')
	{
	    // "gR" or "gr" command
	    Append_Char_ਮੰਜ਼ਿਲ_Redobuff('g');
	    Append_Char_ਮੰਜ਼ਿਲ_Redobuff((cmdchar == 'v') ? 'r' : 'R');
	}
	ਨਹੀਂਤਾਂ
	{
	    ਜੇ (cmdchar == K_PS)
		Append_Char_ਮੰਜ਼ਿਲ_Redobuff('a');
	    ਨਹੀਂਤਾਂ
		Append_Char_ਮੰਜ਼ਿਲ_Redobuff(cmdchar);
	    ਜੇ (cmdchar == 'g')		    // "gI" command
		Append_Char_ਮੰਜ਼ਿਲ_Redobuff('I');
	    ਨਹੀਂਤਾਂ ਜੇ (cmdchar == 'r')	    // "r<CR>" command
		ਗਿਣਤੀ_2 = 1;		    // insert only one <CR>
	}
    }

    ਜੇ (cmdchar == 'R')
    {
	State = ਬਹੁਲਕ_REPLACE;
    }
    ਨਹੀਂਤਾਂ ਜੇ (cmdchar == 'V' || cmdchar == 'v')
    {
	State = ਬਹੁਲਕ_VREPLACE;
	replaceState = ਬਹੁਲਕ_VREPLACE;
	orig_ਸਤਰ_ਗਿਣਤੀ = curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ;
	vr_ਸਤਰਾਂ_ਬਦਲਿਆ = 1;
    }
    ਨਹੀਂਤਾਂ
	State = ਬਹੁਲਕ_ਪਾਓ;

    may_trigger_modechanged();
    stop_ਪਾਓ_ਬਹੁਲਕ = FALSE;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CONCEAL
    // Check if the cursor line needs redrawing after changing State.  If
    // 'concealcursor' is "n" it needs to be redrawn without concealing.
    conceal_check_cursor_ਸਤਰ(cursor_ਸਤਰ_was_concealed);
#ਜੇ_ਅੰਤ

    // Need to position cursor again when on a TAB and when on a char with
    // virtual text.
    ਜੇ (gchar_cursor() == TAB
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
	    || curbuf->b_has_textprop
#ਜੇ_ਅੰਤ
       )
	curwin->w_valid &= ~(VALID_WROW|VALID_WCOL|VALID_VIRTCOL);

    /*
     * Enable langmap or IME, indicated by 'iminsert'.
     * Note that IME may enabled/disabled without us noticing here, thus the
     * 'iminsert' value may not reflect what is actually used.  It is updated
     * when hitting <Esc>.
     */
    ਜੇ (curbuf->b_p_iminsert == B_IMODE_LMAP)
	State |= ਬਹੁਲਕ_LANGMAP;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_ਨਿਵੇਸ਼_ਵਿਧੀ
    im_ਸੈੱਟ_ਸਰਗਰਮ(curbuf->b_p_iminsert == B_IMODE_IM);
#ਜੇ_ਅੰਤ

    setmouse();
    clear_showcmd();
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RIGHTLEFT
    // there is no reverse replace mode
    revins_on = (State == ਬਹੁਲਕ_ਪਾਓ && p_ri);
    ਜੇ (revins_on)
	undisplay_dollar();
    revins_chars = 0;
    revins_legal = 0;
    revins_scol = -1;
#ਜੇ_ਅੰਤ
    ਜੇ (!p_ek)
    {
	MAY_WANT_ਮੰਜ਼ਿਲ_LOG_THIS;

	// Disable bracketed paste mode, we won't recognize the escape
	// sequences.
	ਬਾਹਰਲਾ_str(T_BD);

	// Disable modifyOtherKeys, keys with modifiers would cause exiting
	// Insert mode.
	ਬਾਹਰਲਾ_str_t_TE();
    }

    /*
     * Handle restarting Insert mode.
     * Don't do this for "CTRL-O ." (repeat an insert): In that case we get
     * here with something in the stuff buffer.
     */
    ਜੇ (restart_edit != 0 && stuff_ਖਾਲੀ_ਥਾਵਾਂ())
    {
	/*
	 * After a paste we consider text typed to be part of the insert for
	 * the pasted text. You can backspace over the pasted text too.
	 */
	ਜੇ (where_paste_started.lnum)
	    arrow_ਵਰਤਿਆ = FALSE;
	ਨਹੀਂਤਾਂ
	    arrow_ਵਰਤਿਆ = TRUE;
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
	ਜੇ (((ins_at_eol && curwin->w_cursor.lnum == o_lnum)
		    || curwin->w_curswant > curwin->w_virtcol)
		&& *(ptr = ml_get_curline() + curwin->w_cursor.ਖਾਨਾ) != NUL)
	{
	    ਜੇ (ptr[1] == NUL)
		++curwin->w_cursor.ਖਾਨਾ;
	    ਨਹੀਂਤਾਂ ਜੇ (has_mbyte)
	    {
		i = (*mb_ptr2len)(ptr);
		ਜੇ (ptr[i] == NUL)
		    curwin->w_cursor.ਖਾਨਾ += i;
	    }
	}
	ins_at_eol = FALSE;
    }
    ਨਹੀਂਤਾਂ
	arrow_ਵਰਤਿਆ = FALSE;

    // we are in insert mode now, don't need to start it anymore
    need_ਸ਼ੁਰੂ_insertmode = FALSE;

    // Need to save the line for undo before inserting the first char.
    ins_need_undo = TRUE;

    where_paste_started.lnum = 0;
    can_cindent = TRUE;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
    // The cursor line is not in a closed fold, unless 'insertmode' is set or
    // restarting.
    ਜੇ (!p_im && did_restart_edit == 0)
	foldOpenCursor();
#ਜੇ_ਅੰਤ

    /*
     * If 'showmode' is set, show the current (insert/replace/..) mode.
     * A warning message for changing a readonly file is given here, before
     * actually changing anything.  It's put after the mode, if any.
     */
    i = 0;
    ਜੇ (p_smd && ਸੁਨੇਹਾ_silent == 0)
	i = showmode();

    ਜੇ (!p_im && did_restart_edit == 0)
	change_warning(i == 0 ? 0 : i + 1);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURSOR_SHAPE
    ui_cursor_shape();		// may show different cursor shape
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIGRAPHS
    do_digraph(-1);		// clear digraphs
#ਜੇ_ਅੰਤ

    /*
     * Get the current length of the redo buffer, those characters have to be
     * skipped if we want to get to the inserted characters.
     */
    string_T inserted = get_inserted();
    new_ਪਾਓ_skip = (ਪੂਰਨ)inserted.ਲੰਮਾਈ;
    ਜੇ (inserted.string != ਨਿਰਾ)
	vim_free(inserted.string);

    old_indent = 0;

    /*
     * Main loop in Insert mode: repeat until Insert mode is left.
     */
    ਲਈ (;;)
    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RIGHTLEFT
	ਜੇ (!revins_legal)
	    revins_scol = -1;	    // reset on illegal motions
	ਨਹੀਂਤਾਂ
	    revins_legal = 0;
#ਜੇ_ਅੰਤ
	ਜੇ (arrow_ਵਰਤਿਆ)	    // don't repeat insert when arrow key used
	    ਗਿਣਤੀ_2 = 0;

	ਜੇ (update_Insstart_orig)
	    Insstart_orig = Insstart;

	ਜੇ (stop_ਪਾਓ_ਬਹੁਲਕ && !ins_compl_ਸਰਗਰਮ())
	{
	    // ":stopinsert" used or 'insertmode' reset
	    ਗਿਣਤੀ_2 = 0;
	    ਜਾਓ doESCkey;
	}

	// set curwin->w_curswant for next K_DOWN or K_UP
	ਜੇ (!arrow_ਵਰਤਿਆ)
	    curwin->w_ਸੈੱਟ_curswant = true;

	// If there is no typeahead may check for timestamps (e.g., for when a
	// menu invoked a shell command).
	ਜੇ (stuff_ਖਾਲੀ_ਥਾਵਾਂ())
	{
	    did_check_timestamps = FALSE;
	    ਜੇ (need_check_timestamps)
		check_timestamps(FALSE);
	}

	/*
	 * When emsg() was called msg_scroll will have been set.
	 */
	ਸੁਨੇਹਾ_scroll = FALSE;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
	// When 'mousefocus' is set a mouse movement may have taken us to
	// another window.  "need_mouse_correct" may then be set because of an
	// autocommand.
	ਜੇ (need_mouse_correct)
	    gui_mouse_correct();
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
	// Open fold at the cursor line, according to 'foldopen'.
	ਜੇ (fdo_flags & FDO_ਪਾਓ)
	    foldOpenCursor();
	// Close folds where the cursor isn't, according to 'foldclose'
	ਜੇ (!char_avail())
	    foldCheckClose();
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਕੰਮ_CHANNEL
	ਜੇ (bt_prompt(curbuf))
	{
	    init_prompt(cmdchar_todo);
	    cmdchar_todo = NUL;
	}
#ਜੇ_ਅੰਤ

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
	ਜੇ (curbuf->b_mod_ਸੈੱਟ
		&& curwin->w_p_wrap
		&& !curwin->w_p_sms
		&& !did_backspace
		&& curwin->w_topline == old_topline
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
		&& curwin->w_topfill == old_topfill
#ਜੇ_ਅੰਤ
		&& ਗਿਣਤੀ_2 <= 1
		)
	{
	    mincol = curwin->w_wcol;
	    validate_cursor_ਖਾਨਾ();

	    ਜੇ (
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_VARTABS
		curwin->w_wcol < mincol - tabstop_at(get_nolist_virtcol(),
						     curbuf->b_p_ts,
						     curbuf->b_p_vts_array,
						     FALSE)
#ਨਹੀਂਤਾਂ
		(ਪੂਰਨ)curwin->w_wcol < mincol - curbuf->b_p_ts
#ਜੇ_ਅੰਤ
		    && curwin->w_wrow ==
				   curwin->w_ਉਚਾਈ - 1 - get_scrolloff_ਮੁੱਲ()
		    && (curwin->w_cursor.lnum != curwin->w_topline
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
			|| curwin->w_topfill > 0
#ਜੇ_ਅੰਤ
		    ))
	    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
		ਜੇ (curwin->w_topfill > 0)
		    --curwin->w_topfill;
		ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
		ਜੇ (hasFolding(curwin->w_topline, ਨਿਰਾ, &old_topline))
		    ਸੈੱਟ_topline(curwin, old_topline + 1);
		ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
		    ਸੈੱਟ_topline(curwin, curwin->w_topline + 1);
	    }
	}

	// May need to adjust w_topline to show the cursor.
	ਜੇ (ਗਿਣਤੀ_2 <= 1)
	    update_topline();

	did_backspace = FALSE;

	ਜੇ (ਗਿਣਤੀ_2 <= 1)
	    validate_cursor();		// may set must_redraw

	/*
	 * Redraw the display when no characters are waiting.
	 * Also shows mode, ruler and positions cursor.
	 */
	ins_redraw(TRUE);

	ਜੇ (curwin->w_p_scb)
	    do_check_scrollbind(TRUE);

	ਜੇ (curwin->w_p_crb)
	    do_check_cursorbind();
	ਜੇ (ਗਿਣਤੀ_2 <= 1)
	    update_curswant();
	old_topline = curwin->w_topline;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
	old_topfill = curwin->w_topfill;
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_ON_FLY_SCROLL
	dont_scroll = FALSE;		// allow scrolling here
#ਜੇ_ਅੰਤ
	// May request the keyboard protocol state now.
	may_send_t_RK();

	/*
	 * Get a character for Insert mode.  Ignore K_IGNORE and K_NOP.
	 */
	ਜੇ (c != K_CURSORHOLD && c != K_COMPLETE_DELAY)
	    lastc = c;		// remember the previous char for CTRL-D

	// After using CTRL-G U the next cursor key will not break undo.
	ਜੇ (dont_sync_undo == MAYBE)
	    dont_sync_undo = TRUE;
	ਨਹੀਂਤਾਂ
	    dont_sync_undo = FALSE;
	ਜੇ (cmdchar == K_PS)
	    // Got here from normal mode when bracketed paste started.
	    c = K_PS;
	ਨਹੀਂਤਾਂ
	{
	    // Trigger autocomplete when entering Insert mode, either directly
	    // or via change commands like 'ciw', 'cw', etc., before the first
	    // character is typed.
	    ਜੇ (ins_just_started)
	    {
		ins_just_started = FALSE;
		ਜੇ (ins_compl_has_autocomplete() && !char_avail()
			&& curwin->w_cursor.ਖਾਨਾ > 0)
		{
		    c = char_before_cursor();
		    ਜੇ (vim_isprintc(c))
		    {
			ins_compl_enable_autocomplete();
			ins_compl_arm_autostart();
			ins_compl_init_get_longest();
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RIGHTLEFT
			ਜੇ (p_hkmap)
			    c = hkmap(c);		// Hebrew mode mapping
#ਜੇ_ਅੰਤ
			// Defer until the delay expires (K_COMPLETE_DELAY), or
			// trigger now when no delay is in effect.
			ਜੇ (!ins_compl_arm_autocomplete_delay())
			    ਜਾਓ docomplete;
		    }
		}
	    }

	    ਕਰੋ
	    {
		c = ਸੁਰੱਖਿਅਤ_vgetc();

		ਜੇ (stop_ਪਾਓ_ਬਹੁਲਕ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMINAL
			|| (c == K_IGNORE && ਪਦ_use_loop())
#ਜੇ_ਅੰਤ
		   )
		{
		    // Insert mode ended, possibly from a callback, or a timer
		    // must have opened a terminal window.
		    ਜੇ (c != K_IGNORE && c != K_NOP)
			vungetc(c);
		    ਗਿਣਤੀ_2 = 0;

		    ਜੇ (!bt_prompt(curwin->w_ਬਫਰ)
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMINAL
			    && !bt_terminal(curwin->w_ਬਫਰ)
#ਜੇ_ਅੰਤ
			    && stop_ਪਾਓ_ਬਹੁਲਕ)
			// :stopinsert command via callback or via server command
			nomove = FALSE;
		    ਨਹੀਂਤਾਂ
			nomove = TRUE;
		    ins_compl_prep(ESC);
		    ਜਾਓ doESCkey;
		}
	    } ਜਦੋਂ (c == K_IGNORE || c == K_NOP);
	}

	// Don't want K_CURSORHOLD for the second key, e.g., after CTRL-V.
	did_cursorhold = TRUE;
	ਜੇ (c != K_CURSORHOLD && c != K_COMPLETE_DELAY)
	{
	    // Don't want delayed autocompletion from the previous key either.
	    ins_compl_clear_autocomplete_delay();
	    ins_compl_disarm_autostart();
	    // A completion already on screen goes on being what it was.
	    ਜੇ (!ins_compl_ਸਰਗਰਮ())
		ins_compl_disable_autocomplete();
	}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RIGHTLEFT
	ਜੇ (p_hkmap && ਕੁੰਜੀ_Typed)
	    c = hkmap(c);		// Hebrew mode mapping
#ਜੇ_ਅੰਤ

	// If the window was made so small that nothing shows, make it at least
	// one line and one column when typing.
	ਜੇ (ਕੁੰਜੀ_Typed && !ਕੁੰਜੀ_Stuffed)
	    win_ensure_ਮਾਪ();

	/*
	 * Special handling of keys while the popup menu is visible or wanted
	 * and the cursor is still in the completed word.  Only when there is
	 * a match, skip this when no matches were found.
	 */
	ਜੇ (ins_compl_ਸਰਗਰਮ() && curwin->w_cursor.ਖਾਨਾ >= ins_compl_ਖਾਨਾ()
		&& ins_compl_has_shown_ਮੇਲ() && pum_wanted())
	{
	    // BS: Delete one character from "compl_leader".
	    ਜੇ ((c == K_BS || c == Ctrl_H)
			&& curwin->w_cursor.ਖਾਨਾ > ins_compl_ਖਾਨਾ()
			&& (c = ins_compl_bs()) == NUL)
		ਜਾਰੀ;

	    // When no match was selected or it was edited.
	    ਜੇ (!ins_compl_ਵਰਤਿਆ_ਮੇਲ())
	    {
		// CTRL-L: Add one character from the current match to
		// "compl_leader".  Except when at the original match and
		// there is nothing to add, CTRL-L works like CTRL-P then.
		ਜੇ (c == Ctrl_L
			&& (!ctrl_x_ਬਹੁਲਕ_ਸਤਰ_or_eval()
			    || ins_compl_long_shown_ਮੇਲ()))
		{
		    ins_compl_addfrommatch();
		    ਜਾਰੀ;
		}

		// A non-white character that fits in with the current
		// completion: Add to "compl_leader".
		ਜੇ (ins_compl_accept_char(c))
		{
#ਜੇ defined(FEAT_EVAL)
		    // Trigger InsertCharPre.
		    char_u *str = do_ਪਾਓ_char_pre(c);
		    char_u *p;

		    ਜੇ (str != ਨਿਰਾ)
		    {
			ਲਈ (p = str; *p != NUL; MB_PTR_ADV(p))
			    ins_compl_addleader(PTR2CHAR(p));
			vim_free(str);
		    }
		    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
			ins_compl_addleader(c);
		    ਜਾਰੀ;
		}

		// Pressing CTRL-Y selects the current match.  When
		// ins_compl_enter_selects() is set the Enter key does the
		// same.
		ਜੇ ((c == Ctrl_Y || (ins_compl_enter_selects()
				    && (c == CAR || c == K_KENTER || c == NL)))
			&& stop_arrow() == OK)
		{
		    ins_compl_delete();
		    ਜੇ (ins_compl_preinsert_longest()
			    && !ins_compl_is_ਮੇਲ_selected())
		    {
			ins_compl_ਪਾਓ(FALSE, TRUE);
			ins_compl_init_get_longest();
			ਜਾਰੀ;
		    }
		    ਨਹੀਂਤਾਂ
			ins_compl_ਪਾਓ(FALSE, FALSE);
		}
		// Delete preinserted text when typing special chars
		ਨਹੀਂਤਾਂ ਜੇ (IS_WHITE_NL_OR_NUL(c) && ins_compl_preinsert_effect())
		    ins_compl_delete();
	    }
	}

	// Prepare for or stop CTRL-X mode.  This doesn't do completion, but
	// it does fix up the text when finishing completion.
	ins_compl_init_get_longest();
	ਜੇ (ins_compl_prep(c))
	    ਜਾਰੀ;

	// CTRL-\ CTRL-N goes to Normal mode,
	// CTRL-\ CTRL-G goes to mode selected with 'insertmode',
	// CTRL-\ CTRL-O is like CTRL-O but without moving the cursor.
	ਜੇ (c == Ctrl_BSL)
	{
	    // may need to redraw when no more chars available now
	    ins_redraw(FALSE);
	    ++no_mapping;
	    ++allow_ਕੁੰਜੀਆਂ;
	    c = plain_vgetc();
	    --no_mapping;
	    --allow_ਕੁੰਜੀਆਂ;
	    ਜੇ (c != Ctrl_N && c != Ctrl_G && c != Ctrl_O)
	    {
		// it's something else
		vungetc(c);
		c = Ctrl_BSL;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (c == Ctrl_G && p_im)
		ਜਾਰੀ;
	    ਨਹੀਂਤਾਂ
	    {
		ਜੇ (c == Ctrl_O)
		{
		    ins_ctrl_o();
		    ins_at_eol = FALSE;	// cursor keeps its column
		    nomove = TRUE;
		}
		ਗਿਣਤੀ_2 = 0;
		ਜਾਓ doESCkey;
	    }
	}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIGRAPHS
	c = do_digraph(c);
#ਜੇ_ਅੰਤ

	ਜੇ ((c == Ctrl_V || c == Ctrl_Q) && ctrl_x_ਬਹੁਲਕ_cmdline())
	    ਜਾਓ docomplete;
	ਜੇ (c == Ctrl_V || c == Ctrl_Q)
	{
	    ins_ctrl_v();
	    c = Ctrl_V;	// pretend CTRL-V is last typed character
	    ਜਾਰੀ;
	}

	ਜੇ (cindent_on() && ctrl_x_ਬਹੁਲਕ_none())
	{
	    // A key name preceded by a bang means this key is not to be
	    // inserted.  Skip ahead to the re-indenting below.
	    // A key name preceded by a star means that indenting has to be
	    // done before inserting the key.
	    ਸਤਰ_is_white = inindent(0);
	    ਜੇ (ਵਿੱਚ_cinkeys(c, '!', ਸਤਰ_is_white))
		ਜਾਓ force_cindent;
	    ਜੇ (can_cindent && ਵਿੱਚ_cinkeys(c, '*', ਸਤਰ_is_white)
							&& stop_arrow() == OK)
		do_c_ਵਿਅੰਜਕ_indent();
	}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RIGHTLEFT
	ਜੇ (curwin->w_p_rl)
	    ਚੋਣ (c)
	    {
		ਹਾਲਤ K_ਖੱਬਾ:	c = K_ਸੱਜਾ; ਰੋਕੋ;
		ਹਾਲਤ K_S_ਖੱਬਾ:	c = K_S_ਸੱਜਾ; ਰੋਕੋ;
		ਹਾਲਤ K_C_ਖੱਬਾ:	c = K_C_ਸੱਜਾ; ਰੋਕੋ;
		ਹਾਲਤ K_ਸੱਜਾ:	c = K_ਖੱਬਾ; ਰੋਕੋ;
		ਹਾਲਤ K_S_ਸੱਜਾ: c = K_S_ਖੱਬਾ; ਰੋਕੋ;
		ਹਾਲਤ K_C_ਸੱਜਾ: c = K_C_ਖੱਬਾ; ਰੋਕੋ;
	    }
#ਜੇ_ਅੰਤ

	/*
	 * If 'keymodel' contains "startsel", may start selection.  If it
	 * does, a CTRL-O and c will be stuffed, we need to get these
	 * characters.
	 */
	ਜੇ (ins_ਸ਼ੁਰੂ_select(c))
	    ਜਾਰੀ;

	/*
	 * The big switch to handle a character in insert mode.
	 */
	ਚੋਣ (c)
	{
	ਹਾਲਤ ESC:	// End input mode
	    ਜੇ (echeck_abbr(ESC + ABBR_OFF))
		ਰੋਕੋ;
	    // FALLTHROUGH

	ਹਾਲਤ Ctrl_C:	// End input mode
	    ਜੇ (c == Ctrl_C && cmdwin_type != 0)
	    {
		// Close the cmdline window.
		cmdwin_ਨਤੀਜਾ = K_IGNORE;
		got_int = FALSE; // don't stop executing autocommands et al.
		nomove = TRUE;
		ਜਾਓ doESCkey;
	    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਕੰਮ_CHANNEL
	    ਜੇ (c == Ctrl_C && bt_prompt(curbuf))
	    {
		ਜੇ (invoke_prompt_ਰੁਕਾਵਟ())
		{
		    ਜੇ (!bt_prompt(curbuf))
			// buffer changed to a non-prompt buffer, get out of
			// Insert mode
			ਜਾਓ doESCkey;
		    ਰੋਕੋ;
		}
	    }
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UNIX
do_intr:
#ਜੇ_ਅੰਤ
	    // when 'insertmode' set, and not halfway a mapping, don't leave
	    // Insert mode
	    ਜੇ (goto_im())
	    {
		ਜੇ (got_int)
		{
		    (ਖਾਲੀ)vgetc();		// flush all buffers
		    got_int = FALSE;
		}
		ਨਹੀਂਤਾਂ
		    vim_beep(BO_IM);
		ਰੋਕੋ;
	    }
doESCkey:
	    // Drop a pending autocomplete so it does not outlive Insert mode.
	    ins_compl_clear_autocomplete_delay();
	    /*
	     * This is the ONLY return from edit()!
	     */
	    // Always update o_lnum, so that a "CTRL-O ." that adds a line
	    // still puts the cursor back after the inserted text.
	    ਜੇ (ins_at_eol && gchar_cursor() == NUL)
		o_lnum = curwin->w_cursor.lnum;

	    ਜੇ (ins_esc(&ਗਿਣਤੀ_2, cmdchar, nomove))
	    {
		// When CTRL-C was typed got_int will be set, with the result
		// that the autocommands won't be executed. When mapped got_int
		// is not set, but let's keep the behavior the same.
		ਜੇ (cmdchar != 'r' && cmdchar != 'v' && c != Ctrl_C)
		    ins_apply_autocmds(EVENT_INSERTLEAVE);
		did_cursorhold = FALSE;

		// ins_redraw() triggers TextChangedI only when no characters
		// are in the typeahead buffer, so reset curbuf->b_last_changedtick only
		// if the TextChangedI was not blocked by char_avail() (e.g. using :norm!)
		// and the TextChangedI autocommand has been triggered.
		ਜੇ (!char_avail() && curbuf->b_ਆਖਰੀ_changedtick_i == CHANGEDTICK(curbuf))
		    curbuf->b_ਆਖਰੀ_changedtick = CHANGEDTICK(curbuf);
		ਵਾਪਸ (c == Ctrl_O);
	    }
	    ਜਾਰੀ;

	ਹਾਲਤ Ctrl_Z:	// suspend when 'insertmode' set
	    ਜੇ (!p_im)
		ਜਾਓ normalchar;	// insert CTRL-Z as normal char
	    do_cmdline_cmd((char_u *)"stop");
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURSOR_SHAPE
	    ui_cursor_shape();		// may need to update cursor shape
#ਜੇ_ਅੰਤ
	    ਜਾਰੀ;

	ਹਾਲਤ Ctrl_O:	// execute one command
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_COMPL_FUNC
	    ਜੇ (ctrl_x_ਬਹੁਲਕ_omni())
		ਜਾਓ docomplete;
#ਜੇ_ਅੰਤ
	    ਜੇ (echeck_abbr(Ctrl_O + ABBR_OFF))
		ਰੋਕੋ;
	    ins_ctrl_o();

	    // don't move the cursor left when 'virtualedit' has "onemore".
	    ਜੇ (get_ve_flags() & VE_ONEMORE)
	    {
		ins_at_eol = FALSE;
		nomove = TRUE;
	    }
	    ਗਿਣਤੀ_2 = 0;
	    ਜਾਓ doESCkey;

	ਹਾਲਤ K_INS:	// toggle insert/replace mode
	ਹਾਲਤ K_KINS:
	    ins_ਪਾਓ(replaceState);
	    ਰੋਕੋ;

	ਹਾਲਤ K_SELECT:	// end of Select mode mapping - ignore
	    ਰੋਕੋ;

	ਹਾਲਤ K_HELP:	// Help key works like <ESC> <Help>
	ਹਾਲਤ K_F1:
	ਹਾਲਤ K_XF1:
	    stuffcharReadbuff(K_HELP);
	    ਜੇ (p_im)
		need_ਸ਼ੁਰੂ_insertmode = TRUE;
	    ਜਾਓ doESCkey;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_NETBEANS_INTG
	ਹਾਲਤ K_F21:	// NetBeans command
	    ++no_mapping;		// don't map the next key hits
	    i = plain_vgetc();
	    --no_mapping;
	    netbeans_keycommand(i);
	    ਰੋਕੋ;
#ਜੇ_ਅੰਤ

	ਹਾਲਤ K_ZERO:	// Insert the previously inserted text.
	ਹਾਲਤ NUL:
	ਹਾਲਤ Ctrl_A:
	    // For ^@ the trailing ESC will end the insert, unless there is an
	    // error.
	    ਜੇ (stuff_inserted(NUL, 1L, (c == Ctrl_A)) == FAIL
						   && c != Ctrl_A && !p_im)
		ਜਾਓ doESCkey;		// quit insert mode
	    inserted_space = FALSE;
	    ਰੋਕੋ;

	ਹਾਲਤ Ctrl_R:	// insert the contents of a register
	    ਜੇ (ctrl_x_ਬਹੁਲਕ_ਰਜਿਸਟਰ() && !ins_compl_ਸਰਗਰਮ())
		ਜਾਓ docomplete;
	    ins_reg();
	    auto_format(FALSE, TRUE);
	    inserted_space = FALSE;
	    ਰੋਕੋ;

	ਹਾਲਤ Ctrl_G:	// commands starting with CTRL-G
	    ins_ctrl_g();
	    ਰੋਕੋ;

	ਹਾਲਤ Ctrl_HAT:	// switch input mode and/or langmap
	    ins_ctrl_hat();
	    ਰੋਕੋ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RIGHTLEFT
	ਹਾਲਤ Ctrl__:	// switch between languages
	    ਜੇ (!p_ari)
		ਜਾਓ normalchar;
	    ins_ctrl_();
	    ਰੋਕੋ;
#ਜੇ_ਅੰਤ

	ਹਾਲਤ Ctrl_D:	// Make indent one shiftwidth smaller.
#ਜੇ defined(FEAT_ਲੱਭੋ_ID)
	    ਜੇ (ctrl_x_ਬਹੁਲਕ_ਰਸਤਾ_ਨਿਰਧਾਰਤ_ਕਰਦਾ())
		ਜਾਓ docomplete;
#ਜੇ_ਅੰਤ
	    // FALLTHROUGH

	ਹਾਲਤ Ctrl_T:	// Make indent one shiftwidth greater.
	    ਜੇ (c == Ctrl_T && ctrl_x_ਬਹੁਲਕ_thesaurus())
	    {
		ਜੇ (has_compl_option(FALSE))
		    ਜਾਓ docomplete;
		ਰੋਕੋ;
	    }

	    ins_ਖਿਸਕਾਓ(c, lastc);
	    auto_format(FALSE, TRUE);
	    inserted_space = FALSE;
	    ਰੋਕੋ;

	ਹਾਲਤ K_DEL:	// delete character under the cursor
	ਹਾਲਤ K_KDEL:
	    ins_del();
	    auto_format(FALSE, TRUE);
	    ਰੋਕੋ;

	ਹਾਲਤ K_BS:	// delete character before the cursor
	ਹਾਲਤ K_S_BS:
	ਹਾਲਤ Ctrl_H:
	    did_backspace = ins_bs(c, BACKSPACE_CHAR, &inserted_space);
	    auto_format(FALSE, TRUE);
	    ਜੇ (did_backspace)
		MAY_TRIGGER_AUTOCOMPLETE(c);
	    ਰੋਕੋ;

	ਹਾਲਤ Ctrl_W:	// delete word before the cursor
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਕੰਮ_CHANNEL
	    ਜੇ (bt_prompt(curbuf) && (mod_ਨਕਾਬ & MOD_ਨਕਾਬ_ਖਿਸਕਾਓ) == 0)
	    {
		// In a prompt window CTRL-W is used for window commands.
		// Use Shift-CTRL-W to delete a word.
		stuffcharReadbuff(Ctrl_W);
		restart_edit = 'A';
		nomove = TRUE;
		ਗਿਣਤੀ_2 = 0;
		ਜਾਓ doESCkey;
	    }
#ਜੇ_ਅੰਤ
	    did_backspace = ins_bs(c, BACKSPACE_ਸ਼ਬਦ, &inserted_space);
	    auto_format(FALSE, TRUE);
	    ਜੇ (did_backspace)
		MAY_TRIGGER_AUTOCOMPLETE(c);
	    ਰੋਕੋ;

	ਹਾਲਤ Ctrl_U:	// delete all inserted text in current line
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_COMPL_FUNC
	    // CTRL-X CTRL-U completes with 'completefunc'.
	    ਜੇ (ctrl_x_ਬਹੁਲਕ_function())
		ਜਾਓ docomplete;
#ਜੇ_ਅੰਤ
	    did_backspace = ins_bs(c, BACKSPACE_ਸਤਰ, &inserted_space);
	    auto_format(FALSE, TRUE);
	    inserted_space = FALSE;
	    ਜੇ (did_backspace)
		MAY_TRIGGER_AUTOCOMPLETE(c);
	    ਰੋਕੋ;

	ਹਾਲਤ K_LEFTMOUSE:   // mouse keys
	ਹਾਲਤ K_LEFTMOUSE_NM:
	ਹਾਲਤ K_LEFTDRAG:
	ਹਾਲਤ K_LEFTRELEASE:
	ਹਾਲਤ K_LEFTRELEASE_NM:
	ਹਾਲਤ K_MOUSEMOVE:
	ਹਾਲਤ K_MIDDLEMOUSE:
	ਹਾਲਤ K_MIDDLEDRAG:
	ਹਾਲਤ K_MIDDLERELEASE:
	ਹਾਲਤ K_RIGHTMOUSE:
	ਹਾਲਤ K_RIGHTDRAG:
	ਹਾਲਤ K_RIGHTRELEASE:
	ਹਾਲਤ K_X1MOUSE:
	ਹਾਲਤ K_X1DRAG:
	ਹਾਲਤ K_X1RELEASE:
	ਹਾਲਤ K_X2MOUSE:
	ਹਾਲਤ K_X2DRAG:
	ਹਾਲਤ K_X2RELEASE:
	    ins_mouse(c);
	    ਰੋਕੋ;

	ਹਾਲਤ K_MOUSEDOWN: // Default action for scroll wheel up: scroll up
	    ins_mousescroll(MSCR_ਥੱਲੇ);
	    ਰੋਕੋ;

	ਹਾਲਤ K_MOUSEUP:	// Default action for scroll wheel down: scroll down
	    ins_mousescroll(MSCR_ਉੱਪਰ);
	    ਰੋਕੋ;

	ਹਾਲਤ K_MOUSELEFT: // Scroll wheel left
	    ins_mousescroll(MSCR_ਖੱਬਾ);
	    ਰੋਕੋ;

	ਹਾਲਤ K_MOUSERIGHT: // Scroll wheel right
	    ins_mousescroll(MSCR_ਸੱਜਾ);
	    ਰੋਕੋ;

	ਹਾਲਤ K_PS:
	    bracketed_paste(PASTE_ਪਾਓ, FALSE, ਨਿਰਾ);
	    ਜੇ (cmdchar == K_PS)
		// invoked from normal mode, bail out
		ਜਾਓ doESCkey;
	    ਰੋਕੋ;
	ਹਾਲਤ K_PE:
	    // Got K_PE without K_PS, ignore.
	    ਰੋਕੋ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_TABLINE
	ਹਾਲਤ K_TABLINE:
	ਹਾਲਤ K_TABMENU:
	    ins_tabline(c);
	    ਰੋਕੋ;
#ਜੇ_ਅੰਤ

	ਹਾਲਤ K_IGNORE:	// Something mapped to nothing
	    ਰੋਕੋ;

	ਹਾਲਤ K_COMMAND:		    // <Cmd>command<CR>
	ਹਾਲਤ K_SCRIPT_COMMAND:	    // <ScriptCmd>command<CR>
	    {
		bufref_T    save_curbuf;
		varnumber_T tick = CHANGEDTICK(curbuf);

		ਸੈੱਟ_bufref(&save_curbuf, curbuf);
		do_cmdkey_command(c, 0);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMINAL
		ਜੇ (ਪਦ_use_loop())
		    // Started a terminal that gets the input, exit Insert mode.
		    ਜਾਓ doESCkey;
#ਜੇ_ਅੰਤ
		ਜੇ (curbuf->b_u_synced
			|| (bufref_valid(&save_curbuf)
			    && curbuf == save_curbuf.br_buf
			    && tick != CHANGEDTICK(curbuf)))
		{
		    // The command synced undo or changed this buffer.
		    // Save the cursor line before the next typed edit.
		    ins_need_undo = TRUE;
		}
	    }
	    ਰੋਕੋ;

	ਹਾਲਤ K_CURSORHOLD:	// Didn't type something for a while.
	    ins_apply_autocmds(EVENT_CURSORHOLDI);
	    did_cursorhold = TRUE;
	    // If CTRL-G U was used apply it to the next typed key.
	    ਜੇ (dont_sync_undo == TRUE)
		dont_sync_undo = MAYBE;
	    ਰੋਕੋ;

	ਹਾਲਤ K_COMPLETE_DELAY:	// 'autocompletedelay' expired
	    // If CTRL-G U was used apply it to the next typed key.
	    ਜੇ (dont_sync_undo == TRUE)
		dont_sync_undo = MAYBE;
	    ins_compl_clear_autocomplete_delay();
	    ਜੇ (!ins_compl_has_autocomplete() || char_avail()
		    || curwin->w_cursor.ਖਾਨਾ == 0)
		ਰੋਕੋ;
	    c = char_before_cursor();
	    ਜੇ (!vim_isprintc(c))
		ਰੋਕੋ;
	    // The completion may have been cleared while waiting, so re-enable
	    // autocomplete to match a zero delay.
	    ins_compl_enable_autocomplete();
	    ins_compl_arm_autostart();
	    ਜਾਓ docomplete;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_MSWIN
	    // On MS-Windows ignore <M-F4>, we get it when closing the window
	    // was cancelled.
	ਹਾਲਤ K_F4:
	    ਜੇ (mod_ਨਕਾਬ != MOD_ਨਕਾਬ_ALT)
		ਜਾਓ normalchar;
	    ਰੋਕੋ;
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
	ਹਾਲਤ K_VER_SCROLLBAR:
	    ins_scroll();
	    ਰੋਕੋ;

	ਹਾਲਤ K_HOR_SCROLLBAR:
	    ins_horscroll();
	    ਰੋਕੋ;
#ਜੇ_ਅੰਤ

	ਹਾਲਤ K_HOME:	// <Home>
	ਹਾਲਤ K_KHOME:
	ਹਾਲਤ K_S_HOME:
	ਹਾਲਤ K_C_HOME:
	    ins_home(c);
	    ਰੋਕੋ;

	ਹਾਲਤ K_ਸਿਰਾ:	// <End>
	ਹਾਲਤ K_KEND:
	ਹਾਲਤ K_S_ਸਿਰਾ:
	ਹਾਲਤ K_C_ਸਿਰਾ:
	    ins_ਸਿਰਾ(c);
	    ਰੋਕੋ;

	ਹਾਲਤ K_ਖੱਬਾ:	// <Left>
	    ਜੇ (mod_ਨਕਾਬ & (MOD_ਨਕਾਬ_ਖਿਸਕਾਓ|MOD_ਨਕਾਬ_CTRL))
		ins_s_ਖੱਬਾ();
	    ਨਹੀਂਤਾਂ
		ins_ਖੱਬਾ();
	    ਰੋਕੋ;

	ਹਾਲਤ K_S_ਖੱਬਾ:	// <S-Left>
	ਹਾਲਤ K_C_ਖੱਬਾ:
	    ins_s_ਖੱਬਾ();
	    ਰੋਕੋ;

	ਹਾਲਤ K_ਸੱਜਾ:	// <Right>
	    ਜੇ (mod_ਨਕਾਬ & (MOD_ਨਕਾਬ_ਖਿਸਕਾਓ|MOD_ਨਕਾਬ_CTRL))
		ins_s_ਸੱਜਾ();
	    ਨਹੀਂਤਾਂ
		ins_ਸੱਜਾ();
	    ਰੋਕੋ;

	ਹਾਲਤ K_S_ਸੱਜਾ:	// <S-Right>
	ਹਾਲਤ K_C_ਸੱਜਾ:
	    ins_s_ਸੱਜਾ();
	    ਰੋਕੋ;

	ਹਾਲਤ K_ਉੱਪਰ:	// <Up>
	    ਜੇ (pum_visible())
		ਜਾਓ docomplete;
	    ਜੇ (mod_ਨਕਾਬ & MOD_ਨਕਾਬ_ਖਿਸਕਾਓ)
		ins_pageup();
	    ਨਹੀਂਤਾਂ
		ins_ਉੱਪਰ(FALSE);
	    ਰੋਕੋ;

	ਹਾਲਤ K_S_ਉੱਪਰ:	// <S-Up>
	ਹਾਲਤ K_PAGEUP:
	ਹਾਲਤ K_KPAGEUP:
	    ਜੇ (pum_visible())
	    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
		// CTRL-SHIFT-<Up> scrolls the info popup up a line,
		// CTRL-SHIFT-<PageUp> a page.  Shift is folded into K_S_UP but
		// stays in mod_mask for PageUp, hence the asymmetric check.
		ਜੇ (c == K_S_ਉੱਪਰ ? (mod_ਨਕਾਬ & MOD_ਨਕਾਬ_CTRL)
			: ((mod_ਨਕਾਬ & MOD_ਨਕਾਬ_CTRL)
			    && (mod_ਨਕਾਬ & MOD_ਨਕਾਬ_ਖਿਸਕਾਓ)))
		{
		    popup_scroll_info(-1, c != K_S_ਉੱਪਰ);
		    ਰੋਕੋ;
		}
#ਜੇ_ਅੰਤ
		ਜਾਓ docomplete;
	    }
	    ins_pageup();
	    ਰੋਕੋ;

	ਹਾਲਤ K_ਥੱਲੇ:	// <Down>
	    ਜੇ (pum_visible())
		ਜਾਓ docomplete;
	    ਜੇ (mod_ਨਕਾਬ & MOD_ਨਕਾਬ_ਖਿਸਕਾਓ)
		ins_pagedown();
	    ਨਹੀਂਤਾਂ
		ins_ਥੱਲੇ(FALSE);
	    ਰੋਕੋ;

	ਹਾਲਤ K_S_ਥੱਲੇ:	// <S-Down>
	ਹਾਲਤ K_PAGEDOWN:
	ਹਾਲਤ K_KPAGEDOWN:
	    ਜੇ (pum_visible())
	    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
		// CTRL-SHIFT-<Down>/<PageDown> scroll the info popup down.
		ਜੇ (c == K_S_ਥੱਲੇ ? (mod_ਨਕਾਬ & MOD_ਨਕਾਬ_CTRL)
			: ((mod_ਨਕਾਬ & MOD_ਨਕਾਬ_CTRL)
			    && (mod_ਨਕਾਬ & MOD_ਨਕਾਬ_ਖਿਸਕਾਓ)))
		{
		    popup_scroll_info(1, c != K_S_ਥੱਲੇ);
		    ਰੋਕੋ;
		}
#ਜੇ_ਅੰਤ
		ਜਾਓ docomplete;
	    }
	    ins_pagedown();
	    ਰੋਕੋ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DND
	ਹਾਲਤ K_DROP:	// drag-n-drop event
	    ins_drop();
	    ਰੋਕੋ;
#ਜੇ_ਅੰਤ

	ਹਾਲਤ K_S_TAB:	// When not mapped, use like a normal TAB
	    c = TAB;
	    // FALLTHROUGH

	ਹਾਲਤ TAB:	// TAB or Complete patterns along path
#ਜੇ defined(FEAT_ਲੱਭੋ_ID)
	    ਜੇ (ctrl_x_ਬਹੁਲਕ_ਰਸਤਾ_patterns())
		ਜਾਓ docomplete;
#ਜੇ_ਅੰਤ
	    inserted_space = FALSE;
	    ਜੇ (ins_tab())
		ਜਾਓ normalchar;	// insert TAB as a normal char
	    auto_format(FALSE, TRUE);
	    ਰੋਕੋ;

	ਹਾਲਤ K_KENTER:	// <Enter>
	    c = CAR;
	    // FALLTHROUGH
	ਹਾਲਤ CAR:
	ਹਾਲਤ NL:
#ਜੇ defined(FEAT_QUICKFIX)
	    // In a quickfix window a <CR> jumps to the error under the
	    // cursor.
	    ਜੇ (bt_quickfix(curbuf) && c == CAR)
	    {
		ਜੇ (curwin->w_llist_ref == ਨਿਰਾ)    // quickfix window
		    do_cmdline_cmd((char_u *)".cc");
		ਨਹੀਂਤਾਂ				    // location list window
		    do_cmdline_cmd((char_u *)".ll");
		ਰੋਕੋ;
	    }
#ਜੇ_ਅੰਤ
	    ਜੇ (cmdwin_type != 0)
	    {
		// Execute the command in the cmdline window.
		cmdwin_ਨਤੀਜਾ = CAR;
		ਜਾਓ doESCkey;
	    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਕੰਮ_CHANNEL
	    ਜੇ (bt_prompt(curbuf))
	    {
		invoke_prompt_callback();
		ਜੇ (!bt_prompt(curbuf))
		    // buffer changed to a non-prompt buffer, get out of
		    // Insert mode
		    ਜਾਓ doESCkey;
		ਰੋਕੋ;
	    }
#ਜੇ_ਅੰਤ
	    ਜੇ (ins_eol(c) == FAIL && !p_im)
		ਜਾਓ doESCkey;	    // out of memory
	    auto_format(FALSE, FALSE);
	    inserted_space = FALSE;
	    ਰੋਕੋ;

	ਹਾਲਤ Ctrl_K:	    // digraph or keyword completion
	    ਜੇ (ctrl_x_ਬਹੁਲਕ_dictionary())
	    {
		ਜੇ (has_compl_option(TRUE))
		    ਜਾਓ docomplete;
		ਰੋਕੋ;
	    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIGRAPHS
	    c = ins_digraph();
	    ਜੇ (c == NUL)
		ਰੋਕੋ;
#ਜੇ_ਅੰਤ
	    ਜਾਓ normalchar;

	ਹਾਲਤ Ctrl_X:	// Enter CTRL-X mode
	    ins_ctrl_x();
	    ਰੋਕੋ;

	ਹਾਲਤ Ctrl_RSB:	// Tag name completion after ^X
	    ਜੇ (!ctrl_x_ਬਹੁਲਕ_ਟੈਗ_ਸੂਚੀ())
		ਜਾਓ normalchar;
	    ਜਾਓ docomplete;

	ਹਾਲਤ Ctrl_F:	// File name completion after ^X
	    ਜੇ (!ctrl_x_ਬਹੁਲਕ_files())
		ਜਾਓ normalchar;
	    ਜਾਓ docomplete;

	ਹਾਲਤ 's':	// Spelling completion after ^X
	ਹਾਲਤ Ctrl_S:
	    ਜੇ (!ctrl_x_ਬਹੁਲਕ_spell())
		ਜਾਓ normalchar;
	    ਜਾਓ docomplete;

	ਹਾਲਤ Ctrl_L:	// Whole line completion after ^X
	    ਜੇ (!ctrl_x_ਬਹੁਲਕ_ਸਾਰਾ_ਸਤਰ())
	    {
		// CTRL-L with 'insertmode' set: Leave Insert mode
		ਜੇ (p_im)
		{
		    ਜੇ (echeck_abbr(Ctrl_L + ABBR_OFF))
			ਰੋਕੋ;
		    ਜਾਓ doESCkey;
		}
		ਜਾਓ normalchar;
	    }
	    // FALLTHROUGH

	ਹਾਲਤ Ctrl_P:	// Do previous/next pattern completion
	ਹਾਲਤ Ctrl_N:
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
	    // CTRL-SHIFT-P/N scroll the info popup one line.
	    ਜੇ (pum_visible() && (mod_ਨਕਾਬ & MOD_ਨਕਾਬ_ਖਿਸਕਾਓ)
		    && (c == Ctrl_P || c == Ctrl_N))
	    {
		popup_scroll_info(c == Ctrl_P ? -1 : 1, false);
		ਰੋਕੋ;
	    }
#ਜੇ_ਅੰਤ
	    // if 'complete' is empty then plain ^P is no longer special,
	    // but it is under other ^X modes
	    ਜੇ (*curbuf->b_p_cpt == NUL
		    && (ctrl_x_ਬਹੁਲਕ_ਸਾਧਾਰਨ() || ctrl_x_ਬਹੁਲਕ_ਸਾਰਾ_ਸਤਰ())
		    && !compl_ਸਥਿਤੀ_local())
		ਜਾਓ normalchar;

docomplete:
	    ins_compl_clear_autocomplete_delay();
	    compl_busy = TRUE;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
	    disable_fold_update++;  // don't redraw folds here
#ਜੇ_ਅੰਤ
	    ਜੇ (ins_complete(c, TRUE) == FAIL)
		compl_ਸਥਿਤੀ_clear();
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
	    disable_fold_update--;
#ਜੇ_ਅੰਤ
	    compl_busy = FALSE;
	    can_si = may_do_si(); // allow smartindenting
	    ਰੋਕੋ;

	ਹਾਲਤ Ctrl_Y:	// copy from previous line or scroll down
	ਹਾਲਤ Ctrl_E:	// copy from next line	   or scroll up
	    c = ins_ctrl_ey(c);
	    ਰੋਕੋ;

	ਮੂਲ:
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UNIX
	    ਜੇ (c == intr_char)		// special interrupt char
		ਜਾਓ do_intr;
#ਜੇ_ਅੰਤ

normalchar:
	    /*
	     * Insert a normal character.
	     */
#ਜੇ defined(FEAT_EVAL)
	    ਜੇ (!p_paste)
	    {
		// Trigger InsertCharPre.
		char_u *str = do_ਪਾਓ_char_pre(c);
		char_u *p;

		ਜੇ (str != ਨਿਰਾ)
		{
		    ਜੇ (*str != NUL && stop_arrow() != FAIL)
		    {
			// Insert the new value of v:char literally.
			ਲਈ (p = str; *p != NUL; MB_PTR_ADV(p))
			{
			    c = PTR2CHAR(p);
			    ਜੇ (c == CAR || c == K_KENTER || c == NL)
				ins_eol(c);
			    ਨਹੀਂਤਾਂ
				ins_char(c);
			}
			Append_ਮੰਜ਼ਿਲ_Redobuff_Lit(str, -1);
		    }
		    vim_free(str);
		    c = NUL;
		}

		// If the new value is already inserted or an empty string
		// then don't insert any character.
		ਜੇ (c == NUL)
		    ਰੋਕੋ;
	    }
#ਜੇ_ਅੰਤ
	    // Try to perform smart-indenting.
	    ins_try_si(c);

	    ਜੇ (c == ' ')
	    {
		inserted_space = TRUE;
		ਜੇ (inindent(0))
		    can_cindent = FALSE;
		ਜੇ (Insstart_blank_vcol == MAXCOL
			&& curwin->w_cursor.lnum == Insstart.lnum)
		    Insstart_blank_vcol = get_nolist_virtcol();
	    }

	    // Insert a normal character and check for abbreviations on a
	    // special character.  Let CTRL-] expand abbreviations without
	    // inserting it.
	    ਜੇ (vim_iswordc(c) || (!echeck_abbr(
			// Add ABBR_OFF for characters above 0x100, this is
			// what check_abbr() expects.
				(has_mbyte && c >= 0x100) ? (c + ABBR_OFF) : c)
			&& c != Ctrl_RSB))
	    {
		ਪਾਓ_special(c, FALSE, FALSE);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RIGHTLEFT
		revins_legal++;
		revins_chars++;
#ਜੇ_ਅੰਤ
	    }

	    auto_format(FALSE, TRUE);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
	    // When inserting a character the cursor line must never be in a
	    // closed fold.
	    foldOpenCursor();
#ਜੇ_ਅੰਤ
	    // Trigger autocompletion
	    ਜੇ (ins_compl_has_autocomplete() && !char_avail()
		    && vim_isprintc(c))
		TRIGGER_AUTOCOMPLETE();

	    ਰੋਕੋ;
	}   // end of switch (c)

	// If typed something may trigger CursorHoldI again; K_COMPLETE_DELAY is
	// injected, not typed.
	ਜੇ (c != K_CURSORHOLD && c != K_COMPLETE_DELAY
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_COMPL_FUNC
		// but not in CTRL-X mode, a script can't restore the state
		&& ctrl_x_ਬਹੁਲਕ_ਸਾਧਾਰਨ()
#ਜੇ_ਅੰਤ
	       )
	    did_cursorhold = FALSE;

	// Check if we need to cancel completion mode because the window
	// or tab page was changed
	ਜੇ (ins_compl_ਸਰਗਰਮ() && !ins_compl_win_ਸਰਗਰਮ(curwin))
	    ins_compl_cancel();

	// If the cursor was moved we didn't just insert a space
	ਜੇ (arrow_ਵਰਤਿਆ)
	    inserted_space = FALSE;

	ਜੇ (can_cindent && cindent_on() && ctrl_x_ਬਹੁਲਕ_ਸਾਧਾਰਨ())
	{
force_cindent:
	    /*
	     * Indent now if a key was typed that is in 'cinkeys'.
	     */
	    ਜੇ (ਵਿੱਚ_cinkeys(c, ' ', ਸਤਰ_is_white))
	    {
		ਜੇ (stop_arrow() == OK)
		    // re-indent the current line
		    do_c_ਵਿਅੰਜਕ_indent();
	    }
	}

    }	// for (;;)
    // NOTREACHED
}

    ਪੂਰਨ
ins_need_undo_get(ਖਾਲੀ)
{
    ਵਾਪਸ ins_need_undo;
}

/*
 * Redraw for Insert mode.
 * This is postponed until getting the next character to make '$' in the 'cpo'
 * option work correctly.
 * Only redraw when there are no characters available.  This speeds up
 * inserting sequences of characters (e.g., for CTRL-R).
 */
    ਖਾਲੀ
ins_redraw(ਪੂਰਨ ready)	    // not busy with something
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CONCEAL
    linenr_T	conceal_old_cursor_ਸਤਰ = 0;
    linenr_T	conceal_new_cursor_ਸਤਰ = 0;
    ਪੂਰਨ		conceal_update_ਸਤਰਾਂ = FALSE;
#ਜੇ_ਅੰਤ

    ਜੇ (char_avail())
	ਵਾਪਸ;

    // Trigger CursorMoved if the cursor moved.  Not when the popup menu is
    // visible, the command might delete it.
    ਜੇ (ready && (has_cursormovedI()
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
		|| popup_visible
#ਜੇ_ਅੰਤ
#ਜੇ defined(FEAT_CONCEAL)
		|| curwin->w_p_cole > 0
#ਜੇ_ਅੰਤ
		)
	    && !EQUAL_POS(ਆਖਰੀ_cursormoved, curwin->w_cursor)
	    && !pum_visible())
    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
	// Need to update the screen first, to make sure syntax
	// highlighting is correct after making a change (e.g., inserting
	// a "(".  The autocommand may also require a redraw, so it's done
	// again below, unfortunately.
	ਜੇ (syntax_present(curwin) && must_redraw)
	    update_screen(0);
#ਜੇ_ਅੰਤ
	ਜੇ (has_cursormovedI())
	{
	    // Make sure curswant is correct, an autocommand may call
	    // getcurpos().
	    update_curswant();
	    ins_apply_autocmds(EVENT_CURSORMOVEDI);
	}
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
	ਜੇ (popup_visible)
	    popup_check_cursor_pos();
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CONCEAL
	ਜੇ (curwin->w_p_cole > 0)
	{
	    conceal_old_cursor_ਸਤਰ = ਆਖਰੀ_cursormoved.lnum;
	    conceal_new_cursor_ਸਤਰ = curwin->w_cursor.lnum;
	    conceal_update_ਸਤਰਾਂ = TRUE;
	}
#ਜੇ_ਅੰਤ
	ਆਖਰੀ_cursormoved = curwin->w_cursor;
    }

    // Trigger TextChangedI if b_changedtick_i differs.
    ਜੇ (ready && has_textchangedI()
	    && curbuf->b_ਆਖਰੀ_changedtick_i != CHANGEDTICK(curbuf)
	    && !pum_visible())
    {
	aco_save_T	aco;
	varnumber_T	tick = CHANGEDTICK(curbuf);

	// Save and restore curwin and curbuf, in case the autocmd changes
	// them.
	aucmd_prepbuf(&aco, curbuf);
	apply_autocmds(EVENT_TEXTCHANGEDI, ਨਿਰਾ, ਨਿਰਾ, FALSE, curbuf);
	aucmd_restbuf(&aco);
	curbuf->b_ਆਖਰੀ_changedtick_i = CHANGEDTICK(curbuf);
	ਜੇ (tick != CHANGEDTICK(curbuf))  // see ins_apply_autocmds()
	    u_save(curwin->w_cursor.lnum,
					(linenr_T)(curwin->w_cursor.lnum + 1));
    }

    // Trigger TextChangedP if b_changedtick_pum differs. When the popupmenu
    // closes TextChangedI will need to trigger for backwards compatibility,
    // thus use different b_last_changedtick* variables.
    ਜੇ (ready && has_textchangedP()
	    && curbuf->b_ਆਖਰੀ_changedtick_pum != CHANGEDTICK(curbuf)
	    && pum_visible())
    {
	aco_save_T	aco;
	varnumber_T	tick = CHANGEDTICK(curbuf);

	// Save and restore curwin and curbuf, in case the autocmd changes
	// them.
	aucmd_prepbuf(&aco, curbuf);
	apply_autocmds(EVENT_TEXTCHANGEDP, ਨਿਰਾ, ਨਿਰਾ, FALSE, curbuf);
	aucmd_restbuf(&aco);
	curbuf->b_ਆਖਰੀ_changedtick_pum = CHANGEDTICK(curbuf);
	ਜੇ (tick != CHANGEDTICK(curbuf))  // see ins_apply_autocmds()
	    u_save(curwin->w_cursor.lnum,
					(linenr_T)(curwin->w_cursor.lnum + 1));
    }

    ਜੇ (ready)
	may_trigger_win_scrolled_resized();

    // Trigger SafeState if nothing is pending.
    may_trigger_safestate(ready
	    && !ins_compl_ਸਰਗਰਮ()
	    && !pum_visible());

#ਜੇ defined(FEAT_CONCEAL)
    ਜੇ ((conceal_update_ਸਤਰਾਂ
	    && (conceal_old_cursor_ਸਤਰ != conceal_new_cursor_ਸਤਰ
		|| conceal_cursor_ਸਤਰ(curwin)))
	    || need_cursor_ਸਤਰ_redraw)
    {
	ਜੇ (conceal_old_cursor_ਸਤਰ != conceal_new_cursor_ਸਤਰ)
	    redrawWinline(curwin, conceal_old_cursor_ਸਤਰ);
	redrawWinline(curwin, conceal_new_cursor_ਸਤਰ == 0
			    ? curwin->w_cursor.lnum : conceal_new_cursor_ਸਤਰ);
	curwin->w_valid &= ~VALID_CROW;
	need_cursor_ਸਤਰ_redraw = FALSE;
    }
#ਜੇ_ਅੰਤ
    ਜੇ (must_redraw)
	update_screen(0);
    ਨਹੀਂਤਾਂ ਜੇ (clear_cmdline || redraw_cmdline)
	showmode();		// clear cmdline and show mode
    showruler(FALSE);
    setcursor();
    emsg_on_display = FALSE;	// may remove error message now
}

/*
 * Handle a CTRL-V or CTRL-Q typed in Insert mode.
 */
    ਸਥਿਰ ਖਾਲੀ
ins_ctrl_v(ਖਾਲੀ)
{
    ਪੂਰਨ		c;
    ਪੂਰਨ		did_putchar = FALSE;

    // may need to redraw when no more chars available now
    ins_redraw(FALSE);

    ਜੇ (redrawing() && !char_avail())
    {
	edit_putchar('^', TRUE);
	did_putchar = TRUE;
    }
    Append_ਮੰਜ਼ਿਲ_Redobuff((char_u *)CTRL_V_STR);	// CTRL-V

    ਜੋੜੋ_ਮੰਜ਼ਿਲ_showcmd_c(Ctrl_V);

    // Do not change any modifyOtherKeys ESC sequence to a normal key for
    // CTRL-SHIFT-V.
    c = get_literal(mod_ਨਕਾਬ & MOD_ਨਕਾਬ_ਖਿਸਕਾਓ);
    ਜੇ (did_putchar)
	// when the line fits in 'columns' the '^' is at the start of the next
	// line and will not removed by the redraw
	edit_unputchar();
    clear_showcmd();

    ਪਾਓ_special(c, FALSE, TRUE);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RIGHTLEFT
    revins_chars++;
    revins_legal++;
#ਜੇ_ਅੰਤ
}

/*
 * After getting an ESC or CSI for a literal key: If the typeahead buffer
 * contains a modifyOtherKeys sequence then decode it and return the result.
 * Otherwise return "c".
 * Note that this doesn't wait for characters, they must be in the typeahead
 * buffer already.
 */
    ਸਥਿਰ ਪੂਰਨ
decode_Modify_Other_ਕੁੰਜੀਆਂ(ਪੂਰਨ c)
{
    char_u  *p = typebuf.tb_buf + typebuf.tb_off;
    ਪੂਰਨ	    idx;
    ਪੂਰਨ	    form = 0;
    ਪੂਰਨ	    argidx = 0;
    ਪੂਰਨ	    arg[2] = {0, 0};

    // Recognize:
    // form 0: {lead}{key};{modifier}u
    // form 1: {lead}27;{modifier};{key}~
    ਜੇ (typebuf.tb_len >= 4 && (c == CSI || (c == ESC && *p == '[')))
    {
	idx = (*p == '[');
	ਜੇ (p[idx] == '2' && p[idx + 1] == '7' && p[idx + 2] == ';' &&
		kitty_protocol_state != KKPS_ENABLED)
	{
	    form = 1;
	    idx += 3;
	}
	ਜਦੋਂ (idx < typebuf.tb_len && argidx < 2)
	{
	    ਜੇ (p[idx] == ';')
		++argidx;
	    ਨਹੀਂਤਾਂ ਜੇ (VIM_ISDIGIT(p[idx]))
		arg[argidx] = arg[argidx] * 10 + (p[idx] - '0');
	    ਨਹੀਂਤਾਂ
		ਰੋਕੋ;
	    ++idx;
	}
	ਪੂਰਨ kitty_no_mods = argidx == 0 && kitty_protocol_state == KKPS_ENABLED;
	ਜੇ (idx < typebuf.tb_len
		&& p[idx] == (form == 1 ? '~' : 'u')
		&& (argidx == 1 || kitty_no_mods))
	{
	    // Match, consume the code.
	    typebuf.tb_off += idx + 1;
	    typebuf.tb_len -= idx + 1;
#ਜੇ defined(FEAT_CLIENTSERVER) || defined(FEAT_EVAL)
	    ਜੇ (typebuf.tb_len == 0)
		typebuf_was_filled = FALSE;
#ਜੇ_ਅੰਤ

	    mod_ਨਕਾਬ = kitty_no_mods ? 0 : decode_modifiers(arg[!form]);
	    c = ਮਿਲਾਓ_modify_Other_ਕੁੰਜੀਆਂ(arg[form], &mod_ਨਕਾਬ);
	}
    }

    ਵਾਪਸ c;
}

/*
 * Put a character directly onto the screen.  It's not stored in a buffer.
 * Used while handling CTRL-K, CTRL-V, etc. in Insert mode.
 */
ਸਥਿਰ ਪੂਰਨ  pc_ਸਥਿਤੀ;
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PC_ਸਥਿਤੀ_UNSET	0	// pc_bytes was not set
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PC_ਸਥਿਤੀ_ਸੱਜਾ	1	// right half of double-wide char
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PC_ਸਥਿਤੀ_ਖੱਬਾ	2	// left half of double-wide char
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PC_ਸਥਿਤੀ_ਸੈੱਟ	3	// pc_bytes was filled
ਸਥਿਰ char_u pc_ਬਾਈਟਾਂ[MB_MAXBYTES + 1]; // saved bytes
ਸਥਿਰ ਪੂਰਨ  pc_attr;
ਸਥਿਰ ਪੂਰਨ  pc_ਪੰਕਤੀ;
ਸਥਿਰ ਪੂਰਨ  pc_ਖਾਨਾ;

    ਖਾਲੀ
edit_putchar(ਪੂਰਨ c, ਪੂਰਨ highlight)
{
    ਪੂਰਨ	    attr;

    ਜੇ (Screen_ਸਤਰਾਂ == ਨਿਰਾ)
	ਵਾਪਸ;

    update_topline();	// just in case w_topline isn't valid
    validate_cursor();
    ਜੇ (highlight)
	attr = HL_ATTR(HLF_8);
    ਨਹੀਂਤਾਂ
	attr = 0;
    pc_ਪੰਕਤੀ = W_WINROW(curwin) + curwin->w_wrow;
    pc_ਖਾਨਾ = curwin->w_wincol;
    pc_ਸਥਿਤੀ = PC_ਸਥਿਤੀ_UNSET;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RIGHTLEFT
    ਜੇ (curwin->w_p_rl)
    {
	pc_ਖਾਨਾ += curwin->w_ਚੌੜਾਈ - 1 - curwin->w_wcol;
	ਜੇ (has_mbyte)
	{
	    ਪੂਰਨ fix_ਖਾਨਾ = mb_fix_ਖਾਨਾ(pc_ਖਾਨਾ, pc_ਪੰਕਤੀ);

	    ਜੇ (fix_ਖਾਨਾ != pc_ਖਾਨਾ)
	    {
		screen_putchar(' ', pc_ਪੰਕਤੀ, fix_ਖਾਨਾ, attr);
		--curwin->w_wcol;
		pc_ਸਥਿਤੀ = PC_ਸਥਿਤੀ_ਸੱਜਾ;
	    }
	}
    }
    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
    {
	pc_ਖਾਨਾ += curwin->w_wcol;
	ਜੇ (mb_lefthalve(pc_ਪੰਕਤੀ, pc_ਖਾਨਾ))
	    pc_ਸਥਿਤੀ = PC_ਸਥਿਤੀ_ਖੱਬਾ;
    }

    // save the character to be able to put it back
    ਜੇ (pc_ਸਥਿਤੀ == PC_ਸਥਿਤੀ_UNSET)
    {
	screen_getbytes(pc_ਪੰਕਤੀ, pc_ਖਾਨਾ, pc_ਬਾਈਟਾਂ, &pc_attr);
	pc_ਸਥਿਤੀ = PC_ਸਥਿਤੀ_ਸੈੱਟ;
    }
    screen_putchar(c, pc_ਪੰਕਤੀ, pc_ਖਾਨਾ, attr);
}

#ਜੇ defined(FEAT_ਕੰਮ_CHANNEL)
/*
 * Set the insert start position for when using a prompt buffer.
 */
    ਖਾਲੀ
ਸੈੱਟ_insstart(linenr_T lnum, ਪੂਰਨ ਖਾਨਾ)
{
    Insstart.lnum = lnum;
    Insstart.ਖਾਨਾ = ਖਾਨਾ;
    Insstart_orig = Insstart;
    Insstart_textlen = Insstart.ਖਾਨਾ;
    Insstart_blank_vcol = MAXCOL;
    arrow_ਵਰਤਿਆ = FALSE;
}
#ਜੇ_ਅੰਤ

/*
 * Undo the previous edit_putchar().
 */
    ਖਾਲੀ
edit_unputchar(ਖਾਲੀ)
{
    ਜੇ (pc_ਸਥਿਤੀ != PC_ਸਥਿਤੀ_UNSET && pc_ਪੰਕਤੀ >= ਸੁਨੇਹਾ_scrolled)
    {
	ਜੇ (pc_ਸਥਿਤੀ == PC_ਸਥਿਤੀ_ਸੱਜਾ)
	    ++curwin->w_wcol;
	ਜੇ (pc_ਸਥਿਤੀ == PC_ਸਥਿਤੀ_ਸੱਜਾ || pc_ਸਥਿਤੀ == PC_ਸਥਿਤੀ_ਖੱਬਾ)
	    redrawWinline(curwin, curwin->w_cursor.lnum);
	ਨਹੀਂਤਾਂ
	    screen_puts(pc_ਬਾਈਟਾਂ, pc_ਪੰਕਤੀ - ਸੁਨੇਹਾ_scrolled, pc_ਖਾਨਾ, pc_attr);
    }
}

/*
 * Called when "$" is in 'cpoptions': display a '$' at the end of the changed
 * text.  Only works when cursor is in the line that changes.
 */
    ਖਾਲੀ
display_dollar(colnr_T ਖਾਨਾ_arg)
{
    colnr_T ਖਾਨਾ = ਖਾਨਾ_arg < 0 ? 0 : ਖਾਨਾ_arg;
    colnr_T save_ਖਾਨਾ;

    ਜੇ (!redrawing())
	ਵਾਪਸ;

    cursor_off();
    save_ਖਾਨਾ = curwin->w_cursor.ਖਾਨਾ;
    curwin->w_cursor.ਖਾਨਾ = ਖਾਨਾ;
    ਜੇ (has_mbyte)
    {
	char_u *p;

	// If on the last byte of a multi-byte move to the first byte.
	p = ml_get_curline();
	curwin->w_cursor.ਖਾਨਾ -= (*mb_ਸਿਰ_off)(p, p + ਖਾਨਾ);
    }
    curs_columns(FALSE);	    // recompute w_wrow and w_wcol
    ਜੇ (curwin->w_wcol < curwin->w_ਚੌੜਾਈ)
    {
	edit_putchar('$', FALSE);
	dollar_vcol = curwin->w_virtcol;
    }
    curwin->w_cursor.ਖਾਨਾ = save_ਖਾਨਾ;
}

/*
 * Call this function before moving the cursor from the normal insert position
 * in insert mode.
 */
    ਖਾਲੀ
undisplay_dollar(ਖਾਲੀ)
{
    ਜੇ (dollar_vcol < 0)
	ਵਾਪਸ;

    dollar_vcol = -1;
    redrawWinline(curwin, curwin->w_cursor.lnum);
}

/*
 * Truncate the space at the end of a line.  This is to be used only in an
 * insert mode.  It handles fixing the replace stack for MODE_REPLACE and
 * MODE_VREPLACE modes.
 */
    ਖਾਲੀ
truncate_spaces(char_u *ਸਤਰ, size_t len)
{
    ਪੂਰਨ	    i;

    // find start of trailing white space
    ਲਈ (i = (ਪੂਰਨ)len - 1; i >= 0 && VIM_ISWHITE(ਸਤਰ[i]); i--)
    {
	ਜੇ (State & REPLACE_FLAG)
	    replace_join(0);	    // remove a NUL from the replace stack
    }
    ਸਤਰ[i + 1] = NUL;
}

/*
 * Backspace the cursor until the given column.  Handles MODE_REPLACE and
 * MODE_VREPLACE modes correctly.  May also be used when not in insert mode at
 * all.  Will attempt not to go before "col" even when there is a composing
 * character.
 */
    ਖਾਲੀ
backspace_until_ਕਾਲਮ(ਪੂਰਨ ਖਾਨਾ)
{
    ਜਦੋਂ ((ਪੂਰਨ)curwin->w_cursor.ਖਾਨਾ > ਖਾਨਾ)
    {
	curwin->w_cursor.ਖਾਨਾ--;
	ਜੇ (State & REPLACE_FLAG)
	    replace_do_bs(ਖਾਨਾ);
	ਨਹੀਂਤਾਂ ਜੇ (!del_char_after_ਖਾਨਾ(ਖਾਨਾ))
	    ਰੋਕੋ;
    }
}

/*
 * Like del_char(), but make sure not to go before column "limit_col".
 * Only matters when there are composing characters.
 * Return TRUE when something was deleted.
 */
    ਸਥਿਰ ਪੂਰਨ
del_char_after_ਖਾਨਾ(ਪੂਰਨ ਹੱਦਬੰਦੀ_ਖਾਨਾ ਅਣਵਰਤਿਆ)
{
    ਜੇ (enc_utf8 && ਹੱਦਬੰਦੀ_ਖਾਨਾ >= 0)
    {
	colnr_T ecol = curwin->w_cursor.ਖਾਨਾ + 1;

	// Make sure the cursor is at the start of a character, but
	// skip forward again when going too far back because of a
	// composing character.
	mb_adjust_cursor();
	ਜਦੋਂ (curwin->w_cursor.ਖਾਨਾ < (colnr_T)ਹੱਦਬੰਦੀ_ਖਾਨਾ)
	{
	    ਪੂਰਨ l = utf_ptr2len(ml_get_cursor());

	    ਜੇ (l == 0)  // end of line
		ਰੋਕੋ;
	    curwin->w_cursor.ਖਾਨਾ += l;
	}
	ਜੇ (*ml_get_cursor() == NUL || curwin->w_cursor.ਖਾਨਾ == ecol)
	    ਵਾਪਸ FALSE;
	del_ਬਾਈਟਾਂ((ਲੰਮਾ)((ਪੂਰਨ)ecol - curwin->w_cursor.ਖਾਨਾ), FALSE, TRUE);
    }
    ਨਹੀਂਤਾਂ
	(ਖਾਲੀ)del_char(FALSE);
    ਵਾਪਸ TRUE;
}

/*
 * Next character is interpreted literally.
 * A one, two or three digit decimal number is interpreted as its byte value.
 * If one or two digits are entered, the next character is given to vungetc().
 * For Unicode a character > 255 may be returned.
 * If "noReduceKeys" is TRUE do not change any modifyOtherKeys ESC sequence
 * into a normal key, return ESC.
 */
    ਪੂਰਨ
get_literal(ਪੂਰਨ no_Reduce_ਕੁੰਜੀਆਂ)
{
    ਪੂਰਨ		cc;
    ਪੂਰਨ		nc;
    ਪੂਰਨ		i;
    ਪੂਰਨ		hex = FALSE;
    ਪੂਰਨ		octal = FALSE;
    ਪੂਰਨ		unicode = 0;

    ਜੇ (got_int)
	ਵਾਪਸ Ctrl_C;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
    /*
     * In GUI there is no point inserting the internal code for a special key.
     * It is more useful to insert the string "<KEY>" instead.	This would
     * probably be useful in a text window too, but it would not be
     * vi-compatible (maybe there should be an option for it?) -- webb
     */
    ਜੇ (gui.ਵਿੱਚ_use)
    {
	++allow_ਕੁੰਜੀਆਂ;
	ਜੇ (no_Reduce_ਕੁੰਜੀਆਂ)
	    ++no_reduce_ਕੁੰਜੀਆਂ;
    }
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_ON_FLY_SCROLL
    dont_scroll = TRUE;		// disallow scrolling here
#ਜੇ_ਅੰਤ
    ++no_mapping;		// don't map the next key hits
    cc = 0;
    i = 0;
    ਲਈ (;;)
    {
	nc = plain_vgetc();
	ਜੇ ((nc == ESC || nc == CSI) && !no_Reduce_ਕੁੰਜੀਆਂ)
	    nc = decode_Modify_Other_ਕੁੰਜੀਆਂ(nc);

	ਜੇ ((mod_ਨਕਾਬ & ~MOD_ਨਕਾਬ_ਖਿਸਕਾਓ) != 0)
	    // A character with non-Shift modifiers should not be a valid
	    // character for i_CTRL-V_digit.
	    ਰੋਕੋ;

	ਜੇ ((State & ਬਹੁਲਕ_CMDLINE) == 0 && MB_BYTE2LEN_CHECK(nc) == 1)
	    ਜੋੜੋ_ਮੰਜ਼ਿਲ_showcmd(nc);
	ਜੇ (nc == 'x' || nc == 'X')
	    hex = TRUE;
	ਨਹੀਂਤਾਂ ਜੇ (nc == 'o' || nc == 'O')
	    octal = TRUE;
	ਨਹੀਂਤਾਂ ਜੇ (nc == 'u' || nc == 'U')
	    unicode = nc;
	ਨਹੀਂਤਾਂ
	{
	    ਜੇ (hex || unicode != 0)
	    {
		ਜੇ (!vim_isxdigit(nc))
		    ਰੋਕੋ;
		cc = cc * 16 + hex2nr(nc);
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (octal)
	    {
		ਜੇ (nc < '0' || nc > '7')
		    ਰੋਕੋ;
		cc = cc * 8 + nc - '0';
	    }
	    ਨਹੀਂਤਾਂ
	    {
		ਜੇ (!VIM_ISDIGIT(nc))
		    ਰੋਕੋ;
		cc = cc * 10 + nc - '0';
	    }

	    ++i;
	}

	ਜੇ (cc > 255 && unicode == 0)
	    cc = 255;		// limit range to 0-255
	nc = 0;

	ਜੇ (hex)		// hex: up to two chars
	{
	    ਜੇ (i >= 2)
		ਰੋਕੋ;
	}
	ਨਹੀਂਤਾਂ ਜੇ (unicode)	// Unicode: up to four or eight chars
	{
	    ਜੇ ((unicode == 'u' && i >= 4) || (unicode == 'U' && i >= 8))
		ਰੋਕੋ;
	}
	ਨਹੀਂਤਾਂ ਜੇ (i >= 3)	// decimal or octal: up to three chars
	    ਰੋਕੋ;
    }
    ਜੇ (i == 0)	    // no number entered
    {
	ਜੇ (nc == K_ZERO)   // NUL is stored as NL
	{
	    cc = '\n';
	    nc = 0;
	}
	ਨਹੀਂਤਾਂ
	{
	    cc = nc;
	    nc = 0;
	}
    }

    ਜੇ (cc == 0)	// NUL is stored as NL
	cc = '\n';
    ਜੇ (enc_dbcs && (cc & 0xff) == 0)
	cc = '?';	// don't accept an illegal DBCS char, the NUL in the
			// second byte will cause trouble!

    --no_mapping;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
    ਜੇ (gui.ਵਿੱਚ_use)
    {
	--allow_ਕੁੰਜੀਆਂ;
	ਜੇ (no_Reduce_ਕੁੰਜੀਆਂ)
	    --no_reduce_ਕੁੰਜੀਆਂ;
    }
#ਜੇ_ਅੰਤ
    ਜੇ (nc)
    {
	vungetc(nc);
	// A character typed with i_CTRL-V_digit cannot have modifiers.
	mod_ਨਕਾਬ = 0;
    }
    got_int = FALSE;	    // CTRL-C typed after CTRL-V is not an interrupt
    ਵਾਪਸ cc;
}

/*
 * Insert character, taking care of special keys and mod_mask
 */
    ਸਥਿਰ ਖਾਲੀ
ਪਾਓ_special(
    ਪੂਰਨ	    c,
    ਪੂਰਨ	    allow_modmask,
    ਪੂਰਨ	    ctrlv)	    // c was typed after CTRL-V
{
    char_u  *p;
    ਪੂਰਨ	    len;

    /*
     * Special function key, translate into "<Key>". Up to the last '>' is
     * inserted with ins_str(), so as not to replace characters in replace
     * mode.
     * Only use mod_mask for special keys, to avoid things like <S-Space>,
     * unless 'allow_modmask' is TRUE.
     */
#ਜੇ defined(MACOS_X) || defined(FEAT_GUI_GTK)
    // Command-key never produces a normal key
    ਜੇ (mod_ਨਕਾਬ & MOD_ਨਕਾਬ_CMD)
	allow_modmask = TRUE;
#ਜੇ_ਅੰਤ
    ਜੇ (IS_SPECIAL(c) || (mod_ਨਕਾਬ && allow_modmask))
    {
	p = get_special_ਕੁੰਜੀ_name(c, mod_ਨਕਾਬ);
	len = (ਪੂਰਨ)STRLEN(p);
	c = p[len - 1];
	ਜੇ (len > 2)
	{
	    ਜੇ (stop_arrow() == FAIL)
		ਵਾਪਸ;
	    p[len - 1] = NUL;
	    ins_str(p, len - 1);
	    Append_ਮੰਜ਼ਿਲ_Redobuff_Lit(p, -1);
	    ctrlv = FALSE;
	}
    }
    ਜੇ (stop_arrow() == OK)
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
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ISSPECIAL(c)	((c) < ' ' || (c) >= DEL || (c) == '0' || (c) == '^')

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
    ਖਾਲੀ
insertchar(
    ਪੂਰਨ		c,			// character to insert or NUL
    ਪੂਰਨ		flags,			// INSCHAR_FORMAT, etc.
    ਪੂਰਨ		ਦੂਜਾ_indent)		// indent for second line if >= 0
{
    ਪੂਰਨ		textwidth;
    char_u	*p;
    ਪੂਰਨ		fo_ins_blank;
    ਪੂਰਨ		force_format = flags & INSCHAR_FORMAT;

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
    ਜੇ (textwidth > 0
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
#ਜੇ defined(FEAT_EVAL)
	ਪੂਰਨ     do_ਅੰਦਰੂਨੀ = TRUE;
	colnr_T virtcol = get_nolist_virtcol()
				  + char2cells(c != NUL ? c : gchar_cursor());

	ਜੇ (*curbuf->b_p_fex != NUL && (flags & INSCHAR_NO_FEX) == 0
		&& (force_format || virtcol > (colnr_T)textwidth))
	{
	    do_ਅੰਦਰੂਨੀ = (fex_format(curwin->w_cursor.lnum, 1L, c) != 0);
	    // It may be required to save for undo again, e.g. when setline()
	    // was called.
	    ins_need_undo = TRUE;
	}
	ਜੇ (do_ਅੰਦਰੂਨੀ)
#ਜੇ_ਅੰਤ
	    ਅੰਦਰੂਨੀ_format(textwidth, ਦੂਜਾ_indent, flags, c == NUL, c);
    }

    ਜੇ (c == NUL)	    // only formatting was wanted
	ਵਾਪਸ;

    // Check whether this character should end a comment.
    ਜੇ (did_ai && c == ਸਿਰਾ_comment_pending)
    {
	char_u  *ਸਤਰ;
	char_u	lead_ਸਿਰਾ[COM_MAX_LEN];	    // end-comment string
	ਪੂਰਨ	ਵਿਚਕਾਰ_len, ਸਿਰਾ_len;
	ਪੂਰਨ	i;

	/*
	 * Need to remove existing (middle) comment leader and insert end
	 * comment leader.  First, check what comment leader we can find.
	 */
	i = get_leader_len(ਸਤਰ = ml_get_curline(), &p, FALSE, TRUE);
	ਜੇ (i > 0 && vim_strchr(p, COM_ਵਿਚਕਾਰ) != ਨਿਰਾ)	// Just checking
	{
	    // Skip middle-comment string
	    ਜਦੋਂ (*p && p[-1] != ':')	// find end of middle flags
		++p;
	    ਵਿਚਕਾਰ_len = copy_option_part(&p, lead_ਸਿਰਾ, COM_MAX_LEN, ",");
	    // Don't count trailing white space for middle_len
	    ਜਦੋਂ (ਵਿਚਕਾਰ_len > 0 && VIM_ISWHITE(lead_ਸਿਰਾ[ਵਿਚਕਾਰ_len - 1]))
		--ਵਿਚਕਾਰ_len;

	    // Find the end-comment string
	    ਜਦੋਂ (*p && p[-1] != ':')	// find end of end flags
		++p;
	    ਸਿਰਾ_len = copy_option_part(&p, lead_ਸਿਰਾ, COM_MAX_LEN, ",");

	    // Skip white space before the cursor
	    i = curwin->w_cursor.ਖਾਨਾ;
	    ਜਦੋਂ (--i >= 0 && VIM_ISWHITE(ਸਤਰ[i]))
		;
	    i++;

	    // Skip to before the middle leader
	    i -= ਵਿਚਕਾਰ_len;

	    // Check some expected things before we go on
	    ਜੇ (i >= 0 && ਸਿਰਾ_len > 0 && lead_ਸਿਰਾ[ਸਿਰਾ_len - 1] == ਸਿਰਾ_comment_pending)
	    {
		// Backspace over all the stuff we want to replace
		backspace_until_ਕਾਲਮ(i);

		// Insert the end-comment string, except for the last
		// character, which will get inserted as normal later.
		ins_ਬਾਈਟਾਂ_len(lead_ਸਿਰਾ, ਸਿਰਾ_len - 1);
	    }
	}
    }
    ਸਿਰਾ_comment_pending = NUL;

    did_ai = FALSE;
    did_si = FALSE;
    can_si = FALSE;
    can_si_ਪਿੱਠ = FALSE;

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
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_ON_FLY_SCROLL
    dont_scroll = FALSE;		// allow scrolling here
#ਜੇ_ਅੰਤ

    ਜੇ (       !ISSPECIAL(c)
	    && (!has_mbyte || (*mb_char2len)(c) == 1)
	    && !has_insertcharpre()
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
	    // Skip typeahead if test_override("char_avail", 1) was called.
	    && !disable_char_avail_for_testing
#ਜੇ_ਅੰਤ
	    && vpeekc() != NUL
	    && !(State & REPLACE_FLAG)
	    && !cindent_on()
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RIGHTLEFT
	    && !p_ri
#ਜੇ_ਅੰਤ
	   )
    {
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਨਿਵੇਸ਼_BUFLEN 100
	char_u		buf[ਨਿਵੇਸ਼_BUFLEN + 1];
	ਪੂਰਨ		i;
	colnr_T		virtcol = 0;

	buf[0] = c;
	i = 1;
	ਜੇ (textwidth > 0)
	    virtcol = get_nolist_virtcol();
	/*
	 * Stop the string when:
	 * - no more chars available
	 * - finding a special character (command key)
	 * - buffer is full
	 * - running into the 'textwidth' boundary
	 * - need to check for abbreviation: A non-word char after a word-char
	 */
	ਜਦੋਂ (	   (c = vpeekc()) != NUL
		&& !ISSPECIAL(c)
		&& (!has_mbyte || MB_BYTE2LEN_CHECK(c) == 1)
		&& i < ਨਿਵੇਸ਼_BUFLEN
		&& (textwidth == 0
		    || (virtcol += byte2cells(buf[i - 1])) < (colnr_T)textwidth)
		&& !(!no_abbr && !vim_iswordc(c) && vim_iswordc(buf[i - 1])))
	{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RIGHTLEFT
	    c = vgetc();
	    ਜੇ (p_hkmap && ਕੁੰਜੀ_Typed)
		c = hkmap(c);		    // Hebrew mode mapping
	    buf[i++] = c;
#ਨਹੀਂਤਾਂ
	    buf[i++] = vgetc();
#ਜੇ_ਅੰਤ
	}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIGRAPHS
	do_digraph(-1);			// clear digraphs
	do_digraph(buf[i-1]);		// may be the start of a digraph
#ਜੇ_ਅੰਤ
	buf[i] = NUL;
	ins_str(buf, i);
	ਜੇ (flags & INSCHAR_CTRLV)
	{
	    redo_literal(*buf);
	    i = 1;
	}
	ਨਹੀਂਤਾਂ
	    i = 0;
	ਜੇ (buf[i] != NUL)
	    Append_ਮੰਜ਼ਿਲ_Redobuff_Lit(buf + i, -1);
    }
    ਨਹੀਂਤਾਂ
    {
	ਪੂਰਨ		cc;

	ਜੇ (has_mbyte && (cc = (*mb_char2len)(c)) > 1)
	{
	    char_u	buf[MB_MAXBYTES + 1];

	    (*mb_char2bytes)(c, buf);
	    buf[cc] = NUL;
	    ins_char_ਬਾਈਟਾਂ(buf, cc);
	    Append_Char_ਮੰਜ਼ਿਲ_Redobuff(c);
	}
	ਨਹੀਂਤਾਂ
	{
	    ins_char(c);
	    ਜੇ (flags & INSCHAR_CTRLV)
		redo_literal(c);
	    ਨਹੀਂਤਾਂ
		Append_Char_ਮੰਜ਼ਿਲ_Redobuff(c);
	}
    }
}

/*
 * Put a character in the redo buffer, for when just after a CTRL-V.
 */
    ਸਥਿਰ ਖਾਲੀ
redo_literal(ਪੂਰਨ c)
{
    char_u	buf[10];

    // Only digits need special treatment.  Translate them into a string of
    // three digits.
    ਜੇ (VIM_ISDIGIT(c))
    {
	vim_snprintf((ਅੱਖਰ *)buf, ਆਕਾਰ(buf), "%03d", c);
	Append_ਮੰਜ਼ਿਲ_Redobuff(buf);
    }
    ਨਹੀਂਤਾਂ
	Append_Char_ਮੰਜ਼ਿਲ_Redobuff(c);
}

/*
 * start_arrow() is called when an arrow key is used in insert mode.
 * For undo/redo it resembles hitting the <ESC> key.
 */
    ਖਾਲੀ
ਸ਼ੁਰੂ_arrow(
    pos_T    *ਸਿਰਾ_ਪਾਓ_pos)		// can be NULL
{
    ਸ਼ੁਰੂ_arrow_common(ਸਿਰਾ_ਪਾਓ_pos, TRUE);
}

/*
 * Like start_arrow() but with end_change argument.
 * Will prepare for redo of CTRL-G U if "end_change" is FALSE.
 */
    ਸਥਿਰ ਖਾਲੀ
ਸ਼ੁਰੂ_arrow_with_change(
    pos_T    *ਸਿਰਾ_ਪਾਓ_pos,		// can be NULL
    ਪੂਰਨ	      ਸਿਰਾ_change)		// end undoable change
{
    ਸ਼ੁਰੂ_arrow_common(ਸਿਰਾ_ਪਾਓ_pos, ਸਿਰਾ_change);
    ਜੇ (!ਸਿਰਾ_change)
    {
	Append_Char_ਮੰਜ਼ਿਲ_Redobuff(Ctrl_G);
	Append_Char_ਮੰਜ਼ਿਲ_Redobuff('U');
    }
}

    ਸਥਿਰ ਖਾਲੀ
ਸ਼ੁਰੂ_arrow_common(
    pos_T    *ਸਿਰਾ_ਪਾਓ_pos,		// can be NULL
    ਪੂਰਨ	      ਸਿਰਾ_change)		// end undoable change
{
    ਜੇ (!arrow_ਵਰਤਿਆ && ਸਿਰਾ_change)	// something has been inserted
    {
	Append_ਮੰਜ਼ਿਲ_Redobuff(ESC_STR);
	stop_ਪਾਓ(ਸਿਰਾ_ਪਾਓ_pos, FALSE, FALSE);
	arrow_ਵਰਤਿਆ = TRUE;	// this means we stopped the current insert
    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SPELL
    check_spell_redraw();
#ਜੇ_ਅੰਤ
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SPELL
/*
 * If we skipped highlighting word at cursor, do it now.
 * It may be skipped again, thus reset spell_redraw_lnum first.
 */
    ਸਥਿਰ ਖਾਲੀ
check_spell_redraw(ਖਾਲੀ)
{
    ਜੇ (spell_redraw_lnum != 0)
    {
	linenr_T	lnum = spell_redraw_lnum;

	spell_redraw_lnum = 0;
	redrawWinline(curwin, lnum);
    }
}

#ਜੇ_ਅੰਤ

/*
 * stop_arrow() is called before a change is made in insert mode.
 * If an arrow key has been used, start a new insertion.
 * Returns FAIL if undo is impossible, shouldn't insert then.
 */
    ਪੂਰਨ
stop_arrow(ਖਾਲੀ)
{
    ਜੇ (arrow_ਵਰਤਿਆ)
    {
	Insstart = curwin->w_cursor;	// new insertion starts here
	ਜੇ (Insstart.ਖਾਨਾ > Insstart_orig.ਖਾਨਾ && !ins_need_undo)
	    // Don't update the original insert position when moved to the
	    // right, except when nothing was inserted yet.
	    update_Insstart_orig = FALSE;
	Insstart_textlen = (colnr_T)linetabsize_str(ml_get_curline());

	ਜੇ (u_save_cursor() == OK)
	{
	    arrow_ਵਰਤਿਆ = FALSE;
	    ins_need_undo = FALSE;
	}

	ai_ਖਾਨਾ = 0;
	ਜੇ (State & VREPLACE_FLAG)
	{
	    orig_ਸਤਰ_ਗਿਣਤੀ = curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ;
	    vr_ਸਤਰਾਂ_ਬਦਲਿਆ = 1;
	}
	ResetRedobuff();
	Append_ਮੰਜ਼ਿਲ_Redobuff((char_u *)"1i");   // pretend we start an insertion
	new_ਪਾਓ_skip = 2;
    }
    ਨਹੀਂਤਾਂ ਜੇ (ins_need_undo)
    {
	ਜੇ (u_save_cursor() == OK)
	{
	    // A command or event may have moved the cursor before the next
	    // edit. Pull Insstart back only when the cursor moved above it,
	    // so that later edits can properly decide whether an extra undo
	    // entry is needed. Advancing Insstart would mis-place '[ after a
	    // register paste.
	    ਜੇ (LT_POS(curwin->w_cursor, Insstart))
	    {
		Insstart = curwin->w_cursor;
		Insstart_textlen = (colnr_T)linetabsize_str(ml_get_curline());
	    }
	    ins_need_undo = FALSE;
	}
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
    // Always open fold at the cursor line when inserting something.
    foldOpenCursor();
#ਜੇ_ਅੰਤ

    ਵਾਪਸ (arrow_ਵਰਤਿਆ || ins_need_undo ? FAIL : OK);
}

/*
 * Do a few things to stop inserting.
 * "end_insert_pos" is where insert ended.  It is NULL when we already jumped
 * to another window/buffer.
 */
    ਸਥਿਰ ਖਾਲੀ
stop_ਪਾਓ(
    pos_T	*ਸਿਰਾ_ਪਾਓ_pos,
    ਪੂਰਨ		esc,			// called by ins_esc()
    ਪੂਰਨ		nomove)			// <c-\><c-o>, don't move cursor
{
    ਪੂਰਨ		cc;
    string_T	inserted;

    stop_redo_ins();
    replace_flush();		// abandon replace stack

    /*
     * Save the inserted text for later redo with ^@ and CTRL-A.
     * Don't do it when "restart_edit" was set and nothing was inserted,
     * otherwise CTRL-O w and then <Left> will clear "last_insert".
     */
    inserted = get_inserted();
    ਪੂਰਨ added = inserted.string == ਨਿਰਾ ? 0 : (ਪੂਰਨ)inserted.ਲੰਮਾਈ - new_ਪਾਓ_skip;
    ਜੇ (did_restart_edit == 0 || added > 0)
    {
	vim_free(ਆਖਰੀ_ਪਾਓ.string);
	ਆਖਰੀ_ਪਾਓ = inserted;			    // structure copy
	ਆਖਰੀ_ਪਾਓ_skip = added < 0 ? 0 : new_ਪਾਓ_skip;
    }
    ਨਹੀਂਤਾਂ
	vim_free(inserted.string);

    ਜੇ (!arrow_ਵਰਤਿਆ && ਸਿਰਾ_ਪਾਓ_pos != ਨਿਰਾ)
    {
	// Auto-format now.  It may seem strange to do this when stopping an
	// insertion (or moving the cursor), but it's required when appending
	// a line and having it end in a space.  But only do it when something
	// was actually inserted, otherwise undo won't work.
	ਜੇ (!ins_need_undo && has_format_option(FO_AUTO))
	{
	    pos_T   tpos = curwin->w_cursor;

	    // When the cursor is at the end of the line after a space the
	    // formatting will move it to the following word.  Avoid that by
	    // moving the cursor onto the space.
	    cc = 'x';
	    ਜੇ (curwin->w_cursor.ਖਾਨਾ > 0 && gchar_cursor() == NUL)
	    {
		dec_cursor();
		cc = gchar_cursor();
		ਜੇ (!VIM_ISWHITE(cc))
		    curwin->w_cursor = tpos;
	    }

	    auto_format(TRUE, FALSE);

	    ਜੇ (VIM_ISWHITE(cc))
	    {
		ਜੇ (gchar_cursor() != NUL)
		    inc_cursor();
		// If the cursor is still at the same character, also keep
		// the "coladd".
		ਜੇ (gchar_cursor() == NUL
			&& curwin->w_cursor.lnum == tpos.lnum
			&& curwin->w_cursor.ਖਾਨਾ == tpos.ਖਾਨਾ)
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
	ਜੇ (!nomove && did_ai && (esc || (vim_strchr(p_cpo, CPO_INDENT) == ਨਿਰਾ
			&& curwin->w_cursor.lnum != ਸਿਰਾ_ਪਾਓ_pos->lnum))
		&& ਸਿਰਾ_ਪਾਓ_pos->lnum <= curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
	{
	    pos_T	tpos = curwin->w_cursor;
	    colnr_T	prev_ਖਾਨਾ = ਸਿਰਾ_ਪਾਓ_pos->ਖਾਨਾ;
	    colnr_T	strip_ਖਾਨਾ;

	    curwin->w_cursor = *ਸਿਰਾ_ਪਾਓ_pos;
	    check_cursor_ਖਾਨਾ();  // make sure it is not past the line

	    // Where the loop would actually start (back up if on NUL).
	    strip_ਖਾਨਾ = curwin->w_cursor.ਖਾਨਾ;
	    ਜੇ (gchar_cursor() == NUL && strip_ਖਾਨਾ > 0)
		--strip_ਖਾਨਾ;

	    // Don't strip if non-whitespace follows: setline() from a <Cmd>
	    // mapping or CursorHoldI autocmd may have inserted content.
	    ਜੇ (*skipwhite(ml_get_curline() + strip_ਖਾਨਾ) == NUL)
	    {
		curwin->w_cursor.ਖਾਨਾ = strip_ਖਾਨਾ;
		ਲਈ (;;)
		{
		    ਜੇ (gchar_cursor() == NUL && curwin->w_cursor.ਖਾਨਾ > 0)
			--curwin->w_cursor.ਖਾਨਾ;
		    cc = gchar_cursor();
		    ਜੇ (!VIM_ISWHITE(cc))
			ਰੋਕੋ;
		    ਜੇ (del_char(TRUE) == FAIL)
			ਰੋਕੋ;  // should not happen
		}
		ਜੇ (curwin->w_cursor.lnum != tpos.lnum)
		    curwin->w_cursor = tpos;
		ਨਹੀਂਤਾਂ ਜੇ (curwin->w_cursor.ਖਾਨਾ < prev_ਖਾਨਾ)
		{
		    // reset tpos, could have been invalidated in the loop above
		    tpos = curwin->w_cursor;
		    tpos.ਖਾਨਾ++;
		    ਜੇ (cc != NUL && gchar_pos(&tpos) == NUL)
			++curwin->w_cursor.ਖਾਨਾ;	// put cursor back on the NUL
		}

		// <C-S-Right> may have started Visual mode, adjust the position for
		// deleted characters.
		ਜੇ (VIsual_ਸਰਗਰਮ)
		    check_visual_pos();
	    }
	    ਨਹੀਂਤਾਂ
		// Non-whitespace follows, keep original cursor.
		curwin->w_cursor = tpos;
	}
    }
    did_ai = FALSE;
    did_si = FALSE;
    can_si = FALSE;
    can_si_ਪਿੱਠ = FALSE;

    // Set '[ and '] to the inserted text.  When end_insert_pos is NULL we are
    // now in a different buffer.
    ਜੇ (ਸਿਰਾ_ਪਾਓ_pos != ਨਿਰਾ)
    {
	curbuf->b_op_ਸ਼ੁਰੂ = Insstart;
	curbuf->b_op_ਸ਼ੁਰੂ_orig = Insstart_orig;
	curbuf->b_op_ਸਿਰਾ = *ਸਿਰਾ_ਪਾਓ_pos;
    }
}

/*
 * Set the last inserted text to a single character.
 * Used for the replace command.
 */
    ਖਾਲੀ
ਸੈੱਟ_ਆਖਰੀ_ਪਾਓ(ਪੂਰਨ c)
{
    char_u	*s;

    vim_free(ਆਖਰੀ_ਪਾਓ.string);
    ਆਖਰੀ_ਪਾਓ.string = alloc(MB_MAXBYTES * 3 + 5);
    ਜੇ (ਆਖਰੀ_ਪਾਓ.string == ਨਿਰਾ)
    {
	ਆਖਰੀ_ਪਾਓ.ਲੰਮਾਈ = 0;
	ਵਾਪਸ;
    }

    s = ਆਖਰੀ_ਪਾਓ.string;
    // Use the CTRL-V only when entering a special char
    ਜੇ (c < ' ' || c == DEL)
	*s++ = Ctrl_V;
    s = ਜੋੜੋ_char2buf(c, s);
    *s++ = ESC;
    *s = NUL;
    ਆਖਰੀ_ਪਾਓ.ਲੰਮਾਈ = (size_t)(s - ਆਖਰੀ_ਪਾਓ.string);
    ਆਖਰੀ_ਪਾਓ_skip = 0;
}

#ਜੇ defined(EXITFREE)
    ਖਾਲੀ
free_ਆਖਰੀ_ਪਾਓ(ਖਾਲੀ)
{
    VIM_CLEAR_STRING(ਆਖਰੀ_ਪਾਓ);
}
#ਜੇ_ਅੰਤ

/*
 * Add character "c" to buffer "s".  Escape the special meaning of K_SPECIAL
 * and CSI.  Handle multi-byte characters.
 * Returns a pointer to after the added bytes.
 */
    char_u *
ਜੋੜੋ_char2buf(ਪੂਰਨ c, char_u *s)
{
    char_u	ਆਰਜ਼ੀ[MB_MAXBYTES + 1];
    ਪੂਰਨ		i;
    ਪੂਰਨ		len;

    len = (*mb_char2bytes)(c, ਆਰਜ਼ੀ);
    ਲਈ (i = 0; i < len; ++i)
    {
	c = ਆਰਜ਼ੀ[i];
	// Need to escape K_SPECIAL and CSI like in the typeahead buffer.
	ਜੇ (c == K_SPECIAL)
	{
	    *s++ = K_SPECIAL;
	    *s++ = KS_SPECIAL;
	    *s++ = KE_FILLER;
	}
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
	ਨਹੀਂਤਾਂ ਜੇ (c == CSI)
	{
	    *s++ = CSI;
	    *s++ = KS_EXTRA;
	    *s++ = (ਪੂਰਨ)KE_CSI;
	}
#ਜੇ_ਅੰਤ
	ਨਹੀਂਤਾਂ
	    *s++ = c;
    }
    ਵਾਪਸ s;
}

/*
 * move cursor to start of line
 * if flags & BL_WHITE	move to first non-white
 * if flags & BL_SOL	move to first non-white if startofline is set,
 *			    otherwise keep "curswant" column
 * if flags & BL_FIX	don't leave the cursor on a NUL.
 */
    ਖਾਲੀ
beginline(ਪੂਰਨ flags)
{
    ਜੇ ((flags & BL_SOL) && !p_sol)
	coladvance(curwin->w_curswant);
    ਨਹੀਂਤਾਂ
    {
	curwin->w_cursor.ਖਾਨਾ = 0;
	curwin->w_cursor.coladd = 0;

	ਜੇ (flags & (BL_WHITE | BL_SOL))
	{
	    char_u  *ptr;

	    ਲਈ (ptr = ml_get_curline(); VIM_ISWHITE(*ptr)
			       && !((flags & BL_FIX) && ptr[1] == NUL); ++ptr)
		++curwin->w_cursor.ਖਾਨਾ;
	}
	curwin->w_ਸੈੱਟ_curswant = true;
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

    ਪੂਰਨ
oneright(ਖਾਲੀ)
{
    char_u	*ptr;
    ਪੂਰਨ		l;

    ਜੇ (virtual_ਸਰਗਰਮ())
    {
	pos_T	prevpos = curwin->w_cursor;

	// Adjust for multi-wide char (excluding TAB)
	ptr = ml_get_cursor();
	coladvance(getviscol() + ((*ptr != TAB
					  && vim_isprintc((*mb_ptr2char)(ptr)))
		    ? ptr2cells(ptr) : 1));
	curwin->w_ਸੈੱਟ_curswant = true;
	// Return OK if the cursor moved, FAIL otherwise (at window edge).
	ਵਾਪਸ (prevpos.ਖਾਨਾ != curwin->w_cursor.ਖਾਨਾ
		    || prevpos.coladd != curwin->w_cursor.coladd) ? OK : FAIL;
    }

    ptr = ml_get_cursor();
    ਜੇ (*ptr == NUL)
	ਵਾਪਸ FAIL;	    // already at the very end

    ਜੇ (has_mbyte)
	l = (*mb_ptr2len)(ptr);
    ਨਹੀਂਤਾਂ
	l = 1;

    // move "l" bytes right, but don't end up on the NUL, unless 'virtualedit'
    // contains "onemore".
    ਜੇ (ptr[l] == NUL && (get_ve_flags() & VE_ONEMORE) == 0)
	ਵਾਪਸ FAIL;
    curwin->w_cursor.ਖਾਨਾ += l;

    curwin->w_ਸੈੱਟ_curswant = true;
    adjust_skipcol();
    ਵਾਪਸ OK;
}

    ਪੂਰਨ
oneleft(ਖਾਲੀ)
{
    ਜੇ (virtual_ਸਰਗਰਮ())
    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_LINEBREAK
	ਪੂਰਨ ਚੌੜਾਈ;
#ਜੇ_ਅੰਤ
	ਪੂਰਨ v = getviscol();

	ਜੇ (v == 0)
	    ਵਾਪਸ FAIL;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_LINEBREAK
	// We might get stuck on 'showbreak', skip over it.
	ਚੌੜਾਈ = 1;
	ਲਈ (;;)
	{
	    coladvance(v - ਚੌੜਾਈ);
	    // getviscol() is slow, skip it when 'showbreak' is empty,
	    // 'breakindent' is not set and there are no multi-byte
	    // characters
	    ਜੇ ((*get_showbreak_ਮੁੱਲ(curwin) == NUL && !curwin->w_p_bri
					     && !has_mbyte) || getviscol() < v)
		ਰੋਕੋ;
	    ++ਚੌੜਾਈ;
	}
#ਨਹੀਂਤਾਂ
	coladvance(v - 1);
#ਜੇ_ਅੰਤ

	ਜੇ (curwin->w_cursor.coladd == 1)
	{
	    char_u *ptr;

	    // Adjust for multi-wide char (not a TAB)
	    ptr = ml_get_cursor();
	    ਜੇ (*ptr != TAB && vim_isprintc((*mb_ptr2char)(ptr))
							 && ptr2cells(ptr) > 1)
		curwin->w_cursor.coladd = 0;
	}

	curwin->w_ਸੈੱਟ_curswant = true;
	adjust_skipcol();
	ਵਾਪਸ OK;
    }

    ਜੇ (curwin->w_cursor.ਖਾਨਾ == 0)
	ਵਾਪਸ FAIL;

    curwin->w_ਸੈੱਟ_curswant = true;
    --curwin->w_cursor.ਖਾਨਾ;

    // if the character on the left of the current cursor is a multi-byte
    // character, move to its first byte
    ਜੇ (has_mbyte)
	mb_adjust_cursor();
    adjust_skipcol();
    ਵਾਪਸ OK;
}

/*
 * Move the cursor up "n" lines in window "wp".
 * Takes care of closed folds.
 */
    ਖਾਲੀ
cursor_ਉੱਪਰ_inner(win_T *wp, ਲੰਮਾ n)
{
    linenr_T	lnum = wp->w_cursor.lnum;

    ਜੇ (n >= lnum)
	lnum = 1;
    ਨਹੀਂਤਾਂ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
	ਜੇ (hasAnyFolding(wp))
    {
	/*
	 * Count each sequence of folded lines as one logical line.
	 */
	// go to the start of the current fold
	(ਖਾਲੀ)hasFoldingWin(wp, lnum, &lnum, ਨਿਰਾ, TRUE, ਨਿਰਾ);

	ਜਦੋਂ (n--)
	{
	    // move up one line
	    --lnum;
	    ਜੇ (lnum <= 1)
		ਰੋਕੋ;
	    // If we entered a fold, move to the beginning, unless in
	    // Insert mode or when 'foldopen' contains "all": it will open
	    // in a moment.
	    ਜੇ (n > 0 || !((State & ਬਹੁਲਕ_ਪਾਓ) || (fdo_flags & FDO_ALL)))
		(ਖਾਲੀ)hasFoldingWin(wp, lnum, &lnum, ਨਿਰਾ, TRUE, ਨਿਰਾ);
	}
	ਜੇ (lnum < 1)
	    lnum = 1;
    }
    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	lnum -= n;

    wp->w_cursor.lnum = lnum;
}

    ਪੂਰਨ
cursor_ਉੱਪਰ(
    ਲੰਮਾ	n,
    ਪੂਰਨ		upd_topline)	    // When TRUE: update topline
{
    // This fails if the cursor is already in the first line or the count is
    // larger than the line number and '-' is in 'cpoptions'
    linenr_T lnum = curwin->w_cursor.lnum;
    ਜੇ (n > 0 && (lnum <= 1
		       || (n >= lnum && vim_strchr(p_cpo, CPO_MINUS) != ਨਿਰਾ)))
	ਵਾਪਸ FAIL;
    cursor_ਉੱਪਰ_inner(curwin, n);

    // try to advance to the column we want to be at
    coladvance(curwin->w_curswant);

    ਜੇ (upd_topline)
	update_topline();	// make sure curwin->w_topline is valid

    ਵਾਪਸ OK;
}

/*
 * Move the cursor down "n" lines in window "wp".
 * Takes care of closed folds.
 */
    ਖਾਲੀ
cursor_ਥੱਲੇ_inner(win_T *wp, ਲੰਮਾ n)
{
    linenr_T	lnum = wp->w_cursor.lnum;
    linenr_T	ਸਤਰ_ਗਿਣਤੀ = wp->w_ਬਫਰ->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ;

    ਜੇ (lnum + n >= ਸਤਰ_ਗਿਣਤੀ)
	lnum = ਸਤਰ_ਗਿਣਤੀ;
    ਨਹੀਂਤਾਂ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
	ਜੇ (hasAnyFolding(wp))
    {
	linenr_T	ਆਖਰੀ;

	// count each sequence of folded lines as one logical line
	ਜਦੋਂ (n--)
	{
	    ਜੇ (hasFoldingWin(wp, lnum, ਨਿਰਾ, &ਆਖਰੀ, TRUE, ਨਿਰਾ))
		lnum = ਆਖਰੀ + 1;
	    ਨਹੀਂਤਾਂ
		++lnum;
	    ਜੇ (lnum >= ਸਤਰ_ਗਿਣਤੀ)
		ਰੋਕੋ;
	}
	ਜੇ (lnum > ਸਤਰ_ਗਿਣਤੀ)
	    lnum = ਸਤਰ_ਗਿਣਤੀ;
    }
    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	lnum += n;

    wp->w_cursor.lnum = lnum;
}

/*
 * Cursor down a number of logical lines.
 */
    ਪੂਰਨ
cursor_ਥੱਲੇ(
    ਲੰਮਾ	n,
    ਪੂਰਨ		upd_topline)	    // When TRUE: update topline
{
    linenr_T	lnum = curwin->w_cursor.lnum;
    linenr_T	ਸਤਰ_ਗਿਣਤੀ = curwin->w_ਬਫਰ->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ;
    // This fails if the cursor is already in the last (folded) line, or would
    // move beyond the last line and '-' is in 'cpoptions'.
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
    hasFoldingWin(curwin, lnum, ਨਿਰਾ, &lnum, TRUE, ਨਿਰਾ);
#ਜੇ_ਅੰਤ
    ਜੇ (n > 0
	    && (lnum >= ਸਤਰ_ਗਿਣਤੀ
		|| (lnum + n > ਸਤਰ_ਗਿਣਤੀ
				     && vim_strchr(p_cpo, CPO_MINUS) != ਨਿਰਾ)))
	ਵਾਪਸ FAIL;
    cursor_ਥੱਲੇ_inner(curwin, n);

    // try to advance to the column we want to be at
    coladvance(curwin->w_curswant);

    ਜੇ (upd_topline)
	update_topline();	// make sure curwin->w_topline is valid

    ਵਾਪਸ OK;
}

/*
 * Stuff the last inserted text in the read buffer.
 * Last_insert actually is a copy of the redo buffer, so we
 * first have to remove the command.
 */
    ਪੂਰਨ
stuff_inserted(
    ਪੂਰਨ	    c,		// Command character to be inserted
    ਲੰਮਾ    ਗਿਣਤੀ_2,	// Repeat this many times
    ਪੂਰਨ	    no_esc)	// Don't add an ESC at the end
{
    string_T	ਪਾਓ;				// text to be inserted
    char_u	ਆਖਰੀ = ' ';

    ਪਾਓ = get_ਆਖਰੀ_ਪਾਓ();
    ਜੇ (ਪਾਓ.string == ਨਿਰਾ)
    {
	emsg(_(e_no_inserted_ਪਾਠ_yet));
	ਵਾਪਸ FAIL;
    }

    // may want to stuff the command character, to start Insert mode
    ਜੇ (c != NUL)
	stuffcharReadbuff(c);

    ਜੇ (ਪਾਓ.ਲੰਮਾਈ > 0)
    {
	char_u	*p;

	// look for the last ESC in 'insert'
	ਲਈ (p = ਪਾਓ.string + ਪਾਓ.ਲੰਮਾਈ - 1; p >= ਪਾਓ.string; --p)
	{
	    ਜੇ (*p == ESC)
	    {
		ਪਾਓ.ਲੰਮਾਈ = (size_t)(p - ਪਾਓ.string);
		ਰੋਕੋ;
	    }
	}
    }

    ਜੇ (ਪਾਓ.ਲੰਮਾਈ > 0)
    {
	char_u	*p = ਪਾਓ.string + ਪਾਓ.ਲੰਮਾਈ - 1;

	// when the last char is either "0" or "^" it will be quoted if no ESC
	// comes after it OR if it will insert more than once and "ptr"
	// starts with ^D.	-- Acevedo
	ਜੇ ((*p == '0' || *p == '^')
		&& (no_esc || (*ਪਾਓ.string == Ctrl_D && ਗਿਣਤੀ_2 > 1)))
	{
	    ਆਖਰੀ = *p;
	    --ਪਾਓ.ਲੰਮਾਈ;
	}
    }

    ਕਰੋ
    {
	stuffReadbuffLen(ਪਾਓ.string, (ਲੰਮਾ)ਪਾਓ.ਲੰਮਾਈ);
	// a trailing "0" is inserted as "<C-V>048", "^" as "<C-V>^"
	ਚੋਣ (ਆਖਰੀ)
	{
	ਹਾਲਤ '0':
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਪਾਠ_ਮੰਜ਼ਿਲ_ਪਾਓ "\026\060\064\070"
	    stuffReadbuffLen((char_u *)ਪਾਠ_ਮੰਜ਼ਿਲ_ਪਾਓ, STRLEN_LITERAL(ਪਾਠ_ਮੰਜ਼ਿਲ_ਪਾਓ));
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ ਪਾਠ_ਮੰਜ਼ਿਲ_ਪਾਓ
	    ਰੋਕੋ;

	ਹਾਲਤ '^':
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਪਾਠ_ਮੰਜ਼ਿਲ_ਪਾਓ "\026^"
	    stuffReadbuffLen((char_u *)ਪਾਠ_ਮੰਜ਼ਿਲ_ਪਾਓ, STRLEN_LITERAL(ਪਾਠ_ਮੰਜ਼ਿਲ_ਪਾਓ));
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ ਪਾਠ_ਮੰਜ਼ਿਲ_ਪਾਓ
	    ਰੋਕੋ;

	ਮੂਲ:
	    ਰੋਕੋ;
	}
    }
    ਜਦੋਂ (--ਗਿਣਤੀ_2 > 0);

    // may want to stuff a trailing ESC, to get out of Insert mode
    ਜੇ (!no_esc)
	stuffcharReadbuff(ESC);

    ਵਾਪਸ OK;
}

    string_T
get_ਆਖਰੀ_ਪਾਓ(ਖਾਲੀ)
{
    string_T ਪਾਓ = {ਨਿਰਾ, 0};

    ਜੇ (ਆਖਰੀ_ਪਾਓ.string != ਨਿਰਾ)
    {
	ਪਾਓ.string = ਆਖਰੀ_ਪਾਓ.string + ਆਖਰੀ_ਪਾਓ_skip;
	ਪਾਓ.ਲੰਮਾਈ = (size_t)(ਆਖਰੀ_ਪਾਓ.ਲੰਮਾਈ - ਆਖਰੀ_ਪਾਓ_skip);
    }

    ਵਾਪਸ ਪਾਓ;
}

/*
 * Get last inserted string, and remove trailing <Esc>.
 * Returns pointer to allocated memory (must be freed) or NULL.
 */
    char_u *
get_ਆਖਰੀ_ਪਾਓ_save(ਖਾਲੀ)
{
    string_T	ਪਾਓ = get_ਆਖਰੀ_ਪਾਓ();
    char_u	*s;

    ਜੇ (ਪਾਓ.string == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;
    s = vim_strnsave(ਪਾਓ.string, ਪਾਓ.ਲੰਮਾਈ);
    ਜੇ (s == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    ਜੇ (ਪਾਓ.ਲੰਮਾਈ > 0 && s[ਪਾਓ.ਲੰਮਾਈ - 1] == ESC)	// remove trailing ESC
	s[--ਪਾਓ.ਲੰਮਾਈ] = NUL;
    ਵਾਪਸ s;
}

/*
 * Check the word in front of the cursor for an abbreviation.
 * Called when the non-id character "c" has been entered.
 * When an abbreviation is recognized it is removed from the text and
 * the replacement string is inserted in typebuf.tb_buf[], followed by "c".
 */
    ਸਥਿਰ ਪੂਰਨ
echeck_abbr(ਪੂਰਨ c)
{
    // Don't check for abbreviation in paste mode, when disabled and just
    // after moving around with cursor keys.
    ਜੇ (p_paste || no_abbr || arrow_ਵਰਤਿਆ)
	ਵਾਪਸ FALSE;

    ਵਾਪਸ check_abbr(c, ml_get_curline(), curwin->w_cursor.ਖਾਨਾ,
		curwin->w_cursor.lnum == Insstart.lnum ? Insstart.ਖਾਨਾ : 0);
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

ਸਥਿਰ char_u	*replace_ਢੇਰ = ਨਿਰਾ;
ਸਥਿਰ ਲੰਮਾ	replace_ਢੇਰ_nr = 0;	    // next entry in replace stack
ਸਥਿਰ ਲੰਮਾ	replace_ਢੇਰ_len = 0;	    // max. number of entries

    ਖਾਲੀ
replace_ਧੱਕੋ(
    ਪੂਰਨ	    c)	    // character that is replaced (NUL is none)
{
    char_u  *p;

    ਜੇ (replace_ਢੇਰ_nr < replace_ਵਿੱਥ)	// nothing to do
	ਵਾਪਸ;
    ਜੇ (replace_ਢੇਰ_len <= replace_ਢੇਰ_nr)
    {
	replace_ਢੇਰ_len += 50;
	p = ALLOC_MULT(char_u, replace_ਢੇਰ_len);
	ਜੇ (p == ਨਿਰਾ)	    // out of memory
	{
	    replace_ਢੇਰ_len -= 50;
	    ਵਾਪਸ;
	}
	ਜੇ (replace_ਢੇਰ != ਨਿਰਾ)
	{
	    mch_memmove(p, replace_ਢੇਰ,
				 (size_t)(replace_ਢੇਰ_nr * ਆਕਾਰ(char_u)));
	    vim_free(replace_ਢੇਰ);
	}
	replace_ਢੇਰ = p;
    }
    p = replace_ਢੇਰ + replace_ਢੇਰ_nr - replace_ਵਿੱਥ;
    ਜੇ (replace_ਵਿੱਥ)
	mch_memmove(p + 1, p, (size_t)(replace_ਵਿੱਥ * ਆਕਾਰ(char_u)));
    *p = c;
    ++replace_ਢੇਰ_nr;
}

/*
 * Push a character onto the replace stack.  Handles a multi-byte character in
 * reverse byte order, so that the first byte is popped off first.
 * Return the number of bytes done (includes composing characters).
 */
    ਪੂਰਨ
replace_ਧੱਕੋ_mb(char_u *p)
{
    ਪੂਰਨ l = (*mb_ptr2len)(p);
    ਪੂਰਨ j;

    ਲਈ (j = l - 1; j >= 0; --j)
	replace_ਧੱਕੋ(p[j]);
    ਵਾਪਸ l;
}

/*
 * Pop one item from the replace stack.
 * return -1 if stack empty
 * return replaced character or NUL otherwise
 */
    ਸਥਿਰ ਪੂਰਨ
replace_ਕੱਢੋ(ਖਾਲੀ)
{
    ਜੇ (replace_ਢੇਰ_nr == 0)
	ਵਾਪਸ -1;
    ਵਾਪਸ (ਪੂਰਨ)replace_ਢੇਰ[--replace_ਢੇਰ_nr];
}

/*
 * Join the top two items on the replace stack.  This removes to "off"'th NUL
 * encountered.
 */
    ਖਾਲੀ
replace_join(
    ਪੂਰਨ	    off)	// offset for which NUL to remove
{
    ਪੂਰਨ	    i;

    ਲਈ (i = replace_ਢੇਰ_nr; --i >= 0; )
	ਜੇ (replace_ਢੇਰ[i] == NUL && off-- <= 0)
	{
	    --replace_ਢੇਰ_nr;
	    mch_memmove(replace_ਢੇਰ + i, replace_ਢੇਰ + i + 1,
					      (size_t)(replace_ਢੇਰ_nr - i));
	    ਵਾਪਸ;
	}
}

/*
 * Pop bytes from the replace stack until a NUL is found, and insert them
 * before the cursor.  Can only be used in MODE_REPLACE or MODE_VREPLACE state.
 */
    ਸਥਿਰ ਖਾਲੀ
replace_ਕੱਢੋ_ins(ਖਾਲੀ)
{
    ਪੂਰਨ	    cc;
    ਪੂਰਨ	    oldState = State;

    State = ਬਹੁਲਕ_ਸਾਧਾਰਨ;			// don't want MODE_REPLACE here
    ਜਦੋਂ ((cc = replace_ਕੱਢੋ()) > 0)
    {
	mb_replace_ਕੱਢੋ_ins(cc);
	dec_cursor();
    }
    State = oldState;
}

/*
 * Insert bytes popped from the replace stack. "cc" is the first byte.  If it
 * indicates a multi-byte char, pop the other bytes too.
 */
    ਸਥਿਰ ਖਾਲੀ
mb_replace_ਕੱਢੋ_ins(ਪੂਰਨ cc)
{
    ਪੂਰਨ		n;
    char_u	buf[MB_MAXBYTES + 1];
    ਪੂਰਨ		i;
    ਪੂਰਨ		c;

    ਜੇ (has_mbyte && (n = MB_BYTE2LEN(cc)) > 1)
    {
	buf[0] = cc;
	ਲਈ (i = 1; i < n; ++i)
	    buf[i] = replace_ਕੱਢੋ();
	ins_ਬਾਈਟਾਂ_len(buf, n);
    }
    ਨਹੀਂਤਾਂ
	ins_char(cc);

    ਜੇ (enc_utf8)
	// Handle composing chars.
	ਲਈ (;;)
	{
	    c = replace_ਕੱਢੋ();
	    ਜੇ (c == -1)	    // stack empty
		ਰੋਕੋ;
	    ਜੇ ((n = MB_BYTE2LEN(c)) == 1)
	    {
		// Not a multi-byte char, put it back.
		replace_ਧੱਕੋ(c);
		ਰੋਕੋ;
	    }

	    buf[0] = c;
	    ਲਈ (i = 1; i < n; ++i)
		buf[i] = replace_ਕੱਢੋ();
	    ਜੇ (utf_iscomposing(utf_ptr2char(buf)))
		ins_ਬਾਈਟਾਂ_len(buf, n);
	    ਨਹੀਂਤਾਂ
	    {
		// Not a composing char, put it back.
		ਲਈ (i = n - 1; i >= 0; --i)
		    replace_ਧੱਕੋ(buf[i]);
		ਰੋਕੋ;
	    }

	}
}

/*
 * make the replace stack empty
 * (called when exiting replace mode)
 */
    ਸਥਿਰ ਖਾਲੀ
replace_flush(ਖਾਲੀ)
{
    VIM_CLEAR(replace_ਢੇਰ);
    replace_ਢੇਰ_len = 0;
    replace_ਢੇਰ_nr = 0;
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
    ਸਥਿਰ ਖਾਲੀ
replace_do_bs(ਪੂਰਨ ਹੱਦਬੰਦੀ_ਖਾਨਾ)
{
    ਪੂਰਨ		cc;
    ਪੂਰਨ		orig_len = 0;
    ਪੂਰਨ		ins_len;
    ਪੂਰਨ		orig_vcols = 0;
    colnr_T	ਸ਼ੁਰੂ_vcol;
    char_u	*p;
    ਪੂਰਨ		i;
    ਪੂਰਨ		vcol;

    cc = replace_ਕੱਢੋ();
    ਜੇ (cc > 0)
    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
	size_t	len_before = 0;  // init to shut up GCC

	ਜੇ (curbuf->b_has_textprop)
	{
	    // Do not adjust text properties for individual delete and insert
	    // operations, do it afterwards on the resulting text.
	    len_before = ml_get_curline_len();
	    ++ਪਾਠ_prop_frozen;
	}
#ਜੇ_ਅੰਤ
	ਜੇ (State & VREPLACE_FLAG)
	{
	    // Get the number of screen cells used by the character we are
	    // going to delete.
	    getvcol(curwin, &curwin->w_cursor, ਨਿਰਾ, &ਸ਼ੁਰੂ_vcol, ਨਿਰਾ, 0);
	    orig_vcols = chartabsize(ml_get_cursor(), ਸ਼ੁਰੂ_vcol);
	}
	ਜੇ (has_mbyte)
	{
	    (ਖਾਲੀ)del_char_after_ਖਾਨਾ(ਹੱਦਬੰਦੀ_ਖਾਨਾ);
	    ਜੇ (State & VREPLACE_FLAG)
		orig_len = ml_get_cursor_len();
	    replace_ਧੱਕੋ(cc);
	}
	ਨਹੀਂਤਾਂ
	{
	    pchar_cursor(cc);
	    ਜੇ (State & VREPLACE_FLAG)
		orig_len = ml_get_cursor_len() - 1;
	}
	replace_ਕੱਢੋ_ins();

	ਜੇ (State & VREPLACE_FLAG)
	{
	    // Get the number of screen cells used by the inserted characters
	    p = ml_get_cursor();
	    ins_len = ml_get_cursor_len() - orig_len;
	    vcol = ਸ਼ੁਰੂ_vcol;
	    ਲਈ (i = 0; i < ins_len; ++i)
	    {
		vcol += chartabsize(p + i, vcol);
		i += (*mb_ptr2len)(p + i) - 1;
	    }
	    vcol -= ਸ਼ੁਰੂ_vcol;

	    // Delete spaces that were inserted after the cursor to keep the
	    // text aligned.
	    curwin->w_cursor.ਖਾਨਾ += ins_len;
	    ਜਦੋਂ (vcol > orig_vcols && gchar_cursor() == ' ')
	    {
		del_char(FALSE);
		++orig_vcols;
	    }
	    curwin->w_cursor.ਖਾਨਾ -= ins_len;
	}

	// mark the buffer as changed and prepare for displaying
	ਬਦਲਿਆ_ਬਾਈਟਾਂ(curwin->w_cursor.lnum, curwin->w_cursor.ਖਾਨਾ);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
	ਜੇ (curbuf->b_has_textprop)
	{
	    size_t len_now = ml_get_curline_len();

	    --ਪਾਠ_prop_frozen;
	    adjust_prop_columns(curwin->w_cursor.lnum, curwin->w_cursor.ਖਾਨਾ,
					   (ਪੂਰਨ)(len_now - len_before), 0);
	}
#ਜੇ_ਅੰਤ
    }
    ਨਹੀਂਤਾਂ ਜੇ (cc == 0)
	(ਖਾਲੀ)del_char_after_ਖਾਨਾ(ਹੱਦਬੰਦੀ_ਖਾਨਾ);
}

#ਜੇ defined(FEAT_RIGHTLEFT)
/*
 * Map Hebrew keyboard when in hkmap mode.
 */
    ਪੂਰਨ
hkmap(ਪੂਰਨ c)
{
    ਜੇ (p_hkmapp)   // phonetic mapping, by Ilya Dogolazky
    {
	ਗਿਣਤੀ {hALEF=0, BET, GIMEL, DALET, HEI, VAV, ZAIN, HET, TET, IUD,
	    KAFsofit, hKAF, LAMED, MEMsofit, MEM, NUNsofit, NUN, SAMEH, AIN,
	    PEIsofit, PEI, ZADIsofit, ZADI, KOF, RESH, hSHIN, TAV};
	ਸਥਿਰ char_u map[26] =
	    {(char_u)hALEF/*a*/, (char_u)BET  /*b*/, (char_u)hKAF    /*c*/,
	     (char_u)DALET/*d*/, (char_u)-1   /*e*/, (char_u)PEIsofit/*f*/,
	     (char_u)GIMEL/*g*/, (char_u)HEI  /*h*/, (char_u)IUD     /*i*/,
	     (char_u)HET  /*j*/, (char_u)KOF  /*k*/, (char_u)LAMED   /*l*/,
	     (char_u)MEM  /*m*/, (char_u)NUN  /*n*/, (char_u)SAMEH   /*o*/,
	     (char_u)PEI  /*p*/, (char_u)-1   /*q*/, (char_u)RESH    /*r*/,
	     (char_u)ZAIN /*s*/, (char_u)TAV  /*t*/, (char_u)TET     /*u*/,
	     (char_u)VAV  /*v*/, (char_u)hSHIN/*w*/, (char_u)-1      /*x*/,
	     (char_u)AIN  /*y*/, (char_u)ZADI /*z*/};

	ਜੇ (c == 'N' || c == 'M' || c == 'P' || c == 'C' || c == 'Z')
	    ਵਾਪਸ (ਪੂਰਨ)(map[CharOrd(c)] - 1 + p_aleph);
							    // '-1'='sofit'
	ਨਹੀਂਤਾਂ ਜੇ (c == 'x')
	    ਵਾਪਸ 'X';
	ਨਹੀਂਤਾਂ ਜੇ (c == 'q')
	    ਵਾਪਸ '\''; // {geresh}={'}
	ਨਹੀਂਤਾਂ ਜੇ (c == 246)
	    ਵਾਪਸ ' ';  // \"o --> ' ' for a german keyboard
	ਨਹੀਂਤਾਂ ਜੇ (c == 228)
	    ਵਾਪਸ ' ';  // \"a --> ' '      -- / --
	ਨਹੀਂਤਾਂ ਜੇ (c == 252)
	    ਵਾਪਸ ' ';  // \"u --> ' '      -- / --
	// NOTE: islower() does not do the right thing for us on Linux so we
	// do this the same was as 5.7 and previous, so it works correctly on
	// all systems.  Specifically, the e.g. Delete and Arrow keys are
	// munged and won't work if e.g. searching for Hebrew text.
	ਨਹੀਂਤਾਂ ਜੇ (c >= 'a' && c <= 'z')
	    ਵਾਪਸ (ਪੂਰਨ)(map[Char_Ord_ਨੀਵਾਂ(c)] + p_aleph);
	ਨਹੀਂਤਾਂ
	    ਵਾਪਸ c;
    }
    ਨਹੀਂਤਾਂ
    {
	ਚੋਣ (c)
	{
	    ਹਾਲਤ '`':	ਵਾਪਸ ';';
	    ਹਾਲਤ '/':	ਵਾਪਸ '.';
	    ਹਾਲਤ '\'':	ਵਾਪਸ ',';
	    ਹਾਲਤ 'q':	ਵਾਪਸ '/';
	    ਹਾਲਤ 'w':	ਵਾਪਸ '\'';

			// Hebrew letters - set offset from 'a'
	    ਹਾਲਤ ',':	c = '{'; ਰੋਕੋ;
	    ਹਾਲਤ '.':	c = 'v'; ਰੋਕੋ;
	    ਹਾਲਤ ';':	c = 't'; ਰੋਕੋ;
	    ਮੂਲ: {
			 ਸਥਿਰ ਅੱਖਰ str[] = "zqbcxlsjphmkwonu ydafe rig";

			 ਜੇ (c < 'a' || c > 'z')
			     ਵਾਪਸ c;
			 c = str[Char_Ord_ਨੀਵਾਂ(c)];
			 ਰੋਕੋ;
		     }
	}

	ਵਾਪਸ (ਪੂਰਨ)(Char_Ord_ਨੀਵਾਂ(c) + p_aleph);
    }
}
#ਜੇ_ਅੰਤ

    ਸਥਿਰ ਖਾਲੀ
ins_reg(ਖਾਲੀ)
{
    ਪੂਰਨ		need_redraw = FALSE;
    ਪੂਰਨ		regname;
    ਪੂਰਨ		literally = 0;
    ਪੂਰਨ		vis_ਸਰਗਰਮ = VIsual_ਸਰਗਰਮ;

    /*
     * If we are going to wait for a character, show a '"'.
     */
    pc_ਸਥਿਤੀ = PC_ਸਥਿਤੀ_UNSET;
    ਜੇ (redrawing() && !char_avail())
    {
	// may need to redraw when no more chars available now
	ins_redraw(FALSE);

	edit_putchar('"', TRUE);
	ਜੋੜੋ_ਮੰਜ਼ਿਲ_showcmd_c(Ctrl_R);
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_ON_FLY_SCROLL
    dont_scroll = TRUE;		// disallow scrolling here
#ਜੇ_ਅੰਤ

    /*
     * Don't map the register name. This also prevents the mode message to be
     * deleted when ESC is hit.
     */
    ++no_mapping;
    ++allow_ਕੁੰਜੀਆਂ;
    regname = plain_vgetc();
    LANGMAP_ADJUST(regname, TRUE);
    ਜੇ (regname == Ctrl_R || regname == Ctrl_O || regname == Ctrl_P)
    {
	// Get a third key for literal register insertion
	literally = regname;
	ਜੋੜੋ_ਮੰਜ਼ਿਲ_showcmd_c(literally);
	regname = plain_vgetc();
	LANGMAP_ADJUST(regname, TRUE);
    }
    --no_mapping;
    --allow_ਕੁੰਜੀਆਂ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    // Don't call u_sync() while typing the expression or giving an error
    // message for it. Only call it explicitly.
    ++no_u_sync;
    ਜੇ (regname == '=')
    {
	pos_T	curpos = curwin->w_cursor;
# ifdef HAVE_ਨਿਵੇਸ਼_ਵਿਧੀ
	ਪੂਰਨ	im_on = im_get_ਸਥਿਤੀ();
# endif
	// Sync undo when evaluating the expression calls setline() or
	// append(), so that it can be undone separately.
	u_sync_once = 2;

	regname = get_ਵਿਅੰਜਕ_ਰਜਿਸਟਰ();

	// Cursor may be moved back a column.
	curwin->w_cursor = curpos;
	check_cursor();
# ifdef HAVE_ਨਿਵੇਸ਼_ਵਿਧੀ
	// Restore the Input Method.
	ਜੇ (im_on)
	    im_ਸੈੱਟ_ਸਰਗਰਮ(TRUE);
# endif
    }
    ਜੇ (regname == NUL || !valid_yank_reg(regname, FALSE))
    {
	vim_beep(BO_REG);
	need_redraw = TRUE;	// remove the '"'
    }
    ਨਹੀਂਤਾਂ
    {
#ਜੇ_ਅੰਤ
	ਜੇ (literally == Ctrl_O || literally == Ctrl_P)
	{
	    // Append the command to the redo buffer.
	    Append_Char_ਮੰਜ਼ਿਲ_Redobuff(Ctrl_R);
	    Append_Char_ਮੰਜ਼ਿਲ_Redobuff(literally);
	    Append_Char_ਮੰਜ਼ਿਲ_Redobuff(regname);

	    do_put(regname, ਨਿਰਾ, BACKWARD, 1L,
		 (literally == Ctrl_P ? PUT_FIXINDENT : 0) | PUT_CURSEND);
	}
	ਨਹੀਂਤਾਂ ਜੇ (ਪਾਓ_reg(regname, literally) == FAIL)
	{
	    vim_beep(BO_REG);
	    need_redraw = TRUE;	// remove the '"'
	}
	ਨਹੀਂਤਾਂ ਜੇ (stop_ਪਾਓ_ਬਹੁਲਕ)
	    // When the '=' register was used and a function was invoked that
	    // did ":stopinsert" then stuff_empty() returns FALSE but we won't
	    // insert anything, need to remove the '"'
	    need_redraw = TRUE;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    }
    --no_u_sync;
    ਜੇ (u_sync_once == 1)
	ins_need_undo = TRUE;
    u_sync_once = 0;
#ਜੇ_ਅੰਤ
    clear_showcmd();

    // If the inserted register is empty, we need to remove the '"'
    ਜੇ (need_redraw || stuff_ਖਾਲੀ_ਥਾਵਾਂ())
	edit_unputchar();

    // Disallow starting Visual mode here, would get a weird mode.
    ਜੇ (!vis_ਸਰਗਰਮ && VIsual_ਸਰਗਰਮ)
	ਸਿਰਾ_visual_ਬਹੁਲਕ();
}

/*
 * CTRL-G commands in Insert mode.
 */
    ਸਥਿਰ ਖਾਲੀ
ins_ctrl_g(ਖਾਲੀ)
{
    ਪੂਰਨ		c;

    // Right after CTRL-X the cursor will be after the ruler.
    setcursor();

    /*
     * Don't map the second key. This also prevents the mode message to be
     * deleted when ESC is hit.
     */
    ++no_mapping;
    ++allow_ਕੁੰਜੀਆਂ;
    c = plain_vgetc();
    --no_mapping;
    --allow_ਕੁੰਜੀਆਂ;
    ਚੋਣ (c)
    {
	// CTRL-G k and CTRL-G <Up>: cursor up to Insstart.col
	ਹਾਲਤ K_ਉੱਪਰ:
	ਹਾਲਤ Ctrl_K:
	ਹਾਲਤ 'k': ins_ਉੱਪਰ(TRUE);
		  ਰੋਕੋ;

	// CTRL-G j and CTRL-G <Down>: cursor down to Insstart.col
	ਹਾਲਤ K_ਥੱਲੇ:
	ਹਾਲਤ Ctrl_J:
	ਹਾਲਤ 'j': ins_ਥੱਲੇ(TRUE);
		  ਰੋਕੋ;

	// CTRL-G u: start new undoable edit
	ਹਾਲਤ 'u': u_sync(TRUE);
		  ins_need_undo = TRUE;

		  // Need to reset Insstart, esp. because a BS that joins
		  // a line to the previous one must save for undo.
		  update_Insstart_orig = FALSE;
		  Insstart = curwin->w_cursor;
		  ਰੋਕੋ;

	// CTRL-G U: do not break undo with the next char
	ਹਾਲਤ 'U':
		  // Allow one left/right cursor movement with the next char,
		  // without breaking undo.
		  dont_sync_undo = MAYBE;
		  ਰੋਕੋ;

	ਹਾਲਤ ESC:
		  // Esc after CTRL-G cancels it.
		  ਰੋਕੋ;

	// Unknown CTRL-G command, reserved for future expansion.
	ਮੂਲ:  vim_beep(BO_CTRLG);
    }
}

/*
 * CTRL-^ in Insert mode.
 */
    ਸਥਿਰ ਖਾਲੀ
ins_ctrl_hat(ਖਾਲੀ)
{
    ਜੇ (map_ਮੰਜ਼ਿਲ_exists_ਬਹੁਲਕ((char_u *)"", ਬਹੁਲਕ_LANGMAP, FALSE))
    {
	// ":lmap" mappings exists, Toggle use of ":lmap" mappings.
	ਜੇ (State & ਬਹੁਲਕ_LANGMAP)
	{
	    curbuf->b_p_iminsert = B_IMODE_NONE;
	    State &= ~ਬਹੁਲਕ_LANGMAP;
	}
	ਨਹੀਂਤਾਂ
	{
	    curbuf->b_p_iminsert = B_IMODE_LMAP;
	    State |= ਬਹੁਲਕ_LANGMAP;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_ਨਿਵੇਸ਼_ਵਿਧੀ
	    im_ਸੈੱਟ_ਸਰਗਰਮ(FALSE);
#ਜੇ_ਅੰਤ
	}
    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_ਨਿਵੇਸ਼_ਵਿਧੀ
    ਨਹੀਂਤਾਂ
    {
	// There are no ":lmap" mappings, toggle IM
	ਜੇ (im_get_ਸਥਿਤੀ())
	{
	    curbuf->b_p_iminsert = B_IMODE_NONE;
	    im_ਸੈੱਟ_ਸਰਗਰਮ(FALSE);
	}
	ਨਹੀਂਤਾਂ
	{
	    curbuf->b_p_iminsert = B_IMODE_IM;
	    State &= ~ਬਹੁਲਕ_LANGMAP;
	    im_ਸੈੱਟ_ਸਰਗਰਮ(TRUE);
	}
    }
#ਜੇ_ਅੰਤ
    ਸੈੱਟ_iminsert_global();
    showmode();
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
    // may show different cursor shape or color
    ਜੇ (gui.ਵਿੱਚ_use)
	gui_update_cursor(TRUE, FALSE);
#ਜੇ_ਅੰਤ
#ਜੇ defined(FEAT_KEYMAP)
    // Show/unshow value of 'keymap' in status lines.
    ਸਥਿਤੀ_redraw_curbuf();
#ਜੇ_ਅੰਤ
}

/*
 * Handle ESC in insert mode.
 * Returns TRUE when leaving insert mode, FALSE when going to repeat the
 * insert.
 */
    ਸਥਿਰ ਪੂਰਨ
ins_esc(
    ਲੰਮਾ	*ਗਿਣਤੀ_2,
    ਪੂਰਨ		cmdchar,
    ਪੂਰਨ		nomove)	    // don't move cursor
{
    ਪੂਰਨ		ਆਰਜ਼ੀ;
    ਸਥਿਰ ਪੂਰਨ	disabled_redraw = FALSE;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CONCEAL
    // Remember if the cursor line was concealed before changing State.
    ਪੂਰਨ		cursor_ਸਤਰ_was_concealed = curwin->w_p_cole > 0
						&& conceal_cursor_ਸਤਰ(curwin);
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SPELL
    check_spell_redraw();
#ਜੇ_ਅੰਤ

    ਆਰਜ਼ੀ = curwin->w_cursor.ਖਾਨਾ;
    ਜੇ (disabled_redraw)
    {
	ਜੇ (RedrawingDisabled > 0)
	    --RedrawingDisabled;
	disabled_redraw = FALSE;
    }
    ਜੇ (!arrow_ਵਰਤਿਆ)
    {
	/*
	 * Don't append the ESC for "r<CR>" and "grx".
	 * When 'insertmode' is set only CTRL-L stops Insert mode.  Needed for
	 * when "count" is non-zero.
	 */
	ਜੇ (cmdchar != 'r' && cmdchar != 'v')
	    Append_ਮੰਜ਼ਿਲ_Redobuff(p_im ? (char_u *)"\014" : ESC_STR);

	/*
	 * Repeating insert may take a long time.  Check for
	 * interrupt now and then.
	 */
	ਜੇ (*ਗਿਣਤੀ_2 > 0)
	{
	    ਸਤਰ_breakcheck();
	    ਜੇ (got_int)
		*ਗਿਣਤੀ_2 = 0;
	}

	ਜੇ (--*ਗਿਣਤੀ_2 > 0)	// repeat what was typed
	{
	    // Vi repeats the insert without replacing characters.
	    ਜੇ (vim_strchr(p_cpo, CPO_REPLCNT) != ਨਿਰਾ)
		State &= ~REPLACE_FLAG;

	    (ਖਾਲੀ)ਸ਼ੁਰੂ_redo_ins();
	    ਜੇ (cmdchar == 'r' || cmdchar == 'v')
		stuffRedoReadbuff(ESC_STR);	// no ESC in redo buffer
	    ++RedrawingDisabled;
	    disabled_redraw = TRUE;
	    ਵਾਪਸ FALSE;	// repeat the insert
	}
	stop_ਪਾਓ(&curwin->w_cursor, TRUE, nomove);
	undisplay_dollar();
    }

    ਜੇ (cmdchar != 'r' && cmdchar != 'v')
	ins_apply_autocmds(EVENT_INSERTLEAVEPRE);

    // When an autoindent was removed, curswant stays after the
    // indent
    ਜੇ (restart_edit == NUL && (colnr_T)ਆਰਜ਼ੀ == curwin->w_cursor.ਖਾਨਾ)
	curwin->w_ਸੈੱਟ_curswant = true;

    // Remember the last Insert position in the '^ mark.
    ਜੇ ((cmdmod.cmod_flags & CMOD_KEEPJUMPS) == 0)
	curbuf->b_ਆਖਰੀ_ਪਾਓ = curwin->w_cursor;

    /*
     * The cursor should end up on the last inserted character.
     * Don't do it for CTRL-O, unless past the end of the line.
     */
    ਜੇ (!nomove
	    && (curwin->w_cursor.ਖਾਨਾ != 0
		|| curwin->w_cursor.coladd > 0)
	    && (restart_edit == NUL
		   || (gchar_cursor() == NUL && !VIsual_ਸਰਗਰਮ))
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RIGHTLEFT
	    && !revins_on
#ਜੇ_ਅੰਤ
				      )
    {
	ਜੇ (curwin->w_cursor.coladd > 0 || get_ve_flags() == VE_ALL)
	{
	    oneleft();
	    ਜੇ (restart_edit != NUL)
		++curwin->w_cursor.coladd;
	}
	ਨਹੀਂਤਾਂ
	{
	    --curwin->w_cursor.ਖਾਨਾ;
	    curwin->w_valid &= ~(VALID_WCOL|VALID_VIRTCOL);
	    // Correct cursor for multi-byte character.
	    ਜੇ (has_mbyte)
		mb_adjust_cursor();
	}
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_ਨਿਵੇਸ਼_ਵਿਧੀ
    // Disable IM to allow typing English directly for Normal mode commands.
    // When ":lmap" is enabled don't change 'iminsert' (IM can be enabled as
    // well).
    ਜੇ (!(State & ਬਹੁਲਕ_LANGMAP))
	im_save_ਸਥਿਤੀ(&curbuf->b_p_iminsert);
    im_ਸੈੱਟ_ਸਰਗਰਮ(FALSE);
#ਜੇ_ਅੰਤ

    State = ਬਹੁਲਕ_ਸਾਧਾਰਨ;
    may_trigger_modechanged();
    // need to position cursor again when on a TAB and when on a char with
    // virtual text.
    ਜੇ (gchar_cursor() == TAB
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
	    || curbuf->b_has_textprop
#ਜੇ_ਅੰਤ
       )
	curwin->w_valid &= ~(VALID_WROW|VALID_WCOL|VALID_VIRTCOL);

    setmouse();
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURSOR_SHAPE
    ui_cursor_shape();		// may show different cursor shape
#ਜੇ_ਅੰਤ
    ਜੇ (!p_ek)
    {
	MAY_WANT_ਮੰਜ਼ਿਲ_LOG_THIS;

	// Re-enable bracketed paste mode.
	ਬਾਹਰਲਾ_str_t_BE();

	// Re-enable modifyOtherKeys.
	ਬਾਹਰਲਾ_str_t_TI();
    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CONCEAL
    // Check if the cursor line needs redrawing after changing State.  If
    // 'concealcursor' is "i" it needs to be redrawn without concealing.
    conceal_check_cursor_ਸਤਰ(cursor_ਸਤਰ_was_concealed);
#ਜੇ_ਅੰਤ

    // When recording or for CTRL-O, need to display the new mode.
    // Otherwise remove the mode message.
    ਜੇ (reg_recording != 0 || restart_edit != NUL)
	showmode();
    ਨਹੀਂਤਾਂ ਜੇ (p_smd && (got_int || !skip_showmode()))
	ਸੁਨੇਹਾ("");

    ਵਾਪਸ TRUE;	    // exit Insert mode
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RIGHTLEFT
/*
 * Toggle language: hkmap and revins_on.
 * Move to end of reverse inserted text.
 */
    ਸਥਿਰ ਖਾਲੀ
ins_ctrl_(ਖਾਲੀ)
{
    ਜੇ (revins_on && revins_chars && revins_scol >= 0)
    {
	ਜਦੋਂ (gchar_cursor() != NUL && revins_chars--)
	    ++curwin->w_cursor.ਖਾਨਾ;
    }
    p_ri = !p_ri;
    revins_on = (State == ਬਹੁਲਕ_ਪਾਓ && p_ri);
    ਜੇ (revins_on)
    {
	revins_scol = curwin->w_cursor.ਖਾਨਾ;
	revins_legal++;
	revins_chars = 0;
	undisplay_dollar();
    }
    ਨਹੀਂਤਾਂ
	revins_scol = -1;
    p_hkmap = curwin->w_p_rl ^ p_ri;    // be consistent!
    showmode();
}
#ਜੇ_ਅੰਤ

/*
 * If 'keymodel' contains "startsel", may start selection.
 * Returns TRUE when a CTRL-O and other keys stuffed.
 */
    ਸਥਿਰ ਪੂਰਨ
ins_ਸ਼ੁਰੂ_select(ਪੂਰਨ c)
{
    ਜੇ (!km_startsel)
	ਵਾਪਸ FALSE;
    ਚੋਣ (c)
    {
	ਹਾਲਤ K_KHOME:
	ਹਾਲਤ K_KEND:
	ਹਾਲਤ K_PAGEUP:
	ਹਾਲਤ K_KPAGEUP:
	ਹਾਲਤ K_PAGEDOWN:
	ਹਾਲਤ K_KPAGEDOWN:
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MACOS_X
	ਹਾਲਤ K_ਖੱਬਾ:
	ਹਾਲਤ K_ਸੱਜਾ:
	ਹਾਲਤ K_ਉੱਪਰ:
	ਹਾਲਤ K_ਥੱਲੇ:
	ਹਾਲਤ K_ਸਿਰਾ:
	ਹਾਲਤ K_HOME:
#ਜੇ_ਅੰਤ
	    ਜੇ (!(mod_ਨਕਾਬ & MOD_ਨਕਾਬ_ਖਿਸਕਾਓ))
		ਰੋਕੋ;
	    // FALLTHROUGH
	ਹਾਲਤ K_S_ਖੱਬਾ:
	ਹਾਲਤ K_S_ਸੱਜਾ:
	ਹਾਲਤ K_S_ਉੱਪਰ:
	ਹਾਲਤ K_S_ਥੱਲੇ:
	ਹਾਲਤ K_S_ਸਿਰਾ:
	ਹਾਲਤ K_S_HOME:
	    // Start selection right away, the cursor can move with CTRL-O when
	    // beyond the end of the line.
	    ਸ਼ੁਰੂ_selection();

	    // Execute the key in (insert) Select mode.
	    stuffcharReadbuff(Ctrl_O);
	    ਜੇ (mod_ਨਕਾਬ)
	    {
		char_u	    buf[4];

		buf[0] = K_SPECIAL;
		buf[1] = KS_MODIFIER;
		buf[2] = mod_ਨਕਾਬ;
		buf[3] = NUL;
		stuffReadbuffLen(buf, 3L);
	    }
	    stuffcharReadbuff(c);
	    ਵਾਪਸ TRUE;
    }
    ਵਾਪਸ FALSE;
}

/*
 * <Insert> key in Insert mode: toggle insert/replace mode.
 */
    ਸਥਿਰ ਖਾਲੀ
ins_ਪਾਓ(ਪੂਰਨ replaceState)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    ਸੈੱਟ_vim_var_string(VV_INSERTMODE,
		   (char_u *)((State & REPLACE_FLAG) ? "i"
			    : replaceState == ਬਹੁਲਕ_VREPLACE ? "v" : "r"), 1);
#ਜੇ_ਅੰਤ
    ins_apply_autocmds(EVENT_INSERTCHANGE);
    ਜੇ (State & REPLACE_FLAG)
	State = ਬਹੁਲਕ_ਪਾਓ | (State & ਬਹੁਲਕ_LANGMAP);
    ਨਹੀਂਤਾਂ
	State = replaceState | (State & ਬਹੁਲਕ_LANGMAP);
    may_trigger_modechanged();
    Append_Char_ਮੰਜ਼ਿਲ_Redobuff(K_INS);
    showmode();
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURSOR_SHAPE
    ui_cursor_shape();		// may show different cursor shape
#ਜੇ_ਅੰਤ
}

/*
 * Pressed CTRL-O in Insert mode.
 */
    ਸਥਿਰ ਖਾਲੀ
ins_ctrl_o(ਖਾਲੀ)
{
    restart_VIsual_select = 0;
    ਜੇ (State & VREPLACE_FLAG)
	restart_edit = 'V';
    ਨਹੀਂਤਾਂ ਜੇ (State & REPLACE_FLAG)
	restart_edit = 'R';
    ਨਹੀਂਤਾਂ
	restart_edit = 'I';
    ਜੇ (virtual_ਸਰਗਰਮ())
	ins_at_eol = FALSE;	// cursor always keeps its column
    ਨਹੀਂਤਾਂ
	ins_at_eol = (gchar_cursor() == NUL);
}

/*
 * If the cursor is on an indent, ^T/^D insert/delete one
 * shiftwidth.	Otherwise ^T/^D behave like a "<<" or ">>".
 * Always round the indent to 'shiftwidth', this is compatible
 * with vi.  But vi only supports ^T and ^D after an
 * autoindent, we support it everywhere.
 */
    ਸਥਿਰ ਖਾਲੀ
ins_ਖਿਸਕਾਓ(ਪੂਰਨ c, ਪੂਰਨ lastc)
{
    ਜੇ (stop_arrow() == FAIL)
	ਵਾਪਸ;
    Append_Char_ਮੰਜ਼ਿਲ_Redobuff(c);

    /*
     * 0^D and ^^D: remove all indent.
     */
    ਜੇ (c == Ctrl_D && (lastc == '0' || lastc == '^')
						  && curwin->w_cursor.ਖਾਨਾ > 0)
    {
	--curwin->w_cursor.ਖਾਨਾ;
	(ਖਾਲੀ)del_char(FALSE);		// delete the '^' or '0'
	// In Replace mode, restore the characters that '^' or '0' replaced.
	ਜੇ (State & REPLACE_FLAG)
	    replace_ਕੱਢੋ_ins();
	ਜੇ (lastc == '^')
	    old_indent = get_indent();	// remember curr. indent
	change_indent(INDENT_ਸੈੱਟ, 0, TRUE, 0, TRUE);
    }
    ਨਹੀਂਤਾਂ
	change_indent(c == Ctrl_D ? INDENT_DEC : INDENT_INC, 0, TRUE, 0, TRUE);

    ਜੇ (did_ai && *skipwhite(ml_get_curline()) != NUL)
	did_ai = FALSE;
    did_si = FALSE;
    can_si = FALSE;
    can_si_ਪਿੱਠ = FALSE;
    can_cindent = FALSE;	// no cindenting after ^D or ^T
}

    ਸਥਿਰ ਖਾਲੀ
ins_del(ਖਾਲੀ)
{
    ਪੂਰਨ	    ਆਰਜ਼ੀ;

    ਜੇ (stop_arrow() == FAIL)
	ਵਾਪਸ;
    ਜੇ (gchar_cursor() == NUL)		// delete newline
    {
	ਆਰਜ਼ੀ = curwin->w_cursor.ਖਾਨਾ;
	ਜੇ (!can_bs(BS_EOL)		// only if "eol" included
		|| do_join(2, FALSE, TRUE, FALSE, FALSE) == FAIL)
	    vim_beep(BO_BS);
	ਨਹੀਂਤਾਂ
	{
	    curwin->w_cursor.ਖਾਨਾ = ਆਰਜ਼ੀ;
	    // Adjust orig_line_count in case more lines have been deleted than
	    // have been added. That makes sure, that open_line() later
	    // can access all buffer lines correctly
	    ਜੇ (State & VREPLACE_FLAG &&
		    orig_ਸਤਰ_ਗਿਣਤੀ > curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
		orig_ਸਤਰ_ਗਿਣਤੀ = curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ;
	}
    }
    ਨਹੀਂਤਾਂ ਜੇ (del_char(FALSE) == FAIL)  // delete char under cursor
	vim_beep(BO_BS);
    did_ai = FALSE;
    did_si = FALSE;
    can_si = FALSE;
    can_si_ਪਿੱਠ = FALSE;
    Append_Char_ਮੰਜ਼ਿਲ_Redobuff(K_DEL);
}

/*
 * Delete one character for ins_bs().
 */
    ਸਥਿਰ ਖਾਲੀ
ins_bs_one(ਖਾਲੀ)
{
    dec_cursor();
    ਜੇ (State & REPLACE_FLAG)
    {
	// Don't delete characters before the insert point when in
	// Replace mode
	ਜੇ (curwin->w_cursor.lnum != Insstart.lnum
		|| curwin->w_cursor.ਖਾਨਾ >= Insstart.ਖਾਨਾ)
	    replace_do_bs(-1);
    }
    ਨਹੀਂਤਾਂ
	(ਖਾਲੀ)del_char(FALSE);
}

/*
 * Handle Backspace, delete-word and delete-line in Insert mode.
 * Return TRUE when backspace was actually used.
 */
    ਸਥਿਰ ਪੂਰਨ
ins_bs(
    ਪੂਰਨ		c,
    ਪੂਰਨ		ਬਹੁਲਕ,
    ਪੂਰਨ		*inserted_space_p)
{
    linenr_T	lnum;
    ਪੂਰਨ		cc;
    ਪੂਰਨ		ਆਰਜ਼ੀ = 0;	    // init for GCC
    colnr_T	save_ਖਾਨਾ;
    colnr_T	mincol;
    ਪੂਰਨ		did_backspace = FALSE;
    ਪੂਰਨ		ਵਿੱਚ_indent;
    ਪੂਰਨ		oldState;
    ਪੂਰਨ		cpc[MAX_MCO];	    // composing characters
    ਪੂਰਨ		call_fix_indent = FALSE;

    /*
     * can't delete anything in an empty file
     * can't backup past first character in buffer
     * can't backup past starting point unless 'backspace' > 1
     * can backup to a previous line if 'backspace' == 0
     */
    ਜੇ (       BUFEMPTY()
	    || (
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RIGHTLEFT
		!revins_on &&
#ਜੇ_ਅੰਤ
		((curwin->w_cursor.lnum == 1 && curwin->w_cursor.ਖਾਨਾ == 0)
		    || (!can_bs(BS_ਸ਼ੁਰੂ)
			&& ((arrow_ਵਰਤਿਆ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਕੰਮ_CHANNEL
				&& !bt_prompt(curbuf)
#ਜੇ_ਅੰਤ
			) || (curwin->w_cursor.lnum == Insstart_orig.lnum
				&& curwin->w_cursor.ਖਾਨਾ <= Insstart_orig.ਖਾਨਾ)))
		    || (!can_bs(BS_INDENT) && !arrow_ਵਰਤਿਆ && ai_ਖਾਨਾ > 0
					 && curwin->w_cursor.ਖਾਨਾ <= ai_ਖਾਨਾ)
		    || (!can_bs(BS_EOL) && curwin->w_cursor.ਖਾਨਾ == 0))))
    {
	vim_beep(BO_BS);
	ਵਾਪਸ FALSE;
    }

    ਜੇ (stop_arrow() == FAIL)
	ਵਾਪਸ FALSE;
    ਵਿੱਚ_indent = inindent(0);
    ਜੇ (ਵਿੱਚ_indent)
	can_cindent = FALSE;
    ਸਿਰਾ_comment_pending = NUL;	// After BS, don't auto-end comment
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RIGHTLEFT
    ਜੇ (revins_on)	    // put cursor after last inserted char
	inc_cursor();
#ਜੇ_ਅੰਤ

    // Virtualedit:
    //	BACKSPACE_CHAR eats a virtual space
    //	BACKSPACE_WORD eats all coladd
    //	BACKSPACE_LINE eats all coladd and keeps going
    ਜੇ (curwin->w_cursor.coladd > 0)
    {
	ਜੇ (ਬਹੁਲਕ == BACKSPACE_CHAR)
	{
	    --curwin->w_cursor.coladd;
	    ਵਾਪਸ TRUE;
	}
	ਜੇ (ਬਹੁਲਕ == BACKSPACE_ਸ਼ਬਦ)
	{
	    curwin->w_cursor.coladd = 0;
	    ਵਾਪਸ TRUE;
	}
	curwin->w_cursor.coladd = 0;
    }

    /*
     * Delete newline!
     */
    ਜੇ (curwin->w_cursor.ਖਾਨਾ == 0)
    {
	lnum = Insstart.lnum;
	ਜੇ (curwin->w_cursor.lnum == lnum
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RIGHTLEFT
			|| revins_on
#ਜੇ_ਅੰਤ
				    )
	{
	    ਜੇ (u_save((linenr_T)(curwin->w_cursor.lnum - 2),
			       (linenr_T)(curwin->w_cursor.lnum + 1)) == FAIL)
		ਵਾਪਸ FALSE;
	    --Insstart.lnum;
	    Insstart.ਖਾਨਾ = ml_get_len(Insstart.lnum);
	}
	/*
	 * In replace mode:
	 * cc < 0: NL was inserted, delete it
	 * cc >= 0: NL was replaced, put original characters back
	 */
	cc = -1;
	ਜੇ (State & REPLACE_FLAG)
	    cc = replace_ਕੱਢੋ();	    // returns -1 if NL was inserted
	/*
	 * In replace mode, in the line we started replacing, we only move the
	 * cursor.
	 */
	ਜੇ ((State & REPLACE_FLAG) && curwin->w_cursor.lnum <= lnum)
	{
	    dec_cursor();
	}
	ਨਹੀਂਤਾਂ
	{
	    ਜੇ (!(State & VREPLACE_FLAG)
				   || curwin->w_cursor.lnum > orig_ਸਤਰ_ਗਿਣਤੀ)
	    {
		ਆਰਜ਼ੀ = gchar_cursor();	// remember current char
		--curwin->w_cursor.lnum;

		// When "aw" is in 'formatoptions' we must delete the space at
		// the end of the line, otherwise the line will be broken
		// again when auto-formatting.
		ਜੇ (has_format_option(FO_AUTO)
					   && has_format_option(FO_WHITE_PAR))
		{
		    char_u  *ptr = ml_get_buf(curbuf, curwin->w_cursor.lnum,
									FALSE);
		    ਪੂਰਨ	    len = ml_get_curline_len();

		    ਜੇ (len > 0 && ptr[len - 1] == ' ')
		    {
			char_u *newp = alloc(curbuf->b_ml.ml_ਸਤਰ_len - 1);

			ਜੇ (newp != ਨਿਰਾ)
			{
			    mch_memmove(newp, ptr, len - 1);
			    newp[len - 1] = NUL;
			    ਜੇ (curbuf->b_ml.ml_ਸਤਰ_len > len + 1)
				mch_memmove(newp + len, ptr + len + 1,
					   curbuf->b_ml.ml_ਸਤਰ_len - len - 1);

			    ਜੇ (curbuf->b_ml.ml_flags
					      & (ML_ਸਤਰ_DIRTY | ML_ALLOCATED))
				vim_free(curbuf->b_ml.ml_ਸਤਰ_ptr);
			    curbuf->b_ml.ml_ਸਤਰ_ptr = newp;
			    curbuf->b_ml.ml_ਸਤਰ_len--;
			    curbuf->b_ml.ml_ਸਤਰ_textlen--;
			    curbuf->b_ml.ml_flags |= ML_ਸਤਰ_DIRTY;
			}
		    }
		}

		(ਖਾਲੀ)do_join(2, FALSE, FALSE, FALSE, FALSE);
		ਜੇ (ਆਰਜ਼ੀ == NUL && gchar_cursor() != NUL)
		    inc_cursor();
	    }
	    ਨਹੀਂਤਾਂ
		dec_cursor();

	    /*
	     * In MODE_REPLACE mode we have to put back the text that was
	     * replaced by the NL. On the replace stack is first a
	     * NUL-terminated sequence of characters that were deleted and then
	     * the characters that NL replaced.
	     */
	    ਜੇ (State & REPLACE_FLAG)
	    {
		/*
		 * Do the next ins_char() in MODE_NORMAL state, to
		 * prevent ins_char() from replacing characters and
		 * avoiding showmatch().
		 */
		oldState = State;
		State = ਬਹੁਲਕ_ਸਾਧਾਰਨ;
		/*
		 * restore characters (blanks) deleted after cursor
		 */
		ਜਦੋਂ (cc > 0)
		{
		    save_ਖਾਨਾ = curwin->w_cursor.ਖਾਨਾ;
		    mb_replace_ਕੱਢੋ_ins(cc);
		    curwin->w_cursor.ਖਾਨਾ = save_ਖਾਨਾ;
		    cc = replace_ਕੱਢੋ();
		}
		// restore the characters that NL replaced
		replace_ਕੱਢੋ_ins();
		State = oldState;
	    }
	}
	did_ai = FALSE;
    }
    ਨਹੀਂਤਾਂ
    {
	/*
	 * Delete character(s) before the cursor.
	 */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RIGHTLEFT
	ਜੇ (revins_on)		// put cursor on last inserted char
	    dec_cursor();
#ਜੇ_ਅੰਤ
	mincol = 0;
						// keep indent
	ਜੇ (ਬਹੁਲਕ == BACKSPACE_ਸਤਰ
		&& (curbuf->b_p_ai || cindent_on())
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RIGHTLEFT
		&& !revins_on
#ਜੇ_ਅੰਤ
			    )
	{
	    save_ਖਾਨਾ = curwin->w_cursor.ਖਾਨਾ;
	    beginline(BL_WHITE);
	    ਜੇ (curwin->w_cursor.ਖਾਨਾ < save_ਖਾਨਾ)
	    {
		mincol = curwin->w_cursor.ਖਾਨਾ;
		// should now fix the indent to match with the previous line
		call_fix_indent = TRUE;
	    }
	    curwin->w_cursor.ਖਾਨਾ = save_ਖਾਨਾ;
	}

	/*
	 * Handle deleting one 'shiftwidth' or 'softtabstop'.
	 */
	ਜੇ (	   ਬਹੁਲਕ == BACKSPACE_CHAR
		&& ((p_sta && ਵਿੱਚ_indent)
		    || ((get_sts_ਮੁੱਲ() != 0
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_VARTABS
			|| tabstop_ਗਿਣਤੀ(curbuf->b_p_vsts_array)
#ਜੇ_ਅੰਤ
			)
			&& curwin->w_cursor.ਖਾਨਾ > 0
			&& (*(ml_get_cursor() - 1) == TAB
			    || (*(ml_get_cursor() - 1) == ' '
				&& (!*inserted_space_p
				    || arrow_ਵਰਤਿਆ))))))
	{
	    colnr_T	vcol = 0;
	    colnr_T	want_vcol;
	    char_u	*ਸਤਰ;
	    char_u	*ptr;
	    char_u	*cursor_ptr;
	    char_u	*space_ptr;
	    colnr_T	space_vcol = 0;
	    ਪੂਰਨ		prev_space = FALSE;
	    colnr_T	want_ਖਾਨਾ;

	    *inserted_space_p = FALSE;

	    space_ptr = ptr = ਸਤਰ = ml_get_curline();
	    cursor_ptr = ਸਤਰ + curwin->w_cursor.ਖਾਨਾ;

	    // Compute virtual column of cursor position, and find the last
	    // whitespace before cursor that is preceded by non-whitespace.
	    // Use chartabsize() so that virtual text and wrapping are ignored.
	    ਜਦੋਂ (ptr < cursor_ptr)
	    {
		ਪੂਰਨ	cur_space = VIM_ISWHITE(*ptr);

		ਜੇ (!prev_space && cur_space)
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
	    ਜੇ (p_sta && ਵਿੱਚ_indent)
		want_vcol -= want_vcol % (ਪੂਰਨ)get_sw_ਮੁੱਲ(curbuf);
	    ਨਹੀਂਤਾਂ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_VARTABS
		want_vcol = tabstop_ਸ਼ੁਰੂ(want_vcol, get_sts_ਮੁੱਲ(),
						       curbuf->b_p_vsts_array);
#ਨਹੀਂਤਾਂ
		want_vcol -= want_vcol % (ਪੂਰਨ)get_sts_ਮੁੱਲ();
#ਜੇ_ਅੰਤ

	    // Find the position to stop backspacing.
	    // Use chartabsize() so that virtual text and wrapping are ignored.
	    ਜਦੋਂ (TRUE)
	    {
		ਪੂਰਨ ਮਾਪ = chartabsize(space_ptr, space_vcol);

		ਜੇ (space_vcol + ਮਾਪ > want_vcol)
		    ਰੋਕੋ;
		space_vcol += ਮਾਪ;
		MB_PTR_ADV(space_ptr);
	    }
	    want_ਖਾਨਾ = space_ptr - ਸਤਰ;

	    // Delete characters until we are at or before want_col.
	    ਜਦੋਂ (curwin->w_cursor.ਖਾਨਾ > want_ਖਾਨਾ)
		ins_bs_one();

	    // Insert extra spaces until we are at want_vcol.
	    ਲਈ (; space_vcol < want_vcol; space_vcol++)
	    {
		// Remember the first char we inserted
		ਜੇ (curwin->w_cursor.lnum == Insstart_orig.lnum
				   && curwin->w_cursor.ਖਾਨਾ < Insstart_orig.ਖਾਨਾ)
		    Insstart_orig.ਖਾਨਾ = curwin->w_cursor.ਖਾਨਾ;

		ਜੇ (State & VREPLACE_FLAG)
		    ins_char(' ');
		ਨਹੀਂਤਾਂ
		{
		    ins_str((char_u *)" ", 1);
		    ਜੇ ((State & REPLACE_FLAG))
			replace_ਧੱਕੋ(NUL);
		}
	    }
	}

	/*
	 * Delete up to starting point, start of line or previous word.
	 */
	ਨਹੀਂਤਾਂ
	{
	    ਪੂਰਨ cclass = 0, prev_cclass = 0;

	    ਜੇ (has_mbyte)
		cclass = mb_get_class(ml_get_cursor());
	    ਕਰੋ
	    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RIGHTLEFT
		ਜੇ (!revins_on) // put cursor on char to be deleted
#ਜੇ_ਅੰਤ
		    dec_cursor();

		cc = gchar_cursor();
		// look multi-byte character class
		ਜੇ (has_mbyte)
		{
		    prev_cclass = cclass;
		    cclass = mb_get_class(ml_get_cursor());
		}

		// start of word?
		ਜੇ (ਬਹੁਲਕ == BACKSPACE_ਸ਼ਬਦ && !vim_isspace(cc))
		{
		    ਬਹੁਲਕ = BACKSPACE_ਸ਼ਬਦ_NOT_SPACE;
		    ਆਰਜ਼ੀ = vim_iswordc(cc);
		}
		// end of word?
		ਨਹੀਂਤਾਂ ਜੇ (ਬਹੁਲਕ == BACKSPACE_ਸ਼ਬਦ_NOT_SPACE
			&& ((vim_isspace(cc) || vim_iswordc(cc) != ਆਰਜ਼ੀ)
			|| prev_cclass != cclass))
		{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RIGHTLEFT
		    ਜੇ (!revins_on)
#ਜੇ_ਅੰਤ
			inc_cursor();
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RIGHTLEFT
		    ਨਹੀਂਤਾਂ ਜੇ (State & REPLACE_FLAG)
			dec_cursor();
#ਜੇ_ਅੰਤ
		    ਰੋਕੋ;
		}
		ਜੇ (State & REPLACE_FLAG)
		    replace_do_bs(-1);
		ਨਹੀਂਤਾਂ
		{
		    ਜੇ (enc_utf8 && p_deco)
			(ਖਾਲੀ)utfc_ptr2char(ml_get_cursor(), cpc);
		    (ਖਾਲੀ)del_char(FALSE);
		    /*
		     * If there are combining characters and 'delcombine' is set
		     * move the cursor back.  Don't back up before the base
		     * character.
		     */
		    ਜੇ (enc_utf8 && p_deco && cpc[0] != NUL)
			inc_cursor();
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RIGHTLEFT
		    ਜੇ (revins_chars)
		    {
			revins_chars--;
			revins_legal++;
		    }
		    ਜੇ (revins_on && gchar_cursor() == NUL)
			ਰੋਕੋ;
#ਜੇ_ਅੰਤ
		}
		// Just a single backspace?:
		ਜੇ (ਬਹੁਲਕ == BACKSPACE_CHAR)
		    ਰੋਕੋ;
	    } ਜਦੋਂ (
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RIGHTLEFT
		    revins_on ||
#ਜੇ_ਅੰਤ
		    (curwin->w_cursor.ਖਾਨਾ > mincol
		    &&  (can_bs(BS_NOSTOP)
			|| (curwin->w_cursor.lnum != Insstart_orig.lnum
			|| curwin->w_cursor.ਖਾਨਾ != Insstart_orig.ਖਾਨਾ)
		    )));
	}
	did_backspace = TRUE;
    }
    did_si = FALSE;
    can_si = FALSE;
    can_si_ਪਿੱਠ = FALSE;
    ਜੇ (curwin->w_cursor.ਖਾਨਾ <= 1)
	did_ai = FALSE;

    ਜੇ (call_fix_indent)
	fix_indent();

    /*
     * It's a little strange to put backspaces into the redo
     * buffer, but it makes auto-indent a lot easier to deal
     * with.
     */
    Append_Char_ਮੰਜ਼ਿਲ_Redobuff(c);

    // If deleted before the insertion point, adjust it
    ਜੇ (curwin->w_cursor.lnum == Insstart_orig.lnum
				  && curwin->w_cursor.ਖਾਨਾ < Insstart_orig.ਖਾਨਾ)
	Insstart_orig.ਖਾਨਾ = curwin->w_cursor.ਖਾਨਾ;

    // vi behaviour: the cursor moves backward but the character that
    //		     was there remains visible
    // Vim behaviour: the cursor moves backward and the character that
    //		      was there is erased from the screen.
    // We can emulate the vi behaviour by pretending there is a dollar
    // displayed even when there isn't.
    //  --pkv Sun Jan 19 01:56:40 EST 2003
    ਜੇ (vim_strchr(p_cpo, CPO_BACKSPACE) != ਨਿਰਾ && dollar_vcol == -1)
	dollar_vcol = curwin->w_virtcol;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
    // When deleting a char the cursor line must never be in a closed fold.
    // E.g., when 'foldmethod' is indent and deleting the first non-white
    // char before a Tab.
    ਜੇ (did_backspace)
	foldOpenCursor();
#ਜੇ_ਅੰਤ

    ਵਾਪਸ did_backspace;
}

/*
 * Handle receiving P_PS: start paste mode.  Inserts the following text up to
 * P_PE literally.
 * When "drop" is TRUE then consume the text and drop it.
 */
    ਪੂਰਨ
bracketed_paste(paste_ਬਹੁਲਕ_T ਬਹੁਲਕ, ਪੂਰਨ drop, garray_T *gap)
{
    ਪੂਰਨ		c;
    char_u	buf[NUMBUFLEN + MB_MAXBYTES];
    ਪੂਰਨ		idx = 0;
    char_u	*ਸਿਰਾ = ਲੱਭੋ_termcode((char_u *)"PE");
    ਪੂਰਨ		ret_char = -1;
    ਪੂਰਨ		save_allow_ਕੁੰਜੀਆਂ = allow_ਕੁੰਜੀਆਂ;
    ਪੂਰਨ		save_paste = p_paste;

    // If the end code is too long we can't detect it, read everything.
    ਜੇ (ਸਿਰਾ != ਨਿਰਾ && STRLEN(ਸਿਰਾ) >= NUMBUFLEN)
	ਸਿਰਾ = ਨਿਰਾ;
    ++no_mapping;
    allow_ਕੁੰਜੀਆਂ = 0;
    ਜੇ (!p_paste)
	// Also have the side effects of setting 'paste' to make it work much
	// faster.
	ਸੈੱਟ_option_ਮੁੱਲ_give_err((char_u *)"paste", TRUE, ਨਿਰਾ, 0);

    ਲਈ (;;)
    {
	// When the end is not defined read everything there is.
	ਜੇ (ਸਿਰਾ == ਨਿਰਾ && vpeekc() == NUL)
	    ਰੋਕੋ;
	ਕਰੋ
	    c = vgetc();
	ਜਦੋਂ (c == K_IGNORE || c == K_VER_SCROLLBAR || c == K_HOR_SCROLLBAR);

	ਜੇ (c == NUL || got_int || (ex_ਸਾਧਾਰਨ_busy > 0 && c == Ctrl_C))
	    // When CTRL-C was encountered the typeahead will be flushed and we
	    // won't get the end sequence.  Except when using ":normal".
	    ਰੋਕੋ;

	ਜੇ (has_mbyte)
	    idx += (*mb_char2bytes)(c, buf + idx);
	ਨਹੀਂਤਾਂ
	    buf[idx++] = c;
	buf[idx] = NUL;
	ਜੇ (ਸਿਰਾ != ਨਿਰਾ && STRNCMP(buf, ਸਿਰਾ, idx) == 0)
	{
	    ਜੇ (ਸਿਰਾ[idx] == NUL)
		ਰੋਕੋ; // Found the end of paste code.
	    ਜਾਰੀ;
	}
	ਜੇ (!drop)
	{
	    ਚੋਣ (ਬਹੁਲਕ)
	    {
		ਹਾਲਤ PASTE_CMDLINE:
		    put_on_cmdline(buf, idx, TRUE);
		    ਰੋਕੋ;

		ਹਾਲਤ PASTE_EX:
		    // add one for the NUL that is going to be appended
		    ਜੇ (gap != ਨਿਰਾ && ga_grow(gap, idx + 1) == OK)
		    {
			mch_memmove((ਅੱਖਰ *)gap->ga_ਅੰਕੜੇ + gap->ga_len,
							     buf, (size_t)idx);
			gap->ga_len += idx;
		    }
		    ਰੋਕੋ;

		ਹਾਲਤ PASTE_ਪਾਓ:
		    ਜੇ (stop_arrow() == OK)
		    {
			c = buf[0];
			ਜੇ (idx == 1 && (c == CAR || c == K_KENTER || c == NL))
			    ins_eol(c);
			ਨਹੀਂਤਾਂ
			{
			    ins_char_ਬਾਈਟਾਂ(buf, idx);
			    Append_ਮੰਜ਼ਿਲ_Redobuff_Lit(buf, idx);
			}
		    }
		    ਰੋਕੋ;

		ਹਾਲਤ PASTE_ONE_CHAR:
		    ਜੇ (ret_char == -1)
		    {
			ਜੇ (has_mbyte)
			    ret_char = (*mb_ptr2char)(buf);
			ਨਹੀਂਤਾਂ
			    ret_char = buf[0];
		    }
		    ਰੋਕੋ;
	    }
	}
	idx = 0;
    }

    --no_mapping;
    allow_ਕੁੰਜੀਆਂ = save_allow_ਕੁੰਜੀਆਂ;
    ਜੇ (!save_paste)
	ਸੈੱਟ_option_ਮੁੱਲ_give_err((char_u *)"paste", FALSE, ਨਿਰਾ, 0);

    ਵਾਪਸ ret_char;
}

#ਜੇ defined(FEAT_GUI_TABLINE)
    ਸਥਿਰ ਖਾਲੀ
ins_tabline(ਪੂਰਨ c)
{
    // We will be leaving the current window, unless closing another tab.
    ਜੇ (c != K_TABMENU || current_tabmenu != TABLINE_MENU_CLOSE
		|| (current_tab != 0 && current_tab != tabpage_ਸੂਚਕਾਂਕ(curtab)))
    {
	undisplay_dollar();
	ਸ਼ੁਰੂ_arrow(&curwin->w_cursor);
	can_cindent = TRUE;
    }

    ਜੇ (c == K_TABLINE)
	goto_tabpage(current_tab);
    ਨਹੀਂਤਾਂ
    {
	handle_tabmenu();
	redraw_statuslines();	// will redraw the tabline when needed
    }
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_GUI)
    ਖਾਲੀ
ins_scroll(ਖਾਲੀ)
{
    pos_T	tpos;

    undisplay_dollar();
    tpos = curwin->w_cursor;
    ਜੇ (gui_do_scroll())
    {
	ਸ਼ੁਰੂ_arrow(&tpos);
	can_cindent = TRUE;
    }
}

    ਖਾਲੀ
ins_horscroll(ਖਾਲੀ)
{
    pos_T	tpos;

    undisplay_dollar();
    tpos = curwin->w_cursor;
    ਜੇ (do_mousescroll_horiz(scrollbar_ਮੁੱਲ))
    {
	ਸ਼ੁਰੂ_arrow(&tpos);
	can_cindent = TRUE;
    }
}
#ਜੇ_ਅੰਤ

    ਸਥਿਰ ਖਾਲੀ
ins_ਖੱਬਾ(ਖਾਲੀ)
{
    pos_T	tpos;
    ਪੂਰਨ		ਸਿਰਾ_change = dont_sync_undo == FALSE; // end undoable change

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
    ਜੇ ((fdo_flags & FDO_HOR) && ਕੁੰਜੀ_Typed)
	foldOpenCursor();
#ਜੇ_ਅੰਤ
    undisplay_dollar();
    tpos = curwin->w_cursor;
    ਜੇ (oneleft() == OK)
    {
#ਜੇ defined(FEAT_XIM) && defined(FEAT_GUI_GTK)
	// Only call start_arrow() when not busy with preediting, it will
	// break undo.  K_LEFT is inserted in im_correct_cursor().
	ਜੇ (p_imst == IM_OVER_THE_SPOT || !im_is_preediting())
#ਜੇ_ਅੰਤ
	{
	    ਸ਼ੁਰੂ_arrow_with_change(&tpos, ਸਿਰਾ_change);
	    ਜੇ (!ਸਿਰਾ_change)
		Append_Char_ਮੰਜ਼ਿਲ_Redobuff(K_ਖੱਬਾ);
	}
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RIGHTLEFT
	// If exit reversed string, position is fixed
	ਜੇ (revins_scol != -1 && (ਪੂਰਨ)curwin->w_cursor.ਖਾਨਾ >= revins_scol)
	    revins_legal++;
	revins_chars++;
#ਜੇ_ਅੰਤ
    }

    /*
     * if 'whichwrap' set for cursor in insert mode may go to
     * previous line
     */
    ਨਹੀਂਤਾਂ ਜੇ (vim_strchr(p_ww, '[') != ਨਿਰਾ && curwin->w_cursor.lnum > 1)
    {
	// always break undo when moving upwards/downwards, else undo may break
	ਸ਼ੁਰੂ_arrow(&tpos);
	--(curwin->w_cursor.lnum);
	coladvance((colnr_T)MAXCOL);
	curwin->w_ਸੈੱਟ_curswant = true;	// so we stay at the end
    }
    ਨਹੀਂਤਾਂ
	vim_beep(BO_CRSR);
    dont_sync_undo = FALSE;
}

    ਸਥਿਰ ਖਾਲੀ
ins_home(ਪੂਰਨ c)
{
    pos_T	tpos;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
    ਜੇ ((fdo_flags & FDO_HOR) && ਕੁੰਜੀ_Typed)
	foldOpenCursor();
#ਜੇ_ਅੰਤ
    undisplay_dollar();
    tpos = curwin->w_cursor;
    ਜੇ (c == K_C_HOME)
	curwin->w_cursor.lnum = 1;
    curwin->w_cursor.ਖਾਨਾ = 0;
    curwin->w_cursor.coladd = 0;
    curwin->w_curswant = 0;
    ਸ਼ੁਰੂ_arrow(&tpos);
}

    ਸਥਿਰ ਖਾਲੀ
ins_ਸਿਰਾ(ਪੂਰਨ c)
{
    pos_T	tpos;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
    ਜੇ ((fdo_flags & FDO_HOR) && ਕੁੰਜੀ_Typed)
	foldOpenCursor();
#ਜੇ_ਅੰਤ
    undisplay_dollar();
    tpos = curwin->w_cursor;
    ਜੇ (c == K_C_ਸਿਰਾ)
	curwin->w_cursor.lnum = curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ;
    coladvance((colnr_T)MAXCOL);
    curwin->w_curswant = MAXCOL;

    ਸ਼ੁਰੂ_arrow(&tpos);
}

    ਸਥਿਰ ਖਾਲੀ
ins_s_ਖੱਬਾ(ਖਾਲੀ)
{
    ਪੂਰਨ ਸਿਰਾ_change = dont_sync_undo == FALSE; // end undoable change
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
    ਜੇ ((fdo_flags & FDO_HOR) && ਕੁੰਜੀ_Typed)
	foldOpenCursor();
#ਜੇ_ਅੰਤ
    undisplay_dollar();
    ਜੇ (curwin->w_cursor.lnum > 1 || curwin->w_cursor.ਖਾਨਾ > 0)
    {
	ਸ਼ੁਰੂ_arrow_with_change(&curwin->w_cursor, ਸਿਰਾ_change);
	ਜੇ (!ਸਿਰਾ_change)
	    Append_Char_ਮੰਜ਼ਿਲ_Redobuff(K_S_ਖੱਬਾ);
	(ਖਾਲੀ)bck_ਸ਼ਬਦ(1L, FALSE, FALSE);
	curwin->w_ਸੈੱਟ_curswant = true;
    }
    ਨਹੀਂਤਾਂ
	vim_beep(BO_CRSR);
    dont_sync_undo = FALSE;
}

    ਸਥਿਰ ਖਾਲੀ
ins_ਸੱਜਾ(ਖਾਲੀ)
{
    ਪੂਰਨ ਸਿਰਾ_change = dont_sync_undo == FALSE; // end undoable change

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
    ਜੇ ((fdo_flags & FDO_HOR) && ਕੁੰਜੀ_Typed)
	foldOpenCursor();
#ਜੇ_ਅੰਤ
    undisplay_dollar();
    ਜੇ (gchar_cursor() != NUL || virtual_ਸਰਗਰਮ())
    {
	ਸ਼ੁਰੂ_arrow_with_change(&curwin->w_cursor, ਸਿਰਾ_change);
	ਜੇ (!ਸਿਰਾ_change)
	    Append_Char_ਮੰਜ਼ਿਲ_Redobuff(K_ਸੱਜਾ);
	curwin->w_ਸੈੱਟ_curswant = true;
	ਜੇ (virtual_ਸਰਗਰਮ())
	    oneright();
	ਨਹੀਂਤਾਂ
	{
	    ਜੇ (has_mbyte)
		curwin->w_cursor.ਖਾਨਾ += (*mb_ptr2len)(ml_get_cursor());
	    ਨਹੀਂਤਾਂ
		++curwin->w_cursor.ਖਾਨਾ;
	}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RIGHTLEFT
	revins_legal++;
	ਜੇ (revins_chars)
	    revins_chars--;
#ਜੇ_ਅੰਤ
    }
    // if 'whichwrap' set for cursor in insert mode, may move the
    // cursor to the next line
    ਨਹੀਂਤਾਂ ਜੇ (vim_strchr(p_ww, ']') != ਨਿਰਾ
	    && curwin->w_cursor.lnum < curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
    {
	ਸ਼ੁਰੂ_arrow(&curwin->w_cursor);
	curwin->w_ਸੈੱਟ_curswant = true;
	++curwin->w_cursor.lnum;
	curwin->w_cursor.ਖਾਨਾ = 0;
    }
    ਨਹੀਂਤਾਂ
	vim_beep(BO_CRSR);
    dont_sync_undo = FALSE;
}

    ਸਥਿਰ ਖਾਲੀ
ins_s_ਸੱਜਾ(ਖਾਲੀ)
{
    ਪੂਰਨ ਸਿਰਾ_change = dont_sync_undo == FALSE; // end undoable change
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
    ਜੇ ((fdo_flags & FDO_HOR) && ਕੁੰਜੀ_Typed)
	foldOpenCursor();
#ਜੇ_ਅੰਤ
    undisplay_dollar();
    ਜੇ (curwin->w_cursor.lnum < curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ
	    || gchar_cursor() != NUL)
    {
	ਸ਼ੁਰੂ_arrow_with_change(&curwin->w_cursor, ਸਿਰਾ_change);
	ਜੇ (!ਸਿਰਾ_change)
	    Append_Char_ਮੰਜ਼ਿਲ_Redobuff(K_S_ਸੱਜਾ);
	(ਖਾਲੀ)fwd_ਸ਼ਬਦ(1L, FALSE, 0);
	curwin->w_ਸੈੱਟ_curswant = true;
    }
    ਨਹੀਂਤਾਂ
	vim_beep(BO_CRSR);
    dont_sync_undo = FALSE;
}

    ਸਥਿਰ ਖਾਲੀ
ins_ਉੱਪਰ(
    ਪੂਰਨ		startcol)	// when TRUE move to Insstart.col
{
    pos_T	tpos;
    linenr_T	old_topline = curwin->w_topline;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
    ਪੂਰਨ		old_topfill = curwin->w_topfill;
#ਜੇ_ਅੰਤ

    undisplay_dollar();
    tpos = curwin->w_cursor;
    ਜੇ (cursor_ਉੱਪਰ(1L, TRUE) == OK)
    {
	ਜੇ (startcol)
	    coladvance(getvcol_nolist(&Insstart));
	ਜੇ (old_topline != curwin->w_topline
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
		|| old_topfill != curwin->w_topfill
#ਜੇ_ਅੰਤ
		)
	    redraw_later(UPD_VALID);
	ਸ਼ੁਰੂ_arrow(&tpos);
	can_cindent = TRUE;
    }
    ਨਹੀਂਤਾਂ
	vim_beep(BO_CRSR);
}

    ਸਥਿਰ ਖਾਲੀ
ins_pageup(ਖਾਲੀ)
{
    pos_T	tpos;

    undisplay_dollar();

    ਜੇ (mod_ਨਕਾਬ & MOD_ਨਕਾਬ_CTRL)
    {
	// <C-PageUp>: tab page back
	ਜੇ (ਪਹਿਲਾ_tabpage->tp_ਅਗਲਾ != ਨਿਰਾ)
	{
	    ਸ਼ੁਰੂ_arrow(&curwin->w_cursor);
	    goto_tabpage(-1);
	}
	ਵਾਪਸ;
    }

    tpos = curwin->w_cursor;
    ਜੇ (pagescroll(BACKWARD, 1L, FALSE) == OK)
    {
	ਸ਼ੁਰੂ_arrow(&tpos);
	can_cindent = TRUE;
    }
    ਨਹੀਂਤਾਂ
	vim_beep(BO_CRSR);
}

    ਸਥਿਰ ਖਾਲੀ
ins_ਥੱਲੇ(
    ਪੂਰਨ		startcol)	// when TRUE move to Insstart.col
{
    pos_T	tpos;
    linenr_T	old_topline = curwin->w_topline;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
    ਪੂਰਨ		old_topfill = curwin->w_topfill;
#ਜੇ_ਅੰਤ

    undisplay_dollar();
    tpos = curwin->w_cursor;
    ਜੇ (cursor_ਥੱਲੇ(1L, TRUE) == OK)
    {
	ਜੇ (startcol)
	    coladvance(getvcol_nolist(&Insstart));
	ਜੇ (old_topline != curwin->w_topline
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
		|| old_topfill != curwin->w_topfill
#ਜੇ_ਅੰਤ
		)
	    redraw_later(UPD_VALID);
	ਸ਼ੁਰੂ_arrow(&tpos);
	can_cindent = TRUE;
    }
    ਨਹੀਂਤਾਂ
	vim_beep(BO_CRSR);
}

    ਸਥਿਰ ਖਾਲੀ
ins_pagedown(ਖਾਲੀ)
{
    pos_T	tpos;

    undisplay_dollar();

    ਜੇ (mod_ਨਕਾਬ & MOD_ਨਕਾਬ_CTRL)
    {
	// <C-PageDown>: tab page forward
	ਜੇ (ਪਹਿਲਾ_tabpage->tp_ਅਗਲਾ != ਨਿਰਾ)
	{
	    ਸ਼ੁਰੂ_arrow(&curwin->w_cursor);
	    goto_tabpage(0);
	}
	ਵਾਪਸ;
    }

    tpos = curwin->w_cursor;
    ਜੇ (pagescroll(FORWARD, 1L, FALSE) == OK)
    {
	ਸ਼ੁਰੂ_arrow(&tpos);
	can_cindent = TRUE;
    }
    ਨਹੀਂਤਾਂ
	vim_beep(BO_CRSR);
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DND
    ਸਥਿਰ ਖਾਲੀ
ins_drop(ਖਾਲੀ)
{
    do_put('~', ਨਿਰਾ, BACKWARD, 1L, PUT_CURSEND);
}
#ਜੇ_ਅੰਤ

/*
 * Handle TAB in Insert or Replace mode.
 * Return TRUE when the TAB needs to be inserted like a normal character.
 */
    ਸਥਿਰ ਪੂਰਨ
ins_tab(ਖਾਲੀ)
{
    ਪੂਰਨ		ind;
    ਪੂਰਨ		i;
    ਪੂਰਨ		ਆਰਜ਼ੀ;

    ਜੇ (Insstart_blank_vcol == MAXCOL && curwin->w_cursor.lnum == Insstart.lnum)
	Insstart_blank_vcol = get_nolist_virtcol();
    ਜੇ (echeck_abbr(TAB + ABBR_OFF))
	ਵਾਪਸ FALSE;

    ind = inindent(0);
    ਜੇ (ind)
	can_cindent = FALSE;

    /*
     * When nothing special, insert TAB like a normal character.
     */
    ਜੇ (!curbuf->b_p_et
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_VARTABS
	    && !(p_sta && ind
		// These five lines mean 'tabstop' != 'shiftwidth'
		&& ((tabstop_ਗਿਣਤੀ(curbuf->b_p_vts_array) > 1)
		    || (tabstop_ਗਿਣਤੀ(curbuf->b_p_vts_array) == 1
			&& tabstop_ਪਹਿਲਾ(curbuf->b_p_vts_array)
						       != get_sw_ਮੁੱਲ(curbuf))
		    || (tabstop_ਗਿਣਤੀ(curbuf->b_p_vts_array) == 0
			&& curbuf->b_p_ts != get_sw_ਮੁੱਲ(curbuf))))
	    && tabstop_ਗਿਣਤੀ(curbuf->b_p_vsts_array) == 0
#ਨਹੀਂਤਾਂ
	    && !(p_sta && ind && curbuf->b_p_ts != get_sw_ਮੁੱਲ(curbuf))
#ਜੇ_ਅੰਤ
	    && get_sts_ਮੁੱਲ() == 0)
	ਵਾਪਸ TRUE;

    ਜੇ (stop_arrow() == FAIL)
	ਵਾਪਸ TRUE;

    did_ai = FALSE;
    did_si = FALSE;
    can_si = FALSE;
    can_si_ਪਿੱਠ = FALSE;
    Append_ਮੰਜ਼ਿਲ_Redobuff((char_u *)"\t");

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_VARTABS
    ਜੇ (p_sta && ind)		// insert tab in indent, use 'shiftwidth'
    {
	ਆਰਜ਼ੀ = (ਪੂਰਨ)get_sw_ਮੁੱਲ(curbuf);
	ਆਰਜ਼ੀ -= get_nolist_virtcol() % ਆਰਜ਼ੀ;
    }
    ਨਹੀਂਤਾਂ ਜੇ (tabstop_ਗਿਣਤੀ(curbuf->b_p_vsts_array) > 0 || curbuf->b_p_sts != 0)
				// use 'softtabstop' when set
	ਆਰਜ਼ੀ = tabstop_padding(get_nolist_virtcol(), get_sts_ਮੁੱਲ(),
						     curbuf->b_p_vsts_array);
    ਨਹੀਂਤਾਂ			// otherwise use 'tabstop'
	ਆਰਜ਼ੀ = tabstop_padding(get_nolist_virtcol(), curbuf->b_p_ts,
						     curbuf->b_p_vts_array);
#ਨਹੀਂਤਾਂ
    ਜੇ (p_sta && ind)		// insert tab in indent, use 'shiftwidth'
	ਆਰਜ਼ੀ = (ਪੂਰਨ)get_sw_ਮੁੱਲ(curbuf);
    ਨਹੀਂਤਾਂ ਜੇ (curbuf->b_p_sts != 0) // use 'softtabstop' when set
	ਆਰਜ਼ੀ = (ਪੂਰਨ)get_sts_ਮੁੱਲ();
    ਨਹੀਂਤਾਂ			// otherwise use 'tabstop'
	ਆਰਜ਼ੀ = (ਪੂਰਨ)curbuf->b_p_ts;
    ਆਰਜ਼ੀ -= get_nolist_virtcol() % ਆਰਜ਼ੀ;
#ਜੇ_ਅੰਤ

    /*
     * Insert the first space with ins_char().	It will delete one char in
     * replace mode.  Insert the rest with ins_str(); it will not delete any
     * chars.  For MODE_VREPLACE state, we use ins_char() for all characters.
     */
    ins_char(' ');
    ਜਦੋਂ (--ਆਰਜ਼ੀ > 0)
    {
	ਜੇ (State & VREPLACE_FLAG)
	    ins_char(' ');
	ਨਹੀਂਤਾਂ
	{
	    ins_str((char_u *)" ", 1);
	    ਜੇ (State & REPLACE_FLAG)	    // no char replaced
		replace_ਧੱਕੋ(NUL);
	}
    }

    /*
     * When 'expandtab' not set: Replace spaces by TABs where possible.
     */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_VARTABS
    ਜੇ (!curbuf->b_p_et && (tabstop_ਗਿਣਤੀ(curbuf->b_p_vsts_array) > 0
			    || get_sts_ਮੁੱਲ() > 0
			    || (p_sta && ind)))
#ਨਹੀਂਤਾਂ
    ਜੇ (!curbuf->b_p_et && (get_sts_ਮੁੱਲ() || (p_sta && ind)))
#ਜੇ_ਅੰਤ
    {
	char_u		*ptr;
	char_u		*saved_ਸਤਰ = ਨਿਰਾ;	// init for GCC
	pos_T		pos;
	pos_T		fpos;
	pos_T		*cursor;
	colnr_T		want_vcol, vcol;
	ਪੂਰਨ		change_ਖਾਨਾ = -1;
	ਪੂਰਨ		save_list = curwin->w_p_list;
	char_u		*tab = (char_u *)"\t";
	chartabsize_T	cts;

	/*
	 * Get the current line.  For MODE_VREPLACE state, don't make real
	 * changes yet, just work on a copy of the line.
	 */
	ਜੇ (State & VREPLACE_FLAG)
	{
	    pos = curwin->w_cursor;
	    cursor = &pos;
	    saved_ਸਤਰ = vim_strnsave(ml_get_curline(), ml_get_curline_len());
	    ਜੇ (saved_ਸਤਰ == ਨਿਰਾ)
		ਵਾਪਸ FALSE;
	    ptr = saved_ਸਤਰ + pos.ਖਾਨਾ;
	}
	ਨਹੀਂਤਾਂ
	{
	    ptr = ml_get_cursor();
	    cursor = &curwin->w_cursor;
	}

	// When 'L' is not in 'cpoptions' a tab always takes up 'ts' spaces.
	ਜੇ (vim_strchr(p_cpo, CPO_LISTWM) == ਨਿਰਾ)
	    curwin->w_p_list = FALSE;

	// Find first white before the cursor
	fpos = curwin->w_cursor;
	ਜਦੋਂ (fpos.ਖਾਨਾ > 0 && VIM_ISWHITE(ptr[-1]))
	{
	    --fpos.ਖਾਨਾ;
	    --ptr;
	}

	// In Replace mode, don't change characters before the insert point.
	ਜੇ ((State & REPLACE_FLAG)
		&& fpos.lnum == Insstart.lnum
		&& fpos.ਖਾਨਾ < Insstart.ਖਾਨਾ)
	{
	    ptr += Insstart.ਖਾਨਾ - fpos.ਖਾਨਾ;
	    fpos.ਖਾਨਾ = Insstart.ਖਾਨਾ;
	}

	// compute virtual column numbers of first white and cursor
	getvcol(curwin, &fpos, &vcol, ਨਿਰਾ, ਨਿਰਾ, 0);
	getvcol(curwin, cursor, &want_vcol, ਨਿਰਾ, ਨਿਰਾ, 0);

	init_chartabsize_arg(&cts, curwin, 0, vcol, tab, tab);

	// Use as many TABs as possible.  Beware of 'breakindent', 'showbreak'
	// and 'linebreak' adding extra virtual columns.
	ਜਦੋਂ (VIM_ISWHITE(*ptr))
	{
	    i = lbr_chartabsize(&cts);
	    ਜੇ (cts.cts_vcol + i > want_vcol)
		ਰੋਕੋ;
	    ਜੇ (*ptr != TAB)
	    {
		*ptr = TAB;
		ਜੇ (change_ਖਾਨਾ < 0)
		{
		    change_ਖਾਨਾ = fpos.ਖਾਨਾ;  // Column of first change
		    // May have to adjust Insstart
		    ਜੇ (fpos.lnum == Insstart.lnum && fpos.ਖਾਨਾ < Insstart.ਖਾਨਾ)
			Insstart.ਖਾਨਾ = fpos.ਖਾਨਾ;
		}
	    }
	    ++fpos.ਖਾਨਾ;
	    ++ptr;
	    cts.cts_vcol += i;
	}
	vcol = cts.cts_vcol;
	clear_chartabsize_arg(&cts);

	ਜੇ (change_ਖਾਨਾ >= 0)
	{
	    ਪੂਰਨ		    repl_off = 0;

	    // Skip over the spaces we need.
	    init_chartabsize_arg(&cts, curwin, 0, vcol, ptr, ptr);
	    ਜਦੋਂ (cts.cts_vcol < want_vcol && *cts.cts_ptr == ' ')
	    {
		cts.cts_vcol += lbr_chartabsize(&cts);
		++cts.cts_ptr;
		++repl_off;
	    }
	    ptr = cts.cts_ptr;
	    vcol = cts.cts_vcol;
	    clear_chartabsize_arg(&cts);

	    ਜੇ (vcol > want_vcol)
	    {
		// Must have a char with 'showbreak' just before it.
		--ptr;
		--repl_off;
	    }
	    fpos.ਖਾਨਾ += repl_off;

	    // Delete following spaces.
	    i = cursor->ਖਾਨਾ - fpos.ਖਾਨਾ;
	    ਜੇ (i > 0)
	    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
		ਜੇ (!(State & VREPLACE_FLAG))
		{
		    char_u  *newp;
		    ਪੂਰਨ	    ਖਾਨਾ;

		    newp = alloc(curbuf->b_ml.ml_ਸਤਰ_len - i);
		    ਜੇ (newp == ਨਿਰਾ)
			ਵਾਪਸ FALSE;

		    ਖਾਨਾ = ptr - curbuf->b_ml.ml_ਸਤਰ_ptr;
		    ਜੇ (ਖਾਨਾ > 0)
			mch_memmove(newp, ptr - ਖਾਨਾ, ਖਾਨਾ);
		    mch_memmove(newp + ਖਾਨਾ, ptr + i,
					   curbuf->b_ml.ml_ਸਤਰ_len - ਖਾਨਾ - i);

		    ਜੇ (curbuf->b_ml.ml_flags & (ML_ਸਤਰ_DIRTY | ML_ALLOCATED))
			vim_free(curbuf->b_ml.ml_ਸਤਰ_ptr);
		    curbuf->b_ml.ml_ਸਤਰ_ptr = newp;
		    curbuf->b_ml.ml_ਸਤਰ_len -= i;
		    curbuf->b_ml.ml_ਸਤਰ_textlen = 0;
		    curbuf->b_ml.ml_flags =
			   (curbuf->b_ml.ml_flags | ML_ਸਤਰ_DIRTY) & ~ML_ਖਾਲੀ_ਥਾਵਾਂ;
		}
		ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
		    STRMOVE(ptr, ptr + i);
		// correct replace stack.
		ਜੇ ((State & REPLACE_FLAG) && !(State & VREPLACE_FLAG))
		    ਲਈ (ਆਰਜ਼ੀ = i; --ਆਰਜ਼ੀ >= 0; )
			replace_join(repl_off);
	    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_NETBEANS_INTG
	    ਜੇ (netbeans_ਸਰਗਰਮ())
	    {
		netbeans_removed(curbuf, fpos.lnum, cursor->ਖਾਨਾ, (ਲੰਮਾ)(i + 1));
		netbeans_inserted(curbuf, fpos.lnum, cursor->ਖਾਨਾ,
							   (char_u *)"\t", 1);
	    }
#ਜੇ_ਅੰਤ
	    cursor->ਖਾਨਾ -= i;

	    /*
	     * In MODE_VREPLACE state, we haven't changed anything yet.  Do it
	     * now by backspacing over the changed spacing and then inserting
	     * the new spacing.
	     */
	    ਜੇ (State & VREPLACE_FLAG)
	    {
		// Backspace from real cursor to change_col
		backspace_until_ਕਾਲਮ(change_ਖਾਨਾ);

		// Insert each char in saved_line from changed_col to
		// ptr-cursor
		ins_ਬਾਈਟਾਂ_len(saved_ਸਤਰ + change_ਖਾਨਾ,
						    cursor->ਖਾਨਾ - change_ਖਾਨਾ);
	    }
	}

	ਜੇ (State & VREPLACE_FLAG)
	    vim_free(saved_ਸਤਰ);
	curwin->w_p_list = save_list;
    }

    ਵਾਪਸ FALSE;
}

/*
 * Handle CR or NL in insert mode.
 * Return FAIL when out of memory or can't undo.
 */
    ਪੂਰਨ
ins_eol(ਪੂਰਨ c)
{
    ਪੂਰਨ	    i;

    ਜੇ (echeck_abbr(c + ABBR_OFF))
	ਵਾਪਸ OK;
    ਜੇ (stop_arrow() == FAIL)
	ਵਾਪਸ FAIL;
    undisplay_dollar();

    /*
     * Strange Vi behaviour: In Replace mode, typing a NL will not delete the
     * character under the cursor.  Only push a NUL on the replace stack,
     * nothing to put back when the NL is deleted.
     */
    ਜੇ ((State & REPLACE_FLAG) && !(State & VREPLACE_FLAG))
	replace_ਧੱਕੋ(NUL);

    /*
     * In MODE_VREPLACE state, a NL replaces the rest of the line, and starts
     * replacing the next line, so we push all of the characters left on the
     * line onto the replace stack.  This is not done here though, it is done
     * in open_line().
     */

    // Put cursor on NUL if on the last char and coladd is 1 (happens after
    // CTRL-O).
    ਜੇ (virtual_ਸਰਗਰਮ() && curwin->w_cursor.coladd > 0)
	coladvance(getviscol());

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RIGHTLEFT
    // NL in reverse insert will always start in the end of
    // current line.
    ਜੇ (revins_on)
	curwin->w_cursor.ਖਾਨਾ += ml_get_cursor_len();
#ਜੇ_ਅੰਤ

    Append_ਮੰਜ਼ਿਲ_Redobuff(NL_STR);
    i = open_ਸਤਰ(FORWARD,
	    has_format_option(FO_RET_COMS) ? OPENLINE_DO_COM : 0, old_indent,
	    ਨਿਰਾ);
    old_indent = 0;
    can_cindent = TRUE;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
    // When inserting a line the cursor line must never be in a closed fold.
    foldOpenCursor();
#ਜੇ_ਅੰਤ

    ਵਾਪਸ i;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIGRAPHS
/*
 * Handle digraph in insert mode.
 * Returns character still to be inserted, or NUL when nothing remaining to be
 * done.
 */
    ਸਥਿਰ ਪੂਰਨ
ins_digraph(ਖਾਲੀ)
{
    ਪੂਰਨ	    c;
    ਪੂਰਨ	    cc;
    ਪੂਰਨ	    did_putchar = FALSE;

    pc_ਸਥਿਤੀ = PC_ਸਥਿਤੀ_UNSET;
    ਜੇ (redrawing() && !char_avail())
    {
	// may need to redraw when no more chars available now
	ins_redraw(FALSE);

	edit_putchar('?', TRUE);
	did_putchar = TRUE;
	ਜੋੜੋ_ਮੰਜ਼ਿਲ_showcmd_c(Ctrl_K);
    }

# ifdef USE_ON_FLY_SCROLL
    dont_scroll = TRUE;		// disallow scrolling here
# endif

    // don't map the digraph chars. This also prevents the
    // mode message to be deleted when ESC is hit
    ++no_mapping;
    ++allow_ਕੁੰਜੀਆਂ;
    c = plain_vgetc();
    --no_mapping;
    --allow_ਕੁੰਜੀਆਂ;
    ਜੇ (did_putchar)
	// when the line fits in 'columns' the '?' is at the start of the next
	// line and will not be removed by the redraw
	edit_unputchar();

    ਜੇ (IS_SPECIAL(c) || mod_ਨਕਾਬ)	    // special key
    {
	clear_showcmd();
	ਪਾਓ_special(c, TRUE, FALSE);
	ਵਾਪਸ NUL;
    }
    ਜੇ (c != ESC)
    {
	did_putchar = FALSE;
	ਜੇ (redrawing() && !char_avail())
	{
	    // may need to redraw when no more chars available now
	    ins_redraw(FALSE);

	    ਜੇ (char2cells(c) == 1)
	    {
		ins_redraw(FALSE);
		edit_putchar(c, TRUE);
		did_putchar = TRUE;
	    }
	    ਜੋੜੋ_ਮੰਜ਼ਿਲ_showcmd_c(c);
	}
	++no_mapping;
	++allow_ਕੁੰਜੀਆਂ;
	cc = plain_vgetc();
	--no_mapping;
	--allow_ਕੁੰਜੀਆਂ;
	ਜੇ (did_putchar)
	    // when the line fits in 'columns' the '?' is at the start of the
	    // next line and will not be removed by a redraw
	    edit_unputchar();
	ਜੇ (cc != ESC)
	{
	    Append_ਮੰਜ਼ਿਲ_Redobuff((char_u *)CTRL_V_STR);
	    c = digraph_get(c, cc, TRUE);
	    clear_showcmd();
	    ਵਾਪਸ c;
	}
    }
    clear_showcmd();
    ਵਾਪਸ NUL;
}
#ਜੇ_ਅੰਤ

/*
 * Handle CTRL-E and CTRL-Y in Insert mode: copy char from other line.
 * Returns the char to be inserted, or NUL if none found.
 */
    ਪੂਰਨ
ins_copychar(linenr_T lnum)
{
    ਪੂਰਨ		    c;
    char_u	    *ptr, *prev_ptr;
    char_u	    *ਸਤਰ;
    chartabsize_T   cts;

    ਜੇ (lnum < 1 || lnum > curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
    {
	vim_beep(BO_COPY);
	ਵਾਪਸ NUL;
    }

    // try to advance to the cursor column
    validate_virtcol();
    ਸਤਰ = ml_get(lnum);
    prev_ptr = ਸਤਰ;
    init_chartabsize_arg(&cts, curwin, lnum, 0, ਸਤਰ, ਸਤਰ);
    ਜਦੋਂ (cts.cts_vcol < curwin->w_virtcol && *cts.cts_ptr != NUL)
    {
	prev_ptr = cts.cts_ptr;
	cts.cts_vcol += lbr_chartabsize_adv(&cts);
    }
    ਜੇ (cts.cts_vcol > curwin->w_virtcol)
	ptr = prev_ptr;
    ਨਹੀਂਤਾਂ
	ptr = cts.cts_ptr;
    clear_chartabsize_arg(&cts);

    c = (*mb_ptr2char)(ptr);
    ਜੇ (c == NUL)
	vim_beep(BO_COPY);
    ਵਾਪਸ c;
}

/*
 * CTRL-Y or CTRL-E typed in Insert mode.
 */
    ਸਥਿਰ ਪੂਰਨ
ins_ctrl_ey(ਪੂਰਨ tc)
{
    ਪੂਰਨ	    c = tc;

    ਜੇ (ctrl_x_ਬਹੁਲਕ_scroll())
    {
	ਜੇ (c == Ctrl_Y)
	    scrolldown_clamp();
	ਨਹੀਂਤਾਂ
	    scrollup_clamp();
	redraw_later(UPD_VALID);
    }
    ਨਹੀਂਤਾਂ
    {
	c = ins_copychar(curwin->w_cursor.lnum + (c == Ctrl_Y ? -1 : 1));
	ਜੇ (c != NUL)
	{
	    ਲੰਮਾ	tw_save;

	    // The character must be taken literally, insert like it
	    // was typed after a CTRL-V, and pretend 'textwidth'
	    // wasn't set.  Digits, 'o' and 'x' are special after a
	    // CTRL-V, don't use it for these.
	    ਜੇ (c < 256 && !ਸੁਰੱਖਿਅਤ_isalnum(c))
		Append_ਮੰਜ਼ਿਲ_Redobuff((char_u *)CTRL_V_STR);	// CTRL-V
	    tw_save = curbuf->b_p_tw;
	    curbuf->b_p_tw = -1;
	    ਪਾਓ_special(c, TRUE, FALSE);
	    curbuf->b_p_tw = tw_save;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RIGHTLEFT
	    revins_chars++;
	    revins_legal++;
#ਜੇ_ਅੰਤ
	    c = Ctrl_V;	// pretend CTRL-V is last character
	    auto_format(FALSE, TRUE);
	}
    }
    ਵਾਪਸ c;
}

/*
 * Get the value that w_virtcol would have when 'list' is off.
 * Unless 'cpo' contains the 'L' flag.
 */
    colnr_T
get_nolist_virtcol(ਖਾਲੀ)
{
    // check validity of cursor in current buffer
    ਜੇ (curwin->w_ਬਫਰ == ਨਿਰਾ
	|| curwin->w_ਬਫਰ->b_ml.ml_mfp == ਨਿਰਾ
	|| curwin->w_cursor.lnum > curwin->w_ਬਫਰ->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
	ਵਾਪਸ 0;
    ਜੇ (curwin->w_p_list && vim_strchr(p_cpo, CPO_LISTWM) == ਨਿਰਾ)
	ਵਾਪਸ getvcol_nolist(&curwin->w_cursor);
    validate_virtcol();
    ਵਾਪਸ curwin->w_virtcol;
}

#ਜੇ defined(FEAT_EVAL)
/*
 * Handle the InsertCharPre autocommand.
 * "c" is the character that was typed.
 * Return a pointer to allocated memory with the replacement string.
 * Return NULL to continue inserting "c".
 */
    ਸਥਿਰ char_u *
do_ਪਾਓ_char_pre(ਪੂਰਨ c)
{
    char_u	*res;
    char_u	buf[MB_MAXBYTES + 1];
    size_t	buflen;
    ਪੂਰਨ		save_State = State;

    // Return quickly when there is nothing to do.
    ਜੇ (!has_insertcharpre())
	ਵਾਪਸ ਨਿਰਾ;

    ਜੇ (c == Ctrl_RSB)
	ਵਾਪਸ ਨਿਰਾ;

    ਜੇ (has_mbyte)
	buflen = (*mb_char2bytes)(c, buf);
    ਨਹੀਂਤਾਂ
    {
	buf[0] = c;
	buflen = 1;
    }
    buf[buflen] = NUL;

    // Lock the text to avoid weird things from happening.
    ++textlock;
    ਸੈੱਟ_vim_var_string(VV_CHAR, buf, (ਪੂਰਨ)buflen);  // set v:char

    res = ਨਿਰਾ;
    ਜੇ (ins_apply_autocmds(EVENT_INSERTCHARPRE))
    {
	// Get the value of v:char.  It may be empty or more than one
	// character.  Only use it when changed, otherwise continue with the
	// original character to avoid breaking autoindent.
	ਜੇ (STRCMP(buf, get_vim_var_str(VV_CHAR)) != 0)
	    res = vim_strsave(get_vim_var_str(VV_CHAR));
    }

    ਸੈੱਟ_vim_var_string(VV_CHAR, ਨਿਰਾ, -1);  // clear v:char
    --textlock;

    // Restore the State, it may have been changed.
    State = save_State;

    ਵਾਪਸ res;
}
#ਜੇ_ਅੰਤ

    ਪੂਰਨ
get_can_cindent(ਖਾਲੀ)
{
    ਵਾਪਸ can_cindent;
}

    ਖਾਲੀ
ਸੈੱਟ_can_cindent(ਪੂਰਨ val)
{
    can_cindent = val;
}

/*
 * Trigger "event" and take care of fixing undo.
 */
    ਪੂਰਨ
ins_apply_autocmds(event_T event)
{
    varnumber_T	tick = CHANGEDTICK(curbuf);
    ਪੂਰਨ r;

    r = apply_autocmds(event, ਨਿਰਾ, ਨਿਰਾ, FALSE, curbuf);

    // If u_savesub() was called then we are not prepared to start
    // a new line.  Call u_save() with no contents to fix that.
    // Except when leaving Insert mode.
    ਜੇ (event != EVENT_INSERTLEAVE && tick != CHANGEDTICK(curbuf))
	u_save(curwin->w_cursor.lnum, (linenr_T)(curwin->w_cursor.lnum + 1));

    ਵਾਪਸ r;
}
