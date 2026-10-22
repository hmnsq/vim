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

#ವ್ಯಾಖ್ಯಾನಿಸು BACKSPACE_CHAR		    1
#ವ್ಯಾಖ್ಯಾನಿಸು BACKSPACE_ಶಬ್ದ		    2
#ವ್ಯಾಖ್ಯಾನಿಸು BACKSPACE_ಶಬ್ದ_NOT_SPACE    3
#ವ್ಯಾಖ್ಯಾನಿಸು BACKSPACE_ಸಾಲು		    4

// Set when doing something for completion that may call edit() recursively,
// which is not allowed.
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	compl_busy = FALSE;


ಸ್ಥಿರ ಶೂನ್ಯ ins_ctrl_v(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ ಸೇರಿಸು_special(ಪೂರ್ಣಾಂಕ, ಪೂರ್ಣಾಂಕ, ಪೂರ್ಣಾಂಕ);
ಸ್ಥಿರ ಶೂನ್ಯ redo_literal(ಪೂರ್ಣಾಂಕ c);
ಸ್ಥಿರ ಶೂನ್ಯ ಆರಂಭ_arrow_common(pos_T *ಕೊನೆ_ಸೇರಿಸು_pos, ಪೂರ್ಣಾಂಕ change);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SPELL
ಸ್ಥಿರ ಶೂನ್ಯ check_spell_redraw(ಶೂನ್ಯ);
#ಆದರೆ_ಅಂತ್ಯ
ಸ್ಥಿರ ಶೂನ್ಯ stop_ಸೇರಿಸು(pos_T *ಕೊನೆ_ಸೇರಿಸು_pos, ಪೂರ್ಣಾಂಕ esc, ಪೂರ್ಣಾಂಕ nomove);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ  echeck_abbr(ಪೂರ್ಣಾಂಕ);
ಸ್ಥಿರ ಶೂನ್ಯ mb_replace_ತೆಗೆ_ins(ಪೂರ್ಣಾಂಕ cc);
ಸ್ಥಿರ ಶೂನ್ಯ replace_flush(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ replace_do_bs(ಪೂರ್ಣಾಂಕ ಎಲ್ಲೆ_ಲಂಬಸಾಲು);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ del_char_after_ಲಂಬಸಾಲು(ಪೂರ್ಣಾಂಕ ಎಲ್ಲೆ_ಲಂಬಸಾಲು);
ಸ್ಥಿರ ಶೂನ್ಯ ins_reg(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ ins_ctrl_g(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ ins_ctrl_hat(ಶೂನ್ಯ);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ  ins_esc(ದೀರ್ಘ *ಎಣಿಕೆ_2, ಪೂರ್ಣಾಂಕ cmdchar, ಪೂರ್ಣಾಂಕ nomove);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RIGHTLEFT
ಸ್ಥಿರ ಶೂನ್ಯ ins_ctrl_(ಶೂನ್ಯ);
#ಆದರೆ_ಅಂತ್ಯ
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ins_ಆರಂಭ_select(ಪೂರ್ಣಾಂಕ c);
ಸ್ಥಿರ ಶೂನ್ಯ ins_ಸೇರಿಸು(ಪೂರ್ಣಾಂಕ replaceState);
ಸ್ಥಿರ ಶೂನ್ಯ ins_ctrl_o(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ ins_ಸರಿಸು(ಪೂರ್ಣಾಂಕ c, ಪೂರ್ಣಾಂಕ lastc);
ಸ್ಥಿರ ಶೂನ್ಯ ins_del(ಶೂನ್ಯ);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ  ins_bs(ಪೂರ್ಣಾಂಕ c, ಪೂರ್ಣಾಂಕ ಬಹುಲಕ, ಪೂರ್ಣಾಂಕ *inserted_space_p);
#ಆದರೆ defined(FEAT_GUI_TABLINE)
ಸ್ಥಿರ ಶೂನ್ಯ ins_tabline(ಪೂರ್ಣಾಂಕ c);
#ಆದರೆ_ಅಂತ್ಯ
ಸ್ಥಿರ ಶೂನ್ಯ ins_ಎಡ(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ ins_home(ಪೂರ್ಣಾಂಕ c);
ಸ್ಥಿರ ಶೂನ್ಯ ins_ಕೊನೆ(ಪೂರ್ಣಾಂಕ c);
ಸ್ಥಿರ ಶೂನ್ಯ ins_s_ಎಡ(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ ins_ಬಲ(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ ins_s_ಬಲ(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ ins_ಮೇಲೆ(ಪೂರ್ಣಾಂಕ startcol);
ಸ್ಥಿರ ಶೂನ್ಯ ins_pageup(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ ins_ಕೆಳಕ್ಕೆ(ಪೂರ್ಣಾಂಕ startcol);
ಸ್ಥಿರ ಶೂನ್ಯ ins_pagedown(ಶೂನ್ಯ);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DND
ಸ್ಥಿರ ಶೂನ್ಯ ins_drop(ಶೂನ್ಯ);
#ಆದರೆ_ಅಂತ್ಯ
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ  ins_tab(ಶೂನ್ಯ);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIGRAPHS
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ  ins_digraph(ಶೂನ್ಯ);
#ಆದರೆ_ಅಂತ್ಯ
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ  ins_ctrl_ey(ಪೂರ್ಣಾಂಕ tc);
#ಆದರೆ defined(FEAT_EVAL)
ಸ್ಥಿರ char_u *do_ಸೇರಿಸು_char_pre(ಪೂರ್ಣಾಂಕ c);
#ಆದರೆ_ಅಂತ್ಯ

ಸ್ಥಿರ colnr_T	Insstart_textlen;	// length of line when insert started
ಸ್ಥಿರ colnr_T	Insstart_blank_vcol;	// vcol for first inserted blank
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	update_Insstart_orig = TRUE; // set Insstart_orig to Insstart

ಸ್ಥಿರ string_T	ಕೊನೆಯ_ಸೇರಿಸು = {ಖಾಲಿ, 0};    // the text of the previous insert,
					    // K_SPECIAL and CSI are escaped
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	ಕೊನೆಯ_ಸೇರಿಸು_skip; // nr of chars in front of previous insert
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	new_ಸೇರಿಸು_skip;  // nr of chars in front of current insert
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	did_restart_edit;	// "restart_edit" when calling edit()

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	can_cindent;		// may do cindenting on this line

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RIGHTLEFT
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	revins_on;		// reverse insert mode on
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	revins_chars;		// how much to skip after edit
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	revins_legal;		// was the last char 'legal'?
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	revins_scol;		// start column of revins session
#ಆದರೆ_ಅಂತ್ಯ

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	ins_need_undo;		// call u_save() before inserting a
					// char.  Set when edit() is called.
					// after that arrow_used is used.

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	dont_sync_undo = FALSE;	// CTRL-G U prevents syncing undo for
					// the next left/right cursor key

// With 'autocompletedelay' set, arm the delay and let the main loop fire
// Insert-mode autocommands; the popup is shown later on K_COMPLETE_DELAY.
// Otherwise trigger completion right away.
#ವ್ಯಾಖ್ಯಾನಿಸು TRIGGER_AUTOCOMPLETE()			\
    ಮಾಡು {					\
	update_screen(UPD_VALID);  /* Show char (deletion) immediately */ \
	ಹೊರಹೋಗು_flush();				\
	ins_compl_enable_autocomplete();	\
	ins_compl_arm_autostart();		\
	ಆದರೆ (!ins_compl_arm_autocomplete_delay())\
	    ಹೋಗು docomplete;			\
    } ಆಗ (0)

#ವ್ಯಾಖ್ಯಾನಿಸು MAY_TRIGGER_AUTOCOMPLETE(c)				\
    ಮಾಡು {							\
	ಆದರೆ (ins_compl_has_autocomplete() && !char_avail()	\
		&& curwin->w_cursor.ಲಂಬಸಾಲು > 0)			\
	{							\
	    (c) = char_before_cursor();				\
	    ಆದರೆ (vim_isprintc(c))				\
		TRIGGER_AUTOCOMPLETE();				\
	}							\
    } ಆಗ (0)

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
    ಪೂರ್ಣಾಂಕ
edit(
    ಪೂರ್ಣಾಂಕ		cmdchar,
    ಪೂರ್ಣಾಂಕ		startln,	// if set, insert at start of line
    ದೀರ್ಘ	ಎಣಿಕೆ_2)
{
    ಪೂರ್ಣಾಂಕ		c = 0;
    char_u	*ptr;
    ಪೂರ್ಣಾಂಕ		lastc = 0;
    ಪೂರ್ಣಾಂಕ		mincol;
    ಸ್ಥಿರ linenr_T o_lnum = 0;
    ಪೂರ್ಣಾಂಕ		i;
    ಪೂರ್ಣಾಂಕ		did_backspace = TRUE;	    // previous char was backspace
    ಪೂರ್ಣಾಂಕ		ಸಾಲು_is_white = FALSE;	    // line is empty before insert
    linenr_T	old_topline = 0;	    // topline before insertion
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
    ಪೂರ್ಣಾಂಕ		old_topfill = -1;
#ಆದರೆ_ಅಂತ್ಯ
    ಪೂರ್ಣಾಂಕ		inserted_space = FALSE;     // just inserted a space
    ಪೂರ್ಣಾಂಕ		replaceState = ಬಹುಲಕ_REPLACE;
    ಪೂರ್ಣಾಂಕ		nomove = FALSE;		    // don't move cursor on return
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಕೆಲಸ_CHANNEL
    ಪೂರ್ಣಾಂಕ		cmdchar_todo = cmdchar;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CONCEAL
    ಪೂರ್ಣಾಂಕ		cursor_ಸಾಲು_was_concealed;
#ಆದರೆ_ಅಂತ್ಯ
    ಪೂರ್ಣಾಂಕ		ins_just_started = TRUE;

    // Remember whether editing was restarted after CTRL-O.
    did_restart_edit = restart_edit;

    // sleep before redrawing, needed for "CTRL-O :" that results in an
    // error message
    check_for_delay(TRUE);

    // set Insstart_orig to Insstart
    update_Insstart_orig = TRUE;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_SANDBOX
    // Don't allow inserting in the sandbox.
    ಆದರೆ (sandbox != 0)
    {
	emsg(_(e_not_allowed_ಒಳ_sandbox));
	ಹಿಂತಿರುಗಿ FALSE;
    }
#ಆದರೆ_ಅಂತ್ಯ
    // Don't allow changes in the buffer while editing the cmdline.  The
    // caller of getcmdline() may get confused.
    // Don't allow recursive insert mode when busy with completion.
    ಆದರೆ (textlock != 0 || ins_compl_ಸಕ್ರಿಯ() || compl_busy || pum_visible())
    {
	emsg(_(e_not_allowed_ಗಮ್ಯ_change_ಪಠ್ಯ_or_change_ಕಿಟಕಿ));
	ಹಿಂತಿರುಗಿ FALSE;
    }
    ins_compl_clear();	    // clear stuff for CTRL-X mode

    /*
     * Trigger InsertEnter autocommands.  Do not do this for "r<CR>" or "grx".
     */
    ಆದರೆ (cmdchar != 'r' && cmdchar != 'v')
    {
	pos_T   save_cursor = curwin->w_cursor;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
	ಆದರೆ (cmdchar == 'R')
	    ptr = (char_u *)"r";
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (cmdchar == 'V')
	    ptr = (char_u *)"v";
	ಇಲ್ಲದಿದ್ದರೆ
	    ptr = (char_u *)"i";
	ಗಣ_vim_var_string(VV_INSERTMODE, ptr, 1);
	ಗಣ_vim_var_string(VV_CHAR, ಖಾಲಿ, -1);  // clear v:char
#ಆದರೆ_ಅಂತ್ಯ
	ins_apply_autocmds(EVENT_INSERTENTER);

	// Check for changed highlighting, e.g. for ModeMsg.
	ಆದರೆ (need_highlight_ಬದಲಾಯಿತು)
	    highlight_ಬದಲಾಯಿತು();

	// Make sure the cursor didn't move.  Do call check_cursor_col() in
	// case the text was modified.  Since Insert mode was not started yet
	// a call to check_cursor_col() may move the cursor, especially with
	// the "A" command, thus set State to avoid that. Also check that the
	// line number is still valid (lines may have been deleted).
	// Do not restore if v:char was set to a non-empty string.
	ಆದರೆ (!EQUAL_POS(curwin->w_cursor, save_cursor)
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
		&& *get_vim_var_str(VV_CHAR) == NUL
#ಆದರೆ_ಅಂತ್ಯ
		&& save_cursor.lnum <= curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	{
	    ಪೂರ್ಣಾಂಕ save_state = State;

	    curwin->w_cursor = save_cursor;
	    State = ಬಹುಲಕ_ಸೇರಿಸು;
	    check_cursor_ಲಂಬಸಾಲು();
	    State = save_state;
	}
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CONCEAL
    // Check if the cursor line was concealed before changing State.
    cursor_ಸಾಲು_was_concealed = curwin->w_p_cole > 0
						&& conceal_cursor_ಸಾಲು(curwin);
#ಆದರೆ_ಅಂತ್ಯ

    /*
     * When doing a paste with the middle mouse button, Insstart is set to
     * where the paste started.
     */
    ಆದರೆ (where_paste_started.lnum != 0)
	Insstart = where_paste_started;
    ಇಲ್ಲದಿದ್ದರೆ
    {
	Insstart = curwin->w_cursor;
	ಆದರೆ (startln)
	    Insstart.ಲಂಬಸಾಲು = 0;
    }
    Insstart_textlen = (colnr_T)linetabsize_str(ml_get_curline());
    Insstart_blank_vcol = MAXCOL;
    ಆದರೆ (!did_ai)
	ai_ಲಂಬಸಾಲು = 0;

    ಆದರೆ (cmdchar != NUL && restart_edit == 0)
    {
	ResetRedobuff();
	Append_ಸಂಖ್ಯೆ_ಗಮ್ಯ_Redobuff(ಎಣಿಕೆ_2);
	ಆದರೆ (cmdchar == 'V' || cmdchar == 'v')
	{
	    // "gR" or "gr" command
	    Append_Char_ಗಮ್ಯ_Redobuff('g');
	    Append_Char_ಗಮ್ಯ_Redobuff((cmdchar == 'v') ? 'r' : 'R');
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಆದರೆ (cmdchar == K_PS)
		Append_Char_ಗಮ್ಯ_Redobuff('a');
	    ಇಲ್ಲದಿದ್ದರೆ
		Append_Char_ಗಮ್ಯ_Redobuff(cmdchar);
	    ಆದರೆ (cmdchar == 'g')		    // "gI" command
		Append_Char_ಗಮ್ಯ_Redobuff('I');
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (cmdchar == 'r')	    // "r<CR>" command
		ಎಣಿಕೆ_2 = 1;		    // insert only one <CR>
	}
    }

    ಆದರೆ (cmdchar == 'R')
    {
	State = ಬಹುಲಕ_REPLACE;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (cmdchar == 'V' || cmdchar == 'v')
    {
	State = ಬಹುಲಕ_VREPLACE;
	replaceState = ಬಹುಲಕ_VREPLACE;
	orig_ಸಾಲು_ಎಣಿಕೆ = curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
	vr_ಸಾಲುಗಳು_ಬದಲಾಯಿತು = 1;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	State = ಬಹುಲಕ_ಸೇರಿಸು;

    may_trigger_modechanged();
    stop_ಸೇರಿಸು_ಬಹುಲಕ = FALSE;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CONCEAL
    // Check if the cursor line needs redrawing after changing State.  If
    // 'concealcursor' is "n" it needs to be redrawn without concealing.
    conceal_check_cursor_ಸಾಲು(cursor_ಸಾಲು_was_concealed);
#ಆದರೆ_ಅಂತ್ಯ

    // Need to position cursor again when on a TAB and when on a char with
    // virtual text.
    ಆದರೆ (gchar_cursor() == TAB
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
	    || curbuf->b_has_textprop
#ಆದರೆ_ಅಂತ್ಯ
       )
	curwin->w_valid &= ~(VALID_WROW|VALID_WCOL|VALID_VIRTCOL);

    /*
     * Enable langmap or IME, indicated by 'iminsert'.
     * Note that IME may enabled/disabled without us noticing here, thus the
     * 'iminsert' value may not reflect what is actually used.  It is updated
     * when hitting <Esc>.
     */
    ಆದರೆ (curbuf->b_p_iminsert == B_IMODE_LMAP)
	State |= ಬಹುಲಕ_LANGMAP;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_ಒಳಹರಿವು_ವಿಧಾನ
    im_ಗಣ_ಸಕ್ರಿಯ(curbuf->b_p_iminsert == B_IMODE_IM);
#ಆದರೆ_ಅಂತ್ಯ

    setmouse();
    clear_showcmd();
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RIGHTLEFT
    // there is no reverse replace mode
    revins_on = (State == ಬಹುಲಕ_ಸೇರಿಸು && p_ri);
    ಆದರೆ (revins_on)
	undisplay_dollar();
    revins_chars = 0;
    revins_legal = 0;
    revins_scol = -1;
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ (!p_ek)
    {
	MAY_WANT_ಗಮ್ಯ_LOG_THIS;

	// Disable bracketed paste mode, we won't recognize the escape
	// sequences.
	ಹೊರಹೋಗು_str(T_BD);

	// Disable modifyOtherKeys, keys with modifiers would cause exiting
	// Insert mode.
	ಹೊರಹೋಗು_str_t_TE();
    }

    /*
     * Handle restarting Insert mode.
     * Don't do this for "CTRL-O ." (repeat an insert): In that case we get
     * here with something in the stuff buffer.
     */
    ಆದರೆ (restart_edit != 0 && stuff_ಖಾಲಿಗಳು())
    {
	/*
	 * After a paste we consider text typed to be part of the insert for
	 * the pasted text. You can backspace over the pasted text too.
	 */
	ಆದರೆ (where_paste_started.lnum)
	    arrow_ಬಳಸಿದ = FALSE;
	ಇಲ್ಲದಿದ್ದರೆ
	    arrow_ಬಳಸಿದ = TRUE;
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
	ಆದರೆ (((ins_at_eol && curwin->w_cursor.lnum == o_lnum)
		    || curwin->w_curswant > curwin->w_virtcol)
		&& *(ptr = ml_get_curline() + curwin->w_cursor.ಲಂಬಸಾಲು) != NUL)
	{
	    ಆದರೆ (ptr[1] == NUL)
		++curwin->w_cursor.ಲಂಬಸಾಲು;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (has_mbyte)
	    {
		i = (*mb_ptr2len)(ptr);
		ಆದರೆ (ptr[i] == NUL)
		    curwin->w_cursor.ಲಂಬಸಾಲು += i;
	    }
	}
	ins_at_eol = FALSE;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	arrow_ಬಳಸಿದ = FALSE;

    // we are in insert mode now, don't need to start it anymore
    need_ಆರಂಭ_insertmode = FALSE;

    // Need to save the line for undo before inserting the first char.
    ins_need_undo = TRUE;

    where_paste_started.lnum = 0;
    can_cindent = TRUE;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
    // The cursor line is not in a closed fold, unless 'insertmode' is set or
    // restarting.
    ಆದರೆ (!p_im && did_restart_edit == 0)
	foldOpenCursor();
#ಆದರೆ_ಅಂತ್ಯ

    /*
     * If 'showmode' is set, show the current (insert/replace/..) mode.
     * A warning message for changing a readonly file is given here, before
     * actually changing anything.  It's put after the mode, if any.
     */
    i = 0;
    ಆದರೆ (p_smd && ಸಂದೇಶ_silent == 0)
	i = showmode();

    ಆದರೆ (!p_im && did_restart_edit == 0)
	change_warning(i == 0 ? 0 : i + 1);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURSOR_SHAPE
    ui_cursor_shape();		// may show different cursor shape
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIGRAPHS
    do_digraph(-1);		// clear digraphs
#ಆದರೆ_ಅಂತ್ಯ

    /*
     * Get the current length of the redo buffer, those characters have to be
     * skipped if we want to get to the inserted characters.
     */
    string_T inserted = get_inserted();
    new_ಸೇರಿಸು_skip = (ಪೂರ್ಣಾಂಕ)inserted.ಉದ್ದಳತೆ;
    ಆದರೆ (inserted.string != ಖಾಲಿ)
	vim_free(inserted.string);

    old_indent = 0;

    /*
     * Main loop in Insert mode: repeat until Insert mode is left.
     */
    ಗಾಗಿ (;;)
    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RIGHTLEFT
	ಆದರೆ (!revins_legal)
	    revins_scol = -1;	    // reset on illegal motions
	ಇಲ್ಲದಿದ್ದರೆ
	    revins_legal = 0;
#ಆದರೆ_ಅಂತ್ಯ
	ಆದರೆ (arrow_ಬಳಸಿದ)	    // don't repeat insert when arrow key used
	    ಎಣಿಕೆ_2 = 0;

	ಆದರೆ (update_Insstart_orig)
	    Insstart_orig = Insstart;

	ಆದರೆ (stop_ಸೇರಿಸು_ಬಹುಲಕ && !ins_compl_ಸಕ್ರಿಯ())
	{
	    // ":stopinsert" used or 'insertmode' reset
	    ಎಣಿಕೆ_2 = 0;
	    ಹೋಗು doESCkey;
	}

	// set curwin->w_curswant for next K_DOWN or K_UP
	ಆದರೆ (!arrow_ಬಳಸಿದ)
	    curwin->w_ಗಣ_curswant = true;

	// If there is no typeahead may check for timestamps (e.g., for when a
	// menu invoked a shell command).
	ಆದರೆ (stuff_ಖಾಲಿಗಳು())
	{
	    did_check_timestamps = FALSE;
	    ಆದರೆ (need_check_timestamps)
		check_timestamps(FALSE);
	}

	/*
	 * When emsg() was called msg_scroll will have been set.
	 */
	ಸಂದೇಶ_scroll = FALSE;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
	// When 'mousefocus' is set a mouse movement may have taken us to
	// another window.  "need_mouse_correct" may then be set because of an
	// autocommand.
	ಆದರೆ (need_mouse_correct)
	    gui_mouse_correct();
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
	// Open fold at the cursor line, according to 'foldopen'.
	ಆದರೆ (fdo_flags & FDO_ಸೇರಿಸು)
	    foldOpenCursor();
	// Close folds where the cursor isn't, according to 'foldclose'
	ಆದರೆ (!char_avail())
	    foldCheckClose();
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಕೆಲಸ_CHANNEL
	ಆದರೆ (bt_prompt(curbuf))
	{
	    init_prompt(cmdchar_todo);
	    cmdchar_todo = NUL;
	}
#ಆದರೆ_ಅಂತ್ಯ

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
	ಆದರೆ (curbuf->b_mod_ಗಣ
		&& curwin->w_p_wrap
		&& !curwin->w_p_sms
		&& !did_backspace
		&& curwin->w_topline == old_topline
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
		&& curwin->w_topfill == old_topfill
#ಆದರೆ_ಅಂತ್ಯ
		&& ಎಣಿಕೆ_2 <= 1
		)
	{
	    mincol = curwin->w_wcol;
	    validate_cursor_ಲಂಬಸಾಲು();

	    ಆದರೆ (
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_VARTABS
		curwin->w_wcol < mincol - tabstop_at(get_nolist_virtcol(),
						     curbuf->b_p_ts,
						     curbuf->b_p_vts_array,
						     FALSE)
#ಇಲ್ಲದಿದ್ದರೆ
		(ಪೂರ್ಣಾಂಕ)curwin->w_wcol < mincol - curbuf->b_p_ts
#ಆದರೆ_ಅಂತ್ಯ
		    && curwin->w_wrow ==
				   curwin->w_ಎತ್ತರ - 1 - get_scrolloff_ಮೌಲ್ಯ()
		    && (curwin->w_cursor.lnum != curwin->w_topline
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
			|| curwin->w_topfill > 0
#ಆದರೆ_ಅಂತ್ಯ
		    ))
	    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
		ಆದರೆ (curwin->w_topfill > 0)
		    --curwin->w_topfill;
		ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
		ಆದರೆ (hasFolding(curwin->w_topline, ಖಾಲಿ, &old_topline))
		    ಗಣ_topline(curwin, old_topline + 1);
		ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
		    ಗಣ_topline(curwin, curwin->w_topline + 1);
	    }
	}

	// May need to adjust w_topline to show the cursor.
	ಆದರೆ (ಎಣಿಕೆ_2 <= 1)
	    update_topline();

	did_backspace = FALSE;

	ಆದರೆ (ಎಣಿಕೆ_2 <= 1)
	    validate_cursor();		// may set must_redraw

	/*
	 * Redraw the display when no characters are waiting.
	 * Also shows mode, ruler and positions cursor.
	 */
	ins_redraw(TRUE);

	ಆದರೆ (curwin->w_p_scb)
	    do_check_scrollbind(TRUE);

	ಆದರೆ (curwin->w_p_crb)
	    do_check_cursorbind();
	ಆದರೆ (ಎಣಿಕೆ_2 <= 1)
	    update_curswant();
	old_topline = curwin->w_topline;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
	old_topfill = curwin->w_topfill;
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_ON_FLY_SCROLL
	dont_scroll = FALSE;		// allow scrolling here
#ಆದರೆ_ಅಂತ್ಯ
	// May request the keyboard protocol state now.
	may_send_t_RK();

	/*
	 * Get a character for Insert mode.  Ignore K_IGNORE and K_NOP.
	 */
	ಆದರೆ (c != K_CURSORHOLD && c != K_COMPLETE_DELAY)
	    lastc = c;		// remember the previous char for CTRL-D

	// After using CTRL-G U the next cursor key will not break undo.
	ಆದರೆ (dont_sync_undo == MAYBE)
	    dont_sync_undo = TRUE;
	ಇಲ್ಲದಿದ್ದರೆ
	    dont_sync_undo = FALSE;
	ಆದರೆ (cmdchar == K_PS)
	    // Got here from normal mode when bracketed paste started.
	    c = K_PS;
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    // Trigger autocomplete when entering Insert mode, either directly
	    // or via change commands like 'ciw', 'cw', etc., before the first
	    // character is typed.
	    ಆದರೆ (ins_just_started)
	    {
		ins_just_started = FALSE;
		ಆದರೆ (ins_compl_has_autocomplete() && !char_avail()
			&& curwin->w_cursor.ಲಂಬಸಾಲು > 0)
		{
		    c = char_before_cursor();
		    ಆದರೆ (vim_isprintc(c))
		    {
			ins_compl_enable_autocomplete();
			ins_compl_arm_autostart();
			ins_compl_init_get_longest();
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RIGHTLEFT
			ಆದರೆ (p_hkmap)
			    c = hkmap(c);		// Hebrew mode mapping
#ಆದರೆ_ಅಂತ್ಯ
			// Defer until the delay expires (K_COMPLETE_DELAY), or
			// trigger now when no delay is in effect.
			ಆದರೆ (!ins_compl_arm_autocomplete_delay())
			    ಹೋಗು docomplete;
		    }
		}
	    }

	    ಮಾಡು
	    {
		c = ಸುರಕ್ಷಿತ_vgetc();

		ಆದರೆ (stop_ಸೇರಿಸು_ಬಹುಲಕ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMINAL
			|| (c == K_IGNORE && ಪದ_use_loop())
#ಆದರೆ_ಅಂತ್ಯ
		   )
		{
		    // Insert mode ended, possibly from a callback, or a timer
		    // must have opened a terminal window.
		    ಆದರೆ (c != K_IGNORE && c != K_NOP)
			vungetc(c);
		    ಎಣಿಕೆ_2 = 0;

		    ಆದರೆ (!bt_prompt(curwin->w_ಬಫರ್)
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMINAL
			    && !bt_terminal(curwin->w_ಬಫರ್)
#ಆದರೆ_ಅಂತ್ಯ
			    && stop_ಸೇರಿಸು_ಬಹುಲಕ)
			// :stopinsert command via callback or via server command
			nomove = FALSE;
		    ಇಲ್ಲದಿದ್ದರೆ
			nomove = TRUE;
		    ins_compl_prep(ESC);
		    ಹೋಗು doESCkey;
		}
	    } ಆಗ (c == K_IGNORE || c == K_NOP);
	}

	// Don't want K_CURSORHOLD for the second key, e.g., after CTRL-V.
	did_cursorhold = TRUE;
	ಆದರೆ (c != K_CURSORHOLD && c != K_COMPLETE_DELAY)
	{
	    // Don't want delayed autocompletion from the previous key either.
	    ins_compl_clear_autocomplete_delay();
	    ins_compl_disarm_autostart();
	    // A completion already on screen goes on being what it was.
	    ಆದರೆ (!ins_compl_ಸಕ್ರಿಯ())
		ins_compl_disable_autocomplete();
	}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RIGHTLEFT
	ಆದರೆ (p_hkmap && ಕೀಲಿ_Typed)
	    c = hkmap(c);		// Hebrew mode mapping
#ಆದರೆ_ಅಂತ್ಯ

	// If the window was made so small that nothing shows, make it at least
	// one line and one column when typing.
	ಆದರೆ (ಕೀಲಿ_Typed && !ಕೀಲಿ_Stuffed)
	    win_ensure_ಗಾತ್ರ();

	/*
	 * Special handling of keys while the popup menu is visible or wanted
	 * and the cursor is still in the completed word.  Only when there is
	 * a match, skip this when no matches were found.
	 */
	ಆದರೆ (ins_compl_ಸಕ್ರಿಯ() && curwin->w_cursor.ಲಂಬಸಾಲು >= ins_compl_ಲಂಬಸಾಲು()
		&& ins_compl_has_shown_ಹೊಂದಿಕೆ() && pum_wanted())
	{
	    // BS: Delete one character from "compl_leader".
	    ಆದರೆ ((c == K_BS || c == Ctrl_H)
			&& curwin->w_cursor.ಲಂಬಸಾಲು > ins_compl_ಲಂಬಸಾಲು()
			&& (c = ins_compl_bs()) == NUL)
		ಮುಂದುವರಿ;

	    // When no match was selected or it was edited.
	    ಆದರೆ (!ins_compl_ಬಳಸಿದ_ಹೊಂದಿಕೆ())
	    {
		// CTRL-L: Add one character from the current match to
		// "compl_leader".  Except when at the original match and
		// there is nothing to add, CTRL-L works like CTRL-P then.
		ಆದರೆ (c == Ctrl_L
			&& (!ctrl_x_ಬಹುಲಕ_ಸಾಲು_or_eval()
			    || ins_compl_long_shown_ಹೊಂದಿಕೆ()))
		{
		    ins_compl_addfrommatch();
		    ಮುಂದುವರಿ;
		}

		// A non-white character that fits in with the current
		// completion: Add to "compl_leader".
		ಆದರೆ (ins_compl_accept_char(c))
		{
#ಆದರೆ defined(FEAT_EVAL)
		    // Trigger InsertCharPre.
		    char_u *str = do_ಸೇರಿಸು_char_pre(c);
		    char_u *p;

		    ಆದರೆ (str != ಖಾಲಿ)
		    {
			ಗಾಗಿ (p = str; *p != NUL; MB_PTR_ADV(p))
			    ins_compl_addleader(PTR2CHAR(p));
			vim_free(str);
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
			ins_compl_addleader(c);
		    ಮುಂದುವರಿ;
		}

		// Pressing CTRL-Y selects the current match.  When
		// ins_compl_enter_selects() is set the Enter key does the
		// same.
		ಆದರೆ ((c == Ctrl_Y || (ins_compl_enter_selects()
				    && (c == CAR || c == K_KENTER || c == NL)))
			&& stop_arrow() == OK)
		{
		    ins_compl_delete();
		    ಆದರೆ (ins_compl_preinsert_longest()
			    && !ins_compl_is_ಹೊಂದಿಕೆ_selected())
		    {
			ins_compl_ಸೇರಿಸು(FALSE, TRUE);
			ins_compl_init_get_longest();
			ಮುಂದುವರಿ;
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
			ins_compl_ಸೇರಿಸು(FALSE, FALSE);
		}
		// Delete preinserted text when typing special chars
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (IS_WHITE_NL_OR_NUL(c) && ins_compl_preinsert_effect())
		    ins_compl_delete();
	    }
	}

	// Prepare for or stop CTRL-X mode.  This doesn't do completion, but
	// it does fix up the text when finishing completion.
	ins_compl_init_get_longest();
	ಆದರೆ (ins_compl_prep(c))
	    ಮುಂದುವರಿ;

	// CTRL-\ CTRL-N goes to Normal mode,
	// CTRL-\ CTRL-G goes to mode selected with 'insertmode',
	// CTRL-\ CTRL-O is like CTRL-O but without moving the cursor.
	ಆದರೆ (c == Ctrl_BSL)
	{
	    // may need to redraw when no more chars available now
	    ins_redraw(FALSE);
	    ++no_mapping;
	    ++allow_ಕೀಲಿಗಳು;
	    c = plain_vgetc();
	    --no_mapping;
	    --allow_ಕೀಲಿಗಳು;
	    ಆದರೆ (c != Ctrl_N && c != Ctrl_G && c != Ctrl_O)
	    {
		// it's something else
		vungetc(c);
		c = Ctrl_BSL;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c == Ctrl_G && p_im)
		ಮುಂದುವರಿ;
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		ಆದರೆ (c == Ctrl_O)
		{
		    ins_ctrl_o();
		    ins_at_eol = FALSE;	// cursor keeps its column
		    nomove = TRUE;
		}
		ಎಣಿಕೆ_2 = 0;
		ಹೋಗು doESCkey;
	    }
	}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIGRAPHS
	c = do_digraph(c);
#ಆದರೆ_ಅಂತ್ಯ

	ಆದರೆ ((c == Ctrl_V || c == Ctrl_Q) && ctrl_x_ಬಹುಲಕ_cmdline())
	    ಹೋಗು docomplete;
	ಆದರೆ (c == Ctrl_V || c == Ctrl_Q)
	{
	    ins_ctrl_v();
	    c = Ctrl_V;	// pretend CTRL-V is last typed character
	    ಮುಂದುವರಿ;
	}

	ಆದರೆ (cindent_on() && ctrl_x_ಬಹುಲಕ_none())
	{
	    // A key name preceded by a bang means this key is not to be
	    // inserted.  Skip ahead to the re-indenting below.
	    // A key name preceded by a star means that indenting has to be
	    // done before inserting the key.
	    ಸಾಲು_is_white = inindent(0);
	    ಆದರೆ (ಒಳ_cinkeys(c, '!', ಸಾಲು_is_white))
		ಹೋಗು force_cindent;
	    ಆದರೆ (can_cindent && ಒಳ_cinkeys(c, '*', ಸಾಲು_is_white)
							&& stop_arrow() == OK)
		do_c_ಅಭಿವ್ಯಕ್ತಿ_indent();
	}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RIGHTLEFT
	ಆದರೆ (curwin->w_p_rl)
	    ಆಯ್ಕೆ (c)
	    {
		ಸ್ಥಿತಿ K_ಎಡ:	c = K_ಬಲ; ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ K_S_ಎಡ:	c = K_S_ಬಲ; ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ K_C_ಎಡ:	c = K_C_ಬಲ; ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ K_ಬಲ:	c = K_ಎಡ; ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ K_S_ಬಲ: c = K_S_ಎಡ; ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ K_C_ಬಲ: c = K_C_ಎಡ; ನಿಲ್ಲಿಸು;
	    }
#ಆದರೆ_ಅಂತ್ಯ

	/*
	 * If 'keymodel' contains "startsel", may start selection.  If it
	 * does, a CTRL-O and c will be stuffed, we need to get these
	 * characters.
	 */
	ಆದರೆ (ins_ಆರಂಭ_select(c))
	    ಮುಂದುವರಿ;

	/*
	 * The big switch to handle a character in insert mode.
	 */
	ಆಯ್ಕೆ (c)
	{
	ಸ್ಥಿತಿ ESC:	// End input mode
	    ಆದರೆ (echeck_abbr(ESC + ABBR_OFF))
		ನಿಲ್ಲಿಸು;
	    // FALLTHROUGH

	ಸ್ಥಿತಿ Ctrl_C:	// End input mode
	    ಆದರೆ (c == Ctrl_C && cmdwin_type != 0)
	    {
		// Close the cmdline window.
		cmdwin_ಫಲಿತಾಂಶ = K_IGNORE;
		got_int = FALSE; // don't stop executing autocommands et al.
		nomove = TRUE;
		ಹೋಗು doESCkey;
	    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಕೆಲಸ_CHANNEL
	    ಆದರೆ (c == Ctrl_C && bt_prompt(curbuf))
	    {
		ಆದರೆ (invoke_prompt_ಅಡಚಣೆ())
		{
		    ಆದರೆ (!bt_prompt(curbuf))
			// buffer changed to a non-prompt buffer, get out of
			// Insert mode
			ಹೋಗು doESCkey;
		    ನಿಲ್ಲಿಸು;
		}
	    }
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UNIX
do_intr:
#ಆದರೆ_ಅಂತ್ಯ
	    // when 'insertmode' set, and not halfway a mapping, don't leave
	    // Insert mode
	    ಆದರೆ (goto_im())
	    {
		ಆದರೆ (got_int)
		{
		    (ಶೂನ್ಯ)vgetc();		// flush all buffers
		    got_int = FALSE;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		    vim_beep(BO_IM);
		ನಿಲ್ಲಿಸು;
	    }
doESCkey:
	    // Drop a pending autocomplete so it does not outlive Insert mode.
	    ins_compl_clear_autocomplete_delay();
	    /*
	     * This is the ONLY return from edit()!
	     */
	    // Always update o_lnum, so that a "CTRL-O ." that adds a line
	    // still puts the cursor back after the inserted text.
	    ಆದರೆ (ins_at_eol && gchar_cursor() == NUL)
		o_lnum = curwin->w_cursor.lnum;

	    ಆದರೆ (ins_esc(&ಎಣಿಕೆ_2, cmdchar, nomove))
	    {
		// When CTRL-C was typed got_int will be set, with the result
		// that the autocommands won't be executed. When mapped got_int
		// is not set, but let's keep the behavior the same.
		ಆದರೆ (cmdchar != 'r' && cmdchar != 'v' && c != Ctrl_C)
		    ins_apply_autocmds(EVENT_INSERTLEAVE);
		did_cursorhold = FALSE;

		// ins_redraw() triggers TextChangedI only when no characters
		// are in the typeahead buffer, so reset curbuf->b_last_changedtick only
		// if the TextChangedI was not blocked by char_avail() (e.g. using :norm!)
		// and the TextChangedI autocommand has been triggered.
		ಆದರೆ (!char_avail() && curbuf->b_ಕೊನೆಯ_changedtick_i == CHANGEDTICK(curbuf))
		    curbuf->b_ಕೊನೆಯ_changedtick = CHANGEDTICK(curbuf);
		ಹಿಂತಿರುಗಿ (c == Ctrl_O);
	    }
	    ಮುಂದುವರಿ;

	ಸ್ಥಿತಿ Ctrl_Z:	// suspend when 'insertmode' set
	    ಆದರೆ (!p_im)
		ಹೋಗು normalchar;	// insert CTRL-Z as normal char
	    do_cmdline_cmd((char_u *)"stop");
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURSOR_SHAPE
	    ui_cursor_shape();		// may need to update cursor shape
#ಆದರೆ_ಅಂತ್ಯ
	    ಮುಂದುವರಿ;

	ಸ್ಥಿತಿ Ctrl_O:	// execute one command
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_COMPL_FUNC
	    ಆದರೆ (ctrl_x_ಬಹುಲಕ_omni())
		ಹೋಗು docomplete;
#ಆದರೆ_ಅಂತ್ಯ
	    ಆದರೆ (echeck_abbr(Ctrl_O + ABBR_OFF))
		ನಿಲ್ಲಿಸು;
	    ins_ctrl_o();

	    // don't move the cursor left when 'virtualedit' has "onemore".
	    ಆದರೆ (get_ve_flags() & VE_ONEMORE)
	    {
		ins_at_eol = FALSE;
		nomove = TRUE;
	    }
	    ಎಣಿಕೆ_2 = 0;
	    ಹೋಗು doESCkey;

	ಸ್ಥಿತಿ K_INS:	// toggle insert/replace mode
	ಸ್ಥಿತಿ K_KINS:
	    ins_ಸೇರಿಸು(replaceState);
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ K_SELECT:	// end of Select mode mapping - ignore
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ K_HELP:	// Help key works like <ESC> <Help>
	ಸ್ಥಿತಿ K_F1:
	ಸ್ಥಿತಿ K_XF1:
	    stuffcharReadbuff(K_HELP);
	    ಆದರೆ (p_im)
		need_ಆರಂಭ_insertmode = TRUE;
	    ಹೋಗು doESCkey;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_NETBEANS_INTG
	ಸ್ಥಿತಿ K_F21:	// NetBeans command
	    ++no_mapping;		// don't map the next key hits
	    i = plain_vgetc();
	    --no_mapping;
	    netbeans_keycommand(i);
	    ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ

	ಸ್ಥಿತಿ K_ZERO:	// Insert the previously inserted text.
	ಸ್ಥಿತಿ NUL:
	ಸ್ಥಿತಿ Ctrl_A:
	    // For ^@ the trailing ESC will end the insert, unless there is an
	    // error.
	    ಆದರೆ (stuff_inserted(NUL, 1L, (c == Ctrl_A)) == FAIL
						   && c != Ctrl_A && !p_im)
		ಹೋಗು doESCkey;		// quit insert mode
	    inserted_space = FALSE;
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ Ctrl_R:	// insert the contents of a register
	    ಆದರೆ (ctrl_x_ಬಹುಲಕ_ರಿಜಿಸ್ಟರ್() && !ins_compl_ಸಕ್ರಿಯ())
		ಹೋಗು docomplete;
	    ins_reg();
	    auto_format(FALSE, TRUE);
	    inserted_space = FALSE;
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ Ctrl_G:	// commands starting with CTRL-G
	    ins_ctrl_g();
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ Ctrl_HAT:	// switch input mode and/or langmap
	    ins_ctrl_hat();
	    ನಿಲ್ಲಿಸು;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RIGHTLEFT
	ಸ್ಥಿತಿ Ctrl__:	// switch between languages
	    ಆದರೆ (!p_ari)
		ಹೋಗು normalchar;
	    ins_ctrl_();
	    ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ

	ಸ್ಥಿತಿ Ctrl_D:	// Make indent one shiftwidth smaller.
#ಆದರೆ defined(FEAT_ಕಂಡುಹಿಡಿ_ID)
	    ಆದರೆ (ctrl_x_ಬಹುಲಕ_ಮಾರ್ಗ_ನಿರೂಪಿಸುತ್ತದೆ())
		ಹೋಗು docomplete;
#ಆದರೆ_ಅಂತ್ಯ
	    // FALLTHROUGH

	ಸ್ಥಿತಿ Ctrl_T:	// Make indent one shiftwidth greater.
	    ಆದರೆ (c == Ctrl_T && ctrl_x_ಬಹುಲಕ_thesaurus())
	    {
		ಆದರೆ (has_compl_option(FALSE))
		    ಹೋಗು docomplete;
		ನಿಲ್ಲಿಸು;
	    }

	    ins_ಸರಿಸು(c, lastc);
	    auto_format(FALSE, TRUE);
	    inserted_space = FALSE;
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ K_DEL:	// delete character under the cursor
	ಸ್ಥಿತಿ K_KDEL:
	    ins_del();
	    auto_format(FALSE, TRUE);
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ K_BS:	// delete character before the cursor
	ಸ್ಥಿತಿ K_S_BS:
	ಸ್ಥಿತಿ Ctrl_H:
	    did_backspace = ins_bs(c, BACKSPACE_CHAR, &inserted_space);
	    auto_format(FALSE, TRUE);
	    ಆದರೆ (did_backspace)
		MAY_TRIGGER_AUTOCOMPLETE(c);
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ Ctrl_W:	// delete word before the cursor
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಕೆಲಸ_CHANNEL
	    ಆದರೆ (bt_prompt(curbuf) && (mod_ಮುಖವಾಡ & MOD_ಮುಖವಾಡ_ಸರಿಸು) == 0)
	    {
		// In a prompt window CTRL-W is used for window commands.
		// Use Shift-CTRL-W to delete a word.
		stuffcharReadbuff(Ctrl_W);
		restart_edit = 'A';
		nomove = TRUE;
		ಎಣಿಕೆ_2 = 0;
		ಹೋಗು doESCkey;
	    }
#ಆದರೆ_ಅಂತ್ಯ
	    did_backspace = ins_bs(c, BACKSPACE_ಶಬ್ದ, &inserted_space);
	    auto_format(FALSE, TRUE);
	    ಆದರೆ (did_backspace)
		MAY_TRIGGER_AUTOCOMPLETE(c);
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ Ctrl_U:	// delete all inserted text in current line
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_COMPL_FUNC
	    // CTRL-X CTRL-U completes with 'completefunc'.
	    ಆದರೆ (ctrl_x_ಬಹುಲಕ_function())
		ಹೋಗು docomplete;
#ಆದರೆ_ಅಂತ್ಯ
	    did_backspace = ins_bs(c, BACKSPACE_ಸಾಲು, &inserted_space);
	    auto_format(FALSE, TRUE);
	    inserted_space = FALSE;
	    ಆದರೆ (did_backspace)
		MAY_TRIGGER_AUTOCOMPLETE(c);
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ K_LEFTMOUSE:   // mouse keys
	ಸ್ಥಿತಿ K_LEFTMOUSE_NM:
	ಸ್ಥಿತಿ K_LEFTDRAG:
	ಸ್ಥಿತಿ K_LEFTRELEASE:
	ಸ್ಥಿತಿ K_LEFTRELEASE_NM:
	ಸ್ಥಿತಿ K_MOUSEMOVE:
	ಸ್ಥಿತಿ K_MIDDLEMOUSE:
	ಸ್ಥಿತಿ K_MIDDLEDRAG:
	ಸ್ಥಿತಿ K_MIDDLERELEASE:
	ಸ್ಥಿತಿ K_RIGHTMOUSE:
	ಸ್ಥಿತಿ K_RIGHTDRAG:
	ಸ್ಥಿತಿ K_RIGHTRELEASE:
	ಸ್ಥಿತಿ K_X1MOUSE:
	ಸ್ಥಿತಿ K_X1DRAG:
	ಸ್ಥಿತಿ K_X1RELEASE:
	ಸ್ಥಿತಿ K_X2MOUSE:
	ಸ್ಥಿತಿ K_X2DRAG:
	ಸ್ಥಿತಿ K_X2RELEASE:
	    ins_mouse(c);
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ K_MOUSEDOWN: // Default action for scroll wheel up: scroll up
	    ins_mousescroll(MSCR_ಕೆಳಕ್ಕೆ);
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ K_MOUSEUP:	// Default action for scroll wheel down: scroll down
	    ins_mousescroll(MSCR_ಮೇಲೆ);
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ K_MOUSELEFT: // Scroll wheel left
	    ins_mousescroll(MSCR_ಎಡ);
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ K_MOUSERIGHT: // Scroll wheel right
	    ins_mousescroll(MSCR_ಬಲ);
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ K_PS:
	    bracketed_paste(PASTE_ಸೇರಿಸು, FALSE, ಖಾಲಿ);
	    ಆದರೆ (cmdchar == K_PS)
		// invoked from normal mode, bail out
		ಹೋಗು doESCkey;
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ K_PE:
	    // Got K_PE without K_PS, ignore.
	    ನಿಲ್ಲಿಸು;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_TABLINE
	ಸ್ಥಿತಿ K_TABLINE:
	ಸ್ಥಿತಿ K_TABMENU:
	    ins_tabline(c);
	    ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ

	ಸ್ಥಿತಿ K_IGNORE:	// Something mapped to nothing
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ K_COMMAND:		    // <Cmd>command<CR>
	ಸ್ಥಿತಿ K_SCRIPT_COMMAND:	    // <ScriptCmd>command<CR>
	    {
		bufref_T    save_curbuf;
		varnumber_T tick = CHANGEDTICK(curbuf);

		ಗಣ_bufref(&save_curbuf, curbuf);
		do_cmdkey_command(c, 0);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMINAL
		ಆದರೆ (ಪದ_use_loop())
		    // Started a terminal that gets the input, exit Insert mode.
		    ಹೋಗು doESCkey;
#ಆದರೆ_ಅಂತ್ಯ
		ಆದರೆ (curbuf->b_u_synced
			|| (bufref_valid(&save_curbuf)
			    && curbuf == save_curbuf.br_buf
			    && tick != CHANGEDTICK(curbuf)))
		{
		    // The command synced undo or changed this buffer.
		    // Save the cursor line before the next typed edit.
		    ins_need_undo = TRUE;
		}
	    }
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ K_CURSORHOLD:	// Didn't type something for a while.
	    ins_apply_autocmds(EVENT_CURSORHOLDI);
	    did_cursorhold = TRUE;
	    // If CTRL-G U was used apply it to the next typed key.
	    ಆದರೆ (dont_sync_undo == TRUE)
		dont_sync_undo = MAYBE;
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ K_COMPLETE_DELAY:	// 'autocompletedelay' expired
	    // If CTRL-G U was used apply it to the next typed key.
	    ಆದರೆ (dont_sync_undo == TRUE)
		dont_sync_undo = MAYBE;
	    ins_compl_clear_autocomplete_delay();
	    ಆದರೆ (!ins_compl_has_autocomplete() || char_avail()
		    || curwin->w_cursor.ಲಂಬಸಾಲು == 0)
		ನಿಲ್ಲಿಸು;
	    c = char_before_cursor();
	    ಆದರೆ (!vim_isprintc(c))
		ನಿಲ್ಲಿಸು;
	    // The completion may have been cleared while waiting, so re-enable
	    // autocomplete to match a zero delay.
	    ins_compl_enable_autocomplete();
	    ins_compl_arm_autostart();
	    ಹೋಗು docomplete;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_MSWIN
	    // On MS-Windows ignore <M-F4>, we get it when closing the window
	    // was cancelled.
	ಸ್ಥಿತಿ K_F4:
	    ಆದರೆ (mod_ಮುಖವಾಡ != MOD_ಮುಖವಾಡ_ALT)
		ಹೋಗು normalchar;
	    ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
	ಸ್ಥಿತಿ K_VER_SCROLLBAR:
	    ins_scroll();
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ K_HOR_SCROLLBAR:
	    ins_horscroll();
	    ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ

	ಸ್ಥಿತಿ K_HOME:	// <Home>
	ಸ್ಥಿತಿ K_KHOME:
	ಸ್ಥಿತಿ K_S_HOME:
	ಸ್ಥಿತಿ K_C_HOME:
	    ins_home(c);
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ K_ಕೊನೆ:	// <End>
	ಸ್ಥಿತಿ K_KEND:
	ಸ್ಥಿತಿ K_S_ಕೊನೆ:
	ಸ್ಥಿತಿ K_C_ಕೊನೆ:
	    ins_ಕೊನೆ(c);
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ K_ಎಡ:	// <Left>
	    ಆದರೆ (mod_ಮುಖವಾಡ & (MOD_ಮುಖವಾಡ_ಸರಿಸು|MOD_ಮುಖವಾಡ_CTRL))
		ins_s_ಎಡ();
	    ಇಲ್ಲದಿದ್ದರೆ
		ins_ಎಡ();
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ K_S_ಎಡ:	// <S-Left>
	ಸ್ಥಿತಿ K_C_ಎಡ:
	    ins_s_ಎಡ();
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ K_ಬಲ:	// <Right>
	    ಆದರೆ (mod_ಮುಖವಾಡ & (MOD_ಮುಖವಾಡ_ಸರಿಸು|MOD_ಮುಖವಾಡ_CTRL))
		ins_s_ಬಲ();
	    ಇಲ್ಲದಿದ್ದರೆ
		ins_ಬಲ();
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ K_S_ಬಲ:	// <S-Right>
	ಸ್ಥಿತಿ K_C_ಬಲ:
	    ins_s_ಬಲ();
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ K_ಮೇಲೆ:	// <Up>
	    ಆದರೆ (pum_visible())
		ಹೋಗು docomplete;
	    ಆದರೆ (mod_ಮುಖವಾಡ & MOD_ಮುಖವಾಡ_ಸರಿಸು)
		ins_pageup();
	    ಇಲ್ಲದಿದ್ದರೆ
		ins_ಮೇಲೆ(FALSE);
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ K_S_ಮೇಲೆ:	// <S-Up>
	ಸ್ಥಿತಿ K_PAGEUP:
	ಸ್ಥಿತಿ K_KPAGEUP:
	    ಆದರೆ (pum_visible())
	    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
		// CTRL-SHIFT-<Up> scrolls the info popup up a line,
		// CTRL-SHIFT-<PageUp> a page.  Shift is folded into K_S_UP but
		// stays in mod_mask for PageUp, hence the asymmetric check.
		ಆದರೆ (c == K_S_ಮೇಲೆ ? (mod_ಮುಖವಾಡ & MOD_ಮುಖವಾಡ_CTRL)
			: ((mod_ಮುಖವಾಡ & MOD_ಮುಖವಾಡ_CTRL)
			    && (mod_ಮುಖವಾಡ & MOD_ಮುಖವಾಡ_ಸರಿಸು)))
		{
		    popup_scroll_info(-1, c != K_S_ಮೇಲೆ);
		    ನಿಲ್ಲಿಸು;
		}
#ಆದರೆ_ಅಂತ್ಯ
		ಹೋಗು docomplete;
	    }
	    ins_pageup();
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ K_ಕೆಳಕ್ಕೆ:	// <Down>
	    ಆದರೆ (pum_visible())
		ಹೋಗು docomplete;
	    ಆದರೆ (mod_ಮುಖವಾಡ & MOD_ಮುಖವಾಡ_ಸರಿಸು)
		ins_pagedown();
	    ಇಲ್ಲದಿದ್ದರೆ
		ins_ಕೆಳಕ್ಕೆ(FALSE);
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ K_S_ಕೆಳಕ್ಕೆ:	// <S-Down>
	ಸ್ಥಿತಿ K_PAGEDOWN:
	ಸ್ಥಿತಿ K_KPAGEDOWN:
	    ಆದರೆ (pum_visible())
	    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
		// CTRL-SHIFT-<Down>/<PageDown> scroll the info popup down.
		ಆದರೆ (c == K_S_ಕೆಳಕ್ಕೆ ? (mod_ಮುಖವಾಡ & MOD_ಮುಖವಾಡ_CTRL)
			: ((mod_ಮುಖವಾಡ & MOD_ಮುಖವಾಡ_CTRL)
			    && (mod_ಮುಖವಾಡ & MOD_ಮುಖವಾಡ_ಸರಿಸು)))
		{
		    popup_scroll_info(1, c != K_S_ಕೆಳಕ್ಕೆ);
		    ನಿಲ್ಲಿಸು;
		}
#ಆದರೆ_ಅಂತ್ಯ
		ಹೋಗು docomplete;
	    }
	    ins_pagedown();
	    ನಿಲ್ಲಿಸು;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DND
	ಸ್ಥಿತಿ K_DROP:	// drag-n-drop event
	    ins_drop();
	    ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ

	ಸ್ಥಿತಿ K_S_TAB:	// When not mapped, use like a normal TAB
	    c = TAB;
	    // FALLTHROUGH

	ಸ್ಥಿತಿ TAB:	// TAB or Complete patterns along path
#ಆದರೆ defined(FEAT_ಕಂಡುಹಿಡಿ_ID)
	    ಆದರೆ (ctrl_x_ಬಹುಲಕ_ಮಾರ್ಗ_patterns())
		ಹೋಗು docomplete;
#ಆದರೆ_ಅಂತ್ಯ
	    inserted_space = FALSE;
	    ಆದರೆ (ins_tab())
		ಹೋಗು normalchar;	// insert TAB as a normal char
	    auto_format(FALSE, TRUE);
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ K_KENTER:	// <Enter>
	    c = CAR;
	    // FALLTHROUGH
	ಸ್ಥಿತಿ CAR:
	ಸ್ಥಿತಿ NL:
#ಆದರೆ defined(FEAT_QUICKFIX)
	    // In a quickfix window a <CR> jumps to the error under the
	    // cursor.
	    ಆದರೆ (bt_quickfix(curbuf) && c == CAR)
	    {
		ಆದರೆ (curwin->w_llist_ref == ಖಾಲಿ)    // quickfix window
		    do_cmdline_cmd((char_u *)".cc");
		ಇಲ್ಲದಿದ್ದರೆ				    // location list window
		    do_cmdline_cmd((char_u *)".ll");
		ನಿಲ್ಲಿಸು;
	    }
#ಆದರೆ_ಅಂತ್ಯ
	    ಆದರೆ (cmdwin_type != 0)
	    {
		// Execute the command in the cmdline window.
		cmdwin_ಫಲಿತಾಂಶ = CAR;
		ಹೋಗು doESCkey;
	    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಕೆಲಸ_CHANNEL
	    ಆದರೆ (bt_prompt(curbuf))
	    {
		invoke_prompt_callback();
		ಆದರೆ (!bt_prompt(curbuf))
		    // buffer changed to a non-prompt buffer, get out of
		    // Insert mode
		    ಹೋಗು doESCkey;
		ನಿಲ್ಲಿಸು;
	    }
#ಆದರೆ_ಅಂತ್ಯ
	    ಆದರೆ (ins_eol(c) == FAIL && !p_im)
		ಹೋಗು doESCkey;	    // out of memory
	    auto_format(FALSE, FALSE);
	    inserted_space = FALSE;
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ Ctrl_K:	    // digraph or keyword completion
	    ಆದರೆ (ctrl_x_ಬಹುಲಕ_dictionary())
	    {
		ಆದರೆ (has_compl_option(TRUE))
		    ಹೋಗು docomplete;
		ನಿಲ್ಲಿಸು;
	    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIGRAPHS
	    c = ins_digraph();
	    ಆದರೆ (c == NUL)
		ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
	    ಹೋಗು normalchar;

	ಸ್ಥಿತಿ Ctrl_X:	// Enter CTRL-X mode
	    ins_ctrl_x();
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ Ctrl_RSB:	// Tag name completion after ^X
	    ಆದರೆ (!ctrl_x_ಬಹುಲಕ_ಟ್ಯಾಗುಗಳು())
		ಹೋಗು normalchar;
	    ಹೋಗು docomplete;

	ಸ್ಥಿತಿ Ctrl_F:	// File name completion after ^X
	    ಆದರೆ (!ctrl_x_ಬಹುಲಕ_files())
		ಹೋಗು normalchar;
	    ಹೋಗು docomplete;

	ಸ್ಥಿತಿ 's':	// Spelling completion after ^X
	ಸ್ಥಿತಿ Ctrl_S:
	    ಆದರೆ (!ctrl_x_ಬಹುಲಕ_spell())
		ಹೋಗು normalchar;
	    ಹೋಗು docomplete;

	ಸ್ಥಿತಿ Ctrl_L:	// Whole line completion after ^X
	    ಆದರೆ (!ctrl_x_ಬಹುಲಕ_ಸಂಪೂರ್ಣ_ಸಾಲು())
	    {
		// CTRL-L with 'insertmode' set: Leave Insert mode
		ಆದರೆ (p_im)
		{
		    ಆದರೆ (echeck_abbr(Ctrl_L + ABBR_OFF))
			ನಿಲ್ಲಿಸು;
		    ಹೋಗು doESCkey;
		}
		ಹೋಗು normalchar;
	    }
	    // FALLTHROUGH

	ಸ್ಥಿತಿ Ctrl_P:	// Do previous/next pattern completion
	ಸ್ಥಿತಿ Ctrl_N:
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
	    // CTRL-SHIFT-P/N scroll the info popup one line.
	    ಆದರೆ (pum_visible() && (mod_ಮುಖವಾಡ & MOD_ಮುಖವಾಡ_ಸರಿಸು)
		    && (c == Ctrl_P || c == Ctrl_N))
	    {
		popup_scroll_info(c == Ctrl_P ? -1 : 1, false);
		ನಿಲ್ಲಿಸು;
	    }
#ಆದರೆ_ಅಂತ್ಯ
	    // if 'complete' is empty then plain ^P is no longer special,
	    // but it is under other ^X modes
	    ಆದರೆ (*curbuf->b_p_cpt == NUL
		    && (ctrl_x_ಬಹುಲಕ_ಸಾಮಾನ್ಯ() || ctrl_x_ಬಹುಲಕ_ಸಂಪೂರ್ಣ_ಸಾಲು())
		    && !compl_ಸ್ಥಿತಿ_local())
		ಹೋಗು normalchar;

docomplete:
	    ins_compl_clear_autocomplete_delay();
	    compl_busy = TRUE;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
	    disable_fold_update++;  // don't redraw folds here
#ಆದರೆ_ಅಂತ್ಯ
	    ಆದರೆ (ins_complete(c, TRUE) == FAIL)
		compl_ಸ್ಥಿತಿ_clear();
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
	    disable_fold_update--;
#ಆದರೆ_ಅಂತ್ಯ
	    compl_busy = FALSE;
	    can_si = may_do_si(); // allow smartindenting
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ Ctrl_Y:	// copy from previous line or scroll down
	ಸ್ಥಿತಿ Ctrl_E:	// copy from next line	   or scroll up
	    c = ins_ctrl_ey(c);
	    ನಿಲ್ಲಿಸು;

	ಪೂರ್ವನಿಯೋಜಿತ:
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UNIX
	    ಆದರೆ (c == intr_char)		// special interrupt char
		ಹೋಗು do_intr;
#ಆದರೆ_ಅಂತ್ಯ

normalchar:
	    /*
	     * Insert a normal character.
	     */
#ಆದರೆ defined(FEAT_EVAL)
	    ಆದರೆ (!p_paste)
	    {
		// Trigger InsertCharPre.
		char_u *str = do_ಸೇರಿಸು_char_pre(c);
		char_u *p;

		ಆದರೆ (str != ಖಾಲಿ)
		{
		    ಆದರೆ (*str != NUL && stop_arrow() != FAIL)
		    {
			// Insert the new value of v:char literally.
			ಗಾಗಿ (p = str; *p != NUL; MB_PTR_ADV(p))
			{
			    c = PTR2CHAR(p);
			    ಆದರೆ (c == CAR || c == K_KENTER || c == NL)
				ins_eol(c);
			    ಇಲ್ಲದಿದ್ದರೆ
				ins_char(c);
			}
			Append_ಗಮ್ಯ_Redobuff_Lit(str, -1);
		    }
		    vim_free(str);
		    c = NUL;
		}

		// If the new value is already inserted or an empty string
		// then don't insert any character.
		ಆದರೆ (c == NUL)
		    ನಿಲ್ಲಿಸು;
	    }
#ಆದರೆ_ಅಂತ್ಯ
	    // Try to perform smart-indenting.
	    ins_try_si(c);

	    ಆದರೆ (c == ' ')
	    {
		inserted_space = TRUE;
		ಆದರೆ (inindent(0))
		    can_cindent = FALSE;
		ಆದರೆ (Insstart_blank_vcol == MAXCOL
			&& curwin->w_cursor.lnum == Insstart.lnum)
		    Insstart_blank_vcol = get_nolist_virtcol();
	    }

	    // Insert a normal character and check for abbreviations on a
	    // special character.  Let CTRL-] expand abbreviations without
	    // inserting it.
	    ಆದರೆ (vim_iswordc(c) || (!echeck_abbr(
			// Add ABBR_OFF for characters above 0x100, this is
			// what check_abbr() expects.
				(has_mbyte && c >= 0x100) ? (c + ABBR_OFF) : c)
			&& c != Ctrl_RSB))
	    {
		ಸೇರಿಸು_special(c, FALSE, FALSE);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RIGHTLEFT
		revins_legal++;
		revins_chars++;
#ಆದರೆ_ಅಂತ್ಯ
	    }

	    auto_format(FALSE, TRUE);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
	    // When inserting a character the cursor line must never be in a
	    // closed fold.
	    foldOpenCursor();
#ಆದರೆ_ಅಂತ್ಯ
	    // Trigger autocompletion
	    ಆದರೆ (ins_compl_has_autocomplete() && !char_avail()
		    && vim_isprintc(c))
		TRIGGER_AUTOCOMPLETE();

	    ನಿಲ್ಲಿಸು;
	}   // end of switch (c)

	// If typed something may trigger CursorHoldI again; K_COMPLETE_DELAY is
	// injected, not typed.
	ಆದರೆ (c != K_CURSORHOLD && c != K_COMPLETE_DELAY
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_COMPL_FUNC
		// but not in CTRL-X mode, a script can't restore the state
		&& ctrl_x_ಬಹುಲಕ_ಸಾಮಾನ್ಯ()
#ಆದರೆ_ಅಂತ್ಯ
	       )
	    did_cursorhold = FALSE;

	// Check if we need to cancel completion mode because the window
	// or tab page was changed
	ಆದರೆ (ins_compl_ಸಕ್ರಿಯ() && !ins_compl_win_ಸಕ್ರಿಯ(curwin))
	    ins_compl_cancel();

	// If the cursor was moved we didn't just insert a space
	ಆದರೆ (arrow_ಬಳಸಿದ)
	    inserted_space = FALSE;

	ಆದರೆ (can_cindent && cindent_on() && ctrl_x_ಬಹುಲಕ_ಸಾಮಾನ್ಯ())
	{
force_cindent:
	    /*
	     * Indent now if a key was typed that is in 'cinkeys'.
	     */
	    ಆದರೆ (ಒಳ_cinkeys(c, ' ', ಸಾಲು_is_white))
	    {
		ಆದರೆ (stop_arrow() == OK)
		    // re-indent the current line
		    do_c_ಅಭಿವ್ಯಕ್ತಿ_indent();
	    }
	}

    }	// for (;;)
    // NOTREACHED
}

    ಪೂರ್ಣಾಂಕ
ins_need_undo_get(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ ins_need_undo;
}

/*
 * Redraw for Insert mode.
 * This is postponed until getting the next character to make '$' in the 'cpo'
 * option work correctly.
 * Only redraw when there are no characters available.  This speeds up
 * inserting sequences of characters (e.g., for CTRL-R).
 */
    ಶೂನ್ಯ
ins_redraw(ಪೂರ್ಣಾಂಕ ready)	    // not busy with something
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CONCEAL
    linenr_T	conceal_old_cursor_ಸಾಲು = 0;
    linenr_T	conceal_new_cursor_ಸಾಲು = 0;
    ಪೂರ್ಣಾಂಕ		conceal_update_ಸಾಲುಗಳು = FALSE;
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (char_avail())
	ಹಿಂತಿರುಗಿ;

    // Trigger CursorMoved if the cursor moved.  Not when the popup menu is
    // visible, the command might delete it.
    ಆದರೆ (ready && (has_cursormovedI()
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
		|| popup_visible
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(FEAT_CONCEAL)
		|| curwin->w_p_cole > 0
#ಆದರೆ_ಅಂತ್ಯ
		)
	    && !EQUAL_POS(ಕೊನೆಯ_cursormoved, curwin->w_cursor)
	    && !pum_visible())
    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
	// Need to update the screen first, to make sure syntax
	// highlighting is correct after making a change (e.g., inserting
	// a "(".  The autocommand may also require a redraw, so it's done
	// again below, unfortunately.
	ಆದರೆ (syntax_present(curwin) && must_redraw)
	    update_screen(0);
#ಆದರೆ_ಅಂತ್ಯ
	ಆದರೆ (has_cursormovedI())
	{
	    // Make sure curswant is correct, an autocommand may call
	    // getcurpos().
	    update_curswant();
	    ins_apply_autocmds(EVENT_CURSORMOVEDI);
	}
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
	ಆದರೆ (popup_visible)
	    popup_check_cursor_pos();
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CONCEAL
	ಆದರೆ (curwin->w_p_cole > 0)
	{
	    conceal_old_cursor_ಸಾಲು = ಕೊನೆಯ_cursormoved.lnum;
	    conceal_new_cursor_ಸಾಲು = curwin->w_cursor.lnum;
	    conceal_update_ಸಾಲುಗಳು = TRUE;
	}
#ಆದರೆ_ಅಂತ್ಯ
	ಕೊನೆಯ_cursormoved = curwin->w_cursor;
    }

    // Trigger TextChangedI if b_changedtick_i differs.
    ಆದರೆ (ready && has_textchangedI()
	    && curbuf->b_ಕೊನೆಯ_changedtick_i != CHANGEDTICK(curbuf)
	    && !pum_visible())
    {
	aco_save_T	aco;
	varnumber_T	tick = CHANGEDTICK(curbuf);

	// Save and restore curwin and curbuf, in case the autocmd changes
	// them.
	aucmd_prepbuf(&aco, curbuf);
	apply_autocmds(EVENT_TEXTCHANGEDI, ಖಾಲಿ, ಖಾಲಿ, FALSE, curbuf);
	aucmd_restbuf(&aco);
	curbuf->b_ಕೊನೆಯ_changedtick_i = CHANGEDTICK(curbuf);
	ಆದರೆ (tick != CHANGEDTICK(curbuf))  // see ins_apply_autocmds()
	    u_save(curwin->w_cursor.lnum,
					(linenr_T)(curwin->w_cursor.lnum + 1));
    }

    // Trigger TextChangedP if b_changedtick_pum differs. When the popupmenu
    // closes TextChangedI will need to trigger for backwards compatibility,
    // thus use different b_last_changedtick* variables.
    ಆದರೆ (ready && has_textchangedP()
	    && curbuf->b_ಕೊನೆಯ_changedtick_pum != CHANGEDTICK(curbuf)
	    && pum_visible())
    {
	aco_save_T	aco;
	varnumber_T	tick = CHANGEDTICK(curbuf);

	// Save and restore curwin and curbuf, in case the autocmd changes
	// them.
	aucmd_prepbuf(&aco, curbuf);
	apply_autocmds(EVENT_TEXTCHANGEDP, ಖಾಲಿ, ಖಾಲಿ, FALSE, curbuf);
	aucmd_restbuf(&aco);
	curbuf->b_ಕೊನೆಯ_changedtick_pum = CHANGEDTICK(curbuf);
	ಆದರೆ (tick != CHANGEDTICK(curbuf))  // see ins_apply_autocmds()
	    u_save(curwin->w_cursor.lnum,
					(linenr_T)(curwin->w_cursor.lnum + 1));
    }

    ಆದರೆ (ready)
	may_trigger_win_scrolled_resized();

    // Trigger SafeState if nothing is pending.
    may_trigger_safestate(ready
	    && !ins_compl_ಸಕ್ರಿಯ()
	    && !pum_visible());

#ಆದರೆ defined(FEAT_CONCEAL)
    ಆದರೆ ((conceal_update_ಸಾಲುಗಳು
	    && (conceal_old_cursor_ಸಾಲು != conceal_new_cursor_ಸಾಲು
		|| conceal_cursor_ಸಾಲು(curwin)))
	    || need_cursor_ಸಾಲು_redraw)
    {
	ಆದರೆ (conceal_old_cursor_ಸಾಲು != conceal_new_cursor_ಸಾಲು)
	    redrawWinline(curwin, conceal_old_cursor_ಸಾಲು);
	redrawWinline(curwin, conceal_new_cursor_ಸಾಲು == 0
			    ? curwin->w_cursor.lnum : conceal_new_cursor_ಸಾಲು);
	curwin->w_valid &= ~VALID_CROW;
	need_cursor_ಸಾಲು_redraw = FALSE;
    }
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ (must_redraw)
	update_screen(0);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (clear_cmdline || redraw_cmdline)
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
    ಪೂರ್ಣಾಂಕ		c;
    ಪೂರ್ಣಾಂಕ		did_putchar = FALSE;

    // may need to redraw when no more chars available now
    ins_redraw(FALSE);

    ಆದರೆ (redrawing() && !char_avail())
    {
	edit_putchar('^', TRUE);
	did_putchar = TRUE;
    }
    Append_ಗಮ್ಯ_Redobuff((char_u *)CTRL_V_STR);	// CTRL-V

    ಕೂಡಿಸು_ಗಮ್ಯ_showcmd_c(Ctrl_V);

    // Do not change any modifyOtherKeys ESC sequence to a normal key for
    // CTRL-SHIFT-V.
    c = get_literal(mod_ಮುಖವಾಡ & MOD_ಮುಖವಾಡ_ಸರಿಸು);
    ಆದರೆ (did_putchar)
	// when the line fits in 'columns' the '^' is at the start of the next
	// line and will not removed by the redraw
	edit_unputchar();
    clear_showcmd();

    ಸೇರಿಸು_special(c, FALSE, TRUE);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RIGHTLEFT
    revins_chars++;
    revins_legal++;
#ಆದರೆ_ಅಂತ್ಯ
}

/*
 * After getting an ESC or CSI for a literal key: If the typeahead buffer
 * contains a modifyOtherKeys sequence then decode it and return the result.
 * Otherwise return "c".
 * Note that this doesn't wait for characters, they must be in the typeahead
 * buffer already.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
decode_Modify_Other_ಕೀಲಿಗಳು(ಪೂರ್ಣಾಂಕ c)
{
    char_u  *p = typebuf.tb_buf + typebuf.tb_off;
    ಪೂರ್ಣಾಂಕ	    idx;
    ಪೂರ್ಣಾಂಕ	    form = 0;
    ಪೂರ್ಣಾಂಕ	    argidx = 0;
    ಪೂರ್ಣಾಂಕ	    arg[2] = {0, 0};

    // Recognize:
    // form 0: {lead}{key};{modifier}u
    // form 1: {lead}27;{modifier};{key}~
    ಆದರೆ (typebuf.tb_len >= 4 && (c == CSI || (c == ESC && *p == '[')))
    {
	idx = (*p == '[');
	ಆದರೆ (p[idx] == '2' && p[idx + 1] == '7' && p[idx + 2] == ';' &&
		kitty_protocol_state != KKPS_ENABLED)
	{
	    form = 1;
	    idx += 3;
	}
	ಆಗ (idx < typebuf.tb_len && argidx < 2)
	{
	    ಆದರೆ (p[idx] == ';')
		++argidx;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (VIM_ISDIGIT(p[idx]))
		arg[argidx] = arg[argidx] * 10 + (p[idx] - '0');
	    ಇಲ್ಲದಿದ್ದರೆ
		ನಿಲ್ಲಿಸು;
	    ++idx;
	}
	ಪೂರ್ಣಾಂಕ kitty_no_mods = argidx == 0 && kitty_protocol_state == KKPS_ENABLED;
	ಆದರೆ (idx < typebuf.tb_len
		&& p[idx] == (form == 1 ? '~' : 'u')
		&& (argidx == 1 || kitty_no_mods))
	{
	    // Match, consume the code.
	    typebuf.tb_off += idx + 1;
	    typebuf.tb_len -= idx + 1;
#ಆದರೆ defined(FEAT_CLIENTSERVER) || defined(FEAT_EVAL)
	    ಆದರೆ (typebuf.tb_len == 0)
		typebuf_was_filled = FALSE;
#ಆದರೆ_ಅಂತ್ಯ

	    mod_ಮುಖವಾಡ = kitty_no_mods ? 0 : decode_modifiers(arg[!form]);
	    c = ವಿಲೀನ_modify_Other_ಕೀಲಿಗಳು(arg[form], &mod_ಮುಖವಾಡ);
	}
    }

    ಹಿಂತಿರುಗಿ c;
}

/*
 * Put a character directly onto the screen.  It's not stored in a buffer.
 * Used while handling CTRL-K, CTRL-V, etc. in Insert mode.
 */
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ  pc_ಸ್ಥಿತಿ;
#ವ್ಯಾಖ್ಯಾನಿಸು PC_ಸ್ಥಿತಿ_UNSET	0	// pc_bytes was not set
#ವ್ಯಾಖ್ಯಾನಿಸು PC_ಸ್ಥಿತಿ_ಬಲ	1	// right half of double-wide char
#ವ್ಯಾಖ್ಯಾನಿಸು PC_ಸ್ಥಿತಿ_ಎಡ	2	// left half of double-wide char
#ವ್ಯಾಖ್ಯಾನಿಸು PC_ಸ್ಥಿತಿ_ಗಣ	3	// pc_bytes was filled
ಸ್ಥಿರ char_u pc_ಬೈಟುಗಳು[MB_MAXBYTES + 1]; // saved bytes
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ  pc_attr;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ  pc_ಅಡ್ಡಸಾಲು;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ  pc_ಲಂಬಸಾಲು;

    ಶೂನ್ಯ
edit_putchar(ಪೂರ್ಣಾಂಕ c, ಪೂರ್ಣಾಂಕ highlight)
{
    ಪೂರ್ಣಾಂಕ	    attr;

    ಆದರೆ (Screen_ಸಾಲುಗಳು == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    update_topline();	// just in case w_topline isn't valid
    validate_cursor();
    ಆದರೆ (highlight)
	attr = HL_ATTR(HLF_8);
    ಇಲ್ಲದಿದ್ದರೆ
	attr = 0;
    pc_ಅಡ್ಡಸಾಲು = W_WINROW(curwin) + curwin->w_wrow;
    pc_ಲಂಬಸಾಲು = curwin->w_wincol;
    pc_ಸ್ಥಿತಿ = PC_ಸ್ಥಿತಿ_UNSET;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RIGHTLEFT
    ಆದರೆ (curwin->w_p_rl)
    {
	pc_ಲಂಬಸಾಲು += curwin->w_ಅಗಲ - 1 - curwin->w_wcol;
	ಆದರೆ (has_mbyte)
	{
	    ಪೂರ್ಣಾಂಕ fix_ಲಂಬಸಾಲು = mb_fix_ಲಂಬಸಾಲು(pc_ಲಂಬಸಾಲು, pc_ಅಡ್ಡಸಾಲು);

	    ಆದರೆ (fix_ಲಂಬಸಾಲು != pc_ಲಂಬಸಾಲು)
	    {
		screen_putchar(' ', pc_ಅಡ್ಡಸಾಲು, fix_ಲಂಬಸಾಲು, attr);
		--curwin->w_wcol;
		pc_ಸ್ಥಿತಿ = PC_ಸ್ಥಿತಿ_ಬಲ;
	    }
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
    {
	pc_ಲಂಬಸಾಲು += curwin->w_wcol;
	ಆದರೆ (mb_lefthalve(pc_ಅಡ್ಡಸಾಲು, pc_ಲಂಬಸಾಲು))
	    pc_ಸ್ಥಿತಿ = PC_ಸ್ಥಿತಿ_ಎಡ;
    }

    // save the character to be able to put it back
    ಆದರೆ (pc_ಸ್ಥಿತಿ == PC_ಸ್ಥಿತಿ_UNSET)
    {
	screen_getbytes(pc_ಅಡ್ಡಸಾಲು, pc_ಲಂಬಸಾಲು, pc_ಬೈಟುಗಳು, &pc_attr);
	pc_ಸ್ಥಿತಿ = PC_ಸ್ಥಿತಿ_ಗಣ;
    }
    screen_putchar(c, pc_ಅಡ್ಡಸಾಲು, pc_ಲಂಬಸಾಲು, attr);
}

#ಆದರೆ defined(FEAT_ಕೆಲಸ_CHANNEL)
/*
 * Set the insert start position for when using a prompt buffer.
 */
    ಶೂನ್ಯ
ಗಣ_insstart(linenr_T lnum, ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು)
{
    Insstart.lnum = lnum;
    Insstart.ಲಂಬಸಾಲು = ಲಂಬಸಾಲು;
    Insstart_orig = Insstart;
    Insstart_textlen = Insstart.ಲಂಬಸಾಲು;
    Insstart_blank_vcol = MAXCOL;
    arrow_ಬಳಸಿದ = FALSE;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Undo the previous edit_putchar().
 */
    ಶೂನ್ಯ
edit_unputchar(ಶೂನ್ಯ)
{
    ಆದರೆ (pc_ಸ್ಥಿತಿ != PC_ಸ್ಥಿತಿ_UNSET && pc_ಅಡ್ಡಸಾಲು >= ಸಂದೇಶ_scrolled)
    {
	ಆದರೆ (pc_ಸ್ಥಿತಿ == PC_ಸ್ಥಿತಿ_ಬಲ)
	    ++curwin->w_wcol;
	ಆದರೆ (pc_ಸ್ಥಿತಿ == PC_ಸ್ಥಿತಿ_ಬಲ || pc_ಸ್ಥಿತಿ == PC_ಸ್ಥಿತಿ_ಎಡ)
	    redrawWinline(curwin, curwin->w_cursor.lnum);
	ಇಲ್ಲದಿದ್ದರೆ
	    screen_puts(pc_ಬೈಟುಗಳು, pc_ಅಡ್ಡಸಾಲು - ಸಂದೇಶ_scrolled, pc_ಲಂಬಸಾಲು, pc_attr);
    }
}

/*
 * Called when "$" is in 'cpoptions': display a '$' at the end of the changed
 * text.  Only works when cursor is in the line that changes.
 */
    ಶೂನ್ಯ
display_dollar(colnr_T ಲಂಬಸಾಲು_arg)
{
    colnr_T ಲಂಬಸಾಲು = ಲಂಬಸಾಲು_arg < 0 ? 0 : ಲಂಬಸಾಲು_arg;
    colnr_T save_ಲಂಬಸಾಲು;

    ಆದರೆ (!redrawing())
	ಹಿಂತಿರುಗಿ;

    cursor_off();
    save_ಲಂಬಸಾಲು = curwin->w_cursor.ಲಂಬಸಾಲು;
    curwin->w_cursor.ಲಂಬಸಾಲು = ಲಂಬಸಾಲು;
    ಆದರೆ (has_mbyte)
    {
	char_u *p;

	// If on the last byte of a multi-byte move to the first byte.
	p = ml_get_curline();
	curwin->w_cursor.ಲಂಬಸಾಲು -= (*mb_ತಲೆ_off)(p, p + ಲಂಬಸಾಲು);
    }
    curs_columns(FALSE);	    // recompute w_wrow and w_wcol
    ಆದರೆ (curwin->w_wcol < curwin->w_ಅಗಲ)
    {
	edit_putchar('$', FALSE);
	dollar_vcol = curwin->w_virtcol;
    }
    curwin->w_cursor.ಲಂಬಸಾಲು = save_ಲಂಬಸಾಲು;
}

/*
 * Call this function before moving the cursor from the normal insert position
 * in insert mode.
 */
    ಶೂನ್ಯ
undisplay_dollar(ಶೂನ್ಯ)
{
    ಆದರೆ (dollar_vcol < 0)
	ಹಿಂತಿರುಗಿ;

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
    ಪೂರ್ಣಾಂಕ	    i;

    // find start of trailing white space
    ಗಾಗಿ (i = (ಪೂರ್ಣಾಂಕ)len - 1; i >= 0 && VIM_ISWHITE(ಸಾಲು[i]); i--)
    {
	ಆದರೆ (State & REPLACE_FLAG)
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
backspace_until_ಸ್ತಂಭ(ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು)
{
    ಆಗ ((ಪೂರ್ಣಾಂಕ)curwin->w_cursor.ಲಂಬಸಾಲು > ಲಂಬಸಾಲು)
    {
	curwin->w_cursor.ಲಂಬಸಾಲು--;
	ಆದರೆ (State & REPLACE_FLAG)
	    replace_do_bs(ಲಂಬಸಾಲು);
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!del_char_after_ಲಂಬಸಾಲು(ಲಂಬಸಾಲು))
	    ನಿಲ್ಲಿಸು;
    }
}

/*
 * Like del_char(), but make sure not to go before column "limit_col".
 * Only matters when there are composing characters.
 * Return TRUE when something was deleted.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
del_char_after_ಲಂಬಸಾಲು(ಪೂರ್ಣಾಂಕ ಎಲ್ಲೆ_ಲಂಬಸಾಲು ಬಳಸದ)
{
    ಆದರೆ (enc_utf8 && ಎಲ್ಲೆ_ಲಂಬಸಾಲು >= 0)
    {
	colnr_T ecol = curwin->w_cursor.ಲಂಬಸಾಲು + 1;

	// Make sure the cursor is at the start of a character, but
	// skip forward again when going too far back because of a
	// composing character.
	mb_adjust_cursor();
	ಆಗ (curwin->w_cursor.ಲಂಬಸಾಲು < (colnr_T)ಎಲ್ಲೆ_ಲಂಬಸಾಲು)
	{
	    ಪೂರ್ಣಾಂಕ l = utf_ptr2len(ml_get_cursor());

	    ಆದರೆ (l == 0)  // end of line
		ನಿಲ್ಲಿಸು;
	    curwin->w_cursor.ಲಂಬಸಾಲು += l;
	}
	ಆದರೆ (*ml_get_cursor() == NUL || curwin->w_cursor.ಲಂಬಸಾಲು == ecol)
	    ಹಿಂತಿರುಗಿ FALSE;
	del_ಬೈಟುಗಳು((ದೀರ್ಘ)((ಪೂರ್ಣಾಂಕ)ecol - curwin->w_cursor.ಲಂಬಸಾಲು), FALSE, TRUE);
    }
    ಇಲ್ಲದಿದ್ದರೆ
	(ಶೂನ್ಯ)del_char(FALSE);
    ಹಿಂತಿರುಗಿ TRUE;
}

/*
 * Next character is interpreted literally.
 * A one, two or three digit decimal number is interpreted as its byte value.
 * If one or two digits are entered, the next character is given to vungetc().
 * For Unicode a character > 255 may be returned.
 * If "noReduceKeys" is TRUE do not change any modifyOtherKeys ESC sequence
 * into a normal key, return ESC.
 */
    ಪೂರ್ಣಾಂಕ
get_literal(ಪೂರ್ಣಾಂಕ no_Reduce_ಕೀಲಿಗಳು)
{
    ಪೂರ್ಣಾಂಕ		cc;
    ಪೂರ್ಣಾಂಕ		nc;
    ಪೂರ್ಣಾಂಕ		i;
    ಪೂರ್ಣಾಂಕ		hex = FALSE;
    ಪೂರ್ಣಾಂಕ		octal = FALSE;
    ಪೂರ್ಣಾಂಕ		unicode = 0;

    ಆದರೆ (got_int)
	ಹಿಂತಿರುಗಿ Ctrl_C;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
    /*
     * In GUI there is no point inserting the internal code for a special key.
     * It is more useful to insert the string "<KEY>" instead.	This would
     * probably be useful in a text window too, but it would not be
     * vi-compatible (maybe there should be an option for it?) -- webb
     */
    ಆದರೆ (gui.ಒಳ_use)
    {
	++allow_ಕೀಲಿಗಳು;
	ಆದರೆ (no_Reduce_ಕೀಲಿಗಳು)
	    ++no_reduce_ಕೀಲಿಗಳು;
    }
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_ON_FLY_SCROLL
    dont_scroll = TRUE;		// disallow scrolling here
#ಆದರೆ_ಅಂತ್ಯ
    ++no_mapping;		// don't map the next key hits
    cc = 0;
    i = 0;
    ಗಾಗಿ (;;)
    {
	nc = plain_vgetc();
	ಆದರೆ ((nc == ESC || nc == CSI) && !no_Reduce_ಕೀಲಿಗಳು)
	    nc = decode_Modify_Other_ಕೀಲಿಗಳು(nc);

	ಆದರೆ ((mod_ಮುಖವಾಡ & ~MOD_ಮುಖವಾಡ_ಸರಿಸು) != 0)
	    // A character with non-Shift modifiers should not be a valid
	    // character for i_CTRL-V_digit.
	    ನಿಲ್ಲಿಸು;

	ಆದರೆ ((State & ಬಹುಲಕ_CMDLINE) == 0 && MB_BYTE2LEN_CHECK(nc) == 1)
	    ಕೂಡಿಸು_ಗಮ್ಯ_showcmd(nc);
	ಆದರೆ (nc == 'x' || nc == 'X')
	    hex = TRUE;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (nc == 'o' || nc == 'O')
	    octal = TRUE;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (nc == 'u' || nc == 'U')
	    unicode = nc;
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಆದರೆ (hex || unicode != 0)
	    {
		ಆದರೆ (!vim_isxdigit(nc))
		    ನಿಲ್ಲಿಸು;
		cc = cc * 16 + hex2nr(nc);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (octal)
	    {
		ಆದರೆ (nc < '0' || nc > '7')
		    ನಿಲ್ಲಿಸು;
		cc = cc * 8 + nc - '0';
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		ಆದರೆ (!VIM_ISDIGIT(nc))
		    ನಿಲ್ಲಿಸು;
		cc = cc * 10 + nc - '0';
	    }

	    ++i;
	}

	ಆದರೆ (cc > 255 && unicode == 0)
	    cc = 255;		// limit range to 0-255
	nc = 0;

	ಆದರೆ (hex)		// hex: up to two chars
	{
	    ಆದರೆ (i >= 2)
		ನಿಲ್ಲಿಸು;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (unicode)	// Unicode: up to four or eight chars
	{
	    ಆದರೆ ((unicode == 'u' && i >= 4) || (unicode == 'U' && i >= 8))
		ನಿಲ್ಲಿಸು;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (i >= 3)	// decimal or octal: up to three chars
	    ನಿಲ್ಲಿಸು;
    }
    ಆದರೆ (i == 0)	    // no number entered
    {
	ಆದರೆ (nc == K_ZERO)   // NUL is stored as NL
	{
	    cc = '\n';
	    nc = 0;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    cc = nc;
	    nc = 0;
	}
    }

    ಆದರೆ (cc == 0)	// NUL is stored as NL
	cc = '\n';
    ಆದರೆ (enc_dbcs && (cc & 0xff) == 0)
	cc = '?';	// don't accept an illegal DBCS char, the NUL in the
			// second byte will cause trouble!

    --no_mapping;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
    ಆದರೆ (gui.ಒಳ_use)
    {
	--allow_ಕೀಲಿಗಳು;
	ಆದರೆ (no_Reduce_ಕೀಲಿಗಳು)
	    --no_reduce_ಕೀಲಿಗಳು;
    }
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ (nc)
    {
	vungetc(nc);
	// A character typed with i_CTRL-V_digit cannot have modifiers.
	mod_ಮುಖವಾಡ = 0;
    }
    got_int = FALSE;	    // CTRL-C typed after CTRL-V is not an interrupt
    ಹಿಂತಿರುಗಿ cc;
}

/*
 * Insert character, taking care of special keys and mod_mask
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ಸೇರಿಸು_special(
    ಪೂರ್ಣಾಂಕ	    c,
    ಪೂರ್ಣಾಂಕ	    allow_modmask,
    ಪೂರ್ಣಾಂಕ	    ctrlv)	    // c was typed after CTRL-V
{
    char_u  *p;
    ಪೂರ್ಣಾಂಕ	    len;

    /*
     * Special function key, translate into "<Key>". Up to the last '>' is
     * inserted with ins_str(), so as not to replace characters in replace
     * mode.
     * Only use mod_mask for special keys, to avoid things like <S-Space>,
     * unless 'allow_modmask' is TRUE.
     */
#ಆದರೆ defined(MACOS_X) || defined(FEAT_GUI_GTK)
    // Command-key never produces a normal key
    ಆದರೆ (mod_ಮುಖವಾಡ & MOD_ಮುಖವಾಡ_CMD)
	allow_modmask = TRUE;
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ (IS_SPECIAL(c) || (mod_ಮುಖವಾಡ && allow_modmask))
    {
	p = get_special_ಕೀಲಿ_name(c, mod_ಮುಖವಾಡ);
	len = (ಪೂರ್ಣಾಂಕ)STRLEN(p);
	c = p[len - 1];
	ಆದರೆ (len > 2)
	{
	    ಆದರೆ (stop_arrow() == FAIL)
		ಹಿಂತಿರುಗಿ;
	    p[len - 1] = NUL;
	    ins_str(p, len - 1);
	    Append_ಗಮ್ಯ_Redobuff_Lit(p, -1);
	    ctrlv = FALSE;
	}
    }
    ಆದರೆ (stop_arrow() == OK)
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
#ವ್ಯಾಖ್ಯಾನಿಸು ISSPECIAL(c)	((c) < ' ' || (c) >= DEL || (c) == '0' || (c) == '^')

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
    ಪೂರ್ಣಾಂಕ		c,			// character to insert or NUL
    ಪೂರ್ಣಾಂಕ		flags,			// INSCHAR_FORMAT, etc.
    ಪೂರ್ಣಾಂಕ		ಎರಡನೇ_indent)		// indent for second line if >= 0
{
    ಪೂರ್ಣಾಂಕ		textwidth;
    char_u	*p;
    ಪೂರ್ಣಾಂಕ		fo_ins_blank;
    ಪೂರ್ಣಾಂಕ		force_format = flags & INSCHAR_FORMAT;

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
    ಆದರೆ (textwidth > 0
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
#ಆದರೆ defined(FEAT_EVAL)
	ಪೂರ್ಣಾಂಕ     do_ಆಂತರಿಕ = TRUE;
	colnr_T virtcol = get_nolist_virtcol()
				  + char2cells(c != NUL ? c : gchar_cursor());

	ಆದರೆ (*curbuf->b_p_fex != NUL && (flags & INSCHAR_NO_FEX) == 0
		&& (force_format || virtcol > (colnr_T)textwidth))
	{
	    do_ಆಂತರಿಕ = (fex_format(curwin->w_cursor.lnum, 1L, c) != 0);
	    // It may be required to save for undo again, e.g. when setline()
	    // was called.
	    ins_need_undo = TRUE;
	}
	ಆದರೆ (do_ಆಂತರಿಕ)
#ಆದರೆ_ಅಂತ್ಯ
	    ಆಂತರಿಕ_format(textwidth, ಎರಡನೇ_indent, flags, c == NUL, c);
    }

    ಆದರೆ (c == NUL)	    // only formatting was wanted
	ಹಿಂತಿರುಗಿ;

    // Check whether this character should end a comment.
    ಆದರೆ (did_ai && c == ಕೊನೆ_comment_pending)
    {
	char_u  *ಸಾಲು;
	char_u	lead_ಕೊನೆ[COM_MAX_LEN];	    // end-comment string
	ಪೂರ್ಣಾಂಕ	ಮಧ್ಯ_len, ಕೊನೆ_len;
	ಪೂರ್ಣಾಂಕ	i;

	/*
	 * Need to remove existing (middle) comment leader and insert end
	 * comment leader.  First, check what comment leader we can find.
	 */
	i = get_leader_len(ಸಾಲು = ml_get_curline(), &p, FALSE, TRUE);
	ಆದರೆ (i > 0 && vim_strchr(p, COM_ಮಧ್ಯ) != ಖಾಲಿ)	// Just checking
	{
	    // Skip middle-comment string
	    ಆಗ (*p && p[-1] != ':')	// find end of middle flags
		++p;
	    ಮಧ್ಯ_len = copy_option_part(&p, lead_ಕೊನೆ, COM_MAX_LEN, ",");
	    // Don't count trailing white space for middle_len
	    ಆಗ (ಮಧ್ಯ_len > 0 && VIM_ISWHITE(lead_ಕೊನೆ[ಮಧ್ಯ_len - 1]))
		--ಮಧ್ಯ_len;

	    // Find the end-comment string
	    ಆಗ (*p && p[-1] != ':')	// find end of end flags
		++p;
	    ಕೊನೆ_len = copy_option_part(&p, lead_ಕೊನೆ, COM_MAX_LEN, ",");

	    // Skip white space before the cursor
	    i = curwin->w_cursor.ಲಂಬಸಾಲು;
	    ಆಗ (--i >= 0 && VIM_ISWHITE(ಸಾಲು[i]))
		;
	    i++;

	    // Skip to before the middle leader
	    i -= ಮಧ್ಯ_len;

	    // Check some expected things before we go on
	    ಆದರೆ (i >= 0 && ಕೊನೆ_len > 0 && lead_ಕೊನೆ[ಕೊನೆ_len - 1] == ಕೊನೆ_comment_pending)
	    {
		// Backspace over all the stuff we want to replace
		backspace_until_ಸ್ತಂಭ(i);

		// Insert the end-comment string, except for the last
		// character, which will get inserted as normal later.
		ins_ಬೈಟುಗಳು_len(lead_ಕೊನೆ, ಕೊನೆ_len - 1);
	    }
	}
    }
    ಕೊನೆ_comment_pending = NUL;

    did_ai = FALSE;
    did_si = FALSE;
    can_si = FALSE;
    can_si_ಹಿಂಭಾಗ = FALSE;

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
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_ON_FLY_SCROLL
    dont_scroll = FALSE;		// allow scrolling here
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (       !ISSPECIAL(c)
	    && (!has_mbyte || (*mb_char2len)(c) == 1)
	    && !has_insertcharpre()
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
	    // Skip typeahead if test_override("char_avail", 1) was called.
	    && !disable_char_avail_for_testing
#ಆದರೆ_ಅಂತ್ಯ
	    && vpeekc() != NUL
	    && !(State & REPLACE_FLAG)
	    && !cindent_on()
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RIGHTLEFT
	    && !p_ri
#ಆದರೆ_ಅಂತ್ಯ
	   )
    {
#ವ್ಯಾಖ್ಯಾನಿಸು ಒಳಹರಿವು_BUFLEN 100
	char_u		buf[ಒಳಹರಿವು_BUFLEN + 1];
	ಪೂರ್ಣಾಂಕ		i;
	colnr_T		virtcol = 0;

	buf[0] = c;
	i = 1;
	ಆದರೆ (textwidth > 0)
	    virtcol = get_nolist_virtcol();
	/*
	 * Stop the string when:
	 * - no more chars available
	 * - finding a special character (command key)
	 * - buffer is full
	 * - running into the 'textwidth' boundary
	 * - need to check for abbreviation: A non-word char after a word-char
	 */
	ಆಗ (	   (c = vpeekc()) != NUL
		&& !ISSPECIAL(c)
		&& (!has_mbyte || MB_BYTE2LEN_CHECK(c) == 1)
		&& i < ಒಳಹರಿವು_BUFLEN
		&& (textwidth == 0
		    || (virtcol += byte2cells(buf[i - 1])) < (colnr_T)textwidth)
		&& !(!no_abbr && !vim_iswordc(c) && vim_iswordc(buf[i - 1])))
	{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RIGHTLEFT
	    c = vgetc();
	    ಆದರೆ (p_hkmap && ಕೀಲಿ_Typed)
		c = hkmap(c);		    // Hebrew mode mapping
	    buf[i++] = c;
#ಇಲ್ಲದಿದ್ದರೆ
	    buf[i++] = vgetc();
#ಆದರೆ_ಅಂತ್ಯ
	}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIGRAPHS
	do_digraph(-1);			// clear digraphs
	do_digraph(buf[i-1]);		// may be the start of a digraph
#ಆದರೆ_ಅಂತ್ಯ
	buf[i] = NUL;
	ins_str(buf, i);
	ಆದರೆ (flags & INSCHAR_CTRLV)
	{
	    redo_literal(*buf);
	    i = 1;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    i = 0;
	ಆದರೆ (buf[i] != NUL)
	    Append_ಗಮ್ಯ_Redobuff_Lit(buf + i, -1);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಪೂರ್ಣಾಂಕ		cc;

	ಆದರೆ (has_mbyte && (cc = (*mb_char2len)(c)) > 1)
	{
	    char_u	buf[MB_MAXBYTES + 1];

	    (*mb_char2bytes)(c, buf);
	    buf[cc] = NUL;
	    ins_char_ಬೈಟುಗಳು(buf, cc);
	    Append_Char_ಗಮ್ಯ_Redobuff(c);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ins_char(c);
	    ಆದರೆ (flags & INSCHAR_CTRLV)
		redo_literal(c);
	    ಇಲ್ಲದಿದ್ದರೆ
		Append_Char_ಗಮ್ಯ_Redobuff(c);
	}
    }
}

/*
 * Put a character in the redo buffer, for when just after a CTRL-V.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
redo_literal(ಪೂರ್ಣಾಂಕ c)
{
    char_u	buf[10];

    // Only digits need special treatment.  Translate them into a string of
    // three digits.
    ಆದರೆ (VIM_ISDIGIT(c))
    {
	vim_snprintf((ಅಕ್ಷರ *)buf, ಗಾತ್ರ(buf), "%03d", c);
	Append_ಗಮ್ಯ_Redobuff(buf);
    }
    ಇಲ್ಲದಿದ್ದರೆ
	Append_Char_ಗಮ್ಯ_Redobuff(c);
}

/*
 * start_arrow() is called when an arrow key is used in insert mode.
 * For undo/redo it resembles hitting the <ESC> key.
 */
    ಶೂನ್ಯ
ಆರಂಭ_arrow(
    pos_T    *ಕೊನೆ_ಸೇರಿಸು_pos)		// can be NULL
{
    ಆರಂಭ_arrow_common(ಕೊನೆ_ಸೇರಿಸು_pos, TRUE);
}

/*
 * Like start_arrow() but with end_change argument.
 * Will prepare for redo of CTRL-G U if "end_change" is FALSE.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ಆರಂಭ_arrow_with_change(
    pos_T    *ಕೊನೆ_ಸೇರಿಸು_pos,		// can be NULL
    ಪೂರ್ಣಾಂಕ	      ಕೊನೆ_change)		// end undoable change
{
    ಆರಂಭ_arrow_common(ಕೊನೆ_ಸೇರಿಸು_pos, ಕೊನೆ_change);
    ಆದರೆ (!ಕೊನೆ_change)
    {
	Append_Char_ಗಮ್ಯ_Redobuff(Ctrl_G);
	Append_Char_ಗಮ್ಯ_Redobuff('U');
    }
}

    ಸ್ಥಿರ ಶೂನ್ಯ
ಆರಂಭ_arrow_common(
    pos_T    *ಕೊನೆ_ಸೇರಿಸು_pos,		// can be NULL
    ಪೂರ್ಣಾಂಕ	      ಕೊನೆ_change)		// end undoable change
{
    ಆದರೆ (!arrow_ಬಳಸಿದ && ಕೊನೆ_change)	// something has been inserted
    {
	Append_ಗಮ್ಯ_Redobuff(ESC_STR);
	stop_ಸೇರಿಸು(ಕೊನೆ_ಸೇರಿಸು_pos, FALSE, FALSE);
	arrow_ಬಳಸಿದ = TRUE;	// this means we stopped the current insert
    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SPELL
    check_spell_redraw();
#ಆದರೆ_ಅಂತ್ಯ
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SPELL
/*
 * If we skipped highlighting word at cursor, do it now.
 * It may be skipped again, thus reset spell_redraw_lnum first.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
check_spell_redraw(ಶೂನ್ಯ)
{
    ಆದರೆ (spell_redraw_lnum != 0)
    {
	linenr_T	lnum = spell_redraw_lnum;

	spell_redraw_lnum = 0;
	redrawWinline(curwin, lnum);
    }
}

#ಆದರೆ_ಅಂತ್ಯ

/*
 * stop_arrow() is called before a change is made in insert mode.
 * If an arrow key has been used, start a new insertion.
 * Returns FAIL if undo is impossible, shouldn't insert then.
 */
    ಪೂರ್ಣಾಂಕ
stop_arrow(ಶೂನ್ಯ)
{
    ಆದರೆ (arrow_ಬಳಸಿದ)
    {
	Insstart = curwin->w_cursor;	// new insertion starts here
	ಆದರೆ (Insstart.ಲಂಬಸಾಲು > Insstart_orig.ಲಂಬಸಾಲು && !ins_need_undo)
	    // Don't update the original insert position when moved to the
	    // right, except when nothing was inserted yet.
	    update_Insstart_orig = FALSE;
	Insstart_textlen = (colnr_T)linetabsize_str(ml_get_curline());

	ಆದರೆ (u_save_cursor() == OK)
	{
	    arrow_ಬಳಸಿದ = FALSE;
	    ins_need_undo = FALSE;
	}

	ai_ಲಂಬಸಾಲು = 0;
	ಆದರೆ (State & VREPLACE_FLAG)
	{
	    orig_ಸಾಲು_ಎಣಿಕೆ = curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
	    vr_ಸಾಲುಗಳು_ಬದಲಾಯಿತು = 1;
	}
	ResetRedobuff();
	Append_ಗಮ್ಯ_Redobuff((char_u *)"1i");   // pretend we start an insertion
	new_ಸೇರಿಸು_skip = 2;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ins_need_undo)
    {
	ಆದರೆ (u_save_cursor() == OK)
	{
	    // A command or event may have moved the cursor before the next
	    // edit. Pull Insstart back only when the cursor moved above it,
	    // so that later edits can properly decide whether an extra undo
	    // entry is needed. Advancing Insstart would mis-place '[ after a
	    // register paste.
	    ಆದರೆ (LT_POS(curwin->w_cursor, Insstart))
	    {
		Insstart = curwin->w_cursor;
		Insstart_textlen = (colnr_T)linetabsize_str(ml_get_curline());
	    }
	    ins_need_undo = FALSE;
	}
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
    // Always open fold at the cursor line when inserting something.
    foldOpenCursor();
#ಆದರೆ_ಅಂತ್ಯ

    ಹಿಂತಿರುಗಿ (arrow_ಬಳಸಿದ || ins_need_undo ? FAIL : OK);
}

/*
 * Do a few things to stop inserting.
 * "end_insert_pos" is where insert ended.  It is NULL when we already jumped
 * to another window/buffer.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
stop_ಸೇರಿಸು(
    pos_T	*ಕೊನೆ_ಸೇರಿಸು_pos,
    ಪೂರ್ಣಾಂಕ		esc,			// called by ins_esc()
    ಪೂರ್ಣಾಂಕ		nomove)			// <c-\><c-o>, don't move cursor
{
    ಪೂರ್ಣಾಂಕ		cc;
    string_T	inserted;

    stop_redo_ins();
    replace_flush();		// abandon replace stack

    /*
     * Save the inserted text for later redo with ^@ and CTRL-A.
     * Don't do it when "restart_edit" was set and nothing was inserted,
     * otherwise CTRL-O w and then <Left> will clear "last_insert".
     */
    inserted = get_inserted();
    ಪೂರ್ಣಾಂಕ added = inserted.string == ಖಾಲಿ ? 0 : (ಪೂರ್ಣಾಂಕ)inserted.ಉದ್ದಳತೆ - new_ಸೇರಿಸು_skip;
    ಆದರೆ (did_restart_edit == 0 || added > 0)
    {
	vim_free(ಕೊನೆಯ_ಸೇರಿಸು.string);
	ಕೊನೆಯ_ಸೇರಿಸು = inserted;			    // structure copy
	ಕೊನೆಯ_ಸೇರಿಸು_skip = added < 0 ? 0 : new_ಸೇರಿಸು_skip;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	vim_free(inserted.string);

    ಆದರೆ (!arrow_ಬಳಸಿದ && ಕೊನೆ_ಸೇರಿಸು_pos != ಖಾಲಿ)
    {
	// Auto-format now.  It may seem strange to do this when stopping an
	// insertion (or moving the cursor), but it's required when appending
	// a line and having it end in a space.  But only do it when something
	// was actually inserted, otherwise undo won't work.
	ಆದರೆ (!ins_need_undo && has_format_option(FO_AUTO))
	{
	    pos_T   tpos = curwin->w_cursor;

	    // When the cursor is at the end of the line after a space the
	    // formatting will move it to the following word.  Avoid that by
	    // moving the cursor onto the space.
	    cc = 'x';
	    ಆದರೆ (curwin->w_cursor.ಲಂಬಸಾಲು > 0 && gchar_cursor() == NUL)
	    {
		dec_cursor();
		cc = gchar_cursor();
		ಆದರೆ (!VIM_ISWHITE(cc))
		    curwin->w_cursor = tpos;
	    }

	    auto_format(TRUE, FALSE);

	    ಆದರೆ (VIM_ISWHITE(cc))
	    {
		ಆದರೆ (gchar_cursor() != NUL)
		    inc_cursor();
		// If the cursor is still at the same character, also keep
		// the "coladd".
		ಆದರೆ (gchar_cursor() == NUL
			&& curwin->w_cursor.lnum == tpos.lnum
			&& curwin->w_cursor.ಲಂಬಸಾಲು == tpos.ಲಂಬಸಾಲು)
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
	ಆದರೆ (!nomove && did_ai && (esc || (vim_strchr(p_cpo, CPO_INDENT) == ಖಾಲಿ
			&& curwin->w_cursor.lnum != ಕೊನೆ_ಸೇರಿಸು_pos->lnum))
		&& ಕೊನೆ_ಸೇರಿಸು_pos->lnum <= curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	{
	    pos_T	tpos = curwin->w_cursor;
	    colnr_T	prev_ಲಂಬಸಾಲು = ಕೊನೆ_ಸೇರಿಸು_pos->ಲಂಬಸಾಲು;
	    colnr_T	strip_ಲಂಬಸಾಲು;

	    curwin->w_cursor = *ಕೊನೆ_ಸೇರಿಸು_pos;
	    check_cursor_ಲಂಬಸಾಲು();  // make sure it is not past the line

	    // Where the loop would actually start (back up if on NUL).
	    strip_ಲಂಬಸಾಲು = curwin->w_cursor.ಲಂಬಸಾಲು;
	    ಆದರೆ (gchar_cursor() == NUL && strip_ಲಂಬಸಾಲು > 0)
		--strip_ಲಂಬಸಾಲು;

	    // Don't strip if non-whitespace follows: setline() from a <Cmd>
	    // mapping or CursorHoldI autocmd may have inserted content.
	    ಆದರೆ (*skipwhite(ml_get_curline() + strip_ಲಂಬಸಾಲು) == NUL)
	    {
		curwin->w_cursor.ಲಂಬಸಾಲು = strip_ಲಂಬಸಾಲು;
		ಗಾಗಿ (;;)
		{
		    ಆದರೆ (gchar_cursor() == NUL && curwin->w_cursor.ಲಂಬಸಾಲು > 0)
			--curwin->w_cursor.ಲಂಬಸಾಲು;
		    cc = gchar_cursor();
		    ಆದರೆ (!VIM_ISWHITE(cc))
			ನಿಲ್ಲಿಸು;
		    ಆದರೆ (del_char(TRUE) == FAIL)
			ನಿಲ್ಲಿಸು;  // should not happen
		}
		ಆದರೆ (curwin->w_cursor.lnum != tpos.lnum)
		    curwin->w_cursor = tpos;
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (curwin->w_cursor.ಲಂಬಸಾಲು < prev_ಲಂಬಸಾಲು)
		{
		    // reset tpos, could have been invalidated in the loop above
		    tpos = curwin->w_cursor;
		    tpos.ಲಂಬಸಾಲು++;
		    ಆದರೆ (cc != NUL && gchar_pos(&tpos) == NUL)
			++curwin->w_cursor.ಲಂಬಸಾಲು;	// put cursor back on the NUL
		}

		// <C-S-Right> may have started Visual mode, adjust the position for
		// deleted characters.
		ಆದರೆ (VIsual_ಸಕ್ರಿಯ)
		    check_visual_pos();
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		// Non-whitespace follows, keep original cursor.
		curwin->w_cursor = tpos;
	}
    }
    did_ai = FALSE;
    did_si = FALSE;
    can_si = FALSE;
    can_si_ಹಿಂಭಾಗ = FALSE;

    // Set '[ and '] to the inserted text.  When end_insert_pos is NULL we are
    // now in a different buffer.
    ಆದರೆ (ಕೊನೆ_ಸೇರಿಸು_pos != ಖಾಲಿ)
    {
	curbuf->b_op_ಆರಂಭ = Insstart;
	curbuf->b_op_ಆರಂಭ_orig = Insstart_orig;
	curbuf->b_op_ಕೊನೆ = *ಕೊನೆ_ಸೇರಿಸು_pos;
    }
}

/*
 * Set the last inserted text to a single character.
 * Used for the replace command.
 */
    ಶೂನ್ಯ
ಗಣ_ಕೊನೆಯ_ಸೇರಿಸು(ಪೂರ್ಣಾಂಕ c)
{
    char_u	*s;

    vim_free(ಕೊನೆಯ_ಸೇರಿಸು.string);
    ಕೊನೆಯ_ಸೇರಿಸು.string = alloc(MB_MAXBYTES * 3 + 5);
    ಆದರೆ (ಕೊನೆಯ_ಸೇರಿಸು.string == ಖಾಲಿ)
    {
	ಕೊನೆಯ_ಸೇರಿಸು.ಉದ್ದಳತೆ = 0;
	ಹಿಂತಿರುಗಿ;
    }

    s = ಕೊನೆಯ_ಸೇರಿಸು.string;
    // Use the CTRL-V only when entering a special char
    ಆದರೆ (c < ' ' || c == DEL)
	*s++ = Ctrl_V;
    s = ಕೂಡಿಸು_char2buf(c, s);
    *s++ = ESC;
    *s = NUL;
    ಕೊನೆಯ_ಸೇರಿಸು.ಉದ್ದಳತೆ = (size_t)(s - ಕೊನೆಯ_ಸೇರಿಸು.string);
    ಕೊನೆಯ_ಸೇರಿಸು_skip = 0;
}

#ಆದರೆ defined(EXITFREE)
    ಶೂನ್ಯ
free_ಕೊನೆಯ_ಸೇರಿಸು(ಶೂನ್ಯ)
{
    VIM_CLEAR_STRING(ಕೊನೆಯ_ಸೇರಿಸು);
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Add character "c" to buffer "s".  Escape the special meaning of K_SPECIAL
 * and CSI.  Handle multi-byte characters.
 * Returns a pointer to after the added bytes.
 */
    char_u *
ಕೂಡಿಸು_char2buf(ಪೂರ್ಣಾಂಕ c, char_u *s)
{
    char_u	ತಾತ್ಕಾಲಿಕ[MB_MAXBYTES + 1];
    ಪೂರ್ಣಾಂಕ		i;
    ಪೂರ್ಣಾಂಕ		len;

    len = (*mb_char2bytes)(c, ತಾತ್ಕಾಲಿಕ);
    ಗಾಗಿ (i = 0; i < len; ++i)
    {
	c = ತಾತ್ಕಾಲಿಕ[i];
	// Need to escape K_SPECIAL and CSI like in the typeahead buffer.
	ಆದರೆ (c == K_SPECIAL)
	{
	    *s++ = K_SPECIAL;
	    *s++ = KS_SPECIAL;
	    *s++ = KE_FILLER;
	}
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c == CSI)
	{
	    *s++ = CSI;
	    *s++ = KS_EXTRA;
	    *s++ = (ಪೂರ್ಣಾಂಕ)KE_CSI;
	}
#ಆದರೆ_ಅಂತ್ಯ
	ಇಲ್ಲದಿದ್ದರೆ
	    *s++ = c;
    }
    ಹಿಂತಿರುಗಿ s;
}

/*
 * move cursor to start of line
 * if flags & BL_WHITE	move to first non-white
 * if flags & BL_SOL	move to first non-white if startofline is set,
 *			    otherwise keep "curswant" column
 * if flags & BL_FIX	don't leave the cursor on a NUL.
 */
    ಶೂನ್ಯ
beginline(ಪೂರ್ಣಾಂಕ flags)
{
    ಆದರೆ ((flags & BL_SOL) && !p_sol)
	coladvance(curwin->w_curswant);
    ಇಲ್ಲದಿದ್ದರೆ
    {
	curwin->w_cursor.ಲಂಬಸಾಲು = 0;
	curwin->w_cursor.coladd = 0;

	ಆದರೆ (flags & (BL_WHITE | BL_SOL))
	{
	    char_u  *ptr;

	    ಗಾಗಿ (ptr = ml_get_curline(); VIM_ISWHITE(*ptr)
			       && !((flags & BL_FIX) && ptr[1] == NUL); ++ptr)
		++curwin->w_cursor.ಲಂಬಸಾಲು;
	}
	curwin->w_ಗಣ_curswant = true;
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

    ಪೂರ್ಣಾಂಕ
oneright(ಶೂನ್ಯ)
{
    char_u	*ptr;
    ಪೂರ್ಣಾಂಕ		l;

    ಆದರೆ (virtual_ಸಕ್ರಿಯ())
    {
	pos_T	prevpos = curwin->w_cursor;

	// Adjust for multi-wide char (excluding TAB)
	ptr = ml_get_cursor();
	coladvance(getviscol() + ((*ptr != TAB
					  && vim_isprintc((*mb_ptr2char)(ptr)))
		    ? ptr2cells(ptr) : 1));
	curwin->w_ಗಣ_curswant = true;
	// Return OK if the cursor moved, FAIL otherwise (at window edge).
	ಹಿಂತಿರುಗಿ (prevpos.ಲಂಬಸಾಲು != curwin->w_cursor.ಲಂಬಸಾಲು
		    || prevpos.coladd != curwin->w_cursor.coladd) ? OK : FAIL;
    }

    ptr = ml_get_cursor();
    ಆದರೆ (*ptr == NUL)
	ಹಿಂತಿರುಗಿ FAIL;	    // already at the very end

    ಆದರೆ (has_mbyte)
	l = (*mb_ptr2len)(ptr);
    ಇಲ್ಲದಿದ್ದರೆ
	l = 1;

    // move "l" bytes right, but don't end up on the NUL, unless 'virtualedit'
    // contains "onemore".
    ಆದರೆ (ptr[l] == NUL && (get_ve_flags() & VE_ONEMORE) == 0)
	ಹಿಂತಿರುಗಿ FAIL;
    curwin->w_cursor.ಲಂಬಸಾಲು += l;

    curwin->w_ಗಣ_curswant = true;
    adjust_skipcol();
    ಹಿಂತಿರುಗಿ OK;
}

    ಪೂರ್ಣಾಂಕ
oneleft(ಶೂನ್ಯ)
{
    ಆದರೆ (virtual_ಸಕ್ರಿಯ())
    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_LINEBREAK
	ಪೂರ್ಣಾಂಕ ಅಗಲ;
#ಆದರೆ_ಅಂತ್ಯ
	ಪೂರ್ಣಾಂಕ v = getviscol();

	ಆದರೆ (v == 0)
	    ಹಿಂತಿರುಗಿ FAIL;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_LINEBREAK
	// We might get stuck on 'showbreak', skip over it.
	ಅಗಲ = 1;
	ಗಾಗಿ (;;)
	{
	    coladvance(v - ಅಗಲ);
	    // getviscol() is slow, skip it when 'showbreak' is empty,
	    // 'breakindent' is not set and there are no multi-byte
	    // characters
	    ಆದರೆ ((*get_showbreak_ಮೌಲ್ಯ(curwin) == NUL && !curwin->w_p_bri
					     && !has_mbyte) || getviscol() < v)
		ನಿಲ್ಲಿಸು;
	    ++ಅಗಲ;
	}
#ಇಲ್ಲದಿದ್ದರೆ
	coladvance(v - 1);
#ಆದರೆ_ಅಂತ್ಯ

	ಆದರೆ (curwin->w_cursor.coladd == 1)
	{
	    char_u *ptr;

	    // Adjust for multi-wide char (not a TAB)
	    ptr = ml_get_cursor();
	    ಆದರೆ (*ptr != TAB && vim_isprintc((*mb_ptr2char)(ptr))
							 && ptr2cells(ptr) > 1)
		curwin->w_cursor.coladd = 0;
	}

	curwin->w_ಗಣ_curswant = true;
	adjust_skipcol();
	ಹಿಂತಿರುಗಿ OK;
    }

    ಆದರೆ (curwin->w_cursor.ಲಂಬಸಾಲು == 0)
	ಹಿಂತಿರುಗಿ FAIL;

    curwin->w_ಗಣ_curswant = true;
    --curwin->w_cursor.ಲಂಬಸಾಲು;

    // if the character on the left of the current cursor is a multi-byte
    // character, move to its first byte
    ಆದರೆ (has_mbyte)
	mb_adjust_cursor();
    adjust_skipcol();
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Move the cursor up "n" lines in window "wp".
 * Takes care of closed folds.
 */
    ಶೂನ್ಯ
cursor_ಮೇಲೆ_inner(win_T *wp, ದೀರ್ಘ n)
{
    linenr_T	lnum = wp->w_cursor.lnum;

    ಆದರೆ (n >= lnum)
	lnum = 1;
    ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
	ಆದರೆ (hasAnyFolding(wp))
    {
	/*
	 * Count each sequence of folded lines as one logical line.
	 */
	// go to the start of the current fold
	(ಶೂನ್ಯ)hasFoldingWin(wp, lnum, &lnum, ಖಾಲಿ, TRUE, ಖಾಲಿ);

	ಆಗ (n--)
	{
	    // move up one line
	    --lnum;
	    ಆದರೆ (lnum <= 1)
		ನಿಲ್ಲಿಸು;
	    // If we entered a fold, move to the beginning, unless in
	    // Insert mode or when 'foldopen' contains "all": it will open
	    // in a moment.
	    ಆದರೆ (n > 0 || !((State & ಬಹುಲಕ_ಸೇರಿಸು) || (fdo_flags & FDO_ALL)))
		(ಶೂನ್ಯ)hasFoldingWin(wp, lnum, &lnum, ಖಾಲಿ, TRUE, ಖಾಲಿ);
	}
	ಆದರೆ (lnum < 1)
	    lnum = 1;
    }
    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	lnum -= n;

    wp->w_cursor.lnum = lnum;
}

    ಪೂರ್ಣಾಂಕ
cursor_ಮೇಲೆ(
    ದೀರ್ಘ	n,
    ಪೂರ್ಣಾಂಕ		upd_topline)	    // When TRUE: update topline
{
    // This fails if the cursor is already in the first line or the count is
    // larger than the line number and '-' is in 'cpoptions'
    linenr_T lnum = curwin->w_cursor.lnum;
    ಆದರೆ (n > 0 && (lnum <= 1
		       || (n >= lnum && vim_strchr(p_cpo, CPO_MINUS) != ಖಾಲಿ)))
	ಹಿಂತಿರುಗಿ FAIL;
    cursor_ಮೇಲೆ_inner(curwin, n);

    // try to advance to the column we want to be at
    coladvance(curwin->w_curswant);

    ಆದರೆ (upd_topline)
	update_topline();	// make sure curwin->w_topline is valid

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Move the cursor down "n" lines in window "wp".
 * Takes care of closed folds.
 */
    ಶೂನ್ಯ
cursor_ಕೆಳಕ್ಕೆ_inner(win_T *wp, ದೀರ್ಘ n)
{
    linenr_T	lnum = wp->w_cursor.lnum;
    linenr_T	ಸಾಲು_ಎಣಿಕೆ = wp->w_ಬಫರ್->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;

    ಆದರೆ (lnum + n >= ಸಾಲು_ಎಣಿಕೆ)
	lnum = ಸಾಲು_ಎಣಿಕೆ;
    ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
	ಆದರೆ (hasAnyFolding(wp))
    {
	linenr_T	ಕೊನೆಯ;

	// count each sequence of folded lines as one logical line
	ಆಗ (n--)
	{
	    ಆದರೆ (hasFoldingWin(wp, lnum, ಖಾಲಿ, &ಕೊನೆಯ, TRUE, ಖಾಲಿ))
		lnum = ಕೊನೆಯ + 1;
	    ಇಲ್ಲದಿದ್ದರೆ
		++lnum;
	    ಆದರೆ (lnum >= ಸಾಲು_ಎಣಿಕೆ)
		ನಿಲ್ಲಿಸು;
	}
	ಆದರೆ (lnum > ಸಾಲು_ಎಣಿಕೆ)
	    lnum = ಸಾಲು_ಎಣಿಕೆ;
    }
    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	lnum += n;

    wp->w_cursor.lnum = lnum;
}

/*
 * Cursor down a number of logical lines.
 */
    ಪೂರ್ಣಾಂಕ
cursor_ಕೆಳಕ್ಕೆ(
    ದೀರ್ಘ	n,
    ಪೂರ್ಣಾಂಕ		upd_topline)	    // When TRUE: update topline
{
    linenr_T	lnum = curwin->w_cursor.lnum;
    linenr_T	ಸಾಲು_ಎಣಿಕೆ = curwin->w_ಬಫರ್->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
    // This fails if the cursor is already in the last (folded) line, or would
    // move beyond the last line and '-' is in 'cpoptions'.
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
    hasFoldingWin(curwin, lnum, ಖಾಲಿ, &lnum, TRUE, ಖಾಲಿ);
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ (n > 0
	    && (lnum >= ಸಾಲು_ಎಣಿಕೆ
		|| (lnum + n > ಸಾಲು_ಎಣಿಕೆ
				     && vim_strchr(p_cpo, CPO_MINUS) != ಖಾಲಿ)))
	ಹಿಂತಿರುಗಿ FAIL;
    cursor_ಕೆಳಕ್ಕೆ_inner(curwin, n);

    // try to advance to the column we want to be at
    coladvance(curwin->w_curswant);

    ಆದರೆ (upd_topline)
	update_topline();	// make sure curwin->w_topline is valid

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Stuff the last inserted text in the read buffer.
 * Last_insert actually is a copy of the redo buffer, so we
 * first have to remove the command.
 */
    ಪೂರ್ಣಾಂಕ
stuff_inserted(
    ಪೂರ್ಣಾಂಕ	    c,		// Command character to be inserted
    ದೀರ್ಘ    ಎಣಿಕೆ_2,	// Repeat this many times
    ಪೂರ್ಣಾಂಕ	    no_esc)	// Don't add an ESC at the end
{
    string_T	ಸೇರಿಸು;				// text to be inserted
    char_u	ಕೊನೆಯ = ' ';

    ಸೇರಿಸು = get_ಕೊನೆಯ_ಸೇರಿಸು();
    ಆದರೆ (ಸೇರಿಸು.string == ಖಾಲಿ)
    {
	emsg(_(e_no_inserted_ಪಠ್ಯ_yet));
	ಹಿಂತಿರುಗಿ FAIL;
    }

    // may want to stuff the command character, to start Insert mode
    ಆದರೆ (c != NUL)
	stuffcharReadbuff(c);

    ಆದರೆ (ಸೇರಿಸು.ಉದ್ದಳತೆ > 0)
    {
	char_u	*p;

	// look for the last ESC in 'insert'
	ಗಾಗಿ (p = ಸೇರಿಸು.string + ಸೇರಿಸು.ಉದ್ದಳತೆ - 1; p >= ಸೇರಿಸು.string; --p)
	{
	    ಆದರೆ (*p == ESC)
	    {
		ಸೇರಿಸು.ಉದ್ದಳತೆ = (size_t)(p - ಸೇರಿಸು.string);
		ನಿಲ್ಲಿಸು;
	    }
	}
    }

    ಆದರೆ (ಸೇರಿಸು.ಉದ್ದಳತೆ > 0)
    {
	char_u	*p = ಸೇರಿಸು.string + ಸೇರಿಸು.ಉದ್ದಳತೆ - 1;

	// when the last char is either "0" or "^" it will be quoted if no ESC
	// comes after it OR if it will insert more than once and "ptr"
	// starts with ^D.	-- Acevedo
	ಆದರೆ ((*p == '0' || *p == '^')
		&& (no_esc || (*ಸೇರಿಸು.string == Ctrl_D && ಎಣಿಕೆ_2 > 1)))
	{
	    ಕೊನೆಯ = *p;
	    --ಸೇರಿಸು.ಉದ್ದಳತೆ;
	}
    }

    ಮಾಡು
    {
	stuffReadbuffLen(ಸೇರಿಸು.string, (ದೀರ್ಘ)ಸೇರಿಸು.ಉದ್ದಳತೆ);
	// a trailing "0" is inserted as "<C-V>048", "^" as "<C-V>^"
	ಆಯ್ಕೆ (ಕೊನೆಯ)
	{
	ಸ್ಥಿತಿ '0':
#ವ್ಯಾಖ್ಯಾನಿಸು ಪಠ್ಯ_ಗಮ್ಯ_ಸೇರಿಸು "\026\060\064\070"
	    stuffReadbuffLen((char_u *)ಪಠ್ಯ_ಗಮ್ಯ_ಸೇರಿಸು, STRLEN_LITERAL(ಪಠ್ಯ_ಗಮ್ಯ_ಸೇರಿಸು));
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು ಪಠ್ಯ_ಗಮ್ಯ_ಸೇರಿಸು
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ '^':
#ವ್ಯಾಖ್ಯಾನಿಸು ಪಠ್ಯ_ಗಮ್ಯ_ಸೇರಿಸು "\026^"
	    stuffReadbuffLen((char_u *)ಪಠ್ಯ_ಗಮ್ಯ_ಸೇರಿಸು, STRLEN_LITERAL(ಪಠ್ಯ_ಗಮ್ಯ_ಸೇರಿಸು));
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು ಪಠ್ಯ_ಗಮ್ಯ_ಸೇರಿಸು
	    ನಿಲ್ಲಿಸು;

	ಪೂರ್ವನಿಯೋಜಿತ:
	    ನಿಲ್ಲಿಸು;
	}
    }
    ಆಗ (--ಎಣಿಕೆ_2 > 0);

    // may want to stuff a trailing ESC, to get out of Insert mode
    ಆದರೆ (!no_esc)
	stuffcharReadbuff(ESC);

    ಹಿಂತಿರುಗಿ OK;
}

    string_T
get_ಕೊನೆಯ_ಸೇರಿಸು(ಶೂನ್ಯ)
{
    string_T ಸೇರಿಸು = {ಖಾಲಿ, 0};

    ಆದರೆ (ಕೊನೆಯ_ಸೇರಿಸು.string != ಖಾಲಿ)
    {
	ಸೇರಿಸು.string = ಕೊನೆಯ_ಸೇರಿಸು.string + ಕೊನೆಯ_ಸೇರಿಸು_skip;
	ಸೇರಿಸು.ಉದ್ದಳತೆ = (size_t)(ಕೊನೆಯ_ಸೇರಿಸು.ಉದ್ದಳತೆ - ಕೊನೆಯ_ಸೇರಿಸು_skip);
    }

    ಹಿಂತಿರುಗಿ ಸೇರಿಸು;
}

/*
 * Get last inserted string, and remove trailing <Esc>.
 * Returns pointer to allocated memory (must be freed) or NULL.
 */
    char_u *
get_ಕೊನೆಯ_ಸೇರಿಸು_save(ಶೂನ್ಯ)
{
    string_T	ಸೇರಿಸು = get_ಕೊನೆಯ_ಸೇರಿಸು();
    char_u	*s;

    ಆದರೆ (ಸೇರಿಸು.string == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    s = vim_strnsave(ಸೇರಿಸು.string, ಸೇರಿಸು.ಉದ್ದಳತೆ);
    ಆದರೆ (s == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ಆದರೆ (ಸೇರಿಸು.ಉದ್ದಳತೆ > 0 && s[ಸೇರಿಸು.ಉದ್ದಳತೆ - 1] == ESC)	// remove trailing ESC
	s[--ಸೇರಿಸು.ಉದ್ದಳತೆ] = NUL;
    ಹಿಂತಿರುಗಿ s;
}

/*
 * Check the word in front of the cursor for an abbreviation.
 * Called when the non-id character "c" has been entered.
 * When an abbreviation is recognized it is removed from the text and
 * the replacement string is inserted in typebuf.tb_buf[], followed by "c".
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
echeck_abbr(ಪೂರ್ಣಾಂಕ c)
{
    // Don't check for abbreviation in paste mode, when disabled and just
    // after moving around with cursor keys.
    ಆದರೆ (p_paste || no_abbr || arrow_ಬಳಸಿದ)
	ಹಿಂತಿರುಗಿ FALSE;

    ಹಿಂತಿರುಗಿ check_abbr(c, ml_get_curline(), curwin->w_cursor.ಲಂಬಸಾಲು,
		curwin->w_cursor.lnum == Insstart.lnum ? Insstart.ಲಂಬಸಾಲು : 0);
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

ಸ್ಥಿರ char_u	*replace_ರಾಶಿ = ಖಾಲಿ;
ಸ್ಥಿರ ದೀರ್ಘ	replace_ರಾಶಿ_nr = 0;	    // next entry in replace stack
ಸ್ಥಿರ ದೀರ್ಘ	replace_ರಾಶಿ_len = 0;	    // max. number of entries

    ಶೂನ್ಯ
replace_ತಳ್ಳು(
    ಪೂರ್ಣಾಂಕ	    c)	    // character that is replaced (NUL is none)
{
    char_u  *p;

    ಆದರೆ (replace_ರಾಶಿ_nr < replace_ಸ್ಥಾನಾಂತರ)	// nothing to do
	ಹಿಂತಿರುಗಿ;
    ಆದರೆ (replace_ರಾಶಿ_len <= replace_ರಾಶಿ_nr)
    {
	replace_ರಾಶಿ_len += 50;
	p = ALLOC_MULT(char_u, replace_ರಾಶಿ_len);
	ಆದರೆ (p == ಖಾಲಿ)	    // out of memory
	{
	    replace_ರಾಶಿ_len -= 50;
	    ಹಿಂತಿರುಗಿ;
	}
	ಆದರೆ (replace_ರಾಶಿ != ಖಾಲಿ)
	{
	    mch_memmove(p, replace_ರಾಶಿ,
				 (size_t)(replace_ರಾಶಿ_nr * ಗಾತ್ರ(char_u)));
	    vim_free(replace_ರಾಶಿ);
	}
	replace_ರಾಶಿ = p;
    }
    p = replace_ರಾಶಿ + replace_ರಾಶಿ_nr - replace_ಸ್ಥಾನಾಂತರ;
    ಆದರೆ (replace_ಸ್ಥಾನಾಂತರ)
	mch_memmove(p + 1, p, (size_t)(replace_ಸ್ಥಾನಾಂತರ * ಗಾತ್ರ(char_u)));
    *p = c;
    ++replace_ರಾಶಿ_nr;
}

/*
 * Push a character onto the replace stack.  Handles a multi-byte character in
 * reverse byte order, so that the first byte is popped off first.
 * Return the number of bytes done (includes composing characters).
 */
    ಪೂರ್ಣಾಂಕ
replace_ತಳ್ಳು_mb(char_u *p)
{
    ಪೂರ್ಣಾಂಕ l = (*mb_ptr2len)(p);
    ಪೂರ್ಣಾಂಕ j;

    ಗಾಗಿ (j = l - 1; j >= 0; --j)
	replace_ತಳ್ಳು(p[j]);
    ಹಿಂತಿರುಗಿ l;
}

/*
 * Pop one item from the replace stack.
 * return -1 if stack empty
 * return replaced character or NUL otherwise
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
replace_ತೆಗೆ(ಶೂನ್ಯ)
{
    ಆದರೆ (replace_ರಾಶಿ_nr == 0)
	ಹಿಂತಿರುಗಿ -1;
    ಹಿಂತಿರುಗಿ (ಪೂರ್ಣಾಂಕ)replace_ರಾಶಿ[--replace_ರಾಶಿ_nr];
}

/*
 * Join the top two items on the replace stack.  This removes to "off"'th NUL
 * encountered.
 */
    ಶೂನ್ಯ
replace_join(
    ಪೂರ್ಣಾಂಕ	    off)	// offset for which NUL to remove
{
    ಪೂರ್ಣಾಂಕ	    i;

    ಗಾಗಿ (i = replace_ರಾಶಿ_nr; --i >= 0; )
	ಆದರೆ (replace_ರಾಶಿ[i] == NUL && off-- <= 0)
	{
	    --replace_ರಾಶಿ_nr;
	    mch_memmove(replace_ರಾಶಿ + i, replace_ರಾಶಿ + i + 1,
					      (size_t)(replace_ರಾಶಿ_nr - i));
	    ಹಿಂತಿರುಗಿ;
	}
}

/*
 * Pop bytes from the replace stack until a NUL is found, and insert them
 * before the cursor.  Can only be used in MODE_REPLACE or MODE_VREPLACE state.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
replace_ತೆಗೆ_ins(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ	    cc;
    ಪೂರ್ಣಾಂಕ	    oldState = State;

    State = ಬಹುಲಕ_ಸಾಮಾನ್ಯ;			// don't want MODE_REPLACE here
    ಆಗ ((cc = replace_ತೆಗೆ()) > 0)
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
mb_replace_ತೆಗೆ_ins(ಪೂರ್ಣಾಂಕ cc)
{
    ಪೂರ್ಣಾಂಕ		n;
    char_u	buf[MB_MAXBYTES + 1];
    ಪೂರ್ಣಾಂಕ		i;
    ಪೂರ್ಣಾಂಕ		c;

    ಆದರೆ (has_mbyte && (n = MB_BYTE2LEN(cc)) > 1)
    {
	buf[0] = cc;
	ಗಾಗಿ (i = 1; i < n; ++i)
	    buf[i] = replace_ತೆಗೆ();
	ins_ಬೈಟುಗಳು_len(buf, n);
    }
    ಇಲ್ಲದಿದ್ದರೆ
	ins_char(cc);

    ಆದರೆ (enc_utf8)
	// Handle composing chars.
	ಗಾಗಿ (;;)
	{
	    c = replace_ತೆಗೆ();
	    ಆದರೆ (c == -1)	    // stack empty
		ನಿಲ್ಲಿಸು;
	    ಆದರೆ ((n = MB_BYTE2LEN(c)) == 1)
	    {
		// Not a multi-byte char, put it back.
		replace_ತಳ್ಳು(c);
		ನಿಲ್ಲಿಸು;
	    }

	    buf[0] = c;
	    ಗಾಗಿ (i = 1; i < n; ++i)
		buf[i] = replace_ತೆಗೆ();
	    ಆದರೆ (utf_iscomposing(utf_ptr2char(buf)))
		ins_ಬೈಟುಗಳು_len(buf, n);
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		// Not a composing char, put it back.
		ಗಾಗಿ (i = n - 1; i >= 0; --i)
		    replace_ತಳ್ಳು(buf[i]);
		ನಿಲ್ಲಿಸು;
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
replace_do_bs(ಪೂರ್ಣಾಂಕ ಎಲ್ಲೆ_ಲಂಬಸಾಲು)
{
    ಪೂರ್ಣಾಂಕ		cc;
    ಪೂರ್ಣಾಂಕ		orig_len = 0;
    ಪೂರ್ಣಾಂಕ		ins_len;
    ಪೂರ್ಣಾಂಕ		orig_vcols = 0;
    colnr_T	ಆರಂಭ_vcol;
    char_u	*p;
    ಪೂರ್ಣಾಂಕ		i;
    ಪೂರ್ಣಾಂಕ		vcol;

    cc = replace_ತೆಗೆ();
    ಆದರೆ (cc > 0)
    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
	size_t	len_before = 0;  // init to shut up GCC

	ಆದರೆ (curbuf->b_has_textprop)
	{
	    // Do not adjust text properties for individual delete and insert
	    // operations, do it afterwards on the resulting text.
	    len_before = ml_get_curline_len();
	    ++ಪಠ್ಯ_prop_frozen;
	}
#ಆದರೆ_ಅಂತ್ಯ
	ಆದರೆ (State & VREPLACE_FLAG)
	{
	    // Get the number of screen cells used by the character we are
	    // going to delete.
	    getvcol(curwin, &curwin->w_cursor, ಖಾಲಿ, &ಆರಂಭ_vcol, ಖಾಲಿ, 0);
	    orig_vcols = chartabsize(ml_get_cursor(), ಆರಂಭ_vcol);
	}
	ಆದರೆ (has_mbyte)
	{
	    (ಶೂನ್ಯ)del_char_after_ಲಂಬಸಾಲು(ಎಲ್ಲೆ_ಲಂಬಸಾಲು);
	    ಆದರೆ (State & VREPLACE_FLAG)
		orig_len = ml_get_cursor_len();
	    replace_ತಳ್ಳು(cc);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    pchar_cursor(cc);
	    ಆದರೆ (State & VREPLACE_FLAG)
		orig_len = ml_get_cursor_len() - 1;
	}
	replace_ತೆಗೆ_ins();

	ಆದರೆ (State & VREPLACE_FLAG)
	{
	    // Get the number of screen cells used by the inserted characters
	    p = ml_get_cursor();
	    ins_len = ml_get_cursor_len() - orig_len;
	    vcol = ಆರಂಭ_vcol;
	    ಗಾಗಿ (i = 0; i < ins_len; ++i)
	    {
		vcol += chartabsize(p + i, vcol);
		i += (*mb_ptr2len)(p + i) - 1;
	    }
	    vcol -= ಆರಂಭ_vcol;

	    // Delete spaces that were inserted after the cursor to keep the
	    // text aligned.
	    curwin->w_cursor.ಲಂಬಸಾಲು += ins_len;
	    ಆಗ (vcol > orig_vcols && gchar_cursor() == ' ')
	    {
		del_char(FALSE);
		++orig_vcols;
	    }
	    curwin->w_cursor.ಲಂಬಸಾಲು -= ins_len;
	}

	// mark the buffer as changed and prepare for displaying
	ಬದಲಾಯಿತು_ಬೈಟುಗಳು(curwin->w_cursor.lnum, curwin->w_cursor.ಲಂಬಸಾಲು);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
	ಆದರೆ (curbuf->b_has_textprop)
	{
	    size_t len_now = ml_get_curline_len();

	    --ಪಠ್ಯ_prop_frozen;
	    adjust_prop_columns(curwin->w_cursor.lnum, curwin->w_cursor.ಲಂಬಸಾಲು,
					   (ಪೂರ್ಣಾಂಕ)(len_now - len_before), 0);
	}
#ಆದರೆ_ಅಂತ್ಯ
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (cc == 0)
	(ಶೂನ್ಯ)del_char_after_ಲಂಬಸಾಲು(ಎಲ್ಲೆ_ಲಂಬಸಾಲು);
}

#ಆದರೆ defined(FEAT_RIGHTLEFT)
/*
 * Map Hebrew keyboard when in hkmap mode.
 */
    ಪೂರ್ಣಾಂಕ
hkmap(ಪೂರ್ಣಾಂಕ c)
{
    ಆದರೆ (p_hkmapp)   // phonetic mapping, by Ilya Dogolazky
    {
	ಎಣಿಕೆ {hALEF=0, BET, GIMEL, DALET, HEI, VAV, ZAIN, HET, TET, IUD,
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

	ಆದರೆ (c == 'N' || c == 'M' || c == 'P' || c == 'C' || c == 'Z')
	    ಹಿಂತಿರುಗಿ (ಪೂರ್ಣಾಂಕ)(map[CharOrd(c)] - 1 + p_aleph);
							    // '-1'='sofit'
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c == 'x')
	    ಹಿಂತಿರುಗಿ 'X';
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c == 'q')
	    ಹಿಂತಿರುಗಿ '\''; // {geresh}={'}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c == 246)
	    ಹಿಂತಿರುಗಿ ' ';  // \"o --> ' ' for a german keyboard
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c == 228)
	    ಹಿಂತಿರುಗಿ ' ';  // \"a --> ' '      -- / --
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c == 252)
	    ಹಿಂತಿರುಗಿ ' ';  // \"u --> ' '      -- / --
	// NOTE: islower() does not do the right thing for us on Linux so we
	// do this the same was as 5.7 and previous, so it works correctly on
	// all systems.  Specifically, the e.g. Delete and Arrow keys are
	// munged and won't work if e.g. searching for Hebrew text.
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c >= 'a' && c <= 'z')
	    ಹಿಂತಿರುಗಿ (ಪೂರ್ಣಾಂಕ)(map[Char_Ord_ಕೆಳಗಿನ(c)] + p_aleph);
	ಇಲ್ಲದಿದ್ದರೆ
	    ಹಿಂತಿರುಗಿ c;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಆಯ್ಕೆ (c)
	{
	    ಸ್ಥಿತಿ '`':	ಹಿಂತಿರುಗಿ ';';
	    ಸ್ಥಿತಿ '/':	ಹಿಂತಿರುಗಿ '.';
	    ಸ್ಥಿತಿ '\'':	ಹಿಂತಿರುಗಿ ',';
	    ಸ್ಥಿತಿ 'q':	ಹಿಂತಿರುಗಿ '/';
	    ಸ್ಥಿತಿ 'w':	ಹಿಂತಿರುಗಿ '\'';

			// Hebrew letters - set offset from 'a'
	    ಸ್ಥಿತಿ ',':	c = '{'; ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ '.':	c = 'v'; ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ ';':	c = 't'; ನಿಲ್ಲಿಸು;
	    ಪೂರ್ವನಿಯೋಜಿತ: {
			 ಸ್ಥಿರ ಅಕ್ಷರ str[] = "zqbcxlsjphmkwonu ydafe rig";

			 ಆದರೆ (c < 'a' || c > 'z')
			     ಹಿಂತಿರುಗಿ c;
			 c = str[Char_Ord_ಕೆಳಗಿನ(c)];
			 ನಿಲ್ಲಿಸು;
		     }
	}

	ಹಿಂತಿರುಗಿ (ಪೂರ್ಣಾಂಕ)(Char_Ord_ಕೆಳಗಿನ(c) + p_aleph);
    }
}
#ಆದರೆ_ಅಂತ್ಯ

    ಸ್ಥಿರ ಶೂನ್ಯ
ins_reg(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ		need_redraw = FALSE;
    ಪೂರ್ಣಾಂಕ		regname;
    ಪೂರ್ಣಾಂಕ		literally = 0;
    ಪೂರ್ಣಾಂಕ		vis_ಸಕ್ರಿಯ = VIsual_ಸಕ್ರಿಯ;

    /*
     * If we are going to wait for a character, show a '"'.
     */
    pc_ಸ್ಥಿತಿ = PC_ಸ್ಥಿತಿ_UNSET;
    ಆದರೆ (redrawing() && !char_avail())
    {
	// may need to redraw when no more chars available now
	ins_redraw(FALSE);

	edit_putchar('"', TRUE);
	ಕೂಡಿಸು_ಗಮ್ಯ_showcmd_c(Ctrl_R);
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_ON_FLY_SCROLL
    dont_scroll = TRUE;		// disallow scrolling here
#ಆದರೆ_ಅಂತ್ಯ

    /*
     * Don't map the register name. This also prevents the mode message to be
     * deleted when ESC is hit.
     */
    ++no_mapping;
    ++allow_ಕೀಲಿಗಳು;
    regname = plain_vgetc();
    LANGMAP_ADJUST(regname, TRUE);
    ಆದರೆ (regname == Ctrl_R || regname == Ctrl_O || regname == Ctrl_P)
    {
	// Get a third key for literal register insertion
	literally = regname;
	ಕೂಡಿಸು_ಗಮ್ಯ_showcmd_c(literally);
	regname = plain_vgetc();
	LANGMAP_ADJUST(regname, TRUE);
    }
    --no_mapping;
    --allow_ಕೀಲಿಗಳು;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    // Don't call u_sync() while typing the expression or giving an error
    // message for it. Only call it explicitly.
    ++no_u_sync;
    ಆದರೆ (regname == '=')
    {
	pos_T	curpos = curwin->w_cursor;
# ifdef HAVE_ಒಳಹರಿವು_ವಿಧಾನ
	ಪೂರ್ಣಾಂಕ	im_on = im_get_ಸ್ಥಿತಿ();
# endif
	// Sync undo when evaluating the expression calls setline() or
	// append(), so that it can be undone separately.
	u_sync_once = 2;

	regname = get_ಅಭಿವ್ಯಕ್ತಿ_ರಿಜಿಸ್ಟರ್();

	// Cursor may be moved back a column.
	curwin->w_cursor = curpos;
	check_cursor();
# ifdef HAVE_ಒಳಹರಿವು_ವಿಧಾನ
	// Restore the Input Method.
	ಆದರೆ (im_on)
	    im_ಗಣ_ಸಕ್ರಿಯ(TRUE);
# endif
    }
    ಆದರೆ (regname == NUL || !valid_yank_reg(regname, FALSE))
    {
	vim_beep(BO_REG);
	need_redraw = TRUE;	// remove the '"'
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
#ಆದರೆ_ಅಂತ್ಯ
	ಆದರೆ (literally == Ctrl_O || literally == Ctrl_P)
	{
	    // Append the command to the redo buffer.
	    Append_Char_ಗಮ್ಯ_Redobuff(Ctrl_R);
	    Append_Char_ಗಮ್ಯ_Redobuff(literally);
	    Append_Char_ಗಮ್ಯ_Redobuff(regname);

	    do_put(regname, ಖಾಲಿ, BACKWARD, 1L,
		 (literally == Ctrl_P ? PUT_FIXINDENT : 0) | PUT_CURSEND);
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಸೇರಿಸು_reg(regname, literally) == FAIL)
	{
	    vim_beep(BO_REG);
	    need_redraw = TRUE;	// remove the '"'
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (stop_ಸೇರಿಸು_ಬಹುಲಕ)
	    // When the '=' register was used and a function was invoked that
	    // did ":stopinsert" then stuff_empty() returns FALSE but we won't
	    // insert anything, need to remove the '"'
	    need_redraw = TRUE;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    }
    --no_u_sync;
    ಆದರೆ (u_sync_once == 1)
	ins_need_undo = TRUE;
    u_sync_once = 0;
#ಆದರೆ_ಅಂತ್ಯ
    clear_showcmd();

    // If the inserted register is empty, we need to remove the '"'
    ಆದರೆ (need_redraw || stuff_ಖಾಲಿಗಳು())
	edit_unputchar();

    // Disallow starting Visual mode here, would get a weird mode.
    ಆದರೆ (!vis_ಸಕ್ರಿಯ && VIsual_ಸಕ್ರಿಯ)
	ಕೊನೆ_visual_ಬಹುಲಕ();
}

/*
 * CTRL-G commands in Insert mode.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ins_ctrl_g(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ		c;

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
    ಆಯ್ಕೆ (c)
    {
	// CTRL-G k and CTRL-G <Up>: cursor up to Insstart.col
	ಸ್ಥಿತಿ K_ಮೇಲೆ:
	ಸ್ಥಿತಿ Ctrl_K:
	ಸ್ಥಿತಿ 'k': ins_ಮೇಲೆ(TRUE);
		  ನಿಲ್ಲಿಸು;

	// CTRL-G j and CTRL-G <Down>: cursor down to Insstart.col
	ಸ್ಥಿತಿ K_ಕೆಳಕ್ಕೆ:
	ಸ್ಥಿತಿ Ctrl_J:
	ಸ್ಥಿತಿ 'j': ins_ಕೆಳಕ್ಕೆ(TRUE);
		  ನಿಲ್ಲಿಸು;

	// CTRL-G u: start new undoable edit
	ಸ್ಥಿತಿ 'u': u_sync(TRUE);
		  ins_need_undo = TRUE;

		  // Need to reset Insstart, esp. because a BS that joins
		  // a line to the previous one must save for undo.
		  update_Insstart_orig = FALSE;
		  Insstart = curwin->w_cursor;
		  ನಿಲ್ಲಿಸು;

	// CTRL-G U: do not break undo with the next char
	ಸ್ಥಿತಿ 'U':
		  // Allow one left/right cursor movement with the next char,
		  // without breaking undo.
		  dont_sync_undo = MAYBE;
		  ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ ESC:
		  // Esc after CTRL-G cancels it.
		  ನಿಲ್ಲಿಸು;

	// Unknown CTRL-G command, reserved for future expansion.
	ಪೂರ್ವನಿಯೋಜಿತ:  vim_beep(BO_CTRLG);
    }
}

/*
 * CTRL-^ in Insert mode.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ins_ctrl_hat(ಶೂನ್ಯ)
{
    ಆದರೆ (map_ಗಮ್ಯ_exists_ಬಹುಲಕ((char_u *)"", ಬಹುಲಕ_LANGMAP, FALSE))
    {
	// ":lmap" mappings exists, Toggle use of ":lmap" mappings.
	ಆದರೆ (State & ಬಹುಲಕ_LANGMAP)
	{
	    curbuf->b_p_iminsert = B_IMODE_NONE;
	    State &= ~ಬಹುಲಕ_LANGMAP;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    curbuf->b_p_iminsert = B_IMODE_LMAP;
	    State |= ಬಹುಲಕ_LANGMAP;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_ಒಳಹರಿವು_ವಿಧಾನ
	    im_ಗಣ_ಸಕ್ರಿಯ(FALSE);
#ಆದರೆ_ಅಂತ್ಯ
	}
    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_ಒಳಹರಿವು_ವಿಧಾನ
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// There are no ":lmap" mappings, toggle IM
	ಆದರೆ (im_get_ಸ್ಥಿತಿ())
	{
	    curbuf->b_p_iminsert = B_IMODE_NONE;
	    im_ಗಣ_ಸಕ್ರಿಯ(FALSE);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    curbuf->b_p_iminsert = B_IMODE_IM;
	    State &= ~ಬಹುಲಕ_LANGMAP;
	    im_ಗಣ_ಸಕ್ರಿಯ(TRUE);
	}
    }
#ಆದರೆ_ಅಂತ್ಯ
    ಗಣ_iminsert_global();
    showmode();
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
    // may show different cursor shape or color
    ಆದರೆ (gui.ಒಳ_use)
	gui_update_cursor(TRUE, FALSE);
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(FEAT_KEYMAP)
    // Show/unshow value of 'keymap' in status lines.
    ಸ್ಥಿತಿ_redraw_curbuf();
#ಆದರೆ_ಅಂತ್ಯ
}

/*
 * Handle ESC in insert mode.
 * Returns TRUE when leaving insert mode, FALSE when going to repeat the
 * insert.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ins_esc(
    ದೀರ್ಘ	*ಎಣಿಕೆ_2,
    ಪೂರ್ಣಾಂಕ		cmdchar,
    ಪೂರ್ಣಾಂಕ		nomove)	    // don't move cursor
{
    ಪೂರ್ಣಾಂಕ		ತಾತ್ಕಾಲಿಕ;
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	disabled_redraw = FALSE;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CONCEAL
    // Remember if the cursor line was concealed before changing State.
    ಪೂರ್ಣಾಂಕ		cursor_ಸಾಲು_was_concealed = curwin->w_p_cole > 0
						&& conceal_cursor_ಸಾಲು(curwin);
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SPELL
    check_spell_redraw();
#ಆದರೆ_ಅಂತ್ಯ

    ತಾತ್ಕಾಲಿಕ = curwin->w_cursor.ಲಂಬಸಾಲು;
    ಆದರೆ (disabled_redraw)
    {
	ಆದರೆ (RedrawingDisabled > 0)
	    --RedrawingDisabled;
	disabled_redraw = FALSE;
    }
    ಆದರೆ (!arrow_ಬಳಸಿದ)
    {
	/*
	 * Don't append the ESC for "r<CR>" and "grx".
	 * When 'insertmode' is set only CTRL-L stops Insert mode.  Needed for
	 * when "count" is non-zero.
	 */
	ಆದರೆ (cmdchar != 'r' && cmdchar != 'v')
	    Append_ಗಮ್ಯ_Redobuff(p_im ? (char_u *)"\014" : ESC_STR);

	/*
	 * Repeating insert may take a long time.  Check for
	 * interrupt now and then.
	 */
	ಆದರೆ (*ಎಣಿಕೆ_2 > 0)
	{
	    ಸಾಲು_breakcheck();
	    ಆದರೆ (got_int)
		*ಎಣಿಕೆ_2 = 0;
	}

	ಆದರೆ (--*ಎಣಿಕೆ_2 > 0)	// repeat what was typed
	{
	    // Vi repeats the insert without replacing characters.
	    ಆದರೆ (vim_strchr(p_cpo, CPO_REPLCNT) != ಖಾಲಿ)
		State &= ~REPLACE_FLAG;

	    (ಶೂನ್ಯ)ಆರಂಭ_redo_ins();
	    ಆದರೆ (cmdchar == 'r' || cmdchar == 'v')
		stuffRedoReadbuff(ESC_STR);	// no ESC in redo buffer
	    ++RedrawingDisabled;
	    disabled_redraw = TRUE;
	    ಹಿಂತಿರುಗಿ FALSE;	// repeat the insert
	}
	stop_ಸೇರಿಸು(&curwin->w_cursor, TRUE, nomove);
	undisplay_dollar();
    }

    ಆದರೆ (cmdchar != 'r' && cmdchar != 'v')
	ins_apply_autocmds(EVENT_INSERTLEAVEPRE);

    // When an autoindent was removed, curswant stays after the
    // indent
    ಆದರೆ (restart_edit == NUL && (colnr_T)ತಾತ್ಕಾಲಿಕ == curwin->w_cursor.ಲಂಬಸಾಲು)
	curwin->w_ಗಣ_curswant = true;

    // Remember the last Insert position in the '^ mark.
    ಆದರೆ ((cmdmod.cmod_flags & CMOD_KEEPJUMPS) == 0)
	curbuf->b_ಕೊನೆಯ_ಸೇರಿಸು = curwin->w_cursor;

    /*
     * The cursor should end up on the last inserted character.
     * Don't do it for CTRL-O, unless past the end of the line.
     */
    ಆದರೆ (!nomove
	    && (curwin->w_cursor.ಲಂಬಸಾಲು != 0
		|| curwin->w_cursor.coladd > 0)
	    && (restart_edit == NUL
		   || (gchar_cursor() == NUL && !VIsual_ಸಕ್ರಿಯ))
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RIGHTLEFT
	    && !revins_on
#ಆದರೆ_ಅಂತ್ಯ
				      )
    {
	ಆದರೆ (curwin->w_cursor.coladd > 0 || get_ve_flags() == VE_ALL)
	{
	    oneleft();
	    ಆದರೆ (restart_edit != NUL)
		++curwin->w_cursor.coladd;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    --curwin->w_cursor.ಲಂಬಸಾಲು;
	    curwin->w_valid &= ~(VALID_WCOL|VALID_VIRTCOL);
	    // Correct cursor for multi-byte character.
	    ಆದರೆ (has_mbyte)
		mb_adjust_cursor();
	}
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_ಒಳಹರಿವು_ವಿಧಾನ
    // Disable IM to allow typing English directly for Normal mode commands.
    // When ":lmap" is enabled don't change 'iminsert' (IM can be enabled as
    // well).
    ಆದರೆ (!(State & ಬಹುಲಕ_LANGMAP))
	im_save_ಸ್ಥಿತಿ(&curbuf->b_p_iminsert);
    im_ಗಣ_ಸಕ್ರಿಯ(FALSE);
#ಆದರೆ_ಅಂತ್ಯ

    State = ಬಹುಲಕ_ಸಾಮಾನ್ಯ;
    may_trigger_modechanged();
    // need to position cursor again when on a TAB and when on a char with
    // virtual text.
    ಆದರೆ (gchar_cursor() == TAB
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
	    || curbuf->b_has_textprop
#ಆದರೆ_ಅಂತ್ಯ
       )
	curwin->w_valid &= ~(VALID_WROW|VALID_WCOL|VALID_VIRTCOL);

    setmouse();
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURSOR_SHAPE
    ui_cursor_shape();		// may show different cursor shape
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ (!p_ek)
    {
	MAY_WANT_ಗಮ್ಯ_LOG_THIS;

	// Re-enable bracketed paste mode.
	ಹೊರಹೋಗು_str_t_BE();

	// Re-enable modifyOtherKeys.
	ಹೊರಹೋಗು_str_t_TI();
    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CONCEAL
    // Check if the cursor line needs redrawing after changing State.  If
    // 'concealcursor' is "i" it needs to be redrawn without concealing.
    conceal_check_cursor_ಸಾಲು(cursor_ಸಾಲು_was_concealed);
#ಆದರೆ_ಅಂತ್ಯ

    // When recording or for CTRL-O, need to display the new mode.
    // Otherwise remove the mode message.
    ಆದರೆ (reg_recording != 0 || restart_edit != NUL)
	showmode();
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (p_smd && (got_int || !skip_showmode()))
	ಸಂದೇಶ("");

    ಹಿಂತಿರುಗಿ TRUE;	    // exit Insert mode
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RIGHTLEFT
/*
 * Toggle language: hkmap and revins_on.
 * Move to end of reverse inserted text.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ins_ctrl_(ಶೂನ್ಯ)
{
    ಆದರೆ (revins_on && revins_chars && revins_scol >= 0)
    {
	ಆಗ (gchar_cursor() != NUL && revins_chars--)
	    ++curwin->w_cursor.ಲಂಬಸಾಲು;
    }
    p_ri = !p_ri;
    revins_on = (State == ಬಹುಲಕ_ಸೇರಿಸು && p_ri);
    ಆದರೆ (revins_on)
    {
	revins_scol = curwin->w_cursor.ಲಂಬಸಾಲು;
	revins_legal++;
	revins_chars = 0;
	undisplay_dollar();
    }
    ಇಲ್ಲದಿದ್ದರೆ
	revins_scol = -1;
    p_hkmap = curwin->w_p_rl ^ p_ri;    // be consistent!
    showmode();
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * If 'keymodel' contains "startsel", may start selection.
 * Returns TRUE when a CTRL-O and other keys stuffed.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ins_ಆರಂಭ_select(ಪೂರ್ಣಾಂಕ c)
{
    ಆದರೆ (!km_startsel)
	ಹಿಂತಿರುಗಿ FALSE;
    ಆಯ್ಕೆ (c)
    {
	ಸ್ಥಿತಿ K_KHOME:
	ಸ್ಥಿತಿ K_KEND:
	ಸ್ಥಿತಿ K_PAGEUP:
	ಸ್ಥಿತಿ K_KPAGEUP:
	ಸ್ಥಿತಿ K_PAGEDOWN:
	ಸ್ಥಿತಿ K_KPAGEDOWN:
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MACOS_X
	ಸ್ಥಿತಿ K_ಎಡ:
	ಸ್ಥಿತಿ K_ಬಲ:
	ಸ್ಥಿತಿ K_ಮೇಲೆ:
	ಸ್ಥಿತಿ K_ಕೆಳಕ್ಕೆ:
	ಸ್ಥಿತಿ K_ಕೊನೆ:
	ಸ್ಥಿತಿ K_HOME:
#ಆದರೆ_ಅಂತ್ಯ
	    ಆದರೆ (!(mod_ಮುಖವಾಡ & MOD_ಮುಖವಾಡ_ಸರಿಸು))
		ನಿಲ್ಲಿಸು;
	    // FALLTHROUGH
	ಸ್ಥಿತಿ K_S_ಎಡ:
	ಸ್ಥಿತಿ K_S_ಬಲ:
	ಸ್ಥಿತಿ K_S_ಮೇಲೆ:
	ಸ್ಥಿತಿ K_S_ಕೆಳಕ್ಕೆ:
	ಸ್ಥಿತಿ K_S_ಕೊನೆ:
	ಸ್ಥಿತಿ K_S_HOME:
	    // Start selection right away, the cursor can move with CTRL-O when
	    // beyond the end of the line.
	    ಆರಂಭ_selection();

	    // Execute the key in (insert) Select mode.
	    stuffcharReadbuff(Ctrl_O);
	    ಆದರೆ (mod_ಮುಖವಾಡ)
	    {
		char_u	    buf[4];

		buf[0] = K_SPECIAL;
		buf[1] = KS_MODIFIER;
		buf[2] = mod_ಮುಖವಾಡ;
		buf[3] = NUL;
		stuffReadbuffLen(buf, 3L);
	    }
	    stuffcharReadbuff(c);
	    ಹಿಂತಿರುಗಿ TRUE;
    }
    ಹಿಂತಿರುಗಿ FALSE;
}

/*
 * <Insert> key in Insert mode: toggle insert/replace mode.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ins_ಸೇರಿಸು(ಪೂರ್ಣಾಂಕ replaceState)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    ಗಣ_vim_var_string(VV_INSERTMODE,
		   (char_u *)((State & REPLACE_FLAG) ? "i"
			    : replaceState == ಬಹುಲಕ_VREPLACE ? "v" : "r"), 1);
#ಆದರೆ_ಅಂತ್ಯ
    ins_apply_autocmds(EVENT_INSERTCHANGE);
    ಆದರೆ (State & REPLACE_FLAG)
	State = ಬಹುಲಕ_ಸೇರಿಸು | (State & ಬಹುಲಕ_LANGMAP);
    ಇಲ್ಲದಿದ್ದರೆ
	State = replaceState | (State & ಬಹುಲಕ_LANGMAP);
    may_trigger_modechanged();
    Append_Char_ಗಮ್ಯ_Redobuff(K_INS);
    showmode();
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURSOR_SHAPE
    ui_cursor_shape();		// may show different cursor shape
#ಆದರೆ_ಅಂತ್ಯ
}

/*
 * Pressed CTRL-O in Insert mode.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ins_ctrl_o(ಶೂನ್ಯ)
{
    restart_VIsual_select = 0;
    ಆದರೆ (State & VREPLACE_FLAG)
	restart_edit = 'V';
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (State & REPLACE_FLAG)
	restart_edit = 'R';
    ಇಲ್ಲದಿದ್ದರೆ
	restart_edit = 'I';
    ಆದರೆ (virtual_ಸಕ್ರಿಯ())
	ins_at_eol = FALSE;	// cursor always keeps its column
    ಇಲ್ಲದಿದ್ದರೆ
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
ins_ಸರಿಸು(ಪೂರ್ಣಾಂಕ c, ಪೂರ್ಣಾಂಕ lastc)
{
    ಆದರೆ (stop_arrow() == FAIL)
	ಹಿಂತಿರುಗಿ;
    Append_Char_ಗಮ್ಯ_Redobuff(c);

    /*
     * 0^D and ^^D: remove all indent.
     */
    ಆದರೆ (c == Ctrl_D && (lastc == '0' || lastc == '^')
						  && curwin->w_cursor.ಲಂಬಸಾಲು > 0)
    {
	--curwin->w_cursor.ಲಂಬಸಾಲು;
	(ಶೂನ್ಯ)del_char(FALSE);		// delete the '^' or '0'
	// In Replace mode, restore the characters that '^' or '0' replaced.
	ಆದರೆ (State & REPLACE_FLAG)
	    replace_ತೆಗೆ_ins();
	ಆದರೆ (lastc == '^')
	    old_indent = get_indent();	// remember curr. indent
	change_indent(INDENT_ಗಣ, 0, TRUE, 0, TRUE);
    }
    ಇಲ್ಲದಿದ್ದರೆ
	change_indent(c == Ctrl_D ? INDENT_DEC : INDENT_INC, 0, TRUE, 0, TRUE);

    ಆದರೆ (did_ai && *skipwhite(ml_get_curline()) != NUL)
	did_ai = FALSE;
    did_si = FALSE;
    can_si = FALSE;
    can_si_ಹಿಂಭಾಗ = FALSE;
    can_cindent = FALSE;	// no cindenting after ^D or ^T
}

    ಸ್ಥಿರ ಶೂನ್ಯ
ins_del(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ	    ತಾತ್ಕಾಲಿಕ;

    ಆದರೆ (stop_arrow() == FAIL)
	ಹಿಂತಿರುಗಿ;
    ಆದರೆ (gchar_cursor() == NUL)		// delete newline
    {
	ತಾತ್ಕಾಲಿಕ = curwin->w_cursor.ಲಂಬಸಾಲು;
	ಆದರೆ (!can_bs(BS_EOL)		// only if "eol" included
		|| do_join(2, FALSE, TRUE, FALSE, FALSE) == FAIL)
	    vim_beep(BO_BS);
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    curwin->w_cursor.ಲಂಬಸಾಲು = ತಾತ್ಕಾಲಿಕ;
	    // Adjust orig_line_count in case more lines have been deleted than
	    // have been added. That makes sure, that open_line() later
	    // can access all buffer lines correctly
	    ಆದರೆ (State & VREPLACE_FLAG &&
		    orig_ಸಾಲು_ಎಣಿಕೆ > curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
		orig_ಸಾಲು_ಎಣಿಕೆ = curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (del_char(FALSE) == FAIL)  // delete char under cursor
	vim_beep(BO_BS);
    did_ai = FALSE;
    did_si = FALSE;
    can_si = FALSE;
    can_si_ಹಿಂಭಾಗ = FALSE;
    Append_Char_ಗಮ್ಯ_Redobuff(K_DEL);
}

/*
 * Delete one character for ins_bs().
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ins_bs_one(ಶೂನ್ಯ)
{
    dec_cursor();
    ಆದರೆ (State & REPLACE_FLAG)
    {
	// Don't delete characters before the insert point when in
	// Replace mode
	ಆದರೆ (curwin->w_cursor.lnum != Insstart.lnum
		|| curwin->w_cursor.ಲಂಬಸಾಲು >= Insstart.ಲಂಬಸಾಲು)
	    replace_do_bs(-1);
    }
    ಇಲ್ಲದಿದ್ದರೆ
	(ಶೂನ್ಯ)del_char(FALSE);
}

/*
 * Handle Backspace, delete-word and delete-line in Insert mode.
 * Return TRUE when backspace was actually used.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ins_bs(
    ಪೂರ್ಣಾಂಕ		c,
    ಪೂರ್ಣಾಂಕ		ಬಹುಲಕ,
    ಪೂರ್ಣಾಂಕ		*inserted_space_p)
{
    linenr_T	lnum;
    ಪೂರ್ಣಾಂಕ		cc;
    ಪೂರ್ಣಾಂಕ		ತಾತ್ಕಾಲಿಕ = 0;	    // init for GCC
    colnr_T	save_ಲಂಬಸಾಲು;
    colnr_T	mincol;
    ಪೂರ್ಣಾಂಕ		did_backspace = FALSE;
    ಪೂರ್ಣಾಂಕ		ಒಳ_indent;
    ಪೂರ್ಣಾಂಕ		oldState;
    ಪೂರ್ಣಾಂಕ		cpc[MAX_MCO];	    // composing characters
    ಪೂರ್ಣಾಂಕ		call_fix_indent = FALSE;

    /*
     * can't delete anything in an empty file
     * can't backup past first character in buffer
     * can't backup past starting point unless 'backspace' > 1
     * can backup to a previous line if 'backspace' == 0
     */
    ಆದರೆ (       BUFEMPTY()
	    || (
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RIGHTLEFT
		!revins_on &&
#ಆದರೆ_ಅಂತ್ಯ
		((curwin->w_cursor.lnum == 1 && curwin->w_cursor.ಲಂಬಸಾಲು == 0)
		    || (!can_bs(BS_ಆರಂಭ)
			&& ((arrow_ಬಳಸಿದ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಕೆಲಸ_CHANNEL
				&& !bt_prompt(curbuf)
#ಆದರೆ_ಅಂತ್ಯ
			) || (curwin->w_cursor.lnum == Insstart_orig.lnum
				&& curwin->w_cursor.ಲಂಬಸಾಲು <= Insstart_orig.ಲಂಬಸಾಲು)))
		    || (!can_bs(BS_INDENT) && !arrow_ಬಳಸಿದ && ai_ಲಂಬಸಾಲು > 0
					 && curwin->w_cursor.ಲಂಬಸಾಲು <= ai_ಲಂಬಸಾಲು)
		    || (!can_bs(BS_EOL) && curwin->w_cursor.ಲಂಬಸಾಲು == 0))))
    {
	vim_beep(BO_BS);
	ಹಿಂತಿರುಗಿ FALSE;
    }

    ಆದರೆ (stop_arrow() == FAIL)
	ಹಿಂತಿರುಗಿ FALSE;
    ಒಳ_indent = inindent(0);
    ಆದರೆ (ಒಳ_indent)
	can_cindent = FALSE;
    ಕೊನೆ_comment_pending = NUL;	// After BS, don't auto-end comment
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RIGHTLEFT
    ಆದರೆ (revins_on)	    // put cursor after last inserted char
	inc_cursor();
#ಆದರೆ_ಅಂತ್ಯ

    // Virtualedit:
    //	BACKSPACE_CHAR eats a virtual space
    //	BACKSPACE_WORD eats all coladd
    //	BACKSPACE_LINE eats all coladd and keeps going
    ಆದರೆ (curwin->w_cursor.coladd > 0)
    {
	ಆದರೆ (ಬಹುಲಕ == BACKSPACE_CHAR)
	{
	    --curwin->w_cursor.coladd;
	    ಹಿಂತಿರುಗಿ TRUE;
	}
	ಆದರೆ (ಬಹುಲಕ == BACKSPACE_ಶಬ್ದ)
	{
	    curwin->w_cursor.coladd = 0;
	    ಹಿಂತಿರುಗಿ TRUE;
	}
	curwin->w_cursor.coladd = 0;
    }

    /*
     * Delete newline!
     */
    ಆದರೆ (curwin->w_cursor.ಲಂಬಸಾಲು == 0)
    {
	lnum = Insstart.lnum;
	ಆದರೆ (curwin->w_cursor.lnum == lnum
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RIGHTLEFT
			|| revins_on
#ಆದರೆ_ಅಂತ್ಯ
				    )
	{
	    ಆದರೆ (u_save((linenr_T)(curwin->w_cursor.lnum - 2),
			       (linenr_T)(curwin->w_cursor.lnum + 1)) == FAIL)
		ಹಿಂತಿರುಗಿ FALSE;
	    --Insstart.lnum;
	    Insstart.ಲಂಬಸಾಲು = ml_get_len(Insstart.lnum);
	}
	/*
	 * In replace mode:
	 * cc < 0: NL was inserted, delete it
	 * cc >= 0: NL was replaced, put original characters back
	 */
	cc = -1;
	ಆದರೆ (State & REPLACE_FLAG)
	    cc = replace_ತೆಗೆ();	    // returns -1 if NL was inserted
	/*
	 * In replace mode, in the line we started replacing, we only move the
	 * cursor.
	 */
	ಆದರೆ ((State & REPLACE_FLAG) && curwin->w_cursor.lnum <= lnum)
	{
	    dec_cursor();
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಆದರೆ (!(State & VREPLACE_FLAG)
				   || curwin->w_cursor.lnum > orig_ಸಾಲು_ಎಣಿಕೆ)
	    {
		ತಾತ್ಕಾಲಿಕ = gchar_cursor();	// remember current char
		--curwin->w_cursor.lnum;

		// When "aw" is in 'formatoptions' we must delete the space at
		// the end of the line, otherwise the line will be broken
		// again when auto-formatting.
		ಆದರೆ (has_format_option(FO_AUTO)
					   && has_format_option(FO_WHITE_PAR))
		{
		    char_u  *ptr = ml_get_buf(curbuf, curwin->w_cursor.lnum,
									FALSE);
		    ಪೂರ್ಣಾಂಕ	    len = ml_get_curline_len();

		    ಆದರೆ (len > 0 && ptr[len - 1] == ' ')
		    {
			char_u *newp = alloc(curbuf->b_ml.ml_ಸಾಲು_len - 1);

			ಆದರೆ (newp != ಖಾಲಿ)
			{
			    mch_memmove(newp, ptr, len - 1);
			    newp[len - 1] = NUL;
			    ಆದರೆ (curbuf->b_ml.ml_ಸಾಲು_len > len + 1)
				mch_memmove(newp + len, ptr + len + 1,
					   curbuf->b_ml.ml_ಸಾಲು_len - len - 1);

			    ಆದರೆ (curbuf->b_ml.ml_flags
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
		ಆದರೆ (ತಾತ್ಕಾಲಿಕ == NUL && gchar_cursor() != NUL)
		    inc_cursor();
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		dec_cursor();

	    /*
	     * In MODE_REPLACE mode we have to put back the text that was
	     * replaced by the NL. On the replace stack is first a
	     * NUL-terminated sequence of characters that were deleted and then
	     * the characters that NL replaced.
	     */
	    ಆದರೆ (State & REPLACE_FLAG)
	    {
		/*
		 * Do the next ins_char() in MODE_NORMAL state, to
		 * prevent ins_char() from replacing characters and
		 * avoiding showmatch().
		 */
		oldState = State;
		State = ಬಹುಲಕ_ಸಾಮಾನ್ಯ;
		/*
		 * restore characters (blanks) deleted after cursor
		 */
		ಆಗ (cc > 0)
		{
		    save_ಲಂಬಸಾಲು = curwin->w_cursor.ಲಂಬಸಾಲು;
		    mb_replace_ತೆಗೆ_ins(cc);
		    curwin->w_cursor.ಲಂಬಸಾಲು = save_ಲಂಬಸಾಲು;
		    cc = replace_ತೆಗೆ();
		}
		// restore the characters that NL replaced
		replace_ತೆಗೆ_ins();
		State = oldState;
	    }
	}
	did_ai = FALSE;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	/*
	 * Delete character(s) before the cursor.
	 */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RIGHTLEFT
	ಆದರೆ (revins_on)		// put cursor on last inserted char
	    dec_cursor();
#ಆದರೆ_ಅಂತ್ಯ
	mincol = 0;
						// keep indent
	ಆದರೆ (ಬಹುಲಕ == BACKSPACE_ಸಾಲು
		&& (curbuf->b_p_ai || cindent_on())
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RIGHTLEFT
		&& !revins_on
#ಆದರೆ_ಅಂತ್ಯ
			    )
	{
	    save_ಲಂಬಸಾಲು = curwin->w_cursor.ಲಂಬಸಾಲು;
	    beginline(BL_WHITE);
	    ಆದರೆ (curwin->w_cursor.ಲಂಬಸಾಲು < save_ಲಂಬಸಾಲು)
	    {
		mincol = curwin->w_cursor.ಲಂಬಸಾಲು;
		// should now fix the indent to match with the previous line
		call_fix_indent = TRUE;
	    }
	    curwin->w_cursor.ಲಂಬಸಾಲು = save_ಲಂಬಸಾಲು;
	}

	/*
	 * Handle deleting one 'shiftwidth' or 'softtabstop'.
	 */
	ಆದರೆ (	   ಬಹುಲಕ == BACKSPACE_CHAR
		&& ((p_sta && ಒಳ_indent)
		    || ((get_sts_ಮೌಲ್ಯ() != 0
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_VARTABS
			|| tabstop_ಎಣಿಕೆ(curbuf->b_p_vsts_array)
#ಆದರೆ_ಅಂತ್ಯ
			)
			&& curwin->w_cursor.ಲಂಬಸಾಲು > 0
			&& (*(ml_get_cursor() - 1) == TAB
			    || (*(ml_get_cursor() - 1) == ' '
				&& (!*inserted_space_p
				    || arrow_ಬಳಸಿದ))))))
	{
	    colnr_T	vcol = 0;
	    colnr_T	want_vcol;
	    char_u	*ಸಾಲು;
	    char_u	*ptr;
	    char_u	*cursor_ptr;
	    char_u	*space_ptr;
	    colnr_T	space_vcol = 0;
	    ಪೂರ್ಣಾಂಕ		prev_space = FALSE;
	    colnr_T	want_ಲಂಬಸಾಲು;

	    *inserted_space_p = FALSE;

	    space_ptr = ptr = ಸಾಲು = ml_get_curline();
	    cursor_ptr = ಸಾಲು + curwin->w_cursor.ಲಂಬಸಾಲು;

	    // Compute virtual column of cursor position, and find the last
	    // whitespace before cursor that is preceded by non-whitespace.
	    // Use chartabsize() so that virtual text and wrapping are ignored.
	    ಆಗ (ptr < cursor_ptr)
	    {
		ಪೂರ್ಣಾಂಕ	cur_space = VIM_ISWHITE(*ptr);

		ಆದರೆ (!prev_space && cur_space)
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
	    ಆದರೆ (p_sta && ಒಳ_indent)
		want_vcol -= want_vcol % (ಪೂರ್ಣಾಂಕ)get_sw_ಮೌಲ್ಯ(curbuf);
	    ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_VARTABS
		want_vcol = tabstop_ಆರಂಭ(want_vcol, get_sts_ಮೌಲ್ಯ(),
						       curbuf->b_p_vsts_array);
#ಇಲ್ಲದಿದ್ದರೆ
		want_vcol -= want_vcol % (ಪೂರ್ಣಾಂಕ)get_sts_ಮೌಲ್ಯ();
#ಆದರೆ_ಅಂತ್ಯ

	    // Find the position to stop backspacing.
	    // Use chartabsize() so that virtual text and wrapping are ignored.
	    ಆಗ (TRUE)
	    {
		ಪೂರ್ಣಾಂಕ ಗಾತ್ರ_2 = chartabsize(space_ptr, space_vcol);

		ಆದರೆ (space_vcol + ಗಾತ್ರ_2 > want_vcol)
		    ನಿಲ್ಲಿಸು;
		space_vcol += ಗಾತ್ರ_2;
		MB_PTR_ADV(space_ptr);
	    }
	    want_ಲಂಬಸಾಲು = space_ptr - ಸಾಲು;

	    // Delete characters until we are at or before want_col.
	    ಆಗ (curwin->w_cursor.ಲಂಬಸಾಲು > want_ಲಂಬಸಾಲು)
		ins_bs_one();

	    // Insert extra spaces until we are at want_vcol.
	    ಗಾಗಿ (; space_vcol < want_vcol; space_vcol++)
	    {
		// Remember the first char we inserted
		ಆದರೆ (curwin->w_cursor.lnum == Insstart_orig.lnum
				   && curwin->w_cursor.ಲಂಬಸಾಲು < Insstart_orig.ಲಂಬಸಾಲು)
		    Insstart_orig.ಲಂಬಸಾಲು = curwin->w_cursor.ಲಂಬಸಾಲು;

		ಆದರೆ (State & VREPLACE_FLAG)
		    ins_char(' ');
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    ins_str((char_u *)" ", 1);
		    ಆದರೆ ((State & REPLACE_FLAG))
			replace_ತಳ್ಳು(NUL);
		}
	    }
	}

	/*
	 * Delete up to starting point, start of line or previous word.
	 */
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಪೂರ್ಣಾಂಕ cclass = 0, prev_cclass = 0;

	    ಆದರೆ (has_mbyte)
		cclass = mb_get_class(ml_get_cursor());
	    ಮಾಡು
	    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RIGHTLEFT
		ಆದರೆ (!revins_on) // put cursor on char to be deleted
#ಆದರೆ_ಅಂತ್ಯ
		    dec_cursor();

		cc = gchar_cursor();
		// look multi-byte character class
		ಆದರೆ (has_mbyte)
		{
		    prev_cclass = cclass;
		    cclass = mb_get_class(ml_get_cursor());
		}

		// start of word?
		ಆದರೆ (ಬಹುಲಕ == BACKSPACE_ಶಬ್ದ && !vim_isspace(cc))
		{
		    ಬಹುಲಕ = BACKSPACE_ಶಬ್ದ_NOT_SPACE;
		    ತಾತ್ಕಾಲಿಕ = vim_iswordc(cc);
		}
		// end of word?
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಬಹುಲಕ == BACKSPACE_ಶಬ್ದ_NOT_SPACE
			&& ((vim_isspace(cc) || vim_iswordc(cc) != ತಾತ್ಕಾಲಿಕ)
			|| prev_cclass != cclass))
		{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RIGHTLEFT
		    ಆದರೆ (!revins_on)
#ಆದರೆ_ಅಂತ್ಯ
			inc_cursor();
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RIGHTLEFT
		    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (State & REPLACE_FLAG)
			dec_cursor();
#ಆದರೆ_ಅಂತ್ಯ
		    ನಿಲ್ಲಿಸು;
		}
		ಆದರೆ (State & REPLACE_FLAG)
		    replace_do_bs(-1);
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    ಆದರೆ (enc_utf8 && p_deco)
			(ಶೂನ್ಯ)utfc_ptr2char(ml_get_cursor(), cpc);
		    (ಶೂನ್ಯ)del_char(FALSE);
		    /*
		     * If there are combining characters and 'delcombine' is set
		     * move the cursor back.  Don't back up before the base
		     * character.
		     */
		    ಆದರೆ (enc_utf8 && p_deco && cpc[0] != NUL)
			inc_cursor();
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RIGHTLEFT
		    ಆದರೆ (revins_chars)
		    {
			revins_chars--;
			revins_legal++;
		    }
		    ಆದರೆ (revins_on && gchar_cursor() == NUL)
			ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
		}
		// Just a single backspace?:
		ಆದರೆ (ಬಹುಲಕ == BACKSPACE_CHAR)
		    ನಿಲ್ಲಿಸು;
	    } ಆಗ (
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RIGHTLEFT
		    revins_on ||
#ಆದರೆ_ಅಂತ್ಯ
		    (curwin->w_cursor.ಲಂಬಸಾಲು > mincol
		    &&  (can_bs(BS_NOSTOP)
			|| (curwin->w_cursor.lnum != Insstart_orig.lnum
			|| curwin->w_cursor.ಲಂಬಸಾಲು != Insstart_orig.ಲಂಬಸಾಲು)
		    )));
	}
	did_backspace = TRUE;
    }
    did_si = FALSE;
    can_si = FALSE;
    can_si_ಹಿಂಭಾಗ = FALSE;
    ಆದರೆ (curwin->w_cursor.ಲಂಬಸಾಲು <= 1)
	did_ai = FALSE;

    ಆದರೆ (call_fix_indent)
	fix_indent();

    /*
     * It's a little strange to put backspaces into the redo
     * buffer, but it makes auto-indent a lot easier to deal
     * with.
     */
    Append_Char_ಗಮ್ಯ_Redobuff(c);

    // If deleted before the insertion point, adjust it
    ಆದರೆ (curwin->w_cursor.lnum == Insstart_orig.lnum
				  && curwin->w_cursor.ಲಂಬಸಾಲು < Insstart_orig.ಲಂಬಸಾಲು)
	Insstart_orig.ಲಂಬಸಾಲು = curwin->w_cursor.ಲಂಬಸಾಲು;

    // vi behaviour: the cursor moves backward but the character that
    //		     was there remains visible
    // Vim behaviour: the cursor moves backward and the character that
    //		      was there is erased from the screen.
    // We can emulate the vi behaviour by pretending there is a dollar
    // displayed even when there isn't.
    //  --pkv Sun Jan 19 01:56:40 EST 2003
    ಆದರೆ (vim_strchr(p_cpo, CPO_BACKSPACE) != ಖಾಲಿ && dollar_vcol == -1)
	dollar_vcol = curwin->w_virtcol;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
    // When deleting a char the cursor line must never be in a closed fold.
    // E.g., when 'foldmethod' is indent and deleting the first non-white
    // char before a Tab.
    ಆದರೆ (did_backspace)
	foldOpenCursor();
#ಆದರೆ_ಅಂತ್ಯ

    ಹಿಂತಿರುಗಿ did_backspace;
}

/*
 * Handle receiving P_PS: start paste mode.  Inserts the following text up to
 * P_PE literally.
 * When "drop" is TRUE then consume the text and drop it.
 */
    ಪೂರ್ಣಾಂಕ
bracketed_paste(paste_ಬಹುಲಕ_T ಬಹುಲಕ, ಪೂರ್ಣಾಂಕ drop, garray_T *gap)
{
    ಪೂರ್ಣಾಂಕ		c;
    char_u	buf[NUMBUFLEN + MB_MAXBYTES];
    ಪೂರ್ಣಾಂಕ		idx = 0;
    char_u	*ಕೊನೆ = ಕಂಡುಹಿಡಿ_termcode((char_u *)"PE");
    ಪೂರ್ಣಾಂಕ		ret_char = -1;
    ಪೂರ್ಣಾಂಕ		save_allow_ಕೀಲಿಗಳು = allow_ಕೀಲಿಗಳು;
    ಪೂರ್ಣಾಂಕ		save_paste = p_paste;

    // If the end code is too long we can't detect it, read everything.
    ಆದರೆ (ಕೊನೆ != ಖಾಲಿ && STRLEN(ಕೊನೆ) >= NUMBUFLEN)
	ಕೊನೆ = ಖಾಲಿ;
    ++no_mapping;
    allow_ಕೀಲಿಗಳು = 0;
    ಆದರೆ (!p_paste)
	// Also have the side effects of setting 'paste' to make it work much
	// faster.
	ಗಣ_option_ಮೌಲ್ಯ_give_err((char_u *)"paste", TRUE, ಖಾಲಿ, 0);

    ಗಾಗಿ (;;)
    {
	// When the end is not defined read everything there is.
	ಆದರೆ (ಕೊನೆ == ಖಾಲಿ && vpeekc() == NUL)
	    ನಿಲ್ಲಿಸು;
	ಮಾಡು
	    c = vgetc();
	ಆಗ (c == K_IGNORE || c == K_VER_SCROLLBAR || c == K_HOR_SCROLLBAR);

	ಆದರೆ (c == NUL || got_int || (ex_ಸಾಮಾನ್ಯ_busy > 0 && c == Ctrl_C))
	    // When CTRL-C was encountered the typeahead will be flushed and we
	    // won't get the end sequence.  Except when using ":normal".
	    ನಿಲ್ಲಿಸು;

	ಆದರೆ (has_mbyte)
	    idx += (*mb_char2bytes)(c, buf + idx);
	ಇಲ್ಲದಿದ್ದರೆ
	    buf[idx++] = c;
	buf[idx] = NUL;
	ಆದರೆ (ಕೊನೆ != ಖಾಲಿ && STRNCMP(buf, ಕೊನೆ, idx) == 0)
	{
	    ಆದರೆ (ಕೊನೆ[idx] == NUL)
		ನಿಲ್ಲಿಸು; // Found the end of paste code.
	    ಮುಂದುವರಿ;
	}
	ಆದರೆ (!drop)
	{
	    ಆಯ್ಕೆ (ಬಹುಲಕ)
	    {
		ಸ್ಥಿತಿ PASTE_CMDLINE:
		    put_on_cmdline(buf, idx, TRUE);
		    ನಿಲ್ಲಿಸು;

		ಸ್ಥಿತಿ PASTE_EX:
		    // add one for the NUL that is going to be appended
		    ಆದರೆ (gap != ಖಾಲಿ && ga_grow(gap, idx + 1) == OK)
		    {
			mch_memmove((ಅಕ್ಷರ *)gap->ga_ದತ್ತಾಂಶ + gap->ga_len,
							     buf, (size_t)idx);
			gap->ga_len += idx;
		    }
		    ನಿಲ್ಲಿಸು;

		ಸ್ಥಿತಿ PASTE_ಸೇರಿಸು:
		    ಆದರೆ (stop_arrow() == OK)
		    {
			c = buf[0];
			ಆದರೆ (idx == 1 && (c == CAR || c == K_KENTER || c == NL))
			    ins_eol(c);
			ಇಲ್ಲದಿದ್ದರೆ
			{
			    ins_char_ಬೈಟುಗಳು(buf, idx);
			    Append_ಗಮ್ಯ_Redobuff_Lit(buf, idx);
			}
		    }
		    ನಿಲ್ಲಿಸು;

		ಸ್ಥಿತಿ PASTE_ONE_CHAR:
		    ಆದರೆ (ret_char == -1)
		    {
			ಆದರೆ (has_mbyte)
			    ret_char = (*mb_ptr2char)(buf);
			ಇಲ್ಲದಿದ್ದರೆ
			    ret_char = buf[0];
		    }
		    ನಿಲ್ಲಿಸು;
	    }
	}
	idx = 0;
    }

    --no_mapping;
    allow_ಕೀಲಿಗಳು = save_allow_ಕೀಲಿಗಳು;
    ಆದರೆ (!save_paste)
	ಗಣ_option_ಮೌಲ್ಯ_give_err((char_u *)"paste", FALSE, ಖಾಲಿ, 0);

    ಹಿಂತಿರುಗಿ ret_char;
}

#ಆದರೆ defined(FEAT_GUI_TABLINE)
    ಸ್ಥಿರ ಶೂನ್ಯ
ins_tabline(ಪೂರ್ಣಾಂಕ c)
{
    // We will be leaving the current window, unless closing another tab.
    ಆದರೆ (c != K_TABMENU || current_tabmenu != TABLINE_MENU_CLOSE
		|| (current_tab != 0 && current_tab != tabpage_ಸೂಚ್ಯಂಕ(curtab)))
    {
	undisplay_dollar();
	ಆರಂಭ_arrow(&curwin->w_cursor);
	can_cindent = TRUE;
    }

    ಆದರೆ (c == K_TABLINE)
	goto_tabpage(current_tab);
    ಇಲ್ಲದಿದ್ದರೆ
    {
	handle_tabmenu();
	redraw_statuslines();	// will redraw the tabline when needed
    }
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_GUI)
    ಶೂನ್ಯ
ins_scroll(ಶೂನ್ಯ)
{
    pos_T	tpos;

    undisplay_dollar();
    tpos = curwin->w_cursor;
    ಆದರೆ (gui_do_scroll())
    {
	ಆರಂಭ_arrow(&tpos);
	can_cindent = TRUE;
    }
}

    ಶೂನ್ಯ
ins_horscroll(ಶೂನ್ಯ)
{
    pos_T	tpos;

    undisplay_dollar();
    tpos = curwin->w_cursor;
    ಆದರೆ (do_mousescroll_horiz(scrollbar_ಮೌಲ್ಯ))
    {
	ಆರಂಭ_arrow(&tpos);
	can_cindent = TRUE;
    }
}
#ಆದರೆ_ಅಂತ್ಯ

    ಸ್ಥಿರ ಶೂನ್ಯ
ins_ಎಡ(ಶೂನ್ಯ)
{
    pos_T	tpos;
    ಪೂರ್ಣಾಂಕ		ಕೊನೆ_change = dont_sync_undo == FALSE; // end undoable change

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
    ಆದರೆ ((fdo_flags & FDO_HOR) && ಕೀಲಿ_Typed)
	foldOpenCursor();
#ಆದರೆ_ಅಂತ್ಯ
    undisplay_dollar();
    tpos = curwin->w_cursor;
    ಆದರೆ (oneleft() == OK)
    {
#ಆದರೆ defined(FEAT_XIM) && defined(FEAT_GUI_GTK)
	// Only call start_arrow() when not busy with preediting, it will
	// break undo.  K_LEFT is inserted in im_correct_cursor().
	ಆದರೆ (p_imst == IM_OVER_THE_SPOT || !im_is_preediting())
#ಆದರೆ_ಅಂತ್ಯ
	{
	    ಆರಂಭ_arrow_with_change(&tpos, ಕೊನೆ_change);
	    ಆದರೆ (!ಕೊನೆ_change)
		Append_Char_ಗಮ್ಯ_Redobuff(K_ಎಡ);
	}
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RIGHTLEFT
	// If exit reversed string, position is fixed
	ಆದರೆ (revins_scol != -1 && (ಪೂರ್ಣಾಂಕ)curwin->w_cursor.ಲಂಬಸಾಲು >= revins_scol)
	    revins_legal++;
	revins_chars++;
#ಆದರೆ_ಅಂತ್ಯ
    }

    /*
     * if 'whichwrap' set for cursor in insert mode may go to
     * previous line
     */
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (vim_strchr(p_ww, '[') != ಖಾಲಿ && curwin->w_cursor.lnum > 1)
    {
	// always break undo when moving upwards/downwards, else undo may break
	ಆರಂಭ_arrow(&tpos);
	--(curwin->w_cursor.lnum);
	coladvance((colnr_T)MAXCOL);
	curwin->w_ಗಣ_curswant = true;	// so we stay at the end
    }
    ಇಲ್ಲದಿದ್ದರೆ
	vim_beep(BO_CRSR);
    dont_sync_undo = FALSE;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
ins_home(ಪೂರ್ಣಾಂಕ c)
{
    pos_T	tpos;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
    ಆದರೆ ((fdo_flags & FDO_HOR) && ಕೀಲಿ_Typed)
	foldOpenCursor();
#ಆದರೆ_ಅಂತ್ಯ
    undisplay_dollar();
    tpos = curwin->w_cursor;
    ಆದರೆ (c == K_C_HOME)
	curwin->w_cursor.lnum = 1;
    curwin->w_cursor.ಲಂಬಸಾಲು = 0;
    curwin->w_cursor.coladd = 0;
    curwin->w_curswant = 0;
    ಆರಂಭ_arrow(&tpos);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
ins_ಕೊನೆ(ಪೂರ್ಣಾಂಕ c)
{
    pos_T	tpos;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
    ಆದರೆ ((fdo_flags & FDO_HOR) && ಕೀಲಿ_Typed)
	foldOpenCursor();
#ಆದರೆ_ಅಂತ್ಯ
    undisplay_dollar();
    tpos = curwin->w_cursor;
    ಆದರೆ (c == K_C_ಕೊನೆ)
	curwin->w_cursor.lnum = curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
    coladvance((colnr_T)MAXCOL);
    curwin->w_curswant = MAXCOL;

    ಆರಂಭ_arrow(&tpos);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
ins_s_ಎಡ(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ ಕೊನೆ_change = dont_sync_undo == FALSE; // end undoable change
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
    ಆದರೆ ((fdo_flags & FDO_HOR) && ಕೀಲಿ_Typed)
	foldOpenCursor();
#ಆದರೆ_ಅಂತ್ಯ
    undisplay_dollar();
    ಆದರೆ (curwin->w_cursor.lnum > 1 || curwin->w_cursor.ಲಂಬಸಾಲು > 0)
    {
	ಆರಂಭ_arrow_with_change(&curwin->w_cursor, ಕೊನೆ_change);
	ಆದರೆ (!ಕೊನೆ_change)
	    Append_Char_ಗಮ್ಯ_Redobuff(K_S_ಎಡ);
	(ಶೂನ್ಯ)bck_ಶಬ್ದ(1L, FALSE, FALSE);
	curwin->w_ಗಣ_curswant = true;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	vim_beep(BO_CRSR);
    dont_sync_undo = FALSE;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
ins_ಬಲ(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ ಕೊನೆ_change = dont_sync_undo == FALSE; // end undoable change

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
    ಆದರೆ ((fdo_flags & FDO_HOR) && ಕೀಲಿ_Typed)
	foldOpenCursor();
#ಆದರೆ_ಅಂತ್ಯ
    undisplay_dollar();
    ಆದರೆ (gchar_cursor() != NUL || virtual_ಸಕ್ರಿಯ())
    {
	ಆರಂಭ_arrow_with_change(&curwin->w_cursor, ಕೊನೆ_change);
	ಆದರೆ (!ಕೊನೆ_change)
	    Append_Char_ಗಮ್ಯ_Redobuff(K_ಬಲ);
	curwin->w_ಗಣ_curswant = true;
	ಆದರೆ (virtual_ಸಕ್ರಿಯ())
	    oneright();
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಆದರೆ (has_mbyte)
		curwin->w_cursor.ಲಂಬಸಾಲು += (*mb_ptr2len)(ml_get_cursor());
	    ಇಲ್ಲದಿದ್ದರೆ
		++curwin->w_cursor.ಲಂಬಸಾಲು;
	}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RIGHTLEFT
	revins_legal++;
	ಆದರೆ (revins_chars)
	    revins_chars--;
#ಆದರೆ_ಅಂತ್ಯ
    }
    // if 'whichwrap' set for cursor in insert mode, may move the
    // cursor to the next line
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (vim_strchr(p_ww, ']') != ಖಾಲಿ
	    && curwin->w_cursor.lnum < curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
    {
	ಆರಂಭ_arrow(&curwin->w_cursor);
	curwin->w_ಗಣ_curswant = true;
	++curwin->w_cursor.lnum;
	curwin->w_cursor.ಲಂಬಸಾಲು = 0;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	vim_beep(BO_CRSR);
    dont_sync_undo = FALSE;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
ins_s_ಬಲ(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ ಕೊನೆ_change = dont_sync_undo == FALSE; // end undoable change
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
    ಆದರೆ ((fdo_flags & FDO_HOR) && ಕೀಲಿ_Typed)
	foldOpenCursor();
#ಆದರೆ_ಅಂತ್ಯ
    undisplay_dollar();
    ಆದರೆ (curwin->w_cursor.lnum < curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ
	    || gchar_cursor() != NUL)
    {
	ಆರಂಭ_arrow_with_change(&curwin->w_cursor, ಕೊನೆ_change);
	ಆದರೆ (!ಕೊನೆ_change)
	    Append_Char_ಗಮ್ಯ_Redobuff(K_S_ಬಲ);
	(ಶೂನ್ಯ)fwd_ಶಬ್ದ(1L, FALSE, 0);
	curwin->w_ಗಣ_curswant = true;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	vim_beep(BO_CRSR);
    dont_sync_undo = FALSE;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
ins_ಮೇಲೆ(
    ಪೂರ್ಣಾಂಕ		startcol)	// when TRUE move to Insstart.col
{
    pos_T	tpos;
    linenr_T	old_topline = curwin->w_topline;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
    ಪೂರ್ಣಾಂಕ		old_topfill = curwin->w_topfill;
#ಆದರೆ_ಅಂತ್ಯ

    undisplay_dollar();
    tpos = curwin->w_cursor;
    ಆದರೆ (cursor_ಮೇಲೆ(1L, TRUE) == OK)
    {
	ಆದರೆ (startcol)
	    coladvance(getvcol_nolist(&Insstart));
	ಆದರೆ (old_topline != curwin->w_topline
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
		|| old_topfill != curwin->w_topfill
#ಆದರೆ_ಅಂತ್ಯ
		)
	    redraw_later(UPD_VALID);
	ಆರಂಭ_arrow(&tpos);
	can_cindent = TRUE;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	vim_beep(BO_CRSR);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
ins_pageup(ಶೂನ್ಯ)
{
    pos_T	tpos;

    undisplay_dollar();

    ಆದರೆ (mod_ಮುಖವಾಡ & MOD_ಮುಖವಾಡ_CTRL)
    {
	// <C-PageUp>: tab page back
	ಆದರೆ (ಮೊದಲ_tabpage->tp_ಮುಂದಿನ != ಖಾಲಿ)
	{
	    ಆರಂಭ_arrow(&curwin->w_cursor);
	    goto_tabpage(-1);
	}
	ಹಿಂತಿರುಗಿ;
    }

    tpos = curwin->w_cursor;
    ಆದರೆ (pagescroll(BACKWARD, 1L, FALSE) == OK)
    {
	ಆರಂಭ_arrow(&tpos);
	can_cindent = TRUE;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	vim_beep(BO_CRSR);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
ins_ಕೆಳಕ್ಕೆ(
    ಪೂರ್ಣಾಂಕ		startcol)	// when TRUE move to Insstart.col
{
    pos_T	tpos;
    linenr_T	old_topline = curwin->w_topline;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
    ಪೂರ್ಣಾಂಕ		old_topfill = curwin->w_topfill;
#ಆದರೆ_ಅಂತ್ಯ

    undisplay_dollar();
    tpos = curwin->w_cursor;
    ಆದರೆ (cursor_ಕೆಳಕ್ಕೆ(1L, TRUE) == OK)
    {
	ಆದರೆ (startcol)
	    coladvance(getvcol_nolist(&Insstart));
	ಆದರೆ (old_topline != curwin->w_topline
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
		|| old_topfill != curwin->w_topfill
#ಆದರೆ_ಅಂತ್ಯ
		)
	    redraw_later(UPD_VALID);
	ಆರಂಭ_arrow(&tpos);
	can_cindent = TRUE;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	vim_beep(BO_CRSR);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
ins_pagedown(ಶೂನ್ಯ)
{
    pos_T	tpos;

    undisplay_dollar();

    ಆದರೆ (mod_ಮುಖವಾಡ & MOD_ಮುಖವಾಡ_CTRL)
    {
	// <C-PageDown>: tab page forward
	ಆದರೆ (ಮೊದಲ_tabpage->tp_ಮುಂದಿನ != ಖಾಲಿ)
	{
	    ಆರಂಭ_arrow(&curwin->w_cursor);
	    goto_tabpage(0);
	}
	ಹಿಂತಿರುಗಿ;
    }

    tpos = curwin->w_cursor;
    ಆದರೆ (pagescroll(FORWARD, 1L, FALSE) == OK)
    {
	ಆರಂಭ_arrow(&tpos);
	can_cindent = TRUE;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	vim_beep(BO_CRSR);
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DND
    ಸ್ಥಿರ ಶೂನ್ಯ
ins_drop(ಶೂನ್ಯ)
{
    do_put('~', ಖಾಲಿ, BACKWARD, 1L, PUT_CURSEND);
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Handle TAB in Insert or Replace mode.
 * Return TRUE when the TAB needs to be inserted like a normal character.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ins_tab(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ		ind;
    ಪೂರ್ಣಾಂಕ		i;
    ಪೂರ್ಣಾಂಕ		ತಾತ್ಕಾಲಿಕ;

    ಆದರೆ (Insstart_blank_vcol == MAXCOL && curwin->w_cursor.lnum == Insstart.lnum)
	Insstart_blank_vcol = get_nolist_virtcol();
    ಆದರೆ (echeck_abbr(TAB + ABBR_OFF))
	ಹಿಂತಿರುಗಿ FALSE;

    ind = inindent(0);
    ಆದರೆ (ind)
	can_cindent = FALSE;

    /*
     * When nothing special, insert TAB like a normal character.
     */
    ಆದರೆ (!curbuf->b_p_et
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_VARTABS
	    && !(p_sta && ind
		// These five lines mean 'tabstop' != 'shiftwidth'
		&& ((tabstop_ಎಣಿಕೆ(curbuf->b_p_vts_array) > 1)
		    || (tabstop_ಎಣಿಕೆ(curbuf->b_p_vts_array) == 1
			&& tabstop_ಮೊದಲ(curbuf->b_p_vts_array)
						       != get_sw_ಮೌಲ್ಯ(curbuf))
		    || (tabstop_ಎಣಿಕೆ(curbuf->b_p_vts_array) == 0
			&& curbuf->b_p_ts != get_sw_ಮೌಲ್ಯ(curbuf))))
	    && tabstop_ಎಣಿಕೆ(curbuf->b_p_vsts_array) == 0
#ಇಲ್ಲದಿದ್ದರೆ
	    && !(p_sta && ind && curbuf->b_p_ts != get_sw_ಮೌಲ್ಯ(curbuf))
#ಆದರೆ_ಅಂತ್ಯ
	    && get_sts_ಮೌಲ್ಯ() == 0)
	ಹಿಂತಿರುಗಿ TRUE;

    ಆದರೆ (stop_arrow() == FAIL)
	ಹಿಂತಿರುಗಿ TRUE;

    did_ai = FALSE;
    did_si = FALSE;
    can_si = FALSE;
    can_si_ಹಿಂಭಾಗ = FALSE;
    Append_ಗಮ್ಯ_Redobuff((char_u *)"\t");

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_VARTABS
    ಆದರೆ (p_sta && ind)		// insert tab in indent, use 'shiftwidth'
    {
	ತಾತ್ಕಾಲಿಕ = (ಪೂರ್ಣಾಂಕ)get_sw_ಮೌಲ್ಯ(curbuf);
	ತಾತ್ಕಾಲಿಕ -= get_nolist_virtcol() % ತಾತ್ಕಾಲಿಕ;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (tabstop_ಎಣಿಕೆ(curbuf->b_p_vsts_array) > 0 || curbuf->b_p_sts != 0)
				// use 'softtabstop' when set
	ತಾತ್ಕಾಲಿಕ = tabstop_padding(get_nolist_virtcol(), get_sts_ಮೌಲ್ಯ(),
						     curbuf->b_p_vsts_array);
    ಇಲ್ಲದಿದ್ದರೆ			// otherwise use 'tabstop'
	ತಾತ್ಕಾಲಿಕ = tabstop_padding(get_nolist_virtcol(), curbuf->b_p_ts,
						     curbuf->b_p_vts_array);
#ಇಲ್ಲದಿದ್ದರೆ
    ಆದರೆ (p_sta && ind)		// insert tab in indent, use 'shiftwidth'
	ತಾತ್ಕಾಲಿಕ = (ಪೂರ್ಣಾಂಕ)get_sw_ಮೌಲ್ಯ(curbuf);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (curbuf->b_p_sts != 0) // use 'softtabstop' when set
	ತಾತ್ಕಾಲಿಕ = (ಪೂರ್ಣಾಂಕ)get_sts_ಮೌಲ್ಯ();
    ಇಲ್ಲದಿದ್ದರೆ			// otherwise use 'tabstop'
	ತಾತ್ಕಾಲಿಕ = (ಪೂರ್ಣಾಂಕ)curbuf->b_p_ts;
    ತಾತ್ಕಾಲಿಕ -= get_nolist_virtcol() % ತಾತ್ಕಾಲಿಕ;
#ಆದರೆ_ಅಂತ್ಯ

    /*
     * Insert the first space with ins_char().	It will delete one char in
     * replace mode.  Insert the rest with ins_str(); it will not delete any
     * chars.  For MODE_VREPLACE state, we use ins_char() for all characters.
     */
    ins_char(' ');
    ಆಗ (--ತಾತ್ಕಾಲಿಕ > 0)
    {
	ಆದರೆ (State & VREPLACE_FLAG)
	    ins_char(' ');
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ins_str((char_u *)" ", 1);
	    ಆದರೆ (State & REPLACE_FLAG)	    // no char replaced
		replace_ತಳ್ಳು(NUL);
	}
    }

    /*
     * When 'expandtab' not set: Replace spaces by TABs where possible.
     */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_VARTABS
    ಆದರೆ (!curbuf->b_p_et && (tabstop_ಎಣಿಕೆ(curbuf->b_p_vsts_array) > 0
			    || get_sts_ಮೌಲ್ಯ() > 0
			    || (p_sta && ind)))
#ಇಲ್ಲದಿದ್ದರೆ
    ಆದರೆ (!curbuf->b_p_et && (get_sts_ಮೌಲ್ಯ() || (p_sta && ind)))
#ಆದರೆ_ಅಂತ್ಯ
    {
	char_u		*ptr;
	char_u		*saved_ಸಾಲು = ಖಾಲಿ;	// init for GCC
	pos_T		pos;
	pos_T		fpos;
	pos_T		*cursor;
	colnr_T		want_vcol, vcol;
	ಪೂರ್ಣಾಂಕ		change_ಲಂಬಸಾಲು = -1;
	ಪೂರ್ಣಾಂಕ		save_list = curwin->w_p_list;
	char_u		*tab = (char_u *)"\t";
	chartabsize_T	cts;

	/*
	 * Get the current line.  For MODE_VREPLACE state, don't make real
	 * changes yet, just work on a copy of the line.
	 */
	ಆದರೆ (State & VREPLACE_FLAG)
	{
	    pos = curwin->w_cursor;
	    cursor = &pos;
	    saved_ಸಾಲು = vim_strnsave(ml_get_curline(), ml_get_curline_len());
	    ಆದರೆ (saved_ಸಾಲು == ಖಾಲಿ)
		ಹಿಂತಿರುಗಿ FALSE;
	    ptr = saved_ಸಾಲು + pos.ಲಂಬಸಾಲು;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ptr = ml_get_cursor();
	    cursor = &curwin->w_cursor;
	}

	// When 'L' is not in 'cpoptions' a tab always takes up 'ts' spaces.
	ಆದರೆ (vim_strchr(p_cpo, CPO_LISTWM) == ಖಾಲಿ)
	    curwin->w_p_list = FALSE;

	// Find first white before the cursor
	fpos = curwin->w_cursor;
	ಆಗ (fpos.ಲಂಬಸಾಲು > 0 && VIM_ISWHITE(ptr[-1]))
	{
	    --fpos.ಲಂಬಸಾಲು;
	    --ptr;
	}

	// In Replace mode, don't change characters before the insert point.
	ಆದರೆ ((State & REPLACE_FLAG)
		&& fpos.lnum == Insstart.lnum
		&& fpos.ಲಂಬಸಾಲು < Insstart.ಲಂಬಸಾಲು)
	{
	    ptr += Insstart.ಲಂಬಸಾಲು - fpos.ಲಂಬಸಾಲು;
	    fpos.ಲಂಬಸಾಲು = Insstart.ಲಂಬಸಾಲು;
	}

	// compute virtual column numbers of first white and cursor
	getvcol(curwin, &fpos, &vcol, ಖಾಲಿ, ಖಾಲಿ, 0);
	getvcol(curwin, cursor, &want_vcol, ಖಾಲಿ, ಖಾಲಿ, 0);

	init_chartabsize_arg(&cts, curwin, 0, vcol, tab, tab);

	// Use as many TABs as possible.  Beware of 'breakindent', 'showbreak'
	// and 'linebreak' adding extra virtual columns.
	ಆಗ (VIM_ISWHITE(*ptr))
	{
	    i = lbr_chartabsize(&cts);
	    ಆದರೆ (cts.cts_vcol + i > want_vcol)
		ನಿಲ್ಲಿಸು;
	    ಆದರೆ (*ptr != TAB)
	    {
		*ptr = TAB;
		ಆದರೆ (change_ಲಂಬಸಾಲು < 0)
		{
		    change_ಲಂಬಸಾಲು = fpos.ಲಂಬಸಾಲು;  // Column of first change
		    // May have to adjust Insstart
		    ಆದರೆ (fpos.lnum == Insstart.lnum && fpos.ಲಂಬಸಾಲು < Insstart.ಲಂಬಸಾಲು)
			Insstart.ಲಂಬಸಾಲು = fpos.ಲಂಬಸಾಲು;
		}
	    }
	    ++fpos.ಲಂಬಸಾಲು;
	    ++ptr;
	    cts.cts_vcol += i;
	}
	vcol = cts.cts_vcol;
	clear_chartabsize_arg(&cts);

	ಆದರೆ (change_ಲಂಬಸಾಲು >= 0)
	{
	    ಪೂರ್ಣಾಂಕ		    repl_off = 0;

	    // Skip over the spaces we need.
	    init_chartabsize_arg(&cts, curwin, 0, vcol, ptr, ptr);
	    ಆಗ (cts.cts_vcol < want_vcol && *cts.cts_ptr == ' ')
	    {
		cts.cts_vcol += lbr_chartabsize(&cts);
		++cts.cts_ptr;
		++repl_off;
	    }
	    ptr = cts.cts_ptr;
	    vcol = cts.cts_vcol;
	    clear_chartabsize_arg(&cts);

	    ಆದರೆ (vcol > want_vcol)
	    {
		// Must have a char with 'showbreak' just before it.
		--ptr;
		--repl_off;
	    }
	    fpos.ಲಂಬಸಾಲು += repl_off;

	    // Delete following spaces.
	    i = cursor->ಲಂಬಸಾಲು - fpos.ಲಂಬಸಾಲು;
	    ಆದರೆ (i > 0)
	    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
		ಆದರೆ (!(State & VREPLACE_FLAG))
		{
		    char_u  *newp;
		    ಪೂರ್ಣಾಂಕ	    ಲಂಬಸಾಲು;

		    newp = alloc(curbuf->b_ml.ml_ಸಾಲು_len - i);
		    ಆದರೆ (newp == ಖಾಲಿ)
			ಹಿಂತಿರುಗಿ FALSE;

		    ಲಂಬಸಾಲು = ptr - curbuf->b_ml.ml_ಸಾಲು_ptr;
		    ಆದರೆ (ಲಂಬಸಾಲು > 0)
			mch_memmove(newp, ptr - ಲಂಬಸಾಲು, ಲಂಬಸಾಲು);
		    mch_memmove(newp + ಲಂಬಸಾಲು, ptr + i,
					   curbuf->b_ml.ml_ಸಾಲು_len - ಲಂಬಸಾಲು - i);

		    ಆದರೆ (curbuf->b_ml.ml_flags & (ML_ಸಾಲು_DIRTY | ML_ALLOCATED))
			vim_free(curbuf->b_ml.ml_ಸಾಲು_ptr);
		    curbuf->b_ml.ml_ಸಾಲು_ptr = newp;
		    curbuf->b_ml.ml_ಸಾಲು_len -= i;
		    curbuf->b_ml.ml_ಸಾಲು_textlen = 0;
		    curbuf->b_ml.ml_flags =
			   (curbuf->b_ml.ml_flags | ML_ಸಾಲು_DIRTY) & ~ML_ಖಾಲಿಗಳು;
		}
		ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
		    STRMOVE(ptr, ptr + i);
		// correct replace stack.
		ಆದರೆ ((State & REPLACE_FLAG) && !(State & VREPLACE_FLAG))
		    ಗಾಗಿ (ತಾತ್ಕಾಲಿಕ = i; --ತಾತ್ಕಾಲಿಕ >= 0; )
			replace_join(repl_off);
	    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_NETBEANS_INTG
	    ಆದರೆ (netbeans_ಸಕ್ರಿಯ())
	    {
		netbeans_removed(curbuf, fpos.lnum, cursor->ಲಂಬಸಾಲು, (ದೀರ್ಘ)(i + 1));
		netbeans_inserted(curbuf, fpos.lnum, cursor->ಲಂಬಸಾಲು,
							   (char_u *)"\t", 1);
	    }
#ಆದರೆ_ಅಂತ್ಯ
	    cursor->ಲಂಬಸಾಲು -= i;

	    /*
	     * In MODE_VREPLACE state, we haven't changed anything yet.  Do it
	     * now by backspacing over the changed spacing and then inserting
	     * the new spacing.
	     */
	    ಆದರೆ (State & VREPLACE_FLAG)
	    {
		// Backspace from real cursor to change_col
		backspace_until_ಸ್ತಂಭ(change_ಲಂಬಸಾಲು);

		// Insert each char in saved_line from changed_col to
		// ptr-cursor
		ins_ಬೈಟುಗಳು_len(saved_ಸಾಲು + change_ಲಂಬಸಾಲು,
						    cursor->ಲಂಬಸಾಲು - change_ಲಂಬಸಾಲು);
	    }
	}

	ಆದರೆ (State & VREPLACE_FLAG)
	    vim_free(saved_ಸಾಲು);
	curwin->w_p_list = save_list;
    }

    ಹಿಂತಿರುಗಿ FALSE;
}

/*
 * Handle CR or NL in insert mode.
 * Return FAIL when out of memory or can't undo.
 */
    ಪೂರ್ಣಾಂಕ
ins_eol(ಪೂರ್ಣಾಂಕ c)
{
    ಪೂರ್ಣಾಂಕ	    i;

    ಆದರೆ (echeck_abbr(c + ABBR_OFF))
	ಹಿಂತಿರುಗಿ OK;
    ಆದರೆ (stop_arrow() == FAIL)
	ಹಿಂತಿರುಗಿ FAIL;
    undisplay_dollar();

    /*
     * Strange Vi behaviour: In Replace mode, typing a NL will not delete the
     * character under the cursor.  Only push a NUL on the replace stack,
     * nothing to put back when the NL is deleted.
     */
    ಆದರೆ ((State & REPLACE_FLAG) && !(State & VREPLACE_FLAG))
	replace_ತಳ್ಳು(NUL);

    /*
     * In MODE_VREPLACE state, a NL replaces the rest of the line, and starts
     * replacing the next line, so we push all of the characters left on the
     * line onto the replace stack.  This is not done here though, it is done
     * in open_line().
     */

    // Put cursor on NUL if on the last char and coladd is 1 (happens after
    // CTRL-O).
    ಆದರೆ (virtual_ಸಕ್ರಿಯ() && curwin->w_cursor.coladd > 0)
	coladvance(getviscol());

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RIGHTLEFT
    // NL in reverse insert will always start in the end of
    // current line.
    ಆದರೆ (revins_on)
	curwin->w_cursor.ಲಂಬಸಾಲು += ml_get_cursor_len();
#ಆದರೆ_ಅಂತ್ಯ

    Append_ಗಮ್ಯ_Redobuff(NL_STR);
    i = open_ಸಾಲು(FORWARD,
	    has_format_option(FO_RET_COMS) ? OPENLINE_DO_COM : 0, old_indent,
	    ಖಾಲಿ);
    old_indent = 0;
    can_cindent = TRUE;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
    // When inserting a line the cursor line must never be in a closed fold.
    foldOpenCursor();
#ಆದರೆ_ಅಂತ್ಯ

    ಹಿಂತಿರುಗಿ i;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIGRAPHS
/*
 * Handle digraph in insert mode.
 * Returns character still to be inserted, or NUL when nothing remaining to be
 * done.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ins_digraph(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ	    c;
    ಪೂರ್ಣಾಂಕ	    cc;
    ಪೂರ್ಣಾಂಕ	    did_putchar = FALSE;

    pc_ಸ್ಥಿತಿ = PC_ಸ್ಥಿತಿ_UNSET;
    ಆದರೆ (redrawing() && !char_avail())
    {
	// may need to redraw when no more chars available now
	ins_redraw(FALSE);

	edit_putchar('?', TRUE);
	did_putchar = TRUE;
	ಕೂಡಿಸು_ಗಮ್ಯ_showcmd_c(Ctrl_K);
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
    ಆದರೆ (did_putchar)
	// when the line fits in 'columns' the '?' is at the start of the next
	// line and will not be removed by the redraw
	edit_unputchar();

    ಆದರೆ (IS_SPECIAL(c) || mod_ಮುಖವಾಡ)	    // special key
    {
	clear_showcmd();
	ಸೇರಿಸು_special(c, TRUE, FALSE);
	ಹಿಂತಿರುಗಿ NUL;
    }
    ಆದರೆ (c != ESC)
    {
	did_putchar = FALSE;
	ಆದರೆ (redrawing() && !char_avail())
	{
	    // may need to redraw when no more chars available now
	    ins_redraw(FALSE);

	    ಆದರೆ (char2cells(c) == 1)
	    {
		ins_redraw(FALSE);
		edit_putchar(c, TRUE);
		did_putchar = TRUE;
	    }
	    ಕೂಡಿಸು_ಗಮ್ಯ_showcmd_c(c);
	}
	++no_mapping;
	++allow_ಕೀಲಿಗಳು;
	cc = plain_vgetc();
	--no_mapping;
	--allow_ಕೀಲಿಗಳು;
	ಆದರೆ (did_putchar)
	    // when the line fits in 'columns' the '?' is at the start of the
	    // next line and will not be removed by a redraw
	    edit_unputchar();
	ಆದರೆ (cc != ESC)
	{
	    Append_ಗಮ್ಯ_Redobuff((char_u *)CTRL_V_STR);
	    c = digraph_get(c, cc, TRUE);
	    clear_showcmd();
	    ಹಿಂತಿರುಗಿ c;
	}
    }
    clear_showcmd();
    ಹಿಂತಿರುಗಿ NUL;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Handle CTRL-E and CTRL-Y in Insert mode: copy char from other line.
 * Returns the char to be inserted, or NUL if none found.
 */
    ಪೂರ್ಣಾಂಕ
ins_copychar(linenr_T lnum)
{
    ಪೂರ್ಣಾಂಕ		    c;
    char_u	    *ptr, *prev_ptr;
    char_u	    *ಸಾಲು;
    chartabsize_T   cts;

    ಆದರೆ (lnum < 1 || lnum > curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
    {
	vim_beep(BO_COPY);
	ಹಿಂತಿರುಗಿ NUL;
    }

    // try to advance to the cursor column
    validate_virtcol();
    ಸಾಲು = ml_get(lnum);
    prev_ptr = ಸಾಲು;
    init_chartabsize_arg(&cts, curwin, lnum, 0, ಸಾಲು, ಸಾಲು);
    ಆಗ (cts.cts_vcol < curwin->w_virtcol && *cts.cts_ptr != NUL)
    {
	prev_ptr = cts.cts_ptr;
	cts.cts_vcol += lbr_chartabsize_adv(&cts);
    }
    ಆದರೆ (cts.cts_vcol > curwin->w_virtcol)
	ptr = prev_ptr;
    ಇಲ್ಲದಿದ್ದರೆ
	ptr = cts.cts_ptr;
    clear_chartabsize_arg(&cts);

    c = (*mb_ptr2char)(ptr);
    ಆದರೆ (c == NUL)
	vim_beep(BO_COPY);
    ಹಿಂತಿರುಗಿ c;
}

/*
 * CTRL-Y or CTRL-E typed in Insert mode.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ins_ctrl_ey(ಪೂರ್ಣಾಂಕ tc)
{
    ಪೂರ್ಣಾಂಕ	    c = tc;

    ಆದರೆ (ctrl_x_ಬಹುಲಕ_scroll())
    {
	ಆದರೆ (c == Ctrl_Y)
	    scrolldown_clamp();
	ಇಲ್ಲದಿದ್ದರೆ
	    scrollup_clamp();
	redraw_later(UPD_VALID);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	c = ins_copychar(curwin->w_cursor.lnum + (c == Ctrl_Y ? -1 : 1));
	ಆದರೆ (c != NUL)
	{
	    ದೀರ್ಘ	tw_save;

	    // The character must be taken literally, insert like it
	    // was typed after a CTRL-V, and pretend 'textwidth'
	    // wasn't set.  Digits, 'o' and 'x' are special after a
	    // CTRL-V, don't use it for these.
	    ಆದರೆ (c < 256 && !ಸುರಕ್ಷಿತ_isalnum(c))
		Append_ಗಮ್ಯ_Redobuff((char_u *)CTRL_V_STR);	// CTRL-V
	    tw_save = curbuf->b_p_tw;
	    curbuf->b_p_tw = -1;
	    ಸೇರಿಸು_special(c, TRUE, FALSE);
	    curbuf->b_p_tw = tw_save;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RIGHTLEFT
	    revins_chars++;
	    revins_legal++;
#ಆದರೆ_ಅಂತ್ಯ
	    c = Ctrl_V;	// pretend CTRL-V is last character
	    auto_format(FALSE, TRUE);
	}
    }
    ಹಿಂತಿರುಗಿ c;
}

/*
 * Get the value that w_virtcol would have when 'list' is off.
 * Unless 'cpo' contains the 'L' flag.
 */
    colnr_T
get_nolist_virtcol(ಶೂನ್ಯ)
{
    // check validity of cursor in current buffer
    ಆದರೆ (curwin->w_ಬಫರ್ == ಖಾಲಿ
	|| curwin->w_ಬಫರ್->b_ml.ml_mfp == ಖಾಲಿ
	|| curwin->w_cursor.lnum > curwin->w_ಬಫರ್->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	ಹಿಂತಿರುಗಿ 0;
    ಆದರೆ (curwin->w_p_list && vim_strchr(p_cpo, CPO_LISTWM) == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ getvcol_nolist(&curwin->w_cursor);
    validate_virtcol();
    ಹಿಂತಿರುಗಿ curwin->w_virtcol;
}

#ಆದರೆ defined(FEAT_EVAL)
/*
 * Handle the InsertCharPre autocommand.
 * "c" is the character that was typed.
 * Return a pointer to allocated memory with the replacement string.
 * Return NULL to continue inserting "c".
 */
    ಸ್ಥಿರ char_u *
do_ಸೇರಿಸು_char_pre(ಪೂರ್ಣಾಂಕ c)
{
    char_u	*res;
    char_u	buf[MB_MAXBYTES + 1];
    size_t	buflen;
    ಪೂರ್ಣಾಂಕ		save_State = State;

    // Return quickly when there is nothing to do.
    ಆದರೆ (!has_insertcharpre())
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ಆದರೆ (c == Ctrl_RSB)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ಆದರೆ (has_mbyte)
	buflen = (*mb_char2bytes)(c, buf);
    ಇಲ್ಲದಿದ್ದರೆ
    {
	buf[0] = c;
	buflen = 1;
    }
    buf[buflen] = NUL;

    // Lock the text to avoid weird things from happening.
    ++textlock;
    ಗಣ_vim_var_string(VV_CHAR, buf, (ಪೂರ್ಣಾಂಕ)buflen);  // set v:char

    res = ಖಾಲಿ;
    ಆದರೆ (ins_apply_autocmds(EVENT_INSERTCHARPRE))
    {
	// Get the value of v:char.  It may be empty or more than one
	// character.  Only use it when changed, otherwise continue with the
	// original character to avoid breaking autoindent.
	ಆದರೆ (STRCMP(buf, get_vim_var_str(VV_CHAR)) != 0)
	    res = vim_strsave(get_vim_var_str(VV_CHAR));
    }

    ಗಣ_vim_var_string(VV_CHAR, ಖಾಲಿ, -1);  // clear v:char
    --textlock;

    // Restore the State, it may have been changed.
    State = save_State;

    ಹಿಂತಿರುಗಿ res;
}
#ಆದರೆ_ಅಂತ್ಯ

    ಪೂರ್ಣಾಂಕ
get_can_cindent(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ can_cindent;
}

    ಶೂನ್ಯ
ಗಣ_can_cindent(ಪೂರ್ಣಾಂಕ val)
{
    can_cindent = val;
}

/*
 * Trigger "event" and take care of fixing undo.
 */
    ಪೂರ್ಣಾಂಕ
ins_apply_autocmds(event_T event)
{
    varnumber_T	tick = CHANGEDTICK(curbuf);
    ಪೂರ್ಣಾಂಕ r;

    r = apply_autocmds(event, ಖಾಲಿ, ಖಾಲಿ, FALSE, curbuf);

    // If u_savesub() was called then we are not prepared to start
    // a new line.  Call u_save() with no contents to fix that.
    // Except when leaving Insert mode.
    ಆದರೆ (event != EVENT_INSERTLEAVE && tick != CHANGEDTICK(curbuf))
	u_save(curwin->w_cursor.lnum, (linenr_T)(curwin->w_cursor.lnum + 1));

    ಹಿಂತಿರುಗಿ r;
}
