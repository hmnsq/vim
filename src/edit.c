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

#include "vim.h"

#define BACKSPACE_CHAR		    1
#define BACKSPACE_WORD		    2
#define BACKSPACE_WORD_NOT_SPACE    3
#define BACKSPACE_LINE		    4

// Set when doing something for completion that may call edit() recursively,
// which is not allowed.
static số_nguyên	compl_busy = FALSE;


static void ins_ctrl_v(void);
static void insert_special(số_nguyên, số_nguyên, số_nguyên);
static void redo_literal(số_nguyên c);
static void start_arrow_common(pos_T *end_insert_pos, số_nguyên change);
#ifdef FEAT_SPELL
static void check_spell_redraw(void);
#endif
static void stop_insert(pos_T *end_insert_pos, số_nguyên esc, số_nguyên nomove);
static số_nguyên  echeck_abbr(số_nguyên);
static void mb_replace_pop_ins(số_nguyên cc);
static void replace_flush(void);
static void replace_do_bs(số_nguyên limit_col);
static số_nguyên del_char_after_col(số_nguyên limit_col);
static void ins_reg(void);
static void ins_ctrl_g(void);
static void ins_ctrl_hat(void);
static số_nguyên  ins_esc(long *count, số_nguyên cmdchar, số_nguyên nomove);
#ifdef FEAT_RIGHTLEFT
static void ins_ctrl_(void);
#endif
static số_nguyên ins_start_select(số_nguyên c);
static void ins_insert(số_nguyên replaceState);
static void ins_ctrl_o(void);
static void ins_shift(số_nguyên c, số_nguyên lastc);
static void ins_del(void);
static số_nguyên  ins_bs(số_nguyên c, số_nguyên mode, số_nguyên *inserted_space_p);
#nếu defined(FEAT_GUI_TABLINE)
static void ins_tabline(số_nguyên c);
#endif
static void ins_left(void);
static void ins_home(số_nguyên c);
static void ins_end(số_nguyên c);
static void ins_s_left(void);
static void ins_right(void);
static void ins_s_right(void);
static void ins_up(số_nguyên startcol);
static void ins_pageup(void);
static void ins_down(số_nguyên startcol);
static void ins_pagedown(void);
#ifdef FEAT_DND
static void ins_drop(void);
#endif
static số_nguyên  ins_tab(void);
#ifdef FEAT_DIGRAPHS
static số_nguyên  ins_digraph(void);
#endif
static số_nguyên  ins_ctrl_ey(số_nguyên tc);
#nếu defined(FEAT_EVAL)
static char_u *do_insert_char_pre(số_nguyên c);
#endif

static colnr_T	Insstart_textlen;	// length of line when insert started
static colnr_T	Insstart_blank_vcol;	// vcol for first inserted blank
static số_nguyên	update_Insstart_orig = TRUE; // set Insstart_orig to Insstart

static string_T	last_insert = {NULL, 0};    // the text of the previous insert,
					    // K_SPECIAL and CSI are escaped
static số_nguyên	last_insert_skip; // nr of chars in front of previous insert
static số_nguyên	new_insert_skip;  // nr of chars in front of current insert
static số_nguyên	did_restart_edit;	// "restart_edit" when calling edit()

static số_nguyên	can_cindent;		// may do cindenting on this line

#ifdef FEAT_RIGHTLEFT
static số_nguyên	revins_on;		// reverse insert mode on
static số_nguyên	revins_chars;		// how much to skip after edit
static số_nguyên	revins_legal;		// was the last char 'legal'?
static số_nguyên	revins_scol;		// start column of revins session
#endif

static số_nguyên	ins_need_undo;		// call u_save() before inserting a
					// char.  Set when edit() is called.
					// after that arrow_used is used.

static số_nguyên	dont_sync_undo = FALSE;	// CTRL-G U prevents syncing undo for
					// the next left/right cursor key

// With 'autocompletedelay' set, arm the delay and let the main loop fire
// Insert-mode autocommands; the popup is shown later on K_COMPLETE_DELAY.
// Otherwise trigger completion right away.
#define TRIGGER_AUTOCOMPLETE()			\
    do {					\
	update_screen(UPD_VALID);  /* Show char (deletion) immediately */ \
	out_flush();				\
	ins_compl_enable_autocomplete();	\
	ins_compl_arm_autostart();		\
	nếu (!ins_compl_arm_autocomplete_delay())\
	    goto docomplete;			\
    } trong_khi (0)

#define MAY_TRIGGER_AUTOCOMPLETE(c)				\
    do {							\
	nếu (ins_compl_has_autocomplete() && !char_avail()	\
		&& curwin->w_cursor.col > 0)			\
	{							\
	    (c) = char_before_cursor();				\
	    nếu (vim_isprintc(c))				\
		TRIGGER_AUTOCOMPLETE();				\
	}							\
    } trong_khi (0)

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
    số_nguyên
edit(
    số_nguyên		cmdchar,
    số_nguyên		startln,	// if set, insert at start of line
    long	count)
{
    số_nguyên		c = 0;
    char_u	*ptr;
    số_nguyên		lastc = 0;
    số_nguyên		mincol;
    static linenr_T o_lnum = 0;
    số_nguyên		i;
    số_nguyên		did_backspace = TRUE;	    // previous char was backspace
    số_nguyên		line_is_white = FALSE;	    // line is empty before insert
    linenr_T	old_topline = 0;	    // topline before insertion
#ifdef FEAT_DIFF
    số_nguyên		old_topfill = -1;
#endif
    số_nguyên		inserted_space = FALSE;     // just inserted a space
    số_nguyên		replaceState = MODE_REPLACE;
    số_nguyên		nomove = FALSE;		    // don't move cursor on return
#ifdef FEAT_JOB_CHANNEL
    số_nguyên		cmdchar_todo = cmdchar;
#endif
#ifdef FEAT_CONCEAL
    số_nguyên		cursor_line_was_concealed;
#endif
    số_nguyên		ins_just_started = TRUE;

    // Remember whether editing was restarted after CTRL-O.
    did_restart_edit = restart_edit;

    // sleep before redrawing, needed for "CTRL-O :" that results in an
    // error message
    check_for_delay(TRUE);

    // set Insstart_orig to Insstart
    update_Insstart_orig = TRUE;

#ifdef HAVE_SANDBOX
    // Don't allow inserting in the sandbox.
    nếu (sandbox != 0)
    {
	emsg(_(e_not_allowed_in_sandbox));
	trả_về FALSE;
    }
#endif
    // Don't allow changes in the buffer while editing the cmdline.  The
    // caller of getcmdline() may get confused.
    // Don't allow recursive insert mode when busy with completion.
    nếu (textlock != 0 || ins_compl_active() || compl_busy || pum_visible())
    {
	emsg(_(e_not_allowed_to_change_text_or_change_window));
	trả_về FALSE;
    }
    ins_compl_clear();	    // clear stuff for CTRL-X mode

    /*
     * Trigger InsertEnter autocommands.  Do not do this for "r<CR>" or "grx".
     */
    nếu (cmdchar != 'r' && cmdchar != 'v')
    {
	pos_T   save_cursor = curwin->w_cursor;

#ifdef FEAT_EVAL
	nếu (cmdchar == 'R')
	    ptr = (char_u *)"r";
	không_thì nếu (cmdchar == 'V')
	    ptr = (char_u *)"v";
	không_thì
	    ptr = (char_u *)"i";
	set_vim_var_string(VV_INSERTMODE, ptr, 1);
	set_vim_var_string(VV_CHAR, NULL, -1);  // clear v:char
#endif
	ins_apply_autocmds(EVENT_INSERTENTER);

	// Check for changed highlighting, e.g. for ModeMsg.
	nếu (need_highlight_changed)
	    highlight_changed();

	// Make sure the cursor didn't move.  Do call check_cursor_col() in
	// case the text was modified.  Since Insert mode was not started yet
	// a call to check_cursor_col() may move the cursor, especially with
	// the "A" command, thus set State to avoid that. Also check that the
	// line number is still valid (lines may have been deleted).
	// Do not restore if v:char was set to a non-empty string.
	nếu (!EQUAL_POS(curwin->w_cursor, save_cursor)
#ifdef FEAT_EVAL
		&& *get_vim_var_str(VV_CHAR) == NUL
#endif
		&& save_cursor.lnum <= curbuf->b_ml.ml_line_count)
	{
	    số_nguyên save_state = State;

	    curwin->w_cursor = save_cursor;
	    State = MODE_INSERT;
	    check_cursor_col();
	    State = save_state;
	}
    }

#ifdef FEAT_CONCEAL
    // Check if the cursor line was concealed before changing State.
    cursor_line_was_concealed = curwin->w_p_cole > 0
						&& conceal_cursor_line(curwin);
#endif

    /*
     * When doing a paste with the middle mouse button, Insstart is set to
     * where the paste started.
     */
    nếu (where_paste_started.lnum != 0)
	Insstart = where_paste_started;
    không_thì
    {
	Insstart = curwin->w_cursor;
	nếu (startln)
	    Insstart.col = 0;
    }
    Insstart_textlen = (colnr_T)linetabsize_str(ml_get_curline());
    Insstart_blank_vcol = MAXCOL;
    nếu (!did_ai)
	ai_col = 0;

    nếu (cmdchar != NUL && restart_edit == 0)
    {
	ResetRedobuff();
	AppendNumberToRedobuff(count);
	nếu (cmdchar == 'V' || cmdchar == 'v')
	{
	    // "gR" or "gr" command
	    AppendCharToRedobuff('g');
	    AppendCharToRedobuff((cmdchar == 'v') ? 'r' : 'R');
	}
	không_thì
	{
	    nếu (cmdchar == K_PS)
		AppendCharToRedobuff('a');
	    không_thì
		AppendCharToRedobuff(cmdchar);
	    nếu (cmdchar == 'g')		    // "gI" command
		AppendCharToRedobuff('I');
	    không_thì nếu (cmdchar == 'r')	    // "r<CR>" command
		count = 1;		    // insert only one <CR>
	}
    }

    nếu (cmdchar == 'R')
    {
	State = MODE_REPLACE;
    }
    không_thì nếu (cmdchar == 'V' || cmdchar == 'v')
    {
	State = MODE_VREPLACE;
	replaceState = MODE_VREPLACE;
	orig_line_count = curbuf->b_ml.ml_line_count;
	vr_lines_changed = 1;
    }
    không_thì
	State = MODE_INSERT;

    may_trigger_modechanged();
    stop_insert_mode = FALSE;

#ifdef FEAT_CONCEAL
    // Check if the cursor line needs redrawing after changing State.  If
    // 'concealcursor' is "n" it needs to be redrawn without concealing.
    conceal_check_cursor_line(cursor_line_was_concealed);
#endif

    // Need to position cursor again when on a TAB and when on a char with
    // virtual text.
    nếu (gchar_cursor() == TAB
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
    nếu (curbuf->b_p_iminsert == B_IMODE_LMAP)
	State |= MODE_LANGMAP;
#ifdef HAVE_INPUT_METHOD
    im_set_active(curbuf->b_p_iminsert == B_IMODE_IM);
#endif

    setmouse();
    clear_showcmd();
#ifdef FEAT_RIGHTLEFT
    // there is no reverse replace mode
    revins_on = (State == MODE_INSERT && p_ri);
    nếu (revins_on)
	undisplay_dollar();
    revins_chars = 0;
    revins_legal = 0;
    revins_scol = -1;
#endif
    nếu (!p_ek)
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
    nếu (restart_edit != 0 && stuff_empty())
    {
	/*
	 * After a paste we consider text typed to be part of the insert for
	 * the pasted text. You can backspace over the pasted text too.
	 */
	nếu (where_paste_started.lnum)
	    arrow_used = FALSE;
	không_thì
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
	nếu (((ins_at_eol && curwin->w_cursor.lnum == o_lnum)
		    || curwin->w_curswant > curwin->w_virtcol)
		&& *(ptr = ml_get_curline() + curwin->w_cursor.col) != NUL)
	{
	    nếu (ptr[1] == NUL)
		++curwin->w_cursor.col;
	    không_thì nếu (has_mbyte)
	    {
		i = (*mb_ptr2len)(ptr);
		nếu (ptr[i] == NUL)
		    curwin->w_cursor.col += i;
	    }
	}
	ins_at_eol = FALSE;
    }
    không_thì
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
    nếu (!p_im && did_restart_edit == 0)
	foldOpenCursor();
#endif

    /*
     * If 'showmode' is set, show the current (insert/replace/..) mode.
     * A warning message for changing a readonly file is given here, before
     * actually changing anything.  It's put after the mode, if any.
     */
    i = 0;
    nếu (p_smd && msg_silent == 0)
	i = showmode();

    nếu (!p_im && did_restart_edit == 0)
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
    new_insert_skip = (số_nguyên)inserted.length;
    nếu (inserted.string != NULL)
	vim_free(inserted.string);

    old_indent = 0;

    /*
     * Main loop in Insert mode: repeat until Insert mode is left.
     */
    cho (;;)
    {
#ifdef FEAT_RIGHTLEFT
	nếu (!revins_legal)
	    revins_scol = -1;	    // reset on illegal motions
	không_thì
	    revins_legal = 0;
#endif
	nếu (arrow_used)	    // don't repeat insert when arrow key used
	    count = 0;

	nếu (update_Insstart_orig)
	    Insstart_orig = Insstart;

	nếu (stop_insert_mode && !ins_compl_active())
	{
	    // ":stopinsert" used or 'insertmode' reset
	    count = 0;
	    goto doESCkey;
	}

	// set curwin->w_curswant for next K_DOWN or K_UP
	nếu (!arrow_used)
	    curwin->w_set_curswant = true;

	// If there is no typeahead may check for timestamps (e.g., for when a
	// menu invoked a shell command).
	nếu (stuff_empty())
	{
	    did_check_timestamps = FALSE;
	    nếu (need_check_timestamps)
		check_timestamps(FALSE);
	}

	/*
	 * When emsg() was called msg_scroll will have been set.
	 */
	msg_scroll = FALSE;

#ifdef FEAT_GUI
	// When 'mousefocus' is set a mouse movement may have taken us to
	// another window.  "need_mouse_correct" may then be set because of an
	// autocommand.
	nếu (need_mouse_correct)
	    gui_mouse_correct();
#endif

#ifdef FEAT_FOLDING
	// Open fold at the cursor line, according to 'foldopen'.
	nếu (fdo_flags & FDO_INSERT)
	    foldOpenCursor();
	// Close folds where the cursor isn't, according to 'foldclose'
	nếu (!char_avail())
	    foldCheckClose();
#endif

#ifdef FEAT_JOB_CHANNEL
	nếu (bt_prompt(curbuf))
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
	nếu (curbuf->b_mod_set
		&& curwin->w_p_wrap
		&& !curwin->w_p_sms
		&& !did_backspace
		&& curwin->w_topline == old_topline
#ifdef FEAT_DIFF
		&& curwin->w_topfill == old_topfill
#endif
		&& count <= 1
		)
	{
	    mincol = curwin->w_wcol;
	    validate_cursor_col();

	    nếu (
#ifdef FEAT_VARTABS
		curwin->w_wcol < mincol - tabstop_at(get_nolist_virtcol(),
						     curbuf->b_p_ts,
						     curbuf->b_p_vts_array,
						     FALSE)
#không_thì
		(số_nguyên)curwin->w_wcol < mincol - curbuf->b_p_ts
#endif
		    && curwin->w_wrow ==
				   curwin->w_height - 1 - get_scrolloff_value()
		    && (curwin->w_cursor.lnum != curwin->w_topline
#ifdef FEAT_DIFF
			|| curwin->w_topfill > 0
#endif
		    ))
	    {
#ifdef FEAT_DIFF
		nếu (curwin->w_topfill > 0)
		    --curwin->w_topfill;
		không_thì
#endif
#ifdef FEAT_FOLDING
		nếu (hasFolding(curwin->w_topline, NULL, &old_topline))
		    set_topline(curwin, old_topline + 1);
		không_thì
#endif
		    set_topline(curwin, curwin->w_topline + 1);
	    }
	}

	// May need to adjust w_topline to show the cursor.
	nếu (count <= 1)
	    update_topline();

	did_backspace = FALSE;

	nếu (count <= 1)
	    validate_cursor();		// may set must_redraw

	/*
	 * Redraw the display when no characters are waiting.
	 * Also shows mode, ruler and positions cursor.
	 */
	ins_redraw(TRUE);

	nếu (curwin->w_p_scb)
	    do_check_scrollbind(TRUE);

	nếu (curwin->w_p_crb)
	    do_check_cursorbind();
	nếu (count <= 1)
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
	nếu (c != K_CURSORHOLD && c != K_COMPLETE_DELAY)
	    lastc = c;		// remember the previous char for CTRL-D

	// After using CTRL-G U the next cursor key will not break undo.
	nếu (dont_sync_undo == MAYBE)
	    dont_sync_undo = TRUE;
	không_thì
	    dont_sync_undo = FALSE;
	nếu (cmdchar == K_PS)
	    // Got here from normal mode when bracketed paste started.
	    c = K_PS;
	không_thì
	{
	    // Trigger autocomplete when entering Insert mode, either directly
	    // or via change commands like 'ciw', 'cw', etc., before the first
	    // character is typed.
	    nếu (ins_just_started)
	    {
		ins_just_started = FALSE;
		nếu (ins_compl_has_autocomplete() && !char_avail()
			&& curwin->w_cursor.col > 0)
		{
		    c = char_before_cursor();
		    nếu (vim_isprintc(c))
		    {
			ins_compl_enable_autocomplete();
			ins_compl_arm_autostart();
			ins_compl_init_get_longest();
#ifdef FEAT_RIGHTLEFT
			nếu (p_hkmap)
			    c = hkmap(c);		// Hebrew mode mapping
#endif
			// Defer until the delay expires (K_COMPLETE_DELAY), or
			// trigger now when no delay is in effect.
			nếu (!ins_compl_arm_autocomplete_delay())
			    goto docomplete;
		    }
		}
	    }

	    do
	    {
		c = safe_vgetc();

		nếu (stop_insert_mode
#ifdef FEAT_TERMINAL
			|| (c == K_IGNORE && term_use_loop())
#endif
		   )
		{
		    // Insert mode ended, possibly from a callback, or a timer
		    // must have opened a terminal window.
		    nếu (c != K_IGNORE && c != K_NOP)
			vungetc(c);
		    count = 0;

		    nếu (!bt_prompt(curwin->w_buffer)
#ifdef FEAT_TERMINAL
			    && !bt_terminal(curwin->w_buffer)
#endif
			    && stop_insert_mode)
			// :stopinsert command via callback or via server command
			nomove = FALSE;
		    không_thì
			nomove = TRUE;
		    ins_compl_prep(ESC);
		    goto doESCkey;
		}
	    } trong_khi (c == K_IGNORE || c == K_NOP);
	}

	// Don't want K_CURSORHOLD for the second key, e.g., after CTRL-V.
	did_cursorhold = TRUE;
	nếu (c != K_CURSORHOLD && c != K_COMPLETE_DELAY)
	{
	    // Don't want delayed autocompletion from the previous key either.
	    ins_compl_clear_autocomplete_delay();
	    ins_compl_disarm_autostart();
	    // A completion already on screen goes on being what it was.
	    nếu (!ins_compl_active())
		ins_compl_disable_autocomplete();
	}

#ifdef FEAT_RIGHTLEFT
	nếu (p_hkmap && KeyTyped)
	    c = hkmap(c);		// Hebrew mode mapping
#endif

	// If the window was made so small that nothing shows, make it at least
	// one line and one column when typing.
	nếu (KeyTyped && !KeyStuffed)
	    win_ensure_size();

	/*
	 * Special handling of keys while the popup menu is visible or wanted
	 * and the cursor is still in the completed word.  Only when there is
	 * a match, skip this when no matches were found.
	 */
	nếu (ins_compl_active() && curwin->w_cursor.col >= ins_compl_col()
		&& ins_compl_has_shown_match() && pum_wanted())
	{
	    // BS: Delete one character from "compl_leader".
	    nếu ((c == K_BS || c == Ctrl_H)
			&& curwin->w_cursor.col > ins_compl_col()
			&& (c = ins_compl_bs()) == NUL)
		continue;

	    // When no match was selected or it was edited.
	    nếu (!ins_compl_used_match())
	    {
		// CTRL-L: Add one character from the current match to
		// "compl_leader".  Except when at the original match and
		// there is nothing to add, CTRL-L works like CTRL-P then.
		nếu (c == Ctrl_L
			&& (!ctrl_x_mode_line_or_eval()
			    || ins_compl_long_shown_match()))
		{
		    ins_compl_addfrommatch();
		    continue;
		}

		// A non-white character that fits in with the current
		// completion: Add to "compl_leader".
		nếu (ins_compl_accept_char(c))
		{
#nếu defined(FEAT_EVAL)
		    // Trigger InsertCharPre.
		    char_u *str = do_insert_char_pre(c);
		    char_u *p;

		    nếu (str != NULL)
		    {
			cho (p = str; *p != NUL; MB_PTR_ADV(p))
			    ins_compl_addleader(PTR2CHAR(p));
			vim_free(str);
		    }
		    không_thì
#endif
			ins_compl_addleader(c);
		    continue;
		}

		// Pressing CTRL-Y selects the current match.  When
		// ins_compl_enter_selects() is set the Enter key does the
		// same.
		nếu ((c == Ctrl_Y || (ins_compl_enter_selects()
				    && (c == CAR || c == K_KENTER || c == NL)))
			&& stop_arrow() == OK)
		{
		    ins_compl_delete();
		    nếu (ins_compl_preinsert_longest()
			    && !ins_compl_is_match_selected())
		    {
			ins_compl_insert(FALSE, TRUE);
			ins_compl_init_get_longest();
			continue;
		    }
		    không_thì
			ins_compl_insert(FALSE, FALSE);
		}
		// Delete preinserted text when typing special chars
		không_thì nếu (IS_WHITE_NL_OR_NUL(c) && ins_compl_preinsert_effect())
		    ins_compl_delete();
	    }
	}

	// Prepare for or stop CTRL-X mode.  This doesn't do completion, but
	// it does fix up the text when finishing completion.
	ins_compl_init_get_longest();
	nếu (ins_compl_prep(c))
	    continue;

	// CTRL-\ CTRL-N goes to Normal mode,
	// CTRL-\ CTRL-G goes to mode selected with 'insertmode',
	// CTRL-\ CTRL-O is like CTRL-O but without moving the cursor.
	nếu (c == Ctrl_BSL)
	{
	    // may need to redraw when no more chars available now
	    ins_redraw(FALSE);
	    ++no_mapping;
	    ++allow_keys;
	    c = plain_vgetc();
	    --no_mapping;
	    --allow_keys;
	    nếu (c != Ctrl_N && c != Ctrl_G && c != Ctrl_O)
	    {
		// it's something else
		vungetc(c);
		c = Ctrl_BSL;
	    }
	    không_thì nếu (c == Ctrl_G && p_im)
		continue;
	    không_thì
	    {
		nếu (c == Ctrl_O)
		{
		    ins_ctrl_o();
		    ins_at_eol = FALSE;	// cursor keeps its column
		    nomove = TRUE;
		}
		count = 0;
		goto doESCkey;
	    }
	}

#ifdef FEAT_DIGRAPHS
	c = do_digraph(c);
#endif

	nếu ((c == Ctrl_V || c == Ctrl_Q) && ctrl_x_mode_cmdline())
	    goto docomplete;
	nếu (c == Ctrl_V || c == Ctrl_Q)
	{
	    ins_ctrl_v();
	    c = Ctrl_V;	// pretend CTRL-V is last typed character
	    continue;
	}

	nếu (cindent_on() && ctrl_x_mode_none())
	{
	    // A key name preceded by a bang means this key is not to be
	    // inserted.  Skip ahead to the re-indenting below.
	    // A key name preceded by a star means that indenting has to be
	    // done before inserting the key.
	    line_is_white = inindent(0);
	    nếu (in_cinkeys(c, '!', line_is_white))
		goto force_cindent;
	    nếu (can_cindent && in_cinkeys(c, '*', line_is_white)
							&& stop_arrow() == OK)
		do_c_expr_indent();
	}

#ifdef FEAT_RIGHTLEFT
	nếu (curwin->w_p_rl)
	    switch (c)
	    {
		case K_LEFT:	c = K_RIGHT; break;
		case K_S_LEFT:	c = K_S_RIGHT; break;
		case K_C_LEFT:	c = K_C_RIGHT; break;
		case K_RIGHT:	c = K_LEFT; break;
		case K_S_RIGHT: c = K_S_LEFT; break;
		case K_C_RIGHT: c = K_C_LEFT; break;
	    }
#endif

	/*
	 * If 'keymodel' contains "startsel", may start selection.  If it
	 * does, a CTRL-O and c will be stuffed, we need to get these
	 * characters.
	 */
	nếu (ins_start_select(c))
	    continue;

	/*
	 * The big switch to handle a character in insert mode.
	 */
	switch (c)
	{
	case ESC:	// End input mode
	    nếu (echeck_abbr(ESC + ABBR_OFF))
		break;
	    // FALLTHROUGH

	case Ctrl_C:	// End input mode
	    nếu (c == Ctrl_C && cmdwin_type != 0)
	    {
		// Close the cmdline window.
		cmdwin_result = K_IGNORE;
		got_int = FALSE; // don't stop executing autocommands et al.
		nomove = TRUE;
		goto doESCkey;
	    }
#ifdef FEAT_JOB_CHANNEL
	    nếu (c == Ctrl_C && bt_prompt(curbuf))
	    {
		nếu (invoke_prompt_interrupt())
		{
		    nếu (!bt_prompt(curbuf))
			// buffer changed to a non-prompt buffer, get out of
			// Insert mode
			goto doESCkey;
		    break;
		}
	    }
#endif

#ifdef UNIX
do_intr:
#endif
	    // when 'insertmode' set, and not halfway a mapping, don't leave
	    // Insert mode
	    nếu (goto_im())
	    {
		nếu (got_int)
		{
		    (void)vgetc();		// flush all buffers
		    got_int = FALSE;
		}
		không_thì
		    vim_beep(BO_IM);
		break;
	    }
doESCkey:
	    // Drop a pending autocomplete so it does not outlive Insert mode.
	    ins_compl_clear_autocomplete_delay();
	    /*
	     * This is the ONLY return from edit()!
	     */
	    // Always update o_lnum, so that a "CTRL-O ." that adds a line
	    // still puts the cursor back after the inserted text.
	    nếu (ins_at_eol && gchar_cursor() == NUL)
		o_lnum = curwin->w_cursor.lnum;

	    nếu (ins_esc(&count, cmdchar, nomove))
	    {
		// When CTRL-C was typed got_int will be set, with the result
		// that the autocommands won't be executed. When mapped got_int
		// is not set, but let's keep the behavior the same.
		nếu (cmdchar != 'r' && cmdchar != 'v' && c != Ctrl_C)
		    ins_apply_autocmds(EVENT_INSERTLEAVE);
		did_cursorhold = FALSE;

		// ins_redraw() triggers TextChangedI only when no characters
		// are in the typeahead buffer, so reset curbuf->b_last_changedtick only
		// if the TextChangedI was not blocked by char_avail() (e.g. using :norm!)
		// and the TextChangedI autocommand has been triggered.
		nếu (!char_avail() && curbuf->b_last_changedtick_i == CHANGEDTICK(curbuf))
		    curbuf->b_last_changedtick = CHANGEDTICK(curbuf);
		trả_về (c == Ctrl_O);
	    }
	    continue;

	case Ctrl_Z:	// suspend when 'insertmode' set
	    nếu (!p_im)
		goto normalchar;	// insert CTRL-Z as normal char
	    do_cmdline_cmd((char_u *)"stop");
#ifdef CURSOR_SHAPE
	    ui_cursor_shape();		// may need to update cursor shape
#endif
	    continue;

	case Ctrl_O:	// execute one command
#ifdef FEAT_COMPL_FUNC
	    nếu (ctrl_x_mode_omni())
		goto docomplete;
#endif
	    nếu (echeck_abbr(Ctrl_O + ABBR_OFF))
		break;
	    ins_ctrl_o();

	    // don't move the cursor left when 'virtualedit' has "onemore".
	    nếu (get_ve_flags() & VE_ONEMORE)
	    {
		ins_at_eol = FALSE;
		nomove = TRUE;
	    }
	    count = 0;
	    goto doESCkey;

	case K_INS:	// toggle insert/replace mode
	case K_KINS:
	    ins_insert(replaceState);
	    break;

	case K_SELECT:	// end of Select mode mapping - ignore
	    break;

	case K_HELP:	// Help key works like <ESC> <Help>
	case K_F1:
	case K_XF1:
	    stuffcharReadbuff(K_HELP);
	    nếu (p_im)
		need_start_insertmode = TRUE;
	    goto doESCkey;

#ifdef FEAT_NETBEANS_INTG
	case K_F21:	// NetBeans command
	    ++no_mapping;		// don't map the next key hits
	    i = plain_vgetc();
	    --no_mapping;
	    netbeans_keycommand(i);
	    break;
#endif

	case K_ZERO:	// Insert the previously inserted text.
	case NUL:
	case Ctrl_A:
	    // For ^@ the trailing ESC will end the insert, unless there is an
	    // error.
	    nếu (stuff_inserted(NUL, 1L, (c == Ctrl_A)) == FAIL
						   && c != Ctrl_A && !p_im)
		goto doESCkey;		// quit insert mode
	    inserted_space = FALSE;
	    break;

	case Ctrl_R:	// insert the contents of a register
	    nếu (ctrl_x_mode_register() && !ins_compl_active())
		goto docomplete;
	    ins_reg();
	    auto_format(FALSE, TRUE);
	    inserted_space = FALSE;
	    break;

	case Ctrl_G:	// commands starting with CTRL-G
	    ins_ctrl_g();
	    break;

	case Ctrl_HAT:	// switch input mode and/or langmap
	    ins_ctrl_hat();
	    break;

#ifdef FEAT_RIGHTLEFT
	case Ctrl__:	// switch between languages
	    nếu (!p_ari)
		goto normalchar;
	    ins_ctrl_();
	    break;
#endif

	case Ctrl_D:	// Make indent one shiftwidth smaller.
#nếu defined(FEAT_FIND_ID)
	    nếu (ctrl_x_mode_path_defines())
		goto docomplete;
#endif
	    // FALLTHROUGH

	case Ctrl_T:	// Make indent one shiftwidth greater.
	    nếu (c == Ctrl_T && ctrl_x_mode_thesaurus())
	    {
		nếu (has_compl_option(FALSE))
		    goto docomplete;
		break;
	    }

	    ins_shift(c, lastc);
	    auto_format(FALSE, TRUE);
	    inserted_space = FALSE;
	    break;

	case K_DEL:	// delete character under the cursor
	case K_KDEL:
	    ins_del();
	    auto_format(FALSE, TRUE);
	    break;

	case K_BS:	// delete character before the cursor
	case K_S_BS:
	case Ctrl_H:
	    did_backspace = ins_bs(c, BACKSPACE_CHAR, &inserted_space);
	    auto_format(FALSE, TRUE);
	    nếu (did_backspace)
		MAY_TRIGGER_AUTOCOMPLETE(c);
	    break;

	case Ctrl_W:	// delete word before the cursor
#ifdef FEAT_JOB_CHANNEL
	    nếu (bt_prompt(curbuf) && (mod_mask & MOD_MASK_SHIFT) == 0)
	    {
		// In a prompt window CTRL-W is used for window commands.
		// Use Shift-CTRL-W to delete a word.
		stuffcharReadbuff(Ctrl_W);
		restart_edit = 'A';
		nomove = TRUE;
		count = 0;
		goto doESCkey;
	    }
#endif
	    did_backspace = ins_bs(c, BACKSPACE_WORD, &inserted_space);
	    auto_format(FALSE, TRUE);
	    nếu (did_backspace)
		MAY_TRIGGER_AUTOCOMPLETE(c);
	    break;

	case Ctrl_U:	// delete all inserted text in current line
#ifdef FEAT_COMPL_FUNC
	    // CTRL-X CTRL-U completes with 'completefunc'.
	    nếu (ctrl_x_mode_function())
		goto docomplete;
#endif
	    did_backspace = ins_bs(c, BACKSPACE_LINE, &inserted_space);
	    auto_format(FALSE, TRUE);
	    inserted_space = FALSE;
	    nếu (did_backspace)
		MAY_TRIGGER_AUTOCOMPLETE(c);
	    break;

	case K_LEFTMOUSE:   // mouse keys
	case K_LEFTMOUSE_NM:
	case K_LEFTDRAG:
	case K_LEFTRELEASE:
	case K_LEFTRELEASE_NM:
	case K_MOUSEMOVE:
	case K_MIDDLEMOUSE:
	case K_MIDDLEDRAG:
	case K_MIDDLERELEASE:
	case K_RIGHTMOUSE:
	case K_RIGHTDRAG:
	case K_RIGHTRELEASE:
	case K_X1MOUSE:
	case K_X1DRAG:
	case K_X1RELEASE:
	case K_X2MOUSE:
	case K_X2DRAG:
	case K_X2RELEASE:
	    ins_mouse(c);
	    break;

	case K_MOUSEDOWN: // Default action for scroll wheel up: scroll up
	    ins_mousescroll(MSCR_DOWN);
	    break;

	case K_MOUSEUP:	// Default action for scroll wheel down: scroll down
	    ins_mousescroll(MSCR_UP);
	    break;

	case K_MOUSELEFT: // Scroll wheel left
	    ins_mousescroll(MSCR_LEFT);
	    break;

	case K_MOUSERIGHT: // Scroll wheel right
	    ins_mousescroll(MSCR_RIGHT);
	    break;

	case K_PS:
	    bracketed_paste(PASTE_INSERT, FALSE, NULL);
	    nếu (cmdchar == K_PS)
		// invoked from normal mode, bail out
		goto doESCkey;
	    break;
	case K_PE:
	    // Got K_PE without K_PS, ignore.
	    break;

#ifdef FEAT_GUI_TABLINE
	case K_TABLINE:
	case K_TABMENU:
	    ins_tabline(c);
	    break;
#endif

	case K_IGNORE:	// Something mapped to nothing
	    break;

	case K_COMMAND:		    // <Cmd>command<CR>
	case K_SCRIPT_COMMAND:	    // <ScriptCmd>command<CR>
	    {
		bufref_T    save_curbuf;
		varnumber_T tick = CHANGEDTICK(curbuf);

		set_bufref(&save_curbuf, curbuf);
		do_cmdkey_command(c, 0);

#ifdef FEAT_TERMINAL
		nếu (term_use_loop())
		    // Started a terminal that gets the input, exit Insert mode.
		    goto doESCkey;
#endif
		nếu (curbuf->b_u_synced
			|| (bufref_valid(&save_curbuf)
			    && curbuf == save_curbuf.br_buf
			    && tick != CHANGEDTICK(curbuf)))
		{
		    // The command synced undo or changed this buffer.
		    // Save the cursor line before the next typed edit.
		    ins_need_undo = TRUE;
		}
	    }
	    break;

	case K_CURSORHOLD:	// Didn't type something for a while.
	    ins_apply_autocmds(EVENT_CURSORHOLDI);
	    did_cursorhold = TRUE;
	    // If CTRL-G U was used apply it to the next typed key.
	    nếu (dont_sync_undo == TRUE)
		dont_sync_undo = MAYBE;
	    break;

	case K_COMPLETE_DELAY:	// 'autocompletedelay' expired
	    // If CTRL-G U was used apply it to the next typed key.
	    nếu (dont_sync_undo == TRUE)
		dont_sync_undo = MAYBE;
	    ins_compl_clear_autocomplete_delay();
	    nếu (!ins_compl_has_autocomplete() || char_avail()
		    || curwin->w_cursor.col == 0)
		break;
	    c = char_before_cursor();
	    nếu (!vim_isprintc(c))
		break;
	    // The completion may have been cleared while waiting, so re-enable
	    // autocomplete to match a zero delay.
	    ins_compl_enable_autocomplete();
	    ins_compl_arm_autostart();
	    goto docomplete;

#ifdef FEAT_GUI_MSWIN
	    // On MS-Windows ignore <M-F4>, we get it when closing the window
	    // was cancelled.
	case K_F4:
	    nếu (mod_mask != MOD_MASK_ALT)
		goto normalchar;
	    break;
#endif

#ifdef FEAT_GUI
	case K_VER_SCROLLBAR:
	    ins_scroll();
	    break;

	case K_HOR_SCROLLBAR:
	    ins_horscroll();
	    break;
#endif

	case K_HOME:	// <Home>
	case K_KHOME:
	case K_S_HOME:
	case K_C_HOME:
	    ins_home(c);
	    break;

	case K_END:	// <End>
	case K_KEND:
	case K_S_END:
	case K_C_END:
	    ins_end(c);
	    break;

	case K_LEFT:	// <Left>
	    nếu (mod_mask & (MOD_MASK_SHIFT|MOD_MASK_CTRL))
		ins_s_left();
	    không_thì
		ins_left();
	    break;

	case K_S_LEFT:	// <S-Left>
	case K_C_LEFT:
	    ins_s_left();
	    break;

	case K_RIGHT:	// <Right>
	    nếu (mod_mask & (MOD_MASK_SHIFT|MOD_MASK_CTRL))
		ins_s_right();
	    không_thì
		ins_right();
	    break;

	case K_S_RIGHT:	// <S-Right>
	case K_C_RIGHT:
	    ins_s_right();
	    break;

	case K_UP:	// <Up>
	    nếu (pum_visible())
		goto docomplete;
	    nếu (mod_mask & MOD_MASK_SHIFT)
		ins_pageup();
	    không_thì
		ins_up(FALSE);
	    break;

	case K_S_UP:	// <S-Up>
	case K_PAGEUP:
	case K_KPAGEUP:
	    nếu (pum_visible())
	    {
#ifdef FEAT_PROP_POPUP
		// CTRL-SHIFT-<Up> scrolls the info popup up a line,
		// CTRL-SHIFT-<PageUp> a page.  Shift is folded into K_S_UP but
		// stays in mod_mask for PageUp, hence the asymmetric check.
		nếu (c == K_S_UP ? (mod_mask & MOD_MASK_CTRL)
			: ((mod_mask & MOD_MASK_CTRL)
			    && (mod_mask & MOD_MASK_SHIFT)))
		{
		    popup_scroll_info(-1, c != K_S_UP);
		    break;
		}
#endif
		goto docomplete;
	    }
	    ins_pageup();
	    break;

	case K_DOWN:	// <Down>
	    nếu (pum_visible())
		goto docomplete;
	    nếu (mod_mask & MOD_MASK_SHIFT)
		ins_pagedown();
	    không_thì
		ins_down(FALSE);
	    break;

	case K_S_DOWN:	// <S-Down>
	case K_PAGEDOWN:
	case K_KPAGEDOWN:
	    nếu (pum_visible())
	    {
#ifdef FEAT_PROP_POPUP
		// CTRL-SHIFT-<Down>/<PageDown> scroll the info popup down.
		nếu (c == K_S_DOWN ? (mod_mask & MOD_MASK_CTRL)
			: ((mod_mask & MOD_MASK_CTRL)
			    && (mod_mask & MOD_MASK_SHIFT)))
		{
		    popup_scroll_info(1, c != K_S_DOWN);
		    break;
		}
#endif
		goto docomplete;
	    }
	    ins_pagedown();
	    break;

#ifdef FEAT_DND
	case K_DROP:	// drag-n-drop event
	    ins_drop();
	    break;
#endif

	case K_S_TAB:	// When not mapped, use like a normal TAB
	    c = TAB;
	    // FALLTHROUGH

	case TAB:	// TAB or Complete patterns along path
#nếu defined(FEAT_FIND_ID)
	    nếu (ctrl_x_mode_path_patterns())
		goto docomplete;
#endif
	    inserted_space = FALSE;
	    nếu (ins_tab())
		goto normalchar;	// insert TAB as a normal char
	    auto_format(FALSE, TRUE);
	    break;

	case K_KENTER:	// <Enter>
	    c = CAR;
	    // FALLTHROUGH
	case CAR:
	case NL:
#nếu defined(FEAT_QUICKFIX)
	    // In a quickfix window a <CR> jumps to the error under the
	    // cursor.
	    nếu (bt_quickfix(curbuf) && c == CAR)
	    {
		nếu (curwin->w_llist_ref == NULL)    // quickfix window
		    do_cmdline_cmd((char_u *)".cc");
		không_thì				    // location list window
		    do_cmdline_cmd((char_u *)".ll");
		break;
	    }
#endif
	    nếu (cmdwin_type != 0)
	    {
		// Execute the command in the cmdline window.
		cmdwin_result = CAR;
		goto doESCkey;
	    }
#ifdef FEAT_JOB_CHANNEL
	    nếu (bt_prompt(curbuf))
	    {
		invoke_prompt_callback();
		nếu (!bt_prompt(curbuf))
		    // buffer changed to a non-prompt buffer, get out of
		    // Insert mode
		    goto doESCkey;
		break;
	    }
#endif
	    nếu (ins_eol(c) == FAIL && !p_im)
		goto doESCkey;	    // out of memory
	    auto_format(FALSE, FALSE);
	    inserted_space = FALSE;
	    break;

	case Ctrl_K:	    // digraph or keyword completion
	    nếu (ctrl_x_mode_dictionary())
	    {
		nếu (has_compl_option(TRUE))
		    goto docomplete;
		break;
	    }
#ifdef FEAT_DIGRAPHS
	    c = ins_digraph();
	    nếu (c == NUL)
		break;
#endif
	    goto normalchar;

	case Ctrl_X:	// Enter CTRL-X mode
	    ins_ctrl_x();
	    break;

	case Ctrl_RSB:	// Tag name completion after ^X
	    nếu (!ctrl_x_mode_tags())
		goto normalchar;
	    goto docomplete;

	case Ctrl_F:	// File name completion after ^X
	    nếu (!ctrl_x_mode_files())
		goto normalchar;
	    goto docomplete;

	case 's':	// Spelling completion after ^X
	case Ctrl_S:
	    nếu (!ctrl_x_mode_spell())
		goto normalchar;
	    goto docomplete;

	case Ctrl_L:	// Whole line completion after ^X
	    nếu (!ctrl_x_mode_whole_line())
	    {
		// CTRL-L with 'insertmode' set: Leave Insert mode
		nếu (p_im)
		{
		    nếu (echeck_abbr(Ctrl_L + ABBR_OFF))
			break;
		    goto doESCkey;
		}
		goto normalchar;
	    }
	    // FALLTHROUGH

	case Ctrl_P:	// Do previous/next pattern completion
	case Ctrl_N:
#ifdef FEAT_PROP_POPUP
	    // CTRL-SHIFT-P/N scroll the info popup one line.
	    nếu (pum_visible() && (mod_mask & MOD_MASK_SHIFT)
		    && (c == Ctrl_P || c == Ctrl_N))
	    {
		popup_scroll_info(c == Ctrl_P ? -1 : 1, false);
		break;
	    }
#endif
	    // if 'complete' is empty then plain ^P is no longer special,
	    // but it is under other ^X modes
	    nếu (*curbuf->b_p_cpt == NUL
		    && (ctrl_x_mode_normal() || ctrl_x_mode_whole_line())
		    && !compl_status_local())
		goto normalchar;

docomplete:
	    ins_compl_clear_autocomplete_delay();
	    compl_busy = TRUE;
#ifdef FEAT_FOLDING
	    disable_fold_update++;  // don't redraw folds here
#endif
	    nếu (ins_complete(c, TRUE) == FAIL)
		compl_status_clear();
#ifdef FEAT_FOLDING
	    disable_fold_update--;
#endif
	    compl_busy = FALSE;
	    can_si = may_do_si(); // allow smartindenting
	    break;

	case Ctrl_Y:	// copy from previous line or scroll down
	case Ctrl_E:	// copy from next line	   or scroll up
	    c = ins_ctrl_ey(c);
	    break;

	default:
#ifdef UNIX
	    nếu (c == intr_char)		// special interrupt char
		goto do_intr;
#endif

normalchar:
	    /*
	     * Insert a normal character.
	     */
#nếu defined(FEAT_EVAL)
	    nếu (!p_paste)
	    {
		// Trigger InsertCharPre.
		char_u *str = do_insert_char_pre(c);
		char_u *p;

		nếu (str != NULL)
		{
		    nếu (*str != NUL && stop_arrow() != FAIL)
		    {
			// Insert the new value of v:char literally.
			cho (p = str; *p != NUL; MB_PTR_ADV(p))
			{
			    c = PTR2CHAR(p);
			    nếu (c == CAR || c == K_KENTER || c == NL)
				ins_eol(c);
			    không_thì
				ins_char(c);
			}
			AppendToRedobuffLit(str, -1);
		    }
		    vim_free(str);
		    c = NUL;
		}

		// If the new value is already inserted or an empty string
		// then don't insert any character.
		nếu (c == NUL)
		    break;
	    }
#endif
	    // Try to perform smart-indenting.
	    ins_try_si(c);

	    nếu (c == ' ')
	    {
		inserted_space = TRUE;
		nếu (inindent(0))
		    can_cindent = FALSE;
		nếu (Insstart_blank_vcol == MAXCOL
			&& curwin->w_cursor.lnum == Insstart.lnum)
		    Insstart_blank_vcol = get_nolist_virtcol();
	    }

	    // Insert a normal character and check for abbreviations on a
	    // special character.  Let CTRL-] expand abbreviations without
	    // inserting it.
	    nếu (vim_iswordc(c) || (!echeck_abbr(
			// Add ABBR_OFF for characters above 0x100, this is
			// what check_abbr() expects.
				(has_mbyte && c >= 0x100) ? (c + ABBR_OFF) : c)
			&& c != Ctrl_RSB))
	    {
		insert_special(c, FALSE, FALSE);
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
	    nếu (ins_compl_has_autocomplete() && !char_avail()
		    && vim_isprintc(c))
		TRIGGER_AUTOCOMPLETE();

	    break;
	}   // end of switch (c)

	// If typed something may trigger CursorHoldI again; K_COMPLETE_DELAY is
	// injected, not typed.
	nếu (c != K_CURSORHOLD && c != K_COMPLETE_DELAY
#ifdef FEAT_COMPL_FUNC
		// but not in CTRL-X mode, a script can't restore the state
		&& ctrl_x_mode_normal()
#endif
	       )
	    did_cursorhold = FALSE;

	// Check if we need to cancel completion mode because the window
	// or tab page was changed
	nếu (ins_compl_active() && !ins_compl_win_active(curwin))
	    ins_compl_cancel();

	// If the cursor was moved we didn't just insert a space
	nếu (arrow_used)
	    inserted_space = FALSE;

	nếu (can_cindent && cindent_on() && ctrl_x_mode_normal())
	{
force_cindent:
	    /*
	     * Indent now if a key was typed that is in 'cinkeys'.
	     */
	    nếu (in_cinkeys(c, ' ', line_is_white))
	    {
		nếu (stop_arrow() == OK)
		    // re-indent the current line
		    do_c_expr_indent();
	    }
	}

    }	// for (;;)
    // NOTREACHED
}

    số_nguyên
ins_need_undo_get(void)
{
    trả_về ins_need_undo;
}

/*
 * Redraw for Insert mode.
 * This is postponed until getting the next character to make '$' in the 'cpo'
 * option work correctly.
 * Only redraw when there are no characters available.  This speeds up
 * inserting sequences of characters (e.g., for CTRL-R).
 */
    void
ins_redraw(số_nguyên ready)	    // not busy with something
{
#ifdef FEAT_CONCEAL
    linenr_T	conceal_old_cursor_line = 0;
    linenr_T	conceal_new_cursor_line = 0;
    số_nguyên		conceal_update_lines = FALSE;
#endif

    nếu (char_avail())
	trả_về;

    // Trigger CursorMoved if the cursor moved.  Not when the popup menu is
    // visible, the command might delete it.
    nếu (ready && (has_cursormovedI()
#ifdef FEAT_PROP_POPUP
		|| popup_visible
#endif
#nếu defined(FEAT_CONCEAL)
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
	nếu (syntax_present(curwin) && must_redraw)
	    update_screen(0);
#endif
	nếu (has_cursormovedI())
	{
	    // Make sure curswant is correct, an autocommand may call
	    // getcurpos().
	    update_curswant();
	    ins_apply_autocmds(EVENT_CURSORMOVEDI);
	}
#ifdef FEAT_PROP_POPUP
	nếu (popup_visible)
	    popup_check_cursor_pos();
#endif
#ifdef FEAT_CONCEAL
	nếu (curwin->w_p_cole > 0)
	{
	    conceal_old_cursor_line = last_cursormoved.lnum;
	    conceal_new_cursor_line = curwin->w_cursor.lnum;
	    conceal_update_lines = TRUE;
	}
#endif
	last_cursormoved = curwin->w_cursor;
    }

    // Trigger TextChangedI if b_changedtick_i differs.
    nếu (ready && has_textchangedI()
	    && curbuf->b_last_changedtick_i != CHANGEDTICK(curbuf)
	    && !pum_visible())
    {
	aco_save_T	aco;
	varnumber_T	tick = CHANGEDTICK(curbuf);

	// Save and restore curwin and curbuf, in case the autocmd changes
	// them.
	aucmd_prepbuf(&aco, curbuf);
	apply_autocmds(EVENT_TEXTCHANGEDI, NULL, NULL, FALSE, curbuf);
	aucmd_restbuf(&aco);
	curbuf->b_last_changedtick_i = CHANGEDTICK(curbuf);
	nếu (tick != CHANGEDTICK(curbuf))  // see ins_apply_autocmds()
	    u_save(curwin->w_cursor.lnum,
					(linenr_T)(curwin->w_cursor.lnum + 1));
    }

    // Trigger TextChangedP if b_changedtick_pum differs. When the popupmenu
    // closes TextChangedI will need to trigger for backwards compatibility,
    // thus use different b_last_changedtick* variables.
    nếu (ready && has_textchangedP()
	    && curbuf->b_last_changedtick_pum != CHANGEDTICK(curbuf)
	    && pum_visible())
    {
	aco_save_T	aco;
	varnumber_T	tick = CHANGEDTICK(curbuf);

	// Save and restore curwin and curbuf, in case the autocmd changes
	// them.
	aucmd_prepbuf(&aco, curbuf);
	apply_autocmds(EVENT_TEXTCHANGEDP, NULL, NULL, FALSE, curbuf);
	aucmd_restbuf(&aco);
	curbuf->b_last_changedtick_pum = CHANGEDTICK(curbuf);
	nếu (tick != CHANGEDTICK(curbuf))  // see ins_apply_autocmds()
	    u_save(curwin->w_cursor.lnum,
					(linenr_T)(curwin->w_cursor.lnum + 1));
    }

    nếu (ready)
	may_trigger_win_scrolled_resized();

    // Trigger SafeState if nothing is pending.
    may_trigger_safestate(ready
	    && !ins_compl_active()
	    && !pum_visible());

#nếu defined(FEAT_CONCEAL)
    nếu ((conceal_update_lines
	    && (conceal_old_cursor_line != conceal_new_cursor_line
		|| conceal_cursor_line(curwin)))
	    || need_cursor_line_redraw)
    {
	nếu (conceal_old_cursor_line != conceal_new_cursor_line)
	    redrawWinline(curwin, conceal_old_cursor_line);
	redrawWinline(curwin, conceal_new_cursor_line == 0
			    ? curwin->w_cursor.lnum : conceal_new_cursor_line);
	curwin->w_valid &= ~VALID_CROW;
	need_cursor_line_redraw = FALSE;
    }
#endif
    nếu (must_redraw)
	update_screen(0);
    không_thì nếu (clear_cmdline || redraw_cmdline)
	showmode();		// clear cmdline and show mode
    showruler(FALSE);
    setcursor();
    emsg_on_display = FALSE;	// may remove error message now
}

/*
 * Handle a CTRL-V or CTRL-Q typed in Insert mode.
 */
    static void
ins_ctrl_v(void)
{
    số_nguyên		c;
    số_nguyên		did_putchar = FALSE;

    // may need to redraw when no more chars available now
    ins_redraw(FALSE);

    nếu (redrawing() && !char_avail())
    {
	edit_putchar('^', TRUE);
	did_putchar = TRUE;
    }
    AppendToRedobuff((char_u *)CTRL_V_STR);	// CTRL-V

    add_to_showcmd_c(Ctrl_V);

    // Do not change any modifyOtherKeys ESC sequence to a normal key for
    // CTRL-SHIFT-V.
    c = get_literal(mod_mask & MOD_MASK_SHIFT);
    nếu (did_putchar)
	// when the line fits in 'columns' the '^' is at the start of the next
	// line and will not removed by the redraw
	edit_unputchar();
    clear_showcmd();

    insert_special(c, FALSE, TRUE);
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
    static số_nguyên
decodeModifyOtherKeys(số_nguyên c)
{
    char_u  *p = typebuf.tb_buf + typebuf.tb_off;
    số_nguyên	    idx;
    số_nguyên	    form = 0;
    số_nguyên	    argidx = 0;
    số_nguyên	    arg[2] = {0, 0};

    // Recognize:
    // form 0: {lead}{key};{modifier}u
    // form 1: {lead}27;{modifier};{key}~
    nếu (typebuf.tb_len >= 4 && (c == CSI || (c == ESC && *p == '[')))
    {
	idx = (*p == '[');
	nếu (p[idx] == '2' && p[idx + 1] == '7' && p[idx + 2] == ';' &&
		kitty_protocol_state != KKPS_ENABLED)
	{
	    form = 1;
	    idx += 3;
	}
	trong_khi (idx < typebuf.tb_len && argidx < 2)
	{
	    nếu (p[idx] == ';')
		++argidx;
	    không_thì nếu (VIM_ISDIGIT(p[idx]))
		arg[argidx] = arg[argidx] * 10 + (p[idx] - '0');
	    không_thì
		break;
	    ++idx;
	}
	số_nguyên kitty_no_mods = argidx == 0 && kitty_protocol_state == KKPS_ENABLED;
	nếu (idx < typebuf.tb_len
		&& p[idx] == (form == 1 ? '~' : 'u')
		&& (argidx == 1 || kitty_no_mods))
	{
	    // Match, consume the code.
	    typebuf.tb_off += idx + 1;
	    typebuf.tb_len -= idx + 1;
#nếu defined(FEAT_CLIENTSERVER) || defined(FEAT_EVAL)
	    nếu (typebuf.tb_len == 0)
		typebuf_was_filled = FALSE;
#endif

	    mod_mask = kitty_no_mods ? 0 : decode_modifiers(arg[!form]);
	    c = merge_modifyOtherKeys(arg[form], &mod_mask);
	}
    }

    trả_về c;
}

/*
 * Put a character directly onto the screen.  It's not stored in a buffer.
 * Used while handling CTRL-K, CTRL-V, etc. in Insert mode.
 */
static số_nguyên  pc_status;
#define PC_STATUS_UNSET	0	// pc_bytes was not set
#define PC_STATUS_RIGHT	1	// right half of double-wide char
#define PC_STATUS_LEFT	2	// left half of double-wide char
#define PC_STATUS_SET	3	// pc_bytes was filled
static char_u pc_bytes[MB_MAXBYTES + 1]; // saved bytes
static số_nguyên  pc_attr;
static số_nguyên  pc_row;
static số_nguyên  pc_col;

    void
edit_putchar(số_nguyên c, số_nguyên highlight)
{
    số_nguyên	    attr;

    nếu (ScreenLines == NULL)
	trả_về;

    update_topline();	// just in case w_topline isn't valid
    validate_cursor();
    nếu (highlight)
	attr = HL_ATTR(HLF_8);
    không_thì
	attr = 0;
    pc_row = W_WINROW(curwin) + curwin->w_wrow;
    pc_col = curwin->w_wincol;
    pc_status = PC_STATUS_UNSET;
#ifdef FEAT_RIGHTLEFT
    nếu (curwin->w_p_rl)
    {
	pc_col += curwin->w_width - 1 - curwin->w_wcol;
	nếu (has_mbyte)
	{
	    số_nguyên fix_col = mb_fix_col(pc_col, pc_row);

	    nếu (fix_col != pc_col)
	    {
		screen_putchar(' ', pc_row, fix_col, attr);
		--curwin->w_wcol;
		pc_status = PC_STATUS_RIGHT;
	    }
	}
    }
    không_thì
#endif
    {
	pc_col += curwin->w_wcol;
	nếu (mb_lefthalve(pc_row, pc_col))
	    pc_status = PC_STATUS_LEFT;
    }

    // save the character to be able to put it back
    nếu (pc_status == PC_STATUS_UNSET)
    {
	screen_getbytes(pc_row, pc_col, pc_bytes, &pc_attr);
	pc_status = PC_STATUS_SET;
    }
    screen_putchar(c, pc_row, pc_col, attr);
}

#nếu defined(FEAT_JOB_CHANNEL)
/*
 * Set the insert start position for when using a prompt buffer.
 */
    void
set_insstart(linenr_T lnum, số_nguyên col)
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
    void
edit_unputchar(void)
{
    nếu (pc_status != PC_STATUS_UNSET && pc_row >= msg_scrolled)
    {
	nếu (pc_status == PC_STATUS_RIGHT)
	    ++curwin->w_wcol;
	nếu (pc_status == PC_STATUS_RIGHT || pc_status == PC_STATUS_LEFT)
	    redrawWinline(curwin, curwin->w_cursor.lnum);
	không_thì
	    screen_puts(pc_bytes, pc_row - msg_scrolled, pc_col, pc_attr);
    }
}

/*
 * Called when "$" is in 'cpoptions': display a '$' at the end of the changed
 * text.  Only works when cursor is in the line that changes.
 */
    void
display_dollar(colnr_T col_arg)
{
    colnr_T col = col_arg < 0 ? 0 : col_arg;
    colnr_T save_col;

    nếu (!redrawing())
	trả_về;

    cursor_off();
    save_col = curwin->w_cursor.col;
    curwin->w_cursor.col = col;
    nếu (has_mbyte)
    {
	char_u *p;

	// If on the last byte of a multi-byte move to the first byte.
	p = ml_get_curline();
	curwin->w_cursor.col -= (*mb_head_off)(p, p + col);
    }
    curs_columns(FALSE);	    // recompute w_wrow and w_wcol
    nếu (curwin->w_wcol < curwin->w_width)
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
    void
undisplay_dollar(void)
{
    nếu (dollar_vcol < 0)
	trả_về;

    dollar_vcol = -1;
    redrawWinline(curwin, curwin->w_cursor.lnum);
}

/*
 * Truncate the space at the end of a line.  This is to be used only in an
 * insert mode.  It handles fixing the replace stack for MODE_REPLACE and
 * MODE_VREPLACE modes.
 */
    void
truncate_spaces(char_u *line, size_t len)
{
    số_nguyên	    i;

    // find start of trailing white space
    cho (i = (số_nguyên)len - 1; i >= 0 && VIM_ISWHITE(line[i]); i--)
    {
	nếu (State & REPLACE_FLAG)
	    replace_join(0);	    // remove a NUL from the replace stack
    }
    line[i + 1] = NUL;
}

/*
 * Backspace the cursor until the given column.  Handles MODE_REPLACE and
 * MODE_VREPLACE modes correctly.  May also be used when not in insert mode at
 * all.  Will attempt not to go before "col" even when there is a composing
 * character.
 */
    void
backspace_until_column(số_nguyên col)
{
    trong_khi ((số_nguyên)curwin->w_cursor.col > col)
    {
	curwin->w_cursor.col--;
	nếu (State & REPLACE_FLAG)
	    replace_do_bs(col);
	không_thì nếu (!del_char_after_col(col))
	    break;
    }
}

/*
 * Like del_char(), but make sure not to go before column "limit_col".
 * Only matters when there are composing characters.
 * Return TRUE when something was deleted.
 */
    static số_nguyên
del_char_after_col(số_nguyên limit_col UNUSED)
{
    nếu (enc_utf8 && limit_col >= 0)
    {
	colnr_T ecol = curwin->w_cursor.col + 1;

	// Make sure the cursor is at the start of a character, but
	// skip forward again when going too far back because of a
	// composing character.
	mb_adjust_cursor();
	trong_khi (curwin->w_cursor.col < (colnr_T)limit_col)
	{
	    số_nguyên l = utf_ptr2len(ml_get_cursor());

	    nếu (l == 0)  // end of line
		break;
	    curwin->w_cursor.col += l;
	}
	nếu (*ml_get_cursor() == NUL || curwin->w_cursor.col == ecol)
	    trả_về FALSE;
	del_bytes((long)((số_nguyên)ecol - curwin->w_cursor.col), FALSE, TRUE);
    }
    không_thì
	(void)del_char(FALSE);
    trả_về TRUE;
}

/*
 * Next character is interpreted literally.
 * A one, two or three digit decimal number is interpreted as its byte value.
 * If one or two digits are entered, the next character is given to vungetc().
 * For Unicode a character > 255 may be returned.
 * If "noReduceKeys" is TRUE do not change any modifyOtherKeys ESC sequence
 * into a normal key, return ESC.
 */
    số_nguyên
get_literal(số_nguyên noReduceKeys)
{
    số_nguyên		cc;
    số_nguyên		nc;
    số_nguyên		i;
    số_nguyên		hex = FALSE;
    số_nguyên		octal = FALSE;
    số_nguyên		unicode = 0;

    nếu (got_int)
	trả_về Ctrl_C;

#ifdef FEAT_GUI
    /*
     * In GUI there is no point inserting the internal code for a special key.
     * It is more useful to insert the string "<KEY>" instead.	This would
     * probably be useful in a text window too, but it would not be
     * vi-compatible (maybe there should be an option for it?) -- webb
     */
    nếu (gui.in_use)
    {
	++allow_keys;
	nếu (noReduceKeys)
	    ++no_reduce_keys;
    }
#endif
#ifdef USE_ON_FLY_SCROLL
    dont_scroll = TRUE;		// disallow scrolling here
#endif
    ++no_mapping;		// don't map the next key hits
    cc = 0;
    i = 0;
    cho (;;)
    {
	nc = plain_vgetc();
	nếu ((nc == ESC || nc == CSI) && !noReduceKeys)
	    nc = decodeModifyOtherKeys(nc);

	nếu ((mod_mask & ~MOD_MASK_SHIFT) != 0)
	    // A character with non-Shift modifiers should not be a valid
	    // character for i_CTRL-V_digit.
	    break;

	nếu ((State & MODE_CMDLINE) == 0 && MB_BYTE2LEN_CHECK(nc) == 1)
	    add_to_showcmd(nc);
	nếu (nc == 'x' || nc == 'X')
	    hex = TRUE;
	không_thì nếu (nc == 'o' || nc == 'O')
	    octal = TRUE;
	không_thì nếu (nc == 'u' || nc == 'U')
	    unicode = nc;
	không_thì
	{
	    nếu (hex || unicode != 0)
	    {
		nếu (!vim_isxdigit(nc))
		    break;
		cc = cc * 16 + hex2nr(nc);
	    }
	    không_thì nếu (octal)
	    {
		nếu (nc < '0' || nc > '7')
		    break;
		cc = cc * 8 + nc - '0';
	    }
	    không_thì
	    {
		nếu (!VIM_ISDIGIT(nc))
		    break;
		cc = cc * 10 + nc - '0';
	    }

	    ++i;
	}

	nếu (cc > 255 && unicode == 0)
	    cc = 255;		// limit range to 0-255
	nc = 0;

	nếu (hex)		// hex: up to two chars
	{
	    nếu (i >= 2)
		break;
	}
	không_thì nếu (unicode)	// Unicode: up to four or eight chars
	{
	    nếu ((unicode == 'u' && i >= 4) || (unicode == 'U' && i >= 8))
		break;
	}
	không_thì nếu (i >= 3)	// decimal or octal: up to three chars
	    break;
    }
    nếu (i == 0)	    // no number entered
    {
	nếu (nc == K_ZERO)   // NUL is stored as NL
	{
	    cc = '\n';
	    nc = 0;
	}
	không_thì
	{
	    cc = nc;
	    nc = 0;
	}
    }

    nếu (cc == 0)	// NUL is stored as NL
	cc = '\n';
    nếu (enc_dbcs && (cc & 0xff) == 0)
	cc = '?';	// don't accept an illegal DBCS char, the NUL in the
			// second byte will cause trouble!

    --no_mapping;
#ifdef FEAT_GUI
    nếu (gui.in_use)
    {
	--allow_keys;
	nếu (noReduceKeys)
	    --no_reduce_keys;
    }
#endif
    nếu (nc)
    {
	vungetc(nc);
	// A character typed with i_CTRL-V_digit cannot have modifiers.
	mod_mask = 0;
    }
    got_int = FALSE;	    // CTRL-C typed after CTRL-V is not an interrupt
    trả_về cc;
}

/*
 * Insert character, taking care of special keys and mod_mask
 */
    static void
insert_special(
    số_nguyên	    c,
    số_nguyên	    allow_modmask,
    số_nguyên	    ctrlv)	    // c was typed after CTRL-V
{
    char_u  *p;
    số_nguyên	    len;

    /*
     * Special function key, translate into "<Key>". Up to the last '>' is
     * inserted with ins_str(), so as not to replace characters in replace
     * mode.
     * Only use mod_mask for special keys, to avoid things like <S-Space>,
     * unless 'allow_modmask' is TRUE.
     */
#nếu defined(MACOS_X) || defined(FEAT_GUI_GTK)
    // Command-key never produces a normal key
    nếu (mod_mask & MOD_MASK_CMD)
	allow_modmask = TRUE;
#endif
    nếu (IS_SPECIAL(c) || (mod_mask && allow_modmask))
    {
	p = get_special_key_name(c, mod_mask);
	len = (số_nguyên)STRLEN(p);
	c = p[len - 1];
	nếu (len > 2)
	{
	    nếu (stop_arrow() == FAIL)
		trả_về;
	    p[len - 1] = NUL;
	    ins_str(p, len - 1);
	    AppendToRedobuffLit(p, -1);
	    ctrlv = FALSE;
	}
    }
    nếu (stop_arrow() == OK)
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
    void
insertchar(
    số_nguyên		c,			// character to insert or NUL
    số_nguyên		flags,			// INSCHAR_FORMAT, etc.
    số_nguyên		second_indent)		// indent for second line if >= 0
{
    số_nguyên		textwidth;
    char_u	*p;
    số_nguyên		fo_ins_blank;
    số_nguyên		force_format = flags & INSCHAR_FORMAT;

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
    nếu (textwidth > 0
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
#nếu defined(FEAT_EVAL)
	số_nguyên     do_internal = TRUE;
	colnr_T virtcol = get_nolist_virtcol()
				  + char2cells(c != NUL ? c : gchar_cursor());

	nếu (*curbuf->b_p_fex != NUL && (flags & INSCHAR_NO_FEX) == 0
		&& (force_format || virtcol > (colnr_T)textwidth))
	{
	    do_internal = (fex_format(curwin->w_cursor.lnum, 1L, c) != 0);
	    // It may be required to save for undo again, e.g. when setline()
	    // was called.
	    ins_need_undo = TRUE;
	}
	nếu (do_internal)
#endif
	    internal_format(textwidth, second_indent, flags, c == NUL, c);
    }

    nếu (c == NUL)	    // only formatting was wanted
	trả_về;

    // Check whether this character should end a comment.
    nếu (did_ai && c == end_comment_pending)
    {
	char_u  *line;
	char_u	lead_end[COM_MAX_LEN];	    // end-comment string
	số_nguyên	middle_len, end_len;
	số_nguyên	i;

	/*
	 * Need to remove existing (middle) comment leader and insert end
	 * comment leader.  First, check what comment leader we can find.
	 */
	i = get_leader_len(line = ml_get_curline(), &p, FALSE, TRUE);
	nếu (i > 0 && vim_strchr(p, COM_MIDDLE) != NULL)	// Just checking
	{
	    // Skip middle-comment string
	    trong_khi (*p && p[-1] != ':')	// find end of middle flags
		++p;
	    middle_len = copy_option_part(&p, lead_end, COM_MAX_LEN, ",");
	    // Don't count trailing white space for middle_len
	    trong_khi (middle_len > 0 && VIM_ISWHITE(lead_end[middle_len - 1]))
		--middle_len;

	    // Find the end-comment string
	    trong_khi (*p && p[-1] != ':')	// find end of end flags
		++p;
	    end_len = copy_option_part(&p, lead_end, COM_MAX_LEN, ",");

	    // Skip white space before the cursor
	    i = curwin->w_cursor.col;
	    trong_khi (--i >= 0 && VIM_ISWHITE(line[i]))
		;
	    i++;

	    // Skip to before the middle leader
	    i -= middle_len;

	    // Check some expected things before we go on
	    nếu (i >= 0 && end_len > 0 && lead_end[end_len - 1] == end_comment_pending)
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

    nếu (       !ISSPECIAL(c)
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
#define INPUT_BUFLEN 100
	char_u		buf[INPUT_BUFLEN + 1];
	số_nguyên		i;
	colnr_T		virtcol = 0;

	buf[0] = c;
	i = 1;
	nếu (textwidth > 0)
	    virtcol = get_nolist_virtcol();
	/*
	 * Stop the string when:
	 * - no more chars available
	 * - finding a special character (command key)
	 * - buffer is full
	 * - running into the 'textwidth' boundary
	 * - need to check for abbreviation: A non-word char after a word-char
	 */
	trong_khi (	   (c = vpeekc()) != NUL
		&& !ISSPECIAL(c)
		&& (!has_mbyte || MB_BYTE2LEN_CHECK(c) == 1)
		&& i < INPUT_BUFLEN
		&& (textwidth == 0
		    || (virtcol += byte2cells(buf[i - 1])) < (colnr_T)textwidth)
		&& !(!no_abbr && !vim_iswordc(c) && vim_iswordc(buf[i - 1])))
	{
#ifdef FEAT_RIGHTLEFT
	    c = vgetc();
	    nếu (p_hkmap && KeyTyped)
		c = hkmap(c);		    // Hebrew mode mapping
	    buf[i++] = c;
#không_thì
	    buf[i++] = vgetc();
#endif
	}

#ifdef FEAT_DIGRAPHS
	do_digraph(-1);			// clear digraphs
	do_digraph(buf[i-1]);		// may be the start of a digraph
#endif
	buf[i] = NUL;
	ins_str(buf, i);
	nếu (flags & INSCHAR_CTRLV)
	{
	    redo_literal(*buf);
	    i = 1;
	}
	không_thì
	    i = 0;
	nếu (buf[i] != NUL)
	    AppendToRedobuffLit(buf + i, -1);
    }
    không_thì
    {
	số_nguyên		cc;

	nếu (has_mbyte && (cc = (*mb_char2len)(c)) > 1)
	{
	    char_u	buf[MB_MAXBYTES + 1];

	    (*mb_char2bytes)(c, buf);
	    buf[cc] = NUL;
	    ins_char_bytes(buf, cc);
	    AppendCharToRedobuff(c);
	}
	không_thì
	{
	    ins_char(c);
	    nếu (flags & INSCHAR_CTRLV)
		redo_literal(c);
	    không_thì
		AppendCharToRedobuff(c);
	}
    }
}

/*
 * Put a character in the redo buffer, for when just after a CTRL-V.
 */
    static void
redo_literal(số_nguyên c)
{
    char_u	buf[10];

    // Only digits need special treatment.  Translate them into a string of
    // three digits.
    nếu (VIM_ISDIGIT(c))
    {
	vim_snprintf((char *)buf, sizeof(buf), "%03d", c);
	AppendToRedobuff(buf);
    }
    không_thì
	AppendCharToRedobuff(c);
}

/*
 * start_arrow() is called when an arrow key is used in insert mode.
 * For undo/redo it resembles hitting the <ESC> key.
 */
    void
start_arrow(
    pos_T    *end_insert_pos)		// can be NULL
{
    start_arrow_common(end_insert_pos, TRUE);
}

/*
 * Like start_arrow() but with end_change argument.
 * Will prepare for redo of CTRL-G U if "end_change" is FALSE.
 */
    static void
start_arrow_with_change(
    pos_T    *end_insert_pos,		// can be NULL
    số_nguyên	      end_change)		// end undoable change
{
    start_arrow_common(end_insert_pos, end_change);
    nếu (!end_change)
    {
	AppendCharToRedobuff(Ctrl_G);
	AppendCharToRedobuff('U');
    }
}

    static void
start_arrow_common(
    pos_T    *end_insert_pos,		// can be NULL
    số_nguyên	      end_change)		// end undoable change
{
    nếu (!arrow_used && end_change)	// something has been inserted
    {
	AppendToRedobuff(ESC_STR);
	stop_insert(end_insert_pos, FALSE, FALSE);
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
    static void
check_spell_redraw(void)
{
    nếu (spell_redraw_lnum != 0)
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
    số_nguyên
stop_arrow(void)
{
    nếu (arrow_used)
    {
	Insstart = curwin->w_cursor;	// new insertion starts here
	nếu (Insstart.col > Insstart_orig.col && !ins_need_undo)
	    // Don't update the original insert position when moved to the
	    // right, except when nothing was inserted yet.
	    update_Insstart_orig = FALSE;
	Insstart_textlen = (colnr_T)linetabsize_str(ml_get_curline());

	nếu (u_save_cursor() == OK)
	{
	    arrow_used = FALSE;
	    ins_need_undo = FALSE;
	}

	ai_col = 0;
	nếu (State & VREPLACE_FLAG)
	{
	    orig_line_count = curbuf->b_ml.ml_line_count;
	    vr_lines_changed = 1;
	}
	ResetRedobuff();
	AppendToRedobuff((char_u *)"1i");   // pretend we start an insertion
	new_insert_skip = 2;
    }
    không_thì nếu (ins_need_undo)
    {
	nếu (u_save_cursor() == OK)
	{
	    // A command or event may have moved the cursor before the next
	    // edit. Pull Insstart back only when the cursor moved above it,
	    // so that later edits can properly decide whether an extra undo
	    // entry is needed. Advancing Insstart would mis-place '[ after a
	    // register paste.
	    nếu (LT_POS(curwin->w_cursor, Insstart))
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

    trả_về (arrow_used || ins_need_undo ? FAIL : OK);
}

/*
 * Do a few things to stop inserting.
 * "end_insert_pos" is where insert ended.  It is NULL when we already jumped
 * to another window/buffer.
 */
    static void
stop_insert(
    pos_T	*end_insert_pos,
    số_nguyên		esc,			// called by ins_esc()
    số_nguyên		nomove)			// <c-\><c-o>, don't move cursor
{
    số_nguyên		cc;
    string_T	inserted;

    stop_redo_ins();
    replace_flush();		// abandon replace stack

    /*
     * Save the inserted text for later redo with ^@ and CTRL-A.
     * Don't do it when "restart_edit" was set and nothing was inserted,
     * otherwise CTRL-O w and then <Left> will clear "last_insert".
     */
    inserted = get_inserted();
    số_nguyên added = inserted.string == NULL ? 0 : (số_nguyên)inserted.length - new_insert_skip;
    nếu (did_restart_edit == 0 || added > 0)
    {
	vim_free(last_insert.string);
	last_insert = inserted;			    // structure copy
	last_insert_skip = added < 0 ? 0 : new_insert_skip;
    }
    không_thì
	vim_free(inserted.string);

    nếu (!arrow_used && end_insert_pos != NULL)
    {
	// Auto-format now.  It may seem strange to do this when stopping an
	// insertion (or moving the cursor), but it's required when appending
	// a line and having it end in a space.  But only do it when something
	// was actually inserted, otherwise undo won't work.
	nếu (!ins_need_undo && has_format_option(FO_AUTO))
	{
	    pos_T   tpos = curwin->w_cursor;

	    // When the cursor is at the end of the line after a space the
	    // formatting will move it to the following word.  Avoid that by
	    // moving the cursor onto the space.
	    cc = 'x';
	    nếu (curwin->w_cursor.col > 0 && gchar_cursor() == NUL)
	    {
		dec_cursor();
		cc = gchar_cursor();
		nếu (!VIM_ISWHITE(cc))
		    curwin->w_cursor = tpos;
	    }

	    auto_format(TRUE, FALSE);

	    nếu (VIM_ISWHITE(cc))
	    {
		nếu (gchar_cursor() != NUL)
		    inc_cursor();
		// If the cursor is still at the same character, also keep
		// the "coladd".
		nếu (gchar_cursor() == NUL
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
	nếu (!nomove && did_ai && (esc || (vim_strchr(p_cpo, CPO_INDENT) == NULL
			&& curwin->w_cursor.lnum != end_insert_pos->lnum))
		&& end_insert_pos->lnum <= curbuf->b_ml.ml_line_count)
	{
	    pos_T	tpos = curwin->w_cursor;
	    colnr_T	prev_col = end_insert_pos->col;
	    colnr_T	strip_col;

	    curwin->w_cursor = *end_insert_pos;
	    check_cursor_col();  // make sure it is not past the line

	    // Where the loop would actually start (back up if on NUL).
	    strip_col = curwin->w_cursor.col;
	    nếu (gchar_cursor() == NUL && strip_col > 0)
		--strip_col;

	    // Don't strip if non-whitespace follows: setline() from a <Cmd>
	    // mapping or CursorHoldI autocmd may have inserted content.
	    nếu (*skipwhite(ml_get_curline() + strip_col) == NUL)
	    {
		curwin->w_cursor.col = strip_col;
		cho (;;)
		{
		    nếu (gchar_cursor() == NUL && curwin->w_cursor.col > 0)
			--curwin->w_cursor.col;
		    cc = gchar_cursor();
		    nếu (!VIM_ISWHITE(cc))
			break;
		    nếu (del_char(TRUE) == FAIL)
			break;  // should not happen
		}
		nếu (curwin->w_cursor.lnum != tpos.lnum)
		    curwin->w_cursor = tpos;
		không_thì nếu (curwin->w_cursor.col < prev_col)
		{
		    // reset tpos, could have been invalidated in the loop above
		    tpos = curwin->w_cursor;
		    tpos.col++;
		    nếu (cc != NUL && gchar_pos(&tpos) == NUL)
			++curwin->w_cursor.col;	// put cursor back on the NUL
		}

		// <C-S-Right> may have started Visual mode, adjust the position for
		// deleted characters.
		nếu (VIsual_active)
		    check_visual_pos();
	    }
	    không_thì
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
    nếu (end_insert_pos != NULL)
    {
	curbuf->b_op_start = Insstart;
	curbuf->b_op_start_orig = Insstart_orig;
	curbuf->b_op_end = *end_insert_pos;
    }
}

/*
 * Set the last inserted text to a single character.
 * Used for the replace command.
 */
    void
set_last_insert(số_nguyên c)
{
    char_u	*s;

    vim_free(last_insert.string);
    last_insert.string = alloc(MB_MAXBYTES * 3 + 5);
    nếu (last_insert.string == NULL)
    {
	last_insert.length = 0;
	trả_về;
    }

    s = last_insert.string;
    // Use the CTRL-V only when entering a special char
    nếu (c < ' ' || c == DEL)
	*s++ = Ctrl_V;
    s = add_char2buf(c, s);
    *s++ = ESC;
    *s = NUL;
    last_insert.length = (size_t)(s - last_insert.string);
    last_insert_skip = 0;
}

#nếu defined(EXITFREE)
    void
free_last_insert(void)
{
    VIM_CLEAR_STRING(last_insert);
}
#endif

/*
 * Add character "c" to buffer "s".  Escape the special meaning of K_SPECIAL
 * and CSI.  Handle multi-byte characters.
 * Returns a pointer to after the added bytes.
 */
    char_u *
add_char2buf(số_nguyên c, char_u *s)
{
    char_u	temp[MB_MAXBYTES + 1];
    số_nguyên		i;
    số_nguyên		len;

    len = (*mb_char2bytes)(c, temp);
    cho (i = 0; i < len; ++i)
    {
	c = temp[i];
	// Need to escape K_SPECIAL and CSI like in the typeahead buffer.
	nếu (c == K_SPECIAL)
	{
	    *s++ = K_SPECIAL;
	    *s++ = KS_SPECIAL;
	    *s++ = KE_FILLER;
	}
#ifdef FEAT_GUI
	không_thì nếu (c == CSI)
	{
	    *s++ = CSI;
	    *s++ = KS_EXTRA;
	    *s++ = (số_nguyên)KE_CSI;
	}
#endif
	không_thì
	    *s++ = c;
    }
    trả_về s;
}

/*
 * move cursor to start of line
 * if flags & BL_WHITE	move to first non-white
 * if flags & BL_SOL	move to first non-white if startofline is set,
 *			    otherwise keep "curswant" column
 * if flags & BL_FIX	don't leave the cursor on a NUL.
 */
    void
beginline(số_nguyên flags)
{
    nếu ((flags & BL_SOL) && !p_sol)
	coladvance(curwin->w_curswant);
    không_thì
    {
	curwin->w_cursor.col = 0;
	curwin->w_cursor.coladd = 0;

	nếu (flags & (BL_WHITE | BL_SOL))
	{
	    char_u  *ptr;

	    cho (ptr = ml_get_curline(); VIM_ISWHITE(*ptr)
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

    số_nguyên
oneright(void)
{
    char_u	*ptr;
    số_nguyên		l;

    nếu (virtual_active())
    {
	pos_T	prevpos = curwin->w_cursor;

	// Adjust for multi-wide char (excluding TAB)
	ptr = ml_get_cursor();
	coladvance(getviscol() + ((*ptr != TAB
					  && vim_isprintc((*mb_ptr2char)(ptr)))
		    ? ptr2cells(ptr) : 1));
	curwin->w_set_curswant = true;
	// Return OK if the cursor moved, FAIL otherwise (at window edge).
	trả_về (prevpos.col != curwin->w_cursor.col
		    || prevpos.coladd != curwin->w_cursor.coladd) ? OK : FAIL;
    }

    ptr = ml_get_cursor();
    nếu (*ptr == NUL)
	trả_về FAIL;	    // already at the very end

    nếu (has_mbyte)
	l = (*mb_ptr2len)(ptr);
    không_thì
	l = 1;

    // move "l" bytes right, but don't end up on the NUL, unless 'virtualedit'
    // contains "onemore".
    nếu (ptr[l] == NUL && (get_ve_flags() & VE_ONEMORE) == 0)
	trả_về FAIL;
    curwin->w_cursor.col += l;

    curwin->w_set_curswant = true;
    adjust_skipcol();
    trả_về OK;
}

    số_nguyên
oneleft(void)
{
    nếu (virtual_active())
    {
#ifdef FEAT_LINEBREAK
	số_nguyên width;
#endif
	số_nguyên v = getviscol();

	nếu (v == 0)
	    trả_về FAIL;

#ifdef FEAT_LINEBREAK
	// We might get stuck on 'showbreak', skip over it.
	width = 1;
	cho (;;)
	{
	    coladvance(v - width);
	    // getviscol() is slow, skip it when 'showbreak' is empty,
	    // 'breakindent' is not set and there are no multi-byte
	    // characters
	    nếu ((*get_showbreak_value(curwin) == NUL && !curwin->w_p_bri
					     && !has_mbyte) || getviscol() < v)
		break;
	    ++width;
	}
#không_thì
	coladvance(v - 1);
#endif

	nếu (curwin->w_cursor.coladd == 1)
	{
	    char_u *ptr;

	    // Adjust for multi-wide char (not a TAB)
	    ptr = ml_get_cursor();
	    nếu (*ptr != TAB && vim_isprintc((*mb_ptr2char)(ptr))
							 && ptr2cells(ptr) > 1)
		curwin->w_cursor.coladd = 0;
	}

	curwin->w_set_curswant = true;
	adjust_skipcol();
	trả_về OK;
    }

    nếu (curwin->w_cursor.col == 0)
	trả_về FAIL;

    curwin->w_set_curswant = true;
    --curwin->w_cursor.col;

    // if the character on the left of the current cursor is a multi-byte
    // character, move to its first byte
    nếu (has_mbyte)
	mb_adjust_cursor();
    adjust_skipcol();
    trả_về OK;
}

/*
 * Move the cursor up "n" lines in window "wp".
 * Takes care of closed folds.
 */
    void
cursor_up_inner(win_T *wp, long n)
{
    linenr_T	lnum = wp->w_cursor.lnum;

    nếu (n >= lnum)
	lnum = 1;
    không_thì
#ifdef FEAT_FOLDING
	nếu (hasAnyFolding(wp))
    {
	/*
	 * Count each sequence of folded lines as one logical line.
	 */
	// go to the start of the current fold
	(void)hasFoldingWin(wp, lnum, &lnum, NULL, TRUE, NULL);

	trong_khi (n--)
	{
	    // move up one line
	    --lnum;
	    nếu (lnum <= 1)
		break;
	    // If we entered a fold, move to the beginning, unless in
	    // Insert mode or when 'foldopen' contains "all": it will open
	    // in a moment.
	    nếu (n > 0 || !((State & MODE_INSERT) || (fdo_flags & FDO_ALL)))
		(void)hasFoldingWin(wp, lnum, &lnum, NULL, TRUE, NULL);
	}
	nếu (lnum < 1)
	    lnum = 1;
    }
    không_thì
#endif
	lnum -= n;

    wp->w_cursor.lnum = lnum;
}

    số_nguyên
cursor_up(
    long	n,
    số_nguyên		upd_topline)	    // When TRUE: update topline
{
    // This fails if the cursor is already in the first line or the count is
    // larger than the line number and '-' is in 'cpoptions'
    linenr_T lnum = curwin->w_cursor.lnum;
    nếu (n > 0 && (lnum <= 1
		       || (n >= lnum && vim_strchr(p_cpo, CPO_MINUS) != NULL)))
	trả_về FAIL;
    cursor_up_inner(curwin, n);

    // try to advance to the column we want to be at
    coladvance(curwin->w_curswant);

    nếu (upd_topline)
	update_topline();	// make sure curwin->w_topline is valid

    trả_về OK;
}

/*
 * Move the cursor down "n" lines in window "wp".
 * Takes care of closed folds.
 */
    void
cursor_down_inner(win_T *wp, long n)
{
    linenr_T	lnum = wp->w_cursor.lnum;
    linenr_T	line_count = wp->w_buffer->b_ml.ml_line_count;

    nếu (lnum + n >= line_count)
	lnum = line_count;
    không_thì
#ifdef FEAT_FOLDING
	nếu (hasAnyFolding(wp))
    {
	linenr_T	last;

	// count each sequence of folded lines as one logical line
	trong_khi (n--)
	{
	    nếu (hasFoldingWin(wp, lnum, NULL, &last, TRUE, NULL))
		lnum = last + 1;
	    không_thì
		++lnum;
	    nếu (lnum >= line_count)
		break;
	}
	nếu (lnum > line_count)
	    lnum = line_count;
    }
    không_thì
#endif
	lnum += n;

    wp->w_cursor.lnum = lnum;
}

/*
 * Cursor down a number of logical lines.
 */
    số_nguyên
cursor_down(
    long	n,
    số_nguyên		upd_topline)	    // When TRUE: update topline
{
    linenr_T	lnum = curwin->w_cursor.lnum;
    linenr_T	line_count = curwin->w_buffer->b_ml.ml_line_count;
    // This fails if the cursor is already in the last (folded) line, or would
    // move beyond the last line and '-' is in 'cpoptions'.
#ifdef FEAT_FOLDING
    hasFoldingWin(curwin, lnum, NULL, &lnum, TRUE, NULL);
#endif
    nếu (n > 0
	    && (lnum >= line_count
		|| (lnum + n > line_count
				     && vim_strchr(p_cpo, CPO_MINUS) != NULL)))
	trả_về FAIL;
    cursor_down_inner(curwin, n);

    // try to advance to the column we want to be at
    coladvance(curwin->w_curswant);

    nếu (upd_topline)
	update_topline();	// make sure curwin->w_topline is valid

    trả_về OK;
}

/*
 * Stuff the last inserted text in the read buffer.
 * Last_insert actually is a copy of the redo buffer, so we
 * first have to remove the command.
 */
    số_nguyên
stuff_inserted(
    số_nguyên	    c,		// Command character to be inserted
    long    count,	// Repeat this many times
    số_nguyên	    no_esc)	// Don't add an ESC at the end
{
    string_T	insert;				// text to be inserted
    char_u	last = ' ';

    insert = get_last_insert();
    nếu (insert.string == NULL)
    {
	emsg(_(e_no_inserted_text_yet));
	trả_về FAIL;
    }

    // may want to stuff the command character, to start Insert mode
    nếu (c != NUL)
	stuffcharReadbuff(c);

    nếu (insert.length > 0)
    {
	char_u	*p;

	// look for the last ESC in 'insert'
	cho (p = insert.string + insert.length - 1; p >= insert.string; --p)
	{
	    nếu (*p == ESC)
	    {
		insert.length = (size_t)(p - insert.string);
		break;
	    }
	}
    }

    nếu (insert.length > 0)
    {
	char_u	*p = insert.string + insert.length - 1;

	// when the last char is either "0" or "^" it will be quoted if no ESC
	// comes after it OR if it will insert more than once and "ptr"
	// starts with ^D.	-- Acevedo
	nếu ((*p == '0' || *p == '^')
		&& (no_esc || (*insert.string == Ctrl_D && count > 1)))
	{
	    last = *p;
	    --insert.length;
	}
    }

    do
    {
	stuffReadbuffLen(insert.string, (long)insert.length);
	// a trailing "0" is inserted as "<C-V>048", "^" as "<C-V>^"
	switch (last)
	{
	case '0':
#define TEXT_TO_INSERT "\026\060\064\070"
	    stuffReadbuffLen((char_u *)TEXT_TO_INSERT, STRLEN_LITERAL(TEXT_TO_INSERT));
#undef TEXT_TO_INSERT
	    break;

	case '^':
#define TEXT_TO_INSERT "\026^"
	    stuffReadbuffLen((char_u *)TEXT_TO_INSERT, STRLEN_LITERAL(TEXT_TO_INSERT));
#undef TEXT_TO_INSERT
	    break;

	default:
	    break;
	}
    }
    trong_khi (--count > 0);

    // may want to stuff a trailing ESC, to get out of Insert mode
    nếu (!no_esc)
	stuffcharReadbuff(ESC);

    trả_về OK;
}

    string_T
get_last_insert(void)
{
    string_T insert = {NULL, 0};

    nếu (last_insert.string != NULL)
    {
	insert.string = last_insert.string + last_insert_skip;
	insert.length = (size_t)(last_insert.length - last_insert_skip);
    }

    trả_về insert;
}

/*
 * Get last inserted string, and remove trailing <Esc>.
 * Returns pointer to allocated memory (must be freed) or NULL.
 */
    char_u *
get_last_insert_save(void)
{
    string_T	insert = get_last_insert();
    char_u	*s;

    nếu (insert.string == NULL)
	trả_về NULL;
    s = vim_strnsave(insert.string, insert.length);
    nếu (s == NULL)
	trả_về NULL;

    nếu (insert.length > 0 && s[insert.length - 1] == ESC)	// remove trailing ESC
	s[--insert.length] = NUL;
    trả_về s;
}

/*
 * Check the word in front of the cursor for an abbreviation.
 * Called when the non-id character "c" has been entered.
 * When an abbreviation is recognized it is removed from the text and
 * the replacement string is inserted in typebuf.tb_buf[], followed by "c".
 */
    static số_nguyên
echeck_abbr(số_nguyên c)
{
    // Don't check for abbreviation in paste mode, when disabled and just
    // after moving around with cursor keys.
    nếu (p_paste || no_abbr || arrow_used)
	trả_về FALSE;

    trả_về check_abbr(c, ml_get_curline(), curwin->w_cursor.col,
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

static char_u	*replace_stack = NULL;
static long	replace_stack_nr = 0;	    // next entry in replace stack
static long	replace_stack_len = 0;	    // max. number of entries

    void
replace_push(
    số_nguyên	    c)	    // character that is replaced (NUL is none)
{
    char_u  *p;

    nếu (replace_stack_nr < replace_offset)	// nothing to do
	trả_về;
    nếu (replace_stack_len <= replace_stack_nr)
    {
	replace_stack_len += 50;
	p = ALLOC_MULT(char_u, replace_stack_len);
	nếu (p == NULL)	    // out of memory
	{
	    replace_stack_len -= 50;
	    trả_về;
	}
	nếu (replace_stack != NULL)
	{
	    mch_memmove(p, replace_stack,
				 (size_t)(replace_stack_nr * sizeof(char_u)));
	    vim_free(replace_stack);
	}
	replace_stack = p;
    }
    p = replace_stack + replace_stack_nr - replace_offset;
    nếu (replace_offset)
	mch_memmove(p + 1, p, (size_t)(replace_offset * sizeof(char_u)));
    *p = c;
    ++replace_stack_nr;
}

/*
 * Push a character onto the replace stack.  Handles a multi-byte character in
 * reverse byte order, so that the first byte is popped off first.
 * Return the number of bytes done (includes composing characters).
 */
    số_nguyên
replace_push_mb(char_u *p)
{
    số_nguyên l = (*mb_ptr2len)(p);
    số_nguyên j;

    cho (j = l - 1; j >= 0; --j)
	replace_push(p[j]);
    trả_về l;
}

/*
 * Pop one item from the replace stack.
 * return -1 if stack empty
 * return replaced character or NUL otherwise
 */
    static số_nguyên
replace_pop(void)
{
    nếu (replace_stack_nr == 0)
	trả_về -1;
    trả_về (số_nguyên)replace_stack[--replace_stack_nr];
}

/*
 * Join the top two items on the replace stack.  This removes to "off"'th NUL
 * encountered.
 */
    void
replace_join(
    số_nguyên	    off)	// offset for which NUL to remove
{
    số_nguyên	    i;

    cho (i = replace_stack_nr; --i >= 0; )
	nếu (replace_stack[i] == NUL && off-- <= 0)
	{
	    --replace_stack_nr;
	    mch_memmove(replace_stack + i, replace_stack + i + 1,
					      (size_t)(replace_stack_nr - i));
	    trả_về;
	}
}

/*
 * Pop bytes from the replace stack until a NUL is found, and insert them
 * before the cursor.  Can only be used in MODE_REPLACE or MODE_VREPLACE state.
 */
    static void
replace_pop_ins(void)
{
    số_nguyên	    cc;
    số_nguyên	    oldState = State;

    State = MODE_NORMAL;			// don't want MODE_REPLACE here
    trong_khi ((cc = replace_pop()) > 0)
    {
	mb_replace_pop_ins(cc);
	dec_cursor();
    }
    State = oldState;
}

/*
 * Insert bytes popped from the replace stack. "cc" is the first byte.  If it
 * indicates a multi-byte char, pop the other bytes too.
 */
    static void
mb_replace_pop_ins(số_nguyên cc)
{
    số_nguyên		n;
    char_u	buf[MB_MAXBYTES + 1];
    số_nguyên		i;
    số_nguyên		c;

    nếu (has_mbyte && (n = MB_BYTE2LEN(cc)) > 1)
    {
	buf[0] = cc;
	cho (i = 1; i < n; ++i)
	    buf[i] = replace_pop();
	ins_bytes_len(buf, n);
    }
    không_thì
	ins_char(cc);

    nếu (enc_utf8)
	// Handle composing chars.
	cho (;;)
	{
	    c = replace_pop();
	    nếu (c == -1)	    // stack empty
		break;
	    nếu ((n = MB_BYTE2LEN(c)) == 1)
	    {
		// Not a multi-byte char, put it back.
		replace_push(c);
		break;
	    }

	    buf[0] = c;
	    cho (i = 1; i < n; ++i)
		buf[i] = replace_pop();
	    nếu (utf_iscomposing(utf_ptr2char(buf)))
		ins_bytes_len(buf, n);
	    không_thì
	    {
		// Not a composing char, put it back.
		cho (i = n - 1; i >= 0; --i)
		    replace_push(buf[i]);
		break;
	    }

	}
}

/*
 * make the replace stack empty
 * (called when exiting replace mode)
 */
    static void
replace_flush(void)
{
    VIM_CLEAR(replace_stack);
    replace_stack_len = 0;
    replace_stack_nr = 0;
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
    static void
replace_do_bs(số_nguyên limit_col)
{
    số_nguyên		cc;
    số_nguyên		orig_len = 0;
    số_nguyên		ins_len;
    số_nguyên		orig_vcols = 0;
    colnr_T	start_vcol;
    char_u	*p;
    số_nguyên		i;
    số_nguyên		vcol;

    cc = replace_pop();
    nếu (cc > 0)
    {
#ifdef FEAT_PROP_POPUP
	size_t	len_before = 0;  // init to shut up GCC

	nếu (curbuf->b_has_textprop)
	{
	    // Do not adjust text properties for individual delete and insert
	    // operations, do it afterwards on the resulting text.
	    len_before = ml_get_curline_len();
	    ++text_prop_frozen;
	}
#endif
	nếu (State & VREPLACE_FLAG)
	{
	    // Get the number of screen cells used by the character we are
	    // going to delete.
	    getvcol(curwin, &curwin->w_cursor, NULL, &start_vcol, NULL, 0);
	    orig_vcols = chartabsize(ml_get_cursor(), start_vcol);
	}
	nếu (has_mbyte)
	{
	    (void)del_char_after_col(limit_col);
	    nếu (State & VREPLACE_FLAG)
		orig_len = ml_get_cursor_len();
	    replace_push(cc);
	}
	không_thì
	{
	    pchar_cursor(cc);
	    nếu (State & VREPLACE_FLAG)
		orig_len = ml_get_cursor_len() - 1;
	}
	replace_pop_ins();

	nếu (State & VREPLACE_FLAG)
	{
	    // Get the number of screen cells used by the inserted characters
	    p = ml_get_cursor();
	    ins_len = ml_get_cursor_len() - orig_len;
	    vcol = start_vcol;
	    cho (i = 0; i < ins_len; ++i)
	    {
		vcol += chartabsize(p + i, vcol);
		i += (*mb_ptr2len)(p + i) - 1;
	    }
	    vcol -= start_vcol;

	    // Delete spaces that were inserted after the cursor to keep the
	    // text aligned.
	    curwin->w_cursor.col += ins_len;
	    trong_khi (vcol > orig_vcols && gchar_cursor() == ' ')
	    {
		del_char(FALSE);
		++orig_vcols;
	    }
	    curwin->w_cursor.col -= ins_len;
	}

	// mark the buffer as changed and prepare for displaying
	changed_bytes(curwin->w_cursor.lnum, curwin->w_cursor.col);

#ifdef FEAT_PROP_POPUP
	nếu (curbuf->b_has_textprop)
	{
	    size_t len_now = ml_get_curline_len();

	    --text_prop_frozen;
	    adjust_prop_columns(curwin->w_cursor.lnum, curwin->w_cursor.col,
					   (số_nguyên)(len_now - len_before), 0);
	}
#endif
    }
    không_thì nếu (cc == 0)
	(void)del_char_after_col(limit_col);
}

#nếu defined(FEAT_RIGHTLEFT)
/*
 * Map Hebrew keyboard when in hkmap mode.
 */
    số_nguyên
hkmap(số_nguyên c)
{
    nếu (p_hkmapp)   // phonetic mapping, by Ilya Dogolazky
    {
	enum {hALEF=0, BET, GIMEL, DALET, HEI, VAV, ZAIN, HET, TET, IUD,
	    KAFsofit, hKAF, LAMED, MEMsofit, MEM, NUNsofit, NUN, SAMEH, AIN,
	    PEIsofit, PEI, ZADIsofit, ZADI, KOF, RESH, hSHIN, TAV};
	static char_u map[26] =
	    {(char_u)hALEF/*a*/, (char_u)BET  /*b*/, (char_u)hKAF    /*c*/,
	     (char_u)DALET/*d*/, (char_u)-1   /*e*/, (char_u)PEIsofit/*f*/,
	     (char_u)GIMEL/*g*/, (char_u)HEI  /*h*/, (char_u)IUD     /*i*/,
	     (char_u)HET  /*j*/, (char_u)KOF  /*k*/, (char_u)LAMED   /*l*/,
	     (char_u)MEM  /*m*/, (char_u)NUN  /*n*/, (char_u)SAMEH   /*o*/,
	     (char_u)PEI  /*p*/, (char_u)-1   /*q*/, (char_u)RESH    /*r*/,
	     (char_u)ZAIN /*s*/, (char_u)TAV  /*t*/, (char_u)TET     /*u*/,
	     (char_u)VAV  /*v*/, (char_u)hSHIN/*w*/, (char_u)-1      /*x*/,
	     (char_u)AIN  /*y*/, (char_u)ZADI /*z*/};

	nếu (c == 'N' || c == 'M' || c == 'P' || c == 'C' || c == 'Z')
	    trả_về (số_nguyên)(map[CharOrd(c)] - 1 + p_aleph);
							    // '-1'='sofit'
	không_thì nếu (c == 'x')
	    trả_về 'X';
	không_thì nếu (c == 'q')
	    trả_về '\''; // {geresh}={'}
	không_thì nếu (c == 246)
	    trả_về ' ';  // \"o --> ' ' for a german keyboard
	không_thì nếu (c == 228)
	    trả_về ' ';  // \"a --> ' '      -- / --
	không_thì nếu (c == 252)
	    trả_về ' ';  // \"u --> ' '      -- / --
	// NOTE: islower() does not do the right thing for us on Linux so we
	// do this the same was as 5.7 and previous, so it works correctly on
	// all systems.  Specifically, the e.g. Delete and Arrow keys are
	// munged and won't work if e.g. searching for Hebrew text.
	không_thì nếu (c >= 'a' && c <= 'z')
	    trả_về (số_nguyên)(map[CharOrdLow(c)] + p_aleph);
	không_thì
	    trả_về c;
    }
    không_thì
    {
	switch (c)
	{
	    case '`':	trả_về ';';
	    case '/':	trả_về '.';
	    case '\'':	trả_về ',';
	    case 'q':	trả_về '/';
	    case 'w':	trả_về '\'';

			// Hebrew letters - set offset from 'a'
	    case ',':	c = '{'; break;
	    case '.':	c = 'v'; break;
	    case ';':	c = 't'; break;
	    default: {
			 static char str[] = "zqbcxlsjphmkwonu ydafe rig";

			 nếu (c < 'a' || c > 'z')
			     trả_về c;
			 c = str[CharOrdLow(c)];
			 break;
		     }
	}

	trả_về (số_nguyên)(CharOrdLow(c) + p_aleph);
    }
}
#endif

    static void
ins_reg(void)
{
    số_nguyên		need_redraw = FALSE;
    số_nguyên		regname;
    số_nguyên		literally = 0;
    số_nguyên		vis_active = VIsual_active;

    /*
     * If we are going to wait for a character, show a '"'.
     */
    pc_status = PC_STATUS_UNSET;
    nếu (redrawing() && !char_avail())
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
    ++allow_keys;
    regname = plain_vgetc();
    LANGMAP_ADJUST(regname, TRUE);
    nếu (regname == Ctrl_R || regname == Ctrl_O || regname == Ctrl_P)
    {
	// Get a third key for literal register insertion
	literally = regname;
	add_to_showcmd_c(literally);
	regname = plain_vgetc();
	LANGMAP_ADJUST(regname, TRUE);
    }
    --no_mapping;
    --allow_keys;

#ifdef FEAT_EVAL
    // Don't call u_sync() while typing the expression or giving an error
    // message for it. Only call it explicitly.
    ++no_u_sync;
    nếu (regname == '=')
    {
	pos_T	curpos = curwin->w_cursor;
# ifdef HAVE_INPUT_METHOD
	số_nguyên	im_on = im_get_status();
# endif
	// Sync undo when evaluating the expression calls setline() or
	// append(), so that it can be undone separately.
	u_sync_once = 2;

	regname = get_expr_register();

	// Cursor may be moved back a column.
	curwin->w_cursor = curpos;
	check_cursor();
# ifdef HAVE_INPUT_METHOD
	// Restore the Input Method.
	nếu (im_on)
	    im_set_active(TRUE);
# endif
    }
    nếu (regname == NUL || !valid_yank_reg(regname, FALSE))
    {
	vim_beep(BO_REG);
	need_redraw = TRUE;	// remove the '"'
    }
    không_thì
    {
#endif
	nếu (literally == Ctrl_O || literally == Ctrl_P)
	{
	    // Append the command to the redo buffer.
	    AppendCharToRedobuff(Ctrl_R);
	    AppendCharToRedobuff(literally);
	    AppendCharToRedobuff(regname);

	    do_put(regname, NULL, BACKWARD, 1L,
		 (literally == Ctrl_P ? PUT_FIXINDENT : 0) | PUT_CURSEND);
	}
	không_thì nếu (insert_reg(regname, literally) == FAIL)
	{
	    vim_beep(BO_REG);
	    need_redraw = TRUE;	// remove the '"'
	}
	không_thì nếu (stop_insert_mode)
	    // When the '=' register was used and a function was invoked that
	    // did ":stopinsert" then stuff_empty() returns FALSE but we won't
	    // insert anything, need to remove the '"'
	    need_redraw = TRUE;

#ifdef FEAT_EVAL
    }
    --no_u_sync;
    nếu (u_sync_once == 1)
	ins_need_undo = TRUE;
    u_sync_once = 0;
#endif
    clear_showcmd();

    // If the inserted register is empty, we need to remove the '"'
    nếu (need_redraw || stuff_empty())
	edit_unputchar();

    // Disallow starting Visual mode here, would get a weird mode.
    nếu (!vis_active && VIsual_active)
	end_visual_mode();
}

/*
 * CTRL-G commands in Insert mode.
 */
    static void
ins_ctrl_g(void)
{
    số_nguyên		c;

    // Right after CTRL-X the cursor will be after the ruler.
    setcursor();

    /*
     * Don't map the second key. This also prevents the mode message to be
     * deleted when ESC is hit.
     */
    ++no_mapping;
    ++allow_keys;
    c = plain_vgetc();
    --no_mapping;
    --allow_keys;
    switch (c)
    {
	// CTRL-G k and CTRL-G <Up>: cursor up to Insstart.col
	case K_UP:
	case Ctrl_K:
	case 'k': ins_up(TRUE);
		  break;

	// CTRL-G j and CTRL-G <Down>: cursor down to Insstart.col
	case K_DOWN:
	case Ctrl_J:
	case 'j': ins_down(TRUE);
		  break;

	// CTRL-G u: start new undoable edit
	case 'u': u_sync(TRUE);
		  ins_need_undo = TRUE;

		  // Need to reset Insstart, esp. because a BS that joins
		  // a line to the previous one must save for undo.
		  update_Insstart_orig = FALSE;
		  Insstart = curwin->w_cursor;
		  break;

	// CTRL-G U: do not break undo with the next char
	case 'U':
		  // Allow one left/right cursor movement with the next char,
		  // without breaking undo.
		  dont_sync_undo = MAYBE;
		  break;

	case ESC:
		  // Esc after CTRL-G cancels it.
		  break;

	// Unknown CTRL-G command, reserved for future expansion.
	default:  vim_beep(BO_CTRLG);
    }
}

/*
 * CTRL-^ in Insert mode.
 */
    static void
ins_ctrl_hat(void)
{
    nếu (map_to_exists_mode((char_u *)"", MODE_LANGMAP, FALSE))
    {
	// ":lmap" mappings exists, Toggle use of ":lmap" mappings.
	nếu (State & MODE_LANGMAP)
	{
	    curbuf->b_p_iminsert = B_IMODE_NONE;
	    State &= ~MODE_LANGMAP;
	}
	không_thì
	{
	    curbuf->b_p_iminsert = B_IMODE_LMAP;
	    State |= MODE_LANGMAP;
#ifdef HAVE_INPUT_METHOD
	    im_set_active(FALSE);
#endif
	}
    }
#ifdef HAVE_INPUT_METHOD
    không_thì
    {
	// There are no ":lmap" mappings, toggle IM
	nếu (im_get_status())
	{
	    curbuf->b_p_iminsert = B_IMODE_NONE;
	    im_set_active(FALSE);
	}
	không_thì
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
    nếu (gui.in_use)
	gui_update_cursor(TRUE, FALSE);
#endif
#nếu defined(FEAT_KEYMAP)
    // Show/unshow value of 'keymap' in status lines.
    status_redraw_curbuf();
#endif
}

/*
 * Handle ESC in insert mode.
 * Returns TRUE when leaving insert mode, FALSE when going to repeat the
 * insert.
 */
    static số_nguyên
ins_esc(
    long	*count,
    số_nguyên		cmdchar,
    số_nguyên		nomove)	    // don't move cursor
{
    số_nguyên		temp;
    static số_nguyên	disabled_redraw = FALSE;
#ifdef FEAT_CONCEAL
    // Remember if the cursor line was concealed before changing State.
    số_nguyên		cursor_line_was_concealed = curwin->w_p_cole > 0
						&& conceal_cursor_line(curwin);
#endif

#ifdef FEAT_SPELL
    check_spell_redraw();
#endif

    temp = curwin->w_cursor.col;
    nếu (disabled_redraw)
    {
	nếu (RedrawingDisabled > 0)
	    --RedrawingDisabled;
	disabled_redraw = FALSE;
    }
    nếu (!arrow_used)
    {
	/*
	 * Don't append the ESC for "r<CR>" and "grx".
	 * When 'insertmode' is set only CTRL-L stops Insert mode.  Needed for
	 * when "count" is non-zero.
	 */
	nếu (cmdchar != 'r' && cmdchar != 'v')
	    AppendToRedobuff(p_im ? (char_u *)"\014" : ESC_STR);

	/*
	 * Repeating insert may take a long time.  Check for
	 * interrupt now and then.
	 */
	nếu (*count > 0)
	{
	    line_breakcheck();
	    nếu (got_int)
		*count = 0;
	}

	nếu (--*count > 0)	// repeat what was typed
	{
	    // Vi repeats the insert without replacing characters.
	    nếu (vim_strchr(p_cpo, CPO_REPLCNT) != NULL)
		State &= ~REPLACE_FLAG;

	    (void)start_redo_ins();
	    nếu (cmdchar == 'r' || cmdchar == 'v')
		stuffRedoReadbuff(ESC_STR);	// no ESC in redo buffer
	    ++RedrawingDisabled;
	    disabled_redraw = TRUE;
	    trả_về FALSE;	// repeat the insert
	}
	stop_insert(&curwin->w_cursor, TRUE, nomove);
	undisplay_dollar();
    }

    nếu (cmdchar != 'r' && cmdchar != 'v')
	ins_apply_autocmds(EVENT_INSERTLEAVEPRE);

    // When an autoindent was removed, curswant stays after the
    // indent
    nếu (restart_edit == NUL && (colnr_T)temp == curwin->w_cursor.col)
	curwin->w_set_curswant = true;

    // Remember the last Insert position in the '^ mark.
    nếu ((cmdmod.cmod_flags & CMOD_KEEPJUMPS) == 0)
	curbuf->b_last_insert = curwin->w_cursor;

    /*
     * The cursor should end up on the last inserted character.
     * Don't do it for CTRL-O, unless past the end of the line.
     */
    nếu (!nomove
	    && (curwin->w_cursor.col != 0
		|| curwin->w_cursor.coladd > 0)
	    && (restart_edit == NUL
		   || (gchar_cursor() == NUL && !VIsual_active))
#ifdef FEAT_RIGHTLEFT
	    && !revins_on
#endif
				      )
    {
	nếu (curwin->w_cursor.coladd > 0 || get_ve_flags() == VE_ALL)
	{
	    oneleft();
	    nếu (restart_edit != NUL)
		++curwin->w_cursor.coladd;
	}
	không_thì
	{
	    --curwin->w_cursor.col;
	    curwin->w_valid &= ~(VALID_WCOL|VALID_VIRTCOL);
	    // Correct cursor for multi-byte character.
	    nếu (has_mbyte)
		mb_adjust_cursor();
	}
    }

#ifdef HAVE_INPUT_METHOD
    // Disable IM to allow typing English directly for Normal mode commands.
    // When ":lmap" is enabled don't change 'iminsert' (IM can be enabled as
    // well).
    nếu (!(State & MODE_LANGMAP))
	im_save_status(&curbuf->b_p_iminsert);
    im_set_active(FALSE);
#endif

    State = MODE_NORMAL;
    may_trigger_modechanged();
    // need to position cursor again when on a TAB and when on a char with
    // virtual text.
    nếu (gchar_cursor() == TAB
#ifdef FEAT_PROP_POPUP
	    || curbuf->b_has_textprop
#endif
       )
	curwin->w_valid &= ~(VALID_WROW|VALID_WCOL|VALID_VIRTCOL);

    setmouse();
#ifdef CURSOR_SHAPE
    ui_cursor_shape();		// may show different cursor shape
#endif
    nếu (!p_ek)
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
    conceal_check_cursor_line(cursor_line_was_concealed);
#endif

    // When recording or for CTRL-O, need to display the new mode.
    // Otherwise remove the mode message.
    nếu (reg_recording != 0 || restart_edit != NUL)
	showmode();
    không_thì nếu (p_smd && (got_int || !skip_showmode()))
	msg("");

    trả_về TRUE;	    // exit Insert mode
}

#ifdef FEAT_RIGHTLEFT
/*
 * Toggle language: hkmap and revins_on.
 * Move to end of reverse inserted text.
 */
    static void
ins_ctrl_(void)
{
    nếu (revins_on && revins_chars && revins_scol >= 0)
    {
	trong_khi (gchar_cursor() != NUL && revins_chars--)
	    ++curwin->w_cursor.col;
    }
    p_ri = !p_ri;
    revins_on = (State == MODE_INSERT && p_ri);
    nếu (revins_on)
    {
	revins_scol = curwin->w_cursor.col;
	revins_legal++;
	revins_chars = 0;
	undisplay_dollar();
    }
    không_thì
	revins_scol = -1;
    p_hkmap = curwin->w_p_rl ^ p_ri;    // be consistent!
    showmode();
}
#endif

/*
 * If 'keymodel' contains "startsel", may start selection.
 * Returns TRUE when a CTRL-O and other keys stuffed.
 */
    static số_nguyên
ins_start_select(số_nguyên c)
{
    nếu (!km_startsel)
	trả_về FALSE;
    switch (c)
    {
	case K_KHOME:
	case K_KEND:
	case K_PAGEUP:
	case K_KPAGEUP:
	case K_PAGEDOWN:
	case K_KPAGEDOWN:
#ifdef MACOS_X
	case K_LEFT:
	case K_RIGHT:
	case K_UP:
	case K_DOWN:
	case K_END:
	case K_HOME:
#endif
	    nếu (!(mod_mask & MOD_MASK_SHIFT))
		break;
	    // FALLTHROUGH
	case K_S_LEFT:
	case K_S_RIGHT:
	case K_S_UP:
	case K_S_DOWN:
	case K_S_END:
	case K_S_HOME:
	    // Start selection right away, the cursor can move with CTRL-O when
	    // beyond the end of the line.
	    start_selection();

	    // Execute the key in (insert) Select mode.
	    stuffcharReadbuff(Ctrl_O);
	    nếu (mod_mask)
	    {
		char_u	    buf[4];

		buf[0] = K_SPECIAL;
		buf[1] = KS_MODIFIER;
		buf[2] = mod_mask;
		buf[3] = NUL;
		stuffReadbuffLen(buf, 3L);
	    }
	    stuffcharReadbuff(c);
	    trả_về TRUE;
    }
    trả_về FALSE;
}

/*
 * <Insert> key in Insert mode: toggle insert/replace mode.
 */
    static void
ins_insert(số_nguyên replaceState)
{
#ifdef FEAT_EVAL
    set_vim_var_string(VV_INSERTMODE,
		   (char_u *)((State & REPLACE_FLAG) ? "i"
			    : replaceState == MODE_VREPLACE ? "v" : "r"), 1);
#endif
    ins_apply_autocmds(EVENT_INSERTCHANGE);
    nếu (State & REPLACE_FLAG)
	State = MODE_INSERT | (State & MODE_LANGMAP);
    không_thì
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
    static void
ins_ctrl_o(void)
{
    restart_VIsual_select = 0;
    nếu (State & VREPLACE_FLAG)
	restart_edit = 'V';
    không_thì nếu (State & REPLACE_FLAG)
	restart_edit = 'R';
    không_thì
	restart_edit = 'I';
    nếu (virtual_active())
	ins_at_eol = FALSE;	// cursor always keeps its column
    không_thì
	ins_at_eol = (gchar_cursor() == NUL);
}

/*
 * If the cursor is on an indent, ^T/^D insert/delete one
 * shiftwidth.	Otherwise ^T/^D behave like a "<<" or ">>".
 * Always round the indent to 'shiftwidth', this is compatible
 * with vi.  But vi only supports ^T and ^D after an
 * autoindent, we support it everywhere.
 */
    static void
ins_shift(số_nguyên c, số_nguyên lastc)
{
    nếu (stop_arrow() == FAIL)
	trả_về;
    AppendCharToRedobuff(c);

    /*
     * 0^D and ^^D: remove all indent.
     */
    nếu (c == Ctrl_D && (lastc == '0' || lastc == '^')
						  && curwin->w_cursor.col > 0)
    {
	--curwin->w_cursor.col;
	(void)del_char(FALSE);		// delete the '^' or '0'
	// In Replace mode, restore the characters that '^' or '0' replaced.
	nếu (State & REPLACE_FLAG)
	    replace_pop_ins();
	nếu (lastc == '^')
	    old_indent = get_indent();	// remember curr. indent
	change_indent(INDENT_SET, 0, TRUE, 0, TRUE);
    }
    không_thì
	change_indent(c == Ctrl_D ? INDENT_DEC : INDENT_INC, 0, TRUE, 0, TRUE);

    nếu (did_ai && *skipwhite(ml_get_curline()) != NUL)
	did_ai = FALSE;
    did_si = FALSE;
    can_si = FALSE;
    can_si_back = FALSE;
    can_cindent = FALSE;	// no cindenting after ^D or ^T
}

    static void
ins_del(void)
{
    số_nguyên	    temp;

    nếu (stop_arrow() == FAIL)
	trả_về;
    nếu (gchar_cursor() == NUL)		// delete newline
    {
	temp = curwin->w_cursor.col;
	nếu (!can_bs(BS_EOL)		// only if "eol" included
		|| do_join(2, FALSE, TRUE, FALSE, FALSE) == FAIL)
	    vim_beep(BO_BS);
	không_thì
	{
	    curwin->w_cursor.col = temp;
	    // Adjust orig_line_count in case more lines have been deleted than
	    // have been added. That makes sure, that open_line() later
	    // can access all buffer lines correctly
	    nếu (State & VREPLACE_FLAG &&
		    orig_line_count > curbuf->b_ml.ml_line_count)
		orig_line_count = curbuf->b_ml.ml_line_count;
	}
    }
    không_thì nếu (del_char(FALSE) == FAIL)  // delete char under cursor
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
    static void
ins_bs_one(void)
{
    dec_cursor();
    nếu (State & REPLACE_FLAG)
    {
	// Don't delete characters before the insert point when in
	// Replace mode
	nếu (curwin->w_cursor.lnum != Insstart.lnum
		|| curwin->w_cursor.col >= Insstart.col)
	    replace_do_bs(-1);
    }
    không_thì
	(void)del_char(FALSE);
}

/*
 * Handle Backspace, delete-word and delete-line in Insert mode.
 * Return TRUE when backspace was actually used.
 */
    static số_nguyên
ins_bs(
    số_nguyên		c,
    số_nguyên		mode,
    số_nguyên		*inserted_space_p)
{
    linenr_T	lnum;
    số_nguyên		cc;
    số_nguyên		temp = 0;	    // init for GCC
    colnr_T	save_col;
    colnr_T	mincol;
    số_nguyên		did_backspace = FALSE;
    số_nguyên		in_indent;
    số_nguyên		oldState;
    số_nguyên		cpc[MAX_MCO];	    // composing characters
    số_nguyên		call_fix_indent = FALSE;

    /*
     * can't delete anything in an empty file
     * can't backup past first character in buffer
     * can't backup past starting point unless 'backspace' > 1
     * can backup to a previous line if 'backspace' == 0
     */
    nếu (       BUFEMPTY()
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
	trả_về FALSE;
    }

    nếu (stop_arrow() == FAIL)
	trả_về FALSE;
    in_indent = inindent(0);
    nếu (in_indent)
	can_cindent = FALSE;
    end_comment_pending = NUL;	// After BS, don't auto-end comment
#ifdef FEAT_RIGHTLEFT
    nếu (revins_on)	    // put cursor after last inserted char
	inc_cursor();
#endif

    // Virtualedit:
    //	BACKSPACE_CHAR eats a virtual space
    //	BACKSPACE_WORD eats all coladd
    //	BACKSPACE_LINE eats all coladd and keeps going
    nếu (curwin->w_cursor.coladd > 0)
    {
	nếu (mode == BACKSPACE_CHAR)
	{
	    --curwin->w_cursor.coladd;
	    trả_về TRUE;
	}
	nếu (mode == BACKSPACE_WORD)
	{
	    curwin->w_cursor.coladd = 0;
	    trả_về TRUE;
	}
	curwin->w_cursor.coladd = 0;
    }

    /*
     * Delete newline!
     */
    nếu (curwin->w_cursor.col == 0)
    {
	lnum = Insstart.lnum;
	nếu (curwin->w_cursor.lnum == lnum
#ifdef FEAT_RIGHTLEFT
			|| revins_on
#endif
				    )
	{
	    nếu (u_save((linenr_T)(curwin->w_cursor.lnum - 2),
			       (linenr_T)(curwin->w_cursor.lnum + 1)) == FAIL)
		trả_về FALSE;
	    --Insstart.lnum;
	    Insstart.col = ml_get_len(Insstart.lnum);
	}
	/*
	 * In replace mode:
	 * cc < 0: NL was inserted, delete it
	 * cc >= 0: NL was replaced, put original characters back
	 */
	cc = -1;
	nếu (State & REPLACE_FLAG)
	    cc = replace_pop();	    // returns -1 if NL was inserted
	/*
	 * In replace mode, in the line we started replacing, we only move the
	 * cursor.
	 */
	nếu ((State & REPLACE_FLAG) && curwin->w_cursor.lnum <= lnum)
	{
	    dec_cursor();
	}
	không_thì
	{
	    nếu (!(State & VREPLACE_FLAG)
				   || curwin->w_cursor.lnum > orig_line_count)
	    {
		temp = gchar_cursor();	// remember current char
		--curwin->w_cursor.lnum;

		// When "aw" is in 'formatoptions' we must delete the space at
		// the end of the line, otherwise the line will be broken
		// again when auto-formatting.
		nếu (has_format_option(FO_AUTO)
					   && has_format_option(FO_WHITE_PAR))
		{
		    char_u  *ptr = ml_get_buf(curbuf, curwin->w_cursor.lnum,
									FALSE);
		    số_nguyên	    len = ml_get_curline_len();

		    nếu (len > 0 && ptr[len - 1] == ' ')
		    {
			char_u *newp = alloc(curbuf->b_ml.ml_line_len - 1);

			nếu (newp != NULL)
			{
			    mch_memmove(newp, ptr, len - 1);
			    newp[len - 1] = NUL;
			    nếu (curbuf->b_ml.ml_line_len > len + 1)
				mch_memmove(newp + len, ptr + len + 1,
					   curbuf->b_ml.ml_line_len - len - 1);

			    nếu (curbuf->b_ml.ml_flags
					      & (ML_LINE_DIRTY | ML_ALLOCATED))
				vim_free(curbuf->b_ml.ml_line_ptr);
			    curbuf->b_ml.ml_line_ptr = newp;
			    curbuf->b_ml.ml_line_len--;
			    curbuf->b_ml.ml_line_textlen--;
			    curbuf->b_ml.ml_flags |= ML_LINE_DIRTY;
			}
		    }
		}

		(void)do_join(2, FALSE, FALSE, FALSE, FALSE);
		nếu (temp == NUL && gchar_cursor() != NUL)
		    inc_cursor();
	    }
	    không_thì
		dec_cursor();

	    /*
	     * In MODE_REPLACE mode we have to put back the text that was
	     * replaced by the NL. On the replace stack is first a
	     * NUL-terminated sequence of characters that were deleted and then
	     * the characters that NL replaced.
	     */
	    nếu (State & REPLACE_FLAG)
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
		trong_khi (cc > 0)
		{
		    save_col = curwin->w_cursor.col;
		    mb_replace_pop_ins(cc);
		    curwin->w_cursor.col = save_col;
		    cc = replace_pop();
		}
		// restore the characters that NL replaced
		replace_pop_ins();
		State = oldState;
	    }
	}
	did_ai = FALSE;
    }
    không_thì
    {
	/*
	 * Delete character(s) before the cursor.
	 */
#ifdef FEAT_RIGHTLEFT
	nếu (revins_on)		// put cursor on last inserted char
	    dec_cursor();
#endif
	mincol = 0;
						// keep indent
	nếu (mode == BACKSPACE_LINE
		&& (curbuf->b_p_ai || cindent_on())
#ifdef FEAT_RIGHTLEFT
		&& !revins_on
#endif
			    )
	{
	    save_col = curwin->w_cursor.col;
	    beginline(BL_WHITE);
	    nếu (curwin->w_cursor.col < save_col)
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
	nếu (	   mode == BACKSPACE_CHAR
		&& ((p_sta && in_indent)
		    || ((get_sts_value() != 0
#ifdef FEAT_VARTABS
			|| tabstop_count(curbuf->b_p_vsts_array)
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
	    char_u	*line;
	    char_u	*ptr;
	    char_u	*cursor_ptr;
	    char_u	*space_ptr;
	    colnr_T	space_vcol = 0;
	    số_nguyên		prev_space = FALSE;
	    colnr_T	want_col;

	    *inserted_space_p = FALSE;

	    space_ptr = ptr = line = ml_get_curline();
	    cursor_ptr = line + curwin->w_cursor.col;

	    // Compute virtual column of cursor position, and find the last
	    // whitespace before cursor that is preceded by non-whitespace.
	    // Use chartabsize() so that virtual text and wrapping are ignored.
	    trong_khi (ptr < cursor_ptr)
	    {
		số_nguyên	cur_space = VIM_ISWHITE(*ptr);

		nếu (!prev_space && cur_space)
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
	    nếu (p_sta && in_indent)
		want_vcol -= want_vcol % (số_nguyên)get_sw_value(curbuf);
	    không_thì
#ifdef FEAT_VARTABS
		want_vcol = tabstop_start(want_vcol, get_sts_value(),
						       curbuf->b_p_vsts_array);
#không_thì
		want_vcol -= want_vcol % (số_nguyên)get_sts_value();
#endif

	    // Find the position to stop backspacing.
	    // Use chartabsize() so that virtual text and wrapping are ignored.
	    trong_khi (TRUE)
	    {
		số_nguyên size = chartabsize(space_ptr, space_vcol);

		nếu (space_vcol + size > want_vcol)
		    break;
		space_vcol += size;
		MB_PTR_ADV(space_ptr);
	    }
	    want_col = space_ptr - line;

	    // Delete characters until we are at or before want_col.
	    trong_khi (curwin->w_cursor.col > want_col)
		ins_bs_one();

	    // Insert extra spaces until we are at want_vcol.
	    cho (; space_vcol < want_vcol; space_vcol++)
	    {
		// Remember the first char we inserted
		nếu (curwin->w_cursor.lnum == Insstart_orig.lnum
				   && curwin->w_cursor.col < Insstart_orig.col)
		    Insstart_orig.col = curwin->w_cursor.col;

		nếu (State & VREPLACE_FLAG)
		    ins_char(' ');
		không_thì
		{
		    ins_str((char_u *)" ", 1);
		    nếu ((State & REPLACE_FLAG))
			replace_push(NUL);
		}
	    }
	}

	/*
	 * Delete up to starting point, start of line or previous word.
	 */
	không_thì
	{
	    số_nguyên cclass = 0, prev_cclass = 0;

	    nếu (has_mbyte)
		cclass = mb_get_class(ml_get_cursor());
	    do
	    {
#ifdef FEAT_RIGHTLEFT
		nếu (!revins_on) // put cursor on char to be deleted
#endif
		    dec_cursor();

		cc = gchar_cursor();
		// look multi-byte character class
		nếu (has_mbyte)
		{
		    prev_cclass = cclass;
		    cclass = mb_get_class(ml_get_cursor());
		}

		// start of word?
		nếu (mode == BACKSPACE_WORD && !vim_isspace(cc))
		{
		    mode = BACKSPACE_WORD_NOT_SPACE;
		    temp = vim_iswordc(cc);
		}
		// end of word?
		không_thì nếu (mode == BACKSPACE_WORD_NOT_SPACE
			&& ((vim_isspace(cc) || vim_iswordc(cc) != temp)
			|| prev_cclass != cclass))
		{
#ifdef FEAT_RIGHTLEFT
		    nếu (!revins_on)
#endif
			inc_cursor();
#ifdef FEAT_RIGHTLEFT
		    không_thì nếu (State & REPLACE_FLAG)
			dec_cursor();
#endif
		    break;
		}
		nếu (State & REPLACE_FLAG)
		    replace_do_bs(-1);
		không_thì
		{
		    nếu (enc_utf8 && p_deco)
			(void)utfc_ptr2char(ml_get_cursor(), cpc);
		    (void)del_char(FALSE);
		    /*
		     * If there are combining characters and 'delcombine' is set
		     * move the cursor back.  Don't back up before the base
		     * character.
		     */
		    nếu (enc_utf8 && p_deco && cpc[0] != NUL)
			inc_cursor();
#ifdef FEAT_RIGHTLEFT
		    nếu (revins_chars)
		    {
			revins_chars--;
			revins_legal++;
		    }
		    nếu (revins_on && gchar_cursor() == NUL)
			break;
#endif
		}
		// Just a single backspace?:
		nếu (mode == BACKSPACE_CHAR)
		    break;
	    } trong_khi (
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
    nếu (curwin->w_cursor.col <= 1)
	did_ai = FALSE;

    nếu (call_fix_indent)
	fix_indent();

    /*
     * It's a little strange to put backspaces into the redo
     * buffer, but it makes auto-indent a lot easier to deal
     * with.
     */
    AppendCharToRedobuff(c);

    // If deleted before the insertion point, adjust it
    nếu (curwin->w_cursor.lnum == Insstart_orig.lnum
				  && curwin->w_cursor.col < Insstart_orig.col)
	Insstart_orig.col = curwin->w_cursor.col;

    // vi behaviour: the cursor moves backward but the character that
    //		     was there remains visible
    // Vim behaviour: the cursor moves backward and the character that
    //		      was there is erased from the screen.
    // We can emulate the vi behaviour by pretending there is a dollar
    // displayed even when there isn't.
    //  --pkv Sun Jan 19 01:56:40 EST 2003
    nếu (vim_strchr(p_cpo, CPO_BACKSPACE) != NULL && dollar_vcol == -1)
	dollar_vcol = curwin->w_virtcol;

#ifdef FEAT_FOLDING
    // When deleting a char the cursor line must never be in a closed fold.
    // E.g., when 'foldmethod' is indent and deleting the first non-white
    // char before a Tab.
    nếu (did_backspace)
	foldOpenCursor();
#endif

    trả_về did_backspace;
}

/*
 * Handle receiving P_PS: start paste mode.  Inserts the following text up to
 * P_PE literally.
 * When "drop" is TRUE then consume the text and drop it.
 */
    số_nguyên
bracketed_paste(paste_mode_T mode, số_nguyên drop, garray_T *gap)
{
    số_nguyên		c;
    char_u	buf[NUMBUFLEN + MB_MAXBYTES];
    số_nguyên		idx = 0;
    char_u	*end = find_termcode((char_u *)"PE");
    số_nguyên		ret_char = -1;
    số_nguyên		save_allow_keys = allow_keys;
    số_nguyên		save_paste = p_paste;

    // If the end code is too long we can't detect it, read everything.
    nếu (end != NULL && STRLEN(end) >= NUMBUFLEN)
	end = NULL;
    ++no_mapping;
    allow_keys = 0;
    nếu (!p_paste)
	// Also have the side effects of setting 'paste' to make it work much
	// faster.
	set_option_value_give_err((char_u *)"paste", TRUE, NULL, 0);

    cho (;;)
    {
	// When the end is not defined read everything there is.
	nếu (end == NULL && vpeekc() == NUL)
	    break;
	do
	    c = vgetc();
	trong_khi (c == K_IGNORE || c == K_VER_SCROLLBAR || c == K_HOR_SCROLLBAR);

	nếu (c == NUL || got_int || (ex_normal_busy > 0 && c == Ctrl_C))
	    // When CTRL-C was encountered the typeahead will be flushed and we
	    // won't get the end sequence.  Except when using ":normal".
	    break;

	nếu (has_mbyte)
	    idx += (*mb_char2bytes)(c, buf + idx);
	không_thì
	    buf[idx++] = c;
	buf[idx] = NUL;
	nếu (end != NULL && STRNCMP(buf, end, idx) == 0)
	{
	    nếu (end[idx] == NUL)
		break; // Found the end of paste code.
	    continue;
	}
	nếu (!drop)
	{
	    switch (mode)
	    {
		case PASTE_CMDLINE:
		    put_on_cmdline(buf, idx, TRUE);
		    break;

		case PASTE_EX:
		    // add one for the NUL that is going to be appended
		    nếu (gap != NULL && ga_grow(gap, idx + 1) == OK)
		    {
			mch_memmove((char *)gap->ga_data + gap->ga_len,
							     buf, (size_t)idx);
			gap->ga_len += idx;
		    }
		    break;

		case PASTE_INSERT:
		    nếu (stop_arrow() == OK)
		    {
			c = buf[0];
			nếu (idx == 1 && (c == CAR || c == K_KENTER || c == NL))
			    ins_eol(c);
			không_thì
			{
			    ins_char_bytes(buf, idx);
			    AppendToRedobuffLit(buf, idx);
			}
		    }
		    break;

		case PASTE_ONE_CHAR:
		    nếu (ret_char == -1)
		    {
			nếu (has_mbyte)
			    ret_char = (*mb_ptr2char)(buf);
			không_thì
			    ret_char = buf[0];
		    }
		    break;
	    }
	}
	idx = 0;
    }

    --no_mapping;
    allow_keys = save_allow_keys;
    nếu (!save_paste)
	set_option_value_give_err((char_u *)"paste", FALSE, NULL, 0);

    trả_về ret_char;
}

#nếu defined(FEAT_GUI_TABLINE)
    static void
ins_tabline(số_nguyên c)
{
    // We will be leaving the current window, unless closing another tab.
    nếu (c != K_TABMENU || current_tabmenu != TABLINE_MENU_CLOSE
		|| (current_tab != 0 && current_tab != tabpage_index(curtab)))
    {
	undisplay_dollar();
	start_arrow(&curwin->w_cursor);
	can_cindent = TRUE;
    }

    nếu (c == K_TABLINE)
	goto_tabpage(current_tab);
    không_thì
    {
	handle_tabmenu();
	redraw_statuslines();	// will redraw the tabline when needed
    }
}
#endif

#nếu defined(FEAT_GUI)
    void
ins_scroll(void)
{
    pos_T	tpos;

    undisplay_dollar();
    tpos = curwin->w_cursor;
    nếu (gui_do_scroll())
    {
	start_arrow(&tpos);
	can_cindent = TRUE;
    }
}

    void
ins_horscroll(void)
{
    pos_T	tpos;

    undisplay_dollar();
    tpos = curwin->w_cursor;
    nếu (do_mousescroll_horiz(scrollbar_value))
    {
	start_arrow(&tpos);
	can_cindent = TRUE;
    }
}
#endif

    static void
ins_left(void)
{
    pos_T	tpos;
    số_nguyên		end_change = dont_sync_undo == FALSE; // end undoable change

#ifdef FEAT_FOLDING
    nếu ((fdo_flags & FDO_HOR) && KeyTyped)
	foldOpenCursor();
#endif
    undisplay_dollar();
    tpos = curwin->w_cursor;
    nếu (oneleft() == OK)
    {
#nếu defined(FEAT_XIM) && defined(FEAT_GUI_GTK)
	// Only call start_arrow() when not busy with preediting, it will
	// break undo.  K_LEFT is inserted in im_correct_cursor().
	nếu (p_imst == IM_OVER_THE_SPOT || !im_is_preediting())
#endif
	{
	    start_arrow_with_change(&tpos, end_change);
	    nếu (!end_change)
		AppendCharToRedobuff(K_LEFT);
	}
#ifdef FEAT_RIGHTLEFT
	// If exit reversed string, position is fixed
	nếu (revins_scol != -1 && (số_nguyên)curwin->w_cursor.col >= revins_scol)
	    revins_legal++;
	revins_chars++;
#endif
    }

    /*
     * if 'whichwrap' set for cursor in insert mode may go to
     * previous line
     */
    không_thì nếu (vim_strchr(p_ww, '[') != NULL && curwin->w_cursor.lnum > 1)
    {
	// always break undo when moving upwards/downwards, else undo may break
	start_arrow(&tpos);
	--(curwin->w_cursor.lnum);
	coladvance((colnr_T)MAXCOL);
	curwin->w_set_curswant = true;	// so we stay at the end
    }
    không_thì
	vim_beep(BO_CRSR);
    dont_sync_undo = FALSE;
}

    static void
ins_home(số_nguyên c)
{
    pos_T	tpos;

#ifdef FEAT_FOLDING
    nếu ((fdo_flags & FDO_HOR) && KeyTyped)
	foldOpenCursor();
#endif
    undisplay_dollar();
    tpos = curwin->w_cursor;
    nếu (c == K_C_HOME)
	curwin->w_cursor.lnum = 1;
    curwin->w_cursor.col = 0;
    curwin->w_cursor.coladd = 0;
    curwin->w_curswant = 0;
    start_arrow(&tpos);
}

    static void
ins_end(số_nguyên c)
{
    pos_T	tpos;

#ifdef FEAT_FOLDING
    nếu ((fdo_flags & FDO_HOR) && KeyTyped)
	foldOpenCursor();
#endif
    undisplay_dollar();
    tpos = curwin->w_cursor;
    nếu (c == K_C_END)
	curwin->w_cursor.lnum = curbuf->b_ml.ml_line_count;
    coladvance((colnr_T)MAXCOL);
    curwin->w_curswant = MAXCOL;

    start_arrow(&tpos);
}

    static void
ins_s_left(void)
{
    số_nguyên end_change = dont_sync_undo == FALSE; // end undoable change
#ifdef FEAT_FOLDING
    nếu ((fdo_flags & FDO_HOR) && KeyTyped)
	foldOpenCursor();
#endif
    undisplay_dollar();
    nếu (curwin->w_cursor.lnum > 1 || curwin->w_cursor.col > 0)
    {
	start_arrow_with_change(&curwin->w_cursor, end_change);
	nếu (!end_change)
	    AppendCharToRedobuff(K_S_LEFT);
	(void)bck_word(1L, FALSE, FALSE);
	curwin->w_set_curswant = true;
    }
    không_thì
	vim_beep(BO_CRSR);
    dont_sync_undo = FALSE;
}

    static void
ins_right(void)
{
    số_nguyên end_change = dont_sync_undo == FALSE; // end undoable change

#ifdef FEAT_FOLDING
    nếu ((fdo_flags & FDO_HOR) && KeyTyped)
	foldOpenCursor();
#endif
    undisplay_dollar();
    nếu (gchar_cursor() != NUL || virtual_active())
    {
	start_arrow_with_change(&curwin->w_cursor, end_change);
	nếu (!end_change)
	    AppendCharToRedobuff(K_RIGHT);
	curwin->w_set_curswant = true;
	nếu (virtual_active())
	    oneright();
	không_thì
	{
	    nếu (has_mbyte)
		curwin->w_cursor.col += (*mb_ptr2len)(ml_get_cursor());
	    không_thì
		++curwin->w_cursor.col;
	}

#ifdef FEAT_RIGHTLEFT
	revins_legal++;
	nếu (revins_chars)
	    revins_chars--;
#endif
    }
    // if 'whichwrap' set for cursor in insert mode, may move the
    // cursor to the next line
    không_thì nếu (vim_strchr(p_ww, ']') != NULL
	    && curwin->w_cursor.lnum < curbuf->b_ml.ml_line_count)
    {
	start_arrow(&curwin->w_cursor);
	curwin->w_set_curswant = true;
	++curwin->w_cursor.lnum;
	curwin->w_cursor.col = 0;
    }
    không_thì
	vim_beep(BO_CRSR);
    dont_sync_undo = FALSE;
}

    static void
ins_s_right(void)
{
    số_nguyên end_change = dont_sync_undo == FALSE; // end undoable change
#ifdef FEAT_FOLDING
    nếu ((fdo_flags & FDO_HOR) && KeyTyped)
	foldOpenCursor();
#endif
    undisplay_dollar();
    nếu (curwin->w_cursor.lnum < curbuf->b_ml.ml_line_count
	    || gchar_cursor() != NUL)
    {
	start_arrow_with_change(&curwin->w_cursor, end_change);
	nếu (!end_change)
	    AppendCharToRedobuff(K_S_RIGHT);
	(void)fwd_word(1L, FALSE, 0);
	curwin->w_set_curswant = true;
    }
    không_thì
	vim_beep(BO_CRSR);
    dont_sync_undo = FALSE;
}

    static void
ins_up(
    số_nguyên		startcol)	// when TRUE move to Insstart.col
{
    pos_T	tpos;
    linenr_T	old_topline = curwin->w_topline;
#ifdef FEAT_DIFF
    số_nguyên		old_topfill = curwin->w_topfill;
#endif

    undisplay_dollar();
    tpos = curwin->w_cursor;
    nếu (cursor_up(1L, TRUE) == OK)
    {
	nếu (startcol)
	    coladvance(getvcol_nolist(&Insstart));
	nếu (old_topline != curwin->w_topline
#ifdef FEAT_DIFF
		|| old_topfill != curwin->w_topfill
#endif
		)
	    redraw_later(UPD_VALID);
	start_arrow(&tpos);
	can_cindent = TRUE;
    }
    không_thì
	vim_beep(BO_CRSR);
}

    static void
ins_pageup(void)
{
    pos_T	tpos;

    undisplay_dollar();

    nếu (mod_mask & MOD_MASK_CTRL)
    {
	// <C-PageUp>: tab page back
	nếu (first_tabpage->tp_next != NULL)
	{
	    start_arrow(&curwin->w_cursor);
	    goto_tabpage(-1);
	}
	trả_về;
    }

    tpos = curwin->w_cursor;
    nếu (pagescroll(BACKWARD, 1L, FALSE) == OK)
    {
	start_arrow(&tpos);
	can_cindent = TRUE;
    }
    không_thì
	vim_beep(BO_CRSR);
}

    static void
ins_down(
    số_nguyên		startcol)	// when TRUE move to Insstart.col
{
    pos_T	tpos;
    linenr_T	old_topline = curwin->w_topline;
#ifdef FEAT_DIFF
    số_nguyên		old_topfill = curwin->w_topfill;
#endif

    undisplay_dollar();
    tpos = curwin->w_cursor;
    nếu (cursor_down(1L, TRUE) == OK)
    {
	nếu (startcol)
	    coladvance(getvcol_nolist(&Insstart));
	nếu (old_topline != curwin->w_topline
#ifdef FEAT_DIFF
		|| old_topfill != curwin->w_topfill
#endif
		)
	    redraw_later(UPD_VALID);
	start_arrow(&tpos);
	can_cindent = TRUE;
    }
    không_thì
	vim_beep(BO_CRSR);
}

    static void
ins_pagedown(void)
{
    pos_T	tpos;

    undisplay_dollar();

    nếu (mod_mask & MOD_MASK_CTRL)
    {
	// <C-PageDown>: tab page forward
	nếu (first_tabpage->tp_next != NULL)
	{
	    start_arrow(&curwin->w_cursor);
	    goto_tabpage(0);
	}
	trả_về;
    }

    tpos = curwin->w_cursor;
    nếu (pagescroll(FORWARD, 1L, FALSE) == OK)
    {
	start_arrow(&tpos);
	can_cindent = TRUE;
    }
    không_thì
	vim_beep(BO_CRSR);
}

#ifdef FEAT_DND
    static void
ins_drop(void)
{
    do_put('~', NULL, BACKWARD, 1L, PUT_CURSEND);
}
#endif

/*
 * Handle TAB in Insert or Replace mode.
 * Return TRUE when the TAB needs to be inserted like a normal character.
 */
    static số_nguyên
ins_tab(void)
{
    số_nguyên		ind;
    số_nguyên		i;
    số_nguyên		temp;

    nếu (Insstart_blank_vcol == MAXCOL && curwin->w_cursor.lnum == Insstart.lnum)
	Insstart_blank_vcol = get_nolist_virtcol();
    nếu (echeck_abbr(TAB + ABBR_OFF))
	trả_về FALSE;

    ind = inindent(0);
    nếu (ind)
	can_cindent = FALSE;

    /*
     * When nothing special, insert TAB like a normal character.
     */
    nếu (!curbuf->b_p_et
#ifdef FEAT_VARTABS
	    && !(p_sta && ind
		// These five lines mean 'tabstop' != 'shiftwidth'
		&& ((tabstop_count(curbuf->b_p_vts_array) > 1)
		    || (tabstop_count(curbuf->b_p_vts_array) == 1
			&& tabstop_first(curbuf->b_p_vts_array)
						       != get_sw_value(curbuf))
		    || (tabstop_count(curbuf->b_p_vts_array) == 0
			&& curbuf->b_p_ts != get_sw_value(curbuf))))
	    && tabstop_count(curbuf->b_p_vsts_array) == 0
#không_thì
	    && !(p_sta && ind && curbuf->b_p_ts != get_sw_value(curbuf))
#endif
	    && get_sts_value() == 0)
	trả_về TRUE;

    nếu (stop_arrow() == FAIL)
	trả_về TRUE;

    did_ai = FALSE;
    did_si = FALSE;
    can_si = FALSE;
    can_si_back = FALSE;
    AppendToRedobuff((char_u *)"\t");

#ifdef FEAT_VARTABS
    nếu (p_sta && ind)		// insert tab in indent, use 'shiftwidth'
    {
	temp = (số_nguyên)get_sw_value(curbuf);
	temp -= get_nolist_virtcol() % temp;
    }
    không_thì nếu (tabstop_count(curbuf->b_p_vsts_array) > 0 || curbuf->b_p_sts != 0)
				// use 'softtabstop' when set
	temp = tabstop_padding(get_nolist_virtcol(), get_sts_value(),
						     curbuf->b_p_vsts_array);
    không_thì			// otherwise use 'tabstop'
	temp = tabstop_padding(get_nolist_virtcol(), curbuf->b_p_ts,
						     curbuf->b_p_vts_array);
#không_thì
    nếu (p_sta && ind)		// insert tab in indent, use 'shiftwidth'
	temp = (số_nguyên)get_sw_value(curbuf);
    không_thì nếu (curbuf->b_p_sts != 0) // use 'softtabstop' when set
	temp = (số_nguyên)get_sts_value();
    không_thì			// otherwise use 'tabstop'
	temp = (số_nguyên)curbuf->b_p_ts;
    temp -= get_nolist_virtcol() % temp;
#endif

    /*
     * Insert the first space with ins_char().	It will delete one char in
     * replace mode.  Insert the rest with ins_str(); it will not delete any
     * chars.  For MODE_VREPLACE state, we use ins_char() for all characters.
     */
    ins_char(' ');
    trong_khi (--temp > 0)
    {
	nếu (State & VREPLACE_FLAG)
	    ins_char(' ');
	không_thì
	{
	    ins_str((char_u *)" ", 1);
	    nếu (State & REPLACE_FLAG)	    // no char replaced
		replace_push(NUL);
	}
    }

    /*
     * When 'expandtab' not set: Replace spaces by TABs where possible.
     */
#ifdef FEAT_VARTABS
    nếu (!curbuf->b_p_et && (tabstop_count(curbuf->b_p_vsts_array) > 0
			    || get_sts_value() > 0
			    || (p_sta && ind)))
#không_thì
    nếu (!curbuf->b_p_et && (get_sts_value() || (p_sta && ind)))
#endif
    {
	char_u		*ptr;
	char_u		*saved_line = NULL;	// init for GCC
	pos_T		pos;
	pos_T		fpos;
	pos_T		*cursor;
	colnr_T		want_vcol, vcol;
	số_nguyên		change_col = -1;
	số_nguyên		save_list = curwin->w_p_list;
	char_u		*tab = (char_u *)"\t";
	chartabsize_T	cts;

	/*
	 * Get the current line.  For MODE_VREPLACE state, don't make real
	 * changes yet, just work on a copy of the line.
	 */
	nếu (State & VREPLACE_FLAG)
	{
	    pos = curwin->w_cursor;
	    cursor = &pos;
	    saved_line = vim_strnsave(ml_get_curline(), ml_get_curline_len());
	    nếu (saved_line == NULL)
		trả_về FALSE;
	    ptr = saved_line + pos.col;
	}
	không_thì
	{
	    ptr = ml_get_cursor();
	    cursor = &curwin->w_cursor;
	}

	// When 'L' is not in 'cpoptions' a tab always takes up 'ts' spaces.
	nếu (vim_strchr(p_cpo, CPO_LISTWM) == NULL)
	    curwin->w_p_list = FALSE;

	// Find first white before the cursor
	fpos = curwin->w_cursor;
	trong_khi (fpos.col > 0 && VIM_ISWHITE(ptr[-1]))
	{
	    --fpos.col;
	    --ptr;
	}

	// In Replace mode, don't change characters before the insert point.
	nếu ((State & REPLACE_FLAG)
		&& fpos.lnum == Insstart.lnum
		&& fpos.col < Insstart.col)
	{
	    ptr += Insstart.col - fpos.col;
	    fpos.col = Insstart.col;
	}

	// compute virtual column numbers of first white and cursor
	getvcol(curwin, &fpos, &vcol, NULL, NULL, 0);
	getvcol(curwin, cursor, &want_vcol, NULL, NULL, 0);

	init_chartabsize_arg(&cts, curwin, 0, vcol, tab, tab);

	// Use as many TABs as possible.  Beware of 'breakindent', 'showbreak'
	// and 'linebreak' adding extra virtual columns.
	trong_khi (VIM_ISWHITE(*ptr))
	{
	    i = lbr_chartabsize(&cts);
	    nếu (cts.cts_vcol + i > want_vcol)
		break;
	    nếu (*ptr != TAB)
	    {
		*ptr = TAB;
		nếu (change_col < 0)
		{
		    change_col = fpos.col;  // Column of first change
		    // May have to adjust Insstart
		    nếu (fpos.lnum == Insstart.lnum && fpos.col < Insstart.col)
			Insstart.col = fpos.col;
		}
	    }
	    ++fpos.col;
	    ++ptr;
	    cts.cts_vcol += i;
	}
	vcol = cts.cts_vcol;
	clear_chartabsize_arg(&cts);

	nếu (change_col >= 0)
	{
	    số_nguyên		    repl_off = 0;

	    // Skip over the spaces we need.
	    init_chartabsize_arg(&cts, curwin, 0, vcol, ptr, ptr);
	    trong_khi (cts.cts_vcol < want_vcol && *cts.cts_ptr == ' ')
	    {
		cts.cts_vcol += lbr_chartabsize(&cts);
		++cts.cts_ptr;
		++repl_off;
	    }
	    ptr = cts.cts_ptr;
	    vcol = cts.cts_vcol;
	    clear_chartabsize_arg(&cts);

	    nếu (vcol > want_vcol)
	    {
		// Must have a char with 'showbreak' just before it.
		--ptr;
		--repl_off;
	    }
	    fpos.col += repl_off;

	    // Delete following spaces.
	    i = cursor->col - fpos.col;
	    nếu (i > 0)
	    {
#ifdef FEAT_PROP_POPUP
		nếu (!(State & VREPLACE_FLAG))
		{
		    char_u  *newp;
		    số_nguyên	    col;

		    newp = alloc(curbuf->b_ml.ml_line_len - i);
		    nếu (newp == NULL)
			trả_về FALSE;

		    col = ptr - curbuf->b_ml.ml_line_ptr;
		    nếu (col > 0)
			mch_memmove(newp, ptr - col, col);
		    mch_memmove(newp + col, ptr + i,
					   curbuf->b_ml.ml_line_len - col - i);

		    nếu (curbuf->b_ml.ml_flags & (ML_LINE_DIRTY | ML_ALLOCATED))
			vim_free(curbuf->b_ml.ml_line_ptr);
		    curbuf->b_ml.ml_line_ptr = newp;
		    curbuf->b_ml.ml_line_len -= i;
		    curbuf->b_ml.ml_line_textlen = 0;
		    curbuf->b_ml.ml_flags =
			   (curbuf->b_ml.ml_flags | ML_LINE_DIRTY) & ~ML_EMPTY;
		}
		không_thì
#endif
		    STRMOVE(ptr, ptr + i);
		// correct replace stack.
		nếu ((State & REPLACE_FLAG) && !(State & VREPLACE_FLAG))
		    cho (temp = i; --temp >= 0; )
			replace_join(repl_off);
	    }
#ifdef FEAT_NETBEANS_INTG
	    nếu (netbeans_active())
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
	    nếu (State & VREPLACE_FLAG)
	    {
		// Backspace from real cursor to change_col
		backspace_until_column(change_col);

		// Insert each char in saved_line from changed_col to
		// ptr-cursor
		ins_bytes_len(saved_line + change_col,
						    cursor->col - change_col);
	    }
	}

	nếu (State & VREPLACE_FLAG)
	    vim_free(saved_line);
	curwin->w_p_list = save_list;
    }

    trả_về FALSE;
}

/*
 * Handle CR or NL in insert mode.
 * Return FAIL when out of memory or can't undo.
 */
    số_nguyên
ins_eol(số_nguyên c)
{
    số_nguyên	    i;

    nếu (echeck_abbr(c + ABBR_OFF))
	trả_về OK;
    nếu (stop_arrow() == FAIL)
	trả_về FAIL;
    undisplay_dollar();

    /*
     * Strange Vi behaviour: In Replace mode, typing a NL will not delete the
     * character under the cursor.  Only push a NUL on the replace stack,
     * nothing to put back when the NL is deleted.
     */
    nếu ((State & REPLACE_FLAG) && !(State & VREPLACE_FLAG))
	replace_push(NUL);

    /*
     * In MODE_VREPLACE state, a NL replaces the rest of the line, and starts
     * replacing the next line, so we push all of the characters left on the
     * line onto the replace stack.  This is not done here though, it is done
     * in open_line().
     */

    // Put cursor on NUL if on the last char and coladd is 1 (happens after
    // CTRL-O).
    nếu (virtual_active() && curwin->w_cursor.coladd > 0)
	coladvance(getviscol());

#ifdef FEAT_RIGHTLEFT
    // NL in reverse insert will always start in the end of
    // current line.
    nếu (revins_on)
	curwin->w_cursor.col += ml_get_cursor_len();
#endif

    AppendToRedobuff(NL_STR);
    i = open_line(FORWARD,
	    has_format_option(FO_RET_COMS) ? OPENLINE_DO_COM : 0, old_indent,
	    NULL);
    old_indent = 0;
    can_cindent = TRUE;
#ifdef FEAT_FOLDING
    // When inserting a line the cursor line must never be in a closed fold.
    foldOpenCursor();
#endif

    trả_về i;
}

#ifdef FEAT_DIGRAPHS
/*
 * Handle digraph in insert mode.
 * Returns character still to be inserted, or NUL when nothing remaining to be
 * done.
 */
    static số_nguyên
ins_digraph(void)
{
    số_nguyên	    c;
    số_nguyên	    cc;
    số_nguyên	    did_putchar = FALSE;

    pc_status = PC_STATUS_UNSET;
    nếu (redrawing() && !char_avail())
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
    ++allow_keys;
    c = plain_vgetc();
    --no_mapping;
    --allow_keys;
    nếu (did_putchar)
	// when the line fits in 'columns' the '?' is at the start of the next
	// line and will not be removed by the redraw
	edit_unputchar();

    nếu (IS_SPECIAL(c) || mod_mask)	    // special key
    {
	clear_showcmd();
	insert_special(c, TRUE, FALSE);
	trả_về NUL;
    }
    nếu (c != ESC)
    {
	did_putchar = FALSE;
	nếu (redrawing() && !char_avail())
	{
	    // may need to redraw when no more chars available now
	    ins_redraw(FALSE);

	    nếu (char2cells(c) == 1)
	    {
		ins_redraw(FALSE);
		edit_putchar(c, TRUE);
		did_putchar = TRUE;
	    }
	    add_to_showcmd_c(c);
	}
	++no_mapping;
	++allow_keys;
	cc = plain_vgetc();
	--no_mapping;
	--allow_keys;
	nếu (did_putchar)
	    // when the line fits in 'columns' the '?' is at the start of the
	    // next line and will not be removed by a redraw
	    edit_unputchar();
	nếu (cc != ESC)
	{
	    AppendToRedobuff((char_u *)CTRL_V_STR);
	    c = digraph_get(c, cc, TRUE);
	    clear_showcmd();
	    trả_về c;
	}
    }
    clear_showcmd();
    trả_về NUL;
}
#endif

/*
 * Handle CTRL-E and CTRL-Y in Insert mode: copy char from other line.
 * Returns the char to be inserted, or NUL if none found.
 */
    số_nguyên
ins_copychar(linenr_T lnum)
{
    số_nguyên		    c;
    char_u	    *ptr, *prev_ptr;
    char_u	    *line;
    chartabsize_T   cts;

    nếu (lnum < 1 || lnum > curbuf->b_ml.ml_line_count)
    {
	vim_beep(BO_COPY);
	trả_về NUL;
    }

    // try to advance to the cursor column
    validate_virtcol();
    line = ml_get(lnum);
    prev_ptr = line;
    init_chartabsize_arg(&cts, curwin, lnum, 0, line, line);
    trong_khi (cts.cts_vcol < curwin->w_virtcol && *cts.cts_ptr != NUL)
    {
	prev_ptr = cts.cts_ptr;
	cts.cts_vcol += lbr_chartabsize_adv(&cts);
    }
    nếu (cts.cts_vcol > curwin->w_virtcol)
	ptr = prev_ptr;
    không_thì
	ptr = cts.cts_ptr;
    clear_chartabsize_arg(&cts);

    c = (*mb_ptr2char)(ptr);
    nếu (c == NUL)
	vim_beep(BO_COPY);
    trả_về c;
}

/*
 * CTRL-Y or CTRL-E typed in Insert mode.
 */
    static số_nguyên
ins_ctrl_ey(số_nguyên tc)
{
    số_nguyên	    c = tc;

    nếu (ctrl_x_mode_scroll())
    {
	nếu (c == Ctrl_Y)
	    scrolldown_clamp();
	không_thì
	    scrollup_clamp();
	redraw_later(UPD_VALID);
    }
    không_thì
    {
	c = ins_copychar(curwin->w_cursor.lnum + (c == Ctrl_Y ? -1 : 1));
	nếu (c != NUL)
	{
	    long	tw_save;

	    // The character must be taken literally, insert like it
	    // was typed after a CTRL-V, and pretend 'textwidth'
	    // wasn't set.  Digits, 'o' and 'x' are special after a
	    // CTRL-V, don't use it for these.
	    nếu (c < 256 && !SAFE_isalnum(c))
		AppendToRedobuff((char_u *)CTRL_V_STR);	// CTRL-V
	    tw_save = curbuf->b_p_tw;
	    curbuf->b_p_tw = -1;
	    insert_special(c, TRUE, FALSE);
	    curbuf->b_p_tw = tw_save;
#ifdef FEAT_RIGHTLEFT
	    revins_chars++;
	    revins_legal++;
#endif
	    c = Ctrl_V;	// pretend CTRL-V is last character
	    auto_format(FALSE, TRUE);
	}
    }
    trả_về c;
}

/*
 * Get the value that w_virtcol would have when 'list' is off.
 * Unless 'cpo' contains the 'L' flag.
 */
    colnr_T
get_nolist_virtcol(void)
{
    // check validity of cursor in current buffer
    nếu (curwin->w_buffer == NULL
	|| curwin->w_buffer->b_ml.ml_mfp == NULL
	|| curwin->w_cursor.lnum > curwin->w_buffer->b_ml.ml_line_count)
	trả_về 0;
    nếu (curwin->w_p_list && vim_strchr(p_cpo, CPO_LISTWM) == NULL)
	trả_về getvcol_nolist(&curwin->w_cursor);
    validate_virtcol();
    trả_về curwin->w_virtcol;
}

#nếu defined(FEAT_EVAL)
/*
 * Handle the InsertCharPre autocommand.
 * "c" is the character that was typed.
 * Return a pointer to allocated memory with the replacement string.
 * Return NULL to continue inserting "c".
 */
    static char_u *
do_insert_char_pre(số_nguyên c)
{
    char_u	*res;
    char_u	buf[MB_MAXBYTES + 1];
    size_t	buflen;
    số_nguyên		save_State = State;

    // Return quickly when there is nothing to do.
    nếu (!has_insertcharpre())
	trả_về NULL;

    nếu (c == Ctrl_RSB)
	trả_về NULL;

    nếu (has_mbyte)
	buflen = (*mb_char2bytes)(c, buf);
    không_thì
    {
	buf[0] = c;
	buflen = 1;
    }
    buf[buflen] = NUL;

    // Lock the text to avoid weird things from happening.
    ++textlock;
    set_vim_var_string(VV_CHAR, buf, (số_nguyên)buflen);  // set v:char

    res = NULL;
    nếu (ins_apply_autocmds(EVENT_INSERTCHARPRE))
    {
	// Get the value of v:char.  It may be empty or more than one
	// character.  Only use it when changed, otherwise continue with the
	// original character to avoid breaking autoindent.
	nếu (STRCMP(buf, get_vim_var_str(VV_CHAR)) != 0)
	    res = vim_strsave(get_vim_var_str(VV_CHAR));
    }

    set_vim_var_string(VV_CHAR, NULL, -1);  // clear v:char
    --textlock;

    // Restore the State, it may have been changed.
    State = save_State;

    trả_về res;
}
#endif

    số_nguyên
get_can_cindent(void)
{
    trả_về can_cindent;
}

    void
set_can_cindent(số_nguyên val)
{
    can_cindent = val;
}

/*
 * Trigger "event" and take care of fixing undo.
 */
    số_nguyên
ins_apply_autocmds(event_T event)
{
    varnumber_T	tick = CHANGEDTICK(curbuf);
    số_nguyên r;

    r = apply_autocmds(event, NULL, NULL, FALSE, curbuf);

    // If u_savesub() was called then we are not prepared to start
    // a new line.  Call u_save() with no contents to fix that.
    // Except when leaving Insert mode.
    nếu (event != EVENT_INSERTLEAVE && tick != CHANGEDTICK(curbuf))
	u_save(curwin->w_cursor.lnum, (linenr_T)(curwin->w_cursor.lnum + 1));

    trả_về r;
}
