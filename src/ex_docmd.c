/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

/*
 * ex_docmd.c: functions for executing an Ex command line.
 */

#include "vim.h"

static පූර්ණ_සංඛ්‍යා	quitmore = 0;
static පූර්ණ_සංඛ්‍යා	ex_pressedreturn = FALSE;
#ifndef FEAT_PRINTER
# define ex_hardcopy	ex_ni
#endif

#ifdef FEAT_EVAL
static char_u	*do_one_cmd(char_u **, පූර්ණ_සංඛ්‍යා, cstack_T *, char_u *(*fgetline)(පූර්ණ_සංඛ්‍යා, void *, පූර්ණ_සංඛ්‍යා, getline_opt_T), void *cookie);
#නැතහොත්
static char_u	*do_one_cmd(char_u **, පූර්ණ_සංඛ්‍යා, char_u *(*fgetline)(පූර්ණ_සංඛ්‍යා, void *, පූර්ණ_සංඛ්‍යා, getline_opt_T), void *cookie);
static පූර්ණ_සංඛ්‍යා	if_level = 0;		// depth in :if
#endif
static void	append_command(char_u *cmd);

#ifndef FEAT_MENU
# define ex_emenu		ex_ni
# define ex_menu		ex_ni
# define ex_menutranslate	ex_ni
#endif
static void	ex_autocmd(exarg_T *eap);
static void	ex_doautocmd(exarg_T *eap);
static void	ex_bunload(exarg_T *eap);
static void	ex_buffer(exarg_T *eap);
static void	do_exbuffer(exarg_T *eap);
static void	ex_bmodified(exarg_T *eap);
static void	ex_bnext(exarg_T *eap);
static void	ex_bprevious(exarg_T *eap);
static void	ex_brewind(exarg_T *eap);
static void	ex_blast(exarg_T *eap);
static char_u	*getargcmd(char_u **);
static පූර්ණ_සංඛ්‍යා	getargopt(exarg_T *eap);
#ifndef FEAT_QUICKFIX
# define ex_make		ex_ni
# define ex_cbuffer		ex_ni
# define ex_cc			ex_ni
# define ex_cnext		ex_ni
# define ex_cbelow		ex_ni
# define ex_cfile		ex_ni
# define qf_list		ex_ni
# define qf_age			ex_ni
# define qf_history		ex_ni
# define ex_helpgrep		ex_ni
# define ex_vimgrep		ex_ni
# define ex_cclose		ex_ni
# define ex_copen		ex_ni
# define ex_cwindow		ex_ni
# define ex_cbottom		ex_ni
#endif
#නම් !defined(FEAT_QUICKFIX) || !defined(FEAT_EVAL)
# define ex_cexpr		ex_ni
#endif

static linenr_T default_address(exarg_T *eap);
static void address_default_all(exarg_T *eap);
static void	get_flags(exarg_T *eap);
#නම් !defined(FEAT_PERL) \
	|| !defined(FEAT_PYTHON) || !defined(FEAT_PYTHON3) \
	|| !defined(FEAT_TCL) \
	|| !defined(FEAT_RUBY) \
	|| !defined(FEAT_LUA) \
	|| !defined(FEAT_MZSCHEME)
# define HAVE_EX_SCRIPT_NI
static void	ex_script_ni(exarg_T *eap);
#endif
static var_na	*invalid_range(exarg_T *eap);
static void	correct_range(exarg_T *eap);
#ifdef FEAT_QUICKFIX
static char_u	*replace_makeprg(exarg_T *eap, char_u *p, char_u **cmdlinep);
#endif
static char_u	*repl_cmdline(exarg_T *eap, char_u *src, size_t srclen, char_u *repl, char_u **cmdlinep);
static void	ex_highlight(exarg_T *eap);
static void	ex_colorscheme(exarg_T *eap);
static void	ex_cquit(exarg_T *eap);
static void	ex_quit_all(exarg_T *eap);
static void	ex_close(exarg_T *eap);
static void	ex_win_close(පූර්ණ_සංඛ්‍යා forceit, win_T *win, tabpage_T *tp);
static void	ex_only(exarg_T *eap);
static void	ex_resize(exarg_T *eap);
static void	ex_stag(exarg_T *eap);
static void	ex_tabclose(exarg_T *eap);
static void	ex_tabonly(exarg_T *eap);
static void	ex_tabnext(exarg_T *eap);
static void	ex_tabmove(exarg_T *eap);
static void	ex_tabs(exarg_T *eap);
#නම් defined(FEAT_QUICKFIX)
static void	ex_pclose(exarg_T *eap);
static void	ex_ptag(exarg_T *eap);
static void	ex_pedit(exarg_T *eap);
static void	ex_pbuffer(exarg_T *eap);
static void	prepare_preview_window(void);
static void	back_to_current_window(win_T *curwin_save);
#නැතහොත්
# define ex_pclose		ex_ni
# define ex_ptag		ex_ni
# define ex_pedit		ex_ni
# define ex_pbuffer		ex_ni
#endif
static void	ex_hide(exarg_T *eap);
static void	ex_exit(exarg_T *eap);
static void	ex_print(exarg_T *eap);
#ifdef FEAT_BYTEOFF
static void	ex_goto(exarg_T *eap);
#නැතහොත්
# define ex_goto		ex_ni
#endif
static void	ex_shell(exarg_T *eap);
static void	ex_preserve(exarg_T *eap);
static void	ex_recover(exarg_T *eap);
static void	ex_mode(exarg_T *eap);
static void	ex_wrongmodifier(exarg_T *eap);
static void	ex_find(exarg_T *eap);
static void	ex_open(exarg_T *eap);
static void	ex_edit(exarg_T *eap);
#ifndef FEAT_GUI
# define ex_gui			ex_nogui
static void	ex_nogui(exarg_T *eap);
#endif
#නම් defined(FEAT_GUI_MSWIN) && defined(FEAT_MENU) && defined(FEAT_TEAROFF)
static void	ex_tearoff(exarg_T *eap);
#නැතහොත්
# define ex_tearoff		ex_ni
#endif
#නම් (defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_GTK) \
	|| defined(FEAT_TERM_POPUP_MENU)) && defined(FEAT_MENU)
static void	ex_popup(exarg_T *eap);
#නැතහොත්
# define ex_popup		ex_ni
#endif
#ifndef FEAT_GUI_MSWIN
# define ex_simalt		ex_ni
#endif
#නම් !defined(FEAT_GUI_MSWIN) && !defined(FEAT_GUI_GTK) && !defined(FEAT_GUI_MOTIF)
# define gui_mch_find_dialog	ex_ni
# define gui_mch_replace_dialog ex_ni
#endif
#නම් !defined(FEAT_GUI_GTK)
# define ex_helpfind		ex_ni
#endif
#ifndef FEAT_CSCOPE
# define ex_cscope		ex_ni
# define ex_scscope		ex_ni
# define ex_cstag		ex_ni
#endif
#ifndef FEAT_SYN_HL
# define ex_syntax		ex_ni
# define ex_ownsyntax		ex_ni
#endif
#නම් !defined(FEAT_SYN_HL) || !defined(FEAT_PROFILE)
# define ex_syntime		ex_ni
#endif
#ifndef FEAT_SPELL
# define ex_spell		ex_ni
# define ex_mkspell		ex_ni
# define ex_spelldump		ex_ni
# define ex_spellinfo		ex_ni
# define ex_spellrepall		ex_ni
#endif
#ifndef FEAT_PERSISTENT_UNDO
# define ex_rundo		ex_ni
# define ex_wundo		ex_ni
#endif
#ifndef FEAT_LUA
# define ex_lua			ex_script_ni
# define ex_luado		ex_ni
# define ex_luafile		ex_ni
#endif
#ifndef FEAT_MZSCHEME
# define ex_mzscheme		ex_script_ni
# define ex_mzfile		ex_ni
#endif
#ifndef FEAT_PERL
# define ex_perl		ex_script_ni
# define ex_perldo		ex_ni
#endif
#ifndef FEAT_PYTHON
# define ex_python		ex_script_ni
# define ex_pydo		ex_ni
# define ex_pyfile		ex_ni
#endif
#ifndef FEAT_PYTHON3
# define ex_py3			ex_script_ni
# define ex_py3do		ex_ni
# define ex_py3file		ex_ni
#endif
#නම් !defined(FEAT_PYTHON) && !defined(FEAT_PYTHON3)
# define ex_pyx			ex_script_ni
# define ex_pyxdo		ex_ni
# define ex_pyxfile		ex_ni
#endif
#ifndef FEAT_TCL
# define ex_tcl			ex_script_ni
# define ex_tcldo		ex_ni
# define ex_tclfile		ex_ni
#endif
#ifndef FEAT_RUBY
# define ex_ruby		ex_script_ni
# define ex_rubydo		ex_ni
# define ex_rubyfile		ex_ni
#endif
#ifndef FEAT_KEYMAP
# define ex_loadkeymap		ex_ni
#endif
static void	ex_swapname(exarg_T *eap);
static void	ex_syncbind(exarg_T *eap);
static void	ex_read(exarg_T *eap);
static void	ex_pwd(exarg_T *eap);
static void	ex_equal(exarg_T *eap);
static void	ex_sleep(exarg_T *eap);
static void	ex_winsize(exarg_T *eap);
static void	ex_wincmd(exarg_T *eap);
#නම් defined(FEAT_GUI) || defined(UNIX) || defined(VMS) || defined(MSWIN)
static void	ex_winpos(exarg_T *eap);
#නැතහොත්
# define ex_winpos	    ex_ni
#endif
static void	ex_operators(exarg_T *eap);
static void	ex_put(exarg_T *eap);
static void	ex_iput(exarg_T *eap);
static void	ex_copymove(exarg_T *eap);
static void	ex_submagic(exarg_T *eap);
static void	ex_join(exarg_T *eap);
static void	ex_at(exarg_T *eap);
static void	ex_bang(exarg_T *eap);
static void	ex_undo(exarg_T *eap);
#ifdef FEAT_PERSISTENT_UNDO
static void	ex_wundo(exarg_T *eap);
static void	ex_rundo(exarg_T *eap);
#endif
static void	ex_redo(exarg_T *eap);
static void	ex_later(exarg_T *eap);
static void	ex_redir(exarg_T *eap);
static void	ex_redrawstatus(exarg_T *eap);
static void	ex_redrawtabline(exarg_T *eap);
static void	ex_redrawtabpanel(exarg_T *eap);
static void	close_redir(void);
static void	ex_mark(exarg_T *eap);
static void	ex_startinsert(exarg_T *eap);
static void	ex_stopinsert(exarg_T *eap);
#ifdef FEAT_FIND_ID
static void	ex_checkpath(exarg_T *eap);
static void	ex_findpat(exarg_T *eap);
#නැතහොත්
# define ex_findpat		ex_ni
# define ex_checkpath		ex_ni
#endif
#නම් defined(FEAT_FIND_ID) && defined(FEAT_QUICKFIX)
static void	ex_psearch(exarg_T *eap);
#නැතහොත්
# define ex_psearch		ex_ni
#endif
static void	ex_tag(exarg_T *eap);
static void	ex_tag_cmd(exarg_T *eap, char_u *name);
#ifndef FEAT_EVAL
# define ex_block		ex_ni
# define ex_break		ex_ni
# define ex_breakadd		ex_ni
# define ex_breakdel		ex_ni
# define ex_breaklist		ex_ni
# define ex_call		ex_ni
# define ex_catch		ex_ni
# define ex_class		ex_ni
# define ex_compiler		ex_ni
# define ex_continue		ex_ni
# define ex_debug		ex_ni
# define ex_debuggreedy		ex_ni
# define ex_defcompile		ex_ni
# define ex_delfunction		ex_ni
# define ex_disassemble		ex_ni
# define ex_echo		ex_ni
# define ex_echohl		ex_ni
# define ex_else		ex_ni
# define ex_endblock		ex_ni
# define ex_endfunction		ex_ni
# define ex_endif		ex_ni
# define ex_endtry		ex_ni
# define ex_endwhile		ex_ni
# define ex_eval		ex_ni
# define ex_execute		ex_ni
# define ex_finally		ex_ni
# define ex_incdec		ex_ni
# define ex_finish		ex_ni
# define ex_function		ex_ni
# define ex_if			ex_ni
# define ex_let			ex_ni
# define ex_var			ex_ni
# define ex_lockvar		ex_ni
# define ex_oldfiles		ex_ni
# define ex_options		ex_ni
# define ex_packadd		ex_ni
# define ex_packloadall		ex_ni
# define ex_return		ex_ni
# define ex_scriptnames		ex_ni
# define ex_throw		ex_ni
# define ex_try			ex_ni
# define ex_type		ex_ni
# define ex_unlet		ex_ni
# define ex_while		ex_ni
# define ex_import		ex_ni
# define ex_export		ex_ni
#endif
#ifndef FEAT_SESSION
# define ex_loadview		ex_ni
#endif
#ifndef FEAT_VIMINFO
# define ex_viminfo		ex_ni
#endif
static void	ex_behave(exarg_T *eap);
static void	ex_filetype(exarg_T *eap);
static void	ex_setfiletype(exarg_T *eap);
#ifndef FEAT_DIFF
# define ex_diffoff		ex_ni
# define ex_diffpatch		ex_ni
# define ex_diffgetput		ex_ni
# define ex_diffsplit		ex_ni
# define ex_diffthis		ex_ni
# define ex_diffupdate		ex_ni
#endif
static void	ex_digraphs(exarg_T *eap);
#ifdef FEAT_SEARCH_EXTRA
static void	ex_nohlsearch(exarg_T *eap);
#නැතහොත්
# define ex_nohlsearch		ex_ni
# define ex_match		ex_ni
#endif
#ifdef FEAT_CRYPT
static void	ex_X(exarg_T *eap);
#නැතහොත්
# define ex_X			ex_ni
#endif
#ifdef FEAT_FOLDING
static void	ex_fold(exarg_T *eap);
static void	ex_foldopen(exarg_T *eap);
static void	ex_folddo(exarg_T *eap);
#නැතහොත්
# define ex_fold		ex_ni
# define ex_foldopen		ex_ni
# define ex_folddo		ex_ni
#endif
#නම් !(defined(HAVE_LOCALE_H) || defined(X_LOCALE))
# define ex_language		ex_ni
#endif
#ifndef FEAT_SIGNS
# define ex_sign		ex_ni
#endif
#ifndef FEAT_NETBEANS_INTG
# define ex_nbclose		ex_ni
# define ex_nbkey		ex_ni
# define ex_nbstart		ex_ni
#endif

#ifndef FEAT_PROFILE
# define ex_profile		ex_ni
#endif
#ifndef FEAT_TERMINAL
# define ex_terminal		ex_ni
#endif
#නම් !defined(FEAT_X11) || !defined(FEAT_XCLIPBOARD)
# define ex_xrestore		ex_ni
#endif
#නම් !defined(FEAT_WAYLAND)
# define ex_wlrestore		ex_ni
#endif
#නම් !defined(HAVE_CLIPMETHOD)
# define ex_clipreset		ex_ni
#endif
#නම් !defined(FEAT_PROP_POPUP)
# define ex_popupclear		ex_ni
#endif

/*
 * Declare cmdnames[].
 */
#define DO_DECLARE_EXCMD
#include "ex_cmds.h"
#include "ex_cmdidxs.h"

static char_u dollar_command[2] = {'$', 0};


#ifdef FEAT_EVAL
// Struct for storing a line inside a while/for loop
typedef sa_nracanaa
{
    char_u	*line;		// command line
    linenr_T	lnum;		// sourcing_lnum of the line
} wcmd_T;

/*
 * Structure used to store info for line position in a while or for loop.
 * This is required, because do_one_cmd() may invoke ex_function(), which
 * reads more lines that may come from the while/for loop.
 */
sa_nracanaa loop_cookie
{
    garray_T	*lines_gap;		// growarray with line info
    පූර්ණ_සංඛ්‍යා		current_line;		// last read line from growarray
    පූර්ණ_සංඛ්‍යා		repeating;		// TRUE when looping a second time
    // When "repeating" is FALSE use "getline" and "cookie" to get lines
    char_u	*(*lc_getline)(පූර්ණ_සංඛ්‍යා, void *, පූර්ණ_සංඛ්‍යා, getline_opt_T);
    void	*cookie;
};

static char_u	*get_loop_line(පූර්ණ_සංඛ්‍යා c, void *cookie, පූර්ණ_සංඛ්‍යා indent, getline_opt_T options);
static පූර්ණ_සංඛ්‍යා	store_loop_line(garray_T *gap, char_u *line);
static void	free_cmdlines(garray_T *gap);

// Struct to save a few things while debugging.  Used in do_cmdline() only.
sa_nracanaa dbg_stuff
{
    පූර්ණ_සංඛ්‍යා		trylevel;
    පූර්ණ_සංඛ්‍යා		force_abort;
    except_T	*caught_stack;
    char_u	*vv_exception;
    char_u	*vv_throwpoint;
    පූර්ණ_සංඛ්‍යා		did_emsg;
    පූර්ණ_සංඛ්‍යා		got_int;
    පූර්ණ_සංඛ්‍යා		did_throw;
    පූර්ණ_සංඛ්‍යා		need_rethrow;
    පූර්ණ_සංඛ්‍යා		check_cstack;
    except_T	*current_exception;
};

    static void
save_dbg_stuff(sa_nracanaa dbg_stuff *dsp)
{
    dsp->trylevel	= trylevel;		trylevel = 0;
    dsp->force_abort	= force_abort;		force_abort = FALSE;
    dsp->caught_stack	= caught_stack;		caught_stack = NULL;
    dsp->vv_exception	= v_exception(NULL);
    dsp->vv_throwpoint	= v_throwpoint(NULL);

    // Necessary for debugging an inactive ":catch", ":finally", ":endtry"
    dsp->did_emsg	= did_emsg;		did_emsg     = FALSE;
    dsp->got_int	= got_int;		got_int	     = FALSE;
    dsp->did_throw	= did_throw;		did_throw    = FALSE;
    dsp->need_rethrow	= need_rethrow;		need_rethrow = FALSE;
    dsp->check_cstack	= check_cstack;		check_cstack = FALSE;
    dsp->current_exception = current_exception;	current_exception = NULL;
}

    static void
restore_dbg_stuff(sa_nracanaa dbg_stuff *dsp)
{
    suppress_errthrow = FALSE;
    trylevel = dsp->trylevel;
    force_abort = dsp->force_abort;
    caught_stack = dsp->caught_stack;
    (void)v_exception(dsp->vv_exception);
    (void)v_throwpoint(dsp->vv_throwpoint);
    did_emsg = dsp->did_emsg;
    got_int = dsp->got_int;
    did_throw = dsp->did_throw;
    need_rethrow = dsp->need_rethrow;
    check_cstack = dsp->check_cstack;
    current_exception = dsp->current_exception;
}
#endif

/*
 * Check if ffname differs from fnum.
 * fnum is a buffer number. 0 == current buffer, 1-or-more must be a valid
 * buffer ID.
 * ffname is a full path to where a buffer lives on-disk or would live on-disk.
 *
 */
    static පූර්ණ_සංඛ්‍යා
is_other_file(පූර්ණ_සංඛ්‍යා fnum, char_u *ffname)
{
    නම් (fnum != 0)
    {
	නම් (fnum == curbuf->b_fnum)
	    ආපසු FALSE;

	ආපසු TRUE;
    }

    නම් (ffname == NULL)
	ආපසු TRUE;

    නම් (*ffname == NUL)
	ආපසු FALSE;

    // TODO: Need a reliable way to know whether a buffer is meant to live
    // on-disk !curbuf->b_dev_valid is not always available (example: missing
    // on Windows)
    නම් (curbuf->b_sfname != NULL
	    && *curbuf->b_sfname != NUL)
	// This occurs with unsaved buffers. In which case `ffname` actually
	// corresponds to curbuf->b_sfname
	ආපසු fnamecmp(ffname, curbuf->b_sfname) != 0;

    ආපසු otherfile(ffname);
}

/*
 * do_exmode(): Repeatedly get commands for the "Ex" mode, until the ":vi"
 * command is given.
 */
    void
do_exmode(
    පූර්ණ_සංඛ්‍යා		improved)	    // TRUE for "improved Ex" mode
{
    පූර්ණ_සංඛ්‍යා		save_msg_scroll;
    පූර්ණ_සංඛ්‍යා		prev_msg_row;
    linenr_T	prev_line;
    varnumber_T	changedtick;

    නම් (improved)
	exmode_active = EXMODE_VIM;
    නැතහොත්
	exmode_active = EXMODE_NORMAL;
    State = MODE_NORMAL;
    may_trigger_modechanged();

    // When using ":global /pat/ visual" and then "Q" we return to continue
    // the :global command.
    නම් (global_busy)
	ආපසු;

    save_msg_scroll = msg_scroll;
    ++RedrawingDisabled;	    // don't redisplay the window
    ++no_wait_return;		    // don't wait for return
#ifdef FEAT_GUI
    // Ignore scrollbar and mouse events in Ex mode
    ++hold_gui_events;
#endif

    msg(_("Entering Ex mode.  Type \"visual\" to go to Normal mode."));
    අතරතුර (exmode_active)
    {
	// Check for a ":normal" command and no more characters left.
	නම් (ex_normal_busy > 0 && typebuf.tb_len == 0)
	{
	    exmode_active = FALSE;
	    bha_nga;
	}
	msg_scroll = TRUE;
	need_wait_return = FALSE;
	ex_pressedreturn = FALSE;
	ex_no_reprint = FALSE;
	changedtick = CHANGEDTICK(curbuf);
	prev_msg_row = msg_row;
	prev_line = curwin->w_cursor.lnum;
	නම් (improved)
	{
	    cmdline_row = msg_row;
	    do_cmdline(NULL, getexline, NULL, 0);
	}
	නැතහොත්
	    do_cmdline(NULL, getexmodeline, NULL, DOCMD_NOWAIT);
	lines_left = Rows - 1;

	නම් ((prev_line != curwin->w_cursor.lnum
		   || changedtick != CHANGEDTICK(curbuf)) && !ex_no_reprint)
	{
	    නම් (curbuf->b_ml.ml_flags & ML_EMPTY)
		emsg(_(e_empty_buffer));
	    නැතහොත්
	    {
		නම් (ex_pressedreturn)
		{
		    // go up one line, to overwrite the ":<CR>" line, so the
		    // output doesn't contain empty lines.
		    msg_row = prev_msg_row;
		    නම් (prev_msg_row == Rows - 1)
			msg_row--;
		}
		msg_col = 0;
		print_line_no_prefix(curwin->w_cursor.lnum, FALSE, FALSE);
		msg_clr_eos();
	    }
	}
	නැතහොත් නම් (ex_pressedreturn && !ex_no_reprint)	// must be at EOF
	{
	    නම් (curbuf->b_ml.ml_flags & ML_EMPTY)
		emsg(_(e_empty_buffer));
	    නැතහොත්
		emsg(_(e_at_end_of_file));
	}
    }

#ifdef FEAT_GUI
    --hold_gui_events;
#endif
    නම් (RedrawingDisabled > 0)
	--RedrawingDisabled;
    --no_wait_return;
    update_screen(UPD_CLEAR);
    need_wait_return = FALSE;
    msg_scroll = save_msg_scroll;
}

/*
 * Print the executed command for when 'verbose' is set.
 * When "lnum" is 0 only print the command.
 */
    static void
msg_verbose_cmd(linenr_T lnum, char_u *cmd)
{
    ++no_wait_return;
    verbose_enter_scroll();

    නම් (lnum == 0)
	smsg(_("Executing: %s"), cmd);
    නැතහොත්
	smsg(_("line %ld: %s"), (long)lnum, cmd);
    නම් (msg_silent == 0)
	msg_puts("\n");   // don't overwrite this

    verbose_leave_scroll();
    --no_wait_return;
}

/*
 * Execute a simple command line.  Used for translated commands like "*".
 */
    පූර්ණ_සංඛ්‍යා
do_cmdline_cmd(char_u *cmd)
{
    ආපසු do_cmdline(cmd, NULL, NULL,
				   DOCMD_VERBOSE|DOCMD_NOWAIT|DOCMD_KEYTYPED);
}

/*
 * Execute the "+cmd" argument of "edit +cmd fname" and the like.
 * This allows for using a range without ":" in Vim9 script.
 */
    static පූර්ණ_සංඛ්‍යා
do_cmd_argument(char_u *cmd)
{
    ආපසු do_cmdline(cmd, NULL, NULL,
		      DOCMD_VERBOSE|DOCMD_NOWAIT|DOCMD_KEYTYPED|DOCMD_RANGEOK);
}

/*
 * do_cmdline(): execute one Ex command line
 *
 * 1. Execute "cmdline" when it is not NULL.
 *    If "cmdline" is NULL, or more lines are needed, fgetline() is used.
 * 2. Split up in parts separated with '|'.
 *
 * This function can be called recursively!
 *
 * flags:
 * DOCMD_VERBOSE  - The command will be included in the error message.
 * DOCMD_NOWAIT   - Don't call wait_return() and friends.
 * DOCMD_REPEAT   - Repeat execution until fgetline() returns NULL.
 * DOCMD_KEYTYPED - Don't reset KeyTyped.
 * DOCMD_EXCRESET - Reset the exception environment (used for debugging).
 * DOCMD_KEEPLINE - Store first typed line (for repeating with ".").
 *
 * return FAIL if cmdline could not be executed, OK otherwise
 */
    පූර්ණ_සංඛ්‍යා
do_cmdline(
    char_u	*cmdline,
    char_u	*(*fgetline)(පූර්ණ_සංඛ්‍යා, void *, පූර්ණ_සංඛ්‍යා, getline_opt_T),
    void	*cookie,		// argument for fgetline()
    පූර්ණ_සංඛ්‍යා		flags)
{
    char_u	*next_cmdline;		// next cmd to execute
    char_u	*cmdline_copy = NULL;	// copy of cmd line
    පූර්ණ_සංඛ්‍යා		used_getline = FALSE;	// used "fgetline" to obtain command
    static පූර්ණ_සංඛ්‍යා	recursive = 0;		// recursive depth
    පූර්ණ_සංඛ්‍යා		msg_didout_before_start = 0;
    පූර්ණ_සංඛ්‍යා		count = 0;		// line number count
    පූර්ණ_සංඛ්‍යා		did_inc_RedrawingDisabled = FALSE;
    පූර්ණ_සංඛ්‍යා		retval = OK;
#ifdef FEAT_EVAL
    cstack_T	cstack;			// conditional stack
    garray_T	lines_ga;		// keep lines for ":while"/":for"
    පූර්ණ_සංඛ්‍යා		current_line = 0;	// active line in lines_ga
    පූර්ණ_සංඛ්‍යා		current_line_before = 0;
    char_u	*fname = NULL;		// function or script name
    linenr_T	*breakpoint = NULL;	// ptr to breakpoint field in cookie
    පූර්ණ_සංඛ්‍යා		*dbg_tick = NULL;	// ptr to dbg_tick field in cookie
    sa_nracanaa dbg_stuff debug_saved;	// saved things for debug mode
    පූර්ණ_සංඛ්‍යා		initial_trylevel;
    msglist_T	**saved_msg_list = NULL;
    msglist_T	*private_msg_list = NULL;

    // "fgetline" and "cookie" passed to do_one_cmd()
    char_u	*(*cmd_getline)(පූර්ණ_සංඛ්‍යා, void *, පූර්ණ_සංඛ්‍යා, getline_opt_T);
    void	*cmd_cookie;
    sa_nracanaa loop_cookie cmd_loop_cookie;
    void	*real_cookie;
    පූර්ණ_සංඛ්‍යා		getline_is_func;
#නැතහොත්
# define cmd_getline fgetline
# define cmd_cookie cookie
#endif
    static පූර්ණ_සංඛ්‍යා	call_depth = 0;		// recursiveness
#ifdef FEAT_EVAL
    // For every pair of do_cmdline()/do_one_cmd() calls, use an extra memory
    // location for storing error messages to be converted to an exception.
    // This ensures that the do_errthrow() call in do_one_cmd() does not
    // combine the messages stored by an earlier invocation of do_one_cmd()
    // with the command name of the later one.  This would happen when
    // BufWritePost autocommands are executed after a write error.
    saved_msg_list = msg_list;
    msg_list = &private_msg_list;
#endif

    // It's possible to create an endless loop with ":execute", catch that
    // here.  The value of 200 allows nested function calls, ":source", etc.
    // Allow 200 or 'maxfuncdepth', whatever is larger.
    නම් (call_depth >= 200
#ifdef FEAT_EVAL
	    && call_depth >= p_mfd
#endif
	    )
    {
	emsg(_(e_command_too_recursive));
#ifdef FEAT_EVAL
	// When converting to an exception, we do not include the command name
	// since this is not an error of the specific command.
	do_errthrow((cstack_T *)NULL, (char_u *)NULL);
	msg_list = saved_msg_list;
#endif
	ආපසු FAIL;
    }
    ++call_depth;

#ifdef FEAT_EVAL
    CLEAR_FIELD(cstack);
    cstack.cs_idx = -1;
    ga_init2(&lines_ga, parimaa_na(wcmd_T), 10);

    real_cookie = getline_cookie(fgetline, cookie);

    // Inside a function use a higher nesting level.
    getline_is_func = getline_equal(fgetline, cookie, get_func_line);
    නම් (getline_is_func && ex_nesting_level == func_level(real_cookie))
	++ex_nesting_level;

    // Get the function or script name and the address where the next breakpoint
    // line and the debug tick for a function or script are stored.
    නම් (getline_is_func)
    {
	fname = func_name(real_cookie);
	breakpoint = func_breakpoint(real_cookie);
	dbg_tick = func_dbg_tick(real_cookie);
    }
    නැතහොත් නම් (getline_equal(fgetline, cookie, getsourceline))
    {
	fname = SOURCING_NAME;
	breakpoint = source_breakpoint(real_cookie);
	dbg_tick = source_dbg_tick(real_cookie);
    }

    /*
     * Initialize "force_abort"  and "suppress_errthrow" at the top level.
     */
    නම් (!recursive)
    {
	force_abort = FALSE;
	suppress_errthrow = FALSE;
    }

    /*
     * If requested, store and reset the global values controlling the
     * exception handling (used when debugging).  Otherwise clear it to avoid
     * a bogus compiler warning when the optimizer uses inline functions...
     */
    නම් (flags & DOCMD_EXCRESET)
	save_dbg_stuff(&debug_saved);
    නැතහොත්
	CLEAR_FIELD(debug_saved);

    initial_trylevel = trylevel;

    /*
     * "did_throw" will be set to TRUE when an exception is being thrown.
     */
    did_throw = FALSE;
#endif
    /*
     * "did_emsg" will be set to TRUE when emsg() is used, in which case we
     * cancel the whole command line, and any if/endif or loop.
     * If force_abort is set, we cancel everything.
     */
#ifdef FEAT_EVAL
    did_emsg_cumul += did_emsg;
#endif
    did_emsg = FALSE;

    /*
     * KeyTyped is only set when calling vgetc().  Reset it here when not
     * calling vgetc() (sourced command lines).
     */
    නම් (!(flags & DOCMD_KEYTYPED)
			       && !getline_equal(fgetline, cookie, getexline))
	KeyTyped = FALSE;

    /*
     * Continue executing command lines:
     * - when inside an ":if", ":while" or ":for"
     * - for multiple commands on one line, separated with '|'
     * - when repeating until there are no more lines (for ":source")
     */
    next_cmdline = cmdline;
    do
    {
#ifdef FEAT_EVAL
	getline_is_func = getline_equal(fgetline, cookie, get_func_line);
#endif

	// stop skipping cmds for an error msg after all endif/while/for
	නම් (next_cmdline == NULL
#ifdef FEAT_EVAL
		&& !force_abort
		&& cstack.cs_idx < 0
		&& !(getline_is_func && func_has_abort(real_cookie))
#endif
							)
	{
#ifdef FEAT_EVAL
	    did_emsg_cumul += did_emsg;
#endif
	    did_emsg = FALSE;
	}

	/*
	 * 1. If repeating a line in a loop, get a line from lines_ga.
	 * 2. If no line given: Get an allocated line with fgetline().
	 * 3. If a line is given: Make a copy, so we can mess with it.
	 */

#ifdef FEAT_EVAL
	// 1. If repeating, get a previous line from lines_ga.
	නම් (cstack.cs_looplevel > 0 && current_line < lines_ga.ga_len)
	{
	    // Each '|' separated command is stored separately in lines_ga, to
	    // be able to jump to it.  Don't use next_cmdline now.
	    VIM_CLEAR(cmdline_copy);

	    // Check if a function has returned or, unless it has an unclosed
	    // try conditional, aborted.
	    නම් (getline_is_func)
	    {
# ifdef FEAT_PROFILE
		නම් (do_profiling == PROF_YES)
		    func_line_end(real_cookie);
# endif
		නම් (func_has_ended(real_cookie))
		{
		    retval = FAIL;
		    bha_nga;
		}
	    }
# ifdef FEAT_PROFILE
	    නැතහොත් නම් (do_profiling == PROF_YES
			    && getline_equal(fgetline, cookie, getsourceline))
		script_line_end();
# endif

	    // Check if a sourced file hit a ":finish" command.
	    නම් (source_finished(fgetline, cookie))
	    {
		retval = FAIL;
		bha_nga;
	    }

	    // If breakpoints have been added/deleted need to check for it.
	    නම් (breakpoint != NULL && dbg_tick != NULL
						   && *dbg_tick != debug_tick)
	    {
		*breakpoint = dbg_find_breakpoint(
				getline_equal(fgetline, cookie, getsourceline),
							fname, SOURCING_LNUM);
		*dbg_tick = debug_tick;
	    }

	    next_cmdline = ((wcmd_T *)(lines_ga.ga_data))[current_line].line;
	    SOURCING_LNUM = ((wcmd_T *)(lines_ga.ga_data))[current_line].lnum;

	    // Did we encounter a breakpoint?
	    නම් (breakpoint != NULL && *breakpoint != 0
					      && *breakpoint <= SOURCING_LNUM)
	    {
		dbg_breakpoint(fname, SOURCING_LNUM);
		// Find next breakpoint.
		*breakpoint = dbg_find_breakpoint(
			       getline_equal(fgetline, cookie, getsourceline),
							fname, SOURCING_LNUM);
		*dbg_tick = debug_tick;
	    }
# ifdef FEAT_PROFILE
	    නම් (do_profiling == PROF_YES)
	    {
		නම් (getline_is_func)
		    func_line_start(real_cookie, SOURCING_LNUM);
		නැතහොත් නම් (getline_equal(fgetline, cookie, getsourceline))
		    script_line_start();
	    }
# endif
	}
#endif

	// 2. If no line given, get an allocated line with fgetline().
	නම් (next_cmdline == NULL)
	{
	    /*
	     * Need to set msg_didout for the first line after an ":if",
	     * otherwise the ":if" will be overwritten.
	     */
	    නම් (count == 1 && getline_equal(fgetline, cookie, getexline))
		msg_didout = TRUE;
	    නම් (fgetline == NULL || (next_cmdline = fgetline(':', cookie,
#ifdef FEAT_EVAL
		    cstack.cs_idx < 0 ? 0 : (cstack.cs_idx + 1) * 2
#නැතහොත්
		    0
#endif
		    , in_vim9script() ? GETLINE_CONCAT_CONTBAR
					       : GETLINE_CONCAT_CONT)) == NULL)
	    {
		// Don't call wait_return() for aborted command line.  The NULL
		// returned for the end of a sourced file or executed function
		// doesn't do this.
		නම් (KeyTyped && !(flags & DOCMD_REPEAT))
		    need_wait_return = FALSE;
		retval = FAIL;
		bha_nga;
	    }
	    used_getline = TRUE;

	    /*
	     * Keep the first typed line.  Clear it when more lines are typed.
	     */
	    නම් (flags & DOCMD_KEEPLINE)
	    {
		vim_free(repeat_cmdline);
		නම් (count == 0)
		    repeat_cmdline = vim_strsave(next_cmdline);
		නැතහොත්
		    repeat_cmdline = NULL;
	    }
	}

	// 3. Make a copy of the command so we can mess with it.
	නැතහොත් නම් (cmdline_copy == NULL)
	{
	    next_cmdline = vim_strsave(next_cmdline);
	    නම් (next_cmdline == NULL)
	    {
		emsg(_(e_out_of_memory));
		retval = FAIL;
		bha_nga;
	    }
	}
	cmdline_copy = next_cmdline;

#ifdef FEAT_EVAL
	/*
	 * Inside a while/for loop, and when the command looks like a ":while"
	 * or ":for", the line is stored, because we may need it later when
	 * looping.
	 *
	 * When there is a '|' and another command, it is stored separately,
	 * because we need to be able to jump back to it from an
	 * :endwhile/:endfor.
	 *
	 * Pass a different "fgetline" function to do_one_cmd() below,
	 * that it stores lines in or reads them from "lines_ga".  Makes it
	 * possible to define a function inside a while/for loop and handles
	 * line continuation.
	 */
	නම් ((cstack.cs_looplevel > 0 || has_loop_cmd(next_cmdline)))
	{
	    cmd_getline = get_loop_line;
	    cmd_cookie = (void *)&cmd_loop_cookie;
	    cmd_loop_cookie.lines_gap = &lines_ga;
	    cmd_loop_cookie.current_line = current_line;
	    cmd_loop_cookie.lc_getline = fgetline;
	    cmd_loop_cookie.cookie = cookie;
	    cmd_loop_cookie.repeating = (current_line < lines_ga.ga_len);

	    // Save the current line when encountering it the first time.
	    නම් (current_line == lines_ga.ga_len
		    && store_loop_line(&lines_ga, next_cmdline) == FAIL)
	    {
		retval = FAIL;
		bha_nga;
	    }
	    current_line_before = current_line;
	}
	නැතහොත්
	{
	    cmd_getline = fgetline;
	    cmd_cookie = cookie;
	}

	did_endif = FALSE;
#endif

	නම් (count++ == 0)
	{
	    /*
	     * All output from the commands is put below each other, without
	     * waiting for a return. Don't do this when executing commands
	     * from a script or when being called recursive (e.g. for ":e
	     * +command file").
	     */
	    නම් (!(flags & DOCMD_NOWAIT) && !recursive)
	    {
		msg_didout_before_start = msg_didout;
		msg_didany = FALSE; // no output yet
		msg_start();
		msg_scroll = TRUE;  // put messages below each other
		++no_wait_return;   // don't wait for return until finished
		++RedrawingDisabled;
		did_inc_RedrawingDisabled = TRUE;
	    }
	}

	නම් ((p_verbose >= 15 && SOURCING_NAME != NULL) || p_verbose >= 16)
	    msg_verbose_cmd(SOURCING_LNUM, cmdline_copy);

	/*
	 * 2. Execute one '|' separated command.
	 *    do_one_cmd() will return NULL if there is no trailing '|'.
	 *    "cmdline_copy" can change, e.g. for '%' and '#' expansion.
	 */
	++recursive;
	next_cmdline = do_one_cmd(&cmdline_copy, flags,
#ifdef FEAT_EVAL
				&cstack,
#endif
				cmd_getline, cmd_cookie);
	--recursive;

#ifdef FEAT_EVAL
	නම් (cmd_cookie == (void *)&cmd_loop_cookie)
	    // Use "current_line" from "cmd_loop_cookie", it may have been
	    // incremented when defining a function.
	    current_line = cmd_loop_cookie.current_line;
#endif

	නම් (next_cmdline == NULL)
	{
	    VIM_CLEAR(cmdline_copy);

	    /*
	     * If the command was typed, remember it for the ':' register.
	     * Do this AFTER executing the command to make :@: work.
	     */
	    නම් (getline_equal(fgetline, cookie, getexline)
						  && new_last_cmdline != NULL)
	    {
		vim_free(last_cmdline);
		last_cmdline = new_last_cmdline;
		new_last_cmdline = NULL;
	    }
	}
	නැතහොත්
	{
	    // need to copy the command after the '|' to cmdline_copy, for the
	    // next do_one_cmd()
	    STRMOVE(cmdline_copy, next_cmdline);
	    next_cmdline = cmdline_copy;
	}


#ifdef FEAT_EVAL
	// reset did_emsg for a function that is not aborted by an error
	නම් (did_emsg && !force_abort
		&& getline_equal(fgetline, cookie, get_func_line)
					      && !func_has_abort(real_cookie))
	{
	    // did_emsg_cumul is not set here
	    did_emsg = FALSE;
	}

	නම් (cstack.cs_looplevel > 0)
	{
	    ++current_line;

	    /*
	     * An ":endwhile", ":endfor" and ":continue" is handled here.
	     * If we were executing commands, jump back to the ":while" or
	     * ":for".
	     * If we were not executing commands, decrement cs_looplevel.
	     */
	    නම් (cstack.cs_lflags & (CSL_HAD_CONT | CSL_HAD_ENDLOOP))
	    {
		cstack.cs_lflags &= ~(CSL_HAD_CONT | CSL_HAD_ENDLOOP);

		// Jump back to the matching ":while" or ":for".  Be careful
		// not to use a cs_line[] from an entry that isn't a ":while"
		// or ":for": It would make "current_line" invalid and can
		// cause a crash.
		නම් (!did_emsg && !got_int && !did_throw
			&& cstack.cs_idx >= 0
			&& (cstack.cs_flags[cstack.cs_idx]
						      & (CSF_WHILE | CSF_FOR))
			&& cstack.cs_line[cstack.cs_idx] >= 0
			&& (cstack.cs_flags[cstack.cs_idx] & CSF_ACTIVE))
		{
		    current_line = cstack.cs_line[cstack.cs_idx];
						// remember we jumped there
		    cstack.cs_lflags |= CSL_HAD_LOOP;
		    line_breakcheck();		// check if CTRL-C typed

		    // Check for the next breakpoint at or after the ":while"
		    // or ":for".
		    නම් (breakpoint != NULL && lines_ga.ga_len > current_line)
		    {
			*breakpoint = dbg_find_breakpoint(
			       getline_equal(fgetline, cookie, getsourceline),
									fname,
			   ((wcmd_T *)lines_ga.ga_data)[current_line].lnum-1);
			*dbg_tick = debug_tick;
		    }
		}
		නැතහොත්
		{
		    // can only get here with ":endwhile" or ":endfor"
		    නම් (cstack.cs_idx >= 0)
			rewind_conditionals(&cstack, cstack.cs_idx - 1,
				   CSF_WHILE | CSF_FOR, &cstack.cs_looplevel);
		}
	    }

	    /*
	     * For a ":while" or ":for" we need to remember the line number.
	     */
	    නැතහොත් නම් (cstack.cs_lflags & CSL_HAD_LOOP)
	    {
		cstack.cs_lflags &= ~CSL_HAD_LOOP;
		cstack.cs_line[cstack.cs_idx] = current_line_before;
	    }
	}

	// Check for the next breakpoint after a watchexpression
	නම් (breakpoint != NULL && has_watchexpr())
	{
	    *breakpoint = dbg_find_breakpoint(FALSE, fname, SOURCING_LNUM);
	    *dbg_tick = debug_tick;
	}

	/*
	 * When not inside any ":while" loop, clear remembered lines.
	 */
	නම් (cstack.cs_looplevel == 0)
	{
	    නම් (lines_ga.ga_len > 0)
	    {
		SOURCING_LNUM =
		       ((wcmd_T *)lines_ga.ga_data)[lines_ga.ga_len - 1].lnum;
		free_cmdlines(&lines_ga);
	    }
	    current_line = 0;
	}

	/*
	 * A ":finally" makes did_emsg, got_int, and did_throw pending for
	 * being restored at the ":endtry".  Reset them here and set the
	 * ACTIVE and FINALLY flags, so that the finally clause gets executed.
	 * This includes the case where a missing ":endif", ":endwhile" or
	 * ":endfor" was detected by the ":finally" itself.
	 */
	නම් (cstack.cs_lflags & CSL_HAD_FINA)
	{
	    cstack.cs_lflags &= ~CSL_HAD_FINA;
	    report_make_pending(cstack.cs_pending[cstack.cs_idx]
		    & (CSTP_ERROR | CSTP_INTERRUPT | CSTP_THROW),
		    did_throw ? (void *)current_exception : NULL);
	    did_emsg = got_int = did_throw = FALSE;
	    cstack.cs_flags[cstack.cs_idx] |= CSF_ACTIVE | CSF_FINALLY;
	}

	// Update global "trylevel" for recursive calls to do_cmdline() from
	// within this loop.
	trylevel = initial_trylevel + cstack.cs_trylevel;

	/*
	 * If the outermost try conditional (across function calls and sourced
	 * files) is aborted because of an error, an interrupt, or an uncaught
	 * exception, cancel everything.  If it is left normally, reset
	 * force_abort to get the non-EH compatible abortion behavior for
	 * the rest of the script.
	 */
	නම් (trylevel == 0 && !did_emsg && !got_int && !did_throw)
	    force_abort = FALSE;

	// Convert an interrupt to an exception if appropriate.
	(void)do_intthrow(&cstack);
#endif // FEAT_EVAL

    }
    /*
     * Continue executing command lines when:
     * - no CTRL-C typed, no aborting error, no exception thrown or try
     *   conditionals need to be checked for executing finally clauses or
     *   catching an interrupt exception
     * - didn't get an error message or lines are not typed
     * - there is a command after '|', inside a :if, :while, :for or :try, or
     *   looping for ":source" command or function call.
     */
    අතරතුර (!((got_int
#ifdef FEAT_EVAL
		    || (did_emsg && (force_abort || in_vim9script()))
		    || did_throw
#endif
	     )
#ifdef FEAT_EVAL
		&& cstack.cs_trylevel == 0
#endif
	    )
	    && !(did_emsg
#ifdef FEAT_EVAL
		// Keep going when inside try/catch, so that the error can be
		// deal with, except when it is a syntax error, it may cause
		// the :endtry to be missed.
		&& (cstack.cs_trylevel == 0 || did_emsg_syntax)
#endif
		&& used_getline
			    && (getline_equal(fgetline, cookie, getexmodeline)
			       || getline_equal(fgetline, cookie, getexline)))
	    && (next_cmdline != NULL
#ifdef FEAT_EVAL
			|| cstack.cs_idx >= 0
#endif
			|| (flags & DOCMD_REPEAT)));

    vim_free(cmdline_copy);
    did_emsg_syntax = FALSE;
#ifdef FEAT_EVAL
    free_cmdlines(&lines_ga);
    ga_clear(&lines_ga);

    නම් (cstack.cs_idx >= 0)
    {
	/*
	 * If a sourced file or executed function ran to its end, report the
	 * unclosed conditional.
	 * In Vim9 script do not give a second error, executing aborts after
	 * the first one.
	 */
	නම් (!got_int && !did_throw && !aborting()
		&& !(did_emsg && in_vim9script())
		&& ((getline_equal(fgetline, cookie, getsourceline)
			&& !source_finished(fgetline, cookie))
		    || (getline_equal(fgetline, cookie, get_func_line)
					    && !func_has_ended(real_cookie))))
	{
	    නම් (cstack.cs_flags[cstack.cs_idx] & CSF_TRY)
		emsg(_(e_missing_endtry));
	    නැතහොත් නම් (cstack.cs_flags[cstack.cs_idx] & CSF_WHILE)
		emsg(_(e_missing_endwhile));
	    නැතහොත් නම් (cstack.cs_flags[cstack.cs_idx] & CSF_FOR)
		emsg(_(e_missing_endfor));
	    නැතහොත්
		emsg(_(e_missing_endif));
	}

	/*
	 * Reset "trylevel" in case of a ":finish" or ":return" or a missing
	 * ":endtry" in a sourced file or executed function.  If the try
	 * conditional is in its finally clause, ignore anything pending.
	 * If it is in a catch clause, finish the caught exception.
	 * Also cleanup any "cs_forinfo" structures.
	 */
	do
	{
	    පූර්ණ_සංඛ්‍යා idx = cleanup_conditionals(&cstack, 0, TRUE);

	    නම් (idx >= 0)
		--idx;	    // remove try block not in its finally clause
	    rewind_conditionals(&cstack, idx, CSF_WHILE | CSF_FOR,
							&cstack.cs_looplevel);
	}
	අතරතුර (cstack.cs_idx >= 0);
	trylevel = initial_trylevel;
    }

    // If a missing ":endtry", ":endwhile", ":endfor", or ":endif" or a memory
    // lack was reported above and the error message is to be converted to an
    // exception, do this now after rewinding the cstack.
    do_errthrow(&cstack, getline_equal(fgetline, cookie, get_func_line)
				  ? (char_u *)"endfunction" : (char_u *)NULL);

    නම් (trylevel == 0)
    {
	// Just in case did_throw got set but current_exception wasn't.
	නම් (current_exception == NULL)
	    did_throw = FALSE;

	/*
	 * When an exception is being thrown out of the outermost try
	 * conditional, discard the uncaught exception, disable the conversion
	 * of interrupts or errors to exceptions, and ensure that no more
	 * commands are executed.
	 */
	නම් (did_throw)
	    handle_did_throw();

	/*
	 * On an interrupt or an aborting error not converted to an exception,
	 * disable the conversion of errors to exceptions.  (Interrupts are not
	 * converted anymore, here.) This enables also the interrupt message
	 * when force_abort is set and did_emsg unset in case of an interrupt
	 * from a finally clause after an error.
	 */
	නැතහොත් නම් (got_int || (did_emsg && force_abort))
	    suppress_errthrow = TRUE;
    }

    /*
     * The current cstack will be freed when do_cmdline() returns.  An uncaught
     * exception will have to be rethrown in the previous cstack.  If a function
     * has just returned or a script file was just finished and the previous
     * cstack belongs to the same function or, respectively, script file, it
     * will have to be checked for finally clauses to be executed due to the
     * ":return" or ":finish".  This is done in do_one_cmd().
     */
    නම් (did_throw)
	need_rethrow = TRUE;
    නම් ((getline_equal(fgetline, cookie, getsourceline)
		&& ex_nesting_level > source_level(real_cookie))
	    || (getline_equal(fgetline, cookie, get_func_line)
		&& ex_nesting_level > func_level(real_cookie) + 1))
    {
	නම් (!did_throw)
	    check_cstack = TRUE;
    }
    නැතහොත්
    {
	// When leaving a function, reduce nesting level.
	නම් (getline_equal(fgetline, cookie, get_func_line))
	    --ex_nesting_level;
	/*
	 * Go to debug mode when returning from a function in which we are
	 * single-stepping.
	 */
	නම් ((getline_equal(fgetline, cookie, getsourceline)
		    || getline_equal(fgetline, cookie, get_func_line))
		&& ex_nesting_level + 1 <= debug_break_level)
	    do_debug(getline_equal(fgetline, cookie, getsourceline)
		    ? (char_u *)_("End of sourced file")
		    : (char_u *)_("End of function"));
    }

    /*
     * Restore the exception environment (done after returning from the
     * debugger).
     */
    නම් (flags & DOCMD_EXCRESET)
	restore_dbg_stuff(&debug_saved);

    msg_list = saved_msg_list;

    // Cleanup if "cs_emsg_silent_list" remains.
    නම් (cstack.cs_emsg_silent_list != NULL)
    {
	eslist_T *elem, *temp;

	සඳහා (elem = cstack.cs_emsg_silent_list; elem != NULL; elem = temp)
	{
	    temp = elem->next;
	    vim_free(elem);
	}
    }
#endif // FEAT_EVAL

    /*
     * If there was too much output to fit on the command line, ask the user to
     * hit return before redrawing the screen. With the ":global" command we do
     * this only once after the command is finished.
     */
    නම් (did_inc_RedrawingDisabled)
    {
	නම් (RedrawingDisabled > 0)
	    --RedrawingDisabled;
	--no_wait_return;
	msg_scroll = FALSE;

	/*
	 * When just finished an ":if"-":else" which was typed, no need to
	 * wait for hit-return.  Also for an error situation.
	 */
	නම් (retval == FAIL
#ifdef FEAT_EVAL
		|| (did_endif && KeyTyped && !did_emsg)
#endif
					    )
	{
	    need_wait_return = FALSE;
	    msg_didany = FALSE;		// don't wait when restarting edit
	}
	නැතහොත් නම් (need_wait_return)
	{
	    /*
	     * The msg_start() above clears msg_didout. The wait_return() we do
	     * here should not overwrite the command that may be shown before
	     * doing that.
	     */
	    msg_didout |= msg_didout_before_start;
	    wait_return(FALSE);
	}
    }

#ifdef FEAT_EVAL
    did_endif = FALSE;  // in case do_cmdline used recursively
#නැතහොත්
    /*
     * Reset if_level, in case a sourced script file contains more ":if" than
     * ":endif" (could be ":if x | foo | endif").
     */
    if_level = 0;
#endif

    --call_depth;
    ආපසු retval;
}

#නම් defined(FEAT_EVAL)
/*
 * Handle when "did_throw" is set after executing commands.
 */
    void
handle_did_throw(void)
{
    var_na	*p = NULL;
    msglist_T	*messages = NULL;
    ESTACK_CHECK_DECLARATION;

    /*
     * If the uncaught exception is a user exception, report it as an
     * error.  If it is an error exception, display the saved error
     * message now.  For an interrupt exception, do nothing; the
     * interrupt message is given elsewhere.
     */
    switch (current_exception->type)
    {
	pak_sa ET_USER:
	    vim_snprintf((var_na *)IObuff, IOSIZE,
		    _(e_exception_not_caught_str),
		    current_exception->value);
	    p = (var_na *)vim_strsave(IObuff);
	    bha_nga;
	pak_sa ET_ERROR:
	    messages = current_exception->messages;
	    current_exception->messages = NULL;
	    bha_nga;
	pak_sa ET_INTERRUPT:
	    bha_nga;
    }

    estack_push(ETYPE_EXCEPT, current_exception->throw_name,
					current_exception->throw_lnum);
    ESTACK_CHECK_SETUP;
    current_exception->throw_name = NULL;

    discard_current_exception();	// uses IObuff if 'verbose'

    // If "silent!" is active the uncaught exception is not fatal.
    නම් (emsg_silent == 0)
    {
	suppress_errthrow = TRUE;
	force_abort = TRUE;
    }

    නම් (messages != NULL)
    {
	do
	{
	    msglist_T	*next = messages->next;
	    පූර්ණ_සංඛ්‍යා		save_compiling = estack_compiling;

	    estack_compiling = messages->msg_compiling;
	    emsg(messages->msg);
	    vim_free(messages->msg);
	    vim_free(messages->sfile);
	    vim_free(messages);
	    messages = next;
	    estack_compiling = save_compiling;
	}
	අතරතුර (messages != NULL);
    }
    නැතහොත් නම් (p != NULL)
    {
	emsg(p);
	vim_free(p);
    }
    vim_free(SOURCING_NAME);
    ESTACK_CHECK_NOW;
    estack_pop();
}

/*
 * Obtain a line when inside a ":while" or ":for" loop.
 */
    static char_u *
get_loop_line(පූර්ණ_සංඛ්‍යා c, void *cookie, පූර්ණ_සංඛ්‍යා indent, getline_opt_T options)
{
    sa_nracanaa loop_cookie	*cp = (sa_nracanaa loop_cookie *)cookie;
    wcmd_T		*wp;
    char_u		*line;

    නම් (cp->current_line + 1 >= cp->lines_gap->ga_len)
    {
	නම් (cp->repeating)
	    ආපසු NULL;	// trying to read past ":endwhile"/":endfor"

	// First time inside the ":while"/":for": get line normally.
	නම් (cp->lc_getline == NULL)
	    line = getcmdline(c, 0L, indent, 0);
	නැතහොත්
	    line = cp->lc_getline(c, cp->cookie, indent, options);
	නම් (line != NULL && store_loop_line(cp->lines_gap, line) == OK)
	    ++cp->current_line;

	ආපසු line;
    }

    KeyTyped = FALSE;
    ++cp->current_line;
    wp = (wcmd_T *)(cp->lines_gap->ga_data) + cp->current_line;
    SOURCING_LNUM = wp->lnum;
    ආපසු vim_strsave(wp->line);
}

/*
 * Store a line in "gap" so that a ":while" loop can execute it again.
 */
    static පූර්ණ_සංඛ්‍යා
store_loop_line(garray_T *gap, char_u *line)
{
    නම් (ga_grow(gap, 1) == FAIL)
	ආපසු FAIL;
    ((wcmd_T *)(gap->ga_data))[gap->ga_len].line = vim_strsave(line);
    ((wcmd_T *)(gap->ga_data))[gap->ga_len].lnum = SOURCING_LNUM;
    ++gap->ga_len;
    ආපසු OK;
}

/*
 * Free the lines stored for a ":while" or ":for" loop.
 */
    static void
free_cmdlines(garray_T *gap)
{
    අතරතුර (gap->ga_len > 0)
    {
	vim_free(((wcmd_T *)(gap->ga_data))[gap->ga_len - 1].line);
	--gap->ga_len;
    }
}
#endif

/*
 * If "fgetline" is get_loop_line(), return TRUE if the getline it uses equals
 * "func".  * Otherwise return TRUE when "fgetline" equals "func".
 */
    පූර්ණ_සංඛ්‍යා
getline_equal(
    char_u	*(*fgetline)(පූර්ණ_සංඛ්‍යා, void *, පූර්ණ_සංඛ්‍යා, getline_opt_T),
    void	*cookie UNUSED,		// argument for fgetline()
    char_u	*(*func)(පූර්ණ_සංඛ්‍යා, void *, පූර්ණ_සංඛ්‍යා, getline_opt_T))
{
#ifdef FEAT_EVAL
    char_u		*(*gp)(පූර්ණ_සංඛ්‍යා, void *, පූර්ණ_සංඛ්‍යා, getline_opt_T);
    sa_nracanaa loop_cookie *cp;

    // When "fgetline" is "get_loop_line()" use the "cookie" to find the
    // function that's originally used to obtain the lines.  This may be
    // nested several levels.
    gp = fgetline;
    cp = (sa_nracanaa loop_cookie *)cookie;
    අතරතුර (gp == get_loop_line)
    {
	gp = cp->lc_getline;
	cp = cp->cookie;
    }
    ආපසු gp == func;
#නැතහොත්
    ආපසු fgetline == func;
#endif
}

/*
 * If "fgetline" is get_loop_line(), return the cookie used by the original
 * getline function.  Otherwise return "cookie".
 */
    void *
getline_cookie(
    char_u	*(*fgetline)(පූර්ණ_සංඛ්‍යා, void *, පූර්ණ_සංඛ්‍යා, getline_opt_T) UNUSED,
    void	*cookie)		// argument for fgetline()
{
#ifdef FEAT_EVAL
    char_u		*(*gp)(පූර්ණ_සංඛ්‍යා, void *, පූර්ණ_සංඛ්‍යා, getline_opt_T);
    sa_nracanaa loop_cookie  *cp;

    // When "fgetline" is "get_loop_line()" use the "cookie" to find the
    // cookie that's originally used to obtain the lines.  This may be nested
    // several levels.
    gp = fgetline;
    cp = (sa_nracanaa loop_cookie *)cookie;
    අතරතුර (gp == get_loop_line)
    {
	gp = cp->lc_getline;
	cp = cp->cookie;
    }
    ආපසු cp;
#නැතහොත්
    ආපසු cookie;
#endif
}

#නම් defined(FEAT_EVAL)
/*
 * Get the next line source line without advancing.
 */
    char_u *
getline_peek(
    char_u	*(*fgetline)(පූර්ණ_සංඛ්‍යා, void *, පූර්ණ_සංඛ්‍යා, getline_opt_T) UNUSED,
    void	*cookie)		// argument for fgetline()
{
    char_u		*(*gp)(පූර්ණ_සංඛ්‍යා, void *, පූර්ණ_සංඛ්‍යා, getline_opt_T);
    sa_nracanaa loop_cookie  *cp;
    wcmd_T		*wp;

    // When "fgetline" is "get_loop_line()" use the "cookie" to find the
    // cookie that's originally used to obtain the lines.  This may be nested
    // several levels.
    gp = fgetline;
    cp = (sa_nracanaa loop_cookie *)cookie;
    අතරතුර (gp == get_loop_line)
    {
	නම් (cp->current_line + 1 < cp->lines_gap->ga_len)
	{
	    // executing lines a second time, use the stored copy
	    wp = (wcmd_T *)(cp->lines_gap->ga_data) + cp->current_line + 1;
	    ආපසු wp->line;
	}
	gp = cp->lc_getline;
	cp = cp->cookie;
    }
    නම් (gp == getsourceline)
	ආපසු source_nextline(cp);
    ආපසු NULL;
}
#endif


/*
 * Helper function to apply an offset for buffer commands, i.e. ":bdelete",
 * ":bwipeout", etc.
 * Returns the buffer number.
 */
    static පූර්ණ_සංඛ්‍යා
compute_buffer_local_count(පූර්ණ_සංඛ්‍යා addr_type, පූර්ණ_සංඛ්‍යා lnum, පූර්ණ_සංඛ්‍යා offset)
{
    buf_T   *buf;
    buf_T   *nextbuf;
    පූර්ණ_සංඛ්‍යා     count = offset;

    buf = firstbuf;
    අතරතුර (buf->b_next != NULL && buf->b_fnum < lnum)
	buf = buf->b_next;
    අතරතුර (count != 0)
    {
	count += (offset < 0) ? 1 : -1;
	nextbuf = (offset < 0) ? buf->b_prev : buf->b_next;
	නම් (nextbuf == NULL)
	    bha_nga;
	buf = nextbuf;
	නම් (addr_type == ADDR_LOADED_BUFFERS)
	    // skip over unloaded buffers
	    අතරතුර (buf->b_ml.ml_mfp == NULL)
	    {
		nextbuf = (offset < 0) ? buf->b_prev : buf->b_next;
		නම් (nextbuf == NULL)
		    bha_nga;
		buf = nextbuf;
	    }
    }
    // we might have gone too far, last buffer is not loadedd
    නම් (addr_type == ADDR_LOADED_BUFFERS)
	අතරතුර (buf->b_ml.ml_mfp == NULL)
	{
	    nextbuf = (offset >= 0) ? buf->b_prev : buf->b_next;
	    නම් (nextbuf == NULL)
		bha_nga;
	    buf = nextbuf;
	}
    ආපසු buf->b_fnum;
}

/*
 * Return the window number of "win".
 * When "win" is NULL return the number of windows.
 */
    static පූර්ණ_සංඛ්‍යා
current_win_nr(win_T *win)
{
    win_T	*wp;
    පූර්ණ_සංඛ්‍යා		nr = 0;

    FOR_ALL_WINDOWS(wp)
    {
	++nr;
	නම් (wp == win)
	    bha_nga;
    }
    ආපසු nr;
}

    static පූර්ණ_සංඛ්‍යා
current_tab_nr(tabpage_T *tab)
{
    tabpage_T	*tp;
    පූර්ණ_සංඛ්‍යා		nr = 0;

    FOR_ALL_TABPAGES(tp)
    {
	++nr;
	නම් (tp == tab)
	    bha_nga;
    }
    ආපසු nr;
}

    static පූර්ණ_සංඛ්‍යා
comment_start(char_u *p, පූර්ණ_සංඛ්‍යා starts_with_colon UNUSED)
{
    නම් (in_vim9script())
	ආපසු p[0] == '#' && !starts_with_colon;
    ආපසු *p == '"';
}

#define CURRENT_WIN_NR current_win_nr(curwin)
#define LAST_WIN_NR current_win_nr(NULL)
#define CURRENT_TAB_NR current_tab_nr(curtab)
#define LAST_TAB_NR current_tab_nr(NULL)

/*
 * Execute one Ex command.
 *
 * If "flags" has DOCMD_VERBOSE, the command will be included in the error
 * message.
 *
 * 1. skip comment lines and leading space
 * 2. handle command modifiers
 * 3. find the command
 * 4. parse range
 * 5. Parse the command.
 * 6. parse arguments
 * 7. switch on command name
 *
 * Note: "fgetline" can be NULL.
 *
 * This function may be called recursively!
 */
    static char_u *
do_one_cmd(
    char_u	**cmdlinep,
    පූර්ණ_සංඛ්‍යා		flags,
#ifdef FEAT_EVAL
    cstack_T	*cstack,
#endif
    char_u	*(*fgetline)(පූර්ණ_සංඛ්‍යා, void *, පූර්ණ_සංඛ්‍යා, getline_opt_T),
    void	*cookie)		// argument for fgetline()
{
    char_u	*p;
    linenr_T	lnum;
    long	n;
    var_na	*errormsg = NULL;	// error message
    char_u	*after_modifier = NULL;
    exarg_T	ea;			// Ex command arguments
    cmdmod_T	save_cmdmod;
    පූර්ණ_සංඛ්‍යා		save_reg_executing = reg_executing;
    පූර්ණ_සංඛ්‍යා		save_pending_end_reg_executing = pending_end_reg_executing;
    පූර්ණ_සංඛ්‍යා		ni;			// set when Not Implemented
    char_u	*cmd;
    පූර්ණ_සංඛ්‍යා		starts_with_colon = FALSE;
    පූර්ණ_සංඛ්‍යා		may_have_range;
#ifdef FEAT_EVAL
    පූර්ණ_සංඛ්‍යා		did_set_expr_line = FALSE;
#endif
    පූර්ණ_සංඛ්‍යා		sourcing = flags & DOCMD_VERBOSE;
    පූර්ණ_සංඛ්‍යා		did_append_cmd = FALSE;

    CLEAR_FIELD(ea);
    ea.line1 = 1;
    ea.line2 = 1;
#ifdef FEAT_EVAL
    ++ex_nesting_level;
#endif

    // When the last file has not been edited :q has to be typed twice.
    නම් (quitmore
#ifdef FEAT_EVAL
	    // avoid that a function call in 'statusline' does this
	    && !getline_equal(fgetline, cookie, get_func_line)
#endif
	    // avoid that an autocommand, e.g. QuitPre, does this
	    && !getline_equal(fgetline, cookie, getnextac))
	--quitmore;

    /*
     * Reset browse, confirm, etc..  They are restored when returning, for
     * recursive calls.
     */
    save_cmdmod = cmdmod;

    // "#!anything" is handled like a comment.
    නම් ((*cmdlinep)[0] == '#' && (*cmdlinep)[1] == '!')
	goto doend;

/*
 * 1. Skip comment lines and leading white space and colons.
 * 2. Handle command modifiers.
 */
    // The "ea" structure holds the arguments that can be used.
    ea.cmd = *cmdlinep;
    ea.cmdlinep = cmdlinep;
    ea.ea_getline = fgetline;
    ea.cookie = cookie;
#ifdef FEAT_EVAL
    ea.cstack = cstack;
    starts_with_colon = *skipwhite(ea.cmd) == ':';
#endif
    නම් (parse_command_modifiers(&ea, &errormsg, &cmdmod, FALSE) == FAIL)
	goto doend;
    apply_cmdmod(&cmdmod);
    after_modifier = ea.cmd;

#ifdef FEAT_EVAL
    ea.skip = did_emsg || got_int || did_throw || (cstack->cs_idx >= 0
			 && !(cstack->cs_flags[cstack->cs_idx] & CSF_ACTIVE));
#නැතහොත්
    ea.skip = (if_level > 0);
#endif

/*
 * 3. Skip over the range to find the command.  Let "p" point to after it.
 *
 * We need the command to know what kind of range it uses.
 */
    cmd = ea.cmd;

    // In Vim9 script a colon is required before the range.  This may also be
    // after command modifiers.
    පූර්ණ_සංඛ්‍යා vim9script = in_vim9script();
    නම් (vim9script && (flags & DOCMD_RANGEOK) == 0)
    {
	may_have_range = FALSE;
	සඳහා (p = ea.cmd; p >= *cmdlinep; --p)
	{
	    නම් (*p == ':')
		may_have_range = TRUE;
	    නම් (p < ea.cmd && !VIM_ISWHITE(*p))
		bha_nga;
	}
    }
    නැතහොත්
	may_have_range = TRUE;
    නම් (may_have_range)
	ea.cmd = skip_range(ea.cmd, TRUE, NULL);

#ifdef FEAT_EVAL
    // Handle ":export" - it functions almost like a command modifier.
    // ":export var Name: type"
    // ":export def Name(..."
    // etc.
    නම් (vim9script && checkforcmd_noparen(&ea.cmd, "export", 6))
	is_export = TRUE;
#endif

    නම් (vim9script && !may_have_range)
    {
	නම් (ea.cmd == cmd + 1 && *cmd == '$')
	    // should be "$VAR = val"
	    --ea.cmd;
#ifdef FEAT_EVAL
	p = find_ex_command(&ea, NULL, lookup_scriptitem, NULL);
#නැතහොත්
	p = find_ex_command(&ea, NULL, NULL, NULL);
#endif
	නම් (ea.cmdidx == CMD_SIZE)
	{
	    char_u *ar = skip_range(ea.cmd, TRUE, NULL);

	    // If a ':' before the range is missing, give a clearer error
	    // message.
	    නම් (ar > ea.cmd && !ea.skip)
	    {
		semsg(_(e_colon_required_before_range_str), ea.cmd);
		goto doend;
	    }
	}
    }
    නැතහොත්
	p = find_ex_command(&ea, NULL, NULL, NULL);

#ifdef FEAT_EVAL
# ifdef FEAT_PROFILE
    // Count this line for profiling if skip is TRUE.
    නම් (do_profiling == PROF_YES
	    && (!ea.skip || cstack->cs_idx == 0 || (cstack->cs_idx > 0
		     && (cstack->cs_flags[cstack->cs_idx - 1] & CSF_ACTIVE))))
    {
	පූර්ණ_සංඛ්‍යා skip = did_emsg || got_int || did_throw;

	නම් (ea.cmdidx == CMD_catch)
	    skip = !skip && !(cstack->cs_idx >= 0
			  && (cstack->cs_flags[cstack->cs_idx] & CSF_THROWN)
			  && !(cstack->cs_flags[cstack->cs_idx] & CSF_CAUGHT));
	නැතහොත් නම් (ea.cmdidx == CMD_else || ea.cmdidx == CMD_elseif)
	    skip = skip || !(cstack->cs_idx >= 0
			  && !(cstack->cs_flags[cstack->cs_idx]
						  & (CSF_ACTIVE | CSF_TRUE)));
	නැතහොත් නම් (ea.cmdidx == CMD_finally)
	    skip = FALSE;
	නැතහොත් නම් (ea.cmdidx != CMD_endif
		&& ea.cmdidx != CMD_endfor
		&& ea.cmdidx != CMD_endtry
		&& ea.cmdidx != CMD_endwhile)
	    skip = ea.skip;

	නම් (!skip)
	{
	    නම් (getline_equal(fgetline, cookie, get_func_line))
		func_line_exec(getline_cookie(fgetline, cookie));
	    නැතහොත් නම් (getline_equal(fgetline, cookie, getsourceline))
		script_line_exec();
	}
    }
# endif
#endif

    ea.cmd = cmd;

#ifdef FEAT_EVAL
    // May go to debug mode.  If this happens and the ">quit" debug command is
    // used, throw an interrupt exception and skip the next command.
    dbg_check_breakpoint(&ea);
    නම් (!ea.skip && got_int)
    {
	ea.skip = TRUE;
	(void)do_intthrow(cstack);
    }
#endif

/*
 * 4. parse a range specifier of the form: addr [,addr] [;addr] ..
 *
 * where 'addr' is:
 *
 * %	      (entire file)
 * $  [+-NUM]
 * 'x [+-NUM] (where x denotes a currently defined mark)
 * .  [+-NUM]
 * [+-NUM]..
 * NUM
 *
 * The ea.cmd pointer is updated to point to the first character following the
 * range spec. If an initial address is found, but no second, the upper bound
 * is equal to the lower.
 */

    // ea.addr_type for user commands is set by find_ucmd
    නම් (!IS_USER_CMDIDX(ea.cmdidx))
    {
	නම් (ea.cmdidx != CMD_SIZE)
	    ea.addr_type = cmdnames[(පූර්ණ_සංඛ්‍යා)ea.cmdidx].cmd_addr_type;
	නැතහොත්
	    ea.addr_type = ADDR_LINES;

	// :wincmd range depends on the argument.
	නම් (ea.cmdidx == CMD_wincmd && p != NULL)
	    get_wincmd_addr_type(skipwhite(p), &ea);
#ifdef FEAT_QUICKFIX
	// :.cc in quickfix window uses line number
	නම් ((ea.cmdidx == CMD_cc || ea.cmdidx == CMD_ll) && bt_quickfix(curbuf))
	    ea.addr_type = ADDR_OTHER;
#endif
    }

    නම් (!may_have_range)
	ea.line1 = ea.line2 = default_address(&ea);
    නැතහොත් නම් (parse_cmd_address(&ea, &errormsg, FALSE) == FAIL)
	goto doend;

/*
 * 5. Parse the command.
 */

    /*
     * Skip ':' and any white space
     */
    ea.cmd = skipwhite(ea.cmd);
    අතරතුර (*ea.cmd == ':')
	ea.cmd = skipwhite(ea.cmd + 1);

    /*
     * If we got a line, but no command, then go to the line.
     * If we find a '|' or '\n' we set ea.nextcmd.
     */
    නම් (*ea.cmd == NUL || comment_start(ea.cmd, starts_with_colon)
			       || (ea.nextcmd = check_nextcmd(ea.cmd)) != NULL)
    {
	/*
	 * strange vi behaviour:
	 * ":3"		jumps to line 3
	 * ":3|..."	prints line 3  (not in Vim9 script)
	 * ":|"		prints current line  (not in Vim9 script)
	 */
	නම් (ea.skip)	    // skip this if inside :if
	    goto doend;
	errormsg = ex_range_without_command(&ea);
	goto doend;
    }

    // If this looks like an undefined user command and there are CmdUndefined
    // autocommands defined, trigger the matching autocommands.
    නම් (p != NULL && ea.cmdidx == CMD_SIZE && !ea.skip
	    && ASCII_ISUPPER(*ea.cmd)
	    && has_cmdundefined())
    {
	පූර්ණ_සංඛ්‍යා ret;

	p = ea.cmd;
	අතරතුර (ASCII_ISALNUM(*p))
	    ++p;
	p = vim_strnsave(ea.cmd, p - ea.cmd);
	ret = apply_autocmds(EVENT_CMDUNDEFINED, p, p, TRUE, NULL);
	vim_free(p);
	// If the autocommands did something and didn't cause an error, try
	// finding the command again.
	p = (ret
#ifdef FEAT_EVAL
		&& !aborting()
#endif
		) ? find_ex_command(&ea, NULL, NULL, NULL) : ea.cmd;
    }

    නම් (p == NULL)
    {
	නම් (!ea.skip)
	    errormsg = _(e_ambiguous_use_of_user_defined_command);
	goto doend;
    }
    // Check for wrong commands.
    නම් (*p == '!' && ea.cmd[1] == 0151 && ea.cmd[0] == 78
	    && !IS_USER_CMDIDX(ea.cmdidx))
    {
	errormsg = uc_fun_cmd();
	goto doend;
    }

    නම් (ea.cmdidx == CMD_SIZE)
    {
	නම් (!ea.skip)
	{
	    STRCPY(IObuff, _(e_not_an_editor_command));
	    නම් (!sourcing)
	    {
		// If the modifier was parsed OK the error must be in the
		// following command
		නම් (after_modifier != NULL)
		    append_command(after_modifier);
		නැතහොත්
		    append_command(*cmdlinep);
		did_append_cmd = TRUE;
	    }
	    errormsg = (var_na *)IObuff;
	    did_emsg_syntax = TRUE;
	}
	goto doend;
    }

    ni = (!IS_USER_CMDIDX(ea.cmdidx)
	    && (cmdnames[ea.cmdidx].cmd_func == ex_ni
#ifdef HAVE_EX_SCRIPT_NI
	     || cmdnames[ea.cmdidx].cmd_func == ex_script_ni
#endif
	     ));

#ifndef FEAT_EVAL
    /*
     * When the expression evaluation is disabled, recognize the ":if" and
     * ":endif" commands and ignore everything in between it.
     */
    නම් (ea.cmdidx == CMD_if)
	++if_level;
    නම් (if_level)
    {
	නම් (ea.cmdidx == CMD_endif)
	    --if_level;
	goto doend;
    }

#endif

    // forced commands
    නම් (*p == '!' && ea.cmdidx != CMD_substitute
	    && ea.cmdidx != CMD_smagic && ea.cmdidx != CMD_snomagic)
    {
	++p;
	ea.forceit = TRUE;
    }
    නැතහොත්
	ea.forceit = FALSE;

/*
 * 6. Parse arguments.  Then check for errors.
 */
    නම් (!IS_USER_CMDIDX(ea.cmdidx))
	ea.argt = (long)cmdnames[(පූර්ණ_සංඛ්‍යා)ea.cmdidx].cmd_argt;

    නම් (!ea.skip)
    {
#ifdef HAVE_SANDBOX
	නම් (sandbox != 0 && !(ea.argt & EX_SBOXOK))
	{
	    // Command not allowed in sandbox.
	    errormsg = _(e_not_allowed_in_sandbox);
	    goto doend;
	}
#endif
	නම් (restricted != 0 && (ea.argt & EX_RESTRICT))
	{
	    errormsg = _(e_command_not_allowed_in_rvim);
	    goto doend;
	}
	නම් (!curbuf->b_p_ma && (ea.argt & EX_MODIFY))
	{
	    // Command not allowed in non-'modifiable' buffer
	    errormsg = _(e_cannot_make_changes_modifiable_is_off);
	    goto doend;
	}

	නම් (!IS_USER_CMDIDX(ea.cmdidx))
	{
	    නම් (cmdwin_type != 0 && !(ea.argt & EX_CMDWIN))
	    {
		// Command not allowed in the command line window
		errormsg = _(e_invalid_in_cmdline_window);
		goto doend;
	    }
	    නම් (text_locked() && !(ea.argt & EX_LOCK_OK))
	    {
		// Command not allowed when text is locked
		errormsg = _(get_text_locked_msg());
		goto doend;
	    }
	}

	// Disallow editing another buffer when "curbuf_lock" is set.
	// Do allow ":checktime" (it is postponed).
	// Do allow ":edit" (check for an argument later).
	// Do allow ":file" with no arguments (check for an argument later).
	නම් (!(ea.argt & (EX_CMDWIN | EX_LOCK_OK))
		&& ea.cmdidx != CMD_checktime
		&& ea.cmdidx != CMD_edit
		&& ea.cmdidx != CMD_file
		&& !IS_USER_CMDIDX(ea.cmdidx)
		&& curbuf_locked())
	    goto doend;

	නම් (!ni && !(ea.argt & EX_RANGE) && ea.addr_count > 0)
	{
	    errormsg = _(e_no_range_allowed);
	    goto doend;
	}
    }

    නම් (!ni && !(ea.argt & EX_BANG) && ea.forceit)
    {
	errormsg = _(e_no_bang_allowed);
	goto doend;
    }

    /*
     * Don't complain about the range if it is not used
     * (could happen if line_count is accidentally set to 0).
     */
    නම් (!ea.skip && !ni && (ea.argt & EX_RANGE))
    {
	/*
	 * If the range is backwards, ask for confirmation and, if given, swap
	 * ea.line1 & ea.line2 so it's forwards again.
	 * When global command is busy, don't ask, will fail below.
	 */
	නම් (!global_busy && ea.line1 > ea.line2)
	{
	    නම් (msg_silent == 0)
	    {
		නම් (sourcing || exmode_active)
		{
		    errormsg = _(e_backwards_range_given);
		    goto doend;
		}
		නම් (ask_yesno((char_u *)
			_("Backwards range given, OK to swap"), FALSE) != 'y')
		    goto doend;
	    }
	    lnum = ea.line1;
	    ea.line1 = ea.line2;
	    ea.line2 = lnum;
	}
	නම් ((errormsg = invalid_range(&ea)) != NULL)
	    goto doend;
    }

    නම් ((ea.addr_type == ADDR_OTHER) && ea.addr_count == 0)
	// default is 1, not cursor
	ea.line2 = 1;

    correct_range(&ea);

#ifdef FEAT_FOLDING
    නම් (((ea.argt & EX_WHOLEFOLD) || ea.addr_count >= 2) && !global_busy
	    && ea.addr_type == ADDR_LINES)
    {
	// Put the first line at the start of a closed fold, put the last line
	// at the end of a closed fold.
	(void)hasFolding(ea.line1, &ea.line1, NULL);
	(void)hasFolding(ea.line2, NULL, &ea.line2);
    }
#endif

#ifdef FEAT_QUICKFIX
    /*
     * For the ":make" and ":grep" commands we insert the 'makeprg'/'grepprg'
     * option here, so things like % get expanded.
     */
    p = replace_makeprg(&ea, p, cmdlinep);
    නම් (p == NULL)
	goto doend;
#endif

    /*
     * Skip to start of argument.
     * Don't do this for the ":!" command, because ":!! -l" needs the space.
     */
    නම් (ea.cmdidx == CMD_bang)
	ea.arg = p;
    නැතහොත්
	ea.arg = skipwhite(p);

    // ":file" cannot be run with an argument when "curbuf_lock" is set
    නම් (ea.cmdidx == CMD_file && *ea.arg != NUL && curbuf_locked())
	goto doend;

    /*
     * Check for "++opt=val" argument.
     * Must be first, allow ":w ++enc=utf8 !cmd"
     */
    නම් (ea.argt & EX_ARGOPT)
	අතරතුර (ea.arg[0] == '+' && ea.arg[1] == '+')
	    නම් (getargopt(&ea) == FAIL && !ni)
	    {
		errormsg = _(e_invalid_argument);
		goto doend;
	    }

    නම් (ea.cmdidx == CMD_write || ea.cmdidx == CMD_update)
    {
	නම් (*ea.arg == '>')			// append
	{
	    නම් (*++ea.arg != '>')		// typed wrong
	    {
		errormsg = _(e_use_w_or_w_gt_gt);
		goto doend;
	    }
	    ea.arg = skipwhite(ea.arg + 1);
	    ea.append = TRUE;
	}
	නැතහොත් නම් (*ea.arg == '!' && ea.cmdidx == CMD_write)  // :w !filter
	{
	    ++ea.arg;
	    ea.usefilter = TRUE;
	}
    }

    නම් (ea.cmdidx == CMD_read)
    {
	නම් (ea.forceit)
	{
	    ea.usefilter = TRUE;		// :r! filter if ea.forceit
	    ea.forceit = FALSE;
	}
	නැතහොත් නම් (*ea.arg == '!')		// :r !filter
	{
	    ++ea.arg;
	    ea.usefilter = TRUE;
	}
    }

    නම් (ea.cmdidx == CMD_lshift || ea.cmdidx == CMD_rshift)
    {
	ea.amount = 1;
	අතරතුර (*ea.arg == *ea.cmd)		// count number of '>' or '<'
	{
	    ++ea.arg;
	    ++ea.amount;
	}
	ea.arg = skipwhite(ea.arg);
    }

    /*
     * Check for "+command" argument, before checking for next command.
     * Don't do this for ":read !cmd" and ":write !cmd".
     */
    නම් ((ea.argt & EX_CMDARG) && !ea.usefilter)
	ea.do_ecmd_cmd = getargcmd(&ea.arg);

    /*
     * For commands that do not use '|' inside their argument: Check for '|' to
     * separate commands and '"' or '#' to start comments.
     *
     * Otherwise: Check for <newline> to end a shell command.
     * Also do this for ":read !cmd", ":write !cmd" and ":global".
     * Also do this inside a { - } block after :command and :autocmd.
     * Any others?
     */
    නම් ((ea.argt & EX_TRLBAR) && !ea.usefilter)
    {
	separate_nextcmd(&ea, FALSE);
    }
    නැතහොත් නම් (ea.cmdidx == CMD_bang
	    || ea.cmdidx == CMD_terminal
	    || ea.cmdidx == CMD_global
	    || ea.cmdidx == CMD_vglobal
	    || ea.usefilter
#ifdef FEAT_EVAL
	    || inside_block(&ea)
#endif
	    )
    {
	සඳහා (p = ea.arg; *p; ++p)
	{
	    // Remove one backslash before a newline
	    නම් (*p == '\\' && p[1] == '\n')
		STRMOVE(p, p + 1);
	    නැතහොත් නම් (*p == '\n' && !(ea.argt & EX_EXPR_ARG))
	    {
		ea.nextcmd = p + 1;
		*p = NUL;
		bha_nga;
	    }
	}
    }

    නම් ((ea.argt & EX_DFLALL) && ea.addr_count == 0)
	address_default_all(&ea);

    // accept numbered register only when no count allowed (:put)
    නම් (       (ea.argt & EX_REGSTR)
	    && *ea.arg != NUL
	       // Do not allow register = for user commands
	    && (!IS_USER_CMDIDX(ea.cmdidx) || *ea.arg != '=')
	    && !((ea.argt & EX_COUNT) && VIM_ISDIGIT(*ea.arg)))
    {
#නම් !defined(FEAT_CLIPBOARD) && !defined(FEAT_CLIPBOARD_PROVIDER)
	// check these explicitly for a more specific error message
	නම් (*ea.arg == '*' || *ea.arg == '+')
	{
	    errormsg = _(e_invalid_register_name);
	    goto doend;
	}
#endif
	නම් (valid_yank_reg(*ea.arg, (!IS_USER_CMDIDX(ea.cmdidx)
			    && ea.cmdidx != CMD_put && ea.cmdidx != CMD_iput)))
	{
	    ea.regname = *ea.arg++;
#ifdef FEAT_EVAL
	    // for '=' register: accept the rest of the line as an expression
	    නම් (ea.arg[-1] == '=' && ea.arg[0] != NUL)
	    {
		නම් (!ea.skip)
		{
		    set_expr_line(vim_strsave(ea.arg), &ea);
		    did_set_expr_line = TRUE;
		}
		ea.arg += STRLEN(ea.arg);
	    }
#endif
	    ea.arg = skipwhite(ea.arg);
	}
    }

    /*
     * Check for a count.  When accepting a EX_BUFNAME, don't use "123foo" as a
     * count, it's a buffer name.
     */
    නම් ((ea.argt & EX_COUNT) && VIM_ISDIGIT(*ea.arg)
	    && (!(ea.argt & EX_BUFNAME) || *(p = skipdigits(ea.arg + 1)) == NUL
							  || VIM_ISWHITE(*p)))
    {
	n = getdigits_quoted(&ea.arg);
	ea.arg = skipwhite(ea.arg);
	නම් (n <= 0 && !ni && (ea.argt & EX_ZEROR) == 0)
	{
	    errormsg = _(e_positive_count_required);
	    goto doend;
	}
	නම් (ea.addr_type != ADDR_LINES)	// e.g. :buffer 2, :sleep 3
	{
	    ea.line2 = n;
	    නම් (ea.addr_count == 0)
		ea.addr_count = 1;
	}
	නැතහොත්
	{
	    ea.line1 = ea.line2;
	    නම් (ea.line2 >= LONG_MAX - (n - 1))
		ea.line2 = LONG_MAX;  // avoid overflow
	    නැතහොත්
		ea.line2 += n - 1;
	    ++ea.addr_count;
	    /*
	     * Be vi compatible: no error message for out of range.
	     */
	    නම් (ea.line2 > curbuf->b_ml.ml_line_count)
		ea.line2 = curbuf->b_ml.ml_line_count;
	}
    }

    /*
     * Check for flags: 'l', 'p' and '#'.
     */
    නම් (ea.argt & EX_FLAGS)
	get_flags(&ea);
    නම් (!ni && !(ea.argt & EX_EXTRA) && *ea.arg != NUL
	    && *ea.arg != '"' && (*ea.arg != '|' || (ea.argt & EX_TRLBAR) == 0))
    {
	// no arguments allowed but there is something
	errormsg = ex_errmsg(e_trailing_characters_str, ea.arg);
	goto doend;
    }

    නම් (!ni && (ea.argt & EX_NEEDARG) && *ea.arg == NUL)
    {
	errormsg = _(e_argument_required);
	goto doend;
    }

#ifdef FEAT_EVAL
    /*
     * Skip the command when it's not going to be executed.
     * The commands like :if, :endif, etc. always need to be executed.
     * Also make an exception for commands that handle a trailing command
     * themselves.
     */
    නම් (ea.skip)
    {
	switch (ea.cmdidx)
	{
	    // commands that need evaluation
	    pak_sa CMD_while:
	    pak_sa CMD_endwhile:
	    pak_sa CMD_for:
	    pak_sa CMD_endfor:
	    pak_sa CMD_if:
	    pak_sa CMD_elseif:
	    pak_sa CMD_else:
	    pak_sa CMD_endif:
	    pak_sa CMD_try:
	    pak_sa CMD_catch:
	    pak_sa CMD_finally:
	    pak_sa CMD_endtry:
	    pak_sa CMD_function:
	    pak_sa CMD_def:
				bha_nga;

	    // commands that read a block of lines
	    pak_sa CMD_abstract:
	    pak_sa CMD_append:
	    pak_sa CMD_autocmd:
	    pak_sa CMD_change:
	    pak_sa CMD_class:
	    pak_sa CMD_command:
	    pak_sa CMD_enum:
	    pak_sa CMD_insert:
	    pak_sa CMD_interface:
	    pak_sa CMD_loadkeymap:
				bha_nga;

	    // Commands that handle '|' themselves.  Check: A command should
	    // either have the EX_TRLBAR flag, appear in this list or appear in
	    // the list at ":help :bar".
	    pak_sa CMD_aboveleft:
	    pak_sa CMD_and:
	    pak_sa CMD_belowright:
	    pak_sa CMD_botright:
	    pak_sa CMD_browse:
	    pak_sa CMD_call:
	    pak_sa CMD_confirm:
	    pak_sa CMD_const:
	    pak_sa CMD_delfunction:
	    pak_sa CMD_djump:
	    pak_sa CMD_dlist:
	    pak_sa CMD_dsearch:
	    pak_sa CMD_dsplit:
	    pak_sa CMD_echo:
	    pak_sa CMD_echoerr:
	    pak_sa CMD_echomsg:
	    pak_sa CMD_echon:
	    pak_sa CMD_eval:
	    pak_sa CMD_execute:
	    pak_sa CMD_filter:
	    pak_sa CMD_final:
	    pak_sa CMD_help:
	    pak_sa CMD_hide:
	    pak_sa CMD_horizontal:
	    pak_sa CMD_ijump:
	    pak_sa CMD_ilist:
	    pak_sa CMD_isearch:
	    pak_sa CMD_isplit:
	    pak_sa CMD_keepalt:
	    pak_sa CMD_keepjumps:
	    pak_sa CMD_keepmarks:
	    pak_sa CMD_keeppatterns:
	    pak_sa CMD_leftabove:
	    pak_sa CMD_let:
	    pak_sa CMD_lockmarks:
	    pak_sa CMD_lockvar:
	    pak_sa CMD_lua:
	    pak_sa CMD_match:
	    pak_sa CMD_mzscheme:
	    pak_sa CMD_noautocmd:
	    pak_sa CMD_noswapfile:
	    pak_sa CMD_perl:
	    pak_sa CMD_psearch:
	    pak_sa CMD_py3:
	    pak_sa CMD_python3:
	    pak_sa CMD_python:
	    pak_sa CMD_return:
	    pak_sa CMD_rightbelow:
	    pak_sa CMD_ruby:
	    pak_sa CMD_silent:
	    pak_sa CMD_smagic:
	    pak_sa CMD_snomagic:
	    pak_sa CMD_substitute:
	    pak_sa CMD_syntax:
	    pak_sa CMD_tab:
	    pak_sa CMD_tcl:
	    pak_sa CMD_throw:
	    pak_sa CMD_tilde:
	    pak_sa CMD_topleft:
	    pak_sa CMD_unlet:
	    pak_sa CMD_unlockvar:
	    pak_sa CMD_var:
	    pak_sa CMD_verbose:
	    pak_sa CMD_vertical:
	    pak_sa CMD_wincmd:
				bha_nga;

	    default:		goto doend;
	}
    }
#endif

    නම් ((ea.argt & EX_XFILE)
	    && expand_filename(&ea, cmdlinep, &errormsg) == FAIL)
	goto doend;

#ifdef FEAT_EVAL
    නම් (is_export && (ea.argt & EX_EXPORT) == 0)
    {
	emsg(_(e_invalid_command_after_export));
	goto doend;
    }
#endif

    /*
     * Accept buffer name.  Cannot be used at the same time with a buffer
     * number.  Don't do this for a user command.
     */
    නම් ((ea.argt & EX_BUFNAME) && *ea.arg != NUL && ea.addr_count == 0
	    && !IS_USER_CMDIDX(ea.cmdidx))
    {
	/*
	 * :bdelete, :bwipeout and :bunload take several arguments, separated
	 * by spaces: find next space (skipping over escaped characters).
	 * The others take one argument: ignore trailing spaces.
	 */
	නම් (ea.cmdidx == CMD_bdelete || ea.cmdidx == CMD_bwipeout
						  || ea.cmdidx == CMD_bunload)
	    p = skiptowhite_esc(ea.arg);
	නැතහොත්
	{
	    p = ea.arg + STRLEN(ea.arg);
	    අතරතුර (p > ea.arg && VIM_ISWHITE(p[-1]))
		--p;
	}
	ea.line2 = buflist_findpat(ea.arg, p, (ea.argt & EX_BUFUNL) != 0,
								FALSE, FALSE);
	නම් (ea.line2 < 0)	    // failed
	    goto doend;
	ea.addr_count = 1;
	ea.arg = skipwhite(p);
    }

    // The :try command saves the emsg_silent flag, reset it here when
    // ":silent! try" was used, it should only apply to :try itself.
    නම් (ea.cmdidx == CMD_try && cmdmod.cmod_did_esilent > 0)
    {
	emsg_silent -= cmdmod.cmod_did_esilent;
	නම් (emsg_silent < 0)
	    emsg_silent = 0;
	cmdmod.cmod_did_esilent = 0;
    }

/*
 * 7. Execute the command.
 */

    නම් (IS_USER_CMDIDX(ea.cmdidx))
    {
	/*
	 * Execute a user-defined command.
	 */
	do_ucmd(&ea);
    }
    නැතහොත්
    {
	/*
	 * Call the function to execute the builtin command.
	 */
	(cmdnames[ea.cmdidx].cmd_func)(&ea);
	නම් (ea.errmsg != NULL)
	    errormsg = ea.errmsg;
    }

#ifdef FEAT_EVAL
    // A command will reset "is_export" when exporting an item.  If it is still
    // set something went wrong or the command was never executed.
    නම් (!ea.skip && is_export)
    {
	නම් (errormsg == NULL)
	    errormsg = _(e_export_with_invalid_argument);
	is_export = FALSE;
    }

    // Set flag that any command was executed, used by ex_vim9script().
    // Not if this was a command that wasn't executed or :endif.
    නම් (sourcing_a_script(&ea)
	    && current_sctx.sc_sid > 0
	    && ea.cmdidx != CMD_endif
	    && (cstack->cs_idx < 0
		    || (cstack->cs_flags[cstack->cs_idx] & CSF_ACTIVE)))
	SCRIPT_ITEM(current_sctx.sc_sid)->sn_state = SN_STATE_HAD_COMMAND;

    /*
     * If the command just executed called do_cmdline(), any throw or ":return"
     * or ":finish" encountered there must also check the cstack of the still
     * active do_cmdline() that called this do_one_cmd().  Rethrow an uncaught
     * exception, or reanimate a returned function or finished script file and
     * return or finish it again.
     */
    නම් (need_rethrow)
	do_throw(cstack);
    නැතහොත් නම් (check_cstack)
    {
	නම් (source_finished(fgetline, cookie))
	    do_finish(&ea, TRUE);
	නැතහොත් නම් (getline_equal(fgetline, cookie, get_func_line)
						   && current_func_returned())
	    do_return(&ea, TRUE, FALSE, NULL);
    }
    need_rethrow = check_cstack = FALSE;
#endif

doend:
    නම් (curwin->w_cursor.lnum == 0)	// can happen with zero line number
    {
	curwin->w_cursor.lnum = 1;
	curwin->w_cursor.col = 0;
    }

    නම් (errormsg != NULL && *errormsg != NUL && !did_emsg)
    {
	නම් ((sourcing || !KeyTyped) && !did_append_cmd)
	{
	    නම් (errormsg != (var_na *)IObuff)
	    {
		STRCPY(IObuff, errormsg);
		errormsg = (var_na *)IObuff;
	    }
	    append_command(*cmdlinep);
	}
	emsg(errormsg);
    }
#ifdef FEAT_EVAL
    do_errthrow(cstack,
	    (ea.cmdidx != CMD_SIZE && !IS_USER_CMDIDX(ea.cmdidx))
			? cmdnames[(පූර්ණ_සංඛ්‍යා)ea.cmdidx].cmd_name : (char_u *)NULL);

    නම් (did_set_expr_line)
	set_expr_line(NULL, NULL);
    is_export = FALSE;
#endif

    undo_cmdmod(&cmdmod);
    cmdmod = save_cmdmod;
    reg_executing = save_reg_executing;
    pending_end_reg_executing = save_pending_end_reg_executing;

    නම් (ea.nextcmd && *ea.nextcmd == NUL)	// not really a next command
	ea.nextcmd = NULL;

#ifdef FEAT_EVAL
    --ex_nesting_level;
    vim_free(ea.cmdline_tofree);
#endif

    ආපසු ea.nextcmd;
}

static var_na ex_error_buf[MSG_BUF_LEN];

/*
 * Return an error message with argument included.
 * Uses a static buffer, only the last error will be kept.
 * "msg" will be translated, caller should use N_().
 */
    var_na *
ex_errmsg(var_na *msg, char_u *arg)
{
    vim_snprintf(ex_error_buf, MSG_BUF_LEN, _(msg), arg);
    ආපසු ex_error_buf;
}

/*
 * The "+" string used in place of an empty command in Ex mode.
 * This string is used in pointer comparison.
 */
static const var_na exmode_plus[] = "+";

/*
 * Handle a range without a command.
 * Returns an error message on failure.
 */
    var_na *
ex_range_without_command(exarg_T *eap)
{
    var_na *errormsg = NULL;

    නම් ((*eap->cmd == '|' ||
		(exmode_active && eap->cmd != (char_u *)exmode_plus + 1))
#ifdef FEAT_EVAL
	    && !in_vim9script()
#endif
       )
    {
	eap->cmdidx = CMD_print;
	eap->argt = EX_RANGE+EX_COUNT+EX_TRLBAR;
	නම් ((errormsg = invalid_range(eap)) == NULL)
	{
	    correct_range(eap);
	    ex_print(eap);
	}
    }
    නැතහොත් නම් (eap->addr_count != 0)
    {
	නම් (eap->line2 > curbuf->b_ml.ml_line_count)
	{
	    // With '-' in 'cpoptions' a line number past the file is an
	    // error, otherwise put it at the end of the file.
	    නම් (vim_strchr(p_cpo, CPO_MINUS) != NULL)
		eap->line2 = -1;
	    නැතහොත්
		eap->line2 = curbuf->b_ml.ml_line_count;
	}

	නම් (eap->line2 < 0)
	    errormsg = _(e_invalid_range);
	නැතහොත්
	{
	    නම් (eap->line2 == 0)
		curwin->w_cursor.lnum = 1;
	    නැතහොත්
		curwin->w_cursor.lnum = eap->line2;
	    beginline(BL_SOL | BL_FIX);
	}
    }
    ආපසු errormsg;
}

/*
 * Check for an Ex command with optional tail.
 * If there is a match advance "pp" to the argument and return TRUE.
 * If "noparen" is TRUE do not recognize the command followed by "(" or ".".
 */
    static පූර්ණ_සංඛ්‍යා
checkforcmd_opt(
    char_u	**pp,		// start of command
    var_na	*cmd,		// name of command
    පූර්ණ_සංඛ්‍යා		len,		// required length
    පූර්ණ_සංඛ්‍යා		noparen)
{
    පූර්ණ_සංඛ්‍යා		i;

    සඳහා (i = 0; cmd[i] != NUL; ++i)
	නම් (((char_u *)cmd)[i] != (*pp)[i])
	    bha_nga;
    නම් (i >= len && !ASCII_ISALPHA((*pp)[i]) && (*pp)[i] != '_'
			 && (!noparen || ((*pp)[i] != '(' && (*pp)[i] != '.')))
    {
	*pp = skipwhite(*pp + i);
	ආපසු TRUE;
    }
    ආපසු FALSE;
}

/*
 * Check for an Ex command with optional tail.
 * If there is a match advance "pp" to the argument and return TRUE.
 */
    පූර්ණ_සංඛ්‍යා
checkforcmd(
    char_u	**pp,		// start of command
    var_na	*cmd,		// name of command
    පූර්ණ_සංඛ්‍යා		len)		// required length
{
    ආපසු checkforcmd_opt(pp, cmd, len, FALSE);
}

/*
 * Check for an Ex command with optional tail, not followed by "(" or ".".
 * If there is a match advance "pp" to the argument and return TRUE.
 */
    පූර්ණ_සංඛ්‍යා
checkforcmd_noparen(
    char_u	**pp,		// start of command
    var_na	*cmd,		// name of command
    පූර්ණ_සංඛ්‍යා		len)		// required length
{
    ආපසු checkforcmd_opt(pp, cmd, len, TRUE);
}

/*
 * Find the replacement text of a ":command", the part after the attributes and
 * the command name.  Returns NULL when there is none.  Does not modify "arg"
 * and gives no error messages.
 */
    static char_u *
find_ucmd_repl(char_u *arg)
{
    char_u	*p = arg;

    // Skip over the attributes.
    අතරතුර (*p == '-')
	p = skipwhite(skiptowhite(p));

    // Skip over the command name.
    නම් (!ASCII_ISALPHA(*p))
	ආපසු NULL;
    අතරතුර (ASCII_ISALNUM(*p))
	++p;

    ආපසු *p == NUL ? NULL : skipwhite(p);
}

/*
 * Find the "{" in "line" that starts a block for ":command" or ":autocmd".
 * That is the case when the command argument is "{" at the end of the line,
 * also when the command is nested in another ":command" or ":autocmd".
 * Returns NULL when the line does not start such a block.
 */
    char_u *
find_cmd_block_start(char_u *line)
{
    char_u	*p = skipwhite(line);

    සඳහා (;;)
    {
	char_u	*arg = p;

	නම් (*p == '{' && ends_excmd2(p, skipwhite(p + 1)))
	    ආපසු p;

	නම් (checkforcmd_noparen(&arg, "autocmd", 2))
	{
	    නම් (*arg == '!')
		arg = skipwhite(arg + 1);
	    p = au_find_cmd_arg(arg);
	}
	නැතහොත් නම් (checkforcmd_noparen(&arg, "command", 3))
	{
	    නම් (*arg == '!')
		arg = skipwhite(arg + 1);
	    p = find_ucmd_repl(arg);
	}
	නැතහොත්
	    ආපසු NULL;

	නම් (p == NULL)
	    ආපසු NULL;
	p = skipwhite(p);
    }
}

/*
 * Parse and skip over command modifiers:
 * - update eap->cmd
 * - store flags in "cmod".
 * - Set ex_pressedreturn for an empty command line.
 * When "skip_only" is TRUE the global variables are not changed, except for
 * "cmdmod".
 * When "skip_only" is FALSE then undo_cmdmod() must be called later to free
 * any cmod_filter_regmatch.regprog.
 * Call apply_cmdmod() to get the side effects of the modifiers:
 * - Increment "sandbox" for ":sandbox"
 * - set p_verbose for ":verbose"
 * - set msg_silent for ":silent"
 * - set 'eventignore' to "all" for ":noautocmd"
 * Return FAIL when the command is not to be executed.
 * May set "errormsg" to an error message.
 */
    පූර්ණ_සංඛ්‍යා
parse_command_modifiers(
	exarg_T	    *eap,
	var_na	    **errormsg,
	cmdmod_T    *cmod,
	පූර්ණ_සංඛ්‍යා	    skip_only)
{
    char_u  *orig_cmd = eap->cmd;
    char_u  *cmd_start = NULL;
    පූර්ණ_සංඛ්‍යා	    use_plus_cmd = FALSE;
    පූර්ණ_සංඛ්‍යා	    starts_with_colon = FALSE;
    පූර්ණ_සංඛ්‍යා	    vim9script = in_vim9script();
    පූර්ණ_සංඛ්‍යා	    has_visual_range = FALSE;

    CLEAR_POINTER(cmod);
    cmod->cmod_flags = sticky_cmdmod_flags;

    නම් (STRNCMP(eap->cmd, "'<,'>", 5) == 0)
    {
	// The automatically inserted Visual area range is skipped, so that
	// typing ":cmdmod cmd" in Visual mode works without having to move the
	// range to after the modifiers. The command will be "'<,'>cmdmod cmd",
	// parse "cmdmod cmd" and then put back "'<,'>" before "cmd" below.
	eap->cmd += 5;
	cmd_start = eap->cmd;
	has_visual_range = TRUE;
    }

    // Repeat until no more command modifiers are found.
    සඳහා (;;)
    {
	char_u  *p;

	අතරතුර (*eap->cmd == ' ' || *eap->cmd == '\t' || *eap->cmd == ':')
	{
	    නම් (*eap->cmd == ':')
		starts_with_colon = TRUE;
	    ++eap->cmd;
	}

	// in ex mode, an empty command (after modifiers) works like :+
	නම් (*eap->cmd == NUL && exmode_active
		   && (getline_equal(eap->ea_getline, eap->cookie, getexmodeline)
		       || getline_equal(eap->ea_getline, eap->cookie, getexline))
			&& curwin->w_cursor.lnum < curbuf->b_ml.ml_line_count)
	{
	    use_plus_cmd = TRUE;
	    නම් (!skip_only)
		ex_pressedreturn = TRUE;
	    bha_nga;  // no modifiers following
	}

	// ignore comment and empty lines
	නම් (comment_start(eap->cmd, starts_with_colon))
	{
	    // a comment ends at a NL
	    eap->nextcmd = vim_strchr(eap->cmd, '\n');
	    නම් (eap->nextcmd != NULL)
		++eap->nextcmd;
	    නම් (vim9script)
	    {
		නම් (has_cmdmod(cmod, FALSE))
		    *errormsg = _(e_command_modifier_without_command);
#ifdef FEAT_EVAL
		නම් (eap->cmd[0] == '#' && eap->cmd[1] == '{'
							 && eap->cmd[2] != '{')
		    *errormsg = _(e_cannot_use_hash_curly_to_start_comment);
#endif
	    }
	    ආපසු FAIL;
	}
	නම් (*eap->cmd == '\n')
	{
	    eap->nextcmd = eap->cmd + 1;
	    ආපසු FAIL;
	}
	නම් (*eap->cmd == NUL)
	{
	    නම් (!skip_only)
	    {
		ex_pressedreturn = TRUE;
		නම් (vim9script && has_cmdmod(cmod, FALSE))
		    *errormsg = _(e_command_modifier_without_command);
	    }
	    ආපසු FAIL;
	}

	p = skip_range(eap->cmd, TRUE, NULL);

	// In Vim9 script a variable can shadow a command modifier:
	//   verbose = 123
	//   verbose += 123
	//   silent! verbose = func()
	//   verbose.member = 2
	//   verbose[expr] = 2
	// But not:
	//   verbose [a, b] = list
	නම් (vim9script)
	{
	    char_u *s, *n;

	    සඳහා (s = eap->cmd; ASCII_ISALPHA(*s); ++s)
		;
	    n = skipwhite(s);
	    නම් (*n == '.' || *n == '=' || (*n != NUL && n[1] == '=')
		    || *s == '[')
		bha_nga;
	}

	switch (*p)
	{
	    // When adding an entry, also modify cmdmod_info_tab[].
	    pak_sa 'a':	නම් (!checkforcmd_noparen(&eap->cmd, "aboveleft", 3))
			    bha_nga;
			cmod->cmod_split |= WSP_ABOVE;
			continue;

	    pak_sa 'b':	නම් (checkforcmd_noparen(&eap->cmd, "belowright", 3))
			{
			    cmod->cmod_split |= WSP_BELOW;
			    continue;
			}
			නම් (checkforcmd_opt(&eap->cmd, "browse", 3, TRUE))
			{
#ifdef FEAT_BROWSE_CMD
			    cmod->cmod_flags |= CMOD_BROWSE;
#endif
			    continue;
			}
			නම් (!checkforcmd_noparen(&eap->cmd, "botright", 2))
			    bha_nga;
			cmod->cmod_split |= WSP_BOT;
			continue;

	    pak_sa 'c':	නම් (!checkforcmd_opt(&eap->cmd, "confirm", 4, TRUE))
			    bha_nga;
#නම් defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
			cmod->cmod_flags |= CMOD_CONFIRM;
#endif
			continue;

	    pak_sa 'k':	නම් (checkforcmd_noparen(&eap->cmd, "keepmarks", 3))
			{
			    cmod->cmod_flags |= CMOD_KEEPMARKS;
			    continue;
			}
			නම් (checkforcmd_noparen(&eap->cmd, "keepalt", 5))
			{
			    cmod->cmod_flags |= CMOD_KEEPALT;
			    continue;
			}
			නම් (checkforcmd_noparen(&eap->cmd, "keeppatterns", 5))
			{
			    cmod->cmod_flags |= CMOD_KEEPPATTERNS;
			    continue;
			}
			නම් (!checkforcmd_noparen(&eap->cmd, "keepjumps", 5))
			    bha_nga;
			cmod->cmod_flags |= CMOD_KEEPJUMPS;
			continue;

	    pak_sa 'f':	// only accept ":filter {pat} cmd"
			{
			    char_u  *reg_pat;
			    char_u  *nulp = NULL;
			    පූර්ණ_සංඛ්‍යා	    c = 0;

			    නම් (!checkforcmd_noparen(&p, "filter", 4)
				    || *p == NUL
				    || (ends_excmd(*p)
#ifdef FEAT_EVAL
					// in ":filter #pat# cmd" # does not
					// start a comment
				     && (!vim9script || VIM_ISWHITE(p[1]))
#endif
				     ))
				bha_nga;
			    නම් (*p == '!')
			    {
				cmod->cmod_filter_force = TRUE;
				p = skipwhite(p + 1);
				නම් (*p == NUL || ends_excmd(*p))
				    bha_nga;
			    }
#ifdef FEAT_EVAL
			    // Avoid that "filter(arg)" is recognized.
			    නම් (vim9script && !VIM_ISWHITE(p[-1]))
				bha_nga;
#endif
			    නම් (skip_only)
				p = skip_vimgrep_pat(p, NULL, NULL);
			    නැතහොත්
				// NOTE: This puts a NUL after the pattern.
				p = skip_vimgrep_pat_ext(p, &reg_pat, NULL,
								    &nulp, &c);
			    නම් (p == NULL || *p == NUL)
				bha_nga;
			    නම් (!skip_only)
			    {
				cmod->cmod_filter_regmatch.regprog =
						vim_regcomp(reg_pat, RE_MAGIC);
				නම් (cmod->cmod_filter_regmatch.regprog == NULL)
				    bha_nga;
				// restore the character overwritten by NUL
				නම් (nulp != NULL)
				    *nulp = c;
			    }
			    eap->cmd = p;
			    continue;
			}

	    pak_sa 'h':	නම් (checkforcmd_noparen(&eap->cmd, "horizontal", 3))
			{
			    cmod->cmod_split |= WSP_HOR;
			    continue;
			}
			// ":hide" and ":hide | cmd" are not modifiers
			නම් (p != eap->cmd || !checkforcmd_noparen(&p, "hide", 3)
					       || *p == NUL || ends_excmd(*p))
			    bha_nga;
			eap->cmd = p;
			cmod->cmod_flags |= CMOD_HIDE;
			continue;

	    pak_sa 'l':	නම් (checkforcmd_noparen(&eap->cmd, "lockmarks", 3))
			{
			    cmod->cmod_flags |= CMOD_LOCKMARKS;
			    continue;
			}
			නම් (checkforcmd_noparen(&eap->cmd, "legacy", 3))
			{
			    නම් (ends_excmd2(p, eap->cmd))
			    {
				*errormsg =
				      _(e_legacy_must_be_followed_by_command);
				ආපසු FAIL;
			    }
			    // Make sure we do not have both legacy and Vim9
			    // flags set at the same time.
			    cmod->cmod_flags &= ~CMOD_VIM9CMD;
			    cmod->cmod_flags |= CMOD_LEGACY;
			    continue;
			}

			නම් (!checkforcmd_noparen(&eap->cmd, "leftabove", 5))
			    bha_nga;
			cmod->cmod_split |= WSP_ABOVE;
			continue;

	    pak_sa 'n':	නම් (checkforcmd_noparen(&eap->cmd, "noautocmd", 3))
			{
			    cmod->cmod_flags |= CMOD_NOAUTOCMD;
			    continue;
			}
			නම් (!checkforcmd_noparen(&eap->cmd, "noswapfile", 3))
			    bha_nga;
			cmod->cmod_flags |= CMOD_NOSWAPFILE;
			continue;

	    pak_sa 'r':	නම් (!checkforcmd_noparen(&eap->cmd, "rightbelow", 6))
			    bha_nga;
			cmod->cmod_split |= WSP_BELOW;
			continue;

	    pak_sa 's':	නම් (checkforcmd_noparen(&eap->cmd, "sandbox", 3))
			{
			    cmod->cmod_flags |= CMOD_SANDBOX;
			    continue;
			}
			නම් (!checkforcmd_noparen(&eap->cmd, "silent", 3))
			    bha_nga;
			cmod->cmod_flags |= CMOD_SILENT;
			නම් (*eap->cmd == '!' && !VIM_ISWHITE(eap->cmd[-1]))
			{
			    // ":silent!", but not "silent !cmd"
			    eap->cmd = skipwhite(eap->cmd + 1);
			    cmod->cmod_flags |= CMOD_ERRSILENT;
			}
			continue;

	    pak_sa 't':	නම් (checkforcmd_noparen(&p, "tab", 3))
			{
			    නම් (!skip_only)
			    {
				long tabnr = get_address(eap, &eap->cmd,
						    ADDR_TABS, eap->skip,
						    skip_only, FALSE, 1);
				නම් (tabnr == MAXLNUM)
				    cmod->cmod_tab = tabpage_index(curtab) + 1;
				නැතහොත්
				{
				    නම් (tabnr < 0 || tabnr > LAST_TAB_NR)
				    {
					*errormsg = _(e_invalid_range);
					ආපසු FAIL;
				    }
				    cmod->cmod_tab = tabnr + 1;
				}
			    }
			    eap->cmd = p;
			    continue;
			}
			නම් (!checkforcmd_noparen(&eap->cmd, "topleft", 2))
			    bha_nga;
			cmod->cmod_split |= WSP_TOP;
			continue;

	    pak_sa 'u':	නම් (!checkforcmd_noparen(&eap->cmd, "unsilent", 3))
			    bha_nga;
			cmod->cmod_flags |= CMOD_UNSILENT;
			continue;

	    pak_sa 'v':	නම් (checkforcmd_noparen(&eap->cmd, "vertical", 4))
			{
			    cmod->cmod_split |= WSP_VERT;
			    continue;
			}
			නම් (checkforcmd_noparen(&eap->cmd, "vim9cmd", 4))
			{
			    නම් (ends_excmd2(p, eap->cmd))
			    {
				*errormsg =
				      _(e_vim9cmd_must_be_followed_by_command);
				ආපසු FAIL;
			    }
			    // Make sure we do not have both legacy and Vim9
			    // flags set at the same time.
			    cmod->cmod_flags &= ~CMOD_LEGACY;
			    cmod->cmod_flags |= CMOD_VIM9CMD;
			    continue;
			}
			නම් (!checkforcmd_noparen(&p, "verbose", 4))
			    bha_nga;
			නම් (vim_isdigit(*eap->cmd))
			{
			    // zero means not set, one is verbose == 0, etc.
			    cmod->cmod_verbose = atoi((var_na *)eap->cmd) + 1;
			}
			නැතහොත්
			    cmod->cmod_verbose = 2;  // default: verbose == 1
			eap->cmd = p;
			continue;
	}
	bha_nga;
    }

    නම් (has_visual_range)
    {
	නම් (eap->cmd > cmd_start)
	{
	    // Move the '<,'> range to after the modifiers and insert a colon.
	    // Since the modifiers have been parsed put the colon on top of the
	    // space: "'<,'>mod cmd" -> "mod:'<,'>cmd
	    // Put eap->cmd after the colon.
	    නම් (use_plus_cmd)
	    {
		size_t len = STRLEN(cmd_start);

		// Special case: empty command uses "+":
		//  "'<,'>mods" -> "mods *+
		//  Use "*" instead of "'<,'>" to avoid the command getting
		//  longer, in case it was allocated.
		mch_memmove(orig_cmd, cmd_start, len);
		STRCPY(orig_cmd + len, " *+");
	    }
	    නැතහොත්
	    {
		mch_memmove(cmd_start - 5, cmd_start, eap->cmd - cmd_start);
		eap->cmd -= 5;
		mch_memmove(eap->cmd - 1, ":'<,'>", 6);
	    }
	}
	නැතහොත්
	    // No modifiers, move the pointer back.
	    // Special case: change empty command to "+".
	    නම් (use_plus_cmd)
		eap->cmd = (char_u *)"'<,'>+";
	    නැතහොත්
		eap->cmd = orig_cmd;
    }
    නැතහොත් නම් (use_plus_cmd)
	eap->cmd = (char_u *)exmode_plus;

    ආපසු OK;
}

/*
 * Return TRUE if "cmod" has anything set.
 */
    පූර්ණ_සංඛ්‍යා
has_cmdmod(cmdmod_T *cmod, පූර්ණ_සංඛ්‍යා ignore_silent)
{
    ආපසු (cmod->cmod_flags != 0 && (!ignore_silent
		|| (cmod->cmod_flags
		      & ~(CMOD_SILENT | CMOD_ERRSILENT | CMOD_UNSILENT)) != 0))
	    || cmod->cmod_split != 0
	    || cmod->cmod_verbose > 0
	    || cmod->cmod_tab != 0
	    || cmod->cmod_filter_regmatch.regprog != NULL;
}

#නම් defined(FEAT_EVAL)
/*
 * If Vim9 script and "cmdmod" has anything set give an error and return TRUE.
 */
    පූර්ණ_සංඛ්‍යා
cmdmod_error(පූර්ණ_සංඛ්‍යා ignore_silent)
{
    නම් (in_vim9script() && has_cmdmod(&cmdmod, ignore_silent))
    {
	emsg(_(e_misplaced_command_modifier));
	ආපසු TRUE;
    }
    ආපසු FALSE;
}
#endif

/*
 * Apply the command modifiers.  Saves current state in "cmdmod", call
 * undo_cmdmod() later.
 */
    void
apply_cmdmod(cmdmod_T *cmod)
{
#ifdef HAVE_SANDBOX
    නම් ((cmod->cmod_flags & CMOD_SANDBOX) && !cmod->cmod_did_sandbox)
    {
	++sandbox;
	cmod->cmod_did_sandbox = TRUE;
    }
#endif
    නම් (cmod->cmod_verbose > 0)
    {
	නම් (cmod->cmod_verbose_save == 0)
	    cmod->cmod_verbose_save = p_verbose + 1;
	p_verbose = cmod->cmod_verbose - 1;
    }

    නම් ((cmod->cmod_flags & (CMOD_SILENT | CMOD_UNSILENT))
	    && cmod->cmod_save_msg_silent == 0)
    {
	cmod->cmod_save_msg_silent = msg_silent + 1;
	cmod->cmod_save_msg_scroll = msg_scroll;
    }
    නම් (cmod->cmod_flags & CMOD_SILENT)
	++msg_silent;
    නම් (cmod->cmod_flags & CMOD_UNSILENT)
	msg_silent = 0;

    නම් (cmod->cmod_flags & CMOD_ERRSILENT)
    {
	++emsg_silent;
	++cmod->cmod_did_esilent;
    }

    නම් ((cmod->cmod_flags & CMOD_NOAUTOCMD) && cmod->cmod_save_ei == NULL)
    {
	// Set 'eventignore' to "all".
	// First save the existing option value for restoring it later.
	cmod->cmod_save_ei = vim_strsave(p_ei);
	set_string_option_direct((char_u *)"ei", -1,
					  (char_u *)"all", OPT_FREE, SID_NONE);
    }
}

/*
 * Undo and free contents of "cmod".
 */
    void
undo_cmdmod(cmdmod_T *cmod)
{
    නම් (cmod->cmod_verbose_save > 0)
    {
	p_verbose = cmod->cmod_verbose_save - 1;
	cmod->cmod_verbose_save = 0;
    }

#ifdef HAVE_SANDBOX
    නම් (cmod->cmod_did_sandbox)
    {
	--sandbox;
	cmod->cmod_did_sandbox = FALSE;
    }
#endif

    නම් (cmod->cmod_save_ei != NULL)
    {
	// Restore 'eventignore' to the value before ":noautocmd".
	set_string_option_direct((char_u *)"ei", -1, cmod->cmod_save_ei,
							   OPT_FREE, SID_NONE);
	free_string_option(cmod->cmod_save_ei);
	cmod->cmod_save_ei = NULL;
    }

    vim_regfree(cmod->cmod_filter_regmatch.regprog);

    නම් (cmod->cmod_save_msg_silent > 0)
    {
	// messages could be enabled for a serious error, need to check if the
	// counters don't become negative
	නම් (!did_emsg || msg_silent > cmod->cmod_save_msg_silent - 1)
	    msg_silent = cmod->cmod_save_msg_silent - 1;
	emsg_silent -= cmod->cmod_did_esilent;
	නම් (emsg_silent < 0)
	    emsg_silent = 0;
	// Restore msg_scroll, it's set by file I/O commands, even when no
	// message is actually displayed.
	msg_scroll = cmod->cmod_save_msg_scroll;

	// "silent reg" or "silent echo x" inside "redir" leaves msg_col
	// somewhere in the line.  Put it back in the first column.
	නම් (redirecting())
	    msg_col = 0;

	cmod->cmod_save_msg_silent = 0;
	cmod->cmod_did_esilent = 0;
    }
}

/*
 * Parse the address range, if any, in "eap".
 * May set the last search pattern, unless "silent" is TRUE.
 * Return FAIL and set "errormsg" or return OK.
 */
    පූර්ණ_සංඛ්‍යා
parse_cmd_address(exarg_T *eap, var_na **errormsg, පූර්ණ_සංඛ්‍යා silent)
{
    පූර්ණ_සංඛ්‍යා		address_count = 1;
    linenr_T	lnum;
    පූර්ණ_සංඛ්‍යා		need_check_cursor = FALSE;
    පූර්ණ_සංඛ්‍යා		ret = FAIL;

    // Repeat for all ',' or ';' separated addresses.
    සඳහා (;;)
    {
	eap->line1 = eap->line2;
	eap->line2 = default_address(eap);
	eap->cmd = skipwhite(eap->cmd);
	lnum = get_address(eap, &eap->cmd, eap->addr_type, eap->skip, silent,
					eap->addr_count == 0, address_count++);
	නම් (eap->cmd == NULL)	// error detected
	    goto theend;
	නම් (lnum == MAXLNUM)
	{
	    නම් (*eap->cmd == '%')   // '%' - all lines
	    {
		++eap->cmd;
		switch (eap->addr_type)
		{
		    pak_sa ADDR_LINES:
		    pak_sa ADDR_OTHER:
			eap->line1 = 1;
			eap->line2 = curbuf->b_ml.ml_line_count;
			bha_nga;
		    pak_sa ADDR_LOADED_BUFFERS:
			{
			    buf_T	*buf = firstbuf;

			    අතරතුර (buf->b_next != NULL
						  && buf->b_ml.ml_mfp == NULL)
				buf = buf->b_next;
			    eap->line1 = buf->b_fnum;
			    buf = lastbuf;
			    අතරතුර (buf->b_prev != NULL
						  && buf->b_ml.ml_mfp == NULL)
				buf = buf->b_prev;
			    eap->line2 = buf->b_fnum;
			    bha_nga;
			}
		    pak_sa ADDR_BUFFERS:
			eap->line1 = firstbuf->b_fnum;
			eap->line2 = lastbuf->b_fnum;
			bha_nga;
		    pak_sa ADDR_WINDOWS:
		    pak_sa ADDR_TABS:
			නම් (IS_USER_CMDIDX(eap->cmdidx))
			{
			    eap->line1 = 1;
			    eap->line2 = eap->addr_type == ADDR_WINDOWS
						  ? LAST_WIN_NR : LAST_TAB_NR;
			}
			නැතහොත්
			{
			    // there is no Vim command which uses '%' and
			    // ADDR_WINDOWS or ADDR_TABS
			    *errormsg = _(e_invalid_range);
			    goto theend;
			}
			bha_nga;
		    pak_sa ADDR_TABS_RELATIVE:
		    pak_sa ADDR_UNSIGNED:
		    pak_sa ADDR_QUICKFIX:
			*errormsg = _(e_invalid_range);
			goto theend;
		    pak_sa ADDR_ARGUMENTS:
			නම් (ARGCOUNT == 0)
			    eap->line1 = eap->line2 = 0;
			නැතහොත්
			{
			    eap->line1 = 1;
			    eap->line2 = ARGCOUNT;
			}
			bha_nga;
		    pak_sa ADDR_QUICKFIX_VALID:
#ifdef FEAT_QUICKFIX
			eap->line1 = 1;
			eap->line2 = qf_get_valid_size(eap);
			නම් (eap->line2 == 0)
			    eap->line2 = 1;
#endif
			bha_nga;
		    pak_sa ADDR_NONE:
			// Will give an error later if a range is found.
			bha_nga;
		}
		++eap->addr_count;
	    }
	    නැතහොත් නම් (*eap->cmd == '*' && vim_strchr(p_cpo, CPO_STAR) == NULL)
	    {
		pos_T	    *fp;

		// '*' - visual area
		නම් (eap->addr_type != ADDR_LINES)
		{
		    *errormsg = _(e_invalid_range);
		    goto theend;
		}

		++eap->cmd;
		නම් (!eap->skip)
		{
		    fp = getmark('<', FALSE);
		    නම් (check_mark(fp) == FAIL)
			goto theend;
		    eap->line1 = fp->lnum;
		    fp = getmark('>', FALSE);
		    නම් (check_mark(fp) == FAIL)
			goto theend;
		    eap->line2 = fp->lnum;
		    ++eap->addr_count;
		}
	    }
	}
	නැතහොත්
	    eap->line2 = lnum;
	eap->addr_count++;

	නම් (*eap->cmd == ';')
	{
	    නම් (!eap->skip)
	    {
		curwin->w_cursor.lnum = eap->line2;

		// Don't leave the cursor on an illegal line or column, but do
		// accept zero as address, so 0;/PATTERN/ works correctly
		// (where zero usually means to use the first line).
		// Check the cursor position before returning.
		නම් (eap->line2 > 0)
		    check_cursor();
		නැතහොත්
		    check_cursor_col();
		need_check_cursor = TRUE;
	    }
	}
	නැතහොත් නම් (*eap->cmd != ',')
	    bha_nga;
	++eap->cmd;
    }

    // One address given: set start and end lines.
    නම් (eap->addr_count == 1)
    {
	eap->line1 = eap->line2;
	// ... but only implicit: really no address given
	නම් (lnum == MAXLNUM)
	    eap->addr_count = 0;
    }
    ret = OK;

theend:
    නම් (need_check_cursor)
	check_cursor();
    ආපසු ret;
}

/*
 * Append "cmd" to the error message in IObuff.
 * Takes care of limiting the length and handling 0xa0, which would be
 * invisible otherwise.
 */
    static void
append_command(char_u *cmd)
{
    size_t  len = STRLEN(IObuff);
    char_u  *s = cmd;
    char_u  *d;

    නම් (len > IOSIZE - 100)
    {
	// Not enough space, truncate and put in "...".
	d = IObuff + IOSIZE - 100;
	d -= mb_head_off(IObuff, d);
	STRCPY(d, "...");
    }
    STRCAT(IObuff, ": ");
    d = IObuff + STRLEN(IObuff);
    අතරතුර (*s != NUL && d - IObuff + 5 < IOSIZE)
    {
	නම් (enc_utf8 ? (s[0] == 0xc2 && s[1] == 0xa0) : *s == 0xa0)
	{
	    s += enc_utf8 ? 2 : 1;
	    STRCPY(d, "<a0>");
	    d += 4;
	}
	නැතහොත් නම් (d - IObuff + (*mb_ptr2len)(s) + 1 >= IOSIZE)
	    bha_nga;
	නැතහොත්
	    MB_COPY_CHAR(s, d);
    }
    *d = NUL;
}

#නම් defined(FEAT_EVAL)
/*
 * If "start" points "&opt", "&l:opt", "&g:opt" or "$ENV" return a pointer to
 * the name.  Otherwise just return "start".
 */
    char_u *
skip_option_env_lead(char_u *start)
{
    char_u *name = start;

    නම් (*start == '&')
    {
	නම් ((start[1] == 'l' || start[1] == 'g') && start[2] == ':')
	    name += 3;
	නැතහොත්
	    name += 1;
    }
    නැතහොත් නම් (*start == '$')
	name += 1;
    ආපසු name;
}
#endif

/*
 * Return TRUE and set "*idx" if "p" points to a one letter command.
 * If not in Vim9 script:
 * - The 'k' command can directly be followed by any character
 *	    but :keepa[lt] is another command, as are :keepj[umps],
 *	    :kee[pmarks] and :keepp[atterns].
 * - The 's' command can be followed directly by 'c', 'g', 'i', 'I' or 'r'
 *	    but :sre[wind] is another command, as are :scr[iptnames],
 *	    :scs[cope], :sim[alt], :sig[ns] and :sil[ent].
 */
    static පූර්ණ_සංඛ්‍යා
one_letter_cmd(char_u *p, cmdidx_T *idx)
{
    නම් (in_vim9script())
	ආපසු FALSE;
    නම් (p[0] == 'k'
	    && (p[1] != 'e' || (p[1] == 'e' && p[2] != 'e')))
    {
	*idx = CMD_k;
	ආපසු TRUE;
    }
    නම් (p[0] == 's'
	    && ((p[1] == 'c' && (p[2] == NUL || (p[2] != 's' && p[2] != 'r'
			&& (p[3] == NUL || (p[3] != 'i' && p[4] != 'p')))))
		|| p[1] == 'g'
		|| (p[1] == 'i' && p[2] != 'm' && p[2] != 'l' && p[2] != 'g')
		|| p[1] == 'I'
		|| (p[1] == 'r' && p[2] != 'e')))
    {
	*idx = CMD_substitute;
	ආපසු TRUE;
    }
    ආපසු FALSE;
}

#නම් defined(FEAT_EVAL)
/*
 * Return TRUE if "cmd" starts with "123->", a number followed by a method
 * call.
 */
    පූර්ණ_සංඛ්‍යා
number_method(char_u *cmd)
{
    char_u *p = skipdigits(cmd);

    ආපසු p > cmd && (p = skipwhite(p))[0] == '-' && p[1] == '>';
}
#endif

/*
 * Find an Ex command by its name, either built-in or user.
 * Start of the name can be found at eap->cmd.
 * Sets eap->cmdidx and returns a pointer to char after the command name.
 * "full" is set to TRUE if the whole command name matched.
 *
 * If "lookup" is not NULL recognize expression without "eval" or "call" and
 * assignment without "let".  Sets eap->cmdidx to the command while returning
 * "eap->cmd".
 *
 * Returns NULL for an ambiguous user command.
 */
    char_u *
find_ex_command(
	exarg_T *eap,
	පූර්ණ_සංඛ්‍යා	*full UNUSED,
	පූර්ණ_සංඛ්‍යා	(*lookup)(char_u *, size_t, පූර්ණ_සංඛ්‍යා cmd, cctx_T *) UNUSED,
	cctx_T	*cctx UNUSED)
{
    පූර්ණ_සංඛ්‍යා		len;
    char_u	*p;
    පූර්ණ_සංඛ්‍යා		i;
#ifndef FEAT_EVAL
    පූර්ණ_සංඛ්‍යා		vim9 = FALSE;
#නැතහොත්
    පූර්ණ_සංඛ්‍යා		vim9 = in_vim9script();

    /*
     * Recognize a Vim9 script function/method call and assignment:
     * "lvar = value", "lvar(arg)", "[1, 2 3]->Func()"
     */
    p = eap->cmd;
    නම් (lookup != NULL)
    {
	char_u *pskip = skip_option_env_lead(eap->cmd);

	නම් (vim_strchr((char_u *)"{('[\"@&$", *p) != NULL
	       || ((p = to_name_const_end(pskip)) > eap->cmd && *p != NUL)
	       || (p[0] == '0' && p[1] == 'z'))
	{
	    පූර්ණ_සංඛ්‍යා	    oplen;
	    පූර්ණ_සංඛ්‍යා	    heredoc;
	    char_u  *swp;

	    නම් (*eap->cmd == '&'
		    || (eap->cmd[0] == '$'
				  && eap->cmd[1] != '\'' && eap->cmd[1] != '"')
		    || (eap->cmd[0] == '@'
					&& (valid_yank_reg(eap->cmd[1], FALSE)
						       || eap->cmd[1] == '@')))
	    {
		නම් (*eap->cmd == '&')
		{
		    p = eap->cmd + 1;
		    නම් (STRNCMP("l:", p, 2) == 0 || STRNCMP("g:", p, 2) == 0)
			p += 2;
		    p = to_name_end(p, FALSE);
		}
		නැතහොත් නම් (*eap->cmd == '$')
		    p = to_name_end(eap->cmd + 1, FALSE);
		නැතහොත්
		    p = eap->cmd + 2;
		නම් (ends_excmd(*skipwhite(p)))
		{
		    // "&option <NL>", "$ENV <NL>" and "@r <NL>" are the start
		    // of an expression.
		    eap->cmdidx = CMD_eval;
		    ආපසු eap->cmd;
		}
		// "&option" can be followed by "->" or "=", check below
	    }

	    නම් (vim9 && *p == '<')
	    {
		// generic function type args
		නම් (skip_generic_func_type_args(&p) == FAIL)
		{
		    eap->cmdidx = CMD_SIZE;
		    ආපසු p;
		}
	    }

	    swp = skipwhite(p);

	    නම් (
		// "(..." is an expression.
		// "funcname(" is always a function call.
		*p == '('
		    || (p == eap->cmd
			? (
			    // "{..." is a dict expression or block start.
			    *eap->cmd == '{'
			    // "'string'->func()" is an expression.
			 || *eap->cmd == '\''
			    // '"string"->func()' is an expression.
			 || *eap->cmd == '"'
			    // '$"string"->func()' is an expression.
			    // "$'string'->func()" is an expression.
			 || (eap->cmd[0] == '$'
			     && (eap->cmd[1] == '\'' || eap->cmd[1] == '"'))
			    // '0z1234->func()' is an expression.
			 || (eap->cmd[0] == '0' && eap->cmd[1] == 'z')
			    // "g:varname" is an expression.
			 || eap->cmd[1] == ':'
			    )
			    // "varname->func()" is an expression.
			: (*swp == '-' && swp[1] == '>')))
	    {
		නම් (*eap->cmd == '{' && ends_excmd(*skipwhite(eap->cmd + 1)))
		{
		    // "{" by itself is the start of a block.
		    eap->cmdidx = CMD_block;
		    ආපසු eap->cmd + 1;
		}
		eap->cmdidx = CMD_eval;
		ආපසු eap->cmd;
	    }

	    නම් ((p != eap->cmd && (
			    // "varname[]" is an expression.
			    *p == '['
			    // "varname.key" is an expression.
			 || (*p == '.'
				     && (ASCII_ISALPHA(p[1]) || p[1] == '_'))))
			// g:[key] is an expression
		    || STRNCMP(eap->cmd, "g:[", 3) == 0)
	    {
		char_u	*after = eap->cmd;

		// When followed by "=" or "+=" then it is an assignment.
		// Skip over the whole thing, it can be:
		//	name.member = val
		//	name[a : b] = val
		//	name[idx] = val
		//	name[idx].member = val
		//	etc.
		eap->cmdidx = CMD_eval;
		++emsg_off;
		නම් (skip_expr(&after, NULL) == OK)
		{
		    after = skipwhite(after);
		    නම් (*after == '=' || (*after != NUL && after[1] == '=')
					 || (after[0] == '.' && after[1] == '.'
							   && after[2] == '='))
			eap->cmdidx = CMD_var;
		}
		--emsg_off;
		ආපසු eap->cmd;
	    }

	    // "[...]->Method()" is a list expression, but "[a, b] = Func()" is
	    // an assignment.
	    // If there is no line break inside the "[...]" then "p" is
	    // advanced to after the "]" by to_name_const_end(): check if a "="
	    // follows.
	    // If "[...]" has a line break "p" still points at the "[" and it
	    // can't be an assignment.
	    නම් (*eap->cmd == '[')
	    {
		char_u	    *eq;

		p = to_name_const_end(eap->cmd);
		නම් (p == eap->cmd && *p == '[')
		{
		    පූර්ණ_සංඛ්‍යා count = 0;
		    පූර්ණ_සංඛ්‍යා	semicolon = FALSE;

		    p = skip_var_list(eap->cmd, TRUE, &count, &semicolon, TRUE);
		}
		eq = p;
		නම් (eq != NULL)
		{
		    eq = skipwhite(eq);
		    නම් (vim_strchr((char_u *)"+-*/%.", *eq) != NULL)
		    {
			නම් (eq[0] == '.' && eq[1] == '.')
			    ++eq;
			++eq;
		    }
		}
		නම් (p == NULL || p == eap->cmd || *eq != '=')
		{
		    eap->cmdidx = CMD_eval;
		    ආපසු eap->cmd;
		}
		නම් (p > eap->cmd && *eq == '=')
		{
		    eap->cmdidx = CMD_var;
		    ආපසු eap->cmd;
		}
	    }

	    // Recognize an assignment if we recognize the variable name:
	    // "g:var = expr"
	    // "@r = expr"
	    // "&opt = expr"
	    // "var = expr"  where "var" is a variable name or we are skipping
	    // (variable declaration might have been skipped).
	    // Not "redir => var" (when skipping).
	    oplen = assignment_len(skipwhite(p), &heredoc);
	    නම් (oplen > 0)
	    {
		නම් (((p - eap->cmd) > 2 && eap->cmd[1] == ':')
			|| *eap->cmd == '&'
			|| *eap->cmd == '$'
			|| *eap->cmd == '@'
			|| (eap->skip && IS_WHITE_OR_NUL(
						      *(skipwhite(p) + oplen)))
			|| lookup(eap->cmd, p - eap->cmd, TRUE, cctx) == OK)
		{
		    eap->cmdidx = CMD_var;
		    ආපසු eap->cmd;
		}
	    }

	    // Recognize trying to use a type for a w:, b:, t: or g: variable:
	    // "w:varname: number = 123".
	    නම් (eap->cmd[1] == ':' && *p == ':')
	    {
		eap->cmdidx = CMD_var;
		ආපසු eap->cmd;
	    }
	}

	// 1234->func() is a method call
	නම් (number_method(eap->cmd))
	{
	    eap->cmdidx = CMD_eval;
	    ආපසු eap->cmd;
	}

	// "g:", "s:" and "l:" are always assumed to be a variable, thus start
	// an expression.  A global/substitute/list command needs to use a
	// longer name.
	නම් (vim_strchr((char_u *)"gsl", *p) != NULL && p[1] == ':')
	{
	    eap->cmdidx = CMD_eval;
	    ආපසු eap->cmd;
	}

	// If it is an ID it might be a variable with an operator on the next
	// line, if the variable exists it can't be an Ex command.
	නම් (p > eap->cmd && ends_excmd(*skipwhite(p))
		&& (lookup(eap->cmd, p - eap->cmd, TRUE, cctx) == OK
		    || (ASCII_ISALPHA(eap->cmd[0]) && eap->cmd[1] == ':')))
	{
	    eap->cmdidx = CMD_eval;
	    ආපසු eap->cmd;
	}

	// Check for "++nr" and "--nr".
	නම් (p == eap->cmd && p[0] != NUL && p[0] == p[1]
						   && (*p == '+' || *p == '-'))
	{
	    eap->cmdidx = *p == '+' ? CMD_increment : CMD_decrement;
	    ආපසු eap->cmd + 2;
	}
    }
#endif

    /*
     * Isolate the command and search for it in the command table.
     */
    p = eap->cmd;
    නම් (one_letter_cmd(p, &eap->cmdidx))
    {
	++p;
	නම් (full != NULL)
	    *full = TRUE;
    }
    නැතහොත්
    {
	අතරතුර (ASCII_ISALPHA(*p))
	    ++p;
	// for python 3.x support ":py3", ":python3", ":py3file", etc.
	නම් (eap->cmd[0] == 'p' && eap->cmd[1] == 'y')
	{
	    අතරතුර (ASCII_ISALNUM(*p))
		++p;
	}
	නැතහොත් නම් (*p == '9' && STRNCMP("vim9", eap->cmd, 4) == 0)
	{
	    // include "9" for "vim9*" commands; "vim9cmd" and "vim9script".
	    ++p;
	    අතරතුර (ASCII_ISALPHA(*p))
		++p;
	}

	// check for non-alpha command
	නම් (p == eap->cmd && vim_strchr((char_u *)"@*!=><&~#}", *p) != NULL)
	    ++p;
	len = (පූර්ණ_සංඛ්‍යා)(p - eap->cmd);
	// The "d" command can directly be followed by 'l' or 'p' flag, when
	// not in Vim9 script.
	නම් (!vim9 && *eap->cmd == 'd' && (p[-1] == 'l' || p[-1] == 'p'))
	{
	    // Check for ":dl", ":dell", etc. to ":deletel": that's
	    // :delete with the 'l' flag.  Same for 'p'.
	    සඳහා (i = 0; i < len; ++i)
		නම් (eap->cmd[i] != ((char_u *)"delete")[i])
		    bha_nga;
	    නම් (i == len - 1)
	    {
		--len;
		නම් (p[-1] == 'l')
		    eap->flags |= EXFLAG_LIST;
		නැතහොත්
		    eap->flags |= EXFLAG_PRINT;
	    }
	}

	නම් (ASCII_ISLOWER(eap->cmd[0]))
	{
	    පූර්ණ_සංඛ්‍යා c1 = eap->cmd[0];
	    පූර්ණ_සංඛ්‍යා c2 = len == 1 ? NUL : eap->cmd[1];

	    නම් (command_count != (පූර්ණ_සංඛ්‍යා)CMD_SIZE)
	    {
		iemsg(e_command_table_needs_to_be_updated_run_make_cmdidxs);
		getout(1);
	    }

	    // Use a precomputed index for fast look-up in cmdnames[]
	    // taking into account the first 2 letters of eap->cmd.
	    eap->cmdidx = cmdidxs1[CharOrdLow(c1)];
	    නම් (ASCII_ISLOWER(c2))
		eap->cmdidx += cmdidxs2[CharOrdLow(c1)][CharOrdLow(c2)];
	}
	නැතහොත් නම් (ASCII_ISUPPER(eap->cmd[0]))
	    eap->cmdidx = CMD_Next;
	නැතහොත්
	    eap->cmdidx = CMD_bang;

	සඳහා ( ; (පූර්ණ_සංඛ්‍යා)eap->cmdidx < (පූර්ණ_සංඛ්‍යා)CMD_SIZE;
			       eap->cmdidx = (cmdidx_T)((පූර්ණ_සංඛ්‍යා)eap->cmdidx + 1))
	    නම් (STRNCMP(cmdnames[(පූර්ණ_සංඛ්‍යා)eap->cmdidx].cmd_name, (var_na *)eap->cmd,
							    (size_t)len) == 0)
	    {
#ifdef FEAT_EVAL
		නම් (full != NULL && cmdnames[eap->cmdidx].cmd_name[len] == NUL)
		    *full = TRUE;
#endif
		bha_nga;
	    }

	// :Print and :mode are not supported in Vim9 script.
	// Some commands cannot be shortened in Vim9 script.
	නම් (vim9 && eap->cmdidx != CMD_SIZE)
	{
	    නම් (eap->cmdidx == CMD_mode || eap->cmdidx == CMD_Print)
		eap->cmdidx = CMD_SIZE;
	    නැතහොත් නම් ((cmdnames[eap->cmdidx].cmd_argt & EX_WHOLE)
			  && len < (පූර්ණ_සංඛ්‍යා)cmdnames[eap->cmdidx].cmd_namelen)
	    {
		semsg(_(e_command_cannot_be_shortened_str), eap->cmd);
		eap->cmdidx = CMD_SIZE;
	    }
	}

	// Do not recognize ":*" as the star command unless '*' is in
	// 'cpoptions'.
	නම් (eap->cmdidx == CMD_star && vim_strchr(p_cpo, CPO_STAR) == NULL)
	    p = eap->cmd;

	// Look for a user defined command as a last resort.  Let ":Print" be
	// overruled by a user defined command.
	නම් ((eap->cmdidx == CMD_SIZE || eap->cmdidx == CMD_Print)
		&& *eap->cmd >= 'A' && *eap->cmd <= 'Z')
	{
	    // User defined commands may contain digits.
	    අතරතුර (ASCII_ISALNUM(*p))
		++p;
	    p = find_ucmd(eap, p, full, NULL, NULL);
	}
	නම් (p == NULL || p == eap->cmd)
	    eap->cmdidx = CMD_SIZE;
    }

    // ":fina" means ":finally" in legacy script, for backwards compatibility.
    නම් (eap->cmdidx == CMD_final && p - eap->cmd == 4 && !vim9)
	eap->cmdidx = CMD_finally;

     // Force ":ho" to be unresolved.  Without this, find_ex_command()
     // matches it to CMD_horizontal (the only "ho*" entry), which makes
     // fullcommand("ho") return "horizontal" even though ":ho" cannot be
     // used as the modifier (cmdmods[] requires 3 chars, "hor").
    නම් (eap->cmdidx == CMD_horizontal && p - eap->cmd == 2)
	eap->cmdidx = CMD_SIZE;

#ifdef FEAT_EVAL
    නම් (eap->cmdidx < CMD_SIZE
	    && vim9
	    && !IS_WHITE_NL_OR_NUL(*p) && *p != '!' && *p != '|'
	    && (eap->cmdidx < 0 ||
		(cmdnames[eap->cmdidx].cmd_argt & EX_NONWHITE_OK) == 0))
    {
	// A name that goes on with an underscore is not this command with an
	// argument: "ch_log" is not ":change".  Leave it to be reported as an
	// invalid command.
	නම් (*p != '_')
	{
	    char_u *cmd = vim_strnsave(eap->cmd, p - eap->cmd);

	    semsg(_(e_command_str_not_followed_by_white_space_str), cmd,
								    eap->cmd);
	    vim_free(cmd);
	}
	eap->cmdidx = CMD_SIZE;
    }
#endif

    ආපසු p;
}

#නම් defined(FEAT_EVAL)
typedef sa_nracanaa
{
    var_na	*name;
    පූර්ණ_සංඛ්‍යා		minlen;
    පූර්ණ_සංඛ්‍යා		has_count;  // :123verbose  :3tab
} cmdmod_info_T;

static cmdmod_info_T cmdmod_info_tab[] = {
    {"aboveleft", 3, FALSE},
    {"belowright", 3, FALSE},
    {"botright", 2, FALSE},
    {"browse", 3, FALSE},
    {"confirm", 4, FALSE},
    {"filter", 4, FALSE},
    {"hide", 3, FALSE},
    {"horizontal", 3, FALSE},
    {"keepalt", 5, FALSE},
    {"keepjumps", 5, FALSE},
    {"keepmarks", 3, FALSE},
    {"keeppatterns", 5, FALSE},
    {"leftabove", 5, FALSE},
    {"legacy", 3, FALSE},
    {"lockmarks", 3, FALSE},
    {"noautocmd", 3, FALSE},
    {"noswapfile", 3, FALSE},
    {"rightbelow", 6, FALSE},
    {"sandbox", 3, FALSE},
    {"silent", 3, FALSE},
    {"tab", 3, TRUE},
    {"topleft", 2, FALSE},
    {"unsilent", 3, FALSE},
    {"verbose", 4, TRUE},
    {"vertical", 4, FALSE},
    {"vim9cmd", 4, FALSE}
};	// cmdmod_info_tab

/*
 * Return length of a command modifier (including optional count).
 * Return zero when it's not a modifier.
 */
    පූර්ණ_සංඛ්‍යා
modifier_len(char_u *cmd)
{
    පූර්ණ_සංඛ්‍යා		i, j;
    char_u	*p = cmd;

    නම් (VIM_ISDIGIT(*cmd))
	p = skipwhite(skipdigits(cmd + 1));
    සඳහා (i = 0; i < (පූර්ණ_සංඛ්‍යා)ARRAY_LENGTH(cmdmod_info_tab); ++i)
    {
	// cmdmod_info_tab[] is sorted by name: once the first letter is past
	// the command's first letter no later entry can match.
	නම් (cmdmod_info_tab[i].name[0] > *p)
	    bha_nga;
	සඳහා (j = 0; p[j] != NUL; ++j)
	    නම් (p[j] != cmdmod_info_tab[i].name[j])
		bha_nga;
	නම් (!ASCII_ISALPHA(p[j]) && j >= cmdmod_info_tab[i].minlen
					&& (p == cmd || cmdmod_info_tab[i].has_count))
	    ආපසු j + (පූර්ණ_සංඛ්‍යා)(p - cmd);
    }
    ආපසු 0;
}

/*
 * Return > 0 if an Ex command "name" exists.
 * Return 2 if there is an exact match.
 * Return 3 if there is an ambiguous match.
 */
    පූර්ණ_සංඛ්‍යා
cmd_exists(char_u *name)
{
    exarg_T	ea;
    පූර්ණ_සංඛ්‍යා		full = FALSE;
    පූර්ණ_සංඛ්‍යා		i;
    පූර්ණ_සංඛ්‍යා		j;
    char_u	*p;

    // Check command modifiers.
    සඳහා (i = 0; i < (පූර්ණ_සංඛ්‍යා)ARRAY_LENGTH(cmdmod_info_tab); ++i)
    {
	සඳහා (j = 0; name[j] != NUL; ++j)
	    නම් (name[j] != cmdmod_info_tab[i].name[j])
		bha_nga;
	නම් (name[j] == NUL && j >= cmdmod_info_tab[i].minlen)
	    ආපසු (cmdmod_info_tab[i].name[j] == NUL ? 2 : 1);
    }

    // Check built-in commands and user defined commands.
    // For ":2match" and ":3match" we need to skip the number.
    ea.cmd = (*name == '2' || *name == '3') ? name + 1 : name;
    ea.cmdidx = (cmdidx_T)0;
    ea.flags = 0;
    p = find_ex_command(&ea, &full, NULL, NULL);
    නම් (p == NULL)
	ආපසු 3;
    නම් (vim_isdigit(*name) && ea.cmdidx != CMD_match)
	ආපසු 0;
    නම් (*skipwhite(p) != NUL)
	ආපසු 0;	// trailing garbage
    ආපසු (ea.cmdidx == CMD_SIZE ? 0 : (full ? 2 : 1));
}

/*
 * Add the attributes of an Ex command to "d" in the terms of ":command":
 * "argt" are the EX_ flags, "addr_type" the address type and "def" the
 * default for a range or count, -1 when there is none.
 */
    void
ex_command_attrs(dict_T *d, long_u argt, cmd_addr_T addr_type, long def)
{
    static var_na *addr_names[] = {
	"lines", "windows", "arguments", "loaded_buffers", "buffers", "tabs",
	"tabs_relative", "quickfix_valid", "quickfix", "unsigned", "other",
	"none"
    };
    var_na	*nargs;

    නම් (!(argt & EX_EXTRA))
	nargs = "0";
    නැතහොත් නම් (!(argt & EX_NOSPC))
	nargs = (argt & EX_NEEDARG) ? "+" : "*";
    නැතහොත් නම් (!(argt & EX_NEEDARG))
	nargs = "?";
    නැතහොත්
	nargs = (argt & EX_ARGSPACE) ? "_" : "1";
    dict_add_string(d, "nargs", (char_u *)nargs);

    නම් (!(argt & EX_RANGE))
	dict_add_bool(d, "range", FALSE);
    නැතහොත් නම් (argt & EX_DFLALL)
	dict_add_string(d, "range", (char_u *)"%");
    නැතහොත් නම් (def >= 0 && !(argt & EX_COUNT))
	dict_add_number(d, "range", def);
    නැතහොත්
	dict_add_string(d, "range", (char_u *)".");
    නම් (!(argt & EX_COUNT))
	dict_add_bool(d, "count", FALSE);
    නැතහොත්
	dict_add_number(d, "count", def < 0 ? 0 : def);
    dict_add_bool(d, "bang", (argt & EX_BANG) != 0);
    dict_add_bool(d, "bar", (argt & EX_TRLBAR) != 0);
    dict_add_bool(d, "register", (argt & EX_REGSTR) != 0);
    dict_add_string(d, "addr", (char_u *)addr_names[addr_type]);
}

/*
 * Add what getinfo() reports for the Ex command "name" to "d".  "vim9" is TRUE
 * to find the command as in Vim9 script, FALSE as in legacy script, -1 for the
 * context of the caller.
 * Returns FAIL when there is no such command or the abbreviation is
 * ambiguous.
 */
    පූර්ණ_සංඛ්‍යා
ex_command_info(char_u *name, පූර්ණ_සංඛ්‍යා vim9, dict_T *d)
{
    exarg_T	ea;
    char_u	*p;
    පූර්ණ_සංඛ්‍යා		i;
    පූර්ණ_සංඛ්‍යා		j;
    පූර්ණ_සංඛ්‍යා		save_cmod_flags = cmdmod.cmod_flags;
    පූර්ණ_සංඛ්‍යා		ret = OK;

    නම් (vim9 != -1)
    {
	cmdmod.cmod_flags &= ~(CMOD_VIM9CMD | CMOD_LEGACY);
	cmdmod.cmod_flags |= vim9 ? CMOD_VIM9CMD : CMOD_LEGACY;
    }

    // An abbreviation of a command modifier, like ":bo" for ":botright".
    සඳහා (i = 0; i < (පූර්ණ_සංඛ්‍යා)ARRAY_LENGTH(cmdmod_info_tab); ++i)
    {
	සඳහා (j = 0; name[j] != NUL; ++j)
	    නම් (name[j] != cmdmod_info_tab[i].name[j])
		bha_nga;
	නම් (name[j] == NUL && j >= cmdmod_info_tab[i].minlen)
	{
	    name = (char_u *)cmdmod_info_tab[i].name;
	    bha_nga;
	}
    }

    // For ":2match" and ":3match" we need to skip the number.
    ea.cmd = (*name == '2' || *name == '3') ? name + 1 : name;
    ea.cmdidx = (cmdidx_T)0;
    ea.flags = 0;
    ea.addr_count = 0;
    // Don't complain about using "en" or ":let" in Vim9 script.
    ++emsg_silent;
    p = find_ex_command(&ea, NULL, NULL, NULL);
    නම් (p == NULL || ea.cmdidx == CMD_SIZE
	    || (vim_isdigit(*name) && ea.cmdidx != CMD_match)
	    || *skipwhite(p) != NUL
	    || not_in_vim9(&ea, true) == FAIL)
	ret = FAIL;
    --emsg_silent;

    නම් (ret == FAIL)
	;
    නැතහොත් නම් (IS_USER_CMDIDX(ea.cmdidx))
	ret = user_command_info(ea.useridx, ea.cmdidx, d);
    නැතහොත්
    {
	පූර්ණ_සංඛ්‍යා	notimpl = cmdnames[ea.cmdidx].cmd_func == ex_ni
# ifdef HAVE_EX_SCRIPT_NI
			    || cmdnames[ea.cmdidx].cmd_func == ex_script_ni
# endif
			    ;

	dict_add_string(d, "name", cmdnames[ea.cmdidx].cmd_name);
	dict_add_string(d, "kind", (char_u *)(
		    cmdnames[ea.cmdidx].cmd_func == ex_wrongmodifier
						    ? "modifier" : "builtin"));
	dict_add_bool(d, "available", !notimpl);
	ex_command_attrs(d, cmdnames[ea.cmdidx].cmd_argt,
				       cmdnames[ea.cmdidx].cmd_addr_type, -1);
    }
    cmdmod.cmod_flags = save_cmod_flags;
    ආපසු ret;
}

/*
 * "fullcommand" function
 */
    void
f_fullcommand(typval_T *argvars, typval_T *rettv)
{
    exarg_T	ea;
    char_u	*name;
    char_u	*p;
    පූර්ණ_සංඛ්‍යා		vim9script = in_vim9script();
    පූර්ණ_සංඛ්‍යා		save_cmod_flags = cmdmod.cmod_flags;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = NULL;

    නම් (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL))
	ආපසු;

    name = tv_get_string(&argvars[0]);
    නම් (name == NULL)
	ආපසු;

    නම් (argvars[1].v_type != VAR_UNKNOWN)
    {
	vim9script = tv_get_bool(&argvars[1]);
	cmdmod.cmod_flags &= ~(CMOD_VIM9CMD | CMOD_LEGACY);
	cmdmod.cmod_flags |= vim9script ? CMOD_VIM9CMD : CMOD_LEGACY;
    }

    අතරතුර (*name == ':')
	name++;
    name = skip_range(name, TRUE, NULL);

    ea.cmd = (*name == '2' || *name == '3') ? name + 1 : name;
    ea.cmdidx = (cmdidx_T)0;
    ea.addr_count = 0;
    ++emsg_silent;  // don't complain about using "en" in Vim9 script
    p = find_ex_command(&ea, NULL, NULL, NULL);
    --emsg_silent;
    නම් (p == NULL || ea.cmdidx == CMD_SIZE)
	goto theend;

    නම් (vim9script && not_in_vim9(&ea, true) == FAIL)
	goto theend;

    rettv->vval.v_string = vim_strsave(IS_USER_CMDIDX(ea.cmdidx)
				 ? get_user_command_name(ea.useridx, ea.cmdidx)
				 : cmdnames[ea.cmdidx].cmd_name);
theend:
    cmdmod.cmod_flags = save_cmod_flags;
}
#endif

    cmdidx_T
excmd_get_cmdidx(char_u *cmd, පූර්ණ_සංඛ්‍යා len)
{
    cmdidx_T idx;

    නම් (!one_letter_cmd(cmd, &idx))
	සඳහා (idx = (cmdidx_T)0; (පූර්ණ_සංඛ්‍යා)idx < (පූර්ණ_සංඛ්‍යා)CMD_SIZE;
		idx = (cmdidx_T)((පූර්ණ_සංඛ්‍යා)idx + 1))
	    නම් (STRNCMP(cmdnames[(පූර්ණ_සංඛ්‍යා)idx].cmd_name, cmd, (size_t)len) == 0)
		bha_nga;

    ආපසු idx;
}

    long
excmd_get_argt(cmdidx_T idx)
{
    ආපසු (long)cmdnames[(පූර්ණ_සංඛ්‍යා)idx].cmd_argt;
}

/*
 * Skip a range specifier of the form: addr [,addr] [;addr] ..
 *
 * Backslashed delimiters after / or ? will be skipped, and commands will
 * not be expanded between /'s and ?'s or after "'".
 *
 * Also skip white space and ":" characters after the range.
 * Returns the "cmd" pointer advanced to beyond the range.
 */
    char_u *
skip_range(
    char_u	*cmd_start,
    පූර්ණ_සංඛ්‍යා		skip_star,	// skip "*" used for Visual range
    පූර්ණ_සංඛ්‍යා		*ctx)		// pointer to xp_context or NULL
{
    char_u	*cmd = cmd_start;
    unsigned	delim;

    අතරතුර (vim_strchr((char_u *)" \t0123456789.$%'/?-+,;\\", *cmd) != NULL)
    {
	නම් (*cmd == '\\')
	{
	    නම් (cmd[1] == '?' || cmd[1] == '/' || cmd[1] == '&')
		++cmd;
	    නැතහොත්
		bha_nga;
	}
	නැතහොත් නම් (*cmd == '\'')
	{
	    char_u *p = cmd;

	    // a quote is only valid at the start or after a separator
	    අතරතුර (p > cmd_start)
	    {
		--p;
		නම් (!VIM_ISWHITE(*p))
		    bha_nga;
	    }
	    නම් (cmd > cmd_start && !VIM_ISWHITE(*p) && *p != ',' && *p != ';')
		bha_nga;
	    නම් (*++cmd == NUL && ctx != NULL)
		*ctx = EXPAND_NOTHING;
	}
	නැතහොත් නම් (*cmd == '/' || *cmd == '?')
	{
	    delim = *cmd++;
	    අතරතුර (*cmd != NUL && *cmd != delim)
		නම් (*cmd++ == '\\' && *cmd != NUL)
		    ++cmd;
	    නම් (*cmd == NUL && ctx != NULL)
		*ctx = EXPAND_NOTHING;
	}
	නම් (*cmd != NUL)
	    ++cmd;
    }

    // Skip ":" and white space.
    අතරතුර (*cmd == ':')
	cmd = skipwhite(cmd + 1);

    // Skip "*" used for Visual range.
    නම් (skip_star && *cmd == '*' && vim_strchr(p_cpo, CPO_STAR) == NULL)
	cmd = skipwhite(cmd + 1);

    ආපසු cmd;
}

    static void
addr_error(cmd_addr_T addr_type)
{
    නම් (addr_type == ADDR_NONE)
	emsg(_(e_no_range_allowed));
    නැතහොත්
	emsg(_(e_invalid_range));
}

/*
 * Return the default address for an address type.
 */
    static linenr_T
default_address(exarg_T *eap)
{
    linenr_T lnum = 0;

    switch (eap->addr_type)
    {
	pak_sa ADDR_LINES:
	pak_sa ADDR_OTHER:
	    // Default is the cursor line number.  Avoid using an invalid
	    // line number though.
	    නම් (curwin->w_cursor.lnum > curbuf->b_ml.ml_line_count)
		lnum = curbuf->b_ml.ml_line_count;
	    නැතහොත්
		lnum = curwin->w_cursor.lnum;
	    bha_nga;
	pak_sa ADDR_WINDOWS:
	    lnum = CURRENT_WIN_NR;
	    bha_nga;
	pak_sa ADDR_ARGUMENTS:
	    lnum = curwin->w_arg_idx + 1;
	    නම් (lnum > ARGCOUNT)
		lnum = ARGCOUNT;
	    bha_nga;
	pak_sa ADDR_LOADED_BUFFERS:
	pak_sa ADDR_BUFFERS:
	    lnum = curbuf->b_fnum;
	    bha_nga;
	pak_sa ADDR_TABS:
	    lnum = CURRENT_TAB_NR;
	    bha_nga;
	pak_sa ADDR_TABS_RELATIVE:
	pak_sa ADDR_UNSIGNED:
	    lnum = 1;
	    bha_nga;
	pak_sa ADDR_QUICKFIX:
#ifdef FEAT_QUICKFIX
	    lnum = qf_get_cur_idx(eap);
#endif
	    bha_nga;
	pak_sa ADDR_QUICKFIX_VALID:
#ifdef FEAT_QUICKFIX
	    lnum = qf_get_cur_valid_idx(eap);
#endif
	    bha_nga;
	pak_sa ADDR_NONE:
	    // Will give an error later if a range is found.
	    bha_nga;
    }
    ආපසු lnum;
}

/*
 * Get a single EX address.
 *
 * Set ptr to the next character after the part that was interpreted.
 * Set ptr to NULL when an error is encountered.
 * This may set the last used search pattern.
 *
 * Return MAXLNUM when no Ex address was found.
 */
    linenr_T
get_address(
    exarg_T	*eap UNUSED,
    char_u	**ptr,
    cmd_addr_T	addr_type,
    පූර්ණ_සංඛ්‍යා		skip,		// only skip the address, don't use it
    පූර්ණ_සංඛ්‍යා		silent,		// no errors or side effects
    පූර්ණ_සංඛ්‍යා		to_other_file,  // flag: may jump to other file
    පූර්ණ_සංඛ්‍යා		address_count UNUSED) // 1 for first address, >1 after comma
{
    පූර්ණ_සංඛ්‍යා		c;
    පූර්ණ_සංඛ්‍යා		i;
    long	n;
    char_u	*cmd;
    pos_T	pos;
    pos_T	*fp;
    linenr_T	lnum;
    buf_T	*buf;

    cmd = skipwhite(*ptr);
    lnum = MAXLNUM;
    do
    {
	switch (*cmd)
	{
	    pak_sa '.':			    // '.' - Cursor position
		++cmd;
		switch (addr_type)
		{
		    pak_sa ADDR_LINES:
		    pak_sa ADDR_OTHER:
			lnum = curwin->w_cursor.lnum;
			bha_nga;
		    pak_sa ADDR_WINDOWS:
			lnum = CURRENT_WIN_NR;
			bha_nga;
		    pak_sa ADDR_ARGUMENTS:
			lnum = curwin->w_arg_idx + 1;
			bha_nga;
		    pak_sa ADDR_LOADED_BUFFERS:
		    pak_sa ADDR_BUFFERS:
			lnum = curbuf->b_fnum;
			bha_nga;
		    pak_sa ADDR_TABS:
			lnum = CURRENT_TAB_NR;
			bha_nga;
		    pak_sa ADDR_NONE:
		    pak_sa ADDR_TABS_RELATIVE:
		    pak_sa ADDR_UNSIGNED:
			addr_error(addr_type);
			cmd = NULL;
			goto error;
			bha_nga;
		    pak_sa ADDR_QUICKFIX:
#ifdef FEAT_QUICKFIX
			lnum = qf_get_cur_idx(eap);
#endif
			bha_nga;
		    pak_sa ADDR_QUICKFIX_VALID:
#ifdef FEAT_QUICKFIX
			lnum = qf_get_cur_valid_idx(eap);
#endif
			bha_nga;
		}
		bha_nga;

	    pak_sa '$':			    // '$' - last line
		++cmd;
		switch (addr_type)
		{
		    pak_sa ADDR_LINES:
		    pak_sa ADDR_OTHER:
			lnum = curbuf->b_ml.ml_line_count;
			bha_nga;
		    pak_sa ADDR_WINDOWS:
			lnum = LAST_WIN_NR;
			bha_nga;
		    pak_sa ADDR_ARGUMENTS:
			lnum = ARGCOUNT;
			bha_nga;
		    pak_sa ADDR_LOADED_BUFFERS:
			buf = lastbuf;
			අතරතුර (buf->b_ml.ml_mfp == NULL)
			{
			    නම් (buf->b_prev == NULL)
				bha_nga;
			    buf = buf->b_prev;
			}
			lnum = buf->b_fnum;
			bha_nga;
		    pak_sa ADDR_BUFFERS:
			lnum = lastbuf->b_fnum;
			bha_nga;
		    pak_sa ADDR_TABS:
			lnum = LAST_TAB_NR;
			bha_nga;
		    pak_sa ADDR_NONE:
		    pak_sa ADDR_TABS_RELATIVE:
		    pak_sa ADDR_UNSIGNED:
			addr_error(addr_type);
			cmd = NULL;
			goto error;
			bha_nga;
		    pak_sa ADDR_QUICKFIX:
#ifdef FEAT_QUICKFIX
			lnum = qf_get_size(eap);
			නම් (lnum == 0)
			    lnum = 1;
#endif
			bha_nga;
		    pak_sa ADDR_QUICKFIX_VALID:
#ifdef FEAT_QUICKFIX
			lnum = qf_get_valid_size(eap);
			නම් (lnum == 0)
			    lnum = 1;
#endif
			bha_nga;
		}
		bha_nga;

	    pak_sa '\'':			    // ''' - mark
		නම් (*++cmd == NUL)
		{
		    cmd = NULL;
		    goto error;
		}
		නම් (addr_type != ADDR_LINES)
		{
		    addr_error(addr_type);
		    cmd = NULL;
		    goto error;
		}
		නම් (skip)
		    ++cmd;
		නැතහොත්
		{
		    // Only accept a mark in another file when it is
		    // used by itself: ":'M".
		    fp = getmark(*cmd, to_other_file && cmd[1] == NUL);
		    ++cmd;
		    නම් (fp == (pos_T *)-1)
			// Jumped to another file.
			lnum = curwin->w_cursor.lnum;
		    නැතහොත්
		    {
			නම් (check_mark(fp) == FAIL)
			{
			    cmd = NULL;
			    goto error;
			}
			lnum = fp->lnum;
		    }
		}
		bha_nga;

	    pak_sa '/':
	    pak_sa '?':			// '/' or '?' - search
		c = *cmd++;
		නම් (addr_type != ADDR_LINES)
		{
		    addr_error(addr_type);
		    cmd = NULL;
		    goto error;
		}
		නම් (skip)	// skip "/pat/"
		{
		    cmd = skip_regexp(cmd, c, magic_isset());
		    නම් (*cmd == c)
			++cmd;
		}
		නැතහොත්
		{
		    පූර්ණ_සංඛ්‍යා flags;

		    pos = curwin->w_cursor; // save curwin->w_cursor

		    // When '/' or '?' follows another address, start from
		    // there.
		    නම් (lnum > 0 && lnum != MAXLNUM)
			curwin->w_cursor.lnum =
				lnum > curbuf->b_ml.ml_line_count
					   ? curbuf->b_ml.ml_line_count : lnum;

		    // Start a forward search at the end of the line (unless
		    // before the first line).
		    // Start a backward search at the start of the line.
		    // This makes sure we never match in the current
		    // line, and can match anywhere in the
		    // next/previous line.
		    නම් (c == '/' && curwin->w_cursor.lnum > 0)
			curwin->w_cursor.col = MAXCOL;
		    නැතහොත්
			curwin->w_cursor.col = 0;
		    searchcmdlen = 0;
		    flags = silent ? SEARCH_KEEP : SEARCH_HIS | SEARCH_MSG;
		    නම් (!do_search(NULL, c, c, cmd, STRLEN(cmd), 1L, flags, NULL))
		    {
			curwin->w_cursor = pos;
			cmd = NULL;
			goto error;
		    }
		    lnum = curwin->w_cursor.lnum;
		    curwin->w_cursor = pos;
		    // adjust command string pointer
		    cmd += searchcmdlen;
		}
		bha_nga;

	    pak_sa '\\':		    // "\?", "\/" or "\&", repeat search
		++cmd;
		නම් (addr_type != ADDR_LINES)
		{
		    addr_error(addr_type);
		    cmd = NULL;
		    goto error;
		}
		නම් (*cmd == '&')
		    i = RE_SUBST;
		නැතහොත් නම් (*cmd == '?' || *cmd == '/')
		    i = RE_SEARCH;
		නැතහොත්
		{
		    emsg(_(e_backslash_should_be_followed_by));
		    cmd = NULL;
		    goto error;
		}

		නම් (!skip)
		{
		    /*
		     * When search follows another address, start from
		     * there.
		     */
		    නම් (lnum != MAXLNUM)
			pos.lnum = lnum;
		    නැතහොත්
			pos.lnum = curwin->w_cursor.lnum;

		    /*
		     * Start the search just like for the above
		     * do_search().
		     */
		    නම් (*cmd != '?')
			pos.col = MAXCOL;
		    නැතහොත්
			pos.col = 0;
		    pos.coladd = 0;
		    නම් (searchit(curwin, curbuf, &pos, NULL,
				*cmd == '?' ? BACKWARD : FORWARD,
				(char_u *)"", 0, 1L, SEARCH_MSG, i, NULL) != FAIL)
			lnum = pos.lnum;
		    නැතහොත්
		    {
			cmd = NULL;
			goto error;
		    }
		}
		++cmd;
		bha_nga;

	    default:
		නම් (VIM_ISDIGIT(*cmd))	// absolute line number
		    lnum = getdigits(&cmd);
	}

	සඳහා (;;)
	{
	    cmd = skipwhite(cmd);
	    නම් (*cmd != '-' && *cmd != '+' && !VIM_ISDIGIT(*cmd))
		bha_nga;

	    නම් (lnum == MAXLNUM)
	    {
		switch (addr_type)
		{
		    pak_sa ADDR_LINES:
		    pak_sa ADDR_OTHER:
			// "+1" is same as ".+1"
			lnum = curwin->w_cursor.lnum;
			bha_nga;
		    pak_sa ADDR_WINDOWS:
			lnum = CURRENT_WIN_NR;
			bha_nga;
		    pak_sa ADDR_ARGUMENTS:
			lnum = curwin->w_arg_idx + 1;
			bha_nga;
		    pak_sa ADDR_LOADED_BUFFERS:
		    pak_sa ADDR_BUFFERS:
			lnum = curbuf->b_fnum;
			bha_nga;
		    pak_sa ADDR_TABS:
			lnum = CURRENT_TAB_NR;
			bha_nga;
		    pak_sa ADDR_TABS_RELATIVE:
			lnum = 1;
			bha_nga;
		    pak_sa ADDR_QUICKFIX:
#ifdef FEAT_QUICKFIX
			lnum = qf_get_cur_idx(eap);
#endif
			bha_nga;
		    pak_sa ADDR_QUICKFIX_VALID:
#ifdef FEAT_QUICKFIX
			lnum = qf_get_cur_valid_idx(eap);
#endif
			bha_nga;
		    pak_sa ADDR_NONE:
		    pak_sa ADDR_UNSIGNED:
			lnum = 0;
			bha_nga;
		}
	    }

	    නම් (VIM_ISDIGIT(*cmd))
		i = '+';		// "number" is same as "+number"
	    නැතහොත්
		i = *cmd++;
	    නම් (!VIM_ISDIGIT(*cmd))	// '+' is '+1'
		n = 1;
	    නැතහොත්
	    {
		// "number", "+number" or "-number"
		n = getdigits(&cmd);
		නම් (n == MAXLNUM)
		{
		    emsg(_(e_line_number_out_of_range));
		    cmd = NULL;
		    goto error;
		}
	    }

	    නම් (addr_type == ADDR_TABS_RELATIVE)
	    {
		emsg(_(e_invalid_range));
		cmd = NULL;
		goto error;
	    }
	    නැතහොත් නම් (addr_type == ADDR_LOADED_BUFFERS
		    || addr_type == ADDR_BUFFERS)
		lnum = compute_buffer_local_count(
				    addr_type, lnum, (i == '-') ? -1 * n : n);
	    නැතහොත්
	    {
#ifdef FEAT_FOLDING
		// Relative line addressing: need to adjust for lines in a
		// closed fold after the first address.
		නම් (addr_type == ADDR_LINES && (i == '-' || i == '+')
							 && address_count >= 2)
		    (void)hasFolding(lnum, NULL, &lnum);
#endif
		නම් (i == '-')
		    lnum -= n;
		නැතහොත්
		{
		    නම් (lnum >= 0 && n >= LONG_MAX - lnum)
		    {
			emsg(_(e_line_number_out_of_range));
			cmd = NULL;
			goto error;
		    }
		    lnum += n;
		}
	    }
	}
    } අතරතුර (*cmd == '/' || *cmd == '?');

error:
    *ptr = cmd;
    ආපසු lnum;
}

/*
 * Set eap->line1 and eap->line2 to the whole range.
 * Used for commands with the EX_DFLALL flag and no range given.
 */
    static void
address_default_all(exarg_T *eap)
{
    eap->line1 = 1;
    switch (eap->addr_type)
    {
	pak_sa ADDR_LINES:
	pak_sa ADDR_OTHER:
	    eap->line2 = curbuf->b_ml.ml_line_count;
	    bha_nga;
	pak_sa ADDR_LOADED_BUFFERS:
	    {
		buf_T *buf = firstbuf;

		අතරතුර (buf->b_next != NULL && buf->b_ml.ml_mfp == NULL)
		    buf = buf->b_next;
		eap->line1 = buf->b_fnum;
		buf = lastbuf;
		අතරතුර (buf->b_prev != NULL && buf->b_ml.ml_mfp == NULL)
		    buf = buf->b_prev;
		eap->line2 = buf->b_fnum;
	    }
	    bha_nga;
	pak_sa ADDR_BUFFERS:
	    eap->line1 = firstbuf->b_fnum;
	    eap->line2 = lastbuf->b_fnum;
	    bha_nga;
	pak_sa ADDR_WINDOWS:
	    eap->line2 = LAST_WIN_NR;
	    bha_nga;
	pak_sa ADDR_TABS:
	    eap->line2 = LAST_TAB_NR;
	    bha_nga;
	pak_sa ADDR_TABS_RELATIVE:
	    eap->line2 = 1;
	    bha_nga;
	pak_sa ADDR_ARGUMENTS:
	    නම් (ARGCOUNT == 0)
		eap->line1 = eap->line2 = 0;
	    නැතහොත්
		eap->line2 = ARGCOUNT;
	    bha_nga;
	pak_sa ADDR_QUICKFIX_VALID:
#ifdef FEAT_QUICKFIX
	    eap->line2 = qf_get_valid_size(eap);
	    නම් (eap->line2 == 0)
		eap->line2 = 1;
#endif
	    bha_nga;
	pak_sa ADDR_NONE:
	pak_sa ADDR_UNSIGNED:
	pak_sa ADDR_QUICKFIX:
	    iemsg("Cannot use EX_DFLALL with ADDR_NONE, ADDR_UNSIGNED or ADDR_QUICKFIX");
	    bha_nga;
    }
}


/*
 * Get flags from an Ex command argument.
 */
    static void
get_flags(exarg_T *eap)
{
    අතරතුර (vim_strchr((char_u *)"lp#", *eap->arg) != NULL)
    {
	නම් (*eap->arg == 'l')
	    eap->flags |= EXFLAG_LIST;
	නැතහොත් නම් (*eap->arg == 'p')
	    eap->flags |= EXFLAG_PRINT;
	නැතහොත්
	    eap->flags |= EXFLAG_NR;
	eap->arg = skipwhite(eap->arg + 1);
    }
}

/*
 * Function called for command which is Not Implemented.  NI!
 */
    void
ex_ni(exarg_T *eap)
{
    නම් (!eap->skip)
	eap->errmsg =
		_(e_sorry_command_is_not_available_in_this_version);
}

#ifdef HAVE_EX_SCRIPT_NI
/*
 * Function called for script command which is Not Implemented.  NI!
 * Skips over ":perl <<EOF" constructs.
 */
    static void
ex_script_ni(exarg_T *eap)
{
    නම් (!eap->skip)
	ex_ni(eap);
    නැතහොත්
	vim_free(script_get(eap, eap->arg));
}
#endif

/*
 * Check range in Ex command for validity.
 * Return NULL when valid, error message when invalid.
 */
    static var_na *
invalid_range(exarg_T *eap)
{
    buf_T	*buf;

    නම් (       eap->line1 < 0
	    || eap->line2 < 0
	    || eap->line1 > eap->line2)
	ආපසු _(e_invalid_range);

    නම් (eap->argt & EX_RANGE)
    {
	switch (eap->addr_type)
	{
	    pak_sa ADDR_LINES:
		නම් (eap->line2 > curbuf->b_ml.ml_line_count
#ifdef FEAT_DIFF
			    + (eap->cmdidx == CMD_diffget ||
				eap->cmdidx == CMD_diffput)
#endif
		   )
		    ආපසු _(e_invalid_range);
		bha_nga;
	    pak_sa ADDR_ARGUMENTS:
		// add 1 if ARGCOUNT is 0
		නම් (eap->line2 > ARGCOUNT + (!ARGCOUNT))
		    ආපසු _(e_invalid_range);
		bha_nga;
	    pak_sa ADDR_BUFFERS:
		// Only a boundary check, not whether the buffers actually
		// exist.
		නම් (eap->line1 < 1 || eap->line2 > get_highest_fnum())
		    ආපසු _(e_invalid_range);
		bha_nga;
	    pak_sa ADDR_LOADED_BUFFERS:
		buf = firstbuf;
		අතරතුර (buf->b_ml.ml_mfp == NULL)
		{
		    නම් (buf->b_next == NULL)
			ආපසු _(e_invalid_range);
		    buf = buf->b_next;
		}
		නම් (eap->line1 < buf->b_fnum)
		    ආපසු _(e_invalid_range);
		buf = lastbuf;
		අතරතුර (buf->b_ml.ml_mfp == NULL)
		{
		    නම් (buf->b_prev == NULL)
			ආපසු _(e_invalid_range);
		    buf = buf->b_prev;
		}
		නම් (eap->line2 > buf->b_fnum)
		    ආපසු _(e_invalid_range);
		bha_nga;
	    pak_sa ADDR_WINDOWS:
		නම් (eap->line2 > LAST_WIN_NR)
		    ආපසු _(e_invalid_range);
		bha_nga;
	    pak_sa ADDR_TABS:
		නම් (eap->line2 > LAST_TAB_NR)
		    ආපසු _(e_invalid_range);
		bha_nga;
	    pak_sa ADDR_TABS_RELATIVE:
	    pak_sa ADDR_OTHER:
		// Any range is OK.
		bha_nga;
	    pak_sa ADDR_QUICKFIX:
#ifdef FEAT_QUICKFIX
		// No error for value that is too big, will use the last entry.
		නම් (eap->line2 <= 0)
		{
		    නම් (eap->addr_count == 0)
			ආපසු _(e_no_errors);
		    ආපසු _(e_invalid_range);
		}
#endif
		bha_nga;
	    pak_sa ADDR_QUICKFIX_VALID:
#ifdef FEAT_QUICKFIX
		නම් ((eap->line2 != 1 && eap->line2 > qf_get_valid_size(eap))
			|| eap->line2 < 0)
		    ආපසු _(e_invalid_range);
#endif
		bha_nga;
	    pak_sa ADDR_UNSIGNED:
	    pak_sa ADDR_NONE:
		// Will give an error elsewhere.
		bha_nga;
	}
    }
    ආපසු NULL;
}

/*
 * Correct the range for zero line number, if required.
 */
    static void
correct_range(exarg_T *eap)
{
    නම් (!(eap->argt & EX_ZEROR))	    // zero in range not allowed
    {
	නම් (eap->line1 == 0)
	    eap->line1 = 1;
	නම් (eap->line2 == 0)
	    eap->line2 = 1;
    }
}

#ifdef FEAT_QUICKFIX
/*
 * For a ":vimgrep" or ":vimgrepadd" command return a pointer past the
 * pattern.  Otherwise return eap->arg.
 */
    static char_u *
skip_grep_pat(exarg_T *eap)
{
    char_u	*p = eap->arg;

    නම් (*p != NUL && (eap->cmdidx == CMD_vimgrep || eap->cmdidx == CMD_lvimgrep
		|| eap->cmdidx == CMD_vimgrepadd
		|| eap->cmdidx == CMD_lvimgrepadd
		|| grep_internal(eap->cmdidx)))
    {
	p = skip_vimgrep_pat(p, NULL, NULL);
	නම් (p == NULL)
	    p = eap->arg;
    }
    ආපසු p;
}

/*
 * For the ":make" and ":grep" commands insert the 'makeprg'/'grepprg' option
 * in the command line, so that things like % get expanded.
 */
    static char_u *
replace_makeprg(exarg_T *eap, char_u *p, char_u **cmdlinep)
{
    char_u	*new_cmdline;
    char_u	*program;
    char_u	*pos;
    char_u	*ptr;
    පූර්ණ_සංඛ්‍යා		len;
    පූර්ණ_සංඛ්‍යා		i;

    /*
     * Don't do it when ":vimgrep" is used for ":grep".
     */
    නම් ((eap->cmdidx == CMD_make || eap->cmdidx == CMD_lmake
		     || eap->cmdidx == CMD_grep || eap->cmdidx == CMD_lgrep
		     || eap->cmdidx == CMD_grepadd
		     || eap->cmdidx == CMD_lgrepadd)
	    && !grep_internal(eap->cmdidx))
    {
	නම් (eap->cmdidx == CMD_grep || eap->cmdidx == CMD_lgrep
	    || eap->cmdidx == CMD_grepadd || eap->cmdidx == CMD_lgrepadd)
	{
	    නම් (*curbuf->b_p_gp == NUL)
		program = p_gp;
	    නැතහොත්
		program = curbuf->b_p_gp;
	}
	නැතහොත්
	{
	    නම් (*curbuf->b_p_mp == NUL)
		program = p_mp;
	    නැතහොත්
		program = curbuf->b_p_mp;
	}

	p = skipwhite(p);

	නම් ((pos = (char_u *)strstr((var_na *)program, "$*")) != NULL)
	{
	    // replace $* by given arguments
	    i = 1;
	    අතරතුර ((pos = (char_u *)strstr((var_na *)pos + 2, "$*")) != NULL)
		++i;
	    len = (පූර්ණ_සංඛ්‍යා)STRLEN(p);
	    new_cmdline = alloc(STRLEN(program) + (size_t)i * (len - 2) + 1);
	    නම් (new_cmdline == NULL)
		ආපසු NULL;			// out of memory
	    ptr = new_cmdline;
	    අතරතුර ((pos = (char_u *)strstr((var_na *)program, "$*")) != NULL)
	    {
		i = (පූර්ණ_සංඛ්‍යා)(pos - program);
		STRNCPY(ptr, program, i);
		STRCPY(ptr += i, p);
		ptr += len;
		program = pos + 2;
	    }
	    STRCPY(ptr, program);
	}
	නැතහොත්
	{
	    size_t program_len = STRLEN(program);

	    new_cmdline = alloc(program_len + STRLEN(p) + 2);
	    නම් (new_cmdline == NULL)
		ආපසු NULL;			// out of memory
	    STRCPY(new_cmdline, program);
	    STRCPY(new_cmdline + program_len, " ");
	    STRCPY(new_cmdline + program_len + 1, p);
	}
	msg_make(p);

	// 'eap->cmd' is not set here, because it is not used at CMD_make
	vim_free(*cmdlinep);
	*cmdlinep = new_cmdline;
	p = new_cmdline;
    }
    ආපසු p;
}
#endif

/*
 * Expand file name in Ex command argument.
 * When an error is detected, "errormsgp" is set to a non-NULL pointer.
 * Return FAIL for failure, OK otherwise.
 */
    පූර්ණ_සංඛ්‍යා
expand_filename(
    exarg_T	*eap,
    char_u	**cmdlinep,
    var_na	**errormsgp)
{
    පූර්ණ_සංඛ්‍යා		has_wildcards;	// need to expand wildcards
    char_u	*repl;
    size_t	srclen;
    char_u	*p;
    පූර්ණ_සංඛ්‍යා		n;
    පූර්ණ_සංඛ්‍යා		escaped;

#ifdef FEAT_QUICKFIX
    // Skip a regexp pattern for ":vimgrep[add] pat file..."
    p = skip_grep_pat(eap);
#නැතහොත්
    p = eap->arg;
#endif

    /*
     * Decide to expand wildcards *before* replacing '%', '#', etc.  If
     * the file name contains a wildcard it should not cause expanding.
     * (it will be expanded anyway if there is a wildcard before replacing).
     */
    has_wildcards = mch_has_wildcard(p);
    අතරතුර (*p != NUL)
    {
#ifdef FEAT_EVAL
	// Skip over `=expr`, wildcards in it are not expanded.
	නම් (p[0] == '`' && p[1] == '=')
	{
	    p += 2;
	    (void)skip_expr(&p, NULL);
	    නම් (*p == '`')
		++p;
	    continue;
	}
#endif
	/*
	 * Quick check if this cannot be the start of a special string.
	 * Also removes backslash before '%', '#' and '<'.
	 */
	නම් (vim_strchr((char_u *)"%#<", *p) == NULL)
	{
	    ++p;
	    continue;
	}

	/*
	 * Try to find a match at this position.
	 */
	repl = eval_vars(p, eap->arg, &srclen, &(eap->do_ecmd_lnum),
						    errormsgp, &escaped, TRUE);
	නම් (*errormsgp != NULL)		// error detected
	    ආපසු FAIL;
	නම් (repl == NULL)		// no match found
	{
	    p += srclen;
	    continue;
	}

	// Wildcards won't be expanded below, the replacement is taken
	// literally.  But do expand "~/file", "~user/file" and "$HOME/file".
	නම් (vim_strchr(repl, '$') != NULL || vim_strchr(repl, '~') != NULL)
	{
	    char_u *l = repl;

	    repl = expand_env_save(repl);
	    vim_free(l);
	}

	// Need to escape white space et al. with a backslash.
	// Don't do this for:
	// - replacement that already has been escaped: "##"
	// - shell commands (may have to use quotes instead).
	// - non-unix systems when there is a single argument (spaces don't
	//   separate arguments then).
	නම් (!eap->usefilter
		&& !escaped
		&& eap->cmdidx != CMD_bang
		&& eap->cmdidx != CMD_grep
		&& eap->cmdidx != CMD_grepadd
		&& eap->cmdidx != CMD_hardcopy
		&& eap->cmdidx != CMD_lgrep
		&& eap->cmdidx != CMD_lgrepadd
		&& eap->cmdidx != CMD_lmake
		&& eap->cmdidx != CMD_make
		&& eap->cmdidx != CMD_terminal
#ifndef UNIX
		&& !(eap->argt & EX_NOSPC)
#endif
		)
	{
	    char_u	*l;
#ifdef BACKSLASH_IN_FILENAME
	    // Don't escape a backslash here, because rem_backslash() doesn't
	    // remove it later.
	    static char_u *nobslash = (char_u *)" \t\"|";
# define ESCAPE_CHARS nobslash
#නැතහොත්
# define ESCAPE_CHARS escape_chars
#endif

	    සඳහා (l = repl; *l; ++l)
		නම් (vim_strchr(ESCAPE_CHARS, *l) != NULL)
		{
		    l = vim_strsave_escaped(repl, ESCAPE_CHARS);
		    නම් (l != NULL)
		    {
			vim_free(repl);
			repl = l;
		    }
		    bha_nga;
		}
	}

	// For a shell command a '!' must be escaped.
	නම් ((eap->usefilter || eap->cmdidx == CMD_bang
						|| eap->cmdidx == CMD_terminal)
			    && vim_strpbrk(repl, (char_u *)"!") != NULL)
	{
	    char_u	*l;

	    l = vim_strsave_escaped(repl, (char_u *)"!");
	    නම් (l != NULL)
	    {
		vim_free(repl);
		repl = l;
	    }
	}

	p = repl_cmdline(eap, p, srclen, repl, cmdlinep);
	vim_free(repl);
	නම් (p == NULL)
	    ආපසු FAIL;
    }

    /*
     * One file argument: Expand wildcards.
     * Don't do this with ":r !command" or ":w !command".
     */
    නම් ((eap->argt & EX_NOSPC) && !eap->usefilter)
    {
	/*
	 * May do this twice:
	 * 1. Replace environment variables.
	 * 2. Replace any other wildcards, remove backslashes.
	 */
	සඳහා (n = 1; n <= 2; ++n)
	{
	    නම් (n == 2)
	    {
		/*
		 * Halve the number of backslashes (this is Vi compatible).
		 * For Unix and OS/2, when wildcards are expanded, this is
		 * done by ExpandOne() below.
		 */
#නම් defined(UNIX)
		නම් (!has_wildcards)
#endif
		    backslash_halve(eap->arg);
	    }

	    නම් (has_wildcards)
	    {
		නම් (n == 1)
		{
		    /*
		     * First loop: May expand environment variables.  This
		     * can be done much faster with expand_env() than with
		     * something else (e.g., calling a shell).
		     * After expanding environment variables, check again
		     * if there are still wildcards present.
		     */
		    නම් (vim_strchr(eap->arg, '$') != NULL
			    || vim_strchr(eap->arg, '~') != NULL)
		    {
			expand_env_esc(eap->arg, NameBuff, MAXPATHL,
					(char_u *)(" \t" PATH_ESC_WILDCARDS),
					TRUE, NULL);
			has_wildcards = mch_has_wildcard(NameBuff);
			p = NameBuff;
		    }
		    නැතහොත්
			p = NULL;
		}
		නැතහොත් // n == 2
		{
		    expand_T	xpc;
		    පූර්ණ_සංඛ්‍යා		options = WILD_LIST_NOTFOUND
					       | WILD_NOERROR | WILD_ADD_SLASH;

		    ExpandInit(&xpc);
		    xpc.xp_context = EXPAND_FILES;
		    නම් (p_wic)
			options += WILD_ICASE;
		    p = ExpandOne(&xpc, eap->arg, NULL,
						   options, WILD_EXPAND_FREE);
		    නම් (p == NULL)
			ආපසු FAIL;
		}
		නම් (p != NULL)
		{
		    (void)repl_cmdline(eap, eap->arg, STRLEN(eap->arg),
								 p, cmdlinep);
		    නම් (n == 2)	// p came from ExpandOne()
			vim_free(p);
		}
	    }
	}
    }
    ආපසු OK;
}

/*
 * Replace part of the command line, keeping eap->cmd, eap->arg and
 * eap->nextcmd correct.
 * "src" points to the part that is to be replaced, of length "srclen".
 * "repl" is the replacement string.
 * Returns a pointer to the character after the replaced string.
 * Returns NULL for failure.
 */
    static char_u *
repl_cmdline(
    exarg_T	*eap,
    char_u	*src,
    size_t	srclen,
    char_u	*repl,
    char_u	**cmdlinep)
{
    size_t	repllen;
    size_t	taillen;
    size_t	i;
    char_u	*new_cmdline;
    size_t	new_cmdlinelen;

    /*
     * The new command line is build in new_cmdline[].
     * First allocate it.
     * Careful: a "+cmd" argument may have been NUL terminated.
     */
    repllen = STRLEN(repl);
    taillen = STRLEN(src + srclen);
    i = (src - *cmdlinep) + repllen + taillen + 3;
    නම් (eap->nextcmd != NULL)
	i += STRLEN(eap->nextcmd);	// add space for next command
    නම් ((new_cmdline = alloc(i)) == NULL)
	ආපසු NULL;			// out of memory!

    /*
     * Copy the stuff before the expanded part.
     * Copy the expanded stuff.
     * Copy what came after the expanded part.
     * Copy the next commands, if there are any.
     */
    new_cmdlinelen = src - *cmdlinep;	// length of part before replacement
    mch_memmove(new_cmdline, *cmdlinep, new_cmdlinelen);

    mch_memmove(new_cmdline + new_cmdlinelen, repl, repllen);
    new_cmdlinelen += repllen;		// remember the end of the string
    STRCPY(new_cmdline + new_cmdlinelen, src + srclen);
    src = new_cmdline + new_cmdlinelen;	// remember where to continue

    නම් (eap->nextcmd != NULL)		// append next command
    {
	new_cmdlinelen += taillen + 1;
	STRCPY(new_cmdline + new_cmdlinelen, eap->nextcmd);
	eap->nextcmd = new_cmdline + new_cmdlinelen;
    }
    eap->cmd = new_cmdline + (eap->cmd - *cmdlinep);
    eap->arg = new_cmdline + (eap->arg - *cmdlinep);
    නම් (eap->do_ecmd_cmd != NULL && eap->do_ecmd_cmd != dollar_command)
	eap->do_ecmd_cmd = new_cmdline + (eap->do_ecmd_cmd - *cmdlinep);
    vim_free(*cmdlinep);
    *cmdlinep = new_cmdline;

    ආපසු src;
}

/*
 * Check for '|' to separate commands and '"' to start comments.
 * If "keep_backslash" is TRUE do not remove any backslash.
 */
    void
separate_nextcmd(exarg_T *eap, පූර්ණ_සංඛ්‍යා keep_backslash)
{
    පූර්ණ_සංඛ්‍යා		vim9script = in_vim9script();
    char_u	*p;

#ifdef FEAT_QUICKFIX
    p = skip_grep_pat(eap);
#නැතහොත්
    p = eap->arg;
#endif

    සඳහා ( ; *p; MB_PTR_ADV(p))
    {
	නම් (*p == Ctrl_V)
	{
	    නම් ((eap->argt & (EX_CTRLV | EX_XFILE)) || keep_backslash)
		++p;		// skip CTRL-V and next char
	    නැතහොත්
				// remove CTRL-V and skip next char
		STRMOVE(p, p + 1);
	    නම් (*p == NUL)		// stop at NUL after CTRL-V
		bha_nga;
	}

#ifdef FEAT_EVAL
	// Skip over `=expr` when wildcards are expanded.
	නැතහොත් නම් (p[0] == '`' && p[1] == '=' && (eap->argt & EX_XFILE))
	{
	    p += 2;
	    (void)skip_expr(&p, NULL);
	    නම් (*p == NUL)		// stop at NUL after CTRL-V
		bha_nga;
	}
#endif

	// Check for '"'/'#': start of comment or '|': next command
	// :@" and :*" do not start a comment!
	// :redir @" doesn't either.
	නැතහොත් නම් ((*p == '"'
		    && !vim9script
		    && !(eap->argt & EX_NOTRLCOM)
		    && ((eap->cmdidx != CMD_at && eap->cmdidx != CMD_star)
							      || p != eap->arg)
		    && (eap->cmdidx != CMD_redir
					 || p != eap->arg + 1 || p[-1] != '@'))
		|| (*p == '#'
		    && vim9script
		    && !(eap->argt & EX_NOTRLCOM)
		    && p > eap->cmd && VIM_ISWHITE(p[-1]))
		|| (*p == '|'
		    && eap->cmdidx != CMD_append
		    && eap->cmdidx != CMD_change
		    && eap->cmdidx != CMD_insert)
		|| *p == '\n')
	{
	    /*
	     * We remove the '\' before the '|', unless EX_CTRLV is used
	     * AND 'b' is present in 'cpoptions'.
	     */
	    නම් ((vim_strchr(p_cpo, CPO_BAR) == NULL
			      || !(eap->argt & EX_CTRLV)) && *(p - 1) == '\\')
	    {
		නම් (!keep_backslash)
		{
		    STRMOVE(p - 1, p);	// remove the '\'
		    --p;
		}
	    }
	    නැතහොත්
	    {
		set_nextcmd(eap, p);
		*p = NUL;
		bha_nga;
	    }
	}
    }

    නම් (!(eap->argt & EX_NOTRLCOM))	// remove trailing spaces
	del_trailing_spaces(eap->arg);
}

/*
 * get + command from ex argument
 */
    static char_u *
getargcmd(char_u **argp)
{
    char_u *arg = *argp;
    char_u *command = NULL;

    නම් (*arg == '+')	    // +[command]
    {
	++arg;
	නම් (vim_isspace(*arg) || *arg == NUL)
	    command = dollar_command;
	නැතහොත්
	{
	    command = arg;
	    arg = skip_cmd_arg(command, TRUE);
	    නම් (*arg != NUL)
		*arg++ = NUL;		// terminate command with NUL
	}

	arg = skipwhite(arg);	// skip over spaces
	*argp = arg;
    }
    ආපසු command;
}

/*
 * Find end of "+command" argument.  Skip over "\ " and "\\".
 */
    char_u *
skip_cmd_arg(
    char_u *p,
    පූර්ණ_සංඛ්‍යා	   rembs)	// TRUE to halve the number of backslashes
{
    අතරතුර (*p && !vim_isspace(*p))
    {
	නම් (*p == '\\' && p[1] != NUL)
	{
	    නම් (rembs)
		STRMOVE(p, p + 1);
	    නැතහොත්
		++p;
	}
	MB_PTR_ADV(p);
    }
    ආපසු p;
}

    පූර්ණ_සංඛ්‍යා
get_bad_opt(char_u *p, exarg_T *eap)
{
    නම් (STRICMP(p, "keep") == 0)
	eap->bad_char = BAD_KEEP;
    නැතහොත් නම් (STRICMP(p, "drop") == 0)
	eap->bad_char = BAD_DROP;
    නැතහොත් නම් (MB_BYTE2LEN(*p) == 1 && p[1] == NUL)
	eap->bad_char = *p;
    නැතහොත්
	ආපසු FAIL;
    ආපසු OK;
}

/*
 * Function given to ExpandGeneric() to obtain the list of bad= names.
 */
    static char_u *
get_bad_name(expand_T *xp UNUSED, පූර්ණ_සංඛ්‍යා idx)
{
    // Note: Keep this in sync with getargopt.
    static var_na *(p_bad_values[]) =
    {
	"?",
	"keep",
	"drop",
    };

    නම් (idx < 0 || idx >= (පූර්ණ_සංඛ්‍යා)ARRAY_LENGTH(p_bad_values))
	ආපසු NULL;

    ආපසු (char_u*)p_bad_values[idx];
}

/*
 * Get "++opt=arg" argument.
 * Return FAIL or OK.
 */
    static පූර්ණ_සංඛ්‍යා
getargopt(exarg_T *eap)
{
    char_u	*arg = eap->arg + 2;
    පූර්ණ_සංඛ්‍යා		*pp = NULL;
    පූර්ණ_සංඛ්‍යා		bad_char_idx;
    char_u	*p;

    // Note: Keep this in sync with get_argopt_name.

    // ":edit ++[no]bin[ary] file"
    නම් (STRNCMP(arg, "bin", 3) == 0 || STRNCMP(arg, "nobin", 5) == 0)
    {
	නම් (*arg == 'n')
	{
	    arg += 2;
	    eap->force_bin = FORCE_NOBIN;
	}
	නැතහොත්
	    eap->force_bin = FORCE_BIN;
	නම් (!checkforcmd(&arg, "binary", 3))
	    ආපසු FAIL;
	eap->arg = skipwhite(arg);
	ආපසු OK;
    }

    // ":read ++edit file"
    නම් (STRNCMP(arg, "edit", 4) == 0)
    {
	eap->read_edit = TRUE;
	eap->arg = skipwhite(arg + 4);
	ආපසු OK;
    }

    නම් (STRNCMP(arg, "ff", 2) == 0)
    {
	arg += 2;
	pp = &eap->force_ff;
    }
    නැතහොත් නම් (STRNCMP(arg, "fileformat", 10) == 0)
    {
	arg += 10;
	pp = &eap->force_ff;
    }
    නැතහොත් නම් (STRNCMP(arg, "enc", 3) == 0)
    {
	නම් (STRNCMP(arg, "encoding", 8) == 0)
	    arg += 8;
	නැතහොත්
	    arg += 3;
	pp = &eap->force_enc;
    }
    නැතහොත් නම් (STRNCMP(arg, "bad", 3) == 0)
    {
	arg += 3;
	pp = &bad_char_idx;
    }

    නම් (pp == NULL || *arg != '=')
	ආපසු FAIL;

    ++arg;
    *pp = (පූර්ණ_සංඛ්‍යා)(arg - eap->cmd);
    arg = skip_cmd_arg(arg, FALSE);
    eap->arg = skipwhite(arg);
    *arg = NUL;

    නම් (pp == &eap->force_ff)
    {
	නම් (check_ff_value(eap->cmd + eap->force_ff) == FAIL)
	    ආපසු FAIL;
	eap->force_ff = eap->cmd[eap->force_ff];
    }
    නැතහොත් නම් (pp == &eap->force_enc)
    {
	// Make 'fileencoding' lower case.
	සඳහා (p = eap->cmd + eap->force_enc; *p != NUL; ++p)
	    *p = TOLOWER_ASC(*p);
    }
    නැතහොත්
    {
	// Check ++bad= argument.  Must be a single-byte character, "keep" or
	// "drop".
	නම් (get_bad_opt(eap->cmd + bad_char_idx, eap) == FAIL)
	    ආපසු FAIL;
    }

    ආපසු OK;
}

/*
 * Function given to ExpandGeneric() to obtain the list of ++opt names.
 */
    static char_u *
get_argopt_name(expand_T *xp UNUSED, පූර්ණ_සංඛ්‍යා idx)
{
    // Note: Keep this in sync with getargopt.
    static var_na *(p_opt_values[]) =
    {
	"fileformat=",
	"encoding=",
	"binary",
	"nobinary",
	"bad=",
	"edit",
    };

    නම් (idx < 0 || idx >= (පූර්ණ_සංඛ්‍යා)ARRAY_LENGTH(p_opt_values))
	ආපසු NULL;

    ආපසු (char_u*)p_opt_values[idx];
}

/*
 * Command-line expansion for ++opt=name.
 */
    පූර්ණ_සංඛ්‍යා
expand_argopt(
	char_u	    *pat,
	expand_T    *xp,
	regmatch_T  *rmp,
	char_u	    ***matches,
	පූර්ණ_සංඛ්‍යා	    *numMatches)
{
    නම් (xp->xp_pattern > xp->xp_line && *(xp->xp_pattern-1) == '=')
    {
	char_u *(*cb)(expand_T *, පූර්ණ_සංඛ්‍යා) = NULL;

	char_u *name_end = xp->xp_pattern - 1;
	නම් (name_end - xp->xp_line >= 2
		&& STRNCMP(name_end - 2, "ff", 2) == 0)
	    cb = get_fileformat_name;
	නැතහොත් නම් (name_end - xp->xp_line >= 10
		&& STRNCMP(name_end - 10, "fileformat", 10) == 0)
	    cb = get_fileformat_name;
	නැතහොත් නම් (name_end - xp->xp_line >= 3
		&& STRNCMP(name_end - 3, "enc", 3) == 0)
	    cb = get_encoding_name;
	නැතහොත් නම් (name_end - xp->xp_line >= 8
		&& STRNCMP(name_end - 8, "encoding", 8) == 0)
	    cb = get_encoding_name;
	නැතහොත් නම් (name_end - xp->xp_line >= 3
		&& STRNCMP(name_end - 3, "bad", 3) == 0)
	    cb = get_bad_name;

	නම් (cb != NULL)
	{
	    ආපසු ExpandGeneric(
		    pat,
		    xp,
		    rmp,
		    matches,
		    numMatches,
		    cb,
		    FALSE);
	}
	ආපසු FAIL;
    }

    // Special handling of "ff" which acts as a short form of
    // "fileformat", as "ff" is not a substring of it.
    නම් (xp->xp_pattern_len == 2
	    && STRNCMP(xp->xp_pattern, "ff", xp->xp_pattern_len) == 0)
    {
	*matches = ALLOC_MULT(char_u *, 1);
	නම් (*matches == NULL)
	    ආපසු FAIL;
	*numMatches = 1;
	(*matches)[0] = vim_strsave((char_u*)"fileformat=");
	ආපසු OK;
    }

    ආපසු ExpandGeneric(
	    pat,
	    xp,
	    rmp,
	    matches,
	    numMatches,
	    get_argopt_name,
	    FALSE);
}

    static void
ex_autocmd(exarg_T *eap)
{
    නම් (eap->skip)
    {
	skip_cmd_block(eap);
	ආපසු;
    }

    /*
     * Disallow autocommands from .exrc and .vimrc in current
     * directory for security reasons.
     */
    නම් (secure)
    {
	secure = 2;
	eap->errmsg =
	      _(e_command_not_allowed_from_vimrc_in_current_dir_or_tag_search);
    }
    නැතහොත් නම් (eap->cmdidx == CMD_autocmd)
	do_autocmd(eap, eap->arg, eap->forceit);
    නැතහොත්
	do_augroup(eap->arg, eap->forceit);
}

/*
 * ":doautocmd": Apply the automatic commands to the current buffer.
 */
    static void
ex_doautocmd(exarg_T *eap)
{
    char_u	*arg = eap->arg;
    පූර්ණ_සංඛ්‍යා		call_do_modelines = check_nomodeline(&arg);
    පූර්ණ_සංඛ්‍යා		did_aucmd;

    (void)do_doautocmd(arg, TRUE, &did_aucmd);
    // Only when there is no <nomodeline>.
    නම් (call_do_modelines && did_aucmd)
	do_modelines(0);
}

/*
 * :[N]bunload[!] [N] [bufname] unload buffer
 * :[N]bdelete[!] [N] [bufname] delete buffer from buffer list
 * :[N]bwipeout[!] [N] [bufname] delete buffer really
 */
    static void
ex_bunload(exarg_T *eap)
{
    නම් (ERROR_IF_ANY_POPUP_WINDOW)
	ආපසු;
    eap->errmsg = do_bufdel(
	    eap->cmdidx == CMD_bdelete ? DOBUF_DEL
		: eap->cmdidx == CMD_bwipeout ? DOBUF_WIPE
		: DOBUF_UNLOAD, eap->arg,
	    eap->addr_count, (පූර්ණ_සංඛ්‍යා)eap->line1, (පූර්ණ_සංඛ්‍යා)eap->line2, eap->forceit);
}

/*
 * :[N]buffer [N]	to buffer N
 * :[N]sbuffer [N]	to buffer N
 */
    static void
ex_buffer(exarg_T *eap)
{
    නම් (ERROR_IF_ANY_POPUP_WINDOW)
	ආපසු;
    do_exbuffer(eap);
}

/*
 * ":buffer" command and alike.
 */
    static void
do_exbuffer(exarg_T *eap)
{
    නම් (*eap->arg)
	eap->errmsg = ex_errmsg(e_trailing_characters_str, eap->arg);
    නැතහොත්
    {
	නම් (eap->addr_count == 0)	// default is current buffer
	    goto_buffer(eap, DOBUF_CURRENT, FORWARD, 0);
	නැතහොත්
	    goto_buffer(eap, DOBUF_FIRST, FORWARD, (පූර්ණ_සංඛ්‍යා)eap->line2);
	නම් (eap->do_ecmd_cmd != NULL)
	    do_cmd_argument(eap->do_ecmd_cmd);
    }
}

/*
 * :[N]bmodified [N]	to next mod. buffer
 * :[N]sbmodified [N]	to next mod. buffer
 */
    static void
ex_bmodified(exarg_T *eap)
{
    goto_buffer(eap, DOBUF_MOD, FORWARD, (පූර්ණ_සංඛ්‍යා)eap->line2);
    නම් (eap->do_ecmd_cmd != NULL)
	do_cmd_argument(eap->do_ecmd_cmd);
}

/*
 * :[N]bnext [N]	to next buffer
 * :[N]sbnext [N]	split and to next buffer
 */
    static void
ex_bnext(exarg_T *eap)
{
    නම් (ERROR_IF_ANY_POPUP_WINDOW)
	ආපසු;

    goto_buffer(eap, DOBUF_CURRENT, FORWARD, (පූර්ණ_සංඛ්‍යා)eap->line2);
    නම් (eap->do_ecmd_cmd != NULL)
	do_cmd_argument(eap->do_ecmd_cmd);
}

/*
 * :[N]bNext [N]	to previous buffer
 * :[N]bprevious [N]	to previous buffer
 * :[N]sbNext [N]	split and to previous buffer
 * :[N]sbprevious [N]	split and to previous buffer
 */
    static void
ex_bprevious(exarg_T *eap)
{
    නම් (ERROR_IF_ANY_POPUP_WINDOW)
	ආපසු;

    goto_buffer(eap, DOBUF_CURRENT, BACKWARD, (පූර්ණ_සංඛ්‍යා)eap->line2);
    නම් (eap->do_ecmd_cmd != NULL)
	do_cmd_argument(eap->do_ecmd_cmd);
}

/*
 * :brewind		to first buffer
 * :bfirst		to first buffer
 * :sbrewind		split and to first buffer
 * :sbfirst		split and to first buffer
 */
    static void
ex_brewind(exarg_T *eap)
{
    නම් (ERROR_IF_ANY_POPUP_WINDOW)
	ආපසු;

    goto_buffer(eap, DOBUF_FIRST, FORWARD, 0);
    නම් (eap->do_ecmd_cmd != NULL)
	do_cmd_argument(eap->do_ecmd_cmd);
}

/*
 * :blast		to last buffer
 * :sblast		split and to last buffer
 */
    static void
ex_blast(exarg_T *eap)
{
    නම් (ERROR_IF_ANY_POPUP_WINDOW)
	ආපසු;

    goto_buffer(eap, DOBUF_LAST, BACKWARD, 0);
    නම් (eap->do_ecmd_cmd != NULL)
	do_cmd_argument(eap->do_ecmd_cmd);
}

/*
 * Check if "c" ends an Ex command.
 * In Vim9 script does not check for white space before #.
 */
    පූර්ණ_සංඛ්‍යා
ends_excmd(පූර්ණ_සංඛ්‍යා c)
{
    පූර්ණ_සංඛ්‍යා comment_char = '"';

    නම් (in_vim9script())
	comment_char = '#';
    ආපසු (c == NUL || c == '|' || c == comment_char || c == '\n');
}

/*
 * Like ends_excmd() but checks that a # in Vim9 script either has "cmd" equal
 * to "cmd_start" or has a white space character before it.
 */
    පූර්ණ_සංඛ්‍යා
ends_excmd2(char_u *cmd_start UNUSED, char_u *cmd)
{
    පූර්ණ_සංඛ්‍යා c = *cmd;

    නම් (c == NUL || c == '|' || c == '\n')
	ආපසු TRUE;
    නම් (in_vim9script())
	//  # starts a comment, #{ might be a mistake, #{{ can start a fold
	ආපසු c == '#' && (cmd[1] != '{' || cmd[2] == '{')
				 && (cmd == cmd_start || VIM_ISWHITE(cmd[-1]));
    ආපසු c == '"';
}

#නම් defined(FEAT_SYN_HL) || defined(FEAT_SEARCH_EXTRA) || defined(FEAT_EVAL)
/*
 * Return the next command, after the first '|' or '\n'.
 * Return NULL if not found.
 */
    char_u *
find_nextcmd(char_u *p)
{
    අතරතුර (*p != '|' && *p != '\n')
    {
	නම් (*p == NUL)
	    ආපසු NULL;
	++p;
    }
    ආපසු (p + 1);
}
#endif

/*
 * Check if *p is a separator between Ex commands, skipping over white space.
 * Return NULL if it isn't, the following character if it is.
 */
    char_u *
check_nextcmd(char_u *p)
{
    char_u *s = skipwhite(p);

    නම් (*s == '|' || *s == '\n')
	ආපසු (s + 1);
    නැතහොත්
	ආපසු NULL;
}

/*
 * If "eap->nextcmd" is not set, check for a next command at "p".
 */
    void
set_nextcmd(exarg_T *eap, char_u *arg)
{
    char_u *p = skipwhite(arg);

    නම් (in_vim9script() && *p == '#')
    {
	char_u *nl = vim_strchr(p, NL);
	නම් (nl != NULL)
	    p = nl;
    }

    p = check_nextcmd(p);

    නම් (eap->nextcmd == NULL)
	eap->nextcmd = p;
    නැතහොත් නම් (p != NULL)
	// cannot use "| command" inside a  {} block
	semsg(_(e_cannot_use_bar_to_separate_commands_here_str), arg);
}

/*
 * - if there are more files to edit
 * - and this is the last window
 * - and forceit not used
 * - and not repeated twice on a row
 *    return FAIL and give error message if 'message' TRUE
 * return OK otherwise
 */
    static පූර්ණ_සංඛ්‍යා
check_more(
    පූර්ණ_සංඛ්‍යා message,	    // when FALSE check only, no messages
    පූර්ණ_සංඛ්‍යා forceit)
{
    පූර්ණ_සංඛ්‍යා	    n = ARGCOUNT - curwin->w_arg_idx - 1;

    නම් (!forceit && only_one_window()
	    && ARGCOUNT > 1 && !arg_had_last && n > 0 && quitmore == 0)
    {
	නම් (message)
	{
#නම් defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
	    නම් ((p_confirm || (cmdmod.cmod_flags & CMOD_CONFIRM))
						    && curbuf->b_fname != NULL)
	    {
		char_u	buff[DIALOG_MSG_SIZE];

		vim_snprintf((var_na *)buff, DIALOG_MSG_SIZE,
			NGETTEXT("%d more file to edit.  Quit anyway?",
			    "%d more files to edit.  Quit anyway?", n), n);
		නම් (vim_dialog_yesno(VIM_QUESTION, NULL, buff, 1) == VIM_YES)
		    ආපසු OK;
		ආපසු FAIL;
	    }
#endif
	    semsg(NGETTEXT(e_nr_more_file_to_edit,
			   e_nr_more_files_to_edit , n), n);
	    quitmore = 2;	    // next try to quit is allowed
	}
	ආපසු FAIL;
    }
    ආපසු OK;
}

/*
 * Function given to ExpandGeneric() to obtain the list of command names.
 */
    char_u *
get_command_name(expand_T *xp UNUSED, පූර්ණ_සංඛ්‍යා idx)
{
    නම් (idx >= (පූර්ණ_සංඛ්‍යා)CMD_SIZE)
	ආපසු expand_user_command_name(idx);
    // the following are no real commands
    නම් (STRNCMP(cmdnames[idx].cmd_name, "{", 1) == 0 ||
	STRNCMP(cmdnames[idx].cmd_name, "}", 1) == 0)
	ආපසු (char_u *)"";
    ආපසු cmdnames[idx].cmd_name;
}

    static void
ex_colorscheme(exarg_T *eap)
{
    නම් (*eap->arg == NUL)
    {
#ifdef FEAT_EVAL
	char_u *expr = vim_strsave((char_u *)"g:colors_name");
	char_u *p = NULL;

	නම් (expr != NULL)
	{
	    ++emsg_off;
	    p = eval_to_string(expr, FALSE, FALSE);
	    --emsg_off;
	    vim_free(expr);
	}
	නම් (p != NULL)
	{
	    msg((var_na *)p);
	    vim_free(p);
	}
	නැතහොත්
	    msg("default");
#නැතහොත්
	msg(_("unknown"));
#endif
    }
    නැතහොත් නම් (load_colors(eap->arg) == FAIL)
	semsg(_(e_cannot_find_color_scheme_str), eap->arg);

#ifdef FEAT_VTP
    නැතහොත් නම් (has_vtp_working())
    {
	// background color change requires clear + redraw
	update_screen(UPD_CLEAR);
	redrawcmd();
    }
#endif
}

    static void
ex_highlight(exarg_T *eap)
{
    නම් (*eap->arg == NUL && eap->cmd[2] == '!')
	msg(_("Greetings, Vim user!"));
    do_highlight(eap->arg, eap->forceit, FALSE);
}


/*
 * Call this function if we thought we were going to exit, but we won't
 * (because of an error).  May need to restore the terminal mode.
 */
    void
not_exiting(පූර්ණ_සංඛ්‍යා save_exiting)
{
    exiting = save_exiting;
    settmode(TMODE_RAW);
}

    පූර්ණ_සංඛ්‍යා
before_quit_autocmds(win_T *wp, පූර්ණ_සංඛ්‍යා quit_all, පූර්ණ_සංඛ්‍යා forceit)
{
    apply_autocmds(EVENT_QUITPRE, NULL, NULL, FALSE, wp->w_buffer);

    // Bail out when autocommands closed the window.
    // Refuse to quit when the buffer in the last window is being closed (can
    // only happen in autocommands).
    නම් (!win_valid(wp)
	    || curbuf_locked()
	    || (wp->w_buffer->b_nwindows == 1 && wp->w_buffer->b_locked > 0))
	ආපසු TRUE;

    නම් (quit_all || (check_more(FALSE, forceit) == OK && only_one_window()))
    {
	apply_autocmds(EVENT_EXITPRE, NULL, NULL, FALSE, curbuf);
	// Refuse to quit when locked or when the window was closed or the
	// buffer in the last window is being closed (can only happen in
	// autocommands).
	නම් (!win_valid(wp) || curbuf_locked()
			  || (curbuf->b_nwindows == 1 && curbuf->b_locked > 0))
	    ආපසු TRUE;
    }

    ආපසු FALSE;
}

/*
 * ":quit": quit current window, quit Vim if the last window is closed.
 * ":{nr}quit": quit window {nr}
 * Also used when closing a terminal window that's the last one.
 */
    void
ex_quit(exarg_T *eap)
{
    win_T	*wp;

    නම් (cmdwin_type != 0)
    {
	cmdwin_result = Ctrl_C;
	ආපසු;
    }
    // Don't quit while editing the command line.
    නම් (text_locked())
    {
	text_locked_msg();
	ආපසු;
    }
    නම් (eap->addr_count > 0)
    {
	පූර්ණ_සංඛ්‍යා	wnr = eap->line2;

	සඳහා (wp = firstwin; wp->w_next != NULL; wp = wp->w_next)
	    නම් (--wnr <= 0)
		bha_nga;
    }
    නැතහොත්
	wp = curwin;

    // Refuse to quit when locked.
    නම් (curbuf_locked())
	ආපසු;

    // Trigger QuitPre and maybe ExitPre
    නම් (before_quit_autocmds(wp, FALSE, eap->forceit))
	ආපසු;

#ifdef FEAT_NETBEANS_INTG
    netbeansForcedQuit = eap->forceit;
#endif

    පූර්ණ_සංඛ්‍යා save_exiting = exiting;
    /*
     * If there is only one relevant window we will exit.
     */
    නම් (check_more(FALSE, eap->forceit) == OK && only_one_window())
	exiting = TRUE;
    නම් ((!buf_hide(wp->w_buffer)
		&& check_changed(wp->w_buffer, (p_awa ? CCGD_AW : 0)
				       | (eap->forceit ? CCGD_FORCEIT : 0)
				       | CCGD_EXCMD))
	    || check_more(TRUE, eap->forceit) == FAIL
	    || (only_one_window() && check_changed_any(eap->forceit, TRUE)))
    {
	not_exiting(save_exiting);
    }
    නැතහොත්
    {
	// quit last window
	// Note: only_one_window() returns true, even so a help window is
	// still open. In that case only quit, if no address has been
	// specified. Example:
	// :h|wincmd w|1q     - don't quit
	// :h|wincmd w|q      - quit
	නම් (only_one_window() && (ONE_WINDOW || eap->addr_count == 0))
	    getout(0);
	not_exiting(save_exiting);
#ifdef FEAT_GUI
	need_mouse_correct = TRUE;
#endif
	// close window; may free buffer
	win_close(wp, !buf_hide(wp->w_buffer) || eap->forceit);
    }
}

/*
 * ":cquit".
 */
    static void
ex_cquit(exarg_T *eap UNUSED)
{
    // this does not always pass on the exit code to the Manx compiler. why?
    getout(eap->addr_count > 0 ? (පූර්ණ_සංඛ්‍යා)eap->line2 : EXIT_FAILURE);
}

/*
 * Do preparations for "qall" and "wqall".
 * Returns FAIL when quitting should be aborted.
 */
    පූර්ණ_සංඛ්‍යා
before_quit_all(exarg_T *eap)
{
    නම් (cmdwin_type != 0)
    {
	නම් (eap->forceit)
	    cmdwin_result = K_XF1;	// ex_window() takes care of this
	නැතහොත්
	    cmdwin_result = K_XF2;
	ආපසු FAIL;
    }

    // Don't quit while editing the command line.
    නම් (text_locked())
    {
	text_locked_msg();
	ආපසු FAIL;
    }

    නම් (before_quit_autocmds(curwin, TRUE, eap->forceit))
	ආපසු FAIL;

    ආපසු OK;
}

/*
 * ":qall": try to quit all windows
 */
    static void
ex_quit_all(exarg_T *eap)
{
    නම් (before_quit_all(eap) == FAIL)
	ආපසු;
    පූර්ණ_සංඛ්‍යා save_exiting = exiting;
    exiting = TRUE;
    නම් (eap->forceit || !check_changed_any(FALSE, FALSE))
	getout(0);
    not_exiting(save_exiting);
}

/*
 * ":close": close current window, unless it is the last one
 */
    static void
ex_close(exarg_T *eap)
{
    win_T	*win;
    පූර්ණ_සංඛ්‍යා		winnr = 0;
    නම් (cmdwin_type != 0)
	cmdwin_result = Ctrl_C;
    නැතහොත්
	නම් (!text_locked() && !curbuf_locked())
	{
	    නම් (eap->addr_count == 0)
		ex_win_close(eap->forceit, curwin, NULL);
	    නැතහොත්
	    {
		FOR_ALL_WINDOWS(win)
		{
		    winnr++;
		    නම් (winnr == eap->line2)
			bha_nga;
		}
		නම් (win == NULL)
		    win = lastwin;
		ex_win_close(eap->forceit, win, NULL);
	    }
	}
}

#ifdef FEAT_QUICKFIX
/*
 * ":pclose": Close any preview window.
 */
    static void
ex_pclose(exarg_T *eap)
{
    win_T	*win;

    // First close any normal window.
    FOR_ALL_WINDOWS(win)
	නම් (win->w_p_pvw)
	{
	    ex_win_close(eap->forceit, win, NULL);
	    ආපසු;
	}
# ifdef FEAT_PROP_POPUP
    // Also when 'previewpopup' is empty, it might have been cleared.
    popup_close_preview();
# endif
}
#endif

/*
 * Close window "win" and take care of handling closing the last window for a
 * modified buffer.
 */
    static void
ex_win_close(
    පූර්ණ_සංඛ්‍යා		forceit,
    win_T	*win,
    tabpage_T	*tp)		// NULL or the tab page "win" is in
{
    පූර්ණ_සංඛ්‍යා		need_hide;
    buf_T	*buf = win->w_buffer;

    // Never close the autocommand window.
    නම් (is_aucmd_win(win))
    {
	emsg(_(e_cannot_close_autocmd_or_popup_window));
	ආපසු;
    }
    නම් (window_layout_locked(CMD_close))
	ආපසු;

    need_hide = (bufIsChanged(buf) && buf->b_nwindows <= 1);
    නම් (need_hide && !buf_hide(buf) && !forceit)
    {
#නම් defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
	නම් ((p_confirm || (cmdmod.cmod_flags & CMOD_CONFIRM)) && p_write)
	{
# ifdef FEAT_TERMINAL
	    නම් (term_job_running(buf->b_term))
	    {
		නම් (term_confirm_stop(buf) == FAIL)
		    ආපසු;
		// Manually kill the terminal here because this command will
		// hide it otherwise.
		free_terminal(buf);
		need_hide = FALSE;
	    }
	    නැතහොත්
# endif
	    {
		bufref_T bufref;

		set_bufref(&bufref, buf);
		dialog_changed(buf, FALSE);
		නම් (bufref_valid(&bufref) && bufIsChanged(buf))
		    ආපසු;
		need_hide = FALSE;
	    }
	}
	නැතහොත්
#endif
	{
	    no_write_message();
	    ආපසු;
	}
    }

#ifdef FEAT_GUI
    need_mouse_correct = TRUE;
#endif

    // free buffer when not hiding it or when it's a scratch buffer
    නම් (tp == NULL)
	win_close(win, !need_hide && !buf_hide(buf));
    නැතහොත්
	win_close_othertab(win, !need_hide && !buf_hide(buf), tp);
}

/*
 * Handle the argument for a tabpage related ex command.
 * Returns a tabpage number.
 * When an error is encountered then eap->errmsg is set.
 */
    static පූර්ණ_සංඛ්‍යා
get_tabpage_arg(exarg_T *eap)
{
    පූර්ණ_සංඛ්‍යා tab_number;
    පූර්ණ_සංඛ්‍යා unaccept_arg0 = (eap->cmdidx == CMD_tabmove) ? 0 : 1;

    නම් (eap->arg && *eap->arg != NUL)
    {
	char_u *p = eap->arg;
	char_u *p_save;
	පූර්ණ_සංඛ්‍යා    relative = 0; // argument +N/-N means: go to N places to the
			     // right/left relative to the current position.

	නම් (*p == '-')
	{
	    relative = -1;
	    p++;
	}
	නැතහොත් නම් (*p == '+')
	{
	    relative = 1;
	    p++;
	}

	p_save = p;
	tab_number = getdigits(&p);

	නම් (relative == 0)
	{
	    නම් (STRCMP(p, "$") == 0)
		tab_number = LAST_TAB_NR;
	    නැතහොත් නම් (STRCMP(p, "#") == 0)
		නම් (valid_tabpage(lastused_tabpage))
		    tab_number = tabpage_index(lastused_tabpage);
		නැතහොත්
		{
		    eap->errmsg = ex_errmsg(e_invalid_value_for_argument_str,
								     eap->arg);
		    tab_number = 0;
		    goto theend;
		}
	    නැතහොත් නම් (p == p_save || *p_save == '-' || *p != NUL
		    || tab_number > LAST_TAB_NR)
	    {
		// No numbers as argument.
		eap->errmsg = ex_errmsg(e_invalid_argument_str, eap->arg);
		goto theend;
	    }
	}
	නැතහොත්
	{
	    නම් (*p_save == NUL)
		tab_number = 1;
	    නැතහොත් නම් (p == p_save || *p_save == '-' || *p != NUL
		    || tab_number == 0)
	    {
		// No numbers as argument.
		eap->errmsg = ex_errmsg(e_invalid_argument_str, eap->arg);
		goto theend;
	    }
	    tab_number = tab_number * relative + tabpage_index(curtab);
	    නම් (!unaccept_arg0 && relative == -1)
		--tab_number;
	}
	නම් (tab_number < unaccept_arg0 || tab_number > LAST_TAB_NR)
	    eap->errmsg = ex_errmsg(e_invalid_argument_str, eap->arg);
    }
    නැතහොත් නම් (eap->addr_count > 0)
    {
	නම් (unaccept_arg0 && eap->line2 == 0)
	{
	    eap->errmsg = _(e_invalid_range);
	    tab_number = 0;
	}
	නැතහොත්
	{
	    tab_number = eap->line2;
	    නම් (!unaccept_arg0)
	    {
		char_u *cmdp = eap->cmd;

		අතරතුර (--cmdp > *eap->cmdlinep
			&& (VIM_ISWHITE(*cmdp) || VIM_ISDIGIT(*cmdp)))
		    ;
		නම් (*cmdp == '-')
		{
		    --tab_number;
		    නම් (tab_number < unaccept_arg0)
			eap->errmsg = _(e_invalid_range);
		}
	    }
	}
    }
    නැතහොත්
    {
	switch (eap->cmdidx)
	{
	pak_sa CMD_tabnext:
	    tab_number = tabpage_index(curtab) + 1;
	    නම් (tab_number > LAST_TAB_NR)
		tab_number = 1;
	    bha_nga;
	pak_sa CMD_tabmove:
	    tab_number = LAST_TAB_NR;
	    bha_nga;
	default:
	    tab_number = tabpage_index(curtab);
	}
    }

theend:
    ආපසු tab_number;
}

/*
 * ":tabclose": close current tab page, unless it is the last one.
 * ":tabclose N": close tab page N.
 */
    static void
ex_tabclose(exarg_T *eap)
{
    tabpage_T	*tp;
    පූර්ණ_සංඛ්‍යා		tab_number;

    නම් (cmdwin_type != 0)
    {
	cmdwin_result = K_IGNORE;
	ආපසු;
    }

    නම් (first_tabpage->tp_next == NULL)
    {
	emsg(_(e_cannot_close_last_tab_page));
	ආපසු;
    }

    නම් (window_layout_locked(CMD_tabclose))
	ආපසු;

    tab_number = get_tabpage_arg(eap);
    නම් (eap->errmsg != NULL)
	ආපසු;

    tp = find_tabpage(tab_number);
    නම් (tp == NULL)
    {
	beep_flush();
	ආපසු;
    }
    නම් (tp != curtab)
    {
	tabpage_close_other(tp, eap->forceit);
	ආපසු;
    }
    නැතහොත් නම් (!text_locked() && !curbuf_locked())
	tabpage_close(eap->forceit);
}

/*
 * ":tabonly": close all tab pages except the current one
 */
    static void
ex_tabonly(exarg_T *eap)
{
    tabpage_T	*tp;
    පූර්ණ_සංඛ්‍යා		done;
    පූර්ණ_සංඛ්‍යා		tab_number;

    නම් (cmdwin_type != 0)
    {
	cmdwin_result = K_IGNORE;
	ආපසු;
    }

    නම් (first_tabpage->tp_next == NULL)
    {
	msg(_("Already only one tab page"));
	ආපසු;
    }

    නම් (window_layout_locked(CMD_tabonly))
	ආපසු;

    tab_number = get_tabpage_arg(eap);
    නම් (eap->errmsg != NULL)
	ආපසු;

    goto_tabpage(tab_number);
    // Repeat this up to a 1000 times, because autocommands may
    // mess up the lists.
    සඳහා (done = 0; done < 1000; ++done)
    {
	FOR_ALL_TABPAGES(tp)
	    නම් (tp->tp_topframe != topframe)
	    {
		tabpage_close_other(tp, eap->forceit);
		// if we failed to close it quit
		නම් (valid_tabpage(tp))
		    done = 1000;
		// start over, "tp" is now invalid
		bha_nga;
	    }
	නම් (first_tabpage->tp_next == NULL)
	    bha_nga;
    }
}

/*
 * Close the current tab page.
 */
    void
tabpage_close(පූර්ණ_සංඛ්‍යා forceit)
{
    නම් (window_layout_locked(CMD_tabclose))
	ආපසු;

    trigger_tabclosedpre(curtab);
    curtab->tp_did_tabclosedpre = TRUE;
    tabpage_T *save_curtab = curtab;

    // First close all the windows but the current one.  If that worked then
    // close the last window in this tab, that will close it.
    නම් (!ONE_WINDOW)
	close_others(TRUE, forceit);
    නම් (ONE_WINDOW)
	ex_win_close(forceit, curwin, NULL);
    නම් (curtab == save_curtab)
	// When closing the tab page failed, reset tp_did_tabclosedpre so that
	// TabClosedPre behaves consistently on next :close vs :tabclose.
	curtab->tp_did_tabclosedpre = FALSE;
#ifdef FEAT_GUI
    need_mouse_correct = TRUE;
#endif
}

/*
 * Close tab page "tp", which is not the current tab page.
 * Note that autocommands may make "tp" invalid.
 * Also takes care of the tab pages line disappearing when closing the
 * last-but-one tab page.
 */
    void
tabpage_close_other(tabpage_T *tp, පූර්ණ_සංඛ්‍යා forceit)
{
    පූර්ණ_සංඛ්‍යා		done = 0;
    win_T	*wp;

    නම් (window_layout_locked(CMD_SIZE))
	ආපසු;

    trigger_tabclosedpre(tp);
    tp->tp_did_tabclosedpre = TRUE;

    // Limit to 1000 windows, autocommands may add a window while we close
    // one.  OK, so I'm paranoid...
    අතරතුර (++done < 1000)
    {
	wp = tp->tp_firstwin;
	ex_win_close(forceit, wp, tp);

	// Autocommands may delete the tab page under our fingers.
	නම් (!valid_tabpage(tp))
	    bha_nga;
	// We may fail to close a window with a modified buffer.
	නම් (tp->tp_firstwin == wp)
	{
	    done = 1000;
	    bha_nga;
	}
    }
    නම් (done >= 1000)
    {
	// When closing the tab page failed, reset tp_did_tabclosedpre so that
	// TabClosedPre behaves consistently on next :close vs :tabclose.
	tp->tp_did_tabclosedpre = FALSE;
	ආපසු;
    }

    apply_autocmds(EVENT_TABCLOSED, NULL, NULL, FALSE, curbuf);
}

/*
 * ":only".
 */
    static void
ex_only(exarg_T *eap)
{
    නම් (window_layout_locked(CMD_only))
	ආපසු;
#ifdef FEAT_GUI
    need_mouse_correct = TRUE;
#endif
    නම් (eap->addr_count > 0)
    {
	win_T   *wp;
	පූර්ණ_සංඛ්‍යා	wnr = eap->line2;
	සඳහා (wp = firstwin; --wnr > 0; )
	{
	    නම් (wp->w_next == NULL)
		bha_nga;
	    නැතහොත්
		wp = wp->w_next;
	}
	win_goto(wp);
    }
    close_others(TRUE, eap->forceit);
}

    static void
ex_hide(exarg_T *eap UNUSED)
{
    // ":hide" or ":hide | cmd": hide current window
    නම් (eap->skip)
	ආපසු;

    නම් (window_layout_locked(CMD_hide))
	ආපසු;
#ifdef FEAT_GUI
    need_mouse_correct = TRUE;
#endif
    නම් (eap->addr_count == 0)
	win_close(curwin, FALSE);	// don't free buffer
    නැතහොත්
    {
	පූර්ණ_සංඛ්‍යා	winnr = 0;
	win_T	*win;

	FOR_ALL_WINDOWS(win)
	{
	    winnr++;
	    නම් (winnr == eap->line2)
		bha_nga;
	}
	නම් (win == NULL)
	    win = lastwin;
	win_close(win, FALSE);
    }
}

/*
 * ":stop" and ":suspend": Suspend Vim.
 */
    void
ex_stop(exarg_T *eap)
{
    /*
     * Disallow suspending for "rvim".
     */
    නම් (check_restricted())
	ආපසු;

    නම් (!eap->forceit)
	autowrite_all();
    apply_autocmds(EVENT_VIMSUSPEND, NULL, NULL, FALSE, NULL);
    windgoto((පූර්ණ_සංඛ්‍යා)Rows - 1, cmdline_col_off);
    out_char('\n');
    out_flush();
    stoptermcap();
    out_flush();		// needed for SUN to restore xterm buffer
    mch_restore_title(SAVE_RESTORE_BOTH);	// restore window titles
    ui_suspend();		// call machine specific function
    maketitle();
    resettitle();		// force updating the title
    starttermcap();
    scroll_start();		// scroll screen before redrawing
    redraw_later_clear();
    shell_resized();	// may have resized window
    apply_autocmds(EVENT_VIMRESUME, NULL, NULL, FALSE, NULL);
}

/*
 * ":exit", ":xit" and ":wq": Write file and quit the current window.
 */
    static void
ex_exit(exarg_T *eap)
{
#ifdef FEAT_EVAL
    නම් (not_in_vim9(eap, false) == FAIL)
	ආපසු;
#endif
    නම් (cmdwin_type != 0)
    {
	cmdwin_result = Ctrl_C;
	ආපසු;
    }
    // Don't quit while editing the command line.
    නම් (text_locked())
    {
	text_locked_msg();
	ආපසු;
    }

    පූර්ණ_සංඛ්‍යා save_exiting = exiting;
    /*
     * we plan to exit if there is only one relevant window
     */
    නම් (check_more(FALSE, eap->forceit) == OK && only_one_window())
	exiting = TRUE;

    // Write the buffer for ":wq" or when it was changed.
    // Trigger QuitPre and ExitPre.
    // Check if we can exit now, after autocommands have changed things.
    නම් (((eap->cmdidx == CMD_wq || curbufIsChanged()) && do_write(eap) == FAIL)
	    || before_quit_autocmds(curwin, FALSE, eap->forceit)
	    || check_more(TRUE, eap->forceit) == FAIL
	    || (only_one_window() && check_changed_any(eap->forceit, FALSE)))
    {
	not_exiting(save_exiting);
    }
    නැතහොත්
    {
	නම් (only_one_window())	    // quit last window, exit Vim
	    getout(0);
	not_exiting(save_exiting);
#ifdef FEAT_GUI
	need_mouse_correct = TRUE;
#endif
	// Quit current window, may free the buffer.
	win_close(curwin, !buf_hide(curwin->w_buffer));
    }
}

/*
 * ":print", ":list", ":number".
 */
    static void
ex_print(exarg_T *eap)
{
    නම් (curbuf->b_ml.ml_flags & ML_EMPTY)
	emsg(_(e_empty_buffer));
    නැතහොත්
    {
	සඳහා ( ;!got_int; ui_breakcheck())
	{
	    print_line(eap->line1,
		    (eap->cmdidx == CMD_number || eap->cmdidx == CMD_pound
						 || (eap->flags & EXFLAG_NR)),
		    eap->cmdidx == CMD_list || (eap->flags & EXFLAG_LIST));
	    නම් (++eap->line1 > eap->line2)
		bha_nga;
	    out_flush();	    // show one line at a time
	}
	setpcmark();
	// put cursor at last line
	curwin->w_cursor.lnum = eap->line2;
	beginline(BL_SOL | BL_FIX);
    }

    ex_no_reprint = TRUE;
}

#ifdef FEAT_BYTEOFF
    static void
ex_goto(exarg_T *eap)
{
    goto_byte(eap->line2);
}
#endif

/*
 * ":shell".
 */
    static void
ex_shell(exarg_T *eap UNUSED)
{
    do_shell(NULL, 0);
}

#නම් defined(HAVE_DROP_FILE)

static පූර්ණ_සංඛ්‍යා drop_busy = FALSE;
static පූර්ණ_සංඛ්‍යා drop_filec;
static char_u **drop_filev = NULL;
static පූර්ණ_සංඛ්‍යා drop_split;
static void (*drop_callback)(void *);
static void *drop_cookie;

    static void
handle_drop_internal(void)
{
    exarg_T	ea;
    පූර්ණ_සංඛ්‍යා		save_msg_scroll = msg_scroll;

    // Setting the argument list may cause screen updates and being called
    // recursively.  Avoid that by setting drop_busy.
    drop_busy = TRUE;

    // Check whether the current buffer is changed. If so, we will need
    // to split the current window or data could be lost.
    // We don't need to check if the 'hidden' option is set, as in this
    // case the buffer won't be lost.
    නම් (!buf_hide(curbuf) && !drop_split)
    {
	++emsg_off;
	drop_split = check_changed(curbuf, CCGD_AW);
	--emsg_off;
    }
    නම් (drop_split)
    {
	නම් (win_split(0, 0) == FAIL)
	    ආපසු;
	RESET_BINDING(curwin);

	// When splitting the window, create a new alist.  Otherwise the
	// existing one is overwritten.
	alist_unlink(curwin->w_alist);
	alist_new();
    }

    /*
     * Set up the new argument list.
     */
    alist_set(ALIST(curwin), drop_filec, drop_filev, FALSE, NULL, 0);

    /*
     * Move to the first file.
     */
    // Fake up a minimal "next" command for do_argfile()
    CLEAR_FIELD(ea);
    ea.cmd = (char_u *)"next";
    do_argfile(&ea, 0);

    // do_ecmd() may set need_start_insertmode, but since we never left Insert
    // mode that is not needed here.
    need_start_insertmode = FALSE;

    // Restore msg_scroll, otherwise a following command may cause scrolling
    // unexpectedly.  The screen will be redrawn by the caller, thus
    // msg_scroll being set by displaying a message is irrelevant.
    msg_scroll = save_msg_scroll;

    නම් (drop_callback != NULL)
	drop_callback(drop_cookie);

    drop_filev = NULL;
    drop_busy = FALSE;
}

/*
 * Handle a file drop. The code is here because a drop is *nearly* like an
 * :args command, but not quite (we have a list of exact filenames, so we
 * don't want to (a) parse a command line, or (b) expand wildcards). So the
 * code is very similar to :args and hence needs access to a lot of the static
 * functions in this file.
 *
 * The "filev" list must have been allocated using alloc(), as should each item
 * in the list. This function takes over responsibility for freeing the "filev"
 * list.
 */
    void
handle_drop(
    පූර්ණ_සංඛ්‍යා		filec,		// the number of files dropped
    char_u	**filev,	// the list of files dropped
    පූර්ණ_සංඛ්‍යා		split,		// force splitting the window
    void	(*callback)(void *), // to be called after setting the argument
				     // list
    void	*cookie)	// argument for "callback" (allocated)
{
    // Cannot handle recursive drops, finish the pending one.
    නම් (drop_busy)
    {
	FreeWild(filec, filev);
	vim_free(cookie);
	ආපසු;
    }

    // When calling handle_drop() more than once in a row we only use the last
    // one.
    නම් (drop_filev != NULL)
    {
	FreeWild(drop_filec, drop_filev);
	vim_free(drop_cookie);
    }

    drop_filec = filec;
    drop_filev = filev;
    drop_split = split;
    drop_callback = callback;
    drop_cookie = cookie;

    // Postpone this when:
    // - editing the command line
    // - not possible to change the current buffer
    // - updating the screen
    // As it may change buffers and window structures that are in use and cause
    // freed memory to be used.
    නම් (text_locked() || curbuf_locked() || updating_screen)
	ආපසු;

    handle_drop_internal();
}

/*
 * To be called when text is unlocked, curbuf is unlocked or updating_screen is
 * reset: Handle a postponed drop.
 */
    void
handle_any_postponed_drop(void)
{
    නම් (!drop_busy && drop_filev != NULL
	     && !text_locked() && !curbuf_locked() && !updating_screen)
	handle_drop_internal();
}
#endif

/*
 * ":preserve".
 */
    static void
ex_preserve(exarg_T *eap UNUSED)
{
    curbuf->b_flags |= BF_PRESERVED;
    ml_preserve(curbuf, TRUE);
}

/*
 * ":recover".
 */
    static void
ex_recover(exarg_T *eap)
{
    // Set recoverymode right away to avoid the ATTENTION prompt.
    recoverymode = TRUE;
    නම් (!check_changed(curbuf, (p_awa ? CCGD_AW : 0)
			     | CCGD_MULTWIN
			     | (eap->forceit ? CCGD_FORCEIT : 0)
			     | CCGD_EXCMD)

	    && (*eap->arg == NUL
			     || setfname(curbuf, eap->arg, NULL, TRUE) == OK))
	ml_recover(TRUE);
    recoverymode = FALSE;
}

/*
 * Command modifier used in a wrong way.  Also for other commands that can't
 * appear at the toplevel.
 */
    static void
ex_wrongmodifier(exarg_T *eap)
{
    eap->errmsg = ex_errmsg(e_invalid_command_str, eap->cmd);
}

#නම් defined(FEAT_EVAL)

// callback function for 'findfunc'
static callback_T ffu_cb;

    static callback_T *
get_findfunc_callback(void)
{
    ආපසු *curbuf->b_p_ffu != NUL ? &curbuf->b_ffu_cb : &ffu_cb;
}

/*
 * Call 'findfunc' to obtain a list of file names.
 */
    static list_T *
call_findfunc(char_u *pat, පූර්ණ_සංඛ්‍යා cmdcomplete)
{
    typval_T	args[3];
    callback_T	*cb;
    typval_T	rettv;
    පූර්ණ_සංඛ්‍යා		retval;
    sctx_T	saved_sctx = current_sctx;
    sctx_T	*ctx;

    args[0].v_type = VAR_STRING;
    args[0].vval.v_string = pat;
    args[1].v_type = VAR_BOOL;
    args[1].vval.v_number = cmdcomplete;
    args[2].v_type = VAR_UNKNOWN;

    // Lock the text to prevent weird things from happening.  Also disallow
    // switching to another window, it should not be needed and may end up in
    // Insert mode in another buffer.
    ++textlock;

    ctx = get_option_sctx("findfunc");
    නම් (ctx != NULL)
	current_sctx = *ctx;

    cb = get_findfunc_callback();
    retval = call_callback(cb, -1, &rettv, 2, args);

    current_sctx = saved_sctx;

    --textlock;

    list_T *retlist = NULL;

    නම් (retval == OK)
    {
	නම් (rettv.v_type == VAR_LIST)
	    retlist = list_copy(rettv.vval.v_list, FALSE, FALSE, get_copyID());
	නැතහොත්
	    emsg(_(e_invalid_return_type_from_findfunc));

	clear_tv(&rettv);
    }

    ආපසු retlist;
}

/*
 * Find file names matching "pat" using 'findfunc' and return it in "files".
 * Used for expanding the :find, :sfind and :tabfind command argument.
 * Returns OK on success and FAIL otherwise.
 */
    පූර්ණ_සංඛ්‍යා
expand_findfunc(expand_T *xp, char_u *pat, char_u ***files, පූර්ණ_සංඛ්‍යා *numMatches)
{
    list_T	*l;
    පූර්ණ_සංඛ්‍යා		len;

    *numMatches = 0;
    *files = NULL;

    l = call_findfunc(pat, VVAL_TRUE);

    නම් (l == NULL)
	ආපසු FAIL;

    len = list_len(l);
    නම් (len == 0)	    // empty List
    {
	list_free(l);
	ආපසු FAIL;
    }

    expand_process_user_list(l, files, numMatches, xp);
    list_free(l);

    ආපසු OK;
}

/*
 * Use 'findfunc' to find file 'findarg'.  The 'count' argument is used to find
 * the n'th matching file.
 */
    static char_u *
findfunc_find_file(char_u *findarg, පූර්ණ_සංඛ්‍යා findarg_len, පූර්ණ_සංඛ්‍යා count)
{
    list_T	*fname_list;
    char_u	*ret_fname = NULL;
    char_u	cc;
    පූර්ණ_සංඛ්‍යා		fname_count;

    cc = findarg[findarg_len];
    findarg[findarg_len] = NUL;

    fname_list = call_findfunc(findarg, VVAL_FALSE);
    fname_count = list_len(fname_list);

    නම් (fname_count == 0)
	semsg(_(e_cant_find_file_str_in_path), findarg);
    නැතහොත්
    {
	නම් (count > fname_count)
	    semsg(_(e_no_more_file_str_found_in_path), findarg);
	නැතහොත්
	{
	    listitem_T *li = list_find(fname_list, count - 1);

	    නම් (li != NULL)
	    {
		typval_T *tv = &li->li_tv;

		නම් (tv->v_type == VAR_STRING && tv->vval.v_string != NULL)
		    ret_fname = vim_strsave(tv->vval.v_string);
		නැතහොත් නම් (tv->v_type == VAR_DICT && tv->vval.v_dict != NULL)
		    ret_fname = dict_get_string(tv->vval.v_dict, "word", TRUE);
	    }
	}
    }

    නම් (fname_list != NULL)
	list_free(fname_list);

    findarg[findarg_len] = cc;

    ආපසු ret_fname;
}

/*
 * Process the 'findfunc' option value.
 * Returns NULL on success and an error message on failure.
 */
    var_na *
did_set_findfunc(optset_T *args UNUSED)
{
    පූර්ණ_සංඛ්‍යා	retval;

    නම් (args->os_flags & OPT_LOCAL)
	// buffer-local option set
	retval = option_set_callback_func(curbuf->b_p_ffu, &curbuf->b_ffu_cb);
    නැතහොත්
    {
	// global option set
	retval = option_set_callback_func(p_ffu, &ffu_cb);
	// when using :set, free the local callback
	නම් (!(args->os_flags & OPT_GLOBAL))
	    free_callback(&curbuf->b_ffu_cb);
    }

    නම් (retval == FAIL)
	ආපසු e_invalid_argument;

    // If the option value starts with <SID> or s:, then replace that with
    // the script identifier.
    char_u	**varp = (char_u **)args->os_varp;
    char_u	*name = get_scriptlocal_funcname(*varp);
    නම් (name != NULL)
    {
	free_string_option(*varp);
	*varp = name;
    }

    ආපසු NULL;
}

# නම් defined(EXITFREE)
    void
free_findfunc_option(void)
{
    free_callback(&ffu_cb);
}
# endif

/*
 * Mark the global 'findfunc' callback with "copyID" so that it is not
 * garbage collected.
 */
    පූර්ණ_සංඛ්‍යා
set_ref_in_findfunc(පූර්ණ_සංඛ්‍යා copyID UNUSED)
{
    පූර්ණ_සංඛ්‍යා abort = FALSE;
    abort = set_ref_in_callback(&ffu_cb, copyID);
    ආපසු abort;
}
#endif

/*
 * :sview [+command] file	split window with new file, read-only
 * :split [[+command] file]	split window with current or new file
 * :vsplit [[+command] file]	split window vertically with current or new file
 * :new [[+command] file]	split window with no or new file
 * :vnew [[+command] file]	split vertically window with no or new file
 * :sfind [+command] file	split window with file in 'path'
 *
 * :tabedit			open new Tab page with empty window
 * :tabedit [+command] file	open new Tab page and edit "file"
 * :tabnew [[+command] file]	just like :tabedit
 * :tabfind [+command] file	open new Tab page and find "file"
 */
    void
ex_splitview(exarg_T *eap)
{
    win_T	*old_curwin = curwin;
    char_u	*fname = NULL;
#ifdef FEAT_BROWSE
    char_u	dot_path[] = ".";
    පූර්ණ_සංඛ්‍යා		save_cmod_flags = cmdmod.cmod_flags;
#endif
    පූර්ණ_සංඛ්‍යා		use_tab = eap->cmdidx == CMD_tabedit
		       || eap->cmdidx == CMD_tabfind
		       || eap->cmdidx == CMD_tabnew;

    නම් (ERROR_IF_ANY_POPUP_WINDOW)
	ආපසු;

#ifdef FEAT_GUI
    need_mouse_correct = TRUE;
#endif

#ifdef FEAT_QUICKFIX
    // A ":split" in the quickfix window works like ":new".  Don't want two
    // quickfix windows.  But it's OK when doing ":tab split".
    නම් (bt_quickfix(curbuf) && cmdmod.cmod_tab == 0)
    {
	නම් (eap->cmdidx == CMD_split)
	    eap->cmdidx = CMD_new;
	නම් (eap->cmdidx == CMD_vsplit)
	    eap->cmdidx = CMD_vnew;
    }
#endif

    නම් (eap->cmdidx == CMD_sfind || eap->cmdidx == CMD_tabfind)
    {
	char_u	*file_to_find = NULL;
	var_na	*search_ctx = NULL;

	නම් (*get_findfunc() != NUL)
	{
#ifdef FEAT_EVAL
	    fname = findfunc_find_file(eap->arg, (පූර්ණ_සංඛ්‍යා)STRLEN(eap->arg),
				       eap->addr_count > 0 ? eap->line2 : 1);
#endif
	}
	නැතහොත්
	{
	    fname = find_file_in_path(eap->arg, (පූර්ණ_සංඛ්‍යා)STRLEN(eap->arg),
					      FNAME_MESS, TRUE, curbuf->b_ffname,
					      &file_to_find, &search_ctx);
	    vim_free(file_to_find);
	    vim_findfile_cleanup(search_ctx);
	}
	නම් (fname == NULL)
	    goto theend;
	eap->arg = fname;
    }
#ifdef FEAT_BROWSE
    නැතහොත් නම් ((cmdmod.cmod_flags & CMOD_BROWSE)
	    && eap->cmdidx != CMD_vnew
	    && eap->cmdidx != CMD_new)
    {
	නම් (
# ifdef FEAT_GUI
	    !gui.in_use &&
# endif
		au_has_group((char_u *)"FileExplorer"))
	{
	    // No browsing supported but we do have the file explorer:
	    // Edit the directory.
	    නම් (*eap->arg == NUL || !mch_isdir(eap->arg))
		eap->arg = dot_path;
	}
	නැතහොත්
	{
	    fname = do_browse(0, (char_u *)(use_tab
			? _("Edit File in new tab page")
			: _("Edit File in new window")),
					  eap->arg, NULL, NULL, NULL, curbuf);
	    නම් (fname == NULL)
		goto theend;
	    eap->arg = fname;
	}
    }
    cmdmod.cmod_flags &= ~CMOD_BROWSE;	// Don't browse again in do_ecmd().
#endif

    /*
     * Either open new tab page or split the window.
     */
    නම් (use_tab)
    {
	නම් (win_new_tabpage(cmdmod.cmod_tab != 0 ? cmdmod.cmod_tab
			 : eap->addr_count == 0 ? 0
					       : (පූර්ණ_සංඛ්‍යා)eap->line2 + 1) != FAIL)
	{
	    do_exedit(eap, old_curwin);

	    // set the alternate buffer for the window we came from
	    නම් (curwin != old_curwin
		    && win_valid(old_curwin)
		    && old_curwin->w_buffer != curbuf
		    && (cmdmod.cmod_flags & CMOD_KEEPALT) == 0)
		old_curwin->w_alt_fnum = curbuf->b_fnum;
	}
    }
    නැතහොත් නම් (win_split(eap->addr_count > 0 ? (පූර්ණ_සංඛ්‍යා)eap->line2 : 0,
				     *eap->cmd == 'v' ? WSP_VERT : 0) != FAIL)
    {
	// Reset 'scrollbind' when editing another file, but keep it when
	// doing ":split" without arguments.
	නම් (*eap->arg != NUL)
	    RESET_BINDING(curwin);
	නැතහොත්
	    do_check_scrollbind(FALSE);
	do_exedit(eap, old_curwin);
    }

#ifdef FEAT_BROWSE
    cmdmod.cmod_flags = save_cmod_flags;
#endif

theend:
    vim_free(fname);
}

/*
 * Open a new tab page.
 */
    void
tabpage_new(void)
{
    exarg_T	ea;

    CLEAR_FIELD(ea);
    ea.cmdidx = CMD_tabnew;
    ea.cmd = (char_u *)"tabn";
    ea.arg = (char_u *)"";
    ex_splitview(&ea);
}

/*
 * :tabnext command
 */
    static void
ex_tabnext(exarg_T *eap)
{
    පූර්ණ_සංඛ්‍යා tab_number;

    නම් (ERROR_IF_POPUP_WINDOW)
	ආපසු;
    switch (eap->cmdidx)
    {
	pak_sa CMD_tabfirst:
	pak_sa CMD_tabrewind:
	    goto_tabpage(1);
	    bha_nga;
	pak_sa CMD_tablast:
	    goto_tabpage(9999);
	    bha_nga;
	pak_sa CMD_tabprevious:
	pak_sa CMD_tabNext:
	    නම් (eap->arg && *eap->arg != NUL)
	    {
		char_u *p = eap->arg;
		char_u *p_save = p;

		tab_number = getdigits(&p);
		නම් (p == p_save || *p_save == '-' || *p != NUL
			    || tab_number == 0)
		{
		    // No numbers as argument.
		    eap->errmsg = ex_errmsg(e_invalid_argument_str, eap->arg);
		    ආපසු;
		}
	    }
	    නැතහොත්
	    {
		නම් (eap->addr_count == 0)
		    tab_number = 1;
		නැතහොත්
		{
		    tab_number = eap->line2;
		    නම් (tab_number < 1)
		    {
			eap->errmsg = _(e_invalid_range);
			ආපසු;
		    }
		}
	    }
	    goto_tabpage(-tab_number);
	    bha_nga;
	default: // CMD_tabnext
	    tab_number = get_tabpage_arg(eap);
	    නම් (eap->errmsg == NULL)
		goto_tabpage(tab_number);
	    bha_nga;
    }
}

/*
 * :tabmove command
 */
    static void
ex_tabmove(exarg_T *eap)
{
    පූර්ණ_සංඛ්‍යා tab_number;

    tab_number = get_tabpage_arg(eap);
    නම් (eap->errmsg == NULL)
	tabpage_move(tab_number);
}

/*
 * :tabs command: List tabs and their contents.
 */
    static void
ex_tabs(exarg_T *eap UNUSED)
{
    tabpage_T	*tp;
    win_T	*wp;
    පූර්ණ_සංඛ්‍යා		tabcount = 1;

    msg_start();
    msg_scroll = TRUE;
    සඳහා (tp = first_tabpage; tp != NULL && !got_int; tp = tp->tp_next)
    {
	msg_putchar('\n');
	vim_snprintf((var_na *)IObuff, IOSIZE, _("Tab page %d"), tabcount++);
	msg_outtrans_attr(IObuff, HL_ATTR(HLF_T));
	out_flush();	    // output one line at a time
	ui_breakcheck();

	නම් (tp  == curtab)
	    wp = firstwin;
	නැතහොත්
	    wp = tp->tp_firstwin;
	සඳහා ( ; wp != NULL && !got_int; wp = wp->w_next)
	{
	    msg_putchar('\n');
	    msg_putchar(wp == curwin ? '>' : ' ');
	    msg_putchar(' ');
	    msg_putchar(bufIsChanged(wp->w_buffer) ? '+' : ' ');
	    msg_putchar(' ');
	    නම් (buf_spname(wp->w_buffer) != NULL)
		vim_strncpy(IObuff, buf_spname(wp->w_buffer), IOSIZE - 1);
	    නැතහොත්
		home_replace(wp->w_buffer, wp->w_buffer->b_fname,
							IObuff, IOSIZE, TRUE);
	    msg_outtrans(IObuff);
	    out_flush();	    // output one line at a time
	    ui_breakcheck();
	}
    }
}

/*
 * ":mode": Set screen mode.
 * If no argument given, just get the screen size and redraw.
 */
    static void
ex_mode(exarg_T *eap)
{
    නම් (*eap->arg == NUL)
	shell_resized();
    නැතහොත්
	emsg(_(e_screen_mode_setting_not_supported));
}

/*
 * ":resize".
 * set, increment or decrement current window height
 */
    static void
ex_resize(exarg_T *eap)
{
    පූර්ණ_සංඛ්‍යා		n;
    win_T	*wp = curwin;

    නම් (eap->addr_count > 0)
    {
	n = eap->line2;
	සඳහා (wp = firstwin; wp->w_next != NULL && --n > 0; wp = wp->w_next)
	    ;
    }

#ifdef FEAT_GUI
    need_mouse_correct = TRUE;
#endif
    n = atol((var_na *)eap->arg);
    නම් (cmdmod.cmod_split & WSP_VERT)
    {
	නම් (*eap->arg == '-' || *eap->arg == '+')
	    n += wp->w_width;
	නැතහොත් නම් (n == 0 && eap->arg[0] == NUL)	// default is very wide
	    n = 9999;
	win_setwidth_win(n, wp);
    }
    නැතහොත්
    {
	නම් (*eap->arg == '-' || *eap->arg == '+')
	    n += wp->w_height;
	නැතහොත් නම් (n == 0 && eap->arg[0] == NUL)	// default is very high
	    n = 9999;
	win_setheight_win(n, wp);
    }
}

/*
 * ":find [+command] <file>" command.
 */
    static void
ex_find(exarg_T *eap)
{
    නම් (!check_can_set_curbuf_forceit(eap->forceit))
	ආපසු;

    char_u	*fname = NULL;
    පූර්ණ_සංඛ්‍යා		count;
    char_u	*file_to_find = NULL;
    var_na	*search_ctx = NULL;

    නම් (*get_findfunc() != NUL)
    {
#ifdef FEAT_EVAL
	fname = findfunc_find_file(eap->arg, (පූර්ණ_සංඛ්‍යා)STRLEN(eap->arg),
					eap->addr_count > 0 ? eap->line2 : 1);
#endif
    }
    නැතහොත්
    {
	fname = find_file_in_path(eap->arg, (පූර්ණ_සංඛ්‍යා)STRLEN(eap->arg), FNAME_MESS,
			       TRUE, curbuf->b_ffname, &file_to_find, &search_ctx);
	නම් (eap->addr_count > 0)
	{
	    // Repeat finding the file "count" times.  This matters when it appears
	    // several times in the path.
	    count = eap->line2;
	    අතරතුර (fname != NULL && --count > 0)
	    {
		vim_free(fname);
		fname = find_file_in_path(NULL, 0, FNAME_MESS,
			      FALSE, curbuf->b_ffname, &file_to_find, &search_ctx);
	    }
	}
	VIM_CLEAR(file_to_find);
	vim_findfile_cleanup(search_ctx);
    }

    නම් (fname == NULL)
	ආපසු;

    eap->arg = fname;
    do_exedit(eap, NULL);
    vim_free(fname);
}

/*
 * ":open" simulation: for now works just like ":visual".
 */
    static void
ex_open(exarg_T *eap)
{
    regmatch_T	regmatch;
    char_u	*p;

#ifdef FEAT_EVAL
    නම් (not_in_vim9(eap, false) == FAIL)
	ආපසු;
#endif
    curwin->w_cursor.lnum = eap->line2;
    beginline(BL_SOL | BL_FIX);
    නම් (*eap->arg == '/')
    {
	// ":open /pattern/": put cursor in column found with pattern
	++eap->arg;
	p = skip_regexp(eap->arg, '/', magic_isset());
	*p = NUL;
	regmatch.regprog = vim_regcomp(eap->arg, magic_isset() ? RE_MAGIC : 0);
	නම් (regmatch.regprog != NULL)
	{
	    // make a copy of the line, when searching for a mark it might be
	    // flushed
	    char_u *line = vim_strsave(ml_get_curline());

	    regmatch.rm_ic = p_ic;
	    නම් (vim_regexec(&regmatch, line, (colnr_T)0))
		curwin->w_cursor.col = (colnr_T)(regmatch.startp[0] - line);
	    නැතහොත්
		emsg(_(e_no_match));
	    vim_regfree(regmatch.regprog);
	    vim_free(line);
	}
	// Move to the NUL, ignore any other arguments.
	eap->arg += STRLEN(eap->arg);
    }
    check_cursor();

    eap->cmdidx = CMD_visual;
    do_exedit(eap, NULL);
}

/*
 * ":edit", ":badd", ":balt", ":visual".
 */
    static void
ex_edit(exarg_T *eap)
{
    char_u *ffname = eap->cmdidx == CMD_enew ? NULL : eap->arg;

    // Exclude commands which keep the window's current buffer
    නම් (
	    eap->cmdidx != CMD_badd
	    && eap->cmdidx != CMD_balt
	    // All other commands must obey 'winfixbuf' / ! rules
	    && (is_other_file(0, ffname) && !check_can_set_curbuf_forceit(eap->forceit))
    )
	ආපසු;

    do_exedit(eap, NULL);
}

/*
 * ":edit <file>" command and alike.
 */
    void
do_exedit(
    exarg_T	*eap,
    win_T	*old_curwin)	    // curwin before doing a split or NULL
{
    පූර්ණ_සංඛ්‍යා		n;
    පූර්ණ_සංඛ්‍යා		need_hide;
    පූර්ණ_සංඛ්‍යා		exmode_was = exmode_active;

    නම් ((eap->cmdidx != CMD_pedit && ERROR_IF_POPUP_WINDOW)
						 || ERROR_IF_TERM_POPUP_WINDOW)
	ආපසු;
    /*
     * ":vi" command ends Ex mode.
     */
    නම් (exmode_active && (eap->cmdidx == CMD_visual
						|| eap->cmdidx == CMD_view))
    {
	exmode_active = FALSE;
	ex_pressedreturn = FALSE;
	නම් (*eap->arg == NUL)
	{
	    // Special case:  ":global/pat/visual\NLvi-commands"
	    නම් (global_busy)
	    {
		නම් (eap->nextcmd != NULL)
		{
		    stuffReadbuff(eap->nextcmd);
		    eap->nextcmd = NULL;
		}

		නම් (exmode_was != EXMODE_VIM)
		    settmode(TMODE_RAW);
		පූර්ණ_සංඛ්‍යා save_RedrawingDisabled = RedrawingDisabled;
		RedrawingDisabled = 0;
		පූර්ණ_සංඛ්‍යා save_nwr = no_wait_return;
		no_wait_return = 0;
		need_wait_return = FALSE;
		පූර්ණ_සංඛ්‍යා save_ms = msg_scroll;
		msg_scroll = 0;
#ifdef FEAT_GUI
		පූර්ණ_සංඛ්‍යා save_he = hold_gui_events;
		hold_gui_events = 0;
#endif
		set_must_redraw(UPD_CLEAR);
		pending_exmode_active = TRUE;

		main_loop(FALSE, TRUE);

		pending_exmode_active = FALSE;
		RedrawingDisabled = save_RedrawingDisabled;
		no_wait_return = save_nwr;
		msg_scroll = save_ms;
#ifdef FEAT_GUI
		hold_gui_events = save_he;
#endif
	    }
	    ආපසු;
	}
    }

    නම් ((eap->cmdidx == CMD_new
		|| eap->cmdidx == CMD_tabnew
		|| eap->cmdidx == CMD_tabedit
		|| eap->cmdidx == CMD_vnew) && *eap->arg == NUL)
    {
	// ":new" or ":tabnew" without argument: edit a new empty buffer
	setpcmark();
	(void)do_ecmd(0, NULL, NULL, eap, ECMD_ONE,
		      ECMD_HIDE + (eap->forceit ? ECMD_FORCEIT : 0),
		      old_curwin == NULL ? curwin : NULL);
    }
    නැතහොත් නම් ((eap->cmdidx != CMD_split && eap->cmdidx != CMD_vsplit)
	    || *eap->arg != NUL
#ifdef FEAT_BROWSE
	    || (cmdmod.cmod_flags & CMOD_BROWSE)
#endif
	    )
    {
	// Can't edit another file when "textlock" or "curbuf_lock" is set.
	// Only ":edit" or ":script" can bring us here, others are stopped
	// earlier.
	නම් (*eap->arg != NUL && text_or_buf_locked())
	    ආපසු;

	n = readonlymode;
	නම් (eap->cmdidx == CMD_view || eap->cmdidx == CMD_sview)
	    readonlymode = TRUE;
	නැතහොත් නම් (eap->cmdidx == CMD_enew)
	    readonlymode = FALSE;   // 'readonly' doesn't make sense in an
				    // empty buffer
	නම් (eap->cmdidx != CMD_balt && eap->cmdidx != CMD_badd)
	    setpcmark();
	නම් (do_ecmd(0, (eap->cmdidx == CMD_enew ? NULL : eap->arg),
		    NULL, eap,
		    // ":edit" goes to first line if Vi compatible
		    (*eap->arg == NUL && eap->do_ecmd_lnum == 0
				      && vim_strchr(p_cpo, CPO_GOTO1) != NULL)
					       ? ECMD_ONE : eap->do_ecmd_lnum,
		    (buf_hide(curbuf) ? ECMD_HIDE : 0)
		    + (eap->forceit ? ECMD_FORCEIT : 0)
		      // after a split we can use an existing buffer
		    + (old_curwin != NULL ? ECMD_OLDBUF : 0)
		    + (eap->cmdidx == CMD_badd ? ECMD_ADDBUF : 0)
		    + (eap->cmdidx == CMD_balt ? ECMD_ALTBUF : 0)
		    , old_curwin == NULL ? curwin : NULL) == FAIL)
	{
	    // Editing the file failed.  If the window was split, close it.
	    නම් (old_curwin != NULL)
	    {
		need_hide = (curbufIsChanged() && curbuf->b_nwindows <= 1);
		නම් (!need_hide || buf_hide(curbuf))
		{
#නම් defined(FEAT_EVAL)
		    cleanup_T   cs;

		    // Reset the error/interrupt/exception state here so that
		    // aborting() returns FALSE when closing a window.
		    enter_cleanup(&cs);
#endif
#ifdef FEAT_GUI
		    need_mouse_correct = TRUE;
#endif
		    win_close(curwin, !need_hide && !buf_hide(curbuf));

#නම් defined(FEAT_EVAL)
		    // Restore the error/interrupt/exception state if not
		    // discarded by a new aborting error, interrupt, or
		    // uncaught exception.
		    leave_cleanup(&cs);
#endif
		}
	    }
	}
	නැතහොත් නම් (readonlymode && curbuf->b_nwindows == 1)
	{
	    // When editing an already visited buffer, 'readonly' won't be set
	    // but the previous value is kept.  With ":view" and ":sview" we
	    // want the  file to be readonly, except when another window is
	    // editing the same buffer.
	    curbuf->b_p_ro = TRUE;
	}
	readonlymode = n;
    }
    නැතහොත්
    {
	නම් (eap->do_ecmd_cmd != NULL)
	    do_cmd_argument(eap->do_ecmd_cmd);
	n = curwin->w_arg_idx_invalid;
	check_arg_idx(curwin);
	නම් (n != curwin->w_arg_idx_invalid)
	    maketitle();
    }

    /*
     * if ":split file" worked, set alternate file name in old window to new
     * file
     */
    නම් (old_curwin != NULL
	    && *eap->arg != NUL
	    && curwin != old_curwin
	    && win_valid(old_curwin)
	    && old_curwin->w_buffer != curbuf
	    && (cmdmod.cmod_flags & CMOD_KEEPALT) == 0)
	old_curwin->w_alt_fnum = curbuf->b_fnum;

    ex_no_reprint = TRUE;
}

#ifndef FEAT_GUI
/*
 * ":gui" and ":gvim" when there is no GUI.
 */
    static void
ex_nogui(exarg_T *eap)
{
    eap->errmsg = _(e_gui_cannot_be_used_not_enabled_at_compile_time);
}
#endif

#නම් defined(FEAT_GUI_MSWIN) && defined(FEAT_MENU) && defined(FEAT_TEAROFF)
    static void
ex_tearoff(exarg_T *eap)
{
    gui_make_tearoff(eap->arg);
}
#endif

#නම් (defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_GTK) \
	|| defined(FEAT_TERM_POPUP_MENU)) && defined(FEAT_MENU)
    static void
ex_popup(exarg_T *eap)
{
# නම් defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_GTK)
    නම් (gui.in_use)
	gui_make_popup(eap->arg, eap->forceit);
#  ifdef FEAT_TERM_POPUP_MENU
    නැතහොත්
#  endif
# endif
# ifdef FEAT_TERM_POPUP_MENU
	pum_make_popup(eap->arg, eap->forceit);
# endif
}
#endif

    static void
ex_swapname(exarg_T *eap UNUSED)
{
    නම් (curbuf->b_ml.ml_mfp == NULL || curbuf->b_ml.ml_mfp->mf_fname == NULL)
	msg(_("No swap file"));
    නැතහොත්
	msg((var_na *)curbuf->b_ml.ml_mfp->mf_fname);
}

/*
 * ":syncbind" forces all 'scrollbind' windows to have the same relative
 * offset.
 * (1998-11-02 16:21:01  R. Edward Ralston <eralston@computer.org>)
 */
    static void
ex_syncbind(exarg_T *eap UNUSED)
{
    win_T	*wp;
    win_T	*save_curwin = curwin;
    buf_T	*save_curbuf = curbuf;
    long	topline;
    long	y;
    linenr_T	old_linenr = curwin->w_cursor.lnum;

    setpcmark();

    /*
     * determine max topline
     */
    නම් (curwin->w_p_scb)
    {
	topline = curwin->w_topline;
	FOR_ALL_WINDOWS(wp)
	{
	    නම් (wp->w_p_scb && wp->w_buffer)
	    {
		y = wp->w_buffer->b_ml.ml_line_count - get_scrolloff_value();
		නම් (topline > y)
		    topline = y;
	    }
	}
	නම් (topline < 1)
	    topline = 1;
    }
    නැතහොත්
    {
	topline = 1;
    }


    /*
     * Set all scrollbind windows to the same topline.
     */
    FOR_ALL_WINDOWS(curwin)
    {
	නම් (curwin->w_p_scb)
	{
	    curbuf = curwin->w_buffer;
	    y = topline - curwin->w_topline;
	    නම් (y > 0)
		scrollup(y, TRUE);
	    නැතහොත්
		scrolldown(-y, TRUE);
	    curwin->w_scbind_pos = topline;
	    redraw_later(UPD_VALID);
	    cursor_correct();
	    curwin->w_redr_status = true;
	}
    }
    curwin = save_curwin;
    curbuf = save_curbuf;
    නම් (curwin->w_p_scb)
    {
	did_syncbind = TRUE;
	checkpcmark();
	නම් (old_linenr != curwin->w_cursor.lnum)
	{
	    char_u ctrl_o[2];

	    ctrl_o[0] = Ctrl_O;
	    ctrl_o[1] = 0;
	    ins_typebuf(ctrl_o, REMAP_NONE, 0, TRUE, FALSE);
	}
    }
}


    static void
ex_read(exarg_T *eap)
{
    පූර්ණ_සංඛ්‍යා		i;
    පූර්ණ_සංඛ්‍යා		empty = (curbuf->b_ml.ml_flags & ML_EMPTY);
    linenr_T	lnum;

    නම් (eap->usefilter)			// :r!cmd
    {
	do_bang(1, eap, FALSE, FALSE, TRUE);
	ආපසු;
    }

    නම් (u_save(eap->line2, (linenr_T)(eap->line2 + 1)) == FAIL)
	ආපසු;

#ifdef FEAT_BROWSE
    නම් (cmdmod.cmod_flags & CMOD_BROWSE)
    {
	char_u *browseFile;

	browseFile = do_browse(0, (char_u *)_("Append File"), eap->arg,
		NULL, NULL, NULL, curbuf);
	නම් (browseFile != NULL)
	{
	    i = readfile(browseFile, NULL,
		    eap->line2, (linenr_T)0, (linenr_T)MAXLNUM, eap, 0);
	    vim_free(browseFile);
	}
	නැතහොත්
	    i = OK;
    }
    නැතහොත්
#endif
	නම් (*eap->arg == NUL)
	{
	    නම් (check_fname() == FAIL)	// check for no file name
		ආපසු;
	    i = readfile(curbuf->b_ffname, curbuf->b_fname,
		    eap->line2, (linenr_T)0, (linenr_T)MAXLNUM, eap, 0);
	}
	නැතහොත්
	{
	    නම් (vim_strchr(p_cpo, CPO_ALTREAD) != NULL)
		(void)setaltfname(eap->arg, eap->arg, (linenr_T)1);
	    i = readfile(eap->arg, NULL,
		    eap->line2, (linenr_T)0, (linenr_T)MAXLNUM, eap, 0);

	}
    නම් (i != OK)
    {
#නම් defined(FEAT_EVAL)
	නම් (!aborting())
#endif
	    semsg(_(e_cant_open_file_str), eap->arg);
    }
    නැතහොත්
    {
	නම් (empty && exmode_active)
	{
	    // Delete the empty line that remains.  Historically ex does
	    // this but vi doesn't.
	    නම් (eap->line2 == 0)
		lnum = curbuf->b_ml.ml_line_count;
	    නැතහොත්
		lnum = 1;
	    නම් (*ml_get(lnum) == NUL && u_savedel(lnum, 1L) == OK)
	    {
		ml_delete(lnum);
		නම් (curwin->w_cursor.lnum > 1
			&& curwin->w_cursor.lnum >= lnum)
		    --curwin->w_cursor.lnum;
		deleted_lines_mark(lnum, 1L);
	    }
	}
	redraw_curbuf_later(UPD_VALID);
    }
}

static char_u	*prev_dir = NULL;

#නම් defined(EXITFREE)
    void
free_cd_dir(void)
{
    VIM_CLEAR(prev_dir);
    VIM_CLEAR(globaldir);
}
#endif

/*
 * Get the previous directory for the given chdir scope.
 */
    static char_u *
get_prevdir(cdscope_T scope)
{
    නම් (scope == CDSCOPE_WINDOW)
	ආපසු curwin->w_prevdir;
    නැතහොත් නම් (scope == CDSCOPE_TABPAGE)
	ආපසු curtab->tp_prevdir;
    ආපසු prev_dir;
}

/*
 * Deal with the side effects of changing the current directory.
 * When 'scope' is CDSCOPE_TABPAGE then this was after an ":tcd" command.
 * When 'scope' is CDSCOPE_WINDOW then this was after an ":lcd" command.
 */
    void
post_chdir(cdscope_T scope)
{
    නම් (scope != CDSCOPE_WINDOW)
	// Clear tab local directory for both :cd and :tcd
	VIM_CLEAR(curtab->tp_localdir);
    VIM_CLEAR(curwin->w_localdir);
    නම් (scope != CDSCOPE_GLOBAL)
    {
	char_u	*pdir = get_prevdir(scope);

	// If still in the global directory, need to remember current
	// directory as the global directory.
	නම් (globaldir == NULL && pdir != NULL)
	    globaldir = vim_strsave(pdir);

	// Remember this local directory for the window.
	නම් (mch_dirname(NameBuff, MAXPATHL) == OK)
	{
	    නම් (scope == CDSCOPE_TABPAGE)
		curtab->tp_localdir = vim_strsave(NameBuff);
	    නැතහොත්
		curwin->w_localdir = vim_strsave(NameBuff);
	}
    }
    නැතහොත්
    {
	// We are now in the global directory, no need to remember its name.
	VIM_CLEAR(globaldir);
    }

    last_chdir_reason = NULL;
    shorten_fnames(vim_strchr(p_cpo, CPO_NOSYMLINKS) == NULL);
}

/*
 * Trigger DirChangedPre for "acmd_fname" with directory "new_dir".
 */
    void
trigger_DirChangedPre(char_u *acmd_fname, char_u *new_dir)
{
#ifdef FEAT_EVAL
    dict_T	    *v_event;
    save_v_event_T  save_v_event;

    v_event = get_v_event(&save_v_event);
    (void)dict_add_string(v_event, "directory", new_dir);
    dict_set_items_ro(v_event);
#endif
    apply_autocmds(EVENT_DIRCHANGEDPRE, acmd_fname, new_dir, FALSE, curbuf);
#ifdef FEAT_EVAL
    restore_v_event(v_event, &save_v_event);
#endif
}

/*
 * Change directory function used by :cd/:tcd/:lcd Ex commands and the
 * chdir() function.
 * scope == CDSCOPE_WINDOW: changes the window-local directory
 * scope == CDSCOPE_TABPAGE: changes the tab-local directory
 * Otherwise: changes the global directory
 * Returns TRUE if the directory is successfully changed.
 */
    පූර්ණ_සංඛ්‍යා
changedir_func(
	char_u		*new_dir,
	පූර්ණ_සංඛ්‍යා		forceit,
	cdscope_T	scope)
{
    char_u	*pdir = NULL;
    පූර්ණ_සංඛ්‍යා		dir_differs;
    char_u	*acmd_fname = NULL;
    char_u	**pp;
    char_u	*tofree;

    නම් (new_dir == NULL || allbuf_locked())
	ආපසු FALSE;

    නම් (vim_strchr(p_cpo, CPO_CHDIR) != NULL && curbufIsChanged() && !forceit)
    {
	emsg(_(e_cannot_change_directory_buffer_is_modified_add_bang_to_override));
	ආපසු FALSE;
    }

    // ":cd -": Change to previous directory
    නම් (STRCMP(new_dir, "-") == 0)
    {
	pdir = get_prevdir(scope);
	නම් (pdir == NULL)
	{
	    emsg(_(e_no_previous_directory));
	    ආපසු FALSE;
	}
	new_dir = pdir;
    }

    // Save current directory for next ":cd -"
    නම් (mch_dirname(NameBuff, MAXPATHL) == OK)
	pdir = vim_strsave(NameBuff);
    නැතහොත්
	pdir = NULL;

    // For UNIX ":cd" means: go to home directory.
    // On other systems too if 'cdhome' is set.
#නම් defined(UNIX) || defined(VMS)
    නම් (*new_dir == NUL)
#නැතහොත්
    නම් (*new_dir == NUL && p_cdh)
#endif
    {
	// use NameBuff for home directory name
#ifdef VMS
	char_u	*p;

	p = mch_getenv((char_u *)"SYS$LOGIN");
	නම් (p == NULL || *p == NUL)	// empty is the same as not set
	    NameBuff[0] = NUL;
	නැතහොත්
	    vim_strncpy(NameBuff, p, MAXPATHL - 1);
#නැතහොත්
	expand_env((char_u *)"$HOME", NameBuff, MAXPATHL);
#endif
	new_dir = NameBuff;
    }
    dir_differs = pdir == NULL
			    || pathcmp((var_na *)pdir, (var_na *)new_dir, -1) != 0;
    නම් (dir_differs)
    {
	නම් (scope == CDSCOPE_WINDOW)
	    acmd_fname = (char_u *)"window";
	නැතහොත් නම් (scope == CDSCOPE_TABPAGE)
	    acmd_fname = (char_u *)"tabpage";
	නැතහොත්
	    acmd_fname = (char_u *)"global";
	trigger_DirChangedPre(acmd_fname, new_dir);

	නම් (vim_chdir(new_dir))
	{
	    emsg(_(e_command_failed));
	    vim_free(pdir);
	    ආපසු FALSE;
	}
    }

    නම් (scope == CDSCOPE_WINDOW)
	pp = &curwin->w_prevdir;
    නැතහොත් නම් (scope == CDSCOPE_TABPAGE)
	pp = &curtab->tp_prevdir;
    නැතහොත්
	pp = &prev_dir;
    tofree = *pp;  // new_dir may use this
    *pp = pdir;

    post_chdir(scope);

    නම් (dir_differs)
	apply_autocmds(EVENT_DIRCHANGED, acmd_fname, new_dir, FALSE, curbuf);
    vim_free(tofree);
    ආපසු TRUE;
}

/*
 * ":cd", ":tcd", ":lcd", ":chdir" ":tchdir" and ":lchdir".
 */
    void
ex_cd(exarg_T *eap)
{
    char_u	*new_dir;

    new_dir = eap->arg;
#නම් !defined(UNIX) && !defined(VMS)
    // for non-UNIX ":cd" means: print current directory unless 'cdhome' is set
    නම් (*new_dir == NUL && !p_cdh)
    {
	ex_pwd(NULL);
	ආපසු;
    }
#endif

    cdscope_T	scope = CDSCOPE_GLOBAL;

    නම් (eap->cmdidx == CMD_lcd || eap->cmdidx == CMD_lchdir)
	scope = CDSCOPE_WINDOW;
    නැතහොත් නම් (eap->cmdidx == CMD_tcd || eap->cmdidx == CMD_tchdir)
	scope = CDSCOPE_TABPAGE;

    නම් (changedir_func(new_dir, eap->forceit, scope))
    {
	// Echo the new current directory if the command was typed.
	නම් (KeyTyped || p_verbose >= 5)
	    ex_pwd(eap);
    }
}

/*
 * ":pwd".
 */
    static void
ex_pwd(exarg_T *eap UNUSED)
{
    නම් (mch_dirname(NameBuff, MAXPATHL) == OK)
    {
#ifdef BACKSLASH_IN_FILENAME
	slash_adjust(NameBuff);
#endif
	නම් (p_verbose > 0)
	{
	    var_na *context = "global";

	    නම් (last_chdir_reason != NULL)
		context = last_chdir_reason;
	    නැතහොත් නම් (curwin->w_localdir != NULL)
		context = "window";
	    නැතහොත් නම් (curtab->tp_localdir != NULL)
		context = "tabpage";
	    smsg("[%s] %s", context, (var_na *)NameBuff);
	}
	නැතහොත්
	    msg((var_na *)NameBuff);
    }
    නැතහොත්
	emsg(_(e_directory_unknown));
}

/*
 * ":=".
 */
    static void
ex_equal(exarg_T *eap)
{
    smsg("%ld", (long)eap->line2);
    ex_may_print(eap);
}

    static void
ex_sleep(exarg_T *eap)
{
    පූර්ණ_සංඛ්‍යා		n;
    long	len;

    නම් (cursor_valid())
    {
	n = W_WINROW(curwin) + curwin->w_wrow - msg_scrolled;
	නම් (n >= 0)
	    windgoto(n, curwin->w_wincol + curwin->w_wcol);
    }

    len = eap->line2;
    switch (*eap->arg)
    {
	pak_sa 'm': bha_nga;
	pak_sa NUL: len *= 1000L; bha_nga;
	default: semsg(_(e_invalid_argument_str), eap->arg); ආපසු;
    }

    // Hide the cursor if invoked with !
    do_sleep(len, eap->forceit);
}

/*
 * Sleep for "msec" milliseconds, but keep checking for a CTRL-C every second.
 * Hide the cursor if "hide_cursor" is TRUE.
 */
    void
do_sleep(long msec, පූර්ණ_සංඛ්‍යා hide_cursor)
{
    long	done = 0;
    long	wait_now;
#ifdef ELAPSED_FUNC
    elapsed_T	start_tv;

    // Remember at what time we started, so that we know how much longer we
    // should wait after waiting for a bit.
    ELAPSED_INIT(start_tv);
#endif

    නම් (hide_cursor)
	cursor_sleep();
    නැතහොත්
	cursor_on();

    out_flush_cursor(FALSE, FALSE);
    අතරතුර (!got_int && done < msec)
    {
	wait_now = msec - done > 1000L ? 1000L : msec - done;
#ifdef FEAT_TIMERS
	{
	    long    due_time = check_due_timer();

	    නම් (due_time > 0 && due_time < wait_now)
		wait_now = due_time;
	}
#endif
#ifdef FEAT_JOB_CHANNEL
	නම් (has_any_channel() && wait_now > 20L)
	    wait_now = 20L;
#endif
#ifdef FEAT_SOUND
	නම් (has_any_sound_callback() && wait_now > 20L)
	    wait_now = 20L;
#endif
	ui_delay(wait_now, TRUE);

#ifdef FEAT_JOB_CHANNEL
	නම් (has_any_channel())
	    ui_breakcheck_force(TRUE);
	නැතහොත්
#endif
	    ui_breakcheck();
#ifdef MESSAGE_QUEUE
	// Process the netbeans and clientserver messages that may have been
	// received in the call to ui_breakcheck() when the GUI is in use. This
	// may occur when running a test case.
	parse_queued_messages();
#endif

#ifdef ELAPSED_FUNC
	// actual time passed
	done = ELAPSED_FUNC(start_tv);
#නැතහොත්
	// guesstimate time passed (will actually be more)
	done += wait_now;
#endif
    }

    // If CTRL-C was typed to interrupt the sleep, drop the CTRL-C from the
    // input buffer, otherwise a following call to input() fails.
    නම් (got_int)
	(void)vpeekc();

    නම් (hide_cursor)
	cursor_unsleep();
}

/*
 * ":winsize" command (obsolete).
 */
    static void
ex_winsize(exarg_T *eap)
{
    පූර්ණ_සංඛ්‍යා		w, h;
    char_u	*arg = eap->arg;
    char_u	*p;

    නම් (!SAFE_isdigit(*arg))
    {
	semsg(_(e_invalid_argument_str), arg);
	ආපසු;
    }
    w = getdigits(&arg);
    arg = skipwhite(arg);
    p = arg;
    h = getdigits(&arg);
    නම් (*p != NUL && *arg == NUL)
	set_shellsize(w, h, TRUE);
    නැතහොත්
	emsg(_(e_winsize_requires_two_number_arguments));
}

    static void
ex_wincmd(exarg_T *eap)
{
    පූර්ණ_සංඛ්‍යා		xchar = NUL;
    char_u	*p;

    නම් (*eap->arg == 'g' || *eap->arg == Ctrl_G)
    {
	// CTRL-W g and CTRL-W CTRL-G  have an extra command character
	නම් (eap->arg[1] == NUL)
	{
	    emsg(_(e_invalid_argument));
	    ආපසු;
	}
	xchar = eap->arg[1];
	p = eap->arg + 2;
    }
    නැතහොත්
	p = eap->arg + 1;

    set_nextcmd(eap, p);
    p = skipwhite(p);
    නම් (*p != NUL && *p != (
#ifdef FEAT_EVAL
	    in_vim9script() ? '#' :
#endif
		'"')
	    && eap->nextcmd == NULL)
	emsg(_(e_invalid_argument));
    නැතහොත් නම් (!eap->skip)
    {
	// Pass flags on for ":vertical wincmd ]".
	postponed_split_flags = cmdmod.cmod_split;
	postponed_split_tab = cmdmod.cmod_tab;
	do_window(*eap->arg, eap->addr_count > 0 ? eap->line2 : 0L, xchar);
	postponed_split_flags = 0;
	postponed_split_tab = 0;
    }
}

#නම් defined(FEAT_GUI) || defined(UNIX) || defined(VMS) || defined(MSWIN)
/*
 * ":winpos".
 */
    static void
ex_winpos(exarg_T *eap)
{
    පූර්ණ_සංඛ්‍යා		x, y;
    char_u	*arg = eap->arg;
    char_u	*p;

    නම් (*arg == NUL)
    {
# නම් defined(FEAT_GUI) || defined(MSWIN)
#  ifdef VIMDLL
	නම් (gui.in_use ? gui_mch_get_winpos(&x, &y) != FAIL :
		mch_get_winpos(&x, &y) != FAIL)
#  elif defined(FEAT_GUI)
	නම් (gui.in_use && gui_mch_get_winpos(&x, &y) != FAIL)
#  නැතහොත්
	නම් (mch_get_winpos(&x, &y) != FAIL)
#  endif
	{
	    sprintf((var_na *)IObuff, _("Window position: X %d, Y %d"), x, y);
	    msg((var_na *)IObuff);
	}
	නැතහොත්
# endif
	    emsg(_(e_obtaining_window_position_not_implemented_for_this_platform));
    }
    නැතහොත්
    {
	x = getdigits(&arg);
	arg = skipwhite(arg);
	p = arg;
	y = getdigits(&arg);
	නම් (*p == NUL || *arg != NUL)
	{
	    emsg(_(e_winpos_requires_two_number_arguments));
	    ආපසු;
	}
# ifdef FEAT_GUI
	නම් (gui.in_use)
	    gui_mch_set_winpos(x, y);
	නැතහොත් නම් (gui.starting)
	{
	    // Remember the coordinates for when the window is opened.
	    gui_win_x = x;
	    gui_win_y = y;
	}
#  නම් defined(HAVE_TGETENT) || defined(VIMDLL)
	නැතහොත්
#  endif
# endif
# නම් defined(MSWIN) && (!defined(FEAT_GUI) || defined(VIMDLL))
	    mch_set_winpos(x, y);
# endif
# ifdef HAVE_TGETENT
	නම් (*T_CWP)
	    term_set_winpos(x, y);
# endif
    }
}
#endif

/*
 * Handle command that work like operators: ":delete", ":yank", ":>" and ":<".
 */
    static void
ex_operators(exarg_T *eap)
{
    oparg_T	oa;

    clear_oparg(&oa);
    oa.regname = eap->regname;
    oa.start.lnum = eap->line1;
    oa.end.lnum = eap->line2;
    oa.line_count = eap->line2 - eap->line1 + 1;
    oa.motion_type = MLINE;
    virtual_op = FALSE;
    නම් (eap->cmdidx != CMD_yank)	// position cursor for undo
    {
	setpcmark();
	curwin->w_cursor.lnum = eap->line1;
	beginline(BL_SOL | BL_FIX);
    }

    නම් (VIsual_active)
	end_visual_mode();

    switch (eap->cmdidx)
    {
	pak_sa CMD_delete:
	    oa.op_type = OP_DELETE;
	    op_delete(&oa);
	    bha_nga;

	pak_sa CMD_yank:
	    oa.op_type = OP_YANK;
	    (void)op_yank(&oa, FALSE, TRUE);
	    bha_nga;

	default:    // CMD_rshift or CMD_lshift
	    නම් (
#ifdef FEAT_RIGHTLEFT
		(eap->cmdidx == CMD_rshift) ^ curwin->w_p_rl
#නැතහොත්
		eap->cmdidx == CMD_rshift
#endif
						)
		oa.op_type = OP_RSHIFT;
	    නැතහොත්
		oa.op_type = OP_LSHIFT;
	    op_shift(&oa, FALSE, eap->amount);
	    bha_nga;
    }
    virtual_op = MAYBE;
    ex_may_print(eap);
}

/*
 * ":put".
 */
    static void
ex_put(exarg_T *eap)
{
    // ":0put" works like ":1put!".
    නම් (eap->line2 == 0)
    {
	eap->line2 = 1;
	eap->forceit = TRUE;
    }
    curwin->w_cursor.lnum = eap->line2;
    check_cursor_col();
    do_put(eap->regname, NULL, eap->forceit ? BACKWARD : FORWARD, 1L,
						       PUT_LINE|PUT_CURSLINE);
}

/*
 * ":iput".
 */
    static void
ex_iput(exarg_T *eap)
{
    // ":0iput" works like ":1iput!".
    නම් (eap->line2 == 0)
    {
	eap->line2 = 1;
	eap->forceit = TRUE;
    }
    curwin->w_cursor.lnum = eap->line2;
    check_cursor_col();
    do_put(eap->regname, NULL, eap->forceit ? BACKWARD : FORWARD, 1L,
						      PUT_LINE|PUT_CURSLINE
						      |PUT_FIXINDENT);
}

/*
 * Handle ":copy" and ":move".
 */
    static void
ex_copymove(exarg_T *eap)
{
    long	n;

#ifdef FEAT_EVAL
    නම් (not_in_vim9(eap, false) == FAIL)
	ආපසු;
#endif
    n = get_address(eap, &eap->arg, eap->addr_type, FALSE, FALSE, FALSE, 1);
    නම් (eap->arg == NULL)	    // error detected
    {
	eap->nextcmd = NULL;
	ආපසු;
    }
    get_flags(eap);

    /*
     * move or copy lines from 'eap->line1'-'eap->line2' to below line 'n'
     */
    නම් (n == MAXLNUM || n < 0 || n > curbuf->b_ml.ml_line_count)
    {
	emsg(_(e_invalid_range));
	ආපසු;
    }

    නම් (eap->cmdidx == CMD_move)
    {
	නම් (do_move(eap->line1, eap->line2, n) == FAIL)
	    ආපසු;
    }
    නැතහොත්
	ex_copy(eap->line1, eap->line2, n);
    u_clearline();
    beginline(BL_SOL | BL_FIX);
    ex_may_print(eap);
}

/*
 * Print the current line if flags were given to the Ex command.
 */
    void
ex_may_print(exarg_T *eap)
{
    නම් (eap->flags != 0)
    {
	print_line(curwin->w_cursor.lnum, (eap->flags & EXFLAG_NR),
						  (eap->flags & EXFLAG_LIST));
	ex_no_reprint = TRUE;
    }
}

/*
 * ":smagic" and ":snomagic".
 */
    static void
ex_submagic(exarg_T *eap)
{
    optmagic_T saved = magic_overruled;

    magic_overruled = eap->cmdidx == CMD_smagic
					  ? OPTION_MAGIC_ON : OPTION_MAGIC_OFF;
    ex_substitute(eap);
    magic_overruled = saved;
}

/*
 * ":join".
 */
    static void
ex_join(exarg_T *eap)
{
    curwin->w_cursor.lnum = eap->line1;
    නම් (eap->line1 == eap->line2)
    {
	නම් (eap->addr_count >= 2)   // :2,2join does nothing
	    ආපසු;
	නම් (eap->line2 == curbuf->b_ml.ml_line_count)
	{
	    beep_flush();
	    ආපසු;
	}
	++eap->line2;
    }
    (void)do_join(eap->line2 - eap->line1 + 1, !eap->forceit, TRUE, TRUE, TRUE);
    beginline(BL_WHITE | BL_FIX);
    ex_may_print(eap);
}

/*
 * ":[addr]@r" or ":[addr]*r": execute register
 */
    static void
ex_at(exarg_T *eap)
{
    පූර්ණ_සංඛ්‍යා		c;
    පූර්ණ_සංඛ්‍යා		prev_len = typebuf.tb_len;

    curwin->w_cursor.lnum = eap->line2;
    check_cursor_col();

#ifdef USE_ON_FLY_SCROLL
    dont_scroll = TRUE;		// disallow scrolling here
#endif

    // get the register name.  No name means to use the previous one
    c = *eap->arg;
    නම් (c == NUL || (c == '*' && *eap->cmd == '*'))
	c = '@';
    // Put the register in the typeahead buffer with the "silent" flag.
    නම් (do_execreg(c, TRUE, vim_strchr(p_cpo, CPO_EXECBUF) != NULL, TRUE)
								      == FAIL)
    {
	beep_flush();
	ආපසු;
    }

    පූර්ණ_සංඛ්‍යා	save_efr = exec_from_reg;

    exec_from_reg = TRUE;

    /*
     * Execute from the typeahead buffer.
     * Continue until the stuff buffer is empty and all added characters
     * have been consumed.
     */
    අතරතුර (!stuff_empty() || typebuf.tb_len > prev_len)
	(void)do_cmdline(NULL, getexline, NULL, DOCMD_NOWAIT|DOCMD_VERBOSE);

    exec_from_reg = save_efr;
}

/*
 * ":!".
 */
    static void
ex_bang(exarg_T *eap)
{
    do_bang(eap->addr_count, eap, eap->forceit, TRUE, TRUE);
}

/*
 * ":undo".
 */
    static void
ex_undo(exarg_T *eap)
{
    නම් (eap->addr_count == 1)	    // :undo 123
	undo_time(eap->line2, FALSE, FALSE, TRUE);
    නැතහොත්
	u_undo(1);
}

#ifdef FEAT_PERSISTENT_UNDO
    static void
ex_wundo(exarg_T *eap)
{
    char_u hash[UNDO_HASH_SIZE];

    u_compute_hash(hash);
    u_write_undo(eap->arg, eap->forceit, curbuf, hash);
}

    static void
ex_rundo(exarg_T *eap)
{
    char_u hash[UNDO_HASH_SIZE];

    u_compute_hash(hash);
    u_read_undo(eap->arg, hash, NULL);
}
#endif

/*
 * ":redo".
 */
    static void
ex_redo(exarg_T *eap UNUSED)
{
    u_redo(1);
}

/*
 * ":earlier" and ":later".
 */
    static void
ex_later(exarg_T *eap)
{
    long	count = 0;
    පූර්ණ_සංඛ්‍යා		sec = FALSE;
    පූර්ණ_සංඛ්‍යා		file = FALSE;
    char_u	*p = eap->arg;

    නම් (*p == NUL)
	count = 1;
    නැතහොත් නම් (SAFE_isdigit(*p))
    {
	count = getdigits(&p);
	switch (*p)
	{
	    pak_sa 's': ++p; sec = TRUE; bha_nga;
	    pak_sa 'm': ++p; sec = TRUE; count *= 60; bha_nga;
	    pak_sa 'h': ++p; sec = TRUE; count *= 60 * 60; bha_nga;
	    pak_sa 'd': ++p; sec = TRUE; count *= 24 * 60 * 60; bha_nga;
	    pak_sa 'f': ++p; file = TRUE; bha_nga;
	}
    }

    නම් (*p != NUL)
	semsg(_(e_invalid_argument_str), eap->arg);
    නැතහොත්
	undo_time(eap->cmdidx == CMD_earlier ? -count : count,
							    sec, file, FALSE);
}

/*
 * ":redir": start/stop redirection.
 */
    static void
ex_redir(exarg_T *eap)
{
    var_na	*mode;
    char_u	*fname;
    char_u	*arg = eap->arg;

#ifdef FEAT_EVAL
    නම් (redir_execute)
    {
	emsg(_(e_cannot_use_redir_inside_execute));
	ආපසු;
    }
#endif

    නම් (STRICMP(eap->arg, "END") == 0)
	close_redir();
    නැතහොත්
    {
	නම් (*arg == '>')
	{
	    ++arg;
	    නම් (*arg == '>')
	    {
		++arg;
		mode = "a";
	    }
	    නැතහොත්
		mode = "w";
	    arg = skipwhite(arg);

	    close_redir();

	    // Expand environment variables and "~/".
	    fname = expand_env_save(arg);
	    නම් (fname == NULL)
		ආපසු;
#ifdef FEAT_BROWSE
	    නම් (cmdmod.cmod_flags & CMOD_BROWSE)
	    {
		char_u	*browseFile;

		browseFile = do_browse(BROWSE_SAVE,
			(char_u *)_("Save Redirection"),
			fname, NULL, NULL,
			(char_u *)_(BROWSE_FILTER_ALL_FILES), curbuf);
		නම් (browseFile == NULL)
		    ආපසු;		// operation cancelled
		vim_free(fname);
		fname = browseFile;
		eap->forceit = TRUE;	// since dialog already asked
	    }
#endif

	    redir_fd = open_exfile(fname, eap->forceit, mode);
	    vim_free(fname);
	}
#ifdef FEAT_EVAL
	නැතහොත් නම් (*arg == '@')
	{
	    // redirect to a register a-z (resp. A-Z for appending)
	    close_redir();
	    ++arg;
	    නම් (ASCII_ISALPHA(*arg)
# ifdef HAVE_CLIPMETHOD
		    || (clipmethod == CLIPMETHOD_PROVIDER
			&& (*arg == '*' || *arg == '+'))
# endif
# ifdef FEAT_CLIPBOARD
		    || *arg == '*'
		    || *arg == '+'
# endif
		    || *arg == '"')
	    {
		redir_reg = *arg++;
		නම් (*arg == '>' && arg[1] == '>')  // append
		    arg += 2;
		නැතහොත්
		{
		    // Can use both "@a" and "@a>".
		    නම් (*arg == '>')
			arg++;
		    // Make register empty when not using @A-@Z and the
		    // command is valid.
		    නම් (*arg == NUL && !SAFE_isupper(redir_reg))
			write_reg_contents(redir_reg, (char_u *)"", -1, FALSE);
		}
	    }
	    නම් (*arg != NUL)
	    {
		redir_reg = 0;
		semsg(_(e_invalid_argument_str), eap->arg);
	    }
	}
	නැතහොත් නම් (*arg == '=' && arg[1] == '>')
	{
	    පූර්ණ_සංඛ්‍යා append;

	    // redirect to a variable
	    close_redir();
	    arg += 2;

	    නම් (*arg == '>')
	    {
		++arg;
		append = TRUE;
	    }
	    නැතහොත්
		append = FALSE;

	    නම් (var_redir_start(skipwhite(arg), append) == OK)
		redir_vname = 1;
	}
#endif

	// TODO: redirect to a buffer

	නැතහොත්
	    semsg(_(e_invalid_argument_str), eap->arg);
    }

    // Make sure redirection is not off.  Can happen for cmdline completion
    // that indirectly invokes a command to catch its output.
    නම් (redir_fd != NULL
#ifdef FEAT_EVAL
			  || redir_reg || redir_vname
#endif
							)
	redir_off = FALSE;
}

/*
 * ":redraw": force redraw, with clear for ":redraw!".
 */
    void
ex_redraw(exarg_T *eap)
{
    redraw_cmd(eap->forceit);
}

/*
 * ":redraw": force redraw, with clear if "clear" is TRUE.
 */
    void
redraw_cmd(පූර්ණ_සංඛ්‍යා clear)
{
    පූර්ණ_සංඛ්‍යා save_RedrawingDisabled = RedrawingDisabled;
    RedrawingDisabled = 0;

    පූර්ණ_සංඛ්‍යා save_p_lz = p_lz;
    p_lz = FALSE;

    validate_cursor();
    update_topline();
    update_screen(clear ? UPD_CLEAR : VIsual_active ? UPD_INVERTED : 0);
    නම් ((State & MODE_CMDLINE) == 0)
	setcursor(); // put cursor back where it belongs
    නම් (need_maketitle)
	maketitle();
#නම් defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
# ifdef VIMDLL
    නම් (!gui.in_use)
# endif
	resize_console_buf();
#endif
    RedrawingDisabled = save_RedrawingDisabled;
    p_lz = save_p_lz;

    // After drawing the statusline screen_attr may still be set.
    screen_stop_highlight();

    // Reset msg_didout, so that a message that's there is overwritten.
    msg_didout = FALSE;
    msg_col = 0;

    // No need to wait after an intentional redraw.
    need_wait_return = FALSE;

    // When invoked from a callback or autocmd the command line may be active.
    නම් (State & MODE_CMDLINE)
	redrawcmdline();

    out_flush();
}

/*
 * ":redrawstatus": force redraw of status line(s)
 */
    static void
ex_redrawstatus(exarg_T *eap UNUSED)
{
    නම් (eap->forceit)
	status_redraw_all();
    නැතහොත්
	status_redraw_curbuf();
    redraw_vseps = TRUE;
    නම් (msg_scrolled && (State & MODE_CMDLINE))
	ආපසු;  // redraw later

    පූර්ණ_සංඛ්‍යා save_RedrawingDisabled = RedrawingDisabled;
    RedrawingDisabled = 0;

    පූර්ණ_සංඛ්‍යා save_p_lz = p_lz;
    p_lz = FALSE;

    නම් (State & MODE_CMDLINE)
	redraw_statuslines();
    නැතහොත්
	update_screen(VIsual_active ? UPD_INVERTED : 0);
    RedrawingDisabled = save_RedrawingDisabled;
    p_lz = save_p_lz;
    out_flush();
}

/*
 * ":redrawtabline": force redraw of the tabline
 */
    static void
ex_redrawtabline(exarg_T *eap UNUSED)
{
    පූර්ණ_සංඛ්‍යා save_RedrawingDisabled = RedrawingDisabled;
    RedrawingDisabled = 0;

    පූර්ණ_සංඛ්‍යා save_p_lz = p_lz;
    p_lz = FALSE;

    draw_tabline();

    RedrawingDisabled = save_RedrawingDisabled;
    p_lz = save_p_lz;
    out_flush();
}

/*
 * ":redrawtabpanel": force redraw of the tabpanel
 */
    static void
ex_redrawtabpanel(exarg_T *eap UNUSED)
{
    පූර්ණ_සංඛ්‍යා save_RedrawingDisabled = RedrawingDisabled;
    RedrawingDisabled = 0;

    පූර්ණ_සංඛ්‍යා save_p_lz = p_lz;
    p_lz = FALSE;

#නම් defined(FEAT_TABPANEL)
    draw_tabpanel();
#නැතහොත්
    emsg(_(e_cannot_not_support_redrawtabpanel));
#endif

    RedrawingDisabled = save_RedrawingDisabled;
    p_lz = save_p_lz;
    out_flush();
}

    static void
close_redir(void)
{
    නම් (redir_fd != NULL)
    {
	fclose(redir_fd);
	redir_fd = NULL;
    }
#ifdef FEAT_EVAL
    redir_reg = 0;
    නම් (redir_vname)
    {
	var_redir_stop();
	redir_vname = 0;
    }
#endif
}

#නම් defined(FEAT_SESSION) || defined(FEAT_EVAL)
    පූර්ණ_සංඛ්‍යා
vim_mkdir_emsg(char_u *name, පූර්ණ_සංඛ්‍යා prot UNUSED)
{
    නම් (vim_mkdir(name, prot) != 0)
    {
	semsg(_(e_cannot_create_directory_str), name);
	ආපසු FAIL;
    }
    ආපසු OK;
}
#endif

/*
 * Open a file for writing for an Ex command, with some checks.
 * Return file descriptor, or NULL on failure.
 */
    FILE *
open_exfile(
    char_u	*fname,
    පූර්ණ_සංඛ්‍යා		forceit,
    var_na	*mode)	    // "w" for create new file or "a" for append
{
    FILE	*fd;

#ifdef UNIX
    // with Unix it is possible to open a directory
    නම් (mch_isdir(fname))
    {
	semsg(_(e_str_is_directory), fname);
	ආපසු NULL;
    }
#endif
    නම් (!forceit && *mode != 'a' && vim_fexists(fname))
    {
	semsg(_(e_str_exists_add_bang_to_override), fname);
	ආපසු NULL;
    }

    නම් ((fd = mch_fopen((var_na *)fname, mode)) == NULL)
	semsg(_(e_cannot_open_str_for_writing_2), fname);

    ආපසු fd;
}

/*
 * ":mark" and ":k".
 */
    static void
ex_mark(exarg_T *eap)
{
    pos_T	pos;

#ifdef FEAT_EVAL
    නම් (not_in_vim9(eap, false) == FAIL)
	ආපසු;
#endif
    නම් (*eap->arg == NUL)		// No argument?
    {
	emsg(_(e_argument_required));
	ආපසු;
    }

    නම් (eap->arg[1] != NUL)	// more than one character?
    {
	semsg(_(e_trailing_characters_str), eap->arg);
	ආපසු;
    }

    pos = curwin->w_cursor;		// save curwin->w_cursor
    curwin->w_cursor.lnum = eap->line2;
    beginline(BL_WHITE | BL_FIX);
    නම් (setmark(*eap->arg) == FAIL)	// set mark
	emsg(_(e_argument_must_be_letter_or_forward_backward_quote));
    curwin->w_cursor = pos;		// restore curwin->w_cursor
}

/*
 * Update w_topline, w_leftcol and the cursor position.
 */
    void
update_topline_cursor(void)
{
    check_cursor();		// put cursor on valid line
    update_topline();
    නම් (!curwin->w_p_wrap)
	validate_cursor();
    update_curswant();
}

/*
 * Save the current State and go to Normal mode.
 * Return TRUE if the typeahead could be saved.
 */
    පූර්ණ_සංඛ්‍යා
save_current_state(save_state_T *sst)
{
    sst->save_msg_scroll = msg_scroll;
    sst->save_restart_edit = restart_edit;
    sst->save_msg_didout = msg_didout;
    sst->save_State = State;
    sst->save_insertmode = p_im;
    sst->save_finish_op = finish_op;
    sst->save_opcount = opcount;
    sst->save_reg_executing = reg_executing;
    sst->save_pending_end_reg_executing = pending_end_reg_executing;

    msg_scroll = FALSE;		    // no msg scrolling in Normal mode
    restart_edit = 0;		    // don't go to Insert mode
    p_im = FALSE;		    // don't use 'insertmode'

    sst->save_script_version = current_sctx.sc_version;
    current_sctx.sc_version = 1;    // not in Vim9 script

    /*
     * Save the current typeahead.  This is required to allow using ":normal"
     * from an event handler and makes sure we don't hang when the argument
     * ends with half a command.
     */
    save_typeahead(&sst->tabuf);
    ආපසු sst->tabuf.typebuf_valid;
}

    void
restore_current_state(save_state_T *sst)
{
    // Restore the previous typeahead.
    restore_typeahead(&sst->tabuf, FALSE);

    msg_scroll = sst->save_msg_scroll;
    restart_edit = sst->save_restart_edit;
    p_im = sst->save_insertmode;
    finish_op = sst->save_finish_op;
    opcount = sst->save_opcount;
    reg_executing = sst->save_reg_executing;
    pending_end_reg_executing = sst->save_pending_end_reg_executing;
    msg_didout |= sst->save_msg_didout;	// don't reset msg_didout now
    current_sctx.sc_version = sst->save_script_version;

    // Restore the state (needed when called from a function executed for
    // 'indentexpr'). Update the mouse and cursor, they may have changed.
    State = sst->save_State;
#ifdef CURSOR_SHAPE
    ui_cursor_shape();		// may show different cursor shape
#endif
}

/*
 * ":normal[!] {commands}": Execute normal mode commands.
 */
    void
ex_normal(exarg_T *eap)
{
    save_state_T save_state;
    char_u	*arg = NULL;
    පූර්ණ_සංඛ්‍යා		l;
    char_u	*p;

    නම් (ex_normal_lock > 0)
    {
	emsg(_(e_not_allowed_here));
	ආපසු;
    }
    නම් (ex_normal_busy >= p_mmd)
    {
	emsg(_(e_recursive_use_of_normal_too_deep));
	ආපසු;
    }

    /*
     * vgetc() expects a CSI and K_SPECIAL to have been escaped.  Don't do
     * this for the K_SPECIAL leading byte, otherwise special keys will not
     * work.
     */
    නම් (has_mbyte)
    {
	පූර්ණ_සංඛ්‍යා	len = 0;

	// Count the number of characters to be escaped.
	සඳහා (p = eap->arg; *p != NUL; ++p)
	{
#ifdef FEAT_GUI
	    නම් (*p == CSI)  // leadbyte CSI
		len += 2;
#endif
	    සඳහා (l = (*mb_ptr2len)(p) - 1; l > 0; --l)
		නම් (*++p == K_SPECIAL	  // trailbyte K_SPECIAL or CSI
#ifdef FEAT_GUI
			|| *p == CSI
#endif
			)
		    len += 2;
	}
	නම් (len > 0)
	{
	    arg = alloc(STRLEN(eap->arg) + len + 1);
	    නම් (arg != NULL)
	    {
		len = 0;
		සඳහා (p = eap->arg; *p != NUL; ++p)
		{
		    arg[len++] = *p;
#ifdef FEAT_GUI
		    නම් (*p == CSI)
		    {
			arg[len++] = KS_EXTRA;
			arg[len++] = (පූර්ණ_සංඛ්‍යා)KE_CSI;
		    }
#endif
		    සඳහා (l = (*mb_ptr2len)(p) - 1; l > 0; --l)
		    {
			arg[len++] = *++p;
			නම් (*p == K_SPECIAL)
			{
			    arg[len++] = KS_SPECIAL;
			    arg[len++] = KE_FILLER;
			}
#ifdef FEAT_GUI
			නැතහොත් නම් (*p == CSI)
			{
			    arg[len++] = KS_EXTRA;
			    arg[len++] = (පූර්ණ_සංඛ්‍යා)KE_CSI;
			}
#endif
		    }
		    arg[len] = NUL;
		}
	    }
	}
    }

    ++ex_normal_busy;
    නම් (save_current_state(&save_state))
    {
	/*
	 * Repeat the :normal command for each line in the range.  When no
	 * range given, execute it just once, without positioning the cursor
	 * first.
	 */
	do
	{
	    නම් (eap->addr_count != 0)
	    {
		curwin->w_cursor.lnum = eap->line1++;
		curwin->w_cursor.col = 0;
		check_cursor_moved(curwin);
	    }

	    exec_normal_cmd(arg != NULL
		     ? arg
		     : eap->arg, eap->forceit ? REMAP_NONE : REMAP_YES, FALSE);
	}
	අතරතුර (eap->addr_count > 0 && eap->line1 <= eap->line2 && !got_int);
    }

    // Might not return to the main loop when in an event handler.
    update_topline_cursor();

    restore_current_state(&save_state);
    --ex_normal_busy;
    setmouse();
#ifdef CURSOR_SHAPE
    ui_cursor_shape();		// may show different cursor shape
#endif

    vim_free(arg);
}

/*
 * ":startinsert", ":startreplace" and ":startgreplace"
 */
    static void
ex_startinsert(exarg_T *eap)
{
#ifdef FEAT_TERMINAL
    // Ignore this when running in an active terminal.
    නම් (term_job_running(curbuf->b_term))
	ආපසු;
#endif
    නම් (!curbuf->b_p_ma && !p_im)
    {
	// Only give this error when 'insertmode' is off.
	emsg(_(e_cannot_make_changes_modifiable_is_off));
	ආපසු;
    }
    නම් (eap->forceit)
    {
	// cursor line can be zero on startup
	නම් (!curwin->w_cursor.lnum)
	    curwin->w_cursor.lnum = 1;
	set_cursor_for_append_to_line();
    }

    // Ignore the command when already in Insert mode.  Inserting an
    // expression register that invokes a function can do this.
    නම් (State & MODE_INSERT)
	ආපසු;

    නම් (eap->cmdidx == CMD_startinsert)
	restart_edit = 'a';
    නැතහොත් නම් (eap->cmdidx == CMD_startreplace)
	restart_edit = 'R';
    නැතහොත්
	restart_edit = 'V';

    නම් (!eap->forceit)
    {
	නම් (eap->cmdidx == CMD_startinsert)
	    restart_edit = 'i';
	curwin->w_curswant = 0;	    // avoid MAXCOL
    }

    නම් (VIsual_active)
	showmode();
}

/*
 * ":stopinsert"
 */
    static void
ex_stopinsert(exarg_T *eap UNUSED)
{
    restart_edit = 0;
    stop_insert_mode = TRUE;
#නම් defined(FEAT_CLIENTSERVER) || defined(FEAT_EVAL)
    // when called from remote_expr in insert mode, make sure insert mode is
    // ended by adding K_NOP to the typeahead buffer
    නම් (vgetc_busy)
	ins_char_typebuf(K_NOP, 0);
#endif
    clearmode();
}

/*
 * Execute normal mode command "cmd".
 * "remap" can be REMAP_NONE or REMAP_YES.
 */
    void
exec_normal_cmd(char_u *cmd, පූර්ණ_සංඛ්‍යා remap, පූර්ණ_සංඛ්‍යා silent)
{
    // Stuff the argument into the typeahead buffer.
    ins_typebuf(cmd, remap, 0, TRUE, silent);
    exec_normal(FALSE, FALSE, FALSE);
}

/*
 * Execute normal_cmd() until there is no typeahead left.
 * When "use_vpeekc" is TRUE use vpeekc() to check for available chars.
 */
    void
exec_normal(පූර්ණ_සංඛ්‍යා was_typed, පූර්ණ_සංඛ්‍යා use_vpeekc, පූර්ණ_සංඛ්‍යා may_use_terminal_loop UNUSED)
{
    oparg_T	oa;
    පූර්ණ_සංඛ්‍යා		c;

    // When calling vpeekc() from feedkeys() it will return Ctrl_C when there
    // is nothing to get, so also check for Ctrl_C.
    clear_oparg(&oa);
    finish_op = FALSE;
    අතරතුර ((!stuff_empty()
		|| ((was_typed || !typebuf_typed()) && typebuf.tb_len > 0)
		|| (use_vpeekc && (c = vpeekc()) != NUL && c != Ctrl_C))
	    && !got_int)
    {
	update_topline_cursor();
#ifdef FEAT_TERMINAL
	නම් (may_use_terminal_loop && term_use_loop()
		&& oa.op_type == OP_NOP && oa.regname == NUL
		&& !VIsual_active)
	{
	    // If terminal_loop() returns OK we got a key that is handled
	    // in Normal model.  With FAIL we first need to position the
	    // cursor and the screen needs to be redrawn.
	    නම් (terminal_loop(TRUE) == OK)
		normal_cmd(&oa, TRUE);
	}
	නැතහොත්
#endif
	    // execute a Normal mode cmd
	    normal_cmd(&oa, TRUE);
    }
}

#ifdef FEAT_FIND_ID
    static void
ex_checkpath(exarg_T *eap)
{
    find_pattern_in_path(NULL, 0, 0, FALSE, FALSE, CHECK_PATH, 1L,
	    eap->forceit ? ACTION_SHOW_ALL : ACTION_SHOW,
	    (linenr_T)1, (linenr_T)MAXLNUM, eap->forceit, FALSE);
}

# නම් defined(FEAT_QUICKFIX)
/*
 * ":psearch"
 */
    static void
ex_psearch(exarg_T *eap)
{
    g_do_tagpreview = p_pvh;
    ex_findpat(eap);
    g_do_tagpreview = 0;
}
# endif

    static void
ex_findpat(exarg_T *eap)
{
    පූර්ණ_සංඛ්‍යා		whole = TRUE;
    long	n;
    char_u	*p;
    පූර්ණ_සංඛ්‍යා		action;

    switch (cmdnames[eap->cmdidx].cmd_name[2])
    {
	pak_sa 'e':	// ":psearch", ":isearch" and ":dsearch"
		නම් (cmdnames[eap->cmdidx].cmd_name[0] == 'p')
		    action = ACTION_GOTO;
		නැතහොත්
		    action = ACTION_SHOW;
		bha_nga;
	pak_sa 'i':	// ":ilist" and ":dlist"
		action = ACTION_SHOW_ALL;
		bha_nga;
	pak_sa 'u':	// ":ijump" and ":djump"
		action = ACTION_GOTO;
		bha_nga;
	default:	// ":isplit" and ":dsplit"
		action = ACTION_SPLIT;
		bha_nga;
    }

    n = 1;
    නම් (vim_isdigit(*eap->arg))	// get count
    {
	n = getdigits(&eap->arg);
	eap->arg = skipwhite(eap->arg);
    }
    නම් (*eap->arg == '/')   // Match regexp, not just whole words
    {
	whole = FALSE;
	++eap->arg;
	p = skip_regexp(eap->arg, '/', magic_isset());
	නම් (*p)
	{
	    *p++ = NUL;
	    p = skipwhite(p);

	    // Check for trailing illegal characters
	    නම් (!ends_excmd2(eap->arg, p))
		eap->errmsg = ex_errmsg(e_trailing_characters_str, p);
	    නැතහොත්
		set_nextcmd(eap, p);
	}
    }
    නම් (!eap->skip)
	find_pattern_in_path(eap->arg, 0, (පූර්ණ_සංඛ්‍යා)STRLEN(eap->arg),
		whole, !eap->forceit,
		*eap->cmd == 'd' ? FIND_DEFINE : FIND_ANY, n, action,
		eap->line1, eap->line2, eap->forceit, FALSE);
}
#endif


#ifdef FEAT_QUICKFIX
/*
 * ":ptag", ":ptselect", ":ptjump", ":ptnext", etc.
 */
    static void
ex_ptag(exarg_T *eap)
{
    g_do_tagpreview = p_pvh;  // will be reset to 0 in ex_tag_cmd()
    ex_tag_cmd(eap, cmdnames[eap->cmdidx].cmd_name + 1);
}

/*
 * ":pedit"
 */
    static void
ex_pedit(exarg_T *eap)
{
    win_T	*curwin_save = curwin;
    prepare_preview_window();

    // Edit the file.
    do_exedit(eap, NULL);

    back_to_current_window(curwin_save);
}

/*
 * ":pbuffer"
 */
    static void
ex_pbuffer(exarg_T *eap)
{
    win_T	*curwin_save = curwin;
    prepare_preview_window();

    // Go to the buffer.
    do_exbuffer(eap);

    back_to_current_window(curwin_save);
}

    static void
prepare_preview_window(void)
{
    නම් (ERROR_IF_ANY_POPUP_WINDOW)
	ආපසු;

    // Open the preview window or popup and make it the current window.
    g_do_tagpreview = p_pvh;
    prepare_tagpreview(TRUE, TRUE, FALSE);
}

    static void
back_to_current_window(win_T *curwin_save)
{
    නම් (curwin != curwin_save && win_valid(curwin_save))
    {
	// Return cursor to where we were
	validate_cursor();
	redraw_later(UPD_VALID);
	win_enter(curwin_save, TRUE);
    }
# ifdef FEAT_PROP_POPUP
    නැතහොත් නම් (WIN_IS_POPUP(curwin))
    {
	// can't keep focus in popup window
	win_enter(firstwin, TRUE);
    }
# endif
    g_do_tagpreview = 0;
}
#endif

/*
 * ":stag", ":stselect" and ":stjump".
 */
    static void
ex_stag(exarg_T *eap)
{
    postponed_split = -1;
    postponed_split_flags = cmdmod.cmod_split;
    postponed_split_tab = cmdmod.cmod_tab;
    ex_tag_cmd(eap, cmdnames[eap->cmdidx].cmd_name + 1);
    postponed_split_flags = 0;
    postponed_split_tab = 0;
}

/*
 * ":tag", ":tselect", ":tjump", ":tnext", etc.
 */
    static void
ex_tag(exarg_T *eap)
{
    ex_tag_cmd(eap, cmdnames[eap->cmdidx].cmd_name);
}

    static void
ex_tag_cmd(exarg_T *eap, char_u *name)
{
    පූර්ණ_සංඛ්‍යා		cmd;

    switch (name[1])
    {
	pak_sa 'j': cmd = DT_JUMP;	// ":tjump"
		  bha_nga;
	pak_sa 's': cmd = DT_SELECT;	// ":tselect"
		  bha_nga;
	pak_sa 'p': cmd = DT_PREV;	// ":tprevious"
		  bha_nga;
	pak_sa 'N': cmd = DT_PREV;	// ":tNext"
		  bha_nga;
	pak_sa 'n': cmd = DT_NEXT;	// ":tnext"
		  bha_nga;
	pak_sa 'o': cmd = DT_POP;		// ":pop"
		  bha_nga;
	pak_sa 'f':			// ":tfirst"
	pak_sa 'r': cmd = DT_FIRST;	// ":trewind"
		  bha_nga;
	pak_sa 'l': cmd = DT_LAST;	// ":tlast"
		  bha_nga;
	default:			// ":tag"
#ifdef FEAT_CSCOPE
		  නම් (p_cst && *eap->arg != NUL)
		  {
		      ex_cstag(eap);
		      ආපසු;
		  }
#endif
		  cmd = DT_TAG;
		  bha_nga;
    }

    නම් (name[0] == 'l')
    {
#ifndef FEAT_QUICKFIX
	ex_ni(eap);
	ආපසු;
#නැතහොත්
	cmd = DT_LTAG;
#endif
    }

    do_tag(eap->arg, cmd, eap->addr_count > 0 ? (පූර්ණ_සංඛ්‍යා)eap->line2 : 1,
							  eap->forceit, TRUE);
}

ga_nanaa {
    SPEC_PERC = 0,
    SPEC_HASH,
    SPEC_CWORD,	    // cursor word
    SPEC_CCWORD,    // cursor WORD
    SPEC_CEXPR,	    // expr under cursor
    SPEC_CFILE,	    // cursor path name
    SPEC_SFILE,	    // ":so" file name
    SPEC_SLNUM,	    // ":so" file line number
    SPEC_STACK,	    // call stack
    SPEC_SCRIPT,    // script file name
    SPEC_AFILE,	    // autocommand file name
    SPEC_ABUF,	    // autocommand buffer number
    SPEC_AMATCH,    // autocommand match name
    SPEC_SFLNUM,    // script file line number
    SPEC_SID	    // script ID: <SNR>123_
#ifdef FEAT_CLIENTSERVER
    , SPEC_CLIENT
#endif
};

/*
 * Check "str" for starting with a special cmdline variable.
 * If found return one of the SPEC_ values and set "*usedlen" to the length of
 * the variable.  Otherwise return -1 and "*usedlen" is unchanged.
 */
    පූර්ණ_සංඛ්‍යා
find_cmdline_var(char_u *src, size_t *usedlen)
{
    // must be sorted by the 'value' field because it is used by bsearch()!
    static keyvalue_T spec_str_tab[] = {
	KEYVALUE_ENTRY(SPEC_SID, "SID>"),	    // script ID: <SNR>123_
	KEYVALUE_ENTRY(SPEC_ABUF, "abuf>"),	    // autocommand buffer number
	KEYVALUE_ENTRY(SPEC_AFILE, "afile>"),	    // autocommand file name
	KEYVALUE_ENTRY(SPEC_AMATCH, "amatch>"),	    // autocommand match name
	KEYVALUE_ENTRY(SPEC_CCWORD, "cWORD>"),	    // cursor WORD
	KEYVALUE_ENTRY(SPEC_CEXPR, "cexpr>"),	    // expr under cursor
	KEYVALUE_ENTRY(SPEC_CFILE, "cfile>"),	    // cursor path name
#ifdef FEAT_CLIENTSERVER
	KEYVALUE_ENTRY(SPEC_CLIENT, "client>"),
#endif
	KEYVALUE_ENTRY(SPEC_CWORD, "cword>"),	    // cursor word
	KEYVALUE_ENTRY(SPEC_SCRIPT, "script>"),	    // script file name
	KEYVALUE_ENTRY(SPEC_SFILE, "sfile>"),	    // ":so" file name
	KEYVALUE_ENTRY(SPEC_SFLNUM, "sflnum>"),	    // script file line number
	KEYVALUE_ENTRY(SPEC_SLNUM, "slnum>"),	    // ":so" file line number
	KEYVALUE_ENTRY(SPEC_STACK, "stack>")	    // call stack
    };
    keyvalue_T target;
    keyvalue_T *entry;

    switch (*src)
    {
    pak_sa '%':
	*usedlen = 1;
	ආපසු SPEC_PERC;

    pak_sa '#':
	*usedlen = 1;
	ආපසු SPEC_HASH;

    pak_sa '<':
	target.key = 0;
	target.value.string = src + 1;	    // skip over '<'
	target.value.length = 0;	    // not used, see cmp_keyvalue_value_n()

	entry = (keyvalue_T *)bsearch(&target, &spec_str_tab,
		ARRAY_LENGTH(spec_str_tab), parimaa_na(spec_str_tab[0]),
		cmp_keyvalue_value_n);
	නම් (entry == NULL)
	    ආපසු -1;

	*usedlen = entry->value.length + 1;
	ආපසු entry->key;

    default:
	bha_nga;
    }

    ආපසු -1;
}

/*
 * Evaluate cmdline variables.
 *
 * change "%"	    to curbuf->b_ffname
 *	  "#"	    to curwin->w_alt_fnum
 *	  "%%"	    to curwin->w_alt_fnum in Vim9 script
 *	  "<cword>" to word under the cursor
 *	  "<cWORD>" to WORD under the cursor
 *	  "<cexpr>" to C-expression under the cursor
 *	  "<cfile>" to path name under the cursor
 *	  "<sfile>" to sourced file name
 *	  "<stack>" to call stack
 *	  "<script>" to current script name
 *	  "<slnum>" to sourced file line number
 *	  "<afile>" to file name for autocommand
 *	  "<abuf>"  to buffer number for autocommand
 *	  "<amatch>" to matching name for autocommand
 *
 * When an error is detected, "errormsg" is set to a non-NULL pointer (may be
 * "" for error without a message) and NULL is returned.
 * Returns an allocated string if a valid match was found.
 * Returns NULL if no match was found.	"usedlen" then still contains the
 * number of characters to skip.
 */
    char_u *
eval_vars(
    char_u	*src,		// pointer into commandline
    char_u	*srcstart,	// beginning of valid memory for src
    size_t	*usedlen,	// characters after src that are used
    linenr_T	*lnump,		// line number for :e command, or NULL
    var_na	**errormsg,	// pointer to error message
    පූර්ණ_සංඛ්‍යා		*escaped,	// return value has escaped white space (can
				// be NULL)
    පූර්ණ_සංඛ්‍යා		empty_is_error)	// empty result is considered an error
{
    පූර්ණ_සංඛ්‍යා		i;
    char_u	*s;
    char_u	*result = (char_u *)"";
    char_u	*resultbuf = NULL;
    size_t	resultlen;
    buf_T	*buf;
    පූර්ණ_සංඛ්‍යා		valid = VALID_HEAD + VALID_PATH;    // assume valid result
    පූර්ණ_සංඛ්‍යා		spec_idx;
    පූර්ණ_සංඛ්‍යා		tilde_file = FALSE;
    පූර්ණ_සංඛ්‍යා		skip_mod = FALSE;
    char_u	strbuf[30];

    *errormsg = NULL;
    නම් (escaped != NULL)
	*escaped = FALSE;

    /*
     * Check if there is something to do.
     */
    spec_idx = find_cmdline_var(src, usedlen);
    නම් (spec_idx < 0)	// no match
    {
	*usedlen = 1;
	ආපසු NULL;
    }

    /*
     * Skip when preceded with a backslash "\%" and "\#".
     * Note: In "\\%" the % is also not recognized!
     */
    නම් (src > srcstart && src[-1] == '\\')
    {
	*usedlen = 0;
	STRMOVE(src - 1, src);	// remove backslash
	ආපසු NULL;
    }

    /*
     * word or WORD under cursor
     */
    නම් (spec_idx == SPEC_CWORD || spec_idx == SPEC_CCWORD
						     || spec_idx == SPEC_CEXPR)
    {
	resultlen = find_ident_under_cursor(&result,
		spec_idx == SPEC_CWORD ? (FIND_IDENT | FIND_STRING)
	      : spec_idx == SPEC_CEXPR ? (FIND_IDENT | FIND_STRING | FIND_EVAL)
	      : FIND_STRING);
	නම් (resultlen == 0)
	{
	    *errormsg = "";
	    ආපසු NULL;
	}
    }

    /*
     * '#': Alternate file name
     * '%': Current file name
     *	    File name under the cursor
     *	    File name for autocommand
     *	and following modifiers
     */
    නැතහොත්
    {
	size_t off = 0;

	switch (spec_idx)
	{
	pak_sa SPEC_PERC:
#ifdef FEAT_EVAL
		නම් (!in_vim9script() || src[1] != '%')
#endif
		{
		    // '%': current file
		    නම් (curbuf->b_fname == NULL)
		    {
			result = (char_u *)"";
			valid = 0;	    // Must have ":p:h" to be valid
		    }
		    නැතහොත්
		    {
			result = curbuf->b_fname;
			tilde_file = STRCMP(result, "~") == 0;
		    }
		    bha_nga;
		}
#ifdef FEAT_EVAL
		// "%%" alternate file
		off = 1;
#endif
		// FALLTHROUGH
	pak_sa SPEC_HASH:		// '#' or "#99": alternate file
		නම් (off == 0 ? src[1] == '#' : src[2] == '%')
		{
		    // "##" or "%%%": the argument list
		    result = arg_all();
		    resultbuf = result;
		    *usedlen = off + 2;
		    නම් (escaped != NULL)
			*escaped = TRUE;
		    skip_mod = TRUE;
		    bha_nga;
		}
		s = src + off + 1;
		නම් (*s == '<')		// "#<99" uses v:oldfiles
		    ++s;
		i = (පූර්ණ_සංඛ්‍යා)getdigits(&s);
		නම් (s == src + off + 2 && src[off + 1] == '-')
		    // just a minus sign, don't skip over it
		    s--;
		*usedlen = (පූර්ණ_සංඛ්‍යා)(s - src); // length of what we expand

		නම් (src[off + 1] == '<' && i != 0)
		{
		    නම් (*usedlen < off + 2)
		    {
			// Should we give an error message for #<text?
			*usedlen = off + 1;
			ආපසු NULL;
		    }
#ifdef FEAT_EVAL
		    result = list_find_str(get_vim_var_list(VV_OLDFILES),
								     (long)i);
		    නම් (result == NULL)
		    {
			*errormsg = "";
			ආපසු NULL;
		    }
#නැතහොත්
		    *errormsg = _(e_hashsmall_is_not_available_without_the_eval_feature);
		    ආපසු NULL;
#endif
		}
		නැතහොත්
		{
		    නම් (i == 0 && src[off + 1] == '<' && *usedlen > off + 1)
			*usedlen = off + 1;
		    buf = buflist_findnr(i);
		    නම් (buf == NULL)
		    {
			*errormsg = _(e_no_alternate_file_name_to_substitute_for_hash);
			ආපසු NULL;
		    }
		    නම් (lnump != NULL)
			*lnump = ECMD_LAST;
		    නම් (buf->b_fname == NULL)
		    {
			result = (char_u *)"";
			valid = 0;	    // Must have ":p:h" to be valid
		    }
		    නැතහොත්
		    {
			result = buf->b_fname;
			tilde_file = STRCMP(result, "~") == 0;
		    }
		}
		bha_nga;

	pak_sa SPEC_CFILE:	// file name under cursor
		result = file_name_at_cursor(FNAME_MESS|FNAME_HYP, 1L, NULL);
		නම් (result == NULL)
		{
		    *errormsg = "";
		    ආපසු NULL;
		}
		resultbuf = result;	    // remember allocated string
		bha_nga;

	pak_sa SPEC_AFILE:	// file name for autocommand
		result = autocmd_fname;
		නම් (result != NULL && !autocmd_fname_full)
		{
		    // Still need to turn the fname into a full path.  It is
		    // postponed to avoid a delay when <afile> is not used.
		    autocmd_fname_full = TRUE;
		    result = FullName_save(autocmd_fname, FALSE);
		    vim_free(autocmd_fname);
		    autocmd_fname = result;
		}
		නම් (result == NULL)
		{
		    *errormsg = _(e_no_autocommand_file_name_to_substitute_for_afile);
		    ආපසු NULL;
		}
		result = shorten_fname1(result);
		bha_nga;

	pak_sa SPEC_ABUF:		// buffer number for autocommand
		නම් (autocmd_bufnr <= 0)
		{
		    *errormsg = _(e_no_autocommand_buffer_number_to_substitute_for_abuf);
		    ආපසු NULL;
		}
		sprintf((var_na *)strbuf, "%d", autocmd_bufnr);
		result = strbuf;
		bha_nga;

	pak_sa SPEC_AMATCH:	// match name for autocommand
		result = autocmd_match;
		නම් (result == NULL)
		{
		    *errormsg = _(e_no_autocommand_match_name_to_substitute_for_amatch);
		    ආපසු NULL;
		}
		bha_nga;

	pak_sa SPEC_SFILE:	// file name for ":so" command
		result = estack_sfile(ESTACK_SFILE);
		නම් (result == NULL)
		{
		    *errormsg = _(e_no_source_file_name_to_substitute_for_sfile);
		    ආපසු NULL;
		}
		resultbuf = result;	    // remember allocated string
		bha_nga;
	pak_sa SPEC_STACK:	// call stack
		result = estack_sfile(ESTACK_STACK);
		නම් (result == NULL)
		{
		    *errormsg = _(e_no_call_stack_to_substitute_for_stack);
		    ආපසු NULL;
		}
		resultbuf = result;	    // remember allocated string
		bha_nga;
	pak_sa SPEC_SCRIPT:	// script file name
		result = estack_sfile(ESTACK_SCRIPT);
		නම් (result == NULL)
		{
		    *errormsg = _(e_no_script_file_name_to_substitute_for_script);
		    ආපසු NULL;
		}
		resultbuf = result;	    // remember allocated string
		bha_nga;

	pak_sa SPEC_SLNUM:	// line in file for ":so" command
		නම් (SOURCING_NAME == NULL || SOURCING_LNUM == 0)
		{
		    *errormsg = _(e_no_line_number_to_use_for_slnum);
		    ආපසු NULL;
		}
		sprintf((var_na *)strbuf, "%ld", SOURCING_LNUM);
		result = strbuf;
		bha_nga;

#ifdef FEAT_EVAL
	pak_sa SPEC_SFLNUM:	// line in script file
		නම් (current_sctx.sc_lnum + SOURCING_LNUM == 0)
		{
		    *errormsg = _(e_no_line_number_to_use_for_sflnum);
		    ආපසු NULL;
		}
		sprintf((var_na *)strbuf, "%ld",
				 (long)(current_sctx.sc_lnum + SOURCING_LNUM));
		result = strbuf;
		bha_nga;

	pak_sa SPEC_SID:
		නම් (current_sctx.sc_sid <= 0)
		{
		    *errormsg = _(e_using_sid_not_in_script_context);
		    ආපසු NULL;
		}
		sprintf((var_na *)strbuf, "<SNR>%d_", current_sctx.sc_sid);
		result = strbuf;
		bha_nga;
#endif

#ifdef FEAT_CLIENTSERVER
	pak_sa SPEC_CLIENT:	// Source of last submitted input
# ifdef MSWIN
		sprintf((var_na *)strbuf, PRINTF_HEX_LONG_U,
							(long_u)clientWindow);
		result = strbuf;
# නැතහොත්
#  ifdef FEAT_SOCKETSERVER
		නම් (clientserver_method == CLIENTSERVER_METHOD_SOCKET)
		{
		    නම් (client_socket == NULL)
			result = (char_u *)"";
		    නැතහොත්
			result = client_socket;
		}
#  endif
#  ifdef FEAT_X11
		නම් (clientserver_method == CLIENTSERVER_METHOD_X11)
		{
		    sprintf((var_na *)strbuf, PRINTF_HEX_LONG_U,
							(long_u)clientWindow);
		    result = strbuf;
		}
#  endif
# endif
		bha_nga;
#endif
	}

	resultlen = STRLEN(result);	// length of new string
	නම් (src[*usedlen] == '<')	// remove the file name extension
	{
	    ++*usedlen;
	    නම් ((s = vim_strrchr(result, '.')) != NULL && s >= gettail(result))
		resultlen = s - result;
	}
	නැතහොත් නම් (!skip_mod)
	{
	    valid |= modify_fname(src, tilde_file, usedlen, &result, &resultbuf,
								  &resultlen);
	    නම් (result == NULL)
	    {
		*errormsg = "";
		ආපසු NULL;
	    }
	}
    }

    නම් (resultlen == 0 || valid != VALID_HEAD + VALID_PATH)
    {
	නම් (empty_is_error)
	{
	    නම් (valid != VALID_HEAD + VALID_PATH)
		*errormsg = _(e_empty_file_name_for_percent_or_hash_only_works_with_ph);
	    නැතහොත්
		*errormsg = _(e_evaluates_to_an_empty_string);
	}
	result = NULL;
    }
    නැතහොත්
	result = vim_strnsave(result, resultlen);
    vim_free(resultbuf);
    ආපසු result;
}

/*
 * Expand the <sfile> string in "arg".
 *
 * Returns an allocated string, or NULL for any error.
 */
    char_u *
expand_sfile(char_u *arg)
{
    var_na	*errormsg;
    char_u	*result;
    size_t	resultlen;
    char_u	*newres;
    size_t	len;
    char_u	*repl;
    size_t	repllen;
    size_t	srclen;
    char_u	*p;

    resultlen = STRLEN(arg);
    result = vim_strnsave(arg, resultlen);
    නම් (result == NULL)
	ආපසු NULL;

    සඳහා (p = result; *p; )
    {
	නම් (STRNCMP(p, "<sfile>", 7) != 0)
	    ++p;
	නැතහොත්
	{
	    // replace "<sfile>" with the sourced file name, and do ":" stuff
	    repl = eval_vars(p, result, &srclen, NULL, &errormsg, NULL, TRUE);
	    නම් (errormsg != NULL)
	    {
		නම් (*errormsg)
		    emsg(errormsg);
		vim_free(result);
		ආපසු NULL;
	    }
	    නම් (repl == NULL)		// no match (cannot happen)
	    {
		p += srclen;
		continue;
	    }
	    repllen = STRLEN(repl);
	    resultlen += repllen - srclen;
	    newres = alloc(resultlen + 1);
	    නම් (newres == NULL)
	    {
		vim_free(repl);
		vim_free(result);
		ආපසු NULL;
	    }
	    len = p - result;
	    mch_memmove(newres, result, len);
	    STRCPY(newres + len, repl);
	    len += repllen;
	    STRCPY(newres + len, p + srclen);
	    vim_free(repl);
	    vim_free(result);
	    result = newres;
	    p = newres + len;		// continue after the match
	}
    }

    ආපසු result;
}

#නම් defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
/*
 * Make a dialog message in "buff[DIALOG_MSG_SIZE]".
 * "format" must contain "%s".
 */
    void
dialog_msg(char_u *buff, var_na *format, char_u *fname)
{
    නම් (fname == NULL)
	fname = (char_u *)_("Untitled");
    vim_snprintf((var_na *)buff, DIALOG_MSG_SIZE, format, fname);
}
#endif

/*
 * ":behave {mswin,xterm}"
 */
    static void
ex_behave(exarg_T *eap)
{
    නම් (STRCMP(eap->arg, "mswin") == 0)
    {
	set_option_value_give_err((char_u *)"selection",
						 0L, (char_u *)"exclusive", 0);
	set_option_value_give_err((char_u *)"selectmode",
						 0L, (char_u *)"mouse,key", 0);
	set_option_value_give_err((char_u *)"mousemodel",
						     0L, (char_u *)"popup", 0);
	set_option_value_give_err((char_u *)"keymodel",
					  0L, (char_u *)"startsel,stopsel", 0);
    }
    නැතහොත් නම් (STRCMP(eap->arg, "xterm") == 0)
    {
	set_option_value_give_err((char_u *)"selection",
						 0L, (char_u *)"inclusive", 0);
	set_option_value_give_err((char_u *)"selectmode", 0L, (char_u *)"", 0);
	set_option_value_give_err((char_u *)"mousemodel",
						    0L, (char_u *)"extend", 0);
	set_option_value_give_err((char_u *)"keymodel", 0L, (char_u *)"", 0);
    }
    නැතහොත්
	semsg(_(e_invalid_argument_str), eap->arg);
}

static පූර්ණ_සංඛ්‍යා filetype_detect = FALSE;
static පූර්ණ_සංඛ්‍යා filetype_plugin = FALSE;
static පූර්ණ_සංඛ්‍යා filetype_indent = FALSE;

/*
 * ":filetype [plugin] [indent] {on,off,detect}"
 * on: Load the filetype.vim file to install autocommands for file types.
 * off: Load the ftoff.vim file to remove all autocommands for file types.
 * plugin on: load filetype.vim and ftplugin.vim
 * plugin off: load ftplugof.vim
 * indent on: load filetype.vim and indent.vim
 * indent off: load indoff.vim
 */
    static void
ex_filetype(exarg_T *eap)
{
    char_u	*arg = eap->arg;
    පූර්ණ_සංඛ්‍යා		plugin = FALSE;
    පූර්ණ_සංඛ්‍යා		indent = FALSE;

    නම් (*eap->arg == NUL)
    {
	// Print current status.
	smsg("filetype detection:%s  plugin:%s  indent:%s",
		filetype_detect ? "ON" : "OFF",
		filetype_plugin ? (filetype_detect ? "ON" : "(on)") : "OFF",
		filetype_indent ? (filetype_detect ? "ON" : "(on)") : "OFF");
	ආපසු;
    }

    // Accept "plugin" and "indent" in any order.
    සඳහා (;;)
    {
	නම් (STRNCMP(arg, "plugin", 6) == 0)
	{
	    plugin = TRUE;
	    arg = skipwhite(arg + 6);
	    continue;
	}
	නම් (STRNCMP(arg, "indent", 6) == 0)
	{
	    indent = TRUE;
	    arg = skipwhite(arg + 6);
	    continue;
	}
	bha_nga;
    }
    නම් (STRCMP(arg, "on") == 0 || STRCMP(arg, "detect") == 0)
    {
	නම් (*arg == 'o' || !filetype_detect)
	{
	    source_runtime((char_u *)FILETYPE_FILE, DIP_ALL);
	    filetype_detect = TRUE;
	    නම් (plugin)
	    {
		source_runtime((char_u *)FTPLUGIN_FILE, DIP_ALL);
		filetype_plugin = TRUE;
	    }
	    නම් (indent)
	    {
		source_runtime((char_u *)INDENT_FILE, DIP_ALL);
		filetype_indent = TRUE;
	    }
	}
	නම් (*arg == 'd')
	{
	    (void)do_doautocmd((char_u *)"filetypedetect BufRead", TRUE, NULL);
	    do_modelines(0);
	}
    }
    නැතහොත් නම් (STRCMP(arg, "off") == 0)
    {
	නම් (plugin || indent)
	{
	    නම් (plugin)
	    {
		source_runtime((char_u *)FTPLUGOF_FILE, DIP_ALL);
		filetype_plugin = FALSE;
	    }
	    නම් (indent)
	    {
		source_runtime((char_u *)INDOFF_FILE, DIP_ALL);
		filetype_indent = FALSE;
	    }
	}
	නැතහොත්
	{
	    source_runtime((char_u *)FTOFF_FILE, DIP_ALL);
	    filetype_detect = FALSE;
	}
    }
    නැතහොත්
	semsg(_(e_invalid_argument_str), arg);
}

/*
 * ":setfiletype [FALLBACK] {name}"
 */
    static void
ex_setfiletype(exarg_T *eap)
{
    නම් (curbuf->b_did_filetype)
	ආපසු;

    char_u *arg = eap->arg;
    නම් (STRNCMP(arg, "FALLBACK ", 9) == 0)
	arg += 9;

    set_option_value_give_err((char_u *)"filetype", 0L, arg, OPT_LOCAL);
    නම් (arg != eap->arg)
	curbuf->b_did_filetype = false;
}

    static void
ex_digraphs(exarg_T *eap UNUSED)
{
#ifdef FEAT_DIGRAPHS
    නම් (*eap->arg != NUL)
	putdigraph(eap->arg);
    නැතහොත්
	listdigraphs(eap->forceit);
#නැතහොත්
    emsg(_(e_no_digraphs_version));
#endif
}

#නම් defined(FEAT_SEARCH_EXTRA)
    void
set_no_hlsearch(පූර්ණ_සංඛ්‍යා flag)
{
    no_hlsearch = flag;
# ifdef FEAT_EVAL
    set_vim_var_nr(VV_HLSEARCH, !no_hlsearch && p_hls);
# endif
}

/*
 * ":nohlsearch"
 */
    static void
ex_nohlsearch(exarg_T *eap UNUSED)
{
    set_no_hlsearch(TRUE);
    redraw_all_later(UPD_SOME_VALID);
}
#endif

#ifdef FEAT_CRYPT
/*
 * ":X": Get crypt key
 */
    static void
ex_X(exarg_T *eap UNUSED)
{
    crypt_check_current_method();
    (void)crypt_get_key(TRUE, TRUE);
}
#endif

#ifdef FEAT_FOLDING
    static void
ex_fold(exarg_T *eap)
{
    නම් (foldManualAllowed(TRUE))
	foldCreate(eap->line1, eap->line2);
}

    static void
ex_foldopen(exarg_T *eap)
{
    opFoldRange(eap->line1, eap->line2, eap->cmdidx == CMD_foldopen,
							 eap->forceit, FALSE);
}

    static void
ex_folddo(exarg_T *eap)
{
    linenr_T	lnum;

# ifdef FEAT_CLIPBOARD
    start_global_changes();
# endif
# ifdef FEAT_CLIPBOARD_PROVIDER
    inc_clip_provider();
# endif

    // First set the marks for all lines closed/open.
    සඳහා (lnum = eap->line1; lnum <= eap->line2; ++lnum)
	නම් (hasFolding(lnum, NULL, NULL) == (eap->cmdidx == CMD_folddoclosed))
	    ml_setmarked(lnum);

    // Execute the command on the marked lines.
    global_exe(eap->arg);
    ml_clearmarked();	   // clear rest of the marks
# ifdef FEAT_CLIPBOARD
    end_global_changes();
# endif
# ifdef FEAT_CLIPBOARD_PROVIDER
    dec_clip_provider();
# endif
}
#endif

#නම් defined(FEAT_QUICKFIX)
/*
 * Returns TRUE if the supplied Ex cmdidx is for a location list command
 * instead of a quickfix command.
 */
    පූර්ණ_සංඛ්‍යා
is_loclist_cmd(පූර්ණ_සංඛ්‍යා cmdidx)
{
    නම් (cmdidx < 0 || cmdidx >= CMD_SIZE)
	ආපසු FALSE;
    ආපසු cmdnames[cmdidx].cmd_name[0] == 'l';
}
#endif

    පූර්ණ_සංඛ්‍යා
get_pressedreturn(void)
{
    ආපසු ex_pressedreturn;
}

    void
set_pressedreturn(පූර්ණ_සංඛ්‍යා val)
{
    ex_pressedreturn = val;
}
