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

#ଅନ୍ତର୍ଭାଵ "vim.h"

#define BACKSPACE_CHAR		    1
#define BACKSPACE_WORD		    2
#define BACKSPACE_WORD_NOT_SPACE    3
#define BACKSPACE_ଧାଡ଼ି		    4

// Set when doing something for completion that may call edit() recursively,
// which is not allowed.
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	compl_busy = FALSE;


ସ୍ଥିର ଶୂନ୍ୟ ins_ctrl_v(ଶୂନ୍ୟ);
ସ୍ଥିର ଶୂନ୍ୟ ଭର୍ତ୍ତି_special(ପୂର୍ଣ୍ଣସଂଖ୍ୟା, ପୂର୍ଣ୍ଣସଂଖ୍ୟା, ପୂର୍ଣ୍ଣସଂଖ୍ୟା);
ସ୍ଥିର ଶୂନ୍ୟ redo_literal(ପୂର୍ଣ୍ଣସଂଖ୍ୟା c);
ସ୍ଥିର ଶୂନ୍ୟ start_arrow_common(pos_T *end_ଭର୍ତ୍ତି_pos, ପୂର୍ଣ୍ଣସଂଖ୍ୟା change);
#ifdef FEAT_SPELL
ସ୍ଥିର ଶୂନ୍ୟ check_spell_redraw(ଶୂନ୍ୟ);
#endif
ସ୍ଥିର ଶୂନ୍ୟ stop_ଭର୍ତ୍ତି(pos_T *end_ଭର୍ତ୍ତି_pos, ପୂର୍ଣ୍ଣସଂଖ୍ୟା esc, ପୂର୍ଣ୍ଣସଂଖ୍ୟା nomove);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା  echeck_abbr(ପୂର୍ଣ୍ଣସଂଖ୍ୟା);
ସ୍ଥିର ଶୂନ୍ୟ mb_replace_କାଢ଼_ins(ପୂର୍ଣ୍ଣସଂଖ୍ୟା cc);
ସ୍ଥିର ଶୂନ୍ୟ replace_flush(ଶୂନ୍ୟ);
ସ୍ଥିର ଶୂନ୍ୟ replace_do_bs(ପୂର୍ଣ୍ଣସଂଖ୍ୟା limit_col);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା del_char_after_col(ପୂର୍ଣ୍ଣସଂଖ୍ୟା limit_col);
ସ୍ଥିର ଶୂନ୍ୟ ins_reg(ଶୂନ୍ୟ);
ସ୍ଥିର ଶୂନ୍ୟ ins_ctrl_g(ଶୂନ୍ୟ);
ସ୍ଥିର ଶୂନ୍ୟ ins_ctrl_hat(ଶୂନ୍ୟ);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା  ins_esc(long *ଗଣନା_2, ପୂର୍ଣ୍ଣସଂଖ୍ୟା cmdchar, ପୂର୍ଣ୍ଣସଂଖ୍ୟା nomove);
#ifdef FEAT_RIGHTLEFT
ସ୍ଥିର ଶୂନ୍ୟ ins_ctrl_(ଶୂନ୍ୟ);
#endif
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା ins_start_select(ପୂର୍ଣ୍ଣସଂଖ୍ୟା c);
ସ୍ଥିର ଶୂନ୍ୟ ins_ଭର୍ତ୍ତି(ପୂର୍ଣ୍ଣସଂଖ୍ୟା replaceState);
ସ୍ଥିର ଶୂନ୍ୟ ins_ctrl_o(ଶୂନ୍ୟ);
ସ୍ଥିର ଶୂନ୍ୟ ins_shift(ପୂର୍ଣ୍ଣସଂଖ୍ୟା c, ପୂର୍ଣ୍ଣସଂଖ୍ୟା lastc);
ସ୍ଥିର ଶୂନ୍ୟ ins_del(ଶୂନ୍ୟ);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା  ins_bs(ପୂର୍ଣ୍ଣସଂଖ୍ୟା c, ପୂର୍ଣ୍ଣସଂଖ୍ୟା mode, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *inserted_space_p);
#ଯଦି defined(FEAT_GUI_TABLINE)
ସ୍ଥିର ଶୂନ୍ୟ ins_tabline(ପୂର୍ଣ୍ଣସଂଖ୍ୟା c);
#endif
ସ୍ଥିର ଶୂନ୍ୟ ins_ବାମ(ଶୂନ୍ୟ);
ସ୍ଥିର ଶୂନ୍ୟ ins_home(ପୂର୍ଣ୍ଣସଂଖ୍ୟା c);
ସ୍ଥିର ଶୂନ୍ୟ ins_end(ପୂର୍ଣ୍ଣସଂଖ୍ୟା c);
ସ୍ଥିର ଶୂନ୍ୟ ins_s_ବାମ(ଶୂନ୍ୟ);
ସ୍ଥିର ଶୂନ୍ୟ ins_ଡାହାଣ(ଶୂନ୍ୟ);
ସ୍ଥିର ଶୂନ୍ୟ ins_s_ଡାହାଣ(ଶୂନ୍ୟ);
ସ୍ଥିର ଶୂନ୍ୟ ins_up(ପୂର୍ଣ୍ଣସଂଖ୍ୟା startcol);
ସ୍ଥିର ଶୂନ୍ୟ ins_pageup(ଶୂନ୍ୟ);
ସ୍ଥିର ଶୂନ୍ୟ ins_down(ପୂର୍ଣ୍ଣସଂଖ୍ୟା startcol);
ସ୍ଥିର ଶୂନ୍ୟ ins_pagedown(ଶୂନ୍ୟ);
#ifdef FEAT_DND
ସ୍ଥିର ଶୂନ୍ୟ ins_drop(ଶୂନ୍ୟ);
#endif
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା  ins_tab(ଶୂନ୍ୟ);
#ifdef FEAT_DIGRAPHS
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା  ins_digraph(ଶୂନ୍ୟ);
#endif
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା  ins_ctrl_ey(ପୂର୍ଣ୍ଣସଂଖ୍ୟା tc);
#ଯଦି defined(FEAT_EVAL)
ସ୍ଥିର char_u *do_ଭର୍ତ୍ତି_char_pre(ପୂର୍ଣ୍ଣସଂଖ୍ୟା c);
#endif

ସ୍ଥିର colnr_T	Insstart_textlen;	// length of line when insert started
ସ୍ଥିର colnr_T	Insstart_blank_vcol;	// vcol for first inserted blank
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	update_Insstart_orig = TRUE; // set Insstart_orig to Insstart

ସ୍ଥିର string_T	last_ଭର୍ତ୍ତି = {ଖାଲି, 0};    // the text of the previous insert,
					    // K_SPECIAL and CSI are escaped
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	last_ଭର୍ତ୍ତି_skip; // nr of chars in front of previous insert
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	new_ଭର୍ତ୍ତି_skip;  // nr of chars in front of current insert
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	did_restart_edit;	// "restart_edit" when calling edit()

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	can_cindent;		// may do cindenting on this line

#ifdef FEAT_RIGHTLEFT
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	revins_on;		// reverse insert mode on
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	revins_chars;		// how much to skip after edit
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	revins_legal;		// was the last char 'legal'?
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	revins_scol;		// start column of revins session
#endif

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	ins_need_undo;		// call u_save() before inserting a
					// char.  Set when edit() is called.
					// after that arrow_used is used.

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	dont_sync_undo = FALSE;	// CTRL-G U prevents syncing undo for
					// the next left/right cursor key

// With 'autocompletedelay' set, arm the delay and let the main loop fire
// Insert-mode autocommands; the popup is shown later on K_COMPLETE_DELAY.
// Otherwise trigger completion right away.
#define TRIGGER_AUTOCOMPLETE()			\
    କର {					\
	update_screen(UPD_VALID);  /* Show char (deletion) immediately */ \
	out_flush();				\
	ins_compl_enable_autocomplete();	\
	ins_compl_arm_autostart();		\
	ଯଦି (!ins_compl_arm_autocomplete_delay())\
	    ଯାଅ docomplete;			\
    } ଯେତେବେଳେ (0)

#define MAY_TRIGGER_AUTOCOMPLETE(c)				\
    କର {							\
	ଯଦି (ins_compl_has_autocomplete() && !char_avail()	\
		&& curwin->w_cursor.col > 0)			\
	{							\
	    (c) = char_before_cursor();				\
	    ଯଦି (vim_isprintc(c))				\
		TRIGGER_AUTOCOMPLETE();				\
	}							\
    } ଯେତେବେଳେ (0)

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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
edit(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		cmdchar,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		startln,	// if set, insert at start of line
    long	ଗଣନା_2)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		c = 0;
    char_u	*ptr;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		lastc = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		mincol;
    ସ୍ଥିର linenr_T o_lnum = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		did_backspace = TRUE;	    // previous char was backspace
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ଧାଡ଼ି_is_white = FALSE;	    // line is empty before insert
    linenr_T	old_topline = 0;	    // topline before insertion
#ifdef FEAT_DIFF
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		old_topfill = -1;
#endif
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		inserted_space = FALSE;     // just inserted a space
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		replaceState = MODE_REPLACE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		nomove = FALSE;		    // don't move cursor on return
#ifdef FEAT_JOB_CHANNEL
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		cmdchar_todo = cmdchar;
#endif
#ifdef FEAT_CONCEAL
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		cursor_ଧାଡ଼ି_was_concealed;
#endif
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ins_just_started = TRUE;

    // Remember whether editing was restarted after CTRL-O.
    did_restart_edit = restart_edit;

    // sleep before redrawing, needed for "CTRL-O :" that results in an
    // error message
    check_for_delay(TRUE);

    // set Insstart_orig to Insstart
    update_Insstart_orig = TRUE;

#ifdef HAVE_SANDBOX
    // Don't allow inserting in the sandbox.
    ଯଦି (sandbox != 0)
    {
	emsg(_(e_not_allowed_in_sandbox));
	ଫେରନ୍ତୁ FALSE;
    }
#endif
    // Don't allow changes in the buffer while editing the cmdline.  The
    // caller of getcmdline() may get confused.
    // Don't allow recursive insert mode when busy with completion.
    ଯଦି (textlock != 0 || ins_compl_active() || compl_busy || pum_visible())
    {
	emsg(_(e_not_allowed_to_change_text_or_change_window));
	ଫେରନ୍ତୁ FALSE;
    }
    ins_compl_clear();	    // clear stuff for CTRL-X mode

    /*
     * Trigger InsertEnter autocommands.  Do not do this for "r<CR>" or "grx".
     */
    ଯଦି (cmdchar != 'r' && cmdchar != 'v')
    {
	pos_T   save_cursor = curwin->w_cursor;

#ifdef FEAT_EVAL
	ଯଦି (cmdchar == 'R')
	    ptr = (char_u *)"r";
	ଅନ୍ୟଥା ଯଦି (cmdchar == 'V')
	    ptr = (char_u *)"v";
	ଅନ୍ୟଥା
	    ptr = (char_u *)"i";
	set_vim_var_string(VV_INSERTMODE, ptr, 1);
	set_vim_var_string(VV_CHAR, ଖାଲି, -1);  // clear v:char
#endif
	ins_apply_autocmds(EVENT_INSERTENTER);

	// Check for changed highlighting, e.g. for ModeMsg.
	ଯଦି (need_highlight_changed)
	    highlight_changed();

	// Make sure the cursor didn't move.  Do call check_cursor_col() in
	// case the text was modified.  Since Insert mode was not started yet
	// a call to check_cursor_col() may move the cursor, especially with
	// the "A" command, thus set State to avoid that. Also check that the
	// line number is still valid (lines may have been deleted).
	// Do not restore if v:char was set to a non-empty string.
	ଯଦି (!EQUAL_POS(curwin->w_cursor, save_cursor)
#ifdef FEAT_EVAL
		&& *get_vim_var_str(VV_CHAR) == NUL
#endif
		&& save_cursor.lnum <= curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା)
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା save_state = State;

	    curwin->w_cursor = save_cursor;
	    State = MODE_ଭର୍ତ୍ତି;
	    check_cursor_col();
	    State = save_state;
	}
    }

#ifdef FEAT_CONCEAL
    // Check if the cursor line was concealed before changing State.
    cursor_ଧାଡ଼ି_was_concealed = curwin->w_p_cole > 0
						&& conceal_cursor_ଧାଡ଼ି(curwin);
#endif

    /*
     * When doing a paste with the middle mouse button, Insstart is set to
     * where the paste started.
     */
    ଯଦି (where_paste_started.lnum != 0)
	Insstart = where_paste_started;
    ଅନ୍ୟଥା
    {
	Insstart = curwin->w_cursor;
	ଯଦି (startln)
	    Insstart.col = 0;
    }
    Insstart_textlen = (colnr_T)linetabsize_str(ml_get_curline());
    Insstart_blank_vcol = MAXCOL;
    ଯଦି (!did_ai)
	ai_col = 0;

    ଯଦି (cmdchar != NUL && restart_edit == 0)
    {
	ResetRedobuff();
	Append_ସଂଖ୍ୟା_To_Redobuff(ଗଣନା_2);
	ଯଦି (cmdchar == 'V' || cmdchar == 'v')
	{
	    // "gR" or "gr" command
	    AppendCharToRedobuff('g');
	    AppendCharToRedobuff((cmdchar == 'v') ? 'r' : 'R');
	}
	ଅନ୍ୟଥା
	{
	    ଯଦି (cmdchar == K_PS)
		AppendCharToRedobuff('a');
	    ଅନ୍ୟଥା
		AppendCharToRedobuff(cmdchar);
	    ଯଦି (cmdchar == 'g')		    // "gI" command
		AppendCharToRedobuff('I');
	    ଅନ୍ୟଥା ଯଦି (cmdchar == 'r')	    // "r<CR>" command
		ଗଣନା_2 = 1;		    // insert only one <CR>
	}
    }

    ଯଦି (cmdchar == 'R')
    {
	State = MODE_REPLACE;
    }
    ଅନ୍ୟଥା ଯଦି (cmdchar == 'V' || cmdchar == 'v')
    {
	State = MODE_VREPLACE;
	replaceState = MODE_VREPLACE;
	orig_ଧାଡ଼ି_ଗଣନା = curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା;
	vr_ଧାଡ଼ିଗୁଡ଼ିକ_changed = 1;
    }
    ଅନ୍ୟଥା
	State = MODE_ଭର୍ତ୍ତି;

    may_trigger_modechanged();
    stop_ଭର୍ତ୍ତି_mode = FALSE;

#ifdef FEAT_CONCEAL
    // Check if the cursor line needs redrawing after changing State.  If
    // 'concealcursor' is "n" it needs to be redrawn without concealing.
    conceal_check_cursor_ଧାଡ଼ି(cursor_ଧାଡ଼ି_was_concealed);
#endif

    // Need to position cursor again when on a TAB and when on a char with
    // virtual text.
    ଯଦି (gchar_cursor() == TAB
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
    ଯଦି (curbuf->b_p_iminsert == B_IMODE_LMAP)
	State |= MODE_LANGMAP;
#ifdef HAVE_ନିବେଶ_METHOD
    im_set_active(curbuf->b_p_iminsert == B_IMODE_IM);
#endif

    setmouse();
    clear_showcmd();
#ifdef FEAT_RIGHTLEFT
    // there is no reverse replace mode
    revins_on = (State == MODE_ଭର୍ତ୍ତି && p_ri);
    ଯଦି (revins_on)
	undisplay_dollar();
    revins_chars = 0;
    revins_legal = 0;
    revins_scol = -1;
#endif
    ଯଦି (!p_ek)
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
    ଯଦି (restart_edit != 0 && stuff_empty())
    {
	/*
	 * After a paste we consider text typed to be part of the insert for
	 * the pasted text. You can backspace over the pasted text too.
	 */
	ଯଦି (where_paste_started.lnum)
	    arrow_used = FALSE;
	ଅନ୍ୟଥା
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
	ଯଦି (((ins_at_eol && curwin->w_cursor.lnum == o_lnum)
		    || curwin->w_curswant > curwin->w_virtcol)
		&& *(ptr = ml_get_curline() + curwin->w_cursor.col) != NUL)
	{
	    ଯଦି (ptr[1] == NUL)
		++curwin->w_cursor.col;
	    ଅନ୍ୟଥା ଯଦି (has_mbyte)
	    {
		i = (*mb_ptr2len)(ptr);
		ଯଦି (ptr[i] == NUL)
		    curwin->w_cursor.col += i;
	    }
	}
	ins_at_eol = FALSE;
    }
    ଅନ୍ୟଥା
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
    ଯଦି (!p_im && did_restart_edit == 0)
	foldOpenCursor();
#endif

    /*
     * If 'showmode' is set, show the current (insert/replace/..) mode.
     * A warning message for changing a readonly file is given here, before
     * actually changing anything.  It's put after the mode, if any.
     */
    i = 0;
    ଯଦି (p_smd && ସନ୍ଦେଶ_silent == 0)
	i = showmode();

    ଯଦି (!p_im && did_restart_edit == 0)
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
    new_ଭର୍ତ୍ତି_skip = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)inserted.length;
    ଯଦି (inserted.string != ଖାଲି)
	vim_free(inserted.string);

    old_indent = 0;

    /*
     * Main loop in Insert mode: repeat until Insert mode is left.
     */
    ପାଇଁ (;;)
    {
#ifdef FEAT_RIGHTLEFT
	ଯଦି (!revins_legal)
	    revins_scol = -1;	    // reset on illegal motions
	ଅନ୍ୟଥା
	    revins_legal = 0;
#endif
	ଯଦି (arrow_used)	    // don't repeat insert when arrow key used
	    ଗଣନା_2 = 0;

	ଯଦି (update_Insstart_orig)
	    Insstart_orig = Insstart;

	ଯଦି (stop_ଭର୍ତ୍ତି_mode && !ins_compl_active())
	{
	    // ":stopinsert" used or 'insertmode' reset
	    ଗଣନା_2 = 0;
	    ଯାଅ doESCkey;
	}

	// set curwin->w_curswant for next K_DOWN or K_UP
	ଯଦି (!arrow_used)
	    curwin->w_set_curswant = true;

	// If there is no typeahead may check for timestamps (e.g., for when a
	// menu invoked a shell command).
	ଯଦି (stuff_empty())
	{
	    did_check_timestamps = FALSE;
	    ଯଦି (need_check_timestamps)
		check_timestamps(FALSE);
	}

	/*
	 * When emsg() was called msg_scroll will have been set.
	 */
	ସନ୍ଦେଶ_scroll = FALSE;

#ifdef FEAT_GUI
	// When 'mousefocus' is set a mouse movement may have taken us to
	// another window.  "need_mouse_correct" may then be set because of an
	// autocommand.
	ଯଦି (need_mouse_correct)
	    gui_mouse_correct();
#endif

#ifdef FEAT_FOLDING
	// Open fold at the cursor line, according to 'foldopen'.
	ଯଦି (fdo_flags & FDO_ଭର୍ତ୍ତି)
	    foldOpenCursor();
	// Close folds where the cursor isn't, according to 'foldclose'
	ଯଦି (!char_avail())
	    foldCheckClose();
#endif

#ifdef FEAT_JOB_CHANNEL
	ଯଦି (bt_prompt(curbuf))
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
	ଯଦି (curbuf->b_mod_set
		&& curwin->w_p_wrap
		&& !curwin->w_p_sms
		&& !did_backspace
		&& curwin->w_topline == old_topline
#ifdef FEAT_DIFF
		&& curwin->w_topfill == old_topfill
#endif
		&& ଗଣନା_2 <= 1
		)
	{
	    mincol = curwin->w_wcol;
	    validate_cursor_col();

	    ଯଦି (
#ifdef FEAT_VARTABS
		curwin->w_wcol < mincol - tabstop_at(get_nolist_virtcol(),
						     curbuf->b_p_ts,
						     curbuf->b_p_vts_array,
						     FALSE)
#ଅନ୍ୟଥା
		(ପୂର୍ଣ୍ଣସଂଖ୍ୟା)curwin->w_wcol < mincol - curbuf->b_p_ts
#endif
		    && curwin->w_wrow ==
				   curwin->w_height - 1 - get_scrolloff_ମୂଲ୍ୟ()
		    && (curwin->w_cursor.lnum != curwin->w_topline
#ifdef FEAT_DIFF
			|| curwin->w_topfill > 0
#endif
		    ))
	    {
#ifdef FEAT_DIFF
		ଯଦି (curwin->w_topfill > 0)
		    --curwin->w_topfill;
		ଅନ୍ୟଥା
#endif
#ifdef FEAT_FOLDING
		ଯଦି (hasFolding(curwin->w_topline, ଖାଲି, &old_topline))
		    set_topline(curwin, old_topline + 1);
		ଅନ୍ୟଥା
#endif
		    set_topline(curwin, curwin->w_topline + 1);
	    }
	}

	// May need to adjust w_topline to show the cursor.
	ଯଦି (ଗଣନା_2 <= 1)
	    update_topline();

	did_backspace = FALSE;

	ଯଦି (ଗଣନା_2 <= 1)
	    validate_cursor();		// may set must_redraw

	/*
	 * Redraw the display when no characters are waiting.
	 * Also shows mode, ruler and positions cursor.
	 */
	ins_redraw(TRUE);

	ଯଦି (curwin->w_p_scb)
	    do_check_scrollbind(TRUE);

	ଯଦି (curwin->w_p_crb)
	    do_check_cursorbind();
	ଯଦି (ଗଣନା_2 <= 1)
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
	ଯଦି (c != K_CURSORHOLD && c != K_COMPLETE_DELAY)
	    lastc = c;		// remember the previous char for CTRL-D

	// After using CTRL-G U the next cursor key will not break undo.
	ଯଦି (dont_sync_undo == MAYBE)
	    dont_sync_undo = TRUE;
	ଅନ୍ୟଥା
	    dont_sync_undo = FALSE;
	ଯଦି (cmdchar == K_PS)
	    // Got here from normal mode when bracketed paste started.
	    c = K_PS;
	ଅନ୍ୟଥା
	{
	    // Trigger autocomplete when entering Insert mode, either directly
	    // or via change commands like 'ciw', 'cw', etc., before the first
	    // character is typed.
	    ଯଦି (ins_just_started)
	    {
		ins_just_started = FALSE;
		ଯଦି (ins_compl_has_autocomplete() && !char_avail()
			&& curwin->w_cursor.col > 0)
		{
		    c = char_before_cursor();
		    ଯଦି (vim_isprintc(c))
		    {
			ins_compl_enable_autocomplete();
			ins_compl_arm_autostart();
			ins_compl_init_get_longest();
#ifdef FEAT_RIGHTLEFT
			ଯଦି (p_hkmap)
			    c = hkmap(c);		// Hebrew mode mapping
#endif
			// Defer until the delay expires (K_COMPLETE_DELAY), or
			// trigger now when no delay is in effect.
			ଯଦି (!ins_compl_arm_autocomplete_delay())
			    ଯାଅ docomplete;
		    }
		}
	    }

	    କର
	    {
		c = safe_vgetc();

		ଯଦି (stop_ଭର୍ତ୍ତି_mode
#ifdef FEAT_TERMINAL
			|| (c == K_IGNORE && term_use_loop())
#endif
		   )
		{
		    // Insert mode ended, possibly from a callback, or a timer
		    // must have opened a terminal window.
		    ଯଦି (c != K_IGNORE && c != K_NOP)
			vungetc(c);
		    ଗଣନା_2 = 0;

		    ଯଦି (!bt_prompt(curwin->w_buffer)
#ifdef FEAT_TERMINAL
			    && !bt_terminal(curwin->w_buffer)
#endif
			    && stop_ଭର୍ତ୍ତି_mode)
			// :stopinsert command via callback or via server command
			nomove = FALSE;
		    ଅନ୍ୟଥା
			nomove = TRUE;
		    ins_compl_prep(ESC);
		    ଯାଅ doESCkey;
		}
	    } ଯେତେବେଳେ (c == K_IGNORE || c == K_NOP);
	}

	// Don't want K_CURSORHOLD for the second key, e.g., after CTRL-V.
	did_cursorhold = TRUE;
	ଯଦି (c != K_CURSORHOLD && c != K_COMPLETE_DELAY)
	{
	    // Don't want delayed autocompletion from the previous key either.
	    ins_compl_clear_autocomplete_delay();
	    ins_compl_disarm_autostart();
	    // A completion already on screen goes on being what it was.
	    ଯଦି (!ins_compl_active())
		ins_compl_disable_autocomplete();
	}

#ifdef FEAT_RIGHTLEFT
	ଯଦି (p_hkmap && ଚାବି_Typed)
	    c = hkmap(c);		// Hebrew mode mapping
#endif

	// If the window was made so small that nothing shows, make it at least
	// one line and one column when typing.
	ଯଦି (ଚାବି_Typed && !ଚାବି_Stuffed)
	    win_ensure_size();

	/*
	 * Special handling of keys while the popup menu is visible or wanted
	 * and the cursor is still in the completed word.  Only when there is
	 * a match, skip this when no matches were found.
	 */
	ଯଦି (ins_compl_active() && curwin->w_cursor.col >= ins_compl_col()
		&& ins_compl_has_shown_match() && pum_wanted())
	{
	    // BS: Delete one character from "compl_leader".
	    ଯଦି ((c == K_BS || c == Ctrl_H)
			&& curwin->w_cursor.col > ins_compl_col()
			&& (c = ins_compl_bs()) == NUL)
		ଜାରି;

	    // When no match was selected or it was edited.
	    ଯଦି (!ins_compl_used_match())
	    {
		// CTRL-L: Add one character from the current match to
		// "compl_leader".  Except when at the original match and
		// there is nothing to add, CTRL-L works like CTRL-P then.
		ଯଦି (c == Ctrl_L
			&& (!ctrl_x_mode_ଧାଡ଼ି_or_eval()
			    || ins_compl_long_shown_match()))
		{
		    ins_compl_addfrommatch();
		    ଜାରି;
		}

		// A non-white character that fits in with the current
		// completion: Add to "compl_leader".
		ଯଦି (ins_compl_accept_char(c))
		{
#ଯଦି defined(FEAT_EVAL)
		    // Trigger InsertCharPre.
		    char_u *str = do_ଭର୍ତ୍ତି_char_pre(c);
		    char_u *p;

		    ଯଦି (str != ଖାଲି)
		    {
			ପାଇଁ (p = str; *p != NUL; MB_PTR_ADV(p))
			    ins_compl_addleader(PTR2CHAR(p));
			vim_free(str);
		    }
		    ଅନ୍ୟଥା
#endif
			ins_compl_addleader(c);
		    ଜାରି;
		}

		// Pressing CTRL-Y selects the current match.  When
		// ins_compl_enter_selects() is set the Enter key does the
		// same.
		ଯଦି ((c == Ctrl_Y || (ins_compl_enter_selects()
				    && (c == CAR || c == K_KENTER || c == NL)))
			&& stop_arrow() == OK)
		{
		    ins_compl_delete();
		    ଯଦି (ins_compl_preinsert_longest()
			    && !ins_compl_is_match_selected())
		    {
			ins_compl_ଭର୍ତ୍ତି(FALSE, TRUE);
			ins_compl_init_get_longest();
			ଜାରି;
		    }
		    ଅନ୍ୟଥା
			ins_compl_ଭର୍ତ୍ତି(FALSE, FALSE);
		}
		// Delete preinserted text when typing special chars
		ଅନ୍ୟଥା ଯଦି (IS_WHITE_NL_OR_NUL(c) && ins_compl_preinsert_effect())
		    ins_compl_delete();
	    }
	}

	// Prepare for or stop CTRL-X mode.  This doesn't do completion, but
	// it does fix up the text when finishing completion.
	ins_compl_init_get_longest();
	ଯଦି (ins_compl_prep(c))
	    ଜାରି;

	// CTRL-\ CTRL-N goes to Normal mode,
	// CTRL-\ CTRL-G goes to mode selected with 'insertmode',
	// CTRL-\ CTRL-O is like CTRL-O but without moving the cursor.
	ଯଦି (c == Ctrl_BSL)
	{
	    // may need to redraw when no more chars available now
	    ins_redraw(FALSE);
	    ++no_mapping;
	    ++allow_ଚାବିଗୁଡ଼ିକ;
	    c = plain_vgetc();
	    --no_mapping;
	    --allow_ଚାବିଗୁଡ଼ିକ;
	    ଯଦି (c != Ctrl_N && c != Ctrl_G && c != Ctrl_O)
	    {
		// it's something else
		vungetc(c);
		c = Ctrl_BSL;
	    }
	    ଅନ୍ୟଥା ଯଦି (c == Ctrl_G && p_im)
		ଜାରି;
	    ଅନ୍ୟଥା
	    {
		ଯଦି (c == Ctrl_O)
		{
		    ins_ctrl_o();
		    ins_at_eol = FALSE;	// cursor keeps its column
		    nomove = TRUE;
		}
		ଗଣନା_2 = 0;
		ଯାଅ doESCkey;
	    }
	}

#ifdef FEAT_DIGRAPHS
	c = do_digraph(c);
#endif

	ଯଦି ((c == Ctrl_V || c == Ctrl_Q) && ctrl_x_mode_cmdline())
	    ଯାଅ docomplete;
	ଯଦି (c == Ctrl_V || c == Ctrl_Q)
	{
	    ins_ctrl_v();
	    c = Ctrl_V;	// pretend CTRL-V is last typed character
	    ଜାରି;
	}

	ଯଦି (cindent_on() && ctrl_x_mode_none())
	{
	    // A key name preceded by a bang means this key is not to be
	    // inserted.  Skip ahead to the re-indenting below.
	    // A key name preceded by a star means that indenting has to be
	    // done before inserting the key.
	    ଧାଡ଼ି_is_white = inindent(0);
	    ଯଦି (in_cinkeys(c, '!', ଧାଡ଼ି_is_white))
		ଯାଅ force_cindent;
	    ଯଦି (can_cindent && in_cinkeys(c, '*', ଧାଡ଼ି_is_white)
							&& stop_arrow() == OK)
		do_c_ପଦସମଷ୍ଟି_indent();
	}

#ifdef FEAT_RIGHTLEFT
	ଯଦି (curwin->w_p_rl)
	    ବାଛ (c)
	    {
		ସ୍ଥିତି K_ବାମ:	c = K_ଡାହାଣ; ଭାଙ୍ଗ;
		ସ୍ଥିତି K_S_ବାମ:	c = K_S_ଡାହାଣ; ଭାଙ୍ଗ;
		ସ୍ଥିତି K_C_ବାମ:	c = K_C_ଡାହାଣ; ଭାଙ୍ଗ;
		ସ୍ଥିତି K_ଡାହାଣ:	c = K_ବାମ; ଭାଙ୍ଗ;
		ସ୍ଥିତି K_S_ଡାହାଣ: c = K_S_ବାମ; ଭାଙ୍ଗ;
		ସ୍ଥିତି K_C_ଡାହାଣ: c = K_C_ବାମ; ଭାଙ୍ଗ;
	    }
#endif

	/*
	 * If 'keymodel' contains "startsel", may start selection.  If it
	 * does, a CTRL-O and c will be stuffed, we need to get these
	 * characters.
	 */
	ଯଦି (ins_start_select(c))
	    ଜାରି;

	/*
	 * The big switch to handle a character in insert mode.
	 */
	ବାଛ (c)
	{
	ସ୍ଥିତି ESC:	// End input mode
	    ଯଦି (echeck_abbr(ESC + ABBR_OFF))
		ଭାଙ୍ଗ;
	    // FALLTHROUGH

	ସ୍ଥିତି Ctrl_C:	// End input mode
	    ଯଦି (c == Ctrl_C && cmdwin_type != 0)
	    {
		// Close the cmdline window.
		cmdwin_result = K_IGNORE;
		got_int = FALSE; // don't stop executing autocommands et al.
		nomove = TRUE;
		ଯାଅ doESCkey;
	    }
#ifdef FEAT_JOB_CHANNEL
	    ଯଦି (c == Ctrl_C && bt_prompt(curbuf))
	    {
		ଯଦି (invoke_prompt_interrupt())
		{
		    ଯଦି (!bt_prompt(curbuf))
			// buffer changed to a non-prompt buffer, get out of
			// Insert mode
			ଯାଅ doESCkey;
		    ଭାଙ୍ଗ;
		}
	    }
#endif

#ifdef UNIX
do_intr:
#endif
	    // when 'insertmode' set, and not halfway a mapping, don't leave
	    // Insert mode
	    ଯଦି (goto_im())
	    {
		ଯଦି (got_int)
		{
		    (ଶୂନ୍ୟ)vgetc();		// flush all buffers
		    got_int = FALSE;
		}
		ଅନ୍ୟଥା
		    vim_beep(BO_IM);
		ଭାଙ୍ଗ;
	    }
doESCkey:
	    // Drop a pending autocomplete so it does not outlive Insert mode.
	    ins_compl_clear_autocomplete_delay();
	    /*
	     * This is the ONLY return from edit()!
	     */
	    // Always update o_lnum, so that a "CTRL-O ." that adds a line
	    // still puts the cursor back after the inserted text.
	    ଯଦି (ins_at_eol && gchar_cursor() == NUL)
		o_lnum = curwin->w_cursor.lnum;

	    ଯଦି (ins_esc(&ଗଣନା_2, cmdchar, nomove))
	    {
		// When CTRL-C was typed got_int will be set, with the result
		// that the autocommands won't be executed. When mapped got_int
		// is not set, but let's keep the behavior the same.
		ଯଦି (cmdchar != 'r' && cmdchar != 'v' && c != Ctrl_C)
		    ins_apply_autocmds(EVENT_INSERTLEAVE);
		did_cursorhold = FALSE;

		// ins_redraw() triggers TextChangedI only when no characters
		// are in the typeahead buffer, so reset curbuf->b_last_changedtick only
		// if the TextChangedI was not blocked by char_avail() (e.g. using :norm!)
		// and the TextChangedI autocommand has been triggered.
		ଯଦି (!char_avail() && curbuf->b_last_changedtick_i == CHANGEDTICK(curbuf))
		    curbuf->b_last_changedtick = CHANGEDTICK(curbuf);
		ଫେରନ୍ତୁ (c == Ctrl_O);
	    }
	    ଜାରି;

	ସ୍ଥିତି Ctrl_Z:	// suspend when 'insertmode' set
	    ଯଦି (!p_im)
		ଯାଅ normalchar;	// insert CTRL-Z as normal char
	    do_cmdline_cmd((char_u *)"stop");
#ifdef CURSOR_SHAPE
	    ui_cursor_shape();		// may need to update cursor shape
#endif
	    ଜାରି;

	ସ୍ଥିତି Ctrl_O:	// execute one command
#ifdef FEAT_COMPL_FUNC
	    ଯଦି (ctrl_x_mode_omni())
		ଯାଅ docomplete;
#endif
	    ଯଦି (echeck_abbr(Ctrl_O + ABBR_OFF))
		ଭାଙ୍ଗ;
	    ins_ctrl_o();

	    // don't move the cursor left when 'virtualedit' has "onemore".
	    ଯଦି (get_ve_flags() & VE_ONEMORE)
	    {
		ins_at_eol = FALSE;
		nomove = TRUE;
	    }
	    ଗଣନା_2 = 0;
	    ଯାଅ doESCkey;

	ସ୍ଥିତି K_INS:	// toggle insert/replace mode
	ସ୍ଥିତି K_KINS:
	    ins_ଭର୍ତ୍ତି(replaceState);
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି K_SELECT:	// end of Select mode mapping - ignore
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି K_HELP:	// Help key works like <ESC> <Help>
	ସ୍ଥିତି K_F1:
	ସ୍ଥିତି K_XF1:
	    stuffcharReadbuff(K_HELP);
	    ଯଦି (p_im)
		need_start_insertmode = TRUE;
	    ଯାଅ doESCkey;

#ifdef FEAT_NETBEANS_INTG
	ସ୍ଥିତି K_F21:	// NetBeans command
	    ++no_mapping;		// don't map the next key hits
	    i = plain_vgetc();
	    --no_mapping;
	    netbeans_keycommand(i);
	    ଭାଙ୍ଗ;
#endif

	ସ୍ଥିତି K_ZERO:	// Insert the previously inserted text.
	ସ୍ଥିତି NUL:
	ସ୍ଥିତି Ctrl_A:
	    // For ^@ the trailing ESC will end the insert, unless there is an
	    // error.
	    ଯଦି (stuff_inserted(NUL, 1L, (c == Ctrl_A)) == FAIL
						   && c != Ctrl_A && !p_im)
		ଯାଅ doESCkey;		// quit insert mode
	    inserted_space = FALSE;
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି Ctrl_R:	// insert the contents of a register
	    ଯଦି (ctrl_x_mode_register() && !ins_compl_active())
		ଯାଅ docomplete;
	    ins_reg();
	    auto_format(FALSE, TRUE);
	    inserted_space = FALSE;
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି Ctrl_G:	// commands starting with CTRL-G
	    ins_ctrl_g();
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି Ctrl_HAT:	// switch input mode and/or langmap
	    ins_ctrl_hat();
	    ଭାଙ୍ଗ;

#ifdef FEAT_RIGHTLEFT
	ସ୍ଥିତି Ctrl__:	// switch between languages
	    ଯଦି (!p_ari)
		ଯାଅ normalchar;
	    ins_ctrl_();
	    ଭାଙ୍ଗ;
#endif

	ସ୍ଥିତି Ctrl_D:	// Make indent one shiftwidth smaller.
#ଯଦି defined(FEAT_FIND_ID)
	    ଯଦି (ctrl_x_mode_path_defines())
		ଯାଅ docomplete;
#endif
	    // FALLTHROUGH

	ସ୍ଥିତି Ctrl_T:	// Make indent one shiftwidth greater.
	    ଯଦି (c == Ctrl_T && ctrl_x_mode_thesaurus())
	    {
		ଯଦି (has_compl_option(FALSE))
		    ଯାଅ docomplete;
		ଭାଙ୍ଗ;
	    }

	    ins_shift(c, lastc);
	    auto_format(FALSE, TRUE);
	    inserted_space = FALSE;
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି K_DEL:	// delete character under the cursor
	ସ୍ଥିତି K_KDEL:
	    ins_del();
	    auto_format(FALSE, TRUE);
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି K_BS:	// delete character before the cursor
	ସ୍ଥିତି K_S_BS:
	ସ୍ଥିତି Ctrl_H:
	    did_backspace = ins_bs(c, BACKSPACE_CHAR, &inserted_space);
	    auto_format(FALSE, TRUE);
	    ଯଦି (did_backspace)
		MAY_TRIGGER_AUTOCOMPLETE(c);
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି Ctrl_W:	// delete word before the cursor
#ifdef FEAT_JOB_CHANNEL
	    ଯଦି (bt_prompt(curbuf) && (mod_mask & MOD_MASK_SHIFT) == 0)
	    {
		// In a prompt window CTRL-W is used for window commands.
		// Use Shift-CTRL-W to delete a word.
		stuffcharReadbuff(Ctrl_W);
		restart_edit = 'A';
		nomove = TRUE;
		ଗଣନା_2 = 0;
		ଯାଅ doESCkey;
	    }
#endif
	    did_backspace = ins_bs(c, BACKSPACE_WORD, &inserted_space);
	    auto_format(FALSE, TRUE);
	    ଯଦି (did_backspace)
		MAY_TRIGGER_AUTOCOMPLETE(c);
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି Ctrl_U:	// delete all inserted text in current line
#ifdef FEAT_COMPL_FUNC
	    // CTRL-X CTRL-U completes with 'completefunc'.
	    ଯଦି (ctrl_x_mode_function())
		ଯାଅ docomplete;
#endif
	    did_backspace = ins_bs(c, BACKSPACE_ଧାଡ଼ି, &inserted_space);
	    auto_format(FALSE, TRUE);
	    inserted_space = FALSE;
	    ଯଦି (did_backspace)
		MAY_TRIGGER_AUTOCOMPLETE(c);
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି K_LEFTMOUSE:   // mouse keys
	ସ୍ଥିତି K_LEFTMOUSE_NM:
	ସ୍ଥିତି K_LEFTDRAG:
	ସ୍ଥିତି K_LEFTRELEASE:
	ସ୍ଥିତି K_LEFTRELEASE_NM:
	ସ୍ଥିତି K_MOUSEMOVE:
	ସ୍ଥିତି K_MIDDLEMOUSE:
	ସ୍ଥିତି K_MIDDLEDRAG:
	ସ୍ଥିତି K_MIDDLERELEASE:
	ସ୍ଥିତି K_RIGHTMOUSE:
	ସ୍ଥିତି K_RIGHTDRAG:
	ସ୍ଥିତି K_RIGHTRELEASE:
	ସ୍ଥିତି K_X1MOUSE:
	ସ୍ଥିତି K_X1DRAG:
	ସ୍ଥିତି K_X1RELEASE:
	ସ୍ଥିତି K_X2MOUSE:
	ସ୍ଥିତି K_X2DRAG:
	ସ୍ଥିତି K_X2RELEASE:
	    ins_mouse(c);
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି K_MOUSEDOWN: // Default action for scroll wheel up: scroll up
	    ins_mousescroll(MSCR_DOWN);
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି K_MOUSEUP:	// Default action for scroll wheel down: scroll down
	    ins_mousescroll(MSCR_UP);
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି K_MOUSELEFT: // Scroll wheel left
	    ins_mousescroll(MSCR_ବାମ);
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି K_MOUSERIGHT: // Scroll wheel right
	    ins_mousescroll(MSCR_ଡାହାଣ);
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି K_PS:
	    bracketed_paste(PASTE_ଭର୍ତ୍ତି, FALSE, ଖାଲି);
	    ଯଦି (cmdchar == K_PS)
		// invoked from normal mode, bail out
		ଯାଅ doESCkey;
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି K_PE:
	    // Got K_PE without K_PS, ignore.
	    ଭାଙ୍ଗ;

#ifdef FEAT_GUI_TABLINE
	ସ୍ଥିତି K_TABLINE:
	ସ୍ଥିତି K_TABMENU:
	    ins_tabline(c);
	    ଭାଙ୍ଗ;
#endif

	ସ୍ଥିତି K_IGNORE:	// Something mapped to nothing
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି K_COMMAND:		    // <Cmd>command<CR>
	ସ୍ଥିତି K_SCRIPT_COMMAND:	    // <ScriptCmd>command<CR>
	    {
		bufref_T    save_curbuf;
		varnumber_T tick = CHANGEDTICK(curbuf);

		set_bufref(&save_curbuf, curbuf);
		do_cmdkey_command(c, 0);

#ifdef FEAT_TERMINAL
		ଯଦି (term_use_loop())
		    // Started a terminal that gets the input, exit Insert mode.
		    ଯାଅ doESCkey;
#endif
		ଯଦି (curbuf->b_u_synced
			|| (bufref_valid(&save_curbuf)
			    && curbuf == save_curbuf.br_buf
			    && tick != CHANGEDTICK(curbuf)))
		{
		    // The command synced undo or changed this buffer.
		    // Save the cursor line before the next typed edit.
		    ins_need_undo = TRUE;
		}
	    }
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି K_CURSORHOLD:	// Didn't type something for a while.
	    ins_apply_autocmds(EVENT_CURSORHOLDI);
	    did_cursorhold = TRUE;
	    // If CTRL-G U was used apply it to the next typed key.
	    ଯଦି (dont_sync_undo == TRUE)
		dont_sync_undo = MAYBE;
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି K_COMPLETE_DELAY:	// 'autocompletedelay' expired
	    // If CTRL-G U was used apply it to the next typed key.
	    ଯଦି (dont_sync_undo == TRUE)
		dont_sync_undo = MAYBE;
	    ins_compl_clear_autocomplete_delay();
	    ଯଦି (!ins_compl_has_autocomplete() || char_avail()
		    || curwin->w_cursor.col == 0)
		ଭାଙ୍ଗ;
	    c = char_before_cursor();
	    ଯଦି (!vim_isprintc(c))
		ଭାଙ୍ଗ;
	    // The completion may have been cleared while waiting, so re-enable
	    // autocomplete to match a zero delay.
	    ins_compl_enable_autocomplete();
	    ins_compl_arm_autostart();
	    ଯାଅ docomplete;

#ifdef FEAT_GUI_MSWIN
	    // On MS-Windows ignore <M-F4>, we get it when closing the window
	    // was cancelled.
	ସ୍ଥିତି K_F4:
	    ଯଦି (mod_mask != MOD_MASK_ALT)
		ଯାଅ normalchar;
	    ଭାଙ୍ଗ;
#endif

#ifdef FEAT_GUI
	ସ୍ଥିତି K_VER_SCROLLBAR:
	    ins_scroll();
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି K_HOR_SCROLLBAR:
	    ins_horscroll();
	    ଭାଙ୍ଗ;
#endif

	ସ୍ଥିତି K_HOME:	// <Home>
	ସ୍ଥିତି K_KHOME:
	ସ୍ଥିତି K_S_HOME:
	ସ୍ଥିତି K_C_HOME:
	    ins_home(c);
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି K_END:	// <End>
	ସ୍ଥିତି K_KEND:
	ସ୍ଥିତି K_S_END:
	ସ୍ଥିତି K_C_END:
	    ins_end(c);
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି K_ବାମ:	// <Left>
	    ଯଦି (mod_mask & (MOD_MASK_SHIFT|MOD_MASK_CTRL))
		ins_s_ବାମ();
	    ଅନ୍ୟଥା
		ins_ବାମ();
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି K_S_ବାମ:	// <S-Left>
	ସ୍ଥିତି K_C_ବାମ:
	    ins_s_ବାମ();
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି K_ଡାହାଣ:	// <Right>
	    ଯଦି (mod_mask & (MOD_MASK_SHIFT|MOD_MASK_CTRL))
		ins_s_ଡାହାଣ();
	    ଅନ୍ୟଥା
		ins_ଡାହାଣ();
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି K_S_ଡାହାଣ:	// <S-Right>
	ସ୍ଥିତି K_C_ଡାହାଣ:
	    ins_s_ଡାହାଣ();
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି K_UP:	// <Up>
	    ଯଦି (pum_visible())
		ଯାଅ docomplete;
	    ଯଦି (mod_mask & MOD_MASK_SHIFT)
		ins_pageup();
	    ଅନ୍ୟଥା
		ins_up(FALSE);
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି K_S_UP:	// <S-Up>
	ସ୍ଥିତି K_PAGEUP:
	ସ୍ଥିତି K_KPAGEUP:
	    ଯଦି (pum_visible())
	    {
#ifdef FEAT_PROP_POPUP
		// CTRL-SHIFT-<Up> scrolls the info popup up a line,
		// CTRL-SHIFT-<PageUp> a page.  Shift is folded into K_S_UP but
		// stays in mod_mask for PageUp, hence the asymmetric check.
		ଯଦି (c == K_S_UP ? (mod_mask & MOD_MASK_CTRL)
			: ((mod_mask & MOD_MASK_CTRL)
			    && (mod_mask & MOD_MASK_SHIFT)))
		{
		    popup_scroll_info(-1, c != K_S_UP);
		    ଭାଙ୍ଗ;
		}
#endif
		ଯାଅ docomplete;
	    }
	    ins_pageup();
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି K_DOWN:	// <Down>
	    ଯଦି (pum_visible())
		ଯାଅ docomplete;
	    ଯଦି (mod_mask & MOD_MASK_SHIFT)
		ins_pagedown();
	    ଅନ୍ୟଥା
		ins_down(FALSE);
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି K_S_DOWN:	// <S-Down>
	ସ୍ଥିତି K_PAGEDOWN:
	ସ୍ଥିତି K_KPAGEDOWN:
	    ଯଦି (pum_visible())
	    {
#ifdef FEAT_PROP_POPUP
		// CTRL-SHIFT-<Down>/<PageDown> scroll the info popup down.
		ଯଦି (c == K_S_DOWN ? (mod_mask & MOD_MASK_CTRL)
			: ((mod_mask & MOD_MASK_CTRL)
			    && (mod_mask & MOD_MASK_SHIFT)))
		{
		    popup_scroll_info(1, c != K_S_DOWN);
		    ଭାଙ୍ଗ;
		}
#endif
		ଯାଅ docomplete;
	    }
	    ins_pagedown();
	    ଭାଙ୍ଗ;

#ifdef FEAT_DND
	ସ୍ଥିତି K_DROP:	// drag-n-drop event
	    ins_drop();
	    ଭାଙ୍ଗ;
#endif

	ସ୍ଥିତି K_S_TAB:	// When not mapped, use like a normal TAB
	    c = TAB;
	    // FALLTHROUGH

	ସ୍ଥିତି TAB:	// TAB or Complete patterns along path
#ଯଦି defined(FEAT_FIND_ID)
	    ଯଦି (ctrl_x_mode_path_patterns())
		ଯାଅ docomplete;
#endif
	    inserted_space = FALSE;
	    ଯଦି (ins_tab())
		ଯାଅ normalchar;	// insert TAB as a normal char
	    auto_format(FALSE, TRUE);
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି K_KENTER:	// <Enter>
	    c = CAR;
	    // FALLTHROUGH
	ସ୍ଥିତି CAR:
	ସ୍ଥିତି NL:
#ଯଦି defined(FEAT_QUICKFIX)
	    // In a quickfix window a <CR> jumps to the error under the
	    // cursor.
	    ଯଦି (bt_quickfix(curbuf) && c == CAR)
	    {
		ଯଦି (curwin->w_llist_ref == ଖାଲି)    // quickfix window
		    do_cmdline_cmd((char_u *)".cc");
		ଅନ୍ୟଥା				    // location list window
		    do_cmdline_cmd((char_u *)".ll");
		ଭାଙ୍ଗ;
	    }
#endif
	    ଯଦି (cmdwin_type != 0)
	    {
		// Execute the command in the cmdline window.
		cmdwin_result = CAR;
		ଯାଅ doESCkey;
	    }
#ifdef FEAT_JOB_CHANNEL
	    ଯଦି (bt_prompt(curbuf))
	    {
		invoke_prompt_callback();
		ଯଦି (!bt_prompt(curbuf))
		    // buffer changed to a non-prompt buffer, get out of
		    // Insert mode
		    ଯାଅ doESCkey;
		ଭାଙ୍ଗ;
	    }
#endif
	    ଯଦି (ins_eol(c) == FAIL && !p_im)
		ଯାଅ doESCkey;	    // out of memory
	    auto_format(FALSE, FALSE);
	    inserted_space = FALSE;
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି Ctrl_K:	    // digraph or keyword completion
	    ଯଦି (ctrl_x_mode_dictionary())
	    {
		ଯଦି (has_compl_option(TRUE))
		    ଯାଅ docomplete;
		ଭାଙ୍ଗ;
	    }
#ifdef FEAT_DIGRAPHS
	    c = ins_digraph();
	    ଯଦି (c == NUL)
		ଭାଙ୍ଗ;
#endif
	    ଯାଅ normalchar;

	ସ୍ଥିତି Ctrl_X:	// Enter CTRL-X mode
	    ins_ctrl_x();
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି Ctrl_RSB:	// Tag name completion after ^X
	    ଯଦି (!ctrl_x_mode_tags())
		ଯାଅ normalchar;
	    ଯାଅ docomplete;

	ସ୍ଥିତି Ctrl_F:	// File name completion after ^X
	    ଯଦି (!ctrl_x_mode_files())
		ଯାଅ normalchar;
	    ଯାଅ docomplete;

	ସ୍ଥିତି 's':	// Spelling completion after ^X
	ସ୍ଥିତି Ctrl_S:
	    ଯଦି (!ctrl_x_mode_spell())
		ଯାଅ normalchar;
	    ଯାଅ docomplete;

	ସ୍ଥିତି Ctrl_L:	// Whole line completion after ^X
	    ଯଦି (!ctrl_x_mode_whole_ଧାଡ଼ି())
	    {
		// CTRL-L with 'insertmode' set: Leave Insert mode
		ଯଦି (p_im)
		{
		    ଯଦି (echeck_abbr(Ctrl_L + ABBR_OFF))
			ଭାଙ୍ଗ;
		    ଯାଅ doESCkey;
		}
		ଯାଅ normalchar;
	    }
	    // FALLTHROUGH

	ସ୍ଥିତି Ctrl_P:	// Do previous/next pattern completion
	ସ୍ଥିତି Ctrl_N:
#ifdef FEAT_PROP_POPUP
	    // CTRL-SHIFT-P/N scroll the info popup one line.
	    ଯଦି (pum_visible() && (mod_mask & MOD_MASK_SHIFT)
		    && (c == Ctrl_P || c == Ctrl_N))
	    {
		popup_scroll_info(c == Ctrl_P ? -1 : 1, false);
		ଭାଙ୍ଗ;
	    }
#endif
	    // if 'complete' is empty then plain ^P is no longer special,
	    // but it is under other ^X modes
	    ଯଦି (*curbuf->b_p_cpt == NUL
		    && (ctrl_x_mode_normal() || ctrl_x_mode_whole_ଧାଡ଼ି())
		    && !compl_status_local())
		ଯାଅ normalchar;

docomplete:
	    ins_compl_clear_autocomplete_delay();
	    compl_busy = TRUE;
#ifdef FEAT_FOLDING
	    disable_fold_update++;  // don't redraw folds here
#endif
	    ଯଦି (ins_complete(c, TRUE) == FAIL)
		compl_status_clear();
#ifdef FEAT_FOLDING
	    disable_fold_update--;
#endif
	    compl_busy = FALSE;
	    can_si = may_do_si(); // allow smartindenting
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି Ctrl_Y:	// copy from previous line or scroll down
	ସ୍ଥିତି Ctrl_E:	// copy from next line	   or scroll up
	    c = ins_ctrl_ey(c);
	    ଭାଙ୍ଗ;

	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
#ifdef UNIX
	    ଯଦି (c == intr_char)		// special interrupt char
		ଯାଅ do_intr;
#endif

normalchar:
	    /*
	     * Insert a normal character.
	     */
#ଯଦି defined(FEAT_EVAL)
	    ଯଦି (!p_paste)
	    {
		// Trigger InsertCharPre.
		char_u *str = do_ଭର୍ତ୍ତି_char_pre(c);
		char_u *p;

		ଯଦି (str != ଖାଲି)
		{
		    ଯଦି (*str != NUL && stop_arrow() != FAIL)
		    {
			// Insert the new value of v:char literally.
			ପାଇଁ (p = str; *p != NUL; MB_PTR_ADV(p))
			{
			    c = PTR2CHAR(p);
			    ଯଦି (c == CAR || c == K_KENTER || c == NL)
				ins_eol(c);
			    ଅନ୍ୟଥା
				ins_char(c);
			}
			AppendToRedobuffLit(str, -1);
		    }
		    vim_free(str);
		    c = NUL;
		}

		// If the new value is already inserted or an empty string
		// then don't insert any character.
		ଯଦି (c == NUL)
		    ଭାଙ୍ଗ;
	    }
#endif
	    // Try to perform smart-indenting.
	    ins_try_si(c);

	    ଯଦି (c == ' ')
	    {
		inserted_space = TRUE;
		ଯଦି (inindent(0))
		    can_cindent = FALSE;
		ଯଦି (Insstart_blank_vcol == MAXCOL
			&& curwin->w_cursor.lnum == Insstart.lnum)
		    Insstart_blank_vcol = get_nolist_virtcol();
	    }

	    // Insert a normal character and check for abbreviations on a
	    // special character.  Let CTRL-] expand abbreviations without
	    // inserting it.
	    ଯଦି (vim_iswordc(c) || (!echeck_abbr(
			// Add ABBR_OFF for characters above 0x100, this is
			// what check_abbr() expects.
				(has_mbyte && c >= 0x100) ? (c + ABBR_OFF) : c)
			&& c != Ctrl_RSB))
	    {
		ଭର୍ତ୍ତି_special(c, FALSE, FALSE);
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
	    ଯଦି (ins_compl_has_autocomplete() && !char_avail()
		    && vim_isprintc(c))
		TRIGGER_AUTOCOMPLETE();

	    ଭାଙ୍ଗ;
	}   // end of switch (c)

	// If typed something may trigger CursorHoldI again; K_COMPLETE_DELAY is
	// injected, not typed.
	ଯଦି (c != K_CURSORHOLD && c != K_COMPLETE_DELAY
#ifdef FEAT_COMPL_FUNC
		// but not in CTRL-X mode, a script can't restore the state
		&& ctrl_x_mode_normal()
#endif
	       )
	    did_cursorhold = FALSE;

	// Check if we need to cancel completion mode because the window
	// or tab page was changed
	ଯଦି (ins_compl_active() && !ins_compl_win_active(curwin))
	    ins_compl_cancel();

	// If the cursor was moved we didn't just insert a space
	ଯଦି (arrow_used)
	    inserted_space = FALSE;

	ଯଦି (can_cindent && cindent_on() && ctrl_x_mode_normal())
	{
force_cindent:
	    /*
	     * Indent now if a key was typed that is in 'cinkeys'.
	     */
	    ଯଦି (in_cinkeys(c, ' ', ଧାଡ଼ି_is_white))
	    {
		ଯଦି (stop_arrow() == OK)
		    // re-indent the current line
		    do_c_ପଦସମଷ୍ଟି_indent();
	    }
	}

    }	// for (;;)
    // NOTREACHED
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ins_need_undo_get(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ ins_need_undo;
}

/*
 * Redraw for Insert mode.
 * This is postponed until getting the next character to make '$' in the 'cpo'
 * option work correctly.
 * Only redraw when there are no characters available.  This speeds up
 * inserting sequences of characters (e.g., for CTRL-R).
 */
    ଶୂନ୍ୟ
ins_redraw(ପୂର୍ଣ୍ଣସଂଖ୍ୟା ready)	    // not busy with something
{
#ifdef FEAT_CONCEAL
    linenr_T	conceal_old_cursor_ଧାଡ଼ି = 0;
    linenr_T	conceal_new_cursor_ଧାଡ଼ି = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		conceal_update_ଧାଡ଼ିଗୁଡ଼ିକ = FALSE;
#endif

    ଯଦି (char_avail())
	ଫେରନ୍ତୁ;

    // Trigger CursorMoved if the cursor moved.  Not when the popup menu is
    // visible, the command might delete it.
    ଯଦି (ready && (has_cursormovedI()
#ifdef FEAT_PROP_POPUP
		|| popup_visible
#endif
#ଯଦି defined(FEAT_CONCEAL)
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
	ଯଦି (syntax_present(curwin) && must_redraw)
	    update_screen(0);
#endif
	ଯଦି (has_cursormovedI())
	{
	    // Make sure curswant is correct, an autocommand may call
	    // getcurpos().
	    update_curswant();
	    ins_apply_autocmds(EVENT_CURSORMOVEDI);
	}
#ifdef FEAT_PROP_POPUP
	ଯଦି (popup_visible)
	    popup_check_cursor_pos();
#endif
#ifdef FEAT_CONCEAL
	ଯଦି (curwin->w_p_cole > 0)
	{
	    conceal_old_cursor_ଧାଡ଼ି = last_cursormoved.lnum;
	    conceal_new_cursor_ଧାଡ଼ି = curwin->w_cursor.lnum;
	    conceal_update_ଧାଡ଼ିଗୁଡ଼ିକ = TRUE;
	}
#endif
	last_cursormoved = curwin->w_cursor;
    }

    // Trigger TextChangedI if b_changedtick_i differs.
    ଯଦି (ready && has_textchangedI()
	    && curbuf->b_last_changedtick_i != CHANGEDTICK(curbuf)
	    && !pum_visible())
    {
	aco_save_T	aco;
	varnumber_T	tick = CHANGEDTICK(curbuf);

	// Save and restore curwin and curbuf, in case the autocmd changes
	// them.
	aucmd_prepbuf(&aco, curbuf);
	apply_autocmds(EVENT_TEXTCHANGEDI, ଖାଲି, ଖାଲି, FALSE, curbuf);
	aucmd_restbuf(&aco);
	curbuf->b_last_changedtick_i = CHANGEDTICK(curbuf);
	ଯଦି (tick != CHANGEDTICK(curbuf))  // see ins_apply_autocmds()
	    u_save(curwin->w_cursor.lnum,
					(linenr_T)(curwin->w_cursor.lnum + 1));
    }

    // Trigger TextChangedP if b_changedtick_pum differs. When the popupmenu
    // closes TextChangedI will need to trigger for backwards compatibility,
    // thus use different b_last_changedtick* variables.
    ଯଦି (ready && has_textchangedP()
	    && curbuf->b_last_changedtick_pum != CHANGEDTICK(curbuf)
	    && pum_visible())
    {
	aco_save_T	aco;
	varnumber_T	tick = CHANGEDTICK(curbuf);

	// Save and restore curwin and curbuf, in case the autocmd changes
	// them.
	aucmd_prepbuf(&aco, curbuf);
	apply_autocmds(EVENT_TEXTCHANGEDP, ଖାଲି, ଖାଲି, FALSE, curbuf);
	aucmd_restbuf(&aco);
	curbuf->b_last_changedtick_pum = CHANGEDTICK(curbuf);
	ଯଦି (tick != CHANGEDTICK(curbuf))  // see ins_apply_autocmds()
	    u_save(curwin->w_cursor.lnum,
					(linenr_T)(curwin->w_cursor.lnum + 1));
    }

    ଯଦି (ready)
	may_trigger_win_scrolled_resized();

    // Trigger SafeState if nothing is pending.
    may_trigger_safestate(ready
	    && !ins_compl_active()
	    && !pum_visible());

#ଯଦି defined(FEAT_CONCEAL)
    ଯଦି ((conceal_update_ଧାଡ଼ିଗୁଡ଼ିକ
	    && (conceal_old_cursor_ଧାଡ଼ି != conceal_new_cursor_ଧାଡ଼ି
		|| conceal_cursor_ଧାଡ଼ି(curwin)))
	    || need_cursor_ଧାଡ଼ି_redraw)
    {
	ଯଦି (conceal_old_cursor_ଧାଡ଼ି != conceal_new_cursor_ଧାଡ଼ି)
	    redrawWinline(curwin, conceal_old_cursor_ଧାଡ଼ି);
	redrawWinline(curwin, conceal_new_cursor_ଧାଡ଼ି == 0
			    ? curwin->w_cursor.lnum : conceal_new_cursor_ଧାଡ଼ି);
	curwin->w_valid &= ~VALID_CROW;
	need_cursor_ଧାଡ଼ି_redraw = FALSE;
    }
#endif
    ଯଦି (must_redraw)
	update_screen(0);
    ଅନ୍ୟଥା ଯଦି (clear_cmdline || redraw_cmdline)
	showmode();		// clear cmdline and show mode
    showruler(FALSE);
    setcursor();
    emsg_on_display = FALSE;	// may remove error message now
}

/*
 * Handle a CTRL-V or CTRL-Q typed in Insert mode.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
ins_ctrl_v(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		c;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		did_putchar = FALSE;

    // may need to redraw when no more chars available now
    ins_redraw(FALSE);

    ଯଦି (redrawing() && !char_avail())
    {
	edit_putchar('^', TRUE);
	did_putchar = TRUE;
    }
    AppendToRedobuff((char_u *)CTRL_V_STR);	// CTRL-V

    add_to_showcmd_c(Ctrl_V);

    // Do not change any modifyOtherKeys ESC sequence to a normal key for
    // CTRL-SHIFT-V.
    c = get_literal(mod_mask & MOD_MASK_SHIFT);
    ଯଦି (did_putchar)
	// when the line fits in 'columns' the '^' is at the start of the next
	// line and will not removed by the redraw
	edit_unputchar();
    clear_showcmd();

    ଭର୍ତ୍ତି_special(c, FALSE, TRUE);
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
decode_Modify_Other_ଚାବିଗୁଡ଼ିକ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା c)
{
    char_u  *p = typebuf.tb_buf + typebuf.tb_off;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    idx;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    form = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    argidx = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    arg[2] = {0, 0};

    // Recognize:
    // form 0: {lead}{key};{modifier}u
    // form 1: {lead}27;{modifier};{key}~
    ଯଦି (typebuf.tb_len >= 4 && (c == CSI || (c == ESC && *p == '[')))
    {
	idx = (*p == '[');
	ଯଦି (p[idx] == '2' && p[idx + 1] == '7' && p[idx + 2] == ';' &&
		kitty_protocol_state != KKPS_ENABLED)
	{
	    form = 1;
	    idx += 3;
	}
	ଯେତେବେଳେ (idx < typebuf.tb_len && argidx < 2)
	{
	    ଯଦି (p[idx] == ';')
		++argidx;
	    ଅନ୍ୟଥା ଯଦି (VIM_ISDIGIT(p[idx]))
		arg[argidx] = arg[argidx] * 10 + (p[idx] - '0');
	    ଅନ୍ୟଥା
		ଭାଙ୍ଗ;
	    ++idx;
	}
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା kitty_no_mods = argidx == 0 && kitty_protocol_state == KKPS_ENABLED;
	ଯଦି (idx < typebuf.tb_len
		&& p[idx] == (form == 1 ? '~' : 'u')
		&& (argidx == 1 || kitty_no_mods))
	{
	    // Match, consume the code.
	    typebuf.tb_off += idx + 1;
	    typebuf.tb_len -= idx + 1;
#ଯଦି defined(FEAT_CLIENTSERVER) || defined(FEAT_EVAL)
	    ଯଦି (typebuf.tb_len == 0)
		typebuf_was_filled = FALSE;
#endif

	    mod_mask = kitty_no_mods ? 0 : decode_modifiers(arg[!form]);
	    c = merge_modify_Other_ଚାବିଗୁଡ଼ିକ(arg[form], &mod_mask);
	}
    }

    ଫେରନ୍ତୁ c;
}

/*
 * Put a character directly onto the screen.  It's not stored in a buffer.
 * Used while handling CTRL-K, CTRL-V, etc. in Insert mode.
 */
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା  pc_status;
#define PC_STATUS_UNSET	0	// pc_bytes was not set
#define PC_STATUS_ଡାହାଣ	1	// right half of double-wide char
#define PC_STATUS_ବାମ	2	// left half of double-wide char
#define PC_STATUS_SET	3	// pc_bytes was filled
ସ୍ଥିର char_u pc_bytes[MB_MAXBYTES + 1]; // saved bytes
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା  pc_attr;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା  pc_row;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା  pc_col;

    ଶୂନ୍ୟ
edit_putchar(ପୂର୍ଣ୍ଣସଂଖ୍ୟା c, ପୂର୍ଣ୍ଣସଂଖ୍ୟା highlight)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    attr;

    ଯଦି (Screen_ଧାଡ଼ିଗୁଡ଼ିକ == ଖାଲି)
	ଫେରନ୍ତୁ;

    update_topline();	// just in case w_topline isn't valid
    validate_cursor();
    ଯଦି (highlight)
	attr = HL_ATTR(HLF_8);
    ଅନ୍ୟଥା
	attr = 0;
    pc_row = W_WINROW(curwin) + curwin->w_wrow;
    pc_col = curwin->w_wincol;
    pc_status = PC_STATUS_UNSET;
#ifdef FEAT_RIGHTLEFT
    ଯଦି (curwin->w_p_rl)
    {
	pc_col += curwin->w_width - 1 - curwin->w_wcol;
	ଯଦି (has_mbyte)
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା fix_col = mb_fix_col(pc_col, pc_row);

	    ଯଦି (fix_col != pc_col)
	    {
		screen_putchar(' ', pc_row, fix_col, attr);
		--curwin->w_wcol;
		pc_status = PC_STATUS_ଡାହାଣ;
	    }
	}
    }
    ଅନ୍ୟଥା
#endif
    {
	pc_col += curwin->w_wcol;
	ଯଦି (mb_lefthalve(pc_row, pc_col))
	    pc_status = PC_STATUS_ବାମ;
    }

    // save the character to be able to put it back
    ଯଦି (pc_status == PC_STATUS_UNSET)
    {
	screen_getbytes(pc_row, pc_col, pc_bytes, &pc_attr);
	pc_status = PC_STATUS_SET;
    }
    screen_putchar(c, pc_row, pc_col, attr);
}

#ଯଦି defined(FEAT_JOB_CHANNEL)
/*
 * Set the insert start position for when using a prompt buffer.
 */
    ଶୂନ୍ୟ
set_insstart(linenr_T lnum, ପୂର୍ଣ୍ଣସଂଖ୍ୟା col)
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
    ଶୂନ୍ୟ
edit_unputchar(ଶୂନ୍ୟ)
{
    ଯଦି (pc_status != PC_STATUS_UNSET && pc_row >= ସନ୍ଦେଶ_scrolled)
    {
	ଯଦି (pc_status == PC_STATUS_ଡାହାଣ)
	    ++curwin->w_wcol;
	ଯଦି (pc_status == PC_STATUS_ଡାହାଣ || pc_status == PC_STATUS_ବାମ)
	    redrawWinline(curwin, curwin->w_cursor.lnum);
	ଅନ୍ୟଥା
	    screen_puts(pc_bytes, pc_row - ସନ୍ଦେଶ_scrolled, pc_col, pc_attr);
    }
}

/*
 * Called when "$" is in 'cpoptions': display a '$' at the end of the changed
 * text.  Only works when cursor is in the line that changes.
 */
    ଶୂନ୍ୟ
display_dollar(colnr_T col_arg)
{
    colnr_T col = col_arg < 0 ? 0 : col_arg;
    colnr_T save_col;

    ଯଦି (!redrawing())
	ଫେରନ୍ତୁ;

    cursor_off();
    save_col = curwin->w_cursor.col;
    curwin->w_cursor.col = col;
    ଯଦି (has_mbyte)
    {
	char_u *p;

	// If on the last byte of a multi-byte move to the first byte.
	p = ml_get_curline();
	curwin->w_cursor.col -= (*mb_head_off)(p, p + col);
    }
    curs_columns(FALSE);	    // recompute w_wrow and w_wcol
    ଯଦି (curwin->w_wcol < curwin->w_width)
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
    ଶୂନ୍ୟ
undisplay_dollar(ଶୂନ୍ୟ)
{
    ଯଦି (dollar_vcol < 0)
	ଫେରନ୍ତୁ;

    dollar_vcol = -1;
    redrawWinline(curwin, curwin->w_cursor.lnum);
}

/*
 * Truncate the space at the end of a line.  This is to be used only in an
 * insert mode.  It handles fixing the replace stack for MODE_REPLACE and
 * MODE_VREPLACE modes.
 */
    ଶୂନ୍ୟ
truncate_spaces(char_u *ଧାଡ଼ି, size_t len)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    i;

    // find start of trailing white space
    ପାଇଁ (i = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)len - 1; i >= 0 && VIM_ISWHITE(ଧାଡ଼ି[i]); i--)
    {
	ଯଦି (State & REPLACE_FLAG)
	    replace_join(0);	    // remove a NUL from the replace stack
    }
    ଧାଡ଼ି[i + 1] = NUL;
}

/*
 * Backspace the cursor until the given column.  Handles MODE_REPLACE and
 * MODE_VREPLACE modes correctly.  May also be used when not in insert mode at
 * all.  Will attempt not to go before "col" even when there is a composing
 * character.
 */
    ଶୂନ୍ୟ
backspace_until_column(ପୂର୍ଣ୍ଣସଂଖ୍ୟା col)
{
    ଯେତେବେଳେ ((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)curwin->w_cursor.col > col)
    {
	curwin->w_cursor.col--;
	ଯଦି (State & REPLACE_FLAG)
	    replace_do_bs(col);
	ଅନ୍ୟଥା ଯଦି (!del_char_after_col(col))
	    ଭାଙ୍ଗ;
    }
}

/*
 * Like del_char(), but make sure not to go before column "limit_col".
 * Only matters when there are composing characters.
 * Return TRUE when something was deleted.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
del_char_after_col(ପୂର୍ଣ୍ଣସଂଖ୍ୟା limit_col UNUSED)
{
    ଯଦି (enc_utf8 && limit_col >= 0)
    {
	colnr_T ecol = curwin->w_cursor.col + 1;

	// Make sure the cursor is at the start of a character, but
	// skip forward again when going too far back because of a
	// composing character.
	mb_adjust_cursor();
	ଯେତେବେଳେ (curwin->w_cursor.col < (colnr_T)limit_col)
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା l = utf_ptr2len(ml_get_cursor());

	    ଯଦି (l == 0)  // end of line
		ଭାଙ୍ଗ;
	    curwin->w_cursor.col += l;
	}
	ଯଦି (*ml_get_cursor() == NUL || curwin->w_cursor.col == ecol)
	    ଫେରନ୍ତୁ FALSE;
	del_bytes((long)((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ecol - curwin->w_cursor.col), FALSE, TRUE);
    }
    ଅନ୍ୟଥା
	(ଶୂନ୍ୟ)del_char(FALSE);
    ଫେରନ୍ତୁ TRUE;
}

/*
 * Next character is interpreted literally.
 * A one, two or three digit decimal number is interpreted as its byte value.
 * If one or two digits are entered, the next character is given to vungetc().
 * For Unicode a character > 255 may be returned.
 * If "noReduceKeys" is TRUE do not change any modifyOtherKeys ESC sequence
 * into a normal key, return ESC.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
get_literal(ପୂର୍ଣ୍ଣସଂଖ୍ୟା no_Reduce_ଚାବିଗୁଡ଼ିକ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		cc;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		nc;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		hex = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		octal = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		unicode = 0;

    ଯଦି (got_int)
	ଫେରନ୍ତୁ Ctrl_C;

#ifdef FEAT_GUI
    /*
     * In GUI there is no point inserting the internal code for a special key.
     * It is more useful to insert the string "<KEY>" instead.	This would
     * probably be useful in a text window too, but it would not be
     * vi-compatible (maybe there should be an option for it?) -- webb
     */
    ଯଦି (gui.in_use)
    {
	++allow_ଚାବିଗୁଡ଼ିକ;
	ଯଦି (no_Reduce_ଚାବିଗୁଡ଼ିକ)
	    ++no_reduce_ଚାବିଗୁଡ଼ିକ;
    }
#endif
#ifdef USE_ON_FLY_SCROLL
    dont_scroll = TRUE;		// disallow scrolling here
#endif
    ++no_mapping;		// don't map the next key hits
    cc = 0;
    i = 0;
    ପାଇଁ (;;)
    {
	nc = plain_vgetc();
	ଯଦି ((nc == ESC || nc == CSI) && !no_Reduce_ଚାବିଗୁଡ଼ିକ)
	    nc = decode_Modify_Other_ଚାବିଗୁଡ଼ିକ(nc);

	ଯଦି ((mod_mask & ~MOD_MASK_SHIFT) != 0)
	    // A character with non-Shift modifiers should not be a valid
	    // character for i_CTRL-V_digit.
	    ଭାଙ୍ଗ;

	ଯଦି ((State & MODE_CMDLINE) == 0 && MB_BYTE2LEN_CHECK(nc) == 1)
	    add_to_showcmd(nc);
	ଯଦି (nc == 'x' || nc == 'X')
	    hex = TRUE;
	ଅନ୍ୟଥା ଯଦି (nc == 'o' || nc == 'O')
	    octal = TRUE;
	ଅନ୍ୟଥା ଯଦି (nc == 'u' || nc == 'U')
	    unicode = nc;
	ଅନ୍ୟଥା
	{
	    ଯଦି (hex || unicode != 0)
	    {
		ଯଦି (!vim_isxdigit(nc))
		    ଭାଙ୍ଗ;
		cc = cc * 16 + hex2nr(nc);
	    }
	    ଅନ୍ୟଥା ଯଦି (octal)
	    {
		ଯଦି (nc < '0' || nc > '7')
		    ଭାଙ୍ଗ;
		cc = cc * 8 + nc - '0';
	    }
	    ଅନ୍ୟଥା
	    {
		ଯଦି (!VIM_ISDIGIT(nc))
		    ଭାଙ୍ଗ;
		cc = cc * 10 + nc - '0';
	    }

	    ++i;
	}

	ଯଦି (cc > 255 && unicode == 0)
	    cc = 255;		// limit range to 0-255
	nc = 0;

	ଯଦି (hex)		// hex: up to two chars
	{
	    ଯଦି (i >= 2)
		ଭାଙ୍ଗ;
	}
	ଅନ୍ୟଥା ଯଦି (unicode)	// Unicode: up to four or eight chars
	{
	    ଯଦି ((unicode == 'u' && i >= 4) || (unicode == 'U' && i >= 8))
		ଭାଙ୍ଗ;
	}
	ଅନ୍ୟଥା ଯଦି (i >= 3)	// decimal or octal: up to three chars
	    ଭାଙ୍ଗ;
    }
    ଯଦି (i == 0)	    // no number entered
    {
	ଯଦି (nc == K_ZERO)   // NUL is stored as NL
	{
	    cc = '\n';
	    nc = 0;
	}
	ଅନ୍ୟଥା
	{
	    cc = nc;
	    nc = 0;
	}
    }

    ଯଦି (cc == 0)	// NUL is stored as NL
	cc = '\n';
    ଯଦି (enc_dbcs && (cc & 0xff) == 0)
	cc = '?';	// don't accept an illegal DBCS char, the NUL in the
			// second byte will cause trouble!

    --no_mapping;
#ifdef FEAT_GUI
    ଯଦି (gui.in_use)
    {
	--allow_ଚାବିଗୁଡ଼ିକ;
	ଯଦି (no_Reduce_ଚାବିଗୁଡ଼ିକ)
	    --no_reduce_ଚାବିଗୁଡ଼ିକ;
    }
#endif
    ଯଦି (nc)
    {
	vungetc(nc);
	// A character typed with i_CTRL-V_digit cannot have modifiers.
	mod_mask = 0;
    }
    got_int = FALSE;	    // CTRL-C typed after CTRL-V is not an interrupt
    ଫେରନ୍ତୁ cc;
}

/*
 * Insert character, taking care of special keys and mod_mask
 */
    ସ୍ଥିର ଶୂନ୍ୟ
ଭର୍ତ୍ତି_special(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    c,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    allow_modmask,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    ctrlv)	    // c was typed after CTRL-V
{
    char_u  *p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    len;

    /*
     * Special function key, translate into "<Key>". Up to the last '>' is
     * inserted with ins_str(), so as not to replace characters in replace
     * mode.
     * Only use mod_mask for special keys, to avoid things like <S-Space>,
     * unless 'allow_modmask' is TRUE.
     */
#ଯଦି defined(MACOS_X) || defined(FEAT_GUI_GTK)
    // Command-key never produces a normal key
    ଯଦି (mod_mask & MOD_MASK_CMD)
	allow_modmask = TRUE;
#endif
    ଯଦି (IS_SPECIAL(c) || (mod_mask && allow_modmask))
    {
	p = get_special_ଚାବି_name(c, mod_mask);
	len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(p);
	c = p[len - 1];
	ଯଦି (len > 2)
	{
	    ଯଦି (stop_arrow() == FAIL)
		ଫେରନ୍ତୁ;
	    p[len - 1] = NUL;
	    ins_str(p, len - 1);
	    AppendToRedobuffLit(p, -1);
	    ctrlv = FALSE;
	}
    }
    ଯଦି (stop_arrow() == OK)
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
    ଶୂନ୍ୟ
insertchar(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		c,			// character to insert or NUL
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		flags,			// INSCHAR_FORMAT, etc.
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		second_indent)		// indent for second line if >= 0
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		textwidth;
    char_u	*p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		fo_ins_blank;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		force_format = flags & INSCHAR_FORMAT;

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
    ଯଦି (textwidth > 0
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
#ଯଦି defined(FEAT_EVAL)
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା     do_internal = TRUE;
	colnr_T virtcol = get_nolist_virtcol()
				  + char2cells(c != NUL ? c : gchar_cursor());

	ଯଦି (*curbuf->b_p_fex != NUL && (flags & INSCHAR_NO_FEX) == 0
		&& (force_format || virtcol > (colnr_T)textwidth))
	{
	    do_internal = (fex_format(curwin->w_cursor.lnum, 1L, c) != 0);
	    // It may be required to save for undo again, e.g. when setline()
	    // was called.
	    ins_need_undo = TRUE;
	}
	ଯଦି (do_internal)
#endif
	    internal_format(textwidth, second_indent, flags, c == NUL, c);
    }

    ଯଦି (c == NUL)	    // only formatting was wanted
	ଫେରନ୍ତୁ;

    // Check whether this character should end a comment.
    ଯଦି (did_ai && c == end_comment_pending)
    {
	char_u  *ଧାଡ଼ି;
	char_u	lead_end[COM_MAX_LEN];	    // end-comment string
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	ମଧ୍ୟ_len, end_len;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	i;

	/*
	 * Need to remove existing (middle) comment leader and insert end
	 * comment leader.  First, check what comment leader we can find.
	 */
	i = get_leader_len(ଧାଡ଼ି = ml_get_curline(), &p, FALSE, TRUE);
	ଯଦି (i > 0 && vim_strchr(p, COM_ମଧ୍ୟ) != ଖାଲି)	// Just checking
	{
	    // Skip middle-comment string
	    ଯେତେବେଳେ (*p && p[-1] != ':')	// find end of middle flags
		++p;
	    ମଧ୍ୟ_len = copy_option_part(&p, lead_end, COM_MAX_LEN, ",");
	    // Don't count trailing white space for middle_len
	    ଯେତେବେଳେ (ମଧ୍ୟ_len > 0 && VIM_ISWHITE(lead_end[ମଧ୍ୟ_len - 1]))
		--ମଧ୍ୟ_len;

	    // Find the end-comment string
	    ଯେତେବେଳେ (*p && p[-1] != ':')	// find end of end flags
		++p;
	    end_len = copy_option_part(&p, lead_end, COM_MAX_LEN, ",");

	    // Skip white space before the cursor
	    i = curwin->w_cursor.col;
	    ଯେତେବେଳେ (--i >= 0 && VIM_ISWHITE(ଧାଡ଼ି[i]))
		;
	    i++;

	    // Skip to before the middle leader
	    i -= ମଧ୍ୟ_len;

	    // Check some expected things before we go on
	    ଯଦି (i >= 0 && end_len > 0 && lead_end[end_len - 1] == end_comment_pending)
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

    ଯଦି (       !ISSPECIAL(c)
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
#define ନିବେଶ_BUFLEN 100
	char_u		buf[ନିବେଶ_BUFLEN + 1];
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
	colnr_T		virtcol = 0;

	buf[0] = c;
	i = 1;
	ଯଦି (textwidth > 0)
	    virtcol = get_nolist_virtcol();
	/*
	 * Stop the string when:
	 * - no more chars available
	 * - finding a special character (command key)
	 * - buffer is full
	 * - running into the 'textwidth' boundary
	 * - need to check for abbreviation: A non-word char after a word-char
	 */
	ଯେତେବେଳେ (	   (c = vpeekc()) != NUL
		&& !ISSPECIAL(c)
		&& (!has_mbyte || MB_BYTE2LEN_CHECK(c) == 1)
		&& i < ନିବେଶ_BUFLEN
		&& (textwidth == 0
		    || (virtcol += byte2cells(buf[i - 1])) < (colnr_T)textwidth)
		&& !(!no_abbr && !vim_iswordc(c) && vim_iswordc(buf[i - 1])))
	{
#ifdef FEAT_RIGHTLEFT
	    c = vgetc();
	    ଯଦି (p_hkmap && ଚାବି_Typed)
		c = hkmap(c);		    // Hebrew mode mapping
	    buf[i++] = c;
#ଅନ୍ୟଥା
	    buf[i++] = vgetc();
#endif
	}

#ifdef FEAT_DIGRAPHS
	do_digraph(-1);			// clear digraphs
	do_digraph(buf[i-1]);		// may be the start of a digraph
#endif
	buf[i] = NUL;
	ins_str(buf, i);
	ଯଦି (flags & INSCHAR_CTRLV)
	{
	    redo_literal(*buf);
	    i = 1;
	}
	ଅନ୍ୟଥା
	    i = 0;
	ଯଦି (buf[i] != NUL)
	    AppendToRedobuffLit(buf + i, -1);
    }
    ଅନ୍ୟଥା
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		cc;

	ଯଦି (has_mbyte && (cc = (*mb_char2len)(c)) > 1)
	{
	    char_u	buf[MB_MAXBYTES + 1];

	    (*mb_char2bytes)(c, buf);
	    buf[cc] = NUL;
	    ins_char_bytes(buf, cc);
	    AppendCharToRedobuff(c);
	}
	ଅନ୍ୟଥା
	{
	    ins_char(c);
	    ଯଦି (flags & INSCHAR_CTRLV)
		redo_literal(c);
	    ଅନ୍ୟଥା
		AppendCharToRedobuff(c);
	}
    }
}

/*
 * Put a character in the redo buffer, for when just after a CTRL-V.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
redo_literal(ପୂର୍ଣ୍ଣସଂଖ୍ୟା c)
{
    char_u	buf[10];

    // Only digits need special treatment.  Translate them into a string of
    // three digits.
    ଯଦି (VIM_ISDIGIT(c))
    {
	vim_snprintf((ଅକ୍ଷର *)buf, ଆକାର(buf), "%03d", c);
	AppendToRedobuff(buf);
    }
    ଅନ୍ୟଥା
	AppendCharToRedobuff(c);
}

/*
 * start_arrow() is called when an arrow key is used in insert mode.
 * For undo/redo it resembles hitting the <ESC> key.
 */
    ଶୂନ୍ୟ
start_arrow(
    pos_T    *end_ଭର୍ତ୍ତି_pos)		// can be NULL
{
    start_arrow_common(end_ଭର୍ତ୍ତି_pos, TRUE);
}

/*
 * Like start_arrow() but with end_change argument.
 * Will prepare for redo of CTRL-G U if "end_change" is FALSE.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
start_arrow_with_change(
    pos_T    *end_ଭର୍ତ୍ତି_pos,		// can be NULL
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	      end_change)		// end undoable change
{
    start_arrow_common(end_ଭର୍ତ୍ତି_pos, end_change);
    ଯଦି (!end_change)
    {
	AppendCharToRedobuff(Ctrl_G);
	AppendCharToRedobuff('U');
    }
}

    ସ୍ଥିର ଶୂନ୍ୟ
start_arrow_common(
    pos_T    *end_ଭର୍ତ୍ତି_pos,		// can be NULL
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	      end_change)		// end undoable change
{
    ଯଦି (!arrow_used && end_change)	// something has been inserted
    {
	AppendToRedobuff(ESC_STR);
	stop_ଭର୍ତ୍ତି(end_ଭର୍ତ୍ତି_pos, FALSE, FALSE);
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
    ସ୍ଥିର ଶୂନ୍ୟ
check_spell_redraw(ଶୂନ୍ୟ)
{
    ଯଦି (spell_redraw_lnum != 0)
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
stop_arrow(ଶୂନ୍ୟ)
{
    ଯଦି (arrow_used)
    {
	Insstart = curwin->w_cursor;	// new insertion starts here
	ଯଦି (Insstart.col > Insstart_orig.col && !ins_need_undo)
	    // Don't update the original insert position when moved to the
	    // right, except when nothing was inserted yet.
	    update_Insstart_orig = FALSE;
	Insstart_textlen = (colnr_T)linetabsize_str(ml_get_curline());

	ଯଦି (u_save_cursor() == OK)
	{
	    arrow_used = FALSE;
	    ins_need_undo = FALSE;
	}

	ai_col = 0;
	ଯଦି (State & VREPLACE_FLAG)
	{
	    orig_ଧାଡ଼ି_ଗଣନା = curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା;
	    vr_ଧାଡ଼ିଗୁଡ଼ିକ_changed = 1;
	}
	ResetRedobuff();
	AppendToRedobuff((char_u *)"1i");   // pretend we start an insertion
	new_ଭର୍ତ୍ତି_skip = 2;
    }
    ଅନ୍ୟଥା ଯଦି (ins_need_undo)
    {
	ଯଦି (u_save_cursor() == OK)
	{
	    // A command or event may have moved the cursor before the next
	    // edit. Pull Insstart back only when the cursor moved above it,
	    // so that later edits can properly decide whether an extra undo
	    // entry is needed. Advancing Insstart would mis-place '[ after a
	    // register paste.
	    ଯଦି (LT_POS(curwin->w_cursor, Insstart))
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

    ଫେରନ୍ତୁ (arrow_used || ins_need_undo ? FAIL : OK);
}

/*
 * Do a few things to stop inserting.
 * "end_insert_pos" is where insert ended.  It is NULL when we already jumped
 * to another window/buffer.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
stop_ଭର୍ତ୍ତି(
    pos_T	*end_ଭର୍ତ୍ତି_pos,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		esc,			// called by ins_esc()
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		nomove)			// <c-\><c-o>, don't move cursor
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		cc;
    string_T	inserted;

    stop_redo_ins();
    replace_flush();		// abandon replace stack

    /*
     * Save the inserted text for later redo with ^@ and CTRL-A.
     * Don't do it when "restart_edit" was set and nothing was inserted,
     * otherwise CTRL-O w and then <Left> will clear "last_insert".
     */
    inserted = get_inserted();
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା added = inserted.string == ଖାଲି ? 0 : (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)inserted.length - new_ଭର୍ତ୍ତି_skip;
    ଯଦି (did_restart_edit == 0 || added > 0)
    {
	vim_free(last_ଭର୍ତ୍ତି.string);
	last_ଭର୍ତ୍ତି = inserted;			    // structure copy
	last_ଭର୍ତ୍ତି_skip = added < 0 ? 0 : new_ଭର୍ତ୍ତି_skip;
    }
    ଅନ୍ୟଥା
	vim_free(inserted.string);

    ଯଦି (!arrow_used && end_ଭର୍ତ୍ତି_pos != ଖାଲି)
    {
	// Auto-format now.  It may seem strange to do this when stopping an
	// insertion (or moving the cursor), but it's required when appending
	// a line and having it end in a space.  But only do it when something
	// was actually inserted, otherwise undo won't work.
	ଯଦି (!ins_need_undo && has_format_option(FO_AUTO))
	{
	    pos_T   tpos = curwin->w_cursor;

	    // When the cursor is at the end of the line after a space the
	    // formatting will move it to the following word.  Avoid that by
	    // moving the cursor onto the space.
	    cc = 'x';
	    ଯଦି (curwin->w_cursor.col > 0 && gchar_cursor() == NUL)
	    {
		dec_cursor();
		cc = gchar_cursor();
		ଯଦି (!VIM_ISWHITE(cc))
		    curwin->w_cursor = tpos;
	    }

	    auto_format(TRUE, FALSE);

	    ଯଦି (VIM_ISWHITE(cc))
	    {
		ଯଦି (gchar_cursor() != NUL)
		    inc_cursor();
		// If the cursor is still at the same character, also keep
		// the "coladd".
		ଯଦି (gchar_cursor() == NUL
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
	ଯଦି (!nomove && did_ai && (esc || (vim_strchr(p_cpo, CPO_INDENT) == ଖାଲି
			&& curwin->w_cursor.lnum != end_ଭର୍ତ୍ତି_pos->lnum))
		&& end_ଭର୍ତ୍ତି_pos->lnum <= curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା)
	{
	    pos_T	tpos = curwin->w_cursor;
	    colnr_T	prev_col = end_ଭର୍ତ୍ତି_pos->col;
	    colnr_T	strip_col;

	    curwin->w_cursor = *end_ଭର୍ତ୍ତି_pos;
	    check_cursor_col();  // make sure it is not past the line

	    // Where the loop would actually start (back up if on NUL).
	    strip_col = curwin->w_cursor.col;
	    ଯଦି (gchar_cursor() == NUL && strip_col > 0)
		--strip_col;

	    // Don't strip if non-whitespace follows: setline() from a <Cmd>
	    // mapping or CursorHoldI autocmd may have inserted content.
	    ଯଦି (*skipwhite(ml_get_curline() + strip_col) == NUL)
	    {
		curwin->w_cursor.col = strip_col;
		ପାଇଁ (;;)
		{
		    ଯଦି (gchar_cursor() == NUL && curwin->w_cursor.col > 0)
			--curwin->w_cursor.col;
		    cc = gchar_cursor();
		    ଯଦି (!VIM_ISWHITE(cc))
			ଭାଙ୍ଗ;
		    ଯଦି (del_char(TRUE) == FAIL)
			ଭାଙ୍ଗ;  // should not happen
		}
		ଯଦି (curwin->w_cursor.lnum != tpos.lnum)
		    curwin->w_cursor = tpos;
		ଅନ୍ୟଥା ଯଦି (curwin->w_cursor.col < prev_col)
		{
		    // reset tpos, could have been invalidated in the loop above
		    tpos = curwin->w_cursor;
		    tpos.col++;
		    ଯଦି (cc != NUL && gchar_pos(&tpos) == NUL)
			++curwin->w_cursor.col;	// put cursor back on the NUL
		}

		// <C-S-Right> may have started Visual mode, adjust the position for
		// deleted characters.
		ଯଦି (VIsual_active)
		    check_visual_pos();
	    }
	    ଅନ୍ୟଥା
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
    ଯଦି (end_ଭର୍ତ୍ତି_pos != ଖାଲି)
    {
	curbuf->b_op_start = Insstart;
	curbuf->b_op_start_orig = Insstart_orig;
	curbuf->b_op_end = *end_ଭର୍ତ୍ତି_pos;
    }
}

/*
 * Set the last inserted text to a single character.
 * Used for the replace command.
 */
    ଶୂନ୍ୟ
set_last_ଭର୍ତ୍ତି(ପୂର୍ଣ୍ଣସଂଖ୍ୟା c)
{
    char_u	*s;

    vim_free(last_ଭର୍ତ୍ତି.string);
    last_ଭର୍ତ୍ତି.string = alloc(MB_MAXBYTES * 3 + 5);
    ଯଦି (last_ଭର୍ତ୍ତି.string == ଖାଲି)
    {
	last_ଭର୍ତ୍ତି.length = 0;
	ଫେରନ୍ତୁ;
    }

    s = last_ଭର୍ତ୍ତି.string;
    // Use the CTRL-V only when entering a special char
    ଯଦି (c < ' ' || c == DEL)
	*s++ = Ctrl_V;
    s = add_char2buf(c, s);
    *s++ = ESC;
    *s = NUL;
    last_ଭର୍ତ୍ତି.length = (size_t)(s - last_ଭର୍ତ୍ତି.string);
    last_ଭର୍ତ୍ତି_skip = 0;
}

#ଯଦି defined(EXITFREE)
    ଶୂନ୍ୟ
free_last_ଭର୍ତ୍ତି(ଶୂନ୍ୟ)
{
    VIM_CLEAR_STRING(last_ଭର୍ତ୍ତି);
}
#endif

/*
 * Add character "c" to buffer "s".  Escape the special meaning of K_SPECIAL
 * and CSI.  Handle multi-byte characters.
 * Returns a pointer to after the added bytes.
 */
    char_u *
add_char2buf(ପୂର୍ଣ୍ଣସଂଖ୍ୟା c, char_u *s)
{
    char_u	temp[MB_MAXBYTES + 1];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len;

    len = (*mb_char2bytes)(c, temp);
    ପାଇଁ (i = 0; i < len; ++i)
    {
	c = temp[i];
	// Need to escape K_SPECIAL and CSI like in the typeahead buffer.
	ଯଦି (c == K_SPECIAL)
	{
	    *s++ = K_SPECIAL;
	    *s++ = KS_SPECIAL;
	    *s++ = KE_FILLER;
	}
#ifdef FEAT_GUI
	ଅନ୍ୟଥା ଯଦି (c == CSI)
	{
	    *s++ = CSI;
	    *s++ = KS_EXTRA;
	    *s++ = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)KE_CSI;
	}
#endif
	ଅନ୍ୟଥା
	    *s++ = c;
    }
    ଫେରନ୍ତୁ s;
}

/*
 * move cursor to start of line
 * if flags & BL_WHITE	move to first non-white
 * if flags & BL_SOL	move to first non-white if startofline is set,
 *			    otherwise keep "curswant" column
 * if flags & BL_FIX	don't leave the cursor on a NUL.
 */
    ଶୂନ୍ୟ
beginline(ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags)
{
    ଯଦି ((flags & BL_SOL) && !p_sol)
	coladvance(curwin->w_curswant);
    ଅନ୍ୟଥା
    {
	curwin->w_cursor.col = 0;
	curwin->w_cursor.coladd = 0;

	ଯଦି (flags & (BL_WHITE | BL_SOL))
	{
	    char_u  *ptr;

	    ପାଇଁ (ptr = ml_get_curline(); VIM_ISWHITE(*ptr)
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

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
oneright(ଶୂନ୍ୟ)
{
    char_u	*ptr;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		l;

    ଯଦି (virtual_active())
    {
	pos_T	prevpos = curwin->w_cursor;

	// Adjust for multi-wide char (excluding TAB)
	ptr = ml_get_cursor();
	coladvance(getviscol() + ((*ptr != TAB
					  && vim_isprintc((*mb_ptr2char)(ptr)))
		    ? ptr2cells(ptr) : 1));
	curwin->w_set_curswant = true;
	// Return OK if the cursor moved, FAIL otherwise (at window edge).
	ଫେରନ୍ତୁ (prevpos.col != curwin->w_cursor.col
		    || prevpos.coladd != curwin->w_cursor.coladd) ? OK : FAIL;
    }

    ptr = ml_get_cursor();
    ଯଦି (*ptr == NUL)
	ଫେରନ୍ତୁ FAIL;	    // already at the very end

    ଯଦି (has_mbyte)
	l = (*mb_ptr2len)(ptr);
    ଅନ୍ୟଥା
	l = 1;

    // move "l" bytes right, but don't end up on the NUL, unless 'virtualedit'
    // contains "onemore".
    ଯଦି (ptr[l] == NUL && (get_ve_flags() & VE_ONEMORE) == 0)
	ଫେରନ୍ତୁ FAIL;
    curwin->w_cursor.col += l;

    curwin->w_set_curswant = true;
    adjust_skipcol();
    ଫେରନ୍ତୁ OK;
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
oneleft(ଶୂନ୍ୟ)
{
    ଯଦି (virtual_active())
    {
#ifdef FEAT_LINEBREAK
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା width;
#endif
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା v = getviscol();

	ଯଦି (v == 0)
	    ଫେରନ୍ତୁ FAIL;

#ifdef FEAT_LINEBREAK
	// We might get stuck on 'showbreak', skip over it.
	width = 1;
	ପାଇଁ (;;)
	{
	    coladvance(v - width);
	    // getviscol() is slow, skip it when 'showbreak' is empty,
	    // 'breakindent' is not set and there are no multi-byte
	    // characters
	    ଯଦି ((*get_showbreak_ମୂଲ୍ୟ(curwin) == NUL && !curwin->w_p_bri
					     && !has_mbyte) || getviscol() < v)
		ଭାଙ୍ଗ;
	    ++width;
	}
#ଅନ୍ୟଥା
	coladvance(v - 1);
#endif

	ଯଦି (curwin->w_cursor.coladd == 1)
	{
	    char_u *ptr;

	    // Adjust for multi-wide char (not a TAB)
	    ptr = ml_get_cursor();
	    ଯଦି (*ptr != TAB && vim_isprintc((*mb_ptr2char)(ptr))
							 && ptr2cells(ptr) > 1)
		curwin->w_cursor.coladd = 0;
	}

	curwin->w_set_curswant = true;
	adjust_skipcol();
	ଫେରନ୍ତୁ OK;
    }

    ଯଦି (curwin->w_cursor.col == 0)
	ଫେରନ୍ତୁ FAIL;

    curwin->w_set_curswant = true;
    --curwin->w_cursor.col;

    // if the character on the left of the current cursor is a multi-byte
    // character, move to its first byte
    ଯଦି (has_mbyte)
	mb_adjust_cursor();
    adjust_skipcol();
    ଫେରନ୍ତୁ OK;
}

/*
 * Move the cursor up "n" lines in window "wp".
 * Takes care of closed folds.
 */
    ଶୂନ୍ୟ
cursor_up_inner(win_T *wp, long n)
{
    linenr_T	lnum = wp->w_cursor.lnum;

    ଯଦି (n >= lnum)
	lnum = 1;
    ଅନ୍ୟଥା
#ifdef FEAT_FOLDING
	ଯଦି (hasAnyFolding(wp))
    {
	/*
	 * Count each sequence of folded lines as one logical line.
	 */
	// go to the start of the current fold
	(ଶୂନ୍ୟ)hasFoldingWin(wp, lnum, &lnum, ଖାଲି, TRUE, ଖାଲି);

	ଯେତେବେଳେ (n--)
	{
	    // move up one line
	    --lnum;
	    ଯଦି (lnum <= 1)
		ଭାଙ୍ଗ;
	    // If we entered a fold, move to the beginning, unless in
	    // Insert mode or when 'foldopen' contains "all": it will open
	    // in a moment.
	    ଯଦି (n > 0 || !((State & MODE_ଭର୍ତ୍ତି) || (fdo_flags & FDO_ALL)))
		(ଶୂନ୍ୟ)hasFoldingWin(wp, lnum, &lnum, ଖାଲି, TRUE, ଖାଲି);
	}
	ଯଦି (lnum < 1)
	    lnum = 1;
    }
    ଅନ୍ୟଥା
#endif
	lnum -= n;

    wp->w_cursor.lnum = lnum;
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
cursor_up(
    long	n,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		upd_topline)	    // When TRUE: update topline
{
    // This fails if the cursor is already in the first line or the count is
    // larger than the line number and '-' is in 'cpoptions'
    linenr_T lnum = curwin->w_cursor.lnum;
    ଯଦି (n > 0 && (lnum <= 1
		       || (n >= lnum && vim_strchr(p_cpo, CPO_MINUS) != ଖାଲି)))
	ଫେରନ୍ତୁ FAIL;
    cursor_up_inner(curwin, n);

    // try to advance to the column we want to be at
    coladvance(curwin->w_curswant);

    ଯଦି (upd_topline)
	update_topline();	// make sure curwin->w_topline is valid

    ଫେରନ୍ତୁ OK;
}

/*
 * Move the cursor down "n" lines in window "wp".
 * Takes care of closed folds.
 */
    ଶୂନ୍ୟ
cursor_down_inner(win_T *wp, long n)
{
    linenr_T	lnum = wp->w_cursor.lnum;
    linenr_T	ଧାଡ଼ି_ଗଣନା = wp->w_buffer->b_ml.ml_ଧାଡ଼ି_ଗଣନା;

    ଯଦି (lnum + n >= ଧାଡ଼ି_ଗଣନା)
	lnum = ଧାଡ଼ି_ଗଣନା;
    ଅନ୍ୟଥା
#ifdef FEAT_FOLDING
	ଯଦି (hasAnyFolding(wp))
    {
	linenr_T	last;

	// count each sequence of folded lines as one logical line
	ଯେତେବେଳେ (n--)
	{
	    ଯଦି (hasFoldingWin(wp, lnum, ଖାଲି, &last, TRUE, ଖାଲି))
		lnum = last + 1;
	    ଅନ୍ୟଥା
		++lnum;
	    ଯଦି (lnum >= ଧାଡ଼ି_ଗଣନା)
		ଭାଙ୍ଗ;
	}
	ଯଦି (lnum > ଧାଡ଼ି_ଗଣନା)
	    lnum = ଧାଡ଼ି_ଗଣନା;
    }
    ଅନ୍ୟଥା
#endif
	lnum += n;

    wp->w_cursor.lnum = lnum;
}

/*
 * Cursor down a number of logical lines.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
cursor_down(
    long	n,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		upd_topline)	    // When TRUE: update topline
{
    linenr_T	lnum = curwin->w_cursor.lnum;
    linenr_T	ଧାଡ଼ି_ଗଣନା = curwin->w_buffer->b_ml.ml_ଧାଡ଼ି_ଗଣନା;
    // This fails if the cursor is already in the last (folded) line, or would
    // move beyond the last line and '-' is in 'cpoptions'.
#ifdef FEAT_FOLDING
    hasFoldingWin(curwin, lnum, ଖାଲି, &lnum, TRUE, ଖାଲି);
#endif
    ଯଦି (n > 0
	    && (lnum >= ଧାଡ଼ି_ଗଣନା
		|| (lnum + n > ଧାଡ଼ି_ଗଣନା
				     && vim_strchr(p_cpo, CPO_MINUS) != ଖାଲି)))
	ଫେରନ୍ତୁ FAIL;
    cursor_down_inner(curwin, n);

    // try to advance to the column we want to be at
    coladvance(curwin->w_curswant);

    ଯଦି (upd_topline)
	update_topline();	// make sure curwin->w_topline is valid

    ଫେରନ୍ତୁ OK;
}

/*
 * Stuff the last inserted text in the read buffer.
 * Last_insert actually is a copy of the redo buffer, so we
 * first have to remove the command.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
stuff_inserted(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    c,		// Command character to be inserted
    long    ଗଣନା_2,	// Repeat this many times
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    no_esc)	// Don't add an ESC at the end
{
    string_T	ଭର୍ତ୍ତି;				// text to be inserted
    char_u	last = ' ';

    ଭର୍ତ୍ତି = get_last_ଭର୍ତ୍ତି();
    ଯଦି (ଭର୍ତ୍ତି.string == ଖାଲି)
    {
	emsg(_(e_no_inserted_text_yet));
	ଫେରନ୍ତୁ FAIL;
    }

    // may want to stuff the command character, to start Insert mode
    ଯଦି (c != NUL)
	stuffcharReadbuff(c);

    ଯଦି (ଭର୍ତ୍ତି.length > 0)
    {
	char_u	*p;

	// look for the last ESC in 'insert'
	ପାଇଁ (p = ଭର୍ତ୍ତି.string + ଭର୍ତ୍ତି.length - 1; p >= ଭର୍ତ୍ତି.string; --p)
	{
	    ଯଦି (*p == ESC)
	    {
		ଭର୍ତ୍ତି.length = (size_t)(p - ଭର୍ତ୍ତି.string);
		ଭାଙ୍ଗ;
	    }
	}
    }

    ଯଦି (ଭର୍ତ୍ତି.length > 0)
    {
	char_u	*p = ଭର୍ତ୍ତି.string + ଭର୍ତ୍ତି.length - 1;

	// when the last char is either "0" or "^" it will be quoted if no ESC
	// comes after it OR if it will insert more than once and "ptr"
	// starts with ^D.	-- Acevedo
	ଯଦି ((*p == '0' || *p == '^')
		&& (no_esc || (*ଭର୍ତ୍ତି.string == Ctrl_D && ଗଣନା_2 > 1)))
	{
	    last = *p;
	    --ଭର୍ତ୍ତି.length;
	}
    }

    କର
    {
	stuffReadbuffLen(ଭର୍ତ୍ତି.string, (long)ଭର୍ତ୍ତି.length);
	// a trailing "0" is inserted as "<C-V>048", "^" as "<C-V>^"
	ବାଛ (last)
	{
	ସ୍ଥିତି '0':
#define TEXT_TO_ଭର୍ତ୍ତି "\026\060\064\070"
	    stuffReadbuffLen((char_u *)TEXT_TO_ଭର୍ତ୍ତି, STRLEN_LITERAL(TEXT_TO_ଭର୍ତ୍ତି));
#undef TEXT_TO_ଭର୍ତ୍ତି
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି '^':
#define TEXT_TO_ଭର୍ତ୍ତି "\026^"
	    stuffReadbuffLen((char_u *)TEXT_TO_ଭର୍ତ୍ତି, STRLEN_LITERAL(TEXT_TO_ଭର୍ତ୍ତି));
#undef TEXT_TO_ଭର୍ତ୍ତି
	    ଭାଙ୍ଗ;

	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    ଭାଙ୍ଗ;
	}
    }
    ଯେତେବେଳେ (--ଗଣନା_2 > 0);

    // may want to stuff a trailing ESC, to get out of Insert mode
    ଯଦି (!no_esc)
	stuffcharReadbuff(ESC);

    ଫେରନ୍ତୁ OK;
}

    string_T
get_last_ଭର୍ତ୍ତି(ଶୂନ୍ୟ)
{
    string_T ଭର୍ତ୍ତି = {ଖାଲି, 0};

    ଯଦି (last_ଭର୍ତ୍ତି.string != ଖାଲି)
    {
	ଭର୍ତ୍ତି.string = last_ଭର୍ତ୍ତି.string + last_ଭର୍ତ୍ତି_skip;
	ଭର୍ତ୍ତି.length = (size_t)(last_ଭର୍ତ୍ତି.length - last_ଭର୍ତ୍ତି_skip);
    }

    ଫେରନ୍ତୁ ଭର୍ତ୍ତି;
}

/*
 * Get last inserted string, and remove trailing <Esc>.
 * Returns pointer to allocated memory (must be freed) or NULL.
 */
    char_u *
get_last_ଭର୍ତ୍ତି_save(ଶୂନ୍ୟ)
{
    string_T	ଭର୍ତ୍ତି = get_last_ଭର୍ତ୍ତି();
    char_u	*s;

    ଯଦି (ଭର୍ତ୍ତି.string == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;
    s = vim_strnsave(ଭର୍ତ୍ତି.string, ଭର୍ତ୍ତି.length);
    ଯଦି (s == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    ଯଦି (ଭର୍ତ୍ତି.length > 0 && s[ଭର୍ତ୍ତି.length - 1] == ESC)	// remove trailing ESC
	s[--ଭର୍ତ୍ତି.length] = NUL;
    ଫେରନ୍ତୁ s;
}

/*
 * Check the word in front of the cursor for an abbreviation.
 * Called when the non-id character "c" has been entered.
 * When an abbreviation is recognized it is removed from the text and
 * the replacement string is inserted in typebuf.tb_buf[], followed by "c".
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
echeck_abbr(ପୂର୍ଣ୍ଣସଂଖ୍ୟା c)
{
    // Don't check for abbreviation in paste mode, when disabled and just
    // after moving around with cursor keys.
    ଯଦି (p_paste || no_abbr || arrow_used)
	ଫେରନ୍ତୁ FALSE;

    ଫେରନ୍ତୁ check_abbr(c, ml_get_curline(), curwin->w_cursor.col,
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

ସ୍ଥିର char_u	*replace_ଗଦା = ଖାଲି;
ସ୍ଥିର long	replace_ଗଦା_nr = 0;	    // next entry in replace stack
ସ୍ଥିର long	replace_ଗଦା_len = 0;	    // max. number of entries

    ଶୂନ୍ୟ
replace_ଠେଲ(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    c)	    // character that is replaced (NUL is none)
{
    char_u  *p;

    ଯଦି (replace_ଗଦା_nr < replace_offset)	// nothing to do
	ଫେରନ୍ତୁ;
    ଯଦି (replace_ଗଦା_len <= replace_ଗଦା_nr)
    {
	replace_ଗଦା_len += 50;
	p = ALLOC_MULT(char_u, replace_ଗଦା_len);
	ଯଦି (p == ଖାଲି)	    // out of memory
	{
	    replace_ଗଦା_len -= 50;
	    ଫେରନ୍ତୁ;
	}
	ଯଦି (replace_ଗଦା != ଖାଲି)
	{
	    mch_memmove(p, replace_ଗଦା,
				 (size_t)(replace_ଗଦା_nr * ଆକାର(char_u)));
	    vim_free(replace_ଗଦା);
	}
	replace_ଗଦା = p;
    }
    p = replace_ଗଦା + replace_ଗଦା_nr - replace_offset;
    ଯଦି (replace_offset)
	mch_memmove(p + 1, p, (size_t)(replace_offset * ଆକାର(char_u)));
    *p = c;
    ++replace_ଗଦା_nr;
}

/*
 * Push a character onto the replace stack.  Handles a multi-byte character in
 * reverse byte order, so that the first byte is popped off first.
 * Return the number of bytes done (includes composing characters).
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
replace_ଠେଲ_mb(char_u *p)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା l = (*mb_ptr2len)(p);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା j;

    ପାଇଁ (j = l - 1; j >= 0; --j)
	replace_ଠେଲ(p[j]);
    ଫେରନ୍ତୁ l;
}

/*
 * Pop one item from the replace stack.
 * return -1 if stack empty
 * return replaced character or NUL otherwise
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
replace_କାଢ଼(ଶୂନ୍ୟ)
{
    ଯଦି (replace_ଗଦା_nr == 0)
	ଫେରନ୍ତୁ -1;
    ଫେରନ୍ତୁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)replace_ଗଦା[--replace_ଗଦା_nr];
}

/*
 * Join the top two items on the replace stack.  This removes to "off"'th NUL
 * encountered.
 */
    ଶୂନ୍ୟ
replace_join(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    off)	// offset for which NUL to remove
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    i;

    ପାଇଁ (i = replace_ଗଦା_nr; --i >= 0; )
	ଯଦି (replace_ଗଦା[i] == NUL && off-- <= 0)
	{
	    --replace_ଗଦା_nr;
	    mch_memmove(replace_ଗଦା + i, replace_ଗଦା + i + 1,
					      (size_t)(replace_ଗଦା_nr - i));
	    ଫେରନ୍ତୁ;
	}
}

/*
 * Pop bytes from the replace stack until a NUL is found, and insert them
 * before the cursor.  Can only be used in MODE_REPLACE or MODE_VREPLACE state.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
replace_କାଢ଼_ins(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    cc;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    oldState = State;

    State = MODE_NORMAL;			// don't want MODE_REPLACE here
    ଯେତେବେଳେ ((cc = replace_କାଢ଼()) > 0)
    {
	mb_replace_କାଢ଼_ins(cc);
	dec_cursor();
    }
    State = oldState;
}

/*
 * Insert bytes popped from the replace stack. "cc" is the first byte.  If it
 * indicates a multi-byte char, pop the other bytes too.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
mb_replace_କାଢ଼_ins(ପୂର୍ଣ୍ଣସଂଖ୍ୟା cc)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		n;
    char_u	buf[MB_MAXBYTES + 1];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		c;

    ଯଦି (has_mbyte && (n = MB_BYTE2LEN(cc)) > 1)
    {
	buf[0] = cc;
	ପାଇଁ (i = 1; i < n; ++i)
	    buf[i] = replace_କାଢ଼();
	ins_bytes_len(buf, n);
    }
    ଅନ୍ୟଥା
	ins_char(cc);

    ଯଦି (enc_utf8)
	// Handle composing chars.
	ପାଇଁ (;;)
	{
	    c = replace_କାଢ଼();
	    ଯଦି (c == -1)	    // stack empty
		ଭାଙ୍ଗ;
	    ଯଦି ((n = MB_BYTE2LEN(c)) == 1)
	    {
		// Not a multi-byte char, put it back.
		replace_ଠେଲ(c);
		ଭାଙ୍ଗ;
	    }

	    buf[0] = c;
	    ପାଇଁ (i = 1; i < n; ++i)
		buf[i] = replace_କାଢ଼();
	    ଯଦି (utf_iscomposing(utf_ptr2char(buf)))
		ins_bytes_len(buf, n);
	    ଅନ୍ୟଥା
	    {
		// Not a composing char, put it back.
		ପାଇଁ (i = n - 1; i >= 0; --i)
		    replace_ଠେଲ(buf[i]);
		ଭାଙ୍ଗ;
	    }

	}
}

/*
 * make the replace stack empty
 * (called when exiting replace mode)
 */
    ସ୍ଥିର ଶୂନ୍ୟ
replace_flush(ଶୂନ୍ୟ)
{
    VIM_CLEAR(replace_ଗଦା);
    replace_ଗଦା_len = 0;
    replace_ଗଦା_nr = 0;
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
    ସ୍ଥିର ଶୂନ୍ୟ
replace_do_bs(ପୂର୍ଣ୍ଣସଂଖ୍ୟା limit_col)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		cc;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		orig_len = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ins_len;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		orig_vcols = 0;
    colnr_T	start_vcol;
    char_u	*p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		vcol;

    cc = replace_କାଢ଼();
    ଯଦି (cc > 0)
    {
#ifdef FEAT_PROP_POPUP
	size_t	len_before = 0;  // init to shut up GCC

	ଯଦି (curbuf->b_has_textprop)
	{
	    // Do not adjust text properties for individual delete and insert
	    // operations, do it afterwards on the resulting text.
	    len_before = ml_get_curline_len();
	    ++text_prop_frozen;
	}
#endif
	ଯଦି (State & VREPLACE_FLAG)
	{
	    // Get the number of screen cells used by the character we are
	    // going to delete.
	    getvcol(curwin, &curwin->w_cursor, ଖାଲି, &start_vcol, ଖାଲି, 0);
	    orig_vcols = chartabsize(ml_get_cursor(), start_vcol);
	}
	ଯଦି (has_mbyte)
	{
	    (ଶୂନ୍ୟ)del_char_after_col(limit_col);
	    ଯଦି (State & VREPLACE_FLAG)
		orig_len = ml_get_cursor_len();
	    replace_ଠେଲ(cc);
	}
	ଅନ୍ୟଥା
	{
	    pchar_cursor(cc);
	    ଯଦି (State & VREPLACE_FLAG)
		orig_len = ml_get_cursor_len() - 1;
	}
	replace_କାଢ଼_ins();

	ଯଦି (State & VREPLACE_FLAG)
	{
	    // Get the number of screen cells used by the inserted characters
	    p = ml_get_cursor();
	    ins_len = ml_get_cursor_len() - orig_len;
	    vcol = start_vcol;
	    ପାଇଁ (i = 0; i < ins_len; ++i)
	    {
		vcol += chartabsize(p + i, vcol);
		i += (*mb_ptr2len)(p + i) - 1;
	    }
	    vcol -= start_vcol;

	    // Delete spaces that were inserted after the cursor to keep the
	    // text aligned.
	    curwin->w_cursor.col += ins_len;
	    ଯେତେବେଳେ (vcol > orig_vcols && gchar_cursor() == ' ')
	    {
		del_char(FALSE);
		++orig_vcols;
	    }
	    curwin->w_cursor.col -= ins_len;
	}

	// mark the buffer as changed and prepare for displaying
	changed_bytes(curwin->w_cursor.lnum, curwin->w_cursor.col);

#ifdef FEAT_PROP_POPUP
	ଯଦି (curbuf->b_has_textprop)
	{
	    size_t len_now = ml_get_curline_len();

	    --text_prop_frozen;
	    adjust_prop_columns(curwin->w_cursor.lnum, curwin->w_cursor.col,
					   (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(len_now - len_before), 0);
	}
#endif
    }
    ଅନ୍ୟଥା ଯଦି (cc == 0)
	(ଶୂନ୍ୟ)del_char_after_col(limit_col);
}

#ଯଦି defined(FEAT_RIGHTLEFT)
/*
 * Map Hebrew keyboard when in hkmap mode.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
hkmap(ପୂର୍ଣ୍ଣସଂଖ୍ୟା c)
{
    ଯଦି (p_hkmapp)   // phonetic mapping, by Ilya Dogolazky
    {
	ଗଣନା {hALEF=0, BET, GIMEL, DALET, HEI, VAV, ZAIN, HET, TET, IUD,
	    KAFsofit, hKAF, LAMED, MEMsofit, MEM, NUNsofit, NUN, SAMEH, AIN,
	    PEIsofit, PEI, ZADIsofit, ZADI, KOF, RESH, hSHIN, TAV};
	ସ୍ଥିର char_u map[26] =
	    {(char_u)hALEF/*a*/, (char_u)BET  /*b*/, (char_u)hKAF    /*c*/,
	     (char_u)DALET/*d*/, (char_u)-1   /*e*/, (char_u)PEIsofit/*f*/,
	     (char_u)GIMEL/*g*/, (char_u)HEI  /*h*/, (char_u)IUD     /*i*/,
	     (char_u)HET  /*j*/, (char_u)KOF  /*k*/, (char_u)LAMED   /*l*/,
	     (char_u)MEM  /*m*/, (char_u)NUN  /*n*/, (char_u)SAMEH   /*o*/,
	     (char_u)PEI  /*p*/, (char_u)-1   /*q*/, (char_u)RESH    /*r*/,
	     (char_u)ZAIN /*s*/, (char_u)TAV  /*t*/, (char_u)TET     /*u*/,
	     (char_u)VAV  /*v*/, (char_u)hSHIN/*w*/, (char_u)-1      /*x*/,
	     (char_u)AIN  /*y*/, (char_u)ZADI /*z*/};

	ଯଦି (c == 'N' || c == 'M' || c == 'P' || c == 'C' || c == 'Z')
	    ଫେରନ୍ତୁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(map[CharOrd(c)] - 1 + p_aleph);
							    // '-1'='sofit'
	ଅନ୍ୟଥା ଯଦି (c == 'x')
	    ଫେରନ୍ତୁ 'X';
	ଅନ୍ୟଥା ଯଦି (c == 'q')
	    ଫେରନ୍ତୁ '\''; // {geresh}={'}
	ଅନ୍ୟଥା ଯଦି (c == 246)
	    ଫେରନ୍ତୁ ' ';  // \"o --> ' ' for a german keyboard
	ଅନ୍ୟଥା ଯଦି (c == 228)
	    ଫେରନ୍ତୁ ' ';  // \"a --> ' '      -- / --
	ଅନ୍ୟଥା ଯଦି (c == 252)
	    ଫେରନ୍ତୁ ' ';  // \"u --> ' '      -- / --
	// NOTE: islower() does not do the right thing for us on Linux so we
	// do this the same was as 5.7 and previous, so it works correctly on
	// all systems.  Specifically, the e.g. Delete and Arrow keys are
	// munged and won't work if e.g. searching for Hebrew text.
	ଅନ୍ୟଥା ଯଦି (c >= 'a' && c <= 'z')
	    ଫେରନ୍ତୁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(map[Char_Ord_ତଳ(c)] + p_aleph);
	ଅନ୍ୟଥା
	    ଫେରନ୍ତୁ c;
    }
    ଅନ୍ୟଥା
    {
	ବାଛ (c)
	{
	    ସ୍ଥିତି '`':	ଫେରନ୍ତୁ ';';
	    ସ୍ଥିତି '/':	ଫେରନ୍ତୁ '.';
	    ସ୍ଥିତି '\'':	ଫେରନ୍ତୁ ',';
	    ସ୍ଥିତି 'q':	ଫେରନ୍ତୁ '/';
	    ସ୍ଥିତି 'w':	ଫେରନ୍ତୁ '\'';

			// Hebrew letters - set offset from 'a'
	    ସ୍ଥିତି ',':	c = '{'; ଭାଙ୍ଗ;
	    ସ୍ଥିତି '.':	c = 'v'; ଭାଙ୍ଗ;
	    ସ୍ଥିତି ';':	c = 't'; ଭାଙ୍ଗ;
	    ପୂର୍ବନିର୍ଦ୍ଧାରିତ: {
			 ସ୍ଥିର ଅକ୍ଷର str[] = "zqbcxlsjphmkwonu ydafe rig";

			 ଯଦି (c < 'a' || c > 'z')
			     ଫେରନ୍ତୁ c;
			 c = str[Char_Ord_ତଳ(c)];
			 ଭାଙ୍ଗ;
		     }
	}

	ଫେରନ୍ତୁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(Char_Ord_ତଳ(c) + p_aleph);
    }
}
#endif

    ସ୍ଥିର ଶୂନ୍ୟ
ins_reg(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		need_redraw = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		regname;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		literally = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		vis_active = VIsual_active;

    /*
     * If we are going to wait for a character, show a '"'.
     */
    pc_status = PC_STATUS_UNSET;
    ଯଦି (redrawing() && !char_avail())
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
    ++allow_ଚାବିଗୁଡ଼ିକ;
    regname = plain_vgetc();
    LANGMAP_ADJUST(regname, TRUE);
    ଯଦି (regname == Ctrl_R || regname == Ctrl_O || regname == Ctrl_P)
    {
	// Get a third key for literal register insertion
	literally = regname;
	add_to_showcmd_c(literally);
	regname = plain_vgetc();
	LANGMAP_ADJUST(regname, TRUE);
    }
    --no_mapping;
    --allow_ଚାବିଗୁଡ଼ିକ;

#ifdef FEAT_EVAL
    // Don't call u_sync() while typing the expression or giving an error
    // message for it. Only call it explicitly.
    ++no_u_sync;
    ଯଦି (regname == '=')
    {
	pos_T	curpos = curwin->w_cursor;
# ifdef HAVE_ନିବେଶ_METHOD
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	im_on = im_get_status();
# endif
	// Sync undo when evaluating the expression calls setline() or
	// append(), so that it can be undone separately.
	u_sync_once = 2;

	regname = get_ପଦସମଷ୍ଟି_register();

	// Cursor may be moved back a column.
	curwin->w_cursor = curpos;
	check_cursor();
# ifdef HAVE_ନିବେଶ_METHOD
	// Restore the Input Method.
	ଯଦି (im_on)
	    im_set_active(TRUE);
# endif
    }
    ଯଦି (regname == NUL || !valid_yank_reg(regname, FALSE))
    {
	vim_beep(BO_REG);
	need_redraw = TRUE;	// remove the '"'
    }
    ଅନ୍ୟଥା
    {
#endif
	ଯଦି (literally == Ctrl_O || literally == Ctrl_P)
	{
	    // Append the command to the redo buffer.
	    AppendCharToRedobuff(Ctrl_R);
	    AppendCharToRedobuff(literally);
	    AppendCharToRedobuff(regname);

	    do_put(regname, ଖାଲି, BACKWARD, 1L,
		 (literally == Ctrl_P ? PUT_FIXINDENT : 0) | PUT_CURSEND);
	}
	ଅନ୍ୟଥା ଯଦି (ଭର୍ତ୍ତି_reg(regname, literally) == FAIL)
	{
	    vim_beep(BO_REG);
	    need_redraw = TRUE;	// remove the '"'
	}
	ଅନ୍ୟଥା ଯଦି (stop_ଭର୍ତ୍ତି_mode)
	    // When the '=' register was used and a function was invoked that
	    // did ":stopinsert" then stuff_empty() returns FALSE but we won't
	    // insert anything, need to remove the '"'
	    need_redraw = TRUE;

#ifdef FEAT_EVAL
    }
    --no_u_sync;
    ଯଦି (u_sync_once == 1)
	ins_need_undo = TRUE;
    u_sync_once = 0;
#endif
    clear_showcmd();

    // If the inserted register is empty, we need to remove the '"'
    ଯଦି (need_redraw || stuff_empty())
	edit_unputchar();

    // Disallow starting Visual mode here, would get a weird mode.
    ଯଦି (!vis_active && VIsual_active)
	end_visual_mode();
}

/*
 * CTRL-G commands in Insert mode.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
ins_ctrl_g(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		c;

    // Right after CTRL-X the cursor will be after the ruler.
    setcursor();

    /*
     * Don't map the second key. This also prevents the mode message to be
     * deleted when ESC is hit.
     */
    ++no_mapping;
    ++allow_ଚାବିଗୁଡ଼ିକ;
    c = plain_vgetc();
    --no_mapping;
    --allow_ଚାବିଗୁଡ଼ିକ;
    ବାଛ (c)
    {
	// CTRL-G k and CTRL-G <Up>: cursor up to Insstart.col
	ସ୍ଥିତି K_UP:
	ସ୍ଥିତି Ctrl_K:
	ସ୍ଥିତି 'k': ins_up(TRUE);
		  ଭାଙ୍ଗ;

	// CTRL-G j and CTRL-G <Down>: cursor down to Insstart.col
	ସ୍ଥିତି K_DOWN:
	ସ୍ଥିତି Ctrl_J:
	ସ୍ଥିତି 'j': ins_down(TRUE);
		  ଭାଙ୍ଗ;

	// CTRL-G u: start new undoable edit
	ସ୍ଥିତି 'u': u_sync(TRUE);
		  ins_need_undo = TRUE;

		  // Need to reset Insstart, esp. because a BS that joins
		  // a line to the previous one must save for undo.
		  update_Insstart_orig = FALSE;
		  Insstart = curwin->w_cursor;
		  ଭାଙ୍ଗ;

	// CTRL-G U: do not break undo with the next char
	ସ୍ଥିତି 'U':
		  // Allow one left/right cursor movement with the next char,
		  // without breaking undo.
		  dont_sync_undo = MAYBE;
		  ଭାଙ୍ଗ;

	ସ୍ଥିତି ESC:
		  // Esc after CTRL-G cancels it.
		  ଭାଙ୍ଗ;

	// Unknown CTRL-G command, reserved for future expansion.
	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:  vim_beep(BO_CTRLG);
    }
}

/*
 * CTRL-^ in Insert mode.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
ins_ctrl_hat(ଶୂନ୍ୟ)
{
    ଯଦି (map_to_exists_mode((char_u *)"", MODE_LANGMAP, FALSE))
    {
	// ":lmap" mappings exists, Toggle use of ":lmap" mappings.
	ଯଦି (State & MODE_LANGMAP)
	{
	    curbuf->b_p_iminsert = B_IMODE_NONE;
	    State &= ~MODE_LANGMAP;
	}
	ଅନ୍ୟଥା
	{
	    curbuf->b_p_iminsert = B_IMODE_LMAP;
	    State |= MODE_LANGMAP;
#ifdef HAVE_ନିବେଶ_METHOD
	    im_set_active(FALSE);
#endif
	}
    }
#ifdef HAVE_ନିବେଶ_METHOD
    ଅନ୍ୟଥା
    {
	// There are no ":lmap" mappings, toggle IM
	ଯଦି (im_get_status())
	{
	    curbuf->b_p_iminsert = B_IMODE_NONE;
	    im_set_active(FALSE);
	}
	ଅନ୍ୟଥା
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
    ଯଦି (gui.in_use)
	gui_update_cursor(TRUE, FALSE);
#endif
#ଯଦି defined(FEAT_KEYMAP)
    // Show/unshow value of 'keymap' in status lines.
    status_redraw_curbuf();
#endif
}

/*
 * Handle ESC in insert mode.
 * Returns TRUE when leaving insert mode, FALSE when going to repeat the
 * insert.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ins_esc(
    long	*ଗଣନା_2,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		cmdchar,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		nomove)	    // don't move cursor
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		temp;
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	disabled_redraw = FALSE;
#ifdef FEAT_CONCEAL
    // Remember if the cursor line was concealed before changing State.
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		cursor_ଧାଡ଼ି_was_concealed = curwin->w_p_cole > 0
						&& conceal_cursor_ଧାଡ଼ି(curwin);
#endif

#ifdef FEAT_SPELL
    check_spell_redraw();
#endif

    temp = curwin->w_cursor.col;
    ଯଦି (disabled_redraw)
    {
	ଯଦି (RedrawingDisabled > 0)
	    --RedrawingDisabled;
	disabled_redraw = FALSE;
    }
    ଯଦି (!arrow_used)
    {
	/*
	 * Don't append the ESC for "r<CR>" and "grx".
	 * When 'insertmode' is set only CTRL-L stops Insert mode.  Needed for
	 * when "count" is non-zero.
	 */
	ଯଦି (cmdchar != 'r' && cmdchar != 'v')
	    AppendToRedobuff(p_im ? (char_u *)"\014" : ESC_STR);

	/*
	 * Repeating insert may take a long time.  Check for
	 * interrupt now and then.
	 */
	ଯଦି (*ଗଣନା_2 > 0)
	{
	    ଧାଡ଼ି_breakcheck();
	    ଯଦି (got_int)
		*ଗଣନା_2 = 0;
	}

	ଯଦି (--*ଗଣନା_2 > 0)	// repeat what was typed
	{
	    // Vi repeats the insert without replacing characters.
	    ଯଦି (vim_strchr(p_cpo, CPO_REPLCNT) != ଖାଲି)
		State &= ~REPLACE_FLAG;

	    (ଶୂନ୍ୟ)start_redo_ins();
	    ଯଦି (cmdchar == 'r' || cmdchar == 'v')
		stuffRedoReadbuff(ESC_STR);	// no ESC in redo buffer
	    ++RedrawingDisabled;
	    disabled_redraw = TRUE;
	    ଫେରନ୍ତୁ FALSE;	// repeat the insert
	}
	stop_ଭର୍ତ୍ତି(&curwin->w_cursor, TRUE, nomove);
	undisplay_dollar();
    }

    ଯଦି (cmdchar != 'r' && cmdchar != 'v')
	ins_apply_autocmds(EVENT_INSERTLEAVEPRE);

    // When an autoindent was removed, curswant stays after the
    // indent
    ଯଦି (restart_edit == NUL && (colnr_T)temp == curwin->w_cursor.col)
	curwin->w_set_curswant = true;

    // Remember the last Insert position in the '^ mark.
    ଯଦି ((cmdmod.cmod_flags & CMOD_KEEPJUMPS) == 0)
	curbuf->b_last_ଭର୍ତ୍ତି = curwin->w_cursor;

    /*
     * The cursor should end up on the last inserted character.
     * Don't do it for CTRL-O, unless past the end of the line.
     */
    ଯଦି (!nomove
	    && (curwin->w_cursor.col != 0
		|| curwin->w_cursor.coladd > 0)
	    && (restart_edit == NUL
		   || (gchar_cursor() == NUL && !VIsual_active))
#ifdef FEAT_RIGHTLEFT
	    && !revins_on
#endif
				      )
    {
	ଯଦି (curwin->w_cursor.coladd > 0 || get_ve_flags() == VE_ALL)
	{
	    oneleft();
	    ଯଦି (restart_edit != NUL)
		++curwin->w_cursor.coladd;
	}
	ଅନ୍ୟଥା
	{
	    --curwin->w_cursor.col;
	    curwin->w_valid &= ~(VALID_WCOL|VALID_VIRTCOL);
	    // Correct cursor for multi-byte character.
	    ଯଦି (has_mbyte)
		mb_adjust_cursor();
	}
    }

#ifdef HAVE_ନିବେଶ_METHOD
    // Disable IM to allow typing English directly for Normal mode commands.
    // When ":lmap" is enabled don't change 'iminsert' (IM can be enabled as
    // well).
    ଯଦି (!(State & MODE_LANGMAP))
	im_save_status(&curbuf->b_p_iminsert);
    im_set_active(FALSE);
#endif

    State = MODE_NORMAL;
    may_trigger_modechanged();
    // need to position cursor again when on a TAB and when on a char with
    // virtual text.
    ଯଦି (gchar_cursor() == TAB
#ifdef FEAT_PROP_POPUP
	    || curbuf->b_has_textprop
#endif
       )
	curwin->w_valid &= ~(VALID_WROW|VALID_WCOL|VALID_VIRTCOL);

    setmouse();
#ifdef CURSOR_SHAPE
    ui_cursor_shape();		// may show different cursor shape
#endif
    ଯଦି (!p_ek)
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
    conceal_check_cursor_ଧାଡ଼ି(cursor_ଧାଡ଼ି_was_concealed);
#endif

    // When recording or for CTRL-O, need to display the new mode.
    // Otherwise remove the mode message.
    ଯଦି (reg_recording != 0 || restart_edit != NUL)
	showmode();
    ଅନ୍ୟଥା ଯଦି (p_smd && (got_int || !skip_showmode()))
	ସନ୍ଦେଶ("");

    ଫେରନ୍ତୁ TRUE;	    // exit Insert mode
}

#ifdef FEAT_RIGHTLEFT
/*
 * Toggle language: hkmap and revins_on.
 * Move to end of reverse inserted text.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
ins_ctrl_(ଶୂନ୍ୟ)
{
    ଯଦି (revins_on && revins_chars && revins_scol >= 0)
    {
	ଯେତେବେଳେ (gchar_cursor() != NUL && revins_chars--)
	    ++curwin->w_cursor.col;
    }
    p_ri = !p_ri;
    revins_on = (State == MODE_ଭର୍ତ୍ତି && p_ri);
    ଯଦି (revins_on)
    {
	revins_scol = curwin->w_cursor.col;
	revins_legal++;
	revins_chars = 0;
	undisplay_dollar();
    }
    ଅନ୍ୟଥା
	revins_scol = -1;
    p_hkmap = curwin->w_p_rl ^ p_ri;    // be consistent!
    showmode();
}
#endif

/*
 * If 'keymodel' contains "startsel", may start selection.
 * Returns TRUE when a CTRL-O and other keys stuffed.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ins_start_select(ପୂର୍ଣ୍ଣସଂଖ୍ୟା c)
{
    ଯଦି (!km_startsel)
	ଫେରନ୍ତୁ FALSE;
    ବାଛ (c)
    {
	ସ୍ଥିତି K_KHOME:
	ସ୍ଥିତି K_KEND:
	ସ୍ଥିତି K_PAGEUP:
	ସ୍ଥିତି K_KPAGEUP:
	ସ୍ଥିତି K_PAGEDOWN:
	ସ୍ଥିତି K_KPAGEDOWN:
#ifdef MACOS_X
	ସ୍ଥିତି K_ବାମ:
	ସ୍ଥିତି K_ଡାହାଣ:
	ସ୍ଥିତି K_UP:
	ସ୍ଥିତି K_DOWN:
	ସ୍ଥିତି K_END:
	ସ୍ଥିତି K_HOME:
#endif
	    ଯଦି (!(mod_mask & MOD_MASK_SHIFT))
		ଭାଙ୍ଗ;
	    // FALLTHROUGH
	ସ୍ଥିତି K_S_ବାମ:
	ସ୍ଥିତି K_S_ଡାହାଣ:
	ସ୍ଥିତି K_S_UP:
	ସ୍ଥିତି K_S_DOWN:
	ସ୍ଥିତି K_S_END:
	ସ୍ଥିତି K_S_HOME:
	    // Start selection right away, the cursor can move with CTRL-O when
	    // beyond the end of the line.
	    start_selection();

	    // Execute the key in (insert) Select mode.
	    stuffcharReadbuff(Ctrl_O);
	    ଯଦି (mod_mask)
	    {
		char_u	    buf[4];

		buf[0] = K_SPECIAL;
		buf[1] = KS_MODIFIER;
		buf[2] = mod_mask;
		buf[3] = NUL;
		stuffReadbuffLen(buf, 3L);
	    }
	    stuffcharReadbuff(c);
	    ଫେରନ୍ତୁ TRUE;
    }
    ଫେରନ୍ତୁ FALSE;
}

/*
 * <Insert> key in Insert mode: toggle insert/replace mode.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
ins_ଭର୍ତ୍ତି(ପୂର୍ଣ୍ଣସଂଖ୍ୟା replaceState)
{
#ifdef FEAT_EVAL
    set_vim_var_string(VV_INSERTMODE,
		   (char_u *)((State & REPLACE_FLAG) ? "i"
			    : replaceState == MODE_VREPLACE ? "v" : "r"), 1);
#endif
    ins_apply_autocmds(EVENT_INSERTCHANGE);
    ଯଦି (State & REPLACE_FLAG)
	State = MODE_ଭର୍ତ୍ତି | (State & MODE_LANGMAP);
    ଅନ୍ୟଥା
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
    ସ୍ଥିର ଶୂନ୍ୟ
ins_ctrl_o(ଶୂନ୍ୟ)
{
    restart_VIsual_select = 0;
    ଯଦି (State & VREPLACE_FLAG)
	restart_edit = 'V';
    ଅନ୍ୟଥା ଯଦି (State & REPLACE_FLAG)
	restart_edit = 'R';
    ଅନ୍ୟଥା
	restart_edit = 'I';
    ଯଦି (virtual_active())
	ins_at_eol = FALSE;	// cursor always keeps its column
    ଅନ୍ୟଥା
	ins_at_eol = (gchar_cursor() == NUL);
}

/*
 * If the cursor is on an indent, ^T/^D insert/delete one
 * shiftwidth.	Otherwise ^T/^D behave like a "<<" or ">>".
 * Always round the indent to 'shiftwidth', this is compatible
 * with vi.  But vi only supports ^T and ^D after an
 * autoindent, we support it everywhere.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
ins_shift(ପୂର୍ଣ୍ଣସଂଖ୍ୟା c, ପୂର୍ଣ୍ଣସଂଖ୍ୟା lastc)
{
    ଯଦି (stop_arrow() == FAIL)
	ଫେରନ୍ତୁ;
    AppendCharToRedobuff(c);

    /*
     * 0^D and ^^D: remove all indent.
     */
    ଯଦି (c == Ctrl_D && (lastc == '0' || lastc == '^')
						  && curwin->w_cursor.col > 0)
    {
	--curwin->w_cursor.col;
	(ଶୂନ୍ୟ)del_char(FALSE);		// delete the '^' or '0'
	// In Replace mode, restore the characters that '^' or '0' replaced.
	ଯଦି (State & REPLACE_FLAG)
	    replace_କାଢ଼_ins();
	ଯଦି (lastc == '^')
	    old_indent = get_indent();	// remember curr. indent
	change_indent(INDENT_SET, 0, TRUE, 0, TRUE);
    }
    ଅନ୍ୟଥା
	change_indent(c == Ctrl_D ? INDENT_DEC : INDENT_INC, 0, TRUE, 0, TRUE);

    ଯଦି (did_ai && *skipwhite(ml_get_curline()) != NUL)
	did_ai = FALSE;
    did_si = FALSE;
    can_si = FALSE;
    can_si_back = FALSE;
    can_cindent = FALSE;	// no cindenting after ^D or ^T
}

    ସ୍ଥିର ଶୂନ୍ୟ
ins_del(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    temp;

    ଯଦି (stop_arrow() == FAIL)
	ଫେରନ୍ତୁ;
    ଯଦି (gchar_cursor() == NUL)		// delete newline
    {
	temp = curwin->w_cursor.col;
	ଯଦି (!can_bs(BS_EOL)		// only if "eol" included
		|| do_join(2, FALSE, TRUE, FALSE, FALSE) == FAIL)
	    vim_beep(BO_BS);
	ଅନ୍ୟଥା
	{
	    curwin->w_cursor.col = temp;
	    // Adjust orig_line_count in case more lines have been deleted than
	    // have been added. That makes sure, that open_line() later
	    // can access all buffer lines correctly
	    ଯଦି (State & VREPLACE_FLAG &&
		    orig_ଧାଡ଼ି_ଗଣନା > curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା)
		orig_ଧାଡ଼ି_ଗଣନା = curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା;
	}
    }
    ଅନ୍ୟଥା ଯଦି (del_char(FALSE) == FAIL)  // delete char under cursor
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
    ସ୍ଥିର ଶୂନ୍ୟ
ins_bs_one(ଶୂନ୍ୟ)
{
    dec_cursor();
    ଯଦି (State & REPLACE_FLAG)
    {
	// Don't delete characters before the insert point when in
	// Replace mode
	ଯଦି (curwin->w_cursor.lnum != Insstart.lnum
		|| curwin->w_cursor.col >= Insstart.col)
	    replace_do_bs(-1);
    }
    ଅନ୍ୟଥା
	(ଶୂନ୍ୟ)del_char(FALSE);
}

/*
 * Handle Backspace, delete-word and delete-line in Insert mode.
 * Return TRUE when backspace was actually used.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ins_bs(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		c,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		mode,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*inserted_space_p)
{
    linenr_T	lnum;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		cc;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		temp = 0;	    // init for GCC
    colnr_T	save_col;
    colnr_T	mincol;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		did_backspace = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		in_indent;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		oldState;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		cpc[MAX_MCO];	    // composing characters
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		call_fix_indent = FALSE;

    /*
     * can't delete anything in an empty file
     * can't backup past first character in buffer
     * can't backup past starting point unless 'backspace' > 1
     * can backup to a previous line if 'backspace' == 0
     */
    ଯଦି (       BUFEMPTY()
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
	ଫେରନ୍ତୁ FALSE;
    }

    ଯଦି (stop_arrow() == FAIL)
	ଫେରନ୍ତୁ FALSE;
    in_indent = inindent(0);
    ଯଦି (in_indent)
	can_cindent = FALSE;
    end_comment_pending = NUL;	// After BS, don't auto-end comment
#ifdef FEAT_RIGHTLEFT
    ଯଦି (revins_on)	    // put cursor after last inserted char
	inc_cursor();
#endif

    // Virtualedit:
    //	BACKSPACE_CHAR eats a virtual space
    //	BACKSPACE_WORD eats all coladd
    //	BACKSPACE_LINE eats all coladd and keeps going
    ଯଦି (curwin->w_cursor.coladd > 0)
    {
	ଯଦି (mode == BACKSPACE_CHAR)
	{
	    --curwin->w_cursor.coladd;
	    ଫେରନ୍ତୁ TRUE;
	}
	ଯଦି (mode == BACKSPACE_WORD)
	{
	    curwin->w_cursor.coladd = 0;
	    ଫେରନ୍ତୁ TRUE;
	}
	curwin->w_cursor.coladd = 0;
    }

    /*
     * Delete newline!
     */
    ଯଦି (curwin->w_cursor.col == 0)
    {
	lnum = Insstart.lnum;
	ଯଦି (curwin->w_cursor.lnum == lnum
#ifdef FEAT_RIGHTLEFT
			|| revins_on
#endif
				    )
	{
	    ଯଦି (u_save((linenr_T)(curwin->w_cursor.lnum - 2),
			       (linenr_T)(curwin->w_cursor.lnum + 1)) == FAIL)
		ଫେରନ୍ତୁ FALSE;
	    --Insstart.lnum;
	    Insstart.col = ml_get_len(Insstart.lnum);
	}
	/*
	 * In replace mode:
	 * cc < 0: NL was inserted, delete it
	 * cc >= 0: NL was replaced, put original characters back
	 */
	cc = -1;
	ଯଦି (State & REPLACE_FLAG)
	    cc = replace_କାଢ଼();	    // returns -1 if NL was inserted
	/*
	 * In replace mode, in the line we started replacing, we only move the
	 * cursor.
	 */
	ଯଦି ((State & REPLACE_FLAG) && curwin->w_cursor.lnum <= lnum)
	{
	    dec_cursor();
	}
	ଅନ୍ୟଥା
	{
	    ଯଦି (!(State & VREPLACE_FLAG)
				   || curwin->w_cursor.lnum > orig_ଧାଡ଼ି_ଗଣନା)
	    {
		temp = gchar_cursor();	// remember current char
		--curwin->w_cursor.lnum;

		// When "aw" is in 'formatoptions' we must delete the space at
		// the end of the line, otherwise the line will be broken
		// again when auto-formatting.
		ଯଦି (has_format_option(FO_AUTO)
					   && has_format_option(FO_WHITE_PAR))
		{
		    char_u  *ptr = ml_get_buf(curbuf, curwin->w_cursor.lnum,
									FALSE);
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    len = ml_get_curline_len();

		    ଯଦି (len > 0 && ptr[len - 1] == ' ')
		    {
			char_u *newp = alloc(curbuf->b_ml.ml_ଧାଡ଼ି_len - 1);

			ଯଦି (newp != ଖାଲି)
			{
			    mch_memmove(newp, ptr, len - 1);
			    newp[len - 1] = NUL;
			    ଯଦି (curbuf->b_ml.ml_ଧାଡ଼ି_len > len + 1)
				mch_memmove(newp + len, ptr + len + 1,
					   curbuf->b_ml.ml_ଧାଡ଼ି_len - len - 1);

			    ଯଦି (curbuf->b_ml.ml_flags
					      & (ML_ଧାଡ଼ି_DIRTY | ML_ALLOCATED))
				vim_free(curbuf->b_ml.ml_ଧାଡ଼ି_ptr);
			    curbuf->b_ml.ml_ଧାଡ଼ି_ptr = newp;
			    curbuf->b_ml.ml_ଧାଡ଼ି_len--;
			    curbuf->b_ml.ml_ଧାଡ଼ି_textlen--;
			    curbuf->b_ml.ml_flags |= ML_ଧାଡ଼ି_DIRTY;
			}
		    }
		}

		(ଶୂନ୍ୟ)do_join(2, FALSE, FALSE, FALSE, FALSE);
		ଯଦି (temp == NUL && gchar_cursor() != NUL)
		    inc_cursor();
	    }
	    ଅନ୍ୟଥା
		dec_cursor();

	    /*
	     * In MODE_REPLACE mode we have to put back the text that was
	     * replaced by the NL. On the replace stack is first a
	     * NUL-terminated sequence of characters that were deleted and then
	     * the characters that NL replaced.
	     */
	    ଯଦି (State & REPLACE_FLAG)
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
		ଯେତେବେଳେ (cc > 0)
		{
		    save_col = curwin->w_cursor.col;
		    mb_replace_କାଢ଼_ins(cc);
		    curwin->w_cursor.col = save_col;
		    cc = replace_କାଢ଼();
		}
		// restore the characters that NL replaced
		replace_କାଢ଼_ins();
		State = oldState;
	    }
	}
	did_ai = FALSE;
    }
    ଅନ୍ୟଥା
    {
	/*
	 * Delete character(s) before the cursor.
	 */
#ifdef FEAT_RIGHTLEFT
	ଯଦି (revins_on)		// put cursor on last inserted char
	    dec_cursor();
#endif
	mincol = 0;
						// keep indent
	ଯଦି (mode == BACKSPACE_ଧାଡ଼ି
		&& (curbuf->b_p_ai || cindent_on())
#ifdef FEAT_RIGHTLEFT
		&& !revins_on
#endif
			    )
	{
	    save_col = curwin->w_cursor.col;
	    beginline(BL_WHITE);
	    ଯଦି (curwin->w_cursor.col < save_col)
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
	ଯଦି (	   mode == BACKSPACE_CHAR
		&& ((p_sta && in_indent)
		    || ((get_sts_ମୂଲ୍ୟ() != 0
#ifdef FEAT_VARTABS
			|| tabstop_ଗଣନା(curbuf->b_p_vsts_array)
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
	    char_u	*ଧାଡ଼ି;
	    char_u	*ptr;
	    char_u	*cursor_ptr;
	    char_u	*space_ptr;
	    colnr_T	space_vcol = 0;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		prev_space = FALSE;
	    colnr_T	want_col;

	    *inserted_space_p = FALSE;

	    space_ptr = ptr = ଧାଡ଼ି = ml_get_curline();
	    cursor_ptr = ଧାଡ଼ି + curwin->w_cursor.col;

	    // Compute virtual column of cursor position, and find the last
	    // whitespace before cursor that is preceded by non-whitespace.
	    // Use chartabsize() so that virtual text and wrapping are ignored.
	    ଯେତେବେଳେ (ptr < cursor_ptr)
	    {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା	cur_space = VIM_ISWHITE(*ptr);

		ଯଦି (!prev_space && cur_space)
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
	    ଯଦି (p_sta && in_indent)
		want_vcol -= want_vcol % (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)get_sw_ମୂଲ୍ୟ(curbuf);
	    ଅନ୍ୟଥା
#ifdef FEAT_VARTABS
		want_vcol = tabstop_start(want_vcol, get_sts_ମୂଲ୍ୟ(),
						       curbuf->b_p_vsts_array);
#ଅନ୍ୟଥା
		want_vcol -= want_vcol % (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)get_sts_ମୂଲ୍ୟ();
#endif

	    // Find the position to stop backspacing.
	    // Use chartabsize() so that virtual text and wrapping are ignored.
	    ଯେତେବେଳେ (TRUE)
	    {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା size = chartabsize(space_ptr, space_vcol);

		ଯଦି (space_vcol + size > want_vcol)
		    ଭାଙ୍ଗ;
		space_vcol += size;
		MB_PTR_ADV(space_ptr);
	    }
	    want_col = space_ptr - ଧାଡ଼ି;

	    // Delete characters until we are at or before want_col.
	    ଯେତେବେଳେ (curwin->w_cursor.col > want_col)
		ins_bs_one();

	    // Insert extra spaces until we are at want_vcol.
	    ପାଇଁ (; space_vcol < want_vcol; space_vcol++)
	    {
		// Remember the first char we inserted
		ଯଦି (curwin->w_cursor.lnum == Insstart_orig.lnum
				   && curwin->w_cursor.col < Insstart_orig.col)
		    Insstart_orig.col = curwin->w_cursor.col;

		ଯଦି (State & VREPLACE_FLAG)
		    ins_char(' ');
		ଅନ୍ୟଥା
		{
		    ins_str((char_u *)" ", 1);
		    ଯଦି ((State & REPLACE_FLAG))
			replace_ଠେଲ(NUL);
		}
	    }
	}

	/*
	 * Delete up to starting point, start of line or previous word.
	 */
	ଅନ୍ୟଥା
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା cclass = 0, prev_cclass = 0;

	    ଯଦି (has_mbyte)
		cclass = mb_get_class(ml_get_cursor());
	    କର
	    {
#ifdef FEAT_RIGHTLEFT
		ଯଦି (!revins_on) // put cursor on char to be deleted
#endif
		    dec_cursor();

		cc = gchar_cursor();
		// look multi-byte character class
		ଯଦି (has_mbyte)
		{
		    prev_cclass = cclass;
		    cclass = mb_get_class(ml_get_cursor());
		}

		// start of word?
		ଯଦି (mode == BACKSPACE_WORD && !vim_isspace(cc))
		{
		    mode = BACKSPACE_WORD_NOT_SPACE;
		    temp = vim_iswordc(cc);
		}
		// end of word?
		ଅନ୍ୟଥା ଯଦି (mode == BACKSPACE_WORD_NOT_SPACE
			&& ((vim_isspace(cc) || vim_iswordc(cc) != temp)
			|| prev_cclass != cclass))
		{
#ifdef FEAT_RIGHTLEFT
		    ଯଦି (!revins_on)
#endif
			inc_cursor();
#ifdef FEAT_RIGHTLEFT
		    ଅନ୍ୟଥା ଯଦି (State & REPLACE_FLAG)
			dec_cursor();
#endif
		    ଭାଙ୍ଗ;
		}
		ଯଦି (State & REPLACE_FLAG)
		    replace_do_bs(-1);
		ଅନ୍ୟଥା
		{
		    ଯଦି (enc_utf8 && p_deco)
			(ଶୂନ୍ୟ)utfc_ptr2char(ml_get_cursor(), cpc);
		    (ଶୂନ୍ୟ)del_char(FALSE);
		    /*
		     * If there are combining characters and 'delcombine' is set
		     * move the cursor back.  Don't back up before the base
		     * character.
		     */
		    ଯଦି (enc_utf8 && p_deco && cpc[0] != NUL)
			inc_cursor();
#ifdef FEAT_RIGHTLEFT
		    ଯଦି (revins_chars)
		    {
			revins_chars--;
			revins_legal++;
		    }
		    ଯଦି (revins_on && gchar_cursor() == NUL)
			ଭାଙ୍ଗ;
#endif
		}
		// Just a single backspace?:
		ଯଦି (mode == BACKSPACE_CHAR)
		    ଭାଙ୍ଗ;
	    } ଯେତେବେଳେ (
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
    ଯଦି (curwin->w_cursor.col <= 1)
	did_ai = FALSE;

    ଯଦି (call_fix_indent)
	fix_indent();

    /*
     * It's a little strange to put backspaces into the redo
     * buffer, but it makes auto-indent a lot easier to deal
     * with.
     */
    AppendCharToRedobuff(c);

    // If deleted before the insertion point, adjust it
    ଯଦି (curwin->w_cursor.lnum == Insstart_orig.lnum
				  && curwin->w_cursor.col < Insstart_orig.col)
	Insstart_orig.col = curwin->w_cursor.col;

    // vi behaviour: the cursor moves backward but the character that
    //		     was there remains visible
    // Vim behaviour: the cursor moves backward and the character that
    //		      was there is erased from the screen.
    // We can emulate the vi behaviour by pretending there is a dollar
    // displayed even when there isn't.
    //  --pkv Sun Jan 19 01:56:40 EST 2003
    ଯଦି (vim_strchr(p_cpo, CPO_BACKSPACE) != ଖାଲି && dollar_vcol == -1)
	dollar_vcol = curwin->w_virtcol;

#ifdef FEAT_FOLDING
    // When deleting a char the cursor line must never be in a closed fold.
    // E.g., when 'foldmethod' is indent and deleting the first non-white
    // char before a Tab.
    ଯଦି (did_backspace)
	foldOpenCursor();
#endif

    ଫେରନ୍ତୁ did_backspace;
}

/*
 * Handle receiving P_PS: start paste mode.  Inserts the following text up to
 * P_PE literally.
 * When "drop" is TRUE then consume the text and drop it.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
bracketed_paste(paste_mode_T mode, ପୂର୍ଣ୍ଣସଂଖ୍ୟା drop, garray_T *gap)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		c;
    char_u	buf[NUMBUFLEN + MB_MAXBYTES];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		idx = 0;
    char_u	*end = find_termcode((char_u *)"PE");
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ret_char = -1;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		save_allow_ଚାବିଗୁଡ଼ିକ = allow_ଚାବିଗୁଡ଼ିକ;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		save_paste = p_paste;

    // If the end code is too long we can't detect it, read everything.
    ଯଦି (end != ଖାଲି && STRLEN(end) >= NUMBUFLEN)
	end = ଖାଲି;
    ++no_mapping;
    allow_ଚାବିଗୁଡ଼ିକ = 0;
    ଯଦି (!p_paste)
	// Also have the side effects of setting 'paste' to make it work much
	// faster.
	set_option_ମୂଲ୍ୟ_give_err((char_u *)"paste", TRUE, ଖାଲି, 0);

    ପାଇଁ (;;)
    {
	// When the end is not defined read everything there is.
	ଯଦି (end == ଖାଲି && vpeekc() == NUL)
	    ଭାଙ୍ଗ;
	କର
	    c = vgetc();
	ଯେତେବେଳେ (c == K_IGNORE || c == K_VER_SCROLLBAR || c == K_HOR_SCROLLBAR);

	ଯଦି (c == NUL || got_int || (ex_normal_busy > 0 && c == Ctrl_C))
	    // When CTRL-C was encountered the typeahead will be flushed and we
	    // won't get the end sequence.  Except when using ":normal".
	    ଭାଙ୍ଗ;

	ଯଦି (has_mbyte)
	    idx += (*mb_char2bytes)(c, buf + idx);
	ଅନ୍ୟଥା
	    buf[idx++] = c;
	buf[idx] = NUL;
	ଯଦି (end != ଖାଲି && STRNCMP(buf, end, idx) == 0)
	{
	    ଯଦି (end[idx] == NUL)
		ଭାଙ୍ଗ; // Found the end of paste code.
	    ଜାରି;
	}
	ଯଦି (!drop)
	{
	    ବାଛ (mode)
	    {
		ସ୍ଥିତି PASTE_CMDLINE:
		    put_on_cmdline(buf, idx, TRUE);
		    ଭାଙ୍ଗ;

		ସ୍ଥିତି PASTE_EX:
		    // add one for the NUL that is going to be appended
		    ଯଦି (gap != ଖାଲି && ga_grow(gap, idx + 1) == OK)
		    {
			mch_memmove((ଅକ୍ଷର *)gap->ga_data + gap->ga_len,
							     buf, (size_t)idx);
			gap->ga_len += idx;
		    }
		    ଭାଙ୍ଗ;

		ସ୍ଥିତି PASTE_ଭର୍ତ୍ତି:
		    ଯଦି (stop_arrow() == OK)
		    {
			c = buf[0];
			ଯଦି (idx == 1 && (c == CAR || c == K_KENTER || c == NL))
			    ins_eol(c);
			ଅନ୍ୟଥା
			{
			    ins_char_bytes(buf, idx);
			    AppendToRedobuffLit(buf, idx);
			}
		    }
		    ଭାଙ୍ଗ;

		ସ୍ଥିତି PASTE_ONE_CHAR:
		    ଯଦି (ret_char == -1)
		    {
			ଯଦି (has_mbyte)
			    ret_char = (*mb_ptr2char)(buf);
			ଅନ୍ୟଥା
			    ret_char = buf[0];
		    }
		    ଭାଙ୍ଗ;
	    }
	}
	idx = 0;
    }

    --no_mapping;
    allow_ଚାବିଗୁଡ଼ିକ = save_allow_ଚାବିଗୁଡ଼ିକ;
    ଯଦି (!save_paste)
	set_option_ମୂଲ୍ୟ_give_err((char_u *)"paste", FALSE, ଖାଲି, 0);

    ଫେରନ୍ତୁ ret_char;
}

#ଯଦି defined(FEAT_GUI_TABLINE)
    ସ୍ଥିର ଶୂନ୍ୟ
ins_tabline(ପୂର୍ଣ୍ଣସଂଖ୍ୟା c)
{
    // We will be leaving the current window, unless closing another tab.
    ଯଦି (c != K_TABMENU || current_tabmenu != TABLINE_MENU_CLOSE
		|| (current_tab != 0 && current_tab != tabpage_index(curtab)))
    {
	undisplay_dollar();
	start_arrow(&curwin->w_cursor);
	can_cindent = TRUE;
    }

    ଯଦି (c == K_TABLINE)
	goto_tabpage(current_tab);
    ଅନ୍ୟଥା
    {
	handle_tabmenu();
	redraw_statuslines();	// will redraw the tabline when needed
    }
}
#endif

#ଯଦି defined(FEAT_GUI)
    ଶୂନ୍ୟ
ins_scroll(ଶୂନ୍ୟ)
{
    pos_T	tpos;

    undisplay_dollar();
    tpos = curwin->w_cursor;
    ଯଦି (gui_do_scroll())
    {
	start_arrow(&tpos);
	can_cindent = TRUE;
    }
}

    ଶୂନ୍ୟ
ins_horscroll(ଶୂନ୍ୟ)
{
    pos_T	tpos;

    undisplay_dollar();
    tpos = curwin->w_cursor;
    ଯଦି (do_mousescroll_horiz(scrollbar_ମୂଲ୍ୟ))
    {
	start_arrow(&tpos);
	can_cindent = TRUE;
    }
}
#endif

    ସ୍ଥିର ଶୂନ୍ୟ
ins_ବାମ(ଶୂନ୍ୟ)
{
    pos_T	tpos;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		end_change = dont_sync_undo == FALSE; // end undoable change

#ifdef FEAT_FOLDING
    ଯଦି ((fdo_flags & FDO_HOR) && ଚାବି_Typed)
	foldOpenCursor();
#endif
    undisplay_dollar();
    tpos = curwin->w_cursor;
    ଯଦି (oneleft() == OK)
    {
#ଯଦି defined(FEAT_XIM) && defined(FEAT_GUI_GTK)
	// Only call start_arrow() when not busy with preediting, it will
	// break undo.  K_LEFT is inserted in im_correct_cursor().
	ଯଦି (p_imst == IM_OVER_THE_SPOT || !im_is_preediting())
#endif
	{
	    start_arrow_with_change(&tpos, end_change);
	    ଯଦି (!end_change)
		AppendCharToRedobuff(K_ବାମ);
	}
#ifdef FEAT_RIGHTLEFT
	// If exit reversed string, position is fixed
	ଯଦି (revins_scol != -1 && (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)curwin->w_cursor.col >= revins_scol)
	    revins_legal++;
	revins_chars++;
#endif
    }

    /*
     * if 'whichwrap' set for cursor in insert mode may go to
     * previous line
     */
    ଅନ୍ୟଥା ଯଦି (vim_strchr(p_ww, '[') != ଖାଲି && curwin->w_cursor.lnum > 1)
    {
	// always break undo when moving upwards/downwards, else undo may break
	start_arrow(&tpos);
	--(curwin->w_cursor.lnum);
	coladvance((colnr_T)MAXCOL);
	curwin->w_set_curswant = true;	// so we stay at the end
    }
    ଅନ୍ୟଥା
	vim_beep(BO_CRSR);
    dont_sync_undo = FALSE;
}

    ସ୍ଥିର ଶୂନ୍ୟ
ins_home(ପୂର୍ଣ୍ଣସଂଖ୍ୟା c)
{
    pos_T	tpos;

#ifdef FEAT_FOLDING
    ଯଦି ((fdo_flags & FDO_HOR) && ଚାବି_Typed)
	foldOpenCursor();
#endif
    undisplay_dollar();
    tpos = curwin->w_cursor;
    ଯଦି (c == K_C_HOME)
	curwin->w_cursor.lnum = 1;
    curwin->w_cursor.col = 0;
    curwin->w_cursor.coladd = 0;
    curwin->w_curswant = 0;
    start_arrow(&tpos);
}

    ସ୍ଥିର ଶୂନ୍ୟ
ins_end(ପୂର୍ଣ୍ଣସଂଖ୍ୟା c)
{
    pos_T	tpos;

#ifdef FEAT_FOLDING
    ଯଦି ((fdo_flags & FDO_HOR) && ଚାବି_Typed)
	foldOpenCursor();
#endif
    undisplay_dollar();
    tpos = curwin->w_cursor;
    ଯଦି (c == K_C_END)
	curwin->w_cursor.lnum = curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା;
    coladvance((colnr_T)MAXCOL);
    curwin->w_curswant = MAXCOL;

    start_arrow(&tpos);
}

    ସ୍ଥିର ଶୂନ୍ୟ
ins_s_ବାମ(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା end_change = dont_sync_undo == FALSE; // end undoable change
#ifdef FEAT_FOLDING
    ଯଦି ((fdo_flags & FDO_HOR) && ଚାବି_Typed)
	foldOpenCursor();
#endif
    undisplay_dollar();
    ଯଦି (curwin->w_cursor.lnum > 1 || curwin->w_cursor.col > 0)
    {
	start_arrow_with_change(&curwin->w_cursor, end_change);
	ଯଦି (!end_change)
	    AppendCharToRedobuff(K_S_ବାମ);
	(ଶୂନ୍ୟ)bck_word(1L, FALSE, FALSE);
	curwin->w_set_curswant = true;
    }
    ଅନ୍ୟଥା
	vim_beep(BO_CRSR);
    dont_sync_undo = FALSE;
}

    ସ୍ଥିର ଶୂନ୍ୟ
ins_ଡାହାଣ(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା end_change = dont_sync_undo == FALSE; // end undoable change

#ifdef FEAT_FOLDING
    ଯଦି ((fdo_flags & FDO_HOR) && ଚାବି_Typed)
	foldOpenCursor();
#endif
    undisplay_dollar();
    ଯଦି (gchar_cursor() != NUL || virtual_active())
    {
	start_arrow_with_change(&curwin->w_cursor, end_change);
	ଯଦି (!end_change)
	    AppendCharToRedobuff(K_ଡାହାଣ);
	curwin->w_set_curswant = true;
	ଯଦି (virtual_active())
	    oneright();
	ଅନ୍ୟଥା
	{
	    ଯଦି (has_mbyte)
		curwin->w_cursor.col += (*mb_ptr2len)(ml_get_cursor());
	    ଅନ୍ୟଥା
		++curwin->w_cursor.col;
	}

#ifdef FEAT_RIGHTLEFT
	revins_legal++;
	ଯଦି (revins_chars)
	    revins_chars--;
#endif
    }
    // if 'whichwrap' set for cursor in insert mode, may move the
    // cursor to the next line
    ଅନ୍ୟଥା ଯଦି (vim_strchr(p_ww, ']') != ଖାଲି
	    && curwin->w_cursor.lnum < curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା)
    {
	start_arrow(&curwin->w_cursor);
	curwin->w_set_curswant = true;
	++curwin->w_cursor.lnum;
	curwin->w_cursor.col = 0;
    }
    ଅନ୍ୟଥା
	vim_beep(BO_CRSR);
    dont_sync_undo = FALSE;
}

    ସ୍ଥିର ଶୂନ୍ୟ
ins_s_ଡାହାଣ(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା end_change = dont_sync_undo == FALSE; // end undoable change
#ifdef FEAT_FOLDING
    ଯଦି ((fdo_flags & FDO_HOR) && ଚାବି_Typed)
	foldOpenCursor();
#endif
    undisplay_dollar();
    ଯଦି (curwin->w_cursor.lnum < curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା
	    || gchar_cursor() != NUL)
    {
	start_arrow_with_change(&curwin->w_cursor, end_change);
	ଯଦି (!end_change)
	    AppendCharToRedobuff(K_S_ଡାହାଣ);
	(ଶୂନ୍ୟ)fwd_word(1L, FALSE, 0);
	curwin->w_set_curswant = true;
    }
    ଅନ୍ୟଥା
	vim_beep(BO_CRSR);
    dont_sync_undo = FALSE;
}

    ସ୍ଥିର ଶୂନ୍ୟ
ins_up(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		startcol)	// when TRUE move to Insstart.col
{
    pos_T	tpos;
    linenr_T	old_topline = curwin->w_topline;
#ifdef FEAT_DIFF
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		old_topfill = curwin->w_topfill;
#endif

    undisplay_dollar();
    tpos = curwin->w_cursor;
    ଯଦି (cursor_up(1L, TRUE) == OK)
    {
	ଯଦି (startcol)
	    coladvance(getvcol_nolist(&Insstart));
	ଯଦି (old_topline != curwin->w_topline
#ifdef FEAT_DIFF
		|| old_topfill != curwin->w_topfill
#endif
		)
	    redraw_later(UPD_VALID);
	start_arrow(&tpos);
	can_cindent = TRUE;
    }
    ଅନ୍ୟଥା
	vim_beep(BO_CRSR);
}

    ସ୍ଥିର ଶୂନ୍ୟ
ins_pageup(ଶୂନ୍ୟ)
{
    pos_T	tpos;

    undisplay_dollar();

    ଯଦି (mod_mask & MOD_MASK_CTRL)
    {
	// <C-PageUp>: tab page back
	ଯଦି (first_tabpage->tp_next != ଖାଲି)
	{
	    start_arrow(&curwin->w_cursor);
	    goto_tabpage(-1);
	}
	ଫେରନ୍ତୁ;
    }

    tpos = curwin->w_cursor;
    ଯଦି (pagescroll(BACKWARD, 1L, FALSE) == OK)
    {
	start_arrow(&tpos);
	can_cindent = TRUE;
    }
    ଅନ୍ୟଥା
	vim_beep(BO_CRSR);
}

    ସ୍ଥିର ଶୂନ୍ୟ
ins_down(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		startcol)	// when TRUE move to Insstart.col
{
    pos_T	tpos;
    linenr_T	old_topline = curwin->w_topline;
#ifdef FEAT_DIFF
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		old_topfill = curwin->w_topfill;
#endif

    undisplay_dollar();
    tpos = curwin->w_cursor;
    ଯଦି (cursor_down(1L, TRUE) == OK)
    {
	ଯଦି (startcol)
	    coladvance(getvcol_nolist(&Insstart));
	ଯଦି (old_topline != curwin->w_topline
#ifdef FEAT_DIFF
		|| old_topfill != curwin->w_topfill
#endif
		)
	    redraw_later(UPD_VALID);
	start_arrow(&tpos);
	can_cindent = TRUE;
    }
    ଅନ୍ୟଥା
	vim_beep(BO_CRSR);
}

    ସ୍ଥିର ଶୂନ୍ୟ
ins_pagedown(ଶୂନ୍ୟ)
{
    pos_T	tpos;

    undisplay_dollar();

    ଯଦି (mod_mask & MOD_MASK_CTRL)
    {
	// <C-PageDown>: tab page forward
	ଯଦି (first_tabpage->tp_next != ଖାଲି)
	{
	    start_arrow(&curwin->w_cursor);
	    goto_tabpage(0);
	}
	ଫେରନ୍ତୁ;
    }

    tpos = curwin->w_cursor;
    ଯଦି (pagescroll(FORWARD, 1L, FALSE) == OK)
    {
	start_arrow(&tpos);
	can_cindent = TRUE;
    }
    ଅନ୍ୟଥା
	vim_beep(BO_CRSR);
}

#ifdef FEAT_DND
    ସ୍ଥିର ଶୂନ୍ୟ
ins_drop(ଶୂନ୍ୟ)
{
    do_put('~', ଖାଲି, BACKWARD, 1L, PUT_CURSEND);
}
#endif

/*
 * Handle TAB in Insert or Replace mode.
 * Return TRUE when the TAB needs to be inserted like a normal character.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ins_tab(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ind;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		temp;

    ଯଦି (Insstart_blank_vcol == MAXCOL && curwin->w_cursor.lnum == Insstart.lnum)
	Insstart_blank_vcol = get_nolist_virtcol();
    ଯଦି (echeck_abbr(TAB + ABBR_OFF))
	ଫେରନ୍ତୁ FALSE;

    ind = inindent(0);
    ଯଦି (ind)
	can_cindent = FALSE;

    /*
     * When nothing special, insert TAB like a normal character.
     */
    ଯଦି (!curbuf->b_p_et
#ifdef FEAT_VARTABS
	    && !(p_sta && ind
		// These five lines mean 'tabstop' != 'shiftwidth'
		&& ((tabstop_ଗଣନା(curbuf->b_p_vts_array) > 1)
		    || (tabstop_ଗଣନା(curbuf->b_p_vts_array) == 1
			&& tabstop_first(curbuf->b_p_vts_array)
						       != get_sw_ମୂଲ୍ୟ(curbuf))
		    || (tabstop_ଗଣନା(curbuf->b_p_vts_array) == 0
			&& curbuf->b_p_ts != get_sw_ମୂଲ୍ୟ(curbuf))))
	    && tabstop_ଗଣନା(curbuf->b_p_vsts_array) == 0
#ଅନ୍ୟଥା
	    && !(p_sta && ind && curbuf->b_p_ts != get_sw_ମୂଲ୍ୟ(curbuf))
#endif
	    && get_sts_ମୂଲ୍ୟ() == 0)
	ଫେରନ୍ତୁ TRUE;

    ଯଦି (stop_arrow() == FAIL)
	ଫେରନ୍ତୁ TRUE;

    did_ai = FALSE;
    did_si = FALSE;
    can_si = FALSE;
    can_si_back = FALSE;
    AppendToRedobuff((char_u *)"\t");

#ifdef FEAT_VARTABS
    ଯଦି (p_sta && ind)		// insert tab in indent, use 'shiftwidth'
    {
	temp = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)get_sw_ମୂଲ୍ୟ(curbuf);
	temp -= get_nolist_virtcol() % temp;
    }
    ଅନ୍ୟଥା ଯଦି (tabstop_ଗଣନା(curbuf->b_p_vsts_array) > 0 || curbuf->b_p_sts != 0)
				// use 'softtabstop' when set
	temp = tabstop_padding(get_nolist_virtcol(), get_sts_ମୂଲ୍ୟ(),
						     curbuf->b_p_vsts_array);
    ଅନ୍ୟଥା			// otherwise use 'tabstop'
	temp = tabstop_padding(get_nolist_virtcol(), curbuf->b_p_ts,
						     curbuf->b_p_vts_array);
#ଅନ୍ୟଥା
    ଯଦି (p_sta && ind)		// insert tab in indent, use 'shiftwidth'
	temp = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)get_sw_ମୂଲ୍ୟ(curbuf);
    ଅନ୍ୟଥା ଯଦି (curbuf->b_p_sts != 0) // use 'softtabstop' when set
	temp = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)get_sts_ମୂଲ୍ୟ();
    ଅନ୍ୟଥା			// otherwise use 'tabstop'
	temp = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)curbuf->b_p_ts;
    temp -= get_nolist_virtcol() % temp;
#endif

    /*
     * Insert the first space with ins_char().	It will delete one char in
     * replace mode.  Insert the rest with ins_str(); it will not delete any
     * chars.  For MODE_VREPLACE state, we use ins_char() for all characters.
     */
    ins_char(' ');
    ଯେତେବେଳେ (--temp > 0)
    {
	ଯଦି (State & VREPLACE_FLAG)
	    ins_char(' ');
	ଅନ୍ୟଥା
	{
	    ins_str((char_u *)" ", 1);
	    ଯଦି (State & REPLACE_FLAG)	    // no char replaced
		replace_ଠେଲ(NUL);
	}
    }

    /*
     * When 'expandtab' not set: Replace spaces by TABs where possible.
     */
#ifdef FEAT_VARTABS
    ଯଦି (!curbuf->b_p_et && (tabstop_ଗଣନା(curbuf->b_p_vsts_array) > 0
			    || get_sts_ମୂଲ୍ୟ() > 0
			    || (p_sta && ind)))
#ଅନ୍ୟଥା
    ଯଦି (!curbuf->b_p_et && (get_sts_ମୂଲ୍ୟ() || (p_sta && ind)))
#endif
    {
	char_u		*ptr;
	char_u		*saved_ଧାଡ଼ି = ଖାଲି;	// init for GCC
	pos_T		pos;
	pos_T		fpos;
	pos_T		*cursor;
	colnr_T		want_vcol, vcol;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		change_col = -1;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		save_list = curwin->w_p_list;
	char_u		*tab = (char_u *)"\t";
	chartabsize_T	cts;

	/*
	 * Get the current line.  For MODE_VREPLACE state, don't make real
	 * changes yet, just work on a copy of the line.
	 */
	ଯଦି (State & VREPLACE_FLAG)
	{
	    pos = curwin->w_cursor;
	    cursor = &pos;
	    saved_ଧାଡ଼ି = vim_strnsave(ml_get_curline(), ml_get_curline_len());
	    ଯଦି (saved_ଧାଡ଼ି == ଖାଲି)
		ଫେରନ୍ତୁ FALSE;
	    ptr = saved_ଧାଡ଼ି + pos.col;
	}
	ଅନ୍ୟଥା
	{
	    ptr = ml_get_cursor();
	    cursor = &curwin->w_cursor;
	}

	// When 'L' is not in 'cpoptions' a tab always takes up 'ts' spaces.
	ଯଦି (vim_strchr(p_cpo, CPO_LISTWM) == ଖାଲି)
	    curwin->w_p_list = FALSE;

	// Find first white before the cursor
	fpos = curwin->w_cursor;
	ଯେତେବେଳେ (fpos.col > 0 && VIM_ISWHITE(ptr[-1]))
	{
	    --fpos.col;
	    --ptr;
	}

	// In Replace mode, don't change characters before the insert point.
	ଯଦି ((State & REPLACE_FLAG)
		&& fpos.lnum == Insstart.lnum
		&& fpos.col < Insstart.col)
	{
	    ptr += Insstart.col - fpos.col;
	    fpos.col = Insstart.col;
	}

	// compute virtual column numbers of first white and cursor
	getvcol(curwin, &fpos, &vcol, ଖାଲି, ଖାଲି, 0);
	getvcol(curwin, cursor, &want_vcol, ଖାଲି, ଖାଲି, 0);

	init_chartabsize_arg(&cts, curwin, 0, vcol, tab, tab);

	// Use as many TABs as possible.  Beware of 'breakindent', 'showbreak'
	// and 'linebreak' adding extra virtual columns.
	ଯେତେବେଳେ (VIM_ISWHITE(*ptr))
	{
	    i = lbr_chartabsize(&cts);
	    ଯଦି (cts.cts_vcol + i > want_vcol)
		ଭାଙ୍ଗ;
	    ଯଦି (*ptr != TAB)
	    {
		*ptr = TAB;
		ଯଦି (change_col < 0)
		{
		    change_col = fpos.col;  // Column of first change
		    // May have to adjust Insstart
		    ଯଦି (fpos.lnum == Insstart.lnum && fpos.col < Insstart.col)
			Insstart.col = fpos.col;
		}
	    }
	    ++fpos.col;
	    ++ptr;
	    cts.cts_vcol += i;
	}
	vcol = cts.cts_vcol;
	clear_chartabsize_arg(&cts);

	ଯଦି (change_col >= 0)
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    repl_off = 0;

	    // Skip over the spaces we need.
	    init_chartabsize_arg(&cts, curwin, 0, vcol, ptr, ptr);
	    ଯେତେବେଳେ (cts.cts_vcol < want_vcol && *cts.cts_ptr == ' ')
	    {
		cts.cts_vcol += lbr_chartabsize(&cts);
		++cts.cts_ptr;
		++repl_off;
	    }
	    ptr = cts.cts_ptr;
	    vcol = cts.cts_vcol;
	    clear_chartabsize_arg(&cts);

	    ଯଦି (vcol > want_vcol)
	    {
		// Must have a char with 'showbreak' just before it.
		--ptr;
		--repl_off;
	    }
	    fpos.col += repl_off;

	    // Delete following spaces.
	    i = cursor->col - fpos.col;
	    ଯଦି (i > 0)
	    {
#ifdef FEAT_PROP_POPUP
		ଯଦି (!(State & VREPLACE_FLAG))
		{
		    char_u  *newp;
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    col;

		    newp = alloc(curbuf->b_ml.ml_ଧାଡ଼ି_len - i);
		    ଯଦି (newp == ଖାଲି)
			ଫେରନ୍ତୁ FALSE;

		    col = ptr - curbuf->b_ml.ml_ଧାଡ଼ି_ptr;
		    ଯଦି (col > 0)
			mch_memmove(newp, ptr - col, col);
		    mch_memmove(newp + col, ptr + i,
					   curbuf->b_ml.ml_ଧାଡ଼ି_len - col - i);

		    ଯଦି (curbuf->b_ml.ml_flags & (ML_ଧାଡ଼ି_DIRTY | ML_ALLOCATED))
			vim_free(curbuf->b_ml.ml_ଧାଡ଼ି_ptr);
		    curbuf->b_ml.ml_ଧାଡ଼ି_ptr = newp;
		    curbuf->b_ml.ml_ଧାଡ଼ି_len -= i;
		    curbuf->b_ml.ml_ଧାଡ଼ି_textlen = 0;
		    curbuf->b_ml.ml_flags =
			   (curbuf->b_ml.ml_flags | ML_ଧାଡ଼ି_DIRTY) & ~ML_EMPTY;
		}
		ଅନ୍ୟଥା
#endif
		    STRMOVE(ptr, ptr + i);
		// correct replace stack.
		ଯଦି ((State & REPLACE_FLAG) && !(State & VREPLACE_FLAG))
		    ପାଇଁ (temp = i; --temp >= 0; )
			replace_join(repl_off);
	    }
#ifdef FEAT_NETBEANS_INTG
	    ଯଦି (netbeans_active())
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
	    ଯଦି (State & VREPLACE_FLAG)
	    {
		// Backspace from real cursor to change_col
		backspace_until_column(change_col);

		// Insert each char in saved_line from changed_col to
		// ptr-cursor
		ins_bytes_len(saved_ଧାଡ଼ି + change_col,
						    cursor->col - change_col);
	    }
	}

	ଯଦି (State & VREPLACE_FLAG)
	    vim_free(saved_ଧାଡ଼ି);
	curwin->w_p_list = save_list;
    }

    ଫେରନ୍ତୁ FALSE;
}

/*
 * Handle CR or NL in insert mode.
 * Return FAIL when out of memory or can't undo.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ins_eol(ପୂର୍ଣ୍ଣସଂଖ୍ୟା c)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    i;

    ଯଦି (echeck_abbr(c + ABBR_OFF))
	ଫେରନ୍ତୁ OK;
    ଯଦି (stop_arrow() == FAIL)
	ଫେରନ୍ତୁ FAIL;
    undisplay_dollar();

    /*
     * Strange Vi behaviour: In Replace mode, typing a NL will not delete the
     * character under the cursor.  Only push a NUL on the replace stack,
     * nothing to put back when the NL is deleted.
     */
    ଯଦି ((State & REPLACE_FLAG) && !(State & VREPLACE_FLAG))
	replace_ଠେଲ(NUL);

    /*
     * In MODE_VREPLACE state, a NL replaces the rest of the line, and starts
     * replacing the next line, so we push all of the characters left on the
     * line onto the replace stack.  This is not done here though, it is done
     * in open_line().
     */

    // Put cursor on NUL if on the last char and coladd is 1 (happens after
    // CTRL-O).
    ଯଦି (virtual_active() && curwin->w_cursor.coladd > 0)
	coladvance(getviscol());

#ifdef FEAT_RIGHTLEFT
    // NL in reverse insert will always start in the end of
    // current line.
    ଯଦି (revins_on)
	curwin->w_cursor.col += ml_get_cursor_len();
#endif

    AppendToRedobuff(NL_STR);
    i = open_ଧାଡ଼ି(FORWARD,
	    has_format_option(FO_RET_COMS) ? OPENLINE_DO_COM : 0, old_indent,
	    ଖାଲି);
    old_indent = 0;
    can_cindent = TRUE;
#ifdef FEAT_FOLDING
    // When inserting a line the cursor line must never be in a closed fold.
    foldOpenCursor();
#endif

    ଫେରନ୍ତୁ i;
}

#ifdef FEAT_DIGRAPHS
/*
 * Handle digraph in insert mode.
 * Returns character still to be inserted, or NUL when nothing remaining to be
 * done.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ins_digraph(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    c;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    cc;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    did_putchar = FALSE;

    pc_status = PC_STATUS_UNSET;
    ଯଦି (redrawing() && !char_avail())
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
    ++allow_ଚାବିଗୁଡ଼ିକ;
    c = plain_vgetc();
    --no_mapping;
    --allow_ଚାବିଗୁଡ଼ିକ;
    ଯଦି (did_putchar)
	// when the line fits in 'columns' the '?' is at the start of the next
	// line and will not be removed by the redraw
	edit_unputchar();

    ଯଦି (IS_SPECIAL(c) || mod_mask)	    // special key
    {
	clear_showcmd();
	ଭର୍ତ୍ତି_special(c, TRUE, FALSE);
	ଫେରନ୍ତୁ NUL;
    }
    ଯଦି (c != ESC)
    {
	did_putchar = FALSE;
	ଯଦି (redrawing() && !char_avail())
	{
	    // may need to redraw when no more chars available now
	    ins_redraw(FALSE);

	    ଯଦି (char2cells(c) == 1)
	    {
		ins_redraw(FALSE);
		edit_putchar(c, TRUE);
		did_putchar = TRUE;
	    }
	    add_to_showcmd_c(c);
	}
	++no_mapping;
	++allow_ଚାବିଗୁଡ଼ିକ;
	cc = plain_vgetc();
	--no_mapping;
	--allow_ଚାବିଗୁଡ଼ିକ;
	ଯଦି (did_putchar)
	    // when the line fits in 'columns' the '?' is at the start of the
	    // next line and will not be removed by a redraw
	    edit_unputchar();
	ଯଦି (cc != ESC)
	{
	    AppendToRedobuff((char_u *)CTRL_V_STR);
	    c = digraph_get(c, cc, TRUE);
	    clear_showcmd();
	    ଫେରନ୍ତୁ c;
	}
    }
    clear_showcmd();
    ଫେରନ୍ତୁ NUL;
}
#endif

/*
 * Handle CTRL-E and CTRL-Y in Insert mode: copy char from other line.
 * Returns the char to be inserted, or NUL if none found.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ins_copychar(linenr_T lnum)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    c;
    char_u	    *ptr, *prev_ptr;
    char_u	    *ଧାଡ଼ି;
    chartabsize_T   cts;

    ଯଦି (lnum < 1 || lnum > curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା)
    {
	vim_beep(BO_COPY);
	ଫେରନ୍ତୁ NUL;
    }

    // try to advance to the cursor column
    validate_virtcol();
    ଧାଡ଼ି = ml_get(lnum);
    prev_ptr = ଧାଡ଼ି;
    init_chartabsize_arg(&cts, curwin, lnum, 0, ଧାଡ଼ି, ଧାଡ଼ି);
    ଯେତେବେଳେ (cts.cts_vcol < curwin->w_virtcol && *cts.cts_ptr != NUL)
    {
	prev_ptr = cts.cts_ptr;
	cts.cts_vcol += lbr_chartabsize_adv(&cts);
    }
    ଯଦି (cts.cts_vcol > curwin->w_virtcol)
	ptr = prev_ptr;
    ଅନ୍ୟଥା
	ptr = cts.cts_ptr;
    clear_chartabsize_arg(&cts);

    c = (*mb_ptr2char)(ptr);
    ଯଦି (c == NUL)
	vim_beep(BO_COPY);
    ଫେରନ୍ତୁ c;
}

/*
 * CTRL-Y or CTRL-E typed in Insert mode.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ins_ctrl_ey(ପୂର୍ଣ୍ଣସଂଖ୍ୟା tc)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    c = tc;

    ଯଦି (ctrl_x_mode_scroll())
    {
	ଯଦି (c == Ctrl_Y)
	    scrolldown_clamp();
	ଅନ୍ୟଥା
	    scrollup_clamp();
	redraw_later(UPD_VALID);
    }
    ଅନ୍ୟଥା
    {
	c = ins_copychar(curwin->w_cursor.lnum + (c == Ctrl_Y ? -1 : 1));
	ଯଦି (c != NUL)
	{
	    long	tw_save;

	    // The character must be taken literally, insert like it
	    // was typed after a CTRL-V, and pretend 'textwidth'
	    // wasn't set.  Digits, 'o' and 'x' are special after a
	    // CTRL-V, don't use it for these.
	    ଯଦି (c < 256 && !SAFE_isalnum(c))
		AppendToRedobuff((char_u *)CTRL_V_STR);	// CTRL-V
	    tw_save = curbuf->b_p_tw;
	    curbuf->b_p_tw = -1;
	    ଭର୍ତ୍ତି_special(c, TRUE, FALSE);
	    curbuf->b_p_tw = tw_save;
#ifdef FEAT_RIGHTLEFT
	    revins_chars++;
	    revins_legal++;
#endif
	    c = Ctrl_V;	// pretend CTRL-V is last character
	    auto_format(FALSE, TRUE);
	}
    }
    ଫେରନ୍ତୁ c;
}

/*
 * Get the value that w_virtcol would have when 'list' is off.
 * Unless 'cpo' contains the 'L' flag.
 */
    colnr_T
get_nolist_virtcol(ଶୂନ୍ୟ)
{
    // check validity of cursor in current buffer
    ଯଦି (curwin->w_buffer == ଖାଲି
	|| curwin->w_buffer->b_ml.ml_mfp == ଖାଲି
	|| curwin->w_cursor.lnum > curwin->w_buffer->b_ml.ml_ଧାଡ଼ି_ଗଣନା)
	ଫେରନ୍ତୁ 0;
    ଯଦି (curwin->w_p_list && vim_strchr(p_cpo, CPO_LISTWM) == ଖାଲି)
	ଫେରନ୍ତୁ getvcol_nolist(&curwin->w_cursor);
    validate_virtcol();
    ଫେରନ୍ତୁ curwin->w_virtcol;
}

#ଯଦି defined(FEAT_EVAL)
/*
 * Handle the InsertCharPre autocommand.
 * "c" is the character that was typed.
 * Return a pointer to allocated memory with the replacement string.
 * Return NULL to continue inserting "c".
 */
    ସ୍ଥିର char_u *
do_ଭର୍ତ୍ତି_char_pre(ପୂର୍ଣ୍ଣସଂଖ୍ୟା c)
{
    char_u	*res;
    char_u	buf[MB_MAXBYTES + 1];
    size_t	buflen;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		save_State = State;

    // Return quickly when there is nothing to do.
    ଯଦି (!has_insertcharpre())
	ଫେରନ୍ତୁ ଖାଲି;

    ଯଦି (c == Ctrl_RSB)
	ଫେରନ୍ତୁ ଖାଲି;

    ଯଦି (has_mbyte)
	buflen = (*mb_char2bytes)(c, buf);
    ଅନ୍ୟଥା
    {
	buf[0] = c;
	buflen = 1;
    }
    buf[buflen] = NUL;

    // Lock the text to avoid weird things from happening.
    ++textlock;
    set_vim_var_string(VV_CHAR, buf, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)buflen);  // set v:char

    res = ଖାଲି;
    ଯଦି (ins_apply_autocmds(EVENT_INSERTCHARPRE))
    {
	// Get the value of v:char.  It may be empty or more than one
	// character.  Only use it when changed, otherwise continue with the
	// original character to avoid breaking autoindent.
	ଯଦି (STRCMP(buf, get_vim_var_str(VV_CHAR)) != 0)
	    res = vim_strsave(get_vim_var_str(VV_CHAR));
    }

    set_vim_var_string(VV_CHAR, ଖାଲି, -1);  // clear v:char
    --textlock;

    // Restore the State, it may have been changed.
    State = save_State;

    ଫେରନ୍ତୁ res;
}
#endif

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
get_can_cindent(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ can_cindent;
}

    ଶୂନ୍ୟ
set_can_cindent(ପୂର୍ଣ୍ଣସଂଖ୍ୟା val)
{
    can_cindent = val;
}

/*
 * Trigger "event" and take care of fixing undo.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ins_apply_autocmds(event_T event)
{
    varnumber_T	tick = CHANGEDTICK(curbuf);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

    r = apply_autocmds(event, ଖାଲି, ଖାଲି, FALSE, curbuf);

    // If u_savesub() was called then we are not prepared to start
    // a new line.  Call u_save() with no contents to fix that.
    // Except when leaving Insert mode.
    ଯଦି (event != EVENT_INSERTLEAVE && tick != CHANGEDTICK(curbuf))
	u_save(curwin->w_cursor.lnum, (linenr_T)(curwin->w_cursor.lnum + 1));

    ଫେରନ୍ତୁ r;
}
