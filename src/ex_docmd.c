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

#ضمن "vim.h"

ساكن صحيح	quitmore = 0;
ساكن صحيح	ex_pressedreturn = FALSE;
#ifndef FEAT_PRINTER
# define ex_hardcopy	ex_ni
#endif

#ifdef FEAT_EVAL
ساكن char_u	*do_one_cmd(char_u **, صحيح, cstack_T *, char_u *(*fgetline)(صحيح, فراغ *, صحيح, getline_opt_T), فراغ *cookie);
#وإلا
ساكن char_u	*do_one_cmd(char_u **, صحيح, char_u *(*fgetline)(صحيح, فراغ *, صحيح, getline_opt_T), فراغ *cookie);
ساكن صحيح	if_level = 0;		// depth in :if
#endif
ساكن فراغ	append_command(char_u *cmd);

#ifndef FEAT_MENU
# define ex_emenu		ex_ni
# define ex_menu		ex_ni
# define ex_menutranslate	ex_ni
#endif
ساكن فراغ	ex_autocmd(exarg_T *eap);
ساكن فراغ	ex_doautocmd(exarg_T *eap);
ساكن فراغ	ex_bunload(exarg_T *eap);
ساكن فراغ	ex_buffer(exarg_T *eap);
ساكن فراغ	do_exbuffer(exarg_T *eap);
ساكن فراغ	ex_bmodified(exarg_T *eap);
ساكن فراغ	ex_bnext(exarg_T *eap);
ساكن فراغ	ex_bprevious(exarg_T *eap);
ساكن فراغ	ex_brewind(exarg_T *eap);
ساكن فراغ	ex_blast(exarg_T *eap);
ساكن char_u	*getargcmd(char_u **);
ساكن صحيح	getargopt(exarg_T *eap);
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
#إذا !defined(FEAT_QUICKFIX) || !defined(FEAT_EVAL)
# define ex_cexpr		ex_ni
#endif

ساكن linenr_T default_address(exarg_T *eap);
ساكن فراغ address_default_all(exarg_T *eap);
ساكن فراغ	get_flags(exarg_T *eap);
#إذا !defined(FEAT_PERL) \
	|| !defined(FEAT_PYTHON) || !defined(FEAT_PYTHON3) \
	|| !defined(FEAT_TCL) \
	|| !defined(FEAT_RUBY) \
	|| !defined(FEAT_LUA) \
	|| !defined(FEAT_MZSCHEME)
# define HAVE_EX_SCRIPT_NI
ساكن فراغ	ex_script_ni(exarg_T *eap);
#endif
ساكن حرف	*invalid_range(exarg_T *eap);
ساكن فراغ	correct_range(exarg_T *eap);
#ifdef FEAT_QUICKFIX
ساكن char_u	*replace_makeprg(exarg_T *eap, char_u *p, char_u **cmdlinep);
#endif
ساكن char_u	*repl_cmdline(exarg_T *eap, char_u *src, size_t srclen, char_u *repl, char_u **cmdlinep);
ساكن فراغ	ex_highlight(exarg_T *eap);
ساكن فراغ	ex_colorscheme(exarg_T *eap);
ساكن فراغ	ex_cquit(exarg_T *eap);
ساكن فراغ	ex_quit_all(exarg_T *eap);
ساكن فراغ	ex_close(exarg_T *eap);
ساكن فراغ	ex_win_close(صحيح forceit, win_T *win, tabpage_T *tp);
ساكن فراغ	ex_only(exarg_T *eap);
ساكن فراغ	ex_resize(exarg_T *eap);
ساكن فراغ	ex_stag(exarg_T *eap);
ساكن فراغ	ex_tabclose(exarg_T *eap);
ساكن فراغ	ex_tabonly(exarg_T *eap);
ساكن فراغ	ex_tabnext(exarg_T *eap);
ساكن فراغ	ex_tabmove(exarg_T *eap);
ساكن فراغ	ex_tabs(exarg_T *eap);
#إذا defined(FEAT_QUICKFIX)
ساكن فراغ	ex_pclose(exarg_T *eap);
ساكن فراغ	ex_ptag(exarg_T *eap);
ساكن فراغ	ex_pedit(exarg_T *eap);
ساكن فراغ	ex_pbuffer(exarg_T *eap);
ساكن فراغ	prepare_preview_window(فراغ);
ساكن فراغ	back_to_current_window(win_T *curwin_save);
#وإلا
# define ex_pclose		ex_ni
# define ex_ptag		ex_ni
# define ex_pedit		ex_ni
# define ex_pbuffer		ex_ni
#endif
ساكن فراغ	ex_hide(exarg_T *eap);
ساكن فراغ	ex_exit(exarg_T *eap);
ساكن فراغ	ex_print(exarg_T *eap);
#ifdef FEAT_BYTEOFF
ساكن فراغ	ex_goto(exarg_T *eap);
#وإلا
# define ex_goto		ex_ni
#endif
ساكن فراغ	ex_shell(exarg_T *eap);
ساكن فراغ	ex_preserve(exarg_T *eap);
ساكن فراغ	ex_recover(exarg_T *eap);
ساكن فراغ	ex_mode(exarg_T *eap);
ساكن فراغ	ex_wrongmodifier(exarg_T *eap);
ساكن فراغ	ex_find(exarg_T *eap);
ساكن فراغ	ex_open(exarg_T *eap);
ساكن فراغ	ex_edit(exarg_T *eap);
#ifndef FEAT_GUI
# define ex_gui			ex_nogui
ساكن فراغ	ex_nogui(exarg_T *eap);
#endif
#إذا defined(FEAT_GUI_MSWIN) && defined(FEAT_MENU) && defined(FEAT_TEAROFF)
ساكن فراغ	ex_tearoff(exarg_T *eap);
#وإلا
# define ex_tearoff		ex_ni
#endif
#إذا (defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_GTK) \
	|| defined(FEAT_TERM_POPUP_MENU)) && defined(FEAT_MENU)
ساكن فراغ	ex_popup(exarg_T *eap);
#وإلا
# define ex_popup		ex_ni
#endif
#ifndef FEAT_GUI_MSWIN
# define ex_simalt		ex_ni
#endif
#إذا !defined(FEAT_GUI_MSWIN) && !defined(FEAT_GUI_GTK) && !defined(FEAT_GUI_MOTIF)
# define gui_mch_find_dialog	ex_ni
# define gui_mch_replace_dialog ex_ni
#endif
#إذا !defined(FEAT_GUI_GTK)
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
#إذا !defined(FEAT_SYN_HL) || !defined(FEAT_PROFILE)
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
#إذا !defined(FEAT_PYTHON) && !defined(FEAT_PYTHON3)
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
ساكن فراغ	ex_swapname(exarg_T *eap);
ساكن فراغ	ex_syncbind(exarg_T *eap);
ساكن فراغ	ex_read(exarg_T *eap);
ساكن فراغ	ex_pwd(exarg_T *eap);
ساكن فراغ	ex_equal(exarg_T *eap);
ساكن فراغ	ex_sleep(exarg_T *eap);
ساكن فراغ	ex_winsize(exarg_T *eap);
ساكن فراغ	ex_wincmd(exarg_T *eap);
#إذا defined(FEAT_GUI) || defined(UNIX) || defined(VMS) || defined(MSWIN)
ساكن فراغ	ex_winpos(exarg_T *eap);
#وإلا
# define ex_winpos	    ex_ni
#endif
ساكن فراغ	ex_operators(exarg_T *eap);
ساكن فراغ	ex_put(exarg_T *eap);
ساكن فراغ	ex_iput(exarg_T *eap);
ساكن فراغ	ex_copymove(exarg_T *eap);
ساكن فراغ	ex_submagic(exarg_T *eap);
ساكن فراغ	ex_join(exarg_T *eap);
ساكن فراغ	ex_at(exarg_T *eap);
ساكن فراغ	ex_bang(exarg_T *eap);
ساكن فراغ	ex_undo(exarg_T *eap);
#ifdef FEAT_PERSISTENT_UNDO
ساكن فراغ	ex_wundo(exarg_T *eap);
ساكن فراغ	ex_rundo(exarg_T *eap);
#endif
ساكن فراغ	ex_redo(exarg_T *eap);
ساكن فراغ	ex_later(exarg_T *eap);
ساكن فراغ	ex_redir(exarg_T *eap);
ساكن فراغ	ex_redrawstatus(exarg_T *eap);
ساكن فراغ	ex_redrawtabline(exarg_T *eap);
ساكن فراغ	ex_redrawtabpanel(exarg_T *eap);
ساكن فراغ	close_redir(فراغ);
ساكن فراغ	ex_mark(exarg_T *eap);
ساكن فراغ	ex_startinsert(exarg_T *eap);
ساكن فراغ	ex_stopinsert(exarg_T *eap);
#ifdef FEAT_FIND_ID
ساكن فراغ	ex_checkpath(exarg_T *eap);
ساكن فراغ	ex_findpat(exarg_T *eap);
#وإلا
# define ex_findpat		ex_ni
# define ex_checkpath		ex_ni
#endif
#إذا defined(FEAT_FIND_ID) && defined(FEAT_QUICKFIX)
ساكن فراغ	ex_psearch(exarg_T *eap);
#وإلا
# define ex_psearch		ex_ni
#endif
ساكن فراغ	ex_tag(exarg_T *eap);
ساكن فراغ	ex_tag_cmd(exarg_T *eap, char_u *name);
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
ساكن فراغ	ex_behave(exarg_T *eap);
ساكن فراغ	ex_filetype(exarg_T *eap);
ساكن فراغ	ex_setfiletype(exarg_T *eap);
#ifndef FEAT_DIFF
# define ex_diffoff		ex_ni
# define ex_diffpatch		ex_ni
# define ex_diffgetput		ex_ni
# define ex_diffsplit		ex_ni
# define ex_diffthis		ex_ni
# define ex_diffupdate		ex_ni
#endif
ساكن فراغ	ex_digraphs(exarg_T *eap);
#ifdef FEAT_تلاش_EXTRA
ساكن فراغ	ex_nohlsearch(exarg_T *eap);
#وإلا
# define ex_nohlsearch		ex_ni
# define ex_match		ex_ni
#endif
#ifdef FEAT_CRYPT
ساكن فراغ	ex_X(exarg_T *eap);
#وإلا
# define ex_X			ex_ni
#endif
#ifdef FEAT_FOLDING
ساكن فراغ	ex_fold(exarg_T *eap);
ساكن فراغ	ex_foldopen(exarg_T *eap);
ساكن فراغ	ex_folddo(exarg_T *eap);
#وإلا
# define ex_fold		ex_ni
# define ex_foldopen		ex_ni
# define ex_folddo		ex_ni
#endif
#إذا !(defined(HAVE_LOCALE_H) || defined(X_LOCALE))
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
#إذا !defined(FEAT_X11) || !defined(FEAT_XCLIPBOARD)
# define ex_xrestore		ex_ni
#endif
#إذا !defined(FEAT_WAYLAND)
# define ex_wlrestore		ex_ni
#endif
#إذا !defined(HAVE_CLIPMETHOD)
# define ex_clipreset		ex_ni
#endif
#إذا !defined(FEAT_PROP_POPUP)
# define ex_popupclear		ex_ni
#endif

/*
 * Declare cmdnames[].
 */
#define DO_DECLARE_EXCMD
#ضمن "ex_cmds.h"
#ضمن "ex_cmdidxs.h"

ساكن char_u dollar_command[2] = {'$', 0};


#ifdef FEAT_EVAL
// Struct for storing a line inside a while/for loop
تعريفنوع بنية
{
    char_u	*سطر;		// command line
    linenr_T	lnum;		// sourcing_lnum of the line
} wcmd_T;

/*
 * Structure used to store info for line position in a while or for loop.
 * This is required, because do_one_cmd() may invoke ex_function(), which
 * reads more lines that may come from the while/for loop.
 */
بنية loop_cookie
{
    garray_T	*سطریں_gap;		// growarray with line info
    صحيح		current_سطر;		// last read line from growarray
    صحيح		repeating;		// TRUE when looping a second time
    // When "repeating" is FALSE use "getline" and "cookie" to get lines
    char_u	*(*lc_getline)(صحيح, فراغ *, صحيح, getline_opt_T);
    فراغ	*cookie;
};

ساكن char_u	*get_loop_سطر(صحيح c, فراغ *cookie, صحيح indent, getline_opt_T options);
ساكن صحيح	store_loop_سطر(garray_T *gap, char_u *سطر);
ساكن فراغ	free_cmdlines(garray_T *gap);

// Struct to save a few things while debugging.  Used in do_cmdline() only.
بنية dbg_stuff
{
    صحيح		trylevel;
    صحيح		force_abort;
    except_T	*caught_ڈھیر;
    char_u	*vv_exception;
    char_u	*vv_throwpoint;
    صحيح		did_emsg;
    صحيح		got_int;
    صحيح		did_throw;
    صحيح		need_rethrow;
    صحيح		check_cstack;
    except_T	*current_exception;
};

    ساكن فراغ
save_dbg_stuff(بنية dbg_stuff *dsp)
{
    dsp->trylevel	= trylevel;		trylevel = 0;
    dsp->force_abort	= force_abort;		force_abort = FALSE;
    dsp->caught_ڈھیر	= caught_ڈھیر;		caught_ڈھیر = عدم;
    dsp->vv_exception	= v_exception(عدم);
    dsp->vv_throwpoint	= v_throwpoint(عدم);

    // Necessary for debugging an inactive ":catch", ":finally", ":endtry"
    dsp->did_emsg	= did_emsg;		did_emsg     = FALSE;
    dsp->got_int	= got_int;		got_int	     = FALSE;
    dsp->did_throw	= did_throw;		did_throw    = FALSE;
    dsp->need_rethrow	= need_rethrow;		need_rethrow = FALSE;
    dsp->check_cstack	= check_cstack;		check_cstack = FALSE;
    dsp->current_exception = current_exception;	current_exception = عدم;
}

    ساكن فراغ
restore_dbg_stuff(بنية dbg_stuff *dsp)
{
    suppress_errthrow = FALSE;
    trylevel = dsp->trylevel;
    force_abort = dsp->force_abort;
    caught_ڈھیر = dsp->caught_ڈھیر;
    (فراغ)v_exception(dsp->vv_exception);
    (فراغ)v_throwpoint(dsp->vv_throwpoint);
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
    ساكن صحيح
is_other_file(صحيح fnum, char_u *ffname)
{
    إذا (fnum != 0)
    {
	إذا (fnum == curbuf->b_fnum)
	    أرجع FALSE;

	أرجع TRUE;
    }

    إذا (ffname == عدم)
	أرجع TRUE;

    إذا (*ffname == NUL)
	أرجع FALSE;

    // TODO: Need a reliable way to know whether a buffer is meant to live
    // on-disk !curbuf->b_dev_valid is not always available (example: missing
    // on Windows)
    إذا (curbuf->b_sfname != عدم
	    && *curbuf->b_sfname != NUL)
	// This occurs with unsaved buffers. In which case `ffname` actually
	// corresponds to curbuf->b_sfname
	أرجع fnamecmp(ffname, curbuf->b_sfname) != 0;

    أرجع otherfile(ffname);
}

/*
 * do_exmode(): Repeatedly get commands for the "Ex" mode, until the ":vi"
 * command is given.
 */
    فراغ
do_exmode(
    صحيح		improved)	    // TRUE for "improved Ex" mode
{
    صحيح		save_پیغام_scroll;
    صحيح		prev_پیغام_row;
    linenr_T	prev_سطر;
    varnumber_T	changedtick;

    إذا (improved)
	exmode_active = EXMODE_VIM;
    وإلا
	exmode_active = EXMODE_NORMAL;
    State = MODE_NORMAL;
    may_trigger_modechanged();

    // When using ":global /pat/ visual" and then "Q" we return to continue
    // the :global command.
    إذا (global_busy)
	أرجع;

    save_پیغام_scroll = پیغام_scroll;
    ++RedrawingDisabled;	    // don't redisplay the window
    ++no_wait_return;		    // don't wait for return
#ifdef FEAT_GUI
    // Ignore scrollbar and mouse events in Ex mode
    ++hold_gui_events;
#endif

    پیغام(_("Entering Ex mode.  Type \"visual\" to go to Normal mode."));
    طالما (exmode_active)
    {
	// Check for a ":normal" command and no more characters left.
	إذا (ex_normal_busy > 0 && typebuf.tb_len == 0)
	{
	    exmode_active = FALSE;
	    اكسر;
	}
	پیغام_scroll = TRUE;
	need_wait_return = FALSE;
	ex_pressedreturn = FALSE;
	ex_no_reprint = FALSE;
	changedtick = CHANGEDTICK(curbuf);
	prev_پیغام_row = پیغام_row;
	prev_سطر = curwin->w_cursor.lnum;
	إذا (improved)
	{
	    cmdline_row = پیغام_row;
	    do_cmdline(عدم, getexline, عدم, 0);
	}
	وإلا
	    do_cmdline(عدم, getexmodeline, عدم, DOCMD_NOWAIT);
	سطریں_بایاں = Rows - 1;

	إذا ((prev_سطر != curwin->w_cursor.lnum
		   || changedtick != CHANGEDTICK(curbuf)) && !ex_no_reprint)
	{
	    إذا (curbuf->b_ml.ml_flags & ML_EMPTY)
		emsg(_(e_empty_buffer));
	    وإلا
	    {
		إذا (ex_pressedreturn)
		{
		    // go up one line, to overwrite the ":<CR>" line, so the
		    // output doesn't contain empty lines.
		    پیغام_row = prev_پیغام_row;
		    إذا (prev_پیغام_row == Rows - 1)
			پیغام_row--;
		}
		پیغام_col = 0;
		print_سطر_no_prefix(curwin->w_cursor.lnum, FALSE, FALSE);
		پیغام_clr_eos();
	    }
	}
	وإلا إذا (ex_pressedreturn && !ex_no_reprint)	// must be at EOF
	{
	    إذا (curbuf->b_ml.ml_flags & ML_EMPTY)
		emsg(_(e_empty_buffer));
	    وإلا
		emsg(_(e_at_end_of_file));
	}
    }

#ifdef FEAT_GUI
    --hold_gui_events;
#endif
    إذا (RedrawingDisabled > 0)
	--RedrawingDisabled;
    --no_wait_return;
    update_screen(UPD_CLEAR);
    need_wait_return = FALSE;
    پیغام_scroll = save_پیغام_scroll;
}

/*
 * Print the executed command for when 'verbose' is set.
 * When "lnum" is 0 only print the command.
 */
    ساكن فراغ
پیغام_verbose_cmd(linenr_T lnum, char_u *cmd)
{
    ++no_wait_return;
    verbose_enter_scroll();

    إذا (lnum == 0)
	smsg(_("Executing: %s"), cmd);
    وإلا
	smsg(_("line %ld: %s"), (long)lnum, cmd);
    إذا (پیغام_silent == 0)
	پیغام_puts("\n");   // don't overwrite this

    verbose_leave_scroll();
    --no_wait_return;
}

/*
 * Execute a simple command line.  Used for translated commands like "*".
 */
    صحيح
do_cmdline_cmd(char_u *cmd)
{
    أرجع do_cmdline(cmd, عدم, عدم,
				   DOCMD_VERBOSE|DOCMD_NOWAIT|DOCMD_KEYTYPED);
}

/*
 * Execute the "+cmd" argument of "edit +cmd fname" and the like.
 * This allows for using a range without ":" in Vim9 script.
 */
    ساكن صحيح
do_cmd_argument(char_u *cmd)
{
    أرجع do_cmdline(cmd, عدم, عدم,
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
    صحيح
do_cmdline(
    char_u	*cmdline,
    char_u	*(*fgetline)(صحيح, فراغ *, صحيح, getline_opt_T),
    فراغ	*cookie,		// argument for fgetline()
    صحيح		flags)
{
    char_u	*next_cmdline;		// next cmd to execute
    char_u	*cmdline_copy = عدم;	// copy of cmd line
    صحيح		used_getline = FALSE;	// used "fgetline" to obtain command
    ساكن صحيح	recursive = 0;		// recursive depth
    صحيح		پیغام_didout_before_start = 0;
    صحيح		گنتی = 0;		// line number count
    صحيح		did_inc_RedrawingDisabled = FALSE;
    صحيح		retval = OK;
#ifdef FEAT_EVAL
    cstack_T	cstack;			// conditional stack
    garray_T	سطریں_ga;		// keep lines for ":while"/":for"
    صحيح		current_سطر = 0;	// active line in lines_ga
    صحيح		current_سطر_before = 0;
    char_u	*fname = عدم;		// function or script name
    linenr_T	*breakpoint = عدم;	// ptr to breakpoint field in cookie
    صحيح		*dbg_tick = عدم;	// ptr to dbg_tick field in cookie
    بنية dbg_stuff debug_saved;	// saved things for debug mode
    صحيح		initial_trylevel;
    msglist_T	**saved_پیغام_list = عدم;
    msglist_T	*private_پیغام_list = عدم;

    // "fgetline" and "cookie" passed to do_one_cmd()
    char_u	*(*cmd_getline)(صحيح, فراغ *, صحيح, getline_opt_T);
    فراغ	*cmd_cookie;
    بنية loop_cookie cmd_loop_cookie;
    فراغ	*real_cookie;
    صحيح		getline_is_func;
#وإلا
# define cmd_getline fgetline
# define cmd_cookie cookie
#endif
    ساكن صحيح	call_depth = 0;		// recursiveness
#ifdef FEAT_EVAL
    // For every pair of do_cmdline()/do_one_cmd() calls, use an extra memory
    // location for storing error messages to be converted to an exception.
    // This ensures that the do_errthrow() call in do_one_cmd() does not
    // combine the messages stored by an earlier invocation of do_one_cmd()
    // with the command name of the later one.  This would happen when
    // BufWritePost autocommands are executed after a write error.
    saved_پیغام_list = پیغام_list;
    پیغام_list = &private_پیغام_list;
#endif

    // It's possible to create an endless loop with ":execute", catch that
    // here.  The value of 200 allows nested function calls, ":source", etc.
    // Allow 200 or 'maxfuncdepth', whatever is larger.
    إذا (call_depth >= 200
#ifdef FEAT_EVAL
	    && call_depth >= p_mfd
#endif
	    )
    {
	emsg(_(e_command_too_recursive));
#ifdef FEAT_EVAL
	// When converting to an exception, we do not include the command name
	// since this is not an error of the specific command.
	do_errthrow((cstack_T *)عدم, (char_u *)عدم);
	پیغام_list = saved_پیغام_list;
#endif
	أرجع FAIL;
    }
    ++call_depth;

#ifdef FEAT_EVAL
    CLEAR_FIELD(cstack);
    cstack.cs_idx = -1;
    ga_init2(&سطریں_ga, حجم(wcmd_T), 10);

    real_cookie = getline_cookie(fgetline, cookie);

    // Inside a function use a higher nesting level.
    getline_is_func = getline_equal(fgetline, cookie, get_func_سطر);
    إذا (getline_is_func && ex_nesting_level == func_level(real_cookie))
	++ex_nesting_level;

    // Get the function or script name and the address where the next breakpoint
    // line and the debug tick for a function or script are stored.
    إذا (getline_is_func)
    {
	fname = func_name(real_cookie);
	breakpoint = func_breakpoint(real_cookie);
	dbg_tick = func_dbg_tick(real_cookie);
    }
    وإلا إذا (getline_equal(fgetline, cookie, getsourceline))
    {
	fname = SOURCING_NAME;
	breakpoint = source_breakpoint(real_cookie);
	dbg_tick = source_dbg_tick(real_cookie);
    }

    /*
     * Initialize "force_abort"  and "suppress_errthrow" at the top level.
     */
    إذا (!recursive)
    {
	force_abort = FALSE;
	suppress_errthrow = FALSE;
    }

    /*
     * If requested, store and reset the global values controlling the
     * exception handling (used when debugging).  Otherwise clear it to avoid
     * a bogus compiler warning when the optimizer uses inline functions...
     */
    إذا (flags & DOCMD_EXCRESET)
	save_dbg_stuff(&debug_saved);
    وإلا
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
    إذا (!(flags & DOCMD_KEYTYPED)
			       && !getline_equal(fgetline, cookie, getexline))
	کلید_Typed = FALSE;

    /*
     * Continue executing command lines:
     * - when inside an ":if", ":while" or ":for"
     * - for multiple commands on one line, separated with '|'
     * - when repeating until there are no more lines (for ":source")
     */
    next_cmdline = cmdline;
    افعل
    {
#ifdef FEAT_EVAL
	getline_is_func = getline_equal(fgetline, cookie, get_func_سطر);
#endif

	// stop skipping cmds for an error msg after all endif/while/for
	إذا (next_cmdline == عدم
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
	إذا (cstack.cs_looplevel > 0 && current_سطر < سطریں_ga.ga_len)
	{
	    // Each '|' separated command is stored separately in lines_ga, to
	    // be able to jump to it.  Don't use next_cmdline now.
	    VIM_CLEAR(cmdline_copy);

	    // Check if a function has returned or, unless it has an unclosed
	    // try conditional, aborted.
	    إذا (getline_is_func)
	    {
# ifdef FEAT_PROFILE
		إذا (do_profiling == PROF_YES)
		    func_سطر_end(real_cookie);
# endif
		إذا (func_has_ended(real_cookie))
		{
		    retval = FAIL;
		    اكسر;
		}
	    }
# ifdef FEAT_PROFILE
	    وإلا إذا (do_profiling == PROF_YES
			    && getline_equal(fgetline, cookie, getsourceline))
		script_سطر_end();
# endif

	    // Check if a sourced file hit a ":finish" command.
	    إذا (source_finished(fgetline, cookie))
	    {
		retval = FAIL;
		اكسر;
	    }

	    // If breakpoints have been added/deleted need to check for it.
	    إذا (breakpoint != عدم && dbg_tick != عدم
						   && *dbg_tick != debug_tick)
	    {
		*breakpoint = dbg_find_breakpoint(
				getline_equal(fgetline, cookie, getsourceline),
							fname, SOURCING_LNUM);
		*dbg_tick = debug_tick;
	    }

	    next_cmdline = ((wcmd_T *)(سطریں_ga.ga_data))[current_سطر].سطر;
	    SOURCING_LNUM = ((wcmd_T *)(سطریں_ga.ga_data))[current_سطر].lnum;

	    // Did we encounter a breakpoint?
	    إذا (breakpoint != عدم && *breakpoint != 0
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
	    إذا (do_profiling == PROF_YES)
	    {
		إذا (getline_is_func)
		    func_سطر_start(real_cookie, SOURCING_LNUM);
		وإلا إذا (getline_equal(fgetline, cookie, getsourceline))
		    script_سطر_start();
	    }
# endif
	}
#endif

	// 2. If no line given, get an allocated line with fgetline().
	إذا (next_cmdline == عدم)
	{
	    /*
	     * Need to set msg_didout for the first line after an ":if",
	     * otherwise the ":if" will be overwritten.
	     */
	    إذا (گنتی == 1 && getline_equal(fgetline, cookie, getexline))
		پیغام_didout = TRUE;
	    إذا (fgetline == عدم || (next_cmdline = fgetline(':', cookie,
#ifdef FEAT_EVAL
		    cstack.cs_idx < 0 ? 0 : (cstack.cs_idx + 1) * 2
#وإلا
		    0
#endif
		    , in_vim9script() ? GETLINE_CONCAT_CONTBAR
					       : GETLINE_CONCAT_CONT)) == عدم)
	    {
		// Don't call wait_return() for aborted command line.  The NULL
		// returned for the end of a sourced file or executed function
		// doesn't do this.
		إذا (کلید_Typed && !(flags & DOCMD_REPEAT))
		    need_wait_return = FALSE;
		retval = FAIL;
		اكسر;
	    }
	    used_getline = TRUE;

	    /*
	     * Keep the first typed line.  Clear it when more lines are typed.
	     */
	    إذا (flags & DOCMD_KEEPLINE)
	    {
		vim_free(repeat_cmdline);
		إذا (گنتی == 0)
		    repeat_cmdline = vim_strsave(next_cmdline);
		وإلا
		    repeat_cmdline = عدم;
	    }
	}

	// 3. Make a copy of the command so we can mess with it.
	وإلا إذا (cmdline_copy == عدم)
	{
	    next_cmdline = vim_strsave(next_cmdline);
	    إذا (next_cmdline == عدم)
	    {
		emsg(_(e_out_of_memory));
		retval = FAIL;
		اكسر;
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
	إذا ((cstack.cs_looplevel > 0 || has_loop_cmd(next_cmdline)))
	{
	    cmd_getline = get_loop_سطر;
	    cmd_cookie = (فراغ *)&cmd_loop_cookie;
	    cmd_loop_cookie.سطریں_gap = &سطریں_ga;
	    cmd_loop_cookie.current_سطر = current_سطر;
	    cmd_loop_cookie.lc_getline = fgetline;
	    cmd_loop_cookie.cookie = cookie;
	    cmd_loop_cookie.repeating = (current_سطر < سطریں_ga.ga_len);

	    // Save the current line when encountering it the first time.
	    إذا (current_سطر == سطریں_ga.ga_len
		    && store_loop_سطر(&سطریں_ga, next_cmdline) == FAIL)
	    {
		retval = FAIL;
		اكسر;
	    }
	    current_سطر_before = current_سطر;
	}
	وإلا
	{
	    cmd_getline = fgetline;
	    cmd_cookie = cookie;
	}

	did_endif = FALSE;
#endif

	إذا (گنتی++ == 0)
	{
	    /*
	     * All output from the commands is put below each other, without
	     * waiting for a return. Don't do this when executing commands
	     * from a script or when being called recursive (e.g. for ":e
	     * +command file").
	     */
	    إذا (!(flags & DOCMD_NOWAIT) && !recursive)
	    {
		پیغام_didout_before_start = پیغام_didout;
		پیغام_didany = FALSE; // no output yet
		پیغام_start();
		پیغام_scroll = TRUE;  // put messages below each other
		++no_wait_return;   // don't wait for return until finished
		++RedrawingDisabled;
		did_inc_RedrawingDisabled = TRUE;
	    }
	}

	إذا ((p_verbose >= 15 && SOURCING_NAME != عدم) || p_verbose >= 16)
	    پیغام_verbose_cmd(SOURCING_LNUM, cmdline_copy);

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
	إذا (cmd_cookie == (فراغ *)&cmd_loop_cookie)
	    // Use "current_line" from "cmd_loop_cookie", it may have been
	    // incremented when defining a function.
	    current_سطر = cmd_loop_cookie.current_سطر;
#endif

	إذا (next_cmdline == عدم)
	{
	    VIM_CLEAR(cmdline_copy);

	    /*
	     * If the command was typed, remember it for the ':' register.
	     * Do this AFTER executing the command to make :@: work.
	     */
	    إذا (getline_equal(fgetline, cookie, getexline)
						  && new_last_cmdline != عدم)
	    {
		vim_free(last_cmdline);
		last_cmdline = new_last_cmdline;
		new_last_cmdline = عدم;
	    }
	}
	وإلا
	{
	    // need to copy the command after the '|' to cmdline_copy, for the
	    // next do_one_cmd()
	    STRMOVE(cmdline_copy, next_cmdline);
	    next_cmdline = cmdline_copy;
	}


#ifdef FEAT_EVAL
	// reset did_emsg for a function that is not aborted by an error
	إذا (did_emsg && !force_abort
		&& getline_equal(fgetline, cookie, get_func_سطر)
					      && !func_has_abort(real_cookie))
	{
	    // did_emsg_cumul is not set here
	    did_emsg = FALSE;
	}

	إذا (cstack.cs_looplevel > 0)
	{
	    ++current_سطر;

	    /*
	     * An ":endwhile", ":endfor" and ":continue" is handled here.
	     * If we were executing commands, jump back to the ":while" or
	     * ":for".
	     * If we were not executing commands, decrement cs_looplevel.
	     */
	    إذا (cstack.cs_lflags & (CSL_HAD_CONT | CSL_HAD_ENDLOOP))
	    {
		cstack.cs_lflags &= ~(CSL_HAD_CONT | CSL_HAD_ENDLOOP);

		// Jump back to the matching ":while" or ":for".  Be careful
		// not to use a cs_line[] from an entry that isn't a ":while"
		// or ":for": It would make "current_line" invalid and can
		// cause a crash.
		إذا (!did_emsg && !got_int && !did_throw
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
		    إذا (breakpoint != عدم && سطریں_ga.ga_len > current_سطر)
		    {
			*breakpoint = dbg_find_breakpoint(
			       getline_equal(fgetline, cookie, getsourceline),
									fname,
			   ((wcmd_T *)سطریں_ga.ga_data)[current_سطر].lnum-1);
			*dbg_tick = debug_tick;
		    }
		}
		وإلا
		{
		    // can only get here with ":endwhile" or ":endfor"
		    إذا (cstack.cs_idx >= 0)
			rewind_conditionals(&cstack, cstack.cs_idx - 1,
				   CSF_WHILE | CSF_FOR, &cstack.cs_looplevel);
		}
	    }

	    /*
	     * For a ":while" or ":for" we need to remember the line number.
	     */
	    وإلا إذا (cstack.cs_lflags & CSL_HAD_LOOP)
	    {
		cstack.cs_lflags &= ~CSL_HAD_LOOP;
		cstack.cs_سطر[cstack.cs_idx] = current_سطر_before;
	    }
	}

	// Check for the next breakpoint after a watchexpression
	إذا (breakpoint != عدم && has_watchexpr())
	{
	    *breakpoint = dbg_find_breakpoint(FALSE, fname, SOURCING_LNUM);
	    *dbg_tick = debug_tick;
	}

	/*
	 * When not inside any ":while" loop, clear remembered lines.
	 */
	إذا (cstack.cs_looplevel == 0)
	{
	    إذا (سطریں_ga.ga_len > 0)
	    {
		SOURCING_LNUM =
		       ((wcmd_T *)سطریں_ga.ga_data)[سطریں_ga.ga_len - 1].lnum;
		free_cmdlines(&سطریں_ga);
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
	إذا (cstack.cs_lflags & CSL_HAD_FINA)
	{
	    cstack.cs_lflags &= ~CSL_HAD_FINA;
	    report_make_pending(cstack.cs_pending[cstack.cs_idx]
		    & (CSTP_ERROR | CSTP_INTERRUPT | CSTP_THROW),
		    did_throw ? (فراغ *)current_exception : عدم);
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
	إذا (trylevel == 0 && !did_emsg && !got_int && !did_throw)
	    force_abort = FALSE;

	// Convert an interrupt to an exception if appropriate.
	(فراغ)do_intthrow(&cstack);
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
    طالما (!((got_int
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
	    && (next_cmdline != عدم
#ifdef FEAT_EVAL
			|| cstack.cs_idx >= 0
#endif
			|| (flags & DOCMD_REPEAT)));

    vim_free(cmdline_copy);
    did_emsg_syntax = FALSE;
#ifdef FEAT_EVAL
    free_cmdlines(&سطریں_ga);
    ga_clear(&سطریں_ga);

    إذا (cstack.cs_idx >= 0)
    {
	/*
	 * If a sourced file or executed function ran to its end, report the
	 * unclosed conditional.
	 * In Vim9 script do not give a second error, executing aborts after
	 * the first one.
	 */
	إذا (!got_int && !did_throw && !aborting()
		&& !(did_emsg && in_vim9script())
		&& ((getline_equal(fgetline, cookie, getsourceline)
			&& !source_finished(fgetline, cookie))
		    || (getline_equal(fgetline, cookie, get_func_سطر)
					    && !func_has_ended(real_cookie))))
	{
	    إذا (cstack.cs_flags[cstack.cs_idx] & CSF_TRY)
		emsg(_(e_missing_endtry));
	    وإلا إذا (cstack.cs_flags[cstack.cs_idx] & CSF_WHILE)
		emsg(_(e_missing_endwhile));
	    وإلا إذا (cstack.cs_flags[cstack.cs_idx] & CSF_FOR)
		emsg(_(e_missing_endfor));
	    وإلا
		emsg(_(e_missing_endif));
	}

	/*
	 * Reset "trylevel" in case of a ":finish" or ":return" or a missing
	 * ":endtry" in a sourced file or executed function.  If the try
	 * conditional is in its finally clause, ignore anything pending.
	 * If it is in a catch clause, finish the caught exception.
	 * Also cleanup any "cs_forinfo" structures.
	 */
	افعل
	{
	    صحيح idx = cleanup_conditionals(&cstack, 0, TRUE);

	    إذا (idx >= 0)
		--idx;	    // remove try block not in its finally clause
	    rewind_conditionals(&cstack, idx, CSF_WHILE | CSF_FOR,
							&cstack.cs_looplevel);
	}
	طالما (cstack.cs_idx >= 0);
	trylevel = initial_trylevel;
    }

    // If a missing ":endtry", ":endwhile", ":endfor", or ":endif" or a memory
    // lack was reported above and the error message is to be converted to an
    // exception, do this now after rewinding the cstack.
    do_errthrow(&cstack, getline_equal(fgetline, cookie, get_func_سطر)
				  ? (char_u *)"endfunction" : (char_u *)عدم);

    إذا (trylevel == 0)
    {
	// Just in case did_throw got set but current_exception wasn't.
	إذا (current_exception == عدم)
	    did_throw = FALSE;

	/*
	 * When an exception is being thrown out of the outermost try
	 * conditional, discard the uncaught exception, disable the conversion
	 * of interrupts or errors to exceptions, and ensure that no more
	 * commands are executed.
	 */
	إذا (did_throw)
	    handle_did_throw();

	/*
	 * On an interrupt or an aborting error not converted to an exception,
	 * disable the conversion of errors to exceptions.  (Interrupts are not
	 * converted anymore, here.) This enables also the interrupt message
	 * when force_abort is set and did_emsg unset in case of an interrupt
	 * from a finally clause after an error.
	 */
	وإلا إذا (got_int || (did_emsg && force_abort))
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
    إذا (did_throw)
	need_rethrow = TRUE;
    إذا ((getline_equal(fgetline, cookie, getsourceline)
		&& ex_nesting_level > source_level(real_cookie))
	    || (getline_equal(fgetline, cookie, get_func_سطر)
		&& ex_nesting_level > func_level(real_cookie) + 1))
    {
	إذا (!did_throw)
	    check_cstack = TRUE;
    }
    وإلا
    {
	// When leaving a function, reduce nesting level.
	إذا (getline_equal(fgetline, cookie, get_func_سطر))
	    --ex_nesting_level;
	/*
	 * Go to debug mode when returning from a function in which we are
	 * single-stepping.
	 */
	إذا ((getline_equal(fgetline, cookie, getsourceline)
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
    إذا (flags & DOCMD_EXCRESET)
	restore_dbg_stuff(&debug_saved);

    پیغام_list = saved_پیغام_list;

    // Cleanup if "cs_emsg_silent_list" remains.
    إذا (cstack.cs_emsg_silent_list != عدم)
    {
	eslist_T *elem, *temp;

	لكل (elem = cstack.cs_emsg_silent_list; elem != عدم; elem = temp)
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
    إذا (did_inc_RedrawingDisabled)
    {
	إذا (RedrawingDisabled > 0)
	    --RedrawingDisabled;
	--no_wait_return;
	پیغام_scroll = FALSE;

	/*
	 * When just finished an ":if"-":else" which was typed, no need to
	 * wait for hit-return.  Also for an error situation.
	 */
	إذا (retval == FAIL
#ifdef FEAT_EVAL
		|| (did_endif && کلید_Typed && !did_emsg)
#endif
					    )
	{
	    need_wait_return = FALSE;
	    پیغام_didany = FALSE;		// don't wait when restarting edit
	}
	وإلا إذا (need_wait_return)
	{
	    /*
	     * The msg_start() above clears msg_didout. The wait_return() we do
	     * here should not overwrite the command that may be shown before
	     * doing that.
	     */
	    پیغام_didout |= پیغام_didout_before_start;
	    wait_return(FALSE);
	}
    }

#ifdef FEAT_EVAL
    did_endif = FALSE;  // in case do_cmdline used recursively
#وإلا
    /*
     * Reset if_level, in case a sourced script file contains more ":if" than
     * ":endif" (could be ":if x | foo | endif").
     */
    if_level = 0;
#endif

    --call_depth;
    أرجع retval;
}

#إذا defined(FEAT_EVAL)
/*
 * Handle when "did_throw" is set after executing commands.
 */
    فراغ
handle_did_throw(فراغ)
{
    حرف	*p = عدم;
    msglist_T	*messages = عدم;
    ESTACK_CHECK_DECLARATION;

    /*
     * If the uncaught exception is a user exception, report it as an
     * error.  If it is an error exception, display the saved error
     * message now.  For an interrupt exception, do nothing; the
     * interrupt message is given elsewhere.
     */
    بدل (current_exception->type)
    {
	حالة ET_USER:
	    vim_snprintf((حرف *)IObuff, IOSIZE,
		    _(e_exception_not_caught_str),
		    current_exception->قدر);
	    p = (حرف *)vim_strsave(IObuff);
	    اكسر;
	حالة ET_ERROR:
	    messages = current_exception->messages;
	    current_exception->messages = عدم;
	    اكسر;
	حالة ET_INTERRUPT:
	    اكسر;
    }

    estack_ڈالو(ETYPE_EXCEPT, current_exception->throw_name,
					current_exception->throw_lnum);
    ESTACK_CHECK_SETUP;
    current_exception->throw_name = عدم;

    discard_current_exception();	// uses IObuff if 'verbose'

    // If "silent!" is active the uncaught exception is not fatal.
    إذا (emsg_silent == 0)
    {
	suppress_errthrow = TRUE;
	force_abort = TRUE;
    }

    إذا (messages != عدم)
    {
	افعل
	{
	    msglist_T	*next = messages->next;
	    صحيح		save_compiling = estack_compiling;

	    estack_compiling = messages->پیغام_compiling;
	    emsg(messages->پیغام);
	    vim_free(messages->پیغام);
	    vim_free(messages->sfile);
	    vim_free(messages);
	    messages = next;
	    estack_compiling = save_compiling;
	}
	طالما (messages != عدم);
    }
    وإلا إذا (p != عدم)
    {
	emsg(p);
	vim_free(p);
    }
    vim_free(SOURCING_NAME);
    ESTACK_CHECK_NOW;
    estack_نکالو();
}

/*
 * Obtain a line when inside a ":while" or ":for" loop.
 */
    ساكن char_u *
get_loop_سطر(صحيح c, فراغ *cookie, صحيح indent, getline_opt_T options)
{
    بنية loop_cookie	*cp = (بنية loop_cookie *)cookie;
    wcmd_T		*wp;
    char_u		*سطر;

    إذا (cp->current_سطر + 1 >= cp->سطریں_gap->ga_len)
    {
	إذا (cp->repeating)
	    أرجع عدم;	// trying to read past ":endwhile"/":endfor"

	// First time inside the ":while"/":for": get line normally.
	إذا (cp->lc_getline == عدم)
	    سطر = getcmdline(c, 0L, indent, 0);
	وإلا
	    سطر = cp->lc_getline(c, cp->cookie, indent, options);
	إذا (سطر != عدم && store_loop_سطر(cp->سطریں_gap, سطر) == OK)
	    ++cp->current_سطر;

	أرجع سطر;
    }

    کلید_Typed = FALSE;
    ++cp->current_سطر;
    wp = (wcmd_T *)(cp->سطریں_gap->ga_data) + cp->current_سطر;
    SOURCING_LNUM = wp->lnum;
    أرجع vim_strsave(wp->سطر);
}

/*
 * Store a line in "gap" so that a ":while" loop can execute it again.
 */
    ساكن صحيح
store_loop_سطر(garray_T *gap, char_u *سطر)
{
    إذا (ga_grow(gap, 1) == FAIL)
	أرجع FAIL;
    ((wcmd_T *)(gap->ga_data))[gap->ga_len].سطر = vim_strsave(سطر);
    ((wcmd_T *)(gap->ga_data))[gap->ga_len].lnum = SOURCING_LNUM;
    ++gap->ga_len;
    أرجع OK;
}

/*
 * Free the lines stored for a ":while" or ":for" loop.
 */
    ساكن فراغ
free_cmdlines(garray_T *gap)
{
    طالما (gap->ga_len > 0)
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
    صحيح
getline_equal(
    char_u	*(*fgetline)(صحيح, فراغ *, صحيح, getline_opt_T),
    فراغ	*cookie UNUSED,		// argument for fgetline()
    char_u	*(*func)(صحيح, فراغ *, صحيح, getline_opt_T))
{
#ifdef FEAT_EVAL
    char_u		*(*gp)(صحيح, فراغ *, صحيح, getline_opt_T);
    بنية loop_cookie *cp;

    // When "fgetline" is "get_loop_line()" use the "cookie" to find the
    // function that's originally used to obtain the lines.  This may be
    // nested several levels.
    gp = fgetline;
    cp = (بنية loop_cookie *)cookie;
    طالما (gp == get_loop_سطر)
    {
	gp = cp->lc_getline;
	cp = cp->cookie;
    }
    أرجع gp == func;
#وإلا
    أرجع fgetline == func;
#endif
}

/*
 * If "fgetline" is get_loop_line(), return the cookie used by the original
 * getline function.  Otherwise return "cookie".
 */
    فراغ *
getline_cookie(
    char_u	*(*fgetline)(صحيح, فراغ *, صحيح, getline_opt_T) UNUSED,
    فراغ	*cookie)		// argument for fgetline()
{
#ifdef FEAT_EVAL
    char_u		*(*gp)(صحيح, فراغ *, صحيح, getline_opt_T);
    بنية loop_cookie  *cp;

    // When "fgetline" is "get_loop_line()" use the "cookie" to find the
    // cookie that's originally used to obtain the lines.  This may be nested
    // several levels.
    gp = fgetline;
    cp = (بنية loop_cookie *)cookie;
    طالما (gp == get_loop_سطر)
    {
	gp = cp->lc_getline;
	cp = cp->cookie;
    }
    أرجع cp;
#وإلا
    أرجع cookie;
#endif
}

#إذا defined(FEAT_EVAL)
/*
 * Get the next line source line without advancing.
 */
    char_u *
getline_peek(
    char_u	*(*fgetline)(صحيح, فراغ *, صحيح, getline_opt_T) UNUSED,
    فراغ	*cookie)		// argument for fgetline()
{
    char_u		*(*gp)(صحيح, فراغ *, صحيح, getline_opt_T);
    بنية loop_cookie  *cp;
    wcmd_T		*wp;

    // When "fgetline" is "get_loop_line()" use the "cookie" to find the
    // cookie that's originally used to obtain the lines.  This may be nested
    // several levels.
    gp = fgetline;
    cp = (بنية loop_cookie *)cookie;
    طالما (gp == get_loop_سطر)
    {
	إذا (cp->current_سطر + 1 < cp->سطریں_gap->ga_len)
	{
	    // executing lines a second time, use the stored copy
	    wp = (wcmd_T *)(cp->سطریں_gap->ga_data) + cp->current_سطر + 1;
	    أرجع wp->سطر;
	}
	gp = cp->lc_getline;
	cp = cp->cookie;
    }
    إذا (gp == getsourceline)
	أرجع source_nextline(cp);
    أرجع عدم;
}
#endif


/*
 * Helper function to apply an offset for buffer commands, i.e. ":bdelete",
 * ":bwipeout", etc.
 * Returns the buffer number.
 */
    ساكن صحيح
compute_buffer_local_گنتی(صحيح addr_type, صحيح lnum, صحيح offset)
{
    buf_T   *buf;
    buf_T   *nextbuf;
    صحيح     گنتی = offset;

    buf = firstbuf;
    طالما (buf->b_next != عدم && buf->b_fnum < lnum)
	buf = buf->b_next;
    طالما (گنتی != 0)
    {
	گنتی += (offset < 0) ? 1 : -1;
	nextbuf = (offset < 0) ? buf->b_prev : buf->b_next;
	إذا (nextbuf == عدم)
	    اكسر;
	buf = nextbuf;
	إذا (addr_type == ADDR_LOADED_BUFFERS)
	    // skip over unloaded buffers
	    طالما (buf->b_ml.ml_mfp == عدم)
	    {
		nextbuf = (offset < 0) ? buf->b_prev : buf->b_next;
		إذا (nextbuf == عدم)
		    اكسر;
		buf = nextbuf;
	    }
    }
    // we might have gone too far, last buffer is not loadedd
    إذا (addr_type == ADDR_LOADED_BUFFERS)
	طالما (buf->b_ml.ml_mfp == عدم)
	{
	    nextbuf = (offset >= 0) ? buf->b_prev : buf->b_next;
	    إذا (nextbuf == عدم)
		اكسر;
	    buf = nextbuf;
	}
    أرجع buf->b_fnum;
}

/*
 * Return the window number of "win".
 * When "win" is NULL return the number of windows.
 */
    ساكن صحيح
current_win_nr(win_T *win)
{
    win_T	*wp;
    صحيح		nr = 0;

    FOR_ALL_WINDOWS(wp)
    {
	++nr;
	إذا (wp == win)
	    اكسر;
    }
    أرجع nr;
}

    ساكن صحيح
current_tab_nr(tabpage_T *tab)
{
    tabpage_T	*tp;
    صحيح		nr = 0;

    FOR_ALL_TABPAGES(tp)
    {
	++nr;
	إذا (tp == tab)
	    اكسر;
    }
    أرجع nr;
}

    ساكن صحيح
comment_start(char_u *p, صحيح starts_with_colon UNUSED)
{
    إذا (in_vim9script())
	أرجع p[0] == '#' && !starts_with_colon;
    أرجع *p == '"';
}

#define CURRENT_WIN_NR current_win_nr(curwin)
#define LAST_WIN_NR current_win_nr(عدم)
#define CURRENT_TAB_NR current_tab_nr(curtab)
#define LAST_TAB_NR current_tab_nr(عدم)

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
    ساكن char_u *
do_one_cmd(
    char_u	**cmdlinep,
    صحيح		flags,
#ifdef FEAT_EVAL
    cstack_T	*cstack,
#endif
    char_u	*(*fgetline)(صحيح, فراغ *, صحيح, getline_opt_T),
    فراغ	*cookie)		// argument for fgetline()
{
    char_u	*p;
    linenr_T	lnum;
    long	n;
    حرف	*errormsg = عدم;	// error message
    char_u	*after_modifier = عدم;
    exarg_T	ea;			// Ex command arguments
    cmdmod_T	save_cmdmod;
    صحيح		save_reg_executing = reg_executing;
    صحيح		save_pending_end_reg_executing = pending_end_reg_executing;
    صحيح		ni;			// set when Not Implemented
    char_u	*cmd;
    صحيح		starts_with_colon = FALSE;
    صحيح		may_have_range;
#ifdef FEAT_EVAL
    صحيح		did_set_اظہاریہ_سطر = FALSE;
#endif
    صحيح		sourcing = flags & DOCMD_VERBOSE;
    صحيح		did_append_cmd = FALSE;

    CLEAR_FIELD(ea);
    ea.line1 = 1;
    ea.line2 = 1;
#ifdef FEAT_EVAL
    ++ex_nesting_level;
#endif

    // When the last file has not been edited :q has to be typed twice.
    إذا (quitmore
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
    إذا ((*cmdlinep)[0] == '#' && (*cmdlinep)[1] == '!')
	اذهب doend;

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
    إذا (parse_command_modifiers(&ea, &errormsg, &cmdmod, FALSE) == FAIL)
	اذهب doend;
    apply_cmdmod(&cmdmod);
    after_modifier = ea.cmd;

#ifdef FEAT_EVAL
    ea.skip = did_emsg || got_int || did_throw || (cstack->cs_idx >= 0
			 && !(cstack->cs_flags[cstack->cs_idx] & CSF_ACTIVE));
#وإلا
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
    صحيح vim9script = in_vim9script();
    إذا (vim9script && (flags & DOCMD_RANGEOK) == 0)
    {
	may_have_range = FALSE;
	لكل (p = ea.cmd; p >= *cmdlinep; --p)
	{
	    إذا (*p == ':')
		may_have_range = TRUE;
	    إذا (p < ea.cmd && !VIM_ISWHITE(*p))
		اكسر;
	}
    }
    وإلا
	may_have_range = TRUE;
    إذا (may_have_range)
	ea.cmd = skip_range(ea.cmd, TRUE, عدم);

#ifdef FEAT_EVAL
    // Handle ":export" - it functions almost like a command modifier.
    // ":export var Name: type"
    // ":export def Name(..."
    // etc.
    إذا (vim9script && checkforcmd_noparen(&ea.cmd, "export", 6))
	is_export = TRUE;
#endif

    إذا (vim9script && !may_have_range)
    {
	إذا (ea.cmd == cmd + 1 && *cmd == '$')
	    // should be "$VAR = val"
	    --ea.cmd;
#ifdef FEAT_EVAL
	p = find_ex_command(&ea, عدم, lookup_scriptitem, عدم);
#وإلا
	p = find_ex_command(&ea, عدم, عدم, عدم);
#endif
	إذا (ea.cmdidx == CMD_SIZE)
	{
	    char_u *ar = skip_range(ea.cmd, TRUE, عدم);

	    // If a ':' before the range is missing, give a clearer error
	    // message.
	    إذا (ar > ea.cmd && !ea.skip)
	    {
		semsg(_(e_colon_required_before_range_str), ea.cmd);
		اذهب doend;
	    }
	}
    }
    وإلا
	p = find_ex_command(&ea, عدم, عدم, عدم);

#ifdef FEAT_EVAL
# ifdef FEAT_PROFILE
    // Count this line for profiling if skip is TRUE.
    إذا (do_profiling == PROF_YES
	    && (!ea.skip || cstack->cs_idx == 0 || (cstack->cs_idx > 0
		     && (cstack->cs_flags[cstack->cs_idx - 1] & CSF_ACTIVE))))
    {
	صحيح skip = did_emsg || got_int || did_throw;

	إذا (ea.cmdidx == CMD_catch)
	    skip = !skip && !(cstack->cs_idx >= 0
			  && (cstack->cs_flags[cstack->cs_idx] & CSF_THROWN)
			  && !(cstack->cs_flags[cstack->cs_idx] & CSF_CAUGHT));
	وإلا إذا (ea.cmdidx == CMD_else || ea.cmdidx == CMD_elseif)
	    skip = skip || !(cstack->cs_idx >= 0
			  && !(cstack->cs_flags[cstack->cs_idx]
						  & (CSF_ACTIVE | CSF_TRUE)));
	وإلا إذا (ea.cmdidx == CMD_finally)
	    skip = FALSE;
	وإلا إذا (ea.cmdidx != CMD_endif
		&& ea.cmdidx != CMD_endfor
		&& ea.cmdidx != CMD_endtry
		&& ea.cmdidx != CMD_endwhile)
	    skip = ea.skip;

	إذا (!skip)
	{
	    إذا (getline_equal(fgetline, cookie, get_func_سطر))
		func_سطر_exec(getline_cookie(fgetline, cookie));
	    وإلا إذا (getline_equal(fgetline, cookie, getsourceline))
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
    إذا (!ea.skip && got_int)
    {
	ea.skip = TRUE;
	(فراغ)do_intthrow(cstack);
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
    إذا (!IS_USER_CMDIDX(ea.cmdidx))
    {
	إذا (ea.cmdidx != CMD_SIZE)
	    ea.addr_type = cmdnames[(صحيح)ea.cmdidx].cmd_addr_type;
	وإلا
	    ea.addr_type = ADDR_سطریں;

	// :wincmd range depends on the argument.
	إذا (ea.cmdidx == CMD_wincmd && p != عدم)
	    get_wincmd_addr_type(skipwhite(p), &ea);
#ifdef FEAT_QUICKFIX
	// :.cc in quickfix window uses line number
	إذا ((ea.cmdidx == CMD_cc || ea.cmdidx == CMD_ll) && bt_quickfix(curbuf))
	    ea.addr_type = ADDR_OTHER;
#endif
    }

    إذا (!may_have_range)
	ea.line1 = ea.line2 = default_address(&ea);
    وإلا إذا (parse_cmd_address(&ea, &errormsg, FALSE) == FAIL)
	اذهب doend;

/*
 * 5. Parse the command.
 */

    /*
     * Skip ':' and any white space
     */
    ea.cmd = skipwhite(ea.cmd);
    طالما (*ea.cmd == ':')
	ea.cmd = skipwhite(ea.cmd + 1);

    /*
     * If we got a line, but no command, then go to the line.
     * If we find a '|' or '\n' we set ea.nextcmd.
     */
    إذا (*ea.cmd == NUL || comment_start(ea.cmd, starts_with_colon)
			       || (ea.nextcmd = check_nextcmd(ea.cmd)) != عدم)
    {
	/*
	 * strange vi behaviour:
	 * ":3"		jumps to line 3
	 * ":3|..."	prints line 3  (not in Vim9 script)
	 * ":|"		prints current line  (not in Vim9 script)
	 */
	إذا (ea.skip)	    // skip this if inside :if
	    اذهب doend;
	errormsg = ex_range_without_command(&ea);
	اذهب doend;
    }

    // If this looks like an undefined user command and there are CmdUndefined
    // autocommands defined, trigger the matching autocommands.
    إذا (p != عدم && ea.cmdidx == CMD_SIZE && !ea.skip
	    && ASCII_ISUPPER(*ea.cmd)
	    && has_cmdundefined())
    {
	صحيح ret;

	p = ea.cmd;
	طالما (ASCII_ISALNUM(*p))
	    ++p;
	p = vim_strnsave(ea.cmd, p - ea.cmd);
	ret = apply_autocmds(EVENT_CMDUNDEFINED, p, p, TRUE, عدم);
	vim_free(p);
	// If the autocommands did something and didn't cause an error, try
	// finding the command again.
	p = (ret
#ifdef FEAT_EVAL
		&& !aborting()
#endif
		) ? find_ex_command(&ea, عدم, عدم, عدم) : ea.cmd;
    }

    إذا (p == عدم)
    {
	إذا (!ea.skip)
	    errormsg = _(e_ambiguous_use_of_user_defined_command);
	اذهب doend;
    }
    // Check for wrong commands.
    إذا (*p == '!' && ea.cmd[1] == 0151 && ea.cmd[0] == 78
	    && !IS_USER_CMDIDX(ea.cmdidx))
    {
	errormsg = uc_fun_cmd();
	اذهب doend;
    }

    إذا (ea.cmdidx == CMD_SIZE)
    {
	إذا (!ea.skip)
	{
	    STRCPY(IObuff, _(e_not_an_editor_command));
	    إذا (!sourcing)
	    {
		// If the modifier was parsed OK the error must be in the
		// following command
		إذا (after_modifier != عدم)
		    append_command(after_modifier);
		وإلا
		    append_command(*cmdlinep);
		did_append_cmd = TRUE;
	    }
	    errormsg = (حرف *)IObuff;
	    did_emsg_syntax = TRUE;
	}
	اذهب doend;
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
    إذا (ea.cmdidx == CMD_if)
	++if_level;
    إذا (if_level)
    {
	إذا (ea.cmdidx == CMD_endif)
	    --if_level;
	اذهب doend;
    }

#endif

    // forced commands
    إذا (*p == '!' && ea.cmdidx != CMD_substitute
	    && ea.cmdidx != CMD_smagic && ea.cmdidx != CMD_snomagic)
    {
	++p;
	ea.forceit = TRUE;
    }
    وإلا
	ea.forceit = FALSE;

/*
 * 6. Parse arguments.  Then check for errors.
 */
    إذا (!IS_USER_CMDIDX(ea.cmdidx))
	ea.argt = (long)cmdnames[(صحيح)ea.cmdidx].cmd_argt;

    إذا (!ea.skip)
    {
#ifdef HAVE_SANDBOX
	إذا (sandbox != 0 && !(ea.argt & EX_SBOXOK))
	{
	    // Command not allowed in sandbox.
	    errormsg = _(e_not_allowed_in_sandbox);
	    اذهب doend;
	}
#endif
	إذا (restricted != 0 && (ea.argt & EX_RESTRICT))
	{
	    errormsg = _(e_command_not_allowed_in_rvim);
	    اذهب doend;
	}
	إذا (!curbuf->b_p_ma && (ea.argt & EX_MODIFY))
	{
	    // Command not allowed in non-'modifiable' buffer
	    errormsg = _(e_cannot_make_changes_modifiable_is_off);
	    اذهب doend;
	}

	إذا (!IS_USER_CMDIDX(ea.cmdidx))
	{
	    إذا (cmdwin_type != 0 && !(ea.argt & EX_CMDWIN))
	    {
		// Command not allowed in the command line window
		errormsg = _(e_invalid_in_cmdline_window);
		اذهب doend;
	    }
	    إذا (text_locked() && !(ea.argt & EX_LOCK_OK))
	    {
		// Command not allowed when text is locked
		errormsg = _(get_text_locked_پیغام());
		اذهب doend;
	    }
	}

	// Disallow editing another buffer when "curbuf_lock" is set.
	// Do allow ":checktime" (it is postponed).
	// Do allow ":edit" (check for an argument later).
	// Do allow ":file" with no arguments (check for an argument later).
	إذا (!(ea.argt & (EX_CMDWIN | EX_LOCK_OK))
		&& ea.cmdidx != CMD_checktime
		&& ea.cmdidx != CMD_edit
		&& ea.cmdidx != CMD_file
		&& !IS_USER_CMDIDX(ea.cmdidx)
		&& curbuf_locked())
	    اذهب doend;

	إذا (!ni && !(ea.argt & EX_RANGE) && ea.addr_گنتی > 0)
	{
	    errormsg = _(e_no_range_allowed);
	    اذهب doend;
	}
    }

    إذا (!ni && !(ea.argt & EX_BANG) && ea.forceit)
    {
	errormsg = _(e_no_bang_allowed);
	اذهب doend;
    }

    /*
     * Don't complain about the range if it is not used
     * (could happen if line_count is accidentally set to 0).
     */
    إذا (!ea.skip && !ni && (ea.argt & EX_RANGE))
    {
	/*
	 * If the range is backwards, ask for confirmation and, if given, swap
	 * ea.line1 & ea.line2 so it's forwards again.
	 * When global command is busy, don't ask, will fail below.
	 */
	إذا (!global_busy && ea.line1 > ea.line2)
	{
	    إذا (پیغام_silent == 0)
	    {
		إذا (sourcing || exmode_active)
		{
		    errormsg = _(e_backwards_range_given);
		    اذهب doend;
		}
		إذا (ask_yesno((char_u *)
			_("Backwards range given, OK to swap"), FALSE) != 'y')
		    اذهب doend;
	    }
	    lnum = ea.line1;
	    ea.line1 = ea.line2;
	    ea.line2 = lnum;
	}
	إذا ((errormsg = invalid_range(&ea)) != عدم)
	    اذهب doend;
    }

    إذا ((ea.addr_type == ADDR_OTHER) && ea.addr_گنتی == 0)
	// default is 1, not cursor
	ea.line2 = 1;

    correct_range(&ea);

#ifdef FEAT_FOLDING
    إذا (((ea.argt & EX_WHOLEFOLD) || ea.addr_گنتی >= 2) && !global_busy
	    && ea.addr_type == ADDR_سطریں)
    {
	// Put the first line at the start of a closed fold, put the last line
	// at the end of a closed fold.
	(فراغ)hasFolding(ea.line1, &ea.line1, عدم);
	(فراغ)hasFolding(ea.line2, عدم, &ea.line2);
    }
#endif

#ifdef FEAT_QUICKFIX
    /*
     * For the ":make" and ":grep" commands we insert the 'makeprg'/'grepprg'
     * option here, so things like % get expanded.
     */
    p = replace_makeprg(&ea, p, cmdlinep);
    إذا (p == عدم)
	اذهب doend;
#endif

    /*
     * Skip to start of argument.
     * Don't do this for the ":!" command, because ":!! -l" needs the space.
     */
    إذا (ea.cmdidx == CMD_bang)
	ea.arg = p;
    وإلا
	ea.arg = skipwhite(p);

    // ":file" cannot be run with an argument when "curbuf_lock" is set
    إذا (ea.cmdidx == CMD_file && *ea.arg != NUL && curbuf_locked())
	اذهب doend;

    /*
     * Check for "++opt=val" argument.
     * Must be first, allow ":w ++enc=utf8 !cmd"
     */
    إذا (ea.argt & EX_ARGOPT)
	طالما (ea.arg[0] == '+' && ea.arg[1] == '+')
	    إذا (getargopt(&ea) == FAIL && !ni)
	    {
		errormsg = _(e_invalid_argument);
		اذهب doend;
	    }

    إذا (ea.cmdidx == CMD_write || ea.cmdidx == CMD_update)
    {
	إذا (*ea.arg == '>')			// append
	{
	    إذا (*++ea.arg != '>')		// typed wrong
	    {
		errormsg = _(e_use_w_or_w_gt_gt);
		اذهب doend;
	    }
	    ea.arg = skipwhite(ea.arg + 1);
	    ea.append = TRUE;
	}
	وإلا إذا (*ea.arg == '!' && ea.cmdidx == CMD_write)  // :w !filter
	{
	    ++ea.arg;
	    ea.usefilter = TRUE;
	}
    }

    إذا (ea.cmdidx == CMD_read)
    {
	إذا (ea.forceit)
	{
	    ea.usefilter = TRUE;		// :r! filter if ea.forceit
	    ea.forceit = FALSE;
	}
	وإلا إذا (*ea.arg == '!')		// :r !filter
	{
	    ++ea.arg;
	    ea.usefilter = TRUE;
	}
    }

    إذا (ea.cmdidx == CMD_lshift || ea.cmdidx == CMD_rshift)
    {
	ea.amount = 1;
	طالما (*ea.arg == *ea.cmd)		// count number of '>' or '<'
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
    إذا ((ea.argt & EX_CMDARG) && !ea.usefilter)
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
    إذا ((ea.argt & EX_TRLBAR) && !ea.usefilter)
    {
	separate_nextcmd(&ea, FALSE);
    }
    وإلا إذا (ea.cmdidx == CMD_bang
	    || ea.cmdidx == CMD_terminal
	    || ea.cmdidx == CMD_global
	    || ea.cmdidx == CMD_vglobal
	    || ea.usefilter
#ifdef FEAT_EVAL
	    || inside_block(&ea)
#endif
	    )
    {
	لكل (p = ea.arg; *p; ++p)
	{
	    // Remove one backslash before a newline
	    إذا (*p == '\\' && p[1] == '\n')
		STRMOVE(p, p + 1);
	    وإلا إذا (*p == '\n' && !(ea.argt & EX_اظہاریہ_ARG))
	    {
		ea.nextcmd = p + 1;
		*p = NUL;
		اكسر;
	    }
	}
    }

    إذا ((ea.argt & EX_DFLALL) && ea.addr_گنتی == 0)
	address_default_all(&ea);

    // accept numbered register only when no count allowed (:put)
    إذا (       (ea.argt & EX_REGSTR)
	    && *ea.arg != NUL
	       // Do not allow register = for user commands
	    && (!IS_USER_CMDIDX(ea.cmdidx) || *ea.arg != '=')
	    && !((ea.argt & EX_گنتی) && VIM_ISDIGIT(*ea.arg)))
    {
#إذا !defined(FEAT_CLIPBOARD) && !defined(FEAT_CLIPBOARD_PROVIDER)
	// check these explicitly for a more specific error message
	إذا (*ea.arg == '*' || *ea.arg == '+')
	{
	    errormsg = _(e_invalid_register_name);
	    اذهب doend;
	}
#endif
	إذا (valid_yank_reg(*ea.arg, (!IS_USER_CMDIDX(ea.cmdidx)
			    && ea.cmdidx != CMD_put && ea.cmdidx != CMD_iput)))
	{
	    ea.regname = *ea.arg++;
#ifdef FEAT_EVAL
	    // for '=' register: accept the rest of the line as an expression
	    إذا (ea.arg[-1] == '=' && ea.arg[0] != NUL)
	    {
		إذا (!ea.skip)
		{
		    set_اظہاریہ_سطر(vim_strsave(ea.arg), &ea);
		    did_set_اظہاریہ_سطر = TRUE;
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
    إذا ((ea.argt & EX_گنتی) && VIM_ISDIGIT(*ea.arg)
	    && (!(ea.argt & EX_BUFNAME) || *(p = skipdigits(ea.arg + 1)) == NUL
							  || VIM_ISWHITE(*p)))
    {
	n = getdigits_quoted(&ea.arg);
	ea.arg = skipwhite(ea.arg);
	إذا (n <= 0 && !ni && (ea.argt & EX_ZEROR) == 0)
	{
	    errormsg = _(e_positive_گنتی_required);
	    اذهب doend;
	}
	إذا (ea.addr_type != ADDR_سطریں)	// e.g. :buffer 2, :sleep 3
	{
	    ea.line2 = n;
	    إذا (ea.addr_گنتی == 0)
		ea.addr_گنتی = 1;
	}
	وإلا
	{
	    ea.line1 = ea.line2;
	    إذا (ea.line2 >= LONG_MAX - (n - 1))
		ea.line2 = LONG_MAX;  // avoid overflow
	    وإلا
		ea.line2 += n - 1;
	    ++ea.addr_گنتی;
	    /*
	     * Be vi compatible: no error message for out of range.
	     */
	    إذا (ea.line2 > curbuf->b_ml.ml_سطر_گنتی)
		ea.line2 = curbuf->b_ml.ml_سطر_گنتی;
	}
    }

    /*
     * Check for flags: 'l', 'p' and '#'.
     */
    إذا (ea.argt & EX_FLAGS)
	get_flags(&ea);
    إذا (!ni && !(ea.argt & EX_EXTRA) && *ea.arg != NUL
	    && *ea.arg != '"' && (*ea.arg != '|' || (ea.argt & EX_TRLBAR) == 0))
    {
	// no arguments allowed but there is something
	errormsg = ex_errmsg(e_trailing_characters_str, ea.arg);
	اذهب doend;
    }

    إذا (!ni && (ea.argt & EX_NEEDARG) && *ea.arg == NUL)
    {
	errormsg = _(e_argument_required);
	اذهب doend;
    }

#ifdef FEAT_EVAL
    /*
     * Skip the command when it's not going to be executed.
     * The commands like :if, :endif, etc. always need to be executed.
     * Also make an exception for commands that handle a trailing command
     * themselves.
     */
    إذا (ea.skip)
    {
	بدل (ea.cmdidx)
	{
	    // commands that need evaluation
	    حالة CMD_while:
	    حالة CMD_endwhile:
	    حالة CMD_for:
	    حالة CMD_endfor:
	    حالة CMD_if:
	    حالة CMD_elseif:
	    حالة CMD_else:
	    حالة CMD_endif:
	    حالة CMD_try:
	    حالة CMD_catch:
	    حالة CMD_finally:
	    حالة CMD_endtry:
	    حالة CMD_function:
	    حالة CMD_def:
				اكسر;

	    // commands that read a block of lines
	    حالة CMD_abstract:
	    حالة CMD_append:
	    حالة CMD_autocmd:
	    حالة CMD_change:
	    حالة CMD_class:
	    حالة CMD_command:
	    حالة CMD_enum:
	    حالة CMD_داخل_کرو:
	    حالة CMD_interface:
	    حالة CMD_loadkeymap:
				اكسر;

	    // Commands that handle '|' themselves.  Check: A command should
	    // either have the EX_TRLBAR flag, appear in this list or appear in
	    // the list at ":help :bar".
	    حالة CMD_aboveleft:
	    حالة CMD_and:
	    حالة CMD_belowright:
	    حالة CMD_botright:
	    حالة CMD_browse:
	    حالة CMD_call:
	    حالة CMD_confirm:
	    حالة CMD_const:
	    حالة CMD_delfunction:
	    حالة CMD_djump:
	    حالة CMD_dlist:
	    حالة CMD_dsearch:
	    حالة CMD_dsplit:
	    حالة CMD_echo:
	    حالة CMD_echoerr:
	    حالة CMD_echomsg:
	    حالة CMD_echon:
	    حالة CMD_eval:
	    حالة CMD_execute:
	    حالة CMD_filter:
	    حالة CMD_final:
	    حالة CMD_help:
	    حالة CMD_hide:
	    حالة CMD_horizontal:
	    حالة CMD_ijump:
	    حالة CMD_ilist:
	    حالة CMD_isearch:
	    حالة CMD_isplit:
	    حالة CMD_keepalt:
	    حالة CMD_keepjumps:
	    حالة CMD_keepmarks:
	    حالة CMD_keeppatterns:
	    حالة CMD_leftabove:
	    حالة CMD_let:
	    حالة CMD_lockmarks:
	    حالة CMD_lockvar:
	    حالة CMD_lua:
	    حالة CMD_match:
	    حالة CMD_mzscheme:
	    حالة CMD_noautocmd:
	    حالة CMD_noswapfile:
	    حالة CMD_perl:
	    حالة CMD_psearch:
	    حالة CMD_py3:
	    حالة CMD_python3:
	    حالة CMD_python:
	    حالة CMD_return:
	    حالة CMD_rightbelow:
	    حالة CMD_ruby:
	    حالة CMD_silent:
	    حالة CMD_smagic:
	    حالة CMD_snomagic:
	    حالة CMD_substitute:
	    حالة CMD_syntax:
	    حالة CMD_tab:
	    حالة CMD_tcl:
	    حالة CMD_throw:
	    حالة CMD_tilde:
	    حالة CMD_topleft:
	    حالة CMD_unlet:
	    حالة CMD_unlockvar:
	    حالة CMD_var:
	    حالة CMD_verbose:
	    حالة CMD_vertical:
	    حالة CMD_wincmd:
				اكسر;

	    افتراضي:		اذهب doend;
	}
    }
#endif

    إذا ((ea.argt & EX_XFILE)
	    && expand_filename(&ea, cmdlinep, &errormsg) == FAIL)
	اذهب doend;

#ifdef FEAT_EVAL
    إذا (is_export && (ea.argt & EX_EXPORT) == 0)
    {
	emsg(_(e_invalid_command_after_export));
	اذهب doend;
    }
#endif

    /*
     * Accept buffer name.  Cannot be used at the same time with a buffer
     * number.  Don't do this for a user command.
     */
    إذا ((ea.argt & EX_BUFNAME) && *ea.arg != NUL && ea.addr_گنتی == 0
	    && !IS_USER_CMDIDX(ea.cmdidx))
    {
	/*
	 * :bdelete, :bwipeout and :bunload take several arguments, separated
	 * by spaces: find next space (skipping over escaped characters).
	 * The others take one argument: ignore trailing spaces.
	 */
	إذا (ea.cmdidx == CMD_bdelete || ea.cmdidx == CMD_bwipeout
						  || ea.cmdidx == CMD_bunload)
	    p = skiptowhite_esc(ea.arg);
	وإلا
	{
	    p = ea.arg + STRLEN(ea.arg);
	    طالما (p > ea.arg && VIM_ISWHITE(p[-1]))
		--p;
	}
	ea.line2 = buflist_findpat(ea.arg, p, (ea.argt & EX_BUFUNL) != 0,
								FALSE, FALSE);
	إذا (ea.line2 < 0)	    // failed
	    اذهب doend;
	ea.addr_گنتی = 1;
	ea.arg = skipwhite(p);
    }

    // The :try command saves the emsg_silent flag, reset it here when
    // ":silent! try" was used, it should only apply to :try itself.
    إذا (ea.cmdidx == CMD_try && cmdmod.cmod_did_esilent > 0)
    {
	emsg_silent -= cmdmod.cmod_did_esilent;
	إذا (emsg_silent < 0)
	    emsg_silent = 0;
	cmdmod.cmod_did_esilent = 0;
    }

/*
 * 7. Execute the command.
 */

    إذا (IS_USER_CMDIDX(ea.cmdidx))
    {
	/*
	 * Execute a user-defined command.
	 */
	do_ucmd(&ea);
    }
    وإلا
    {
	/*
	 * Call the function to execute the builtin command.
	 */
	(cmdnames[ea.cmdidx].cmd_func)(&ea);
	إذا (ea.errmsg != عدم)
	    errormsg = ea.errmsg;
    }

#ifdef FEAT_EVAL
    // A command will reset "is_export" when exporting an item.  If it is still
    // set something went wrong or the command was never executed.
    إذا (!ea.skip && is_export)
    {
	إذا (errormsg == عدم)
	    errormsg = _(e_export_with_invalid_argument);
	is_export = FALSE;
    }

    // Set flag that any command was executed, used by ex_vim9script().
    // Not if this was a command that wasn't executed or :endif.
    إذا (sourcing_a_script(&ea)
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
    إذا (need_rethrow)
	do_throw(cstack);
    وإلا إذا (check_cstack)
    {
	إذا (source_finished(fgetline, cookie))
	    do_finish(&ea, TRUE);
	وإلا إذا (getline_equal(fgetline, cookie, get_func_سطر)
						   && current_func_returned())
	    do_return(&ea, TRUE, FALSE, عدم);
    }
    need_rethrow = check_cstack = FALSE;
#endif

doend:
    إذا (curwin->w_cursor.lnum == 0)	// can happen with zero line number
    {
	curwin->w_cursor.lnum = 1;
	curwin->w_cursor.col = 0;
    }

    إذا (errormsg != عدم && *errormsg != NUL && !did_emsg)
    {
	إذا ((sourcing || !کلید_Typed) && !did_append_cmd)
	{
	    إذا (errormsg != (حرف *)IObuff)
	    {
		STRCPY(IObuff, errormsg);
		errormsg = (حرف *)IObuff;
	    }
	    append_command(*cmdlinep);
	}
	emsg(errormsg);
    }
#ifdef FEAT_EVAL
    do_errthrow(cstack,
	    (ea.cmdidx != CMD_SIZE && !IS_USER_CMDIDX(ea.cmdidx))
			? cmdnames[(صحيح)ea.cmdidx].cmd_name : (char_u *)عدم);

    إذا (did_set_اظہاریہ_سطر)
	set_اظہاریہ_سطر(عدم, عدم);
    is_export = FALSE;
#endif

    undo_cmdmod(&cmdmod);
    cmdmod = save_cmdmod;
    reg_executing = save_reg_executing;
    pending_end_reg_executing = save_pending_end_reg_executing;

    إذا (ea.nextcmd && *ea.nextcmd == NUL)	// not really a next command
	ea.nextcmd = عدم;

#ifdef FEAT_EVAL
    --ex_nesting_level;
    vim_free(ea.cmdline_tofree);
#endif

    أرجع ea.nextcmd;
}

ساكن حرف ex_error_buf[پیغام_BUF_LEN];

/*
 * Return an error message with argument included.
 * Uses a static buffer, only the last error will be kept.
 * "msg" will be translated, caller should use N_().
 */
    حرف *
ex_errmsg(حرف *پیغام, char_u *arg)
{
    vim_snprintf(ex_error_buf, پیغام_BUF_LEN, _(پیغام), arg);
    أرجع ex_error_buf;
}

/*
 * The "+" string used in place of an empty command in Ex mode.
 * This string is used in pointer comparison.
 */
ساكن ثابت حرف exmode_plus[] = "+";

/*
 * Handle a range without a command.
 * Returns an error message on failure.
 */
    حرف *
ex_range_without_command(exarg_T *eap)
{
    حرف *errormsg = عدم;

    إذا ((*eap->cmd == '|' ||
		(exmode_active && eap->cmd != (char_u *)exmode_plus + 1))
#ifdef FEAT_EVAL
	    && !in_vim9script()
#endif
       )
    {
	eap->cmdidx = CMD_print;
	eap->argt = EX_RANGE+EX_گنتی+EX_TRLBAR;
	إذا ((errormsg = invalid_range(eap)) == عدم)
	{
	    correct_range(eap);
	    ex_print(eap);
	}
    }
    وإلا إذا (eap->addr_گنتی != 0)
    {
	إذا (eap->line2 > curbuf->b_ml.ml_سطر_گنتی)
	{
	    // With '-' in 'cpoptions' a line number past the file is an
	    // error, otherwise put it at the end of the file.
	    إذا (vim_strchr(p_cpo, CPO_MINUS) != عدم)
		eap->line2 = -1;
	    وإلا
		eap->line2 = curbuf->b_ml.ml_سطر_گنتی;
	}

	إذا (eap->line2 < 0)
	    errormsg = _(e_invalid_range);
	وإلا
	{
	    إذا (eap->line2 == 0)
		curwin->w_cursor.lnum = 1;
	    وإلا
		curwin->w_cursor.lnum = eap->line2;
	    beginline(BL_SOL | BL_FIX);
	}
    }
    أرجع errormsg;
}

/*
 * Check for an Ex command with optional tail.
 * If there is a match advance "pp" to the argument and return TRUE.
 * If "noparen" is TRUE do not recognize the command followed by "(" or ".".
 */
    ساكن صحيح
checkforcmd_opt(
    char_u	**pp,		// start of command
    حرف	*cmd,		// name of command
    صحيح		len,		// required length
    صحيح		noparen)
{
    صحيح		i;

    لكل (i = 0; cmd[i] != NUL; ++i)
	إذا (((char_u *)cmd)[i] != (*pp)[i])
	    اكسر;
    إذا (i >= len && !ASCII_ISALPHA((*pp)[i]) && (*pp)[i] != '_'
			 && (!noparen || ((*pp)[i] != '(' && (*pp)[i] != '.')))
    {
	*pp = skipwhite(*pp + i);
	أرجع TRUE;
    }
    أرجع FALSE;
}

/*
 * Check for an Ex command with optional tail.
 * If there is a match advance "pp" to the argument and return TRUE.
 */
    صحيح
checkforcmd(
    char_u	**pp,		// start of command
    حرف	*cmd,		// name of command
    صحيح		len)		// required length
{
    أرجع checkforcmd_opt(pp, cmd, len, FALSE);
}

/*
 * Check for an Ex command with optional tail, not followed by "(" or ".".
 * If there is a match advance "pp" to the argument and return TRUE.
 */
    صحيح
checkforcmd_noparen(
    char_u	**pp,		// start of command
    حرف	*cmd,		// name of command
    صحيح		len)		// required length
{
    أرجع checkforcmd_opt(pp, cmd, len, TRUE);
}

/*
 * Find the replacement text of a ":command", the part after the attributes and
 * the command name.  Returns NULL when there is none.  Does not modify "arg"
 * and gives no error messages.
 */
    ساكن char_u *
find_ucmd_repl(char_u *arg)
{
    char_u	*p = arg;

    // Skip over the attributes.
    طالما (*p == '-')
	p = skipwhite(skiptowhite(p));

    // Skip over the command name.
    إذا (!ASCII_ISALPHA(*p))
	أرجع عدم;
    طالما (ASCII_ISALNUM(*p))
	++p;

    أرجع *p == NUL ? عدم : skipwhite(p);
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

    لكل (;;)
    {
	char_u	*arg = p;

	إذا (*p == '{' && ends_excmd2(p, skipwhite(p + 1)))
	    أرجع p;

	إذا (checkforcmd_noparen(&arg, "autocmd", 2))
	{
	    إذا (*arg == '!')
		arg = skipwhite(arg + 1);
	    p = au_find_cmd_arg(arg);
	}
	وإلا إذا (checkforcmd_noparen(&arg, "command", 3))
	{
	    إذا (*arg == '!')
		arg = skipwhite(arg + 1);
	    p = find_ucmd_repl(arg);
	}
	وإلا
	    أرجع عدم;

	إذا (p == عدم)
	    أرجع عدم;
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
    صحيح
parse_command_modifiers(
	exarg_T	    *eap,
	حرف	    **errormsg,
	cmdmod_T    *cmod,
	صحيح	    skip_only)
{
    char_u  *orig_cmd = eap->cmd;
    char_u  *cmd_start = عدم;
    صحيح	    use_plus_cmd = FALSE;
    صحيح	    starts_with_colon = FALSE;
    صحيح	    vim9script = in_vim9script();
    صحيح	    has_visual_range = FALSE;

    CLEAR_POINTER(cmod);
    cmod->cmod_flags = sticky_cmdmod_flags;

    إذا (STRNCMP(eap->cmd, "'<,'>", 5) == 0)
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
    لكل (;;)
    {
	char_u  *p;

	طالما (*eap->cmd == ' ' || *eap->cmd == '\t' || *eap->cmd == ':')
	{
	    إذا (*eap->cmd == ':')
		starts_with_colon = TRUE;
	    ++eap->cmd;
	}

	// in ex mode, an empty command (after modifiers) works like :+
	إذا (*eap->cmd == NUL && exmode_active
		   && (getline_equal(eap->ea_getline, eap->cookie, getexmodeline)
		       || getline_equal(eap->ea_getline, eap->cookie, getexline))
			&& curwin->w_cursor.lnum < curbuf->b_ml.ml_سطر_گنتی)
	{
	    use_plus_cmd = TRUE;
	    إذا (!skip_only)
		ex_pressedreturn = TRUE;
	    اكسر;  // no modifiers following
	}

	// ignore comment and empty lines
	إذا (comment_start(eap->cmd, starts_with_colon))
	{
	    // a comment ends at a NL
	    eap->nextcmd = vim_strchr(eap->cmd, '\n');
	    إذا (eap->nextcmd != عدم)
		++eap->nextcmd;
	    إذا (vim9script)
	    {
		إذا (has_cmdmod(cmod, FALSE))
		    *errormsg = _(e_command_modifier_without_command);
#ifdef FEAT_EVAL
		إذا (eap->cmd[0] == '#' && eap->cmd[1] == '{'
							 && eap->cmd[2] != '{')
		    *errormsg = _(e_cannot_use_hash_curly_to_start_comment);
#endif
	    }
	    أرجع FAIL;
	}
	إذا (*eap->cmd == '\n')
	{
	    eap->nextcmd = eap->cmd + 1;
	    أرجع FAIL;
	}
	إذا (*eap->cmd == NUL)
	{
	    إذا (!skip_only)
	    {
		ex_pressedreturn = TRUE;
		إذا (vim9script && has_cmdmod(cmod, FALSE))
		    *errormsg = _(e_command_modifier_without_command);
	    }
	    أرجع FAIL;
	}

	p = skip_range(eap->cmd, TRUE, عدم);

	// In Vim9 script a variable can shadow a command modifier:
	//   verbose = 123
	//   verbose += 123
	//   silent! verbose = func()
	//   verbose.member = 2
	//   verbose[expr] = 2
	// But not:
	//   verbose [a, b] = list
	إذا (vim9script)
	{
	    char_u *s, *n;

	    لكل (s = eap->cmd; ASCII_ISALPHA(*s); ++s)
		;
	    n = skipwhite(s);
	    إذا (*n == '.' || *n == '=' || (*n != NUL && n[1] == '=')
		    || *s == '[')
		اكسر;
	}

	بدل (*p)
	{
	    // When adding an entry, also modify cmdmod_info_tab[].
	    حالة 'a':	إذا (!checkforcmd_noparen(&eap->cmd, "aboveleft", 3))
			    اكسر;
			cmod->cmod_split |= WSP_ABOVE;
			استمر;

	    حالة 'b':	إذا (checkforcmd_noparen(&eap->cmd, "belowright", 3))
			{
			    cmod->cmod_split |= WSP_BELOW;
			    استمر;
			}
			إذا (checkforcmd_opt(&eap->cmd, "browse", 3, TRUE))
			{
#ifdef FEAT_BROWSE_CMD
			    cmod->cmod_flags |= CMOD_BROWSE;
#endif
			    استمر;
			}
			إذا (!checkforcmd_noparen(&eap->cmd, "botright", 2))
			    اكسر;
			cmod->cmod_split |= WSP_BOT;
			استمر;

	    حالة 'c':	إذا (!checkforcmd_opt(&eap->cmd, "confirm", 4, TRUE))
			    اكسر;
#إذا defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
			cmod->cmod_flags |= CMOD_CONFIRM;
#endif
			استمر;

	    حالة 'k':	إذا (checkforcmd_noparen(&eap->cmd, "keepmarks", 3))
			{
			    cmod->cmod_flags |= CMOD_KEEPMARKS;
			    استمر;
			}
			إذا (checkforcmd_noparen(&eap->cmd, "keepalt", 5))
			{
			    cmod->cmod_flags |= CMOD_KEEPALT;
			    استمر;
			}
			إذا (checkforcmd_noparen(&eap->cmd, "keeppatterns", 5))
			{
			    cmod->cmod_flags |= CMOD_KEEPPATTERNS;
			    استمر;
			}
			إذا (!checkforcmd_noparen(&eap->cmd, "keepjumps", 5))
			    اكسر;
			cmod->cmod_flags |= CMOD_KEEPJUMPS;
			استمر;

	    حالة 'f':	// only accept ":filter {pat} cmd"
			{
			    char_u  *reg_pat;
			    char_u  *nulp = عدم;
			    صحيح	    c = 0;

			    إذا (!checkforcmd_noparen(&p, "filter", 4)
				    || *p == NUL
				    || (ends_excmd(*p)
#ifdef FEAT_EVAL
					// in ":filter #pat# cmd" # does not
					// start a comment
				     && (!vim9script || VIM_ISWHITE(p[1]))
#endif
				     ))
				اكسر;
			    إذا (*p == '!')
			    {
				cmod->cmod_filter_force = TRUE;
				p = skipwhite(p + 1);
				إذا (*p == NUL || ends_excmd(*p))
				    اكسر;
			    }
#ifdef FEAT_EVAL
			    // Avoid that "filter(arg)" is recognized.
			    إذا (vim9script && !VIM_ISWHITE(p[-1]))
				اكسر;
#endif
			    إذا (skip_only)
				p = skip_vimgrep_pat(p, عدم, عدم);
			    وإلا
				// NOTE: This puts a NUL after the pattern.
				p = skip_vimgrep_pat_ext(p, &reg_pat, عدم,
								    &nulp, &c);
			    إذا (p == عدم || *p == NUL)
				اكسر;
			    إذا (!skip_only)
			    {
				cmod->cmod_filter_regmatch.regprog =
						vim_regcomp(reg_pat, RE_MAGIC);
				إذا (cmod->cmod_filter_regmatch.regprog == عدم)
				    اكسر;
				// restore the character overwritten by NUL
				إذا (nulp != عدم)
				    *nulp = c;
			    }
			    eap->cmd = p;
			    استمر;
			}

	    حالة 'h':	إذا (checkforcmd_noparen(&eap->cmd, "horizontal", 3))
			{
			    cmod->cmod_split |= WSP_HOR;
			    استمر;
			}
			// ":hide" and ":hide | cmd" are not modifiers
			إذا (p != eap->cmd || !checkforcmd_noparen(&p, "hide", 3)
					       || *p == NUL || ends_excmd(*p))
			    اكسر;
			eap->cmd = p;
			cmod->cmod_flags |= CMOD_HIDE;
			استمر;

	    حالة 'l':	إذا (checkforcmd_noparen(&eap->cmd, "lockmarks", 3))
			{
			    cmod->cmod_flags |= CMOD_LOCKMARKS;
			    استمر;
			}
			إذا (checkforcmd_noparen(&eap->cmd, "legacy", 3))
			{
			    إذا (ends_excmd2(p, eap->cmd))
			    {
				*errormsg =
				      _(e_legacy_must_be_followed_by_command);
				أرجع FAIL;
			    }
			    // Make sure we do not have both legacy and Vim9
			    // flags set at the same time.
			    cmod->cmod_flags &= ~CMOD_VIM9CMD;
			    cmod->cmod_flags |= CMOD_LEGACY;
			    استمر;
			}

			إذا (!checkforcmd_noparen(&eap->cmd, "leftabove", 5))
			    اكسر;
			cmod->cmod_split |= WSP_ABOVE;
			استمر;

	    حالة 'n':	إذا (checkforcmd_noparen(&eap->cmd, "noautocmd", 3))
			{
			    cmod->cmod_flags |= CMOD_NOAUTOCMD;
			    استمر;
			}
			إذا (!checkforcmd_noparen(&eap->cmd, "noswapfile", 3))
			    اكسر;
			cmod->cmod_flags |= CMOD_NOSWAPFILE;
			استمر;

	    حالة 'r':	إذا (!checkforcmd_noparen(&eap->cmd, "rightbelow", 6))
			    اكسر;
			cmod->cmod_split |= WSP_BELOW;
			استمر;

	    حالة 's':	إذا (checkforcmd_noparen(&eap->cmd, "sandbox", 3))
			{
			    cmod->cmod_flags |= CMOD_SANDBOX;
			    استمر;
			}
			إذا (!checkforcmd_noparen(&eap->cmd, "silent", 3))
			    اكسر;
			cmod->cmod_flags |= CMOD_SILENT;
			إذا (*eap->cmd == '!' && !VIM_ISWHITE(eap->cmd[-1]))
			{
			    // ":silent!", but not "silent !cmd"
			    eap->cmd = skipwhite(eap->cmd + 1);
			    cmod->cmod_flags |= CMOD_ERRSILENT;
			}
			استمر;

	    حالة 't':	إذا (checkforcmd_noparen(&p, "tab", 3))
			{
			    إذا (!skip_only)
			    {
				long tabnr = get_address(eap, &eap->cmd,
						    ADDR_TABS, eap->skip,
						    skip_only, FALSE, 1);
				إذا (tabnr == MAXLNUM)
				    cmod->cmod_tab = tabpage_index(curtab) + 1;
				وإلا
				{
				    إذا (tabnr < 0 || tabnr > LAST_TAB_NR)
				    {
					*errormsg = _(e_invalid_range);
					أرجع FAIL;
				    }
				    cmod->cmod_tab = tabnr + 1;
				}
			    }
			    eap->cmd = p;
			    استمر;
			}
			إذا (!checkforcmd_noparen(&eap->cmd, "topleft", 2))
			    اكسر;
			cmod->cmod_split |= WSP_چوٹی;
			استمر;

	    حالة 'u':	إذا (!checkforcmd_noparen(&eap->cmd, "unsilent", 3))
			    اكسر;
			cmod->cmod_flags |= CMOD_UNSILENT;
			استمر;

	    حالة 'v':	إذا (checkforcmd_noparen(&eap->cmd, "vertical", 4))
			{
			    cmod->cmod_split |= WSP_VERT;
			    استمر;
			}
			إذا (checkforcmd_noparen(&eap->cmd, "vim9cmd", 4))
			{
			    إذا (ends_excmd2(p, eap->cmd))
			    {
				*errormsg =
				      _(e_vim9cmd_must_be_followed_by_command);
				أرجع FAIL;
			    }
			    // Make sure we do not have both legacy and Vim9
			    // flags set at the same time.
			    cmod->cmod_flags &= ~CMOD_LEGACY;
			    cmod->cmod_flags |= CMOD_VIM9CMD;
			    استمر;
			}
			إذا (!checkforcmd_noparen(&p, "verbose", 4))
			    اكسر;
			إذا (vim_isdigit(*eap->cmd))
			{
			    // zero means not set, one is verbose == 0, etc.
			    cmod->cmod_verbose = atoi((حرف *)eap->cmd) + 1;
			}
			وإلا
			    cmod->cmod_verbose = 2;  // default: verbose == 1
			eap->cmd = p;
			استمر;
	}
	اكسر;
    }

    إذا (has_visual_range)
    {
	إذا (eap->cmd > cmd_start)
	{
	    // Move the '<,'> range to after the modifiers and insert a colon.
	    // Since the modifiers have been parsed put the colon on top of the
	    // space: "'<,'>mod cmd" -> "mod:'<,'>cmd
	    // Put eap->cmd after the colon.
	    إذا (use_plus_cmd)
	    {
		size_t len = STRLEN(cmd_start);

		// Special case: empty command uses "+":
		//  "'<,'>mods" -> "mods *+
		//  Use "*" instead of "'<,'>" to avoid the command getting
		//  longer, in case it was allocated.
		mch_memmove(orig_cmd, cmd_start, len);
		STRCPY(orig_cmd + len, " *+");
	    }
	    وإلا
	    {
		mch_memmove(cmd_start - 5, cmd_start, eap->cmd - cmd_start);
		eap->cmd -= 5;
		mch_memmove(eap->cmd - 1, ":'<,'>", 6);
	    }
	}
	وإلا
	    // No modifiers, move the pointer back.
	    // Special case: change empty command to "+".
	    إذا (use_plus_cmd)
		eap->cmd = (char_u *)"'<,'>+";
	    وإلا
		eap->cmd = orig_cmd;
    }
    وإلا إذا (use_plus_cmd)
	eap->cmd = (char_u *)exmode_plus;

    أرجع OK;
}

/*
 * Return TRUE if "cmod" has anything set.
 */
    صحيح
has_cmdmod(cmdmod_T *cmod, صحيح ignore_silent)
{
    أرجع (cmod->cmod_flags != 0 && (!ignore_silent
		|| (cmod->cmod_flags
		      & ~(CMOD_SILENT | CMOD_ERRSILENT | CMOD_UNSILENT)) != 0))
	    || cmod->cmod_split != 0
	    || cmod->cmod_verbose > 0
	    || cmod->cmod_tab != 0
	    || cmod->cmod_filter_regmatch.regprog != عدم;
}

#إذا defined(FEAT_EVAL)
/*
 * If Vim9 script and "cmdmod" has anything set give an error and return TRUE.
 */
    صحيح
cmdmod_error(صحيح ignore_silent)
{
    إذا (in_vim9script() && has_cmdmod(&cmdmod, ignore_silent))
    {
	emsg(_(e_misplaced_command_modifier));
	أرجع TRUE;
    }
    أرجع FALSE;
}
#endif

/*
 * Apply the command modifiers.  Saves current state in "cmdmod", call
 * undo_cmdmod() later.
 */
    فراغ
apply_cmdmod(cmdmod_T *cmod)
{
#ifdef HAVE_SANDBOX
    إذا ((cmod->cmod_flags & CMOD_SANDBOX) && !cmod->cmod_did_sandbox)
    {
	++sandbox;
	cmod->cmod_did_sandbox = TRUE;
    }
#endif
    إذا (cmod->cmod_verbose > 0)
    {
	إذا (cmod->cmod_verbose_save == 0)
	    cmod->cmod_verbose_save = p_verbose + 1;
	p_verbose = cmod->cmod_verbose - 1;
    }

    إذا ((cmod->cmod_flags & (CMOD_SILENT | CMOD_UNSILENT))
	    && cmod->cmod_save_پیغام_silent == 0)
    {
	cmod->cmod_save_پیغام_silent = پیغام_silent + 1;
	cmod->cmod_save_پیغام_scroll = پیغام_scroll;
    }
    إذا (cmod->cmod_flags & CMOD_SILENT)
	++پیغام_silent;
    إذا (cmod->cmod_flags & CMOD_UNSILENT)
	پیغام_silent = 0;

    إذا (cmod->cmod_flags & CMOD_ERRSILENT)
    {
	++emsg_silent;
	++cmod->cmod_did_esilent;
    }

    إذا ((cmod->cmod_flags & CMOD_NOAUTOCMD) && cmod->cmod_save_ei == عدم)
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
    فراغ
undo_cmdmod(cmdmod_T *cmod)
{
    إذا (cmod->cmod_verbose_save > 0)
    {
	p_verbose = cmod->cmod_verbose_save - 1;
	cmod->cmod_verbose_save = 0;
    }

#ifdef HAVE_SANDBOX
    إذا (cmod->cmod_did_sandbox)
    {
	--sandbox;
	cmod->cmod_did_sandbox = FALSE;
    }
#endif

    إذا (cmod->cmod_save_ei != عدم)
    {
	// Restore 'eventignore' to the value before ":noautocmd".
	set_string_option_direct((char_u *)"ei", -1, cmod->cmod_save_ei,
							   OPT_FREE, SID_NONE);
	free_string_option(cmod->cmod_save_ei);
	cmod->cmod_save_ei = عدم;
    }

    vim_regfree(cmod->cmod_filter_regmatch.regprog);

    إذا (cmod->cmod_save_پیغام_silent > 0)
    {
	// messages could be enabled for a serious error, need to check if the
	// counters don't become negative
	إذا (!did_emsg || پیغام_silent > cmod->cmod_save_پیغام_silent - 1)
	    پیغام_silent = cmod->cmod_save_پیغام_silent - 1;
	emsg_silent -= cmod->cmod_did_esilent;
	إذا (emsg_silent < 0)
	    emsg_silent = 0;
	// Restore msg_scroll, it's set by file I/O commands, even when no
	// message is actually displayed.
	پیغام_scroll = cmod->cmod_save_پیغام_scroll;

	// "silent reg" or "silent echo x" inside "redir" leaves msg_col
	// somewhere in the line.  Put it back in the first column.
	إذا (redirecting())
	    پیغام_col = 0;

	cmod->cmod_save_پیغام_silent = 0;
	cmod->cmod_did_esilent = 0;
    }
}

/*
 * Parse the address range, if any, in "eap".
 * May set the last search pattern, unless "silent" is TRUE.
 * Return FAIL and set "errormsg" or return OK.
 */
    صحيح
parse_cmd_address(exarg_T *eap, حرف **errormsg, صحيح silent)
{
    صحيح		address_گنتی = 1;
    linenr_T	lnum;
    صحيح		need_check_cursor = FALSE;
    صحيح		ret = FAIL;

    // Repeat for all ',' or ';' separated addresses.
    لكل (;;)
    {
	eap->line1 = eap->line2;
	eap->line2 = default_address(eap);
	eap->cmd = skipwhite(eap->cmd);
	lnum = get_address(eap, &eap->cmd, eap->addr_type, eap->skip, silent,
					eap->addr_گنتی == 0, address_گنتی++);
	إذا (eap->cmd == عدم)	// error detected
	    اذهب theend;
	إذا (lnum == MAXLNUM)
	{
	    إذا (*eap->cmd == '%')   // '%' - all lines
	    {
		++eap->cmd;
		بدل (eap->addr_type)
		{
		    حالة ADDR_سطریں:
		    حالة ADDR_OTHER:
			eap->line1 = 1;
			eap->line2 = curbuf->b_ml.ml_سطر_گنتی;
			اكسر;
		    حالة ADDR_LOADED_BUFFERS:
			{
			    buf_T	*buf = firstbuf;

			    طالما (buf->b_next != عدم
						  && buf->b_ml.ml_mfp == عدم)
				buf = buf->b_next;
			    eap->line1 = buf->b_fnum;
			    buf = lastbuf;
			    طالما (buf->b_prev != عدم
						  && buf->b_ml.ml_mfp == عدم)
				buf = buf->b_prev;
			    eap->line2 = buf->b_fnum;
			    اكسر;
			}
		    حالة ADDR_BUFFERS:
			eap->line1 = firstbuf->b_fnum;
			eap->line2 = lastbuf->b_fnum;
			اكسر;
		    حالة ADDR_WINDOWS:
		    حالة ADDR_TABS:
			إذا (IS_USER_CMDIDX(eap->cmdidx))
			{
			    eap->line1 = 1;
			    eap->line2 = eap->addr_type == ADDR_WINDOWS
						  ? LAST_WIN_NR : LAST_TAB_NR;
			}
			وإلا
			{
			    // there is no Vim command which uses '%' and
			    // ADDR_WINDOWS or ADDR_TABS
			    *errormsg = _(e_invalid_range);
			    اذهب theend;
			}
			اكسر;
		    حالة ADDR_TABS_RELATIVE:
		    حالة ADDR_UNSIGNED:
		    حالة ADDR_QUICKFIX:
			*errormsg = _(e_invalid_range);
			اذهب theend;
		    حالة ADDR_ARGUMENTS:
			إذا (ARGCOUNT == 0)
			    eap->line1 = eap->line2 = 0;
			وإلا
			{
			    eap->line1 = 1;
			    eap->line2 = ARGCOUNT;
			}
			اكسر;
		    حالة ADDR_QUICKFIX_VALID:
#ifdef FEAT_QUICKFIX
			eap->line1 = 1;
			eap->line2 = qf_get_valid_size(eap);
			إذا (eap->line2 == 0)
			    eap->line2 = 1;
#endif
			اكسر;
		    حالة ADDR_NONE:
			// Will give an error later if a range is found.
			اكسر;
		}
		++eap->addr_گنتی;
	    }
	    وإلا إذا (*eap->cmd == '*' && vim_strchr(p_cpo, CPO_STAR) == عدم)
	    {
		pos_T	    *fp;

		// '*' - visual area
		إذا (eap->addr_type != ADDR_سطریں)
		{
		    *errormsg = _(e_invalid_range);
		    اذهب theend;
		}

		++eap->cmd;
		إذا (!eap->skip)
		{
		    fp = getmark('<', FALSE);
		    إذا (check_mark(fp) == FAIL)
			اذهب theend;
		    eap->line1 = fp->lnum;
		    fp = getmark('>', FALSE);
		    إذا (check_mark(fp) == FAIL)
			اذهب theend;
		    eap->line2 = fp->lnum;
		    ++eap->addr_گنتی;
		}
	    }
	}
	وإلا
	    eap->line2 = lnum;
	eap->addr_گنتی++;

	إذا (*eap->cmd == ';')
	{
	    إذا (!eap->skip)
	    {
		curwin->w_cursor.lnum = eap->line2;

		// Don't leave the cursor on an illegal line or column, but do
		// accept zero as address, so 0;/PATTERN/ works correctly
		// (where zero usually means to use the first line).
		// Check the cursor position before returning.
		إذا (eap->line2 > 0)
		    check_cursor();
		وإلا
		    check_cursor_col();
		need_check_cursor = TRUE;
	    }
	}
	وإلا إذا (*eap->cmd != ',')
	    اكسر;
	++eap->cmd;
    }

    // One address given: set start and end lines.
    إذا (eap->addr_گنتی == 1)
    {
	eap->line1 = eap->line2;
	// ... but only implicit: really no address given
	إذا (lnum == MAXLNUM)
	    eap->addr_گنتی = 0;
    }
    ret = OK;

theend:
    إذا (need_check_cursor)
	check_cursor();
    أرجع ret;
}

/*
 * Append "cmd" to the error message in IObuff.
 * Takes care of limiting the length and handling 0xa0, which would be
 * invisible otherwise.
 */
    ساكن فراغ
append_command(char_u *cmd)
{
    size_t  len = STRLEN(IObuff);
    char_u  *s = cmd;
    char_u  *d;

    إذا (len > IOSIZE - 100)
    {
	// Not enough space, truncate and put in "...".
	d = IObuff + IOSIZE - 100;
	d -= mb_head_off(IObuff, d);
	STRCPY(d, "...");
    }
    STRCAT(IObuff, ": ");
    d = IObuff + STRLEN(IObuff);
    طالما (*s != NUL && d - IObuff + 5 < IOSIZE)
    {
	إذا (enc_utf8 ? (s[0] == 0xc2 && s[1] == 0xa0) : *s == 0xa0)
	{
	    s += enc_utf8 ? 2 : 1;
	    STRCPY(d, "<a0>");
	    d += 4;
	}
	وإلا إذا (d - IObuff + (*mb_ptr2len)(s) + 1 >= IOSIZE)
	    اكسر;
	وإلا
	    MB_COPY_CHAR(s, d);
    }
    *d = NUL;
}

#إذا defined(FEAT_EVAL)
/*
 * If "start" points "&opt", "&l:opt", "&g:opt" or "$ENV" return a pointer to
 * the name.  Otherwise just return "start".
 */
    char_u *
skip_option_env_lead(char_u *start)
{
    char_u *name = start;

    إذا (*start == '&')
    {
	إذا ((start[1] == 'l' || start[1] == 'g') && start[2] == ':')
	    name += 3;
	وإلا
	    name += 1;
    }
    وإلا إذا (*start == '$')
	name += 1;
    أرجع name;
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
    ساكن صحيح
one_letter_cmd(char_u *p, cmdidx_T *idx)
{
    إذا (in_vim9script())
	أرجع FALSE;
    إذا (p[0] == 'k'
	    && (p[1] != 'e' || (p[1] == 'e' && p[2] != 'e')))
    {
	*idx = CMD_k;
	أرجع TRUE;
    }
    إذا (p[0] == 's'
	    && ((p[1] == 'c' && (p[2] == NUL || (p[2] != 's' && p[2] != 'r'
			&& (p[3] == NUL || (p[3] != 'i' && p[4] != 'p')))))
		|| p[1] == 'g'
		|| (p[1] == 'i' && p[2] != 'm' && p[2] != 'l' && p[2] != 'g')
		|| p[1] == 'I'
		|| (p[1] == 'r' && p[2] != 'e')))
    {
	*idx = CMD_substitute;
	أرجع TRUE;
    }
    أرجع FALSE;
}

#إذا defined(FEAT_EVAL)
/*
 * Return TRUE if "cmd" starts with "123->", a number followed by a method
 * call.
 */
    صحيح
نمبر_method(char_u *cmd)
{
    char_u *p = skipdigits(cmd);

    أرجع p > cmd && (p = skipwhite(p))[0] == '-' && p[1] == '>';
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
	صحيح	*full UNUSED,
	صحيح	(*lookup)(char_u *, size_t, صحيح cmd, cctx_T *) UNUSED,
	cctx_T	*cctx UNUSED)
{
    صحيح		len;
    char_u	*p;
    صحيح		i;
#ifndef FEAT_EVAL
    صحيح		vim9 = FALSE;
#وإلا
    صحيح		vim9 = in_vim9script();

    /*
     * Recognize a Vim9 script function/method call and assignment:
     * "lvar = value", "lvar(arg)", "[1, 2 3]->Func()"
     */
    p = eap->cmd;
    إذا (lookup != عدم)
    {
	char_u *pskip = skip_option_env_lead(eap->cmd);

	إذا (vim_strchr((char_u *)"{('[\"@&$", *p) != عدم
	       || ((p = to_name_const_end(pskip)) > eap->cmd && *p != NUL)
	       || (p[0] == '0' && p[1] == 'z'))
	{
	    صحيح	    oplen;
	    صحيح	    heredoc;
	    char_u  *swp;

	    إذا (*eap->cmd == '&'
		    || (eap->cmd[0] == '$'
				  && eap->cmd[1] != '\'' && eap->cmd[1] != '"')
		    || (eap->cmd[0] == '@'
					&& (valid_yank_reg(eap->cmd[1], FALSE)
						       || eap->cmd[1] == '@')))
	    {
		إذا (*eap->cmd == '&')
		{
		    p = eap->cmd + 1;
		    إذا (STRNCMP("l:", p, 2) == 0 || STRNCMP("g:", p, 2) == 0)
			p += 2;
		    p = to_name_end(p, FALSE);
		}
		وإلا إذا (*eap->cmd == '$')
		    p = to_name_end(eap->cmd + 1, FALSE);
		وإلا
		    p = eap->cmd + 2;
		إذا (ends_excmd(*skipwhite(p)))
		{
		    // "&option <NL>", "$ENV <NL>" and "@r <NL>" are the start
		    // of an expression.
		    eap->cmdidx = CMD_eval;
		    أرجع eap->cmd;
		}
		// "&option" can be followed by "->" or "=", check below
	    }

	    إذا (vim9 && *p == '<')
	    {
		// generic function type args
		إذا (skip_generic_func_type_args(&p) == FAIL)
		{
		    eap->cmdidx = CMD_SIZE;
		    أرجع p;
		}
	    }

	    swp = skipwhite(p);

	    إذا (
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
		إذا (*eap->cmd == '{' && ends_excmd(*skipwhite(eap->cmd + 1)))
		{
		    // "{" by itself is the start of a block.
		    eap->cmdidx = CMD_block;
		    أرجع eap->cmd + 1;
		}
		eap->cmdidx = CMD_eval;
		أرجع eap->cmd;
	    }

	    إذا ((p != eap->cmd && (
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
		إذا (skip_اظہاریہ(&after, عدم) == OK)
		{
		    after = skipwhite(after);
		    إذا (*after == '=' || (*after != NUL && after[1] == '=')
					 || (after[0] == '.' && after[1] == '.'
							   && after[2] == '='))
			eap->cmdidx = CMD_var;
		}
		--emsg_off;
		أرجع eap->cmd;
	    }

	    // "[...]->Method()" is a list expression, but "[a, b] = Func()" is
	    // an assignment.
	    // If there is no line break inside the "[...]" then "p" is
	    // advanced to after the "]" by to_name_const_end(): check if a "="
	    // follows.
	    // If "[...]" has a line break "p" still points at the "[" and it
	    // can't be an assignment.
	    إذا (*eap->cmd == '[')
	    {
		char_u	    *eq;

		p = to_name_const_end(eap->cmd);
		إذا (p == eap->cmd && *p == '[')
		{
		    صحيح گنتی = 0;
		    صحيح	semicolon = FALSE;

		    p = skip_var_list(eap->cmd, TRUE, &گنتی, &semicolon, TRUE);
		}
		eq = p;
		إذا (eq != عدم)
		{
		    eq = skipwhite(eq);
		    إذا (vim_strchr((char_u *)"+-*/%.", *eq) != عدم)
		    {
			إذا (eq[0] == '.' && eq[1] == '.')
			    ++eq;
			++eq;
		    }
		}
		إذا (p == عدم || p == eap->cmd || *eq != '=')
		{
		    eap->cmdidx = CMD_eval;
		    أرجع eap->cmd;
		}
		إذا (p > eap->cmd && *eq == '=')
		{
		    eap->cmdidx = CMD_var;
		    أرجع eap->cmd;
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
	    إذا (oplen > 0)
	    {
		إذا (((p - eap->cmd) > 2 && eap->cmd[1] == ':')
			|| *eap->cmd == '&'
			|| *eap->cmd == '$'
			|| *eap->cmd == '@'
			|| (eap->skip && IS_WHITE_OR_NUL(
						      *(skipwhite(p) + oplen)))
			|| lookup(eap->cmd, p - eap->cmd, TRUE, cctx) == OK)
		{
		    eap->cmdidx = CMD_var;
		    أرجع eap->cmd;
		}
	    }

	    // Recognize trying to use a type for a w:, b:, t: or g: variable:
	    // "w:varname: number = 123".
	    إذا (eap->cmd[1] == ':' && *p == ':')
	    {
		eap->cmdidx = CMD_var;
		أرجع eap->cmd;
	    }
	}

	// 1234->func() is a method call
	إذا (نمبر_method(eap->cmd))
	{
	    eap->cmdidx = CMD_eval;
	    أرجع eap->cmd;
	}

	// "g:", "s:" and "l:" are always assumed to be a variable, thus start
	// an expression.  A global/substitute/list command needs to use a
	// longer name.
	إذا (vim_strchr((char_u *)"gsl", *p) != عدم && p[1] == ':')
	{
	    eap->cmdidx = CMD_eval;
	    أرجع eap->cmd;
	}

	// If it is an ID it might be a variable with an operator on the next
	// line, if the variable exists it can't be an Ex command.
	إذا (p > eap->cmd && ends_excmd(*skipwhite(p))
		&& (lookup(eap->cmd, p - eap->cmd, TRUE, cctx) == OK
		    || (ASCII_ISALPHA(eap->cmd[0]) && eap->cmd[1] == ':')))
	{
	    eap->cmdidx = CMD_eval;
	    أرجع eap->cmd;
	}

	// Check for "++nr" and "--nr".
	إذا (p == eap->cmd && p[0] != NUL && p[0] == p[1]
						   && (*p == '+' || *p == '-'))
	{
	    eap->cmdidx = *p == '+' ? CMD_increment : CMD_decrement;
	    أرجع eap->cmd + 2;
	}
    }
#endif

    /*
     * Isolate the command and search for it in the command table.
     */
    p = eap->cmd;
    إذا (one_letter_cmd(p, &eap->cmdidx))
    {
	++p;
	إذا (full != عدم)
	    *full = TRUE;
    }
    وإلا
    {
	طالما (ASCII_ISALPHA(*p))
	    ++p;
	// for python 3.x support ":py3", ":python3", ":py3file", etc.
	إذا (eap->cmd[0] == 'p' && eap->cmd[1] == 'y')
	{
	    طالما (ASCII_ISALNUM(*p))
		++p;
	}
	وإلا إذا (*p == '9' && STRNCMP("vim9", eap->cmd, 4) == 0)
	{
	    // include "9" for "vim9*" commands; "vim9cmd" and "vim9script".
	    ++p;
	    طالما (ASCII_ISALPHA(*p))
		++p;
	}

	// check for non-alpha command
	إذا (p == eap->cmd && vim_strchr((char_u *)"@*!=><&~#}", *p) != عدم)
	    ++p;
	len = (صحيح)(p - eap->cmd);
	// The "d" command can directly be followed by 'l' or 'p' flag, when
	// not in Vim9 script.
	إذا (!vim9 && *eap->cmd == 'd' && (p[-1] == 'l' || p[-1] == 'p'))
	{
	    // Check for ":dl", ":dell", etc. to ":deletel": that's
	    // :delete with the 'l' flag.  Same for 'p'.
	    لكل (i = 0; i < len; ++i)
		إذا (eap->cmd[i] != ((char_u *)"delete")[i])
		    اكسر;
	    إذا (i == len - 1)
	    {
		--len;
		إذا (p[-1] == 'l')
		    eap->flags |= EXFLAG_LIST;
		وإلا
		    eap->flags |= EXFLAG_PRINT;
	    }
	}

	إذا (ASCII_ISLOWER(eap->cmd[0]))
	{
	    صحيح c1 = eap->cmd[0];
	    صحيح c2 = len == 1 ? NUL : eap->cmd[1];

	    إذا (command_گنتی != (صحيح)CMD_SIZE)
	    {
		iemsg(e_command_table_needs_to_be_updated_run_make_cmdidxs);
		getout(1);
	    }

	    // Use a precomputed index for fast look-up in cmdnames[]
	    // taking into account the first 2 letters of eap->cmd.
	    eap->cmdidx = cmdidxs1[Char_Ord_کم(c1)];
	    إذا (ASCII_ISLOWER(c2))
		eap->cmdidx += cmdidxs2[Char_Ord_کم(c1)][Char_Ord_کم(c2)];
	}
	وإلا إذا (ASCII_ISUPPER(eap->cmd[0]))
	    eap->cmdidx = CMD_Next;
	وإلا
	    eap->cmdidx = CMD_bang;

	لكل ( ; (صحيح)eap->cmdidx < (صحيح)CMD_SIZE;
			       eap->cmdidx = (cmdidx_T)((صحيح)eap->cmdidx + 1))
	    إذا (STRNCMP(cmdnames[(صحيح)eap->cmdidx].cmd_name, (حرف *)eap->cmd,
							    (size_t)len) == 0)
	    {
#ifdef FEAT_EVAL
		إذا (full != عدم && cmdnames[eap->cmdidx].cmd_name[len] == NUL)
		    *full = TRUE;
#endif
		اكسر;
	    }

	// :Print and :mode are not supported in Vim9 script.
	// Some commands cannot be shortened in Vim9 script.
	إذا (vim9 && eap->cmdidx != CMD_SIZE)
	{
	    إذا (eap->cmdidx == CMD_mode || eap->cmdidx == CMD_Print)
		eap->cmdidx = CMD_SIZE;
	    وإلا إذا ((cmdnames[eap->cmdidx].cmd_argt & EX_WHOLE)
			  && len < (صحيح)cmdnames[eap->cmdidx].cmd_namelen)
	    {
		semsg(_(e_command_cannot_be_shortened_str), eap->cmd);
		eap->cmdidx = CMD_SIZE;
	    }
	}

	// Do not recognize ":*" as the star command unless '*' is in
	// 'cpoptions'.
	إذا (eap->cmdidx == CMD_star && vim_strchr(p_cpo, CPO_STAR) == عدم)
	    p = eap->cmd;

	// Look for a user defined command as a last resort.  Let ":Print" be
	// overruled by a user defined command.
	إذا ((eap->cmdidx == CMD_SIZE || eap->cmdidx == CMD_Print)
		&& *eap->cmd >= 'A' && *eap->cmd <= 'Z')
	{
	    // User defined commands may contain digits.
	    طالما (ASCII_ISALNUM(*p))
		++p;
	    p = find_ucmd(eap, p, full, عدم, عدم);
	}
	إذا (p == عدم || p == eap->cmd)
	    eap->cmdidx = CMD_SIZE;
    }

    // ":fina" means ":finally" in legacy script, for backwards compatibility.
    إذا (eap->cmdidx == CMD_final && p - eap->cmd == 4 && !vim9)
	eap->cmdidx = CMD_finally;

     // Force ":ho" to be unresolved.  Without this, find_ex_command()
     // matches it to CMD_horizontal (the only "ho*" entry), which makes
     // fullcommand("ho") return "horizontal" even though ":ho" cannot be
     // used as the modifier (cmdmods[] requires 3 chars, "hor").
    إذا (eap->cmdidx == CMD_horizontal && p - eap->cmd == 2)
	eap->cmdidx = CMD_SIZE;

#ifdef FEAT_EVAL
    إذا (eap->cmdidx < CMD_SIZE
	    && vim9
	    && !IS_WHITE_NL_OR_NUL(*p) && *p != '!' && *p != '|'
	    && (eap->cmdidx < 0 ||
		(cmdnames[eap->cmdidx].cmd_argt & EX_NONWHITE_OK) == 0))
    {
	// A name that goes on with an underscore is not this command with an
	// argument: "ch_log" is not ":change".  Leave it to be reported as an
	// invalid command.
	إذا (*p != '_')
	{
	    char_u *cmd = vim_strnsave(eap->cmd, p - eap->cmd);

	    semsg(_(e_command_str_not_followed_by_white_space_str), cmd,
								    eap->cmd);
	    vim_free(cmd);
	}
	eap->cmdidx = CMD_SIZE;
    }
#endif

    أرجع p;
}

#إذا defined(FEAT_EVAL)
تعريفنوع بنية
{
    حرف	*name;
    صحيح		minlen;
    صحيح		has_گنتی;  // :123verbose  :3tab
} cmdmod_info_T;

ساكن cmdmod_info_T cmdmod_info_tab[] = {
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
    صحيح
modifier_len(char_u *cmd)
{
    صحيح		i, j;
    char_u	*p = cmd;

    إذا (VIM_ISDIGIT(*cmd))
	p = skipwhite(skipdigits(cmd + 1));
    لكل (i = 0; i < (صحيح)ARRAY_LENGTH(cmdmod_info_tab); ++i)
    {
	// cmdmod_info_tab[] is sorted by name: once the first letter is past
	// the command's first letter no later entry can match.
	إذا (cmdmod_info_tab[i].name[0] > *p)
	    اكسر;
	لكل (j = 0; p[j] != NUL; ++j)
	    إذا (p[j] != cmdmod_info_tab[i].name[j])
		اكسر;
	إذا (!ASCII_ISALPHA(p[j]) && j >= cmdmod_info_tab[i].minlen
					&& (p == cmd || cmdmod_info_tab[i].has_گنتی))
	    أرجع j + (صحيح)(p - cmd);
    }
    أرجع 0;
}

/*
 * Return > 0 if an Ex command "name" exists.
 * Return 2 if there is an exact match.
 * Return 3 if there is an ambiguous match.
 */
    صحيح
cmd_exists(char_u *name)
{
    exarg_T	ea;
    صحيح		full = FALSE;
    صحيح		i;
    صحيح		j;
    char_u	*p;

    // Check command modifiers.
    لكل (i = 0; i < (صحيح)ARRAY_LENGTH(cmdmod_info_tab); ++i)
    {
	لكل (j = 0; name[j] != NUL; ++j)
	    إذا (name[j] != cmdmod_info_tab[i].name[j])
		اكسر;
	إذا (name[j] == NUL && j >= cmdmod_info_tab[i].minlen)
	    أرجع (cmdmod_info_tab[i].name[j] == NUL ? 2 : 1);
    }

    // Check built-in commands and user defined commands.
    // For ":2match" and ":3match" we need to skip the number.
    ea.cmd = (*name == '2' || *name == '3') ? name + 1 : name;
    ea.cmdidx = (cmdidx_T)0;
    ea.flags = 0;
    p = find_ex_command(&ea, &full, عدم, عدم);
    إذا (p == عدم)
	أرجع 3;
    إذا (vim_isdigit(*name) && ea.cmdidx != CMD_match)
	أرجع 0;
    إذا (*skipwhite(p) != NUL)
	أرجع 0;	// trailing garbage
    أرجع (ea.cmdidx == CMD_SIZE ? 0 : (full ? 2 : 1));
}

/*
 * Add the attributes of an Ex command to "d" in the terms of ":command":
 * "argt" are the EX_ flags, "addr_type" the address type and "def" the
 * default for a range or count, -1 when there is none.
 */
    فراغ
ex_command_attrs(dict_T *d, long_u argt, cmd_addr_T addr_type, long def)
{
    ساكن حرف *addr_names[] = {
	"lines", "windows", "arguments", "loaded_buffers", "buffers", "tabs",
	"tabs_relative", "quickfix_valid", "quickfix", "unsigned", "other",
	"none"
    };
    حرف	*nargs;

    إذا (!(argt & EX_EXTRA))
	nargs = "0";
    وإلا إذا (!(argt & EX_NOSPC))
	nargs = (argt & EX_NEEDARG) ? "+" : "*";
    وإلا إذا (!(argt & EX_NEEDARG))
	nargs = "?";
    وإلا
	nargs = (argt & EX_ARGSPACE) ? "_" : "1";
    dict_add_string(d, "nargs", (char_u *)nargs);

    إذا (!(argt & EX_RANGE))
	dict_add_bool(d, "range", FALSE);
    وإلا إذا (argt & EX_DFLALL)
	dict_add_string(d, "range", (char_u *)"%");
    وإلا إذا (def >= 0 && !(argt & EX_گنتی))
	dict_add_نمبر(d, "range", def);
    وإلا
	dict_add_string(d, "range", (char_u *)".");
    إذا (!(argt & EX_گنتی))
	dict_add_bool(d, "count", FALSE);
    وإلا
	dict_add_نمبر(d, "count", def < 0 ? 0 : def);
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
    صحيح
ex_command_info(char_u *name, صحيح vim9, dict_T *d)
{
    exarg_T	ea;
    char_u	*p;
    صحيح		i;
    صحيح		j;
    صحيح		save_cmod_flags = cmdmod.cmod_flags;
    صحيح		ret = OK;

    إذا (vim9 != -1)
    {
	cmdmod.cmod_flags &= ~(CMOD_VIM9CMD | CMOD_LEGACY);
	cmdmod.cmod_flags |= vim9 ? CMOD_VIM9CMD : CMOD_LEGACY;
    }

    // An abbreviation of a command modifier, like ":bo" for ":botright".
    لكل (i = 0; i < (صحيح)ARRAY_LENGTH(cmdmod_info_tab); ++i)
    {
	لكل (j = 0; name[j] != NUL; ++j)
	    إذا (name[j] != cmdmod_info_tab[i].name[j])
		اكسر;
	إذا (name[j] == NUL && j >= cmdmod_info_tab[i].minlen)
	{
	    name = (char_u *)cmdmod_info_tab[i].name;
	    اكسر;
	}
    }

    // For ":2match" and ":3match" we need to skip the number.
    ea.cmd = (*name == '2' || *name == '3') ? name + 1 : name;
    ea.cmdidx = (cmdidx_T)0;
    ea.flags = 0;
    ea.addr_گنتی = 0;
    // Don't complain about using "en" or ":let" in Vim9 script.
    ++emsg_silent;
    p = find_ex_command(&ea, عدم, عدم, عدم);
    إذا (p == عدم || ea.cmdidx == CMD_SIZE
	    || (vim_isdigit(*name) && ea.cmdidx != CMD_match)
	    || *skipwhite(p) != NUL
	    || not_in_vim9(&ea, true) == FAIL)
	ret = FAIL;
    --emsg_silent;

    إذا (ret == FAIL)
	;
    وإلا إذا (IS_USER_CMDIDX(ea.cmdidx))
	ret = user_command_info(ea.useridx, ea.cmdidx, d);
    وإلا
    {
	صحيح	notimpl = cmdnames[ea.cmdidx].cmd_func == ex_ni
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
    أرجع ret;
}

/*
 * "fullcommand" function
 */
    فراغ
f_fullcommand(typval_T *argvars, typval_T *rettv)
{
    exarg_T	ea;
    char_u	*name;
    char_u	*p;
    صحيح		vim9script = in_vim9script();
    صحيح		save_cmod_flags = cmdmod.cmod_flags;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = عدم;

    إذا (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL))
	أرجع;

    name = tv_get_string(&argvars[0]);
    إذا (name == عدم)
	أرجع;

    إذا (argvars[1].v_type != VAR_UNKNOWN)
    {
	vim9script = tv_get_bool(&argvars[1]);
	cmdmod.cmod_flags &= ~(CMOD_VIM9CMD | CMOD_LEGACY);
	cmdmod.cmod_flags |= vim9script ? CMOD_VIM9CMD : CMOD_LEGACY;
    }

    طالما (*name == ':')
	name++;
    name = skip_range(name, TRUE, عدم);

    ea.cmd = (*name == '2' || *name == '3') ? name + 1 : name;
    ea.cmdidx = (cmdidx_T)0;
    ea.addr_گنتی = 0;
    ++emsg_silent;  // don't complain about using "en" in Vim9 script
    p = find_ex_command(&ea, عدم, عدم, عدم);
    --emsg_silent;
    إذا (p == عدم || ea.cmdidx == CMD_SIZE)
	اذهب theend;

    إذا (vim9script && not_in_vim9(&ea, true) == FAIL)
	اذهب theend;

    rettv->vval.v_string = vim_strsave(IS_USER_CMDIDX(ea.cmdidx)
				 ? get_user_command_name(ea.useridx, ea.cmdidx)
				 : cmdnames[ea.cmdidx].cmd_name);
theend:
    cmdmod.cmod_flags = save_cmod_flags;
}
#endif

    cmdidx_T
excmd_get_cmdidx(char_u *cmd, صحيح len)
{
    cmdidx_T idx;

    إذا (!one_letter_cmd(cmd, &idx))
	لكل (idx = (cmdidx_T)0; (صحيح)idx < (صحيح)CMD_SIZE;
		idx = (cmdidx_T)((صحيح)idx + 1))
	    إذا (STRNCMP(cmdnames[(صحيح)idx].cmd_name, cmd, (size_t)len) == 0)
		اكسر;

    أرجع idx;
}

    long
excmd_get_argt(cmdidx_T idx)
{
    أرجع (long)cmdnames[(صحيح)idx].cmd_argt;
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
    صحيح		skip_star,	// skip "*" used for Visual range
    صحيح		*ctx)		// pointer to xp_context or NULL
{
    char_u	*cmd = cmd_start;
    unsigned	delim;

    طالما (vim_strchr((char_u *)" \t0123456789.$%'/?-+,;\\", *cmd) != عدم)
    {
	إذا (*cmd == '\\')
	{
	    إذا (cmd[1] == '?' || cmd[1] == '/' || cmd[1] == '&')
		++cmd;
	    وإلا
		اكسر;
	}
	وإلا إذا (*cmd == '\'')
	{
	    char_u *p = cmd;

	    // a quote is only valid at the start or after a separator
	    طالما (p > cmd_start)
	    {
		--p;
		إذا (!VIM_ISWHITE(*p))
		    اكسر;
	    }
	    إذا (cmd > cmd_start && !VIM_ISWHITE(*p) && *p != ',' && *p != ';')
		اكسر;
	    إذا (*++cmd == NUL && ctx != عدم)
		*ctx = EXPAND_NOTHING;
	}
	وإلا إذا (*cmd == '/' || *cmd == '?')
	{
	    delim = *cmd++;
	    طالما (*cmd != NUL && *cmd != delim)
		إذا (*cmd++ == '\\' && *cmd != NUL)
		    ++cmd;
	    إذا (*cmd == NUL && ctx != عدم)
		*ctx = EXPAND_NOTHING;
	}
	إذا (*cmd != NUL)
	    ++cmd;
    }

    // Skip ":" and white space.
    طالما (*cmd == ':')
	cmd = skipwhite(cmd + 1);

    // Skip "*" used for Visual range.
    إذا (skip_star && *cmd == '*' && vim_strchr(p_cpo, CPO_STAR) == عدم)
	cmd = skipwhite(cmd + 1);

    أرجع cmd;
}

    ساكن فراغ
addr_error(cmd_addr_T addr_type)
{
    إذا (addr_type == ADDR_NONE)
	emsg(_(e_no_range_allowed));
    وإلا
	emsg(_(e_invalid_range));
}

/*
 * Return the default address for an address type.
 */
    ساكن linenr_T
default_address(exarg_T *eap)
{
    linenr_T lnum = 0;

    بدل (eap->addr_type)
    {
	حالة ADDR_سطریں:
	حالة ADDR_OTHER:
	    // Default is the cursor line number.  Avoid using an invalid
	    // line number though.
	    إذا (curwin->w_cursor.lnum > curbuf->b_ml.ml_سطر_گنتی)
		lnum = curbuf->b_ml.ml_سطر_گنتی;
	    وإلا
		lnum = curwin->w_cursor.lnum;
	    اكسر;
	حالة ADDR_WINDOWS:
	    lnum = CURRENT_WIN_NR;
	    اكسر;
	حالة ADDR_ARGUMENTS:
	    lnum = curwin->w_arg_idx + 1;
	    إذا (lnum > ARGCOUNT)
		lnum = ARGCOUNT;
	    اكسر;
	حالة ADDR_LOADED_BUFFERS:
	حالة ADDR_BUFFERS:
	    lnum = curbuf->b_fnum;
	    اكسر;
	حالة ADDR_TABS:
	    lnum = CURRENT_TAB_NR;
	    اكسر;
	حالة ADDR_TABS_RELATIVE:
	حالة ADDR_UNSIGNED:
	    lnum = 1;
	    اكسر;
	حالة ADDR_QUICKFIX:
#ifdef FEAT_QUICKFIX
	    lnum = qf_get_cur_idx(eap);
#endif
	    اكسر;
	حالة ADDR_QUICKFIX_VALID:
#ifdef FEAT_QUICKFIX
	    lnum = qf_get_cur_valid_idx(eap);
#endif
	    اكسر;
	حالة ADDR_NONE:
	    // Will give an error later if a range is found.
	    اكسر;
    }
    أرجع lnum;
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
    صحيح		skip,		// only skip the address, don't use it
    صحيح		silent,		// no errors or side effects
    صحيح		to_other_file,  // flag: may jump to other file
    صحيح		address_گنتی UNUSED) // 1 for first address, >1 after comma
{
    صحيح		c;
    صحيح		i;
    long	n;
    char_u	*cmd;
    pos_T	pos;
    pos_T	*fp;
    linenr_T	lnum;
    buf_T	*buf;

    cmd = skipwhite(*ptr);
    lnum = MAXLNUM;
    افعل
    {
	بدل (*cmd)
	{
	    حالة '.':			    // '.' - Cursor position
		++cmd;
		بدل (addr_type)
		{
		    حالة ADDR_سطریں:
		    حالة ADDR_OTHER:
			lnum = curwin->w_cursor.lnum;
			اكسر;
		    حالة ADDR_WINDOWS:
			lnum = CURRENT_WIN_NR;
			اكسر;
		    حالة ADDR_ARGUMENTS:
			lnum = curwin->w_arg_idx + 1;
			اكسر;
		    حالة ADDR_LOADED_BUFFERS:
		    حالة ADDR_BUFFERS:
			lnum = curbuf->b_fnum;
			اكسر;
		    حالة ADDR_TABS:
			lnum = CURRENT_TAB_NR;
			اكسر;
		    حالة ADDR_NONE:
		    حالة ADDR_TABS_RELATIVE:
		    حالة ADDR_UNSIGNED:
			addr_error(addr_type);
			cmd = عدم;
			اذهب error;
			اكسر;
		    حالة ADDR_QUICKFIX:
#ifdef FEAT_QUICKFIX
			lnum = qf_get_cur_idx(eap);
#endif
			اكسر;
		    حالة ADDR_QUICKFIX_VALID:
#ifdef FEAT_QUICKFIX
			lnum = qf_get_cur_valid_idx(eap);
#endif
			اكسر;
		}
		اكسر;

	    حالة '$':			    // '$' - last line
		++cmd;
		بدل (addr_type)
		{
		    حالة ADDR_سطریں:
		    حالة ADDR_OTHER:
			lnum = curbuf->b_ml.ml_سطر_گنتی;
			اكسر;
		    حالة ADDR_WINDOWS:
			lnum = LAST_WIN_NR;
			اكسر;
		    حالة ADDR_ARGUMENTS:
			lnum = ARGCOUNT;
			اكسر;
		    حالة ADDR_LOADED_BUFFERS:
			buf = lastbuf;
			طالما (buf->b_ml.ml_mfp == عدم)
			{
			    إذا (buf->b_prev == عدم)
				اكسر;
			    buf = buf->b_prev;
			}
			lnum = buf->b_fnum;
			اكسر;
		    حالة ADDR_BUFFERS:
			lnum = lastbuf->b_fnum;
			اكسر;
		    حالة ADDR_TABS:
			lnum = LAST_TAB_NR;
			اكسر;
		    حالة ADDR_NONE:
		    حالة ADDR_TABS_RELATIVE:
		    حالة ADDR_UNSIGNED:
			addr_error(addr_type);
			cmd = عدم;
			اذهب error;
			اكسر;
		    حالة ADDR_QUICKFIX:
#ifdef FEAT_QUICKFIX
			lnum = qf_get_size(eap);
			إذا (lnum == 0)
			    lnum = 1;
#endif
			اكسر;
		    حالة ADDR_QUICKFIX_VALID:
#ifdef FEAT_QUICKFIX
			lnum = qf_get_valid_size(eap);
			إذا (lnum == 0)
			    lnum = 1;
#endif
			اكسر;
		}
		اكسر;

	    حالة '\'':			    // ''' - mark
		إذا (*++cmd == NUL)
		{
		    cmd = عدم;
		    اذهب error;
		}
		إذا (addr_type != ADDR_سطریں)
		{
		    addr_error(addr_type);
		    cmd = عدم;
		    اذهب error;
		}
		إذا (skip)
		    ++cmd;
		وإلا
		{
		    // Only accept a mark in another file when it is
		    // used by itself: ":'M".
		    fp = getmark(*cmd, to_other_file && cmd[1] == NUL);
		    ++cmd;
		    إذا (fp == (pos_T *)-1)
			// Jumped to another file.
			lnum = curwin->w_cursor.lnum;
		    وإلا
		    {
			إذا (check_mark(fp) == FAIL)
			{
			    cmd = عدم;
			    اذهب error;
			}
			lnum = fp->lnum;
		    }
		}
		اكسر;

	    حالة '/':
	    حالة '?':			// '/' or '?' - search
		c = *cmd++;
		إذا (addr_type != ADDR_سطریں)
		{
		    addr_error(addr_type);
		    cmd = عدم;
		    اذهب error;
		}
		إذا (skip)	// skip "/pat/"
		{
		    cmd = skip_regexp(cmd, c, magic_isset());
		    إذا (*cmd == c)
			++cmd;
		}
		وإلا
		{
		    صحيح flags;

		    pos = curwin->w_cursor; // save curwin->w_cursor

		    // When '/' or '?' follows another address, start from
		    // there.
		    إذا (lnum > 0 && lnum != MAXLNUM)
			curwin->w_cursor.lnum =
				lnum > curbuf->b_ml.ml_سطر_گنتی
					   ? curbuf->b_ml.ml_سطر_گنتی : lnum;

		    // Start a forward search at the end of the line (unless
		    // before the first line).
		    // Start a backward search at the start of the line.
		    // This makes sure we never match in the current
		    // line, and can match anywhere in the
		    // next/previous line.
		    إذا (c == '/' && curwin->w_cursor.lnum > 0)
			curwin->w_cursor.col = MAXCOL;
		    وإلا
			curwin->w_cursor.col = 0;
		    searchcmdlen = 0;
		    flags = silent ? تلاش_KEEP : تلاش_HIS | تلاش_پیغام;
		    إذا (!do_تلاش(عدم, c, c, cmd, STRLEN(cmd), 1L, flags, عدم))
		    {
			curwin->w_cursor = pos;
			cmd = عدم;
			اذهب error;
		    }
		    lnum = curwin->w_cursor.lnum;
		    curwin->w_cursor = pos;
		    // adjust command string pointer
		    cmd += searchcmdlen;
		}
		اكسر;

	    حالة '\\':		    // "\?", "\/" or "\&", repeat search
		++cmd;
		إذا (addr_type != ADDR_سطریں)
		{
		    addr_error(addr_type);
		    cmd = عدم;
		    اذهب error;
		}
		إذا (*cmd == '&')
		    i = RE_SUBST;
		وإلا إذا (*cmd == '?' || *cmd == '/')
		    i = RE_تلاش;
		وإلا
		{
		    emsg(_(e_backslash_should_be_followed_by));
		    cmd = عدم;
		    اذهب error;
		}

		إذا (!skip)
		{
		    /*
		     * When search follows another address, start from
		     * there.
		     */
		    إذا (lnum != MAXLNUM)
			pos.lnum = lnum;
		    وإلا
			pos.lnum = curwin->w_cursor.lnum;

		    /*
		     * Start the search just like for the above
		     * do_search().
		     */
		    إذا (*cmd != '?')
			pos.col = MAXCOL;
		    وإلا
			pos.col = 0;
		    pos.coladd = 0;
		    إذا (searchit(curwin, curbuf, &pos, عدم,
				*cmd == '?' ? BACKWARD : FORWARD,
				(char_u *)"", 0, 1L, تلاش_پیغام, i, عدم) != FAIL)
			lnum = pos.lnum;
		    وإلا
		    {
			cmd = عدم;
			اذهب error;
		    }
		}
		++cmd;
		اكسر;

	    افتراضي:
		إذا (VIM_ISDIGIT(*cmd))	// absolute line number
		    lnum = getdigits(&cmd);
	}

	لكل (;;)
	{
	    cmd = skipwhite(cmd);
	    إذا (*cmd != '-' && *cmd != '+' && !VIM_ISDIGIT(*cmd))
		اكسر;

	    إذا (lnum == MAXLNUM)
	    {
		بدل (addr_type)
		{
		    حالة ADDR_سطریں:
		    حالة ADDR_OTHER:
			// "+1" is same as ".+1"
			lnum = curwin->w_cursor.lnum;
			اكسر;
		    حالة ADDR_WINDOWS:
			lnum = CURRENT_WIN_NR;
			اكسر;
		    حالة ADDR_ARGUMENTS:
			lnum = curwin->w_arg_idx + 1;
			اكسر;
		    حالة ADDR_LOADED_BUFFERS:
		    حالة ADDR_BUFFERS:
			lnum = curbuf->b_fnum;
			اكسر;
		    حالة ADDR_TABS:
			lnum = CURRENT_TAB_NR;
			اكسر;
		    حالة ADDR_TABS_RELATIVE:
			lnum = 1;
			اكسر;
		    حالة ADDR_QUICKFIX:
#ifdef FEAT_QUICKFIX
			lnum = qf_get_cur_idx(eap);
#endif
			اكسر;
		    حالة ADDR_QUICKFIX_VALID:
#ifdef FEAT_QUICKFIX
			lnum = qf_get_cur_valid_idx(eap);
#endif
			اكسر;
		    حالة ADDR_NONE:
		    حالة ADDR_UNSIGNED:
			lnum = 0;
			اكسر;
		}
	    }

	    إذا (VIM_ISDIGIT(*cmd))
		i = '+';		// "number" is same as "+number"
	    وإلا
		i = *cmd++;
	    إذا (!VIM_ISDIGIT(*cmd))	// '+' is '+1'
		n = 1;
	    وإلا
	    {
		// "number", "+number" or "-number"
		n = getdigits(&cmd);
		إذا (n == MAXLNUM)
		{
		    emsg(_(e_سطر_نمبر_out_of_range));
		    cmd = عدم;
		    اذهب error;
		}
	    }

	    إذا (addr_type == ADDR_TABS_RELATIVE)
	    {
		emsg(_(e_invalid_range));
		cmd = عدم;
		اذهب error;
	    }
	    وإلا إذا (addr_type == ADDR_LOADED_BUFFERS
		    || addr_type == ADDR_BUFFERS)
		lnum = compute_buffer_local_گنتی(
				    addr_type, lnum, (i == '-') ? -1 * n : n);
	    وإلا
	    {
#ifdef FEAT_FOLDING
		// Relative line addressing: need to adjust for lines in a
		// closed fold after the first address.
		إذا (addr_type == ADDR_سطریں && (i == '-' || i == '+')
							 && address_گنتی >= 2)
		    (فراغ)hasFolding(lnum, عدم, &lnum);
#endif
		إذا (i == '-')
		    lnum -= n;
		وإلا
		{
		    إذا (lnum >= 0 && n >= LONG_MAX - lnum)
		    {
			emsg(_(e_سطر_نمبر_out_of_range));
			cmd = عدم;
			اذهب error;
		    }
		    lnum += n;
		}
	    }
	}
    } طالما (*cmd == '/' || *cmd == '?');

error:
    *ptr = cmd;
    أرجع lnum;
}

/*
 * Set eap->line1 and eap->line2 to the whole range.
 * Used for commands with the EX_DFLALL flag and no range given.
 */
    ساكن فراغ
address_default_all(exarg_T *eap)
{
    eap->line1 = 1;
    بدل (eap->addr_type)
    {
	حالة ADDR_سطریں:
	حالة ADDR_OTHER:
	    eap->line2 = curbuf->b_ml.ml_سطر_گنتی;
	    اكسر;
	حالة ADDR_LOADED_BUFFERS:
	    {
		buf_T *buf = firstbuf;

		طالما (buf->b_next != عدم && buf->b_ml.ml_mfp == عدم)
		    buf = buf->b_next;
		eap->line1 = buf->b_fnum;
		buf = lastbuf;
		طالما (buf->b_prev != عدم && buf->b_ml.ml_mfp == عدم)
		    buf = buf->b_prev;
		eap->line2 = buf->b_fnum;
	    }
	    اكسر;
	حالة ADDR_BUFFERS:
	    eap->line1 = firstbuf->b_fnum;
	    eap->line2 = lastbuf->b_fnum;
	    اكسر;
	حالة ADDR_WINDOWS:
	    eap->line2 = LAST_WIN_NR;
	    اكسر;
	حالة ADDR_TABS:
	    eap->line2 = LAST_TAB_NR;
	    اكسر;
	حالة ADDR_TABS_RELATIVE:
	    eap->line2 = 1;
	    اكسر;
	حالة ADDR_ARGUMENTS:
	    إذا (ARGCOUNT == 0)
		eap->line1 = eap->line2 = 0;
	    وإلا
		eap->line2 = ARGCOUNT;
	    اكسر;
	حالة ADDR_QUICKFIX_VALID:
#ifdef FEAT_QUICKFIX
	    eap->line2 = qf_get_valid_size(eap);
	    إذا (eap->line2 == 0)
		eap->line2 = 1;
#endif
	    اكسر;
	حالة ADDR_NONE:
	حالة ADDR_UNSIGNED:
	حالة ADDR_QUICKFIX:
	    iemsg("Cannot use EX_DFLALL with ADDR_NONE, ADDR_UNSIGNED or ADDR_QUICKFIX");
	    اكسر;
    }
}


/*
 * Get flags from an Ex command argument.
 */
    ساكن فراغ
get_flags(exarg_T *eap)
{
    طالما (vim_strchr((char_u *)"lp#", *eap->arg) != عدم)
    {
	إذا (*eap->arg == 'l')
	    eap->flags |= EXFLAG_LIST;
	وإلا إذا (*eap->arg == 'p')
	    eap->flags |= EXFLAG_PRINT;
	وإلا
	    eap->flags |= EXFLAG_NR;
	eap->arg = skipwhite(eap->arg + 1);
    }
}

/*
 * Function called for command which is Not Implemented.  NI!
 */
    فراغ
ex_ni(exarg_T *eap)
{
    إذا (!eap->skip)
	eap->errmsg =
		_(e_sorry_command_is_not_available_in_this_version);
}

#ifdef HAVE_EX_SCRIPT_NI
/*
 * Function called for script command which is Not Implemented.  NI!
 * Skips over ":perl <<EOF" constructs.
 */
    ساكن فراغ
ex_script_ni(exarg_T *eap)
{
    إذا (!eap->skip)
	ex_ni(eap);
    وإلا
	vim_free(script_get(eap, eap->arg));
}
#endif

/*
 * Check range in Ex command for validity.
 * Return NULL when valid, error message when invalid.
 */
    ساكن حرف *
invalid_range(exarg_T *eap)
{
    buf_T	*buf;

    إذا (       eap->line1 < 0
	    || eap->line2 < 0
	    || eap->line1 > eap->line2)
	أرجع _(e_invalid_range);

    إذا (eap->argt & EX_RANGE)
    {
	بدل (eap->addr_type)
	{
	    حالة ADDR_سطریں:
		إذا (eap->line2 > curbuf->b_ml.ml_سطر_گنتی
#ifdef FEAT_DIFF
			    + (eap->cmdidx == CMD_diffget ||
				eap->cmdidx == CMD_diffput)
#endif
		   )
		    أرجع _(e_invalid_range);
		اكسر;
	    حالة ADDR_ARGUMENTS:
		// add 1 if ARGCOUNT is 0
		إذا (eap->line2 > ARGCOUNT + (!ARGCOUNT))
		    أرجع _(e_invalid_range);
		اكسر;
	    حالة ADDR_BUFFERS:
		// Only a boundary check, not whether the buffers actually
		// exist.
		إذا (eap->line1 < 1 || eap->line2 > get_highest_fnum())
		    أرجع _(e_invalid_range);
		اكسر;
	    حالة ADDR_LOADED_BUFFERS:
		buf = firstbuf;
		طالما (buf->b_ml.ml_mfp == عدم)
		{
		    إذا (buf->b_next == عدم)
			أرجع _(e_invalid_range);
		    buf = buf->b_next;
		}
		إذا (eap->line1 < buf->b_fnum)
		    أرجع _(e_invalid_range);
		buf = lastbuf;
		طالما (buf->b_ml.ml_mfp == عدم)
		{
		    إذا (buf->b_prev == عدم)
			أرجع _(e_invalid_range);
		    buf = buf->b_prev;
		}
		إذا (eap->line2 > buf->b_fnum)
		    أرجع _(e_invalid_range);
		اكسر;
	    حالة ADDR_WINDOWS:
		إذا (eap->line2 > LAST_WIN_NR)
		    أرجع _(e_invalid_range);
		اكسر;
	    حالة ADDR_TABS:
		إذا (eap->line2 > LAST_TAB_NR)
		    أرجع _(e_invalid_range);
		اكسر;
	    حالة ADDR_TABS_RELATIVE:
	    حالة ADDR_OTHER:
		// Any range is OK.
		اكسر;
	    حالة ADDR_QUICKFIX:
#ifdef FEAT_QUICKFIX
		// No error for value that is too big, will use the last entry.
		إذا (eap->line2 <= 0)
		{
		    إذا (eap->addr_گنتی == 0)
			أرجع _(e_no_errors);
		    أرجع _(e_invalid_range);
		}
#endif
		اكسر;
	    حالة ADDR_QUICKFIX_VALID:
#ifdef FEAT_QUICKFIX
		إذا ((eap->line2 != 1 && eap->line2 > qf_get_valid_size(eap))
			|| eap->line2 < 0)
		    أرجع _(e_invalid_range);
#endif
		اكسر;
	    حالة ADDR_UNSIGNED:
	    حالة ADDR_NONE:
		// Will give an error elsewhere.
		اكسر;
	}
    }
    أرجع عدم;
}

/*
 * Correct the range for zero line number, if required.
 */
    ساكن فراغ
correct_range(exarg_T *eap)
{
    إذا (!(eap->argt & EX_ZEROR))	    // zero in range not allowed
    {
	إذا (eap->line1 == 0)
	    eap->line1 = 1;
	إذا (eap->line2 == 0)
	    eap->line2 = 1;
    }
}

#ifdef FEAT_QUICKFIX
/*
 * For a ":vimgrep" or ":vimgrepadd" command return a pointer past the
 * pattern.  Otherwise return eap->arg.
 */
    ساكن char_u *
skip_grep_pat(exarg_T *eap)
{
    char_u	*p = eap->arg;

    إذا (*p != NUL && (eap->cmdidx == CMD_vimgrep || eap->cmdidx == CMD_lvimgrep
		|| eap->cmdidx == CMD_vimgrepadd
		|| eap->cmdidx == CMD_lvimgrepadd
		|| grep_internal(eap->cmdidx)))
    {
	p = skip_vimgrep_pat(p, عدم, عدم);
	إذا (p == عدم)
	    p = eap->arg;
    }
    أرجع p;
}

/*
 * For the ":make" and ":grep" commands insert the 'makeprg'/'grepprg' option
 * in the command line, so that things like % get expanded.
 */
    ساكن char_u *
replace_makeprg(exarg_T *eap, char_u *p, char_u **cmdlinep)
{
    char_u	*new_cmdline;
    char_u	*program;
    char_u	*pos;
    char_u	*ptr;
    صحيح		len;
    صحيح		i;

    /*
     * Don't do it when ":vimgrep" is used for ":grep".
     */
    إذا ((eap->cmdidx == CMD_make || eap->cmdidx == CMD_lmake
		     || eap->cmdidx == CMD_grep || eap->cmdidx == CMD_lgrep
		     || eap->cmdidx == CMD_grepadd
		     || eap->cmdidx == CMD_lgrepadd)
	    && !grep_internal(eap->cmdidx))
    {
	إذا (eap->cmdidx == CMD_grep || eap->cmdidx == CMD_lgrep
	    || eap->cmdidx == CMD_grepadd || eap->cmdidx == CMD_lgrepadd)
	{
	    إذا (*curbuf->b_p_gp == NUL)
		program = p_gp;
	    وإلا
		program = curbuf->b_p_gp;
	}
	وإلا
	{
	    إذا (*curbuf->b_p_mp == NUL)
		program = p_mp;
	    وإلا
		program = curbuf->b_p_mp;
	}

	p = skipwhite(p);

	إذا ((pos = (char_u *)strstr((حرف *)program, "$*")) != عدم)
	{
	    // replace $* by given arguments
	    i = 1;
	    طالما ((pos = (char_u *)strstr((حرف *)pos + 2, "$*")) != عدم)
		++i;
	    len = (صحيح)STRLEN(p);
	    new_cmdline = alloc(STRLEN(program) + (size_t)i * (len - 2) + 1);
	    إذا (new_cmdline == عدم)
		أرجع عدم;			// out of memory
	    ptr = new_cmdline;
	    طالما ((pos = (char_u *)strstr((حرف *)program, "$*")) != عدم)
	    {
		i = (صحيح)(pos - program);
		STRNCPY(ptr, program, i);
		STRCPY(ptr += i, p);
		ptr += len;
		program = pos + 2;
	    }
	    STRCPY(ptr, program);
	}
	وإلا
	{
	    size_t program_len = STRLEN(program);

	    new_cmdline = alloc(program_len + STRLEN(p) + 2);
	    إذا (new_cmdline == عدم)
		أرجع عدم;			// out of memory
	    STRCPY(new_cmdline, program);
	    STRCPY(new_cmdline + program_len, " ");
	    STRCPY(new_cmdline + program_len + 1, p);
	}
	پیغام_make(p);

	// 'eap->cmd' is not set here, because it is not used at CMD_make
	vim_free(*cmdlinep);
	*cmdlinep = new_cmdline;
	p = new_cmdline;
    }
    أرجع p;
}
#endif

/*
 * Expand file name in Ex command argument.
 * When an error is detected, "errormsgp" is set to a non-NULL pointer.
 * Return FAIL for failure, OK otherwise.
 */
    صحيح
expand_filename(
    exarg_T	*eap,
    char_u	**cmdlinep,
    حرف	**errormsgp)
{
    صحيح		has_wildcards;	// need to expand wildcards
    char_u	*repl;
    size_t	srclen;
    char_u	*p;
    صحيح		n;
    صحيح		escaped;

#ifdef FEAT_QUICKFIX
    // Skip a regexp pattern for ":vimgrep[add] pat file..."
    p = skip_grep_pat(eap);
#وإلا
    p = eap->arg;
#endif

    /*
     * Decide to expand wildcards *before* replacing '%', '#', etc.  If
     * the file name contains a wildcard it should not cause expanding.
     * (it will be expanded anyway if there is a wildcard before replacing).
     */
    has_wildcards = mch_has_wildcard(p);
    طالما (*p != NUL)
    {
#ifdef FEAT_EVAL
	// Skip over `=expr`, wildcards in it are not expanded.
	إذا (p[0] == '`' && p[1] == '=')
	{
	    p += 2;
	    (فراغ)skip_اظہاریہ(&p, عدم);
	    إذا (*p == '`')
		++p;
	    استمر;
	}
#endif
	/*
	 * Quick check if this cannot be the start of a special string.
	 * Also removes backslash before '%', '#' and '<'.
	 */
	إذا (vim_strchr((char_u *)"%#<", *p) == عدم)
	{
	    ++p;
	    استمر;
	}

	/*
	 * Try to find a match at this position.
	 */
	repl = eval_vars(p, eap->arg, &srclen, &(eap->do_ecmd_lnum),
						    errormsgp, &escaped, TRUE);
	إذا (*errormsgp != عدم)		// error detected
	    أرجع FAIL;
	إذا (repl == عدم)		// no match found
	{
	    p += srclen;
	    استمر;
	}

	// Wildcards won't be expanded below, the replacement is taken
	// literally.  But do expand "~/file", "~user/file" and "$HOME/file".
	إذا (vim_strchr(repl, '$') != عدم || vim_strchr(repl, '~') != عدم)
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
	إذا (!eap->usefilter
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
	    ساكن char_u *nobslash = (char_u *)" \t\"|";
# define ESCAPE_CHARS nobslash
#وإلا
# define ESCAPE_CHARS escape_chars
#endif

	    لكل (l = repl; *l; ++l)
		إذا (vim_strchr(ESCAPE_CHARS, *l) != عدم)
		{
		    l = vim_strsave_escaped(repl, ESCAPE_CHARS);
		    إذا (l != عدم)
		    {
			vim_free(repl);
			repl = l;
		    }
		    اكسر;
		}
	}

	// For a shell command a '!' must be escaped.
	إذا ((eap->usefilter || eap->cmdidx == CMD_bang
						|| eap->cmdidx == CMD_terminal)
			    && vim_strpbrk(repl, (char_u *)"!") != عدم)
	{
	    char_u	*l;

	    l = vim_strsave_escaped(repl, (char_u *)"!");
	    إذا (l != عدم)
	    {
		vim_free(repl);
		repl = l;
	    }
	}

	p = repl_cmdline(eap, p, srclen, repl, cmdlinep);
	vim_free(repl);
	إذا (p == عدم)
	    أرجع FAIL;
    }

    /*
     * One file argument: Expand wildcards.
     * Don't do this with ":r !command" or ":w !command".
     */
    إذا ((eap->argt & EX_NOSPC) && !eap->usefilter)
    {
	/*
	 * May do this twice:
	 * 1. Replace environment variables.
	 * 2. Replace any other wildcards, remove backslashes.
	 */
	لكل (n = 1; n <= 2; ++n)
	{
	    إذا (n == 2)
	    {
		/*
		 * Halve the number of backslashes (this is Vi compatible).
		 * For Unix and OS/2, when wildcards are expanded, this is
		 * done by ExpandOne() below.
		 */
#إذا defined(UNIX)
		إذا (!has_wildcards)
#endif
		    backslash_halve(eap->arg);
	    }

	    إذا (has_wildcards)
	    {
		إذا (n == 1)
		{
		    /*
		     * First loop: May expand environment variables.  This
		     * can be done much faster with expand_env() than with
		     * something else (e.g., calling a shell).
		     * After expanding environment variables, check again
		     * if there are still wildcards present.
		     */
		    إذا (vim_strchr(eap->arg, '$') != عدم
			    || vim_strchr(eap->arg, '~') != عدم)
		    {
			expand_env_esc(eap->arg, NameBuff, MAXPATHL,
					(char_u *)(" \t" PATH_ESC_WILDCARDS),
					TRUE, عدم);
			has_wildcards = mch_has_wildcard(NameBuff);
			p = NameBuff;
		    }
		    وإلا
			p = عدم;
		}
		وإلا // n == 2
		{
		    expand_T	xpc;
		    صحيح		options = WILD_LIST_NOTFOUND
					       | WILD_NOERROR | WILD_ADD_SLASH;

		    ExpandInit(&xpc);
		    xpc.xp_context = EXPAND_FILES;
		    إذا (p_wic)
			options += WILD_ICASE;
		    p = ExpandOne(&xpc, eap->arg, عدم,
						   options, WILD_EXPAND_FREE);
		    إذا (p == عدم)
			أرجع FAIL;
		}
		إذا (p != عدم)
		{
		    (فراغ)repl_cmdline(eap, eap->arg, STRLEN(eap->arg),
								 p, cmdlinep);
		    إذا (n == 2)	// p came from ExpandOne()
			vim_free(p);
		}
	    }
	}
    }
    أرجع OK;
}

/*
 * Replace part of the command line, keeping eap->cmd, eap->arg and
 * eap->nextcmd correct.
 * "src" points to the part that is to be replaced, of length "srclen".
 * "repl" is the replacement string.
 * Returns a pointer to the character after the replaced string.
 * Returns NULL for failure.
 */
    ساكن char_u *
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
    إذا (eap->nextcmd != عدم)
	i += STRLEN(eap->nextcmd);	// add space for next command
    إذا ((new_cmdline = alloc(i)) == عدم)
	أرجع عدم;			// out of memory!

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

    إذا (eap->nextcmd != عدم)		// append next command
    {
	new_cmdlinelen += taillen + 1;
	STRCPY(new_cmdline + new_cmdlinelen, eap->nextcmd);
	eap->nextcmd = new_cmdline + new_cmdlinelen;
    }
    eap->cmd = new_cmdline + (eap->cmd - *cmdlinep);
    eap->arg = new_cmdline + (eap->arg - *cmdlinep);
    إذا (eap->do_ecmd_cmd != عدم && eap->do_ecmd_cmd != dollar_command)
	eap->do_ecmd_cmd = new_cmdline + (eap->do_ecmd_cmd - *cmdlinep);
    vim_free(*cmdlinep);
    *cmdlinep = new_cmdline;

    أرجع src;
}

/*
 * Check for '|' to separate commands and '"' to start comments.
 * If "keep_backslash" is TRUE do not remove any backslash.
 */
    فراغ
separate_nextcmd(exarg_T *eap, صحيح keep_backslash)
{
    صحيح		vim9script = in_vim9script();
    char_u	*p;

#ifdef FEAT_QUICKFIX
    p = skip_grep_pat(eap);
#وإلا
    p = eap->arg;
#endif

    لكل ( ; *p; MB_PTR_ADV(p))
    {
	إذا (*p == Ctrl_V)
	{
	    إذا ((eap->argt & (EX_CTRLV | EX_XFILE)) || keep_backslash)
		++p;		// skip CTRL-V and next char
	    وإلا
				// remove CTRL-V and skip next char
		STRMOVE(p, p + 1);
	    إذا (*p == NUL)		// stop at NUL after CTRL-V
		اكسر;
	}

#ifdef FEAT_EVAL
	// Skip over `=expr` when wildcards are expanded.
	وإلا إذا (p[0] == '`' && p[1] == '=' && (eap->argt & EX_XFILE))
	{
	    p += 2;
	    (فراغ)skip_اظہاریہ(&p, عدم);
	    إذا (*p == NUL)		// stop at NUL after CTRL-V
		اكسر;
	}
#endif

	// Check for '"'/'#': start of comment or '|': next command
	// :@" and :*" do not start a comment!
	// :redir @" doesn't either.
	وإلا إذا ((*p == '"'
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
		    && eap->cmdidx != CMD_داخل_کرو)
		|| *p == '\n')
	{
	    /*
	     * We remove the '\' before the '|', unless EX_CTRLV is used
	     * AND 'b' is present in 'cpoptions'.
	     */
	    إذا ((vim_strchr(p_cpo, CPO_BAR) == عدم
			      || !(eap->argt & EX_CTRLV)) && *(p - 1) == '\\')
	    {
		إذا (!keep_backslash)
		{
		    STRMOVE(p - 1, p);	// remove the '\'
		    --p;
		}
	    }
	    وإلا
	    {
		set_nextcmd(eap, p);
		*p = NUL;
		اكسر;
	    }
	}
    }

    إذا (!(eap->argt & EX_NOTRLCOM))	// remove trailing spaces
	del_trailing_spaces(eap->arg);
}

/*
 * get + command from ex argument
 */
    ساكن char_u *
getargcmd(char_u **argp)
{
    char_u *arg = *argp;
    char_u *command = عدم;

    إذا (*arg == '+')	    // +[command]
    {
	++arg;
	إذا (vim_isspace(*arg) || *arg == NUL)
	    command = dollar_command;
	وإلا
	{
	    command = arg;
	    arg = skip_cmd_arg(command, TRUE);
	    إذا (*arg != NUL)
		*arg++ = NUL;		// terminate command with NUL
	}

	arg = skipwhite(arg);	// skip over spaces
	*argp = arg;
    }
    أرجع command;
}

/*
 * Find end of "+command" argument.  Skip over "\ " and "\\".
 */
    char_u *
skip_cmd_arg(
    char_u *p,
    صحيح	   rembs)	// TRUE to halve the number of backslashes
{
    طالما (*p && !vim_isspace(*p))
    {
	إذا (*p == '\\' && p[1] != NUL)
	{
	    إذا (rembs)
		STRMOVE(p, p + 1);
	    وإلا
		++p;
	}
	MB_PTR_ADV(p);
    }
    أرجع p;
}

    صحيح
get_bad_opt(char_u *p, exarg_T *eap)
{
    إذا (STRICMP(p, "keep") == 0)
	eap->bad_char = BAD_KEEP;
    وإلا إذا (STRICMP(p, "drop") == 0)
	eap->bad_char = BAD_DROP;
    وإلا إذا (MB_BYTE2LEN(*p) == 1 && p[1] == NUL)
	eap->bad_char = *p;
    وإلا
	أرجع FAIL;
    أرجع OK;
}

/*
 * Function given to ExpandGeneric() to obtain the list of bad= names.
 */
    ساكن char_u *
get_bad_name(expand_T *xp UNUSED, صحيح idx)
{
    // Note: Keep this in sync with getargopt.
    ساكن حرف *(p_bad_اقدار[]) =
    {
	"?",
	"keep",
	"drop",
    };

    إذا (idx < 0 || idx >= (صحيح)ARRAY_LENGTH(p_bad_اقدار))
	أرجع عدم;

    أرجع (char_u*)p_bad_اقدار[idx];
}

/*
 * Get "++opt=arg" argument.
 * Return FAIL or OK.
 */
    ساكن صحيح
getargopt(exarg_T *eap)
{
    char_u	*arg = eap->arg + 2;
    صحيح		*pp = عدم;
    صحيح		bad_char_idx;
    char_u	*p;

    // Note: Keep this in sync with get_argopt_name.

    // ":edit ++[no]bin[ary] file"
    إذا (STRNCMP(arg, "bin", 3) == 0 || STRNCMP(arg, "nobin", 5) == 0)
    {
	إذا (*arg == 'n')
	{
	    arg += 2;
	    eap->force_bin = FORCE_NOBIN;
	}
	وإلا
	    eap->force_bin = FORCE_BIN;
	إذا (!checkforcmd(&arg, "binary", 3))
	    أرجع FAIL;
	eap->arg = skipwhite(arg);
	أرجع OK;
    }

    // ":read ++edit file"
    إذا (STRNCMP(arg, "edit", 4) == 0)
    {
	eap->read_edit = TRUE;
	eap->arg = skipwhite(arg + 4);
	أرجع OK;
    }

    إذا (STRNCMP(arg, "ff", 2) == 0)
    {
	arg += 2;
	pp = &eap->force_ff;
    }
    وإلا إذا (STRNCMP(arg, "fileformat", 10) == 0)
    {
	arg += 10;
	pp = &eap->force_ff;
    }
    وإلا إذا (STRNCMP(arg, "enc", 3) == 0)
    {
	إذا (STRNCMP(arg, "encoding", 8) == 0)
	    arg += 8;
	وإلا
	    arg += 3;
	pp = &eap->force_enc;
    }
    وإلا إذا (STRNCMP(arg, "bad", 3) == 0)
    {
	arg += 3;
	pp = &bad_char_idx;
    }

    إذا (pp == عدم || *arg != '=')
	أرجع FAIL;

    ++arg;
    *pp = (صحيح)(arg - eap->cmd);
    arg = skip_cmd_arg(arg, FALSE);
    eap->arg = skipwhite(arg);
    *arg = NUL;

    إذا (pp == &eap->force_ff)
    {
	إذا (check_ff_قدر(eap->cmd + eap->force_ff) == FAIL)
	    أرجع FAIL;
	eap->force_ff = eap->cmd[eap->force_ff];
    }
    وإلا إذا (pp == &eap->force_enc)
    {
	// Make 'fileencoding' lower case.
	لكل (p = eap->cmd + eap->force_enc; *p != NUL; ++p)
	    *p = TOLOWER_ASC(*p);
    }
    وإلا
    {
	// Check ++bad= argument.  Must be a single-byte character, "keep" or
	// "drop".
	إذا (get_bad_opt(eap->cmd + bad_char_idx, eap) == FAIL)
	    أرجع FAIL;
    }

    أرجع OK;
}

/*
 * Function given to ExpandGeneric() to obtain the list of ++opt names.
 */
    ساكن char_u *
get_argopt_name(expand_T *xp UNUSED, صحيح idx)
{
    // Note: Keep this in sync with getargopt.
    ساكن حرف *(p_opt_اقدار[]) =
    {
	"fileformat=",
	"encoding=",
	"binary",
	"nobinary",
	"bad=",
	"edit",
    };

    إذا (idx < 0 || idx >= (صحيح)ARRAY_LENGTH(p_opt_اقدار))
	أرجع عدم;

    أرجع (char_u*)p_opt_اقدار[idx];
}

/*
 * Command-line expansion for ++opt=name.
 */
    صحيح
expand_argopt(
	char_u	    *pat,
	expand_T    *xp,
	regmatch_T  *rmp,
	char_u	    ***matches,
	صحيح	    *عدد_Matches)
{
    إذا (xp->xp_pattern > xp->xp_سطر && *(xp->xp_pattern-1) == '=')
    {
	char_u *(*cb)(expand_T *, صحيح) = عدم;

	char_u *name_end = xp->xp_pattern - 1;
	إذا (name_end - xp->xp_سطر >= 2
		&& STRNCMP(name_end - 2, "ff", 2) == 0)
	    cb = get_fileformat_name;
	وإلا إذا (name_end - xp->xp_سطر >= 10
		&& STRNCMP(name_end - 10, "fileformat", 10) == 0)
	    cb = get_fileformat_name;
	وإلا إذا (name_end - xp->xp_سطر >= 3
		&& STRNCMP(name_end - 3, "enc", 3) == 0)
	    cb = get_encoding_name;
	وإلا إذا (name_end - xp->xp_سطر >= 8
		&& STRNCMP(name_end - 8, "encoding", 8) == 0)
	    cb = get_encoding_name;
	وإلا إذا (name_end - xp->xp_سطر >= 3
		&& STRNCMP(name_end - 3, "bad", 3) == 0)
	    cb = get_bad_name;

	إذا (cb != عدم)
	{
	    أرجع ExpandGeneric(
		    pat,
		    xp,
		    rmp,
		    matches,
		    عدد_Matches,
		    cb,
		    FALSE);
	}
	أرجع FAIL;
    }

    // Special handling of "ff" which acts as a short form of
    // "fileformat", as "ff" is not a substring of it.
    إذا (xp->xp_pattern_len == 2
	    && STRNCMP(xp->xp_pattern, "ff", xp->xp_pattern_len) == 0)
    {
	*matches = ALLOC_MULT(char_u *, 1);
	إذا (*matches == عدم)
	    أرجع FAIL;
	*عدد_Matches = 1;
	(*matches)[0] = vim_strsave((char_u*)"fileformat=");
	أرجع OK;
    }

    أرجع ExpandGeneric(
	    pat,
	    xp,
	    rmp,
	    matches,
	    عدد_Matches,
	    get_argopt_name,
	    FALSE);
}

    ساكن فراغ
ex_autocmd(exarg_T *eap)
{
    إذا (eap->skip)
    {
	skip_cmd_block(eap);
	أرجع;
    }

    /*
     * Disallow autocommands from .exrc and .vimrc in current
     * directory for security reasons.
     */
    إذا (secure)
    {
	secure = 2;
	eap->errmsg =
	      _(e_command_not_allowed_from_vimrc_in_current_dir_or_tag_تلاش);
    }
    وإلا إذا (eap->cmdidx == CMD_autocmd)
	do_autocmd(eap, eap->arg, eap->forceit);
    وإلا
	do_augroup(eap->arg, eap->forceit);
}

/*
 * ":doautocmd": Apply the automatic commands to the current buffer.
 */
    ساكن فراغ
ex_doautocmd(exarg_T *eap)
{
    char_u	*arg = eap->arg;
    صحيح		call_do_modelines = check_nomodeline(&arg);
    صحيح		did_aucmd;

    (فراغ)do_doautocmd(arg, TRUE, &did_aucmd);
    // Only when there is no <nomodeline>.
    إذا (call_do_modelines && did_aucmd)
	do_modelines(0);
}

/*
 * :[N]bunload[!] [N] [bufname] unload buffer
 * :[N]bdelete[!] [N] [bufname] delete buffer from buffer list
 * :[N]bwipeout[!] [N] [bufname] delete buffer really
 */
    ساكن فراغ
ex_bunload(exarg_T *eap)
{
    إذا (ERROR_IF_ANY_POPUP_WINDOW)
	أرجع;
    eap->errmsg = do_bufdel(
	    eap->cmdidx == CMD_bdelete ? DOBUF_DEL
		: eap->cmdidx == CMD_bwipeout ? DOBUF_WIPE
		: DOBUF_UNLOAD, eap->arg,
	    eap->addr_گنتی, (صحيح)eap->line1, (صحيح)eap->line2, eap->forceit);
}

/*
 * :[N]buffer [N]	to buffer N
 * :[N]sbuffer [N]	to buffer N
 */
    ساكن فراغ
ex_buffer(exarg_T *eap)
{
    إذا (ERROR_IF_ANY_POPUP_WINDOW)
	أرجع;
    do_exbuffer(eap);
}

/*
 * ":buffer" command and alike.
 */
    ساكن فراغ
do_exbuffer(exarg_T *eap)
{
    إذا (*eap->arg)
	eap->errmsg = ex_errmsg(e_trailing_characters_str, eap->arg);
    وإلا
    {
	إذا (eap->addr_گنتی == 0)	// default is current buffer
	    goto_buffer(eap, DOBUF_CURRENT, FORWARD, 0);
	وإلا
	    goto_buffer(eap, DOBUF_FIRST, FORWARD, (صحيح)eap->line2);
	إذا (eap->do_ecmd_cmd != عدم)
	    do_cmd_argument(eap->do_ecmd_cmd);
    }
}

/*
 * :[N]bmodified [N]	to next mod. buffer
 * :[N]sbmodified [N]	to next mod. buffer
 */
    ساكن فراغ
ex_bmodified(exarg_T *eap)
{
    goto_buffer(eap, DOBUF_MOD, FORWARD, (صحيح)eap->line2);
    إذا (eap->do_ecmd_cmd != عدم)
	do_cmd_argument(eap->do_ecmd_cmd);
}

/*
 * :[N]bnext [N]	to next buffer
 * :[N]sbnext [N]	split and to next buffer
 */
    ساكن فراغ
ex_bnext(exarg_T *eap)
{
    إذا (ERROR_IF_ANY_POPUP_WINDOW)
	أرجع;

    goto_buffer(eap, DOBUF_CURRENT, FORWARD, (صحيح)eap->line2);
    إذا (eap->do_ecmd_cmd != عدم)
	do_cmd_argument(eap->do_ecmd_cmd);
}

/*
 * :[N]bNext [N]	to previous buffer
 * :[N]bprevious [N]	to previous buffer
 * :[N]sbNext [N]	split and to previous buffer
 * :[N]sbprevious [N]	split and to previous buffer
 */
    ساكن فراغ
ex_bprevious(exarg_T *eap)
{
    إذا (ERROR_IF_ANY_POPUP_WINDOW)
	أرجع;

    goto_buffer(eap, DOBUF_CURRENT, BACKWARD, (صحيح)eap->line2);
    إذا (eap->do_ecmd_cmd != عدم)
	do_cmd_argument(eap->do_ecmd_cmd);
}

/*
 * :brewind		to first buffer
 * :bfirst		to first buffer
 * :sbrewind		split and to first buffer
 * :sbfirst		split and to first buffer
 */
    ساكن فراغ
ex_brewind(exarg_T *eap)
{
    إذا (ERROR_IF_ANY_POPUP_WINDOW)
	أرجع;

    goto_buffer(eap, DOBUF_FIRST, FORWARD, 0);
    إذا (eap->do_ecmd_cmd != عدم)
	do_cmd_argument(eap->do_ecmd_cmd);
}

/*
 * :blast		to last buffer
 * :sblast		split and to last buffer
 */
    ساكن فراغ
ex_blast(exarg_T *eap)
{
    إذا (ERROR_IF_ANY_POPUP_WINDOW)
	أرجع;

    goto_buffer(eap, DOBUF_LAST, BACKWARD, 0);
    إذا (eap->do_ecmd_cmd != عدم)
	do_cmd_argument(eap->do_ecmd_cmd);
}

/*
 * Check if "c" ends an Ex command.
 * In Vim9 script does not check for white space before #.
 */
    صحيح
ends_excmd(صحيح c)
{
    صحيح comment_char = '"';

    إذا (in_vim9script())
	comment_char = '#';
    أرجع (c == NUL || c == '|' || c == comment_char || c == '\n');
}

/*
 * Like ends_excmd() but checks that a # in Vim9 script either has "cmd" equal
 * to "cmd_start" or has a white space character before it.
 */
    صحيح
ends_excmd2(char_u *cmd_start UNUSED, char_u *cmd)
{
    صحيح c = *cmd;

    إذا (c == NUL || c == '|' || c == '\n')
	أرجع TRUE;
    إذا (in_vim9script())
	//  # starts a comment, #{ might be a mistake, #{{ can start a fold
	أرجع c == '#' && (cmd[1] != '{' || cmd[2] == '{')
				 && (cmd == cmd_start || VIM_ISWHITE(cmd[-1]));
    أرجع c == '"';
}

#إذا defined(FEAT_SYN_HL) || defined(FEAT_تلاش_EXTRA) || defined(FEAT_EVAL)
/*
 * Return the next command, after the first '|' or '\n'.
 * Return NULL if not found.
 */
    char_u *
find_nextcmd(char_u *p)
{
    طالما (*p != '|' && *p != '\n')
    {
	إذا (*p == NUL)
	    أرجع عدم;
	++p;
    }
    أرجع (p + 1);
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

    إذا (*s == '|' || *s == '\n')
	أرجع (s + 1);
    وإلا
	أرجع عدم;
}

/*
 * If "eap->nextcmd" is not set, check for a next command at "p".
 */
    فراغ
set_nextcmd(exarg_T *eap, char_u *arg)
{
    char_u *p = skipwhite(arg);

    إذا (in_vim9script() && *p == '#')
    {
	char_u *nl = vim_strchr(p, NL);
	إذا (nl != عدم)
	    p = nl;
    }

    p = check_nextcmd(p);

    إذا (eap->nextcmd == عدم)
	eap->nextcmd = p;
    وإلا إذا (p != عدم)
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
    ساكن صحيح
check_more(
    صحيح message,	    // when FALSE check only, no messages
    صحيح forceit)
{
    صحيح	    n = ARGCOUNT - curwin->w_arg_idx - 1;

    إذا (!forceit && only_one_window()
	    && ARGCOUNT > 1 && !arg_had_last && n > 0 && quitmore == 0)
    {
	إذا (message)
	{
#إذا defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
	    إذا ((p_confirm || (cmdmod.cmod_flags & CMOD_CONFIRM))
						    && curbuf->b_fname != عدم)
	    {
		char_u	buff[DIALOG_پیغام_SIZE];

		vim_snprintf((حرف *)buff, DIALOG_پیغام_SIZE,
			NGETTEXT("%d more file to edit.  Quit anyway?",
			    "%d more files to edit.  Quit anyway?", n), n);
		إذا (vim_dialog_yesno(VIM_QUESTION, عدم, buff, 1) == VIM_YES)
		    أرجع OK;
		أرجع FAIL;
	    }
#endif
	    semsg(NGETTEXT(e_nr_more_file_to_edit,
			   e_nr_more_files_to_edit , n), n);
	    quitmore = 2;	    // next try to quit is allowed
	}
	أرجع FAIL;
    }
    أرجع OK;
}

/*
 * Function given to ExpandGeneric() to obtain the list of command names.
 */
    char_u *
get_command_name(expand_T *xp UNUSED, صحيح idx)
{
    إذا (idx >= (صحيح)CMD_SIZE)
	أرجع expand_user_command_name(idx);
    // the following are no real commands
    إذا (STRNCMP(cmdnames[idx].cmd_name, "{", 1) == 0 ||
	STRNCMP(cmdnames[idx].cmd_name, "}", 1) == 0)
	أرجع (char_u *)"";
    أرجع cmdnames[idx].cmd_name;
}

    ساكن فراغ
ex_colorscheme(exarg_T *eap)
{
    إذا (*eap->arg == NUL)
    {
#ifdef FEAT_EVAL
	char_u *اظہاریہ = vim_strsave((char_u *)"g:colors_name");
	char_u *p = عدم;

	إذا (اظہاریہ != عدم)
	{
	    ++emsg_off;
	    p = eval_to_string(اظہاریہ, FALSE, FALSE);
	    --emsg_off;
	    vim_free(اظہاریہ);
	}
	إذا (p != عدم)
	{
	    پیغام((حرف *)p);
	    vim_free(p);
	}
	وإلا
	    پیغام("default");
#وإلا
	پیغام(_("unknown"));
#endif
    }
    وإلا إذا (load_colors(eap->arg) == FAIL)
	semsg(_(e_cannot_find_color_scheme_str), eap->arg);

#ifdef FEAT_VTP
    وإلا إذا (has_vtp_working())
    {
	// background color change requires clear + redraw
	update_screen(UPD_CLEAR);
	redrawcmd();
    }
#endif
}

    ساكن فراغ
ex_highlight(exarg_T *eap)
{
    إذا (*eap->arg == NUL && eap->cmd[2] == '!')
	پیغام(_("Greetings, Vim user!"));
    do_highlight(eap->arg, eap->forceit, FALSE);
}


/*
 * Call this function if we thought we were going to exit, but we won't
 * (because of an error).  May need to restore the terminal mode.
 */
    فراغ
not_exiting(صحيح save_exiting)
{
    exiting = save_exiting;
    settmode(TMODE_RAW);
}

    صحيح
before_quit_autocmds(win_T *wp, صحيح quit_all, صحيح forceit)
{
    apply_autocmds(EVENT_QUITPRE, عدم, عدم, FALSE, wp->w_buffer);

    // Bail out when autocommands closed the window.
    // Refuse to quit when the buffer in the last window is being closed (can
    // only happen in autocommands).
    إذا (!win_valid(wp)
	    || curbuf_locked()
	    || (wp->w_buffer->b_nwindows == 1 && wp->w_buffer->b_locked > 0))
	أرجع TRUE;

    إذا (quit_all || (check_more(FALSE, forceit) == OK && only_one_window()))
    {
	apply_autocmds(EVENT_EXITPRE, عدم, عدم, FALSE, curbuf);
	// Refuse to quit when locked or when the window was closed or the
	// buffer in the last window is being closed (can only happen in
	// autocommands).
	إذا (!win_valid(wp) || curbuf_locked()
			  || (curbuf->b_nwindows == 1 && curbuf->b_locked > 0))
	    أرجع TRUE;
    }

    أرجع FALSE;
}

/*
 * ":quit": quit current window, quit Vim if the last window is closed.
 * ":{nr}quit": quit window {nr}
 * Also used when closing a terminal window that's the last one.
 */
    فراغ
ex_quit(exarg_T *eap)
{
    win_T	*wp;

    إذا (cmdwin_type != 0)
    {
	cmdwin_result = Ctrl_C;
	أرجع;
    }
    // Don't quit while editing the command line.
    إذا (text_locked())
    {
	text_locked_پیغام();
	أرجع;
    }
    إذا (eap->addr_گنتی > 0)
    {
	صحيح	wnr = eap->line2;

	لكل (wp = firstwin; wp->w_next != عدم; wp = wp->w_next)
	    إذا (--wnr <= 0)
		اكسر;
    }
    وإلا
	wp = curwin;

    // Refuse to quit when locked.
    إذا (curbuf_locked())
	أرجع;

    // Trigger QuitPre and maybe ExitPre
    إذا (before_quit_autocmds(wp, FALSE, eap->forceit))
	أرجع;

#ifdef FEAT_NETBEANS_INTG
    netbeansForcedQuit = eap->forceit;
#endif

    صحيح save_exiting = exiting;
    /*
     * If there is only one relevant window we will exit.
     */
    إذا (check_more(FALSE, eap->forceit) == OK && only_one_window())
	exiting = TRUE;
    إذا ((!buf_hide(wp->w_buffer)
		&& check_changed(wp->w_buffer, (p_awa ? CCGD_AW : 0)
				       | (eap->forceit ? CCGD_FORCEIT : 0)
				       | CCGD_EXCMD))
	    || check_more(TRUE, eap->forceit) == FAIL
	    || (only_one_window() && check_changed_any(eap->forceit, TRUE)))
    {
	not_exiting(save_exiting);
    }
    وإلا
    {
	// quit last window
	// Note: only_one_window() returns true, even so a help window is
	// still open. In that case only quit, if no address has been
	// specified. Example:
	// :h|wincmd w|1q     - don't quit
	// :h|wincmd w|q      - quit
	إذا (only_one_window() && (ONE_WINDOW || eap->addr_گنتی == 0))
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
    ساكن فراغ
ex_cquit(exarg_T *eap UNUSED)
{
    // this does not always pass on the exit code to the Manx compiler. why?
    getout(eap->addr_گنتی > 0 ? (صحيح)eap->line2 : EXIT_FAILURE);
}

/*
 * Do preparations for "qall" and "wqall".
 * Returns FAIL when quitting should be aborted.
 */
    صحيح
before_quit_all(exarg_T *eap)
{
    إذا (cmdwin_type != 0)
    {
	إذا (eap->forceit)
	    cmdwin_result = K_XF1;	// ex_window() takes care of this
	وإلا
	    cmdwin_result = K_XF2;
	أرجع FAIL;
    }

    // Don't quit while editing the command line.
    إذا (text_locked())
    {
	text_locked_پیغام();
	أرجع FAIL;
    }

    إذا (before_quit_autocmds(curwin, TRUE, eap->forceit))
	أرجع FAIL;

    أرجع OK;
}

/*
 * ":qall": try to quit all windows
 */
    ساكن فراغ
ex_quit_all(exarg_T *eap)
{
    إذا (before_quit_all(eap) == FAIL)
	أرجع;
    صحيح save_exiting = exiting;
    exiting = TRUE;
    إذا (eap->forceit || !check_changed_any(FALSE, FALSE))
	getout(0);
    not_exiting(save_exiting);
}

/*
 * ":close": close current window, unless it is the last one
 */
    ساكن فراغ
ex_close(exarg_T *eap)
{
    win_T	*win;
    صحيح		winnr = 0;
    إذا (cmdwin_type != 0)
	cmdwin_result = Ctrl_C;
    وإلا
	إذا (!text_locked() && !curbuf_locked())
	{
	    إذا (eap->addr_گنتی == 0)
		ex_win_close(eap->forceit, curwin, عدم);
	    وإلا
	    {
		FOR_ALL_WINDOWS(win)
		{
		    winnr++;
		    إذا (winnr == eap->line2)
			اكسر;
		}
		إذا (win == عدم)
		    win = lastwin;
		ex_win_close(eap->forceit, win, عدم);
	    }
	}
}

#ifdef FEAT_QUICKFIX
/*
 * ":pclose": Close any preview window.
 */
    ساكن فراغ
ex_pclose(exarg_T *eap)
{
    win_T	*win;

    // First close any normal window.
    FOR_ALL_WINDOWS(win)
	إذا (win->w_p_pvw)
	{
	    ex_win_close(eap->forceit, win, عدم);
	    أرجع;
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
    ساكن فراغ
ex_win_close(
    صحيح		forceit,
    win_T	*win,
    tabpage_T	*tp)		// NULL or the tab page "win" is in
{
    صحيح		need_hide;
    buf_T	*buf = win->w_buffer;

    // Never close the autocommand window.
    إذا (is_aucmd_win(win))
    {
	emsg(_(e_cannot_close_autocmd_or_popup_window));
	أرجع;
    }
    إذا (window_layout_locked(CMD_close))
	أرجع;

    need_hide = (bufIsChanged(buf) && buf->b_nwindows <= 1);
    إذا (need_hide && !buf_hide(buf) && !forceit)
    {
#إذا defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
	إذا ((p_confirm || (cmdmod.cmod_flags & CMOD_CONFIRM)) && p_write)
	{
# ifdef FEAT_TERMINAL
	    إذا (term_job_running(buf->b_term))
	    {
		إذا (term_confirm_stop(buf) == FAIL)
		    أرجع;
		// Manually kill the terminal here because this command will
		// hide it otherwise.
		free_terminal(buf);
		need_hide = FALSE;
	    }
	    وإلا
# endif
	    {
		bufref_T bufref;

		set_bufref(&bufref, buf);
		dialog_changed(buf, FALSE);
		إذا (bufref_valid(&bufref) && bufIsChanged(buf))
		    أرجع;
		need_hide = FALSE;
	    }
	}
	وإلا
#endif
	{
	    no_write_message();
	    أرجع;
	}
    }

#ifdef FEAT_GUI
    need_mouse_correct = TRUE;
#endif

    // free buffer when not hiding it or when it's a scratch buffer
    إذا (tp == عدم)
	win_close(win, !need_hide && !buf_hide(buf));
    وإلا
	win_close_othertab(win, !need_hide && !buf_hide(buf), tp);
}

/*
 * Handle the argument for a tabpage related ex command.
 * Returns a tabpage number.
 * When an error is encountered then eap->errmsg is set.
 */
    ساكن صحيح
get_tabpage_arg(exarg_T *eap)
{
    صحيح tab_نمبر;
    صحيح unaccept_arg0 = (eap->cmdidx == CMD_tabmove) ? 0 : 1;

    إذا (eap->arg && *eap->arg != NUL)
    {
	char_u *p = eap->arg;
	char_u *p_save;
	صحيح    relative = 0; // argument +N/-N means: go to N places to the
			     // right/left relative to the current position.

	إذا (*p == '-')
	{
	    relative = -1;
	    p++;
	}
	وإلا إذا (*p == '+')
	{
	    relative = 1;
	    p++;
	}

	p_save = p;
	tab_نمبر = getdigits(&p);

	إذا (relative == 0)
	{
	    إذا (STRCMP(p, "$") == 0)
		tab_نمبر = LAST_TAB_NR;
	    وإلا إذا (STRCMP(p, "#") == 0)
		إذا (valid_tabpage(lastused_tabpage))
		    tab_نمبر = tabpage_index(lastused_tabpage);
		وإلا
		{
		    eap->errmsg = ex_errmsg(e_invalid_قدر_for_argument_str,
								     eap->arg);
		    tab_نمبر = 0;
		    اذهب theend;
		}
	    وإلا إذا (p == p_save || *p_save == '-' || *p != NUL
		    || tab_نمبر > LAST_TAB_NR)
	    {
		// No numbers as argument.
		eap->errmsg = ex_errmsg(e_invalid_argument_str, eap->arg);
		اذهب theend;
	    }
	}
	وإلا
	{
	    إذا (*p_save == NUL)
		tab_نمبر = 1;
	    وإلا إذا (p == p_save || *p_save == '-' || *p != NUL
		    || tab_نمبر == 0)
	    {
		// No numbers as argument.
		eap->errmsg = ex_errmsg(e_invalid_argument_str, eap->arg);
		اذهب theend;
	    }
	    tab_نمبر = tab_نمبر * relative + tabpage_index(curtab);
	    إذا (!unaccept_arg0 && relative == -1)
		--tab_نمبر;
	}
	إذا (tab_نمبر < unaccept_arg0 || tab_نمبر > LAST_TAB_NR)
	    eap->errmsg = ex_errmsg(e_invalid_argument_str, eap->arg);
    }
    وإلا إذا (eap->addr_گنتی > 0)
    {
	إذا (unaccept_arg0 && eap->line2 == 0)
	{
	    eap->errmsg = _(e_invalid_range);
	    tab_نمبر = 0;
	}
	وإلا
	{
	    tab_نمبر = eap->line2;
	    إذا (!unaccept_arg0)
	    {
		char_u *cmdp = eap->cmd;

		طالما (--cmdp > *eap->cmdlinep
			&& (VIM_ISWHITE(*cmdp) || VIM_ISDIGIT(*cmdp)))
		    ;
		إذا (*cmdp == '-')
		{
		    --tab_نمبر;
		    إذا (tab_نمبر < unaccept_arg0)
			eap->errmsg = _(e_invalid_range);
		}
	    }
	}
    }
    وإلا
    {
	بدل (eap->cmdidx)
	{
	حالة CMD_tabnext:
	    tab_نمبر = tabpage_index(curtab) + 1;
	    إذا (tab_نمبر > LAST_TAB_NR)
		tab_نمبر = 1;
	    اكسر;
	حالة CMD_tabmove:
	    tab_نمبر = LAST_TAB_NR;
	    اكسر;
	افتراضي:
	    tab_نمبر = tabpage_index(curtab);
	}
    }

theend:
    أرجع tab_نمبر;
}

/*
 * ":tabclose": close current tab page, unless it is the last one.
 * ":tabclose N": close tab page N.
 */
    ساكن فراغ
ex_tabclose(exarg_T *eap)
{
    tabpage_T	*tp;
    صحيح		tab_نمبر;

    إذا (cmdwin_type != 0)
    {
	cmdwin_result = K_IGNORE;
	أرجع;
    }

    إذا (first_tabpage->tp_next == عدم)
    {
	emsg(_(e_cannot_close_last_tab_page));
	أرجع;
    }

    إذا (window_layout_locked(CMD_tabclose))
	أرجع;

    tab_نمبر = get_tabpage_arg(eap);
    إذا (eap->errmsg != عدم)
	أرجع;

    tp = find_tabpage(tab_نمبر);
    إذا (tp == عدم)
    {
	beep_flush();
	أرجع;
    }
    إذا (tp != curtab)
    {
	tabpage_close_other(tp, eap->forceit);
	أرجع;
    }
    وإلا إذا (!text_locked() && !curbuf_locked())
	tabpage_close(eap->forceit);
}

/*
 * ":tabonly": close all tab pages except the current one
 */
    ساكن فراغ
ex_tabonly(exarg_T *eap)
{
    tabpage_T	*tp;
    صحيح		done;
    صحيح		tab_نمبر;

    إذا (cmdwin_type != 0)
    {
	cmdwin_result = K_IGNORE;
	أرجع;
    }

    إذا (first_tabpage->tp_next == عدم)
    {
	پیغام(_("Already only one tab page"));
	أرجع;
    }

    إذا (window_layout_locked(CMD_tabonly))
	أرجع;

    tab_نمبر = get_tabpage_arg(eap);
    إذا (eap->errmsg != عدم)
	أرجع;

    goto_tabpage(tab_نمبر);
    // Repeat this up to a 1000 times, because autocommands may
    // mess up the lists.
    لكل (done = 0; done < 1000; ++done)
    {
	FOR_ALL_TABPAGES(tp)
	    إذا (tp->tp_topframe != topframe)
	    {
		tabpage_close_other(tp, eap->forceit);
		// if we failed to close it quit
		إذا (valid_tabpage(tp))
		    done = 1000;
		// start over, "tp" is now invalid
		اكسر;
	    }
	إذا (first_tabpage->tp_next == عدم)
	    اكسر;
    }
}

/*
 * Close the current tab page.
 */
    فراغ
tabpage_close(صحيح forceit)
{
    إذا (window_layout_locked(CMD_tabclose))
	أرجع;

    trigger_tabclosedpre(curtab);
    curtab->tp_did_tabclosedpre = TRUE;
    tabpage_T *save_curtab = curtab;

    // First close all the windows but the current one.  If that worked then
    // close the last window in this tab, that will close it.
    إذا (!ONE_WINDOW)
	close_others(TRUE, forceit);
    إذا (ONE_WINDOW)
	ex_win_close(forceit, curwin, عدم);
    إذا (curtab == save_curtab)
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
    فراغ
tabpage_close_other(tabpage_T *tp, صحيح forceit)
{
    صحيح		done = 0;
    win_T	*wp;

    إذا (window_layout_locked(CMD_SIZE))
	أرجع;

    trigger_tabclosedpre(tp);
    tp->tp_did_tabclosedpre = TRUE;

    // Limit to 1000 windows, autocommands may add a window while we close
    // one.  OK, so I'm paranoid...
    طالما (++done < 1000)
    {
	wp = tp->tp_firstwin;
	ex_win_close(forceit, wp, tp);

	// Autocommands may delete the tab page under our fingers.
	إذا (!valid_tabpage(tp))
	    اكسر;
	// We may fail to close a window with a modified buffer.
	إذا (tp->tp_firstwin == wp)
	{
	    done = 1000;
	    اكسر;
	}
    }
    إذا (done >= 1000)
    {
	// When closing the tab page failed, reset tp_did_tabclosedpre so that
	// TabClosedPre behaves consistently on next :close vs :tabclose.
	tp->tp_did_tabclosedpre = FALSE;
	أرجع;
    }

    apply_autocmds(EVENT_TABCLOSED, عدم, عدم, FALSE, curbuf);
}

/*
 * ":only".
 */
    ساكن فراغ
ex_only(exarg_T *eap)
{
    إذا (window_layout_locked(CMD_only))
	أرجع;
#ifdef FEAT_GUI
    need_mouse_correct = TRUE;
#endif
    إذا (eap->addr_گنتی > 0)
    {
	win_T   *wp;
	صحيح	wnr = eap->line2;
	لكل (wp = firstwin; --wnr > 0; )
	{
	    إذا (wp->w_next == عدم)
		اكسر;
	    وإلا
		wp = wp->w_next;
	}
	win_goto(wp);
    }
    close_others(TRUE, eap->forceit);
}

    ساكن فراغ
ex_hide(exarg_T *eap UNUSED)
{
    // ":hide" or ":hide | cmd": hide current window
    إذا (eap->skip)
	أرجع;

    إذا (window_layout_locked(CMD_hide))
	أرجع;
#ifdef FEAT_GUI
    need_mouse_correct = TRUE;
#endif
    إذا (eap->addr_گنتی == 0)
	win_close(curwin, FALSE);	// don't free buffer
    وإلا
    {
	صحيح	winnr = 0;
	win_T	*win;

	FOR_ALL_WINDOWS(win)
	{
	    winnr++;
	    إذا (winnr == eap->line2)
		اكسر;
	}
	إذا (win == عدم)
	    win = lastwin;
	win_close(win, FALSE);
    }
}

/*
 * ":stop" and ":suspend": Suspend Vim.
 */
    فراغ
ex_stop(exarg_T *eap)
{
    /*
     * Disallow suspending for "rvim".
     */
    إذا (check_restricted())
	أرجع;

    إذا (!eap->forceit)
	autowrite_all();
    apply_autocmds(EVENT_VIMSUSPEND, عدم, عدم, FALSE, عدم);
    windgoto((صحيح)Rows - 1, cmdline_col_off);
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
    apply_autocmds(EVENT_VIMRESUME, عدم, عدم, FALSE, عدم);
}

/*
 * ":exit", ":xit" and ":wq": Write file and quit the current window.
 */
    ساكن فراغ
ex_exit(exarg_T *eap)
{
#ifdef FEAT_EVAL
    إذا (not_in_vim9(eap, false) == FAIL)
	أرجع;
#endif
    إذا (cmdwin_type != 0)
    {
	cmdwin_result = Ctrl_C;
	أرجع;
    }
    // Don't quit while editing the command line.
    إذا (text_locked())
    {
	text_locked_پیغام();
	أرجع;
    }

    صحيح save_exiting = exiting;
    /*
     * we plan to exit if there is only one relevant window
     */
    إذا (check_more(FALSE, eap->forceit) == OK && only_one_window())
	exiting = TRUE;

    // Write the buffer for ":wq" or when it was changed.
    // Trigger QuitPre and ExitPre.
    // Check if we can exit now, after autocommands have changed things.
    إذا (((eap->cmdidx == CMD_wq || curbufIsChanged()) && do_write(eap) == FAIL)
	    || before_quit_autocmds(curwin, FALSE, eap->forceit)
	    || check_more(TRUE, eap->forceit) == FAIL
	    || (only_one_window() && check_changed_any(eap->forceit, FALSE)))
    {
	not_exiting(save_exiting);
    }
    وإلا
    {
	إذا (only_one_window())	    // quit last window, exit Vim
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
    ساكن فراغ
ex_print(exarg_T *eap)
{
    إذا (curbuf->b_ml.ml_flags & ML_EMPTY)
	emsg(_(e_empty_buffer));
    وإلا
    {
	لكل ( ;!got_int; ui_breakcheck())
	{
	    print_سطر(eap->line1,
		    (eap->cmdidx == CMD_نمبر || eap->cmdidx == CMD_pound
						 || (eap->flags & EXFLAG_NR)),
		    eap->cmdidx == CMD_list || (eap->flags & EXFLAG_LIST));
	    إذا (++eap->line1 > eap->line2)
		اكسر;
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
    ساكن فراغ
ex_goto(exarg_T *eap)
{
    goto_byte(eap->line2);
}
#endif

/*
 * ":shell".
 */
    ساكن فراغ
ex_shell(exarg_T *eap UNUSED)
{
    do_shell(عدم, 0);
}

#إذا defined(HAVE_DROP_FILE)

ساكن صحيح drop_busy = FALSE;
ساكن صحيح drop_filec;
ساكن char_u **drop_filev = عدم;
ساكن صحيح drop_split;
ساكن فراغ (*drop_callback)(فراغ *);
ساكن فراغ *drop_cookie;

    ساكن فراغ
handle_drop_internal(فراغ)
{
    exarg_T	ea;
    صحيح		save_پیغام_scroll = پیغام_scroll;

    // Setting the argument list may cause screen updates and being called
    // recursively.  Avoid that by setting drop_busy.
    drop_busy = TRUE;

    // Check whether the current buffer is changed. If so, we will need
    // to split the current window or data could be lost.
    // We don't need to check if the 'hidden' option is set, as in this
    // case the buffer won't be lost.
    إذا (!buf_hide(curbuf) && !drop_split)
    {
	++emsg_off;
	drop_split = check_changed(curbuf, CCGD_AW);
	--emsg_off;
    }
    إذا (drop_split)
    {
	إذا (win_split(0, 0) == FAIL)
	    أرجع;
	RESET_BINDING(curwin);

	// When splitting the window, create a new alist.  Otherwise the
	// existing one is overwritten.
	alist_unlink(curwin->w_alist);
	alist_new();
    }

    /*
     * Set up the new argument list.
     */
    alist_set(ALIST(curwin), drop_filec, drop_filev, FALSE, عدم, 0);

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
    پیغام_scroll = save_پیغام_scroll;

    إذا (drop_callback != عدم)
	drop_callback(drop_cookie);

    drop_filev = عدم;
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
    فراغ
handle_drop(
    صحيح		filec,		// the number of files dropped
    char_u	**filev,	// the list of files dropped
    صحيح		split,		// force splitting the window
    فراغ	(*callback)(فراغ *), // to be called after setting the argument
				     // list
    فراغ	*cookie)	// argument for "callback" (allocated)
{
    // Cannot handle recursive drops, finish the pending one.
    إذا (drop_busy)
    {
	FreeWild(filec, filev);
	vim_free(cookie);
	أرجع;
    }

    // When calling handle_drop() more than once in a row we only use the last
    // one.
    إذا (drop_filev != عدم)
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
    إذا (text_locked() || curbuf_locked() || updating_screen)
	أرجع;

    handle_drop_internal();
}

/*
 * To be called when text is unlocked, curbuf is unlocked or updating_screen is
 * reset: Handle a postponed drop.
 */
    فراغ
handle_any_postponed_drop(فراغ)
{
    إذا (!drop_busy && drop_filev != عدم
	     && !text_locked() && !curbuf_locked() && !updating_screen)
	handle_drop_internal();
}
#endif

/*
 * ":preserve".
 */
    ساكن فراغ
ex_preserve(exarg_T *eap UNUSED)
{
    curbuf->b_flags |= BF_PRESERVED;
    ml_preserve(curbuf, TRUE);
}

/*
 * ":recover".
 */
    ساكن فراغ
ex_recover(exarg_T *eap)
{
    // Set recoverymode right away to avoid the ATTENTION prompt.
    recoverymode = TRUE;
    إذا (!check_changed(curbuf, (p_awa ? CCGD_AW : 0)
			     | CCGD_MULTWIN
			     | (eap->forceit ? CCGD_FORCEIT : 0)
			     | CCGD_EXCMD)

	    && (*eap->arg == NUL
			     || setfname(curbuf, eap->arg, عدم, TRUE) == OK))
	ml_recover(TRUE);
    recoverymode = FALSE;
}

/*
 * Command modifier used in a wrong way.  Also for other commands that can't
 * appear at the toplevel.
 */
    ساكن فراغ
ex_wrongmodifier(exarg_T *eap)
{
    eap->errmsg = ex_errmsg(e_invalid_command_str, eap->cmd);
}

#إذا defined(FEAT_EVAL)

// callback function for 'findfunc'
ساكن callback_T ffu_cb;

    ساكن callback_T *
get_findfunc_callback(فراغ)
{
    أرجع *curbuf->b_p_ffu != NUL ? &curbuf->b_ffu_cb : &ffu_cb;
}

/*
 * Call 'findfunc' to obtain a list of file names.
 */
    ساكن list_T *
call_findfunc(char_u *pat, صحيح cmdcomplete)
{
    typval_T	args[3];
    callback_T	*cb;
    typval_T	rettv;
    صحيح		retval;
    sctx_T	saved_sctx = current_sctx;
    sctx_T	*ctx;

    args[0].v_type = VAR_STRING;
    args[0].vval.v_string = pat;
    args[1].v_type = VAR_BOOL;
    args[1].vval.v_نمبر = cmdcomplete;
    args[2].v_type = VAR_UNKNOWN;

    // Lock the text to prevent weird things from happening.  Also disallow
    // switching to another window, it should not be needed and may end up in
    // Insert mode in another buffer.
    ++textlock;

    ctx = get_option_sctx("findfunc");
    إذا (ctx != عدم)
	current_sctx = *ctx;

    cb = get_findfunc_callback();
    retval = call_callback(cb, -1, &rettv, 2, args);

    current_sctx = saved_sctx;

    --textlock;

    list_T *retlist = عدم;

    إذا (retval == OK)
    {
	إذا (rettv.v_type == VAR_LIST)
	    retlist = list_copy(rettv.vval.v_list, FALSE, FALSE, get_copyID());
	وإلا
	    emsg(_(e_invalid_return_type_from_findfunc));

	clear_tv(&rettv);
    }

    أرجع retlist;
}

/*
 * Find file names matching "pat" using 'findfunc' and return it in "files".
 * Used for expanding the :find, :sfind and :tabfind command argument.
 * Returns OK on success and FAIL otherwise.
 */
    صحيح
expand_findfunc(expand_T *xp, char_u *pat, char_u ***files, صحيح *عدد_Matches)
{
    list_T	*l;
    صحيح		len;

    *عدد_Matches = 0;
    *files = عدم;

    l = call_findfunc(pat, VVAL_TRUE);

    إذا (l == عدم)
	أرجع FAIL;

    len = list_len(l);
    إذا (len == 0)	    // empty List
    {
	list_free(l);
	أرجع FAIL;
    }

    expand_process_user_list(l, files, عدد_Matches, xp);
    list_free(l);

    أرجع OK;
}

/*
 * Use 'findfunc' to find file 'findarg'.  The 'count' argument is used to find
 * the n'th matching file.
 */
    ساكن char_u *
findfunc_find_file(char_u *findarg, صحيح findarg_len, صحيح گنتی)
{
    list_T	*fname_list;
    char_u	*ret_fname = عدم;
    char_u	cc;
    صحيح		fname_گنتی;

    cc = findarg[findarg_len];
    findarg[findarg_len] = NUL;

    fname_list = call_findfunc(findarg, VVAL_FALSE);
    fname_گنتی = list_len(fname_list);

    إذا (fname_گنتی == 0)
	semsg(_(e_cant_find_file_str_in_path), findarg);
    وإلا
    {
	إذا (گنتی > fname_گنتی)
	    semsg(_(e_no_more_file_str_found_in_path), findarg);
	وإلا
	{
	    listitem_T *li = list_find(fname_list, گنتی - 1);

	    إذا (li != عدم)
	    {
		typval_T *tv = &li->li_tv;

		إذا (tv->v_type == VAR_STRING && tv->vval.v_string != عدم)
		    ret_fname = vim_strsave(tv->vval.v_string);
		وإلا إذا (tv->v_type == VAR_DICT && tv->vval.v_dict != عدم)
		    ret_fname = dict_get_string(tv->vval.v_dict, "word", TRUE);
	    }
	}
    }

    إذا (fname_list != عدم)
	list_free(fname_list);

    findarg[findarg_len] = cc;

    أرجع ret_fname;
}

/*
 * Process the 'findfunc' option value.
 * Returns NULL on success and an error message on failure.
 */
    حرف *
did_set_findfunc(optset_T *args UNUSED)
{
    صحيح	retval;

    إذا (args->os_flags & OPT_LOCAL)
	// buffer-local option set
	retval = option_set_callback_func(curbuf->b_p_ffu, &curbuf->b_ffu_cb);
    وإلا
    {
	// global option set
	retval = option_set_callback_func(p_ffu, &ffu_cb);
	// when using :set, free the local callback
	إذا (!(args->os_flags & OPT_GLOBAL))
	    free_callback(&curbuf->b_ffu_cb);
    }

    إذا (retval == FAIL)
	أرجع e_invalid_argument;

    // If the option value starts with <SID> or s:, then replace that with
    // the script identifier.
    char_u	**varp = (char_u **)args->os_varp;
    char_u	*name = get_scriptlocal_funcname(*varp);
    إذا (name != عدم)
    {
	free_string_option(*varp);
	*varp = name;
    }

    أرجع عدم;
}

# إذا defined(EXITFREE)
    فراغ
free_findfunc_option(فراغ)
{
    free_callback(&ffu_cb);
}
# endif

/*
 * Mark the global 'findfunc' callback with "copyID" so that it is not
 * garbage collected.
 */
    صحيح
set_ref_in_findfunc(صحيح copyID UNUSED)
{
    صحيح abort = FALSE;
    abort = set_ref_in_callback(&ffu_cb, copyID);
    أرجع abort;
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
    فراغ
ex_splitview(exarg_T *eap)
{
    win_T	*old_curwin = curwin;
    char_u	*fname = عدم;
#ifdef FEAT_BROWSE
    char_u	dot_path[] = ".";
    صحيح		save_cmod_flags = cmdmod.cmod_flags;
#endif
    صحيح		use_tab = eap->cmdidx == CMD_tabedit
		       || eap->cmdidx == CMD_tabfind
		       || eap->cmdidx == CMD_tabnew;

    إذا (ERROR_IF_ANY_POPUP_WINDOW)
	أرجع;

#ifdef FEAT_GUI
    need_mouse_correct = TRUE;
#endif

#ifdef FEAT_QUICKFIX
    // A ":split" in the quickfix window works like ":new".  Don't want two
    // quickfix windows.  But it's OK when doing ":tab split".
    إذا (bt_quickfix(curbuf) && cmdmod.cmod_tab == 0)
    {
	إذا (eap->cmdidx == CMD_split)
	    eap->cmdidx = CMD_new;
	إذا (eap->cmdidx == CMD_vsplit)
	    eap->cmdidx = CMD_vnew;
    }
#endif

    إذا (eap->cmdidx == CMD_sfind || eap->cmdidx == CMD_tabfind)
    {
	char_u	*file_to_find = عدم;
	حرف	*تلاش_ctx = عدم;

	إذا (*get_findfunc() != NUL)
	{
#ifdef FEAT_EVAL
	    fname = findfunc_find_file(eap->arg, (صحيح)STRLEN(eap->arg),
				       eap->addr_گنتی > 0 ? eap->line2 : 1);
#endif
	}
	وإلا
	{
	    fname = find_file_in_path(eap->arg, (صحيح)STRLEN(eap->arg),
					      FNAME_MESS, TRUE, curbuf->b_ffname,
					      &file_to_find, &تلاش_ctx);
	    vim_free(file_to_find);
	    vim_findfile_cleanup(تلاش_ctx);
	}
	إذا (fname == عدم)
	    اذهب theend;
	eap->arg = fname;
    }
#ifdef FEAT_BROWSE
    وإلا إذا ((cmdmod.cmod_flags & CMOD_BROWSE)
	    && eap->cmdidx != CMD_vnew
	    && eap->cmdidx != CMD_new)
    {
	إذا (
# ifdef FEAT_GUI
	    !gui.in_use &&
# endif
		au_has_group((char_u *)"FileExplorer"))
	{
	    // No browsing supported but we do have the file explorer:
	    // Edit the directory.
	    إذا (*eap->arg == NUL || !mch_isdir(eap->arg))
		eap->arg = dot_path;
	}
	وإلا
	{
	    fname = do_browse(0, (char_u *)(use_tab
			? _("Edit File in new tab page")
			: _("Edit File in new window")),
					  eap->arg, عدم, عدم, عدم, curbuf);
	    إذا (fname == عدم)
		اذهب theend;
	    eap->arg = fname;
	}
    }
    cmdmod.cmod_flags &= ~CMOD_BROWSE;	// Don't browse again in do_ecmd().
#endif

    /*
     * Either open new tab page or split the window.
     */
    إذا (use_tab)
    {
	إذا (win_new_tabpage(cmdmod.cmod_tab != 0 ? cmdmod.cmod_tab
			 : eap->addr_گنتی == 0 ? 0
					       : (صحيح)eap->line2 + 1) != FAIL)
	{
	    do_exedit(eap, old_curwin);

	    // set the alternate buffer for the window we came from
	    إذا (curwin != old_curwin
		    && win_valid(old_curwin)
		    && old_curwin->w_buffer != curbuf
		    && (cmdmod.cmod_flags & CMOD_KEEPALT) == 0)
		old_curwin->w_alt_fnum = curbuf->b_fnum;
	}
    }
    وإلا إذا (win_split(eap->addr_گنتی > 0 ? (صحيح)eap->line2 : 0,
				     *eap->cmd == 'v' ? WSP_VERT : 0) != FAIL)
    {
	// Reset 'scrollbind' when editing another file, but keep it when
	// doing ":split" without arguments.
	إذا (*eap->arg != NUL)
	    RESET_BINDING(curwin);
	وإلا
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
    فراغ
tabpage_new(فراغ)
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
    ساكن فراغ
ex_tabnext(exarg_T *eap)
{
    صحيح tab_نمبر;

    إذا (ERROR_IF_POPUP_WINDOW)
	أرجع;
    بدل (eap->cmdidx)
    {
	حالة CMD_tabfirst:
	حالة CMD_tabrewind:
	    goto_tabpage(1);
	    اكسر;
	حالة CMD_tablast:
	    goto_tabpage(9999);
	    اكسر;
	حالة CMD_tabprevious:
	حالة CMD_tabNext:
	    إذا (eap->arg && *eap->arg != NUL)
	    {
		char_u *p = eap->arg;
		char_u *p_save = p;

		tab_نمبر = getdigits(&p);
		إذا (p == p_save || *p_save == '-' || *p != NUL
			    || tab_نمبر == 0)
		{
		    // No numbers as argument.
		    eap->errmsg = ex_errmsg(e_invalid_argument_str, eap->arg);
		    أرجع;
		}
	    }
	    وإلا
	    {
		إذا (eap->addr_گنتی == 0)
		    tab_نمبر = 1;
		وإلا
		{
		    tab_نمبر = eap->line2;
		    إذا (tab_نمبر < 1)
		    {
			eap->errmsg = _(e_invalid_range);
			أرجع;
		    }
		}
	    }
	    goto_tabpage(-tab_نمبر);
	    اكسر;
	افتراضي: // CMD_tabnext
	    tab_نمبر = get_tabpage_arg(eap);
	    إذا (eap->errmsg == عدم)
		goto_tabpage(tab_نمبر);
	    اكسر;
    }
}

/*
 * :tabmove command
 */
    ساكن فراغ
ex_tabmove(exarg_T *eap)
{
    صحيح tab_نمبر;

    tab_نمبر = get_tabpage_arg(eap);
    إذا (eap->errmsg == عدم)
	tabpage_move(tab_نمبر);
}

/*
 * :tabs command: List tabs and their contents.
 */
    ساكن فراغ
ex_tabs(exarg_T *eap UNUSED)
{
    tabpage_T	*tp;
    win_T	*wp;
    صحيح		tabcount = 1;

    پیغام_start();
    پیغام_scroll = TRUE;
    لكل (tp = first_tabpage; tp != عدم && !got_int; tp = tp->tp_next)
    {
	پیغام_putchar('\n');
	vim_snprintf((حرف *)IObuff, IOSIZE, _("Tab page %d"), tabcount++);
	پیغام_outtrans_attr(IObuff, HL_ATTR(HLF_T));
	out_flush();	    // output one line at a time
	ui_breakcheck();

	إذا (tp  == curtab)
	    wp = firstwin;
	وإلا
	    wp = tp->tp_firstwin;
	لكل ( ; wp != عدم && !got_int; wp = wp->w_next)
	{
	    پیغام_putchar('\n');
	    پیغام_putchar(wp == curwin ? '>' : ' ');
	    پیغام_putchar(' ');
	    پیغام_putchar(bufIsChanged(wp->w_buffer) ? '+' : ' ');
	    پیغام_putchar(' ');
	    إذا (buf_spname(wp->w_buffer) != عدم)
		vim_strncpy(IObuff, buf_spname(wp->w_buffer), IOSIZE - 1);
	    وإلا
		home_replace(wp->w_buffer, wp->w_buffer->b_fname,
							IObuff, IOSIZE, TRUE);
	    پیغام_outtrans(IObuff);
	    out_flush();	    // output one line at a time
	    ui_breakcheck();
	}
    }
}

/*
 * ":mode": Set screen mode.
 * If no argument given, just get the screen size and redraw.
 */
    ساكن فراغ
ex_mode(exarg_T *eap)
{
    إذا (*eap->arg == NUL)
	shell_resized();
    وإلا
	emsg(_(e_screen_mode_setting_not_supported));
}

/*
 * ":resize".
 * set, increment or decrement current window height
 */
    ساكن فراغ
ex_resize(exarg_T *eap)
{
    صحيح		n;
    win_T	*wp = curwin;

    إذا (eap->addr_گنتی > 0)
    {
	n = eap->line2;
	لكل (wp = firstwin; wp->w_next != عدم && --n > 0; wp = wp->w_next)
	    ;
    }

#ifdef FEAT_GUI
    need_mouse_correct = TRUE;
#endif
    n = atol((حرف *)eap->arg);
    إذا (cmdmod.cmod_split & WSP_VERT)
    {
	إذا (*eap->arg == '-' || *eap->arg == '+')
	    n += wp->w_width;
	وإلا إذا (n == 0 && eap->arg[0] == NUL)	// default is very wide
	    n = 9999;
	win_setwidth_win(n, wp);
    }
    وإلا
    {
	إذا (*eap->arg == '-' || *eap->arg == '+')
	    n += wp->w_height;
	وإلا إذا (n == 0 && eap->arg[0] == NUL)	// default is very high
	    n = 9999;
	win_setheight_win(n, wp);
    }
}

/*
 * ":find [+command] <file>" command.
 */
    ساكن فراغ
ex_find(exarg_T *eap)
{
    إذا (!check_can_set_curbuf_forceit(eap->forceit))
	أرجع;

    char_u	*fname = عدم;
    صحيح		گنتی;
    char_u	*file_to_find = عدم;
    حرف	*تلاش_ctx = عدم;

    إذا (*get_findfunc() != NUL)
    {
#ifdef FEAT_EVAL
	fname = findfunc_find_file(eap->arg, (صحيح)STRLEN(eap->arg),
					eap->addr_گنتی > 0 ? eap->line2 : 1);
#endif
    }
    وإلا
    {
	fname = find_file_in_path(eap->arg, (صحيح)STRLEN(eap->arg), FNAME_MESS,
			       TRUE, curbuf->b_ffname, &file_to_find, &تلاش_ctx);
	إذا (eap->addr_گنتی > 0)
	{
	    // Repeat finding the file "count" times.  This matters when it appears
	    // several times in the path.
	    گنتی = eap->line2;
	    طالما (fname != عدم && --گنتی > 0)
	    {
		vim_free(fname);
		fname = find_file_in_path(عدم, 0, FNAME_MESS,
			      FALSE, curbuf->b_ffname, &file_to_find, &تلاش_ctx);
	    }
	}
	VIM_CLEAR(file_to_find);
	vim_findfile_cleanup(تلاش_ctx);
    }

    إذا (fname == عدم)
	أرجع;

    eap->arg = fname;
    do_exedit(eap, عدم);
    vim_free(fname);
}

/*
 * ":open" simulation: for now works just like ":visual".
 */
    ساكن فراغ
ex_open(exarg_T *eap)
{
    regmatch_T	regmatch;
    char_u	*p;

#ifdef FEAT_EVAL
    إذا (not_in_vim9(eap, false) == FAIL)
	أرجع;
#endif
    curwin->w_cursor.lnum = eap->line2;
    beginline(BL_SOL | BL_FIX);
    إذا (*eap->arg == '/')
    {
	// ":open /pattern/": put cursor in column found with pattern
	++eap->arg;
	p = skip_regexp(eap->arg, '/', magic_isset());
	*p = NUL;
	regmatch.regprog = vim_regcomp(eap->arg, magic_isset() ? RE_MAGIC : 0);
	إذا (regmatch.regprog != عدم)
	{
	    // make a copy of the line, when searching for a mark it might be
	    // flushed
	    char_u *سطر = vim_strsave(ml_get_curline());

	    regmatch.rm_ic = p_ic;
	    إذا (vim_regexec(&regmatch, سطر, (colnr_T)0))
		curwin->w_cursor.col = (colnr_T)(regmatch.startp[0] - سطر);
	    وإلا
		emsg(_(e_no_match));
	    vim_regfree(regmatch.regprog);
	    vim_free(سطر);
	}
	// Move to the NUL, ignore any other arguments.
	eap->arg += STRLEN(eap->arg);
    }
    check_cursor();

    eap->cmdidx = CMD_visual;
    do_exedit(eap, عدم);
}

/*
 * ":edit", ":badd", ":balt", ":visual".
 */
    ساكن فراغ
ex_edit(exarg_T *eap)
{
    char_u *ffname = eap->cmdidx == CMD_enew ? عدم : eap->arg;

    // Exclude commands which keep the window's current buffer
    إذا (
	    eap->cmdidx != CMD_badd
	    && eap->cmdidx != CMD_balt
	    // All other commands must obey 'winfixbuf' / ! rules
	    && (is_other_file(0, ffname) && !check_can_set_curbuf_forceit(eap->forceit))
    )
	أرجع;

    do_exedit(eap, عدم);
}

/*
 * ":edit <file>" command and alike.
 */
    فراغ
do_exedit(
    exarg_T	*eap,
    win_T	*old_curwin)	    // curwin before doing a split or NULL
{
    صحيح		n;
    صحيح		need_hide;
    صحيح		exmode_was = exmode_active;

    إذا ((eap->cmdidx != CMD_pedit && ERROR_IF_POPUP_WINDOW)
						 || ERROR_IF_TERM_POPUP_WINDOW)
	أرجع;
    /*
     * ":vi" command ends Ex mode.
     */
    إذا (exmode_active && (eap->cmdidx == CMD_visual
						|| eap->cmdidx == CMD_view))
    {
	exmode_active = FALSE;
	ex_pressedreturn = FALSE;
	إذا (*eap->arg == NUL)
	{
	    // Special case:  ":global/pat/visual\NLvi-commands"
	    إذا (global_busy)
	    {
		إذا (eap->nextcmd != عدم)
		{
		    stuffReadbuff(eap->nextcmd);
		    eap->nextcmd = عدم;
		}

		إذا (exmode_was != EXMODE_VIM)
		    settmode(TMODE_RAW);
		صحيح save_RedrawingDisabled = RedrawingDisabled;
		RedrawingDisabled = 0;
		صحيح save_nwr = no_wait_return;
		no_wait_return = 0;
		need_wait_return = FALSE;
		صحيح save_ms = پیغام_scroll;
		پیغام_scroll = 0;
#ifdef FEAT_GUI
		صحيح save_he = hold_gui_events;
		hold_gui_events = 0;
#endif
		set_must_redraw(UPD_CLEAR);
		pending_exmode_active = TRUE;

		main_loop(FALSE, TRUE);

		pending_exmode_active = FALSE;
		RedrawingDisabled = save_RedrawingDisabled;
		no_wait_return = save_nwr;
		پیغام_scroll = save_ms;
#ifdef FEAT_GUI
		hold_gui_events = save_he;
#endif
	    }
	    أرجع;
	}
    }

    إذا ((eap->cmdidx == CMD_new
		|| eap->cmdidx == CMD_tabnew
		|| eap->cmdidx == CMD_tabedit
		|| eap->cmdidx == CMD_vnew) && *eap->arg == NUL)
    {
	// ":new" or ":tabnew" without argument: edit a new empty buffer
	setpcmark();
	(فراغ)do_ecmd(0, عدم, عدم, eap, ECMD_ONE,
		      ECMD_HIDE + (eap->forceit ? ECMD_FORCEIT : 0),
		      old_curwin == عدم ? curwin : عدم);
    }
    وإلا إذا ((eap->cmdidx != CMD_split && eap->cmdidx != CMD_vsplit)
	    || *eap->arg != NUL
#ifdef FEAT_BROWSE
	    || (cmdmod.cmod_flags & CMOD_BROWSE)
#endif
	    )
    {
	// Can't edit another file when "textlock" or "curbuf_lock" is set.
	// Only ":edit" or ":script" can bring us here, others are stopped
	// earlier.
	إذا (*eap->arg != NUL && text_or_buf_locked())
	    أرجع;

	n = readonlymode;
	إذا (eap->cmdidx == CMD_view || eap->cmdidx == CMD_sview)
	    readonlymode = TRUE;
	وإلا إذا (eap->cmdidx == CMD_enew)
	    readonlymode = FALSE;   // 'readonly' doesn't make sense in an
				    // empty buffer
	إذا (eap->cmdidx != CMD_balt && eap->cmdidx != CMD_badd)
	    setpcmark();
	إذا (do_ecmd(0, (eap->cmdidx == CMD_enew ? عدم : eap->arg),
		    عدم, eap,
		    // ":edit" goes to first line if Vi compatible
		    (*eap->arg == NUL && eap->do_ecmd_lnum == 0
				      && vim_strchr(p_cpo, CPO_GOTO1) != عدم)
					       ? ECMD_ONE : eap->do_ecmd_lnum,
		    (buf_hide(curbuf) ? ECMD_HIDE : 0)
		    + (eap->forceit ? ECMD_FORCEIT : 0)
		      // after a split we can use an existing buffer
		    + (old_curwin != عدم ? ECMD_OLDBUF : 0)
		    + (eap->cmdidx == CMD_badd ? ECMD_ADDBUF : 0)
		    + (eap->cmdidx == CMD_balt ? ECMD_ALTBUF : 0)
		    , old_curwin == عدم ? curwin : عدم) == FAIL)
	{
	    // Editing the file failed.  If the window was split, close it.
	    إذا (old_curwin != عدم)
	    {
		need_hide = (curbufIsChanged() && curbuf->b_nwindows <= 1);
		إذا (!need_hide || buf_hide(curbuf))
		{
#إذا defined(FEAT_EVAL)
		    cleanup_T   cs;

		    // Reset the error/interrupt/exception state here so that
		    // aborting() returns FALSE when closing a window.
		    enter_cleanup(&cs);
#endif
#ifdef FEAT_GUI
		    need_mouse_correct = TRUE;
#endif
		    win_close(curwin, !need_hide && !buf_hide(curbuf));

#إذا defined(FEAT_EVAL)
		    // Restore the error/interrupt/exception state if not
		    // discarded by a new aborting error, interrupt, or
		    // uncaught exception.
		    leave_cleanup(&cs);
#endif
		}
	    }
	}
	وإلا إذا (readonlymode && curbuf->b_nwindows == 1)
	{
	    // When editing an already visited buffer, 'readonly' won't be set
	    // but the previous value is kept.  With ":view" and ":sview" we
	    // want the  file to be readonly, except when another window is
	    // editing the same buffer.
	    curbuf->b_p_ro = TRUE;
	}
	readonlymode = n;
    }
    وإلا
    {
	إذا (eap->do_ecmd_cmd != عدم)
	    do_cmd_argument(eap->do_ecmd_cmd);
	n = curwin->w_arg_idx_invalid;
	check_arg_idx(curwin);
	إذا (n != curwin->w_arg_idx_invalid)
	    maketitle();
    }

    /*
     * if ":split file" worked, set alternate file name in old window to new
     * file
     */
    إذا (old_curwin != عدم
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
    ساكن فراغ
ex_nogui(exarg_T *eap)
{
    eap->errmsg = _(e_gui_cannot_be_used_not_enabled_at_compile_time);
}
#endif

#إذا defined(FEAT_GUI_MSWIN) && defined(FEAT_MENU) && defined(FEAT_TEAROFF)
    ساكن فراغ
ex_tearoff(exarg_T *eap)
{
    gui_make_tearoff(eap->arg);
}
#endif

#إذا (defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_GTK) \
	|| defined(FEAT_TERM_POPUP_MENU)) && defined(FEAT_MENU)
    ساكن فراغ
ex_popup(exarg_T *eap)
{
# إذا defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_GTK)
    إذا (gui.in_use)
	gui_make_popup(eap->arg, eap->forceit);
#  ifdef FEAT_TERM_POPUP_MENU
    وإلا
#  endif
# endif
# ifdef FEAT_TERM_POPUP_MENU
	pum_make_popup(eap->arg, eap->forceit);
# endif
}
#endif

    ساكن فراغ
ex_swapname(exarg_T *eap UNUSED)
{
    إذا (curbuf->b_ml.ml_mfp == عدم || curbuf->b_ml.ml_mfp->mf_fname == عدم)
	پیغام(_("No swap file"));
    وإلا
	پیغام((حرف *)curbuf->b_ml.ml_mfp->mf_fname);
}

/*
 * ":syncbind" forces all 'scrollbind' windows to have the same relative
 * offset.
 * (1998-11-02 16:21:01  R. Edward Ralston <eralston@computer.org>)
 */
    ساكن فراغ
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
    إذا (curwin->w_p_scb)
    {
	topline = curwin->w_topline;
	FOR_ALL_WINDOWS(wp)
	{
	    إذا (wp->w_p_scb && wp->w_buffer)
	    {
		y = wp->w_buffer->b_ml.ml_سطر_گنتی - get_scrolloff_قدر();
		إذا (topline > y)
		    topline = y;
	    }
	}
	إذا (topline < 1)
	    topline = 1;
    }
    وإلا
    {
	topline = 1;
    }


    /*
     * Set all scrollbind windows to the same topline.
     */
    FOR_ALL_WINDOWS(curwin)
    {
	إذا (curwin->w_p_scb)
	{
	    curbuf = curwin->w_buffer;
	    y = topline - curwin->w_topline;
	    إذا (y > 0)
		scrollup(y, TRUE);
	    وإلا
		scrolldown(-y, TRUE);
	    curwin->w_scbind_pos = topline;
	    redraw_later(UPD_VALID);
	    cursor_correct();
	    curwin->w_redr_status = true;
	}
    }
    curwin = save_curwin;
    curbuf = save_curbuf;
    إذا (curwin->w_p_scb)
    {
	did_syncbind = TRUE;
	checkpcmark();
	إذا (old_linenr != curwin->w_cursor.lnum)
	{
	    char_u ctrl_o[2];

	    ctrl_o[0] = Ctrl_O;
	    ctrl_o[1] = 0;
	    ins_typebuf(ctrl_o, REMAP_NONE, 0, TRUE, FALSE);
	}
    }
}


    ساكن فراغ
ex_read(exarg_T *eap)
{
    صحيح		i;
    صحيح		empty = (curbuf->b_ml.ml_flags & ML_EMPTY);
    linenr_T	lnum;

    إذا (eap->usefilter)			// :r!cmd
    {
	do_bang(1, eap, FALSE, FALSE, TRUE);
	أرجع;
    }

    إذا (u_save(eap->line2, (linenr_T)(eap->line2 + 1)) == FAIL)
	أرجع;

#ifdef FEAT_BROWSE
    إذا (cmdmod.cmod_flags & CMOD_BROWSE)
    {
	char_u *browseFile;

	browseFile = do_browse(0, (char_u *)_("Append File"), eap->arg,
		عدم, عدم, عدم, curbuf);
	إذا (browseFile != عدم)
	{
	    i = readfile(browseFile, عدم,
		    eap->line2, (linenr_T)0, (linenr_T)MAXLNUM, eap, 0);
	    vim_free(browseFile);
	}
	وإلا
	    i = OK;
    }
    وإلا
#endif
	إذا (*eap->arg == NUL)
	{
	    إذا (check_fname() == FAIL)	// check for no file name
		أرجع;
	    i = readfile(curbuf->b_ffname, curbuf->b_fname,
		    eap->line2, (linenr_T)0, (linenr_T)MAXLNUM, eap, 0);
	}
	وإلا
	{
	    إذا (vim_strchr(p_cpo, CPO_ALTREAD) != عدم)
		(فراغ)setaltfname(eap->arg, eap->arg, (linenr_T)1);
	    i = readfile(eap->arg, عدم,
		    eap->line2, (linenr_T)0, (linenr_T)MAXLNUM, eap, 0);

	}
    إذا (i != OK)
    {
#إذا defined(FEAT_EVAL)
	إذا (!aborting())
#endif
	    semsg(_(e_cant_open_file_str), eap->arg);
    }
    وإلا
    {
	إذا (empty && exmode_active)
	{
	    // Delete the empty line that remains.  Historically ex does
	    // this but vi doesn't.
	    إذا (eap->line2 == 0)
		lnum = curbuf->b_ml.ml_سطر_گنتی;
	    وإلا
		lnum = 1;
	    إذا (*ml_get(lnum) == NUL && u_savedel(lnum, 1L) == OK)
	    {
		ml_delete(lnum);
		إذا (curwin->w_cursor.lnum > 1
			&& curwin->w_cursor.lnum >= lnum)
		    --curwin->w_cursor.lnum;
		deleted_سطریں_mark(lnum, 1L);
	    }
	}
	redraw_curbuf_later(UPD_VALID);
    }
}

ساكن char_u	*prev_dir = عدم;

#إذا defined(EXITFREE)
    فراغ
free_cd_dir(فراغ)
{
    VIM_CLEAR(prev_dir);
    VIM_CLEAR(globaldir);
}
#endif

/*
 * Get the previous directory for the given chdir scope.
 */
    ساكن char_u *
get_prevdir(cdscope_T scope)
{
    إذا (scope == CDSCOPE_WINDOW)
	أرجع curwin->w_prevdir;
    وإلا إذا (scope == CDSCOPE_TABPAGE)
	أرجع curtab->tp_prevdir;
    أرجع prev_dir;
}

/*
 * Deal with the side effects of changing the current directory.
 * When 'scope' is CDSCOPE_TABPAGE then this was after an ":tcd" command.
 * When 'scope' is CDSCOPE_WINDOW then this was after an ":lcd" command.
 */
    فراغ
post_chdir(cdscope_T scope)
{
    إذا (scope != CDSCOPE_WINDOW)
	// Clear tab local directory for both :cd and :tcd
	VIM_CLEAR(curtab->tp_localdir);
    VIM_CLEAR(curwin->w_localdir);
    إذا (scope != CDSCOPE_GLOBAL)
    {
	char_u	*pdir = get_prevdir(scope);

	// If still in the global directory, need to remember current
	// directory as the global directory.
	إذا (globaldir == عدم && pdir != عدم)
	    globaldir = vim_strsave(pdir);

	// Remember this local directory for the window.
	إذا (mch_dirname(NameBuff, MAXPATHL) == OK)
	{
	    إذا (scope == CDSCOPE_TABPAGE)
		curtab->tp_localdir = vim_strsave(NameBuff);
	    وإلا
		curwin->w_localdir = vim_strsave(NameBuff);
	}
    }
    وإلا
    {
	// We are now in the global directory, no need to remember its name.
	VIM_CLEAR(globaldir);
    }

    last_chdir_reason = عدم;
    shorten_fnames(vim_strchr(p_cpo, CPO_NOSYMLINKS) == عدم);
}

/*
 * Trigger DirChangedPre for "acmd_fname" with directory "new_dir".
 */
    فراغ
trigger_DirChangedPre(char_u *acmd_fname, char_u *new_dir)
{
#ifdef FEAT_EVAL
    dict_T	    *v_event;
    save_v_event_T  save_v_event;

    v_event = get_v_event(&save_v_event);
    (فراغ)dict_add_string(v_event, "directory", new_dir);
    dict_set_اشیاء_ro(v_event);
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
    صحيح
changedir_func(
	char_u		*new_dir,
	صحيح		forceit,
	cdscope_T	scope)
{
    char_u	*pdir = عدم;
    صحيح		dir_differs;
    char_u	*acmd_fname = عدم;
    char_u	**pp;
    char_u	*tofree;

    إذا (new_dir == عدم || allbuf_locked())
	أرجع FALSE;

    إذا (vim_strchr(p_cpo, CPO_CHDIR) != عدم && curbufIsChanged() && !forceit)
    {
	emsg(_(e_cannot_change_directory_buffer_is_modified_add_bang_to_override));
	أرجع FALSE;
    }

    // ":cd -": Change to previous directory
    إذا (STRCMP(new_dir, "-") == 0)
    {
	pdir = get_prevdir(scope);
	إذا (pdir == عدم)
	{
	    emsg(_(e_no_previous_directory));
	    أرجع FALSE;
	}
	new_dir = pdir;
    }

    // Save current directory for next ":cd -"
    إذا (mch_dirname(NameBuff, MAXPATHL) == OK)
	pdir = vim_strsave(NameBuff);
    وإلا
	pdir = عدم;

    // For UNIX ":cd" means: go to home directory.
    // On other systems too if 'cdhome' is set.
#إذا defined(UNIX) || defined(VMS)
    إذا (*new_dir == NUL)
#وإلا
    إذا (*new_dir == NUL && p_cdh)
#endif
    {
	// use NameBuff for home directory name
#ifdef VMS
	char_u	*p;

	p = mch_getenv((char_u *)"SYS$LOGIN");
	إذا (p == عدم || *p == NUL)	// empty is the same as not set
	    NameBuff[0] = NUL;
	وإلا
	    vim_strncpy(NameBuff, p, MAXPATHL - 1);
#وإلا
	expand_env((char_u *)"$HOME", NameBuff, MAXPATHL);
#endif
	new_dir = NameBuff;
    }
    dir_differs = pdir == عدم
			    || pathcmp((حرف *)pdir, (حرف *)new_dir, -1) != 0;
    إذا (dir_differs)
    {
	إذا (scope == CDSCOPE_WINDOW)
	    acmd_fname = (char_u *)"window";
	وإلا إذا (scope == CDSCOPE_TABPAGE)
	    acmd_fname = (char_u *)"tabpage";
	وإلا
	    acmd_fname = (char_u *)"global";
	trigger_DirChangedPre(acmd_fname, new_dir);

	إذا (vim_chdir(new_dir))
	{
	    emsg(_(e_command_failed));
	    vim_free(pdir);
	    أرجع FALSE;
	}
    }

    إذا (scope == CDSCOPE_WINDOW)
	pp = &curwin->w_prevdir;
    وإلا إذا (scope == CDSCOPE_TABPAGE)
	pp = &curtab->tp_prevdir;
    وإلا
	pp = &prev_dir;
    tofree = *pp;  // new_dir may use this
    *pp = pdir;

    post_chdir(scope);

    إذا (dir_differs)
	apply_autocmds(EVENT_DIRCHANGED, acmd_fname, new_dir, FALSE, curbuf);
    vim_free(tofree);
    أرجع TRUE;
}

/*
 * ":cd", ":tcd", ":lcd", ":chdir" ":tchdir" and ":lchdir".
 */
    فراغ
ex_cd(exarg_T *eap)
{
    char_u	*new_dir;

    new_dir = eap->arg;
#إذا !defined(UNIX) && !defined(VMS)
    // for non-UNIX ":cd" means: print current directory unless 'cdhome' is set
    إذا (*new_dir == NUL && !p_cdh)
    {
	ex_pwd(عدم);
	أرجع;
    }
#endif

    cdscope_T	scope = CDSCOPE_GLOBAL;

    إذا (eap->cmdidx == CMD_lcd || eap->cmdidx == CMD_lchdir)
	scope = CDSCOPE_WINDOW;
    وإلا إذا (eap->cmdidx == CMD_tcd || eap->cmdidx == CMD_tchdir)
	scope = CDSCOPE_TABPAGE;

    إذا (changedir_func(new_dir, eap->forceit, scope))
    {
	// Echo the new current directory if the command was typed.
	إذا (کلید_Typed || p_verbose >= 5)
	    ex_pwd(eap);
    }
}

/*
 * ":pwd".
 */
    ساكن فراغ
ex_pwd(exarg_T *eap UNUSED)
{
    إذا (mch_dirname(NameBuff, MAXPATHL) == OK)
    {
#ifdef BACKSLASH_IN_FILENAME
	slash_adjust(NameBuff);
#endif
	إذا (p_verbose > 0)
	{
	    حرف *context = "global";

	    إذا (last_chdir_reason != عدم)
		context = last_chdir_reason;
	    وإلا إذا (curwin->w_localdir != عدم)
		context = "window";
	    وإلا إذا (curtab->tp_localdir != عدم)
		context = "tabpage";
	    smsg("[%s] %s", context, (حرف *)NameBuff);
	}
	وإلا
	    پیغام((حرف *)NameBuff);
    }
    وإلا
	emsg(_(e_directory_unknown));
}

/*
 * ":=".
 */
    ساكن فراغ
ex_equal(exarg_T *eap)
{
    smsg("%ld", (long)eap->line2);
    ex_may_print(eap);
}

    ساكن فراغ
ex_sleep(exarg_T *eap)
{
    صحيح		n;
    long	len;

    إذا (cursor_valid())
    {
	n = W_WINROW(curwin) + curwin->w_wrow - پیغام_scrolled;
	إذا (n >= 0)
	    windgoto(n, curwin->w_wincol + curwin->w_wcol);
    }

    len = eap->line2;
    بدل (*eap->arg)
    {
	حالة 'm': اكسر;
	حالة NUL: len *= 1000L; اكسر;
	افتراضي: semsg(_(e_invalid_argument_str), eap->arg); أرجع;
    }

    // Hide the cursor if invoked with !
    do_sleep(len, eap->forceit);
}

/*
 * Sleep for "msec" milliseconds, but keep checking for a CTRL-C every second.
 * Hide the cursor if "hide_cursor" is TRUE.
 */
    فراغ
do_sleep(long msec, صحيح hide_cursor)
{
    long	done = 0;
    long	wait_now;
#ifdef ELAPSED_FUNC
    elapsed_T	start_tv;

    // Remember at what time we started, so that we know how much longer we
    // should wait after waiting for a bit.
    ELAPSED_INIT(start_tv);
#endif

    إذا (hide_cursor)
	cursor_sleep();
    وإلا
	cursor_on();

    out_flush_cursor(FALSE, FALSE);
    طالما (!got_int && done < msec)
    {
	wait_now = msec - done > 1000L ? 1000L : msec - done;
#ifdef FEAT_TIMERS
	{
	    long    due_time = check_due_timer();

	    إذا (due_time > 0 && due_time < wait_now)
		wait_now = due_time;
	}
#endif
#ifdef FEAT_JOB_CHANNEL
	إذا (has_any_channel() && wait_now > 20L)
	    wait_now = 20L;
#endif
#ifdef FEAT_SOUND
	إذا (has_any_sound_callback() && wait_now > 20L)
	    wait_now = 20L;
#endif
	ui_delay(wait_now, TRUE);

#ifdef FEAT_JOB_CHANNEL
	إذا (has_any_channel())
	    ui_breakcheck_force(TRUE);
	وإلا
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
#وإلا
	// guesstimate time passed (will actually be more)
	done += wait_now;
#endif
    }

    // If CTRL-C was typed to interrupt the sleep, drop the CTRL-C from the
    // input buffer, otherwise a following call to input() fails.
    إذا (got_int)
	(فراغ)vpeekc();

    إذا (hide_cursor)
	cursor_unsleep();
}

/*
 * ":winsize" command (obsolete).
 */
    ساكن فراغ
ex_winsize(exarg_T *eap)
{
    صحيح		w, h;
    char_u	*arg = eap->arg;
    char_u	*p;

    إذا (!SAFE_isdigit(*arg))
    {
	semsg(_(e_invalid_argument_str), arg);
	أرجع;
    }
    w = getdigits(&arg);
    arg = skipwhite(arg);
    p = arg;
    h = getdigits(&arg);
    إذا (*p != NUL && *arg == NUL)
	set_shellsize(w, h, TRUE);
    وإلا
	emsg(_(e_winsize_requires_two_نمبر_arguments));
}

    ساكن فراغ
ex_wincmd(exarg_T *eap)
{
    صحيح		xchar = NUL;
    char_u	*p;

    إذا (*eap->arg == 'g' || *eap->arg == Ctrl_G)
    {
	// CTRL-W g and CTRL-W CTRL-G  have an extra command character
	إذا (eap->arg[1] == NUL)
	{
	    emsg(_(e_invalid_argument));
	    أرجع;
	}
	xchar = eap->arg[1];
	p = eap->arg + 2;
    }
    وإلا
	p = eap->arg + 1;

    set_nextcmd(eap, p);
    p = skipwhite(p);
    إذا (*p != NUL && *p != (
#ifdef FEAT_EVAL
	    in_vim9script() ? '#' :
#endif
		'"')
	    && eap->nextcmd == عدم)
	emsg(_(e_invalid_argument));
    وإلا إذا (!eap->skip)
    {
	// Pass flags on for ":vertical wincmd ]".
	postponed_split_flags = cmdmod.cmod_split;
	postponed_split_tab = cmdmod.cmod_tab;
	do_window(*eap->arg, eap->addr_گنتی > 0 ? eap->line2 : 0L, xchar);
	postponed_split_flags = 0;
	postponed_split_tab = 0;
    }
}

#إذا defined(FEAT_GUI) || defined(UNIX) || defined(VMS) || defined(MSWIN)
/*
 * ":winpos".
 */
    ساكن فراغ
ex_winpos(exarg_T *eap)
{
    صحيح		x, y;
    char_u	*arg = eap->arg;
    char_u	*p;

    إذا (*arg == NUL)
    {
# إذا defined(FEAT_GUI) || defined(MSWIN)
#  ifdef VIMDLL
	إذا (gui.in_use ? gui_mch_get_winpos(&x, &y) != FAIL :
		mch_get_winpos(&x, &y) != FAIL)
#  elif defined(FEAT_GUI)
	إذا (gui.in_use && gui_mch_get_winpos(&x, &y) != FAIL)
#  وإلا
	إذا (mch_get_winpos(&x, &y) != FAIL)
#  endif
	{
	    sprintf((حرف *)IObuff, _("Window position: X %d, Y %d"), x, y);
	    پیغام((حرف *)IObuff);
	}
	وإلا
# endif
	    emsg(_(e_obtaining_window_position_not_implemented_for_this_platform));
    }
    وإلا
    {
	x = getdigits(&arg);
	arg = skipwhite(arg);
	p = arg;
	y = getdigits(&arg);
	إذا (*p == NUL || *arg != NUL)
	{
	    emsg(_(e_winpos_requires_two_نمبر_arguments));
	    أرجع;
	}
# ifdef FEAT_GUI
	إذا (gui.in_use)
	    gui_mch_set_winpos(x, y);
	وإلا إذا (gui.starting)
	{
	    // Remember the coordinates for when the window is opened.
	    gui_win_x = x;
	    gui_win_y = y;
	}
#  إذا defined(HAVE_TGETENT) || defined(VIMDLL)
	وإلا
#  endif
# endif
# إذا defined(MSWIN) && (!defined(FEAT_GUI) || defined(VIMDLL))
	    mch_set_winpos(x, y);
# endif
# ifdef HAVE_TGETENT
	إذا (*T_CWP)
	    term_set_winpos(x, y);
# endif
    }
}
#endif

/*
 * Handle command that work like operators: ":delete", ":yank", ":>" and ":<".
 */
    ساكن فراغ
ex_operators(exarg_T *eap)
{
    oparg_T	oa;

    clear_oparg(&oa);
    oa.regname = eap->regname;
    oa.start.lnum = eap->line1;
    oa.end.lnum = eap->line2;
    oa.سطر_گنتی = eap->line2 - eap->line1 + 1;
    oa.motion_type = MLINE;
    virtual_op = FALSE;
    إذا (eap->cmdidx != CMD_yank)	// position cursor for undo
    {
	setpcmark();
	curwin->w_cursor.lnum = eap->line1;
	beginline(BL_SOL | BL_FIX);
    }

    إذا (VIsual_active)
	end_visual_mode();

    بدل (eap->cmdidx)
    {
	حالة CMD_delete:
	    oa.op_type = OP_DELETE;
	    op_delete(&oa);
	    اكسر;

	حالة CMD_yank:
	    oa.op_type = OP_YANK;
	    (فراغ)op_yank(&oa, FALSE, TRUE);
	    اكسر;

	افتراضي:    // CMD_rshift or CMD_lshift
	    إذا (
#ifdef FEAT_RIGHTLEFT
		(eap->cmdidx == CMD_rshift) ^ curwin->w_p_rl
#وإلا
		eap->cmdidx == CMD_rshift
#endif
						)
		oa.op_type = OP_RSHIFT;
	    وإلا
		oa.op_type = OP_LSHIFT;
	    op_shift(&oa, FALSE, eap->amount);
	    اكسر;
    }
    virtual_op = MAYBE;
    ex_may_print(eap);
}

/*
 * ":put".
 */
    ساكن فراغ
ex_put(exarg_T *eap)
{
    // ":0put" works like ":1put!".
    إذا (eap->line2 == 0)
    {
	eap->line2 = 1;
	eap->forceit = TRUE;
    }
    curwin->w_cursor.lnum = eap->line2;
    check_cursor_col();
    do_put(eap->regname, عدم, eap->forceit ? BACKWARD : FORWARD, 1L,
						       PUT_سطر|PUT_CURSLINE);
}

/*
 * ":iput".
 */
    ساكن فراغ
ex_iput(exarg_T *eap)
{
    // ":0iput" works like ":1iput!".
    إذا (eap->line2 == 0)
    {
	eap->line2 = 1;
	eap->forceit = TRUE;
    }
    curwin->w_cursor.lnum = eap->line2;
    check_cursor_col();
    do_put(eap->regname, عدم, eap->forceit ? BACKWARD : FORWARD, 1L,
						      PUT_سطر|PUT_CURSLINE
						      |PUT_FIXINDENT);
}

/*
 * Handle ":copy" and ":move".
 */
    ساكن فراغ
ex_copymove(exarg_T *eap)
{
    long	n;

#ifdef FEAT_EVAL
    إذا (not_in_vim9(eap, false) == FAIL)
	أرجع;
#endif
    n = get_address(eap, &eap->arg, eap->addr_type, FALSE, FALSE, FALSE, 1);
    إذا (eap->arg == عدم)	    // error detected
    {
	eap->nextcmd = عدم;
	أرجع;
    }
    get_flags(eap);

    /*
     * move or copy lines from 'eap->line1'-'eap->line2' to below line 'n'
     */
    إذا (n == MAXLNUM || n < 0 || n > curbuf->b_ml.ml_سطر_گنتی)
    {
	emsg(_(e_invalid_range));
	أرجع;
    }

    إذا (eap->cmdidx == CMD_move)
    {
	إذا (do_move(eap->line1, eap->line2, n) == FAIL)
	    أرجع;
    }
    وإلا
	ex_copy(eap->line1, eap->line2, n);
    u_clearline();
    beginline(BL_SOL | BL_FIX);
    ex_may_print(eap);
}

/*
 * Print the current line if flags were given to the Ex command.
 */
    فراغ
ex_may_print(exarg_T *eap)
{
    إذا (eap->flags != 0)
    {
	print_سطر(curwin->w_cursor.lnum, (eap->flags & EXFLAG_NR),
						  (eap->flags & EXFLAG_LIST));
	ex_no_reprint = TRUE;
    }
}

/*
 * ":smagic" and ":snomagic".
 */
    ساكن فراغ
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
    ساكن فراغ
ex_join(exarg_T *eap)
{
    curwin->w_cursor.lnum = eap->line1;
    إذا (eap->line1 == eap->line2)
    {
	إذا (eap->addr_گنتی >= 2)   // :2,2join does nothing
	    أرجع;
	إذا (eap->line2 == curbuf->b_ml.ml_سطر_گنتی)
	{
	    beep_flush();
	    أرجع;
	}
	++eap->line2;
    }
    (فراغ)do_join(eap->line2 - eap->line1 + 1, !eap->forceit, TRUE, TRUE, TRUE);
    beginline(BL_WHITE | BL_FIX);
    ex_may_print(eap);
}

/*
 * ":[addr]@r" or ":[addr]*r": execute register
 */
    ساكن فراغ
ex_at(exarg_T *eap)
{
    صحيح		c;
    صحيح		prev_len = typebuf.tb_len;

    curwin->w_cursor.lnum = eap->line2;
    check_cursor_col();

#ifdef USE_ON_FLY_SCROLL
    dont_scroll = TRUE;		// disallow scrolling here
#endif

    // get the register name.  No name means to use the previous one
    c = *eap->arg;
    إذا (c == NUL || (c == '*' && *eap->cmd == '*'))
	c = '@';
    // Put the register in the typeahead buffer with the "silent" flag.
    إذا (do_execreg(c, TRUE, vim_strchr(p_cpo, CPO_EXECBUF) != عدم, TRUE)
								      == FAIL)
    {
	beep_flush();
	أرجع;
    }

    صحيح	save_efr = exec_from_reg;

    exec_from_reg = TRUE;

    /*
     * Execute from the typeahead buffer.
     * Continue until the stuff buffer is empty and all added characters
     * have been consumed.
     */
    طالما (!stuff_empty() || typebuf.tb_len > prev_len)
	(فراغ)do_cmdline(عدم, getexline, عدم, DOCMD_NOWAIT|DOCMD_VERBOSE);

    exec_from_reg = save_efr;
}

/*
 * ":!".
 */
    ساكن فراغ
ex_bang(exarg_T *eap)
{
    do_bang(eap->addr_گنتی, eap, eap->forceit, TRUE, TRUE);
}

/*
 * ":undo".
 */
    ساكن فراغ
ex_undo(exarg_T *eap)
{
    إذا (eap->addr_گنتی == 1)	    // :undo 123
	undo_time(eap->line2, FALSE, FALSE, TRUE);
    وإلا
	u_undo(1);
}

#ifdef FEAT_PERSISTENT_UNDO
    ساكن فراغ
ex_wundo(exarg_T *eap)
{
    char_u hash[UNDO_HASH_SIZE];

    u_compute_hash(hash);
    u_write_undo(eap->arg, eap->forceit, curbuf, hash);
}

    ساكن فراغ
ex_rundo(exarg_T *eap)
{
    char_u hash[UNDO_HASH_SIZE];

    u_compute_hash(hash);
    u_read_undo(eap->arg, hash, عدم);
}
#endif

/*
 * ":redo".
 */
    ساكن فراغ
ex_redo(exarg_T *eap UNUSED)
{
    u_redo(1);
}

/*
 * ":earlier" and ":later".
 */
    ساكن فراغ
ex_later(exarg_T *eap)
{
    long	گنتی = 0;
    صحيح		sec = FALSE;
    صحيح		file = FALSE;
    char_u	*p = eap->arg;

    إذا (*p == NUL)
	گنتی = 1;
    وإلا إذا (SAFE_isdigit(*p))
    {
	گنتی = getdigits(&p);
	بدل (*p)
	{
	    حالة 's': ++p; sec = TRUE; اكسر;
	    حالة 'm': ++p; sec = TRUE; گنتی *= 60; اكسر;
	    حالة 'h': ++p; sec = TRUE; گنتی *= 60 * 60; اكسر;
	    حالة 'd': ++p; sec = TRUE; گنتی *= 24 * 60 * 60; اكسر;
	    حالة 'f': ++p; file = TRUE; اكسر;
	}
    }

    إذا (*p != NUL)
	semsg(_(e_invalid_argument_str), eap->arg);
    وإلا
	undo_time(eap->cmdidx == CMD_earlier ? -گنتی : گنتی,
							    sec, file, FALSE);
}

/*
 * ":redir": start/stop redirection.
 */
    ساكن فراغ
ex_redir(exarg_T *eap)
{
    حرف	*mode;
    char_u	*fname;
    char_u	*arg = eap->arg;

#ifdef FEAT_EVAL
    إذا (redir_execute)
    {
	emsg(_(e_cannot_use_redir_inside_execute));
	أرجع;
    }
#endif

    إذا (STRICMP(eap->arg, "END") == 0)
	close_redir();
    وإلا
    {
	إذا (*arg == '>')
	{
	    ++arg;
	    إذا (*arg == '>')
	    {
		++arg;
		mode = "a";
	    }
	    وإلا
		mode = "w";
	    arg = skipwhite(arg);

	    close_redir();

	    // Expand environment variables and "~/".
	    fname = expand_env_save(arg);
	    إذا (fname == عدم)
		أرجع;
#ifdef FEAT_BROWSE
	    إذا (cmdmod.cmod_flags & CMOD_BROWSE)
	    {
		char_u	*browseFile;

		browseFile = do_browse(BROWSE_SAVE,
			(char_u *)_("Save Redirection"),
			fname, عدم, عدم,
			(char_u *)_(BROWSE_FILTER_ALL_FILES), curbuf);
		إذا (browseFile == عدم)
		    أرجع;		// operation cancelled
		vim_free(fname);
		fname = browseFile;
		eap->forceit = TRUE;	// since dialog already asked
	    }
#endif

	    redir_fd = open_exfile(fname, eap->forceit, mode);
	    vim_free(fname);
	}
#ifdef FEAT_EVAL
	وإلا إذا (*arg == '@')
	{
	    // redirect to a register a-z (resp. A-Z for appending)
	    close_redir();
	    ++arg;
	    إذا (ASCII_ISALPHA(*arg)
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
		إذا (*arg == '>' && arg[1] == '>')  // append
		    arg += 2;
		وإلا
		{
		    // Can use both "@a" and "@a>".
		    إذا (*arg == '>')
			arg++;
		    // Make register empty when not using @A-@Z and the
		    // command is valid.
		    إذا (*arg == NUL && !SAFE_isupper(redir_reg))
			write_reg_contents(redir_reg, (char_u *)"", -1, FALSE);
		}
	    }
	    إذا (*arg != NUL)
	    {
		redir_reg = 0;
		semsg(_(e_invalid_argument_str), eap->arg);
	    }
	}
	وإلا إذا (*arg == '=' && arg[1] == '>')
	{
	    صحيح append;

	    // redirect to a variable
	    close_redir();
	    arg += 2;

	    إذا (*arg == '>')
	    {
		++arg;
		append = TRUE;
	    }
	    وإلا
		append = FALSE;

	    إذا (var_redir_start(skipwhite(arg), append) == OK)
		redir_vname = 1;
	}
#endif

	// TODO: redirect to a buffer

	وإلا
	    semsg(_(e_invalid_argument_str), eap->arg);
    }

    // Make sure redirection is not off.  Can happen for cmdline completion
    // that indirectly invokes a command to catch its output.
    إذا (redir_fd != عدم
#ifdef FEAT_EVAL
			  || redir_reg || redir_vname
#endif
							)
	redir_off = FALSE;
}

/*
 * ":redraw": force redraw, with clear for ":redraw!".
 */
    فراغ
ex_redraw(exarg_T *eap)
{
    redraw_cmd(eap->forceit);
}

/*
 * ":redraw": force redraw, with clear if "clear" is TRUE.
 */
    فراغ
redraw_cmd(صحيح clear)
{
    صحيح save_RedrawingDisabled = RedrawingDisabled;
    RedrawingDisabled = 0;

    صحيح save_p_lz = p_lz;
    p_lz = FALSE;

    validate_cursor();
    update_topline();
    update_screen(clear ? UPD_CLEAR : VIsual_active ? UPD_INVERTED : 0);
    إذا ((State & MODE_CMDLINE) == 0)
	setcursor(); // put cursor back where it belongs
    إذا (need_maketitle)
	maketitle();
#إذا defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
# ifdef VIMDLL
    إذا (!gui.in_use)
# endif
	resize_console_buf();
#endif
    RedrawingDisabled = save_RedrawingDisabled;
    p_lz = save_p_lz;

    // After drawing the statusline screen_attr may still be set.
    screen_stop_highlight();

    // Reset msg_didout, so that a message that's there is overwritten.
    پیغام_didout = FALSE;
    پیغام_col = 0;

    // No need to wait after an intentional redraw.
    need_wait_return = FALSE;

    // When invoked from a callback or autocmd the command line may be active.
    إذا (State & MODE_CMDLINE)
	redrawcmdline();

    out_flush();
}

/*
 * ":redrawstatus": force redraw of status line(s)
 */
    ساكن فراغ
ex_redrawstatus(exarg_T *eap UNUSED)
{
    إذا (eap->forceit)
	status_redraw_all();
    وإلا
	status_redraw_curbuf();
    redraw_vseps = TRUE;
    إذا (پیغام_scrolled && (State & MODE_CMDLINE))
	أرجع;  // redraw later

    صحيح save_RedrawingDisabled = RedrawingDisabled;
    RedrawingDisabled = 0;

    صحيح save_p_lz = p_lz;
    p_lz = FALSE;

    إذا (State & MODE_CMDLINE)
	redraw_statuslines();
    وإلا
	update_screen(VIsual_active ? UPD_INVERTED : 0);
    RedrawingDisabled = save_RedrawingDisabled;
    p_lz = save_p_lz;
    out_flush();
}

/*
 * ":redrawtabline": force redraw of the tabline
 */
    ساكن فراغ
ex_redrawtabline(exarg_T *eap UNUSED)
{
    صحيح save_RedrawingDisabled = RedrawingDisabled;
    RedrawingDisabled = 0;

    صحيح save_p_lz = p_lz;
    p_lz = FALSE;

    draw_tabline();

    RedrawingDisabled = save_RedrawingDisabled;
    p_lz = save_p_lz;
    out_flush();
}

/*
 * ":redrawtabpanel": force redraw of the tabpanel
 */
    ساكن فراغ
ex_redrawtabpanel(exarg_T *eap UNUSED)
{
    صحيح save_RedrawingDisabled = RedrawingDisabled;
    RedrawingDisabled = 0;

    صحيح save_p_lz = p_lz;
    p_lz = FALSE;

#إذا defined(FEAT_TABPANEL)
    draw_tabpanel();
#وإلا
    emsg(_(e_cannot_not_support_redrawtabpanel));
#endif

    RedrawingDisabled = save_RedrawingDisabled;
    p_lz = save_p_lz;
    out_flush();
}

    ساكن فراغ
close_redir(فراغ)
{
    إذا (redir_fd != عدم)
    {
	fclose(redir_fd);
	redir_fd = عدم;
    }
#ifdef FEAT_EVAL
    redir_reg = 0;
    إذا (redir_vname)
    {
	var_redir_stop();
	redir_vname = 0;
    }
#endif
}

#إذا defined(FEAT_SESSION) || defined(FEAT_EVAL)
    صحيح
vim_mkdir_emsg(char_u *name, صحيح prot UNUSED)
{
    إذا (vim_mkdir(name, prot) != 0)
    {
	semsg(_(e_cannot_create_directory_str), name);
	أرجع FAIL;
    }
    أرجع OK;
}
#endif

/*
 * Open a file for writing for an Ex command, with some checks.
 * Return file descriptor, or NULL on failure.
 */
    FILE *
open_exfile(
    char_u	*fname,
    صحيح		forceit,
    حرف	*mode)	    // "w" for create new file or "a" for append
{
    FILE	*fd;

#ifdef UNIX
    // with Unix it is possible to open a directory
    إذا (mch_isdir(fname))
    {
	semsg(_(e_str_is_directory), fname);
	أرجع عدم;
    }
#endif
    إذا (!forceit && *mode != 'a' && vim_fexists(fname))
    {
	semsg(_(e_str_exists_add_bang_to_override), fname);
	أرجع عدم;
    }

    إذا ((fd = mch_fopen((حرف *)fname, mode)) == عدم)
	semsg(_(e_cannot_open_str_for_writing_2), fname);

    أرجع fd;
}

/*
 * ":mark" and ":k".
 */
    ساكن فراغ
ex_mark(exarg_T *eap)
{
    pos_T	pos;

#ifdef FEAT_EVAL
    إذا (not_in_vim9(eap, false) == FAIL)
	أرجع;
#endif
    إذا (*eap->arg == NUL)		// No argument?
    {
	emsg(_(e_argument_required));
	أرجع;
    }

    إذا (eap->arg[1] != NUL)	// more than one character?
    {
	semsg(_(e_trailing_characters_str), eap->arg);
	أرجع;
    }

    pos = curwin->w_cursor;		// save curwin->w_cursor
    curwin->w_cursor.lnum = eap->line2;
    beginline(BL_WHITE | BL_FIX);
    إذا (setmark(*eap->arg) == FAIL)	// set mark
	emsg(_(e_argument_must_be_letter_or_forward_backward_quote));
    curwin->w_cursor = pos;		// restore curwin->w_cursor
}

/*
 * Update w_topline, w_leftcol and the cursor position.
 */
    فراغ
update_topline_cursor(فراغ)
{
    check_cursor();		// put cursor on valid line
    update_topline();
    إذا (!curwin->w_p_wrap)
	validate_cursor();
    update_curswant();
}

/*
 * Save the current State and go to Normal mode.
 * Return TRUE if the typeahead could be saved.
 */
    صحيح
save_current_state(save_state_T *sst)
{
    sst->save_پیغام_scroll = پیغام_scroll;
    sst->save_restart_edit = restart_edit;
    sst->save_پیغام_didout = پیغام_didout;
    sst->save_State = State;
    sst->save_insertmode = p_im;
    sst->save_finish_op = finish_op;
    sst->save_opcount = opcount;
    sst->save_reg_executing = reg_executing;
    sst->save_pending_end_reg_executing = pending_end_reg_executing;

    پیغام_scroll = FALSE;		    // no msg scrolling in Normal mode
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
    أرجع sst->tabuf.typebuf_valid;
}

    فراغ
restore_current_state(save_state_T *sst)
{
    // Restore the previous typeahead.
    restore_typeahead(&sst->tabuf, FALSE);

    پیغام_scroll = sst->save_پیغام_scroll;
    restart_edit = sst->save_restart_edit;
    p_im = sst->save_insertmode;
    finish_op = sst->save_finish_op;
    opcount = sst->save_opcount;
    reg_executing = sst->save_reg_executing;
    pending_end_reg_executing = sst->save_pending_end_reg_executing;
    پیغام_didout |= sst->save_پیغام_didout;	// don't reset msg_didout now
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
    فراغ
ex_normal(exarg_T *eap)
{
    save_state_T save_state;
    char_u	*arg = عدم;
    صحيح		l;
    char_u	*p;

    إذا (ex_normal_lock > 0)
    {
	emsg(_(e_not_allowed_here));
	أرجع;
    }
    إذا (ex_normal_busy >= p_mmd)
    {
	emsg(_(e_recursive_use_of_normal_too_deep));
	أرجع;
    }

    /*
     * vgetc() expects a CSI and K_SPECIAL to have been escaped.  Don't do
     * this for the K_SPECIAL leading byte, otherwise special keys will not
     * work.
     */
    إذا (has_mbyte)
    {
	صحيح	len = 0;

	// Count the number of characters to be escaped.
	لكل (p = eap->arg; *p != NUL; ++p)
	{
#ifdef FEAT_GUI
	    إذا (*p == CSI)  // leadbyte CSI
		len += 2;
#endif
	    لكل (l = (*mb_ptr2len)(p) - 1; l > 0; --l)
		إذا (*++p == K_SPECIAL	  // trailbyte K_SPECIAL or CSI
#ifdef FEAT_GUI
			|| *p == CSI
#endif
			)
		    len += 2;
	}
	إذا (len > 0)
	{
	    arg = alloc(STRLEN(eap->arg) + len + 1);
	    إذا (arg != عدم)
	    {
		len = 0;
		لكل (p = eap->arg; *p != NUL; ++p)
		{
		    arg[len++] = *p;
#ifdef FEAT_GUI
		    إذا (*p == CSI)
		    {
			arg[len++] = KS_EXTRA;
			arg[len++] = (صحيح)KE_CSI;
		    }
#endif
		    لكل (l = (*mb_ptr2len)(p) - 1; l > 0; --l)
		    {
			arg[len++] = *++p;
			إذا (*p == K_SPECIAL)
			{
			    arg[len++] = KS_SPECIAL;
			    arg[len++] = KE_FILLER;
			}
#ifdef FEAT_GUI
			وإلا إذا (*p == CSI)
			{
			    arg[len++] = KS_EXTRA;
			    arg[len++] = (صحيح)KE_CSI;
			}
#endif
		    }
		    arg[len] = NUL;
		}
	    }
	}
    }

    ++ex_normal_busy;
    إذا (save_current_state(&save_state))
    {
	/*
	 * Repeat the :normal command for each line in the range.  When no
	 * range given, execute it just once, without positioning the cursor
	 * first.
	 */
	افعل
	{
	    إذا (eap->addr_گنتی != 0)
	    {
		curwin->w_cursor.lnum = eap->line1++;
		curwin->w_cursor.col = 0;
		check_cursor_moved(curwin);
	    }

	    exec_normal_cmd(arg != عدم
		     ? arg
		     : eap->arg, eap->forceit ? REMAP_NONE : REMAP_YES, FALSE);
	}
	طالما (eap->addr_گنتی > 0 && eap->line1 <= eap->line2 && !got_int);
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
    ساكن فراغ
ex_startinsert(exarg_T *eap)
{
#ifdef FEAT_TERMINAL
    // Ignore this when running in an active terminal.
    إذا (term_job_running(curbuf->b_term))
	أرجع;
#endif
    إذا (!curbuf->b_p_ma && !p_im)
    {
	// Only give this error when 'insertmode' is off.
	emsg(_(e_cannot_make_changes_modifiable_is_off));
	أرجع;
    }
    إذا (eap->forceit)
    {
	// cursor line can be zero on startup
	إذا (!curwin->w_cursor.lnum)
	    curwin->w_cursor.lnum = 1;
	set_cursor_for_append_to_سطر();
    }

    // Ignore the command when already in Insert mode.  Inserting an
    // expression register that invokes a function can do this.
    إذا (State & MODE_داخل_کرو)
	أرجع;

    إذا (eap->cmdidx == CMD_startinsert)
	restart_edit = 'a';
    وإلا إذا (eap->cmdidx == CMD_startreplace)
	restart_edit = 'R';
    وإلا
	restart_edit = 'V';

    إذا (!eap->forceit)
    {
	إذا (eap->cmdidx == CMD_startinsert)
	    restart_edit = 'i';
	curwin->w_curswant = 0;	    // avoid MAXCOL
    }

    إذا (VIsual_active)
	showmode();
}

/*
 * ":stopinsert"
 */
    ساكن فراغ
ex_stopinsert(exarg_T *eap UNUSED)
{
    restart_edit = 0;
    stop_داخل_کرو_mode = TRUE;
#إذا defined(FEAT_CLIENTSERVER) || defined(FEAT_EVAL)
    // when called from remote_expr in insert mode, make sure insert mode is
    // ended by adding K_NOP to the typeahead buffer
    إذا (vgetc_busy)
	ins_char_typebuf(K_NOP, 0);
#endif
    clearmode();
}

/*
 * Execute normal mode command "cmd".
 * "remap" can be REMAP_NONE or REMAP_YES.
 */
    فراغ
exec_normal_cmd(char_u *cmd, صحيح remap, صحيح silent)
{
    // Stuff the argument into the typeahead buffer.
    ins_typebuf(cmd, remap, 0, TRUE, silent);
    exec_normal(FALSE, FALSE, FALSE);
}

/*
 * Execute normal_cmd() until there is no typeahead left.
 * When "use_vpeekc" is TRUE use vpeekc() to check for available chars.
 */
    فراغ
exec_normal(صحيح was_typed, صحيح use_vpeekc, صحيح may_use_terminal_loop UNUSED)
{
    oparg_T	oa;
    صحيح		c;

    // When calling vpeekc() from feedkeys() it will return Ctrl_C when there
    // is nothing to get, so also check for Ctrl_C.
    clear_oparg(&oa);
    finish_op = FALSE;
    طالما ((!stuff_empty()
		|| ((was_typed || !typebuf_typed()) && typebuf.tb_len > 0)
		|| (use_vpeekc && (c = vpeekc()) != NUL && c != Ctrl_C))
	    && !got_int)
    {
	update_topline_cursor();
#ifdef FEAT_TERMINAL
	إذا (may_use_terminal_loop && term_use_loop()
		&& oa.op_type == OP_NOP && oa.regname == NUL
		&& !VIsual_active)
	{
	    // If terminal_loop() returns OK we got a key that is handled
	    // in Normal model.  With FAIL we first need to position the
	    // cursor and the screen needs to be redrawn.
	    إذا (terminal_loop(TRUE) == OK)
		normal_cmd(&oa, TRUE);
	}
	وإلا
#endif
	    // execute a Normal mode cmd
	    normal_cmd(&oa, TRUE);
    }
}

#ifdef FEAT_FIND_ID
    ساكن فراغ
ex_checkpath(exarg_T *eap)
{
    find_pattern_in_path(عدم, 0, 0, FALSE, FALSE, CHECK_PATH, 1L,
	    eap->forceit ? ACTION_SHOW_ALL : ACTION_SHOW,
	    (linenr_T)1, (linenr_T)MAXLNUM, eap->forceit, FALSE);
}

# إذا defined(FEAT_QUICKFIX)
/*
 * ":psearch"
 */
    ساكن فراغ
ex_psearch(exarg_T *eap)
{
    g_do_tagpreview = p_pvh;
    ex_findpat(eap);
    g_do_tagpreview = 0;
}
# endif

    ساكن فراغ
ex_findpat(exarg_T *eap)
{
    صحيح		whole = TRUE;
    long	n;
    char_u	*p;
    صحيح		action;

    بدل (cmdnames[eap->cmdidx].cmd_name[2])
    {
	حالة 'e':	// ":psearch", ":isearch" and ":dsearch"
		إذا (cmdnames[eap->cmdidx].cmd_name[0] == 'p')
		    action = ACTION_GOTO;
		وإلا
		    action = ACTION_SHOW;
		اكسر;
	حالة 'i':	// ":ilist" and ":dlist"
		action = ACTION_SHOW_ALL;
		اكسر;
	حالة 'u':	// ":ijump" and ":djump"
		action = ACTION_GOTO;
		اكسر;
	افتراضي:	// ":isplit" and ":dsplit"
		action = ACTION_SPLIT;
		اكسر;
    }

    n = 1;
    إذا (vim_isdigit(*eap->arg))	// get count
    {
	n = getdigits(&eap->arg);
	eap->arg = skipwhite(eap->arg);
    }
    إذا (*eap->arg == '/')   // Match regexp, not just whole words
    {
	whole = FALSE;
	++eap->arg;
	p = skip_regexp(eap->arg, '/', magic_isset());
	إذا (*p)
	{
	    *p++ = NUL;
	    p = skipwhite(p);

	    // Check for trailing illegal characters
	    إذا (!ends_excmd2(eap->arg, p))
		eap->errmsg = ex_errmsg(e_trailing_characters_str, p);
	    وإلا
		set_nextcmd(eap, p);
	}
    }
    إذا (!eap->skip)
	find_pattern_in_path(eap->arg, 0, (صحيح)STRLEN(eap->arg),
		whole, !eap->forceit,
		*eap->cmd == 'd' ? FIND_DEFINE : FIND_ANY, n, action,
		eap->line1, eap->line2, eap->forceit, FALSE);
}
#endif


#ifdef FEAT_QUICKFIX
/*
 * ":ptag", ":ptselect", ":ptjump", ":ptnext", etc.
 */
    ساكن فراغ
ex_ptag(exarg_T *eap)
{
    g_do_tagpreview = p_pvh;  // will be reset to 0 in ex_tag_cmd()
    ex_tag_cmd(eap, cmdnames[eap->cmdidx].cmd_name + 1);
}

/*
 * ":pedit"
 */
    ساكن فراغ
ex_pedit(exarg_T *eap)
{
    win_T	*curwin_save = curwin;
    prepare_preview_window();

    // Edit the file.
    do_exedit(eap, عدم);

    back_to_current_window(curwin_save);
}

/*
 * ":pbuffer"
 */
    ساكن فراغ
ex_pbuffer(exarg_T *eap)
{
    win_T	*curwin_save = curwin;
    prepare_preview_window();

    // Go to the buffer.
    do_exbuffer(eap);

    back_to_current_window(curwin_save);
}

    ساكن فراغ
prepare_preview_window(فراغ)
{
    إذا (ERROR_IF_ANY_POPUP_WINDOW)
	أرجع;

    // Open the preview window or popup and make it the current window.
    g_do_tagpreview = p_pvh;
    prepare_tagpreview(TRUE, TRUE, FALSE);
}

    ساكن فراغ
back_to_current_window(win_T *curwin_save)
{
    إذا (curwin != curwin_save && win_valid(curwin_save))
    {
	// Return cursor to where we were
	validate_cursor();
	redraw_later(UPD_VALID);
	win_enter(curwin_save, TRUE);
    }
# ifdef FEAT_PROP_POPUP
    وإلا إذا (WIN_IS_POPUP(curwin))
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
    ساكن فراغ
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
    ساكن فراغ
ex_tag(exarg_T *eap)
{
    ex_tag_cmd(eap, cmdnames[eap->cmdidx].cmd_name);
}

    ساكن فراغ
ex_tag_cmd(exarg_T *eap, char_u *name)
{
    صحيح		cmd;

    بدل (name[1])
    {
	حالة 'j': cmd = DT_JUMP;	// ":tjump"
		  اكسر;
	حالة 's': cmd = DT_SELECT;	// ":tselect"
		  اكسر;
	حالة 'p': cmd = DT_PREV;	// ":tprevious"
		  اكسر;
	حالة 'N': cmd = DT_PREV;	// ":tNext"
		  اكسر;
	حالة 'n': cmd = DT_NEXT;	// ":tnext"
		  اكسر;
	حالة 'o': cmd = DT_نکالو;		// ":pop"
		  اكسر;
	حالة 'f':			// ":tfirst"
	حالة 'r': cmd = DT_FIRST;	// ":trewind"
		  اكسر;
	حالة 'l': cmd = DT_LAST;	// ":tlast"
		  اكسر;
	افتراضي:			// ":tag"
#ifdef FEAT_CSCOPE
		  إذا (p_cst && *eap->arg != NUL)
		  {
		      ex_cstag(eap);
		      أرجع;
		  }
#endif
		  cmd = DT_TAG;
		  اكسر;
    }

    إذا (name[0] == 'l')
    {
#ifndef FEAT_QUICKFIX
	ex_ni(eap);
	أرجع;
#وإلا
	cmd = DT_LTAG;
#endif
    }

    do_tag(eap->arg, cmd, eap->addr_گنتی > 0 ? (صحيح)eap->line2 : 1,
							  eap->forceit, TRUE);
}

تعداد {
    SPEC_PERC = 0,
    SPEC_HASH,
    SPEC_CWORD,	    // cursor word
    SPEC_CCWORD,    // cursor WORD
    SPEC_CEXPR,	    // expr under cursor
    SPEC_CFILE,	    // cursor path name
    SPEC_SFILE,	    // ":so" file name
    SPEC_SLNUM,	    // ":so" file line number
    SPEC_ڈھیر,	    // call stack
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
    صحيح
find_cmdline_var(char_u *src, size_t *usedlen)
{
    // must be sorted by the 'value' field because it is used by bsearch()!
    ساكن keyvalue_T spec_str_tab[] = {
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
	KEYVALUE_ENTRY(SPEC_ڈھیر, "stack>")	    // call stack
    };
    keyvalue_T target;
    keyvalue_T *entry;

    بدل (*src)
    {
    حالة '%':
	*usedlen = 1;
	أرجع SPEC_PERC;

    حالة '#':
	*usedlen = 1;
	أرجع SPEC_HASH;

    حالة '<':
	target.کلید = 0;
	target.قدر.string = src + 1;	    // skip over '<'
	target.قدر.length = 0;	    // not used, see cmp_keyvalue_value_n()

	entry = (keyvalue_T *)bsearch(&target, &spec_str_tab,
		ARRAY_LENGTH(spec_str_tab), حجم(spec_str_tab[0]),
		cmp_keyvalue_قدر_n);
	إذا (entry == عدم)
	    أرجع -1;

	*usedlen = entry->قدر.length + 1;
	أرجع entry->کلید;

    افتراضي:
	اكسر;
    }

    أرجع -1;
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
    حرف	**errormsg,	// pointer to error message
    صحيح		*escaped,	// return value has escaped white space (can
				// be NULL)
    صحيح		empty_is_error)	// empty result is considered an error
{
    صحيح		i;
    char_u	*s;
    char_u	*result = (char_u *)"";
    char_u	*resultbuf = عدم;
    size_t	resultlen;
    buf_T	*buf;
    صحيح		valid = VALID_HEAD + VALID_PATH;    // assume valid result
    صحيح		spec_idx;
    صحيح		tilde_file = FALSE;
    صحيح		skip_mod = FALSE;
    char_u	strbuf[30];

    *errormsg = عدم;
    إذا (escaped != عدم)
	*escaped = FALSE;

    /*
     * Check if there is something to do.
     */
    spec_idx = find_cmdline_var(src, usedlen);
    إذا (spec_idx < 0)	// no match
    {
	*usedlen = 1;
	أرجع عدم;
    }

    /*
     * Skip when preceded with a backslash "\%" and "\#".
     * Note: In "\\%" the % is also not recognized!
     */
    إذا (src > srcstart && src[-1] == '\\')
    {
	*usedlen = 0;
	STRMOVE(src - 1, src);	// remove backslash
	أرجع عدم;
    }

    /*
     * word or WORD under cursor
     */
    إذا (spec_idx == SPEC_CWORD || spec_idx == SPEC_CCWORD
						     || spec_idx == SPEC_CEXPR)
    {
	resultlen = find_ident_under_cursor(&result,
		spec_idx == SPEC_CWORD ? (FIND_IDENT | FIND_STRING)
	      : spec_idx == SPEC_CEXPR ? (FIND_IDENT | FIND_STRING | FIND_EVAL)
	      : FIND_STRING);
	إذا (resultlen == 0)
	{
	    *errormsg = "";
	    أرجع عدم;
	}
    }

    /*
     * '#': Alternate file name
     * '%': Current file name
     *	    File name under the cursor
     *	    File name for autocommand
     *	and following modifiers
     */
    وإلا
    {
	size_t off = 0;

	بدل (spec_idx)
	{
	حالة SPEC_PERC:
#ifdef FEAT_EVAL
		إذا (!in_vim9script() || src[1] != '%')
#endif
		{
		    // '%': current file
		    إذا (curbuf->b_fname == عدم)
		    {
			result = (char_u *)"";
			valid = 0;	    // Must have ":p:h" to be valid
		    }
		    وإلا
		    {
			result = curbuf->b_fname;
			tilde_file = STRCMP(result, "~") == 0;
		    }
		    اكسر;
		}
#ifdef FEAT_EVAL
		// "%%" alternate file
		off = 1;
#endif
		// FALLTHROUGH
	حالة SPEC_HASH:		// '#' or "#99": alternate file
		إذا (off == 0 ? src[1] == '#' : src[2] == '%')
		{
		    // "##" or "%%%": the argument list
		    result = arg_all();
		    resultbuf = result;
		    *usedlen = off + 2;
		    إذا (escaped != عدم)
			*escaped = TRUE;
		    skip_mod = TRUE;
		    اكسر;
		}
		s = src + off + 1;
		إذا (*s == '<')		// "#<99" uses v:oldfiles
		    ++s;
		i = (صحيح)getdigits(&s);
		إذا (s == src + off + 2 && src[off + 1] == '-')
		    // just a minus sign, don't skip over it
		    s--;
		*usedlen = (صحيح)(s - src); // length of what we expand

		إذا (src[off + 1] == '<' && i != 0)
		{
		    إذا (*usedlen < off + 2)
		    {
			// Should we give an error message for #<text?
			*usedlen = off + 1;
			أرجع عدم;
		    }
#ifdef FEAT_EVAL
		    result = list_find_str(get_vim_var_list(VV_OLDFILES),
								     (long)i);
		    إذا (result == عدم)
		    {
			*errormsg = "";
			أرجع عدم;
		    }
#وإلا
		    *errormsg = _(e_hashsmall_is_not_available_without_the_eval_feature);
		    أرجع عدم;
#endif
		}
		وإلا
		{
		    إذا (i == 0 && src[off + 1] == '<' && *usedlen > off + 1)
			*usedlen = off + 1;
		    buf = buflist_findnr(i);
		    إذا (buf == عدم)
		    {
			*errormsg = _(e_no_alternate_file_name_to_substitute_for_hash);
			أرجع عدم;
		    }
		    إذا (lnump != عدم)
			*lnump = ECMD_LAST;
		    إذا (buf->b_fname == عدم)
		    {
			result = (char_u *)"";
			valid = 0;	    // Must have ":p:h" to be valid
		    }
		    وإلا
		    {
			result = buf->b_fname;
			tilde_file = STRCMP(result, "~") == 0;
		    }
		}
		اكسر;

	حالة SPEC_CFILE:	// file name under cursor
		result = file_name_at_cursor(FNAME_MESS|FNAME_HYP, 1L, عدم);
		إذا (result == عدم)
		{
		    *errormsg = "";
		    أرجع عدم;
		}
		resultbuf = result;	    // remember allocated string
		اكسر;

	حالة SPEC_AFILE:	// file name for autocommand
		result = autocmd_fname;
		إذا (result != عدم && !autocmd_fname_full)
		{
		    // Still need to turn the fname into a full path.  It is
		    // postponed to avoid a delay when <afile> is not used.
		    autocmd_fname_full = TRUE;
		    result = FullName_save(autocmd_fname, FALSE);
		    vim_free(autocmd_fname);
		    autocmd_fname = result;
		}
		إذا (result == عدم)
		{
		    *errormsg = _(e_no_autocommand_file_name_to_substitute_for_afile);
		    أرجع عدم;
		}
		result = shorten_fname1(result);
		اكسر;

	حالة SPEC_ABUF:		// buffer number for autocommand
		إذا (autocmd_bufnr <= 0)
		{
		    *errormsg = _(e_no_autocommand_buffer_نمبر_to_substitute_for_abuf);
		    أرجع عدم;
		}
		sprintf((حرف *)strbuf, "%d", autocmd_bufnr);
		result = strbuf;
		اكسر;

	حالة SPEC_AMATCH:	// match name for autocommand
		result = autocmd_match;
		إذا (result == عدم)
		{
		    *errormsg = _(e_no_autocommand_match_name_to_substitute_for_amatch);
		    أرجع عدم;
		}
		اكسر;

	حالة SPEC_SFILE:	// file name for ":so" command
		result = estack_sfile(ESTACK_SFILE);
		إذا (result == عدم)
		{
		    *errormsg = _(e_no_source_file_name_to_substitute_for_sfile);
		    أرجع عدم;
		}
		resultbuf = result;	    // remember allocated string
		اكسر;
	حالة SPEC_ڈھیر:	// call stack
		result = estack_sfile(ESTACK_ڈھیر);
		إذا (result == عدم)
		{
		    *errormsg = _(e_no_call_ڈھیر_to_substitute_for_ڈھیر);
		    أرجع عدم;
		}
		resultbuf = result;	    // remember allocated string
		اكسر;
	حالة SPEC_SCRIPT:	// script file name
		result = estack_sfile(ESTACK_SCRIPT);
		إذا (result == عدم)
		{
		    *errormsg = _(e_no_script_file_name_to_substitute_for_script);
		    أرجع عدم;
		}
		resultbuf = result;	    // remember allocated string
		اكسر;

	حالة SPEC_SLNUM:	// line in file for ":so" command
		إذا (SOURCING_NAME == عدم || SOURCING_LNUM == 0)
		{
		    *errormsg = _(e_no_سطر_نمبر_to_use_for_slnum);
		    أرجع عدم;
		}
		sprintf((حرف *)strbuf, "%ld", SOURCING_LNUM);
		result = strbuf;
		اكسر;

#ifdef FEAT_EVAL
	حالة SPEC_SFLNUM:	// line in script file
		إذا (current_sctx.sc_lnum + SOURCING_LNUM == 0)
		{
		    *errormsg = _(e_no_سطر_نمبر_to_use_for_sflnum);
		    أرجع عدم;
		}
		sprintf((حرف *)strbuf, "%ld",
				 (long)(current_sctx.sc_lnum + SOURCING_LNUM));
		result = strbuf;
		اكسر;

	حالة SPEC_SID:
		إذا (current_sctx.sc_sid <= 0)
		{
		    *errormsg = _(e_using_sid_not_in_script_context);
		    أرجع عدم;
		}
		sprintf((حرف *)strbuf, "<SNR>%d_", current_sctx.sc_sid);
		result = strbuf;
		اكسر;
#endif

#ifdef FEAT_CLIENTSERVER
	حالة SPEC_CLIENT:	// Source of last submitted input
# ifdef MSWIN
		sprintf((حرف *)strbuf, PRINTF_HEX_LONG_U,
							(long_u)clientWindow);
		result = strbuf;
# وإلا
#  ifdef FEAT_SOCKETSERVER
		إذا (clientserver_method == CLIENTSERVER_METHOD_SOCKET)
		{
		    إذا (client_socket == عدم)
			result = (char_u *)"";
		    وإلا
			result = client_socket;
		}
#  endif
#  ifdef FEAT_X11
		إذا (clientserver_method == CLIENTSERVER_METHOD_X11)
		{
		    sprintf((حرف *)strbuf, PRINTF_HEX_LONG_U,
							(long_u)clientWindow);
		    result = strbuf;
		}
#  endif
# endif
		اكسر;
#endif
	}

	resultlen = STRLEN(result);	// length of new string
	إذا (src[*usedlen] == '<')	// remove the file name extension
	{
	    ++*usedlen;
	    إذا ((s = vim_strrchr(result, '.')) != عدم && s >= gettail(result))
		resultlen = s - result;
	}
	وإلا إذا (!skip_mod)
	{
	    valid |= modify_fname(src, tilde_file, usedlen, &result, &resultbuf,
								  &resultlen);
	    إذا (result == عدم)
	    {
		*errormsg = "";
		أرجع عدم;
	    }
	}
    }

    إذا (resultlen == 0 || valid != VALID_HEAD + VALID_PATH)
    {
	إذا (empty_is_error)
	{
	    إذا (valid != VALID_HEAD + VALID_PATH)
		*errormsg = _(e_empty_file_name_for_percent_or_hash_only_works_with_ph);
	    وإلا
		*errormsg = _(e_evaluates_to_an_empty_string);
	}
	result = عدم;
    }
    وإلا
	result = vim_strnsave(result, resultlen);
    vim_free(resultbuf);
    أرجع result;
}

/*
 * Expand the <sfile> string in "arg".
 *
 * Returns an allocated string, or NULL for any error.
 */
    char_u *
expand_sfile(char_u *arg)
{
    حرف	*errormsg;
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
    إذا (result == عدم)
	أرجع عدم;

    لكل (p = result; *p; )
    {
	إذا (STRNCMP(p, "<sfile>", 7) != 0)
	    ++p;
	وإلا
	{
	    // replace "<sfile>" with the sourced file name, and do ":" stuff
	    repl = eval_vars(p, result, &srclen, عدم, &errormsg, عدم, TRUE);
	    إذا (errormsg != عدم)
	    {
		إذا (*errormsg)
		    emsg(errormsg);
		vim_free(result);
		أرجع عدم;
	    }
	    إذا (repl == عدم)		// no match (cannot happen)
	    {
		p += srclen;
		استمر;
	    }
	    repllen = STRLEN(repl);
	    resultlen += repllen - srclen;
	    newres = alloc(resultlen + 1);
	    إذا (newres == عدم)
	    {
		vim_free(repl);
		vim_free(result);
		أرجع عدم;
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

    أرجع result;
}

#إذا defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
/*
 * Make a dialog message in "buff[DIALOG_MSG_SIZE]".
 * "format" must contain "%s".
 */
    فراغ
dialog_پیغام(char_u *buff, حرف *format, char_u *fname)
{
    إذا (fname == عدم)
	fname = (char_u *)_("Untitled");
    vim_snprintf((حرف *)buff, DIALOG_پیغام_SIZE, format, fname);
}
#endif

/*
 * ":behave {mswin,xterm}"
 */
    ساكن فراغ
ex_behave(exarg_T *eap)
{
    إذا (STRCMP(eap->arg, "mswin") == 0)
    {
	set_option_قدر_give_err((char_u *)"selection",
						 0L, (char_u *)"exclusive", 0);
	set_option_قدر_give_err((char_u *)"selectmode",
						 0L, (char_u *)"mouse,key", 0);
	set_option_قدر_give_err((char_u *)"mousemodel",
						     0L, (char_u *)"popup", 0);
	set_option_قدر_give_err((char_u *)"keymodel",
					  0L, (char_u *)"startsel,stopsel", 0);
    }
    وإلا إذا (STRCMP(eap->arg, "xterm") == 0)
    {
	set_option_قدر_give_err((char_u *)"selection",
						 0L, (char_u *)"inclusive", 0);
	set_option_قدر_give_err((char_u *)"selectmode", 0L, (char_u *)"", 0);
	set_option_قدر_give_err((char_u *)"mousemodel",
						    0L, (char_u *)"extend", 0);
	set_option_قدر_give_err((char_u *)"keymodel", 0L, (char_u *)"", 0);
    }
    وإلا
	semsg(_(e_invalid_argument_str), eap->arg);
}

ساكن صحيح filetype_detect = FALSE;
ساكن صحيح filetype_plugin = FALSE;
ساكن صحيح filetype_indent = FALSE;

/*
 * ":filetype [plugin] [indent] {on,off,detect}"
 * on: Load the filetype.vim file to install autocommands for file types.
 * off: Load the ftoff.vim file to remove all autocommands for file types.
 * plugin on: load filetype.vim and ftplugin.vim
 * plugin off: load ftplugof.vim
 * indent on: load filetype.vim and indent.vim
 * indent off: load indoff.vim
 */
    ساكن فراغ
ex_filetype(exarg_T *eap)
{
    char_u	*arg = eap->arg;
    صحيح		plugin = FALSE;
    صحيح		indent = FALSE;

    إذا (*eap->arg == NUL)
    {
	// Print current status.
	smsg("filetype detection:%s  plugin:%s  indent:%s",
		filetype_detect ? "ON" : "OFF",
		filetype_plugin ? (filetype_detect ? "ON" : "(on)") : "OFF",
		filetype_indent ? (filetype_detect ? "ON" : "(on)") : "OFF");
	أرجع;
    }

    // Accept "plugin" and "indent" in any order.
    لكل (;;)
    {
	إذا (STRNCMP(arg, "plugin", 6) == 0)
	{
	    plugin = TRUE;
	    arg = skipwhite(arg + 6);
	    استمر;
	}
	إذا (STRNCMP(arg, "indent", 6) == 0)
	{
	    indent = TRUE;
	    arg = skipwhite(arg + 6);
	    استمر;
	}
	اكسر;
    }
    إذا (STRCMP(arg, "on") == 0 || STRCMP(arg, "detect") == 0)
    {
	إذا (*arg == 'o' || !filetype_detect)
	{
	    source_runtime((char_u *)FILETYPE_FILE, DIP_ALL);
	    filetype_detect = TRUE;
	    إذا (plugin)
	    {
		source_runtime((char_u *)FTPLUGIN_FILE, DIP_ALL);
		filetype_plugin = TRUE;
	    }
	    إذا (indent)
	    {
		source_runtime((char_u *)INDENT_FILE, DIP_ALL);
		filetype_indent = TRUE;
	    }
	}
	إذا (*arg == 'd')
	{
	    (فراغ)do_doautocmd((char_u *)"filetypedetect BufRead", TRUE, عدم);
	    do_modelines(0);
	}
    }
    وإلا إذا (STRCMP(arg, "off") == 0)
    {
	إذا (plugin || indent)
	{
	    إذا (plugin)
	    {
		source_runtime((char_u *)FTPLUGOF_FILE, DIP_ALL);
		filetype_plugin = FALSE;
	    }
	    إذا (indent)
	    {
		source_runtime((char_u *)INDOFF_FILE, DIP_ALL);
		filetype_indent = FALSE;
	    }
	}
	وإلا
	{
	    source_runtime((char_u *)FTOFF_FILE, DIP_ALL);
	    filetype_detect = FALSE;
	}
    }
    وإلا
	semsg(_(e_invalid_argument_str), arg);
}

/*
 * ":setfiletype [FALLBACK] {name}"
 */
    ساكن فراغ
ex_setfiletype(exarg_T *eap)
{
    إذا (curbuf->b_did_filetype)
	أرجع;

    char_u *arg = eap->arg;
    إذا (STRNCMP(arg, "FALLBACK ", 9) == 0)
	arg += 9;

    set_option_قدر_give_err((char_u *)"filetype", 0L, arg, OPT_LOCAL);
    إذا (arg != eap->arg)
	curbuf->b_did_filetype = false;
}

    ساكن فراغ
ex_digraphs(exarg_T *eap UNUSED)
{
#ifdef FEAT_DIGRAPHS
    إذا (*eap->arg != NUL)
	putdigraph(eap->arg);
    وإلا
	listdigraphs(eap->forceit);
#وإلا
    emsg(_(e_no_digraphs_version));
#endif
}

#إذا defined(FEAT_تلاش_EXTRA)
    فراغ
set_no_hlsearch(صحيح flag)
{
    no_hlsearch = flag;
# ifdef FEAT_EVAL
    set_vim_var_nr(VV_HLSEARCH, !no_hlsearch && p_hls);
# endif
}

/*
 * ":nohlsearch"
 */
    ساكن فراغ
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
    ساكن فراغ
ex_X(exarg_T *eap UNUSED)
{
    crypt_check_current_method();
    (فراغ)crypt_get_کلید(TRUE, TRUE);
}
#endif

#ifdef FEAT_FOLDING
    ساكن فراغ
ex_fold(exarg_T *eap)
{
    إذا (foldManualAllowed(TRUE))
	foldCreate(eap->line1, eap->line2);
}

    ساكن فراغ
ex_foldopen(exarg_T *eap)
{
    opFoldRange(eap->line1, eap->line2, eap->cmdidx == CMD_foldopen,
							 eap->forceit, FALSE);
}

    ساكن فراغ
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
    لكل (lnum = eap->line1; lnum <= eap->line2; ++lnum)
	إذا (hasFolding(lnum, عدم, عدم) == (eap->cmdidx == CMD_folddoclosed))
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

#إذا defined(FEAT_QUICKFIX)
/*
 * Returns TRUE if the supplied Ex cmdidx is for a location list command
 * instead of a quickfix command.
 */
    صحيح
is_loclist_cmd(صحيح cmdidx)
{
    إذا (cmdidx < 0 || cmdidx >= CMD_SIZE)
	أرجع FALSE;
    أرجع cmdnames[cmdidx].cmd_name[0] == 'l';
}
#endif

    صحيح
get_pressedreturn(فراغ)
{
    أرجع ex_pressedreturn;
}

    فراغ
set_pressedreturn(صحيح val)
{
    ex_pressedreturn = val;
}
