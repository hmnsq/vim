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

#ಅನ್ತರ್ಭಾವ "vim.h"

#define BACKSPACE_CHAR		    1
#define BACKSPACE_WORD		    2
#define BACKSPACE_WORD_NOT_SPACE    3
#define BACKSPACE_ಸಾಲು		    4

// Set when doing something for completion that may call edit() recursively,
// which is not allowed.
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	compl_busy = FALSE;


ಸ್ಥಿರ ಶೂನ್ಯ ins_ctrl_v(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ ಸೇರಿಸು_special(ಪೂರ್ಣಾಙ್ಕ, ಪೂರ್ಣಾಙ್ಕ, ಪೂರ್ಣಾಙ್ಕ);
ಸ್ಥಿರ ಶೂನ್ಯ redo_literal(ಪೂರ್ಣಾಙ್ಕ c);
ಸ್ಥಿರ ಶೂನ್ಯ start_arrow_common(pos_T *end_ಸೇರಿಸು_pos, ಪೂರ್ಣಾಙ್ಕ change);
#ifdef FEAT_SPELL
ಸ್ಥಿರ ಶೂನ್ಯ check_spell_redraw(ಶೂನ್ಯ);
#endif
ಸ್ಥಿರ ಶೂನ್ಯ stop_ಸೇರಿಸು(pos_T *end_ಸೇರಿಸು_pos, ಪೂರ್ಣಾಙ್ಕ esc, ಪೂರ್ಣಾಙ್ಕ nomove);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ  echeck_abbr(ಪೂರ್ಣಾಙ್ಕ);
ಸ್ಥಿರ ಶೂನ್ಯ mb_replace_ತೆಗೆ_ins(ಪೂರ್ಣಾಙ್ಕ cc);
ಸ್ಥಿರ ಶೂನ್ಯ replace_flush(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ replace_do_bs(ಪೂರ್ಣಾಙ್ಕ limit_col);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ del_char_after_col(ಪೂರ್ಣಾಙ್ಕ limit_col);
ಸ್ಥಿರ ಶೂನ್ಯ ins_reg(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ ins_ctrl_g(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ ins_ctrl_hat(ಶೂನ್ಯ);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ  ins_esc(long *ಎಣಿಕೆ, ಪೂರ್ಣಾಙ್ಕ cmdchar, ಪೂರ್ಣಾಙ್ಕ nomove);
#ifdef FEAT_RIGHTLEFT
ಸ್ಥಿರ ಶೂನ್ಯ ins_ctrl_(ಶೂನ್ಯ);
#endif
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ ins_start_select(ಪೂರ್ಣಾಙ್ಕ c);
ಸ್ಥಿರ ಶೂನ್ಯ ins_ಸೇರಿಸು(ಪೂರ್ಣಾಙ್ಕ replaceState);
ಸ್ಥಿರ ಶೂನ್ಯ ins_ctrl_o(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ ins_shift(ಪೂರ್ಣಾಙ್ಕ c, ಪೂರ್ಣಾಙ್ಕ lastc);
ಸ್ಥಿರ ಶೂನ್ಯ ins_del(ಶೂನ್ಯ);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ  ins_bs(ಪೂರ್ಣಾಙ್ಕ c, ಪೂರ್ಣಾಙ್ಕ mode, ಪೂರ್ಣಾಙ್ಕ *inserted_space_p);
#ಯದಿ defined(FEAT_GUI_TABLINE)
ಸ್ಥಿರ ಶೂನ್ಯ ins_tabline(ಪೂರ್ಣಾಙ್ಕ c);
#endif
ಸ್ಥಿರ ಶೂನ್ಯ ins_ಎಡ(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ ins_home(ಪೂರ್ಣಾಙ್ಕ c);
ಸ್ಥಿರ ಶೂನ್ಯ ins_end(ಪೂರ್ಣಾಙ್ಕ c);
ಸ್ಥಿರ ಶೂನ್ಯ ins_s_ಎಡ(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ ins_ಬಲ(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ ins_s_ಬಲ(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ ins_up(ಪೂರ್ಣಾಙ್ಕ startcol);
ಸ್ಥಿರ ಶೂನ್ಯ ins_pageup(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ ins_down(ಪೂರ್ಣಾಙ್ಕ startcol);
ಸ್ಥಿರ ಶೂನ್ಯ ins_pagedown(ಶೂನ್ಯ);
#ifdef FEAT_DND
ಸ್ಥಿರ ಶೂನ್ಯ ins_drop(ಶೂನ್ಯ);
#endif
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ  ins_tab(ಶೂನ್ಯ);
#ifdef FEAT_DIGRAPHS
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ  ins_digraph(ಶೂನ್ಯ);
#endif
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ  ins_ctrl_ey(ಪೂರ್ಣಾಙ್ಕ tc);
#ಯದಿ defined(FEAT_EVAL)
ಸ್ಥಿರ char_u *do_ಸೇರಿಸು_char_pre(ಪೂರ್ಣಾಙ್ಕ c);
#endif

ಸ್ಥಿರ colnr_T	Insstart_textlen;	// length of line when insert started
ಸ್ಥಿರ colnr_T	Insstart_blank_vcol;	// vcol for first inserted blank
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	update_Insstart_orig = TRUE; // set Insstart_orig to Insstart

ಸ್ಥಿರ string_T	last_ಸೇರಿಸು = {ರಿಕ್ತ, 0};    // the text of the previous insert,
					    // K_SPECIAL and CSI are escaped
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	last_ಸೇರಿಸು_skip; // nr of chars in front of previous insert
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	new_ಸೇರಿಸು_skip;  // nr of chars in front of current insert
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	did_restart_edit;	// "restart_edit" when calling edit()

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	can_cindent;		// may do cindenting on this line

#ifdef FEAT_RIGHTLEFT
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	revins_on;		// reverse insert mode on
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	revins_chars;		// how much to skip after edit
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	revins_legal;		// was the last char 'legal'?
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	revins_scol;		// start column of revins session
#endif

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	ins_need_undo;		// call u_save() before inserting a
					// char.  Set when edit() is called.
					// after that arrow_used is used.

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	dont_sync_undo = FALSE;	// CTRL-G U prevents syncing undo for
					// the next left/right cursor key

// With 'autocompletedelay' set, arm the delay and let the main loop fire
// Insert-mode autocommands; the popup is shown later on K_COMPLETE_DELAY.
// Otherwise trigger completion right away.
#define TRIGGER_AUTOCOMPLETE()			\
    ಕುರು {					\
	update_screen(UPD_VALID);  /* Show char (deletion) immediately */ \
	out_flush();				\
	ins_compl_enable_autocomplete();	\
	ins_compl_arm_autostart();		\
	ಯದಿ (!ins_compl_arm_autocomplete_delay())\
	    ಗಚ್ಛ docomplete;			\
    } ಯಾವತ್ (0)

#define MAY_TRIGGER_AUTOCOMPLETE(c)				\
    ಕುರು {							\
	ಯದಿ (ins_compl_has_autocomplete() && !char_avail()	\
		&& curwin->w_cursor.col > 0)			\
	{							\
	    (c) = char_before_cursor();				\
	    ಯದಿ (vim_isprintc(c))				\
		TRIGGER_AUTOCOMPLETE();				\
	}							\
    } ಯಾವತ್ (0)

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
    ಪೂರ್ಣಾಙ್ಕ
edit(
    ಪೂರ್ಣಾಙ್ಕ		cmdchar,
    ಪೂರ್ಣಾಙ್ಕ		startln,	// if set, insert at start of line
    long	ಎಣಿಕೆ)
{
    ಪೂರ್ಣಾಙ್ಕ		c = 0;
    char_u	*ptr;
    ಪೂರ್ಣಾಙ್ಕ		lastc = 0;
    ಪೂರ್ಣಾಙ್ಕ		mincol;
    ಸ್ಥಿರ linenr_T o_lnum = 0;
    ಪೂರ್ಣಾಙ್ಕ		i;
    ಪೂರ್ಣಾಙ್ಕ		did_backspace = TRUE;	    // previous char was backspace
    ಪೂರ್ಣಾಙ್ಕ		ಸಾಲು_is_white = FALSE;	    // line is empty before insert
    linenr_T	old_topline = 0;	    // topline before insertion
#ifdef FEAT_DIFF
    ಪೂರ್ಣಾಙ್ಕ		old_topfill = -1;
#endif
    ಪೂರ್ಣಾಙ್ಕ		inserted_space = FALSE;     // just inserted a space
    ಪೂರ್ಣಾಙ್ಕ		replaceState = MODE_REPLACE;
    ಪೂರ್ಣಾಙ್ಕ		nomove = FALSE;		    // don't move cursor on return
#ifdef FEAT_JOB_CHANNEL
    ಪೂರ್ಣಾಙ್ಕ		cmdchar_todo = cmdchar;
#endif
#ifdef FEAT_CONCEAL
    ಪೂರ್ಣಾಙ್ಕ		cursor_ಸಾಲು_was_concealed;
#endif
    ಪೂರ್ಣಾಙ್ಕ		ins_just_started = TRUE;

    // Remember whether editing was restarted after CTRL-O.
    did_restart_edit = restart_edit;

    // sleep before redrawing, needed for "CTRL-O :" that results in an
    // error message
    check_for_delay(TRUE);

    // set Insstart_orig to Insstart
    update_Insstart_orig = TRUE;

#ifdef HAVE_SANDBOX
    // Don't allow inserting in the sandbox.
    ಯದಿ (sandbox != 0)
    {
	emsg(_(e_not_allowed_in_sandbox));
	ಪ್ರತ್ಯಾಗಮ FALSE;
    }
#endif
    // Don't allow changes in the buffer while editing the cmdline.  The
    // caller of getcmdline() may get confused.
    // Don't allow recursive insert mode when busy with completion.
    ಯದಿ (textlock != 0 || ins_compl_active() || compl_busy || pum_visible())
    {
	emsg(_(e_not_allowed_to_change_text_or_change_window));
	ಪ್ರತ್ಯಾಗಮ FALSE;
    }
    ins_compl_clear();	    // clear stuff for CTRL-X mode

    /*
     * Trigger InsertEnter autocommands.  Do not do this for "r<CR>" or "grx".
     */
    ಯದಿ (cmdchar != 'r' && cmdchar != 'v')
    {
	pos_T   save_cursor = curwin->w_cursor;

#ifdef FEAT_EVAL
	ಯದಿ (cmdchar == 'R')
	    ptr = (char_u *)"r";
	ಅನ್ಯಥಾ ಯದಿ (cmdchar == 'V')
	    ptr = (char_u *)"v";
	ಅನ್ಯಥಾ
	    ptr = (char_u *)"i";
	set_vim_var_string(VV_INSERTMODE, ptr, 1);
	set_vim_var_string(VV_CHAR, ರಿಕ್ತ, -1);  // clear v:char
#endif
	ins_apply_autocmds(EVENT_INSERTENTER);

	// Check for changed highlighting, e.g. for ModeMsg.
	ಯದಿ (need_highlight_changed)
	    highlight_changed();

	// Make sure the cursor didn't move.  Do call check_cursor_col() in
	// case the text was modified.  Since Insert mode was not started yet
	// a call to check_cursor_col() may move the cursor, especially with
	// the "A" command, thus set State to avoid that. Also check that the
	// line number is still valid (lines may have been deleted).
	// Do not restore if v:char was set to a non-empty string.
	ಯದಿ (!EQUAL_POS(curwin->w_cursor, save_cursor)
#ifdef FEAT_EVAL
		&& *get_vim_var_str(VV_CHAR) == NUL
#endif
		&& save_cursor.lnum <= curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	{
	    ಪೂರ್ಣಾಙ್ಕ save_state = State;

	    curwin->w_cursor = save_cursor;
	    State = MODE_ಸೇರಿಸು;
	    check_cursor_col();
	    State = save_state;
	}
    }

#ifdef FEAT_CONCEAL
    // Check if the cursor line was concealed before changing State.
    cursor_ಸಾಲು_was_concealed = curwin->w_p_cole > 0
						&& conceal_cursor_ಸಾಲು(curwin);
#endif

    /*
     * When doing a paste with the middle mouse button, Insstart is set to
     * where the paste started.
     */
    ಯದಿ (where_paste_started.lnum != 0)
	Insstart = where_paste_started;
    ಅನ್ಯಥಾ
    {
	Insstart = curwin->w_cursor;
	ಯದಿ (startln)
	    Insstart.col = 0;
    }
    Insstart_textlen = (colnr_T)linetabsize_str(ml_get_curline());
    Insstart_blank_vcol = MAXCOL;
    ಯದಿ (!did_ai)
	ai_col = 0;

    ಯದಿ (cmdchar != NUL && restart_edit == 0)
    {
	ResetRedobuff();
	Append_ಸಂಖ್ಯೆ_To_Redobuff(ಎಣಿಕೆ);
	ಯದಿ (cmdchar == 'V' || cmdchar == 'v')
	{
	    // "gR" or "gr" command
	    AppendCharToRedobuff('g');
	    AppendCharToRedobuff((cmdchar == 'v') ? 'r' : 'R');
	}
	ಅನ್ಯಥಾ
	{
	    ಯದಿ (cmdchar == K_PS)
		AppendCharToRedobuff('a');
	    ಅನ್ಯಥಾ
		AppendCharToRedobuff(cmdchar);
	    ಯದಿ (cmdchar == 'g')		    // "gI" command
		AppendCharToRedobuff('I');
	    ಅನ್ಯಥಾ ಯದಿ (cmdchar == 'r')	    // "r<CR>" command
		ಎಣಿಕೆ = 1;		    // insert only one <CR>
	}
    }

    ಯದಿ (cmdchar == 'R')
    {
	State = MODE_REPLACE;
    }
    ಅನ್ಯಥಾ ಯದಿ (cmdchar == 'V' || cmdchar == 'v')
    {
	State = MODE_VREPLACE;
	replaceState = MODE_VREPLACE;
	orig_ಸಾಲು_ಎಣಿಕೆ = curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
	vr_ಸಾಲುಗಳು_changed = 1;
    }
    ಅನ್ಯಥಾ
	State = MODE_ಸೇರಿಸು;

    may_trigger_modechanged();
    stop_ಸೇರಿಸು_mode = FALSE;

#ifdef FEAT_CONCEAL
    // Check if the cursor line needs redrawing after changing State.  If
    // 'concealcursor' is "n" it needs to be redrawn without concealing.
    conceal_check_cursor_ಸಾಲು(cursor_ಸಾಲು_was_concealed);
#endif

    // Need to position cursor again when on a TAB and when on a char with
    // virtual text.
    ಯದಿ (gchar_cursor() == TAB
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
    ಯದಿ (curbuf->b_p_iminsert == B_IMODE_LMAP)
	State |= MODE_LANGMAP;
#ifdef HAVE_ಒಳಹರಿವು_METHOD
    im_set_active(curbuf->b_p_iminsert == B_IMODE_IM);
#endif

    setmouse();
    clear_showcmd();
#ifdef FEAT_RIGHTLEFT
    // there is no reverse replace mode
    revins_on = (State == MODE_ಸೇರಿಸು && p_ri);
    ಯದಿ (revins_on)
	undisplay_dollar();
    revins_chars = 0;
    revins_legal = 0;
    revins_scol = -1;
#endif
    ಯದಿ (!p_ek)
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
    ಯದಿ (restart_edit != 0 && stuff_empty())
    {
	/*
	 * After a paste we consider text typed to be part of the insert for
	 * the pasted text. You can backspace over the pasted text too.
	 */
	ಯದಿ (where_paste_started.lnum)
	    arrow_used = FALSE;
	ಅನ್ಯಥಾ
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
	ಯದಿ (((ins_at_eol && curwin->w_cursor.lnum == o_lnum)
		    || curwin->w_curswant > curwin->w_virtcol)
		&& *(ptr = ml_get_curline() + curwin->w_cursor.col) != NUL)
	{
	    ಯದಿ (ptr[1] == NUL)
		++curwin->w_cursor.col;
	    ಅನ್ಯಥಾ ಯದಿ (has_mbyte)
	    {
		i = (*mb_ptr2len)(ptr);
		ಯದಿ (ptr[i] == NUL)
		    curwin->w_cursor.col += i;
	    }
	}
	ins_at_eol = FALSE;
    }
    ಅನ್ಯಥಾ
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
    ಯದಿ (!p_im && did_restart_edit == 0)
	foldOpenCursor();
#endif

    /*
     * If 'showmode' is set, show the current (insert/replace/..) mode.
     * A warning message for changing a readonly file is given here, before
     * actually changing anything.  It's put after the mode, if any.
     */
    i = 0;
    ಯದಿ (p_smd && ಸಂದೇಶ_silent == 0)
	i = showmode();

    ಯದಿ (!p_im && did_restart_edit == 0)
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
    new_ಸೇರಿಸು_skip = (ಪೂರ್ಣಾಙ್ಕ)inserted.length;
    ಯದಿ (inserted.string != ರಿಕ್ತ)
	vim_free(inserted.string);

    old_indent = 0;

    /*
     * Main loop in Insert mode: repeat until Insert mode is left.
     */
    ಪ್ರತಿ (;;)
    {
#ifdef FEAT_RIGHTLEFT
	ಯದಿ (!revins_legal)
	    revins_scol = -1;	    // reset on illegal motions
	ಅನ್ಯಥಾ
	    revins_legal = 0;
#endif
	ಯದಿ (arrow_used)	    // don't repeat insert when arrow key used
	    ಎಣಿಕೆ = 0;

	ಯದಿ (update_Insstart_orig)
	    Insstart_orig = Insstart;

	ಯದಿ (stop_ಸೇರಿಸು_mode && !ins_compl_active())
	{
	    // ":stopinsert" used or 'insertmode' reset
	    ಎಣಿಕೆ = 0;
	    ಗಚ್ಛ doESCkey;
	}

	// set curwin->w_curswant for next K_DOWN or K_UP
	ಯದಿ (!arrow_used)
	    curwin->w_set_curswant = true;

	// If there is no typeahead may check for timestamps (e.g., for when a
	// menu invoked a shell command).
	ಯದಿ (stuff_empty())
	{
	    did_check_timestamps = FALSE;
	    ಯದಿ (need_check_timestamps)
		check_timestamps(FALSE);
	}

	/*
	 * When emsg() was called msg_scroll will have been set.
	 */
	ಸಂದೇಶ_scroll = FALSE;

#ifdef FEAT_GUI
	// When 'mousefocus' is set a mouse movement may have taken us to
	// another window.  "need_mouse_correct" may then be set because of an
	// autocommand.
	ಯದಿ (need_mouse_correct)
	    gui_mouse_correct();
#endif

#ifdef FEAT_FOLDING
	// Open fold at the cursor line, according to 'foldopen'.
	ಯದಿ (fdo_flags & FDO_ಸೇರಿಸು)
	    foldOpenCursor();
	// Close folds where the cursor isn't, according to 'foldclose'
	ಯದಿ (!char_avail())
	    foldCheckClose();
#endif

#ifdef FEAT_JOB_CHANNEL
	ಯದಿ (bt_prompt(curbuf))
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
	ಯದಿ (curbuf->b_mod_set
		&& curwin->w_p_wrap
		&& !curwin->w_p_sms
		&& !did_backspace
		&& curwin->w_topline == old_topline
#ifdef FEAT_DIFF
		&& curwin->w_topfill == old_topfill
#endif
		&& ಎಣಿಕೆ <= 1
		)
	{
	    mincol = curwin->w_wcol;
	    validate_cursor_col();

	    ಯದಿ (
#ifdef FEAT_VARTABS
		curwin->w_wcol < mincol - tabstop_at(get_nolist_virtcol(),
						     curbuf->b_p_ts,
						     curbuf->b_p_vts_array,
						     FALSE)
#ಅನ್ಯಥಾ
		(ಪೂರ್ಣಾಙ್ಕ)curwin->w_wcol < mincol - curbuf->b_p_ts
#endif
		    && curwin->w_wrow ==
				   curwin->w_height - 1 - get_scrolloff_ಮೌಲ್ಯ()
		    && (curwin->w_cursor.lnum != curwin->w_topline
#ifdef FEAT_DIFF
			|| curwin->w_topfill > 0
#endif
		    ))
	    {
#ifdef FEAT_DIFF
		ಯದಿ (curwin->w_topfill > 0)
		    --curwin->w_topfill;
		ಅನ್ಯಥಾ
#endif
#ifdef FEAT_FOLDING
		ಯದಿ (hasFolding(curwin->w_topline, ರಿಕ್ತ, &old_topline))
		    set_topline(curwin, old_topline + 1);
		ಅನ್ಯಥಾ
#endif
		    set_topline(curwin, curwin->w_topline + 1);
	    }
	}

	// May need to adjust w_topline to show the cursor.
	ಯದಿ (ಎಣಿಕೆ <= 1)
	    update_topline();

	did_backspace = FALSE;

	ಯದಿ (ಎಣಿಕೆ <= 1)
	    validate_cursor();		// may set must_redraw

	/*
	 * Redraw the display when no characters are waiting.
	 * Also shows mode, ruler and positions cursor.
	 */
	ins_redraw(TRUE);

	ಯದಿ (curwin->w_p_scb)
	    do_check_scrollbind(TRUE);

	ಯದಿ (curwin->w_p_crb)
	    do_check_cursorbind();
	ಯದಿ (ಎಣಿಕೆ <= 1)
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
	ಯದಿ (c != K_CURSORHOLD && c != K_COMPLETE_DELAY)
	    lastc = c;		// remember the previous char for CTRL-D

	// After using CTRL-G U the next cursor key will not break undo.
	ಯದಿ (dont_sync_undo == MAYBE)
	    dont_sync_undo = TRUE;
	ಅನ್ಯಥಾ
	    dont_sync_undo = FALSE;
	ಯದಿ (cmdchar == K_PS)
	    // Got here from normal mode when bracketed paste started.
	    c = K_PS;
	ಅನ್ಯಥಾ
	{
	    // Trigger autocomplete when entering Insert mode, either directly
	    // or via change commands like 'ciw', 'cw', etc., before the first
	    // character is typed.
	    ಯದಿ (ins_just_started)
	    {
		ins_just_started = FALSE;
		ಯದಿ (ins_compl_has_autocomplete() && !char_avail()
			&& curwin->w_cursor.col > 0)
		{
		    c = char_before_cursor();
		    ಯದಿ (vim_isprintc(c))
		    {
			ins_compl_enable_autocomplete();
			ins_compl_arm_autostart();
			ins_compl_init_get_longest();
#ifdef FEAT_RIGHTLEFT
			ಯದಿ (p_hkmap)
			    c = hkmap(c);		// Hebrew mode mapping
#endif
			// Defer until the delay expires (K_COMPLETE_DELAY), or
			// trigger now when no delay is in effect.
			ಯದಿ (!ins_compl_arm_autocomplete_delay())
			    ಗಚ್ಛ docomplete;
		    }
		}
	    }

	    ಕುರು
	    {
		c = safe_vgetc();

		ಯದಿ (stop_ಸೇರಿಸು_mode
#ifdef FEAT_TERMINAL
			|| (c == K_IGNORE && term_use_loop())
#endif
		   )
		{
		    // Insert mode ended, possibly from a callback, or a timer
		    // must have opened a terminal window.
		    ಯದಿ (c != K_IGNORE && c != K_NOP)
			vungetc(c);
		    ಎಣಿಕೆ = 0;

		    ಯದಿ (!bt_prompt(curwin->w_buffer)
#ifdef FEAT_TERMINAL
			    && !bt_terminal(curwin->w_buffer)
#endif
			    && stop_ಸೇರಿಸು_mode)
			// :stopinsert command via callback or via server command
			nomove = FALSE;
		    ಅನ್ಯಥಾ
			nomove = TRUE;
		    ins_compl_prep(ESC);
		    ಗಚ್ಛ doESCkey;
		}
	    } ಯಾವತ್ (c == K_IGNORE || c == K_NOP);
	}

	// Don't want K_CURSORHOLD for the second key, e.g., after CTRL-V.
	did_cursorhold = TRUE;
	ಯದಿ (c != K_CURSORHOLD && c != K_COMPLETE_DELAY)
	{
	    // Don't want delayed autocompletion from the previous key either.
	    ins_compl_clear_autocomplete_delay();
	    ins_compl_disarm_autostart();
	    // A completion already on screen goes on being what it was.
	    ಯದಿ (!ins_compl_active())
		ins_compl_disable_autocomplete();
	}

#ifdef FEAT_RIGHTLEFT
	ಯದಿ (p_hkmap && ಕೀಲಿ_Typed)
	    c = hkmap(c);		// Hebrew mode mapping
#endif

	// If the window was made so small that nothing shows, make it at least
	// one line and one column when typing.
	ಯದಿ (ಕೀಲಿ_Typed && !ಕೀಲಿ_Stuffed)
	    win_ensure_size();

	/*
	 * Special handling of keys while the popup menu is visible or wanted
	 * and the cursor is still in the completed word.  Only when there is
	 * a match, skip this when no matches were found.
	 */
	ಯದಿ (ins_compl_active() && curwin->w_cursor.col >= ins_compl_col()
		&& ins_compl_has_shown_match() && pum_wanted())
	{
	    // BS: Delete one character from "compl_leader".
	    ಯದಿ ((c == K_BS || c == Ctrl_H)
			&& curwin->w_cursor.col > ins_compl_col()
			&& (c = ins_compl_bs()) == NUL)
		ಅನುವರ್ತ;

	    // When no match was selected or it was edited.
	    ಯದಿ (!ins_compl_used_match())
	    {
		// CTRL-L: Add one character from the current match to
		// "compl_leader".  Except when at the original match and
		// there is nothing to add, CTRL-L works like CTRL-P then.
		ಯದಿ (c == Ctrl_L
			&& (!ctrl_x_mode_ಸಾಲು_or_eval()
			    || ins_compl_long_shown_match()))
		{
		    ins_compl_addfrommatch();
		    ಅನುವರ್ತ;
		}

		// A non-white character that fits in with the current
		// completion: Add to "compl_leader".
		ಯದಿ (ins_compl_accept_char(c))
		{
#ಯದಿ defined(FEAT_EVAL)
		    // Trigger InsertCharPre.
		    char_u *str = do_ಸೇರಿಸು_char_pre(c);
		    char_u *p;

		    ಯದಿ (str != ರಿಕ್ತ)
		    {
			ಪ್ರತಿ (p = str; *p != NUL; MB_PTR_ADV(p))
			    ins_compl_addleader(PTR2CHAR(p));
			vim_free(str);
		    }
		    ಅನ್ಯಥಾ
#endif
			ins_compl_addleader(c);
		    ಅನುವರ್ತ;
		}

		// Pressing CTRL-Y selects the current match.  When
		// ins_compl_enter_selects() is set the Enter key does the
		// same.
		ಯದಿ ((c == Ctrl_Y || (ins_compl_enter_selects()
				    && (c == CAR || c == K_KENTER || c == NL)))
			&& stop_arrow() == OK)
		{
		    ins_compl_delete();
		    ಯದಿ (ins_compl_preinsert_longest()
			    && !ins_compl_is_match_selected())
		    {
			ins_compl_ಸೇರಿಸು(FALSE, TRUE);
			ins_compl_init_get_longest();
			ಅನುವರ್ತ;
		    }
		    ಅನ್ಯಥಾ
			ins_compl_ಸೇರಿಸು(FALSE, FALSE);
		}
		// Delete preinserted text when typing special chars
		ಅನ್ಯಥಾ ಯದಿ (IS_WHITE_NL_OR_NUL(c) && ins_compl_preinsert_effect())
		    ins_compl_delete();
	    }
	}

	// Prepare for or stop CTRL-X mode.  This doesn't do completion, but
	// it does fix up the text when finishing completion.
	ins_compl_init_get_longest();
	ಯದಿ (ins_compl_prep(c))
	    ಅನುವರ್ತ;

	// CTRL-\ CTRL-N goes to Normal mode,
	// CTRL-\ CTRL-G goes to mode selected with 'insertmode',
	// CTRL-\ CTRL-O is like CTRL-O but without moving the cursor.
	ಯದಿ (c == Ctrl_BSL)
	{
	    // may need to redraw when no more chars available now
	    ins_redraw(FALSE);
	    ++no_mapping;
	    ++allow_ಕೀಲಿಗಳು;
	    c = plain_vgetc();
	    --no_mapping;
	    --allow_ಕೀಲಿಗಳು;
	    ಯದಿ (c != Ctrl_N && c != Ctrl_G && c != Ctrl_O)
	    {
		// it's something else
		vungetc(c);
		c = Ctrl_BSL;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (c == Ctrl_G && p_im)
		ಅನುವರ್ತ;
	    ಅನ್ಯಥಾ
	    {
		ಯದಿ (c == Ctrl_O)
		{
		    ins_ctrl_o();
		    ins_at_eol = FALSE;	// cursor keeps its column
		    nomove = TRUE;
		}
		ಎಣಿಕೆ = 0;
		ಗಚ್ಛ doESCkey;
	    }
	}

#ifdef FEAT_DIGRAPHS
	c = do_digraph(c);
#endif

	ಯದಿ ((c == Ctrl_V || c == Ctrl_Q) && ctrl_x_mode_cmdline())
	    ಗಚ್ಛ docomplete;
	ಯದಿ (c == Ctrl_V || c == Ctrl_Q)
	{
	    ins_ctrl_v();
	    c = Ctrl_V;	// pretend CTRL-V is last typed character
	    ಅನುವರ್ತ;
	}

	ಯದಿ (cindent_on() && ctrl_x_mode_none())
	{
	    // A key name preceded by a bang means this key is not to be
	    // inserted.  Skip ahead to the re-indenting below.
	    // A key name preceded by a star means that indenting has to be
	    // done before inserting the key.
	    ಸಾಲು_is_white = inindent(0);
	    ಯದಿ (in_cinkeys(c, '!', ಸಾಲು_is_white))
		ಗಚ್ಛ force_cindent;
	    ಯದಿ (can_cindent && in_cinkeys(c, '*', ಸಾಲು_is_white)
							&& stop_arrow() == OK)
		do_c_ಅಭಿವ್ಯಕ್ತಿ_indent();
	}

#ifdef FEAT_RIGHTLEFT
	ಯದಿ (curwin->w_p_rl)
	    ವಿಕಲ್ಪ (c)
	    {
		ಪಕ್ಷ K_ಎಡ:	c = K_ಬಲ; ಭಙ್ಗ;
		ಪಕ್ಷ K_S_ಎಡ:	c = K_S_ಬಲ; ಭಙ್ಗ;
		ಪಕ್ಷ K_C_ಎಡ:	c = K_C_ಬಲ; ಭಙ್ಗ;
		ಪಕ್ಷ K_ಬಲ:	c = K_ಎಡ; ಭಙ್ಗ;
		ಪಕ್ಷ K_S_ಬಲ: c = K_S_ಎಡ; ಭಙ್ಗ;
		ಪಕ್ಷ K_C_ಬಲ: c = K_C_ಎಡ; ಭಙ್ಗ;
	    }
#endif

	/*
	 * If 'keymodel' contains "startsel", may start selection.  If it
	 * does, a CTRL-O and c will be stuffed, we need to get these
	 * characters.
	 */
	ಯದಿ (ins_start_select(c))
	    ಅನುವರ್ತ;

	/*
	 * The big switch to handle a character in insert mode.
	 */
	ವಿಕಲ್ಪ (c)
	{
	ಪಕ್ಷ ESC:	// End input mode
	    ಯದಿ (echeck_abbr(ESC + ABBR_OFF))
		ಭಙ್ಗ;
	    // FALLTHROUGH

	ಪಕ್ಷ Ctrl_C:	// End input mode
	    ಯದಿ (c == Ctrl_C && cmdwin_type != 0)
	    {
		// Close the cmdline window.
		cmdwin_result = K_IGNORE;
		got_int = FALSE; // don't stop executing autocommands et al.
		nomove = TRUE;
		ಗಚ್ಛ doESCkey;
	    }
#ifdef FEAT_JOB_CHANNEL
	    ಯದಿ (c == Ctrl_C && bt_prompt(curbuf))
	    {
		ಯದಿ (invoke_prompt_interrupt())
		{
		    ಯದಿ (!bt_prompt(curbuf))
			// buffer changed to a non-prompt buffer, get out of
			// Insert mode
			ಗಚ್ಛ doESCkey;
		    ಭಙ್ಗ;
		}
	    }
#endif

#ifdef UNIX
do_intr:
#endif
	    // when 'insertmode' set, and not halfway a mapping, don't leave
	    // Insert mode
	    ಯದಿ (goto_im())
	    {
		ಯದಿ (got_int)
		{
		    (ಶೂನ್ಯ)vgetc();		// flush all buffers
		    got_int = FALSE;
		}
		ಅನ್ಯಥಾ
		    vim_beep(BO_IM);
		ಭಙ್ಗ;
	    }
doESCkey:
	    // Drop a pending autocomplete so it does not outlive Insert mode.
	    ins_compl_clear_autocomplete_delay();
	    /*
	     * This is the ONLY return from edit()!
	     */
	    // Always update o_lnum, so that a "CTRL-O ." that adds a line
	    // still puts the cursor back after the inserted text.
	    ಯದಿ (ins_at_eol && gchar_cursor() == NUL)
		o_lnum = curwin->w_cursor.lnum;

	    ಯದಿ (ins_esc(&ಎಣಿಕೆ, cmdchar, nomove))
	    {
		// When CTRL-C was typed got_int will be set, with the result
		// that the autocommands won't be executed. When mapped got_int
		// is not set, but let's keep the behavior the same.
		ಯದಿ (cmdchar != 'r' && cmdchar != 'v' && c != Ctrl_C)
		    ins_apply_autocmds(EVENT_INSERTLEAVE);
		did_cursorhold = FALSE;

		// ins_redraw() triggers TextChangedI only when no characters
		// are in the typeahead buffer, so reset curbuf->b_last_changedtick only
		// if the TextChangedI was not blocked by char_avail() (e.g. using :norm!)
		// and the TextChangedI autocommand has been triggered.
		ಯದಿ (!char_avail() && curbuf->b_last_changedtick_i == CHANGEDTICK(curbuf))
		    curbuf->b_last_changedtick = CHANGEDTICK(curbuf);
		ಪ್ರತ್ಯಾಗಮ (c == Ctrl_O);
	    }
	    ಅನುವರ್ತ;

	ಪಕ್ಷ Ctrl_Z:	// suspend when 'insertmode' set
	    ಯದಿ (!p_im)
		ಗಚ್ಛ normalchar;	// insert CTRL-Z as normal char
	    do_cmdline_cmd((char_u *)"stop");
#ifdef CURSOR_SHAPE
	    ui_cursor_shape();		// may need to update cursor shape
#endif
	    ಅನುವರ್ತ;

	ಪಕ್ಷ Ctrl_O:	// execute one command
#ifdef FEAT_COMPL_FUNC
	    ಯದಿ (ctrl_x_mode_omni())
		ಗಚ್ಛ docomplete;
#endif
	    ಯದಿ (echeck_abbr(Ctrl_O + ABBR_OFF))
		ಭಙ್ಗ;
	    ins_ctrl_o();

	    // don't move the cursor left when 'virtualedit' has "onemore".
	    ಯದಿ (get_ve_flags() & VE_ONEMORE)
	    {
		ins_at_eol = FALSE;
		nomove = TRUE;
	    }
	    ಎಣಿಕೆ = 0;
	    ಗಚ್ಛ doESCkey;

	ಪಕ್ಷ K_INS:	// toggle insert/replace mode
	ಪಕ್ಷ K_KINS:
	    ins_ಸೇರಿಸು(replaceState);
	    ಭಙ್ಗ;

	ಪಕ್ಷ K_SELECT:	// end of Select mode mapping - ignore
	    ಭಙ್ಗ;

	ಪಕ್ಷ K_HELP:	// Help key works like <ESC> <Help>
	ಪಕ್ಷ K_F1:
	ಪಕ್ಷ K_XF1:
	    stuffcharReadbuff(K_HELP);
	    ಯದಿ (p_im)
		need_start_insertmode = TRUE;
	    ಗಚ್ಛ doESCkey;

#ifdef FEAT_NETBEANS_INTG
	ಪಕ್ಷ K_F21:	// NetBeans command
	    ++no_mapping;		// don't map the next key hits
	    i = plain_vgetc();
	    --no_mapping;
	    netbeans_keycommand(i);
	    ಭಙ್ಗ;
#endif

	ಪಕ್ಷ K_ZERO:	// Insert the previously inserted text.
	ಪಕ್ಷ NUL:
	ಪಕ್ಷ Ctrl_A:
	    // For ^@ the trailing ESC will end the insert, unless there is an
	    // error.
	    ಯದಿ (stuff_inserted(NUL, 1L, (c == Ctrl_A)) == FAIL
						   && c != Ctrl_A && !p_im)
		ಗಚ್ಛ doESCkey;		// quit insert mode
	    inserted_space = FALSE;
	    ಭಙ್ಗ;

	ಪಕ್ಷ Ctrl_R:	// insert the contents of a register
	    ಯದಿ (ctrl_x_mode_register() && !ins_compl_active())
		ಗಚ್ಛ docomplete;
	    ins_reg();
	    auto_format(FALSE, TRUE);
	    inserted_space = FALSE;
	    ಭಙ್ಗ;

	ಪಕ್ಷ Ctrl_G:	// commands starting with CTRL-G
	    ins_ctrl_g();
	    ಭಙ್ಗ;

	ಪಕ್ಷ Ctrl_HAT:	// switch input mode and/or langmap
	    ins_ctrl_hat();
	    ಭಙ್ಗ;

#ifdef FEAT_RIGHTLEFT
	ಪಕ್ಷ Ctrl__:	// switch between languages
	    ಯದಿ (!p_ari)
		ಗಚ್ಛ normalchar;
	    ins_ctrl_();
	    ಭಙ್ಗ;
#endif

	ಪಕ್ಷ Ctrl_D:	// Make indent one shiftwidth smaller.
#ಯದಿ defined(FEAT_FIND_ID)
	    ಯದಿ (ctrl_x_mode_path_defines())
		ಗಚ್ಛ docomplete;
#endif
	    // FALLTHROUGH

	ಪಕ್ಷ Ctrl_T:	// Make indent one shiftwidth greater.
	    ಯದಿ (c == Ctrl_T && ctrl_x_mode_thesaurus())
	    {
		ಯದಿ (has_compl_option(FALSE))
		    ಗಚ್ಛ docomplete;
		ಭಙ್ಗ;
	    }

	    ins_shift(c, lastc);
	    auto_format(FALSE, TRUE);
	    inserted_space = FALSE;
	    ಭಙ್ಗ;

	ಪಕ್ಷ K_DEL:	// delete character under the cursor
	ಪಕ್ಷ K_KDEL:
	    ins_del();
	    auto_format(FALSE, TRUE);
	    ಭಙ್ಗ;

	ಪಕ್ಷ K_BS:	// delete character before the cursor
	ಪಕ್ಷ K_S_BS:
	ಪಕ್ಷ Ctrl_H:
	    did_backspace = ins_bs(c, BACKSPACE_CHAR, &inserted_space);
	    auto_format(FALSE, TRUE);
	    ಯದಿ (did_backspace)
		MAY_TRIGGER_AUTOCOMPLETE(c);
	    ಭಙ್ಗ;

	ಪಕ್ಷ Ctrl_W:	// delete word before the cursor
#ifdef FEAT_JOB_CHANNEL
	    ಯದಿ (bt_prompt(curbuf) && (mod_mask & MOD_MASK_SHIFT) == 0)
	    {
		// In a prompt window CTRL-W is used for window commands.
		// Use Shift-CTRL-W to delete a word.
		stuffcharReadbuff(Ctrl_W);
		restart_edit = 'A';
		nomove = TRUE;
		ಎಣಿಕೆ = 0;
		ಗಚ್ಛ doESCkey;
	    }
#endif
	    did_backspace = ins_bs(c, BACKSPACE_WORD, &inserted_space);
	    auto_format(FALSE, TRUE);
	    ಯದಿ (did_backspace)
		MAY_TRIGGER_AUTOCOMPLETE(c);
	    ಭಙ್ಗ;

	ಪಕ್ಷ Ctrl_U:	// delete all inserted text in current line
#ifdef FEAT_COMPL_FUNC
	    // CTRL-X CTRL-U completes with 'completefunc'.
	    ಯದಿ (ctrl_x_mode_function())
		ಗಚ್ಛ docomplete;
#endif
	    did_backspace = ins_bs(c, BACKSPACE_ಸಾಲು, &inserted_space);
	    auto_format(FALSE, TRUE);
	    inserted_space = FALSE;
	    ಯದಿ (did_backspace)
		MAY_TRIGGER_AUTOCOMPLETE(c);
	    ಭಙ್ಗ;

	ಪಕ್ಷ K_LEFTMOUSE:   // mouse keys
	ಪಕ್ಷ K_LEFTMOUSE_NM:
	ಪಕ್ಷ K_LEFTDRAG:
	ಪಕ್ಷ K_LEFTRELEASE:
	ಪಕ್ಷ K_LEFTRELEASE_NM:
	ಪಕ್ಷ K_MOUSEMOVE:
	ಪಕ್ಷ K_MIDDLEMOUSE:
	ಪಕ್ಷ K_MIDDLEDRAG:
	ಪಕ್ಷ K_MIDDLERELEASE:
	ಪಕ್ಷ K_RIGHTMOUSE:
	ಪಕ್ಷ K_RIGHTDRAG:
	ಪಕ್ಷ K_RIGHTRELEASE:
	ಪಕ್ಷ K_X1MOUSE:
	ಪಕ್ಷ K_X1DRAG:
	ಪಕ್ಷ K_X1RELEASE:
	ಪಕ್ಷ K_X2MOUSE:
	ಪಕ್ಷ K_X2DRAG:
	ಪಕ್ಷ K_X2RELEASE:
	    ins_mouse(c);
	    ಭಙ್ಗ;

	ಪಕ್ಷ K_MOUSEDOWN: // Default action for scroll wheel up: scroll up
	    ins_mousescroll(MSCR_DOWN);
	    ಭಙ್ಗ;

	ಪಕ್ಷ K_MOUSEUP:	// Default action for scroll wheel down: scroll down
	    ins_mousescroll(MSCR_UP);
	    ಭಙ್ಗ;

	ಪಕ್ಷ K_MOUSELEFT: // Scroll wheel left
	    ins_mousescroll(MSCR_ಎಡ);
	    ಭಙ್ಗ;

	ಪಕ್ಷ K_MOUSERIGHT: // Scroll wheel right
	    ins_mousescroll(MSCR_ಬಲ);
	    ಭಙ್ಗ;

	ಪಕ್ಷ K_PS:
	    bracketed_paste(PASTE_ಸೇರಿಸು, FALSE, ರಿಕ್ತ);
	    ಯದಿ (cmdchar == K_PS)
		// invoked from normal mode, bail out
		ಗಚ್ಛ doESCkey;
	    ಭಙ್ಗ;
	ಪಕ್ಷ K_PE:
	    // Got K_PE without K_PS, ignore.
	    ಭಙ್ಗ;

#ifdef FEAT_GUI_TABLINE
	ಪಕ್ಷ K_TABLINE:
	ಪಕ್ಷ K_TABMENU:
	    ins_tabline(c);
	    ಭಙ್ಗ;
#endif

	ಪಕ್ಷ K_IGNORE:	// Something mapped to nothing
	    ಭಙ್ಗ;

	ಪಕ್ಷ K_COMMAND:		    // <Cmd>command<CR>
	ಪಕ್ಷ K_SCRIPT_COMMAND:	    // <ScriptCmd>command<CR>
	    {
		bufref_T    save_curbuf;
		varnumber_T tick = CHANGEDTICK(curbuf);

		set_bufref(&save_curbuf, curbuf);
		do_cmdkey_command(c, 0);

#ifdef FEAT_TERMINAL
		ಯದಿ (term_use_loop())
		    // Started a terminal that gets the input, exit Insert mode.
		    ಗಚ್ಛ doESCkey;
#endif
		ಯದಿ (curbuf->b_u_synced
			|| (bufref_valid(&save_curbuf)
			    && curbuf == save_curbuf.br_buf
			    && tick != CHANGEDTICK(curbuf)))
		{
		    // The command synced undo or changed this buffer.
		    // Save the cursor line before the next typed edit.
		    ins_need_undo = TRUE;
		}
	    }
	    ಭಙ್ಗ;

	ಪಕ್ಷ K_CURSORHOLD:	// Didn't type something for a while.
	    ins_apply_autocmds(EVENT_CURSORHOLDI);
	    did_cursorhold = TRUE;
	    // If CTRL-G U was used apply it to the next typed key.
	    ಯದಿ (dont_sync_undo == TRUE)
		dont_sync_undo = MAYBE;
	    ಭಙ್ಗ;

	ಪಕ್ಷ K_COMPLETE_DELAY:	// 'autocompletedelay' expired
	    // If CTRL-G U was used apply it to the next typed key.
	    ಯದಿ (dont_sync_undo == TRUE)
		dont_sync_undo = MAYBE;
	    ins_compl_clear_autocomplete_delay();
	    ಯದಿ (!ins_compl_has_autocomplete() || char_avail()
		    || curwin->w_cursor.col == 0)
		ಭಙ್ಗ;
	    c = char_before_cursor();
	    ಯದಿ (!vim_isprintc(c))
		ಭಙ್ಗ;
	    // The completion may have been cleared while waiting, so re-enable
	    // autocomplete to match a zero delay.
	    ins_compl_enable_autocomplete();
	    ins_compl_arm_autostart();
	    ಗಚ್ಛ docomplete;

#ifdef FEAT_GUI_MSWIN
	    // On MS-Windows ignore <M-F4>, we get it when closing the window
	    // was cancelled.
	ಪಕ್ಷ K_F4:
	    ಯದಿ (mod_mask != MOD_MASK_ALT)
		ಗಚ್ಛ normalchar;
	    ಭಙ್ಗ;
#endif

#ifdef FEAT_GUI
	ಪಕ್ಷ K_VER_SCROLLBAR:
	    ins_scroll();
	    ಭಙ್ಗ;

	ಪಕ್ಷ K_HOR_SCROLLBAR:
	    ins_horscroll();
	    ಭಙ್ಗ;
#endif

	ಪಕ್ಷ K_HOME:	// <Home>
	ಪಕ್ಷ K_KHOME:
	ಪಕ್ಷ K_S_HOME:
	ಪಕ್ಷ K_C_HOME:
	    ins_home(c);
	    ಭಙ್ಗ;

	ಪಕ್ಷ K_END:	// <End>
	ಪಕ್ಷ K_KEND:
	ಪಕ್ಷ K_S_END:
	ಪಕ್ಷ K_C_END:
	    ins_end(c);
	    ಭಙ್ಗ;

	ಪಕ್ಷ K_ಎಡ:	// <Left>
	    ಯದಿ (mod_mask & (MOD_MASK_SHIFT|MOD_MASK_CTRL))
		ins_s_ಎಡ();
	    ಅನ್ಯಥಾ
		ins_ಎಡ();
	    ಭಙ್ಗ;

	ಪಕ್ಷ K_S_ಎಡ:	// <S-Left>
	ಪಕ್ಷ K_C_ಎಡ:
	    ins_s_ಎಡ();
	    ಭಙ್ಗ;

	ಪಕ್ಷ K_ಬಲ:	// <Right>
	    ಯದಿ (mod_mask & (MOD_MASK_SHIFT|MOD_MASK_CTRL))
		ins_s_ಬಲ();
	    ಅನ್ಯಥಾ
		ins_ಬಲ();
	    ಭಙ್ಗ;

	ಪಕ್ಷ K_S_ಬಲ:	// <S-Right>
	ಪಕ್ಷ K_C_ಬಲ:
	    ins_s_ಬಲ();
	    ಭಙ್ಗ;

	ಪಕ್ಷ K_UP:	// <Up>
	    ಯದಿ (pum_visible())
		ಗಚ್ಛ docomplete;
	    ಯದಿ (mod_mask & MOD_MASK_SHIFT)
		ins_pageup();
	    ಅನ್ಯಥಾ
		ins_up(FALSE);
	    ಭಙ್ಗ;

	ಪಕ್ಷ K_S_UP:	// <S-Up>
	ಪಕ್ಷ K_PAGEUP:
	ಪಕ್ಷ K_KPAGEUP:
	    ಯದಿ (pum_visible())
	    {
#ifdef FEAT_PROP_POPUP
		// CTRL-SHIFT-<Up> scrolls the info popup up a line,
		// CTRL-SHIFT-<PageUp> a page.  Shift is folded into K_S_UP but
		// stays in mod_mask for PageUp, hence the asymmetric check.
		ಯದಿ (c == K_S_UP ? (mod_mask & MOD_MASK_CTRL)
			: ((mod_mask & MOD_MASK_CTRL)
			    && (mod_mask & MOD_MASK_SHIFT)))
		{
		    popup_scroll_info(-1, c != K_S_UP);
		    ಭಙ್ಗ;
		}
#endif
		ಗಚ್ಛ docomplete;
	    }
	    ins_pageup();
	    ಭಙ್ಗ;

	ಪಕ್ಷ K_DOWN:	// <Down>
	    ಯದಿ (pum_visible())
		ಗಚ್ಛ docomplete;
	    ಯದಿ (mod_mask & MOD_MASK_SHIFT)
		ins_pagedown();
	    ಅನ್ಯಥಾ
		ins_down(FALSE);
	    ಭಙ್ಗ;

	ಪಕ್ಷ K_S_DOWN:	// <S-Down>
	ಪಕ್ಷ K_PAGEDOWN:
	ಪಕ್ಷ K_KPAGEDOWN:
	    ಯದಿ (pum_visible())
	    {
#ifdef FEAT_PROP_POPUP
		// CTRL-SHIFT-<Down>/<PageDown> scroll the info popup down.
		ಯದಿ (c == K_S_DOWN ? (mod_mask & MOD_MASK_CTRL)
			: ((mod_mask & MOD_MASK_CTRL)
			    && (mod_mask & MOD_MASK_SHIFT)))
		{
		    popup_scroll_info(1, c != K_S_DOWN);
		    ಭಙ್ಗ;
		}
#endif
		ಗಚ್ಛ docomplete;
	    }
	    ins_pagedown();
	    ಭಙ್ಗ;

#ifdef FEAT_DND
	ಪಕ್ಷ K_DROP:	// drag-n-drop event
	    ins_drop();
	    ಭಙ್ಗ;
#endif

	ಪಕ್ಷ K_S_TAB:	// When not mapped, use like a normal TAB
	    c = TAB;
	    // FALLTHROUGH

	ಪಕ್ಷ TAB:	// TAB or Complete patterns along path
#ಯದಿ defined(FEAT_FIND_ID)
	    ಯದಿ (ctrl_x_mode_path_patterns())
		ಗಚ್ಛ docomplete;
#endif
	    inserted_space = FALSE;
	    ಯದಿ (ins_tab())
		ಗಚ್ಛ normalchar;	// insert TAB as a normal char
	    auto_format(FALSE, TRUE);
	    ಭಙ್ಗ;

	ಪಕ್ಷ K_KENTER:	// <Enter>
	    c = CAR;
	    // FALLTHROUGH
	ಪಕ್ಷ CAR:
	ಪಕ್ಷ NL:
#ಯದಿ defined(FEAT_QUICKFIX)
	    // In a quickfix window a <CR> jumps to the error under the
	    // cursor.
	    ಯದಿ (bt_quickfix(curbuf) && c == CAR)
	    {
		ಯದಿ (curwin->w_llist_ref == ರಿಕ್ತ)    // quickfix window
		    do_cmdline_cmd((char_u *)".cc");
		ಅನ್ಯಥಾ				    // location list window
		    do_cmdline_cmd((char_u *)".ll");
		ಭಙ್ಗ;
	    }
#endif
	    ಯದಿ (cmdwin_type != 0)
	    {
		// Execute the command in the cmdline window.
		cmdwin_result = CAR;
		ಗಚ್ಛ doESCkey;
	    }
#ifdef FEAT_JOB_CHANNEL
	    ಯದಿ (bt_prompt(curbuf))
	    {
		invoke_prompt_callback();
		ಯದಿ (!bt_prompt(curbuf))
		    // buffer changed to a non-prompt buffer, get out of
		    // Insert mode
		    ಗಚ್ಛ doESCkey;
		ಭಙ್ಗ;
	    }
#endif
	    ಯದಿ (ins_eol(c) == FAIL && !p_im)
		ಗಚ್ಛ doESCkey;	    // out of memory
	    auto_format(FALSE, FALSE);
	    inserted_space = FALSE;
	    ಭಙ್ಗ;

	ಪಕ್ಷ Ctrl_K:	    // digraph or keyword completion
	    ಯದಿ (ctrl_x_mode_dictionary())
	    {
		ಯದಿ (has_compl_option(TRUE))
		    ಗಚ್ಛ docomplete;
		ಭಙ್ಗ;
	    }
#ifdef FEAT_DIGRAPHS
	    c = ins_digraph();
	    ಯದಿ (c == NUL)
		ಭಙ್ಗ;
#endif
	    ಗಚ್ಛ normalchar;

	ಪಕ್ಷ Ctrl_X:	// Enter CTRL-X mode
	    ins_ctrl_x();
	    ಭಙ್ಗ;

	ಪಕ್ಷ Ctrl_RSB:	// Tag name completion after ^X
	    ಯದಿ (!ctrl_x_mode_tags())
		ಗಚ್ಛ normalchar;
	    ಗಚ್ಛ docomplete;

	ಪಕ್ಷ Ctrl_F:	// File name completion after ^X
	    ಯದಿ (!ctrl_x_mode_files())
		ಗಚ್ಛ normalchar;
	    ಗಚ್ಛ docomplete;

	ಪಕ್ಷ 's':	// Spelling completion after ^X
	ಪಕ್ಷ Ctrl_S:
	    ಯದಿ (!ctrl_x_mode_spell())
		ಗಚ್ಛ normalchar;
	    ಗಚ್ಛ docomplete;

	ಪಕ್ಷ Ctrl_L:	// Whole line completion after ^X
	    ಯದಿ (!ctrl_x_mode_whole_ಸಾಲು())
	    {
		// CTRL-L with 'insertmode' set: Leave Insert mode
		ಯದಿ (p_im)
		{
		    ಯದಿ (echeck_abbr(Ctrl_L + ABBR_OFF))
			ಭಙ್ಗ;
		    ಗಚ್ಛ doESCkey;
		}
		ಗಚ್ಛ normalchar;
	    }
	    // FALLTHROUGH

	ಪಕ್ಷ Ctrl_P:	// Do previous/next pattern completion
	ಪಕ್ಷ Ctrl_N:
#ifdef FEAT_PROP_POPUP
	    // CTRL-SHIFT-P/N scroll the info popup one line.
	    ಯದಿ (pum_visible() && (mod_mask & MOD_MASK_SHIFT)
		    && (c == Ctrl_P || c == Ctrl_N))
	    {
		popup_scroll_info(c == Ctrl_P ? -1 : 1, false);
		ಭಙ್ಗ;
	    }
#endif
	    // if 'complete' is empty then plain ^P is no longer special,
	    // but it is under other ^X modes
	    ಯದಿ (*curbuf->b_p_cpt == NUL
		    && (ctrl_x_mode_normal() || ctrl_x_mode_whole_ಸಾಲು())
		    && !compl_status_local())
		ಗಚ್ಛ normalchar;

docomplete:
	    ins_compl_clear_autocomplete_delay();
	    compl_busy = TRUE;
#ifdef FEAT_FOLDING
	    disable_fold_update++;  // don't redraw folds here
#endif
	    ಯದಿ (ins_complete(c, TRUE) == FAIL)
		compl_status_clear();
#ifdef FEAT_FOLDING
	    disable_fold_update--;
#endif
	    compl_busy = FALSE;
	    can_si = may_do_si(); // allow smartindenting
	    ಭಙ್ಗ;

	ಪಕ್ಷ Ctrl_Y:	// copy from previous line or scroll down
	ಪಕ್ಷ Ctrl_E:	// copy from next line	   or scroll up
	    c = ins_ctrl_ey(c);
	    ಭಙ್ಗ;

	ಸ್ವಭಾವ:
#ifdef UNIX
	    ಯದಿ (c == intr_char)		// special interrupt char
		ಗಚ್ಛ do_intr;
#endif

normalchar:
	    /*
	     * Insert a normal character.
	     */
#ಯದಿ defined(FEAT_EVAL)
	    ಯದಿ (!p_paste)
	    {
		// Trigger InsertCharPre.
		char_u *str = do_ಸೇರಿಸು_char_pre(c);
		char_u *p;

		ಯದಿ (str != ರಿಕ್ತ)
		{
		    ಯದಿ (*str != NUL && stop_arrow() != FAIL)
		    {
			// Insert the new value of v:char literally.
			ಪ್ರತಿ (p = str; *p != NUL; MB_PTR_ADV(p))
			{
			    c = PTR2CHAR(p);
			    ಯದಿ (c == CAR || c == K_KENTER || c == NL)
				ins_eol(c);
			    ಅನ್ಯಥಾ
				ins_char(c);
			}
			AppendToRedobuffLit(str, -1);
		    }
		    vim_free(str);
		    c = NUL;
		}

		// If the new value is already inserted or an empty string
		// then don't insert any character.
		ಯದಿ (c == NUL)
		    ಭಙ್ಗ;
	    }
#endif
	    // Try to perform smart-indenting.
	    ins_try_si(c);

	    ಯದಿ (c == ' ')
	    {
		inserted_space = TRUE;
		ಯದಿ (inindent(0))
		    can_cindent = FALSE;
		ಯದಿ (Insstart_blank_vcol == MAXCOL
			&& curwin->w_cursor.lnum == Insstart.lnum)
		    Insstart_blank_vcol = get_nolist_virtcol();
	    }

	    // Insert a normal character and check for abbreviations on a
	    // special character.  Let CTRL-] expand abbreviations without
	    // inserting it.
	    ಯದಿ (vim_iswordc(c) || (!echeck_abbr(
			// Add ABBR_OFF for characters above 0x100, this is
			// what check_abbr() expects.
				(has_mbyte && c >= 0x100) ? (c + ABBR_OFF) : c)
			&& c != Ctrl_RSB))
	    {
		ಸೇರಿಸು_special(c, FALSE, FALSE);
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
	    ಯದಿ (ins_compl_has_autocomplete() && !char_avail()
		    && vim_isprintc(c))
		TRIGGER_AUTOCOMPLETE();

	    ಭಙ್ಗ;
	}   // end of switch (c)

	// If typed something may trigger CursorHoldI again; K_COMPLETE_DELAY is
	// injected, not typed.
	ಯದಿ (c != K_CURSORHOLD && c != K_COMPLETE_DELAY
#ifdef FEAT_COMPL_FUNC
		// but not in CTRL-X mode, a script can't restore the state
		&& ctrl_x_mode_normal()
#endif
	       )
	    did_cursorhold = FALSE;

	// Check if we need to cancel completion mode because the window
	// or tab page was changed
	ಯದಿ (ins_compl_active() && !ins_compl_win_active(curwin))
	    ins_compl_cancel();

	// If the cursor was moved we didn't just insert a space
	ಯದಿ (arrow_used)
	    inserted_space = FALSE;

	ಯದಿ (can_cindent && cindent_on() && ctrl_x_mode_normal())
	{
force_cindent:
	    /*
	     * Indent now if a key was typed that is in 'cinkeys'.
	     */
	    ಯದಿ (in_cinkeys(c, ' ', ಸಾಲು_is_white))
	    {
		ಯದಿ (stop_arrow() == OK)
		    // re-indent the current line
		    do_c_ಅಭಿವ್ಯಕ್ತಿ_indent();
	    }
	}

    }	// for (;;)
    // NOTREACHED
}

    ಪೂರ್ಣಾಙ್ಕ
ins_need_undo_get(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ ins_need_undo;
}

/*
 * Redraw for Insert mode.
 * This is postponed until getting the next character to make '$' in the 'cpo'
 * option work correctly.
 * Only redraw when there are no characters available.  This speeds up
 * inserting sequences of characters (e.g., for CTRL-R).
 */
    ಶೂನ್ಯ
ins_redraw(ಪೂರ್ಣಾಙ್ಕ ready)	    // not busy with something
{
#ifdef FEAT_CONCEAL
    linenr_T	conceal_old_cursor_ಸಾಲು = 0;
    linenr_T	conceal_new_cursor_ಸಾಲು = 0;
    ಪೂರ್ಣಾಙ್ಕ		conceal_update_ಸಾಲುಗಳು = FALSE;
#endif

    ಯದಿ (char_avail())
	ಪ್ರತ್ಯಾಗಮ;

    // Trigger CursorMoved if the cursor moved.  Not when the popup menu is
    // visible, the command might delete it.
    ಯದಿ (ready && (has_cursormovedI()
#ifdef FEAT_PROP_POPUP
		|| popup_visible
#endif
#ಯದಿ defined(FEAT_CONCEAL)
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
	ಯದಿ (syntax_present(curwin) && must_redraw)
	    update_screen(0);
#endif
	ಯದಿ (has_cursormovedI())
	{
	    // Make sure curswant is correct, an autocommand may call
	    // getcurpos().
	    update_curswant();
	    ins_apply_autocmds(EVENT_CURSORMOVEDI);
	}
#ifdef FEAT_PROP_POPUP
	ಯದಿ (popup_visible)
	    popup_check_cursor_pos();
#endif
#ifdef FEAT_CONCEAL
	ಯದಿ (curwin->w_p_cole > 0)
	{
	    conceal_old_cursor_ಸಾಲು = last_cursormoved.lnum;
	    conceal_new_cursor_ಸಾಲು = curwin->w_cursor.lnum;
	    conceal_update_ಸಾಲುಗಳು = TRUE;
	}
#endif
	last_cursormoved = curwin->w_cursor;
    }

    // Trigger TextChangedI if b_changedtick_i differs.
    ಯದಿ (ready && has_textchangedI()
	    && curbuf->b_last_changedtick_i != CHANGEDTICK(curbuf)
	    && !pum_visible())
    {
	aco_save_T	aco;
	varnumber_T	tick = CHANGEDTICK(curbuf);

	// Save and restore curwin and curbuf, in case the autocmd changes
	// them.
	aucmd_prepbuf(&aco, curbuf);
	apply_autocmds(EVENT_TEXTCHANGEDI, ರಿಕ್ತ, ರಿಕ್ತ, FALSE, curbuf);
	aucmd_restbuf(&aco);
	curbuf->b_last_changedtick_i = CHANGEDTICK(curbuf);
	ಯದಿ (tick != CHANGEDTICK(curbuf))  // see ins_apply_autocmds()
	    u_save(curwin->w_cursor.lnum,
					(linenr_T)(curwin->w_cursor.lnum + 1));
    }

    // Trigger TextChangedP if b_changedtick_pum differs. When the popupmenu
    // closes TextChangedI will need to trigger for backwards compatibility,
    // thus use different b_last_changedtick* variables.
    ಯದಿ (ready && has_textchangedP()
	    && curbuf->b_last_changedtick_pum != CHANGEDTICK(curbuf)
	    && pum_visible())
    {
	aco_save_T	aco;
	varnumber_T	tick = CHANGEDTICK(curbuf);

	// Save and restore curwin and curbuf, in case the autocmd changes
	// them.
	aucmd_prepbuf(&aco, curbuf);
	apply_autocmds(EVENT_TEXTCHANGEDP, ರಿಕ್ತ, ರಿಕ್ತ, FALSE, curbuf);
	aucmd_restbuf(&aco);
	curbuf->b_last_changedtick_pum = CHANGEDTICK(curbuf);
	ಯದಿ (tick != CHANGEDTICK(curbuf))  // see ins_apply_autocmds()
	    u_save(curwin->w_cursor.lnum,
					(linenr_T)(curwin->w_cursor.lnum + 1));
    }

    ಯದಿ (ready)
	may_trigger_win_scrolled_resized();

    // Trigger SafeState if nothing is pending.
    may_trigger_safestate(ready
	    && !ins_compl_active()
	    && !pum_visible());

#ಯದಿ defined(FEAT_CONCEAL)
    ಯದಿ ((conceal_update_ಸಾಲುಗಳು
	    && (conceal_old_cursor_ಸಾಲು != conceal_new_cursor_ಸಾಲು
		|| conceal_cursor_ಸಾಲು(curwin)))
	    || need_cursor_ಸಾಲು_redraw)
    {
	ಯದಿ (conceal_old_cursor_ಸಾಲು != conceal_new_cursor_ಸಾಲು)
	    redrawWinline(curwin, conceal_old_cursor_ಸಾಲು);
	redrawWinline(curwin, conceal_new_cursor_ಸಾಲು == 0
			    ? curwin->w_cursor.lnum : conceal_new_cursor_ಸಾಲು);
	curwin->w_valid &= ~VALID_CROW;
	need_cursor_ಸಾಲು_redraw = FALSE;
    }
#endif
    ಯದಿ (must_redraw)
	update_screen(0);
    ಅನ್ಯಥಾ ಯದಿ (clear_cmdline || redraw_cmdline)
	showmode();		// clear cmdline and show mode
    showruler(FALSE);
    setcursor();
    emsg_on_display = FALSE;	// may remove error message now
}

/*
 * Handle a CTRL-V or CTRL-Q typed in Insert mode.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ins_ctrl_v(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ		c;
    ಪೂರ್ಣಾಙ್ಕ		did_putchar = FALSE;

    // may need to redraw when no more chars available now
    ins_redraw(FALSE);

    ಯದಿ (redrawing() && !char_avail())
    {
	edit_putchar('^', TRUE);
	did_putchar = TRUE;
    }
    AppendToRedobuff((char_u *)CTRL_V_STR);	// CTRL-V

    add_to_showcmd_c(Ctrl_V);

    // Do not change any modifyOtherKeys ESC sequence to a normal key for
    // CTRL-SHIFT-V.
    c = get_literal(mod_mask & MOD_MASK_SHIFT);
    ಯದಿ (did_putchar)
	// when the line fits in 'columns' the '^' is at the start of the next
	// line and will not removed by the redraw
	edit_unputchar();
    clear_showcmd();

    ಸೇರಿಸು_special(c, FALSE, TRUE);
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
decode_Modify_Other_ಕೀಲಿಗಳು(ಪೂರ್ಣಾಙ್ಕ c)
{
    char_u  *p = typebuf.tb_buf + typebuf.tb_off;
    ಪೂರ್ಣಾಙ್ಕ	    idx;
    ಪೂರ್ಣಾಙ್ಕ	    form = 0;
    ಪೂರ್ಣಾಙ್ಕ	    argidx = 0;
    ಪೂರ್ಣಾಙ್ಕ	    arg[2] = {0, 0};

    // Recognize:
    // form 0: {lead}{key};{modifier}u
    // form 1: {lead}27;{modifier};{key}~
    ಯದಿ (typebuf.tb_len >= 4 && (c == CSI || (c == ESC && *p == '[')))
    {
	idx = (*p == '[');
	ಯದಿ (p[idx] == '2' && p[idx + 1] == '7' && p[idx + 2] == ';' &&
		kitty_protocol_state != KKPS_ENABLED)
	{
	    form = 1;
	    idx += 3;
	}
	ಯಾವತ್ (idx < typebuf.tb_len && argidx < 2)
	{
	    ಯದಿ (p[idx] == ';')
		++argidx;
	    ಅನ್ಯಥಾ ಯದಿ (VIM_ISDIGIT(p[idx]))
		arg[argidx] = arg[argidx] * 10 + (p[idx] - '0');
	    ಅನ್ಯಥಾ
		ಭಙ್ಗ;
	    ++idx;
	}
	ಪೂರ್ಣಾಙ್ಕ kitty_no_mods = argidx == 0 && kitty_protocol_state == KKPS_ENABLED;
	ಯದಿ (idx < typebuf.tb_len
		&& p[idx] == (form == 1 ? '~' : 'u')
		&& (argidx == 1 || kitty_no_mods))
	{
	    // Match, consume the code.
	    typebuf.tb_off += idx + 1;
	    typebuf.tb_len -= idx + 1;
#ಯದಿ defined(FEAT_CLIENTSERVER) || defined(FEAT_EVAL)
	    ಯದಿ (typebuf.tb_len == 0)
		typebuf_was_filled = FALSE;
#endif

	    mod_mask = kitty_no_mods ? 0 : decode_modifiers(arg[!form]);
	    c = merge_modify_Other_ಕೀಲಿಗಳು(arg[form], &mod_mask);
	}
    }

    ಪ್ರತ್ಯಾಗಮ c;
}

/*
 * Put a character directly onto the screen.  It's not stored in a buffer.
 * Used while handling CTRL-K, CTRL-V, etc. in Insert mode.
 */
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ  pc_status;
#define PC_STATUS_UNSET	0	// pc_bytes was not set
#define PC_STATUS_ಬಲ	1	// right half of double-wide char
#define PC_STATUS_ಎಡ	2	// left half of double-wide char
#define PC_STATUS_SET	3	// pc_bytes was filled
ಸ್ಥಿರ char_u pc_bytes[MB_MAXBYTES + 1]; // saved bytes
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ  pc_attr;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ  pc_row;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ  pc_col;

    ಶೂನ್ಯ
edit_putchar(ಪೂರ್ಣಾಙ್ಕ c, ಪೂರ್ಣಾಙ್ಕ highlight)
{
    ಪೂರ್ಣಾಙ್ಕ	    attr;

    ಯದಿ (Screen_ಸಾಲುಗಳು == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    update_topline();	// just in case w_topline isn't valid
    validate_cursor();
    ಯದಿ (highlight)
	attr = HL_ATTR(HLF_8);
    ಅನ್ಯಥಾ
	attr = 0;
    pc_row = W_WINROW(curwin) + curwin->w_wrow;
    pc_col = curwin->w_wincol;
    pc_status = PC_STATUS_UNSET;
#ifdef FEAT_RIGHTLEFT
    ಯದಿ (curwin->w_p_rl)
    {
	pc_col += curwin->w_width - 1 - curwin->w_wcol;
	ಯದಿ (has_mbyte)
	{
	    ಪೂರ್ಣಾಙ್ಕ fix_col = mb_fix_col(pc_col, pc_row);

	    ಯದಿ (fix_col != pc_col)
	    {
		screen_putchar(' ', pc_row, fix_col, attr);
		--curwin->w_wcol;
		pc_status = PC_STATUS_ಬಲ;
	    }
	}
    }
    ಅನ್ಯಥಾ
#endif
    {
	pc_col += curwin->w_wcol;
	ಯದಿ (mb_lefthalve(pc_row, pc_col))
	    pc_status = PC_STATUS_ಎಡ;
    }

    // save the character to be able to put it back
    ಯದಿ (pc_status == PC_STATUS_UNSET)
    {
	screen_getbytes(pc_row, pc_col, pc_bytes, &pc_attr);
	pc_status = PC_STATUS_SET;
    }
    screen_putchar(c, pc_row, pc_col, attr);
}

#ಯದಿ defined(FEAT_JOB_CHANNEL)
/*
 * Set the insert start position for when using a prompt buffer.
 */
    ಶೂನ್ಯ
set_insstart(linenr_T lnum, ಪೂರ್ಣಾಙ್ಕ col)
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
    ಶೂನ್ಯ
edit_unputchar(ಶೂನ್ಯ)
{
    ಯದಿ (pc_status != PC_STATUS_UNSET && pc_row >= ಸಂದೇಶ_scrolled)
    {
	ಯದಿ (pc_status == PC_STATUS_ಬಲ)
	    ++curwin->w_wcol;
	ಯದಿ (pc_status == PC_STATUS_ಬಲ || pc_status == PC_STATUS_ಎಡ)
	    redrawWinline(curwin, curwin->w_cursor.lnum);
	ಅನ್ಯಥಾ
	    screen_puts(pc_bytes, pc_row - ಸಂದೇಶ_scrolled, pc_col, pc_attr);
    }
}

/*
 * Called when "$" is in 'cpoptions': display a '$' at the end of the changed
 * text.  Only works when cursor is in the line that changes.
 */
    ಶೂನ್ಯ
display_dollar(colnr_T col_arg)
{
    colnr_T col = col_arg < 0 ? 0 : col_arg;
    colnr_T save_col;

    ಯದಿ (!redrawing())
	ಪ್ರತ್ಯಾಗಮ;

    cursor_off();
    save_col = curwin->w_cursor.col;
    curwin->w_cursor.col = col;
    ಯದಿ (has_mbyte)
    {
	char_u *p;

	// If on the last byte of a multi-byte move to the first byte.
	p = ml_get_curline();
	curwin->w_cursor.col -= (*mb_head_off)(p, p + col);
    }
    curs_columns(FALSE);	    // recompute w_wrow and w_wcol
    ಯದಿ (curwin->w_wcol < curwin->w_width)
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
    ಶೂನ್ಯ
undisplay_dollar(ಶೂನ್ಯ)
{
    ಯದಿ (dollar_vcol < 0)
	ಪ್ರತ್ಯಾಗಮ;

    dollar_vcol = -1;
    redrawWinline(curwin, curwin->w_cursor.lnum);
}

/*
 * Truncate the space at the end of a line.  This is to be used only in an
 * insert mode.  It handles fixing the replace stack for MODE_REPLACE and
 * MODE_VREPLACE modes.
 */
    ಶೂನ್ಯ
truncate_spaces(char_u *ಸಾಲು, size_t len)
{
    ಪೂರ್ಣಾಙ್ಕ	    i;

    // find start of trailing white space
    ಪ್ರತಿ (i = (ಪೂರ್ಣಾಙ್ಕ)len - 1; i >= 0 && VIM_ISWHITE(ಸಾಲು[i]); i--)
    {
	ಯದಿ (State & REPLACE_FLAG)
	    replace_join(0);	    // remove a NUL from the replace stack
    }
    ಸಾಲು[i + 1] = NUL;
}

/*
 * Backspace the cursor until the given column.  Handles MODE_REPLACE and
 * MODE_VREPLACE modes correctly.  May also be used when not in insert mode at
 * all.  Will attempt not to go before "col" even when there is a composing
 * character.
 */
    ಶೂನ್ಯ
backspace_until_column(ಪೂರ್ಣಾಙ್ಕ col)
{
    ಯಾವತ್ ((ಪೂರ್ಣಾಙ್ಕ)curwin->w_cursor.col > col)
    {
	curwin->w_cursor.col--;
	ಯದಿ (State & REPLACE_FLAG)
	    replace_do_bs(col);
	ಅನ್ಯಥಾ ಯದಿ (!del_char_after_col(col))
	    ಭಙ್ಗ;
    }
}

/*
 * Like del_char(), but make sure not to go before column "limit_col".
 * Only matters when there are composing characters.
 * Return TRUE when something was deleted.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
del_char_after_col(ಪೂರ್ಣಾಙ್ಕ limit_col UNUSED)
{
    ಯದಿ (enc_utf8 && limit_col >= 0)
    {
	colnr_T ecol = curwin->w_cursor.col + 1;

	// Make sure the cursor is at the start of a character, but
	// skip forward again when going too far back because of a
	// composing character.
	mb_adjust_cursor();
	ಯಾವತ್ (curwin->w_cursor.col < (colnr_T)limit_col)
	{
	    ಪೂರ್ಣಾಙ್ಕ l = utf_ptr2len(ml_get_cursor());

	    ಯದಿ (l == 0)  // end of line
		ಭಙ್ಗ;
	    curwin->w_cursor.col += l;
	}
	ಯದಿ (*ml_get_cursor() == NUL || curwin->w_cursor.col == ecol)
	    ಪ್ರತ್ಯಾಗಮ FALSE;
	del_bytes((long)((ಪೂರ್ಣಾಙ್ಕ)ecol - curwin->w_cursor.col), FALSE, TRUE);
    }
    ಅನ್ಯಥಾ
	(ಶೂನ್ಯ)del_char(FALSE);
    ಪ್ರತ್ಯಾಗಮ TRUE;
}

/*
 * Next character is interpreted literally.
 * A one, two or three digit decimal number is interpreted as its byte value.
 * If one or two digits are entered, the next character is given to vungetc().
 * For Unicode a character > 255 may be returned.
 * If "noReduceKeys" is TRUE do not change any modifyOtherKeys ESC sequence
 * into a normal key, return ESC.
 */
    ಪೂರ್ಣಾಙ್ಕ
get_literal(ಪೂರ್ಣಾಙ್ಕ no_Reduce_ಕೀಲಿಗಳು)
{
    ಪೂರ್ಣಾಙ್ಕ		cc;
    ಪೂರ್ಣಾಙ್ಕ		nc;
    ಪೂರ್ಣಾಙ್ಕ		i;
    ಪೂರ್ಣಾಙ್ಕ		hex = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		octal = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		unicode = 0;

    ಯದಿ (got_int)
	ಪ್ರತ್ಯಾಗಮ Ctrl_C;

#ifdef FEAT_GUI
    /*
     * In GUI there is no point inserting the internal code for a special key.
     * It is more useful to insert the string "<KEY>" instead.	This would
     * probably be useful in a text window too, but it would not be
     * vi-compatible (maybe there should be an option for it?) -- webb
     */
    ಯದಿ (gui.in_use)
    {
	++allow_ಕೀಲಿಗಳು;
	ಯದಿ (no_Reduce_ಕೀಲಿಗಳು)
	    ++no_reduce_ಕೀಲಿಗಳು;
    }
#endif
#ifdef USE_ON_FLY_SCROLL
    dont_scroll = TRUE;		// disallow scrolling here
#endif
    ++no_mapping;		// don't map the next key hits
    cc = 0;
    i = 0;
    ಪ್ರತಿ (;;)
    {
	nc = plain_vgetc();
	ಯದಿ ((nc == ESC || nc == CSI) && !no_Reduce_ಕೀಲಿಗಳು)
	    nc = decode_Modify_Other_ಕೀಲಿಗಳು(nc);

	ಯದಿ ((mod_mask & ~MOD_MASK_SHIFT) != 0)
	    // A character with non-Shift modifiers should not be a valid
	    // character for i_CTRL-V_digit.
	    ಭಙ್ಗ;

	ಯದಿ ((State & MODE_CMDLINE) == 0 && MB_BYTE2LEN_CHECK(nc) == 1)
	    add_to_showcmd(nc);
	ಯದಿ (nc == 'x' || nc == 'X')
	    hex = TRUE;
	ಅನ್ಯಥಾ ಯದಿ (nc == 'o' || nc == 'O')
	    octal = TRUE;
	ಅನ್ಯಥಾ ಯದಿ (nc == 'u' || nc == 'U')
	    unicode = nc;
	ಅನ್ಯಥಾ
	{
	    ಯದಿ (hex || unicode != 0)
	    {
		ಯದಿ (!vim_isxdigit(nc))
		    ಭಙ್ಗ;
		cc = cc * 16 + hex2nr(nc);
	    }
	    ಅನ್ಯಥಾ ಯದಿ (octal)
	    {
		ಯದಿ (nc < '0' || nc > '7')
		    ಭಙ್ಗ;
		cc = cc * 8 + nc - '0';
	    }
	    ಅನ್ಯಥಾ
	    {
		ಯದಿ (!VIM_ISDIGIT(nc))
		    ಭಙ್ಗ;
		cc = cc * 10 + nc - '0';
	    }

	    ++i;
	}

	ಯದಿ (cc > 255 && unicode == 0)
	    cc = 255;		// limit range to 0-255
	nc = 0;

	ಯದಿ (hex)		// hex: up to two chars
	{
	    ಯದಿ (i >= 2)
		ಭಙ್ಗ;
	}
	ಅನ್ಯಥಾ ಯದಿ (unicode)	// Unicode: up to four or eight chars
	{
	    ಯದಿ ((unicode == 'u' && i >= 4) || (unicode == 'U' && i >= 8))
		ಭಙ್ಗ;
	}
	ಅನ್ಯಥಾ ಯದಿ (i >= 3)	// decimal or octal: up to three chars
	    ಭಙ್ಗ;
    }
    ಯದಿ (i == 0)	    // no number entered
    {
	ಯದಿ (nc == K_ZERO)   // NUL is stored as NL
	{
	    cc = '\n';
	    nc = 0;
	}
	ಅನ್ಯಥಾ
	{
	    cc = nc;
	    nc = 0;
	}
    }

    ಯದಿ (cc == 0)	// NUL is stored as NL
	cc = '\n';
    ಯದಿ (enc_dbcs && (cc & 0xff) == 0)
	cc = '?';	// don't accept an illegal DBCS char, the NUL in the
			// second byte will cause trouble!

    --no_mapping;
#ifdef FEAT_GUI
    ಯದಿ (gui.in_use)
    {
	--allow_ಕೀಲಿಗಳು;
	ಯದಿ (no_Reduce_ಕೀಲಿಗಳು)
	    --no_reduce_ಕೀಲಿಗಳು;
    }
#endif
    ಯದಿ (nc)
    {
	vungetc(nc);
	// A character typed with i_CTRL-V_digit cannot have modifiers.
	mod_mask = 0;
    }
    got_int = FALSE;	    // CTRL-C typed after CTRL-V is not an interrupt
    ಪ್ರತ್ಯಾಗಮ cc;
}

/*
 * Insert character, taking care of special keys and mod_mask
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ಸೇರಿಸು_special(
    ಪೂರ್ಣಾಙ್ಕ	    c,
    ಪೂರ್ಣಾಙ್ಕ	    allow_modmask,
    ಪೂರ್ಣಾಙ್ಕ	    ctrlv)	    // c was typed after CTRL-V
{
    char_u  *p;
    ಪೂರ್ಣಾಙ್ಕ	    len;

    /*
     * Special function key, translate into "<Key>". Up to the last '>' is
     * inserted with ins_str(), so as not to replace characters in replace
     * mode.
     * Only use mod_mask for special keys, to avoid things like <S-Space>,
     * unless 'allow_modmask' is TRUE.
     */
#ಯದಿ defined(MACOS_X) || defined(FEAT_GUI_GTK)
    // Command-key never produces a normal key
    ಯದಿ (mod_mask & MOD_MASK_CMD)
	allow_modmask = TRUE;
#endif
    ಯದಿ (IS_SPECIAL(c) || (mod_mask && allow_modmask))
    {
	p = get_special_ಕೀಲಿ_name(c, mod_mask);
	len = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(p);
	c = p[len - 1];
	ಯದಿ (len > 2)
	{
	    ಯದಿ (stop_arrow() == FAIL)
		ಪ್ರತ್ಯಾಗಮ;
	    p[len - 1] = NUL;
	    ins_str(p, len - 1);
	    AppendToRedobuffLit(p, -1);
	    ctrlv = FALSE;
	}
    }
    ಯದಿ (stop_arrow() == OK)
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
    ಶೂನ್ಯ
insertchar(
    ಪೂರ್ಣಾಙ್ಕ		c,			// character to insert or NUL
    ಪೂರ್ಣಾಙ್ಕ		flags,			// INSCHAR_FORMAT, etc.
    ಪೂರ್ಣಾಙ್ಕ		second_indent)		// indent for second line if >= 0
{
    ಪೂರ್ಣಾಙ್ಕ		textwidth;
    char_u	*p;
    ಪೂರ್ಣಾಙ್ಕ		fo_ins_blank;
    ಪೂರ್ಣಾಙ್ಕ		force_format = flags & INSCHAR_FORMAT;

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
    ಯದಿ (textwidth > 0
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
#ಯದಿ defined(FEAT_EVAL)
	ಪೂರ್ಣಾಙ್ಕ     do_internal = TRUE;
	colnr_T virtcol = get_nolist_virtcol()
				  + char2cells(c != NUL ? c : gchar_cursor());

	ಯದಿ (*curbuf->b_p_fex != NUL && (flags & INSCHAR_NO_FEX) == 0
		&& (force_format || virtcol > (colnr_T)textwidth))
	{
	    do_internal = (fex_format(curwin->w_cursor.lnum, 1L, c) != 0);
	    // It may be required to save for undo again, e.g. when setline()
	    // was called.
	    ins_need_undo = TRUE;
	}
	ಯದಿ (do_internal)
#endif
	    internal_format(textwidth, second_indent, flags, c == NUL, c);
    }

    ಯದಿ (c == NUL)	    // only formatting was wanted
	ಪ್ರತ್ಯಾಗಮ;

    // Check whether this character should end a comment.
    ಯದಿ (did_ai && c == end_comment_pending)
    {
	char_u  *ಸಾಲು;
	char_u	lead_end[COM_MAX_LEN];	    // end-comment string
	ಪೂರ್ಣಾಙ್ಕ	ಮಧ್ಯ_len, end_len;
	ಪೂರ್ಣಾಙ್ಕ	i;

	/*
	 * Need to remove existing (middle) comment leader and insert end
	 * comment leader.  First, check what comment leader we can find.
	 */
	i = get_leader_len(ಸಾಲು = ml_get_curline(), &p, FALSE, TRUE);
	ಯದಿ (i > 0 && vim_strchr(p, COM_ಮಧ್ಯ) != ರಿಕ್ತ)	// Just checking
	{
	    // Skip middle-comment string
	    ಯಾವತ್ (*p && p[-1] != ':')	// find end of middle flags
		++p;
	    ಮಧ್ಯ_len = copy_option_part(&p, lead_end, COM_MAX_LEN, ",");
	    // Don't count trailing white space for middle_len
	    ಯಾವತ್ (ಮಧ್ಯ_len > 0 && VIM_ISWHITE(lead_end[ಮಧ್ಯ_len - 1]))
		--ಮಧ್ಯ_len;

	    // Find the end-comment string
	    ಯಾವತ್ (*p && p[-1] != ':')	// find end of end flags
		++p;
	    end_len = copy_option_part(&p, lead_end, COM_MAX_LEN, ",");

	    // Skip white space before the cursor
	    i = curwin->w_cursor.col;
	    ಯಾವತ್ (--i >= 0 && VIM_ISWHITE(ಸಾಲು[i]))
		;
	    i++;

	    // Skip to before the middle leader
	    i -= ಮಧ್ಯ_len;

	    // Check some expected things before we go on
	    ಯದಿ (i >= 0 && end_len > 0 && lead_end[end_len - 1] == end_comment_pending)
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

    ಯದಿ (       !ISSPECIAL(c)
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
#define ಒಳಹರಿವು_BUFLEN 100
	char_u		buf[ಒಳಹರಿವು_BUFLEN + 1];
	ಪೂರ್ಣಾಙ್ಕ		i;
	colnr_T		virtcol = 0;

	buf[0] = c;
	i = 1;
	ಯದಿ (textwidth > 0)
	    virtcol = get_nolist_virtcol();
	/*
	 * Stop the string when:
	 * - no more chars available
	 * - finding a special character (command key)
	 * - buffer is full
	 * - running into the 'textwidth' boundary
	 * - need to check for abbreviation: A non-word char after a word-char
	 */
	ಯಾವತ್ (	   (c = vpeekc()) != NUL
		&& !ISSPECIAL(c)
		&& (!has_mbyte || MB_BYTE2LEN_CHECK(c) == 1)
		&& i < ಒಳಹರಿವು_BUFLEN
		&& (textwidth == 0
		    || (virtcol += byte2cells(buf[i - 1])) < (colnr_T)textwidth)
		&& !(!no_abbr && !vim_iswordc(c) && vim_iswordc(buf[i - 1])))
	{
#ifdef FEAT_RIGHTLEFT
	    c = vgetc();
	    ಯದಿ (p_hkmap && ಕೀಲಿ_Typed)
		c = hkmap(c);		    // Hebrew mode mapping
	    buf[i++] = c;
#ಅನ್ಯಥಾ
	    buf[i++] = vgetc();
#endif
	}

#ifdef FEAT_DIGRAPHS
	do_digraph(-1);			// clear digraphs
	do_digraph(buf[i-1]);		// may be the start of a digraph
#endif
	buf[i] = NUL;
	ins_str(buf, i);
	ಯದಿ (flags & INSCHAR_CTRLV)
	{
	    redo_literal(*buf);
	    i = 1;
	}
	ಅನ್ಯಥಾ
	    i = 0;
	ಯದಿ (buf[i] != NUL)
	    AppendToRedobuffLit(buf + i, -1);
    }
    ಅನ್ಯಥಾ
    {
	ಪೂರ್ಣಾಙ್ಕ		cc;

	ಯದಿ (has_mbyte && (cc = (*mb_char2len)(c)) > 1)
	{
	    char_u	buf[MB_MAXBYTES + 1];

	    (*mb_char2bytes)(c, buf);
	    buf[cc] = NUL;
	    ins_char_bytes(buf, cc);
	    AppendCharToRedobuff(c);
	}
	ಅನ್ಯಥಾ
	{
	    ins_char(c);
	    ಯದಿ (flags & INSCHAR_CTRLV)
		redo_literal(c);
	    ಅನ್ಯಥಾ
		AppendCharToRedobuff(c);
	}
    }
}

/*
 * Put a character in the redo buffer, for when just after a CTRL-V.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
redo_literal(ಪೂರ್ಣಾಙ್ಕ c)
{
    char_u	buf[10];

    // Only digits need special treatment.  Translate them into a string of
    // three digits.
    ಯದಿ (VIM_ISDIGIT(c))
    {
	vim_snprintf((ವರ್ಣ *)buf, ಪರಿಮಾಣ(buf), "%03d", c);
	AppendToRedobuff(buf);
    }
    ಅನ್ಯಥಾ
	AppendCharToRedobuff(c);
}

/*
 * start_arrow() is called when an arrow key is used in insert mode.
 * For undo/redo it resembles hitting the <ESC> key.
 */
    ಶೂನ್ಯ
start_arrow(
    pos_T    *end_ಸೇರಿಸು_pos)		// can be NULL
{
    start_arrow_common(end_ಸೇರಿಸು_pos, TRUE);
}

/*
 * Like start_arrow() but with end_change argument.
 * Will prepare for redo of CTRL-G U if "end_change" is FALSE.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
start_arrow_with_change(
    pos_T    *end_ಸೇರಿಸು_pos,		// can be NULL
    ಪೂರ್ಣಾಙ್ಕ	      end_change)		// end undoable change
{
    start_arrow_common(end_ಸೇರಿಸು_pos, end_change);
    ಯದಿ (!end_change)
    {
	AppendCharToRedobuff(Ctrl_G);
	AppendCharToRedobuff('U');
    }
}

    ಸ್ಥಿರ ಶೂನ್ಯ
start_arrow_common(
    pos_T    *end_ಸೇರಿಸು_pos,		// can be NULL
    ಪೂರ್ಣಾಙ್ಕ	      end_change)		// end undoable change
{
    ಯದಿ (!arrow_used && end_change)	// something has been inserted
    {
	AppendToRedobuff(ESC_STR);
	stop_ಸೇರಿಸು(end_ಸೇರಿಸು_pos, FALSE, FALSE);
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
    ಸ್ಥಿರ ಶೂನ್ಯ
check_spell_redraw(ಶೂನ್ಯ)
{
    ಯದಿ (spell_redraw_lnum != 0)
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
    ಪೂರ್ಣಾಙ್ಕ
stop_arrow(ಶೂನ್ಯ)
{
    ಯದಿ (arrow_used)
    {
	Insstart = curwin->w_cursor;	// new insertion starts here
	ಯದಿ (Insstart.col > Insstart_orig.col && !ins_need_undo)
	    // Don't update the original insert position when moved to the
	    // right, except when nothing was inserted yet.
	    update_Insstart_orig = FALSE;
	Insstart_textlen = (colnr_T)linetabsize_str(ml_get_curline());

	ಯದಿ (u_save_cursor() == OK)
	{
	    arrow_used = FALSE;
	    ins_need_undo = FALSE;
	}

	ai_col = 0;
	ಯದಿ (State & VREPLACE_FLAG)
	{
	    orig_ಸಾಲು_ಎಣಿಕೆ = curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
	    vr_ಸಾಲುಗಳು_changed = 1;
	}
	ResetRedobuff();
	AppendToRedobuff((char_u *)"1i");   // pretend we start an insertion
	new_ಸೇರಿಸು_skip = 2;
    }
    ಅನ್ಯಥಾ ಯದಿ (ins_need_undo)
    {
	ಯದಿ (u_save_cursor() == OK)
	{
	    // A command or event may have moved the cursor before the next
	    // edit. Pull Insstart back only when the cursor moved above it,
	    // so that later edits can properly decide whether an extra undo
	    // entry is needed. Advancing Insstart would mis-place '[ after a
	    // register paste.
	    ಯದಿ (LT_POS(curwin->w_cursor, Insstart))
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

    ಪ್ರತ್ಯಾಗಮ (arrow_used || ins_need_undo ? FAIL : OK);
}

/*
 * Do a few things to stop inserting.
 * "end_insert_pos" is where insert ended.  It is NULL when we already jumped
 * to another window/buffer.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
stop_ಸೇರಿಸು(
    pos_T	*end_ಸೇರಿಸು_pos,
    ಪೂರ್ಣಾಙ್ಕ		esc,			// called by ins_esc()
    ಪೂರ್ಣಾಙ್ಕ		nomove)			// <c-\><c-o>, don't move cursor
{
    ಪೂರ್ಣಾಙ್ಕ		cc;
    string_T	inserted;

    stop_redo_ins();
    replace_flush();		// abandon replace stack

    /*
     * Save the inserted text for later redo with ^@ and CTRL-A.
     * Don't do it when "restart_edit" was set and nothing was inserted,
     * otherwise CTRL-O w and then <Left> will clear "last_insert".
     */
    inserted = get_inserted();
    ಪೂರ್ಣಾಙ್ಕ added = inserted.string == ರಿಕ್ತ ? 0 : (ಪೂರ್ಣಾಙ್ಕ)inserted.length - new_ಸೇರಿಸು_skip;
    ಯದಿ (did_restart_edit == 0 || added > 0)
    {
	vim_free(last_ಸೇರಿಸು.string);
	last_ಸೇರಿಸು = inserted;			    // structure copy
	last_ಸೇರಿಸು_skip = added < 0 ? 0 : new_ಸೇರಿಸು_skip;
    }
    ಅನ್ಯಥಾ
	vim_free(inserted.string);

    ಯದಿ (!arrow_used && end_ಸೇರಿಸು_pos != ರಿಕ್ತ)
    {
	// Auto-format now.  It may seem strange to do this when stopping an
	// insertion (or moving the cursor), but it's required when appending
	// a line and having it end in a space.  But only do it when something
	// was actually inserted, otherwise undo won't work.
	ಯದಿ (!ins_need_undo && has_format_option(FO_AUTO))
	{
	    pos_T   tpos = curwin->w_cursor;

	    // When the cursor is at the end of the line after a space the
	    // formatting will move it to the following word.  Avoid that by
	    // moving the cursor onto the space.
	    cc = 'x';
	    ಯದಿ (curwin->w_cursor.col > 0 && gchar_cursor() == NUL)
	    {
		dec_cursor();
		cc = gchar_cursor();
		ಯದಿ (!VIM_ISWHITE(cc))
		    curwin->w_cursor = tpos;
	    }

	    auto_format(TRUE, FALSE);

	    ಯದಿ (VIM_ISWHITE(cc))
	    {
		ಯದಿ (gchar_cursor() != NUL)
		    inc_cursor();
		// If the cursor is still at the same character, also keep
		// the "coladd".
		ಯದಿ (gchar_cursor() == NUL
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
	ಯದಿ (!nomove && did_ai && (esc || (vim_strchr(p_cpo, CPO_INDENT) == ರಿಕ್ತ
			&& curwin->w_cursor.lnum != end_ಸೇರಿಸು_pos->lnum))
		&& end_ಸೇರಿಸು_pos->lnum <= curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	{
	    pos_T	tpos = curwin->w_cursor;
	    colnr_T	prev_col = end_ಸೇರಿಸು_pos->col;
	    colnr_T	strip_col;

	    curwin->w_cursor = *end_ಸೇರಿಸು_pos;
	    check_cursor_col();  // make sure it is not past the line

	    // Where the loop would actually start (back up if on NUL).
	    strip_col = curwin->w_cursor.col;
	    ಯದಿ (gchar_cursor() == NUL && strip_col > 0)
		--strip_col;

	    // Don't strip if non-whitespace follows: setline() from a <Cmd>
	    // mapping or CursorHoldI autocmd may have inserted content.
	    ಯದಿ (*skipwhite(ml_get_curline() + strip_col) == NUL)
	    {
		curwin->w_cursor.col = strip_col;
		ಪ್ರತಿ (;;)
		{
		    ಯದಿ (gchar_cursor() == NUL && curwin->w_cursor.col > 0)
			--curwin->w_cursor.col;
		    cc = gchar_cursor();
		    ಯದಿ (!VIM_ISWHITE(cc))
			ಭಙ್ಗ;
		    ಯದಿ (del_char(TRUE) == FAIL)
			ಭಙ್ಗ;  // should not happen
		}
		ಯದಿ (curwin->w_cursor.lnum != tpos.lnum)
		    curwin->w_cursor = tpos;
		ಅನ್ಯಥಾ ಯದಿ (curwin->w_cursor.col < prev_col)
		{
		    // reset tpos, could have been invalidated in the loop above
		    tpos = curwin->w_cursor;
		    tpos.col++;
		    ಯದಿ (cc != NUL && gchar_pos(&tpos) == NUL)
			++curwin->w_cursor.col;	// put cursor back on the NUL
		}

		// <C-S-Right> may have started Visual mode, adjust the position for
		// deleted characters.
		ಯದಿ (VIsual_active)
		    check_visual_pos();
	    }
	    ಅನ್ಯಥಾ
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
    ಯದಿ (end_ಸೇರಿಸು_pos != ರಿಕ್ತ)
    {
	curbuf->b_op_start = Insstart;
	curbuf->b_op_start_orig = Insstart_orig;
	curbuf->b_op_end = *end_ಸೇರಿಸು_pos;
    }
}

/*
 * Set the last inserted text to a single character.
 * Used for the replace command.
 */
    ಶೂನ್ಯ
set_last_ಸೇರಿಸು(ಪೂರ್ಣಾಙ್ಕ c)
{
    char_u	*s;

    vim_free(last_ಸೇರಿಸು.string);
    last_ಸೇರಿಸು.string = alloc(MB_MAXBYTES * 3 + 5);
    ಯದಿ (last_ಸೇರಿಸು.string == ರಿಕ್ತ)
    {
	last_ಸೇರಿಸು.length = 0;
	ಪ್ರತ್ಯಾಗಮ;
    }

    s = last_ಸೇರಿಸು.string;
    // Use the CTRL-V only when entering a special char
    ಯದಿ (c < ' ' || c == DEL)
	*s++ = Ctrl_V;
    s = add_char2buf(c, s);
    *s++ = ESC;
    *s = NUL;
    last_ಸೇರಿಸು.length = (size_t)(s - last_ಸೇರಿಸು.string);
    last_ಸೇರಿಸು_skip = 0;
}

#ಯದಿ defined(EXITFREE)
    ಶೂನ್ಯ
free_last_ಸೇರಿಸು(ಶೂನ್ಯ)
{
    VIM_CLEAR_STRING(last_ಸೇರಿಸು);
}
#endif

/*
 * Add character "c" to buffer "s".  Escape the special meaning of K_SPECIAL
 * and CSI.  Handle multi-byte characters.
 * Returns a pointer to after the added bytes.
 */
    char_u *
add_char2buf(ಪೂರ್ಣಾಙ್ಕ c, char_u *s)
{
    char_u	temp[MB_MAXBYTES + 1];
    ಪೂರ್ಣಾಙ್ಕ		i;
    ಪೂರ್ಣಾಙ್ಕ		len;

    len = (*mb_char2bytes)(c, temp);
    ಪ್ರತಿ (i = 0; i < len; ++i)
    {
	c = temp[i];
	// Need to escape K_SPECIAL and CSI like in the typeahead buffer.
	ಯದಿ (c == K_SPECIAL)
	{
	    *s++ = K_SPECIAL;
	    *s++ = KS_SPECIAL;
	    *s++ = KE_FILLER;
	}
#ifdef FEAT_GUI
	ಅನ್ಯಥಾ ಯದಿ (c == CSI)
	{
	    *s++ = CSI;
	    *s++ = KS_EXTRA;
	    *s++ = (ಪೂರ್ಣಾಙ್ಕ)KE_CSI;
	}
#endif
	ಅನ್ಯಥಾ
	    *s++ = c;
    }
    ಪ್ರತ್ಯಾಗಮ s;
}

/*
 * move cursor to start of line
 * if flags & BL_WHITE	move to first non-white
 * if flags & BL_SOL	move to first non-white if startofline is set,
 *			    otherwise keep "curswant" column
 * if flags & BL_FIX	don't leave the cursor on a NUL.
 */
    ಶೂನ್ಯ
beginline(ಪೂರ್ಣಾಙ್ಕ flags)
{
    ಯದಿ ((flags & BL_SOL) && !p_sol)
	coladvance(curwin->w_curswant);
    ಅನ್ಯಥಾ
    {
	curwin->w_cursor.col = 0;
	curwin->w_cursor.coladd = 0;

	ಯದಿ (flags & (BL_WHITE | BL_SOL))
	{
	    char_u  *ptr;

	    ಪ್ರತಿ (ptr = ml_get_curline(); VIM_ISWHITE(*ptr)
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

    ಪೂರ್ಣಾಙ್ಕ
oneright(ಶೂನ್ಯ)
{
    char_u	*ptr;
    ಪೂರ್ಣಾಙ್ಕ		l;

    ಯದಿ (virtual_active())
    {
	pos_T	prevpos = curwin->w_cursor;

	// Adjust for multi-wide char (excluding TAB)
	ptr = ml_get_cursor();
	coladvance(getviscol() + ((*ptr != TAB
					  && vim_isprintc((*mb_ptr2char)(ptr)))
		    ? ptr2cells(ptr) : 1));
	curwin->w_set_curswant = true;
	// Return OK if the cursor moved, FAIL otherwise (at window edge).
	ಪ್ರತ್ಯಾಗಮ (prevpos.col != curwin->w_cursor.col
		    || prevpos.coladd != curwin->w_cursor.coladd) ? OK : FAIL;
    }

    ptr = ml_get_cursor();
    ಯದಿ (*ptr == NUL)
	ಪ್ರತ್ಯಾಗಮ FAIL;	    // already at the very end

    ಯದಿ (has_mbyte)
	l = (*mb_ptr2len)(ptr);
    ಅನ್ಯಥಾ
	l = 1;

    // move "l" bytes right, but don't end up on the NUL, unless 'virtualedit'
    // contains "onemore".
    ಯದಿ (ptr[l] == NUL && (get_ve_flags() & VE_ONEMORE) == 0)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    curwin->w_cursor.col += l;

    curwin->w_set_curswant = true;
    adjust_skipcol();
    ಪ್ರತ್ಯಾಗಮ OK;
}

    ಪೂರ್ಣಾಙ್ಕ
oneleft(ಶೂನ್ಯ)
{
    ಯದಿ (virtual_active())
    {
#ifdef FEAT_LINEBREAK
	ಪೂರ್ಣಾಙ್ಕ width;
#endif
	ಪೂರ್ಣಾಙ್ಕ v = getviscol();

	ಯದಿ (v == 0)
	    ಪ್ರತ್ಯಾಗಮ FAIL;

#ifdef FEAT_LINEBREAK
	// We might get stuck on 'showbreak', skip over it.
	width = 1;
	ಪ್ರತಿ (;;)
	{
	    coladvance(v - width);
	    // getviscol() is slow, skip it when 'showbreak' is empty,
	    // 'breakindent' is not set and there are no multi-byte
	    // characters
	    ಯದಿ ((*get_showbreak_ಮೌಲ್ಯ(curwin) == NUL && !curwin->w_p_bri
					     && !has_mbyte) || getviscol() < v)
		ಭಙ್ಗ;
	    ++width;
	}
#ಅನ್ಯಥಾ
	coladvance(v - 1);
#endif

	ಯದಿ (curwin->w_cursor.coladd == 1)
	{
	    char_u *ptr;

	    // Adjust for multi-wide char (not a TAB)
	    ptr = ml_get_cursor();
	    ಯದಿ (*ptr != TAB && vim_isprintc((*mb_ptr2char)(ptr))
							 && ptr2cells(ptr) > 1)
		curwin->w_cursor.coladd = 0;
	}

	curwin->w_set_curswant = true;
	adjust_skipcol();
	ಪ್ರತ್ಯಾಗಮ OK;
    }

    ಯದಿ (curwin->w_cursor.col == 0)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    curwin->w_set_curswant = true;
    --curwin->w_cursor.col;

    // if the character on the left of the current cursor is a multi-byte
    // character, move to its first byte
    ಯದಿ (has_mbyte)
	mb_adjust_cursor();
    adjust_skipcol();
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Move the cursor up "n" lines in window "wp".
 * Takes care of closed folds.
 */
    ಶೂನ್ಯ
cursor_up_inner(win_T *wp, long n)
{
    linenr_T	lnum = wp->w_cursor.lnum;

    ಯದಿ (n >= lnum)
	lnum = 1;
    ಅನ್ಯಥಾ
#ifdef FEAT_FOLDING
	ಯದಿ (hasAnyFolding(wp))
    {
	/*
	 * Count each sequence of folded lines as one logical line.
	 */
	// go to the start of the current fold
	(ಶೂನ್ಯ)hasFoldingWin(wp, lnum, &lnum, ರಿಕ್ತ, TRUE, ರಿಕ್ತ);

	ಯಾವತ್ (n--)
	{
	    // move up one line
	    --lnum;
	    ಯದಿ (lnum <= 1)
		ಭಙ್ಗ;
	    // If we entered a fold, move to the beginning, unless in
	    // Insert mode or when 'foldopen' contains "all": it will open
	    // in a moment.
	    ಯದಿ (n > 0 || !((State & MODE_ಸೇರಿಸು) || (fdo_flags & FDO_ALL)))
		(ಶೂನ್ಯ)hasFoldingWin(wp, lnum, &lnum, ರಿಕ್ತ, TRUE, ರಿಕ್ತ);
	}
	ಯದಿ (lnum < 1)
	    lnum = 1;
    }
    ಅನ್ಯಥಾ
#endif
	lnum -= n;

    wp->w_cursor.lnum = lnum;
}

    ಪೂರ್ಣಾಙ್ಕ
cursor_up(
    long	n,
    ಪೂರ್ಣಾಙ್ಕ		upd_topline)	    // When TRUE: update topline
{
    // This fails if the cursor is already in the first line or the count is
    // larger than the line number and '-' is in 'cpoptions'
    linenr_T lnum = curwin->w_cursor.lnum;
    ಯದಿ (n > 0 && (lnum <= 1
		       || (n >= lnum && vim_strchr(p_cpo, CPO_MINUS) != ರಿಕ್ತ)))
	ಪ್ರತ್ಯಾಗಮ FAIL;
    cursor_up_inner(curwin, n);

    // try to advance to the column we want to be at
    coladvance(curwin->w_curswant);

    ಯದಿ (upd_topline)
	update_topline();	// make sure curwin->w_topline is valid

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Move the cursor down "n" lines in window "wp".
 * Takes care of closed folds.
 */
    ಶೂನ್ಯ
cursor_down_inner(win_T *wp, long n)
{
    linenr_T	lnum = wp->w_cursor.lnum;
    linenr_T	ಸಾಲು_ಎಣಿಕೆ = wp->w_buffer->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;

    ಯದಿ (lnum + n >= ಸಾಲು_ಎಣಿಕೆ)
	lnum = ಸಾಲು_ಎಣಿಕೆ;
    ಅನ್ಯಥಾ
#ifdef FEAT_FOLDING
	ಯದಿ (hasAnyFolding(wp))
    {
	linenr_T	last;

	// count each sequence of folded lines as one logical line
	ಯಾವತ್ (n--)
	{
	    ಯದಿ (hasFoldingWin(wp, lnum, ರಿಕ್ತ, &last, TRUE, ರಿಕ್ತ))
		lnum = last + 1;
	    ಅನ್ಯಥಾ
		++lnum;
	    ಯದಿ (lnum >= ಸಾಲು_ಎಣಿಕೆ)
		ಭಙ್ಗ;
	}
	ಯದಿ (lnum > ಸಾಲು_ಎಣಿಕೆ)
	    lnum = ಸಾಲು_ಎಣಿಕೆ;
    }
    ಅನ್ಯಥಾ
#endif
	lnum += n;

    wp->w_cursor.lnum = lnum;
}

/*
 * Cursor down a number of logical lines.
 */
    ಪೂರ್ಣಾಙ್ಕ
cursor_down(
    long	n,
    ಪೂರ್ಣಾಙ್ಕ		upd_topline)	    // When TRUE: update topline
{
    linenr_T	lnum = curwin->w_cursor.lnum;
    linenr_T	ಸಾಲು_ಎಣಿಕೆ = curwin->w_buffer->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
    // This fails if the cursor is already in the last (folded) line, or would
    // move beyond the last line and '-' is in 'cpoptions'.
#ifdef FEAT_FOLDING
    hasFoldingWin(curwin, lnum, ರಿಕ್ತ, &lnum, TRUE, ರಿಕ್ತ);
#endif
    ಯದಿ (n > 0
	    && (lnum >= ಸಾಲು_ಎಣಿಕೆ
		|| (lnum + n > ಸಾಲು_ಎಣಿಕೆ
				     && vim_strchr(p_cpo, CPO_MINUS) != ರಿಕ್ತ)))
	ಪ್ರತ್ಯಾಗಮ FAIL;
    cursor_down_inner(curwin, n);

    // try to advance to the column we want to be at
    coladvance(curwin->w_curswant);

    ಯದಿ (upd_topline)
	update_topline();	// make sure curwin->w_topline is valid

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Stuff the last inserted text in the read buffer.
 * Last_insert actually is a copy of the redo buffer, so we
 * first have to remove the command.
 */
    ಪೂರ್ಣಾಙ್ಕ
stuff_inserted(
    ಪೂರ್ಣಾಙ್ಕ	    c,		// Command character to be inserted
    long    ಎಣಿಕೆ,	// Repeat this many times
    ಪೂರ್ಣಾಙ್ಕ	    no_esc)	// Don't add an ESC at the end
{
    string_T	ಸೇರಿಸು;				// text to be inserted
    char_u	last = ' ';

    ಸೇರಿಸು = get_last_ಸೇರಿಸು();
    ಯದಿ (ಸೇರಿಸು.string == ರಿಕ್ತ)
    {
	emsg(_(e_no_inserted_text_yet));
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    // may want to stuff the command character, to start Insert mode
    ಯದಿ (c != NUL)
	stuffcharReadbuff(c);

    ಯದಿ (ಸೇರಿಸು.length > 0)
    {
	char_u	*p;

	// look for the last ESC in 'insert'
	ಪ್ರತಿ (p = ಸೇರಿಸು.string + ಸೇರಿಸು.length - 1; p >= ಸೇರಿಸು.string; --p)
	{
	    ಯದಿ (*p == ESC)
	    {
		ಸೇರಿಸು.length = (size_t)(p - ಸೇರಿಸು.string);
		ಭಙ್ಗ;
	    }
	}
    }

    ಯದಿ (ಸೇರಿಸು.length > 0)
    {
	char_u	*p = ಸೇರಿಸು.string + ಸೇರಿಸು.length - 1;

	// when the last char is either "0" or "^" it will be quoted if no ESC
	// comes after it OR if it will insert more than once and "ptr"
	// starts with ^D.	-- Acevedo
	ಯದಿ ((*p == '0' || *p == '^')
		&& (no_esc || (*ಸೇರಿಸು.string == Ctrl_D && ಎಣಿಕೆ > 1)))
	{
	    last = *p;
	    --ಸೇರಿಸು.length;
	}
    }

    ಕುರು
    {
	stuffReadbuffLen(ಸೇರಿಸು.string, (long)ಸೇರಿಸು.length);
	// a trailing "0" is inserted as "<C-V>048", "^" as "<C-V>^"
	ವಿಕಲ್ಪ (last)
	{
	ಪಕ್ಷ '0':
#define TEXT_TO_ಸೇರಿಸು "\026\060\064\070"
	    stuffReadbuffLen((char_u *)TEXT_TO_ಸೇರಿಸು, STRLEN_LITERAL(TEXT_TO_ಸೇರಿಸು));
#undef TEXT_TO_ಸೇರಿಸು
	    ಭಙ್ಗ;

	ಪಕ್ಷ '^':
#define TEXT_TO_ಸೇರಿಸು "\026^"
	    stuffReadbuffLen((char_u *)TEXT_TO_ಸೇರಿಸು, STRLEN_LITERAL(TEXT_TO_ಸೇರಿಸು));
#undef TEXT_TO_ಸೇರಿಸು
	    ಭಙ್ಗ;

	ಸ್ವಭಾವ:
	    ಭಙ್ಗ;
	}
    }
    ಯಾವತ್ (--ಎಣಿಕೆ > 0);

    // may want to stuff a trailing ESC, to get out of Insert mode
    ಯದಿ (!no_esc)
	stuffcharReadbuff(ESC);

    ಪ್ರತ್ಯಾಗಮ OK;
}

    string_T
get_last_ಸೇರಿಸು(ಶೂನ್ಯ)
{
    string_T ಸೇರಿಸು = {ರಿಕ್ತ, 0};

    ಯದಿ (last_ಸೇರಿಸು.string != ರಿಕ್ತ)
    {
	ಸೇರಿಸು.string = last_ಸೇರಿಸು.string + last_ಸೇರಿಸು_skip;
	ಸೇರಿಸು.length = (size_t)(last_ಸೇರಿಸು.length - last_ಸೇರಿಸು_skip);
    }

    ಪ್ರತ್ಯಾಗಮ ಸೇರಿಸು;
}

/*
 * Get last inserted string, and remove trailing <Esc>.
 * Returns pointer to allocated memory (must be freed) or NULL.
 */
    char_u *
get_last_ಸೇರಿಸು_save(ಶೂನ್ಯ)
{
    string_T	ಸೇರಿಸು = get_last_ಸೇರಿಸು();
    char_u	*s;

    ಯದಿ (ಸೇರಿಸು.string == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    s = vim_strnsave(ಸೇರಿಸು.string, ಸೇರಿಸು.length);
    ಯದಿ (s == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    ಯದಿ (ಸೇರಿಸು.length > 0 && s[ಸೇರಿಸು.length - 1] == ESC)	// remove trailing ESC
	s[--ಸೇರಿಸು.length] = NUL;
    ಪ್ರತ್ಯಾಗಮ s;
}

/*
 * Check the word in front of the cursor for an abbreviation.
 * Called when the non-id character "c" has been entered.
 * When an abbreviation is recognized it is removed from the text and
 * the replacement string is inserted in typebuf.tb_buf[], followed by "c".
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
echeck_abbr(ಪೂರ್ಣಾಙ್ಕ c)
{
    // Don't check for abbreviation in paste mode, when disabled and just
    // after moving around with cursor keys.
    ಯದಿ (p_paste || no_abbr || arrow_used)
	ಪ್ರತ್ಯಾಗಮ FALSE;

    ಪ್ರತ್ಯಾಗಮ check_abbr(c, ml_get_curline(), curwin->w_cursor.col,
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

ಸ್ಥಿರ char_u	*replace_ರಾಶಿ = ರಿಕ್ತ;
ಸ್ಥಿರ long	replace_ರಾಶಿ_nr = 0;	    // next entry in replace stack
ಸ್ಥಿರ long	replace_ರಾಶಿ_len = 0;	    // max. number of entries

    ಶೂನ್ಯ
replace_ತಳ್ಳು(
    ಪೂರ್ಣಾಙ್ಕ	    c)	    // character that is replaced (NUL is none)
{
    char_u  *p;

    ಯದಿ (replace_ರಾಶಿ_nr < replace_offset)	// nothing to do
	ಪ್ರತ್ಯಾಗಮ;
    ಯದಿ (replace_ರಾಶಿ_len <= replace_ರಾಶಿ_nr)
    {
	replace_ರಾಶಿ_len += 50;
	p = ALLOC_MULT(char_u, replace_ರಾಶಿ_len);
	ಯದಿ (p == ರಿಕ್ತ)	    // out of memory
	{
	    replace_ರಾಶಿ_len -= 50;
	    ಪ್ರತ್ಯಾಗಮ;
	}
	ಯದಿ (replace_ರಾಶಿ != ರಿಕ್ತ)
	{
	    mch_memmove(p, replace_ರಾಶಿ,
				 (size_t)(replace_ರಾಶಿ_nr * ಪರಿಮಾಣ(char_u)));
	    vim_free(replace_ರಾಶಿ);
	}
	replace_ರಾಶಿ = p;
    }
    p = replace_ರಾಶಿ + replace_ರಾಶಿ_nr - replace_offset;
    ಯದಿ (replace_offset)
	mch_memmove(p + 1, p, (size_t)(replace_offset * ಪರಿಮಾಣ(char_u)));
    *p = c;
    ++replace_ರಾಶಿ_nr;
}

/*
 * Push a character onto the replace stack.  Handles a multi-byte character in
 * reverse byte order, so that the first byte is popped off first.
 * Return the number of bytes done (includes composing characters).
 */
    ಪೂರ್ಣಾಙ್ಕ
replace_ತಳ್ಳು_mb(char_u *p)
{
    ಪೂರ್ಣಾಙ್ಕ l = (*mb_ptr2len)(p);
    ಪೂರ್ಣಾಙ್ಕ j;

    ಪ್ರತಿ (j = l - 1; j >= 0; --j)
	replace_ತಳ್ಳು(p[j]);
    ಪ್ರತ್ಯಾಗಮ l;
}

/*
 * Pop one item from the replace stack.
 * return -1 if stack empty
 * return replaced character or NUL otherwise
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
replace_ತೆಗೆ(ಶೂನ್ಯ)
{
    ಯದಿ (replace_ರಾಶಿ_nr == 0)
	ಪ್ರತ್ಯಾಗಮ -1;
    ಪ್ರತ್ಯಾಗಮ (ಪೂರ್ಣಾಙ್ಕ)replace_ರಾಶಿ[--replace_ರಾಶಿ_nr];
}

/*
 * Join the top two items on the replace stack.  This removes to "off"'th NUL
 * encountered.
 */
    ಶೂನ್ಯ
replace_join(
    ಪೂರ್ಣಾಙ್ಕ	    off)	// offset for which NUL to remove
{
    ಪೂರ್ಣಾಙ್ಕ	    i;

    ಪ್ರತಿ (i = replace_ರಾಶಿ_nr; --i >= 0; )
	ಯದಿ (replace_ರಾಶಿ[i] == NUL && off-- <= 0)
	{
	    --replace_ರಾಶಿ_nr;
	    mch_memmove(replace_ರಾಶಿ + i, replace_ರಾಶಿ + i + 1,
					      (size_t)(replace_ರಾಶಿ_nr - i));
	    ಪ್ರತ್ಯಾಗಮ;
	}
}

/*
 * Pop bytes from the replace stack until a NUL is found, and insert them
 * before the cursor.  Can only be used in MODE_REPLACE or MODE_VREPLACE state.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
replace_ತೆಗೆ_ins(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ	    cc;
    ಪೂರ್ಣಾಙ್ಕ	    oldState = State;

    State = MODE_NORMAL;			// don't want MODE_REPLACE here
    ಯಾವತ್ ((cc = replace_ತೆಗೆ()) > 0)
    {
	mb_replace_ತೆಗೆ_ins(cc);
	dec_cursor();
    }
    State = oldState;
}

/*
 * Insert bytes popped from the replace stack. "cc" is the first byte.  If it
 * indicates a multi-byte char, pop the other bytes too.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
mb_replace_ತೆಗೆ_ins(ಪೂರ್ಣಾಙ್ಕ cc)
{
    ಪೂರ್ಣಾಙ್ಕ		n;
    char_u	buf[MB_MAXBYTES + 1];
    ಪೂರ್ಣಾಙ್ಕ		i;
    ಪೂರ್ಣಾಙ್ಕ		c;

    ಯದಿ (has_mbyte && (n = MB_BYTE2LEN(cc)) > 1)
    {
	buf[0] = cc;
	ಪ್ರತಿ (i = 1; i < n; ++i)
	    buf[i] = replace_ತೆಗೆ();
	ins_bytes_len(buf, n);
    }
    ಅನ್ಯಥಾ
	ins_char(cc);

    ಯದಿ (enc_utf8)
	// Handle composing chars.
	ಪ್ರತಿ (;;)
	{
	    c = replace_ತೆಗೆ();
	    ಯದಿ (c == -1)	    // stack empty
		ಭಙ್ಗ;
	    ಯದಿ ((n = MB_BYTE2LEN(c)) == 1)
	    {
		// Not a multi-byte char, put it back.
		replace_ತಳ್ಳು(c);
		ಭಙ್ಗ;
	    }

	    buf[0] = c;
	    ಪ್ರತಿ (i = 1; i < n; ++i)
		buf[i] = replace_ತೆಗೆ();
	    ಯದಿ (utf_iscomposing(utf_ptr2char(buf)))
		ins_bytes_len(buf, n);
	    ಅನ್ಯಥಾ
	    {
		// Not a composing char, put it back.
		ಪ್ರತಿ (i = n - 1; i >= 0; --i)
		    replace_ತಳ್ಳು(buf[i]);
		ಭಙ್ಗ;
	    }

	}
}

/*
 * make the replace stack empty
 * (called when exiting replace mode)
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
replace_flush(ಶೂನ್ಯ)
{
    VIM_CLEAR(replace_ರಾಶಿ);
    replace_ರಾಶಿ_len = 0;
    replace_ರಾಶಿ_nr = 0;
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
    ಸ್ಥಿರ ಶೂನ್ಯ
replace_do_bs(ಪೂರ್ಣಾಙ್ಕ limit_col)
{
    ಪೂರ್ಣಾಙ್ಕ		cc;
    ಪೂರ್ಣಾಙ್ಕ		orig_len = 0;
    ಪೂರ್ಣಾಙ್ಕ		ins_len;
    ಪೂರ್ಣಾಙ್ಕ		orig_vcols = 0;
    colnr_T	start_vcol;
    char_u	*p;
    ಪೂರ್ಣಾಙ್ಕ		i;
    ಪೂರ್ಣಾಙ್ಕ		vcol;

    cc = replace_ತೆಗೆ();
    ಯದಿ (cc > 0)
    {
#ifdef FEAT_PROP_POPUP
	size_t	len_before = 0;  // init to shut up GCC

	ಯದಿ (curbuf->b_has_textprop)
	{
	    // Do not adjust text properties for individual delete and insert
	    // operations, do it afterwards on the resulting text.
	    len_before = ml_get_curline_len();
	    ++text_prop_frozen;
	}
#endif
	ಯದಿ (State & VREPLACE_FLAG)
	{
	    // Get the number of screen cells used by the character we are
	    // going to delete.
	    getvcol(curwin, &curwin->w_cursor, ರಿಕ್ತ, &start_vcol, ರಿಕ್ತ, 0);
	    orig_vcols = chartabsize(ml_get_cursor(), start_vcol);
	}
	ಯದಿ (has_mbyte)
	{
	    (ಶೂನ್ಯ)del_char_after_col(limit_col);
	    ಯದಿ (State & VREPLACE_FLAG)
		orig_len = ml_get_cursor_len();
	    replace_ತಳ್ಳು(cc);
	}
	ಅನ್ಯಥಾ
	{
	    pchar_cursor(cc);
	    ಯದಿ (State & VREPLACE_FLAG)
		orig_len = ml_get_cursor_len() - 1;
	}
	replace_ತೆಗೆ_ins();

	ಯದಿ (State & VREPLACE_FLAG)
	{
	    // Get the number of screen cells used by the inserted characters
	    p = ml_get_cursor();
	    ins_len = ml_get_cursor_len() - orig_len;
	    vcol = start_vcol;
	    ಪ್ರತಿ (i = 0; i < ins_len; ++i)
	    {
		vcol += chartabsize(p + i, vcol);
		i += (*mb_ptr2len)(p + i) - 1;
	    }
	    vcol -= start_vcol;

	    // Delete spaces that were inserted after the cursor to keep the
	    // text aligned.
	    curwin->w_cursor.col += ins_len;
	    ಯಾವತ್ (vcol > orig_vcols && gchar_cursor() == ' ')
	    {
		del_char(FALSE);
		++orig_vcols;
	    }
	    curwin->w_cursor.col -= ins_len;
	}

	// mark the buffer as changed and prepare for displaying
	changed_bytes(curwin->w_cursor.lnum, curwin->w_cursor.col);

#ifdef FEAT_PROP_POPUP
	ಯದಿ (curbuf->b_has_textprop)
	{
	    size_t len_now = ml_get_curline_len();

	    --text_prop_frozen;
	    adjust_prop_columns(curwin->w_cursor.lnum, curwin->w_cursor.col,
					   (ಪೂರ್ಣಾಙ್ಕ)(len_now - len_before), 0);
	}
#endif
    }
    ಅನ್ಯಥಾ ಯದಿ (cc == 0)
	(ಶೂನ್ಯ)del_char_after_col(limit_col);
}

#ಯದಿ defined(FEAT_RIGHTLEFT)
/*
 * Map Hebrew keyboard when in hkmap mode.
 */
    ಪೂರ್ಣಾಙ್ಕ
hkmap(ಪೂರ್ಣಾಙ್ಕ c)
{
    ಯದಿ (p_hkmapp)   // phonetic mapping, by Ilya Dogolazky
    {
	ಗಣನಾ {hALEF=0, BET, GIMEL, DALET, HEI, VAV, ZAIN, HET, TET, IUD,
	    KAFsofit, hKAF, LAMED, MEMsofit, MEM, NUNsofit, NUN, SAMEH, AIN,
	    PEIsofit, PEI, ZADIsofit, ZADI, KOF, RESH, hSHIN, TAV};
	ಸ್ಥಿರ char_u map[26] =
	    {(char_u)hALEF/*a*/, (char_u)BET  /*b*/, (char_u)hKAF    /*c*/,
	     (char_u)DALET/*d*/, (char_u)-1   /*e*/, (char_u)PEIsofit/*f*/,
	     (char_u)GIMEL/*g*/, (char_u)HEI  /*h*/, (char_u)IUD     /*i*/,
	     (char_u)HET  /*j*/, (char_u)KOF  /*k*/, (char_u)LAMED   /*l*/,
	     (char_u)MEM  /*m*/, (char_u)NUN  /*n*/, (char_u)SAMEH   /*o*/,
	     (char_u)PEI  /*p*/, (char_u)-1   /*q*/, (char_u)RESH    /*r*/,
	     (char_u)ZAIN /*s*/, (char_u)TAV  /*t*/, (char_u)TET     /*u*/,
	     (char_u)VAV  /*v*/, (char_u)hSHIN/*w*/, (char_u)-1      /*x*/,
	     (char_u)AIN  /*y*/, (char_u)ZADI /*z*/};

	ಯದಿ (c == 'N' || c == 'M' || c == 'P' || c == 'C' || c == 'Z')
	    ಪ್ರತ್ಯಾಗಮ (ಪೂರ್ಣಾಙ್ಕ)(map[CharOrd(c)] - 1 + p_aleph);
							    // '-1'='sofit'
	ಅನ್ಯಥಾ ಯದಿ (c == 'x')
	    ಪ್ರತ್ಯಾಗಮ 'X';
	ಅನ್ಯಥಾ ಯದಿ (c == 'q')
	    ಪ್ರತ್ಯಾಗಮ '\''; // {geresh}={'}
	ಅನ್ಯಥಾ ಯದಿ (c == 246)
	    ಪ್ರತ್ಯಾಗಮ ' ';  // \"o --> ' ' for a german keyboard
	ಅನ್ಯಥಾ ಯದಿ (c == 228)
	    ಪ್ರತ್ಯಾಗಮ ' ';  // \"a --> ' '      -- / --
	ಅನ್ಯಥಾ ಯದಿ (c == 252)
	    ಪ್ರತ್ಯಾಗಮ ' ';  // \"u --> ' '      -- / --
	// NOTE: islower() does not do the right thing for us on Linux so we
	// do this the same was as 5.7 and previous, so it works correctly on
	// all systems.  Specifically, the e.g. Delete and Arrow keys are
	// munged and won't work if e.g. searching for Hebrew text.
	ಅನ್ಯಥಾ ಯದಿ (c >= 'a' && c <= 'z')
	    ಪ್ರತ್ಯಾಗಮ (ಪೂರ್ಣಾಙ್ಕ)(map[Char_Ord_ಕೆಳಗಿನ(c)] + p_aleph);
	ಅನ್ಯಥಾ
	    ಪ್ರತ್ಯಾಗಮ c;
    }
    ಅನ್ಯಥಾ
    {
	ವಿಕಲ್ಪ (c)
	{
	    ಪಕ್ಷ '`':	ಪ್ರತ್ಯಾಗಮ ';';
	    ಪಕ್ಷ '/':	ಪ್ರತ್ಯಾಗಮ '.';
	    ಪಕ್ಷ '\'':	ಪ್ರತ್ಯಾಗಮ ',';
	    ಪಕ್ಷ 'q':	ಪ್ರತ್ಯಾಗಮ '/';
	    ಪಕ್ಷ 'w':	ಪ್ರತ್ಯಾಗಮ '\'';

			// Hebrew letters - set offset from 'a'
	    ಪಕ್ಷ ',':	c = '{'; ಭಙ್ಗ;
	    ಪಕ್ಷ '.':	c = 'v'; ಭಙ್ಗ;
	    ಪಕ್ಷ ';':	c = 't'; ಭಙ್ಗ;
	    ಸ್ವಭಾವ: {
			 ಸ್ಥಿರ ವರ್ಣ str[] = "zqbcxlsjphmkwonu ydafe rig";

			 ಯದಿ (c < 'a' || c > 'z')
			     ಪ್ರತ್ಯಾಗಮ c;
			 c = str[Char_Ord_ಕೆಳಗಿನ(c)];
			 ಭಙ್ಗ;
		     }
	}

	ಪ್ರತ್ಯಾಗಮ (ಪೂರ್ಣಾಙ್ಕ)(Char_Ord_ಕೆಳಗಿನ(c) + p_aleph);
    }
}
#endif

    ಸ್ಥಿರ ಶೂನ್ಯ
ins_reg(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ		need_redraw = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		regname;
    ಪೂರ್ಣಾಙ್ಕ		literally = 0;
    ಪೂರ್ಣಾಙ್ಕ		vis_active = VIsual_active;

    /*
     * If we are going to wait for a character, show a '"'.
     */
    pc_status = PC_STATUS_UNSET;
    ಯದಿ (redrawing() && !char_avail())
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
    ++allow_ಕೀಲಿಗಳು;
    regname = plain_vgetc();
    LANGMAP_ADJUST(regname, TRUE);
    ಯದಿ (regname == Ctrl_R || regname == Ctrl_O || regname == Ctrl_P)
    {
	// Get a third key for literal register insertion
	literally = regname;
	add_to_showcmd_c(literally);
	regname = plain_vgetc();
	LANGMAP_ADJUST(regname, TRUE);
    }
    --no_mapping;
    --allow_ಕೀಲಿಗಳು;

#ifdef FEAT_EVAL
    // Don't call u_sync() while typing the expression or giving an error
    // message for it. Only call it explicitly.
    ++no_u_sync;
    ಯದಿ (regname == '=')
    {
	pos_T	curpos = curwin->w_cursor;
# ifdef HAVE_ಒಳಹರಿವು_METHOD
	ಪೂರ್ಣಾಙ್ಕ	im_on = im_get_status();
# endif
	// Sync undo when evaluating the expression calls setline() or
	// append(), so that it can be undone separately.
	u_sync_once = 2;

	regname = get_ಅಭಿವ್ಯಕ್ತಿ_register();

	// Cursor may be moved back a column.
	curwin->w_cursor = curpos;
	check_cursor();
# ifdef HAVE_ಒಳಹರಿವು_METHOD
	// Restore the Input Method.
	ಯದಿ (im_on)
	    im_set_active(TRUE);
# endif
    }
    ಯದಿ (regname == NUL || !valid_yank_reg(regname, FALSE))
    {
	vim_beep(BO_REG);
	need_redraw = TRUE;	// remove the '"'
    }
    ಅನ್ಯಥಾ
    {
#endif
	ಯದಿ (literally == Ctrl_O || literally == Ctrl_P)
	{
	    // Append the command to the redo buffer.
	    AppendCharToRedobuff(Ctrl_R);
	    AppendCharToRedobuff(literally);
	    AppendCharToRedobuff(regname);

	    do_put(regname, ರಿಕ್ತ, BACKWARD, 1L,
		 (literally == Ctrl_P ? PUT_FIXINDENT : 0) | PUT_CURSEND);
	}
	ಅನ್ಯಥಾ ಯದಿ (ಸೇರಿಸು_reg(regname, literally) == FAIL)
	{
	    vim_beep(BO_REG);
	    need_redraw = TRUE;	// remove the '"'
	}
	ಅನ್ಯಥಾ ಯದಿ (stop_ಸೇರಿಸು_mode)
	    // When the '=' register was used and a function was invoked that
	    // did ":stopinsert" then stuff_empty() returns FALSE but we won't
	    // insert anything, need to remove the '"'
	    need_redraw = TRUE;

#ifdef FEAT_EVAL
    }
    --no_u_sync;
    ಯದಿ (u_sync_once == 1)
	ins_need_undo = TRUE;
    u_sync_once = 0;
#endif
    clear_showcmd();

    // If the inserted register is empty, we need to remove the '"'
    ಯದಿ (need_redraw || stuff_empty())
	edit_unputchar();

    // Disallow starting Visual mode here, would get a weird mode.
    ಯದಿ (!vis_active && VIsual_active)
	end_visual_mode();
}

/*
 * CTRL-G commands in Insert mode.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ins_ctrl_g(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ		c;

    // Right after CTRL-X the cursor will be after the ruler.
    setcursor();

    /*
     * Don't map the second key. This also prevents the mode message to be
     * deleted when ESC is hit.
     */
    ++no_mapping;
    ++allow_ಕೀಲಿಗಳು;
    c = plain_vgetc();
    --no_mapping;
    --allow_ಕೀಲಿಗಳು;
    ವಿಕಲ್ಪ (c)
    {
	// CTRL-G k and CTRL-G <Up>: cursor up to Insstart.col
	ಪಕ್ಷ K_UP:
	ಪಕ್ಷ Ctrl_K:
	ಪಕ್ಷ 'k': ins_up(TRUE);
		  ಭಙ್ಗ;

	// CTRL-G j and CTRL-G <Down>: cursor down to Insstart.col
	ಪಕ್ಷ K_DOWN:
	ಪಕ್ಷ Ctrl_J:
	ಪಕ್ಷ 'j': ins_down(TRUE);
		  ಭಙ್ಗ;

	// CTRL-G u: start new undoable edit
	ಪಕ್ಷ 'u': u_sync(TRUE);
		  ins_need_undo = TRUE;

		  // Need to reset Insstart, esp. because a BS that joins
		  // a line to the previous one must save for undo.
		  update_Insstart_orig = FALSE;
		  Insstart = curwin->w_cursor;
		  ಭಙ್ಗ;

	// CTRL-G U: do not break undo with the next char
	ಪಕ್ಷ 'U':
		  // Allow one left/right cursor movement with the next char,
		  // without breaking undo.
		  dont_sync_undo = MAYBE;
		  ಭಙ್ಗ;

	ಪಕ್ಷ ESC:
		  // Esc after CTRL-G cancels it.
		  ಭಙ್ಗ;

	// Unknown CTRL-G command, reserved for future expansion.
	ಸ್ವಭಾವ:  vim_beep(BO_CTRLG);
    }
}

/*
 * CTRL-^ in Insert mode.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ins_ctrl_hat(ಶೂನ್ಯ)
{
    ಯದಿ (map_to_exists_mode((char_u *)"", MODE_LANGMAP, FALSE))
    {
	// ":lmap" mappings exists, Toggle use of ":lmap" mappings.
	ಯದಿ (State & MODE_LANGMAP)
	{
	    curbuf->b_p_iminsert = B_IMODE_NONE;
	    State &= ~MODE_LANGMAP;
	}
	ಅನ್ಯಥಾ
	{
	    curbuf->b_p_iminsert = B_IMODE_LMAP;
	    State |= MODE_LANGMAP;
#ifdef HAVE_ಒಳಹರಿವು_METHOD
	    im_set_active(FALSE);
#endif
	}
    }
#ifdef HAVE_ಒಳಹರಿವು_METHOD
    ಅನ್ಯಥಾ
    {
	// There are no ":lmap" mappings, toggle IM
	ಯದಿ (im_get_status())
	{
	    curbuf->b_p_iminsert = B_IMODE_NONE;
	    im_set_active(FALSE);
	}
	ಅನ್ಯಥಾ
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
    ಯದಿ (gui.in_use)
	gui_update_cursor(TRUE, FALSE);
#endif
#ಯದಿ defined(FEAT_KEYMAP)
    // Show/unshow value of 'keymap' in status lines.
    status_redraw_curbuf();
#endif
}

/*
 * Handle ESC in insert mode.
 * Returns TRUE when leaving insert mode, FALSE when going to repeat the
 * insert.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
ins_esc(
    long	*ಎಣಿಕೆ,
    ಪೂರ್ಣಾಙ್ಕ		cmdchar,
    ಪೂರ್ಣಾಙ್ಕ		nomove)	    // don't move cursor
{
    ಪೂರ್ಣಾಙ್ಕ		temp;
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	disabled_redraw = FALSE;
#ifdef FEAT_CONCEAL
    // Remember if the cursor line was concealed before changing State.
    ಪೂರ್ಣಾಙ್ಕ		cursor_ಸಾಲು_was_concealed = curwin->w_p_cole > 0
						&& conceal_cursor_ಸಾಲು(curwin);
#endif

#ifdef FEAT_SPELL
    check_spell_redraw();
#endif

    temp = curwin->w_cursor.col;
    ಯದಿ (disabled_redraw)
    {
	ಯದಿ (RedrawingDisabled > 0)
	    --RedrawingDisabled;
	disabled_redraw = FALSE;
    }
    ಯದಿ (!arrow_used)
    {
	/*
	 * Don't append the ESC for "r<CR>" and "grx".
	 * When 'insertmode' is set only CTRL-L stops Insert mode.  Needed for
	 * when "count" is non-zero.
	 */
	ಯದಿ (cmdchar != 'r' && cmdchar != 'v')
	    AppendToRedobuff(p_im ? (char_u *)"\014" : ESC_STR);

	/*
	 * Repeating insert may take a long time.  Check for
	 * interrupt now and then.
	 */
	ಯದಿ (*ಎಣಿಕೆ > 0)
	{
	    ಸಾಲು_breakcheck();
	    ಯದಿ (got_int)
		*ಎಣಿಕೆ = 0;
	}

	ಯದಿ (--*ಎಣಿಕೆ > 0)	// repeat what was typed
	{
	    // Vi repeats the insert without replacing characters.
	    ಯದಿ (vim_strchr(p_cpo, CPO_REPLCNT) != ರಿಕ್ತ)
		State &= ~REPLACE_FLAG;

	    (ಶೂನ್ಯ)start_redo_ins();
	    ಯದಿ (cmdchar == 'r' || cmdchar == 'v')
		stuffRedoReadbuff(ESC_STR);	// no ESC in redo buffer
	    ++RedrawingDisabled;
	    disabled_redraw = TRUE;
	    ಪ್ರತ್ಯಾಗಮ FALSE;	// repeat the insert
	}
	stop_ಸೇರಿಸು(&curwin->w_cursor, TRUE, nomove);
	undisplay_dollar();
    }

    ಯದಿ (cmdchar != 'r' && cmdchar != 'v')
	ins_apply_autocmds(EVENT_INSERTLEAVEPRE);

    // When an autoindent was removed, curswant stays after the
    // indent
    ಯದಿ (restart_edit == NUL && (colnr_T)temp == curwin->w_cursor.col)
	curwin->w_set_curswant = true;

    // Remember the last Insert position in the '^ mark.
    ಯದಿ ((cmdmod.cmod_flags & CMOD_KEEPJUMPS) == 0)
	curbuf->b_last_ಸೇರಿಸು = curwin->w_cursor;

    /*
     * The cursor should end up on the last inserted character.
     * Don't do it for CTRL-O, unless past the end of the line.
     */
    ಯದಿ (!nomove
	    && (curwin->w_cursor.col != 0
		|| curwin->w_cursor.coladd > 0)
	    && (restart_edit == NUL
		   || (gchar_cursor() == NUL && !VIsual_active))
#ifdef FEAT_RIGHTLEFT
	    && !revins_on
#endif
				      )
    {
	ಯದಿ (curwin->w_cursor.coladd > 0 || get_ve_flags() == VE_ALL)
	{
	    oneleft();
	    ಯದಿ (restart_edit != NUL)
		++curwin->w_cursor.coladd;
	}
	ಅನ್ಯಥಾ
	{
	    --curwin->w_cursor.col;
	    curwin->w_valid &= ~(VALID_WCOL|VALID_VIRTCOL);
	    // Correct cursor for multi-byte character.
	    ಯದಿ (has_mbyte)
		mb_adjust_cursor();
	}
    }

#ifdef HAVE_ಒಳಹರಿವು_METHOD
    // Disable IM to allow typing English directly for Normal mode commands.
    // When ":lmap" is enabled don't change 'iminsert' (IM can be enabled as
    // well).
    ಯದಿ (!(State & MODE_LANGMAP))
	im_save_status(&curbuf->b_p_iminsert);
    im_set_active(FALSE);
#endif

    State = MODE_NORMAL;
    may_trigger_modechanged();
    // need to position cursor again when on a TAB and when on a char with
    // virtual text.
    ಯದಿ (gchar_cursor() == TAB
#ifdef FEAT_PROP_POPUP
	    || curbuf->b_has_textprop
#endif
       )
	curwin->w_valid &= ~(VALID_WROW|VALID_WCOL|VALID_VIRTCOL);

    setmouse();
#ifdef CURSOR_SHAPE
    ui_cursor_shape();		// may show different cursor shape
#endif
    ಯದಿ (!p_ek)
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
    conceal_check_cursor_ಸಾಲು(cursor_ಸಾಲು_was_concealed);
#endif

    // When recording or for CTRL-O, need to display the new mode.
    // Otherwise remove the mode message.
    ಯದಿ (reg_recording != 0 || restart_edit != NUL)
	showmode();
    ಅನ್ಯಥಾ ಯದಿ (p_smd && (got_int || !skip_showmode()))
	ಸಂದೇಶ("");

    ಪ್ರತ್ಯಾಗಮ TRUE;	    // exit Insert mode
}

#ifdef FEAT_RIGHTLEFT
/*
 * Toggle language: hkmap and revins_on.
 * Move to end of reverse inserted text.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ins_ctrl_(ಶೂನ್ಯ)
{
    ಯದಿ (revins_on && revins_chars && revins_scol >= 0)
    {
	ಯಾವತ್ (gchar_cursor() != NUL && revins_chars--)
	    ++curwin->w_cursor.col;
    }
    p_ri = !p_ri;
    revins_on = (State == MODE_ಸೇರಿಸು && p_ri);
    ಯದಿ (revins_on)
    {
	revins_scol = curwin->w_cursor.col;
	revins_legal++;
	revins_chars = 0;
	undisplay_dollar();
    }
    ಅನ್ಯಥಾ
	revins_scol = -1;
    p_hkmap = curwin->w_p_rl ^ p_ri;    // be consistent!
    showmode();
}
#endif

/*
 * If 'keymodel' contains "startsel", may start selection.
 * Returns TRUE when a CTRL-O and other keys stuffed.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
ins_start_select(ಪೂರ್ಣಾಙ್ಕ c)
{
    ಯದಿ (!km_startsel)
	ಪ್ರತ್ಯಾಗಮ FALSE;
    ವಿಕಲ್ಪ (c)
    {
	ಪಕ್ಷ K_KHOME:
	ಪಕ್ಷ K_KEND:
	ಪಕ್ಷ K_PAGEUP:
	ಪಕ್ಷ K_KPAGEUP:
	ಪಕ್ಷ K_PAGEDOWN:
	ಪಕ್ಷ K_KPAGEDOWN:
#ifdef MACOS_X
	ಪಕ್ಷ K_ಎಡ:
	ಪಕ್ಷ K_ಬಲ:
	ಪಕ್ಷ K_UP:
	ಪಕ್ಷ K_DOWN:
	ಪಕ್ಷ K_END:
	ಪಕ್ಷ K_HOME:
#endif
	    ಯದಿ (!(mod_mask & MOD_MASK_SHIFT))
		ಭಙ್ಗ;
	    // FALLTHROUGH
	ಪಕ್ಷ K_S_ಎಡ:
	ಪಕ್ಷ K_S_ಬಲ:
	ಪಕ್ಷ K_S_UP:
	ಪಕ್ಷ K_S_DOWN:
	ಪಕ್ಷ K_S_END:
	ಪಕ್ಷ K_S_HOME:
	    // Start selection right away, the cursor can move with CTRL-O when
	    // beyond the end of the line.
	    start_selection();

	    // Execute the key in (insert) Select mode.
	    stuffcharReadbuff(Ctrl_O);
	    ಯದಿ (mod_mask)
	    {
		char_u	    buf[4];

		buf[0] = K_SPECIAL;
		buf[1] = KS_MODIFIER;
		buf[2] = mod_mask;
		buf[3] = NUL;
		stuffReadbuffLen(buf, 3L);
	    }
	    stuffcharReadbuff(c);
	    ಪ್ರತ್ಯಾಗಮ TRUE;
    }
    ಪ್ರತ್ಯಾಗಮ FALSE;
}

/*
 * <Insert> key in Insert mode: toggle insert/replace mode.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ins_ಸೇರಿಸು(ಪೂರ್ಣಾಙ್ಕ replaceState)
{
#ifdef FEAT_EVAL
    set_vim_var_string(VV_INSERTMODE,
		   (char_u *)((State & REPLACE_FLAG) ? "i"
			    : replaceState == MODE_VREPLACE ? "v" : "r"), 1);
#endif
    ins_apply_autocmds(EVENT_INSERTCHANGE);
    ಯದಿ (State & REPLACE_FLAG)
	State = MODE_ಸೇರಿಸು | (State & MODE_LANGMAP);
    ಅನ್ಯಥಾ
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
    ಸ್ಥಿರ ಶೂನ್ಯ
ins_ctrl_o(ಶೂನ್ಯ)
{
    restart_VIsual_select = 0;
    ಯದಿ (State & VREPLACE_FLAG)
	restart_edit = 'V';
    ಅನ್ಯಥಾ ಯದಿ (State & REPLACE_FLAG)
	restart_edit = 'R';
    ಅನ್ಯಥಾ
	restart_edit = 'I';
    ಯದಿ (virtual_active())
	ins_at_eol = FALSE;	// cursor always keeps its column
    ಅನ್ಯಥಾ
	ins_at_eol = (gchar_cursor() == NUL);
}

/*
 * If the cursor is on an indent, ^T/^D insert/delete one
 * shiftwidth.	Otherwise ^T/^D behave like a "<<" or ">>".
 * Always round the indent to 'shiftwidth', this is compatible
 * with vi.  But vi only supports ^T and ^D after an
 * autoindent, we support it everywhere.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ins_shift(ಪೂರ್ಣಾಙ್ಕ c, ಪೂರ್ಣಾಙ್ಕ lastc)
{
    ಯದಿ (stop_arrow() == FAIL)
	ಪ್ರತ್ಯಾಗಮ;
    AppendCharToRedobuff(c);

    /*
     * 0^D and ^^D: remove all indent.
     */
    ಯದಿ (c == Ctrl_D && (lastc == '0' || lastc == '^')
						  && curwin->w_cursor.col > 0)
    {
	--curwin->w_cursor.col;
	(ಶೂನ್ಯ)del_char(FALSE);		// delete the '^' or '0'
	// In Replace mode, restore the characters that '^' or '0' replaced.
	ಯದಿ (State & REPLACE_FLAG)
	    replace_ತೆಗೆ_ins();
	ಯದಿ (lastc == '^')
	    old_indent = get_indent();	// remember curr. indent
	change_indent(INDENT_SET, 0, TRUE, 0, TRUE);
    }
    ಅನ್ಯಥಾ
	change_indent(c == Ctrl_D ? INDENT_DEC : INDENT_INC, 0, TRUE, 0, TRUE);

    ಯದಿ (did_ai && *skipwhite(ml_get_curline()) != NUL)
	did_ai = FALSE;
    did_si = FALSE;
    can_si = FALSE;
    can_si_back = FALSE;
    can_cindent = FALSE;	// no cindenting after ^D or ^T
}

    ಸ್ಥಿರ ಶೂನ್ಯ
ins_del(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ	    temp;

    ಯದಿ (stop_arrow() == FAIL)
	ಪ್ರತ್ಯಾಗಮ;
    ಯದಿ (gchar_cursor() == NUL)		// delete newline
    {
	temp = curwin->w_cursor.col;
	ಯದಿ (!can_bs(BS_EOL)		// only if "eol" included
		|| do_join(2, FALSE, TRUE, FALSE, FALSE) == FAIL)
	    vim_beep(BO_BS);
	ಅನ್ಯಥಾ
	{
	    curwin->w_cursor.col = temp;
	    // Adjust orig_line_count in case more lines have been deleted than
	    // have been added. That makes sure, that open_line() later
	    // can access all buffer lines correctly
	    ಯದಿ (State & VREPLACE_FLAG &&
		    orig_ಸಾಲು_ಎಣಿಕೆ > curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
		orig_ಸಾಲು_ಎಣಿಕೆ = curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
	}
    }
    ಅನ್ಯಥಾ ಯದಿ (del_char(FALSE) == FAIL)  // delete char under cursor
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
    ಸ್ಥಿರ ಶೂನ್ಯ
ins_bs_one(ಶೂನ್ಯ)
{
    dec_cursor();
    ಯದಿ (State & REPLACE_FLAG)
    {
	// Don't delete characters before the insert point when in
	// Replace mode
	ಯದಿ (curwin->w_cursor.lnum != Insstart.lnum
		|| curwin->w_cursor.col >= Insstart.col)
	    replace_do_bs(-1);
    }
    ಅನ್ಯಥಾ
	(ಶೂನ್ಯ)del_char(FALSE);
}

/*
 * Handle Backspace, delete-word and delete-line in Insert mode.
 * Return TRUE when backspace was actually used.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
ins_bs(
    ಪೂರ್ಣಾಙ್ಕ		c,
    ಪೂರ್ಣಾಙ್ಕ		mode,
    ಪೂರ್ಣಾಙ್ಕ		*inserted_space_p)
{
    linenr_T	lnum;
    ಪೂರ್ಣಾಙ್ಕ		cc;
    ಪೂರ್ಣಾಙ್ಕ		temp = 0;	    // init for GCC
    colnr_T	save_col;
    colnr_T	mincol;
    ಪೂರ್ಣಾಙ್ಕ		did_backspace = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		in_indent;
    ಪೂರ್ಣಾಙ್ಕ		oldState;
    ಪೂರ್ಣಾಙ್ಕ		cpc[MAX_MCO];	    // composing characters
    ಪೂರ್ಣಾಙ್ಕ		call_fix_indent = FALSE;

    /*
     * can't delete anything in an empty file
     * can't backup past first character in buffer
     * can't backup past starting point unless 'backspace' > 1
     * can backup to a previous line if 'backspace' == 0
     */
    ಯದಿ (       BUFEMPTY()
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
	ಪ್ರತ್ಯಾಗಮ FALSE;
    }

    ಯದಿ (stop_arrow() == FAIL)
	ಪ್ರತ್ಯಾಗಮ FALSE;
    in_indent = inindent(0);
    ಯದಿ (in_indent)
	can_cindent = FALSE;
    end_comment_pending = NUL;	// After BS, don't auto-end comment
#ifdef FEAT_RIGHTLEFT
    ಯದಿ (revins_on)	    // put cursor after last inserted char
	inc_cursor();
#endif

    // Virtualedit:
    //	BACKSPACE_CHAR eats a virtual space
    //	BACKSPACE_WORD eats all coladd
    //	BACKSPACE_LINE eats all coladd and keeps going
    ಯದಿ (curwin->w_cursor.coladd > 0)
    {
	ಯದಿ (mode == BACKSPACE_CHAR)
	{
	    --curwin->w_cursor.coladd;
	    ಪ್ರತ್ಯಾಗಮ TRUE;
	}
	ಯದಿ (mode == BACKSPACE_WORD)
	{
	    curwin->w_cursor.coladd = 0;
	    ಪ್ರತ್ಯಾಗಮ TRUE;
	}
	curwin->w_cursor.coladd = 0;
    }

    /*
     * Delete newline!
     */
    ಯದಿ (curwin->w_cursor.col == 0)
    {
	lnum = Insstart.lnum;
	ಯದಿ (curwin->w_cursor.lnum == lnum
#ifdef FEAT_RIGHTLEFT
			|| revins_on
#endif
				    )
	{
	    ಯದಿ (u_save((linenr_T)(curwin->w_cursor.lnum - 2),
			       (linenr_T)(curwin->w_cursor.lnum + 1)) == FAIL)
		ಪ್ರತ್ಯಾಗಮ FALSE;
	    --Insstart.lnum;
	    Insstart.col = ml_get_len(Insstart.lnum);
	}
	/*
	 * In replace mode:
	 * cc < 0: NL was inserted, delete it
	 * cc >= 0: NL was replaced, put original characters back
	 */
	cc = -1;
	ಯದಿ (State & REPLACE_FLAG)
	    cc = replace_ತೆಗೆ();	    // returns -1 if NL was inserted
	/*
	 * In replace mode, in the line we started replacing, we only move the
	 * cursor.
	 */
	ಯದಿ ((State & REPLACE_FLAG) && curwin->w_cursor.lnum <= lnum)
	{
	    dec_cursor();
	}
	ಅನ್ಯಥಾ
	{
	    ಯದಿ (!(State & VREPLACE_FLAG)
				   || curwin->w_cursor.lnum > orig_ಸಾಲು_ಎಣಿಕೆ)
	    {
		temp = gchar_cursor();	// remember current char
		--curwin->w_cursor.lnum;

		// When "aw" is in 'formatoptions' we must delete the space at
		// the end of the line, otherwise the line will be broken
		// again when auto-formatting.
		ಯದಿ (has_format_option(FO_AUTO)
					   && has_format_option(FO_WHITE_PAR))
		{
		    char_u  *ptr = ml_get_buf(curbuf, curwin->w_cursor.lnum,
									FALSE);
		    ಪೂರ್ಣಾಙ್ಕ	    len = ml_get_curline_len();

		    ಯದಿ (len > 0 && ptr[len - 1] == ' ')
		    {
			char_u *newp = alloc(curbuf->b_ml.ml_ಸಾಲು_len - 1);

			ಯದಿ (newp != ರಿಕ್ತ)
			{
			    mch_memmove(newp, ptr, len - 1);
			    newp[len - 1] = NUL;
			    ಯದಿ (curbuf->b_ml.ml_ಸಾಲು_len > len + 1)
				mch_memmove(newp + len, ptr + len + 1,
					   curbuf->b_ml.ml_ಸಾಲು_len - len - 1);

			    ಯದಿ (curbuf->b_ml.ml_flags
					      & (ML_ಸಾಲು_DIRTY | ML_ALLOCATED))
				vim_free(curbuf->b_ml.ml_ಸಾಲು_ptr);
			    curbuf->b_ml.ml_ಸಾಲು_ptr = newp;
			    curbuf->b_ml.ml_ಸಾಲು_len--;
			    curbuf->b_ml.ml_ಸಾಲು_textlen--;
			    curbuf->b_ml.ml_flags |= ML_ಸಾಲು_DIRTY;
			}
		    }
		}

		(ಶೂನ್ಯ)do_join(2, FALSE, FALSE, FALSE, FALSE);
		ಯದಿ (temp == NUL && gchar_cursor() != NUL)
		    inc_cursor();
	    }
	    ಅನ್ಯಥಾ
		dec_cursor();

	    /*
	     * In MODE_REPLACE mode we have to put back the text that was
	     * replaced by the NL. On the replace stack is first a
	     * NUL-terminated sequence of characters that were deleted and then
	     * the characters that NL replaced.
	     */
	    ಯದಿ (State & REPLACE_FLAG)
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
		ಯಾವತ್ (cc > 0)
		{
		    save_col = curwin->w_cursor.col;
		    mb_replace_ತೆಗೆ_ins(cc);
		    curwin->w_cursor.col = save_col;
		    cc = replace_ತೆಗೆ();
		}
		// restore the characters that NL replaced
		replace_ತೆಗೆ_ins();
		State = oldState;
	    }
	}
	did_ai = FALSE;
    }
    ಅನ್ಯಥಾ
    {
	/*
	 * Delete character(s) before the cursor.
	 */
#ifdef FEAT_RIGHTLEFT
	ಯದಿ (revins_on)		// put cursor on last inserted char
	    dec_cursor();
#endif
	mincol = 0;
						// keep indent
	ಯದಿ (mode == BACKSPACE_ಸಾಲು
		&& (curbuf->b_p_ai || cindent_on())
#ifdef FEAT_RIGHTLEFT
		&& !revins_on
#endif
			    )
	{
	    save_col = curwin->w_cursor.col;
	    beginline(BL_WHITE);
	    ಯದಿ (curwin->w_cursor.col < save_col)
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
	ಯದಿ (	   mode == BACKSPACE_CHAR
		&& ((p_sta && in_indent)
		    || ((get_sts_ಮೌಲ್ಯ() != 0
#ifdef FEAT_VARTABS
			|| tabstop_ಎಣಿಕೆ(curbuf->b_p_vsts_array)
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
	    char_u	*ಸಾಲು;
	    char_u	*ptr;
	    char_u	*cursor_ptr;
	    char_u	*space_ptr;
	    colnr_T	space_vcol = 0;
	    ಪೂರ್ಣಾಙ್ಕ		prev_space = FALSE;
	    colnr_T	want_col;

	    *inserted_space_p = FALSE;

	    space_ptr = ptr = ಸಾಲು = ml_get_curline();
	    cursor_ptr = ಸಾಲು + curwin->w_cursor.col;

	    // Compute virtual column of cursor position, and find the last
	    // whitespace before cursor that is preceded by non-whitespace.
	    // Use chartabsize() so that virtual text and wrapping are ignored.
	    ಯಾವತ್ (ptr < cursor_ptr)
	    {
		ಪೂರ್ಣಾಙ್ಕ	cur_space = VIM_ISWHITE(*ptr);

		ಯದಿ (!prev_space && cur_space)
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
	    ಯದಿ (p_sta && in_indent)
		want_vcol -= want_vcol % (ಪೂರ್ಣಾಙ್ಕ)get_sw_ಮೌಲ್ಯ(curbuf);
	    ಅನ್ಯಥಾ
#ifdef FEAT_VARTABS
		want_vcol = tabstop_start(want_vcol, get_sts_ಮೌಲ್ಯ(),
						       curbuf->b_p_vsts_array);
#ಅನ್ಯಥಾ
		want_vcol -= want_vcol % (ಪೂರ್ಣಾಙ್ಕ)get_sts_ಮೌಲ್ಯ();
#endif

	    // Find the position to stop backspacing.
	    // Use chartabsize() so that virtual text and wrapping are ignored.
	    ಯಾವತ್ (TRUE)
	    {
		ಪೂರ್ಣಾಙ್ಕ size = chartabsize(space_ptr, space_vcol);

		ಯದಿ (space_vcol + size > want_vcol)
		    ಭಙ್ಗ;
		space_vcol += size;
		MB_PTR_ADV(space_ptr);
	    }
	    want_col = space_ptr - ಸಾಲು;

	    // Delete characters until we are at or before want_col.
	    ಯಾವತ್ (curwin->w_cursor.col > want_col)
		ins_bs_one();

	    // Insert extra spaces until we are at want_vcol.
	    ಪ್ರತಿ (; space_vcol < want_vcol; space_vcol++)
	    {
		// Remember the first char we inserted
		ಯದಿ (curwin->w_cursor.lnum == Insstart_orig.lnum
				   && curwin->w_cursor.col < Insstart_orig.col)
		    Insstart_orig.col = curwin->w_cursor.col;

		ಯದಿ (State & VREPLACE_FLAG)
		    ins_char(' ');
		ಅನ್ಯಥಾ
		{
		    ins_str((char_u *)" ", 1);
		    ಯದಿ ((State & REPLACE_FLAG))
			replace_ತಳ್ಳು(NUL);
		}
	    }
	}

	/*
	 * Delete up to starting point, start of line or previous word.
	 */
	ಅನ್ಯಥಾ
	{
	    ಪೂರ್ಣಾಙ್ಕ cclass = 0, prev_cclass = 0;

	    ಯದಿ (has_mbyte)
		cclass = mb_get_class(ml_get_cursor());
	    ಕುರು
	    {
#ifdef FEAT_RIGHTLEFT
		ಯದಿ (!revins_on) // put cursor on char to be deleted
#endif
		    dec_cursor();

		cc = gchar_cursor();
		// look multi-byte character class
		ಯದಿ (has_mbyte)
		{
		    prev_cclass = cclass;
		    cclass = mb_get_class(ml_get_cursor());
		}

		// start of word?
		ಯದಿ (mode == BACKSPACE_WORD && !vim_isspace(cc))
		{
		    mode = BACKSPACE_WORD_NOT_SPACE;
		    temp = vim_iswordc(cc);
		}
		// end of word?
		ಅನ್ಯಥಾ ಯದಿ (mode == BACKSPACE_WORD_NOT_SPACE
			&& ((vim_isspace(cc) || vim_iswordc(cc) != temp)
			|| prev_cclass != cclass))
		{
#ifdef FEAT_RIGHTLEFT
		    ಯದಿ (!revins_on)
#endif
			inc_cursor();
#ifdef FEAT_RIGHTLEFT
		    ಅನ್ಯಥಾ ಯದಿ (State & REPLACE_FLAG)
			dec_cursor();
#endif
		    ಭಙ್ಗ;
		}
		ಯದಿ (State & REPLACE_FLAG)
		    replace_do_bs(-1);
		ಅನ್ಯಥಾ
		{
		    ಯದಿ (enc_utf8 && p_deco)
			(ಶೂನ್ಯ)utfc_ptr2char(ml_get_cursor(), cpc);
		    (ಶೂನ್ಯ)del_char(FALSE);
		    /*
		     * If there are combining characters and 'delcombine' is set
		     * move the cursor back.  Don't back up before the base
		     * character.
		     */
		    ಯದಿ (enc_utf8 && p_deco && cpc[0] != NUL)
			inc_cursor();
#ifdef FEAT_RIGHTLEFT
		    ಯದಿ (revins_chars)
		    {
			revins_chars--;
			revins_legal++;
		    }
		    ಯದಿ (revins_on && gchar_cursor() == NUL)
			ಭಙ್ಗ;
#endif
		}
		// Just a single backspace?:
		ಯದಿ (mode == BACKSPACE_CHAR)
		    ಭಙ್ಗ;
	    } ಯಾವತ್ (
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
    ಯದಿ (curwin->w_cursor.col <= 1)
	did_ai = FALSE;

    ಯದಿ (call_fix_indent)
	fix_indent();

    /*
     * It's a little strange to put backspaces into the redo
     * buffer, but it makes auto-indent a lot easier to deal
     * with.
     */
    AppendCharToRedobuff(c);

    // If deleted before the insertion point, adjust it
    ಯದಿ (curwin->w_cursor.lnum == Insstart_orig.lnum
				  && curwin->w_cursor.col < Insstart_orig.col)
	Insstart_orig.col = curwin->w_cursor.col;

    // vi behaviour: the cursor moves backward but the character that
    //		     was there remains visible
    // Vim behaviour: the cursor moves backward and the character that
    //		      was there is erased from the screen.
    // We can emulate the vi behaviour by pretending there is a dollar
    // displayed even when there isn't.
    //  --pkv Sun Jan 19 01:56:40 EST 2003
    ಯದಿ (vim_strchr(p_cpo, CPO_BACKSPACE) != ರಿಕ್ತ && dollar_vcol == -1)
	dollar_vcol = curwin->w_virtcol;

#ifdef FEAT_FOLDING
    // When deleting a char the cursor line must never be in a closed fold.
    // E.g., when 'foldmethod' is indent and deleting the first non-white
    // char before a Tab.
    ಯದಿ (did_backspace)
	foldOpenCursor();
#endif

    ಪ್ರತ್ಯಾಗಮ did_backspace;
}

/*
 * Handle receiving P_PS: start paste mode.  Inserts the following text up to
 * P_PE literally.
 * When "drop" is TRUE then consume the text and drop it.
 */
    ಪೂರ್ಣಾಙ್ಕ
bracketed_paste(paste_mode_T mode, ಪೂರ್ಣಾಙ್ಕ drop, garray_T *gap)
{
    ಪೂರ್ಣಾಙ್ಕ		c;
    char_u	buf[NUMBUFLEN + MB_MAXBYTES];
    ಪೂರ್ಣಾಙ್ಕ		idx = 0;
    char_u	*end = find_termcode((char_u *)"PE");
    ಪೂರ್ಣಾಙ್ಕ		ret_char = -1;
    ಪೂರ್ಣಾಙ್ಕ		save_allow_ಕೀಲಿಗಳು = allow_ಕೀಲಿಗಳು;
    ಪೂರ್ಣಾಙ್ಕ		save_paste = p_paste;

    // If the end code is too long we can't detect it, read everything.
    ಯದಿ (end != ರಿಕ್ತ && STRLEN(end) >= NUMBUFLEN)
	end = ರಿಕ್ತ;
    ++no_mapping;
    allow_ಕೀಲಿಗಳು = 0;
    ಯದಿ (!p_paste)
	// Also have the side effects of setting 'paste' to make it work much
	// faster.
	set_option_ಮೌಲ್ಯ_give_err((char_u *)"paste", TRUE, ರಿಕ್ತ, 0);

    ಪ್ರತಿ (;;)
    {
	// When the end is not defined read everything there is.
	ಯದಿ (end == ರಿಕ್ತ && vpeekc() == NUL)
	    ಭಙ್ಗ;
	ಕುರು
	    c = vgetc();
	ಯಾವತ್ (c == K_IGNORE || c == K_VER_SCROLLBAR || c == K_HOR_SCROLLBAR);

	ಯದಿ (c == NUL || got_int || (ex_normal_busy > 0 && c == Ctrl_C))
	    // When CTRL-C was encountered the typeahead will be flushed and we
	    // won't get the end sequence.  Except when using ":normal".
	    ಭಙ್ಗ;

	ಯದಿ (has_mbyte)
	    idx += (*mb_char2bytes)(c, buf + idx);
	ಅನ್ಯಥಾ
	    buf[idx++] = c;
	buf[idx] = NUL;
	ಯದಿ (end != ರಿಕ್ತ && STRNCMP(buf, end, idx) == 0)
	{
	    ಯದಿ (end[idx] == NUL)
		ಭಙ್ಗ; // Found the end of paste code.
	    ಅನುವರ್ತ;
	}
	ಯದಿ (!drop)
	{
	    ವಿಕಲ್ಪ (mode)
	    {
		ಪಕ್ಷ PASTE_CMDLINE:
		    put_on_cmdline(buf, idx, TRUE);
		    ಭಙ್ಗ;

		ಪಕ್ಷ PASTE_EX:
		    // add one for the NUL that is going to be appended
		    ಯದಿ (gap != ರಿಕ್ತ && ga_grow(gap, idx + 1) == OK)
		    {
			mch_memmove((ವರ್ಣ *)gap->ga_data + gap->ga_len,
							     buf, (size_t)idx);
			gap->ga_len += idx;
		    }
		    ಭಙ್ಗ;

		ಪಕ್ಷ PASTE_ಸೇರಿಸು:
		    ಯದಿ (stop_arrow() == OK)
		    {
			c = buf[0];
			ಯದಿ (idx == 1 && (c == CAR || c == K_KENTER || c == NL))
			    ins_eol(c);
			ಅನ್ಯಥಾ
			{
			    ins_char_bytes(buf, idx);
			    AppendToRedobuffLit(buf, idx);
			}
		    }
		    ಭಙ್ಗ;

		ಪಕ್ಷ PASTE_ONE_CHAR:
		    ಯದಿ (ret_char == -1)
		    {
			ಯದಿ (has_mbyte)
			    ret_char = (*mb_ptr2char)(buf);
			ಅನ್ಯಥಾ
			    ret_char = buf[0];
		    }
		    ಭಙ್ಗ;
	    }
	}
	idx = 0;
    }

    --no_mapping;
    allow_ಕೀಲಿಗಳು = save_allow_ಕೀಲಿಗಳು;
    ಯದಿ (!save_paste)
	set_option_ಮೌಲ್ಯ_give_err((char_u *)"paste", FALSE, ರಿಕ್ತ, 0);

    ಪ್ರತ್ಯಾಗಮ ret_char;
}

#ಯದಿ defined(FEAT_GUI_TABLINE)
    ಸ್ಥಿರ ಶೂನ್ಯ
ins_tabline(ಪೂರ್ಣಾಙ್ಕ c)
{
    // We will be leaving the current window, unless closing another tab.
    ಯದಿ (c != K_TABMENU || current_tabmenu != TABLINE_MENU_CLOSE
		|| (current_tab != 0 && current_tab != tabpage_index(curtab)))
    {
	undisplay_dollar();
	start_arrow(&curwin->w_cursor);
	can_cindent = TRUE;
    }

    ಯದಿ (c == K_TABLINE)
	goto_tabpage(current_tab);
    ಅನ್ಯಥಾ
    {
	handle_tabmenu();
	redraw_statuslines();	// will redraw the tabline when needed
    }
}
#endif

#ಯದಿ defined(FEAT_GUI)
    ಶೂನ್ಯ
ins_scroll(ಶೂನ್ಯ)
{
    pos_T	tpos;

    undisplay_dollar();
    tpos = curwin->w_cursor;
    ಯದಿ (gui_do_scroll())
    {
	start_arrow(&tpos);
	can_cindent = TRUE;
    }
}

    ಶೂನ್ಯ
ins_horscroll(ಶೂನ್ಯ)
{
    pos_T	tpos;

    undisplay_dollar();
    tpos = curwin->w_cursor;
    ಯದಿ (do_mousescroll_horiz(scrollbar_ಮೌಲ್ಯ))
    {
	start_arrow(&tpos);
	can_cindent = TRUE;
    }
}
#endif

    ಸ್ಥಿರ ಶೂನ್ಯ
ins_ಎಡ(ಶೂನ್ಯ)
{
    pos_T	tpos;
    ಪೂರ್ಣಾಙ್ಕ		end_change = dont_sync_undo == FALSE; // end undoable change

#ifdef FEAT_FOLDING
    ಯದಿ ((fdo_flags & FDO_HOR) && ಕೀಲಿ_Typed)
	foldOpenCursor();
#endif
    undisplay_dollar();
    tpos = curwin->w_cursor;
    ಯದಿ (oneleft() == OK)
    {
#ಯದಿ defined(FEAT_XIM) && defined(FEAT_GUI_GTK)
	// Only call start_arrow() when not busy with preediting, it will
	// break undo.  K_LEFT is inserted in im_correct_cursor().
	ಯದಿ (p_imst == IM_OVER_THE_SPOT || !im_is_preediting())
#endif
	{
	    start_arrow_with_change(&tpos, end_change);
	    ಯದಿ (!end_change)
		AppendCharToRedobuff(K_ಎಡ);
	}
#ifdef FEAT_RIGHTLEFT
	// If exit reversed string, position is fixed
	ಯದಿ (revins_scol != -1 && (ಪೂರ್ಣಾಙ್ಕ)curwin->w_cursor.col >= revins_scol)
	    revins_legal++;
	revins_chars++;
#endif
    }

    /*
     * if 'whichwrap' set for cursor in insert mode may go to
     * previous line
     */
    ಅನ್ಯಥಾ ಯದಿ (vim_strchr(p_ww, '[') != ರಿಕ್ತ && curwin->w_cursor.lnum > 1)
    {
	// always break undo when moving upwards/downwards, else undo may break
	start_arrow(&tpos);
	--(curwin->w_cursor.lnum);
	coladvance((colnr_T)MAXCOL);
	curwin->w_set_curswant = true;	// so we stay at the end
    }
    ಅನ್ಯಥಾ
	vim_beep(BO_CRSR);
    dont_sync_undo = FALSE;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
ins_home(ಪೂರ್ಣಾಙ್ಕ c)
{
    pos_T	tpos;

#ifdef FEAT_FOLDING
    ಯದಿ ((fdo_flags & FDO_HOR) && ಕೀಲಿ_Typed)
	foldOpenCursor();
#endif
    undisplay_dollar();
    tpos = curwin->w_cursor;
    ಯದಿ (c == K_C_HOME)
	curwin->w_cursor.lnum = 1;
    curwin->w_cursor.col = 0;
    curwin->w_cursor.coladd = 0;
    curwin->w_curswant = 0;
    start_arrow(&tpos);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
ins_end(ಪೂರ್ಣಾಙ್ಕ c)
{
    pos_T	tpos;

#ifdef FEAT_FOLDING
    ಯದಿ ((fdo_flags & FDO_HOR) && ಕೀಲಿ_Typed)
	foldOpenCursor();
#endif
    undisplay_dollar();
    tpos = curwin->w_cursor;
    ಯದಿ (c == K_C_END)
	curwin->w_cursor.lnum = curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
    coladvance((colnr_T)MAXCOL);
    curwin->w_curswant = MAXCOL;

    start_arrow(&tpos);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
ins_s_ಎಡ(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ end_change = dont_sync_undo == FALSE; // end undoable change
#ifdef FEAT_FOLDING
    ಯದಿ ((fdo_flags & FDO_HOR) && ಕೀಲಿ_Typed)
	foldOpenCursor();
#endif
    undisplay_dollar();
    ಯದಿ (curwin->w_cursor.lnum > 1 || curwin->w_cursor.col > 0)
    {
	start_arrow_with_change(&curwin->w_cursor, end_change);
	ಯದಿ (!end_change)
	    AppendCharToRedobuff(K_S_ಎಡ);
	(ಶೂನ್ಯ)bck_word(1L, FALSE, FALSE);
	curwin->w_set_curswant = true;
    }
    ಅನ್ಯಥಾ
	vim_beep(BO_CRSR);
    dont_sync_undo = FALSE;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
ins_ಬಲ(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ end_change = dont_sync_undo == FALSE; // end undoable change

#ifdef FEAT_FOLDING
    ಯದಿ ((fdo_flags & FDO_HOR) && ಕೀಲಿ_Typed)
	foldOpenCursor();
#endif
    undisplay_dollar();
    ಯದಿ (gchar_cursor() != NUL || virtual_active())
    {
	start_arrow_with_change(&curwin->w_cursor, end_change);
	ಯದಿ (!end_change)
	    AppendCharToRedobuff(K_ಬಲ);
	curwin->w_set_curswant = true;
	ಯದಿ (virtual_active())
	    oneright();
	ಅನ್ಯಥಾ
	{
	    ಯದಿ (has_mbyte)
		curwin->w_cursor.col += (*mb_ptr2len)(ml_get_cursor());
	    ಅನ್ಯಥಾ
		++curwin->w_cursor.col;
	}

#ifdef FEAT_RIGHTLEFT
	revins_legal++;
	ಯದಿ (revins_chars)
	    revins_chars--;
#endif
    }
    // if 'whichwrap' set for cursor in insert mode, may move the
    // cursor to the next line
    ಅನ್ಯಥಾ ಯದಿ (vim_strchr(p_ww, ']') != ರಿಕ್ತ
	    && curwin->w_cursor.lnum < curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
    {
	start_arrow(&curwin->w_cursor);
	curwin->w_set_curswant = true;
	++curwin->w_cursor.lnum;
	curwin->w_cursor.col = 0;
    }
    ಅನ್ಯಥಾ
	vim_beep(BO_CRSR);
    dont_sync_undo = FALSE;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
ins_s_ಬಲ(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ end_change = dont_sync_undo == FALSE; // end undoable change
#ifdef FEAT_FOLDING
    ಯದಿ ((fdo_flags & FDO_HOR) && ಕೀಲಿ_Typed)
	foldOpenCursor();
#endif
    undisplay_dollar();
    ಯದಿ (curwin->w_cursor.lnum < curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ
	    || gchar_cursor() != NUL)
    {
	start_arrow_with_change(&curwin->w_cursor, end_change);
	ಯದಿ (!end_change)
	    AppendCharToRedobuff(K_S_ಬಲ);
	(ಶೂನ್ಯ)fwd_word(1L, FALSE, 0);
	curwin->w_set_curswant = true;
    }
    ಅನ್ಯಥಾ
	vim_beep(BO_CRSR);
    dont_sync_undo = FALSE;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
ins_up(
    ಪೂರ್ಣಾಙ್ಕ		startcol)	// when TRUE move to Insstart.col
{
    pos_T	tpos;
    linenr_T	old_topline = curwin->w_topline;
#ifdef FEAT_DIFF
    ಪೂರ್ಣಾಙ್ಕ		old_topfill = curwin->w_topfill;
#endif

    undisplay_dollar();
    tpos = curwin->w_cursor;
    ಯದಿ (cursor_up(1L, TRUE) == OK)
    {
	ಯದಿ (startcol)
	    coladvance(getvcol_nolist(&Insstart));
	ಯದಿ (old_topline != curwin->w_topline
#ifdef FEAT_DIFF
		|| old_topfill != curwin->w_topfill
#endif
		)
	    redraw_later(UPD_VALID);
	start_arrow(&tpos);
	can_cindent = TRUE;
    }
    ಅನ್ಯಥಾ
	vim_beep(BO_CRSR);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
ins_pageup(ಶೂನ್ಯ)
{
    pos_T	tpos;

    undisplay_dollar();

    ಯದಿ (mod_mask & MOD_MASK_CTRL)
    {
	// <C-PageUp>: tab page back
	ಯದಿ (first_tabpage->tp_next != ರಿಕ್ತ)
	{
	    start_arrow(&curwin->w_cursor);
	    goto_tabpage(-1);
	}
	ಪ್ರತ್ಯಾಗಮ;
    }

    tpos = curwin->w_cursor;
    ಯದಿ (pagescroll(BACKWARD, 1L, FALSE) == OK)
    {
	start_arrow(&tpos);
	can_cindent = TRUE;
    }
    ಅನ್ಯಥಾ
	vim_beep(BO_CRSR);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
ins_down(
    ಪೂರ್ಣಾಙ್ಕ		startcol)	// when TRUE move to Insstart.col
{
    pos_T	tpos;
    linenr_T	old_topline = curwin->w_topline;
#ifdef FEAT_DIFF
    ಪೂರ್ಣಾಙ್ಕ		old_topfill = curwin->w_topfill;
#endif

    undisplay_dollar();
    tpos = curwin->w_cursor;
    ಯದಿ (cursor_down(1L, TRUE) == OK)
    {
	ಯದಿ (startcol)
	    coladvance(getvcol_nolist(&Insstart));
	ಯದಿ (old_topline != curwin->w_topline
#ifdef FEAT_DIFF
		|| old_topfill != curwin->w_topfill
#endif
		)
	    redraw_later(UPD_VALID);
	start_arrow(&tpos);
	can_cindent = TRUE;
    }
    ಅನ್ಯಥಾ
	vim_beep(BO_CRSR);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
ins_pagedown(ಶೂನ್ಯ)
{
    pos_T	tpos;

    undisplay_dollar();

    ಯದಿ (mod_mask & MOD_MASK_CTRL)
    {
	// <C-PageDown>: tab page forward
	ಯದಿ (first_tabpage->tp_next != ರಿಕ್ತ)
	{
	    start_arrow(&curwin->w_cursor);
	    goto_tabpage(0);
	}
	ಪ್ರತ್ಯಾಗಮ;
    }

    tpos = curwin->w_cursor;
    ಯದಿ (pagescroll(FORWARD, 1L, FALSE) == OK)
    {
	start_arrow(&tpos);
	can_cindent = TRUE;
    }
    ಅನ್ಯಥಾ
	vim_beep(BO_CRSR);
}

#ifdef FEAT_DND
    ಸ್ಥಿರ ಶೂನ್ಯ
ins_drop(ಶೂನ್ಯ)
{
    do_put('~', ರಿಕ್ತ, BACKWARD, 1L, PUT_CURSEND);
}
#endif

/*
 * Handle TAB in Insert or Replace mode.
 * Return TRUE when the TAB needs to be inserted like a normal character.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
ins_tab(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ		ind;
    ಪೂರ್ಣಾಙ್ಕ		i;
    ಪೂರ್ಣಾಙ್ಕ		temp;

    ಯದಿ (Insstart_blank_vcol == MAXCOL && curwin->w_cursor.lnum == Insstart.lnum)
	Insstart_blank_vcol = get_nolist_virtcol();
    ಯದಿ (echeck_abbr(TAB + ABBR_OFF))
	ಪ್ರತ್ಯಾಗಮ FALSE;

    ind = inindent(0);
    ಯದಿ (ind)
	can_cindent = FALSE;

    /*
     * When nothing special, insert TAB like a normal character.
     */
    ಯದಿ (!curbuf->b_p_et
#ifdef FEAT_VARTABS
	    && !(p_sta && ind
		// These five lines mean 'tabstop' != 'shiftwidth'
		&& ((tabstop_ಎಣಿಕೆ(curbuf->b_p_vts_array) > 1)
		    || (tabstop_ಎಣಿಕೆ(curbuf->b_p_vts_array) == 1
			&& tabstop_first(curbuf->b_p_vts_array)
						       != get_sw_ಮೌಲ್ಯ(curbuf))
		    || (tabstop_ಎಣಿಕೆ(curbuf->b_p_vts_array) == 0
			&& curbuf->b_p_ts != get_sw_ಮೌಲ್ಯ(curbuf))))
	    && tabstop_ಎಣಿಕೆ(curbuf->b_p_vsts_array) == 0
#ಅನ್ಯಥಾ
	    && !(p_sta && ind && curbuf->b_p_ts != get_sw_ಮೌಲ್ಯ(curbuf))
#endif
	    && get_sts_ಮೌಲ್ಯ() == 0)
	ಪ್ರತ್ಯಾಗಮ TRUE;

    ಯದಿ (stop_arrow() == FAIL)
	ಪ್ರತ್ಯಾಗಮ TRUE;

    did_ai = FALSE;
    did_si = FALSE;
    can_si = FALSE;
    can_si_back = FALSE;
    AppendToRedobuff((char_u *)"\t");

#ifdef FEAT_VARTABS
    ಯದಿ (p_sta && ind)		// insert tab in indent, use 'shiftwidth'
    {
	temp = (ಪೂರ್ಣಾಙ್ಕ)get_sw_ಮೌಲ್ಯ(curbuf);
	temp -= get_nolist_virtcol() % temp;
    }
    ಅನ್ಯಥಾ ಯದಿ (tabstop_ಎಣಿಕೆ(curbuf->b_p_vsts_array) > 0 || curbuf->b_p_sts != 0)
				// use 'softtabstop' when set
	temp = tabstop_padding(get_nolist_virtcol(), get_sts_ಮೌಲ್ಯ(),
						     curbuf->b_p_vsts_array);
    ಅನ್ಯಥಾ			// otherwise use 'tabstop'
	temp = tabstop_padding(get_nolist_virtcol(), curbuf->b_p_ts,
						     curbuf->b_p_vts_array);
#ಅನ್ಯಥಾ
    ಯದಿ (p_sta && ind)		// insert tab in indent, use 'shiftwidth'
	temp = (ಪೂರ್ಣಾಙ್ಕ)get_sw_ಮೌಲ್ಯ(curbuf);
    ಅನ್ಯಥಾ ಯದಿ (curbuf->b_p_sts != 0) // use 'softtabstop' when set
	temp = (ಪೂರ್ಣಾಙ್ಕ)get_sts_ಮೌಲ್ಯ();
    ಅನ್ಯಥಾ			// otherwise use 'tabstop'
	temp = (ಪೂರ್ಣಾಙ್ಕ)curbuf->b_p_ts;
    temp -= get_nolist_virtcol() % temp;
#endif

    /*
     * Insert the first space with ins_char().	It will delete one char in
     * replace mode.  Insert the rest with ins_str(); it will not delete any
     * chars.  For MODE_VREPLACE state, we use ins_char() for all characters.
     */
    ins_char(' ');
    ಯಾವತ್ (--temp > 0)
    {
	ಯದಿ (State & VREPLACE_FLAG)
	    ins_char(' ');
	ಅನ್ಯಥಾ
	{
	    ins_str((char_u *)" ", 1);
	    ಯದಿ (State & REPLACE_FLAG)	    // no char replaced
		replace_ತಳ್ಳು(NUL);
	}
    }

    /*
     * When 'expandtab' not set: Replace spaces by TABs where possible.
     */
#ifdef FEAT_VARTABS
    ಯದಿ (!curbuf->b_p_et && (tabstop_ಎಣಿಕೆ(curbuf->b_p_vsts_array) > 0
			    || get_sts_ಮೌಲ್ಯ() > 0
			    || (p_sta && ind)))
#ಅನ್ಯಥಾ
    ಯದಿ (!curbuf->b_p_et && (get_sts_ಮೌಲ್ಯ() || (p_sta && ind)))
#endif
    {
	char_u		*ptr;
	char_u		*saved_ಸಾಲು = ರಿಕ್ತ;	// init for GCC
	pos_T		pos;
	pos_T		fpos;
	pos_T		*cursor;
	colnr_T		want_vcol, vcol;
	ಪೂರ್ಣಾಙ್ಕ		change_col = -1;
	ಪೂರ್ಣಾಙ್ಕ		save_list = curwin->w_p_list;
	char_u		*tab = (char_u *)"\t";
	chartabsize_T	cts;

	/*
	 * Get the current line.  For MODE_VREPLACE state, don't make real
	 * changes yet, just work on a copy of the line.
	 */
	ಯದಿ (State & VREPLACE_FLAG)
	{
	    pos = curwin->w_cursor;
	    cursor = &pos;
	    saved_ಸಾಲು = vim_strnsave(ml_get_curline(), ml_get_curline_len());
	    ಯದಿ (saved_ಸಾಲು == ರಿಕ್ತ)
		ಪ್ರತ್ಯಾಗಮ FALSE;
	    ptr = saved_ಸಾಲು + pos.col;
	}
	ಅನ್ಯಥಾ
	{
	    ptr = ml_get_cursor();
	    cursor = &curwin->w_cursor;
	}

	// When 'L' is not in 'cpoptions' a tab always takes up 'ts' spaces.
	ಯದಿ (vim_strchr(p_cpo, CPO_LISTWM) == ರಿಕ್ತ)
	    curwin->w_p_list = FALSE;

	// Find first white before the cursor
	fpos = curwin->w_cursor;
	ಯಾವತ್ (fpos.col > 0 && VIM_ISWHITE(ptr[-1]))
	{
	    --fpos.col;
	    --ptr;
	}

	// In Replace mode, don't change characters before the insert point.
	ಯದಿ ((State & REPLACE_FLAG)
		&& fpos.lnum == Insstart.lnum
		&& fpos.col < Insstart.col)
	{
	    ptr += Insstart.col - fpos.col;
	    fpos.col = Insstart.col;
	}

	// compute virtual column numbers of first white and cursor
	getvcol(curwin, &fpos, &vcol, ರಿಕ್ತ, ರಿಕ್ತ, 0);
	getvcol(curwin, cursor, &want_vcol, ರಿಕ್ತ, ರಿಕ್ತ, 0);

	init_chartabsize_arg(&cts, curwin, 0, vcol, tab, tab);

	// Use as many TABs as possible.  Beware of 'breakindent', 'showbreak'
	// and 'linebreak' adding extra virtual columns.
	ಯಾವತ್ (VIM_ISWHITE(*ptr))
	{
	    i = lbr_chartabsize(&cts);
	    ಯದಿ (cts.cts_vcol + i > want_vcol)
		ಭಙ್ಗ;
	    ಯದಿ (*ptr != TAB)
	    {
		*ptr = TAB;
		ಯದಿ (change_col < 0)
		{
		    change_col = fpos.col;  // Column of first change
		    // May have to adjust Insstart
		    ಯದಿ (fpos.lnum == Insstart.lnum && fpos.col < Insstart.col)
			Insstart.col = fpos.col;
		}
	    }
	    ++fpos.col;
	    ++ptr;
	    cts.cts_vcol += i;
	}
	vcol = cts.cts_vcol;
	clear_chartabsize_arg(&cts);

	ಯದಿ (change_col >= 0)
	{
	    ಪೂರ್ಣಾಙ್ಕ		    repl_off = 0;

	    // Skip over the spaces we need.
	    init_chartabsize_arg(&cts, curwin, 0, vcol, ptr, ptr);
	    ಯಾವತ್ (cts.cts_vcol < want_vcol && *cts.cts_ptr == ' ')
	    {
		cts.cts_vcol += lbr_chartabsize(&cts);
		++cts.cts_ptr;
		++repl_off;
	    }
	    ptr = cts.cts_ptr;
	    vcol = cts.cts_vcol;
	    clear_chartabsize_arg(&cts);

	    ಯದಿ (vcol > want_vcol)
	    {
		// Must have a char with 'showbreak' just before it.
		--ptr;
		--repl_off;
	    }
	    fpos.col += repl_off;

	    // Delete following spaces.
	    i = cursor->col - fpos.col;
	    ಯದಿ (i > 0)
	    {
#ifdef FEAT_PROP_POPUP
		ಯದಿ (!(State & VREPLACE_FLAG))
		{
		    char_u  *newp;
		    ಪೂರ್ಣಾಙ್ಕ	    col;

		    newp = alloc(curbuf->b_ml.ml_ಸಾಲು_len - i);
		    ಯದಿ (newp == ರಿಕ್ತ)
			ಪ್ರತ್ಯಾಗಮ FALSE;

		    col = ptr - curbuf->b_ml.ml_ಸಾಲು_ptr;
		    ಯದಿ (col > 0)
			mch_memmove(newp, ptr - col, col);
		    mch_memmove(newp + col, ptr + i,
					   curbuf->b_ml.ml_ಸಾಲು_len - col - i);

		    ಯದಿ (curbuf->b_ml.ml_flags & (ML_ಸಾಲು_DIRTY | ML_ALLOCATED))
			vim_free(curbuf->b_ml.ml_ಸಾಲು_ptr);
		    curbuf->b_ml.ml_ಸಾಲು_ptr = newp;
		    curbuf->b_ml.ml_ಸಾಲು_len -= i;
		    curbuf->b_ml.ml_ಸಾಲು_textlen = 0;
		    curbuf->b_ml.ml_flags =
			   (curbuf->b_ml.ml_flags | ML_ಸಾಲು_DIRTY) & ~ML_EMPTY;
		}
		ಅನ್ಯಥಾ
#endif
		    STRMOVE(ptr, ptr + i);
		// correct replace stack.
		ಯದಿ ((State & REPLACE_FLAG) && !(State & VREPLACE_FLAG))
		    ಪ್ರತಿ (temp = i; --temp >= 0; )
			replace_join(repl_off);
	    }
#ifdef FEAT_NETBEANS_INTG
	    ಯದಿ (netbeans_active())
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
	    ಯದಿ (State & VREPLACE_FLAG)
	    {
		// Backspace from real cursor to change_col
		backspace_until_column(change_col);

		// Insert each char in saved_line from changed_col to
		// ptr-cursor
		ins_bytes_len(saved_ಸಾಲು + change_col,
						    cursor->col - change_col);
	    }
	}

	ಯದಿ (State & VREPLACE_FLAG)
	    vim_free(saved_ಸಾಲು);
	curwin->w_p_list = save_list;
    }

    ಪ್ರತ್ಯಾಗಮ FALSE;
}

/*
 * Handle CR or NL in insert mode.
 * Return FAIL when out of memory or can't undo.
 */
    ಪೂರ್ಣಾಙ್ಕ
ins_eol(ಪೂರ್ಣಾಙ್ಕ c)
{
    ಪೂರ್ಣಾಙ್ಕ	    i;

    ಯದಿ (echeck_abbr(c + ABBR_OFF))
	ಪ್ರತ್ಯಾಗಮ OK;
    ಯದಿ (stop_arrow() == FAIL)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    undisplay_dollar();

    /*
     * Strange Vi behaviour: In Replace mode, typing a NL will not delete the
     * character under the cursor.  Only push a NUL on the replace stack,
     * nothing to put back when the NL is deleted.
     */
    ಯದಿ ((State & REPLACE_FLAG) && !(State & VREPLACE_FLAG))
	replace_ತಳ್ಳು(NUL);

    /*
     * In MODE_VREPLACE state, a NL replaces the rest of the line, and starts
     * replacing the next line, so we push all of the characters left on the
     * line onto the replace stack.  This is not done here though, it is done
     * in open_line().
     */

    // Put cursor on NUL if on the last char and coladd is 1 (happens after
    // CTRL-O).
    ಯದಿ (virtual_active() && curwin->w_cursor.coladd > 0)
	coladvance(getviscol());

#ifdef FEAT_RIGHTLEFT
    // NL in reverse insert will always start in the end of
    // current line.
    ಯದಿ (revins_on)
	curwin->w_cursor.col += ml_get_cursor_len();
#endif

    AppendToRedobuff(NL_STR);
    i = open_ಸಾಲು(FORWARD,
	    has_format_option(FO_RET_COMS) ? OPENLINE_DO_COM : 0, old_indent,
	    ರಿಕ್ತ);
    old_indent = 0;
    can_cindent = TRUE;
#ifdef FEAT_FOLDING
    // When inserting a line the cursor line must never be in a closed fold.
    foldOpenCursor();
#endif

    ಪ್ರತ್ಯಾಗಮ i;
}

#ifdef FEAT_DIGRAPHS
/*
 * Handle digraph in insert mode.
 * Returns character still to be inserted, or NUL when nothing remaining to be
 * done.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
ins_digraph(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ	    c;
    ಪೂರ್ಣಾಙ್ಕ	    cc;
    ಪೂರ್ಣಾಙ್ಕ	    did_putchar = FALSE;

    pc_status = PC_STATUS_UNSET;
    ಯದಿ (redrawing() && !char_avail())
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
    ++allow_ಕೀಲಿಗಳು;
    c = plain_vgetc();
    --no_mapping;
    --allow_ಕೀಲಿಗಳು;
    ಯದಿ (did_putchar)
	// when the line fits in 'columns' the '?' is at the start of the next
	// line and will not be removed by the redraw
	edit_unputchar();

    ಯದಿ (IS_SPECIAL(c) || mod_mask)	    // special key
    {
	clear_showcmd();
	ಸೇರಿಸು_special(c, TRUE, FALSE);
	ಪ್ರತ್ಯಾಗಮ NUL;
    }
    ಯದಿ (c != ESC)
    {
	did_putchar = FALSE;
	ಯದಿ (redrawing() && !char_avail())
	{
	    // may need to redraw when no more chars available now
	    ins_redraw(FALSE);

	    ಯದಿ (char2cells(c) == 1)
	    {
		ins_redraw(FALSE);
		edit_putchar(c, TRUE);
		did_putchar = TRUE;
	    }
	    add_to_showcmd_c(c);
	}
	++no_mapping;
	++allow_ಕೀಲಿಗಳು;
	cc = plain_vgetc();
	--no_mapping;
	--allow_ಕೀಲಿಗಳು;
	ಯದಿ (did_putchar)
	    // when the line fits in 'columns' the '?' is at the start of the
	    // next line and will not be removed by a redraw
	    edit_unputchar();
	ಯದಿ (cc != ESC)
	{
	    AppendToRedobuff((char_u *)CTRL_V_STR);
	    c = digraph_get(c, cc, TRUE);
	    clear_showcmd();
	    ಪ್ರತ್ಯಾಗಮ c;
	}
    }
    clear_showcmd();
    ಪ್ರತ್ಯಾಗಮ NUL;
}
#endif

/*
 * Handle CTRL-E and CTRL-Y in Insert mode: copy char from other line.
 * Returns the char to be inserted, or NUL if none found.
 */
    ಪೂರ್ಣಾಙ್ಕ
ins_copychar(linenr_T lnum)
{
    ಪೂರ್ಣಾಙ್ಕ		    c;
    char_u	    *ptr, *prev_ptr;
    char_u	    *ಸಾಲು;
    chartabsize_T   cts;

    ಯದಿ (lnum < 1 || lnum > curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
    {
	vim_beep(BO_COPY);
	ಪ್ರತ್ಯಾಗಮ NUL;
    }

    // try to advance to the cursor column
    validate_virtcol();
    ಸಾಲು = ml_get(lnum);
    prev_ptr = ಸಾಲು;
    init_chartabsize_arg(&cts, curwin, lnum, 0, ಸಾಲು, ಸಾಲು);
    ಯಾವತ್ (cts.cts_vcol < curwin->w_virtcol && *cts.cts_ptr != NUL)
    {
	prev_ptr = cts.cts_ptr;
	cts.cts_vcol += lbr_chartabsize_adv(&cts);
    }
    ಯದಿ (cts.cts_vcol > curwin->w_virtcol)
	ptr = prev_ptr;
    ಅನ್ಯಥಾ
	ptr = cts.cts_ptr;
    clear_chartabsize_arg(&cts);

    c = (*mb_ptr2char)(ptr);
    ಯದಿ (c == NUL)
	vim_beep(BO_COPY);
    ಪ್ರತ್ಯಾಗಮ c;
}

/*
 * CTRL-Y or CTRL-E typed in Insert mode.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
ins_ctrl_ey(ಪೂರ್ಣಾಙ್ಕ tc)
{
    ಪೂರ್ಣಾಙ್ಕ	    c = tc;

    ಯದಿ (ctrl_x_mode_scroll())
    {
	ಯದಿ (c == Ctrl_Y)
	    scrolldown_clamp();
	ಅನ್ಯಥಾ
	    scrollup_clamp();
	redraw_later(UPD_VALID);
    }
    ಅನ್ಯಥಾ
    {
	c = ins_copychar(curwin->w_cursor.lnum + (c == Ctrl_Y ? -1 : 1));
	ಯದಿ (c != NUL)
	{
	    long	tw_save;

	    // The character must be taken literally, insert like it
	    // was typed after a CTRL-V, and pretend 'textwidth'
	    // wasn't set.  Digits, 'o' and 'x' are special after a
	    // CTRL-V, don't use it for these.
	    ಯದಿ (c < 256 && !SAFE_isalnum(c))
		AppendToRedobuff((char_u *)CTRL_V_STR);	// CTRL-V
	    tw_save = curbuf->b_p_tw;
	    curbuf->b_p_tw = -1;
	    ಸೇರಿಸು_special(c, TRUE, FALSE);
	    curbuf->b_p_tw = tw_save;
#ifdef FEAT_RIGHTLEFT
	    revins_chars++;
	    revins_legal++;
#endif
	    c = Ctrl_V;	// pretend CTRL-V is last character
	    auto_format(FALSE, TRUE);
	}
    }
    ಪ್ರತ್ಯಾಗಮ c;
}

/*
 * Get the value that w_virtcol would have when 'list' is off.
 * Unless 'cpo' contains the 'L' flag.
 */
    colnr_T
get_nolist_virtcol(ಶೂನ್ಯ)
{
    // check validity of cursor in current buffer
    ಯದಿ (curwin->w_buffer == ರಿಕ್ತ
	|| curwin->w_buffer->b_ml.ml_mfp == ರಿಕ್ತ
	|| curwin->w_cursor.lnum > curwin->w_buffer->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	ಪ್ರತ್ಯಾಗಮ 0;
    ಯದಿ (curwin->w_p_list && vim_strchr(p_cpo, CPO_LISTWM) == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ getvcol_nolist(&curwin->w_cursor);
    validate_virtcol();
    ಪ್ರತ್ಯಾಗಮ curwin->w_virtcol;
}

#ಯದಿ defined(FEAT_EVAL)
/*
 * Handle the InsertCharPre autocommand.
 * "c" is the character that was typed.
 * Return a pointer to allocated memory with the replacement string.
 * Return NULL to continue inserting "c".
 */
    ಸ್ಥಿರ char_u *
do_ಸೇರಿಸು_char_pre(ಪೂರ್ಣಾಙ್ಕ c)
{
    char_u	*res;
    char_u	buf[MB_MAXBYTES + 1];
    size_t	buflen;
    ಪೂರ್ಣಾಙ್ಕ		save_State = State;

    // Return quickly when there is nothing to do.
    ಯದಿ (!has_insertcharpre())
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    ಯದಿ (c == Ctrl_RSB)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    ಯದಿ (has_mbyte)
	buflen = (*mb_char2bytes)(c, buf);
    ಅನ್ಯಥಾ
    {
	buf[0] = c;
	buflen = 1;
    }
    buf[buflen] = NUL;

    // Lock the text to avoid weird things from happening.
    ++textlock;
    set_vim_var_string(VV_CHAR, buf, (ಪೂರ್ಣಾಙ್ಕ)buflen);  // set v:char

    res = ರಿಕ್ತ;
    ಯದಿ (ins_apply_autocmds(EVENT_INSERTCHARPRE))
    {
	// Get the value of v:char.  It may be empty or more than one
	// character.  Only use it when changed, otherwise continue with the
	// original character to avoid breaking autoindent.
	ಯದಿ (STRCMP(buf, get_vim_var_str(VV_CHAR)) != 0)
	    res = vim_strsave(get_vim_var_str(VV_CHAR));
    }

    set_vim_var_string(VV_CHAR, ರಿಕ್ತ, -1);  // clear v:char
    --textlock;

    // Restore the State, it may have been changed.
    State = save_State;

    ಪ್ರತ್ಯಾಗಮ res;
}
#endif

    ಪೂರ್ಣಾಙ್ಕ
get_can_cindent(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ can_cindent;
}

    ಶೂನ್ಯ
set_can_cindent(ಪೂರ್ಣಾಙ್ಕ val)
{
    can_cindent = val;
}

/*
 * Trigger "event" and take care of fixing undo.
 */
    ಪೂರ್ಣಾಙ್ಕ
ins_apply_autocmds(event_T event)
{
    varnumber_T	tick = CHANGEDTICK(curbuf);
    ಪೂರ್ಣಾಙ್ಕ r;

    r = apply_autocmds(event, ರಿಕ್ತ, ರಿಕ್ತ, FALSE, curbuf);

    // If u_savesub() was called then we are not prepared to start
    // a new line.  Call u_save() with no contents to fix that.
    // Except when leaving Insert mode.
    ಯದಿ (event != EVENT_INSERTLEAVE && tick != CHANGEDTICK(curbuf))
	u_save(curwin->w_cursor.lnum, (linenr_T)(curwin->w_cursor.lnum + 1));

    ಪ್ರತ್ಯಾಗಮ r;
}
