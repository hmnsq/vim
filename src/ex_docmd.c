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

#अन्तर्भाव "vim.h"

स्थिर पूर्णांक	quitmore = 0;
स्थिर पूर्णांक	ex_pressedreturn = FALSE;
#ifndef FEAT_PRINTER
# define ex_hardcopy	ex_ni
#endif

#ifdef FEAT_EVAL
स्थिर char_u	*do_one_cmd(char_u **, पूर्णांक, cstack_T *, char_u *(*fgetline)(पूर्णांक, शून्य *, पूर्णांक, getline_opt_T), शून्य *cookie);
#अन्यथा
स्थिर char_u	*do_one_cmd(char_u **, पूर्णांक, char_u *(*fgetline)(पूर्णांक, शून्य *, पूर्णांक, getline_opt_T), शून्य *cookie);
स्थिर पूर्णांक	if_level = 0;		// depth in :if
#endif
स्थिर शून्य	append_command(char_u *cmd);

#ifndef FEAT_MENU
# define ex_emenu		ex_ni
# define ex_menu		ex_ni
# define ex_menutranslate	ex_ni
#endif
स्थिर शून्य	ex_autocmd(exarg_T *eap);
स्थिर शून्य	ex_doautocmd(exarg_T *eap);
स्थिर शून्य	ex_bunload(exarg_T *eap);
स्थिर शून्य	ex_buffer(exarg_T *eap);
स्थिर शून्य	do_exbuffer(exarg_T *eap);
स्थिर शून्य	ex_bmodified(exarg_T *eap);
स्थिर शून्य	ex_bnext(exarg_T *eap);
स्थिर शून्य	ex_bprevious(exarg_T *eap);
स्थिर शून्य	ex_brewind(exarg_T *eap);
स्थिर शून्य	ex_blast(exarg_T *eap);
स्थिर char_u	*getargcmd(char_u **);
स्थिर पूर्णांक	getargopt(exarg_T *eap);
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
#जर !defined(FEAT_QUICKFIX) || !defined(FEAT_EVAL)
# define ex_cexpr		ex_ni
#endif

स्थिर linenr_T default_address(exarg_T *eap);
स्थिर शून्य address_default_all(exarg_T *eap);
स्थिर शून्य	get_flags(exarg_T *eap);
#जर !defined(FEAT_PERL) \
	|| !defined(FEAT_PYTHON) || !defined(FEAT_PYTHON3) \
	|| !defined(FEAT_TCL) \
	|| !defined(FEAT_RUBY) \
	|| !defined(FEAT_LUA) \
	|| !defined(FEAT_MZSCHEME)
# define HAVE_EX_SCRIPT_NI
स्थिर शून्य	ex_script_ni(exarg_T *eap);
#endif
स्थिर अक्षर	*invalid_range(exarg_T *eap);
स्थिर शून्य	correct_range(exarg_T *eap);
#ifdef FEAT_QUICKFIX
स्थिर char_u	*replace_makeprg(exarg_T *eap, char_u *p, char_u **cmdlinep);
#endif
स्थिर char_u	*repl_cmdline(exarg_T *eap, char_u *src, माप_प्रकार srclen, char_u *repl, char_u **cmdlinep);
स्थिर शून्य	ex_highlight(exarg_T *eap);
स्थिर शून्य	ex_colorscheme(exarg_T *eap);
स्थिर शून्य	ex_cquit(exarg_T *eap);
स्थिर शून्य	ex_quit_all(exarg_T *eap);
स्थिर शून्य	ex_close(exarg_T *eap);
स्थिर शून्य	ex_win_close(पूर्णांक forceit, win_T *win, tabpage_T *tp);
स्थिर शून्य	ex_only(exarg_T *eap);
स्थिर शून्य	ex_resize(exarg_T *eap);
स्थिर शून्य	ex_stag(exarg_T *eap);
स्थिर शून्य	ex_tabclose(exarg_T *eap);
स्थिर शून्य	ex_tabonly(exarg_T *eap);
स्थिर शून्य	ex_tabnext(exarg_T *eap);
स्थिर शून्य	ex_tabmove(exarg_T *eap);
स्थिर शून्य	ex_tabs(exarg_T *eap);
#जर defined(FEAT_QUICKFIX)
स्थिर शून्य	ex_pclose(exarg_T *eap);
स्थिर शून्य	ex_ptag(exarg_T *eap);
स्थिर शून्य	ex_pedit(exarg_T *eap);
स्थिर शून्य	ex_pbuffer(exarg_T *eap);
स्थिर शून्य	prepare_preview_window(शून्य);
स्थिर शून्य	back_to_current_window(win_T *curwin_save);
#अन्यथा
# define ex_pclose		ex_ni
# define ex_ptag		ex_ni
# define ex_pedit		ex_ni
# define ex_pbuffer		ex_ni
#endif
स्थिर शून्य	ex_hide(exarg_T *eap);
स्थिर शून्य	ex_exit(exarg_T *eap);
स्थिर शून्य	ex_print(exarg_T *eap);
#ifdef FEAT_BYTEOFF
स्थिर शून्य	ex_goto(exarg_T *eap);
#अन्यथा
# define ex_goto		ex_ni
#endif
स्थिर शून्य	ex_shell(exarg_T *eap);
स्थिर शून्य	ex_preserve(exarg_T *eap);
स्थिर शून्य	ex_recover(exarg_T *eap);
स्थिर शून्य	ex_mode(exarg_T *eap);
स्थिर शून्य	ex_wrongmodifier(exarg_T *eap);
स्थिर शून्य	ex_find(exarg_T *eap);
स्थिर शून्य	ex_open(exarg_T *eap);
स्थिर शून्य	ex_edit(exarg_T *eap);
#ifndef FEAT_GUI
# define ex_gui			ex_nogui
स्थिर शून्य	ex_nogui(exarg_T *eap);
#endif
#जर defined(FEAT_GUI_MSWIN) && defined(FEAT_MENU) && defined(FEAT_TEAROFF)
स्थिर शून्य	ex_tearoff(exarg_T *eap);
#अन्यथा
# define ex_tearoff		ex_ni
#endif
#जर (defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_GTK) \
	|| defined(FEAT_TERM_POPUP_MENU)) && defined(FEAT_MENU)
स्थिर शून्य	ex_popup(exarg_T *eap);
#अन्यथा
# define ex_popup		ex_ni
#endif
#ifndef FEAT_GUI_MSWIN
# define ex_simalt		ex_ni
#endif
#जर !defined(FEAT_GUI_MSWIN) && !defined(FEAT_GUI_GTK) && !defined(FEAT_GUI_MOTIF)
# define gui_mch_find_dialog	ex_ni
# define gui_mch_replace_dialog ex_ni
#endif
#जर !defined(FEAT_GUI_GTK)
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
#जर !defined(FEAT_SYN_HL) || !defined(FEAT_PROFILE)
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
#जर !defined(FEAT_PYTHON) && !defined(FEAT_PYTHON3)
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
स्थिर शून्य	ex_swapname(exarg_T *eap);
स्थिर शून्य	ex_syncbind(exarg_T *eap);
स्थिर शून्य	ex_read(exarg_T *eap);
स्थिर शून्य	ex_pwd(exarg_T *eap);
स्थिर शून्य	ex_equal(exarg_T *eap);
स्थिर शून्य	ex_sleep(exarg_T *eap);
स्थिर शून्य	ex_winsize(exarg_T *eap);
स्थिर शून्य	ex_wincmd(exarg_T *eap);
#जर defined(FEAT_GUI) || defined(UNIX) || defined(VMS) || defined(MSWIN)
स्थिर शून्य	ex_winpos(exarg_T *eap);
#अन्यथा
# define ex_winpos	    ex_ni
#endif
स्थिर शून्य	ex_operators(exarg_T *eap);
स्थिर शून्य	ex_put(exarg_T *eap);
स्थिर शून्य	ex_iput(exarg_T *eap);
स्थिर शून्य	ex_copymove(exarg_T *eap);
स्थिर शून्य	ex_submagic(exarg_T *eap);
स्थिर शून्य	ex_join(exarg_T *eap);
स्थिर शून्य	ex_at(exarg_T *eap);
स्थिर शून्य	ex_bang(exarg_T *eap);
स्थिर शून्य	ex_undo(exarg_T *eap);
#ifdef FEAT_PERSISTENT_UNDO
स्थिर शून्य	ex_wundo(exarg_T *eap);
स्थिर शून्य	ex_rundo(exarg_T *eap);
#endif
स्थिर शून्य	ex_redo(exarg_T *eap);
स्थिर शून्य	ex_later(exarg_T *eap);
स्थिर शून्य	ex_redir(exarg_T *eap);
स्थिर शून्य	ex_redrawstatus(exarg_T *eap);
स्थिर शून्य	ex_redrawtabline(exarg_T *eap);
स्थिर शून्य	ex_redrawtabpanel(exarg_T *eap);
स्थिर शून्य	close_redir(शून्य);
स्थिर शून्य	ex_mark(exarg_T *eap);
स्थिर शून्य	ex_startinsert(exarg_T *eap);
स्थिर शून्य	ex_stopinsert(exarg_T *eap);
#ifdef FEAT_FIND_ID
स्थिर शून्य	ex_checkpath(exarg_T *eap);
स्थिर शून्य	ex_findpat(exarg_T *eap);
#अन्यथा
# define ex_findpat		ex_ni
# define ex_checkpath		ex_ni
#endif
#जर defined(FEAT_FIND_ID) && defined(FEAT_QUICKFIX)
स्थिर शून्य	ex_psearch(exarg_T *eap);
#अन्यथा
# define ex_psearch		ex_ni
#endif
स्थिर शून्य	ex_tag(exarg_T *eap);
स्थिर शून्य	ex_tag_cmd(exarg_T *eap, char_u *name);
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
स्थिर शून्य	ex_behave(exarg_T *eap);
स्थिर शून्य	ex_filetype(exarg_T *eap);
स्थिर शून्य	ex_setfiletype(exarg_T *eap);
#ifndef FEAT_DIFF
# define ex_diffoff		ex_ni
# define ex_diffpatch		ex_ni
# define ex_diffgetput		ex_ni
# define ex_diffsplit		ex_ni
# define ex_diffthis		ex_ni
# define ex_diffupdate		ex_ni
#endif
स्थिर शून्य	ex_digraphs(exarg_T *eap);
#ifdef FEAT_शोध_EXTRA
स्थिर शून्य	ex_nohlsearch(exarg_T *eap);
#अन्यथा
# define ex_nohlsearch		ex_ni
# define ex_match		ex_ni
#endif
#ifdef FEAT_CRYPT
स्थिर शून्य	ex_X(exarg_T *eap);
#अन्यथा
# define ex_X			ex_ni
#endif
#ifdef FEAT_FOLDING
स्थिर शून्य	ex_fold(exarg_T *eap);
स्थिर शून्य	ex_foldopen(exarg_T *eap);
स्थिर शून्य	ex_folddo(exarg_T *eap);
#अन्यथा
# define ex_fold		ex_ni
# define ex_foldopen		ex_ni
# define ex_folddo		ex_ni
#endif
#जर !(defined(HAVE_LOCALE_H) || defined(X_LOCALE))
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
#जर !defined(FEAT_X11) || !defined(FEAT_XCLIPBOARD)
# define ex_xrestore		ex_ni
#endif
#जर !defined(FEAT_WAYLAND)
# define ex_wlrestore		ex_ni
#endif
#जर !defined(HAVE_CLIPMETHOD)
# define ex_clipreset		ex_ni
#endif
#जर !defined(FEAT_PROP_POPUP)
# define ex_popupclear		ex_ni
#endif

/*
 * Declare cmdnames[].
 */
#define DO_DECLARE_EXCMD
#अन्तर्भाव "ex_cmds.h"
#अन्तर्भाव "ex_cmdidxs.h"

स्थिर char_u dollar_command[2] = {'$', 0};


#ifdef FEAT_EVAL
// Struct for storing a line inside a while/for loop
प्रकारव्याख्या संरचना
{
    char_u	*ओळ;		// command line
    linenr_T	lnum;		// sourcing_lnum of the line
} wcmd_T;

/*
 * Structure used to store info for line position in a while or for loop.
 * This is required, because do_one_cmd() may invoke ex_function(), which
 * reads more lines that may come from the while/for loop.
 */
संरचना loop_cookie
{
    garray_T	*ओळी_gap;		// growarray with line info
    पूर्णांक		current_ओळ;		// last read line from growarray
    पूर्णांक		repeating;		// TRUE when looping a second time
    // When "repeating" is FALSE use "getline" and "cookie" to get lines
    char_u	*(*lc_getline)(पूर्णांक, शून्य *, पूर्णांक, getline_opt_T);
    शून्य	*cookie;
};

स्थिर char_u	*get_loop_ओळ(पूर्णांक c, शून्य *cookie, पूर्णांक indent, getline_opt_T options);
स्थिर पूर्णांक	store_loop_ओळ(garray_T *gap, char_u *ओळ);
स्थिर शून्य	free_cmdlines(garray_T *gap);

// Struct to save a few things while debugging.  Used in do_cmdline() only.
संरचना dbg_stuff
{
    पूर्णांक		trylevel;
    पूर्णांक		force_abort;
    except_T	*caught_चळत;
    char_u	*vv_exception;
    char_u	*vv_throwpoint;
    पूर्णांक		did_emsg;
    पूर्णांक		got_int;
    पूर्णांक		did_throw;
    पूर्णांक		need_rethrow;
    पूर्णांक		check_cstack;
    except_T	*current_exception;
};

    स्थिर शून्य
save_dbg_stuff(संरचना dbg_stuff *dsp)
{
    dsp->trylevel	= trylevel;		trylevel = 0;
    dsp->force_abort	= force_abort;		force_abort = FALSE;
    dsp->caught_चळत	= caught_चळत;		caught_चळत = रिक्त;
    dsp->vv_exception	= v_exception(रिक्त);
    dsp->vv_throwpoint	= v_throwpoint(रिक्त);

    // Necessary for debugging an inactive ":catch", ":finally", ":endtry"
    dsp->did_emsg	= did_emsg;		did_emsg     = FALSE;
    dsp->got_int	= got_int;		got_int	     = FALSE;
    dsp->did_throw	= did_throw;		did_throw    = FALSE;
    dsp->need_rethrow	= need_rethrow;		need_rethrow = FALSE;
    dsp->check_cstack	= check_cstack;		check_cstack = FALSE;
    dsp->current_exception = current_exception;	current_exception = रिक्त;
}

    स्थिर शून्य
restore_dbg_stuff(संरचना dbg_stuff *dsp)
{
    suppress_errthrow = FALSE;
    trylevel = dsp->trylevel;
    force_abort = dsp->force_abort;
    caught_चळत = dsp->caught_चळत;
    (शून्य)v_exception(dsp->vv_exception);
    (शून्य)v_throwpoint(dsp->vv_throwpoint);
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
    स्थिर पूर्णांक
is_other_file(पूर्णांक fnum, char_u *ffname)
{
    जर (fnum != 0)
    {
	जर (fnum == curbuf->b_fnum)
	    परत FALSE;

	परत TRUE;
    }

    जर (ffname == रिक्त)
	परत TRUE;

    जर (*ffname == NUL)
	परत FALSE;

    // TODO: Need a reliable way to know whether a buffer is meant to live
    // on-disk !curbuf->b_dev_valid is not always available (example: missing
    // on Windows)
    जर (curbuf->b_sfname != रिक्त
	    && *curbuf->b_sfname != NUL)
	// This occurs with unsaved buffers. In which case `ffname` actually
	// corresponds to curbuf->b_sfname
	परत fnamecmp(ffname, curbuf->b_sfname) != 0;

    परत otherfile(ffname);
}

/*
 * do_exmode(): Repeatedly get commands for the "Ex" mode, until the ":vi"
 * command is given.
 */
    शून्य
do_exmode(
    पूर्णांक		improved)	    // TRUE for "improved Ex" mode
{
    पूर्णांक		save_संदेश_scroll;
    पूर्णांक		prev_संदेश_row;
    linenr_T	prev_ओळ;
    varnumber_T	changedtick;

    जर (improved)
	exmode_active = EXMODE_VIM;
    अन्यथा
	exmode_active = EXMODE_NORMAL;
    State = MODE_NORMAL;
    may_trigger_modechanged();

    // When using ":global /pat/ visual" and then "Q" we return to continue
    // the :global command.
    जर (global_busy)
	परत;

    save_संदेश_scroll = संदेश_scroll;
    ++RedrawingDisabled;	    // don't redisplay the window
    ++no_wait_return;		    // don't wait for return
#ifdef FEAT_GUI
    // Ignore scrollbar and mouse events in Ex mode
    ++hold_gui_events;
#endif

    संदेश(_("Entering Ex mode.  Type \"visual\" to go to Normal mode."));
    पर्यंत (exmode_active)
    {
	// Check for a ":normal" command and no more characters left.
	जर (ex_normal_busy > 0 && typebuf.tb_len == 0)
	{
	    exmode_active = FALSE;
	    थांबा;
	}
	संदेश_scroll = TRUE;
	need_wait_return = FALSE;
	ex_pressedreturn = FALSE;
	ex_no_reprint = FALSE;
	changedtick = CHANGEDTICK(curbuf);
	prev_संदेश_row = संदेश_row;
	prev_ओळ = curwin->w_cursor.lnum;
	जर (improved)
	{
	    cmdline_row = संदेश_row;
	    do_cmdline(रिक्त, getexline, रिक्त, 0);
	}
	अन्यथा
	    do_cmdline(रिक्त, getexmodeline, रिक्त, DOCMD_NOWAIT);
	ओळी_डावा = Rows - 1;

	जर ((prev_ओळ != curwin->w_cursor.lnum
		   || changedtick != CHANGEDTICK(curbuf)) && !ex_no_reprint)
	{
	    जर (curbuf->b_ml.ml_flags & ML_EMPTY)
		emsg(_(e_empty_buffer));
	    अन्यथा
	    {
		जर (ex_pressedreturn)
		{
		    // go up one line, to overwrite the ":<CR>" line, so the
		    // output doesn't contain empty lines.
		    संदेश_row = prev_संदेश_row;
		    जर (prev_संदेश_row == Rows - 1)
			संदेश_row--;
		}
		संदेश_col = 0;
		print_ओळ_no_prefix(curwin->w_cursor.lnum, FALSE, FALSE);
		संदेश_clr_eos();
	    }
	}
	अन्यथा जर (ex_pressedreturn && !ex_no_reprint)	// must be at EOF
	{
	    जर (curbuf->b_ml.ml_flags & ML_EMPTY)
		emsg(_(e_empty_buffer));
	    अन्यथा
		emsg(_(e_at_end_of_file));
	}
    }

#ifdef FEAT_GUI
    --hold_gui_events;
#endif
    जर (RedrawingDisabled > 0)
	--RedrawingDisabled;
    --no_wait_return;
    update_screen(UPD_CLEAR);
    need_wait_return = FALSE;
    संदेश_scroll = save_संदेश_scroll;
}

/*
 * Print the executed command for when 'verbose' is set.
 * When "lnum" is 0 only print the command.
 */
    स्थिर शून्य
संदेश_verbose_cmd(linenr_T lnum, char_u *cmd)
{
    ++no_wait_return;
    verbose_enter_scroll();

    जर (lnum == 0)
	smsg(_("Executing: %s"), cmd);
    अन्यथा
	smsg(_("line %ld: %s"), (दीर्घ)lnum, cmd);
    जर (संदेश_silent == 0)
	संदेश_puts("\n");   // don't overwrite this

    verbose_leave_scroll();
    --no_wait_return;
}

/*
 * Execute a simple command line.  Used for translated commands like "*".
 */
    पूर्णांक
do_cmdline_cmd(char_u *cmd)
{
    परत do_cmdline(cmd, रिक्त, रिक्त,
				   DOCMD_VERBOSE|DOCMD_NOWAIT|DOCMD_KEYTYPED);
}

/*
 * Execute the "+cmd" argument of "edit +cmd fname" and the like.
 * This allows for using a range without ":" in Vim9 script.
 */
    स्थिर पूर्णांक
do_cmd_argument(char_u *cmd)
{
    परत do_cmdline(cmd, रिक्त, रिक्त,
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
    पूर्णांक
do_cmdline(
    char_u	*cmdline,
    char_u	*(*fgetline)(पूर्णांक, शून्य *, पूर्णांक, getline_opt_T),
    शून्य	*cookie,		// argument for fgetline()
    पूर्णांक		flags)
{
    char_u	*next_cmdline;		// next cmd to execute
    char_u	*cmdline_copy = रिक्त;	// copy of cmd line
    पूर्णांक		used_getline = FALSE;	// used "fgetline" to obtain command
    स्थिर पूर्णांक	recursive = 0;		// recursive depth
    पूर्णांक		संदेश_didout_before_start = 0;
    पूर्णांक		मोजणी = 0;		// line number count
    पूर्णांक		did_inc_RedrawingDisabled = FALSE;
    पूर्णांक		retval = OK;
#ifdef FEAT_EVAL
    cstack_T	cstack;			// conditional stack
    garray_T	ओळी_ga;		// keep lines for ":while"/":for"
    पूर्णांक		current_ओळ = 0;	// active line in lines_ga
    पूर्णांक		current_ओळ_before = 0;
    char_u	*fname = रिक्त;		// function or script name
    linenr_T	*breakpoint = रिक्त;	// ptr to breakpoint field in cookie
    पूर्णांक		*dbg_tick = रिक्त;	// ptr to dbg_tick field in cookie
    संरचना dbg_stuff debug_saved;	// saved things for debug mode
    पूर्णांक		initial_trylevel;
    msglist_T	**saved_संदेश_list = रिक्त;
    msglist_T	*private_संदेश_list = रिक्त;

    // "fgetline" and "cookie" passed to do_one_cmd()
    char_u	*(*cmd_getline)(पूर्णांक, शून्य *, पूर्णांक, getline_opt_T);
    शून्य	*cmd_cookie;
    संरचना loop_cookie cmd_loop_cookie;
    शून्य	*real_cookie;
    पूर्णांक		getline_is_func;
#अन्यथा
# define cmd_getline fgetline
# define cmd_cookie cookie
#endif
    स्थिर पूर्णांक	call_depth = 0;		// recursiveness
#ifdef FEAT_EVAL
    // For every pair of do_cmdline()/do_one_cmd() calls, use an extra memory
    // location for storing error messages to be converted to an exception.
    // This ensures that the do_errthrow() call in do_one_cmd() does not
    // combine the messages stored by an earlier invocation of do_one_cmd()
    // with the command name of the later one.  This would happen when
    // BufWritePost autocommands are executed after a write error.
    saved_संदेश_list = संदेश_list;
    संदेश_list = &private_संदेश_list;
#endif

    // It's possible to create an endless loop with ":execute", catch that
    // here.  The value of 200 allows nested function calls, ":source", etc.
    // Allow 200 or 'maxfuncdepth', whatever is larger.
    जर (call_depth >= 200
#ifdef FEAT_EVAL
	    && call_depth >= p_mfd
#endif
	    )
    {
	emsg(_(e_command_too_recursive));
#ifdef FEAT_EVAL
	// When converting to an exception, we do not include the command name
	// since this is not an error of the specific command.
	do_errthrow((cstack_T *)रिक्त, (char_u *)रिक्त);
	संदेश_list = saved_संदेश_list;
#endif
	परत FAIL;
    }
    ++call_depth;

#ifdef FEAT_EVAL
    CLEAR_FIELD(cstack);
    cstack.cs_idx = -1;
    ga_init2(&ओळी_ga, आकार(wcmd_T), 10);

    real_cookie = getline_cookie(fgetline, cookie);

    // Inside a function use a higher nesting level.
    getline_is_func = getline_equal(fgetline, cookie, get_func_ओळ);
    जर (getline_is_func && ex_nesting_level == func_level(real_cookie))
	++ex_nesting_level;

    // Get the function or script name and the address where the next breakpoint
    // line and the debug tick for a function or script are stored.
    जर (getline_is_func)
    {
	fname = func_name(real_cookie);
	breakpoint = func_breakpoint(real_cookie);
	dbg_tick = func_dbg_tick(real_cookie);
    }
    अन्यथा जर (getline_equal(fgetline, cookie, getsourceline))
    {
	fname = SOURCING_NAME;
	breakpoint = source_breakpoint(real_cookie);
	dbg_tick = source_dbg_tick(real_cookie);
    }

    /*
     * Initialize "force_abort"  and "suppress_errthrow" at the top level.
     */
    जर (!recursive)
    {
	force_abort = FALSE;
	suppress_errthrow = FALSE;
    }

    /*
     * If requested, store and reset the global values controlling the
     * exception handling (used when debugging).  Otherwise clear it to avoid
     * a bogus compiler warning when the optimizer uses inline functions...
     */
    जर (flags & DOCMD_EXCRESET)
	save_dbg_stuff(&debug_saved);
    अन्यथा
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
    जर (!(flags & DOCMD_KEYTYPED)
			       && !getline_equal(fgetline, cookie, getexline))
	किल्ली_Typed = FALSE;

    /*
     * Continue executing command lines:
     * - when inside an ":if", ":while" or ":for"
     * - for multiple commands on one line, separated with '|'
     * - when repeating until there are no more lines (for ":source")
     */
    next_cmdline = cmdline;
    करा
    {
#ifdef FEAT_EVAL
	getline_is_func = getline_equal(fgetline, cookie, get_func_ओळ);
#endif

	// stop skipping cmds for an error msg after all endif/while/for
	जर (next_cmdline == रिक्त
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
	जर (cstack.cs_looplevel > 0 && current_ओळ < ओळी_ga.ga_len)
	{
	    // Each '|' separated command is stored separately in lines_ga, to
	    // be able to jump to it.  Don't use next_cmdline now.
	    VIM_CLEAR(cmdline_copy);

	    // Check if a function has returned or, unless it has an unclosed
	    // try conditional, aborted.
	    जर (getline_is_func)
	    {
# ifdef FEAT_PROFILE
		जर (do_profiling == PROF_YES)
		    func_ओळ_end(real_cookie);
# endif
		जर (func_has_ended(real_cookie))
		{
		    retval = FAIL;
		    थांबा;
		}
	    }
# ifdef FEAT_PROFILE
	    अन्यथा जर (do_profiling == PROF_YES
			    && getline_equal(fgetline, cookie, getsourceline))
		script_ओळ_end();
# endif

	    // Check if a sourced file hit a ":finish" command.
	    जर (source_finished(fgetline, cookie))
	    {
		retval = FAIL;
		थांबा;
	    }

	    // If breakpoints have been added/deleted need to check for it.
	    जर (breakpoint != रिक्त && dbg_tick != रिक्त
						   && *dbg_tick != debug_tick)
	    {
		*breakpoint = dbg_find_breakpoint(
				getline_equal(fgetline, cookie, getsourceline),
							fname, SOURCING_LNUM);
		*dbg_tick = debug_tick;
	    }

	    next_cmdline = ((wcmd_T *)(ओळी_ga.ga_data))[current_ओळ].ओळ;
	    SOURCING_LNUM = ((wcmd_T *)(ओळी_ga.ga_data))[current_ओळ].lnum;

	    // Did we encounter a breakpoint?
	    जर (breakpoint != रिक्त && *breakpoint != 0
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
	    जर (do_profiling == PROF_YES)
	    {
		जर (getline_is_func)
		    func_ओळ_start(real_cookie, SOURCING_LNUM);
		अन्यथा जर (getline_equal(fgetline, cookie, getsourceline))
		    script_ओळ_start();
	    }
# endif
	}
#endif

	// 2. If no line given, get an allocated line with fgetline().
	जर (next_cmdline == रिक्त)
	{
	    /*
	     * Need to set msg_didout for the first line after an ":if",
	     * otherwise the ":if" will be overwritten.
	     */
	    जर (मोजणी == 1 && getline_equal(fgetline, cookie, getexline))
		संदेश_didout = TRUE;
	    जर (fgetline == रिक्त || (next_cmdline = fgetline(':', cookie,
#ifdef FEAT_EVAL
		    cstack.cs_idx < 0 ? 0 : (cstack.cs_idx + 1) * 2
#अन्यथा
		    0
#endif
		    , in_vim9script() ? GETLINE_CONCAT_CONTBAR
					       : GETLINE_CONCAT_CONT)) == रिक्त)
	    {
		// Don't call wait_return() for aborted command line.  The NULL
		// returned for the end of a sourced file or executed function
		// doesn't do this.
		जर (किल्ली_Typed && !(flags & DOCMD_REPEAT))
		    need_wait_return = FALSE;
		retval = FAIL;
		थांबा;
	    }
	    used_getline = TRUE;

	    /*
	     * Keep the first typed line.  Clear it when more lines are typed.
	     */
	    जर (flags & DOCMD_KEEPLINE)
	    {
		vim_free(repeat_cmdline);
		जर (मोजणी == 0)
		    repeat_cmdline = vim_strsave(next_cmdline);
		अन्यथा
		    repeat_cmdline = रिक्त;
	    }
	}

	// 3. Make a copy of the command so we can mess with it.
	अन्यथा जर (cmdline_copy == रिक्त)
	{
	    next_cmdline = vim_strsave(next_cmdline);
	    जर (next_cmdline == रिक्त)
	    {
		emsg(_(e_out_of_memory));
		retval = FAIL;
		थांबा;
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
	जर ((cstack.cs_looplevel > 0 || has_loop_cmd(next_cmdline)))
	{
	    cmd_getline = get_loop_ओळ;
	    cmd_cookie = (शून्य *)&cmd_loop_cookie;
	    cmd_loop_cookie.ओळी_gap = &ओळी_ga;
	    cmd_loop_cookie.current_ओळ = current_ओळ;
	    cmd_loop_cookie.lc_getline = fgetline;
	    cmd_loop_cookie.cookie = cookie;
	    cmd_loop_cookie.repeating = (current_ओळ < ओळी_ga.ga_len);

	    // Save the current line when encountering it the first time.
	    जर (current_ओळ == ओळी_ga.ga_len
		    && store_loop_ओळ(&ओळी_ga, next_cmdline) == FAIL)
	    {
		retval = FAIL;
		थांबा;
	    }
	    current_ओळ_before = current_ओळ;
	}
	अन्यथा
	{
	    cmd_getline = fgetline;
	    cmd_cookie = cookie;
	}

	did_endif = FALSE;
#endif

	जर (मोजणी++ == 0)
	{
	    /*
	     * All output from the commands is put below each other, without
	     * waiting for a return. Don't do this when executing commands
	     * from a script or when being called recursive (e.g. for ":e
	     * +command file").
	     */
	    जर (!(flags & DOCMD_NOWAIT) && !recursive)
	    {
		संदेश_didout_before_start = संदेश_didout;
		संदेश_didany = FALSE; // no output yet
		संदेश_start();
		संदेश_scroll = TRUE;  // put messages below each other
		++no_wait_return;   // don't wait for return until finished
		++RedrawingDisabled;
		did_inc_RedrawingDisabled = TRUE;
	    }
	}

	जर ((p_verbose >= 15 && SOURCING_NAME != रिक्त) || p_verbose >= 16)
	    संदेश_verbose_cmd(SOURCING_LNUM, cmdline_copy);

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
	जर (cmd_cookie == (शून्य *)&cmd_loop_cookie)
	    // Use "current_line" from "cmd_loop_cookie", it may have been
	    // incremented when defining a function.
	    current_ओळ = cmd_loop_cookie.current_ओळ;
#endif

	जर (next_cmdline == रिक्त)
	{
	    VIM_CLEAR(cmdline_copy);

	    /*
	     * If the command was typed, remember it for the ':' register.
	     * Do this AFTER executing the command to make :@: work.
	     */
	    जर (getline_equal(fgetline, cookie, getexline)
						  && new_last_cmdline != रिक्त)
	    {
		vim_free(last_cmdline);
		last_cmdline = new_last_cmdline;
		new_last_cmdline = रिक्त;
	    }
	}
	अन्यथा
	{
	    // need to copy the command after the '|' to cmdline_copy, for the
	    // next do_one_cmd()
	    STRMOVE(cmdline_copy, next_cmdline);
	    next_cmdline = cmdline_copy;
	}


#ifdef FEAT_EVAL
	// reset did_emsg for a function that is not aborted by an error
	जर (did_emsg && !force_abort
		&& getline_equal(fgetline, cookie, get_func_ओळ)
					      && !func_has_abort(real_cookie))
	{
	    // did_emsg_cumul is not set here
	    did_emsg = FALSE;
	}

	जर (cstack.cs_looplevel > 0)
	{
	    ++current_ओळ;

	    /*
	     * An ":endwhile", ":endfor" and ":continue" is handled here.
	     * If we were executing commands, jump back to the ":while" or
	     * ":for".
	     * If we were not executing commands, decrement cs_looplevel.
	     */
	    जर (cstack.cs_lflags & (CSL_HAD_CONT | CSL_HAD_ENDLOOP))
	    {
		cstack.cs_lflags &= ~(CSL_HAD_CONT | CSL_HAD_ENDLOOP);

		// Jump back to the matching ":while" or ":for".  Be careful
		// not to use a cs_line[] from an entry that isn't a ":while"
		// or ":for": It would make "current_line" invalid and can
		// cause a crash.
		जर (!did_emsg && !got_int && !did_throw
			&& cstack.cs_idx >= 0
			&& (cstack.cs_flags[cstack.cs_idx]
						      & (CSF_WHILE | CSF_FOR))
			&& cstack.cs_ओळ[cstack.cs_idx] >= 0
			&& (cstack.cs_flags[cstack.cs_idx] & CSF_ACTIVE))
		{
		    current_ओळ = cstack.cs_ओळ[cstack.cs_idx];
						// remember we jumped there
		    cstack.cs_lflags |= CSL_HAD_LOOP;
		    ओळ_breakcheck();		// check if CTRL-C typed

		    // Check for the next breakpoint at or after the ":while"
		    // or ":for".
		    जर (breakpoint != रिक्त && ओळी_ga.ga_len > current_ओळ)
		    {
			*breakpoint = dbg_find_breakpoint(
			       getline_equal(fgetline, cookie, getsourceline),
									fname,
			   ((wcmd_T *)ओळी_ga.ga_data)[current_ओळ].lnum-1);
			*dbg_tick = debug_tick;
		    }
		}
		अन्यथा
		{
		    // can only get here with ":endwhile" or ":endfor"
		    जर (cstack.cs_idx >= 0)
			rewind_conditionals(&cstack, cstack.cs_idx - 1,
				   CSF_WHILE | CSF_FOR, &cstack.cs_looplevel);
		}
	    }

	    /*
	     * For a ":while" or ":for" we need to remember the line number.
	     */
	    अन्यथा जर (cstack.cs_lflags & CSL_HAD_LOOP)
	    {
		cstack.cs_lflags &= ~CSL_HAD_LOOP;
		cstack.cs_ओळ[cstack.cs_idx] = current_ओळ_before;
	    }
	}

	// Check for the next breakpoint after a watchexpression
	जर (breakpoint != रिक्त && has_watchexpr())
	{
	    *breakpoint = dbg_find_breakpoint(FALSE, fname, SOURCING_LNUM);
	    *dbg_tick = debug_tick;
	}

	/*
	 * When not inside any ":while" loop, clear remembered lines.
	 */
	जर (cstack.cs_looplevel == 0)
	{
	    जर (ओळी_ga.ga_len > 0)
	    {
		SOURCING_LNUM =
		       ((wcmd_T *)ओळी_ga.ga_data)[ओळी_ga.ga_len - 1].lnum;
		free_cmdlines(&ओळी_ga);
	    }
	    current_ओळ = 0;
	}

	/*
	 * A ":finally" makes did_emsg, got_int, and did_throw pending for
	 * being restored at the ":endtry".  Reset them here and set the
	 * ACTIVE and FINALLY flags, so that the finally clause gets executed.
	 * This includes the case where a missing ":endif", ":endwhile" or
	 * ":endfor" was detected by the ":finally" itself.
	 */
	जर (cstack.cs_lflags & CSL_HAD_FINA)
	{
	    cstack.cs_lflags &= ~CSL_HAD_FINA;
	    report_make_pending(cstack.cs_pending[cstack.cs_idx]
		    & (CSTP_ERROR | CSTP_INTERRUPT | CSTP_THROW),
		    did_throw ? (शून्य *)current_exception : रिक्त);
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
	जर (trylevel == 0 && !did_emsg && !got_int && !did_throw)
	    force_abort = FALSE;

	// Convert an interrupt to an exception if appropriate.
	(शून्य)do_intthrow(&cstack);
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
    पर्यंत (!((got_int
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
	    && (next_cmdline != रिक्त
#ifdef FEAT_EVAL
			|| cstack.cs_idx >= 0
#endif
			|| (flags & DOCMD_REPEAT)));

    vim_free(cmdline_copy);
    did_emsg_syntax = FALSE;
#ifdef FEAT_EVAL
    free_cmdlines(&ओळी_ga);
    ga_clear(&ओळी_ga);

    जर (cstack.cs_idx >= 0)
    {
	/*
	 * If a sourced file or executed function ran to its end, report the
	 * unclosed conditional.
	 * In Vim9 script do not give a second error, executing aborts after
	 * the first one.
	 */
	जर (!got_int && !did_throw && !aborting()
		&& !(did_emsg && in_vim9script())
		&& ((getline_equal(fgetline, cookie, getsourceline)
			&& !source_finished(fgetline, cookie))
		    || (getline_equal(fgetline, cookie, get_func_ओळ)
					    && !func_has_ended(real_cookie))))
	{
	    जर (cstack.cs_flags[cstack.cs_idx] & CSF_TRY)
		emsg(_(e_missing_endtry));
	    अन्यथा जर (cstack.cs_flags[cstack.cs_idx] & CSF_WHILE)
		emsg(_(e_missing_endwhile));
	    अन्यथा जर (cstack.cs_flags[cstack.cs_idx] & CSF_FOR)
		emsg(_(e_missing_endfor));
	    अन्यथा
		emsg(_(e_missing_endif));
	}

	/*
	 * Reset "trylevel" in case of a ":finish" or ":return" or a missing
	 * ":endtry" in a sourced file or executed function.  If the try
	 * conditional is in its finally clause, ignore anything pending.
	 * If it is in a catch clause, finish the caught exception.
	 * Also cleanup any "cs_forinfo" structures.
	 */
	करा
	{
	    पूर्णांक idx = cleanup_conditionals(&cstack, 0, TRUE);

	    जर (idx >= 0)
		--idx;	    // remove try block not in its finally clause
	    rewind_conditionals(&cstack, idx, CSF_WHILE | CSF_FOR,
							&cstack.cs_looplevel);
	}
	पर्यंत (cstack.cs_idx >= 0);
	trylevel = initial_trylevel;
    }

    // If a missing ":endtry", ":endwhile", ":endfor", or ":endif" or a memory
    // lack was reported above and the error message is to be converted to an
    // exception, do this now after rewinding the cstack.
    do_errthrow(&cstack, getline_equal(fgetline, cookie, get_func_ओळ)
				  ? (char_u *)"endfunction" : (char_u *)रिक्त);

    जर (trylevel == 0)
    {
	// Just in case did_throw got set but current_exception wasn't.
	जर (current_exception == रिक्त)
	    did_throw = FALSE;

	/*
	 * When an exception is being thrown out of the outermost try
	 * conditional, discard the uncaught exception, disable the conversion
	 * of interrupts or errors to exceptions, and ensure that no more
	 * commands are executed.
	 */
	जर (did_throw)
	    handle_did_throw();

	/*
	 * On an interrupt or an aborting error not converted to an exception,
	 * disable the conversion of errors to exceptions.  (Interrupts are not
	 * converted anymore, here.) This enables also the interrupt message
	 * when force_abort is set and did_emsg unset in case of an interrupt
	 * from a finally clause after an error.
	 */
	अन्यथा जर (got_int || (did_emsg && force_abort))
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
    जर (did_throw)
	need_rethrow = TRUE;
    जर ((getline_equal(fgetline, cookie, getsourceline)
		&& ex_nesting_level > source_level(real_cookie))
	    || (getline_equal(fgetline, cookie, get_func_ओळ)
		&& ex_nesting_level > func_level(real_cookie) + 1))
    {
	जर (!did_throw)
	    check_cstack = TRUE;
    }
    अन्यथा
    {
	// When leaving a function, reduce nesting level.
	जर (getline_equal(fgetline, cookie, get_func_ओळ))
	    --ex_nesting_level;
	/*
	 * Go to debug mode when returning from a function in which we are
	 * single-stepping.
	 */
	जर ((getline_equal(fgetline, cookie, getsourceline)
		    || getline_equal(fgetline, cookie, get_func_ओळ))
		&& ex_nesting_level + 1 <= debug_break_level)
	    do_debug(getline_equal(fgetline, cookie, getsourceline)
		    ? (char_u *)_("End of sourced file")
		    : (char_u *)_("End of function"));
    }

    /*
     * Restore the exception environment (done after returning from the
     * debugger).
     */
    जर (flags & DOCMD_EXCRESET)
	restore_dbg_stuff(&debug_saved);

    संदेश_list = saved_संदेश_list;

    // Cleanup if "cs_emsg_silent_list" remains.
    जर (cstack.cs_emsg_silent_list != रिक्त)
    {
	eslist_T *elem, *temp;

	साठी (elem = cstack.cs_emsg_silent_list; elem != रिक्त; elem = temp)
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
    जर (did_inc_RedrawingDisabled)
    {
	जर (RedrawingDisabled > 0)
	    --RedrawingDisabled;
	--no_wait_return;
	संदेश_scroll = FALSE;

	/*
	 * When just finished an ":if"-":else" which was typed, no need to
	 * wait for hit-return.  Also for an error situation.
	 */
	जर (retval == FAIL
#ifdef FEAT_EVAL
		|| (did_endif && किल्ली_Typed && !did_emsg)
#endif
					    )
	{
	    need_wait_return = FALSE;
	    संदेश_didany = FALSE;		// don't wait when restarting edit
	}
	अन्यथा जर (need_wait_return)
	{
	    /*
	     * The msg_start() above clears msg_didout. The wait_return() we do
	     * here should not overwrite the command that may be shown before
	     * doing that.
	     */
	    संदेश_didout |= संदेश_didout_before_start;
	    wait_return(FALSE);
	}
    }

#ifdef FEAT_EVAL
    did_endif = FALSE;  // in case do_cmdline used recursively
#अन्यथा
    /*
     * Reset if_level, in case a sourced script file contains more ":if" than
     * ":endif" (could be ":if x | foo | endif").
     */
    if_level = 0;
#endif

    --call_depth;
    परत retval;
}

#जर defined(FEAT_EVAL)
/*
 * Handle when "did_throw" is set after executing commands.
 */
    शून्य
handle_did_throw(शून्य)
{
    अक्षर	*p = रिक्त;
    msglist_T	*messages = रिक्त;
    ESTACK_CHECK_DECLARATION;

    /*
     * If the uncaught exception is a user exception, report it as an
     * error.  If it is an error exception, display the saved error
     * message now.  For an interrupt exception, do nothing; the
     * interrupt message is given elsewhere.
     */
    निवड (current_exception->type)
    {
	स्थिती ET_USER:
	    vim_snprintf((अक्षर *)IObuff, IOSIZE,
		    _(e_exception_not_caught_str),
		    current_exception->मूल्य);
	    p = (अक्षर *)vim_strsave(IObuff);
	    थांबा;
	स्थिती ET_ERROR:
	    messages = current_exception->messages;
	    current_exception->messages = रिक्त;
	    थांबा;
	स्थिती ET_INTERRUPT:
	    थांबा;
    }

    estack_ढकला(ETYPE_EXCEPT, current_exception->throw_name,
					current_exception->throw_lnum);
    ESTACK_CHECK_SETUP;
    current_exception->throw_name = रिक्त;

    discard_current_exception();	// uses IObuff if 'verbose'

    // If "silent!" is active the uncaught exception is not fatal.
    जर (emsg_silent == 0)
    {
	suppress_errthrow = TRUE;
	force_abort = TRUE;
    }

    जर (messages != रिक्त)
    {
	करा
	{
	    msglist_T	*next = messages->next;
	    पूर्णांक		save_compiling = estack_compiling;

	    estack_compiling = messages->संदेश_compiling;
	    emsg(messages->संदेश);
	    vim_free(messages->संदेश);
	    vim_free(messages->sfile);
	    vim_free(messages);
	    messages = next;
	    estack_compiling = save_compiling;
	}
	पर्यंत (messages != रिक्त);
    }
    अन्यथा जर (p != रिक्त)
    {
	emsg(p);
	vim_free(p);
    }
    vim_free(SOURCING_NAME);
    ESTACK_CHECK_NOW;
    estack_काढा();
}

/*
 * Obtain a line when inside a ":while" or ":for" loop.
 */
    स्थिर char_u *
get_loop_ओळ(पूर्णांक c, शून्य *cookie, पूर्णांक indent, getline_opt_T options)
{
    संरचना loop_cookie	*cp = (संरचना loop_cookie *)cookie;
    wcmd_T		*wp;
    char_u		*ओळ;

    जर (cp->current_ओळ + 1 >= cp->ओळी_gap->ga_len)
    {
	जर (cp->repeating)
	    परत रिक्त;	// trying to read past ":endwhile"/":endfor"

	// First time inside the ":while"/":for": get line normally.
	जर (cp->lc_getline == रिक्त)
	    ओळ = getcmdline(c, 0L, indent, 0);
	अन्यथा
	    ओळ = cp->lc_getline(c, cp->cookie, indent, options);
	जर (ओळ != रिक्त && store_loop_ओळ(cp->ओळी_gap, ओळ) == OK)
	    ++cp->current_ओळ;

	परत ओळ;
    }

    किल्ली_Typed = FALSE;
    ++cp->current_ओळ;
    wp = (wcmd_T *)(cp->ओळी_gap->ga_data) + cp->current_ओळ;
    SOURCING_LNUM = wp->lnum;
    परत vim_strsave(wp->ओळ);
}

/*
 * Store a line in "gap" so that a ":while" loop can execute it again.
 */
    स्थिर पूर्णांक
store_loop_ओळ(garray_T *gap, char_u *ओळ)
{
    जर (ga_grow(gap, 1) == FAIL)
	परत FAIL;
    ((wcmd_T *)(gap->ga_data))[gap->ga_len].ओळ = vim_strsave(ओळ);
    ((wcmd_T *)(gap->ga_data))[gap->ga_len].lnum = SOURCING_LNUM;
    ++gap->ga_len;
    परत OK;
}

/*
 * Free the lines stored for a ":while" or ":for" loop.
 */
    स्थिर शून्य
free_cmdlines(garray_T *gap)
{
    पर्यंत (gap->ga_len > 0)
    {
	vim_free(((wcmd_T *)(gap->ga_data))[gap->ga_len - 1].ओळ);
	--gap->ga_len;
    }
}
#endif

/*
 * If "fgetline" is get_loop_line(), return TRUE if the getline it uses equals
 * "func".  * Otherwise return TRUE when "fgetline" equals "func".
 */
    पूर्णांक
getline_equal(
    char_u	*(*fgetline)(पूर्णांक, शून्य *, पूर्णांक, getline_opt_T),
    शून्य	*cookie UNUSED,		// argument for fgetline()
    char_u	*(*func)(पूर्णांक, शून्य *, पूर्णांक, getline_opt_T))
{
#ifdef FEAT_EVAL
    char_u		*(*gp)(पूर्णांक, शून्य *, पूर्णांक, getline_opt_T);
    संरचना loop_cookie *cp;

    // When "fgetline" is "get_loop_line()" use the "cookie" to find the
    // function that's originally used to obtain the lines.  This may be
    // nested several levels.
    gp = fgetline;
    cp = (संरचना loop_cookie *)cookie;
    पर्यंत (gp == get_loop_ओळ)
    {
	gp = cp->lc_getline;
	cp = cp->cookie;
    }
    परत gp == func;
#अन्यथा
    परत fgetline == func;
#endif
}

/*
 * If "fgetline" is get_loop_line(), return the cookie used by the original
 * getline function.  Otherwise return "cookie".
 */
    शून्य *
getline_cookie(
    char_u	*(*fgetline)(पूर्णांक, शून्य *, पूर्णांक, getline_opt_T) UNUSED,
    शून्य	*cookie)		// argument for fgetline()
{
#ifdef FEAT_EVAL
    char_u		*(*gp)(पूर्णांक, शून्य *, पूर्णांक, getline_opt_T);
    संरचना loop_cookie  *cp;

    // When "fgetline" is "get_loop_line()" use the "cookie" to find the
    // cookie that's originally used to obtain the lines.  This may be nested
    // several levels.
    gp = fgetline;
    cp = (संरचना loop_cookie *)cookie;
    पर्यंत (gp == get_loop_ओळ)
    {
	gp = cp->lc_getline;
	cp = cp->cookie;
    }
    परत cp;
#अन्यथा
    परत cookie;
#endif
}

#जर defined(FEAT_EVAL)
/*
 * Get the next line source line without advancing.
 */
    char_u *
getline_peek(
    char_u	*(*fgetline)(पूर्णांक, शून्य *, पूर्णांक, getline_opt_T) UNUSED,
    शून्य	*cookie)		// argument for fgetline()
{
    char_u		*(*gp)(पूर्णांक, शून्य *, पूर्णांक, getline_opt_T);
    संरचना loop_cookie  *cp;
    wcmd_T		*wp;

    // When "fgetline" is "get_loop_line()" use the "cookie" to find the
    // cookie that's originally used to obtain the lines.  This may be nested
    // several levels.
    gp = fgetline;
    cp = (संरचना loop_cookie *)cookie;
    पर्यंत (gp == get_loop_ओळ)
    {
	जर (cp->current_ओळ + 1 < cp->ओळी_gap->ga_len)
	{
	    // executing lines a second time, use the stored copy
	    wp = (wcmd_T *)(cp->ओळी_gap->ga_data) + cp->current_ओळ + 1;
	    परत wp->ओळ;
	}
	gp = cp->lc_getline;
	cp = cp->cookie;
    }
    जर (gp == getsourceline)
	परत source_nextline(cp);
    परत रिक्त;
}
#endif


/*
 * Helper function to apply an offset for buffer commands, i.e. ":bdelete",
 * ":bwipeout", etc.
 * Returns the buffer number.
 */
    स्थिर पूर्णांक
compute_buffer_local_मोजणी(पूर्णांक addr_type, पूर्णांक lnum, पूर्णांक offset)
{
    buf_T   *buf;
    buf_T   *nextbuf;
    पूर्णांक     मोजणी = offset;

    buf = firstbuf;
    पर्यंत (buf->b_next != रिक्त && buf->b_fnum < lnum)
	buf = buf->b_next;
    पर्यंत (मोजणी != 0)
    {
	मोजणी += (offset < 0) ? 1 : -1;
	nextbuf = (offset < 0) ? buf->b_prev : buf->b_next;
	जर (nextbuf == रिक्त)
	    थांबा;
	buf = nextbuf;
	जर (addr_type == ADDR_LOADED_BUFFERS)
	    // skip over unloaded buffers
	    पर्यंत (buf->b_ml.ml_mfp == रिक्त)
	    {
		nextbuf = (offset < 0) ? buf->b_prev : buf->b_next;
		जर (nextbuf == रिक्त)
		    थांबा;
		buf = nextbuf;
	    }
    }
    // we might have gone too far, last buffer is not loadedd
    जर (addr_type == ADDR_LOADED_BUFFERS)
	पर्यंत (buf->b_ml.ml_mfp == रिक्त)
	{
	    nextbuf = (offset >= 0) ? buf->b_prev : buf->b_next;
	    जर (nextbuf == रिक्त)
		थांबा;
	    buf = nextbuf;
	}
    परत buf->b_fnum;
}

/*
 * Return the window number of "win".
 * When "win" is NULL return the number of windows.
 */
    स्थिर पूर्णांक
current_win_nr(win_T *win)
{
    win_T	*wp;
    पूर्णांक		nr = 0;

    FOR_ALL_WINDOWS(wp)
    {
	++nr;
	जर (wp == win)
	    थांबा;
    }
    परत nr;
}

    स्थिर पूर्णांक
current_tab_nr(tabpage_T *tab)
{
    tabpage_T	*tp;
    पूर्णांक		nr = 0;

    FOR_ALL_TABPAGES(tp)
    {
	++nr;
	जर (tp == tab)
	    थांबा;
    }
    परत nr;
}

    स्थिर पूर्णांक
comment_start(char_u *p, पूर्णांक starts_with_colon UNUSED)
{
    जर (in_vim9script())
	परत p[0] == '#' && !starts_with_colon;
    परत *p == '"';
}

#define CURRENT_WIN_NR current_win_nr(curwin)
#define LAST_WIN_NR current_win_nr(रिक्त)
#define CURRENT_TAB_NR current_tab_nr(curtab)
#define LAST_TAB_NR current_tab_nr(रिक्त)

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
    स्थिर char_u *
do_one_cmd(
    char_u	**cmdlinep,
    पूर्णांक		flags,
#ifdef FEAT_EVAL
    cstack_T	*cstack,
#endif
    char_u	*(*fgetline)(पूर्णांक, शून्य *, पूर्णांक, getline_opt_T),
    शून्य	*cookie)		// argument for fgetline()
{
    char_u	*p;
    linenr_T	lnum;
    दीर्घ	n;
    अक्षर	*errormsg = रिक्त;	// error message
    char_u	*after_modifier = रिक्त;
    exarg_T	ea;			// Ex command arguments
    cmdmod_T	save_cmdmod;
    पूर्णांक		save_reg_executing = reg_executing;
    पूर्णांक		save_pending_end_reg_executing = pending_end_reg_executing;
    पूर्णांक		ni;			// set when Not Implemented
    char_u	*cmd;
    पूर्णांक		starts_with_colon = FALSE;
    पूर्णांक		may_have_range;
#ifdef FEAT_EVAL
    पूर्णांक		did_set_अभिव्यक्ती_ओळ = FALSE;
#endif
    पूर्णांक		sourcing = flags & DOCMD_VERBOSE;
    पूर्णांक		did_append_cmd = FALSE;

    CLEAR_FIELD(ea);
    ea.line1 = 1;
    ea.line2 = 1;
#ifdef FEAT_EVAL
    ++ex_nesting_level;
#endif

    // When the last file has not been edited :q has to be typed twice.
    जर (quitmore
#ifdef FEAT_EVAL
	    // avoid that a function call in 'statusline' does this
	    && !getline_equal(fgetline, cookie, get_func_ओळ)
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
    जर ((*cmdlinep)[0] == '#' && (*cmdlinep)[1] == '!')
	जा doend;

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
    जर (parse_command_modifiers(&ea, &errormsg, &cmdmod, FALSE) == FAIL)
	जा doend;
    apply_cmdmod(&cmdmod);
    after_modifier = ea.cmd;

#ifdef FEAT_EVAL
    ea.skip = did_emsg || got_int || did_throw || (cstack->cs_idx >= 0
			 && !(cstack->cs_flags[cstack->cs_idx] & CSF_ACTIVE));
#अन्यथा
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
    पूर्णांक vim9script = in_vim9script();
    जर (vim9script && (flags & DOCMD_RANGEOK) == 0)
    {
	may_have_range = FALSE;
	साठी (p = ea.cmd; p >= *cmdlinep; --p)
	{
	    जर (*p == ':')
		may_have_range = TRUE;
	    जर (p < ea.cmd && !VIM_ISWHITE(*p))
		थांबा;
	}
    }
    अन्यथा
	may_have_range = TRUE;
    जर (may_have_range)
	ea.cmd = skip_range(ea.cmd, TRUE, रिक्त);

#ifdef FEAT_EVAL
    // Handle ":export" - it functions almost like a command modifier.
    // ":export var Name: type"
    // ":export def Name(..."
    // etc.
    जर (vim9script && checkforcmd_noparen(&ea.cmd, "export", 6))
	is_export = TRUE;
#endif

    जर (vim9script && !may_have_range)
    {
	जर (ea.cmd == cmd + 1 && *cmd == '$')
	    // should be "$VAR = val"
	    --ea.cmd;
#ifdef FEAT_EVAL
	p = find_ex_command(&ea, रिक्त, lookup_scriptitem, रिक्त);
#अन्यथा
	p = find_ex_command(&ea, रिक्त, रिक्त, रिक्त);
#endif
	जर (ea.cmdidx == CMD_SIZE)
	{
	    char_u *ar = skip_range(ea.cmd, TRUE, रिक्त);

	    // If a ':' before the range is missing, give a clearer error
	    // message.
	    जर (ar > ea.cmd && !ea.skip)
	    {
		semsg(_(e_colon_required_before_range_str), ea.cmd);
		जा doend;
	    }
	}
    }
    अन्यथा
	p = find_ex_command(&ea, रिक्त, रिक्त, रिक्त);

#ifdef FEAT_EVAL
# ifdef FEAT_PROFILE
    // Count this line for profiling if skip is TRUE.
    जर (do_profiling == PROF_YES
	    && (!ea.skip || cstack->cs_idx == 0 || (cstack->cs_idx > 0
		     && (cstack->cs_flags[cstack->cs_idx - 1] & CSF_ACTIVE))))
    {
	पूर्णांक skip = did_emsg || got_int || did_throw;

	जर (ea.cmdidx == CMD_catch)
	    skip = !skip && !(cstack->cs_idx >= 0
			  && (cstack->cs_flags[cstack->cs_idx] & CSF_THROWN)
			  && !(cstack->cs_flags[cstack->cs_idx] & CSF_CAUGHT));
	अन्यथा जर (ea.cmdidx == CMD_else || ea.cmdidx == CMD_elseif)
	    skip = skip || !(cstack->cs_idx >= 0
			  && !(cstack->cs_flags[cstack->cs_idx]
						  & (CSF_ACTIVE | CSF_TRUE)));
	अन्यथा जर (ea.cmdidx == CMD_finally)
	    skip = FALSE;
	अन्यथा जर (ea.cmdidx != CMD_endif
		&& ea.cmdidx != CMD_endfor
		&& ea.cmdidx != CMD_endtry
		&& ea.cmdidx != CMD_endwhile)
	    skip = ea.skip;

	जर (!skip)
	{
	    जर (getline_equal(fgetline, cookie, get_func_ओळ))
		func_ओळ_exec(getline_cookie(fgetline, cookie));
	    अन्यथा जर (getline_equal(fgetline, cookie, getsourceline))
		script_ओळ_exec();
	}
    }
# endif
#endif

    ea.cmd = cmd;

#ifdef FEAT_EVAL
    // May go to debug mode.  If this happens and the ">quit" debug command is
    // used, throw an interrupt exception and skip the next command.
    dbg_check_breakpoint(&ea);
    जर (!ea.skip && got_int)
    {
	ea.skip = TRUE;
	(शून्य)do_intthrow(cstack);
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
    जर (!IS_USER_CMDIDX(ea.cmdidx))
    {
	जर (ea.cmdidx != CMD_SIZE)
	    ea.addr_type = cmdnames[(पूर्णांक)ea.cmdidx].cmd_addr_type;
	अन्यथा
	    ea.addr_type = ADDR_ओळी;

	// :wincmd range depends on the argument.
	जर (ea.cmdidx == CMD_wincmd && p != रिक्त)
	    get_wincmd_addr_type(skipwhite(p), &ea);
#ifdef FEAT_QUICKFIX
	// :.cc in quickfix window uses line number
	जर ((ea.cmdidx == CMD_cc || ea.cmdidx == CMD_ll) && bt_quickfix(curbuf))
	    ea.addr_type = ADDR_OTHER;
#endif
    }

    जर (!may_have_range)
	ea.line1 = ea.line2 = default_address(&ea);
    अन्यथा जर (parse_cmd_address(&ea, &errormsg, FALSE) == FAIL)
	जा doend;

/*
 * 5. Parse the command.
 */

    /*
     * Skip ':' and any white space
     */
    ea.cmd = skipwhite(ea.cmd);
    पर्यंत (*ea.cmd == ':')
	ea.cmd = skipwhite(ea.cmd + 1);

    /*
     * If we got a line, but no command, then go to the line.
     * If we find a '|' or '\n' we set ea.nextcmd.
     */
    जर (*ea.cmd == NUL || comment_start(ea.cmd, starts_with_colon)
			       || (ea.nextcmd = check_nextcmd(ea.cmd)) != रिक्त)
    {
	/*
	 * strange vi behaviour:
	 * ":3"		jumps to line 3
	 * ":3|..."	prints line 3  (not in Vim9 script)
	 * ":|"		prints current line  (not in Vim9 script)
	 */
	जर (ea.skip)	    // skip this if inside :if
	    जा doend;
	errormsg = ex_range_without_command(&ea);
	जा doend;
    }

    // If this looks like an undefined user command and there are CmdUndefined
    // autocommands defined, trigger the matching autocommands.
    जर (p != रिक्त && ea.cmdidx == CMD_SIZE && !ea.skip
	    && ASCII_ISUPPER(*ea.cmd)
	    && has_cmdundefined())
    {
	पूर्णांक ret;

	p = ea.cmd;
	पर्यंत (ASCII_ISALNUM(*p))
	    ++p;
	p = vim_strnsave(ea.cmd, p - ea.cmd);
	ret = apply_autocmds(EVENT_CMDUNDEFINED, p, p, TRUE, रिक्त);
	vim_free(p);
	// If the autocommands did something and didn't cause an error, try
	// finding the command again.
	p = (ret
#ifdef FEAT_EVAL
		&& !aborting()
#endif
		) ? find_ex_command(&ea, रिक्त, रिक्त, रिक्त) : ea.cmd;
    }

    जर (p == रिक्त)
    {
	जर (!ea.skip)
	    errormsg = _(e_ambiguous_use_of_user_defined_command);
	जा doend;
    }
    // Check for wrong commands.
    जर (*p == '!' && ea.cmd[1] == 0151 && ea.cmd[0] == 78
	    && !IS_USER_CMDIDX(ea.cmdidx))
    {
	errormsg = uc_fun_cmd();
	जा doend;
    }

    जर (ea.cmdidx == CMD_SIZE)
    {
	जर (!ea.skip)
	{
	    STRCPY(IObuff, _(e_not_an_editor_command));
	    जर (!sourcing)
	    {
		// If the modifier was parsed OK the error must be in the
		// following command
		जर (after_modifier != रिक्त)
		    append_command(after_modifier);
		अन्यथा
		    append_command(*cmdlinep);
		did_append_cmd = TRUE;
	    }
	    errormsg = (अक्षर *)IObuff;
	    did_emsg_syntax = TRUE;
	}
	जा doend;
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
    जर (ea.cmdidx == CMD_if)
	++if_level;
    जर (if_level)
    {
	जर (ea.cmdidx == CMD_endif)
	    --if_level;
	जा doend;
    }

#endif

    // forced commands
    जर (*p == '!' && ea.cmdidx != CMD_substitute
	    && ea.cmdidx != CMD_smagic && ea.cmdidx != CMD_snomagic)
    {
	++p;
	ea.forceit = TRUE;
    }
    अन्यथा
	ea.forceit = FALSE;

/*
 * 6. Parse arguments.  Then check for errors.
 */
    जर (!IS_USER_CMDIDX(ea.cmdidx))
	ea.argt = (दीर्घ)cmdnames[(पूर्णांक)ea.cmdidx].cmd_argt;

    जर (!ea.skip)
    {
#ifdef HAVE_SANDBOX
	जर (sandbox != 0 && !(ea.argt & EX_SBOXOK))
	{
	    // Command not allowed in sandbox.
	    errormsg = _(e_not_allowed_in_sandbox);
	    जा doend;
	}
#endif
	जर (restricted != 0 && (ea.argt & EX_RESTRICT))
	{
	    errormsg = _(e_command_not_allowed_in_rvim);
	    जा doend;
	}
	जर (!curbuf->b_p_ma && (ea.argt & EX_MODIFY))
	{
	    // Command not allowed in non-'modifiable' buffer
	    errormsg = _(e_cannot_make_changes_modifiable_is_off);
	    जा doend;
	}

	जर (!IS_USER_CMDIDX(ea.cmdidx))
	{
	    जर (cmdwin_type != 0 && !(ea.argt & EX_CMDWIN))
	    {
		// Command not allowed in the command line window
		errormsg = _(e_invalid_in_cmdline_window);
		जा doend;
	    }
	    जर (text_locked() && !(ea.argt & EX_LOCK_OK))
	    {
		// Command not allowed when text is locked
		errormsg = _(get_text_locked_संदेश());
		जा doend;
	    }
	}

	// Disallow editing another buffer when "curbuf_lock" is set.
	// Do allow ":checktime" (it is postponed).
	// Do allow ":edit" (check for an argument later).
	// Do allow ":file" with no arguments (check for an argument later).
	जर (!(ea.argt & (EX_CMDWIN | EX_LOCK_OK))
		&& ea.cmdidx != CMD_checktime
		&& ea.cmdidx != CMD_edit
		&& ea.cmdidx != CMD_file
		&& !IS_USER_CMDIDX(ea.cmdidx)
		&& curbuf_locked())
	    जा doend;

	जर (!ni && !(ea.argt & EX_RANGE) && ea.addr_मोजणी > 0)
	{
	    errormsg = _(e_no_range_allowed);
	    जा doend;
	}
    }

    जर (!ni && !(ea.argt & EX_BANG) && ea.forceit)
    {
	errormsg = _(e_no_bang_allowed);
	जा doend;
    }

    /*
     * Don't complain about the range if it is not used
     * (could happen if line_count is accidentally set to 0).
     */
    जर (!ea.skip && !ni && (ea.argt & EX_RANGE))
    {
	/*
	 * If the range is backwards, ask for confirmation and, if given, swap
	 * ea.line1 & ea.line2 so it's forwards again.
	 * When global command is busy, don't ask, will fail below.
	 */
	जर (!global_busy && ea.line1 > ea.line2)
	{
	    जर (संदेश_silent == 0)
	    {
		जर (sourcing || exmode_active)
		{
		    errormsg = _(e_backwards_range_given);
		    जा doend;
		}
		जर (ask_yesno((char_u *)
			_("Backwards range given, OK to swap"), FALSE) != 'y')
		    जा doend;
	    }
	    lnum = ea.line1;
	    ea.line1 = ea.line2;
	    ea.line2 = lnum;
	}
	जर ((errormsg = invalid_range(&ea)) != रिक्त)
	    जा doend;
    }

    जर ((ea.addr_type == ADDR_OTHER) && ea.addr_मोजणी == 0)
	// default is 1, not cursor
	ea.line2 = 1;

    correct_range(&ea);

#ifdef FEAT_FOLDING
    जर (((ea.argt & EX_WHOLEFOLD) || ea.addr_मोजणी >= 2) && !global_busy
	    && ea.addr_type == ADDR_ओळी)
    {
	// Put the first line at the start of a closed fold, put the last line
	// at the end of a closed fold.
	(शून्य)hasFolding(ea.line1, &ea.line1, रिक्त);
	(शून्य)hasFolding(ea.line2, रिक्त, &ea.line2);
    }
#endif

#ifdef FEAT_QUICKFIX
    /*
     * For the ":make" and ":grep" commands we insert the 'makeprg'/'grepprg'
     * option here, so things like % get expanded.
     */
    p = replace_makeprg(&ea, p, cmdlinep);
    जर (p == रिक्त)
	जा doend;
#endif

    /*
     * Skip to start of argument.
     * Don't do this for the ":!" command, because ":!! -l" needs the space.
     */
    जर (ea.cmdidx == CMD_bang)
	ea.arg = p;
    अन्यथा
	ea.arg = skipwhite(p);

    // ":file" cannot be run with an argument when "curbuf_lock" is set
    जर (ea.cmdidx == CMD_file && *ea.arg != NUL && curbuf_locked())
	जा doend;

    /*
     * Check for "++opt=val" argument.
     * Must be first, allow ":w ++enc=utf8 !cmd"
     */
    जर (ea.argt & EX_ARGOPT)
	पर्यंत (ea.arg[0] == '+' && ea.arg[1] == '+')
	    जर (getargopt(&ea) == FAIL && !ni)
	    {
		errormsg = _(e_invalid_argument);
		जा doend;
	    }

    जर (ea.cmdidx == CMD_write || ea.cmdidx == CMD_update)
    {
	जर (*ea.arg == '>')			// append
	{
	    जर (*++ea.arg != '>')		// typed wrong
	    {
		errormsg = _(e_use_w_or_w_gt_gt);
		जा doend;
	    }
	    ea.arg = skipwhite(ea.arg + 1);
	    ea.append = TRUE;
	}
	अन्यथा जर (*ea.arg == '!' && ea.cmdidx == CMD_write)  // :w !filter
	{
	    ++ea.arg;
	    ea.usefilter = TRUE;
	}
    }

    जर (ea.cmdidx == CMD_read)
    {
	जर (ea.forceit)
	{
	    ea.usefilter = TRUE;		// :r! filter if ea.forceit
	    ea.forceit = FALSE;
	}
	अन्यथा जर (*ea.arg == '!')		// :r !filter
	{
	    ++ea.arg;
	    ea.usefilter = TRUE;
	}
    }

    जर (ea.cmdidx == CMD_lshift || ea.cmdidx == CMD_rshift)
    {
	ea.amount = 1;
	पर्यंत (*ea.arg == *ea.cmd)		// count number of '>' or '<'
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
    जर ((ea.argt & EX_CMDARG) && !ea.usefilter)
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
    जर ((ea.argt & EX_TRLBAR) && !ea.usefilter)
    {
	separate_nextcmd(&ea, FALSE);
    }
    अन्यथा जर (ea.cmdidx == CMD_bang
	    || ea.cmdidx == CMD_terminal
	    || ea.cmdidx == CMD_global
	    || ea.cmdidx == CMD_vglobal
	    || ea.usefilter
#ifdef FEAT_EVAL
	    || inside_block(&ea)
#endif
	    )
    {
	साठी (p = ea.arg; *p; ++p)
	{
	    // Remove one backslash before a newline
	    जर (*p == '\\' && p[1] == '\n')
		STRMOVE(p, p + 1);
	    अन्यथा जर (*p == '\n' && !(ea.argt & EX_अभिव्यक्ती_ARG))
	    {
		ea.nextcmd = p + 1;
		*p = NUL;
		थांबा;
	    }
	}
    }

    जर ((ea.argt & EX_DFLALL) && ea.addr_मोजणी == 0)
	address_default_all(&ea);

    // accept numbered register only when no count allowed (:put)
    जर (       (ea.argt & EX_REGSTR)
	    && *ea.arg != NUL
	       // Do not allow register = for user commands
	    && (!IS_USER_CMDIDX(ea.cmdidx) || *ea.arg != '=')
	    && !((ea.argt & EX_मोजणी) && VIM_ISDIGIT(*ea.arg)))
    {
#जर !defined(FEAT_CLIPBOARD) && !defined(FEAT_CLIPBOARD_PROVIDER)
	// check these explicitly for a more specific error message
	जर (*ea.arg == '*' || *ea.arg == '+')
	{
	    errormsg = _(e_invalid_register_name);
	    जा doend;
	}
#endif
	जर (valid_yank_reg(*ea.arg, (!IS_USER_CMDIDX(ea.cmdidx)
			    && ea.cmdidx != CMD_put && ea.cmdidx != CMD_iput)))
	{
	    ea.regname = *ea.arg++;
#ifdef FEAT_EVAL
	    // for '=' register: accept the rest of the line as an expression
	    जर (ea.arg[-1] == '=' && ea.arg[0] != NUL)
	    {
		जर (!ea.skip)
		{
		    set_अभिव्यक्ती_ओळ(vim_strsave(ea.arg), &ea);
		    did_set_अभिव्यक्ती_ओळ = TRUE;
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
    जर ((ea.argt & EX_मोजणी) && VIM_ISDIGIT(*ea.arg)
	    && (!(ea.argt & EX_BUFNAME) || *(p = skipdigits(ea.arg + 1)) == NUL
							  || VIM_ISWHITE(*p)))
    {
	n = getdigits_quoted(&ea.arg);
	ea.arg = skipwhite(ea.arg);
	जर (n <= 0 && !ni && (ea.argt & EX_ZEROR) == 0)
	{
	    errormsg = _(e_positive_मोजणी_required);
	    जा doend;
	}
	जर (ea.addr_type != ADDR_ओळी)	// e.g. :buffer 2, :sleep 3
	{
	    ea.line2 = n;
	    जर (ea.addr_मोजणी == 0)
		ea.addr_मोजणी = 1;
	}
	अन्यथा
	{
	    ea.line1 = ea.line2;
	    जर (ea.line2 >= दीर्घ_उच्च - (n - 1))
		ea.line2 = दीर्घ_उच्च;  // avoid overflow
	    अन्यथा
		ea.line2 += n - 1;
	    ++ea.addr_मोजणी;
	    /*
	     * Be vi compatible: no error message for out of range.
	     */
	    जर (ea.line2 > curbuf->b_ml.ml_ओळ_मोजणी)
		ea.line2 = curbuf->b_ml.ml_ओळ_मोजणी;
	}
    }

    /*
     * Check for flags: 'l', 'p' and '#'.
     */
    जर (ea.argt & EX_FLAGS)
	get_flags(&ea);
    जर (!ni && !(ea.argt & EX_EXTRA) && *ea.arg != NUL
	    && *ea.arg != '"' && (*ea.arg != '|' || (ea.argt & EX_TRLBAR) == 0))
    {
	// no arguments allowed but there is something
	errormsg = ex_errmsg(e_trailing_characters_str, ea.arg);
	जा doend;
    }

    जर (!ni && (ea.argt & EX_NEEDARG) && *ea.arg == NUL)
    {
	errormsg = _(e_argument_required);
	जा doend;
    }

#ifdef FEAT_EVAL
    /*
     * Skip the command when it's not going to be executed.
     * The commands like :if, :endif, etc. always need to be executed.
     * Also make an exception for commands that handle a trailing command
     * themselves.
     */
    जर (ea.skip)
    {
	निवड (ea.cmdidx)
	{
	    // commands that need evaluation
	    स्थिती CMD_while:
	    स्थिती CMD_endwhile:
	    स्थिती CMD_for:
	    स्थिती CMD_endfor:
	    स्थिती CMD_if:
	    स्थिती CMD_elseif:
	    स्थिती CMD_else:
	    स्थिती CMD_endif:
	    स्थिती CMD_try:
	    स्थिती CMD_catch:
	    स्थिती CMD_finally:
	    स्थिती CMD_endtry:
	    स्थिती CMD_function:
	    स्थिती CMD_def:
				थांबा;

	    // commands that read a block of lines
	    स्थिती CMD_abstract:
	    स्थिती CMD_append:
	    स्थिती CMD_autocmd:
	    स्थिती CMD_change:
	    स्थिती CMD_class:
	    स्थिती CMD_command:
	    स्थिती CMD_enum:
	    स्थिती CMD_घाला:
	    स्थिती CMD_interface:
	    स्थिती CMD_loadkeymap:
				थांबा;

	    // Commands that handle '|' themselves.  Check: A command should
	    // either have the EX_TRLBAR flag, appear in this list or appear in
	    // the list at ":help :bar".
	    स्थिती CMD_aboveleft:
	    स्थिती CMD_and:
	    स्थिती CMD_belowright:
	    स्थिती CMD_botright:
	    स्थिती CMD_browse:
	    स्थिती CMD_call:
	    स्थिती CMD_confirm:
	    स्थिती CMD_const:
	    स्थिती CMD_delfunction:
	    स्थिती CMD_djump:
	    स्थिती CMD_dlist:
	    स्थिती CMD_dsearch:
	    स्थिती CMD_dsplit:
	    स्थिती CMD_echo:
	    स्थिती CMD_echoerr:
	    स्थिती CMD_echomsg:
	    स्थिती CMD_echon:
	    स्थिती CMD_eval:
	    स्थिती CMD_execute:
	    स्थिती CMD_filter:
	    स्थिती CMD_final:
	    स्थिती CMD_help:
	    स्थिती CMD_hide:
	    स्थिती CMD_horizontal:
	    स्थिती CMD_ijump:
	    स्थिती CMD_ilist:
	    स्थिती CMD_isearch:
	    स्थिती CMD_isplit:
	    स्थिती CMD_keepalt:
	    स्थिती CMD_keepjumps:
	    स्थिती CMD_keepmarks:
	    स्थिती CMD_keeppatterns:
	    स्थिती CMD_leftabove:
	    स्थिती CMD_let:
	    स्थिती CMD_lockmarks:
	    स्थिती CMD_lockvar:
	    स्थिती CMD_lua:
	    स्थिती CMD_match:
	    स्थिती CMD_mzscheme:
	    स्थिती CMD_noautocmd:
	    स्थिती CMD_noswapfile:
	    स्थिती CMD_perl:
	    स्थिती CMD_psearch:
	    स्थिती CMD_py3:
	    स्थिती CMD_python3:
	    स्थिती CMD_python:
	    स्थिती CMD_return:
	    स्थिती CMD_rightbelow:
	    स्थिती CMD_ruby:
	    स्थिती CMD_silent:
	    स्थिती CMD_smagic:
	    स्थिती CMD_snomagic:
	    स्थिती CMD_substitute:
	    स्थिती CMD_syntax:
	    स्थिती CMD_tab:
	    स्थिती CMD_tcl:
	    स्थिती CMD_throw:
	    स्थिती CMD_tilde:
	    स्थिती CMD_topleft:
	    स्थिती CMD_unlet:
	    स्थिती CMD_unlockvar:
	    स्थिती CMD_var:
	    स्थिती CMD_verbose:
	    स्थिती CMD_vertical:
	    स्थिती CMD_wincmd:
				थांबा;

	    पूर्वनिर्धारित:		जा doend;
	}
    }
#endif

    जर ((ea.argt & EX_XFILE)
	    && expand_filename(&ea, cmdlinep, &errormsg) == FAIL)
	जा doend;

#ifdef FEAT_EVAL
    जर (is_export && (ea.argt & EX_EXPORT) == 0)
    {
	emsg(_(e_invalid_command_after_export));
	जा doend;
    }
#endif

    /*
     * Accept buffer name.  Cannot be used at the same time with a buffer
     * number.  Don't do this for a user command.
     */
    जर ((ea.argt & EX_BUFNAME) && *ea.arg != NUL && ea.addr_मोजणी == 0
	    && !IS_USER_CMDIDX(ea.cmdidx))
    {
	/*
	 * :bdelete, :bwipeout and :bunload take several arguments, separated
	 * by spaces: find next space (skipping over escaped characters).
	 * The others take one argument: ignore trailing spaces.
	 */
	जर (ea.cmdidx == CMD_bdelete || ea.cmdidx == CMD_bwipeout
						  || ea.cmdidx == CMD_bunload)
	    p = skiptowhite_esc(ea.arg);
	अन्यथा
	{
	    p = ea.arg + STRLEN(ea.arg);
	    पर्यंत (p > ea.arg && VIM_ISWHITE(p[-1]))
		--p;
	}
	ea.line2 = buflist_findpat(ea.arg, p, (ea.argt & EX_BUFUNL) != 0,
								FALSE, FALSE);
	जर (ea.line2 < 0)	    // failed
	    जा doend;
	ea.addr_मोजणी = 1;
	ea.arg = skipwhite(p);
    }

    // The :try command saves the emsg_silent flag, reset it here when
    // ":silent! try" was used, it should only apply to :try itself.
    जर (ea.cmdidx == CMD_try && cmdmod.cmod_did_esilent > 0)
    {
	emsg_silent -= cmdmod.cmod_did_esilent;
	जर (emsg_silent < 0)
	    emsg_silent = 0;
	cmdmod.cmod_did_esilent = 0;
    }

/*
 * 7. Execute the command.
 */

    जर (IS_USER_CMDIDX(ea.cmdidx))
    {
	/*
	 * Execute a user-defined command.
	 */
	do_ucmd(&ea);
    }
    अन्यथा
    {
	/*
	 * Call the function to execute the builtin command.
	 */
	(cmdnames[ea.cmdidx].cmd_func)(&ea);
	जर (ea.errmsg != रिक्त)
	    errormsg = ea.errmsg;
    }

#ifdef FEAT_EVAL
    // A command will reset "is_export" when exporting an item.  If it is still
    // set something went wrong or the command was never executed.
    जर (!ea.skip && is_export)
    {
	जर (errormsg == रिक्त)
	    errormsg = _(e_export_with_invalid_argument);
	is_export = FALSE;
    }

    // Set flag that any command was executed, used by ex_vim9script().
    // Not if this was a command that wasn't executed or :endif.
    जर (sourcing_a_script(&ea)
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
    जर (need_rethrow)
	do_throw(cstack);
    अन्यथा जर (check_cstack)
    {
	जर (source_finished(fgetline, cookie))
	    do_finish(&ea, TRUE);
	अन्यथा जर (getline_equal(fgetline, cookie, get_func_ओळ)
						   && current_func_returned())
	    do_return(&ea, TRUE, FALSE, रिक्त);
    }
    need_rethrow = check_cstack = FALSE;
#endif

doend:
    जर (curwin->w_cursor.lnum == 0)	// can happen with zero line number
    {
	curwin->w_cursor.lnum = 1;
	curwin->w_cursor.col = 0;
    }

    जर (errormsg != रिक्त && *errormsg != NUL && !did_emsg)
    {
	जर ((sourcing || !किल्ली_Typed) && !did_append_cmd)
	{
	    जर (errormsg != (अक्षर *)IObuff)
	    {
		STRCPY(IObuff, errormsg);
		errormsg = (अक्षर *)IObuff;
	    }
	    append_command(*cmdlinep);
	}
	emsg(errormsg);
    }
#ifdef FEAT_EVAL
    do_errthrow(cstack,
	    (ea.cmdidx != CMD_SIZE && !IS_USER_CMDIDX(ea.cmdidx))
			? cmdnames[(पूर्णांक)ea.cmdidx].cmd_name : (char_u *)रिक्त);

    जर (did_set_अभिव्यक्ती_ओळ)
	set_अभिव्यक्ती_ओळ(रिक्त, रिक्त);
    is_export = FALSE;
#endif

    undo_cmdmod(&cmdmod);
    cmdmod = save_cmdmod;
    reg_executing = save_reg_executing;
    pending_end_reg_executing = save_pending_end_reg_executing;

    जर (ea.nextcmd && *ea.nextcmd == NUL)	// not really a next command
	ea.nextcmd = रिक्त;

#ifdef FEAT_EVAL
    --ex_nesting_level;
    vim_free(ea.cmdline_tofree);
#endif

    परत ea.nextcmd;
}

स्थिर अक्षर ex_error_buf[संदेश_BUF_LEN];

/*
 * Return an error message with argument included.
 * Uses a static buffer, only the last error will be kept.
 * "msg" will be translated, caller should use N_().
 */
    अक्षर *
ex_errmsg(अक्षर *संदेश, char_u *arg)
{
    vim_snprintf(ex_error_buf, संदेश_BUF_LEN, _(संदेश), arg);
    परत ex_error_buf;
}

/*
 * The "+" string used in place of an empty command in Ex mode.
 * This string is used in pointer comparison.
 */
स्थिर स्थिरांक अक्षर exmode_plus[] = "+";

/*
 * Handle a range without a command.
 * Returns an error message on failure.
 */
    अक्षर *
ex_range_without_command(exarg_T *eap)
{
    अक्षर *errormsg = रिक्त;

    जर ((*eap->cmd == '|' ||
		(exmode_active && eap->cmd != (char_u *)exmode_plus + 1))
#ifdef FEAT_EVAL
	    && !in_vim9script()
#endif
       )
    {
	eap->cmdidx = CMD_print;
	eap->argt = EX_RANGE+EX_मोजणी+EX_TRLBAR;
	जर ((errormsg = invalid_range(eap)) == रिक्त)
	{
	    correct_range(eap);
	    ex_print(eap);
	}
    }
    अन्यथा जर (eap->addr_मोजणी != 0)
    {
	जर (eap->line2 > curbuf->b_ml.ml_ओळ_मोजणी)
	{
	    // With '-' in 'cpoptions' a line number past the file is an
	    // error, otherwise put it at the end of the file.
	    जर (vim_strchr(p_cpo, CPO_MINUS) != रिक्त)
		eap->line2 = -1;
	    अन्यथा
		eap->line2 = curbuf->b_ml.ml_ओळ_मोजणी;
	}

	जर (eap->line2 < 0)
	    errormsg = _(e_invalid_range);
	अन्यथा
	{
	    जर (eap->line2 == 0)
		curwin->w_cursor.lnum = 1;
	    अन्यथा
		curwin->w_cursor.lnum = eap->line2;
	    beginline(BL_SOL | BL_FIX);
	}
    }
    परत errormsg;
}

/*
 * Check for an Ex command with optional tail.
 * If there is a match advance "pp" to the argument and return TRUE.
 * If "noparen" is TRUE do not recognize the command followed by "(" or ".".
 */
    स्थिर पूर्णांक
checkforcmd_opt(
    char_u	**pp,		// start of command
    अक्षर	*cmd,		// name of command
    पूर्णांक		len,		// required length
    पूर्णांक		noparen)
{
    पूर्णांक		i;

    साठी (i = 0; cmd[i] != NUL; ++i)
	जर (((char_u *)cmd)[i] != (*pp)[i])
	    थांबा;
    जर (i >= len && !ASCII_ISALPHA((*pp)[i]) && (*pp)[i] != '_'
			 && (!noparen || ((*pp)[i] != '(' && (*pp)[i] != '.')))
    {
	*pp = skipwhite(*pp + i);
	परत TRUE;
    }
    परत FALSE;
}

/*
 * Check for an Ex command with optional tail.
 * If there is a match advance "pp" to the argument and return TRUE.
 */
    पूर्णांक
checkforcmd(
    char_u	**pp,		// start of command
    अक्षर	*cmd,		// name of command
    पूर्णांक		len)		// required length
{
    परत checkforcmd_opt(pp, cmd, len, FALSE);
}

/*
 * Check for an Ex command with optional tail, not followed by "(" or ".".
 * If there is a match advance "pp" to the argument and return TRUE.
 */
    पूर्णांक
checkforcmd_noparen(
    char_u	**pp,		// start of command
    अक्षर	*cmd,		// name of command
    पूर्णांक		len)		// required length
{
    परत checkforcmd_opt(pp, cmd, len, TRUE);
}

/*
 * Find the replacement text of a ":command", the part after the attributes and
 * the command name.  Returns NULL when there is none.  Does not modify "arg"
 * and gives no error messages.
 */
    स्थिर char_u *
find_ucmd_repl(char_u *arg)
{
    char_u	*p = arg;

    // Skip over the attributes.
    पर्यंत (*p == '-')
	p = skipwhite(skiptowhite(p));

    // Skip over the command name.
    जर (!ASCII_ISALPHA(*p))
	परत रिक्त;
    पर्यंत (ASCII_ISALNUM(*p))
	++p;

    परत *p == NUL ? रिक्त : skipwhite(p);
}

/*
 * Find the "{" in "line" that starts a block for ":command" or ":autocmd".
 * That is the case when the command argument is "{" at the end of the line,
 * also when the command is nested in another ":command" or ":autocmd".
 * Returns NULL when the line does not start such a block.
 */
    char_u *
find_cmd_block_start(char_u *ओळ)
{
    char_u	*p = skipwhite(ओळ);

    साठी (;;)
    {
	char_u	*arg = p;

	जर (*p == '{' && ends_excmd2(p, skipwhite(p + 1)))
	    परत p;

	जर (checkforcmd_noparen(&arg, "autocmd", 2))
	{
	    जर (*arg == '!')
		arg = skipwhite(arg + 1);
	    p = au_find_cmd_arg(arg);
	}
	अन्यथा जर (checkforcmd_noparen(&arg, "command", 3))
	{
	    जर (*arg == '!')
		arg = skipwhite(arg + 1);
	    p = find_ucmd_repl(arg);
	}
	अन्यथा
	    परत रिक्त;

	जर (p == रिक्त)
	    परत रिक्त;
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
    पूर्णांक
parse_command_modifiers(
	exarg_T	    *eap,
	अक्षर	    **errormsg,
	cmdmod_T    *cmod,
	पूर्णांक	    skip_only)
{
    char_u  *orig_cmd = eap->cmd;
    char_u  *cmd_start = रिक्त;
    पूर्णांक	    use_plus_cmd = FALSE;
    पूर्णांक	    starts_with_colon = FALSE;
    पूर्णांक	    vim9script = in_vim9script();
    पूर्णांक	    has_visual_range = FALSE;

    CLEAR_POINTER(cmod);
    cmod->cmod_flags = sticky_cmdmod_flags;

    जर (STRNCMP(eap->cmd, "'<,'>", 5) == 0)
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
    साठी (;;)
    {
	char_u  *p;

	पर्यंत (*eap->cmd == ' ' || *eap->cmd == '\t' || *eap->cmd == ':')
	{
	    जर (*eap->cmd == ':')
		starts_with_colon = TRUE;
	    ++eap->cmd;
	}

	// in ex mode, an empty command (after modifiers) works like :+
	जर (*eap->cmd == NUL && exmode_active
		   && (getline_equal(eap->ea_getline, eap->cookie, getexmodeline)
		       || getline_equal(eap->ea_getline, eap->cookie, getexline))
			&& curwin->w_cursor.lnum < curbuf->b_ml.ml_ओळ_मोजणी)
	{
	    use_plus_cmd = TRUE;
	    जर (!skip_only)
		ex_pressedreturn = TRUE;
	    थांबा;  // no modifiers following
	}

	// ignore comment and empty lines
	जर (comment_start(eap->cmd, starts_with_colon))
	{
	    // a comment ends at a NL
	    eap->nextcmd = vim_strchr(eap->cmd, '\n');
	    जर (eap->nextcmd != रिक्त)
		++eap->nextcmd;
	    जर (vim9script)
	    {
		जर (has_cmdmod(cmod, FALSE))
		    *errormsg = _(e_command_modifier_without_command);
#ifdef FEAT_EVAL
		जर (eap->cmd[0] == '#' && eap->cmd[1] == '{'
							 && eap->cmd[2] != '{')
		    *errormsg = _(e_cannot_use_hash_curly_to_start_comment);
#endif
	    }
	    परत FAIL;
	}
	जर (*eap->cmd == '\n')
	{
	    eap->nextcmd = eap->cmd + 1;
	    परत FAIL;
	}
	जर (*eap->cmd == NUL)
	{
	    जर (!skip_only)
	    {
		ex_pressedreturn = TRUE;
		जर (vim9script && has_cmdmod(cmod, FALSE))
		    *errormsg = _(e_command_modifier_without_command);
	    }
	    परत FAIL;
	}

	p = skip_range(eap->cmd, TRUE, रिक्त);

	// In Vim9 script a variable can shadow a command modifier:
	//   verbose = 123
	//   verbose += 123
	//   silent! verbose = func()
	//   verbose.member = 2
	//   verbose[expr] = 2
	// But not:
	//   verbose [a, b] = list
	जर (vim9script)
	{
	    char_u *s, *n;

	    साठी (s = eap->cmd; ASCII_ISALPHA(*s); ++s)
		;
	    n = skipwhite(s);
	    जर (*n == '.' || *n == '=' || (*n != NUL && n[1] == '=')
		    || *s == '[')
		थांबा;
	}

	निवड (*p)
	{
	    // When adding an entry, also modify cmdmod_info_tab[].
	    स्थिती 'a':	जर (!checkforcmd_noparen(&eap->cmd, "aboveleft", 3))
			    थांबा;
			cmod->cmod_split |= WSP_ABOVE;
			सुरूठेवा;

	    स्थिती 'b':	जर (checkforcmd_noparen(&eap->cmd, "belowright", 3))
			{
			    cmod->cmod_split |= WSP_BELOW;
			    सुरूठेवा;
			}
			जर (checkforcmd_opt(&eap->cmd, "browse", 3, TRUE))
			{
#ifdef FEAT_BROWSE_CMD
			    cmod->cmod_flags |= CMOD_BROWSE;
#endif
			    सुरूठेवा;
			}
			जर (!checkforcmd_noparen(&eap->cmd, "botright", 2))
			    थांबा;
			cmod->cmod_split |= WSP_BOT;
			सुरूठेवा;

	    स्थिती 'c':	जर (!checkforcmd_opt(&eap->cmd, "confirm", 4, TRUE))
			    थांबा;
#जर defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
			cmod->cmod_flags |= CMOD_CONFIRM;
#endif
			सुरूठेवा;

	    स्थिती 'k':	जर (checkforcmd_noparen(&eap->cmd, "keepmarks", 3))
			{
			    cmod->cmod_flags |= CMOD_KEEPMARKS;
			    सुरूठेवा;
			}
			जर (checkforcmd_noparen(&eap->cmd, "keepalt", 5))
			{
			    cmod->cmod_flags |= CMOD_KEEPALT;
			    सुरूठेवा;
			}
			जर (checkforcmd_noparen(&eap->cmd, "keeppatterns", 5))
			{
			    cmod->cmod_flags |= CMOD_KEEPPATTERNS;
			    सुरूठेवा;
			}
			जर (!checkforcmd_noparen(&eap->cmd, "keepjumps", 5))
			    थांबा;
			cmod->cmod_flags |= CMOD_KEEPJUMPS;
			सुरूठेवा;

	    स्थिती 'f':	// only accept ":filter {pat} cmd"
			{
			    char_u  *reg_pat;
			    char_u  *nulp = रिक्त;
			    पूर्णांक	    c = 0;

			    जर (!checkforcmd_noparen(&p, "filter", 4)
				    || *p == NUL
				    || (ends_excmd(*p)
#ifdef FEAT_EVAL
					// in ":filter #pat# cmd" # does not
					// start a comment
				     && (!vim9script || VIM_ISWHITE(p[1]))
#endif
				     ))
				थांबा;
			    जर (*p == '!')
			    {
				cmod->cmod_filter_force = TRUE;
				p = skipwhite(p + 1);
				जर (*p == NUL || ends_excmd(*p))
				    थांबा;
			    }
#ifdef FEAT_EVAL
			    // Avoid that "filter(arg)" is recognized.
			    जर (vim9script && !VIM_ISWHITE(p[-1]))
				थांबा;
#endif
			    जर (skip_only)
				p = skip_vimgrep_pat(p, रिक्त, रिक्त);
			    अन्यथा
				// NOTE: This puts a NUL after the pattern.
				p = skip_vimgrep_pat_ext(p, &reg_pat, रिक्त,
								    &nulp, &c);
			    जर (p == रिक्त || *p == NUL)
				थांबा;
			    जर (!skip_only)
			    {
				cmod->cmod_filter_regmatch.regprog =
						vim_regcomp(reg_pat, RE_MAGIC);
				जर (cmod->cmod_filter_regmatch.regprog == रिक्त)
				    थांबा;
				// restore the character overwritten by NUL
				जर (nulp != रिक्त)
				    *nulp = c;
			    }
			    eap->cmd = p;
			    सुरूठेवा;
			}

	    स्थिती 'h':	जर (checkforcmd_noparen(&eap->cmd, "horizontal", 3))
			{
			    cmod->cmod_split |= WSP_HOR;
			    सुरूठेवा;
			}
			// ":hide" and ":hide | cmd" are not modifiers
			जर (p != eap->cmd || !checkforcmd_noparen(&p, "hide", 3)
					       || *p == NUL || ends_excmd(*p))
			    थांबा;
			eap->cmd = p;
			cmod->cmod_flags |= CMOD_HIDE;
			सुरूठेवा;

	    स्थिती 'l':	जर (checkforcmd_noparen(&eap->cmd, "lockmarks", 3))
			{
			    cmod->cmod_flags |= CMOD_LOCKMARKS;
			    सुरूठेवा;
			}
			जर (checkforcmd_noparen(&eap->cmd, "legacy", 3))
			{
			    जर (ends_excmd2(p, eap->cmd))
			    {
				*errormsg =
				      _(e_legacy_must_be_followed_by_command);
				परत FAIL;
			    }
			    // Make sure we do not have both legacy and Vim9
			    // flags set at the same time.
			    cmod->cmod_flags &= ~CMOD_VIM9CMD;
			    cmod->cmod_flags |= CMOD_LEGACY;
			    सुरूठेवा;
			}

			जर (!checkforcmd_noparen(&eap->cmd, "leftabove", 5))
			    थांबा;
			cmod->cmod_split |= WSP_ABOVE;
			सुरूठेवा;

	    स्थिती 'n':	जर (checkforcmd_noparen(&eap->cmd, "noautocmd", 3))
			{
			    cmod->cmod_flags |= CMOD_NOAUTOCMD;
			    सुरूठेवा;
			}
			जर (!checkforcmd_noparen(&eap->cmd, "noswapfile", 3))
			    थांबा;
			cmod->cmod_flags |= CMOD_NOSWAPFILE;
			सुरूठेवा;

	    स्थिती 'r':	जर (!checkforcmd_noparen(&eap->cmd, "rightbelow", 6))
			    थांबा;
			cmod->cmod_split |= WSP_BELOW;
			सुरूठेवा;

	    स्थिती 's':	जर (checkforcmd_noparen(&eap->cmd, "sandbox", 3))
			{
			    cmod->cmod_flags |= CMOD_SANDBOX;
			    सुरूठेवा;
			}
			जर (!checkforcmd_noparen(&eap->cmd, "silent", 3))
			    थांबा;
			cmod->cmod_flags |= CMOD_SILENT;
			जर (*eap->cmd == '!' && !VIM_ISWHITE(eap->cmd[-1]))
			{
			    // ":silent!", but not "silent !cmd"
			    eap->cmd = skipwhite(eap->cmd + 1);
			    cmod->cmod_flags |= CMOD_ERRSILENT;
			}
			सुरूठेवा;

	    स्थिती 't':	जर (checkforcmd_noparen(&p, "tab", 3))
			{
			    जर (!skip_only)
			    {
				दीर्घ tabnr = get_address(eap, &eap->cmd,
						    ADDR_TABS, eap->skip,
						    skip_only, FALSE, 1);
				जर (tabnr == MAXLNUM)
				    cmod->cmod_tab = tabpage_index(curtab) + 1;
				अन्यथा
				{
				    जर (tabnr < 0 || tabnr > LAST_TAB_NR)
				    {
					*errormsg = _(e_invalid_range);
					परत FAIL;
				    }
				    cmod->cmod_tab = tabnr + 1;
				}
			    }
			    eap->cmd = p;
			    सुरूठेवा;
			}
			जर (!checkforcmd_noparen(&eap->cmd, "topleft", 2))
			    थांबा;
			cmod->cmod_split |= WSP_शीर्ष;
			सुरूठेवा;

	    स्थिती 'u':	जर (!checkforcmd_noparen(&eap->cmd, "unsilent", 3))
			    थांबा;
			cmod->cmod_flags |= CMOD_UNSILENT;
			सुरूठेवा;

	    स्थिती 'v':	जर (checkforcmd_noparen(&eap->cmd, "vertical", 4))
			{
			    cmod->cmod_split |= WSP_VERT;
			    सुरूठेवा;
			}
			जर (checkforcmd_noparen(&eap->cmd, "vim9cmd", 4))
			{
			    जर (ends_excmd2(p, eap->cmd))
			    {
				*errormsg =
				      _(e_vim9cmd_must_be_followed_by_command);
				परत FAIL;
			    }
			    // Make sure we do not have both legacy and Vim9
			    // flags set at the same time.
			    cmod->cmod_flags &= ~CMOD_LEGACY;
			    cmod->cmod_flags |= CMOD_VIM9CMD;
			    सुरूठेवा;
			}
			जर (!checkforcmd_noparen(&p, "verbose", 4))
			    थांबा;
			जर (vim_isdigit(*eap->cmd))
			{
			    // zero means not set, one is verbose == 0, etc.
			    cmod->cmod_verbose = म_से_प((अक्षर *)eap->cmd) + 1;
			}
			अन्यथा
			    cmod->cmod_verbose = 2;  // default: verbose == 1
			eap->cmd = p;
			सुरूठेवा;
	}
	थांबा;
    }

    जर (has_visual_range)
    {
	जर (eap->cmd > cmd_start)
	{
	    // Move the '<,'> range to after the modifiers and insert a colon.
	    // Since the modifiers have been parsed put the colon on top of the
	    // space: "'<,'>mod cmd" -> "mod:'<,'>cmd
	    // Put eap->cmd after the colon.
	    जर (use_plus_cmd)
	    {
		माप_प्रकार len = STRLEN(cmd_start);

		// Special case: empty command uses "+":
		//  "'<,'>mods" -> "mods *+
		//  Use "*" instead of "'<,'>" to avoid the command getting
		//  longer, in case it was allocated.
		mch_memmove(orig_cmd, cmd_start, len);
		STRCPY(orig_cmd + len, " *+");
	    }
	    अन्यथा
	    {
		mch_memmove(cmd_start - 5, cmd_start, eap->cmd - cmd_start);
		eap->cmd -= 5;
		mch_memmove(eap->cmd - 1, ":'<,'>", 6);
	    }
	}
	अन्यथा
	    // No modifiers, move the pointer back.
	    // Special case: change empty command to "+".
	    जर (use_plus_cmd)
		eap->cmd = (char_u *)"'<,'>+";
	    अन्यथा
		eap->cmd = orig_cmd;
    }
    अन्यथा जर (use_plus_cmd)
	eap->cmd = (char_u *)exmode_plus;

    परत OK;
}

/*
 * Return TRUE if "cmod" has anything set.
 */
    पूर्णांक
has_cmdmod(cmdmod_T *cmod, पूर्णांक ignore_silent)
{
    परत (cmod->cmod_flags != 0 && (!ignore_silent
		|| (cmod->cmod_flags
		      & ~(CMOD_SILENT | CMOD_ERRSILENT | CMOD_UNSILENT)) != 0))
	    || cmod->cmod_split != 0
	    || cmod->cmod_verbose > 0
	    || cmod->cmod_tab != 0
	    || cmod->cmod_filter_regmatch.regprog != रिक्त;
}

#जर defined(FEAT_EVAL)
/*
 * If Vim9 script and "cmdmod" has anything set give an error and return TRUE.
 */
    पूर्णांक
cmdmod_error(पूर्णांक ignore_silent)
{
    जर (in_vim9script() && has_cmdmod(&cmdmod, ignore_silent))
    {
	emsg(_(e_misplaced_command_modifier));
	परत TRUE;
    }
    परत FALSE;
}
#endif

/*
 * Apply the command modifiers.  Saves current state in "cmdmod", call
 * undo_cmdmod() later.
 */
    शून्य
apply_cmdmod(cmdmod_T *cmod)
{
#ifdef HAVE_SANDBOX
    जर ((cmod->cmod_flags & CMOD_SANDBOX) && !cmod->cmod_did_sandbox)
    {
	++sandbox;
	cmod->cmod_did_sandbox = TRUE;
    }
#endif
    जर (cmod->cmod_verbose > 0)
    {
	जर (cmod->cmod_verbose_save == 0)
	    cmod->cmod_verbose_save = p_verbose + 1;
	p_verbose = cmod->cmod_verbose - 1;
    }

    जर ((cmod->cmod_flags & (CMOD_SILENT | CMOD_UNSILENT))
	    && cmod->cmod_save_संदेश_silent == 0)
    {
	cmod->cmod_save_संदेश_silent = संदेश_silent + 1;
	cmod->cmod_save_संदेश_scroll = संदेश_scroll;
    }
    जर (cmod->cmod_flags & CMOD_SILENT)
	++संदेश_silent;
    जर (cmod->cmod_flags & CMOD_UNSILENT)
	संदेश_silent = 0;

    जर (cmod->cmod_flags & CMOD_ERRSILENT)
    {
	++emsg_silent;
	++cmod->cmod_did_esilent;
    }

    जर ((cmod->cmod_flags & CMOD_NOAUTOCMD) && cmod->cmod_save_ei == रिक्त)
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
    शून्य
undo_cmdmod(cmdmod_T *cmod)
{
    जर (cmod->cmod_verbose_save > 0)
    {
	p_verbose = cmod->cmod_verbose_save - 1;
	cmod->cmod_verbose_save = 0;
    }

#ifdef HAVE_SANDBOX
    जर (cmod->cmod_did_sandbox)
    {
	--sandbox;
	cmod->cmod_did_sandbox = FALSE;
    }
#endif

    जर (cmod->cmod_save_ei != रिक्त)
    {
	// Restore 'eventignore' to the value before ":noautocmd".
	set_string_option_direct((char_u *)"ei", -1, cmod->cmod_save_ei,
							   OPT_FREE, SID_NONE);
	free_string_option(cmod->cmod_save_ei);
	cmod->cmod_save_ei = रिक्त;
    }

    vim_regfree(cmod->cmod_filter_regmatch.regprog);

    जर (cmod->cmod_save_संदेश_silent > 0)
    {
	// messages could be enabled for a serious error, need to check if the
	// counters don't become negative
	जर (!did_emsg || संदेश_silent > cmod->cmod_save_संदेश_silent - 1)
	    संदेश_silent = cmod->cmod_save_संदेश_silent - 1;
	emsg_silent -= cmod->cmod_did_esilent;
	जर (emsg_silent < 0)
	    emsg_silent = 0;
	// Restore msg_scroll, it's set by file I/O commands, even when no
	// message is actually displayed.
	संदेश_scroll = cmod->cmod_save_संदेश_scroll;

	// "silent reg" or "silent echo x" inside "redir" leaves msg_col
	// somewhere in the line.  Put it back in the first column.
	जर (redirecting())
	    संदेश_col = 0;

	cmod->cmod_save_संदेश_silent = 0;
	cmod->cmod_did_esilent = 0;
    }
}

/*
 * Parse the address range, if any, in "eap".
 * May set the last search pattern, unless "silent" is TRUE.
 * Return FAIL and set "errormsg" or return OK.
 */
    पूर्णांक
parse_cmd_address(exarg_T *eap, अक्षर **errormsg, पूर्णांक silent)
{
    पूर्णांक		address_मोजणी = 1;
    linenr_T	lnum;
    पूर्णांक		need_check_cursor = FALSE;
    पूर्णांक		ret = FAIL;

    // Repeat for all ',' or ';' separated addresses.
    साठी (;;)
    {
	eap->line1 = eap->line2;
	eap->line2 = default_address(eap);
	eap->cmd = skipwhite(eap->cmd);
	lnum = get_address(eap, &eap->cmd, eap->addr_type, eap->skip, silent,
					eap->addr_मोजणी == 0, address_मोजणी++);
	जर (eap->cmd == रिक्त)	// error detected
	    जा theend;
	जर (lnum == MAXLNUM)
	{
	    जर (*eap->cmd == '%')   // '%' - all lines
	    {
		++eap->cmd;
		निवड (eap->addr_type)
		{
		    स्थिती ADDR_ओळी:
		    स्थिती ADDR_OTHER:
			eap->line1 = 1;
			eap->line2 = curbuf->b_ml.ml_ओळ_मोजणी;
			थांबा;
		    स्थिती ADDR_LOADED_BUFFERS:
			{
			    buf_T	*buf = firstbuf;

			    पर्यंत (buf->b_next != रिक्त
						  && buf->b_ml.ml_mfp == रिक्त)
				buf = buf->b_next;
			    eap->line1 = buf->b_fnum;
			    buf = lastbuf;
			    पर्यंत (buf->b_prev != रिक्त
						  && buf->b_ml.ml_mfp == रिक्त)
				buf = buf->b_prev;
			    eap->line2 = buf->b_fnum;
			    थांबा;
			}
		    स्थिती ADDR_BUFFERS:
			eap->line1 = firstbuf->b_fnum;
			eap->line2 = lastbuf->b_fnum;
			थांबा;
		    स्थिती ADDR_WINDOWS:
		    स्थिती ADDR_TABS:
			जर (IS_USER_CMDIDX(eap->cmdidx))
			{
			    eap->line1 = 1;
			    eap->line2 = eap->addr_type == ADDR_WINDOWS
						  ? LAST_WIN_NR : LAST_TAB_NR;
			}
			अन्यथा
			{
			    // there is no Vim command which uses '%' and
			    // ADDR_WINDOWS or ADDR_TABS
			    *errormsg = _(e_invalid_range);
			    जा theend;
			}
			थांबा;
		    स्थिती ADDR_TABS_RELATIVE:
		    स्थिती ADDR_UNSIGNED:
		    स्थिती ADDR_QUICKFIX:
			*errormsg = _(e_invalid_range);
			जा theend;
		    स्थिती ADDR_ARGUMENTS:
			जर (ARGCOUNT == 0)
			    eap->line1 = eap->line2 = 0;
			अन्यथा
			{
			    eap->line1 = 1;
			    eap->line2 = ARGCOUNT;
			}
			थांबा;
		    स्थिती ADDR_QUICKFIX_VALID:
#ifdef FEAT_QUICKFIX
			eap->line1 = 1;
			eap->line2 = qf_get_valid_size(eap);
			जर (eap->line2 == 0)
			    eap->line2 = 1;
#endif
			थांबा;
		    स्थिती ADDR_NONE:
			// Will give an error later if a range is found.
			थांबा;
		}
		++eap->addr_मोजणी;
	    }
	    अन्यथा जर (*eap->cmd == '*' && vim_strchr(p_cpo, CPO_STAR) == रिक्त)
	    {
		pos_T	    *fp;

		// '*' - visual area
		जर (eap->addr_type != ADDR_ओळी)
		{
		    *errormsg = _(e_invalid_range);
		    जा theend;
		}

		++eap->cmd;
		जर (!eap->skip)
		{
		    fp = getmark('<', FALSE);
		    जर (check_mark(fp) == FAIL)
			जा theend;
		    eap->line1 = fp->lnum;
		    fp = getmark('>', FALSE);
		    जर (check_mark(fp) == FAIL)
			जा theend;
		    eap->line2 = fp->lnum;
		    ++eap->addr_मोजणी;
		}
	    }
	}
	अन्यथा
	    eap->line2 = lnum;
	eap->addr_मोजणी++;

	जर (*eap->cmd == ';')
	{
	    जर (!eap->skip)
	    {
		curwin->w_cursor.lnum = eap->line2;

		// Don't leave the cursor on an illegal line or column, but do
		// accept zero as address, so 0;/PATTERN/ works correctly
		// (where zero usually means to use the first line).
		// Check the cursor position before returning.
		जर (eap->line2 > 0)
		    check_cursor();
		अन्यथा
		    check_cursor_col();
		need_check_cursor = TRUE;
	    }
	}
	अन्यथा जर (*eap->cmd != ',')
	    थांबा;
	++eap->cmd;
    }

    // One address given: set start and end lines.
    जर (eap->addr_मोजणी == 1)
    {
	eap->line1 = eap->line2;
	// ... but only implicit: really no address given
	जर (lnum == MAXLNUM)
	    eap->addr_मोजणी = 0;
    }
    ret = OK;

theend:
    जर (need_check_cursor)
	check_cursor();
    परत ret;
}

/*
 * Append "cmd" to the error message in IObuff.
 * Takes care of limiting the length and handling 0xa0, which would be
 * invisible otherwise.
 */
    स्थिर शून्य
append_command(char_u *cmd)
{
    माप_प्रकार  len = STRLEN(IObuff);
    char_u  *s = cmd;
    char_u  *d;

    जर (len > IOSIZE - 100)
    {
	// Not enough space, truncate and put in "...".
	d = IObuff + IOSIZE - 100;
	d -= mb_head_off(IObuff, d);
	STRCPY(d, "...");
    }
    STRCAT(IObuff, ": ");
    d = IObuff + STRLEN(IObuff);
    पर्यंत (*s != NUL && d - IObuff + 5 < IOSIZE)
    {
	जर (enc_utf8 ? (s[0] == 0xc2 && s[1] == 0xa0) : *s == 0xa0)
	{
	    s += enc_utf8 ? 2 : 1;
	    STRCPY(d, "<a0>");
	    d += 4;
	}
	अन्यथा जर (d - IObuff + (*mb_ptr2len)(s) + 1 >= IOSIZE)
	    थांबा;
	अन्यथा
	    MB_COPY_CHAR(s, d);
    }
    *d = NUL;
}

#जर defined(FEAT_EVAL)
/*
 * If "start" points "&opt", "&l:opt", "&g:opt" or "$ENV" return a pointer to
 * the name.  Otherwise just return "start".
 */
    char_u *
skip_option_env_lead(char_u *start)
{
    char_u *name = start;

    जर (*start == '&')
    {
	जर ((start[1] == 'l' || start[1] == 'g') && start[2] == ':')
	    name += 3;
	अन्यथा
	    name += 1;
    }
    अन्यथा जर (*start == '$')
	name += 1;
    परत name;
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
    स्थिर पूर्णांक
one_letter_cmd(char_u *p, cmdidx_T *idx)
{
    जर (in_vim9script())
	परत FALSE;
    जर (p[0] == 'k'
	    && (p[1] != 'e' || (p[1] == 'e' && p[2] != 'e')))
    {
	*idx = CMD_k;
	परत TRUE;
    }
    जर (p[0] == 's'
	    && ((p[1] == 'c' && (p[2] == NUL || (p[2] != 's' && p[2] != 'r'
			&& (p[3] == NUL || (p[3] != 'i' && p[4] != 'p')))))
		|| p[1] == 'g'
		|| (p[1] == 'i' && p[2] != 'm' && p[2] != 'l' && p[2] != 'g')
		|| p[1] == 'I'
		|| (p[1] == 'r' && p[2] != 'e')))
    {
	*idx = CMD_substitute;
	परत TRUE;
    }
    परत FALSE;
}

#जर defined(FEAT_EVAL)
/*
 * Return TRUE if "cmd" starts with "123->", a number followed by a method
 * call.
 */
    पूर्णांक
संख्या_method(char_u *cmd)
{
    char_u *p = skipdigits(cmd);

    परत p > cmd && (p = skipwhite(p))[0] == '-' && p[1] == '>';
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
	पूर्णांक	*full UNUSED,
	पूर्णांक	(*lookup)(char_u *, माप_प्रकार, पूर्णांक cmd, cctx_T *) UNUSED,
	cctx_T	*cctx UNUSED)
{
    पूर्णांक		len;
    char_u	*p;
    पूर्णांक		i;
#ifndef FEAT_EVAL
    पूर्णांक		vim9 = FALSE;
#अन्यथा
    पूर्णांक		vim9 = in_vim9script();

    /*
     * Recognize a Vim9 script function/method call and assignment:
     * "lvar = value", "lvar(arg)", "[1, 2 3]->Func()"
     */
    p = eap->cmd;
    जर (lookup != रिक्त)
    {
	char_u *pskip = skip_option_env_lead(eap->cmd);

	जर (vim_strchr((char_u *)"{('[\"@&$", *p) != रिक्त
	       || ((p = to_name_const_end(pskip)) > eap->cmd && *p != NUL)
	       || (p[0] == '0' && p[1] == 'z'))
	{
	    पूर्णांक	    oplen;
	    पूर्णांक	    heredoc;
	    char_u  *swp;

	    जर (*eap->cmd == '&'
		    || (eap->cmd[0] == '$'
				  && eap->cmd[1] != '\'' && eap->cmd[1] != '"')
		    || (eap->cmd[0] == '@'
					&& (valid_yank_reg(eap->cmd[1], FALSE)
						       || eap->cmd[1] == '@')))
	    {
		जर (*eap->cmd == '&')
		{
		    p = eap->cmd + 1;
		    जर (STRNCMP("l:", p, 2) == 0 || STRNCMP("g:", p, 2) == 0)
			p += 2;
		    p = to_name_end(p, FALSE);
		}
		अन्यथा जर (*eap->cmd == '$')
		    p = to_name_end(eap->cmd + 1, FALSE);
		अन्यथा
		    p = eap->cmd + 2;
		जर (ends_excmd(*skipwhite(p)))
		{
		    // "&option <NL>", "$ENV <NL>" and "@r <NL>" are the start
		    // of an expression.
		    eap->cmdidx = CMD_eval;
		    परत eap->cmd;
		}
		// "&option" can be followed by "->" or "=", check below
	    }

	    जर (vim9 && *p == '<')
	    {
		// generic function type args
		जर (skip_generic_func_type_args(&p) == FAIL)
		{
		    eap->cmdidx = CMD_SIZE;
		    परत p;
		}
	    }

	    swp = skipwhite(p);

	    जर (
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
		जर (*eap->cmd == '{' && ends_excmd(*skipwhite(eap->cmd + 1)))
		{
		    // "{" by itself is the start of a block.
		    eap->cmdidx = CMD_block;
		    परत eap->cmd + 1;
		}
		eap->cmdidx = CMD_eval;
		परत eap->cmd;
	    }

	    जर ((p != eap->cmd && (
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
		जर (skip_अभिव्यक्ती(&after, रिक्त) == OK)
		{
		    after = skipwhite(after);
		    जर (*after == '=' || (*after != NUL && after[1] == '=')
					 || (after[0] == '.' && after[1] == '.'
							   && after[2] == '='))
			eap->cmdidx = CMD_var;
		}
		--emsg_off;
		परत eap->cmd;
	    }

	    // "[...]->Method()" is a list expression, but "[a, b] = Func()" is
	    // an assignment.
	    // If there is no line break inside the "[...]" then "p" is
	    // advanced to after the "]" by to_name_const_end(): check if a "="
	    // follows.
	    // If "[...]" has a line break "p" still points at the "[" and it
	    // can't be an assignment.
	    जर (*eap->cmd == '[')
	    {
		char_u	    *eq;

		p = to_name_const_end(eap->cmd);
		जर (p == eap->cmd && *p == '[')
		{
		    पूर्णांक मोजणी = 0;
		    पूर्णांक	semicolon = FALSE;

		    p = skip_var_list(eap->cmd, TRUE, &मोजणी, &semicolon, TRUE);
		}
		eq = p;
		जर (eq != रिक्त)
		{
		    eq = skipwhite(eq);
		    जर (vim_strchr((char_u *)"+-*/%.", *eq) != रिक्त)
		    {
			जर (eq[0] == '.' && eq[1] == '.')
			    ++eq;
			++eq;
		    }
		}
		जर (p == रिक्त || p == eap->cmd || *eq != '=')
		{
		    eap->cmdidx = CMD_eval;
		    परत eap->cmd;
		}
		जर (p > eap->cmd && *eq == '=')
		{
		    eap->cmdidx = CMD_var;
		    परत eap->cmd;
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
	    जर (oplen > 0)
	    {
		जर (((p - eap->cmd) > 2 && eap->cmd[1] == ':')
			|| *eap->cmd == '&'
			|| *eap->cmd == '$'
			|| *eap->cmd == '@'
			|| (eap->skip && IS_WHITE_OR_NUL(
						      *(skipwhite(p) + oplen)))
			|| lookup(eap->cmd, p - eap->cmd, TRUE, cctx) == OK)
		{
		    eap->cmdidx = CMD_var;
		    परत eap->cmd;
		}
	    }

	    // Recognize trying to use a type for a w:, b:, t: or g: variable:
	    // "w:varname: number = 123".
	    जर (eap->cmd[1] == ':' && *p == ':')
	    {
		eap->cmdidx = CMD_var;
		परत eap->cmd;
	    }
	}

	// 1234->func() is a method call
	जर (संख्या_method(eap->cmd))
	{
	    eap->cmdidx = CMD_eval;
	    परत eap->cmd;
	}

	// "g:", "s:" and "l:" are always assumed to be a variable, thus start
	// an expression.  A global/substitute/list command needs to use a
	// longer name.
	जर (vim_strchr((char_u *)"gsl", *p) != रिक्त && p[1] == ':')
	{
	    eap->cmdidx = CMD_eval;
	    परत eap->cmd;
	}

	// If it is an ID it might be a variable with an operator on the next
	// line, if the variable exists it can't be an Ex command.
	जर (p > eap->cmd && ends_excmd(*skipwhite(p))
		&& (lookup(eap->cmd, p - eap->cmd, TRUE, cctx) == OK
		    || (ASCII_ISALPHA(eap->cmd[0]) && eap->cmd[1] == ':')))
	{
	    eap->cmdidx = CMD_eval;
	    परत eap->cmd;
	}

	// Check for "++nr" and "--nr".
	जर (p == eap->cmd && p[0] != NUL && p[0] == p[1]
						   && (*p == '+' || *p == '-'))
	{
	    eap->cmdidx = *p == '+' ? CMD_increment : CMD_decrement;
	    परत eap->cmd + 2;
	}
    }
#endif

    /*
     * Isolate the command and search for it in the command table.
     */
    p = eap->cmd;
    जर (one_letter_cmd(p, &eap->cmdidx))
    {
	++p;
	जर (full != रिक्त)
	    *full = TRUE;
    }
    अन्यथा
    {
	पर्यंत (ASCII_ISALPHA(*p))
	    ++p;
	// for python 3.x support ":py3", ":python3", ":py3file", etc.
	जर (eap->cmd[0] == 'p' && eap->cmd[1] == 'y')
	{
	    पर्यंत (ASCII_ISALNUM(*p))
		++p;
	}
	अन्यथा जर (*p == '9' && STRNCMP("vim9", eap->cmd, 4) == 0)
	{
	    // include "9" for "vim9*" commands; "vim9cmd" and "vim9script".
	    ++p;
	    पर्यंत (ASCII_ISALPHA(*p))
		++p;
	}

	// check for non-alpha command
	जर (p == eap->cmd && vim_strchr((char_u *)"@*!=><&~#}", *p) != रिक्त)
	    ++p;
	len = (पूर्णांक)(p - eap->cmd);
	// The "d" command can directly be followed by 'l' or 'p' flag, when
	// not in Vim9 script.
	जर (!vim9 && *eap->cmd == 'd' && (p[-1] == 'l' || p[-1] == 'p'))
	{
	    // Check for ":dl", ":dell", etc. to ":deletel": that's
	    // :delete with the 'l' flag.  Same for 'p'.
	    साठी (i = 0; i < len; ++i)
		जर (eap->cmd[i] != ((char_u *)"delete")[i])
		    थांबा;
	    जर (i == len - 1)
	    {
		--len;
		जर (p[-1] == 'l')
		    eap->flags |= EXFLAG_LIST;
		अन्यथा
		    eap->flags |= EXFLAG_PRINT;
	    }
	}

	जर (ASCII_ISLOWER(eap->cmd[0]))
	{
	    पूर्णांक c1 = eap->cmd[0];
	    पूर्णांक c2 = len == 1 ? NUL : eap->cmd[1];

	    जर (command_मोजणी != (पूर्णांक)CMD_SIZE)
	    {
		iemsg(e_command_table_needs_to_be_updated_run_make_cmdidxs);
		getout(1);
	    }

	    // Use a precomputed index for fast look-up in cmdnames[]
	    // taking into account the first 2 letters of eap->cmd.
	    eap->cmdidx = cmdidxs1[Char_Ord_खालचा(c1)];
	    जर (ASCII_ISLOWER(c2))
		eap->cmdidx += cmdidxs2[Char_Ord_खालचा(c1)][Char_Ord_खालचा(c2)];
	}
	अन्यथा जर (ASCII_ISUPPER(eap->cmd[0]))
	    eap->cmdidx = CMD_Next;
	अन्यथा
	    eap->cmdidx = CMD_bang;

	साठी ( ; (पूर्णांक)eap->cmdidx < (पूर्णांक)CMD_SIZE;
			       eap->cmdidx = (cmdidx_T)((पूर्णांक)eap->cmdidx + 1))
	    जर (STRNCMP(cmdnames[(पूर्णांक)eap->cmdidx].cmd_name, (अक्षर *)eap->cmd,
							    (माप_प्रकार)len) == 0)
	    {
#ifdef FEAT_EVAL
		जर (full != रिक्त && cmdnames[eap->cmdidx].cmd_name[len] == NUL)
		    *full = TRUE;
#endif
		थांबा;
	    }

	// :Print and :mode are not supported in Vim9 script.
	// Some commands cannot be shortened in Vim9 script.
	जर (vim9 && eap->cmdidx != CMD_SIZE)
	{
	    जर (eap->cmdidx == CMD_mode || eap->cmdidx == CMD_Print)
		eap->cmdidx = CMD_SIZE;
	    अन्यथा जर ((cmdnames[eap->cmdidx].cmd_argt & EX_WHOLE)
			  && len < (पूर्णांक)cmdnames[eap->cmdidx].cmd_namelen)
	    {
		semsg(_(e_command_cannot_be_shortened_str), eap->cmd);
		eap->cmdidx = CMD_SIZE;
	    }
	}

	// Do not recognize ":*" as the star command unless '*' is in
	// 'cpoptions'.
	जर (eap->cmdidx == CMD_star && vim_strchr(p_cpo, CPO_STAR) == रिक्त)
	    p = eap->cmd;

	// Look for a user defined command as a last resort.  Let ":Print" be
	// overruled by a user defined command.
	जर ((eap->cmdidx == CMD_SIZE || eap->cmdidx == CMD_Print)
		&& *eap->cmd >= 'A' && *eap->cmd <= 'Z')
	{
	    // User defined commands may contain digits.
	    पर्यंत (ASCII_ISALNUM(*p))
		++p;
	    p = find_ucmd(eap, p, full, रिक्त, रिक्त);
	}
	जर (p == रिक्त || p == eap->cmd)
	    eap->cmdidx = CMD_SIZE;
    }

    // ":fina" means ":finally" in legacy script, for backwards compatibility.
    जर (eap->cmdidx == CMD_final && p - eap->cmd == 4 && !vim9)
	eap->cmdidx = CMD_finally;

     // Force ":ho" to be unresolved.  Without this, find_ex_command()
     // matches it to CMD_horizontal (the only "ho*" entry), which makes
     // fullcommand("ho") return "horizontal" even though ":ho" cannot be
     // used as the modifier (cmdmods[] requires 3 chars, "hor").
    जर (eap->cmdidx == CMD_horizontal && p - eap->cmd == 2)
	eap->cmdidx = CMD_SIZE;

#ifdef FEAT_EVAL
    जर (eap->cmdidx < CMD_SIZE
	    && vim9
	    && !IS_WHITE_NL_OR_NUL(*p) && *p != '!' && *p != '|'
	    && (eap->cmdidx < 0 ||
		(cmdnames[eap->cmdidx].cmd_argt & EX_NONWHITE_OK) == 0))
    {
	// A name that goes on with an underscore is not this command with an
	// argument: "ch_log" is not ":change".  Leave it to be reported as an
	// invalid command.
	जर (*p != '_')
	{
	    char_u *cmd = vim_strnsave(eap->cmd, p - eap->cmd);

	    semsg(_(e_command_str_not_followed_by_white_space_str), cmd,
								    eap->cmd);
	    vim_free(cmd);
	}
	eap->cmdidx = CMD_SIZE;
    }
#endif

    परत p;
}

#जर defined(FEAT_EVAL)
प्रकारव्याख्या संरचना
{
    अक्षर	*name;
    पूर्णांक		minlen;
    पूर्णांक		has_मोजणी;  // :123verbose  :3tab
} cmdmod_info_T;

स्थिर cmdmod_info_T cmdmod_info_tab[] = {
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
    पूर्णांक
modifier_len(char_u *cmd)
{
    पूर्णांक		i, j;
    char_u	*p = cmd;

    जर (VIM_ISDIGIT(*cmd))
	p = skipwhite(skipdigits(cmd + 1));
    साठी (i = 0; i < (पूर्णांक)ARRAY_LENGTH(cmdmod_info_tab); ++i)
    {
	// cmdmod_info_tab[] is sorted by name: once the first letter is past
	// the command's first letter no later entry can match.
	जर (cmdmod_info_tab[i].name[0] > *p)
	    थांबा;
	साठी (j = 0; p[j] != NUL; ++j)
	    जर (p[j] != cmdmod_info_tab[i].name[j])
		थांबा;
	जर (!ASCII_ISALPHA(p[j]) && j >= cmdmod_info_tab[i].minlen
					&& (p == cmd || cmdmod_info_tab[i].has_मोजणी))
	    परत j + (पूर्णांक)(p - cmd);
    }
    परत 0;
}

/*
 * Return > 0 if an Ex command "name" exists.
 * Return 2 if there is an exact match.
 * Return 3 if there is an ambiguous match.
 */
    पूर्णांक
cmd_exists(char_u *name)
{
    exarg_T	ea;
    पूर्णांक		full = FALSE;
    पूर्णांक		i;
    पूर्णांक		j;
    char_u	*p;

    // Check command modifiers.
    साठी (i = 0; i < (पूर्णांक)ARRAY_LENGTH(cmdmod_info_tab); ++i)
    {
	साठी (j = 0; name[j] != NUL; ++j)
	    जर (name[j] != cmdmod_info_tab[i].name[j])
		थांबा;
	जर (name[j] == NUL && j >= cmdmod_info_tab[i].minlen)
	    परत (cmdmod_info_tab[i].name[j] == NUL ? 2 : 1);
    }

    // Check built-in commands and user defined commands.
    // For ":2match" and ":3match" we need to skip the number.
    ea.cmd = (*name == '2' || *name == '3') ? name + 1 : name;
    ea.cmdidx = (cmdidx_T)0;
    ea.flags = 0;
    p = find_ex_command(&ea, &full, रिक्त, रिक्त);
    जर (p == रिक्त)
	परत 3;
    जर (vim_isdigit(*name) && ea.cmdidx != CMD_match)
	परत 0;
    जर (*skipwhite(p) != NUL)
	परत 0;	// trailing garbage
    परत (ea.cmdidx == CMD_SIZE ? 0 : (full ? 2 : 1));
}

/*
 * Add the attributes of an Ex command to "d" in the terms of ":command":
 * "argt" are the EX_ flags, "addr_type" the address type and "def" the
 * default for a range or count, -1 when there is none.
 */
    शून्य
ex_command_attrs(dict_T *d, long_u argt, cmd_addr_T addr_type, दीर्घ def)
{
    स्थिर अक्षर *addr_names[] = {
	"lines", "windows", "arguments", "loaded_buffers", "buffers", "tabs",
	"tabs_relative", "quickfix_valid", "quickfix", "unsigned", "other",
	"none"
    };
    अक्षर	*nargs;

    जर (!(argt & EX_EXTRA))
	nargs = "0";
    अन्यथा जर (!(argt & EX_NOSPC))
	nargs = (argt & EX_NEEDARG) ? "+" : "*";
    अन्यथा जर (!(argt & EX_NEEDARG))
	nargs = "?";
    अन्यथा
	nargs = (argt & EX_ARGSPACE) ? "_" : "1";
    dict_add_string(d, "nargs", (char_u *)nargs);

    जर (!(argt & EX_RANGE))
	dict_add_bool(d, "range", FALSE);
    अन्यथा जर (argt & EX_DFLALL)
	dict_add_string(d, "range", (char_u *)"%");
    अन्यथा जर (def >= 0 && !(argt & EX_मोजणी))
	dict_add_संख्या(d, "range", def);
    अन्यथा
	dict_add_string(d, "range", (char_u *)".");
    जर (!(argt & EX_मोजणी))
	dict_add_bool(d, "count", FALSE);
    अन्यथा
	dict_add_संख्या(d, "count", def < 0 ? 0 : def);
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
    पूर्णांक
ex_command_info(char_u *name, पूर्णांक vim9, dict_T *d)
{
    exarg_T	ea;
    char_u	*p;
    पूर्णांक		i;
    पूर्णांक		j;
    पूर्णांक		save_cmod_flags = cmdmod.cmod_flags;
    पूर्णांक		ret = OK;

    जर (vim9 != -1)
    {
	cmdmod.cmod_flags &= ~(CMOD_VIM9CMD | CMOD_LEGACY);
	cmdmod.cmod_flags |= vim9 ? CMOD_VIM9CMD : CMOD_LEGACY;
    }

    // An abbreviation of a command modifier, like ":bo" for ":botright".
    साठी (i = 0; i < (पूर्णांक)ARRAY_LENGTH(cmdmod_info_tab); ++i)
    {
	साठी (j = 0; name[j] != NUL; ++j)
	    जर (name[j] != cmdmod_info_tab[i].name[j])
		थांबा;
	जर (name[j] == NUL && j >= cmdmod_info_tab[i].minlen)
	{
	    name = (char_u *)cmdmod_info_tab[i].name;
	    थांबा;
	}
    }

    // For ":2match" and ":3match" we need to skip the number.
    ea.cmd = (*name == '2' || *name == '3') ? name + 1 : name;
    ea.cmdidx = (cmdidx_T)0;
    ea.flags = 0;
    ea.addr_मोजणी = 0;
    // Don't complain about using "en" or ":let" in Vim9 script.
    ++emsg_silent;
    p = find_ex_command(&ea, रिक्त, रिक्त, रिक्त);
    जर (p == रिक्त || ea.cmdidx == CMD_SIZE
	    || (vim_isdigit(*name) && ea.cmdidx != CMD_match)
	    || *skipwhite(p) != NUL
	    || not_in_vim9(&ea, true) == FAIL)
	ret = FAIL;
    --emsg_silent;

    जर (ret == FAIL)
	;
    अन्यथा जर (IS_USER_CMDIDX(ea.cmdidx))
	ret = user_command_info(ea.useridx, ea.cmdidx, d);
    अन्यथा
    {
	पूर्णांक	notimpl = cmdnames[ea.cmdidx].cmd_func == ex_ni
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
    परत ret;
}

/*
 * "fullcommand" function
 */
    शून्य
f_fullcommand(typval_T *argvars, typval_T *rettv)
{
    exarg_T	ea;
    char_u	*name;
    char_u	*p;
    पूर्णांक		vim9script = in_vim9script();
    पूर्णांक		save_cmod_flags = cmdmod.cmod_flags;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = रिक्त;

    जर (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL))
	परत;

    name = tv_get_string(&argvars[0]);
    जर (name == रिक्त)
	परत;

    जर (argvars[1].v_type != VAR_UNKNOWN)
    {
	vim9script = tv_get_bool(&argvars[1]);
	cmdmod.cmod_flags &= ~(CMOD_VIM9CMD | CMOD_LEGACY);
	cmdmod.cmod_flags |= vim9script ? CMOD_VIM9CMD : CMOD_LEGACY;
    }

    पर्यंत (*name == ':')
	name++;
    name = skip_range(name, TRUE, रिक्त);

    ea.cmd = (*name == '2' || *name == '3') ? name + 1 : name;
    ea.cmdidx = (cmdidx_T)0;
    ea.addr_मोजणी = 0;
    ++emsg_silent;  // don't complain about using "en" in Vim9 script
    p = find_ex_command(&ea, रिक्त, रिक्त, रिक्त);
    --emsg_silent;
    जर (p == रिक्त || ea.cmdidx == CMD_SIZE)
	जा theend;

    जर (vim9script && not_in_vim9(&ea, true) == FAIL)
	जा theend;

    rettv->vval.v_string = vim_strsave(IS_USER_CMDIDX(ea.cmdidx)
				 ? get_user_command_name(ea.useridx, ea.cmdidx)
				 : cmdnames[ea.cmdidx].cmd_name);
theend:
    cmdmod.cmod_flags = save_cmod_flags;
}
#endif

    cmdidx_T
excmd_get_cmdidx(char_u *cmd, पूर्णांक len)
{
    cmdidx_T idx;

    जर (!one_letter_cmd(cmd, &idx))
	साठी (idx = (cmdidx_T)0; (पूर्णांक)idx < (पूर्णांक)CMD_SIZE;
		idx = (cmdidx_T)((पूर्णांक)idx + 1))
	    जर (STRNCMP(cmdnames[(पूर्णांक)idx].cmd_name, cmd, (माप_प्रकार)len) == 0)
		थांबा;

    परत idx;
}

    दीर्घ
excmd_get_argt(cmdidx_T idx)
{
    परत (दीर्घ)cmdnames[(पूर्णांक)idx].cmd_argt;
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
    पूर्णांक		skip_star,	// skip "*" used for Visual range
    पूर्णांक		*ctx)		// pointer to xp_context or NULL
{
    char_u	*cmd = cmd_start;
    अचिन्हित	delim;

    पर्यंत (vim_strchr((char_u *)" \t0123456789.$%'/?-+,;\\", *cmd) != रिक्त)
    {
	जर (*cmd == '\\')
	{
	    जर (cmd[1] == '?' || cmd[1] == '/' || cmd[1] == '&')
		++cmd;
	    अन्यथा
		थांबा;
	}
	अन्यथा जर (*cmd == '\'')
	{
	    char_u *p = cmd;

	    // a quote is only valid at the start or after a separator
	    पर्यंत (p > cmd_start)
	    {
		--p;
		जर (!VIM_ISWHITE(*p))
		    थांबा;
	    }
	    जर (cmd > cmd_start && !VIM_ISWHITE(*p) && *p != ',' && *p != ';')
		थांबा;
	    जर (*++cmd == NUL && ctx != रिक्त)
		*ctx = EXPAND_NOTHING;
	}
	अन्यथा जर (*cmd == '/' || *cmd == '?')
	{
	    delim = *cmd++;
	    पर्यंत (*cmd != NUL && *cmd != delim)
		जर (*cmd++ == '\\' && *cmd != NUL)
		    ++cmd;
	    जर (*cmd == NUL && ctx != रिक्त)
		*ctx = EXPAND_NOTHING;
	}
	जर (*cmd != NUL)
	    ++cmd;
    }

    // Skip ":" and white space.
    पर्यंत (*cmd == ':')
	cmd = skipwhite(cmd + 1);

    // Skip "*" used for Visual range.
    जर (skip_star && *cmd == '*' && vim_strchr(p_cpo, CPO_STAR) == रिक्त)
	cmd = skipwhite(cmd + 1);

    परत cmd;
}

    स्थिर शून्य
addr_error(cmd_addr_T addr_type)
{
    जर (addr_type == ADDR_NONE)
	emsg(_(e_no_range_allowed));
    अन्यथा
	emsg(_(e_invalid_range));
}

/*
 * Return the default address for an address type.
 */
    स्थिर linenr_T
default_address(exarg_T *eap)
{
    linenr_T lnum = 0;

    निवड (eap->addr_type)
    {
	स्थिती ADDR_ओळी:
	स्थिती ADDR_OTHER:
	    // Default is the cursor line number.  Avoid using an invalid
	    // line number though.
	    जर (curwin->w_cursor.lnum > curbuf->b_ml.ml_ओळ_मोजणी)
		lnum = curbuf->b_ml.ml_ओळ_मोजणी;
	    अन्यथा
		lnum = curwin->w_cursor.lnum;
	    थांबा;
	स्थिती ADDR_WINDOWS:
	    lnum = CURRENT_WIN_NR;
	    थांबा;
	स्थिती ADDR_ARGUMENTS:
	    lnum = curwin->w_arg_idx + 1;
	    जर (lnum > ARGCOUNT)
		lnum = ARGCOUNT;
	    थांबा;
	स्थिती ADDR_LOADED_BUFFERS:
	स्थिती ADDR_BUFFERS:
	    lnum = curbuf->b_fnum;
	    थांबा;
	स्थिती ADDR_TABS:
	    lnum = CURRENT_TAB_NR;
	    थांबा;
	स्थिती ADDR_TABS_RELATIVE:
	स्थिती ADDR_UNSIGNED:
	    lnum = 1;
	    थांबा;
	स्थिती ADDR_QUICKFIX:
#ifdef FEAT_QUICKFIX
	    lnum = qf_get_cur_idx(eap);
#endif
	    थांबा;
	स्थिती ADDR_QUICKFIX_VALID:
#ifdef FEAT_QUICKFIX
	    lnum = qf_get_cur_valid_idx(eap);
#endif
	    थांबा;
	स्थिती ADDR_NONE:
	    // Will give an error later if a range is found.
	    थांबा;
    }
    परत lnum;
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
    पूर्णांक		skip,		// only skip the address, don't use it
    पूर्णांक		silent,		// no errors or side effects
    पूर्णांक		to_other_file,  // flag: may jump to other file
    पूर्णांक		address_मोजणी UNUSED) // 1 for first address, >1 after comma
{
    पूर्णांक		c;
    पूर्णांक		i;
    दीर्घ	n;
    char_u	*cmd;
    pos_T	pos;
    pos_T	*fp;
    linenr_T	lnum;
    buf_T	*buf;

    cmd = skipwhite(*ptr);
    lnum = MAXLNUM;
    करा
    {
	निवड (*cmd)
	{
	    स्थिती '.':			    // '.' - Cursor position
		++cmd;
		निवड (addr_type)
		{
		    स्थिती ADDR_ओळी:
		    स्थिती ADDR_OTHER:
			lnum = curwin->w_cursor.lnum;
			थांबा;
		    स्थिती ADDR_WINDOWS:
			lnum = CURRENT_WIN_NR;
			थांबा;
		    स्थिती ADDR_ARGUMENTS:
			lnum = curwin->w_arg_idx + 1;
			थांबा;
		    स्थिती ADDR_LOADED_BUFFERS:
		    स्थिती ADDR_BUFFERS:
			lnum = curbuf->b_fnum;
			थांबा;
		    स्थिती ADDR_TABS:
			lnum = CURRENT_TAB_NR;
			थांबा;
		    स्थिती ADDR_NONE:
		    स्थिती ADDR_TABS_RELATIVE:
		    स्थिती ADDR_UNSIGNED:
			addr_error(addr_type);
			cmd = रिक्त;
			जा error;
			थांबा;
		    स्थिती ADDR_QUICKFIX:
#ifdef FEAT_QUICKFIX
			lnum = qf_get_cur_idx(eap);
#endif
			थांबा;
		    स्थिती ADDR_QUICKFIX_VALID:
#ifdef FEAT_QUICKFIX
			lnum = qf_get_cur_valid_idx(eap);
#endif
			थांबा;
		}
		थांबा;

	    स्थिती '$':			    // '$' - last line
		++cmd;
		निवड (addr_type)
		{
		    स्थिती ADDR_ओळी:
		    स्थिती ADDR_OTHER:
			lnum = curbuf->b_ml.ml_ओळ_मोजणी;
			थांबा;
		    स्थिती ADDR_WINDOWS:
			lnum = LAST_WIN_NR;
			थांबा;
		    स्थिती ADDR_ARGUMENTS:
			lnum = ARGCOUNT;
			थांबा;
		    स्थिती ADDR_LOADED_BUFFERS:
			buf = lastbuf;
			पर्यंत (buf->b_ml.ml_mfp == रिक्त)
			{
			    जर (buf->b_prev == रिक्त)
				थांबा;
			    buf = buf->b_prev;
			}
			lnum = buf->b_fnum;
			थांबा;
		    स्थिती ADDR_BUFFERS:
			lnum = lastbuf->b_fnum;
			थांबा;
		    स्थिती ADDR_TABS:
			lnum = LAST_TAB_NR;
			थांबा;
		    स्थिती ADDR_NONE:
		    स्थिती ADDR_TABS_RELATIVE:
		    स्थिती ADDR_UNSIGNED:
			addr_error(addr_type);
			cmd = रिक्त;
			जा error;
			थांबा;
		    स्थिती ADDR_QUICKFIX:
#ifdef FEAT_QUICKFIX
			lnum = qf_get_size(eap);
			जर (lnum == 0)
			    lnum = 1;
#endif
			थांबा;
		    स्थिती ADDR_QUICKFIX_VALID:
#ifdef FEAT_QUICKFIX
			lnum = qf_get_valid_size(eap);
			जर (lnum == 0)
			    lnum = 1;
#endif
			थांबा;
		}
		थांबा;

	    स्थिती '\'':			    // ''' - mark
		जर (*++cmd == NUL)
		{
		    cmd = रिक्त;
		    जा error;
		}
		जर (addr_type != ADDR_ओळी)
		{
		    addr_error(addr_type);
		    cmd = रिक्त;
		    जा error;
		}
		जर (skip)
		    ++cmd;
		अन्यथा
		{
		    // Only accept a mark in another file when it is
		    // used by itself: ":'M".
		    fp = getmark(*cmd, to_other_file && cmd[1] == NUL);
		    ++cmd;
		    जर (fp == (pos_T *)-1)
			// Jumped to another file.
			lnum = curwin->w_cursor.lnum;
		    अन्यथा
		    {
			जर (check_mark(fp) == FAIL)
			{
			    cmd = रिक्त;
			    जा error;
			}
			lnum = fp->lnum;
		    }
		}
		थांबा;

	    स्थिती '/':
	    स्थिती '?':			// '/' or '?' - search
		c = *cmd++;
		जर (addr_type != ADDR_ओळी)
		{
		    addr_error(addr_type);
		    cmd = रिक्त;
		    जा error;
		}
		जर (skip)	// skip "/pat/"
		{
		    cmd = skip_regexp(cmd, c, magic_isset());
		    जर (*cmd == c)
			++cmd;
		}
		अन्यथा
		{
		    पूर्णांक flags;

		    pos = curwin->w_cursor; // save curwin->w_cursor

		    // When '/' or '?' follows another address, start from
		    // there.
		    जर (lnum > 0 && lnum != MAXLNUM)
			curwin->w_cursor.lnum =
				lnum > curbuf->b_ml.ml_ओळ_मोजणी
					   ? curbuf->b_ml.ml_ओळ_मोजणी : lnum;

		    // Start a forward search at the end of the line (unless
		    // before the first line).
		    // Start a backward search at the start of the line.
		    // This makes sure we never match in the current
		    // line, and can match anywhere in the
		    // next/previous line.
		    जर (c == '/' && curwin->w_cursor.lnum > 0)
			curwin->w_cursor.col = MAXCOL;
		    अन्यथा
			curwin->w_cursor.col = 0;
		    searchcmdlen = 0;
		    flags = silent ? शोध_KEEP : शोध_HIS | शोध_संदेश;
		    जर (!do_शोध(रिक्त, c, c, cmd, STRLEN(cmd), 1L, flags, रिक्त))
		    {
			curwin->w_cursor = pos;
			cmd = रिक्त;
			जा error;
		    }
		    lnum = curwin->w_cursor.lnum;
		    curwin->w_cursor = pos;
		    // adjust command string pointer
		    cmd += searchcmdlen;
		}
		थांबा;

	    स्थिती '\\':		    // "\?", "\/" or "\&", repeat search
		++cmd;
		जर (addr_type != ADDR_ओळी)
		{
		    addr_error(addr_type);
		    cmd = रिक्त;
		    जा error;
		}
		जर (*cmd == '&')
		    i = RE_SUBST;
		अन्यथा जर (*cmd == '?' || *cmd == '/')
		    i = RE_शोध;
		अन्यथा
		{
		    emsg(_(e_backslash_should_be_followed_by));
		    cmd = रिक्त;
		    जा error;
		}

		जर (!skip)
		{
		    /*
		     * When search follows another address, start from
		     * there.
		     */
		    जर (lnum != MAXLNUM)
			pos.lnum = lnum;
		    अन्यथा
			pos.lnum = curwin->w_cursor.lnum;

		    /*
		     * Start the search just like for the above
		     * do_search().
		     */
		    जर (*cmd != '?')
			pos.col = MAXCOL;
		    अन्यथा
			pos.col = 0;
		    pos.coladd = 0;
		    जर (searchit(curwin, curbuf, &pos, रिक्त,
				*cmd == '?' ? BACKWARD : FORWARD,
				(char_u *)"", 0, 1L, शोध_संदेश, i, रिक्त) != FAIL)
			lnum = pos.lnum;
		    अन्यथा
		    {
			cmd = रिक्त;
			जा error;
		    }
		}
		++cmd;
		थांबा;

	    पूर्वनिर्धारित:
		जर (VIM_ISDIGIT(*cmd))	// absolute line number
		    lnum = getdigits(&cmd);
	}

	साठी (;;)
	{
	    cmd = skipwhite(cmd);
	    जर (*cmd != '-' && *cmd != '+' && !VIM_ISDIGIT(*cmd))
		थांबा;

	    जर (lnum == MAXLNUM)
	    {
		निवड (addr_type)
		{
		    स्थिती ADDR_ओळी:
		    स्थिती ADDR_OTHER:
			// "+1" is same as ".+1"
			lnum = curwin->w_cursor.lnum;
			थांबा;
		    स्थिती ADDR_WINDOWS:
			lnum = CURRENT_WIN_NR;
			थांबा;
		    स्थिती ADDR_ARGUMENTS:
			lnum = curwin->w_arg_idx + 1;
			थांबा;
		    स्थिती ADDR_LOADED_BUFFERS:
		    स्थिती ADDR_BUFFERS:
			lnum = curbuf->b_fnum;
			थांबा;
		    स्थिती ADDR_TABS:
			lnum = CURRENT_TAB_NR;
			थांबा;
		    स्थिती ADDR_TABS_RELATIVE:
			lnum = 1;
			थांबा;
		    स्थिती ADDR_QUICKFIX:
#ifdef FEAT_QUICKFIX
			lnum = qf_get_cur_idx(eap);
#endif
			थांबा;
		    स्थिती ADDR_QUICKFIX_VALID:
#ifdef FEAT_QUICKFIX
			lnum = qf_get_cur_valid_idx(eap);
#endif
			थांबा;
		    स्थिती ADDR_NONE:
		    स्थिती ADDR_UNSIGNED:
			lnum = 0;
			थांबा;
		}
	    }

	    जर (VIM_ISDIGIT(*cmd))
		i = '+';		// "number" is same as "+number"
	    अन्यथा
		i = *cmd++;
	    जर (!VIM_ISDIGIT(*cmd))	// '+' is '+1'
		n = 1;
	    अन्यथा
	    {
		// "number", "+number" or "-number"
		n = getdigits(&cmd);
		जर (n == MAXLNUM)
		{
		    emsg(_(e_ओळ_संख्या_out_of_range));
		    cmd = रिक्त;
		    जा error;
		}
	    }

	    जर (addr_type == ADDR_TABS_RELATIVE)
	    {
		emsg(_(e_invalid_range));
		cmd = रिक्त;
		जा error;
	    }
	    अन्यथा जर (addr_type == ADDR_LOADED_BUFFERS
		    || addr_type == ADDR_BUFFERS)
		lnum = compute_buffer_local_मोजणी(
				    addr_type, lnum, (i == '-') ? -1 * n : n);
	    अन्यथा
	    {
#ifdef FEAT_FOLDING
		// Relative line addressing: need to adjust for lines in a
		// closed fold after the first address.
		जर (addr_type == ADDR_ओळी && (i == '-' || i == '+')
							 && address_मोजणी >= 2)
		    (शून्य)hasFolding(lnum, रिक्त, &lnum);
#endif
		जर (i == '-')
		    lnum -= n;
		अन्यथा
		{
		    जर (lnum >= 0 && n >= दीर्घ_उच्च - lnum)
		    {
			emsg(_(e_ओळ_संख्या_out_of_range));
			cmd = रिक्त;
			जा error;
		    }
		    lnum += n;
		}
	    }
	}
    } पर्यंत (*cmd == '/' || *cmd == '?');

error:
    *ptr = cmd;
    परत lnum;
}

/*
 * Set eap->line1 and eap->line2 to the whole range.
 * Used for commands with the EX_DFLALL flag and no range given.
 */
    स्थिर शून्य
address_default_all(exarg_T *eap)
{
    eap->line1 = 1;
    निवड (eap->addr_type)
    {
	स्थिती ADDR_ओळी:
	स्थिती ADDR_OTHER:
	    eap->line2 = curbuf->b_ml.ml_ओळ_मोजणी;
	    थांबा;
	स्थिती ADDR_LOADED_BUFFERS:
	    {
		buf_T *buf = firstbuf;

		पर्यंत (buf->b_next != रिक्त && buf->b_ml.ml_mfp == रिक्त)
		    buf = buf->b_next;
		eap->line1 = buf->b_fnum;
		buf = lastbuf;
		पर्यंत (buf->b_prev != रिक्त && buf->b_ml.ml_mfp == रिक्त)
		    buf = buf->b_prev;
		eap->line2 = buf->b_fnum;
	    }
	    थांबा;
	स्थिती ADDR_BUFFERS:
	    eap->line1 = firstbuf->b_fnum;
	    eap->line2 = lastbuf->b_fnum;
	    थांबा;
	स्थिती ADDR_WINDOWS:
	    eap->line2 = LAST_WIN_NR;
	    थांबा;
	स्थिती ADDR_TABS:
	    eap->line2 = LAST_TAB_NR;
	    थांबा;
	स्थिती ADDR_TABS_RELATIVE:
	    eap->line2 = 1;
	    थांबा;
	स्थिती ADDR_ARGUMENTS:
	    जर (ARGCOUNT == 0)
		eap->line1 = eap->line2 = 0;
	    अन्यथा
		eap->line2 = ARGCOUNT;
	    थांबा;
	स्थिती ADDR_QUICKFIX_VALID:
#ifdef FEAT_QUICKFIX
	    eap->line2 = qf_get_valid_size(eap);
	    जर (eap->line2 == 0)
		eap->line2 = 1;
#endif
	    थांबा;
	स्थिती ADDR_NONE:
	स्थिती ADDR_UNSIGNED:
	स्थिती ADDR_QUICKFIX:
	    iemsg("Cannot use EX_DFLALL with ADDR_NONE, ADDR_UNSIGNED or ADDR_QUICKFIX");
	    थांबा;
    }
}


/*
 * Get flags from an Ex command argument.
 */
    स्थिर शून्य
get_flags(exarg_T *eap)
{
    पर्यंत (vim_strchr((char_u *)"lp#", *eap->arg) != रिक्त)
    {
	जर (*eap->arg == 'l')
	    eap->flags |= EXFLAG_LIST;
	अन्यथा जर (*eap->arg == 'p')
	    eap->flags |= EXFLAG_PRINT;
	अन्यथा
	    eap->flags |= EXFLAG_NR;
	eap->arg = skipwhite(eap->arg + 1);
    }
}

/*
 * Function called for command which is Not Implemented.  NI!
 */
    शून्य
ex_ni(exarg_T *eap)
{
    जर (!eap->skip)
	eap->errmsg =
		_(e_sorry_command_is_not_available_in_this_version);
}

#ifdef HAVE_EX_SCRIPT_NI
/*
 * Function called for script command which is Not Implemented.  NI!
 * Skips over ":perl <<EOF" constructs.
 */
    स्थिर शून्य
ex_script_ni(exarg_T *eap)
{
    जर (!eap->skip)
	ex_ni(eap);
    अन्यथा
	vim_free(script_get(eap, eap->arg));
}
#endif

/*
 * Check range in Ex command for validity.
 * Return NULL when valid, error message when invalid.
 */
    स्थिर अक्षर *
invalid_range(exarg_T *eap)
{
    buf_T	*buf;

    जर (       eap->line1 < 0
	    || eap->line2 < 0
	    || eap->line1 > eap->line2)
	परत _(e_invalid_range);

    जर (eap->argt & EX_RANGE)
    {
	निवड (eap->addr_type)
	{
	    स्थिती ADDR_ओळी:
		जर (eap->line2 > curbuf->b_ml.ml_ओळ_मोजणी
#ifdef FEAT_DIFF
			    + (eap->cmdidx == CMD_diffget ||
				eap->cmdidx == CMD_diffput)
#endif
		   )
		    परत _(e_invalid_range);
		थांबा;
	    स्थिती ADDR_ARGUMENTS:
		// add 1 if ARGCOUNT is 0
		जर (eap->line2 > ARGCOUNT + (!ARGCOUNT))
		    परत _(e_invalid_range);
		थांबा;
	    स्थिती ADDR_BUFFERS:
		// Only a boundary check, not whether the buffers actually
		// exist.
		जर (eap->line1 < 1 || eap->line2 > get_highest_fnum())
		    परत _(e_invalid_range);
		थांबा;
	    स्थिती ADDR_LOADED_BUFFERS:
		buf = firstbuf;
		पर्यंत (buf->b_ml.ml_mfp == रिक्त)
		{
		    जर (buf->b_next == रिक्त)
			परत _(e_invalid_range);
		    buf = buf->b_next;
		}
		जर (eap->line1 < buf->b_fnum)
		    परत _(e_invalid_range);
		buf = lastbuf;
		पर्यंत (buf->b_ml.ml_mfp == रिक्त)
		{
		    जर (buf->b_prev == रिक्त)
			परत _(e_invalid_range);
		    buf = buf->b_prev;
		}
		जर (eap->line2 > buf->b_fnum)
		    परत _(e_invalid_range);
		थांबा;
	    स्थिती ADDR_WINDOWS:
		जर (eap->line2 > LAST_WIN_NR)
		    परत _(e_invalid_range);
		थांबा;
	    स्थिती ADDR_TABS:
		जर (eap->line2 > LAST_TAB_NR)
		    परत _(e_invalid_range);
		थांबा;
	    स्थिती ADDR_TABS_RELATIVE:
	    स्थिती ADDR_OTHER:
		// Any range is OK.
		थांबा;
	    स्थिती ADDR_QUICKFIX:
#ifdef FEAT_QUICKFIX
		// No error for value that is too big, will use the last entry.
		जर (eap->line2 <= 0)
		{
		    जर (eap->addr_मोजणी == 0)
			परत _(e_no_errors);
		    परत _(e_invalid_range);
		}
#endif
		थांबा;
	    स्थिती ADDR_QUICKFIX_VALID:
#ifdef FEAT_QUICKFIX
		जर ((eap->line2 != 1 && eap->line2 > qf_get_valid_size(eap))
			|| eap->line2 < 0)
		    परत _(e_invalid_range);
#endif
		थांबा;
	    स्थिती ADDR_UNSIGNED:
	    स्थिती ADDR_NONE:
		// Will give an error elsewhere.
		थांबा;
	}
    }
    परत रिक्त;
}

/*
 * Correct the range for zero line number, if required.
 */
    स्थिर शून्य
correct_range(exarg_T *eap)
{
    जर (!(eap->argt & EX_ZEROR))	    // zero in range not allowed
    {
	जर (eap->line1 == 0)
	    eap->line1 = 1;
	जर (eap->line2 == 0)
	    eap->line2 = 1;
    }
}

#ifdef FEAT_QUICKFIX
/*
 * For a ":vimgrep" or ":vimgrepadd" command return a pointer past the
 * pattern.  Otherwise return eap->arg.
 */
    स्थिर char_u *
skip_grep_pat(exarg_T *eap)
{
    char_u	*p = eap->arg;

    जर (*p != NUL && (eap->cmdidx == CMD_vimgrep || eap->cmdidx == CMD_lvimgrep
		|| eap->cmdidx == CMD_vimgrepadd
		|| eap->cmdidx == CMD_lvimgrepadd
		|| grep_internal(eap->cmdidx)))
    {
	p = skip_vimgrep_pat(p, रिक्त, रिक्त);
	जर (p == रिक्त)
	    p = eap->arg;
    }
    परत p;
}

/*
 * For the ":make" and ":grep" commands insert the 'makeprg'/'grepprg' option
 * in the command line, so that things like % get expanded.
 */
    स्थिर char_u *
replace_makeprg(exarg_T *eap, char_u *p, char_u **cmdlinep)
{
    char_u	*new_cmdline;
    char_u	*program;
    char_u	*pos;
    char_u	*ptr;
    पूर्णांक		len;
    पूर्णांक		i;

    /*
     * Don't do it when ":vimgrep" is used for ":grep".
     */
    जर ((eap->cmdidx == CMD_make || eap->cmdidx == CMD_lmake
		     || eap->cmdidx == CMD_grep || eap->cmdidx == CMD_lgrep
		     || eap->cmdidx == CMD_grepadd
		     || eap->cmdidx == CMD_lgrepadd)
	    && !grep_internal(eap->cmdidx))
    {
	जर (eap->cmdidx == CMD_grep || eap->cmdidx == CMD_lgrep
	    || eap->cmdidx == CMD_grepadd || eap->cmdidx == CMD_lgrepadd)
	{
	    जर (*curbuf->b_p_gp == NUL)
		program = p_gp;
	    अन्यथा
		program = curbuf->b_p_gp;
	}
	अन्यथा
	{
	    जर (*curbuf->b_p_mp == NUL)
		program = p_mp;
	    अन्यथा
		program = curbuf->b_p_mp;
	}

	p = skipwhite(p);

	जर ((pos = (char_u *)म_माला((अक्षर *)program, "$*")) != रिक्त)
	{
	    // replace $* by given arguments
	    i = 1;
	    पर्यंत ((pos = (char_u *)म_माला((अक्षर *)pos + 2, "$*")) != रिक्त)
		++i;
	    len = (पूर्णांक)STRLEN(p);
	    new_cmdline = alloc(STRLEN(program) + (माप_प्रकार)i * (len - 2) + 1);
	    जर (new_cmdline == रिक्त)
		परत रिक्त;			// out of memory
	    ptr = new_cmdline;
	    पर्यंत ((pos = (char_u *)म_माला((अक्षर *)program, "$*")) != रिक्त)
	    {
		i = (पूर्णांक)(pos - program);
		STRNCPY(ptr, program, i);
		STRCPY(ptr += i, p);
		ptr += len;
		program = pos + 2;
	    }
	    STRCPY(ptr, program);
	}
	अन्यथा
	{
	    माप_प्रकार program_len = STRLEN(program);

	    new_cmdline = alloc(program_len + STRLEN(p) + 2);
	    जर (new_cmdline == रिक्त)
		परत रिक्त;			// out of memory
	    STRCPY(new_cmdline, program);
	    STRCPY(new_cmdline + program_len, " ");
	    STRCPY(new_cmdline + program_len + 1, p);
	}
	संदेश_make(p);

	// 'eap->cmd' is not set here, because it is not used at CMD_make
	vim_free(*cmdlinep);
	*cmdlinep = new_cmdline;
	p = new_cmdline;
    }
    परत p;
}
#endif

/*
 * Expand file name in Ex command argument.
 * When an error is detected, "errormsgp" is set to a non-NULL pointer.
 * Return FAIL for failure, OK otherwise.
 */
    पूर्णांक
expand_filename(
    exarg_T	*eap,
    char_u	**cmdlinep,
    अक्षर	**errormsgp)
{
    पूर्णांक		has_wildcards;	// need to expand wildcards
    char_u	*repl;
    माप_प्रकार	srclen;
    char_u	*p;
    पूर्णांक		n;
    पूर्णांक		escaped;

#ifdef FEAT_QUICKFIX
    // Skip a regexp pattern for ":vimgrep[add] pat file..."
    p = skip_grep_pat(eap);
#अन्यथा
    p = eap->arg;
#endif

    /*
     * Decide to expand wildcards *before* replacing '%', '#', etc.  If
     * the file name contains a wildcard it should not cause expanding.
     * (it will be expanded anyway if there is a wildcard before replacing).
     */
    has_wildcards = mch_has_wildcard(p);
    पर्यंत (*p != NUL)
    {
#ifdef FEAT_EVAL
	// Skip over `=expr`, wildcards in it are not expanded.
	जर (p[0] == '`' && p[1] == '=')
	{
	    p += 2;
	    (शून्य)skip_अभिव्यक्ती(&p, रिक्त);
	    जर (*p == '`')
		++p;
	    सुरूठेवा;
	}
#endif
	/*
	 * Quick check if this cannot be the start of a special string.
	 * Also removes backslash before '%', '#' and '<'.
	 */
	जर (vim_strchr((char_u *)"%#<", *p) == रिक्त)
	{
	    ++p;
	    सुरूठेवा;
	}

	/*
	 * Try to find a match at this position.
	 */
	repl = eval_vars(p, eap->arg, &srclen, &(eap->do_ecmd_lnum),
						    errormsgp, &escaped, TRUE);
	जर (*errormsgp != रिक्त)		// error detected
	    परत FAIL;
	जर (repl == रिक्त)		// no match found
	{
	    p += srclen;
	    सुरूठेवा;
	}

	// Wildcards won't be expanded below, the replacement is taken
	// literally.  But do expand "~/file", "~user/file" and "$HOME/file".
	जर (vim_strchr(repl, '$') != रिक्त || vim_strchr(repl, '~') != रिक्त)
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
	जर (!eap->usefilter
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
	    स्थिर char_u *nobslash = (char_u *)" \t\"|";
# define ESCAPE_CHARS nobslash
#अन्यथा
# define ESCAPE_CHARS escape_chars
#endif

	    साठी (l = repl; *l; ++l)
		जर (vim_strchr(ESCAPE_CHARS, *l) != रिक्त)
		{
		    l = vim_strsave_escaped(repl, ESCAPE_CHARS);
		    जर (l != रिक्त)
		    {
			vim_free(repl);
			repl = l;
		    }
		    थांबा;
		}
	}

	// For a shell command a '!' must be escaped.
	जर ((eap->usefilter || eap->cmdidx == CMD_bang
						|| eap->cmdidx == CMD_terminal)
			    && vim_strpbrk(repl, (char_u *)"!") != रिक्त)
	{
	    char_u	*l;

	    l = vim_strsave_escaped(repl, (char_u *)"!");
	    जर (l != रिक्त)
	    {
		vim_free(repl);
		repl = l;
	    }
	}

	p = repl_cmdline(eap, p, srclen, repl, cmdlinep);
	vim_free(repl);
	जर (p == रिक्त)
	    परत FAIL;
    }

    /*
     * One file argument: Expand wildcards.
     * Don't do this with ":r !command" or ":w !command".
     */
    जर ((eap->argt & EX_NOSPC) && !eap->usefilter)
    {
	/*
	 * May do this twice:
	 * 1. Replace environment variables.
	 * 2. Replace any other wildcards, remove backslashes.
	 */
	साठी (n = 1; n <= 2; ++n)
	{
	    जर (n == 2)
	    {
		/*
		 * Halve the number of backslashes (this is Vi compatible).
		 * For Unix and OS/2, when wildcards are expanded, this is
		 * done by ExpandOne() below.
		 */
#जर defined(UNIX)
		जर (!has_wildcards)
#endif
		    backslash_halve(eap->arg);
	    }

	    जर (has_wildcards)
	    {
		जर (n == 1)
		{
		    /*
		     * First loop: May expand environment variables.  This
		     * can be done much faster with expand_env() than with
		     * something else (e.g., calling a shell).
		     * After expanding environment variables, check again
		     * if there are still wildcards present.
		     */
		    जर (vim_strchr(eap->arg, '$') != रिक्त
			    || vim_strchr(eap->arg, '~') != रिक्त)
		    {
			expand_env_esc(eap->arg, NameBuff, MAXPATHL,
					(char_u *)(" \t" PATH_ESC_WILDCARDS),
					TRUE, रिक्त);
			has_wildcards = mch_has_wildcard(NameBuff);
			p = NameBuff;
		    }
		    अन्यथा
			p = रिक्त;
		}
		अन्यथा // n == 2
		{
		    expand_T	xpc;
		    पूर्णांक		options = WILD_LIST_NOTFOUND
					       | WILD_NOERROR | WILD_ADD_SLASH;

		    ExpandInit(&xpc);
		    xpc.xp_context = EXPAND_FILES;
		    जर (p_wic)
			options += WILD_ICASE;
		    p = ExpandOne(&xpc, eap->arg, रिक्त,
						   options, WILD_EXPAND_FREE);
		    जर (p == रिक्त)
			परत FAIL;
		}
		जर (p != रिक्त)
		{
		    (शून्य)repl_cmdline(eap, eap->arg, STRLEN(eap->arg),
								 p, cmdlinep);
		    जर (n == 2)	// p came from ExpandOne()
			vim_free(p);
		}
	    }
	}
    }
    परत OK;
}

/*
 * Replace part of the command line, keeping eap->cmd, eap->arg and
 * eap->nextcmd correct.
 * "src" points to the part that is to be replaced, of length "srclen".
 * "repl" is the replacement string.
 * Returns a pointer to the character after the replaced string.
 * Returns NULL for failure.
 */
    स्थिर char_u *
repl_cmdline(
    exarg_T	*eap,
    char_u	*src,
    माप_प्रकार	srclen,
    char_u	*repl,
    char_u	**cmdlinep)
{
    माप_प्रकार	repllen;
    माप_प्रकार	taillen;
    माप_प्रकार	i;
    char_u	*new_cmdline;
    माप_प्रकार	new_cmdlinelen;

    /*
     * The new command line is build in new_cmdline[].
     * First allocate it.
     * Careful: a "+cmd" argument may have been NUL terminated.
     */
    repllen = STRLEN(repl);
    taillen = STRLEN(src + srclen);
    i = (src - *cmdlinep) + repllen + taillen + 3;
    जर (eap->nextcmd != रिक्त)
	i += STRLEN(eap->nextcmd);	// add space for next command
    जर ((new_cmdline = alloc(i)) == रिक्त)
	परत रिक्त;			// out of memory!

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

    जर (eap->nextcmd != रिक्त)		// append next command
    {
	new_cmdlinelen += taillen + 1;
	STRCPY(new_cmdline + new_cmdlinelen, eap->nextcmd);
	eap->nextcmd = new_cmdline + new_cmdlinelen;
    }
    eap->cmd = new_cmdline + (eap->cmd - *cmdlinep);
    eap->arg = new_cmdline + (eap->arg - *cmdlinep);
    जर (eap->do_ecmd_cmd != रिक्त && eap->do_ecmd_cmd != dollar_command)
	eap->do_ecmd_cmd = new_cmdline + (eap->do_ecmd_cmd - *cmdlinep);
    vim_free(*cmdlinep);
    *cmdlinep = new_cmdline;

    परत src;
}

/*
 * Check for '|' to separate commands and '"' to start comments.
 * If "keep_backslash" is TRUE do not remove any backslash.
 */
    शून्य
separate_nextcmd(exarg_T *eap, पूर्णांक keep_backslash)
{
    पूर्णांक		vim9script = in_vim9script();
    char_u	*p;

#ifdef FEAT_QUICKFIX
    p = skip_grep_pat(eap);
#अन्यथा
    p = eap->arg;
#endif

    साठी ( ; *p; MB_PTR_ADV(p))
    {
	जर (*p == Ctrl_V)
	{
	    जर ((eap->argt & (EX_CTRLV | EX_XFILE)) || keep_backslash)
		++p;		// skip CTRL-V and next char
	    अन्यथा
				// remove CTRL-V and skip next char
		STRMOVE(p, p + 1);
	    जर (*p == NUL)		// stop at NUL after CTRL-V
		थांबा;
	}

#ifdef FEAT_EVAL
	// Skip over `=expr` when wildcards are expanded.
	अन्यथा जर (p[0] == '`' && p[1] == '=' && (eap->argt & EX_XFILE))
	{
	    p += 2;
	    (शून्य)skip_अभिव्यक्ती(&p, रिक्त);
	    जर (*p == NUL)		// stop at NUL after CTRL-V
		थांबा;
	}
#endif

	// Check for '"'/'#': start of comment or '|': next command
	// :@" and :*" do not start a comment!
	// :redir @" doesn't either.
	अन्यथा जर ((*p == '"'
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
		    && eap->cmdidx != CMD_घाला)
		|| *p == '\n')
	{
	    /*
	     * We remove the '\' before the '|', unless EX_CTRLV is used
	     * AND 'b' is present in 'cpoptions'.
	     */
	    जर ((vim_strchr(p_cpo, CPO_BAR) == रिक्त
			      || !(eap->argt & EX_CTRLV)) && *(p - 1) == '\\')
	    {
		जर (!keep_backslash)
		{
		    STRMOVE(p - 1, p);	// remove the '\'
		    --p;
		}
	    }
	    अन्यथा
	    {
		set_nextcmd(eap, p);
		*p = NUL;
		थांबा;
	    }
	}
    }

    जर (!(eap->argt & EX_NOTRLCOM))	// remove trailing spaces
	del_trailing_spaces(eap->arg);
}

/*
 * get + command from ex argument
 */
    स्थिर char_u *
getargcmd(char_u **argp)
{
    char_u *arg = *argp;
    char_u *command = रिक्त;

    जर (*arg == '+')	    // +[command]
    {
	++arg;
	जर (vim_isspace(*arg) || *arg == NUL)
	    command = dollar_command;
	अन्यथा
	{
	    command = arg;
	    arg = skip_cmd_arg(command, TRUE);
	    जर (*arg != NUL)
		*arg++ = NUL;		// terminate command with NUL
	}

	arg = skipwhite(arg);	// skip over spaces
	*argp = arg;
    }
    परत command;
}

/*
 * Find end of "+command" argument.  Skip over "\ " and "\\".
 */
    char_u *
skip_cmd_arg(
    char_u *p,
    पूर्णांक	   rembs)	// TRUE to halve the number of backslashes
{
    पर्यंत (*p && !vim_isspace(*p))
    {
	जर (*p == '\\' && p[1] != NUL)
	{
	    जर (rembs)
		STRMOVE(p, p + 1);
	    अन्यथा
		++p;
	}
	MB_PTR_ADV(p);
    }
    परत p;
}

    पूर्णांक
get_bad_opt(char_u *p, exarg_T *eap)
{
    जर (STRICMP(p, "keep") == 0)
	eap->bad_char = BAD_KEEP;
    अन्यथा जर (STRICMP(p, "drop") == 0)
	eap->bad_char = BAD_DROP;
    अन्यथा जर (MB_BYTE2LEN(*p) == 1 && p[1] == NUL)
	eap->bad_char = *p;
    अन्यथा
	परत FAIL;
    परत OK;
}

/*
 * Function given to ExpandGeneric() to obtain the list of bad= names.
 */
    स्थिर char_u *
get_bad_name(expand_T *xp UNUSED, पूर्णांक idx)
{
    // Note: Keep this in sync with getargopt.
    स्थिर अक्षर *(p_bad_मूल्ये[]) =
    {
	"?",
	"keep",
	"drop",
    };

    जर (idx < 0 || idx >= (पूर्णांक)ARRAY_LENGTH(p_bad_मूल्ये))
	परत रिक्त;

    परत (char_u*)p_bad_मूल्ये[idx];
}

/*
 * Get "++opt=arg" argument.
 * Return FAIL or OK.
 */
    स्थिर पूर्णांक
getargopt(exarg_T *eap)
{
    char_u	*arg = eap->arg + 2;
    पूर्णांक		*pp = रिक्त;
    पूर्णांक		bad_char_idx;
    char_u	*p;

    // Note: Keep this in sync with get_argopt_name.

    // ":edit ++[no]bin[ary] file"
    जर (STRNCMP(arg, "bin", 3) == 0 || STRNCMP(arg, "nobin", 5) == 0)
    {
	जर (*arg == 'n')
	{
	    arg += 2;
	    eap->force_bin = FORCE_NOBIN;
	}
	अन्यथा
	    eap->force_bin = FORCE_BIN;
	जर (!checkforcmd(&arg, "binary", 3))
	    परत FAIL;
	eap->arg = skipwhite(arg);
	परत OK;
    }

    // ":read ++edit file"
    जर (STRNCMP(arg, "edit", 4) == 0)
    {
	eap->read_edit = TRUE;
	eap->arg = skipwhite(arg + 4);
	परत OK;
    }

    जर (STRNCMP(arg, "ff", 2) == 0)
    {
	arg += 2;
	pp = &eap->force_ff;
    }
    अन्यथा जर (STRNCMP(arg, "fileformat", 10) == 0)
    {
	arg += 10;
	pp = &eap->force_ff;
    }
    अन्यथा जर (STRNCMP(arg, "enc", 3) == 0)
    {
	जर (STRNCMP(arg, "encoding", 8) == 0)
	    arg += 8;
	अन्यथा
	    arg += 3;
	pp = &eap->force_enc;
    }
    अन्यथा जर (STRNCMP(arg, "bad", 3) == 0)
    {
	arg += 3;
	pp = &bad_char_idx;
    }

    जर (pp == रिक्त || *arg != '=')
	परत FAIL;

    ++arg;
    *pp = (पूर्णांक)(arg - eap->cmd);
    arg = skip_cmd_arg(arg, FALSE);
    eap->arg = skipwhite(arg);
    *arg = NUL;

    जर (pp == &eap->force_ff)
    {
	जर (check_ff_मूल्य(eap->cmd + eap->force_ff) == FAIL)
	    परत FAIL;
	eap->force_ff = eap->cmd[eap->force_ff];
    }
    अन्यथा जर (pp == &eap->force_enc)
    {
	// Make 'fileencoding' lower case.
	साठी (p = eap->cmd + eap->force_enc; *p != NUL; ++p)
	    *p = TOLOWER_ASC(*p);
    }
    अन्यथा
    {
	// Check ++bad= argument.  Must be a single-byte character, "keep" or
	// "drop".
	जर (get_bad_opt(eap->cmd + bad_char_idx, eap) == FAIL)
	    परत FAIL;
    }

    परत OK;
}

/*
 * Function given to ExpandGeneric() to obtain the list of ++opt names.
 */
    स्थिर char_u *
get_argopt_name(expand_T *xp UNUSED, पूर्णांक idx)
{
    // Note: Keep this in sync with getargopt.
    स्थिर अक्षर *(p_opt_मूल्ये[]) =
    {
	"fileformat=",
	"encoding=",
	"binary",
	"nobinary",
	"bad=",
	"edit",
    };

    जर (idx < 0 || idx >= (पूर्णांक)ARRAY_LENGTH(p_opt_मूल्ये))
	परत रिक्त;

    परत (char_u*)p_opt_मूल्ये[idx];
}

/*
 * Command-line expansion for ++opt=name.
 */
    पूर्णांक
expand_argopt(
	char_u	    *pat,
	expand_T    *xp,
	regmatch_T  *rmp,
	char_u	    ***matches,
	पूर्णांक	    *अंक_Matches)
{
    जर (xp->xp_pattern > xp->xp_ओळ && *(xp->xp_pattern-1) == '=')
    {
	char_u *(*cb)(expand_T *, पूर्णांक) = रिक्त;

	char_u *name_end = xp->xp_pattern - 1;
	जर (name_end - xp->xp_ओळ >= 2
		&& STRNCMP(name_end - 2, "ff", 2) == 0)
	    cb = get_fileformat_name;
	अन्यथा जर (name_end - xp->xp_ओळ >= 10
		&& STRNCMP(name_end - 10, "fileformat", 10) == 0)
	    cb = get_fileformat_name;
	अन्यथा जर (name_end - xp->xp_ओळ >= 3
		&& STRNCMP(name_end - 3, "enc", 3) == 0)
	    cb = get_encoding_name;
	अन्यथा जर (name_end - xp->xp_ओळ >= 8
		&& STRNCMP(name_end - 8, "encoding", 8) == 0)
	    cb = get_encoding_name;
	अन्यथा जर (name_end - xp->xp_ओळ >= 3
		&& STRNCMP(name_end - 3, "bad", 3) == 0)
	    cb = get_bad_name;

	जर (cb != रिक्त)
	{
	    परत ExpandGeneric(
		    pat,
		    xp,
		    rmp,
		    matches,
		    अंक_Matches,
		    cb,
		    FALSE);
	}
	परत FAIL;
    }

    // Special handling of "ff" which acts as a short form of
    // "fileformat", as "ff" is not a substring of it.
    जर (xp->xp_pattern_len == 2
	    && STRNCMP(xp->xp_pattern, "ff", xp->xp_pattern_len) == 0)
    {
	*matches = ALLOC_MULT(char_u *, 1);
	जर (*matches == रिक्त)
	    परत FAIL;
	*अंक_Matches = 1;
	(*matches)[0] = vim_strsave((char_u*)"fileformat=");
	परत OK;
    }

    परत ExpandGeneric(
	    pat,
	    xp,
	    rmp,
	    matches,
	    अंक_Matches,
	    get_argopt_name,
	    FALSE);
}

    स्थिर शून्य
ex_autocmd(exarg_T *eap)
{
    जर (eap->skip)
    {
	skip_cmd_block(eap);
	परत;
    }

    /*
     * Disallow autocommands from .exrc and .vimrc in current
     * directory for security reasons.
     */
    जर (secure)
    {
	secure = 2;
	eap->errmsg =
	      _(e_command_not_allowed_from_vimrc_in_current_dir_or_tag_शोध);
    }
    अन्यथा जर (eap->cmdidx == CMD_autocmd)
	do_autocmd(eap, eap->arg, eap->forceit);
    अन्यथा
	do_augroup(eap->arg, eap->forceit);
}

/*
 * ":doautocmd": Apply the automatic commands to the current buffer.
 */
    स्थिर शून्य
ex_doautocmd(exarg_T *eap)
{
    char_u	*arg = eap->arg;
    पूर्णांक		call_do_modelines = check_nomodeline(&arg);
    पूर्णांक		did_aucmd;

    (शून्य)do_doautocmd(arg, TRUE, &did_aucmd);
    // Only when there is no <nomodeline>.
    जर (call_do_modelines && did_aucmd)
	do_modelines(0);
}

/*
 * :[N]bunload[!] [N] [bufname] unload buffer
 * :[N]bdelete[!] [N] [bufname] delete buffer from buffer list
 * :[N]bwipeout[!] [N] [bufname] delete buffer really
 */
    स्थिर शून्य
ex_bunload(exarg_T *eap)
{
    जर (ERROR_IF_ANY_POPUP_WINDOW)
	परत;
    eap->errmsg = do_bufdel(
	    eap->cmdidx == CMD_bdelete ? DOBUF_DEL
		: eap->cmdidx == CMD_bwipeout ? DOBUF_WIPE
		: DOBUF_UNLOAD, eap->arg,
	    eap->addr_मोजणी, (पूर्णांक)eap->line1, (पूर्णांक)eap->line2, eap->forceit);
}

/*
 * :[N]buffer [N]	to buffer N
 * :[N]sbuffer [N]	to buffer N
 */
    स्थिर शून्य
ex_buffer(exarg_T *eap)
{
    जर (ERROR_IF_ANY_POPUP_WINDOW)
	परत;
    do_exbuffer(eap);
}

/*
 * ":buffer" command and alike.
 */
    स्थिर शून्य
do_exbuffer(exarg_T *eap)
{
    जर (*eap->arg)
	eap->errmsg = ex_errmsg(e_trailing_characters_str, eap->arg);
    अन्यथा
    {
	जर (eap->addr_मोजणी == 0)	// default is current buffer
	    goto_buffer(eap, DOBUF_CURRENT, FORWARD, 0);
	अन्यथा
	    goto_buffer(eap, DOBUF_FIRST, FORWARD, (पूर्णांक)eap->line2);
	जर (eap->do_ecmd_cmd != रिक्त)
	    do_cmd_argument(eap->do_ecmd_cmd);
    }
}

/*
 * :[N]bmodified [N]	to next mod. buffer
 * :[N]sbmodified [N]	to next mod. buffer
 */
    स्थिर शून्य
ex_bmodified(exarg_T *eap)
{
    goto_buffer(eap, DOBUF_MOD, FORWARD, (पूर्णांक)eap->line2);
    जर (eap->do_ecmd_cmd != रिक्त)
	do_cmd_argument(eap->do_ecmd_cmd);
}

/*
 * :[N]bnext [N]	to next buffer
 * :[N]sbnext [N]	split and to next buffer
 */
    स्थिर शून्य
ex_bnext(exarg_T *eap)
{
    जर (ERROR_IF_ANY_POPUP_WINDOW)
	परत;

    goto_buffer(eap, DOBUF_CURRENT, FORWARD, (पूर्णांक)eap->line2);
    जर (eap->do_ecmd_cmd != रिक्त)
	do_cmd_argument(eap->do_ecmd_cmd);
}

/*
 * :[N]bNext [N]	to previous buffer
 * :[N]bprevious [N]	to previous buffer
 * :[N]sbNext [N]	split and to previous buffer
 * :[N]sbprevious [N]	split and to previous buffer
 */
    स्थिर शून्य
ex_bprevious(exarg_T *eap)
{
    जर (ERROR_IF_ANY_POPUP_WINDOW)
	परत;

    goto_buffer(eap, DOBUF_CURRENT, BACKWARD, (पूर्णांक)eap->line2);
    जर (eap->do_ecmd_cmd != रिक्त)
	do_cmd_argument(eap->do_ecmd_cmd);
}

/*
 * :brewind		to first buffer
 * :bfirst		to first buffer
 * :sbrewind		split and to first buffer
 * :sbfirst		split and to first buffer
 */
    स्थिर शून्य
ex_brewind(exarg_T *eap)
{
    जर (ERROR_IF_ANY_POPUP_WINDOW)
	परत;

    goto_buffer(eap, DOBUF_FIRST, FORWARD, 0);
    जर (eap->do_ecmd_cmd != रिक्त)
	do_cmd_argument(eap->do_ecmd_cmd);
}

/*
 * :blast		to last buffer
 * :sblast		split and to last buffer
 */
    स्थिर शून्य
ex_blast(exarg_T *eap)
{
    जर (ERROR_IF_ANY_POPUP_WINDOW)
	परत;

    goto_buffer(eap, DOBUF_LAST, BACKWARD, 0);
    जर (eap->do_ecmd_cmd != रिक्त)
	do_cmd_argument(eap->do_ecmd_cmd);
}

/*
 * Check if "c" ends an Ex command.
 * In Vim9 script does not check for white space before #.
 */
    पूर्णांक
ends_excmd(पूर्णांक c)
{
    पूर्णांक comment_char = '"';

    जर (in_vim9script())
	comment_char = '#';
    परत (c == NUL || c == '|' || c == comment_char || c == '\n');
}

/*
 * Like ends_excmd() but checks that a # in Vim9 script either has "cmd" equal
 * to "cmd_start" or has a white space character before it.
 */
    पूर्णांक
ends_excmd2(char_u *cmd_start UNUSED, char_u *cmd)
{
    पूर्णांक c = *cmd;

    जर (c == NUL || c == '|' || c == '\n')
	परत TRUE;
    जर (in_vim9script())
	//  # starts a comment, #{ might be a mistake, #{{ can start a fold
	परत c == '#' && (cmd[1] != '{' || cmd[2] == '{')
				 && (cmd == cmd_start || VIM_ISWHITE(cmd[-1]));
    परत c == '"';
}

#जर defined(FEAT_SYN_HL) || defined(FEAT_शोध_EXTRA) || defined(FEAT_EVAL)
/*
 * Return the next command, after the first '|' or '\n'.
 * Return NULL if not found.
 */
    char_u *
find_nextcmd(char_u *p)
{
    पर्यंत (*p != '|' && *p != '\n')
    {
	जर (*p == NUL)
	    परत रिक्त;
	++p;
    }
    परत (p + 1);
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

    जर (*s == '|' || *s == '\n')
	परत (s + 1);
    अन्यथा
	परत रिक्त;
}

/*
 * If "eap->nextcmd" is not set, check for a next command at "p".
 */
    शून्य
set_nextcmd(exarg_T *eap, char_u *arg)
{
    char_u *p = skipwhite(arg);

    जर (in_vim9script() && *p == '#')
    {
	char_u *nl = vim_strchr(p, NL);
	जर (nl != रिक्त)
	    p = nl;
    }

    p = check_nextcmd(p);

    जर (eap->nextcmd == रिक्त)
	eap->nextcmd = p;
    अन्यथा जर (p != रिक्त)
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
    स्थिर पूर्णांक
check_more(
    पूर्णांक message,	    // when FALSE check only, no messages
    पूर्णांक forceit)
{
    पूर्णांक	    n = ARGCOUNT - curwin->w_arg_idx - 1;

    जर (!forceit && only_one_window()
	    && ARGCOUNT > 1 && !arg_had_last && n > 0 && quitmore == 0)
    {
	जर (message)
	{
#जर defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
	    जर ((p_confirm || (cmdmod.cmod_flags & CMOD_CONFIRM))
						    && curbuf->b_fname != रिक्त)
	    {
		char_u	buff[DIALOG_संदेश_SIZE];

		vim_snprintf((अक्षर *)buff, DIALOG_संदेश_SIZE,
			NGETTEXT("%d more file to edit.  Quit anyway?",
			    "%d more files to edit.  Quit anyway?", n), n);
		जर (vim_dialog_yesno(VIM_QUESTION, रिक्त, buff, 1) == VIM_YES)
		    परत OK;
		परत FAIL;
	    }
#endif
	    semsg(NGETTEXT(e_nr_more_file_to_edit,
			   e_nr_more_files_to_edit , n), n);
	    quitmore = 2;	    // next try to quit is allowed
	}
	परत FAIL;
    }
    परत OK;
}

/*
 * Function given to ExpandGeneric() to obtain the list of command names.
 */
    char_u *
get_command_name(expand_T *xp UNUSED, पूर्णांक idx)
{
    जर (idx >= (पूर्णांक)CMD_SIZE)
	परत expand_user_command_name(idx);
    // the following are no real commands
    जर (STRNCMP(cmdnames[idx].cmd_name, "{", 1) == 0 ||
	STRNCMP(cmdnames[idx].cmd_name, "}", 1) == 0)
	परत (char_u *)"";
    परत cmdnames[idx].cmd_name;
}

    स्थिर शून्य
ex_colorscheme(exarg_T *eap)
{
    जर (*eap->arg == NUL)
    {
#ifdef FEAT_EVAL
	char_u *अभिव्यक्ती = vim_strsave((char_u *)"g:colors_name");
	char_u *p = रिक्त;

	जर (अभिव्यक्ती != रिक्त)
	{
	    ++emsg_off;
	    p = eval_to_string(अभिव्यक्ती, FALSE, FALSE);
	    --emsg_off;
	    vim_free(अभिव्यक्ती);
	}
	जर (p != रिक्त)
	{
	    संदेश((अक्षर *)p);
	    vim_free(p);
	}
	अन्यथा
	    संदेश("default");
#अन्यथा
	संदेश(_("unknown"));
#endif
    }
    अन्यथा जर (load_colors(eap->arg) == FAIL)
	semsg(_(e_cannot_find_color_scheme_str), eap->arg);

#ifdef FEAT_VTP
    अन्यथा जर (has_vtp_working())
    {
	// background color change requires clear + redraw
	update_screen(UPD_CLEAR);
	redrawcmd();
    }
#endif
}

    स्थिर शून्य
ex_highlight(exarg_T *eap)
{
    जर (*eap->arg == NUL && eap->cmd[2] == '!')
	संदेश(_("Greetings, Vim user!"));
    do_highlight(eap->arg, eap->forceit, FALSE);
}


/*
 * Call this function if we thought we were going to exit, but we won't
 * (because of an error).  May need to restore the terminal mode.
 */
    शून्य
not_exiting(पूर्णांक save_exiting)
{
    exiting = save_exiting;
    settmode(TMODE_RAW);
}

    पूर्णांक
before_quit_autocmds(win_T *wp, पूर्णांक quit_all, पूर्णांक forceit)
{
    apply_autocmds(EVENT_QUITPRE, रिक्त, रिक्त, FALSE, wp->w_buffer);

    // Bail out when autocommands closed the window.
    // Refuse to quit when the buffer in the last window is being closed (can
    // only happen in autocommands).
    जर (!win_valid(wp)
	    || curbuf_locked()
	    || (wp->w_buffer->b_nwindows == 1 && wp->w_buffer->b_locked > 0))
	परत TRUE;

    जर (quit_all || (check_more(FALSE, forceit) == OK && only_one_window()))
    {
	apply_autocmds(EVENT_EXITPRE, रिक्त, रिक्त, FALSE, curbuf);
	// Refuse to quit when locked or when the window was closed or the
	// buffer in the last window is being closed (can only happen in
	// autocommands).
	जर (!win_valid(wp) || curbuf_locked()
			  || (curbuf->b_nwindows == 1 && curbuf->b_locked > 0))
	    परत TRUE;
    }

    परत FALSE;
}

/*
 * ":quit": quit current window, quit Vim if the last window is closed.
 * ":{nr}quit": quit window {nr}
 * Also used when closing a terminal window that's the last one.
 */
    शून्य
ex_quit(exarg_T *eap)
{
    win_T	*wp;

    जर (cmdwin_type != 0)
    {
	cmdwin_result = Ctrl_C;
	परत;
    }
    // Don't quit while editing the command line.
    जर (text_locked())
    {
	text_locked_संदेश();
	परत;
    }
    जर (eap->addr_मोजणी > 0)
    {
	पूर्णांक	wnr = eap->line2;

	साठी (wp = firstwin; wp->w_next != रिक्त; wp = wp->w_next)
	    जर (--wnr <= 0)
		थांबा;
    }
    अन्यथा
	wp = curwin;

    // Refuse to quit when locked.
    जर (curbuf_locked())
	परत;

    // Trigger QuitPre and maybe ExitPre
    जर (before_quit_autocmds(wp, FALSE, eap->forceit))
	परत;

#ifdef FEAT_NETBEANS_INTG
    netbeansForcedQuit = eap->forceit;
#endif

    पूर्णांक save_exiting = exiting;
    /*
     * If there is only one relevant window we will exit.
     */
    जर (check_more(FALSE, eap->forceit) == OK && only_one_window())
	exiting = TRUE;
    जर ((!buf_hide(wp->w_buffer)
		&& check_changed(wp->w_buffer, (p_awa ? CCGD_AW : 0)
				       | (eap->forceit ? CCGD_FORCEIT : 0)
				       | CCGD_EXCMD))
	    || check_more(TRUE, eap->forceit) == FAIL
	    || (only_one_window() && check_changed_any(eap->forceit, TRUE)))
    {
	not_exiting(save_exiting);
    }
    अन्यथा
    {
	// quit last window
	// Note: only_one_window() returns true, even so a help window is
	// still open. In that case only quit, if no address has been
	// specified. Example:
	// :h|wincmd w|1q     - don't quit
	// :h|wincmd w|q      - quit
	जर (only_one_window() && (ONE_WINDOW || eap->addr_मोजणी == 0))
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
    स्थिर शून्य
ex_cquit(exarg_T *eap UNUSED)
{
    // this does not always pass on the exit code to the Manx compiler. why?
    getout(eap->addr_मोजणी > 0 ? (पूर्णांक)eap->line2 : निकास_त्रुटि);
}

/*
 * Do preparations for "qall" and "wqall".
 * Returns FAIL when quitting should be aborted.
 */
    पूर्णांक
before_quit_all(exarg_T *eap)
{
    जर (cmdwin_type != 0)
    {
	जर (eap->forceit)
	    cmdwin_result = K_XF1;	// ex_window() takes care of this
	अन्यथा
	    cmdwin_result = K_XF2;
	परत FAIL;
    }

    // Don't quit while editing the command line.
    जर (text_locked())
    {
	text_locked_संदेश();
	परत FAIL;
    }

    जर (before_quit_autocmds(curwin, TRUE, eap->forceit))
	परत FAIL;

    परत OK;
}

/*
 * ":qall": try to quit all windows
 */
    स्थिर शून्य
ex_quit_all(exarg_T *eap)
{
    जर (before_quit_all(eap) == FAIL)
	परत;
    पूर्णांक save_exiting = exiting;
    exiting = TRUE;
    जर (eap->forceit || !check_changed_any(FALSE, FALSE))
	getout(0);
    not_exiting(save_exiting);
}

/*
 * ":close": close current window, unless it is the last one
 */
    स्थिर शून्य
ex_close(exarg_T *eap)
{
    win_T	*win;
    पूर्णांक		winnr = 0;
    जर (cmdwin_type != 0)
	cmdwin_result = Ctrl_C;
    अन्यथा
	जर (!text_locked() && !curbuf_locked())
	{
	    जर (eap->addr_मोजणी == 0)
		ex_win_close(eap->forceit, curwin, रिक्त);
	    अन्यथा
	    {
		FOR_ALL_WINDOWS(win)
		{
		    winnr++;
		    जर (winnr == eap->line2)
			थांबा;
		}
		जर (win == रिक्त)
		    win = lastwin;
		ex_win_close(eap->forceit, win, रिक्त);
	    }
	}
}

#ifdef FEAT_QUICKFIX
/*
 * ":pclose": Close any preview window.
 */
    स्थिर शून्य
ex_pclose(exarg_T *eap)
{
    win_T	*win;

    // First close any normal window.
    FOR_ALL_WINDOWS(win)
	जर (win->w_p_pvw)
	{
	    ex_win_close(eap->forceit, win, रिक्त);
	    परत;
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
    स्थिर शून्य
ex_win_close(
    पूर्णांक		forceit,
    win_T	*win,
    tabpage_T	*tp)		// NULL or the tab page "win" is in
{
    पूर्णांक		need_hide;
    buf_T	*buf = win->w_buffer;

    // Never close the autocommand window.
    जर (is_aucmd_win(win))
    {
	emsg(_(e_cannot_close_autocmd_or_popup_window));
	परत;
    }
    जर (window_layout_locked(CMD_close))
	परत;

    need_hide = (bufIsChanged(buf) && buf->b_nwindows <= 1);
    जर (need_hide && !buf_hide(buf) && !forceit)
    {
#जर defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
	जर ((p_confirm || (cmdmod.cmod_flags & CMOD_CONFIRM)) && p_write)
	{
# ifdef FEAT_TERMINAL
	    जर (term_job_running(buf->b_term))
	    {
		जर (term_confirm_stop(buf) == FAIL)
		    परत;
		// Manually kill the terminal here because this command will
		// hide it otherwise.
		free_terminal(buf);
		need_hide = FALSE;
	    }
	    अन्यथा
# endif
	    {
		bufref_T bufref;

		set_bufref(&bufref, buf);
		dialog_changed(buf, FALSE);
		जर (bufref_valid(&bufref) && bufIsChanged(buf))
		    परत;
		need_hide = FALSE;
	    }
	}
	अन्यथा
#endif
	{
	    no_write_message();
	    परत;
	}
    }

#ifdef FEAT_GUI
    need_mouse_correct = TRUE;
#endif

    // free buffer when not hiding it or when it's a scratch buffer
    जर (tp == रिक्त)
	win_close(win, !need_hide && !buf_hide(buf));
    अन्यथा
	win_close_othertab(win, !need_hide && !buf_hide(buf), tp);
}

/*
 * Handle the argument for a tabpage related ex command.
 * Returns a tabpage number.
 * When an error is encountered then eap->errmsg is set.
 */
    स्थिर पूर्णांक
get_tabpage_arg(exarg_T *eap)
{
    पूर्णांक tab_संख्या;
    पूर्णांक unaccept_arg0 = (eap->cmdidx == CMD_tabmove) ? 0 : 1;

    जर (eap->arg && *eap->arg != NUL)
    {
	char_u *p = eap->arg;
	char_u *p_save;
	पूर्णांक    relative = 0; // argument +N/-N means: go to N places to the
			     // right/left relative to the current position.

	जर (*p == '-')
	{
	    relative = -1;
	    p++;
	}
	अन्यथा जर (*p == '+')
	{
	    relative = 1;
	    p++;
	}

	p_save = p;
	tab_संख्या = getdigits(&p);

	जर (relative == 0)
	{
	    जर (STRCMP(p, "$") == 0)
		tab_संख्या = LAST_TAB_NR;
	    अन्यथा जर (STRCMP(p, "#") == 0)
		जर (valid_tabpage(lastused_tabpage))
		    tab_संख्या = tabpage_index(lastused_tabpage);
		अन्यथा
		{
		    eap->errmsg = ex_errmsg(e_invalid_मूल्य_for_argument_str,
								     eap->arg);
		    tab_संख्या = 0;
		    जा theend;
		}
	    अन्यथा जर (p == p_save || *p_save == '-' || *p != NUL
		    || tab_संख्या > LAST_TAB_NR)
	    {
		// No numbers as argument.
		eap->errmsg = ex_errmsg(e_invalid_argument_str, eap->arg);
		जा theend;
	    }
	}
	अन्यथा
	{
	    जर (*p_save == NUL)
		tab_संख्या = 1;
	    अन्यथा जर (p == p_save || *p_save == '-' || *p != NUL
		    || tab_संख्या == 0)
	    {
		// No numbers as argument.
		eap->errmsg = ex_errmsg(e_invalid_argument_str, eap->arg);
		जा theend;
	    }
	    tab_संख्या = tab_संख्या * relative + tabpage_index(curtab);
	    जर (!unaccept_arg0 && relative == -1)
		--tab_संख्या;
	}
	जर (tab_संख्या < unaccept_arg0 || tab_संख्या > LAST_TAB_NR)
	    eap->errmsg = ex_errmsg(e_invalid_argument_str, eap->arg);
    }
    अन्यथा जर (eap->addr_मोजणी > 0)
    {
	जर (unaccept_arg0 && eap->line2 == 0)
	{
	    eap->errmsg = _(e_invalid_range);
	    tab_संख्या = 0;
	}
	अन्यथा
	{
	    tab_संख्या = eap->line2;
	    जर (!unaccept_arg0)
	    {
		char_u *cmdp = eap->cmd;

		पर्यंत (--cmdp > *eap->cmdlinep
			&& (VIM_ISWHITE(*cmdp) || VIM_ISDIGIT(*cmdp)))
		    ;
		जर (*cmdp == '-')
		{
		    --tab_संख्या;
		    जर (tab_संख्या < unaccept_arg0)
			eap->errmsg = _(e_invalid_range);
		}
	    }
	}
    }
    अन्यथा
    {
	निवड (eap->cmdidx)
	{
	स्थिती CMD_tabnext:
	    tab_संख्या = tabpage_index(curtab) + 1;
	    जर (tab_संख्या > LAST_TAB_NR)
		tab_संख्या = 1;
	    थांबा;
	स्थिती CMD_tabmove:
	    tab_संख्या = LAST_TAB_NR;
	    थांबा;
	पूर्वनिर्धारित:
	    tab_संख्या = tabpage_index(curtab);
	}
    }

theend:
    परत tab_संख्या;
}

/*
 * ":tabclose": close current tab page, unless it is the last one.
 * ":tabclose N": close tab page N.
 */
    स्थिर शून्य
ex_tabclose(exarg_T *eap)
{
    tabpage_T	*tp;
    पूर्णांक		tab_संख्या;

    जर (cmdwin_type != 0)
    {
	cmdwin_result = K_IGNORE;
	परत;
    }

    जर (first_tabpage->tp_next == रिक्त)
    {
	emsg(_(e_cannot_close_last_tab_page));
	परत;
    }

    जर (window_layout_locked(CMD_tabclose))
	परत;

    tab_संख्या = get_tabpage_arg(eap);
    जर (eap->errmsg != रिक्त)
	परत;

    tp = find_tabpage(tab_संख्या);
    जर (tp == रिक्त)
    {
	beep_flush();
	परत;
    }
    जर (tp != curtab)
    {
	tabpage_close_other(tp, eap->forceit);
	परत;
    }
    अन्यथा जर (!text_locked() && !curbuf_locked())
	tabpage_close(eap->forceit);
}

/*
 * ":tabonly": close all tab pages except the current one
 */
    स्थिर शून्य
ex_tabonly(exarg_T *eap)
{
    tabpage_T	*tp;
    पूर्णांक		done;
    पूर्णांक		tab_संख्या;

    जर (cmdwin_type != 0)
    {
	cmdwin_result = K_IGNORE;
	परत;
    }

    जर (first_tabpage->tp_next == रिक्त)
    {
	संदेश(_("Already only one tab page"));
	परत;
    }

    जर (window_layout_locked(CMD_tabonly))
	परत;

    tab_संख्या = get_tabpage_arg(eap);
    जर (eap->errmsg != रिक्त)
	परत;

    goto_tabpage(tab_संख्या);
    // Repeat this up to a 1000 times, because autocommands may
    // mess up the lists.
    साठी (done = 0; done < 1000; ++done)
    {
	FOR_ALL_TABPAGES(tp)
	    जर (tp->tp_topframe != topframe)
	    {
		tabpage_close_other(tp, eap->forceit);
		// if we failed to close it quit
		जर (valid_tabpage(tp))
		    done = 1000;
		// start over, "tp" is now invalid
		थांबा;
	    }
	जर (first_tabpage->tp_next == रिक्त)
	    थांबा;
    }
}

/*
 * Close the current tab page.
 */
    शून्य
tabpage_close(पूर्णांक forceit)
{
    जर (window_layout_locked(CMD_tabclose))
	परत;

    trigger_tabclosedpre(curtab);
    curtab->tp_did_tabclosedpre = TRUE;
    tabpage_T *save_curtab = curtab;

    // First close all the windows but the current one.  If that worked then
    // close the last window in this tab, that will close it.
    जर (!ONE_WINDOW)
	close_others(TRUE, forceit);
    जर (ONE_WINDOW)
	ex_win_close(forceit, curwin, रिक्त);
    जर (curtab == save_curtab)
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
    शून्य
tabpage_close_other(tabpage_T *tp, पूर्णांक forceit)
{
    पूर्णांक		done = 0;
    win_T	*wp;

    जर (window_layout_locked(CMD_SIZE))
	परत;

    trigger_tabclosedpre(tp);
    tp->tp_did_tabclosedpre = TRUE;

    // Limit to 1000 windows, autocommands may add a window while we close
    // one.  OK, so I'm paranoid...
    पर्यंत (++done < 1000)
    {
	wp = tp->tp_firstwin;
	ex_win_close(forceit, wp, tp);

	// Autocommands may delete the tab page under our fingers.
	जर (!valid_tabpage(tp))
	    थांबा;
	// We may fail to close a window with a modified buffer.
	जर (tp->tp_firstwin == wp)
	{
	    done = 1000;
	    थांबा;
	}
    }
    जर (done >= 1000)
    {
	// When closing the tab page failed, reset tp_did_tabclosedpre so that
	// TabClosedPre behaves consistently on next :close vs :tabclose.
	tp->tp_did_tabclosedpre = FALSE;
	परत;
    }

    apply_autocmds(EVENT_TABCLOSED, रिक्त, रिक्त, FALSE, curbuf);
}

/*
 * ":only".
 */
    स्थिर शून्य
ex_only(exarg_T *eap)
{
    जर (window_layout_locked(CMD_only))
	परत;
#ifdef FEAT_GUI
    need_mouse_correct = TRUE;
#endif
    जर (eap->addr_मोजणी > 0)
    {
	win_T   *wp;
	पूर्णांक	wnr = eap->line2;
	साठी (wp = firstwin; --wnr > 0; )
	{
	    जर (wp->w_next == रिक्त)
		थांबा;
	    अन्यथा
		wp = wp->w_next;
	}
	win_goto(wp);
    }
    close_others(TRUE, eap->forceit);
}

    स्थिर शून्य
ex_hide(exarg_T *eap UNUSED)
{
    // ":hide" or ":hide | cmd": hide current window
    जर (eap->skip)
	परत;

    जर (window_layout_locked(CMD_hide))
	परत;
#ifdef FEAT_GUI
    need_mouse_correct = TRUE;
#endif
    जर (eap->addr_मोजणी == 0)
	win_close(curwin, FALSE);	// don't free buffer
    अन्यथा
    {
	पूर्णांक	winnr = 0;
	win_T	*win;

	FOR_ALL_WINDOWS(win)
	{
	    winnr++;
	    जर (winnr == eap->line2)
		थांबा;
	}
	जर (win == रिक्त)
	    win = lastwin;
	win_close(win, FALSE);
    }
}

/*
 * ":stop" and ":suspend": Suspend Vim.
 */
    शून्य
ex_stop(exarg_T *eap)
{
    /*
     * Disallow suspending for "rvim".
     */
    जर (check_restricted())
	परत;

    जर (!eap->forceit)
	autowrite_all();
    apply_autocmds(EVENT_VIMSUSPEND, रिक्त, रिक्त, FALSE, रिक्त);
    windgoto((पूर्णांक)Rows - 1, cmdline_col_off);
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
    apply_autocmds(EVENT_VIMRESUME, रिक्त, रिक्त, FALSE, रिक्त);
}

/*
 * ":exit", ":xit" and ":wq": Write file and quit the current window.
 */
    स्थिर शून्य
ex_exit(exarg_T *eap)
{
#ifdef FEAT_EVAL
    जर (not_in_vim9(eap, false) == FAIL)
	परत;
#endif
    जर (cmdwin_type != 0)
    {
	cmdwin_result = Ctrl_C;
	परत;
    }
    // Don't quit while editing the command line.
    जर (text_locked())
    {
	text_locked_संदेश();
	परत;
    }

    पूर्णांक save_exiting = exiting;
    /*
     * we plan to exit if there is only one relevant window
     */
    जर (check_more(FALSE, eap->forceit) == OK && only_one_window())
	exiting = TRUE;

    // Write the buffer for ":wq" or when it was changed.
    // Trigger QuitPre and ExitPre.
    // Check if we can exit now, after autocommands have changed things.
    जर (((eap->cmdidx == CMD_wq || curbufIsChanged()) && do_write(eap) == FAIL)
	    || before_quit_autocmds(curwin, FALSE, eap->forceit)
	    || check_more(TRUE, eap->forceit) == FAIL
	    || (only_one_window() && check_changed_any(eap->forceit, FALSE)))
    {
	not_exiting(save_exiting);
    }
    अन्यथा
    {
	जर (only_one_window())	    // quit last window, exit Vim
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
    स्थिर शून्य
ex_print(exarg_T *eap)
{
    जर (curbuf->b_ml.ml_flags & ML_EMPTY)
	emsg(_(e_empty_buffer));
    अन्यथा
    {
	साठी ( ;!got_int; ui_breakcheck())
	{
	    print_ओळ(eap->line1,
		    (eap->cmdidx == CMD_संख्या || eap->cmdidx == CMD_pound
						 || (eap->flags & EXFLAG_NR)),
		    eap->cmdidx == CMD_list || (eap->flags & EXFLAG_LIST));
	    जर (++eap->line1 > eap->line2)
		थांबा;
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
    स्थिर शून्य
ex_goto(exarg_T *eap)
{
    goto_byte(eap->line2);
}
#endif

/*
 * ":shell".
 */
    स्थिर शून्य
ex_shell(exarg_T *eap UNUSED)
{
    do_shell(रिक्त, 0);
}

#जर defined(HAVE_DROP_FILE)

स्थिर पूर्णांक drop_busy = FALSE;
स्थिर पूर्णांक drop_filec;
स्थिर char_u **drop_filev = रिक्त;
स्थिर पूर्णांक drop_split;
स्थिर शून्य (*drop_callback)(शून्य *);
स्थिर शून्य *drop_cookie;

    स्थिर शून्य
handle_drop_internal(शून्य)
{
    exarg_T	ea;
    पूर्णांक		save_संदेश_scroll = संदेश_scroll;

    // Setting the argument list may cause screen updates and being called
    // recursively.  Avoid that by setting drop_busy.
    drop_busy = TRUE;

    // Check whether the current buffer is changed. If so, we will need
    // to split the current window or data could be lost.
    // We don't need to check if the 'hidden' option is set, as in this
    // case the buffer won't be lost.
    जर (!buf_hide(curbuf) && !drop_split)
    {
	++emsg_off;
	drop_split = check_changed(curbuf, CCGD_AW);
	--emsg_off;
    }
    जर (drop_split)
    {
	जर (win_split(0, 0) == FAIL)
	    परत;
	RESET_BINDING(curwin);

	// When splitting the window, create a new alist.  Otherwise the
	// existing one is overwritten.
	alist_unlink(curwin->w_alist);
	alist_new();
    }

    /*
     * Set up the new argument list.
     */
    alist_set(ALIST(curwin), drop_filec, drop_filev, FALSE, रिक्त, 0);

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
    संदेश_scroll = save_संदेश_scroll;

    जर (drop_callback != रिक्त)
	drop_callback(drop_cookie);

    drop_filev = रिक्त;
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
    शून्य
handle_drop(
    पूर्णांक		filec,		// the number of files dropped
    char_u	**filev,	// the list of files dropped
    पूर्णांक		split,		// force splitting the window
    शून्य	(*callback)(शून्य *), // to be called after setting the argument
				     // list
    शून्य	*cookie)	// argument for "callback" (allocated)
{
    // Cannot handle recursive drops, finish the pending one.
    जर (drop_busy)
    {
	FreeWild(filec, filev);
	vim_free(cookie);
	परत;
    }

    // When calling handle_drop() more than once in a row we only use the last
    // one.
    जर (drop_filev != रिक्त)
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
    जर (text_locked() || curbuf_locked() || updating_screen)
	परत;

    handle_drop_internal();
}

/*
 * To be called when text is unlocked, curbuf is unlocked or updating_screen is
 * reset: Handle a postponed drop.
 */
    शून्य
handle_any_postponed_drop(शून्य)
{
    जर (!drop_busy && drop_filev != रिक्त
	     && !text_locked() && !curbuf_locked() && !updating_screen)
	handle_drop_internal();
}
#endif

/*
 * ":preserve".
 */
    स्थिर शून्य
ex_preserve(exarg_T *eap UNUSED)
{
    curbuf->b_flags |= BF_PRESERVED;
    ml_preserve(curbuf, TRUE);
}

/*
 * ":recover".
 */
    स्थिर शून्य
ex_recover(exarg_T *eap)
{
    // Set recoverymode right away to avoid the ATTENTION prompt.
    recoverymode = TRUE;
    जर (!check_changed(curbuf, (p_awa ? CCGD_AW : 0)
			     | CCGD_MULTWIN
			     | (eap->forceit ? CCGD_FORCEIT : 0)
			     | CCGD_EXCMD)

	    && (*eap->arg == NUL
			     || setfname(curbuf, eap->arg, रिक्त, TRUE) == OK))
	ml_recover(TRUE);
    recoverymode = FALSE;
}

/*
 * Command modifier used in a wrong way.  Also for other commands that can't
 * appear at the toplevel.
 */
    स्थिर शून्य
ex_wrongmodifier(exarg_T *eap)
{
    eap->errmsg = ex_errmsg(e_invalid_command_str, eap->cmd);
}

#जर defined(FEAT_EVAL)

// callback function for 'findfunc'
स्थिर callback_T ffu_cb;

    स्थिर callback_T *
get_findfunc_callback(शून्य)
{
    परत *curbuf->b_p_ffu != NUL ? &curbuf->b_ffu_cb : &ffu_cb;
}

/*
 * Call 'findfunc' to obtain a list of file names.
 */
    स्थिर list_T *
call_findfunc(char_u *pat, पूर्णांक cmdcomplete)
{
    typval_T	args[3];
    callback_T	*cb;
    typval_T	rettv;
    पूर्णांक		retval;
    sctx_T	saved_sctx = current_sctx;
    sctx_T	*ctx;

    args[0].v_type = VAR_STRING;
    args[0].vval.v_string = pat;
    args[1].v_type = VAR_BOOL;
    args[1].vval.v_संख्या = cmdcomplete;
    args[2].v_type = VAR_UNKNOWN;

    // Lock the text to prevent weird things from happening.  Also disallow
    // switching to another window, it should not be needed and may end up in
    // Insert mode in another buffer.
    ++textlock;

    ctx = get_option_sctx("findfunc");
    जर (ctx != रिक्त)
	current_sctx = *ctx;

    cb = get_findfunc_callback();
    retval = call_callback(cb, -1, &rettv, 2, args);

    current_sctx = saved_sctx;

    --textlock;

    list_T *retlist = रिक्त;

    जर (retval == OK)
    {
	जर (rettv.v_type == VAR_LIST)
	    retlist = list_copy(rettv.vval.v_list, FALSE, FALSE, get_copyID());
	अन्यथा
	    emsg(_(e_invalid_return_type_from_findfunc));

	clear_tv(&rettv);
    }

    परत retlist;
}

/*
 * Find file names matching "pat" using 'findfunc' and return it in "files".
 * Used for expanding the :find, :sfind and :tabfind command argument.
 * Returns OK on success and FAIL otherwise.
 */
    पूर्णांक
expand_findfunc(expand_T *xp, char_u *pat, char_u ***files, पूर्णांक *अंक_Matches)
{
    list_T	*l;
    पूर्णांक		len;

    *अंक_Matches = 0;
    *files = रिक्त;

    l = call_findfunc(pat, VVAL_TRUE);

    जर (l == रिक्त)
	परत FAIL;

    len = list_len(l);
    जर (len == 0)	    // empty List
    {
	list_free(l);
	परत FAIL;
    }

    expand_process_user_list(l, files, अंक_Matches, xp);
    list_free(l);

    परत OK;
}

/*
 * Use 'findfunc' to find file 'findarg'.  The 'count' argument is used to find
 * the n'th matching file.
 */
    स्थिर char_u *
findfunc_find_file(char_u *findarg, पूर्णांक findarg_len, पूर्णांक मोजणी)
{
    list_T	*fname_list;
    char_u	*ret_fname = रिक्त;
    char_u	cc;
    पूर्णांक		fname_मोजणी;

    cc = findarg[findarg_len];
    findarg[findarg_len] = NUL;

    fname_list = call_findfunc(findarg, VVAL_FALSE);
    fname_मोजणी = list_len(fname_list);

    जर (fname_मोजणी == 0)
	semsg(_(e_cant_find_file_str_in_path), findarg);
    अन्यथा
    {
	जर (मोजणी > fname_मोजणी)
	    semsg(_(e_no_more_file_str_found_in_path), findarg);
	अन्यथा
	{
	    listitem_T *li = list_find(fname_list, मोजणी - 1);

	    जर (li != रिक्त)
	    {
		typval_T *tv = &li->li_tv;

		जर (tv->v_type == VAR_STRING && tv->vval.v_string != रिक्त)
		    ret_fname = vim_strsave(tv->vval.v_string);
		अन्यथा जर (tv->v_type == VAR_DICT && tv->vval.v_dict != रिक्त)
		    ret_fname = dict_get_string(tv->vval.v_dict, "word", TRUE);
	    }
	}
    }

    जर (fname_list != रिक्त)
	list_free(fname_list);

    findarg[findarg_len] = cc;

    परत ret_fname;
}

/*
 * Process the 'findfunc' option value.
 * Returns NULL on success and an error message on failure.
 */
    अक्षर *
did_set_findfunc(optset_T *args UNUSED)
{
    पूर्णांक	retval;

    जर (args->os_flags & OPT_LOCAL)
	// buffer-local option set
	retval = option_set_callback_func(curbuf->b_p_ffu, &curbuf->b_ffu_cb);
    अन्यथा
    {
	// global option set
	retval = option_set_callback_func(p_ffu, &ffu_cb);
	// when using :set, free the local callback
	जर (!(args->os_flags & OPT_GLOBAL))
	    free_callback(&curbuf->b_ffu_cb);
    }

    जर (retval == FAIL)
	परत e_invalid_argument;

    // If the option value starts with <SID> or s:, then replace that with
    // the script identifier.
    char_u	**varp = (char_u **)args->os_varp;
    char_u	*name = get_scriptlocal_funcname(*varp);
    जर (name != रिक्त)
    {
	free_string_option(*varp);
	*varp = name;
    }

    परत रिक्त;
}

# जर defined(EXITFREE)
    शून्य
free_findfunc_option(शून्य)
{
    free_callback(&ffu_cb);
}
# endif

/*
 * Mark the global 'findfunc' callback with "copyID" so that it is not
 * garbage collected.
 */
    पूर्णांक
set_ref_in_findfunc(पूर्णांक copyID UNUSED)
{
    पूर्णांक पात = FALSE;
    पात = set_ref_in_callback(&ffu_cb, copyID);
    परत पात;
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
    शून्य
ex_splitview(exarg_T *eap)
{
    win_T	*old_curwin = curwin;
    char_u	*fname = रिक्त;
#ifdef FEAT_BROWSE
    char_u	dot_path[] = ".";
    पूर्णांक		save_cmod_flags = cmdmod.cmod_flags;
#endif
    पूर्णांक		use_tab = eap->cmdidx == CMD_tabedit
		       || eap->cmdidx == CMD_tabfind
		       || eap->cmdidx == CMD_tabnew;

    जर (ERROR_IF_ANY_POPUP_WINDOW)
	परत;

#ifdef FEAT_GUI
    need_mouse_correct = TRUE;
#endif

#ifdef FEAT_QUICKFIX
    // A ":split" in the quickfix window works like ":new".  Don't want two
    // quickfix windows.  But it's OK when doing ":tab split".
    जर (bt_quickfix(curbuf) && cmdmod.cmod_tab == 0)
    {
	जर (eap->cmdidx == CMD_split)
	    eap->cmdidx = CMD_new;
	जर (eap->cmdidx == CMD_vsplit)
	    eap->cmdidx = CMD_vnew;
    }
#endif

    जर (eap->cmdidx == CMD_sfind || eap->cmdidx == CMD_tabfind)
    {
	char_u	*file_to_find = रिक्त;
	अक्षर	*शोध_ctx = रिक्त;

	जर (*get_findfunc() != NUL)
	{
#ifdef FEAT_EVAL
	    fname = findfunc_find_file(eap->arg, (पूर्णांक)STRLEN(eap->arg),
				       eap->addr_मोजणी > 0 ? eap->line2 : 1);
#endif
	}
	अन्यथा
	{
	    fname = find_file_in_path(eap->arg, (पूर्णांक)STRLEN(eap->arg),
					      FNAME_MESS, TRUE, curbuf->b_ffname,
					      &file_to_find, &शोध_ctx);
	    vim_free(file_to_find);
	    vim_findfile_cleanup(शोध_ctx);
	}
	जर (fname == रिक्त)
	    जा theend;
	eap->arg = fname;
    }
#ifdef FEAT_BROWSE
    अन्यथा जर ((cmdmod.cmod_flags & CMOD_BROWSE)
	    && eap->cmdidx != CMD_vnew
	    && eap->cmdidx != CMD_new)
    {
	जर (
# ifdef FEAT_GUI
	    !gui.in_use &&
# endif
		au_has_group((char_u *)"FileExplorer"))
	{
	    // No browsing supported but we do have the file explorer:
	    // Edit the directory.
	    जर (*eap->arg == NUL || !mch_isdir(eap->arg))
		eap->arg = dot_path;
	}
	अन्यथा
	{
	    fname = do_browse(0, (char_u *)(use_tab
			? _("Edit File in new tab page")
			: _("Edit File in new window")),
					  eap->arg, रिक्त, रिक्त, रिक्त, curbuf);
	    जर (fname == रिक्त)
		जा theend;
	    eap->arg = fname;
	}
    }
    cmdmod.cmod_flags &= ~CMOD_BROWSE;	// Don't browse again in do_ecmd().
#endif

    /*
     * Either open new tab page or split the window.
     */
    जर (use_tab)
    {
	जर (win_new_tabpage(cmdmod.cmod_tab != 0 ? cmdmod.cmod_tab
			 : eap->addr_मोजणी == 0 ? 0
					       : (पूर्णांक)eap->line2 + 1) != FAIL)
	{
	    do_exedit(eap, old_curwin);

	    // set the alternate buffer for the window we came from
	    जर (curwin != old_curwin
		    && win_valid(old_curwin)
		    && old_curwin->w_buffer != curbuf
		    && (cmdmod.cmod_flags & CMOD_KEEPALT) == 0)
		old_curwin->w_alt_fnum = curbuf->b_fnum;
	}
    }
    अन्यथा जर (win_split(eap->addr_मोजणी > 0 ? (पूर्णांक)eap->line2 : 0,
				     *eap->cmd == 'v' ? WSP_VERT : 0) != FAIL)
    {
	// Reset 'scrollbind' when editing another file, but keep it when
	// doing ":split" without arguments.
	जर (*eap->arg != NUL)
	    RESET_BINDING(curwin);
	अन्यथा
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
    शून्य
tabpage_new(शून्य)
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
    स्थिर शून्य
ex_tabnext(exarg_T *eap)
{
    पूर्णांक tab_संख्या;

    जर (ERROR_IF_POPUP_WINDOW)
	परत;
    निवड (eap->cmdidx)
    {
	स्थिती CMD_tabfirst:
	स्थिती CMD_tabrewind:
	    goto_tabpage(1);
	    थांबा;
	स्थिती CMD_tablast:
	    goto_tabpage(9999);
	    थांबा;
	स्थिती CMD_tabprevious:
	स्थिती CMD_tabNext:
	    जर (eap->arg && *eap->arg != NUL)
	    {
		char_u *p = eap->arg;
		char_u *p_save = p;

		tab_संख्या = getdigits(&p);
		जर (p == p_save || *p_save == '-' || *p != NUL
			    || tab_संख्या == 0)
		{
		    // No numbers as argument.
		    eap->errmsg = ex_errmsg(e_invalid_argument_str, eap->arg);
		    परत;
		}
	    }
	    अन्यथा
	    {
		जर (eap->addr_मोजणी == 0)
		    tab_संख्या = 1;
		अन्यथा
		{
		    tab_संख्या = eap->line2;
		    जर (tab_संख्या < 1)
		    {
			eap->errmsg = _(e_invalid_range);
			परत;
		    }
		}
	    }
	    goto_tabpage(-tab_संख्या);
	    थांबा;
	पूर्वनिर्धारित: // CMD_tabnext
	    tab_संख्या = get_tabpage_arg(eap);
	    जर (eap->errmsg == रिक्त)
		goto_tabpage(tab_संख्या);
	    थांबा;
    }
}

/*
 * :tabmove command
 */
    स्थिर शून्य
ex_tabmove(exarg_T *eap)
{
    पूर्णांक tab_संख्या;

    tab_संख्या = get_tabpage_arg(eap);
    जर (eap->errmsg == रिक्त)
	tabpage_move(tab_संख्या);
}

/*
 * :tabs command: List tabs and their contents.
 */
    स्थिर शून्य
ex_tabs(exarg_T *eap UNUSED)
{
    tabpage_T	*tp;
    win_T	*wp;
    पूर्णांक		tabcount = 1;

    संदेश_start();
    संदेश_scroll = TRUE;
    साठी (tp = first_tabpage; tp != रिक्त && !got_int; tp = tp->tp_next)
    {
	संदेश_putchar('\n');
	vim_snprintf((अक्षर *)IObuff, IOSIZE, _("Tab page %d"), tabcount++);
	संदेश_outtrans_attr(IObuff, HL_ATTR(HLF_T));
	out_flush();	    // output one line at a time
	ui_breakcheck();

	जर (tp  == curtab)
	    wp = firstwin;
	अन्यथा
	    wp = tp->tp_firstwin;
	साठी ( ; wp != रिक्त && !got_int; wp = wp->w_next)
	{
	    संदेश_putchar('\n');
	    संदेश_putchar(wp == curwin ? '>' : ' ');
	    संदेश_putchar(' ');
	    संदेश_putchar(bufIsChanged(wp->w_buffer) ? '+' : ' ');
	    संदेश_putchar(' ');
	    जर (buf_spname(wp->w_buffer) != रिक्त)
		vim_strncpy(IObuff, buf_spname(wp->w_buffer), IOSIZE - 1);
	    अन्यथा
		home_replace(wp->w_buffer, wp->w_buffer->b_fname,
							IObuff, IOSIZE, TRUE);
	    संदेश_outtrans(IObuff);
	    out_flush();	    // output one line at a time
	    ui_breakcheck();
	}
    }
}

/*
 * ":mode": Set screen mode.
 * If no argument given, just get the screen size and redraw.
 */
    स्थिर शून्य
ex_mode(exarg_T *eap)
{
    जर (*eap->arg == NUL)
	shell_resized();
    अन्यथा
	emsg(_(e_screen_mode_setting_not_supported));
}

/*
 * ":resize".
 * set, increment or decrement current window height
 */
    स्थिर शून्य
ex_resize(exarg_T *eap)
{
    पूर्णांक		n;
    win_T	*wp = curwin;

    जर (eap->addr_मोजणी > 0)
    {
	n = eap->line2;
	साठी (wp = firstwin; wp->w_next != रिक्त && --n > 0; wp = wp->w_next)
	    ;
    }

#ifdef FEAT_GUI
    need_mouse_correct = TRUE;
#endif
    n = म_से_द((अक्षर *)eap->arg);
    जर (cmdmod.cmod_split & WSP_VERT)
    {
	जर (*eap->arg == '-' || *eap->arg == '+')
	    n += wp->w_width;
	अन्यथा जर (n == 0 && eap->arg[0] == NUL)	// default is very wide
	    n = 9999;
	win_setwidth_win(n, wp);
    }
    अन्यथा
    {
	जर (*eap->arg == '-' || *eap->arg == '+')
	    n += wp->w_height;
	अन्यथा जर (n == 0 && eap->arg[0] == NUL)	// default is very high
	    n = 9999;
	win_setheight_win(n, wp);
    }
}

/*
 * ":find [+command] <file>" command.
 */
    स्थिर शून्य
ex_find(exarg_T *eap)
{
    जर (!check_can_set_curbuf_forceit(eap->forceit))
	परत;

    char_u	*fname = रिक्त;
    पूर्णांक		मोजणी;
    char_u	*file_to_find = रिक्त;
    अक्षर	*शोध_ctx = रिक्त;

    जर (*get_findfunc() != NUL)
    {
#ifdef FEAT_EVAL
	fname = findfunc_find_file(eap->arg, (पूर्णांक)STRLEN(eap->arg),
					eap->addr_मोजणी > 0 ? eap->line2 : 1);
#endif
    }
    अन्यथा
    {
	fname = find_file_in_path(eap->arg, (पूर्णांक)STRLEN(eap->arg), FNAME_MESS,
			       TRUE, curbuf->b_ffname, &file_to_find, &शोध_ctx);
	जर (eap->addr_मोजणी > 0)
	{
	    // Repeat finding the file "count" times.  This matters when it appears
	    // several times in the path.
	    मोजणी = eap->line2;
	    पर्यंत (fname != रिक्त && --मोजणी > 0)
	    {
		vim_free(fname);
		fname = find_file_in_path(रिक्त, 0, FNAME_MESS,
			      FALSE, curbuf->b_ffname, &file_to_find, &शोध_ctx);
	    }
	}
	VIM_CLEAR(file_to_find);
	vim_findfile_cleanup(शोध_ctx);
    }

    जर (fname == रिक्त)
	परत;

    eap->arg = fname;
    do_exedit(eap, रिक्त);
    vim_free(fname);
}

/*
 * ":open" simulation: for now works just like ":visual".
 */
    स्थिर शून्य
ex_open(exarg_T *eap)
{
    regmatch_T	regmatch;
    char_u	*p;

#ifdef FEAT_EVAL
    जर (not_in_vim9(eap, false) == FAIL)
	परत;
#endif
    curwin->w_cursor.lnum = eap->line2;
    beginline(BL_SOL | BL_FIX);
    जर (*eap->arg == '/')
    {
	// ":open /pattern/": put cursor in column found with pattern
	++eap->arg;
	p = skip_regexp(eap->arg, '/', magic_isset());
	*p = NUL;
	regmatch.regprog = vim_regcomp(eap->arg, magic_isset() ? RE_MAGIC : 0);
	जर (regmatch.regprog != रिक्त)
	{
	    // make a copy of the line, when searching for a mark it might be
	    // flushed
	    char_u *ओळ = vim_strsave(ml_get_curline());

	    regmatch.rm_ic = p_ic;
	    जर (vim_regexec(&regmatch, ओळ, (colnr_T)0))
		curwin->w_cursor.col = (colnr_T)(regmatch.startp[0] - ओळ);
	    अन्यथा
		emsg(_(e_no_match));
	    vim_regfree(regmatch.regprog);
	    vim_free(ओळ);
	}
	// Move to the NUL, ignore any other arguments.
	eap->arg += STRLEN(eap->arg);
    }
    check_cursor();

    eap->cmdidx = CMD_visual;
    do_exedit(eap, रिक्त);
}

/*
 * ":edit", ":badd", ":balt", ":visual".
 */
    स्थिर शून्य
ex_edit(exarg_T *eap)
{
    char_u *ffname = eap->cmdidx == CMD_enew ? रिक्त : eap->arg;

    // Exclude commands which keep the window's current buffer
    जर (
	    eap->cmdidx != CMD_badd
	    && eap->cmdidx != CMD_balt
	    // All other commands must obey 'winfixbuf' / ! rules
	    && (is_other_file(0, ffname) && !check_can_set_curbuf_forceit(eap->forceit))
    )
	परत;

    do_exedit(eap, रिक्त);
}

/*
 * ":edit <file>" command and alike.
 */
    शून्य
do_exedit(
    exarg_T	*eap,
    win_T	*old_curwin)	    // curwin before doing a split or NULL
{
    पूर्णांक		n;
    पूर्णांक		need_hide;
    पूर्णांक		exmode_was = exmode_active;

    जर ((eap->cmdidx != CMD_pedit && ERROR_IF_POPUP_WINDOW)
						 || ERROR_IF_TERM_POPUP_WINDOW)
	परत;
    /*
     * ":vi" command ends Ex mode.
     */
    जर (exmode_active && (eap->cmdidx == CMD_visual
						|| eap->cmdidx == CMD_view))
    {
	exmode_active = FALSE;
	ex_pressedreturn = FALSE;
	जर (*eap->arg == NUL)
	{
	    // Special case:  ":global/pat/visual\NLvi-commands"
	    जर (global_busy)
	    {
		जर (eap->nextcmd != रिक्त)
		{
		    stuffReadbuff(eap->nextcmd);
		    eap->nextcmd = रिक्त;
		}

		जर (exmode_was != EXMODE_VIM)
		    settmode(TMODE_RAW);
		पूर्णांक save_RedrawingDisabled = RedrawingDisabled;
		RedrawingDisabled = 0;
		पूर्णांक save_nwr = no_wait_return;
		no_wait_return = 0;
		need_wait_return = FALSE;
		पूर्णांक save_ms = संदेश_scroll;
		संदेश_scroll = 0;
#ifdef FEAT_GUI
		पूर्णांक save_he = hold_gui_events;
		hold_gui_events = 0;
#endif
		set_must_redraw(UPD_CLEAR);
		pending_exmode_active = TRUE;

		main_loop(FALSE, TRUE);

		pending_exmode_active = FALSE;
		RedrawingDisabled = save_RedrawingDisabled;
		no_wait_return = save_nwr;
		संदेश_scroll = save_ms;
#ifdef FEAT_GUI
		hold_gui_events = save_he;
#endif
	    }
	    परत;
	}
    }

    जर ((eap->cmdidx == CMD_new
		|| eap->cmdidx == CMD_tabnew
		|| eap->cmdidx == CMD_tabedit
		|| eap->cmdidx == CMD_vnew) && *eap->arg == NUL)
    {
	// ":new" or ":tabnew" without argument: edit a new empty buffer
	setpcmark();
	(शून्य)do_ecmd(0, रिक्त, रिक्त, eap, ECMD_ONE,
		      ECMD_HIDE + (eap->forceit ? ECMD_FORCEIT : 0),
		      old_curwin == रिक्त ? curwin : रिक्त);
    }
    अन्यथा जर ((eap->cmdidx != CMD_split && eap->cmdidx != CMD_vsplit)
	    || *eap->arg != NUL
#ifdef FEAT_BROWSE
	    || (cmdmod.cmod_flags & CMOD_BROWSE)
#endif
	    )
    {
	// Can't edit another file when "textlock" or "curbuf_lock" is set.
	// Only ":edit" or ":script" can bring us here, others are stopped
	// earlier.
	जर (*eap->arg != NUL && text_or_buf_locked())
	    परत;

	n = readonlymode;
	जर (eap->cmdidx == CMD_view || eap->cmdidx == CMD_sview)
	    readonlymode = TRUE;
	अन्यथा जर (eap->cmdidx == CMD_enew)
	    readonlymode = FALSE;   // 'readonly' doesn't make sense in an
				    // empty buffer
	जर (eap->cmdidx != CMD_balt && eap->cmdidx != CMD_badd)
	    setpcmark();
	जर (do_ecmd(0, (eap->cmdidx == CMD_enew ? रिक्त : eap->arg),
		    रिक्त, eap,
		    // ":edit" goes to first line if Vi compatible
		    (*eap->arg == NUL && eap->do_ecmd_lnum == 0
				      && vim_strchr(p_cpo, CPO_GOTO1) != रिक्त)
					       ? ECMD_ONE : eap->do_ecmd_lnum,
		    (buf_hide(curbuf) ? ECMD_HIDE : 0)
		    + (eap->forceit ? ECMD_FORCEIT : 0)
		      // after a split we can use an existing buffer
		    + (old_curwin != रिक्त ? ECMD_OLDBUF : 0)
		    + (eap->cmdidx == CMD_badd ? ECMD_ADDBUF : 0)
		    + (eap->cmdidx == CMD_balt ? ECMD_ALTBUF : 0)
		    , old_curwin == रिक्त ? curwin : रिक्त) == FAIL)
	{
	    // Editing the file failed.  If the window was split, close it.
	    जर (old_curwin != रिक्त)
	    {
		need_hide = (curbufIsChanged() && curbuf->b_nwindows <= 1);
		जर (!need_hide || buf_hide(curbuf))
		{
#जर defined(FEAT_EVAL)
		    cleanup_T   cs;

		    // Reset the error/interrupt/exception state here so that
		    // aborting() returns FALSE when closing a window.
		    enter_cleanup(&cs);
#endif
#ifdef FEAT_GUI
		    need_mouse_correct = TRUE;
#endif
		    win_close(curwin, !need_hide && !buf_hide(curbuf));

#जर defined(FEAT_EVAL)
		    // Restore the error/interrupt/exception state if not
		    // discarded by a new aborting error, interrupt, or
		    // uncaught exception.
		    leave_cleanup(&cs);
#endif
		}
	    }
	}
	अन्यथा जर (readonlymode && curbuf->b_nwindows == 1)
	{
	    // When editing an already visited buffer, 'readonly' won't be set
	    // but the previous value is kept.  With ":view" and ":sview" we
	    // want the  file to be readonly, except when another window is
	    // editing the same buffer.
	    curbuf->b_p_ro = TRUE;
	}
	readonlymode = n;
    }
    अन्यथा
    {
	जर (eap->do_ecmd_cmd != रिक्त)
	    do_cmd_argument(eap->do_ecmd_cmd);
	n = curwin->w_arg_idx_invalid;
	check_arg_idx(curwin);
	जर (n != curwin->w_arg_idx_invalid)
	    maketitle();
    }

    /*
     * if ":split file" worked, set alternate file name in old window to new
     * file
     */
    जर (old_curwin != रिक्त
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
    स्थिर शून्य
ex_nogui(exarg_T *eap)
{
    eap->errmsg = _(e_gui_cannot_be_used_not_enabled_at_compile_time);
}
#endif

#जर defined(FEAT_GUI_MSWIN) && defined(FEAT_MENU) && defined(FEAT_TEAROFF)
    स्थिर शून्य
ex_tearoff(exarg_T *eap)
{
    gui_make_tearoff(eap->arg);
}
#endif

#जर (defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_GTK) \
	|| defined(FEAT_TERM_POPUP_MENU)) && defined(FEAT_MENU)
    स्थिर शून्य
ex_popup(exarg_T *eap)
{
# जर defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_GTK)
    जर (gui.in_use)
	gui_make_popup(eap->arg, eap->forceit);
#  ifdef FEAT_TERM_POPUP_MENU
    अन्यथा
#  endif
# endif
# ifdef FEAT_TERM_POPUP_MENU
	pum_make_popup(eap->arg, eap->forceit);
# endif
}
#endif

    स्थिर शून्य
ex_swapname(exarg_T *eap UNUSED)
{
    जर (curbuf->b_ml.ml_mfp == रिक्त || curbuf->b_ml.ml_mfp->mf_fname == रिक्त)
	संदेश(_("No swap file"));
    अन्यथा
	संदेश((अक्षर *)curbuf->b_ml.ml_mfp->mf_fname);
}

/*
 * ":syncbind" forces all 'scrollbind' windows to have the same relative
 * offset.
 * (1998-11-02 16:21:01  R. Edward Ralston <eralston@computer.org>)
 */
    स्थिर शून्य
ex_syncbind(exarg_T *eap UNUSED)
{
    win_T	*wp;
    win_T	*save_curwin = curwin;
    buf_T	*save_curbuf = curbuf;
    दीर्घ	topline;
    दीर्घ	y;
    linenr_T	old_linenr = curwin->w_cursor.lnum;

    setpcmark();

    /*
     * determine max topline
     */
    जर (curwin->w_p_scb)
    {
	topline = curwin->w_topline;
	FOR_ALL_WINDOWS(wp)
	{
	    जर (wp->w_p_scb && wp->w_buffer)
	    {
		y = wp->w_buffer->b_ml.ml_ओळ_मोजणी - get_scrolloff_मूल्य();
		जर (topline > y)
		    topline = y;
	    }
	}
	जर (topline < 1)
	    topline = 1;
    }
    अन्यथा
    {
	topline = 1;
    }


    /*
     * Set all scrollbind windows to the same topline.
     */
    FOR_ALL_WINDOWS(curwin)
    {
	जर (curwin->w_p_scb)
	{
	    curbuf = curwin->w_buffer;
	    y = topline - curwin->w_topline;
	    जर (y > 0)
		scrollup(y, TRUE);
	    अन्यथा
		scrolldown(-y, TRUE);
	    curwin->w_scbind_pos = topline;
	    redraw_later(UPD_VALID);
	    cursor_correct();
	    curwin->w_redr_status = true;
	}
    }
    curwin = save_curwin;
    curbuf = save_curbuf;
    जर (curwin->w_p_scb)
    {
	did_syncbind = TRUE;
	checkpcmark();
	जर (old_linenr != curwin->w_cursor.lnum)
	{
	    char_u ctrl_o[2];

	    ctrl_o[0] = Ctrl_O;
	    ctrl_o[1] = 0;
	    ins_typebuf(ctrl_o, REMAP_NONE, 0, TRUE, FALSE);
	}
    }
}


    स्थिर शून्य
ex_read(exarg_T *eap)
{
    पूर्णांक		i;
    पूर्णांक		empty = (curbuf->b_ml.ml_flags & ML_EMPTY);
    linenr_T	lnum;

    जर (eap->usefilter)			// :r!cmd
    {
	do_bang(1, eap, FALSE, FALSE, TRUE);
	परत;
    }

    जर (u_save(eap->line2, (linenr_T)(eap->line2 + 1)) == FAIL)
	परत;

#ifdef FEAT_BROWSE
    जर (cmdmod.cmod_flags & CMOD_BROWSE)
    {
	char_u *browseFile;

	browseFile = do_browse(0, (char_u *)_("Append File"), eap->arg,
		रिक्त, रिक्त, रिक्त, curbuf);
	जर (browseFile != रिक्त)
	{
	    i = readfile(browseFile, रिक्त,
		    eap->line2, (linenr_T)0, (linenr_T)MAXLNUM, eap, 0);
	    vim_free(browseFile);
	}
	अन्यथा
	    i = OK;
    }
    अन्यथा
#endif
	जर (*eap->arg == NUL)
	{
	    जर (check_fname() == FAIL)	// check for no file name
		परत;
	    i = readfile(curbuf->b_ffname, curbuf->b_fname,
		    eap->line2, (linenr_T)0, (linenr_T)MAXLNUM, eap, 0);
	}
	अन्यथा
	{
	    जर (vim_strchr(p_cpo, CPO_ALTREAD) != रिक्त)
		(शून्य)setaltfname(eap->arg, eap->arg, (linenr_T)1);
	    i = readfile(eap->arg, रिक्त,
		    eap->line2, (linenr_T)0, (linenr_T)MAXLNUM, eap, 0);

	}
    जर (i != OK)
    {
#जर defined(FEAT_EVAL)
	जर (!aborting())
#endif
	    semsg(_(e_cant_open_file_str), eap->arg);
    }
    अन्यथा
    {
	जर (empty && exmode_active)
	{
	    // Delete the empty line that remains.  Historically ex does
	    // this but vi doesn't.
	    जर (eap->line2 == 0)
		lnum = curbuf->b_ml.ml_ओळ_मोजणी;
	    अन्यथा
		lnum = 1;
	    जर (*ml_get(lnum) == NUL && u_savedel(lnum, 1L) == OK)
	    {
		ml_delete(lnum);
		जर (curwin->w_cursor.lnum > 1
			&& curwin->w_cursor.lnum >= lnum)
		    --curwin->w_cursor.lnum;
		deleted_ओळी_mark(lnum, 1L);
	    }
	}
	redraw_curbuf_later(UPD_VALID);
    }
}

स्थिर char_u	*prev_dir = रिक्त;

#जर defined(EXITFREE)
    शून्य
free_cd_dir(शून्य)
{
    VIM_CLEAR(prev_dir);
    VIM_CLEAR(globaldir);
}
#endif

/*
 * Get the previous directory for the given chdir scope.
 */
    स्थिर char_u *
get_prevdir(cdscope_T scope)
{
    जर (scope == CDSCOPE_WINDOW)
	परत curwin->w_prevdir;
    अन्यथा जर (scope == CDSCOPE_TABPAGE)
	परत curtab->tp_prevdir;
    परत prev_dir;
}

/*
 * Deal with the side effects of changing the current directory.
 * When 'scope' is CDSCOPE_TABPAGE then this was after an ":tcd" command.
 * When 'scope' is CDSCOPE_WINDOW then this was after an ":lcd" command.
 */
    शून्य
post_chdir(cdscope_T scope)
{
    जर (scope != CDSCOPE_WINDOW)
	// Clear tab local directory for both :cd and :tcd
	VIM_CLEAR(curtab->tp_localdir);
    VIM_CLEAR(curwin->w_localdir);
    जर (scope != CDSCOPE_GLOBAL)
    {
	char_u	*pdir = get_prevdir(scope);

	// If still in the global directory, need to remember current
	// directory as the global directory.
	जर (globaldir == रिक्त && pdir != रिक्त)
	    globaldir = vim_strsave(pdir);

	// Remember this local directory for the window.
	जर (mch_dirname(NameBuff, MAXPATHL) == OK)
	{
	    जर (scope == CDSCOPE_TABPAGE)
		curtab->tp_localdir = vim_strsave(NameBuff);
	    अन्यथा
		curwin->w_localdir = vim_strsave(NameBuff);
	}
    }
    अन्यथा
    {
	// We are now in the global directory, no need to remember its name.
	VIM_CLEAR(globaldir);
    }

    last_chdir_reason = रिक्त;
    shorten_fnames(vim_strchr(p_cpo, CPO_NOSYMLINKS) == रिक्त);
}

/*
 * Trigger DirChangedPre for "acmd_fname" with directory "new_dir".
 */
    शून्य
trigger_DirChangedPre(char_u *acmd_fname, char_u *new_dir)
{
#ifdef FEAT_EVAL
    dict_T	    *v_event;
    save_v_event_T  save_v_event;

    v_event = get_v_event(&save_v_event);
    (शून्य)dict_add_string(v_event, "directory", new_dir);
    dict_set_वस्तुसंच_ro(v_event);
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
    पूर्णांक
changedir_func(
	char_u		*new_dir,
	पूर्णांक		forceit,
	cdscope_T	scope)
{
    char_u	*pdir = रिक्त;
    पूर्णांक		dir_differs;
    char_u	*acmd_fname = रिक्त;
    char_u	**pp;
    char_u	*tofree;

    जर (new_dir == रिक्त || allbuf_locked())
	परत FALSE;

    जर (vim_strchr(p_cpo, CPO_CHDIR) != रिक्त && curbufIsChanged() && !forceit)
    {
	emsg(_(e_cannot_change_directory_buffer_is_modified_add_bang_to_override));
	परत FALSE;
    }

    // ":cd -": Change to previous directory
    जर (STRCMP(new_dir, "-") == 0)
    {
	pdir = get_prevdir(scope);
	जर (pdir == रिक्त)
	{
	    emsg(_(e_no_previous_directory));
	    परत FALSE;
	}
	new_dir = pdir;
    }

    // Save current directory for next ":cd -"
    जर (mch_dirname(NameBuff, MAXPATHL) == OK)
	pdir = vim_strsave(NameBuff);
    अन्यथा
	pdir = रिक्त;

    // For UNIX ":cd" means: go to home directory.
    // On other systems too if 'cdhome' is set.
#जर defined(UNIX) || defined(VMS)
    जर (*new_dir == NUL)
#अन्यथा
    जर (*new_dir == NUL && p_cdh)
#endif
    {
	// use NameBuff for home directory name
#ifdef VMS
	char_u	*p;

	p = mch_getenv((char_u *)"SYS$LOGIN");
	जर (p == रिक्त || *p == NUL)	// empty is the same as not set
	    NameBuff[0] = NUL;
	अन्यथा
	    vim_strncpy(NameBuff, p, MAXPATHL - 1);
#अन्यथा
	expand_env((char_u *)"$HOME", NameBuff, MAXPATHL);
#endif
	new_dir = NameBuff;
    }
    dir_differs = pdir == रिक्त
			    || pathcmp((अक्षर *)pdir, (अक्षर *)new_dir, -1) != 0;
    जर (dir_differs)
    {
	जर (scope == CDSCOPE_WINDOW)
	    acmd_fname = (char_u *)"window";
	अन्यथा जर (scope == CDSCOPE_TABPAGE)
	    acmd_fname = (char_u *)"tabpage";
	अन्यथा
	    acmd_fname = (char_u *)"global";
	trigger_DirChangedPre(acmd_fname, new_dir);

	जर (vim_chdir(new_dir))
	{
	    emsg(_(e_command_failed));
	    vim_free(pdir);
	    परत FALSE;
	}
    }

    जर (scope == CDSCOPE_WINDOW)
	pp = &curwin->w_prevdir;
    अन्यथा जर (scope == CDSCOPE_TABPAGE)
	pp = &curtab->tp_prevdir;
    अन्यथा
	pp = &prev_dir;
    tofree = *pp;  // new_dir may use this
    *pp = pdir;

    post_chdir(scope);

    जर (dir_differs)
	apply_autocmds(EVENT_DIRCHANGED, acmd_fname, new_dir, FALSE, curbuf);
    vim_free(tofree);
    परत TRUE;
}

/*
 * ":cd", ":tcd", ":lcd", ":chdir" ":tchdir" and ":lchdir".
 */
    शून्य
ex_cd(exarg_T *eap)
{
    char_u	*new_dir;

    new_dir = eap->arg;
#जर !defined(UNIX) && !defined(VMS)
    // for non-UNIX ":cd" means: print current directory unless 'cdhome' is set
    जर (*new_dir == NUL && !p_cdh)
    {
	ex_pwd(रिक्त);
	परत;
    }
#endif

    cdscope_T	scope = CDSCOPE_GLOBAL;

    जर (eap->cmdidx == CMD_lcd || eap->cmdidx == CMD_lchdir)
	scope = CDSCOPE_WINDOW;
    अन्यथा जर (eap->cmdidx == CMD_tcd || eap->cmdidx == CMD_tchdir)
	scope = CDSCOPE_TABPAGE;

    जर (changedir_func(new_dir, eap->forceit, scope))
    {
	// Echo the new current directory if the command was typed.
	जर (किल्ली_Typed || p_verbose >= 5)
	    ex_pwd(eap);
    }
}

/*
 * ":pwd".
 */
    स्थिर शून्य
ex_pwd(exarg_T *eap UNUSED)
{
    जर (mch_dirname(NameBuff, MAXPATHL) == OK)
    {
#ifdef BACKSLASH_IN_FILENAME
	slash_adjust(NameBuff);
#endif
	जर (p_verbose > 0)
	{
	    अक्षर *context = "global";

	    जर (last_chdir_reason != रिक्त)
		context = last_chdir_reason;
	    अन्यथा जर (curwin->w_localdir != रिक्त)
		context = "window";
	    अन्यथा जर (curtab->tp_localdir != रिक्त)
		context = "tabpage";
	    smsg("[%s] %s", context, (अक्षर *)NameBuff);
	}
	अन्यथा
	    संदेश((अक्षर *)NameBuff);
    }
    अन्यथा
	emsg(_(e_directory_unknown));
}

/*
 * ":=".
 */
    स्थिर शून्य
ex_equal(exarg_T *eap)
{
    smsg("%ld", (दीर्घ)eap->line2);
    ex_may_print(eap);
}

    स्थिर शून्य
ex_sleep(exarg_T *eap)
{
    पूर्णांक		n;
    दीर्घ	len;

    जर (cursor_valid())
    {
	n = W_WINROW(curwin) + curwin->w_wrow - संदेश_scrolled;
	जर (n >= 0)
	    windgoto(n, curwin->w_wincol + curwin->w_wcol);
    }

    len = eap->line2;
    निवड (*eap->arg)
    {
	स्थिती 'm': थांबा;
	स्थिती NUL: len *= 1000L; थांबा;
	पूर्वनिर्धारित: semsg(_(e_invalid_argument_str), eap->arg); परत;
    }

    // Hide the cursor if invoked with !
    do_sleep(len, eap->forceit);
}

/*
 * Sleep for "msec" milliseconds, but keep checking for a CTRL-C every second.
 * Hide the cursor if "hide_cursor" is TRUE.
 */
    शून्य
do_sleep(दीर्घ msec, पूर्णांक hide_cursor)
{
    दीर्घ	done = 0;
    दीर्घ	wait_now;
#ifdef ELAPSED_FUNC
    elapsed_T	start_tv;

    // Remember at what time we started, so that we know how much longer we
    // should wait after waiting for a bit.
    ELAPSED_INIT(start_tv);
#endif

    जर (hide_cursor)
	cursor_sleep();
    अन्यथा
	cursor_on();

    out_flush_cursor(FALSE, FALSE);
    पर्यंत (!got_int && done < msec)
    {
	wait_now = msec - done > 1000L ? 1000L : msec - done;
#ifdef FEAT_TIMERS
	{
	    दीर्घ    due_time = check_due_timer();

	    जर (due_time > 0 && due_time < wait_now)
		wait_now = due_time;
	}
#endif
#ifdef FEAT_JOB_CHANNEL
	जर (has_any_channel() && wait_now > 20L)
	    wait_now = 20L;
#endif
#ifdef FEAT_SOUND
	जर (has_any_sound_callback() && wait_now > 20L)
	    wait_now = 20L;
#endif
	ui_delay(wait_now, TRUE);

#ifdef FEAT_JOB_CHANNEL
	जर (has_any_channel())
	    ui_breakcheck_force(TRUE);
	अन्यथा
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
#अन्यथा
	// guesstimate time passed (will actually be more)
	done += wait_now;
#endif
    }

    // If CTRL-C was typed to interrupt the sleep, drop the CTRL-C from the
    // input buffer, otherwise a following call to input() fails.
    जर (got_int)
	(शून्य)vpeekc();

    जर (hide_cursor)
	cursor_unsleep();
}

/*
 * ":winsize" command (obsolete).
 */
    स्थिर शून्य
ex_winsize(exarg_T *eap)
{
    पूर्णांक		w, h;
    char_u	*arg = eap->arg;
    char_u	*p;

    जर (!SAFE_isdigit(*arg))
    {
	semsg(_(e_invalid_argument_str), arg);
	परत;
    }
    w = getdigits(&arg);
    arg = skipwhite(arg);
    p = arg;
    h = getdigits(&arg);
    जर (*p != NUL && *arg == NUL)
	set_shellsize(w, h, TRUE);
    अन्यथा
	emsg(_(e_winsize_requires_two_संख्या_arguments));
}

    स्थिर शून्य
ex_wincmd(exarg_T *eap)
{
    पूर्णांक		xchar = NUL;
    char_u	*p;

    जर (*eap->arg == 'g' || *eap->arg == Ctrl_G)
    {
	// CTRL-W g and CTRL-W CTRL-G  have an extra command character
	जर (eap->arg[1] == NUL)
	{
	    emsg(_(e_invalid_argument));
	    परत;
	}
	xchar = eap->arg[1];
	p = eap->arg + 2;
    }
    अन्यथा
	p = eap->arg + 1;

    set_nextcmd(eap, p);
    p = skipwhite(p);
    जर (*p != NUL && *p != (
#ifdef FEAT_EVAL
	    in_vim9script() ? '#' :
#endif
		'"')
	    && eap->nextcmd == रिक्त)
	emsg(_(e_invalid_argument));
    अन्यथा जर (!eap->skip)
    {
	// Pass flags on for ":vertical wincmd ]".
	postponed_split_flags = cmdmod.cmod_split;
	postponed_split_tab = cmdmod.cmod_tab;
	do_window(*eap->arg, eap->addr_मोजणी > 0 ? eap->line2 : 0L, xchar);
	postponed_split_flags = 0;
	postponed_split_tab = 0;
    }
}

#जर defined(FEAT_GUI) || defined(UNIX) || defined(VMS) || defined(MSWIN)
/*
 * ":winpos".
 */
    स्थिर शून्य
ex_winpos(exarg_T *eap)
{
    पूर्णांक		x, y;
    char_u	*arg = eap->arg;
    char_u	*p;

    जर (*arg == NUL)
    {
# जर defined(FEAT_GUI) || defined(MSWIN)
#  ifdef VIMDLL
	जर (gui.in_use ? gui_mch_get_winpos(&x, &y) != FAIL :
		mch_get_winpos(&x, &y) != FAIL)
#  elif defined(FEAT_GUI)
	जर (gui.in_use && gui_mch_get_winpos(&x, &y) != FAIL)
#  अन्यथा
	जर (mch_get_winpos(&x, &y) != FAIL)
#  endif
	{
	    प्र_लिखो((अक्षर *)IObuff, _("Window position: X %d, Y %d"), x, y);
	    संदेश((अक्षर *)IObuff);
	}
	अन्यथा
# endif
	    emsg(_(e_obtaining_window_position_not_implemented_for_this_platform));
    }
    अन्यथा
    {
	x = getdigits(&arg);
	arg = skipwhite(arg);
	p = arg;
	y = getdigits(&arg);
	जर (*p == NUL || *arg != NUL)
	{
	    emsg(_(e_winpos_requires_two_संख्या_arguments));
	    परत;
	}
# ifdef FEAT_GUI
	जर (gui.in_use)
	    gui_mch_set_winpos(x, y);
	अन्यथा जर (gui.starting)
	{
	    // Remember the coordinates for when the window is opened.
	    gui_win_x = x;
	    gui_win_y = y;
	}
#  जर defined(HAVE_TGETENT) || defined(VIMDLL)
	अन्यथा
#  endif
# endif
# जर defined(MSWIN) && (!defined(FEAT_GUI) || defined(VIMDLL))
	    mch_set_winpos(x, y);
# endif
# ifdef HAVE_TGETENT
	जर (*T_CWP)
	    term_set_winpos(x, y);
# endif
    }
}
#endif

/*
 * Handle command that work like operators: ":delete", ":yank", ":>" and ":<".
 */
    स्थिर शून्य
ex_operators(exarg_T *eap)
{
    oparg_T	oa;

    clear_oparg(&oa);
    oa.regname = eap->regname;
    oa.start.lnum = eap->line1;
    oa.end.lnum = eap->line2;
    oa.ओळ_मोजणी = eap->line2 - eap->line1 + 1;
    oa.motion_type = MLINE;
    virtual_op = FALSE;
    जर (eap->cmdidx != CMD_yank)	// position cursor for undo
    {
	setpcmark();
	curwin->w_cursor.lnum = eap->line1;
	beginline(BL_SOL | BL_FIX);
    }

    जर (VIsual_active)
	end_visual_mode();

    निवड (eap->cmdidx)
    {
	स्थिती CMD_delete:
	    oa.op_type = OP_DELETE;
	    op_delete(&oa);
	    थांबा;

	स्थिती CMD_yank:
	    oa.op_type = OP_YANK;
	    (शून्य)op_yank(&oa, FALSE, TRUE);
	    थांबा;

	पूर्वनिर्धारित:    // CMD_rshift or CMD_lshift
	    जर (
#ifdef FEAT_RIGHTLEFT
		(eap->cmdidx == CMD_rshift) ^ curwin->w_p_rl
#अन्यथा
		eap->cmdidx == CMD_rshift
#endif
						)
		oa.op_type = OP_RSHIFT;
	    अन्यथा
		oa.op_type = OP_LSHIFT;
	    op_shift(&oa, FALSE, eap->amount);
	    थांबा;
    }
    virtual_op = MAYBE;
    ex_may_print(eap);
}

/*
 * ":put".
 */
    स्थिर शून्य
ex_put(exarg_T *eap)
{
    // ":0put" works like ":1put!".
    जर (eap->line2 == 0)
    {
	eap->line2 = 1;
	eap->forceit = TRUE;
    }
    curwin->w_cursor.lnum = eap->line2;
    check_cursor_col();
    do_put(eap->regname, रिक्त, eap->forceit ? BACKWARD : FORWARD, 1L,
						       PUT_ओळ|PUT_CURSLINE);
}

/*
 * ":iput".
 */
    स्थिर शून्य
ex_iput(exarg_T *eap)
{
    // ":0iput" works like ":1iput!".
    जर (eap->line2 == 0)
    {
	eap->line2 = 1;
	eap->forceit = TRUE;
    }
    curwin->w_cursor.lnum = eap->line2;
    check_cursor_col();
    do_put(eap->regname, रिक्त, eap->forceit ? BACKWARD : FORWARD, 1L,
						      PUT_ओळ|PUT_CURSLINE
						      |PUT_FIXINDENT);
}

/*
 * Handle ":copy" and ":move".
 */
    स्थिर शून्य
ex_copymove(exarg_T *eap)
{
    दीर्घ	n;

#ifdef FEAT_EVAL
    जर (not_in_vim9(eap, false) == FAIL)
	परत;
#endif
    n = get_address(eap, &eap->arg, eap->addr_type, FALSE, FALSE, FALSE, 1);
    जर (eap->arg == रिक्त)	    // error detected
    {
	eap->nextcmd = रिक्त;
	परत;
    }
    get_flags(eap);

    /*
     * move or copy lines from 'eap->line1'-'eap->line2' to below line 'n'
     */
    जर (n == MAXLNUM || n < 0 || n > curbuf->b_ml.ml_ओळ_मोजणी)
    {
	emsg(_(e_invalid_range));
	परत;
    }

    जर (eap->cmdidx == CMD_move)
    {
	जर (do_move(eap->line1, eap->line2, n) == FAIL)
	    परत;
    }
    अन्यथा
	ex_copy(eap->line1, eap->line2, n);
    u_clearline();
    beginline(BL_SOL | BL_FIX);
    ex_may_print(eap);
}

/*
 * Print the current line if flags were given to the Ex command.
 */
    शून्य
ex_may_print(exarg_T *eap)
{
    जर (eap->flags != 0)
    {
	print_ओळ(curwin->w_cursor.lnum, (eap->flags & EXFLAG_NR),
						  (eap->flags & EXFLAG_LIST));
	ex_no_reprint = TRUE;
    }
}

/*
 * ":smagic" and ":snomagic".
 */
    स्थिर शून्य
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
    स्थिर शून्य
ex_join(exarg_T *eap)
{
    curwin->w_cursor.lnum = eap->line1;
    जर (eap->line1 == eap->line2)
    {
	जर (eap->addr_मोजणी >= 2)   // :2,2join does nothing
	    परत;
	जर (eap->line2 == curbuf->b_ml.ml_ओळ_मोजणी)
	{
	    beep_flush();
	    परत;
	}
	++eap->line2;
    }
    (शून्य)do_join(eap->line2 - eap->line1 + 1, !eap->forceit, TRUE, TRUE, TRUE);
    beginline(BL_WHITE | BL_FIX);
    ex_may_print(eap);
}

/*
 * ":[addr]@r" or ":[addr]*r": execute register
 */
    स्थिर शून्य
ex_at(exarg_T *eap)
{
    पूर्णांक		c;
    पूर्णांक		prev_len = typebuf.tb_len;

    curwin->w_cursor.lnum = eap->line2;
    check_cursor_col();

#ifdef USE_ON_FLY_SCROLL
    dont_scroll = TRUE;		// disallow scrolling here
#endif

    // get the register name.  No name means to use the previous one
    c = *eap->arg;
    जर (c == NUL || (c == '*' && *eap->cmd == '*'))
	c = '@';
    // Put the register in the typeahead buffer with the "silent" flag.
    जर (do_execreg(c, TRUE, vim_strchr(p_cpo, CPO_EXECBUF) != रिक्त, TRUE)
								      == FAIL)
    {
	beep_flush();
	परत;
    }

    पूर्णांक	save_efr = exec_from_reg;

    exec_from_reg = TRUE;

    /*
     * Execute from the typeahead buffer.
     * Continue until the stuff buffer is empty and all added characters
     * have been consumed.
     */
    पर्यंत (!stuff_empty() || typebuf.tb_len > prev_len)
	(शून्य)do_cmdline(रिक्त, getexline, रिक्त, DOCMD_NOWAIT|DOCMD_VERBOSE);

    exec_from_reg = save_efr;
}

/*
 * ":!".
 */
    स्थिर शून्य
ex_bang(exarg_T *eap)
{
    do_bang(eap->addr_मोजणी, eap, eap->forceit, TRUE, TRUE);
}

/*
 * ":undo".
 */
    स्थिर शून्य
ex_undo(exarg_T *eap)
{
    जर (eap->addr_मोजणी == 1)	    // :undo 123
	undo_time(eap->line2, FALSE, FALSE, TRUE);
    अन्यथा
	u_undo(1);
}

#ifdef FEAT_PERSISTENT_UNDO
    स्थिर शून्य
ex_wundo(exarg_T *eap)
{
    char_u hash[UNDO_HASH_SIZE];

    u_compute_hash(hash);
    u_write_undo(eap->arg, eap->forceit, curbuf, hash);
}

    स्थिर शून्य
ex_rundo(exarg_T *eap)
{
    char_u hash[UNDO_HASH_SIZE];

    u_compute_hash(hash);
    u_read_undo(eap->arg, hash, रिक्त);
}
#endif

/*
 * ":redo".
 */
    स्थिर शून्य
ex_redo(exarg_T *eap UNUSED)
{
    u_redo(1);
}

/*
 * ":earlier" and ":later".
 */
    स्थिर शून्य
ex_later(exarg_T *eap)
{
    दीर्घ	मोजणी = 0;
    पूर्णांक		sec = FALSE;
    पूर्णांक		file = FALSE;
    char_u	*p = eap->arg;

    जर (*p == NUL)
	मोजणी = 1;
    अन्यथा जर (SAFE_isdigit(*p))
    {
	मोजणी = getdigits(&p);
	निवड (*p)
	{
	    स्थिती 's': ++p; sec = TRUE; थांबा;
	    स्थिती 'm': ++p; sec = TRUE; मोजणी *= 60; थांबा;
	    स्थिती 'h': ++p; sec = TRUE; मोजणी *= 60 * 60; थांबा;
	    स्थिती 'd': ++p; sec = TRUE; मोजणी *= 24 * 60 * 60; थांबा;
	    स्थिती 'f': ++p; file = TRUE; थांबा;
	}
    }

    जर (*p != NUL)
	semsg(_(e_invalid_argument_str), eap->arg);
    अन्यथा
	undo_time(eap->cmdidx == CMD_earlier ? -मोजणी : मोजणी,
							    sec, file, FALSE);
}

/*
 * ":redir": start/stop redirection.
 */
    स्थिर शून्य
ex_redir(exarg_T *eap)
{
    अक्षर	*mode;
    char_u	*fname;
    char_u	*arg = eap->arg;

#ifdef FEAT_EVAL
    जर (redir_execute)
    {
	emsg(_(e_cannot_use_redir_inside_execute));
	परत;
    }
#endif

    जर (STRICMP(eap->arg, "END") == 0)
	close_redir();
    अन्यथा
    {
	जर (*arg == '>')
	{
	    ++arg;
	    जर (*arg == '>')
	    {
		++arg;
		mode = "a";
	    }
	    अन्यथा
		mode = "w";
	    arg = skipwhite(arg);

	    close_redir();

	    // Expand environment variables and "~/".
	    fname = expand_env_save(arg);
	    जर (fname == रिक्त)
		परत;
#ifdef FEAT_BROWSE
	    जर (cmdmod.cmod_flags & CMOD_BROWSE)
	    {
		char_u	*browseFile;

		browseFile = do_browse(BROWSE_SAVE,
			(char_u *)_("Save Redirection"),
			fname, रिक्त, रिक्त,
			(char_u *)_(BROWSE_FILTER_ALL_FILES), curbuf);
		जर (browseFile == रिक्त)
		    परत;		// operation cancelled
		vim_free(fname);
		fname = browseFile;
		eap->forceit = TRUE;	// since dialog already asked
	    }
#endif

	    redir_fd = open_exfile(fname, eap->forceit, mode);
	    vim_free(fname);
	}
#ifdef FEAT_EVAL
	अन्यथा जर (*arg == '@')
	{
	    // redirect to a register a-z (resp. A-Z for appending)
	    close_redir();
	    ++arg;
	    जर (ASCII_ISALPHA(*arg)
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
		जर (*arg == '>' && arg[1] == '>')  // append
		    arg += 2;
		अन्यथा
		{
		    // Can use both "@a" and "@a>".
		    जर (*arg == '>')
			arg++;
		    // Make register empty when not using @A-@Z and the
		    // command is valid.
		    जर (*arg == NUL && !SAFE_isupper(redir_reg))
			write_reg_contents(redir_reg, (char_u *)"", -1, FALSE);
		}
	    }
	    जर (*arg != NUL)
	    {
		redir_reg = 0;
		semsg(_(e_invalid_argument_str), eap->arg);
	    }
	}
	अन्यथा जर (*arg == '=' && arg[1] == '>')
	{
	    पूर्णांक append;

	    // redirect to a variable
	    close_redir();
	    arg += 2;

	    जर (*arg == '>')
	    {
		++arg;
		append = TRUE;
	    }
	    अन्यथा
		append = FALSE;

	    जर (var_redir_start(skipwhite(arg), append) == OK)
		redir_vname = 1;
	}
#endif

	// TODO: redirect to a buffer

	अन्यथा
	    semsg(_(e_invalid_argument_str), eap->arg);
    }

    // Make sure redirection is not off.  Can happen for cmdline completion
    // that indirectly invokes a command to catch its output.
    जर (redir_fd != रिक्त
#ifdef FEAT_EVAL
			  || redir_reg || redir_vname
#endif
							)
	redir_off = FALSE;
}

/*
 * ":redraw": force redraw, with clear for ":redraw!".
 */
    शून्य
ex_redraw(exarg_T *eap)
{
    redraw_cmd(eap->forceit);
}

/*
 * ":redraw": force redraw, with clear if "clear" is TRUE.
 */
    शून्य
redraw_cmd(पूर्णांक clear)
{
    पूर्णांक save_RedrawingDisabled = RedrawingDisabled;
    RedrawingDisabled = 0;

    पूर्णांक save_p_lz = p_lz;
    p_lz = FALSE;

    validate_cursor();
    update_topline();
    update_screen(clear ? UPD_CLEAR : VIsual_active ? UPD_INVERTED : 0);
    जर ((State & MODE_CMDLINE) == 0)
	setcursor(); // put cursor back where it belongs
    जर (need_maketitle)
	maketitle();
#जर defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
# ifdef VIMDLL
    जर (!gui.in_use)
# endif
	resize_console_buf();
#endif
    RedrawingDisabled = save_RedrawingDisabled;
    p_lz = save_p_lz;

    // After drawing the statusline screen_attr may still be set.
    screen_stop_highlight();

    // Reset msg_didout, so that a message that's there is overwritten.
    संदेश_didout = FALSE;
    संदेश_col = 0;

    // No need to wait after an intentional redraw.
    need_wait_return = FALSE;

    // When invoked from a callback or autocmd the command line may be active.
    जर (State & MODE_CMDLINE)
	redrawcmdline();

    out_flush();
}

/*
 * ":redrawstatus": force redraw of status line(s)
 */
    स्थिर शून्य
ex_redrawstatus(exarg_T *eap UNUSED)
{
    जर (eap->forceit)
	status_redraw_all();
    अन्यथा
	status_redraw_curbuf();
    redraw_vseps = TRUE;
    जर (संदेश_scrolled && (State & MODE_CMDLINE))
	परत;  // redraw later

    पूर्णांक save_RedrawingDisabled = RedrawingDisabled;
    RedrawingDisabled = 0;

    पूर्णांक save_p_lz = p_lz;
    p_lz = FALSE;

    जर (State & MODE_CMDLINE)
	redraw_statuslines();
    अन्यथा
	update_screen(VIsual_active ? UPD_INVERTED : 0);
    RedrawingDisabled = save_RedrawingDisabled;
    p_lz = save_p_lz;
    out_flush();
}

/*
 * ":redrawtabline": force redraw of the tabline
 */
    स्थिर शून्य
ex_redrawtabline(exarg_T *eap UNUSED)
{
    पूर्णांक save_RedrawingDisabled = RedrawingDisabled;
    RedrawingDisabled = 0;

    पूर्णांक save_p_lz = p_lz;
    p_lz = FALSE;

    draw_tabline();

    RedrawingDisabled = save_RedrawingDisabled;
    p_lz = save_p_lz;
    out_flush();
}

/*
 * ":redrawtabpanel": force redraw of the tabpanel
 */
    स्थिर शून्य
ex_redrawtabpanel(exarg_T *eap UNUSED)
{
    पूर्णांक save_RedrawingDisabled = RedrawingDisabled;
    RedrawingDisabled = 0;

    पूर्णांक save_p_lz = p_lz;
    p_lz = FALSE;

#जर defined(FEAT_TABPANEL)
    draw_tabpanel();
#अन्यथा
    emsg(_(e_cannot_not_support_redrawtabpanel));
#endif

    RedrawingDisabled = save_RedrawingDisabled;
    p_lz = save_p_lz;
    out_flush();
}

    स्थिर शून्य
close_redir(शून्य)
{
    जर (redir_fd != रिक्त)
    {
	ख_बंद(redir_fd);
	redir_fd = रिक्त;
    }
#ifdef FEAT_EVAL
    redir_reg = 0;
    जर (redir_vname)
    {
	var_redir_stop();
	redir_vname = 0;
    }
#endif
}

#जर defined(FEAT_SESSION) || defined(FEAT_EVAL)
    पूर्णांक
vim_mkdir_emsg(char_u *name, पूर्णांक prot UNUSED)
{
    जर (vim_mkdir(name, prot) != 0)
    {
	semsg(_(e_cannot_create_directory_str), name);
	परत FAIL;
    }
    परत OK;
}
#endif

/*
 * Open a file for writing for an Ex command, with some checks.
 * Return file descriptor, or NULL on failure.
 */
    खाता *
open_exfile(
    char_u	*fname,
    पूर्णांक		forceit,
    अक्षर	*mode)	    // "w" for create new file or "a" for append
{
    खाता	*fd;

#ifdef UNIX
    // with Unix it is possible to open a directory
    जर (mch_isdir(fname))
    {
	semsg(_(e_str_is_directory), fname);
	परत रिक्त;
    }
#endif
    जर (!forceit && *mode != 'a' && vim_fexists(fname))
    {
	semsg(_(e_str_exists_add_bang_to_override), fname);
	परत रिक्त;
    }

    जर ((fd = mch_fopen((अक्षर *)fname, mode)) == रिक्त)
	semsg(_(e_cannot_open_str_for_writing_2), fname);

    परत fd;
}

/*
 * ":mark" and ":k".
 */
    स्थिर शून्य
ex_mark(exarg_T *eap)
{
    pos_T	pos;

#ifdef FEAT_EVAL
    जर (not_in_vim9(eap, false) == FAIL)
	परत;
#endif
    जर (*eap->arg == NUL)		// No argument?
    {
	emsg(_(e_argument_required));
	परत;
    }

    जर (eap->arg[1] != NUL)	// more than one character?
    {
	semsg(_(e_trailing_characters_str), eap->arg);
	परत;
    }

    pos = curwin->w_cursor;		// save curwin->w_cursor
    curwin->w_cursor.lnum = eap->line2;
    beginline(BL_WHITE | BL_FIX);
    जर (setmark(*eap->arg) == FAIL)	// set mark
	emsg(_(e_argument_must_be_letter_or_forward_backward_quote));
    curwin->w_cursor = pos;		// restore curwin->w_cursor
}

/*
 * Update w_topline, w_leftcol and the cursor position.
 */
    शून्य
update_topline_cursor(शून्य)
{
    check_cursor();		// put cursor on valid line
    update_topline();
    जर (!curwin->w_p_wrap)
	validate_cursor();
    update_curswant();
}

/*
 * Save the current State and go to Normal mode.
 * Return TRUE if the typeahead could be saved.
 */
    पूर्णांक
save_current_state(save_state_T *sst)
{
    sst->save_संदेश_scroll = संदेश_scroll;
    sst->save_restart_edit = restart_edit;
    sst->save_संदेश_didout = संदेश_didout;
    sst->save_State = State;
    sst->save_insertmode = p_im;
    sst->save_finish_op = finish_op;
    sst->save_opcount = opcount;
    sst->save_reg_executing = reg_executing;
    sst->save_pending_end_reg_executing = pending_end_reg_executing;

    संदेश_scroll = FALSE;		    // no msg scrolling in Normal mode
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
    परत sst->tabuf.typebuf_valid;
}

    शून्य
restore_current_state(save_state_T *sst)
{
    // Restore the previous typeahead.
    restore_typeahead(&sst->tabuf, FALSE);

    संदेश_scroll = sst->save_संदेश_scroll;
    restart_edit = sst->save_restart_edit;
    p_im = sst->save_insertmode;
    finish_op = sst->save_finish_op;
    opcount = sst->save_opcount;
    reg_executing = sst->save_reg_executing;
    pending_end_reg_executing = sst->save_pending_end_reg_executing;
    संदेश_didout |= sst->save_संदेश_didout;	// don't reset msg_didout now
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
    शून्य
ex_normal(exarg_T *eap)
{
    save_state_T save_state;
    char_u	*arg = रिक्त;
    पूर्णांक		l;
    char_u	*p;

    जर (ex_normal_lock > 0)
    {
	emsg(_(e_not_allowed_here));
	परत;
    }
    जर (ex_normal_busy >= p_mmd)
    {
	emsg(_(e_recursive_use_of_normal_too_deep));
	परत;
    }

    /*
     * vgetc() expects a CSI and K_SPECIAL to have been escaped.  Don't do
     * this for the K_SPECIAL leading byte, otherwise special keys will not
     * work.
     */
    जर (has_mbyte)
    {
	पूर्णांक	len = 0;

	// Count the number of characters to be escaped.
	साठी (p = eap->arg; *p != NUL; ++p)
	{
#ifdef FEAT_GUI
	    जर (*p == CSI)  // leadbyte CSI
		len += 2;
#endif
	    साठी (l = (*mb_ptr2len)(p) - 1; l > 0; --l)
		जर (*++p == K_SPECIAL	  // trailbyte K_SPECIAL or CSI
#ifdef FEAT_GUI
			|| *p == CSI
#endif
			)
		    len += 2;
	}
	जर (len > 0)
	{
	    arg = alloc(STRLEN(eap->arg) + len + 1);
	    जर (arg != रिक्त)
	    {
		len = 0;
		साठी (p = eap->arg; *p != NUL; ++p)
		{
		    arg[len++] = *p;
#ifdef FEAT_GUI
		    जर (*p == CSI)
		    {
			arg[len++] = KS_EXTRA;
			arg[len++] = (पूर्णांक)KE_CSI;
		    }
#endif
		    साठी (l = (*mb_ptr2len)(p) - 1; l > 0; --l)
		    {
			arg[len++] = *++p;
			जर (*p == K_SPECIAL)
			{
			    arg[len++] = KS_SPECIAL;
			    arg[len++] = KE_FILLER;
			}
#ifdef FEAT_GUI
			अन्यथा जर (*p == CSI)
			{
			    arg[len++] = KS_EXTRA;
			    arg[len++] = (पूर्णांक)KE_CSI;
			}
#endif
		    }
		    arg[len] = NUL;
		}
	    }
	}
    }

    ++ex_normal_busy;
    जर (save_current_state(&save_state))
    {
	/*
	 * Repeat the :normal command for each line in the range.  When no
	 * range given, execute it just once, without positioning the cursor
	 * first.
	 */
	करा
	{
	    जर (eap->addr_मोजणी != 0)
	    {
		curwin->w_cursor.lnum = eap->line1++;
		curwin->w_cursor.col = 0;
		check_cursor_moved(curwin);
	    }

	    exec_normal_cmd(arg != रिक्त
		     ? arg
		     : eap->arg, eap->forceit ? REMAP_NONE : REMAP_YES, FALSE);
	}
	पर्यंत (eap->addr_मोजणी > 0 && eap->line1 <= eap->line2 && !got_int);
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
    स्थिर शून्य
ex_startinsert(exarg_T *eap)
{
#ifdef FEAT_TERMINAL
    // Ignore this when running in an active terminal.
    जर (term_job_running(curbuf->b_term))
	परत;
#endif
    जर (!curbuf->b_p_ma && !p_im)
    {
	// Only give this error when 'insertmode' is off.
	emsg(_(e_cannot_make_changes_modifiable_is_off));
	परत;
    }
    जर (eap->forceit)
    {
	// cursor line can be zero on startup
	जर (!curwin->w_cursor.lnum)
	    curwin->w_cursor.lnum = 1;
	set_cursor_for_append_to_ओळ();
    }

    // Ignore the command when already in Insert mode.  Inserting an
    // expression register that invokes a function can do this.
    जर (State & MODE_घाला)
	परत;

    जर (eap->cmdidx == CMD_startinsert)
	restart_edit = 'a';
    अन्यथा जर (eap->cmdidx == CMD_startreplace)
	restart_edit = 'R';
    अन्यथा
	restart_edit = 'V';

    जर (!eap->forceit)
    {
	जर (eap->cmdidx == CMD_startinsert)
	    restart_edit = 'i';
	curwin->w_curswant = 0;	    // avoid MAXCOL
    }

    जर (VIsual_active)
	showmode();
}

/*
 * ":stopinsert"
 */
    स्थिर शून्य
ex_stopinsert(exarg_T *eap UNUSED)
{
    restart_edit = 0;
    stop_घाला_mode = TRUE;
#जर defined(FEAT_CLIENTSERVER) || defined(FEAT_EVAL)
    // when called from remote_expr in insert mode, make sure insert mode is
    // ended by adding K_NOP to the typeahead buffer
    जर (vgetc_busy)
	ins_char_typebuf(K_NOP, 0);
#endif
    clearmode();
}

/*
 * Execute normal mode command "cmd".
 * "remap" can be REMAP_NONE or REMAP_YES.
 */
    शून्य
exec_normal_cmd(char_u *cmd, पूर्णांक remap, पूर्णांक silent)
{
    // Stuff the argument into the typeahead buffer.
    ins_typebuf(cmd, remap, 0, TRUE, silent);
    exec_normal(FALSE, FALSE, FALSE);
}

/*
 * Execute normal_cmd() until there is no typeahead left.
 * When "use_vpeekc" is TRUE use vpeekc() to check for available chars.
 */
    शून्य
exec_normal(पूर्णांक was_typed, पूर्णांक use_vpeekc, पूर्णांक may_use_terminal_loop UNUSED)
{
    oparg_T	oa;
    पूर्णांक		c;

    // When calling vpeekc() from feedkeys() it will return Ctrl_C when there
    // is nothing to get, so also check for Ctrl_C.
    clear_oparg(&oa);
    finish_op = FALSE;
    पर्यंत ((!stuff_empty()
		|| ((was_typed || !typebuf_typed()) && typebuf.tb_len > 0)
		|| (use_vpeekc && (c = vpeekc()) != NUL && c != Ctrl_C))
	    && !got_int)
    {
	update_topline_cursor();
#ifdef FEAT_TERMINAL
	जर (may_use_terminal_loop && term_use_loop()
		&& oa.op_type == OP_NOP && oa.regname == NUL
		&& !VIsual_active)
	{
	    // If terminal_loop() returns OK we got a key that is handled
	    // in Normal model.  With FAIL we first need to position the
	    // cursor and the screen needs to be redrawn.
	    जर (terminal_loop(TRUE) == OK)
		normal_cmd(&oa, TRUE);
	}
	अन्यथा
#endif
	    // execute a Normal mode cmd
	    normal_cmd(&oa, TRUE);
    }
}

#ifdef FEAT_FIND_ID
    स्थिर शून्य
ex_checkpath(exarg_T *eap)
{
    find_pattern_in_path(रिक्त, 0, 0, FALSE, FALSE, CHECK_PATH, 1L,
	    eap->forceit ? ACTION_SHOW_ALL : ACTION_SHOW,
	    (linenr_T)1, (linenr_T)MAXLNUM, eap->forceit, FALSE);
}

# जर defined(FEAT_QUICKFIX)
/*
 * ":psearch"
 */
    स्थिर शून्य
ex_psearch(exarg_T *eap)
{
    g_do_tagpreview = p_pvh;
    ex_findpat(eap);
    g_do_tagpreview = 0;
}
# endif

    स्थिर शून्य
ex_findpat(exarg_T *eap)
{
    पूर्णांक		whole = TRUE;
    दीर्घ	n;
    char_u	*p;
    पूर्णांक		action;

    निवड (cmdnames[eap->cmdidx].cmd_name[2])
    {
	स्थिती 'e':	// ":psearch", ":isearch" and ":dsearch"
		जर (cmdnames[eap->cmdidx].cmd_name[0] == 'p')
		    action = ACTION_GOTO;
		अन्यथा
		    action = ACTION_SHOW;
		थांबा;
	स्थिती 'i':	// ":ilist" and ":dlist"
		action = ACTION_SHOW_ALL;
		थांबा;
	स्थिती 'u':	// ":ijump" and ":djump"
		action = ACTION_GOTO;
		थांबा;
	पूर्वनिर्धारित:	// ":isplit" and ":dsplit"
		action = ACTION_SPLIT;
		थांबा;
    }

    n = 1;
    जर (vim_isdigit(*eap->arg))	// get count
    {
	n = getdigits(&eap->arg);
	eap->arg = skipwhite(eap->arg);
    }
    जर (*eap->arg == '/')   // Match regexp, not just whole words
    {
	whole = FALSE;
	++eap->arg;
	p = skip_regexp(eap->arg, '/', magic_isset());
	जर (*p)
	{
	    *p++ = NUL;
	    p = skipwhite(p);

	    // Check for trailing illegal characters
	    जर (!ends_excmd2(eap->arg, p))
		eap->errmsg = ex_errmsg(e_trailing_characters_str, p);
	    अन्यथा
		set_nextcmd(eap, p);
	}
    }
    जर (!eap->skip)
	find_pattern_in_path(eap->arg, 0, (पूर्णांक)STRLEN(eap->arg),
		whole, !eap->forceit,
		*eap->cmd == 'd' ? FIND_DEFINE : FIND_ANY, n, action,
		eap->line1, eap->line2, eap->forceit, FALSE);
}
#endif


#ifdef FEAT_QUICKFIX
/*
 * ":ptag", ":ptselect", ":ptjump", ":ptnext", etc.
 */
    स्थिर शून्य
ex_ptag(exarg_T *eap)
{
    g_do_tagpreview = p_pvh;  // will be reset to 0 in ex_tag_cmd()
    ex_tag_cmd(eap, cmdnames[eap->cmdidx].cmd_name + 1);
}

/*
 * ":pedit"
 */
    स्थिर शून्य
ex_pedit(exarg_T *eap)
{
    win_T	*curwin_save = curwin;
    prepare_preview_window();

    // Edit the file.
    do_exedit(eap, रिक्त);

    back_to_current_window(curwin_save);
}

/*
 * ":pbuffer"
 */
    स्थिर शून्य
ex_pbuffer(exarg_T *eap)
{
    win_T	*curwin_save = curwin;
    prepare_preview_window();

    // Go to the buffer.
    do_exbuffer(eap);

    back_to_current_window(curwin_save);
}

    स्थिर शून्य
prepare_preview_window(शून्य)
{
    जर (ERROR_IF_ANY_POPUP_WINDOW)
	परत;

    // Open the preview window or popup and make it the current window.
    g_do_tagpreview = p_pvh;
    prepare_tagpreview(TRUE, TRUE, FALSE);
}

    स्थिर शून्य
back_to_current_window(win_T *curwin_save)
{
    जर (curwin != curwin_save && win_valid(curwin_save))
    {
	// Return cursor to where we were
	validate_cursor();
	redraw_later(UPD_VALID);
	win_enter(curwin_save, TRUE);
    }
# ifdef FEAT_PROP_POPUP
    अन्यथा जर (WIN_IS_POPUP(curwin))
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
    स्थिर शून्य
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
    स्थिर शून्य
ex_tag(exarg_T *eap)
{
    ex_tag_cmd(eap, cmdnames[eap->cmdidx].cmd_name);
}

    स्थिर शून्य
ex_tag_cmd(exarg_T *eap, char_u *name)
{
    पूर्णांक		cmd;

    निवड (name[1])
    {
	स्थिती 'j': cmd = DT_JUMP;	// ":tjump"
		  थांबा;
	स्थिती 's': cmd = DT_SELECT;	// ":tselect"
		  थांबा;
	स्थिती 'p': cmd = DT_PREV;	// ":tprevious"
		  थांबा;
	स्थिती 'N': cmd = DT_PREV;	// ":tNext"
		  थांबा;
	स्थिती 'n': cmd = DT_NEXT;	// ":tnext"
		  थांबा;
	स्थिती 'o': cmd = DT_काढा;		// ":pop"
		  थांबा;
	स्थिती 'f':			// ":tfirst"
	स्थिती 'r': cmd = DT_FIRST;	// ":trewind"
		  थांबा;
	स्थिती 'l': cmd = DT_LAST;	// ":tlast"
		  थांबा;
	पूर्वनिर्धारित:			// ":tag"
#ifdef FEAT_CSCOPE
		  जर (p_cst && *eap->arg != NUL)
		  {
		      ex_cstag(eap);
		      परत;
		  }
#endif
		  cmd = DT_TAG;
		  थांबा;
    }

    जर (name[0] == 'l')
    {
#ifndef FEAT_QUICKFIX
	ex_ni(eap);
	परत;
#अन्यथा
	cmd = DT_LTAG;
#endif
    }

    do_tag(eap->arg, cmd, eap->addr_मोजणी > 0 ? (पूर्णांक)eap->line2 : 1,
							  eap->forceit, TRUE);
}

गणना {
    SPEC_PERC = 0,
    SPEC_HASH,
    SPEC_CWORD,	    // cursor word
    SPEC_CCWORD,    // cursor WORD
    SPEC_CEXPR,	    // expr under cursor
    SPEC_CFILE,	    // cursor path name
    SPEC_SFILE,	    // ":so" file name
    SPEC_SLNUM,	    // ":so" file line number
    SPEC_चळत,	    // call stack
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
    पूर्णांक
find_cmdline_var(char_u *src, माप_प्रकार *usedlen)
{
    // must be sorted by the 'value' field because it is used by bsearch()!
    स्थिर keyvalue_T spec_str_tab[] = {
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
	KEYVALUE_ENTRY(SPEC_चळत, "stack>")	    // call stack
    };
    keyvalue_T target;
    keyvalue_T *entry;

    निवड (*src)
    {
    स्थिती '%':
	*usedlen = 1;
	परत SPEC_PERC;

    स्थिती '#':
	*usedlen = 1;
	परत SPEC_HASH;

    स्थिती '<':
	target.किल्ली = 0;
	target.मूल्य.string = src + 1;	    // skip over '<'
	target.मूल्य.length = 0;	    // not used, see cmp_keyvalue_value_n()

	entry = (keyvalue_T *)द्वा_खोज(&target, &spec_str_tab,
		ARRAY_LENGTH(spec_str_tab), आकार(spec_str_tab[0]),
		cmp_keyvalue_मूल्य_n);
	जर (entry == रिक्त)
	    परत -1;

	*usedlen = entry->मूल्य.length + 1;
	परत entry->किल्ली;

    पूर्वनिर्धारित:
	थांबा;
    }

    परत -1;
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
    माप_प्रकार	*usedlen,	// characters after src that are used
    linenr_T	*lnump,		// line number for :e command, or NULL
    अक्षर	**errormsg,	// pointer to error message
    पूर्णांक		*escaped,	// return value has escaped white space (can
				// be NULL)
    पूर्णांक		empty_is_error)	// empty result is considered an error
{
    पूर्णांक		i;
    char_u	*s;
    char_u	*result = (char_u *)"";
    char_u	*resultbuf = रिक्त;
    माप_प्रकार	resultlen;
    buf_T	*buf;
    पूर्णांक		valid = VALID_HEAD + VALID_PATH;    // assume valid result
    पूर्णांक		spec_idx;
    पूर्णांक		tilde_file = FALSE;
    पूर्णांक		skip_mod = FALSE;
    char_u	strbuf[30];

    *errormsg = रिक्त;
    जर (escaped != रिक्त)
	*escaped = FALSE;

    /*
     * Check if there is something to do.
     */
    spec_idx = find_cmdline_var(src, usedlen);
    जर (spec_idx < 0)	// no match
    {
	*usedlen = 1;
	परत रिक्त;
    }

    /*
     * Skip when preceded with a backslash "\%" and "\#".
     * Note: In "\\%" the % is also not recognized!
     */
    जर (src > srcstart && src[-1] == '\\')
    {
	*usedlen = 0;
	STRMOVE(src - 1, src);	// remove backslash
	परत रिक्त;
    }

    /*
     * word or WORD under cursor
     */
    जर (spec_idx == SPEC_CWORD || spec_idx == SPEC_CCWORD
						     || spec_idx == SPEC_CEXPR)
    {
	resultlen = find_ident_under_cursor(&result,
		spec_idx == SPEC_CWORD ? (FIND_IDENT | FIND_STRING)
	      : spec_idx == SPEC_CEXPR ? (FIND_IDENT | FIND_STRING | FIND_EVAL)
	      : FIND_STRING);
	जर (resultlen == 0)
	{
	    *errormsg = "";
	    परत रिक्त;
	}
    }

    /*
     * '#': Alternate file name
     * '%': Current file name
     *	    File name under the cursor
     *	    File name for autocommand
     *	and following modifiers
     */
    अन्यथा
    {
	माप_प्रकार off = 0;

	निवड (spec_idx)
	{
	स्थिती SPEC_PERC:
#ifdef FEAT_EVAL
		जर (!in_vim9script() || src[1] != '%')
#endif
		{
		    // '%': current file
		    जर (curbuf->b_fname == रिक्त)
		    {
			result = (char_u *)"";
			valid = 0;	    // Must have ":p:h" to be valid
		    }
		    अन्यथा
		    {
			result = curbuf->b_fname;
			tilde_file = STRCMP(result, "~") == 0;
		    }
		    थांबा;
		}
#ifdef FEAT_EVAL
		// "%%" alternate file
		off = 1;
#endif
		// FALLTHROUGH
	स्थिती SPEC_HASH:		// '#' or "#99": alternate file
		जर (off == 0 ? src[1] == '#' : src[2] == '%')
		{
		    // "##" or "%%%": the argument list
		    result = arg_all();
		    resultbuf = result;
		    *usedlen = off + 2;
		    जर (escaped != रिक्त)
			*escaped = TRUE;
		    skip_mod = TRUE;
		    थांबा;
		}
		s = src + off + 1;
		जर (*s == '<')		// "#<99" uses v:oldfiles
		    ++s;
		i = (पूर्णांक)getdigits(&s);
		जर (s == src + off + 2 && src[off + 1] == '-')
		    // just a minus sign, don't skip over it
		    s--;
		*usedlen = (पूर्णांक)(s - src); // length of what we expand

		जर (src[off + 1] == '<' && i != 0)
		{
		    जर (*usedlen < off + 2)
		    {
			// Should we give an error message for #<text?
			*usedlen = off + 1;
			परत रिक्त;
		    }
#ifdef FEAT_EVAL
		    result = list_find_str(get_vim_var_list(VV_OLDFILES),
								     (दीर्घ)i);
		    जर (result == रिक्त)
		    {
			*errormsg = "";
			परत रिक्त;
		    }
#अन्यथा
		    *errormsg = _(e_hashsmall_is_not_available_without_the_eval_feature);
		    परत रिक्त;
#endif
		}
		अन्यथा
		{
		    जर (i == 0 && src[off + 1] == '<' && *usedlen > off + 1)
			*usedlen = off + 1;
		    buf = buflist_findnr(i);
		    जर (buf == रिक्त)
		    {
			*errormsg = _(e_no_alternate_file_name_to_substitute_for_hash);
			परत रिक्त;
		    }
		    जर (lnump != रिक्त)
			*lnump = ECMD_LAST;
		    जर (buf->b_fname == रिक्त)
		    {
			result = (char_u *)"";
			valid = 0;	    // Must have ":p:h" to be valid
		    }
		    अन्यथा
		    {
			result = buf->b_fname;
			tilde_file = STRCMP(result, "~") == 0;
		    }
		}
		थांबा;

	स्थिती SPEC_CFILE:	// file name under cursor
		result = file_name_at_cursor(FNAME_MESS|FNAME_HYP, 1L, रिक्त);
		जर (result == रिक्त)
		{
		    *errormsg = "";
		    परत रिक्त;
		}
		resultbuf = result;	    // remember allocated string
		थांबा;

	स्थिती SPEC_AFILE:	// file name for autocommand
		result = autocmd_fname;
		जर (result != रिक्त && !autocmd_fname_full)
		{
		    // Still need to turn the fname into a full path.  It is
		    // postponed to avoid a delay when <afile> is not used.
		    autocmd_fname_full = TRUE;
		    result = FullName_save(autocmd_fname, FALSE);
		    vim_free(autocmd_fname);
		    autocmd_fname = result;
		}
		जर (result == रिक्त)
		{
		    *errormsg = _(e_no_autocommand_file_name_to_substitute_for_afile);
		    परत रिक्त;
		}
		result = shorten_fname1(result);
		थांबा;

	स्थिती SPEC_ABUF:		// buffer number for autocommand
		जर (autocmd_bufnr <= 0)
		{
		    *errormsg = _(e_no_autocommand_buffer_संख्या_to_substitute_for_abuf);
		    परत रिक्त;
		}
		प्र_लिखो((अक्षर *)strbuf, "%d", autocmd_bufnr);
		result = strbuf;
		थांबा;

	स्थिती SPEC_AMATCH:	// match name for autocommand
		result = autocmd_match;
		जर (result == रिक्त)
		{
		    *errormsg = _(e_no_autocommand_match_name_to_substitute_for_amatch);
		    परत रिक्त;
		}
		थांबा;

	स्थिती SPEC_SFILE:	// file name for ":so" command
		result = estack_sfile(ESTACK_SFILE);
		जर (result == रिक्त)
		{
		    *errormsg = _(e_no_source_file_name_to_substitute_for_sfile);
		    परत रिक्त;
		}
		resultbuf = result;	    // remember allocated string
		थांबा;
	स्थिती SPEC_चळत:	// call stack
		result = estack_sfile(ESTACK_चळत);
		जर (result == रिक्त)
		{
		    *errormsg = _(e_no_call_चळत_to_substitute_for_चळत);
		    परत रिक्त;
		}
		resultbuf = result;	    // remember allocated string
		थांबा;
	स्थिती SPEC_SCRIPT:	// script file name
		result = estack_sfile(ESTACK_SCRIPT);
		जर (result == रिक्त)
		{
		    *errormsg = _(e_no_script_file_name_to_substitute_for_script);
		    परत रिक्त;
		}
		resultbuf = result;	    // remember allocated string
		थांबा;

	स्थिती SPEC_SLNUM:	// line in file for ":so" command
		जर (SOURCING_NAME == रिक्त || SOURCING_LNUM == 0)
		{
		    *errormsg = _(e_no_ओळ_संख्या_to_use_for_slnum);
		    परत रिक्त;
		}
		प्र_लिखो((अक्षर *)strbuf, "%ld", SOURCING_LNUM);
		result = strbuf;
		थांबा;

#ifdef FEAT_EVAL
	स्थिती SPEC_SFLNUM:	// line in script file
		जर (current_sctx.sc_lnum + SOURCING_LNUM == 0)
		{
		    *errormsg = _(e_no_ओळ_संख्या_to_use_for_sflnum);
		    परत रिक्त;
		}
		प्र_लिखो((अक्षर *)strbuf, "%ld",
				 (दीर्घ)(current_sctx.sc_lnum + SOURCING_LNUM));
		result = strbuf;
		थांबा;

	स्थिती SPEC_SID:
		जर (current_sctx.sc_sid <= 0)
		{
		    *errormsg = _(e_using_sid_not_in_script_context);
		    परत रिक्त;
		}
		प्र_लिखो((अक्षर *)strbuf, "<SNR>%d_", current_sctx.sc_sid);
		result = strbuf;
		थांबा;
#endif

#ifdef FEAT_CLIENTSERVER
	स्थिती SPEC_CLIENT:	// Source of last submitted input
# ifdef MSWIN
		प्र_लिखो((अक्षर *)strbuf, PRINTF_HEX_LONG_U,
							(long_u)clientWindow);
		result = strbuf;
# अन्यथा
#  ifdef FEAT_SOCKETSERVER
		जर (clientserver_method == CLIENTSERVER_METHOD_SOCKET)
		{
		    जर (client_socket == रिक्त)
			result = (char_u *)"";
		    अन्यथा
			result = client_socket;
		}
#  endif
#  ifdef FEAT_X11
		जर (clientserver_method == CLIENTSERVER_METHOD_X11)
		{
		    प्र_लिखो((अक्षर *)strbuf, PRINTF_HEX_LONG_U,
							(long_u)clientWindow);
		    result = strbuf;
		}
#  endif
# endif
		थांबा;
#endif
	}

	resultlen = STRLEN(result);	// length of new string
	जर (src[*usedlen] == '<')	// remove the file name extension
	{
	    ++*usedlen;
	    जर ((s = vim_strrchr(result, '.')) != रिक्त && s >= gettail(result))
		resultlen = s - result;
	}
	अन्यथा जर (!skip_mod)
	{
	    valid |= modify_fname(src, tilde_file, usedlen, &result, &resultbuf,
								  &resultlen);
	    जर (result == रिक्त)
	    {
		*errormsg = "";
		परत रिक्त;
	    }
	}
    }

    जर (resultlen == 0 || valid != VALID_HEAD + VALID_PATH)
    {
	जर (empty_is_error)
	{
	    जर (valid != VALID_HEAD + VALID_PATH)
		*errormsg = _(e_empty_file_name_for_percent_or_hash_only_works_with_ph);
	    अन्यथा
		*errormsg = _(e_evaluates_to_an_empty_string);
	}
	result = रिक्त;
    }
    अन्यथा
	result = vim_strnsave(result, resultlen);
    vim_free(resultbuf);
    परत result;
}

/*
 * Expand the <sfile> string in "arg".
 *
 * Returns an allocated string, or NULL for any error.
 */
    char_u *
expand_sfile(char_u *arg)
{
    अक्षर	*errormsg;
    char_u	*result;
    माप_प्रकार	resultlen;
    char_u	*newres;
    माप_प्रकार	len;
    char_u	*repl;
    माप_प्रकार	repllen;
    माप_प्रकार	srclen;
    char_u	*p;

    resultlen = STRLEN(arg);
    result = vim_strnsave(arg, resultlen);
    जर (result == रिक्त)
	परत रिक्त;

    साठी (p = result; *p; )
    {
	जर (STRNCMP(p, "<sfile>", 7) != 0)
	    ++p;
	अन्यथा
	{
	    // replace "<sfile>" with the sourced file name, and do ":" stuff
	    repl = eval_vars(p, result, &srclen, रिक्त, &errormsg, रिक्त, TRUE);
	    जर (errormsg != रिक्त)
	    {
		जर (*errormsg)
		    emsg(errormsg);
		vim_free(result);
		परत रिक्त;
	    }
	    जर (repl == रिक्त)		// no match (cannot happen)
	    {
		p += srclen;
		सुरूठेवा;
	    }
	    repllen = STRLEN(repl);
	    resultlen += repllen - srclen;
	    newres = alloc(resultlen + 1);
	    जर (newres == रिक्त)
	    {
		vim_free(repl);
		vim_free(result);
		परत रिक्त;
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

    परत result;
}

#जर defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
/*
 * Make a dialog message in "buff[DIALOG_MSG_SIZE]".
 * "format" must contain "%s".
 */
    शून्य
dialog_संदेश(char_u *buff, अक्षर *format, char_u *fname)
{
    जर (fname == रिक्त)
	fname = (char_u *)_("Untitled");
    vim_snprintf((अक्षर *)buff, DIALOG_संदेश_SIZE, format, fname);
}
#endif

/*
 * ":behave {mswin,xterm}"
 */
    स्थिर शून्य
ex_behave(exarg_T *eap)
{
    जर (STRCMP(eap->arg, "mswin") == 0)
    {
	set_option_मूल्य_give_err((char_u *)"selection",
						 0L, (char_u *)"exclusive", 0);
	set_option_मूल्य_give_err((char_u *)"selectmode",
						 0L, (char_u *)"mouse,key", 0);
	set_option_मूल्य_give_err((char_u *)"mousemodel",
						     0L, (char_u *)"popup", 0);
	set_option_मूल्य_give_err((char_u *)"keymodel",
					  0L, (char_u *)"startsel,stopsel", 0);
    }
    अन्यथा जर (STRCMP(eap->arg, "xterm") == 0)
    {
	set_option_मूल्य_give_err((char_u *)"selection",
						 0L, (char_u *)"inclusive", 0);
	set_option_मूल्य_give_err((char_u *)"selectmode", 0L, (char_u *)"", 0);
	set_option_मूल्य_give_err((char_u *)"mousemodel",
						    0L, (char_u *)"extend", 0);
	set_option_मूल्य_give_err((char_u *)"keymodel", 0L, (char_u *)"", 0);
    }
    अन्यथा
	semsg(_(e_invalid_argument_str), eap->arg);
}

स्थिर पूर्णांक filetype_detect = FALSE;
स्थिर पूर्णांक filetype_plugin = FALSE;
स्थिर पूर्णांक filetype_indent = FALSE;

/*
 * ":filetype [plugin] [indent] {on,off,detect}"
 * on: Load the filetype.vim file to install autocommands for file types.
 * off: Load the ftoff.vim file to remove all autocommands for file types.
 * plugin on: load filetype.vim and ftplugin.vim
 * plugin off: load ftplugof.vim
 * indent on: load filetype.vim and indent.vim
 * indent off: load indoff.vim
 */
    स्थिर शून्य
ex_filetype(exarg_T *eap)
{
    char_u	*arg = eap->arg;
    पूर्णांक		plugin = FALSE;
    पूर्णांक		indent = FALSE;

    जर (*eap->arg == NUL)
    {
	// Print current status.
	smsg("filetype detection:%s  plugin:%s  indent:%s",
		filetype_detect ? "ON" : "OFF",
		filetype_plugin ? (filetype_detect ? "ON" : "(on)") : "OFF",
		filetype_indent ? (filetype_detect ? "ON" : "(on)") : "OFF");
	परत;
    }

    // Accept "plugin" and "indent" in any order.
    साठी (;;)
    {
	जर (STRNCMP(arg, "plugin", 6) == 0)
	{
	    plugin = TRUE;
	    arg = skipwhite(arg + 6);
	    सुरूठेवा;
	}
	जर (STRNCMP(arg, "indent", 6) == 0)
	{
	    indent = TRUE;
	    arg = skipwhite(arg + 6);
	    सुरूठेवा;
	}
	थांबा;
    }
    जर (STRCMP(arg, "on") == 0 || STRCMP(arg, "detect") == 0)
    {
	जर (*arg == 'o' || !filetype_detect)
	{
	    source_runtime((char_u *)FILETYPE_FILE, DIP_ALL);
	    filetype_detect = TRUE;
	    जर (plugin)
	    {
		source_runtime((char_u *)FTPLUGIN_FILE, DIP_ALL);
		filetype_plugin = TRUE;
	    }
	    जर (indent)
	    {
		source_runtime((char_u *)INDENT_FILE, DIP_ALL);
		filetype_indent = TRUE;
	    }
	}
	जर (*arg == 'd')
	{
	    (शून्य)do_doautocmd((char_u *)"filetypedetect BufRead", TRUE, रिक्त);
	    do_modelines(0);
	}
    }
    अन्यथा जर (STRCMP(arg, "off") == 0)
    {
	जर (plugin || indent)
	{
	    जर (plugin)
	    {
		source_runtime((char_u *)FTPLUGOF_FILE, DIP_ALL);
		filetype_plugin = FALSE;
	    }
	    जर (indent)
	    {
		source_runtime((char_u *)INDOFF_FILE, DIP_ALL);
		filetype_indent = FALSE;
	    }
	}
	अन्यथा
	{
	    source_runtime((char_u *)FTOFF_FILE, DIP_ALL);
	    filetype_detect = FALSE;
	}
    }
    अन्यथा
	semsg(_(e_invalid_argument_str), arg);
}

/*
 * ":setfiletype [FALLBACK] {name}"
 */
    स्थिर शून्य
ex_setfiletype(exarg_T *eap)
{
    जर (curbuf->b_did_filetype)
	परत;

    char_u *arg = eap->arg;
    जर (STRNCMP(arg, "FALLBACK ", 9) == 0)
	arg += 9;

    set_option_मूल्य_give_err((char_u *)"filetype", 0L, arg, OPT_LOCAL);
    जर (arg != eap->arg)
	curbuf->b_did_filetype = false;
}

    स्थिर शून्य
ex_digraphs(exarg_T *eap UNUSED)
{
#ifdef FEAT_DIGRAPHS
    जर (*eap->arg != NUL)
	putdigraph(eap->arg);
    अन्यथा
	listdigraphs(eap->forceit);
#अन्यथा
    emsg(_(e_no_digraphs_version));
#endif
}

#जर defined(FEAT_शोध_EXTRA)
    शून्य
set_no_hlsearch(पूर्णांक flag)
{
    no_hlsearch = flag;
# ifdef FEAT_EVAL
    set_vim_var_nr(VV_HLSEARCH, !no_hlsearch && p_hls);
# endif
}

/*
 * ":nohlsearch"
 */
    स्थिर शून्य
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
    स्थिर शून्य
ex_X(exarg_T *eap UNUSED)
{
    crypt_check_current_method();
    (शून्य)crypt_get_किल्ली(TRUE, TRUE);
}
#endif

#ifdef FEAT_FOLDING
    स्थिर शून्य
ex_fold(exarg_T *eap)
{
    जर (foldManualAllowed(TRUE))
	foldCreate(eap->line1, eap->line2);
}

    स्थिर शून्य
ex_foldopen(exarg_T *eap)
{
    opFoldRange(eap->line1, eap->line2, eap->cmdidx == CMD_foldopen,
							 eap->forceit, FALSE);
}

    स्थिर शून्य
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
    साठी (lnum = eap->line1; lnum <= eap->line2; ++lnum)
	जर (hasFolding(lnum, रिक्त, रिक्त) == (eap->cmdidx == CMD_folddoclosed))
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

#जर defined(FEAT_QUICKFIX)
/*
 * Returns TRUE if the supplied Ex cmdidx is for a location list command
 * instead of a quickfix command.
 */
    पूर्णांक
is_loclist_cmd(पूर्णांक cmdidx)
{
    जर (cmdidx < 0 || cmdidx >= CMD_SIZE)
	परत FALSE;
    परत cmdnames[cmdidx].cmd_name[0] == 'l';
}
#endif

    पूर्णांक
get_pressedreturn(शून्य)
{
    परत ex_pressedreturn;
}

    शून्य
set_pressedreturn(पूर्णांक val)
{
    ex_pressedreturn = val;
}
