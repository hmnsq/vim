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

#شامل "vim.h"

ایستا صحیح	quitmore = 0;
ایستا صحیح	ex_pressedreturn = FALSE;
#ifndef FEAT_PRINTER
# define ex_hardcopy	ex_ni
#endif

#ifdef FEAT_EVAL
ایستا char_u	*do_one_cmd(char_u **, صحیح, cstack_T *, char_u *(*fgetline)(صحیح, تهی *, صحیح, getline_opt_T), تهی *cookie);
#وگرنه
ایستا char_u	*do_one_cmd(char_u **, صحیح, char_u *(*fgetline)(صحیح, تهی *, صحیح, getline_opt_T), تهی *cookie);
ایستا صحیح	if_level = 0;		// depth in :if
#endif
ایستا تهی	append_command(char_u *cmd);

#ifndef FEAT_MENU
# define ex_emenu		ex_ni
# define ex_menu		ex_ni
# define ex_menutranslate	ex_ni
#endif
ایستا تهی	ex_autocmd(exarg_T *eap);
ایستا تهی	ex_doautocmd(exarg_T *eap);
ایستا تهی	ex_bunload(exarg_T *eap);
ایستا تهی	ex_buffer(exarg_T *eap);
ایستا تهی	do_exbuffer(exarg_T *eap);
ایستا تهی	ex_bmodified(exarg_T *eap);
ایستا تهی	ex_bnext(exarg_T *eap);
ایستا تهی	ex_bprevious(exarg_T *eap);
ایستا تهی	ex_brewind(exarg_T *eap);
ایستا تهی	ex_blast(exarg_T *eap);
ایستا char_u	*getargcmd(char_u **);
ایستا صحیح	getargopt(exarg_T *eap);
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
#اگر !defined(FEAT_QUICKFIX) || !defined(FEAT_EVAL)
# define ex_cexpr		ex_ni
#endif

ایستا linenr_T default_address(exarg_T *eap);
ایستا تهی address_default_all(exarg_T *eap);
ایستا تهی	get_flags(exarg_T *eap);
#اگر !defined(FEAT_PERL) \
	|| !defined(FEAT_PYTHON) || !defined(FEAT_PYTHON3) \
	|| !defined(FEAT_TCL) \
	|| !defined(FEAT_RUBY) \
	|| !defined(FEAT_LUA) \
	|| !defined(FEAT_MZSCHEME)
# define HAVE_EX_SCRIPT_NI
ایستا تهی	ex_script_ni(exarg_T *eap);
#endif
ایستا نویسه	*invalid_range(exarg_T *eap);
ایستا تهی	correct_range(exarg_T *eap);
#ifdef FEAT_QUICKFIX
ایستا char_u	*replace_makeprg(exarg_T *eap, char_u *p, char_u **cmdlinep);
#endif
ایستا char_u	*repl_cmdline(exarg_T *eap, char_u *src, size_t srclen, char_u *repl, char_u **cmdlinep);
ایستا تهی	ex_highlight(exarg_T *eap);
ایستا تهی	ex_colorscheme(exarg_T *eap);
ایستا تهی	ex_cquit(exarg_T *eap);
ایستا تهی	ex_quit_all(exarg_T *eap);
ایستا تهی	ex_close(exarg_T *eap);
ایستا تهی	ex_win_close(صحیح forceit, win_T *win, tabpage_T *tp);
ایستا تهی	ex_only(exarg_T *eap);
ایستا تهی	ex_resize(exarg_T *eap);
ایستا تهی	ex_stag(exarg_T *eap);
ایستا تهی	ex_tabclose(exarg_T *eap);
ایستا تهی	ex_tabonly(exarg_T *eap);
ایستا تهی	ex_tabnext(exarg_T *eap);
ایستا تهی	ex_tabmove(exarg_T *eap);
ایستا تهی	ex_tabs(exarg_T *eap);
#اگر defined(FEAT_QUICKFIX)
ایستا تهی	ex_pclose(exarg_T *eap);
ایستا تهی	ex_ptag(exarg_T *eap);
ایستا تهی	ex_pedit(exarg_T *eap);
ایستا تهی	ex_pbuffer(exarg_T *eap);
ایستا تهی	prepare_preview_window(تهی);
ایستا تهی	back_to_current_window(win_T *curwin_save);
#وگرنه
# define ex_pclose		ex_ni
# define ex_ptag		ex_ni
# define ex_pedit		ex_ni
# define ex_pbuffer		ex_ni
#endif
ایستا تهی	ex_hide(exarg_T *eap);
ایستا تهی	ex_exit(exarg_T *eap);
ایستا تهی	ex_print(exarg_T *eap);
#ifdef FEAT_BYTEOFF
ایستا تهی	ex_goto(exarg_T *eap);
#وگرنه
# define ex_goto		ex_ni
#endif
ایستا تهی	ex_shell(exarg_T *eap);
ایستا تهی	ex_preserve(exarg_T *eap);
ایستا تهی	ex_recover(exarg_T *eap);
ایستا تهی	ex_mode(exarg_T *eap);
ایستا تهی	ex_wrongmodifier(exarg_T *eap);
ایستا تهی	ex_find(exarg_T *eap);
ایستا تهی	ex_open(exarg_T *eap);
ایستا تهی	ex_edit(exarg_T *eap);
#ifndef FEAT_GUI
# define ex_gui			ex_nogui
ایستا تهی	ex_nogui(exarg_T *eap);
#endif
#اگر defined(FEAT_GUI_MSWIN) && defined(FEAT_MENU) && defined(FEAT_TEAROFF)
ایستا تهی	ex_tearoff(exarg_T *eap);
#وگرنه
# define ex_tearoff		ex_ni
#endif
#اگر (defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_GTK) \
	|| defined(FEAT_TERM_POPUP_MENU)) && defined(FEAT_MENU)
ایستا تهی	ex_popup(exarg_T *eap);
#وگرنه
# define ex_popup		ex_ni
#endif
#ifndef FEAT_GUI_MSWIN
# define ex_simalt		ex_ni
#endif
#اگر !defined(FEAT_GUI_MSWIN) && !defined(FEAT_GUI_GTK) && !defined(FEAT_GUI_MOTIF)
# define gui_mch_find_dialog	ex_ni
# define gui_mch_replace_dialog ex_ni
#endif
#اگر !defined(FEAT_GUI_GTK)
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
#اگر !defined(FEAT_SYN_HL) || !defined(FEAT_PROFILE)
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
#اگر !defined(FEAT_PYTHON) && !defined(FEAT_PYTHON3)
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
ایستا تهی	ex_swapname(exarg_T *eap);
ایستا تهی	ex_syncbind(exarg_T *eap);
ایستا تهی	ex_read(exarg_T *eap);
ایستا تهی	ex_pwd(exarg_T *eap);
ایستا تهی	ex_equal(exarg_T *eap);
ایستا تهی	ex_sleep(exarg_T *eap);
ایستا تهی	ex_winsize(exarg_T *eap);
ایستا تهی	ex_wincmd(exarg_T *eap);
#اگر defined(FEAT_GUI) || defined(UNIX) || defined(VMS) || defined(MSWIN)
ایستا تهی	ex_winpos(exarg_T *eap);
#وگرنه
# define ex_winpos	    ex_ni
#endif
ایستا تهی	ex_operators(exarg_T *eap);
ایستا تهی	ex_put(exarg_T *eap);
ایستا تهی	ex_iput(exarg_T *eap);
ایستا تهی	ex_copymove(exarg_T *eap);
ایستا تهی	ex_submagic(exarg_T *eap);
ایستا تهی	ex_join(exarg_T *eap);
ایستا تهی	ex_at(exarg_T *eap);
ایستا تهی	ex_bang(exarg_T *eap);
ایستا تهی	ex_undo(exarg_T *eap);
#ifdef FEAT_PERSISTENT_UNDO
ایستا تهی	ex_wundo(exarg_T *eap);
ایستا تهی	ex_rundo(exarg_T *eap);
#endif
ایستا تهی	ex_redo(exarg_T *eap);
ایستا تهی	ex_later(exarg_T *eap);
ایستا تهی	ex_redir(exarg_T *eap);
ایستا تهی	ex_redrawstatus(exarg_T *eap);
ایستا تهی	ex_redrawtabline(exarg_T *eap);
ایستا تهی	ex_redrawtabpanel(exarg_T *eap);
ایستا تهی	close_redir(تهی);
ایستا تهی	ex_mark(exarg_T *eap);
ایستا تهی	ex_startinsert(exarg_T *eap);
ایستا تهی	ex_stopinsert(exarg_T *eap);
#ifdef FEAT_FIND_ID
ایستا تهی	ex_checkpath(exarg_T *eap);
ایستا تهی	ex_findpat(exarg_T *eap);
#وگرنه
# define ex_findpat		ex_ni
# define ex_checkpath		ex_ni
#endif
#اگر defined(FEAT_FIND_ID) && defined(FEAT_QUICKFIX)
ایستا تهی	ex_psearch(exarg_T *eap);
#وگرنه
# define ex_psearch		ex_ni
#endif
ایستا تهی	ex_tag(exarg_T *eap);
ایستا تهی	ex_tag_cmd(exarg_T *eap, char_u *name);
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
ایستا تهی	ex_behave(exarg_T *eap);
ایستا تهی	ex_filetype(exarg_T *eap);
ایستا تهی	ex_setfiletype(exarg_T *eap);
#ifndef FEAT_DIFF
# define ex_diffoff		ex_ni
# define ex_diffpatch		ex_ni
# define ex_diffgetput		ex_ni
# define ex_diffsplit		ex_ni
# define ex_diffthis		ex_ni
# define ex_diffupdate		ex_ni
#endif
ایستا تهی	ex_digraphs(exarg_T *eap);
#ifdef FEAT_جستجو_EXTRA
ایستا تهی	ex_nohlsearch(exarg_T *eap);
#وگرنه
# define ex_nohlsearch		ex_ni
# define ex_match		ex_ni
#endif
#ifdef FEAT_CRYPT
ایستا تهی	ex_X(exarg_T *eap);
#وگرنه
# define ex_X			ex_ni
#endif
#ifdef FEAT_FOLDING
ایستا تهی	ex_fold(exarg_T *eap);
ایستا تهی	ex_foldopen(exarg_T *eap);
ایستا تهی	ex_folddo(exarg_T *eap);
#وگرنه
# define ex_fold		ex_ni
# define ex_foldopen		ex_ni
# define ex_folddo		ex_ni
#endif
#اگر !(defined(HAVE_LOCALE_H) || defined(X_LOCALE))
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
#اگر !defined(FEAT_X11) || !defined(FEAT_XCLIPBOARD)
# define ex_xrestore		ex_ni
#endif
#اگر !defined(FEAT_WAYLAND)
# define ex_wlrestore		ex_ni
#endif
#اگر !defined(HAVE_CLIPMETHOD)
# define ex_clipreset		ex_ni
#endif
#اگر !defined(FEAT_PROP_POPUP)
# define ex_popupclear		ex_ni
#endif

/*
 * Declare cmdnames[].
 */
#define DO_DECLARE_EXCMD
#شامل "ex_cmds.h"
#شامل "ex_cmdidxs.h"

ایستا char_u dollar_command[2] = {'$', 0};


#ifdef FEAT_EVAL
// Struct for storing a line inside a while/for loop
تعریفنوع ساختار
{
    char_u	*سطر;		// command line
    linenr_T	lnum;		// sourcing_lnum of the line
} wcmd_T;

/*
 * Structure used to store info for line position in a while or for loop.
 * This is required, because do_one_cmd() may invoke ex_function(), which
 * reads more lines that may come from the while/for loop.
 */
ساختار loop_cookie
{
    garray_T	*سطرها_gap;		// growarray with line info
    صحیح		current_سطر;		// last read line from growarray
    صحیح		repeating;		// TRUE when looping a second time
    // When "repeating" is FALSE use "getline" and "cookie" to get lines
    char_u	*(*lc_getline)(صحیح, تهی *, صحیح, getline_opt_T);
    تهی	*cookie;
};

ایستا char_u	*get_loop_سطر(صحیح c, تهی *cookie, صحیح indent, getline_opt_T options);
ایستا صحیح	store_loop_سطر(garray_T *gap, char_u *سطر);
ایستا تهی	free_cmdlines(garray_T *gap);

// Struct to save a few things while debugging.  Used in do_cmdline() only.
ساختار dbg_stuff
{
    صحیح		trylevel;
    صحیح		force_abort;
    except_T	*caught_پشته;
    char_u	*vv_exception;
    char_u	*vv_throwpoint;
    صحیح		did_emsg;
    صحیح		got_int;
    صحیح		did_throw;
    صحیح		need_rethrow;
    صحیح		check_cstack;
    except_T	*current_exception;
};

    ایستا تهی
save_dbg_stuff(ساختار dbg_stuff *dsp)
{
    dsp->trylevel	= trylevel;		trylevel = 0;
    dsp->force_abort	= force_abort;		force_abort = FALSE;
    dsp->caught_پشته	= caught_پشته;		caught_پشته = پوچ;
    dsp->vv_exception	= v_exception(پوچ);
    dsp->vv_throwpoint	= v_throwpoint(پوچ);

    // Necessary for debugging an inactive ":catch", ":finally", ":endtry"
    dsp->did_emsg	= did_emsg;		did_emsg     = FALSE;
    dsp->got_int	= got_int;		got_int	     = FALSE;
    dsp->did_throw	= did_throw;		did_throw    = FALSE;
    dsp->need_rethrow	= need_rethrow;		need_rethrow = FALSE;
    dsp->check_cstack	= check_cstack;		check_cstack = FALSE;
    dsp->current_exception = current_exception;	current_exception = پوچ;
}

    ایستا تهی
restore_dbg_stuff(ساختار dbg_stuff *dsp)
{
    suppress_errthrow = FALSE;
    trylevel = dsp->trylevel;
    force_abort = dsp->force_abort;
    caught_پشته = dsp->caught_پشته;
    (تهی)v_exception(dsp->vv_exception);
    (تهی)v_throwpoint(dsp->vv_throwpoint);
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
    ایستا صحیح
is_other_file(صحیح fnum, char_u *ffname)
{
    اگر (fnum != 0)
    {
	اگر (fnum == curbuf->b_fnum)
	    بازگشت FALSE;

	بازگشت TRUE;
    }

    اگر (ffname == پوچ)
	بازگشت TRUE;

    اگر (*ffname == NUL)
	بازگشت FALSE;

    // TODO: Need a reliable way to know whether a buffer is meant to live
    // on-disk !curbuf->b_dev_valid is not always available (example: missing
    // on Windows)
    اگر (curbuf->b_sfname != پوچ
	    && *curbuf->b_sfname != NUL)
	// This occurs with unsaved buffers. In which case `ffname` actually
	// corresponds to curbuf->b_sfname
	بازگشت fnamecmp(ffname, curbuf->b_sfname) != 0;

    بازگشت otherfile(ffname);
}

/*
 * do_exmode(): Repeatedly get commands for the "Ex" mode, until the ":vi"
 * command is given.
 */
    تهی
do_exmode(
    صحیح		improved)	    // TRUE for "improved Ex" mode
{
    صحیح		save_پیام_scroll;
    صحیح		prev_پیام_row;
    linenr_T	prev_سطر;
    varnumber_T	changedtick;

    اگر (improved)
	exmode_active = EXMODE_VIM;
    وگرنه
	exmode_active = EXMODE_NORMAL;
    State = MODE_NORMAL;
    may_trigger_modechanged();

    // When using ":global /pat/ visual" and then "Q" we return to continue
    // the :global command.
    اگر (global_busy)
	بازگشت;

    save_پیام_scroll = پیام_scroll;
    ++RedrawingDisabled;	    // don't redisplay the window
    ++no_wait_return;		    // don't wait for return
#ifdef FEAT_GUI
    // Ignore scrollbar and mouse events in Ex mode
    ++hold_gui_events;
#endif

    پیام(_("Entering Ex mode.  Type \"visual\" to go to Normal mode."));
    وقتی (exmode_active)
    {
	// Check for a ":normal" command and no more characters left.
	اگر (ex_normal_busy > 0 && typebuf.tb_len == 0)
	{
	    exmode_active = FALSE;
	    بشکن;
	}
	پیام_scroll = TRUE;
	need_wait_return = FALSE;
	ex_pressedreturn = FALSE;
	ex_no_reprint = FALSE;
	changedtick = CHANGEDTICK(curbuf);
	prev_پیام_row = پیام_row;
	prev_سطر = curwin->w_cursor.lnum;
	اگر (improved)
	{
	    cmdline_row = پیام_row;
	    do_cmdline(پوچ, getexline, پوچ, 0);
	}
	وگرنه
	    do_cmdline(پوچ, getexmodeline, پوچ, DOCMD_NOWAIT);
	سطرها_چپ = Rows - 1;

	اگر ((prev_سطر != curwin->w_cursor.lnum
		   || changedtick != CHANGEDTICK(curbuf)) && !ex_no_reprint)
	{
	    اگر (curbuf->b_ml.ml_flags & ML_EMPTY)
		emsg(_(e_empty_buffer));
	    وگرنه
	    {
		اگر (ex_pressedreturn)
		{
		    // go up one line, to overwrite the ":<CR>" line, so the
		    // output doesn't contain empty lines.
		    پیام_row = prev_پیام_row;
		    اگر (prev_پیام_row == Rows - 1)
			پیام_row--;
		}
		پیام_col = 0;
		print_سطر_no_prefix(curwin->w_cursor.lnum, FALSE, FALSE);
		پیام_clr_eos();
	    }
	}
	وگرنه اگر (ex_pressedreturn && !ex_no_reprint)	// must be at EOF
	{
	    اگر (curbuf->b_ml.ml_flags & ML_EMPTY)
		emsg(_(e_empty_buffer));
	    وگرنه
		emsg(_(e_at_end_of_file));
	}
    }

#ifdef FEAT_GUI
    --hold_gui_events;
#endif
    اگر (RedrawingDisabled > 0)
	--RedrawingDisabled;
    --no_wait_return;
    update_screen(UPD_CLEAR);
    need_wait_return = FALSE;
    پیام_scroll = save_پیام_scroll;
}

/*
 * Print the executed command for when 'verbose' is set.
 * When "lnum" is 0 only print the command.
 */
    ایستا تهی
پیام_verbose_cmd(linenr_T lnum, char_u *cmd)
{
    ++no_wait_return;
    verbose_enter_scroll();

    اگر (lnum == 0)
	smsg(_("Executing: %s"), cmd);
    وگرنه
	smsg(_("line %ld: %s"), (long)lnum, cmd);
    اگر (پیام_silent == 0)
	پیام_puts("\n");   // don't overwrite this

    verbose_leave_scroll();
    --no_wait_return;
}

/*
 * Execute a simple command line.  Used for translated commands like "*".
 */
    صحیح
do_cmdline_cmd(char_u *cmd)
{
    بازگشت do_cmdline(cmd, پوچ, پوچ,
				   DOCMD_VERBOSE|DOCMD_NOWAIT|DOCMD_KEYTYPED);
}

/*
 * Execute the "+cmd" argument of "edit +cmd fname" and the like.
 * This allows for using a range without ":" in Vim9 script.
 */
    ایستا صحیح
do_cmd_argument(char_u *cmd)
{
    بازگشت do_cmdline(cmd, پوچ, پوچ,
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
    صحیح
do_cmdline(
    char_u	*cmdline,
    char_u	*(*fgetline)(صحیح, تهی *, صحیح, getline_opt_T),
    تهی	*cookie,		// argument for fgetline()
    صحیح		flags)
{
    char_u	*next_cmdline;		// next cmd to execute
    char_u	*cmdline_copy = پوچ;	// copy of cmd line
    صحیح		used_getline = FALSE;	// used "fgetline" to obtain command
    ایستا صحیح	recursive = 0;		// recursive depth
    صحیح		پیام_didout_before_start = 0;
    صحیح		شمار = 0;		// line number count
    صحیح		did_inc_RedrawingDisabled = FALSE;
    صحیح		retval = OK;
#ifdef FEAT_EVAL
    cstack_T	cstack;			// conditional stack
    garray_T	سطرها_ga;		// keep lines for ":while"/":for"
    صحیح		current_سطر = 0;	// active line in lines_ga
    صحیح		current_سطر_before = 0;
    char_u	*fname = پوچ;		// function or script name
    linenr_T	*breakpoint = پوچ;	// ptr to breakpoint field in cookie
    صحیح		*dbg_tick = پوچ;	// ptr to dbg_tick field in cookie
    ساختار dbg_stuff debug_saved;	// saved things for debug mode
    صحیح		initial_trylevel;
    msglist_T	**saved_پیام_list = پوچ;
    msglist_T	*private_پیام_list = پوچ;

    // "fgetline" and "cookie" passed to do_one_cmd()
    char_u	*(*cmd_getline)(صحیح, تهی *, صحیح, getline_opt_T);
    تهی	*cmd_cookie;
    ساختار loop_cookie cmd_loop_cookie;
    تهی	*real_cookie;
    صحیح		getline_is_func;
#وگرنه
# define cmd_getline fgetline
# define cmd_cookie cookie
#endif
    ایستا صحیح	call_depth = 0;		// recursiveness
#ifdef FEAT_EVAL
    // For every pair of do_cmdline()/do_one_cmd() calls, use an extra memory
    // location for storing error messages to be converted to an exception.
    // This ensures that the do_errthrow() call in do_one_cmd() does not
    // combine the messages stored by an earlier invocation of do_one_cmd()
    // with the command name of the later one.  This would happen when
    // BufWritePost autocommands are executed after a write error.
    saved_پیام_list = پیام_list;
    پیام_list = &private_پیام_list;
#endif

    // It's possible to create an endless loop with ":execute", catch that
    // here.  The value of 200 allows nested function calls, ":source", etc.
    // Allow 200 or 'maxfuncdepth', whatever is larger.
    اگر (call_depth >= 200
#ifdef FEAT_EVAL
	    && call_depth >= p_mfd
#endif
	    )
    {
	emsg(_(e_command_too_recursive));
#ifdef FEAT_EVAL
	// When converting to an exception, we do not include the command name
	// since this is not an error of the specific command.
	do_errthrow((cstack_T *)پوچ, (char_u *)پوچ);
	پیام_list = saved_پیام_list;
#endif
	بازگشت FAIL;
    }
    ++call_depth;

#ifdef FEAT_EVAL
    CLEAR_FIELD(cstack);
    cstack.cs_idx = -1;
    ga_init2(&سطرها_ga, اندازه(wcmd_T), 10);

    real_cookie = getline_cookie(fgetline, cookie);

    // Inside a function use a higher nesting level.
    getline_is_func = getline_equal(fgetline, cookie, get_func_سطر);
    اگر (getline_is_func && ex_nesting_level == func_level(real_cookie))
	++ex_nesting_level;

    // Get the function or script name and the address where the next breakpoint
    // line and the debug tick for a function or script are stored.
    اگر (getline_is_func)
    {
	fname = func_name(real_cookie);
	breakpoint = func_breakpoint(real_cookie);
	dbg_tick = func_dbg_tick(real_cookie);
    }
    وگرنه اگر (getline_equal(fgetline, cookie, getsourceline))
    {
	fname = SOURCING_NAME;
	breakpoint = source_breakpoint(real_cookie);
	dbg_tick = source_dbg_tick(real_cookie);
    }

    /*
     * Initialize "force_abort"  and "suppress_errthrow" at the top level.
     */
    اگر (!recursive)
    {
	force_abort = FALSE;
	suppress_errthrow = FALSE;
    }

    /*
     * If requested, store and reset the global values controlling the
     * exception handling (used when debugging).  Otherwise clear it to avoid
     * a bogus compiler warning when the optimizer uses inline functions...
     */
    اگر (flags & DOCMD_EXCRESET)
	save_dbg_stuff(&debug_saved);
    وگرنه
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
    اگر (!(flags & DOCMD_KEYTYPED)
			       && !getline_equal(fgetline, cookie, getexline))
	کلید_Typed = FALSE;

    /*
     * Continue executing command lines:
     * - when inside an ":if", ":while" or ":for"
     * - for multiple commands on one line, separated with '|'
     * - when repeating until there are no more lines (for ":source")
     */
    next_cmdline = cmdline;
    انجامبده
    {
#ifdef FEAT_EVAL
	getline_is_func = getline_equal(fgetline, cookie, get_func_سطر);
#endif

	// stop skipping cmds for an error msg after all endif/while/for
	اگر (next_cmdline == پوچ
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
	اگر (cstack.cs_looplevel > 0 && current_سطر < سطرها_ga.ga_len)
	{
	    // Each '|' separated command is stored separately in lines_ga, to
	    // be able to jump to it.  Don't use next_cmdline now.
	    VIM_CLEAR(cmdline_copy);

	    // Check if a function has returned or, unless it has an unclosed
	    // try conditional, aborted.
	    اگر (getline_is_func)
	    {
# ifdef FEAT_PROFILE
		اگر (do_profiling == PROF_YES)
		    func_سطر_end(real_cookie);
# endif
		اگر (func_has_ended(real_cookie))
		{
		    retval = FAIL;
		    بشکن;
		}
	    }
# ifdef FEAT_PROFILE
	    وگرنه اگر (do_profiling == PROF_YES
			    && getline_equal(fgetline, cookie, getsourceline))
		script_سطر_end();
# endif

	    // Check if a sourced file hit a ":finish" command.
	    اگر (source_finished(fgetline, cookie))
	    {
		retval = FAIL;
		بشکن;
	    }

	    // If breakpoints have been added/deleted need to check for it.
	    اگر (breakpoint != پوچ && dbg_tick != پوچ
						   && *dbg_tick != debug_tick)
	    {
		*breakpoint = dbg_find_breakpoint(
				getline_equal(fgetline, cookie, getsourceline),
							fname, SOURCING_LNUM);
		*dbg_tick = debug_tick;
	    }

	    next_cmdline = ((wcmd_T *)(سطرها_ga.ga_data))[current_سطر].سطر;
	    SOURCING_LNUM = ((wcmd_T *)(سطرها_ga.ga_data))[current_سطر].lnum;

	    // Did we encounter a breakpoint?
	    اگر (breakpoint != پوچ && *breakpoint != 0
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
	    اگر (do_profiling == PROF_YES)
	    {
		اگر (getline_is_func)
		    func_سطر_start(real_cookie, SOURCING_LNUM);
		وگرنه اگر (getline_equal(fgetline, cookie, getsourceline))
		    script_سطر_start();
	    }
# endif
	}
#endif

	// 2. If no line given, get an allocated line with fgetline().
	اگر (next_cmdline == پوچ)
	{
	    /*
	     * Need to set msg_didout for the first line after an ":if",
	     * otherwise the ":if" will be overwritten.
	     */
	    اگر (شمار == 1 && getline_equal(fgetline, cookie, getexline))
		پیام_didout = TRUE;
	    اگر (fgetline == پوچ || (next_cmdline = fgetline(':', cookie,
#ifdef FEAT_EVAL
		    cstack.cs_idx < 0 ? 0 : (cstack.cs_idx + 1) * 2
#وگرنه
		    0
#endif
		    , in_vim9script() ? GETLINE_CONCAT_CONTBAR
					       : GETLINE_CONCAT_CONT)) == پوچ)
	    {
		// Don't call wait_return() for aborted command line.  The NULL
		// returned for the end of a sourced file or executed function
		// doesn't do this.
		اگر (کلید_Typed && !(flags & DOCMD_REPEAT))
		    need_wait_return = FALSE;
		retval = FAIL;
		بشکن;
	    }
	    used_getline = TRUE;

	    /*
	     * Keep the first typed line.  Clear it when more lines are typed.
	     */
	    اگر (flags & DOCMD_KEEPLINE)
	    {
		vim_free(repeat_cmdline);
		اگر (شمار == 0)
		    repeat_cmdline = vim_strsave(next_cmdline);
		وگرنه
		    repeat_cmdline = پوچ;
	    }
	}

	// 3. Make a copy of the command so we can mess with it.
	وگرنه اگر (cmdline_copy == پوچ)
	{
	    next_cmdline = vim_strsave(next_cmdline);
	    اگر (next_cmdline == پوچ)
	    {
		emsg(_(e_out_of_memory));
		retval = FAIL;
		بشکن;
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
	اگر ((cstack.cs_looplevel > 0 || has_loop_cmd(next_cmdline)))
	{
	    cmd_getline = get_loop_سطر;
	    cmd_cookie = (تهی *)&cmd_loop_cookie;
	    cmd_loop_cookie.سطرها_gap = &سطرها_ga;
	    cmd_loop_cookie.current_سطر = current_سطر;
	    cmd_loop_cookie.lc_getline = fgetline;
	    cmd_loop_cookie.cookie = cookie;
	    cmd_loop_cookie.repeating = (current_سطر < سطرها_ga.ga_len);

	    // Save the current line when encountering it the first time.
	    اگر (current_سطر == سطرها_ga.ga_len
		    && store_loop_سطر(&سطرها_ga, next_cmdline) == FAIL)
	    {
		retval = FAIL;
		بشکن;
	    }
	    current_سطر_before = current_سطر;
	}
	وگرنه
	{
	    cmd_getline = fgetline;
	    cmd_cookie = cookie;
	}

	did_endif = FALSE;
#endif

	اگر (شمار++ == 0)
	{
	    /*
	     * All output from the commands is put below each other, without
	     * waiting for a return. Don't do this when executing commands
	     * from a script or when being called recursive (e.g. for ":e
	     * +command file").
	     */
	    اگر (!(flags & DOCMD_NOWAIT) && !recursive)
	    {
		پیام_didout_before_start = پیام_didout;
		پیام_didany = FALSE; // no output yet
		پیام_start();
		پیام_scroll = TRUE;  // put messages below each other
		++no_wait_return;   // don't wait for return until finished
		++RedrawingDisabled;
		did_inc_RedrawingDisabled = TRUE;
	    }
	}

	اگر ((p_verbose >= 15 && SOURCING_NAME != پوچ) || p_verbose >= 16)
	    پیام_verbose_cmd(SOURCING_LNUM, cmdline_copy);

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
	اگر (cmd_cookie == (تهی *)&cmd_loop_cookie)
	    // Use "current_line" from "cmd_loop_cookie", it may have been
	    // incremented when defining a function.
	    current_سطر = cmd_loop_cookie.current_سطر;
#endif

	اگر (next_cmdline == پوچ)
	{
	    VIM_CLEAR(cmdline_copy);

	    /*
	     * If the command was typed, remember it for the ':' register.
	     * Do this AFTER executing the command to make :@: work.
	     */
	    اگر (getline_equal(fgetline, cookie, getexline)
						  && new_last_cmdline != پوچ)
	    {
		vim_free(last_cmdline);
		last_cmdline = new_last_cmdline;
		new_last_cmdline = پوچ;
	    }
	}
	وگرنه
	{
	    // need to copy the command after the '|' to cmdline_copy, for the
	    // next do_one_cmd()
	    STRMOVE(cmdline_copy, next_cmdline);
	    next_cmdline = cmdline_copy;
	}


#ifdef FEAT_EVAL
	// reset did_emsg for a function that is not aborted by an error
	اگر (did_emsg && !force_abort
		&& getline_equal(fgetline, cookie, get_func_سطر)
					      && !func_has_abort(real_cookie))
	{
	    // did_emsg_cumul is not set here
	    did_emsg = FALSE;
	}

	اگر (cstack.cs_looplevel > 0)
	{
	    ++current_سطر;

	    /*
	     * An ":endwhile", ":endfor" and ":continue" is handled here.
	     * If we were executing commands, jump back to the ":while" or
	     * ":for".
	     * If we were not executing commands, decrement cs_looplevel.
	     */
	    اگر (cstack.cs_lflags & (CSL_HAD_CONT | CSL_HAD_ENDLOOP))
	    {
		cstack.cs_lflags &= ~(CSL_HAD_CONT | CSL_HAD_ENDLOOP);

		// Jump back to the matching ":while" or ":for".  Be careful
		// not to use a cs_line[] from an entry that isn't a ":while"
		// or ":for": It would make "current_line" invalid and can
		// cause a crash.
		اگر (!did_emsg && !got_int && !did_throw
			&& cstack.cs_idx >= 0
			&& (cstack.cs_flags[cstack.cs_idx]
						      & (CSF_WHILE | CSF_FOR))
			&& cstack.cs_سطر[cstack.cs_idx] >= 0
			&& (cstack.cs_flags[cstack.cs_idx] & CSF_ACTIVE))
		{
		    current_سطر = cstack.cs_سطر[cstack.cs_idx];
						// remember we jumped there
		    cstack.cs_lflags |= CSL_HAD_LOOP;
		    سطر_breakcheck();		// check if CTRL-C typed

		    // Check for the next breakpoint at or after the ":while"
		    // or ":for".
		    اگر (breakpoint != پوچ && سطرها_ga.ga_len > current_سطر)
		    {
			*breakpoint = dbg_find_breakpoint(
			       getline_equal(fgetline, cookie, getsourceline),
									fname,
			   ((wcmd_T *)سطرها_ga.ga_data)[current_سطر].lnum-1);
			*dbg_tick = debug_tick;
		    }
		}
		وگرنه
		{
		    // can only get here with ":endwhile" or ":endfor"
		    اگر (cstack.cs_idx >= 0)
			rewind_conditionals(&cstack, cstack.cs_idx - 1,
				   CSF_WHILE | CSF_FOR, &cstack.cs_looplevel);
		}
	    }

	    /*
	     * For a ":while" or ":for" we need to remember the line number.
	     */
	    وگرنه اگر (cstack.cs_lflags & CSL_HAD_LOOP)
	    {
		cstack.cs_lflags &= ~CSL_HAD_LOOP;
		cstack.cs_سطر[cstack.cs_idx] = current_سطر_before;
	    }
	}

	// Check for the next breakpoint after a watchexpression
	اگر (breakpoint != پوچ && has_watchexpr())
	{
	    *breakpoint = dbg_find_breakpoint(FALSE, fname, SOURCING_LNUM);
	    *dbg_tick = debug_tick;
	}

	/*
	 * When not inside any ":while" loop, clear remembered lines.
	 */
	اگر (cstack.cs_looplevel == 0)
	{
	    اگر (سطرها_ga.ga_len > 0)
	    {
		SOURCING_LNUM =
		       ((wcmd_T *)سطرها_ga.ga_data)[سطرها_ga.ga_len - 1].lnum;
		free_cmdlines(&سطرها_ga);
	    }
	    current_سطر = 0;
	}

	/*
	 * A ":finally" makes did_emsg, got_int, and did_throw pending for
	 * being restored at the ":endtry".  Reset them here and set the
	 * ACTIVE and FINALLY flags, so that the finally clause gets executed.
	 * This includes the case where a missing ":endif", ":endwhile" or
	 * ":endfor" was detected by the ":finally" itself.
	 */
	اگر (cstack.cs_lflags & CSL_HAD_FINA)
	{
	    cstack.cs_lflags &= ~CSL_HAD_FINA;
	    report_make_pending(cstack.cs_pending[cstack.cs_idx]
		    & (CSTP_ERROR | CSTP_INTERRUPT | CSTP_THROW),
		    did_throw ? (تهی *)current_exception : پوچ);
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
	اگر (trylevel == 0 && !did_emsg && !got_int && !did_throw)
	    force_abort = FALSE;

	// Convert an interrupt to an exception if appropriate.
	(تهی)do_intthrow(&cstack);
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
    وقتی (!((got_int
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
	    && (next_cmdline != پوچ
#ifdef FEAT_EVAL
			|| cstack.cs_idx >= 0
#endif
			|| (flags & DOCMD_REPEAT)));

    vim_free(cmdline_copy);
    did_emsg_syntax = FALSE;
#ifdef FEAT_EVAL
    free_cmdlines(&سطرها_ga);
    ga_clear(&سطرها_ga);

    اگر (cstack.cs_idx >= 0)
    {
	/*
	 * If a sourced file or executed function ran to its end, report the
	 * unclosed conditional.
	 * In Vim9 script do not give a second error, executing aborts after
	 * the first one.
	 */
	اگر (!got_int && !did_throw && !aborting()
		&& !(did_emsg && in_vim9script())
		&& ((getline_equal(fgetline, cookie, getsourceline)
			&& !source_finished(fgetline, cookie))
		    || (getline_equal(fgetline, cookie, get_func_سطر)
					    && !func_has_ended(real_cookie))))
	{
	    اگر (cstack.cs_flags[cstack.cs_idx] & CSF_TRY)
		emsg(_(e_missing_endtry));
	    وگرنه اگر (cstack.cs_flags[cstack.cs_idx] & CSF_WHILE)
		emsg(_(e_missing_endwhile));
	    وگرنه اگر (cstack.cs_flags[cstack.cs_idx] & CSF_FOR)
		emsg(_(e_missing_endfor));
	    وگرنه
		emsg(_(e_missing_endif));
	}

	/*
	 * Reset "trylevel" in case of a ":finish" or ":return" or a missing
	 * ":endtry" in a sourced file or executed function.  If the try
	 * conditional is in its finally clause, ignore anything pending.
	 * If it is in a catch clause, finish the caught exception.
	 * Also cleanup any "cs_forinfo" structures.
	 */
	انجامبده
	{
	    صحیح idx = cleanup_conditionals(&cstack, 0, TRUE);

	    اگر (idx >= 0)
		--idx;	    // remove try block not in its finally clause
	    rewind_conditionals(&cstack, idx, CSF_WHILE | CSF_FOR,
							&cstack.cs_looplevel);
	}
	وقتی (cstack.cs_idx >= 0);
	trylevel = initial_trylevel;
    }

    // If a missing ":endtry", ":endwhile", ":endfor", or ":endif" or a memory
    // lack was reported above and the error message is to be converted to an
    // exception, do this now after rewinding the cstack.
    do_errthrow(&cstack, getline_equal(fgetline, cookie, get_func_سطر)
				  ? (char_u *)"endfunction" : (char_u *)پوچ);

    اگر (trylevel == 0)
    {
	// Just in case did_throw got set but current_exception wasn't.
	اگر (current_exception == پوچ)
	    did_throw = FALSE;

	/*
	 * When an exception is being thrown out of the outermost try
	 * conditional, discard the uncaught exception, disable the conversion
	 * of interrupts or errors to exceptions, and ensure that no more
	 * commands are executed.
	 */
	اگر (did_throw)
	    handle_did_throw();

	/*
	 * On an interrupt or an aborting error not converted to an exception,
	 * disable the conversion of errors to exceptions.  (Interrupts are not
	 * converted anymore, here.) This enables also the interrupt message
	 * when force_abort is set and did_emsg unset in case of an interrupt
	 * from a finally clause after an error.
	 */
	وگرنه اگر (got_int || (did_emsg && force_abort))
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
    اگر (did_throw)
	need_rethrow = TRUE;
    اگر ((getline_equal(fgetline, cookie, getsourceline)
		&& ex_nesting_level > source_level(real_cookie))
	    || (getline_equal(fgetline, cookie, get_func_سطر)
		&& ex_nesting_level > func_level(real_cookie) + 1))
    {
	اگر (!did_throw)
	    check_cstack = TRUE;
    }
    وگرنه
    {
	// When leaving a function, reduce nesting level.
	اگر (getline_equal(fgetline, cookie, get_func_سطر))
	    --ex_nesting_level;
	/*
	 * Go to debug mode when returning from a function in which we are
	 * single-stepping.
	 */
	اگر ((getline_equal(fgetline, cookie, getsourceline)
		    || getline_equal(fgetline, cookie, get_func_سطر))
		&& ex_nesting_level + 1 <= debug_break_level)
	    do_debug(getline_equal(fgetline, cookie, getsourceline)
		    ? (char_u *)_("End of sourced file")
		    : (char_u *)_("End of function"));
    }

    /*
     * Restore the exception environment (done after returning from the
     * debugger).
     */
    اگر (flags & DOCMD_EXCRESET)
	restore_dbg_stuff(&debug_saved);

    پیام_list = saved_پیام_list;

    // Cleanup if "cs_emsg_silent_list" remains.
    اگر (cstack.cs_emsg_silent_list != پوچ)
    {
	eslist_T *elem, *temp;

	برای (elem = cstack.cs_emsg_silent_list; elem != پوچ; elem = temp)
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
    اگر (did_inc_RedrawingDisabled)
    {
	اگر (RedrawingDisabled > 0)
	    --RedrawingDisabled;
	--no_wait_return;
	پیام_scroll = FALSE;

	/*
	 * When just finished an ":if"-":else" which was typed, no need to
	 * wait for hit-return.  Also for an error situation.
	 */
	اگر (retval == FAIL
#ifdef FEAT_EVAL
		|| (did_endif && کلید_Typed && !did_emsg)
#endif
					    )
	{
	    need_wait_return = FALSE;
	    پیام_didany = FALSE;		// don't wait when restarting edit
	}
	وگرنه اگر (need_wait_return)
	{
	    /*
	     * The msg_start() above clears msg_didout. The wait_return() we do
	     * here should not overwrite the command that may be shown before
	     * doing that.
	     */
	    پیام_didout |= پیام_didout_before_start;
	    wait_return(FALSE);
	}
    }

#ifdef FEAT_EVAL
    did_endif = FALSE;  // in case do_cmdline used recursively
#وگرنه
    /*
     * Reset if_level, in case a sourced script file contains more ":if" than
     * ":endif" (could be ":if x | foo | endif").
     */
    if_level = 0;
#endif

    --call_depth;
    بازگشت retval;
}

#اگر defined(FEAT_EVAL)
/*
 * Handle when "did_throw" is set after executing commands.
 */
    تهی
handle_did_throw(تهی)
{
    نویسه	*p = پوچ;
    msglist_T	*messages = پوچ;
    ESTACK_CHECK_DECLARATION;

    /*
     * If the uncaught exception is a user exception, report it as an
     * error.  If it is an error exception, display the saved error
     * message now.  For an interrupt exception, do nothing; the
     * interrupt message is given elsewhere.
     */
    انتخاب (current_exception->type)
    {
	حالت ET_USER:
	    vim_snprintf((نویسه *)IObuff, IOSIZE,
		    _(e_exception_not_caught_str),
		    current_exception->مقدار);
	    p = (نویسه *)vim_strsave(IObuff);
	    بشکن;
	حالت ET_ERROR:
	    messages = current_exception->messages;
	    current_exception->messages = پوچ;
	    بشکن;
	حالت ET_INTERRUPT:
	    بشکن;
    }

    estack_افزودن(ETYPE_EXCEPT, current_exception->throw_name,
					current_exception->throw_lnum);
    ESTACK_CHECK_SETUP;
    current_exception->throw_name = پوچ;

    discard_current_exception();	// uses IObuff if 'verbose'

    // If "silent!" is active the uncaught exception is not fatal.
    اگر (emsg_silent == 0)
    {
	suppress_errthrow = TRUE;
	force_abort = TRUE;
    }

    اگر (messages != پوچ)
    {
	انجامبده
	{
	    msglist_T	*next = messages->next;
	    صحیح		save_compiling = estack_compiling;

	    estack_compiling = messages->پیام_compiling;
	    emsg(messages->پیام);
	    vim_free(messages->پیام);
	    vim_free(messages->sfile);
	    vim_free(messages);
	    messages = next;
	    estack_compiling = save_compiling;
	}
	وقتی (messages != پوچ);
    }
    وگرنه اگر (p != پوچ)
    {
	emsg(p);
	vim_free(p);
    }
    vim_free(SOURCING_NAME);
    ESTACK_CHECK_NOW;
    estack_برداشت();
}

/*
 * Obtain a line when inside a ":while" or ":for" loop.
 */
    ایستا char_u *
get_loop_سطر(صحیح c, تهی *cookie, صحیح indent, getline_opt_T options)
{
    ساختار loop_cookie	*cp = (ساختار loop_cookie *)cookie;
    wcmd_T		*wp;
    char_u		*سطر;

    اگر (cp->current_سطر + 1 >= cp->سطرها_gap->ga_len)
    {
	اگر (cp->repeating)
	    بازگشت پوچ;	// trying to read past ":endwhile"/":endfor"

	// First time inside the ":while"/":for": get line normally.
	اگر (cp->lc_getline == پوچ)
	    سطر = getcmdline(c, 0L, indent, 0);
	وگرنه
	    سطر = cp->lc_getline(c, cp->cookie, indent, options);
	اگر (سطر != پوچ && store_loop_سطر(cp->سطرها_gap, سطر) == OK)
	    ++cp->current_سطر;

	بازگشت سطر;
    }

    کلید_Typed = FALSE;
    ++cp->current_سطر;
    wp = (wcmd_T *)(cp->سطرها_gap->ga_data) + cp->current_سطر;
    SOURCING_LNUM = wp->lnum;
    بازگشت vim_strsave(wp->سطر);
}

/*
 * Store a line in "gap" so that a ":while" loop can execute it again.
 */
    ایستا صحیح
store_loop_سطر(garray_T *gap, char_u *سطر)
{
    اگر (ga_grow(gap, 1) == FAIL)
	بازگشت FAIL;
    ((wcmd_T *)(gap->ga_data))[gap->ga_len].سطر = vim_strsave(سطر);
    ((wcmd_T *)(gap->ga_data))[gap->ga_len].lnum = SOURCING_LNUM;
    ++gap->ga_len;
    بازگشت OK;
}

/*
 * Free the lines stored for a ":while" or ":for" loop.
 */
    ایستا تهی
free_cmdlines(garray_T *gap)
{
    وقتی (gap->ga_len > 0)
    {
	vim_free(((wcmd_T *)(gap->ga_data))[gap->ga_len - 1].سطر);
	--gap->ga_len;
    }
}
#endif

/*
 * If "fgetline" is get_loop_line(), return TRUE if the getline it uses equals
 * "func".  * Otherwise return TRUE when "fgetline" equals "func".
 */
    صحیح
getline_equal(
    char_u	*(*fgetline)(صحیح, تهی *, صحیح, getline_opt_T),
    تهی	*cookie UNUSED,		// argument for fgetline()
    char_u	*(*func)(صحیح, تهی *, صحیح, getline_opt_T))
{
#ifdef FEAT_EVAL
    char_u		*(*gp)(صحیح, تهی *, صحیح, getline_opt_T);
    ساختار loop_cookie *cp;

    // When "fgetline" is "get_loop_line()" use the "cookie" to find the
    // function that's originally used to obtain the lines.  This may be
    // nested several levels.
    gp = fgetline;
    cp = (ساختار loop_cookie *)cookie;
    وقتی (gp == get_loop_سطر)
    {
	gp = cp->lc_getline;
	cp = cp->cookie;
    }
    بازگشت gp == func;
#وگرنه
    بازگشت fgetline == func;
#endif
}

/*
 * If "fgetline" is get_loop_line(), return the cookie used by the original
 * getline function.  Otherwise return "cookie".
 */
    تهی *
getline_cookie(
    char_u	*(*fgetline)(صحیح, تهی *, صحیح, getline_opt_T) UNUSED,
    تهی	*cookie)		// argument for fgetline()
{
#ifdef FEAT_EVAL
    char_u		*(*gp)(صحیح, تهی *, صحیح, getline_opt_T);
    ساختار loop_cookie  *cp;

    // When "fgetline" is "get_loop_line()" use the "cookie" to find the
    // cookie that's originally used to obtain the lines.  This may be nested
    // several levels.
    gp = fgetline;
    cp = (ساختار loop_cookie *)cookie;
    وقتی (gp == get_loop_سطر)
    {
	gp = cp->lc_getline;
	cp = cp->cookie;
    }
    بازگشت cp;
#وگرنه
    بازگشت cookie;
#endif
}

#اگر defined(FEAT_EVAL)
/*
 * Get the next line source line without advancing.
 */
    char_u *
getline_peek(
    char_u	*(*fgetline)(صحیح, تهی *, صحیح, getline_opt_T) UNUSED,
    تهی	*cookie)		// argument for fgetline()
{
    char_u		*(*gp)(صحیح, تهی *, صحیح, getline_opt_T);
    ساختار loop_cookie  *cp;
    wcmd_T		*wp;

    // When "fgetline" is "get_loop_line()" use the "cookie" to find the
    // cookie that's originally used to obtain the lines.  This may be nested
    // several levels.
    gp = fgetline;
    cp = (ساختار loop_cookie *)cookie;
    وقتی (gp == get_loop_سطر)
    {
	اگر (cp->current_سطر + 1 < cp->سطرها_gap->ga_len)
	{
	    // executing lines a second time, use the stored copy
	    wp = (wcmd_T *)(cp->سطرها_gap->ga_data) + cp->current_سطر + 1;
	    بازگشت wp->سطر;
	}
	gp = cp->lc_getline;
	cp = cp->cookie;
    }
    اگر (gp == getsourceline)
	بازگشت source_nextline(cp);
    بازگشت پوچ;
}
#endif


/*
 * Helper function to apply an offset for buffer commands, i.e. ":bdelete",
 * ":bwipeout", etc.
 * Returns the buffer number.
 */
    ایستا صحیح
compute_buffer_local_شمار(صحیح addr_type, صحیح lnum, صحیح offset)
{
    buf_T   *buf;
    buf_T   *nextbuf;
    صحیح     شمار = offset;

    buf = firstbuf;
    وقتی (buf->b_next != پوچ && buf->b_fnum < lnum)
	buf = buf->b_next;
    وقتی (شمار != 0)
    {
	شمار += (offset < 0) ? 1 : -1;
	nextbuf = (offset < 0) ? buf->b_prev : buf->b_next;
	اگر (nextbuf == پوچ)
	    بشکن;
	buf = nextbuf;
	اگر (addr_type == ADDR_LOADED_BUFFERS)
	    // skip over unloaded buffers
	    وقتی (buf->b_ml.ml_mfp == پوچ)
	    {
		nextbuf = (offset < 0) ? buf->b_prev : buf->b_next;
		اگر (nextbuf == پوچ)
		    بشکن;
		buf = nextbuf;
	    }
    }
    // we might have gone too far, last buffer is not loadedd
    اگر (addr_type == ADDR_LOADED_BUFFERS)
	وقتی (buf->b_ml.ml_mfp == پوچ)
	{
	    nextbuf = (offset >= 0) ? buf->b_prev : buf->b_next;
	    اگر (nextbuf == پوچ)
		بشکن;
	    buf = nextbuf;
	}
    بازگشت buf->b_fnum;
}

/*
 * Return the window number of "win".
 * When "win" is NULL return the number of windows.
 */
    ایستا صحیح
current_win_nr(win_T *win)
{
    win_T	*wp;
    صحیح		nr = 0;

    FOR_ALL_WINDOWS(wp)
    {
	++nr;
	اگر (wp == win)
	    بشکن;
    }
    بازگشت nr;
}

    ایستا صحیح
current_tab_nr(tabpage_T *tab)
{
    tabpage_T	*tp;
    صحیح		nr = 0;

    FOR_ALL_TABPAGES(tp)
    {
	++nr;
	اگر (tp == tab)
	    بشکن;
    }
    بازگشت nr;
}

    ایستا صحیح
comment_start(char_u *p, صحیح starts_with_colon UNUSED)
{
    اگر (in_vim9script())
	بازگشت p[0] == '#' && !starts_with_colon;
    بازگشت *p == '"';
}

#define CURRENT_WIN_NR current_win_nr(curwin)
#define LAST_WIN_NR current_win_nr(پوچ)
#define CURRENT_TAB_NR current_tab_nr(curtab)
#define LAST_TAB_NR current_tab_nr(پوچ)

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
    ایستا char_u *
do_one_cmd(
    char_u	**cmdlinep,
    صحیح		flags,
#ifdef FEAT_EVAL
    cstack_T	*cstack,
#endif
    char_u	*(*fgetline)(صحیح, تهی *, صحیح, getline_opt_T),
    تهی	*cookie)		// argument for fgetline()
{
    char_u	*p;
    linenr_T	lnum;
    long	n;
    نویسه	*errormsg = پوچ;	// error message
    char_u	*after_modifier = پوچ;
    exarg_T	ea;			// Ex command arguments
    cmdmod_T	save_cmdmod;
    صحیح		save_reg_executing = reg_executing;
    صحیح		save_pending_end_reg_executing = pending_end_reg_executing;
    صحیح		ni;			// set when Not Implemented
    char_u	*cmd;
    صحیح		starts_with_colon = FALSE;
    صحیح		may_have_range;
#ifdef FEAT_EVAL
    صحیح		did_set_عبارت_سطر = FALSE;
#endif
    صحیح		sourcing = flags & DOCMD_VERBOSE;
    صحیح		did_append_cmd = FALSE;

    CLEAR_FIELD(ea);
    ea.line1 = 1;
    ea.line2 = 1;
#ifdef FEAT_EVAL
    ++ex_nesting_level;
#endif

    // When the last file has not been edited :q has to be typed twice.
    اگر (quitmore
#ifdef FEAT_EVAL
	    // avoid that a function call in 'statusline' does this
	    && !getline_equal(fgetline, cookie, get_func_سطر)
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
    اگر ((*cmdlinep)[0] == '#' && (*cmdlinep)[1] == '!')
	برو doend;

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
    اگر (parse_command_modifiers(&ea, &errormsg, &cmdmod, FALSE) == FAIL)
	برو doend;
    apply_cmdmod(&cmdmod);
    after_modifier = ea.cmd;

#ifdef FEAT_EVAL
    ea.skip = did_emsg || got_int || did_throw || (cstack->cs_idx >= 0
			 && !(cstack->cs_flags[cstack->cs_idx] & CSF_ACTIVE));
#وگرنه
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
    صحیح vim9script = in_vim9script();
    اگر (vim9script && (flags & DOCMD_RANGEOK) == 0)
    {
	may_have_range = FALSE;
	برای (p = ea.cmd; p >= *cmdlinep; --p)
	{
	    اگر (*p == ':')
		may_have_range = TRUE;
	    اگر (p < ea.cmd && !VIM_ISWHITE(*p))
		بشکن;
	}
    }
    وگرنه
	may_have_range = TRUE;
    اگر (may_have_range)
	ea.cmd = skip_range(ea.cmd, TRUE, پوچ);

#ifdef FEAT_EVAL
    // Handle ":export" - it functions almost like a command modifier.
    // ":export var Name: type"
    // ":export def Name(..."
    // etc.
    اگر (vim9script && checkforcmd_noparen(&ea.cmd, "export", 6))
	is_export = TRUE;
#endif

    اگر (vim9script && !may_have_range)
    {
	اگر (ea.cmd == cmd + 1 && *cmd == '$')
	    // should be "$VAR = val"
	    --ea.cmd;
#ifdef FEAT_EVAL
	p = find_ex_command(&ea, پوچ, lookup_scriptitem, پوچ);
#وگرنه
	p = find_ex_command(&ea, پوچ, پوچ, پوچ);
#endif
	اگر (ea.cmdidx == CMD_SIZE)
	{
	    char_u *ar = skip_range(ea.cmd, TRUE, پوچ);

	    // If a ':' before the range is missing, give a clearer error
	    // message.
	    اگر (ar > ea.cmd && !ea.skip)
	    {
		semsg(_(e_colon_required_before_range_str), ea.cmd);
		برو doend;
	    }
	}
    }
    وگرنه
	p = find_ex_command(&ea, پوچ, پوچ, پوچ);

#ifdef FEAT_EVAL
# ifdef FEAT_PROFILE
    // Count this line for profiling if skip is TRUE.
    اگر (do_profiling == PROF_YES
	    && (!ea.skip || cstack->cs_idx == 0 || (cstack->cs_idx > 0
		     && (cstack->cs_flags[cstack->cs_idx - 1] & CSF_ACTIVE))))
    {
	صحیح skip = did_emsg || got_int || did_throw;

	اگر (ea.cmdidx == CMD_catch)
	    skip = !skip && !(cstack->cs_idx >= 0
			  && (cstack->cs_flags[cstack->cs_idx] & CSF_THROWN)
			  && !(cstack->cs_flags[cstack->cs_idx] & CSF_CAUGHT));
	وگرنه اگر (ea.cmdidx == CMD_else || ea.cmdidx == CMD_elseif)
	    skip = skip || !(cstack->cs_idx >= 0
			  && !(cstack->cs_flags[cstack->cs_idx]
						  & (CSF_ACTIVE | CSF_TRUE)));
	وگرنه اگر (ea.cmdidx == CMD_finally)
	    skip = FALSE;
	وگرنه اگر (ea.cmdidx != CMD_endif
		&& ea.cmdidx != CMD_endfor
		&& ea.cmdidx != CMD_endtry
		&& ea.cmdidx != CMD_endwhile)
	    skip = ea.skip;

	اگر (!skip)
	{
	    اگر (getline_equal(fgetline, cookie, get_func_سطر))
		func_سطر_exec(getline_cookie(fgetline, cookie));
	    وگرنه اگر (getline_equal(fgetline, cookie, getsourceline))
		script_سطر_exec();
	}
    }
# endif
#endif

    ea.cmd = cmd;

#ifdef FEAT_EVAL
    // May go to debug mode.  If this happens and the ">quit" debug command is
    // used, throw an interrupt exception and skip the next command.
    dbg_check_breakpoint(&ea);
    اگر (!ea.skip && got_int)
    {
	ea.skip = TRUE;
	(تهی)do_intthrow(cstack);
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
    اگر (!IS_USER_CMDIDX(ea.cmdidx))
    {
	اگر (ea.cmdidx != CMD_SIZE)
	    ea.addr_type = cmdnames[(صحیح)ea.cmdidx].cmd_addr_type;
	وگرنه
	    ea.addr_type = ADDR_سطرها;

	// :wincmd range depends on the argument.
	اگر (ea.cmdidx == CMD_wincmd && p != پوچ)
	    get_wincmd_addr_type(skipwhite(p), &ea);
#ifdef FEAT_QUICKFIX
	// :.cc in quickfix window uses line number
	اگر ((ea.cmdidx == CMD_cc || ea.cmdidx == CMD_ll) && bt_quickfix(curbuf))
	    ea.addr_type = ADDR_OTHER;
#endif
    }

    اگر (!may_have_range)
	ea.line1 = ea.line2 = default_address(&ea);
    وگرنه اگر (parse_cmd_address(&ea, &errormsg, FALSE) == FAIL)
	برو doend;

/*
 * 5. Parse the command.
 */

    /*
     * Skip ':' and any white space
     */
    ea.cmd = skipwhite(ea.cmd);
    وقتی (*ea.cmd == ':')
	ea.cmd = skipwhite(ea.cmd + 1);

    /*
     * If we got a line, but no command, then go to the line.
     * If we find a '|' or '\n' we set ea.nextcmd.
     */
    اگر (*ea.cmd == NUL || comment_start(ea.cmd, starts_with_colon)
			       || (ea.nextcmd = check_nextcmd(ea.cmd)) != پوچ)
    {
	/*
	 * strange vi behaviour:
	 * ":3"		jumps to line 3
	 * ":3|..."	prints line 3  (not in Vim9 script)
	 * ":|"		prints current line  (not in Vim9 script)
	 */
	اگر (ea.skip)	    // skip this if inside :if
	    برو doend;
	errormsg = ex_range_without_command(&ea);
	برو doend;
    }

    // If this looks like an undefined user command and there are CmdUndefined
    // autocommands defined, trigger the matching autocommands.
    اگر (p != پوچ && ea.cmdidx == CMD_SIZE && !ea.skip
	    && ASCII_ISUPPER(*ea.cmd)
	    && has_cmdundefined())
    {
	صحیح ret;

	p = ea.cmd;
	وقتی (ASCII_ISALNUM(*p))
	    ++p;
	p = vim_strnsave(ea.cmd, p - ea.cmd);
	ret = apply_autocmds(EVENT_CMDUNDEFINED, p, p, TRUE, پوچ);
	vim_free(p);
	// If the autocommands did something and didn't cause an error, try
	// finding the command again.
	p = (ret
#ifdef FEAT_EVAL
		&& !aborting()
#endif
		) ? find_ex_command(&ea, پوچ, پوچ, پوچ) : ea.cmd;
    }

    اگر (p == پوچ)
    {
	اگر (!ea.skip)
	    errormsg = _(e_ambiguous_use_of_user_defined_command);
	برو doend;
    }
    // Check for wrong commands.
    اگر (*p == '!' && ea.cmd[1] == 0151 && ea.cmd[0] == 78
	    && !IS_USER_CMDIDX(ea.cmdidx))
    {
	errormsg = uc_fun_cmd();
	برو doend;
    }

    اگر (ea.cmdidx == CMD_SIZE)
    {
	اگر (!ea.skip)
	{
	    STRCPY(IObuff, _(e_not_an_editor_command));
	    اگر (!sourcing)
	    {
		// If the modifier was parsed OK the error must be in the
		// following command
		اگر (after_modifier != پوچ)
		    append_command(after_modifier);
		وگرنه
		    append_command(*cmdlinep);
		did_append_cmd = TRUE;
	    }
	    errormsg = (نویسه *)IObuff;
	    did_emsg_syntax = TRUE;
	}
	برو doend;
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
    اگر (ea.cmdidx == CMD_if)
	++if_level;
    اگر (if_level)
    {
	اگر (ea.cmdidx == CMD_endif)
	    --if_level;
	برو doend;
    }

#endif

    // forced commands
    اگر (*p == '!' && ea.cmdidx != CMD_substitute
	    && ea.cmdidx != CMD_smagic && ea.cmdidx != CMD_snomagic)
    {
	++p;
	ea.forceit = TRUE;
    }
    وگرنه
	ea.forceit = FALSE;

/*
 * 6. Parse arguments.  Then check for errors.
 */
    اگر (!IS_USER_CMDIDX(ea.cmdidx))
	ea.argt = (long)cmdnames[(صحیح)ea.cmdidx].cmd_argt;

    اگر (!ea.skip)
    {
#ifdef HAVE_SANDBOX
	اگر (sandbox != 0 && !(ea.argt & EX_SBOXOK))
	{
	    // Command not allowed in sandbox.
	    errormsg = _(e_not_allowed_in_sandbox);
	    برو doend;
	}
#endif
	اگر (restricted != 0 && (ea.argt & EX_RESTRICT))
	{
	    errormsg = _(e_command_not_allowed_in_rvim);
	    برو doend;
	}
	اگر (!curbuf->b_p_ma && (ea.argt & EX_MODIFY))
	{
	    // Command not allowed in non-'modifiable' buffer
	    errormsg = _(e_cannot_make_changes_modifiable_is_off);
	    برو doend;
	}

	اگر (!IS_USER_CMDIDX(ea.cmdidx))
	{
	    اگر (cmdwin_type != 0 && !(ea.argt & EX_CMDWIN))
	    {
		// Command not allowed in the command line window
		errormsg = _(e_invalid_in_cmdline_window);
		برو doend;
	    }
	    اگر (text_locked() && !(ea.argt & EX_LOCK_OK))
	    {
		// Command not allowed when text is locked
		errormsg = _(get_text_locked_پیام());
		برو doend;
	    }
	}

	// Disallow editing another buffer when "curbuf_lock" is set.
	// Do allow ":checktime" (it is postponed).
	// Do allow ":edit" (check for an argument later).
	// Do allow ":file" with no arguments (check for an argument later).
	اگر (!(ea.argt & (EX_CMDWIN | EX_LOCK_OK))
		&& ea.cmdidx != CMD_checktime
		&& ea.cmdidx != CMD_edit
		&& ea.cmdidx != CMD_file
		&& !IS_USER_CMDIDX(ea.cmdidx)
		&& curbuf_locked())
	    برو doend;

	اگر (!ni && !(ea.argt & EX_RANGE) && ea.addr_شمار > 0)
	{
	    errormsg = _(e_no_range_allowed);
	    برو doend;
	}
    }

    اگر (!ni && !(ea.argt & EX_BANG) && ea.forceit)
    {
	errormsg = _(e_no_bang_allowed);
	برو doend;
    }

    /*
     * Don't complain about the range if it is not used
     * (could happen if line_count is accidentally set to 0).
     */
    اگر (!ea.skip && !ni && (ea.argt & EX_RANGE))
    {
	/*
	 * If the range is backwards, ask for confirmation and, if given, swap
	 * ea.line1 & ea.line2 so it's forwards again.
	 * When global command is busy, don't ask, will fail below.
	 */
	اگر (!global_busy && ea.line1 > ea.line2)
	{
	    اگر (پیام_silent == 0)
	    {
		اگر (sourcing || exmode_active)
		{
		    errormsg = _(e_backwards_range_given);
		    برو doend;
		}
		اگر (ask_yesno((char_u *)
			_("Backwards range given, OK to swap"), FALSE) != 'y')
		    برو doend;
	    }
	    lnum = ea.line1;
	    ea.line1 = ea.line2;
	    ea.line2 = lnum;
	}
	اگر ((errormsg = invalid_range(&ea)) != پوچ)
	    برو doend;
    }

    اگر ((ea.addr_type == ADDR_OTHER) && ea.addr_شمار == 0)
	// default is 1, not cursor
	ea.line2 = 1;

    correct_range(&ea);

#ifdef FEAT_FOLDING
    اگر (((ea.argt & EX_WHOLEFOLD) || ea.addr_شمار >= 2) && !global_busy
	    && ea.addr_type == ADDR_سطرها)
    {
	// Put the first line at the start of a closed fold, put the last line
	// at the end of a closed fold.
	(تهی)hasFolding(ea.line1, &ea.line1, پوچ);
	(تهی)hasFolding(ea.line2, پوچ, &ea.line2);
    }
#endif

#ifdef FEAT_QUICKFIX
    /*
     * For the ":make" and ":grep" commands we insert the 'makeprg'/'grepprg'
     * option here, so things like % get expanded.
     */
    p = replace_makeprg(&ea, p, cmdlinep);
    اگر (p == پوچ)
	برو doend;
#endif

    /*
     * Skip to start of argument.
     * Don't do this for the ":!" command, because ":!! -l" needs the space.
     */
    اگر (ea.cmdidx == CMD_bang)
	ea.arg = p;
    وگرنه
	ea.arg = skipwhite(p);

    // ":file" cannot be run with an argument when "curbuf_lock" is set
    اگر (ea.cmdidx == CMD_file && *ea.arg != NUL && curbuf_locked())
	برو doend;

    /*
     * Check for "++opt=val" argument.
     * Must be first, allow ":w ++enc=utf8 !cmd"
     */
    اگر (ea.argt & EX_ARGOPT)
	وقتی (ea.arg[0] == '+' && ea.arg[1] == '+')
	    اگر (getargopt(&ea) == FAIL && !ni)
	    {
		errormsg = _(e_invalid_argument);
		برو doend;
	    }

    اگر (ea.cmdidx == CMD_write || ea.cmdidx == CMD_update)
    {
	اگر (*ea.arg == '>')			// append
	{
	    اگر (*++ea.arg != '>')		// typed wrong
	    {
		errormsg = _(e_use_w_or_w_gt_gt);
		برو doend;
	    }
	    ea.arg = skipwhite(ea.arg + 1);
	    ea.append = TRUE;
	}
	وگرنه اگر (*ea.arg == '!' && ea.cmdidx == CMD_write)  // :w !filter
	{
	    ++ea.arg;
	    ea.usefilter = TRUE;
	}
    }

    اگر (ea.cmdidx == CMD_read)
    {
	اگر (ea.forceit)
	{
	    ea.usefilter = TRUE;		// :r! filter if ea.forceit
	    ea.forceit = FALSE;
	}
	وگرنه اگر (*ea.arg == '!')		// :r !filter
	{
	    ++ea.arg;
	    ea.usefilter = TRUE;
	}
    }

    اگر (ea.cmdidx == CMD_lshift || ea.cmdidx == CMD_rshift)
    {
	ea.amount = 1;
	وقتی (*ea.arg == *ea.cmd)		// count number of '>' or '<'
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
    اگر ((ea.argt & EX_CMDARG) && !ea.usefilter)
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
    اگر ((ea.argt & EX_TRLBAR) && !ea.usefilter)
    {
	separate_nextcmd(&ea, FALSE);
    }
    وگرنه اگر (ea.cmdidx == CMD_bang
	    || ea.cmdidx == CMD_terminal
	    || ea.cmdidx == CMD_global
	    || ea.cmdidx == CMD_vglobal
	    || ea.usefilter
#ifdef FEAT_EVAL
	    || inside_block(&ea)
#endif
	    )
    {
	برای (p = ea.arg; *p; ++p)
	{
	    // Remove one backslash before a newline
	    اگر (*p == '\\' && p[1] == '\n')
		STRMOVE(p, p + 1);
	    وگرنه اگر (*p == '\n' && !(ea.argt & EX_عبارت_ARG))
	    {
		ea.nextcmd = p + 1;
		*p = NUL;
		بشکن;
	    }
	}
    }

    اگر ((ea.argt & EX_DFLALL) && ea.addr_شمار == 0)
	address_default_all(&ea);

    // accept numbered register only when no count allowed (:put)
    اگر (       (ea.argt & EX_REGSTR)
	    && *ea.arg != NUL
	       // Do not allow register = for user commands
	    && (!IS_USER_CMDIDX(ea.cmdidx) || *ea.arg != '=')
	    && !((ea.argt & EX_شمار) && VIM_ISDIGIT(*ea.arg)))
    {
#اگر !defined(FEAT_CLIPBOARD) && !defined(FEAT_CLIPBOARD_PROVIDER)
	// check these explicitly for a more specific error message
	اگر (*ea.arg == '*' || *ea.arg == '+')
	{
	    errormsg = _(e_invalid_register_name);
	    برو doend;
	}
#endif
	اگر (valid_yank_reg(*ea.arg, (!IS_USER_CMDIDX(ea.cmdidx)
			    && ea.cmdidx != CMD_put && ea.cmdidx != CMD_iput)))
	{
	    ea.regname = *ea.arg++;
#ifdef FEAT_EVAL
	    // for '=' register: accept the rest of the line as an expression
	    اگر (ea.arg[-1] == '=' && ea.arg[0] != NUL)
	    {
		اگر (!ea.skip)
		{
		    set_عبارت_سطر(vim_strsave(ea.arg), &ea);
		    did_set_عبارت_سطر = TRUE;
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
    اگر ((ea.argt & EX_شمار) && VIM_ISDIGIT(*ea.arg)
	    && (!(ea.argt & EX_BUFNAME) || *(p = skipdigits(ea.arg + 1)) == NUL
							  || VIM_ISWHITE(*p)))
    {
	n = getdigits_quoted(&ea.arg);
	ea.arg = skipwhite(ea.arg);
	اگر (n <= 0 && !ni && (ea.argt & EX_ZEROR) == 0)
	{
	    errormsg = _(e_positive_شمار_required);
	    برو doend;
	}
	اگر (ea.addr_type != ADDR_سطرها)	// e.g. :buffer 2, :sleep 3
	{
	    ea.line2 = n;
	    اگر (ea.addr_شمار == 0)
		ea.addr_شمار = 1;
	}
	وگرنه
	{
	    ea.line1 = ea.line2;
	    اگر (ea.line2 >= LONG_MAX - (n - 1))
		ea.line2 = LONG_MAX;  // avoid overflow
	    وگرنه
		ea.line2 += n - 1;
	    ++ea.addr_شمار;
	    /*
	     * Be vi compatible: no error message for out of range.
	     */
	    اگر (ea.line2 > curbuf->b_ml.ml_سطر_شمار)
		ea.line2 = curbuf->b_ml.ml_سطر_شمار;
	}
    }

    /*
     * Check for flags: 'l', 'p' and '#'.
     */
    اگر (ea.argt & EX_FLAGS)
	get_flags(&ea);
    اگر (!ni && !(ea.argt & EX_EXTRA) && *ea.arg != NUL
	    && *ea.arg != '"' && (*ea.arg != '|' || (ea.argt & EX_TRLBAR) == 0))
    {
	// no arguments allowed but there is something
	errormsg = ex_errmsg(e_trailing_characters_str, ea.arg);
	برو doend;
    }

    اگر (!ni && (ea.argt & EX_NEEDARG) && *ea.arg == NUL)
    {
	errormsg = _(e_argument_required);
	برو doend;
    }

#ifdef FEAT_EVAL
    /*
     * Skip the command when it's not going to be executed.
     * The commands like :if, :endif, etc. always need to be executed.
     * Also make an exception for commands that handle a trailing command
     * themselves.
     */
    اگر (ea.skip)
    {
	انتخاب (ea.cmdidx)
	{
	    // commands that need evaluation
	    حالت CMD_while:
	    حالت CMD_endwhile:
	    حالت CMD_for:
	    حالت CMD_endfor:
	    حالت CMD_if:
	    حالت CMD_elseif:
	    حالت CMD_else:
	    حالت CMD_endif:
	    حالت CMD_try:
	    حالت CMD_catch:
	    حالت CMD_finally:
	    حالت CMD_endtry:
	    حالت CMD_function:
	    حالت CMD_def:
				بشکن;

	    // commands that read a block of lines
	    حالت CMD_abstract:
	    حالت CMD_append:
	    حالت CMD_autocmd:
	    حالت CMD_change:
	    حالت CMD_class:
	    حالت CMD_command:
	    حالت CMD_enum:
	    حالت CMD_درج:
	    حالت CMD_interface:
	    حالت CMD_loadkeymap:
				بشکن;

	    // Commands that handle '|' themselves.  Check: A command should
	    // either have the EX_TRLBAR flag, appear in this list or appear in
	    // the list at ":help :bar".
	    حالت CMD_aboveleft:
	    حالت CMD_and:
	    حالت CMD_belowright:
	    حالت CMD_botright:
	    حالت CMD_browse:
	    حالت CMD_call:
	    حالت CMD_confirm:
	    حالت CMD_const:
	    حالت CMD_delfunction:
	    حالت CMD_djump:
	    حالت CMD_dlist:
	    حالت CMD_dsearch:
	    حالت CMD_dsplit:
	    حالت CMD_echo:
	    حالت CMD_echoerr:
	    حالت CMD_echomsg:
	    حالت CMD_echon:
	    حالت CMD_eval:
	    حالت CMD_execute:
	    حالت CMD_filter:
	    حالت CMD_final:
	    حالت CMD_help:
	    حالت CMD_hide:
	    حالت CMD_horizontal:
	    حالت CMD_ijump:
	    حالت CMD_ilist:
	    حالت CMD_isearch:
	    حالت CMD_isplit:
	    حالت CMD_keepalt:
	    حالت CMD_keepjumps:
	    حالت CMD_keepmarks:
	    حالت CMD_keeppatterns:
	    حالت CMD_leftabove:
	    حالت CMD_let:
	    حالت CMD_lockmarks:
	    حالت CMD_lockvar:
	    حالت CMD_lua:
	    حالت CMD_match:
	    حالت CMD_mzscheme:
	    حالت CMD_noautocmd:
	    حالت CMD_noswapfile:
	    حالت CMD_perl:
	    حالت CMD_psearch:
	    حالت CMD_py3:
	    حالت CMD_python3:
	    حالت CMD_python:
	    حالت CMD_return:
	    حالت CMD_rightbelow:
	    حالت CMD_ruby:
	    حالت CMD_silent:
	    حالت CMD_smagic:
	    حالت CMD_snomagic:
	    حالت CMD_substitute:
	    حالت CMD_syntax:
	    حالت CMD_tab:
	    حالت CMD_tcl:
	    حالت CMD_throw:
	    حالت CMD_tilde:
	    حالت CMD_topleft:
	    حالت CMD_unlet:
	    حالت CMD_unlockvar:
	    حالت CMD_var:
	    حالت CMD_verbose:
	    حالت CMD_vertical:
	    حالت CMD_wincmd:
				بشکن;

	    پیشفرض:		برو doend;
	}
    }
#endif

    اگر ((ea.argt & EX_XFILE)
	    && expand_filename(&ea, cmdlinep, &errormsg) == FAIL)
	برو doend;

#ifdef FEAT_EVAL
    اگر (is_export && (ea.argt & EX_EXPORT) == 0)
    {
	emsg(_(e_invalid_command_after_export));
	برو doend;
    }
#endif

    /*
     * Accept buffer name.  Cannot be used at the same time with a buffer
     * number.  Don't do this for a user command.
     */
    اگر ((ea.argt & EX_BUFNAME) && *ea.arg != NUL && ea.addr_شمار == 0
	    && !IS_USER_CMDIDX(ea.cmdidx))
    {
	/*
	 * :bdelete, :bwipeout and :bunload take several arguments, separated
	 * by spaces: find next space (skipping over escaped characters).
	 * The others take one argument: ignore trailing spaces.
	 */
	اگر (ea.cmdidx == CMD_bdelete || ea.cmdidx == CMD_bwipeout
						  || ea.cmdidx == CMD_bunload)
	    p = skiptowhite_esc(ea.arg);
	وگرنه
	{
	    p = ea.arg + STRLEN(ea.arg);
	    وقتی (p > ea.arg && VIM_ISWHITE(p[-1]))
		--p;
	}
	ea.line2 = buflist_findpat(ea.arg, p, (ea.argt & EX_BUFUNL) != 0,
								FALSE, FALSE);
	اگر (ea.line2 < 0)	    // failed
	    برو doend;
	ea.addr_شمار = 1;
	ea.arg = skipwhite(p);
    }

    // The :try command saves the emsg_silent flag, reset it here when
    // ":silent! try" was used, it should only apply to :try itself.
    اگر (ea.cmdidx == CMD_try && cmdmod.cmod_did_esilent > 0)
    {
	emsg_silent -= cmdmod.cmod_did_esilent;
	اگر (emsg_silent < 0)
	    emsg_silent = 0;
	cmdmod.cmod_did_esilent = 0;
    }

/*
 * 7. Execute the command.
 */

    اگر (IS_USER_CMDIDX(ea.cmdidx))
    {
	/*
	 * Execute a user-defined command.
	 */
	do_ucmd(&ea);
    }
    وگرنه
    {
	/*
	 * Call the function to execute the builtin command.
	 */
	(cmdnames[ea.cmdidx].cmd_func)(&ea);
	اگر (ea.errmsg != پوچ)
	    errormsg = ea.errmsg;
    }

#ifdef FEAT_EVAL
    // A command will reset "is_export" when exporting an item.  If it is still
    // set something went wrong or the command was never executed.
    اگر (!ea.skip && is_export)
    {
	اگر (errormsg == پوچ)
	    errormsg = _(e_export_with_invalid_argument);
	is_export = FALSE;
    }

    // Set flag that any command was executed, used by ex_vim9script().
    // Not if this was a command that wasn't executed or :endif.
    اگر (sourcing_a_script(&ea)
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
    اگر (need_rethrow)
	do_throw(cstack);
    وگرنه اگر (check_cstack)
    {
	اگر (source_finished(fgetline, cookie))
	    do_finish(&ea, TRUE);
	وگرنه اگر (getline_equal(fgetline, cookie, get_func_سطر)
						   && current_func_returned())
	    do_return(&ea, TRUE, FALSE, پوچ);
    }
    need_rethrow = check_cstack = FALSE;
#endif

doend:
    اگر (curwin->w_cursor.lnum == 0)	// can happen with zero line number
    {
	curwin->w_cursor.lnum = 1;
	curwin->w_cursor.col = 0;
    }

    اگر (errormsg != پوچ && *errormsg != NUL && !did_emsg)
    {
	اگر ((sourcing || !کلید_Typed) && !did_append_cmd)
	{
	    اگر (errormsg != (نویسه *)IObuff)
	    {
		STRCPY(IObuff, errormsg);
		errormsg = (نویسه *)IObuff;
	    }
	    append_command(*cmdlinep);
	}
	emsg(errormsg);
    }
#ifdef FEAT_EVAL
    do_errthrow(cstack,
	    (ea.cmdidx != CMD_SIZE && !IS_USER_CMDIDX(ea.cmdidx))
			? cmdnames[(صحیح)ea.cmdidx].cmd_name : (char_u *)پوچ);

    اگر (did_set_عبارت_سطر)
	set_عبارت_سطر(پوچ, پوچ);
    is_export = FALSE;
#endif

    undo_cmdmod(&cmdmod);
    cmdmod = save_cmdmod;
    reg_executing = save_reg_executing;
    pending_end_reg_executing = save_pending_end_reg_executing;

    اگر (ea.nextcmd && *ea.nextcmd == NUL)	// not really a next command
	ea.nextcmd = پوچ;

#ifdef FEAT_EVAL
    --ex_nesting_level;
    vim_free(ea.cmdline_tofree);
#endif

    بازگشت ea.nextcmd;
}

ایستا نویسه ex_error_buf[پیام_BUF_LEN];

/*
 * Return an error message with argument included.
 * Uses a static buffer, only the last error will be kept.
 * "msg" will be translated, caller should use N_().
 */
    نویسه *
ex_errmsg(نویسه *پیام, char_u *arg)
{
    vim_snprintf(ex_error_buf, پیام_BUF_LEN, _(پیام), arg);
    بازگشت ex_error_buf;
}

/*
 * The "+" string used in place of an empty command in Ex mode.
 * This string is used in pointer comparison.
 */
ایستا ثابت نویسه exmode_plus[] = "+";

/*
 * Handle a range without a command.
 * Returns an error message on failure.
 */
    نویسه *
ex_range_without_command(exarg_T *eap)
{
    نویسه *errormsg = پوچ;

    اگر ((*eap->cmd == '|' ||
		(exmode_active && eap->cmd != (char_u *)exmode_plus + 1))
#ifdef FEAT_EVAL
	    && !in_vim9script()
#endif
       )
    {
	eap->cmdidx = CMD_print;
	eap->argt = EX_RANGE+EX_شمار+EX_TRLBAR;
	اگر ((errormsg = invalid_range(eap)) == پوچ)
	{
	    correct_range(eap);
	    ex_print(eap);
	}
    }
    وگرنه اگر (eap->addr_شمار != 0)
    {
	اگر (eap->line2 > curbuf->b_ml.ml_سطر_شمار)
	{
	    // With '-' in 'cpoptions' a line number past the file is an
	    // error, otherwise put it at the end of the file.
	    اگر (vim_strchr(p_cpo, CPO_MINUS) != پوچ)
		eap->line2 = -1;
	    وگرنه
		eap->line2 = curbuf->b_ml.ml_سطر_شمار;
	}

	اگر (eap->line2 < 0)
	    errormsg = _(e_invalid_range);
	وگرنه
	{
	    اگر (eap->line2 == 0)
		curwin->w_cursor.lnum = 1;
	    وگرنه
		curwin->w_cursor.lnum = eap->line2;
	    beginline(BL_SOL | BL_FIX);
	}
    }
    بازگشت errormsg;
}

/*
 * Check for an Ex command with optional tail.
 * If there is a match advance "pp" to the argument and return TRUE.
 * If "noparen" is TRUE do not recognize the command followed by "(" or ".".
 */
    ایستا صحیح
checkforcmd_opt(
    char_u	**pp,		// start of command
    نویسه	*cmd,		// name of command
    صحیح		len,		// required length
    صحیح		noparen)
{
    صحیح		i;

    برای (i = 0; cmd[i] != NUL; ++i)
	اگر (((char_u *)cmd)[i] != (*pp)[i])
	    بشکن;
    اگر (i >= len && !ASCII_ISALPHA((*pp)[i]) && (*pp)[i] != '_'
			 && (!noparen || ((*pp)[i] != '(' && (*pp)[i] != '.')))
    {
	*pp = skipwhite(*pp + i);
	بازگشت TRUE;
    }
    بازگشت FALSE;
}

/*
 * Check for an Ex command with optional tail.
 * If there is a match advance "pp" to the argument and return TRUE.
 */
    صحیح
checkforcmd(
    char_u	**pp,		// start of command
    نویسه	*cmd,		// name of command
    صحیح		len)		// required length
{
    بازگشت checkforcmd_opt(pp, cmd, len, FALSE);
}

/*
 * Check for an Ex command with optional tail, not followed by "(" or ".".
 * If there is a match advance "pp" to the argument and return TRUE.
 */
    صحیح
checkforcmd_noparen(
    char_u	**pp,		// start of command
    نویسه	*cmd,		// name of command
    صحیح		len)		// required length
{
    بازگشت checkforcmd_opt(pp, cmd, len, TRUE);
}

/*
 * Find the replacement text of a ":command", the part after the attributes and
 * the command name.  Returns NULL when there is none.  Does not modify "arg"
 * and gives no error messages.
 */
    ایستا char_u *
find_ucmd_repl(char_u *arg)
{
    char_u	*p = arg;

    // Skip over the attributes.
    وقتی (*p == '-')
	p = skipwhite(skiptowhite(p));

    // Skip over the command name.
    اگر (!ASCII_ISALPHA(*p))
	بازگشت پوچ;
    وقتی (ASCII_ISALNUM(*p))
	++p;

    بازگشت *p == NUL ? پوچ : skipwhite(p);
}

/*
 * Find the "{" in "line" that starts a block for ":command" or ":autocmd".
 * That is the case when the command argument is "{" at the end of the line,
 * also when the command is nested in another ":command" or ":autocmd".
 * Returns NULL when the line does not start such a block.
 */
    char_u *
find_cmd_block_start(char_u *سطر)
{
    char_u	*p = skipwhite(سطر);

    برای (;;)
    {
	char_u	*arg = p;

	اگر (*p == '{' && ends_excmd2(p, skipwhite(p + 1)))
	    بازگشت p;

	اگر (checkforcmd_noparen(&arg, "autocmd", 2))
	{
	    اگر (*arg == '!')
		arg = skipwhite(arg + 1);
	    p = au_find_cmd_arg(arg);
	}
	وگرنه اگر (checkforcmd_noparen(&arg, "command", 3))
	{
	    اگر (*arg == '!')
		arg = skipwhite(arg + 1);
	    p = find_ucmd_repl(arg);
	}
	وگرنه
	    بازگشت پوچ;

	اگر (p == پوچ)
	    بازگشت پوچ;
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
    صحیح
parse_command_modifiers(
	exarg_T	    *eap,
	نویسه	    **errormsg,
	cmdmod_T    *cmod,
	صحیح	    skip_only)
{
    char_u  *orig_cmd = eap->cmd;
    char_u  *cmd_start = پوچ;
    صحیح	    use_plus_cmd = FALSE;
    صحیح	    starts_with_colon = FALSE;
    صحیح	    vim9script = in_vim9script();
    صحیح	    has_visual_range = FALSE;

    CLEAR_POINTER(cmod);
    cmod->cmod_flags = sticky_cmdmod_flags;

    اگر (STRNCMP(eap->cmd, "'<,'>", 5) == 0)
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
    برای (;;)
    {
	char_u  *p;

	وقتی (*eap->cmd == ' ' || *eap->cmd == '\t' || *eap->cmd == ':')
	{
	    اگر (*eap->cmd == ':')
		starts_with_colon = TRUE;
	    ++eap->cmd;
	}

	// in ex mode, an empty command (after modifiers) works like :+
	اگر (*eap->cmd == NUL && exmode_active
		   && (getline_equal(eap->ea_getline, eap->cookie, getexmodeline)
		       || getline_equal(eap->ea_getline, eap->cookie, getexline))
			&& curwin->w_cursor.lnum < curbuf->b_ml.ml_سطر_شمار)
	{
	    use_plus_cmd = TRUE;
	    اگر (!skip_only)
		ex_pressedreturn = TRUE;
	    بشکن;  // no modifiers following
	}

	// ignore comment and empty lines
	اگر (comment_start(eap->cmd, starts_with_colon))
	{
	    // a comment ends at a NL
	    eap->nextcmd = vim_strchr(eap->cmd, '\n');
	    اگر (eap->nextcmd != پوچ)
		++eap->nextcmd;
	    اگر (vim9script)
	    {
		اگر (has_cmdmod(cmod, FALSE))
		    *errormsg = _(e_command_modifier_without_command);
#ifdef FEAT_EVAL
		اگر (eap->cmd[0] == '#' && eap->cmd[1] == '{'
							 && eap->cmd[2] != '{')
		    *errormsg = _(e_cannot_use_hash_curly_to_start_comment);
#endif
	    }
	    بازگشت FAIL;
	}
	اگر (*eap->cmd == '\n')
	{
	    eap->nextcmd = eap->cmd + 1;
	    بازگشت FAIL;
	}
	اگر (*eap->cmd == NUL)
	{
	    اگر (!skip_only)
	    {
		ex_pressedreturn = TRUE;
		اگر (vim9script && has_cmdmod(cmod, FALSE))
		    *errormsg = _(e_command_modifier_without_command);
	    }
	    بازگشت FAIL;
	}

	p = skip_range(eap->cmd, TRUE, پوچ);

	// In Vim9 script a variable can shadow a command modifier:
	//   verbose = 123
	//   verbose += 123
	//   silent! verbose = func()
	//   verbose.member = 2
	//   verbose[expr] = 2
	// But not:
	//   verbose [a, b] = list
	اگر (vim9script)
	{
	    char_u *s, *n;

	    برای (s = eap->cmd; ASCII_ISALPHA(*s); ++s)
		;
	    n = skipwhite(s);
	    اگر (*n == '.' || *n == '=' || (*n != NUL && n[1] == '=')
		    || *s == '[')
		بشکن;
	}

	انتخاب (*p)
	{
	    // When adding an entry, also modify cmdmod_info_tab[].
	    حالت 'a':	اگر (!checkforcmd_noparen(&eap->cmd, "aboveleft", 3))
			    بشکن;
			cmod->cmod_split |= WSP_ABOVE;
			ادامه;

	    حالت 'b':	اگر (checkforcmd_noparen(&eap->cmd, "belowright", 3))
			{
			    cmod->cmod_split |= WSP_BELOW;
			    ادامه;
			}
			اگر (checkforcmd_opt(&eap->cmd, "browse", 3, TRUE))
			{
#ifdef FEAT_BROWSE_CMD
			    cmod->cmod_flags |= CMOD_BROWSE;
#endif
			    ادامه;
			}
			اگر (!checkforcmd_noparen(&eap->cmd, "botright", 2))
			    بشکن;
			cmod->cmod_split |= WSP_BOT;
			ادامه;

	    حالت 'c':	اگر (!checkforcmd_opt(&eap->cmd, "confirm", 4, TRUE))
			    بشکن;
#اگر defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
			cmod->cmod_flags |= CMOD_CONFIRM;
#endif
			ادامه;

	    حالت 'k':	اگر (checkforcmd_noparen(&eap->cmd, "keepmarks", 3))
			{
			    cmod->cmod_flags |= CMOD_KEEPMARKS;
			    ادامه;
			}
			اگر (checkforcmd_noparen(&eap->cmd, "keepalt", 5))
			{
			    cmod->cmod_flags |= CMOD_KEEPALT;
			    ادامه;
			}
			اگر (checkforcmd_noparen(&eap->cmd, "keeppatterns", 5))
			{
			    cmod->cmod_flags |= CMOD_KEEPPATTERNS;
			    ادامه;
			}
			اگر (!checkforcmd_noparen(&eap->cmd, "keepjumps", 5))
			    بشکن;
			cmod->cmod_flags |= CMOD_KEEPJUMPS;
			ادامه;

	    حالت 'f':	// only accept ":filter {pat} cmd"
			{
			    char_u  *reg_pat;
			    char_u  *nulp = پوچ;
			    صحیح	    c = 0;

			    اگر (!checkforcmd_noparen(&p, "filter", 4)
				    || *p == NUL
				    || (ends_excmd(*p)
#ifdef FEAT_EVAL
					// in ":filter #pat# cmd" # does not
					// start a comment
				     && (!vim9script || VIM_ISWHITE(p[1]))
#endif
				     ))
				بشکن;
			    اگر (*p == '!')
			    {
				cmod->cmod_filter_force = TRUE;
				p = skipwhite(p + 1);
				اگر (*p == NUL || ends_excmd(*p))
				    بشکن;
			    }
#ifdef FEAT_EVAL
			    // Avoid that "filter(arg)" is recognized.
			    اگر (vim9script && !VIM_ISWHITE(p[-1]))
				بشکن;
#endif
			    اگر (skip_only)
				p = skip_vimgrep_pat(p, پوچ, پوچ);
			    وگرنه
				// NOTE: This puts a NUL after the pattern.
				p = skip_vimgrep_pat_ext(p, &reg_pat, پوچ,
								    &nulp, &c);
			    اگر (p == پوچ || *p == NUL)
				بشکن;
			    اگر (!skip_only)
			    {
				cmod->cmod_filter_regmatch.regprog =
						vim_regcomp(reg_pat, RE_MAGIC);
				اگر (cmod->cmod_filter_regmatch.regprog == پوچ)
				    بشکن;
				// restore the character overwritten by NUL
				اگر (nulp != پوچ)
				    *nulp = c;
			    }
			    eap->cmd = p;
			    ادامه;
			}

	    حالت 'h':	اگر (checkforcmd_noparen(&eap->cmd, "horizontal", 3))
			{
			    cmod->cmod_split |= WSP_HOR;
			    ادامه;
			}
			// ":hide" and ":hide | cmd" are not modifiers
			اگر (p != eap->cmd || !checkforcmd_noparen(&p, "hide", 3)
					       || *p == NUL || ends_excmd(*p))
			    بشکن;
			eap->cmd = p;
			cmod->cmod_flags |= CMOD_HIDE;
			ادامه;

	    حالت 'l':	اگر (checkforcmd_noparen(&eap->cmd, "lockmarks", 3))
			{
			    cmod->cmod_flags |= CMOD_LOCKMARKS;
			    ادامه;
			}
			اگر (checkforcmd_noparen(&eap->cmd, "legacy", 3))
			{
			    اگر (ends_excmd2(p, eap->cmd))
			    {
				*errormsg =
				      _(e_legacy_must_be_followed_by_command);
				بازگشت FAIL;
			    }
			    // Make sure we do not have both legacy and Vim9
			    // flags set at the same time.
			    cmod->cmod_flags &= ~CMOD_VIM9CMD;
			    cmod->cmod_flags |= CMOD_LEGACY;
			    ادامه;
			}

			اگر (!checkforcmd_noparen(&eap->cmd, "leftabove", 5))
			    بشکن;
			cmod->cmod_split |= WSP_ABOVE;
			ادامه;

	    حالت 'n':	اگر (checkforcmd_noparen(&eap->cmd, "noautocmd", 3))
			{
			    cmod->cmod_flags |= CMOD_NOAUTOCMD;
			    ادامه;
			}
			اگر (!checkforcmd_noparen(&eap->cmd, "noswapfile", 3))
			    بشکن;
			cmod->cmod_flags |= CMOD_NOSWAPFILE;
			ادامه;

	    حالت 'r':	اگر (!checkforcmd_noparen(&eap->cmd, "rightbelow", 6))
			    بشکن;
			cmod->cmod_split |= WSP_BELOW;
			ادامه;

	    حالت 's':	اگر (checkforcmd_noparen(&eap->cmd, "sandbox", 3))
			{
			    cmod->cmod_flags |= CMOD_SANDBOX;
			    ادامه;
			}
			اگر (!checkforcmd_noparen(&eap->cmd, "silent", 3))
			    بشکن;
			cmod->cmod_flags |= CMOD_SILENT;
			اگر (*eap->cmd == '!' && !VIM_ISWHITE(eap->cmd[-1]))
			{
			    // ":silent!", but not "silent !cmd"
			    eap->cmd = skipwhite(eap->cmd + 1);
			    cmod->cmod_flags |= CMOD_ERRSILENT;
			}
			ادامه;

	    حالت 't':	اگر (checkforcmd_noparen(&p, "tab", 3))
			{
			    اگر (!skip_only)
			    {
				long tabnr = get_address(eap, &eap->cmd,
						    ADDR_TABS, eap->skip,
						    skip_only, FALSE, 1);
				اگر (tabnr == MAXLNUM)
				    cmod->cmod_tab = tabpage_index(curtab) + 1;
				وگرنه
				{
				    اگر (tabnr < 0 || tabnr > LAST_TAB_NR)
				    {
					*errormsg = _(e_invalid_range);
					بازگشت FAIL;
				    }
				    cmod->cmod_tab = tabnr + 1;
				}
			    }
			    eap->cmd = p;
			    ادامه;
			}
			اگر (!checkforcmd_noparen(&eap->cmd, "topleft", 2))
			    بشکن;
			cmod->cmod_split |= WSP_بالاترین;
			ادامه;

	    حالت 'u':	اگر (!checkforcmd_noparen(&eap->cmd, "unsilent", 3))
			    بشکن;
			cmod->cmod_flags |= CMOD_UNSILENT;
			ادامه;

	    حالت 'v':	اگر (checkforcmd_noparen(&eap->cmd, "vertical", 4))
			{
			    cmod->cmod_split |= WSP_VERT;
			    ادامه;
			}
			اگر (checkforcmd_noparen(&eap->cmd, "vim9cmd", 4))
			{
			    اگر (ends_excmd2(p, eap->cmd))
			    {
				*errormsg =
				      _(e_vim9cmd_must_be_followed_by_command);
				بازگشت FAIL;
			    }
			    // Make sure we do not have both legacy and Vim9
			    // flags set at the same time.
			    cmod->cmod_flags &= ~CMOD_LEGACY;
			    cmod->cmod_flags |= CMOD_VIM9CMD;
			    ادامه;
			}
			اگر (!checkforcmd_noparen(&p, "verbose", 4))
			    بشکن;
			اگر (vim_isdigit(*eap->cmd))
			{
			    // zero means not set, one is verbose == 0, etc.
			    cmod->cmod_verbose = atoi((نویسه *)eap->cmd) + 1;
			}
			وگرنه
			    cmod->cmod_verbose = 2;  // default: verbose == 1
			eap->cmd = p;
			ادامه;
	}
	بشکن;
    }

    اگر (has_visual_range)
    {
	اگر (eap->cmd > cmd_start)
	{
	    // Move the '<,'> range to after the modifiers and insert a colon.
	    // Since the modifiers have been parsed put the colon on top of the
	    // space: "'<,'>mod cmd" -> "mod:'<,'>cmd
	    // Put eap->cmd after the colon.
	    اگر (use_plus_cmd)
	    {
		size_t len = STRLEN(cmd_start);

		// Special case: empty command uses "+":
		//  "'<,'>mods" -> "mods *+
		//  Use "*" instead of "'<,'>" to avoid the command getting
		//  longer, in case it was allocated.
		mch_memmove(orig_cmd, cmd_start, len);
		STRCPY(orig_cmd + len, " *+");
	    }
	    وگرنه
	    {
		mch_memmove(cmd_start - 5, cmd_start, eap->cmd - cmd_start);
		eap->cmd -= 5;
		mch_memmove(eap->cmd - 1, ":'<,'>", 6);
	    }
	}
	وگرنه
	    // No modifiers, move the pointer back.
	    // Special case: change empty command to "+".
	    اگر (use_plus_cmd)
		eap->cmd = (char_u *)"'<,'>+";
	    وگرنه
		eap->cmd = orig_cmd;
    }
    وگرنه اگر (use_plus_cmd)
	eap->cmd = (char_u *)exmode_plus;

    بازگشت OK;
}

/*
 * Return TRUE if "cmod" has anything set.
 */
    صحیح
has_cmdmod(cmdmod_T *cmod, صحیح ignore_silent)
{
    بازگشت (cmod->cmod_flags != 0 && (!ignore_silent
		|| (cmod->cmod_flags
		      & ~(CMOD_SILENT | CMOD_ERRSILENT | CMOD_UNSILENT)) != 0))
	    || cmod->cmod_split != 0
	    || cmod->cmod_verbose > 0
	    || cmod->cmod_tab != 0
	    || cmod->cmod_filter_regmatch.regprog != پوچ;
}

#اگر defined(FEAT_EVAL)
/*
 * If Vim9 script and "cmdmod" has anything set give an error and return TRUE.
 */
    صحیح
cmdmod_error(صحیح ignore_silent)
{
    اگر (in_vim9script() && has_cmdmod(&cmdmod, ignore_silent))
    {
	emsg(_(e_misplaced_command_modifier));
	بازگشت TRUE;
    }
    بازگشت FALSE;
}
#endif

/*
 * Apply the command modifiers.  Saves current state in "cmdmod", call
 * undo_cmdmod() later.
 */
    تهی
apply_cmdmod(cmdmod_T *cmod)
{
#ifdef HAVE_SANDBOX
    اگر ((cmod->cmod_flags & CMOD_SANDBOX) && !cmod->cmod_did_sandbox)
    {
	++sandbox;
	cmod->cmod_did_sandbox = TRUE;
    }
#endif
    اگر (cmod->cmod_verbose > 0)
    {
	اگر (cmod->cmod_verbose_save == 0)
	    cmod->cmod_verbose_save = p_verbose + 1;
	p_verbose = cmod->cmod_verbose - 1;
    }

    اگر ((cmod->cmod_flags & (CMOD_SILENT | CMOD_UNSILENT))
	    && cmod->cmod_save_پیام_silent == 0)
    {
	cmod->cmod_save_پیام_silent = پیام_silent + 1;
	cmod->cmod_save_پیام_scroll = پیام_scroll;
    }
    اگر (cmod->cmod_flags & CMOD_SILENT)
	++پیام_silent;
    اگر (cmod->cmod_flags & CMOD_UNSILENT)
	پیام_silent = 0;

    اگر (cmod->cmod_flags & CMOD_ERRSILENT)
    {
	++emsg_silent;
	++cmod->cmod_did_esilent;
    }

    اگر ((cmod->cmod_flags & CMOD_NOAUTOCMD) && cmod->cmod_save_ei == پوچ)
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
    تهی
undo_cmdmod(cmdmod_T *cmod)
{
    اگر (cmod->cmod_verbose_save > 0)
    {
	p_verbose = cmod->cmod_verbose_save - 1;
	cmod->cmod_verbose_save = 0;
    }

#ifdef HAVE_SANDBOX
    اگر (cmod->cmod_did_sandbox)
    {
	--sandbox;
	cmod->cmod_did_sandbox = FALSE;
    }
#endif

    اگر (cmod->cmod_save_ei != پوچ)
    {
	// Restore 'eventignore' to the value before ":noautocmd".
	set_string_option_direct((char_u *)"ei", -1, cmod->cmod_save_ei,
							   OPT_FREE, SID_NONE);
	free_string_option(cmod->cmod_save_ei);
	cmod->cmod_save_ei = پوچ;
    }

    vim_regfree(cmod->cmod_filter_regmatch.regprog);

    اگر (cmod->cmod_save_پیام_silent > 0)
    {
	// messages could be enabled for a serious error, need to check if the
	// counters don't become negative
	اگر (!did_emsg || پیام_silent > cmod->cmod_save_پیام_silent - 1)
	    پیام_silent = cmod->cmod_save_پیام_silent - 1;
	emsg_silent -= cmod->cmod_did_esilent;
	اگر (emsg_silent < 0)
	    emsg_silent = 0;
	// Restore msg_scroll, it's set by file I/O commands, even when no
	// message is actually displayed.
	پیام_scroll = cmod->cmod_save_پیام_scroll;

	// "silent reg" or "silent echo x" inside "redir" leaves msg_col
	// somewhere in the line.  Put it back in the first column.
	اگر (redirecting())
	    پیام_col = 0;

	cmod->cmod_save_پیام_silent = 0;
	cmod->cmod_did_esilent = 0;
    }
}

/*
 * Parse the address range, if any, in "eap".
 * May set the last search pattern, unless "silent" is TRUE.
 * Return FAIL and set "errormsg" or return OK.
 */
    صحیح
parse_cmd_address(exarg_T *eap, نویسه **errormsg, صحیح silent)
{
    صحیح		address_شمار = 1;
    linenr_T	lnum;
    صحیح		need_check_cursor = FALSE;
    صحیح		ret = FAIL;

    // Repeat for all ',' or ';' separated addresses.
    برای (;;)
    {
	eap->line1 = eap->line2;
	eap->line2 = default_address(eap);
	eap->cmd = skipwhite(eap->cmd);
	lnum = get_address(eap, &eap->cmd, eap->addr_type, eap->skip, silent,
					eap->addr_شمار == 0, address_شمار++);
	اگر (eap->cmd == پوچ)	// error detected
	    برو theend;
	اگر (lnum == MAXLNUM)
	{
	    اگر (*eap->cmd == '%')   // '%' - all lines
	    {
		++eap->cmd;
		انتخاب (eap->addr_type)
		{
		    حالت ADDR_سطرها:
		    حالت ADDR_OTHER:
			eap->line1 = 1;
			eap->line2 = curbuf->b_ml.ml_سطر_شمار;
			بشکن;
		    حالت ADDR_LOADED_BUFFERS:
			{
			    buf_T	*buf = firstbuf;

			    وقتی (buf->b_next != پوچ
						  && buf->b_ml.ml_mfp == پوچ)
				buf = buf->b_next;
			    eap->line1 = buf->b_fnum;
			    buf = lastbuf;
			    وقتی (buf->b_prev != پوچ
						  && buf->b_ml.ml_mfp == پوچ)
				buf = buf->b_prev;
			    eap->line2 = buf->b_fnum;
			    بشکن;
			}
		    حالت ADDR_BUFFERS:
			eap->line1 = firstbuf->b_fnum;
			eap->line2 = lastbuf->b_fnum;
			بشکن;
		    حالت ADDR_WINDOWS:
		    حالت ADDR_TABS:
			اگر (IS_USER_CMDIDX(eap->cmdidx))
			{
			    eap->line1 = 1;
			    eap->line2 = eap->addr_type == ADDR_WINDOWS
						  ? LAST_WIN_NR : LAST_TAB_NR;
			}
			وگرنه
			{
			    // there is no Vim command which uses '%' and
			    // ADDR_WINDOWS or ADDR_TABS
			    *errormsg = _(e_invalid_range);
			    برو theend;
			}
			بشکن;
		    حالت ADDR_TABS_RELATIVE:
		    حالت ADDR_UNSIGNED:
		    حالت ADDR_QUICKFIX:
			*errormsg = _(e_invalid_range);
			برو theend;
		    حالت ADDR_ARGUMENTS:
			اگر (ARGCOUNT == 0)
			    eap->line1 = eap->line2 = 0;
			وگرنه
			{
			    eap->line1 = 1;
			    eap->line2 = ARGCOUNT;
			}
			بشکن;
		    حالت ADDR_QUICKFIX_VALID:
#ifdef FEAT_QUICKFIX
			eap->line1 = 1;
			eap->line2 = qf_get_valid_size(eap);
			اگر (eap->line2 == 0)
			    eap->line2 = 1;
#endif
			بشکن;
		    حالت ADDR_NONE:
			// Will give an error later if a range is found.
			بشکن;
		}
		++eap->addr_شمار;
	    }
	    وگرنه اگر (*eap->cmd == '*' && vim_strchr(p_cpo, CPO_STAR) == پوچ)
	    {
		pos_T	    *fp;

		// '*' - visual area
		اگر (eap->addr_type != ADDR_سطرها)
		{
		    *errormsg = _(e_invalid_range);
		    برو theend;
		}

		++eap->cmd;
		اگر (!eap->skip)
		{
		    fp = getmark('<', FALSE);
		    اگر (check_mark(fp) == FAIL)
			برو theend;
		    eap->line1 = fp->lnum;
		    fp = getmark('>', FALSE);
		    اگر (check_mark(fp) == FAIL)
			برو theend;
		    eap->line2 = fp->lnum;
		    ++eap->addr_شمار;
		}
	    }
	}
	وگرنه
	    eap->line2 = lnum;
	eap->addr_شمار++;

	اگر (*eap->cmd == ';')
	{
	    اگر (!eap->skip)
	    {
		curwin->w_cursor.lnum = eap->line2;

		// Don't leave the cursor on an illegal line or column, but do
		// accept zero as address, so 0;/PATTERN/ works correctly
		// (where zero usually means to use the first line).
		// Check the cursor position before returning.
		اگر (eap->line2 > 0)
		    check_cursor();
		وگرنه
		    check_cursor_col();
		need_check_cursor = TRUE;
	    }
	}
	وگرنه اگر (*eap->cmd != ',')
	    بشکن;
	++eap->cmd;
    }

    // One address given: set start and end lines.
    اگر (eap->addr_شمار == 1)
    {
	eap->line1 = eap->line2;
	// ... but only implicit: really no address given
	اگر (lnum == MAXLNUM)
	    eap->addr_شمار = 0;
    }
    ret = OK;

theend:
    اگر (need_check_cursor)
	check_cursor();
    بازگشت ret;
}

/*
 * Append "cmd" to the error message in IObuff.
 * Takes care of limiting the length and handling 0xa0, which would be
 * invisible otherwise.
 */
    ایستا تهی
append_command(char_u *cmd)
{
    size_t  len = STRLEN(IObuff);
    char_u  *s = cmd;
    char_u  *d;

    اگر (len > IOSIZE - 100)
    {
	// Not enough space, truncate and put in "...".
	d = IObuff + IOSIZE - 100;
	d -= mb_head_off(IObuff, d);
	STRCPY(d, "...");
    }
    STRCAT(IObuff, ": ");
    d = IObuff + STRLEN(IObuff);
    وقتی (*s != NUL && d - IObuff + 5 < IOSIZE)
    {
	اگر (enc_utf8 ? (s[0] == 0xc2 && s[1] == 0xa0) : *s == 0xa0)
	{
	    s += enc_utf8 ? 2 : 1;
	    STRCPY(d, "<a0>");
	    d += 4;
	}
	وگرنه اگر (d - IObuff + (*mb_ptr2len)(s) + 1 >= IOSIZE)
	    بشکن;
	وگرنه
	    MB_COPY_CHAR(s, d);
    }
    *d = NUL;
}

#اگر defined(FEAT_EVAL)
/*
 * If "start" points "&opt", "&l:opt", "&g:opt" or "$ENV" return a pointer to
 * the name.  Otherwise just return "start".
 */
    char_u *
skip_option_env_lead(char_u *start)
{
    char_u *name = start;

    اگر (*start == '&')
    {
	اگر ((start[1] == 'l' || start[1] == 'g') && start[2] == ':')
	    name += 3;
	وگرنه
	    name += 1;
    }
    وگرنه اگر (*start == '$')
	name += 1;
    بازگشت name;
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
    ایستا صحیح
one_letter_cmd(char_u *p, cmdidx_T *idx)
{
    اگر (in_vim9script())
	بازگشت FALSE;
    اگر (p[0] == 'k'
	    && (p[1] != 'e' || (p[1] == 'e' && p[2] != 'e')))
    {
	*idx = CMD_k;
	بازگشت TRUE;
    }
    اگر (p[0] == 's'
	    && ((p[1] == 'c' && (p[2] == NUL || (p[2] != 's' && p[2] != 'r'
			&& (p[3] == NUL || (p[3] != 'i' && p[4] != 'p')))))
		|| p[1] == 'g'
		|| (p[1] == 'i' && p[2] != 'm' && p[2] != 'l' && p[2] != 'g')
		|| p[1] == 'I'
		|| (p[1] == 'r' && p[2] != 'e')))
    {
	*idx = CMD_substitute;
	بازگشت TRUE;
    }
    بازگشت FALSE;
}

#اگر defined(FEAT_EVAL)
/*
 * Return TRUE if "cmd" starts with "123->", a number followed by a method
 * call.
 */
    صحیح
عدد_method(char_u *cmd)
{
    char_u *p = skipdigits(cmd);

    بازگشت p > cmd && (p = skipwhite(p))[0] == '-' && p[1] == '>';
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
	صحیح	*full UNUSED,
	صحیح	(*lookup)(char_u *, size_t, صحیح cmd, cctx_T *) UNUSED,
	cctx_T	*cctx UNUSED)
{
    صحیح		len;
    char_u	*p;
    صحیح		i;
#ifndef FEAT_EVAL
    صحیح		vim9 = FALSE;
#وگرنه
    صحیح		vim9 = in_vim9script();

    /*
     * Recognize a Vim9 script function/method call and assignment:
     * "lvar = value", "lvar(arg)", "[1, 2 3]->Func()"
     */
    p = eap->cmd;
    اگر (lookup != پوچ)
    {
	char_u *pskip = skip_option_env_lead(eap->cmd);

	اگر (vim_strchr((char_u *)"{('[\"@&$", *p) != پوچ
	       || ((p = to_name_const_end(pskip)) > eap->cmd && *p != NUL)
	       || (p[0] == '0' && p[1] == 'z'))
	{
	    صحیح	    oplen;
	    صحیح	    heredoc;
	    char_u  *swp;

	    اگر (*eap->cmd == '&'
		    || (eap->cmd[0] == '$'
				  && eap->cmd[1] != '\'' && eap->cmd[1] != '"')
		    || (eap->cmd[0] == '@'
					&& (valid_yank_reg(eap->cmd[1], FALSE)
						       || eap->cmd[1] == '@')))
	    {
		اگر (*eap->cmd == '&')
		{
		    p = eap->cmd + 1;
		    اگر (STRNCMP("l:", p, 2) == 0 || STRNCMP("g:", p, 2) == 0)
			p += 2;
		    p = to_name_end(p, FALSE);
		}
		وگرنه اگر (*eap->cmd == '$')
		    p = to_name_end(eap->cmd + 1, FALSE);
		وگرنه
		    p = eap->cmd + 2;
		اگر (ends_excmd(*skipwhite(p)))
		{
		    // "&option <NL>", "$ENV <NL>" and "@r <NL>" are the start
		    // of an expression.
		    eap->cmdidx = CMD_eval;
		    بازگشت eap->cmd;
		}
		// "&option" can be followed by "->" or "=", check below
	    }

	    اگر (vim9 && *p == '<')
	    {
		// generic function type args
		اگر (skip_generic_func_type_args(&p) == FAIL)
		{
		    eap->cmdidx = CMD_SIZE;
		    بازگشت p;
		}
	    }

	    swp = skipwhite(p);

	    اگر (
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
		اگر (*eap->cmd == '{' && ends_excmd(*skipwhite(eap->cmd + 1)))
		{
		    // "{" by itself is the start of a block.
		    eap->cmdidx = CMD_block;
		    بازگشت eap->cmd + 1;
		}
		eap->cmdidx = CMD_eval;
		بازگشت eap->cmd;
	    }

	    اگر ((p != eap->cmd && (
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
		اگر (skip_عبارت(&after, پوچ) == OK)
		{
		    after = skipwhite(after);
		    اگر (*after == '=' || (*after != NUL && after[1] == '=')
					 || (after[0] == '.' && after[1] == '.'
							   && after[2] == '='))
			eap->cmdidx = CMD_var;
		}
		--emsg_off;
		بازگشت eap->cmd;
	    }

	    // "[...]->Method()" is a list expression, but "[a, b] = Func()" is
	    // an assignment.
	    // If there is no line break inside the "[...]" then "p" is
	    // advanced to after the "]" by to_name_const_end(): check if a "="
	    // follows.
	    // If "[...]" has a line break "p" still points at the "[" and it
	    // can't be an assignment.
	    اگر (*eap->cmd == '[')
	    {
		char_u	    *eq;

		p = to_name_const_end(eap->cmd);
		اگر (p == eap->cmd && *p == '[')
		{
		    صحیح شمار = 0;
		    صحیح	semicolon = FALSE;

		    p = skip_var_list(eap->cmd, TRUE, &شمار, &semicolon, TRUE);
		}
		eq = p;
		اگر (eq != پوچ)
		{
		    eq = skipwhite(eq);
		    اگر (vim_strchr((char_u *)"+-*/%.", *eq) != پوچ)
		    {
			اگر (eq[0] == '.' && eq[1] == '.')
			    ++eq;
			++eq;
		    }
		}
		اگر (p == پوچ || p == eap->cmd || *eq != '=')
		{
		    eap->cmdidx = CMD_eval;
		    بازگشت eap->cmd;
		}
		اگر (p > eap->cmd && *eq == '=')
		{
		    eap->cmdidx = CMD_var;
		    بازگشت eap->cmd;
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
	    اگر (oplen > 0)
	    {
		اگر (((p - eap->cmd) > 2 && eap->cmd[1] == ':')
			|| *eap->cmd == '&'
			|| *eap->cmd == '$'
			|| *eap->cmd == '@'
			|| (eap->skip && IS_WHITE_OR_NUL(
						      *(skipwhite(p) + oplen)))
			|| lookup(eap->cmd, p - eap->cmd, TRUE, cctx) == OK)
		{
		    eap->cmdidx = CMD_var;
		    بازگشت eap->cmd;
		}
	    }

	    // Recognize trying to use a type for a w:, b:, t: or g: variable:
	    // "w:varname: number = 123".
	    اگر (eap->cmd[1] == ':' && *p == ':')
	    {
		eap->cmdidx = CMD_var;
		بازگشت eap->cmd;
	    }
	}

	// 1234->func() is a method call
	اگر (عدد_method(eap->cmd))
	{
	    eap->cmdidx = CMD_eval;
	    بازگشت eap->cmd;
	}

	// "g:", "s:" and "l:" are always assumed to be a variable, thus start
	// an expression.  A global/substitute/list command needs to use a
	// longer name.
	اگر (vim_strchr((char_u *)"gsl", *p) != پوچ && p[1] == ':')
	{
	    eap->cmdidx = CMD_eval;
	    بازگشت eap->cmd;
	}

	// If it is an ID it might be a variable with an operator on the next
	// line, if the variable exists it can't be an Ex command.
	اگر (p > eap->cmd && ends_excmd(*skipwhite(p))
		&& (lookup(eap->cmd, p - eap->cmd, TRUE, cctx) == OK
		    || (ASCII_ISALPHA(eap->cmd[0]) && eap->cmd[1] == ':')))
	{
	    eap->cmdidx = CMD_eval;
	    بازگشت eap->cmd;
	}

	// Check for "++nr" and "--nr".
	اگر (p == eap->cmd && p[0] != NUL && p[0] == p[1]
						   && (*p == '+' || *p == '-'))
	{
	    eap->cmdidx = *p == '+' ? CMD_increment : CMD_decrement;
	    بازگشت eap->cmd + 2;
	}
    }
#endif

    /*
     * Isolate the command and search for it in the command table.
     */
    p = eap->cmd;
    اگر (one_letter_cmd(p, &eap->cmdidx))
    {
	++p;
	اگر (full != پوچ)
	    *full = TRUE;
    }
    وگرنه
    {
	وقتی (ASCII_ISALPHA(*p))
	    ++p;
	// for python 3.x support ":py3", ":python3", ":py3file", etc.
	اگر (eap->cmd[0] == 'p' && eap->cmd[1] == 'y')
	{
	    وقتی (ASCII_ISALNUM(*p))
		++p;
	}
	وگرنه اگر (*p == '9' && STRNCMP("vim9", eap->cmd, 4) == 0)
	{
	    // include "9" for "vim9*" commands; "vim9cmd" and "vim9script".
	    ++p;
	    وقتی (ASCII_ISALPHA(*p))
		++p;
	}

	// check for non-alpha command
	اگر (p == eap->cmd && vim_strchr((char_u *)"@*!=><&~#}", *p) != پوچ)
	    ++p;
	len = (صحیح)(p - eap->cmd);
	// The "d" command can directly be followed by 'l' or 'p' flag, when
	// not in Vim9 script.
	اگر (!vim9 && *eap->cmd == 'd' && (p[-1] == 'l' || p[-1] == 'p'))
	{
	    // Check for ":dl", ":dell", etc. to ":deletel": that's
	    // :delete with the 'l' flag.  Same for 'p'.
	    برای (i = 0; i < len; ++i)
		اگر (eap->cmd[i] != ((char_u *)"delete")[i])
		    بشکن;
	    اگر (i == len - 1)
	    {
		--len;
		اگر (p[-1] == 'l')
		    eap->flags |= EXFLAG_LIST;
		وگرنه
		    eap->flags |= EXFLAG_PRINT;
	    }
	}

	اگر (ASCII_ISLOWER(eap->cmd[0]))
	{
	    صحیح c1 = eap->cmd[0];
	    صحیح c2 = len == 1 ? NUL : eap->cmd[1];

	    اگر (command_شمار != (صحیح)CMD_SIZE)
	    {
		iemsg(e_command_table_needs_to_be_updated_run_make_cmdidxs);
		getout(1);
	    }

	    // Use a precomputed index for fast look-up in cmdnames[]
	    // taking into account the first 2 letters of eap->cmd.
	    eap->cmdidx = cmdidxs1[Char_Ord_کم(c1)];
	    اگر (ASCII_ISLOWER(c2))
		eap->cmdidx += cmdidxs2[Char_Ord_کم(c1)][Char_Ord_کم(c2)];
	}
	وگرنه اگر (ASCII_ISUPPER(eap->cmd[0]))
	    eap->cmdidx = CMD_Next;
	وگرنه
	    eap->cmdidx = CMD_bang;

	برای ( ; (صحیح)eap->cmdidx < (صحیح)CMD_SIZE;
			       eap->cmdidx = (cmdidx_T)((صحیح)eap->cmdidx + 1))
	    اگر (STRNCMP(cmdnames[(صحیح)eap->cmdidx].cmd_name, (نویسه *)eap->cmd,
							    (size_t)len) == 0)
	    {
#ifdef FEAT_EVAL
		اگر (full != پوچ && cmdnames[eap->cmdidx].cmd_name[len] == NUL)
		    *full = TRUE;
#endif
		بشکن;
	    }

	// :Print and :mode are not supported in Vim9 script.
	// Some commands cannot be shortened in Vim9 script.
	اگر (vim9 && eap->cmdidx != CMD_SIZE)
	{
	    اگر (eap->cmdidx == CMD_mode || eap->cmdidx == CMD_Print)
		eap->cmdidx = CMD_SIZE;
	    وگرنه اگر ((cmdnames[eap->cmdidx].cmd_argt & EX_WHOLE)
			  && len < (صحیح)cmdnames[eap->cmdidx].cmd_namelen)
	    {
		semsg(_(e_command_cannot_be_shortened_str), eap->cmd);
		eap->cmdidx = CMD_SIZE;
	    }
	}

	// Do not recognize ":*" as the star command unless '*' is in
	// 'cpoptions'.
	اگر (eap->cmdidx == CMD_star && vim_strchr(p_cpo, CPO_STAR) == پوچ)
	    p = eap->cmd;

	// Look for a user defined command as a last resort.  Let ":Print" be
	// overruled by a user defined command.
	اگر ((eap->cmdidx == CMD_SIZE || eap->cmdidx == CMD_Print)
		&& *eap->cmd >= 'A' && *eap->cmd <= 'Z')
	{
	    // User defined commands may contain digits.
	    وقتی (ASCII_ISALNUM(*p))
		++p;
	    p = find_ucmd(eap, p, full, پوچ, پوچ);
	}
	اگر (p == پوچ || p == eap->cmd)
	    eap->cmdidx = CMD_SIZE;
    }

    // ":fina" means ":finally" in legacy script, for backwards compatibility.
    اگر (eap->cmdidx == CMD_final && p - eap->cmd == 4 && !vim9)
	eap->cmdidx = CMD_finally;

     // Force ":ho" to be unresolved.  Without this, find_ex_command()
     // matches it to CMD_horizontal (the only "ho*" entry), which makes
     // fullcommand("ho") return "horizontal" even though ":ho" cannot be
     // used as the modifier (cmdmods[] requires 3 chars, "hor").
    اگر (eap->cmdidx == CMD_horizontal && p - eap->cmd == 2)
	eap->cmdidx = CMD_SIZE;

#ifdef FEAT_EVAL
    اگر (eap->cmdidx < CMD_SIZE
	    && vim9
	    && !IS_WHITE_NL_OR_NUL(*p) && *p != '!' && *p != '|'
	    && (eap->cmdidx < 0 ||
		(cmdnames[eap->cmdidx].cmd_argt & EX_NONWHITE_OK) == 0))
    {
	// A name that goes on with an underscore is not this command with an
	// argument: "ch_log" is not ":change".  Leave it to be reported as an
	// invalid command.
	اگر (*p != '_')
	{
	    char_u *cmd = vim_strnsave(eap->cmd, p - eap->cmd);

	    semsg(_(e_command_str_not_followed_by_white_space_str), cmd,
								    eap->cmd);
	    vim_free(cmd);
	}
	eap->cmdidx = CMD_SIZE;
    }
#endif

    بازگشت p;
}

#اگر defined(FEAT_EVAL)
تعریفنوع ساختار
{
    نویسه	*name;
    صحیح		minlen;
    صحیح		has_شمار;  // :123verbose  :3tab
} cmdmod_info_T;

ایستا cmdmod_info_T cmdmod_info_tab[] = {
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
    صحیح
modifier_len(char_u *cmd)
{
    صحیح		i, j;
    char_u	*p = cmd;

    اگر (VIM_ISDIGIT(*cmd))
	p = skipwhite(skipdigits(cmd + 1));
    برای (i = 0; i < (صحیح)ARRAY_LENGTH(cmdmod_info_tab); ++i)
    {
	// cmdmod_info_tab[] is sorted by name: once the first letter is past
	// the command's first letter no later entry can match.
	اگر (cmdmod_info_tab[i].name[0] > *p)
	    بشکن;
	برای (j = 0; p[j] != NUL; ++j)
	    اگر (p[j] != cmdmod_info_tab[i].name[j])
		بشکن;
	اگر (!ASCII_ISALPHA(p[j]) && j >= cmdmod_info_tab[i].minlen
					&& (p == cmd || cmdmod_info_tab[i].has_شمار))
	    بازگشت j + (صحیح)(p - cmd);
    }
    بازگشت 0;
}

/*
 * Return > 0 if an Ex command "name" exists.
 * Return 2 if there is an exact match.
 * Return 3 if there is an ambiguous match.
 */
    صحیح
cmd_exists(char_u *name)
{
    exarg_T	ea;
    صحیح		full = FALSE;
    صحیح		i;
    صحیح		j;
    char_u	*p;

    // Check command modifiers.
    برای (i = 0; i < (صحیح)ARRAY_LENGTH(cmdmod_info_tab); ++i)
    {
	برای (j = 0; name[j] != NUL; ++j)
	    اگر (name[j] != cmdmod_info_tab[i].name[j])
		بشکن;
	اگر (name[j] == NUL && j >= cmdmod_info_tab[i].minlen)
	    بازگشت (cmdmod_info_tab[i].name[j] == NUL ? 2 : 1);
    }

    // Check built-in commands and user defined commands.
    // For ":2match" and ":3match" we need to skip the number.
    ea.cmd = (*name == '2' || *name == '3') ? name + 1 : name;
    ea.cmdidx = (cmdidx_T)0;
    ea.flags = 0;
    p = find_ex_command(&ea, &full, پوچ, پوچ);
    اگر (p == پوچ)
	بازگشت 3;
    اگر (vim_isdigit(*name) && ea.cmdidx != CMD_match)
	بازگشت 0;
    اگر (*skipwhite(p) != NUL)
	بازگشت 0;	// trailing garbage
    بازگشت (ea.cmdidx == CMD_SIZE ? 0 : (full ? 2 : 1));
}

/*
 * Add the attributes of an Ex command to "d" in the terms of ":command":
 * "argt" are the EX_ flags, "addr_type" the address type and "def" the
 * default for a range or count, -1 when there is none.
 */
    تهی
ex_command_attrs(dict_T *d, long_u argt, cmd_addr_T addr_type, long def)
{
    ایستا نویسه *addr_names[] = {
	"lines", "windows", "arguments", "loaded_buffers", "buffers", "tabs",
	"tabs_relative", "quickfix_valid", "quickfix", "unsigned", "other",
	"none"
    };
    نویسه	*nargs;

    اگر (!(argt & EX_EXTRA))
	nargs = "0";
    وگرنه اگر (!(argt & EX_NOSPC))
	nargs = (argt & EX_NEEDARG) ? "+" : "*";
    وگرنه اگر (!(argt & EX_NEEDARG))
	nargs = "?";
    وگرنه
	nargs = (argt & EX_ARGSPACE) ? "_" : "1";
    dict_add_string(d, "nargs", (char_u *)nargs);

    اگر (!(argt & EX_RANGE))
	dict_add_bool(d, "range", FALSE);
    وگرنه اگر (argt & EX_DFLALL)
	dict_add_string(d, "range", (char_u *)"%");
    وگرنه اگر (def >= 0 && !(argt & EX_شمار))
	dict_add_عدد(d, "range", def);
    وگرنه
	dict_add_string(d, "range", (char_u *)".");
    اگر (!(argt & EX_شمار))
	dict_add_bool(d, "count", FALSE);
    وگرنه
	dict_add_عدد(d, "count", def < 0 ? 0 : def);
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
    صحیح
ex_command_info(char_u *name, صحیح vim9, dict_T *d)
{
    exarg_T	ea;
    char_u	*p;
    صحیح		i;
    صحیح		j;
    صحیح		save_cmod_flags = cmdmod.cmod_flags;
    صحیح		ret = OK;

    اگر (vim9 != -1)
    {
	cmdmod.cmod_flags &= ~(CMOD_VIM9CMD | CMOD_LEGACY);
	cmdmod.cmod_flags |= vim9 ? CMOD_VIM9CMD : CMOD_LEGACY;
    }

    // An abbreviation of a command modifier, like ":bo" for ":botright".
    برای (i = 0; i < (صحیح)ARRAY_LENGTH(cmdmod_info_tab); ++i)
    {
	برای (j = 0; name[j] != NUL; ++j)
	    اگر (name[j] != cmdmod_info_tab[i].name[j])
		بشکن;
	اگر (name[j] == NUL && j >= cmdmod_info_tab[i].minlen)
	{
	    name = (char_u *)cmdmod_info_tab[i].name;
	    بشکن;
	}
    }

    // For ":2match" and ":3match" we need to skip the number.
    ea.cmd = (*name == '2' || *name == '3') ? name + 1 : name;
    ea.cmdidx = (cmdidx_T)0;
    ea.flags = 0;
    ea.addr_شمار = 0;
    // Don't complain about using "en" or ":let" in Vim9 script.
    ++emsg_silent;
    p = find_ex_command(&ea, پوچ, پوچ, پوچ);
    اگر (p == پوچ || ea.cmdidx == CMD_SIZE
	    || (vim_isdigit(*name) && ea.cmdidx != CMD_match)
	    || *skipwhite(p) != NUL
	    || not_in_vim9(&ea, true) == FAIL)
	ret = FAIL;
    --emsg_silent;

    اگر (ret == FAIL)
	;
    وگرنه اگر (IS_USER_CMDIDX(ea.cmdidx))
	ret = user_command_info(ea.useridx, ea.cmdidx, d);
    وگرنه
    {
	صحیح	notimpl = cmdnames[ea.cmdidx].cmd_func == ex_ni
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
    بازگشت ret;
}

/*
 * "fullcommand" function
 */
    تهی
f_fullcommand(typval_T *argvars, typval_T *rettv)
{
    exarg_T	ea;
    char_u	*name;
    char_u	*p;
    صحیح		vim9script = in_vim9script();
    صحیح		save_cmod_flags = cmdmod.cmod_flags;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = پوچ;

    اگر (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL))
	بازگشت;

    name = tv_get_string(&argvars[0]);
    اگر (name == پوچ)
	بازگشت;

    اگر (argvars[1].v_type != VAR_UNKNOWN)
    {
	vim9script = tv_get_bool(&argvars[1]);
	cmdmod.cmod_flags &= ~(CMOD_VIM9CMD | CMOD_LEGACY);
	cmdmod.cmod_flags |= vim9script ? CMOD_VIM9CMD : CMOD_LEGACY;
    }

    وقتی (*name == ':')
	name++;
    name = skip_range(name, TRUE, پوچ);

    ea.cmd = (*name == '2' || *name == '3') ? name + 1 : name;
    ea.cmdidx = (cmdidx_T)0;
    ea.addr_شمار = 0;
    ++emsg_silent;  // don't complain about using "en" in Vim9 script
    p = find_ex_command(&ea, پوچ, پوچ, پوچ);
    --emsg_silent;
    اگر (p == پوچ || ea.cmdidx == CMD_SIZE)
	برو theend;

    اگر (vim9script && not_in_vim9(&ea, true) == FAIL)
	برو theend;

    rettv->vval.v_string = vim_strsave(IS_USER_CMDIDX(ea.cmdidx)
				 ? get_user_command_name(ea.useridx, ea.cmdidx)
				 : cmdnames[ea.cmdidx].cmd_name);
theend:
    cmdmod.cmod_flags = save_cmod_flags;
}
#endif

    cmdidx_T
excmd_get_cmdidx(char_u *cmd, صحیح len)
{
    cmdidx_T idx;

    اگر (!one_letter_cmd(cmd, &idx))
	برای (idx = (cmdidx_T)0; (صحیح)idx < (صحیح)CMD_SIZE;
		idx = (cmdidx_T)((صحیح)idx + 1))
	    اگر (STRNCMP(cmdnames[(صحیح)idx].cmd_name, cmd, (size_t)len) == 0)
		بشکن;

    بازگشت idx;
}

    long
excmd_get_argt(cmdidx_T idx)
{
    بازگشت (long)cmdnames[(صحیح)idx].cmd_argt;
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
    صحیح		skip_star,	// skip "*" used for Visual range
    صحیح		*ctx)		// pointer to xp_context or NULL
{
    char_u	*cmd = cmd_start;
    unsigned	delim;

    وقتی (vim_strchr((char_u *)" \t0123456789.$%'/?-+,;\\", *cmd) != پوچ)
    {
	اگر (*cmd == '\\')
	{
	    اگر (cmd[1] == '?' || cmd[1] == '/' || cmd[1] == '&')
		++cmd;
	    وگرنه
		بشکن;
	}
	وگرنه اگر (*cmd == '\'')
	{
	    char_u *p = cmd;

	    // a quote is only valid at the start or after a separator
	    وقتی (p > cmd_start)
	    {
		--p;
		اگر (!VIM_ISWHITE(*p))
		    بشکن;
	    }
	    اگر (cmd > cmd_start && !VIM_ISWHITE(*p) && *p != ',' && *p != ';')
		بشکن;
	    اگر (*++cmd == NUL && ctx != پوچ)
		*ctx = EXPAND_NOTHING;
	}
	وگرنه اگر (*cmd == '/' || *cmd == '?')
	{
	    delim = *cmd++;
	    وقتی (*cmd != NUL && *cmd != delim)
		اگر (*cmd++ == '\\' && *cmd != NUL)
		    ++cmd;
	    اگر (*cmd == NUL && ctx != پوچ)
		*ctx = EXPAND_NOTHING;
	}
	اگر (*cmd != NUL)
	    ++cmd;
    }

    // Skip ":" and white space.
    وقتی (*cmd == ':')
	cmd = skipwhite(cmd + 1);

    // Skip "*" used for Visual range.
    اگر (skip_star && *cmd == '*' && vim_strchr(p_cpo, CPO_STAR) == پوچ)
	cmd = skipwhite(cmd + 1);

    بازگشت cmd;
}

    ایستا تهی
addr_error(cmd_addr_T addr_type)
{
    اگر (addr_type == ADDR_NONE)
	emsg(_(e_no_range_allowed));
    وگرنه
	emsg(_(e_invalid_range));
}

/*
 * Return the default address for an address type.
 */
    ایستا linenr_T
default_address(exarg_T *eap)
{
    linenr_T lnum = 0;

    انتخاب (eap->addr_type)
    {
	حالت ADDR_سطرها:
	حالت ADDR_OTHER:
	    // Default is the cursor line number.  Avoid using an invalid
	    // line number though.
	    اگر (curwin->w_cursor.lnum > curbuf->b_ml.ml_سطر_شمار)
		lnum = curbuf->b_ml.ml_سطر_شمار;
	    وگرنه
		lnum = curwin->w_cursor.lnum;
	    بشکن;
	حالت ADDR_WINDOWS:
	    lnum = CURRENT_WIN_NR;
	    بشکن;
	حالت ADDR_ARGUMENTS:
	    lnum = curwin->w_arg_idx + 1;
	    اگر (lnum > ARGCOUNT)
		lnum = ARGCOUNT;
	    بشکن;
	حالت ADDR_LOADED_BUFFERS:
	حالت ADDR_BUFFERS:
	    lnum = curbuf->b_fnum;
	    بشکن;
	حالت ADDR_TABS:
	    lnum = CURRENT_TAB_NR;
	    بشکن;
	حالت ADDR_TABS_RELATIVE:
	حالت ADDR_UNSIGNED:
	    lnum = 1;
	    بشکن;
	حالت ADDR_QUICKFIX:
#ifdef FEAT_QUICKFIX
	    lnum = qf_get_cur_idx(eap);
#endif
	    بشکن;
	حالت ADDR_QUICKFIX_VALID:
#ifdef FEAT_QUICKFIX
	    lnum = qf_get_cur_valid_idx(eap);
#endif
	    بشکن;
	حالت ADDR_NONE:
	    // Will give an error later if a range is found.
	    بشکن;
    }
    بازگشت lnum;
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
    صحیح		skip,		// only skip the address, don't use it
    صحیح		silent,		// no errors or side effects
    صحیح		to_other_file,  // flag: may jump to other file
    صحیح		address_شمار UNUSED) // 1 for first address, >1 after comma
{
    صحیح		c;
    صحیح		i;
    long	n;
    char_u	*cmd;
    pos_T	pos;
    pos_T	*fp;
    linenr_T	lnum;
    buf_T	*buf;

    cmd = skipwhite(*ptr);
    lnum = MAXLNUM;
    انجامبده
    {
	انتخاب (*cmd)
	{
	    حالت '.':			    // '.' - Cursor position
		++cmd;
		انتخاب (addr_type)
		{
		    حالت ADDR_سطرها:
		    حالت ADDR_OTHER:
			lnum = curwin->w_cursor.lnum;
			بشکن;
		    حالت ADDR_WINDOWS:
			lnum = CURRENT_WIN_NR;
			بشکن;
		    حالت ADDR_ARGUMENTS:
			lnum = curwin->w_arg_idx + 1;
			بشکن;
		    حالت ADDR_LOADED_BUFFERS:
		    حالت ADDR_BUFFERS:
			lnum = curbuf->b_fnum;
			بشکن;
		    حالت ADDR_TABS:
			lnum = CURRENT_TAB_NR;
			بشکن;
		    حالت ADDR_NONE:
		    حالت ADDR_TABS_RELATIVE:
		    حالت ADDR_UNSIGNED:
			addr_error(addr_type);
			cmd = پوچ;
			برو error;
			بشکن;
		    حالت ADDR_QUICKFIX:
#ifdef FEAT_QUICKFIX
			lnum = qf_get_cur_idx(eap);
#endif
			بشکن;
		    حالت ADDR_QUICKFIX_VALID:
#ifdef FEAT_QUICKFIX
			lnum = qf_get_cur_valid_idx(eap);
#endif
			بشکن;
		}
		بشکن;

	    حالت '$':			    // '$' - last line
		++cmd;
		انتخاب (addr_type)
		{
		    حالت ADDR_سطرها:
		    حالت ADDR_OTHER:
			lnum = curbuf->b_ml.ml_سطر_شمار;
			بشکن;
		    حالت ADDR_WINDOWS:
			lnum = LAST_WIN_NR;
			بشکن;
		    حالت ADDR_ARGUMENTS:
			lnum = ARGCOUNT;
			بشکن;
		    حالت ADDR_LOADED_BUFFERS:
			buf = lastbuf;
			وقتی (buf->b_ml.ml_mfp == پوچ)
			{
			    اگر (buf->b_prev == پوچ)
				بشکن;
			    buf = buf->b_prev;
			}
			lnum = buf->b_fnum;
			بشکن;
		    حالت ADDR_BUFFERS:
			lnum = lastbuf->b_fnum;
			بشکن;
		    حالت ADDR_TABS:
			lnum = LAST_TAB_NR;
			بشکن;
		    حالت ADDR_NONE:
		    حالت ADDR_TABS_RELATIVE:
		    حالت ADDR_UNSIGNED:
			addr_error(addr_type);
			cmd = پوچ;
			برو error;
			بشکن;
		    حالت ADDR_QUICKFIX:
#ifdef FEAT_QUICKFIX
			lnum = qf_get_size(eap);
			اگر (lnum == 0)
			    lnum = 1;
#endif
			بشکن;
		    حالت ADDR_QUICKFIX_VALID:
#ifdef FEAT_QUICKFIX
			lnum = qf_get_valid_size(eap);
			اگر (lnum == 0)
			    lnum = 1;
#endif
			بشکن;
		}
		بشکن;

	    حالت '\'':			    // ''' - mark
		اگر (*++cmd == NUL)
		{
		    cmd = پوچ;
		    برو error;
		}
		اگر (addr_type != ADDR_سطرها)
		{
		    addr_error(addr_type);
		    cmd = پوچ;
		    برو error;
		}
		اگر (skip)
		    ++cmd;
		وگرنه
		{
		    // Only accept a mark in another file when it is
		    // used by itself: ":'M".
		    fp = getmark(*cmd, to_other_file && cmd[1] == NUL);
		    ++cmd;
		    اگر (fp == (pos_T *)-1)
			// Jumped to another file.
			lnum = curwin->w_cursor.lnum;
		    وگرنه
		    {
			اگر (check_mark(fp) == FAIL)
			{
			    cmd = پوچ;
			    برو error;
			}
			lnum = fp->lnum;
		    }
		}
		بشکن;

	    حالت '/':
	    حالت '?':			// '/' or '?' - search
		c = *cmd++;
		اگر (addr_type != ADDR_سطرها)
		{
		    addr_error(addr_type);
		    cmd = پوچ;
		    برو error;
		}
		اگر (skip)	// skip "/pat/"
		{
		    cmd = skip_regexp(cmd, c, magic_isset());
		    اگر (*cmd == c)
			++cmd;
		}
		وگرنه
		{
		    صحیح flags;

		    pos = curwin->w_cursor; // save curwin->w_cursor

		    // When '/' or '?' follows another address, start from
		    // there.
		    اگر (lnum > 0 && lnum != MAXLNUM)
			curwin->w_cursor.lnum =
				lnum > curbuf->b_ml.ml_سطر_شمار
					   ? curbuf->b_ml.ml_سطر_شمار : lnum;

		    // Start a forward search at the end of the line (unless
		    // before the first line).
		    // Start a backward search at the start of the line.
		    // This makes sure we never match in the current
		    // line, and can match anywhere in the
		    // next/previous line.
		    اگر (c == '/' && curwin->w_cursor.lnum > 0)
			curwin->w_cursor.col = MAXCOL;
		    وگرنه
			curwin->w_cursor.col = 0;
		    searchcmdlen = 0;
		    flags = silent ? جستجو_KEEP : جستجو_HIS | جستجو_پیام;
		    اگر (!do_جستجو(پوچ, c, c, cmd, STRLEN(cmd), 1L, flags, پوچ))
		    {
			curwin->w_cursor = pos;
			cmd = پوچ;
			برو error;
		    }
		    lnum = curwin->w_cursor.lnum;
		    curwin->w_cursor = pos;
		    // adjust command string pointer
		    cmd += searchcmdlen;
		}
		بشکن;

	    حالت '\\':		    // "\?", "\/" or "\&", repeat search
		++cmd;
		اگر (addr_type != ADDR_سطرها)
		{
		    addr_error(addr_type);
		    cmd = پوچ;
		    برو error;
		}
		اگر (*cmd == '&')
		    i = RE_SUBST;
		وگرنه اگر (*cmd == '?' || *cmd == '/')
		    i = RE_جستجو;
		وگرنه
		{
		    emsg(_(e_backslash_should_be_followed_by));
		    cmd = پوچ;
		    برو error;
		}

		اگر (!skip)
		{
		    /*
		     * When search follows another address, start from
		     * there.
		     */
		    اگر (lnum != MAXLNUM)
			pos.lnum = lnum;
		    وگرنه
			pos.lnum = curwin->w_cursor.lnum;

		    /*
		     * Start the search just like for the above
		     * do_search().
		     */
		    اگر (*cmd != '?')
			pos.col = MAXCOL;
		    وگرنه
			pos.col = 0;
		    pos.coladd = 0;
		    اگر (searchit(curwin, curbuf, &pos, پوچ,
				*cmd == '?' ? BACKWARD : FORWARD,
				(char_u *)"", 0, 1L, جستجو_پیام, i, پوچ) != FAIL)
			lnum = pos.lnum;
		    وگرنه
		    {
			cmd = پوچ;
			برو error;
		    }
		}
		++cmd;
		بشکن;

	    پیشفرض:
		اگر (VIM_ISDIGIT(*cmd))	// absolute line number
		    lnum = getdigits(&cmd);
	}

	برای (;;)
	{
	    cmd = skipwhite(cmd);
	    اگر (*cmd != '-' && *cmd != '+' && !VIM_ISDIGIT(*cmd))
		بشکن;

	    اگر (lnum == MAXLNUM)
	    {
		انتخاب (addr_type)
		{
		    حالت ADDR_سطرها:
		    حالت ADDR_OTHER:
			// "+1" is same as ".+1"
			lnum = curwin->w_cursor.lnum;
			بشکن;
		    حالت ADDR_WINDOWS:
			lnum = CURRENT_WIN_NR;
			بشکن;
		    حالت ADDR_ARGUMENTS:
			lnum = curwin->w_arg_idx + 1;
			بشکن;
		    حالت ADDR_LOADED_BUFFERS:
		    حالت ADDR_BUFFERS:
			lnum = curbuf->b_fnum;
			بشکن;
		    حالت ADDR_TABS:
			lnum = CURRENT_TAB_NR;
			بشکن;
		    حالت ADDR_TABS_RELATIVE:
			lnum = 1;
			بشکن;
		    حالت ADDR_QUICKFIX:
#ifdef FEAT_QUICKFIX
			lnum = qf_get_cur_idx(eap);
#endif
			بشکن;
		    حالت ADDR_QUICKFIX_VALID:
#ifdef FEAT_QUICKFIX
			lnum = qf_get_cur_valid_idx(eap);
#endif
			بشکن;
		    حالت ADDR_NONE:
		    حالت ADDR_UNSIGNED:
			lnum = 0;
			بشکن;
		}
	    }

	    اگر (VIM_ISDIGIT(*cmd))
		i = '+';		// "number" is same as "+number"
	    وگرنه
		i = *cmd++;
	    اگر (!VIM_ISDIGIT(*cmd))	// '+' is '+1'
		n = 1;
	    وگرنه
	    {
		// "number", "+number" or "-number"
		n = getdigits(&cmd);
		اگر (n == MAXLNUM)
		{
		    emsg(_(e_سطر_عدد_out_of_range));
		    cmd = پوچ;
		    برو error;
		}
	    }

	    اگر (addr_type == ADDR_TABS_RELATIVE)
	    {
		emsg(_(e_invalid_range));
		cmd = پوچ;
		برو error;
	    }
	    وگرنه اگر (addr_type == ADDR_LOADED_BUFFERS
		    || addr_type == ADDR_BUFFERS)
		lnum = compute_buffer_local_شمار(
				    addr_type, lnum, (i == '-') ? -1 * n : n);
	    وگرنه
	    {
#ifdef FEAT_FOLDING
		// Relative line addressing: need to adjust for lines in a
		// closed fold after the first address.
		اگر (addr_type == ADDR_سطرها && (i == '-' || i == '+')
							 && address_شمار >= 2)
		    (تهی)hasFolding(lnum, پوچ, &lnum);
#endif
		اگر (i == '-')
		    lnum -= n;
		وگرنه
		{
		    اگر (lnum >= 0 && n >= LONG_MAX - lnum)
		    {
			emsg(_(e_سطر_عدد_out_of_range));
			cmd = پوچ;
			برو error;
		    }
		    lnum += n;
		}
	    }
	}
    } وقتی (*cmd == '/' || *cmd == '?');

error:
    *ptr = cmd;
    بازگشت lnum;
}

/*
 * Set eap->line1 and eap->line2 to the whole range.
 * Used for commands with the EX_DFLALL flag and no range given.
 */
    ایستا تهی
address_default_all(exarg_T *eap)
{
    eap->line1 = 1;
    انتخاب (eap->addr_type)
    {
	حالت ADDR_سطرها:
	حالت ADDR_OTHER:
	    eap->line2 = curbuf->b_ml.ml_سطر_شمار;
	    بشکن;
	حالت ADDR_LOADED_BUFFERS:
	    {
		buf_T *buf = firstbuf;

		وقتی (buf->b_next != پوچ && buf->b_ml.ml_mfp == پوچ)
		    buf = buf->b_next;
		eap->line1 = buf->b_fnum;
		buf = lastbuf;
		وقتی (buf->b_prev != پوچ && buf->b_ml.ml_mfp == پوچ)
		    buf = buf->b_prev;
		eap->line2 = buf->b_fnum;
	    }
	    بشکن;
	حالت ADDR_BUFFERS:
	    eap->line1 = firstbuf->b_fnum;
	    eap->line2 = lastbuf->b_fnum;
	    بشکن;
	حالت ADDR_WINDOWS:
	    eap->line2 = LAST_WIN_NR;
	    بشکن;
	حالت ADDR_TABS:
	    eap->line2 = LAST_TAB_NR;
	    بشکن;
	حالت ADDR_TABS_RELATIVE:
	    eap->line2 = 1;
	    بشکن;
	حالت ADDR_ARGUMENTS:
	    اگر (ARGCOUNT == 0)
		eap->line1 = eap->line2 = 0;
	    وگرنه
		eap->line2 = ARGCOUNT;
	    بشکن;
	حالت ADDR_QUICKFIX_VALID:
#ifdef FEAT_QUICKFIX
	    eap->line2 = qf_get_valid_size(eap);
	    اگر (eap->line2 == 0)
		eap->line2 = 1;
#endif
	    بشکن;
	حالت ADDR_NONE:
	حالت ADDR_UNSIGNED:
	حالت ADDR_QUICKFIX:
	    iemsg("Cannot use EX_DFLALL with ADDR_NONE, ADDR_UNSIGNED or ADDR_QUICKFIX");
	    بشکن;
    }
}


/*
 * Get flags from an Ex command argument.
 */
    ایستا تهی
get_flags(exarg_T *eap)
{
    وقتی (vim_strchr((char_u *)"lp#", *eap->arg) != پوچ)
    {
	اگر (*eap->arg == 'l')
	    eap->flags |= EXFLAG_LIST;
	وگرنه اگر (*eap->arg == 'p')
	    eap->flags |= EXFLAG_PRINT;
	وگرنه
	    eap->flags |= EXFLAG_NR;
	eap->arg = skipwhite(eap->arg + 1);
    }
}

/*
 * Function called for command which is Not Implemented.  NI!
 */
    تهی
ex_ni(exarg_T *eap)
{
    اگر (!eap->skip)
	eap->errmsg =
		_(e_sorry_command_is_not_available_in_this_version);
}

#ifdef HAVE_EX_SCRIPT_NI
/*
 * Function called for script command which is Not Implemented.  NI!
 * Skips over ":perl <<EOF" constructs.
 */
    ایستا تهی
ex_script_ni(exarg_T *eap)
{
    اگر (!eap->skip)
	ex_ni(eap);
    وگرنه
	vim_free(script_get(eap, eap->arg));
}
#endif

/*
 * Check range in Ex command for validity.
 * Return NULL when valid, error message when invalid.
 */
    ایستا نویسه *
invalid_range(exarg_T *eap)
{
    buf_T	*buf;

    اگر (       eap->line1 < 0
	    || eap->line2 < 0
	    || eap->line1 > eap->line2)
	بازگشت _(e_invalid_range);

    اگر (eap->argt & EX_RANGE)
    {
	انتخاب (eap->addr_type)
	{
	    حالت ADDR_سطرها:
		اگر (eap->line2 > curbuf->b_ml.ml_سطر_شمار
#ifdef FEAT_DIFF
			    + (eap->cmdidx == CMD_diffget ||
				eap->cmdidx == CMD_diffput)
#endif
		   )
		    بازگشت _(e_invalid_range);
		بشکن;
	    حالت ADDR_ARGUMENTS:
		// add 1 if ARGCOUNT is 0
		اگر (eap->line2 > ARGCOUNT + (!ARGCOUNT))
		    بازگشت _(e_invalid_range);
		بشکن;
	    حالت ADDR_BUFFERS:
		// Only a boundary check, not whether the buffers actually
		// exist.
		اگر (eap->line1 < 1 || eap->line2 > get_highest_fnum())
		    بازگشت _(e_invalid_range);
		بشکن;
	    حالت ADDR_LOADED_BUFFERS:
		buf = firstbuf;
		وقتی (buf->b_ml.ml_mfp == پوچ)
		{
		    اگر (buf->b_next == پوچ)
			بازگشت _(e_invalid_range);
		    buf = buf->b_next;
		}
		اگر (eap->line1 < buf->b_fnum)
		    بازگشت _(e_invalid_range);
		buf = lastbuf;
		وقتی (buf->b_ml.ml_mfp == پوچ)
		{
		    اگر (buf->b_prev == پوچ)
			بازگشت _(e_invalid_range);
		    buf = buf->b_prev;
		}
		اگر (eap->line2 > buf->b_fnum)
		    بازگشت _(e_invalid_range);
		بشکن;
	    حالت ADDR_WINDOWS:
		اگر (eap->line2 > LAST_WIN_NR)
		    بازگشت _(e_invalid_range);
		بشکن;
	    حالت ADDR_TABS:
		اگر (eap->line2 > LAST_TAB_NR)
		    بازگشت _(e_invalid_range);
		بشکن;
	    حالت ADDR_TABS_RELATIVE:
	    حالت ADDR_OTHER:
		// Any range is OK.
		بشکن;
	    حالت ADDR_QUICKFIX:
#ifdef FEAT_QUICKFIX
		// No error for value that is too big, will use the last entry.
		اگر (eap->line2 <= 0)
		{
		    اگر (eap->addr_شمار == 0)
			بازگشت _(e_no_errors);
		    بازگشت _(e_invalid_range);
		}
#endif
		بشکن;
	    حالت ADDR_QUICKFIX_VALID:
#ifdef FEAT_QUICKFIX
		اگر ((eap->line2 != 1 && eap->line2 > qf_get_valid_size(eap))
			|| eap->line2 < 0)
		    بازگشت _(e_invalid_range);
#endif
		بشکن;
	    حالت ADDR_UNSIGNED:
	    حالت ADDR_NONE:
		// Will give an error elsewhere.
		بشکن;
	}
    }
    بازگشت پوچ;
}

/*
 * Correct the range for zero line number, if required.
 */
    ایستا تهی
correct_range(exarg_T *eap)
{
    اگر (!(eap->argt & EX_ZEROR))	    // zero in range not allowed
    {
	اگر (eap->line1 == 0)
	    eap->line1 = 1;
	اگر (eap->line2 == 0)
	    eap->line2 = 1;
    }
}

#ifdef FEAT_QUICKFIX
/*
 * For a ":vimgrep" or ":vimgrepadd" command return a pointer past the
 * pattern.  Otherwise return eap->arg.
 */
    ایستا char_u *
skip_grep_pat(exarg_T *eap)
{
    char_u	*p = eap->arg;

    اگر (*p != NUL && (eap->cmdidx == CMD_vimgrep || eap->cmdidx == CMD_lvimgrep
		|| eap->cmdidx == CMD_vimgrepadd
		|| eap->cmdidx == CMD_lvimgrepadd
		|| grep_internal(eap->cmdidx)))
    {
	p = skip_vimgrep_pat(p, پوچ, پوچ);
	اگر (p == پوچ)
	    p = eap->arg;
    }
    بازگشت p;
}

/*
 * For the ":make" and ":grep" commands insert the 'makeprg'/'grepprg' option
 * in the command line, so that things like % get expanded.
 */
    ایستا char_u *
replace_makeprg(exarg_T *eap, char_u *p, char_u **cmdlinep)
{
    char_u	*new_cmdline;
    char_u	*program;
    char_u	*pos;
    char_u	*ptr;
    صحیح		len;
    صحیح		i;

    /*
     * Don't do it when ":vimgrep" is used for ":grep".
     */
    اگر ((eap->cmdidx == CMD_make || eap->cmdidx == CMD_lmake
		     || eap->cmdidx == CMD_grep || eap->cmdidx == CMD_lgrep
		     || eap->cmdidx == CMD_grepadd
		     || eap->cmdidx == CMD_lgrepadd)
	    && !grep_internal(eap->cmdidx))
    {
	اگر (eap->cmdidx == CMD_grep || eap->cmdidx == CMD_lgrep
	    || eap->cmdidx == CMD_grepadd || eap->cmdidx == CMD_lgrepadd)
	{
	    اگر (*curbuf->b_p_gp == NUL)
		program = p_gp;
	    وگرنه
		program = curbuf->b_p_gp;
	}
	وگرنه
	{
	    اگر (*curbuf->b_p_mp == NUL)
		program = p_mp;
	    وگرنه
		program = curbuf->b_p_mp;
	}

	p = skipwhite(p);

	اگر ((pos = (char_u *)strstr((نویسه *)program, "$*")) != پوچ)
	{
	    // replace $* by given arguments
	    i = 1;
	    وقتی ((pos = (char_u *)strstr((نویسه *)pos + 2, "$*")) != پوچ)
		++i;
	    len = (صحیح)STRLEN(p);
	    new_cmdline = alloc(STRLEN(program) + (size_t)i * (len - 2) + 1);
	    اگر (new_cmdline == پوچ)
		بازگشت پوچ;			// out of memory
	    ptr = new_cmdline;
	    وقتی ((pos = (char_u *)strstr((نویسه *)program, "$*")) != پوچ)
	    {
		i = (صحیح)(pos - program);
		STRNCPY(ptr, program, i);
		STRCPY(ptr += i, p);
		ptr += len;
		program = pos + 2;
	    }
	    STRCPY(ptr, program);
	}
	وگرنه
	{
	    size_t program_len = STRLEN(program);

	    new_cmdline = alloc(program_len + STRLEN(p) + 2);
	    اگر (new_cmdline == پوچ)
		بازگشت پوچ;			// out of memory
	    STRCPY(new_cmdline, program);
	    STRCPY(new_cmdline + program_len, " ");
	    STRCPY(new_cmdline + program_len + 1, p);
	}
	پیام_make(p);

	// 'eap->cmd' is not set here, because it is not used at CMD_make
	vim_free(*cmdlinep);
	*cmdlinep = new_cmdline;
	p = new_cmdline;
    }
    بازگشت p;
}
#endif

/*
 * Expand file name in Ex command argument.
 * When an error is detected, "errormsgp" is set to a non-NULL pointer.
 * Return FAIL for failure, OK otherwise.
 */
    صحیح
expand_filename(
    exarg_T	*eap,
    char_u	**cmdlinep,
    نویسه	**errormsgp)
{
    صحیح		has_wildcards;	// need to expand wildcards
    char_u	*repl;
    size_t	srclen;
    char_u	*p;
    صحیح		n;
    صحیح		escaped;

#ifdef FEAT_QUICKFIX
    // Skip a regexp pattern for ":vimgrep[add] pat file..."
    p = skip_grep_pat(eap);
#وگرنه
    p = eap->arg;
#endif

    /*
     * Decide to expand wildcards *before* replacing '%', '#', etc.  If
     * the file name contains a wildcard it should not cause expanding.
     * (it will be expanded anyway if there is a wildcard before replacing).
     */
    has_wildcards = mch_has_wildcard(p);
    وقتی (*p != NUL)
    {
#ifdef FEAT_EVAL
	// Skip over `=expr`, wildcards in it are not expanded.
	اگر (p[0] == '`' && p[1] == '=')
	{
	    p += 2;
	    (تهی)skip_عبارت(&p, پوچ);
	    اگر (*p == '`')
		++p;
	    ادامه;
	}
#endif
	/*
	 * Quick check if this cannot be the start of a special string.
	 * Also removes backslash before '%', '#' and '<'.
	 */
	اگر (vim_strchr((char_u *)"%#<", *p) == پوچ)
	{
	    ++p;
	    ادامه;
	}

	/*
	 * Try to find a match at this position.
	 */
	repl = eval_vars(p, eap->arg, &srclen, &(eap->do_ecmd_lnum),
						    errormsgp, &escaped, TRUE);
	اگر (*errormsgp != پوچ)		// error detected
	    بازگشت FAIL;
	اگر (repl == پوچ)		// no match found
	{
	    p += srclen;
	    ادامه;
	}

	// Wildcards won't be expanded below, the replacement is taken
	// literally.  But do expand "~/file", "~user/file" and "$HOME/file".
	اگر (vim_strchr(repl, '$') != پوچ || vim_strchr(repl, '~') != پوچ)
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
	اگر (!eap->usefilter
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
	    ایستا char_u *nobslash = (char_u *)" \t\"|";
# define ESCAPE_CHARS nobslash
#وگرنه
# define ESCAPE_CHARS escape_chars
#endif

	    برای (l = repl; *l; ++l)
		اگر (vim_strchr(ESCAPE_CHARS, *l) != پوچ)
		{
		    l = vim_strsave_escaped(repl, ESCAPE_CHARS);
		    اگر (l != پوچ)
		    {
			vim_free(repl);
			repl = l;
		    }
		    بشکن;
		}
	}

	// For a shell command a '!' must be escaped.
	اگر ((eap->usefilter || eap->cmdidx == CMD_bang
						|| eap->cmdidx == CMD_terminal)
			    && vim_strpbrk(repl, (char_u *)"!") != پوچ)
	{
	    char_u	*l;

	    l = vim_strsave_escaped(repl, (char_u *)"!");
	    اگر (l != پوچ)
	    {
		vim_free(repl);
		repl = l;
	    }
	}

	p = repl_cmdline(eap, p, srclen, repl, cmdlinep);
	vim_free(repl);
	اگر (p == پوچ)
	    بازگشت FAIL;
    }

    /*
     * One file argument: Expand wildcards.
     * Don't do this with ":r !command" or ":w !command".
     */
    اگر ((eap->argt & EX_NOSPC) && !eap->usefilter)
    {
	/*
	 * May do this twice:
	 * 1. Replace environment variables.
	 * 2. Replace any other wildcards, remove backslashes.
	 */
	برای (n = 1; n <= 2; ++n)
	{
	    اگر (n == 2)
	    {
		/*
		 * Halve the number of backslashes (this is Vi compatible).
		 * For Unix and OS/2, when wildcards are expanded, this is
		 * done by ExpandOne() below.
		 */
#اگر defined(UNIX)
		اگر (!has_wildcards)
#endif
		    backslash_halve(eap->arg);
	    }

	    اگر (has_wildcards)
	    {
		اگر (n == 1)
		{
		    /*
		     * First loop: May expand environment variables.  This
		     * can be done much faster with expand_env() than with
		     * something else (e.g., calling a shell).
		     * After expanding environment variables, check again
		     * if there are still wildcards present.
		     */
		    اگر (vim_strchr(eap->arg, '$') != پوچ
			    || vim_strchr(eap->arg, '~') != پوچ)
		    {
			expand_env_esc(eap->arg, NameBuff, MAXPATHL,
					(char_u *)(" \t" PATH_ESC_WILDCARDS),
					TRUE, پوچ);
			has_wildcards = mch_has_wildcard(NameBuff);
			p = NameBuff;
		    }
		    وگرنه
			p = پوچ;
		}
		وگرنه // n == 2
		{
		    expand_T	xpc;
		    صحیح		options = WILD_LIST_NOTFOUND
					       | WILD_NOERROR | WILD_ADD_SLASH;

		    ExpandInit(&xpc);
		    xpc.xp_context = EXPAND_FILES;
		    اگر (p_wic)
			options += WILD_ICASE;
		    p = ExpandOne(&xpc, eap->arg, پوچ,
						   options, WILD_EXPAND_FREE);
		    اگر (p == پوچ)
			بازگشت FAIL;
		}
		اگر (p != پوچ)
		{
		    (تهی)repl_cmdline(eap, eap->arg, STRLEN(eap->arg),
								 p, cmdlinep);
		    اگر (n == 2)	// p came from ExpandOne()
			vim_free(p);
		}
	    }
	}
    }
    بازگشت OK;
}

/*
 * Replace part of the command line, keeping eap->cmd, eap->arg and
 * eap->nextcmd correct.
 * "src" points to the part that is to be replaced, of length "srclen".
 * "repl" is the replacement string.
 * Returns a pointer to the character after the replaced string.
 * Returns NULL for failure.
 */
    ایستا char_u *
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
    اگر (eap->nextcmd != پوچ)
	i += STRLEN(eap->nextcmd);	// add space for next command
    اگر ((new_cmdline = alloc(i)) == پوچ)
	بازگشت پوچ;			// out of memory!

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

    اگر (eap->nextcmd != پوچ)		// append next command
    {
	new_cmdlinelen += taillen + 1;
	STRCPY(new_cmdline + new_cmdlinelen, eap->nextcmd);
	eap->nextcmd = new_cmdline + new_cmdlinelen;
    }
    eap->cmd = new_cmdline + (eap->cmd - *cmdlinep);
    eap->arg = new_cmdline + (eap->arg - *cmdlinep);
    اگر (eap->do_ecmd_cmd != پوچ && eap->do_ecmd_cmd != dollar_command)
	eap->do_ecmd_cmd = new_cmdline + (eap->do_ecmd_cmd - *cmdlinep);
    vim_free(*cmdlinep);
    *cmdlinep = new_cmdline;

    بازگشت src;
}

/*
 * Check for '|' to separate commands and '"' to start comments.
 * If "keep_backslash" is TRUE do not remove any backslash.
 */
    تهی
separate_nextcmd(exarg_T *eap, صحیح keep_backslash)
{
    صحیح		vim9script = in_vim9script();
    char_u	*p;

#ifdef FEAT_QUICKFIX
    p = skip_grep_pat(eap);
#وگرنه
    p = eap->arg;
#endif

    برای ( ; *p; MB_PTR_ADV(p))
    {
	اگر (*p == Ctrl_V)
	{
	    اگر ((eap->argt & (EX_CTRLV | EX_XFILE)) || keep_backslash)
		++p;		// skip CTRL-V and next char
	    وگرنه
				// remove CTRL-V and skip next char
		STRMOVE(p, p + 1);
	    اگر (*p == NUL)		// stop at NUL after CTRL-V
		بشکن;
	}

#ifdef FEAT_EVAL
	// Skip over `=expr` when wildcards are expanded.
	وگرنه اگر (p[0] == '`' && p[1] == '=' && (eap->argt & EX_XFILE))
	{
	    p += 2;
	    (تهی)skip_عبارت(&p, پوچ);
	    اگر (*p == NUL)		// stop at NUL after CTRL-V
		بشکن;
	}
#endif

	// Check for '"'/'#': start of comment or '|': next command
	// :@" and :*" do not start a comment!
	// :redir @" doesn't either.
	وگرنه اگر ((*p == '"'
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
		    && eap->cmdidx != CMD_درج)
		|| *p == '\n')
	{
	    /*
	     * We remove the '\' before the '|', unless EX_CTRLV is used
	     * AND 'b' is present in 'cpoptions'.
	     */
	    اگر ((vim_strchr(p_cpo, CPO_BAR) == پوچ
			      || !(eap->argt & EX_CTRLV)) && *(p - 1) == '\\')
	    {
		اگر (!keep_backslash)
		{
		    STRMOVE(p - 1, p);	// remove the '\'
		    --p;
		}
	    }
	    وگرنه
	    {
		set_nextcmd(eap, p);
		*p = NUL;
		بشکن;
	    }
	}
    }

    اگر (!(eap->argt & EX_NOTRLCOM))	// remove trailing spaces
	del_trailing_spaces(eap->arg);
}

/*
 * get + command from ex argument
 */
    ایستا char_u *
getargcmd(char_u **argp)
{
    char_u *arg = *argp;
    char_u *command = پوچ;

    اگر (*arg == '+')	    // +[command]
    {
	++arg;
	اگر (vim_isspace(*arg) || *arg == NUL)
	    command = dollar_command;
	وگرنه
	{
	    command = arg;
	    arg = skip_cmd_arg(command, TRUE);
	    اگر (*arg != NUL)
		*arg++ = NUL;		// terminate command with NUL
	}

	arg = skipwhite(arg);	// skip over spaces
	*argp = arg;
    }
    بازگشت command;
}

/*
 * Find end of "+command" argument.  Skip over "\ " and "\\".
 */
    char_u *
skip_cmd_arg(
    char_u *p,
    صحیح	   rembs)	// TRUE to halve the number of backslashes
{
    وقتی (*p && !vim_isspace(*p))
    {
	اگر (*p == '\\' && p[1] != NUL)
	{
	    اگر (rembs)
		STRMOVE(p, p + 1);
	    وگرنه
		++p;
	}
	MB_PTR_ADV(p);
    }
    بازگشت p;
}

    صحیح
get_bad_opt(char_u *p, exarg_T *eap)
{
    اگر (STRICMP(p, "keep") == 0)
	eap->bad_char = BAD_KEEP;
    وگرنه اگر (STRICMP(p, "drop") == 0)
	eap->bad_char = BAD_DROP;
    وگرنه اگر (MB_BYTE2LEN(*p) == 1 && p[1] == NUL)
	eap->bad_char = *p;
    وگرنه
	بازگشت FAIL;
    بازگشت OK;
}

/*
 * Function given to ExpandGeneric() to obtain the list of bad= names.
 */
    ایستا char_u *
get_bad_name(expand_T *xp UNUSED, صحیح idx)
{
    // Note: Keep this in sync with getargopt.
    ایستا نویسه *(p_bad_مقادیر[]) =
    {
	"?",
	"keep",
	"drop",
    };

    اگر (idx < 0 || idx >= (صحیح)ARRAY_LENGTH(p_bad_مقادیر))
	بازگشت پوچ;

    بازگشت (char_u*)p_bad_مقادیر[idx];
}

/*
 * Get "++opt=arg" argument.
 * Return FAIL or OK.
 */
    ایستا صحیح
getargopt(exarg_T *eap)
{
    char_u	*arg = eap->arg + 2;
    صحیح		*pp = پوچ;
    صحیح		bad_char_idx;
    char_u	*p;

    // Note: Keep this in sync with get_argopt_name.

    // ":edit ++[no]bin[ary] file"
    اگر (STRNCMP(arg, "bin", 3) == 0 || STRNCMP(arg, "nobin", 5) == 0)
    {
	اگر (*arg == 'n')
	{
	    arg += 2;
	    eap->force_bin = FORCE_NOBIN;
	}
	وگرنه
	    eap->force_bin = FORCE_BIN;
	اگر (!checkforcmd(&arg, "binary", 3))
	    بازگشت FAIL;
	eap->arg = skipwhite(arg);
	بازگشت OK;
    }

    // ":read ++edit file"
    اگر (STRNCMP(arg, "edit", 4) == 0)
    {
	eap->read_edit = TRUE;
	eap->arg = skipwhite(arg + 4);
	بازگشت OK;
    }

    اگر (STRNCMP(arg, "ff", 2) == 0)
    {
	arg += 2;
	pp = &eap->force_ff;
    }
    وگرنه اگر (STRNCMP(arg, "fileformat", 10) == 0)
    {
	arg += 10;
	pp = &eap->force_ff;
    }
    وگرنه اگر (STRNCMP(arg, "enc", 3) == 0)
    {
	اگر (STRNCMP(arg, "encoding", 8) == 0)
	    arg += 8;
	وگرنه
	    arg += 3;
	pp = &eap->force_enc;
    }
    وگرنه اگر (STRNCMP(arg, "bad", 3) == 0)
    {
	arg += 3;
	pp = &bad_char_idx;
    }

    اگر (pp == پوچ || *arg != '=')
	بازگشت FAIL;

    ++arg;
    *pp = (صحیح)(arg - eap->cmd);
    arg = skip_cmd_arg(arg, FALSE);
    eap->arg = skipwhite(arg);
    *arg = NUL;

    اگر (pp == &eap->force_ff)
    {
	اگر (check_ff_مقدار(eap->cmd + eap->force_ff) == FAIL)
	    بازگشت FAIL;
	eap->force_ff = eap->cmd[eap->force_ff];
    }
    وگرنه اگر (pp == &eap->force_enc)
    {
	// Make 'fileencoding' lower case.
	برای (p = eap->cmd + eap->force_enc; *p != NUL; ++p)
	    *p = TOLOWER_ASC(*p);
    }
    وگرنه
    {
	// Check ++bad= argument.  Must be a single-byte character, "keep" or
	// "drop".
	اگر (get_bad_opt(eap->cmd + bad_char_idx, eap) == FAIL)
	    بازگشت FAIL;
    }

    بازگشت OK;
}

/*
 * Function given to ExpandGeneric() to obtain the list of ++opt names.
 */
    ایستا char_u *
get_argopt_name(expand_T *xp UNUSED, صحیح idx)
{
    // Note: Keep this in sync with getargopt.
    ایستا نویسه *(p_opt_مقادیر[]) =
    {
	"fileformat=",
	"encoding=",
	"binary",
	"nobinary",
	"bad=",
	"edit",
    };

    اگر (idx < 0 || idx >= (صحیح)ARRAY_LENGTH(p_opt_مقادیر))
	بازگشت پوچ;

    بازگشت (char_u*)p_opt_مقادیر[idx];
}

/*
 * Command-line expansion for ++opt=name.
 */
    صحیح
expand_argopt(
	char_u	    *pat,
	expand_T    *xp,
	regmatch_T  *rmp,
	char_u	    ***matches,
	صحیح	    *رقم_Matches)
{
    اگر (xp->xp_pattern > xp->xp_سطر && *(xp->xp_pattern-1) == '=')
    {
	char_u *(*cb)(expand_T *, صحیح) = پوچ;

	char_u *name_end = xp->xp_pattern - 1;
	اگر (name_end - xp->xp_سطر >= 2
		&& STRNCMP(name_end - 2, "ff", 2) == 0)
	    cb = get_fileformat_name;
	وگرنه اگر (name_end - xp->xp_سطر >= 10
		&& STRNCMP(name_end - 10, "fileformat", 10) == 0)
	    cb = get_fileformat_name;
	وگرنه اگر (name_end - xp->xp_سطر >= 3
		&& STRNCMP(name_end - 3, "enc", 3) == 0)
	    cb = get_encoding_name;
	وگرنه اگر (name_end - xp->xp_سطر >= 8
		&& STRNCMP(name_end - 8, "encoding", 8) == 0)
	    cb = get_encoding_name;
	وگرنه اگر (name_end - xp->xp_سطر >= 3
		&& STRNCMP(name_end - 3, "bad", 3) == 0)
	    cb = get_bad_name;

	اگر (cb != پوچ)
	{
	    بازگشت ExpandGeneric(
		    pat,
		    xp,
		    rmp,
		    matches,
		    رقم_Matches,
		    cb,
		    FALSE);
	}
	بازگشت FAIL;
    }

    // Special handling of "ff" which acts as a short form of
    // "fileformat", as "ff" is not a substring of it.
    اگر (xp->xp_pattern_len == 2
	    && STRNCMP(xp->xp_pattern, "ff", xp->xp_pattern_len) == 0)
    {
	*matches = ALLOC_MULT(char_u *, 1);
	اگر (*matches == پوچ)
	    بازگشت FAIL;
	*رقم_Matches = 1;
	(*matches)[0] = vim_strsave((char_u*)"fileformat=");
	بازگشت OK;
    }

    بازگشت ExpandGeneric(
	    pat,
	    xp,
	    rmp,
	    matches,
	    رقم_Matches,
	    get_argopt_name,
	    FALSE);
}

    ایستا تهی
ex_autocmd(exarg_T *eap)
{
    اگر (eap->skip)
    {
	skip_cmd_block(eap);
	بازگشت;
    }

    /*
     * Disallow autocommands from .exrc and .vimrc in current
     * directory for security reasons.
     */
    اگر (secure)
    {
	secure = 2;
	eap->errmsg =
	      _(e_command_not_allowed_from_vimrc_in_current_dir_or_tag_جستجو);
    }
    وگرنه اگر (eap->cmdidx == CMD_autocmd)
	do_autocmd(eap, eap->arg, eap->forceit);
    وگرنه
	do_augroup(eap->arg, eap->forceit);
}

/*
 * ":doautocmd": Apply the automatic commands to the current buffer.
 */
    ایستا تهی
ex_doautocmd(exarg_T *eap)
{
    char_u	*arg = eap->arg;
    صحیح		call_do_modelines = check_nomodeline(&arg);
    صحیح		did_aucmd;

    (تهی)do_doautocmd(arg, TRUE, &did_aucmd);
    // Only when there is no <nomodeline>.
    اگر (call_do_modelines && did_aucmd)
	do_modelines(0);
}

/*
 * :[N]bunload[!] [N] [bufname] unload buffer
 * :[N]bdelete[!] [N] [bufname] delete buffer from buffer list
 * :[N]bwipeout[!] [N] [bufname] delete buffer really
 */
    ایستا تهی
ex_bunload(exarg_T *eap)
{
    اگر (ERROR_IF_ANY_POPUP_WINDOW)
	بازگشت;
    eap->errmsg = do_bufdel(
	    eap->cmdidx == CMD_bdelete ? DOBUF_DEL
		: eap->cmdidx == CMD_bwipeout ? DOBUF_WIPE
		: DOBUF_UNLOAD, eap->arg,
	    eap->addr_شمار, (صحیح)eap->line1, (صحیح)eap->line2, eap->forceit);
}

/*
 * :[N]buffer [N]	to buffer N
 * :[N]sbuffer [N]	to buffer N
 */
    ایستا تهی
ex_buffer(exarg_T *eap)
{
    اگر (ERROR_IF_ANY_POPUP_WINDOW)
	بازگشت;
    do_exbuffer(eap);
}

/*
 * ":buffer" command and alike.
 */
    ایستا تهی
do_exbuffer(exarg_T *eap)
{
    اگر (*eap->arg)
	eap->errmsg = ex_errmsg(e_trailing_characters_str, eap->arg);
    وگرنه
    {
	اگر (eap->addr_شمار == 0)	// default is current buffer
	    goto_buffer(eap, DOBUF_CURRENT, FORWARD, 0);
	وگرنه
	    goto_buffer(eap, DOBUF_FIRST, FORWARD, (صحیح)eap->line2);
	اگر (eap->do_ecmd_cmd != پوچ)
	    do_cmd_argument(eap->do_ecmd_cmd);
    }
}

/*
 * :[N]bmodified [N]	to next mod. buffer
 * :[N]sbmodified [N]	to next mod. buffer
 */
    ایستا تهی
ex_bmodified(exarg_T *eap)
{
    goto_buffer(eap, DOBUF_MOD, FORWARD, (صحیح)eap->line2);
    اگر (eap->do_ecmd_cmd != پوچ)
	do_cmd_argument(eap->do_ecmd_cmd);
}

/*
 * :[N]bnext [N]	to next buffer
 * :[N]sbnext [N]	split and to next buffer
 */
    ایستا تهی
ex_bnext(exarg_T *eap)
{
    اگر (ERROR_IF_ANY_POPUP_WINDOW)
	بازگشت;

    goto_buffer(eap, DOBUF_CURRENT, FORWARD, (صحیح)eap->line2);
    اگر (eap->do_ecmd_cmd != پوچ)
	do_cmd_argument(eap->do_ecmd_cmd);
}

/*
 * :[N]bNext [N]	to previous buffer
 * :[N]bprevious [N]	to previous buffer
 * :[N]sbNext [N]	split and to previous buffer
 * :[N]sbprevious [N]	split and to previous buffer
 */
    ایستا تهی
ex_bprevious(exarg_T *eap)
{
    اگر (ERROR_IF_ANY_POPUP_WINDOW)
	بازگشت;

    goto_buffer(eap, DOBUF_CURRENT, BACKWARD, (صحیح)eap->line2);
    اگر (eap->do_ecmd_cmd != پوچ)
	do_cmd_argument(eap->do_ecmd_cmd);
}

/*
 * :brewind		to first buffer
 * :bfirst		to first buffer
 * :sbrewind		split and to first buffer
 * :sbfirst		split and to first buffer
 */
    ایستا تهی
ex_brewind(exarg_T *eap)
{
    اگر (ERROR_IF_ANY_POPUP_WINDOW)
	بازگشت;

    goto_buffer(eap, DOBUF_FIRST, FORWARD, 0);
    اگر (eap->do_ecmd_cmd != پوچ)
	do_cmd_argument(eap->do_ecmd_cmd);
}

/*
 * :blast		to last buffer
 * :sblast		split and to last buffer
 */
    ایستا تهی
ex_blast(exarg_T *eap)
{
    اگر (ERROR_IF_ANY_POPUP_WINDOW)
	بازگشت;

    goto_buffer(eap, DOBUF_LAST, BACKWARD, 0);
    اگر (eap->do_ecmd_cmd != پوچ)
	do_cmd_argument(eap->do_ecmd_cmd);
}

/*
 * Check if "c" ends an Ex command.
 * In Vim9 script does not check for white space before #.
 */
    صحیح
ends_excmd(صحیح c)
{
    صحیح comment_char = '"';

    اگر (in_vim9script())
	comment_char = '#';
    بازگشت (c == NUL || c == '|' || c == comment_char || c == '\n');
}

/*
 * Like ends_excmd() but checks that a # in Vim9 script either has "cmd" equal
 * to "cmd_start" or has a white space character before it.
 */
    صحیح
ends_excmd2(char_u *cmd_start UNUSED, char_u *cmd)
{
    صحیح c = *cmd;

    اگر (c == NUL || c == '|' || c == '\n')
	بازگشت TRUE;
    اگر (in_vim9script())
	//  # starts a comment, #{ might be a mistake, #{{ can start a fold
	بازگشت c == '#' && (cmd[1] != '{' || cmd[2] == '{')
				 && (cmd == cmd_start || VIM_ISWHITE(cmd[-1]));
    بازگشت c == '"';
}

#اگر defined(FEAT_SYN_HL) || defined(FEAT_جستجو_EXTRA) || defined(FEAT_EVAL)
/*
 * Return the next command, after the first '|' or '\n'.
 * Return NULL if not found.
 */
    char_u *
find_nextcmd(char_u *p)
{
    وقتی (*p != '|' && *p != '\n')
    {
	اگر (*p == NUL)
	    بازگشت پوچ;
	++p;
    }
    بازگشت (p + 1);
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

    اگر (*s == '|' || *s == '\n')
	بازگشت (s + 1);
    وگرنه
	بازگشت پوچ;
}

/*
 * If "eap->nextcmd" is not set, check for a next command at "p".
 */
    تهی
set_nextcmd(exarg_T *eap, char_u *arg)
{
    char_u *p = skipwhite(arg);

    اگر (in_vim9script() && *p == '#')
    {
	char_u *nl = vim_strchr(p, NL);
	اگر (nl != پوچ)
	    p = nl;
    }

    p = check_nextcmd(p);

    اگر (eap->nextcmd == پوچ)
	eap->nextcmd = p;
    وگرنه اگر (p != پوچ)
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
    ایستا صحیح
check_more(
    صحیح message,	    // when FALSE check only, no messages
    صحیح forceit)
{
    صحیح	    n = ARGCOUNT - curwin->w_arg_idx - 1;

    اگر (!forceit && only_one_window()
	    && ARGCOUNT > 1 && !arg_had_last && n > 0 && quitmore == 0)
    {
	اگر (message)
	{
#اگر defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
	    اگر ((p_confirm || (cmdmod.cmod_flags & CMOD_CONFIRM))
						    && curbuf->b_fname != پوچ)
	    {
		char_u	buff[DIALOG_پیام_SIZE];

		vim_snprintf((نویسه *)buff, DIALOG_پیام_SIZE,
			NGETTEXT("%d more file to edit.  Quit anyway?",
			    "%d more files to edit.  Quit anyway?", n), n);
		اگر (vim_dialog_yesno(VIM_QUESTION, پوچ, buff, 1) == VIM_YES)
		    بازگشت OK;
		بازگشت FAIL;
	    }
#endif
	    semsg(NGETTEXT(e_nr_more_file_to_edit,
			   e_nr_more_files_to_edit , n), n);
	    quitmore = 2;	    // next try to quit is allowed
	}
	بازگشت FAIL;
    }
    بازگشت OK;
}

/*
 * Function given to ExpandGeneric() to obtain the list of command names.
 */
    char_u *
get_command_name(expand_T *xp UNUSED, صحیح idx)
{
    اگر (idx >= (صحیح)CMD_SIZE)
	بازگشت expand_user_command_name(idx);
    // the following are no real commands
    اگر (STRNCMP(cmdnames[idx].cmd_name, "{", 1) == 0 ||
	STRNCMP(cmdnames[idx].cmd_name, "}", 1) == 0)
	بازگشت (char_u *)"";
    بازگشت cmdnames[idx].cmd_name;
}

    ایستا تهی
ex_colorscheme(exarg_T *eap)
{
    اگر (*eap->arg == NUL)
    {
#ifdef FEAT_EVAL
	char_u *عبارت = vim_strsave((char_u *)"g:colors_name");
	char_u *p = پوچ;

	اگر (عبارت != پوچ)
	{
	    ++emsg_off;
	    p = eval_to_string(عبارت, FALSE, FALSE);
	    --emsg_off;
	    vim_free(عبارت);
	}
	اگر (p != پوچ)
	{
	    پیام((نویسه *)p);
	    vim_free(p);
	}
	وگرنه
	    پیام("default");
#وگرنه
	پیام(_("unknown"));
#endif
    }
    وگرنه اگر (load_colors(eap->arg) == FAIL)
	semsg(_(e_cannot_find_color_scheme_str), eap->arg);

#ifdef FEAT_VTP
    وگرنه اگر (has_vtp_working())
    {
	// background color change requires clear + redraw
	update_screen(UPD_CLEAR);
	redrawcmd();
    }
#endif
}

    ایستا تهی
ex_highlight(exarg_T *eap)
{
    اگر (*eap->arg == NUL && eap->cmd[2] == '!')
	پیام(_("Greetings, Vim user!"));
    do_highlight(eap->arg, eap->forceit, FALSE);
}


/*
 * Call this function if we thought we were going to exit, but we won't
 * (because of an error).  May need to restore the terminal mode.
 */
    تهی
not_exiting(صحیح save_exiting)
{
    exiting = save_exiting;
    settmode(TMODE_RAW);
}

    صحیح
before_quit_autocmds(win_T *wp, صحیح quit_all, صحیح forceit)
{
    apply_autocmds(EVENT_QUITPRE, پوچ, پوچ, FALSE, wp->w_buffer);

    // Bail out when autocommands closed the window.
    // Refuse to quit when the buffer in the last window is being closed (can
    // only happen in autocommands).
    اگر (!win_valid(wp)
	    || curbuf_locked()
	    || (wp->w_buffer->b_nwindows == 1 && wp->w_buffer->b_locked > 0))
	بازگشت TRUE;

    اگر (quit_all || (check_more(FALSE, forceit) == OK && only_one_window()))
    {
	apply_autocmds(EVENT_EXITPRE, پوچ, پوچ, FALSE, curbuf);
	// Refuse to quit when locked or when the window was closed or the
	// buffer in the last window is being closed (can only happen in
	// autocommands).
	اگر (!win_valid(wp) || curbuf_locked()
			  || (curbuf->b_nwindows == 1 && curbuf->b_locked > 0))
	    بازگشت TRUE;
    }

    بازگشت FALSE;
}

/*
 * ":quit": quit current window, quit Vim if the last window is closed.
 * ":{nr}quit": quit window {nr}
 * Also used when closing a terminal window that's the last one.
 */
    تهی
ex_quit(exarg_T *eap)
{
    win_T	*wp;

    اگر (cmdwin_type != 0)
    {
	cmdwin_result = Ctrl_C;
	بازگشت;
    }
    // Don't quit while editing the command line.
    اگر (text_locked())
    {
	text_locked_پیام();
	بازگشت;
    }
    اگر (eap->addr_شمار > 0)
    {
	صحیح	wnr = eap->line2;

	برای (wp = firstwin; wp->w_next != پوچ; wp = wp->w_next)
	    اگر (--wnr <= 0)
		بشکن;
    }
    وگرنه
	wp = curwin;

    // Refuse to quit when locked.
    اگر (curbuf_locked())
	بازگشت;

    // Trigger QuitPre and maybe ExitPre
    اگر (before_quit_autocmds(wp, FALSE, eap->forceit))
	بازگشت;

#ifdef FEAT_NETBEANS_INTG
    netbeansForcedQuit = eap->forceit;
#endif

    صحیح save_exiting = exiting;
    /*
     * If there is only one relevant window we will exit.
     */
    اگر (check_more(FALSE, eap->forceit) == OK && only_one_window())
	exiting = TRUE;
    اگر ((!buf_hide(wp->w_buffer)
		&& check_changed(wp->w_buffer, (p_awa ? CCGD_AW : 0)
				       | (eap->forceit ? CCGD_FORCEIT : 0)
				       | CCGD_EXCMD))
	    || check_more(TRUE, eap->forceit) == FAIL
	    || (only_one_window() && check_changed_any(eap->forceit, TRUE)))
    {
	not_exiting(save_exiting);
    }
    وگرنه
    {
	// quit last window
	// Note: only_one_window() returns true, even so a help window is
	// still open. In that case only quit, if no address has been
	// specified. Example:
	// :h|wincmd w|1q     - don't quit
	// :h|wincmd w|q      - quit
	اگر (only_one_window() && (ONE_WINDOW || eap->addr_شمار == 0))
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
    ایستا تهی
ex_cquit(exarg_T *eap UNUSED)
{
    // this does not always pass on the exit code to the Manx compiler. why?
    getout(eap->addr_شمار > 0 ? (صحیح)eap->line2 : EXIT_FAILURE);
}

/*
 * Do preparations for "qall" and "wqall".
 * Returns FAIL when quitting should be aborted.
 */
    صحیح
before_quit_all(exarg_T *eap)
{
    اگر (cmdwin_type != 0)
    {
	اگر (eap->forceit)
	    cmdwin_result = K_XF1;	// ex_window() takes care of this
	وگرنه
	    cmdwin_result = K_XF2;
	بازگشت FAIL;
    }

    // Don't quit while editing the command line.
    اگر (text_locked())
    {
	text_locked_پیام();
	بازگشت FAIL;
    }

    اگر (before_quit_autocmds(curwin, TRUE, eap->forceit))
	بازگشت FAIL;

    بازگشت OK;
}

/*
 * ":qall": try to quit all windows
 */
    ایستا تهی
ex_quit_all(exarg_T *eap)
{
    اگر (before_quit_all(eap) == FAIL)
	بازگشت;
    صحیح save_exiting = exiting;
    exiting = TRUE;
    اگر (eap->forceit || !check_changed_any(FALSE, FALSE))
	getout(0);
    not_exiting(save_exiting);
}

/*
 * ":close": close current window, unless it is the last one
 */
    ایستا تهی
ex_close(exarg_T *eap)
{
    win_T	*win;
    صحیح		winnr = 0;
    اگر (cmdwin_type != 0)
	cmdwin_result = Ctrl_C;
    وگرنه
	اگر (!text_locked() && !curbuf_locked())
	{
	    اگر (eap->addr_شمار == 0)
		ex_win_close(eap->forceit, curwin, پوچ);
	    وگرنه
	    {
		FOR_ALL_WINDOWS(win)
		{
		    winnr++;
		    اگر (winnr == eap->line2)
			بشکن;
		}
		اگر (win == پوچ)
		    win = lastwin;
		ex_win_close(eap->forceit, win, پوچ);
	    }
	}
}

#ifdef FEAT_QUICKFIX
/*
 * ":pclose": Close any preview window.
 */
    ایستا تهی
ex_pclose(exarg_T *eap)
{
    win_T	*win;

    // First close any normal window.
    FOR_ALL_WINDOWS(win)
	اگر (win->w_p_pvw)
	{
	    ex_win_close(eap->forceit, win, پوچ);
	    بازگشت;
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
    ایستا تهی
ex_win_close(
    صحیح		forceit,
    win_T	*win,
    tabpage_T	*tp)		// NULL or the tab page "win" is in
{
    صحیح		need_hide;
    buf_T	*buf = win->w_buffer;

    // Never close the autocommand window.
    اگر (is_aucmd_win(win))
    {
	emsg(_(e_cannot_close_autocmd_or_popup_window));
	بازگشت;
    }
    اگر (window_layout_locked(CMD_close))
	بازگشت;

    need_hide = (bufIsChanged(buf) && buf->b_nwindows <= 1);
    اگر (need_hide && !buf_hide(buf) && !forceit)
    {
#اگر defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
	اگر ((p_confirm || (cmdmod.cmod_flags & CMOD_CONFIRM)) && p_write)
	{
# ifdef FEAT_TERMINAL
	    اگر (term_job_running(buf->b_term))
	    {
		اگر (term_confirm_stop(buf) == FAIL)
		    بازگشت;
		// Manually kill the terminal here because this command will
		// hide it otherwise.
		free_terminal(buf);
		need_hide = FALSE;
	    }
	    وگرنه
# endif
	    {
		bufref_T bufref;

		set_bufref(&bufref, buf);
		dialog_changed(buf, FALSE);
		اگر (bufref_valid(&bufref) && bufIsChanged(buf))
		    بازگشت;
		need_hide = FALSE;
	    }
	}
	وگرنه
#endif
	{
	    no_write_message();
	    بازگشت;
	}
    }

#ifdef FEAT_GUI
    need_mouse_correct = TRUE;
#endif

    // free buffer when not hiding it or when it's a scratch buffer
    اگر (tp == پوچ)
	win_close(win, !need_hide && !buf_hide(buf));
    وگرنه
	win_close_othertab(win, !need_hide && !buf_hide(buf), tp);
}

/*
 * Handle the argument for a tabpage related ex command.
 * Returns a tabpage number.
 * When an error is encountered then eap->errmsg is set.
 */
    ایستا صحیح
get_tabpage_arg(exarg_T *eap)
{
    صحیح tab_عدد;
    صحیح unaccept_arg0 = (eap->cmdidx == CMD_tabmove) ? 0 : 1;

    اگر (eap->arg && *eap->arg != NUL)
    {
	char_u *p = eap->arg;
	char_u *p_save;
	صحیح    relative = 0; // argument +N/-N means: go to N places to the
			     // right/left relative to the current position.

	اگر (*p == '-')
	{
	    relative = -1;
	    p++;
	}
	وگرنه اگر (*p == '+')
	{
	    relative = 1;
	    p++;
	}

	p_save = p;
	tab_عدد = getdigits(&p);

	اگر (relative == 0)
	{
	    اگر (STRCMP(p, "$") == 0)
		tab_عدد = LAST_TAB_NR;
	    وگرنه اگر (STRCMP(p, "#") == 0)
		اگر (valid_tabpage(lastused_tabpage))
		    tab_عدد = tabpage_index(lastused_tabpage);
		وگرنه
		{
		    eap->errmsg = ex_errmsg(e_invalid_مقدار_for_argument_str,
								     eap->arg);
		    tab_عدد = 0;
		    برو theend;
		}
	    وگرنه اگر (p == p_save || *p_save == '-' || *p != NUL
		    || tab_عدد > LAST_TAB_NR)
	    {
		// No numbers as argument.
		eap->errmsg = ex_errmsg(e_invalid_argument_str, eap->arg);
		برو theend;
	    }
	}
	وگرنه
	{
	    اگر (*p_save == NUL)
		tab_عدد = 1;
	    وگرنه اگر (p == p_save || *p_save == '-' || *p != NUL
		    || tab_عدد == 0)
	    {
		// No numbers as argument.
		eap->errmsg = ex_errmsg(e_invalid_argument_str, eap->arg);
		برو theend;
	    }
	    tab_عدد = tab_عدد * relative + tabpage_index(curtab);
	    اگر (!unaccept_arg0 && relative == -1)
		--tab_عدد;
	}
	اگر (tab_عدد < unaccept_arg0 || tab_عدد > LAST_TAB_NR)
	    eap->errmsg = ex_errmsg(e_invalid_argument_str, eap->arg);
    }
    وگرنه اگر (eap->addr_شمار > 0)
    {
	اگر (unaccept_arg0 && eap->line2 == 0)
	{
	    eap->errmsg = _(e_invalid_range);
	    tab_عدد = 0;
	}
	وگرنه
	{
	    tab_عدد = eap->line2;
	    اگر (!unaccept_arg0)
	    {
		char_u *cmdp = eap->cmd;

		وقتی (--cmdp > *eap->cmdlinep
			&& (VIM_ISWHITE(*cmdp) || VIM_ISDIGIT(*cmdp)))
		    ;
		اگر (*cmdp == '-')
		{
		    --tab_عدد;
		    اگر (tab_عدد < unaccept_arg0)
			eap->errmsg = _(e_invalid_range);
		}
	    }
	}
    }
    وگرنه
    {
	انتخاب (eap->cmdidx)
	{
	حالت CMD_tabnext:
	    tab_عدد = tabpage_index(curtab) + 1;
	    اگر (tab_عدد > LAST_TAB_NR)
		tab_عدد = 1;
	    بشکن;
	حالت CMD_tabmove:
	    tab_عدد = LAST_TAB_NR;
	    بشکن;
	پیشفرض:
	    tab_عدد = tabpage_index(curtab);
	}
    }

theend:
    بازگشت tab_عدد;
}

/*
 * ":tabclose": close current tab page, unless it is the last one.
 * ":tabclose N": close tab page N.
 */
    ایستا تهی
ex_tabclose(exarg_T *eap)
{
    tabpage_T	*tp;
    صحیح		tab_عدد;

    اگر (cmdwin_type != 0)
    {
	cmdwin_result = K_IGNORE;
	بازگشت;
    }

    اگر (first_tabpage->tp_next == پوچ)
    {
	emsg(_(e_cannot_close_last_tab_page));
	بازگشت;
    }

    اگر (window_layout_locked(CMD_tabclose))
	بازگشت;

    tab_عدد = get_tabpage_arg(eap);
    اگر (eap->errmsg != پوچ)
	بازگشت;

    tp = find_tabpage(tab_عدد);
    اگر (tp == پوچ)
    {
	beep_flush();
	بازگشت;
    }
    اگر (tp != curtab)
    {
	tabpage_close_other(tp, eap->forceit);
	بازگشت;
    }
    وگرنه اگر (!text_locked() && !curbuf_locked())
	tabpage_close(eap->forceit);
}

/*
 * ":tabonly": close all tab pages except the current one
 */
    ایستا تهی
ex_tabonly(exarg_T *eap)
{
    tabpage_T	*tp;
    صحیح		done;
    صحیح		tab_عدد;

    اگر (cmdwin_type != 0)
    {
	cmdwin_result = K_IGNORE;
	بازگشت;
    }

    اگر (first_tabpage->tp_next == پوچ)
    {
	پیام(_("Already only one tab page"));
	بازگشت;
    }

    اگر (window_layout_locked(CMD_tabonly))
	بازگشت;

    tab_عدد = get_tabpage_arg(eap);
    اگر (eap->errmsg != پوچ)
	بازگشت;

    goto_tabpage(tab_عدد);
    // Repeat this up to a 1000 times, because autocommands may
    // mess up the lists.
    برای (done = 0; done < 1000; ++done)
    {
	FOR_ALL_TABPAGES(tp)
	    اگر (tp->tp_topframe != topframe)
	    {
		tabpage_close_other(tp, eap->forceit);
		// if we failed to close it quit
		اگر (valid_tabpage(tp))
		    done = 1000;
		// start over, "tp" is now invalid
		بشکن;
	    }
	اگر (first_tabpage->tp_next == پوچ)
	    بشکن;
    }
}

/*
 * Close the current tab page.
 */
    تهی
tabpage_close(صحیح forceit)
{
    اگر (window_layout_locked(CMD_tabclose))
	بازگشت;

    trigger_tabclosedpre(curtab);
    curtab->tp_did_tabclosedpre = TRUE;
    tabpage_T *save_curtab = curtab;

    // First close all the windows but the current one.  If that worked then
    // close the last window in this tab, that will close it.
    اگر (!ONE_WINDOW)
	close_others(TRUE, forceit);
    اگر (ONE_WINDOW)
	ex_win_close(forceit, curwin, پوچ);
    اگر (curtab == save_curtab)
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
    تهی
tabpage_close_other(tabpage_T *tp, صحیح forceit)
{
    صحیح		done = 0;
    win_T	*wp;

    اگر (window_layout_locked(CMD_SIZE))
	بازگشت;

    trigger_tabclosedpre(tp);
    tp->tp_did_tabclosedpre = TRUE;

    // Limit to 1000 windows, autocommands may add a window while we close
    // one.  OK, so I'm paranoid...
    وقتی (++done < 1000)
    {
	wp = tp->tp_firstwin;
	ex_win_close(forceit, wp, tp);

	// Autocommands may delete the tab page under our fingers.
	اگر (!valid_tabpage(tp))
	    بشکن;
	// We may fail to close a window with a modified buffer.
	اگر (tp->tp_firstwin == wp)
	{
	    done = 1000;
	    بشکن;
	}
    }
    اگر (done >= 1000)
    {
	// When closing the tab page failed, reset tp_did_tabclosedpre so that
	// TabClosedPre behaves consistently on next :close vs :tabclose.
	tp->tp_did_tabclosedpre = FALSE;
	بازگشت;
    }

    apply_autocmds(EVENT_TABCLOSED, پوچ, پوچ, FALSE, curbuf);
}

/*
 * ":only".
 */
    ایستا تهی
ex_only(exarg_T *eap)
{
    اگر (window_layout_locked(CMD_only))
	بازگشت;
#ifdef FEAT_GUI
    need_mouse_correct = TRUE;
#endif
    اگر (eap->addr_شمار > 0)
    {
	win_T   *wp;
	صحیح	wnr = eap->line2;
	برای (wp = firstwin; --wnr > 0; )
	{
	    اگر (wp->w_next == پوچ)
		بشکن;
	    وگرنه
		wp = wp->w_next;
	}
	win_goto(wp);
    }
    close_others(TRUE, eap->forceit);
}

    ایستا تهی
ex_hide(exarg_T *eap UNUSED)
{
    // ":hide" or ":hide | cmd": hide current window
    اگر (eap->skip)
	بازگشت;

    اگر (window_layout_locked(CMD_hide))
	بازگشت;
#ifdef FEAT_GUI
    need_mouse_correct = TRUE;
#endif
    اگر (eap->addr_شمار == 0)
	win_close(curwin, FALSE);	// don't free buffer
    وگرنه
    {
	صحیح	winnr = 0;
	win_T	*win;

	FOR_ALL_WINDOWS(win)
	{
	    winnr++;
	    اگر (winnr == eap->line2)
		بشکن;
	}
	اگر (win == پوچ)
	    win = lastwin;
	win_close(win, FALSE);
    }
}

/*
 * ":stop" and ":suspend": Suspend Vim.
 */
    تهی
ex_stop(exarg_T *eap)
{
    /*
     * Disallow suspending for "rvim".
     */
    اگر (check_restricted())
	بازگشت;

    اگر (!eap->forceit)
	autowrite_all();
    apply_autocmds(EVENT_VIMSUSPEND, پوچ, پوچ, FALSE, پوچ);
    windgoto((صحیح)Rows - 1, cmdline_col_off);
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
    apply_autocmds(EVENT_VIMRESUME, پوچ, پوچ, FALSE, پوچ);
}

/*
 * ":exit", ":xit" and ":wq": Write file and quit the current window.
 */
    ایستا تهی
ex_exit(exarg_T *eap)
{
#ifdef FEAT_EVAL
    اگر (not_in_vim9(eap, false) == FAIL)
	بازگشت;
#endif
    اگر (cmdwin_type != 0)
    {
	cmdwin_result = Ctrl_C;
	بازگشت;
    }
    // Don't quit while editing the command line.
    اگر (text_locked())
    {
	text_locked_پیام();
	بازگشت;
    }

    صحیح save_exiting = exiting;
    /*
     * we plan to exit if there is only one relevant window
     */
    اگر (check_more(FALSE, eap->forceit) == OK && only_one_window())
	exiting = TRUE;

    // Write the buffer for ":wq" or when it was changed.
    // Trigger QuitPre and ExitPre.
    // Check if we can exit now, after autocommands have changed things.
    اگر (((eap->cmdidx == CMD_wq || curbufIsChanged()) && do_write(eap) == FAIL)
	    || before_quit_autocmds(curwin, FALSE, eap->forceit)
	    || check_more(TRUE, eap->forceit) == FAIL
	    || (only_one_window() && check_changed_any(eap->forceit, FALSE)))
    {
	not_exiting(save_exiting);
    }
    وگرنه
    {
	اگر (only_one_window())	    // quit last window, exit Vim
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
    ایستا تهی
ex_print(exarg_T *eap)
{
    اگر (curbuf->b_ml.ml_flags & ML_EMPTY)
	emsg(_(e_empty_buffer));
    وگرنه
    {
	برای ( ;!got_int; ui_breakcheck())
	{
	    print_سطر(eap->line1,
		    (eap->cmdidx == CMD_عدد || eap->cmdidx == CMD_pound
						 || (eap->flags & EXFLAG_NR)),
		    eap->cmdidx == CMD_list || (eap->flags & EXFLAG_LIST));
	    اگر (++eap->line1 > eap->line2)
		بشکن;
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
    ایستا تهی
ex_goto(exarg_T *eap)
{
    goto_byte(eap->line2);
}
#endif

/*
 * ":shell".
 */
    ایستا تهی
ex_shell(exarg_T *eap UNUSED)
{
    do_shell(پوچ, 0);
}

#اگر defined(HAVE_DROP_FILE)

ایستا صحیح drop_busy = FALSE;
ایستا صحیح drop_filec;
ایستا char_u **drop_filev = پوچ;
ایستا صحیح drop_split;
ایستا تهی (*drop_callback)(تهی *);
ایستا تهی *drop_cookie;

    ایستا تهی
handle_drop_internal(تهی)
{
    exarg_T	ea;
    صحیح		save_پیام_scroll = پیام_scroll;

    // Setting the argument list may cause screen updates and being called
    // recursively.  Avoid that by setting drop_busy.
    drop_busy = TRUE;

    // Check whether the current buffer is changed. If so, we will need
    // to split the current window or data could be lost.
    // We don't need to check if the 'hidden' option is set, as in this
    // case the buffer won't be lost.
    اگر (!buf_hide(curbuf) && !drop_split)
    {
	++emsg_off;
	drop_split = check_changed(curbuf, CCGD_AW);
	--emsg_off;
    }
    اگر (drop_split)
    {
	اگر (win_split(0, 0) == FAIL)
	    بازگشت;
	RESET_BINDING(curwin);

	// When splitting the window, create a new alist.  Otherwise the
	// existing one is overwritten.
	alist_unlink(curwin->w_alist);
	alist_new();
    }

    /*
     * Set up the new argument list.
     */
    alist_set(ALIST(curwin), drop_filec, drop_filev, FALSE, پوچ, 0);

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
    پیام_scroll = save_پیام_scroll;

    اگر (drop_callback != پوچ)
	drop_callback(drop_cookie);

    drop_filev = پوچ;
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
    تهی
handle_drop(
    صحیح		filec,		// the number of files dropped
    char_u	**filev,	// the list of files dropped
    صحیح		split,		// force splitting the window
    تهی	(*callback)(تهی *), // to be called after setting the argument
				     // list
    تهی	*cookie)	// argument for "callback" (allocated)
{
    // Cannot handle recursive drops, finish the pending one.
    اگر (drop_busy)
    {
	FreeWild(filec, filev);
	vim_free(cookie);
	بازگشت;
    }

    // When calling handle_drop() more than once in a row we only use the last
    // one.
    اگر (drop_filev != پوچ)
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
    اگر (text_locked() || curbuf_locked() || updating_screen)
	بازگشت;

    handle_drop_internal();
}

/*
 * To be called when text is unlocked, curbuf is unlocked or updating_screen is
 * reset: Handle a postponed drop.
 */
    تهی
handle_any_postponed_drop(تهی)
{
    اگر (!drop_busy && drop_filev != پوچ
	     && !text_locked() && !curbuf_locked() && !updating_screen)
	handle_drop_internal();
}
#endif

/*
 * ":preserve".
 */
    ایستا تهی
ex_preserve(exarg_T *eap UNUSED)
{
    curbuf->b_flags |= BF_PRESERVED;
    ml_preserve(curbuf, TRUE);
}

/*
 * ":recover".
 */
    ایستا تهی
ex_recover(exarg_T *eap)
{
    // Set recoverymode right away to avoid the ATTENTION prompt.
    recoverymode = TRUE;
    اگر (!check_changed(curbuf, (p_awa ? CCGD_AW : 0)
			     | CCGD_MULTWIN
			     | (eap->forceit ? CCGD_FORCEIT : 0)
			     | CCGD_EXCMD)

	    && (*eap->arg == NUL
			     || setfname(curbuf, eap->arg, پوچ, TRUE) == OK))
	ml_recover(TRUE);
    recoverymode = FALSE;
}

/*
 * Command modifier used in a wrong way.  Also for other commands that can't
 * appear at the toplevel.
 */
    ایستا تهی
ex_wrongmodifier(exarg_T *eap)
{
    eap->errmsg = ex_errmsg(e_invalid_command_str, eap->cmd);
}

#اگر defined(FEAT_EVAL)

// callback function for 'findfunc'
ایستا callback_T ffu_cb;

    ایستا callback_T *
get_findfunc_callback(تهی)
{
    بازگشت *curbuf->b_p_ffu != NUL ? &curbuf->b_ffu_cb : &ffu_cb;
}

/*
 * Call 'findfunc' to obtain a list of file names.
 */
    ایستا list_T *
call_findfunc(char_u *pat, صحیح cmdcomplete)
{
    typval_T	args[3];
    callback_T	*cb;
    typval_T	rettv;
    صحیح		retval;
    sctx_T	saved_sctx = current_sctx;
    sctx_T	*ctx;

    args[0].v_type = VAR_STRING;
    args[0].vval.v_string = pat;
    args[1].v_type = VAR_BOOL;
    args[1].vval.v_عدد = cmdcomplete;
    args[2].v_type = VAR_UNKNOWN;

    // Lock the text to prevent weird things from happening.  Also disallow
    // switching to another window, it should not be needed and may end up in
    // Insert mode in another buffer.
    ++textlock;

    ctx = get_option_sctx("findfunc");
    اگر (ctx != پوچ)
	current_sctx = *ctx;

    cb = get_findfunc_callback();
    retval = call_callback(cb, -1, &rettv, 2, args);

    current_sctx = saved_sctx;

    --textlock;

    list_T *retlist = پوچ;

    اگر (retval == OK)
    {
	اگر (rettv.v_type == VAR_LIST)
	    retlist = list_copy(rettv.vval.v_list, FALSE, FALSE, get_copyID());
	وگرنه
	    emsg(_(e_invalid_return_type_from_findfunc));

	clear_tv(&rettv);
    }

    بازگشت retlist;
}

/*
 * Find file names matching "pat" using 'findfunc' and return it in "files".
 * Used for expanding the :find, :sfind and :tabfind command argument.
 * Returns OK on success and FAIL otherwise.
 */
    صحیح
expand_findfunc(expand_T *xp, char_u *pat, char_u ***files, صحیح *رقم_Matches)
{
    list_T	*l;
    صحیح		len;

    *رقم_Matches = 0;
    *files = پوچ;

    l = call_findfunc(pat, VVAL_TRUE);

    اگر (l == پوچ)
	بازگشت FAIL;

    len = list_len(l);
    اگر (len == 0)	    // empty List
    {
	list_free(l);
	بازگشت FAIL;
    }

    expand_process_user_list(l, files, رقم_Matches, xp);
    list_free(l);

    بازگشت OK;
}

/*
 * Use 'findfunc' to find file 'findarg'.  The 'count' argument is used to find
 * the n'th matching file.
 */
    ایستا char_u *
findfunc_find_file(char_u *findarg, صحیح findarg_len, صحیح شمار)
{
    list_T	*fname_list;
    char_u	*ret_fname = پوچ;
    char_u	cc;
    صحیح		fname_شمار;

    cc = findarg[findarg_len];
    findarg[findarg_len] = NUL;

    fname_list = call_findfunc(findarg, VVAL_FALSE);
    fname_شمار = list_len(fname_list);

    اگر (fname_شمار == 0)
	semsg(_(e_cant_find_file_str_in_path), findarg);
    وگرنه
    {
	اگر (شمار > fname_شمار)
	    semsg(_(e_no_more_file_str_found_in_path), findarg);
	وگرنه
	{
	    listitem_T *li = list_find(fname_list, شمار - 1);

	    اگر (li != پوچ)
	    {
		typval_T *tv = &li->li_tv;

		اگر (tv->v_type == VAR_STRING && tv->vval.v_string != پوچ)
		    ret_fname = vim_strsave(tv->vval.v_string);
		وگرنه اگر (tv->v_type == VAR_DICT && tv->vval.v_dict != پوچ)
		    ret_fname = dict_get_string(tv->vval.v_dict, "word", TRUE);
	    }
	}
    }

    اگر (fname_list != پوچ)
	list_free(fname_list);

    findarg[findarg_len] = cc;

    بازگشت ret_fname;
}

/*
 * Process the 'findfunc' option value.
 * Returns NULL on success and an error message on failure.
 */
    نویسه *
did_set_findfunc(optset_T *args UNUSED)
{
    صحیح	retval;

    اگر (args->os_flags & OPT_LOCAL)
	// buffer-local option set
	retval = option_set_callback_func(curbuf->b_p_ffu, &curbuf->b_ffu_cb);
    وگرنه
    {
	// global option set
	retval = option_set_callback_func(p_ffu, &ffu_cb);
	// when using :set, free the local callback
	اگر (!(args->os_flags & OPT_GLOBAL))
	    free_callback(&curbuf->b_ffu_cb);
    }

    اگر (retval == FAIL)
	بازگشت e_invalid_argument;

    // If the option value starts with <SID> or s:, then replace that with
    // the script identifier.
    char_u	**varp = (char_u **)args->os_varp;
    char_u	*name = get_scriptlocal_funcname(*varp);
    اگر (name != پوچ)
    {
	free_string_option(*varp);
	*varp = name;
    }

    بازگشت پوچ;
}

# اگر defined(EXITFREE)
    تهی
free_findfunc_option(تهی)
{
    free_callback(&ffu_cb);
}
# endif

/*
 * Mark the global 'findfunc' callback with "copyID" so that it is not
 * garbage collected.
 */
    صحیح
set_ref_in_findfunc(صحیح copyID UNUSED)
{
    صحیح abort = FALSE;
    abort = set_ref_in_callback(&ffu_cb, copyID);
    بازگشت abort;
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
    تهی
ex_splitview(exarg_T *eap)
{
    win_T	*old_curwin = curwin;
    char_u	*fname = پوچ;
#ifdef FEAT_BROWSE
    char_u	dot_path[] = ".";
    صحیح		save_cmod_flags = cmdmod.cmod_flags;
#endif
    صحیح		use_tab = eap->cmdidx == CMD_tabedit
		       || eap->cmdidx == CMD_tabfind
		       || eap->cmdidx == CMD_tabnew;

    اگر (ERROR_IF_ANY_POPUP_WINDOW)
	بازگشت;

#ifdef FEAT_GUI
    need_mouse_correct = TRUE;
#endif

#ifdef FEAT_QUICKFIX
    // A ":split" in the quickfix window works like ":new".  Don't want two
    // quickfix windows.  But it's OK when doing ":tab split".
    اگر (bt_quickfix(curbuf) && cmdmod.cmod_tab == 0)
    {
	اگر (eap->cmdidx == CMD_split)
	    eap->cmdidx = CMD_new;
	اگر (eap->cmdidx == CMD_vsplit)
	    eap->cmdidx = CMD_vnew;
    }
#endif

    اگر (eap->cmdidx == CMD_sfind || eap->cmdidx == CMD_tabfind)
    {
	char_u	*file_to_find = پوچ;
	نویسه	*جستجو_ctx = پوچ;

	اگر (*get_findfunc() != NUL)
	{
#ifdef FEAT_EVAL
	    fname = findfunc_find_file(eap->arg, (صحیح)STRLEN(eap->arg),
				       eap->addr_شمار > 0 ? eap->line2 : 1);
#endif
	}
	وگرنه
	{
	    fname = find_file_in_path(eap->arg, (صحیح)STRLEN(eap->arg),
					      FNAME_MESS, TRUE, curbuf->b_ffname,
					      &file_to_find, &جستجو_ctx);
	    vim_free(file_to_find);
	    vim_findfile_cleanup(جستجو_ctx);
	}
	اگر (fname == پوچ)
	    برو theend;
	eap->arg = fname;
    }
#ifdef FEAT_BROWSE
    وگرنه اگر ((cmdmod.cmod_flags & CMOD_BROWSE)
	    && eap->cmdidx != CMD_vnew
	    && eap->cmdidx != CMD_new)
    {
	اگر (
# ifdef FEAT_GUI
	    !gui.in_use &&
# endif
		au_has_group((char_u *)"FileExplorer"))
	{
	    // No browsing supported but we do have the file explorer:
	    // Edit the directory.
	    اگر (*eap->arg == NUL || !mch_isdir(eap->arg))
		eap->arg = dot_path;
	}
	وگرنه
	{
	    fname = do_browse(0, (char_u *)(use_tab
			? _("Edit File in new tab page")
			: _("Edit File in new window")),
					  eap->arg, پوچ, پوچ, پوچ, curbuf);
	    اگر (fname == پوچ)
		برو theend;
	    eap->arg = fname;
	}
    }
    cmdmod.cmod_flags &= ~CMOD_BROWSE;	// Don't browse again in do_ecmd().
#endif

    /*
     * Either open new tab page or split the window.
     */
    اگر (use_tab)
    {
	اگر (win_new_tabpage(cmdmod.cmod_tab != 0 ? cmdmod.cmod_tab
			 : eap->addr_شمار == 0 ? 0
					       : (صحیح)eap->line2 + 1) != FAIL)
	{
	    do_exedit(eap, old_curwin);

	    // set the alternate buffer for the window we came from
	    اگر (curwin != old_curwin
		    && win_valid(old_curwin)
		    && old_curwin->w_buffer != curbuf
		    && (cmdmod.cmod_flags & CMOD_KEEPALT) == 0)
		old_curwin->w_alt_fnum = curbuf->b_fnum;
	}
    }
    وگرنه اگر (win_split(eap->addr_شمار > 0 ? (صحیح)eap->line2 : 0,
				     *eap->cmd == 'v' ? WSP_VERT : 0) != FAIL)
    {
	// Reset 'scrollbind' when editing another file, but keep it when
	// doing ":split" without arguments.
	اگر (*eap->arg != NUL)
	    RESET_BINDING(curwin);
	وگرنه
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
    تهی
tabpage_new(تهی)
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
    ایستا تهی
ex_tabnext(exarg_T *eap)
{
    صحیح tab_عدد;

    اگر (ERROR_IF_POPUP_WINDOW)
	بازگشت;
    انتخاب (eap->cmdidx)
    {
	حالت CMD_tabfirst:
	حالت CMD_tabrewind:
	    goto_tabpage(1);
	    بشکن;
	حالت CMD_tablast:
	    goto_tabpage(9999);
	    بشکن;
	حالت CMD_tabprevious:
	حالت CMD_tabNext:
	    اگر (eap->arg && *eap->arg != NUL)
	    {
		char_u *p = eap->arg;
		char_u *p_save = p;

		tab_عدد = getdigits(&p);
		اگر (p == p_save || *p_save == '-' || *p != NUL
			    || tab_عدد == 0)
		{
		    // No numbers as argument.
		    eap->errmsg = ex_errmsg(e_invalid_argument_str, eap->arg);
		    بازگشت;
		}
	    }
	    وگرنه
	    {
		اگر (eap->addr_شمار == 0)
		    tab_عدد = 1;
		وگرنه
		{
		    tab_عدد = eap->line2;
		    اگر (tab_عدد < 1)
		    {
			eap->errmsg = _(e_invalid_range);
			بازگشت;
		    }
		}
	    }
	    goto_tabpage(-tab_عدد);
	    بشکن;
	پیشفرض: // CMD_tabnext
	    tab_عدد = get_tabpage_arg(eap);
	    اگر (eap->errmsg == پوچ)
		goto_tabpage(tab_عدد);
	    بشکن;
    }
}

/*
 * :tabmove command
 */
    ایستا تهی
ex_tabmove(exarg_T *eap)
{
    صحیح tab_عدد;

    tab_عدد = get_tabpage_arg(eap);
    اگر (eap->errmsg == پوچ)
	tabpage_move(tab_عدد);
}

/*
 * :tabs command: List tabs and their contents.
 */
    ایستا تهی
ex_tabs(exarg_T *eap UNUSED)
{
    tabpage_T	*tp;
    win_T	*wp;
    صحیح		tabcount = 1;

    پیام_start();
    پیام_scroll = TRUE;
    برای (tp = first_tabpage; tp != پوچ && !got_int; tp = tp->tp_next)
    {
	پیام_putchar('\n');
	vim_snprintf((نویسه *)IObuff, IOSIZE, _("Tab page %d"), tabcount++);
	پیام_outtrans_attr(IObuff, HL_ATTR(HLF_T));
	out_flush();	    // output one line at a time
	ui_breakcheck();

	اگر (tp  == curtab)
	    wp = firstwin;
	وگرنه
	    wp = tp->tp_firstwin;
	برای ( ; wp != پوچ && !got_int; wp = wp->w_next)
	{
	    پیام_putchar('\n');
	    پیام_putchar(wp == curwin ? '>' : ' ');
	    پیام_putchar(' ');
	    پیام_putchar(bufIsChanged(wp->w_buffer) ? '+' : ' ');
	    پیام_putchar(' ');
	    اگر (buf_spname(wp->w_buffer) != پوچ)
		vim_strncpy(IObuff, buf_spname(wp->w_buffer), IOSIZE - 1);
	    وگرنه
		home_replace(wp->w_buffer, wp->w_buffer->b_fname,
							IObuff, IOSIZE, TRUE);
	    پیام_outtrans(IObuff);
	    out_flush();	    // output one line at a time
	    ui_breakcheck();
	}
    }
}

/*
 * ":mode": Set screen mode.
 * If no argument given, just get the screen size and redraw.
 */
    ایستا تهی
ex_mode(exarg_T *eap)
{
    اگر (*eap->arg == NUL)
	shell_resized();
    وگرنه
	emsg(_(e_screen_mode_setting_not_supported));
}

/*
 * ":resize".
 * set, increment or decrement current window height
 */
    ایستا تهی
ex_resize(exarg_T *eap)
{
    صحیح		n;
    win_T	*wp = curwin;

    اگر (eap->addr_شمار > 0)
    {
	n = eap->line2;
	برای (wp = firstwin; wp->w_next != پوچ && --n > 0; wp = wp->w_next)
	    ;
    }

#ifdef FEAT_GUI
    need_mouse_correct = TRUE;
#endif
    n = atol((نویسه *)eap->arg);
    اگر (cmdmod.cmod_split & WSP_VERT)
    {
	اگر (*eap->arg == '-' || *eap->arg == '+')
	    n += wp->w_width;
	وگرنه اگر (n == 0 && eap->arg[0] == NUL)	// default is very wide
	    n = 9999;
	win_setwidth_win(n, wp);
    }
    وگرنه
    {
	اگر (*eap->arg == '-' || *eap->arg == '+')
	    n += wp->w_height;
	وگرنه اگر (n == 0 && eap->arg[0] == NUL)	// default is very high
	    n = 9999;
	win_setheight_win(n, wp);
    }
}

/*
 * ":find [+command] <file>" command.
 */
    ایستا تهی
ex_find(exarg_T *eap)
{
    اگر (!check_can_set_curbuf_forceit(eap->forceit))
	بازگشت;

    char_u	*fname = پوچ;
    صحیح		شمار;
    char_u	*file_to_find = پوچ;
    نویسه	*جستجو_ctx = پوچ;

    اگر (*get_findfunc() != NUL)
    {
#ifdef FEAT_EVAL
	fname = findfunc_find_file(eap->arg, (صحیح)STRLEN(eap->arg),
					eap->addr_شمار > 0 ? eap->line2 : 1);
#endif
    }
    وگرنه
    {
	fname = find_file_in_path(eap->arg, (صحیح)STRLEN(eap->arg), FNAME_MESS,
			       TRUE, curbuf->b_ffname, &file_to_find, &جستجو_ctx);
	اگر (eap->addr_شمار > 0)
	{
	    // Repeat finding the file "count" times.  This matters when it appears
	    // several times in the path.
	    شمار = eap->line2;
	    وقتی (fname != پوچ && --شمار > 0)
	    {
		vim_free(fname);
		fname = find_file_in_path(پوچ, 0, FNAME_MESS,
			      FALSE, curbuf->b_ffname, &file_to_find, &جستجو_ctx);
	    }
	}
	VIM_CLEAR(file_to_find);
	vim_findfile_cleanup(جستجو_ctx);
    }

    اگر (fname == پوچ)
	بازگشت;

    eap->arg = fname;
    do_exedit(eap, پوچ);
    vim_free(fname);
}

/*
 * ":open" simulation: for now works just like ":visual".
 */
    ایستا تهی
ex_open(exarg_T *eap)
{
    regmatch_T	regmatch;
    char_u	*p;

#ifdef FEAT_EVAL
    اگر (not_in_vim9(eap, false) == FAIL)
	بازگشت;
#endif
    curwin->w_cursor.lnum = eap->line2;
    beginline(BL_SOL | BL_FIX);
    اگر (*eap->arg == '/')
    {
	// ":open /pattern/": put cursor in column found with pattern
	++eap->arg;
	p = skip_regexp(eap->arg, '/', magic_isset());
	*p = NUL;
	regmatch.regprog = vim_regcomp(eap->arg, magic_isset() ? RE_MAGIC : 0);
	اگر (regmatch.regprog != پوچ)
	{
	    // make a copy of the line, when searching for a mark it might be
	    // flushed
	    char_u *سطر = vim_strsave(ml_get_curline());

	    regmatch.rm_ic = p_ic;
	    اگر (vim_regexec(&regmatch, سطر, (colnr_T)0))
		curwin->w_cursor.col = (colnr_T)(regmatch.startp[0] - سطر);
	    وگرنه
		emsg(_(e_no_match));
	    vim_regfree(regmatch.regprog);
	    vim_free(سطر);
	}
	// Move to the NUL, ignore any other arguments.
	eap->arg += STRLEN(eap->arg);
    }
    check_cursor();

    eap->cmdidx = CMD_visual;
    do_exedit(eap, پوچ);
}

/*
 * ":edit", ":badd", ":balt", ":visual".
 */
    ایستا تهی
ex_edit(exarg_T *eap)
{
    char_u *ffname = eap->cmdidx == CMD_enew ? پوچ : eap->arg;

    // Exclude commands which keep the window's current buffer
    اگر (
	    eap->cmdidx != CMD_badd
	    && eap->cmdidx != CMD_balt
	    // All other commands must obey 'winfixbuf' / ! rules
	    && (is_other_file(0, ffname) && !check_can_set_curbuf_forceit(eap->forceit))
    )
	بازگشت;

    do_exedit(eap, پوچ);
}

/*
 * ":edit <file>" command and alike.
 */
    تهی
do_exedit(
    exarg_T	*eap,
    win_T	*old_curwin)	    // curwin before doing a split or NULL
{
    صحیح		n;
    صحیح		need_hide;
    صحیح		exmode_was = exmode_active;

    اگر ((eap->cmdidx != CMD_pedit && ERROR_IF_POPUP_WINDOW)
						 || ERROR_IF_TERM_POPUP_WINDOW)
	بازگشت;
    /*
     * ":vi" command ends Ex mode.
     */
    اگر (exmode_active && (eap->cmdidx == CMD_visual
						|| eap->cmdidx == CMD_view))
    {
	exmode_active = FALSE;
	ex_pressedreturn = FALSE;
	اگر (*eap->arg == NUL)
	{
	    // Special case:  ":global/pat/visual\NLvi-commands"
	    اگر (global_busy)
	    {
		اگر (eap->nextcmd != پوچ)
		{
		    stuffReadbuff(eap->nextcmd);
		    eap->nextcmd = پوچ;
		}

		اگر (exmode_was != EXMODE_VIM)
		    settmode(TMODE_RAW);
		صحیح save_RedrawingDisabled = RedrawingDisabled;
		RedrawingDisabled = 0;
		صحیح save_nwr = no_wait_return;
		no_wait_return = 0;
		need_wait_return = FALSE;
		صحیح save_ms = پیام_scroll;
		پیام_scroll = 0;
#ifdef FEAT_GUI
		صحیح save_he = hold_gui_events;
		hold_gui_events = 0;
#endif
		set_must_redraw(UPD_CLEAR);
		pending_exmode_active = TRUE;

		main_loop(FALSE, TRUE);

		pending_exmode_active = FALSE;
		RedrawingDisabled = save_RedrawingDisabled;
		no_wait_return = save_nwr;
		پیام_scroll = save_ms;
#ifdef FEAT_GUI
		hold_gui_events = save_he;
#endif
	    }
	    بازگشت;
	}
    }

    اگر ((eap->cmdidx == CMD_new
		|| eap->cmdidx == CMD_tabnew
		|| eap->cmdidx == CMD_tabedit
		|| eap->cmdidx == CMD_vnew) && *eap->arg == NUL)
    {
	// ":new" or ":tabnew" without argument: edit a new empty buffer
	setpcmark();
	(تهی)do_ecmd(0, پوچ, پوچ, eap, ECMD_ONE,
		      ECMD_HIDE + (eap->forceit ? ECMD_FORCEIT : 0),
		      old_curwin == پوچ ? curwin : پوچ);
    }
    وگرنه اگر ((eap->cmdidx != CMD_split && eap->cmdidx != CMD_vsplit)
	    || *eap->arg != NUL
#ifdef FEAT_BROWSE
	    || (cmdmod.cmod_flags & CMOD_BROWSE)
#endif
	    )
    {
	// Can't edit another file when "textlock" or "curbuf_lock" is set.
	// Only ":edit" or ":script" can bring us here, others are stopped
	// earlier.
	اگر (*eap->arg != NUL && text_or_buf_locked())
	    بازگشت;

	n = readonlymode;
	اگر (eap->cmdidx == CMD_view || eap->cmdidx == CMD_sview)
	    readonlymode = TRUE;
	وگرنه اگر (eap->cmdidx == CMD_enew)
	    readonlymode = FALSE;   // 'readonly' doesn't make sense in an
				    // empty buffer
	اگر (eap->cmdidx != CMD_balt && eap->cmdidx != CMD_badd)
	    setpcmark();
	اگر (do_ecmd(0, (eap->cmdidx == CMD_enew ? پوچ : eap->arg),
		    پوچ, eap,
		    // ":edit" goes to first line if Vi compatible
		    (*eap->arg == NUL && eap->do_ecmd_lnum == 0
				      && vim_strchr(p_cpo, CPO_GOTO1) != پوچ)
					       ? ECMD_ONE : eap->do_ecmd_lnum,
		    (buf_hide(curbuf) ? ECMD_HIDE : 0)
		    + (eap->forceit ? ECMD_FORCEIT : 0)
		      // after a split we can use an existing buffer
		    + (old_curwin != پوچ ? ECMD_OLDBUF : 0)
		    + (eap->cmdidx == CMD_badd ? ECMD_ADDBUF : 0)
		    + (eap->cmdidx == CMD_balt ? ECMD_ALTBUF : 0)
		    , old_curwin == پوچ ? curwin : پوچ) == FAIL)
	{
	    // Editing the file failed.  If the window was split, close it.
	    اگر (old_curwin != پوچ)
	    {
		need_hide = (curbufIsChanged() && curbuf->b_nwindows <= 1);
		اگر (!need_hide || buf_hide(curbuf))
		{
#اگر defined(FEAT_EVAL)
		    cleanup_T   cs;

		    // Reset the error/interrupt/exception state here so that
		    // aborting() returns FALSE when closing a window.
		    enter_cleanup(&cs);
#endif
#ifdef FEAT_GUI
		    need_mouse_correct = TRUE;
#endif
		    win_close(curwin, !need_hide && !buf_hide(curbuf));

#اگر defined(FEAT_EVAL)
		    // Restore the error/interrupt/exception state if not
		    // discarded by a new aborting error, interrupt, or
		    // uncaught exception.
		    leave_cleanup(&cs);
#endif
		}
	    }
	}
	وگرنه اگر (readonlymode && curbuf->b_nwindows == 1)
	{
	    // When editing an already visited buffer, 'readonly' won't be set
	    // but the previous value is kept.  With ":view" and ":sview" we
	    // want the  file to be readonly, except when another window is
	    // editing the same buffer.
	    curbuf->b_p_ro = TRUE;
	}
	readonlymode = n;
    }
    وگرنه
    {
	اگر (eap->do_ecmd_cmd != پوچ)
	    do_cmd_argument(eap->do_ecmd_cmd);
	n = curwin->w_arg_idx_invalid;
	check_arg_idx(curwin);
	اگر (n != curwin->w_arg_idx_invalid)
	    maketitle();
    }

    /*
     * if ":split file" worked, set alternate file name in old window to new
     * file
     */
    اگر (old_curwin != پوچ
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
    ایستا تهی
ex_nogui(exarg_T *eap)
{
    eap->errmsg = _(e_gui_cannot_be_used_not_enabled_at_compile_time);
}
#endif

#اگر defined(FEAT_GUI_MSWIN) && defined(FEAT_MENU) && defined(FEAT_TEAROFF)
    ایستا تهی
ex_tearoff(exarg_T *eap)
{
    gui_make_tearoff(eap->arg);
}
#endif

#اگر (defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_GTK) \
	|| defined(FEAT_TERM_POPUP_MENU)) && defined(FEAT_MENU)
    ایستا تهی
ex_popup(exarg_T *eap)
{
# اگر defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_GTK)
    اگر (gui.in_use)
	gui_make_popup(eap->arg, eap->forceit);
#  ifdef FEAT_TERM_POPUP_MENU
    وگرنه
#  endif
# endif
# ifdef FEAT_TERM_POPUP_MENU
	pum_make_popup(eap->arg, eap->forceit);
# endif
}
#endif

    ایستا تهی
ex_swapname(exarg_T *eap UNUSED)
{
    اگر (curbuf->b_ml.ml_mfp == پوچ || curbuf->b_ml.ml_mfp->mf_fname == پوچ)
	پیام(_("No swap file"));
    وگرنه
	پیام((نویسه *)curbuf->b_ml.ml_mfp->mf_fname);
}

/*
 * ":syncbind" forces all 'scrollbind' windows to have the same relative
 * offset.
 * (1998-11-02 16:21:01  R. Edward Ralston <eralston@computer.org>)
 */
    ایستا تهی
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
    اگر (curwin->w_p_scb)
    {
	topline = curwin->w_topline;
	FOR_ALL_WINDOWS(wp)
	{
	    اگر (wp->w_p_scb && wp->w_buffer)
	    {
		y = wp->w_buffer->b_ml.ml_سطر_شمار - get_scrolloff_مقدار();
		اگر (topline > y)
		    topline = y;
	    }
	}
	اگر (topline < 1)
	    topline = 1;
    }
    وگرنه
    {
	topline = 1;
    }


    /*
     * Set all scrollbind windows to the same topline.
     */
    FOR_ALL_WINDOWS(curwin)
    {
	اگر (curwin->w_p_scb)
	{
	    curbuf = curwin->w_buffer;
	    y = topline - curwin->w_topline;
	    اگر (y > 0)
		scrollup(y, TRUE);
	    وگرنه
		scrolldown(-y, TRUE);
	    curwin->w_scbind_pos = topline;
	    redraw_later(UPD_VALID);
	    cursor_correct();
	    curwin->w_redr_status = true;
	}
    }
    curwin = save_curwin;
    curbuf = save_curbuf;
    اگر (curwin->w_p_scb)
    {
	did_syncbind = TRUE;
	checkpcmark();
	اگر (old_linenr != curwin->w_cursor.lnum)
	{
	    char_u ctrl_o[2];

	    ctrl_o[0] = Ctrl_O;
	    ctrl_o[1] = 0;
	    ins_typebuf(ctrl_o, REMAP_NONE, 0, TRUE, FALSE);
	}
    }
}


    ایستا تهی
ex_read(exarg_T *eap)
{
    صحیح		i;
    صحیح		empty = (curbuf->b_ml.ml_flags & ML_EMPTY);
    linenr_T	lnum;

    اگر (eap->usefilter)			// :r!cmd
    {
	do_bang(1, eap, FALSE, FALSE, TRUE);
	بازگشت;
    }

    اگر (u_save(eap->line2, (linenr_T)(eap->line2 + 1)) == FAIL)
	بازگشت;

#ifdef FEAT_BROWSE
    اگر (cmdmod.cmod_flags & CMOD_BROWSE)
    {
	char_u *browseFile;

	browseFile = do_browse(0, (char_u *)_("Append File"), eap->arg,
		پوچ, پوچ, پوچ, curbuf);
	اگر (browseFile != پوچ)
	{
	    i = readfile(browseFile, پوچ,
		    eap->line2, (linenr_T)0, (linenr_T)MAXLNUM, eap, 0);
	    vim_free(browseFile);
	}
	وگرنه
	    i = OK;
    }
    وگرنه
#endif
	اگر (*eap->arg == NUL)
	{
	    اگر (check_fname() == FAIL)	// check for no file name
		بازگشت;
	    i = readfile(curbuf->b_ffname, curbuf->b_fname,
		    eap->line2, (linenr_T)0, (linenr_T)MAXLNUM, eap, 0);
	}
	وگرنه
	{
	    اگر (vim_strchr(p_cpo, CPO_ALTREAD) != پوچ)
		(تهی)setaltfname(eap->arg, eap->arg, (linenr_T)1);
	    i = readfile(eap->arg, پوچ,
		    eap->line2, (linenr_T)0, (linenr_T)MAXLNUM, eap, 0);

	}
    اگر (i != OK)
    {
#اگر defined(FEAT_EVAL)
	اگر (!aborting())
#endif
	    semsg(_(e_cant_open_file_str), eap->arg);
    }
    وگرنه
    {
	اگر (empty && exmode_active)
	{
	    // Delete the empty line that remains.  Historically ex does
	    // this but vi doesn't.
	    اگر (eap->line2 == 0)
		lnum = curbuf->b_ml.ml_سطر_شمار;
	    وگرنه
		lnum = 1;
	    اگر (*ml_get(lnum) == NUL && u_savedel(lnum, 1L) == OK)
	    {
		ml_delete(lnum);
		اگر (curwin->w_cursor.lnum > 1
			&& curwin->w_cursor.lnum >= lnum)
		    --curwin->w_cursor.lnum;
		deleted_سطرها_mark(lnum, 1L);
	    }
	}
	redraw_curbuf_later(UPD_VALID);
    }
}

ایستا char_u	*prev_dir = پوچ;

#اگر defined(EXITFREE)
    تهی
free_cd_dir(تهی)
{
    VIM_CLEAR(prev_dir);
    VIM_CLEAR(globaldir);
}
#endif

/*
 * Get the previous directory for the given chdir scope.
 */
    ایستا char_u *
get_prevdir(cdscope_T scope)
{
    اگر (scope == CDSCOPE_WINDOW)
	بازگشت curwin->w_prevdir;
    وگرنه اگر (scope == CDSCOPE_TABPAGE)
	بازگشت curtab->tp_prevdir;
    بازگشت prev_dir;
}

/*
 * Deal with the side effects of changing the current directory.
 * When 'scope' is CDSCOPE_TABPAGE then this was after an ":tcd" command.
 * When 'scope' is CDSCOPE_WINDOW then this was after an ":lcd" command.
 */
    تهی
post_chdir(cdscope_T scope)
{
    اگر (scope != CDSCOPE_WINDOW)
	// Clear tab local directory for both :cd and :tcd
	VIM_CLEAR(curtab->tp_localdir);
    VIM_CLEAR(curwin->w_localdir);
    اگر (scope != CDSCOPE_GLOBAL)
    {
	char_u	*pdir = get_prevdir(scope);

	// If still in the global directory, need to remember current
	// directory as the global directory.
	اگر (globaldir == پوچ && pdir != پوچ)
	    globaldir = vim_strsave(pdir);

	// Remember this local directory for the window.
	اگر (mch_dirname(NameBuff, MAXPATHL) == OK)
	{
	    اگر (scope == CDSCOPE_TABPAGE)
		curtab->tp_localdir = vim_strsave(NameBuff);
	    وگرنه
		curwin->w_localdir = vim_strsave(NameBuff);
	}
    }
    وگرنه
    {
	// We are now in the global directory, no need to remember its name.
	VIM_CLEAR(globaldir);
    }

    last_chdir_reason = پوچ;
    shorten_fnames(vim_strchr(p_cpo, CPO_NOSYMLINKS) == پوچ);
}

/*
 * Trigger DirChangedPre for "acmd_fname" with directory "new_dir".
 */
    تهی
trigger_DirChangedPre(char_u *acmd_fname, char_u *new_dir)
{
#ifdef FEAT_EVAL
    dict_T	    *v_event;
    save_v_event_T  save_v_event;

    v_event = get_v_event(&save_v_event);
    (تهی)dict_add_string(v_event, "directory", new_dir);
    dict_set_اقلام_ro(v_event);
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
    صحیح
changedir_func(
	char_u		*new_dir,
	صحیح		forceit,
	cdscope_T	scope)
{
    char_u	*pdir = پوچ;
    صحیح		dir_differs;
    char_u	*acmd_fname = پوچ;
    char_u	**pp;
    char_u	*tofree;

    اگر (new_dir == پوچ || allbuf_locked())
	بازگشت FALSE;

    اگر (vim_strchr(p_cpo, CPO_CHDIR) != پوچ && curbufIsChanged() && !forceit)
    {
	emsg(_(e_cannot_change_directory_buffer_is_modified_add_bang_to_override));
	بازگشت FALSE;
    }

    // ":cd -": Change to previous directory
    اگر (STRCMP(new_dir, "-") == 0)
    {
	pdir = get_prevdir(scope);
	اگر (pdir == پوچ)
	{
	    emsg(_(e_no_previous_directory));
	    بازگشت FALSE;
	}
	new_dir = pdir;
    }

    // Save current directory for next ":cd -"
    اگر (mch_dirname(NameBuff, MAXPATHL) == OK)
	pdir = vim_strsave(NameBuff);
    وگرنه
	pdir = پوچ;

    // For UNIX ":cd" means: go to home directory.
    // On other systems too if 'cdhome' is set.
#اگر defined(UNIX) || defined(VMS)
    اگر (*new_dir == NUL)
#وگرنه
    اگر (*new_dir == NUL && p_cdh)
#endif
    {
	// use NameBuff for home directory name
#ifdef VMS
	char_u	*p;

	p = mch_getenv((char_u *)"SYS$LOGIN");
	اگر (p == پوچ || *p == NUL)	// empty is the same as not set
	    NameBuff[0] = NUL;
	وگرنه
	    vim_strncpy(NameBuff, p, MAXPATHL - 1);
#وگرنه
	expand_env((char_u *)"$HOME", NameBuff, MAXPATHL);
#endif
	new_dir = NameBuff;
    }
    dir_differs = pdir == پوچ
			    || pathcmp((نویسه *)pdir, (نویسه *)new_dir, -1) != 0;
    اگر (dir_differs)
    {
	اگر (scope == CDSCOPE_WINDOW)
	    acmd_fname = (char_u *)"window";
	وگرنه اگر (scope == CDSCOPE_TABPAGE)
	    acmd_fname = (char_u *)"tabpage";
	وگرنه
	    acmd_fname = (char_u *)"global";
	trigger_DirChangedPre(acmd_fname, new_dir);

	اگر (vim_chdir(new_dir))
	{
	    emsg(_(e_command_failed));
	    vim_free(pdir);
	    بازگشت FALSE;
	}
    }

    اگر (scope == CDSCOPE_WINDOW)
	pp = &curwin->w_prevdir;
    وگرنه اگر (scope == CDSCOPE_TABPAGE)
	pp = &curtab->tp_prevdir;
    وگرنه
	pp = &prev_dir;
    tofree = *pp;  // new_dir may use this
    *pp = pdir;

    post_chdir(scope);

    اگر (dir_differs)
	apply_autocmds(EVENT_DIRCHANGED, acmd_fname, new_dir, FALSE, curbuf);
    vim_free(tofree);
    بازگشت TRUE;
}

/*
 * ":cd", ":tcd", ":lcd", ":chdir" ":tchdir" and ":lchdir".
 */
    تهی
ex_cd(exarg_T *eap)
{
    char_u	*new_dir;

    new_dir = eap->arg;
#اگر !defined(UNIX) && !defined(VMS)
    // for non-UNIX ":cd" means: print current directory unless 'cdhome' is set
    اگر (*new_dir == NUL && !p_cdh)
    {
	ex_pwd(پوچ);
	بازگشت;
    }
#endif

    cdscope_T	scope = CDSCOPE_GLOBAL;

    اگر (eap->cmdidx == CMD_lcd || eap->cmdidx == CMD_lchdir)
	scope = CDSCOPE_WINDOW;
    وگرنه اگر (eap->cmdidx == CMD_tcd || eap->cmdidx == CMD_tchdir)
	scope = CDSCOPE_TABPAGE;

    اگر (changedir_func(new_dir, eap->forceit, scope))
    {
	// Echo the new current directory if the command was typed.
	اگر (کلید_Typed || p_verbose >= 5)
	    ex_pwd(eap);
    }
}

/*
 * ":pwd".
 */
    ایستا تهی
ex_pwd(exarg_T *eap UNUSED)
{
    اگر (mch_dirname(NameBuff, MAXPATHL) == OK)
    {
#ifdef BACKSLASH_IN_FILENAME
	slash_adjust(NameBuff);
#endif
	اگر (p_verbose > 0)
	{
	    نویسه *context = "global";

	    اگر (last_chdir_reason != پوچ)
		context = last_chdir_reason;
	    وگرنه اگر (curwin->w_localdir != پوچ)
		context = "window";
	    وگرنه اگر (curtab->tp_localdir != پوچ)
		context = "tabpage";
	    smsg("[%s] %s", context, (نویسه *)NameBuff);
	}
	وگرنه
	    پیام((نویسه *)NameBuff);
    }
    وگرنه
	emsg(_(e_directory_unknown));
}

/*
 * ":=".
 */
    ایستا تهی
ex_equal(exarg_T *eap)
{
    smsg("%ld", (long)eap->line2);
    ex_may_print(eap);
}

    ایستا تهی
ex_sleep(exarg_T *eap)
{
    صحیح		n;
    long	len;

    اگر (cursor_valid())
    {
	n = W_WINROW(curwin) + curwin->w_wrow - پیام_scrolled;
	اگر (n >= 0)
	    windgoto(n, curwin->w_wincol + curwin->w_wcol);
    }

    len = eap->line2;
    انتخاب (*eap->arg)
    {
	حالت 'm': بشکن;
	حالت NUL: len *= 1000L; بشکن;
	پیشفرض: semsg(_(e_invalid_argument_str), eap->arg); بازگشت;
    }

    // Hide the cursor if invoked with !
    do_sleep(len, eap->forceit);
}

/*
 * Sleep for "msec" milliseconds, but keep checking for a CTRL-C every second.
 * Hide the cursor if "hide_cursor" is TRUE.
 */
    تهی
do_sleep(long msec, صحیح hide_cursor)
{
    long	done = 0;
    long	wait_now;
#ifdef ELAPSED_FUNC
    elapsed_T	start_tv;

    // Remember at what time we started, so that we know how much longer we
    // should wait after waiting for a bit.
    ELAPSED_INIT(start_tv);
#endif

    اگر (hide_cursor)
	cursor_sleep();
    وگرنه
	cursor_on();

    out_flush_cursor(FALSE, FALSE);
    وقتی (!got_int && done < msec)
    {
	wait_now = msec - done > 1000L ? 1000L : msec - done;
#ifdef FEAT_TIMERS
	{
	    long    due_time = check_due_timer();

	    اگر (due_time > 0 && due_time < wait_now)
		wait_now = due_time;
	}
#endif
#ifdef FEAT_JOB_CHANNEL
	اگر (has_any_channel() && wait_now > 20L)
	    wait_now = 20L;
#endif
#ifdef FEAT_SOUND
	اگر (has_any_sound_callback() && wait_now > 20L)
	    wait_now = 20L;
#endif
	ui_delay(wait_now, TRUE);

#ifdef FEAT_JOB_CHANNEL
	اگر (has_any_channel())
	    ui_breakcheck_force(TRUE);
	وگرنه
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
#وگرنه
	// guesstimate time passed (will actually be more)
	done += wait_now;
#endif
    }

    // If CTRL-C was typed to interrupt the sleep, drop the CTRL-C from the
    // input buffer, otherwise a following call to input() fails.
    اگر (got_int)
	(تهی)vpeekc();

    اگر (hide_cursor)
	cursor_unsleep();
}

/*
 * ":winsize" command (obsolete).
 */
    ایستا تهی
ex_winsize(exarg_T *eap)
{
    صحیح		w, h;
    char_u	*arg = eap->arg;
    char_u	*p;

    اگر (!SAFE_isdigit(*arg))
    {
	semsg(_(e_invalid_argument_str), arg);
	بازگشت;
    }
    w = getdigits(&arg);
    arg = skipwhite(arg);
    p = arg;
    h = getdigits(&arg);
    اگر (*p != NUL && *arg == NUL)
	set_shellsize(w, h, TRUE);
    وگرنه
	emsg(_(e_winsize_requires_two_عدد_arguments));
}

    ایستا تهی
ex_wincmd(exarg_T *eap)
{
    صحیح		xchar = NUL;
    char_u	*p;

    اگر (*eap->arg == 'g' || *eap->arg == Ctrl_G)
    {
	// CTRL-W g and CTRL-W CTRL-G  have an extra command character
	اگر (eap->arg[1] == NUL)
	{
	    emsg(_(e_invalid_argument));
	    بازگشت;
	}
	xchar = eap->arg[1];
	p = eap->arg + 2;
    }
    وگرنه
	p = eap->arg + 1;

    set_nextcmd(eap, p);
    p = skipwhite(p);
    اگر (*p != NUL && *p != (
#ifdef FEAT_EVAL
	    in_vim9script() ? '#' :
#endif
		'"')
	    && eap->nextcmd == پوچ)
	emsg(_(e_invalid_argument));
    وگرنه اگر (!eap->skip)
    {
	// Pass flags on for ":vertical wincmd ]".
	postponed_split_flags = cmdmod.cmod_split;
	postponed_split_tab = cmdmod.cmod_tab;
	do_window(*eap->arg, eap->addr_شمار > 0 ? eap->line2 : 0L, xchar);
	postponed_split_flags = 0;
	postponed_split_tab = 0;
    }
}

#اگر defined(FEAT_GUI) || defined(UNIX) || defined(VMS) || defined(MSWIN)
/*
 * ":winpos".
 */
    ایستا تهی
ex_winpos(exarg_T *eap)
{
    صحیح		x, y;
    char_u	*arg = eap->arg;
    char_u	*p;

    اگر (*arg == NUL)
    {
# اگر defined(FEAT_GUI) || defined(MSWIN)
#  ifdef VIMDLL
	اگر (gui.in_use ? gui_mch_get_winpos(&x, &y) != FAIL :
		mch_get_winpos(&x, &y) != FAIL)
#  elif defined(FEAT_GUI)
	اگر (gui.in_use && gui_mch_get_winpos(&x, &y) != FAIL)
#  وگرنه
	اگر (mch_get_winpos(&x, &y) != FAIL)
#  endif
	{
	    sprintf((نویسه *)IObuff, _("Window position: X %d, Y %d"), x, y);
	    پیام((نویسه *)IObuff);
	}
	وگرنه
# endif
	    emsg(_(e_obtaining_window_position_not_implemented_for_this_platform));
    }
    وگرنه
    {
	x = getdigits(&arg);
	arg = skipwhite(arg);
	p = arg;
	y = getdigits(&arg);
	اگر (*p == NUL || *arg != NUL)
	{
	    emsg(_(e_winpos_requires_two_عدد_arguments));
	    بازگشت;
	}
# ifdef FEAT_GUI
	اگر (gui.in_use)
	    gui_mch_set_winpos(x, y);
	وگرنه اگر (gui.starting)
	{
	    // Remember the coordinates for when the window is opened.
	    gui_win_x = x;
	    gui_win_y = y;
	}
#  اگر defined(HAVE_TGETENT) || defined(VIMDLL)
	وگرنه
#  endif
# endif
# اگر defined(MSWIN) && (!defined(FEAT_GUI) || defined(VIMDLL))
	    mch_set_winpos(x, y);
# endif
# ifdef HAVE_TGETENT
	اگر (*T_CWP)
	    term_set_winpos(x, y);
# endif
    }
}
#endif

/*
 * Handle command that work like operators: ":delete", ":yank", ":>" and ":<".
 */
    ایستا تهی
ex_operators(exarg_T *eap)
{
    oparg_T	oa;

    clear_oparg(&oa);
    oa.regname = eap->regname;
    oa.start.lnum = eap->line1;
    oa.end.lnum = eap->line2;
    oa.سطر_شمار = eap->line2 - eap->line1 + 1;
    oa.motion_type = MLINE;
    virtual_op = FALSE;
    اگر (eap->cmdidx != CMD_yank)	// position cursor for undo
    {
	setpcmark();
	curwin->w_cursor.lnum = eap->line1;
	beginline(BL_SOL | BL_FIX);
    }

    اگر (VIsual_active)
	end_visual_mode();

    انتخاب (eap->cmdidx)
    {
	حالت CMD_delete:
	    oa.op_type = OP_DELETE;
	    op_delete(&oa);
	    بشکن;

	حالت CMD_yank:
	    oa.op_type = OP_YANK;
	    (تهی)op_yank(&oa, FALSE, TRUE);
	    بشکن;

	پیشفرض:    // CMD_rshift or CMD_lshift
	    اگر (
#ifdef FEAT_RIGHTLEFT
		(eap->cmdidx == CMD_rshift) ^ curwin->w_p_rl
#وگرنه
		eap->cmdidx == CMD_rshift
#endif
						)
		oa.op_type = OP_RSHIFT;
	    وگرنه
		oa.op_type = OP_LSHIFT;
	    op_shift(&oa, FALSE, eap->amount);
	    بشکن;
    }
    virtual_op = MAYBE;
    ex_may_print(eap);
}

/*
 * ":put".
 */
    ایستا تهی
ex_put(exarg_T *eap)
{
    // ":0put" works like ":1put!".
    اگر (eap->line2 == 0)
    {
	eap->line2 = 1;
	eap->forceit = TRUE;
    }
    curwin->w_cursor.lnum = eap->line2;
    check_cursor_col();
    do_put(eap->regname, پوچ, eap->forceit ? BACKWARD : FORWARD, 1L,
						       PUT_سطر|PUT_CURSLINE);
}

/*
 * ":iput".
 */
    ایستا تهی
ex_iput(exarg_T *eap)
{
    // ":0iput" works like ":1iput!".
    اگر (eap->line2 == 0)
    {
	eap->line2 = 1;
	eap->forceit = TRUE;
    }
    curwin->w_cursor.lnum = eap->line2;
    check_cursor_col();
    do_put(eap->regname, پوچ, eap->forceit ? BACKWARD : FORWARD, 1L,
						      PUT_سطر|PUT_CURSLINE
						      |PUT_FIXINDENT);
}

/*
 * Handle ":copy" and ":move".
 */
    ایستا تهی
ex_copymove(exarg_T *eap)
{
    long	n;

#ifdef FEAT_EVAL
    اگر (not_in_vim9(eap, false) == FAIL)
	بازگشت;
#endif
    n = get_address(eap, &eap->arg, eap->addr_type, FALSE, FALSE, FALSE, 1);
    اگر (eap->arg == پوچ)	    // error detected
    {
	eap->nextcmd = پوچ;
	بازگشت;
    }
    get_flags(eap);

    /*
     * move or copy lines from 'eap->line1'-'eap->line2' to below line 'n'
     */
    اگر (n == MAXLNUM || n < 0 || n > curbuf->b_ml.ml_سطر_شمار)
    {
	emsg(_(e_invalid_range));
	بازگشت;
    }

    اگر (eap->cmdidx == CMD_move)
    {
	اگر (do_move(eap->line1, eap->line2, n) == FAIL)
	    بازگشت;
    }
    وگرنه
	ex_copy(eap->line1, eap->line2, n);
    u_clearline();
    beginline(BL_SOL | BL_FIX);
    ex_may_print(eap);
}

/*
 * Print the current line if flags were given to the Ex command.
 */
    تهی
ex_may_print(exarg_T *eap)
{
    اگر (eap->flags != 0)
    {
	print_سطر(curwin->w_cursor.lnum, (eap->flags & EXFLAG_NR),
						  (eap->flags & EXFLAG_LIST));
	ex_no_reprint = TRUE;
    }
}

/*
 * ":smagic" and ":snomagic".
 */
    ایستا تهی
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
    ایستا تهی
ex_join(exarg_T *eap)
{
    curwin->w_cursor.lnum = eap->line1;
    اگر (eap->line1 == eap->line2)
    {
	اگر (eap->addr_شمار >= 2)   // :2,2join does nothing
	    بازگشت;
	اگر (eap->line2 == curbuf->b_ml.ml_سطر_شمار)
	{
	    beep_flush();
	    بازگشت;
	}
	++eap->line2;
    }
    (تهی)do_join(eap->line2 - eap->line1 + 1, !eap->forceit, TRUE, TRUE, TRUE);
    beginline(BL_WHITE | BL_FIX);
    ex_may_print(eap);
}

/*
 * ":[addr]@r" or ":[addr]*r": execute register
 */
    ایستا تهی
ex_at(exarg_T *eap)
{
    صحیح		c;
    صحیح		prev_len = typebuf.tb_len;

    curwin->w_cursor.lnum = eap->line2;
    check_cursor_col();

#ifdef USE_ON_FLY_SCROLL
    dont_scroll = TRUE;		// disallow scrolling here
#endif

    // get the register name.  No name means to use the previous one
    c = *eap->arg;
    اگر (c == NUL || (c == '*' && *eap->cmd == '*'))
	c = '@';
    // Put the register in the typeahead buffer with the "silent" flag.
    اگر (do_execreg(c, TRUE, vim_strchr(p_cpo, CPO_EXECBUF) != پوچ, TRUE)
								      == FAIL)
    {
	beep_flush();
	بازگشت;
    }

    صحیح	save_efr = exec_from_reg;

    exec_from_reg = TRUE;

    /*
     * Execute from the typeahead buffer.
     * Continue until the stuff buffer is empty and all added characters
     * have been consumed.
     */
    وقتی (!stuff_empty() || typebuf.tb_len > prev_len)
	(تهی)do_cmdline(پوچ, getexline, پوچ, DOCMD_NOWAIT|DOCMD_VERBOSE);

    exec_from_reg = save_efr;
}

/*
 * ":!".
 */
    ایستا تهی
ex_bang(exarg_T *eap)
{
    do_bang(eap->addr_شمار, eap, eap->forceit, TRUE, TRUE);
}

/*
 * ":undo".
 */
    ایستا تهی
ex_undo(exarg_T *eap)
{
    اگر (eap->addr_شمار == 1)	    // :undo 123
	undo_time(eap->line2, FALSE, FALSE, TRUE);
    وگرنه
	u_undo(1);
}

#ifdef FEAT_PERSISTENT_UNDO
    ایستا تهی
ex_wundo(exarg_T *eap)
{
    char_u hash[UNDO_HASH_SIZE];

    u_compute_hash(hash);
    u_write_undo(eap->arg, eap->forceit, curbuf, hash);
}

    ایستا تهی
ex_rundo(exarg_T *eap)
{
    char_u hash[UNDO_HASH_SIZE];

    u_compute_hash(hash);
    u_read_undo(eap->arg, hash, پوچ);
}
#endif

/*
 * ":redo".
 */
    ایستا تهی
ex_redo(exarg_T *eap UNUSED)
{
    u_redo(1);
}

/*
 * ":earlier" and ":later".
 */
    ایستا تهی
ex_later(exarg_T *eap)
{
    long	شمار = 0;
    صحیح		sec = FALSE;
    صحیح		file = FALSE;
    char_u	*p = eap->arg;

    اگر (*p == NUL)
	شمار = 1;
    وگرنه اگر (SAFE_isdigit(*p))
    {
	شمار = getdigits(&p);
	انتخاب (*p)
	{
	    حالت 's': ++p; sec = TRUE; بشکن;
	    حالت 'm': ++p; sec = TRUE; شمار *= 60; بشکن;
	    حالت 'h': ++p; sec = TRUE; شمار *= 60 * 60; بشکن;
	    حالت 'd': ++p; sec = TRUE; شمار *= 24 * 60 * 60; بشکن;
	    حالت 'f': ++p; file = TRUE; بشکن;
	}
    }

    اگر (*p != NUL)
	semsg(_(e_invalid_argument_str), eap->arg);
    وگرنه
	undo_time(eap->cmdidx == CMD_earlier ? -شمار : شمار,
							    sec, file, FALSE);
}

/*
 * ":redir": start/stop redirection.
 */
    ایستا تهی
ex_redir(exarg_T *eap)
{
    نویسه	*mode;
    char_u	*fname;
    char_u	*arg = eap->arg;

#ifdef FEAT_EVAL
    اگر (redir_execute)
    {
	emsg(_(e_cannot_use_redir_inside_execute));
	بازگشت;
    }
#endif

    اگر (STRICMP(eap->arg, "END") == 0)
	close_redir();
    وگرنه
    {
	اگر (*arg == '>')
	{
	    ++arg;
	    اگر (*arg == '>')
	    {
		++arg;
		mode = "a";
	    }
	    وگرنه
		mode = "w";
	    arg = skipwhite(arg);

	    close_redir();

	    // Expand environment variables and "~/".
	    fname = expand_env_save(arg);
	    اگر (fname == پوچ)
		بازگشت;
#ifdef FEAT_BROWSE
	    اگر (cmdmod.cmod_flags & CMOD_BROWSE)
	    {
		char_u	*browseFile;

		browseFile = do_browse(BROWSE_SAVE,
			(char_u *)_("Save Redirection"),
			fname, پوچ, پوچ,
			(char_u *)_(BROWSE_FILTER_ALL_FILES), curbuf);
		اگر (browseFile == پوچ)
		    بازگشت;		// operation cancelled
		vim_free(fname);
		fname = browseFile;
		eap->forceit = TRUE;	// since dialog already asked
	    }
#endif

	    redir_fd = open_exfile(fname, eap->forceit, mode);
	    vim_free(fname);
	}
#ifdef FEAT_EVAL
	وگرنه اگر (*arg == '@')
	{
	    // redirect to a register a-z (resp. A-Z for appending)
	    close_redir();
	    ++arg;
	    اگر (ASCII_ISALPHA(*arg)
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
		اگر (*arg == '>' && arg[1] == '>')  // append
		    arg += 2;
		وگرنه
		{
		    // Can use both "@a" and "@a>".
		    اگر (*arg == '>')
			arg++;
		    // Make register empty when not using @A-@Z and the
		    // command is valid.
		    اگر (*arg == NUL && !SAFE_isupper(redir_reg))
			write_reg_contents(redir_reg, (char_u *)"", -1, FALSE);
		}
	    }
	    اگر (*arg != NUL)
	    {
		redir_reg = 0;
		semsg(_(e_invalid_argument_str), eap->arg);
	    }
	}
	وگرنه اگر (*arg == '=' && arg[1] == '>')
	{
	    صحیح append;

	    // redirect to a variable
	    close_redir();
	    arg += 2;

	    اگر (*arg == '>')
	    {
		++arg;
		append = TRUE;
	    }
	    وگرنه
		append = FALSE;

	    اگر (var_redir_start(skipwhite(arg), append) == OK)
		redir_vname = 1;
	}
#endif

	// TODO: redirect to a buffer

	وگرنه
	    semsg(_(e_invalid_argument_str), eap->arg);
    }

    // Make sure redirection is not off.  Can happen for cmdline completion
    // that indirectly invokes a command to catch its output.
    اگر (redir_fd != پوچ
#ifdef FEAT_EVAL
			  || redir_reg || redir_vname
#endif
							)
	redir_off = FALSE;
}

/*
 * ":redraw": force redraw, with clear for ":redraw!".
 */
    تهی
ex_redraw(exarg_T *eap)
{
    redraw_cmd(eap->forceit);
}

/*
 * ":redraw": force redraw, with clear if "clear" is TRUE.
 */
    تهی
redraw_cmd(صحیح clear)
{
    صحیح save_RedrawingDisabled = RedrawingDisabled;
    RedrawingDisabled = 0;

    صحیح save_p_lz = p_lz;
    p_lz = FALSE;

    validate_cursor();
    update_topline();
    update_screen(clear ? UPD_CLEAR : VIsual_active ? UPD_INVERTED : 0);
    اگر ((State & MODE_CMDLINE) == 0)
	setcursor(); // put cursor back where it belongs
    اگر (need_maketitle)
	maketitle();
#اگر defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
# ifdef VIMDLL
    اگر (!gui.in_use)
# endif
	resize_console_buf();
#endif
    RedrawingDisabled = save_RedrawingDisabled;
    p_lz = save_p_lz;

    // After drawing the statusline screen_attr may still be set.
    screen_stop_highlight();

    // Reset msg_didout, so that a message that's there is overwritten.
    پیام_didout = FALSE;
    پیام_col = 0;

    // No need to wait after an intentional redraw.
    need_wait_return = FALSE;

    // When invoked from a callback or autocmd the command line may be active.
    اگر (State & MODE_CMDLINE)
	redrawcmdline();

    out_flush();
}

/*
 * ":redrawstatus": force redraw of status line(s)
 */
    ایستا تهی
ex_redrawstatus(exarg_T *eap UNUSED)
{
    اگر (eap->forceit)
	status_redraw_all();
    وگرنه
	status_redraw_curbuf();
    redraw_vseps = TRUE;
    اگر (پیام_scrolled && (State & MODE_CMDLINE))
	بازگشت;  // redraw later

    صحیح save_RedrawingDisabled = RedrawingDisabled;
    RedrawingDisabled = 0;

    صحیح save_p_lz = p_lz;
    p_lz = FALSE;

    اگر (State & MODE_CMDLINE)
	redraw_statuslines();
    وگرنه
	update_screen(VIsual_active ? UPD_INVERTED : 0);
    RedrawingDisabled = save_RedrawingDisabled;
    p_lz = save_p_lz;
    out_flush();
}

/*
 * ":redrawtabline": force redraw of the tabline
 */
    ایستا تهی
ex_redrawtabline(exarg_T *eap UNUSED)
{
    صحیح save_RedrawingDisabled = RedrawingDisabled;
    RedrawingDisabled = 0;

    صحیح save_p_lz = p_lz;
    p_lz = FALSE;

    draw_tabline();

    RedrawingDisabled = save_RedrawingDisabled;
    p_lz = save_p_lz;
    out_flush();
}

/*
 * ":redrawtabpanel": force redraw of the tabpanel
 */
    ایستا تهی
ex_redrawtabpanel(exarg_T *eap UNUSED)
{
    صحیح save_RedrawingDisabled = RedrawingDisabled;
    RedrawingDisabled = 0;

    صحیح save_p_lz = p_lz;
    p_lz = FALSE;

#اگر defined(FEAT_TABPANEL)
    draw_tabpanel();
#وگرنه
    emsg(_(e_cannot_not_support_redrawtabpanel));
#endif

    RedrawingDisabled = save_RedrawingDisabled;
    p_lz = save_p_lz;
    out_flush();
}

    ایستا تهی
close_redir(تهی)
{
    اگر (redir_fd != پوچ)
    {
	fclose(redir_fd);
	redir_fd = پوچ;
    }
#ifdef FEAT_EVAL
    redir_reg = 0;
    اگر (redir_vname)
    {
	var_redir_stop();
	redir_vname = 0;
    }
#endif
}

#اگر defined(FEAT_SESSION) || defined(FEAT_EVAL)
    صحیح
vim_mkdir_emsg(char_u *name, صحیح prot UNUSED)
{
    اگر (vim_mkdir(name, prot) != 0)
    {
	semsg(_(e_cannot_create_directory_str), name);
	بازگشت FAIL;
    }
    بازگشت OK;
}
#endif

/*
 * Open a file for writing for an Ex command, with some checks.
 * Return file descriptor, or NULL on failure.
 */
    FILE *
open_exfile(
    char_u	*fname,
    صحیح		forceit,
    نویسه	*mode)	    // "w" for create new file or "a" for append
{
    FILE	*fd;

#ifdef UNIX
    // with Unix it is possible to open a directory
    اگر (mch_isdir(fname))
    {
	semsg(_(e_str_is_directory), fname);
	بازگشت پوچ;
    }
#endif
    اگر (!forceit && *mode != 'a' && vim_fexists(fname))
    {
	semsg(_(e_str_exists_add_bang_to_override), fname);
	بازگشت پوچ;
    }

    اگر ((fd = mch_fopen((نویسه *)fname, mode)) == پوچ)
	semsg(_(e_cannot_open_str_for_writing_2), fname);

    بازگشت fd;
}

/*
 * ":mark" and ":k".
 */
    ایستا تهی
ex_mark(exarg_T *eap)
{
    pos_T	pos;

#ifdef FEAT_EVAL
    اگر (not_in_vim9(eap, false) == FAIL)
	بازگشت;
#endif
    اگر (*eap->arg == NUL)		// No argument?
    {
	emsg(_(e_argument_required));
	بازگشت;
    }

    اگر (eap->arg[1] != NUL)	// more than one character?
    {
	semsg(_(e_trailing_characters_str), eap->arg);
	بازگشت;
    }

    pos = curwin->w_cursor;		// save curwin->w_cursor
    curwin->w_cursor.lnum = eap->line2;
    beginline(BL_WHITE | BL_FIX);
    اگر (setmark(*eap->arg) == FAIL)	// set mark
	emsg(_(e_argument_must_be_letter_or_forward_backward_quote));
    curwin->w_cursor = pos;		// restore curwin->w_cursor
}

/*
 * Update w_topline, w_leftcol and the cursor position.
 */
    تهی
update_topline_cursor(تهی)
{
    check_cursor();		// put cursor on valid line
    update_topline();
    اگر (!curwin->w_p_wrap)
	validate_cursor();
    update_curswant();
}

/*
 * Save the current State and go to Normal mode.
 * Return TRUE if the typeahead could be saved.
 */
    صحیح
save_current_state(save_state_T *sst)
{
    sst->save_پیام_scroll = پیام_scroll;
    sst->save_restart_edit = restart_edit;
    sst->save_پیام_didout = پیام_didout;
    sst->save_State = State;
    sst->save_insertmode = p_im;
    sst->save_finish_op = finish_op;
    sst->save_opcount = opcount;
    sst->save_reg_executing = reg_executing;
    sst->save_pending_end_reg_executing = pending_end_reg_executing;

    پیام_scroll = FALSE;		    // no msg scrolling in Normal mode
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
    بازگشت sst->tabuf.typebuf_valid;
}

    تهی
restore_current_state(save_state_T *sst)
{
    // Restore the previous typeahead.
    restore_typeahead(&sst->tabuf, FALSE);

    پیام_scroll = sst->save_پیام_scroll;
    restart_edit = sst->save_restart_edit;
    p_im = sst->save_insertmode;
    finish_op = sst->save_finish_op;
    opcount = sst->save_opcount;
    reg_executing = sst->save_reg_executing;
    pending_end_reg_executing = sst->save_pending_end_reg_executing;
    پیام_didout |= sst->save_پیام_didout;	// don't reset msg_didout now
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
    تهی
ex_normal(exarg_T *eap)
{
    save_state_T save_state;
    char_u	*arg = پوچ;
    صحیح		l;
    char_u	*p;

    اگر (ex_normal_lock > 0)
    {
	emsg(_(e_not_allowed_here));
	بازگشت;
    }
    اگر (ex_normal_busy >= p_mmd)
    {
	emsg(_(e_recursive_use_of_normal_too_deep));
	بازگشت;
    }

    /*
     * vgetc() expects a CSI and K_SPECIAL to have been escaped.  Don't do
     * this for the K_SPECIAL leading byte, otherwise special keys will not
     * work.
     */
    اگر (has_mbyte)
    {
	صحیح	len = 0;

	// Count the number of characters to be escaped.
	برای (p = eap->arg; *p != NUL; ++p)
	{
#ifdef FEAT_GUI
	    اگر (*p == CSI)  // leadbyte CSI
		len += 2;
#endif
	    برای (l = (*mb_ptr2len)(p) - 1; l > 0; --l)
		اگر (*++p == K_SPECIAL	  // trailbyte K_SPECIAL or CSI
#ifdef FEAT_GUI
			|| *p == CSI
#endif
			)
		    len += 2;
	}
	اگر (len > 0)
	{
	    arg = alloc(STRLEN(eap->arg) + len + 1);
	    اگر (arg != پوچ)
	    {
		len = 0;
		برای (p = eap->arg; *p != NUL; ++p)
		{
		    arg[len++] = *p;
#ifdef FEAT_GUI
		    اگر (*p == CSI)
		    {
			arg[len++] = KS_EXTRA;
			arg[len++] = (صحیح)KE_CSI;
		    }
#endif
		    برای (l = (*mb_ptr2len)(p) - 1; l > 0; --l)
		    {
			arg[len++] = *++p;
			اگر (*p == K_SPECIAL)
			{
			    arg[len++] = KS_SPECIAL;
			    arg[len++] = KE_FILLER;
			}
#ifdef FEAT_GUI
			وگرنه اگر (*p == CSI)
			{
			    arg[len++] = KS_EXTRA;
			    arg[len++] = (صحیح)KE_CSI;
			}
#endif
		    }
		    arg[len] = NUL;
		}
	    }
	}
    }

    ++ex_normal_busy;
    اگر (save_current_state(&save_state))
    {
	/*
	 * Repeat the :normal command for each line in the range.  When no
	 * range given, execute it just once, without positioning the cursor
	 * first.
	 */
	انجامبده
	{
	    اگر (eap->addr_شمار != 0)
	    {
		curwin->w_cursor.lnum = eap->line1++;
		curwin->w_cursor.col = 0;
		check_cursor_moved(curwin);
	    }

	    exec_normal_cmd(arg != پوچ
		     ? arg
		     : eap->arg, eap->forceit ? REMAP_NONE : REMAP_YES, FALSE);
	}
	وقتی (eap->addr_شمار > 0 && eap->line1 <= eap->line2 && !got_int);
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
    ایستا تهی
ex_startinsert(exarg_T *eap)
{
#ifdef FEAT_TERMINAL
    // Ignore this when running in an active terminal.
    اگر (term_job_running(curbuf->b_term))
	بازگشت;
#endif
    اگر (!curbuf->b_p_ma && !p_im)
    {
	// Only give this error when 'insertmode' is off.
	emsg(_(e_cannot_make_changes_modifiable_is_off));
	بازگشت;
    }
    اگر (eap->forceit)
    {
	// cursor line can be zero on startup
	اگر (!curwin->w_cursor.lnum)
	    curwin->w_cursor.lnum = 1;
	set_cursor_for_append_to_سطر();
    }

    // Ignore the command when already in Insert mode.  Inserting an
    // expression register that invokes a function can do this.
    اگر (State & MODE_درج)
	بازگشت;

    اگر (eap->cmdidx == CMD_startinsert)
	restart_edit = 'a';
    وگرنه اگر (eap->cmdidx == CMD_startreplace)
	restart_edit = 'R';
    وگرنه
	restart_edit = 'V';

    اگر (!eap->forceit)
    {
	اگر (eap->cmdidx == CMD_startinsert)
	    restart_edit = 'i';
	curwin->w_curswant = 0;	    // avoid MAXCOL
    }

    اگر (VIsual_active)
	showmode();
}

/*
 * ":stopinsert"
 */
    ایستا تهی
ex_stopinsert(exarg_T *eap UNUSED)
{
    restart_edit = 0;
    stop_درج_mode = TRUE;
#اگر defined(FEAT_CLIENTSERVER) || defined(FEAT_EVAL)
    // when called from remote_expr in insert mode, make sure insert mode is
    // ended by adding K_NOP to the typeahead buffer
    اگر (vgetc_busy)
	ins_char_typebuf(K_NOP, 0);
#endif
    clearmode();
}

/*
 * Execute normal mode command "cmd".
 * "remap" can be REMAP_NONE or REMAP_YES.
 */
    تهی
exec_normal_cmd(char_u *cmd, صحیح remap, صحیح silent)
{
    // Stuff the argument into the typeahead buffer.
    ins_typebuf(cmd, remap, 0, TRUE, silent);
    exec_normal(FALSE, FALSE, FALSE);
}

/*
 * Execute normal_cmd() until there is no typeahead left.
 * When "use_vpeekc" is TRUE use vpeekc() to check for available chars.
 */
    تهی
exec_normal(صحیح was_typed, صحیح use_vpeekc, صحیح may_use_terminal_loop UNUSED)
{
    oparg_T	oa;
    صحیح		c;

    // When calling vpeekc() from feedkeys() it will return Ctrl_C when there
    // is nothing to get, so also check for Ctrl_C.
    clear_oparg(&oa);
    finish_op = FALSE;
    وقتی ((!stuff_empty()
		|| ((was_typed || !typebuf_typed()) && typebuf.tb_len > 0)
		|| (use_vpeekc && (c = vpeekc()) != NUL && c != Ctrl_C))
	    && !got_int)
    {
	update_topline_cursor();
#ifdef FEAT_TERMINAL
	اگر (may_use_terminal_loop && term_use_loop()
		&& oa.op_type == OP_NOP && oa.regname == NUL
		&& !VIsual_active)
	{
	    // If terminal_loop() returns OK we got a key that is handled
	    // in Normal model.  With FAIL we first need to position the
	    // cursor and the screen needs to be redrawn.
	    اگر (terminal_loop(TRUE) == OK)
		normal_cmd(&oa, TRUE);
	}
	وگرنه
#endif
	    // execute a Normal mode cmd
	    normal_cmd(&oa, TRUE);
    }
}

#ifdef FEAT_FIND_ID
    ایستا تهی
ex_checkpath(exarg_T *eap)
{
    find_pattern_in_path(پوچ, 0, 0, FALSE, FALSE, CHECK_PATH, 1L,
	    eap->forceit ? ACTION_SHOW_ALL : ACTION_SHOW,
	    (linenr_T)1, (linenr_T)MAXLNUM, eap->forceit, FALSE);
}

# اگر defined(FEAT_QUICKFIX)
/*
 * ":psearch"
 */
    ایستا تهی
ex_psearch(exarg_T *eap)
{
    g_do_tagpreview = p_pvh;
    ex_findpat(eap);
    g_do_tagpreview = 0;
}
# endif

    ایستا تهی
ex_findpat(exarg_T *eap)
{
    صحیح		whole = TRUE;
    long	n;
    char_u	*p;
    صحیح		action;

    انتخاب (cmdnames[eap->cmdidx].cmd_name[2])
    {
	حالت 'e':	// ":psearch", ":isearch" and ":dsearch"
		اگر (cmdnames[eap->cmdidx].cmd_name[0] == 'p')
		    action = ACTION_GOTO;
		وگرنه
		    action = ACTION_SHOW;
		بشکن;
	حالت 'i':	// ":ilist" and ":dlist"
		action = ACTION_SHOW_ALL;
		بشکن;
	حالت 'u':	// ":ijump" and ":djump"
		action = ACTION_GOTO;
		بشکن;
	پیشفرض:	// ":isplit" and ":dsplit"
		action = ACTION_SPLIT;
		بشکن;
    }

    n = 1;
    اگر (vim_isdigit(*eap->arg))	// get count
    {
	n = getdigits(&eap->arg);
	eap->arg = skipwhite(eap->arg);
    }
    اگر (*eap->arg == '/')   // Match regexp, not just whole words
    {
	whole = FALSE;
	++eap->arg;
	p = skip_regexp(eap->arg, '/', magic_isset());
	اگر (*p)
	{
	    *p++ = NUL;
	    p = skipwhite(p);

	    // Check for trailing illegal characters
	    اگر (!ends_excmd2(eap->arg, p))
		eap->errmsg = ex_errmsg(e_trailing_characters_str, p);
	    وگرنه
		set_nextcmd(eap, p);
	}
    }
    اگر (!eap->skip)
	find_pattern_in_path(eap->arg, 0, (صحیح)STRLEN(eap->arg),
		whole, !eap->forceit,
		*eap->cmd == 'd' ? FIND_DEFINE : FIND_ANY, n, action,
		eap->line1, eap->line2, eap->forceit, FALSE);
}
#endif


#ifdef FEAT_QUICKFIX
/*
 * ":ptag", ":ptselect", ":ptjump", ":ptnext", etc.
 */
    ایستا تهی
ex_ptag(exarg_T *eap)
{
    g_do_tagpreview = p_pvh;  // will be reset to 0 in ex_tag_cmd()
    ex_tag_cmd(eap, cmdnames[eap->cmdidx].cmd_name + 1);
}

/*
 * ":pedit"
 */
    ایستا تهی
ex_pedit(exarg_T *eap)
{
    win_T	*curwin_save = curwin;
    prepare_preview_window();

    // Edit the file.
    do_exedit(eap, پوچ);

    back_to_current_window(curwin_save);
}

/*
 * ":pbuffer"
 */
    ایستا تهی
ex_pbuffer(exarg_T *eap)
{
    win_T	*curwin_save = curwin;
    prepare_preview_window();

    // Go to the buffer.
    do_exbuffer(eap);

    back_to_current_window(curwin_save);
}

    ایستا تهی
prepare_preview_window(تهی)
{
    اگر (ERROR_IF_ANY_POPUP_WINDOW)
	بازگشت;

    // Open the preview window or popup and make it the current window.
    g_do_tagpreview = p_pvh;
    prepare_tagpreview(TRUE, TRUE, FALSE);
}

    ایستا تهی
back_to_current_window(win_T *curwin_save)
{
    اگر (curwin != curwin_save && win_valid(curwin_save))
    {
	// Return cursor to where we were
	validate_cursor();
	redraw_later(UPD_VALID);
	win_enter(curwin_save, TRUE);
    }
# ifdef FEAT_PROP_POPUP
    وگرنه اگر (WIN_IS_POPUP(curwin))
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
    ایستا تهی
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
    ایستا تهی
ex_tag(exarg_T *eap)
{
    ex_tag_cmd(eap, cmdnames[eap->cmdidx].cmd_name);
}

    ایستا تهی
ex_tag_cmd(exarg_T *eap, char_u *name)
{
    صحیح		cmd;

    انتخاب (name[1])
    {
	حالت 'j': cmd = DT_JUMP;	// ":tjump"
		  بشکن;
	حالت 's': cmd = DT_SELECT;	// ":tselect"
		  بشکن;
	حالت 'p': cmd = DT_PREV;	// ":tprevious"
		  بشکن;
	حالت 'N': cmd = DT_PREV;	// ":tNext"
		  بشکن;
	حالت 'n': cmd = DT_NEXT;	// ":tnext"
		  بشکن;
	حالت 'o': cmd = DT_برداشت;		// ":pop"
		  بشکن;
	حالت 'f':			// ":tfirst"
	حالت 'r': cmd = DT_FIRST;	// ":trewind"
		  بشکن;
	حالت 'l': cmd = DT_LAST;	// ":tlast"
		  بشکن;
	پیشفرض:			// ":tag"
#ifdef FEAT_CSCOPE
		  اگر (p_cst && *eap->arg != NUL)
		  {
		      ex_cstag(eap);
		      بازگشت;
		  }
#endif
		  cmd = DT_TAG;
		  بشکن;
    }

    اگر (name[0] == 'l')
    {
#ifndef FEAT_QUICKFIX
	ex_ni(eap);
	بازگشت;
#وگرنه
	cmd = DT_LTAG;
#endif
    }

    do_tag(eap->arg, cmd, eap->addr_شمار > 0 ? (صحیح)eap->line2 : 1,
							  eap->forceit, TRUE);
}

شمارش {
    SPEC_PERC = 0,
    SPEC_HASH,
    SPEC_CWORD,	    // cursor word
    SPEC_CCWORD,    // cursor WORD
    SPEC_CEXPR,	    // expr under cursor
    SPEC_CFILE,	    // cursor path name
    SPEC_SFILE,	    // ":so" file name
    SPEC_SLNUM,	    // ":so" file line number
    SPEC_پشته,	    // call stack
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
    صحیح
find_cmdline_var(char_u *src, size_t *usedlen)
{
    // must be sorted by the 'value' field because it is used by bsearch()!
    ایستا keyvalue_T spec_str_tab[] = {
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
	KEYVALUE_ENTRY(SPEC_پشته, "stack>")	    // call stack
    };
    keyvalue_T target;
    keyvalue_T *entry;

    انتخاب (*src)
    {
    حالت '%':
	*usedlen = 1;
	بازگشت SPEC_PERC;

    حالت '#':
	*usedlen = 1;
	بازگشت SPEC_HASH;

    حالت '<':
	target.کلید = 0;
	target.مقدار.string = src + 1;	    // skip over '<'
	target.مقدار.length = 0;	    // not used, see cmp_keyvalue_value_n()

	entry = (keyvalue_T *)bsearch(&target, &spec_str_tab,
		ARRAY_LENGTH(spec_str_tab), اندازه(spec_str_tab[0]),
		cmp_keyvalue_مقدار_n);
	اگر (entry == پوچ)
	    بازگشت -1;

	*usedlen = entry->مقدار.length + 1;
	بازگشت entry->کلید;

    پیشفرض:
	بشکن;
    }

    بازگشت -1;
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
    نویسه	**errormsg,	// pointer to error message
    صحیح		*escaped,	// return value has escaped white space (can
				// be NULL)
    صحیح		empty_is_error)	// empty result is considered an error
{
    صحیح		i;
    char_u	*s;
    char_u	*result = (char_u *)"";
    char_u	*resultbuf = پوچ;
    size_t	resultlen;
    buf_T	*buf;
    صحیح		valid = VALID_HEAD + VALID_PATH;    // assume valid result
    صحیح		spec_idx;
    صحیح		tilde_file = FALSE;
    صحیح		skip_mod = FALSE;
    char_u	strbuf[30];

    *errormsg = پوچ;
    اگر (escaped != پوچ)
	*escaped = FALSE;

    /*
     * Check if there is something to do.
     */
    spec_idx = find_cmdline_var(src, usedlen);
    اگر (spec_idx < 0)	// no match
    {
	*usedlen = 1;
	بازگشت پوچ;
    }

    /*
     * Skip when preceded with a backslash "\%" and "\#".
     * Note: In "\\%" the % is also not recognized!
     */
    اگر (src > srcstart && src[-1] == '\\')
    {
	*usedlen = 0;
	STRMOVE(src - 1, src);	// remove backslash
	بازگشت پوچ;
    }

    /*
     * word or WORD under cursor
     */
    اگر (spec_idx == SPEC_CWORD || spec_idx == SPEC_CCWORD
						     || spec_idx == SPEC_CEXPR)
    {
	resultlen = find_ident_under_cursor(&result,
		spec_idx == SPEC_CWORD ? (FIND_IDENT | FIND_STRING)
	      : spec_idx == SPEC_CEXPR ? (FIND_IDENT | FIND_STRING | FIND_EVAL)
	      : FIND_STRING);
	اگر (resultlen == 0)
	{
	    *errormsg = "";
	    بازگشت پوچ;
	}
    }

    /*
     * '#': Alternate file name
     * '%': Current file name
     *	    File name under the cursor
     *	    File name for autocommand
     *	and following modifiers
     */
    وگرنه
    {
	size_t off = 0;

	انتخاب (spec_idx)
	{
	حالت SPEC_PERC:
#ifdef FEAT_EVAL
		اگر (!in_vim9script() || src[1] != '%')
#endif
		{
		    // '%': current file
		    اگر (curbuf->b_fname == پوچ)
		    {
			result = (char_u *)"";
			valid = 0;	    // Must have ":p:h" to be valid
		    }
		    وگرنه
		    {
			result = curbuf->b_fname;
			tilde_file = STRCMP(result, "~") == 0;
		    }
		    بشکن;
		}
#ifdef FEAT_EVAL
		// "%%" alternate file
		off = 1;
#endif
		// FALLTHROUGH
	حالت SPEC_HASH:		// '#' or "#99": alternate file
		اگر (off == 0 ? src[1] == '#' : src[2] == '%')
		{
		    // "##" or "%%%": the argument list
		    result = arg_all();
		    resultbuf = result;
		    *usedlen = off + 2;
		    اگر (escaped != پوچ)
			*escaped = TRUE;
		    skip_mod = TRUE;
		    بشکن;
		}
		s = src + off + 1;
		اگر (*s == '<')		// "#<99" uses v:oldfiles
		    ++s;
		i = (صحیح)getdigits(&s);
		اگر (s == src + off + 2 && src[off + 1] == '-')
		    // just a minus sign, don't skip over it
		    s--;
		*usedlen = (صحیح)(s - src); // length of what we expand

		اگر (src[off + 1] == '<' && i != 0)
		{
		    اگر (*usedlen < off + 2)
		    {
			// Should we give an error message for #<text?
			*usedlen = off + 1;
			بازگشت پوچ;
		    }
#ifdef FEAT_EVAL
		    result = list_find_str(get_vim_var_list(VV_OLDFILES),
								     (long)i);
		    اگر (result == پوچ)
		    {
			*errormsg = "";
			بازگشت پوچ;
		    }
#وگرنه
		    *errormsg = _(e_hashsmall_is_not_available_without_the_eval_feature);
		    بازگشت پوچ;
#endif
		}
		وگرنه
		{
		    اگر (i == 0 && src[off + 1] == '<' && *usedlen > off + 1)
			*usedlen = off + 1;
		    buf = buflist_findnr(i);
		    اگر (buf == پوچ)
		    {
			*errormsg = _(e_no_alternate_file_name_to_substitute_for_hash);
			بازگشت پوچ;
		    }
		    اگر (lnump != پوچ)
			*lnump = ECMD_LAST;
		    اگر (buf->b_fname == پوچ)
		    {
			result = (char_u *)"";
			valid = 0;	    // Must have ":p:h" to be valid
		    }
		    وگرنه
		    {
			result = buf->b_fname;
			tilde_file = STRCMP(result, "~") == 0;
		    }
		}
		بشکن;

	حالت SPEC_CFILE:	// file name under cursor
		result = file_name_at_cursor(FNAME_MESS|FNAME_HYP, 1L, پوچ);
		اگر (result == پوچ)
		{
		    *errormsg = "";
		    بازگشت پوچ;
		}
		resultbuf = result;	    // remember allocated string
		بشکن;

	حالت SPEC_AFILE:	// file name for autocommand
		result = autocmd_fname;
		اگر (result != پوچ && !autocmd_fname_full)
		{
		    // Still need to turn the fname into a full path.  It is
		    // postponed to avoid a delay when <afile> is not used.
		    autocmd_fname_full = TRUE;
		    result = FullName_save(autocmd_fname, FALSE);
		    vim_free(autocmd_fname);
		    autocmd_fname = result;
		}
		اگر (result == پوچ)
		{
		    *errormsg = _(e_no_autocommand_file_name_to_substitute_for_afile);
		    بازگشت پوچ;
		}
		result = shorten_fname1(result);
		بشکن;

	حالت SPEC_ABUF:		// buffer number for autocommand
		اگر (autocmd_bufnr <= 0)
		{
		    *errormsg = _(e_no_autocommand_buffer_عدد_to_substitute_for_abuf);
		    بازگشت پوچ;
		}
		sprintf((نویسه *)strbuf, "%d", autocmd_bufnr);
		result = strbuf;
		بشکن;

	حالت SPEC_AMATCH:	// match name for autocommand
		result = autocmd_match;
		اگر (result == پوچ)
		{
		    *errormsg = _(e_no_autocommand_match_name_to_substitute_for_amatch);
		    بازگشت پوچ;
		}
		بشکن;

	حالت SPEC_SFILE:	// file name for ":so" command
		result = estack_sfile(ESTACK_SFILE);
		اگر (result == پوچ)
		{
		    *errormsg = _(e_no_source_file_name_to_substitute_for_sfile);
		    بازگشت پوچ;
		}
		resultbuf = result;	    // remember allocated string
		بشکن;
	حالت SPEC_پشته:	// call stack
		result = estack_sfile(ESTACK_پشته);
		اگر (result == پوچ)
		{
		    *errormsg = _(e_no_call_پشته_to_substitute_for_پشته);
		    بازگشت پوچ;
		}
		resultbuf = result;	    // remember allocated string
		بشکن;
	حالت SPEC_SCRIPT:	// script file name
		result = estack_sfile(ESTACK_SCRIPT);
		اگر (result == پوچ)
		{
		    *errormsg = _(e_no_script_file_name_to_substitute_for_script);
		    بازگشت پوچ;
		}
		resultbuf = result;	    // remember allocated string
		بشکن;

	حالت SPEC_SLNUM:	// line in file for ":so" command
		اگر (SOURCING_NAME == پوچ || SOURCING_LNUM == 0)
		{
		    *errormsg = _(e_no_سطر_عدد_to_use_for_slnum);
		    بازگشت پوچ;
		}
		sprintf((نویسه *)strbuf, "%ld", SOURCING_LNUM);
		result = strbuf;
		بشکن;

#ifdef FEAT_EVAL
	حالت SPEC_SFLNUM:	// line in script file
		اگر (current_sctx.sc_lnum + SOURCING_LNUM == 0)
		{
		    *errormsg = _(e_no_سطر_عدد_to_use_for_sflnum);
		    بازگشت پوچ;
		}
		sprintf((نویسه *)strbuf, "%ld",
				 (long)(current_sctx.sc_lnum + SOURCING_LNUM));
		result = strbuf;
		بشکن;

	حالت SPEC_SID:
		اگر (current_sctx.sc_sid <= 0)
		{
		    *errormsg = _(e_using_sid_not_in_script_context);
		    بازگشت پوچ;
		}
		sprintf((نویسه *)strbuf, "<SNR>%d_", current_sctx.sc_sid);
		result = strbuf;
		بشکن;
#endif

#ifdef FEAT_CLIENTSERVER
	حالت SPEC_CLIENT:	// Source of last submitted input
# ifdef MSWIN
		sprintf((نویسه *)strbuf, PRINTF_HEX_LONG_U,
							(long_u)clientWindow);
		result = strbuf;
# وگرنه
#  ifdef FEAT_SOCKETSERVER
		اگر (clientserver_method == CLIENTSERVER_METHOD_SOCKET)
		{
		    اگر (client_socket == پوچ)
			result = (char_u *)"";
		    وگرنه
			result = client_socket;
		}
#  endif
#  ifdef FEAT_X11
		اگر (clientserver_method == CLIENTSERVER_METHOD_X11)
		{
		    sprintf((نویسه *)strbuf, PRINTF_HEX_LONG_U,
							(long_u)clientWindow);
		    result = strbuf;
		}
#  endif
# endif
		بشکن;
#endif
	}

	resultlen = STRLEN(result);	// length of new string
	اگر (src[*usedlen] == '<')	// remove the file name extension
	{
	    ++*usedlen;
	    اگر ((s = vim_strrchr(result, '.')) != پوچ && s >= gettail(result))
		resultlen = s - result;
	}
	وگرنه اگر (!skip_mod)
	{
	    valid |= modify_fname(src, tilde_file, usedlen, &result, &resultbuf,
								  &resultlen);
	    اگر (result == پوچ)
	    {
		*errormsg = "";
		بازگشت پوچ;
	    }
	}
    }

    اگر (resultlen == 0 || valid != VALID_HEAD + VALID_PATH)
    {
	اگر (empty_is_error)
	{
	    اگر (valid != VALID_HEAD + VALID_PATH)
		*errormsg = _(e_empty_file_name_for_percent_or_hash_only_works_with_ph);
	    وگرنه
		*errormsg = _(e_evaluates_to_an_empty_string);
	}
	result = پوچ;
    }
    وگرنه
	result = vim_strnsave(result, resultlen);
    vim_free(resultbuf);
    بازگشت result;
}

/*
 * Expand the <sfile> string in "arg".
 *
 * Returns an allocated string, or NULL for any error.
 */
    char_u *
expand_sfile(char_u *arg)
{
    نویسه	*errormsg;
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
    اگر (result == پوچ)
	بازگشت پوچ;

    برای (p = result; *p; )
    {
	اگر (STRNCMP(p, "<sfile>", 7) != 0)
	    ++p;
	وگرنه
	{
	    // replace "<sfile>" with the sourced file name, and do ":" stuff
	    repl = eval_vars(p, result, &srclen, پوچ, &errormsg, پوچ, TRUE);
	    اگر (errormsg != پوچ)
	    {
		اگر (*errormsg)
		    emsg(errormsg);
		vim_free(result);
		بازگشت پوچ;
	    }
	    اگر (repl == پوچ)		// no match (cannot happen)
	    {
		p += srclen;
		ادامه;
	    }
	    repllen = STRLEN(repl);
	    resultlen += repllen - srclen;
	    newres = alloc(resultlen + 1);
	    اگر (newres == پوچ)
	    {
		vim_free(repl);
		vim_free(result);
		بازگشت پوچ;
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

    بازگشت result;
}

#اگر defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
/*
 * Make a dialog message in "buff[DIALOG_MSG_SIZE]".
 * "format" must contain "%s".
 */
    تهی
dialog_پیام(char_u *buff, نویسه *format, char_u *fname)
{
    اگر (fname == پوچ)
	fname = (char_u *)_("Untitled");
    vim_snprintf((نویسه *)buff, DIALOG_پیام_SIZE, format, fname);
}
#endif

/*
 * ":behave {mswin,xterm}"
 */
    ایستا تهی
ex_behave(exarg_T *eap)
{
    اگر (STRCMP(eap->arg, "mswin") == 0)
    {
	set_option_مقدار_give_err((char_u *)"selection",
						 0L, (char_u *)"exclusive", 0);
	set_option_مقدار_give_err((char_u *)"selectmode",
						 0L, (char_u *)"mouse,key", 0);
	set_option_مقدار_give_err((char_u *)"mousemodel",
						     0L, (char_u *)"popup", 0);
	set_option_مقدار_give_err((char_u *)"keymodel",
					  0L, (char_u *)"startsel,stopsel", 0);
    }
    وگرنه اگر (STRCMP(eap->arg, "xterm") == 0)
    {
	set_option_مقدار_give_err((char_u *)"selection",
						 0L, (char_u *)"inclusive", 0);
	set_option_مقدار_give_err((char_u *)"selectmode", 0L, (char_u *)"", 0);
	set_option_مقدار_give_err((char_u *)"mousemodel",
						    0L, (char_u *)"extend", 0);
	set_option_مقدار_give_err((char_u *)"keymodel", 0L, (char_u *)"", 0);
    }
    وگرنه
	semsg(_(e_invalid_argument_str), eap->arg);
}

ایستا صحیح filetype_detect = FALSE;
ایستا صحیح filetype_plugin = FALSE;
ایستا صحیح filetype_indent = FALSE;

/*
 * ":filetype [plugin] [indent] {on,off,detect}"
 * on: Load the filetype.vim file to install autocommands for file types.
 * off: Load the ftoff.vim file to remove all autocommands for file types.
 * plugin on: load filetype.vim and ftplugin.vim
 * plugin off: load ftplugof.vim
 * indent on: load filetype.vim and indent.vim
 * indent off: load indoff.vim
 */
    ایستا تهی
ex_filetype(exarg_T *eap)
{
    char_u	*arg = eap->arg;
    صحیح		plugin = FALSE;
    صحیح		indent = FALSE;

    اگر (*eap->arg == NUL)
    {
	// Print current status.
	smsg("filetype detection:%s  plugin:%s  indent:%s",
		filetype_detect ? "ON" : "OFF",
		filetype_plugin ? (filetype_detect ? "ON" : "(on)") : "OFF",
		filetype_indent ? (filetype_detect ? "ON" : "(on)") : "OFF");
	بازگشت;
    }

    // Accept "plugin" and "indent" in any order.
    برای (;;)
    {
	اگر (STRNCMP(arg, "plugin", 6) == 0)
	{
	    plugin = TRUE;
	    arg = skipwhite(arg + 6);
	    ادامه;
	}
	اگر (STRNCMP(arg, "indent", 6) == 0)
	{
	    indent = TRUE;
	    arg = skipwhite(arg + 6);
	    ادامه;
	}
	بشکن;
    }
    اگر (STRCMP(arg, "on") == 0 || STRCMP(arg, "detect") == 0)
    {
	اگر (*arg == 'o' || !filetype_detect)
	{
	    source_runtime((char_u *)FILETYPE_FILE, DIP_ALL);
	    filetype_detect = TRUE;
	    اگر (plugin)
	    {
		source_runtime((char_u *)FTPLUGIN_FILE, DIP_ALL);
		filetype_plugin = TRUE;
	    }
	    اگر (indent)
	    {
		source_runtime((char_u *)INDENT_FILE, DIP_ALL);
		filetype_indent = TRUE;
	    }
	}
	اگر (*arg == 'd')
	{
	    (تهی)do_doautocmd((char_u *)"filetypedetect BufRead", TRUE, پوچ);
	    do_modelines(0);
	}
    }
    وگرنه اگر (STRCMP(arg, "off") == 0)
    {
	اگر (plugin || indent)
	{
	    اگر (plugin)
	    {
		source_runtime((char_u *)FTPLUGOF_FILE, DIP_ALL);
		filetype_plugin = FALSE;
	    }
	    اگر (indent)
	    {
		source_runtime((char_u *)INDOFF_FILE, DIP_ALL);
		filetype_indent = FALSE;
	    }
	}
	وگرنه
	{
	    source_runtime((char_u *)FTOFF_FILE, DIP_ALL);
	    filetype_detect = FALSE;
	}
    }
    وگرنه
	semsg(_(e_invalid_argument_str), arg);
}

/*
 * ":setfiletype [FALLBACK] {name}"
 */
    ایستا تهی
ex_setfiletype(exarg_T *eap)
{
    اگر (curbuf->b_did_filetype)
	بازگشت;

    char_u *arg = eap->arg;
    اگر (STRNCMP(arg, "FALLBACK ", 9) == 0)
	arg += 9;

    set_option_مقدار_give_err((char_u *)"filetype", 0L, arg, OPT_LOCAL);
    اگر (arg != eap->arg)
	curbuf->b_did_filetype = false;
}

    ایستا تهی
ex_digraphs(exarg_T *eap UNUSED)
{
#ifdef FEAT_DIGRAPHS
    اگر (*eap->arg != NUL)
	putdigraph(eap->arg);
    وگرنه
	listdigraphs(eap->forceit);
#وگرنه
    emsg(_(e_no_digraphs_version));
#endif
}

#اگر defined(FEAT_جستجو_EXTRA)
    تهی
set_no_hlsearch(صحیح flag)
{
    no_hlsearch = flag;
# ifdef FEAT_EVAL
    set_vim_var_nr(VV_HLSEARCH, !no_hlsearch && p_hls);
# endif
}

/*
 * ":nohlsearch"
 */
    ایستا تهی
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
    ایستا تهی
ex_X(exarg_T *eap UNUSED)
{
    crypt_check_current_method();
    (تهی)crypt_get_کلید(TRUE, TRUE);
}
#endif

#ifdef FEAT_FOLDING
    ایستا تهی
ex_fold(exarg_T *eap)
{
    اگر (foldManualAllowed(TRUE))
	foldCreate(eap->line1, eap->line2);
}

    ایستا تهی
ex_foldopen(exarg_T *eap)
{
    opFoldRange(eap->line1, eap->line2, eap->cmdidx == CMD_foldopen,
							 eap->forceit, FALSE);
}

    ایستا تهی
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
    برای (lnum = eap->line1; lnum <= eap->line2; ++lnum)
	اگر (hasFolding(lnum, پوچ, پوچ) == (eap->cmdidx == CMD_folddoclosed))
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

#اگر defined(FEAT_QUICKFIX)
/*
 * Returns TRUE if the supplied Ex cmdidx is for a location list command
 * instead of a quickfix command.
 */
    صحیح
is_loclist_cmd(صحیح cmdidx)
{
    اگر (cmdidx < 0 || cmdidx >= CMD_SIZE)
	بازگشت FALSE;
    بازگشت cmdnames[cmdidx].cmd_name[0] == 'l';
}
#endif

    صحیح
get_pressedreturn(تهی)
{
    بازگشت ex_pressedreturn;
}

    تهی
set_pressedreturn(صحیح val)
{
    ex_pressedreturn = val;
}
